#include "uox3.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "cSpawnRegion.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "scriptc.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "ObjectFactory.h"
#include "StringUtility.hpp"


cItem *Items = NULL;

ItemTypes FindItemTypeFromTag( const UString& strToFind );

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ApplySpawnItemSection( CSpawnItem *applyTo, const DFNTAGS tag, const SI32 ndata, const SI32 odata, const UString& cdata )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load item data from script sections and apply to spawner objects
//o-----------------------------------------------------------------------------------------------o
bool ApplySpawnItemSection( CSpawnItem *applyTo, const DFNTAGS tag, const SI32 ndata, const SI32 odata, const UString& cdata )
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

UI16 addRandomColor( const std::string& colorlist );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load items from the script sections to the array
//o-----------------------------------------------------------------------------------------------o
bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply )
{
	if( toApply == NULL || !ValidateObject( applyTo ) )
		return false;

	UString cdata;
	SI32 ndata = -1, odata = -1;
	bool isSpawner = (applyTo->GetObjType() == OT_SPAWNER);

	TAGMAPOBJECT customTag;
	UString customTagName;
	UString customTagStringValue;

	for( DFNTAGS tag = toApply->FirstTag(); !toApply->AtEndTags(); tag = toApply->NextTag() )
	{
		cdata = toApply->GrabData( ndata, odata );

		if( isSpawner && ApplySpawnItemSection( static_cast<CSpawnItem *>(applyTo), tag, ndata, odata, cdata ) )
			continue;

		switch( tag )
		{
			case DFNTAG_AMMO:
				applyTo->SetAmmoID( cdata.section( " ", 0, 0 ).stripWhiteSpace().toUShort() );
				if( cdata.sectionCount( " " ) > 0 )
					applyTo->SetAmmoHue( cdata.section( " ", 1, 1 ).stripWhiteSpace().toUShort() );
				break;
			case DFNTAG_AMMOFX:
				applyTo->SetAmmoFX( cdata.section( " ", 0, 0 ).stripWhiteSpace().toUShort() );
				if( cdata.sectionCount( " " ) > 0 )
				{
					applyTo->SetAmmoFXHue( cdata.section( " ", 1, 1 ).stripWhiteSpace().toUShort() );
					if( cdata.sectionCount( " " ) > 1 )
						applyTo->SetAmmoFXRender( cdata.section( " ", 2, 2 ).stripWhiteSpace().toUShort() );
				}
				break;
			case DFNTAG_AMOUNT:
				if( ndata && ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						UI16 rndAmount = static_cast<UI16>(RandomNum( ndata, odata ));
						applyTo->SetAmount( rndAmount );
					}
					else
					{
						applyTo->SetAmount( ndata );
					}
				}
				else
					Console.warning( format("Invalid data found in AMOUNT tag inside item script %s", cdata.c_str() ));
				break;
			case DFNTAG_DAMAGE:
			case DFNTAG_ATT:
				if( ndata && ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetLoDamage( static_cast<SI16>( ndata ) );
						applyTo->SetHiDamage( static_cast<SI16>( odata ) );
					}
					else
					{
						applyTo->SetLoDamage( static_cast<SI16>( ndata ) );
						applyTo->SetHiDamage( static_cast<SI16>( ndata ) );
					}
				}
				else
					Console.warning( format("Invalid data found in ATT/DAMAGE tag inside item script %s", cdata.c_str() ));
				break;
			case DFNTAG_AC:				applyTo->SetArmourClass( static_cast<UI08>(ndata) );	break;
			case DFNTAG_CREATOR:		applyTo->SetCreator( ndata );							break;
			case DFNTAG_COLOUR:			applyTo->SetColour( static_cast<UI16>(ndata) );			break;
			case DFNTAG_COLOURLIST:		applyTo->SetColour( addRandomColor( cdata ) );			break;
			case DFNTAG_CORPSE:			applyTo->SetCorpse( ndata != 0 )		;				break;
			case DFNTAG_COLD:			applyTo->SetWeatherDamage( COLD, ndata != 0 );			break;
			case DFNTAG_ELEMENTRESIST:
				if( cdata.sectionCount( " " ) == 3 )
				{
					applyTo->SetResist( cdata.section( " ", 0, 0 ).stripWhiteSpace().toUShort(), HEAT );
					applyTo->SetResist( cdata.section( " ", 1, 1 ).stripWhiteSpace().toUShort(), COLD );
					applyTo->SetResist( cdata.section( " ", 2, 2 ).stripWhiteSpace().toUShort(), LIGHTNING );
					applyTo->SetResist( cdata.section( " ", 3, 3 ).stripWhiteSpace().toUShort(), POISON );
				}
				break;
			case DFNTAG_DEF:
				if( ndata && ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>(RandomNum( ndata, odata )), PHYSICAL );
					}
					else
					{
						applyTo->SetResist( ndata, PHYSICAL );
					}
				}
				else
					Console.warning( format("Invalid data found in DEF tag inside item script %s", cdata.c_str() ));
				break;
			case DFNTAG_DEX:
				if( ndata && ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetDexterity( static_cast<SI16>(RandomNum( ndata, odata )) );
					}
					else
					{
						applyTo->SetDexterity( ndata );
					}
				}
				else
					Console.warning( format("Invalid data found in DEX tag inside item script %s", cdata.c_str() ));
				break;				
			case DFNTAG_DEXADD:			applyTo->SetDexterity2( static_cast<SI16>(ndata) );					break;
			case DFNTAG_DIR:			applyTo->SetDir( cdata.toByte() );			break;
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
			case DFNTAG_GOOD:			applyTo->SetGood( static_cast< SI16 >(ndata) );					break;
			case DFNTAG_GLOW:			applyTo->SetGlow( ndata );					break;
			case DFNTAG_GLOWBC:			applyTo->SetGlowColour( static_cast<UI16>(ndata) );		break;
			case DFNTAG_GLOWTYPE:		applyTo->SetGlowEffect( static_cast<UI08>(ndata) );		break;
			case DFNTAG_GET:
			{
				ScriptSection *toFind = FileLookup->FindEntry( cdata, items_def );
				if( toFind == NULL )
					Console.warning( format("Invalid script entry (%s) called with GET tag, item serial 0x%X", cdata.c_str(), applyTo->GetSerial()));
				else if( toFind == toApply )
					Console.warning( format("Infinite loop avoided with GET tag inside item script %s", cdata.c_str() ));
				else
					ApplyItemSection( applyTo, toFind );
			}
				break;
			case DFNTAG_HP:
				if( ndata && ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetHP( static_cast<SI16>(RandomNum( ndata, odata )) );
					}
					else
					{
						applyTo->SetHP( ndata );
					}
				}
				else
					Console.warning( format("Invalid data found in HP tag inside item script %s", cdata.c_str() ));
				break;
			case DFNTAG_HIDAMAGE:		applyTo->SetHiDamage( static_cast<SI16>(ndata) );		break;
			case DFNTAG_HEAT:			applyTo->SetWeatherDamage( HEAT, ndata != 0 );			break;
			case DFNTAG_ID:				applyTo->SetID( static_cast<UI16>(ndata) );				break;
			case DFNTAG_INTELLIGENCE:	applyTo->SetIntelligence( static_cast<SI16>(ndata) );	break;
			case DFNTAG_INTADD:			applyTo->SetIntelligence2( static_cast<SI16>(ndata) );	break;
			case DFNTAG_LODAMAGE:		applyTo->SetLoDamage( static_cast<SI16>(ndata) );		break;
			case DFNTAG_LAYER:			applyTo->SetLayer( static_cast<ItemLayers>(ndata) );	break;
			case DFNTAG_LIGHT:			applyTo->SetWeatherDamage( LIGHT, ndata != 0 );			break;
			case DFNTAG_LIGHTNING:		applyTo->SetWeatherDamage( LIGHTNING, ndata != 0 );		break;
			case DFNTAG_MAXHP:			applyTo->SetMaxHP( static_cast<SI16>(ndata) );			break;
			case DFNTAG_MOVABLE:		applyTo->SetMovable( static_cast<SI08>(ndata) );		break;
			case DFNTAG_MORE:			applyTo->SetTempVar( CITV_MORE, ndata );				break;
			case DFNTAG_MORE2:																	break;
			case DFNTAG_MOREX:			applyTo->SetTempVar( CITV_MOREX, ndata );				break;
			case DFNTAG_MOREY:			applyTo->SetTempVar( CITV_MOREY, ndata );				break;
			case DFNTAG_MOREZ:			applyTo->SetTempVar( CITV_MOREZ, ndata );				break;
			case DFNTAG_NAME:			applyTo->SetName( cdata );								break;
			case DFNTAG_NAME2:			applyTo->SetName2( cdata.c_str() );						break;
			case DFNTAG_NEWBIE:			applyTo->SetNewbie( true );								break;
			case DFNTAG_OFFSPELL:		applyTo->SetOffSpell( static_cast<SI08>(ndata) );		break;
			case DFNTAG_POISONDAMAGE:	applyTo->SetWeatherDamage( POISON, ndata != 0 );		break;
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
				if( ndata && ndata >= 0 )
				{
					if( odata && odata >= 0 )
					{
						applyTo->SetBuyValue( ndata );
						applyTo->SetSellValue( odata );
					}
					else
					{
						// Only one value was specified in DFN tag - use it for both buy and sell value
						applyTo->SetBuyValue( ndata );
						applyTo->SetSellValue( ndata );
					}
				}
				else
					Console.warning( format("Invalid data found in VALUE tag inside item script %s", cdata.c_str() ));
				break;
			case DFNTAG_WEIGHT:			applyTo->SetWeight( ndata );
				applyTo->SetBaseWeight( ndata ); // Let's set the base-weight as well. Primarily used for containers.
				break;
			case DFNTAG_WEIGHTMAX:		applyTo->SetWeightMax( ndata );				break;
			case DFNTAG_WIPE:			applyTo->SetWipeable( ndata != 0 );			break;
			case DFNTAG_ADDMENUITEM:
				Console.print(cdata);
				break;
			case DFNTAG_CUSTOMSTRINGTAG:
				customTagName			= cdata.section( " ", 0, 0 );
				customTagStringValue	= cdata.section(" ", 1 );
				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= FALSE;
					customTag.m_StringValue	= customTagStringValue;
					customTag.m_IntValue	= customTag.m_StringValue.length();
					customTag.m_ObjectType	= TAGMAP_TYPE_STRING;
					applyTo->SetTag( customTagName, customTag );
				}
				break;
			case DFNTAG_CUSTOMINTTAG:
				customTagName			= cdata.section(" ", 0, 0);
				customTagStringValue	= cdata.section(" ", 1);
				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= FALSE;
					customTag.m_IntValue = customTagStringValue.toInt();
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					applyTo->SetTag( customTagName, customTag );
				}
				break;
			case DFNTAG_SPAWNOBJ:
			case DFNTAG_SPAWNOBJLIST:
				break;
			case DFNTAG_LOOT:       Items->AddRespawnItem( applyTo, cdata, true, true); break;
			case DFNTAG_PACKITEM:
				if( cdata.sectionCount( "," ) != 0 )
					Items->AddRespawnItem( applyTo, cdata.section( ",", 0, 0 ), true, false, cdata.section( ",", 1, 1 ).stripWhiteSpace().toUShort() ); //section 0 = id, section 1 = amount
				else
					Items->AddRespawnItem( applyTo, cdata, true, false, 1 );
				break;
			default:					Console.warning( format("Unknown items dfn tag %i %s %i %i ", tag, cdata.c_str(), ndata, odata ));	break;
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateItem( CSocket *mSock, CChar *mChar, const UI16 iID, const UI16 iAmount, const UI16 iColour, const ObjectType itemType, const bool inPack )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic item and gives it an ID, Colour, and amount, also will
//|					automatically look for an entry in harditems.dfn and set its location (be it in
//|					a pack or on the ground).
//o-----------------------------------------------------------------------------------------------o
CItem * cItem::CreateItem( CSocket *mSock, CChar *mChar, const UI16 iID, const UI16 iAmount, const UI16 iColour, const ObjectType itemType, const bool inPack )
{
	if( inPack && !ValidateObject( mChar->GetPackItem() ) )
	{
		Console.warning(format( "CreateItem(): Character %s(0x%X) has no pack, item creation aborted.", mChar->GetName().c_str(), mChar->GetSerial()) );
		return NULL;
	}

	CItem *iCreated = CreateBaseItem( mChar->WorldNumber(), itemType, mChar->GetInstanceID() );
	if( iCreated == NULL )
		return NULL;

	if( iID != 0x0000 )
	{
		if( Map->IsValidTile( iID ) )
			iCreated->SetID( iID );
	}
	if( iColour != 0x0000 )
		iCreated->SetColour( iColour );

	// Only set item to decayable by default if ini setting is enabled
	if( cwmWorldState->ServerData()->BaseItemsDecayable() )
	{
		iCreated->SetDecayable( true );
	}

	GetScriptItemSettings( iCreated );

	if( iAmount > 1 )
	{
		iCreated->SetPileable( true );
		iCreated->SetAmount( iAmount );
	}

	cScript *toGrab = JSMapping->GetScript( iCreated->GetScriptTrigger() );
	if( toGrab != NULL )
		toGrab->OnCreate( iCreated, false );

	return PlaceItem( mSock, mChar, iCreated, inPack );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateScriptItem( CSocket *mSock, CChar *mChar, const std::string &item,
//|						const UI16 iAmount, const ObjectType itemType, const bool inPack, const UI16 iColor )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a script item, gives it an amount, and sets
//|					its location (be it in a pack or on the ground).
//o-----------------------------------------------------------------------------------------------o
CItem * cItem::CreateScriptItem( CSocket *mSock, CChar *mChar, const std::string &item, const UI16 iAmount, const ObjectType itemType, const bool inPack, const UI16 iColor )
{
	if( inPack && !ValidateObject( mChar->GetPackItem() ) )
	{
		Console.warning( format("CreateScriptItem(): Character %s(0x%X) has no pack, item creation aborted.", mChar->GetName().c_str(), mChar->GetSerial() ));
		return NULL;
	}

	CItem *iCreated = CreateBaseScriptItem( item, mChar->WorldNumber(), iAmount, mChar->GetInstanceID(), itemType );
	if( iCreated == NULL )
		return NULL;

	if( iColor != 0xFFFF )
		iCreated->SetColour( iColor );
	if( iAmount > 1 && !iCreated->isPileable() )
	{
		// Turns out we need to spawn more than one item, let's do that here:
		CItem *iCreated2 = NULL;
		for( UI16 i = 0; i < iAmount-1; ++i ) //minus 1 because 1 item has already been created at this point
		{
			iCreated2 = CreateBaseScriptItem( item, mChar->WorldNumber(), 1, mChar->GetInstanceID(), itemType );
			if( iCreated2 )
			{
				if( iColor != 0xFFFF )
					iCreated2->SetColour( iColor );
				PlaceItem( mSock, mChar, iCreated2, inPack );
			}
		}
		// Return the original item created in the function.
		return PlaceItem( mSock, mChar, iCreated, inPack );
	}

	return PlaceItem( mSock, mChar, iCreated, inPack );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateRandomItem( CSocket *mSock, const std::string& itemList )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a random item from an itemlist in specified dfn file,
//|						gives it a random buy/sell value, and places it
//o-----------------------------------------------------------------------------------------------o
CItem *cItem::CreateRandomItem( CSocket *mSock, const std::string& itemList )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return NULL;

	CItem *iCreated = CreateRandomItem( itemList, mChar->WorldNumber(), mChar->GetInstanceID() );
	if( iCreated == NULL )
		return NULL;

	if( iCreated->GetBuyValue() != 0 )
	{
		iCreated->SetBuyValue( RandomNum( static_cast<UI32>(1), iCreated->GetBuyValue() ) );
		iCreated->SetSellValue( static_cast<UI32>(iCreated->GetBuyValue() / 2) );
	}
	if( iCreated->GetHP() != 0 )
		iCreated->SetHP( static_cast<SI16>(RandomNum( static_cast<SI16>(1), iCreated->GetHP() )) );

	return PlaceItem( mSock, mChar, iCreated, true );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateRandomItem( const std::string& sItemList, const UI08 worldNum, const UI16 instanceID )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a random item from an itemlist in specified dfn file
//o-----------------------------------------------------------------------------------------------o
CItem *cItem::CreateRandomItem( const std::string& sItemList, const UI08 worldNum, const UI16 instanceID )
{
	CItem * iCreated	= NULL;
	UString sect		= "ITEMLIST " + sItemList;
	sect				= sect.stripWhiteSpace();

	if( sect == "blank" ) // The itemlist-entry is just a blank filler item
		return NULL;

	ScriptSection *ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList != NULL )
	{
		const size_t i = ItemList->NumEntries();
		if( i > 0 )
		{
			UString k = ItemList->MoveTo( RandomNum( static_cast< size_t >(0), i - 1 ) );
			if( !k.empty() )
			{
				if( k.upper() == "ITEMLIST" )
					iCreated = CreateRandomItem( ItemList->GrabData(), worldNum, instanceID );
				else
					iCreated = CreateBaseScriptItem( k, worldNum, 1, instanceID );
			}
		}
	}
	return iCreated;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj *CreateMulti( CChar *mChar, const std::string& cName, const UI16 iID, const bool isBoat )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a multi, and looks for an entry in harditems.dfn
//o-----------------------------------------------------------------------------------------------o
CMultiObj * cItem::CreateMulti( CChar *mChar, const std::string& cName, const UI16 iID, const bool isBoat )
{
	CMultiObj *mCreated = static_cast< CMultiObj * >(ObjectFactory::getSingleton().CreateObject( (isBoat) ? OT_BOAT : OT_MULTI ));
	if( mCreated == NULL )
		return NULL;

	mCreated->SetID( iID );
	GetScriptItemSettings( mCreated );
	mCreated->WorldNumber( mChar->WorldNumber() );
	mCreated->SetInstanceID( mChar->GetInstanceID() );
	mCreated->SetDecayable( false );
	if( !cName.empty() )
		mCreated->SetName( cName );

	return mCreated;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateBaseItem( const UI08 worldNum, const ObjectType itemType, const UI16 instanceID )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic item
//o-----------------------------------------------------------------------------------------------o
CItem * cItem::CreateBaseItem( const UI08 worldNum, const ObjectType itemType, const UI16 instanceID )
{
	if( itemType != OT_ITEM && itemType != OT_SPAWNER )
		return NULL;

	CItem *iCreated = static_cast< CItem * >(ObjectFactory::getSingleton().CreateObject( itemType ));
	if( iCreated == NULL )
		return NULL;

	iCreated->SetWipeable( true );
	iCreated->WorldNumber( worldNum );
	iCreated->SetInstanceID( instanceID );

	return iCreated;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateBaseScriptItem( UString ourItem, const UI08 worldNum, const UI16 iAmount, const UI16 instanceID, const ObjectType itemType )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic item from the scripts
//o-----------------------------------------------------------------------------------------------o
CItem * cItem::CreateBaseScriptItem( UString ourItem, const UI08 worldNum, const UI16 iAmount, const UI16 instanceID, const ObjectType itemType )
{
	ourItem						= ourItem.stripWhiteSpace();

	if( ourItem == "blank" ) // The lootlist-entry is just a blank filler item
		return NULL;

	ScriptSection *itemCreate	= FileLookup->FindEntry( ourItem, items_def );
	if( itemCreate == NULL )
	{
		Console.error( format("CreateBaseScriptItem(): Bad script item %s (Item Not Found).", ourItem.c_str()) );
		return NULL;
	}

	CItem *iCreated = NULL;
	if( itemCreate->ItemListExist() )
		iCreated = CreateRandomItem( itemCreate->ItemListData(), worldNum, instanceID );
	else
	{
		iCreated = CreateBaseItem( worldNum, itemType, instanceID );
		if( iCreated == NULL )
			return NULL;

		// Only set item to decayable by default if ini setting is enabled
		if( cwmWorldState->ServerData()->ScriptItemsDecayable() )
		{
			iCreated->SetDecayable( true );
		}

		if( !ApplyItemSection( iCreated, itemCreate ) )
			Console.error( "Trying to apply an item section failed" );

		if( !iCreated->GetMaxHP() && iCreated->GetHP() )
			iCreated->SetMaxHP( iCreated->GetHP() );

		cScript *toGrab = JSMapping->GetScript( iCreated->GetScriptTrigger() );
		if( toGrab != NULL )
			toGrab->OnCreate( iCreated, true );
	}
	if( iAmount > 1 && iCreated->isPileable() )
		iCreated->SetAmount( iAmount );

	return iCreated;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GetScriptItemSettings( CItem *iCreated )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Grabs item entries from harditems.dfn
//o-----------------------------------------------------------------------------------------------o
void cItem::GetScriptItemSettings( CItem *iCreated )
{
	const UString item = "x" + UString::number( iCreated->GetID(), 16 );
	ScriptSection *toFind = FileLookup->FindEntrySubStr( item, hard_items_def );
	if( toFind != NULL )
		ApplyItemSection( iCreated, toFind );
}

CItem *autoStack( CSocket *mSock, CItem *iToStack, CItem *iPack );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem * PlaceItem( CSocket *mSock, CChar *mChar, CItem *iCreated, const bool inPack )
//|	Date		-	10/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Places an item that was just created
//o-----------------------------------------------------------------------------------------------o
CItem * cItem::PlaceItem( CSocket *mSock, CChar *mChar, CItem *iCreated, const bool inPack )
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
	}
	else
		iCreated->SetLocation( mChar );

	return iCreated;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DecayItem( CItem& toDecay, const UI32 nextDecayItems )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Cause items to decay when left on the ground
//o-----------------------------------------------------------------------------------------------o
bool DecayItem( CItem& toDecay, const UI32 nextDecayItems )
{
	if( toDecay.GetDecayTime() == 0 || !cwmWorldState->ServerData()->GlobalItemDecay() )
	{
		toDecay.SetDecayTime( nextDecayItems );
		return false;
	}
	const bool isCorpse = toDecay.isCorpse();

	// Multis
	if( !toDecay.IsFieldSpell() && !isCorpse ) // Gives fieldspells a chance to decay in multis
	{
		if( toDecay.IsLockedDown() || toDecay.isDoorOpen() ||
		   ( ValidateObject( toDecay.GetMultiObj() ) &&
			( toDecay.GetMovable() >= 2 || !cwmWorldState->ServerData()->ItemDecayInHouses() ) ) )
		{
			toDecay.SetDecayTime( nextDecayItems );
			return false;
		}
	}

	if( toDecay.IsContType() )
	{
		if( !isCorpse || ValidateObject(toDecay.GetOwnerObj()) || !cwmWorldState->ServerData()->CorpseLootDecay() )
		{
			CDataList< CItem * > *iCont = toDecay.GetContainsList();
			for( CItem *io = iCont->First(); !iCont->Finished(); io = iCont->Next() )
			{
				if( ValidateObject( io ) )
				{
					if( io->GetLayer() != IL_HAIR && io->GetLayer() != IL_FACIALHAIR )
					{
						io->SetCont( NULL );
						io->SetLocation( (&toDecay) );
						io->SetDecayTime( nextDecayItems );
					}
				}
			}
		}
	}
	toDecay.Delete();
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	PackTypes getPackType( CItem *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the pack type based on ID
//o-----------------------------------------------------------------------------------------------o
PackTypes cItem::getPackType( CItem *i )
{
	PackTypes packType = PT_UNKNOWN;
	switch( i->GetID() )
	{
		case 0x2006:	// coffin
		case 0x0ECA:	// bones
		case 0x0ECB:	// bones
		case 0x0ECC:	// bones
		case 0x0ECD:	// bones
		case 0x0ECE:	// bones
		case 0x0ECF:	// bones
		case 0x0ED0:	// bones
		case 0x0ED1:	// bones
		case 0x0ED2:	// bones
			packType = PT_COFFIN;
			break;
		case 0x0E75:	// backpack
		case 0x0E79:	// pouch
		case 0x09B0:	// pouch
			packType = PT_PACK;
			break;
		case 0x0E76:	// leather bag
		case 0x2256:	// bagball
		case 0x2257:	// bagball
			packType = PT_BAG;
			break;
		case 0x0E77:	// barrel
		case 0x0E7F:	// keg
		case 0x0E83:	// empty tub
		case 0x0FAE:	// barrel with lids
		case 0x1AD7:	// potion kegs
		case 0x1940:	// barrel with lids
			packType = PT_BARREL;
			break;
		case 0x0E7A:	// square basket
		case 0x24D5:	// SE basket
		case 0x24D6:	// SE basket
		case 0x24D9:	// SE basket
		case 0x24DA:	// SE basket
			packType = PT_SQBASKET;
			break;
		case 0x0990:	// round basket
		case 0x09AC:	// round bushel
		case 0x09B1:	// round basket
		case 0x24D7:	// SE basket
		case 0x24D8:	// SE basket
		case 0x24DD:	// SE basket
		case 0x1882:	// winnoning basket
			packType = PT_RBASKET;
			break;
		case 0x0E40:	// gold chest
		case 0x0E41:	// gold chest
		case 0x4025:	// gargoyle chest
		case 0x4026:	// gargoyle chest
		case 0xA304:	// metal chest
		case 0xA305:	// metal chest
		case 0xA306:	// rusty metal chest
		case 0xA307:	// rusty metal chest
		case 0xA308:	// gold chest
		case 0xA309:	// gold chest
		case 0xA30A:	// barnacle metal chest
		case 0xA30B:	// barnacle metal chest
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
		case 0x232A:	// giftbox
		case 0x232B:	// giftbox
			packType = PT_GIFTBOX1;
			break;
		case 0x2857:	// SE armoire
		case 0x2858:	// SE armoire
			packType = PT_SEARMOIRE1;
			break;
		case 0x285B:	// SE armoire
		case 0x285C:	// SE armoire
			packType = PT_SEARMOIRE2;
			break;
		case 0x285D:	// SE armoire
		case 0x285E:	// SE armoire
		case 0x2859:	// SE armoire
		case 0x285A:	// SE armoire
			packType = PT_SEARMOIRE3;
			break;
		case 0x24DB:	// SE basket
		case 0x24DC:	// SE basket
			packType = PT_SEBASKET;
			break;
		case 0x280B:	// SE chest
		case 0x280C:	// SE chest
			packType = PT_SECHEST1;
			break;
		case 0x280D:	// SE chest
		case 0x280E:	// SE chest
			packType = PT_SECHEST2;
			break;
		case 0x280F:	// SE chest
		case 0x2810:	// SE chest
			packType = PT_SECHEST3;
			break;
		case 0x2811:	// SE chest
		case 0x2812:	// SE chest
		case 0x2815:	// SE cabinet
		case 0x2816:	// SE cabinet
		case 0x2817:	// SE chest
		case 0x2818:	// SE chest
			packType = PT_SECHEST4;
			break;
		case 0x2813:	// SE chest
		case 0x2814:	// SE chest
			packType = PT_SECHEST5;
			break;
		case 0x46A5:	// SA giftbox
		case 0x46A6:	// SA giftbox
			packType = PT_GIFTBOX2;
			break;
		case 0x46A2:	// SA giftbox
			packType = PT_GIFTBOX3;
			break;
		case 0x46A3:	// SA giftbox
			packType = PT_GIFTBOX4;
			break;
		case 0x46A4:	// SA giftbox
			packType = PT_GIFTBOX5;
			break;
		case 0x46A7:	// SA giftbox
			packType = PT_GIFTBOX6;
			break;
		case 0x0E1C:	// Backgammon board
		case 0x0FAD:	// Backgammon board
			packType = PT_GAME_BACKGAMMON;
			break;
		case 0x0FA6:	// Chess board
			packType = PT_GAME_CHESS;
			break;
		case 0xA202:	// Dolphin mailbox
		case 0xA203:	// Dolphin mailbox
		case 0xA204:	// Dolphin mailbox
		case 0xA205:	// Dolphin mailbox
			packType = PT_MAILBOX1;
			break;
		case 0xA206:	// Squirrel mailbox
		case 0xA207:	// Squirrel mailbox
		case 0xA208:	// Squirrel mailbox
		case 0xA209:	// Squirrel mailbox
			packType = PT_MAILBOX2;
			break;
		case 0xA1F5:	// Wooden barrel mailbox
		case 0xA1F7:	// Wooden barrel mailbox
		case 0xA1F8:	// Wooden barrel mailbox
		case 0xA1F9:	// Wooden barrel mailbox
			packType = PT_MAILBOX3;
			break;
		case 0xA268:	// Light mailbox
		case 0xA269:	// Light mailbox
		case 0xA26A:	// Light mailbox
		case 0xA26B:	// Light mailbox
		case 0xA26C:	// Light mailbox
		case 0xA26D:	// Light mailbox
		case 0xA26E:	// Light mailbox
		case 0xA26F:	// Light mailbox
			packType = PT_MAILBOX4;
			break;
		case 0xA3EB:	// Sitting kitten mailbox
		case 0xA3EC:	// Sitting kitten mailbox
		case 0xA3ED:	// Sitting kitten mailbox
		case 0xA3EE:	// Sitting kitten mailbox
			packType = PT_MAILBOX5;
			break;
		case 0xA3EF:	// Standing kitten mailbox
		case 0xA3F0:	// Standing kitten mailbox
		case 0xA3F1:	// Standing kitten mailbox
		case 0xA3F2:	// Standing kitten mailbox
			packType = PT_MAILBOX6;
			break;
		case 0xA3F3:	// Scarecrow mailbox
		case 0xA3F4:	// Scarecrow mailbox
		case 0xA3F5:	// Scarecrow mailbox
		case 0xA3F6:	// Scarecrow mailbox
			packType = PT_MAILBOX7;
			break;
		case 0xA3F7:	// Lion mailbox
		case 0xA3F8:	// Lion mailbox
		case 0xA3F9:	// Lion mailbox
		case 0xA3FA:	// Lion mailbox
			packType = PT_MAILBOX8;
			break;
		case 0x4141:	// Square gray mailbox
		case 0x4142:	// Square gray mailbox
		case 0x4143:	// Square gray mailbox
		case 0x4144:	// Square gray mailbox
			packType = PT_MAILBOX9;
			break;
		default:
			packType = PT_UNKNOWN;
			break;
	}
	return packType;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddRespawnItem( CItem *s, const std::string& x, const bool inCont, const bool randomItem, const UI16 itemAmount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles spawning of items from spawn objects/containers
//o-----------------------------------------------------------------------------------------------o
void cItem::AddRespawnItem( CItem *s, const std::string& x, const bool inCont, const bool randomItem, const UI16 itemAmount )
{
	if( !ValidateObject( s ) || x.empty() )
		return;
	CItem *c = NULL;
	if( randomItem )
		c = CreateRandomItem( x, s->WorldNumber(), s->GetInstanceID() );
	else
		c = CreateBaseScriptItem( x, s->WorldNumber(), itemAmount, s->GetInstanceID() );
	if( c == NULL )
		return;

	if( itemAmount > 1 && !c->isPileable() ) //Not stackable? Okay, spawn 'em all individually
	{
		CItem *iCreated2 = NULL;
		for( UI08 i = 0; i < itemAmount; ++i )
		{
			if( randomItem )
				iCreated2 = CreateRandomItem( x, s->WorldNumber(), s->GetInstanceID() );
			else
				iCreated2 = CreateBaseScriptItem( x, s->WorldNumber(), 1, s->GetInstanceID() );
			if( iCreated2 )
			{
				if( inCont )
				{
					iCreated2->SetCont( s );
					iCreated2->PlaceInPack();
				}
				else
					iCreated2->SetLocation( s );
				iCreated2->SetSpawn( s->GetSerial() );
			}
		}
	}

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
				case PT_SEBASKET:
					c->SetY( ( RandomNum( 0, 49 ) ) + 50 );
					break;
				case PT_BOOKCASE:
				case PT_FARMOIRE:
				case PT_WARMOIRE:
				case PT_DRAWER:
				case PT_DRESSER:
				case PT_SECHEST1:
				case PT_SECHEST2:
				case PT_SECHEST3:
				case PT_SECHEST4:
				case PT_SECHEST5:
				case PT_SEARMOIRE1:
				case PT_SEARMOIRE2:
				case PT_SEARMOIRE3:
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
					Console.warning( " A non-container item was set as container spawner" );
					break;
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GlowItem( CItem *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle glowing items
//o-----------------------------------------------------------------------------------------------o
void cItem::GlowItem( CItem *i )
{
	if( i->GetGlow() != INVALIDSERIAL )
	{
		CItem *j = calcItemObjFromSer( i->GetGlow() );
		if( !ValidateObject( j ) )
			return;

		CBaseObject *getCont = i->GetCont();
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
			CChar *s = static_cast<CChar *>(getCont);
			if( ValidateObject( s ) )
			{
				j->SetCont( getCont );
				j->SetX( s->GetX() );
				j->SetY( s->GetY() );
				j->SetZ( s->GetZ()+4 );
				if( isOnline( (*s) ) )
					j->SetDir( 29 );
				else
					j->SetDir( 99 );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckEquipment( CChar *p )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks equipment of character and validates that they have enough strength
//|					to have each item equipped
//o-----------------------------------------------------------------------------------------------o
void cItem::CheckEquipment( CChar *p )
{
	if( ValidateObject( p ) )
	{
		CSocket *pSock = p->GetSocket();
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
						getTileName( (*i), itemname );
					else
						itemname = i->GetName();

					i->SetCont( NULL );
					i->SetLocation( p );

					SOCKLIST nearbyChars = FindNearbyPlayers( p );
					for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void StoreItemRandomValue( CItem *i, CTownRegion *tReg )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Stores an item's random "good" value (used by trade system)
//o-----------------------------------------------------------------------------------------------o
void cItem::StoreItemRandomValue( CItem *i, CTownRegion *tReg )
{
	if( i->GetGood() < 0 )
		return;
	if( tReg == NULL )
	{
		CBaseObject *getLastCont = i->GetCont();
		if( getLastCont != NULL )
			tReg = calcRegionFromXY( getLastCont->GetX(), getLastCont->GetY(), getLastCont->WorldNumber(), getLastCont->GetInstanceID() );
		if( tReg == NULL )
			return;
	}

	const SI32 min = tReg->GetGoodRnd1( static_cast<UI08>(i->GetGood()) );
	const SI32 max = tReg->GetGoodRnd2( static_cast<UI08>(i->GetGood()) );

	if( max != 0 || min != 0 )
		i->SetRndValueRate( RandomNum( min, max ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *DupeItem( CSocket *s, CItem *i, UI32 amount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Duplicates selected item
//o-----------------------------------------------------------------------------------------------o
CItem *cItem::DupeItem( CSocket *s, CItem *i, UI32 amount )
{
	CChar *mChar		= s->CurrcharObj();
	CBaseObject *iCont	= i->GetCont();
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

	cScript *toGrab = JSMapping->GetScript( c->GetScriptTrigger() );
	if( toGrab != NULL )
		toGrab->OnCreate( c, false );

	return c;
}
