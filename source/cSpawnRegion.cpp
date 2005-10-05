#include "uox3.h"
#include "cSpawnRegion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "mapstuff.h"
#include "classes.h"
#include "Dictionary.h"

namespace UOX
{

const TIMERVAL	DEFSPAWN_NEXTTIME		= 0;
const UI16		DEFSPAWN_CALL			= 1;
const SI16		DEFSPAWN_X1				= 0;
const SI16		DEFSPAWN_X2				= 0;
const SI16		DEFSPAWN_Y1				= 0;
const SI16		DEFSPAWN_Y2				= 0;
const SI08		DEFSPAWN_Z				= 0;
const size_t	DEFSPAWN_MAXCSPAWN		= 0;
const size_t	DEFSPAWN_MAXISPAWN		= 0;
const UI08		DEFSPAWN_MAXTIME		= 0;
const UI08		DEFSPAWN_MINTIME		= 0;
const SI32		DEFSPAWN_CURCSPAWN		= 0;
const SI32		DEFSPAWN_CURISPAWN		= 0;
const UI08		DEFSPAWN_WORLDNUM		= 0;

CSpawnRegion::CSpawnRegion( UI16 spawnregion ) : regionnum( spawnregion ), maxcspawn( DEFSPAWN_MAXCSPAWN ), maxispawn( DEFSPAWN_MAXISPAWN ), 
curcspawn( DEFSPAWN_CURCSPAWN ), curispawn( DEFSPAWN_CURISPAWN ), mintime( DEFSPAWN_MINTIME ), maxtime( DEFSPAWN_MAXTIME ), 
nexttime( DEFSPAWN_NEXTTIME ), x1( DEFSPAWN_X1 ), x2( DEFSPAWN_X2 ), y1( DEFSPAWN_Y1 ), y2( DEFSPAWN_Y2 ), z( DEFSPAWN_Z ), 
call( DEFSPAWN_CALL ), worldNumber( DEFSPAWN_WORLDNUM )
{
	sItems.resize( 0 );
	sNpcs.resize( 0 );
	name = Dictionary->GetEntry( 1117 );
// note: doesn't go here, but i'll see it here.  when an item is spawned, as soon as it's moved it needs to lose it's
// spawn setting.  If not, then when people pick up spawned items, they will disappear (on region spawns)
}

CSpawnRegion::~CSpawnRegion()
{
	sItems.resize( 0 );
	sNpcs.resize( 0 );
// Wipe out all items and npcs
}

//o---------------------------------------------------------------------------o
//|	Function	-	std::string Name( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn Region's name
//o---------------------------------------------------------------------------o
const std::string CSpawnRegion::GetName( void ) const
{
	return name;
}

void CSpawnRegion::SetName( const std::string newName )
{
	name = newName;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 MaxSpawn( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of objects a region can spawn
//o---------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxSpawn( void ) const
{
	return ( maxcspawn + maxispawn );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 MaxCharSpawn( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of characters to spawn
//o---------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxCharSpawn( void ) const
{
	return maxcspawn;
}
void CSpawnRegion::SetMaxCharSpawn( size_t newVal )
{
	maxcspawn = newVal;
}


//o---------------------------------------------------------------------------o
//|	Function	-	SI32 MaxItemSpawn( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of items to spawn
//o---------------------------------------------------------------------------o
size_t CSpawnRegion::GetMaxItemSpawn( void ) const
{
	return maxispawn;
}

void CSpawnRegion::SetMaxItemSpawn( size_t newVal )
{
	maxispawn = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 Current( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Current amount of objects spawned
//o---------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrent( void ) const
{
	return ( curcspawn + curispawn );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 CurrentCharAmt( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Current amount of characters spawned
//o---------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrentCharAmt( void ) const
{
	return curcspawn;
}

void CSpawnRegion::IncCurrentCharAmt( SI16 incAmt )
{
	curcspawn += incAmt;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 CurrentItemAmt( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Current amount of items spawned
//o---------------------------------------------------------------------------o
SI32 CSpawnRegion::GetCurrentItemAmt( void ) const
{
	return curispawn;
}
void CSpawnRegion::IncCurrentItemAmt( SI16 incAmt )
{
	curispawn += incAmt;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 RegionNum( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn region number
//o---------------------------------------------------------------------------o
UI16 CSpawnRegion::GetRegionNum( void ) const
{
	return regionnum;
}

void CSpawnRegion::SetRegionNum( UI16 newVal )
{
	regionnum = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 MinTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Minimum amount of time to pass before a spawnregion
//|					spawns a new object
//o---------------------------------------------------------------------------o
UI08 CSpawnRegion::GetMinTime( void ) const
{
	return mintime;
}

void CSpawnRegion::SetMinTime( UI08 newVal )
{
	mintime = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 MaxTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Maximum amount of time to pass before a spawnregion
//|					spawns a new object
//o---------------------------------------------------------------------------o
UI08 CSpawnRegion::GetMaxTime( void ) const
{
	return maxtime;
}
void CSpawnRegion::SetMaxTime( UI08 newVal )
{
	maxtime = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 NextTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	When a spawnregion will next spawn a new object
//o---------------------------------------------------------------------------o
TIMERVAL CSpawnRegion::GetNextTime( void ) const
{
	return nexttime;
}
void CSpawnRegion::SetNextTime( TIMERVAL newVal )
{
	nexttime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	SI08 Z()
//|	Date			-	04/22/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Z Level of the Spawn Region
//o--------------------------------------------------------------------------o
void CSpawnRegion::SetZ( SI08 newVal )
{
	z = newVal;
}
SI08 CSpawnRegion::GetZ( void ) const
{
	return z;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 Y1()
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	The y pos of the top corner of the spawnregion
//o---------------------------------------------------------------------------o
SI16 CSpawnRegion::GetY1( void ) const
{
	return y1;
}
void CSpawnRegion::SetY1( SI16 newVal )
{
	y1 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 X1()
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	The x pos of the top corner of the spawnregion
//o---------------------------------------------------------------------------o
SI16 CSpawnRegion::GetX1( void ) const
{
	return x1;
}
void CSpawnRegion::SetX1( SI16 newVal )
{
	x1 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 Y2()
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	The y pos of the bottom corner of the spawn region
//o---------------------------------------------------------------------------o
SI16 CSpawnRegion::GetY2( void ) const
{
	return y2;
}
void CSpawnRegion::SetY2( SI16 newVal )
{
	y2 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 X2()
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	The x pos of the bottom corner of the spawn region
//o---------------------------------------------------------------------------o
SI16 CSpawnRegion::GetX2( void ) const
{
	return x2;
}
void CSpawnRegion::SetX2( SI16 newVal )
{
	x2 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 WorldNumber()
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	The world number of the spawnregion
//o---------------------------------------------------------------------------o
UI08 CSpawnRegion::WorldNumber( void ) const
{
	return worldNumber;
}
void CSpawnRegion::WorldNumber( UI08 newVal )
{
	worldNumber = newVal;
}

void CSpawnRegion::LoadNPCList( std::string npcList )
{
	UString sect = "NPCLIST " + npcList;
	ScriptSection *CharList = FileLookup->FindEntry( sect, npc_def );
	if( CharList != NULL )
	{
		for( UString npc = CharList->First() ; !CharList->AtEnd(); npc = CharList->Next() )
		{
			if( npc.upper() == "NPCLIST" )
				LoadNPCList( CharList->GrabData() );
			else
				sNpcs.push_back( npc );
		}
	}
}

void CSpawnRegion::LoadItemList( std::string itemList )
{
	UString sect = "ITEMLIST " + itemList;
	ScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList != NULL )
	{
		for( UString itm = ItemList->First() ; !ItemList->AtEnd(); itm = ItemList->Next() )
		{
			if( itm.upper() == "ITEMLIST" )
				LoadItemList( ItemList->GrabData() );
			else
				sItems.push_back( itm );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	Load( ScriptSection *toScan )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the spawnregion from spawn.scp script entry
//o---------------------------------------------------------------------------o
void CSpawnRegion::Load( ScriptSection *toScan )
{
	UString sect;
	UString data;
	UString UTag;

	for( UString tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
	{
		if( !tag.empty() )
		{
			UTag = tag.upper();
			data = toScan->GrabData();

			if( UTag == "NPCLIST" )
				LoadNPCList( data );
			else if( UTag == "NPC" )
				sNpcs.push_back( data );
			else if( UTag == "ITEMLIST" )
				LoadItemList( data );
			else if( UTag == "ITEM" )
				sItems.push_back( data );
			else if( UTag == "MAXITEMS" )
				maxispawn = data.toUInt();
			else if( UTag == "MAXNPCS" )
				maxcspawn = data.toUInt();
			else if( UTag == "X1" )
				x1 = data.toShort();
			else if( UTag == "X2" )
				x2 = data.toShort();
			else if( UTag == "Y1" )
				y1 = data.toShort();
			else if( UTag == "Y2" )
				y2 = data.toShort();
			else if( UTag == "MINTIME" )
				mintime = data.toUByte();
			else if( UTag == "MAXTIME" )
				maxtime = data.toUByte();
			else if( UTag == "NAME" )
				name = data;
			else if( UTag == "CALL" )
				call = data.toUShort();
			else if( UTag == "WORLD" )
				worldNumber = data.toUByte();
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	doRegionSpawn( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do spawn stuff for spawnregion
//o---------------------------------------------------------------------------o
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
		CChar *spawnChar		= NULL;
		CItem *spawnItem		= NULL;
		const UI08 spawnChars	= (shouldSpawnChars?0:50);
		const UI08 spawnItems	= (shouldSpawnItems?100:49);
		for( UI16 i = 0; i < call && ( shouldSpawnItems || shouldSpawnChars ); ++i )
		{
			if( RandomNum( spawnChars, spawnItems ) > 49 )
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
	SetNextTime( BuildTimeValue( (R32)( 60 * RandomNum( GetMinTime(), GetMaxTime() ) ) ) );
}

void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway );
//o---------------------------------------------------------------------------o
//|	Function	-	cChar *RegionSpawnChar( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do a char spawn
//o---------------------------------------------------------------------------o
CChar *CSpawnRegion::RegionSpawnChar( void )
{
	CChar *CSpawn = Npcs->CreateBaseNPC( sNpcs[RandomNum( static_cast< size_t >(0), sNpcs.size() - 1 )] );
	if( CSpawn != NULL )
	{
		if( FindSpotToSpawn( CSpawn ) )
		{
            CSpawn->SetSpawned( true );
			CSpawn->ShouldSave( false );
			CSpawn->SetSpawn( static_cast<UI32>(regionnum) );
			InitializeWanderArea( CSpawn, 10, 10 );
			Npcs->PostSpawnUpdate( CSpawn );
			IncCurrentCharAmt();
		}
		else
		{
			CSpawn->Delete();
			CSpawn = NULL;
		}
	}
	return CSpawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cItem *RegionSpawnItem( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do a item spawn
//o---------------------------------------------------------------------------o
CItem *CSpawnRegion::RegionSpawnItem( void )
{
	CItem *ISpawn = Items->CreateBaseScriptItem( sItems[RandomNum( static_cast< size_t >(0), sItems.size() - 1 )], worldNumber );
	if( ISpawn != NULL )
	{
		if( FindSpotToSpawn( ISpawn ) )
		{
			ISpawn->SetSpawn( static_cast<UI32>(regionnum) );
            ISpawn->SetSpawned( true );
			ISpawn->ShouldSave( false );
			IncCurrentItemAmt();
		}
		else
		{
			ISpawn->Delete();
			ISpawn = NULL;
		}
	}
	return ISpawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool FindSpotForItem( SI16 &x, SI16 &y, SI08 &z )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o---------------------------------------------------------------------------o
bool CSpawnRegion::FindSpotToSpawn( CBaseObject *mObj )
{
	SI16 x, y;
	SI08 z;
	bool rvalue = false;
	for( UI08 a = 0; a < 100; ++a ) 
	{
		x = RandomNum( x1, x2 );
		y = RandomNum( y1, y2 );
		z = Map->MapElevation( x, y, worldNumber );

		if( Map->CanMonsterMoveHere( x, y, z, worldNumber ) )
		{
			mObj->SetLocation( x, y, z );
			rvalue = true;
			break;
		}
	}
	// if( !rvalue )
	//Console << "Problem regionspawn [" << regionnum << "] found. Nothing will be spawned" << myendl;
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkSpawned()
//|	Programmer	-	sereg
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if an item/npc should be removed from spawnlist and saved
//o---------------------------------------------------------------------------o
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
			Console.Warning( 2, "Invalid Object found in CSpawnRegion character list, AutoCorrecting." );
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
			Console.Warning( 2, "Invalid Object found in CSpawnRegion item list, AutoCorrecting." );
			spawnedItems.Remove( iCheck );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deleteSpawnedChar( CChar *toDelete )
//|	Programmer	-	sereg
//o---------------------------------------------------------------------------o
//|	Purpose		-	delete a npc from spawnlist
//o---------------------------------------------------------------------------o
void CSpawnRegion::deleteSpawnedChar( CChar *toDelete )
{
	if( spawnedChars.Remove( toDelete ) )
		IncCurrentCharAmt( -1 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deleteSpawnedItem( CItem *toDelete )
//|	Programmer	-	sereg
//o---------------------------------------------------------------------------o
//|	Purpose		-	delete an item from spawnlist
//o---------------------------------------------------------------------------o
void CSpawnRegion::deleteSpawnedItem( CItem *toDelete )
{
	if( spawnedItems.Remove( toDelete ) )
		IncCurrentItemAmt( -1 );
}

CDataList< CItem * > * CSpawnRegion::GetSpawnedItemsList( void )
{
	return &spawnedItems;
}

CDataList< CChar * > * CSpawnRegion::GetSpawnedCharsList( void )
{
	return &spawnedChars;
}

}
