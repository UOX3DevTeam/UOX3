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


// These are defines that I'll use. I have a history of working with properties, so that's why
// I'm using custom definitions here versus what may be defined in the other includes.
// NOTE: P = Property, P_C = Property for Characters, P_PF = Property for Pathfinding
//       P_M = Property for Movement

// These are the debugging defines

// These are the errors that we should want to see. RARELY results in any console spam unless
// someone is trying to use an exploit or is sending invalid data directly to the server.

#define DEBUG_WALKING 0
#define DEBUG_WALK_BLOCK 0

#define DEBUG_NPCWALK		0
#define DEBUG_WALK			0
#define DEBUG_PATHFIND		0

#undef DBGFILE
#define DBGFILE "walking.cpp"

#define MAX_ITEM_Z_INFLUENCE 10 // Any item above this height is discarded as being too far away to effect the char
#define MAX_Z_LEVITATE 10			// Maximum total height to reach a tile marked as 'LEVITATABLE'
												// Items with a mark as climbable have no height limit


inline SI08 higher( SI08 a, SI08 b )
{
	if( a < b )
		return b;
	else
		return a;
}

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
  ///return ((h & 0x8) ? (((h & 0xF) ^ 0xF) + 1) : h & 0xF);
	//return (h & 0x7);
	//return ((h & 0x8) ? (((h & 0xF) ^ 0xF) + 1) : h & 0xF);
	return (SI08)((h & 0x8) ? ((h & 0xF) >> 1) : h & 0xF);
} 

UI08 FlagColour( CChar *a, CChar *b )
{
	if( a == NULL )
		return 3;
	GuildRelation guild;
	SI08 race;
	if( b != NULL )
	{
	    guild = GuildSys->Compare( a, b );
		race = Races->Compare( a, b );
	}
	else
	{
		guild = GR_UNKNOWN;
		race = 0;
	}
	if( a->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() ) 
		return 6;
	else if( race != 0 || guild != GR_UNKNOWN )
	{
		if( guild == GR_ALLY || guild == GR_SAME || race > 0 )//Same guild (Green)
			return 2;
		else if( guild == GR_WAR || race < 0 ) // Enemy guild.. set to orange
			return 5;
	}

	if( a->IsMurderer() )
		return 6;		// If a bad, show as red.
	else if( a->IsInnocent() )
		return 1;		// If a good, show as blue.
	else 
		return 3;		// grey
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

void cMovement::Walking( CChar *c, UI08 dir, SI16 sequence )
{
	// sometimes the NPC movement code comes up with -1, for example, if we are following someone
	// and we are directly on top of them
	// ^ That's because chardir returns -1 when direction is the same.  Actually, this
	// should only happen when a player walks on top of an NPC and the NPC tries to move.

	if( !isValidDirection( dir ) )
	{
		Console.Error( 2, "%s (cMovement::Walking) caught bad direction = %s %d 0x%x\n", DBGFILE, c->GetName(), dir, dir );
		// If I don't do this, the NPC will keep trying to walk on the same step, which is
		// where he's already at. Can cause an infinite loop. (Trust me, was one of the things
		// that locked up NW Alpha 2)
		// Will call something like NPCRandomMove in the future.
		if( c->IsNpc() )
			c->FlushPath();
		return;
	}

	cSocket *mSock = calcSocketObjFromChar( c );
    
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

	// this if assumes that chars[s].dir has no high-bits just lets make sure of it
	// assert((chars[c].dir & 0xFFF0) == 0);
	// this assertion is failing, so either my assumption about it is wrong or there
	// is a bugaboo
	
	// see if we have stopped to turn or if we are moving
	const bool amTurning = ( (dir&0x07) != c->GetDir() );
	if( !amTurning )
	{
		if( !CheckForRunning( c, dir ) )
			return;
		
		if( !CheckForStealth( c ) )
			return;
		
		// if this was an NPC lets reset their move timer
		// this seems to be an usual place within this function to reset this
		// i guess they can turn a whole lot this way
		// Thyme: Already reset in NPCMovement (which calls this function, and NPCWalk)
		//if( chars[c].npc ) 
		//{
		//	chars[c].npcmovetime=(UI32)(uiCurrentTime+(R64)(NPCSPEED*CLOCKS_PER_SEC)); //reset move timer
		//}

		SI08 myz = illegal_z;
		SI16 myx = GetXfromDir( dir, c->GetX() );
		SI16 myy = GetYfromDir( dir, c->GetY() );
		if( !calc_move( c, c->GetX(), c->GetY(), myz, dir ) )
		{
#if DEBUG_WALK
			Console.Print( "DEBUG: %s (cMovement::Walking) Character Walk Failed for %s\n", DBGFILE, chars[c].GetName() );
			Console.Print( "DEBUG: %s (cMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, chars[c].GetX(), chars[c].GetY(), chars[c].GetZ() );
			Console.Print( "DEBUG: %s (cMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz );
#endif
			if( mSock != NULL )
				deny( mSock, c, sequence );
			if( c->IsNpc() )
				c->FlushPath();
			return;
		}
		dispz = z = myz;
#if DEBUG_WALK
		Console.Print( "DEBUG: %s (cMovement::Walking) Character Walk Passed for %s\n", DBGFILE, chars[c].GetName() );
		Console.Print( "DEBUG: %s (cMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, chars[c].GetX(), chars[c].GetY(), chars[c].GetZ() );
		Console.Print( "DEBUG: %s (cMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz );
#endif

		if( c->IsNpc() && CheckForCharacterAtXYZ( c, myx, myy, myz ) )
		{
			c->FlushPath();
			return;
		}

		MoveCharForDirection( c, dir );
		
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
		if( cwmWorldState->ServerData()->GetAmbientFootsteps() )
  			playTileSound( mSock );
		
		// since we actually moved, update the regions code
		HandleRegionStuffAfterMove( c, oldx, oldy );
	}
/*
	else
		Console.Error( "Player is turning in the same spot.\n" );
*/
	
	// do all of the following regardless of whether turning or moving i guess
	
	// set the player direction to contain only the cardinal direction bits
	c->SetDir( (dir&0x07) );
	
	SendWalkToPlayer( c, mSock, sequence );
	SendWalkToOtherPlayers( c, dir, oldx, oldy );
	OutputShoveMessage( c, mSock );
	
	// keep on checking this even if we just turned, because if you are taking damage
	// for standing here, lets keep on dishing it out. if we pass whether we actually
	// moved or not we can optimize things some
	HandleItemCollision( c, mSock, amTurning, oldx, oldy );
	
	// i'm going ahead and optimizing this, if you haven't really moved, should be
	// no need to check for teleporters and the weather shouldn't change
	if( !amTurning )
	{
		HandleTeleporters( c, oldx, oldy );
		HandleWeatherChanges( c, mSock );
	}
	
	// would have already collided, right??
	if( !amTurning )
		Magic->GateCollision( c );
	
	// again, don't know if we need to check when turning or not
	if( !amTurning )
		checkRegion( c ); // doesn't change physical coords, so no point in making a change
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

bool cMovement::isFrozen( CChar *c, cSocket *mSock, SI16 sequence )
{
	if( c->GetCasting() > 0 )
	{
		if( mSock != NULL )
		{
			sysmessage( mSock, 1380 );
			deny( mSock, c, sequence );
		}
#if DEBUG_WALK
		Console.Print( "DEBUG: %s (cMovement::isFrozen) casting char %s\n", DBGFILE, chars[c].GetName() );
#endif
		return true;
	}
	if( c->IsFrozen() )
	{
		if( mSock != NULL )
		{
			sysmessage( mSock, 1381 );
			deny( mSock, c, sequence );
		}
#if DEBUG_WALK
		Console..Print( "DEBUG: %s (cMovement::isFrozen) frozen char %s\n", DBGFILE, c->GetName() );
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

bool cMovement::isOverloaded( CChar *c, cSocket *mSock, SI16 sequence )
{
	// Who are we going to check for weight restrictions?
	if( !c->IsDead() && !c->IsNpc() && c->GetCommandLevel() < CNSCMDLEVEL )
	{
		if( mSock != NULL )
		{
			if( Weight->isOverloaded( c ) )
				c->SetStamina( (UI16)(((c->GetWeight() / 100) - ((c->GetStrength() * WEIGHT_PER_STR) + 30)) * 2) );
			if( c->GetStamina() <= 0 )
			{
				c->SetStamina( 0 );
				sysmessage( mSock, 1382 );
				deny( mSock, c, sequence );
#if DEBUG_WALK
				Console.Print( "DEBUG: %s (cMovement::Walking) overloaded char %s\n", DBGFILE, c->GetName() );
#endif
				updateStats( c, 2 );
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

#pragma note( "cMovement::CheckMovementType() is currently unrefrenced, are we planning on making use of it in the future?" )
SI16 cMovement::CheckMovementType( CChar *c )
{
	// Am I a GM Body?
	if( IsGMBody( c ) )
		return P_C_IS_GM_BODY;

	// Am I a player?
	if( !c->IsNpc() )
		return P_C_IS_PLAYER;

	// Change this to a flag in NPC.scp

	SI16 retval = P_C_IS_NPC;
	switch( c->GetID() )
	{
	case 0x0010 : // Water Elemental
		retval |= P_C_IS_FISH;
		break;
	case 0x005F : // Kraken
	case 0x0096 : // Sea Serpent
	case 0x0097 : // Dolphin
		retval = P_C_IS_FISH;
		break;
	default : // Regular NPC
		break;
	}
	return retval;
}

// Examins a location (cx, cy, cz) to determine if a character is found there.  If so, return true
// If not, return false.  Used for the npc stacking problem
bool cMovement::CheckForCharacterAtXYZ( CChar *c, SI16 cx, SI16 cy, SI08 cz )
{
	int xOffset = MapRegion->GetGridX( cx );
	int yOffset = MapRegion->GetGridY( cy );
	SubRegion *MapArea = MapRegion->GetGrid( xOffset, yOffset, c->WorldNumber() );	// check 3 cols... do we really NEED to?
	if( MapArea == NULL )	// no valid region
		return false;
	MapArea->PushChar();
	for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
	{
		if( tempChar == NULL )
			continue;
		if( tempChar != c && ( isOnline( tempChar ) || tempChar->IsNpc() ) )
		{	// x=x,y=y, and distance btw z's <= MAX STEP
			if( tempChar->GetX() == cx && tempChar->GetY() == cy && tempChar->GetZ() >= cz && tempChar->GetZ() <= (cz + 5) )	// 2 people will still bump into each other, if slightly offset
			{
				MapArea->PopChar();	// restore before returning
				return true;
			}
		}
	}
	MapArea->PopChar();
	return false;
}

// check if GM Body

bool cMovement::CanGMWalk( CTileUni &xyb )
{
	UI16 blockid = xyb.ID();

	CTile newTile;
	Map->SeekTile( blockid, &newTile );

	if( Map->IsRoofOrFloorTile( &newTile ) )
		return true;
	
	if( xyb.Type() == 0 )		// map tile
		return true;

	if( xyb.Blocking() )	// blocking
		return true;

	if( xyb.Flag2() & 0x16 )	// climbable, standable, windows/doors
		return true;

	if( xyb.Door() )	// door
		return true;

	return false;
}

bool cMovement::CanNPCWalk( CTileUni &xyb )
{
	UI16 blockid = xyb.ID();

	CTile newTile;
	Map->SeekTile( blockid, &newTile );

	if( Map->IsRoofOrFloorTile( &newTile ) )
		return true;
	
	if( xyb.Type() == 0 )
		return true;

	if( xyb.Standable() || xyb.Climbable() )
		return true;

	return false;
}

bool cMovement::CanPlayerWalk( CTileUni &xyb )
{
	UI16 blockid = xyb.ID();

	CTile newTile;
	Map->SeekTile( blockid, &newTile );

	if( Map->IsRoofOrFloorTile( &newTile ) )
		return true;
	
	if( xyb.Type() == 0 )			// map tile
		return true;

	if( xyb.Standable() || xyb.Climbable() )		// standable, climbable
		return true;

	return false;
}


bool cMovement::CanFishWalk( CTileUni &xyb )
{
	UI16 blockid = xyb.ID();
	
	if( Map->IsTileWet( blockid ) )
		return true;

	// Can they walk/swim on water tiles?
	if( blockid > 0x00A7 && blockid < 0x00AC )
		return true;
	if( blockid > 0x1796 && blockid < 0x179D )
		return true;
	if( blockid > 0x346D && blockid < 0x3486 )
		return true;
	if( blockid > 0x3493 && blockid < 0x34AC )
		return true;
	if( blockid > 0x34B7 && blockid < 0x34CB )
		return true;

	// Can they walk/swim on water ripples and splashes?
	if( blockid > 0x34D0 && blockid < 0x34D6 )
		return true;
	if( blockid > 0x352C && blockid < 0x3531 )
		return true;

	// Can they walk/swim on whirlpools?
//	if( blockid > 0x348F && blockid < 0x3494 )
//		return true;
//	if( blockid > 0x34B4 && blockid < 0x34B8 )
//		return true;

	// Can they walk/swim on/up waterfalls?
	if( blockid > 0x34EC && blockid < 0x3529 )
		return true;

	// Can they walk/swim on the coastlines?
//	if( blockid > 0x179C && blockid < 0x17B3 )
//		return true;
//	if( blockid == 0x1796 )
//		return true;

	return false;
}

// needs testing... not totally accurate, but something to hold place.

bool cMovement::CanBirdWalk( CTileUni &xyb )
{
	return ( CanNPCWalk( xyb ) || CanFishWalk( xyb ) );
}

// if we have a valid socket, see if we need to deny the movement request because of
// something to do with the walk sequence being out of sync.
bool cMovement::VerifySequence( CChar *c, cSocket *mSock, SI16 sequence )
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
		if( !c->IsNpc() )
			c->SetNpcMoveTime( uiCurrentTime + ( ( cwmWorldState->ServerData()->GetSystemTimerStatus( STAMINA_REGEN ) * CLOCKS_PER_SEC ) / 2 ) );

		// if we are using stealth
		if( c->GetStealth() != -1 )	// Stealth - stop hiding if player runs
			c->ExposeToView();
		//Don't regenerate stamina while running
		c->SetRegen( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( STAMINA_REGEN ) )), 1 );
		c->SetRunning( c->GetRunning() + 1 );

		if( !c->IsDead() && c->GetCommandLevel() < CNSCMDLEVEL )
		{
			bool reduceStamina = ( c->IsOnHorse() && c->GetRunning() > ( cwmWorldState->ServerData()->GetMaxStaminaMovement() * 2 ) );
			if( !reduceStamina )
				reduceStamina = ( c->GetRunning() > ( cwmWorldState->ServerData()->GetMaxStaminaMovement() * 4 ) );
			if( reduceStamina )
			{
				c->SetRunning( 0 );
				c->SetStamina( c->GetStamina() - 1 );
				updateStats( c, 2 );
			}
		}
		if( c->IsAtWar() && c->GetTarg() != INVALIDSERIAL )
			c->SetTimeout( BuildTimeValue( 2 ) );

	}
	else
	{
		c->SetRunning( 0 );
		if( !c->IsNpc() )
			c->SetNpcMoveTime( uiCurrentTime + ( ( cwmWorldState->ServerData()->GetSystemTimerStatus( STAMINA_REGEN ) * CLOCKS_PER_SEC ) / 4 ) );
	}
	return true;
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cMovement::CheckForStealth( CChar *c )
//|	Date					-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
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
	if( c->GetHidden() && !c->IsPermHidden() )
	{
		// Sept 22, 2002 - Xuri
		if(c->IsOnHorse())
		{
			if(!cwmWorldState->ServerData()->GetCharHideWhileMounted())
				c->ExposeToView();
		}
		//
		if( c->GetStealth() != -1 )
		{
			c->SetStealth( c->GetStealth() + 1 );
			if( c->GetStealth() > ( cwmWorldState->ServerData()->GetMaxStealthMovement() * c->GetSkill( STEALTH ) / 1000 ) )
				c->ExposeToView();
		}
		else
			c->ExposeToView();
	}
	return true;
}

// see if a player has tried to move into a house they were banned from
bool cMovement::CheckForHouseBan( CChar *c, cSocket *mSock )
{
	if( !c->IsNpc() ) // this code is also called from npcs-walking code, so only check for players to cut down lag!
	{
		CMultiObj *house = findMulti( c );
		if( house != NULL ) 
		{
			if( c->GetMultiObj() != house )
				c->SetMulti( house );//Set them inside the multi!
			if( house->IsOnBanList( c ) )
			{
				SI16 sx, sy, ex, ey;
				Map->MultiArea( house, sx, sy, ex, ey );
				c->SetLocation( ex, ey + 1, c->GetZ() );
				c->Teleport();
				if( mSock != NULL )
				{
					sysmessage( mSock, 1639 );
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
	} 
	return true;
}

// Thyme 2000.09.21
// I already made sure I could move there (even the crazy XY block stuff) so this IS a valid move. Just move the
// directions. Oh, and since I we already have the GetX/YfromDir functions (and we need those) why don't we just
// use them here?

void cMovement::MoveCharForDirection( CChar *c, UI08 dir )
{
	c->SetX( GetXfromDir( dir, c->GetX() ) );
	c->SetY( GetYfromDir( dir, c->GetY() ) );
}

// Split up of FillXYBlockStuff


void cMovement::GetBlockingMap( SI16 x, SI16 y, CTileUni *xyblock, int &xycount, SI16 oldx, SI16 oldy, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;
	// This oldx, oldy stuff is... problematic, to say the least
	UI16 mapid = 0;
	UI16 mapid_old = 0;
	SI08 mapz = Map->AverageMapElevation( x, y, mapid, worldNumber );
	SI08 mapz_old = Map->AverageMapElevation( oldx, oldy, mapid_old, worldNumber );
	if( mapz_old <= illegal_z )	
		mapz_old = mapz;
	if( mapz != illegal_z )
	{
		CLand land;
		Map->SeekLand( mapid, &land );
		
		xyblock[xycount].Type( 0 );
		xyblock[xycount].BaseZ( min( mapz_old, mapz ) );
		xyblock[xycount].ID( mapid );
		xyblock[xycount] = land;
		xyblock[xycount].Height( mapz - xyblock[xycount].BaseZ() );
		xyblock[xycount].Weight( 255 );
		xycount++;
	}
}


void cMovement::GetBlockingStatics( SI16 x, SI16 y, CTileUni *xyblock, int &xycount, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;
	MapStaticIterator msi( x, y, worldNumber );
	staticrecord *stat = NULL;
 	while( stat = msi.Next() )
	{
		CTile tile;
		msi.GetTile( &tile );
		xyblock[xycount].Type( 2 );
		xyblock[xycount].BaseZ( stat->zoff );
		xyblock[xycount].ID( stat->itemid );
		xyblock[xycount] = tile;
		xyblock[xycount].Weight( 255 );
		xycount++;
		if( xycount >= XYMAX )	// don't overflow
			return;
	}
}

void cMovement::GetBlockingDynamics( SI16 x, SI16 y, CTileUni *xyblock, int &xycount, UI08 worldNumber )
{
	if( xycount >= XYMAX )	// don't overflow
		return;
	int xOffset = MapRegion->GetGridX( x );
	int yOffset = MapRegion->GetGridY( y );
	SubRegion *MapArea = MapRegion->GetGrid( xOffset, yOffset, worldNumber );
	if( MapArea == NULL )	// no valid region
		return;
	MapArea->PushItem();
	for( CItem *tItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tItem = MapArea->GetNextItem() )
	{
		if( tItem == NULL )
			continue;
		if( tItem->GetID() < 0x4000 )
		{
#if DEBUG_WALKING
			Console.Print( "DEBUG: Item X: %i\nItem Y: %i\n", tItem->GetX(), tItem->GetY() );
#endif
			if( tItem->GetX() == x && tItem->GetY() == y )
			{
				CTile tile;
				Map->SeekTile( tItem->GetID(), &tile );
				xyblock[xycount].Type( 1 );
				xyblock[xycount].BaseZ( tItem->GetZ() );
				xyblock[xycount].ID( tItem->GetID() );
				xyblock[xycount] = tile;
				xycount++;
				if( xycount >= XYMAX )	// don't overflow
					return;
			}
		}
		else if( abs( tItem->GetX() - x ) <= BUILDRANGE && abs( tItem->GetY() - y) <= BUILDRANGE )
		{
			UI16 multiID = tItem->GetID() - 0x4000;
			SI32 length = 0;		// should be SI32, not long
			Map->SeekMulti( multiID, &length );
			if( length == -1 || length >= 17000000 ) //Too big... bug fix hopefully (Abaddon 13 Sept 1999)
			{
				Console.Error( 2, "Walking() - Bad length in multi file. Avoiding stall" );
				length = 0;
			}
			for( int j = 0; j < length; j++ )
			{
				st_multi *multi;
				multi = Map->SeekIntoMulti( multiID, j );
				if( multi->visible && (tItem->GetX() + multi->x) == x && (tItem->GetY() + multi->y) == y )
				{
					CTile tile;
					Map->SeekTile( multi->tile, &tile );
					xyblock[xycount].Type( 2 );
					xyblock[xycount].BaseZ( multi->z + tItem->GetZ() );
					xyblock[xycount].ID( multi->tile );
					xyblock[xycount] = tile;
					xyblock[xycount].Weight( 255 );
					xycount++;
					if( xycount >= XYMAX )	// don't overflow
						return;
				}
			}
		}
	}
	MapArea->PopItem();
}

// so we are going to move, lets update the regions
// FYI, Items equal to or greater than 1000000 are considered characters...
void cMovement::HandleRegionStuffAfterMove( CChar *c, SI16 oldx, SI16 oldy )
{
	// save where we were moving to
	const SI16 nowx = c->GetX();
	const SI16 nowy = c->GetY();

	// i'm trying a new optimization here, if we end up in the same map cell
	// as we started, i'm sure there's no real reason to remove and readd back
	// to the same spot..
	UI08 worldNumber = c->WorldNumber();
	SubRegion *cell1 = MapRegion->GetCell( oldx, oldy, worldNumber );
	SubRegion *cell2 = MapRegion->GetCell( nowx, nowy, worldNumber );
	if( cell1 != cell2 )
	{
		cell1->RemoveChar( c );
		cell2->AddChar( c );
	}
#if DEBUG_WALKING
	else
		Console.Print( "DEBUG: Character: %s(%i) didn't change regions.", c->GetName(), c->GetSerial() );
#endif

	// i'm moving this to the end because the regions shouldn't care what the z was
	c->SetDispZ( dispz );
	c->SetZ( z );
}


// actually send the walk command back to the player and increment the sequence
void cMovement::SendWalkToPlayer( CChar *c, cSocket *mSock, SI16 sequence )
{
	if( mSock != NULL )
	{
		CPWalkOK toSend;

		toSend.SequenceNumber( mSock->GetByte( 2 ) );
		if( c->GetHidden() )
			toSend.OtherByte( 0 );
		else
			toSend.OtherByte( 0x41 );

		mSock->Send( &toSend );
		mSock->WalkSequence( sequence );
		if( mSock->WalkSequence() == 255 )
			mSock->WalkSequence( 0 );
	}
}

// send out our movement to all other players who can see us move
void cMovement::SendWalkToOtherPlayers( CChar *c, UI08 dir, SI16 oldx, SI16 oldy )
{
	// lets cache these vars in advance
	const SI16 newx = c->GetX();
	const SI16 newy = c->GetY();

	CPExtMove toSend = (*c);
	if( c->IsNpc() && c->CanRun() && c->IsAtWar() )
		toSend.Direction( dir|0x80 );
	char flag = 0;
	if( c->IsAtWar() ) 
		flag = 0x40; 
	if( c->GetHidden() ) 
		flag |= 0x80;
	if( c->IsDead() && !c->IsAtWar() ) 
		flag |= 0x80;
	if( c->GetPoisoned() ) 
		flag |= 0x04;
	toSend.Flag( flag );

	Network->PushConn();
	for( cSocket *tSend = Network->FirstSocket(); !Network->FinishedSockets(); tSend = Network->NextSocket() )
	{	// lets see, its much cheaper to call perm[i] first so i'm reordering this
		if( tSend == NULL )
			continue;
		CChar *mChar = tSend->CurrcharObj();
		if( mChar == NULL )
			continue;
		const UI08 visibleRange = (UI08)(tSend->Range() + Races->VisRange( mChar->GetRace() ) + 1);
		const UI08 clientRange = tSend->Range();
		if( c != mChar && objInRange( c, mChar, visibleRange ) )
		{
			SI16 dxNew = (SI16)abs( newx - mChar->GetX() );
			SI16 dyNew = (SI16)abs( newy - mChar->GetY() );

			SI16 dxOld = (SI16)abs( oldx - mChar->GetX() );
			SI16 dyOld = (SI16)abs( oldy - mChar->GetY() );

			// if new y or new x is limit of our range, and ( old x or y is greater than our range (ie couldn't see it) 
			// or it was JUST in rnage), send the character
//			if(	( ( dxNew == visibleRange || dyNew == visibleRange ) )
//				&& ( dxOld > visibleRange || dyOld > visibleRange ) ||
//				( dxNew == visibleRange && dyNew == visibleRange ) )

			// sereg, ok we had some problems with it (non-appearing chars), lets try this way till someone gets a more slighter/cheaper idea
			if( ( ( ( ( dxNew >= clientRange ) && ( dxNew <= visibleRange ) ) ||
				  ( ( dyNew >= clientRange ) && ( dyNew <= visibleRange ) ) ) &&
				( ( dxOld > clientRange ) || ( dyOld > clientRange ) ) ) ||
				( ( dxNew == clientRange ) && ( dyNew == clientRange ) ) )
			{
				c->SendToSocket( tSend, true, c );
			}
			else
			{
				toSend.FlagColour( FlagColour( c, mChar ) );
				tSend->Send( &toSend );
			}
		}
	}
	Network->PopConn();
}

// see if we should mention that we shove something out of the way
void cMovement::OutputShoveMessage( CChar *c, cSocket *mSock )
{
	if( mSock == NULL )
		return;

	// GMs, counselors, and ghosts don't shove things
	if( c->GetCommandLevel() >= CNSCMDLEVEL || c->GetID() == 0x03DB || c->IsDead() )
		return;
	// lets cache these vars in advance
	UI08 worldNumber = c->WorldNumber();
	SubRegion *grid = MapRegion->GetCell( c->GetX(), c->GetY(), worldNumber );
	if( grid == NULL )
		return;
	CChar *ourChar = NULL;
	grid->PushChar();
	SI16 x = c->GetX();
	SI16 y = c->GetY();
	SI08 z = c->GetZ();
	UI16 targTrig = c->GetScriptTrigger();
	cScript *toExecute = Trigger->GetScript( targTrig );
	for( ourChar = grid->FirstChar(); !grid->FinishedChars(); ourChar = grid->GetNextChar() )
	{
		if( ourChar == NULL )
			continue;
		if( ourChar != c && ( ourChar->IsNpc() || isOnline( ourChar ) ) )
		{
			if( ourChar->GetX() == x && ourChar->GetY() == y && ourChar->GetZ() == z )
			{
				if( toExecute != NULL )
					toExecute->OnCollide( mSock, c, ourChar );
				UI16 tTrig = ourChar->GetScriptTrigger();
				cScript *tExec = Trigger->GetScript( tTrig );
				if( tExec != NULL )
					tExec->OnCollide( calcSocketObjFromChar( ourChar ), ourChar, c );
				if( !ourChar->GetHidden() && !ourChar->IsPermHidden() )
				{
					sysmessage( mSock, 1383, ourChar->GetName() );
					c->SetStamina( max( c->GetStamina() - 4, 0 ) );
					updateStats( c, 2 );  // arm code
				}
				else if( ourChar->GetCommandLevel() < CNSCMDLEVEL )
				{
					sysmessage( mSock, 1384 );
					c->SetStamina( max( c->GetStamina() - 4, 0 ) );
					updateStats( c, 2 );  // arm code
				}
			}
		}
	}
	grid->PopChar();
}

// handle item collisions, make items that appear on the edge of our sight because
// visible, buildings when they get in range, and if the character steps on something
// that might cause damage
void cMovement::HandleItemCollision( CChar *mChar, cSocket *mSock, bool amTurning, SI16 oldx, SI16 oldy )
{
	// lets cache these vars in advance
	UI08 visibleRange = 18;
	if( mSock != NULL )
		visibleRange = (UI08)(mSock->Range() + Races->VisRange( mChar->GetRace() ) + 1);
	const SI16 newx = mChar->GetX();
	const SI16 newy = mChar->GetY();
	UI16 id;
	bool EffRange;
	
	int xOffset = MapRegion->GetGridX( newx );
	int yOffset = MapRegion->GetGridY( newy );

	bool isGM = mChar->IsGM();
	SI16 dxNew, dyNew, dxOld, dyOld;
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

	UI08 worldNumber = mChar->WorldNumber();
	const SI16 justOut = (SI16)(visibleRange + 1);
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter2, yOffset + counter1, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			if( mSock != NULL )		// Only send char stuff if we have a valid socket
			{
				MapArea->PushChar();
				for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
				{
					if( tempChar == NULL )
						continue;
					// Character Send Stuff
					if( tempChar->IsNpc() || isOnline( tempChar ) || ( isGM && cwmWorldState->ServerData()->GetShowHiddenNpcStatus() ) )
					{
						dxNew = (SI16)abs( tempChar->GetX() - newx );
						dyNew = (SI16)abs( tempChar->GetY() - newy );
						dxOld = (SI16)abs( tempChar->GetX() - oldx );
						dyOld = (SI16)abs( tempChar->GetY() - oldy );

						if( ( dxNew == visibleRange || dyNew == visibleRange ) && ( dxOld > visibleRange || dyOld > visibleRange ) ||
							( dxNew == visibleRange && dyNew == visibleRange ) )
						{
							tempChar->SendToSocket( mSock, true, tempChar );
							UI16 targTrig = mChar->GetScriptTrigger();
							cScript *toExecute = Trigger->GetScript( targTrig );
							if( toExecute != NULL )
								toExecute->InRange( mChar, tempChar );
						}
						else if( ( dxNew == justOut && dxOld == visibleRange ) || ( dyNew == justOut && dyOld == visibleRange ) )
						{
							UI16 targTrig = mChar->GetScriptTrigger();
							cScript *toExecute = Trigger->GetScript( targTrig );
							if( toExecute != NULL )
								toExecute->OutOfRange( mChar, tempChar );
							// item out of range
						}
					}
				}
				MapArea->PopChar();
			}
			CItem *tItem = NULL;
			MapArea->PushItem();
			CChar *caster = NULL;
			for( tItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tItem = MapArea->GetNextItem() )
			{
				if( tItem == NULL )
					continue;
				id = tItem->GetID();
				// Item Send Stuff
				// Why recalculate all this stuff several times?
				EffRange = ( ( tItem->GetX() == newx ) && 
							 ( tItem->GetY() == newy ) && 
							 ( mChar->GetZ() >= tItem->GetZ() ) && 
							 ( mChar->GetZ() <= ( tItem->GetZ() + 5 ) ) );
				if( EffRange )
				{
					switch( id )
					{
					case 0x3996:	// Fire Field
					case 0x398C:
						if( mChar->IsInnocent() )
						{
							caster = calcCharObjFromSer( tItem->GetMoreY() );	// store caster in morey
							if( mChar->IsInnocent() && caster != NULL && caster->GetCommandLevel() < CNSCMDLEVEL )
								criminal( caster );
						}
						soundeffect( mChar, 520 );
						if( !Magic->CheckResist( NULL, mChar, 4 ) )
							Magic->MagicDamage( mChar, tItem->GetMoreX() / 300 );

						break;
					case 0x3915:
					case 0x3920:	// Poison field
						if( mChar->IsInnocent() )
						{
							caster = calcCharObjFromSer( tItem->GetMoreY() );	// store caster in morey
							if( mChar->IsInnocent() && caster != NULL && caster->GetCommandLevel() < CNSCMDLEVEL )
								criminal( caster );
						}
						soundeffect( mChar, 520 );
						if( !Magic->CheckResist( NULL, mChar, 5 ) )
							Magic->PoisonDamage( mChar, 1 );

						break;
					case 0x3979:	// Paralyze Field
					case 0x3967:
						if( mChar->IsInnocent() )
						{
							caster = calcCharObjFromSer( tItem->GetMoreY() );	// store caster in morey
							if( mChar->IsInnocent() && caster != NULL && caster->GetCommandLevel() < CNSCMDLEVEL )
								criminal( caster );
						}
						soundeffect( mChar, 0x0204 );
						if( !Magic->CheckResist( NULL, mChar, 6 ) )
							tempeffect( mChar, mChar, 1, 0, 0, 0 );

						break;
					default:
						if( id < 0x4000 )
						{
							UI16 targTrig = tItem->GetScriptTrigger();
							cScript *toExecute = Trigger->GetScript( targTrig );
							if( toExecute != NULL )
								toExecute->OnCollide( mSock, mChar, tItem );
						}
						break;
					}
				}//if( EffRange )

				// always check for new items if we actually moved
				if( !amTurning )
				{
					dxNew = (SI16)abs( tItem->GetX() - newx );
					dyNew = (SI16)abs( tItem->GetY() - newy );
					dxOld = (SI16)abs( tItem->GetX() - oldx );
					dyOld = (SI16)abs( tItem->GetY() - oldy );
					// is the item a building on the BUILDRANGE?
					if( id >= 0x4000 && mSock != NULL )
					{
						if( ( (id >= 0x407A && id <= 0x407F) || id == 0x5388 ) && ( dxNew == BUILDRANGE || dyNew == BUILDRANGE ) )
							sendItem( mSock, tItem );
						else if( ( dxNew == visibleRange || dyNew == visibleRange ) && ( dxOld > visibleRange || dyOld > visibleRange ) )
							sendItem( mSock, tItem );
					}
					else
					{
						if( ( dxNew == visibleRange || dyNew == visibleRange ) && ( dxOld > visibleRange || dyOld > visibleRange ) ||
							( dxNew == visibleRange && dyNew == visibleRange ) )
						{//just came into range?
							if( tItem->GetVisible() < 2 || isGM )// we're a GM, or not hidden
							{
								sendItem( mSock, tItem );
								UI16 targTrig = mChar->GetScriptTrigger();
								cScript *toExecute = Trigger->GetScript( targTrig );
								if( toExecute != NULL )
									toExecute->InRange( mChar, tItem );
							}
						}
						else if( ( dxNew == justOut && dxOld == visibleRange ) || ( dyNew == justOut && dyOld == visibleRange ) )
						{//leaving range?
							UI16 targTrig = mChar->GetScriptTrigger();
							cScript *toExecute = Trigger->GetScript( targTrig );
							if( toExecute != NULL )
								toExecute->OutOfRange( mChar, tItem );
							// item out of range
						}
					}
				}
			}
			MapArea->PopItem();
		}
	}
}

void cMovement::HandleTeleporters( CChar *c, SI16 oldx, SI16 oldy )
{
	if( c->GetX() != oldx || c->GetY() != oldy )
	{
		if( !c->IsNpc() )
			objTeleporters( c );
		teleporters( c );
	}
}

// start of LB's no rain & snow in buildings stuff 
#pragma note( "Param Warning: in cMovement::HandleWeatherChanges(), mSock is unrefrenced" )
void cMovement::HandleWeatherChanges( CChar *c, cSocket *mSock )
{
	if( !c->IsNpc() && isOnline( c ) ) // check for being in buildings (for weather) only for PC's
		return;
}

// return whether someone is a GM Body
bool cMovement::IsGMBody( CChar *c )
{
	if( c->IsGM() || c->GetID() == 0x03DB || c->GetID() == 0x0192 || c->GetID() == 0x0193 || c->IsDead() ) 
        return true;
    return false;
}

void cMovement::CombatWalk( CChar *i ) // Only for switching to combat mode
{
	if( i == NULL )
		return;
	CPExtMove toSend = (*i);
	if( i->IsNpc() && i->CanRun() && i->IsAtWar() )
		toSend.Direction( i->GetDir()|0x80 );
	char flag = 0;
	if( i->IsAtWar() ) 
		flag = 0x40; 
	if( i->GetHidden() || ( i->IsDead() && !i->IsAtWar() ) )
		flag |= 0x80;
	if( i->GetPoisoned() ) 
		flag |= 0x04;
	toSend.Flag( flag );

	if( !i->IsAtWar() )
		i->SetTarg( INVALIDSERIAL );
    
	Network->PushConn();
	for( cSocket *tSend = Network->FirstSocket(); !Network->FinishedSockets(); tSend = Network->NextSocket() )
    {
		if( tSend == NULL )
			continue;
		CChar *mChar = tSend->CurrcharObj();
		if( mChar != i && charInRange( i, mChar ) )
        {
			toSend.FlagColour( FlagColour( i, mChar ) );
            tSend->Send( &toSend );
        }
    }
	Network->PopConn();
}

void cMovement::NpcWalk( CChar *i, UI08 j, SI08 getWander )   //type is npcwalk mode (0 for normal, 1 for box, 2 for circle)
{
	SI16 fx1 = i->GetFx( 1 );
	SI16 fx2 = i->GetFx( 2 );
	SI16 fy1 = i->GetFy( 1 );
	SI16 fy2 = i->GetFy( 2 );
	SI08 fz1 = i->GetFz();
    // if we are walking in an area, and the area is not properly defined, just don't bother with the area anymore
    if( ( ( getWander == 3 ) && ( fx1 == -1 || fx2 == -1 || fy1 == -1 || fy2 == -1 ) ) ||
        ( ( getWander == 4 ) && ( fx1 == -1 || fx2 == -1 || fy1 == -1 ) ) ) // circle's don't use fy2, so don't require them! fur 10/30/1999
        
    {
        i->SetNpcWander( 2 ); // Wander freely from now on
    }
	// Thyme New Stuff 2000.09.21
	SI16 newx = GetXfromDir( j, i->GetX() );
	SI16 newy = GetYfromDir( j, i->GetY() );
	UI08 worldNumber = i->WorldNumber();
	// Let's make this a little more readable.
	UI08 jMod = (j & 0x87);
	switch( getWander )
	{
	case 2:	// Wander freely
	case 5:	// Wander freely after fleeing
		Walking( i, jMod, 256 );
		break;
	case 3:	// Wander inside a box
		if( checkBoundingBox( newx, newy, fx1, fy1, fz1, fx2, fy2, worldNumber ) )
			Walking( i, jMod, 256 );
		break;
	case 4:	// Wander inside a circle
		if( checkBoundingCircle( newx, newy, fx1, fy1, fz1, fx2, worldNumber ) )
			Walking( i, jMod, 256 );
		break;
	default:
		Console.Error( 2, "Bad NPC Wander type passed to NpcWalk: %i\n", getWander );
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

	switch ( dir & 0x07 )
	{
	case 0x00 :
	case 0x01 :
	case 0x07 :
		y--; break;
	case 0x03 :
	case 0x04 :
	case 0x05 :
		y++; break;
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

   	switch ( dir & 0x07 )
	{
	case 0x01 :
	case 0x02 :
	case 0x03 :
		x++; break;
	case 0x05 :
	case 0x06 :
	case 0x07 :
		x--; break;
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
	if( c == NULL ) 
		return;

	// Make sure the character has taken used all of their previously saved steps
	if( c->StillGotDirs() ) 
		return;

	// Thyme 2000.09.21
	// initial rewrite of pathfinding...

	SI16 newx, newy;
	SI08 newz;
	const SI16 oldx = c->GetX();
	const SI16 oldy = c->GetY();

#pragma note( "PathFind() needs to be touched up, UI08 can possibly be set to -1" )
	for( UI08 pn = 0; pn < pathLen; pn++ )
	{
		bool bFound = false;
		int pf_neg = ( ( RandomNum( 0, 1 ) ) ? 1 : -1 );
		UI08 pf_dir = Direction( oldx, oldy, gx, gy );
		for( UI08 i = 0 ; i < 8 ; i++ )
		{
			pf_neg *= -1;
			pf_dir += ( i * pf_neg );
			if( pf_dir < 8 && calc_move( c, oldx, oldy, newz, pf_dir ) )
			{
				newx = GetXfromDir( pf_dir, oldx );	// moved inside if to reduce calculations
				newy = GetYfromDir( pf_dir, oldy );
				if( ( pn < P_PF_MRV ) && CheckForCharacterAtXYZ( c, newx, newy, newz ) )
					continue;

				if( willRun )
					c->PushDirection( pf_dir | 0x80 );	// or it with 0x80 to ensure they run
				else
					c->PushDirection( pf_dir );
				bFound = true;
				break;
			}
		}
		if( !bFound )
		{
#if DEBUG_PATHFIND
			Console << "Character stuck!" << myendl;
#endif
			break;
		}
	}
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	void cMovement::NpcMovement( CChar *i )
//|	Date					-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Calculate, and handle NPC AI movement
//|									
//|	Modification	-	09/22/2002	-	Fixed fleeing NPCs by reversing values for 
//|									xfactor & yfactor lines
//o--------------------------------------------------------------------------o
//|	Returns				- NA
//o--------------------------------------------------------------------------o	
void cMovement::NpcMovement( CChar *i )
{
    CHARACTER k;
	UI08 j;
	const R32 npcSpeed = static_cast< R32 >(cwmWorldState->ServerData()->GetNPCSpeed());
    if( i->IsNpc() && ( i->GetNpcMoveTime() <= uiCurrentTime || overflow ) )
    {
#if DEBUG_NPCWALK
		Console.Print( "DEBUG: ENTER (%s): %d AI %d WAR %d J\n", chars[i].GetName(), chars[i].GetNpcWander(), chars[i].IsAtWar(), j );
#endif
		if( i->IsAtWar() && i->GetNpcWander() != 5 )
        {
            CHARACTER l = i->GetAttacker();
            if( l != INVALIDSERIAL && ( isOnline( &chars[l] ) || chars[l].IsNpc() ) )
            {
				UI08 charDir = getCharDir( i, chars[l].GetX(), chars[l].GetY() );
				UI16 charDist = getDist( i, &chars[l] );
                if( charDir < 8 && ( charDist <= 1 || ( Combat->getCombatSkill( i ) == ARCHERY && charDist <= 3 ) ) )
				{
					i->FlushPath();

					cSocket *mSock = calcSocketObjFromChar( i );
					bool los = LineOfSight( mSock, i, chars[l].GetX(), chars[l].GetY(), chars[l].GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING );
					if( los && charDir != i->GetDir() )
					{
						i->SetDir( charDir );
						Walking( i, charDir, 256 );
						return;
					}
				}
				else
                {
					PathFind( i, chars[l].GetX(), chars[l].GetY() );
					j = i->PopDirection();
					Walking( i, j, 256 );
                }	
	        }
			else
				i->FlushPath();
        }
        else
        {
            switch( i->GetNpcWander() )
            {
            case 0: // No movement
                break;
            case 1: // Follow the follow target
                k = i->GetFTarg();
                if( k == INVALIDSERIAL || k >= cmem ) 
					return;
                if( isOnline( &chars[k] ) || chars[k].IsNpc() )
                {
					UI08 charDir = getCharDir( i, chars[k].GetX(), chars[k].GetY() );
					if( !objInRange( i, &chars[k], 1 ) && charDir < 8 )
                    {
                        PathFind( i, chars[k].GetX(), chars[k].GetY() );
						j = i->PopDirection();
                        Walking( i, j, 256 );
                    }
					// Dupois - Added April 4, 1999
					// Has the Escortee reached the destination ??
					// no need for -1 check on k, as we wouldn't be here if that were true
					if( !chars[k].IsDead() && i->GetQuestDestRegion() == i->GetRegion() )	// Pay the Escortee and free the NPC
						MsgBoardQuestEscortArrive( i, calcSocketObjFromChar( k ) );
                }
                break;
            case 2: // Wander freely, avoiding obstacles.
            case 3: // Wander freely, within a defined box
            case 4: // Wander freely, within a defined circle
				j = RandomNum( 1, 5 );
				if( j == 1 )
					break;
				else if( j == 2 )
					j = RandomNum( 0, 8 );
				else	// Move in the same direction the majority of the time
                    j = i->GetDir();
                NpcWalk( i, j, i->GetNpcWander() );
                break;
            case 5: //FLEE!!!!!!
                k = i->GetTarg();
                if( k == INVALIDSERIAL || k >= cmem ) 
					return;
				if( getDist( i, &chars[k] ) < P_PF_MFD )
				{	// calculate a x,y to flee towards
					UI16 mydist = P_PF_MFD - getDist( i, &chars[k] ) + 1;
					j = getCharDir( i, chars[k].GetX(), chars[k].GetY() );
					SI16 myx = GetXfromDir( j, i->GetX() );
					SI16 myy = GetYfromDir( j, i->GetY() );

					SI16 xfactor = 0;
					SI16 yfactor = 0;
					// Sept 22, 2002 - Xuri
					if( myx != i->GetX() )
					{
						if( myx < i->GetX() )
							xfactor = 1;
						else
							xfactor = -1;
					}

					if( myy != i->GetY() )
					{
						if( myy < i->GetY() )
							yfactor = 1;
						else
							yfactor = -1;
					}
					//
					myx += (SI16)( xfactor * mydist );
					myy += (SI16)( yfactor * mydist );

					// now, got myx, myy... lets go.
					PathFind( i, myx, myy );
					j = i->PopDirection();
					Walking( i, j, 256 );
				}
				else
				{ // wander freely... don't just stop because I'm out of range.
					j = RandomNum( 1, 5 );
					if( j == 1 )
						break;
					else if( j == 2 )
						j = RandomNum( 0, 8 );
					else	// Move in the same direction the majority of the time
        				j = i->GetDir();
					NpcWalk( i, j, i->GetNpcWander() );
				}
                break;
            default:
                break;
            }
        }
		if( i->GetNpcWander() == 1 )	// Followers need to keep up with Players
			i->SetNpcMoveTime( BuildTimeValue( static_cast< R32 >(npcSpeed / 4) ) );
		else
			i->SetNpcMoveTime( BuildTimeValue( npcSpeed ) );
    }
}

// This is my attempt at the writing a more effective pathfinding algorithm/sequence

// Function      : cMovement::Distance
// Written by    : Thyme
// Revision Date : 2000.09.08
// Purpose       : Calculate the shortest walkable distance between two points.
// Method        : The methodology behind this is pretty simple actually. The shortest distance
// between two walkable points would be to walk a diagonal line until sx=dx or sy=dy
// and then follow the line until the goal is reached. Since a diagonal step is
// the same distance as a lateral step, there's no need to use the Pythagorean theorem
// in the calculation. This calculation does not take into account any blocking objects.
// It will be used as a heuristic in determining priority of movement.

SI16 cMovement::Distance( SI16 sx, SI16 sy, SI16 dx, SI16 dy )
{
	return (SI16)( ( abs(sx-dx) < abs(sy-dy) ) ? abs(sy-dy) : abs(sx-dx) );
}

// Function      : cMovement::CanCharWalk()
// Written by    : Dupois
// Revised by    : Thyme
// Revision Date : 2000.09.17
// Purpose       : Check if a character can walk to a specified x,y location
// Method        : Modified the old CheckWalkable function so that it can be utilized throughout
// the walking code. Ever wonder why NPCs can walk through walls and stuff in combat mode? This
// is the fix, plus more.

UI08 cMovement::Direction( SI16 sx, SI16 sy, SI16 dx, SI16 dy )
{
	UI08 dir;
	
	SI16 xdif = (SI16)(dx - sx);
	SI16 ydif = (SI16)(dy - sy);
	
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
    CHARACTER c           Character's index in chars[]
	int x, y			  new cords.
	int oldx, oldy		  old cords.
	bool justask		  don't make any changes, the func. is just asked
						  "what if"..

  Return code:
    new z-value        if not blocked
    illegal_z == -128, if walk is blocked

********************************************************/
SI08 cMovement::calc_walk( CChar *c, SI16 x, SI16 y, SI16 oldx, SI16 oldy, bool justask )
{
	if( c == NULL )
		return illegal_z;
	const SI08 oldz = c->GetZ();
	bool may_levitate = c->MayLevitate();
	bool on_ladder = false;
	SI08 newz = illegal_z;
	bool blocked = false;
	char ontype = 0;

	int xycount = 0;
	UI08 worldNumber = c->WorldNumber();
	CTileUni xyblock[XYMAX];
	GetBlockingMap( x, y, xyblock, xycount, oldx, oldy, worldNumber );
	GetBlockingStatics( x, y, xyblock, xycount, worldNumber );
	GetBlockingDynamics( x, y, xyblock, xycount, worldNumber );

	// first calculate newZ value
	for( int i = 0; i < xycount; i++ )
	{
		CTileUni *tb = &xyblock[i]; // this is a easy/little tricky, to save a little calculation
		                                 // since the [i] is calclated several times below
			                             // if it doesn't help, it doesn't hurt either.
		SI08 nItemTop = (SI08)(tb->BaseZ() + ((xyblock[i].Type() == 0) ? xyblock[i].Height() : calcTileHeight( xyblock[i].Height() ) )); // Calculate the items total height

		// check if the creature is floating on a static (keeping Z or falling)
		if( nItemTop >= newz && nItemTop <= oldz )
		{
			if( tb->Standable() )
			{ // walkable tile
				newz = nItemTop;
				ontype = tb->Type();
				if( tb->ClimbableBit2() ) // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
				continue;
			}
		}

		// So now comes next step, levitation :o)
		// you can gain Z to a limited amount if yo uwere climbing on last move on a ladder
		if( nItemTop >= newz && may_levitate && nItemTop <= oldz + MAX_Z_LEVITATE && tb->Standable() )
		{
			ontype = tb->Type();
			newz = nItemTop;
			if( tb->ClimbableBit2() ) // if it was ladder the char is allowed to `levitate´ next move
				on_ladder = true;
		}
		// check if the creature is climbing on a climbable Z
		// (gaining Z through stairs, ladders, etc)
		// This form has no height limit, and the tile bottom must start lower or
		// equal current height + levitateable limit
		if( nItemTop >= newz && tb->BaseZ() <= oldz + MAX_Z_LEVITATE )
		{
			if( tb->Climbable() || tb->Type() == 0 ||			// Climbable tile, map tiles are also climbable
			( tb->Flag1() == 0 && tb->Flag2() == 0x22 ) ||		// These are a special kind of tiles where OSI forgot to set the climbable flag
			( (nItemTop >= oldz && nItemTop <= oldz + 3) && tb->Standable() ) )		 // Allow to climb a height of 1 even if the climbable flag is not set
			{                 
				ontype = tb->Type();
				newz = nItemTop;
				if( tb->ClimbableBit2() ) // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
			}
		}
	}

#if DEBUG_WALKING
		Console.Print( "DEBUG: CheckWalkable calculate Z=%d\n", newz );
#endif
    SI08 item_influence = higher( newz + MAX_ITEM_Z_INFLUENCE, oldz );
	// also take care to look on all tiles the creature has fallen through
	// (npc's walking on ocean bug)
	// now the new Z-cordinate of creature is known, 
	// check if it hits it's head against something (blocking in other words)
	bool isGM = IsGMBody( c );
	for( int ii = 0; ii < xycount; ii++)
	{
		CTileUni *tb = &xyblock[ii]; 
		SI32 nItemTop = tb->BaseZ() + ( ( tb->Type() == 0) ? tb->Height() : calcTileHeight( tb->Height() ) ); // Calculate the items total height
		if( ( tb->Blocking() || ( tb->Standable() && nItemTop > newz ) ) &&	// Check for blocking tile or stairs
			!( ( isGM || c->IsDead() ) && ( tb->WindowArchDoor() || tb->Door() ) ) )   // ghosts can walk through doors
		{                                                                    // blocking
			if( nItemTop > newz && tb->BaseZ() <= item_influence || ( nItemTop == newz && ontype == 0 ) )
			{ // in effact radius?
				newz = illegal_z;
#if DEBUG_WALKING
				Console.Print( "DEBUG: CheckWalkable blocked due to tile=%d at height=%d.\n", xyblock[ii].ID(), xyblock[ii].BaseZ() );
#endif
				blocked = true;
				break;
			}
		}
// knoxos: MAX_ITEM_Z_INFLUENCE is nice, but not truely correct,
//         since the creature height should be the effect radius, if you are i.e.
//         polymorphed to a "slime", you could go through things you normally 
//         wouldn't get under. (Just leaves the question what happens if you
//         unpolymorph in a place where you can't fit, lucky there are no
//         such gaps or tunnels in Britannia). 
//         (Well UO isn't ment to really think in 3d)
	}

#if DEBUG_WALK
	Console.Print( "DEBUG: CanCharWalk: %dx %dy %dz\n", x, y, z );
#endif
	if( (newz > illegal_z) && (!justask) ) // save information if we have climbed on last move.
		c->MayLevitate( on_ladder );
	return newz;
}

// Function      : cMovement::CanCharMove()
// Written by    : Thyme
// Revision Date : 2000.09.17
// Purpose       : Check if a character can walk to a from x,y to dir direction
// Method        : This handles the funky diagonal moves.

bool cMovement::calc_move( CChar *c, SI16 x, SI16 y, SI08 &z, UI08 dir)
{
	if( ( dir & 0x07 ) % 2 )
	{ // check three ways.
		UI08 ndir = turn_counter_clock_wise( dir );
		if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, true ) == illegal_z )
			return false;
		ndir = turn_clock_wise( dir );
		if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, true ) == illegal_z )
			return false;
	}
	z = calc_walk( c, GetXfromDir( dir, x ), GetYfromDir( dir, y ), x, y, false );
	return z > illegal_z;
}

#pragma note( "cMovement::MoveHeightAdjustment() is currently unrefrenced, are we planning on making use of it in the future?" )
bool cMovement::MoveHeightAdjustment( int MoveType, CTileUni *thisblock, int &ontype, SI32 &nItemTop, SI32 &nNewZ )
{
	if( ( MoveType & P_C_IS_GM_BODY ) && ( CanGMWalk( *(thisblock) ) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->Type();
		return true;
	}
	if( ( MoveType & P_C_IS_PLAYER ) && ( CanPlayerWalk( *(thisblock) ) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->Type();
		return true;
	}
	if( ( MoveType & P_C_IS_FISH ) && ( CanFishWalk( *(thisblock) ) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->Type();
		return true;
	}
	if( ( MoveType & P_C_IS_NPC ) && ( CanNPCWalk( *(thisblock) ) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->Type();
		return true;
	}
	if( ( MoveType & P_C_IS_BIRD ) && ( CanBirdWalk( *(thisblock) ) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->Type();
		return true;
	}
	return false;
}

// knox, reinserted it since some other files access it,
//       100% sure this is wrong, however the smaller ill.
bool cMovement::validNPCMove( SI16 x, SI16 y, SI08 z, CChar *s )
{
	UI08 worldNumber = s->WorldNumber();
    SubRegion *cell = MapRegion->GetCell( x, y, worldNumber );
	if( cell == NULL )
		return true;
    s->SetBlocked( s->IsBlocked() + 1 );
	cell->PushItem();
	for( CItem *tItem = cell->FirstItem(); !cell->FinishedItems(); tItem = cell->GetNextItem() )
	{
		if( tItem == NULL )
			continue;
		CTile tile;
		Map->SeekTile( tItem->GetID(), &tile );
		if( tItem->GetX() == x && tItem->GetY() == y && tItem->GetZ() + tile.Height() > z + 1 && tItem->GetZ() < z + MaxZstep )
		{
			UI16 id = tItem->GetID();
			if( id == 0x3946 || id == 0x3956 )
			{
				cell->PopItem();
				return false;
			}
			if( id <= 0x0200 || id >= 0x0300 && id <= 0x03E2 ) 
			{
				cell->PopItem();
				return false;
			}
			if( id > 0x0854 && id < 0x0866 ) 
			{
				cell->PopItem();
				return false;
			}
        
			if( tItem->GetType() == 12 )
			{
				if( s->IsNpc() && (strlen( s->GetTitle() ) > 0 || s->GetNPCAiType() != 0 ) )
					useDoor( NULL, tItem );
				s->SetBlocked( 0 );
				cell->PopItem();
				return false;
			}
		}
	}
	cell->PopItem();

    // see if the map says its ok to move here
    if( Map->CanMonsterMoveHere( x, y, z, worldNumber ) )
	{
		s->SetBlocked( 0 );
		return true;
	}
    return false;
}

void cMovement::deny( cSocket *mSock, CChar *s, SI16 sequence )
{
	CPWalkDeny denPack;

	denPack.SequenceNumber( sequence );
	denPack.X( s->GetX() );
	denPack.Y( s->GetY() );
	denPack.Direction( s->GetDir() );
	denPack.Z( s->GetDispZ() );

	if( mSock != NULL )
	{
		mSock->Send( &denPack );
		mSock->WalkSequence( -1 );
	}
}

