#include "uox3.h"
#include "debug.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "npcs.cpp"

//o---------------------------------------------------------------------------o
//|	Function	-	void DeleteChar( CChar *k )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Delete character
//o---------------------------------------------------------------------------o
void cCharStuff::DeleteChar( CChar *k )
{
	if( k == NULL )
		return;
	CHARACTER kChar = calcCharFromSer( k->GetSerial() );
	MapRegion->RemoveChar( k );

	// If we delete a NPC we should delete his tempeffects as well
	for( teffect_st *Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
	{
		if( Effect->Destination() == k->GetSerial() )
			Effect->Destination( INVALIDSERIAL );
		
		if( Effect->Source() == k->GetSerial() ) 
			Effect->Source( INVALIDSERIAL );
	}

	// if we delete a NPC we should delete him from spawnregions
	// this will fix several crashes
	if( k->IsNpc() && k->IsSpawned() )
	{
		cSpawnRegion *spawnReg = NULL;

		for( UI32 i = 1; i < totalspawnregions; i++ )
		{
			spawnReg = spawnregion[i];

			if( spawnReg == NULL )
				continue;
			else
				spawnReg->deleteSpawnedChar( k );
		}
	}

	for( CItem *tItem = k->FirstItem(); !k->FinishedItems(); tItem = k->NextItem() )
	{
		if( tItem != NULL )
			Items->DeleItem( tItem );
	}
	
	if( !k->IsNpc() )
	{
		ACTREC *mAcct = k->GetAccountObj();
		if( mAcct != NULL )
		{
			for( UI08 actr = 0; actr < 5; actr++ )
			{
				if( mAcct->characters[actr] != NULL && mAcct->characters[actr]->GetSerial() == k->GetSerial() )
				{
					Accounts->RemoveCharacterFromAccount( mAcct, actr );
					break;
				}
			}
		}
	}

	UI16 scpNum = k->GetScriptTrigger();
	cScript *tScript = Trigger->GetScript( scpNum );
	if( tScript != NULL )
		tScript->OnDelete( k );

	if( k->GetSpawn() != 0 ) 
		ncspawnsp.Remove( k->GetSpawn(), kChar );

	if( k->IsGuarded() )
	{
		CChar *petGuard = Npcs->getGuardingPet( k, k->GetSerial() );
		if( petGuard != NULL )
			petGuard->SetGuarding( INVALIDSERIAL );
		k->SetGuarded( false );
	}
	stopPetGuarding( k );

	k->RemoveFromSight();

	ncharsp.Remove( k->GetSerial(), kChar );
	chars.Delete( kChar );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *MemCharFree( CHARACTER& offset, bool zeroSer )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a place in memory to hold a new NPC
//o---------------------------------------------------------------------------o
CChar * cCharStuff::MemCharFree( CHARACTER& offset, bool zeroSer )
{
	offset = chars.New( zeroSer );
	if( charcount >= cmem ) //Lets ASSUME theres no more memory left instead of acctually checking all the items to find a free one.
		cmem++;
	if( offset == INVALIDSERIAL )
		return NULL;
	return &chars[offset];
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *addRandomLoot( CItem *s, char *lootlist )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Add loot to monsters packs
//o---------------------------------------------------------------------------o
CItem *cCharStuff::addRandomLoot( CItem *s, const char *lootlist )
{
	char sect[512];
	CItem *retitem = NULL;
	sprintf( sect, "LOOTLIST %s", lootlist );
	ScriptSection *LootList = FileLookup->FindEntry( sect, npc_def );
	if( LootList == NULL )
		return NULL;
	int i = LootList->NumEntries();
	if( i > 0 )
	{
		i = RandomNum( 0, i - 1 );
		const char *tag = LootList->MoveTo( i );
		if( tag == NULL )
			return NULL;
		retitem = Items->CreateScriptItem( NULL, tag, false, s->WorldNumber() );
		if( retitem != NULL )
		{
			retitem->SetX( 50 + RandomNum( 0, 79 ) );
			retitem->SetY( 50 + RandomNum( 0, 79 ) );
			retitem->SetZ( 9 );
			retitem->SetCont( s->GetSerial() );
		}
	}
	return retitem;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CHARACTER CreateRandomNpc( cSocket *s, char * npclist, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Adds a random NPC
//o---------------------------------------------------------------------------o
CChar *cCharStuff::CreateRandomNpc( cSocket *s, const char * npcList, UI08 worldNumber )
{
	char sect[512];
	sprintf( sect, "NPCLIST %s", npcList );
	ScriptSection *NpcList = FileLookup->FindEntry( sect, npc_def );
	if( NpcList == NULL )
		return NULL;
	int i = NpcList->NumEntries();
	if( i == 0 )
		return NULL;
	const char *k = NpcList->MoveTo( RandomNum( 0, i - 1 ) );

	if( k != NULL )
		return CreateScriptNpc( s, k, worldNumber );
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CChar *SpawnNPC( CItem *i, int npcNum, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn an NPC
//o---------------------------------------------------------------------------o
CChar *cCharStuff::SpawnNPC( CItem *i, string npcNum, UI08 worldNumber, bool randomNPC = false )
{
	// If the spawner type is 125 and escort quests are not active then abort
	if( i != NULL )
	{
		if( i->GetType() == 125 && !cwmWorldState->ServerData()->GetEscortsEnabled() )
			return NULL;
	}
	CChar *c = NULL;
	if( randomNPC )
		c = CreateRandomNpc( NULL, npcNum.c_str(), worldNumber );
	else
		c = CreateScriptNpc( NULL, npcNum, worldNumber );
	if( c == NULL )
		return NULL;
	CHARACTER mChar = calcCharFromSer( c->GetSerial() );
	c->SetSpawn( 0, mChar );
	// see if we are item spawning or region spawning
	if( i != NULL )
	{
		int awayX = 0, awayY = 0;
		if( i->GetType() == 69 && i->GetCont() == INVALIDSERIAL )   
			awayX = awayY = 10;
		else if( i->GetType() == 125 && i->GetCont() == INVALIDSERIAL )
		{
			awayX = i->GetMore( 3 );
			awayY = i->GetMore( 4 );
		}
		FindSpotForNPC( c, i->GetX(), i->GetY(), awayX, awayY, i->GetZ() );
		c->SetSpawn( i->GetSerial(), mChar );
	}
	PostSpawnUpdate( c );
	if( i != NULL )
	{
		if( i->GetType() == 125 )
			MsgBoardQuestEscortCreate( c );
	}
	return c;
}
//o---------------------------------------------------------------------------o
//|	Function	-	CChar *SpawnNPC( cSpawnRegion *spawnReg, int npcNum, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn an NPC
//o---------------------------------------------------------------------------o
CChar *cCharStuff::SpawnNPC( cSpawnRegion *spawnReg, string npcNum, UI08 worldNumber )
{
	CChar *c = CreateScriptNpc( NULL, npcNum, worldNumber );
	if( c == NULL )
		return NULL;
	if( spawnReg != NULL )
		c->SetSpawn( calcserial( 0, 1, spawnReg->GetRegionNum(), 0 ), calcCharFromSer( c->GetSerial() ) );
	else
		c->SetSpawn( 0, calcCharFromSer( c->GetSerial() ) );
	// see if we are item spawning or region spawning
	if( spawnReg != NULL )
	{
		SI16 xAway		= ( spawnReg->GetX2() - spawnReg->GetX1() ) / 2;
		SI16 yAway		= ( spawnReg->GetY2() - spawnReg->GetY1() ) / 2;
		SI16 originX	= spawnReg->GetX1() + xAway;
		SI16 originY	= spawnReg->GetY1() + yAway;
		if( xAway <= 0 )
		{
			Console.Error( 2, " Spawn region %d: x1 >= x2", spawnReg->GetRegionNum() - 1 );
			xAway = 10;
		}
		else if( yAway <= 0 )
		{
			Console.Error( 2, " Spawn region %d: y1 >= y2", spawnReg->GetRegionNum() - 1 );
			yAway = 10;
		}
		FindSpotForNPC( c, originX, originY, xAway, yAway, illegal_z );
	}
	PostSpawnUpdate( c );
	return c;
}

void cCharStuff::PostSpawnUpdate( CChar *c )
{
	c->SetRegion( calcRegionFromXY( c->GetX(), c->GetY(), c->WorldNumber() ) );

	for( int z = 0; z < TRUESKILLS; z++ )
		Skills->updateSkillLevel( c, z );
   
	c->Update();
	MapRegion->AddChar( c );
	setcharflag( c );
	cScript *toGrab = Trigger->GetScript( c->GetScriptTrigger() );
	if( toGrab != NULL )
		toGrab->OnCreate( c );
}
//o---------------------------------------------------------------------------o
//|	Function	-	CChar *AddNPC( cSocket *s, cSpawnRegion *spawnReg, int npcNum, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn an NPC and place him in the region or on the item that
//|					spawned him.
//o---------------------------------------------------------------------------o
CChar *cCharStuff::AddNPC( cSocket *s, cSpawnRegion *spawnReg, string npcNum, UI08 worldNumber )
{
	CChar *c = CreateScriptNpc( s, npcNum, worldNumber );
	if( c == NULL )
		return NULL;
	if( spawnReg != NULL )
		c->SetSpawn( calcserial( 0, 1, spawnReg->GetRegionNum(), 0 ), calcCharFromSer( c->GetSerial() ) );
	else
		c->SetSpawn( 0, calcCharFromSer( c->GetSerial() ) );
	if( s != NULL )
	{
		const SI16 coreX = s->GetWord( 11 );
		const SI16 coreY = s->GetWord( 13 );
		const SI08 coreZ = s->Buffer()[16] + Map->TileHeight( s->GetWord( 17 ) );
		if( c->GetFx( 1 ) == -1 )
		{
			c->SetFx( coreX, 1 );
			c->SetFy( coreY, 1 );
			if( c->GetFz() != -1 ) 
				c->SetFz( coreZ );
		}
		c->SetLocation( coreX, coreY, coreZ );
	}
	PostSpawnUpdate( c );
	return c;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 getRadius( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Generate a sensible radius given the values from npc.scp
//o---------------------------------------------------------------------------o
SI16 getRadius( CChar *c )
{
	// see if they supplied a 'radius'
	if( c->GetFx( 2 ) > 0)
	{
		// if they were supplying a bounding area, use the radius from that
		if( c->GetFx( 1 ) > 0)
			c->SetFx( abs(c->GetFx( 1 ) - c->GetFx( 2 )), 2 );
	}
	// ensure its not something bogus
	if( c->GetFx( 2 ) <= 0 || c->GetFx( 2 ) > 100)
		c->SetFx( 10, 2 );
	return c->GetFx( 2 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Setup the wander area if the npcwander is rect or circle
//o---------------------------------------------------------------------------o
void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway )
{
	// compute the rectangular bounding area
	if( 3 == c->GetNpcWander() )
	{
		// if they provided a legal rectangle and
		// ensure the bounding rect contains the current location
		// if it doesn't the monster will never move!
		if( c->GetFx( 1 ) >= 0 && c->GetFy( 1 ) >= 0 && c->GetFy( 2 ) >= 0 && c->GetFx( 2 ) >= 0 &&
			checkBoundingBox( c->GetX(), c->GetY(), c->GetFx( 1 ), c->GetFy( 1 ), c->GetFz(), c->GetFx( 2 ), c->GetFy( 2 ), c->WorldNumber() ) )
		{
			// don't do anything to use what they specified in npc.scp		  
		}
		else
		{
			// if they provided a 'radius' in the npc.scp use that
			if( c->GetFx( 2 ) > 0)
				xAway = yAway = getRadius( c );
			// setup info for rectangular areas
			c->SetFx( c->GetX() - xAway, 1 );
			c->SetFy( c->GetY() - yAway, 1 );
			c->SetFx( c->GetX() + xAway, 2 );
			c->SetFy( c->GetX() + yAway, 2 );
		}
	}
	else if( 4 == c->GetNpcWander() )
	{
		// if they provided a legal circle and
		// ensure the bounding circle contains the current location
		// if it doesn't the monster will never move!
		if( c->GetFx( 1 ) >= 0 && c->GetFy( 1 ) >= 0 && c->GetFx( 2 ) >= 0 &&
			checkBoundingCircle( c->GetX(), c->GetY(), c->GetFx( 1 ), c->GetFy( 1 ), c->GetFz(), c->GetFx( 2 ), c->WorldNumber() ) )
		{
			// don't do anything to use what they specified in npc.scp		  
		}
		else
		{
			// if they provided a 'radius' in the npc.scp use that
			if( c->GetFx( 2 ) > 0)
				xAway = yAway = getRadius( c );
			// setup info for circular areas
			c->SetFx( c->GetX(), 1 );
			c->SetFy( c->GetY(), 1 );
			c->SetFx( xAway, 2 );
			c->SetFy( -1, 2 );
		}
	}
	// setting fz1 actually makes it check against the height and slows the system down a lot
	// does anyone really need to constrain the height at which a monster can move?? - fur
	//chars[c].fz1 = chars[c].z;
	
#ifdef DEBUG_SPAWN
	printf("Bounding area for this monster is fx1: %d, fx2: %d, fy1: %d, fy2: %d\n", c->GetFx( 1 ), c->GetFx( 2 ), c->GetFy( 1 ), c->GetFy( 2 ) );
#endif
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::FindSpotForNPC( CHARACTER c, SI16 originX, SI16 originY, SI16 xAway, SI16 yAway, SI08 z )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a valid spot to drop an NPC near the spawners location
//o---------------------------------------------------------------------------o
void cCharStuff::FindSpotForNPC( CChar *c, SI16 originX, SI16 originY, SI16 xAway, SI16 yAway, SI08 z )
{
  /*Zippy's Code chages for area spawns --> (Type 69) xos and yos (X OffSet, Y OffSet) 
    are used to find a random number that is then added to the spawner's x and y (Using 
    the spawner's z) and then place the NPC anywhere in a square around the spawner. 
    This square is random anywhere from -10 to +10 from the spawner's location (for x and 
    y) If the place chosen is not a valid position (the NPC can't walk there) then a new 
    place will be chosen, if a valid place cannot be found in a certain # of tries (50), 
    the NPC will be placed directly on the spawner and the server op will be warned. */

#ifdef DEBUG_SPAWN
	Console.Print( "Going to spawn at (%d,%d) within %d by %d\n", originX, originY, xAway, yAway );
#endif
	
	if( c == NULL )
		return;
	int k = xAway * yAway / 2;
	SI16 xos = 0, yos = 0;
	bool foundSpot = false;
	if( k > 50 ) 
		k = 50;
	
	UI08 worldNumber = c->WorldNumber();
	while( !foundSpot )
    {
		if( --k < 0 ) //this CAN be a bit laggy. adjust as nessicary
		{
			if( xAway > 0 && yAway > 0 )
				Console << "Problem area spawner found, NPC placed at default location" << myendl;
			xos = originX;
			yos = originY;
			foundSpot = true;
			break;
		}
		
		xos = originX + RandomNum( -xAway, xAway );
		yos = originY + RandomNum( -yAway, yAway );
		
		if( xos >= 1 && yos >= 1 )
			foundSpot = Map->CanMonsterMoveHere( xos, yos, z, worldNumber );
    }

	// should we not add and remove from mapregions here????
	SI08 targZ = 0;
	if( illegal_z == z )
		targZ = Map->Height( xos, yos, 0, worldNumber );
	else
		targZ = z;

	c->SetLocation( xos, yos, targZ );
	InitializeWanderArea( c, xAway, yAway );
}


//o---------------------------------------------------------------------------o
//|	Function	-	CHARACTER cCharStuff::AddNPCxyz( cSocket *s, int npcNum, SI16 x1, SI16 y1, SI08 z1, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Add NPC at a given location
//o---------------------------------------------------------------------------o
CChar * cCharStuff::AddNPCxyz( cSocket *s, int npcNum, SI16 x1, SI16 y1, SI08 z1, UI08 worldNumber )
{
	CChar *c = CreateScriptNpc( s, npcNum, worldNumber );
	if( c == NULL )
		return NULL;
	c->SetLocation( x1, y1, z1 );

	if( s != NULL )
	{
		if( c->GetFx( 1 ) == -1 )
		{
			c->SetFx( s->GetWord( 11 ), 1 );
			c->SetFy( s->GetWord( 13 ), 1 );
			if( c->GetFz() != -1 ) 
				c->SetFz( s->Buffer()[16] + Map->TileHeight( s->GetWord( 17 ) ) );
		}
	}
   
	c->SetRegion( calcRegionFromXY(c->GetX(), c->GetY(), worldNumber ) );

	for( int z = 0; z < TRUESKILLS; z++ )
		Skills->updateSkillLevel( c, z );
   
	c->Update();
	setcharflag( c );
	cScript *toGrab = Trigger->GetScript( c->GetScriptTrigger() );
	if( toGrab != NULL )
		toGrab->OnCreate( c );
	return c;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::Split( CChar *k )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Cause NPC to split during combat
//o---------------------------------------------------------------------------o
void cCharStuff::Split( CChar *k )
{
	CChar *c = k->Dupe();
	if( c == NULL )
		return;

	SERIAL serial = c->GetSerial();
	c->SetSerial( serial, calcCharFromSer( c->GetSerial() ) );
	c->SetFTarg( INVALIDSERIAL );
	c->SetLocation( k->GetX() + 1, k->GetY(), k->GetZ() );
	c->SetKills( 0 );
	c->SetHP( k->GetMaxHP() );
	c->SetStamina( k->GetMaxStam() );
	c->SetMana( k->GetMaxMana() );
	if( RandomNum( 0, 34 ) == 5 ) 
		c->SetSplit( 1 );
	else 
		c->SetSplit( 0 );
	c->Update();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::LoadShopList( char *list, CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the shopping list pointed to by data in items.scp
//o---------------------------------------------------------------------------o
void cCharStuff::LoadShopList( const char *list, CChar *c )
{
	CItem *buyLayer = FindItemOnLayer( c, 0x1A );
	CItem *boughtLayer = FindItemOnLayer( c, 0x1B );
	CItem *sellLayer = FindItemOnLayer( c, 0x1C );

	char sect[512];
	sprintf( sect, "SHOPLIST %s", list );
	ScriptSection *ShoppingList = FileLookup->FindEntry( sect, items_def );
	if( ShoppingList == NULL )
		return;

	DFNTAGS tag = DFNTAG_COUNTOFTAGS;
	const char *cdata = NULL;
	UI32 ndata = INVALIDSERIAL, odata = INVALIDSERIAL;
	CItem *retitem = NULL;

	for( tag = ShoppingList->FirstTag(); !ShoppingList->AtEndTags(); tag = ShoppingList->NextTag() )
	{
		cdata = ShoppingList->GrabData( ndata, odata );
		switch( tag )
		{
		case DFNTAG_RSHOPITEM:
				if( buyLayer != NULL ) 
				{
					retitem = Items->CreateScriptItem( NULL, cdata, false, c->WorldNumber() );
					if( retitem != NULL )
					{
						retitem->SetCont( buyLayer->GetSerial() );
						retitem->SetX( 50 + ( RandomNum( 0, 79 ) ) );
						retitem->SetY( 50 + ( RandomNum( 0, 79 ) ) );
						retitem->SetZ( 9 );
						if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
							retitem->SetName( retitem->GetName2() ); // Item identified! -- by Magius(CHE)
					}
				} 
				else 
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Buy Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				break;
		case DFNTAG_SELLITEM:
				if( sellLayer != NULL ) 
				{
					retitem = Items->CreateScriptItem( NULL, cdata, false, c->WorldNumber() );
					if( retitem != NULL )
					{
						retitem->SetCont( sellLayer->GetSerial() );
						retitem->SetValue( retitem->GetValue() / 2 );
						retitem->SetX( 50 + ( RandomNum( 0, 79 ) ) );
						retitem->SetY( 50 + ( RandomNum( 0, 79 ) ) );
						retitem->SetZ( 9 );
						if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
							retitem->SetName( retitem->GetName2() );
					}
				} 
				else 
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Sell Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				break;
		case DFNTAG_SHOPITEM:
				if( boughtLayer != NULL ) 
				{
					retitem = Items->CreateScriptItem( NULL, cdata, false, c->WorldNumber() );
					if( retitem != NULL )
					{
						retitem->SetCont( boughtLayer->GetSerial() );
						retitem->SetX( 50 + ( RandomNum( 0, 79 ) ) );
						retitem->SetY( 50 + ( RandomNum( 0, 79 ) ) );
						retitem->SetZ( 9 );
						if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
							retitem->SetName( retitem->GetName2() );
					}
				} 
				else 
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Bought Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				break;
		case DFNTAG_VALUE:
				if( retitem != NULL ) 
					retitem->SetValue( ndata );
				break;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCharStuff::ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Apply Npc.scp sections to an NPC
//o---------------------------------------------------------------------------o
bool cCharStuff::ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation )
{
	if( NpcCreation == NULL || applyTo == NULL )
		return false;

	UI16 haircolor	= 0xFFFF;
	UI16 storeval	= 0xFFFF;
	char rndlootlist[20];

	CItem *buyPack = NULL, *boughtPack = NULL, *sellPack = NULL;
	CItem *retitem = NULL, *mypack = NULL, *n = NULL;

	DFNTAGS tag = DFNTAG_COUNTOFTAGS;
	const char *cdata = NULL;
	UI32 ndata = INVALIDSERIAL, odata = INVALIDSERIAL;
	for( tag = NpcCreation->FirstTag(); !NpcCreation->AtEndTags(); tag = NpcCreation->NextTag() )
	{
		cdata = NpcCreation->GrabData( ndata, odata );
		switch( tag )
		{
		case DFNTAG_ALCHEMY:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ALCHEMY );		break;
		case DFNTAG_ANATOMY:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ANATOMY );		break;
		case DFNTAG_ANIMALLORE:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ANIMALLORE );	break;
		case DFNTAG_ARMSLORE:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ARMSLORE );		break;
		case DFNTAG_ARCHERY:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ARCHERY );		break;
		case DFNTAG_DAMAGE:
		case DFNTAG_ATT:				applyTo->SetLoDamage( (SI16)ndata );applyTo->SetHiDamage( (SI16)odata );	break;
		case DFNTAG_BACKPACK:			if( mypack == NULL )
											mypack = getPack( applyTo );
										if( mypack == NULL )
										{
											n = Items->SpawnItem( NULL, applyTo, 1, "Backpack", false, 0x0E75, 0, false, false );
											if( n != NULL )
											{
												applyTo->SetPackItem( n );
												n->SetX( 0 );
												n->SetY( 0 );
												n->SetZ( 0 );
												n->SetLayer( 0x15 );
												if( !n->SetCont( applyTo->GetSerial() ) )
													retitem = NULL;
												else
												{
													n->SetType( 1 );
													n->SetDye( true );
													mypack = n;
													retitem = n;
												}
											}
										}
										break;
		case DFNTAG_BEGGING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), BEGGING );			break;
		case DFNTAG_BLACKSMITHING:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), BLACKSMITHING );	break;
		case DFNTAG_BOWCRAFT:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), BOWCRAFT );			break;
		case DFNTAG_COLOUR:				if( retitem != NULL )
											retitem->SetColour( (UI16)ndata );
										break;
		case DFNTAG_CAMPING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), CAMPING );			break;
		case DFNTAG_CARPENTRY:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), CARPENTRY );		break;
		case DFNTAG_CARTOGRAPHY:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), CARTOGRAPHY );		break;
		case DFNTAG_COOKING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), COOKING );			break;
		case DFNTAG_COLOURMATCHHAIR:	if( retitem != NULL )
											retitem->SetColour( (UI16)haircolor );						
										break;
		case DFNTAG_COLOURLIST:			storeval = addRandomColor( cdata );
										if( retitem != NULL )
											retitem->SetColour( storeval );
										break;
		case DFNTAG_CARVE:				applyTo->SetCarve( ndata );											break;
		case DFNTAG_DIR:				if( !strcmp( "NE", cdata ) )
											applyTo->SetDir( 1 );
										else if( !strcmp( "E", cdata ) )
											applyTo->SetDir( 2 );
										else if( !strcmp( "SE", cdata ) )
											applyTo->SetDir( 3 );
										else if( !strcmp( "S", cdata ) )
											applyTo->SetDir( 4 );
										else if( !strcmp( "SW", cdata ) )
											applyTo->SetDir( 5 );
										else if( !strcmp( "W", cdata ) )
											applyTo->SetDir( 6 );
										else if( !strcmp( "NW", cdata ) )
											applyTo->SetDir( 7 );
										else if( !strcmp( "N", cdata ) )
											applyTo->SetDir( 0 );
										break;
		case DFNTAG_DEX:				applyTo->SetDexterity( (SI16)RandomNum( ndata, odata ) );
										applyTo->SetStamina( applyTo->GetMaxStam() );
										break;
		case DFNTAG_DETECTINGHIDDEN:	applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), DETECTINGHIDDEN );	break;
		case DFNTAG_DEF:				applyTo->SetDef( (UI16)RandomNum( ndata, odata ) );							break;
		case DFNTAG_EMOTECOLOUR:		applyTo->SetEmoteColour( (UI16)ndata );										break;
		case DFNTAG_ENTICEMENT:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ENTICEMENT );		break;
		case DFNTAG_EVALUATINGINTEL:	applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), EVALUATINGINTEL );	break;
		case DFNTAG_FAME:				applyTo->SetFame( (SI16)ndata );											break;
		case DFNTAG_FX1:				applyTo->SetFx( (SI16)ndata, 1 );											break;
		case DFNTAG_FX2:				applyTo->SetFx( (SI16)ndata, 2 );											break;
		case DFNTAG_FY1:				applyTo->SetFy( (SI16)ndata, 1 );											break;
		case DFNTAG_FY2:				applyTo->SetFy( (SI16)ndata, 2 );											break;
		case DFNTAG_FZ1:				applyTo->SetFz( (SI08)ndata );												break;
		case DFNTAG_FLEEAT:				applyTo->SetFleeAt( (SI16)ndata );											break;
		case DFNTAG_FISHING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), FISHING );			break;
		case DFNTAG_FORENSICS:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), FORENSICS );		break;
		case DFNTAG_FENCING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), FENCING );			break;
		case DFNTAG_GOLD:				if( mypack == NULL ) 
											mypack = getPack( applyTo );
										if( mypack != NULL )
										{
											n = Items->SpawnItem( NULL, applyTo, 1, "#", true, 0x0EED, 0, true, false );
											if( n != NULL )
											{
												n->SetDecayable( true );
												n->SetAmount( (UI16)( RandomNum( (int)(ndata) , (int)(odata) ) ) ); 
												n->SetCont( mypack->GetSerial() );
											}
										}
										else
											Console << "Warning: Bad NPC Script with problem no backpack for gold" << myendl;
										break;
		case DFNTAG_GET:
									{
										char mTemp[128];
										if( cwmWorldState->ServerData()->ServerScriptSectionHeader() )
											sprintf( mTemp, "NPC %s", cdata );
										else
											strcpy( mTemp, cdata );
										ScriptSection *toFind = FileLookup->FindEntry( mTemp, npc_def );
										ApplyNpcSection( applyTo, toFind );
									}
										break;
		case DFNTAG_HEALING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), HEALING );		break;
		case DFNTAG_HERDING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), HERDING );		break;
		case DFNTAG_HIDING:				applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), HIDING );		break;
		case DFNTAG_HAIRCOLOUR:			haircolor = addRandomColor( cdata );
										if( retitem != NULL )
											retitem->SetColour( haircolor );
										break;
		case DFNTAG_HIDAMAGE:			applyTo->SetHiDamage( (SI16)ndata );									break;
		case DFNTAG_ID:					applyTo->SetID( (UI16)ndata );
										applyTo->SetxID( (UI16)ndata );
										applyTo->SetOrgID( (UI16)ndata );
										break;
		case DFNTAG_ITEM:
										retitem = Items->CreateScriptItem( NULL, cdata, false, applyTo->WorldNumber() );
										if( retitem != NULL )
										{
											if( !retitem->SetCont( applyTo->GetSerial() ) )
												retitem = NULL;
											else if( retitem->GetLayer() == 0 ) 
												Console << "Warning: Bad NPC Script with problem item " << cdata << " executed!" << myendl;
										}
										break;
		case DFNTAG_INTELLIGENCE:		applyTo->SetIntelligence( (SI16)RandomNum( ndata, odata ) );
										applyTo->SetMana( applyTo->GetMaxMana() );
										break;
		case DFNTAG_ITEMID:				applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), ITEMID );		break;
		case DFNTAG_INSCRIPTION:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), INSCRIPTION );	break;
		case DFNTAG_KARMA:				applyTo->SetKarma( (SI16)ndata );										break;
		case DFNTAG_LOOT:				if( mypack == NULL ) 
											mypack = getPack( applyTo );
										if( mypack != NULL ) 
										{
											strcpy( rndlootlist, cdata );
											retitem = addRandomLoot( mypack, rndlootlist );
										}
										else
											Console << "Warning: Bad NPC Script with problem no backpack for loot" << myendl;
										break;
		case DFNTAG_LOCKPICKING:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), LOCKPICKING );		break;
		case DFNTAG_LUMBERJACKING:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), LUMBERJACKING );	break;
		case DFNTAG_LODAMAGE:			applyTo->SetLoDamage( (SI16)ndata );										break;
		case DFNTAG_MAGERY:				applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), MAGERY );			break;
		case DFNTAG_MAGICRESISTANCE:	applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), MAGICRESISTANCE );	break;
		case DFNTAG_MUSICIANSHIP:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), MUSICIANSHIP );		break;
		case DFNTAG_MACEFIGHTING:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), MACEFIGHTING );		break;
		case DFNTAG_MINING:				applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), MINING );			break;
		case DFNTAG_MEDITATION:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), MEDITATION );		break;
		case DFNTAG_NAME:				applyTo->SetName( cdata );													break;
		case DFNTAG_NAMELIST:			setRandomName( applyTo, cdata );											break;
		case DFNTAG_NPCWANDER:			applyTo->SetNpcWander( (SI08)ndata );										break;
		case DFNTAG_NPCAI:				applyTo->SetNPCAiType( (SI16)ndata );										break;
		case DFNTAG_NOTRAIN:			applyTo->SetCanTrain( false );										break;
		case DFNTAG_PACKITEM:			if( mypack == NULL ) 
											mypack = getPack( applyTo );
										if( mypack != NULL ) 
										{
											retitem = Items->CreateScriptItem( NULL, cdata, false, applyTo->WorldNumber() );
											if( retitem != NULL )
											{
												retitem->SetCont( mypack->GetSerial() );
												retitem->SetX( 50 + RandomNum( 0, 79 ) );
												retitem->SetY( 50 + RandomNum( 0, 79 ) );
												retitem->SetZ( 9 );
											}
										}
										else
											Console << "Warning: Bad NPC Script with problem no backpack for packitem" << myendl;
										break;
		case DFNTAG_PRIV1:				applyTo->SetPriv( (UI08)ndata );											break;
		case DFNTAG_PRIV2:				applyTo->SetPriv2( (UI08)ndata );											break;
		case DFNTAG_POISON:				applyTo->SetPoison( (SI08)ndata );											break;
		case DFNTAG_PARRYING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), PARRYING );			break;
		case DFNTAG_PEACEMAKING:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), PEACEMAKING );		break;
		case DFNTAG_PROVOCATION:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), PROVOCATION );		break;
		case DFNTAG_POISONING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), POISONING );		break;
		case DFNTAG_RSHOPITEM:			if( buyPack == NULL )
											buyPack = FindItemOnLayer( applyTo, 0x1A );
										if( buyPack != NULL ) 
										{
											retitem = Items->CreateScriptItem( NULL, cdata, false, applyTo->WorldNumber() );
											if( retitem != NULL )
											{
												retitem->SetCont( buyPack->GetSerial() );
												retitem->SetX( 50 + RandomNum( 0, 79 ) );
												retitem->SetY( 50 + RandomNum( 0, 79 ) );
												retitem->SetZ( 9 );
												if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" )))
													retitem->SetName( retitem->GetName2() ); // Item identified! -- by Magius(CHE)
											}
										}
										else
											Console << "Warning: Bad NPC Script with no Vendor Buy Pack for item" << myendl;
										break;
		case DFNTAG_REATTACKAT:			applyTo->SetReattackAt( (SI16)ndata );										break;
		case DFNTAG_REMOVETRAPS:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), REMOVETRAPS );		break;
		case DFNTAG_RACE:				applyTo->SetRace( (UI16)ndata );											break;
		case DFNTAG_RUNS:				applyTo->SetRun( true );											break;
		case DFNTAG_SKIN:				applyTo->SetSkin( (UI16)ndata );
										applyTo->SetxSkin( (UI16)ndata );
										break;
		case DFNTAG_SHOPKEEPER:			Commands->MakeShop( applyTo );										break;
		case DFNTAG_SHOPLIST:			LoadShopList( cdata, applyTo );										break;
		case DFNTAG_SELLITEM:			if( sellPack == NULL ) 
											sellPack = FindItemOnLayer( applyTo, 0x1C );
										if( sellPack != NULL ) 
										{
											retitem = Items->CreateScriptItem( NULL, cdata, false, applyTo->WorldNumber() );
											if( retitem != NULL )
											{
												retitem->SetCont( sellPack->GetSerial() );
												retitem->SetValue( retitem->GetValue() / 2 );
												retitem->SetX( 50 + RandomNum( 0, 79 ) );
												retitem->SetY( 50 + RandomNum( 0, 79 ) );
												retitem->SetZ( 9 );
												if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
													retitem->SetName( retitem->GetName2() );
											}
										}
										else
											Console << "Warning: Bad NPC Script with no Vendor SellPack for item" << myendl;
										break;
		case DFNTAG_SHOPITEM:			if( boughtPack == NULL ) 
											boughtPack = FindItemOnLayer( applyTo, 0x1B );
										if( boughtPack != NULL ) 
										{
											retitem = Items->CreateScriptItem( NULL, cdata, false, applyTo->WorldNumber() );
											if( retitem != NULL )
											{
												retitem->SetCont( boughtPack->GetSerial() );
												retitem->SetX( 50 + RandomNum( 0, 79 ) );
												retitem->SetY( 50 + RandomNum( 0, 79 ) );
												retitem->SetZ( 9 );
												if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
													retitem->SetName( retitem->GetName2() );
											}
										}
										else
											Console << "Warning: Bad NPC Script with no Vendor Bought Pack for item" << myendl;
										break;
		case DFNTAG_SAYCOLOUR:			applyTo->SetSayColour( (UI16)ndata );										break;
		case DFNTAG_SPATTACK:			applyTo->SetSpAttack( (SI16)ndata );										break;
		case DFNTAG_SPADELAY:			applyTo->SetSpDelay( (SI08)ndata );											break;
		case DFNTAG_SPLIT:				applyTo->SetSplit( (UI08)ndata );											break;
		case DFNTAG_SPLITCHANCE:		applyTo->SetSplitChance( (UI08)ndata );										break;
		case DFNTAG_SNOOPING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), SNOOPING );			break;
		case DFNTAG_SPIRITSPEAK:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), SPIRITSPEAK );		break;
		case DFNTAG_STEALING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), STEALING );			break;
		case DFNTAG_STRENGTH:			applyTo->SetStrength( (SI16)RandomNum( ndata, odata ) );
										applyTo->SetHP( applyTo->GetMaxHP() );
										break;
		case DFNTAG_SWORDSMANSHIP:		applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), SWORDSMANSHIP );	break;
		case DFNTAG_STEALTH:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), STEALTH );			break;
		case DFNTAG_SKINLIST:			storeval = addRandomColor( cdata );
										applyTo->SetSkin( storeval );
										applyTo->SetxSkin( storeval );
										break;
		case DFNTAG_SKILL:				applyTo->SetBaseSkill( (UI16)odata, (UI08)ndata );							break;
/*			else if( !strncmp( tag, "SKILL", 5 ) )
			{
				skill = makeNum( &tag[5] );
				applyTo->SetBaseSkill( makeNum( data ), skill );
			}
*/
		case DFNTAG_SCRIPT:				applyTo->SetScriptTrigger( (UI16)ndata );								break;
		case DFNTAG_TITLE:				applyTo->SetTitle( cdata );												break;
		case DFNTAG_TOTAME:				applyTo->SetTaming( (SI16)ndata );										break;
		case DFNTAG_TACTICS:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), TACTICS );		break;
		case DFNTAG_TAILORING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), TAILORING );	break;
		case DFNTAG_TAMING:				applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), TAMING );		break;
		case DFNTAG_TASTEID:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), TASTEID );		break;
		case DFNTAG_TINKERING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), TINKERING );	break;
		case DFNTAG_TRACKING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), TRACKING );		break;
		case DFNTAG_VALUE:				if( retitem != NULL )
											retitem->SetValue( ndata );
										break;
		case DFNTAG_VETERINARY:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), VETERINARY );	break;
		case DFNTAG_WRESTLING:			applyTo->SetBaseSkill( (UI16)RandomNum( ndata, odata ), WRESTLING );	break;
		case DFNTAG_NOTES:
		case DFNTAG_CATEGORY:
			break;
		default:
			Console << "Unknown tag in ApplyNpcSection(): " << (SI32)tag << myendl;// << " data: " << cdata << " " << ndata << " " << odata << myendl;
			break;
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CHARACTER CreateScriptNpc( cSocket *s, int targNPC, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create an NPC and assign their variables based on npc.scp entrys
//o---------------------------------------------------------------------------o
CChar *cCharStuff::CreateScriptNpc( cSocket *s, int targNPC, UI08 worldNumber )
{
	char sect[512];
	sprintf( sect, "%i", targNPC );
	return CreateScriptNpc( s, sect, worldNumber );
}
//o---------------------------------------------------------------------------o
//|	Function	-	CHARACTER CreateScriptNpc( cSocket *s, string targNPC, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create an NPC and assign their variables based on npc.scp entrys
//o---------------------------------------------------------------------------o
CChar *cCharStuff::CreateScriptNpc( cSocket *s, string targNPC, UI08 worldNumber )
{
	char npcSect[512];
	if( cwmWorldState->ServerData()->ServerScriptSectionHeader() )
		sprintf( npcSect, "NPC %s", targNPC.c_str() );
	else
		strcpy( npcSect, targNPC.c_str() );
	ScriptSection *NpcCreation = FileLookup->FindEntry( npcSect, npc_def );
	if( NpcCreation == NULL )
		return NULL;

	DFNTAGS tag = DFNTAG_COUNTOFTAGS;
	for( tag = NpcCreation->FirstTag(); !NpcCreation->AtEndTags(); tag = NpcCreation->NextTag() )
	{
		if( tag == DFNTAG_NPCLIST )
		{
			UI32 tValue, uValue;
			return CreateRandomNpc( s, NpcCreation->GrabData( tValue, uValue ), worldNumber );
		}
	}

	CHARACTER npcNumOff;
	CChar *npcNum = MemCharFree( npcNumOff );
	if( npcNum == NULL )
		return NULL;

	npcNum->SetSkillTitles( true );
	npcNum->SetNpc( true );
	npcNum->SetLoDamage( 1 );
	npcNum->SetHiDamage( 1 );
	npcNum->SetDef( 1 );
	npcNum->SetSpawn( INVALIDSERIAL, npcNumOff );
	npcNum->WorldNumber( worldNumber );

	if( !ApplyNpcSection( npcNum, NpcCreation ) )
		cout << "Failed to apply NPC script settings for npc " << npcNum->GetSerial() << endl;
	
	return npcNum;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void npcAction( CChar *npc, int x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC does a certain action
//o---------------------------------------------------------------------------o
void npcAction( CChar *npc, int x )
{
	CPCharacterAnimation toSend = (*npc);
	toSend.Action( x );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( tSock->CurrcharObj(), npc ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void npcAct( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC Action
//o---------------------------------------------------------------------------o
void npcAct( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
		npcAction( i, s->AddID1() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void npcToggleCombat( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC Toggles combat mode
//o---------------------------------------------------------------------------o
void npcToggleCombat( CChar *s )
{
	s->SetWar( !s->IsAtWar() );
	Movement->CombatWalk( s );
}


//o---------------------------------------------------------------------------o
//|	Function	-	CChar * cCharStuff::getGuardingPet( CChar *mChar, SERIAL guarded )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the pet guarding an item / character
//o---------------------------------------------------------------------------o
CChar * cCharStuff::getGuardingPet( CChar *mChar, SERIAL guarded )
{
	if( mChar == NULL || guarded == INVALIDSERIAL )
		return NULL;

	CHARLIST *myPets = mChar->GetPetList();
	if( myPets != NULL )
	{
		for( UI32 j = 0; j < myPets->size(); j++ )
		{
			CChar *pet = (*myPets)[j];
			if( pet != NULL )
			{
				if( !pet->IsMounted() && pet->GetNPCAiType() == 32 && pet->GetGuarding() == guarded && pet->GetOwner() != mChar->GetSerial() )
					return pet;
			}
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCharStuff::checkPetFriend( CChar *mChar, CChar *pet )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Search a pets friends list to check if a character is a friend
//o---------------------------------------------------------------------------o
bool cCharStuff::checkPetFriend( CChar *mChar, CChar *pet )
{
	CHARLIST *petFriends = pet->GetFriendList();
	if( petFriends != NULL )
	{
		for( UI32 j = 0; j < petFriends->size(); j++ )
		{
			CChar *getFriend = (*petFriends)[j];
			if( getFriend != NULL && getFriend == mChar )
				return true;
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::stopPetGuarding( CChar *pet )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find the item/char pet is guarding and set it to not guarded
//o---------------------------------------------------------------------------o
void cCharStuff::stopPetGuarding( CChar *pet )
{
	SERIAL petGuarding = pet->GetGuarding();
	if( petGuarding == INVALIDSERIAL )
		return;

	CItem *itemGuard = calcItemObjFromSer( petGuarding );
	if( itemGuard != NULL )
		itemGuard->SetGuarded( false );
	else
	{
		CChar *charGuard = calcCharObjFromSer( petGuarding );
		if( charGuard != NULL )
			charGuard->SetGuarded( false );
	}
	pet->SetGuarding( INVALIDSERIAL );
}

//o---------------------------------------------------------------------------o
//|	Function	-	int addRandomColor( CChar *s, char *colorlist )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Randomly colors character
//o---------------------------------------------------------------------------o
UI16 addRandomColor( const char *colorlist )
{
	char sect[512];
	int i = 0;
	sprintf( sect, "RANDOMCOLOR %s", colorlist );
	ScriptSection *RandomColours = FileLookup->FindEntry( sect, colors_def );
	if( RandomColours == NULL ) 
	{
		Console.Warning( 2, "Error Colorlist %s Not Found", colorlist );
		return 0;
	}
	i = RandomColours->NumEntries();
	if( i > 0 )
	{
		i = rand()%i;
		const char *tag = RandomColours->MoveTo( i );
		return (UI16)makeNum( tag );
	}
	return 0;
}
