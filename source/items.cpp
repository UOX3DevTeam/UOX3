#include "uox3.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "cSpawnRegion.h"
#include "ssection.h"
#include "trigger.h"
#include "scriptc.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "ObjectFactory.h"

#undef DBGFILE
#define DBGFILE "items.cpp"

namespace UOX
{

cItem *Items = NULL;

ItemTypes FindItemTypeFromTag( UString strToFind );

bool ApplySpawnItemSection( CSpawnItem *applyTo, DFNTAGS tag, UI32 ndata, UI32 odata, UString cdata )
{
	if( !ValidateObject( applyTo ) )
		return false;

	switch( tag )
	{
	case DFNTAG_SPAWNOBJLIST:				applyTo->IsSectionAList( true );
	case DFNTAG_SPAWNOBJ:
											applyTo->SetSpawnSection( cdata );
											return true;
	case DFNTAG_INTERVAL:
											applyTo->SetInterval( 0, static_cast<UI08>(ndata) );
											applyTo->SetInterval( 1, static_cast<UI08>(odata) );
											return true;
	default:								break;
	}
	return false;
}

UI16 addRandomColor( std::string colorlist );
//o---------------------------------------------------------------------------o
//|	Function	-	bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load items from the script sections to the array
//o---------------------------------------------------------------------------o
bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply )
{
	if( toApply == NULL || !ValidateObject( applyTo ) )
		return false;
	DFNTAGS tag = DFNTAG_COUNTOFTAGS;
	UString cdata;
	UI32 ndata = INVALIDSERIAL, odata = INVALIDSERIAL;
	bool isSpawner = (applyTo->GetObjType() == OT_SPAWNER);
	for( tag = toApply->FirstTag(); !toApply->AtEndTags(); tag = toApply->NextTag() )
	{
		cdata = toApply->GrabData( ndata, odata );

		if( isSpawner && ApplySpawnItemSection( static_cast<CSpawnItem *>(applyTo), tag, ndata, odata, cdata ) )
			continue;

		switch( tag )
		{
			case DFNTAG_AMOUNT:			
										if( ndata > 0 )
											applyTo->SetAmount( ndata );					
										break;
			case DFNTAG_ATT:			applyTo->SetLoDamage( static_cast<SI16>(ndata) );
										applyTo->SetHiDamage( static_cast<SI16>(odata) ); 
										break;
			case DFNTAG_AC:				applyTo->SetArmourClass( static_cast<UI08>(ndata) );	break;
			case DFNTAG_CREATOR:		applyTo->SetCreator( ndata );							break;
			case DFNTAG_COLOUR:			applyTo->SetColour( static_cast<UI16>(ndata) );			break;
			case DFNTAG_COLOURLIST:		applyTo->SetColour( addRandomColor( cdata ) );			break;
			case DFNTAG_CORPSE:			applyTo->SetCorpse( ndata != 0 )		;				break;
			case DFNTAG_COLD:			applyTo->SetWeatherDamage( COLD, ndata != 0 );			break;
			case DFNTAG_DAMAGE:			applyTo->SetLoDamage( static_cast<SI16>(ndata) );
										applyTo->SetHiDamage( static_cast<SI16>(odata) );
										break;
			case DFNTAG_DEF:			applyTo->SetDef( static_cast<UI16>(RandomNum( ndata, odata )) );	break;
			case DFNTAG_DEX:			applyTo->SetDexterity( static_cast<SI16>(RandomNum( ndata, odata )) );	break;
			case DFNTAG_DEXADD:			applyTo->SetDexterity2( static_cast<SI16>(ndata) );					break;
			case DFNTAG_DIR:			
				{
										UString UTag = cdata.upper();
										if( UTag == "NE" )
											applyTo->SetDir( 1 );
										else if( UTag == "E" )
											applyTo->SetDir( 2 );
										else if( UTag == "SE" )
											applyTo->SetDir( 3 );
										else if( UTag == "S" )
											applyTo->SetDir( 4 );
										else if( UTag == "SW" )
											applyTo->SetDir( 5 );
										else if( UTag == "W" )
											applyTo->SetDir( 6 );
										else if( UTag == "NW" )
											applyTo->SetDir( 7 );
										else if( UTag == "N" )
											applyTo->SetDir( 0 );
				}
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
			case DFNTAG_DOORFLAG:		break;
			case DFNTAG_FAME:			applyTo->SetFame( static_cast<SI16>(ndata) );			break;
			case DFNTAG_GOOD:			applyTo->SetGood( static_cast< SI16 >(ndata) );					break;
			case DFNTAG_GLOW:			applyTo->SetGlow( ndata );					break;
			case DFNTAG_GLOWBC:			applyTo->SetGlowColour( static_cast<UI16>(ndata) );		break;
			case DFNTAG_GLOWTYPE:		applyTo->SetGlowEffect( static_cast<UI08>(ndata) );		break;
			case DFNTAG_GET:
			{
										ScriptSection *toFind = FileLookup->FindEntry( cdata, items_def );
										ApplyItemSection( applyTo, toFind );
			}
										break;
			case DFNTAG_HP:				applyTo->SetHP( static_cast<SI16>(ndata) );				break;
			case DFNTAG_HIDAMAGE:		applyTo->SetHiDamage( static_cast<SI16>(ndata) );		break;
			case DFNTAG_HEAT:			applyTo->SetWeatherDamage( HEAT, ndata != 0 );			break;
			case DFNTAG_ID:				applyTo->SetID( static_cast<UI16>(ndata) );				break;
			case DFNTAG_INTELLIGENCE:	applyTo->SetIntelligence( static_cast<SI16>(ndata) );	break;
			case DFNTAG_INTADD:			applyTo->SetIntelligence2( static_cast<SI16>(ndata) );	break;
			case DFNTAG_LODAMAGE:		applyTo->SetLoDamage( static_cast<SI16>(ndata) );		break;
			case DFNTAG_LAYER:			applyTo->SetLayer( static_cast<SI08>(ndata) );			break;
			case DFNTAG_LIGHT:			applyTo->SetWeatherDamage( LIGHT, ndata != 0 );			break;
			case DFNTAG_LIGHTNING:		applyTo->SetWeatherDamage( LIGHTNING, ndata != 0 );		break;
			case DFNTAG_MAXHP:			applyTo->SetMaxHP( static_cast<SI16>(ndata) );			break;
			case DFNTAG_MOVABLE:		applyTo->SetMovable( static_cast<SI08>(ndata) );		break;
			case DFNTAG_MORE:			applyTo->SetTempVar( CITV_MORE, ndata );								break;
			case DFNTAG_MORE2:			applyTo->SetTempVar( CITV_MOREB, ndata );								break;
			case DFNTAG_MOREX:			applyTo->SetTempVar( CITV_MOREX, ndata );								break;
			case DFNTAG_MOREY:			applyTo->SetTempVar( CITV_MOREY, ndata );								break;
			case DFNTAG_MOREZ:			applyTo->SetTempVar( CITV_MOREZ, ndata );								break;
			case DFNTAG_NAME:			applyTo->SetName( cdata );								break;
			case DFNTAG_NAME2:			applyTo->SetName2( cdata.c_str() );						break;
			case DFNTAG_NEWBIE:			applyTo->SetNewbie( true );								break;
			case DFNTAG_OFFSPELL:		applyTo->SetOffSpell( static_cast<SI08>(ndata) );		break;
			case DFNTAG_POISONED:		applyTo->SetPoisoned( static_cast<UI08>(ndata) );		break;
			case DFNTAG_PILEABLE:		applyTo->SetPileable( ndata != 0 );						break;
			case DFNTAG_PRIV:			applyTo->SetPriv( static_cast<UI08>(ndata) );			break;
			case DFNTAG_RANK:
										applyTo->SetRank( static_cast<SI08>(ndata) );
										if( applyTo->GetRank() <= 0 ) 
											applyTo->SetRank( 10 );
										break;
			case DFNTAG_RACE:			applyTo->SetRace( static_cast<UI16>(ndata) );			break;
			case DFNTAG_RESTOCK:		applyTo->SetRestock( static_cast<UI16>(ndata) );		break;
			case DFNTAG_RAIN:			applyTo->SetWeatherDamage( RAIN, ndata != 0 );			break;
			case DFNTAG_SK_MADE:		applyTo->SetMadeWith( static_cast<SI08>(ndata) );		break;
			case DFNTAG_SPD:			applyTo->SetSpeed( static_cast<UI08>(ndata) );			break;
			case DFNTAG_STRENGTH:		applyTo->SetStrength( static_cast<SI16>(ndata) );		break;
			case DFNTAG_STRADD:			applyTo->SetStrength2( static_cast<SI16>(ndata) );			break;
			case DFNTAG_SNOW:			applyTo->SetWeatherDamage( SNOW, ndata != 0 );			break;
			case DFNTAG_SCRIPT:			applyTo->SetScriptTrigger( static_cast<UI16>(ndata) );	break;
			case DFNTAG_TYPE:			
										ItemTypes iType;
										iType = FindItemTypeFromTag( cdata );
										if( iType == IT_COUNT )
											iType = static_cast<ItemTypes>(ndata);
										if( iType < IT_COUNT )
											applyTo->SetType( iType );
										break;
			case DFNTAG_VISIBLE:		applyTo->SetVisible( (VisibleTypes)ndata );		break;
			case DFNTAG_VALUE:
										if( !cdata.empty() )
										{
											if( cdata.sectionCount( " " ) != 0 )
											{
												applyTo->SetBuyValue(  cdata.section( " ", 0, 0 ).stripWhiteSpace().toULong() );
												applyTo->SetSellValue( cdata.section( " ", 1, 1 ).stripWhiteSpace().toULong() );
												break;
											}
										}
										applyTo->SetBuyValue( ndata );
										applyTo->SetSellValue( (ndata / 2) );
										break;
			case DFNTAG_WEIGHT:			applyTo->SetWeight( ndata );				break;
			case DFNTAG_WIPE:			applyTo->SetWipeable( ndata != 0 );			break;
			case DFNTAG_ADDMENUITEM:
				Console.Print(cdata.c_str());
				break;
			case DFNTAG_SPAWNOBJ:
			case DFNTAG_SPAWNOBJLIST:
			case DFNTAG_NOTES:
			case DFNTAG_CATEGORY:
			case DFNTAG_TYPE2:
				break;
			default:					Console.Warning( 2, "Unknown items dfn tag %i %s %i %i ", tag, cdata.c_str(), ndata, odata );	break;
		}
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *CreateItem( cSocket *mSock, CChar *mChar, UI16 iID, UI32 iAmount, UI16 iColour, bool inPack )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a basic item and gives it an ID, Colour, and amount,
//|							also will automatically look for an entry in harditems.dfn
//|							and set its location (be it in a pack or on the ground).
//o--------------------------------------------------------------------------o
CItem * cItem::CreateItem( cSocket *mSock, CChar *mChar, UI16 iID, UI32 iAmount, UI16 iColour, ObjectType itemType, bool inPack )
{
	if( inPack && !ValidateObject( mChar->GetPackItem() ) )
	{
		Console.Warning( 2, "CreateItem(): Character %s(0x%X) has no pack, item creation aborted.", mChar->GetName().c_str(), mChar->GetSerial() );
		return NULL;
	}

	CItem *iCreated = CreateBaseItem( mChar->WorldNumber(), itemType );
	if( iCreated == NULL )
		return NULL;

	if( iID != 0x0000 )
		iCreated->SetID( iID );
	if( iColour != 0x0000 )
		iCreated->SetColour( iColour );

	iCreated->SetDecayable( false );

	GetScriptItemSettings( iCreated );

	if( iAmount > 1 )
	{
		iCreated->SetPileable( true );
		iCreated->SetAmount( iAmount );
	}

	return PlaceItem( mSock, mChar, iCreated, inPack );
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *CreateScriptItem( cSocket *mSock, CChar *mChar, std::string item, UI32 iAmount, bool inPack )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a script item, gives it an amount, and sets
//|							 its location (be it in a pack or on the ground).
//o--------------------------------------------------------------------------o
CItem * cItem::CreateScriptItem( cSocket *mSock, CChar *mChar, std::string item, UI32 iAmount, ObjectType itemType, bool inPack )
{
	if( inPack && !ValidateObject( mChar->GetPackItem() ) )
	{
		Console.Warning( 2, "CreateScriptItem(): Character %s(0x%X) has no pack, item creation aborted.", mChar->GetName().c_str(), mChar->GetSerial() );
		return NULL;
	}

	CItem *iCreated = CreateBaseScriptItem( item, mChar->WorldNumber(), itemType );
	if( iCreated == NULL )
		return NULL;

	if( iAmount > 0 && iCreated->isPileable() )
		iCreated->SetAmount( iAmount );

	return PlaceItem( mSock, mChar, iCreated, inPack );
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *CreateRandomItem( cSocket *mSock, DEFINITIONCATEGORIES sourceDFN, std::string itemList )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a random item from an itemlist in specified dfn file,
//|						gives it a random buy/sell value, and places it
//o--------------------------------------------------------------------------o
CItem *cItem::CreateRandomItem( cSocket *mSock, std::string itemList )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return NULL;

	CItem *iCreated = CreateRandomItem( itemList, mChar->WorldNumber() );
	if( iCreated != NULL )
	{
		if( iCreated->GetBuyValue() != 0 )
		{
			iCreated->SetBuyValue( RandomNum( static_cast<UI32>(1), iCreated->GetBuyValue() ) );
			iCreated->SetSellValue( static_cast<UI32>(iCreated->GetBuyValue() / 2) );
		}
		if( iCreated->GetHP() != 0 ) 
			iCreated->SetHP( static_cast<SI16>(RandomNum( static_cast<SI16>(1), iCreated->GetHP() )) );
	}
	return PlaceItem( mSock, mChar, iCreated, true );
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *CreateRandomItem( std::string sItemList, DEFINITIONCATEGORIES sourceDFN, UI08 worldNum )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a random item from an itemlist in specified dfn file
//o--------------------------------------------------------------------------o
CItem *cItem::CreateRandomItem( std::string sItemList, UI08 worldNum )
{
	CItem * iCreated	= NULL;
	UString sect		= "ITEMLIST " + sItemList;
	sect				= sect.stripWhiteSpace();
	ScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList != NULL )
	{
		size_t i = ItemList->NumEntries();
		if( i > 0 )
		{
			UString k = ItemList->MoveTo( RandomNum( static_cast< size_t >(0), i - 1 ) );
			if( !k.empty() )
			{
				if( k.upper() == "ITEMLIST" )
					iCreated = CreateRandomItem( ItemList->GrabData(), worldNum );
				else
					iCreated = CreateBaseScriptItem( k, worldNum );
			}
		}
	}
	return iCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CMultiObj *CreateMulti( CChar *mChar, std::string cName, UI16 iID, bool isBoat )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a multi, and looks for an entry in harditems.dfn
//o--------------------------------------------------------------------------o
CMultiObj * cItem::CreateMulti( CChar *mChar, std::string cName, UI16 iID, bool isBoat )
{
	CMultiObj *mCreated = static_cast< CMultiObj * >(ObjectFactory::getSingleton().CreateObject( (isBoat) ? OT_BOAT : OT_MULTI ));
	if( mCreated == NULL ) 
		return NULL;

	mCreated->SetID( iID );
	GetScriptItemSettings( mCreated );
	mCreated->WorldNumber( mChar->WorldNumber() );
	mCreated->SetDecayable( false );
	if( !cName.empty() )
		mCreated->SetName( cName );

	return mCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *CreateBaseItem( UI08 worldNum, ObjectType itemType )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a basic item
//o--------------------------------------------------------------------------o
CItem * cItem::CreateBaseItem( UI08 worldNum, ObjectType itemType )
{
	if( itemType != OT_ITEM && itemType != OT_SPAWNER )
		return NULL;

	CItem *iCreated = static_cast< CItem * >(ObjectFactory::getSingleton().CreateObject( itemType ));
	if( iCreated == NULL )
		return NULL;

	iCreated->SetID( 0x0915 );
	iCreated->SetWipeable( true );
	iCreated->WorldNumber( worldNum );

	return iCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CItem *CreateBaseScriptItem( std::string item, UI08 worldNum )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a basic item from the scripts
//o--------------------------------------------------------------------------o
CItem * cItem::CreateBaseScriptItem( std::string item, UI08 worldNum, ObjectType itemType )
{
	UString ourItem( item );
	ourItem						= ourItem.stripWhiteSpace();
	ScriptSection *itemCreate	= FileLookup->FindEntry( ourItem, items_def );
	if( itemCreate == NULL )
	{
		Console.Error( 2, "CreateBaseScriptItem(): Bad script item %s (Item Not Found).", item.c_str() );
		return NULL;
	}

	CItem *iCreated = NULL;
	if( itemCreate->ItemListExist() )
		iCreated = CreateRandomItem( itemCreate->ItemListData(), worldNum );
	else
	{
		iCreated = CreateBaseItem( worldNum, itemType );
		if( iCreated == NULL )
			return NULL;

		iCreated->SetDecayable( true );

		if( !ApplyItemSection( iCreated, itemCreate ) )
			Console.Error( 2, "Trying to apply an item section failed" );
		
		if( !iCreated->GetMaxHP() && iCreated->GetHP() ) 
			iCreated->SetMaxHP( iCreated->GetHP() );

		cScript *toGrab = Trigger->GetScript( iCreated->GetScriptTrigger() );
		if( toGrab != NULL )
			toGrab->OnCreate( iCreated );
	}
	return iCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	GetScriptItemSettings( CItem *iCreated )
//|	Date			-	10/12/2003
//|	Developers		-	Unknown
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Grabs item entries from harditems.dfn
//o--------------------------------------------------------------------------o
void cItem::GetScriptItemSettings( CItem *iCreated )
{
	UString item = "0x" + UString::number( iCreated->GetID(), 16 );
	ScriptSection *toFind = FileLookup->FindEntrySubStr( item, hard_items_def );
	if( toFind != NULL )
		ApplyItemSection( iCreated, toFind );
}

CItem *autoStack( cSocket *mSock, CItem *iToStack, CItem *iPack );
//o--------------------------------------------------------------------------o
//|	Function		-	CItem *PlaceItem( cSocket *mSock, CChar *mChar, CItem *iCreated, bool inPack )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Places an item that was just created
//o--------------------------------------------------------------------------o
CItem * cItem::PlaceItem( cSocket *mSock, CChar *mChar, CItem *iCreated, bool inPack )
{
	if( inPack )
	{
		if( iCreated->isPileable() )
			iCreated = autoStack( mSock, iCreated, mChar->GetPackItem() );	// if it didn't stack, it's iCreated... if it did, then it's the stack!
		else
		{
			iCreated->SetCont( mChar->GetPackItem() );
			iCreated->PlaceInPack();
		}
		if( mSock != NULL )
			mSock->statwindow( mChar );
	}
	else
		iCreated->SetLocation( mChar );

	return iCreated;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cItem::DecayItem( CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Cause items to decay when left on the ground
//o---------------------------------------------------------------------------o
bool DecayItem( CItem *i, UI32 nextDecayItems ) 
{
	if( i->GetDecayTime() == 0 ) 
	{
		i->SetDecayTime( nextDecayItems );
		return false;
	}
	bool isCorpse = i->isCorpse();
		
	// Multis
	if( !i->IsFieldSpell() && !isCorpse ) // Gives fieldspells a chance to decay in multis
	{
		if( ValidateObject( i->GetMultiObj() ) )
		{				
			i->SetDecayTime( nextDecayItems );
			return false;
		}
	}

	if( i->IsContType() )
	{
		if( !isCorpse || ( isCorpse && ( ValidateObject( i->GetOwnerObj() ) || !cwmWorldState->ServerData()->CorpseLootDecay() ) ) )
		{
			CDataList< CItem * > *iCont = i->GetContainsList();
			for( CItem *io = iCont->First(); !iCont->Finished(); io = iCont->Next() )
			{
				if( ValidateObject( io ) )
				{
					io->SetCont( NULL );
					io->SetLocation( i );
					io->SetDecayTime( nextDecayItems );
				}
			}
		}
	}
	i->Delete();
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	PackTypes cItem::getPackType( CItem *i )
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the pack type based on ID
//o---------------------------------------------------------------------------o
PackTypes cItem::getPackType( CItem *i )
{
	PackTypes packType = PT_UNKNOWN;
	switch( i->GetID() )
	{
		case 0x2006:	// coffin
			packType = PT_COFFIN;
			break;
		case 0x0E75:	// backpack
		case 0x0E79:	// pouch
		case 0x09B0:	// pouch
			packType = PT_PACK;
			break;
		case 0x0E76:	// leather bag
			packType = PT_BAG;
			break;
		case 0x0E77:	// barrel
		case 0x0E7F:	// keg
		case 0x0E83:
		case 0x0FAE:	// barrel with lids
		case 0x1AD7:	// potion kegs
		case 0x1940:	// barrel with lids
			packType = PT_BARREL;
			break;
		case 0x0E7A:	// square basket
			packType = PT_SQBASKET;
			break;
		case 0x0990:	// round basket
		case 0x09AC:
		case 0x09B1:
			packType = PT_RBASKET;
			break;
		case 0x0E40:	// gold chest
		case 0x0E41:	// gold chest
			packType = PT_GCHEST;
			break;
		case 0x0E7D:	// wooden box
		case 0x09AA:	// wooden box
			packType = PT_WBOX;
			break;
		case 0x0E3C:	// large wooden crate
		case 0x0E3D:	// large wooden crate
		case 0x0E3E:	// small wooden create
		case 0x0E3F:	// small wooden crate
		case 0x0E7E:	// wooden crate
		case 0x09A9:	// small wooden crate
			packType = PT_CRATE;
			break;
		case 0x2AF8:	// Shopkeeper buy, sell and sold layers
			packType = PT_SHOPPACK;
			break;
		case 0x0A30:   // chest of drawers (fancy)
		case 0x0A38:   // chest of drawers (fancy)
			packType = PT_DRAWER;
			break;
		case 0x0E42:	// wooden chest
		case 0x0E43:	// wooden chest
			packType = PT_WCHEST;
			break;
		case 0x0E80:	// brass box
		case 0x09A8:	// metal box
			packType = PT_MBOX;
			break;
		case 0x0E7C:	// silver chest
		case 0x09AB:	// silver chest
			packType = PT_SCHEST;
			break;
		case 0x0A97:	// bookcase
		case 0x0A98:	// bookcase
		case 0x0A99:	// bookcase
		case 0x0A9A:	// bookcase
		case 0x0A9B:	// bookcase
		case 0x0A9C:	// bookcase
		case 0x0A9D:	// bookcase (empty)
		case 0x0A9E:	// bookcase (empty)
			packType = PT_BOOKCASE;
			break;
		case 0x0A4C:	// fancy armoire (open)
		case 0x0A4D:	// fancy armoire
		case 0x0A50:	// fancy armoire (open)
		case 0x0A51:	// fancy armoire
			packType = PT_FARMOIRE;
			break;
		case 0x0A4E:	// wooden armoire (open)
		case 0x0A4F:	// wooden armoire
		case 0x0A52:	// wooden armoire (open)
		case 0x0A53:	// wooden armoire
			packType = PT_WARMOIRE;
			break;
		case 0x0A2C:	// chest of drawers (wood)
		case 0x0A34:	// chest of drawers (wood)
		case 0x0A35:	// dresser
		case 0x0A3C:	// dresser
		case 0x0A3D:	// dresser
		case 0x0A44:	// dresser
			packType = PT_DRESSER;
			break;
		case 0x09B2:	// bank box ..OR.. backpack 2
			if( i->GetTempVar( CITV_MOREX ) == 1 )
				packType = PT_BANK;
			else
				packType = PT_PACK2;
			break;
		default:
			packType = PT_UNKNOWN;
			break;
	}
	return packType;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::AddRespawnItem( CItem *s, std::string x, bool inCont, bool randomItem )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Item spawning stuff
//o---------------------------------------------------------------------------o
void cItem::AddRespawnItem( CItem *s, std::string x, bool inCont, bool randomItem )
{
	if( !ValidateObject( s ) || x.empty() )
		return;
	CItem *c = NULL;
	if( randomItem )
		c = CreateRandomItem( x, s->WorldNumber() );
	else
		c = CreateBaseScriptItem( x, s->WorldNumber() );
	if( c == NULL )
		return;

	if( inCont )
		c->SetCont( s );
	else
		c->SetLocation( s );
	c->SetSpawn( s->GetSerial() );
	
	if( inCont )
	{
		CItem *spawnPack = static_cast<CItem *>(c->GetSpawnObj());
		if( ValidateObject( spawnPack ) )
		{
			c->SetX( RandomNum( 0, 99 ) + 18 );
			c->SetZ( 9 );
			switch( getPackType( spawnPack ) )
			{
				case PT_PACK:
				case PT_BAG:
				case PT_SQBASKET:
				case PT_RBASKET:
					c->SetY( ( RandomNum( 0, 49 ) ) + 50 );
					break;
				case PT_BOOKCASE:
				case PT_FARMOIRE:
				case PT_WARMOIRE:
				case PT_DRAWER:
				case PT_DRESSER:
					c->SetY( ( RandomNum( 0, 49 ) ) + 30 );
					break;
				case PT_MBOX:
				case PT_WBOX:
				case PT_BARREL:
					c->SetY( ( RandomNum( 0, 39 ) ) + 100 );
					break;
				case PT_CRATE:
				case PT_WCHEST:
				case PT_SCHEST:
				case PT_GCHEST:
					c->SetY( ( RandomNum( 0, 79 ) ) + 60 );
					c->SetX( ( RandomNum( 0, 79 ) ) + 60 );
					break;
				case PT_COFFIN:
				case PT_SHOPPACK:
				case PT_PACK2:
				case PT_BANK:
				case PT_UNKNOWN:
				default:	
					Console.Warning( 2, " A non-container item was set as container spawner" );
					break;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::GlowItem( CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle glowing items
//o---------------------------------------------------------------------------o
void cItem::GlowItem( CItem *i )
{
	if( i->GetGlow() != INVALIDSERIAL )
	{
		CItem *j = calcItemObjFromSer( i->GetGlow() );
		if( !ValidateObject( j ) )
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
	if( ValidateObject( p ) ) 
	{
		cSocket *pSock = calcSocketObjFromChar( p );
		if( pSock == NULL ) 
			return;

		const SI16 StrengthToCompare = p->GetStrength();
		for( CItem *i = p->FirstItem(); !p->FinishedItems(); i = p->NextItem() )
		{
			if( ValidateObject( i ) )
			{
				if( i->GetStrength() > StrengthToCompare )//if strength required > character's strength
				{
					std::string itemname;
					if( i->GetName() == "#" ) 
						getTileName( i, itemname );
					else 
						itemname = i->GetName();

					i->SetCont( NULL );
					i->SetLocation( p );
					
					SOCKLIST nearbyChars = FindNearbyPlayers( p );
					SOCKLIST_ITERATOR cIter;
					for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
					{
						p->SendWornItems( (*cIter) );
					}
					pSock->sysmessage( 1604, itemname.c_str() );
					Effects->itemSound( pSock, i );
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cItem::StoreItemRandomValue( CItem *i, cTownRegion *tReg )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Remember an items value
//o---------------------------------------------------------------------------o
void cItem::StoreItemRandomValue( CItem *i, cTownRegion *tReg )
{
	if( i->GetGood() < 0 ) 
		return;
	if( tReg == NULL )
	{
		cBaseObject *getLastCont = i->GetCont();
		if( getLastCont != NULL )
			tReg = calcRegionFromXY( getLastCont->GetX(), getLastCont->GetY(), getLastCont->WorldNumber() );
		if( tReg == NULL )
			return;
	}
	
	SI32 min = tReg->GetGoodRnd1( static_cast<UI08>(i->GetGood()) );
	SI32 max = tReg->GetGoodRnd2( static_cast<UI08>(i->GetGood()) );
	
	if( max != 0 || min != 0 )
		i->SetRndValueRate( RandomNum( min, max ) );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *cCommands::DupeItem( cSocket *s, CItem *i, UI32 amount )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Dupe selected item
//o---------------------------------------------------------------------------o
CItem *cItem::DupeItem( cSocket *s, CItem *i, UI32 amount )
{
	CChar *mChar		= s->CurrcharObj();
	cBaseObject *iCont	= i->GetCont();
	CItem *charPack		= mChar->GetPackItem();

	if( !ValidateObject( mChar ) || i->isCorpse() || ( !ValidateObject( iCont ) && !ValidateObject( charPack ) ) )
		return NULL;
	
	CItem *c = i->Dupe();
	if( c == NULL )
		return NULL;

	c->IncLocation( 2, 2 );
	if( ( !ValidateObject( iCont ) || iCont->GetObjType() != OT_ITEM ) && ValidateObject( charPack ) )
		c->SetCont( charPack );

	if( amount > MAX_STACK )
	{
		amount -= MAX_STACK;
		DupeItem( s, i, MAX_STACK );
	}
	c->SetAmount( amount );
	return c;
}

}
