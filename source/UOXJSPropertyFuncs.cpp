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

#include <js/Object.h>
#include <js/Array.h>
#include "js/RootingAPI.h" // For JS::RootedValue
#include "js/Value.h"      // For JS::HandleValue, JS::MutableHandleValue
#include "js/CharacterEncoding.h" // For JS::UTF8Chars
#include "js/Utility.h"    // For JS::GetScript, JS::GetOrCreateScriptID
#include "jsapi.h"

#define INT_FITS_IN_JSVAL(i) ((i) >= INT32_MIN && (i) <= INT32_MAX)

void MakeShop( CChar *c );
void ScriptError( JSContext *cx, const char *txt, ... );

/*
bool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  size_t spellId = JSVAL_TO_INT( id );

  if( spellId >= Magic->spells.size() )
  {
    ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell ID (%i) provided", spellId ).c_str() );
    *vp = JS::CurrentGlobalOrNull;
    return false;
  }

  CSpellInfo *mySpell = &Magic->spells[spellId];
  if( mySpell == nullptr )
  {
    ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell with spellId %i", spellId ).c_str() );
    *vp = JS::CurrentGlobalOrNull;
    return false;
  }

  JSObject *jsSpell = JS_NewObject( cx, &UOXSpell_class, nullptr, obj );
  JS_DefineProperties( cx, jsSpell, CSpellProperties );
  JS::SetReservedSlot( jsSpell, 0, JS::PrivateValue(mySpell) );

  *vp = OBJECT_TO_JSVAL( jsSpell );
  return true;
}
*/

bool JSCSpell_get_id(JSContext *cx, unsigned int argc, JS::Value *vp) {
    auto args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject thisObj(cx);
    if( !args.computeThis( cx, &thisObj ) )
        return false;
    auto priv = JS::GetMaybePtrFromReservedSlot<CSpellInfo>(thisObj, 0);
    for (auto i = 0; i < Magic->spells.size(); ++i) {
        if (&Magic->spells[i] == priv) {
            args.rval().setInt32(i);
            return true;
        }
    }
    return false;
}

bool JSCSpell_get_name(JSContext *cx, unsigned int argc, JS::Value *vp) {
    auto args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject thisObj(cx);
    if (!args.computeThis(cx, &thisObj))
        return false;
    auto priv = JS::GetMaybePtrFromReservedSlot<CSpellInfo>(thisObj, 0);
    for (auto i = 0; i < Magic->spells.size(); ++i) {
        if (&Magic->spells[i] == priv) {
            auto spellName = Dictionary->GetEntry( magic_table[i].spell_name );
            auto tString = JS_NewStringCopyZ( cx, spellName.c_str() );
            args.rval().setString(tString);
            return true;
        }
    }
    return false;
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
IMPL_GET( CSpell, soundEffect,     CSpellInfo, setInt32, Effect() )
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


/*
bool CGlobalSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  size_t skillId = JSVAL_TO_INT( id );

  if( skillId > THROWING )
  {
    ScriptError( cx, oldstrutil::format( "Invalid Skill ID, must be between 0 and 57" ).c_str() );
    *vp = JS::CurrentGlobalOrNull;
    return false;
  }

  CWorldMain::Skill_st *mySkill = &cwmWorldState->skill[skillId];
  if( mySkill == nullptr )
  {
    ScriptError( cx, oldstrutil::format( "Invalid Skill" ).c_str() );
    *vp = JS::CurrentGlobalOrNull;
    return false;
  }

  JSObject *jsSkill = JS_NewObject( cx, &UOXGlobalSkill_class, nullptr, obj );
  JS_DefineProperties( cx, jsSkill, CGlobalSkillProperties );
  JS::SetReservedSlot( jsSkill, 0, JS::PrivateValue(mySkill) );

  *vp = OBJECT_TO_JSVAL( jsSkill );
  return true;
}
*/

// clang-format off
IMPL_GETS( CGlobalSkill, name,         CWorldMain::Skill_st, setString, name.c_str() )
IMPL_GETS( CGlobalSkill, madeWord,     CWorldMain::Skill_st, setString, madeWord.c_str() )
IMPL_GET(  CGlobalSkill, strength,     CWorldMain::Skill_st, setInt32,  strength )
IMPL_GET(  CGlobalSkill, dexterity,    CWorldMain::Skill_st, setInt32,  dexterity )
IMPL_GET(  CGlobalSkill, intelligence, CWorldMain::Skill_st, setInt32,  intelligence )
IMPL_GET(  CGlobalSkill, skillDelay,   CWorldMain::Skill_st, setInt32,  skillDelay )
IMPL_GET(  CGlobalSkill, scriptID,     CWorldMain::Skill_st, setInt32,  jsScript )

// NP is No Private data
IMPL_GET_NP( CTimer, TIMEOUT,            setInt32, tCHAR_TIMEOUT )
IMPL_GET_NP( CTimer, INVIS,              setInt32, tCHAR_INVIS )
IMPL_GET_NP( CTimer, HUNGER,             setInt32, tCHAR_HUNGER )
IMPL_GET_NP( CTimer, THIRST,             setInt32, tCHAR_THIRST )
IMPL_GET_NP( CTimer, POISONTIME,         setInt32, tCHAR_POISONTIME )
IMPL_GET_NP( CTimer, POISONTEXT,         setInt32, tCHAR_POISONTEXT )
IMPL_GET_NP( CTimer, POISONWEAROFF,      setInt32, tCHAR_POISONWEAROFF )
IMPL_GET_NP( CTimer, SPELLTIME,          setInt32, tCHAR_SPELLTIME )
IMPL_GET_NP( CTimer, SPELLRECOVERYTIME,  setInt32, tCHAR_SPELLRECOVERYTIME )
IMPL_GET_NP( CTimer, CRIMFLAG,           setInt32, tCHAR_CRIMFLAG )
IMPL_GET_NP( CTimer, ANTISPAM,           setInt32, tCHAR_ANTISPAM )
IMPL_GET_NP( CTimer, MURDERRATE,         setInt32, tCHAR_MURDERRATE )
IMPL_GET_NP( CTimer, PEACETIMER,         setInt32, tCHAR_PEACETIMER )
IMPL_GET_NP( CTimer, FLYINGTOGGLE,       setInt32, tCHAR_FLYINGTOGGLE )
IMPL_GET_NP( CTimer, FIREFIELDTICK,      setInt32, tCHAR_FIREFIELDTICK )
IMPL_GET_NP( CTimer, POISONFIELDTICK,    setInt32, tCHAR_POISONFIELDTICK )
IMPL_GET_NP( CTimer, PARAFIELDTICK,      setInt32, tCHAR_PARAFIELDTICK )
IMPL_GET_NP( CTimer, MOVETIME,           setInt32, tNPC_MOVETIME )
IMPL_GET_NP( CTimer, SPATIMER,           setInt32, tNPC_SPATIMER )
IMPL_GET_NP( CTimer, SUMMONTIME,         setInt32, tNPC_SUMMONTIME )
IMPL_GET_NP( CTimer, EVADETIME,          setInt32, tNPC_EVADETIME )
IMPL_GET_NP( CTimer, LOYALTYTIME,        setInt32, tNPC_LOYALTYTIME )
IMPL_GET_NP( CTimer, IDLEANIMTIME,       setInt32, tNPC_IDLEANIMTIME )
IMPL_GET_NP( CTimer, LOGOUT,             setInt32, tPC_LOGOUT )
IMPL_GET_NP( CTimer, YOUNGHEAL,          setInt32, tCHAR_YOUNGHEAL )
IMPL_GET_NP( CTimer, YOUNGMESSAGE,       setInt32, tCHAR_YOUNGMESSAGE )

// Socket Timers
IMPL_GET_NP( CTimer, SOCK_SKILLDELAY,      setInt32, tPC_SKILLDELAY )
IMPL_GET_NP( CTimer, SOCK_OBJDELAY,        setInt32, tPC_OBJDELAY )
IMPL_GET_NP( CTimer, SOCK_SPIRITSPEAK,     setInt32, tPC_SPIRITSPEAK )
IMPL_GET_NP( CTimer, SOCK_TRACKING,        setInt32, tPC_TRACKING )
IMPL_GET_NP( CTimer, SOCK_FISHING,         setInt32, tPC_FISHING )
IMPL_GET_NP( CTimer, SOCK_MUTETIME,        setInt32, tPC_MUTETIME )
IMPL_GET_NP( CTimer, SOCK_TRACKINGDISPLAY, setInt32, tPC_TRACKINGDISPLAY )
IMPL_GET_NP( CTimer, SOCK_TRAFFICWARDEN,   setInt32, tPC_TRAFFICWARDEN )
// clang-format on

/*
bool CCreateEntriesProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  UI16 createEntryId = static_cast<UI16>( JSVAL_TO_INT( id ));

  CreateEntry_st *myCreateEntry = Skills->FindItem( createEntryId );
  if( myCreateEntry == nullptr )
  {
    ScriptError( cx, oldstrutil::format( "Invalid create entry ID (%i)", createEntryId ).c_str() );
    *vp = JS::CurrentGlobalOrNull;
    return false;
  }

  JSObject *jsCreateEntry = JS_NewObject( cx, &UOXCreateEntry_class, nullptr, obj );
  JS_DefineProperties( cx, jsCreateEntry, CCreateEntryProperties );
  JS::SetReservedSlot( jsCreateEntry, 0, JS::PrivateValue(myCreateEntry) );

  *vp = OBJECT_TO_JSVAL( jsCreateEntry );
  return true;
}
*/

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

/*
bool CCreateEntryProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CreateEntry_st *gPriv = JS::GetMaybePtrFromReservedSlot<CreateEntry_st>(obj , 0);
  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
    {
    case CMAKE_RESOURCES:
    {
      ResAmountPair_st resEntry;
      std::vector<ResAmountPair_st> resourcesNeeded = gPriv->resourceNeeded;

      // Loop through each resource required, and add them to a JSObject
      jsval amountNeeded = 0;
      jsval targColour = 0;
      JSObject *resources = JS_NewArrayObject( cx, 0, nullptr );
      for( auto i = 0; i < static_cast<int>( resourcesNeeded.size() ); i++ )
      {
        resEntry = resourcesNeeded[i];
        amountNeeded = JS::Int32Value( resEntry.amountNeeded );
        targColour = JS::Int32Value( resEntry.colour );

        // Add each required resource as its own object
        JSObject *resource = JS_NewArrayObject( cx, 0, nullptr );
        JS_SetElement( cx, resource, 0, &amountNeeded );
        JS_SetElement( cx, resource, 1, &targColour );

        // Make a child object with all the supported resource IDs
        JSObject *idList = JS_NewArrayObject( cx, 0, nullptr );
        for( auto j = 0; j < static_cast<int>( resEntry.idList.size() ); j++ )
        {
          jsval targId = JS::Int32Value( resEntry.idList[j] );
          JS_SetElement( cx, idList, j, &targId );
        }
        jsval idListVal = OBJECT_TO_JSVAL( idList );
        JS_SetElement( cx, resource, 2, &idListVal );

        // Now stuff the resource object into the parent object
        jsval resourceVal = OBJECT_TO_JSVAL( resource );
        JS_SetElement( cx, resources, i, &resourceVal );
      }

      *vp = OBJECT_TO_JSVAL( resources );
      break;
    }
    case CMAKE_SKILLS:
    {
      ResSkillReq_st resEntry;
      std::vector<ResSkillReq_st> skillReqs = gPriv->skillReqs;

      // Loop through each skill required, and add the details to a JSObject
      jsval skillNumber = 0;
      jsval minSkill = 0;
      jsval maxSkill = 0;
      JSObject *skills = JS_NewArrayObject( cx, 0, nullptr );
      for( auto i = 0; i < static_cast<int>( skillReqs.size() ); i++ )
      {
        resEntry = skillReqs[i];
        skillNumber = JS::Int32Value( resEntry.skillNumber );
        minSkill = JS::Int32Value( resEntry.minSkill );
        maxSkill = JS::Int32Value( resEntry.maxSkill );

        // Add each required skill as its own object
        JSObject *skill = JS_NewArrayObject( cx, 0, nullptr );
        JS_SetElement( cx, skill, 0, &skillNumber );
        JS_SetElement( cx, skill, 1, &minSkill );
        JS_SetElement( cx, skill, 2, &maxSkill );

        // Now stuff the skill object into the parent object
        jsval skillVal = OBJECT_TO_JSVAL( skill );
        JS_SetElement( cx, skills, i, &skillVal );
      }

      *vp = OBJECT_TO_JSVAL( skills );
      break;
    }
    }
  }
  return true;
}*/

// clang-format off
IMPL_GETS_OBJ(CItem, sectionID,           CItem, setString, GetSectionId().c_str() )
IMPL_GETS_OBJ(CItem, name,                CItem, setString, GetName().c_str() )
IMPL_GETS_OBJ(CItem, title,               CItem, setString, GetTitle().c_str() )
IMPL_GET_OBJ( CItem, x,                   CItem, setInt32,  GetX() )
IMPL_GET_OBJ( CItem, y,                   CItem, setInt32,  GetY() )
IMPL_GET_OBJ( CItem, z,                   CItem, setInt32,  GetZ() )
IMPL_GET_OBJ( CItem, oldX,                CItem, setInt32,  GetOldLocation().x )
IMPL_GET_OBJ( CItem, oldY,                CItem, setInt32,  GetOldLocation().y )
IMPL_GET_OBJ( CItem, oldZ,                CItem, setInt32,  GetOldLocation().z )
IMPL_GET_OBJ( CItem, id,                  CItem, setInt32,  GetId() )
IMPL_GET_OBJ( CItem, colour,              CItem, setInt32,  GetColour() )
IMPL_GET_OBJ( CItem, color,               CItem, setInt32,  GetColour() )
// IMPL_GET_OBJ( CItem, owner,               CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, visible,             CItem, setInt32,  GetVisible() )
IMPL_GET_OBJ( CItem, serial,              CItem, setInt32,  GetSerial() )
IMPL_GET_OBJ( CItem, health,              CItem, setInt32,  GetHP() )
// IMPL_GET_OBJ( CItem, scripttrigger,       CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, scriptTriggers    ,  CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, worldnumber,         CItem, setInt32, WorldNumber() )
IMPL_GET_OBJ( CItem, instanceID,          CItem, setInt32, GetInstanceId() )
IMPL_GET_OBJ( CItem, amount,              CItem, setInt32, GetAmount() )
// IMPL_GET_OBJ( CItem, container,           CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, type,                CItem, setInt32, GetType() )
IMPL_GET_OBJ( CItem, more,                CItem, setInt32, GetTempVar( CITV_MORE ) )
IMPL_GET_OBJ( CItem, more0,               CItem, setInt32, GetTempVar( CITV_MORE0 ) )
IMPL_GET_OBJ( CItem, more1,               CItem, setInt32, GetTempVar( CITV_MORE1 ) )
IMPL_GET_OBJ( CItem, more2,               CItem, setInt32, GetTempVar( CITV_MORE2 ) )
IMPL_GET_OBJ( CItem, morex,               CItem, setInt32, GetTempVar( CITV_MOREX ) )
IMPL_GET_OBJ( CItem, morey,               CItem, setInt32, GetTempVar( CITV_MOREY ) )
IMPL_GET_OBJ( CItem, morez,               CItem, setInt32, GetTempVar( CITV_MOREZ ) )
IMPL_GET_OBJ( CItem, movable,             CItem, setInt32, GetMovable() )
// IMPL_GET_OBJ( CItem, att,                 CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, layer,               CItem, setInt32, GetLayer() )
IMPL_GET_OBJ( CItem, itemsinside,         CItem, setInt32, GetContainsList()->Num() )
// IMPL_GET_OBJ( CItem, totalItemCount    ,  CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, decayable,           CItem, setBoolean, IsDecayable() )
IMPL_GET_OBJ( CItem, decaytime,           CItem, setInt32, GetDecayTime() )
IMPL_GET_OBJ( CItem, lodamage,            CItem, setInt32, GetLoDamage() )
IMPL_GET_OBJ( CItem, hidamage,            CItem, setInt32, GetHiDamage() )
IMPL_GET_OBJ( CItem, ac,                  CItem, setInt32, GetArmourClass() )
IMPL_GET_OBJ( CItem, def,                 CItem, setInt32, GetResist( PHYSICAL ) )
IMPL_GET_OBJ( CItem, resistCold,          CItem, setInt32, GetResist( COLD ) )
IMPL_GET_OBJ( CItem, resistHeat,          CItem, setInt32, GetResist( HEAT ) )
IMPL_GET_OBJ( CItem, resistLight,         CItem, setInt32, GetResist( LIGHT ) )
IMPL_GET_OBJ( CItem, resistLightning,     CItem, setInt32, GetResist( LIGHTNING ) )
IMPL_GET_OBJ( CItem, resistPoison,        CItem, setInt32, GetResist( POISON ) )
IMPL_GET_OBJ( CItem, resistRain,          CItem, setInt32, GetResist( RAIN ) )
IMPL_GET_OBJ( CItem, resistSnow,          CItem, setInt32, GetResist( SNOW ) )
IMPL_GET_OBJ( CItem, damageCold,          CItem, setBoolean, GetWeatherDamage( COLD ) )
IMPL_GET_OBJ( CItem, damageHeat,          CItem, setBoolean, GetWeatherDamage( HEAT ) )
IMPL_GET_OBJ( CItem, damageLight,         CItem, setBoolean, GetWeatherDamage( LIGHT ) )
IMPL_GET_OBJ( CItem, damageLightning,     CItem, setBoolean, GetWeatherDamage( LIGHTNING ) )
IMPL_GET_OBJ( CItem, damagePoison,        CItem, setBoolean, GetWeatherDamage( POISON ) )
IMPL_GET_OBJ( CItem, damageRain,          CItem, setBoolean, GetWeatherDamage( RAIN ) )
IMPL_GET_OBJ( CItem, damageSnow,          CItem, setBoolean, GetWeatherDamage( SNOW ) )
IMPL_GETS_OBJ(CItem, name2,               CItem, setString, GetName2().c_str() )
// IMPL_GET_OBJ( CItem, isChar,              CItem, setBoolean, )  // Do manually
// IMPL_GET_OBJ( CItem, isItem,              CItem, setBoolean, )  // Do manually
IMPL_GET_OBJ( CItem, isSpawner,           CItem, setBoolean, GetObjType() == OT_SPAWNER )
// IMPL_GET_OBJ( CItem, race,                CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, maxhp,               CItem, setInt32, GetMaxHP() )
IMPL_GET_OBJ( CItem, maxUses,             CItem, setInt32, GetMaxUses() )
IMPL_GET_OBJ( CItem, usesLeft,            CItem, setInt32, GetUsesLeft() )
IMPL_GET_OBJ( CItem, rank,                CItem, setInt32, GetRank() )
IMPL_GET_OBJ( CItem, creator,             CItem, setInt32, GetCreator() )
IMPL_GET_OBJ( CItem, poison,              CItem, setInt32, GetPoisoned() )
IMPL_GET_OBJ( CItem, dir,                 CItem, setInt32, GetDir() )
IMPL_GET_OBJ( CItem, wipable,             CItem, setInt32, IsWipeable() )
IMPL_GET_OBJ( CItem, buyvalue,            CItem, setInt32, GetBuyValue() )
IMPL_GET_OBJ( CItem, sellvalue,           CItem, setInt32, GetSellValue() )
IMPL_GET_OBJ( CItem, vendorPrice,         CItem, setInt32, GetVendorPrice() )
IMPL_GET_OBJ( CItem, restock,             CItem, setInt32, GetRestock() )
IMPL_GET_OBJ( CItem, good,                CItem, setInt32, GetGood() )
IMPL_GET_OBJ( CItem, divinelock,          CItem, setBoolean, IsDivineLocked() )
IMPL_GET_OBJ( CItem, baseWeight,          CItem, setInt32, GetWeight() )
IMPL_GET_OBJ( CItem, weight,              CItem, setInt32, GetWeightMax() )
IMPL_GET_OBJ( CItem, weightMax,           CItem, setInt32, GetBaseWeight() )
IMPL_GET_OBJ( CItem, maxItems,            CItem, setInt32, GetMaxItems() )
IMPL_GET_OBJ( CItem, strength,            CItem, setInt32, GetStrength() )
IMPL_GET_OBJ( CItem, dexterity,           CItem, setInt32, GetDexterity() )
IMPL_GET_OBJ( CItem, intelligence,        CItem, setInt32, GetIntelligence() )
IMPL_GET_OBJ( CItem, corpse,              CItem, setBoolean, IsCorpse() )
IMPL_GETS_OBJ(CItem, desc,                CItem, setString, GetDesc().c_str() )
IMPL_GETS_OBJ(CItem, event,               CItem, setString, GetEvent().c_str() )
IMPL_GET_OBJ( CItem, tempLastTraded,      CItem, setInt32, GetTempLastTraded() / 1000 )
IMPL_GET_OBJ( CItem, tempTimer,           CItem, setInt32, GetTempTimer() / 1000 )
IMPL_GET_OBJ( CItem, shouldSave,          CItem, setBoolean, ShouldSave() )
IMPL_GET_OBJ( CItem, isNewbie,            CItem, setBoolean, IsNewbie() )
IMPL_GET_OBJ( CItem, isDispellable,       CItem, setBoolean, IsDispellable() )
IMPL_GET_OBJ( CItem, madeWith,            CItem, setInt32, GetMadeWith() )
IMPL_GET_OBJ( CItem, entryMadeFrom,       CItem, setInt32, EntryMadeFrom() )
IMPL_GET_OBJ( CItem, isPileable,          CItem, setBoolean, IsPileable() )
IMPL_GET_OBJ( CItem, isMarkedByMaker,     CItem, setBoolean, IsMarkedByMaker() )
IMPL_GET_OBJ( CItem, isDyeable,           CItem, setBoolean, IsDyeable() )
IMPL_GET_OBJ( CItem, isDamageable,        CItem, setBoolean, IsDamageable() )
IMPL_GET_OBJ( CItem, isWipeable,          CItem, setBoolean, IsWipeable() )
IMPL_GET_OBJ( CItem, isGuarded,           CItem, setBoolean, IsGuarded() )
IMPL_GET_OBJ( CItem, isDoorOpen,          CItem, setBoolean, IsDoorOpen() )
IMPL_GET_OBJ( CItem, isFieldSpell,        CItem, setBoolean, IsFieldSpell() )
IMPL_GET_OBJ( CItem, isLockedDown,        CItem, setBoolean, IsLockedDown() )
IMPL_GET_OBJ( CItem, isShieldType,        CItem, setBoolean, IsShieldType() )
IMPL_GET_OBJ( CItem, isMetalType,         CItem, setBoolean, IsMetalType() )
IMPL_GET_OBJ( CItem, isLeatherType,       CItem, setBoolean, IsLeatherType() )
IMPL_GET_OBJ( CItem, canBeLockedDown,     CItem, setBoolean, CanBeLockedDown() )
IMPL_GET_OBJ( CItem, isContType,          CItem, setBoolean, IsContType() )
IMPL_GET_OBJ( CItem, carveSection,        CItem, setInt32,   GetCarve() )
IMPL_GET_OBJ( CItem, ammoID,              CItem, setInt32,   GetAmmoId() )
IMPL_GET_OBJ( CItem, ammoHue,             CItem, setInt32,   GetAmmoHue() )
IMPL_GET_OBJ( CItem, ammoFX,              CItem, setInt32,   GetAmmoFX() )
IMPL_GET_OBJ( CItem, ammoFXHue,           CItem, setInt32,   GetAmmoFXHue() )
IMPL_GET_OBJ( CItem, ammoFXRender,        CItem, setInt32,   GetAmmoFXRender() )
IMPL_GET_OBJ( CItem, speed,               CItem, setInt32,   GetSpeed() )
// IMPL_GET_OBJ( CItem, multi,               CItem, setInt32,   ) // Do manually
IMPL_GET_OBJ( CItem, maxRange,            CItem, setInt32,   GetMaxRange() )
IMPL_GET_OBJ( CItem, baseRange,           CItem, setInt32,   GetBaseRange() )
// IMPL_GET_OBJ( CItem, region,              CItem, setInt32, ) // Do manually
IMPL_GET_OBJ( CItem, spawnSerial,         CItem, setInt32,   GetSpawn() )
//IMPL_GETS_OBJ(CItem, origin,              CItem, setString,  GetOrigin() ) // Do manually
IMPL_GET_OBJ( CItem, isItemHeld,          CItem, setBoolean, IsHeldOnCursor() )
IMPL_GET_OBJ( CItem, stealable,           CItem, setInt32,   GetStealable() )
// IMPL_GET_OBJ( CItem, spawnsection,        CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, sectionalist,        CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, mininterval,         CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxinterval,         CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, lockdowns,           CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxLockdowns,        CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, trashContainers,     CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxTrashContainers,  CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, secureContainers,    CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxSecureContainers, CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, friends,             CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxFriends,          CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, guests,              CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxGuests,           CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, owners,              CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxOwners,           CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, bans,                CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxBans,             CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, vendors,             CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, maxVendors,          CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, deed,                CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, isPublic,            CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, buildTimestamp,      CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, tradeTimestamp,      CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, banX,                CItem, setInt32, ) // Do manually
// IMPL_GET_OBJ( CItem, banY,                CItem, setInt32, ) // Do manually
// clang-format on

/*
bool CItemProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CItem *gPriv = JS::GetMaybePtrFromReservedSlot<CItem >(obj , 0);
  SERIAL TempSerial = INVALIDSERIAL;

  if( !ValidateObject( gPriv ))
    return false;

  if( JSVAL_IS_INT( id ))
  {
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
    {
      case CIP_OWNER:
        CChar *pOwner;
        pOwner = gPriv->GetOwnerObj();

        if( !ValidateObject( pOwner ))
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, pOwner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          *vp = OBJECT_TO_JSVAL( myObj );
        }
        break;
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

        *vp = JS::Int32Value( lastScriptTrigger );
        break;
      }
      case CIP_SCRIPTTRIGGERS:
      {
        jsval scriptId;
        JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

        std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
        for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
        {
          scriptId = JS::Int32Value( scriptTriggers[i] );
          JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
        }

        *vp = OBJECT_TO_JSVAL( scriptTriggersJS );
        break;
      }
        // We should Accquie an object here and return that, so you can do
        // item.container.name
      case CIP_CONTAINER:
        TempSerial = gPriv->GetContSerial();

        if( TempSerial == INVALIDSERIAL )
        {
          // Return a JS_NULL
          *vp = JS::CurrentGlobalOrNull;
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
              *vp = JS::CurrentGlobalOrNull;
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
              *vp = JS::CurrentGlobalOrNull;
            }
          }
        }
        break;
      case CIP_ATT:			*vp = JS::Int32Value( RandomNum( gPriv->GetLoDamage(), gPriv->GetHiDamage() ));	break;
      case CIP_TOTALITEMCOUNT: *vp = JS::Int32Value( GetTotalItemCount( gPriv ));	break;
      case CIP_ISCHAR:	*vp = JSVAL_FALSE;								break;
      case CIP_ISITEM:	*vp = JSVAL_TRUE;								break;
      case CIP_RACE:
      {
        CRace *TempRace = nullptr;
        TempRace = Races->Race( gPriv->GetRace() );

        if( TempRace == nullptr )
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myRace	= JSEngine->AcquireObject( IUE_RACE, TempRace, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          *vp = OBJECT_TO_JSVAL( myRace );
        }
        break;
      }
      case CIP_REGION:
      {
        CTownRegion *myReg = gPriv->GetRegion();
        if( myReg == nullptr )
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        else
        {
          JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, myReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          *vp = OBJECT_TO_JSVAL( myTown );
        }
        break;
      }
      case CIP_ORIGIN:
        tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
        *vp = JS::StringValue( tString );
        break;

        // The following entries are specifically for CSpawnItem objects
      case CIP_SPAWNSECTION:
        if( gPriv->GetObjType() == OT_SPAWNER )
        {
          tString = JS_NewStringCopyZ( cx, ( static_cast<CSpawnItem *>( gPriv ))->GetSpawnSection().c_str() );
          *vp = JS::StringValue( tString );
        }
        break;
      case CIP_SECTIONALIST:
        if( gPriv->GetObjType() == OT_SPAWNER )
        {
          *vp = JS::Int32Value(( static_cast<CSpawnItem *>( gPriv ))->IsSectionAList() );
        }
        break;
      case CIP_MININTERVAL:
        if( gPriv->GetObjType() == OT_SPAWNER )
        {
          *vp = JS::Int32Value(( static_cast<CSpawnItem *>( gPriv ))->GetInterval( 0 ));
        }
        break;
      case CIP_MAXINTERVAL:
        if( gPriv->GetObjType() == OT_SPAWNER )
        {
          *vp = JS::Int32Value(( static_cast<CSpawnItem *>( gPriv ))->GetInterval( 1 ));
        }
        break;
      case CIP_MULTI:
        CMultiObj *multi;
        multi = gPriv->GetMultiObj();

        if( !ValidateObject( multi ))
        {
          *vp = JS::CurrentGlobalOrNull;
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
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetLockdownCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXLOCKDOWNS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxLockdowns() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_TRASHCONTAINERS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetTrashContainerCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXTRASHCONTAINERS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxTrashContainers() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_SECURECONTAINERS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetSecureContainerCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXSECURECONTAINERS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxSecureContainers() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_FRIENDS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetFriendCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXFRIENDS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxFriends() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_GUESTS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetGuestCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXGUESTS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxGuests() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_OWNERS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetOwnerCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXOWNERS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxOwners() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_BANS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetBanCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXBANS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxBans() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_VENDORS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetVendorCount() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_MAXVENDORS:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetMaxVendors() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_DEED:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetDeed().c_str() );
          *vp = JS::StringValue( tString );
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_ISPUBLIC:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::BooleanValue(( static_cast<CMultiObj *>( gPriv )->GetPublicStatus() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_BUILDTIMESTAMP:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetBuildTimestamp().c_str() );
          *vp = JS::StringValue( tString );
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_TRADETIMESTAMP:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetTradeTimestamp().c_str() );
          *vp = JS::StringValue( tString );
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_BANX:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetBanX() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      case CIP_BANY:
        if( gPriv->GetObjType() == OT_MULTI )
        {
          *vp = JS::Int32Value(( static_cast<CMultiObj *>( gPriv )->GetBanY() ));
        }
        else
        {
          *vp = JS::CurrentGlobalOrNull;
        }
        break;
      default:
        break;
    }
  }
  return true;
}
*/

bool CItemProps_setProperty(JSContext* cx, JSObject* obj, JS::HandleValue id, JS::MutableHandleValue vp)
{
  CItem *gPriv = JS::GetMaybePtrFromReservedSlot<CItem >(obj , 0);
  if( !ValidateObject( gPriv ))
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );

  if( id.isInt32())
  {
    TIMERVAL newTime;
    switch( id.toInt32() )
    {
      case CIP_SECTIONID:		gPriv->SetSectionId( encaps.toString() );					break;
      case CIP_NAME:			gPriv->SetName( encaps.toString() );						break;
      case CIP_X:				gPriv->SetLocation( static_cast<SI16>( encaps.toInt() ), gPriv->GetY(), gPriv->GetZ() );	break;
      case CIP_Y:				gPriv->SetLocation( gPriv->GetX(), static_cast<SI16>( encaps.toInt() ), gPriv->GetZ() );	break;
      case CIP_Z:				gPriv->SetZ( static_cast<SI08>( encaps.toInt() ));			break;
      case CIP_ID:			gPriv->SetId( static_cast<UI16>( encaps.toInt() ));			break;
      case CIP_COLOUR:		gPriv->SetColour( static_cast<UI16>( encaps.toInt() ));		break;
      case CIP_OWNER:
        if (!vp.isObject() || vp.toObjectOrNull() != JS::CurrentGlobalOrNull(cx))
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
         if (!vp.isObject() || vp.toObjectOrNull() != JS::CurrentGlobalOrNull(cx))
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
          return false;

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
        if (!vp.isObject() || vp.toObjectOrNull() != JS::CurrentGlobalOrNull(cx))
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
        newTime = encaps.toInt();
        if( newTime != 0 )
        {
          newTime = BuildTimeValue( newTime );
        }
        gPriv->SetDecayTime( newTime );
        break;
      case CIP_LODAMAGE:		gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));			break;
      case CIP_HIDAMAGE:		gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));			break;
      case CIP_AC:			gPriv->SetArmourClass( static_cast<UI08>( encaps.toInt() ));		break;
      case CIP_DEF:			gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), PHYSICAL );	break;
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
      case CIP_NAME2:			gPriv->SetName2( encaps.toString() );						break;
      case CIP_RACE:			gPriv->SetRace( static_cast<RACEID>( encaps.toInt() ));		break;
      case CIP_MAXHP:			gPriv->SetMaxHP( static_cast<SI16>( encaps.toInt() ));		break;
      case CIP_MAXUSES:		gPriv->SetMaxUses( static_cast<UI16>( encaps.toInt() ));	break;
      case CIP_USESLEFT:		gPriv->SetUsesLeft( static_cast<UI16>( encaps.toInt() ));	break;
      case CIP_RANK:			gPriv->SetRank( static_cast<SI08>( encaps.toInt() ));		break;
      case CIP_CREATOR:		gPriv->SetCreator( static_cast<SERIAL>( encaps.toInt() ));	break;
      case CIP_POISON:		gPriv->SetPoisoned( static_cast<UI08>( encaps.toInt() ));	break;
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
        newTime = encaps.toInt();
        if( newTime != 0 )
          newTime = BuildTimeValue( newTime );
        gPriv->SetTempLastTraded( newTime );
        break;
      case CIP_TEMPTIMER:
        newTime = encaps.toInt();
        if( newTime != 0 )
          newTime = BuildTimeValue( newTime );
        gPriv->SetTempTimer( newTime );
        break;
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
      default:
        break;
    }
  }

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Item property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

IMPL_GETS_OBJ(CChar, sectionID,           CChar, setString, GetSectionId().c_str() )
IMPL_GETS_OBJ(CChar, name,                CChar, setString, GetName().c_str() )
IMPL_GETS_OBJ(CChar, title,               CChar, setString, GetTitle().c_str() )
IMPL_GET_OBJ( CChar, x,                   CChar, setInt32,  GetX() )
IMPL_GET_OBJ( CChar, y,                   CChar, setInt32,  GetY() )
IMPL_GET_OBJ( CChar, z,                   CChar, setInt32,  GetZ() )
IMPL_GET_OBJ( CChar, oldX,                CChar, setInt32,  GetOldLocation().x )
IMPL_GET_OBJ( CChar, oldY,                CChar, setInt32,  GetOldLocation().y )
IMPL_GET_OBJ( CChar, oldZ,                CChar, setInt32,  GetOldLocation().z )
IMPL_GET_OBJ( CChar, id,                  CChar, setInt32,  GetId() )
IMPL_GET_OBJ( CChar, colour,              CChar, setInt32,  GetColour() )
IMPL_GET_OBJ( CChar, color,               CChar, setInt32,  GetColour() )

bool CCharacterProps_getProperty( JSContext* cx, JSObject* obj, JS::HandleValueArray id, JS::MutableHandleValue vp )
{
  CChar *gPriv = JS::GetMaybePtrFromReservedSlot<CChar >(obj , 0);

  if( !ValidateObject( gPriv ))
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  if( id[0].isInt32())
  {
    CItem *TempItem			= nullptr;
    JSObject *TempObject	= nullptr;
    JSString *tString = nullptr;
    switch( id[0].isInt32() )
    {
      case CCP_ACCOUNTNUM: vp.setInt32( gPriv->GetAccountNum() );	break;
      case CCP_ACCOUNT:
      {
        CAccountBlock_st *accountBlock = &gPriv->GetAccount();
        if( accountBlock == nullptr )
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {	// Otherwise Acquire an object
			JSObject *accountObj = JSEngine->AcquireObject(IUE_ACCOUNT, accountBlock, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
			vp.setObjectOrNull(accountObj);
        }
        break;
      }
      case CCP_CREATEDON: vp.setInt32( gPriv->GetCreatedOn() );		break;
      case CCP_PLAYTIME: vp.setInt32( gPriv->GetPlayTime() );		break;
      case CCP_NAME:
        {
          CSocket *tSock = nullptr;

          std::string mCharName = GetNpcDictName( gPriv, tSock, NRS_SCRIPT );
          std::string convertedString = oldstrutil::stringToWstringToString( mCharName );

          tString = JS_NewStringCopyZ( cx, convertedString.c_str() );
          vp.set(JS::StringValue(tString));
          break;
        }
      case CCP_ORIGNAME:
      {
        [[maybe_unused]] CSocket *tSock = nullptr;

        std::string mCharName = gPriv->GetOrgName();
        std::string convertedString = oldstrutil::stringToWstringToString( mCharName );

        tString = JS_NewStringCopyZ( cx, convertedString.c_str() );
        vp.set(JS::StringValue(tString));
        break;
      }
      case CCP_TITLE:
        tString = JS_NewStringCopyZ( cx, gPriv->GetTitle().c_str() );
        vp.set(JS::StringValue(tString));
        break;
      case CCP_SECTIONID:
        tString = JS_NewStringCopyZ( cx, gPriv->GetSectionId().c_str() );
        vp.set(JS::StringValue(tString));

        break;
      case CCP_X:			vp.setInt32( gPriv->GetX() );			break;
      case CCP_Y:			vp.setInt32( gPriv->GetY() );			break;
      case CCP_Z:			vp.setInt32( gPriv->GetZ() );			break;
      case CCP_OLDX:		
      {
        auto oldLocation = gPriv->GetOldLocation();
        vp.setInt32( oldLocation.x );
        break;
      }
      case CCP_OLDY:		
      {
        auto oldLocation = gPriv->GetOldLocation();
        vp.setInt32( oldLocation.y );
        break;
      }
      case CCP_OLDZ:		
      {
        auto oldLocation = gPriv->GetOldLocation();
        vp.setInt32( oldLocation.z );
        break;
      }
      case CCP_ID:		vp.setInt32( gPriv->GetId() );				break;
      case CCP_COLOUR:	vp.setInt32( gPriv->GetColour() );			break;
      case CCP_CONTROLSLOTS:	vp.setInt32( static_cast<UI08>( gPriv->GetControlSlots() ));			break;
      case CCP_CONTROLSLOTSUSED:	vp.setInt32( static_cast<UI08>( gPriv->GetControlSlotsUsed() ));	break;
      case CCP_ORNERINESS:	vp.setInt32( gPriv->GetOrneriness() );	break;
      case CCP_OWNER:
        CBaseObject *TempObj;
        TempObj = gPriv->GetOwnerObj();

        if( !ValidateObject( TempObj ))
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myChar);
        }
        break;
      case CCP_VISIBLE:		vp.setInt32( static_cast<UI08>( gPriv->GetVisible() ));	break;
      case CCP_SERIAL:
      {
        if (!INT_FITS_IN_JSVAL(gPriv->GetSerial()))
		{
			 vp.setNumber(gPriv->GetSerial());
		}
        else
        {
          vp.setInt32( gPriv->GetSerial() );
        }
        break;
      }
      case CCP_HEALTH:		vp.setInt32( gPriv->GetHP() );				break;
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
        vp.setInt32( lastScriptTrigger );
        break;
      }
	  case CCP_SCRIPTTRIGGERS:
        {
            JS::RootedObject scriptTriggersJS(cx, JS::NewArrayObject(cx, 0));

            std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
            for (size_t i = 0; i < scriptTriggers.size(); i++)
			{
                JS::RootedValue scriptId(cx, JS::Int32Value(scriptTriggers[i]));
                if (!JS_SetElement(cx, scriptTriggersJS, i, scriptId))
				{
                    // Handle error if needed
                    return false;
                }
            }

            vp.setObject(*scriptTriggersJS);
            break;
        }
      case CCP_WORLDNUMBER:	vp.setInt32( gPriv->WorldNumber() );			break;
      case CCP_INSTANCEID:	vp.setInt32( gPriv->GetInstanceId() );		break;
      case CCP_TARGET:
        CChar *tempChar;
        tempChar = gPriv->GetTarg();

        if( !ValidateObject( tempChar ))
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          vp.setObjectOrNull( myChar );
        }
        break;
      case CCP_DEXTERITY:				vp.setInt32( gPriv->GetDexterity() );		break;
      case CCP_INTELLIGENCE:			vp.setInt32( gPriv->GetIntelligence() );		break;
      case CCP_STRENGTH:				vp.setInt32( gPriv->GetStrength() );			break;
      case CCP_ACTUALDEXTERITY:		vp.setInt32( gPriv->ActualDexterity() );		break;
      case CCP_ACTUALINTELLIGENCE:	vp.setInt32( gPriv->ActualIntelligence() );	break;
      case CCP_ACTUALSTRENGTH:		vp.setInt32( gPriv->ActualStrength() );		break;
      case CCP_BASESKILLS:
		{
			JS::RootedObject TempObject(cx, JS_NewObject(cx, &UOXBaseSkills_class));
			JS_DefineProperties( cx, TempObject, CSkillsProps );
			JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
			vp.setObjectOrNull(TempObject);
			break;
		}
      case CCP_SKILLS:
		{
			JS::RootedObject TempObject(cx, JS_NewObject(cx, &UOXSkills_class));
			JS_DefineProperties( cx, TempObject, CSkillsProps );
			JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
			vp.setObjectOrNull(TempObject);
			break;
		}
      case CCP_SKILLUSE:
		{
			JS::RootedObject TempObject(cx, JS_NewObject(cx, &UOXSkillsUsed_class));
			JS_DefineProperties( cx, TempObject, CSkillsProps );
			JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
			vp.setObjectOrNull(TempObject);
			break;
		}
      case CCP_MANA:			vp.setInt32( gPriv->GetMana() );			break;
      case CCP_STAMINA:		vp.setInt32( gPriv->GetStamina() );		break;
      case CCP_CHARPACK:
        TempItem = gPriv->GetPackItem();

        if( !ValidateObject( TempItem ))
		{
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
		}
        else
        {
          // Otherwise Acquire an object
          JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, TempItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myItem);
        }
        break;
      case CCP_FAME:			vp.setInt32( gPriv->GetFame() );					break;
      case CCP_KARMA:			vp.setInt32( gPriv->GetKarma() );				break;
      case CCP_ATTACK:		vp.setInt32( Combat->CalcAttackPower( gPriv, true ));	break;
      case CCP_CANATTACK:		vp.setBoolean( gPriv->GetCanAttack() );		break;
      case CCP_FLEEAT:		vp.setInt32( gPriv->GetFleeAt() );				break;
      case CCP_REATTACKAT:	vp.setInt32( gPriv->GetReattackAt() );			break;
      case CCP_BRKPEACE:		vp.setInt32( gPriv->GetBrkPeaceChance() );		break;
      case CCP_HUNGER:		vp.setInt32( gPriv->GetHunger() );				break;
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

        vp.setInt32( hungerRate );
        break;
      }
      case CCP_THIRST:		vp.setInt32( gPriv->GetThirst() );				break;
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

        vp.setInt32( thirstRate );
        break;
      }
      case CCP_FROZEN:		vp.setBoolean( gPriv->IsFrozen() );			break;
      case CCP_COMMANDLEVEL:	vp.setInt32( gPriv->GetCommandLevel() );			break;
      case CCP_RACE:
      {
        CRace *TempRace	= nullptr;
        TempRace = Races->Race( gPriv->GetRace() );

        if( TempRace == nullptr )
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myRace	= JSEngine->AcquireObject( IUE_RACE, TempRace, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myRace);
        }
        break;
      }
      case CCP_HASSTOLEN:		vp.setBoolean( gPriv->HasStolen() );	break;
      case CCP_CRIMINAL:		vp.setBoolean( gPriv->IsCriminal() );	break;
      case CCP_MURDERER:		vp.setBoolean( gPriv->IsMurderer() );	break;
      case CCP_INNOCENT:		vp.setBoolean( gPriv->IsInnocent() );	break;
      case CCP_NEUTRAL:		vp.setBoolean( gPriv->IsNeutral() );	break;
      case CCP_MURDERCOUNT:	vp.setInt32( gPriv->GetKills() );		break;
      case CCP_GENDER:
        switch( gPriv->GetId() )
        {
          case 0x0190:	// human male, dead or alive
          case 0x0192:	vp.setInt32( 0 );						break;
          case 0x0191:	// human female, dead or alive
          case 0x0193:	vp.setInt32( 1 );						break;
          case 0x025D:	// elf male, dead or alive
          case 0x025F:	vp.setInt32( 2 );						break;
          case 0x025E:	// elf female, dead or alive
          case 0x0260:	vp.setInt32( 3 );						break;
          case 0x029A:	// gargoyle male, dead or alive
          case 0x02B6:	vp.setInt32( 4 );						break;
          case 0x029B:	// gargoyle female, dead or alive
          case 0x02B7:	vp.setInt32( 5 );						break;
          default:		vp.setInt32( 0xFF );						break;
        }
        break;
      case CCP_DEAD:			vp.setBoolean( gPriv->IsDead() );		break;
      case CCP_NPC:			vp.setBoolean( gPriv->IsNpc() );		break;
      case CCP_AWAKE:			vp.setBoolean( gPriv->IsAwake() );		break;
      case CCP_ONLINE:		vp.setBoolean( IsOnline(( *gPriv )));	break;
      case CCP_DIRECTION:		vp.setInt32( gPriv->GetDir() );			break;
        // 3  objects: regions + towns + guilds
      case CCP_ISRUNNING:		vp.setBoolean( gPriv->GetRunning() );	break;
      case CCP_REGION:
      {
        CTownRegion *myReg = gPriv->GetRegion();
        if( myReg == nullptr )
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, myReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myTown);
        }
        break;
      }
      case CCP_ORIGIN:
        tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
		vp.set(JS::StringValue(tString));
        break;
      case CCP_TOWN:
      {
        UI16 tempTownId	= 0xFF;
        tempTownId = gPriv->GetTown();

        // We need to decide here whether 0xFF is a valid town (wilderness) or not
        // i would say no its not
        if( tempTownId == 0xFF )
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Should build the town here
          JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, cwmWorldState->townRegions[tempTownId], JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          vp.setObjectOrNull(myTown);
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
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          JSObject *myGuild = JSEngine->AcquireObject( IUE_GUILD, GuildSys->Guild( tempGuildId ), JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myGuild);
        }
        break;
      }
      case CCP_SOCKET:
      { // So we can declare the variables here
        CSocket *tSock = gPriv->GetSocket();
        if( tSock == nullptr )
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {	// Otherwise Acquire an object
          JSObject *mySock	= JSEngine->AcquireObject( IUE_SOCK, tSock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          vp.setObjectOrNull(mySock);
        }
        break;
      }
      case CCP_ISCHAR:		vp.setBoolean(true);									break;
      case CCP_ISITEM:		vp.setBoolean(false);								break;
      case CCP_ISSPAWNER:		vp.setBoolean(false);								break;
      case CCP_SPAWNSERIAL:	vp.setInt32( gPriv->GetSpawn() );			break;
      case CCP_MAXHP:			vp.setInt32( gPriv->GetMaxHP() );			break;
      case CCP_MAXSTAMINA:	vp.setInt32( gPriv->GetMaxStam() );			break;
      case CCP_MAXMANA:		vp.setInt32( gPriv->GetMaxMana() );			break;
      case CCP_OLDWANDERTYPE:	vp.setInt32( gPriv->GetOldNpcWander() );		break;
      case CCP_WANDERTYPE:	vp.setInt32( gPriv->GetNpcWander() );		break;
      case CCP_FX1:			vp.setInt32( gPriv->GetFx( 0 ));				break;
      case CCP_FY1:			vp.setInt32( gPriv->GetFy( 0 ));				break;
      case CCP_FX2:			vp.setInt32( gPriv->GetFx( 1 ));				break;
      case CCP_FY2:			vp.setInt32( gPriv->GetFy( 1 ));				break;
      case CCP_FZ:			vp.setInt32( gPriv->GetFz() );				break;
      case CCP_ISONHORSE:		vp.setBoolean( gPriv->IsOnHorse() );		break;
      case CCP_ISFLYING:		vp.setBoolean( gPriv->IsFlying() );		break;
      case CCP_ISGUARDED:		vp.setBoolean( gPriv->IsGuarded() );		break;
      case CCP_GUARDING:
      {
        CBaseObject *tempObj = gPriv->GetGuarding();
        if( !ValidateObject( tempObj ))
        {	// Return a JS_NULL
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
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
          vp.setObjectOrNull(myObj);
        }
        break;
      }
      case CCP_TDEXTERITY:	vp.setInt32( gPriv->GetDexterity2() );		break;
      case CCP_TINTELLIGENCE:	vp.setInt32( gPriv->GetIntelligence2() );	break;
      case CCP_TSTRENGTH:		vp.setInt32( gPriv->GetStrength2() );		break;
      case CCP_POISON:		vp.setInt32( gPriv->GetPoisoned() );			break;
      case CCP_LIGHTLEVEL:	vp.setInt32( gPriv->GetFixedLight() );		break;
      case CCP_VULNERABLE:	vp.setBoolean( !gPriv->IsInvulnerable() );	break;
      case CCP_HUNGERSTATUS:	vp.setBoolean( gPriv->WillHunger() );		break;
      case CCP_THIRSTSTATUS:	vp.setBoolean( gPriv->WillThirst() );		break;
      case CCP_LODAMAGE:		vp.setInt32( gPriv->GetLoDamage() );			break;
      case CCP_HIDAMAGE:		vp.setInt32( gPriv->GetHiDamage() );			break;
      case CCP_FLAG:			vp.setInt32( gPriv->GetFlag() );				break;
      case CCP_ATWAR:			vp.setBoolean( gPriv->IsAtWar() );			break;
      case CCP_SPELLCAST:		vp.setInt32( gPriv->GetSpellCast() );		break;
      case CCP_ISCASTING:		vp.setBoolean( gPriv->IsCasting() || gPriv->IsJSCasting() );		break;
      case CCP_PRIV:			vp.setInt32( gPriv->GetPriv() );				break;
      case CCP_TOWNPRIV:		vp.setInt32( gPriv->GetTownPriv() );			break;
      case CCP_GUILDTITLE:
        tString = JS_NewStringCopyZ( cx, gPriv->GetGuildTitle().c_str() );
        vp.set(JS::StringValue(tString));
        break;
      case CCP_HAIRSTYLE:		vp.setInt32( gPriv->GetHairStyle() );		break;
      case CCP_HAIRCOLOUR:	vp.setInt32( gPriv->GetHairColour() );		break;
      case CCP_BEARDSTYLE:	vp.setInt32( gPriv->GetBeardStyle() );		break;
      case CCP_BEARDCOLOUR:	vp.setInt32( gPriv->GetBeardColour() );		break;
      case CCP_FONTTYPE:		vp.setInt32( gPriv->GetFontType() );			break;
      case CCP_SAYCOLOUR:		vp.setInt32( gPriv->GetSayColour() );		break;
      case CCP_EMOTECOLOUR:	vp.setInt32( gPriv->GetEmoteColour() );		break;
      case CCP_ATTACKER:
      {
        // Hm Quite funny, same thing as .owner
        CChar *tempChar	= gPriv->GetAttacker();
        if( !ValidateObject( tempChar ))
        {	// Return a JS_NULL
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myChar);
        }
        break;
      }
      case CCP_RACEGATE:		vp.setInt32( gPriv->GetRaceGate() );			break;
      case CCP_SKILLLOCK:
		{
			JS::RootedObject TempObject(cx, JS_NewObject(cx, &UOXSkillsLock_class));
			JS_DefineProperties( cx, TempObject, CSkillsProps );
			JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
			vp.setObjectOrNull(TempObject);
			break;
		}
      case CCP_DEATHS:		vp.setInt32( gPriv->GetDeaths() );					break;
      case CCP_OWNERCOUNT:	vp.setInt32( static_cast<UI08>( gPriv->GetOwnerCount() ));		break;
      case CCP_NEXTACT:		vp.setInt32( gPriv->GetNextAct() );					break;
      case CCP_PETCOUNT:		vp.setInt32( static_cast<UI08>( gPriv->GetPetList()->Num() ));	break;
      case CCP_FOLLOWERCOUNT:		vp.setInt32( static_cast<UI08>( gPriv->GetFollowerList()->Num() ));	break;
      case CCP_OWNEDITEMSCOUNT:	vp.setInt32( gPriv->GetOwnedItems()->size() );	break;
      case CCP_CELL:			vp.setInt32( gPriv->GetCell() );						break;
      case CCP_ALLMOVE:		vp.setBoolean( gPriv->AllMove() );					break;
      case CCP_HOUSEICONS:	vp.setBoolean( gPriv->ViewHouseAsIcon() );			break;
      case CCP_SPATTACK:		vp.setInt32( gPriv->GetSpAttack() );					break;
      case CCP_SPDELAY:		vp.setInt32( gPriv->GetSpDelay() );					break;
      case CCP_AITYPE:		vp.setInt32( gPriv->GetNpcAiType() );				break;
      case CCP_SPLIT:			vp.setInt32( gPriv->GetSplit() );					break;
      case CCP_SPLITCHANCE:	vp.setInt32( gPriv->GetSplitChance() );				break;
      case CCP_TRAINER:		vp.setBoolean( gPriv->CanTrain() );				break;
      case CCP_HIRELING:		vp.setBoolean( gPriv->CanBeHired() );				break;
      case CCP_WEIGHT:		vp.setInt32( gPriv->GetWeight() );					break;
      case CCP_SQUELCH:		vp.setInt32( gPriv->GetSquelched() );				break;
      case CCP_ISJAILED:		vp.setBoolean( gPriv->IsJailed() );				break;
      case CCP_MAGICREFLECT:	vp.setBoolean( gPriv->IsTempReflected() );			break;
      case CCP_PERMMAGICREFLECT:	vp.setBoolean( gPriv->IsPermReflected() );		break;
      case CCP_TAMED:			vp.setBoolean( gPriv->IsTamed() );					break;
      case CCP_TAMEDHUNGERRATE: vp.setInt32( gPriv->GetTamedHungerRate() );		break;
      case CCP_TAMEDTHIRSTRATE: vp.setInt32( gPriv->GetTamedThirstRate() );		break;
      case CCP_HUNGERWILDCHANCE: vp.setInt32( gPriv->GetTamedHungerWildChance() );	break;
      case CCP_THIRSTWILDCHANCE: vp.setInt32( gPriv->GetTamedThirstWildChance() );	break;
      case CCP_FOODLIST:
        tString = JS_NewStringCopyZ( cx, gPriv->GetFood().c_str() );
       vp.set(JS::StringValue(tString));
        break;
      case CCP_MOUNTED:		vp.setBoolean( gPriv->GetMounted() );				break;
      case CCP_STABLED:		vp.setBoolean( gPriv->GetStabled() );				break;
      case CCP_USINGPOTION:	vp.setBoolean( gPriv->IsUsingPotion() );			break;
      case CCP_STEALTH:		vp.setInt32( gPriv->GetStealth() );					break;
      case CCP_SKILLTOTAME:	vp.setInt32( gPriv->GetTaming() );					break;
      case CCP_SKILLTOPROV:	vp.setInt32( gPriv->GetProvoing() );					break;
      case CCP_SKILLTOPEACE:	vp.setInt32( gPriv->GetPeaceing() );					break;
      case CCP_POISONSTRENGTH:	vp.setInt32( gPriv->GetPoisonStrength() );		break;
      case CCP_ISPOLYMORPHED:	vp.setBoolean( gPriv->IsPolymorphed() );			break;
      case CCP_ISINCOGNITO:	vp.setBoolean( gPriv->IsIncognito() );				break;
      case CCP_ISDISGUISED:	vp.setBoolean( gPriv->IsDisguised() );				break;
      case CCP_CANRUN:		vp.setBoolean( gPriv->CanRun() );					break;
      case CCP_ISMEDITATING:	vp.setBoolean( gPriv->IsMeditating() );			break;
      case CCP_ISGM:			vp.setBoolean( gPriv->IsGM() );					break;
      case CCP_CANBROADCAST:	vp.setBoolean( gPriv->CanBroadcast() );			break;
      case CCP_SINGCLICKSER:	vp.setBoolean( gPriv->GetSingClickSer() );			break;
      case CCP_NOSKILLTITLES:	vp.setBoolean( gPriv->NoSkillTitles() );			break;
      case CCP_ISGMPAGEABLE:	vp.setBoolean( gPriv->IsGMPageable() );			break;
      case CCP_CANSNOOP:		vp.setBoolean( gPriv->CanSnoop() );				break;
      case CCP_ISCOUNSELOR:	vp.setBoolean( gPriv->IsCounselor() );				break;
      case CCP_NONEEDMANA:	vp.setBoolean( gPriv->NoNeedMana() );				break;
      case CCP_ISDISPELLABLE:	 vp.setBoolean( gPriv->IsDispellable() );			break;
      case CCP_NONEEDREAGS:	vp.setBoolean( gPriv->NoNeedReags() );				break;
      case CCP_ISANIMAL:		vp.setBoolean( cwmWorldState->creatures[gPriv->GetId()].IsAnimal() ); break;
      case CCP_ISHUMAN:		vp.setBoolean( cwmWorldState->creatures[gPriv->GetId()].IsHuman() ); break;
      case CCP_ORGID:			vp.setInt32( gPriv->GetOrgId() );					break;
      case CCP_ORGSKIN:		vp.setInt32( gPriv->GetOrgSkin() );					break;
      case CCP_NPCFLAG:		vp.setInt32( static_cast<SI32>(gPriv->GetNPCFlag() ));break;
      case CCP_NPCGUILD:		vp.setInt32( gPriv->GetNPCGuild() );					break;
      case CCP_ISSHOP:		vp.setBoolean( gPriv->IsShop() );					break;
      case CCP_MAXLOYALTY:	vp.setInt32( gPriv->GetMaxLoyalty() );				break;
      case CCP_LOYALTY:		vp.setInt32( gPriv->GetLoyalty() );					break;
      case CCP_LOYALTYRATE:
      {
        // Use global loyalty rate from UOX.INI
        UI16 loyaltyRate = 0;
        if( loyaltyRate == 0 )
        {
          loyaltyRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_LOYALTYRATE );
        }

        vp.setInt32( loyaltyRate );
        break;
      }
      case CCP_SHOULDSAVE:	vp.setBoolean( gPriv->ShouldSave() );			break;
      case CCP_PARTYLOOTABLE:
      {
        Party *toGet = PartyFactory::GetSingleton().Get( gPriv );
        if( toGet == nullptr )
        {
          vp.setBoolean( false );
        }
        else
        {
          CPartyEntry *toScan = toGet->Find( gPriv );
          if( toScan == nullptr )
          {
            vp.setBoolean( false );
          }
          else
          {
            vp.setBoolean( toScan->IsLootable() );
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
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myParty = JSEngine->AcquireObject( IUE_PARTY, tempParty, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		  vp.setObjectOrNull(myParty);
        }
        break;
      }
      case CCP_MULTI:
        CMultiObj *multi;
        multi = gPriv->GetMultiObj();

        if( !ValidateObject( multi ))
        {
          vp.setObjectOrNull(JS::CurrentGlobalOrNull(cx));
        }
        else
        {
          // Otherwise Acquire an object
          JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		   vp.setObjectOrNull(myObj);
        }
        break;
      case CCP_HOUSESOWNED:		vp.setInt32( gPriv->CountHousesOwned( false ));	break;
      case CCP_HOUSESCOOWNED:		vp.setInt32( gPriv->CountHousesOwned( true ));	break;
      default:
        break;
    }
  }

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    // ... but keep track of the property value we're trying to retrieve, stored in vp!
    JS::RootedValue origVp(cx, vp.get());
	bool retVal = origScript->CallParticularEvent("_restorecontext_", id, 0, vp);
	vp.set(origVp);
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Item property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

bool CCharacterProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CChar *gPriv = JS::GetMaybePtrFromReservedSlot<CChar >(obj , 0);
  if( !ValidateObject( gPriv ))
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );

  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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
        if( *vp != JS::CurrentGlobalOrNull )
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
        if( *vp != JS::CurrentGlobalOrNull )
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
          return false;

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
        if( *vp != JS::CurrentGlobalOrNull )
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
      case CCP_ORIGIN:		gPriv->SetOrigin( cwmWorldState->ServerData()->EraStringToEnum( encaps.toString() ));	break;
      case CCP_TOWN:
        cwmWorldState->townRegions[gPriv->GetTown()]->RemoveTownMember( *gPriv );
        if( *vp != JS::CurrentGlobalOrNull )
        {
          cwmWorldState->townRegions[encaps.toInt()]->AddAsTownMember( *gPriv );
        }
        break;
      case CCP_GUILD:
        if( !gPriv->IsNpc() )
        {
          GuildSys->Resign( gPriv->GetSocket() );

          if( *vp != JS::CurrentGlobalOrNull )
          {
            CGuild *myGuild = static_cast<CGuild*>( encaps.toObject() );
            myGuild->NewRecruit( gPriv->GetSerial() );
          }
        }
        break;
      case CCP_OLDWANDERTYPE: gPriv->SetOldNpcWander( static_cast<SI08>( encaps.toInt() )); 		break;
      case CCP_WANDERTYPE: 	gPriv->SetNpcWander( static_cast<SI08>( encaps.toInt() ), true );	break;
      case CCP_FX1:			gPriv->SetFx( static_cast<SI16>( encaps.toInt() ), 0 );	break;
      case CCP_FY1:			gPriv->SetFy( static_cast<SI16>( encaps.toInt() ), 0 );	break;
      case CCP_FX2:			gPriv->SetFx( static_cast<SI16>( encaps.toInt() ), 1 );	break;
      case CCP_FY2:			gPriv->SetFy( static_cast<SI16>( encaps.toInt() ), 1 );	break;
      case CCP_FZ:			gPriv->SetFz( static_cast<SI08>( encaps.toInt() ));		break;
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
      case CCP_PRIV:			gPriv->SetPriv( static_cast<UI16>( encaps.toInt() ));			break;
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
        if( *vp != JS::CurrentGlobalOrNull )
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
      case CCP_DEATHS:		gPriv->SetDeaths( static_cast<UI16>( encaps.toInt() ));		break;
      case CCP_NEXTACT:		gPriv->SetNextAct( static_cast<UI08>( encaps.toInt() ));	break;
      case CCP_CELL:			gPriv->SetCell( static_cast<SI08>( encaps.toInt() ));		break;
      case CCP_ALLMOVE:		gPriv->SetAllMove( encaps.toBool() );						break;
      case CCP_HOUSEICONS:	gPriv->SetViewHouseAsIcon( encaps.toBool() );				break;
      case CCP_SPATTACK:		gPriv->SetSpAttack( static_cast<SI16>( encaps.toInt() ));	break;
      case CCP_SPDELAY:		gPriv->SetSpDelay( static_cast<SI08>( encaps.toInt() ));	break;
      case CCP_AITYPE:		gPriv->SetNPCAiType( static_cast<SI16>( encaps.toInt() ));	break;
      case CCP_SPLIT:			gPriv->SetSplit( static_cast<UI08>( encaps.toInt() ));		break;
      case CCP_SPLITCHANCE:	gPriv->SetSplitChance( static_cast<UI08>( encaps.toInt() ));break;
      case CCP_HIRELING:		gPriv->SetCanHire( encaps.toBool() );						break;
      case CCP_TRAINER:		gPriv->SetCanTrain( encaps.toBool() );						break;
      case CCP_WEIGHT:		gPriv->SetWeight( static_cast<SI32>( encaps.toInt() ));		break;
      case CCP_SQUELCH:		gPriv->SetSquelched( static_cast<UI08>( encaps.toInt() ));	break;
      case CCP_MAGICREFLECT:	gPriv->SetTempReflected( encaps.toBool() );					break;
      case CCP_PERMMAGICREFLECT:	gPriv->SetPermReflected( encaps.toBool() );				break;
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
      case CCP_POISONSTRENGTH:	gPriv->SetPoisonStrength( static_cast<UI08>( encaps.toInt() ));	break;
      case CCP_ISPOLYMORPHED:	gPriv->IsPolymorphed( encaps.toBool() );		break;
      case CCP_ISINCOGNITO:	gPriv->IsIncognito( encaps.toBool() );			break;
      case CCP_ISDISGUISED:	gPriv->IsDisguised( encaps.toBool() );			break;
      case CCP_CANRUN:		gPriv->SetRun( encaps.toBool() );				break;
      case CCP_ISGUARDED:		gPriv->SetGuarded( encaps.toBool() );			break;
      case CCP_GUARDING:
        if( *vp != JS::CurrentGlobalOrNull )
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Character property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}


IMPL_GETS( CRegion, name,              CTownRegion, setString,  GetName().c_str() )
// DECL_GET_SET( CRegion, mayor )
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
IMPL_GET(  CRegion, worldNumber,       CTownRegion, setInt32,   WorldNumber() )
IMPL_GET(  CRegion, instanceID,        CTownRegion, setInt32,   GetInstanceId() )
IMPL_GET(  CRegion, chanceBigOre,      CTownRegion, setInt32,   GetChanceBigOre() )
IMPL_GET(  CRegion, numOrePrefs,       CTownRegion, setInt32,   GetNumOrePreferences() )
IMPL_GET(  CRegion, population,        CTownRegion, setInt32,   GetPopulation() )
IMPL_GETS( CRegion, members,           CTownRegion, setString,  GetTownMemberSerials().c_str() )
IMPL_GET(  CRegion, id,                CTownRegion, setInt32,   GetRegionNum() )
// IMPL_GET( CRegion, scriptTrigger,     CTownRegion, setInt32,   )
// IMPL_GET( CRegion, scriptTriggers,    CTownRegion, setInt32, )
IMPL_GET(  CRegion, numGuards,         CTownRegion, setInt32,   NumGuards() )
IMPL_GET(  CRegion, taxes,             CTownRegion, setInt32,   GetTaxes() )
IMPL_GET(  CRegion, reserves,          CTownRegion, setInt32,   GetReserves() )
IMPL_GET(  CRegion, appearance,        CTownRegion, setInt32,   GetAppearance() )
IMPL_GET(  CRegion, music,             CTownRegion, setInt32,   GetMusicList() )
IMPL_GET(  CRegion, weather,           CTownRegion, setInt32,   GetWeather() )
IMPL_GETS( CRegion, owner,             CTownRegion, setString,  GetOwner().c_str() )

//      case CREGP_MAYOR:
//        CChar *tempMayor;
//        tempMayor = gPriv->GetMayor();
//
//        if( !ValidateObject( tempMayor ))
//          *vp = JS::CurrentGlobalOrNull;
//        else
//        {
//          // Otherwise Acquire an object
//          JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempMayor, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
//          *vp = OBJECT_TO_JSVAL( myChar );
//        }
//        break;
//      case CREGP_SCRIPTTRIGGER:
//      {
//        // For backwards compatibility, get last scripttrigger from vector
//        // For older worldfiles, this will be the only scripttrigger added to the vector after load
//        std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
//        UI16 lastScriptTrigger = 0;
//        auto numberOfTriggers = scriptTriggers.size();
//        if( numberOfTriggers > 0 )
//        {
//          lastScriptTrigger = scriptTriggers[numberOfTriggers - 1];
//        }
//
//        *vp = JS::Int32Value( lastScriptTrigger );
//        break;
//      }
//      case CREGP_SCRIPTTRIGGERS:
//      {
//        jsval scriptId;
//        JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );
//
//        std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
//        for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
//        {
//          scriptId = JS::Int32Value( scriptTriggers[i] );
//          JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
//        }
//
//        *vp = OBJECT_TO_JSVAL( scriptTriggersJS );
//        break;
//      }

IMPL_SETS( CRegion, name,              CTownRegion, toString,  SetName )
IMPL_SET(  CRegion, mayor,             CTownRegion, toInt32,   SetMayorSerial )
IMPL_SET(  CRegion, race,              CTownRegion, toInt32,   SetRace )
IMPL_SET(  CRegion, tax,               CTownRegion, toInt32,   TaxedAmount )
IMPL_SET(  CRegion, taxResource,       CTownRegion, toInt32,   SetResourceId )
IMPL_SET(  CRegion, canMark,           CTownRegion, toBoolean, CanMark )
IMPL_SET(  CRegion, canRecall,         CTownRegion, toBoolean, CanRecall )
IMPL_SET(  CRegion, canGate,           CTownRegion, toBoolean, CanGate )
IMPL_SET(  CRegion, canTeleport,       CTownRegion, toBoolean, CanTeleport )
IMPL_SET(  CRegion, canPlaceHouse,     CTownRegion, toBoolean, CanPlaceHouse )
IMPL_SET(  CRegion, isGuarded,         CTownRegion, toBoolean, IsGuarded )
IMPL_SET(  CRegion, canCastAggressive, CTownRegion, toBoolean, CanCastAggressive )
IMPL_SET(  CRegion, isSafeZone,        CTownRegion, toBoolean, IsSafeZone )
IMPL_SET(  CRegion, health,            CTownRegion, toInt32,   SetHealth )
IMPL_SET(  CRegion, isDungeon,         CTownRegion, toBoolean, IsDungeon )
IMPL_SET(  CRegion, worldNumber,       CTownRegion, toInt32,   WorldNumber )
IMPL_SET(  CRegion, instanceID,        CTownRegion, toInt32,   SetInstanceId )
IMPL_SET(  CRegion, chanceBigOre,      CTownRegion, toInt32,   SetChanceBigOre )
IMPL_SET(  CRegion, id,                CTownRegion, toInt32,  SetRegionNum )
// IMPL_SET( CRegion, scriptTrigger,     CTownRegion, setInt32,   )
// IMPL_SET( CRegion, scriptTriggers,    CTownRegion, setInt32, )
IMPL_SET(  CRegion, numGuards,         CTownRegion, toInt32,   SetNumGuards )
IMPL_SET(  CRegion, taxes,             CTownRegion, toInt32,   SetTaxesLeft )
IMPL_SET(  CRegion, reserves,          CTownRegion, toInt32,   SetReserves )
//IMPL_SET( CRegion, appearance,        CTownRegion, toInt32,   SetAppearance )
IMPL_SET(  CRegion, music,             CTownRegion, toInt32,   SetMusicList )
IMPL_SET(  CRegion, weather,           CTownRegion, toInt32,   SetWeather )

//      case CREGP_SCRIPTTRIGGER:
//      {
//        // For backwards compatibility; clears out other scripts and assigns a specific script
//        UI16 scriptId = static_cast<UI16>( encaps.toInt() );
//        cScript *toExecute	= JSMapping->GetScript( scriptId );
//        if( toExecute == nullptr )
//        {
//          ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
//        }
//        else
//        {
//          gPriv->ClearScriptTriggers();
//          gPriv->AddScriptTrigger( scriptId );
//        }
//        break;
//      }
//      case CREGP_SCRIPTTRIGGERS:
//      {
//        if( *vp != JS::CurrentGlobalOrNull )
//        {
//          UI16 scriptId = static_cast<UI16>( encaps.toInt() );
//          cScript *toExecute	= JSMapping->GetScript( scriptId );
//          if( toExecute == nullptr )
//          {
//            ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
//          }
//          else
//          {
//            gPriv->AddScriptTrigger( scriptId );
//          }
//        }
//        else
//        {
//          // If null value was provided, clear script triggers on object
//          gPriv->ClearScriptTriggers();
//        }
//        break; 
//      }
//      case CREGP_APPEARANCE:			gPriv->SetAppearance( static_cast<WorldType>( encaps.toInt() )); break;
//  // Active script-context might have been lost, so restore it...
//  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
//  {
//    // ... by calling a dummy function in original script!
//    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
//    if( retVal == false )
//    {
//      // Dummy function not found, let shard admin know!
//      Console.Warning( oldstrutil::format( "Script context lost after setting Region property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
//    }
//  }

IMPL_GETS( CSpawnRegion, name,        CSpawnRegion, setString,  GetName().c_str() )
IMPL_GET(  CSpawnRegion, regionNum,   CSpawnRegion, setInt32,   GetRegionNum() )
//IMPL_GET(  CSpawnRegion, itemList,    CSpawnRegion, setInt32,  GetName().c_str() )
//IMPL_GET(  CSpawnRegion, npcList,     CSpawnRegion, setInt32,  GetName().c_str() )
//IMPL_GET(  CSpawnRegion, item,        CSpawnRegion, setInt32,  GetName().c_str() )
//IMPL_GET(  CSpawnRegion, npc,         CSpawnRegion, setInt32,  GetName().c_str() )
IMPL_GET(  CSpawnRegion, maxItems,    CSpawnRegion, setInt32,   GetMaxItemSpawn() )
IMPL_GET(  CSpawnRegion, maxNpcs,     CSpawnRegion, setInt32,   GetMaxCharSpawn() )
IMPL_GET(  CSpawnRegion, itemCount,   CSpawnRegion, setInt32,   GetCurrentItemAmt() )
IMPL_GET(  CSpawnRegion, npcCount,    CSpawnRegion, setInt32,   GetCurrentCharAmt() )
IMPL_GET(  CSpawnRegion, onlyOutside, CSpawnRegion, setBoolean, GetOnlyOutside())
IMPL_GET(  CSpawnRegion, isSpawner,   CSpawnRegion, setBoolean, IsSpawner() )
IMPL_GET(  CSpawnRegion, defZ,        CSpawnRegion, setInt32,  GetDefZ() )
IMPL_GET(  CSpawnRegion, prefZ,       CSpawnRegion, setInt32,  GetPrefZ() )
IMPL_GET(  CSpawnRegion, x1,          CSpawnRegion, setInt32,  GetX1() )
IMPL_GET(  CSpawnRegion, y1,          CSpawnRegion, setInt32,  GetY1() )
IMPL_GET(  CSpawnRegion, x2,          CSpawnRegion, setInt32,  GetX2() )
IMPL_GET(  CSpawnRegion, y2,          CSpawnRegion, setInt32,  GetY2() )
IMPL_GET(  CSpawnRegion, world,       CSpawnRegion, setInt32,  WorldNumber() )
IMPL_GET(  CSpawnRegion, instanceID,  CSpawnRegion, setInt32,  GetInstanceId() )
IMPL_GET(  CSpawnRegion, minTime,     CSpawnRegion, setInt32,  GetMinTime() )
IMPL_GET(  CSpawnRegion, maxTime,     CSpawnRegion, setInt32,  GetMaxTime() )
IMPL_GET(  CSpawnRegion, call,        CSpawnRegion, setInt32,  GetCall() )

//      case CSPAWNREGP_ITEM:
//      case CSPAWNREGP_ITEMLIST:
//      {
//        // This could potentially be a list of item ids - let's convert it to a comma-separated string!
//        auto itemList = gPriv->GetItem();
//        std::string s;
//        for( const auto &piece : itemList )
//        {
//          if( s.empty() )
//          {
//            s += piece;
//          }
//          else
//          {
//            s += ( "," + piece );
//          }
//        }
//        tString = JS_NewStringCopyZ( cx, s.c_str() );
//        *vp = JS::StringValue( tString );
//      }
//        break;
//      case CSPAWNREGP_NPC:
//      case CSPAWNREGP_NPCLIST:
//      {
//        // This could potentially be a list of NPC ids - let's convert it to a comma-separated string!
//        auto  npcList = gPriv->GetNPC();
//        std::string s;
//        for( const auto &piece : npcList )
//        {
//          if( s.empty() )
//          {
//            s += piece;
//          }
//          else
//          {
//            s += ( "," + piece );
//          }
//        }
//        tString = JS_NewStringCopyZ( cx, s.c_str() );
//        *vp = JS::StringValue( tString );
//      }
//        break;

// clang-format off
IMPL_SETS( CSpawnRegion, name,        CSpawnRegion, toString,  SetName )
IMPL_SET(  CSpawnRegion, regionNum,   CSpawnRegion, toInt32,   SetRegionNum )
IMPL_SETS( CSpawnRegion, itemList,    CSpawnRegion, toString,  SetItemList )
IMPL_SETS( CSpawnRegion, npcList,     CSpawnRegion, toString,  SetNPCList )
IMPL_SETS( CSpawnRegion, item,        CSpawnRegion, toString,  SetItem )
IMPL_SETS( CSpawnRegion, npc,         CSpawnRegion, toString,  SetNPC )
IMPL_SET(  CSpawnRegion, maxItems,    CSpawnRegion, toInt32,   SetMaxItemSpawn )
IMPL_SET(  CSpawnRegion, maxNpcs,     CSpawnRegion, toInt32,   SetMaxCharSpawn )
IMPL_SET(  CSpawnRegion, onlyOutside, CSpawnRegion, toBoolean, SetOnlyOutside )
IMPL_SET(  CSpawnRegion, isSpawner,   CSpawnRegion, toBoolean, IsSpawner )
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
// clang-format on


// clang-format off
IMPL_GETS( CGuild, name,         CGuild, setString, Name().c_str() )
IMPL_GET(  CGuild, type,         CGuild, setInt32,  Type() )
//IMPL_GET(  CGuild, master,       CGuild, setInt32,  )
//IMPL_GET(  CGuild, stone,        CGuild, setInt32,  )
IMPL_GET(  CGuild, numMembers,   CGuild, setInt32,  NumMembers() )
IMPL_GET(  CGuild, numRecruits,  CGuild, setInt32,  NumRecruits() )
//IMPL_GET(  CGuild, members,      CGuild, setInt32,  )
//IMPL_GET(  CGuild, recruits,     CGuild, setInt32,  )
IMPL_GETS( CGuild, charter,      CGuild, setString, Charter().c_str() )
IMPL_GETS( CGuild, abbreviation, CGuild, setString, Abbreviation().c_str() )
IMPL_GETS( CGuild, webPage,      CGuild, setString, Webpage().c_str() )
// clang-format on

//      case CGP_MASTER:
//        CChar *gMaster;
//        gMaster = CalcCharObjFromSer( gPriv->Master() );
//
//        if( !ValidateObject( gMaster ))
//        {
//          *vp = JS::CurrentGlobalOrNull;
//        }
//        else
//        {
//          // Otherwise Acquire an object
//          JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, gMaster, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
//          *vp = OBJECT_TO_JSVAL( myChar );
//        }
//        break;
//      case CGP_STONE:
//        CItem *gStone;
//        gStone = CalcItemObjFromSer( gPriv->Stone() );
//
//        if( !ValidateObject( gStone ))
//        {
//          *vp = JS::CurrentGlobalOrNull;
//        }
//        else
//        {
//          // Otherwise Acquire an object
//          JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, gStone, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
//          *vp = OBJECT_TO_JSVAL( myItem );
//        }
//        break;
//      case CGP_MEMBERS:
//      case CGP_RECRUITS:
//        break;

// clang-format off
IMPL_SETS( CGuild, name,         CGuild, toString, Name )
//IMPL_SET(  CGuild, type,         CGuild, toInt32,  Type )
//IMPL_SET(  CGuild, master,       CGuild, toInt32,  )
//IMPL_SET(  CGuild, stone,        CGuild, toInt32,  )
//IMPL_SET(  CGuild, members,      CGuild, toInt32,  )
//IMPL_SET(  CGuild, recruits,     CGuild, toInt32,  )
IMPL_SETS( CGuild, charter,      CGuild, toString, Charter )
IMPL_SETS( CGuild, abbreviation, CGuild, toString, Abbreviation )
IMPL_SETS( CGuild, webPage,      CGuild, toString, Webpage )
// clang-format on

//      case CGP_TYPE:				gPriv->Type( static_cast<GuildType>( encaps.toInt() ));	break;
//      case CGP_MASTER:
//        if( *vp != JS::CurrentGlobalOrNull )
//        {
//          CChar *myChar = static_cast<CChar*>( encaps.toObject() );
//          if( !ValidateObject( myChar ))
//            break;
//
//          gPriv->Master(( *myChar ));
//        }
//        else
//        {
//          gPriv->Master( INVALIDSERIAL );
//        }
//        break;
//      case CGP_STONE:
//        if( *vp != JS::CurrentGlobalOrNull )
//        {
//          CItem *myItem = static_cast<CItem *>( encaps.toObject() );
//          if( !ValidateObject( myItem ))
//            break;
//
//          gPriv->Stone(( *myItem ));
//        }
//        else
//        {
//          gPriv->Stone( INVALIDSERIAL );
//        }
//        break;
//      case CGP_MEMBERS:
//      case CGP_RECRUITS:
//        break;

//IMPL_GET( CRace, id, CRace, setInt32, id )
bool JSCRace_get_id(JSContext *cx, unsigned int argc, JS::Value *vp) {
  auto args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject thisObj(cx);
  if( !args.computeThis( cx, &thisObj ) )
      return false;
  auto priv = JS::GetMaybePtrFromReservedSlot<CRace>(thisObj, 0);
  for (auto i = 0; i < Races->Count(); ++i) {
    if ( Races->Race( i ) == priv ) {
      args.rval().setInt32(i);
      return true;
    }
  }
  return false;
}

// clang-format off
IMPL_GETS( CRace, name,             CRace, setString, Name().c_str() )
IMPL_GET(  CRace, requiresBeard,    CRace, setBoolean, RequiresBeard() )
IMPL_GET(  CRace, requiresNoBeard,  CRace, setBoolean, NoBeard() )
IMPL_GET(  CRace, isPlayerRace,     CRace, setBoolean, IsPlayerRace() )
IMPL_GET(  CRace, genderRestrict,   CRace, setInt32,   GenderRestriction() )
IMPL_GET(  CRace, armourClass,      CRace, setInt32,   ArmourClassRestriction() )
IMPL_GET(  CRace, languageSkillMin, CRace, setInt32,   LanguageMin() )
IMPL_GET(  CRace, poisonResistance, CRace, setDouble,  PoisonResistance() )
IMPL_GET(  CRace, magicResistance,  CRace, setDouble,  MagicResistance() )
IMPL_GET(  CRace, visibleDistance,  CRace, setInt32,   VisibilityRange() )
IMPL_GET(  CRace, nightVision,      CRace, setInt32,   NightVision() )
// clang-format on

// clang-format off
IMPL_SETS( CRace, name,             CRace, toString,  Name )
IMPL_SET(  CRace, requiresBeard,    CRace, toBoolean, RequiresBeard )
IMPL_SET(  CRace, requiresNoBeard,  CRace, toBoolean, NoBeard )
IMPL_SET(  CRace, isPlayerRace,     CRace, toBoolean, IsPlayerRace )
IMPL_SET(  CRace, genderRestrict,   CRace, toInt32,   GenderRestriction )
IMPL_SET(  CRace, armourClass,      CRace, toInt32,   ArmourClassRestriction )
IMPL_SET(  CRace, languageSkillMin, CRace, toInt32,   LanguageMin )
IMPL_SET(  CRace, poisonResistance, CRace, toDouble,  PoisonResistance )
IMPL_SET(  CRace, magicResistance,  CRace, toDouble,  MagicResistance )
IMPL_SET(  CRace, visibleDistance,  CRace, toInt32,   VisibilityRange )
IMPL_SET(  CRace, nightVision,      CRace, toInt32,   NightVision )
// clang-format on

// clang-format off
//IMPL_SET(  CSocket, currentChar,      CSocket, toString, )
IMPL_SET(  CSocket, wasIdleWarned,    CSocket, toBoolean,  WasIdleWarned )
IMPL_SET(  CSocket, tempInt,          CSocket, toInt32,    TempInt  )
IMPL_SET(  CSocket, tempInt2,         CSocket, toInt32,    TempInt2 )
IMPL_SETS( CSocket, xText,            CSocket, toString,   XText )
IMPL_SETS( CSocket, xText2,           CSocket, toString,   XText2 )
IMPL_SET(  CSocket, clickZ,           CSocket, toInt32,    ClickZ )
IMPL_SET(  CSocket, newClient,        CSocket, toBoolean,  NewClient )
IMPL_SET(  CSocket, firstPacket,      CSocket, toBoolean,  FirstPacket )
IMPL_SET(  CSocket, cryptClient,      CSocket, toBoolean,  CryptClient )
IMPL_SET(  CSocket, walkSequence,     CSocket, toInt32,    WalkSequence )
IMPL_SET(  CSocket, currentSpellType, CSocket, toInt32,    CurrentSpellType )
IMPL_SET(  CSocket, logging,          CSocket, toBoolean,  Logging )
IMPL_SET(  CSocket, targetOK,         CSocket, toBoolean,  TargetOK )
IMPL_SET(  CSocket, clickX,           CSocket, toInt32,    ClickX )
IMPL_SET(  CSocket, clickY,           CSocket, toInt32,    ClickY )
IMPL_SET(  CSocket, pickupX,          CSocket, toInt32,    PickupX )
IMPL_SET(  CSocket, pickupY,          CSocket, toInt32,    PickupY )
IMPL_SET(  CSocket, pickupZ,          CSocket, toInt32,    PickupZ )
//IMPL_SET(  CSocket, pickupSpot,       CSocket, toString, )
//IMPL_SET(  CSocket, language,         CSocket, toString, )
IMPL_SET(  CSocket, clientMajorVer,   CSocket, toInt32,    ClientVersionMajor )
IMPL_SET(  CSocket, clientMinorVer,   CSocket, toInt32,    ClientVersionMinor )
IMPL_SET(  CSocket, clientSubVer,     CSocket, toInt32,    ClientVersionSub )
IMPL_SET(  CSocket, clientLetterVer,  CSocket, toInt32,    ClientVersionLetter )
//IMPL_SET(  CSocket, clientType,       CSocket, toInt32, )
//IMPL_SET(  CSocket, tempObj,          CSocket, toString, ) // Todo
// IMPL_SET(  CSocket, tempObj2,         CSocket, toString, ) // Todo
// clang-format on



      //case CSOCKP_CURRENTCHAR:
      //{
      //  CChar *mChar = static_cast<CChar *>( encaps.toObject() );
      //  if( ValidateObject( mChar ))
      //  {
      //    gPriv->CurrcharObj( mChar );
      //  }
      //}
      //  break;
      //case CSOCKP_TEMPOBJ:
      //  if( *vp == JS::CurrentGlobalOrNull )
      //  {
      //    gPriv->TempObj( nullptr );
      //  }
      //  else
      //  {
      //    gPriv->TempObj( static_cast<CBaseObject *>( encaps.toObject() ));
      //  }
      //  break;
      //case CSOCKP_TEMPOBJ2: // Reserved for JS usage
      //  if( *vp == JS::CurrentGlobalOrNull )
      //  {
      //    gPriv->TempObj2( nullptr );
      //  }
      //  else
      //  {
      //    gPriv->TempObj2( static_cast<CBaseObject *>( encaps.toObject() ));
      //  }
      //  break;
      //case CSOCKP_PICKUPSPOT:			gPriv->PickupSpot( static_cast<PickupLocations>( encaps.toInt() ));	break;
      //case CSOCKP_LANGUAGE:			    gPriv->Language( static_cast<UnicodeTypes>( encaps.toInt() ));		break;
      //case CSOCKP_CLIENTTYPE:			gPriv->ClientType( static_cast<ClientTypes>( encaps.toInt() ));		break;

// clang-format off
//IMPL_GET(  CSocket, account,          CSocket, setInt32, PickupZ )
//IMPL_GET(  CSocket, currentChar,      CSocket, setInt32, PickupZ )
IMPL_GET(  CSocket, idleTimeout,      CSocket, setInt32,   IdleTimeout() )
IMPL_GET(  CSocket, wasIdleWarned,    CSocket, setBoolean, WasIdleWarned() )
IMPL_GET(  CSocket, tempInt,          CSocket, setInt32,   TempInt() )
IMPL_GET(  CSocket, tempInt2,         CSocket, setInt32,   TempInt2() )
//IMPL_GET(  CSocket, buffer,           CSocket, setInt32, PickupZ )
IMPL_GETS( CSocket, xText,            CSocket, setString,  XText().c_str() )
IMPL_GETS( CSocket, xText2,           CSocket, setString,  XText2().c_str() )
IMPL_GET(  CSocket, clickZ,           CSocket, setInt32,   ClickZ() )
IMPL_GET(  CSocket, addID,            CSocket, setInt32,   AddId() )
IMPL_GET(  CSocket, newClient,        CSocket, setBoolean, NewClient() )
IMPL_GET(  CSocket, firstPacket,      CSocket, setBoolean, FirstPacket() )
IMPL_GET(  CSocket, cryptClient,      CSocket, setBoolean, CryptClient() )
//IMPL_GET(  CSocket, clientIP,         CSocket, setInt32, PickupZ )
IMPL_GET(  CSocket, walkSequence,     CSocket, setInt32,   WalkSequence() )
IMPL_GET(  CSocket, currentSpellType, CSocket, setInt32,   CurrentSpellType() )
IMPL_GET(  CSocket, logging,          CSocket, setBoolean, Logging() )
IMPL_GET(  CSocket, bytesSent,        CSocket, setInt32,   BytesSent() )
IMPL_GET(  CSocket, bytesReceived,    CSocket, setInt32,   BytesReceived() )
IMPL_GET(  CSocket, targetOK,         CSocket, setBoolean, TargetOK() )
IMPL_GET(  CSocket, clickX,           CSocket, setInt32,   ClickX() )
IMPL_GET(  CSocket, clickY,           CSocket, setInt32,   ClickY() )
IMPL_GET(  CSocket, pickupX,          CSocket, setInt32,   PickupX() )
IMPL_GET(  CSocket, pickupY,          CSocket, setInt32,   PickupY() )
IMPL_GET(  CSocket, pickupZ,          CSocket, setInt32,   PickupZ() )
IMPL_GET(  CSocket, pickupSpot,       CSocket, setInt32,   PickupSpot() )
IMPL_GET(  CSocket, pickupSerial,     CSocket, setInt32,   PickupSerial() )
IMPL_GET(  CSocket, language,         CSocket, setInt32,   Language() )
IMPL_GET(  CSocket, clientMajorVer,   CSocket, setInt32,   ClientVersionMajor() )
IMPL_GET(  CSocket, clientMinorVer,   CSocket, setInt32,   ClientVersionMinor() )
IMPL_GET(  CSocket, clientSubVer,     CSocket, setInt32,   ClientVersionSub() )
IMPL_GET(  CSocket, clientLetterVer,  CSocket, setInt32,   ClientVersionLetter() )
IMPL_GET(  CSocket, clientType,       CSocket, setInt32,   ClientType() )
//IMPL_GET(  CSocket, target,           CSocket, setInt32, PickupZ )
//IMPL_GET(  CSocket, tempObj,          CSocket, setInt32, PickupZ )
//IMPL_GET(  CSocket, tempObj2,         CSocket, setInt32, PickupZ )
// clang-format on

      //case CSOCKP_ACCOUNT:
      //{
      //  CAccountBlock_st *accountBlock = &gPriv->GetAccount();
      //  if( accountBlock == nullptr )
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  else
      //  {	// Otherwise Acquire an object
      //    JSObject *accountObj	= JSEngine->AcquireObject( IUE_ACCOUNT, accountBlock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    *vp = OBJECT_TO_JSVAL( accountObj );
      //  }
      //  break;
      //}
      //case CSOCKP_CURRENTCHAR:
      //{
      //  myChar = gPriv->CurrcharObj();
      //  if( !ValidateObject( myChar ))
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  else
      //  {
      //    JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    *vp = OBJECT_TO_JSVAL( myObj );
      //  }
      //  break;
      //}
      //case CSOCKP_IDLETIMEOUT:
      //  break;
      //case CSOCKP_TEMPOBJ:
      //{
      //  CBaseObject *mObj	= gPriv->TempObj();
      //  if( !ValidateObject( mObj ))
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  else
      //  {
      //    JSObject *myObj = nullptr;
      //    if( mObj->CanBeObjType( OT_ITEM ))
      //    {
      //      myObj = JSEngine->AcquireObject( IUE_ITEM, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    }
      //    else
      //    {
      //      myObj = JSEngine->AcquireObject( IUE_CHAR, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    }
      //    *vp = OBJECT_TO_JSVAL( myObj );
      //  }
      //  break;
      //}
      //case CSOCKP_TEMPOBJ2:
      //{
      //  CBaseObject *mObj	= gPriv->TempObj2();
      //  if( !ValidateObject( mObj ))
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  else
      //  {
      //    JSObject *myObj = nullptr;
      //    if( mObj->CanBeObjType( OT_ITEM ))
      //    {
      //      myObj = JSEngine->AcquireObject( IUE_ITEM, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    }
      //    else
      //    {
      //      myObj = JSEngine->AcquireObject( IUE_CHAR, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    }
      //    *vp = OBJECT_TO_JSVAL( myObj );
      //  }
      //}
      //  break;
      //case CSOCKP_CLIENTIP:
      //  break;
      //case CSOCKP_TARGET:
      //{
      //  SERIAL mySerial		= gPriv->GetDWord( 7 );
      //  // Item
      //  if( mySerial >= BASEITEMSERIAL )
      //  {
      //    CItem *myItem = CalcItemObjFromSer( mySerial );

      //    if( !ValidateObject( myItem ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //      return true;
      //    }

      //    JSObject *myObj = JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    *vp = OBJECT_TO_JSVAL( myObj );
      //  }
      //  // Char
      //  else
      //  {
      //    CChar *myChar = CalcCharObjFromSer( mySerial );

      //    if( !ValidateObject( myChar ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //      return true;
      //    }

      //    JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //    *vp = OBJECT_TO_JSVAL( myObj );
      //  }

      //  return true;
      //}

bool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  JSEncapsulate myClass( cx, obj );
  CChar *myChar = static_cast<CChar*>( myClass.toObject() );

  if( !ValidateObject( myChar ))
    return false;

  UI08 skillId		= static_cast<UI08>( JSVAL_TO_INT( id ));

  if( myClass.ClassName() == "UOXSkills" )
  {
    *vp = JS::Int32Value( myChar->GetSkill( skillId ));
  }
  else if( myClass.ClassName() == "UOXBaseSkills" )
  {
    *vp = JS::Int32Value( myChar->GetBaseSkill( skillId ));
  }
  else if( myClass.ClassName() == "UOXSkillsUsed" )
  {
    vp.setBoolean( myChar->SkillUsed( skillId ));
  }
  else if( myClass.ClassName() == "UOXSkillsLock" )
  {
    *vp = JS::Int32Value( static_cast<UI08>( myChar->GetSkillLock( skillId )));
  }

  return true;
}

bool CSkillsProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  JSEncapsulate myClass( cx, obj );
  CChar *myChar = static_cast<CChar*>( myClass.toObject() );

  if( !ValidateObject( myChar ))
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  UI08 skillId		= static_cast<UI08>( JSVAL_TO_INT( id ));
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Skill property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

// clang-format off
IMPL_GET(  CGumpData, buttons,  SEGumpData_st, setInt32, nIDs.size() )
IMPL_GET(  CGumpData, IDs,      SEGumpData_st, setInt32, nButtons.size() )
// clang-format on

// clang-format off
IMPL_GET( CAccount, id,             CAccountBlock_st, setInt32,  wAccountIndex )
IMPL_GETS(CAccount, username,       CAccountBlock_st, setString, sUsername.c_str() )
IMPL_GET( CAccount, flags,          CAccountBlock_st, setInt32,  wFlags.to_ulong() )
IMPL_GETS(CAccount, comment,        CAccountBlock_st, setString, sContact.c_str() )
IMPL_GET( CAccount, timeban,        CAccountBlock_st, setInt32, wTimeBan)
IMPL_GET( CAccount, firstLogin,     CAccountBlock_st, setInt32, wFirstLogin)
IMPL_GET( CAccount, totalPlayTime,  CAccountBlock_st, setInt32, wTotalPlayTime)
//IMPL_GET( CAccount, character1,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, character2,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, character3,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, character4,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, character5,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, character6,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, character7,     CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, currentChar,    CAccountBlock_st, setInt32,  wAccountIndex ) // manual implementation
//IMPL_GET( CAccount, lastIP,         CAccountBlock_st, setInt32,  wAccountIndex )
IMPL_GET( CAccount, isBanned,       CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_BANNED ) )
IMPL_GET( CAccount, isSuspended,    CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_SUSPENDED ) )
IMPL_GET( CAccount, isPublic,       CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_PUBLIC ) )
IMPL_GET( CAccount, isOnline,       CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_ONLINE ) )
IMPL_GET( CAccount, isSlot1Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER1 ) )
IMPL_GET( CAccount, isSlot2Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER2 ) )
IMPL_GET( CAccount, isSlot3Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER3 ) )
IMPL_GET( CAccount, isSlot4Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER4 ) )
IMPL_GET( CAccount, isSlot5Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER5 ) )
IMPL_GET( CAccount, isSlot6Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER6 ) )
IMPL_GET( CAccount, isSlot7Blocked, CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_CHARACTER7 ) )
IMPL_GET( CAccount, isYoung,        CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_YOUNG ) )
IMPL_GET( CAccount, unused10,       CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_UNUSED10 ) )
IMPL_GET( CAccount, isSeer,         CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_SEER ) )
IMPL_GET( CAccount, isCounselor,    CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_COUNSELOR ) )
IMPL_GET( CAccount, isGM,           CAccountBlock_st, setBoolean,  wFlags.test( AB_FLAGS_GM ) )
// clang-format on

      //case CACCOUNT_CHARACTER1:
      //{
      //  if( myAccount->dwCharacters[0] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[0];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CHARACTER2:
      //{
      //  if( myAccount->dwCharacters[1] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[1];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CHARACTER3:
      //{
      //  if( myAccount->dwCharacters[2] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[2];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CHARACTER4:
      //{
      //  if( myAccount->dwCharacters[3] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[3];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CHARACTER5:
      //{
      //  if( myAccount->dwCharacters[4] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[4];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CHARACTER6:
      //{
      //  if( myAccount->dwCharacters[5] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[5];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CHARACTER7:
      //{
      //  if( myAccount->dwCharacters[6] != INVALIDSERIAL )
      //  {
      //    CBaseObject *TempObj = myAccount->lpCharacters[6];

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_CURRENTCHAR:
      //{
      //  if( myAccount->dwInGame != INVALIDSERIAL )
      //  {
      //    CChar *TempObj = CalcCharObjFromSer( myAccount->dwInGame );

      //    if( !ValidateObject( TempObj ))
      //    {
      //      *vp = JS::CurrentGlobalOrNull;
      //    }
      //    else
      //    {
      //      // Otherwise Acquire an object
      //      JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
      //      *vp = OBJECT_TO_JSVAL( myChar );
      //    }
      //  }
      //  else
      //  {
      //    *vp = JS::CurrentGlobalOrNull;
      //  }
      //  break;
      //}
      //case CACCOUNT_LASTIP:
      //{
      //  std::string ipString = oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0xFF000000 ) >> 24 )) 
      //    + "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x00FF0000 ) >> 16 ))
      //    + "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x0000FF00 ) >> 8 ))
      //    + "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x000000FF ) % 256 ));
      //  tString = JS_NewStringCopyZ( cx, ipString.c_str() );
      //  *vp = JS::StringValue( tString );
      //  break;
      //}

bool CAccountProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CAccountBlock_st *myAccount = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st >(obj , 0);
  if( myAccount == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );

  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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
          return false;
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Account property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

// clang-format off
IMPL_GET_NP( CConsole, mode,    setInt32, Console.CurrentMode() )
IMPL_GET_NP( CConsole, logEcho, setInt32, Console.LogEcho()     )
IMPL_SET_NP( CConsole, mode,    toInt32,  Console.CurrentMode )
IMPL_SET_NP( CConsole, logEcho, toInt32,  Console.CurrentMode )
// clang-format on

// clang-format off
IMPL_GET( CScriptSection, numTags,    CScriptSection,   setInt32, NumEntries() )
IMPL_GET( CScriptSection, atEnd,      CScriptSection, setBoolean, AtEnd() )
IMPL_GET( CScriptSection, atEndTags,  CScriptSection, setBoolean, AtEndTags() )
// clang-format on

// clang-format off
IMPL_SET_DIR(  CResource, logAmount,  MapResource_st, toInt32, logAmt )
//IMPL_SET_DIR(  CResource, logTime,    MapResource_st, toInt32, )
IMPL_SET_DIR(  CResource, oreAmount,  MapResource_st, toInt32, oreAmt )
//IMPL_SET_DIR(  CResource, oreTime,    MapResource_st, toInt32, )
IMPL_SET_DIR(  CResource, fishAmount, MapResource_st, toInt32, fishAmt )
//IMPL_SET_DIR(  CResource, fishTime,   MapResource_st, toInt32, )
// clang-format on

      //case CRESP_LOGTIME:
      //{
      //  gPriv->logTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
      //  break;
      //}
      //case CRESP_ORETIME:				//gPriv->oreTime	= encaps.toInt();			break;
      //{
      //  gPriv->oreTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
      //  break;
      //}
      //case CRESP_FISHTIME:			//gPriv->fishTime	= encaps.toInt();			break;
      //{
      //  gPriv->fishTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
      //  break;
      //}

// clang-format off
IMPL_GET( CResource, logAmount , MapResource_st, setInt32,         logAmt   )
IMPL_GET( CResource, logTime,    MapResource_st, setPrivateUint32, logTime  )
IMPL_GET( CResource, oreAmount , MapResource_st, setInt32,         oreAmt   )
IMPL_GET( CResource, oreTime,    MapResource_st, setPrivateUint32, oreTime  )
IMPL_GET( CResource, fishAmount, MapResource_st, setInt32,         fishAmt  )
IMPL_GET( CResource, fishTime,   MapResource_st, setPrivateUint32, fishTime )
// clang-format on

// clang-format off
//IMPL_GET(  CParty, leader,      Party, setInt32, )
IMPL_GET(  CParty, memberCount, Party, setInt32,   MemberList()->size() )
IMPL_GET(  CParty, isNPC,       Party, setBoolean, IsNPC() )

IMPL_SET(  CParty, isNPC,       Party, toBoolean,  IsNPC )
// clang-format on

      //case CPARTYP_LEADER:
      //{
      //  if( encaps.ClassName() == "UOXChar" || encaps.ClassName() == "UOXSocket" )
      //  {
      //    CChar *newLeader = nullptr;
      //    if( encaps.ClassName() == "UOXChar" )
      //    {
      //      newLeader = static_cast<CChar *>( encaps.toObject() );
      //    }
      //    else
      //    {
      //      CSocket *tempSock = static_cast<CSocket *>( encaps.toObject() );
      //      if( tempSock != nullptr )
      //      {
      //        newLeader = tempSock->CurrcharObj();
      //      }
      //    }
      //    if( ValidateObject( newLeader ))
      //    {
      //      gPriv->Leader( newLeader );
      //    }
      //  }
      //}

bool CPartyProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  Party *gPriv = JS::GetMaybePtrFromReservedSlot<Party >(obj , 0);
  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
    {
      case CPARTYP_LEADER:
      {
        CChar *myChar = gPriv->Leader();
        if( !ValidateObject( myChar ))
        {
          *vp = JS::CurrentGlobalOrNull;
          return true;
        }
        JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
        *vp = OBJECT_TO_JSVAL( myObj );
      }
        break;
      case CPARTYP_MEMBERCOUNT:	*vp = JS::Int32Value( gPriv->MemberList()->size() );			break;
      case CPARTYP_ISNPC:			vp.setBoolean( gPriv->IsNPC() );					break;
      default:																				break;
    }
  }
  return true;
}

bool CScriptProps_getProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, jsval id, jsval *vp )
{
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
    {
      case CSCRIPT_SCRIPTID:		
        *vp = JS::Int32Value( JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx )) );
        break;
      default:
        break;
    }
  }
  return true;
}

bool CSocket_equality( JSContext *cx, JSObject *obj, jsval v, bool *bp )
{
  JSEncapsulate srcObj( cx, obj );
  CSocket *srcSock = static_cast<CSocket *>( srcObj.toObject() );
  JSEncapsulate trgObj( cx, &v );
  if( trgObj.isType( JSOT_OBJECT ))
  {
    if( srcObj.ClassName() != trgObj.ClassName() )
    {
      *bp = false;
    }
    else
    {
      CSocket *trgSock = static_cast<CSocket *>( trgObj.toObject() );
      *bp = ( srcSock == trgSock ) ? true : false;
    }
  }
  else
  {
    *bp = ( srcSock == nullptr && trgObj.isType( JSOT_NULL )) ? true : false;
  }
  return true;
}
bool CBaseObject_equality( JSContext *cx, JSObject *obj, jsval v, bool *bp )
{
  JSEncapsulate srcObj( cx, obj );
  CBaseObject *src = static_cast<CBaseObject *>( srcObj.toObject() );
  if( !ValidateObject( src ))
  {
    *bp = false;
  }
  else
  {
    JSEncapsulate trgObj( cx, &v );
    if( trgObj.isType( JSOT_OBJECT ))
    {
      if( srcObj.ClassName() != trgObj.ClassName() )
      {
        *bp = false;
      }
      else
      {
        CBaseObject *trg = static_cast<CBaseObject *>( trgObj.toObject() );
        if( !ValidateObject( trg ))
        {
          *bp = false;
        }
        else	// both valid base objects!  Now, we'll declare equality based on SERIAL, not pointer
        {
          *bp = ( src->GetSerial() == trg->GetSerial() ) ? true : false;
        }
      }
    }
    else
    {
      *bp = ( src == nullptr && trgObj.isType( JSOT_NULL )) ? true : false;
    }
  }
  return true;
}
bool CParty_equality( JSContext *cx, JSObject *obj, jsval v, bool *bp )
{
  JSEncapsulate srcObj( cx, obj );
  Party *srcParty = static_cast<Party *>( srcObj.toObject() );
  JSEncapsulate trgObj( cx, &v );
  if( trgObj.isType( JSOT_OBJECT ))
  {
    if( srcObj.ClassName() != trgObj.ClassName() )
    {
      *bp = false;
    }
    else
    {
      Party *trgParty	= static_cast<Party *>( trgObj.toObject() );
      *bp = ( srcParty == trgParty ) ? true : false;
    }
  }
  else
  {
    *bp = ( srcParty == nullptr && trgObj.isType( JSOT_NULL )) ? true : false;
  }
  return true;
}

