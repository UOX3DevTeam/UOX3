#include "uox3.h"
#include "cSpawnRegion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "mapstuff.h"

cSpawnRegion::cSpawnRegion( UI16 spawnregion ) : nexttime( 0 ), call( 1 ), x1( 0 ), x2( 0 ), y1( 0 ), y2( 0 ),
maxcspawn( 0 ), maxispawn( 0 ), maxtime( 0 ), mintime( 0 ), regionnum( spawnregion ), curcspawn( 0 ), curispawn( 0 ),
worldNumber( 0 )
{
	items.resize( 0 );
	npcs.resize( 0 );
	strncpy( name, Dictionary->GetEntry( 1117 ), MAX_REGIONNAME );
// note: doesn't go here, but i'll see it here.  when an item is spawned, as soon as it's moved it needs to lose it's
// spawn setting.  If not, then when people pick up spawned items, they will disappear (on region spawns)
}

cSpawnRegion::~cSpawnRegion()
{
	items.resize( 0 );
	npcs.resize( 0 );
// Wipe out all items and npcs
}

//o---------------------------------------------------------------------------o
//|	Function	-	const char *GetName( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs region name
//o---------------------------------------------------------------------------o
const char *cSpawnRegion::GetName( void ) const
{
	return name;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetName( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets region name (max 128 characters)
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetName( const char *newName )
{
	strncpy( name, newName, MAX_REGIONNAME );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetMaxSpawn( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs maximum amount of objects a region can spawn
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetMaxSpawn( void ) const
{
	return ( maxcspawn + maxispawn );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetMaxCharSpawn( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Return maximum amount of characters to spawn
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetMaxCharSpawn( void ) const
{
	return maxcspawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetMaxItemSpawn( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Return maximum amount of items to spawn
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetMaxItemSpawn( void ) const
{
	return maxispawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetMaxCharSpawn( SI32 newVal )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets maximum amount of characters a region can spawn
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetMaxCharSpawn( SI32 newVal )
{
	maxcspawn = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetMaxItemSpawn( SI32 newVal )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets maximum amount of items a region can spawn
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetMaxItemSpawn( SI32 newVal )
{
	maxispawn = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetCurrent( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs current amount of objects spawned
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetCurrent( void ) const
{
	return ( curcspawn + curispawn );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetCurrentCharAmt( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs current amount of characters spawned
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetCurrentCharAmt( void ) const
{
	return curcspawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetCurrentItemAmt( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs current amount of items spawned
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetCurrentItemAmt( void ) const
{
	return curispawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 GetRegionNum( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs region number
//o---------------------------------------------------------------------------o
UI16 cSpawnRegion::GetRegionNum( void ) const
{
	return regionnum;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetRegionNum( UI16 newVal )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets region number
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetRegionNum( UI16 newVal )
{
	regionnum = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 GetMinTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs the minimum amount of time to pass before a spawnregion
//|					spawns a new object
//o---------------------------------------------------------------------------o
UI08 cSpawnRegion::GetMinTime( void ) const
{
	return mintime;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cSpawnRegion::SetMinTime( UI08 newVal )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets the minimum amount of time to pass before a spawnregion
//|					spawns a new object
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetMinTime( UI08 newVal )
{
	mintime = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 GetMaxTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs the maximum amount of time to pass before a spawnregion
//|					spawns a new object
//o---------------------------------------------------------------------------o
UI08 cSpawnRegion::GetMaxTime( void ) const
{
	return maxtime;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cSpawnRegion::SetMaxTime( UI08 newVal )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum amount of time to pass before a spawnregion
//|					spawns a new object
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetMaxTime( UI08 newVal )
{
	maxtime = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 GetNextTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grab when a spawnregion will next spawn a new object
//o---------------------------------------------------------------------------o
SI32 cSpawnRegion::GetNextTime( void ) const
{
	return nexttime;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetNextTime( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Set when a spawnregion will next spawn a new object
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetNextTime( SI32 newVal )
{
	nexttime = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void cSpawnRegion::SetZ( SI08 newVal )
//|	Date					-	04/22/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
void cSpawnRegion::SetZ( SI08 newVal )
{
	z = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	SI08 cSpawnRegion::GetZ( void )
//|	Date					-	04/22/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
SI08 cSpawnRegion::GetZ( void ) const
{
	return z;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 GetY1( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs y1 to get the y pos of the top corner of the spawnregion
//o---------------------------------------------------------------------------o
SI16 cSpawnRegion::GetY1( void ) const
{
	return y1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetY1( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets y1 to the y pos of the top corner of the spawnregion
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetY1( SI16 newVal )
{
	y1 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 GetX1( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs x1 to get the x pos of the top corner of the spawnregion
//o---------------------------------------------------------------------------o
SI16 cSpawnRegion::GetX1( void ) const
{
	return x1;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetX1( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets x1 to the x pos of the top corner of the spawnregion
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetX1( SI16 newVal )
{
	x1 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 GetY2( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs y2 to get the y pos of the bottom corner of the spawnregion
//o---------------------------------------------------------------------------o
SI16 cSpawnRegion::GetY2( void ) const
{
	return y2;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetY2( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets y2 to the y pos of the bottom corner of the spawnregion
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetY2( SI16 newVal )
{
	y2 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 GetX2( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs x2 to get the x pos of the bottom corner of the spawnregion
//o---------------------------------------------------------------------------o
SI16 cSpawnRegion::GetX2( void ) const
{
	return x2;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SetX2( void )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets x2 to the x pos of the second corner of the spawnregion
//o---------------------------------------------------------------------------o
void cSpawnRegion::SetX2( SI16 newVal )
{
	x2 = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	Load( ScriptSection *toScan )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the spawnregion from spawn.scp script entry
//o---------------------------------------------------------------------------o
void cSpawnRegion::Load( ScriptSection *toScan )
{
	const char *data2 = NULL;
	char sect[512];
	char data[512];

	for( const char *tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
	{
		if( tag == NULL )
			continue;

		data2 = toScan->GrabData();
		strcpy( data, data2 );
		strupr( data );

		if( !strcmp( tag, "NPCLIST" ) )
		{
			sprintf( sect, "NPCLIST %s", data );
			ScriptSection *CharList = FileLookup->FindEntry( sect, npc_def );
			if( CharList != NULL )
			{
				for( const char *itm = CharList->First() ; !CharList->AtEnd(); itm = CharList->Next() )
					npcs.push_back( itm );
			}
		}
		else if( !strcmp( tag, "NPC" ) )
			npcs.push_back( data );
		else if( !strcmp( tag, "ITEMLIST" ) )
		{
			sprintf( sect, "ITEMLIST %s", data );
			ScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
			if( ItemList != NULL )
			{
				for( const char *itm = ItemList->First() ; !ItemList->AtEnd(); itm = ItemList->Next() )
					items.push_back( itm );
			}
		}
		else if( !strcmp( tag, "ITEM" ) )
			items.push_back( data );
		else if( !strcmp( tag, "MAXITEMS" ) )
			maxispawn = makeNum( data );
		else if( !strcmp( tag, "MAXNPCS" ) )
			maxcspawn = makeNum( data );
		else if( !strcmp( tag, "X1" ) )
			x1 = (SI16)makeNum( data );
		else if( !strcmp( tag, "X2" ) )
			x2 = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y1" ) )
			y1 = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y2" ) )
			y2 = (SI16)makeNum( data );
		else if( !strcmp( tag, "MINTIME" ) )
			mintime = static_cast<UI08>(makeNum( data ));
		else if( !strcmp( tag, "MAXTIME" ) )
			maxtime = static_cast<UI08>(makeNum( data ));
		else if( !strcmp( tag, "NAME" ) )
			strncpy( name, data, MAX_REGIONNAME );
		else if( !strcmp( tag, "CALL" ) )
			call = (SI16)makeNum( data );
		else if( !strcmp( tag, "WORLD" ) )
			worldNumber = (UI08)makeNum( data );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	doRegionSpawn( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do spawn stuff for spawnregion
//o---------------------------------------------------------------------------o
void cSpawnRegion::doRegionSpawn( void )
{
	SI32 j = 0;
	SI32 k = 0;

	if( npcs.size() == 0 )
		maxcspawn = 0;
	if( items.size() == 0 )
		maxispawn = 0;

	while( scharlist.size() < static_cast<unsigned int>(maxcspawn) )
		scharlist.push_back(NULL);
	while( sitemlist.size() < static_cast<unsigned int>(maxispawn ))
		sitemlist.push_back(NULL);

	for( SI32 i = 0 ; i < call ; i++ )
	{
		for( ; j < static_cast<SI32>(scharlist.size()) ; j++ )
		{
			if( scharlist[j] != NULL )
				continue;
			else
			{
				scharlist[j] = RegionSpawnChar();
				if( scharlist[j] != NULL )
					scharlist[j]->ShouldSave( false );
				break;
			}
		}

// sitemlist.size() should be 
		for( ; k < static_cast<SI32>(sitemlist.size() ); k++ )
		{
			if( sitemlist[k] != NULL && !sitemlist[k]->ShouldSave())
				continue;
			else
			{
				sitemlist[k] = RegionSpawnItem();
				if( sitemlist[k] != NULL )
				{
					SI16 x = 0, y = 0;
					SI08 z = 0;
					if( FindSpotForItem( x, y, z ) )
					{
						sitemlist[k]->SetLocation( x, y, z ); // On a move, we should set save to true
						sitemlist[k]->SetSpawn( calcserial( 0, 1, static_cast<UI08>(regionnum), 0 ), calcItemFromSer( sitemlist[k]->GetSerial() ) );
						RefreshItem( sitemlist[k] );
						sitemlist[k]->ShouldSave( false );
					} 
					else 
						Items->DeleItem( sitemlist[k] );
				}
				break;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cChar *RegionSpawnChar( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do a char spawn
//o---------------------------------------------------------------------------o
CChar *cSpawnRegion::RegionSpawnChar( void )
{
	CChar *CSpawn = NULL;
	CSpawn = Npcs->SpawnNPC( this, npcs[RandomNum( 0, npcs.size() - 1 )], worldNumber );
	if( CSpawn != NULL )
		CSpawn->IsSpawned( true );
	return CSpawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cItem *RegionSpawnItem( void )
//|	Programmer	-	Thyme
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do a item spawn
//o---------------------------------------------------------------------------o
CItem *cSpawnRegion::RegionSpawnItem( void )
{
	CItem *ISpawn = NULL;
	ISpawn = Items->CreateScriptItem( NULL, items[RandomNum( 0, items.size() - 1) ], false, worldNumber );
	if( ISpawn != NULL )
		ISpawn->IsSpawned( true );
	return ISpawn;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool FindSpotForItem( SI16 &x, SI16 &y, SI08 &z )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a random spot within a region valid for dropping an item
//o---------------------------------------------------------------------------o
bool cSpawnRegion::FindSpotForItem( SI16 &x, SI16 &y, SI08 &z )
{
	for( UI08 a = 0; a < 100; a++ ) 
	{
		x = (SI16)(RandomNum( x1, x2 ));
		y = (SI16)(RandomNum( y1, y2 ));
		z = Map->MapElevation( x, y, worldNumber );
		
		if( Map->CanMonsterMoveHere( x, y, z, worldNumber ) )
			return true;
	}
	Console << "Problem regionspawn [" << regionnum << "] found. Nothing will be spawned" << myendl;
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void checkSpawned()
//|	Programmer	-	sereg
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if an item/npc should be removed from spawnlist and saved
//o---------------------------------------------------------------------------o
void cSpawnRegion::checkSpawned( void )
{
	CHARLIST::iterator i;
	ITEMLIST::iterator j;

	for( i = scharlist.begin(); i != scharlist.end(); i++ )
	{
		CChar *charI = (CChar *)(*i);
		if( charI == NULL )
			continue;
		if( charI->GetOwnerObj() != NULL )
		{
			charI->ShouldSave( true );
			charI = NULL;
		}
	}
	
	for( j = sitemlist.begin(); j != sitemlist.end(); j++ )
	{
		CItem *itemJ = (CItem *)(*j);
		if( itemJ == NULL )
			continue;
		if( itemJ->GetCont() != NULL )
		{
			itemJ->ShouldSave( true );
			itemJ = NULL;
		}
	}

}

//o---------------------------------------------------------------------------o
//|	Function	-	void deleteSpawnedChar( CChar *toDelete )
//|	Programmer	-	sereg
//o---------------------------------------------------------------------------o
//|	Purpose		-	delete a npc from spawnlist
//o---------------------------------------------------------------------------o
void cSpawnRegion::deleteSpawnedChar( CChar *toDelete )
{
	CHARLIST::iterator i;

	for( i = scharlist.begin(); i != scharlist.end(); i++ )
	{
		CChar *iChar = (*i);
		if( iChar == NULL )
			continue;
		if( iChar == toDelete )
		{
			iChar = NULL;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deleteSpawnedItem( CItem *toDelete )
//|	Programmer	-	sereg
//o---------------------------------------------------------------------------o
//|	Purpose		-	delete an item from spawnlist
//o---------------------------------------------------------------------------o
void cSpawnRegion::deleteSpawnedItem( CItem *toDelete )
{
	ITEMLIST::iterator i;

	for( i = sitemlist.begin(); i != sitemlist.end(); i++ )
	{
		CItem *iItem = (*i);
		if( iItem == NULL )
			continue;
		if( iItem == toDelete )
			iItem = NULL;
	}
}
