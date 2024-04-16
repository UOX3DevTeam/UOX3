//o------------------------------------------------------------------------------------------------o
//|	File		-	UOXJSPropertyFuncts.cpp
//|	Date		-	12/14/2001
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	1.0		14th December, 2001 Initial implementation
//|							Defines the property specifications for race, region, guild,
//|							item and chars
//o------------------------------------------------------------------------------------------------o
#ifndef __UOXJSPropertySpecs__
#define __UOXJSPropertySpecs__

#include "UOXJSPropertyEnums.h"
#include "enums.h"

#define DECL_GET( main, attr ) bool JS##main ## _get_ ## attr(JSContext *cx, unsigned int argc, JS::Value *vp);
#define DECL_SET( main, attr ) bool JS##main ## _set_ ## attr(JSContext *cx, unsigned int argc, JS::Value *vp);
#define IMPL_GET( main, attr, type, method, accessor ) \
bool JS##main##_get_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) { \
  auto args = JS::CallArgsFromVp(argc, vp); \
  JS::RootedObject thisObj(cx); \
  if (!args.computeThis(cx, &thisObj)) \
    return false; \
  auto priv = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0); \
  args.rval().method(priv->accessor); \
  return true; \
}

#define IMPL_GET_NP(main, attr, method, accessor)                                                          \
bool JS##main##_get_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) {                              \
  auto args = JS::CallArgsFromVp(argc, vp);                                                                \
  JS::RootedObject thisObj(cx);                                                                            \
  if (!args.computeThis(cx, &thisObj))                                                                     \
      return false;                                                                                        \
  args.rval().method( accessor );                                                                          \
  return true;                                                                                             \
}

#define IMPL_GETS(main, attr, type, method, accessor)                         \
bool JS##main##_get_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) { \
  auto args = JS::CallArgsFromVp(argc, vp);                                   \
  JS::RootedObject thisObj(cx);                                               \
  if (!args.computeThis(cx, &thisObj))                                        \
    return false;                                                             \
  auto priv = JS::GetMaybePtrFromReservedSlot< type >(thisObj, 0);            \
  args.rval().method( JS_NewStringCopyZ( cx, priv->accessor ) );              \
  return true;                                                                \
}

#define IMPL_GET_OBJ( main, attr, type, method, accessor ) \
bool JS##main##_get_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) { \
  auto args = JS::CallArgsFromVp(argc, vp); \
  JS::RootedObject thisObj(cx); \
  if (!args.computeThis(cx, &thisObj)) \
    return false; \
  auto priv = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0); \
  SERIAL TempSerial = INVALIDSERIAL; \
  if( !ValidateObject( priv )) \
    return false; \
  args.rval().method(priv->accessor); \
  return true; \
}

#define IMPL_GETS_OBJ(main, attr, type, method, accessor) \
bool JS##main##_get_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) { \
  auto args = JS::CallArgsFromVp(argc, vp); \
  JS::RootedObject thisObj(cx); \
  if (!args.computeThis(cx, &thisObj)) \
    return false; \
  auto priv = JS::GetMaybePtrFromReservedSlot< type >(thisObj, 0); \
	SERIAL TempSerial = INVALIDSERIAL; \
	if( !ValidateObject( priv )) \
    return false; \
  args.rval().method( JS_NewStringCopyZ( cx, priv->accessor ) ); \
  return true; \
}

#define IMPL_SET(main, attr, type, method, accessor)                                                             \
bool JS##main##_set_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) {                                    \
  auto args = JS::CallArgsFromVp(argc, vp);                                                                      \
  JS::RootedObject thisObj(cx);                                                                                  \
  if (!args.computeThis(cx, &thisObj))                                                                           \
      return false;                                                                                              \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor(args.get(0).method());                                                                          \
  if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx))) {                                         \
  }                                                                                                              \
  return true;                                                                                                   \
}

#define IMPL_SET_DIR(main, attr, type, method, accessor)                                                         \
bool JS##main##_set_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) {                                    \
  auto args = JS::CallArgsFromVp(argc, vp);                                                                      \
  JS::RootedObject thisObj(cx);                                                                                  \
  if (!args.computeThis(cx, &thisObj))                                                                           \
      return false;                                                                                              \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor    = args.get(0).method();                                                                      \
  if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx))) {                                         \
  }                                                                                                              \
  return true;                                                                                                   \
}

#define IMPL_SETS(main, attr, type, method, accessor)                                                            \
bool JS##main##_set_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) {                                    \
  auto args = JS::CallArgsFromVp(argc, vp);                                                                      \
  JS::RootedObject thisObj(cx);                                                                                  \
  if (!args.computeThis(cx, &thisObj))                                                                           \
      return false;                                                                                              \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor(convertToString(cx, args.get(0).method()));                                                     \
  if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx))) {                                         \
  }                                                                                                              \
  return true;                                                                                                   \
}

// This is the restore context we need to fix up
// bool retVal = origScript->CallParticularEvent("_restorecontext_", &id, 0, vp);
//    if( !retVal ) \
//    { \
//      Console.Warning( oldstrutil::format( "Script context lost after setting Race property %u. Add 'function
// _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID )); \
//    } \

#define IMPL_SET_NP(main, attr, method, accessor)                                                                \
bool JS##main##_set_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) {                                    \
  auto args = JS::CallArgsFromVp(argc, vp);                                                                      \
  accessor(args.get(0).method());                                                                                \
  return true;                                                                                                   \
}



#define DECL_GET_SET( main, attr ) \
DECL_GET( main, attr ) \
DECL_SET( main, attr )

// Forward declares
DECL_GET( CSpell, id )
DECL_GET( CSpell, action )
DECL_GET( CSpell, delay )
DECL_GET( CSpell, damageDelay )
DECL_GET( CSpell, recoveryDelay )
DECL_GET( CSpell, health )
DECL_GET( CSpell, stamina )
DECL_GET( CSpell, mana )
DECL_GET( CSpell, mantra )
DECL_GET( CSpell, name )
DECL_GET( CSpell, strToSay )
DECL_GET( CSpell, scrollLow )
DECL_GET( CSpell, scrollHigh )
DECL_GET( CSpell, circle )
DECL_GET( CSpell, lowSkill )
DECL_GET( CSpell, highSkill )
DECL_GET( CSpell, ginseng )
DECL_GET( CSpell, moss )
DECL_GET( CSpell, drake )
DECL_GET( CSpell, pearl )
DECL_GET( CSpell, silk )
DECL_GET( CSpell, ash )
DECL_GET( CSpell, shade )
DECL_GET( CSpell, garlic )
DECL_GET( CSpell, requireTarget )
DECL_GET( CSpell, requireItem )
DECL_GET( CSpell, requireLocation )
DECL_GET( CSpell, requireChar )
DECL_GET( CSpell, travelSpell )
DECL_GET( CSpell, fieldSpell )
DECL_GET( CSpell, reflectable )
DECL_GET( CSpell, aggressiveSpell )
DECL_GET( CSpell, resistable )
DECL_GET( CSpell, soundEffect )
DECL_GET( CSpell, enabled )
DECL_GET( CSpell, baseDmg )

// Global Skills
DECL_GET_SET( CGlobalSkill, name )
DECL_GET_SET( CGlobalSkill, madeWord )
DECL_GET_SET( CGlobalSkill, strength )
DECL_GET_SET( CGlobalSkill, dexterity )
DECL_GET_SET( CGlobalSkill, intelligence )
DECL_GET_SET( CGlobalSkill, skillDelay )
DECL_GET_SET( CGlobalSkill, scriptID )

// Create Entry
DECL_GET_SET( CCreateEntry, id )
DECL_GET_SET( CCreateEntry, name )
DECL_GET_SET( CCreateEntry, addItem )
DECL_GET_SET( CCreateEntry, colour )
DECL_GET_SET( CCreateEntry, sound )
DECL_GET_SET( CCreateEntry, minRank )
DECL_GET_SET( CCreateEntry, maxRank )
DECL_GET_SET( CCreateEntry, delay )
DECL_GET_SET( CCreateEntry, spell )
DECL_GET_SET( CCreateEntry, resources )
DECL_GET_SET( CCreateEntry, skills )
DECL_GET_SET( CCreateEntry, avgMinSkill )
DECL_GET_SET( CCreateEntry, avgMaxSkill )

// Timer Properties
DECL_GET( CTimer, TIMEOUT )
DECL_GET( CTimer, INVIS )
DECL_GET( CTimer, HUNGER )
DECL_GET( CTimer, THIRST )
DECL_GET( CTimer, POISONTIME )
DECL_GET( CTimer, POISONTEXT )
DECL_GET( CTimer, POISONWEAROFF )
DECL_GET( CTimer, SPELLTIME )
DECL_GET( CTimer, SPELLRECOVERYTIME )
DECL_GET( CTimer, CRIMFLAG )
DECL_GET( CTimer, ANTISPAM )
DECL_GET( CTimer, MURDERRATE )
DECL_GET( CTimer, PEACETIMER )
DECL_GET( CTimer, FLYINGTOGGLE )
DECL_GET( CTimer, FIREFIELDTICK )
DECL_GET( CTimer, POISONFIELDTICK )
DECL_GET( CTimer, PARAFIELDTICK )
DECL_GET( CTimer, MOVETIME )
DECL_GET( CTimer, SPATIMER )
DECL_GET( CTimer, SUMMONTIME )
DECL_GET( CTimer, EVADETIME )
DECL_GET( CTimer, LOYALTYTIME )
DECL_GET( CTimer, IDLEANIMTIME )
DECL_GET( CTimer, LOGOUT )
DECL_GET( CTimer, SOCK_SKILLDELAY )
DECL_GET( CTimer, SOCK_OBJDELAY )
DECL_GET( CTimer, SOCK_SPIRITSPEAK )
DECL_GET( CTimer, SOCK_TRACKING )
DECL_GET( CTimer, SOCK_FISHING )
DECL_GET( CTimer, SOCK_MUTETIME )
DECL_GET( CTimer, SOCK_TRACKINGDISPLAY )
DECL_GET( CTimer, SOCK_TRAFFICWARDEN )
DECL_GET( CTimer, YOUNGHEAL )
DECL_GET( CTimer, YOUNGMESSAGE )

// Race Properties
DECL_GET_SET( CRace, id )
DECL_GET_SET( CRace, name )
DECL_GET_SET( CRace, weakToWeather )
DECL_GET_SET( CRace, requiresBeard )
DECL_GET_SET( CRace, requiresNoBeard )
DECL_GET_SET( CRace, isPlayerRace )
DECL_GET_SET( CRace, genderRestrict )
DECL_GET_SET( CRace, armourClass )
DECL_GET_SET( CRace, languageSkillMin )
DECL_GET_SET( CRace, skillAdjustment )
DECL_GET_SET( CRace, poisonResistance )
DECL_GET_SET( CRace, magicResistance )
DECL_GET_SET( CRace, visibleDistance )
DECL_GET_SET( CRace, nightVision )

// Region Properties
DECL_GET_SET( CRegion, name )
DECL_GET_SET( CRegion, mayor )
DECL_GET_SET( CRegion, race )
DECL_GET_SET( CRegion, tax )
DECL_GET_SET( CRegion, taxResource )
DECL_GET_SET( CRegion, canMark )
DECL_GET_SET( CRegion, canRecall )
DECL_GET_SET( CRegion, canGate )
DECL_GET_SET( CRegion, canTeleport )
DECL_GET_SET( CRegion, canPlaceHouse )
DECL_GET_SET( CRegion, isGuarded )
DECL_GET_SET( CRegion, canCastAggressive )
DECL_GET_SET( CRegion, isSafeZone )
DECL_GET_SET( CRegion, health )
DECL_GET_SET( CRegion, isDungeon )
DECL_GET_SET( CRegion, worldNumber )
DECL_GET_SET( CRegion, instanceID )
DECL_GET_SET( CRegion, chanceBigOre )
DECL_GET( CRegion, numOrePrefs )
DECL_GET( CRegion, population )
DECL_GET( CRegion, members )
DECL_GET_SET( CRegion, id )
DECL_GET_SET( CRegion, scriptTrigger )
DECL_GET_SET( CRegion, scriptTriggers )
DECL_GET_SET( CRegion, numGuards )
DECL_GET_SET( CRegion, taxes )
DECL_GET_SET( CRegion, reserves )
DECL_GET_SET( CRegion, appearance )
DECL_GET_SET( CRegion, music )
DECL_GET_SET( CRegion, weather )
DECL_GET_SET( CRegion, owner )

// Spawn Region Properties
DECL_GET_SET( CSpawnRegion, name )
DECL_GET_SET( CSpawnRegion, regionNum )
DECL_GET_SET( CSpawnRegion, itemList )
DECL_GET_SET( CSpawnRegion, npcList )
DECL_GET_SET( CSpawnRegion, item )
DECL_GET_SET( CSpawnRegion, npc )
DECL_GET_SET( CSpawnRegion, maxItems )
DECL_GET_SET( CSpawnRegion, maxNpcs )
DECL_GET_SET( CSpawnRegion, itemCount )
DECL_GET_SET( CSpawnRegion, npcCount )
DECL_GET_SET( CSpawnRegion, onlyOutside )
DECL_GET_SET( CSpawnRegion, isSpawner )
DECL_GET_SET( CSpawnRegion, defZ )
DECL_GET_SET( CSpawnRegion, prefZ )
DECL_GET_SET( CSpawnRegion, x1 )
DECL_GET_SET( CSpawnRegion, y1 )
DECL_GET_SET( CSpawnRegion, x2 )
DECL_GET_SET( CSpawnRegion, y2 )
DECL_GET_SET( CSpawnRegion, world )
DECL_GET_SET( CSpawnRegion, instanceID )
DECL_GET_SET( CSpawnRegion, minTime )
DECL_GET_SET( CSpawnRegion, maxTime )
DECL_GET_SET( CSpawnRegion, call )

// Guild Properties
DECL_GET_SET( CGuild, name )
DECL_GET_SET( CGuild, type )
DECL_GET_SET( CGuild, master )
DECL_GET_SET( CGuild, stone )
DECL_GET( CGuild, numMembers )
DECL_GET( CGuild, numRecruits )
DECL_GET( CGuild, members )
DECL_GET( CGuild, recruits )
DECL_GET_SET( CGuild, charter )
DECL_GET_SET( CGuild, abbreviation )
DECL_GET_SET( CGuild, webPage )

// Character Properties
DECL_GET_SET( CCharacter, sectionID )
DECL_GET_SET( CCharacter, name )
DECL_GET_SET( CCharacter, origName )
DECL_GET_SET( CCharacter, title )
DECL_GET_SET( CCharacter, x )
DECL_GET_SET( CCharacter, y )
DECL_GET_SET( CCharacter, z )
DECL_GET_SET( CCharacter, oldX )
DECL_GET_SET( CCharacter, oldY )
DECL_GET_SET( CCharacter, oldZ )
DECL_GET_SET( CCharacter, id )
DECL_GET_SET( CCharacter, colour )
DECL_GET_SET( CCharacter, color )
DECL_GET_SET( CCharacter, skin )
DECL_GET_SET( CCharacter, hue )
DECL_GET_SET( CCharacter, controlSlots )
DECL_GET_SET( CCharacter, controlSlotsUsed )
DECL_GET_SET( CCharacter, orneriness )
DECL_GET_SET( CCharacter, owner )
DECL_GET_SET( CCharacter, visible )
DECL_GET_SET( CCharacter, serial )
DECL_GET_SET( CCharacter, health )
DECL_GET_SET( CCharacter, scripttrigger )
DECL_GET_SET( CCharacter, scriptTriggers )
DECL_GET_SET( CCharacter, worldnumber )
DECL_GET_SET( CCharacter, instanceID )
DECL_GET_SET( CCharacter, target )
DECL_GET_SET( CCharacter, dexterity )
DECL_GET_SET( CCharacter, intelligence )
DECL_GET_SET( CCharacter, strength )
DECL_GET_SET( CCharacter, actualDexterity )
DECL_GET_SET( CCharacter, actualIntelligence )
DECL_GET_SET( CCharacter, actualStrength )
DECL_GET_SET( CCharacter, skills )
DECL_GET_SET( CCharacter, mana )
DECL_GET_SET( CCharacter, stamina )
DECL_GET_SET( CCharacter, pack )
DECL_GET_SET( CCharacter, fame )
DECL_GET_SET( CCharacter, karma )
DECL_GET( CCharacter, attack )
DECL_GET_SET( CCharacter, canAttack )
DECL_GET_SET( CCharacter, fleeAt )
DECL_GET_SET( CCharacter, reAttackAt )
DECL_GET_SET( CCharacter, brkPeaceChance )
DECL_GET_SET( CCharacter, setPeace )
DECL_GET_SET( CCharacter, hunger )
DECL_GET( CCharacter, hungerRate )
DECL_GET_SET( CCharacter, thirst )
DECL_GET( CCharacter, thirstRate )
DECL_GET_SET( CCharacter, frozen )
DECL_GET_SET( CCharacter, commandlevel )
DECL_GET_SET( CCharacter, race )
DECL_GET_SET( CCharacter, hasStolen )
DECL_GET_SET( CCharacter, criminal )
DECL_GET( CCharacter, murderer )
DECL_GET_SET( CCharacter, innocent )
DECL_GET_SET( CCharacter, murdercount )
DECL_GET_SET( CCharacter, neutral )
DECL_GET_SET( CCharacter, npcFlag )
DECL_GET_SET( CCharacter, npCCharacter )
DECL_GET_SET( CCharacter, gender )
DECL_GET_SET( CCharacter, dead )
DECL_GET_SET( CCharacter, npc )
DECL_GET_SET( CCharacter, isAwake )
DECL_GET( CCharacter, online )
DECL_GET_SET( CCharacter, direction )
DECL_GET_SET( CCharacter, isRunning )
DECL_GET_SET( CCharacter, region )
DECL_GET_SET( CCharacter, town )
DECL_GET_SET( CCharacter, guild )
DECL_GET_SET( CCharacter, baseskills )
DECL_GET_SET( CCharacter, skillsused )
DECL_GET_SET( CCharacter, socket )
DECL_GET( CCharacter, isChar )
DECL_GET( CCharacter, isItem )
DECL_GET( CCharacter, isSpawner )
DECL_GET( CCharacter, spawnSerial )
DECL_GET_SET( CCharacter, maxhp )
DECL_GET_SET( CCharacter, maxstamina )
DECL_GET_SET( CCharacter, maxmana )
DECL_GET_SET( CCharacter, oldWandertype )
DECL_GET_SET( CCharacter, wandertype )
DECL_GET_SET( CCharacter, fx1 )
DECL_GET_SET( CCharacter, fy1 )
DECL_GET_SET( CCharacter, fx2 )
DECL_GET_SET( CCharacter, fy2 )
DECL_GET_SET( CCharacter, fz )
DECL_GET_SET( CCharacter, isonhorse )
DECL_GET_SET( CCharacter, isFlying )
DECL_GET_SET( CCharacter, isGuarded )
DECL_GET_SET( CCharacter, guarding )
DECL_GET_SET( CCharacter, tempdex )
DECL_GET_SET( CCharacter, tempint )
DECL_GET_SET( CCharacter, tempstr )
DECL_GET_SET( CCharacter, poison )
DECL_GET_SET( CCharacter, lightlevel )
DECL_GET_SET( CCharacter, vulnerable )
DECL_GET_SET( CCharacter, willhunger )
DECL_GET_SET( CCharacter, willthirst )
DECL_GET_SET( CCharacter, lodamage )
DECL_GET_SET( CCharacter, hidamage )
DECL_GET( CCharacter, flag )
DECL_GET_SET( CCharacter, atWar )
DECL_GET_SET( CCharacter, spellCast )
DECL_GET_SET( CCharacter, isCasting )
DECL_GET_SET( CCharacter, priv )
DECL_GET_SET( CCharacter, townPriv )
DECL_GET_SET( CCharacter, guildTitle )
DECL_GET_SET( CCharacter, hairStyle )
DECL_GET_SET( CCharacter, hairColour )
DECL_GET_SET( CCharacter, hairColor )
DECL_GET_SET( CCharacter, beardStyle )
DECL_GET_SET( CCharacter, beardColour )
DECL_GET_SET( CCharacter, beardColor )
DECL_GET_SET( CCharacter, fontType )
DECL_GET_SET( CCharacter, sayColour )
DECL_GET_SET( CCharacter, emoteColour )
DECL_GET_SET( CCharacter, attacker )
DECL_GET_SET( CCharacter, raceGate )
DECL_GET_SET( CCharacter, skillLock )
DECL_GET_SET( CCharacter, deaths )
DECL_GET_SET( CCharacter, ownerCount )
DECL_GET_SET( CCharacter, nextAct )
DECL_GET( CCharacter, petCount )
DECL_GET( CCharacter, followerCount )
DECL_GET( CCharacter, ownedItemsCount )
DECL_GET_SET( CCharacter, cell )
DECL_GET_SET( CCharacter, allmove )
DECL_GET_SET( CCharacter, houseicons )
DECL_GET_SET( CCharacter, spattack )
DECL_GET_SET( CCharacter, spdelay )
DECL_GET_SET( CCharacter, aitype )
DECL_GET_SET( CCharacter, split )
DECL_GET_SET( CCharacter, splitchance )
DECL_GET_SET( CCharacter, hireling )
DECL_GET_SET( CCharacter, trainer )
DECL_GET_SET( CCharacter, weight )
DECL_GET_SET( CCharacter, squelch )
DECL_GET( CCharacter, isJailed )
DECL_GET_SET( CCharacter, magicReflect )
DECL_GET_SET( CCharacter, permanentMagicReflect )
DECL_GET_SET( CCharacter, tamed )
DECL_GET_SET( CCharacter, tamedHungerRate )
DECL_GET_SET( CCharacter, tamedThirstRate )
DECL_GET_SET( CCharacter, hungerWildChance )
DECL_GET_SET( CCharacter, thirstWildChance )
DECL_GET_SET( CCharacter, foodList )
DECL_GET_SET( CCharacter, mounted )
DECL_GET_SET( CCharacter, stabled )
DECL_GET_SET( CCharacter, isUsingPotion )
DECL_GET_SET( CCharacter, stealth )
DECL_GET_SET( CCharacter, skillToTame )
DECL_GET_SET( CCharacter, skillToProv )
DECL_GET_SET( CCharacter, skillToPeace )
DECL_GET_SET( CCharacter, poisonStrength )
DECL_GET_SET( CCharacter, isPolymorphed )
DECL_GET_SET( CCharacter, isIncognito )
DECL_GET_SET( CCharacter, isDisguised )
DECL_GET_SET( CCharacter, canRun )
DECL_GET_SET( CCharacter, isMeditating )
DECL_GET_SET( CCharacter, isGM )
DECL_GET_SET( CCharacter, canBroadcast )
DECL_GET_SET( CCharacter, singClickSer )
DECL_GET_SET( CCharacter, noSkillTitles )
DECL_GET_SET( CCharacter, isGMPageable )
DECL_GET_SET( CCharacter, canSnoop )
DECL_GET_SET( CCharacter, isCounselor )
DECL_GET_SET( CCharacter, noNeedMana )
DECL_GET_SET( CCharacter, isDispellable )
DECL_GET_SET( CCharacter, noNeedReags )
DECL_GET_SET( CCharacter, orgID )
DECL_GET_SET( CCharacter, orgSkin )
DECL_GET( CCharacter, isAnimal )
DECL_GET( CCharacter, isHuman )
DECL_GET_SET( CCharacter, isShop )
DECL_GET_SET( CCharacter, maxLoyalty )
DECL_GET_SET( CCharacter, loyalty )
DECL_GET_SET( CCharacter, loyaltyRate )
DECL_GET_SET( CCharacter, shouldSave )
DECL_GET_SET( CCharacter, origin )
DECL_GET_SET( CCharacter, partyLootable )
DECL_GET( CCharacter, party )
DECL_GET_SET( CCharacter, multi )
DECL_GET_SET( CCharacter, accountNum )
DECL_GET_SET( CCharacter, account )
DECL_GET_SET( CCharacter, createdOn )
DECL_GET_SET( CCharacter, playTime )
DECL_GET_SET( CCharacter, housesOwned )
DECL_GET_SET( CCharacter, housesCoOwned )

// Item Properties
DECL_GET_SET( CItem, sectionID )
DECL_GET_SET( CItem, name )
DECL_GET_SET( CItem, origName )
DECL_GET_SET( CItem, title )
DECL_GET_SET( CItem, x )
DECL_GET_SET( CItem, y )
DECL_GET_SET( CItem, z )
DECL_GET_SET( CItem, oldX )
DECL_GET_SET( CItem, oldY )
DECL_GET_SET( CItem, oldZ )
DECL_GET_SET( CItem, id )
DECL_GET_SET( CItem, colour )
DECL_GET_SET( CItem, color )
DECL_GET_SET( CItem, skin )
DECL_GET_SET( CItem, hue )
DECL_GET_SET( CItem, owner )
DECL_GET_SET( CItem, visible )
DECL_GET_SET( CItem, serial )
DECL_GET_SET( CItem, health )
DECL_GET_SET( CItem, scripttrigger )
DECL_GET_SET( CItem, scriptTriggers )
DECL_GET_SET( CItem, worldnumber )
DECL_GET_SET( CItem, instanceID )
DECL_GET_SET( CItem, amount )
DECL_GET_SET( CItem, container )
DECL_GET_SET( CItem, type )
DECL_GET_SET( CItem, more )
DECL_GET_SET( CItem, more0 )
DECL_GET_SET( CItem, more1 )
DECL_GET_SET( CItem, more2 )
DECL_GET_SET( CItem, morex )
DECL_GET_SET( CItem, morey )
DECL_GET_SET( CItem, morez )
DECL_GET_SET( CItem, movable )
DECL_GET_SET( CItem, att )
DECL_GET_SET( CItem, layer )
DECL_GET_SET( CItem, itemsinside )
DECL_GET_SET( CItem, totalItemCount )
DECL_GET_SET( CItem, decayable )
DECL_GET_SET( CItem, decaytime )
DECL_GET_SET( CItem, lodamage )
DECL_GET_SET( CItem, hidamage )
DECL_GET_SET( CItem, ac )
DECL_GET_SET( CItem, def )
DECL_GET_SET( CItem, resistCold )
DECL_GET_SET( CItem, resistHeat )
DECL_GET_SET( CItem, resistLight )
DECL_GET_SET( CItem, resistLightning )
DECL_GET_SET( CItem, resistPoison )
DECL_GET_SET( CItem, resistRain )
DECL_GET_SET( CItem, resistSnow )
DECL_GET_SET( CItem, damageHeat )
DECL_GET_SET( CItem, damageCold )
DECL_GET_SET( CItem, damageLight )
DECL_GET_SET( CItem, damageLightning )
DECL_GET_SET( CItem, damagePoison )
DECL_GET_SET( CItem, damageRain )
DECL_GET_SET( CItem, damageSnow )
DECL_GET_SET( CItem, name2 )
DECL_GET_SET( CItem, isChar )
DECL_GET_SET( CItem, isItem )
DECL_GET_SET( CItem, isSpawner )
DECL_GET_SET( CItem, race )
DECL_GET_SET( CItem, maxhp )
DECL_GET_SET( CItem, maxUses )
DECL_GET_SET( CItem, usesLeft )
DECL_GET_SET( CItem, rank )
DECL_GET_SET( CItem, creator )
DECL_GET_SET( CItem, poison )
DECL_GET_SET( CItem, dir )
DECL_GET_SET( CItem, wipable )
DECL_GET_SET( CItem, buyvalue )
DECL_GET_SET( CItem, sellvalue )
DECL_GET_SET( CItem, vendorPrice )
DECL_GET_SET( CItem, restock )
DECL_GET_SET( CItem, good )
DECL_GET_SET( CItem, divinelock )
DECL_GET_SET( CItem, baseWeight )
DECL_GET_SET( CItem, weight )
DECL_GET_SET( CItem, weightMax )
DECL_GET_SET( CItem, maxItems )
DECL_GET_SET( CItem, strength )
DECL_GET_SET( CItem, dexterity )
DECL_GET_SET( CItem, intelligence )
DECL_GET_SET( CItem, corpse )
DECL_GET_SET( CItem, desc )
DECL_GET_SET( CItem, event )
DECL_GET_SET( CItem, tempLastTraded )
DECL_GET_SET( CItem, tempTimer )
DECL_GET_SET( CItem, shouldSave )
DECL_GET_SET( CItem, isNewbie )
DECL_GET_SET( CItem, isDispellable )
DECL_GET_SET( CItem, madeWith )
DECL_GET_SET( CItem, entryMadeFrom )
DECL_GET_SET( CItem, isPileable )
DECL_GET_SET( CItem, isMarkedByMaker )
DECL_GET_SET( CItem, isDyeable )
DECL_GET_SET( CItem, isDamageable )
DECL_GET_SET( CItem, isWipeable )
DECL_GET_SET( CItem, isGuarded )
DECL_GET_SET( CItem, isDoorOpen )
DECL_GET_SET( CItem, isFieldSpell )
DECL_GET_SET( CItem, isLockedDown )
DECL_GET_SET( CItem, isShieldType )
DECL_GET_SET( CItem, isMetalType )
DECL_GET_SET( CItem, isLeatherType )
DECL_GET_SET( CItem, canBeLockedDown )
DECL_GET_SET( CItem, isContType )
DECL_GET_SET( CItem, carveSection )
DECL_GET_SET( CItem, ammoID )
DECL_GET_SET( CItem, ammoHue )
DECL_GET_SET( CItem, ammoFX )
DECL_GET_SET( CItem, ammoFXHue )
DECL_GET_SET( CItem, ammoFXRender )
DECL_GET_SET( CItem, speed )
DECL_GET_SET( CItem, multi )
DECL_GET_SET( CItem, maxRange )
DECL_GET_SET( CItem, baseRange )
DECL_GET_SET( CItem, region )
DECL_GET_SET( CItem, spawnSerial )
DECL_GET_SET( CItem, origin )
DECL_GET_SET( CItem, isItemHeld )
DECL_GET_SET( CItem, stealable )
DECL_GET_SET( CItem, spawnsection )
DECL_GET_SET( CItem, sectionalist )
DECL_GET_SET( CItem, mininterval )
DECL_GET_SET( CItem, maxinterval )
DECL_GET_SET( CItem, lockdowns )
DECL_GET_SET( CItem, maxLockdowns )
DECL_GET_SET( CItem, trashContainers )
DECL_GET_SET( CItem, maxTrashContainers )
DECL_GET_SET( CItem, secureContainers )
DECL_GET_SET( CItem, maxSecureContainers )
DECL_GET_SET( CItem, friends )
DECL_GET_SET( CItem, maxFriends )
DECL_GET_SET( CItem, guests )
DECL_GET_SET( CItem, maxGuests )
DECL_GET_SET( CItem, owners )
DECL_GET_SET( CItem, maxOwners )
DECL_GET_SET( CItem, bans )
DECL_GET_SET( CItem, maxBans )
DECL_GET_SET( CItem, vendors )
DECL_GET_SET( CItem, maxVendors )
DECL_GET_SET( CItem, deed )
DECL_GET_SET( CItem, isPublic )
DECL_GET_SET( CItem, buildTimestamp )
DECL_GET_SET( CItem, tradeTimestamp )
DECL_GET_SET( CItem, banX )
DECL_GET_SET( CItem, banY )

// Socket Properties
DECL_GET_SET( CSocket, account )
DECL_GET_SET( CSocket, currentChar )
DECL_GET_SET( CSocket, idleTimeout )
DECL_GET_SET( CSocket, wasIdleWarned )
DECL_GET_SET( CSocket, tempInt )
DECL_GET_SET( CSocket, tempInt2 )
DECL_GET( CSocket, buffer )
DECL_GET( CSocket, xText )
DECL_GET( CSocket, xText2 )
DECL_GET_SET( CSocket, clickZ )
DECL_GET( CSocket, addID )
DECL_GET_SET( CSocket, newClient )
DECL_GET_SET( CSocket, firstPacket )
DECL_GET_SET( CSocket, cryptClient )
DECL_GET( CSocket, clientIP )
DECL_GET_SET( CSocket, walkSequence )
DECL_GET_SET( CSocket, currentSpellType )
DECL_GET_SET( CSocket, logging )
DECL_GET_SET( CSocket, bytesSent )
DECL_GET_SET( CSocket, bytesReceived )
DECL_GET_SET( CSocket, targetOK )
DECL_GET_SET( CSocket, clickX )
DECL_GET_SET( CSocket, clickY )
DECL_GET_SET( CSocket, pickupX )
DECL_GET_SET( CSocket, pickupY )
DECL_GET_SET( CSocket, pickupZ )
DECL_GET_SET( CSocket, pickupSpot )
DECL_GET_SET( CSocket, pickupSerial )
DECL_GET_SET( CSocket, language )
DECL_GET_SET( CSocket, clientMajorVer )
DECL_GET_SET( CSocket, clientMinorVer )
DECL_GET_SET( CSocket, clientSubVer )
DECL_GET_SET( CSocket, clientLetterVer )
DECL_GET_SET( CSocket, clientType )
DECL_GET_SET( CSocket, target )
DECL_GET_SET( CSocket, tempObj )
DECL_GET_SET( CSocket, tempObj2 )

// Skills Properties
DECL_GET_SET( CSkills, alchemy )
DECL_GET_SET( CSkills, anatomy )
DECL_GET_SET( CSkills, animallore )
DECL_GET_SET( CSkills, itemid )
DECL_GET_SET( CSkills, armslore )
DECL_GET_SET( CSkills, parrying )
DECL_GET_SET( CSkills, begging )
DECL_GET_SET( CSkills, blacksmithing )
DECL_GET_SET( CSkills, bowcraft )
DECL_GET_SET( CSkills, peacemaking )
DECL_GET_SET( CSkills, camping )
DECL_GET_SET( CSkills, carpentry )
DECL_GET_SET( CSkills, cartography )
DECL_GET_SET( CSkills, cooking )
DECL_GET_SET( CSkills, detectinghidden )
DECL_GET_SET( CSkills, enticement )
DECL_GET_SET( CSkills, evaluatingintel )
DECL_GET_SET( CSkills, healing )
DECL_GET_SET( CSkills, fishing )
DECL_GET_SET( CSkills, forensics )
DECL_GET_SET( CSkills, herding )
DECL_GET_SET( CSkills, hiding )
DECL_GET_SET( CSkills, provocation )
DECL_GET_SET( CSkills, inscription )
DECL_GET_SET( CSkills, lockpicking )
DECL_GET_SET( CSkills, magery )
DECL_GET_SET( CSkills, magicresistance )
DECL_GET_SET( CSkills, tactics )
DECL_GET_SET( CSkills, snooping )
DECL_GET_SET( CSkills, musicianship )
DECL_GET_SET( CSkills, poisoning )
DECL_GET_SET( CSkills, archery )
DECL_GET_SET( CSkills, spiritspeak )
DECL_GET_SET( CSkills, stealing )
DECL_GET_SET( CSkills, tailoring )
DECL_GET_SET( CSkills, taming )
DECL_GET_SET( CSkills, tasteid )
DECL_GET_SET( CSkills, tinkering )
DECL_GET_SET( CSkills, tracking )
DECL_GET_SET( CSkills, veterinary )
DECL_GET_SET( CSkills, swordsmanship )
DECL_GET_SET( CSkills, macefighting )
DECL_GET_SET( CSkills, fencing )
DECL_GET_SET( CSkills, wrestling )
DECL_GET_SET( CSkills, lumberjacking )
DECL_GET_SET( CSkills, mining )
DECL_GET_SET( CSkills, meditation )
DECL_GET_SET( CSkills, stealth )
DECL_GET_SET( CSkills, removetrap )
DECL_GET_SET( CSkills, necromancy )
DECL_GET_SET( CSkills, focus )
DECL_GET_SET( CSkills, chivalry )
DECL_GET_SET( CSkills, bushido )
DECL_GET_SET( CSkills, ninjitsu )
DECL_GET_SET( CSkills, spellweaving )
DECL_GET_SET( CSkills, imbuing )
DECL_GET_SET( CSkills, mysticism )
DECL_GET_SET( CSkills, throwing )
DECL_GET_SET( CSkills, allskills )

// Gump Properties
DECL_GET_SET( CGumpData, buttons )
DECL_GET_SET( CGumpData, IDs )

// Account Properties
DECL_GET( CAccount, id )
DECL_GET( CAccount, username )
DECL_GET_SET( CAccount, password )
DECL_GET( CAccount, flags )
DECL_GET_SET( CAccount, path )
DECL_GET_SET( CAccount, comment )
DECL_GET_SET( CAccount, timeban )
DECL_GET( CAccount, firstLogin )
DECL_GET_SET( CAccount, totalPlayTime )
DECL_GET( CAccount, character1 )
DECL_GET( CAccount, character2 )
DECL_GET( CAccount, character3 )
DECL_GET( CAccount, character4 )
DECL_GET( CAccount, character5 )
DECL_GET( CAccount, character6 )
DECL_GET( CAccount, character7 )
DECL_GET( CAccount, currentChar )
DECL_GET( CAccount, lastIP )
DECL_GET_SET( CAccount, isBanned )
DECL_GET_SET( CAccount, isSuspended )
DECL_GET_SET( CAccount, isPublic )
DECL_GET_SET( CAccount, isOnline )
DECL_GET_SET( CAccount, isSlot1Blocked )
DECL_GET_SET( CAccount, isSlot2Blocked )
DECL_GET_SET( CAccount, isSlot3Blocked )
DECL_GET_SET( CAccount, isSlot4Blocked )
DECL_GET_SET( CAccount, isSlot5Blocked )
DECL_GET_SET( CAccount, isSlot6Blocked )
DECL_GET_SET( CAccount, isSlot7Blocked )
DECL_GET_SET( CAccount, isYoung )
DECL_GET_SET( CAccount, unused10 )
DECL_GET_SET( CAccount, isSeer )
DECL_GET_SET( CAccount, isCounselor )
DECL_GET_SET( CAccount, isGM )

// Console Properties
DECL_GET_SET( CConsole, mode )
DECL_GET_SET( CConsole, logEcho )

// Script Section Properties
DECL_GET( CScriptSection, numTags )
DECL_GET( CScriptSection, atEnd )
DECL_GET( CScriptSection, atEndTags )

// Resource Properties
DECL_GET_SET( CResource, logAmount )
DECL_GET_SET( CResource, logTime )
DECL_GET_SET( CResource, oreAmount )
DECL_GET_SET( CResource, oreTime )
DECL_GET_SET( CResource, fishAmount )
DECL_GET_SET( CResource, fishTime )

// Party Properties
DECL_GET_SET( CParty, leader )
DECL_GET( CParty, memberCount )
DECL_GET_SET( CParty, isNPC )

// Script Properties
DECL_GET_SET( CScript, script_id )

// Property table

// clang-format off
inline JSPropertySpec CSpellProperties[] =
{
  JS_PSG( "id",               JSCSpell_get_id,              JSPROP_ENUMANDPERM ),
  JS_PSG( "action",           JSCSpell_get_action,          JSPROP_ENUMANDPERM ),
  JS_PSG( "delay",            JSCSpell_get_delay,           JSPROP_ENUMANDPERM ),
  JS_PSG( "damageDelay",      JSCSpell_get_damageDelay,     JSPROP_ENUMANDPERM ),
  JS_PSG( "recoveryDelay",    JSCSpell_get_recoveryDelay,   JSPROP_ENUMANDPERM ),
  JS_PSG( "health",           JSCSpell_get_health,          JSPROP_ENUMANDPERM ),
  JS_PSG( "stamina",          JSCSpell_get_stamina,         JSPROP_ENUMANDPERM ),
  JS_PSG( "mana",             JSCSpell_get_mana,            JSPROP_ENUMANDPERM ),
  JS_PSG( "mantra",           JSCSpell_get_mantra,          JSPROP_ENUMANDPERM ),
  JS_PSG( "name",             JSCSpell_get_name,            JSPROP_ENUMANDPERM ),
  JS_PSG( "strToSay",         JSCSpell_get_strToSay,        JSPROP_ENUMANDPERM ),
  JS_PSG( "scrollLow",        JSCSpell_get_scrollLow,       JSPROP_ENUMANDPERM ),
  JS_PSG( "scrollHigh",       JSCSpell_get_scrollHigh,      JSPROP_ENUMANDPERM ),
  JS_PSG( "circle",           JSCSpell_get_circle,          JSPROP_ENUMANDPERM ),
  JS_PSG( "lowSkill",         JSCSpell_get_lowSkill,        JSPROP_ENUMANDPERM ),
  JS_PSG( "highSkill",        JSCSpell_get_highSkill,       JSPROP_ENUMANDPERM ),
  JS_PSG( "ginseng",          JSCSpell_get_ginseng,         JSPROP_ENUMANDPERM ),
  JS_PSG( "moss",             JSCSpell_get_moss,            JSPROP_ENUMANDPERM ),
  JS_PSG( "drake",            JSCSpell_get_drake,           JSPROP_ENUMANDPERM ),
  JS_PSG( "pearl",            JSCSpell_get_pearl,           JSPROP_ENUMANDPERM ),
  JS_PSG( "silk",             JSCSpell_get_silk,            JSPROP_ENUMANDPERM ),
  JS_PSG( "ash",              JSCSpell_get_ash,             JSPROP_ENUMANDPERM ),
  JS_PSG( "shade",            JSCSpell_get_shade,           JSPROP_ENUMANDPERM ),
  JS_PSG( "garlic",           JSCSpell_get_garlic,          JSPROP_ENUMANDPERM ),
  JS_PSG( "requireTarget",    JSCSpell_get_requireTarget,   JSPROP_ENUMANDPERM ),
  JS_PSG( "requireItem",      JSCSpell_get_requireItem,     JSPROP_ENUMANDPERM ),
  JS_PSG( "requireLocation",  JSCSpell_get_requireLocation, JSPROP_ENUMANDPERM ),
  JS_PSG( "requireChar",      JSCSpell_get_requireChar,     JSPROP_ENUMANDPERM ),
  JS_PSG( "travelSpell",      JSCSpell_get_travelSpell,     JSPROP_ENUMANDPERM ),
  JS_PSG( "fieldSpell",       JSCSpell_get_fieldSpell,      JSPROP_ENUMANDPERM ),
  JS_PSG( "reflectable",      JSCSpell_get_reflectable,     JSPROP_ENUMANDPERM ),
  JS_PSG( "aggressiveSpell",  JSCSpell_get_aggressiveSpell, JSPROP_ENUMANDPERM ),
  JS_PSG( "resistable",       JSCSpell_get_resistable,      JSPROP_ENUMANDPERM ),
  JS_PSG( "soundEffect",      JSCSpell_get_soundEffect,     JSPROP_ENUMANDPERM ),
  JS_PSG( "enabled",          JSCSpell_get_enabled,         JSPROP_ENUMANDPERM ),
  JS_PSG( "baseDmg",          JSCSpell_get_baseDmg,         JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CGlobalSkillProperties[] =
{
  JS_PSG( "name",           JSCGlobalSkill_get_name,         JSPROP_ENUMANDPERM ),
  JS_PSG( "madeWord",       JSCGlobalSkill_get_madeWord,     JSPROP_ENUMANDPERM ),
  JS_PSG( "strength",       JSCGlobalSkill_get_strength,     JSPROP_ENUMANDPERM ),
  JS_PSG( "dexterity",      JSCGlobalSkill_get_dexterity,    JSPROP_ENUMANDPERM ),
  JS_PSG( "intelligence",   JSCGlobalSkill_get_intelligence, JSPROP_ENUMANDPERM ),
  JS_PSG( "skillDelay",     JSCGlobalSkill_get_skillDelay,   JSPROP_ENUMANDPERM ),
  JS_PSG( "scriptID",       JSCGlobalSkill_get_scriptID,     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CCreateEntryProperties[] =
{
  JS_PSG( "id",          JSCCreateEntry_get_id,           JSPROP_ENUMANDPERM ),
  JS_PSG( "name",        JSCCreateEntry_get_name,         JSPROP_ENUMANDPERM ),
  JS_PSG( "addItem",     JSCCreateEntry_get_addItem,      JSPROP_ENUMANDPERM ),
  JS_PSG( "colour",      JSCCreateEntry_get_colour,       JSPROP_ENUMANDPERM ),
  JS_PSG( "sound",       JSCCreateEntry_get_sound,        JSPROP_ENUMANDPERM ),
  JS_PSG( "minRank",     JSCCreateEntry_get_minRank,      JSPROP_ENUMANDPERM ),
  JS_PSG( "maxRank",     JSCCreateEntry_get_maxRank,      JSPROP_ENUMANDPERM ),
  JS_PSG( "delay",       JSCCreateEntry_get_delay,        JSPROP_ENUMANDPERM ),
  JS_PSG( "spell",       JSCCreateEntry_get_spell,        JSPROP_ENUMANDPERM ),
  JS_PSG( "resources",   JSCCreateEntry_get_resources,    JSPROP_ENUMANDPERM ),
  JS_PSG( "skills",      JSCCreateEntry_get_skills,       JSPROP_ENUMANDPERM ),
  JS_PSG( "avgMinSkill", JSCCreateEntry_get_avgMinSkill,  JSPROP_ENUMANDPERM ),
  JS_PSG( "avgMaxSkill", JSCCreateEntry_get_avgMaxSkill,  JSPROP_ENUMANDPERM ),
  JS_PS_END
};

// clang-format on

inline JSPropertySpec CTimerProperties[] =
{
  // Character Timers
  JS_PSG( "TIMEOUT",			JSCTimer_get_TIMEOUT,			JSPROP_ENUMPERMRO ),
  JS_PSG( "INVIS",				JSCTimer_get_INVIS,			JSPROP_ENUMPERMRO ),
  JS_PSG( "HUNGER",				JSCTimer_get_HUNGER,			JSPROP_ENUMPERMRO ),
  JS_PSG( "THIRST",				JSCTimer_get_THIRST,			JSPROP_ENUMPERMRO ),
  JS_PSG( "POISONTIME",			JSCTimer_get_POISONTIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "POISONTEXT",			JSCTimer_get_POISONTEXT,		JSPROP_ENUMPERMRO ),
  JS_PSG( "POISONWEAROFF",		JSCTimer_get_POISONWEAROFF,	JSPROP_ENUMPERMRO ),
  JS_PSG( "SPELLTIME",			JSCTimer_get_SPELLTIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "SPELLRECOVERYTIME",	JSCTimer_get_SPELLRECOVERYTIME,JSPROP_ENUMPERMRO ),
  JS_PSG( "CRIMFLAG",			JSCTimer_get_CRIMFLAG,			JSPROP_ENUMPERMRO ),
  JS_PSG( "ANTISPAM",			JSCTimer_get_ANTISPAM,			JSPROP_ENUMPERMRO ),
  JS_PSG( "MURDERRATE",			JSCTimer_get_MURDERRATE,		JSPROP_ENUMPERMRO ),
  JS_PSG( "PEACETIMER",			JSCTimer_get_PEACETIMER,		JSPROP_ENUMPERMRO ),
  JS_PSG( "FLYINGTOGGLE",		JSCTimer_get_FLYINGTOGGLE,		JSPROP_ENUMPERMRO ),
  JS_PSG( "FIREFIELDTICK",		JSCTimer_get_FIREFIELDTICK,	JSPROP_ENUMPERMRO ),
  JS_PSG( "POISONFIELDTICK",	JSCTimer_get_POISONFIELDTICK,	JSPROP_ENUMPERMRO ),
  JS_PSG( "PARAFIELDTICK",		JSCTimer_get_PARAFIELDTICK,	JSPROP_ENUMPERMRO ),
  JS_PSG( "MOVETIME",			JSCTimer_get_MOVETIME,			JSPROP_ENUMPERMRO ),
  JS_PSG( "SPATIMER",			JSCTimer_get_SPATIMER,			JSPROP_ENUMPERMRO ),
  JS_PSG( "SUMMONTIME",			JSCTimer_get_SUMMONTIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "EVADETIME",			JSCTimer_get_EVADETIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "LOYALTYTIME",		JSCTimer_get_LOYALTYTIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "IDLEANIMTIME",		JSCTimer_get_IDLEANIMTIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "LOGOUT",				JSCTimer_get_LOGOUT,			JSPROP_ENUMPERMRO ),

  // Socket Timers
  JS_PSG( "SOCK_SKILLDELAY",		JSCTimer_get_SOCK_SKILLDELAY,		JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_OBJDELAY",			JSCTimer_get_SOCK_OBJDELAY,		JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_SPIRITSPEAK",		JSCTimer_get_SOCK_SPIRITSPEAK,		JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_TRACKING",			JSCTimer_get_SOCK_TRACKING,		JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_FISHING",			JSCTimer_get_SOCK_FISHING,			JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_MUTETIME",			JSCTimer_get_SOCK_MUTETIME,		JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_TRACKINGDISPLAY",	JSCTimer_get_SOCK_TRACKINGDISPLAY,	JSPROP_ENUMPERMRO ),
  JS_PSG( "SOCK_TRAFFICWARDEN",		JSCTimer_get_SOCK_TRAFFICWARDEN,	JSPROP_ENUMPERMRO ),
  JS_PS_END
};

inline JSPropertySpec CRaceProperties[] =
{
  JS_PSGS( "id",				JSCRace_get_id,                JSCRace_set_id,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "name",				JSCRace_get_name,	           JSCRace_set_name,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "weakToWeather",		JSCRace_get_weakToWeather,	   JSCRace_set_weakToWeather,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "requiresBeard",		JSCRace_get_requiresBeard,     JSCRace_set_requiresBeard,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "requiresNoBeard",	JSCRace_get_requiresNoBeard,   JSCRace_set_requiresNoBeard,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "isPlayerRace",	    JSCRace_get_isPlayerRace,	   JSCRace_set_isPlayerRace,	  JSPROP_ENUMANDPERM ),
  JS_PSGS( "genderRestrict",	JSCRace_get_genderRestrict,	   JSCRace_set_genderRestrict,	  JSPROP_ENUMANDPERM ),
  JS_PSGS( "armourClass",		JSCRace_get_armourClass,	   JSCRace_set_armourClass,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "languageSkillMin",	JSCRace_get_languageSkillMin,  JSCRace_set_languageSkillMin,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillAdjustment",   JSCRace_get_skillAdjustment,   JSCRace_set_skillAdjustment,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "poisonResistance",	JSCRace_get_poisonResistance,  JSCRace_set_poisonResistance,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "magicResistance",   JSCRace_get_magicResistance,   JSCRace_set_magicResistance,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "visibleDistance",	JSCRace_get_visibleDistance,   JSCRace_set_visibleDistance,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "nightVision",	    JSCRace_get_nightVision,	   JSCRace_set_nightVision,  JSPROP_ENUMANDPERM ),
  JS_PS_END
};

// clang-format off
inline JSPropertySpec CRegionProperties[] =
{
  JS_PSGS( "name",              JSCRegion_get_name,              JSCRegion_set_name,               JSPROP_ENUMANDPERM ),
  JS_PSGS( "mayor",             JSCRegion_get_mayor,             JSCRegion_set_mayor,              JSPROP_ENUMANDPERM ),
  JS_PSGS( "race",              JSCRegion_get_race,              JSCRegion_set_race,               JSPROP_ENUMANDPERM ),
  JS_PSGS( "tax",               JSCRegion_get_tax,               JSCRegion_set_tax,                JSPROP_ENUMANDPERM ),
  JS_PSGS( "taxResource",       JSCRegion_get_taxResource,       JSCRegion_set_taxResource,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "canMark",           JSCRegion_get_canMark,           JSCRegion_set_canMark,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "canRecall",         JSCRegion_get_canRecall,         JSCRegion_set_canRecall,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "canGate",           JSCRegion_get_canGate,           JSCRegion_set_canGate,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "canTeleport",       JSCRegion_get_canTeleport,       JSCRegion_set_canTeleport,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "canPlaceHouse",     JSCRegion_get_canPlaceHouse,     JSCRegion_set_canPlaceHouse,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGuarded",         JSCRegion_get_isGuarded,         JSCRegion_set_isGuarded,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "canCastAggressive", JSCRegion_get_canCastAggressive, JSCRegion_set_canCastAggressive,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSafeZone",        JSCRegion_get_isSafeZone,        JSCRegion_set_isSafeZone,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "health",            JSCRegion_get_health,            JSCRegion_set_health,             JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDungeon",         JSCRegion_get_isDungeon,         JSCRegion_set_isDungeon,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "worldNumber",       JSCRegion_get_worldNumber,       JSCRegion_set_worldNumber,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "instanceID",        JSCRegion_get_instanceID,        JSCRegion_set_instanceID,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "chanceBigOre",      JSCRegion_get_chanceBigOre,      JSCRegion_set_chanceBigOre,       JSPROP_ENUMANDPERM ),
  JS_PSG(  "numOrePrefs",       JSCRegion_get_numOrePrefs,                                         JSPROP_ENUMPERMRO  ),
  JS_PSG(  "population",        JSCRegion_get_population,                                          JSPROP_ENUMPERMRO  ),
  JS_PSG(  "members",           JSCRegion_get_members,                                             JSPROP_ENUMERATE   ), // Looking for array jsprop old one was JSPROP_ENUMPERMIDX
  JS_PSGS( "id",                JSCRegion_get_id,                JSCRegion_set_id,                 JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTrigger",     JSCRegion_get_scriptTrigger,     JSCRegion_set_scriptTrigger,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTriggers",    JSCRegion_get_scriptTriggers,    JSCRegion_set_scriptTriggers,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "numGuards",         JSCRegion_get_numGuards,         JSCRegion_set_numGuards,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "taxes",             JSCRegion_get_taxes,             JSCRegion_set_taxes,              JSPROP_ENUMANDPERM ),
  JS_PSGS( "reserves",          JSCRegion_get_reserves,          JSCRegion_set_reserves,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "appearance",        JSCRegion_get_appearance,        JSCRegion_set_appearance,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "music",             JSCRegion_get_music,             JSCRegion_set_music,              JSPROP_ENUMANDPERM ),
  JS_PSGS( "weather",           JSCRegion_get_weather,           JSCRegion_set_weather,            JSPROP_ENUMANDPERM ),
  JS_PSG(  "owner",             JSCRegion_get_owner,                                               JSPROP_ENUMANDPERM ),
  JS_PS_END
};
// clang-format on

inline JSPropertySpec CSpawnRegionProperties[] =
{
  JS_PSGS( "name",  			JSCSpawnRegion_get_name,	   JSCSpawnRegion_set_name,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "regionNum",			JSCSpawnRegion_get_regionNum,	   JSCSpawnRegion_set_regionNum,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "itemList",			JSCSpawnRegion_get_itemList,	   JSCSpawnRegion_set_itemList,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "npcList",			JSCSpawnRegion_get_npcList,	   JSCSpawnRegion_set_npcList,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "item",				JSCSpawnRegion_get_item,	   JSCSpawnRegion_set_item,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "npc",				JSCSpawnRegion_get_npc,	   JSCSpawnRegion_set_npc,						 JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxItems",			JSCSpawnRegion_get_maxItems,	   JSCSpawnRegion_set_maxItems,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxNpcs",			JSCSpawnRegion_get_maxNpcs,	   JSCSpawnRegion_set_maxNpcs,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "itemCount",			JSCSpawnRegion_get_itemCount,	   JSCSpawnRegion_set_itemCount,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "npcCount",			JSCSpawnRegion_get_npcCount,	   JSCSpawnRegion_set_npcCount,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "onlyOutside",		JSCSpawnRegion_get_onlyOutside,	   JSCSpawnRegion_set_onlyOutside,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSpawner",			JSCSpawnRegion_get_isSpawner,	   JSCSpawnRegion_set_isSpawner,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "defZ",				JSCSpawnRegion_get_defZ,	   JSCSpawnRegion_set_defZ,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "prefZ",				JSCSpawnRegion_get_prefZ,	   JSCSpawnRegion_set_prefZ,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "x1",				JSCSpawnRegion_get_x1,	   JSCSpawnRegion_set_x1,						 JSPROP_ENUMANDPERM ),
  JS_PSGS( "y1",				JSCSpawnRegion_get_y1,	   JSCSpawnRegion_set_y1,						 JSPROP_ENUMANDPERM ),
  JS_PSGS( "x2",				JSCSpawnRegion_get_x2,	   JSCSpawnRegion_set_x2,						 JSPROP_ENUMANDPERM ),
  JS_PSGS( "y2",				JSCSpawnRegion_get_y2,	   JSCSpawnRegion_set_y2,						 JSPROP_ENUMANDPERM ),
  JS_PSGS( "world",				JSCSpawnRegion_get_world,	   JSCSpawnRegion_set_world,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "instanceID",		JSCSpawnRegion_get_instanceID,	   JSCSpawnRegion_set_instanceID,				 JSPROP_ENUMANDPERM ),
  JS_PSGS( "minTime",			JSCSpawnRegion_get_minTime,	   JSCSpawnRegion_set_minTime,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxTime",			JSCSpawnRegion_get_maxTime,	   JSCSpawnRegion_set_maxTime,					 JSPROP_ENUMANDPERM ),
  JS_PSGS( "call",				JSCSpawnRegion_get_call,	   JSCSpawnRegion_set_call,					 JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CGuildProperties[] =
{
  JS_PSGS( "name",			JSCGuild_get_name,		   JSCGuild_set_name,			JSPROP_ENUMANDPERM ),
  JS_PSGS( "type",			JSCGuild_get_type,	       JSCGuild_set_type,			JSPROP_ENUMANDPERM ),
  JS_PSGS( "master",		JSCGuild_get_master,	   JSCGuild_set_master,			JSPROP_ENUMANDPERM ),
  JS_PSGS( "stone",			JSCGuild_get_stone,		   JSCGuild_set_stone,			JSPROP_ENUMANDPERM ),
  JS_PSG( "numMembers",		JSCGuild_get_numMembers,								JSPROP_ENUMPERMRO ),
  JS_PSG( "numRecruits",	JSCGuild_get_numRecruits,								JSPROP_ENUMPERMRO ),
  JS_PSG( "members",		JSCGuild_get_members,									JSPROP_ENUMERATE ),//JSPROP_ENUMPERMIDX
  JS_PSG( "recruits",		JSCGuild_get_recruits,									JSPROP_ENUMERATE ),//JSPROP_ENUMPERMIDX
  JS_PSGS( "charter",		JSCGuild_get_charter,	   JSCGuild_set_charter,		JSPROP_ENUMANDPERM ),
  JS_PSGS( "abbreviation",	JSCGuild_get_abbreviation, JSCGuild_set_abbreviation,	JSPROP_ENUMANDPERM ),
  JS_PSGS( "webPage",		JSCGuild_get_webPage,	   JSCGuild_set_webPage,	    JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CCharacterProps[] =
{
  JS_PSGS( "sectionID",				JSCCharacter_get_sectionID,	   JSCCharacter_set_sectionID,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "name",					JSCCharacter_get_name,	   JSCCharacter_set_name,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "origName",				JSCCharacter_get_origName,	   JSCCharacter_set_origName,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "title",					JSCCharacter_get_title,	   JSCCharacter_set_title,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "x",						JSCCharacter_get_x,	   JSCCharacter_set_x,				     JSPROP_ENUMANDPERM ),
  JS_PSGS( "y",						JSCCharacter_get_y,	   JSCCharacter_set_y,				     JSPROP_ENUMANDPERM ),
  JS_PSGS( "z",						JSCCharacter_get_z,	   JSCCharacter_set_z,				     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldX",					JSCCharacter_get_oldX,	   JSCCharacter_set_oldX,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldY",					JSCCharacter_get_oldY,	   JSCCharacter_set_oldY,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldZ",					JSCCharacter_get_oldZ,	   JSCCharacter_set_oldZ,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "id",						JSCCharacter_get_id,	   JSCCharacter_set_id,				     JSPROP_ENUMANDPERM ),
  JS_PSGS( "colour",					JSCCharacter_get_colour,	   JSCCharacter_set_colour,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "color",					JSCCharacter_get_color,	   JSCCharacter_set_color,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skin",					JSCCharacter_get_skin,	   JSCCharacter_set_skin,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hue",					JSCCharacter_get_hue,	   JSCCharacter_set_hue,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "controlSlots",			JSCCharacter_get_controlSlots,	   JSCCharacter_set_controlSlots,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "controlSlotsUsed",			JSCCharacter_get_controlSlotsUsed,	   JSCCharacter_set_controlSlotsUsed,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "orneriness",				JSCCharacter_get_orneriness,	   JSCCharacter_set_orneriness,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "owner",					JSCCharacter_get_owner,	   JSCCharacter_set_owner,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "visible",				JSCCharacter_get_visible,	   JSCCharacter_set_visible,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "serial",							JSCCharacter_get_serial,	   JSCCharacter_set_serial,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "health",							JSCCharacter_get_health,	   JSCCharacter_set_health,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "scripttrigger",					JSCCharacter_get_scripttrigger,	   JSCCharacter_set_scripttrigger,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTriggers",					JSCCharacter_get_scriptTriggers,	   JSCCharacter_set_scriptTriggers,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "worldnumber",					JSCCharacter_get_worldnumber,	   JSCCharacter_set_worldnumber,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "instanceID",						JSCCharacter_get_instanceID,	   JSCCharacter_set_instanceID,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "target",							JSCCharacter_get_target,	   JSCCharacter_set_target,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "dexterity",						JSCCharacter_get_dexterity,	   JSCCharacter_set_dexterity,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "intelligence",					JSCCharacter_get_intelligence,	   JSCCharacter_set_intelligence,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "strength",						JSCCharacter_get_strength,	   JSCCharacter_set_strength,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "actualDexterity",					JSCCharacter_get_actualDexterity,	   JSCCharacter_set_actualDexterity,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "actualIntelligence",					JSCCharacter_get_actualIntelligence,	   JSCCharacter_set_actualIntelligence,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "actualStrength",						JSCCharacter_get_actualStrength,	   JSCCharacter_set_actualStrength,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skills",							JSCCharacter_get_skills,	   JSCCharacter_set_skills,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "mana",							JSCCharacter_get_mana,	   JSCCharacter_set_mana,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "stamina",						JSCCharacter_get_stamina,	   JSCCharacter_set_stamina,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "pack",							JSCCharacter_get_pack,	   JSCCharacter_set_pack,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fame",							JSCCharacter_get_fame,	   JSCCharacter_set_fame,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "karma",							JSCCharacter_get_karma,	   JSCCharacter_set_karma,			     JSPROP_ENUMANDPERM ),
  JS_PSG( "attack",			JSCCharacter_get_attack,			JSPROP_ENUMPERMRO ),
  JS_PSGS( "canAttack",						JSCCharacter_get_canAttack,	   JSCCharacter_set_canAttack,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fleeAt",						JSCCharacter_get_fleeAt,	   JSCCharacter_set_fleeAt,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "reAttackAt",					JSCCharacter_get_reAttackAt,	   JSCCharacter_set_reAttackAt,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "brkPeaceChance",				JSCCharacter_get_brkPeaceChance,	   JSCCharacter_set_brkPeaceChance,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "setPeace",						JSCCharacter_get_setPeace,	   JSCCharacter_set_setPeace,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hunger",						JSCCharacter_get_hunger,	   JSCCharacter_set_hunger,			     JSPROP_ENUMANDPERM ),
  JS_PSG( "hungerRate",		JSCCharacter_get_hungerRate,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "thirst",						JSCCharacter_get_thirst,	   JSCCharacter_set_thirst,			     JSPROP_ENUMANDPERM ),
  JS_PSG( "thirstRate",		JSCCharacter_get_thirstRate,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "frozen",						JSCCharacter_get_frozen,	   JSCCharacter_set_frozen,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "commandlevel",							JSCCharacter_get_commandlevel,	   JSCCharacter_set_commandlevel,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "race",									JSCCharacter_get_race,	   JSCCharacter_set_race,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hasStolen",								JSCCharacter_get_hasStolen,	   JSCCharacter_set_hasStolen,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "criminal",								JSCCharacter_get_criminal,	   JSCCharacter_set_criminal,		     JSPROP_ENUMANDPERM ),
  JS_PSG( "murderer",		JSCCharacter_get_murderer,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "innocent",								JSCCharacter_get_innocent,	   JSCCharacter_set_innocent,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "murdercount",								JSCCharacter_get_murdercount,	   JSCCharacter_set_murdercount,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "neutral",								JSCCharacter_get_neutral,	   JSCCharacter_set_neutral,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "npcFlag",								JSCCharacter_get_npcFlag,	   JSCCharacter_set_npcFlag,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "npCCharacter",								JSCCharacter_get_npCCharacter,	   JSCCharacter_set_npCCharacter,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "gender",								JSCCharacter_get_gender,	   JSCCharacter_set_gender,			     JSPROP_ENUMANDPERM ),
  JS_PSG( "dead",								JSCCharacter_get_dead,			JSPROP_ENUMPERMRO ),
  JS_PSGS( "npc",								JSCCharacter_get_npc,	   JSCCharacter_set_npc,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isAwake",								JSCCharacter_get_isAwake,	   JSCCharacter_set_isAwake,			     JSPROP_ENUMANDPERM ),
  JS_PSG( "online",			JSCCharacter_get_online,			JSPROP_ENUMPERMRO ),
  JS_PSGS( "direction",						JSCCharacter_get_direction,	   JSCCharacter_set_direction,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isRunning",						JSCCharacter_get_isRunning,	   JSCCharacter_set_isRunning,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "region",						JSCCharacter_get_region,	   JSCCharacter_set_region,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "town",						JSCCharacter_get_town,	   JSCCharacter_set_town,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "guild",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "baseskills",						JSCCharacter_get_baseskills,	   JSCCharacter_set_baseskills,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillsused",						JSCCharacter_get_skillsused,	   JSCCharacter_set_skillsused,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "socket",						JSCCharacter_get_socket,	   JSCCharacter_set_socket,			     JSPROP_ENUMANDPERM ),
  JS_PSG( "isChar",			JSCCharacter_get_isChar,			JSPROP_ENUMPERMRO ),
  JS_PSG( "isItem",			JSCCharacter_get_isItem,			JSPROP_ENUMPERMRO ),
  JS_PSG( "isSpawner",		JSCCharacter_get_isSpawner,		JSPROP_ENUMPERMRO ),
  JS_PSG( "spawnSerial",	JSCCharacter_get_spawnSerial,	JSPROP_ENUMPERMRO ),
  JS_PSGS( "maxhp",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxstamina",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxmana",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldWandertype",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "wandertype",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fx1",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fy1",						JSCCharacter_get_guild,	   JSCCharacter_set_guild,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fx2",						JSCCharacter_get_fx2,	   JSCCharacter_set_fx2,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fy2",						JSCCharacter_get_fy2,	   JSCCharacter_set_fy2,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fz",						JSCCharacter_get_fz,	   JSCCharacter_set_fz,				     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isonhorse",						JSCCharacter_get_isonhorse,	   JSCCharacter_set_isonhorse,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isFlying",						JSCCharacter_get_isFlying,	   JSCCharacter_set_isFlying,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGuarded",						JSCCharacter_get_isGuarded,	   JSCCharacter_set_isGuarded,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "guarding",						JSCCharacter_get_guarding,	   JSCCharacter_set_guarding,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempdex",						JSCCharacter_get_tempdex,	   JSCCharacter_set_tempdex,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempint",						JSCCharacter_get_tempint,	   JSCCharacter_set_tempint,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempstr",						JSCCharacter_get_tempstr,	   JSCCharacter_set_tempstr,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "poison",						JSCCharacter_get_poison,	   JSCCharacter_set_poison,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "lightlevel",						JSCCharacter_get_lightlevel,	   JSCCharacter_set_lightlevel,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "vulnerable",						JSCCharacter_get_vulnerable,	   JSCCharacter_set_vulnerable,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "willhunger",						JSCCharacter_get_willhunger,	   JSCCharacter_set_willhunger,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "willthirst",						JSCCharacter_get_willthirst,	   JSCCharacter_set_willthirst,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "lodamage",						JSCCharacter_get_lodamage,	   JSCCharacter_set_lodamage,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hidamage",						JSCCharacter_get_hidamage,	   JSCCharacter_set_hidamage,		     JSPROP_ENUMANDPERM ),
  JS_PSG( "flag",			JSCCharacter_get_flag,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "atWar",						JSCCharacter_get_atWar,	   JSCCharacter_set_atWar,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "spellCast",						JSCCharacter_get_spellCast,	   JSCCharacter_set_spellCast,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isCasting",						JSCCharacter_get_isCasting,	   JSCCharacter_set_isCasting,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "priv",						JSCCharacter_get_priv,	   JSCCharacter_set_priv,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "townPriv",						JSCCharacter_get_townPriv,	   JSCCharacter_set_townPriv,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "guildTitle",						JSCCharacter_get_guildTitle,	   JSCCharacter_set_guildTitle,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hairStyle",						JSCCharacter_get_hairStyle,	   JSCCharacter_set_hairStyle,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hairColour",						JSCCharacter_get_hairColour,	   JSCCharacter_set_hairColour,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hairColor",						JSCCharacter_get_hairColor,	   JSCCharacter_set_hairColor,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "beardStyle",						JSCCharacter_get_beardStyle,	   JSCCharacter_set_beardStyle,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "beardColour",						JSCCharacter_get_beardColour,	   JSCCharacter_set_beardColour,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "beardColor",						JSCCharacter_get_beardColor,	   JSCCharacter_set_beardColor,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fontType",						JSCCharacter_get_fontType,	   JSCCharacter_set_fontType,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "sayColour",						JSCCharacter_get_sayColour,	   JSCCharacter_set_sayColour,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "emoteColour",						JSCCharacter_get_emoteColour,	   JSCCharacter_set_emoteColour,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "attacker",						JSCCharacter_get_attacker,	   JSCCharacter_set_attacker,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "raceGate",						JSCCharacter_get_raceGate,	   JSCCharacter_set_raceGate,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillLock",						JSCCharacter_get_skillLock,	   JSCCharacter_set_skillLock,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "deaths",						JSCCharacter_get_deaths,	   JSCCharacter_set_deaths,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ownerCount",						JSCCharacter_get_ownerCount,	   JSCCharacter_set_ownerCount,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "nextAct",						JSCCharacter_get_nextAct,	   JSCCharacter_set_nextAct,		     JSPROP_ENUMANDPERM ),
  JS_PSG( "petCount",		JSCCharacter_get_petCount,		JSPROP_ENUMPERMRO ),
  JS_PSG( "followerCount",	JSCCharacter_get_followerCount,		JSPROP_ENUMPERMRO ),
  JS_PSG( "ownedItemsCount",JSCCharacter_get_ownedItemsCount,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "cell",						JSCCharacter_get_cell,	   JSCCharacter_set_cell,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "allmove",						JSCCharacter_get_allmove,	   JSCCharacter_set_allmove,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "houseicons",						JSCCharacter_get_houseicons,	   JSCCharacter_set_houseicons,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "spattack",						JSCCharacter_get_spattack,	   JSCCharacter_set_spattack,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "spdelay",						JSCCharacter_get_spdelay,	   JSCCharacter_set_spdelay,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "aitype",						JSCCharacter_get_aitype,	   JSCCharacter_set_aitype,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "split",						JSCCharacter_get_split,	   JSCCharacter_set_split,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "splitchance",						JSCCharacter_get_splitchance,	   JSCCharacter_set_splitchance,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hireling",						JSCCharacter_get_hireling,	   JSCCharacter_set_hireling,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "trainer",						JSCCharacter_get_trainer,	   JSCCharacter_set_trainer,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "weight",						JSCCharacter_get_weight,	   JSCCharacter_set_weight,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "squelch",						JSCCharacter_get_squelch,	   JSCCharacter_set_squelch,		     JSPROP_ENUMANDPERM ),
  JS_PSG( "isJailed",		JSCCharacter_get_isJailed,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "magicReflect",						JSCCharacter_get_magicReflect,	   JSCCharacter_set_magicReflect,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "permanentMagicReflect",						JSCCharacter_get_permanentMagicReflect,	   JSCCharacter_set_permanentMagicReflect,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tamed",						JSCCharacter_get_tamed,	   JSCCharacter_set_tamed,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tamedHungerRate",						JSCCharacter_get_tamedHungerRate,	   JSCCharacter_set_tamedHungerRate,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tamedThirstRate",						JSCCharacter_get_tamedThirstRate,	   JSCCharacter_set_tamedThirstRate,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hungerWildChance",						JSCCharacter_get_hungerWildChance,	   JSCCharacter_set_hungerWildChance,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "thirstWildChance",						JSCCharacter_get_thirstWildChance,	   JSCCharacter_set_thirstWildChance,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "foodList",						JSCCharacter_get_foodList,	   JSCCharacter_set_foodList,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "mounted",						JSCCharacter_get_mounted,	   JSCCharacter_set_mounted,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "stabled",						JSCCharacter_get_stabled,	   JSCCharacter_set_stabled,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isUsingPotion",						JSCCharacter_get_isUsingPotion,	   JSCCharacter_set_isUsingPotion,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "stealth",						JSCCharacter_get_stealth,	   JSCCharacter_set_stealth,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillToTame",						JSCCharacter_get_skillToTame,	   JSCCharacter_set_skillToTame,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillToProv",						JSCCharacter_get_skillToProv,	   JSCCharacter_set_skillToProv,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillToPeace",						JSCCharacter_get_skillToPeace,	   JSCCharacter_set_skillToPeace,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "poisonStrength",						JSCCharacter_get_poisonStrength,	   JSCCharacter_set_poisonStrength,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isPolymorphed",						JSCCharacter_get_isPolymorphed,	   JSCCharacter_set_isPolymorphed,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isIncognito",						JSCCharacter_get_isIncognito,	   JSCCharacter_set_isIncognito,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDisguised",						JSCCharacter_get_isDisguised,	   JSCCharacter_set_isDisguised,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "canRun",						JSCCharacter_get_canRun,	   JSCCharacter_set_canRun,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isMeditating",						JSCCharacter_get_isMeditating,	   JSCCharacter_set_isMeditating,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGM",						JSCCharacter_get_isGM,	   JSCCharacter_set_isGM,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "canBroadcast",						JSCCharacter_get_canBroadcast,	   JSCCharacter_set_canBroadcast,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "singClickSer",						JSCCharacter_get_singClickSer,	   JSCCharacter_set_singClickSer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "noSkillTitles",						JSCCharacter_get_noSkillTitles,	   JSCCharacter_set_noSkillTitles,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGMPageable",						JSCCharacter_get_isGMPageable,	   JSCCharacter_set_isGMPageable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "canSnoop",						JSCCharacter_get_canSnoop,	   JSCCharacter_set_canSnoop,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isCounselor",						JSCCharacter_get_isCounselor,	   JSCCharacter_set_isCounselor,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "noNeedMana",						JSCCharacter_get_noNeedMana,	   JSCCharacter_set_noNeedMana,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDispellable",						JSCCharacter_get_isDispellable,	   JSCCharacter_set_isDispellable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "noNeedReags",						JSCCharacter_get_noNeedReags,	   JSCCharacter_set_noNeedReags,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "orgID",						JSCCharacter_get_orgID,	   JSCCharacter_set_orgID,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "orgSkin",						JSCCharacter_get_orgSkin,	   JSCCharacter_set_orgSkin,		     JSPROP_ENUMANDPERM ),
  JS_PSG( "isAnimal",		JSCCharacter_get_isAnimal,		JSPROP_ENUMPERMRO ),
  JS_PSG( "isHuman",		JSCCharacter_get_isHuman,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "isShop",						JSCCharacter_get_isShop,	   JSCCharacter_set_isShop,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxLoyalty",						JSCCharacter_get_maxLoyalty,	   JSCCharacter_set_maxLoyalty,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "loyalty",						JSCCharacter_get_loyalty,	   JSCCharacter_set_loyalty,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "loyaltyRate",						JSCCharacter_get_loyaltyRate,	   JSCCharacter_set_loyaltyRate,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "shouldSave",						JSCCharacter_get_shouldSave,	   JSCCharacter_set_shouldSave,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "origin",						JSCCharacter_get_origin,	   JSCCharacter_set_origin,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "partyLootable",						JSCCharacter_get_partyLootable,	   JSCCharacter_set_partyLootable,	     JSPROP_ENUMANDPERM ),
  JS_PSG( "party",			JSCCharacter_get_party,		JSPROP_ENUMPERMRO ),
  JS_PSGS( "multi",							JSCCharacter_get_multi,	   JSCCharacter_set_multi,			     JSPROP_ENUMANDPERM ),
  JS_PSGS( "accountNum",						JSCCharacter_get_accountNum,	   JSCCharacter_set_accountNum,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "account",						JSCCharacter_get_account,	   JSCCharacter_set_account,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "createdOn",						JSCCharacter_get_createdOn,	   JSCCharacter_set_createdOn,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "playTime",						JSCCharacter_get_playTime,	   JSCCharacter_set_playTime,		     JSPROP_ENUMANDPERM ),
  JS_PSGS( "housesOwned",						JSCCharacter_get_housesOwned,	   JSCCharacter_set_housesOwned,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "housesCoOwned",						JSCCharacter_get_housesCoOwned,	   JSCCharacter_set_housesCoOwned,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CItemProps[] =
{
  JS_PSGS( "sectionID",					JSCItem_get_sectionID,	   JSCItem_set_sectionID,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "name",						JSCItem_get_name,		   JSCItem_set_name,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "title",						JSCItem_get_title,	   JSCItem_set_title,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "x",						JSCItem_get_x,	   JSCItem_set_x,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "y",						JSCItem_get_y,	   JSCItem_set_y,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "z",						JSCItem_get_z,	   JSCItem_set_z,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldX",						JSCItem_get_oldX,	   JSCItem_set_oldX,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldY",						JSCItem_get_oldY,	   JSCItem_set_oldY,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "oldZ",						JSCItem_get_oldZ,	   JSCItem_set_oldZ,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "id",						JSCItem_get_id,	   JSCItem_set_id,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "colour",						JSCItem_get_colour,	   JSCItem_set_colour,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "color",						JSCItem_get_color,	   JSCItem_set_color,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "skin",						JSCItem_get_skin,	   JSCItem_set_skin,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hue",						JSCItem_get_hue,	   JSCItem_set_hue,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "owner",						JSCItem_get_owner,	   JSCItem_set_owner,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "visible",						JSCItem_get_visible,	   JSCItem_set_visible,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "serial",						JSCItem_get_serial,	   JSCItem_set_serial,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "health",						JSCItem_get_health,	   JSCItem_set_health,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "scripttrigger",						JSCItem_get_scripttrigger,	   JSCItem_set_scripttrigger,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTriggers",						JSCItem_get_scriptTriggers,	   JSCItem_set_scriptTriggers,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "worldnumber",						JSCItem_get_worldnumber,	   JSCItem_set_worldnumber,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "instanceID",						JSCItem_get_instanceID,	   JSCItem_set_instanceID,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "amount",						JSCItem_get_amount,	   JSCItem_set_amount,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "container",						JSCItem_get_container,	   JSCItem_set_container,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "type",						JSCItem_get_type,	   JSCItem_set_type,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "more",						JSCItem_get_more,	   JSCItem_set_more,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "more0",						JSCItem_get_more0,	   JSCItem_set_more0,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "more1",						JSCItem_get_more1,	   JSCItem_set_more1,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "more2",						JSCItem_get_more2,	   JSCItem_set_more2,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "morex",						JSCItem_get_morex,	   JSCItem_set_morex,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "morey",						JSCItem_get_morey,	   JSCItem_set_morey,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "morez",						JSCItem_get_morez,	   JSCItem_set_morez,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "movable",						JSCItem_get_movable,	   JSCItem_set_movable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "att",						JSCItem_get_att,	   JSCItem_set_att,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "layer",						JSCItem_get_layer,	   JSCItem_set_layer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "itemsinside",						JSCItem_get_itemsinside,	   JSCItem_set_itemsinside,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "totalItemCount",						JSCItem_get_totalItemCount,	   JSCItem_set_totalItemCount,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "decayable",						JSCItem_get_decayable,	   JSCItem_set_decayable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "decaytime",						JSCItem_get_decaytime,	   JSCItem_set_decaytime,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "lodamage",						JSCItem_get_lodamage,	   JSCItem_set_lodamage,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hidamage",						JSCItem_get_hidamage,	   JSCItem_set_hidamage,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ac",						JSCItem_get_ac,	   JSCItem_set_ac,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "def",						JSCItem_get_def,	   JSCItem_set_def,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistCold",						JSCItem_get_resistCold,	   JSCItem_set_resistCold,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistHeat",						JSCItem_get_resistHeat,	   JSCItem_set_resistHeat,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistLight",						JSCItem_get_resistLight,	   JSCItem_set_resistLight,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistLightning",						JSCItem_get_resistLightning,	   JSCItem_set_resistLightning,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistPoison",						JSCItem_get_resistPoison,	   JSCItem_set_resistPoison,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistRain",						JSCItem_get_resistRain,	   JSCItem_set_resistRain,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistSnow",						JSCItem_get_resistSnow,	   JSCItem_set_resistSnow,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageHeat",						JSCItem_get_damageHeat,	   JSCItem_set_damageHeat,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageCold",						JSCItem_get_damageCold,	   JSCItem_set_damageCold,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageLight",						JSCItem_get_damageLight,	   JSCItem_set_damageLight,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageLightning",						JSCItem_get_damageLightning,	   JSCItem_set_damageLightning,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damagePoison",						JSCItem_get_damagePoison,	   JSCItem_set_damagePoison,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageRain",						JSCItem_get_damageRain,	   JSCItem_set_damageRain,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageSnow",						JSCItem_get_damageSnow,	   JSCItem_set_damageSnow,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "name2",						JSCItem_get_name2,	   JSCItem_set_name2,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isChar",						JSCItem_get_isChar,	   JSCItem_set_isChar,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isItem",						JSCItem_get_isItem,	   JSCItem_set_isItem,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isSpawner",						JSCItem_get_isSpawner,	   JSCItem_set_isSpawner,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "race",						JSCItem_get_race,	   JSCItem_set_race,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxhp",						JSCItem_get_maxhp,	   JSCItem_set_maxhp,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxUses",						JSCItem_get_maxUses,	   JSCItem_set_maxUses,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "usesLeft",						JSCItem_get_usesLeft,	   JSCItem_set_usesLeft,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "rank",						JSCItem_get_rank,	   JSCItem_set_rank,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "creator",						JSCItem_get_creator,	   JSCItem_set_creator,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "poison",						JSCItem_get_poison,	   JSCItem_set_poison,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "dir",						JSCItem_get_dir,	   JSCItem_set_dir,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "wipable",						JSCItem_get_wipable,	   JSCItem_set_wipable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "buyvalue",						JSCItem_get_buyvalue,	   JSCItem_set_buyvalue,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "sellvalue",						JSCItem_get_sellvalue,	   JSCItem_set_sellvalue,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "vendorPrice",						JSCItem_get_vendorPrice,	   JSCItem_set_vendorPrice,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "restock",						JSCItem_get_restock,	   JSCItem_set_restock,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "good",						JSCItem_get_good,	   JSCItem_set_good,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "divinelock",						JSCItem_get_divinelock,	   JSCItem_set_divinelock,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "baseWeight",						JSCItem_get_baseWeight,	   JSCItem_set_baseWeight,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "weight",						JSCItem_get_weight,	   JSCItem_set_weight,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "weightMax",						JSCItem_get_weightMax,	   JSCItem_set_weightMax,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxItems",						JSCItem_get_maxItems,	   JSCItem_set_maxItems,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "strength",						JSCItem_get_strength,	   JSCItem_set_strength,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "dexterity",						JSCItem_get_dexterity,	   JSCItem_set_dexterity,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "intelligence",						JSCItem_get_intelligence,	   JSCItem_set_intelligence,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "corpse",						JSCItem_get_corpse,	   JSCItem_set_corpse,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "desc",						JSCItem_get_desc,	   JSCItem_set_desc,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "event",						JSCItem_get_event,	   JSCItem_set_event,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempLastTraded",						JSCItem_get_tempLastTraded,	   JSCItem_set_tempLastTraded,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempTimer",						JSCItem_get_tempTimer,	   JSCItem_set_tempTimer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "shouldSave",						JSCItem_get_shouldSave,	   JSCItem_set_shouldSave,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isNewbie",						JSCItem_get_isNewbie,	   JSCItem_set_isNewbie,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDispellable",						JSCItem_get_isDispellable,	   JSCItem_set_isDispellable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "madeWith",						JSCItem_get_madeWith,	   JSCItem_set_madeWith,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "entryMadeFrom",						JSCItem_get_entryMadeFrom,	   JSCItem_set_entryMadeFrom,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isPileable",						JSCItem_get_isPileable,	   JSCItem_set_isPileable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isMarkedByMaker",						JSCItem_get_isMarkedByMaker,	   JSCItem_set_isMarkedByMaker,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDyeable",						JSCItem_get_isDyeable,	   JSCItem_set_isDyeable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDamageable",						JSCItem_get_isDamageable,	   JSCItem_set_isDamageable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isWipeable",						JSCItem_get_isWipeable,	   JSCItem_set_isWipeable,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGuarded",						JSCItem_get_isGuarded,	   JSCItem_set_isGuarded,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDoorOpen",						JSCItem_get_isDoorOpen,	   JSCItem_set_isDoorOpen,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isFieldSpell",						JSCItem_get_isFieldSpell,	   JSCItem_set_isFieldSpell,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isLockedDown",						JSCItem_get_isLockedDown,	   JSCItem_set_isLockedDown,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isShieldType",						JSCItem_get_isShieldType,	   JSCItem_set_isShieldType,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isMetalType",						JSCItem_get_isMetalType,	   JSCItem_set_isMetalType,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isLeatherType",						JSCItem_get_isLeatherType,	   JSCItem_set_isLeatherType,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "canBeLockedDown",						JSCItem_get_canBeLockedDown,	   JSCItem_set_canBeLockedDown,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isContType",						JSCItem_get_isContType,	   JSCItem_set_isContType,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "carveSection",						JSCItem_get_carveSection,	   JSCItem_set_carveSection,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ammoID",						JSCItem_get_ammoID,	   JSCItem_set_ammoID,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ammoHue",						JSCItem_get_ammoHue,	   JSCItem_set_ammoHue,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ammoFX",						JSCItem_get_ammoFX,	   JSCItem_set_ammoFX,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ammoFXHue",						JSCItem_get_ammoFXHue,	   JSCItem_set_ammoFXHue,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ammoFXRender",						JSCItem_get_ammoFXRender,	   JSCItem_set_ammoFXRender,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "speed",						JSCItem_get_speed,	   JSCItem_set_speed,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "multi",						JSCItem_get_multi,	   JSCItem_set_multi,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxRange",						JSCItem_get_maxRange,	   JSCItem_set_maxRange,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "baseRange",						JSCItem_get_baseRange,	   JSCItem_set_baseRange,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "region",						JSCItem_get_region,	   JSCItem_set_region,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "spawnSerial",						JSCItem_get_spawnSerial,	   JSCItem_set_spawnSerial,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "origin",						JSCItem_get_origin,	   JSCItem_set_origin,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isItemHeld",						JSCItem_get_isItemHeld,	   JSCItem_set_isItemHeld,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "stealable",						JSCItem_get_stealable,	   JSCItem_set_stealable,	     JSPROP_ENUMANDPERM ),

  // The Following vars are specific to CSpawnItem objects
  JS_PSGS( "spawnsection",						JSCItem_get_spawnsection,	   JSCItem_set_spawnsection,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "sectionalist",						JSCItem_get_sectionalist,	   JSCItem_set_sectionalist,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "mininterval",						JSCItem_get_mininterval,	   JSCItem_set_mininterval,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxinterval",						JSCItem_get_maxinterval,	   JSCItem_set_maxinterval,	     JSPROP_ENUMANDPERM ),

  // The Following vars are specific to cMultiObj objects
  JS_PSGS( "lockdowns",						JSCItem_get_lockdowns,	   JSCItem_set_lockdowns,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxLockdowns",						JSCItem_get_maxLockdowns,	   JSCItem_set_maxLockdowns,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "trashContainers",						JSCItem_get_trashContainers,	   JSCItem_set_trashContainers,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxTrashContainers",						JSCItem_get_secureContainers,	   JSCItem_set_secureContainers,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "secureContainers",						JSCItem_get_secureContainers,	   JSCItem_set_secureContainers,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxSecureContainers",						JSCItem_get_maxSecureContainers,	   JSCItem_set_maxSecureContainers,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "friends",						JSCItem_get_friends,	   JSCItem_set_friends,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxFriends",						JSCItem_get_maxFriends,	   JSCItem_set_maxFriends,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "guests",						JSCItem_get_guests,	   JSCItem_set_guests,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxGuests",						JSCItem_get_maxGuests,	   JSCItem_set_maxGuests,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "owners",						JSCItem_get_owners,	   JSCItem_set_owners,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxOwners",						JSCItem_get_maxOwners,	   JSCItem_set_maxOwners,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "bans",						JSCItem_get_bans,	   JSCItem_set_bans,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxBans",						JSCItem_get_maxBans,	   JSCItem_set_maxBans,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "vendors",						JSCItem_get_vendors,	   JSCItem_set_vendors,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxVendors",						JSCItem_get_maxVendors,	   JSCItem_set_maxVendors,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "deed",						JSCItem_get_deed,	   JSCItem_set_deed,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "isPublic",						JSCItem_get_isPublic,	   JSCItem_set_isPublic,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "buildTimestamp",						JSCItem_get_buildTimestamp,	   JSCItem_set_buildTimestamp,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tradeTimestamp",						JSCItem_get_tradeTimestamp,	   JSCItem_set_tradeTimestamp,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "banX",						JSCItem_get_banX,	   JSCItem_set_banX,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "banY",						JSCItem_get_banY,	   JSCItem_set_banY,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CSocketProps[] =
{
  JS_PSGS( "account",			JSCSocket_get_account,	   JSCSocket_set_account,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "currentChar",			JSCSocket_get_currentChar,	   JSCSocket_set_currentChar,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "idleTimeout",			JSCSocket_get_idleTimeout,	   JSCSocket_set_idleTimeout,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "wasIdleWarned",			JSCSocket_get_wasIdleWarned,	   JSCSocket_set_wasIdleWarned,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempInt",			JSCSocket_get_tempInt,	   JSCSocket_set_tempInt,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempInt2",			JSCSocket_get_tempInt2,	   JSCSocket_set_tempInt2,	     JSPROP_ENUMANDPERM ),
  JS_PSG( "buffer",			JSCSocket_get_buffer,	     JSPROP_ENUMPERMIDX ),
  JS_PSG( "xText",			JSCSocket_get_xText,	   JSPROP_ENUMPERMIDX ),
  JS_PSG( "xText2",			JSCSocket_get_xText2,      JSPROP_ENUMPERMIDX ),
  JS_PSGS( "clickZ",			JSCSocket_get_clickZ,	   JSCSocket_set_clickZ,	     JSPROP_ENUMANDPERM ),
  JS_PSG( "addID",				JSCSocket_get_addID,     JSPROP_ENUMPERMIDX ),
  JS_PSGS( "newClient",			JSCSocket_get_newClient,	   JSCSocket_set_newClient,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "firstPacket",			JSCSocket_get_firstPacket,	   JSCSocket_set_firstPacket,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "cryptClient",			JSCSocket_get_cryptClient,	   JSCSocket_set_cryptClient,	     JSPROP_ENUMANDPERM ),
  JS_PSG( "clientIP",			JSCSocket_get_clientIP,        JSPROP_ENUMPERMIDX ),
  JS_PSGS( "walkSequence",			JSCSocket_get_walkSequence,	   JSCSocket_set_walkSequence,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "currentSpellType",			JSCSocket_get_currentSpellType,	   JSCSocket_set_currentSpellType,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "logging",			JSCSocket_get_logging,	   JSCSocket_set_logging,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "bytesSent",			JSCSocket_get_bytesSent,	   JSCSocket_set_bytesSent,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "bytesReceived",			JSCSocket_get_bytesReceived,	   JSCSocket_set_bytesReceived,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "targetOK",			JSCSocket_get_targetOK,	   JSCSocket_set_targetOK,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clickX",			JSCSocket_get_clickX,	   JSCSocket_set_clickX,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clickY",			JSCSocket_get_clickY,	   JSCSocket_set_clickY,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "pickupX",			JSCSocket_get_pickupX,	   JSCSocket_set_pickupX,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "pickupY",			JSCSocket_get_pickupY,	   JSCSocket_set_pickupY,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "pickupZ",			JSCSocket_get_pickupZ,	   JSCSocket_set_pickupZ,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "pickupSpot",			JSCSocket_get_pickupSpot,	   JSCSocket_set_pickupSpot,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "pickupSerial",			JSCSocket_get_pickupSerial,	   JSCSocket_set_pickupSerial,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "language",			JSCSocket_get_language,	   JSCSocket_set_language,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clientMajorVer",			JSCSocket_get_clientMajorVer,	   JSCSocket_set_clientMajorVer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clientMinorVer",			JSCSocket_get_clientMinorVer,	   JSCSocket_set_clientMinorVer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clientSubVer",			JSCSocket_get_clientSubVer,	   JSCSocket_set_clientSubVer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clientLetterVer",			JSCSocket_get_clientLetterVer,	   JSCSocket_set_clientLetterVer,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "clientType",			JSCSocket_get_clientType,	   JSCSocket_set_clientType,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "target",			JSCSocket_get_target,	   JSCSocket_set_target,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempObj",			JSCSocket_get_tempObj,	   JSCSocket_set_tempObj,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tempObj2",			JSCSocket_get_tempObj2,	   JSCSocket_set_tempObj2,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CSkillsProps[] =
{
  JS_PSGS( "alchemy",			JSCSkills_get_alchemy,	   JSCSkills_set_alchemy,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "anatomy",			JSCSkills_get_anatomy,	   JSCSkills_set_anatomy,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "animallore",		JSCSkills_get_animallore,	   JSCSkills_set_animallore,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "itemid",			JSCSkills_get_itemid,	   JSCSkills_set_itemid,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "armslore",			JSCSkills_get_armslore,	   JSCSkills_set_armslore,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "parrying",			JSCSkills_get_parrying,	   JSCSkills_set_parrying,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "begging",			JSCSkills_get_begging,	   JSCSkills_set_begging,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "blacksmithing",		JSCSkills_get_blacksmithing,	   JSCSkills_set_blacksmithing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "bowcraft",			JSCSkills_get_bowcraft,	   JSCSkills_set_bowcraft,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "peacemaking",		JSCSkills_get_peacemaking,	   JSCSkills_set_peacemaking,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "camping",			JSCSkills_get_camping,	   JSCSkills_set_camping,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "carpentry",			JSCSkills_get_carpentry,	   JSCSkills_set_carpentry,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "cartography",		JSCSkills_get_cartography,	   JSCSkills_set_cartography,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "cooking",			JSCSkills_get_cooking,	   JSCSkills_set_cooking,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "detectinghidden",	JSCSkills_get_detectinghidden,	   JSCSkills_set_detectinghidden,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "enticement",		JSCSkills_get_enticement,	   JSCSkills_set_enticement,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "evaluatingintel",	JSCSkills_get_evaluatingintel,	   JSCSkills_set_evaluatingintel,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "healing",			JSCSkills_get_healing,	   JSCSkills_set_healing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fishing",			JSCSkills_get_fishing,	   JSCSkills_set_fishing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "forensics",			JSCSkills_get_forensics,	   JSCSkills_set_forensics,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "herding",			JSCSkills_get_herding,	   JSCSkills_set_herding,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "hiding",			JSCSkills_get_hiding,	   JSCSkills_set_hiding,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "provocation",		JSCSkills_get_provocation,	   JSCSkills_set_provocation,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "inscription",		JSCSkills_get_inscription,	   JSCSkills_set_inscription,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "lockpicking",		JSCSkills_get_lockpicking,	   JSCSkills_set_lockpicking,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "magery",			JSCSkills_get_magery,	   JSCSkills_set_magery,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "magicresistance",	JSCSkills_get_magicresistance,	   JSCSkills_set_magicresistance,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tactics",			JSCSkills_get_tactics,	   JSCSkills_set_tactics,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "snooping",			JSCSkills_get_snooping,	   JSCSkills_set_snooping,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "musicianship",		JSCSkills_get_musicianship,	   JSCSkills_set_musicianship,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "poisoning",			JSCSkills_get_poisoning,	   JSCSkills_set_poisoning,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "archery",			JSCSkills_get_archery,	   JSCSkills_set_archery,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "spiritspeak",		JSCSkills_get_spiritspeak,	   JSCSkills_set_spiritspeak,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "stealing",			JSCSkills_get_stealing,	   JSCSkills_set_stealing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tailoring",			JSCSkills_get_tailoring,	   JSCSkills_set_tailoring,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "taming",			JSCSkills_get_taming,	   JSCSkills_set_taming,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tasteid",			JSCSkills_get_tasteid,	   JSCSkills_set_tasteid,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tinkering",			JSCSkills_get_tinkering,	   JSCSkills_set_tinkering,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "tracking",			JSCSkills_get_tracking,	   JSCSkills_set_tracking,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "veterinary",		JSCSkills_get_veterinary,	   JSCSkills_set_veterinary,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "swordsmanship",		JSCSkills_get_swordsmanship,	   JSCSkills_set_swordsmanship,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "macefighting",		JSCSkills_get_macefighting,	   JSCSkills_set_macefighting,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fencing",			JSCSkills_get_fencing,	   JSCSkills_set_fencing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "wrestling",			JSCSkills_get_wrestling,	   JSCSkills_set_wrestling,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "lumberjacking",		JSCSkills_get_lumberjacking,	   JSCSkills_set_lumberjacking,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "mining",			JSCSkills_get_mining,	   JSCSkills_set_mining,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "meditation",		JSCSkills_get_meditation,	   JSCSkills_set_meditation,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "stealth",			JSCSkills_get_stealth,	   JSCSkills_set_stealth,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "removetrap",		JSCSkills_get_removetrap,	   JSCSkills_set_removetrap,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "necromancy",		JSCSkills_get_necromancy,	   JSCSkills_set_necromancy,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "focus",				JSCSkills_get_focus,	   JSCSkills_set_focus,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "chivalry",			JSCSkills_get_chivalry,	   JSCSkills_set_chivalry,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "bushido",			JSCSkills_get_bushido,	   JSCSkills_set_bushido,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "ninjitsu",			JSCSkills_get_ninjitsu,	   JSCSkills_set_ninjitsu,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "spellweaving",		JSCSkills_get_spellweaving,	   JSCSkills_set_spellweaving,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "imbuing",			JSCSkills_get_imbuing,	   JSCSkills_set_imbuing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "mysticism",			JSCSkills_get_mysticism,	   JSCSkills_set_mysticism,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "throwing",			JSCSkills_get_throwing,	   JSCSkills_set_throwing,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "allskills",			JSCSkills_get_allskills,	   JSCSkills_set_allskills,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CGumpDataProperties[] =
{
  JS_PSGS( "buttons",	JSCGumpData_get_buttons,   JSCGumpData_set_buttons,	 JSPROP_ENUMANDPERM ),
  JS_PSGS( "IDs",		JSCGumpData_get_IDs,	   JSCGumpData_set_IDs,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CAccountProperties[] =
{
  JS_PSG(  "id",                JSCAccount_get_id,                                                   JSPROP_ENUMANDPERM ),
  JS_PSG(  "username",          JSCAccount_get_username,                                             JSPROP_ENUMANDPERM ),
  JS_PSGS( "password",          JSCAccount_get_password,          JSCAccount_set_password,           JSPROP_ENUMANDPERM ),
  JS_PSG(  "flags",             JSCAccount_get_flags,                                                JSPROP_ENUMANDPERM ),
  JS_PSGS( "path",              JSCAccount_get_path,              JSCAccount_set_path,               JSPROP_ENUMANDPERM ),
  JS_PSGS( "comment",           JSCAccount_get_comment,           JSCAccount_set_comment,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "timeban",           JSCAccount_get_timeban,           JSCAccount_set_timeban,            JSPROP_ENUMANDPERM ),
  JS_PSG(  "firstLogin",        JSCAccount_get_firstLogin,                                           JSPROP_ENUMANDPERM ),
  JS_PSGS( "totalPlayTime",     JSCAccount_get_totalPlayTime,     JSCAccount_set_totalPlayTime,      JSPROP_ENUMANDPERM ),
  JS_PSG(  "character1",        JSCAccount_get_character1,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "character2",        JSCAccount_get_character2,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "character3",        JSCAccount_get_character3,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "character4",        JSCAccount_get_character4,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "character5",        JSCAccount_get_character5,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "character6",        JSCAccount_get_character6,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "character7",        JSCAccount_get_character7,                                           JSPROP_ENUMANDPERM ),
  JS_PSG(  "currentChar",       JSCAccount_get_currentChar,                                          JSPROP_ENUMANDPERM ),
  JS_PSG(  "lastIP",            JSCAccount_get_lastIP,                                               JSPROP_ENUMANDPERM ),

  // Flags
  JS_PSGS( "isBanned",          JSCAccount_get_isBanned,           JSCAccount_set_isBanned,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSuspended",       JSCAccount_get_isSuspended,        JSCAccount_set_isSuspended,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "isPublic",          JSCAccount_get_isPublic,           JSCAccount_set_isPublic,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "isOnline",          JSCAccount_get_isOnline,           JSCAccount_set_isOnline,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot1Blocked",    JSCAccount_get_isSlot1Blocked,     JSCAccount_set_isSlot1Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot2Blocked",    JSCAccount_get_isSlot2Blocked,     JSCAccount_set_isSlot2Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot3Blocked",    JSCAccount_get_isSlot3Blocked,     JSCAccount_set_isSlot3Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot4Blocked",    JSCAccount_get_isSlot4Blocked,     JSCAccount_set_isSlot4Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot5Blocked",    JSCAccount_get_isSlot5Blocked,     JSCAccount_set_isSlot5Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot6Blocked",    JSCAccount_get_isSlot6Blocked,     JSCAccount_set_isSlot6Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSlot7Blocked",    JSCAccount_get_isSlot7Blocked,     JSCAccount_set_isSlot7Blocked,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isYoung",           JSCAccount_get_isYoung,            JSCAccount_set_isYoung,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "unused10",          JSCAccount_get_unused10,           JSCAccount_set_unused10,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSeer",            JSCAccount_get_isSeer,             JSCAccount_set_isSeer,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "isCounselor",       JSCAccount_get_isCounselor,        JSCAccount_set_isCounselor,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGM",              JSCAccount_get_isGM,               JSCAccount_set_isGM,              JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CConsoleProperties[] =
{
  JS_PSGS( "mode",		JSCConsole_get_mode,	   JSCConsole_set_mode,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "logEcho",	JSCConsole_get_logEcho,	   JSCConsole_set_logEcho,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CScriptSectionProperties[] =
{
  JS_PSG( "numTags",    JSCScriptSection_get_numTags,      JSPROP_ENUMANDPERM ),
  JS_PSG( "atEnd",      JSCScriptSection_get_atEnd,        JSPROP_ENUMANDPERM ),
  JS_PSG( "atEndTags",  JSCScriptSection_get_atEndTags,    JSPROP_ENUMANDPERM ),
  JS_PS_END
};

// clang-format off
inline JSPropertySpec CResourceProperties[] =
{
  JS_PSGS( "logAmount",  JSCResource_get_logAmount,   JSCResource_set_logAmount,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "logTime",    JSCResource_get_logTime,     JSCResource_set_logTime,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "oreAmount",  JSCResource_get_oreAmount,   JSCResource_set_oreAmount,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "oreTime",    JSCResource_get_oreTime,     JSCResource_set_oreTime,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "fishAmount", JSCResource_get_fishAmount,  JSCResource_set_fishAmount, JSPROP_ENUMANDPERM ),
  JS_PSGS( "fishTime",   JSCResource_get_fishTime,    JSCResource_set_fishTime,   JSPROP_ENUMANDPERM ),
  JS_PS_END
};
// clang-format on

inline JSPropertySpec CPartyProperties[] =
{
  JS_PSGS( "leader",		JSCParty_get_leader,	   JSCParty_set_leader,	     JSPROP_ENUMANDPERM ),
  JS_PSG( "memberCount",	JSCParty_get_memberCount,	     JSPROP_ENUMPERMRO ),
  JS_PSGS( "isNPC",			JSCParty_get_isNPC,		   JSCParty_set_isNPC,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CScriptProperties[] =
{
  JS_PSGS( "script_id",		JSCScript_get_script_id,	   JSCScript_set_script_id,	     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

#endif
