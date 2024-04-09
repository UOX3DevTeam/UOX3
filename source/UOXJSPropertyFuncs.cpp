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

void MakeShop( CChar *c );
void ScriptError( JSContext *cx, const char *txt, ... );

/*
bool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  size_t spellId = JSVAL_TO_INT( id );

  if( spellId >= Magic->spells.size() )
  {
    ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell ID (%i) provided", spellId ).c_str() );
    *vp = JSVAL_NULL;
    return false;
  }

  CSpellInfo *mySpell = &Magic->spells[spellId];
  if( mySpell == nullptr )
  {
    ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell with spellId %i", spellId ).c_str() );
    *vp = JSVAL_NULL;
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
    *vp = JSVAL_NULL;
    return false;
  }

  CWorldMain::Skill_st *mySkill = &cwmWorldState->skill[skillId];
  if( mySkill == nullptr )
  {
    ScriptError( cx, oldstrutil::format( "Invalid Skill" ).c_str() );
    *vp = JSVAL_NULL;
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
    *vp = JSVAL_NULL;
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
        amountNeeded = INT_TO_JSVAL( resEntry.amountNeeded );
        targColour = INT_TO_JSVAL( resEntry.colour );

        // Add each required resource as its own object
        JSObject *resource = JS_NewArrayObject( cx, 0, nullptr );
        JS_SetElement( cx, resource, 0, &amountNeeded );
        JS_SetElement( cx, resource, 1, &targColour );

        // Make a child object with all the supported resource IDs
        JSObject *idList = JS_NewArrayObject( cx, 0, nullptr );
        for( auto j = 0; j < static_cast<int>( resEntry.idList.size() ); j++ )
        {
          jsval targId = INT_TO_JSVAL( resEntry.idList[j] );
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
        skillNumber = INT_TO_JSVAL( resEntry.skillNumber );
        minSkill = INT_TO_JSVAL( resEntry.minSkill );
        maxSkill = INT_TO_JSVAL( resEntry.maxSkill );

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
IMPL_GETS_OBJ(CItem, title,               CItem, setString, GetTitle().c_str(), )
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
          *vp = JSVAL_NULL;
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
      case CIP_ATT:			*vp = INT_TO_JSVAL( RandomNum( gPriv->GetLoDamage(), gPriv->GetHiDamage() ));	break;
      case CIP_TOTALITEMCOUNT: *vp = INT_TO_JSVAL( GetTotalItemCount( gPriv ));	break;
      case CIP_ISCHAR:	*vp = JSVAL_FALSE;								break;
      case CIP_ISITEM:	*vp = JSVAL_TRUE;								break;
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
      case CIP_ORIGIN:
        tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
        *vp = STRING_TO_JSVAL( tString );
        break;

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
      default:
        break;
    }
  }
  return true;
}
*/

bool CItemProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CItem *gPriv = JS::GetMaybePtrFromReservedSlot<CItem >(obj , 0);
  if( !ValidateObject( gPriv ))
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    TIMERVAL newTime;
    switch( JSVAL_TO_INT( id ))
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

bool CCharacterProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CChar *gPriv = JS::GetMaybePtrFromReservedSlot<CChar >(obj , 0);

  if( !ValidateObject( gPriv ))
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  if( JSVAL_IS_INT( id ))
  {
    CItem *TempItem			= nullptr;
    JSObject *TempObject	= nullptr;
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
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
        if( !INT_FITS_IN_JSVAL( gPriv->GetSerial() ))
        {
          JS_NewNumberValue( cx, gPriv->GetSerial(), vp );
        }
        else
        {
          *vp = INT_TO_JSVAL( gPriv->GetSerial() );
        }
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
        JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
        *vp = OBJECT_TO_JSVAL( TempObject );
        break;
      case CCP_SKILLS:
        TempObject = JS_NewObject( cx, &UOXSkills_class, nullptr, obj );
        JS_DefineProperties( cx, TempObject, CSkillsProps );
        JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
        *vp = OBJECT_TO_JSVAL( TempObject );
        break;
      case CCP_SKILLUSE:
        TempObject = JS_NewObject( cx, &UOXSkillsUsed_class, nullptr, obj );
        JS_DefineProperties( cx, TempObject, CSkillsProps );
        JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
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
      case CCP_SPAWNSERIAL:	*vp = INT_TO_JSVAL( gPriv->GetSpawn() );			break;
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
      case CCP_PRIV:			*vp = INT_TO_JSVAL( gPriv->GetPriv() );				break;
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
        JS::SetReservedSlot( TempObject, 0, JS::PrivateValue(gPriv) );
        *vp	= OBJECT_TO_JSVAL( TempObject );
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
      case CCP_AITYPE:		*vp = INT_TO_JSVAL( gPriv->GetNpcAiType() );				break;
      case CCP_SPLIT:			*vp = INT_TO_JSVAL( gPriv->GetSplit() );					break;
      case CCP_SPLITCHANCE:	*vp = INT_TO_JSVAL( gPriv->GetSplitChance() );				break;
      case CCP_TRAINER:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanTrain() );				break;
      case CCP_HIRELING:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanBeHired() );				break;
      case CCP_WEIGHT:		*vp = INT_TO_JSVAL( gPriv->GetWeight() );					break;
      case CCP_SQUELCH:		*vp = INT_TO_JSVAL( gPriv->GetSquelched() );				break;
      case CCP_ISJAILED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsJailed() );				break;
      case CCP_MAGICREFLECT:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsTempReflected() );			break;
      case CCP_PERMMAGICREFLECT:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPermReflected() );		break;
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
      default:
        break;
    }
  }

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    // ... but keep track of the property value we're trying to retrieve, stored in vp!
    jsval origVp = *vp;
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    *vp = origVp;
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

bool CRegionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CTownRegion *gPriv = JS::GetMaybePtrFromReservedSlot<CTownRegion >(obj , 0);
  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
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
  return true;
}
bool CRegionProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CTownRegion *gPriv = JS::GetMaybePtrFromReservedSlot<CTownRegion >(obj , 0);
  if( gPriv == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Region property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}
bool CSpawnRegionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CSpawnRegion *gPriv = JS::GetMaybePtrFromReservedSlot<CSpawnRegion >(obj , 0);

  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
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

  return true;
}
bool CSpawnRegionProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CSpawnRegion *gPriv = JS::GetMaybePtrFromReservedSlot<CSpawnRegion >(obj , 0);

  if( gPriv == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting SpawnRegion property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

bool CGuildProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CGuild *gPriv = JS::GetMaybePtrFromReservedSlot<CGuild >(obj , 0);
  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
    {
      case CGP_NAME:
        tString = JS_NewStringCopyZ( cx, gPriv->Name().c_str() );
        *vp = STRING_TO_JSVAL( tString );
        break;
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
      case CGP_MEMBERS:
      case CGP_RECRUITS:
        break;
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
  return true;
}
bool CGuildProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CGuild *gPriv = JS::GetMaybePtrFromReservedSlot<CGuild >(obj , 0);
  if( gPriv == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
    {
      case CGP_NAME:				gPriv->Name( encaps.toString() );						break;
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
      case CGP_MEMBERS:
      case CGP_RECRUITS:
        break;
      case CGP_CHARTER:			gPriv->Charter( encaps.toString() );				break;
      case CGP_ABBREVIATION:		gPriv->Abbreviation( encaps.toString().c_str() );	break;
      case CGP_WEBPAGE:			gPriv->Webpage( encaps.toString() );				break;
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
      Console.Warning( oldstrutil::format( "Script context lost after setting Guild property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

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
//IMPL_SET( CRace, name, CRace, Name, args.)
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

bool CRaceProps_setProperty(JSContext *cx, unsigned int argc, JS::Value *vp) {

  auto args = JS::CallArgsFromVp(argc, vp);
  JS::RootedObject thisObj(cx);
  if (!args.computeThis(cx, &thisObj))
    return false;
  auto priv = JS::GetMaybePtrFromReservedSlot<CRace>(thisObj, 0);

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ) );
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ) );

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
    {
      case CRP_NAME:				gPriv->Name( argsencaps.toString() );						break;
      case CRP_WEAKTOWEATHER:
        break;
      case CRP_SKILLADJUSTMENT:
        break;
      default:
        break;
    }
  }

  // Active script-context might have been lost, so restore it!
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( !retVal )
    {
      Console.Warning( oldstrutil::format( "Script context lost after setting Race property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

bool CSocketProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CSocket *gPriv = JS::GetMaybePtrFromReservedSlot<CSocket >(obj , 0);
  if( gPriv == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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
      case CSOCKP_BUFFER:
        break;
      case CSOCKP_XTEXT:				gPriv->XText( encaps.toString() );						break;
      case CSOCKP_XTEXT2:				gPriv->XText2( encaps.toString() );						break;
      case CSOCKP_CLICKZ:				gPriv->ClickZ( static_cast<SI08>( encaps.toInt() ));	break;
      case CSOCKP_ADDID:
        break;
      case CSOCKP_NEWCLIENT:			gPriv->NewClient( encaps.toBool() );					break;
      case CSOCKP_FIRSTPACKET:		gPriv->FirstPacket( encaps.toBool() );					break;
      case CSOCKP_CRYPTCLIENT:		gPriv->CryptClient( encaps.toBool() );					break;
      case CSOCKP_CLIENTIP:
        break;
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
      Console.Warning( oldstrutil::format( "Script context lost after setting Socket property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

bool CSocketProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CSocket *gPriv = JS::GetMaybePtrFromReservedSlot<CSocket >(obj , 0);
  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    CChar *myChar;
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
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
      case CSOCKP_BUFFER:
        break;
      case CSOCKP_XTEXT:				
        tString = JS_NewStringCopyZ( cx, gPriv->XText().c_str() );
        *vp = STRING_TO_JSVAL( tString );	break;
      case CSOCKP_XTEXT2:				
        tString = JS_NewStringCopyZ( cx, gPriv->XText2().c_str() );
        *vp = STRING_TO_JSVAL( tString );	break;
      case CSOCKP_CLICKZ:				*vp = INT_TO_JSVAL( gPriv->ClickZ() );					break;
      case CSOCKP_ADDID:
        break;
      case CSOCKP_NEWCLIENT:			*vp = BOOLEAN_TO_JSVAL( gPriv->NewClient() );			break;
      case CSOCKP_FIRSTPACKET:		*vp = BOOLEAN_TO_JSVAL( gPriv->FirstPacket() );			break;
      case CSOCKP_CRYPTCLIENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->CryptClient() );			break;
      case CSOCKP_CLIENTIP:
        break;
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
            return true;
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
            return true;
          }

          JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
          *vp = OBJECT_TO_JSVAL( myObj );
        }

        return true;
      }
        break;
      default:
        break;
    }
  }
  return true;
}

bool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  JSEncapsulate myClass( cx, obj );
  CChar *myChar = static_cast<CChar*>( myClass.toObject() );

  if( !ValidateObject( myChar ))
    return false;

  UI08 skillId		= static_cast<UI08>( JSVAL_TO_INT( id ));

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

bool CGumpDataProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  SEGumpData_st *gPriv = JS::GetMaybePtrFromReservedSlot<SEGumpData_st >(obj , 0);

  if( gPriv == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
    {
      case CGumpData_ID:
        *vp = INT_TO_JSVAL( gPriv->nIDs.size() );
        break;
      case CGumpData_Button:
        *vp = INT_TO_JSVAL( gPriv->nButtons.size() );
        break;
      default:
        break;
    }
  }
  return true;
}

bool CAccountProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  CAccountBlock_st *myAccount = JS::GetMaybePtrFromReservedSlot<CAccountBlock_st >(obj , 0);
  if( myAccount == nullptr )
    return false;

  if( JSVAL_IS_INT( id ))
  {
    JSString *tString = nullptr;
    switch( JSVAL_TO_INT( id ))
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

  return true;
}

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

bool CResourceProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  MapResource_st *gPriv = JS::GetMaybePtrFromReservedSlot<MapResource_st>(obj , 0);
  if( gPriv == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Resource property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

// clang-format off
IMPL_GET( CResource, logAmount , MapResource_st, setInt32,         logAmt   )
IMPL_GET( CResource, logTime,    MapResource_st, setPrivateUint32, logTime  )
IMPL_GET( CResource, oreAmount , MapResource_st, setInt32,         oreAmt   )
IMPL_GET( CResource, oreTime,    MapResource_st, setPrivateUint32, oreTime  )
IMPL_GET( CResource, fishAmount, MapResource_st, setInt32,         fishAmt  )
IMPL_GET( CResource, fishTime,   MapResource_st, setPrivateUint32, fishTime )
// clang-format on

bool CPartyProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
  Party *gPriv = JS::GetMaybePtrFromReservedSlot<Party >(obj , 0);
  if( gPriv == nullptr )
    return false;

  // Keep track of original script that's executing
  auto origScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
  auto origScriptID = JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

  JSEncapsulate encaps( cx, vp );
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
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

  // Active script-context might have been lost, so restore it...
  if( origScript != JSMapping->GetScript( JS::CurrentGlobalOrNull( cx )))
  {
    // ... by calling a dummy function in original script!
    bool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
    if( retVal == false )
    {
      // Dummy function not found, let shard admin know!
      Console.Warning( oldstrutil::format( "Script context lost after setting Party property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
    }
  }

  return true;
}

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
          *vp = JSVAL_NULL;
          return true;
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
  return true;
}

bool CScriptProps_getProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, jsval id, jsval *vp )
{
  if( JSVAL_IS_INT( id ))
  {
    switch( JSVAL_TO_INT( id ))
    {
      case CSCRIPT_SCRIPTID:		
        *vp = INT_TO_JSVAL( JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx )) );
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

