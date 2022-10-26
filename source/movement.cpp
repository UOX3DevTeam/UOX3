//o------------------------------------------------------------------------------------------------o
//|	File: walking.cpp
//o------------------------------------------------------------------------------------------------o
//|	This File is part of UOX3
//|	Ultima Offline eXperiment III
//|	UO Server Emulation Program
//|
//| Copyright 1998 - 2021 by UOX3 contributors
//|	Copyright 1997 - 2001 by Marcus Rating (Cironian)
//|
//|	This program is free software; you can redistribute it and/or modify
//|	it under the terms of the GNU General Public License as published by
//|	the Free Software Foundation; either version 2 of the License, or
//|	(at your option) any later version.
//|
//|	This program is distributed in the hope that it will be useful,
//|	but WITHOUT ANY WARRANTY; without even the implied warranty of
//|	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//|	GNU General Public License for more details.
//|
//|	You should have received a copy of the GNU General Public License
//|	along with this program; if not, write to the Free Software
//|	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//|
//o------------------------------------------------------------------------------------------------o
//|
//| TODO:
//| Modify path struct to use only directions, not store x,y
//| Add running support in PathFind
//| Modify flying
//|
//| An explination of each function precedes it in the code. Hope I can provide ample enough
//| information to help out the future bug fixers. I will try to explain EVERY bit of logic
//| behind what I've done to make things easier.
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "movement.h"
#include "weight.h"
#include "combat.h"
#include "msgboard.h"
#include "cRaces.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "regions.h"
#include "cServerDefinitions.h"
#include "cWeather.hpp"
#include "Dictionary.h"
#include "StringUtility.hpp"
#include "EventTimer.hpp"

#include <algorithm>

CMovement *Movement;

// These are defines that I'll use. I have a history of working with properties, so that's why
// I'm using custom definitions here versus what may be defined in the other includes.
// NOTE: P = Property, P_C = Property for Characters, P_PF = Property for Pathfinding
//       P_M = Property for Movement

// These are the debugging defines

// These are the errors that we should want to see. RARELY results in any console spam unless
// someone is trying to use an exploit or is sending invalid data directly to the server.

#define DEBUG_WALKING		0
#define DEBUG_NPCWALK		0
#define DEBUG_PATHFIND		0

#undef DBGFILE
#define DBGFILE "movement.cpp"

#define XYMAX					256		// Maximum items UOX can handle on one X/Y square

inline UI08 TurnClockWise( UI08 dir )
{
	UI08 t = (( dir & 0x07 ) + 1 ) % 8;
	return static_cast<UI08>( dir & 0x80 ) ? ( t | 0x80 ) : t;
}

inline UI08 TurnCounterClockWise( UI08 dir )
{
	UI08 t = ( dir - 1 ) & 7;
	return static_cast<UI08>( dir & 0x80 ) ? ( t | 0x80 ) : t;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CalcTileHeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Out of some strange reason the tile height seems to be an absolute value of a
//|					two's complement of the last four bit. Don't know if it has a special meaning
//|					if the tile is height is "negative" (stairs in despise blocking bug)
//|
//|	Arguments	-	SI08 h   orignial height as saved in mul file
//|
//|	Returns		- The absoulte value of the two's complement if the value was "negative"
//o------------------------------------------------------------------------------------------------o
inline SI08 CalcTileHeight( SI08 h )
{
	return static_cast<SI08>(( h & 0x8 ) ? (( h & 0xF ) >> 1 ) : h & 0xF );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleTeleporters()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If character is on a teleport location, teleport him
//o------------------------------------------------------------------------------------------------o
auto HandleTeleporters( CChar *s ) -> void
{
	if( !ValidateObject( s ))
		return;

	UI08 charWorld = s->WorldNumber();
	CTeleLocationEntry *getTeleLoc = nullptr;
	bool isOnTeleporter;
	for( size_t i = 0; i < cwmWorldState->teleLocs.size(); ++i )
	{
		isOnTeleporter	= false;
		getTeleLoc		= &cwmWorldState->teleLocs[i];
		if( getTeleLoc )
		{
			if(( getTeleLoc->SourceWorld() == 0xFF && charWorld <= 1 ) || getTeleLoc->SourceWorld() == charWorld )
			{
				// Check if character is on the teleporter location
				if( getTeleLoc->SourceLocation().z != ILLEGAL_Z )
				{
					isOnTeleporter = ( getTeleLoc->SourceLocation() == s->GetLocation() );
				}
				else
				{
					isOnTeleporter = ( getTeleLoc->SourceLocation().x == s->GetX() && getTeleLoc->SourceLocation().y == s->GetY() );
				}

				if( isOnTeleporter )
				{
					// If they are, whisk them away to the target destination
					UI08 targetWorld = 0;
					if( getTeleLoc->SourceWorld() == 0xFF )
					{
						targetWorld = s->WorldNumber();
					}
					else
					{
						targetWorld = getTeleLoc->TargetWorld();
					}
					
					s->SetLocation( static_cast<SI16>( getTeleLoc->TargetLocation().x ), static_cast<SI16>( getTeleLoc->TargetLocation().y ), static_cast<UI08>( getTeleLoc->TargetLocation().z ), targetWorld, s->GetInstanceId() );
					if( !s->IsNpc() )
					{
						if( targetWorld != charWorld )
						{
							SendMapChange( getTeleLoc->TargetWorld(), s->GetSocket() );
						}
						
						// Teleport player's pets
						auto myPets = s->GetPetList();
						for( const auto &myPet : myPets->collection() )
						{
							if( ValidateObject( myPet ))
							{
								if( !myPet->GetMounted() && myPet->IsNpc() && myPet->GetOwnerObj() == s )
								{
									if( ObjInOldRange( s, myPet, DIST_SAMESCREEN ))
									{
										myPet->SetLocation( s );
									}
								}
							}
						}
					}
					break;
				}
				else if( s->GetX() < getTeleLoc->SourceLocation().x )
				{
					break;
				}
			}
		}
	}
}

void CheckRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::Walking()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles both player requests for walking and NPCs walking
//|
//|	Notes		-	This function is called whenever we get a message from the client
//|					to walk/run somewhere. It is also called by the NPC movement functions in this
//|					class to make the NPCs move. The arguments are fairly fixed because we don't
//|					have a lot of control about what the client gives us.
//|
//|					CChar c - Obviously the character index of the character trying to move.
//|
//|					dir - Which direction the character is trying to move in. The first nibble holds
//|					the cardinal direction. If the bit 0x80 is set, it means the character is
//|					running instead of walking.
//|						0: // North
//|						1: // Northeast
//|						2: // East
//|						3: // Southeast
//|						4: // South
//|						5: // Southwest
//|						6: // West
//|						7: // Northwest
//|
//|					sequence - This is what point in the walking sequence we are at, this seems to
//|					roll over once it hits 256
//|
//|					Sometimes the NPC movement code comes up with -1, for example, if we are
//|					following someone and we are directly on top of them.
//|					^ That's because chardir returns -1 when direction is the same.  Actually, this
//|					should only happen when a player walks on top of an NPC and the NPC tries to move.
//o------------------------------------------------------------------------------------------------o
void CMovement::Walking( CSocket *mSock, CChar *c, UI08 dir, SI16 sequence )
{
	if( !IsValidDirection( dir ))
	{
		std::string charName = GetNpcDictName( c );
		Console.Error( oldstrutil::format( "%s (CMovement::Walking) caught bad direction = %s %d 0x%x\n", DBGFILE, charName.c_str(), dir, dir ));
		// If I don't do this, the NPC will keep trying to walk on the same step, which is
		// where he's already at. Can cause an infinite loop. (Trust me, was one of the things
		// that locked up NW Alpha 2)
		// Will call something like NPCRandomMove in the future.
		if( c->IsNpc() )
		{
			c->FlushPath();
		}
		return;
	}

	if( !VerifySequence( c, mSock, sequence ))
		return;

	// If checking for weight is more expensive, shouldn't we check for frozen first?
	if( IsFrozen( c, mSock, sequence ))
		return;

	if( IsOverloaded( c, mSock, sequence ))
		return;

	// save our original location before we even think about moving
	const SI16 oldx = c->GetX();
	const SI16 oldy = c->GetY();

	// see if we have stopped to turn or if we are moving
	const bool amTurning = (( dir & 0x07 ) != c->GetDir() );
	if( !amTurning )
	{
		if( !CheckForRunning( c, dir ))
			return;

		if( !CheckForStealth( c ))
			return;

		SI08 myz = ILLEGAL_Z;
		const SI16 myx = GetXfromDir( dir, oldx );
		const SI16 myy = GetYfromDir( dir, oldy );
		if( !CalcMove( c, oldx, oldy, myz, dir ))
		{
#if DEBUG_WALKING
			std::string charName = GetNpcDictName( c );
			Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) Character Walk Failed for %s\n", DBGFILE, charName ));
			Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, oldx, oldy, c->GetZ() ));
			Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz ));
#endif
			if( mSock != nullptr )
			{
				DenyMovement( mSock, c, sequence );
			}
			if( c->IsNpc() )
			{
				c->FlushPath();

				//The following bits are to stop NPCs from constantly pathfinding
				//if the server thinks the paths they're finding are valid, but they
				//can't actually move there!
				const SI16 fx1 = c->GetFx( 0 );
				const SI16 fx2 = c->GetFx( 1 );
				const SI16 fy1 = c->GetFy( 0 );
				const SI16 fy2 = c->GetFy( 1 );

				c->SetPathFail( c->GetPathFail() + 1 );
				if( c->GetPathFail() > 10 )
				{
					UI16 fx2Actual = 0;
					UI16 fy2Actual = 0;
					SI08 npcWanderType = c->GetNpcWander();
					if( npcWanderType == WT_FLEE )
					{
						// If NPC fails to flee, make him re-attack instead!
						c->SetFleeAt( 0 );
						c->SetReattackAt( 1 );
						c->SetNpcWander( c->GetOldNpcWander() );
						if( c->GetMounted() )
						{
							c->SetTimer( tNPC_MOVETIME, BuildTimeValue( c->GetMountedRunningSpeed() ));
						}
						else
						{
							c->SetTimer( tNPC_MOVETIME, BuildTimeValue( c->GetRunningSpeed() ));
						}
						c->SetOldNpcWander( WT_NONE ); // so it won't save this at the wsc file
					}
					else if( npcWanderType == WT_FOLLOW )
					{
						// If NPC following fails to follow, make it stop
						c->SetOldTargLocX( 0 );
						c->SetOldTargLocY( 0 );
						c->SetNpcWander( WT_NONE );
						c->TextMessage( nullptr, "[Stops following]", SYSTEM, false );
					}
					else if( npcWanderType == WT_BOX )
					{
						fx2Actual = fx2;
						fy2Actual = fy2;
						BoundingBoxTeleport( c, fx2Actual, fy2Actual, oldx, oldy );
					}
					else if( npcWanderType == WT_CIRCLE )
					{
						fx2Actual = fx1 + fx2;
						fy2Actual = fy1 + fx2;
						BoundingBoxTeleport( c, fx2Actual, fy2Actual, oldx, oldy );
					}
					else
					{
						c->SetNpcWander( WT_FREE );
					}
					c->SetPathFail( 0 );
				}
			}
			return;
		}
#if DEBUG_WALKING
		std::string charName = GetNpcDictName( c );
		Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) Character Walk Passed for %s\n", DBGFILE, charName ));
		Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, oldx, oldy, c->GetZ() ));
		Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz ));
#endif

		if( c->IsNpc() && CheckForCharacterAtXYZ( c, myx, myy, myz ))
		{
			c->SetPathFail( c->GetPathFail() + 1 );

			if( c->GetPathFail() < 3 && !c->IsEvading() ) // Try again up to 3 times
			{
				c->FlushPath();

				// If advanced pathfinding is enabled, calculate a new path!
				if( cwmWorldState->ServerData()->AdvancedPathfinding() )
				{
					bool newPath = false;
					newPath = AdvancedPathfinding( c, c->GetPathTargX(), c->GetPathTargY(), ( c->GetRunning() > 0 ));
					if( !newPath )
					{
						c->SetPathResult( 0 ); // partial success
					}
#if DEBUG_WALKING
					std::string charName = GetNpcDictName( c );
					Console.Print( oldstrutil::format( "DEBUG: Walking() - NPC (%s) failed to pathfind (%d times). Calculating new path!\n", charName.c_str(), c->GetPathFail() ));
#endif
				}
				return;
			}
			else if( c->GetPathFail() < 10 && !c->IsEvading() )
			{
#if DEBUG_WALKING
				std::string charName = GetNpcDictName( c );
				Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) Character Walk Passed for %s\n", DBGFILE, charName ));
				Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, oldx, oldy, c->GetZ() ));
				Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz ));
				Console.Print( oldstrutil::format( "DEBUG: Walking() - NPC (%s) failed to pathfind (%d times, but less than 10). Invalidating old target location!\n", charName.c_str(), c->GetPathFail() ));
#endif

				c->FlushPath();
				c->SetPathResult( 0 ); // partial success, but blocked by character

				// Forget it, find a new target location for pathfinding
				c->SetOldTargLocX( 0 );
				c->SetOldTargLocY( 0 );

				// Temporary change NPC's wandermode to none, and try pathfinding again in 5 seconds
				if( c->GetNpcWander() != WT_NONE )
				{
					c->SetOldNpcWander( c->GetNpcWander() );
				}
				c->SetNpcWander( WT_NONE );
				c->SetTimer( tNPC_MOVETIME, BuildTimeValue( 5 ));
				return;
			}
			else
			{
#if DEBUG_WALKING
				std::string charName = GetNpcDictName( c );
				Console.Print( oldstrutil::format( "DEBUG: Walking() - NPC (%s) failed to pathfind %d times! Pausing pathfinding for some time.\n", charName.c_str(), c->GetPathFail() ));
#endif
				c->FlushPath();
				c->SetPathResult( 0 ); // partial success, but blocked by character
				c->SetPathFail( 0 );

				// Pause attempts to move if pathfind keeps failing
				// Wandermode will be restored in HandleNPCWander()
				auto npcWanderType = c->GetNpcWander();
				if( npcWanderType == WT_BOX || npcWanderType == WT_CIRCLE || npcWanderType == WT_FREE )
				{
					c->SetOldNpcWander( npcWanderType );
					c->SetNpcWander( WT_NONE );
					c->SetTimer( tNPC_MOVETIME, BuildTimeValue( 30 ));
				}
				return;
			}
		}

		if( cwmWorldState->ServerData()->AmbientFootsteps() )
		{
			Effects->PlayTileSound( c, mSock );
		}

		MoveCharForDirection( c, myx, myy, myz );
		c->SetPathFail( 0 );

		// i actually moved this for now after the z =  illegal_z, in the end of CrazyXYBlockStuff()
		// can't see how that would hurt anything
		if( !CheckForHouseBan( c, mSock ))
			return;

		//
		//	 OK AT THIS POINT IT IS NOW OFFICIALLY A LEGAL MOVE TO MAKE, LETS GO FOR IT!
		//
		//	 That means any bugs concerning if a move was legal must be before this point!
		//
	}

	// do all of the following regardless of whether turning or moving i guess

	// set the player direction to contain only the cardinal direction bits
	c->WalkDir(( dir & 0x07 ));

	SendWalkToPlayer( c, mSock, sequence );
	SendWalkToOtherPlayers( c, dir, oldx, oldy );

	// Update timestamp for when character last moved
	c->LastMoveTime( cwmWorldState->GetUICurrentTime() );

	// i'm going ahead and optimizing this, if you haven't really moved, should be
	// no need to check for teleporters and the weather shouldn't change
	if( !amTurning )
	{
		OutputShoveMessage( c, mSock );

		HandleItemCollision( c, mSock, oldx, oldy );
		HandleTeleporters( c );
		CheckCharInsideBuilding( c, mSock, true);
		CheckRegion( mSock, ( *c ), false );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::IsValidDirection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a given direction is valid
//|	Notes		-	Return true on the below values:
//|						Direction   Walking Value   Running Value
//| 					North          0 0x00         128 0x80
//|						Northeast      1 0x01         129 0x81
//|						East           2 0x02         130 0x82
//|						Southeast      3 0x03         131 0x83
//|						South          4 0x04         132 0x84
//|						Southwest      5 0x05         133 0x85
//|						West           6 0x06         134 0x86
//|						Northwest      7 0x07         135 0x87
//o------------------------------------------------------------------------------------------------o
bool CMovement::IsValidDirection( UI08 dir )
{
	return ( dir == ( dir & 0x87 ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::IsFrozen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a character is frozen or casting a spell
//|	Notes		-	Because of the way the source uses the frozen flag, I decided to change
//|					something in how this works. If the character is casting a spell (chars.casting is true)
//|					OR if they're frozen (chars.priv2 & FROZEN_BIT) then they can't walk. Why? If a player/npc
//|					has their frozen bit set, and they cast a spell, they will lose their frozen bit at the
//|					end of the spell cast. With this new check, we don't even need to set the frozen bit when
//|					casting a spell!
//o------------------------------------------------------------------------------------------------o
bool CMovement::IsFrozen( CChar *c, CSocket *mSock, SI16 sequence )
{
	if( c->IsCasting() && !cwmWorldState->ServerData()->CastSpellsWhileMoving() )
	{
		if( mSock != nullptr )
		{
			mSock->SysMessage( 1380 ); // You cannot move while casting.
			DenyMovement( mSock, c, sequence );
		}
#if DEBUG_WALKING
		std::string charName = GetNpcDictName( c );
		Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::IsFrozen) casting char %s\n", DBGFILE, charName ));
#endif
		return true;
	}
	if( c->IsFrozen() )
	{
		if( mSock != nullptr )
		{
			mSock->SysMessage( 1381 ); // You are frozen and cannot move.
			DenyMovement( mSock, c, sequence );
		}
#if DEBUG_WALKING
		std::string charName = GetNpcDictName( c );
		Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::IsFrozen) frozen char %s\n", DBGFILE, charName ));
#endif
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::IsOverloaded()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character is overloaded with weight, and returns true or false
//|	Notes		-	CheckForWeight was confusing...
//|
//|					Old code called check weight first then checked socket... I changed it the other way.
//|					Why, well odds are (I may be wrong) if you don't have a socket, you're an NPC and if you
//|					have one, you're a character. We said in the first line that we didn't want to restrict
//|					based upon NPC, so if you're an NPC, the socket/checkweight will never be called anyway.
//|
//|					Rewrote to deny the client... We'll see if it works.
//o------------------------------------------------------------------------------------------------o
bool CMovement::IsOverloaded( CChar *c, CSocket *mSock, SI16 sequence )
{
	// Who are we going to check for weight restrictions?
	if( !c->IsDead() && !c->IsNpc() && c->GetCommandLevel() < CL_CNS )
	{
		if( mSock != nullptr )
		{
			if( Weight->IsOverloaded( c ))
			{
				c->IncStamina( -5 );
			}
			if( c->GetStamina() <= 0 )
			{
				c->SetStamina( 0 );
				mSock->SysMessage( 1783 ); // You are too fatigued to move, because you are carrying too much weight!
				DenyMovement( mSock, c, sequence );
#if DEBUG_WALKING
				std::string charName = GetNpcDictName( c );
				Console.Print( oldstrutil::format( "DEBUG: %s (CMovement::Walking) overloaded char %s\n", DBGFILE, charName ));
#endif
				return true;
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CheckForCharacterAtXYZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Examins a location (cx, cy, cz) to determine if a character is found there.
//|					If so, return true. If not, return false. Used for the npc stacking problem
//|	Notes		-	Here's how I'm going to use it for now.  Movement Type may be used for races, that's why
//|					I put it as an integer.  Here are the values I'm going to use:
//|					BTW, bird doesn't work right now! These values should be scripted, as well, but I want
//|					to test walking before another var is added to chars struct.
//|						GM Body  0x01
//|						Player   0x02
//|						Bird     0x20 (basically, a fish and an NPC, so I could use 0xc0, but I don't wanna)
//|						NPC      0x40
//|						Fish     0x80 (So they can swim!)
//|					I left a gap between Player and NPC because someone may want to implement race
//|					restrictions...
//o------------------------------------------------------------------------------------------------o
auto CMovement::CheckForCharacterAtXYZ( CChar *c, SI16 cx, SI16 cy, SI08 cz ) -> bool
{
	CMapRegion *MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( cx ), MapRegion->GetGridY( cy ), c->WorldNumber() );	// check 3 cols... do we really NEED to?
	if( MapArea ) // no valid region
	{
		auto regChars = MapArea->GetCharList();
		for( const auto &tempChar : regChars->collection() )
		{
			if( ValidateObject( tempChar ) && tempChar->GetInstanceId() == c->GetInstanceId() )
			{
				if( tempChar != c && (( IsOnline(( *tempChar )) && !tempChar->IsDead() ) || tempChar->IsNpc() )) // x=x,y=y, and distance btw z's <= MAX STEP
				{
					if( tempChar->GetX() == cx && tempChar->GetY() == cy && tempChar->GetZ() >= cz && tempChar->GetZ() <= ( cz + 5 ))
					{
						// 2 people will still bump into each other, if slightly offset
						return true;
					}
				}
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::VerifySequence()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Verifies if walk sequence is in sync, and if not, denies movement
//o------------------------------------------------------------------------------------------------o
bool CMovement::VerifySequence( CChar *c, CSocket *mSock, SI16 sequence )
{
	if( mSock != nullptr )
	{
		if( mSock->WalkSequence() + 1 != sequence && sequence != 256 )
		{
			DenyMovement( mSock, c, sequence );
			return false;
		}
		else if( mSock->WalkSequence() == sequence )
		{
			DenyMovement( mSock, c, sequence );
			return false;
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CheckForRunning()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character is running, and updates character accordingly
//|	Notes		-	returns true if updatechar required, or false if not
//o------------------------------------------------------------------------------------------------o
bool CMovement::CheckForRunning( CChar *c, UI08 dir )
{
	// if we are running
	if( dir & 0x80 )
	{
		// if we are using stealth
		if( c->GetStealth() != -1 )	// Stealth - stop hiding if player runs
		{
			c->ExposeToView();
		}
		//Don't regenerate stamina while running
		c->SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_STAMINAREGEN ), 1 );
		c->SetRunning( c->GetRunning() + 1 );

		if( !c->IsDead() && c->GetCommandLevel() < CL_CNS )
		{
			bool reduceStamina = (( c->IsFlying() || c->IsOnHorse() ) && c->GetRunning() > ( cwmWorldState->ServerData()->MaxStaminaMovement() * 2 ));
			if( !reduceStamina )
			{
				reduceStamina = ( c->GetRunning() > ( cwmWorldState->ServerData()->MaxStaminaMovement() * 4 ));
			}
			if( reduceStamina )
			{
				c->SetRunning( 1 );
				c->SetStamina( c->GetStamina() - 1 );
			}
		}
	}
	else
	{
		c->SetRunning( 0 );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CheckForStealth()
//|	Date		-	09/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check to see if character is in stealth mode.
//|
//|	Changes		-	09/22/2002	-	Unhide characters who are mounted while trying to stealth.
//o------------------------------------------------------------------------------------------------o
bool CMovement::CheckForStealth( CChar *c )
{
	if( c->GetVisible() == VT_TEMPHIDDEN || c->GetVisible() == VT_INVISIBLE )
	{
		if( c->IsOnHorse() || c->IsFlying() ) // Consider Gargoyle flying as mounted for this context
		{
			if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
			{
				c->ExposeToView();
			}
		}

		if( c->GetStealth() != -1 )
		{
			c->SetStealth( c->GetStealth() + 1 );
			if( c->GetStealth() > ( cwmWorldState->ServerData()->MaxStealthMovement() * c->GetSkill( STEALTH ) / 1000 ))
			{
				c->ExposeToView();
				auto cSock = c->GetSocket();
				if( cSock != nullptr )
				{
					cSock->SysMessage( 6250 ); // You step out of the shadows
				}
			}
		}
		else
		{
			c->ExposeToView();
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CheckForHouseBan()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	See if a player has tried to move into a house they were banned from
//o------------------------------------------------------------------------------------------------o
bool CMovement::CheckForHouseBan( CChar *c, CSocket *mSock )
{
	CMultiObj *house = FindMulti( c );
	if( ValidateObject( house ))
	{
		if( c->GetMultiObj() != house )
		{
			c->SetMulti( house );//Set them inside the multi!
		}
	}
	else
	{
		if( c->GetMultiObj() != nullptr )
		{
			c->SetMulti( INVALIDSERIAL );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	bool MoveCharForDirection( CChar *c, SI16 newX, SI16 newY, SI08 newZ )
//|	Date		-	21/09/2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	I already made sure I could move there (even the crazy XY block stuff) so
//|					this IS a valid move. Just move the directions. Oh, and since I we already
//|					have the GetX/YfromDir functions (and we need those) why don't we just
//|					use them here?
//o------------------------------------------------------------------------------------------------o
void CMovement::MoveCharForDirection( CChar *c, SI16 newX, SI16 newY, SI08 newZ )
{
	if( !c->IsNpc() )
	{
		// if we're a PC in combat, or casting, we want to break/adjust their timers
		const bool casting = ( c->IsCasting() || c->IsJSCasting() );
		if(( c->IsAtWar() || c->GetAttacker() != nullptr || casting ) && IsOnline( *c ))
		{
			// if it's not an NPC, in combat or casting, and it's online
			if( casting && !cwmWorldState->ServerData()->CastSpellsWhileMoving() )
			{
				Effects->PlayStaticAnimation( c, 0x3735, 0, 30 );
				Effects->PlaySound( c, 0x005C );
				c->TextMessage( c->GetSocket(), 771, EMOTE, false ); // The spell fizzles.
				c->StopSpell();
				c->SetJSCasting( false );
			}
		}
	}
	MapRegion->ChangeRegion( c, newX, newY, c->WorldNumber() );
	c->WalkXY( newX, newY );
	c->WalkZ( newZ );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::GetBlockingStatics()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get a list of static items that block character movement
//o------------------------------------------------------------------------------------------------o
void CMovement::GetBlockingStatics( SI16 x, SI16 y, std::vector<Tile_st> &xyblock, UI16 &xycount, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;

	auto artwork = Map->ArtAt( x, y, worldNumber );
	for( auto &art : artwork )
	{
		xyblock.push_back( art );
		++xycount;
		if( xycount >= XYMAX )	// don't overflow
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::GetBlockingDynamics()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get a list of dynamic items that block character movement
//o------------------------------------------------------------------------------------------------o
auto CMovement::GetBlockingDynamics( SI16 x, SI16 y, std::vector<Tile_st> &xyblock, UI16 &xycount, UI08 worldNumber, UI16 instanceId ) -> void
{
	if( xycount >= XYMAX ) // don't overflow
		return;

	for( auto &MapArea : MapRegion->PopulateList( x, y, worldNumber ))
	{
		if( MapArea )
		{
			auto regItems = MapArea->GetItemList();
			for( const auto &tItem : regItems->collection() )
			{
				if( ValidateObject( tItem ) && tItem->GetInstanceId() == instanceId )
				{
					if( !tItem->CanBeObjType( OT_MULTI ))
					{
#if DEBUG_WALKING
						Console.Print( oldstrutil::format( "DEBUG: Item X: %i\nItem Y: %i\n", tItem->GetX(), tItem->GetY() ));
#endif
						if( tItem->GetX() == x && tItem->GetY() == y )
						{
							auto tile = Tile_st( TileType_t::dyn );
							tile.tileId = tItem->GetId();
							tile.altitude = tItem->GetZ();
							tile.artInfo = &Map->SeekTile( tItem->GetId() );
							xyblock.push_back( tile );
							++xycount;
							if( xycount >= XYMAX ) // don't overflow
							{
								return;
							}
						}
					}
					else if( std::abs( tItem->GetX() - x ) <= DIST_BUILDRANGE && std::abs( tItem->GetY() - y ) <= DIST_BUILDRANGE ) // implication, is, this is now a CMultiObj
					{
						const UI16 multiId = ( tItem->GetId() - 0x4000 );
						SI32 length = 0;
						
						if( !Map->MultiExists( multiId ))
						{
							Console.Error( "Walking() - Bad length in multi file. Avoiding stall" );
							auto map1 = Map->SeekMap( tItem->GetX(), tItem->GetY(), tItem->WorldNumber() );

							if( map1.CheckFlag( TF_WET )) // is it water?
							{
								tItem->SetId( 0x4001 );
							}
							else
							{
								tItem->SetId( 0x4064 );
							}
							length = 0;
						}
						else
						{
							for( auto &multi : Map->SeekMulti( multiId ).items )
							{
								if( multi.flag && ( tItem->GetX() + multi.offsetX ) == x && ( tItem->GetY() + multi.offsetY ) == y )
								{
									auto tile = Tile_st( TileType_t::dyn );
									tile.artInfo = &Map->SeekTile( multi.tileId );
									tile.altitude = multi.altitude + tItem->GetZ();
									xyblock.push_back( tile );
									++xycount;
									if( xycount >= XYMAX ) // don't overflow
									{
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::SendWalkToPlayer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Actually send the walk command back to the player and increment the sequence
//o------------------------------------------------------------------------------------------------o
void CMovement::SendWalkToPlayer( CChar *c, CSocket *mSock, SI16 sequence )
{
	if( mSock != nullptr )
	{
		CPWalkOK toSend;

		toSend.SequenceNumber( mSock->GetByte( 2 ));
		if( c->GetVisible() != VT_VISIBLE )
		{
			toSend.FlagColour( 0 );
		}
		else
		{
			toSend.FlagColour( static_cast<UI08>( c->FlagColour( c )));
		}

		mSock->Send( &toSend );
		mSock->WalkSequence( sequence );
		if( mSock->WalkSequence() == 255 )
		{
			mSock->WalkSequence( 1 );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::SendWalkToOtherPlayers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send the character's walk action to other players in range
//o------------------------------------------------------------------------------------------------o
void CMovement::SendWalkToOtherPlayers( CChar *c, UI08 dir, SI16 oldx, SI16 oldy )
{
	// lets cache these vars in advance
	const SI16 newx		= c->GetX();
	const SI16 newy		= c->GetY();

	bool checkX			= ( oldx != newx );
	bool checkY			= ( oldy != newy );
	UI16 effRange;
	UI08 worldnumber	= c->WorldNumber();
	UI16 instanceId		= c->GetInstanceId();

	CPExtMove toSend	= ( *c );

	for( auto &tSend : Network->connClients )
	{
		if( tSend == nullptr )
			continue;

		CChar *mChar = tSend->CurrcharObj();
		if( !ValidateObject( mChar ))
			continue;

		if( worldnumber != mChar->WorldNumber() || instanceId != mChar->GetInstanceId() )
			continue;

		effRange = static_cast<UI16>( tSend->Range() );
		const auto visibleRange = static_cast<UI16>( tSend->Range() + Races->VisRange( mChar->GetRace() ));
		if( visibleRange >= effRange )
		{
			effRange = visibleRange;
		}

		if( c != mChar )
		{
			// We need to ensure they are within range of our X AND Y location regardless of how they moved.
			const auto dxNew = static_cast<UI16>( abs( newx - mChar->GetX() ));
			const auto dyNew = static_cast<UI16>( abs( newy - mChar->GetY() ));
			if( checkX && dyNew <= ( effRange + 2 )) // Only check X Plane if their x changed
			{
				UI16 dxOld = static_cast<UI16>(abs( oldx - mChar->GetX() ));
				if(( dxNew == effRange ) && ( dxOld > effRange ))
				{
					c->SendToSocket( tSend );
					continue;	// Incase they moved diagonally, lets not update the same character multiple times
				}
				else if(( dxNew > ( effRange + 1 )) && ( dxOld == ( effRange + 1 )))
				{
					c->RemoveFromSight( tSend );
					continue;	// Incase they moved diagonally, lets not update the same character multiple times
				}
			}
			if( checkY && dxNew <= ( effRange + 2 )) // Only check Y plane if their y changed
			{
				UI16 dyOld = static_cast<UI16>(abs( oldy - mChar->GetY() ));
				if(( dyNew == effRange ) && ( dyOld > effRange ))
				{
					c->SendToSocket( tSend );
					continue;
				}
				else if(( dyNew > ( effRange + 1 )) && ( dyOld == ( effRange + 1 )))
				{
					c->RemoveFromSight( tSend );
					continue;
				}
			}
			toSend.FlagColour( static_cast<UI08>(c->FlagColour( mChar )) );
			tSend->Send( &toSend );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::OutputShoveMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	See if we should mention that we shove something out of the way
//o------------------------------------------------------------------------------------------------o
auto CMovement::OutputShoveMessage( CChar *c, CSocket *mSock ) -> void
{
	if( mSock == nullptr )
		return;

	// GMs, counselors, and ghosts don't shove things
	if( c->GetCommandLevel() >= CL_CNS || IsGMBody( c ))
		return;

	// lets cache these vars in advance
	auto grid = MapRegion->GetMapRegion( c );
	if( grid == nullptr )
		return;
				
	auto regChars = grid->GetCharList();
	bool didShove			= false;
	const SI16 x			= c->GetX();
	const SI16 y			= c->GetY();
	const SI08 z			= c->GetZ();
	const UI16 instanceId	= c->GetInstanceId();

	std::vector<UI16> scriptTriggers = c->GetScriptTriggers();
	std::vector<UI16> ourScriptTriggers;
	cScript *toExecute;
	for( const auto &ourChar : regChars->collection() )
	{
		if( ValidateObject( ourChar ) && ourChar != c && ourChar->GetInstanceId() == instanceId )
		{
			if( ourChar->GetX() == x && ourChar->GetY() == y && std::abs( ourChar->GetZ() - z ) <= 4 )
			{
				if(( ourChar->GetVisible() != VT_PERMHIDDEN )
				   && ( !IsGMBody( ourChar )
					 && ( ourChar->IsNpc() || IsOnline(( *ourChar ))) && ourChar->GetCommandLevel() < CL_CNS ))
				{
					// Assume character shoved other character
					didShove = true;
					
					// Run onCollide event on character doing the shoving
					for( auto scriptTrig : scriptTriggers )
					{
						toExecute = JSMapping->GetScript( scriptTrig );
						if( toExecute )
						{
							auto retVal = toExecute->OnCollide( mSock, c, ourChar );
							if( retVal == 0 )
							{
								// Event found, script returned false. Disallow hardcoded functionality, but keep checking
								didShove = false;
							}
							else if( retVal == 1 )
							{
								// Event found, script returned true. Allow hardcoded functionality
								didShove = true;
								break;
							}
						}
					}

					// Run onCollide event on character being shoved
					ourScriptTriggers.clear();
					ourScriptTriggers.shrink_to_fit();
					ourScriptTriggers = ourChar->GetScriptTriggers();
					for( auto scriptTrig : ourScriptTriggers )
					{
						toExecute = JSMapping->GetScript( scriptTrig );
						if( toExecute )
						{
							auto retVal = toExecute->OnCollide( ourChar->GetSocket(), c, ourChar );
							if( retVal == 0 )
							{
								// Event found, script returned false. Disallow hardcoded functionality, but keep checking
								didShove = false;
							}
							else if( retVal == 1 )
							{
								// Event found, script returned true. Allow hardcoded functionality
								didShove = true;
								break;
							}
						}
					}
					
					if( ourChar->GetVisible() == VT_TEMPHIDDEN || ourChar->GetVisible() == VT_INVISIBLE )
					{
						mSock->SysMessage( 1384 ); // Being perfectly rested, you shove something invisible out of the way.
					}
					else
					{
						std::string charName = GetNpcDictName( ourChar );
						mSock->SysMessage( 1383, charName.c_str() ); // Being perfectly rested, you shove %s out of the way.
					}
				}
			}
		}
	}

	if( didShove )
	{
		c->SetStamina( std::max( c->GetStamina() - 4, 0 ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoJSInRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Trigger InRange JS event
//o------------------------------------------------------------------------------------------------o
void DoJSInRange( CBaseObject *mObj, CBaseObject *objInRange )
{
	std::vector<UI16> scriptTriggers = mObj->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			toExecute->InRange( mObj, objInRange );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoJSOutOfRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Trigger OutOfRange JS event
//o------------------------------------------------------------------------------------------------o
void DoJSOutOfRange( CBaseObject *mObj, CBaseObject *objOutOfRange )
{
	std::vector<UI16> scriptTriggers = mObj->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			toExecute->OutOfRange( mObj, objOutOfRange );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UpdateItemsOnPlane()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Update items that come within range or go out of range of the player
//o------------------------------------------------------------------------------------------------o
bool UpdateItemsOnPlane( CSocket *mSock, CChar *mChar, CItem *tItem, UI16 id, UI16 dNew, UI16 dOld, UI16 visibleRange, bool isGM )
{
	if( isGM || tItem->GetVisible() == VT_VISIBLE || ( tItem->GetVisible() == VT_TEMPHIDDEN && tItem->GetOwnerObj() == mChar ))
	{
		if( mSock != nullptr && (( id >= 0x407A && id <= 0x407F) || id == 0x5388 ))
		{
			if( dNew == DIST_BUILDRANGE && dOld > DIST_BUILDRANGE )	// It's a large building
			{
				tItem->SendToSocket( mSock );
				return true;
			}
			else if( dOld == DIST_BUILDRANGE && dNew > DIST_BUILDRANGE )
			{
				tItem->RemoveFromSight( mSock );
				return true;
			}
		}
		else if( dNew == visibleRange && dOld > visibleRange )	// Just came into range
		{
			if( mSock != nullptr )
			{
				tItem->SendToSocket( mSock );
			}
			DoJSInRange( mChar, tItem );
			DoJSInRange( tItem, mChar );
			return true;
		}
		else if( dOld == ( visibleRange + 1 ) && dNew > ( visibleRange + 1 )) // Just went out of range
		{
#pragma note( "Is it necessary to send packets with item removal when they go out of range, or does client handle this itself?" )
			if( mSock != nullptr )
			{
				tItem->RemoveFromSight( mSock );
			}
			DoJSOutOfRange( mChar, tItem );
			DoJSOutOfRange( tItem, mChar );
			return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	UpdateCharsOnPlane()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Update characters that come within range or go out of range of the player
//o------------------------------------------------------------------------------------------------o
bool UpdateCharsOnPlane( CSocket *mSock, CChar *mChar, CChar *tChar, UI16 dNew, UI16 dOld, UI16 visibleRange )
{
	if( dNew == visibleRange && dOld > visibleRange ) // Just came into range
	{
		if( mSock != nullptr )
		{
			tChar->SendToSocket( mSock );
		}
		DoJSInRange( mChar, tChar );
		DoJSInRange( tChar, mChar );
		return true;
	}
	if( dOld == ( visibleRange + 1 ) && dNew > ( visibleRange + 1 )) // Just went out of range
	{
		if( mSock != nullptr )
		{
			tChar->RemoveFromSight( mSock );
		}
		DoJSOutOfRange( mChar, tChar );
		DoJSOutOfRange( tChar, mChar );
		return true;
	}
	return false;
}

void MonsterGate( CChar *s, const std::string& scriptEntry );
void AdvanceObj( CChar *s, UI16 x, bool multiUse );
void SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate );
bool HandleDoubleClickTypes( CSocket *mSock, CChar *mChar, CItem *iUsed, ItemTypes iType );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleObjectCollisions()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle character collision with items of various item types
//o------------------------------------------------------------------------------------------------o
void HandleObjectCollisions( CSocket *mSock, CChar *mChar, CItem *itemCheck, ItemTypes type )
{
	switch( type )
	{
		case IT_OBJTELEPORTER: // teleporters
			if( itemCheck->GetTempVar( CITV_MOREX ) + itemCheck->GetTempVar( CITV_MOREY ) + itemCheck->GetTempVar( CITV_MOREZ ) > 0 )
			{
				mChar->SetLocation( static_cast<UI16>( itemCheck->GetTempVar( CITV_MOREX )), static_cast<UI16>( itemCheck->GetTempVar( CITV_MOREY )), static_cast<SI08>( itemCheck->GetTempVar( CITV_MOREZ )));
			}
			break;
		case IT_ADVANCEGATE: // advancement gates
		case IT_MULTIADVANCEGATE:
			if( !mChar->IsNpc() )
			{
				AdvanceObj( mChar, static_cast<UI16>( itemCheck->GetTempVar( CITV_MOREX )), ( itemCheck->GetType() == IT_MULTIADVANCEGATE ));
			}
			break;
		case IT_MONSTERGATE: // monster gates
			MonsterGate( mChar, itemCheck->GetDesc() );
			break;
		case IT_RACEGATE: // race gates
			Races->ApplyRace( mChar, static_cast<RACEID>( itemCheck->GetTempVar( CITV_MOREX )), itemCheck->GetTempVar( CITV_MOREY ) != 0 );
			break;
		case IT_DAMAGEOBJECT: // damage objects
			if( !mChar->IsInvulnerable() )
			{
				[[maybe_unused]] bool retVal = mChar->Damage( itemCheck->GetTempVar( CITV_MOREX ) + RandomNum( itemCheck->GetTempVar( CITV_MOREY ), itemCheck->GetTempVar( CITV_MOREZ )), PHYSICAL, nullptr );
			}
			break;
		case IT_SOUNDOBJECT: // sound objects
			if( static_cast<UI32>( RandomNum( 1, 100 )) <= itemCheck->GetTempVar( CITV_MOREZ ))
			{
				Effects->PlaySound( itemCheck, static_cast<UI16>( itemCheck->GetTempVar( CITV_MOREX )));
			}
			break;
		case IT_MAPCHANGEOBJECT:
			SocketMapChange( mSock, mChar, itemCheck );
			break;
		case IT_ZEROKILLSGATE: // zero kill gate
			mChar->SetKills( 0 );
			break;
		case IT_WORLDCHANGEGATE:
			if( !mChar->IsNpc() ) // world change gate
			{
				if( mSock != nullptr )
				{
					CPWorldChange wrldChange( static_cast<WorldType>( itemCheck->GetTempVar( CITV_MOREX )), 1 );
					mSock->Send( &wrldChange );
				}
			}
			break;
		case IT_PLANK:
		{
			if( mSock == nullptr )
				break;

			// Only react when players step on open planks
			auto plankId = itemCheck->GetId();
			if( plankId == 0x3e84 || plankId == 0x3ed5 || plankId == 0x3ed4 || plankId == 0x3e89 )
			{
				// Find multi plank belongs to
				auto iMulti = itemCheck->GetMultiObj();
				if( ValidateObject( iMulti ))
				{
					// Check if player was on a multi in their previous step
					auto oldLoc = mChar->GetOldLocation();
					auto oldMulti = FindMulti( oldLoc.x, oldLoc.y, oldLoc.z, mChar->WorldNumber(), mChar->GetInstanceId() );

					if( ValidateObject( oldMulti ) && oldMulti == iMulti )
					{
						auto nextX = mChar->GetX() + ( mChar->GetX() - oldLoc.x );
						auto nextY = mChar->GetY() + ( mChar->GetY() - oldLoc.y );

						if( Movement->CalcWalk( mChar, nextX, nextY, oldLoc.x, oldLoc.y, mChar->GetZ(), false ) == ILLEGAL_Z )
						{
							// Player is moving from boat > open plank, but no place to go from plank
							// Let's teleport them to nearest valid location off the boat
							HandleDoubleClickTypes( mSock, mChar, itemCheck, IT_PLANK );
						}
					}
				}
			}
			break;
		}
		default:
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::HandleItemCollision()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle item collisions, make items that appear on the edge of our sight because
//|					visible, buildings when they get in range, and if the character steps on something
//|					that might cause damage
//o------------------------------------------------------------------------------------------------o
void CMovement::HandleItemCollision( CChar *mChar, CSocket *mSock, SI16 oldx, SI16 oldy )
{
	// lets cache these vars in advance
	UI16 visibleRange = static_cast<UI16>( MAX_VISRANGE + Races->VisRange( mChar->GetRace() ));
	if( mSock != nullptr )
	{
		visibleRange = static_cast<UI16>( mSock->Range() + Races->VisRange( mChar->GetRace() ));
	}

	const SI16 newx	= mChar->GetX();
	const SI16 newy	= mChar->GetY();
	const UI16 instanceId = mChar->GetInstanceId();
	UI16 id;
	ItemTypes type;
	bool EffRange;
	bool inMoveDetectRange = false;
	UI08 moveDetectRange = 1;

	bool isGM = mChar->IsGM();
	UI16 dxNew, dyNew, dxOld, dyOld;
	const bool checkX	= ( oldx != newx );
	const bool checkY	= ( oldy != newy );
	/*
	A note to future people (from 2/10/02)

	The OSI client MAY be ignoring certain items that it thinks fall out of the vis Range...

	Currently, UOX calculates vis in a square... meaning if an item in the corner of our
	'square' is sent to the OSI client, it MAY ignore it because it falls out of the client's
	apparent circle of visibility.

	At this time, I'm not sure this is causing problems, or whether problems are steming from
	a region issue (fixed now).  So I have not updated this code.  If this problem pops up in
	the future, and I'm no longer around...  This is your piece of the puzzle.
	
	Update from future people (from 23/05/2021):
		While UOX calculated vis in a square during movement, it calculated vis in a radius on logins,
		teleports, object additions/removals. This means someone could log in, get sent all objects
		within a radius of X, then when they started moving, they would get sent all objects entering
		a square boundary around character (with distance X) - skipping any objects trapped in the
		area between the circle and the square.
	
		Logins, teleports, object additions and removals all now calculate vis range via square
		pattern instead of a circular one, which seems to fix most of these 20+ year old issues.
	*/
	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	std::vector<UI16> itemScriptTriggers;
	for( auto &MapArea : MapRegion->PopulateList( newx, newy, mChar->WorldNumber() ))
	{
		if( MapArea == nullptr )	// no valid region
			continue;

		if( mSock ) // Only send char stuff if we have a valid socket
		{
			auto regChars = MapArea->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( ValidateObject( tempChar ) && tempChar->GetInstanceId() == instanceId )
				{
					// Character Send Stuff
					if( tempChar->IsNpc() || IsOnline(( *tempChar )) || ( isGM && cwmWorldState->ServerData()->ShowOfflinePCs() ))
					{
						dxNew = static_cast<UI16>( abs( tempChar->GetX() - newx ));
						dyNew = static_cast<UI16>( abs( tempChar->GetY() - newy ));
						if( checkX && dyNew <= ( visibleRange + 2 )) // Only update on x plane if our x changed
						{
							dxOld = static_cast<UI16>( abs( tempChar->GetX() - oldx ));
							if( UpdateCharsOnPlane( mSock, mChar, tempChar, dxNew, dxOld, visibleRange ))
							{
								continue;	// If we moved diagonally, don't update the same char twice.
							}
						}
						if( checkY && dxNew <= ( visibleRange + 2 )) // Only update on y plane if our y changed
						{
							dyOld = static_cast<UI16>( abs( tempChar->GetY() - oldy ));
							
							if( UpdateCharsOnPlane( mSock, mChar, tempChar, dyNew, dyOld, visibleRange ))
							{
								continue; // ?? this will continue anyway
							}
						}
					}
				}
			}
		}
		auto regItems = MapArea->GetItemList();
		for( const auto &tItem : regItems->collection() )
		{
			if( !ValidateObject( tItem ) || tItem->GetInstanceId() != instanceId )
				continue;

			id = tItem->GetId();
			type = tItem->GetType();
			EffRange = ( tItem->GetX() == newx && tItem->GetY() == newy && std::abs( mChar->GetZ() - tItem->GetZ() ) <= 4 );

			// Get max movement detection range from item's MORE property (part 1)
			moveDetectRange = tItem->GetTempVar( CITV_MORE, 1 );

			// Determine if character is moving within the moveDetectRange limit
			inMoveDetectRange = ( std::abs( tItem->GetX() - newx ) <= moveDetectRange && std::abs( tItem->GetY() - newy ) <= moveDetectRange 
				&& std::abs( mChar->GetZ() - tItem->GetZ() ) <= 5 );

			if( EffRange || inMoveDetectRange )
			{
				if( !Magic->HandleFieldEffects( mChar, tItem, id ))
				{
					if( !tItem->CanBeObjType( OT_MULTI ))
					{
						bool scriptExecuted = false;
						auto scriptTriggers = tItem->GetScriptTriggers();
						for( auto i : scriptTriggers )
						{
							// Loop through all scriptIDs registered for item, check for scripts
							cScript *toExecute = JSMapping->GetScript( i );
							if( toExecute )
							{
								if( EffRange )
								{
									// Script was found, let's check for onCollide event
									SI08 retVal = toExecute->OnCollide( mSock, mChar, tItem );
									if( retVal != -1 )
									{
										scriptExecuted = true;
										if( retVal == 1 )
										{
											// Script returned 1 - don't continue with other scripts on item
											break;
										}
									}
								}
								if( inMoveDetectRange )
								{
									UI08 rangeToChar = GetDist( Point3_st( tItem->GetX(), tItem->GetY(), tItem->GetZ() ), Point3_st( newx, newy, mChar->GetZ() ));

									// Script was found, let's check for onMoveDetect event
									SI08 retVal = toExecute->OnMoveDetect( tItem, mChar, rangeToChar, oldx, oldy );
									if( retVal != -1 )
									{
										scriptExecuted = true;
										if( retVal == 1 )
										{
											// Script returned 1 - don't continue with other scripts on item
											break;
										}
									}
								}
							}
						}

						// Handle envoke stuff outside for loop, as we only want this to execute once
						cScript *toExecute = nullptr;
						if( scriptTriggers.size() == 0 || !scriptExecuted )
						{
							UI16 envTrig = 0;
							if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>( type )))
							{
								envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>( type ));
								toExecute = JSMapping->GetScript( envTrig );
							}
							else if( JSMapping->GetEnvokeById()->Check( id ))
							{
								envTrig = JSMapping->GetEnvokeById()->GetScript( id );
								toExecute = JSMapping->GetScript( envTrig );
							}
						}

						if( toExecute )
						{
							if( EffRange )
							{
								// We don't care about the return value from onCollide here, so suppress the warning
								[[maybe_unused]] SI08 retVal = toExecute->OnCollide( mSock, mChar, tItem );
							}
							if( inMoveDetectRange )
							{
								UI08 rangeToChar = GetDist( Point3_st( tItem->GetX(), tItem->GetY(), tItem->GetZ() ), Point3_st( newx, newy, mChar->GetZ() ));

								// We don't care about the return value from onCollide here, so suppress the warning
								[[maybe_unused]] SI08 retVal = toExecute->OnMoveDetect( tItem, mChar, rangeToChar, oldx, oldy );
							}
						}

						// Ok let's trigger onCollide for the character as well
						if( EffRange )
						{
							std::vector<UI16> charScriptTriggers = mChar->GetScriptTriggers();
							for( auto scriptTrig : charScriptTriggers )
							{
								toExecute = JSMapping->GetScript( scriptTrig );
								if( toExecute)
								{
									if( toExecute->OnCollide( mSock, mChar, tItem ) == 1 )
									{
										break;
									}
								}
							}
						}
					}
				}
				if( EffRange )
				{
					HandleObjectCollisions( mSock, mChar, tItem, type );
					Magic->GateCollision( mSock, mChar, tItem, type );
				}
			}

			dxNew = static_cast<UI16>( abs( tItem->GetX() - newx ));
			dyNew = static_cast<UI16>( abs( tItem->GetY() - newy ));
			if( checkX && dyNew <= ( visibleRange + 2 )) // Only update items on furthest x plane if our x changed
			{
				dxOld = static_cast<UI16>( abs( tItem->GetX() - oldx ));
				if( UpdateItemsOnPlane( mSock, mChar, tItem, id, dxNew, dxOld, visibleRange, isGM ))
				{
					continue;	// If we moved diagonally, lets not update an item twice (since it could match the furthest x and y)
				}
			}
			if( checkY && dxNew <= ( visibleRange + 2 )) // Only update items on furthest y plane if our y changed
			{
				dyOld = static_cast<UI16>( abs( tItem->GetY() - oldy ));
				if( UpdateItemsOnPlane( mSock, mChar, tItem, id, dyNew, dyOld, visibleRange, isGM ))
				{
					continue;  // ?????
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::IsGMBody()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character's body is to be considered a GM body for the purposes of
//|					shove messages and walking through doors.
//|					Includes GMs, Counsellors, and Ghosts; Better name may be IsEthereal()
//o------------------------------------------------------------------------------------------------o
bool CMovement::IsGMBody( CChar *c )
{
	if( c->IsGM() || c->GetId() == 0x03DB || c->GetId() == 0x0192 ||
	   c->GetId() == 0x0193 ||	c->GetId() == 0x025F || c->GetId() == 0x0260 ||
	   c->GetId() == 0x02B6 || c->GetId() == 0x02B7 || c->IsDead() )
		return true;

	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CombatWalk()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send combat toggle to nearby players. Only for switching to combat mode
//o------------------------------------------------------------------------------------------------o
void CMovement::CombatWalk( CChar *i )
{
	if( !ValidateObject( i ))
		return;

	CPExtMove toSend = ( *i );

	if( !i->IsAtWar() )
	{
		i->SetTarg( nullptr );
	}

	for( auto &sock : FindNearbyPlayers( i ))
	{
		CChar *mChar = sock->CurrcharObj();
		if( mChar != i )
		{
			toSend.FlagColour( static_cast<UI08>( i->FlagColour( mChar )));
			sock->Send( &toSend );
		}
	}
}

bool CheckBoundingBox( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 fx2, SI16 fy2, UI08 worldNumber, UI16 instanceId );
bool CheckBoundingCircle( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 radius, UI08 worldNumber, UI16 instanceId );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::NpcWalk()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle NPCs walking in boxes and circles
//|	Notes		-	getWander is npcwalk mode (0 for normal, 1 for follow, 2 for free, 3 for box,
//|					4 for circle, 5 for frozen, 6 for flee, 7 for pathfind
//o------------------------------------------------------------------------------------------------o
void CMovement::NpcWalk( CChar *i, UI08 j, SI08 getWander )
{
	const SI16 fx1 = i->GetFx( 0 );
	const SI16 fx2 = i->GetFx( 1 );
	const SI16 fy1 = i->GetFy( 0 );
	const SI16 fy2 = i->GetFy( 1 );
	const SI08 fz1 = i->GetFz();
	// if we are walking in an area, and the area is not properly defined, just don't bother with the area anymore
	if((( getWander == WT_BOX ) && ( fx1 == -1 || fx2 == -1 || fy1 == -1 || fy2 == -1 )) ||
	   (( getWander == WT_CIRCLE ) && ( fx1 == -1 || fx2 == -1 || fy1 == -1 ))) // circle's don't use fy2, so don't require them! 10/30/1999
	{
		i->SetNpcWander( WT_FREE ); // Wander freely from now on
	}
	// New Stuff 2000.09.21
	const SI16 newx			= GetXfromDir( j, i->GetX() );
	const SI16 newy			= GetYfromDir( j, i->GetY() );
	const UI08 worldNumber	= i->WorldNumber();
	const UI16 instanceId	= i->GetInstanceId();
	// Let's make this a little more readable.
	const UI08 jMod			= ( j & 0x87 );
	bool pathFound = true;
	UI16 fx2Actual = 0;
	UI16 fy2Actual = 0;
	switch( getWander )
	{
		case WT_FREE:	// Wander freely
		case WT_FLEE:	// Wander freely after fleeing
			Walking( nullptr, i, jMod, 256 );
			break;
		case WT_BOX:	// Wander inside a box
			if( CheckBoundingBox( newx, newy, fx1, fy1, fz1, fx2, fy2, worldNumber, instanceId ))
			{
				Walking( nullptr, i, jMod, 256 );
			}
			// The NPC is outside it's area, send it back
			else if( !CheckBoundingBox( i->GetX(), i->GetY(), fx1, fy1, fz1, fx2, fy2, worldNumber, instanceId ))
			{
				i->SetOldNpcWander( WT_BOX );
				i->SetNpcWander( WT_PATHFIND );

				if( cwmWorldState->ServerData()->AdvancedPathfinding() )
				{
					pathFound = AdvancedPathfinding( i, fx1, fy1, true );
				}
				else
				{
					PathFind( i, fx1, fy2, true );
				}

				fx2Actual = fx2;
				fy2Actual = fy2;

				if( pathFound )
				{
					j = i->PopDirection();
					Walking( nullptr, i, j, 256 );
				}
			}
			break;
		case WT_CIRCLE:	// Wander inside a circle
			if( CheckBoundingCircle( newx, newy, fx1, fy1, fz1, fx2, worldNumber, instanceId ))
			{
				Walking( nullptr, i, jMod, 256 );
			}
			// The NPC is outside it's area, send it back
			else if( !CheckBoundingCircle( i->GetX(), i->GetY(), fx1, fy1, fz1, fx2, worldNumber, instanceId ))
			{
				i->SetOldNpcWander( WT_CIRCLE );
				i->SetNpcWander( WT_PATHFIND );
				if( cwmWorldState->ServerData()->AdvancedPathfinding() )
				{
					pathFound = AdvancedPathfinding( i, fx1, fy1, true );
				}
				else
				{
					PathFind( i, fx1, fy2, true );
				}

				fx2Actual = fx1 + fx2;
				fy2Actual = fy1 + fx2;

				if( pathFound )
				{
					j = i->PopDirection();
					Walking( nullptr, i, j, 256 );
				}
			}
			break;
		default:
			Console.Error( oldstrutil::format( "Bad NPC Wander type passed to NpcWalk: %i", getWander ));
			break;
	}
	// If path back can't be found, use alternative route - through the magical ether! (teleport)
	if( cwmWorldState->ServerData()->AdvancedPathfinding() && !pathFound )
	{
		BoundingBoxTeleport( i, fx2Actual, fy2Actual, newx, newy );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::BoundingBoxTeleport()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Teleport NPC back to BoundingBox if path wasn't found
//o------------------------------------------------------------------------------------------------o
void CMovement::BoundingBoxTeleport( CChar *nChar, UI16 fx2Actual, UI16 fy2Actual, SI16 newx, SI16 newy )
{
	const SI16 fx1 = nChar->GetFx( 0 );
	const SI16 fx2 = nChar->GetFx( 1 );
	const SI16 fy1 = nChar->GetFy( 0 );
	const SI16 fy2 = nChar->GetFy( 1 );
	const SI08 fz1 = nChar->GetFz();
	const UI08 worldNumber = nChar->WorldNumber();
	const UI16 instanceId = nChar->GetInstanceId();
	bool checkSuccess = true;

	if( !nChar->GetOldNpcWander() )
	{
		nChar->SetOldNpcWander( nChar->GetNpcWander() );
	}

	switch( nChar->GetOldNpcWander() )
	{
		case WT_BOX:
			if( !CheckBoundingBox( newx, newy, fx1, fy1, fz1, fx2, fy2, worldNumber, instanceId ))
			{
				checkSuccess = false;
			}
			break;
		case WT_CIRCLE:
			if( !CheckBoundingCircle( newx, newy, fx1, fy1, fz1, fx2, worldNumber, instanceId ))
			{
				checkSuccess = false;
			}
			break;
		default:
			break;
	}

	if( checkSuccess == false )
	{
		bool boundingBoxTeleport = false;
		bool waterWalk = cwmWorldState->creatures[nChar->GetId()].IsWater();
		bool amphibWalk = cwmWorldState->creatures[nChar->GetId()].IsAmphibian();
		for( UI16 m = fx1; m < fx2Actual; m++ )
		{
			for( UI16 n = fy1; n < fy2Actual; n++ )
			{
				if(( !waterWalk || amphibWalk ) && Map->ValidSpawnLocation( m, n, fz1, worldNumber, false ))
				{
					boundingBoxTeleport = true;
				}
				else if( waterWalk && Map->ValidSpawnLocation( m, n, fz1, worldNumber, true ))
				{
					boundingBoxTeleport = true;
				}
				if( boundingBoxTeleport == true )
				{
#if defined( UOX_DEBUG_MODE )
					std::string charName = GetNpcDictName( nChar );
					Console.Warning( oldstrutil::format( "NPC: %s with serial 0x%X was unable to path back to bounding box, teleporting NPC back.\n", charName.c_str(), nChar->GetSerial() ));
#endif
					nChar->SetLocation( m, n, fz1, nChar->WorldNumber(), nChar->GetInstanceId() );
					nChar->SetNpcWander( nChar->GetOldNpcWander() );
					nChar->SetOldNpcWander( WT_NONE );
					return;
				}
			}
		}
		// If a valid teleport-location hasn't been found at this point, despawn NPC
#if defined( UOX_DEBUG_MODE )
		std::string charName = GetNpcDictName( nChar );
		Console.Warning( oldstrutil::format( "NPC: %s with serial 0x%X was unable to path back to bounding box, no valid teleport location found. Deleting NPC!\n", charName.c_str(), nChar->GetSerial() ));
#endif
		nChar->Delete();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::GetYfromDir()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the new y from given dir
//o------------------------------------------------------------------------------------------------o
SI16 CMovement::GetYfromDir( UI08 dir, SI16 y )
{
	switch( dir & 0x07 )
	{
		case NORTH:
		case NORTHEAST:
		case NORTHWEST:
			--y;
			break;
		case SOUTH:
		case SOUTHEAST:
		case SOUTHWEST:
			++y;
			break;
		default:
			break;
	}
	return y;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::GetXfromDir()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the new x from given dir
//o------------------------------------------------------------------------------------------------o
SI16 CMovement::GetXfromDir( UI08 dir, SI16 x )
{
	switch( dir & 0x07 )
	{
		case EAST:
		case NORTHEAST:
		case SOUTHEAST:
			++x;
			break;
		case WEST:
		case NORTHWEST:
		case SOUTHWEST:
			--x;
			break;
		default:
			break;
	}
	return x;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::PathFind()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate a path from A to B for NPC movement
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Ok, I'm going to babble here, but here's my thinking process...
//|					Max Heuristic is 5 (for now) and I'm not concerned about walls... I'll add that later
//|					Easiest way I think would be for recursive call for now... Will change later if need be
//|					pathfind will call itself with new stuff... as long as the distance get's shorter
//|					We have to take into consideration if the NPC is going to a point or to a character
//|					if we don't want NPCs to walk over each other, this has to be known, because the NPC
//|					that's walking will never reach destination if it's another character.
//|					We must pass, that way if we get chardist=1 then we're ok.  We are basically searching
//|					for the shortest path, which is always a diagonal line, followed by lateral to target
//|					(barring obstacles) On calculation, for the FIRST step, we need to know if a character
//|					is there or not, then after that no biggie because if so we can just recalc the path if
//|					something is blocking. If we don't check on that first step, NPCs will get stuck behind horses
//|					and stuff... Kinda exploitable if I'm on a horse attacking, then step off and behind to hide
//|					while I heal. The first thing we need to do when walking is determine if i'm blocked... then
//|					if I'm an NPC, recalculate my path and step... I'm also gonna take out the path structure
//|					in chars_st... all we need is to hold the directions, not the x and y... Hopefully this will
//|					save memory.
//o------------------------------------------------------------------------------------------------o
void CMovement::PathFind( CChar *c, SI16 gx, SI16 gy, bool willRun, UI08 pathLen )
{
	// Make sure this is a valid character before proceeding
	if( !ValidateObject( c ))
		return;

	// Make sure the character has taken used all of their previously saved steps
	if( c->StillGotDirs() )
		return;

	// Set target location in NPC's mind
	c->SetPathTargX( gx );
	c->SetPathTargY( gy );

	// 2000.09.21
	// initial rewrite of pathfinding...

	const SI16 oldx = c->GetX();
	const SI16 oldy = c->GetY();
	SI16 newx		= oldx;
	SI16 newy		= oldy;
	SI08 newz		= c->GetZ();
	UI08 olddir		= c->GetDir();
	UI08 pf_dir		= Direction( newx, newy, gx, gy );

	for( UI08 pn = 0; pn < pathLen; ++pn )
	{
		bool bFound			= false;
		SI32 pf_neg			= (( RandomNum( 0, 1 )) ? 1 : -1 );
		UI08 newDir			= Direction( newx, newy, gx, gy );

		bool canMoveInDir	= false;
		if( newDir < 8 && CalcMove( c, newx, newy, newz, newDir ))
		{
			pf_dir = newDir;
			canMoveInDir = true;
		}
		for( UI08 i = 0; i < 7; ++i )
		{
			if( canMoveInDir || ( pf_dir < 8 && CalcMove( c, newx, newy, newz, pf_dir )))
			{
				newx = GetXfromDir( pf_dir, newx );	// moved inside if to reduce calculations
				newy = GetYfromDir( pf_dir, newy );
				if(( pn < P_PF_MRV ) && CheckForCharacterAtXYZ( c, newx, newy, newz ))
				{
					// Character is blocking the way. Let's try to find a way around by
					// randomizing the direction a bit!
					SI08 rndDir = pf_dir + RandomNum( -2, 2 );
					if( rndDir < 0 )
					{
						rndDir = 7 + ( rndDir + 1 );
					}
					else if( rndDir > 7 )
					{
						rndDir = 0 + abs( 7 - ( rndDir - 1 ));
					}
					pf_dir = rndDir;
				}

				UI08 dirToPush = pf_dir;
				if( willRun )
				{
					dirToPush |= 0x80;
				}

				c->PushDirection( dirToPush );

				if( olddir != UNKNOWNDIR && olddir != pf_dir )
				{
					c->PushDirection( dirToPush );
				}

				olddir = pf_dir;

				bFound = true;
				break;
			}
			pf_neg *= -1;
			pf_dir = static_cast<UI08>( static_cast<UI08>( pf_dir + ( i * pf_neg )) % 8 );
		}
		if( !bFound )
		{
#if DEBUG_PATHFIND
			Console << "Character stuck!" << myendl;
#endif
			break;
		}
		if( newx == gx && newy == gy )
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::HandleNPCWander()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle NPC movement when in wander modes
//o------------------------------------------------------------------------------------------------o
bool CMovement::HandleNPCWander( CChar& mChar )
{
	bool shouldRun	= false;
	bool canRun		= false;
	CChar *kChar	= nullptr;
	UI08 j;
	SI08 npcWanderType = mChar.GetNpcWander();
	switch( npcWanderType )
	{
		case WT_NONE: // No movement
			if( mChar.GetOldNpcWander() != WT_NONE && mChar.GetTimer( tNPC_MOVETIME ) <= cwmWorldState->GetUICurrentTime() )
			{
				mChar.SetNpcWander( mChar.GetOldNpcWander() );
			}
			break;
		case WT_FOLLOW: // Follow the follow target
			kChar = mChar.GetFTarg();
			if( !ValidateObject( kChar ))
				break;

			// Don't allow NPCs to follow characters who are in a different world/instance!
			if( mChar.WorldNumber() != kChar->WorldNumber() || mChar.GetInstanceId() != kChar->GetInstanceId() )
				break;

			if( IsOnline(( *kChar )) || kChar->IsNpc() )
			{
				const UI16 charDist	= GetDist( &mChar, kChar );
				if( !ObjInRange( &mChar, kChar, DIST_NEXTTILE ) && Direction( &mChar, kChar->GetX(), kChar->GetY() ) < 8 )
				{
					if( mChar.GetStamina() > 0 )
					{
						if( kChar->GetRunning() > 0 )
						{
							canRun = true;
						}
						else if( charDist >= DIST_INRANGE )
						{
							canRun = true;
						}
						else if(( mChar.GetRunning() > 0 ) && ( charDist > DIST_NEARBY ))
						{
							canRun = true;
						}
					}

					// Don't always re-calculate pathfinding on every step
					// Update some of the time if target moves, but also use what we already have
					SI16 oldTargX = 0;
					SI16 oldTargY = 0;
					oldTargX = mChar.GetOldTargLocX();
					oldTargY = mChar.GetOldTargLocY();

					if( !mChar.StillGotDirs() || (( oldTargX != kChar->GetX() || oldTargY != kChar->GetY() ) && RandomNum( 0, 10 ) >= 6 ))
					{
						if( cwmWorldState->ServerData()->AdvancedPathfinding() )
						{
							if( !AdvancedPathfinding( &mChar, kChar->GetX(), kChar->GetY(), canRun ))
							{
								// If NPC is unable to follow owner, teleport to owner
								if( mChar.GetOwnerObj() == kChar )
								{
									mChar.SetLocation( kChar );
								}
								else
								{
									// If NPC following fails to follow, make it stop
									mChar.SetOldTargLocX( 0 );
									mChar.SetOldTargLocY( 0 );
									mChar.TextMessage( nullptr, Dictionary->GetEntry( 9048 ), SYSTEM, false ); // [Stops following]
									mChar.SetNpcWander( WT_NONE );
								}
							}
							else
							{
								mChar.SetOldTargLocX( kChar->GetX() );
								mChar.SetOldTargLocY( kChar->GetY() );
							}
						}
						else
						{
							PathFind( &mChar, kChar->GetX(), kChar->GetY(), canRun );
						}
					}

					j = mChar.PopDirection();
					Walking( nullptr, &mChar, j, 256 );
					shouldRun = (( j&0x80 ) != 0 );
				}
				// Has the Escortee reached the destination ??
				if( !kChar->IsDead() && mChar.GetQuestDestRegion() == mChar.GetRegionNum() )
				{
					MsgBoardQuestEscortArrive( kChar->GetSocket(), &mChar );
				}
			}
			break;
		case WT_FREE: // Wander freely, avoiding obstacles.
		case WT_BOX: // Wander freely, within a defined box
		case WT_CIRCLE: // Wander freely, within a defined circle
			j = RandomNum( 1, 5 );
			if( j == 1 )
			{
				break;
			}
			else if( j == 2 )
			{
				j = RandomNum( 0, 7 );
			}
			else // Move in the same direction the majority of the time
			{
				j = mChar.GetDir();
			}
			shouldRun = (( j&0x80 ) != 0);
			NpcWalk( &mChar, j, npcWanderType );
			break;
		case WT_FROZEN: // No movement whatsoever!
			break;
		case WT_FLEE: // FLEE!!!!!!
			kChar = mChar.GetTarg();
			if( !ValidateObject( kChar ))
				break;

			if(( mChar.WorldNumber() == kChar->WorldNumber() && mChar.GetInstanceId() == kChar->GetInstanceId() )
				||  GetDist( &mChar, kChar ) < P_PF_MFD )
			{	// calculate a x,y to flee towards
				const UI16 mydist	= P_PF_MFD - GetDist( &mChar, kChar ) + 1;
				j					= Direction( &mChar, kChar->GetX(), kChar->GetY() );
				SI16 myx			= GetXfromDir( j, mChar.GetX() );
				SI16 myy			= GetYfromDir( j, mChar.GetY() );

				SI16 xFactor = 0;
				SI16 yFactor = 0;
				// Sept 22, 2002
				if( myx != mChar.GetX() )
				{
					if( myx < mChar.GetX() )
					{
						xFactor = 1;
					}
					else
					{
						xFactor = -1;
					}
				}

				if( myy != mChar.GetY() )
				{
					if( myy < mChar.GetY() )
					{
						yFactor = 1;
					}
					else
					{
						yFactor = -1;
					}
				}

				myx += static_cast<SI16>( xFactor * mydist );
				myy += static_cast<SI16>( yFactor * mydist );

				canRun = ( mChar.GetStamina() > 0 );

				// now, got myx, myy... lets go.

				// Don't re-calculate pathfinding on every step. Use what we have, with a random chance to recalculate
				if( !mChar.StillGotDirs() || RandomNum( 0, 10 ) >= 6 )
				{
					if( cwmWorldState->ServerData()->AdvancedPathfinding() )
					{
						if( !AdvancedPathfinding( &mChar, myx, myy, true ))
						{
							// If NPC fails to flee, make him re-attack instead!
							mChar.SetFleeAt( 0 );
							mChar.SetReattackAt( 1 );
							mChar.SetNpcWander( mChar.GetOldNpcWander() );
							if( mChar.GetMounted() )
							{
								mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedRunningSpeed() ));
							}
							else
							{
								mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetRunningSpeed() ));
							}
							mChar.SetOldNpcWander( WT_NONE ); // so it won't save this at the wsc file
						}
					}
					else
					{
						PathFind( &mChar, myx, myy, true );
					}
				}

				j = mChar.PopDirection();
				shouldRun = (( j&0x80 ) != 0);
				Walking( nullptr, &mChar, j, 256 );
			}
			else
			{
				// wander freely... don't just stop because I'm out of range.
				j = RandomNum( 1, 5 );
				if( j == 1 )
				{
					break;
				}
				else if( j == 2 )
				{
					j = RandomNum( 0, 8 );
				}
				else	// Move in the same direction the majority of the time
				{
					j = mChar.GetDir();
				}
				shouldRun = (( j&0x80 ) != 0);
				NpcWalk( &mChar, j, npcWanderType );
			}
			break;
		case WT_PATHFIND:		// Pathfinding!!!!
			if( mChar.StillGotDirs() )
			{
				j = mChar.PopDirection();
				shouldRun = (( j&0x80 ) != 0);
				Walking( nullptr, &mChar, j, 256 );
			}
			else
			{
				mChar.SetNpcWander( mChar.GetOldNpcWander() );

				std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
				for( auto scriptTrig : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( scriptTrig );
					if( toExecute != nullptr )
					{
						// Reached end of pathfinding, if script returns true/1, prevent other scripts with event from running
						if( toExecute->OnPathfindEnd( &mChar, mChar.GetPathResult() ) == 1 )
						{
							break;
						}
					}
				}
			}
			break;
		default:
			break;
	}
	return shouldRun;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::NpcMovement()
//|	Date		-	09/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate, and handle NPC AI movement
//|
//|	Changes		-	09/22/2002	-	Fixed fleeing NPCs by reversing values for
//|									xFactor & yFactor lines
//o------------------------------------------------------------------------------------------------o
void CMovement::NpcMovement( CChar& mChar )
{
	if( mChar.IsFrozen() || !mChar.IsNpc() || mChar.GetNpcWander() == WT_FROZEN )
		return;

	bool canRun	= (( mChar.GetStamina() > 0 ) && mChar.CanRun() );

	if( mChar.GetTimer( tNPC_MOVETIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
#if DEBUG_NPCWALK
		std::string charName = GetNpcDictName( mChar );
		Console.Print( oldstrutil::format( "DEBUG: ENTER (%s): %d AI %d WAR %d J\n", charName, mChar.GetNpcWander(), mChar.IsAtWar(), j ));
#endif
		bool shouldRun = false;
		if( mChar.IsAtWar() && mChar.GetNpcWander() != WT_FLEE )
		{
			CChar *l = mChar.GetAttacker();
			if( ValidateObject( l ) && ( IsOnline(( *l )) || l->IsNpc() ))
			{
				const UI08 charDir	= Direction( &mChar, l->GetX(), l->GetY() );
				const UI16 charDist	= GetDist( &mChar, l );

				// NPC is using a ranged weapon, and is within range to shoot at the target
				CItem *equippedWeapon = Combat->GetWeapon( &mChar );
				if( charDir < 8 
					&& ( charDist <= 1 
						|| (( Combat->GetCombatSkill( equippedWeapon ) == ARCHERY || Combat->GetCombatSkill( equippedWeapon ) == THROWING ) && charDist <= equippedWeapon->GetMaxRange() )
						|| (( mChar.GetNpcAiType() == AI_CASTER || mChar.GetNpcAiType() == AI_EVIL_CASTER ) && ( charDist <= cwmWorldState->ServerData()->CombatMaxSpellRange() && mChar.GetMana() >= 10 ))))
				{
					bool los = LineOfSight( nullptr, &mChar, l->GetX(), l->GetY(), ( l->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false );
					if( los )
					{
						// Turn towards target
						if( charDir != mChar.GetDir() )
						{
							mChar.SetDir( charDir );
						}

						// No need to move any closer
						mChar.FlushPath();
						mChar.SetOldTargLocX( 0 );
						mChar.SetOldTargLocY( 0 );
						return;
					}
					else if(( !los && charDist <= 1 ) || ( !los && mChar.GetZ() - l->GetZ() >= 20 ))
					{
						// We're right next to target, but still have no LoS - or height difference is too large
						mChar.FlushPath();
						mChar.SetOldTargLocX( 0 );
						mChar.SetOldTargLocY( 0 );
						mChar.SetTimer( tNPC_EVADETIME, BuildTimeValue( 10 ));
						mChar.TextMessage( nullptr, Dictionary->GetEntry( 9049 ), SYSTEM, false ); // [Evading]
						mChar.SetHP( mChar.GetMaxHP() );
						mChar.SetEvadeState( true );
						Combat->InvalidateAttacker( &mChar );
						//Console.Warning( oldstrutil::format( "EvadeTimer started for NPC (%s, 0x%X, at %i, %i, %i, %i). Could no longer see or reach target.\n", mChar.GetName().c_str(), mChar.GetSerial(), mChar.GetX(), mChar.GetY(), mChar.GetZ(), mChar.WorldNumber() ));

						std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
						for( auto scriptTrig : scriptTriggers )
						{
							cScript *toExecute = JSMapping->GetScript( scriptTrig );
							if( toExecute != nullptr )
							{
								// If script returns true/1, prevent other scripts with event from running
								if( toExecute->OnEnterEvadeState( &mChar, l ) == 1 )
								{
									break;
								}
							}
						}
						return;
					}
				}

				if( cwmWorldState->ServerData()->AdvancedPathfinding() )
				{
					// Don't always re-calculate pathfinding on every step
					// Update some of the time if target moves, but also use what we already have
					SI16 targX = 0;
					SI16 targY = 0;
					SI16 oldTargX = 0;
					SI16 oldTargY = 0;
					oldTargX = mChar.GetOldTargLocX();
					oldTargY = mChar.GetOldTargLocY();

					if( mChar.GetPathFail() > 10 && mChar.GetSpAttack() > 0 && mChar.GetMana() > 0 && charDist <= cwmWorldState->ServerData()->CombatMaxSpellRange() )
					{
						// NPC already within spellcasting distance, we can forgive a lack of pathfinding to target
						UI08 NewDir = Direction( &mChar, l->GetX(), l->GetY() );
						
						// Turn towards target
						if( NewDir != mChar.GetDir() )
						{
							mChar.SetDir( NewDir );
						}

						// Clear pathfinding data
						mChar.FlushPath();
						mChar.SetOldTargLocX( 0 );
						mChar.SetOldTargLocY( 0 );
						mChar.SetPathFail( 0 );

						// Temporary change NPC's wandermode to none, and try pathfinding again in 5 seconds
						if( mChar.GetNpcWander() != WT_NONE )
						{
							mChar.SetOldNpcWander( mChar.GetNpcWander() );
						}
						mChar.SetNpcWander( WT_NONE );
						mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( 5 ));
						return;
					}
					else if( mChar.GetSpAttack() > 0 && mChar.GetMana() > 0 )
					{
						targX = mChar.GetPathTargX();
						targY = mChar.GetPathTargY();
					}
					else
					{
						targX = l->GetX();
						targY = l->GetY();
					}

					if( !mChar.StillGotDirs() || (( oldTargX != targX || oldTargY != targY ) && RandomNum( 0, 10 ) >= 6 ))
					{
						if( !AdvancedPathfinding( &mChar, l->GetX(), l->GetY(), canRun ))
						{
							mChar.SetPathFail( mChar.GetPathFail() + 1 );

							if( mChar.GetSpAttack() > 0 && mChar.GetMana() > 0 )
							{
								// What if we try another location that's nearby the target, but not exactly the target?
								SI16 rndNum1 = RandomNum( -2, 2 );
								SI16 rndNum2 = RandomNum( -2, 2 );
								SI16 rndTargX = l->GetX() + rndNum1;
								SI16 rndTargY = l->GetY() + rndNum2;

								if( AdvancedPathfinding( &mChar, rndTargX, rndTargY, canRun ))
								{
									mChar.SetPathFail( 0 );
									mChar.SetOldTargLocX( rndTargX );
									mChar.SetOldTargLocY( rndTargY );
									mChar.SetPathTargX( static_cast<UI16>( rndTargX ));
									mChar.SetPathTargY( static_cast<UI16>( rndTargY ));
								}
								else
								{
									mChar.SetPathFail( mChar.GetPathFail() + 1 );
								}
							}

							if( mChar.IsAtWar() && mChar.GetNpcWander() != WT_FLEE )
							{
								if( mChar.GetSpAttack() > 0 && mChar.GetMana() > 0 && charDist <= cwmWorldState->ServerData()->CombatMaxSpellRange() )
								{
									// NPC already within spellcasting distance, we can forgive a lack of pathfinding to target
									UI08 NewDir = Direction( &mChar, l->GetX(), l->GetY() );
									if( NewDir != mChar.GetDir() )
									{
										mChar.SetDir( NewDir );
									}
									return;
								}

								if( mChar.GetPathFail() < 20 )
								{
									// Let's try just resetting the target first - maybe NPC will find another target that's closer?
									mChar.SetTarg( nullptr );
									mChar.SetWar( false );
									return;
								}

								mChar.FlushPath();
								mChar.SetOldTargLocX( 0 );
								mChar.SetOldTargLocY( 0 );
								mChar.SetTimer( tNPC_EVADETIME, BuildTimeValue( 10 ));
								mChar.TextMessage( nullptr, Dictionary->GetEntry( 9049 ), SYSTEM, false ); // [Evading]
								mChar.SetHP( mChar.GetMaxHP() );
								mChar.SetEvadeState( true );
								Combat->InvalidateAttacker( &mChar );
								//Console.Warning( oldstrutil::format( "EvadeTimer started for NPC (%s, 0x%X, at %i, %i, %i, %i).\n", mChar.GetName().c_str(), mChar.GetSerial(), mChar.GetX(), mChar.GetY(), mChar.GetZ(), mChar.WorldNumber() ));

								std::vector<UI16> scriptTriggers = mChar.GetScriptTriggers();
								for( auto scriptTrig : scriptTriggers )
								{
									cScript *toExecute = JSMapping->GetScript( scriptTrig );
									if( toExecute != nullptr )
									{
										// If script returns true/1, prevent other scripts with event from running
										if( toExecute->OnEnterEvadeState( &mChar, l ) == 1 )
										{
											break;
										}
									}
								}
							}
						}
						else
						{
							// Pathfinding ok!
							mChar.SetPathFail( 0 );
							mChar.SetOldTargLocX( l->GetX() );
							mChar.SetOldTargLocY( l->GetY() );
						}
					}
				}
				else
				{
					PathFind( &mChar, l->GetX(), l->GetY(), canRun ); // Non-advanced pathfinding
				}
				const UI08 j	= mChar.PopDirection();
				shouldRun		= (( j&0x80 ) != 0);
				Walking( nullptr, &mChar, j, 256 );
			}
			else
			{
				mChar.FlushPath();
			}
		}
		else
		{
			shouldRun = HandleNPCWander( mChar );
		}

		SI08 npcWanderType = mChar.GetNpcWander();
		if( shouldRun )
		{
			if( npcWanderType == WT_FOLLOW )
			{
				if( mChar.GetMounted() )
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedRunningSpeed() ));
				}
				else
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetRunningSpeed() / 1.5 )); // Increase follow speed so NPC pets/escorts can keep up with players
				}
			}
			else if( npcWanderType != WT_FLEE )
			{
				if( mChar.GetMounted() )
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedRunningSpeed() ));
				}
				else
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetRunningSpeed() ));
				}
			}
			else
			{
				if( mChar.GetMounted() )
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedFleeingSpeed() ));
				}
				else
				{
					mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetFleeingSpeed() ));
				}
			}
		}
		else if( npcWanderType == WT_NONE && mChar.GetOldNpcWander() != WT_NONE && !mChar.IsAtWar() )
		{
			return;
		}
		else
		{
			if( mChar.GetMounted() )
			{
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetMountedWalkingSpeed() ));
			}
			else
			{
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetWalkingSpeed() ));
			}
		}
	}
	else
	{
		// Play some idle/fidgeting animation instead - if character is not busy doing something else
		if( !mChar.IsAtWar() && mChar.GetNpcWander() != WT_FLEE )
		{
			if( mChar.GetTimer( tNPC_IDLEANIMTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( 3 ));
				mChar.SetTimer( tNPC_IDLEANIMTIME, BuildTimeValue( RandomNum( 5, 20 )));

				if( mChar.GetBodyType() == BT_GARGOYLE || cwmWorldState->ServerData()->ForceNewAnimationPacket() )
				{
					// When using new animation packet, client generally handles which animations
					// to play based on the type, but doesn't work for all creatures
					switch( mChar.GetId() )
					{
						case 0x5: // Eagle
						case 0xc5: // Chaos Dragon
						case 0xc6: // Order Dragon
							// Play only the default idle
							Effects->PlayNewCharacterAnimation( &mChar, N_ACT_IDLE, 0, 0 );
							break;
						case 0x90: // Sea Horse
						case 0x5f: // Turkey
						{
							// Play only one non-standard idle anim, using old animation packet
							Effects->PlayCharacterAnimation( &mChar, 3, 0, 5 );
							break;
						}
						case 0x91: // Sea Serpent
						case 0x96: // Sea Serpent
						case 0x97: // Dolphin
						{
							// Randomize between two non-standard idle anims, using old animation packet
							auto rndVal = static_cast<UI16>( RandomNum( 3, 4 ));
							Effects->PlayCharacterAnimation( &mChar, rndVal, 0, ( rndVal == 3 ? 14 : 19 ));
							break;
						}
						case 0x2b1: // Time Lord
							// No idle variation/fidget
							break;
						default:
							// Randomize between two idles, let client handle which specific anim
							// to play for each specific creature
							Effects->PlayNewCharacterAnimation( &mChar, N_ACT_IDLE, 0, RandomNum( 0, 1 ));
							break;
					}
				}
				else
				{
					if( cwmWorldState->creatures[mChar.GetId()].IsHuman() )
					{
						Effects->PlayCharacterAnimation( &mChar, RandomNum( static_cast<UI16>( ACT_IDLE_LOOK ), static_cast<UI16>( ACT_IDLE_YAWN )), 0, 5 );
					}
					else
					{
						// With old animation packet, more control is needed of which animation to play
						// and how long they should play for
						switch( mChar.GetId() )
						{
							case 0x5: // Eagle
							case 0xc5: // Chaos Dragon
							case 0xc6: // Order Dragon
								// Play only the default idle
								Effects->PlayCharacterAnimation( &mChar, ACT_MONSTER_IDLE_1, 0, 5 );
								break;
							case 0x90: // Sea Horse
							case 0x5f: // Turkey
							{
								// Play only one non-standard idle anim
								Effects->PlayCharacterAnimation( &mChar, ACT_ANIMAL_IDLE, 0, 5 );
								break;
							}
							case 0x91: // Sea Serpent
							case 0x96: // Sea Serpent
							case 0x97: // Dolphin
							{
								// Randomize between two non-standard idle anims
								auto rndVal = static_cast<UI16>( RandomNum( 3, 4 ));
								Effects->PlayCharacterAnimation( &mChar, rndVal, 0, ( rndVal == 3 ? 14 : 19 ));
								break;
							}
							case 0x2b1: // Time Lord
								// No idle variation/fidget
								break;
							default:
								if( cwmWorldState->creatures[mChar.GetId()].IsAnimal() && !cwmWorldState->creatures[mChar.GetId()].CanFly() )
								{
									// Animal, excluding birds, which have animation setups like monsters
									auto rndVal = static_cast<UI16>( RandomNum( 9, 10 ));
									Effects->PlayCharacterAnimation( &mChar, rndVal, 0, ( rndVal == 9 ? 5 : 3 ));
								}
								else
								{
									// Monster
									auto rndVal = static_cast<UI16>( RandomNum( 17, 18 ));
									Effects->PlayCharacterAnimation( &mChar, rndVal, 0, 5 );
								}
								break;
						}
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::GetAverageZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the average Z height level of the maptile at a given coordinate
//o------------------------------------------------------------------------------------------------o
void CMovement::GetAverageZ( UI08 nm, SI16 x, SI16 y, SI08& z, SI08& avg, SI08& top )
{
	SI08 zTop = Map->MapElevation( x, y, nm );
	SI08 zLeft = Map->MapElevation( x, y + 1, nm );
	SI08 zRight = Map->MapElevation( x + 1, y, nm );
	SI08 zBottom = Map->MapElevation( x + 1, y + 1, nm );

	z = zTop;
	if( zLeft < z )
	{
		z = zLeft;
	}
	if( zRight < z )
	{
		z = zRight;
	}
	if( zBottom < z )
	{
		z = zBottom;
	}

	top = zTop;
	if( zLeft > top )
	{
		top = zLeft;
	}
	if( zRight > top )
	{
		top = zRight;
	}
	if( zBottom > top )
	{
		top = zBottom;
	}

	if( abs( zTop - zBottom ) > abs( zLeft - zRight ))
	{
		avg = ( zLeft + zRight ) / 2;
	}
	else
	{
		avg = ( zTop + zBottom ) / 2;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::IsOk()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks that target location has no blocking tiles within range of
//|						character's potential new Z and the top of their head
//o------------------------------------------------------------------------------------------------o
bool CMovement::IsOk( std::vector<Tile_st> &xyblock, UI16 &xycount, UI08 world, SI08 ourZ, 
					SI08 ourTop, SI16 x, SI16 y, UI16 instanceId, bool ignoreDoor, bool waterWalk )
{
	for( auto &tile : xyblock )
	{
		if( tile.CheckFlag( TF_WET ))
		{
			// If blocking object is WET and character can swim, ignore object
			if( waterWalk )
				continue;
		}

		if( tile.CheckFlag( TF_BLOCKING ) || tile.CheckFlag( TF_SURFACE ))
		{
			// If character ignores doors (GMs/Counselors/Ghosts), and this is a door, ignore.
			if( ignoreDoor && tile.type == TileType_t::dyn && ( tile.CheckFlag(TF_DOOR ) || 
					tile.tileId == 0x692 || tile.tileId == 0x846 || tile.tileId == 0x873 ||
					( tile.tileId >= 0x6F5 && tile.tileId <= 0x6F6 )))
				continue;

			SI08 checkz = tile.altitude;
			SI08 checkTop = tile.top();

			if( checkTop > ourZ && ourTop > checkz )
				return false;
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::GetStartZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the Z height at the character's feet at specified location
//o------------------------------------------------------------------------------------------------o
void CMovement::GetStartZ( UI08 world, CChar *c, SI16 x, SI16 y, SI08 z, SI08& zlow, SI08& ztop, UI16 instanceId, bool waterwalk )
{
	SI08 landz = 0;
	SI08 landcent = 0;
	SI08 landtop = 0;
	bool landBlock = true;

	auto map = Map->SeekMap( x, y, world );

	landBlock = map.CheckFlag( TF_BLOCKING );
	if( landBlock && waterwalk && map.CheckFlag( TF_WET ))
	{
		landBlock = false;
	}
	
	std::vector<Tile_st> xyblock;
	UI16 xycount = 0;
	GetBlockingStatics( x, y, xyblock, xycount, world );
	GetBlockingDynamics( x, y, xyblock, xycount, world, instanceId );

	bool considerLand = !Map->IsIgnored( map.tileId );
	GetAverageZ( world, x, y, landz, landcent, landtop );

	bool isset = false;
	SI08 zcenter = zlow = ztop = 0;

	if( considerLand && !landBlock && z >= landcent )
	{
		zlow = landz;
		zcenter = landcent;

		if( !isset || landtop > ztop )
		{
			ztop = landtop;
		}

		isset = true;
	}

	for( auto &tile : xyblock )
	{
		// If the tile is a surface that can be walked on, or swam on...
		if(( !isset || tile.top() >= zcenter ) && 
			( tile.CheckFlag( TF_SURFACE ) || ( waterwalk && tile.CheckFlag( TF_WET ))) && z >= tile.top() )
		{
			// Fetch the base Z position of surface tile
			zlow = tile.altitude;

			// Fetch the top Z position of surface tile as zcenter
			zcenter = tile.top();

			SI08 top = tile.altitude + tile.height();

			if( !isset || top > ztop )
			{
				ztop = top;
			}

			isset = true;
		}
	}

	if( !isset )
	{
		zlow = ztop = z;
	}
	else if( z > ztop )
	{
		ztop = z;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::Direction()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Modified the old CheckWalkable function so that it can be utilized throughout
//|					the walking code. Ever wonder why NPCs can walk through walls and stuff in
//|					combat mode? This is the fix, plus more.
//o------------------------------------------------------------------------------------------------o
UI08 CMovement::Direction( CChar *mChar, SI16 x, SI16 y )
{
	return Direction( mChar->GetX(), mChar->GetY(), x, y );
}
UI08 CMovement::Direction( SI16 sx, SI16 sy, SI16 dx, SI16 dy )
{
	UI08 dir;

	const SI16 xdif = static_cast<SI16>( dx - sx );
	const SI16 ydif = static_cast<SI16>( dy - sy );

	if( xdif == 0 && ydif < 0 )
	{
		dir = NORTH;
	}
	else if( xdif > 0 && ydif < 0 )
	{
		dir = NORTHEAST;
	}
	else if( xdif > 0 && ydif == 0 )
	{
		dir = EAST;
	}
	else if( xdif > 0 && ydif > 0 )
	{
		dir = SOUTHEAST;
	}
	else if( xdif == 0 && ydif > 0 )
	{
		dir = SOUTH;
	}
	else if( xdif < 0 && ydif > 0 )
	{
		dir = SOUTHWEST;
	}
	else if( xdif < 0 && ydif == 0 )
	{
		dir = WEST;
	}
	else if( xdif < 0 && ydif < 0 )
	{
		dir = NORTHWEST;
	}
	else
	{
		dir = UNKNOWNDIR;
	}

	return dir;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CalcWalk()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates new Z height position for a walking character (PC or NPC) and
//|						checks if movement is blocked
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Rewritten checkwalk-function, it calculates new z-position for a walking
//|					creature( PC or NPC ) walks, and checks if movement is blocked.
//|
//|					This function takes a little more calculation time, than the last one, since it
//|					walks two times through the static - tile set. However at least this one is( more )
//|					correct, and these VERy guys now hit their noses on the walls.
//|
//|					In principle it is the same as the World - kernel in UMelange.
//|
//|					Parameters:
//|						CChar *c			Character
//|						SI16 x, y			new cords.
//|						SI16 oldx, oldy		old cords.
//|						bool justask		don't make any changes, the func. is just asked "what if"..
//|						bool waterWalk		Character can swim (true/false)
//|
//|					Return code:
//|						new z - value        if not blocked
//|						illegal_z == -128, if walk is blocked
//o------------------------------------------------------------------------------------------------o
SI08 CMovement::CalcWalk( CChar *c, SI16 x, SI16 y, SI16 oldx, SI16 oldy, SI08 oldz, bool justask, bool waterWalk )
{
	if( !ValidateObject( c ))
		return ILLEGAL_Z;

	const bool isGM		= IsGMBody( c );
	SI08 newz			= ILLEGAL_Z;
	SI08 charHeight = 16;
	UI16 xycount		= 0;
	UI08 worldNumber	= c->WorldNumber();
	UI16 instanceId		= c->GetInstanceId();
	bool landBlock = true;

	std::vector<Tile_st> xyblock;
	GetBlockingStatics( x, y, xyblock, xycount, worldNumber );
	GetBlockingDynamics( x, y, xyblock, xycount, worldNumber, instanceId );

	auto map	= Map->SeekMap( x, y, c->WorldNumber() );
	
	// Does landtile in target location block movement?
	landBlock = map.CheckFlag( TF_BLOCKING );
	
	// If it does, but it's WET and character can swim, it doesn't block!
	if( landBlock && waterWalk )
	{
		// Don't count the underwater coastal landtiles as blocking
		if( map.CheckFlag( TF_WET ) || ( map.terrainInfo->TextureId() >= 76 && map.terrainInfo->TextureId() <= 111 ))
		{
			landBlock = false;
		}
	}
	
	bool considerLand = Map->IsIgnored( map.tileId ); //Special case for a couple of land-tiles. Returns true if tile being checked equals one of those tiles.

	SI08 startTop = 0;
	SI08 startz = 0;
	SI08 landz, landCenter, landtop;

	// Calculate the average Z of landtile at target location
	GetAverageZ( c->WorldNumber(), x, y, landz, landCenter, landtop );

	// Calculate the character's starting height at source location
	GetStartZ( c->WorldNumber(), c, oldx, oldy, oldz, startz, startTop, c->GetInstanceId(), waterWalk );

	// Define the maximum height the character can step up to
	SI08 stepTop = startTop + 2;

	// Define the initial Z position of the character's position + height
	SI08 checkTop = startz + charHeight;

	// Assume move is NOT ok by default
	bool moveIsOk = false;

	// first calculate newZ value

	// Loop through all objects in the target location
	for( auto &tile : xyblock )
	{
		if(( !tile.CheckFlag( TF_BLOCKING ) && tile.CheckFlag( TF_SURFACE ) && !waterWalk ) || ( waterWalk && tile.CheckFlag( TF_WET )))
		{
			SI08 itemz = tile.altitude; // Object's current Z position
			SI08 itemTop = itemz;
			SI08 potentialNewZ = tile.top(); // Character's potential new Z position on top of object
			SI08 testTop = checkTop;

			if( moveIsOk )
			{
				SI08 cmp = std::abs( potentialNewZ - c->GetZ() ) - std::abs( newz - c->GetZ() );
				if( cmp > 0 || ( cmp == 0 && potentialNewZ > newz ))
					continue;
			}

			if( potentialNewZ + charHeight > testTop )
			{
				testTop = potentialNewZ + charHeight;
			}

			if( !tile.CheckFlag( TF_CLIMBABLE ))
			{
				itemTop += tile.height();
			}

			// Check if the character can step up onto the item at target location
			if( stepTop >= itemTop )
			{
				SI08 landCheck = itemz;

				if( tile.height() >= 2 )
				{
					landCheck += 2;
				}
				else
				{
					landCheck += tile.height();
				}

				if( !considerLand && landCheck < landCenter && landCenter > potentialNewZ && testTop > landz )
					continue;

				// Check if target location is OK and doesn't have any tiles on it with blocking flags within the range of character's potential new Z and the top of their head
				if( IsOk( xyblock, xycount, c->WorldNumber(), potentialNewZ, testTop, x, y, c->GetInstanceId(), isGM, waterWalk ))
				{
					newz = potentialNewZ;
					landCenter = potentialNewZ;
					moveIsOk = true;
				}
			}
		}
	}

	if( !considerLand && !landBlock && stepTop >= landz )
	{
		SI08 potentialNewZ = landCenter;
		SI08 testTop = checkTop;

		if( potentialNewZ + charHeight > testTop )
		{
			testTop = potentialNewZ + charHeight;
		}

		bool shouldCheck = true;

		if( moveIsOk )
		{
			SI08 cmp = abs( potentialNewZ - c->GetZ() ) - abs( newz - c->GetZ() );
			if( cmp > 0 || ( cmp == 0 && potentialNewZ > newz ))
			{
				shouldCheck = false;
			}
		}

		// Check if there are any blocking objects at the target landtile location
		if( shouldCheck && IsOk( xyblock, xycount, c->WorldNumber(), potentialNewZ, testTop, x, y, c->GetInstanceId(), isGM, waterWalk ))
		{
			newz = potentialNewZ;
			moveIsOk = true;
		}
	}

	if( !moveIsOk )
	{
		newz = ILLEGAL_Z;
	}

	return newz;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::CalcMove()
//|	Date		-	17.09.2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a character can walk in a specific direction from a given location
//|				-	This handles the funky diagonal moves.
//o------------------------------------------------------------------------------------------------o
bool CMovement::CalcMove( CChar *c, SI16 x, SI16 y, SI08 &z, UI08 dir)
{
	// NPCs that walk on land
	if( !cwmWorldState->creatures[c->GetId()].IsWater() )
	{
		// Is the character moving diagonally (NW, NE, SE, SW)? If so, we should check the nearby directions for blocking items too
		if(( dir & 0x07 ) % 2 )
		{
			// Check direction counter-clockwise to actual direction
			UI08 ndir = TurnCounterClockWise( dir );
			if( CalcWalk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true ) == ILLEGAL_Z && !cwmWorldState->creatures[c->GetId()].IsAmphibian() )
				return false;

			// Check direction clockwise to actual direction
			ndir = TurnClockWise( dir );
			if( CalcWalk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true ) == ILLEGAL_Z && !cwmWorldState->creatures[c->GetId()].IsAmphibian() )
				return false;
		}

		// Check actual direction character is moving
		z = CalcWalk( c, GetXfromDir( dir, x ), GetYfromDir( dir, y ), x, y, c->GetZ(), false );
	}

	// NPCs that can swim
	if( cwmWorldState->creatures[c->GetId()].IsWater() || ( cwmWorldState->creatures[c->GetId()].IsAmphibian() && z == ILLEGAL_Z ))
	{
		// Is the character moving diagonally (NW, NE, SE, SW)? If so, we should check the nearby directions for blocking items too
		if(( dir & 0x07 ) % 2 )
		{
			// Check direction counter-clockwise to actual direction
			UI08 ndir = TurnCounterClockWise( dir );
			if( CalcWalk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true, true ) == ILLEGAL_Z )
				return false;

			// Check direction clockwise to actual direction
			ndir = TurnClockWise( dir );
			if( CalcWalk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true, true ) == ILLEGAL_Z )
				return false;
		}

		// Check actual direction character is moving
		z = CalcWalk( c, GetXfromDir( dir, x ), GetYfromDir( dir, y ), x, y, c->GetZ(), false, true );
	}
	return ( z != ILLEGAL_Z );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::DenyMovement()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deny player movement and send character's previous location back to client
//o------------------------------------------------------------------------------------------------o
void CMovement::DenyMovement( CSocket *mSock, CChar *s, SI16 sequence )
{
	CPWalkDeny denPack;

	denPack.SequenceNumber( static_cast<SI08>( sequence ));
	denPack.X( s->GetX() );
	denPack.Y( s->GetY() );
	denPack.Direction( s->GetDir() );
	denPack.Z( s->GetZ() );

	if( mSock != nullptr )
	{
		mSock->Send( &denPack );
		mSock->WalkSequence( -1 );
	}
}

bool operator == ( const NodeFCost_st& x, const NodeFCost_st& y )
{
	return ( x.fCost == y.fCost );
}

bool operator < ( const NodeFCost_st& x, const NodeFCost_st& y )
{
	return ( x.fCost < y.fCost );
}

bool operator > ( const NodeFCost_st& x, const NodeFCost_st& y )
{
	return ( x.fCost > y.fCost );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement:: PFGrabNodes()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate nodes from source to target location for advanced pathfinding
//o------------------------------------------------------------------------------------------------o
bool CMovement::PFGrabNodes( CChar *mChar, UI16 targX, UI16 targY, UI16 curX, UI16 curY, SI08 curZ,
							UI32 parentSer, std::map<UI32, PfNode_st>& openList, 
							std::map<UI32, UI32>& closedList, std::deque<NodeFCost_st>& fCostList )
{
	std::map<UI32, bool> blockList;
	blockList.clear();

	bool waterWalk = cwmWorldState->creatures[mChar->GetId()].IsWater();
	bool amphibianWalk = cwmWorldState->creatures[mChar->GetId()].IsAmphibian();

	SI08 newZ = ILLEGAL_Z;
	for( SI08 xOff = -1; xOff < 2; ++xOff )
	{
		SI16 checkX = curX + xOff;
		for( SI08 yOff = -1; yOff < 2; ++yOff )
		{
			if( !yOff && !xOff )
				continue;

			SI16 checkY = curY + yOff;
			UI32 locSer = (checkY + ( checkX << 16 ));

			if( amphibianWalk || !waterWalk )
			{
				newZ = CalcWalk( mChar, checkX, checkY, curX, curY, curZ, false );
			}
			if( waterWalk || ( amphibianWalk && newZ == ILLEGAL_Z ))
			{
				newZ = CalcWalk( mChar, checkX, checkY, curX, curY, curZ, false, true );
			}
			if( ILLEGAL_Z == newZ )
			{
				blockList[locSer] = true;
				continue;
			}

			//if( blockList.find( locSer ) != blockList.end() )
			//	continue;

			// Let's make this more expensive by checking for potential blocking characters as well!
			if(( checkX != targX || checkY != targY ) && CheckForCharacterAtXYZ( mChar, checkX, checkY, curZ ))
			{
				blockList[locSer] = true;
				continue;
			}

			// Don't Cut Corners
			bool cornerBlocked = false;
			if( xOff != 0 && yOff != 0 )
			{
				UI32 check1Ser = ( checkY + ( curX << 16 ));
				UI32 check2Ser = ( curY + ( checkX << 16 ));
				if( blockList.find( check1Ser ) != blockList.end() || blockList.find( check2Ser ) != blockList.end() )
				{
					cornerBlocked = true;
				}
				else
				{
					if( CalcWalk( mChar, curX, checkY, curX, curY, curZ, true ) == ILLEGAL_Z )
					{
						cornerBlocked = true;
						blockList[check1Ser] = true;
					}
					else if( CalcWalk( mChar, checkX, curY, curX, curY, curZ, true ) == ILLEGAL_Z )
					{
						cornerBlocked = true;
						blockList[check2Ser] = true;
					}
				}
			}
			if( cornerBlocked )
				continue;

			if( checkX == targX && checkY == targY )
				return true;

			UI08 gCost = 10;
			if( xOff && yOff )
			{
				gCost = 14;
			}

			std::map<UI32, PfNode_st>::const_iterator olIter;
			olIter = openList.find( locSer );
			if( olIter != openList.end() )
			{
				PfNode_st mNode = olIter->second;
				if( mNode.gCost > gCost )
				{
					for( std::deque< NodeFCost_st >::iterator fcIter = fCostList.begin(); fcIter != fCostList.end(); ++fcIter )
					{
						if(( *fcIter ).xySer == locSer )
						{
							( *fcIter ).fCost = mNode.hCost + gCost;
							break;
						}
					}
					mNode.z			= newZ;
					mNode.parent	= parentSer;
					mNode.gCost		= gCost;
					std::sort( fCostList.begin(), fCostList.end() );
				}
			}
			else if( closedList.find( locSer ) == closedList.end() )
			{
				UI16 hCost = 10 * ( abs( checkX - targX) + abs( checkY - targY ));
				UI16 fCost = gCost + hCost;

				openList[locSer] = PfNode_st( hCost, gCost, parentSer, newZ );
				fCostList.push_back( NodeFCost_st( fCost, locSer ));
				std::sort( fCostList.begin(), fCostList.end() );
			}
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMovement::AdvancedPathfinding()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle the advanced variant of NPC pathfinding
//|					Enabled/Disabled throuugh UOX.INI setting - ADVANCEDPATHFINDING=0/1
//o------------------------------------------------------------------------------------------------o
bool CMovement::AdvancedPathfinding( CChar *mChar, UI16 targX, UI16 targY, bool willRun, UI16 maxSteps )
{
	UI16 startX			= mChar->GetX();
	UI16 startY			= mChar->GetY();
	UI16 curX			= mChar->GetX();
	UI16 curY			= mChar->GetY();
	SI08 curZ			= mChar->GetZ();;
	UI08 oldDir			= mChar->GetDir();
	UI16 loopCtr		= 0;
	EVENT_TIMER( mytimer, EVENT_TIMER_OFF );
	// Set target location in NPC's mind
	mChar->SetPathTargX( targX );
	mChar->SetPathTargY( targY );
	EVENT_TIMER_RESET( mytimer );

	// If no maxSteps was provided, set appropriate value based on current scenario
	if( maxSteps == 0 )
	{
		SI08 npcWanderType = mChar->GetNpcWander();
		if( mChar->IsAtWar() )
		{
			if( GetDist( mChar->GetLocation(), Point3_st( targX, targY, curZ )) >= 30 )
			{
				maxSteps = 150;
			}
			else
			{
				maxSteps = 50;
			}
		}
		else
		{
			if( npcWanderType == WT_FREE || npcWanderType == WT_BOX || npcWanderType == WT_CIRCLE )
			{
				if( mChar->IsEvading() ) // If they are evading, they might be attempting to move back to original wanderZone
				{
					maxSteps = 75;
				}
				else
				{
					maxSteps = 2; // Out of combat wandering
				}
			}
			else if( npcWanderType == WT_FLEE )
			{
				maxSteps = 37;
			}
			else if( npcWanderType == WT_FOLLOW )
			{
				maxSteps = 50;
			}
			else
			{
				maxSteps = 250;
			}
		}
	}

	std::map<UI32, PfNode_st>	openList;
	std::map<UI32, UI32>	closedList;
	std::deque<NodeFCost_st>	fCostList;

	openList.clear();
	closedList.clear();
	fCostList.resize( 0 );

	UI32 parentSer			= ( curY + ( curX << 16 ));
	openList[parentSer]		= PfNode_st();
	openList[parentSer].z	= curZ;
	fCostList.push_back( NodeFCost_st( 0, parentSer ));
	while(( curX != targX || curY != targY ) && loopCtr < maxSteps )
	{
		parentSer = fCostList[0].xySer;
		curX = static_cast<UI16>( parentSer >> 16 );
		curY = static_cast<UI16>( parentSer % 65536 );
		curZ = openList[parentSer].z;

		closedList[parentSer] = static_cast<UI32>( openList[parentSer].parent );
		openList.erase( openList.find( parentSer ));
		fCostList.pop_front();

		if( PFGrabNodes( mChar, targX, targY, curX, curY, curZ, parentSer, openList, closedList, fCostList ))
		{
			while( parentSer != 0 )
			{
				UI08 newDir = Direction( curX, curY, targX, targY );

				if( willRun )
				{
					newDir |= 0x80;
				}

				if( oldDir != UNKNOWNDIR && oldDir != newDir )
				{
					mChar->PushDirection( newDir, true );	// NPC's need to "walk" twice when turning
				}
				mChar->PushDirection( newDir, true );

				oldDir		= newDir;
				targX		= curX;
				targY		= curY;
				parentSer 	= closedList[parentSer];
				curX		= static_cast<UI16>( parentSer >> 16 );
				curY		= static_cast<UI16>( parentSer % 65536 );
			}
			break;
		}
		else if( fCostList.empty() )
		{
			break;
		}
		++loopCtr;
	}
	if( loopCtr == maxSteps )
	{
#if defined( UOX_DEBUG_MODE )
		std::string charName = GetNpcDictName( mChar );
		Console.Warning( oldstrutil::format( "AdvancedPathfinding: NPC (%s at %i %i %i %i) unable to find a path, max steps limit (%i) reached, aborting.\n",
			charName.c_str(), mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber(), maxSteps ));
#endif
		mChar->SetPathResult( -1 ); // Pathfinding failed
		EVENT_TIMER_NOW( mytimer, Time when loopCtr == maxSteps, 1 );
		return false;
	}
	else if( loopCtr == 0 && GetDist( mChar->GetLocation(), Point3_st( targX, targY, curZ )) > 1 )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Warning( "AdvancedPathfinding: Unable to pathfind beyond 0 steps, aborting.\n" );
#endif
		mChar->SetPathResult( -1 ); // Pathfinding failed
		EVENT_TIMER_NOW( mytimer, Time when loopCtr == 0 and distance > 1, 1 );
		return false;
	}
	else if( mChar->GetX() == startX && mChar->GetY() == startY && GetDist( mChar->GetLocation(), Point3_st( targX, targY, curZ )) > 1 )
	{
		// NPC never moved, and target location is not nearby
		mChar->SetPathResult( -1 ); // Pathfinding failed
		EVENT_TIMER_NOW( mytimer, Time when NPC never moved, 1 );

		return false;
	}
	else
	{
#if defined( UOX_DEBUG_MODE )
		Console.Print( oldstrutil::format( "AdvancedPathfinding: %u loops to find path.\n", loopCtr ));
#endif
		if( GetDist( mChar->GetLocation(), Point3_st( targX, targY, curZ )) > 1 )
		{
			mChar->SetPathResult( 0 ); // Partial pathfinding success
		}
		else
		{
			mChar->SetPathResult( 1 ); // Pathfinding success
		}
	}
	EVENT_TIMER_NOW( mytimer, Time when total pathfinding used, 1 );
	return true;
}
