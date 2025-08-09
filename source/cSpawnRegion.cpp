#include "uox3.h"
#include "cSpawnRegion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "mapstuff.h"
#include "classes.h"
#include "Dictionary.h"
#include "StringUtility.hpp"
#include <sstream>
#include <iostream>
#include <regex>

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
							excludeArea.x1 = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
							excludeArea.y1 = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
							excludeArea.x2 = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
							excludeArea.y2 = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 ));
							exclusionAreas.push_back( excludeArea );
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
							validLandPos.push_back( Point3_st( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0)), static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//") ), nullptr, 0 )), static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ))));
							validLandPosCheck[ static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )) + ( static_cast<UI16>(std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )) << 16 ) ] = static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
						}
					}
					else if( UTag == "VALIDWATERPOS" )
					{
						data = oldstrutil::simplify( data );
						auto csecs = oldstrutil::sections( data, "," );
						if( csecs.size() == 3 )
						{
							validWaterPos.push_back( Point3_st( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )), static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )), static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ))));
							validWaterPosCheck[ static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )) + ( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )) << 16 ) ] = static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 ));
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
						spawnedItems.Add( spawnItem );
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
						spawnedChars.Add( spawnChar );
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
//|	Function	-	CSpawnRegion::RegionSpawnChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do a char spawn
//o------------------------------------------------------------------------------------------------o
auto CSpawnRegion::RegionSpawnChar() -> CChar *
{	
	// Stuff each NPC entry into a vector
	std::vector<std::pair<std::string, UI16>> npcListVector;
	for( size_t i = 0; i < sNpcs.size(); i++ )
	{
		// Split string for entry into a stringlist based on | as separator
		auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( sNpcs[i], "//" )), "|" );

		UI16 sectionWeight = 1;
		if( csecs.size() > 1 )
		{
			sectionWeight = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
		}

		auto npcSection = ( csecs.size() > 1 ? csecs[1] : csecs[0] );
		npcListVector.emplace_back( npcSection, sectionWeight );
	}

	auto ourNPC = Npcs->ChooseNpcToCreate( npcListVector );
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

	std::string cdata;
	SI32 ndata = -1, odata = -1;
	UI16 npcId = 0;

	for( DFNTAGS tag = npcCreate->FirstTag(); !npcCreate->AtEndTags(); tag = npcCreate->NextTag() )
	{
		cdata = npcCreate->GrabData( ndata, odata );
		switch( tag )
		{
			case DFNTAG_ID:
				npcId = oldstrutil::value<UI16>( cdata ); // static_cast<UI16>( ndata );
				goto foundNpcId;
			default:
				break;
		}
	}
	foundNpcId:

	bool waterCreature = false;
	bool amphiCreature = false;
	if( npcId > 0 )
	{
		waterCreature = cwmWorldState->creatures[npcId].IsWater();
		amphiCreature = cwmWorldState->creatures[npcId].IsAmphibian();
	}

	SI16 x, y;
	SI08 z;
	if( FindCharSpotToSpawn( x, y, z, waterCreature, amphiCreature ))
	{
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
	auto idToSpawn = sItems[RandomNum( static_cast<size_t>( 0 ), sItems.size() - 1 )];
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
		}
	}
	return ISpawn;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::FindCharSpotToSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindCharSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, bool &waterCreature, bool &amphiCreature )
{
	bool rValue = false;
	Point3_st currLoc;
	//std::map<UI32, SI08>::const_iterator checkValid;
	SI08 z2 = ILLEGAL_Z;
	const size_t landPosSize = validLandPos.size();
	const size_t waterPosSize = validWaterPos.size();

	// By default, let's attempt - at most - this many times to find a valid spawn point for the NPC
	UI08 maxSpawnAttempts = 100;

	// However, if we have found some valid spawn points already, reduce the amount of attempts to find MORE
	// valid spawn points, and increase the chance of using those valid spots instead! Always make some attempts to
	// find new spots, though
	if( !waterCreature && landPosSize > 0 ) // land creature
	{
		maxSpawnAttempts = std::max( static_cast<SI08>( 25 ), static_cast<SI08>( maxSpawnAttempts - landPosSize ));
	}
	else if(( waterCreature || amphiCreature ) && waterPosSize > 0 )  // water or amphibian creature
	{
		maxSpawnAttempts = std::max( static_cast<SI08>( 25 ), static_cast<SI08>( maxSpawnAttempts - waterPosSize ));
	}

	for( UI08 a = 0; a < maxSpawnAttempts; ++a )
	{
		x = RandomNum( x1, x2 );
		y = RandomNum( y1, y2 );
		z = Map->MapElevation( x, y, worldNumber );

		if( defZ != ILLEGAL_Z )
		{
			// Use the definite Z level defined in the spawn region
			// Trust that the spawnregion scripter knows what they are doing...
			z = defZ;
		}
		else
		{
			// No definite Z has been defined, look for valid dynamic Z based on prefZ
			const SI08 dynZ = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ );
			if( ILLEGAL_Z != dynZ )
			{
				z = dynZ;
			}

			// Even if we got a valid dynamic Z, there might be a better match with statics, based on prefZ
			const SI08 staticZ = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticZ && staticZ > z )
			{
				z = staticZ;
			}
		}

		// Continue with the next spawn attempt if z is illegal
		if( z == ILLEGAL_Z )
			continue;


		if( forceSpawn )
		{
			// Don't care about validation, force spawn at own risk!
			return true;
		}

		// First go through the lists of already valid spawn locations on land,
		// and see if the chosen location has already been validated
		if( !waterCreature )
		{
			if( !validLandPosCheck.empty() && landPosSize > 0 )
			{
				auto checkValid = validLandPosCheck.find( y + ( x << 16 ));
				if( checkValid != validLandPosCheck.end() )
				{
					z2 = ( *checkValid ).second;
				}

				if( z2 == z && z != ILLEGAL_Z )
				{
					rValue = true;
					break;
				}
			}
		}

		// No luck, so let's try the list of already valid water tiles instead (if NPC can move on water)
		if( waterCreature || amphiCreature )
		{
			if( !validWaterPosCheck.empty() && waterPosSize > 0 )
			{
				auto checkValid = validWaterPosCheck.find( y + ( x << 16 ));
				if( checkValid != validWaterPosCheck.end() )
				{
					z2 = ( *checkValid ).second;
				}

				if( z2 == z && z != ILLEGAL_Z )
				{
					rValue = true;
					break;
				}
			}
		}

		// Make sure the chosen spawn location is not in an exclusion area
		auto illegalLoc = false;
		if( !rValue )
		{
			for( auto &exclusionArea : exclusionAreas )
			{
				if( x >= exclusionArea.x1 && x <= exclusionArea.x2 && y >= exclusionArea.y1 && y <= exclusionArea.y2 )
				{
					// Chosen location is in an exclusion area. Disallow!
					illegalLoc = true;
					break;
				}
			}
		}

		// Since our chosen location has not already been validated, lets validate it with a land-based creature in mind
		if( !illegalLoc && !waterCreature && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId ))
		{
			if( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId ))
			{
				if( z != ILLEGAL_Z )
				{
					rValue = true;
					validLandPos.push_back( Point3_st( x, y, z ));
					validLandPosCheck[ y + ( x << 16) ] = z;
					break;
				}
			}
		}

		// Otherwise, validate it with a water-based creature in mind instead
		if( !illegalLoc && ( waterCreature || amphiCreature ) && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId, false ))
		{
			if( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId ))
			{
				rValue = true;
				validWaterPos.push_back( Point3_st( x, y, z ));
				validWaterPosCheck[ y + ( x << 16) ] = z;
				break;
			}
		}
	}

	// If we haven't found a valid location pick a random location from the stored ones
	if( !rValue && !waterCreature && !validLandPos.empty() && landPosSize > 0 )
	{
		currLoc = validLandPos[RandomNum( static_cast<size_t>( 0 ), ( landPosSize - 1 ))];
		x = static_cast<SI16>( currLoc.x );
		y = static_cast<SI16>( currLoc.y );
		z = static_cast<SI08>( currLoc.z );
		// Recalculate the z coordinate to see whether something has changed
		z2 = Map->MapElevation( x, y, worldNumber );
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ );
		if( ILLEGAL_Z != dynz )
		{
			z2 = dynz;
		}
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
			{
				z2 = staticz;
			}
		}

		if( z2 == z )
		{
			rValue = true;
		}
	}
	else if( !rValue && ( waterCreature || amphiCreature) && !validWaterPos.empty() && waterPosSize > 0 )
	{
		currLoc = validWaterPos[RandomNum( static_cast<size_t>( 0 ), ( waterPosSize - 1 ))];
		x = static_cast<SI16>( currLoc.x );
		y = static_cast<SI16>( currLoc.y );
		z = static_cast<SI08>( currLoc.z );
		// Recalculate the z coordinate to see whether something has changed
		z2 = Map->MapElevation( x, y, worldNumber );
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ );
		if( ILLEGAL_Z != dynz )
		{
			z2 = dynz;
		}
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
			{
				z2 = staticz;
			}
		}

		if( z2 == z )
		{
			rValue = true;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnRegion::FindItemSpotToSpawn()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o------------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindItemSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, std::string idToSpawn )
{
	bool rValue = false;
	Point3_st currLoc;
	SI08 z2 = ILLEGAL_Z;
	std::map<UI32, SI08>::const_iterator checkValid;
	const size_t landPosSize = validLandPos.size();

	// By default, let's try - at most - this many times to find a valid spawn point for the item
	UI08 maxSpawnAttempts = 100;

	// However, if we have found some valid spawn points already, reduce the amount of attempts to find MORE
	// valid spawn points, and increase the chance of using those valid spots instead!
	if( landPosSize > 0 )
	{
		maxSpawnAttempts = std::max( static_cast<SI16>( 25 ), static_cast<SI16>( maxSpawnAttempts - landPosSize ));
	}

	for( UI08 a = 0; a < maxSpawnAttempts; ++a )
	{
		x = RandomNum( x1, x2 );
		y = RandomNum( y1, y2 );
		z = Map->MapElevation( x, y, worldNumber );

		if( defZ != ILLEGAL_Z )
		{
			// Use the definite Z level defined in the spawn region
			// Trust that the spawnregion scripter knows what they are doing...
			z = defZ;
		}
		else
		{
			// No definite Z has been defined, look for valid dynamic Z based on prefZ
			const SI08 dynZ = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ );
			if( ILLEGAL_Z != dynZ )
			{
				z = dynZ;
			}

			// Even if we got a valid dynamic Z, there might be a better match with statics, based on prefZ
			const SI08 staticZ = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticZ && staticZ > z )
			{
				z = staticZ;
			}
		}

		// First go through the lists of already stored good locations
		if( !validLandPosCheck.empty() && landPosSize > 0 )
		{
			auto checkValid = validLandPosCheck.find( y + ( x << 16 ));
			if( checkValid != validLandPosCheck.end() )
			{
				z2 = (*checkValid).second;
			}

			if( z2 == z && z != ILLEGAL_Z )
			{
				rValue = true;
				//break;
			}
		}

		// Make sure the chosen spawn location is not in an exclusion area
		auto illegalLoc = false;
		if( !rValue )
		{
			for( auto &exclusionArea : exclusionAreas )
			{
				if( x >= exclusionArea.x1 && x <= exclusionArea.x2 && y >= exclusionArea.y1 && y <= exclusionArea.y2 )
				{
					// Chosen location is in an exclusion area. Disallow!
					illegalLoc = true;
					break;
				}
			}
		}
		
		// Don't spawn an item in a location where same item has already spawned before
		auto itemAtXYZ = GetItemAtXYZ( x, y, z, worldNumber, instanceId );
		if( ValidateObject( itemAtXYZ ) && itemAtXYZ->GetSectionId() == idToSpawn )
		{
			illegalLoc = true;
			rValue = false;
			break;
		}

		if( !illegalLoc && !rValue && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceId ))
		{
			if( onlyOutside == false || !Map->InBuilding( x, y, z, worldNumber, instanceId ))
			{
				rValue = true;
				validLandPos.push_back( Point3_st( x, y, z ));
				validLandPosCheck[ y + ( x << 16) ] = z;
				break;
			}
		}
	}

	// If we haven't found a valid location pick a random location from the stored ones
	if( !rValue && !validLandPos.empty() && landPosSize > 0 )
	{
		currLoc = validLandPos[RandomNum( static_cast<size_t>( 0 ), ( landPosSize - 1 ))];
		x = static_cast<SI16>( currLoc.x );
		y = static_cast<SI16>( currLoc.y );
		z = static_cast<SI08>( currLoc.z );
		// Recalculate the z coordinate to see whether something has changed
		z2 = Map->MapElevation( x, y, worldNumber );
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, instanceId, prefZ );
		if( ILLEGAL_Z != dynz )
		{
			z2 = dynz;
		}
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
			{
				z2 = staticz;
			}
		}

		if( z2 == z )
		{
			// Nevertheless, don't spawn an item in a location where same item has already spawned before
			auto itemAtXYZ = GetItemAtXYZ( x, y, z, worldNumber, instanceId );
			if( ValidateObject( itemAtXYZ ) && itemAtXYZ->GetSectionId() == idToSpawn )
			{
				rValue = false;
			}
			else
			{
				rValue = true;
			}
		}
	}

	return rValue;
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
