#include "uox3.h"
#include "cSpawnRegion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cScript.h"
#include "mapstuff.h"
#include "classes.h"
#include "Dictionary.h"
#include "StringUtility.hpp"
#include <sstream>
#include <iostream>
#include <regex>
#include <CJSMapping.h>

using namespace std::string_literals;

const TIMERVAL	DEFSPAWN_NEXTTIME		= 0;
const UI16		DEFSPAWN_CALL			= 1;
const SI16		DEFSPAWN_X1				= 0;
const SI16		DEFSPAWN_X2				= 0;
const SI16		DEFSPAWN_Y1				= 0;
const SI16		DEFSPAWN_Y2				= 0;
const size_t	DEFSPAWN_MAXCSPAWN		= 0;
const size_t	DEFSPAWN_MAXISPAWN		= 0;
const UI08		DEFSPAWN_MAXTIME		= 0;
const UI08		DEFSPAWN_MINTIME		= 0;
const SI32		DEFSPAWN_CURCSPAWN		= 0;
const SI32		DEFSPAWN_CURISPAWN		= 0;
const UI08		DEFSPAWN_WORLDNUM		= 0;
const SI08		DEFSPAWN_PREFZ			= 18;
const SI08		DEFSPAWN_DEFZ			= ILLEGAL_Z;
const bool		DEFSPAWN_ONLYOUTSIDE	= false;
const bool		DEFSPAWN_ISSPAWNER		= false;
const bool		DEFSPAWN_FORCESPAWN		= false;

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CSpawnRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for CSpawnRegion class
//o------------------------------------------------------------------------------------------------o
CSpawnRegion::CSpawnRegion( UI16 spawnregion ) : regionNum( spawnregion ), maxCharSpawn( DEFSPAWN_MAXCSPAWN ), maxItemSpawn( DEFSPAWN_MAXISPAWN ),
curCharSpawn( DEFSPAWN_CURCSPAWN ), curItemSpawn( DEFSPAWN_CURISPAWN ), minTime( DEFSPAWN_MINTIME ), maxTime( DEFSPAWN_MAXTIME ),
nextTime( DEFSPAWN_NEXTTIME ), x1( DEFSPAWN_X1 ), x2( DEFSPAWN_X2 ), y1( DEFSPAWN_Y1 ), y2( DEFSPAWN_Y2 ),
prefZ( DEFSPAWN_PREFZ ), defZ( DEFSPAWN_DEFZ ), onlyOutside( DEFSPAWN_ONLYOUTSIDE ), isSpawner( DEFSPAWN_ISSPAWNER ),
call( DEFSPAWN_CALL ), worldNumber( DEFSPAWN_WORLDNUM ), forceSpawn( DEFSPAWN_FORCESPAWN )
{
	sItems.resize( 0 );
	sNpcs.resize( 0 );
	exclusionAreas.resize( 0 );
	name = Dictionary->GetEntry( 1117 );
	// note: doesn't go here, but i'll see it here.  when an item is spawned, as soon as it's moved it needs to lose it's
	// spawn setting.  If not, then when people pick up spawned items, they will disappear (on region spawns)
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::~CSpawnRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for CSpawnRegion class
//o------------------------------------------------------------------------------------------------o
CSpawnRegion::~CSpawnRegion()
{
	sItems.resize( 0 );
	sNpcs.resize( 0 );
	exclusionAreas.resize( 0 );
	// Wipe out all items and npcs
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetName()
//|					CSpawnRegion::SetName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spawn region's name
//o------------------------------------------------------------------------------------------------o
const std::string CSpawnRegion::GetName( void ) const
{
	return name;
}
void CSpawnRegion::SetName( const std::string& newName )
{
	name = newName;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::MaxSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of objects a region can spawn
//o------------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxSpawn( void ) const
{
	return ( maxCharSpawn + maxItemSpawn );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMaxCharSpawn()
//|					CSpawnRegion::SetMaxCharSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of characters to spawn
//o------------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxCharSpawn( void ) const
{
	return maxCharSpawn;
}
void CSpawnRegion::SetMaxCharSpawn( size_t newVal )
{
	maxCharSpawn = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMaxItemSpawn()
//|					CSpawnRegion::SetMaxItemSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of items to spawn
//o------------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxItemSpawn( void ) const
{
	return maxItemSpawn;
}
void CSpawnRegion::SetMaxItemSpawn( size_t newVal )
{
	maxItemSpawn = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::Current()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Current amount of objects spawned
//o------------------------------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrent( void ) const
{
	return ( curCharSpawn + curItemSpawn );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetCurrentCharAmt()
//|					CSpawnRegion::IncCurrentCharAmt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current amount of characters spawned
//o------------------------------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrentCharAmt( void ) const
{
	return curCharSpawn;
}
void CSpawnRegion::IncCurrentCharAmt( SI16 incAmt )
{
	curCharSpawn += incAmt;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetCurrentItemAmt()
//|					CSpawnRegion::IncCurrentItemAmt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current amount of items spawned
//o------------------------------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrentItemAmt( void ) const
{
	return curItemSpawn;
}
void CSpawnRegion::IncCurrentItemAmt( SI16 incAmt )
{
	curItemSpawn += incAmt;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetRegionNum()
//|					CSpawnRegion::SetRegionNum()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spawn region number
//o------------------------------------------------------------------------------------------------o
UI16 CSpawnRegion::GetRegionNum( void ) const
{
	return regionNum;
}
void CSpawnRegion::SetRegionNum( UI16 newVal )
{
	regionNum = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMinTime()
//|					CSpawnRegion::SetMinTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum amount of time to pass before a spawnregion
//|					spawns a new object
//o------------------------------------------------------------------------------------------------o
UI08 CSpawnRegion::GetMinTime( void ) const
{
	return minTime;
}
void CSpawnRegion::SetMinTime( UI08 newVal )
{
	minTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetMaxTime()
//|					CSpawnRegion::SetMaxTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of time to pass before a spawnregion
//|					spawns a new object
//o------------------------------------------------------------------------------------------------o
UI08 CSpawnRegion::GetMaxTime( void ) const
{
	return maxTime;
}
void CSpawnRegion::SetMaxTime( UI08 newVal )
{
	maxTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetNextTime()
//|					CSpawnRegion::SetNextTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for when a spawnregion will next spawn a new object
//o------------------------------------------------------------------------------------------------o
TIMERVAL CSpawnRegion::GetNextTime( void ) const
{
	return nextTime;
}
void CSpawnRegion::SetNextTime( TIMERVAL newVal )
{
	nextTime = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::SetPrefZ()
//|					CSpawnRegion::GetPrefZ()
//|	Date		-	04/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z Level of the Spawn Region
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::SetPrefZ( SI08 newVal )
{
	prefZ = newVal;
}
SI08 CSpawnRegion::GetPrefZ( void ) const
{
	return prefZ;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::SetDefZ()
//|					CSpawnRegion::GetDefZ()
//|	Date		-	04/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets definitive Z Level of the Spawn Region at which to attempt spawning objects
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::SetDefZ( SI08 newVal )
{
	defZ = newVal;
}
SI08 CSpawnRegion::GetDefZ( void ) const
{
	return defZ;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetY1()
//|					CSpawnRegion::SetY1()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the y pos of the top corner of the spawnregion
//o------------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetY1( void ) const
{
	return y1;
}
void CSpawnRegion::SetY1( SI16 newVal )
{
	y1 = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetX1()
//|					CSpawnRegion::SetX1()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the x pos of the top corner of the spawnregion
//o------------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetX1( void ) const
{
	return x1;
}
void CSpawnRegion::SetX1( SI16 newVal )
{
	x1 = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetY2()
//|					CSpawnRegion::SetY2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the y pos of the bottom corner of the spawn region
//o------------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetY2( void ) const
{
	return y2;
}
void CSpawnRegion::SetY2( SI16 newVal )
{
	y2 = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetX2()
//|					CSpawnRegion::SetX2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the x pos of the bottom corner of the spawn region
//o------------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetX2( void ) const
{
	return x2;
}
void CSpawnRegion::SetX2( SI16 newVal )
{
	x2 = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::WorldNumber()
//|					CSpawnRegion::WorldNumber()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the world number of the spawnregion
//o------------------------------------------------------------------------------------------------o
UI08 CSpawnRegion::WorldNumber( void ) const
{
	return worldNumber;
}
void CSpawnRegion::WorldNumber( UI08 newVal )
{
	worldNumber = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetInstanceId()
//|					CSpawnRegion::SetInstanceId()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the instanceId of the spawnregion
//o------------------------------------------------------------------------------------------------o
UI16 CSpawnRegion::GetInstanceId( void ) const
{
	return instanceId;
}
void CSpawnRegion::SetInstanceId( UI16 newVal )
{
	instanceId = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetOnlyOutside()
//|					CSpawnRegion::SetOnlyOutside()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs should only spawn outside buildings
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::GetOnlyOutside( void ) const
{
	return onlyOutside;
}
void CSpawnRegion::SetOnlyOutside( bool newVal )
{
	onlyOutside = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::IsSpawner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items spawned from spawnregion is a spawner or not
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::IsSpawner( void ) const
{
	return isSpawner;
}
void CSpawnRegion::IsSpawner( bool newVal )
{
	isSpawner = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetForceSpawn()
//|					CSpawnRegion::SetForceSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether spawn region should force spawn NPCs, without validating location
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::GetForceSpawn( void ) const
{
	return forceSpawn;
}
void CSpawnRegion::SetForceSpawn( bool newVal )
{
	forceSpawn = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetCall()
//|					CSpawnRegion::SetCall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets number of objects that should be spawned in each call for a spawnregion
//o------------------------------------------------------------------------------------------------o
UI16 CSpawnRegion::GetCall( void ) const
{
	return call;
}
void CSpawnRegion::SetCall( UI16 newVal )
{
	call = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetNPC()
//|					CSpawnRegion::SetNPC()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stringlist of individual NPCs to spawn in a spawnregion
//|					Only gets set during loading in of spawnregions from DFN
//o------------------------------------------------------------------------------------------------o
// WHy isn't this returning a const reference?
auto CSpawnRegion::GetNPC( void ) const -> std::vector<std::string>
{
	return sNpcs;
}
void CSpawnRegion::SetNPC( const std::string &newVal )
{
	// Clear old entries to make room for new ones
	sNpcs.clear();
	sNpcs.push_back( newVal );
	BuildParsedLists();
}
void CSpawnRegion::SetNPCList( std::string newVal )
{
	// Clear old entries to make room for new ones
	sNpcs.clear();

	// Check if a comma delimited string was provided
	if( newVal.find( "," ) != std::string::npos )
	{
		// Strip whitespace from string
		std::regex r( "\\s+" );
		newVal = std::regex_replace( newVal, r, "" );

		// Add section of the string to the sNpcs list with the help of a stringstream
		std::stringstream s_stream( newVal );
		while( s_stream.good() )
		{
			std::string substr;
			getline( s_stream, substr, ',' );
			sNpcs.push_back( substr );
		}
	}
	else
	{
		// Assume an actual NPClist was provided
		LoadNPCList( newVal );
	}
	BuildParsedLists();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetItem()
//|					CSpawnRegion::SetItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stringlist of individual Items to spawn in a spawnregion
//o------------------------------------------------------------------------------------------------o
// THIS SHOULD BE A reference return! If you need to handle  const/non const, include both types
// Just changing all this wojuld prevent copys being made each time it is called!
// Need to change the header as well of course
// example
/*
 auto CSpawnRegion::GetItem() const -> const std::vector<std::string> &
 {
 	return sItems;
 }
 auto CSpawnRegion::GetItem() -> std::vector<std::string> &
 {
 	return sItems;
 }
 */
auto CSpawnRegion::GetItem() const -> std::vector<std::string>
{
	return sItems;
}
void CSpawnRegion::SetItem( const std::string &newVal )
{
	// Clear old entries to make room for new ones
	sItems.clear();
	sItems.push_back( newVal );
	BuildParsedLists();
}
void CSpawnRegion::SetItemList( std::string newVal )
{
	// Clear old entries to make room for new ones
	sItems.clear();

	// Check if a comma delimited string was provided
	if( newVal.find( "," ) != std::string::npos )
	{
		// Strip whitespace from string
		std::regex r( "\\s+" );
		newVal = std::regex_replace( newVal, r, "" );

		// Add section of the string to the sItems list with the help of a stringstream
		std::stringstream s_stream( newVal );
		while( s_stream.good() )
		{
			std::string substr;
			getline( s_stream, substr, ',' );
			sItems.push_back( substr );
		}
	}
	else
	{
		// Assume an actual ItemList was provided
		LoadItemList( newVal );
	}
	BuildParsedLists();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::LoadNPCList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads NPCLIST from DFNs with list of NPCs to spawn for a spawnregion
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::LoadNPCList( const std::string &npcList )
{
	std::string sect = "NPCLIST " + npcList;
	CScriptSection *CharList = FileLookup->FindEntry( sect, npc_def );
	if( CharList != nullptr )
	{
		for( std::string npc = CharList->First(); !CharList->AtEnd(); npc = CharList->Next() )
		{
			if( oldstrutil::upper( npc ) == "NPCLIST" )
			{
				LoadNPCList( CharList->GrabData() );
			}
			else
			{
				auto charData = CharList->GrabData();
				if( !charData.empty() )
				{
					// Entry in npclist contains data, likely another npclist with weight in front
					sNpcs.push_back( npc + "=" + CharList->GrabData() );
				}
				else
				{
					sNpcs.push_back( npc );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::LoadItemList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads ITEMLIST from DFNs with list of Items to spawn for a spawnregion
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::LoadItemList( const std::string &itemList )
{
	std::string sect = "ITEMLIST " + itemList;
	CScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList != nullptr )
	{
		for( std::string itm = ItemList->First(); !ItemList->AtEnd(); itm = ItemList->Next() )
		{
			if( oldstrutil::upper( itm ) == "ITEMLIST" )
			{
				LoadItemList( ItemList->GrabData() );
			}
			else
			{
				sItems.push_back( itm );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function     -   CSpawnRegion::SpawnRegionScripts()
//|                 CSpawnRegion::ClearSpawnRegionScripts()
//|                 CSpawnRegion::AddSpawnRegionScripts()
//o------------------------------------------------------------------------------------------------o
//| Purpose      -   Manage extra scripts applied to objects spawned from this spawn region
//o------------------------------------------------------------------------------------------------o
const std::vector<UI16> &CSpawnRegion::SpawnRegionScripts( void ) const
{
    return spawnRegionScripts;
}

void CSpawnRegion::ClearSpawnRegionScripts( void )
{
    spawnRegionScripts.clear();
}

void CSpawnRegion::AddSpawnRegionScripts( UI16 scriptId )
{
    spawnRegionScripts.push_back( scriptId );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the spawnregion from spawn.dfn script entry
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::Load( CScriptSection *toScan, bool isParentSpawnRegion )
{
	std::string sect;
	std::string data;
	std::string UTag;

	auto coreShardEra = cwmWorldState->ServerData()->ExpansionCoreShardEra();

	// Default to instanceId 0, in case nothing else is specified in DFN
	SetInstanceId( 0 );

	SpawnRegionExclusionAreas_st excludeArea;
	for( std::string tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
	{
		if( !tag.empty() )
		{
			UTag = oldstrutil::upper( tag );
			data = toScan->GrabData();

			switch(( UTag.data()[0] ))
			{
				case 'A':
				{
					if( UTag == "ADDSCRIPT" )
					{
						// Example: ADDSCRIPT=7500,7501,7502
						data = oldstrutil::simplify( data );
						auto csecs = oldstrutil::sections( data, "," );

						for( auto& sect : csecs )
						{
							auto value = oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ) );
							if( value.empty() )
								continue;

							// Script IDs are always decimal
							UI16 scriptId = static_cast< UI16 >( std::stoul( value, nullptr, 10 ) );
							AddSpawnRegionScripts( scriptId );
						}
					}
					break;
				}
				case 'C':
				{
					if( UTag == "CALL" )
					{
						call = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					break;
				}
				case 'D':
				{
					if( UTag == "DEFZ" )
					{
						defZ = static_cast<SI08>( std::stoi( data, nullptr, 0 ));
					}
					break;
				}
				case 'E':
				{
					if( UTag == "EXCLUDEAREA" )
					{
						// For every "EXCLUDEAREA" tag that contains 4 comma-separated coordinates (EXCLUDEAREA=X1,Y1,X2,Y2), push them back into a list of exclusion areas that will be avoided during spawning
						data = oldstrutil::simplify( data );
						auto csecs = oldstrutil::sections( data, "," );
						if( csecs.size() == 4 )
						{
							excludeArea.x1 = static_cast<SI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
							excludeArea.y1 = static_cast<SI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
							excludeArea.x2 = static_cast<SI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
							excludeArea.y2 = static_cast<SI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 ));

							if( excludeArea.x1 > excludeArea.x2 || excludeArea.y1 > excludeArea.y2 )
							{
								Console.Warning( oldstrutil::format( "Invalid coordinates for EXCLUDEAREA in REGIONSPAWN %u: (%i,%i) to (%i,%i) (x1 > x2 or y1 > y2)!", 
									regionNum, excludeArea.x1, excludeArea.y1, excludeArea.x2, excludeArea.y2 ));
							}
							else
							{
								exclusionAreas.push_back( excludeArea );
							}
						}
					}
					else if( UTag == "ERAS" && !isParentSpawnRegion )
					{
						auto ssecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), "," );
						if( ssecs.size() == 1 )
						{
							// Only one era specified
							if( cwmWorldState->ServerData()->EraStringToEnum( ssecs[0], false, false ) != cwmWorldState->ServerData()->ExpansionCoreShardEra() )
							{
								// SpawnRegion not intended for current shard era, abort!
								return false;
							}
						}
						else
						{
							// Multiple eras specified
							bool eraFound = false;
							for( int i = 0; i < ssecs.size(); i++ )
							{
								if( cwmWorldState->ServerData()->EraStringToEnum( oldstrutil::simplify( ssecs[i] ), false, false ) == cwmWorldState->ServerData()->ExpansionCoreShardEra() )
								{
									eraFound = true;
								}
							}
							if( !eraFound )
							{
								// SpawnRegion not intended for current shard era, abort!
								return false;
							}
						}
					}
					break;
				}
				case 'F':
				{
					if( UTag == "FORCESPAWN" )
					{
						forceSpawn = (static_cast<SI08>( std::stoi( data, nullptr, 0 )) == 1 );
					}
					break;
				}
				case 'G':
				{
					if( UTag == "GET" )
					{
						data = "REGIONSPAWN " + oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
						CScriptSection *spawnRegionParent = FileLookup->FindEntry( data, spawn_def );
						if( spawnRegionParent == nullptr )
						{
							Console.Warning( oldstrutil::format( "Invalid script entry (%s) called with GET tag in REGIONSPAWN section.", data.c_str() ));
						}
						else if( spawnRegionParent == toScan )
						{
							Console.Warning( oldstrutil::format( "Infinite loop avoided with GET tag in REGIONSPAWN [%s]", data.c_str() ));
						}
						else
						{
							// Load in data from "parent" spawn region
							auto loadParentResult = Load( spawnRegionParent, true );
							if( !loadParentResult )
							{
								Console.Warning( oldstrutil::format( "Unable to load parent spawn region (%s), aborting!", data.c_str() ));
								return false;
							}
							else
							{
								// Parent loaded! But we don't want the spawn entries from the parent - we want to use our own!
								sNpcs.clear();
								sItems.clear();
							}
						}
					}
					break;
				}
				case 'I':
				{
					if( UTag == "INSTANCEID" )
					{
						instanceId = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "ITEMLIST" && !isParentSpawnRegion )
					{
						LoadItemList( data );
					}
					else if( UTag == "ITEM" && !isParentSpawnRegion )
					{
						sItems.push_back( data );
					}
					else if( UTag == "ISSPAWNER" )
					{
						isSpawner = ( static_cast<SI08>( std::stoi( data, nullptr, 0 )) == 1 );
					}
					break;
				}
				case 'M':
				{
					if( UTag == "MAXITEMS" )
					{
						maxItemSpawn = static_cast<UI32>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "MAXNPCS" )
					{
						maxCharSpawn = static_cast<UI32>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "MINTIME" )
					{
						minTime = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "MAXTIME" )
					{
						maxTime = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
					}
					break;
				}
				case 'N':
				{
					if( UTag == "NAME" )
					{
						name = data;
					}
					else if( UTag == "NPCLIST" && !isParentSpawnRegion )
					{
						LoadNPCList( data );
					}
					else if( UTag == "NPC" && !isParentSpawnRegion )
					{
						sNpcs.push_back( data );
					}
					break;
				}
				case 'O':
				{
					if( UTag == "ONLYOUTSIDE" )
					{
						onlyOutside = (static_cast<SI08>( std::stoi( data, nullptr, 0 )) == 1 );
					}
					break;
				}
				case 'P':
				{
					if( UTag == "PREFZ" )
					{
						prefZ = static_cast<SI08>( std::stoi( data, nullptr, 0 ));
					}
					break;
				}
				case 'V':
				{
					if( UTag == "VALIDLANDPOS" )
					{
						data = oldstrutil::simplify( data );
						auto csecs = oldstrutil::sections( data, "," );
						
						if( csecs.size() == 3 )
						{
							SI16 x = static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
							SI16 y = static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
							SI08 z = static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
							
							// Stuff the provided Z into both cache values
							SI08 baseZ = z;
							SI08 staticZ = z;

							UI32 cacheKey = y + ( x << 16 );
							auto [it, inserted] = validLandPosCheck.try_emplace( cacheKey, x, y, baseZ, staticZ );
							if( inserted )
							{
								validLandPos.emplace_back( x, y, baseZ, staticZ );
							}
						}
						else
						{
							Console.Warning( oldstrutil::format( "Invalid VALIDLANDPOS format in region %i. Expected X,Y,Z.", GetRegionNum() ));
						}
					}
					else if( UTag == "VALIDWATERPOS" )
					{
						data = oldstrutil::simplify( data );
						auto csecs = oldstrutil::sections( data, "," );
						
						if( csecs.size() == 3 )
						{
							SI16 x = static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
							SI16 y = static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
							SI08 z = static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
							
							// Stuff the provided Z into both cache values
							SI08 baseZ = z;
							SI08 staticZ = z;

							UI32 cacheKey = y + ( x << 16 );
							auto [it, inserted] = validWaterPosCheck.try_emplace( cacheKey, x, y, baseZ, staticZ );
							if( inserted )
							{
								validWaterPos.emplace_back( x, y, baseZ, staticZ );
							}
						}
						else
						{
							Console.Warning( oldstrutil::format( "Invalid VALIDWATERPOS format in region %i. Expected X,Y,Z.", GetRegionNum() ));
						}
					}
					break;
				}
				case 'W':
				{
					if( UTag == "WORLD" )
					{
						worldNumber = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					break;
				}
				case 'X':
				{
					if( UTag == "X1" )
					{
						x1 = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
					}
					else if( UTag == "X2" )
					{
						x2 = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
					}
					break;
				}
				case 'Y':
				{
					if( UTag == "Y1" )
					{
						y1 = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
					}
					else if( UTag == "Y2" )
					{
						y2 = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
					}
					break;
				}
				default:
				{
					Console.Warning( oldstrutil::format( "Unknown DFN tag found in REGIONSPAWN section: %s", UTag.c_str() ));
					break;
				}
			}
		}
	}
	BuildParsedLists();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::DoRegionSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do spawn stuff for spawnregion
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::DoRegionSpawn( UI32& itemsSpawned, UI32& npcsSpawned )
{
	// Only perform the region spawn if the spawn region in question is active
	if( !cwmWorldState->ServerData()->GetSpawnRegionsFacetStatus( static_cast<UI32>( WorldNumber() )))
		return;

	if( sNpcs.empty() )
	{
		maxCharSpawn = 0;
	}
	if( sItems.empty() )
	{
		maxItemSpawn = 0;
	}

	bool shouldSpawnChars = ( !sNpcs.empty() && maxCharSpawn > spawnedChars.Num() );
	bool shouldSpawnItems = ( !sItems.empty() && maxItemSpawn > spawnedItems.Num() );
	if( shouldSpawnChars || shouldSpawnItems )
	{
		CChar *spawnChar		= nullptr;
		CItem *spawnItem		= nullptr;
		const UI08 spawnChars	= ( shouldSpawnChars ? 0 : 50 );
		const UI08 spawnItems	= ( shouldSpawnItems ? 100 : 49 );
		for( UI16 i = 0; i < call && ( shouldSpawnItems || shouldSpawnChars ); ++i )
		{
			if( RandomNum( static_cast<UI16>( spawnChars ), static_cast<UI16>( spawnItems )) > 49 )
			{
				if( shouldSpawnItems )
				{
					spawnItem = RegionSpawnItem();
					if( ValidateObject( spawnItem ))
					{
						spawnedItems.Add( spawnItem, false );
						++itemsSpawned;
						shouldSpawnItems = ( spawnedItems.Num() < maxItemSpawn );
					}
				}
			}
			else
			{
				if( shouldSpawnChars )
				{
					spawnChar = RegionSpawnChar();
					if( ValidateObject( spawnChar ))
					{
						spawnedChars.Add( spawnChar, false );
						++npcsSpawned;
						shouldSpawnChars = ( spawnedChars.Num() < maxCharSpawn );
					}
				}
			}
		}
	}

	SetNextTime( BuildTimeValue( static_cast<R64>( RandomNum( static_cast<UI16>( GetMinTime() * 60 ), static_cast<UI16>( GetMaxTime() * 60 )))));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::BuildParsedLists()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Pre-parses raw string NPC and Item lists into weighted pair vectors
//|					to eliminate dynamic string splitting and heap allocations during spawn ticks
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::BuildParsedLists()
{
	parsedNpcList.clear();
	parsedNpcList.reserve( sNpcs.size() );
	for( const auto &entry : sNpcs )
	{
		auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( entry, "//" )), "|" );
		UI16 sectionWeight = 1;
		if( csecs.size() > 1 )
		{
			sectionWeight = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
		}
		auto npcSection = ( csecs.size() > 1 ? csecs[1] : csecs[0] );
		parsedNpcList.emplace_back( npcSection, sectionWeight );
	}

	parsedItemList.clear();
	parsedItemList.reserve( sItems.size() );
	for( const auto &entry : sItems )
	{
		auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( entry, "//" )), "|" );
		UI16 sectionWeight = 1;
		if( csecs.size() > 1 )
		{
			sectionWeight = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
		}
		auto itemSection = ( csecs.size() > 1 ? csecs[1] : csecs[0] );
		parsedItemList.emplace_back( itemSection, sectionWeight );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::RegionSpawnChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do a char spawn
//o------------------------------------------------------------------------------------------------o
auto CSpawnRegion::RegionSpawnChar() -> CChar *
{	
	if( parsedNpcList.empty() )
		return nullptr;

	auto ourNPC = Npcs->ChooseNpcToCreate( parsedNpcList );
	if( ourNPC.empty() )
		return nullptr;

	auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( ourNPC, "//" )), "=" );
	if( oldstrutil::upper( csecs[0] ) == "NPCLIST" )
	{
		// Chosen entry contained another NPCLIST! Let's dive back into it...
		ourNPC = Npcs->NpcListLookup( csecs[1] );
	}

	CScriptSection *npcCreate = FileLookup->FindEntry( ourNPC, npc_def );
	if( npcCreate == nullptr )
		return nullptr;

	// Extract NPC ID from DFN tag
	UI16 npcId = 0;
	for( const auto *entry : npcCreate->collection2() )
	{
		if( entry->tag == DFNTAG_ID )
		{
			npcId = oldstrutil::value<UI16>( entry->cdata );
			break;
		}
	}

	bool waterCreature = false;
	bool amphiCreature = false;
	if( npcId > 0 )
	{
		waterCreature = cwmWorldState->creatures[npcId].IsWater();
		amphiCreature = cwmWorldState->creatures[npcId].IsAmphibian();
	}

	SI16 x, y;
	SI08 z;

	// Start timing
	auto tStart = std::chrono::high_resolution_clock::now();

	if( FindCharSpotToSpawn( x, y, z, waterCreature, amphiCreature ))
	{
		// Mark end of location finding
		auto tLoc = std::chrono::high_resolution_clock::now();

		CChar *CSpawn = nullptr;
		CSpawn = Npcs->CreateBaseNPC( ourNPC, false );

		if( CSpawn != nullptr )
		{
			// NPCs should always wander the whole spawnregion
			CSpawn->SetNpcWander( WT_BOX );
			CSpawn->SetFx( x1, 0 );
			CSpawn->SetFx( x2, 1 );
			CSpawn->SetFy( y1, 0 );
			CSpawn->SetFy( y2, 1 );
			CSpawn->SetLocation( x, y, z, worldNumber, instanceId );
			CSpawn->SetSpawnX( x );
			CSpawn->SetSpawnY( y );
			CSpawn->SetSpawnZ( z );
			CSpawn->SetSpawned( true );
			CSpawn->ShouldSave( false );
			CSpawn->SetSpawn( static_cast<UI32>( regionNum ));
			Npcs->PostSpawnUpdate( CSpawn );
			IncCurrentCharAmt();

			TAGMAPOBJECT tagvalObject;
			tagvalObject.m_ObjectType	= TAGMAP_TYPE_INT;
			tagvalObject.m_IntValue		= this->GetRegionNum();
			tagvalObject.m_Destroy		= false;
			tagvalObject.m_StringValue	= "";
			CSpawn->SetTag( "spawnRegion", tagvalObject );

			// Extra scripts applied from REGIONSPAWN ADDSCRIPT tag
			for( auto scriptId : spawnRegionScripts )
			{
				CSpawn->AddScriptTrigger( scriptId );
			}

			// Get all script triggers and run onSpawn on each
			for( auto scriptTrig : CSpawn->GetScriptTriggers() )
			{
				cScript* toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnSpawn( CSpawn, regionNum );
				}
			}

			// Mark end of NPC creation
			auto tEnd = std::chrono::high_resolution_clock::now();

			// Accumulate and print averages
			static long long totalLocTime = 0;
			static long long totalSpawnTime = 0;
			static int spawnCount = 0;

			totalLocTime += std::chrono::duration_cast<std::chrono::microseconds>( tLoc - tStart ).count();
			totalSpawnTime += std::chrono::duration_cast<std::chrono::microseconds>( tEnd - tLoc ).count();
			spawnCount++;

			if( spawnCount % 100 == 0 )
			{
				Console.Print( oldstrutil::format("Avg Loc Time: %lld us | Avg Creation Time: %lld us\n", (totalLocTime / spawnCount), (totalSpawnTime / spawnCount )));
			}

			return CSpawn;
		}
	}
	else
	{
		Console.Warning( oldstrutil::format( "Unable to find valid location to spawn NPC in region %i", this->GetRegionNum() ));
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::RegionSpawnItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do an item spawn
//o------------------------------------------------------------------------------------------------o
auto CSpawnRegion::RegionSpawnItem() -> CItem *
{
	CItem *ISpawn = nullptr;
	SI16 x, y;
	SI08 z;
	std::string idToSpawn;
	if( !parsedItemList.empty() )
	{
		idToSpawn = Npcs->ChooseNpcToCreate( parsedItemList );
	}
	else if( !sItems.empty() )
	{
		idToSpawn = sItems[RandomNum( static_cast<size_t>( 0 ), sItems.size() - 1 )];
	}
	if( idToSpawn.empty() )
		return nullptr;

	if( FindItemSpotToSpawn( x, y, z, idToSpawn ))
	{
		auto objType = OT_ITEM;
		if( isSpawner )
		{
			objType = OT_SPAWNER;
		}

		ISpawn = Items->CreateBaseScriptItem( nullptr, idToSpawn, worldNumber, 1, instanceId, objType, 0xFFFF, false );
		if( ISpawn != nullptr )
		{
			ISpawn->SetLocation( x, y, z );
			ISpawn->SetSpawn( static_cast<UI32>( regionNum ));
			ISpawn->SetSpawned( true );
			ISpawn->ShouldSave( false );
			IncCurrentItemAmt();
			// Extra scripts applied from REGIONSPAWN ADDSCRIPT tag
			for( auto scriptId : spawnRegionScripts )
			{
				ISpawn->AddScriptTrigger( scriptId );
			}

			// Get all script triggers and run onSpawn on each
			std::vector<UI16> scriptTriggers = ISpawn->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript* toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					// Assumes cScript has OnSpawn( CBaseObject* ), mirroring your OnCreate usage
					toExecute->OnSpawn( ISpawn, regionNum );
				}
			}
		}
	}
	return ISpawn;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::IsInExclusionArea()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if coordinates fall within an exclusion area
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::IsInExclusionArea( SI16 x, SI16 y )
{
	for( auto &exclusionArea : exclusionAreas )
	{
		if( x >= exclusionArea.x1 && x <= exclusionArea.x2 && y >= exclusionArea.y1 && y <= exclusionArea.y2 )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::TryRandomStoredLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper function to try picking at random from list of already valid locations
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::TryRandomStoredLocation( const std::vector<CachedSpawnPoint>& validPositions, SI16& x, SI16& y, SI08& z, SI08& z2 )
{
	if( validPositions.empty() )
		return false;

	const CachedSpawnPoint &loc = validPositions[RandomNum( static_cast<size_t>( 0 ), validPositions.size() - 1 )];
	x = static_cast<SI16>( loc.x );
	y = static_cast<SI16>( loc.y );
	z = static_cast<SI08>( loc.baseZ );
	z2 = static_cast<SI08>( loc.staticZ );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::FindCharSpotToSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for spawning a character
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindCharSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, bool &waterCreature, bool &amphiCreature )
{
	bool foundLoc = false;
	SI08 z2 = ILLEGAL_Z;
	const size_t landPosSize = validLandPos.size();
	const size_t waterPosSize = validWaterPos.size();

	// Determine which cache size to use
	// Note: Amphibians can use either!
	size_t posSize = 0;
	if( amphiCreature )
		posSize = landPosSize + waterPosSize;
	else if( waterCreature )
		posSize = waterPosSize;
	else
		posSize = landPosSize;

	// Scale chance of trying cached locations early based on number of cached locations found (up to 65% chance at 75+ locations found). More cached locations = higher chance of using cached locations
	const R32 cacheUsageProbability = ( 0.65f ) * std::min( static_cast<R32>( posSize ) / 75, 1.0f);
	UI08 maxSpawnAttempts = std::max( static_cast<UI08>( 25 ), static_cast<UI08>( std::max<SI32>( 0, static_cast<SI32>( 100 ) - static_cast<SI32>( posSize ))));

	bool tryWater = waterCreature;
	for( UI08 attempt = 0; attempt < maxSpawnAttempts; ++attempt )
	{
		foundLoc = false;
		SI08 baseZ = ILLEGAL_Z;
		SI08 staticZ = ILLEGAL_Z;

		// Decide whether to randomize spawn location or use already validated location from cache
		if( RandomNum( 0, 100 ) < static_cast<UI08>( cacheUsageProbability * 100 ))
		{
			// For amphibians, decide which pool to pull from based on weighted probability
			if( amphiCreature && posSize > 0 )
			{
				size_t cacheRoll = RandomNum( static_cast<size_t>( 0 ), posSize - 1 );
				tryWater = ( cacheRoll >= landPosSize );
			}

			if( tryWater )
			{
				if( !validWaterPos.empty() && TryRandomStoredLocation( validWaterPos, x, y, z, z2 ))
				{
					baseZ = z;
					staticZ = z2;
					foundLoc = true;
				}
			}
			else
			{
				// Try land location for non-water creatures
				if( !validLandPos.empty() && TryRandomStoredLocation( validLandPos, x, y, z, z2 ))
				{
					baseZ = z;
					staticZ = z2;
					foundLoc = true;
				}
			}
		}

		// No location has been selected yet, so try to find a new random location
		if( !foundLoc )
		{
			x = RandomNum( x1, x2 );
			y = RandomNum( y1, y2 );

			// If chosen location is in an exclusion area defined in spawn region, continue
			if( IsInExclusionArea( x, y ))
				continue;

			// Check if random location exists in cache already; if so we can reuse the base/static Z values without looking them up again
			UI32 cacheKey = y + ( x << 16 );

			if( !waterCreature )
			{
				auto landIt = validLandPosCheck.find( cacheKey );
				if( landIt != validLandPosCheck.end() )
				{
					// Found randomly selected location in land cache
					baseZ = landIt->second.baseZ; // Use cached baseZ
					staticZ = landIt->second.staticZ; // Use cached staticZ
					foundLoc = true;
				}
			}

			if( !foundLoc && ( waterCreature || amphiCreature ))
			{
				auto waterIt = validWaterPosCheck.find( cacheKey );
				if( waterIt != validWaterPosCheck.end() )
				{
					// Found randomly selected location in water cache
					baseZ = waterIt->second.baseZ; // Use cached baseZ
					staticZ = waterIt->second.staticZ; // Use cached staticZ
					foundLoc = true;
				}
			}
		}

		// If DEFZ is defined for spawn region, use it
		// Trust that the spawnregion scripter knows what they are doing...
		if( defZ != ILLEGAL_Z )
		{
			z = defZ;
		}
		else
		{
			// If baseZ is not set by now, location was not found in cache
			if( baseZ == ILLEGAL_Z )
			{
				// Get base map height for the random location
				baseZ = Map->MapElevation( x, y, worldNumber );
			}
			z = baseZ;

			// No definite Z has been defined, look for valid dynamic Z based on prefZ influence
			const SI08 dynZ = Map->DynamicElevation( x, y, baseZ, worldNumber, instanceId, prefZ, ( waterCreature ? false : true ), ( waterCreature ? true : false ));
			if( ILLEGAL_Z != dynZ && dynZ > baseZ )
			{
				z = dynZ;
			}

			// Even if we got a valid dynamic Z, there might be a better match with statics, based on prefZ influence
			if( staticZ == ILLEGAL_Z )
			{
				staticZ = Map->StaticTop( x, y, z, worldNumber, prefZ, ( waterCreature ? false : true ), ( waterCreature ? true : false ));
			}
			if( ILLEGAL_Z != staticZ && staticZ > z )
			{
				z = staticZ;
			}
		}

		// No valid Z found, proceed to next attempt
		if( z == ILLEGAL_Z )
			continue;

		if( forceSpawn )
		{
			// Don't care about validating the location, force spawn at own risk!
			return true;
		}

		// Since our chosen location has not already been validated, lets validate it with a land-based creature in mind
		// Revalidate the chosen location with land-based creature in mind, and store it in quick-lookup cache if valid
		if( !tryWater && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, true, foundLoc ))
		{
			if( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId ))
			{
				UI32 cacheKey = y + ( x << 16 );
				auto [it, inserted] = validLandPosCheck.try_emplace( cacheKey, x, y, baseZ, staticZ );
	
				// Add to vector if inserted successfully
				if( inserted )
				{
					validLandPos.emplace_back( x, y, baseZ, staticZ );
				}
				return true;
			}
		}

		// Otherwise, validate it with a water-based creature in mind instead
		if(( tryWater || amphiCreature ) && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, false, foundLoc ))
		{
			if( onlyOutside == false ) //|| !Map->InBuilding( x, y, z, worldNumber, instanceId ))
			{
				UI32 cacheKey = y + ( x << 16 );
				auto [it, inserted] = validWaterPosCheck.try_emplace( cacheKey, x, y, baseZ, staticZ );
	
				// Add to vector if inserted successfully
				if( inserted )
				{
					validWaterPos.emplace_back( x, y, baseZ, staticZ );
				}
				return true;
			}
		}
	}

	// FALLBACK
	// If we haven't found a valid location in all our allowed attempts, pick a random location from the cached ones
	bool fallbackToWater = waterCreature;
	if( amphiCreature && posSize > 0 )
	{
		size_t cacheRoll = RandomNum( static_cast<size_t>( 0 ), posSize - 1 );
		fallbackToWater = ( cacheRoll >= landPosSize );
	}

	if( !fallbackToWater && !validLandPos.empty() )
	{
		if( TryRandomStoredLocation( validLandPos, x, y, z, z2 ) )
		{
			// Combine baseZ (z) and staticZ (z2) to get the best starting Z
			if( z2 != ILLEGAL_Z && z2 > z )
			{
				z = z2;
			}

			// Run dynamic elevation check
			const SI08 dynZ = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ, true, false );
			if( ILLEGAL_Z != dynZ && dynZ > z )
			{
				z = dynZ;
			}
			
			// Validate spawn location
			if( z != ILLEGAL_Z && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, true, true ) && ( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId ) ) )
			{
				return true;
			}
		}
	}
	else if( fallbackToWater && !validWaterPos.empty() )
	{
		if( TryRandomStoredLocation( validWaterPos, x, y, z, z2 ) )
		{
			// Combine baseZ (z) and staticZ (z2) to get the best starting Z
			if( z2 != ILLEGAL_Z && z2 > z )
			{
				z = z2;
			}

			// Run dynamic elevation check
			const SI08 dynZ = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ, ( waterCreature ? false : true ), ( waterCreature ? true : false ));
			if( ILLEGAL_Z != dynZ && dynZ > z )
			{
				z = dynZ;
			}

			// Validate spawn location
			if( z != ILLEGAL_Z && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, false, true ))
			{
				return true;
			}
		}
	}
	
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::FindItemSpotToSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindItemSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, std::string idToSpawn )
{
	bool foundLoc = false;
	SI08 z2 = ILLEGAL_Z;
	const size_t landPosSize = validLandPos.size();

	// Scale chance of trying cached locations early based on number of cached locations found (up to 80% chance at 75+ locations found). More cached locations = higher chance of using cached locations
	const R32 cacheUsageProbability = ( 0.8f ) * std::min( static_cast<R32>( landPosSize ) / 75, 1.0f);
	UI08 maxSpawnAttempts = std::max( static_cast<UI08>( 25 ), static_cast<UI08>( std::max<SI32>( 0, static_cast<SI32>( 100 ) - static_cast<SI32>( landPosSize ))));

	for( UI08 attempt = 0; attempt < maxSpawnAttempts; ++attempt )
	{
		foundLoc = false;
		SI08 baseZ = ILLEGAL_Z;
		SI08 staticZ = ILLEGAL_Z;

		// Decide whether to randomize spawn location or use already validated location from cache
		if( RandomNum( 0, 100 ) < static_cast<UI08>( cacheUsageProbability * 100 ))
		{
			if( !validLandPos.empty() && TryRandomStoredLocation( validLandPos, x, y, z, z2 ))
			{
				baseZ = z;
				staticZ = z2;
				foundLoc = true;
			}
		}

		// No location has been selected yet, so try to find a new random location
		if( !foundLoc )
		{
			x = RandomNum( x1, x2 );
			y = RandomNum( y1, y2 );

			// If chosen location is in an exclusion area defined in spawn region, continue
			if( IsInExclusionArea( x, y ))
				continue;

			// Check if random location exists in cache already; if so we can reuse the base/static Z values without looking them up again
			UI32 cacheKey = y + ( x << 16 );
			auto landIt = validLandPosCheck.find( cacheKey );
			if( landIt != validLandPosCheck.end() )
			{
				// Found randomly selected location in land cache
				baseZ = landIt->second.baseZ; // Use cached baseZ
				staticZ = landIt->second.staticZ; // Use cached staticZ
				foundLoc = true;
			}
		}

		// If DEFZ is defined for spawn region, use it
		// Trust that the spawnregion scripter knows what they are doing...
		if( defZ != ILLEGAL_Z )
		{
			z = defZ;
		}
		else
		{
			// Location not found in cache, continue with validation!
			// First, get base map height for the random location
			if( baseZ == ILLEGAL_Z )
			{
				baseZ = Map->MapElevation( x, y, worldNumber );
			}
			z = baseZ;

			// No definite Z has been defined, look for valid dynamic Z based on prefZ influence
			const SI08 dynZ = Map->DynamicElevation( x, y, baseZ, worldNumber, instanceId, prefZ, true, false );
			if( ILLEGAL_Z != dynZ && dynZ > baseZ )
			{
				z = dynZ;
			}

			// Even if we got a valid dynamic Z, there might be a better match with statics, based on prefZ influence
			if( staticZ == ILLEGAL_Z )
			{
				staticZ = Map->StaticTop( x, y, z, worldNumber, prefZ, true, false );
			}
			if( ILLEGAL_Z != staticZ && staticZ > z )
			{
				z = staticZ;
			}
		}

		// No valid Z found, proceed to next attempt
		if( z == ILLEGAL_Z )
			continue;

		if( forceSpawn )
		{
			// Don't care about validating the location, force spawn at own risk!
			return true;
		}

		// Don't spawn an item in a location where same item has already spawned before
		auto itemAtXYZ = GetItemAtXYZ( x, y, z, worldNumber, instanceId );
		if( ValidateObject( itemAtXYZ ) && itemAtXYZ->GetSectionId() == idToSpawn )
		{
			continue;
		}

		// Since our chosen location has not already been validated, lets validate it now, and store it in quick-lookup cache if valid
		if( Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, true, foundLoc ))
		{
			if( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId ))
			{
				UI32 cacheKey = y + ( x << 16 );
				auto [it, inserted] = validLandPosCheck.try_emplace( cacheKey, x, y, baseZ, staticZ );

				// Add to vector if inserted successfully
				if( inserted )
				{
					validLandPos.emplace_back( x, y, baseZ, staticZ );
				}
				return true;
			}
		}
	}

	// FALLBACK
	// If we haven't found a valid location in all our allowed attempts, pick a random location from the cached ones
	if( !validLandPos.empty() )
	{
		if( TryRandomStoredLocation( validLandPos, x, y, z, z2 ) )
		{
			// Combine baseZ (z) and staticZ (z2) to get the best starting Z
			if( z2 != ILLEGAL_Z && z2 > z )
			{
				z = z2;
			}

			// Run dynamic elevation check
			const SI08 dynZ = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ, true, false );
			if( ILLEGAL_Z != dynZ && dynZ > z )
			{
				z = dynZ;
			}

			// Don't spawn item in location where same item has already spawned before
			auto itemAtXYZ = GetItemAtXYZ( x, y, z, worldNumber, instanceId );
			if( !( ValidateObject( itemAtXYZ ) && itemAtXYZ->GetSectionId() == idToSpawn ) )
			{
				// Validate spawn location
				if( z != ILLEGAL_Z && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, true, true ) && ( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId )))
				{
					return true;
				}
			}
		}
	}

	return false;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::CheckSpawned()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an item/npc should be removed from spawnlist and saved
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::CheckSpawned( void )
{
	for( CChar *cCheck = spawnedChars.First(); !spawnedChars.Finished(); cCheck = spawnedChars.Next() )
	{
		if( ValidateObject( cCheck ))
		{
			if( ValidateObject( cCheck->GetOwnerObj() ))
			{
				cCheck->ShouldSave( true );
				spawnedChars.Remove( cCheck );
			}
		}
		else
		{
			Console.Warning( "Invalid Object found in CSpawnRegion character list, AutoCorrecting." );
			spawnedChars.Remove( cCheck );
		}
	}

	for( CItem *iCheck = spawnedItems.First(); !spawnedItems.Finished(); iCheck = spawnedItems.Next() )
	{
		if( ValidateObject( iCheck ))
		{
			if( ValidateObject( iCheck->GetCont() ))
			{
				iCheck->ShouldSave( true );
				spawnedItems.Remove( iCheck );
			}
		}
		else
		{
			Console.Warning( "Invalid Object found in CSpawnRegion item list, AutoCorrecting." );
			spawnedItems.Remove( iCheck );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::DeleteSpawnedChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete an npc from spawnlist
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::DeleteSpawnedChar( CChar *toDelete )
{
	if( spawnedChars.Remove( toDelete ))
	{
		IncCurrentCharAmt( -1 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::DeleteSpawnedItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete an item from spawnlist
//o------------------------------------------------------------------------------------------------o
void CSpawnRegion::DeleteSpawnedItem( CItem *toDelete )
{
	if( spawnedItems.Remove( toDelete ))
	{
		IncCurrentItemAmt( -1 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetSpawnedItemsList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of spawned Items for spawnregion
//o------------------------------------------------------------------------------------------------o
GenericList<CItem *> * CSpawnRegion::GetSpawnedItemsList( void )
{
	return &spawnedItems;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::GetSpawnedCharsList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of spawned NPCs for spawnregion
//o------------------------------------------------------------------------------------------------o
GenericList<CChar *> * CSpawnRegion::GetSpawnedCharsList( void )
{
	return &spawnedChars;
}
