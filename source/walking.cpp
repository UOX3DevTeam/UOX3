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
#include "debug.h"


// These are defines that I'll use. I have a history of working with properties, so that's why
// I'm using custom definitions here versus what may be defined in the other includes.
// NOTE: P = Property, P_C = Property for Characters, P_PF = Property for Pathfinding
//       P_M = Property for Movement

#define P_C_PRIV_GM			0x01	// GM Privilege Bit
#define P_C_PRIV_COUNSELOR	0x80	// Counselor Privilege Bit

#define P_C_PRIV2_FROZEN	0x02	// Character Frozen Bit

#define P_C_IS_GM_BODY		0x01	// Bits for different movement types
#define P_C_IS_PLAYER		0x02
#define P_C_IS_BIRD			0x20
#define P_C_IS_NPC			0x40
#define P_C_IS_FISH			0x80

// Maximum Search Depth: Iterations to calculate
#define P_PF_MSD		5
// Maximum Return Value: Number of steps to return (Replaces PATHNUM)
// NOTE: P_PF_MRV CANNOT EXCEED THE VALUE OF PATHNUM FOR THE TIME BEING
#define P_PF_MRV		2
// Maximum Influence Range: Tiles to left/right of path to account for
#define P_PF_MIR		5
// Maximum Blocked Range: MIR to use if character is stuck
#define P_PF_MBR		10
// Minimum Flee Distance: MFD
#define P_PF_MFD		15


// These are the debugging defines

// These are the errors that we should want to see. RARELY results in any console spam unless
// someone is trying to use an exploit or is sending invalid data directly to the server.

#define DEBUG_WALKING 0
#define DEBUG_WALK_BLOCK 0

#define DEBUG_NPCWALK		0
#define DEBUG_WALK			0
#define DEBUG_PATHFIND		0

#define DBGFILE "walking.cpp"

#define MAX_ITEM_Z_INFLUENCE 10 // Any item above this height is discarded as being too far away to effect the char
#define MAX_Z_LEVITATE 10			// Maximum total height to reach a tile marked as 'LEVITATABLE'
												// Items with a mark as climbable have no height limit


inline signed int higher( signed int a, signed int b )
{
	if( a < b )
		return b;
	else
		return a;
}
inline signed int LOWER( signed int a, signed int b )
{
	if( a < b )
		return a;
	else
		return b;
}
inline unsigned int turn_clock_wise( unsigned int dir )
{
	unsigned char t = (dir & 0x07 ) + 1;
	return ( dir &0x80) | ( ( dir > 7 ) ? 0 : t );
}

inline unsigned int turn_counter_clock_wise( unsigned int dir )
{
	return ( dir & 0x80 ) | ( ( dir - 1 ) & 7 );
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

inline int calcTileHeight( int h )
{
  ///return ((h & 0x8) ? (((h & 0xF) ^ 0xF) + 1) : h & 0xF);
	//return (h & 0x7);
	//return ((h & 0x8) ? (((h & 0xF) ^ 0xF) + 1) : h & 0xF);
	return ((h & 0x8) ? ((h & 0xF) >> 1) : h & 0xF);
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
void cMovement::Walking(CHARACTER c, int dir, int sequence)
{
	// sometimes the NPC movement code comes up with -1, for example, if we are following someone
	// and we are directly on top of them
	// ^ That's because chardir returns -1 when direction is the same.  Actually, this
	// should only happen when a player walks on top of an NPC and the NPC tries to move.

	if( !isValidDirection( dir ) )
	{
		printf( "%s (cMovement::Walking) caught bad direction = %s %d 0x%x\n", DBGFILE, chars[c].name, dir, dir );
		// If I don't do this, the NPC will keep trying to walk on the same step, which is
		// where he's already at. Can cause an infinite loop. (Trust me, was one of the things
		// that locked up NW Alpha 2)
		// Will call something like NPCRandomMove in the future.
		if( chars[c].npc )
			chars[c].pathnum += P_PF_MRV;
		return;
	}

    UOXSOCKET socket = calcSocketFromChar( c );
    
    if( !VerifySequence( c, socket, sequence ) )
        return;

	// If checking for weight is more expensive, shouldn't we check for frozen first?
	if( isFrozen( c, socket, sequence ) )
		return;
    
    if( isOverloaded( c, socket, sequence ) )
        return;
    
    
	// save our original location before we even think about moving
	const short int oldx = chars[c].x;
	const short int oldy = chars[c].y;
	const signed char oldz = chars[c].z;
	
	// this if assumes that chars[s].dir has no high-bits just lets make sure of it
	// assert((chars[c].dir & 0xFFF0) == 0);
	// this assertion is failing, so either my assumption about it is wrong or there
	// is a bugaboo
	
	// see if we have stopped to turn or if we are moving
	const bool amTurning = ( (dir&0x07) != chars[c].dir );
	if (!amTurning)
	{
		if (!CheckForRunning(c, socket, dir))
			return;
		
		if (!CheckForStealth(c, socket))
			return;
		
		/* this is already done in the cNetwork method that calls this, so its redundant here
        ** i'm leaving this in because it might make more sense to have it here because we can
        * call it only in the case of actual movement
        if (chars[c].med) //Morrolan - Meditation
        {
            chars[c].med=0; 
            sysmessage(c, "You break your concentration.");
        }
		*/
		
		// if this was an NPC lets reset their move timer
		// this seems to be an usual place within this function to reset this
		// i guess they can turn a whole lot this way
		// Thyme: Already reset in NPCMovement (which calls this function, and NPCWalk)
		//if (chars[c].npc) 
		//{
		//	chars[c].npcmovetime=(unsigned int)(uiCurrentTime+(double)(NPCSPEED*CLOCKS_PER_SEC)); //reset move timer
		//}

		signed char myz = illegal_z;
		short int myx = GetXfromDir( dir, chars[c].x );
		short int myy = GetYfromDir( dir, chars[c].y );
		if( !calc_move( c, chars[c].x, chars[c].y, myz, dir ) )
		{
#if DEBUG_WALK
			printf("%s (cMovement::Walking) Character Walk Failed for %s\n", DBGFILE, chars[c].name);
			printf("%s (cMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, chars[c].x, chars[c].y, chars[c].z);
			printf("%s (cMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz);
#endif
			if( socket != -1 )
				deny( socket, c, sequence );
			if( chars[c].npc )
				chars[c].pathnum += P_PF_MRV;
			return;
		}
		dispz = z = myz;
#if DEBUG_WALK
		printf("%s (cMovement::Walking) Character Walk Passed for %s\n", DBGFILE, chars[c].name);
		printf("%s (cMovement::Walking) sx (%d) sy (%d) sz (%d)\n", DBGFILE, chars[c].x, chars[c].y, chars[c].z);
		printf("%s (cMovement::Walking) dx (%d) dy (%d) dz (%d)\n", DBGFILE, myx, myy, myz);
#endif

		if( chars[c].npc && CheckForCharacterAtXYZ( c, myx, myy, myz ) )
		{
			chars[c].pathnum += P_PF_MRV;
			return;
		}
		
		MoveCharForDirection(c, dir);
		
		// i actually moved this for now after the z =  illegal_z, in the end of CrazyXYBlockStuff()
		// can't see how that would hurt anything
		if (!CheckForHouseBan(c, socket))
			return;
		
		/*
		** OK AT THIS POINT IT IS NOW OFFICIALLY A LEGAL MOVE TO MAKE, LETS GO FOR IT!
		**
		** That means any bugs concerning if a move was legal must be before this point!
		*/

		// i moved this down after we are certain we are moving
		if( server_data.footSteps )
  			playTileSound( socket );
		
		// since we actually moved, update the regions code
		HandleRegionStuffAfterMove(c, oldx, oldy);            
	}
	else
	{
		//printf("Player is turning in the same spot.\n");
	}
	
	// do all of the following regardless of whether turning or moving i guess
	
	// set the player direction to contain only the cardinal direction bits
	chars[c].dir = (dir&0x07);
	
	SendWalkToPlayer(c, socket, sequence);
	
	SendWalkToOtherPlayers(c, dir, oldx, oldy);
	
	OutputShoveMessage(c, socket, oldx, oldy);
	
	// keep on checking this even if we just turned, because if you are taking damage
	// for standing here, lets keep on dishing it out. if we pass whether we actually
	// moved or not we can optimize things some
	HandleItemCollision( c, socket, amTurning, oldx, oldy );
	
	// i'm going ahead and optimizing this, if you haven't really moved, should be
	// no need to check for teleporters and the weather shouldn't change
	if (!amTurning)
	{
		HandleTeleporters(c, socket, oldx, oldy);
		HandleWeatherChanges(c, socket);
	}
	
	// i'm afraid i don't know what this does really, do you need to do it when turning??
	HandleGlowItems(c, socket);
	
	// would have already collided, right??
	if( !amTurning /*&& !chars[c].npc*/ )	// NPCs should be able to use gate
		Magic->GateCollision( c );
	
	// again, don't know if we need to check when turning or not
	if( !amTurning )
		checkregion(c); // doesn't change physical coords, so no point in making a change
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

bool cMovement::isValidDirection(int dir)
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

bool cMovement::isFrozen(CHARACTER c, UOXSOCKET socket, int sequence)
{
	if( chars[c].casting > 0 )
	{
		if( socket != -1 )
		{
			sysmessage( socket, "You cannot move while casting." );
			deny( socket, c, sequence );
		}
#if DEBUG_WALK
		printf( "%s (cMovement::isFrozen) casting char %s\n", DBGFILE, chars[c].name );
#endif
		return true;
	}
	if( chars[c].priv2 & P_C_PRIV2_FROZEN )
	{
		if( socket != -1 )
		{
			sysmessage( socket, "You are frozen and cannot move." );
			deny( socket, c, sequence );
		}
#if DEBUG_WALK
		printf( "%s (cMovement::isFrozen) frozen char %s\n", DBGFILE, chars[c].name );
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

bool cMovement::isOverloaded(CHARACTER c, UOXSOCKET socket, int sequence)
{
	// Who are we going to check for weight restrictions?
	if(  !chars[c].dead &&							// If they're not dead
		 !chars[c].npc &&							// they're not an npc
		 !(chars[c].priv & P_C_PRIV_GM ) &&			// they're not a GM
		 !(chars[c].priv & P_C_PRIV_COUNSELOR ) )	// they're not a counselor
	{
		// Can probably put this in the above check, but I'll keep it here for now.
		if( socket != -1 )
		{
			if( !Weight->CheckWeight( c, socket ) || ( chars[c].stm < 3 ) )
			{
				sysmessage( socket, "You are too fatigued to move" );
				deny( socket, c, sequence );
#if DEBUG_WALK
				printf( "%s (cMovement::Walking) overloaded char %s\n", DBGFILE, chars[c].name );
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

short int cMovement::CheckMovementType( CHARACTER c )
{
	// Am I a GM Body?
	if( IsGMBody( c ) )
		return P_C_IS_GM_BODY;

	// Am I a player?
	if( !chars[c].npc )
		return P_C_IS_PLAYER;

	// Change this to a flag in NPC.scp

	short int retval = P_C_IS_NPC;
	switch( ( chars[c].id1 << 8 ) + chars[c].id2 )
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
bool cMovement::CheckForCharacterAtXYZ(CHARACTER c, short int cx, short int cy, signed char cz)
{
	const int StartGrid = mapRegions->StartGrid( cx, cy );
	
	int checkgrid = 0;
	for( int increment = 0; increment < 3; increment++ )
	{
		checkgrid = StartGrid + ( increment * mapRegions->GetColSize() );
		for( int a = 0; a < 3; a++ )
		{
			int mapitemptr = -1;
			int mapitem = -1;
			int mapchar = -1;
			do //check all items in this cell
			{
				mapchar = mapitem = -1;
				mapitemptr = mapRegions->GetNextItem( checkgrid + a, mapitemptr );
				if( mapitemptr == -1 ) 
					break;
				mapitem = mapRegions->GetItem( checkgrid + a, mapitemptr );
				if( mapitem > 999999 )
					mapchar = mapitem - 1000000;

				if( mapitem != -1 && mapitem >= 1000000 )
				{
					int i = mapchar;
					if( i != c && ( online( i ) || chars[i].npc ) )
					{	// x=x,y=y, and distance btw z's <= MAX STEP
						//if( ( chars[i].x == cx ) && ( chars[i].y == cy ) && ( abs( chars[i].z - cz ) <= /*P_M_MAX_Z_CLIMB*/ MAX_ITEM_CLIMB_HEIGHT ) )
						if( chars[i].x == cx && chars[i].y == cy && chars[i].z >= cz && chars[i].z <= (cz + 5) )	// 2 people will still bump into each other, if slightly offset
						{
							return true;
						}
					}
				}
			} while( mapitem != -1 );
		}
	}
	return false;
}

// check if GM Body

bool cMovement::CanGMWalk( unitile_st xyb )
{
	unsigned short int blockid = xyb.id;

	tile_st newTile;
	Map->SeekTile( blockid, &newTile );

	if( Map->IsRoofOrFloorTile( &newTile ) )
		return true;
	
	if( xyb.type == 0 )		// map tile
		return true;

	if( xyb.flag1 & 0x40 )	// blocking
		return true;

	if( xyb.flag2 & 0x16 )	// climbable, standable, windows/doors
		return true;

	if( xyb.flag4 & 0x20 )	// door
		return true;

	return false;
}

bool cMovement::CanNPCWalk( unitile_st xyb )
{
	unsigned short int blockid = xyb.id;

	tile_st newTile;
	Map->SeekTile( blockid, &newTile );

	if( Map->IsRoofOrFloorTile( &newTile ) )
		return true;
	
	if( xyb.type == 0 )
		return true;

	if( xyb.flag2 & 0x06 )
		return true;

	return false;
}

bool cMovement::CanPlayerWalk( unitile_st xyb )
{
	unsigned short int blockid = xyb.id;

	tile_st newTile;
	Map->SeekTile( blockid, &newTile );

	if( Map->IsRoofOrFloorTile( &newTile ) )
		return true;
	
	if( xyb.type == 0 )			// map tile
		return true;

	if( xyb.flag2 & 0x06 )		// standable, climbable
		return true;

	return false;
}


bool cMovement::CanFishWalk( unitile_st xyb )
{
	unsigned short int blockid = xyb.id;
	
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
//	if ( blockid > 0x348F && blockid < 0x3494 )
//		return true;
//	if ( blockid > 0x34B4 && blockid < 0x34B8 )
//		return true;

	// Can they walk/swim on/up waterfalls?
	if( blockid > 0x34EC && blockid < 0x3529 )
		return true;

	// Can they walk/swim on the coastlines?
//	if ( blockid > 0x179C && blockid < 0x17B3 )
//		return true;
//	if ( blockid == 0x1796 )
//		return true;

	return false;
}

// needs testing... not totally accurate, but something to hold place.

bool cMovement::CanBirdWalk(unitile_st xyb)
{
	return ( CanNPCWalk(xyb) || CanFishWalk(xyb) );
}

// if we have a valid socket, see if we need to deny the movement request because of
// something to do with the walk sequence being out of sync.
bool cMovement::VerifySequence(CHARACTER c, UOXSOCKET socket, int sequence)
{
    if (socket != -1)
    {
        if ((walksequence[socket] + 1 != sequence) && (sequence != 256))
        {
            deny(socket, c, sequence);  
            return false;
        }
        
    }
    return true;
}

bool cMovement::CheckForRunning(CHARACTER c, UOXSOCKET socket, int dir)
// New need for return
// returns true if updatechar required, or false if not
// PARAM WARNING: unreferenced paramater socket
{
	// if we are running
	if (dir&0x80)
	{ //AntiChrist -- if running
		// if we are using stealth
		if (chars[c].stealth!=-1) 
		{ //AntiChrist - Stealth - stop hiding if player runs
			chars[c].stealth=-1;
			chars[c].hidden=0;
			updatechar( c );
		}
		//Don't regenerate stamina while running
		chars[c].regen2=uiCurrentTime+(server_data.staminarate*CLOCKS_PER_SEC);
		chars[c].running++;
		// if all these things
		if( !chars[c].dead )
		{
			bool reduceStamina = ( chars[c].onhorse && chars[c].running > ( server_data.runningstaminasteps * 2 ) );
			if( !reduceStamina )
				reduceStamina = ( chars[c].running > ( server_data.runningstaminasteps * 4 ) );
			if( reduceStamina )
			{
				chars[c].running = 0;
				chars[c].stm--;
				updatestats( c, 2 );
			}
		}
		if( chars[c].war && chars[c].targ != -1 )
		{
			chars[c].timeout = uiCurrentTime + CLOCKS_PER_SEC * 2;
		}

	}
	else 
	{
		chars[c].running=0;
	}                                           
	return true;
}

bool cMovement::CheckForStealth(CHARACTER c, UOXSOCKET socket)
// PARAM WARNING: unreferenced paramater socket
{
	if ((chars[c].hidden)&&(!(chars[c].priv2&8)))
	{
		if(chars[c].stealth!=-1)
		{ //AntiChrist - Stealth
			chars[c].stealth++;
			if(chars[c].stealth>((server_data.maxstealthsteps*chars[c].skill[STEALTH])/1000))
			{
				chars[c].stealth=-1;
				chars[c].hidden=0;
				updatechar( c );
			}
		}
		else
		{
			chars[c].hidden=0;
			updatechar( c );
		}
	}
	return true;
}

// see if a player has tried to move into a house they were banned from
bool cMovement::CheckForHouseBan(CHARACTER c, UOXSOCKET socket)
{
    if( !chars[c].npc ) // this code is also called from npcs-walking code, so only check for players to cut down lag!
    {
        // check if player is banned from a house - crackerjack 8/12/99
        int i = findmulti(chars[c].x, chars[c].y, chars[c].z);
        if( i != -1 ) 
        {
			setserial(c, i, 8);//Set them inside the multi!

            int j=on_hlist(i, chars[c].ser1, chars[c].ser2, chars[c].ser3, chars[c].ser4, NULL);
            if( j == H_BAN ) 
            {
                int sx, sy, ex, ey;
                Map->MultiArea(i,&sx,&sy,&ex,&ey);
                chars[c].x = ex;
                chars[c].y = ey+1;
                teleport(c);
                if (socket!=-1)
				{
					sysmessage(socket, "You are banned from that location.");
					// this wasn't resetting walksequence like the others, probably a bug
					walksequence[socket] = -1;
				}
                return false;
            }
        }
		else
		{	// multi serial is NEVER CLEARED
			if( chars[c].multis != -1 )
				unsetserial( c, 8 );
		}
    } 
    return true;
}

// Thyme 2000.09.21
// I already made sure I could move there (even the crazy XY block stuff) so this IS a valid move. Just move the
// directions. Oh, and since I we already have the GetX/YfromDir functions (and we need those) why don't we just
// use them here?

void cMovement::MoveCharForDirection(CHARACTER c, int dir)
{
	chars[c].x = GetXfromDir( dir, chars[c].x );
	chars[c].y = GetYfromDir( dir, chars[c].y );
}

// Split up of FillXYBlockStuff


void cMovement::GetBlockingMap( SI16 x, SI16 y, unitile_st *xyblock, int &xycount, unsigned short oldx, unsigned short oldy )
{
	// This oldx, oldy stuff is... problematic, to say the least
	int mapid = 0;
	int mapid_old = 0;
	signed int mapz = Map->AverageMapElevation( x, y, mapid );
	signed int mapz_old = Map->AverageMapElevation( oldx, oldy, mapid_old );
	if( mapz_old <= -128 )	{
		mapz_old = mapz;
	}
	if( mapz != illegal_z )
	{
		land_st land;
		Map->SeekLand( mapid, &land );
		
		xyblock[xycount].type = 0;
		xyblock[xycount].basez = min( mapz_old, mapz );
		xyblock[xycount].id = mapid;
		xyblock[xycount].flag1 = land.flag1;
		xyblock[xycount].flag2 = land.flag2;
		xyblock[xycount].flag3 = land.flag3;
		xyblock[xycount].flag4 = land.flag4;
		xyblock[xycount].height = mapz - xyblock[xycount].basez;
		xyblock[xycount].weight = 255;
		xycount++;
	}
}


void cMovement::GetBlockingStatics( SI16 x, SI16 y, unitile_st *xyblock, int &xycount )
{
	MapStaticIterator msi( x, y );
	staticrecord *stat;
 	while( stat = msi.Next() )
	{
		//printf("staticr[X] type=%d, id=%d\n", 2, stat->itemid);
		tile_st tile;
		msi.GetTile(&tile);
		xyblock[xycount].type = 2;
		xyblock[xycount].basez = stat->zoff;
		xyblock[xycount].id = stat->itemid;
		xyblock[xycount].flag1 = tile.flag1;
		xyblock[xycount].flag2 = tile.flag2;
		xyblock[xycount].flag3 = tile.flag3;
		xyblock[xycount].flag4 = tile.flag4;
		xyblock[xycount].height = tile.height;
		xyblock[xycount].weight = 255;
		xycount++;
	}
}

void cMovement::GetBlockingDynamics( SI16 x, SI16 y, unitile_st *xyblock, int &xycount )
{
	// - Tauriel's region stuff 3/6/99
	const int StartGrid = mapRegions->StartGrid( x, y );
	
	// - This needs to use a pointer/array to cut down on walking lag...
	unsigned int checkgrid = 0;
	for( int increment = 0; increment < 3; increment++ )
	{
		checkgrid = StartGrid + (increment * mapRegions->GetColSize() );
		for( int i = 0; i < 3; i++ )
		{
			int mapitem = -1;
			int mapitemptr = -1;
			do //check all items in this cell
			{
				mapitemptr = mapRegions->GetNextItem( checkgrid + i, mapitemptr );
				if( mapitemptr == -1 ) 
					break;
				mapitem = mapRegions->GetItem( checkgrid + i, mapitemptr );
				if( mapitem != -1 && mapitem < 1000000 )
				{
					if( items[mapitem].id1 < 0x40 )
					{
#if DEBUG_WALKING
						printf( "Item X: %i\nItem Y: %i\n", items[mapitem].x, items[mapitem].y );
#endif
						if( ( items[mapitem].x == x ) && ( items[mapitem].y == y ) )
						{
							tile_st tile;
							Map->SeekTile( ( items[mapitem].id1<<8 ) + items[mapitem].id2, &tile );
							xyblock[xycount].type = 1;
							xyblock[xycount].basez = items[mapitem].z;
							xyblock[xycount].id = ( items[mapitem].id1<<8 ) + items[mapitem].id2;
							// Potential thought.  NPCs don't automatically open doors anymore :(  Perhaps store the Index of the item in flag1->4?  Set up as a union, of course
							xyblock[xycount].flag1 = tile.flag1;
							xyblock[xycount].flag2 = tile.flag2;
							xyblock[xycount].flag3 = tile.flag3;
							xyblock[xycount].flag4 = tile.flag4;
							xyblock[xycount].height = tile.height;
							xyblock[xycount].weight = tile.weight;
							xycount++;
						}
					}
					else if (
							( abs( items[mapitem].x - x) <= BUILDRANGE ) &&
							( abs( items[mapitem].y - y) <= BUILDRANGE )
							)
					{
						UOXFile *mfile = NULL;
						SI32 length = 0;		// should be SI32, not long
						Map->SeekMulti( ( ( items[mapitem].id1<<8 ) + items[mapitem].id2 ) - 0x4000, &mfile, &length );
						length = length / MultiRecordSize;
						if( length == -1 || length >= 17000000 )//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
						{
							printf( "walking() - Bad length in multi file. Avoiding stall (Item Serial: %i)\n", items[mapitem].serial );
							length = 0;
						}
						for( int j = 0; j < length; j++ )
						{
							st_multi multi;
							mfile->get_st_multi( &multi );
							if( multi.visible && ( items[mapitem].x + multi.x == x ) && ( items[mapitem].y + multi.y == y ) )
							{
								tile_st tile;
								Map->SeekTile( multi.tile, &tile );
								xyblock[xycount].type = 2;
								xyblock[xycount].basez = multi.z + items[mapitem].z;
								xyblock[xycount].id = multi.tile;
								xyblock[xycount].flag1 = tile.flag1;
								xyblock[xycount].flag2 = tile.flag2;
								xyblock[xycount].flag3 = tile.flag3;
								xyblock[xycount].flag4 = tile.flag4;
								xyblock[xycount].height = tile.height;
								xyblock[xycount].weight = 255;
								xycount++;
							}
						}
					}
				}
			} while( mapitem != -1 );
		} //- end of itemcount for loop
	}
}

// so we are going to move, lets update the regions
// FYI, Items equal to or greater than 1000000 are considered characters...
void cMovement::HandleRegionStuffAfterMove(CHARACTER c, short int oldx, short int oldy)
{
	// save where we were moving to
	const short int nowx = chars[c].x;
	const short int nowy = chars[c].y;

	// i'm trying a new optimization here, if we end up in the same map cell
	// as we started, i'm sure there's no real reason to remove and readd back
	// to the same spot..
	if (mapRegions->GetCell(oldx, oldy) != mapRegions->GetCell(nowx, nowy))
	{
		// restore our original location and remove ourself
		chars[c].x = oldx;
		chars[c].y = oldy;
		mapRegions->RemoveItem(c+1000000);
		// we have to remove it with OLD x,y ... LB, very important, and he is right!

		// restore the new location and add ourselves
		chars[c].x = nowx;
		chars[c].y = nowy;
	
		mapRegions->AddItem(c+1000000);

		// i suspect the new weather code probably needs something here, so if
		// you walk from one region to another it can adjust the weather, but i
		// could be wrong
	}
#if DEBUG_WALKING
	else
	{
		//printf("Guess what? I didn't change regions.\n");
	}
#endif

	// i'm moving this to the end because the regions shouldn't care what the z was
	chars[c].dispz = dispz;
	chars[c].z = z;
}


// actually send the walk command back to the player and increment the sequence
void cMovement::SendWalkToPlayer(CHARACTER c, UOXSOCKET socket, short int sequence)
{
	if (socket!=-1)
	{
		char walkok[4]="\x22\x00\x01";
		walkok[1]=buffer[socket][2];
		walkok[2]=0x41;
		if (chars[c].hidden)
			walkok[2]=0x00;
		Network->xSend(socket, walkok, 3, 0);

		walksequence[socket] = sequence;
		if (walksequence[socket] == 255)
			walksequence[socket] = 0;
	}
}

// send out our movement to all other players who can see us move
void cMovement::SendWalkToOtherPlayers(CHARACTER c, int dir, short int oldx, short int oldy)
{
	// lets cache these vars in advance
	const int visibleRange = Races->getVisRange( chars[c].race );
	const int newx = chars[c].x;
	const int newy = chars[c].y;

	UOXSOCKET i;
	for( i = 0; i < now; i++ )
	{	// lets see, its much cheaper to call perm[i] first so i'm reordering this
		if ((perm[i]) && (inrange1p(c, currchar[i])))
		{
			if (
				(((abs(newx-chars[currchar[i]].x)==visibleRange )||(abs(newy-chars[currchar[i]].y)== visibleRange )) &&
				((abs(oldx-chars[currchar[i]].x)>visibleRange )||(abs(oldy-chars[currchar[i]].y)>visibleRange ))) ||
				((abs(newx-chars[currchar[i]].x)==visibleRange )&&(abs(newy-chars[currchar[i]].y)==visibleRange ))
				)
			{
				impowncreate(i, c, 1);
			}
			else
			{
				extmove[1] = chars[c].ser1;
				extmove[2] = chars[c].ser2;
				extmove[3] = chars[c].ser3;
				extmove[4] = chars[c].ser4;
				extmove[5] = chars[c].id1;
				extmove[6] = chars[c].id2;
				extmove[7] = chars[c].x>>8;
				extmove[8] = chars[c].x%256;
				extmove[9] = chars[c].y>>8;
				extmove[10]=chars[c].y%256;
				extmove[11]=chars[c].dispz;
				extmove[12]=dir;
				extmove[13]=chars[c].skin1; //ripper, skin problems bugfix
				extmove[14]=chars[c].skin2;
				if( chars[c].npc && chars[c].runs && chars[c].war ) // Ripper 10-2-99 makes npcs run in war mode or follow :) (Ab mod, scriptable)
					extmove[12]=dir|0x80;
				if( chars[c].war ) 
					extmove[15] = 0x40; 
				else 
					extmove[15] = 0x00;
				if( chars[c].hidden ) 
					extmove[15] |= 0x80;
				if( chars[c].dead && !chars[c].war ) 
					extmove[15] |= 0x80; // Ripper
				if( chars[c].poisoned ) 
					extmove[15] |= 0x04; //AntiChrist -- thnx to SpaceDog
				int guild, race;
				guild = Guilds->Compare( c, currchar[i] );
				race = Races->Compare( c, currchar[i] );
				if( chars[c].kills > repsys.maxkills ) 
					extmove[16] = 6;
				else if (guild==1 || race==2)//Same guild (Green)
					extmove[16]=2;
				else if (guild==2 || race==1) // Enemy guild.. set to orange
					extmove[16]=5;
				else
				{
					if( chars[c].flag & 0x01 )
						extmove[16] = 6;		// If a bad, show as red.
					else if( chars[c].flag & 0x04 )
						extmove[16] = 1;		// If a good, show as blue.
					else if( chars[c].flag & 0x08 )
						extmove[16] = 2;		// green (guilds)
					else if( chars[c].flag & 0x10 )
						extmove[16] = 5;		// orange (guilds)
					else 
						extmove[16] = 3;		// grey
				}
				if( currchar[i] != c ) // fix from homey (NPCs will display right)
					Network->xSend(i, extmove, 17, 0);
			}
		}
	}
}

// see if we should mention that we shove something out of the way
void cMovement::OutputShoveMessage(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy)
{
	if (socket!=-1)
	{
		// lets cache these vars in advance
		const int StartGrid=mapRegions->StartGrid(chars[c].x,chars[c].y);
		
		int checkgrid = 0;
		for (int increment = 0; increment < 3; increment++)
		{
			checkgrid = StartGrid+(increment*mapRegions->GetColSize());
			for (int a = 0; a < 3; a++)
			{
				int mapitemptr = -1;
				int mapitem = -1;
				int mapchar = -1;
				do //check all items in this cell
				{
					mapchar = mapitem = -1;
					mapitemptr = mapRegions->GetNextItem( checkgrid + a, mapitemptr );
					if( mapitemptr == -1 ) 
						break;
					mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
					if (mapitem>999999)
						mapchar = mapitem-1000000;

					if (mapitem!=-1 && mapitem>=1000000)
					{
						int i=mapchar;
#if DEBUG
						printf("DEBUG: Mapchar %i [%i]\n",mapchar,mapitem);
#endif
						if (!(
							((chars[c].id1==0x03)&&(chars[c].id2==0xDB)) ||
							((chars[c].id1==0x01)&&(chars[c].id2==0x92)) ||
							((chars[c].id1==0x01)&&(chars[c].id2==0x93)) ||
							(chars[c].priv&0x01) ||
							(chars[c].priv&0x80)
							))
						{
							if (i!=c && (online(i) || chars[i].npc))
							{
								if( chars[i].x == chars[c].x && chars[i].y == chars[c].y && chars[i].z == chars[c].z )
								{
									if( !chars[i].hidden && !( chars[i].priv&0x10 ) )
									{
										sprintf(temp, "Being perfectly rested, you shove %s out of the way.", chars[i].name);
										if( socket != -1 ) 
											sysmessage( socket, temp );
										chars[c].stm=max(chars[c].stm-4, 0);
										updatestats(c, 2);  // arm code
									}
									else if(!(chars[i].priv&1)&&!(chars[i].priv&80))//A normal player (No priv1(Not a gm))
									{
										if( socket != -1 ) 
											sysmessage( socket, "Being perfectly rested, you shove something invisible out of the way." );
										chars[c].stm=max(chars[c].stm-4, 0);
										updatestats(c, 2);  // arm code
									}
								}
							}
						}
					}
				} while (mapitem!=-1);
			}
		}
	}
}

// handle item collisions, make items that appear on the edge of our sight because
// visible, buildings when they get in range, and if the character steps on something
// that might cause damage
void cMovement::HandleItemCollision( CHARACTER c, UOXSOCKET socket, bool amTurning, unsigned short oldx, unsigned short oldy )
{
	// apparently we don't want NPCs to be affected by any of this stuff,
	// i'm not sure i agree with that yet
	// It's not the fact that we don't want them to be affected by it, it's just the fact that this also updates
	// the display of the char... So what we need to do is handle the collision stuff for everyone, and only send
	// the stuff to actual characters.
//	if (socket == -1)
//		return;

	// lets cache these vars in advance
	const int visibleRange = Races->getVisRange( chars[c].race );
	const short int newx = chars[c].x;
	const short int newy = chars[c].y;
	unsigned char id1, id2;
	bool EffRange;
	short int oldd, newd;
	
	// - Tauriel's region stuff 3/6/99
	const int StartGrid = mapRegions->StartGrid(newx, newy);

	int checkgrid = 0;
	for (int increment = 0; increment < 3; increment++)
	{
		checkgrid = StartGrid + (increment * mapRegions->GetColSize());
		for (int i=0;i<3;i++)
		{
			int mapitemptr=-1;
			int mapitem=-1;
			int mapchar = -1;
			do //check all items in this cell
			{
				mapitemptr = mapRegions->GetNextItem( checkgrid + i, mapitemptr );
				if( mapitemptr == -1 ) 
					break;
				mapitem = mapRegions->GetItem( checkgrid + i, mapitemptr );
				// Character Send Stuff
				if( mapitem != -1 && mapitem >= 1000000 )
				{
					mapchar = mapitem - 1000000;
					oldd = Distance( oldx, oldy, chars[mapchar].x, chars[mapchar].y );
					newd = Distance( newx, newy, chars[mapchar].x, chars[mapchar].y );
					if( online( mapchar ) || chars[mapchar].npc || ( ( chars[c].priv&0x01) && server_data.showloggedoutpcs ) )
					{
						if( ( oldd > visibleRange ) && ( newd == visibleRange ) )
							impowncreate( socket, mapchar, 1 );
					}
				}
				// Item Send Stuff
				else if( mapitem != -1 && mapitem < 1000000 )
				{
					// Why recalculate all this stuff several times?
					id1 = items[mapitem].id1; id2 = items[mapitem].id2;
					EffRange = ( ( items[mapitem].x == newx ) && 
								 ( items[mapitem].y == newy ) && 
								 ( chars[c].z >= items[mapitem].z ) && 
								 ( chars[c].z <= ( items[mapitem].z + 5 ) ) );
					oldd = Distance( oldx, oldy, items[mapitem].x, items[mapitem].y );
					newd = Distance( newx, newy, items[mapitem].x, items[mapitem].y );
					if( id1 == 0x39 && ( id2 == 0x96 || id2 == 0x8C ) )
					{//Fire Field
						if( EffRange )
						{
							if (!Magic->CheckResist(-1, c, 4))
							{                                               
								Magic->MagicDamage(c, items[mapitem].morex/300);
							}
							soundeffect2(c, 2, 8);
						}
					}
					else if( id1 == 0x39 && ( id2 == 0x15 || id2 == 0x20 ) )
					{//Poison field
						if( EffRange )
						{
							if (!Magic->CheckResist(-1, c, 5))
							{                                               
								Magic->PoisonDamage(c,1);
							}
							soundeffect2(c, 2, 8);
						}
					}
					else if( id1 == 0x39 && ( id2 == 0x79 || id2 == 0x67 ) )
					{//Para Field
						if( EffRange )
						{
							if (!Magic->CheckResist(-1, c, 6))
							{
								tempeffect(c, c, 1, 0, 0, 0);
							}
							soundeffect2(c, 0x02, 0x04);
						}
					}
					else if( id1 < 0x40 )
					{
						// look for item triggers, this was moved from CrazyXYBlockStuff()
						if( !newd )
						{
							if (items[mapitem].trigger!=0)
							{
								if ((items[mapitem].trigtype==1)&&(!chars[c].dead))
								{
									if (!items[mapitem].disabled)
									{
										triggerwitem(socket,mapitem,1);  //When player steps on a trigger
									}
									else // see if disabled trigger can be re-enabled
									{
										if( ( items[mapitem].disabled != 0 ) && ( ( items[mapitem].disabled <= uiCurrentTime ) || (overflow)))
										{
											items[mapitem].disabled = 0;	// re-enable it
											triggerwitem( socket, mapitem, 1 );
										}
									}
								}
							}
						}
					}

					// always check for new items if we actually moved
					if( !amTurning )
					{
						// is the item a building on the BUILDRANGE?
						if( id1 == 0x40 && id2 >= 0x7C && id2 <= 0x7E )
						{
							if( newd == BUILDRANGE )
								senditem(socket, mapitem);
						}
						// otherwise if the item has just now become visible, inform the client about it
						else
						{
							if( ( oldd > visibleRange ) && ( newd == visibleRange ) )
							{
								if( ( !items[mapitem].visible ) || ( ( items[mapitem].visible ) && ( chars[currchar[socket]].priv&0x01 ) ) )// we're a GM, or not hidden
									senditem(socket, mapitem);
							}
							else if( ( oldd == visibleRange ) && ( newd == ( visibleRange + 1 ) ) )
							{
								// item out of range
							}
						}
					}
				}
			} while (mapitem!=-1);
		}
	}
}

void cMovement::HandleTeleporters(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy)
// PARAM WARNING: unreferenced paramater socket
{
	// now this is one wacky optimization. if we haven't moved don't do this
	// well, we wouldn't be in Walking() if we weren't trying to move!
	if ((chars[c].x!=oldx)||(chars[c].y!=oldy))
	{
		if( !chars[c].npc )
			objTeleporters( c );   // ripper
		teleporters(c);
	}
}


/********* start of LB's no rain & snow in buildings stuff ***********/
void cMovement::HandleWeatherChanges(CHARACTER c, UOXSOCKET socket)
{
	if (!chars[c].npc && online(c)) // check for being in buildings (for weather) only for PC's
	{
		// ok, this is already a bug, because the new weather stuff doesn't use this global
		// for the weather.
		if (wtype!=0) // check only neccasairy if it rains or snows ...
		{
			int inDungeon = indungeon(c); // dung-check
			bool i = Map->IsUnderRoof(chars[c].x,chars[c].y,chars[c].z); // static check
			// dynamics-check
			int x = Map->DynamicElevation(chars[c].x,chars[c].y,chars[c].z);
			if (x!=illegal_z)
				if (Boats->GetBoat(socket)!=-1)
					x=illegal_z; // check for dynamic buildings except boats
			if (x==1)
				x=  illegal_z; // 1 seems to be the multi-borders
			
			//printf("x: %i\n",x);
			// ah hah! this was a bug waiting to happen if not already, we have overloaded the use of the
			// variable k, which used to hold the socket
			int k = noweather[c];    
			if (inDungeon || i || x!= illegal_z )
				noweather[c] = 1;
			else
				noweather[c] = 0; // no rain & snow in static buildings+dungeons;
			if (k - noweather[c] !=0)
				weather(socket, 0); // iff outside-inside changes resend weather ...
		}
	}
}

void cMovement::HandleGlowItems(CHARACTER c, UOXSOCKET socket)
// PARAM WARNING: unreferenced paramater socket
{
	// i guess things only glow if you are online, i dunno what that means        
	if( online( c ))
	{
		const int serial = chars[c].serial;
		const int serhash = serial%HASHMAX;
		for( int ci = 0; ci < glowsp[serhash].max; ci++ )
		{
			const int i = glowsp[serhash].pointer[ci];
			if( i != -1 )
			{
				if( items[i].free == 0 )
				{
					Items->GlowItem( c, i );
				}
			}
		}
	}
}

// return whether someone is a GM Body
bool cMovement::IsGMBody(CHARACTER c)
{
	if (
		(( chars[c].priv & P_C_PRIV_GM ) ) || // I got GM privs
        ((chars[c].id1==0x03)&&(chars[c].id2==0xDB)) ||//Gm
        ((chars[c].id1==0x01)&&(chars[c].id2==0x92)) ||//Ghosts
        ((chars[c].id1==0x01)&&(chars[c].id2==0x93))
        ) 
        return true;
    return false;
}

void cMovement::CombatWalk(int s) // Only for switching to combat mode
{
    for (unsigned int i=0;i<now;i++)
    {
		// moved perm[i] first since its much faster
        if ((perm[i]) && (inrange1p(s, currchar[i])))
        {
            extmove[1] = chars[s].ser1;
            extmove[2] = chars[s].ser2;
            extmove[3] = chars[s].ser3;
            extmove[4] = chars[s].ser4;
            extmove[5] = chars[s].id1;
            extmove[6] = chars[s].id2;
            extmove[7] = (unsigned char )(chars[s].x>>8);
            extmove[8] = (unsigned char )(chars[s].x%256);
            extmove[9] = (unsigned char )(chars[s].y>>8);
            extmove[10] = (unsigned char )(chars[s].y%256);
            extmove[11] = chars[s].dispz;
            extmove[12] = (unsigned char )(chars[s].dir&0x7F);
            
            extmove[13]=chars[s].skin1; //ripper, skin problems bugfix
            extmove[14]=chars[s].skin2;
            
            
            if( chars[s].war ) 
				extmove[15] = 0x40; 
			else 
				extmove[15] = 0x00;
            if( chars[s].hidden ) 
				extmove[15] |= 0x80;
            if( chars[s].poisoned ) 
				extmove[15] |= 0x04; //AntiChrist -- thnx to SpaceDog
            const int guild = Guilds->Compare( s, currchar[i] );
            const int race=Races->Compare( s, currchar[i] );
            if (chars[s].kills > repsys.maxkills ) extmove[16]=6; // ripper
            else if (guild==1  || race==2)//Same guild (Green)
                extmove[16]=2;
            else if (guild==2 || race==1) // Enemy guild.. set to orange
                extmove[16]=5;
            else 
			{
				if( chars[s].flag & 0x01 )
					extmove[16] = 6;
				else if( chars[s].flag & 0x04 )
					extmove[16] = 1;
				else if( chars[s].flag & 0x08 )
					extmove[16] = 2;
				else if( chars[s].flag & 0x10 )
					extmove[16] = 5;
				else 
					extmove[16] = 3;
            }
            
            if (!chars[s].war)
            {
                //                              chars[s].attacker=-1;
                chars[s].targ=-1;
            }
            Network->xSend(i, extmove, 17, 0);
        }
    }
}


void cMovement::NpcWalk(CHARACTER i, int j, int type)   //type is npcwalk mode (0 for normal, 1 for box, 2 for circle)
{

    // if we are walking in an area, and the area is not properly defined, just don't bother with the area anymore
    if( ((1 == type) && ( chars[i].fx1 == -1 || chars[i].fx2 == -1 || chars[i].fy1 == -1 || chars[i].fy2 == -1 ) ) ||
        ((2 == type) && ( chars[i].fx1 == -1 || chars[i].fx2 == -1 || chars[i].fy1 == -1)))
        // circle's don't use fy2, so don't require them! fur 10/30/1999
    {
        chars[i].npcWander = 2; // Wander freely from now on
        type = 0;
    }
	// Thyme New Stuff 2000.09.21
	short int newx = GetXfromDir( j, chars[i].x );
	short int newy = GetYfromDir( j, chars[i].y );
	// Let's make this a little more readable.
	switch( type )
	{
	case 0:
		Walking( i, (j & 0x87), 256 );
		break;
	case 1:
		if( checkBoundingBox( newx, newy, chars[i].fx1, chars[i].fy1, chars[i].fz1, chars[i].fx2, chars[i].fy2 ) )
			Walking( i, (j & 0x87), 256 );
		break;
	case 2:
		if( checkBoundingCircle( newx, newy, chars[i].fx1, chars[i].fy1, chars[i].fz1, chars[i].fx2 ) )
			Walking( i, (j & 0x87), 256 );
		break;
	default:
		printf( "ERROR: Unknown NpcWalk %i\n", type );
		break;
	};
}


// Function      : cMovement::GetYfromDir
// Written by    : Unknown
// Revised by    : Thyme
// Revision Date : 2000.09.15
// Purpose       : Return the new y from given dir

unsigned short cMovement::GetYfromDir(int dir, unsigned short y)
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

unsigned short cMovement::GetXfromDir(int dir, unsigned short x)
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
void cMovement::PathFind(CHARACTER c, unsigned short gx, unsigned short gy)
{

	// Make sure this is a valid character before proceeding
	if ( c < 0 || c >= cmem ) 
		return;

	// Make sure the character has taken used all of their previously saved steps
	if ( chars[c].pathnum < P_PF_MRV ) 
		return;
    

	// Thyme 2000.09.21
	// initial rewrite of pathfinding...

	signed int newx, newy;
	signed char newz;
	signed int oldx = chars[c].x;
	signed int oldy = chars[c].y;
	path_st newpath[P_PF_MIR];
	chars[c].pathnum=0;

	for( int pn = 0; pn < P_PF_MRV; pn++ )
	{
		newpath[pn].x = newpath[pn].y = 0;
		int pf_neg = ( ( rand() % 2 ) ? 1 : -1 );
		int pf_dir = Direction( oldx, oldy, gx, gy );
		for( int i = 0 ; i < 8 ; i++ )
		{
			pf_neg *= -1;
			pf_dir += ( i * pf_neg );
			newx = GetXfromDir( pf_dir, oldx );
			newy = GetYfromDir( pf_dir, oldy );
			if( calc_move( c, oldx, oldy, newz, pf_dir ) )
			{
				if( ( pn < P_PF_MRV ) && CheckForCharacterAtXYZ( c, newx, newy, newz ) )
					continue;
				newpath[pn].x = oldx = newx;
				newpath[pn].y = oldy = newy;
				break;
			}
		}
		if( newpath[pn].x == 0 && newpath[pn].y == 0 )
		{
			chars[c].pathnum = P_PF_MRV;
			break;
#if DEBUG_PATHFIND
			printf("Character stuck!\n");
#endif
		}
	}

	for( int i = 0 ; i < P_PF_MRV ; i++ )
	{
		chars[c].path[i].x = newpath[i].x;
		chars[c].path[i].y = newpath[i].y;
#if DEBUG_PATHFIND
		printf("PFDump: %s - %i) %ix, %iy\n",chars[c].name, i+1, chars[c].path[i].x, chars[c].path[i].y);
#endif
	}

}


//NEW NPCMOVEMENT ZIPPY CODE STARTS HERE -- AntiChrist meging codes --
void cMovement::NpcMovement(unsigned int currenttime, int i)//Lag fix
{
    register int k;
    int l;
	int j = rand() % 40;
    int dnpctime = 0;
    if( chars[i].npc && ( chars[i].npcmovetime <= currenttime || ( overflow ) ) )
    {
#if DEBUG_NPCWALK
		printf( "ENTER (%s): %d AI %d WAR %d J\n", chars[i].name, chars[i].npcWander, chars[i].war, j );
#endif
		if( chars[i].war && chars[i].npcWander != 5 )
        {
            l = chars[i].attacker;
            if( l != -1 )
            {
                if( chardist(i, l) > 1 /* || chardir(i, l)!=chars[i].dir // by Thyme: causes problems, will fix */)
                {
					if( Skills->GetCombatSkill( i ) == ARCHERY )
					{
						UOXSOCKET s1 = calcSocketFromChar( i ), s2 = calcSocketFromChar( l );
						unsigned short los = line_of_sight( s1, chars[i].x, chars[i].y, chars[i].z, chars[l].x, chars[l].y, chars[l].z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING );
						if( los == 1 )
						{
							chars[i].dir = chardirxyz( i, chars[l].x, chars[l].y, chars[l].z );
							updatechar( i );
							return;
						}
					}
                    if( online( l ) || chars[l].npc )
                    {
						PathFind( i, chars[l].x, chars[l].y );
                        j = chardirxyz( i, chars[i].path[chars[i].pathnum].x, chars[i].path[chars[i].pathnum].y, chars[i].z );
                        if( ( chars[i].dir & 0x07 ) == ( j & 0x07 ) ) 
							chars[i].pathnum++;
                        Walking( i, j, 256 );
                    }
                }
				else
				{ // if I'm within distance, clear my path... for attacking only.
					chars[i].pathnum += P_PF_MRV;
				}
	        }
        } // end of if l!=-1
        else
        {
            switch( chars[i].npcWander )
            {
            case 0: // No movement
                break;
            case 1: // Follow the follow target
                k = (chars[i].ftarg);
                if( k < 0 || k >= (int)cmem ) 
					return;
                if( online(k) || chars[k].npc )
                {
                    if( chardist(i, k) > 1 /* || chardir(i, k)!=chars[i].dir // by THyme: causes problems, will fix */)
                    {
                        PathFind( i, chars[k].x, chars[k].y );
                        j = chardirxyz( i, chars[i].path[chars[i].pathnum].x, chars[i].path[chars[i].pathnum].y, chars[i].z );
                        chars[i].pathnum++;
                        Walking( i, j, 256 );
                    }
					// Dupois - Added April 4, 1999
					// Has the Escortee reached the destination ??
					// no need for -1 check on k, as we wouldn't be here if that were true
					if( ( !chars[k].dead ) && ( chars[i].questDestRegion == chars[i].region ) )
					{	// Pay the Escortee and free the NPC
						MsgBoardQuestEscortArrive( i, calcSocketFromChar( k ) );
					}
					// End - Dupois
                }
                break;
            case 2: // Wander freely, avoiding obstacles.
                if( j < 8 || j > 32 ) 
					dnpctime = 5;
                if( j > 7 && j< 33 ) // Let's move in the same direction lots of the time.  Looks nicer.
                    j = chars[i].dir;
                NpcWalk( i, j, 0 );
                break;
            case 3: // Wander freely, within a defined box
                if( j < 8 || j > 32 ) 
					dnpctime = 5;
                if( j > 7 && j < 33 ) // Let's move in the same direction lots of the time.  Looks nicer.
                    j = chars[i].dir;
                NpcWalk( i, j, 1 );
                break;
            case 4: // Wander freely, within a defined circle
                if( j < 8 || j > 32 ) 
					dnpctime = 5;
                if( j > 7 && j < 33 ) // Let's move in the same direction lots of the time.  Looks nicer.
                    j = chars[i].dir;
                NpcWalk( i, j, 2 );
                break;
            case 5: //FLEE!!!!!!
                k = chars[i].targ;
                if( k < 0 || k >= (int)cmem ) 
					return;
				if( chardist(i, k) < P_PF_MFD )
				{	// calculate a x,y to flee towards
					int mydist = P_PF_MFD - chardist( i, k ) + 1;
					j = chardirxyz( i, chars[k].x, chars[k].y, chars[k].z );
					short int myx = GetXfromDir( j, chars[i].x );
					short int myy = GetYfromDir( j, chars[i].y );

					short int xfactor = 0;
					short int yfactor = 0;

					if ( myx != chars[i].x )
						if ( myx < chars[i].x )
							xfactor = -1;
						else
							xfactor = 1;

					if ( myy != chars[i].y )
						if ( myy < chars[i].y )
							yfactor = -1;
						else
							yfactor = 1;

					myx += (short int)( xfactor * mydist );
					myy += (short int)( yfactor * mydist );

					// now, got myx, myy... lets go.

					PathFind( i, myx, myy );
					j = chardirxyz( i, chars[i].path[chars[i].pathnum].x, chars[i].path[chars[i].pathnum].y, chars[i].z );
					chars[i].pathnum++;
					Walking( i, j, 256 );
				}
				else
				{ // wander freely... don't just stop because I'm out of range.
					j = rand()%40;
					if( j < 8 || j > 32 ) 
						dnpctime = 5;
					if( j > 7 && j < 33 ) // Let's move in the same direction lots of the time.  Looks nicer.
        				j = chars[i].dir;
					NpcWalk( i, j, 0 );
				}
                break;
            default:
                break;
            }
        }
		if( dnpctime != 0 )
		{
			chars[i].npcmovetime = (unsigned int)(currenttime + double( NPCSPEED * (float)dnpctime * (float)CLOCKS_PER_SEC ) );
		}
		else
		{
			if( chars[i].tamed )
				chars[i].npcmovetime = (unsigned int)(currenttime+double((NPCSPEED/4)*CLOCKS_PER_SEC)); //reset move timer
			else
				chars[i].npcmovetime = (unsigned int)(currenttime+double(NPCSPEED*CLOCKS_PER_SEC)); //reset move timer
		}
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

short int cMovement::Distance(short int sx, short int sy, short int dx, short int dy)
{
	return ( ( abs(sx-dx) < abs(sy-dy) ) ? abs(sy-dy) : abs(sx-dx) );
}

// Function      : cMovement::CanCharWalk()
// Written by    : Dupois
// Revised by    : Thyme
// Revision Date : 2000.09.17
// Purpose       : Check if a character can walk to a specified x,y location
// Method        : Modified the old CheckWalkable function so that it can be utilized throughout
// the walking code. Ever wonder why NPCs can walk through walls and stuff in combat mode? This
// is the fix, plus more.

short int cMovement::Direction(short int sx, short int sy, short int dx, short int dy)
{
	short int dir, xdif, ydif;
	
	xdif = dx - sx;
	ydif = dy - sy;
	
	if( ( xdif == 0 ) && ( ydif < 0 ) ) 
		dir = 0;
	else if( ( xdif > 0 ) && ( ydif < 0 ) ) 
		dir = 1;
	else if( ( xdif > 0 ) && ( ydif == 0 ) ) 
		dir = 2;
	else if( ( xdif > 0 ) && ( ydif > 0 ) ) 
		dir = 3;
	else if( ( xdif == 0 ) && ( ydif > 0 ) ) 
		dir = 4;
	else if( ( xdif < 0 ) && ( ydif > 0 ) ) 
		dir = 5;
	else if( ( xdif < 0 ) && ( ydif == 0 ) ) 
		dir = 6;
	else if( ( xdif < 0 ) && ( ydif < 0 ) ) 
		dir = 7;
	else 
		dir = -1;
	
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
    invalid_z == -128, if walk is blocked

********************************************************/
int cMovement::calc_walk(CHARACTER c, unsigned int x, unsigned int y, unsigned int oldx, unsigned int oldy, bool justask )
{
	const signed int oldz = chars[c].z;
	bool may_levitate = chars[c].may_levitate;
	bool on_ladder = false;
	bool climbing = false;
	signed int newz = illegal_z;
	short int MoveType = CheckMovementType( c );
	bool blocked = false;
	int ontype = 0;

	int xycount = 0;
	unitile_st xyblock[XYMAX];
	GetBlockingMap( x, y, xyblock, xycount, oldx, oldy );
	GetBlockingStatics( x, y, xyblock, xycount );
	GetBlockingDynamics( x, y, xyblock, xycount );

	int i;
	// first calculate newZ value
	for( i = 0; i < xycount; i++ )
	{
		unitile_st *thisblock = &xyblock[i]; // this is a easy/little tricky, to save a little calculation
		                                 // since the [i] is calclated several times below
			                             // if it doesn't help, it doesn't hurt either.
		signed int nItemTop = thisblock->basez + ((xyblock[i].type == 0) ? xyblock[i].height : calcTileHeight(xyblock[i].height)); // Calculate the items total height

		// check if the creature is floating on a static (keeping Z or falling)
		if( ( nItemTop >= newz ) && ( nItemTop <= oldz ) )
		{
			if( thisblock->flag2 & 0x02 )
			{ // walkable tile
				newz = nItemTop;
				ontype = thisblock->type;
				if( thisblock->flag4 == 0x80 ) { // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
				}
				continue;
			}
		}

		// So now comes next step, levitation :o)
		// you can gain Z to a limited amount if yo uwere climbing on last move on a ladder
		if( ( nItemTop >= newz ) && ( may_levitate ) && ( nItemTop <= oldz + MAX_Z_LEVITATE ) &&
			( thisblock->flag2 & 0x02 )
			)
		{
			ontype = thisblock->type;
			newz = nItemTop;
			if( thisblock->flag4 == 0x80 ) { // if it was ladder the char is allowed to `levitate´ next move
				on_ladder = true;
			}
		}
		// check if the creature is climbing on a climbable Z
		// (gaining Z through stairs, ladders, etc)
		// This form has no height limit, and the tile bottom must start lower or
		// equal current height + levitateable limit
		if( ( nItemTop >= newz ) && ( thisblock->basez <= oldz + MAX_Z_LEVITATE ) )
		{
			if( (thisblock->flag2 & 0x04) || ( thisblock->type == 0 ) || // Climbable tile, map tiles are also climbable
			( (thisblock->flag1 == 0) && (thisblock->flag2 == 0x22) ) || // These are a special kind of tiles where OSI forgot 
																		 // to set the climbable flag
			( (nItemTop >= oldz && nItemTop <= oldz + 3) && (thisblock->flag2 & 0x02) )		 // Allow to climb a height of 1 even if the climbable flag is not set
																		 // so you can walkupon mushrooms, grasses or so with height of 1
																		 // if it is a walkable tile of course
			)
			{                 
				ontype = thisblock->type;
				newz = nItemTop;
				if( thisblock->flag4 == 0x80 ) 	{ // if it was ladder the char is allowed to `levitate´ next move
					on_ladder = true;
				}
			}
		}
	}

#if DEBUG_WALKING
		printf( "CheckWalkable calculate Z=%d\n", newz );
#endif
        int item_influence = higher( newz + MAX_ITEM_Z_INFLUENCE, oldz );
		// also take care to look on all tiles the creature has fallen through
		// (npc's walking on ocean bug)
		// now the new Z-cordinate of creature is known, 
		// check if it hits it's head against something (blocking in other words)
		bool isGM = IsGMBody( c );
		for(i = 0; i < xycount; i++)
		{
			unitile_st *thisblock = &xyblock[i]; 
			signed int nItemTop = thisblock->basez + ((xyblock[i].type == 0) ? xyblock[i].height : calcTileHeight(xyblock[i].height)); // Calculate the items total height
			unsigned char flag1 = thisblock->flag1;
			unsigned char flag2 = thisblock->flag2;
			unsigned char flag4 = thisblock->flag4;
		// yeah,yeah,  this if has grown more ugly than the devil hismelf...
		if( ( (flag1 & 0x40) ||                                                  // a normal blocking tile      
			((flag2 & 0x02) && (nItemTop > newz))                                // staircases don't have blocking set, so very guy's could walk into them without this check.
			) &&                                                                 //   but one can walk upon them!   
			!( ( isGM || chars[c].dead ) && ((flag2 & 0x10) || (flag4 & 0x20))   // ghosts can walk through doors
			) 
			) {                                                                    // blocking
				if ((nItemTop > newz) && (thisblock->basez <= item_influence ) ||
				((nItemTop == newz) && (ontype == 0))
				) 
					{ // in effact radius?
                        newz = illegal_z;
#if DEBUG_WALKING
						printf( "CheckWalkable blocked due to tile=%d at height=%d.\n", xyblock[i].id, xyblock[i].basez );
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
//		}
// end knoxos code

#if DEBUG_WALK
	printf("CanCharWalk: %dx %dy %dz\n", x, y, z);
#endif
	if( (newz > illegal_z) && (!justask)) {
		// save information if we have climbed on last move.
		chars[c].may_levitate = on_ladder;
	}
	return newz;
}

// Function      : cMovement::CanCharMove()
// Written by    : Thyme
// Revision Date : 2000.09.17
// Purpose       : Check if a character can walk to a from x,y to dir direction
// Method        : This handles the funky diagonal moves.

bool cMovement::calc_move(CHARACTER c, short int x, short int y, signed char &z, int dir)
{
	if ( ( dir & 0x07 ) % 2 )
	{ // check three ways.
		unsigned int ndir = turn_counter_clock_wise( dir );
		if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, true ) == -128 )
			return false;
		ndir = turn_clock_wise( dir );
		if( calc_walk( c, GetXfromDir( ndir, x ), GetYfromDir( ndir, y ), x, y, true ) == -128 )
			return false;
	}
	z = calc_walk( c, GetXfromDir( dir, x ), GetYfromDir( dir, y ), x, y, false );
	return z > -128;
}

bool cMovement::MoveHeightAdjustment( int MoveType, unitile_st *thisblock, int &ontype, signed int &nItemTop, signed int &nNewZ )
{
	if ( ( MoveType & P_C_IS_GM_BODY ) && ( CanGMWalk(*(thisblock)) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->type;
		return true;
	}
	if ( ( MoveType & P_C_IS_PLAYER ) && ( CanPlayerWalk(*(thisblock)) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->type;
		return true;
	}
	if ( ( MoveType & P_C_IS_FISH ) && ( CanFishWalk(*(thisblock)) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->type;
		return true;
	}
	if ( ( MoveType & P_C_IS_NPC ) && ( CanNPCWalk(*(thisblock)) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->type;
		return true;
	}
	if ( ( MoveType & P_C_IS_BIRD ) && ( CanBirdWalk(*(thisblock)) ) )
	{
		nNewZ = nItemTop;
		ontype = thisblock->type;
		return true;
	}
	return false;
}

// knox, reinserted it since some other files access it,
//       100% sure this is wrong, however the smaller ill.
int cMovement::validNPCMove(short int x, short int y, signed char z, CHARACTER s)
{
    const int getcell=mapRegions->GetCell(x,y);
    int mapitem=-1;
    int mapitemptr=-1;

    chars[s].blocked++;
    do 
    {
        mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
        if (mapitemptr==-1) break;
        mapitem=mapRegions->GetItem(getcell, mapitemptr);
        if (mapitem!=-1 && mapitem<1000000)
        {
		    tile_st tile;
            Map->SeekTile((items[mapitem].id1<<8)+items[mapitem].id2, &tile);
            if (items[mapitem].x==x && items[mapitem].y==y && items[mapitem].z+tile.height>z+1 && items[mapitem].z<z+MaxZstep)
            {
                // bugfix found by JustMichael, moved by crackerjack
                // 8/2/99 makes code run faster too - one less loop :)
                if (items[mapitem].id1==0x39 && (items[mapitem].id2==0x46 || items[mapitem].id2==0x56)) return 0;
                if (items[mapitem].id1<=2 || (items[mapitem].id1==3 && items[mapitem].id2<=0xE2)) return 0;
                if (items[mapitem].id1==0x08 && (items[mapitem].id2>0x54 && items[mapitem].id2<0x66)) return 0;
                
                if (items[mapitem].type==12)
                {
                    if( chars[s].npc && (strlen(chars[s].title) > 0 || chars[s].npcaitype != 0 ) )
                    {                            
                        dooruse( -1, mapitem );
                    }                                   
                    chars[s].blocked = 0;
                    return 0;
                }
                
            }
        }
    } while (mapitem!=-1);

    // see if the map says its ok to move here
    if( Map->CanMonsterMoveHere( x, y, z ) )
	{
		chars[s].blocked = 0;
		return 1;
	}
    return 0;
}

