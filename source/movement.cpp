//------------------------------------------------------------------------
//  walking.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
// Thyme 2000.09.21
// This is my attempt to rewriting the walking code. I'm going to take the code and documentation
// that others before me have used and incorporate my little (big?) fixes and comments.

// Many thanks to all of the previous contributors, and I hope these changes help out.
// Lord Binary
// Morrolan
// Anti-Christ
// fur            : 1999.10.27 - rewrite of walking.cpp with documentation!
//                : 1999.10.27 - ripped apart walking into smaller functions
// Tauriel        : 1999.03.06 - For all of the region stuff
// knoxos         : 2000.08.?? - For finally making use of the flags, and height blocking

// TODO:
// Modify path struct to use only directions, not store x,y
// Add running support in PathFind
// Modify flying

// An explination of each function precedes it in the code. Hope I can provide ample enough
// information to help out the future bug fixers. I will try to explain EVERY bit of logic
// behind what I've done to make things easier.

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

namespace UOX
{

cMovement *Movement;

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
#define MAX_ITEM_Z_INFLUENCE	10		// Any item above this height is discarded as being too far away to effect the char
#define MAX_Z_LEVITATE			10		// Maximum total height to reach a tile marked as 'LEVITATABLE'
										// Items with a mark as climbable have no height limit

inline UI08 turn_clock_wise( UI08 dir )
{
	UI08 t = ((dir & 0x07 ) + 1) % 8;
	return (UI08)(dir & 0x80) ? ( t | 0x80) : t;
}

inline UI08 turn_counter_clock_wise( UI08 dir )
{
	UI08 t = (dir - 1) & 7;
	return (UI08)(dir & 0x80) ? ( t | 0x80) : t;
}


//o-------------------------------------------------------------o
//| Function : calcTileHeight
//| Author   : knoxos
//o-------------------------------------------------------------o
//| Description : 
//|   Out of some strange reason the tile height seems
//|   to be an absolute value of a two's complement of 
//|   the last four bit. Don't know if it has a special
//|   meaning if the tile is height is "negative"
//|
//|   (stairs in despise blocking bug)
//| Arguments :
//|   int h   orignial height as safed in mul file
//|
//| Return code:
//|   The absoulte value of the two's complement if the
//|   the value was "negative"
//o-------------------------------------------------------------o
inline SI08 calcTileHeight( SI08 h )
{
	return (SI08)((h & 0x8) ? ((h & 0xF) >> 1) : h & 0xF);
}

/*
** Walking() This function is called whenever we get a message from the client
** to walk/run somewhere.   It is also called by the NPC movement functions in this
** class to make the NPCs move.  The arguments are fairly fixed because we don't
** have a lot of control about what the client gives us.
**
** CHARACTER s - Obviously the character index of the character trying to move.
**
** dir - Which direction the character is trying to move. The first nibble holds
** the cardinal direction.      If the bit 0x80 is set, it means the character is
** running instead of walking.  
**              0: // North
**              1: // Northeast
**              2: // East
**              3: // Southeast
**              4: // South
**              5: // Southwest
**              6: // West
**              7: // Northwest
**
** sequence - This is what point in the walking sequence we are at, this seems to
**            roll over once it hits 256
**
*/

//o---------------------------------------------------------------------------o
//|	Function	-	void teleporters( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	If character is on a teleport location, teleport him
//o---------------------------------------------------------------------------o
void HandleTeleporters( CChar *s )
{
	if( !ValidateObject( s ) )
		return;
	UI08 charWorld					= s->WorldNumber();
	CTeleLocationEntry *getTeleLoc	= NULL;
	bool isOnTeleporter;
	for( size_t i = 0; i < cwmWorldState->teleLocs.size(); ++i )
	{
		isOnTeleporter	= false;
		getTeleLoc		= &cwmWorldState->teleLocs[i];
		if( getTeleLoc == NULL )
			continue;
		if( getTeleLoc->SourceWorld() == 0xFF || getTeleLoc->SourceWorld() == charWorld )
		{
			if( getTeleLoc->SourceLocation().z != ILLEGAL_Z )
				isOnTeleporter = ( getTeleLoc->SourceLocation() == s->GetLocation() );
			else
				isOnTeleporter = ( getTeleLoc->SourceLocation().x == s->GetX() && getTeleLoc->SourceLocation().y == s->GetY() );
			if( isOnTeleporter )
			{
				s->SetLocation( (SI16)getTeleLoc->TargetLocation().x, (SI16)getTeleLoc->TargetLocation().y, (UI08)getTeleLoc->TargetLocation().z, getTeleLoc->TargetWorld() );
				if( !s->IsNpc() )
				{
					if( getTeleLoc->TargetWorld() != charWorld )
						SendMapChange( getTeleLoc->TargetWorld(), s->GetSocket() );

					CDataList< CChar * > *myPets = s->GetPetList();
					for( CChar *myPet = myPets->First(); !myPets->Finished(); myPet = myPets->Next() )
					{
						if( !ValidateObject( myPet ) )
							continue;
						if( !myPet->GetMounted() && myPet->IsNpc() && myPet->GetOwnerObj() == s )
						{
							if( objInOldRange( s, myPet, DIST_INRANGE ) )
								myPet->SetLocation( s );
						}
					}
				}
				break;
			}
			else if( s->GetX() < getTeleLoc->SourceLocation().x )
				break;
		}
	}
}

void checkRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight );
void cMovement::Walking( CSocket *mSock, CChar *c, UI08 dir, SI16 sequence )
{
	// sometimes the NPC movement code comes up with -1, for example, if we are following someone
	// and we are directly on top of them
	// ^ That's because chardir returns -1 when direction is the same.  Actually, this
	// should only happen when a player walks on top of an NPC and the NPC tries to move.

	if( !isValidDirection( dir ) )
	{
		Console.Error( "%s (cMovement::Walking) caught bad direction = %s %d 0x%x\n", DBGFILE, c->GetName().c_str(), dir, dir );
		// If I don't do this, the NPC will keep trying to walk on the same step, which is
		// where he's already at. Can cause an infinite loop. (Trust me, was one of the things
		// that locked up NW Alpha 2)
		// Will call something like NPCRandomMove in the future.
		if( c->IsNpc() )
			c->FlushPath();
		return;
	}

    if( !VerifySequence( c, mSock, sequence ) )
        return;

	// If checking for weight is more expensive, shouldn't we check for frozen first?
	if( isFrozen( c, mSock, sequence ) )
		return;
    
    if( isOverloaded( c, mSock, sequence ) )
        return;
    
	// save our original location before we even think about moving
	const SI16 oldx = c->GetX();
	const SI16 oldy = c->GetY();

	// see if we have stopped to turn or if we are moving
	const bool amTurning = ( (dir&0x07) != c->GetDir() );
	if( !amTurning )
	{
		if( !CheckForRunning( c, dir ) )
			return;
		
		if( !CheckForStealth( c ) )
			return;
		
		SI08 myz = ILLEGAL_Z;
		const SI16 myx = GetXfromDir( dir, oldx );
		const SI16 myy = GetYfromDir( dir, oldy );
		if( !calc_move( c, oldx, oldy, myz, dir ) )
		{
#if DEBUG_WALKING
			Console.Print( "DEBUG: %s (cMovement::Walking) Character Walk Failed for %s\n", DBGFILE, c->GetName() );
			Console.Print( "DEBUG: %s (cMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, oldx, oldy, c->GetZ() );
			Console.Print( "DEBUG: %s (cMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz );
#endif
			if( mSock != NULL )
				deny( mSock, c, sequence );
			if( c->IsNpc() )
				c->FlushPath();
			return;
		}
#if DEBUG_WALKING
		Console.Print( "DEBUG: %s (cMovement::Walking) Character Walk Passed for %s\n", DBGFILE, c->GetName() );
		Console.Print( "DEBUG: %s (cMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, oldx, oldy, c->GetZ() );
		Console.Print( "DEBUG: %s (cMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz );
#endif

		if( c->IsNpc() && CheckForCharacterAtXYZ( c, myx, myy, myz ) )
		{
			c->FlushPath();
			return;
		}

		MoveCharForDirection( c, myx, myy, myz );

		// i actually moved this for now after the z =  illegal_z, in the end of CrazyXYBlockStuff()
		// can't see how that would hurt anything
		if( !CheckForHouseBan( c, mSock ) )
			return;

		//	
		//	 OK AT THIS POINT IT IS NOW OFFICIALLY A LEGAL MOVE TO MAKE, LETS GO FOR IT!
		//	
		//	 That means any bugs concerning if a move was legal must be before this point!
		//	

		// i moved this down after we are certain we are moving
		if( cwmWorldState->ServerData()->AmbientFootsteps() )
  			Effects->playTileSound( mSock );
	}
	
	// do all of the following regardless of whether turning or moving i guess
	
	// set the player direction to contain only the cardinal direction bits
	c->WalkDir( (dir&0x07) );

	SendWalkToPlayer( c, mSock, sequence );
	SendWalkToOtherPlayers( c, dir, oldx, oldy );
	
	// i'm going ahead and optimizing this, if you haven't really moved, should be
	// no need to check for teleporters and the weather shouldn't change
	if( !amTurning )
	{
		OutputShoveMessage( c, mSock );

		HandleItemCollision( c, mSock, oldx, oldy );
		HandleTeleporters( c );
		CheckCharInsideBuilding( c, mSock, true);
		checkRegion( mSock, (*c), false );
	}
}

// Function      : cMovement::isValidDirection()
// Written by    : Unknown
// Revised by    : Thyme
// Revision Date : 2000.09.21
// Purpose       : Check if a given direction is valid
// Method        : Return true on the below values:
//
// Direction   Walking Value   Running Value
// North          0 0x00         128 0x80
// Northeast      1 0x01         129 0x81
// East           2 0x02         130 0x82
// Southeast      3 0x03         131 0x83
// South          4 0x04         132 0x84
// Southwest      5 0x05         133 0x85
// West           6 0x06         134 0x86
// Northwest      7 0x07         135 0x87

bool cMovement::isValidDirection( UI08 dir )
{
	return ( dir == ( dir & 0x87 ) );
}

// Function      : cMovement::isFrozen()
// Written by    : Unknown
// Revised by    : Thyme
// Revision Date : 2000.09.21
// Purpose       : Check if a character is frozen or casting a spell
// Method        : Because of the way the source uses the frozen flag, I decided to change
// something in how this works. If the character is casting a spell (chars.casting is true)
// OR if they're frozen (chars.priv2 & FROZEN_BIT) then they can't walk. Why? If a player/npc
// has their frozen bit set, and they cast a spell, they will lose their frozen bit at the
// end of the spell cast. With this new check, we don't even need to set the frozen bit when
// casting a spell!

bool cMovement::isFrozen( CChar *c, CSocket *mSock, SI16 sequence )
{
	if( c->IsCasting() )
	{
		if( mSock != NULL )
		{
			mSock->sysmessage( 1380 );
			deny( mSock, c, sequence );
		}
#if DEBUG_WALKING
		Console.Print( "DEBUG: %s (cMovement::isFrozen) casting char %s\n", DBGFILE, c->GetName() );
#endif
		return true;
	}
	if( c->IsFrozen() )
	{
		if( mSock != NULL )
		{
			mSock->sysmessage( 1381 );
			deny( mSock, c, sequence );
		}
#if DEBUG_WALKING
		Console.Print( "DEBUG: %s (cMovement::isFrozen) frozen char %s\n", DBGFILE, c->GetName() );
#endif
		return true;
	} 
	return false;
}

// Thyme 07/28/00

// return TRUE is character is overloaded (with weight)
// return FALSE otherwise

// CheckForWeight was confusing...

// Old code called check weight first then checked socket... I changed it the other way.
// Why, well odds are (I may be wrong) if you don't have a socket, you're an NPC and if you
// have one, you're a character. We said in the first line that we didn't want to restrict
// based upon NPC, so if you're an NPC, the socket/checkweight will never be called anyway.

// Rewrote to deny the client... We'll see if it works.

bool cMovement::isOverloaded( CChar *c, CSocket *mSock, SI16 sequence )
{
	// Who are we going to check for weight restrictions?
	if( !c->IsDead() && !c->IsNpc() && c->GetCommandLevel() < CL_CNS )
	{
		if( mSock != NULL )
		{
			if( Weight->isOverloaded( c ) )
				c->IncStamina( -5 );
			if( c->GetStamina() <= 0 )
			{
				c->SetStamina( 0 );
				mSock->sysmessage( 1382 );
				deny( mSock, c, sequence );
#if DEBUG_WALKING
				Console.Print( "DEBUG: %s (cMovement::Walking) overloaded char %s\n", DBGFILE, c->GetName() );
#endif
				return true;
			}
		}
	}
	return false;
}
// Thyme 07/28/00

// Here's how I'm going to use it for now.  Movement Type may be used for races, that's why
// I put it as an integer.  Here are the values I'm going to use:
// BTW, bird doesn't work right now! These values should be scripted, as well, but I want
// to test walking before another var is added to chars struct.
// GM Body  0x01
// Player   0x02
// Bird     0x20 (basically, a fish and an NPC, so I could use 0xc0, but I don't wanna)
// NPC      0x40
// Fish     0x80 (So they can swim!)
// I left a gap between Player and NPC because someone may want to implement race
// restrictions... 

// Examins a location (cx, cy, cz) to determine if a character is found there.  If so, return true
// If not, return false.  Used for the npc stacking problem
bool cMovement::CheckForCharacterAtXYZ( CChar *c, SI16 cx, SI16 cy, SI08 cz )
{
	CMapRegion *MapArea = MapRegion->GetMapRegion( MapRegion->GetGridX( cx ), MapRegion->GetGridY( cy ), c->WorldNumber() );	// check 3 cols... do we really NEED to?
	if( MapArea == NULL )	// no valid region
		return false;
	CDataList< CChar * > *regChars = MapArea->GetCharList();
	regChars->Push();
	for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
	{
		if( !ValidateObject( tempChar ) )
			continue;
		if( tempChar != c && ( ( isOnline( (*tempChar) ) && !tempChar->IsDead() ) || tempChar->IsNpc() ) )
		{	// x=x,y=y, and distance btw z's <= MAX STEP
			if( tempChar->GetX() == cx && tempChar->GetY() == cy && tempChar->GetZ() >= cz && tempChar->GetZ() <= (cz + 5) )	// 2 people will still bump into each other, if slightly offset
			{
				regChars->Pop();	// restore before returning
				return true;
			}
		}
	}
	regChars->Pop();
	return false;
}

// if we have a valid socket, see if we need to deny the movement request because of
// something to do with the walk sequence being out of sync.
bool cMovement::VerifySequence( CChar *c, CSocket *mSock, SI16 sequence )
{
    if( mSock != NULL )
    {
        if( mSock->WalkSequence() + 1 != sequence && sequence != 256 )
        {
            deny( mSock, c, sequence );  
            return false;
        }
        
    }
    return true;
}

bool cMovement::CheckForRunning( CChar *c, UI08 dir )
// returns true if updatechar required, or false if not
{
	// if we are running
	if( dir&0x80 )
	{
		// if we are using stealth
		if( c->GetStealth() != -1 )	// Stealth - stop hiding if player runs
			c->ExposeToView();
		//Don't regenerate stamina while running
		c->SetRegen( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_STAMINAREGEN ), 1 );
		c->SetRunning( c->GetRunning() + 1 );

		if( !c->IsDead() && c->GetCommandLevel() < CL_CNS )
		{
			bool reduceStamina = ( c->IsOnHorse() && c->GetRunning() > ( cwmWorldState->ServerData()->MaxStaminaMovement() * 2 ) );
			if( !reduceStamina )
				reduceStamina = ( c->GetRunning() > ( cwmWorldState->ServerData()->MaxStaminaMovement() * 4 ) );
			if( reduceStamina )
			{
				c->SetRunning( 1 );
				c->SetStamina( c->GetStamina() - 1 );
			}
		}
		if( c->IsAtWar() && ValidateObject( c->GetTarg() ) )
			if( c->GetTarg()->GetNpcWander() != WT_FLEE )
				c->SetTimer( tCHAR_TIMEOUT, BuildTimeValue( 2 ) );

	}
	else
	{
		c->SetRunning( 0 );
	}
	return true;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool cMovement::CheckForStealth( CChar *c )
//|	Date			-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Check to see if character is in stealth mode. 
//|									
//|	Modification	-	09/22/2002	-	Xuri - Unhide characters who are mounted
//|									while trying to stealth.
//o--------------------------------------------------------------------------o
//|	Returns				- Always returns true it seems
//o--------------------------------------------------------------------------o	
bool cMovement::CheckForStealth( CChar *c )
{
	if( c->GetVisible() == VT_TEMPHIDDEN || c->GetVisible() == VT_INVISIBLE )
	{
		// Sept 22, 2002 - Xuri
		if( c->IsOnHorse() )
		{
			if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
				c->ExposeToView();
		}
		//
		if( c->GetStealth() != -1 )
		{
			c->SetStealth( c->GetStealth() + 1 );
			if( c->GetStealth() > ( cwmWorldState->ServerData()->MaxStealthMovement() * c->GetSkill( STEALTH ) / 1000 ) )
				c->ExposeToView();
		}
		else
			c->ExposeToView();
	}
	return true;
}

// see if a player has tried to move into a house they were banned from
bool cMovement::CheckForHouseBan( CChar *c, CSocket *mSock )
{
	CMultiObj *house = findMulti( c );
	if( ValidateObject( house ) ) 
	{
		if( c->GetMultiObj() != house )
			c->SetMulti( house );//Set them inside the multi!
		if( house->IsOnBanList( c ) )
		{
			SI16 sx, sy, ex, ey;
			Map->MultiArea( house, sx, sy, ex, ey );
			c->SetLocation( ex, ey + 1, c->GetZ() );
			if( mSock != NULL )
			{
				mSock->sysmessage( 1639 );
				mSock->WalkSequence( -1 );
			}
			return false;
		}
	}
	else
	{
		if( c->GetMultiObj() != NULL )
			c->SetMulti( INVALIDSERIAL );
	}
	return true;
}

// Thyme 2000.09.21
// I already made sure I could move there (even the crazy XY block stuff) so this IS a valid move. Just move the
// directions. Oh, and since I we already have the GetX/YfromDir functions (and we need those) why don't we just
// use them here?
void cMovement::MoveCharForDirection( CChar *c, SI16 newX, SI16 newY, SI08 newZ )
{
	if( !c->IsNpc() ) 
	{	// if we're a PC in combat, or casting, we want to break/adjust their timers
		const bool casting = (c->IsCasting() || c->IsJSCasting());
		if( ( c->IsAtWar() || casting ) && isOnline( *c ) )
		{	// if it's not an NPC, in combat or casting, and it's online
			if( casting )
			{
				Effects->PlayStaticAnimation( c, 0x3735, 0, 30 );
				Effects->PlaySound( c, 0x005C );
				c->TextMessage( c->GetSocket(), 771, EMOTE, false );
				c->StopSpell();
				c->SetJSCasting( false );
			}
			else
			{	// otherwise, we're at war!!!  Are we using a bow?
				CItem *mWeapon				= Combat->getWeapon( c );
				const UI08 getFightSkill	= Combat->getCombatSkill( mWeapon );
				if( getFightSkill == ARCHERY )
					c->SetTimer( tCHAR_TIMEOUT, BuildTimeValue( Combat->GetCombatTimeout( c ) ) );
			}
		}
	}
	MapRegion->ChangeRegion( c, newX, newY, c->WorldNumber() );
	c->WalkXY( newX, newY );
	c->WalkZ( newZ );
}

void cMovement::GetBlockingMap( SI16 x, SI16 y, CTileUni *xyblock, UI16 &xycount, SI16 oldx, SI16 oldy, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;
	const SI08 mapz = Map->MapElevation( x, y, worldNumber );
	if( mapz != ILLEGAL_Z )
	{
		const map_st map	= Map->SeekMap( x, y, worldNumber );
		CLand& land			= Map->SeekLand( map.id );

		xyblock[xycount].Flags( land.Flags() );
		xyblock[xycount].Type( 0 );
		xyblock[xycount].BaseZ( mapz );
		xyblock[xycount].Top( mapz );
		++xycount;
	}
}

void cMovement::GetBlockingStatics( SI16 x, SI16 y, CTileUni *xyblock, UI16 &xycount, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;

	CStaticIterator msi( x, y, worldNumber );
	for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
	{
		CTile& tile = Map->SeekTile( stat->itemid );
		xyblock[xycount].Type( 2 );
		xyblock[xycount].BaseZ( stat->zoff );
		xyblock[xycount].Top( stat->zoff + calcTileHeight( tile.Height() ) );
		xyblock[xycount].Flags( tile.Flags() );
		++xycount;
		if( xycount >= XYMAX )	// don't overflow
			break;
	}
}

void cMovement::GetBlockingDynamics( SI16 x, SI16 y, CTileUni *xyblock, UI16 &xycount, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;
	REGIONLIST nearbyRegions = MapRegion->PopulateList( x, y, worldNumber );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *tItem = regItems->First(); !regItems->Finished(); tItem = regItems->Next() )
		{
			if( !ValidateObject( tItem ) )
				continue;
			if( !tItem->CanBeObjType( OT_MULTI ) )
			{
	#if DEBUG_WALKING
				Console.Print( "DEBUG: Item X: %i\nItem Y: %i\n", tItem->GetX(), tItem->GetY() );
	#endif
				if( tItem->GetX() == x && tItem->GetY() == y )
				{
					CTile& tile = Map->SeekTile( tItem->GetID() );
					xyblock[xycount].Type( 1 );
					xyblock[xycount].BaseZ( tItem->GetZ() );
					xyblock[xycount].Top( tItem->GetZ() + calcTileHeight( tile.Height() ) );
					xyblock[xycount].Flags( tile.Flags() );
					++xycount;
					if( xycount >= XYMAX )	// don't overflow
					{
						regItems->Pop();
						return;
					}
				}
			}
			else if( abs( tItem->GetX() - x ) <= DIST_BUILDRANGE && abs( tItem->GetY() - y) <= DIST_BUILDRANGE )
			{	// implication, is, this is now a CMultiObj
				const UI16 multiID = (tItem->GetID() - 0x4000);
				SI32 length = Map->SeekMulti( multiID );
				if( length == -1 || length >= 17000000 ) //Too big... bug fix hopefully (Abaddon 13 Sept 1999)
				{
					Console.Error( "Walking() - Bad length in multi file. Avoiding stall" );
					const map_st map1 = Map->SeekMap( tItem->GetX(), tItem->GetY(), tItem->WorldNumber() );
					CLand& land = Map->SeekLand( map1.id );
					if( land.CheckFlag( TF_WET ) ) // is it water?
						tItem->SetID( 0x4001 );
					else
						tItem->SetID( 0x4064 );
					length = 0;
				}
				for( SI32 j = 0; j < length; ++j )
				{
					const Multi_st& multi = Map->SeekIntoMulti( multiID, j );
					if( multi.visible && (tItem->GetX() + multi.x) == x && (tItem->GetY() + multi.y) == y )
					{
						CTile& tile = Map->SeekTile( multi.tile );
						xyblock[xycount].Type( 2 );
						xyblock[xycount].BaseZ( multi.z + tItem->GetZ() );
						xyblock[xycount].Top( multi.z + tItem->GetZ() + calcTileHeight( tile.Height() ) );
						xyblock[xycount].Flags( tile.Flags() );
						++xycount;
						if( xycount >= XYMAX )	// don't overflow
						{
							regItems->Pop();
							return;
						}
					}
				}
			}
		}
		regItems->Pop();
	}
}

// actually send the walk command back to the player and increment the sequence
void cMovement::SendWalkToPlayer( CChar *c, CSocket *mSock, SI16 sequence )
{
	if( mSock != NULL )
	{
		CPWalkOK toSend;

		toSend.SequenceNumber( mSock->GetByte( 2 ) );
		if( c->GetVisible() != VT_VISIBLE )
			toSend.FlagColour( 0 );
		else
			toSend.FlagColour( static_cast<UI08>(c->FlagColour( c )) );

		mSock->Send( &toSend );
		mSock->WalkSequence( sequence );
		if( mSock->WalkSequence() == 255 )
			mSock->WalkSequence( 1 );
	}
}

void cMovement::SendWalkToOtherPlayers( CChar *c, UI08 dir, SI16 oldx, SI16 oldy )
{
	// lets cache these vars in advance
	const SI16 newx		= c->GetX();
	const SI16 newy		= c->GetY();

	bool checkX			= (oldx != newx);
	bool checkY			= (oldy != newy);
	UI16 effRange;
	UI08 worldnumber	= c->WorldNumber();

	CPExtMove toSend	= (*c);

	Network->PushConn();
	for( CSocket *tSend = Network->FirstSocket(); !Network->FinishedSockets(); tSend = Network->NextSocket() )
	{	// lets see, its much cheaper to call perm[i] first so i'm reordering this
		if( tSend == NULL )
			continue;
		CChar *mChar = tSend->CurrcharObj();
		if( !ValidateObject( mChar ) )
			continue;
		if( worldnumber != mChar->WorldNumber() )
			continue;

		effRange = static_cast<UI16>( tSend->Range() );
		const UI16 visibleRange = static_cast<UI16>( tSend->Range() + Races->VisRange( mChar->GetRace() ));
		if( visibleRange >= effRange )
			effRange = visibleRange;

		if( c != mChar )
		{	// We need to ensure they are within range of our X AND Y location regardless of how they moved.
			const UI16 dxNew = static_cast<UI16>(abs( newx - mChar->GetX() ));
			const UI16 dyNew = static_cast<UI16>(abs( newy - mChar->GetY() ));
			if( checkX && dyNew <= (effRange+2) )	// Only check X Plane if their x changed
			{
				UI16 dxOld = static_cast<UI16>(abs( oldx - mChar->GetX() ));
				if( ( dxNew == effRange ) && ( dxOld > effRange ) )
				{
					c->SendToSocket( tSend );
					continue;	// Incase they moved diagonally, lets not update the same character multiple times
				}
				else if( ( dxNew > (effRange+1) ) && ( dxOld == (effRange+1) ) )
				{
					c->RemoveFromSight( tSend );
					continue;	// Incase they moved diagonally, lets not update the same character multiple times
				}
			}
			if( checkY && dxNew <= (effRange+2) )	// Only check Y plane if their y changed
			{
				UI16 dyOld = static_cast<UI16>(abs( oldy - mChar->GetY() ));
				if( ( dyNew == effRange ) && ( dyOld > effRange ) )
				{
					c->SendToSocket( tSend );
					continue;
				}
				else if( ( dyNew > (effRange+1) ) && ( dyOld == (effRange+1) ) )
				{
					c->RemoveFromSight( tSend );
					continue;
				}
			}
			toSend.FlagColour( static_cast<UI08>(c->FlagColour( mChar )) );
			tSend->Send( &toSend );
		}
	}
	Network->PopConn();
}

// see if we should mention that we shove something out of the way
void cMovement::OutputShoveMessage( CChar *c, CSocket *mSock )
{
	if( mSock == NULL )
		return;

	// GMs, counselors, and ghosts don't shove things
	if( c->GetCommandLevel() >= CL_CNS || IsGMBody( c ) )
		return;
	// lets cache these vars in advance
	CMapRegion *grid = MapRegion->GetMapRegion( c );
	if( grid == NULL )
		return;

	CDataList< CChar * > *regChars = grid->GetCharList();
	regChars->Push();
	bool didShove			= false;
	const SI16 x			= c->GetX();
	const SI16 y			= c->GetY();
	const SI08 z			= c->GetZ();
	const UI16 targTrig		= c->GetScriptTrigger();
	cScript *toExecute		= JSMapping->GetScript( targTrig );
	for( CChar *ourChar = regChars->First(); !regChars->Finished(); ourChar = regChars->Next() )
	{
		if( !ValidateObject( ourChar ) || ourChar == c )
			continue;
		if( ourChar->GetX() == x && ourChar->GetY() == y && ourChar->GetZ() == z )
		{
			if( !IsGMBody( ourChar ) && ( ourChar->IsNpc() || isOnline( (*ourChar) ) ) &&
				ourChar->GetCommandLevel() < CL_CNS )
			{
				didShove = true;

				if( toExecute != NULL )
					toExecute->OnCollide( mSock, c, ourChar );
				UI16 tTrig		= ourChar->GetScriptTrigger();
				cScript *tExec	= JSMapping->GetScript( tTrig );
				if( tExec != NULL )
					tExec->OnCollide( ourChar->GetSocket(), ourChar, c );

				if( ourChar->GetVisible() == VT_TEMPHIDDEN || ourChar->GetVisible() == VT_INVISIBLE )
					mSock->sysmessage( 1384 );
				else
					mSock->sysmessage( 1383, ourChar->GetName().c_str() );
			}
		}
	}

	if( didShove )
		c->SetStamina( UOX_MAX( c->GetStamina() - 4, 0 ) );

	regChars->Pop();
}

void DoJSInRange( CChar *mChar, CBaseObject *objInRange )
{
	const UI16 targTrig	= mChar->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->InRange( mChar, objInRange );
}

void DoJSOutOfRange( CChar *mChar, CBaseObject *objOutOfRange )
{
	const UI16 targTrig	= mChar->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( toExecute != NULL )
		toExecute->OutOfRange( mChar, objOutOfRange );
}

bool UpdateItemsOnPlane( CSocket *mSock, CChar *mChar, CItem *tItem, UI16 id, UI16 dNew, UI16 dOld, UI16 visibleRange, bool isGM )
{
	if( isGM || tItem->GetVisible() == VT_VISIBLE || ( tItem->GetVisible() == VT_TEMPHIDDEN && tItem->GetOwnerObj() == mChar ) )
	{
		if( mSock != NULL && ( (id >= 0x407A && id <= 0x407F) || id == 0x5388 ) )
		{
			if( dNew == DIST_BUILDRANGE && dOld > DIST_BUILDRANGE )	// It's a building
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
			if( mSock != NULL )
				tItem->SendToSocket( mSock );
			DoJSInRange( mChar, tItem );
			return true;
		}
		else if( dOld == (visibleRange+1) && dNew > (visibleRange+1) )	// Just went out of range
		{
			if( mSock != NULL )
				tItem->RemoveFromSight( mSock );
			DoJSOutOfRange( mChar, tItem );
			return true;
		}
	}
	return false;
}

bool UpdateCharsOnPlane( CSocket *mSock, CChar *mChar, CChar *tChar, UI16 dNew, UI16 dOld, UI16 visibleRange )
{
	if( dNew == visibleRange && dOld > visibleRange )	// Just came into range
	{
		if( mSock != NULL )
			tChar->SendToSocket( mSock );
		DoJSInRange( mChar, tChar );
		DoJSInRange( tChar, mChar );
		return true;
	}
	if( dOld == (visibleRange+1) && dNew > (visibleRange+1) )	// Just went out of range
	{
		if( mSock != NULL )
			tChar->RemoveFromSight( mSock );
		DoJSOutOfRange( mChar, tChar );
		DoJSOutOfRange( tChar, mChar );
		return true;
	}
	return false;
}

void MonsterGate( CChar *s, const std::string scriptEntry );
void advanceObj( CChar *s, UI16 x, bool multiUse );
void SocketMapChange( CSocket *sock, CChar *charMoving, CItem *gate );
void HandleObjectCollisions( CSocket *mSock, CChar *mChar, CItem *itemCheck, ItemTypes type )
{
	switch( itemCheck->GetType() )
	{
		case IT_OBJTELEPORTER:														// teleporters
			if( itemCheck->GetTempVar( CITV_MOREX ) + itemCheck->GetTempVar( CITV_MOREY ) + itemCheck->GetTempVar( CITV_MOREZ ) > 0 )
				mChar->SetLocation( static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREX )), static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREY )), static_cast<SI08>(itemCheck->GetTempVar( CITV_MOREZ )) );
			break;
		case IT_ADVANCEGATE:														// advancement gates
		case IT_MULTIADVANCEGATE:
			if( !mChar->IsNpc() )
				advanceObj( mChar, static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREX )), ( itemCheck->GetType() == IT_MULTIADVANCEGATE ) );
			break;
		case IT_MONSTERGATE:	MonsterGate( mChar, itemCheck->GetDesc() );	break;	// monster gates
		case IT_RACEGATE:														// race gates
			Races->gate( mChar, static_cast<RACEID>(itemCheck->GetTempVar( CITV_MOREX )), itemCheck->GetTempVar( CITV_MOREY ) != 0 );
			break;
		case IT_DAMAGEOBJECT:														// damage objects
			if( !mChar->IsInvulnerable() )
			{
				mChar->Damage( itemCheck->GetTempVar( CITV_MOREX ) + RandomNum( itemCheck->GetTempVar( CITV_MOREY ), itemCheck->GetTempVar( CITV_MOREZ ) ), NULL );
			}
			break;
		case IT_SOUNDOBJECT:														// sound objects
			if( static_cast<UI32>(RandomNum( 1, 100 )) <= itemCheck->GetTempVar( CITV_MOREZ ) )
				Effects->PlaySound( itemCheck, static_cast<UI16>(itemCheck->GetTempVar( CITV_MOREX )) );
			break;
		case IT_MAPCHANGEOBJECT:
			SocketMapChange( mSock, mChar, itemCheck );
			break;
		case IT_ZEROKILLSGATE:	mChar->SetKills( 0 );		break;			// zero kill gate
		case IT_WORLDCHANGEGATE:	
			if( !mChar->IsNpc() )	// world change gate
			{
				if( mSock != NULL )
				{
					CPWorldChange wrldChange( (WorldType)itemCheck->GetTempVar( CITV_MOREX ), 1 );
					mSock->Send( &wrldChange );
				}
			}
			break;
	}
}
// handle item collisions, make items that appear on the edge of our sight because
// visible, buildings when they get in range, and if the character steps on something
// that might cause damage
void cMovement::HandleItemCollision( CChar *mChar, CSocket *mSock, SI16 oldx, SI16 oldy )
{
	// lets cache these vars in advance
	UI16 visibleRange	= static_cast<UI16>(MAX_VISRANGE + Races->VisRange( mChar->GetRace() ));
	if( mSock != NULL )
		visibleRange	= static_cast<UI16>(mSock->Range() + Races->VisRange( mChar->GetRace() ));
	const SI16 newx		= mChar->GetX();
	const SI16 newy		= mChar->GetY();
	UI16 id;
	ItemTypes type;
	bool EffRange;
	UI16 envTrig;
	
	bool isGM			= mChar->IsGM();
	UI16 dxNew, dyNew, dxOld, dyOld;
	const bool checkX	= (oldx != newx);
	const bool checkY	= (oldy != newy);
	/*
	A note to future people (from Zippy on 2/10/02)

	The OSI client MAY be ignoring certain items that it thinks fall out of the vis Range...

	Currently, UOX calculates vis in a square... meaning if an item in the corner of our
	'square' is sent to the OSI client, it MAY ignore it because it falls out of the client's
	apparent circle of visibility.

	At this time, I'm not sure this is causing problems, or whether problems are steming from
	a region issue (fixed now).  So I have not updated this code.  If this problem pops up in
	the future, and I'm no longer around...  This is your piece of the puzzle.
	*/
	REGIONLIST nearbyRegions = MapRegion->PopulateList( newx, newy, mChar->WorldNumber() );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		if( mSock != NULL )		// Only send char stuff if we have a valid socket
		{
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( !ValidateObject( tempChar ) )
					continue;
				// Character Send Stuff
				if( tempChar->IsNpc() || isOnline( (*tempChar) ) || ( isGM && cwmWorldState->ServerData()->ShowOfflinePCs() ) )
				{
					dxNew = static_cast<UI16>(abs( tempChar->GetX() - newx ));
					dyNew = static_cast<UI16>(abs( tempChar->GetY() - newy ));
					if( checkX && dyNew <= (visibleRange+2) )	// Only update on x plane if our x changed
					{
						dxOld = static_cast<UI16>(abs( tempChar->GetX() - oldx ));
						if( UpdateCharsOnPlane( mSock, mChar, tempChar, dxNew, dxOld, visibleRange ) )
							continue;	// If we moved diagonally, don't update the same char twice.
					}
					if( checkY && dxNew <= (visibleRange+2) )	// Only update on y plane if our y changed
					{
						dyOld = static_cast<UI16>(abs( tempChar->GetY() - oldy ));

						if( UpdateCharsOnPlane( mSock, mChar, tempChar, dyNew, dyOld, visibleRange ) )
							continue;
					}
				}
			}
			regChars->Pop();
		}
		CDataList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *tItem = regItems->First(); !regItems->Finished(); tItem = regItems->Next() )
		{
			if( !ValidateObject( tItem ) )
				continue;
			id		= tItem->GetID();
			type	= tItem->GetType();
			EffRange = (	tItem->GetX() == newx && tItem->GetY() == newy && 
							mChar->GetZ() >= tItem->GetZ() && mChar->GetZ() <= ( tItem->GetZ() + 5 ) );
			if( EffRange )
			{
				if( !Magic->HandleFieldEffects( mChar, tItem, id ) )
				{
					if( !tItem->CanBeObjType( OT_MULTI ) )
					{
						UI16 targTrig		= tItem->GetScriptTrigger();
						cScript *toExecute	= JSMapping->GetScript( targTrig );
						if( targTrig == 0 || toExecute == NULL )
						{
							if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>(type) ) )
							{
								envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>(type) );
								toExecute = JSMapping->GetScript( envTrig );
							}
							else if( JSMapping->GetEnvokeByID()->Check( id ) )
							{
								envTrig = JSMapping->GetEnvokeByID()->GetScript( id );
								toExecute = JSMapping->GetScript( envTrig );
							}
						}
						if( toExecute != NULL )
							toExecute->OnCollide( mSock, mChar, tItem );
					}
				}
				HandleObjectCollisions( mSock, mChar, tItem, type );
				Magic->GateCollision( mSock, mChar, tItem, type );
			}
			dxNew = static_cast<UI16>(abs( tItem->GetX() - newx ));
			dyNew = static_cast<UI16>(abs( tItem->GetY() - newy ));
			if( checkX && dyNew <= (visibleRange+2) )	// Only update items on furthest x plane if our x changed
			{
				dxOld = static_cast<UI16>(abs( tItem->GetX() - oldx ));
				if( UpdateItemsOnPlane( mSock, mChar, tItem, id, dxNew, dxOld, visibleRange, isGM ) )
					continue;	// If we moved diagonally, lets not update an item twice (since it could match the furthest x and y)
			}
			if( checkY && dxNew <= (visibleRange+2) )	// Only update items on furthest y plane if our y changed
			{
				dyOld = static_cast<UI16>(abs( tItem->GetY() - oldy ));
				if( UpdateItemsOnPlane( mSock, mChar, tItem, id, dyNew, dyOld, visibleRange, isGM ) )
					continue;
			}
		}
		regItems->Pop();
	}
}

// Used for shove messages and walking through doors.
// Includes GMs, Counsellors, and Ghosts; Better name may be IsEthereal()
bool cMovement::IsGMBody( CChar *c )
{
	if( c->IsGM() || c->GetID() == 0x03DB || c->GetID() == 0x0192 || c->GetID() == 0x0193 || c->IsDead() ) 
        return true;
    return false;
}

void cMovement::CombatWalk( CChar *i ) // Only for switching to combat mode
{
	if( !ValidateObject( i ) )
		return;
	CPExtMove toSend = (*i);

	if( !i->IsAtWar() )
		i->SetTarg( NULL );
    
	SOCKLIST nearbyChars = FindNearbyPlayers( i );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
    {
		CChar *mChar = (*cIter)->CurrcharObj();
		if( mChar != i )
        {
			toSend.FlagColour( static_cast<UI08>(i->FlagColour( mChar )) );
            (*cIter)->Send( &toSend );
        }
    }
}

bool checkBoundingBox( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 fx2, SI16 fy2, UI08 worldNumber );
bool checkBoundingCircle( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 radius, UI08 worldNumber );
void cMovement::NpcWalk( CChar *i, UI08 j, SI08 getWander )   //type is npcwalk mode (0 for normal, 1 for box, 2 for circle)
{
	const SI16 fx1 = i->GetFx( 0 );
	const SI16 fx2 = i->GetFx( 1 );
	const SI16 fy1 = i->GetFy( 0 );
	const SI16 fy2 = i->GetFy( 1 );
	const SI08 fz1 = i->GetFz();
    // if we are walking in an area, and the area is not properly defined, just don't bother with the area anymore
    if(	( ( getWander == WT_BOX ) && ( fx1 == -1 || fx2 == -1 || fy1 == -1 || fy2 == -1 ) ) ||
        ( ( getWander == WT_CIRCLE ) && ( fx1 == -1 || fx2 == -1 || fy1 == -1 ) ) ) // circle's don't use fy2, so don't require them! fur 10/30/1999
        
    {
        i->SetNpcWander( WT_FREE ); // Wander freely from now on
    }
	// Thyme New Stuff 2000.09.21
	const SI16 newx			= GetXfromDir( j, i->GetX() );
	const SI16 newy			= GetYfromDir( j, i->GetY() );
	const UI08 worldNumber	= i->WorldNumber();
	// Let's make this a little more readable.
	const UI08 jMod			= (j & 0x87);
	switch( getWander )
	{
		case WT_FREE:	// Wander freely
		case WT_FLEE:	// Wander freely after fleeing
			Walking( NULL, i, jMod, 256 );
			break;
		case WT_BOX:	// Wander inside a box
			if( checkBoundingBox( newx, newy, fx1, fy1, fz1, fx2, fy2, worldNumber ) )
				Walking( NULL, i, jMod, 256 );
			break;
		case WT_CIRCLE:	// Wander inside a circle
			if( checkBoundingCircle( newx, newy, fx1, fy1, fz1, fx2, worldNumber ) )
				Walking( NULL, i, jMod, 256 );
			break;
		default:
			Console.Error( "Bad NPC Wander type passed to NpcWalk: %i", getWander );
			break;
	}
}

// Function      : cMovement::GetYfromDir
// Written by    : Unknown
// Revised by    : Thyme
// Revision Date : 2000.09.15
// Purpose       : Return the new y from given dir

SI16 cMovement::GetYfromDir( UI08 dir, SI16 y )
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

// Function      : cMovement::GetXfromDir
// Written by    : Unknown
// Revised by    : Thyme
// Revision Date : 2000.09.15
// Purpose       : Return the new x from given dir

SI16 cMovement::GetXfromDir( UI08 dir, SI16 x )
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

// Ok, I'm going to babble here, but here's my thinking process...
// Max Heuristic is 5 (for now) and I'm not concerned about walls... I'll add that later
// Easiest way I think would be for recursive call for now... Will change later if need be
// pathfind will call itself with new stuff... as long as the distance get's shorter
// We have to take into consideration if the NPC is going to a point or to a character
// if we don't want NPCs to walk over each other, this has to be known, because the NPC
// that's walking will never reach destination if it's another character.
// We must pass, that way if we get chardist=1 then we're ok.  We are basically searching
// for the shortest path, which is always a diagonal line, followed by lateral to target
// (barring obstacles) On calculation, for the FIRST step, we need to know if a character
// is there or not, then after that no biggie because if so we can just recalc the path if
// something is blocking. If we don't check on that first step, NPCs will get stuck behind horses
// and stuff... Kinda exploitable if I'm on a horse attacking, then step off and behind to hide
// while I heal. The first thing we need to do when walking is determine if i'm blocked... then
// if I'm an NPC, recalculate my path and step... I'm also gonna take out the path structure
// in chars_st... all we need is to hold the directions, not the x and y... Hopefully this will
// save memory.

void cMovement::PathFind( CChar *c, SI16 gx, SI16 gy, bool willRun, UI08 pathLen )
{
	// Make sure this is a valid character before proceeding
	if( !ValidateObject( c ) ) 
		return;

	// Make sure the character has taken used all of their previously saved steps
	if( c->StillGotDirs() ) 
		return;

	// Thyme 2000.09.21
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
		int pf_neg			= ( ( RandomNum( 0, 1 ) ) ? 1 : -1 );
		UI08 newDir			= Direction( newx, newy, gx, gy );
		bool canMoveInDir	= false;
		if( newDir < 8 && calc_move( c, newx, newy, newz, newDir ) )
		{
			pf_dir = newDir;
			canMoveInDir = true;
		}
		for( UI08 i = 0; i < 7; ++i )
		{
			if( canMoveInDir || ( pf_dir < 8 && calc_move( c, newx, newy, newz, pf_dir ) ) )
			{
				newx = GetXfromDir( pf_dir, newx );	// moved inside if to reduce calculations
				newy = GetYfromDir( pf_dir, newy );
				if( ( pn < P_PF_MRV ) && CheckForCharacterAtXYZ( c, newx, newy, newz ) )
					continue;

				UI08 dirToPush = pf_dir;
				if( willRun )
					dirToPush |= 0x80;

				c->PushDirection( dirToPush );

				if( olddir != UNKNOWNDIR && olddir != pf_dir )
					c->PushDirection( dirToPush );

				olddir = pf_dir;

				bFound = true;
				break;
			}
			pf_neg *= -1;
			pf_dir = static_cast<UI08>((UI08)(pf_dir + ( i * pf_neg )) % 8);
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

bool cMovement::HandleNPCWander( CChar& mChar )
{
	bool shouldRun	= false;
	bool canRun		= false;
	CChar *kChar	= NULL;
	UI08 j;
	switch( mChar.GetNpcWander() )
	{
	case WT_NONE: // No movement
		break;
	case WT_FOLLOW: // Follow the follow target
		kChar = mChar.GetFTarg();
		if( !ValidateObject( kChar ) )
			break;
		if( isOnline( (*kChar) ) || kChar->IsNpc() )
		{
			const UI16 charDist	= getDist( &mChar, kChar );
			if( !objInRange( &mChar, kChar, DIST_NEXTTILE ) && Direction( &mChar, kChar->GetX(), kChar->GetY() ) < 8 )
			{
				if( mChar.GetStamina() > 0 )
				{
					if(kChar->GetRunning() > 0)
						canRun = true;
					else if( charDist >= DIST_INRANGE )
						canRun = true;
					else if( (mChar.GetRunning() > 0) && (charDist > DIST_NEARBY) )
						canRun = true;
				}

				if( cwmWorldState->ServerData()->AdvancedPathfinding() )
					AdvancedPathfinding( &mChar, kChar->GetX(), kChar->GetY(), canRun );
				else
					PathFind( &mChar, kChar->GetX(), kChar->GetY(), canRun );
				j = mChar.PopDirection();
				Walking( NULL, &mChar, j, 256 );
				shouldRun = (( j&0x80 ) != 0);
			}
			// Has the Escortee reached the destination ??
			if( !kChar->IsDead() && mChar.GetQuestDestRegion() == mChar.GetRegionNum() )
				MsgBoardQuestEscortArrive( kChar->GetSocket(), &mChar );
		}
		break;
	case WT_FREE: // Wander freely, avoiding obstacles.
	case WT_BOX: // Wander freely, within a defined box
	case WT_CIRCLE: // Wander freely, within a defined circle
		j = RandomNum( 1, 5 );
		if( j == 1 )
			break;
		else if( j == 2 )
			j = RandomNum( 0, 8 );
		else	// Move in the same direction the majority of the time
			j = mChar.GetDir();
		shouldRun = (( j&0x80 ) != 0);
		NpcWalk( &mChar, j, mChar.GetNpcWander() );
		break;
	case WT_FLEE: //FLEE!!!!!!
		kChar = mChar.GetTarg();
		if( !ValidateObject( kChar ) )
			break;
		if( getDist( &mChar, kChar ) < P_PF_MFD )
		{	// calculate a x,y to flee towards
			const UI16 mydist	= P_PF_MFD - getDist( &mChar, kChar ) + 1;
			j					= Direction( &mChar, kChar->GetX(), kChar->GetY() );
			SI16 myx			= GetXfromDir( j, mChar.GetX() );
			SI16 myy			= GetYfromDir( j, mChar.GetY() );

			SI16 xfactor = 0;
			SI16 yfactor = 0;
			// Sept 22, 2002 - Xuri
			if( myx != mChar.GetX() )
			{
				if( myx < mChar.GetX() )
					xfactor = 1;
				else
					xfactor = -1;
			}
			
			if( myy != mChar.GetY() )
			{
				if( myy < mChar.GetY() )
					yfactor = 1;
				else
					yfactor = -1;
			}

			myx += (SI16)( xfactor * mydist );
			myy += (SI16)( yfactor * mydist );

			canRun = (mChar.GetStamina() > 0);

			// now, got myx, myy... lets go.
			if( cwmWorldState->ServerData()->AdvancedPathfinding() )
				AdvancedPathfinding( &mChar, myx, myy, true );
			else
				PathFind( &mChar, myx, myy, true );
			j			= mChar.PopDirection();
			shouldRun	= (( j&0x80 ) != 0);
			Walking( NULL, &mChar, j, 256 );
		}
		else
		{ // wander freely... don't just stop because I'm out of range.
			j = RandomNum( 1, 5 );
			if( j == 1 )
				break;
			else if( j == 2 )
				j = RandomNum( 0, 8 );
			else	// Move in the same direction the majority of the time
        		j = mChar.GetDir();
			shouldRun = (( j&0x80 ) != 0);
			NpcWalk( &mChar, j, mChar.GetNpcWander() );
		}
		break;
	case WT_PATHFIND:		// Pathfinding!!!!
		if( mChar.StillGotDirs() )
		{
			j			= mChar.PopDirection();
			shouldRun	= (( j&0x80 ) != 0);
			Walking( NULL, &mChar, j, 256 );
		}
		else
			mChar.SetNpcWander( mChar.GetOldNpcWander() );
		break;
	default:
		break;
	}
	return shouldRun;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	void cMovement::NpcMovement( CChar& mChar )
//|	Date			-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Calculate, and handle NPC AI movement
//|									
//|	Modification	-	09/22/2002	-	Fixed fleeing NPCs by reversing values for 
//|									xfactor & yfactor lines
//o--------------------------------------------------------------------------o
//|	Returns			- NA
//o--------------------------------------------------------------------------o	
void cMovement::NpcMovement( CChar& mChar )
{
	if( mChar.IsFrozen() || !mChar.IsNpc() )
		return;

	bool shouldRun		= false;
	bool canRun			= ( (mChar.GetStamina() > 0) && mChar.CanRun() );

    if( mChar.GetTimer( tNPC_MOVETIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
    {
#if DEBUG_NPCWALK
		Console.Print( "DEBUG: ENTER (%s): %d AI %d WAR %d J\n", mChar.GetName(), mChar.GetNpcWander(), mChar.IsAtWar(), j );
#endif
		if( mChar.IsAtWar() && mChar.GetNpcWander() != WT_FLEE )
        {
            CChar *l = mChar.GetAttacker();
            if( ValidateObject( l ) && ( isOnline( (*l) ) || l->IsNpc() ) )
            {
				const UI08 charDir	= Direction( &mChar, l->GetX(), l->GetY() );
				const UI16 charDist	= getDist( &mChar, l );
                if( charDir < 8 && ( charDist <= 1 || ( Combat->getCombatSkill( Combat->getWeapon( &mChar ) ) == ARCHERY && charDist <= cwmWorldState->ServerData()->CombatArcherRange() ) ) )
				{
					mChar.FlushPath();

					bool los = LineOfSight( NULL, &mChar, l->GetX(), l->GetY(), ( l->GetZ() + 15 ), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING );
					if( los && charDir != mChar.GetDir() )
					{
						mChar.SetDir( charDir );
						Walking( NULL, &mChar, charDir, 256 );
						return;
					}
				}
				else
                {
					if( cwmWorldState->ServerData()->AdvancedPathfinding() )
					{
						if( !mChar.StillGotDirs() )
						{
							if( !AdvancedPathfinding( &mChar, l->GetX(), l->GetY(), canRun ) )
								Combat->InvalidateAttacker( &mChar );
						}
					}
					else
						PathFind( &mChar, l->GetX(), l->GetY(), canRun );
					const UI08 j	= mChar.PopDirection();
					shouldRun		= (( j&0x80 ) != 0);
					Walking( NULL, &mChar, j, 256 );
				}
	        }
			else
				mChar.FlushPath();
		}
		else
			shouldRun = HandleNPCWander( mChar );

		if( shouldRun )
		{
			if ( mChar.GetNpcWander() != WT_FLEE )
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetRunningSpeed() ) );
			else
				mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetFleeingSpeed() ) );
		}
		else
			mChar.SetTimer( tNPC_MOVETIME, BuildTimeValue( mChar.GetWalkingSpeed() ) );
    }
}

// Function      : cMovement::CanCharWalk()
// Written by    : Dupois
// Revised by    : Thyme
// Revision Date : 2000.09.17
// Purpose       : Check if a character can walk to a specified x,y location
// Method        : Modified the old CheckWalkable function so that it can be utilized throughout
// the walking code. Ever wonder why NPCs can walk through walls and stuff in combat mode? This
// is the fix, plus more.

UI08 cMovement::Direction( CChar *mChar, SI16 x, SI16 y )
{
	return Direction( mChar->GetX(), mChar->GetY(), x, y );
}
UI08 cMovement::Direction( SI16 sx, SI16 sy, SI16 dx, SI16 dy )
{
	UI08 dir;
	
	const SI16 xdif = static_cast<SI16>(dx - sx);
	const SI16 ydif = static_cast<SI16>(dy - sy);
	
	if( xdif == 0 && ydif < 0 ) 
		dir = NORTH;
	else if( xdif > 0 && ydif < 0 ) 
		dir = NORTHEAST;
	else if( xdif > 0 && ydif == 0 ) 
		dir = EAST;
	else if( xdif > 0 && ydif > 0 ) 
		dir = SOUTHEAST;
	else if( xdif == 0 && ydif > 0 ) 
		dir = SOUTH;
	else if( xdif < 0 && ydif > 0 ) 
		dir = SOUTHWEST;
	else if( xdif < 0 && ydif == 0 ) 
		dir = WEST;
	else if( xdif < 0 && ydif < 0 ) 
		dir = NORTHWEST;
	else 
		dir = UNKNOWNDIR;
	
	return dir;
}

/********************************************************
  Function: cMovement::CheckWalkable

  Description: (knoxos)
    Rewritten checkwalk-function, it calculates new z-position
    for a walking creature (PC or NPC) walks, and checks if 
    movement is blocked.

    This function takes a little more calculation time, than the
    last one, since it walks two times through the static-tile set.
    However at least this one is (more) correct, and these VERy guys 
    now hit their noses on the walls.

    In principle it is the same as the World-kernel in UMelange.
   
  Parameters:  
    CChar *c				Character
	int x, y				new cords.
	int oldx, oldy			old cords.
	bool justask			don't make any changes, the func. is just asked
						  "what if"..

  Return code:
    new z-value        if not blocked
    illegal_z == -128, if walk is blocked

********************************************************/
SI08 cMovement::calc_walk( CChar *c, SI16 x, SI16 y, SI16 oldx, SI16 oldy, SI08 oldz, bool justask, bool waterWalk )
{
	if( !ValidateObject( c ) )
		return ILLEGAL_Z;
	const bool isGM		= IsGMBody( c );
	bool may_levitate	= c->MayLevitate();
	bool on_ladder		= false;
	SI08 newz			= ILLEGAL_Z;
	UI08 ontype			= 0;
	UI16 xycount		= 0;
	UI08 worldNumber	= c->WorldNumber();
	UI16 i				= 0;
	SI08 topTile		= ILLEGAL_Z;
	CTileUni *tb;
	CTileUni xyblock[XYMAX];
	GetBlockingMap( x, y, xyblock, xycount, oldx, oldy, worldNumber );
	GetBlockingStatics( x, y, xyblock, xycount, worldNumber );
	GetBlockingDynamics( x, y, xyblock, xycount, worldNumber );

	// first calculate newZ value
	for( i = 0; i < xycount; ++i )
	{
		tb = &xyblock[i];

		if( tb->Top() < newz )
			continue;

		if( tb->Top() >= topTile )
			topTile = tb->Top();

		if( waterWalk )
		{
			if( tb->Top() >= newz && tb->Top() <= oldz && tb->CheckFlag( TF_WET ) )
			{ // swimable tile
				newz	= tb->Top();
				ontype	= tb->Type();
			}
			continue;
		}
		// check if the creature is floating on a static (keeping Z or falling)
		if( tb->CheckFlag( TF_SURFACE ) )
		{
			if( tb->Top() <= oldz )
			{
				newz = tb->Top();
				ontype = tb->Type();
				if( tb->CheckFlag( TF_STAIRRIGHT ) ) // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
				continue;
			}
			// So now comes next step, levitation :o)
			// you can gain Z to a limited amount if yo uwere climbing on last move on a ladder
			if( may_levitate && tb->Top() <= (oldz + MAX_Z_LEVITATE) )
			{
				ontype = tb->Type();
				newz = tb->Top();
				if( tb->CheckFlag( TF_STAIRRIGHT ) ) // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
				continue;
			}
		}

		// check if the creature is climbing on a climbable Z
		// (gaining Z through stairs, ladders, etc)
		// This form has no height limit, and the tile bottom must start lower or
		// equal current height + levitateable limit
		if( tb->BaseZ() <= (oldz + MAX_Z_LEVITATE) )
		{
			if( tb->CheckFlag( TF_CLIMBABLE ) || tb->Type() == 0 ||								// Climbable tile, map tiles are also climbable
				( tb->Flag( 3 ) == 0 && tb->Flag( 2 ) == 0x22 ) ||								// These are a special kind of tiles where OSI forgot to set the climbable flag
				( (tb->Top() >= oldz && tb->Top() <= oldz+3) && tb->CheckFlag( TF_SURFACE ) ) )	// Allow to climb a height of 1 even if the climbable flag is not set
			{                 
				ontype = tb->Type();
				newz = tb->Top();
				if( tb->CheckFlag( TF_STAIRRIGHT ) ) // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
			}
		}
	}

	if( newz <= (oldz - MAX_Z_FALL) && newz < topTile )	// If we are falling from the air, don't block, if we fall through hills, block.
		newz = ILLEGAL_Z;

	if( newz != ILLEGAL_Z )
	{
		const SI08 item_influence = UOX_MAX( static_cast<SI08>(newz + MAX_ITEM_Z_INFLUENCE), oldz );
		// also take care to look on all tiles the creature has fallen through
		// (npc's walking on ocean bug)
		// now the new Z-cordinate of creature is known, 
		// check if it hits it's head against something (blocking in other words)

		for( i = 0; i < xycount; ++i )
		{
			tb = &xyblock[i]; 

			if( tb->Top() < newz )
				continue;

			if( waterWalk )
			{
				if( ( ( tb->Top() > newz && tb->BaseZ() <= item_influence ) ||
					( tb->Top() == newz && ontype == 0 ) && !tb->CheckFlag( TF_WET ) ) )	// Check for blocking tile
				{
					newz = ILLEGAL_Z;
					break;
				}
				continue;
			}
			if( ( tb->CheckFlag( TF_BLOCKING ) || ( tb->CheckFlag( TF_SURFACE ) && tb->Top() > newz ) ) &&	// Check for blocking tile or stairs
				!( isGM && ( tb->CheckFlag( TF_WINDOW ) || tb->CheckFlag( TF_DOOR ) ) ) )				// ghosts can walk through doors
			{
				if( tb->Top() > newz && tb->BaseZ() <= item_influence || ( tb->Top() == newz && ontype == 0 ) )
				{
					newz = ILLEGAL_Z;
					break;
				}
			}
		}
		if( newz != ILLEGAL_Z && !justask ) // save information if we have climbed on last move.
			c->SetLevitate( on_ladder );
	}

	return newz;
}

// Function      : cMovement::CanCharMove()
// Written by    : Thyme
// Revision Date : 2000.09.17
// Purpose       : Check if a character can walk to a from x,y to dir direction
// Method        : This handles the funky diagonal moves.
bool cMovement::calc_move( CChar *c, SI16 x, SI16 y, SI08 &z, UI08 dir)
{
	if( !cwmWorldState->creatures[c->GetID()].IsWater() )
	{
		if( (dir&0x07)%2 )
		{ // check three ways.
			UI08 ndir = turn_counter_clock_wise( dir );
			if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true ) == ILLEGAL_Z && !cwmWorldState->creatures[c->GetID()].IsAmphibian() )
				return false;
			ndir = turn_clock_wise( dir );
			if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true ) == ILLEGAL_Z && !cwmWorldState->creatures[c->GetID()].IsAmphibian() )
				return false;
		}
		z = calc_walk( c, GetXfromDir( dir, x ), GetYfromDir( dir, y ), x, y, c->GetZ(), false );
	}

	if( cwmWorldState->creatures[c->GetID()].IsWater() || ( cwmWorldState->creatures[c->GetID()].IsAmphibian() && z == ILLEGAL_Z) )
	{
		if( (dir&0x07)%2 )
		{ // check three ways.
			UI08 ndir = turn_counter_clock_wise( dir );
			if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true, true ) == ILLEGAL_Z )
				return false;
			ndir = turn_clock_wise( dir );
			if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, c->GetZ(), true, true ) == ILLEGAL_Z )
				return false;
		}
		z = calc_walk( c, GetXfromDir( dir, x ), GetYfromDir( dir, y ), x, y, c->GetZ(), false, true );
	}
	return (z != ILLEGAL_Z);
}

void cMovement::deny( CSocket *mSock, CChar *s, SI16 sequence )
{
	CPWalkDeny denPack;

	denPack.SequenceNumber( (SI08)sequence );
	denPack.X( s->GetX() );
	denPack.Y( s->GetY() );
	denPack.Direction( s->GetDir() );
	denPack.Z( s->GetZ() );

	if( mSock != NULL )
	{
		mSock->Send( &denPack );
		mSock->WalkSequence( -1 );
	}
}

bool operator==(const nodeFCost& x, const nodeFCost& y)
{
    return ( x.fCost == y.fCost );
}

bool operator<(const nodeFCost& x, const nodeFCost& y)
{
	return ( x.fCost < y.fCost );
}

bool operator>(const nodeFCost& x, const nodeFCost& y)
{
	return ( x.fCost > y.fCost );
}

bool cMovement::PFGrabNodes( CChar *mChar, UI16 targX, UI16 targY, UI16 curX, UI16 curY, SI08 curZ, UI32 parentSer, std::map< UI32, pfNode >& openList, std::map< UI32, UI32 >& closedList, std::deque< nodeFCost >& fCostList )
{
	std::map< UI32, bool > blockList;
	blockList.clear();

	SI08 newZ = ILLEGAL_Z;
	for( SI08 xOff = -1; xOff < 2; ++xOff )
	{
		SI16 checkX = curX + xOff;
		for( SI08 yOff = -1; yOff < 2; ++yOff )
		{
			if( !yOff && !xOff )
				continue;

			SI16 checkY = curY + yOff;
			UI32 locSer = (checkY + (checkX<<16));

			newZ = calc_walk( mChar, checkX, checkY, curX, curY, curZ, false );
			if( ILLEGAL_Z == newZ )
			{
				blockList[locSer] = true;
				continue;
			}

			//if( blockList.find( locSer ) != blockList.end() )
				//continue;

			// Don't Cut Corners
			bool cornerBlocked = false;
			if( xOff != 0 && yOff != 0 )
			{
				UI32 check1Ser = (checkY + (curX<<16));
				UI32 check2Ser = (curY + (checkX<<16));
				if( blockList.find( check1Ser ) != blockList.end() || blockList.find( check2Ser ) != blockList.end() )
					cornerBlocked = true;
				else
				{
					if( calc_walk( mChar, curX, checkY, curX, curY, curZ, true ) == ILLEGAL_Z )
					{
						cornerBlocked = true;
						blockList[check1Ser] = true;
					}
					else if( calc_walk( mChar, checkX, curY, curX, curY, curZ, true ) == ILLEGAL_Z )
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
				gCost = 14;

			std::map< UI32, pfNode >::const_iterator olIter;
			olIter = openList.find( locSer );
			if( olIter != openList.end() )
			{
				pfNode mNode = olIter->second;
				if( mNode.gCost > gCost )
				{
					for( std::deque< nodeFCost >::iterator fcIter = fCostList.begin(); fcIter != fCostList.end(); ++fcIter )
					{
						if( (*fcIter).xySer == locSer )
						{
							(*fcIter).fCost = mNode.hCost + gCost;
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
				UI16 hCost = 10*(abs(checkX - targX) + abs(checkY - targY));
				UI16 fCost = gCost + hCost;

				openList[locSer] = pfNode( hCost, gCost, parentSer, newZ );
				fCostList.push_back( nodeFCost( fCost, locSer ) );
				std::sort( fCostList.begin(), fCostList.end() );
			}
		}
	}
	return false;
}

bool cMovement::AdvancedPathfinding( CChar *mChar, UI16 targX, UI16 targY, bool willRun )
{
	UI16 curX			= mChar->GetX();
	UI16 curY			= mChar->GetY();
	SI08 curZ			= mChar->GetZ();
	UI08 dirToPush		= UNKNOWNDIR;
	size_t loopCtr		= 0;
	size_t maxSteps		= 1000;

	std::map< UI32, pfNode >	openList;
	std::map< UI32, UI32 >		closedList;
	std::deque< nodeFCost >		fCostList;

	openList.clear();
	closedList.clear();
	fCostList.resize( 0 );

	UI32 parentSer			= (curY + (curX<<16));
	openList[parentSer]		= pfNode();
	openList[parentSer].z	= curZ;
	fCostList.push_back( nodeFCost( 0, parentSer ) );
	while( ( curX != targX || curY != targY ) && loopCtr < maxSteps )
	{
		parentSer = fCostList[0].xySer;
		curX = static_cast<UI16>(parentSer>>16);
		curY = static_cast<UI16>(parentSer%65536);
		curZ = openList[parentSer].z;

		closedList[parentSer] = openList[parentSer].parent;
		openList.erase( openList.find( parentSer ) );
		fCostList.pop_front();

		if( PFGrabNodes( mChar, targX, targY, curX, curY, curZ, parentSer, openList, closedList, fCostList ) )
		{
			while( parentSer != 0 )
			{
				UI08 newDir = Direction( curX, curY, targX, targY );
				if( willRun )
					newDir |= 0x80;

				if( dirToPush != UNKNOWNDIR && dirToPush != newDir )
					mChar->PushDirection( newDir, true );	// NPC's need to "walk" twice when turning
				mChar->PushDirection( newDir, true );

				dirToPush	= newDir;
				targX		= curX;
				targY		= curY;
				parentSer 	= closedList[parentSer];
				curX		= static_cast<UI16>(parentSer>>16);
				curY		= static_cast<UI16>(parentSer%65536);
			}
			break;
		}
		else if( fCostList.empty() )
			break;
		++loopCtr;
	}
#if defined( UOX_DEBUG_MODE )
	if( loopCtr == maxSteps )
		Console.Warning( "AdvancedPathfinding: Unable to find a path, max steps limit reached.\n" );
	else
		Console.Print( "AdvancedPathfinding: %u loops to find path.\n", loopCtr );
#endif
	if( loopCtr == maxSteps )
		return false;
	return true;
}

}
