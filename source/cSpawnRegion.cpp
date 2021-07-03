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
const bool		DEFSPAWN_ONLYOUTSIDE	= false;

//o-----------------------------------------------------------------------------------------------o
//|	Class		-	CSpawnRegion()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for CSpawnRegion class
//o-----------------------------------------------------------------------------------------------o
CSpawnRegion::CSpawnRegion( UI16 spawnregion ) : regionnum( spawnregion ), maxcspawn( DEFSPAWN_MAXCSPAWN ), maxispawn( DEFSPAWN_MAXISPAWN ),
curcspawn( DEFSPAWN_CURCSPAWN ), curispawn( DEFSPAWN_CURISPAWN ), mintime( DEFSPAWN_MINTIME ), maxtime( DEFSPAWN_MAXTIME ),
nexttime( DEFSPAWN_NEXTTIME ), x1( DEFSPAWN_X1 ), x2( DEFSPAWN_X2 ), y1( DEFSPAWN_Y1 ), y2( DEFSPAWN_Y2 ),
call( DEFSPAWN_CALL ), worldNumber( DEFSPAWN_WORLDNUM ), prefZ( DEFSPAWN_PREFZ ), onlyOutside( DEFSPAWN_ONLYOUTSIDE )
{
	sItems.resize( 0 );
	sNpcs.resize( 0 );
	name = Dictionary->GetEntry( 1117 );
	// note: doesn't go here, but i'll see it here.  when an item is spawned, as soon as it's moved it needs to lose it's
	// spawn setting.  If not, then when people pick up spawned items, they will disappear (on region spawns)
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~CSpawnRegion()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for CSpawnRegion class
//o-----------------------------------------------------------------------------------------------o
CSpawnRegion::~CSpawnRegion()
{
	sItems.resize( 0 );
	sNpcs.resize( 0 );
	// Wipe out all items and npcs
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const std::string GetName( void ) const
//|					void SetName( const std::string& newName )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spawn region's name
//o-----------------------------------------------------------------------------------------------o
const std::string CSpawnRegion::GetName( void ) const
{
	return name;
}
void CSpawnRegion::SetName( const std::string& newName )
{
	name = newName;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 MaxSpawn( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of objects a region can spawn
//o-----------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxSpawn( void ) const
{
	return ( maxcspawn + maxispawn );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetMaxCharSpawn( void ) const
//|					void SetMaxCharSpawn( size_t newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of characters to spawn
//o-----------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxCharSpawn( void ) const
{
	return maxcspawn;
}
void CSpawnRegion::SetMaxCharSpawn( size_t newVal )
{
	maxcspawn = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetMaxItemSpawn( void ) const
//|					void SetMaxItemSpawn( size_t newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of items to spawn
//o-----------------------------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxItemSpawn( void ) const
{
	return maxispawn;
}
void CSpawnRegion::SetMaxItemSpawn( size_t newVal )
{
	maxispawn = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 Current( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Current amount of objects spawned
//o-----------------------------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrent( void ) const
{
	return ( curcspawn + curispawn );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetCurrentCharAmt( void ) const
//|					void IncCurrentCharAmt( SI16 incAmt )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current amount of characters spawned
//o-----------------------------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrentCharAmt( void ) const
{
	return curcspawn;
}
void CSpawnRegion::IncCurrentCharAmt( SI16 incAmt )
{
	curcspawn += incAmt;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetCurrentItemAmt( void ) const
//|					void IncCurrentItemAmt( SI16 incAmt )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets current amount of items spawned
//o-----------------------------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrentItemAmt( void ) const
{
	return curispawn;
}
void CSpawnRegion::IncCurrentItemAmt( SI16 incAmt )
{
	curispawn += incAmt;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetRegionNum( void ) const
//|					void SetRegionNum( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spawn region number
//o-----------------------------------------------------------------------------------------------o
UI16 CSpawnRegion::GetRegionNum( void ) const
{
	return regionnum;
}
void CSpawnRegion::SetRegionNum( UI16 newVal )
{
	regionnum = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetMinTime( void ) const
//|					void SetMinTime( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum amount of time to pass before a spawnregion
//|					spawns a new object
//o-----------------------------------------------------------------------------------------------o
UI08 CSpawnRegion::GetMinTime( void ) const
{
	return mintime;
}
void CSpawnRegion::SetMinTime( UI08 newVal )
{
	mintime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetMaxTime( void ) const
//|					void SetMaxTime( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of time to pass before a spawnregion
//|					spawns a new object
//o-----------------------------------------------------------------------------------------------o
UI08 CSpawnRegion::GetMaxTime( void ) const
{
	return maxtime;
}
void CSpawnRegion::SetMaxTime( UI08 newVal )
{
	maxtime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetNextTime( void ) const
//|					void SetNextTime( TIMERVAL newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for when a spawnregion will next spawn a new object
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CSpawnRegion::GetNextTime( void ) const
{
	return nexttime;
}
void CSpawnRegion::SetNextTime( TIMERVAL newVal )
{
	nexttime = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetPrefZ( SI08 newVal )
//|					SI08 GetPrefZ( void ) const
//|	Date		-	04/22/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z Level of the Spawn Region
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::SetPrefZ( SI08 newVal )
{
	prefZ = newVal;
}
SI08 CSpawnRegion::GetPrefZ( void ) const
{
	return prefZ;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetY1( void ) const
//|					void SetY1( SI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the y pos of the top corner of the spawnregion
//o-----------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetY1( void ) const
{
	return y1;
}
void CSpawnRegion::SetY1( SI16 newVal )
{
	y1 = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetX1( void ) const
//|					void SetX1( SI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the x pos of the top corner of the spawnregion
//o-----------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetX1( void ) const
{
	return x1;
}
void CSpawnRegion::SetX1( SI16 newVal )
{
	x1 = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetY2( void ) const
//|					void SetY2( SI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the y pos of the bottom corner of the spawn region
//o-----------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetY2( void ) const
{
	return y2;
}
void CSpawnRegion::SetY2( SI16 newVal )
{
	y2 = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetX2( void ) const
//|					void SetX2( SI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the x pos of the bottom corner of the spawn region
//o-----------------------------------------------------------------------------------------------o
SI16 CSpawnRegion::GetX2( void ) const
{
	return x2;
}
void CSpawnRegion::SetX2( SI16 newVal )
{
	x2 = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 WorldNumber( void ) const
//|					void WorldNumber( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the world number of the spawnregion
//o-----------------------------------------------------------------------------------------------o
UI08 CSpawnRegion::WorldNumber( void ) const
{
	return worldNumber;
}
void CSpawnRegion::WorldNumber( UI08 newVal )
{
	worldNumber = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetInstanceID( void ) const
//|					void SetInstanceID( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the instanceID of the spawnregion
//o-----------------------------------------------------------------------------------------------o
UI16 CSpawnRegion::GetInstanceID( void ) const
{
	return instanceID;
}
void CSpawnRegion::SetInstanceID( UI16 newVal )
{
	instanceID = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetOnlyOutside( void ) const
//|					void SetOnlyOutside( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs should only spawn outside buildings
//o-----------------------------------------------------------------------------------------------o
bool CSpawnRegion::GetOnlyOutside( void ) const
{
	return onlyOutside;
}
void CSpawnRegion::SetOnlyOutside( bool newVal )
{
	onlyOutside = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetCall( void ) const
//|					void SetCall( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets number of objects that should be spawned in each call for a spawnregion
//o-----------------------------------------------------------------------------------------------o
UI16 CSpawnRegion::GetCall( void ) const
{
	return call;
}
void CSpawnRegion::SetCall( UI16 newVal )
{
	call = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	STRINGLIST GetNPC( void ) const
//|					void SetNPC( STRINGLIST newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stringlist of individual NPCs to spawn in a spawnregion
//o-----------------------------------------------------------------------------------------------o
STRINGLIST CSpawnRegion::GetNPC( void ) const
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
		std::stringstream s_stream(newVal);
		while( s_stream.good() ) {
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	STRINGLIST GetItem( void ) const
//|					void SetItem( STRINGLIST newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stringlist of individual Items to spawn in a spawnregion
//o-----------------------------------------------------------------------------------------------o
STRINGLIST CSpawnRegion::GetItem( void ) const
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
		std::stringstream s_stream(newVal);
		while( s_stream.good() ) {
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadNPCList( const std::string &npcList )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads NPCLIST from DFNs with list of NPCs to spawn for a spawnregion
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::LoadNPCList( const std::string &npcList )
{
	std::string sect = "NPCLIST " + npcList;
	ScriptSection *CharList = FileLookup->FindEntry( sect, npc_def );
	if( CharList != nullptr )
	{
		for( std::string npc = CharList->First() ; !CharList->AtEnd(); npc = CharList->Next() )
		{
			if( strutil::toupper( npc ) == "NPCLIST" )
			{
				LoadNPCList( CharList->GrabData() );
			}
			else
			{
				sNpcs.push_back( npc );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadItemList( const std::string &itemList )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads ITEMLIST from DFNs with list of Items to spawn for a spawnregion
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::LoadItemList( const std::string &itemList )
{
	std::string sect = "ITEMLIST " + itemList;
	ScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList != nullptr )
	{
		for( std::string itm = ItemList->First() ; !ItemList->AtEnd(); itm = ItemList->Next() )
		{
			if( strutil::toupper( itm ) == "ITEMLIST" )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Load( ScriptSection *toScan )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the spawnregion from spawn.dfn script entry
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::Load( ScriptSection *toScan )
{
	std::string sect;
	std::string data;
	std::string UTag;

	for( std::string tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
	{
		if( !tag.empty() )
		{
			UTag = strutil::toupper( tag );
			data = toScan->GrabData();

			// Default to instanceID 0, in case nothing else is specified in DFN
			SetInstanceID( 0 );

			if( UTag == "NPCLIST" )
			{
				LoadNPCList( data );
			}
			else if( UTag == "NPC" )
			{
				sNpcs.push_back( data );
			}
			else if( UTag == "ITEMLIST" )
			{
				LoadItemList( data );
			}
			else if( UTag == "ITEM" )
			{
				sItems.push_back( data );
			}
			else if( UTag == "MAXITEMS" )
			{
				maxispawn = static_cast<UI32>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "MAXNPCS" )
			{
				maxcspawn = static_cast<UI32>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "X1" )
			{
				x1 = static_cast<SI16>(std::stoi(data, nullptr, 0));
			}
			else if( UTag == "X2" )
			{
				x2 = static_cast<SI16>(std::stoi(data, nullptr, 0));
			}
			else if( UTag == "Y1" )
			{
				y1 = static_cast<SI16>(std::stoi(data, nullptr, 0));
			}
			else if( UTag == "Y2" )
			{
				y2 = static_cast<SI16>(std::stoi(data, nullptr, 0));
			}
			else if( UTag == "MINTIME" )
			{
				mintime = static_cast<UI08>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "MAXTIME" )
			{
				maxtime = static_cast<UI08>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "NAME" )
			{
				name = data;
			}
			else if( UTag == "CALL" )
			{
				call = static_cast<UI16>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "WORLD" )
			{
				worldNumber = static_cast<UI16>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "INSTANCEID" )
			{
				instanceID = static_cast<UI16>(std::stoul(data, nullptr, 0));
			}
			else if( UTag == "PREFZ" )
			{
				prefZ = static_cast<SI08>(std::stoi(data, nullptr, 0));
			}
			else if( UTag == "ONLYOUTSIDE" )
			{
				onlyOutside = (static_cast<SI08>(std::stoi(data, nullptr, 0)) == 1);
			}
			else if( UTag == "VALIDLANDPOS" )
			{
				data = strutil::simplify( data );
				auto csecs = strutil::sections( data, "," );
				if( csecs.size() == 3 )
				{
					validLandPos.push_back( point3( static_cast<UI16>(std::stoul(strutil::stripTrim(csecs[0]),nullptr,0)), static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)), static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) ) );
					validLandPosCheck[ static_cast<UI16>(std::stoul(strutil::stripTrim(csecs[1]),nullptr,0)) + ( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) << 16 ) ] = static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0));
				}
			}
			else if( UTag == "VALIDWATERPOS" )
			{
				data = strutil::simplify( data );
				auto csecs = strutil::sections( data, "," );
				if( csecs.size() == 3 )
				{
					validWaterPos.push_back( point3( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)), static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)), static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) ) );
					validWaterPosCheck[ static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) + ( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) << 16 ) ] = static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0));
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doRegionSpawn( UI16& itemsSpawned, UI16& npcsSpawned )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do spawn stuff for spawnregion
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::doRegionSpawn( UI16& itemsSpawned, UI16& npcsSpawned )
{
	if( sNpcs.empty() )
		maxcspawn = 0;
	if( sItems.empty() )
		maxispawn = 0;

	bool shouldSpawnChars = ( !sNpcs.empty() && maxcspawn > spawnedChars.Num() );
	bool shouldSpawnItems = ( !sItems.empty() && maxispawn > spawnedItems.Num() );
	if( shouldSpawnChars || shouldSpawnItems )
	{
		CChar *spawnChar		= nullptr;
		CItem *spawnItem		= nullptr;
		const UI08 spawnChars	= (shouldSpawnChars?0:50);
		const UI08 spawnItems	= (shouldSpawnItems?100:49);
		for( UI16 i = 0; i < call && ( shouldSpawnItems || shouldSpawnChars ); ++i )
		{
			if( RandomNum( static_cast<std::uint16_t>(spawnChars), static_cast<std::uint16_t>(spawnItems) ) > 49 )
			{
				if( shouldSpawnItems )
				{
					spawnItem = RegionSpawnItem();
					if( ValidateObject( spawnItem ) )
					{
						spawnedItems.Add( spawnItem );
						++itemsSpawned;
						shouldSpawnItems = (spawnedItems.Num() < maxispawn);
					}
				}
			}
			else
			{
				if( shouldSpawnChars )
				{
					spawnChar = RegionSpawnChar();
					if( ValidateObject( spawnChar ) )
					{
						spawnedChars.Add( spawnChar );
						++npcsSpawned;
						shouldSpawnChars = (spawnedChars.Num() < maxcspawn);
					}
				}
			}
		}
	}

	SetNextTime( BuildTimeValue( (R32)( RandomNum( static_cast<std::uint16_t>(GetMinTime()*60), static_cast<std::uint16_t>(GetMaxTime()*60) ) ) ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar *RegionSpawnChar( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do a char spawn
//o-----------------------------------------------------------------------------------------------o
CChar *CSpawnRegion::RegionSpawnChar( void )
{
	std::string ourNPC = strutil::stripTrim( sNpcs[RandomNum( static_cast<size_t>( 0 ), sNpcs.size() - 1 )] );
	ScriptSection *npcCreate = FileLookup->FindEntry( ourNPC, npc_def );
	if( npcCreate == nullptr )
		return nullptr;

	std::string cdata;
	SI32 ndata = -1, odata = -1;
	UI16 npcID = 0;

	for( DFNTAGS tag = npcCreate->FirstTag(); !npcCreate->AtEndTags(); tag = npcCreate->NextTag() )
	{
		cdata = npcCreate->GrabData( ndata, odata );
		switch( tag )
		{
			case DFNTAG_ID:
				npcID = static_cast<UI16>( ndata );
				goto foundNpcID;
			default:
				break;
		}
	}
	foundNpcID:

	bool waterCreature = false;
	bool amphiCreature = false;
	if( npcID > 0 )
	{
		waterCreature = cwmWorldState->creatures[npcID].IsWater();
		amphiCreature = cwmWorldState->creatures[npcID].IsAmphibian();
	}

	SI16 x, y;
	SI08 z;
	if( FindCharSpotToSpawn( x, y, z, waterCreature, amphiCreature ) )
	{
		CChar *CSpawn = nullptr;
		CSpawn = Npcs->CreateBaseNPC( ourNPC );

		if( CSpawn != nullptr )
		{
			// NPCs should always wander the whole spawnregion
			CSpawn->SetNpcWander( WT_BOX );
			CSpawn->SetFx( x1, 0 );
			CSpawn->SetFx( x2, 1 );
			CSpawn->SetFy( y1, 0 );
			CSpawn->SetFy( y2, 1 );
			CSpawn->SetLocation( x, y, z, worldNumber, instanceID );
			CSpawn->SetSpawned( true );
			CSpawn->ShouldSave( false );
			CSpawn->SetSpawn( static_cast<UI32>( regionnum ) );
			Npcs->PostSpawnUpdate( CSpawn );
			IncCurrentCharAmt();
			return CSpawn;
		}
	}
	else
	{
		Console.warning( strutil::format("Unable to find valid location to spawn NPC in region %i", this->GetRegionNum() ));
	}
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *RegionSpawnItem( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Do an item spawn
//o-----------------------------------------------------------------------------------------------o
CItem *CSpawnRegion::RegionSpawnItem( void )
{
	CItem *ISpawn = nullptr;
	SI16 x, y;
	SI08 z;
	if( FindItemSpotToSpawn( x, y, z ) )
	{
		ISpawn = Items->CreateBaseScriptItem( sItems[RandomNum( static_cast< size_t >(0), sItems.size() - 1 )], worldNumber, 1 );
		if( ISpawn != nullptr )
		{
			ISpawn->SetLocation( x, y, z );
			ISpawn->SetSpawn( static_cast<UI32>(regionnum) );
			ISpawn->SetSpawned( true );
			ISpawn->ShouldSave( false );
			IncCurrentItemAmt();
		}
	}
	return ISpawn;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FindCharSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, bool &waterCreature, bool &amphiCreature )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o-----------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindCharSpotToSpawn( SI16 &x, SI16 &y, SI08 &z, bool &waterCreature, bool &amphiCreature )
{
	bool rvalue = false;
	point3 currLoc;
	std::map<UI32, SI08>::const_iterator checkValid;
	SI08 z2 = ILLEGAL_Z;
	const size_t landPosSize = validLandPos.size();
	const size_t waterPosSize = validWaterPos.size();

	// By default, let's attempt - at most - this many times to find a valid spawn point for the NPC
	UI08 maxSpawnAttempts = 100;

	// However, if we have found some valid spawn points already, reduce the amount of attempts to find MORE
	// valid spawn points, and increase the chance of using those valid spots instead! Always make some attempts to
	// find new spots, though
	if( !waterCreature && landPosSize > 0 ) // land creature
		maxSpawnAttempts = std::max( static_cast<SI16>( 25 ), static_cast<SI16>( maxSpawnAttempts - landPosSize ) );
	else if(( waterCreature || amphiCreature ) && waterPosSize > 0 )  // water or amphibian creature
		maxSpawnAttempts = std::max( static_cast<SI16>( 25 ), static_cast<SI16>( maxSpawnAttempts - waterPosSize ) );

	for( UI08 a = 0; a < maxSpawnAttempts; ++a )
	{
		x = RandomNum( x1, x2 );
		y = RandomNum( y1, y2 );
		z = Map->MapElevation( x, y, worldNumber );

		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, prefZ, instanceID );
		if( ILLEGAL_Z != dynz )
			z = dynz;
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
				z = staticz;
		}

		// Continue with the next spawn attempt if z is illegal
		if( z == ILLEGAL_Z )
			continue;

		// First go through the lists of already valid spawn locations on land,
		// and see if the chosen location has already been validated
		if( !waterCreature )
		{
			if( !validLandPosCheck.empty() && landPosSize > 0 )
			{
				checkValid = validLandPosCheck.find( y + (x<<16));
				if( checkValid != validLandPosCheck.end() )
					z2 = (*checkValid).second;

				if(z2 == z && z != ILLEGAL_Z)
				{
					rvalue = true;
					break;
				}
			}
		}

		// No luck, so let's try the list of already valid water tiles instead (if NPC can move on water)
		if( waterCreature || amphiCreature )
		{
			if( !validWaterPosCheck.empty() && waterPosSize > 0 )
			{
				checkValid = validWaterPosCheck.find( y + (x<<16));
				if( checkValid != validWaterPosCheck.end() )
					z2 = (*checkValid).second;

				if(z2 == z && z != ILLEGAL_Z)
				{
					rvalue = true;
					break;
				}
			}
		}

		// Since our chosen location has not already been validated, lets validate it with a land-based creature in mind
		if( !waterCreature && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceID ))
		{
			if( onlyOutside == false || !Map->inBuilding( x, y, z, worldNumber, instanceID ) )
			{
				if( z != ILLEGAL_Z )
				{
					rvalue = true;
					validLandPos.push_back( point3( x, y, z ) );
					validLandPosCheck[ y + ( x << 16) ] = z;
					break;
				}
			}
		}

		// Otherwise, validate it with a water-based creature in mind instead
		if(( waterCreature || amphiCreature ) && Map->ValidSpawnLocation( x, y, z, worldNumber, instanceID, false ))
		{
			if( onlyOutside == false || !Map->inBuilding( x, y, z, worldNumber, instanceID ) )
			{
				rvalue = true;
				validWaterPos.push_back( point3( x, y, z ) );
				validWaterPosCheck[ y + ( x << 16) ] = z;
				break;
			}
		}
	}

	// If we haven't found a valid location pick a random location from the stored ones
	if( !rvalue && !waterCreature && !validLandPos.empty() && landPosSize > 0)
	{
		currLoc = validLandPos[RandomNum( static_cast<size_t>(0), (landPosSize-1) )];
		x = static_cast<SI16>(currLoc.x);
		y = static_cast<SI16>(currLoc.y);
		z = static_cast<SI08>(currLoc.z);
		// Recalculate the z coordinate to see whether something has changed
		z2 = Map->MapElevation( x, y, worldNumber );
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, prefZ, instanceID );
		if( ILLEGAL_Z != dynz )
			z2 = dynz;
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
				z2 = staticz;
		}

		if(z2 == z)
			rvalue = true;
	}
	else if( !rvalue && ( waterCreature || amphiCreature) && !validWaterPos.empty() && waterPosSize > 0 )
	{
		currLoc = validWaterPos[RandomNum( static_cast<size_t>(0), (waterPosSize-1) )];
		x = static_cast<SI16>(currLoc.x);
		y = static_cast<SI16>(currLoc.y);
		z = static_cast<SI08>(currLoc.z);
		// Recalculate the z coordinate to see whether something has changed
		z2 = Map->MapElevation( x, y, worldNumber );
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, prefZ, instanceID );
		if( ILLEGAL_Z != dynz )
			z2 = dynz;
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
				z2 = staticz;
		}

		if(z2 == z)
			rvalue = true;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FindItemSpotToSpawn( SI16 &x, SI16 &y, SI08 &z )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o-----------------------------------------------------------------------------------------------o
bool CSpawnRegion::FindItemSpotToSpawn( SI16 &x, SI16 &y, SI08 &z )
{
	bool rvalue = false;
	point3 currLoc;
	SI08 z2 = ILLEGAL_Z;
	std::map<UI32, SI08>::const_iterator checkValid;
	const size_t landPosSize = validLandPos.size();

	// By default, let's try - at most - this many times to find a valid spawn point for the item
	UI08 maxSpawnAttempts = 100;

	// However, if we have found some valid spawn points already, reduce the amount of attempts to find MORE
	// valid spawn points, and increase the chance of using those valid spots instead!
	if( landPosSize > 0 )
		maxSpawnAttempts = std::max( static_cast<SI16>( 25 ), static_cast<SI16>( maxSpawnAttempts - landPosSize ) );

	for( UI08 a = 0; a < maxSpawnAttempts; ++a )
	{
		x = RandomNum( x1, x2 );
		y = RandomNum( y1, y2 );
		z = Map->MapElevation( x, y, worldNumber );

		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, prefZ, instanceID );
		if( ILLEGAL_Z != dynz )
			z = dynz;
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
				z = staticz;
		}

		// First go through the lists of already stored good locations
		if( !validLandPosCheck.empty() && landPosSize > 0 )
		{
			checkValid = validLandPosCheck.find( y + (x<<16));
			if( checkValid != validLandPosCheck.end() )
				z2 = (*checkValid).second;

			if(z2 == z && z != ILLEGAL_Z )
			{
				rvalue = true;
				break;
			}
		}

		if( Map->ValidSpawnLocation( x, y, z, worldNumber, instanceID ) )
		{
			if( onlyOutside == false || !Map->inBuilding( x, y, z, worldNumber, instanceID ) )
			{
				rvalue = true;
				validLandPos.push_back( point3( x, y, z ) );
				validLandPosCheck[ y + ( x << 16) ] = z;
				break;
			}
		}
	}

	// If we haven't found a valid location pick a random location from the stored ones
	if( !rvalue && !validLandPos.empty() && landPosSize > 0)
	{
		currLoc = validLandPos[RandomNum( static_cast<size_t>(0), (landPosSize-1) )];
		x = static_cast<SI16>(currLoc.x);
		y = static_cast<SI16>(currLoc.y);
		z = static_cast<SI08>(currLoc.z);
		// Recalculate the z coordinate to see whether something has changed
		z2 = Map->MapElevation( x, y, worldNumber );
		const SI08 dynz = Map->DynamicElevation( x, y, z, worldNumber, prefZ, instanceID );
		if( ILLEGAL_Z != dynz )
			z2 = dynz;
		else
		{
			const SI08 staticz = Map->StaticTop( x, y, z, worldNumber, prefZ );
			if( ILLEGAL_Z != staticz )
				z2 = staticz;
		}

		if(z2 == z)
			rvalue = true;
	}

	return rvalue;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checkSpawned( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if an item/npc should be removed from spawnlist and saved
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::checkSpawned( void )
{
	for( CChar *cCheck = spawnedChars.First(); !spawnedChars.Finished(); cCheck = spawnedChars.Next() )
	{
		if( ValidateObject( cCheck ) )
		{
			if( ValidateObject( cCheck->GetOwnerObj() ) )
			{
				cCheck->ShouldSave( true );
				spawnedChars.Remove( cCheck );
			}
		}
		else
		{
			Console.warning( "Invalid Object found in CSpawnRegion character list, AutoCorrecting." );
			spawnedChars.Remove( cCheck );
		}
	}

	for( CItem *iCheck = spawnedItems.First(); !spawnedItems.Finished(); iCheck = spawnedItems.Next() )
	{
		if( ValidateObject( iCheck ) )
		{
			if( ValidateObject( iCheck->GetCont() ) )
			{
				iCheck->ShouldSave( true );
				spawnedItems.Remove( iCheck );
			}
		}
		else
		{
			Console.warning( "Invalid Object found in CSpawnRegion item list, AutoCorrecting." );
			spawnedItems.Remove( iCheck );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void deleteSpawnedChar( CChar *toDelete )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete an npc from spawnlist
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::deleteSpawnedChar( CChar *toDelete )
{
	if( spawnedChars.Remove( toDelete ) )
		IncCurrentCharAmt( -1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void deleteSpawnedItem( CItem *toDelete )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete an item from spawnlist
//o-----------------------------------------------------------------------------------------------o
void CSpawnRegion::deleteSpawnedItem( CItem *toDelete )
{
	if( spawnedItems.Remove( toDelete ) )
		IncCurrentItemAmt( -1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	GenericList< CItem * > * GetSpawnedItemsList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of spawned Items for spawnregion
//o-----------------------------------------------------------------------------------------------o
GenericList< CItem * > * CSpawnRegion::GetSpawnedItemsList( void )
{
	return &spawnedItems;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	GenericList< CChar * > * GetSpawnedCharsList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of spawned NPCs for spawnregion
//o-----------------------------------------------------------------------------------------------o
GenericList< CChar * > * CSpawnRegion::GetSpawnedCharsList( void )
{
	return &spawnedChars;
}
