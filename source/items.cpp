#include "uox3.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "items.cpp"

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * cItem::MemItemFree( bool zeroSer, UI08 itemType )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a free item slot, checking freeitemmem[] first
//o---------------------------------------------------------------------------o
CItem *cItem::MemItemFree( ITEM& offset, bool zeroSer, UI08 itemType )
{
	char memerr = 0;

	offset = items.New( zeroSer, itemType );

	int slots = 8192;
					// careful with this number.....
					// if it too low => slow and can cause crashes under
					// w95/98.  This is because w95/98 can only handle 8196
					// subsequent realloc calls ( crappy thing...)
					// free() calls DON'T help !!!, btw
					// so we have to make that number real big
					// under nt and unix this limit doesn't exist
	if( itemcount >= imem ) //theres no more free sluts.. er slots, so get more memory
	{
		int *Copy_loscache = NULL;
		int *Copy_itemids = NULL;

		// create new objects to copy original data to
		if( ( Copy_loscache = new int[imem] ) == NULL )
			memerr = 1;
		else if( ( Copy_itemids = new int[imem] ) == NULL )
			memerr = 1;

		// make sure nothing bad happened
		if( !memerr )
		{
			// make a copy - I prefer memcpy although a copy constructor wouldn't hurt =)
			memcpy( Copy_loscache, loscache, sizeof( int ) * imem );
			memcpy( Copy_itemids, itemids, sizeof( int ) * imem );

			// delete the old objects
			delete [] itemids;
			delete [] loscache;

			// initialize to NULL so if an error has occurred we can properly
			// free them by checking for NULL
			loscache = NULL;
			itemids = NULL;

			// create new objects with more room for more items, etc.
			if( ( loscache = new int[imem + slots] ) == NULL )
				memerr = 2;
			else if( ( itemids = new int[imem + slots] ) == NULL )
				memerr = 2;

			if( !memerr )
			{
				// restore copy to new objects
				memcpy( loscache, Copy_loscache, sizeof( int ) * imem );
				memcpy( itemids, Copy_itemids, sizeof( int ) * imem );

				// delete copies
				delete [] Copy_itemids;
				delete [] Copy_loscache;
			}
		}
		if( memerr )
		{
			// cleanup if neccessary
			if( memerr >= 1 )
			{
				if( Copy_itemids )
					delete [] Copy_itemids;
				if( Copy_loscache )
					delete [] Copy_loscache;

				// cleanup if neccessary
				if( memerr == 2 )
				{
					if( itemids )
						delete [] Copy_itemids;
					if( loscache )
						delete [] Copy_loscache;
				}
			}
			Console.Error( 3, "Could not reallocate item memory after %i. No more items will be created.", imem );
			Console.Warning( 3, " UOX may become unstable." );
		} 
		else 
			imem += slots;
	}
	if( offset == INVALIDSERIAL )
		return NULL;
	return &items[offset];
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::DeleItem( CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Remove an item from the array
//o---------------------------------------------------------------------------o
void cItem::DeleItem( CItem *i )
{
	if( i == NULL )
		return;
	if( i->isFree() )
		return;
	ITEM calcedItem = calcItemFromSer( i->GetSerial() );

	// if we delete an item we should delete it from spawnregions
	// this will fix several crashes
	if( i->IsSpawned() )
	{
		cSpawnRegion *spawnReg = NULL;

		for( UI16 k = 1; k < totalspawnregions; k++ )
		{
			spawnReg = spawnregion[k];
			if( spawnReg == NULL )
				continue;
			else
				spawnReg->deleteSpawnedItem( i );
		}
	}

	i->SetCont( NULL );

	cScript *tScript = NULL;
	UI16 scpNum = i->GetScriptTrigger();
	tScript = Trigger->GetScript( scpNum );
	if( tScript != NULL )
		tScript->OnDelete( i );

	CPRemoveItem toRemove = (*i);

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		tSock->Send( &toRemove );
	Network->PopConn();
	if( i->GetGlow() > 0 )
	{
		CItem *j = calcItemObjFromSer( i->GetGlow() );
		if( j != NULL )
			DeleItem( j );  // LB glow stuff, deletes the glower of a glowing stuff automatically
	}

	ITEM ci = INVALIDSERIAL;
	if( i->GetType() == 61 )
	{
		HashBucketMulti< ITEM > *hashBucketI = nspawnsp.GetBucket( (i->GetSerial())%HASHMAX );
		for( unsigned int j = 0; j < hashBucketI->NumEntries(); j++ )
		{
			ci = hashBucketI->GetEntry( j );
			if( ci != INVALIDSERIAL )
			{
				if( items[ci].isFree() )
					continue;
				if( i == items[ci].GetSpawnObj() )
				{
					if( i != &items[ci] )
						items[ci].SetSpawn( INVALIDSERIAL, ci );
				}
			}
		}
	}

	if( i->GetType() == 62 || i->GetType() == 69 || i->GetType() == 125 )
	{
		HashBucketMulti< CHARACTER > *hashBucketC = ncspawnsp.GetBucket( (i->GetSerial())%HASHMAX );
		for( unsigned int l = 0; l < hashBucketC->NumEntries(); l++ )
		{
			ci = hashBucketC->GetEntry( l );
			if( ci != INVALIDSERIAL )
			{
				if( chars[ci].isFree() )
					continue;
				if( chars[ci].GetSpawnObj() == i )
					chars[ci].SetSpawn( INVALIDSERIAL, ci );
			}
		}
	}

	if( i->GetSpawnObj() != NULL ) 
		nspawnsp.Remove( i->GetSpawn(), calcedItem );

	for( CItem *tItem = i->FirstItemObj(); !i->FinishedItems(); tItem = i->NextItemObj() )
	{
		if( tItem != NULL )
			Items->DeleItem( tItem );
	}

	if( i->isGuarded() )
	{
		CChar *owner = NULL;
		CMultiObj *multi = findMulti( i );
		if( multi != NULL )
			owner = (CChar *)multi->GetOwnerObj();
		if( owner == NULL )
			owner = getPackOwner( i );
		if( owner != NULL )
		{
			CChar *petGuard = Npcs->getGuardingPet( owner, i->GetSerial() );
			if( petGuard != NULL )
				petGuard->SetGuarding( INVALIDSERIAL );
			i->SetGuarded( false );
		}
	}

	nitemsp.Remove( i->GetSerial() );
	items.Delete( calcedItem );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load items from the script sections to the array
//o---------------------------------------------------------------------------o
bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply )
{
	if( toApply == NULL || applyTo == NULL )
		return false;
	DFNTAGS tag = DFNTAG_COUNTOFTAGS;
	const char *cdata = NULL;
	UI32 ndata = INVALIDSERIAL, odata = INVALIDSERIAL;
	for( tag = toApply->FirstTag(); !toApply->AtEndTags(); tag = toApply->NextTag() )
	{
		cdata = toApply->GrabData( ndata, odata );

		switch( tag )
		{
		case DFNTAG_AMOUNT:			applyTo->SetAmount( (UI32)ndata );				break;
		case DFNTAG_ATT:			applyTo->SetLoDamage( (SI16)ndata ); 
									applyTo->SetHiDamage( (SI16)odata ); 
									break;
		case DFNTAG_AC:				applyTo->SetArmourClass( (UI08)ndata );		break;
		case DFNTAG_CREATOR:		applyTo->SetCreator( ndata );				break;
		case DFNTAG_COLOUR:			applyTo->SetColour( (UI16)ndata );			break;
		case DFNTAG_COLOURLIST:		applyTo->SetColour( addRandomColor( cdata ) );		break;
		case DFNTAG_CORPSE:			applyTo->SetCorpse( ndata != 0 );			break;
		case DFNTAG_COLD:			applyTo->ColdDamage( ndata != 0 );			break;
		case DFNTAG_DAMAGE:			applyTo->SetLoDamage( (SI16)ndata );
									applyTo->SetHiDamage( (SI16)odata );
									break;
		case DFNTAG_DEF:			applyTo->SetDef( (UI16)RandomNum( ndata, odata ) );	break;
		case DFNTAG_DEX:			applyTo->SetDexterity( (SI16)RandomNum( ndata, odata ) );	break;
		case DFNTAG_DEXADD:			applyTo->Dexterity2( (SI16)ndata );					break;
		case DFNTAG_DIR:			if( !strcmp( "NE", cdata ) )
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
		case DFNTAG_DYE:			applyTo->SetDye( ndata != 0 );				break;
		case DFNTAG_DECAY:			
									if( ndata == 1 )
										applyTo->SetDecayable( true );
									else
										applyTo->SetDecayable( false );
									break;
		case DFNTAG_DISPELLABLE:	applyTo->SetDispellable( true );			break;
		case DFNTAG_DISABLED:		applyTo->SetDisabled( ndata != 0 );			break;
		case DFNTAG_DOORFLAG:		applyTo->SetDoorDir( (UI08)ndata );			break;
		case DFNTAG_ENHANCED:		applyTo->enhanced = ndata;					break;
		case DFNTAG_FAME:			applyTo->SetFame( (SI16)ndata );			break;
		case DFNTAG_GOOD:			applyTo->SetGood( ndata );					break;
		case DFNTAG_GLOW:			applyTo->SetGlow( ndata );					break;
		case DFNTAG_GLOWBC:			applyTo->SetGlowColour( (UI16)ndata );		break;
		case DFNTAG_GLOWTYPE:		applyTo->SetGlowEffect( (UI08)ndata );		break;
		case DFNTAG_GET:
								{
									char mTemp[128];
									if( cwmWorldState->ServerData()->ServerScriptSectionHeader() )
										sprintf( mTemp, "ITEM %s", cdata );
									else
										strcpy( mTemp, cdata );
									ScriptSection *toFind = FileLookup->FindEntry( mTemp, items_def );
									ApplyItemSection( applyTo, toFind );
								}
									break;
		case DFNTAG_HP:				applyTo->SetHP( (SI16)ndata );				break;
		case DFNTAG_HIDAMAGE:		applyTo->SetHiDamage( (SI16)ndata );		break;
		case DFNTAG_HEAT:			applyTo->HeatDamage( ndata != 0 );			break;
		case DFNTAG_ID:				applyTo->SetID( (UI16)ndata );				break;
		case DFNTAG_INTELLIGENCE:	applyTo->SetIntelligence( (SI16)ndata );	break;
		case DFNTAG_INTADD:			applyTo->Intelligence2( (SI16)ndata );		break;
		case DFNTAG_LODAMAGE:		applyTo->SetLoDamage( (SI16)ndata );		break;
		case DFNTAG_LAYER:			applyTo->SetLayer( (SI08)ndata );			break;
		case DFNTAG_LIGHT:			applyTo->LightDamage( ndata != 0 );			break;
		case DFNTAG_LIGHTNING:		applyTo->LightningDamage( ndata != 0 );		break;
		case DFNTAG_MAXHP:			applyTo->SetMaxHP( (SI16)ndata );			break;
		case DFNTAG_MOVABLE:		applyTo->SetMovable( (SI08)ndata );			break;
		case DFNTAG_MORE:			applyTo->SetMore( ndata );					break;
		case DFNTAG_MORE2:			applyTo->SetMoreB( ndata );					break;
		case DFNTAG_MOREX:			applyTo->SetMoreX( ndata );					break;
		case DFNTAG_MOREY:			applyTo->SetMoreY( ndata );					break;
		case DFNTAG_MOREZ:			applyTo->SetMoreZ( ndata );					break;
		case DFNTAG_NAME:			applyTo->SetName( cdata );					break;
		case DFNTAG_NAME2:			applyTo->SetName2( cdata );					break;
		case DFNTAG_NEWBIE:			applyTo->SetNewbie( true );					break;
		case DFNTAG_OFFSPELL:		applyTo->SetOffSpell( (SI08)ndata );		break;
		case DFNTAG_POISONED:		applyTo->SetPoisoned( (UI08)ndata );		break;
		case DFNTAG_PILEABLE:		applyTo->SetPileable( ndata != 0 );			break;
		case DFNTAG_PRIV:			applyTo->SetPriv( (UI08)ndata );			break;
		case DFNTAG_RANK:
									applyTo->SetRank( (SI08)ndata );
									if( applyTo->GetRank() <= 0 ) 
										applyTo->SetRank( 10 );
									break;
		case DFNTAG_RACE:			applyTo->SetRace( (UI16)ndata );			break;
		case DFNTAG_RESTOCK:		applyTo->SetRestock( (UI16)ndata );			break;
		case DFNTAG_RAIN:			applyTo->RainDamage( ndata != 0 );			break;
		case DFNTAG_SK_MADE:		applyTo->SetMadeWith( (SI08)ndata );		break;
		case DFNTAG_SPD:			applyTo->SetSpeed( (UI08)ndata );			break;
		case DFNTAG_STRENGTH:		applyTo->SetStrength( (SI16)ndata );		break;
		case DFNTAG_STRADD:			applyTo->Strength2( (SI16)ndata );			break;
		case DFNTAG_SNOW:			applyTo->SnowDamage( ndata != 0 );			break;
		case DFNTAG_SCRIPT:			applyTo->SetScriptTrigger( (UI16)ndata );	break;
		case DFNTAG_SPAWNOBJ:		applyTo->SetDesc( cdata );				break;
		case DFNTAG_SPAWNOBJLIST:
									applyTo->SetDesc( cdata );
									applyTo->SetSpawnerList( true );
									break;
		case DFNTAG_TYPE:			applyTo->SetType( (UI08)ndata );			break;
		case DFNTAG_TYPE2:			applyTo->SetType2( (UI08)ndata );			break;
		case DFNTAG_VISIBLE:		applyTo->SetVisible( (SI08)ndata );			break;
		case DFNTAG_VALUE:			applyTo->SetValue( ndata );					break;
		case DFNTAG_WEIGHT:			applyTo->SetWeight( ndata );				break;
		case DFNTAG_WIPE:			applyTo->SetWipeable( ndata != 0 );			break;
		case DFNTAG_NOTES:
		case DFNTAG_CATEGORY:
			break;
		default:					Console.Warning( 2, "Unknown items dfn tag %i %s %i %i ", tag, cdata, ndata, odata );	break;
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *CreateItem( cSocket *s, string name, UI08 worldNumber )
//|	Programmer	-	Abaddon
//| Date		-	28th September, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Does the minimum required to spawn an item from the scripts
//|					assigning a world number and setting the serial
//o---------------------------------------------------------------------------o
CItem *cItem::CreateItem( cSocket *s, std::string name, UI08 worldNumber )
{
	CTile tile;
	char itemSect[512];
 
	if( cwmWorldState->ServerData()->ServerScriptSectionHeader() )
		sprintf( itemSect, "ITEM %s", name.c_str() );
	else
		strcpy( itemSect, name.c_str() );
	ScriptSection *itemCreate = FileLookup->FindEntry( itemSect, items_def );
	if( itemCreate == NULL )
		return NULL;

	// Scan through for an itemlist, if we make an itemlist, then we'll call our random item func and return instantly
	// This way, we skip over some code we double up like an itemcount and itemcount2 increase

	if( itemCreate->ItemListExist() )
	{
		CItem *iListMade = CreateRandomItem( s, itemCreate->ItemListData(), true, worldNumber );
		if( iListMade != NULL )
			iListMade->WorldNumber( worldNumber );
		return iListMade;
	}
	ITEM iMadeOff;
	CItem *iMade = MemItemFree( iMadeOff, true );
	if( iMade == NULL )
		return NULL;

	iMade->SetID( 0x0915 );
	iMade->SetWipeable( false );
	iMade->SetAmount( 1 );		// obviously some things have no default val, and 0 is not a good number

	if( !ApplyItemSection( iMade, itemCreate ) )
		Console.Error( 2, "Trying to apply an item section failed" );

	Map->SeekTile( iMade->GetID(), &tile );
	if( tile.Stackable() )
		iMade->SetPileable( true );
	if( !iMade->GetMaxHP() && iMade->GetHP() ) 
		iMade->SetMaxHP( iMade->GetHP() );

	iMade->WorldNumber( worldNumber );
	return iMade;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *CreateScriptItem( cSocket *s, string name, bool nSpawned, UI08 worldNumber )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create an item based on its script entries
//|									
//|	Modification	-	04/22/2002	-	Added a check here to make sure that there 
//|									are no trailign spaces.
//o---------------------------------------------------------------------------o
CItem * cItem::CreateScriptItem( cSocket *s, std::string name, bool nSpawned, UI08 worldNumber )
{
	if( name[ name.size()-1]==' ' )
		name[ name.size()-1] = 0x00;
	CItem *iMade = CreateItem( s, name, worldNumber );
	if( iMade == NULL )
	{
		Console.Error( 2, "CreateScriptItem(): Bad script item %s (Item Not Found).\n", name.c_str() );
		return NULL;
	}
 
	if( s != NULL && !nSpawned )
		iMade->SetLocation( s->GetWord( 11 ), s->GetWord( 13 ), s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ) );

	if( iMade->GetCont() == NULL ) 
		MapRegion->AddItem( iMade );
	cScript *toGrab = Trigger->GetScript( iMade->GetScriptTrigger() );
	if( toGrab != NULL )
		toGrab->OnCreate( iMade );
	return iMade;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *cItem::CreateRandomItem( cSocket *s, char * sItemList, bool nSpawned )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create a random item from the scripts
//o---------------------------------------------------------------------------o
CItem *cItem::CreateRandomItem( cSocket *s, const char *sItemList, bool nSpawned, UI08 worldNumber )
{
	char sect[512];
	sprintf( sect, "ITEMLIST %s", sItemList );
	ScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList == NULL )
		return NULL;
	int i = ItemList->NumEntries();
	if( i == 0 )
		return NULL;
	const char *k = ItemList->MoveTo( RandomNum( 0, i - 1 ) );
	if( k != NULL )
	{
		CItem *iMade = CreateScriptItem( s, k, nSpawned, worldNumber );
		return iMade;
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * cItem::SpawnItem( cSocket *nSocket, CChar *ch, UI32 nAmount, const char *cName, bool nStackable, UI16 realItemID, UI16 realColour, bool nPack, bool nSend )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn an item
//o---------------------------------------------------------------------------o
CItem * cItem::SpawnItem( cSocket *nSocket, CChar *ch, UI32 nAmount, const char *cName, bool nStackable, UI16 realItemID, UI16 realColour, bool nPack, bool nSend )
{
	if( nAmount == 0 )
		return NULL;
	UI08 worldNumber = ch->WorldNumber();
	CItem *i = NULL;
	bool inpack = false;
	CItem *p = getPack( ch );
	//Auto-Stack code!
	if( nPack && nStackable && p != NULL )
	{
		for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
		{
			if( i != NULL && ( i->GetID() == realItemID ) && i->GetColour() == realColour )
			{
				if( i->GetAmount() + nAmount <= MAX_STACK )
				{
					i->SetAmount( i->GetAmount() + nAmount );
					inpack = true;
					RefreshItem( i );
					break;
				}
			}
		}
	}

	if( !nPack || ( nPack && !inpack ) )
	{
		ITEM iOff;
		i = MemItemFree( iOff, true );
		if( i == NULL ) 
			return NULL;

		i->WorldNumber( worldNumber );
		if( cName != NULL) 
			i->SetName( cName );
		i->SetID( realItemID );
		i->SetColour( realColour );
		if( nPack )
		{
			if( p != NULL )
			{
				i->SetCont( p );
				i->SetX( ( 50 + RandomNum( 0, 79 ) ) );
				i->SetY( ( 50 + RandomNum( 0, 79 ) ) );
				i->SetZ( 9 );
			}
			else
				i->SetLocation( ch );
		}
		i->SetAmount( nAmount );
		i->SetPileable( nStackable );
		i->SetDecayable( true );
		GetScriptItemSetting( i );
		ch->SetMaking( i->GetSerial() );
		RefreshItem( i );
	}
	
	if( nSend && nSocket != NULL ) 
	{ 
	   // added to fix weight bug - sereg 
	   //as i supposed the weight of the total package had been added before
	   UI16 sum_nAmount = i->GetAmount(); 
	   i->SetAmount( nAmount ); 
	   statwindow( nSocket, ch ); 
	   i->SetAmount( sum_nAmount ); 
	} 

	cScript *toGrab = Trigger->GetScript( i->GetScriptTrigger() );
	if( toGrab != NULL )
		toGrab->OnCreate( i );
	return i;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *cItem::SpawnMulti( CChar *ch, char* cName, UI16 realItemId )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn a Multi
//o---------------------------------------------------------------------------o
CMultiObj *cItem::SpawnMulti( CChar *ch, const char *cName, UI16 realItemID )
{
	ITEM cOff;
	CItem *c = MemItemFree( cOff, true, 1 );
	if( c == NULL ) 
		return NULL;

	CMultiObj *i = static_cast< CMultiObj * >(c);
	i->WorldNumber( ch->WorldNumber() );
	if( cName != NULL )
		i->SetName( cName );
	i->SetID( realItemID );
	GetScriptItemSetting( i );
	ch->SetMaking( c->GetSerial() );
	RefreshItem( i );
	return i;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::GetScriptItemSetting( CItem *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get item settings from scripts
//o---------------------------------------------------------------------------o
void cItem::GetScriptItemSetting( CItem *c )
{
	if( c == NULL )
		return;

	char buff[512];
	sprintf( buff, "x%x%x", c->GetID( 1 ), c->GetID( 2 ) );
	Script *tScript = FileLookup->FindScript( buff, hard_items_def );
	if( tScript == NULL )
		return;
	ScriptSection *toFind = tScript->FindEntrySubStr( buff );
	if( toFind == NULL )
		return;
	ApplyItemSection( c, toFind );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * cItem::SpawnItemToPack( cSocket *s, CChar *ch, string name, bool nDigging )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn an item inside a pack
//o---------------------------------------------------------------------------o
CItem * cItem::SpawnItemToPack( cSocket *s, CChar *mChar, std::string name, bool nDigging )
{
	CItem *p = getPack( mChar );
	if( p == NULL ) 
	{
		Console.Warning( 2, "SpawnItemToPack(): Character %s(%i) has no pack, attempting creation of new pack.....\n", mChar->GetName(), mChar->GetSerial() );
		CItem *newPack = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x0E75, 0, false, false );
		if( newPack != NULL )
		{
			mChar->SetPackItem( newPack );
			newPack->SetLayer( 0x15 );
			newPack->SetCont( mChar );
			newPack->SetType( 1 );
			newPack->SetDye( true );
			RefreshItem( newPack );
			p = newPack;
			Console.Print( "SpawnItemToPack(): Pack creation successful for Character %s(%i).\n", mChar->GetName(), mChar->GetSerial() );
		}
		else
		{
			Console.Error( 2, "SpawnItemToPack(): Pack creation failed for Character %s(%i).\n", mChar->GetName(), mChar->GetSerial() );
			return NULL;
		}
	}

	CItem *c = CreateScriptItem( s, name, false, p->WorldNumber() );

	if( c == NULL )
		return NULL;

	c->SetCont( p );
	c->SetX( 50 + RandomNum( 0, 79 ) );
	c->SetY( 50 + RandomNum( 0, 79 ) );
	c->SetZ( 9 );
	// We should use the value the DFNs give us
	//c->SetMovable( 1 ); 

	if( nDigging ) 
	{
		if( c->GetValue() != 0 ) 
			c->SetValue( RandomNum( 1, c->GetValue() ) );
		if( c->GetHP() != 0 ) 
			c->SetHP( RandomNum( 1, c->GetHP() ) );
	}
	GetScriptItemSetting( c );
	RefreshItem( c );
	if( s != NULL )
		statwindow( s, mChar );
	return c;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * cItem::SpawnItemToPack( cSocket *s, CChar *ch, int nItem, bool nDigging )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn an item inside a pack
//o---------------------------------------------------------------------------o
CItem * cItem::SpawnItemToPack( cSocket *s, CChar *mChar, int nItem, bool nDigging )
{
	char temp[128];
	sprintf( temp, "%i", nItem );
	return SpawnItemToPack( s, mChar, temp, nDigging );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cItem::DecayItem( CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Cause items to decay when left on the ground
//o---------------------------------------------------------------------------o
bool cItem::DecayItem( CItem *i ) 
{
	if( i == NULL || i->isFree() )
		return false;
	if( !i->WillDecay() )
		return false;

	if( i->GetDecayTime() > uiCurrentTime && !overflow )
		return false;

	bool retVal = false;
	if( i->GetCont() == NULL )	// Are we on the ground?
	{  // decaytime = 5 minutes, * 60 secs per min, * clocks_per_sec
		SI32 decayTimer = BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) ));
		if( i->GetDecayTime() == 0 ) 
		{
			i->SetDecayTime( decayTimer );
			return false;
		}
		
		// Multis
		if( !i->IsFieldSpell() && !i->isCorpse() ) // Gives fieldspells a chance to decay in multis
		{
			if( i->GetMultiObj() == NULL )
			{
				CMultiObj *multi = findMulti( i );
				if( multi != NULL )
				{
					if( multi->GetMore( 4 ) == 0 )
					{
						i->SetDecayTime( decayTimer );
						i->SetMulti( multi );
						return false;
					}
				}
			} 
			else	// in a house, therefore... no decay
			{					
				i->SetDecayTime( decayTimer );
				return false;
			}
		}

		if( i->isCorpse() && i->GetOwnerObj() != NULL && i->GetMore( 4 ) )
		{
			UI32 preservebody = 0;
			for( CItem *j = i->FirstItemObj(); !i->FinishedItems(); j = i->NextItemObj() )
			{
				if( j != NULL )
					preservebody++;
			}
			if( preservebody > 1  )
			{
				i->SetMore( i->GetMore( 4 ) - 1, 4 );
				i->SetDecayTime( decayTimer );
				return false;
			}
		}
		if( ( !i->isCorpse() && i->GetType() == 1 ) || ( i->isCorpse() && ( i->GetOwnerObj() != NULL || !cwmWorldState->ServerData()->GetCorpseLootDecay() ) ) )
		{
			for( CItem *io = i->FirstItemObj(); !i->FinishedItems(); io = i->NextItemObj() )
			{
                if( io != NULL )
				{
					io->SetCont( NULL );
					io->SetLocation( i );

					io->SetDecayTime( decayTimer );
					RefreshItem( io );
				}
			}
		}
		DeleItem( i );
		retVal = true;
	}
	return retVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::RespawnItem( ITEM i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Respawn an item
//o---------------------------------------------------------------------------o
void cItem::RespawnItem( CItem *i )
{
	if( i == NULL || i->isFree() )
		return;

	switch( i->GetType() )
	{
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 69:
	case 125:
		break;
	default:
		return;
	}

	int j, m, k;
	ITEM ci;

	HashBucketMulti< ITEM > * hashBucketI = NULL;
	HashBucketMulti< CHARACTER > * hashBucketC = NULL;
	for( int c = 0; c < i->GetAmount(); c++ )
	{
		if( i->GetGateTime() + ( c*i->GetMoreZ() * CLOCKS_PER_SEC) <= uiCurrentTime )
		{
			m = 0;
			switch( i->GetType() )
			{
			case 61:
				k = 0;
				hashBucketI = nspawnsp.GetBucket( (i->GetSerial())%HASHMAX );
				for( j = 0; j < static_cast<int>(hashBucketI->NumEntries()); j++ )
				{
					ci = hashBucketI->GetEntry( j );
					if( ci != INVALIDSERIAL )
					{
						if( items[ci].isFree() )
							continue;
						if( i == items[ci].GetSpawnObj() )
						{
							if( i != &items[ci] && items[ci].GetX() == i->GetX() && items[ci].GetY() == i->GetY() && items[ci].GetZ() == i->GetZ() )
							{
								k = 1;
								break;
							}
						}
					}
				}

				if( k == 0 )
				{
					if( i->GetGateTime() == 0 )
					{
						i->SetGateTime( BuildTimeValue( static_cast<R32>(RandomNum( i->GetMoreY() * 60, i->GetMoreZ() * 60 ) )) );
					}
					if( i->GetGateTime() <= uiCurrentTime || overflow )
					{
						const char *lDesc = i->GetDesc();
						if( lDesc[0] != 0 )	// not NULL terminated, so we can do something!
						{
							AddRespawnItem( i, lDesc, false, i->isSpawnerList() );
						}
						else if( i->GetMoreX() != 0 )
						{
							AddRespawnItem( i, i->GetMoreX(), false );
						}
						else
							break;
						i->SetGateTime( 0 );
					}
				}
				break;
			case 62:
			case 69:
			case 125:
				k = 0;

				hashBucketC = ncspawnsp.GetBucket( (i->GetSerial())%HASHMAX );
				for( j = 0; j < static_cast<int>(hashBucketC->NumEntries()); j++ )
				{
					ci = hashBucketC->GetEntry( j );
					if( ci != INVALIDSERIAL )
					{
						if( chars[ci].isFree() )
							continue;
						if( chars[ci].GetSpawnObj() == i )
							k++;
					}
				}

				if( k < i->GetAmount() )
				{
					if( i->GetGateTime() == 0 )
					{
						i->SetGateTime( BuildTimeValue( static_cast<R32>(RandomNum( i->GetMoreY() * 60, i->GetMoreZ() * 60 )) ) );
					}
					if( i->GetGateTime() <= uiCurrentTime || overflow )
					{
						const char *mDesc = i->GetDesc();
						if( mDesc[0] != 0 )	// not NULL terminated, so we can do something!
						{
							Npcs->SpawnNPC( i, mDesc, i->WorldNumber(), i->isSpawnerList() );
							i->SetGateTime( 0 );
						}
						else if( i->GetMoreX() != 0 )
						{
							char temp[512];
							sprintf( temp, "%i", i->GetMoreX() );
							Npcs->SpawnNPC( i, temp, i->WorldNumber(), false );
							i->SetGateTime( 0 );
						}
					}
				}
				break;
			case 63:
			case 64:
			case 65:
				CItem *getItem;
				getItem = NULL;
				for( getItem = i->FirstItemObj(); !i->FinishedItems(); getItem = i->NextItemObj() )
				{
					if( getItem != NULL )
					{
						if( !getItem->isFree() )
							m++;
					}
				}
				if( m < i->GetAmount() )
				{
					if( i->GetGateTime() == 0 )
					{
						i->SetGateTime( BuildTimeValue( static_cast<R32>(RandomNum( i->GetMoreY() * 60, i->GetMoreZ() * 60 )) ) );
					}
					if( i->GetGateTime() <= uiCurrentTime || overflow )
					{
						const char *nDesc = i->GetDesc();
						if( i->GetType() == 63 )
							i->SetType( 64 ); // Lock the container
						if( nDesc[0] != 0 )	// not NULL terminated, so we can do something!
						{
							AddRespawnItem( i, nDesc, true, i->isSpawnerList() );
							i->SetGateTime( 0 );
						}
						else if( i->GetMoreX() != 0 )
						{
							AddRespawnItem( i, i->GetMoreX(), true );
							i->SetGateTime( 0 );
						}
					}
				}
				break;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI08 cItem::PackType( UI08 id1, UI08 id2 )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the packtype based on the packs ID
//o---------------------------------------------------------------------------o
UI08 cItem::PackType( UI16 id )
{
	switch( id )
	{
	case 0x09A8: 
	case 0x09AA:
	case 0x09B0:	return 1;
	case 0x09A9:	return 2;
	case 0x09AB:	return 3;
	case 0x09B2:	return 4;
	case 0x0E3C:
	case 0x0E3D:
	case 0x0E3E:
	case 0x0E3F:
	case 0x0E7E:
	case 0x0E78:	return 2;
	case 0x0E76:
	case 0x0E79:
	case 0x0E7D:
	case 0x0E80:
	case 0x0E7A:	return 1;
	case 0x0E40:
	case 0x0E41:
	case 0x0E42:
	case 0x0E7C:
	case 0x0E43:	return 3;
	case 0x0E75:
	case 0x0E7F:
	case 0x0E83:
	case 0x0E77:	return 4;
	default:		return 0xFF;
	}
	return 0xFF;		// safety catch all (avoids compiler warnings)
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::AddRespawnItem( CItem *s, UI32 x, bool inCont )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Item spawning stuff
//o---------------------------------------------------------------------------o
void cItem::AddRespawnItem( CItem *s, UI32 x, bool inCont )
{
	if( s == NULL )
		return;
	char temp[128];
	sprintf( temp, "%i", x );
	AddRespawnItem( s, temp, inCont, false );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::AddRespawnItem( CItem *s, const char *x, bool inCont )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Item spawning stuff
//o---------------------------------------------------------------------------o
void cItem::AddRespawnItem( CItem *s, const char *x, bool inCont, bool randomItem = false )
{
	if( s == NULL || x == NULL )
		return;
	CItem *c = NULL;
	if( randomItem )
		c = CreateRandomItem( NULL, x, true, s->WorldNumber() );
	else
		c = CreateScriptItem( NULL, x, true, s->WorldNumber() );
	if( c == NULL )
		return;

	if( inCont )
		c->SetCont( s );
	else
		c->SetLocation( s );
	c->SetSpawn( s->GetSerial(), calcItemFromSer( c->GetSerial() ) );
	
	if( inCont )
	{
		CItem *z = NULL;
		if( c->GetSpawnObj() != NULL )
			z = (CItem *)c->GetSpawnObj();
		if( z != NULL )
		{
			UI08 k = PackType( z->GetID() );
			if( k == 0xFF )
			{
				Console.Warning( 2, " A non-container item was set as container spawner" );
				k = 1;
			}
			c->SetX( RandomNum( 0, 99 ) + 18 );
			c->SetZ( 9 );
			switch( k )
			{
			case 1:	c->SetY( ( RandomNum( 0, 49 ) ) + 50 );		break;
			case 2:	c->SetY( ( RandomNum( 0, 49 ) ) + 30 );		break;
			case 3:	c->SetY( ( RandomNum( 0, 39 ) ) + 100 );	break;
			case 4:
				c->SetY( ( RandomNum( 0, 79 ) ) + 60 );
				c->SetX( ( RandomNum( 0, 79 ) ) + 60 );
				break;
			default:	c->SetY( ( RandomNum( 0, 49 ) ) + 30 );
			}
		}
	}
	RefreshItem( c );
}

//o---------------------------------------------------------------------------o
//|		Function	-	menuAddItem( cSocket *s, std::string item )
//|		Programmer	-	Zane
//|		Date		-	January 31, 2003
//o---------------------------------------------------------------------------o
//|		Purpose		-	Create an item selected from the Add menu
//o---------------------------------------------------------------------------o
void cItem::menuAddItem( cSocket *s, std::string item )
{
	CChar *mChar = s->CurrcharObj();
	CItem *c = Items->SpawnItemToPack( s, mChar, item, false );
	if( c == NULL )
		return;
	c->SetName2( c->GetName() );
	c->SetCreator( INVALIDSERIAL );
	c->SetMovable( 1 );
	statwindow( s, mChar );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::GlowItem( CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle glowing items
//o---------------------------------------------------------------------------o
void cItem::GlowItem( CItem *i )
{
	if( i->GetGlow() > 0 )
	{
		CItem *j = calcItemObjFromSer( i->GetGlow() );
		if( j == NULL )
			return;

		//j->SetLayer( i->GetLayer() ); // copy layer information of the glowing item to the invisible light emitting object

		cBaseObject *getCont = i->GetCont();
		if( getCont == NULL ) // On the ground
		{
			j->SetCont( NULL );
			j->SetDir( 29 );
			j->SetLocation( i );
		}
		else if( getCont->GetObjType() == OT_ITEM ) // In a pack
		{
			MapRegion->RemoveItem( j );
			j->SetCont( getCont );
			j->SetDir( 99 );  // gives no light in backpacks
			j->SetX( i->GetX() );
			j->SetY( i->GetY() );
			j->SetZ( i->GetZ() );
		}
		else // Equipped
		{
			CChar *s = (CChar *)getCont;
			if( s != NULL )
			{
				MapRegion->RemoveItem( j );
				j->SetCont( getCont );
				j->SetX( s->GetX() );
				j->SetY( s->GetY() );
				j->SetZ( s->GetZ()+4 );
				if( isOnline( s ) )
					j->SetDir( 29 );
				else
					j->SetDir( 99 );
			}
		}
		RefreshItem( j );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::CheckEquipment( CChar *p )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check equipment of character
//o---------------------------------------------------------------------------o
void cItem::CheckEquipment( CChar *p )
{
	if( p == NULL ) 
		return;

	char *itemname = NULL;
	char temp[512];

	cSocket *pSock = calcSocketObjFromChar( p );
	if( pSock == NULL ) 
		return;

	const SI16 StrengthToCompare = p->GetStrength();
	for( CItem *i = p->FirstItem(); !p->FinishedItems(); i = p->NextItem() )
	{
		if( i == NULL )
			continue;
		if( i->GetStrength() > StrengthToCompare )//if strength required > character's strength
		{
			itemname = (char *)i->GetName();
			if( itemname[0] == '#' ) 
				getTileName( i, temp );
			else 
				strcpy( temp, itemname );

			i->SetCont( NULL );
			i->SetLocation( p );
							
			Network->PushConn();
			for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
			{
				if( charInRange( p, tSock->CurrcharObj() ) )
					wornItems( tSock, p );
			}
			Network->PopConn();
			RefreshItem( i );
			sysmessage( pSock, 1604, temp );
			itemSound( pSock, i );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void StoreItemRandomValue( CItem *i, UI08 tmpreg )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Remember an items value
//o---------------------------------------------------------------------------o
void StoreItemRandomValue( CItem *i, UI08 tmpreg )
{
	if( i->GetGood() < 0 ) 
		return;
	if( tmpreg == 0xFF )
	{
		cBaseObject *getLastCont = i->GetCont();
		if( getLastCont != NULL )
			tmpreg = calcRegionFromXY( getLastCont->GetX(), getLastCont->GetY(), getLastCont->WorldNumber() );
		if( tmpreg == 0xFF )
			return;
	}
	
	SI32 min = region[tmpreg]->GetGoodRnd1( static_cast<UI08>(i->GetGood()) );
	SI32 max = region[tmpreg]->GetGoodRnd2( static_cast<UI08>(i->GetGood()) );
	
	if( max != 0 || min != 0 )
		i->SetRndValueRate( (SI32) RandomNum( min, max ) );
}
