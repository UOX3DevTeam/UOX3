//o------------------------------------------------------------------------------------------------o
//|	File		-	UOXJSPropertyFuncts.cpp
//|	Date		-	12/14/2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	1.0		14th December, 2001 Initial implementation
//|							Includes property getters for CItem and CChar, and property
//|							setters for CChar
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"
#include "UOXJSPropertyEnums.h"
#include "UOXJSPropertyFuncs.h"
#include "JSEncapsulate.h"
#include "CJSEngine.h"

#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "regions.h"
#include "magic.h"
#include "cMagic.h"
#include "scriptc.h"
#include "ssection.h"
#include "classes.h"
#include "movement.h"
#include "Dictionary.h"
#include "cSpawnRegion.h"

#include "PartySystem.h"

#include <jsapi.h>
#include <js/Object.h>
#define INT_FITS_IN_JSVAL(i) ((i) >= INT32_MIN && (i) <= INT32_MAX)
void MakeShop( CChar *c );
void ScriptError( JSContext *cx, const char *txt, ... );

std::map< std::string, intN >		propLookupAccount;
std::map< std::string, intN >		propLookupChar;
std::map< std::string, intN >		propLookupConsole;
std::map< std::string, intN >		propLookupGuild;
std::map< std::string, intN >		propLookupItem;
std::map< std::string, intN >		propLookupParty;
std::map< std::string, intN >		propLookupRace;
std::map< std::string, intN >		propLookupRegion;
std::map< std::string, intN >		propLookupResource;
std::map< std::string, intN >		propLookupSkills;
std::map< std::string, intN >		propLookupSocket;
std::map< std::string, intN >		propLookupSpawnRegion;


UI16    GetPropByName( JSPrototypes protoNum, const std::string& prop )
{
	const std::map< std::string, intN > * toScan = nullptr;
	switch( protoNum )
	{
	case JSP_ITEM: toScan = &propLookupItem; break;
	case JSP_CHAR: toScan = &propLookupChar; break;
	case JSP_SOCK: toScan = &propLookupSocket; break;
	case JSP_GUILD: toScan = &propLookupGuild; break;
	case JSP_RACE: toScan = &propLookupRace; break;
	case JSP_REGION: toScan = &propLookupRegion; break;
	case JSP_SPAWNREGION: toScan = &propLookupSpawnRegion; break;
	case JSP_RESOURCE: toScan = &propLookupResource; break;
	case JSP_ACCOUNT: toScan = &propLookupAccount; break;
	case JSP_CONSOLE: toScan = &propLookupConsole; break;
	case JSP_PARTY: toScan = &propLookupParty; break;
	case JSP_GLOBALSKILLS: toScan = &propLookupSkills; break;
	case JSP_SPELL:
	case JSP_SPELLS:
	case JSP_GLOBALSKILL:
	case JSP_ACCOUNTS:
	case JSP_FILE:
	case JSP_CREATEENTRY:
	case JSP_CREATEENTRIES:
	case JSP_TIMER:
	case JSP_SCRIPT:
	case JSP_GUMP:
	case JSP_PACKET:
		break;
	};
	if( toScan != nullptr )
	{
		std::map< std::string, intN >::const_iterator citer = toScan->find( prop );
		if( citer != toScan->cend() ) {
			return citer->second;
		}
		else {
			return 0xFFFF;
		}
	}
	else
	{
		return 0xFFFF;
	}

}

UI16 getScriptID( JSContext *cx, jsid id, JSPrototypes section )
{
	UI16 propID = 0xFFFF;
	if( JSID_IS_STRING( id ) )
	{
		JS::RootedString str( cx, JSID_TO_STRING( id ));
		JS::UniqueChars chars = JS_EncodeStringToUTF8( cx, str );
		if( !chars )
		{
			return propID;
		}
		propID = GetPropByName( section, chars.get() );
		/*if( propID == 0xFFFF )
		{
			Console.Log( oldstrutil::format( "String property '%s' found on object type %d in script %d", chars, section, JSMapping->currentActive()->GetScriptID() ), "warning.log");
		}*/
	}
	else if( JSID_IS_INT( id ) )
	{
		propID = JSID_TO_INT( id );
	}
	return propID;
}


JSBool CGuildsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	*vp = INT_TO_JSVAL( 0 );
	return JS_TRUE;
}

JSBool CGuildsProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	return JS_TRUE;
}

JSBool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	size_t spellId = JSID_TO_INT( id );

	if( spellId >= Magic->spells.size() )
	{
		ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell ID (%i) provided", spellId ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	CSpellInfo *mySpell = &Magic->spells[spellId];
	if( mySpell == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell with spellId %i", spellId ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsSpell = JS_NewObject( cx, &UOXSpell_class, nullptr, obj );
	JS_DefineProperties( cx, jsSpell, CSpellProperties );
	JS_SetPrivate( cx, jsSpell, mySpell );

	*vp = OBJECT_TO_JSVAL( jsSpell );
	return JS_TRUE;
}

// Direct mozjs accessors for the fully migrated CSpell property family.
FDCLG( CSpell, id )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<CSpellInfo>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	auto spellIter = std::find_if( Magic->spells.begin(), Magic->spells.end(),
		[priv]( const CSpellInfo &spell ) { return &spell == priv; } );
	if( spellIter == Magic->spells.end() )
		return false;

	args.rval().setInt32( static_cast<int32_t>( std::distance( Magic->spells.begin(), spellIter )) );
	return true;
}

FDCLG( CSpell, name )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<CSpellInfo>( thisObj, 0 );
	if( priv == nullptr || Magic->spells.empty() || priv <= Magic->spells.data() || priv > &Magic->spells.back() )
		return false;

	const auto spellIndex = static_cast<size_t>( priv - Magic->spells.data() - 1 );
	const auto spellName = Dictionary->GetEntry( magic_table[spellIndex].spell_name );
	JS::RootedString value( cx, JS_NewStringCopyZ( cx, spellName.c_str() ));
	if( value == nullptr )
		return false;
	args.rval().setString( value );
	return true;
}

// clang-format off
IMPL_GET( CSpell, action,          CSpellInfo, setInt32, Action() )
IMPL_GET( CSpell, baseDmg,         CSpellInfo, setInt32, BaseDmg() )
IMPL_GET( CSpell, health,          CSpellInfo, setInt32, Health() )
IMPL_GET( CSpell, stamina,         CSpellInfo, setInt32, Stamina() )
IMPL_GET( CSpell, mana,            CSpellInfo, setInt32, Mana() )
IMPL_GET( CSpell, scrollLow,       CSpellInfo, setInt32, ScrollLow() )
IMPL_GET( CSpell, scrollHigh,      CSpellInfo, setInt32, ScrollHigh() )
IMPL_GET( CSpell, circle,          CSpellInfo, setInt32, Circle() )
IMPL_GET( CSpell, lowSkill,        CSpellInfo, setInt32, LowSkill() )
IMPL_GET( CSpell, highSkill,       CSpellInfo, setInt32, HighSkill() )
IMPL_GET( CSpell, ginseng,         CSpellInfo, setInt32, Reagants().ginseng )
IMPL_GET( CSpell, moss,            CSpellInfo, setInt32, Reagants().moss )
IMPL_GET( CSpell, drake,           CSpellInfo, setInt32, Reagants().drake )
IMPL_GET( CSpell, pearl,           CSpellInfo, setInt32, Reagants().pearl )
IMPL_GET( CSpell, silk,            CSpellInfo, setInt32, Reagants().silk )
IMPL_GET( CSpell, ash,             CSpellInfo, setInt32, Reagants().ash )
IMPL_GET( CSpell, shade,           CSpellInfo, setInt32, Reagants().shade )
IMPL_GET( CSpell, garlic,          CSpellInfo, setInt32, Reagants().garlic )
IMPL_GET( CSpell, batwing,         CSpellInfo, setInt32, Reagants().batwing )
IMPL_GET( CSpell, daemonBlood,     CSpellInfo, setInt32, Reagants().daemonblood )
IMPL_GET( CSpell, graveDust,       CSpellInfo, setInt32, Reagants().gravedust )
IMPL_GET( CSpell, noxCrystal,      CSpellInfo, setInt32, Reagants().noxcrystal )
IMPL_GET( CSpell, pigIron,         CSpellInfo, setInt32, Reagants().pigiron )
IMPL_GET( CSpell, soundEffect,     CSpellInfo, setInt32, Effect() )
IMPL_GET( CSpell, tithing,         CSpellInfo, setInt32, Tithing() )
IMPL_GET( CSpell, delay,           CSpellInfo, setDouble, Delay() )
IMPL_GET( CSpell, damageDelay,     CSpellInfo, setDouble, DamageDelay() )
IMPL_GET( CSpell, recoveryDelay,   CSpellInfo, setDouble, RecoveryDelay() )
IMPL_GET( CSpell, requireTarget,   CSpellInfo, setBoolean, RequireTarget() )
IMPL_GET( CSpell, requireItem,     CSpellInfo, setBoolean, RequireItemTarget() )
IMPL_GET( CSpell, requireLocation, CSpellInfo, setBoolean, RequireLocTarget() )
IMPL_GET( CSpell, requireChar,     CSpellInfo, setBoolean, RequireCharTarget() )
IMPL_GET( CSpell, travelSpell,     CSpellInfo, setBoolean, TravelSpell() )
IMPL_GET( CSpell, fieldSpell,      CSpellInfo, setBoolean, FieldSpell() )
IMPL_GET( CSpell, reflectable,     CSpellInfo, setBoolean, SpellReflectable() )
IMPL_GET( CSpell, aggressiveSpell, CSpellInfo, setBoolean, AggressiveSpell() )
IMPL_GET( CSpell, resistable,      CSpellInfo, setBoolean, Resistable() )
IMPL_GET( CSpell, enabled,         CSpellInfo, setBoolean, Enabled() )
IMPL_GETS( CSpell, mantra,         CSpellInfo, setString, Mantra().c_str() )
IMPL_GETS( CSpell, strToSay,       CSpellInfo, setString, StringToSay().c_str() )
// clang-format on
JSBool CGlobalSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	size_t skillId = JSID_TO_INT( id );

	if( skillId > THROWING )
	{
		ScriptError( cx, oldstrutil::format( "Invalid Skill ID, must be between 0 and 57" ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	CWorldMain::Skill_st *mySkill = &cwmWorldState->skill[skillId];
	if( mySkill == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "Invalid Skill" ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsSkill = JS_NewObject( cx, &UOXGlobalSkill_class, nullptr, obj );
	JS_DefineProperties( cx, jsSkill, CGlobalSkillProperties );
	JS_SetPrivate( cx, jsSkill, mySkill );

	*vp = OBJECT_TO_JSVAL( jsSkill );
	return JS_TRUE;
}

// clang-format off
IMPL_GETS( CGlobalSkill, name,         CWorldMain::Skill_st, setString, name.c_str() )
IMPL_GETS( CGlobalSkill, madeWord,     CWorldMain::Skill_st, setString, madeWord.c_str() )
IMPL_GET(  CGlobalSkill, strength,     CWorldMain::Skill_st, setInt32,  strength )
IMPL_GET(  CGlobalSkill, dexterity,    CWorldMain::Skill_st, setInt32,  dexterity )
IMPL_GET(  CGlobalSkill, intelligence, CWorldMain::Skill_st, setInt32,  intelligence )
IMPL_GET(  CGlobalSkill, skillDelay,   CWorldMain::Skill_st, setInt32,  skillDelay )
IMPL_GET(  CGlobalSkill, scriptID,     CWorldMain::Skill_st, setInt32,  jsScript )
// clang-format on

// clang-format off
IMPL_GET_NP( CTimer, TIMEOUT,                   setInt32, tCHAR_TIMEOUT )
IMPL_GET_NP( CTimer, INVIS,                     setInt32, tCHAR_INVIS )
IMPL_GET_NP( CTimer, HUNGER,                    setInt32, tCHAR_HUNGER )
IMPL_GET_NP( CTimer, THIRST,                    setInt32, tCHAR_THIRST )
IMPL_GET_NP( CTimer, POISONTIME,                setInt32, tCHAR_POISONTIME )
IMPL_GET_NP( CTimer, POISONTEXT,                setInt32, tCHAR_POISONTEXT )
IMPL_GET_NP( CTimer, POISONWEAROFF,             setInt32, tCHAR_POISONWEAROFF )
IMPL_GET_NP( CTimer, SPELLTIME,                 setInt32, tCHAR_SPELLTIME )
IMPL_GET_NP( CTimer, SPELLRECOVERYTIME,         setInt32, tCHAR_SPELLRECOVERYTIME )
IMPL_GET_NP( CTimer, CRIMFLAG,                  setInt32, tCHAR_CRIMFLAG )
IMPL_GET_NP( CTimer, ANTISPAM,                  setInt32, tCHAR_ANTISPAM )
IMPL_GET_NP( CTimer, MURDERRATE,                setInt32, tCHAR_MURDERRATE )
IMPL_GET_NP( CTimer, PEACETIMER,                setInt32, tCHAR_PEACETIMER )
IMPL_GET_NP( CTimer, FLYINGTOGGLE,              setInt32, tCHAR_FLYINGTOGGLE )
IMPL_GET_NP( CTimer, FIREFIELDTICK,             setInt32, tCHAR_FIREFIELDTICK )
IMPL_GET_NP( CTimer, POISONFIELDTICK,           setInt32, tCHAR_POISONFIELDTICK )
IMPL_GET_NP( CTimer, PARAFIELDTICK,             setInt32, tCHAR_PARAFIELDTICK )
IMPL_GET_NP( CTimer, MOVETIME,                  setInt32, tNPC_MOVETIME )
IMPL_GET_NP( CTimer, SPATIMER,                  setInt32, tNPC_SPATIMER )
IMPL_GET_NP( CTimer, SUMMONTIME,                setInt32, tNPC_SUMMONTIME )
IMPL_GET_NP( CTimer, EVADETIME,                 setInt32, tNPC_EVADETIME )
IMPL_GET_NP( CTimer, LOYALTYTIME,               setInt32, tNPC_LOYALTYTIME )
IMPL_GET_NP( CTimer, IDLEANIMTIME,              setInt32, tNPC_IDLEANIMTIME )
IMPL_GET_NP( CTimer, LOGOUT,                    setInt32, tPC_LOGOUT )
IMPL_GET_NP( CTimer, SOCK_SKILLDELAY,           setInt32, tPC_SKILLDELAY )
IMPL_GET_NP( CTimer, SOCK_OBJDELAY,             setInt32, tPC_OBJDELAY )
IMPL_GET_NP( CTimer, SOCK_SPIRITSPEAK,          setInt32, tPC_SPIRITSPEAK )
IMPL_GET_NP( CTimer, SOCK_TRACKING,             setInt32, tPC_TRACKING )
IMPL_GET_NP( CTimer, SOCK_FISHING,              setInt32, tPC_FISHING )
IMPL_GET_NP( CTimer, SOCK_MUTETIME,             setInt32, tPC_MUTETIME )
IMPL_GET_NP( CTimer, SOCK_TRACKINGDISPLAY,      setInt32, tPC_TRACKINGDISPLAY )
IMPL_GET_NP( CTimer, SOCK_TRAFFICWARDEN,        setInt32, tPC_TRAFFICWARDEN )
IMPL_GET_NP( CTimer, SOCK_SPEEDHACKPENALTY,     setInt32, tPC_SPEEDHACKPENALTY )
IMPL_GET_NP( CTimer, SOCK_SPEEDHACKLOGGED,      setInt32, tPC_SPEEDHACKLOGGED )
// clang-format on

JSBool CCreateEntriesProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	UI16 createEntryId = static_cast<UI16>( JSID_TO_INT( id ));

	CreateEntry_st *myCreateEntry = Skills->FindItem( createEntryId );
	if( myCreateEntry == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "Invalid create entry ID (%i)", createEntryId ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsCreateEntry = JS_NewObject( cx, &UOXCreateEntry_class, nullptr, obj );
	JS_DefineProperties( cx, jsCreateEntry, CCreateEntryProperties );
	JS_SetPrivate( cx, jsCreateEntry, myCreateEntry );

	*vp = OBJECT_TO_JSVAL( jsCreateEntry );
	return JS_TRUE;
}

// clang-format off
IMPL_GET(  CCreateEntry, id,          CreateEntry_st, setInt32,  targId )
IMPL_GETS( CCreateEntry, name,        CreateEntry_st, setString, name.c_str() )
IMPL_GETS( CCreateEntry, addItem,     CreateEntry_st, setString, addItem.c_str() )
IMPL_GET(  CCreateEntry, colour,      CreateEntry_st, setInt32,  colour )
IMPL_GET(  CCreateEntry, sound,       CreateEntry_st, setInt32,  soundPlayed )
IMPL_GET(  CCreateEntry, minRank,     CreateEntry_st, setInt32,  minRank )
IMPL_GET(  CCreateEntry, maxRank,     CreateEntry_st, setInt32,  maxRank )
IMPL_GET(  CCreateEntry, delay,       CreateEntry_st, setInt32,  delay )
IMPL_GET(  CCreateEntry, spell,       CreateEntry_st, setInt32,  spell )
IMPL_GET(  CCreateEntry, avgMinSkill, CreateEntry_st, setInt32,  AverageMinSkill() )
IMPL_GET(  CCreateEntry, avgMaxSkill, CreateEntry_st, setInt32,  AverageMaxSkill() )
// clang-format on

FDCLG( CCreateEntry, resources )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<CreateEntry_st>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	JS::RootedObject resources( cx, JS::NewArrayObject( cx, 0 ) );
	if( resources == nullptr )
		return false;

	for( size_t i = 0; i < priv->resourceNeeded.size(); ++i )
	{
		const auto &entry = priv->resourceNeeded[i];
		JS::RootedObject resource( cx, JS::NewArrayObject( cx, 0 ) );
		JS::RootedObject ids( cx, JS::NewArrayObject( cx, 0 ) );
		if( resource == nullptr || ids == nullptr )
			return false;

		for( size_t j = 0; j < entry.idList.size(); ++j )
		{
			JS::RootedValue idValue( cx, JS::Int32Value( entry.idList[j] ) );
			if( !JS_SetElement( cx, ids, static_cast<uint32_t>( j ), idValue ) )
				return false;
		}

		JS::RootedValue amountValue( cx, JS::Int32Value( entry.amountNeeded ) );
		JS::RootedValue colourValue( cx, JS::Int32Value( entry.colour ) );
		JS::RootedValue idsValue( cx, JS::ObjectValue( *ids ) );
		if( !JS_SetElement( cx, resource, 0, amountValue ) ||
			!JS_SetElement( cx, resource, 1, colourValue ) ||
			!JS_SetElement( cx, resource, 2, idsValue ) )
			return false;

		JS::RootedValue resourceValue( cx, JS::ObjectValue( *resource ) );
		if( !JS_SetElement( cx, resources, static_cast<uint32_t>( i ), resourceValue ) )
			return false;
	}

	args.rval().setObject( *resources );
	return true;
}

FDCLG( CCreateEntry, skills )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<CreateEntry_st>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	JS::RootedObject skills( cx, JS::NewArrayObject( cx, 0 ) );
	if( skills == nullptr )
		return false;

	for( size_t i = 0; i < priv->skillReqs.size(); ++i )
	{
		const auto &entry = priv->skillReqs[i];
		JS::RootedObject skill( cx, JS::NewArrayObject( cx, 0 ) );
		if( skill == nullptr )
			return false;

		JS::RootedValue numberValue( cx, JS::Int32Value( entry.skillNumber ) );
		JS::RootedValue minValue( cx, JS::Int32Value( entry.minSkill ) );
		JS::RootedValue maxValue( cx, JS::Int32Value( entry.maxSkill ) );
		if( !JS_SetElement( cx, skill, 0, numberValue ) ||
			!JS_SetElement( cx, skill, 1, minValue ) ||
			!JS_SetElement( cx, skill, 2, maxValue ) )
			return false;

		JS::RootedValue skillValue( cx, JS::ObjectValue( *skill ) );
		if( !JS_SetElement( cx, skills, static_cast<uint32_t>( i ), skillValue ) )
			return false;
	}

	args.rval().setObject( *skills );
	return true;
}

JSBool CItemProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CItem *gPriv = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	SERIAL TempSerial = INVALIDSERIAL;

	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_ITEM );

	if( propID != 0xFFFF )
	{
		JSString *tString = nullptr;
		switch( propID )
		{
			case CIP_SECTIONID:
				tString = JS_NewStringCopyZ( cx, gPriv->GetSectionId().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_TITLE:
				tString = JS_NewStringCopyZ( cx, gPriv->GetTitle().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_X:			*vp = INT_TO_JSVAL( gPriv->GetX() );			break;
			case CIP_Y:			*vp = INT_TO_JSVAL( gPriv->GetY() );			break;
			case CIP_Z:			*vp = INT_TO_JSVAL( gPriv->GetZ() );			break;
			case CIP_OLDX:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.x );
				break;
			}
			case CIP_OLDY:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.y );
				break;
			}
			case CIP_OLDZ:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.z );
				break;
			}
			case CIP_ID:		*vp = INT_TO_JSVAL( gPriv->GetId() );			break;
			case CIP_COLOUR:	*vp = INT_TO_JSVAL( gPriv->GetColour() );		break;
			case CIP_OWNER:
				CChar *pOwner;
				pOwner = gPriv->GetOwnerObj();

				if( !ValidateObject( pOwner ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, pOwner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			case CIP_VISIBLE:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetVisible() ));	break;
			case CIP_SERIAL:
			{
				int64_t serialVal = gPriv->GetSerial();
				if( serialVal < INT_MIN || serialVal > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetSerial(), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetSerial() );
				}
				break;
			}
			case CIP_HEALTH:		*vp = INT_TO_JSVAL( gPriv->GetHP() );				break;
			case CIP_SCRIPTTRIGGER:
			{
				// For backwards compatibility, get last scripttrigger from vector
				// For older worldfiles, this will be the only scripttrigger added to the vector after load
				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				UI16 lastScriptTrigger = 0;
				auto numberOfTriggers = scriptTriggers.size();
				if( numberOfTriggers > 0 )
				{
					lastScriptTrigger = scriptTriggers[numberOfTriggers-1];
				}

				*vp = INT_TO_JSVAL( lastScriptTrigger );
				break;
			}
			case CIP_SCRIPTTRIGGERS:
			{
				jsval scriptId;
				JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
				{
					scriptId = INT_TO_JSVAL( scriptTriggers[i] );
					JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
				}

				*vp = OBJECT_TO_JSVAL( scriptTriggersJS );
				break;
			}
			case CIP_WORLDNUMBER:	*vp = INT_TO_JSVAL( gPriv->WorldNumber() );			break;
			case CIP_INSTANCEID:	*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );		break;
			case CIP_AMOUNT:		*vp = INT_TO_JSVAL( gPriv->GetAmount() );			break;

				// We should Accquie an object here and return that, so you can do
				// item.container.name
			case CIP_CONTAINER:
				TempSerial = gPriv->GetContSerial();

				if( TempSerial == INVALIDSERIAL )
				{
					// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					if( TempSerial >= BASEITEMSERIAL )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
					{
						CItem *myCont = CalcItemObjFromSer( TempSerial );
						if( ValidateObject( myCont ))
						{
							JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, myCont, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
							*vp = OBJECT_TO_JSVAL( myItem );
						}
						else
						{
							*vp = JSVAL_NULL;
						}
					}
					else
					{
						CChar *chCont = CalcCharObjFromSer( TempSerial );
						if( ValidateObject( chCont ))
						{
							JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, chCont, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
							*vp = OBJECT_TO_JSVAL( myChar );
						}
						else
						{
							*vp = JSVAL_NULL;
						}
					}
				}
				break;
			case CIP_TYPE:			*vp = INT_TO_JSVAL( static_cast<UI16>( gPriv->GetType() ));		break;
			case CIP_MORE:
			{
				int64_t moreVal = gPriv->GetTempVar( CITV_MORE );
				if( moreVal < INT_MIN || moreVal > INT_MAX )
				{
					 JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE ));
				}
				break;
			}
			case CIP_MORE0:
			{
				int64_t more0Val = gPriv->GetTempVar( CITV_MORE0 );
				if( more0Val < INT_MIN || more0Val > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE0 ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE0 ));
				}
				break;
			}
			case CIP_MORE1:
			{
				int64_t more1Val = gPriv->GetTempVar( CITV_MORE1 );
				if( more1Val < INT_MIN || more1Val > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE1 ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE1 ));
				}
				break;
			}
			case CIP_MORE2:
			{
				int64_t more2Val = gPriv->GetTempVar( CITV_MORE2 );
				if( more2Val < INT_MIN || more2Val > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE2 ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE2 ));
				}
				break;
			}
			case CIP_MOREX:
			{
				int64_t morexVal = gPriv->GetTempVar( CITV_MOREX );
				if( morexVal < INT_MIN || morexVal > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MOREX ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MOREX ));
				}
				break;
			}
			case CIP_MOREY:
			{
				int64_t moreyVal = gPriv->GetTempVar( CITV_MOREY );
				if( moreyVal < INT_MIN || moreyVal > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MOREY ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MOREY ));
				}
				break;
			}
			case CIP_MOREZ:
			{
				int64_t morezVal = gPriv->GetTempVar( CITV_MOREZ );
				if( morezVal < INT_MIN || morezVal > INT_MAX )
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MOREZ ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MOREZ ));
				}
				break;
			}
			case CIP_MOVABLE:		*vp = INT_TO_JSVAL( gPriv->GetMovable() );			break;
			case CIP_ATT:			*vp = INT_TO_JSVAL( RandomNum( gPriv->GetLoDamage(), gPriv->GetHiDamage() ));	break;
			case CIP_LAYER:			*vp = INT_TO_JSVAL( gPriv->GetLayer() );			break;
			case CIP_ITEMSINSIDE:	*vp = INT_TO_JSVAL( gPriv->GetContainsList()->Num() );	break;
			case CIP_TOTALITEMCOUNT: *vp = INT_TO_JSVAL( GetTotalItemCount( gPriv ));	break;
			case CIP_DECAYABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsDecayable() );		break;
			case CIP_DECAYTIME:		JS_NewNumberValue( cx, gPriv->GetDecayTime(), vp );	break;
			case CIP_LODAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetLoDamage() );			break;
			case CIP_HIDAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetHiDamage() );			break;
			case CIP_AC:			*vp = INT_TO_JSVAL( gPriv->GetArmourClass() );		break;
			case CIP_DEF:			*vp = INT_TO_JSVAL( gPriv->GetResist( PHYSICAL ));	break;
			case CIP_HEALTHREGENBONUS:	*vp = INT_TO_JSVAL( gPriv->GetHealthRegenBonus() );			break;
			case CIP_STAMINAREGENBONUS:	*vp = INT_TO_JSVAL( gPriv->GetStaminaRegenBonus() );			break;
			case CIP_MANAREGENBONUS:	*vp = INT_TO_JSVAL( gPriv->GetManaRegenBonus() );			break;
			case CIP_RESISTCOLD:	*vp = INT_TO_JSVAL( gPriv->GetResist( COLD ));		break;
			case CIP_RESISTHEAT:	*vp = INT_TO_JSVAL( gPriv->GetResist( HEAT ));		break;
			case CIP_RESISTLIGHT:	*vp = INT_TO_JSVAL( gPriv->GetResist( LIGHT ));	break;
			case CIP_RESISTLIGHTNING:	*vp = INT_TO_JSVAL( gPriv->GetResist( LIGHTNING ));break;
			case CIP_RESISTPOISON:	*vp = INT_TO_JSVAL( gPriv->GetResist( POISON ));	break;
			case CIP_RESISTRAIN:	*vp = INT_TO_JSVAL( gPriv->GetResist( RAIN ));		break;
			case CIP_RESISTSNOW:	*vp = INT_TO_JSVAL( gPriv->GetResist( SNOW ));		break;
			case CIP_DAMAGECOLD:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( COLD ));	break;
			case CIP_DAMAGEHEAT:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( HEAT ));	break;
			case CIP_DAMAGELIGHT:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( LIGHT ));	break;
			case CIP_DAMAGELIGHTNING:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( LIGHTNING ));	break;
			case CIP_DAMAGEPOISON:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( POISON ));	break;
			case CIP_DAMAGERAIN:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( RAIN ));	break;
			case CIP_DAMAGESNOW:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( SNOW ));	break;
			case CIP_SWINGSPEEDINCREASE:	*vp = INT_TO_JSVAL( gPriv->GetSwingSpeedIncrease() );			break;
			case CIP_SPEED:			*vp = INT_TO_JSVAL( gPriv->GetSpeed() );			break;
			case CIP_DURABILITYHPBONUS:	*vp = INT_TO_JSVAL( gPriv->GetDurabilityHpBonus() );			break;
			case CIP_LOWERSTATREQ:		*vp = INT_TO_JSVAL( gPriv->GetLowerStatReq() );			break;
			case CIP_HEALTHLEECH:	*vp = INT_TO_JSVAL( gPriv->GetHealthLeech() );			break;
			case CIP_STAMINALEECH:	*vp = INT_TO_JSVAL( gPriv->GetStaminaLeech() );			break;
			case CIP_MANALEECH:		*vp = INT_TO_JSVAL( gPriv->GetManaLeech() );			break;
			case CIP_HITCHANCE:	*vp = INT_TO_JSVAL( gPriv->GetHitChance() );			break;
			case CIP_DEFENSECHANCE:	*vp = INT_TO_JSVAL( gPriv->GetDefenseChance() );			break;
			case CIP_LUCK:				*vp = INT_TO_JSVAL( gPriv->GetLuck() );				break;
			case CIP_HEALTHBONUS:		*vp = INT_TO_JSVAL( gPriv->GetHealthBonus() );			break;
			case CIP_STAMINABONUS:		*vp = INT_TO_JSVAL( gPriv->GetStaminaBonus() );			break;
			case CIP_MANABONUS:		*vp = INT_TO_JSVAL( gPriv->GetManaBonus() );			break;
			case CIP_ARTIFACTRARITY:		*vp = INT_TO_JSVAL( gPriv->GetArtifactRarity() );			break;
			case CIP_DAMAGEINCREASE:		*vp = INT_TO_JSVAL( gPriv->GetDamageIncrease() );			break;

			case CIP_NAME2:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName2().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_ISCHAR:	*vp = JSVAL_FALSE;								break;
			case CIP_ISITEM:	*vp = JSVAL_TRUE;								break;
			case CIP_ISSPAWNER:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetObjType() == OT_SPAWNER );	break;
			case CIP_RACE:
			{
				CRace *TempRace = nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				if( TempRace == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myRace	= JSEngine->AcquireObject( IUE_RACE, TempRace, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myRace );
				}
				break;
			}
			case CIP_MAXHP:			*vp = INT_TO_JSVAL( gPriv->GetMaxHP() );		break;
			case CIP_MAXUSES:		*vp = INT_TO_JSVAL( gPriv->GetMaxUses() );		break;
			case CIP_USESLEFT:		*vp = INT_TO_JSVAL( gPriv->GetUsesLeft() );		break;
			case CIP_RANK:			*vp = INT_TO_JSVAL( gPriv->GetRank() );			break;
			case CIP_CREATOR:		*vp = INT_TO_JSVAL( gPriv->GetCreator() );		break;
			case CIP_POISON:		*vp = INT_TO_JSVAL( gPriv->GetPoisoned() );		break;
			case CIP_POISONEDBY:	*vp = INT_TO_JSVAL( gPriv->GetPoisonedBy() );	break;
			case CIP_POISONCHARGES:	*vp = INT_TO_JSVAL( gPriv->GetPoisonCharges() );	break;
			case CIP_DIR:			*vp = INT_TO_JSVAL( gPriv->GetDir() );			break;
			case CIP_WIPABLE:		*vp = INT_TO_JSVAL( gPriv->IsWipeable() );		break;
			case CIP_BUYVALUE:		*vp = INT_TO_JSVAL( gPriv->GetBuyValue() );		break;
			case CIP_SELLVALUE:		*vp = INT_TO_JSVAL( gPriv->GetSellValue() );	break;
			case CIP_VENDORPRICE:	*vp = INT_TO_JSVAL( gPriv->GetVendorPrice() );	break;
			case CIP_RESTOCK:		*vp = INT_TO_JSVAL( gPriv->GetRestock() );		break;
			case CIP_GOOD:			*vp = INT_TO_JSVAL( gPriv->GetGood() );			break;
			case CIP_DIVINELOCK:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDivineLocked() ); break;
			case CIP_WEIGHT:		*vp = INT_TO_JSVAL( gPriv->GetWeight() );		break;
			case CIP_WEIGHTMAX:		*vp = INT_TO_JSVAL( gPriv->GetWeightMax() );	break;
			case CIP_BASEWEIGHT:	*vp = INT_TO_JSVAL( gPriv->GetBaseWeight() );	break;
			case CIP_MAXITEMS:		*vp = INT_TO_JSVAL( gPriv->GetMaxItems() );		break;
			case CIP_STRENGTH:		*vp = INT_TO_JSVAL( gPriv->GetStrength() );		break;
			case CIP_DEXTERITY:		*vp = INT_TO_JSVAL( gPriv->GetDexterity() );	break;
			case CIP_INTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->GetIntelligence() );	break;
			case CIP_CORPSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCorpse() );	break;
			case CIP_DESC:
				tString = JS_NewStringCopyZ( cx, gPriv->GetDesc().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_EVENT:
				tString = JS_NewStringCopyZ( cx, gPriv->GetEvent().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_TEMPLASTTRADED:
				JS_NewNumberValue( cx,  gPriv->GetTempLastTraded(), vp );
				break;
			case CIP_TEMPTIMER:
				JS_NewNumberValue( cx, gPriv->GetTempTimer(), vp );
				break;
			case CIP_SHOULDSAVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->ShouldSave() );			break;
			case CIP_ISNEWBIE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsNewbie() );			break;
			case CIP_ISDISPELLABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDispellable() );		break;
			case CIP_MADEWITH:		*vp = INT_TO_JSVAL( gPriv->GetMadeWith() );				break;
			case CIP_ENTRYMADEFROM:	*vp = INT_TO_JSVAL( gPriv->EntryMadeFrom() );			break;
			case CIP_ISPILEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPileable() );			break;
			case CIP_ISMARKEDBYMAKER:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsMarkedByMaker() );	break;
			case CIP_ISDYEABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsDyeable() );			break;
			case CIP_ISDAMAGEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDamageable() );		break;
			case CIP_ISWIPEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsWipeable() );			break;
			case CIP_ISGUARDED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );			break;
			case CIP_ISDOOROPEN:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDoorOpen() );			break;
			case CIP_ISFIELDSPELL:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsFieldSpell() );		break;
			case CIP_ISLOCKEDDOWN:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsLockedDown() );		break;
			case CIP_ISSHIELDTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsShieldType() );		break;
			case CIP_ISMETALTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsMetalType() );			break;
			case CIP_ISLEATHERTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsLeatherType() );		break;
			case CIP_CANBELOCKEDDOWN:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanBeLockedDown() );	break;
			case CIP_TITHING:		*vp = INT_TO_JSVAL( gPriv->GetTithing() );					break;
			case CIP_ISCONTTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsContType() );			break;
			case CIP_CARVESECTION:	*vp = INT_TO_JSVAL( gPriv->GetCarve() );				break;
			case CIP_AMMOID:		*vp = INT_TO_JSVAL( gPriv->GetAmmoId() );				break;
			case CIP_AMMOHUE:		*vp = INT_TO_JSVAL( gPriv->GetAmmoHue() );				break;
			case CIP_AMMOFX:		*vp = INT_TO_JSVAL( gPriv->GetAmmoFX() );				break;
			case CIP_AMMOFXHUE:		*vp = INT_TO_JSVAL( gPriv->GetAmmoFXHue() );			break;
			case CIP_AMMOFXRENDER:	*vp = INT_TO_JSVAL( gPriv->GetAmmoFXRender() );			break;
			case CIP_MAXRANGE:		*vp = INT_TO_JSVAL( gPriv->GetMaxRange() );				break;
			case CIP_BASERANGE:		*vp = INT_TO_JSVAL( gPriv->GetBaseRange() );			break;
			case CIP_REGION:
			{
				CTownRegion *myReg = gPriv->GetRegion();
				if( myReg == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, myReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myTown );
				}
				break;
			}
			case CIP_SPAWNSERIAL:
				JS_NewNumberValue( cx, gPriv->GetSpawn(), vp );
				break;
			case CIP_ORIGIN:
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_ISITEMHELD:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsHeldOnCursor() );		break;
			case CIP_STEALABLE:		*vp = INT_TO_JSVAL( gPriv->GetStealable() );				break;

				// The following entries are specifically for CSpawnItem objects
			case CIP_SPAWNSECTION:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CSpawnItem *>( gPriv ))->GetSpawnSection().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				break;
			case CIP_SECTIONALIST:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					*vp = INT_TO_JSVAL(( static_cast<CSpawnItem *>( gPriv ))->IsSectionAList() );
				}
				break;
			case CIP_MININTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					*vp = INT_TO_JSVAL(( static_cast<CSpawnItem *>( gPriv ))->GetInterval( 0 ));
				}
				break;
			case CIP_MAXINTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					*vp = INT_TO_JSVAL(( static_cast<CSpawnItem *>( gPriv ))->GetInterval( 1 ));
				}
				break;
			case CIP_MULTI:
				CMultiObj *multi;
				multi = gPriv->GetMultiObj();

				if( !ValidateObject( multi ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
				// The following entries are specifically for CMultiObj objects
			case CIP_LOCKDDOWNS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetLockdownCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXLOCKDOWNS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxLockdowns() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_TRASHCONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetTrashContainerCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXTRASHCONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxTrashContainers() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_SECURECONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetSecureContainerCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXSECURECONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxSecureContainers() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_FRIENDS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetFriendCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXFRIENDS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxFriends() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_GUESTS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetGuestCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXGUESTS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxGuests() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_OWNERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetOwnerCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXOWNERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxOwners() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BANS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetBanCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXBANS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxBans() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_VENDORS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetVendorCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXVENDORS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxVendors() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_DEED:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetDeed().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_ISPUBLIC:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = BOOLEAN_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetPublicStatus() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BUILDTIMESTAMP:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetBuildTimestamp().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_TRADETIMESTAMP:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetTradeTimestamp().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BANX:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetBanX() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BANY:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetBanY() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MOVETYPE:
				if( gPriv->GetObjType() == OT_BOAT )
				{
					*vp = INT_TO_JSVAL(( static_cast<CBoatObj *>( gPriv )->GetMoveType() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CItemProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CItem *gPriv = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_ITEM );

	if( propID != 0xFFFF )
	{
		TIMERVAL newTime;
		switch( propID )
		{
			case CIP_SECTIONID:		gPriv->SetSectionId( encaps.toString() );					break;
			case CIP_NAME:			gPriv->SetName( encaps.toString() );						break;
			case CIP_X:				gPriv->SetLocation( static_cast<SI16>( encaps.toInt() ), gPriv->GetY(), gPriv->GetZ() );	break;
			case CIP_Y:				gPriv->SetLocation( gPriv->GetX(), static_cast<SI16>( encaps.toInt() ), gPriv->GetZ() );	break;
			case CIP_Z:				gPriv->SetZ( static_cast<SI08>( encaps.toInt() ));			break;
			case CIP_ID:			gPriv->SetId( static_cast<UI16>( encaps.toInt() ));			break;
			case CIP_COLOUR:		gPriv->SetColour( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_OWNER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar*>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetOwner( myChar );
				}
				else
				{
					gPriv->SetOwner( nullptr );
				}
				break;
			case CIP_VISIBLE:		gPriv->SetVisible( static_cast<VisibleTypes>( encaps.toInt() ));	break;
			case CIP_SERIAL:																	break;
			case CIP_HEALTH:		gPriv->SetHP( static_cast<SI16>( encaps.toInt() ));			break;
			case CIP_SCRIPTTRIGGER:
			{
				// For backwards compatibility; clears out other scripts and assigns a specific script
				UI16 scriptId = static_cast<UI16>( encaps.toInt() );
				cScript *toExecute	= JSMapping->GetScript( scriptId );
				if( toExecute == nullptr )
				{
					ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
				}
				else
				{
					gPriv->ClearScriptTriggers();
					gPriv->AddScriptTrigger( scriptId );
				}
				break;
			}
			case CIP_SCRIPTTRIGGERS:
			{
				if( *vp != JSVAL_NULL )
				{
					UI16 scriptId = static_cast<UI16>( encaps.toInt() );
					cScript *toExecute	= JSMapping->GetScript( scriptId );
					if( toExecute == nullptr )
					{
						ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
					}
					else
					{
						gPriv->AddScriptTrigger( scriptId );
					}
				}
				else
				{
					// If null value was provided, clear script triggers on object
					gPriv->ClearScriptTriggers();
				}
				break; 
			}
			case CIP_WORLDNUMBER:
				if( !Map->InsideValidWorld( gPriv->GetX(), gPriv->GetY(), static_cast<UI08>( encaps.toInt() )))
					return JS_FALSE;

				gPriv->RemoveFromSight();
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), static_cast<UI08>( encaps.toInt() ), gPriv->GetInstanceId() );
				break;
			case CIP_INSTANCEID:
			{
				gPriv->RemoveFromSight();
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), gPriv->WorldNumber(), static_cast<UI16>( encaps.toInt() ));
				break;
			}
			case CIP_AMOUNT:	gPriv->SetAmount( static_cast<UI32>( encaps.toInt() )); 	break;
			case CIP_CONTAINER:
				if( *vp != JSVAL_NULL )
				{
					CBaseObject *myObj = static_cast<CBaseObject*>( encaps.toObject() );
					if( !ValidateObject( myObj ))
						break;

					gPriv->SetCont( myObj, true );
				}
				else
				{
					gPriv->SetCont( nullptr, true );
				}
				break;
			case CIP_TYPE:			gPriv->SetType( static_cast<ItemTypes>( encaps.toInt() )); 	break;
			case CIP_MORE:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE, encaps.toInt() );
				}
				break;
			}
			case CIP_MORE0:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE0, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE0, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE0, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE0, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE0, encaps.toInt() );
				}
				break;
			}
			case CIP_MORE1:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE1, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE1, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE1, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE1, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE1, encaps.toInt() );
				}
				break;
			}
			case CIP_MORE2:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE2, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE2, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE2, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE2, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE2, encaps.toInt() );
				}
				break;
			}
			case CIP_MOREX:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//"));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MOREX, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREX, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREX, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREX, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MOREX, encaps.toInt() );
				}
				break;
			}
			case CIP_MOREY:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//"));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MOREY, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREY, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREY, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREY, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MOREY, encaps.toInt() );
				}
				break;
			}
			case CIP_MOREZ:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MOREZ, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREZ, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREZ, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREZ, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MOREZ, encaps.toInt() );
				}
				break;
			}
			case CIP_MOVABLE:		gPriv->SetMovable( static_cast<SI08>( encaps.toInt() ));	break;
			case CIP_ATT:			
				gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));
				gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));
				break;
			case CIP_LAYER:			gPriv->SetLayer( static_cast<ItemLayers>( encaps.toInt() ));	break;
			case CIP_ITEMSINSIDE:																break;
			case CIP_DECAYABLE:		gPriv->SetDecayable( encaps.toBool() );				 		break;
			case CIP_DECAYTIME:
			{
				jsdouble newTime_double;
				JS_ValueToNumber( cx, *vp, &newTime_double );
				newTime = 0;
				if( newTime_double != 0 )
				{
					newTime = BuildTimeValue( static_cast<R64>( newTime_double ));
				}
				gPriv->SetDecayTime( newTime );
				break;
			}
			case CIP_LODAMAGE:		gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));			break;
			case CIP_HIDAMAGE:		gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));			break;
			case CIP_AC:			gPriv->SetArmourClass( static_cast<UI08>( encaps.toInt() ));		break;
			case CIP_DEF:			gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), PHYSICAL );	break;
			case CIP_HEALTHREGENBONUS:	gPriv->SetHealthRegenBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_STAMINAREGENBONUS:	gPriv->SetStaminaRegenBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_MANAREGENBONUS:	gPriv->SetManaRegenBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_RESISTCOLD:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), COLD );		break;
			case CIP_RESISTHEAT:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), HEAT );		break;
			case CIP_RESISTLIGHT:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), LIGHT );		break;
			case CIP_RESISTLIGHTNING:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), LIGHTNING );	break;
			case CIP_RESISTPOISON:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), POISON );		break;
			case CIP_RESISTRAIN:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), RAIN );		break;
			case CIP_RESISTSNOW:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), SNOW );		break;
			case CIP_DAMAGECOLD:	gPriv->SetWeatherDamage( COLD, encaps.toBool() );			break;
			case CIP_DAMAGEHEAT:	gPriv->SetWeatherDamage( HEAT, encaps.toBool() );			break;
			case CIP_DAMAGELIGHT:	gPriv->SetWeatherDamage( LIGHT, encaps.toBool() );			break;
			case CIP_DAMAGELIGHTNING:	gPriv->SetWeatherDamage( LIGHTNING, encaps.toBool() );	break;
			case CIP_DAMAGEPOISON:	gPriv->SetWeatherDamage( POISON, encaps.toBool() );			break;
			case CIP_DAMAGERAIN:	gPriv->SetWeatherDamage( RAIN, encaps.toBool() );			break;
			case CIP_DAMAGESNOW:	gPriv->SetWeatherDamage( SNOW, encaps.toBool() );			break;
			case CIP_SPEED:			gPriv->SetSpeed( static_cast<UI08>( encaps.toInt() ));		break;
			case CIP_LOWERSTATREQ:	gPriv->SetLowerStatReq( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_SWINGSPEEDINCREASE:	gPriv->SetSwingSpeedIncrease( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_HEALTHLEECH:	gPriv->SetHealthLeech( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_STAMINALEECH:	gPriv->SetStaminaLeech( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_MANALEECH:		gPriv->SetManaLeech( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_HITCHANCE:	gPriv->SetHitChance( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_DEFENSECHANCE:	gPriv->SetDefenseChance( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_LUCK:			gPriv->SetLuck( static_cast<SI16>( encaps.toInt() ));		break;
			case CIP_HEALTHBONUS:		gPriv->SetHealthBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_STAMINABONUS:		gPriv->SetStaminaBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_MANABONUS:		gPriv->SetManaBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_ARTIFACTRARITY:	gPriv->SetArtifactRarity( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_DURABILITYHPBONUS:	gPriv->SetDurabilityHpBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_DAMAGEINCREASE:	gPriv->SetDamageIncrease( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_NAME2:			gPriv->SetName2( encaps.toString() );						break;
			case CIP_RACE:			gPriv->SetRace( static_cast<RACEID>( encaps.toInt() ));		break;
			case CIP_MAXHP:			gPriv->SetMaxHP( static_cast<SI16>( encaps.toInt() ));		break;
			case CIP_MAXUSES:		gPriv->SetMaxUses( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_USESLEFT:		gPriv->SetUsesLeft( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_RANK:			gPriv->SetRank( static_cast<SI08>( encaps.toInt() ));		break;
			case CIP_CREATOR:		gPriv->SetCreator( static_cast<SERIAL>( encaps.toInt() ));	break;
			case CIP_POISON:		gPriv->SetPoisoned( static_cast<UI08>( encaps.toInt() ));	break;
			case CIP_POISONEDBY:	gPriv->SetPoisonedBy( static_cast<UI32>( encaps.toInt() ));	break;
			case CIP_POISONCHARGES:	gPriv->SetPoisonCharges( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_DIR:			gPriv->SetDir( static_cast<SI16>( encaps.toInt() ));		break;
			case CIP_WIPABLE:		gPriv->SetWipeable( encaps.toBool() );						break;
			case CIP_BUYVALUE:		gPriv->SetBuyValue( static_cast<UI32>( encaps.toInt() ));	break;
			case CIP_SELLVALUE:		gPriv->SetSellValue( static_cast<UI32>( encaps.toInt() ));	break;
			case CIP_VENDORPRICE:	gPriv->SetVendorPrice( static_cast<UI32>( encaps.toInt() ));break;
			case CIP_RESTOCK:		gPriv->SetRestock( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_GOOD:			gPriv->SetGood( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_DIVINELOCK:	gPriv->SetDivineLock( encaps.toBool() );					break;
			case CIP_WEIGHT:		gPriv->SetWeight( static_cast<SI32>( encaps.toInt() ));		break;
			case CIP_WEIGHTMAX:		gPriv->SetWeightMax( static_cast<SI32>( encaps.toInt() ));	break;
			case CIP_BASEWEIGHT:	gPriv->SetBaseWeight( static_cast<SI32>( encaps.toInt() ));	break;
			case CIP_MAXITEMS:		gPriv->SetMaxItems( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_STRENGTH:		gPriv->SetStrength( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_DEXTERITY:		gPriv->SetDexterity( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_INTELLIGENCE:	gPriv->SetIntelligence( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_CORPSE:		gPriv->SetCorpse( encaps.toBool() );						break;
			case CIP_DESC:			gPriv->SetDesc( encaps.toString() );						break;
			case CIP_EVENT:			gPriv->SetEvent( encaps.toString() );						break;
			case CIP_TEMPLASTTRADED:
			{
				jsdouble newTime_double;
				JS_ValueToNumber( cx, *vp, &newTime_double );
				newTime = 0;
				if( newTime_double != 0 )
				{
					newTime = BuildTimeValue( static_cast<R64>( newTime_double ));
				}
				gPriv->SetTempLastTraded( newTime );
				break;
			}
			case CIP_TEMPTIMER:
			{
				jsdouble newTime_double;
				JS_ValueToNumber( cx, *vp, &newTime_double );
				newTime = 0;
				if( newTime_double != 0 )
				{
					newTime = BuildTimeValue( static_cast<R64>( newTime_double ));
				}
				gPriv->SetTempTimer( newTime );
				break;
			}
			case CIP_SHOULDSAVE:	gPriv->ShouldSave( encaps.toBool() );						break;
			case CIP_ISNEWBIE:		gPriv->SetNewbie( encaps.toBool() );						break;
			case CIP_ISDISPELLABLE:	gPriv->SetDispellable( encaps.toBool() );					break;
			case CIP_MADEWITH:		gPriv->SetMadeWith( static_cast<SI08>( encaps.toInt() ));	break;
			case CIP_ENTRYMADEFROM:	gPriv->EntryMadeFrom( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_ISPILEABLE:	gPriv->SetPileable( encaps.toBool() );						break;
			case CIP_ISMARKEDBYMAKER:	gPriv->SetMakersMark( encaps.toBool() );				break;
			case CIP_ISDYEABLE:		gPriv->SetDye( encaps.toBool() );							break;
			case CIP_ISDAMAGEABLE:	gPriv->SetDamageable( encaps.toBool() );					break;
			case CIP_ISWIPEABLE:	gPriv->SetWipeable( encaps.toBool() );						break;
			case CIP_ISGUARDED:		gPriv->SetGuarded( encaps.toBool() );						break;
			case CIP_ISDOOROPEN:	gPriv->SetDoorOpen( encaps.toBool() );						break;
			case CIP_CARVESECTION:	gPriv->SetCarve( encaps.toInt() );							break;
			case CIP_AMMOID:		gPriv->SetAmmoId( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_AMMOHUE:		gPriv->SetAmmoHue( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_AMMOFX:		gPriv->SetAmmoFX( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_AMMOFXHUE:		gPriv->SetAmmoFXHue( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_AMMOFXRENDER:	gPriv->SetAmmoFXRender( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_MAXRANGE:		gPriv->SetMaxRange( static_cast<UI08>( encaps.toInt() ));	break;
			case CIP_BASERANGE:		gPriv->SetBaseRange( static_cast<UI08>( encaps.toInt() ));	break;
			case CIP_REGION:		gPriv->SetRegion( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_ORIGIN:		gPriv->SetOrigin( cwmWorldState->ServerData()->EraStringToEnum( encaps.toString() ));						break;
			case CIP_ISITEMHELD:	gPriv->SetHeldOnCursor( encaps.toBool() );					break;
			case CIP_STEALABLE:		gPriv->SetStealable( static_cast<UI08>( encaps.toInt() ));	break;

				// The following entries are specifically for CSpawnItem objects
			case CIP_SPAWNSECTION:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->SetSpawnSection( encaps.toString() );
				}
				break;
			case CIP_SECTIONALIST:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->IsSectionAList( encaps.toBool() );
				}
				break;
			case CIP_MININTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->SetInterval( 0, static_cast<UI08>( encaps.toInt() ));
				}
				break;
			case CIP_MAXINTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->SetInterval( 1, static_cast<UI08>( encaps.toInt() ));
				}
				break;
				// Multis only
			case CIP_MAXLOCKDOWNS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxLockdowns( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXTRASHCONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxTrashContainers( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXSECURECONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxSecureContainers( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXFRIENDS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxFriends( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXGUESTS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxGuests( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXOWNERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxOwners( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXBANS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxBans( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXVENDORS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxVendors( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_DEED:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetDeed( encaps.toString() );
				}
				break;
			case CIP_ISPUBLIC:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetPublicStatus( encaps.toBool() );	
				}
				break;
			case CIP_BUILDTIMESTAMP:
			{
				if( gPriv->GetObjType() == OT_MULTI )
				{
					time_t buildTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
					( static_cast<CMultiObj *>( gPriv ))->SetBuildTimestamp( buildTimestamp );
				}
				break;
			}
			case CIP_TRADETIMESTAMP:
			{
				if( gPriv->GetObjType() == OT_MULTI )
				{
					time_t tradeTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
					( static_cast<CMultiObj *>( gPriv ))->SetTradeTimestamp( tradeTimestamp );
				}
				break;
			}
			case CIP_BANX:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetBanX( static_cast<SI16>( encaps.toInt() ));
				}
				break;
			case CIP_BANY:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetBanY( static_cast<SI16>( encaps.toInt() ));
				}
				break;
			case CIP_MOVETYPE:
				if( gPriv->GetObjType() == OT_BOAT )
				{
					( static_cast<CBoatObj *>( gPriv ))->SetMoveType( static_cast<SI08>( encaps.toInt() ));
				}
				break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

IMPL_GETS_OBJ( CItem, sectionID, CItem, setString, GetSectionId().c_str() )
IMPL_GETS_OBJ( CItem, name, CItem, setString, GetName().c_str() )
IMPL_GETS_OBJ( CItem, title, CItem, setString, GetTitle().c_str() )
IMPL_GET_OBJ( CItem, x, CItem, setInt32, GetX() )
IMPL_GET_OBJ( CItem, y, CItem, setInt32, GetY() )
IMPL_GET_OBJ( CItem, z, CItem, setInt32, GetZ() )
IMPL_GET_OBJ( CItem, id, CItem, setInt32, GetId() )
IMPL_GET_OBJ( CItem, colour, CItem, setInt32, GetColour() )
IMPL_GET_OBJ( CItem, color, CItem, setInt32, GetColour() )
IMPL_GET_OBJ( CItem, visible, CItem, setInt32, GetVisible() )
IMPL_GET_OBJ( CItem, serial, CItem, setNumber, GetSerial() )
IMPL_GET_OBJ( CItem, health, CItem, setInt32, GetHP() )
IMPL_GET_OBJ( CItem, worldnumber, CItem, setInt32, WorldNumber() )
IMPL_GET_OBJ( CItem, worldNumber, CItem, setInt32, WorldNumber() )
IMPL_GET_OBJ( CItem, instanceID, CItem, setInt32, GetInstanceId() )
IMPL_GET_OBJ( CItem, amount, CItem, setInt32, GetAmount() )
IMPL_GET_OBJ( CItem, type, CItem, setInt32, GetType() )
IMPL_SETS( CItem, sectionID, CItem, toString, SetSectionId )
IMPL_SETS( CItem, name, CItem, toString, SetName )

#define IMPL_ITEM_INT_SET( attr, typeName, statement )                    \
FDCLS( CItem, attr )                                                      \
{                                                                         \
	FNARGS                                                                  \
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );       \
	if( !ValidateObject( item )) return false;                              \
	const typeName value = static_cast<typeName>( args.get( 0 ).toInt32() );\
	statement;                                                              \
	return true;                                                            \
}

IMPL_ITEM_INT_SET( x, SI16, item->SetLocation( value, item->GetY(), item->GetZ() ))
IMPL_ITEM_INT_SET( y, SI16, item->SetLocation( item->GetX(), value, item->GetZ() ))
IMPL_ITEM_INT_SET( z, SI08, item->SetZ( value ))
IMPL_ITEM_INT_SET( id, UI16, item->SetId( value ))
IMPL_ITEM_INT_SET( colour, UI16, item->SetColour( value ))
IMPL_ITEM_INT_SET( color, UI16, item->SetColour( value ))
IMPL_ITEM_INT_SET( visible, VisibleTypes, item->SetVisible( value ))
IMPL_ITEM_INT_SET( health, SI16, item->SetHP( value ))
IMPL_ITEM_INT_SET( worldnumber, UI08, item->SetLocation( item->GetX(), item->GetY(), item->GetZ(), value, item->GetInstanceId() ))
IMPL_ITEM_INT_SET( worldNumber, UI08, item->SetLocation( item->GetX(), item->GetY(), item->GetZ(), value, item->GetInstanceId() ))
IMPL_ITEM_INT_SET( instanceID, UI16, item->SetLocation( item->GetX(), item->GetY(), item->GetZ(), item->WorldNumber(), value ))
IMPL_ITEM_INT_SET( amount, UI32, item->SetAmount( value ))
IMPL_ITEM_INT_SET( type, ItemTypes, item->SetType( value ))

#undef IMPL_ITEM_INT_SET

FDCLS( CItem, serial ) { return true; }
FDCLS( CItem, title ) { return true; }

static bool SetLegacyItemProperty( JSContext *cx, JS::HandleObject itemObject, int32_t propertyId, JS::HandleValue value )
{
	JS::RootedValue rootedValue( cx, value );
	return CItemProps_setProperty( cx, itemObject, JS::PropertyKey::Int( propertyId ), false, rootedValue.address() );
}

#define IMPL_ITEM_LEGACY_SET( attr, propertyId )                         \
FDCLS( CItem, attr )                                                     \
{                                                                        \
	FNARGS                                                                 \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 )); \
}

IMPL_ITEM_LEGACY_SET( owner,          CIP_OWNER )
IMPL_ITEM_LEGACY_SET( container,      CIP_CONTAINER )
IMPL_ITEM_LEGACY_SET( scriptTrigger,  CIP_SCRIPTTRIGGER )
IMPL_ITEM_LEGACY_SET( scriptTriggers, CIP_SCRIPTTRIGGERS )

#undef IMPL_ITEM_LEGACY_SET

FDCLG( CItem, owner )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	CChar *owner = item->GetOwnerObj();
	if( !ValidateObject( owner )) { args.rval().setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( IUE_CHAR, owner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

FDCLG( CItem, container )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	const SERIAL serial = item->GetContSerial();
	if( serial == INVALIDSERIAL ) { args.rval().setNull(); return true; }
	CBaseObject *container = serial >= BASEITEMSERIAL ? static_cast<CBaseObject *>( CalcItemObjFromSer( serial )) : static_cast<CBaseObject *>( CalcCharObjFromSer( serial ));
	if( !ValidateObject( container )) { args.rval().setNull(); return true; }
	const auto objectType = serial >= BASEITEMSERIAL ? IUE_ITEM : IUE_CHAR;
	JSObject *wrapper = JSEngine->AcquireObject( objectType, container, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

FDCLG( CItem, scriptTrigger )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	const auto &triggers = item->GetScriptTriggers();
	args.rval().setInt32( triggers.empty() ? 0 : triggers.back() );
	return true;
}

FDCLG( CItem, scriptTriggers )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	const auto &triggers = item->GetScriptTriggers();
	JS::RootedObject result( cx, JS::NewArrayObject( cx, triggers.size() ));
	if( result == nullptr ) return false;
	for( uint32_t index = 0; index < triggers.size(); ++index )
	{
		JS::RootedValue value( cx, JS::Int32Value( triggers[index] ));
		if( !JS_SetElement( cx, result, index, value )) return false;
	}
	args.rval().setObject( *result );
	return true;
}

#define ITEM_METADATA_SET( attr, propertyId )                            \
FDCLS( CItem, attr )                                                     \
{                                                                        \
	FNARGS                                                                 \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 )); \
}
ITEM_METADATA_SET( oldX, CIP_OLDX )
ITEM_METADATA_SET( oldY, CIP_OLDY )
ITEM_METADATA_SET( oldZ, CIP_OLDZ )
ITEM_METADATA_SET( skin, CIP_COLOUR )
ITEM_METADATA_SET( hue, CIP_COLOUR )
ITEM_METADATA_SET( scripttrigger, CIP_SCRIPTTRIGGER )
ITEM_METADATA_SET( decaytime, CIP_DECAYTIME )
ITEM_METADATA_SET( race, CIP_RACE )
ITEM_METADATA_SET( multi, CIP_MULTI )
ITEM_METADATA_SET( region, CIP_REGION )
#undef ITEM_METADATA_SET

FDCLG( CItem, oldX ) { FNARGS auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 ); args.rval().setInt32( item->GetOldLocation().x ); return true; }
FDCLG( CItem, oldY ) { FNARGS auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 ); args.rval().setInt32( item->GetOldLocation().y ); return true; }
FDCLG( CItem, oldZ ) { FNARGS auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 ); args.rval().setInt32( item->GetOldLocation().z ); return true; }
IMPL_GET_OBJ( CItem, skin, CItem, setInt32, GetColour() )
IMPL_GET_OBJ( CItem, hue, CItem, setInt32, GetColour() )

FDCLG( CItem, scripttrigger )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	const auto &triggers = item->GetScriptTriggers();
	args.rval().setInt32( triggers.empty() ? 0 : triggers.back() );
	return true;
}

IMPL_GET_OBJ( CItem, decaytime, CItem, setNumber, GetDecayTime() )

FDCLG( CItem, race )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	CRace *race = Races->Race( item->GetRace() );
	if( race == nullptr ) { args.rval().setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( IUE_RACE, race, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

FDCLG( CItem, multi )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	CMultiObj *multi = item->GetMultiObj();
	if( !ValidateObject( multi )) { args.rval().setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

FDCLG( CItem, region )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	CTownRegion *region = item->GetRegion();
	if( region == nullptr ) { args.rval().setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( IUE_REGION, region, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

FDCLG( CItem, spawnSerial )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	args.rval().setNumber( item->GetSpawn() );
	return true;
}

#define ITEM_SPAWNER_SET( attr, propertyId )                            \
FDCLS( CItem, attr )                                                    \
{                                                                       \
	FNARGS                                                                \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
ITEM_SPAWNER_SET( spawnsection, CIP_SPAWNSECTION )
ITEM_SPAWNER_SET( sectionalist, CIP_SECTIONALIST )
ITEM_SPAWNER_SET( mininterval, CIP_MININTERVAL )
ITEM_SPAWNER_SET( maxinterval, CIP_MAXINTERVAL )
#undef ITEM_SPAWNER_SET

FDCLG( CItem, spawnsection )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	if( !ValidateObject( item )) return false;
	if( item->GetObjType() != OT_SPAWNER ) { args.rval().setNull(); return true; }
	JS::RootedString value( cx, JS_NewStringCopyZ( cx, static_cast<CSpawnItem *>( item )->GetSpawnSection().c_str() ));
	if( value == nullptr ) return false;
	args.rval().setString( value );
	return true;
}

#define ITEM_SPAWNER_GET( attr, method, expression )                    \
FDCLG( CItem, attr )                                                    \
{                                                                       \
	FNARGS                                                                \
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );     \
	if( !ValidateObject( item )) return false;                            \
	if( item->GetObjType() != OT_SPAWNER ) { args.rval().setNull(); return true; } \
	args.rval().method( static_cast<CSpawnItem *>( item )->expression );  \
	return true;                                                          \
}
ITEM_SPAWNER_GET( sectionalist, setBoolean, IsSectionAList() )
ITEM_SPAWNER_GET( mininterval, setInt32, GetInterval( 0 ) )
ITEM_SPAWNER_GET( maxinterval, setInt32, GetInterval( 1 ) )
#undef ITEM_SPAWNER_GET

#define ITEM_MULTI_SET( attr, propertyId )                              \
FDCLS( CItem, attr )                                                    \
{                                                                       \
	FNARGS                                                                \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
ITEM_MULTI_SET( lockdowns, CIP_LOCKDDOWNS )
ITEM_MULTI_SET( maxLockdowns, CIP_MAXLOCKDOWNS )
ITEM_MULTI_SET( trashContainers, CIP_TRASHCONTAINERS )
ITEM_MULTI_SET( maxTrashContainers, CIP_MAXTRASHCONTAINERS )
ITEM_MULTI_SET( secureContainers, CIP_SECURECONTAINERS )
ITEM_MULTI_SET( maxSecureContainers, CIP_MAXSECURECONTAINERS )
ITEM_MULTI_SET( friends, CIP_FRIENDS )
ITEM_MULTI_SET( maxFriends, CIP_MAXFRIENDS )
ITEM_MULTI_SET( guests, CIP_GUESTS )
ITEM_MULTI_SET( maxGuests, CIP_MAXGUESTS )
ITEM_MULTI_SET( owners, CIP_OWNERS )
ITEM_MULTI_SET( maxOwners, CIP_MAXOWNERS )
ITEM_MULTI_SET( bans, CIP_BANS )
ITEM_MULTI_SET( maxBans, CIP_MAXBANS )
ITEM_MULTI_SET( vendors, CIP_VENDORS )
ITEM_MULTI_SET( maxVendors, CIP_MAXVENDORS )
ITEM_MULTI_SET( deed, CIP_DEED )
ITEM_MULTI_SET( isPublic, CIP_ISPUBLIC )
ITEM_MULTI_SET( buildTimestamp, CIP_BUILDTIMESTAMP )
ITEM_MULTI_SET( tradeTimestamp, CIP_TRADETIMESTAMP )
ITEM_MULTI_SET( banX, CIP_BANX )
ITEM_MULTI_SET( banY, CIP_BANY )
#undef ITEM_MULTI_SET

#define ITEM_MULTI_GET( attr, method, expression )                      \
FDCLG( CItem, attr )                                                    \
{                                                                       \
	FNARGS                                                                \
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );     \
	if( !ValidateObject( item )) return false;                            \
	if( item->GetObjType() != OT_MULTI ) { args.rval().setNull(); return true; } \
	args.rval().method( static_cast<CMultiObj *>( item )->expression );   \
	return true;                                                          \
}
ITEM_MULTI_GET( lockdowns, setInt32, GetLockdownCount() )
ITEM_MULTI_GET( maxLockdowns, setInt32, GetMaxLockdowns() )
ITEM_MULTI_GET( trashContainers, setInt32, GetTrashContainerCount() )
ITEM_MULTI_GET( maxTrashContainers, setInt32, GetMaxTrashContainers() )
ITEM_MULTI_GET( secureContainers, setInt32, GetSecureContainerCount() )
ITEM_MULTI_GET( maxSecureContainers, setInt32, GetMaxSecureContainers() )
ITEM_MULTI_GET( friends, setInt32, GetFriendCount() )
ITEM_MULTI_GET( maxFriends, setInt32, GetMaxFriends() )
ITEM_MULTI_GET( guests, setInt32, GetGuestCount() )
ITEM_MULTI_GET( maxGuests, setInt32, GetMaxGuests() )
ITEM_MULTI_GET( owners, setInt32, GetOwnerCount() )
ITEM_MULTI_GET( maxOwners, setInt32, GetMaxOwners() )
ITEM_MULTI_GET( bans, setInt32, GetBanCount() )
ITEM_MULTI_GET( maxBans, setInt32, GetMaxBans() )
ITEM_MULTI_GET( vendors, setInt32, GetVendorCount() )
ITEM_MULTI_GET( maxVendors, setInt32, GetMaxVendors() )
ITEM_MULTI_GET( isPublic, setBoolean, GetPublicStatus() )
ITEM_MULTI_GET( banX, setInt32, GetBanX() )
ITEM_MULTI_GET( banY, setInt32, GetBanY() )
#undef ITEM_MULTI_GET

#define ITEM_MULTI_STRING_GET( attr, expression )                       \
FDCLG( CItem, attr )                                                     \
{                                                                        \
	FNARGS                                                                 \
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );      \
	if( !ValidateObject( item )) return false;                             \
	if( item->GetObjType() != OT_MULTI ) { args.rval().setNull(); return true; } \
	JS::RootedString value( cx, JS_NewStringCopyZ( cx, static_cast<CMultiObj *>( item )->expression.c_str() )); \
	if( value == nullptr ) return false;                                   \
	args.rval().setString( value );                                        \
	return true;                                                           \
}
ITEM_MULTI_STRING_GET( deed, GetDeed() )
ITEM_MULTI_STRING_GET( buildTimestamp, GetBuildTimestamp() )
ITEM_MULTI_STRING_GET( tradeTimestamp, GetTradeTimestamp() )
#undef ITEM_MULTI_STRING_GET

IMPL_GET_OBJ( CItem, movable,       CItem, setInt32,   GetMovable() )
IMPL_GET_OBJ( CItem, layer,         CItem, setInt32,   GetLayer() )
IMPL_GET_OBJ( CItem, decayable,     CItem, setBoolean, IsDecayable() )
IMPL_GETS_OBJ( CItem, name2,        CItem, setString,  GetName2().c_str() )
IMPL_GET_OBJ( CItem, maxhp,         CItem, setInt32,   GetMaxHP() )
IMPL_GET_OBJ( CItem, maxUses,       CItem, setInt32,   GetMaxUses() )
IMPL_GET_OBJ( CItem, usesLeft,      CItem, setInt32,   GetUsesLeft() )
IMPL_GET_OBJ( CItem, rank,          CItem, setInt32,   GetRank() )
IMPL_GET_OBJ( CItem, creator,       CItem, setNumber,  GetCreator() )
IMPL_GET_OBJ( CItem, poison,        CItem, setInt32,   GetPoisoned() )
IMPL_GET_OBJ( CItem, poisonedBy,    CItem, setNumber,  GetPoisonedBy() )
IMPL_GET_OBJ( CItem, poisonCharges, CItem, setInt32,   GetPoisonCharges() )
IMPL_GET_OBJ( CItem, dir,           CItem, setInt32,   GetDir() )
IMPL_GET_OBJ( CItem, wipable,       CItem, setBoolean, IsWipeable() )
IMPL_GET_OBJ( CItem, weight,        CItem, setInt32,   GetWeight() )
IMPL_GET_OBJ( CItem, weightMax,     CItem, setInt32,   GetWeightMax() )
IMPL_GET_OBJ( CItem, baseWeight,    CItem, setInt32,   GetBaseWeight() )
IMPL_GET_OBJ( CItem, maxItems,      CItem, setInt32,   GetMaxItems() )
IMPL_GET_OBJ( CItem, corpse,        CItem, setBoolean, IsCorpse() )
IMPL_GETS_OBJ( CItem, desc,         CItem, setString,  GetDesc().c_str() )

#define IMPL_ITEM_STATE_SET( attr, propertyId )                         \
FDCLS( CItem, attr )                                                    \
{                                                                       \
	FNARGS                                                                \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 ));\
}

IMPL_ITEM_STATE_SET( movable, CIP_MOVABLE )
IMPL_ITEM_STATE_SET( layer, CIP_LAYER )
IMPL_ITEM_STATE_SET( decayable, CIP_DECAYABLE )
IMPL_ITEM_STATE_SET( name2, CIP_NAME2 )
IMPL_ITEM_STATE_SET( maxhp, CIP_MAXHP )
IMPL_ITEM_STATE_SET( maxUses, CIP_MAXUSES )
IMPL_ITEM_STATE_SET( usesLeft, CIP_USESLEFT )
IMPL_ITEM_STATE_SET( rank, CIP_RANK )
IMPL_ITEM_STATE_SET( creator, CIP_CREATOR )
IMPL_ITEM_STATE_SET( poison, CIP_POISON )
IMPL_ITEM_STATE_SET( poisonedBy, CIP_POISONEDBY )
IMPL_ITEM_STATE_SET( poisonCharges, CIP_POISONCHARGES )
IMPL_ITEM_STATE_SET( dir, CIP_DIR )
IMPL_ITEM_STATE_SET( wipable, CIP_WIPABLE )
IMPL_ITEM_STATE_SET( weight, CIP_WEIGHT )
IMPL_ITEM_STATE_SET( weightMax, CIP_WEIGHTMAX )
IMPL_ITEM_STATE_SET( baseWeight, CIP_BASEWEIGHT )
IMPL_ITEM_STATE_SET( maxItems, CIP_MAXITEMS )
IMPL_ITEM_STATE_SET( corpse, CIP_CORPSE )
IMPL_ITEM_STATE_SET( desc, CIP_DESC )

#undef IMPL_ITEM_STATE_SET

#define ITEM_NUM_GET( attr, accessor ) IMPL_GET_OBJ( CItem, attr, CItem, setInt32, accessor )
#define ITEM_BOOL_GET( attr, accessor ) IMPL_GET_OBJ( CItem, attr, CItem, setBoolean, accessor )
IMPL_GET_OBJ( CItem, more,  CItem, setNumber, GetTempVar( CITV_MORE ))
IMPL_GET_OBJ( CItem, more0, CItem, setNumber, GetTempVar( CITV_MORE0 ))
IMPL_GET_OBJ( CItem, more1, CItem, setNumber, GetTempVar( CITV_MORE1 ))
IMPL_GET_OBJ( CItem, more2, CItem, setNumber, GetTempVar( CITV_MORE2 ))
IMPL_GET_OBJ( CItem, morex, CItem, setNumber, GetTempVar( CITV_MOREX ))
IMPL_GET_OBJ( CItem, morey, CItem, setNumber, GetTempVar( CITV_MOREY ))
IMPL_GET_OBJ( CItem, morez, CItem, setNumber, GetTempVar( CITV_MOREZ ))
ITEM_NUM_GET( lodamage, GetLoDamage() )
ITEM_NUM_GET( hidamage, GetHiDamage() )
ITEM_NUM_GET( ac, GetArmourClass() )
ITEM_NUM_GET( def, GetResist( PHYSICAL ))
ITEM_NUM_GET( resistCold, GetResist( COLD ))
ITEM_NUM_GET( resistHeat, GetResist( HEAT ))
ITEM_NUM_GET( resistLight, GetResist( LIGHT ))
ITEM_NUM_GET( resistLightning, GetResist( LIGHTNING ))
ITEM_NUM_GET( resistPoison, GetResist( POISON ))
ITEM_NUM_GET( resistRain, GetResist( RAIN ))
ITEM_NUM_GET( resistSnow, GetResist( SNOW ))
ITEM_BOOL_GET( damageCold, GetWeatherDamage( COLD ))
ITEM_BOOL_GET( damageHeat, GetWeatherDamage( HEAT ))
ITEM_BOOL_GET( damageLight, GetWeatherDamage( LIGHT ))
ITEM_BOOL_GET( damageLightning, GetWeatherDamage( LIGHTNING ))
ITEM_BOOL_GET( damagePoison, GetWeatherDamage( POISON ))
ITEM_BOOL_GET( damageRain, GetWeatherDamage( RAIN ))
ITEM_BOOL_GET( damageSnow, GetWeatherDamage( SNOW ))
ITEM_NUM_GET( speed, GetSpeed() )
ITEM_NUM_GET( swingSpeedIncrease, GetSwingSpeedIncrease() )
ITEM_NUM_GET( damageIncrease, GetDamageIncrease() )
ITEM_NUM_GET( healthLeech, GetHealthLeech() )
ITEM_NUM_GET( staminaLeech, GetStaminaLeech() )
ITEM_NUM_GET( manaLeech, GetManaLeech() )
ITEM_NUM_GET( hitChance, GetHitChance() )
ITEM_NUM_GET( defenseChance, GetDefenseChance() )
ITEM_NUM_GET( luck, GetLuck() )
ITEM_NUM_GET( healthBonus, GetHealthBonus() )
ITEM_NUM_GET( staminaBonus, GetStaminaBonus() )
ITEM_NUM_GET( manaBonus, GetManaBonus() )
ITEM_NUM_GET( artifactRarity, GetArtifactRarity() )
ITEM_NUM_GET( durabilityHpBonus, GetDurabilityHpBonus() )
ITEM_NUM_GET( lowerStateReq, GetLowerStatReq() )
#undef ITEM_NUM_GET
#undef ITEM_BOOL_GET

#define ITEM_NUM_SET( attr, propertyId )                                  \
FDCLS( CItem, attr )                                                      \
{                                                                         \
	FNARGS                                                                  \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 ));  \
}
ITEM_NUM_SET( more, CIP_MORE )
ITEM_NUM_SET( more0, CIP_MORE0 )
ITEM_NUM_SET( more1, CIP_MORE1 )
ITEM_NUM_SET( more2, CIP_MORE2 )
ITEM_NUM_SET( morex, CIP_MOREX )
ITEM_NUM_SET( morey, CIP_MOREY )
ITEM_NUM_SET( morez, CIP_MOREZ )
ITEM_NUM_SET( lodamage, CIP_LODAMAGE )
ITEM_NUM_SET( hidamage, CIP_HIDAMAGE )
ITEM_NUM_SET( ac, CIP_AC )
ITEM_NUM_SET( def, CIP_DEF )
ITEM_NUM_SET( resistCold, CIP_RESISTCOLD )
ITEM_NUM_SET( resistHeat, CIP_RESISTHEAT )
ITEM_NUM_SET( resistLight, CIP_RESISTLIGHT )
ITEM_NUM_SET( resistLightning, CIP_RESISTLIGHTNING )
ITEM_NUM_SET( resistPoison, CIP_RESISTPOISON )
ITEM_NUM_SET( resistRain, CIP_RESISTRAIN )
ITEM_NUM_SET( resistSnow, CIP_RESISTSNOW )
ITEM_NUM_SET( damageCold, CIP_DAMAGECOLD )
ITEM_NUM_SET( damageHeat, CIP_DAMAGEHEAT )
ITEM_NUM_SET( damageLight, CIP_DAMAGELIGHT )
ITEM_NUM_SET( damageLightning, CIP_DAMAGELIGHTNING )
ITEM_NUM_SET( damagePoison, CIP_DAMAGEPOISON )
ITEM_NUM_SET( damageRain, CIP_DAMAGERAIN )
ITEM_NUM_SET( damageSnow, CIP_DAMAGESNOW )
ITEM_NUM_SET( speed, CIP_SPEED )
ITEM_NUM_SET( swingSpeedIncrease, CIP_SWINGSPEEDINCREASE )
ITEM_NUM_SET( damageIncrease, CIP_DAMAGEINCREASE )
ITEM_NUM_SET( healthLeech, CIP_HEALTHLEECH )
ITEM_NUM_SET( staminaLeech, CIP_STAMINALEECH )
ITEM_NUM_SET( manaLeech, CIP_MANALEECH )
ITEM_NUM_SET( hitChance, CIP_HITCHANCE )
ITEM_NUM_SET( defenseChance, CIP_DEFENSECHANCE )
ITEM_NUM_SET( luck, CIP_LUCK )
ITEM_NUM_SET( healthBonus, CIP_HEALTHBONUS )
ITEM_NUM_SET( staminaBonus, CIP_STAMINABONUS )
ITEM_NUM_SET( manaBonus, CIP_MANABONUS )
ITEM_NUM_SET( artifactRarity, CIP_ARTIFACTRARITY )
ITEM_NUM_SET( durabilityHpBonus, CIP_DURABILITYHPBONUS )
ITEM_NUM_SET( lowerStateReq, CIP_LOWERSTATREQ )
#undef ITEM_NUM_SET

#define ITEM_FLAG_GET( attr, method, accessor ) IMPL_GET_OBJ( CItem, attr, CItem, method, accessor )
ITEM_FLAG_GET( shouldSave, setBoolean, ShouldSave() )
ITEM_FLAG_GET( isNewbie, setBoolean, IsNewbie() )
ITEM_FLAG_GET( isDispellable, setBoolean, IsDispellable() )
ITEM_FLAG_GET( madeWith, setInt32, GetMadeWith() )
ITEM_FLAG_GET( entryMadeFrom, setInt32, EntryMadeFrom() )
ITEM_FLAG_GET( isPileable, setBoolean, IsPileable() )
ITEM_FLAG_GET( isMarkedByMaker, setBoolean, IsMarkedByMaker() )
ITEM_FLAG_GET( isDyeable, setBoolean, IsDyeable() )
ITEM_FLAG_GET( isDamageable, setBoolean, IsDamageable() )
ITEM_FLAG_GET( isWipeable, setBoolean, IsWipeable() )
ITEM_FLAG_GET( isGuarded, setBoolean, IsGuarded() )
ITEM_FLAG_GET( isDoorOpen, setBoolean, IsDoorOpen() )
#undef ITEM_FLAG_GET

#define ITEM_BOOL_SET( attr, accessor )                                  \
FDCLS( CItem, attr )                                                     \
{                                                                        \
	FNARGS                                                                 \
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );      \
	if( !ValidateObject( item )) return false;                             \
	item->accessor( args.get( 0 ).toBoolean() );                           \
	return true;                                                          \
}
ITEM_BOOL_SET( shouldSave, ShouldSave )
ITEM_BOOL_SET( isNewbie, SetNewbie )
ITEM_BOOL_SET( isDispellable, SetDispellable )
ITEM_BOOL_SET( isPileable, SetPileable )
ITEM_BOOL_SET( isMarkedByMaker, SetMakersMark )
ITEM_BOOL_SET( isDyeable, SetDye )
ITEM_BOOL_SET( isDamageable, SetDamageable )
ITEM_BOOL_SET( isWipeable, SetWipeable )
ITEM_BOOL_SET( isGuarded, SetGuarded )
ITEM_BOOL_SET( isDoorOpen, SetDoorOpen )
#undef ITEM_BOOL_SET

#define ITEM_META_SET( attr, typeName, accessor )                         \
FDCLS( CItem, attr )                                                      \
{                                                                         \
	FNARGS                                                                  \
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );       \
	if( !ValidateObject( item )) return false;                              \
	item->accessor( static_cast<typeName>( args.get( 0 ).toInt32() ));      \
	return true;                                                            \
}
ITEM_META_SET( madeWith, SI08, SetMadeWith )
ITEM_META_SET( entryMadeFrom, UI16, EntryMadeFrom )
#undef ITEM_META_SET

IMPL_GET_OBJ( CItem, tempLastTraded, CItem, setNumber, GetTempLastTraded() )
IMPL_GET_OBJ( CItem, tempTimer, CItem, setNumber, GetTempTimer() )
#define ITEM_FLAG_GET( attr, method, accessor ) IMPL_GET_OBJ( CItem, attr, CItem, method, accessor )
FDCLG( CItem, isChar ) { FNARGS args.rval().setBoolean( false ); return true; }
FDCLG( CItem, isItem ) { FNARGS args.rval().setBoolean( true ); return true; }
ITEM_FLAG_GET( isSpawner, setBoolean, GetObjType() == OT_SPAWNER )
ITEM_FLAG_GET( isFieldSpell, setBoolean, IsFieldSpell() )
ITEM_FLAG_GET( isLockedDown, setBoolean, IsLockedDown() )
ITEM_FLAG_GET( isShieldType, setBoolean, IsShieldType() )
ITEM_FLAG_GET( isMetalType, setBoolean, IsMetalType() )
ITEM_FLAG_GET( isLeatherType, setBoolean, IsLeatherType() )
ITEM_FLAG_GET( canBeLockedDown, setBoolean, CanBeLockedDown() )
ITEM_FLAG_GET( isContType, setBoolean, IsContType() )
ITEM_FLAG_GET( carveSection, setInt32, GetCarve() )
ITEM_FLAG_GET( ammoID, setInt32, GetAmmoId() )
ITEM_FLAG_GET( ammoHue, setInt32, GetAmmoHue() )
ITEM_FLAG_GET( ammoFX, setInt32, GetAmmoFX() )
ITEM_FLAG_GET( ammoFXHue, setInt32, GetAmmoFXHue() )
ITEM_FLAG_GET( ammoFXRender, setInt32, GetAmmoFXRender() )
ITEM_FLAG_GET( isItemHeld, setBoolean, IsHeldOnCursor() )
#undef ITEM_FLAG_GET

#define ITEM_RETAINED_SET( attr, propertyId )                            \
FDCLS( CItem, attr )                                                     \
{                                                                        \
	FNARGS                                                                 \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 )); \
}
ITEM_RETAINED_SET( tempLastTraded, CIP_TEMPLASTTRADED )
ITEM_RETAINED_SET( tempTimer, CIP_TEMPTIMER )
ITEM_RETAINED_SET( carveSection, CIP_CARVESECTION )
ITEM_RETAINED_SET( ammoID, CIP_AMMOID )
ITEM_RETAINED_SET( ammoHue, CIP_AMMOHUE )
ITEM_RETAINED_SET( ammoFX, CIP_AMMOFX )
ITEM_RETAINED_SET( ammoFXHue, CIP_AMMOFXHUE )
ITEM_RETAINED_SET( ammoFXRender, CIP_AMMOFXRENDER )
ITEM_RETAINED_SET( isItemHeld, CIP_ISITEMHELD )
#undef ITEM_RETAINED_SET

#define ITEM_GENERAL_GET( attr, method, accessor ) IMPL_GET_OBJ( CItem, attr, CItem, method, accessor )
FDCLG( CItem, att )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	if( !ValidateObject( item ))
		return false;
	args.rval().setInt32( RandomNum( item->GetLoDamage(), item->GetHiDamage() ));
	return true;
}
ITEM_GENERAL_GET( itemsinside, setInt32, GetContainsList()->Num() )
FDCLG( CItem, totalItemCount )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	if( !ValidateObject( item ))
		return false;
	args.rval().setInt32( GetTotalItemCount( item ));
	return true;
}
ITEM_GENERAL_GET( healthRegenBonus, setInt32, GetHealthRegenBonus() )
ITEM_GENERAL_GET( staminaRegenBonus, setInt32, GetStaminaRegenBonus() )
ITEM_GENERAL_GET( manaRegenBonus, setInt32, GetManaRegenBonus() )
ITEM_GENERAL_GET( buyvalue, setInt32, GetBuyValue() )
ITEM_GENERAL_GET( sellvalue, setInt32, GetSellValue() )
ITEM_GENERAL_GET( vendorPrice, setInt32, GetVendorPrice() )
ITEM_GENERAL_GET( restock, setInt32, GetRestock() )
ITEM_GENERAL_GET( good, setInt32, GetGood() )
ITEM_GENERAL_GET( divinelock, setBoolean, IsDivineLocked() )
ITEM_GENERAL_GET( strength, setInt32, GetStrength() )
ITEM_GENERAL_GET( dexterity, setInt32, GetDexterity() )
ITEM_GENERAL_GET( intelligence, setInt32, GetIntelligence() )
IMPL_GETS_OBJ( CItem, event, CItem, setString, GetEvent().c_str() )
ITEM_GENERAL_GET( maxRange, setInt32, GetMaxRange() )
ITEM_GENERAL_GET( baseRange, setInt32, GetBaseRange() )
ITEM_GENERAL_GET( origin, setInt32, GetOrigin() )
ITEM_GENERAL_GET( stealable, setInt32, GetStealable() )
FDCLG( CItem, moveType )
{
	FNARGS
	auto item = JS::GetMaybePtrFromReservedSlot<CItem>( thisObj, 0 );
	if( !ValidateObject( item ))
		return false;
	if( item->GetObjType() == OT_BOAT )
		args.rval().setInt32( static_cast<CBoatObj *>( item )->GetMoveType() );
	else
		args.rval().setNull();
	return true;
}
ITEM_GENERAL_GET( tithing, setInt32, GetTithing() )
#undef ITEM_GENERAL_GET

#define ITEM_GENERAL_SET( attr, propertyId )                             \
FDCLS( CItem, attr )                                                     \
{                                                                        \
	FNARGS                                                                 \
	return SetLegacyItemProperty( cx, thisObj, propertyId, args.get( 0 )); \
}
ITEM_GENERAL_SET( att, CIP_ATT )
ITEM_GENERAL_SET( itemsinside, CIP_ITEMSINSIDE )
ITEM_GENERAL_SET( totalItemCount, CIP_TOTALITEMCOUNT )
ITEM_GENERAL_SET( healthRegenBonus, CIP_HEALTHREGENBONUS )
ITEM_GENERAL_SET( staminaRegenBonus, CIP_STAMINAREGENBONUS )
ITEM_GENERAL_SET( manaRegenBonus, CIP_MANAREGENBONUS )
ITEM_GENERAL_SET( buyvalue, CIP_BUYVALUE )
ITEM_GENERAL_SET( sellvalue, CIP_SELLVALUE )
ITEM_GENERAL_SET( vendorPrice, CIP_VENDORPRICE )
ITEM_GENERAL_SET( restock, CIP_RESTOCK )
ITEM_GENERAL_SET( good, CIP_GOOD )
ITEM_GENERAL_SET( divinelock, CIP_DIVINELOCK )
ITEM_GENERAL_SET( strength, CIP_STRENGTH )
ITEM_GENERAL_SET( dexterity, CIP_DEXTERITY )
ITEM_GENERAL_SET( intelligence, CIP_INTELLIGENCE )
ITEM_GENERAL_SET( event, CIP_EVENT )
ITEM_GENERAL_SET( maxRange, CIP_MAXRANGE )
ITEM_GENERAL_SET( baseRange, CIP_BASERANGE )
ITEM_GENERAL_SET( origin, CIP_ORIGIN )
ITEM_GENERAL_SET( stealable, CIP_STEALABLE )
ITEM_GENERAL_SET( moveType, CIP_MOVETYPE )
ITEM_GENERAL_SET( tithing, CIP_TITHING )
#undef ITEM_GENERAL_SET

static bool SetLegacyCharacterProperty( JSContext *cx, JS::HandleObject characterObject, int32_t propertyId, JS::HandleValue value )
{
	JS::RootedValue rootedValue( cx, value );
	return CCharacterProps_setProperty( cx, characterObject, JS::PropertyKey::Int( propertyId ), false, rootedValue.address() );
}

#define CHARACTER_CORE_SET( attr, propertyId )                              \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_CORE_SET( sectionID, CCP_SECTIONID )
CHARACTER_CORE_SET( name, CCP_NAME )
CHARACTER_CORE_SET( origName, CCP_ORIGNAME )
CHARACTER_CORE_SET( title, CCP_TITLE )
CHARACTER_CORE_SET( x, CCP_X )
CHARACTER_CORE_SET( y, CCP_Y )
CHARACTER_CORE_SET( z, CCP_Z )
CHARACTER_CORE_SET( oldX, CCP_OLDX )
CHARACTER_CORE_SET( oldY, CCP_OLDY )
CHARACTER_CORE_SET( oldZ, CCP_OLDZ )
CHARACTER_CORE_SET( id, CCP_ID )
CHARACTER_CORE_SET( colour, CCP_COLOUR )
CHARACTER_CORE_SET( color, CCP_COLOUR )
CHARACTER_CORE_SET( skin, CCP_COLOUR )
CHARACTER_CORE_SET( hue, CCP_COLOUR )
CHARACTER_CORE_SET( controlSlots, CCP_CONTROLSLOTS )
CHARACTER_CORE_SET( controlSlotsUsed, CCP_CONTROLSLOTSUSED )
CHARACTER_CORE_SET( orneriness, CCP_ORNERINESS )
CHARACTER_CORE_SET( visible, CCP_VISIBLE )
CHARACTER_CORE_SET( serial, CCP_SERIAL )
CHARACTER_CORE_SET( health, CCP_HEALTH )
CHARACTER_CORE_SET( worldnumber, CCP_WORLDNUMBER )
CHARACTER_CORE_SET( instanceID, CCP_INSTANCEID )
#undef CHARACTER_CORE_SET

#define CHARACTER_STRING_GET( attr, expression )                         \
FDCLG( CCharacter, attr )                                                 \
{                                                                         \
	FNARGS                                                                  \
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );  \
	if( !ValidateObject( character )) return false;                         \
	const std::string text = expression;                                    \
	JS::RootedString value( cx, JS_NewStringCopyZ( cx, text.c_str() ));      \
	if( value == nullptr ) return false;                                    \
	args.rval().setString( value );                                         \
	return true;                                                            \
}
CHARACTER_STRING_GET( sectionID, character->GetSectionId() )
CHARACTER_STRING_GET( name, oldstrutil::stringToWstringToString( GetNpcDictName( character, nullptr, NRS_SCRIPT )))
CHARACTER_STRING_GET( origName, oldstrutil::stringToWstringToString( character->GetOrgName() ))
CHARACTER_STRING_GET( title, character->GetTitle() )
#undef CHARACTER_STRING_GET

#define CHARACTER_CORE_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_CORE_GET( x, setInt32, GetX() )
CHARACTER_CORE_GET( y, setInt32, GetY() )
CHARACTER_CORE_GET( z, setInt32, GetZ() )
CHARACTER_CORE_GET( id, setInt32, GetId() )
CHARACTER_CORE_GET( colour, setInt32, GetColour() )
CHARACTER_CORE_GET( color, setInt32, GetColour() )
CHARACTER_CORE_GET( skin, setInt32, GetColour() )
CHARACTER_CORE_GET( hue, setInt32, GetColour() )
CHARACTER_CORE_GET( controlSlots, setInt32, GetControlSlots() )
CHARACTER_CORE_GET( controlSlotsUsed, setInt32, GetControlSlotsUsed() )
CHARACTER_CORE_GET( orneriness, setInt32, GetOrneriness() )
CHARACTER_CORE_GET( visible, setInt32, GetVisible() )
CHARACTER_CORE_GET( serial, setNumber, GetSerial() )
CHARACTER_CORE_GET( health, setInt32, GetHP() )
CHARACTER_CORE_GET( worldnumber, setInt32, WorldNumber() )
CHARACTER_CORE_GET( instanceID, setInt32, GetInstanceId() )
#undef CHARACTER_CORE_GET

FDCLG( CCharacter, oldX ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setInt32( character->GetOldLocation().x ); return true; }
FDCLG( CCharacter, oldY ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setInt32( character->GetOldLocation().y ); return true; }
FDCLG( CCharacter, oldZ ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setInt32( character->GetOldLocation().z ); return true; }

#define CHARACTER_STATS_SET( attr, propertyId )                             \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_STATS_SET( owner, CCP_OWNER )
CHARACTER_STATS_SET( scripttrigger, CCP_SCRIPTTRIGGER )
CHARACTER_STATS_SET( scriptTriggers, CCP_SCRIPTTRIGGERS )
CHARACTER_STATS_SET( target, CCP_TARGET )
CHARACTER_STATS_SET( dexterity, CCP_DEXTERITY )
CHARACTER_STATS_SET( intelligence, CCP_INTELLIGENCE )
CHARACTER_STATS_SET( strength, CCP_STRENGTH )
CHARACTER_STATS_SET( actualDexterity, CCP_ACTUALDEXTERITY )
CHARACTER_STATS_SET( actualIntelligence, CCP_ACTUALINTELLIGENCE )
CHARACTER_STATS_SET( actualStrength, CCP_ACTUALSTRENGTH )
CHARACTER_STATS_SET( healthRegenBonus, CCP_HEALTHREGENBONUS )
CHARACTER_STATS_SET( staminaRegenBonus, CCP_STAMINAREGENBONUS )
CHARACTER_STATS_SET( manaRegenBonus, CCP_MANAREGENBONUS )
CHARACTER_STATS_SET( mana, CCP_MANA )
CHARACTER_STATS_SET( stamina, CCP_STAMINA )
CHARACTER_STATS_SET( pack, CCP_CHARPACK )
#undef CHARACTER_STATS_SET

#define CHARACTER_STATS_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_STATS_GET( dexterity, setInt32, GetDexterity() )
CHARACTER_STATS_GET( intelligence, setInt32, GetIntelligence() )
CHARACTER_STATS_GET( strength, setInt32, GetStrength() )
CHARACTER_STATS_GET( actualDexterity, setInt32, ActualDexterity() )
CHARACTER_STATS_GET( actualIntelligence, setInt32, ActualIntelligence() )
CHARACTER_STATS_GET( actualStrength, setInt32, ActualStrength() )
CHARACTER_STATS_GET( healthRegenBonus, setInt32, GetHealthRegenBonus() )
CHARACTER_STATS_GET( staminaRegenBonus, setInt32, GetStaminaRegenBonus() )
CHARACTER_STATS_GET( manaRegenBonus, setInt32, GetManaRegenBonus() )
CHARACTER_STATS_GET( mana, setInt32, GetMana() )
CHARACTER_STATS_GET( stamina, setInt32, GetStamina() )
#undef CHARACTER_STATS_GET

#define CHARACTER_OBJECT_GET( attr, objectType, expression )              \
FDCLG( CCharacter, attr )                                                  \
{                                                                          \
	FNARGS                                                                   \
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );   \
	auto object = expression;                                                \
	if( !ValidateObject( object )) { args.rval().setNull(); return true; }    \
	JSObject *wrapper = JSEngine->AcquireObject( objectType, object, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ))); \
	if( wrapper == nullptr ) return false;                                   \
	args.rval().setObject( *wrapper );                                       \
	return true;                                                             \
}
CHARACTER_OBJECT_GET( owner, IUE_CHAR, character->GetOwnerObj() )
CHARACTER_OBJECT_GET( target, IUE_CHAR, character->GetTarg() )
CHARACTER_OBJECT_GET( pack, IUE_ITEM, character->GetPackItem() )
#undef CHARACTER_OBJECT_GET

FDCLG( CCharacter, scripttrigger )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	const auto &triggers = character->GetScriptTriggers();
	args.rval().setInt32( triggers.empty() ? 0 : triggers.back() );
	return true;
}

FDCLG( CCharacter, scriptTriggers )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	const auto &triggers = character->GetScriptTriggers();
	JS::RootedObject result( cx, JS::NewArrayObject( cx, triggers.size() ));
	if( result == nullptr ) return false;
	for( uint32_t index = 0; index < triggers.size(); ++index )
	{
		JS::RootedValue value( cx, JS::Int32Value( triggers[index] ));
		if( !JS_SetElement( cx, result, index, value )) return false;
	}
	args.rval().setObject( *result );
	return true;
}

#define CHARACTER_STATE_SET( attr, propertyId )                             \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_STATE_SET( fame, CCP_FAME )
CHARACTER_STATE_SET( karma, CCP_KARMA )
CHARACTER_STATE_SET( canAttack, CCP_CANATTACK )
CHARACTER_STATE_SET( karmaLock, CCP_KARMALOCK )
CHARACTER_STATE_SET( fleeAt, CCP_FLEEAT )
CHARACTER_STATE_SET( reAttackAt, CCP_REATTACKAT )
CHARACTER_STATE_SET( brkPeaceChance, CCP_BRKPEACE )
CHARACTER_STATE_SET( hunger, CCP_HUNGER )
CHARACTER_STATE_SET( thirst, CCP_THIRST )
CHARACTER_STATE_SET( frozen, CCP_FROZEN )
CHARACTER_STATE_SET( commandlevel, CCP_COMMANDLEVEL )
CHARACTER_STATE_SET( hasStolen, CCP_HASSTOLEN )
CHARACTER_STATE_SET( criminal, CCP_CRIMINAL )
CHARACTER_STATE_SET( innocent, CCP_INNOCENT )
CHARACTER_STATE_SET( murdercount, CCP_MURDERCOUNT )
CHARACTER_STATE_SET( neutral, CCP_NEUTRAL )
CHARACTER_STATE_SET( npcFlag, CCP_NPCFLAG )
#undef CHARACTER_STATE_SET

#define CHARACTER_STATE_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_STATE_GET( fame, setInt32, GetFame() )
CHARACTER_STATE_GET( karma, setInt32, GetKarma() )
CHARACTER_STATE_GET( canAttack, setBoolean, GetCanAttack() )
CHARACTER_STATE_GET( karmaLock, setBoolean, GetKarmaLock() )
CHARACTER_STATE_GET( fleeAt, setInt32, GetFleeAt() )
CHARACTER_STATE_GET( reAttackAt, setInt32, GetReattackAt() )
CHARACTER_STATE_GET( brkPeaceChance, setInt32, GetBrkPeaceChance() )
CHARACTER_STATE_GET( hunger, setInt32, GetHunger() )
CHARACTER_STATE_GET( thirst, setInt32, GetThirst() )
CHARACTER_STATE_GET( frozen, setBoolean, IsFrozen() )
CHARACTER_STATE_GET( commandlevel, setInt32, GetCommandLevel() )
CHARACTER_STATE_GET( hasStolen, setBoolean, HasStolen() )
CHARACTER_STATE_GET( criminal, setBoolean, IsCriminal() )
CHARACTER_STATE_GET( murderer, setBoolean, IsMurderer() )
CHARACTER_STATE_GET( innocent, setBoolean, IsInnocent() )
CHARACTER_STATE_GET( murdercount, setInt32, GetKills() )
CHARACTER_STATE_GET( neutral, setBoolean, IsNeutral() )
CHARACTER_STATE_GET( npcFlag, setInt32, GetNPCFlag() )
#undef CHARACTER_STATE_GET

FDCLG( CCharacter, attack )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	if( !ValidateObject( character )) return false;
	args.rval().setInt32( Combat->CalcAttackPower( character, true ));
	return true;
}

#define CHARACTER_LIFECYCLE_SET( attr, propertyId )                         \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_LIFECYCLE_SET( dead, CCP_DEAD )
CHARACTER_LIFECYCLE_SET( npc, CCP_NPC )
CHARACTER_LIFECYCLE_SET( isAwake, CCP_AWAKE )
CHARACTER_LIFECYCLE_SET( online, CCP_ONLINE )
CHARACTER_LIFECYCLE_SET( direction, CCP_DIRECTION )
CHARACTER_LIFECYCLE_SET( isRunning, CCP_ISRUNNING )
CHARACTER_LIFECYCLE_SET( isChar, CCP_ISCHAR )
CHARACTER_LIFECYCLE_SET( isItem, CCP_ISITEM )
CHARACTER_LIFECYCLE_SET( isSpawner, CCP_ISSPAWNER )
CHARACTER_LIFECYCLE_SET( isonhorse, CCP_ISONHORSE )
CHARACTER_LIFECYCLE_SET( isFlying, CCP_ISFLYING )
CHARACTER_LIFECYCLE_SET( isGuarded, CCP_ISGUARDED )
CHARACTER_LIFECYCLE_SET( tempdex, CCP_TDEXTERITY )
CHARACTER_LIFECYCLE_SET( tempint, CCP_TINTELLIGENCE )
CHARACTER_LIFECYCLE_SET( tempstr, CCP_TSTRENGTH )
CHARACTER_LIFECYCLE_SET( poison, CCP_POISON )
CHARACTER_LIFECYCLE_SET( poisonedBy, CCP_POISONEDBY )
CHARACTER_LIFECYCLE_SET( lightlevel, CCP_LIGHTLEVEL )
CHARACTER_LIFECYCLE_SET( vulnerable, CCP_VULNERABLE )
CHARACTER_LIFECYCLE_SET( willhunger, CCP_HUNGERSTATUS )
CHARACTER_LIFECYCLE_SET( willthirst, CCP_THIRSTSTATUS )
#undef CHARACTER_LIFECYCLE_SET

#define CHARACTER_LIFECYCLE_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_LIFECYCLE_GET( dead, setBoolean, IsDead() )
CHARACTER_LIFECYCLE_GET( npc, setBoolean, IsNpc() )
CHARACTER_LIFECYCLE_GET( isAwake, setBoolean, IsAwake() )
CHARACTER_LIFECYCLE_GET( direction, setInt32, GetDir() )
CHARACTER_LIFECYCLE_GET( isRunning, setBoolean, GetRunning() )
CHARACTER_LIFECYCLE_GET( isonhorse, setBoolean, IsOnHorse() )
CHARACTER_LIFECYCLE_GET( isFlying, setBoolean, IsFlying() )
CHARACTER_LIFECYCLE_GET( isGuarded, setBoolean, IsGuarded() )
CHARACTER_LIFECYCLE_GET( tempdex, setInt32, GetDexterity2() )
CHARACTER_LIFECYCLE_GET( tempint, setInt32, GetIntelligence2() )
CHARACTER_LIFECYCLE_GET( tempstr, setInt32, GetStrength2() )
CHARACTER_LIFECYCLE_GET( poison, setInt32, GetPoisoned() )
CHARACTER_LIFECYCLE_GET( poisonedBy, setNumber, GetPoisonedBy() )
CHARACTER_LIFECYCLE_GET( lightlevel, setInt32, GetFixedLight() )
CHARACTER_LIFECYCLE_GET( willhunger, setBoolean, WillHunger() )
CHARACTER_LIFECYCLE_GET( willthirst, setBoolean, WillThirst() )
#undef CHARACTER_LIFECYCLE_GET

FDCLG( CCharacter, online ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setBoolean( IsOnline( *character )); return true; }
FDCLG( CCharacter, isChar ) { FNARGS args.rval().setBoolean( true ); return true; }
FDCLG( CCharacter, isItem ) { FNARGS args.rval().setBoolean( false ); return true; }
FDCLG( CCharacter, isSpawner ) { FNARGS args.rval().setBoolean( false ); return true; }
FDCLG( CCharacter, vulnerable ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setBoolean( !character->IsInvulnerable() ); return true; }

#define CHARACTER_NPC_SET( attr, propertyId )                               \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_NPC_SET( npcGuild, CCP_NPCGUILD )
CHARACTER_NPC_SET( spawnSerial, CCP_SPAWNSERIAL )
CHARACTER_NPC_SET( maxhp, CCP_MAXHP )
CHARACTER_NPC_SET( maxstamina, CCP_MAXSTAMINA )
CHARACTER_NPC_SET( maxmana, CCP_MAXMANA )
CHARACTER_NPC_SET( oldWandertype, CCP_OLDWANDERTYPE )
CHARACTER_NPC_SET( wandertype, CCP_WANDERTYPE )
CHARACTER_NPC_SET( fx1, CCP_FX1 )
CHARACTER_NPC_SET( fy1, CCP_FY1 )
CHARACTER_NPC_SET( fx2, CCP_FX2 )
CHARACTER_NPC_SET( fy2, CCP_FY2 )
CHARACTER_NPC_SET( fz, CCP_FZ )
CHARACTER_NPC_SET( spawnX, CCP_SPAWNX )
CHARACTER_NPC_SET( spawnY, CCP_SPAWNY )
CHARACTER_NPC_SET( spawnZ, CCP_SPAWNZ )
CHARACTER_NPC_SET( pathTargX, CCP_PATHTARGX )
CHARACTER_NPC_SET( pathTargY, CCP_PATHTARGY )
CHARACTER_NPC_SET( nextAct, CCP_NEXTACT )
#undef CHARACTER_NPC_SET

#define CHARACTER_NPC_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_NPC_GET( npcGuild, setInt32, GetNPCGuild() )
CHARACTER_NPC_GET( spawnSerial, setNumber, GetSpawn() )
CHARACTER_NPC_GET( maxhp, setInt32, GetMaxHP() )
CHARACTER_NPC_GET( maxstamina, setInt32, GetMaxStam() )
CHARACTER_NPC_GET( maxmana, setInt32, GetMaxMana() )
CHARACTER_NPC_GET( oldWandertype, setInt32, GetOldNpcWander() )
CHARACTER_NPC_GET( wandertype, setInt32, GetNpcWander() )
CHARACTER_NPC_GET( fx1, setInt32, GetFx( 0 ) )
CHARACTER_NPC_GET( fy1, setInt32, GetFy( 0 ) )
CHARACTER_NPC_GET( fx2, setInt32, GetFx( 1 ) )
CHARACTER_NPC_GET( fy2, setInt32, GetFy( 1 ) )
CHARACTER_NPC_GET( fz, setInt32, GetFz() )
CHARACTER_NPC_GET( spawnX, setInt32, GetSpawnX() )
CHARACTER_NPC_GET( spawnY, setInt32, GetSpawnY() )
CHARACTER_NPC_GET( spawnZ, setInt32, GetSpawnZ() )
CHARACTER_NPC_GET( pathTargX, setInt32, GetPathTargX() )
CHARACTER_NPC_GET( pathTargY, setInt32, GetPathTargY() )
CHARACTER_NPC_GET( nextAct, setInt32, GetNextAct() )
#undef CHARACTER_NPC_GET

#define CHARACTER_COMBAT_SET( attr, propertyId )                            \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_COMBAT_SET( lodamage, CCP_LODAMAGE )
CHARACTER_COMBAT_SET( hidamage, CCP_HIDAMAGE )
CHARACTER_COMBAT_SET( flag, CCP_FLAG )
CHARACTER_COMBAT_SET( atWar, CCP_ATWAR )
CHARACTER_COMBAT_SET( spellCast, CCP_SPELLCAST )
CHARACTER_COMBAT_SET( isCasting, CCP_ISCASTING )
CHARACTER_COMBAT_SET( priv, CCP_PRIV )
CHARACTER_COMBAT_SET( townPriv, CCP_TOWNPRIV )
CHARACTER_COMBAT_SET( guildTitle, CCP_GUILDTITLE )
CHARACTER_COMBAT_SET( hairStyle, CCP_HAIRSTYLE )
CHARACTER_COMBAT_SET( hairColour, CCP_HAIRCOLOUR )
CHARACTER_COMBAT_SET( hairColor, CCP_HAIRCOLOUR )
CHARACTER_COMBAT_SET( beardStyle, CCP_BEARDSTYLE )
CHARACTER_COMBAT_SET( beardColour, CCP_BEARDCOLOUR )
CHARACTER_COMBAT_SET( beardColor, CCP_BEARDCOLOUR )
CHARACTER_COMBAT_SET( fontType, CCP_FONTTYPE )
CHARACTER_COMBAT_SET( sayColour, CCP_SAYCOLOUR )
CHARACTER_COMBAT_SET( emoteColour, CCP_EMOTECOLOUR )
CHARACTER_COMBAT_SET( attacker, CCP_ATTACKER )
CHARACTER_COMBAT_SET( raceGate, CCP_RACEGATE )
CHARACTER_COMBAT_SET( deaths, CCP_DEATHS )
CHARACTER_COMBAT_SET( cell, CCP_CELL )
CHARACTER_COMBAT_SET( allmove, CCP_ALLMOVE )
CHARACTER_COMBAT_SET( houseicons, CCP_HOUSEICONS )
CHARACTER_COMBAT_SET( spattack, CCP_SPATTACK )
CHARACTER_COMBAT_SET( spdelay, CCP_SPDELAY )
CHARACTER_COMBAT_SET( swingSpeedIncrease, CCP_SWINGSPEEDINCREASE )
CHARACTER_COMBAT_SET( luck, CCP_LUCK )
CHARACTER_COMBAT_SET( damageIncrease, CCP_DAMAGEINCREASE )
CHARACTER_COMBAT_SET( hitChance, CCP_HITCHANCE )
CHARACTER_COMBAT_SET( defenseChance, CCP_DEFENSECHANCE )
#undef CHARACTER_COMBAT_SET

#define CHARACTER_COMBAT_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_COMBAT_GET( lodamage, setInt32, GetLoDamage() )
CHARACTER_COMBAT_GET( hidamage, setInt32, GetHiDamage() )
CHARACTER_COMBAT_GET( flag, setInt32, GetFlag() )
CHARACTER_COMBAT_GET( atWar, setBoolean, IsAtWar() )
CHARACTER_COMBAT_GET( spellCast, setInt32, GetSpellCast() )
CHARACTER_COMBAT_GET( priv, setNumber, GetPriv() )
CHARACTER_COMBAT_GET( townPriv, setInt32, GetTownPriv() )
CHARACTER_COMBAT_GET( hairStyle, setInt32, GetHairStyle() )
CHARACTER_COMBAT_GET( hairColour, setInt32, GetHairColour() )
CHARACTER_COMBAT_GET( hairColor, setInt32, GetHairColour() )
CHARACTER_COMBAT_GET( beardStyle, setInt32, GetBeardStyle() )
CHARACTER_COMBAT_GET( beardColour, setInt32, GetBeardColour() )
CHARACTER_COMBAT_GET( beardColor, setInt32, GetBeardColour() )
CHARACTER_COMBAT_GET( fontType, setInt32, GetFontType() )
CHARACTER_COMBAT_GET( sayColour, setInt32, GetSayColour() )
CHARACTER_COMBAT_GET( emoteColour, setInt32, GetEmoteColour() )
CHARACTER_COMBAT_GET( raceGate, setInt32, GetRaceGate() )
CHARACTER_COMBAT_GET( deaths, setInt32, GetDeaths() )
CHARACTER_COMBAT_GET( cell, setInt32, GetCell() )
CHARACTER_COMBAT_GET( allmove, setBoolean, AllMove() )
CHARACTER_COMBAT_GET( houseicons, setBoolean, ViewHouseAsIcon() )
CHARACTER_COMBAT_GET( spattack, setInt32, GetSpAttack() )
CHARACTER_COMBAT_GET( spdelay, setInt32, GetSpDelay() )
CHARACTER_COMBAT_GET( swingSpeedIncrease, setInt32, GetSwingSpeedIncrease() )
CHARACTER_COMBAT_GET( luck, setInt32, GetLuck() )
CHARACTER_COMBAT_GET( damageIncrease, setInt32, GetDamageIncrease() )
CHARACTER_COMBAT_GET( hitChance, setInt32, GetHitChance() )
CHARACTER_COMBAT_GET( defenseChance, setInt32, GetDefenseChance() )
#undef CHARACTER_COMBAT_GET

FDCLG( CCharacter, isCasting ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setBoolean( character->IsCasting() || character->IsJSCasting() ); return true; }
FDCLG( CCharacter, guildTitle )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	JS::RootedString value( cx, JS_NewStringCopyZ( cx, character->GetGuildTitle().c_str() ));
	if( value == nullptr ) return false;
	args.rval().setString( value );
	return true;
}
FDCLG( CCharacter, attacker )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	CChar *attacker = character->GetAttacker();
	if( !ValidateObject( attacker )) { args.rval().setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( IUE_CHAR, attacker, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

#define CHARACTER_AI_SET( attr, propertyId )                                \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_AI_SET( aitype, CCP_AITYPE )
CHARACTER_AI_SET( split, CCP_SPLIT )
CHARACTER_AI_SET( splitchance, CCP_SPLITCHANCE )
CHARACTER_AI_SET( hireling, CCP_HIRELING )
CHARACTER_AI_SET( trainer, CCP_TRAINER )
CHARACTER_AI_SET( weight, CCP_WEIGHT )
CHARACTER_AI_SET( squelch, CCP_SQUELCH )
CHARACTER_AI_SET( isJailed, CCP_ISJAILED )
CHARACTER_AI_SET( magicReflect, CCP_MAGICREFLECT )
CHARACTER_AI_SET( permanentMagicReflect, CCP_PERMMAGICREFLECT )
CHARACTER_AI_SET( hideFameKarmaTitle, CCP_HIDEFAMEKARMATITLE )
CHARACTER_AI_SET( noCharCollide, CCP_NOCHARCOLLIDE )
CHARACTER_AI_SET( tamed, CCP_TAMED )
CHARACTER_AI_SET( tamedHungerRate, CCP_TAMEDHUNGERRATE )
CHARACTER_AI_SET( tamedThirstRate, CCP_TAMEDTHIRSTRATE )
CHARACTER_AI_SET( hungerWildChance, CCP_HUNGERWILDCHANCE )
CHARACTER_AI_SET( thirstWildChance, CCP_THIRSTWILDCHANCE )
CHARACTER_AI_SET( foodList, CCP_FOODLIST )
CHARACTER_AI_SET( mounted, CCP_MOUNTED )
CHARACTER_AI_SET( stabled, CCP_STABLED )
CHARACTER_AI_SET( isUsingPotion, CCP_USINGPOTION )
CHARACTER_AI_SET( stealth, CCP_STEALTH )
CHARACTER_AI_SET( skillToTame, CCP_SKILLTOTAME )
CHARACTER_AI_SET( skillToProv, CCP_SKILLTOPROV )
CHARACTER_AI_SET( skillToPeace, CCP_SKILLTOPEACE )
CHARACTER_AI_SET( poisonStrength, CCP_POISONSTRENGTH )
CHARACTER_AI_SET( isPolymorphed, CCP_ISPOLYMORPHED )
CHARACTER_AI_SET( isIncognito, CCP_ISINCOGNITO )
CHARACTER_AI_SET( isDisguised, CCP_ISDISGUISED )
CHARACTER_AI_SET( canRun, CCP_CANRUN )
CHARACTER_AI_SET( isMeditating, CCP_ISMEDITATING )
#undef CHARACTER_AI_SET

#define CHARACTER_AI_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_AI_GET( aitype, setInt32, GetNpcAiType() )
CHARACTER_AI_GET( split, setInt32, GetSplit() )
CHARACTER_AI_GET( splitchance, setInt32, GetSplitChance() )
CHARACTER_AI_GET( hireling, setBoolean, CanBeHired() )
CHARACTER_AI_GET( trainer, setBoolean, CanTrain() )
CHARACTER_AI_GET( weight, setInt32, GetWeight() )
CHARACTER_AI_GET( squelch, setInt32, GetSquelched() )
CHARACTER_AI_GET( isJailed, setBoolean, IsJailed() )
CHARACTER_AI_GET( magicReflect, setBoolean, IsTempReflected() )
CHARACTER_AI_GET( permanentMagicReflect, setBoolean, IsPermReflected() )
CHARACTER_AI_GET( hideFameKarmaTitle, setBoolean, HideFameKarmaTitle() )
CHARACTER_AI_GET( noCharCollide, setBoolean, NoCharCollide() )
CHARACTER_AI_GET( tamed, setBoolean, IsTamed() )
CHARACTER_AI_GET( tamedHungerRate, setInt32, GetTamedHungerRate() )
CHARACTER_AI_GET( tamedThirstRate, setInt32, GetTamedThirstRate() )
CHARACTER_AI_GET( hungerWildChance, setInt32, GetTamedHungerWildChance() )
CHARACTER_AI_GET( thirstWildChance, setInt32, GetTamedThirstWildChance() )
CHARACTER_AI_GET( mounted, setBoolean, GetMounted() )
CHARACTER_AI_GET( stabled, setBoolean, GetStabled() )
CHARACTER_AI_GET( isUsingPotion, setBoolean, IsUsingPotion() )
CHARACTER_AI_GET( stealth, setInt32, GetStealth() )
CHARACTER_AI_GET( skillToTame, setInt32, GetTaming() )
CHARACTER_AI_GET( skillToProv, setInt32, GetProvoing() )
CHARACTER_AI_GET( skillToPeace, setInt32, GetPeaceing() )
CHARACTER_AI_GET( poisonStrength, setInt32, GetPoisonStrength() )
CHARACTER_AI_GET( isPolymorphed, setBoolean, IsPolymorphed() )
CHARACTER_AI_GET( isIncognito, setBoolean, IsIncognito() )
CHARACTER_AI_GET( isDisguised, setBoolean, IsDisguised() )
CHARACTER_AI_GET( canRun, setBoolean, CanRun() )
CHARACTER_AI_GET( isMeditating, setBoolean, IsMeditating() )
#undef CHARACTER_AI_GET

FDCLG( CCharacter, foodList )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	JS::RootedString value( cx, JS_NewStringCopyZ( cx, character->GetFood().c_str() ));
	if( value == nullptr ) return false;
	args.rval().setString( value );
	return true;
}

#define CHARACTER_MISC_SET( attr, propertyId )                              \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_MISC_SET( setPeace, CCP_SETPEACE )
CHARACTER_MISC_SET( gender, CCP_GENDER )
CHARACTER_MISC_SET( guildNumber, CCP_GUILDNUMBER )
CHARACTER_MISC_SET( ownerCount, CCP_OWNERCOUNT )
CHARACTER_MISC_SET( isGM, CCP_ISGM )
CHARACTER_MISC_SET( canBroadcast, CCP_CANBROADCAST )
CHARACTER_MISC_SET( singClickSer, CCP_SINGCLICKSER )
CHARACTER_MISC_SET( noSkillTitles, CCP_NOSKILLTITLES )
CHARACTER_MISC_SET( isGMPageable, CCP_ISGMPAGEABLE )
CHARACTER_MISC_SET( canSnoop, CCP_CANSNOOP )
CHARACTER_MISC_SET( isCounselor, CCP_ISCOUNSELOR )
CHARACTER_MISC_SET( noNeedMana, CCP_NONEEDMANA )
CHARACTER_MISC_SET( isDispellable, CCP_ISDISPELLABLE )
CHARACTER_MISC_SET( noNeedReags, CCP_NONEEDREAGS )
CHARACTER_MISC_SET( orgID, CCP_ORGID )
CHARACTER_MISC_SET( orgSkin, CCP_ORGSKIN )
CHARACTER_MISC_SET( isShop, CCP_ISSHOP )
CHARACTER_MISC_SET( maxLoyalty, CCP_MAXLOYALTY )
CHARACTER_MISC_SET( loyalty, CCP_LOYALTY )
CHARACTER_MISC_SET( loyaltyRate, CCP_LOYALTYRATE )
CHARACTER_MISC_SET( shouldSave, CCP_SHOULDSAVE )
CHARACTER_MISC_SET( origin, CCP_ORIGIN )
CHARACTER_MISC_SET( accountNum, CCP_ACCOUNTNUM )
CHARACTER_MISC_SET( createdOn, CCP_CREATEDON )
CHARACTER_MISC_SET( playTime, CCP_PLAYTIME )
CHARACTER_MISC_SET( housesOwned, CCP_HOUSESOWNED )
CHARACTER_MISC_SET( housesCoOwned, CCP_HOUSESCOOWNED )
CHARACTER_MISC_SET( tithing, CCP_TITHING )
#undef CHARACTER_MISC_SET

#define CHARACTER_MISC_GET( attr, method, expression ) IMPL_GET_OBJ( CCharacter, attr, CChar, method, expression )
CHARACTER_MISC_GET( guildNumber, setInt32, GetGuildNumber() )
CHARACTER_MISC_GET( ownerCount, setInt32, GetOwnerCount() )
CHARACTER_MISC_GET( isGM, setBoolean, IsGM() )
CHARACTER_MISC_GET( canBroadcast, setBoolean, CanBroadcast() )
CHARACTER_MISC_GET( singClickSer, setBoolean, GetSingClickSer() )
CHARACTER_MISC_GET( noSkillTitles, setBoolean, NoSkillTitles() )
CHARACTER_MISC_GET( isGMPageable, setBoolean, IsGMPageable() )
CHARACTER_MISC_GET( canSnoop, setBoolean, CanSnoop() )
CHARACTER_MISC_GET( isCounselor, setBoolean, IsCounselor() )
CHARACTER_MISC_GET( noNeedMana, setBoolean, NoNeedMana() )
CHARACTER_MISC_GET( isDispellable, setBoolean, IsDispellable() )
CHARACTER_MISC_GET( noNeedReags, setBoolean, NoNeedReags() )
CHARACTER_MISC_GET( orgID, setInt32, GetOrgId() )
CHARACTER_MISC_GET( orgSkin, setInt32, GetOrgSkin() )
CHARACTER_MISC_GET( isShop, setBoolean, IsShop() )
CHARACTER_MISC_GET( maxLoyalty, setInt32, GetMaxLoyalty() )
CHARACTER_MISC_GET( loyalty, setInt32, GetLoyalty() )
CHARACTER_MISC_GET( shouldSave, setBoolean, ShouldSave() )
CHARACTER_MISC_GET( accountNum, setInt32, GetAccountNum() )
CHARACTER_MISC_GET( createdOn, setInt32, GetCreatedOn() )
CHARACTER_MISC_GET( playTime, setInt32, GetPlayTime() )
CHARACTER_MISC_GET( housesOwned, setInt32, CountHousesOwned( false ) )
CHARACTER_MISC_GET( housesCoOwned, setInt32, CountHousesOwned( true ) )
CHARACTER_MISC_GET( tithing, setInt32, GetTithing() )
CHARACTER_MISC_GET( lastOnSecs, setNumber, GetLastOnSecs() )
#undef CHARACTER_MISC_GET

FDCLG( CCharacter, setPeace ) { FNARGS args.rval().setUndefined(); return true; }
FDCLG( CCharacter, petCount ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setInt32( character->GetPetList()->Num() ); return true; }
FDCLG( CCharacter, followerCount ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setInt32( character->GetFollowerList()->Num() ); return true; }
FDCLG( CCharacter, ownedItemsCount ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setInt32( character->GetOwnedItems()->size() ); return true; }
FDCLG( CCharacter, isAnimal ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setBoolean( cwmWorldState->creatures[character->GetId()].IsAnimal() ); return true; }
FDCLG( CCharacter, isPackAnimal ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setBoolean( cwmWorldState->creatures[character->GetId()].IsPackAnimal() ); return true; }
FDCLG( CCharacter, isHuman ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); args.rval().setBoolean( cwmWorldState->creatures[character->GetId()].IsHuman() ); return true; }
FDCLG( CCharacter, hungerRate ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); CRace *race = Races->Race( character->GetRace() ); UI16 rate = race == nullptr ? 0 : race->GetHungerRate(); if( rate == 0 ) rate = cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE ); args.rval().setInt32( rate ); return true; }
FDCLG( CCharacter, thirstRate ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); CRace *race = Races->Race( character->GetRace() ); UI16 rate = race == nullptr ? 0 : race->GetThirstRate(); if( rate == 0 ) rate = cwmWorldState->ServerData()->SystemTimer( tSERVER_THIRSTRATE ); args.rval().setInt32( rate ); return true; }
FDCLG( CCharacter, loyaltyRate ) { FNARGS args.rval().setInt32( cwmWorldState->ServerData()->SystemTimer( tSERVER_LOYALTYRATE )); return true; }
FDCLG( CCharacter, gender ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); int32_t gender = 0; switch( character->GetId() ) { case 0x0191: case 0x0193: gender = 1; break; case 0x025D: case 0x025F: gender = 2; break; case 0x025E: case 0x0260: gender = 3; break; case 0x029A: case 0x02B6: gender = 4; break; case 0x029B: case 0x02B7: gender = 5; break; default: break; } args.rval().setInt32( gender ); return true; }
FDCLG( CCharacter, origin ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); const auto text = cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( character->GetOrigin() )); JS::RootedString value( cx, JS_NewStringCopyZ( cx, text.c_str() )); if( value == nullptr ) return false; args.rval().setString( value ); return true; }
FDCLG( CCharacter, lastOn ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); JS::RootedString value( cx, JS_NewStringCopyZ( cx, character->GetLastOn().c_str() )); if( value == nullptr ) return false; args.rval().setString( value ); return true; }

#define CHARACTER_FINAL_SET( attr, propertyId )                              \
FDCLS( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	return SetLegacyCharacterProperty( cx, thisObj, propertyId, args.get( 0 ));\
}
CHARACTER_FINAL_SET( skills, CCP_SKILLS )
CHARACTER_FINAL_SET( race, CCP_RACE )
CHARACTER_FINAL_SET( region, CCP_REGION )
CHARACTER_FINAL_SET( town, CCP_TOWN )
CHARACTER_FINAL_SET( guild, CCP_GUILD )
CHARACTER_FINAL_SET( baseskills, CCP_BASESKILLS )
CHARACTER_FINAL_SET( skillsused, CCP_SKILLUSE )
CHARACTER_FINAL_SET( socket, CCP_SOCKET )
CHARACTER_FINAL_SET( guarding, CCP_GUARDING )
CHARACTER_FINAL_SET( skillLock, CCP_SKILLLOCK )
CHARACTER_FINAL_SET( skillCaps, CCP_SKILLCAP )
CHARACTER_FINAL_SET( partyLootable, CCP_PARTYLOOTABLE )
CHARACTER_FINAL_SET( party, CCP_PARTY )
CHARACTER_FINAL_SET( multi, CCP_MULTI )
CHARACTER_FINAL_SET( account, CCP_ACCOUNT )
#undef CHARACTER_FINAL_SET

static bool CreateCharacterSkillProxy( JSContext *cx, CChar *character, const JSClass *proxyClass, JS::MutableHandleValue result )
{
	JS::RootedObject proxy( cx, JS_NewObject( cx, proxyClass ));
	if( proxy == nullptr || !JS_DefineProperties( cx, proxy, CSkillsProps )) return false;
	JS::SetReservedSlot( proxy, 0, JS::PrivateValue( character ));
	result.setObject( *proxy );
	return true;
}

#define CHARACTER_SKILL_PROXY_GET( attr, proxyClass )                       \
FDCLG( CCharacter, attr )                                                    \
{                                                                            \
	FNARGS                                                                     \
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );     \
	return CreateCharacterSkillProxy( cx, character, proxyClass, args.rval() );\
}
CHARACTER_SKILL_PROXY_GET( skills, &UOXSkills_class )
CHARACTER_SKILL_PROXY_GET( baseskills, &UOXBaseSkills_class )
CHARACTER_SKILL_PROXY_GET( skillsused, &UOXSkillsUsed_class )
CHARACTER_SKILL_PROXY_GET( skillLock, &UOXSkillsLock_class )
CHARACTER_SKILL_PROXY_GET( skillCaps, &UOXSkillsCap_class )
#undef CHARACTER_SKILL_PROXY_GET

static bool SetCharacterObjectResult( JSContext *cx, JS::MutableHandleValue result, IUEEntries objectType, void *object )
{
	if( object == nullptr ) { result.setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( objectType, object, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	result.setObject( *wrapper );
	return true;
}

FDCLG( CCharacter, race ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); return SetCharacterObjectResult( cx, args.rval(), IUE_RACE, Races->Race( character->GetRace() )); }
FDCLG( CCharacter, region ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); return SetCharacterObjectResult( cx, args.rval(), IUE_REGION, character->GetRegion() ); }
FDCLG( CCharacter, town ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); const UI16 town = character->GetTown(); return SetCharacterObjectResult( cx, args.rval(), IUE_REGION, town == 0xFF ? nullptr : cwmWorldState->townRegions[town] ); }
FDCLG( CCharacter, guild ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); const GUILDID guild = character->GetGuildNumber(); return SetCharacterObjectResult( cx, args.rval(), IUE_GUILD, guild == -1 ? nullptr : GuildSys->Guild( guild )); }
FDCLG( CCharacter, socket ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); return SetCharacterObjectResult( cx, args.rval(), IUE_SOCK, character->GetSocket() ); }
FDCLG( CCharacter, party ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); return SetCharacterObjectResult( cx, args.rval(), IUE_PARTY, PartyFactory::GetSingleton().Get( character )); }
FDCLG( CCharacter, multi ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); CMultiObj *multi = character->GetMultiObj(); return SetCharacterObjectResult( cx, args.rval(), IUE_ITEM, ValidateObject( multi ) ? multi : nullptr ); }
FDCLG( CCharacter, account ) { FNARGS auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 ); return SetCharacterObjectResult( cx, args.rval(), IUE_ACCOUNT, &character->GetAccount() ); }
FDCLG( CCharacter, guarding )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	CBaseObject *object = character->GetGuarding();
	if( !ValidateObject( object )) { args.rval().setNull(); return true; }
	return SetCharacterObjectResult( cx, args.rval(), object->CanBeObjType( OT_CHAR ) ? IUE_CHAR : IUE_ITEM, object );
}
FDCLG( CCharacter, partyLootable )
{
	FNARGS
	auto character = JS::GetMaybePtrFromReservedSlot<CChar>( thisObj, 0 );
	Party *party = PartyFactory::GetSingleton().Get( character );
	CPartyEntry *entry = party == nullptr ? nullptr : party->Find( character );
	args.rval().setBoolean( entry != nullptr && entry->IsLootable() );
	return true;
}

JSBool CCharacterProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CChar *gPriv = static_cast<CChar *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_CHAR );
	if( propID != 0xFFFF )
	{
		CItem *TempItem			= nullptr;
		JSObject *TempObject	= nullptr;
		JSString *tString = nullptr;
		switch( propID )
		{
			case CCP_ACCOUNTNUM:	*vp = INT_TO_JSVAL( gPriv->GetAccountNum() );	break;
			case CCP_ACCOUNT:
			{
				CAccountBlock_st *accountBlock = &gPriv->GetAccount();
				if( accountBlock == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{	// Otherwise Acquire an object
					JSObject *accountObj = JSEngine->AcquireObject( IUE_ACCOUNT, accountBlock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( accountObj );
				}
				break;
			}
			case CCP_CREATEDON: *vp = INT_TO_JSVAL( gPriv->GetCreatedOn() );		break;
			case CCP_PLAYTIME: *vp = INT_TO_JSVAL( gPriv->GetPlayTime() );		break;
			case CCP_NAME:
				{
					CSocket *tSock = nullptr;

					std::string mCharName = GetNpcDictName( gPriv, tSock, NRS_SCRIPT );
					std::string convertedString = oldstrutil::stringToWstringToString( mCharName );

					tString = JS_NewStringCopyZ( cx, convertedString.c_str() );
					*vp = STRING_TO_JSVAL( tString );
					break;
				}
			case CCP_ORIGNAME:
			{
				[[maybe_unused]] CSocket *tSock = nullptr;

				std::string mCharName = gPriv->GetOrgName();
				std::string convertedString = oldstrutil::stringToWstringToString( mCharName );

				tString = JS_NewStringCopyZ( cx, convertedString.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			}
			case CCP_TITLE:
				tString = JS_NewStringCopyZ( cx, gPriv->GetTitle().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_SECTIONID:
				tString = JS_NewStringCopyZ( cx, gPriv->GetSectionId().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_X:			*vp = INT_TO_JSVAL( gPriv->GetX() );			break;
			case CCP_Y:			*vp = INT_TO_JSVAL( gPriv->GetY() );			break;
			case CCP_Z:			*vp = INT_TO_JSVAL( gPriv->GetZ() );			break;
			case CCP_OLDX:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.x );
				break;
			}
			case CCP_OLDY:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.y );
				break;
			}
			case CCP_OLDZ:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.z );
				break;
			}
			case CCP_ID:		*vp = INT_TO_JSVAL( gPriv->GetId() );				break;
			case CCP_COLOUR:	*vp = INT_TO_JSVAL( gPriv->GetColour() );			break;
			case CCP_CONTROLSLOTS:	*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetControlSlots() ));			break;
			case CCP_CONTROLSLOTSUSED:	*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetControlSlotsUsed() ));	break;
			case CCP_ORNERINESS:	*vp = INT_TO_JSVAL( gPriv->GetOrneriness() );	break;
			case CCP_OWNER:
				CBaseObject *TempObj;
				TempObj = gPriv->GetOwnerObj();

				if( !ValidateObject( TempObj ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CCP_VISIBLE:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetVisible() ));	break;
			case CCP_SERIAL:
			{
				JS_NewNumberValue( cx, gPriv->GetSerial(), vp );
				break;
			}
			case CCP_HEALTH:		*vp = INT_TO_JSVAL( gPriv->GetHP() );				break;
			case CCP_SCRIPTTRIGGER:
			{
				// For backwards compatibility, get last scripttrigger from vector
				// For older worldfiles, this will be the only scripttrigger added to the vector after load
				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				UI16 lastScriptTrigger = 0;
				auto numberOfTriggers = scriptTriggers.size();
				if( numberOfTriggers > 0 )
				{
					lastScriptTrigger = scriptTriggers[numberOfTriggers-1];
				}
				*vp = INT_TO_JSVAL( lastScriptTrigger );
				break;
			}
			case CCP_SCRIPTTRIGGERS:
			{
				jsval scriptId;
				JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
				{
					scriptId = INT_TO_JSVAL( scriptTriggers[i] );
					JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
				}

				*vp = OBJECT_TO_JSVAL( scriptTriggersJS );
				break;
			}
			case CCP_WORLDNUMBER:	*vp = INT_TO_JSVAL( gPriv->WorldNumber() );			break;
			case CCP_INSTANCEID:	*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );		break;
			case CCP_TARGET:
				CChar *tempChar;
				tempChar = gPriv->GetTarg();

				if( !ValidateObject( tempChar ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CCP_DEXTERITY:				*vp = INT_TO_JSVAL( gPriv->GetDexterity() );		break;
			case CCP_INTELLIGENCE:			*vp = INT_TO_JSVAL( gPriv->GetIntelligence() );		break;
			case CCP_STRENGTH:				*vp = INT_TO_JSVAL( gPriv->GetStrength() );			break;
			case CCP_ACTUALDEXTERITY:		*vp = INT_TO_JSVAL( gPriv->ActualDexterity() );		break;
			case CCP_ACTUALINTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->ActualIntelligence() );	break;
			case CCP_ACTUALSTRENGTH:		*vp = INT_TO_JSVAL( gPriv->ActualStrength() );		break;
			case CCP_BASESKILLS:
				TempObject = JS_NewObject( cx, &UOXBaseSkills_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_SKILLS:
				TempObject = JS_NewObject( cx, &UOXSkills_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_SKILLUSE:
				TempObject = JS_NewObject( cx, &UOXSkillsUsed_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_MANA:			*vp = INT_TO_JSVAL( gPriv->GetMana() );			break;
			case CCP_STAMINA:		*vp = INT_TO_JSVAL( gPriv->GetStamina() );		break;
			case CCP_CHARPACK:
				TempItem = gPriv->GetPackItem();

				if( !ValidateObject( TempItem ))
					*vp = JSVAL_NULL;
				else
				{
					// Otherwise Acquire an object
					JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, TempItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myItem );
				}
				break;
			case CCP_FAME:			*vp = INT_TO_JSVAL( gPriv->GetFame() );					break;
			case CCP_KARMA:			*vp = INT_TO_JSVAL( gPriv->GetKarma() );				break;
			case CCP_ATTACK:		*vp = INT_TO_JSVAL( Combat->CalcAttackPower( gPriv, true ));	break;
			case CCP_CANATTACK:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetCanAttack() );		break;
			case CCP_KARMALOCK:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetKarmaLock() );		break;
			case CCP_FLEEAT:		*vp = INT_TO_JSVAL( gPriv->GetFleeAt() );				break;
			case CCP_REATTACKAT:	*vp = INT_TO_JSVAL( gPriv->GetReattackAt() );			break;
			case CCP_BRKPEACE:		*vp = INT_TO_JSVAL( gPriv->GetBrkPeaceChance() );		break;
			case CCP_HUNGER:		*vp = INT_TO_JSVAL( gPriv->GetHunger() );				break;
			case CCP_HUNGERRATE:
			{
				CRace *TempRace	= nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				// Try to fetch hungerRate from character's race
				UI16 hungerRate = 0;
				if( TempRace != nullptr )
				{
					hungerRate = TempRace->GetHungerRate();
				}

				// If hungerRate from Race is zero, use the global hunger rate from UOX.INI instead
				if( hungerRate == 0 )
				{
					hungerRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE );
				}

				*vp = INT_TO_JSVAL( hungerRate );
				break;
			}
			case CCP_THIRST:		*vp = INT_TO_JSVAL( gPriv->GetThirst() );				break;
			case CCP_THIRSTRATE:
			{
				CRace *TempRace	= nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				// Try to fetch thirstRate from character's race
				UI16 thirstRate = 0;
				if( TempRace != nullptr )
				{
					thirstRate = TempRace->GetThirstRate();
				}

				// If thirstRate from Race is zero, use the global thirst rate from UOX.INI instead
				if( thirstRate == 0 )
				{
					thirstRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_THIRSTRATE );
				}

				*vp = INT_TO_JSVAL( thirstRate );
				break;
			}
			case CCP_FROZEN:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsFrozen() );			break;
			case CCP_COMMANDLEVEL:	*vp = INT_TO_JSVAL( gPriv->GetCommandLevel() );			break;
			case CCP_RACE:
			{
				CRace *TempRace	= nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				if( TempRace == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myRace	= JSEngine->AcquireObject( IUE_RACE, TempRace, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myRace );
				}
				break;
			}
			case CCP_HASSTOLEN:		*vp = BOOLEAN_TO_JSVAL( gPriv->HasStolen() );	break;
			case CCP_CRIMINAL:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCriminal() );	break;
			case CCP_MURDERER:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsMurderer() );	break;
			case CCP_INNOCENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsInnocent() );	break;
			case CCP_NEUTRAL:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsNeutral() );	break;
			case CCP_MURDERCOUNT:	*vp = INT_TO_JSVAL( gPriv->GetKills() );		break;
			case CCP_GENDER:
				switch( gPriv->GetId() )
				{
					case 0x0190:	// human male, dead or alive
					case 0x0192:	*vp = INT_TO_JSVAL( 0 );						break;
					case 0x0191:	// human female, dead or alive
					case 0x0193:	*vp = INT_TO_JSVAL( 1 );						break;
					case 0x025D:	// elf male, dead or alive
					case 0x025F:	*vp = INT_TO_JSVAL( 2 );						break;
					case 0x025E:	// elf female, dead or alive
					case 0x0260:	*vp = INT_TO_JSVAL( 3 );						break;
					case 0x029A:	// gargoyle male, dead or alive
					case 0x02B6:	*vp = INT_TO_JSVAL( 4 );						break;
					case 0x029B:	// gargoyle female, dead or alive
					case 0x02B7:	*vp = INT_TO_JSVAL( 5 );						break;
					default:		*vp = INT_TO_JSVAL( 0xFF );						break;
				}
				break;
			case CCP_DEAD:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDead() );		break;
			case CCP_NPC:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsNpc() );		break;
			case CCP_AWAKE:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsAwake() );		break;
			case CCP_ONLINE:		*vp = BOOLEAN_TO_JSVAL( IsOnline(( *gPriv )));	break;
			case CCP_DIRECTION:		*vp = INT_TO_JSVAL( gPriv->GetDir() );			break;
				// 3  objects: regions + towns + guilds
			case CCP_ISRUNNING:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetRunning() );	break;
			case CCP_REGION:
			{
				CTownRegion *myReg = gPriv->GetRegion();
				if( myReg == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, myReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myTown );
				}
				break;
			}
			case CCP_HEALTHREGENBONUS:		*vp = INT_TO_JSVAL( gPriv->GetHealthRegenBonus() );			break;
			case CCP_STAMINAREGENBONUS:		*vp = INT_TO_JSVAL( gPriv->GetStaminaRegenBonus() );			break;
			case CCP_MANAREGENBONUS:			*vp = INT_TO_JSVAL( gPriv->GetManaRegenBonus() );			break;
			case CCP_ORIGIN:
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_TOWN:
			{
				UI16 tempTownId	= 0xFF;
				tempTownId = gPriv->GetTown();

				// We need to decide here whether 0xFF is a valid town (wilderness) or not
				// i would say no its not
				if( tempTownId == 0xFF )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Should build the town here
					JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, cwmWorldState->townRegions[tempTownId], JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myTown );
				}
				break;
			}
			case CCP_GUILD:
			{
				GUILDID tempGuildId	= -1;
				tempGuildId = gPriv->GetGuildNumber();

				// Character has no guild
				if( tempGuildId == -1 ) // isn't there a constant or something like?
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myGuild = JSEngine->AcquireObject( IUE_GUILD, GuildSys->Guild( tempGuildId ), JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myGuild );
				}
				break;
			}
			case CCP_GUILDNUMBER: *vp = INT_TO_JSVAL( gPriv->GetGuildNumber() );		break;
			case CCP_SOCKET:
			{ // So we can declare the variables here
				CSocket *tSock = gPriv->GetSocket();
				if( tSock == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{	// Otherwise Acquire an object
					JSObject *mySock	= JSEngine->AcquireObject( IUE_SOCK, tSock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( mySock );
				}
				break;
			}
			case CCP_ISCHAR:		*vp = JSVAL_TRUE;									break;
			case CCP_ISITEM:		*vp = JSVAL_FALSE;									break;
			case CCP_ISSPAWNER:		*vp = JSVAL_FALSE;									break;
			case CCP_SPAWNSERIAL:
				JS_NewNumberValue( cx, gPriv->GetSpawn(), vp );
				break;
			case CCP_MAXHP:			*vp = INT_TO_JSVAL( gPriv->GetMaxHP() );			break;
			case CCP_MAXSTAMINA:	*vp = INT_TO_JSVAL( gPriv->GetMaxStam() );			break;
			case CCP_MAXMANA:		*vp = INT_TO_JSVAL( gPriv->GetMaxMana() );			break;
			case CCP_OLDWANDERTYPE:	*vp = INT_TO_JSVAL( gPriv->GetOldNpcWander() );		break;
			case CCP_WANDERTYPE:	*vp = INT_TO_JSVAL( gPriv->GetNpcWander() );		break;
			case CCP_FX1:			*vp = INT_TO_JSVAL( gPriv->GetFx( 0 ));				break;
			case CCP_FY1:			*vp = INT_TO_JSVAL( gPriv->GetFy( 0 ));				break;
			case CCP_FX2:			*vp = INT_TO_JSVAL( gPriv->GetFx( 1 ));				break;
			case CCP_FY2:			*vp = INT_TO_JSVAL( gPriv->GetFy( 1 ));				break;
			case CCP_FZ:			*vp = INT_TO_JSVAL( gPriv->GetFz() );				break;
			case CCP_SPAWNX:		*vp = INT_TO_JSVAL( gPriv->GetSpawnX() );			break;
			case CCP_SPAWNY:		*vp = INT_TO_JSVAL( gPriv->GetSpawnY() );			break;
			case CCP_SPAWNZ:		*vp = INT_TO_JSVAL( gPriv->GetSpawnZ() );			break;
			case CCP_PATHTARGX:		*vp = INT_TO_JSVAL( gPriv->GetPathTargX() );		break;
			case CCP_PATHTARGY:		*vp = INT_TO_JSVAL( gPriv->GetPathTargY() );		break;
			case CCP_ISONHORSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsOnHorse() );		break;
			case CCP_ISFLYING:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsFlying() );		break;
			case CCP_ISGUARDED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );		break;
			case CCP_GUARDING:
			{
				CBaseObject *tempObj = gPriv->GetGuarding();
				if( !ValidateObject( tempObj ))
				{	// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj = nullptr;
					if( tempObj->CanBeObjType( OT_CHAR ))
					{
						myObj = JSEngine->AcquireObject( IUE_CHAR, tempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					else if( tempObj->CanBeObjType( OT_ITEM ))
					{
						myObj = JSEngine->AcquireObject( IUE_ITEM, tempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			}
			case CCP_TDEXTERITY:	*vp = INT_TO_JSVAL( gPriv->GetDexterity2() );		break;
			case CCP_TINTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->GetIntelligence2() );	break;
			case CCP_TSTRENGTH:		*vp = INT_TO_JSVAL( gPriv->GetStrength2() );		break;
			case CCP_POISON:		*vp = INT_TO_JSVAL( gPriv->GetPoisoned() );			break;
			case CCP_POISONEDBY:	*vp = INT_TO_JSVAL( gPriv->GetPoisonedBy() );		break;
			case CCP_LIGHTLEVEL:	*vp = INT_TO_JSVAL( gPriv->GetFixedLight() );		break;
			case CCP_VULNERABLE:	*vp = BOOLEAN_TO_JSVAL( !gPriv->IsInvulnerable() );	break;
			case CCP_HUNGERSTATUS:	*vp = BOOLEAN_TO_JSVAL( gPriv->WillHunger() );		break;
			case CCP_THIRSTSTATUS:	*vp = BOOLEAN_TO_JSVAL( gPriv->WillThirst() );		break;
			case CCP_LODAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetLoDamage() );			break;
			case CCP_HIDAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetHiDamage() );			break;
			case CCP_FLAG:			*vp = INT_TO_JSVAL( gPriv->GetFlag() );				break;
			case CCP_ATWAR:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsAtWar() );			break;
			case CCP_SPELLCAST:		*vp = INT_TO_JSVAL( gPriv->GetSpellCast() );		break;
			case CCP_ISCASTING:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCasting() || gPriv->IsJSCasting() );		break;
			case CCP_PRIV:			JS_NewNumberValue( cx, gPriv->GetPriv(), vp ); 		break;
			case CCP_TOWNPRIV:		*vp = INT_TO_JSVAL( gPriv->GetTownPriv() );			break;
			case CCP_GUILDTITLE:
				tString = JS_NewStringCopyZ( cx, gPriv->GetGuildTitle().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_HAIRSTYLE:		*vp = INT_TO_JSVAL( gPriv->GetHairStyle() );		break;
			case CCP_HAIRCOLOUR:	*vp = INT_TO_JSVAL( gPriv->GetHairColour() );		break;
			case CCP_BEARDSTYLE:	*vp = INT_TO_JSVAL( gPriv->GetBeardStyle() );		break;
			case CCP_BEARDCOLOUR:	*vp = INT_TO_JSVAL( gPriv->GetBeardColour() );		break;
			case CCP_FONTTYPE:		*vp = INT_TO_JSVAL( gPriv->GetFontType() );			break;
			case CCP_SAYCOLOUR:		*vp = INT_TO_JSVAL( gPriv->GetSayColour() );		break;
			case CCP_EMOTECOLOUR:	*vp = INT_TO_JSVAL( gPriv->GetEmoteColour() );		break;
			case CCP_ATTACKER:
			{
				// Hm Quite funny, same thing as .owner
				CChar *tempChar	= gPriv->GetAttacker();
				if( !ValidateObject( tempChar ))
				{	// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			}
			case CCP_RACEGATE:		*vp = INT_TO_JSVAL( gPriv->GetRaceGate() );			break;
			case CCP_SKILLLOCK:
				TempObject = JS_NewObject( cx, &UOXSkillsLock_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp	= OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_SKILLCAP:
				TempObject = JS_NewObject( cx, &UOXSkillsCap_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_DEATHS:		*vp = INT_TO_JSVAL( gPriv->GetDeaths() );					break;
			case CCP_OWNERCOUNT:	*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetOwnerCount() ));		break;
			case CCP_NEXTACT:		*vp = INT_TO_JSVAL( gPriv->GetNextAct() );					break;
			case CCP_PETCOUNT:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetPetList()->Num() ));	break;
			case CCP_FOLLOWERCOUNT:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetFollowerList()->Num() ));	break;
			case CCP_OWNEDITEMSCOUNT:	*vp = INT_TO_JSVAL( gPriv->GetOwnedItems()->size() );	break;
			case CCP_CELL:			*vp = INT_TO_JSVAL( gPriv->GetCell() );						break;
			case CCP_ALLMOVE:		*vp = BOOLEAN_TO_JSVAL( gPriv->AllMove() );					break;
			case CCP_HOUSEICONS:	*vp = BOOLEAN_TO_JSVAL( gPriv->ViewHouseAsIcon() );			break;
			case CCP_SPATTACK:		*vp = INT_TO_JSVAL( gPriv->GetSpAttack() );					break;
			case CCP_SPDELAY:		*vp = INT_TO_JSVAL( gPriv->GetSpDelay() );					break;
			case CCP_SWINGSPEEDINCREASE:	*vp = INT_TO_JSVAL( gPriv->GetSwingSpeedIncrease() );		break;
			case CCP_LUCK:			*vp = INT_TO_JSVAL( gPriv->GetLuck() );						break;
			case CCP_DAMAGEINCREASE:*vp = INT_TO_JSVAL( gPriv->GetDamageIncrease() );			break;
			case CCP_HITCHANCE:		*vp = INT_TO_JSVAL( gPriv->GetHitChance() );				break;
			case CCP_DEFENSECHANCE:	*vp = INT_TO_JSVAL( gPriv->GetDefenseChance() );			break;
			case CCP_AITYPE:		*vp = INT_TO_JSVAL( gPriv->GetNpcAiType() );				break;
			case CCP_SPLIT:			*vp = INT_TO_JSVAL( gPriv->GetSplit() );					break;
			case CCP_SPLITCHANCE:	*vp = INT_TO_JSVAL( gPriv->GetSplitChance() );				break;
			case CCP_VENDORGOLDHELD:*vp = INT_TO_JSVAL( gPriv->GetHoldG() );					break;
			case CCP_TRAINER:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanTrain() );				break;
			case CCP_HIRELING:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanBeHired() );				break;
			case CCP_WEIGHT:		*vp = INT_TO_JSVAL( gPriv->GetWeight() );					break;
			case CCP_SQUELCH:		*vp = INT_TO_JSVAL( gPriv->GetSquelched() );				break;
			case CCP_ISJAILED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsJailed() );				break;
			case CCP_MAGICREFLECT:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsTempReflected() );			break;
			case CCP_PERMMAGICREFLECT:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPermReflected() );		break;
			case CCP_HIDEFAMEKARMATITLE: *vp = BOOLEAN_TO_JSVAL( gPriv->HideFameKarmaTitle() );	break;
			case CCP_NOCHARCOLLIDE: *vp = BOOLEAN_TO_JSVAL( gPriv->NoCharCollide() );		    break;
			case CCP_TAMED:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsTamed() );					break;
			case CCP_TAMEDHUNGERRATE: *vp = INT_TO_JSVAL( gPriv->GetTamedHungerRate() );		break;
			case CCP_TAMEDTHIRSTRATE: *vp = INT_TO_JSVAL( gPriv->GetTamedThirstRate() );		break;
			case CCP_HUNGERWILDCHANCE: *vp = INT_TO_JSVAL( gPriv->GetTamedHungerWildChance() );	break;
			case CCP_THIRSTWILDCHANCE: *vp = INT_TO_JSVAL( gPriv->GetTamedThirstWildChance() );	break;
			case CCP_FOODLIST:
				tString = JS_NewStringCopyZ( cx, gPriv->GetFood().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_MOUNTED:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetMounted() );				break;
			case CCP_STABLED:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetStabled() );				break;
			case CCP_USINGPOTION:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsUsingPotion() );			break;
			case CCP_STEALTH:		*vp = INT_TO_JSVAL( gPriv->GetStealth() );					break;
			case CCP_SKILLTOTAME:	*vp = INT_TO_JSVAL( gPriv->GetTaming() );					break;
			case CCP_SKILLTOPROV:	*vp = INT_TO_JSVAL( gPriv->GetProvoing() );					break;
			case CCP_SKILLTOPEACE:	*vp = INT_TO_JSVAL( gPriv->GetPeaceing() );					break;
			case CCP_POISONSTRENGTH:	*vp = INT_TO_JSVAL( gPriv->GetPoisonStrength() );		break;
			case CCP_ISPOLYMORPHED:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPolymorphed() );			break;
			case CCP_ISINCOGNITO:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsIncognito() );				break;
			case CCP_ISDISGUISED:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDisguised() );				break;
			case CCP_CANRUN:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanRun() );					break;
			case CCP_ISMEDITATING:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsMeditating() );			break;
			case CCP_ISGM:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsGM() );					break;
			case CCP_TITHING:		*vp = INT_TO_JSVAL( gPriv->GetTithing() );					break;
			case CCP_CANBROADCAST:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanBroadcast() );			break;
			case CCP_SINGCLICKSER:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetSingClickSer() );			break;
			case CCP_NOSKILLTITLES:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoSkillTitles() );			break;
			case CCP_ISGMPAGEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsGMPageable() );			break;
			case CCP_CANSNOOP:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanSnoop() );				break;
			case CCP_ISCOUNSELOR:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsCounselor() );				break;
			case CCP_NONEEDMANA:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoNeedMana() );				break;
			case CCP_ISDISPELLABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDispellable() );			break;
			case CCP_NONEEDREAGS:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoNeedReags() );				break;
			case CCP_ISANIMAL:		*vp	= BOOLEAN_TO_JSVAL( cwmWorldState->creatures[gPriv->GetId()].IsAnimal() ); break;
			case CCP_ISPACKANIMAL:	*vp	= BOOLEAN_TO_JSVAL( cwmWorldState->creatures[gPriv->GetId()].IsPackAnimal() ); break;
			case CCP_ISHUMAN:		*vp	= BOOLEAN_TO_JSVAL( cwmWorldState->creatures[gPriv->GetId()].IsHuman() ); break;
			case CCP_ORGID:			*vp = INT_TO_JSVAL( gPriv->GetOrgId() );					break;
			case CCP_ORGSKIN:		*vp = INT_TO_JSVAL( gPriv->GetOrgSkin() );					break;
			case CCP_NPCFLAG:		*vp = INT_TO_JSVAL( static_cast<SI32>(gPriv->GetNPCFlag() ));break;
			case CCP_NPCGUILD:		*vp = INT_TO_JSVAL( gPriv->GetNPCGuild() );					break;
			case CCP_ISSHOP:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsShop() );					break;
			case CCP_MAXLOYALTY:	*vp = INT_TO_JSVAL( gPriv->GetMaxLoyalty() );				break;
			case CCP_LOYALTY:		*vp = INT_TO_JSVAL( gPriv->GetLoyalty() );					break;
			case CCP_LOYALTYRATE:
			{
				// Use global loyalty rate from UOX.INI
				UI16 loyaltyRate = 0;
				if( loyaltyRate == 0 )
				{
					loyaltyRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_LOYALTYRATE );
				}

				*vp = INT_TO_JSVAL( loyaltyRate );
				break;
			}
			case CCP_SHOULDSAVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->ShouldSave() );			break;
			case CCP_PARTYLOOTABLE:
			{
				Party *toGet = PartyFactory::GetSingleton().Get( gPriv );
				if( toGet == nullptr )
				{
					*vp = BOOLEAN_TO_JSVAL( false );
				}
				else
				{
					CPartyEntry *toScan = toGet->Find( gPriv );
					if( toScan == nullptr )
					{
						*vp = BOOLEAN_TO_JSVAL( false );
					}
					else
					{
						*vp = BOOLEAN_TO_JSVAL( toScan->IsLootable() );
					}
				}
				break;
			}
			case CCP_PARTY:
			{
				// Hm Quite funny, same thing as .owner
				Party *tempParty = PartyFactory::GetSingleton().Get( gPriv );
				if( tempParty == nullptr )
				{	// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myParty = JSEngine->AcquireObject( IUE_PARTY, tempParty, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myParty );
				}
				break;
			}
			case CCP_MULTI:
				CMultiObj *multi;
				multi = gPriv->GetMultiObj();

				if( !ValidateObject( multi ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			case CCP_HOUSESOWNED:		*vp = INT_TO_JSVAL( gPriv->CountHousesOwned( false ));	break;
			case CCP_HOUSESCOOWNED:		*vp = INT_TO_JSVAL( gPriv->CountHousesOwned( true ));	break;
			case CCP_LASTON:
			{
				auto lastOnString = gPriv->GetLastOn();
				tString = JS_NewStringCopyZ( cx, lastOnString.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			}
			case CCP_LASTONSECS:
				JS_NewNumberValue( cx, gPriv->GetLastOnSecs(), vp );
				break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool CCharacterProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CChar *gPriv = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_CHAR );

	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CCP_ACCOUNTNUM:	gPriv->SetAccountNum( static_cast<UI16>( encaps.toInt() ));					break;
			case CCP_CREATEDON:		break;
			case CCP_PLAYTIME:		gPriv->SetPlayTime( static_cast<UI32>( encaps.toInt() ));					break;
			case CCP_SECTIONID:		gPriv->SetSectionId( encaps.toString() );									break;
			case CCP_NAME:			gPriv->SetName( encaps.toString() );										break;
			case CCP_ORIGNAME:		gPriv->SetOrgName( encaps.toString() );										break;
			case CCP_TITLE:			gPriv->SetTitle( encaps.toString() );										break;
			case CCP_X:				gPriv->SetLocation( static_cast<SI16>( encaps.toInt() ), gPriv->GetY(), gPriv->GetZ() );	break;
			case CCP_Y:				gPriv->SetLocation( gPriv->GetX(), static_cast<SI16>( encaps.toInt() ), gPriv->GetZ() );	break;
			case CCP_Z:				gPriv->SetZ( static_cast<SI08>( encaps.toInt() ));							break;
			case CCP_ID:			gPriv->SetId( static_cast<UI16>( encaps.toInt() ));							break;
			case CCP_COLOUR:		gPriv->SetColour( static_cast<UI16>( encaps.toInt() ));						break;
			case CCP_CONTROLSLOTS:	gPriv->SetControlSlots( static_cast<UI16>( encaps.toInt() ));				break;
			case CCP_CONTROLSLOTSUSED:	gPriv->SetControlSlotsUsed( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_ORNERINESS:	gPriv->SetOrneriness( static_cast<UI16>( encaps.toInt() ));					break;
			case CCP_OWNER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar*>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetOwner( myChar );
				}
				else
				{
					gPriv->SetOwner( nullptr );
				}
				break;
			case CCP_VISIBLE:		gPriv->SetVisible( static_cast<VisibleTypes>( encaps.toInt() ));	break;
			case CCP_SERIAL:											break;
			case CCP_HEALTH:		gPriv->SetHP( encaps.toInt() );		break;
			case CCP_SCRIPTTRIGGER:
			{
				// For backwards compatibility; clears out other scripts and assigns a specific script
				UI16 scriptId = static_cast<UI16>( encaps.toInt() );
				cScript *toExecute	= JSMapping->GetScript( scriptId );
				if( toExecute == nullptr )
				{
					ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
				}
				else
				{
					gPriv->ClearScriptTriggers();
					gPriv->AddScriptTrigger( scriptId );
				}
				break;
			}
			case CCP_SCRIPTTRIGGERS:
			{
				if( *vp != JSVAL_NULL )
				{
					UI16 scriptId = static_cast<UI16>( encaps.toInt() );
					cScript *toExecute	= JSMapping->GetScript( scriptId );
					if( toExecute == nullptr )
					{
						ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
					}
					else
					{
						gPriv->AddScriptTrigger( scriptId );
					}
				}
				else
				{
					// If null value was provided, clear script triggers on object
					gPriv->ClearScriptTriggers();
				}
				break; 
			}
			case CCP_WORLDNUMBER:
				if( !Map->InsideValidWorld( gPriv->GetX(), gPriv->GetY(), static_cast<UI08>( encaps.toInt() )))
					return JS_FALSE;

				gPriv->RemoveFromSight();
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), static_cast<UI08>( encaps.toInt() ), gPriv->GetInstanceId() );
				if( !gPriv->IsNpc() )
				{
					SendMapChange( static_cast<UI08>( encaps.toInt() ), gPriv->GetSocket() );
				}
				break;
			case CCP_INSTANCEID:
			{
				// Remove character from nearby players
				gPriv->RemoveFromSight();

				// Remove nearby objects from player!
				if( !gPriv->IsNpc() )
				{
					gPriv->RemoveAllObjectsFromSight( gPriv->GetSocket() );
				}
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), gPriv->WorldNumber(), static_cast<UI16>( encaps.toInt() ));
				break;
			}
			case CCP_TARGET:
				//TODO: Check if the user(admin per jscript) can set the target
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar *>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetTarg( myChar );
				}
				else
				{
					gPriv->SetTarg( nullptr );
				}
				break;
			case CCP_DEXTERITY:		gPriv->SetDexterity( encaps.toInt() );				break;
			case CCP_INTELLIGENCE:	gPriv->SetIntelligence( encaps.toInt() );			break;
			case CCP_STRENGTH:		gPriv->SetStrength( encaps.toInt() );				break;
			case CCP_SKILLS:															break;
			case CCP_MANA:			gPriv->SetMana( encaps.toInt() );					break;
			case CCP_STAMINA:		gPriv->SetStamina( encaps.toInt() );				break;
			case CCP_CHARPACK:															break;
			case CCP_FAME:			gPriv->SetFame( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_KARMA:			gPriv->SetKarma( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_HUNGER:		gPriv->SetHunger( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_THIRST:		gPriv->SetThirst( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_CANATTACK:		gPriv->SetCanAttack( encaps.toBool() );					break;
			case CCP_KARMALOCK:		gPriv->SetKarmaLock( encaps.toBool() );					break;
			case CCP_FLEEAT:		gPriv->SetFleeAt( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_REATTACKAT:	gPriv->SetReattackAt( static_cast<SI16>( encaps.toInt() ));			break;
			case CCP_BRKPEACE:		gPriv->SetBrkPeaceChance( encaps.toInt() );				break;
			case CCP_SETPEACE:		gPriv->SetPeace( encaps.toInt() );						break;
			case CCP_FROZEN:		gPriv->SetFrozen( encaps.toBool() );					break;
			case CCP_COMMANDLEVEL:	gPriv->SetCommandLevel( static_cast<UI08>( encaps.toInt() ));		break;
			case CCP_RACE:			Races->ApplyRace( gPriv, static_cast<RACEID>( encaps.toInt() ), true );	break;
			case CCP_MAXHP:			gPriv->SetFixedMaxHP( static_cast<SI16>( encaps.toInt() ));			break;
			case CCP_MAXMANA:		gPriv->SetFixedMaxMana( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_MAXSTAMINA:	gPriv->SetFixedMaxStam( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_HASSTOLEN:
			{
				if( encaps.toBool() )
				{
					FlagForStealing( gPriv );
				}
				else
				{
					gPriv->HasStolen( false );
					gPriv->SetTimer( tCHAR_STEALFLAG, 0 );
					UpdateFlag( gPriv );
				}
				break;
			}
			case CCP_CRIMINAL:
				if( encaps.toBool() )
				{
					MakeCriminal( gPriv );
				}
				else
				{
					gPriv->SetTimer( tCHAR_CRIMFLAG, 0 );
					UpdateFlag( gPriv );
				}
				break;
			case CCP_INNOCENT:
				if( !encaps.toBool() )
				{
					MakeCriminal( gPriv );
				}
				else
				{
					gPriv->SetTimer( tCHAR_CRIMFLAG, 0 );
					UpdateFlag( gPriv );
				}
				break;
			case CCP_NEUTRAL:
				if( encaps.toBool() )
				{
					gPriv->SetFlagNeutral();
				}
				else
				{
					gPriv->SetFlagBlue();
				}
				gPriv->SetTimer( tCHAR_CRIMFLAG, 0 );
				UpdateFlag( gPriv );
				break;
			case CCP_MURDERCOUNT:
				gPriv->SetKills( static_cast<SI16>( encaps.toInt() ));
				UpdateFlag( gPriv );
				break;
			case CCP_GENDER:
				switch( static_cast<SI16>( encaps.toInt() ))
				{
					case 0:	// male
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x0192 );
						}
						else
						{
							gPriv->SetId( 0x0190 );
						}
						break;
					case 1:	// female
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x0193 );
						}
						else
						{
							gPriv->SetId( 0x0191 );
						}
						break;
					case 2:	// elf male
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x025F );
						}
						else
						{
							gPriv->SetId( 0x025D );
						}
						break;
					case 3:	// elf female
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x0260 );
						}
						else
						{
							gPriv->SetId( 0x025E );
						}
						break;
					case 4:	// gargoyle male
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x02B6 );
						}
						else
						{
							gPriv->SetId( 0x029A );
						}
						break;
					case 5: // gargoyle female
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x02B7 );
						}
						else
						{
							gPriv->SetId( 0x029B );
						}
						break;
					default:
						break;
				}
				break;
			case CCP_NPC:			gPriv->SetNpc( encaps.toBool() );						break;
			case CCP_AWAKE:			gPriv->SetAwake( encaps.toBool() );						break;
			case CCP_DIRECTION:		gPriv->SetDir( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_REGION:		gPriv->SetRegion( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_HEALTHREGENBONUS:	gPriv->SetHealthRegenBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_STAMINAREGENBONUS:	gPriv->SetStaminaRegenBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_MANAREGENBONUS:	gPriv->SetManaRegenBonus( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_ORIGIN:		gPriv->SetOrigin( cwmWorldState->ServerData()->EraStringToEnum( encaps.toString() ));	break;
			case CCP_TOWN:
				cwmWorldState->townRegions[gPriv->GetTown()]->RemoveTownMember( *gPriv );
				if( *vp != JSVAL_NULL )
				{
					cwmWorldState->townRegions[encaps.toInt()]->AddAsTownMember( *gPriv );
				}
				break;
			case CCP_GUILD:
				if( !gPriv->IsNpc() )
				{
					GuildSys->Resign( gPriv->GetSocket() );

					if( *vp != JSVAL_NULL )
					{
						CGuild *myGuild = static_cast<CGuild*>( encaps.toObject() );
						myGuild->NewRecruit( gPriv->GetSerial() );
					}
				}
				break;
			case CCP_GUILDNUMBER: gPriv->SetGuildNumber( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_OLDWANDERTYPE: gPriv->SetOldNpcWander( static_cast<SI08>( encaps.toInt() )); 		break;
			case CCP_WANDERTYPE: 	gPriv->SetNpcWander( static_cast<SI08>( encaps.toInt() ), true );	break;
			case CCP_FX1:			gPriv->SetFx( static_cast<SI16>( encaps.toInt() ), 0 );	break;
			case CCP_FY1:			gPriv->SetFy( static_cast<SI16>( encaps.toInt() ), 0 );	break;
			case CCP_FX2:			gPriv->SetFx( static_cast<SI16>( encaps.toInt() ), 1 );	break;
			case CCP_FY2:			gPriv->SetFy( static_cast<SI16>( encaps.toInt() ), 1 );	break;
			case CCP_FZ:			gPriv->SetFz( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_SPAWNX:		gPriv->SetSpawnX( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_SPAWNY:		gPriv->SetSpawnY( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_SPAWNZ:		gPriv->SetSpawnZ( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_PATHTARGX:		gPriv->SetPathTargX( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_PATHTARGY:		gPriv->SetPathTargY( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_ISONHORSE:		gPriv->SetOnHorse( encaps.toBool() );					break;
			case CCP_TDEXTERITY:	gPriv->SetDexterity2( encaps.toInt() );					break;
			case CCP_TINTELLIGENCE:	gPriv->SetIntelligence2( encaps.toInt() );				break;
			case CCP_TSTRENGTH:		gPriv->SetStrength2( encaps.toInt() );					break;
			case CCP_LIGHTLEVEL:
				gPriv->SetFixedLight( static_cast<UI08>( encaps.toInt() ));
				if( gPriv->GetSocket() != nullptr )
				{
					if( static_cast<UI08>( encaps.toInt() ) == 255 )
					{
						DoLight( gPriv->GetSocket(), cwmWorldState->ServerData()->WorldLightCurrentLevel() );
					}
					else
					{
						DoLight( gPriv->GetSocket(), static_cast<UI08>( encaps.toInt() ));
					}
				}
				break;
			case CCP_VULNERABLE:	gPriv->SetInvulnerable( !encaps.toBool() );		break;
			case CCP_HUNGERSTATUS:	gPriv->SetHungerStatus( encaps.toBool() );		break;
			case CCP_THIRSTSTATUS:	gPriv->SetThirstStatus( encaps.toBool() );		break;
			case CCP_LODAMAGE:		gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_HIDAMAGE:		gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_ATWAR:
				gPriv->SetWar( encaps.toBool() );
				Movement->CombatWalk( gPriv );
				break;
			case CCP_SPELLCAST:		gPriv->SetSpellCast( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_ISCASTING:
			{
				bool isCasting = encaps.toBool();
				gPriv->SetCasting( isCasting );
				gPriv->SetJSCasting( isCasting );
			}
				break;
			case CCP_PRIV:			gPriv->SetPriv( static_cast<UI32>( encaps.toInt() ));			break;
			case CCP_TOWNPRIV:		gPriv->SetTownpriv( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_GUILDTITLE:	gPriv->SetGuildTitle( encaps.toString() );						break;
			case CCP_HAIRSTYLE:		gPriv->SetHairStyle( static_cast<UI16>( encaps.toInt() ) );		break;
			case CCP_HAIRCOLOUR:	gPriv->SetHairColour( static_cast<UI16>( encaps.toInt() ) );	break;
			case CCP_BEARDSTYLE:	gPriv->SetBeardStyle( static_cast<UI16>( encaps.toInt() ) );	break;
			case CCP_BEARDCOLOUR:	gPriv->SetBeardColour( static_cast<UI16>( encaps.toInt() ) );	break;
			case CCP_FONTTYPE:		gPriv->SetFontType( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_SAYCOLOUR:		gPriv->SetSayColour( static_cast<COLOUR>( encaps.toInt() ));	break;
			case CCP_EMOTECOLOUR:	gPriv->SetEmoteColour( static_cast<COLOUR>( encaps.toInt() ));	break;
			case CCP_ATTACKER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar *>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetAttacker( myChar );
				}
				else
				{
					gPriv->SetAttacker( nullptr );
				}
				break;
			case CCP_RACEGATE:		gPriv->SetRaceGate( static_cast<RACEID>( encaps.toInt() ));	break;
			case CCP_SKILLLOCK:																	break;
			case CCP_SKILLCAP:																	break;
			case CCP_DEATHS:		gPriv->SetDeaths( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_NEXTACT:		gPriv->SetNextAct( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_CELL:			gPriv->SetCell( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_ALLMOVE:		gPriv->SetAllMove( encaps.toBool() );						break;
			case CCP_HOUSEICONS:	gPriv->SetViewHouseAsIcon( encaps.toBool() );				break;
			case CCP_SPATTACK:		gPriv->SetSpAttack( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_SPDELAY:		gPriv->SetSpDelay( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_SWINGSPEEDINCREASE:	gPriv->SetSwingSpeedIncrease( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_LUCK:			gPriv->SetLuck( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_HITCHANCE:		gPriv->SetHitChance( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_DEFENSECHANCE:	gPriv->SetDefenseChance( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_DAMAGEINCREASE:gPriv->SetDamageIncrease( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_AITYPE:		gPriv->SetNPCAiType( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_SPLIT:			gPriv->SetSplit( static_cast<UI08>( encaps.toInt() ));		break;
			case CCP_SPLITCHANCE:	gPriv->SetSplitChance( static_cast<UI08>( encaps.toInt() ));break;
			case CCP_VENDORGOLDHELD:gPriv->SetHoldG( static_cast<UI32>( encaps.toInt() ));		break;
			case CCP_HIRELING:		gPriv->SetCanHire( encaps.toBool() );						break;
			case CCP_TRAINER:		gPriv->SetCanTrain( encaps.toBool() );						break;
			case CCP_WEIGHT:		gPriv->SetWeight( static_cast<SI32>( encaps.toInt() ));		break;
			case CCP_SQUELCH:		gPriv->SetSquelched( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_MAGICREFLECT:	gPriv->SetTempReflected( encaps.toBool() );					break;
			case CCP_PERMMAGICREFLECT:	gPriv->SetPermReflected( encaps.toBool() );				break;
			case CCP_HIDEFAMEKARMATITLE: gPriv->HideFameKarmaTitle( encaps.toBool() );			break;
			case CCP_NOCHARCOLLIDE: gPriv->NoCharCollide( encaps.toBool() );					break;
			case CCP_TAMED:			gPriv->SetTamed( encaps.toBool() );							break;
			case CCP_TAMEDHUNGERRATE: gPriv->SetTamedHungerRate( static_cast<UI16>( encaps.toInt() )); break;
			case CCP_TAMEDTHIRSTRATE: gPriv->SetTamedThirstRate( static_cast<UI16>( encaps.toInt() )); break;
			case CCP_THIRSTWILDCHANCE: gPriv->SetTamedThirstWildChance( static_cast<UI08>( encaps.toInt() )); break;
			case CCP_HUNGERWILDCHANCE: gPriv->SetTamedHungerWildChance( static_cast<UI08>( encaps.toInt() )); break;
			case CCP_FOODLIST:		gPriv->SetFood( encaps.toString() );			break;
			case CCP_MOUNTED:		gPriv->SetMounted( encaps.toBool() );			break;
			case CCP_STABLED:		gPriv->SetStabled( encaps.toBool() );			break;
			case CCP_USINGPOTION:	gPriv->SetUsingPotion( encaps.toBool() );		break;
			case CCP_STEALTH:		gPriv->SetStealth( encaps.toInt() );			break;
			case CCP_SKILLTOTAME:	gPriv->SetTaming( encaps.toInt() );				break;
			case CCP_SKILLTOPROV:	gPriv->SetProvoing( encaps.toInt() );			break;
			case CCP_SKILLTOPEACE:	gPriv->SetPeaceing( encaps.toInt() );			break;
			case CCP_POISON:		gPriv->SetPoisoned( static_cast<UI08>( encaps.toInt() ));			break;
			case CCP_POISONEDBY:	gPriv->SetPoisoned( static_cast<UI32>( encaps.toInt() ));			break;
			case CCP_POISONSTRENGTH:	gPriv->SetPoisonStrength( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_ISPOLYMORPHED:	gPriv->IsPolymorphed( encaps.toBool() );		break;
			case CCP_ISINCOGNITO:	gPriv->IsIncognito( encaps.toBool() );			break;
			case CCP_ISDISGUISED:	gPriv->IsDisguised( encaps.toBool() );			break;
			case CCP_CANRUN:		gPriv->SetRun( encaps.toBool() );				break;
			case CCP_ISGUARDED:		gPriv->SetGuarded( encaps.toBool() );			break;
			case CCP_GUARDING:
				if( *vp != JSVAL_NULL )
				{
					CBaseObject *myObj = static_cast<CBaseObject *>( encaps.toObject() );
					if( !ValidateObject( myObj ))
						break;

					gPriv->SetGuarding( myObj );
				}
				else
				{
					gPriv->SetGuarding( nullptr );
				}
				break;
			case CCP_ISMEDITATING:	gPriv->SetMeditating( encaps.toBool() );			break;
			case CCP_ISGM:			gPriv->SetGM( encaps.toBool() );					break;
			case CCP_TITHING:		gPriv->SetTithing( static_cast<SI32>( encaps.toInt() ));		break;
			case CCP_CANBROADCAST:	gPriv->SetBroadcast( encaps.toBool() );				break;
			case CCP_SINGCLICKSER:	gPriv->SetSingClickSer( encaps.toBool() );			break;
			case CCP_NOSKILLTITLES:	gPriv->SetSkillTitles( encaps.toBool() );			break;
			case CCP_ISGMPAGEABLE:	gPriv->SetGMPageable( encaps.toBool() );			break;
			case CCP_CANSNOOP:		gPriv->SetSnoop( encaps.toBool() );					break;
			case CCP_ISCOUNSELOR:	gPriv->SetCounselor( encaps.toBool() );				break;
			case CCP_NONEEDMANA:	gPriv->SetNoNeedMana( encaps.toBool() );			break;
			case CCP_ISDISPELLABLE:	gPriv->SetDispellable( encaps.toBool() );			break;
			case CCP_NONEEDREAGS:	gPriv->SetNoNeedReags( encaps.toBool() );			break;
			case CCP_ORGID:			gPriv->SetOrgId( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_ORGSKIN:		gPriv->SetOrgSkin( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_NPCFLAG:
				gPriv->SetNPCFlag( static_cast<cNPC_FLAG>( encaps.toInt() ));
				UpdateFlag( gPriv );
				break;
			case CCP_NPCGUILD:		gPriv->SetNPCGuild( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_ISSHOP:
				if( encaps.toBool() )
				{
					MakeShop( gPriv );
				}
				else
				{
					gPriv->SetShop( false );
					CItem *tPack = nullptr;
					for( UI08 i = IL_SELLCONTAINER; i <= IL_BUYCONTAINER; ++i )
					{
						tPack = gPriv->GetItemAtLayer( static_cast<ItemLayers>( i ));
						if( ValidateObject( tPack ))
						{
							tPack->Delete();
						}
					}
					gPriv->Update();
				}
				break;
			case CCP_MAXLOYALTY:	gPriv->SetMaxLoyalty( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_LOYALTY:		gPriv->SetLoyalty( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_SHOULDSAVE:	gPriv->ShouldSave( encaps.toBool() );			break;
			case CCP_PARTYLOOTABLE:
			{
				Party *toGet = PartyFactory::GetSingleton().Get( gPriv );
				if( toGet != nullptr )
				{
					CPartyEntry *toScan = toGet->Find( gPriv );
					if( toScan != nullptr )
					{
						toScan->IsLootable( encaps.toBool() );
					}
				}
				break;
			}
			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool CRegionProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CTownRegion *gPriv = static_cast<CTownRegion *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_REGION );

	if( propID != 0xFFFF )
	{
		JSString *tString = nullptr;
		switch( propID )
		{
			case CREGP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CREGP_MAYOR:
				CChar *tempMayor;
				tempMayor = gPriv->GetMayor();

				if( !ValidateObject( tempMayor ))
					*vp = JSVAL_NULL;
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempMayor, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CREGP_RACE:				*vp = INT_TO_JSVAL( gPriv->GetRace() );					break;
			case CREGP_TAX:					*vp = INT_TO_JSVAL( gPriv->TaxedAmount() );				break;
			case CREGP_TAXRESOURCE:			*vp = INT_TO_JSVAL( gPriv->GetResourceId() );			break;
			case CREGP_CANMARK:				*vp = BOOLEAN_TO_JSVAL( gPriv->CanMark() );				break;
			case CREGP_CANRECALL:			*vp = BOOLEAN_TO_JSVAL( gPriv->CanRecall() );			break;
			case CREGP_CANGATE:				*vp = BOOLEAN_TO_JSVAL( gPriv->CanGate() );				break;
			case CREGP_CANTELEPORT:			*vp = BOOLEAN_TO_JSVAL( gPriv->CanTeleport() );			break;
			case CREGP_CANPLACEHOUSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanPlaceHouse() );		break;
			case CREGP_ISGUARDED:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );			break;
			case CREGP_WORLDNUMBER:			*vp = INT_TO_JSVAL( gPriv->WorldNumber() );				break;
			case CREGP_INSTANCEID:			*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );			break;
			case CREGP_CANCASTAGGRESSIVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanCastAggressive() );	break;
			case CREGP_ISSAFEZONE:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsSafeZone() );			break;
			case CREGP_HEALTH:				*vp = INT_TO_JSVAL( gPriv->GetHealth() );				break;
			case CREGP_ISDUNGEON:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDungeon() );			break;
			case CREGP_ISDISABLED:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDisabled() );			break;
			case CREGP_CHANCEBIGORE:		*vp = INT_TO_JSVAL( gPriv->GetChanceBigOre() );			break;
			case CREGP_NUMOREPREFERENCES:	*vp = INT_TO_JSVAL( gPriv->GetNumOrePreferences() );	break;
			case CREGP_POPULATION:			*vp = INT_TO_JSVAL( gPriv->GetPopulation() );			break;
			case CREGP_MEMBERS:
				tString = JS_NewStringCopyZ( cx, gPriv->GetTownMemberSerials().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CREGP_ID:					*vp = INT_TO_JSVAL( gPriv->GetRegionNum() );			break;
			case CREGP_NUMGUARDS:			*vp = INT_TO_JSVAL( gPriv->NumGuards() );				break;
			case CREGP_SCRIPTTRIGGER:
			{
				// For backwards compatibility, get last scripttrigger from vector
				// For older worldfiles, this will be the only scripttrigger added to the vector after load
				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				UI16 lastScriptTrigger = 0;
				auto numberOfTriggers = scriptTriggers.size();
				if( numberOfTriggers > 0 )
				{
					lastScriptTrigger = scriptTriggers[numberOfTriggers - 1];
				}

				*vp = INT_TO_JSVAL( lastScriptTrigger );
				break;
			}
			case CREGP_SCRIPTTRIGGERS:
			{
				jsval scriptId;
				JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
				{
					scriptId = INT_TO_JSVAL( scriptTriggers[i] );
					JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
				}

				*vp = OBJECT_TO_JSVAL( scriptTriggersJS );
				break;
			}
			case CREGP_TAXES:				*vp = INT_TO_JSVAL( gPriv->GetTaxes() );				break;
			case CREGP_RESERVES:			*vp = INT_TO_JSVAL( gPriv->GetReserves() );				break;
			case CREGP_APPEARANCE:			*vp = INT_TO_JSVAL( gPriv->GetAppearance() );			break;
			case CREGP_MUSIC:				*vp = INT_TO_JSVAL( gPriv->GetMusicList() );			break;
			case CREGP_WEATHER:				*vp = INT_TO_JSVAL( gPriv->GetWeather() );				break;
			case CREGP_OWNER:
				tString = JS_NewStringCopyZ( cx, gPriv->GetOwner().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}
JSBool CRegionProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CTownRegion *gPriv = static_cast<CTownRegion *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_REGION );

	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CREGP_NAME:				gPriv->SetName( encaps.toString() );						break;
			case CREGP_MAYOR:				gPriv->SetMayorSerial( static_cast<UI32>( encaps.toInt() )); break;
			case CREGP_RACE:				gPriv->SetRace( static_cast<RACEID>( encaps.toInt() ));		break;
			case CREGP_TAX:					gPriv->TaxedAmount( static_cast<SI16>( encaps.toInt() ));	break;
			case CREGP_TAXRESOURCE:			gPriv->SetResourceId( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_CANMARK:				gPriv->CanMark( encaps.toBool() );							break;
			case CREGP_CANRECALL:			gPriv->CanRecall( encaps.toBool() );						break;
			case CREGP_CANGATE:				gPriv->CanGate( encaps.toBool() );							break;
			case CREGP_CANTELEPORT:			gPriv->CanTeleport( encaps.toBool() );						break;
			case CREGP_CANPLACEHOUSE:		gPriv->CanPlaceHouse( encaps.toBool() );					break;
			case CREGP_ISGUARDED:			gPriv->IsGuarded( encaps.toBool() );						break;
			case CREGP_CANCASTAGGRESSIVE:	gPriv->CanCastAggressive( encaps.toBool() );				break;
			case CREGP_ISSAFEZONE:			gPriv->IsSafeZone( encaps.toBool() );						break;
			case CREGP_HEALTH:				gPriv->SetHealth( static_cast<SI16>( encaps.toInt() ));		break;
			case CREGP_ISDUNGEON:			gPriv->IsDungeon( encaps.toBool() );						break;
			case CREGP_ISDISABLED:			gPriv->IsDisabled( encaps.toBool() );						break;
			case CREGP_CHANCEBIGORE:		gPriv->SetChanceBigOre( static_cast<UI08>( encaps.toInt() ));	break;
			case CREGP_NUMGUARDS:			gPriv->SetNumGuards( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_SCRIPTTRIGGER:
			{
				// For backwards compatibility; clears out other scripts and assigns a specific script
				UI16 scriptId = static_cast<UI16>( encaps.toInt() );
				cScript *toExecute	= JSMapping->GetScript( scriptId );
				if( toExecute == nullptr )
				{
					ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
				}
				else
				{
					gPriv->ClearScriptTriggers();
					gPriv->AddScriptTrigger( scriptId );
				}
				break;
			}
			case CREGP_SCRIPTTRIGGERS:
			{
				if( *vp != JSVAL_NULL )
				{
					UI16 scriptId = static_cast<UI16>( encaps.toInt() );
					cScript *toExecute	= JSMapping->GetScript( scriptId );
					if( toExecute == nullptr )
					{
						ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
					}
					else
					{
						gPriv->AddScriptTrigger( scriptId );
					}
				}
				else
				{
					// If null value was provided, clear script triggers on object
					gPriv->ClearScriptTriggers();
				}
				break; 
			}
			case CREGP_TAXES:				gPriv->SetTaxesLeft( static_cast<UI32>( encaps.toInt() ));	break;
			case CREGP_RESERVES:			gPriv->SetReserves( static_cast<UI32>( encaps.toInt() ));	break;
			case CREGP_APPEARANCE:			gPriv->SetAppearance( static_cast<WorldType>( encaps.toInt() )); break;
			case CREGP_MUSIC:				gPriv->SetMusicList( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_WEATHER:				gPriv->SetWeather( static_cast<WEATHID>( encaps.toInt() ));	break;
			case CREGP_WORLDNUMBER:			gPriv->WorldNumber( static_cast<UI08>( encaps.toInt() ));	break;
			case CREGP_INSTANCEID:			gPriv->SetInstanceId( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_MEMBERS:
				break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

IMPL_GETS( CRegion, name,              CTownRegion, setString,  GetName().c_str() )
IMPL_GET(  CRegion, race,              CTownRegion, setInt32,   GetRace() )
IMPL_GET(  CRegion, tax,               CTownRegion, setInt32,   TaxedAmount() )
IMPL_GET(  CRegion, taxResource,       CTownRegion, setInt32,   GetResourceId() )
IMPL_GET(  CRegion, canMark,           CTownRegion, setBoolean, CanMark() )
IMPL_GET(  CRegion, canRecall,         CTownRegion, setBoolean, CanRecall() )
IMPL_GET(  CRegion, canGate,           CTownRegion, setBoolean, CanGate() )
IMPL_GET(  CRegion, canTeleport,       CTownRegion, setBoolean, CanTeleport() )
IMPL_GET(  CRegion, canPlaceHouse,     CTownRegion, setBoolean, CanPlaceHouse() )
IMPL_GET(  CRegion, isGuarded,         CTownRegion, setBoolean, IsGuarded() )
IMPL_GET(  CRegion, canCastAggressive, CTownRegion, setBoolean, CanCastAggressive() )
IMPL_GET(  CRegion, isSafeZone,        CTownRegion, setBoolean, IsSafeZone() )
IMPL_GET(  CRegion, health,            CTownRegion, setInt32,   GetHealth() )
IMPL_GET(  CRegion, isDungeon,         CTownRegion, setBoolean, IsDungeon() )
IMPL_GET(  CRegion, isDisabled,        CTownRegion, setBoolean, IsDisabled() )
IMPL_GET(  CRegion, worldNumber,       CTownRegion, setInt32,   WorldNumber() )
IMPL_GET(  CRegion, instanceID,        CTownRegion, setInt32,   GetInstanceId() )
IMPL_GET(  CRegion, chanceBigOre,      CTownRegion, setInt32,   GetChanceBigOre() )
IMPL_GET(  CRegion, numOrePrefs,       CTownRegion, setInt32,   GetNumOrePreferences() )
IMPL_GET(  CRegion, population,        CTownRegion, setInt32,   GetPopulation() )
IMPL_GETS( CRegion, members,           CTownRegion, setString,  GetTownMemberSerials().c_str() )
IMPL_GET(  CRegion, id,                CTownRegion, setInt32,   GetRegionNum() )
IMPL_GET(  CRegion, numGuards,         CTownRegion, setInt32,   NumGuards() )
IMPL_GET(  CRegion, taxes,             CTownRegion, setInt32,   GetTaxes() )
IMPL_GET(  CRegion, reserves,          CTownRegion, setInt32,   GetReserves() )
IMPL_GET(  CRegion, appearance,        CTownRegion, setInt32,   GetAppearance() )
IMPL_GET(  CRegion, music,             CTownRegion, setInt32,   GetMusicList() )
IMPL_GET(  CRegion, weather,           CTownRegion, setInt32,   GetWeather() )
IMPL_GETS( CRegion, owner,             CTownRegion, setString,  GetOwner().c_str() )

FDCLG( CRegion, mayor )
{
	FNARGS
	auto region = JS::GetMaybePtrFromReservedSlot<CTownRegion>( thisObj, 0 );
	CChar *mayor = region->GetMayor();
	if( !ValidateObject( mayor ))
	{
		args.rval().setNull();
		return true;
	}

	JSObject *mayorObject = JSEngine->AcquireObject( IUE_CHAR, mayor, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( mayorObject == nullptr )
		return false;
	args.rval().setObject( *mayorObject );
	return true;
}

FDCLG( CRegion, scriptTrigger )
{
	FNARGS
	auto region = JS::GetMaybePtrFromReservedSlot<CTownRegion>( thisObj, 0 );
	const auto &triggers = region->GetScriptTriggers();
	args.rval().setInt32( triggers.empty() ? 0 : triggers.back() );
	return true;
}

FDCLG( CRegion, scriptTriggers )
{
	FNARGS
	auto region = JS::GetMaybePtrFromReservedSlot<CTownRegion>( thisObj, 0 );
	const auto &triggers = region->GetScriptTriggers();
	JS::RootedObject result( cx, JS::NewArrayObject( cx, triggers.size() ));
	if( result == nullptr )
		return false;
	for( uint32_t index = 0; index < triggers.size(); ++index )
	{
		JS::RootedValue value( cx, JS::Int32Value( triggers[index] ));
		if( !JS_SetElement( cx, result, index, value ))
			return false;
	}
	args.rval().setObject( *result );
	return true;
}

JSBool CSpawnRegionProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CSpawnRegion *gPriv = static_cast<CSpawnRegion *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_SPAWNREGION );

	if( propID != 0xFFFF )
	{
		JSString *tString = nullptr;
		switch( propID )
		{
			case CSPAWNREGP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CSPAWNREGP_REGIONNUM:				*vp = INT_TO_JSVAL( gPriv->GetRegionNum() );		break;
			case CSPAWNREGP_ITEM:
			case CSPAWNREGP_ITEMLIST:
			{
				// This could potentially be a list of item ids - let's convert it to a comma-separated string!
				auto itemList = gPriv->GetItem();
				std::string s;
				for( const auto &piece : itemList )
				{
					if( s.empty() )
					{
						s += piece;
					}
					else
					{
						s += ( "," + piece );
					}
				}
				tString = JS_NewStringCopyZ( cx, s.c_str() );
				*vp = STRING_TO_JSVAL( tString );
			}
				break;
			case CSPAWNREGP_NPC:
			case CSPAWNREGP_NPCLIST:
			{
				// This could potentially be a list of NPC ids - let's convert it to a comma-separated string!
				auto  npcList = gPriv->GetNPC();
				std::string s;
				for( const auto &piece : npcList )
				{
					if( s.empty() )
					{
						s += piece;
					}
					else
					{
						s += ( "," + piece );
					}
				}
				tString = JS_NewStringCopyZ( cx, s.c_str() );
				*vp = STRING_TO_JSVAL( tString );
			}
				break;
			case CSPAWNREGP_ITEMCOUNT:				*vp = INT_TO_JSVAL( gPriv->GetCurrentItemAmt() );				break;
			case CSPAWNREGP_NPCCOUNT:				*vp = INT_TO_JSVAL( gPriv->GetCurrentCharAmt() );				break;
			case CSPAWNREGP_MAXITEMS:				*vp = INT_TO_JSVAL( gPriv->GetMaxItemSpawn() );					break;
			case CSPAWNREGP_MAXNPCS:				*vp = INT_TO_JSVAL( gPriv->GetMaxCharSpawn() );					break;
			case CSPAWNREGP_MINTIME:				*vp = INT_TO_JSVAL( gPriv->GetMinTime() );						break;
			case CSPAWNREGP_MAXTIME:				*vp = INT_TO_JSVAL( gPriv->GetMaxTime() );						break;
			case CSPAWNREGP_ONLYOUTSIDE:			*vp = BOOLEAN_TO_JSVAL( gPriv->GetOnlyOutside() );				break;
			case CSPAWNREGP_ISSPAWNER:				*vp = BOOLEAN_TO_JSVAL( gPriv->IsSpawner() );					break;
			case CSPAWNREGP_FORCESPAWN:				*vp = BOOLEAN_TO_JSVAL( gPriv->GetForceSpawn() );				break;
			case CSPAWNREGP_X1:						*vp = INT_TO_JSVAL( gPriv->GetX1() );							break;
			case CSPAWNREGP_Y1:						*vp = INT_TO_JSVAL( gPriv->GetY1() );							break;
			case CSPAWNREGP_X2:						*vp = INT_TO_JSVAL( gPriv->GetX2() );							break;
			case CSPAWNREGP_Y2:						*vp = INT_TO_JSVAL( gPriv->GetY2() );							break;
			case CSPAWNREGP_DEFZ:					*vp = INT_TO_JSVAL( gPriv->GetDefZ() );							break;
			case CSPAWNREGP_PREFZ:					*vp = INT_TO_JSVAL( gPriv->GetPrefZ() );						break;
			case CSPAWNREGP_WORLD:					*vp = INT_TO_JSVAL( gPriv->WorldNumber() );						break;
			case CSPAWNREGP_INSTANCEID:				*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );					break;
			case CSPAWNREGP_CALL:					*vp = INT_TO_JSVAL( gPriv->GetCall() );							break;
			default:
				break;
		}
	}

	return JS_TRUE;
}
JSBool CSpawnRegionProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CSpawnRegion *gPriv = static_cast<CSpawnRegion *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_SPAWNREGION );

	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CSPAWNREGP_NAME:				gPriv->SetName( encaps.toString() );							break;
			case CSPAWNREGP_ITEM:				gPriv->SetItem( encaps.toString() );							break;
			case CSPAWNREGP_ITEMLIST:			gPriv->SetItemList( encaps.toString() );						break;
			case CSPAWNREGP_NPC:				gPriv->SetNPC( encaps.toString() );								break;
			case CSPAWNREGP_NPCLIST:			gPriv->SetNPCList( encaps.toString() );							break;
			case CSPAWNREGP_REGIONNUM:			gPriv->SetRegionNum( static_cast<UI16>( encaps.toInt() ));		break;
			case CSPAWNREGP_MAXITEMS:			gPriv->SetMaxItemSpawn( encaps.toInt() );						break;
			case CSPAWNREGP_MAXNPCS:			gPriv->SetMaxCharSpawn( encaps.toInt() );						break;
			case CSPAWNREGP_MINTIME:			gPriv->SetMinTime( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_MAXTIME:			gPriv->SetMaxTime( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_ONLYOUTSIDE:		gPriv->SetOnlyOutside( encaps.toBool() );						break;
			case CSPAWNREGP_ISSPAWNER:			gPriv->IsSpawner( encaps.toBool() );							break;
			case CSPAWNREGP_FORCESPAWN:			gPriv->SetForceSpawn( encaps.toBool() );						break;
			case CSPAWNREGP_X1:					gPriv->SetX1( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_Y1:					gPriv->SetY1( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_X2:					gPriv->SetX2( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_Y2:					gPriv->SetY2( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_DEFZ:				gPriv->SetDefZ( static_cast<SI08>( encaps.toInt() ));			break;
			case CSPAWNREGP_PREFZ:				gPriv->SetPrefZ( static_cast<SI08>( encaps.toInt() ));			break;
			case CSPAWNREGP_WORLD:				gPriv->WorldNumber( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_INSTANCEID:			gPriv->SetInstanceId( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_CALL:				gPriv->SetCall( static_cast<UI16>( encaps.toInt() ));			break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

static std::string JoinSpawnEntries( const std::vector<std::string> &entries )
{
	std::string result;
	for( const auto &entry : entries )
	{
		if( !result.empty() )
			result += ',';
		result += entry;
	}
	return result;
}

IMPL_GETS( CSpawnRegion, name,        CSpawnRegion, setString,  GetName().c_str() )
IMPL_GET(  CSpawnRegion, regionNum,   CSpawnRegion, setInt32,   GetRegionNum() )
#define IMPL_GET_SPAWN_LIST( attr, accessor )                         \
FDCLG( CSpawnRegion, attr )                                           \
{                                                                    \
	FNARGS                                                             \
	auto region = JS::GetMaybePtrFromReservedSlot<CSpawnRegion>( thisObj, 0 ); \
	const std::string entries = JoinSpawnEntries( region->accessor() ); \
	JSString *value = JS_NewStringCopyZ( cx, entries.c_str() );         \
	if( value == nullptr )                                              \
		return false;                                                    \
	args.rval().setString( value );                                     \
	return true;                                                        \
}

IMPL_GET_SPAWN_LIST( itemList, GetItem )
IMPL_GET_SPAWN_LIST( npcList,  GetNPC )
IMPL_GET_SPAWN_LIST( item,     GetItem )
IMPL_GET_SPAWN_LIST( npc,      GetNPC )

#undef IMPL_GET_SPAWN_LIST
IMPL_GET(  CSpawnRegion, maxItems,    CSpawnRegion, setInt32,   GetMaxItemSpawn() )
IMPL_GET(  CSpawnRegion, maxNpcs,     CSpawnRegion, setInt32,   GetMaxCharSpawn() )
IMPL_GET(  CSpawnRegion, itemCount,   CSpawnRegion, setInt32,   GetCurrentItemAmt() )
IMPL_GET(  CSpawnRegion, npcCount,    CSpawnRegion, setInt32,   GetCurrentCharAmt() )
IMPL_GET(  CSpawnRegion, onlyOutside, CSpawnRegion, setBoolean, GetOnlyOutside() )
IMPL_GET(  CSpawnRegion, isSpawner,   CSpawnRegion, setBoolean, IsSpawner() )
IMPL_GET(  CSpawnRegion, forceSpawn,  CSpawnRegion, setBoolean, GetForceSpawn() )
IMPL_GET(  CSpawnRegion, defZ,        CSpawnRegion, setInt32,   GetDefZ() )
IMPL_GET(  CSpawnRegion, prefZ,       CSpawnRegion, setInt32,   GetPrefZ() )
IMPL_GET(  CSpawnRegion, x1,          CSpawnRegion, setInt32,   GetX1() )
IMPL_GET(  CSpawnRegion, y1,          CSpawnRegion, setInt32,   GetY1() )
IMPL_GET(  CSpawnRegion, x2,          CSpawnRegion, setInt32,   GetX2() )
IMPL_GET(  CSpawnRegion, y2,          CSpawnRegion, setInt32,   GetY2() )
IMPL_GET(  CSpawnRegion, world,       CSpawnRegion, setInt32,   WorldNumber() )
IMPL_GET(  CSpawnRegion, instanceID,  CSpawnRegion, setInt32,   GetInstanceId() )
IMPL_GET(  CSpawnRegion, minTime,     CSpawnRegion, setInt32,   GetMinTime() )
IMPL_GET(  CSpawnRegion, maxTime,     CSpawnRegion, setInt32,   GetMaxTime() )
IMPL_GET(  CSpawnRegion, call,        CSpawnRegion, setInt32,   GetCall() )

IMPL_SETS( CSpawnRegion, name,        CSpawnRegion, toString,  SetName )
IMPL_SETS( CSpawnRegion, itemList,    CSpawnRegion, toString,  SetItemList )
IMPL_SETS( CSpawnRegion, npcList,     CSpawnRegion, toString,  SetNPCList )
IMPL_SETS( CSpawnRegion, item,        CSpawnRegion, toString,  SetItem )
IMPL_SETS( CSpawnRegion, npc,         CSpawnRegion, toString,  SetNPC )
IMPL_SET(  CSpawnRegion, maxItems,    CSpawnRegion, toInt32,   SetMaxItemSpawn )
IMPL_SET(  CSpawnRegion, maxNpcs,     CSpawnRegion, toInt32,   SetMaxCharSpawn )
IMPL_SET(  CSpawnRegion, onlyOutside, CSpawnRegion, toBoolean, SetOnlyOutside )
IMPL_SET(  CSpawnRegion, isSpawner,   CSpawnRegion, toBoolean, IsSpawner )
IMPL_SET(  CSpawnRegion, forceSpawn,  CSpawnRegion, toBoolean, SetForceSpawn )
IMPL_SET(  CSpawnRegion, defZ,        CSpawnRegion, toInt32,   SetDefZ )
IMPL_SET(  CSpawnRegion, prefZ,       CSpawnRegion, toInt32,   SetPrefZ )
IMPL_SET(  CSpawnRegion, x1,          CSpawnRegion, toInt32,   SetX1 )
IMPL_SET(  CSpawnRegion, y1,          CSpawnRegion, toInt32,   SetY1 )
IMPL_SET(  CSpawnRegion, x2,          CSpawnRegion, toInt32,   SetX2 )
IMPL_SET(  CSpawnRegion, y2,          CSpawnRegion, toInt32,   SetY2 )
IMPL_SET(  CSpawnRegion, world,       CSpawnRegion, toInt32,   WorldNumber )
IMPL_SET(  CSpawnRegion, instanceID,  CSpawnRegion, toInt32,   SetInstanceId )
IMPL_SET(  CSpawnRegion, minTime,     CSpawnRegion, toInt32,   SetMinTime )
IMPL_SET(  CSpawnRegion, maxTime,     CSpawnRegion, toInt32,   SetMaxTime )
IMPL_SET(  CSpawnRegion, call,        CSpawnRegion, toInt32,   SetCall )

JSBool CGuildProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CGuild *gPriv = static_cast<CGuild *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_GUILD );

	if( propID != 0xFFFF )
	{
		JSString *tString = nullptr;
		switch( propID )
		{
			case CGP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->Name().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGP_ID:
			{
				GUILDID guildId = GuildSys->FindGuildId( gPriv );
				*vp = INT_TO_JSVAL( guildId );
				break;
			}
			case CGP_TYPE:			*vp = INT_TO_JSVAL( gPriv->Type() );		break;
			case CGP_MASTER:
				CChar *gMaster;
				gMaster = CalcCharObjFromSer( gPriv->Master() );

				if( !ValidateObject( gMaster ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, gMaster, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CGP_STONE:
				CItem *gStone;
				gStone = CalcItemObjFromSer( gPriv->Stone() );

				if( !ValidateObject( gStone ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, gStone, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myItem );
				}
				break;
			case CGP_NUMMEMBERS:	*vp = INT_TO_JSVAL( gPriv->NumMembers() );	break;
			case CGP_NUMRECRUITS:	*vp = INT_TO_JSVAL( gPriv->NumRecruits() );	break;
			case CGP_CHARTER:
				tString = JS_NewStringCopyZ( cx, gPriv->Charter().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGP_ABBREVIATION:
				tString = JS_NewStringCopyZ( cx, gPriv->Abbreviation().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGP_WEBPAGE:
				tString = JS_NewStringCopyZ( cx, gPriv->Webpage().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}
JSBool CGuildProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CGuild *gPriv = static_cast<CGuild *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_GUILD );

	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CGP_NAME:				gPriv->Name( encaps.toString() );						break;
			case CGP_ID:
				break;
			case CGP_TYPE:				gPriv->Type( static_cast<GuildType>( encaps.toInt() ));	break;
			case CGP_MASTER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar*>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->Master(( *myChar ));
				}
				else
				{
					gPriv->Master( INVALIDSERIAL );
				}
				break;
			case CGP_STONE:
				if( *vp != JSVAL_NULL )
				{
					CItem *myItem = static_cast<CItem *>( encaps.toObject() );
					if( !ValidateObject( myItem ))
						break;

					gPriv->Stone(( *myItem ));
				}
				else
				{
					gPriv->Stone( INVALIDSERIAL );
				}
				break;
			case CGP_CHARTER:			gPriv->Charter( encaps.toString() );				break;
			case CGP_ABBREVIATION:		gPriv->Abbreviation( encaps.toString().c_str() );	break;
			case CGP_WEBPAGE:			gPriv->Webpage( encaps.toString() );				break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

IMPL_GETS( CGuild, name,         CGuild, setString, Name().c_str() )
IMPL_GET(  CGuild, type,         CGuild, setInt32,  Type() )
IMPL_GET(  CGuild, numMembers,   CGuild, setInt32,  NumMembers() )
IMPL_GET(  CGuild, numRecruits,  CGuild, setInt32,  NumRecruits() )
IMPL_GETS( CGuild, charter,      CGuild, setString, Charter().c_str() )
IMPL_GETS( CGuild, abbreviation, CGuild, setString, Abbreviation().c_str() )
IMPL_GETS( CGuild, webPage,      CGuild, setString, Webpage().c_str() )

IMPL_SETS( CGuild, name,         CGuild, toString, Name )
IMPL_SETS( CGuild, charter,      CGuild, toString, Charter )
IMPL_SETS( CGuild, abbreviation, CGuild, toString, Abbreviation )
IMPL_SETS( CGuild, webPage,      CGuild, toString, Webpage )

FDCLG( CGuild, id )
{
	FNARGS
	auto guild = JS::GetMaybePtrFromReservedSlot<CGuild>( thisObj, 0 );
	args.rval().setInt32( GuildSys->FindGuildId( guild ));
	return true;
}

#define IMPL_GET_GUILD_OBJECT( attr, objectType, serialAccessor, objectTypeId, lookup ) \
FDCLG( CGuild, attr )                                                               \
{                                                                                   \
	FNARGS                                                                            \
	auto guild = JS::GetMaybePtrFromReservedSlot<CGuild>( thisObj, 0 );                \
	objectType *object = lookup( guild->serialAccessor() );                            \
	if( !ValidateObject( object ))                                                     \
	{                                                                                  \
		args.rval().setNull();                                                           \
		return true;                                                                     \
	}                                                                                  \
	JSObject *wrapper = JSEngine->AcquireObject( objectTypeId, object,                 \
		JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));                              \
	if( wrapper == nullptr )                                                           \
		return false;                                                                    \
	args.rval().setObject( *wrapper );                                                  \
	return true;                                                                        \
}

IMPL_GET_GUILD_OBJECT( master, CChar, Master, IUE_CHAR, CalcCharObjFromSer )
IMPL_GET_GUILD_OBJECT( stone,  CItem, Stone,  IUE_ITEM, CalcItemObjFromSer )

#undef IMPL_GET_GUILD_OBJECT

JSBool CRaceProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CRace *gPriv = static_cast<CRace *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_RACE );

	if( propID != 0xFFFF )
	{
		JSString *tString = nullptr;
		UI08 TempRace = 0;
		switch( propID )
		{
			case CRP_ID:
				*vp = INT_TO_JSVAL( gPriv->GetRaceID() );
				break;
			case CRP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->Name().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CRP_REQUIRESBEARD:		*vp = BOOLEAN_TO_JSVAL( gPriv->RequiresBeard() );		break;
			case CRP_REQUIRESNOBEARD:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoBeard() );				break;
			case CRP_ISPLAYERRACE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsPlayerRace() );		break;
			case CRP_GENDERRESTRICT:	*vp = INT_TO_JSVAL( gPriv->GenderRestriction() );		break;
			case CRP_ARMOURCLASS:		*vp = INT_TO_JSVAL( gPriv->ArmourClassRestriction() );	break;
			case CRP_LANGUAGESKILLMIN:	*vp = INT_TO_JSVAL( gPriv->LanguageMin() );				break;
			case CRP_POISONRESISTANCE:	JS_NewNumberValue( cx, gPriv->PoisonResistance(), vp ); break;
			case CRP_MAGICRESISTANCE:	JS_NewNumberValue( cx, gPriv->MagicResistance(), vp );  break;
			case CRP_VISIBLEDISTANCE:	*vp = INT_TO_JSVAL( gPriv->VisibilityRange() );			break;
			case CRP_NIGHTVISION:		*vp = INT_TO_JSVAL( gPriv->NightVision() );				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CRaceProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CRace *gPriv = static_cast<CRace *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_RACE );

	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CRP_NAME:				gPriv->Name( encaps.toString() );						break;
			case CRP_REQUIRESBEARD:		gPriv->RequiresBeard( encaps.toBool() );				break;
			case CRP_REQUIRESNOBEARD:	gPriv->NoBeard( encaps.toBool() );						break;
			case CRP_ISPLAYERRACE:		gPriv->IsPlayerRace( encaps.toBool() );					break;
			case CRP_GENDERRESTRICT:	gPriv->GenderRestriction( static_cast<GENDER>( encaps.toInt() ));		break;
			case CRP_ARMOURCLASS:		gPriv->ArmourClassRestriction( static_cast<UI08>( encaps.toInt() ));	break;
			case CRP_LANGUAGESKILLMIN:	gPriv->LanguageMin( static_cast<UI16>( encaps.toInt() ));				break;
			case CRP_POISONRESISTANCE:	gPriv->PoisonResistance( encaps.toFloat() );					break;
			case CRP_MAGICRESISTANCE:	gPriv->MagicResistance( encaps.toFloat() );						break;
			case CRP_VISIBLEDISTANCE:	gPriv->VisibilityRange( static_cast<SI08>( encaps.toInt() ));	break;
			case CRP_NIGHTVISION:		gPriv->NightVision( static_cast<UI08>( encaps.toInt() ));		break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

// clang-format off
IMPL_GET(  CRace, id,               CRace, setInt32,   GetRaceID() )
IMPL_GETS( CRace, name,             CRace, setString,  Name().c_str() )
IMPL_GET(  CRace, requiresBeard,    CRace, setBoolean, RequiresBeard() )
IMPL_GET(  CRace, requiresNoBeard,  CRace, setBoolean, NoBeard() )
IMPL_GET(  CRace, isPlayerRace,     CRace, setBoolean, IsPlayerRace() )
IMPL_GET(  CRace, genderRestrict,   CRace, setInt32,   GenderRestriction() )
IMPL_GET(  CRace, armourClass,      CRace, setInt32,   ArmourClassRestriction() )
IMPL_GET(  CRace, languageSkillMin, CRace, setInt32,   LanguageMin() )
IMPL_GET(  CRace, poisonResistance, CRace, setNumber,  PoisonResistance() )
IMPL_GET(  CRace, magicResistance,  CRace, setNumber,  MagicResistance() )
IMPL_GET(  CRace, visibleDistance,  CRace, setInt32,   VisibilityRange() )
IMPL_GET(  CRace, nightVision,      CRace, setInt32,   NightVision() )
IMPL_SETS( CRace, name,             CRace, toString,   Name )
IMPL_SET(  CRace, requiresBeard,    CRace, toBoolean,  RequiresBeard )
IMPL_SET(  CRace, requiresNoBeard,  CRace, toBoolean,  NoBeard )
IMPL_SET(  CRace, isPlayerRace,     CRace, toBoolean,  IsPlayerRace )
IMPL_SET(  CRace, poisonResistance, CRace, toNumber,   PoisonResistance )
IMPL_SET(  CRace, magicResistance,  CRace, toNumber,   MagicResistance )
// clang-format on

FDCLS( CRace, id )
{
	return true;
}

#define IMPL_RACE_INT_SET( attr, type, method )                         \
FDCLS( CRace, attr )                                                    \
{                                                                       \
	FNARGS                                                                \
	auto priv = JS::GetMaybePtrFromReservedSlot<CRace>( thisObj, 0 );     \
	if( priv == nullptr )                                                 \
		return false;                                                       \
	priv->method( static_cast<type>( args.get( 0 ).toInt32() ));          \
	return true;                                                          \
}

IMPL_RACE_INT_SET( genderRestrict,   GENDER,     GenderRestriction )
IMPL_RACE_INT_SET( armourClass,      ARMORCLASS, ArmourClassRestriction )
IMPL_RACE_INT_SET( languageSkillMin, SKILLVAL,   LanguageMin )
IMPL_RACE_INT_SET( visibleDistance,  RANGE,      VisibilityRange )
IMPL_RACE_INT_SET( nightVision,      LIGHTLEVEL, NightVision )

#undef IMPL_RACE_INT_SET

JSBool CSocketProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CSocket *gPriv = static_cast<CSocket *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_SOCK );

	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CSOCKP_ACCOUNT:
				break;
			case CSOCKP_CURRENTCHAR:
			{
				CChar *mChar = static_cast<CChar *>( encaps.toObject() );
				if( ValidateObject( mChar ))
				{
					gPriv->CurrcharObj( mChar );
				}
			}
				break;
			case CSOCKP_IDLETIMEOUT:
				break;
			case CSOCKP_WASIDLEWARNED:		gPriv->WasIdleWarned( encaps.toBool() );				break;
			case CSOCKP_TEMPINT:			gPriv->TempInt( encaps.toInt() );						break;
			case CSOCKP_TEMPINT2:			gPriv->TempInt2( encaps.toInt() );						break; // Reserved for JS usage
			case CSOCKP_TEMPOBJ:
				if( *vp == JSVAL_NULL )
				{
					gPriv->TempObj( nullptr );
				}
				else
				{
					gPriv->TempObj( static_cast<CBaseObject *>( encaps.toObject() ));
				}
				break;
			case CSOCKP_TEMPOBJ2: // Reserved for JS usage
				if( *vp == JSVAL_NULL )
				{
					gPriv->TempObj2( nullptr );
				}
				else
				{
					gPriv->TempObj2( static_cast<CBaseObject *>( encaps.toObject() ));
				}
				break;
			case CSOCKP_XTEXT:				gPriv->XText( encaps.toString() );						break;
			case CSOCKP_XTEXT2:				gPriv->XText2( encaps.toString() );						break;
			case CSOCKP_CLICKZ:				gPriv->ClickZ( static_cast<SI08>( encaps.toInt() ));	break;
			case CSOCKP_NEWCLIENT:			gPriv->NewClient( encaps.toBool() );					break;
			case CSOCKP_FIRSTPACKET:		gPriv->FirstPacket( encaps.toBool() );					break;
			case CSOCKP_CRYPTCLIENT:		gPriv->CryptClient( encaps.toBool() );					break;
			case CSOCKP_WALKSEQUENCE:		gPriv->WalkSequence( static_cast<SI16>( encaps.toInt() ));		break;
			case CSOCKP_CURRENTSPELLTYPE:	gPriv->CurrentSpellType( static_cast<SI08>( encaps.toInt() ));	break;
			case CSOCKP_LOGGING:			gPriv->Logging( encaps.toBool() );								break;
			case CSOCKP_BYTESSENT:
			case CSOCKP_BYTESRECEIVED:
				break;
			case CSOCKP_TARGETOK:			gPriv->TargetOK( encaps.toBool() );									break;
			case CSOCKP_CLICKX:				gPriv->ClickX( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_CLICKY:				gPriv->ClickY( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPX:			gPriv->PickupX( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPY:			gPriv->PickupY( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPZ:			gPriv->PickupZ( static_cast<SI08>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPSPOT:			gPriv->PickupSpot( static_cast<PickupLocations>( encaps.toInt() ));	break;
			case CSOCKP_PICKUPSERIAL:
				break;
			case CSOCKP_LANGUAGE:			gPriv->Language( static_cast<UnicodeTypes>( encaps.toInt() ));		break;
			case CSOCKP_CLIENTMAJORVER:		gPriv->ClientVersionMajor( static_cast<UI08>( encaps.toInt() ));	break;
			case CSOCKP_CLIENTMINORVER:		gPriv->ClientVersionMinor( static_cast<UI08>( encaps.toInt() ));	break;
			case CSOCKP_CLIENTSUBVER:		gPriv->ClientVersionSub( static_cast<UI08>( encaps.toInt() ));		break;
			case CSOCKP_CLIENTLETTERVER:	gPriv->ClientVersionLetter( static_cast<UI08>( encaps.toInt() ));	break;
			case CSOCKP_CLIENTTYPE:			gPriv->ClientType( static_cast<ClientTypes>( encaps.toInt() ));		break;
			case CSOCKP_MOVEDEBT:			gPriv->MovementDebt( static_cast<SI32>( encaps.toInt() ));			break;
			case CSOCKP_MOVEDEBTAVG:		gPriv->MovementDebtAverage( static_cast<SI32>( encaps.toInt() ));	break;
			case CSOCKP_MOVEBURSTALLOWANCE:	gPriv->MovementBurstAllowance( static_cast<UI08>( encaps.toInt() ));break;
			case CSOCKP_NEXTMOVETIME:
			{
				jsdouble newTime_double;
				JS_ValueToNumber( cx, *vp, &newTime_double );

				gPriv->NextMovementTime( static_cast<TIMERVAL>( newTime_double ));
				break;
			}
			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool CSocketProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CSocket *gPriv = static_cast<CSocket *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_SOCK );

	if( propID != 0xFFFF )
	{
		CChar *myChar;
		JSString *tString = nullptr;
		switch( propID )
		{
			case CSOCKP_ACCOUNT:
			{
				CAccountBlock_st *accountBlock = &gPriv->GetAccount();
				if( accountBlock == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{	// Otherwise Acquire an object
					JSObject *accountObj	= JSEngine->AcquireObject( IUE_ACCOUNT, accountBlock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( accountObj );
				}
				break;
			}
			case CSOCKP_CURRENTCHAR:
			{
				myChar = gPriv->CurrcharObj();
				if( !ValidateObject( myChar ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			}
			case CSOCKP_IDLETIMEOUT:
				break;
			case CSOCKP_WASIDLEWARNED:		*vp = BOOLEAN_TO_JSVAL( gPriv->WasIdleWarned() );		break;
			case CSOCKP_TEMPINT:			*vp = INT_TO_JSVAL( gPriv->TempInt() );					break;
			case CSOCKP_TEMPINT2:			*vp = INT_TO_JSVAL( gPriv->TempInt2() );				break;
			case CSOCKP_TEMPOBJ:
			{
				CBaseObject *mObj	= gPriv->TempObj();
				if( !ValidateObject( mObj ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myObj = nullptr;
					if( mObj->CanBeObjType( OT_ITEM ))
					{
						myObj = JSEngine->AcquireObject( IUE_ITEM, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					else
					{
						myObj = JSEngine->AcquireObject( IUE_CHAR, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			}
			case CSOCKP_TEMPOBJ2:
			{
				CBaseObject *mObj	= gPriv->TempObj2();
				if( !ValidateObject( mObj ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myObj = nullptr;
					if( mObj->CanBeObjType( OT_ITEM ))
					{
						myObj = JSEngine->AcquireObject( IUE_ITEM, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					else
					{
						myObj = JSEngine->AcquireObject( IUE_CHAR, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					*vp = OBJECT_TO_JSVAL( myObj );
				}
			}
				break;
			case CSOCKP_XTEXT:				
				tString = JS_NewStringCopyZ( cx, gPriv->XText().c_str() );
				*vp = STRING_TO_JSVAL( tString );	break;
			case CSOCKP_XTEXT2:				
				tString = JS_NewStringCopyZ( cx, gPriv->XText2().c_str() );
				*vp = STRING_TO_JSVAL( tString );	break;
			case CSOCKP_CLICKZ:				*vp = INT_TO_JSVAL( gPriv->ClickZ() );					break;
			case CSOCKP_NEWCLIENT:			*vp = BOOLEAN_TO_JSVAL( gPriv->NewClient() );			break;
			case CSOCKP_FIRSTPACKET:		*vp = BOOLEAN_TO_JSVAL( gPriv->FirstPacket() );			break;
			case CSOCKP_CRYPTCLIENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->CryptClient() );			break;
			case CSOCKP_WALKSEQUENCE:		*vp = INT_TO_JSVAL( gPriv->WalkSequence() );			break;
			case CSOCKP_CURRENTSPELLTYPE:	*vp = INT_TO_JSVAL( gPriv->CurrentSpellType() );		break;
			case CSOCKP_LOGGING:			*vp = BOOLEAN_TO_JSVAL( gPriv->Logging() );				break;
			case CSOCKP_BYTESSENT:			*vp = INT_TO_JSVAL( gPriv->BytesSent() );				break;
			case CSOCKP_BYTESRECEIVED:		*vp = INT_TO_JSVAL( gPriv->BytesReceived() );			break;
			case CSOCKP_TARGETOK:			*vp = BOOLEAN_TO_JSVAL( gPriv->TargetOK() );			break;
			case CSOCKP_CLICKX:				*vp = INT_TO_JSVAL( gPriv->ClickX() );					break;
			case CSOCKP_CLICKY:				*vp = INT_TO_JSVAL( gPriv->ClickY() );					break;
			case CSOCKP_PICKUPX:			*vp = INT_TO_JSVAL( gPriv->PickupX() );					break;
			case CSOCKP_PICKUPY:			*vp = INT_TO_JSVAL( gPriv->PickupY() );					break;
			case CSOCKP_PICKUPZ:			*vp = INT_TO_JSVAL( gPriv->PickupZ() );					break;
			case CSOCKP_PICKUPSPOT:			*vp = INT_TO_JSVAL( gPriv->PickupSpot() );				break;
			case CSOCKP_PICKUPSERIAL:
				break;
			case CSOCKP_LANGUAGE:			*vp = INT_TO_JSVAL( gPriv->Language() );				break;
			case CSOCKP_CLIENTMAJORVER:		*vp = INT_TO_JSVAL( gPriv->ClientVersionMajor() );		break;
			case CSOCKP_CLIENTMINORVER:		*vp = INT_TO_JSVAL( gPriv->ClientVersionMinor() );		break;
			case CSOCKP_CLIENTSUBVER:		*vp = INT_TO_JSVAL( gPriv->ClientVersionSub() );		break;
			case CSOCKP_CLIENTLETTERVER:	*vp = INT_TO_JSVAL( gPriv->ClientVersionLetter() );		break;
			case CSOCKP_CLIENTTYPE:			*vp = INT_TO_JSVAL( gPriv->ClientType() );				break;
			case CSOCKP_MOVEDEBT:			*vp = INT_TO_JSVAL( gPriv->MovementDebt() );			break;
			case CSOCKP_MOVEDEBTAVG:		*vp = INT_TO_JSVAL( gPriv->MovementDebtAverage() );		break;
			case CSOCKP_MOVEBURSTALLOWANCE: *vp = INT_TO_JSVAL( gPriv->MovementBurstAllowance() );	break;
			case CSOCKP_NEXTMOVETIME:		JS_NewNumberValue( cx, gPriv->NextMovementTime(), vp );	break;
			case CSOCKP_TARGET:
			{
				SERIAL mySerial		= gPriv->GetDWord( 7 );
				// Item
				if( mySerial >= BASEITEMSERIAL )
				{
					CItem *myItem = CalcItemObjFromSer( mySerial );

					if( !ValidateObject( myItem ))
					{
						*vp = JSVAL_NULL;
						return JS_TRUE;
					}

					JSObject *myObj = JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				// Char
				else
				{
					CChar *myChar = CalcCharObjFromSer( mySerial );

					if( !ValidateObject( myChar ))
					{
						*vp = JSVAL_NULL;
						return JS_TRUE;
					}

					JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}

				return JS_TRUE;
			}
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

#define IMPL_SOCKET_GET( attr, method, accessor ) IMPL_GET( CSocket, attr, CSocket, method, accessor )
#define IMPL_SOCKET_SET( attr, method, accessor ) IMPL_SET( CSocket, attr, CSocket, method, accessor )

IMPL_SOCKET_GET( wasIdleWarned,      setBoolean, WasIdleWarned() )
IMPL_SOCKET_GET( tempInt,            setInt32,   TempInt() )
IMPL_SOCKET_GET( tempInt2,           setInt32,   TempInt2() )
IMPL_GETS( CSocket, xText,  CSocket, setString, XText().c_str() )
IMPL_GETS( CSocket, xText2, CSocket, setString, XText2().c_str() )
IMPL_SOCKET_GET( clickZ,             setInt32,   ClickZ() )
IMPL_SOCKET_GET( newClient,          setBoolean, NewClient() )
IMPL_SOCKET_GET( firstPacket,        setBoolean, FirstPacket() )
IMPL_SOCKET_GET( cryptClient,        setBoolean, CryptClient() )
IMPL_SOCKET_GET( walkSequence,       setInt32,   WalkSequence() )
IMPL_SOCKET_GET( currentSpellType,   setInt32,   CurrentSpellType() )
IMPL_SOCKET_GET( logging,            setBoolean, Logging() )
IMPL_SOCKET_GET( bytesSent,          setInt32,   BytesSent() )
IMPL_SOCKET_GET( bytesReceived,      setInt32,   BytesReceived() )
IMPL_SOCKET_GET( targetOK,           setBoolean, TargetOK() )
IMPL_SOCKET_GET( clickX,             setInt32,   ClickX() )
IMPL_SOCKET_GET( clickY,             setInt32,   ClickY() )
IMPL_SOCKET_GET( pickupX,            setInt32,   PickupX() )
IMPL_SOCKET_GET( pickupY,            setInt32,   PickupY() )
IMPL_SOCKET_GET( pickupZ,            setInt32,   PickupZ() )
IMPL_SOCKET_GET( pickupSpot,         setInt32,   PickupSpot() )
IMPL_SOCKET_GET( language,           setInt32,   Language() )
IMPL_SOCKET_GET( clientMajorVer,     setInt32,   ClientVersionMajor() )
IMPL_SOCKET_GET( clientMinorVer,     setInt32,   ClientVersionMinor() )
IMPL_SOCKET_GET( clientSubVer,       setInt32,   ClientVersionSub() )
IMPL_SOCKET_GET( clientLetterVer,    setInt32,   ClientVersionLetter() )
IMPL_SOCKET_GET( clientType,         setInt32,   ClientType() )
IMPL_SOCKET_GET( nextMoveTime,       setNumber,  NextMovementTime() )
IMPL_SOCKET_GET( moveDebt,           setInt32,   MovementDebt() )
IMPL_SOCKET_GET( moveDebtAvg,        setInt32,   MovementDebtAverage() )
IMPL_SOCKET_GET( moveBurstAllowance, setInt32,   MovementBurstAllowance() )

IMPL_SOCKET_SET( wasIdleWarned,      toBoolean, WasIdleWarned )
IMPL_SOCKET_SET( tempInt,            toInt32,   TempInt )
IMPL_SOCKET_SET( tempInt2,           toInt32,   TempInt2 )
IMPL_SETS( CSocket, xText,  CSocket, toString, XText )
IMPL_SETS( CSocket, xText2, CSocket, toString, XText2 )
IMPL_SOCKET_SET( clickZ,             toInt32,   ClickZ )
IMPL_SOCKET_SET( newClient,          toBoolean, NewClient )
IMPL_SOCKET_SET( firstPacket,        toBoolean, FirstPacket )
IMPL_SOCKET_SET( cryptClient,        toBoolean, CryptClient )
IMPL_SOCKET_SET( walkSequence,       toInt32,   WalkSequence )
IMPL_SOCKET_SET( currentSpellType,   toInt32,   CurrentSpellType )
IMPL_SOCKET_SET( logging,            toBoolean, Logging )
IMPL_SOCKET_SET( targetOK,           toBoolean, TargetOK )
IMPL_SOCKET_SET( clickX,             toInt32,   ClickX )
IMPL_SOCKET_SET( clickY,             toInt32,   ClickY )
IMPL_SOCKET_SET( pickupX,            toInt32,   PickupX )
IMPL_SOCKET_SET( pickupY,            toInt32,   PickupY )
IMPL_SOCKET_SET( pickupZ,            toInt32,   PickupZ )
IMPL_SOCKET_SET( clientMajorVer,     toInt32,   ClientVersionMajor )
IMPL_SOCKET_SET( clientMinorVer,     toInt32,   ClientVersionMinor )
IMPL_SOCKET_SET( clientSubVer,       toInt32,   ClientVersionSub )
IMPL_SOCKET_SET( clientLetterVer,    toInt32,   ClientVersionLetter )
IMPL_SOCKET_SET( nextMoveTime,       toNumber,  NextMovementTime )
IMPL_SOCKET_SET( moveDebt,           toInt32,   MovementDebt )
IMPL_SOCKET_SET( moveDebtAvg,        toInt32,   MovementDebtAverage )
IMPL_SOCKET_SET( moveBurstAllowance, toInt32,   MovementBurstAllowance )

#undef IMPL_SOCKET_GET
#undef IMPL_SOCKET_SET

#define IMPL_SOCKET_LEGACY( attr, propertyId )                                      \
FDCLG( CSocket, attr )                                                              \
{                                                                                   \
	FNARGS                                                                            \
	JS::RootedValue result( cx );                                                     \
	if( !CSocketProps_getProperty( cx, thisObj, JS::PropertyKey::Int( propertyId ), result.address() )) \
		return false;                                                                   \
	args.rval().set( result );                                                        \
	return true;                                                                      \
}                                                                                   \
FDCLS( CSocket, attr )                                                              \
{                                                                                   \
	FNARGS                                                                            \
	JS::RootedValue value( cx, args.get( 0 ));                                        \
	return CSocketProps_setProperty( cx, thisObj, JS::PropertyKey::Int( propertyId ), false, value.address() ); \
}

IMPL_SOCKET_LEGACY( account,      CSOCKP_ACCOUNT )
IMPL_SOCKET_LEGACY( currentChar,  CSOCKP_CURRENTCHAR )
IMPL_SOCKET_LEGACY( idleTimeout,  CSOCKP_IDLETIMEOUT )
IMPL_SOCKET_LEGACY( pickupSerial, CSOCKP_PICKUPSERIAL )
IMPL_SOCKET_LEGACY( target,       CSOCKP_TARGET )
IMPL_SOCKET_LEGACY( tempObj,      CSOCKP_TEMPOBJ )
IMPL_SOCKET_LEGACY( tempObj2,     CSOCKP_TEMPOBJ2 )
#undef IMPL_SOCKET_LEGACY

FDCLS( CSocket, bytesSent ) { return true; }
FDCLS( CSocket, bytesReceived ) { return true; }

#define IMPL_SOCKET_ENUM_SET( attr, type, accessor )                    \
FDCLS( CSocket, attr )                                                  \
{                                                                       \
	FNARGS                                                                \
	auto socket = JS::GetMaybePtrFromReservedSlot<CSocket>( thisObj, 0 ); \
	socket->accessor( static_cast<type>( args.get( 0 ).toInt32() ));      \
	return true;                                                          \
}

IMPL_SOCKET_ENUM_SET( pickupSpot, PickupLocations, PickupSpot )
IMPL_SOCKET_ENUM_SET( language,   UnicodeTypes,    Language )
IMPL_SOCKET_ENUM_SET( clientType, ClientTypes,     ClientType )

#undef IMPL_SOCKET_ENUM_SET

JSBool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	JSEncapsulate myClass( cx, obj );
	CChar *myChar = static_cast<CChar*>( myClass.toObject() );

	if( !ValidateObject( myChar ))
		return JS_FALSE;

	UI08 skillId		= static_cast<UI08>( JSID_TO_INT( id ));

	if( myClass.ClassName() == "UOXSkills" )
	{
		*vp = INT_TO_JSVAL( myChar->GetSkill( skillId ));
	}
	else if( myClass.ClassName() == "UOXBaseSkills" )
	{
		*vp = INT_TO_JSVAL( myChar->GetBaseSkill( skillId ));
	}
	else if( myClass.ClassName() == "UOXSkillsUsed" )
	{
		*vp = BOOLEAN_TO_JSVAL( myChar->SkillUsed( skillId ));
	}
	else if( myClass.ClassName() == "UOXSkillsLock" )
	{
		*vp = INT_TO_JSVAL( static_cast<UI08>( myChar->GetSkillLock( skillId )));
	}
	else if( myClass.ClassName() == "UOXSkillsCap" )
	{
		*vp = INT_TO_JSVAL( myChar->GetSkillCap( skillId ));
	}

	return JS_TRUE;
}

JSBool CSkillsProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	JSEncapsulate myClass( cx, obj );
	CChar *myChar = static_cast<CChar*>( myClass.toObject() );

	if( !ValidateObject( myChar ))
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_GLOBALSKILLS );
	UI08 skillId		= static_cast<UI08>( propID );
	SI16 newSkillValue	= static_cast<SI16>( encaps.toInt() );
	UI08 i				= 0;

	if( myClass.ClassName() == "UOXSkills" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetSkill( newSkillValue, i );
			}
		}
		else
		{
			myChar->SetSkill( newSkillValue, skillId );
		}
	}
	else if( myClass.ClassName() == "UOXBaseSkills" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetBaseSkill( newSkillValue, i );
				Skills->UpdateSkillLevel( myChar, i );
			}
		}
		else
		{
			myChar->SetBaseSkill( newSkillValue, skillId );
			Skills->UpdateSkillLevel( myChar, skillId );
		}
	}
	else if( myClass.ClassName() == "UOXSkillsUsed" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SkillUsed( encaps.toBool(), i );
			}
		}
		else
		{
			myChar->SkillUsed( encaps.toBool(), skillId );
		}
	}
	else if( myClass.ClassName() == "UOXSkillsLock" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetSkillLock( static_cast<SkillLock>( newSkillValue ), i );
			}
		}
		else
		{
			myChar->SetSkillLock( static_cast<SkillLock>( newSkillValue ), skillId );
		}
	}
	else if( myClass.ClassName() == "UOXSkillsCap" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetSkillCap( newSkillValue, i );
			}
		}
		else
		{
			myChar->SetSkillCap( newSkillValue, skillId );
		}
	}

	if( !myChar->IsNpc() )
	{
		CSocket *toFind = myChar->GetSocket();
		if( toFind != nullptr )
		{
			if( skillId == ALLSKILLS )
			{
				for( i = 0; i < ALLSKILLS; ++i )
				{
					toFind->UpdateSkill( i );
				}
			}
			else
			{
				toFind->UpdateSkill( skillId );
			}
		}
	}

	return JS_TRUE;
}

static bool GetSkillValue( JSContext *cx, unsigned int argc, JS::Value *vp, UI08 skillId )
{
	FNARGS
	JSEncapsulate skillObject( cx, thisObj );
	CChar *character = static_cast<CChar *>( skillObject.toObject() );
	if( !ValidateObject( character ))
		return false;

	if( skillObject.ClassName() == "UOXSkills" )
		args.rval().setInt32( character->GetSkill( skillId ));
	else if( skillObject.ClassName() == "UOXBaseSkills" )
		args.rval().setInt32( character->GetBaseSkill( skillId ));
	else if( skillObject.ClassName() == "UOXSkillsUsed" )
		args.rval().setBoolean( character->SkillUsed( skillId ));
	else if( skillObject.ClassName() == "UOXSkillsLock" )
		args.rval().setInt32( static_cast<UI08>( character->GetSkillLock( skillId )));
	else if( skillObject.ClassName() == "UOXSkillsCap" )
		args.rval().setInt32( character->GetSkillCap( skillId ));
	else
		return false;
	return true;
}

static bool SetSkillValue( JSContext *cx, unsigned int argc, JS::Value *vp, UI08 skillId )
{
	FNARGS
	JSEncapsulate skillObject( cx, thisObj );
	CChar *character = static_cast<CChar *>( skillObject.toObject() );
	if( !ValidateObject( character ))
		return false;

	JS::RootedValue value( cx, args.get( 0 ));
	JSEncapsulate encaps( cx, value.address() );
	SI16 newValue = static_cast<SI16>( encaps.toInt() );
	UI08 firstSkill = skillId == ALLSKILLS ? 0 : skillId;
	UI08 lastSkill = skillId == ALLSKILLS ? ALLSKILLS : static_cast<UI08>( skillId + 1 );

	for( UI08 i = firstSkill; i < lastSkill; ++i )
	{
		if( skillObject.ClassName() == "UOXSkills" )
			character->SetSkill( newValue, i );
		else if( skillObject.ClassName() == "UOXBaseSkills" )
		{
			character->SetBaseSkill( newValue, i );
			Skills->UpdateSkillLevel( character, i );
		}
		else if( skillObject.ClassName() == "UOXSkillsUsed" )
			character->SkillUsed( encaps.toBool(), i );
		else if( skillObject.ClassName() == "UOXSkillsLock" )
			character->SetSkillLock( static_cast<SkillLock>( newValue ), i );
		else if( skillObject.ClassName() == "UOXSkillsCap" )
			character->SetSkillCap( newValue, i );
		else
			return false;
	}

	if( !character->IsNpc() && character->GetSocket() != nullptr )
	{
		for( UI08 i = firstSkill; i < lastSkill; ++i )
			character->GetSocket()->UpdateSkill( i );
	}
	return true;
}

#define IMPL_SKILL_PROPERTY( attr, skillId )                                      \
FDCLG( CSkills, attr ) { return GetSkillValue( cx, argc, vp, skillId ); }          \
FDCLS( CSkills, attr ) { return SetSkillValue( cx, argc, vp, skillId ); }

IMPL_SKILL_PROPERTY( alchemy,         ALCHEMY )
IMPL_SKILL_PROPERTY( anatomy,         ANATOMY )
IMPL_SKILL_PROPERTY( animallore,      ANIMALLORE )
IMPL_SKILL_PROPERTY( itemid,          ITEMID )
IMPL_SKILL_PROPERTY( armslore,        ARMSLORE )
IMPL_SKILL_PROPERTY( parrying,        PARRYING )
IMPL_SKILL_PROPERTY( begging,         BEGGING )
IMPL_SKILL_PROPERTY( blacksmithing,   BLACKSMITHING )
IMPL_SKILL_PROPERTY( bowcraft,        BOWCRAFT )
IMPL_SKILL_PROPERTY( peacemaking,     PEACEMAKING )
IMPL_SKILL_PROPERTY( camping,         CAMPING )
IMPL_SKILL_PROPERTY( carpentry,       CARPENTRY )
IMPL_SKILL_PROPERTY( cartography,     CARTOGRAPHY )
IMPL_SKILL_PROPERTY( cooking,         COOKING )
IMPL_SKILL_PROPERTY( detectinghidden, DETECTINGHIDDEN )
IMPL_SKILL_PROPERTY( enticement,      ENTICEMENT )
IMPL_SKILL_PROPERTY( evaluatingintel, EVALUATINGINTEL )
IMPL_SKILL_PROPERTY( healing,         HEALING )
IMPL_SKILL_PROPERTY( fishing,         FISHING )
IMPL_SKILL_PROPERTY( forensics,       FORENSICS )
IMPL_SKILL_PROPERTY( herding,         HERDING )
IMPL_SKILL_PROPERTY( hiding,          HIDING )
IMPL_SKILL_PROPERTY( provocation,     PROVOCATION )
IMPL_SKILL_PROPERTY( inscription,     INSCRIPTION )
IMPL_SKILL_PROPERTY( lockpicking,     LOCKPICKING )
IMPL_SKILL_PROPERTY( magery,          MAGERY )
IMPL_SKILL_PROPERTY( magicresistance, MAGICRESISTANCE )
IMPL_SKILL_PROPERTY( tactics,         TACTICS )
IMPL_SKILL_PROPERTY( snooping,        SNOOPING )
IMPL_SKILL_PROPERTY( musicianship,    MUSICIANSHIP )
IMPL_SKILL_PROPERTY( poisoning,       POISONING )
IMPL_SKILL_PROPERTY( archery,         ARCHERY )
IMPL_SKILL_PROPERTY( spiritspeak,     SPIRITSPEAK )
IMPL_SKILL_PROPERTY( stealing,        STEALING )
IMPL_SKILL_PROPERTY( tailoring,       TAILORING )
IMPL_SKILL_PROPERTY( taming,          TAMING )
IMPL_SKILL_PROPERTY( tasteid,         TASTEID )
IMPL_SKILL_PROPERTY( tinkering,       TINKERING )
IMPL_SKILL_PROPERTY( tracking,        TRACKING )
IMPL_SKILL_PROPERTY( veterinary,      VETERINARY )
IMPL_SKILL_PROPERTY( swordsmanship,   SWORDSMANSHIP )
IMPL_SKILL_PROPERTY( macefighting,    MACEFIGHTING )
IMPL_SKILL_PROPERTY( fencing,         FENCING )
IMPL_SKILL_PROPERTY( wrestling,       WRESTLING )
IMPL_SKILL_PROPERTY( lumberjacking,   LUMBERJACKING )
IMPL_SKILL_PROPERTY( mining,          MINING )
IMPL_SKILL_PROPERTY( meditation,      MEDITATION )
IMPL_SKILL_PROPERTY( stealth,         STEALTH )
IMPL_SKILL_PROPERTY( removetrap,      REMOVETRAP )
IMPL_SKILL_PROPERTY( necromancy,      NECROMANCY )
IMPL_SKILL_PROPERTY( focus,           FOCUS )
IMPL_SKILL_PROPERTY( chivalry,        CHIVALRY )
IMPL_SKILL_PROPERTY( bushido,         BUSHIDO )
IMPL_SKILL_PROPERTY( ninjitsu,        NINJITSU )
IMPL_SKILL_PROPERTY( spellweaving,    SPELLWEAVING )
IMPL_SKILL_PROPERTY( mysticism,       MYSTICISM )
IMPL_SKILL_PROPERTY( imbuing,         IMBUING )
IMPL_SKILL_PROPERTY( throwing,        THROWING )
IMPL_SKILL_PROPERTY( allskills,       ALLSKILLS )

#undef IMPL_SKILL_PROPERTY

FDCLG( CGumpData, buttons )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<SEGumpData_st>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	args.rval().setNumber( static_cast<double>( priv->nButtons.size() ));
	return true;
}

FDCLG( CGumpData, IDs )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<SEGumpData_st>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	args.rval().setNumber( static_cast<double>( priv->nIDs.size() ));
	return true;
}

JSBool CAccountProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	CAccountBlock_st *myAccount = static_cast<CAccountBlock_st *>( JS_GetPrivate( cx, obj ));
	if( myAccount == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_ACCOUNT );
	if( propID != 0xFFFF )
	{
		JSString *tString = nullptr;
		switch( propID )
		{
			case CACCOUNT_ID:	*vp = INT_TO_JSVAL( myAccount->wAccountIndex );		break;
			case CACCOUNT_USERNAME:
				tString = JS_NewStringCopyZ( cx, ( myAccount->sUsername ).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CACCOUNT_PASSWORD: // NO.
			case CACCOUNT_PATH:		// Nah.
				break;
			case CACCOUNT_FLAGS: *vp = INT_TO_JSVAL(( myAccount->wFlags ).to_ulong() );	break;
			case CACCOUNT_COMMENT:
				tString = JS_NewStringCopyZ( cx, ( myAccount->sContact ).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CACCOUNT_TIMEBAN: *vp = INT_TO_JSVAL( myAccount->wTimeBan );				break;
			case CACCOUNT_FIRSTLOGIN: *vp = INT_TO_JSVAL( myAccount->wFirstLogin );			break;
			case CACCOUNT_TOTALPLAYTIME: *vp = INT_TO_JSVAL( myAccount->wTotalPlayTime );	break;
			case CACCOUNT_CHARACTER1:
			{
				if( myAccount->dwCharacters[0] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[0];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER2:
			{
				if( myAccount->dwCharacters[1] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[1];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER3:
			{
				if( myAccount->dwCharacters[2] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[2];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER4:
			{
				if( myAccount->dwCharacters[3] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[3];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER5:
			{
				if( myAccount->dwCharacters[4] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[4];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER6:
			{
				if( myAccount->dwCharacters[5] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[5];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER7:
			{
				if( myAccount->dwCharacters[6] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[6];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CURRENTCHAR:
			{
				if( myAccount->dwInGame != INVALIDSERIAL )
				{
					CChar *TempObj = CalcCharObjFromSer( myAccount->dwInGame );

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_LASTIP:
			{
				std::string ipString = oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0xFF000000 ) >> 24 )) 
					+ "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x00FF0000 ) >> 16 ))
					+ "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x0000FF00 ) >> 8 ))
					+ "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x000000FF ) % 256 ));
				tString = JS_NewStringCopyZ( cx, ipString.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			}
			case CACCOUNT_BANNED:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_BANNED ));		break;
			case CACCOUNT_SUSPENDED:		*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_SUSPENDED ));	break;
			case CACCOUNT_PUBLIC:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_PUBLIC ));		break;
			case CACCOUNT_ONLINE:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_ONLINE ));		break;
			case CACCOUNT_CHARSLOT1BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER1 ));	break;
			case CACCOUNT_CHARSLOT2BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER2 ));	break;
			case CACCOUNT_CHARSLOT3BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER3 ));	break;
			case CACCOUNT_CHARSLOT4BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER4 ));	break;
			case CACCOUNT_CHARSLOT5BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER5 ));	break;
			case CACCOUNT_CHARSLOT6BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER6 ));	break;
			case CACCOUNT_CHARSLOT7BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER7 ));	break;
			case CACCOUNT_YOUNG:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_YOUNG ));		break;
			case CACCOUNT_UNUSED10:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_UNUSED10 ));	break;
			case CACCOUNT_SEER:				*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_SEER ));		break;
			case CACCOUNT_COUNSELOR:		*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_COUNSELOR ));	break;
			case CACCOUNT_GM:				*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_GM ));			break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool CAccountProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	CAccountBlock_st *myAccount = static_cast<CAccountBlock_st *>( JS_GetPrivate( cx, obj ));
	if( myAccount == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );

	UI16 propID = getScriptID( cx, id, JSP_ACCOUNT );
	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CACCOUNT_ID: 
			case CACCOUNT_USERNAME:
			case CACCOUNT_FLAGS:
			case CACCOUNT_PATH:
			case CACCOUNT_CHARACTER1:
			case CACCOUNT_CHARACTER2:
			case CACCOUNT_CHARACTER3:
			case CACCOUNT_CHARACTER4:
			case CACCOUNT_CHARACTER5:
			case CACCOUNT_CHARACTER6:
			case CACCOUNT_CHARACTER7:
			case CACCOUNT_CURRENTCHAR:
			case CACCOUNT_LASTIP:
			case CACCOUNT_FIRSTLOGIN:
				break;
			case CACCOUNT_TOTALPLAYTIME:		myAccount->wTotalPlayTime = static_cast<UI32>( encaps.toInt() );	break;
			case CACCOUNT_PASSWORD:
			{
				std::string newPass = encaps.toString();
				if( newPass.length() > 3 )
				{
					myAccount->sPassword = newPass;
				}
				else
				{
					return JS_FALSE;
				}
				break;
			}
			case CACCOUNT_COMMENT: 
				myAccount->sContact = encaps.toString();
				break;
			case CACCOUNT_TIMEBAN:
			{
				UI32 timeBan = static_cast<UI32>( encaps.toInt() );
				if( timeBan > 0 )
				{
					myAccount->wFlags.set( AB_FLAGS_BANNED, true );
					myAccount->wTimeBan = GetMinutesSinceEpoch() + timeBan;
				}
				else
				{
					myAccount->wTimeBan = 0;
				}
				break;
			}
			case CACCOUNT_BANNED:			myAccount->wFlags.set( AB_FLAGS_BANNED, encaps.toBool() );		break;
			case CACCOUNT_SUSPENDED:		myAccount->wFlags.set( AB_FLAGS_SUSPENDED, encaps.toBool() );	break;
			case CACCOUNT_PUBLIC:			myAccount->wFlags.set( AB_FLAGS_PUBLIC, encaps.toBool() );		break;
			case CACCOUNT_CHARSLOT1BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER1, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT2BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER2, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT3BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER3, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT4BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER4, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT5BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER5, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT6BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER6, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT7BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER7, encaps.toBool() );	break;
			case CACCOUNT_YOUNG:			myAccount->wFlags.set( AB_FLAGS_YOUNG, encaps.toBool() );		break;
			case CACCOUNT_UNUSED10:			myAccount->wFlags.set( AB_FLAGS_UNUSED10, encaps.toBool() );	break;
			case CACCOUNT_SEER:				myAccount->wFlags.set( AB_FLAGS_SEER, encaps.toBool() );		break;
			case CACCOUNT_COUNSELOR:		myAccount->wFlags.set( AB_FLAGS_COUNSELOR, encaps.toBool() );	break;
			case CACCOUNT_GM:				myAccount->wFlags.set( AB_FLAGS_GM, encaps.toBool() );			break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

IMPL_GET(  CAccount, id,            CAccountBlock_st, setInt32,  wAccountIndex )
IMPL_GETS( CAccount, username,      CAccountBlock_st, setString, sUsername.c_str() )
IMPL_GET(  CAccount, flags,         CAccountBlock_st, setInt32,  wFlags.to_ulong() )
IMPL_GETS( CAccount, comment,       CAccountBlock_st, setString, sContact.c_str() )
IMPL_GET(  CAccount, timeban,       CAccountBlock_st, setInt32,  wTimeBan )
IMPL_GET(  CAccount, firstLogin,    CAccountBlock_st, setInt32,  wFirstLogin )
IMPL_GET(  CAccount, totalPlayTime, CAccountBlock_st, setInt32,  wTotalPlayTime )

IMPL_SETS(   CAccount, comment,       CAccountBlock_st, toString, sContact.assign )
IMPL_SET_DIR( CAccount, totalPlayTime, CAccountBlock_st, toInt32,  wTotalPlayTime )

#define IMPL_ACCOUNT_UNDEFINED( attr ) FDCLG( CAccount, attr ) { FNARGS args.rval().setUndefined(); return true; }
#define IMPL_ACCOUNT_NOOP_SET( attr ) FDCLS( CAccount, attr ) { return true; }

IMPL_ACCOUNT_UNDEFINED( password )
IMPL_ACCOUNT_UNDEFINED( path )
IMPL_ACCOUNT_NOOP_SET( flags )
IMPL_ACCOUNT_NOOP_SET( path )
IMPL_ACCOUNT_NOOP_SET( firstLogin )
IMPL_ACCOUNT_NOOP_SET( isOnline )

#undef IMPL_ACCOUNT_UNDEFINED
#undef IMPL_ACCOUNT_NOOP_SET

FDCLS( CAccount, password )
{
	FNARGS
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 );
	JS::RootedString passwordValue( cx, JS::ToString( cx, args.get( 0 )));
	if( passwordValue == nullptr )
		return false;
	const std::string password = convertToString( cx, passwordValue );
	if( password.length() <= 3 )
		return false;
	account->sPassword = password;
	return true;
}

FDCLS( CAccount, timeban )
{
	FNARGS
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 );
	const UI32 minutes = static_cast<UI32>( args.get( 0 ).toInt32() );
	account->wTimeBan = minutes > 0 ? GetMinutesSinceEpoch() + minutes : 0;
	if( minutes > 0 )
		account->wFlags.set( AB_FLAGS_BANNED, true );
	return true;
}

#define IMPL_ACCOUNT_FLAG( attr, flag )                                      \
FDCLG( CAccount, attr )                                                      \
{                                                                            \
	FNARGS                                                                     \
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 ); \
	args.rval().setBoolean( account->wFlags.test( flag ));                     \
	return true;                                                               \
}                                                                            \
FDCLS( CAccount, attr )                                                      \
{                                                                            \
	FNARGS                                                                     \
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 ); \
	account->wFlags.set( flag, args.get( 0 ).toBoolean() );                    \
	return true;                                                               \
}

IMPL_ACCOUNT_FLAG( isBanned,       AB_FLAGS_BANNED )
IMPL_ACCOUNT_FLAG( isSuspended,    AB_FLAGS_SUSPENDED )
IMPL_ACCOUNT_FLAG( isPublic,       AB_FLAGS_PUBLIC )
IMPL_ACCOUNT_FLAG( isSlot1Blocked, AB_FLAGS_CHARACTER1 )
IMPL_ACCOUNT_FLAG( isSlot2Blocked, AB_FLAGS_CHARACTER2 )
IMPL_ACCOUNT_FLAG( isSlot3Blocked, AB_FLAGS_CHARACTER3 )
IMPL_ACCOUNT_FLAG( isSlot4Blocked, AB_FLAGS_CHARACTER4 )
IMPL_ACCOUNT_FLAG( isSlot5Blocked, AB_FLAGS_CHARACTER5 )
IMPL_ACCOUNT_FLAG( isSlot6Blocked, AB_FLAGS_CHARACTER6 )
IMPL_ACCOUNT_FLAG( isSlot7Blocked, AB_FLAGS_CHARACTER7 )
IMPL_ACCOUNT_FLAG( isYoung,        AB_FLAGS_YOUNG )
IMPL_ACCOUNT_FLAG( unused10,       AB_FLAGS_UNUSED10 )
IMPL_ACCOUNT_FLAG( isSeer,         AB_FLAGS_SEER )
IMPL_ACCOUNT_FLAG( isCounselor,    AB_FLAGS_COUNSELOR )
IMPL_ACCOUNT_FLAG( isGM,           AB_FLAGS_GM )

#undef IMPL_ACCOUNT_FLAG

FDCLG( CAccount, isOnline )
{
	FNARGS
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 );
	args.rval().setBoolean( account->wFlags.test( AB_FLAGS_ONLINE ));
	return true;
}

#define IMPL_ACCOUNT_CHARACTER( attr, index )                               \
FDCLG( CAccount, attr )                                                     \
{                                                                           \
	FNARGS                                                                    \
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 ); \
	CBaseObject *character = account->dwCharacters[index] == INVALIDSERIAL ? nullptr : account->lpCharacters[index]; \
	if( !ValidateObject( character ))                                         \
	{                                                                         \
		args.rval().setNull();                                                  \
		return true;                                                            \
	}                                                                         \
	JSObject *wrapper = JSEngine->AcquireObject( IUE_CHAR, character, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ))); \
	if( wrapper == nullptr ) return false;                                    \
	args.rval().setObject( *wrapper );                                         \
	return true;                                                               \
}

IMPL_ACCOUNT_CHARACTER( character1, 0 )
IMPL_ACCOUNT_CHARACTER( character2, 1 )
IMPL_ACCOUNT_CHARACTER( character3, 2 )
IMPL_ACCOUNT_CHARACTER( character4, 3 )
IMPL_ACCOUNT_CHARACTER( character5, 4 )
IMPL_ACCOUNT_CHARACTER( character6, 5 )
IMPL_ACCOUNT_CHARACTER( character7, 6 )

#undef IMPL_ACCOUNT_CHARACTER

FDCLG( CAccount, currentChar )
{
	FNARGS
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 );
	CChar *character = account->dwInGame == INVALIDSERIAL ? nullptr : CalcCharObjFromSer( account->dwInGame );
	if( !ValidateObject( character )) { args.rval().setNull(); return true; }
	JSObject *wrapper = JSEngine->AcquireObject( IUE_CHAR, character, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( wrapper == nullptr ) return false;
	args.rval().setObject( *wrapper );
	return true;
}

FDCLG( CAccount, lastIP )
{
	FNARGS
	auto account = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st>( thisObj, 0 );
	const UI32 ip = account->dwLastIP;
	const std::string text = oldstrutil::format( "%u.%u.%u.%u", ( ip >> 24 ) & 0xFF, ( ip >> 16 ) & 0xFF, ( ip >> 8 ) & 0xFF, ip & 0xFF );
	JSString *value = JS_NewStringCopyZ( cx, text.c_str() );
	if( value == nullptr ) return false;
	args.rval().setString( value );
	return true;
}

// clang-format off
IMPL_GET_NP( CConsole, mode,    setInt32,   Console.CurrentMode() )
IMPL_GET_NP( CConsole, logEcho, setBoolean, Console.LogEcho() )
IMPL_SET_NP( CConsole, mode,    toInt32,    Console.CurrentMode )
IMPL_SET_NP( CConsole, logEcho, toBoolean,  Console.LogEcho )
// clang-format on

// clang-format off
IMPL_GET( CScriptSection, numTags,   CScriptSection, setInt32,   NumEntries() )
IMPL_GET( CScriptSection, atEnd,     CScriptSection, setBoolean, AtEnd() )
IMPL_GET( CScriptSection, atEndTags, CScriptSection, setBoolean, AtEndTags() )
// clang-format on

JSBool CResourceProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	MapResource_st *gPriv = static_cast<MapResource_st*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_RESOURCE );
	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CRESP_LOGAMT:				gPriv->logAmt	= encaps.toInt();			break;
			case CRESP_LOGTIME:
			{
				gPriv->logTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
				break;
			}
			case CRESP_OREAMT:				gPriv->oreAmt	= encaps.toInt();			break;
			case CRESP_ORETIME:				//gPriv->oreTime	= encaps.toInt();			break;
			{
				gPriv->oreTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
				break;
			}
			case CRESP_FISHAMT:				gPriv->fishAmt	= encaps.toInt();			break;
			case CRESP_FISHTIME:			//gPriv->fishTime	= encaps.toInt();			break;
			{
				gPriv->fishTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
				break;
			}
			default:																	break;
		}
	}

	return JS_TRUE;
}

JSBool CResourceProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	MapResource_st *gPriv = static_cast<MapResource_st*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_RESOURCE );
	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CRESP_LOGAMT:				*vp = INT_TO_JSVAL( gPriv->logAmt );			break;
			case CRESP_LOGTIME:				JS_NewNumberValue( cx, gPriv->logTime, vp );	break;
			case CRESP_OREAMT:				*vp = INT_TO_JSVAL( gPriv->oreAmt );			break;
			case CRESP_ORETIME:				JS_NewNumberValue( cx, gPriv->oreTime, vp );	break;
			case CRESP_FISHAMT:				*vp = INT_TO_JSVAL( gPriv->fishAmt );			break;
			case CRESP_FISHTIME:			JS_NewNumberValue( cx, gPriv->fishTime, vp );	break;
			default:																		break;
		}
	}
	return JS_TRUE;
}

// clang-format off
IMPL_GET(     CResource, logAmount,  MapResource_st, setInt32,  logAmt )
IMPL_GET(     CResource, logTime,    MapResource_st, setNumber, logTime )
IMPL_GET(     CResource, oreAmount,  MapResource_st, setInt32,  oreAmt )
IMPL_GET(     CResource, oreTime,    MapResource_st, setNumber, oreTime )
IMPL_GET(     CResource, fishAmount, MapResource_st, setInt32,  fishAmt )
IMPL_GET(     CResource, fishTime,   MapResource_st, setNumber, fishTime )
IMPL_SET_DIR( CResource, logAmount,  MapResource_st, toInt32,   logAmt )
IMPL_SET_DIR( CResource, oreAmount,  MapResource_st, toInt32,   oreAmt )
IMPL_SET_DIR( CResource, fishAmount, MapResource_st, toInt32,   fishAmt )
// clang-format on

#define IMPL_RESOURCE_TIME_SET( attr, field )                                \
FDCLS( CResource, attr )                                                     \
{                                                                            \
	FNARGS                                                                     \
	auto priv = JS::GetMaybePtrFromReservedSlot<MapResource_st>( thisObj, 0 ); \
	if( priv == nullptr )                                                      \
		return false;                                                            \
	priv->field = static_cast<TIMERVAL>( args.get( 0 ).toInt32() * 1000 );     \
	return true;                                                               \
}

IMPL_RESOURCE_TIME_SET( logTime, logTime )
IMPL_RESOURCE_TIME_SET( oreTime, oreTime )
IMPL_RESOURCE_TIME_SET( fishTime, fishTime )

#undef IMPL_RESOURCE_TIME_SET

JSBool CPartyProps_setProperty( JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp )
{
	Party *gPriv = static_cast<Party *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	JSEncapsulate encaps( cx, vp );
	UI16 propID = getScriptID( cx, id, JSP_PARTY );
	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CPARTYP_LEADER:
			{
				if( encaps.ClassName() == "UOXChar" || encaps.ClassName() == "UOXSocket" )
				{
					CChar *newLeader = nullptr;
					if( encaps.ClassName() == "UOXChar" )
					{
						newLeader = static_cast<CChar *>( encaps.toObject() );
					}
					else
					{
						CSocket *tempSock = static_cast<CSocket *>( encaps.toObject() );
						if( tempSock != nullptr )
						{
							newLeader = tempSock->CurrcharObj();
						}
					}
					if( ValidateObject( newLeader ))
					{
						gPriv->Leader( newLeader );
					}
				}
			}
				break;
			case CPARTYP_ISNPC:			gPriv->IsNPC( encaps.toBool() );				break;
			default:																	break;
		}
	}

	return JS_TRUE;
}

JSBool CPartyProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp )
{
	Party *gPriv = static_cast<Party *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	UI16 propID = getScriptID( cx, id, JSP_PARTY );
	if( propID != 0xFFFF )
	{
		switch( propID )
		{
			case CPARTYP_LEADER:
			{
				CChar *myChar = gPriv->Leader();
				if( !ValidateObject( myChar ))
				{
					*vp = JSVAL_NULL;
					return JS_TRUE;
				}
				JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
				*vp = OBJECT_TO_JSVAL( myObj );
			}
				break;
			case CPARTYP_MEMBERCOUNT:	*vp = INT_TO_JSVAL( gPriv->MemberList()->size() );			break;
			case CPARTYP_ISNPC:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsNPC() );					break;
			default:																				break;
		}
	}
	return JS_TRUE;
}

FDCLG( CParty, leader )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<Party>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	CChar *leader = priv->Leader();
	if( !ValidateObject( leader ))
	{
		args.rval().setNull();
		return true;
	}

	JSObject *leaderObject = JSEngine->AcquireObject(
		IUE_CHAR, leader, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	if( leaderObject == nullptr )
		return false;

	args.rval().setObject( *leaderObject );
	return true;
}

FDCLS( CParty, leader )
{
	FNARGS
	auto priv = JS::GetMaybePtrFromReservedSlot<Party>( thisObj, 0 );
	if( priv == nullptr )
		return false;

	JS::RootedValue value( cx, args.get( 0 ));
	JSEncapsulate encaps( cx, value.address() );
	CChar *newLeader = nullptr;
	if( encaps.ClassName() == "UOXChar" )
	{
		newLeader = static_cast<CChar *>( encaps.toObject() );
	}
	else if( encaps.ClassName() == "UOXSocket" )
	{
		CSocket *socket = static_cast<CSocket *>( encaps.toObject() );
		if( socket != nullptr )
			newLeader = socket->CurrcharObj();
	}

	if( ValidateObject( newLeader ))
		priv->Leader( newLeader );
	return true;
}

// clang-format off
IMPL_GET( CParty, memberCount, Party, setInt32,   MemberList()->size() )
IMPL_GET( CParty, isNPC,       Party, setBoolean, IsNPC() )
IMPL_SET( CParty, isNPC,       Party, toBoolean,  IsNPC )
// clang-format on

IMPL_GET( CScript, script_id, cScript, setInt32, GetScriptID() )
