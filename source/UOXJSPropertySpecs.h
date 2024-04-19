//o------------------------------------------------------------------------------------------------o
//|  File    -  UOXJSPropertyFuncts.cpp
//|  Date    -  12/14/2001
//o------------------------------------------------------------------------------------------------o
//|  Notes    -  1.0    14th December, 2001 Initial implementation
//|              Defines the property specifications for race, region, guild,
//|              item and chars
//o------------------------------------------------------------------------------------------------o
#ifndef __UOXJSPropertySpecs__
#define __UOXJSPropertySpecs__

#include "UOXJSPropertyEnums.h"
#include "enums.h"

#define CLSDECLG( main, attr ) JS##main##_get_##attr
#define CLSDECLS( main, attr ) JS##main##_set_##attr
#define PDECLGS( main, attr ) #attr, CLSDECLG( main, attr ), CLSDECLS( main, attr )
#define FDCLG( main, attr ) bool CLSDECLG ## ( main, attr )##(JSContext *cx, unsigned int argc, JS::Value *vp)
#define FDCLS( main, attr ) bool CLSDECLS ## ( main, attr )##(JSContext *cx, unsigned int argc, JS::Value *vp)

#define DECL_GET( main, attr ) FDCLG( main, attr ) ## ;
#define DECL_SET( main, attr ) FDCLS( main, attr ) ## ;

// Lifted from the Mozilla headers - revisit if these change!  Change UX to JS
#define UX_PSG( main, attr, attributes )                                \
JSPropertySpec::nativeAccessors( #attr, CheckAccessorAttrs<attributes>(), CLSDECLG( main, attr ), nullptr )
#define UX_PSGS( main, attr, attributes )                                 \
JSPropertySpec::nativeAccessors( #attr, CheckAccessorAttrs<attributes>(), CLSDECLG( main, attr ), nullptr, CLSDECLS( main, attr ), nullptr )
//


#define FNARGS                              \
  auto args = JS::CallArgsFromVp(argc, vp); \
  JS::RootedObject thisObj(cx);             \
  if (!args.computeThis(cx, &thisObj))      \
    return false;                           \

#define IMPL_GET( main, attr, type, method, accessor ) \
FDCLG( main, attr ) { \
  FNARGS \
  auto priv = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0); \
  args.rval().method(priv->accessor); \
  return true; \
}

#define IMPL_GET_NP(main, attr, method, accessor)                                                          \
FDCLG( main, attr ) {                                                                                      \
  FNARGS                                                                                                   \
  args.rval().method( accessor );                                                                          \
  return true;                                                                                             \
}

#define IMPL_GETS(main, attr, type, method, accessor)                         \
FDCLG( main, attr ) {                                                         \
  FNARGS                                                                      \
  auto priv = JS::GetMaybePtrFromReservedSlot< type >(thisObj, 0);            \
  args.rval().method( JS_NewStringCopyZ( cx, priv->accessor ) );              \
  return true;                                                                \
}

#define IMPL_GET_OBJ( main, attr, type, method, accessor ) \
FDCLG( main, attr ) { \
  FNARGS \
  auto priv = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0); \
  SERIAL TempSerial = INVALIDSERIAL; \
  if( !ValidateObject( priv )) \
    return false; \
  args.rval().method(priv->accessor); \
  return true; \
}

#define IMPL_GETS_OBJ(main, attr, type, method, accessor) \
FDCLG( main, attr ) { \
  FNARGS \
  auto priv = JS::GetMaybePtrFromReservedSlot< type >(thisObj, 0); \
  SERIAL TempSerial = INVALIDSERIAL; \
  if( !ValidateObject( priv )) \
    return false; \
  args.rval().method( JS_NewStringCopyZ( cx, priv->accessor ) ); \
  return true; \
}

#define IMPL_SET(main, attr, type, method, accessor)                                                             \
FDCLS( main, attr ) {                                                                                            \
  FNARGS                                                                                                         \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor(args.get(0).method());                                                                          \
  if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx))) {                                         \
  }                                                                                                              \
  return true;                                                                                                   \
}

#define IMPL_SET_DIR(main, attr, type, method, accessor)                                                         \
FDCLS( main, attr ) {                                    \
  FNARGS                                                                                                         \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor    = args.get(0).method();                                                                      \
  if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx))) {                                         \
  }                                                                                                              \
  return true;                                                                                                   \
}

#define IMPL_SETS(main, attr, type, method, accessor)                                                            \
FDCLS( main, attr ) {                                    \
  FNARGS                                                                                                         \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor(convertToString(cx, args.get(0).method()));                                                     \
  if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx))) {                                         \
  }                                                                                                              \
  return true;                                                                                                   \
}

#define IMPL_SETS_DIR(main, attr, type, method, accessor)                                                        \
FDCLS( main, attr ) {                                    \
  FNARGS                                                                                                         \
  auto priv         = JS::GetMaybePtrFromReservedSlot<type>(thisObj, 0);                                         \
  auto origScript   = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));                                         \
  auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));                                       \
  priv->accessor    = convertToString(cx, args.get(0).method());                                                 \
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
FDCLS( main, attr ) {                                                                                            \
  auto args = JS::CallArgsFromVp(argc, vp);                                                                      \
  accessor(args.get(0).method());                                                                                \
  return true;                                                                                                   \
}

#define DECL_GET_SET( main, attr ) \
DECL_GET( main, attr ) \
DECL_SET( main, attr )

// Forward declares
DECL_GET( CSpell, action )
DECL_GET( CSpell, aggressiveSpell )
DECL_GET( CSpell, ash )
DECL_GET( CSpell, baseDmg )
DECL_GET( CSpell, circle )
DECL_GET( CSpell, damageDelay )
DECL_GET( CSpell, delay )
DECL_GET( CSpell, drake )
DECL_GET( CSpell, enabled )
DECL_GET( CSpell, fieldSpell )
DECL_GET( CSpell, garlic )
DECL_GET( CSpell, ginseng )
DECL_GET( CSpell, health )
DECL_GET( CSpell, highSkill )
DECL_GET( CSpell, id )
DECL_GET( CSpell, lowSkill )
DECL_GET( CSpell, mana )
DECL_GET( CSpell, mantra )
DECL_GET( CSpell, moss )
DECL_GET( CSpell, name )
DECL_GET( CSpell, pearl )
DECL_GET( CSpell, recoveryDelay )
DECL_GET( CSpell, reflectable )
DECL_GET( CSpell, requireChar )
DECL_GET( CSpell, requireItem )
DECL_GET( CSpell, requireLocation )
DECL_GET( CSpell, requireTarget )
DECL_GET( CSpell, resistable )
DECL_GET( CSpell, scrollHigh )
DECL_GET( CSpell, scrollLow )
DECL_GET( CSpell, shade )
DECL_GET( CSpell, silk )
DECL_GET( CSpell, soundEffect )
DECL_GET( CSpell, stamina )
DECL_GET( CSpell, strToSay )
DECL_GET( CSpell, travelSpell )

// Global Skills
DECL_GET_SET( CGlobalSkill, dexterity )
DECL_GET_SET( CGlobalSkill, intelligence )
DECL_GET_SET( CGlobalSkill, madeWord )
DECL_GET_SET( CGlobalSkill, name )
DECL_GET_SET( CGlobalSkill, scriptID )
DECL_GET_SET( CGlobalSkill, skillDelay )
DECL_GET_SET( CGlobalSkill, strength )

// Create Entry
DECL_GET_SET( CCreateEntry, addItem )
DECL_GET_SET( CCreateEntry, avgMaxSkill )
DECL_GET_SET( CCreateEntry, avgMinSkill )
DECL_GET_SET( CCreateEntry, colour )
DECL_GET_SET( CCreateEntry, delay )
DECL_GET_SET( CCreateEntry, id )
DECL_GET_SET( CCreateEntry, maxRank )
DECL_GET_SET( CCreateEntry, minRank )
DECL_GET_SET( CCreateEntry, name )
DECL_GET_SET( CCreateEntry, resources )
DECL_GET_SET( CCreateEntry, skills )
DECL_GET_SET( CCreateEntry, sound )
DECL_GET_SET( CCreateEntry, spell )

// Timer Properties
DECL_GET( CTimer, ANTISPAM )
DECL_GET( CTimer, CRIMFLAG )
DECL_GET( CTimer, EVADETIME )
DECL_GET( CTimer, FIREFIELDTICK )
DECL_GET( CTimer, FLYINGTOGGLE )
DECL_GET( CTimer, HUNGER )
DECL_GET( CTimer, IDLEANIMTIME )
DECL_GET( CTimer, INVIS )
DECL_GET( CTimer, LOGOUT )
DECL_GET( CTimer, LOYALTYTIME )
DECL_GET( CTimer, MOVETIME )
DECL_GET( CTimer, MURDERRATE )
DECL_GET( CTimer, PARAFIELDTICK )
DECL_GET( CTimer, PEACETIMER )
DECL_GET( CTimer, POISONFIELDTICK )
DECL_GET( CTimer, POISONTEXT )
DECL_GET( CTimer, POISONTIME )
DECL_GET( CTimer, POISONWEAROFF )
DECL_GET( CTimer, SOCK_FISHING )
DECL_GET( CTimer, SOCK_MUTETIME )
DECL_GET( CTimer, SOCK_OBJDELAY )
DECL_GET( CTimer, SOCK_SKILLDELAY )
DECL_GET( CTimer, SOCK_SPIRITSPEAK )
DECL_GET( CTimer, SOCK_TRACKING )
DECL_GET( CTimer, SOCK_TRACKINGDISPLAY )
DECL_GET( CTimer, SOCK_TRAFFICWARDEN )
DECL_GET( CTimer, SPATIMER )
DECL_GET( CTimer, SPELLRECOVERYTIME )
DECL_GET( CTimer, SPELLTIME )
DECL_GET( CTimer, SUMMONTIME )
DECL_GET( CTimer, THIRST )
DECL_GET( CTimer, TIMEOUT )
DECL_GET( CTimer, YOUNGHEAL )
DECL_GET( CTimer, YOUNGMESSAGE )

// Race Properties
DECL_GET_SET( CRace, armourClass )
DECL_GET_SET( CRace, genderRestrict )
DECL_GET_SET( CRace, id )
DECL_GET_SET( CRace, isPlayerRace )
DECL_GET_SET( CRace, languageSkillMin )
DECL_GET_SET( CRace, magicResistance )
DECL_GET_SET( CRace, name )
DECL_GET_SET( CRace, nightVision )
DECL_GET_SET( CRace, poisonResistance )
DECL_GET_SET( CRace, requiresBeard )
DECL_GET_SET( CRace, requiresNoBeard )
DECL_GET_SET( CRace, skillAdjustment )
DECL_GET_SET( CRace, visibleDistance )
DECL_GET_SET( CRace, weakToWeather )

// Region Properties
DECL_GET_SET( CRegion, appearance )
DECL_GET_SET( CRegion, canCastAggressive )
DECL_GET_SET( CRegion, canGate )
DECL_GET_SET( CRegion, canMark )
DECL_GET_SET( CRegion, canPlaceHouse )
DECL_GET_SET( CRegion, canRecall )
DECL_GET_SET( CRegion, canTeleport )
DECL_GET_SET( CRegion, chanceBigOre )
DECL_GET_SET( CRegion, health )
DECL_GET_SET( CRegion, id )
DECL_GET_SET( CRegion, instanceID )
DECL_GET_SET( CRegion, isDungeon )
DECL_GET_SET( CRegion, isGuarded )
DECL_GET_SET( CRegion, isSafeZone )
DECL_GET_SET( CRegion, mayor )
DECL_GET( CRegion, members )
DECL_GET_SET( CRegion, music )
DECL_GET_SET( CRegion, name )
DECL_GET_SET( CRegion, numGuards )
DECL_GET( CRegion, numOrePrefs )
DECL_GET_SET( CRegion, owner )
DECL_GET( CRegion, population )
DECL_GET_SET( CRegion, race )
DECL_GET_SET( CRegion, reserves )
DECL_GET_SET( CRegion, scriptTrigger )
DECL_GET_SET( CRegion, scriptTriggers )
DECL_GET_SET( CRegion, tax )
DECL_GET_SET( CRegion, taxes )
DECL_GET_SET( CRegion, taxResource )
DECL_GET_SET( CRegion, weather )
DECL_GET_SET( CRegion, worldNumber )

// Spawn Region Properties
DECL_GET_SET( CSpawnRegion, call )
DECL_GET_SET( CSpawnRegion, defZ )
DECL_GET_SET( CSpawnRegion, instanceID )
DECL_GET_SET( CSpawnRegion, isSpawner )
DECL_GET_SET( CSpawnRegion, item )
DECL_GET_SET( CSpawnRegion, itemCount )
DECL_GET_SET( CSpawnRegion, itemList )
DECL_GET_SET( CSpawnRegion, maxItems )
DECL_GET_SET( CSpawnRegion, maxNpcs )
DECL_GET_SET( CSpawnRegion, maxTime )
DECL_GET_SET( CSpawnRegion, minTime )
DECL_GET_SET( CSpawnRegion, name )
DECL_GET_SET( CSpawnRegion, npc )
DECL_GET_SET( CSpawnRegion, npcCount )
DECL_GET_SET( CSpawnRegion, npcList )
DECL_GET_SET( CSpawnRegion, onlyOutside )
DECL_GET_SET( CSpawnRegion, prefZ )
DECL_GET_SET( CSpawnRegion, regionNum )
DECL_GET_SET( CSpawnRegion, world )
DECL_GET_SET( CSpawnRegion, x1 )
DECL_GET_SET( CSpawnRegion, x2 )
DECL_GET_SET( CSpawnRegion, y1 )
DECL_GET_SET( CSpawnRegion, y2 )

// Guild Properties
DECL_GET_SET( CGuild, abbreviation )
DECL_GET_SET( CGuild, charter )
DECL_GET_SET( CGuild, master )
DECL_GET( CGuild, members )
DECL_GET_SET( CGuild, name )
DECL_GET( CGuild, numMembers )
DECL_GET( CGuild, numRecruits )
DECL_GET( CGuild, recruits )
DECL_GET_SET( CGuild, stone )
DECL_GET_SET( CGuild, type )
DECL_GET_SET( CGuild, webPage )

// Character Properties
DECL_GET_SET( CCharacter, account )
DECL_GET_SET( CCharacter, accountNum )
DECL_GET_SET( CCharacter, actualDexterity )
DECL_GET_SET( CCharacter, actualIntelligence )
DECL_GET_SET( CCharacter, actualStrength )
DECL_GET_SET( CCharacter, aitype )
DECL_GET_SET( CCharacter, allmove )
DECL_GET_SET( CCharacter, attacker )
DECL_GET_SET( CCharacter, atWar )
DECL_GET_SET( CCharacter, baseskills )
DECL_GET_SET( CCharacter, beardColor )
DECL_GET_SET( CCharacter, beardColour )
DECL_GET_SET( CCharacter, beardStyle )
DECL_GET_SET( CCharacter, brkPeaceChance )
DECL_GET_SET( CCharacter, canAttack )
DECL_GET_SET( CCharacter, canBroadcast )
DECL_GET_SET( CCharacter, canRun )
DECL_GET_SET( CCharacter, canSnoop )
DECL_GET_SET( CCharacter, cell )
DECL_GET_SET( CCharacter, color )
DECL_GET_SET( CCharacter, colour )
DECL_GET_SET( CCharacter, commandlevel )
DECL_GET_SET( CCharacter, controlSlots )
DECL_GET_SET( CCharacter, controlSlotsUsed )
DECL_GET_SET( CCharacter, createdOn )
DECL_GET_SET( CCharacter, criminal )
DECL_GET_SET( CCharacter, dead )
DECL_GET_SET( CCharacter, deaths )
DECL_GET_SET( CCharacter, dexterity )
DECL_GET_SET( CCharacter, direction )
DECL_GET_SET( CCharacter, emoteColour )
DECL_GET_SET( CCharacter, fame )
DECL_GET_SET( CCharacter, fleeAt )
DECL_GET_SET( CCharacter, fontType )
DECL_GET_SET( CCharacter, foodList )
DECL_GET_SET( CCharacter, frozen )
DECL_GET_SET( CCharacter, fx1 )
DECL_GET_SET( CCharacter, fx2 )
DECL_GET_SET( CCharacter, fy1 )
DECL_GET_SET( CCharacter, fy2 )
DECL_GET_SET( CCharacter, fz )
DECL_GET_SET( CCharacter, gender )
DECL_GET_SET( CCharacter, guarding )
DECL_GET_SET( CCharacter, guild )
DECL_GET_SET( CCharacter, guildTitle )
DECL_GET_SET( CCharacter, hairColor )
DECL_GET_SET( CCharacter, hairColour )
DECL_GET_SET( CCharacter, hairStyle )
DECL_GET_SET( CCharacter, hasStolen )
DECL_GET_SET( CCharacter, health )
DECL_GET_SET( CCharacter, hidamage )
DECL_GET_SET( CCharacter, hireling )
DECL_GET_SET( CCharacter, houseicons )
DECL_GET_SET( CCharacter, housesCoOwned )
DECL_GET_SET( CCharacter, housesOwned )
DECL_GET_SET( CCharacter, hue )
DECL_GET_SET( CCharacter, hunger )
DECL_GET_SET( CCharacter, hungerWildChance )
DECL_GET_SET( CCharacter, id )
DECL_GET_SET( CCharacter, innocent )
DECL_GET_SET( CCharacter, instanceID )
DECL_GET_SET( CCharacter, intelligence )
DECL_GET_SET( CCharacter, isAwake )
DECL_GET_SET( CCharacter, isCasting )
DECL_GET_SET( CCharacter, isCounselor )
DECL_GET_SET( CCharacter, isDisguised )
DECL_GET_SET( CCharacter, isDispellable )
DECL_GET_SET( CCharacter, isFlying )
DECL_GET_SET( CCharacter, isGM )
DECL_GET_SET( CCharacter, isGMPageable )
DECL_GET_SET( CCharacter, isGuarded )
DECL_GET_SET( CCharacter, isIncognito )
DECL_GET_SET( CCharacter, isMeditating )
DECL_GET_SET( CCharacter, isonhorse )
DECL_GET_SET( CCharacter, isPolymorphed )
DECL_GET_SET( CCharacter, isRunning )
DECL_GET_SET( CCharacter, isShop )
DECL_GET_SET( CCharacter, isUsingPotion )
DECL_GET_SET( CCharacter, karma )
DECL_GET_SET( CCharacter, lightlevel )
DECL_GET_SET( CCharacter, lodamage )
DECL_GET_SET( CCharacter, loyalty )
DECL_GET_SET( CCharacter, loyaltyRate )
DECL_GET_SET( CCharacter, magicReflect )
DECL_GET_SET( CCharacter, mana )
DECL_GET_SET( CCharacter, maxhp )
DECL_GET_SET( CCharacter, maxLoyalty )
DECL_GET_SET( CCharacter, maxmana )
DECL_GET_SET( CCharacter, maxstamina )
DECL_GET_SET( CCharacter, mounted )
DECL_GET_SET( CCharacter, multi )
DECL_GET_SET( CCharacter, murdercount )
DECL_GET_SET( CCharacter, name )
DECL_GET_SET( CCharacter, neutral )
DECL_GET_SET( CCharacter, nextAct )
DECL_GET_SET( CCharacter, noNeedMana )
DECL_GET_SET( CCharacter, noNeedReags )
DECL_GET_SET( CCharacter, noSkillTitles )
DECL_GET_SET( CCharacter, npc )
DECL_GET_SET( CCharacter, npCCharacter )
DECL_GET_SET( CCharacter, npcFlag )
DECL_GET_SET( CCharacter, oldWandertype )
DECL_GET_SET( CCharacter, oldX )
DECL_GET_SET( CCharacter, oldY )
DECL_GET_SET( CCharacter, oldZ )
DECL_GET_SET( CCharacter, orgID )
DECL_GET_SET( CCharacter, orgSkin )
DECL_GET_SET( CCharacter, origin )
DECL_GET_SET( CCharacter, origName )
DECL_GET_SET( CCharacter, orneriness )
DECL_GET_SET( CCharacter, owner )
DECL_GET_SET( CCharacter, ownerCount )
DECL_GET_SET( CCharacter, pack )
DECL_GET_SET( CCharacter, partyLootable )
DECL_GET_SET( CCharacter, permanentMagicReflect )
DECL_GET_SET( CCharacter, playTime )
DECL_GET_SET( CCharacter, poison )
DECL_GET_SET( CCharacter, poisonStrength )
DECL_GET_SET( CCharacter, priv )
DECL_GET_SET( CCharacter, race )
DECL_GET_SET( CCharacter, raceGate )
DECL_GET_SET( CCharacter, reAttackAt )
DECL_GET_SET( CCharacter, region )
DECL_GET_SET( CCharacter, sayColour )
DECL_GET_SET( CCharacter, scripttrigger )
DECL_GET_SET( CCharacter, scriptTriggers )
DECL_GET_SET( CCharacter, sectionID )
DECL_GET_SET( CCharacter, serial )
DECL_GET_SET( CCharacter, setPeace )
DECL_GET_SET( CCharacter, shouldSave )
DECL_GET_SET( CCharacter, singClickSer )
DECL_GET_SET( CCharacter, skillLock )
DECL_GET_SET( CCharacter, skills )
DECL_GET_SET( CCharacter, skillsused )
DECL_GET_SET( CCharacter, skillToPeace )
DECL_GET_SET( CCharacter, skillToProv )
DECL_GET_SET( CCharacter, skillToTame )
DECL_GET_SET( CCharacter, skin )
DECL_GET_SET( CCharacter, socket )
DECL_GET_SET( CCharacter, spattack )
DECL_GET_SET( CCharacter, spdelay )
DECL_GET_SET( CCharacter, spellCast )
DECL_GET_SET( CCharacter, split )
DECL_GET_SET( CCharacter, splitchance )
DECL_GET_SET( CCharacter, squelch )
DECL_GET_SET( CCharacter, stabled )
DECL_GET_SET( CCharacter, stamina )
DECL_GET_SET( CCharacter, stealth )
DECL_GET_SET( CCharacter, strength )
DECL_GET_SET( CCharacter, tamed )
DECL_GET_SET( CCharacter, tamedHungerRate )
DECL_GET_SET( CCharacter, tamedThirstRate )
DECL_GET_SET( CCharacter, target )
DECL_GET_SET( CCharacter, tempdex )
DECL_GET_SET( CCharacter, tempint )
DECL_GET_SET( CCharacter, tempstr )
DECL_GET_SET( CCharacter, thirst )
DECL_GET_SET( CCharacter, thirstWildChance )
DECL_GET_SET( CCharacter, title )
DECL_GET_SET( CCharacter, town )
DECL_GET_SET( CCharacter, townPriv )
DECL_GET_SET( CCharacter, trainer )
DECL_GET_SET( CCharacter, visible )
DECL_GET_SET( CCharacter, vulnerable )
DECL_GET_SET( CCharacter, wandertype )
DECL_GET_SET( CCharacter, weight )
DECL_GET_SET( CCharacter, willhunger )
DECL_GET_SET( CCharacter, willthirst )
DECL_GET_SET( CCharacter, worldnumber )
DECL_GET_SET( CCharacter, x )
DECL_GET_SET( CCharacter, y )
DECL_GET_SET( CCharacter, z )
DECL_GET( CCharacter, attack )
DECL_GET( CCharacter, flag )
DECL_GET( CCharacter, followerCount )
DECL_GET( CCharacter, hungerRate )
DECL_GET( CCharacter, isAnimal )
DECL_GET( CCharacter, isChar )
DECL_GET( CCharacter, isHuman )
DECL_GET( CCharacter, isItem )
DECL_GET( CCharacter, isJailed )
DECL_GET( CCharacter, isSpawner )
DECL_GET( CCharacter, murderer )
DECL_GET( CCharacter, online )
DECL_GET( CCharacter, ownedItemsCount )
DECL_GET( CCharacter, party )
DECL_GET( CCharacter, petCount )
DECL_GET( CCharacter, spawnSerial )
DECL_GET( CCharacter, thirstRate )

// Item Properties
DECL_GET_SET( CItem, ac )
DECL_GET_SET( CItem, ammoFX )
DECL_GET_SET( CItem, ammoFXHue )
DECL_GET_SET( CItem, ammoFXRender )
DECL_GET_SET( CItem, ammoHue )
DECL_GET_SET( CItem, ammoID )
DECL_GET_SET( CItem, amount )
DECL_GET_SET( CItem, att )
DECL_GET_SET( CItem, bans )
DECL_GET_SET( CItem, banX )
DECL_GET_SET( CItem, banY )
DECL_GET_SET( CItem, baseRange )
DECL_GET_SET( CItem, baseWeight )
DECL_GET_SET( CItem, buildTimestamp )
DECL_GET_SET( CItem, buyvalue )
DECL_GET_SET( CItem, canBeLockedDown )
DECL_GET_SET( CItem, carveSection )
DECL_GET_SET( CItem, color )
DECL_GET_SET( CItem, colour )
DECL_GET_SET( CItem, container )
DECL_GET_SET( CItem, corpse )
DECL_GET_SET( CItem, creator )
DECL_GET_SET( CItem, damageCold )
DECL_GET_SET( CItem, damageHeat )
DECL_GET_SET( CItem, damageLight )
DECL_GET_SET( CItem, damageLightning )
DECL_GET_SET( CItem, damagePoison )
DECL_GET_SET( CItem, damageRain )
DECL_GET_SET( CItem, damageSnow )
DECL_GET_SET( CItem, decayable )
DECL_GET_SET( CItem, decaytime )
DECL_GET_SET( CItem, deed )
DECL_GET_SET( CItem, def )
DECL_GET_SET( CItem, desc )
DECL_GET_SET( CItem, dexterity )
DECL_GET_SET( CItem, dir )
DECL_GET_SET( CItem, divinelock )
DECL_GET_SET( CItem, entryMadeFrom )
DECL_GET_SET( CItem, event )
DECL_GET_SET( CItem, friends )
DECL_GET_SET( CItem, good )
DECL_GET_SET( CItem, guests )
DECL_GET_SET( CItem, health )
DECL_GET_SET( CItem, hidamage )
DECL_GET_SET( CItem, hue )
DECL_GET_SET( CItem, id )
DECL_GET_SET( CItem, instanceID )
DECL_GET_SET( CItem, intelligence )
DECL_GET_SET( CItem, isChar )
DECL_GET_SET( CItem, isContType )
DECL_GET_SET( CItem, isDamageable )
DECL_GET_SET( CItem, isDispellable )
DECL_GET_SET( CItem, isDoorOpen )
DECL_GET_SET( CItem, isDyeable )
DECL_GET_SET( CItem, isFieldSpell )
DECL_GET_SET( CItem, isGuarded )
DECL_GET_SET( CItem, isItem )
DECL_GET_SET( CItem, isItemHeld )
DECL_GET_SET( CItem, isLeatherType )
DECL_GET_SET( CItem, isLockedDown )
DECL_GET_SET( CItem, isMarkedByMaker )
DECL_GET_SET( CItem, isMetalType )
DECL_GET_SET( CItem, isNewbie )
DECL_GET_SET( CItem, isPileable )
DECL_GET_SET( CItem, isPublic )
DECL_GET_SET( CItem, isShieldType )
DECL_GET_SET( CItem, isSpawner )
DECL_GET_SET( CItem, isWipeable )
DECL_GET_SET( CItem, itemsinside )
DECL_GET_SET( CItem, layer )
DECL_GET_SET( CItem, lockdowns )
DECL_GET_SET( CItem, lodamage )
DECL_GET_SET( CItem, madeWith )
DECL_GET_SET( CItem, maxBans )
DECL_GET_SET( CItem, maxFriends )
DECL_GET_SET( CItem, maxGuests )
DECL_GET_SET( CItem, maxhp )
DECL_GET_SET( CItem, maxinterval )
DECL_GET_SET( CItem, maxItems )
DECL_GET_SET( CItem, maxLockdowns )
DECL_GET_SET( CItem, maxOwners )
DECL_GET_SET( CItem, maxRange )
DECL_GET_SET( CItem, maxSecureContainers )
DECL_GET_SET( CItem, maxTrashContainers )
DECL_GET_SET( CItem, maxUses )
DECL_GET_SET( CItem, maxVendors )
DECL_GET_SET( CItem, mininterval )
DECL_GET_SET( CItem, more )
DECL_GET_SET( CItem, more0 )
DECL_GET_SET( CItem, more1 )
DECL_GET_SET( CItem, more2 )
DECL_GET_SET( CItem, morex )
DECL_GET_SET( CItem, morey )
DECL_GET_SET( CItem, morez )
DECL_GET_SET( CItem, movable )
DECL_GET_SET( CItem, multi )
DECL_GET_SET( CItem, name )
DECL_GET_SET( CItem, name2 )
DECL_GET_SET( CItem, oldX )
DECL_GET_SET( CItem, oldY )
DECL_GET_SET( CItem, oldZ )
DECL_GET_SET( CItem, origin )
DECL_GET_SET( CItem, origName )
DECL_GET_SET( CItem, owner )
DECL_GET_SET( CItem, owners )
DECL_GET_SET( CItem, poison )
DECL_GET_SET( CItem, race )
DECL_GET_SET( CItem, rank )
DECL_GET_SET( CItem, region )
DECL_GET_SET( CItem, resistCold )
DECL_GET_SET( CItem, resistHeat )
DECL_GET_SET( CItem, resistLight )
DECL_GET_SET( CItem, resistLightning )
DECL_GET_SET( CItem, resistPoison )
DECL_GET_SET( CItem, resistRain )
DECL_GET_SET( CItem, resistSnow )
DECL_GET_SET( CItem, restock )
DECL_GET_SET( CItem, scripttrigger )
DECL_GET_SET( CItem, scriptTriggers )
DECL_GET_SET( CItem, sectionalist )
DECL_GET_SET( CItem, sectionID )
DECL_GET_SET( CItem, secureContainers )
DECL_GET_SET( CItem, sellvalue )
DECL_GET_SET( CItem, serial )
DECL_GET_SET( CItem, shouldSave )
DECL_GET_SET( CItem, skin )
DECL_GET_SET( CItem, spawnsection )
DECL_GET_SET( CItem, spawnSerial )
DECL_GET_SET( CItem, speed )
DECL_GET_SET( CItem, stealable )
DECL_GET_SET( CItem, strength )
DECL_GET_SET( CItem, tempLastTraded )
DECL_GET_SET( CItem, tempTimer )
DECL_GET_SET( CItem, title )
DECL_GET_SET( CItem, totalItemCount )
DECL_GET_SET( CItem, tradeTimestamp )
DECL_GET_SET( CItem, trashContainers )
DECL_GET_SET( CItem, type )
DECL_GET_SET( CItem, usesLeft )
DECL_GET_SET( CItem, vendorPrice )
DECL_GET_SET( CItem, vendors )
DECL_GET_SET( CItem, visible )
DECL_GET_SET( CItem, weight )
DECL_GET_SET( CItem, weightMax )
DECL_GET_SET( CItem, wipable )
DECL_GET_SET( CItem, worldnumber )
DECL_GET_SET( CItem, x )
DECL_GET_SET( CItem, y )
DECL_GET_SET( CItem, z )

// Socket Properties
DECL_GET_SET( CSocket, account )
DECL_GET_SET( CSocket, bytesReceived )
DECL_GET_SET( CSocket, bytesSent )
DECL_GET_SET( CSocket, clickX )
DECL_GET_SET( CSocket, clickY )
DECL_GET_SET( CSocket, clickZ )
DECL_GET_SET( CSocket, clientLetterVer )
DECL_GET_SET( CSocket, clientMajorVer )
DECL_GET_SET( CSocket, clientMinorVer )
DECL_GET_SET( CSocket, clientSubVer )
DECL_GET_SET( CSocket, clientType )
DECL_GET_SET( CSocket, cryptClient )
DECL_GET_SET( CSocket, currentChar )
DECL_GET_SET( CSocket, currentSpellType )
DECL_GET_SET( CSocket, firstPacket )
DECL_GET_SET( CSocket, idleTimeout )
DECL_GET_SET( CSocket, language )
DECL_GET_SET( CSocket, logging )
DECL_GET_SET( CSocket, newClient )
DECL_GET_SET( CSocket, pickupSerial )
DECL_GET_SET( CSocket, pickupSpot )
DECL_GET_SET( CSocket, pickupX )
DECL_GET_SET( CSocket, pickupY )
DECL_GET_SET( CSocket, pickupZ )
DECL_GET_SET( CSocket, target )
DECL_GET_SET( CSocket, targetOK )
DECL_GET_SET( CSocket, tempInt )
DECL_GET_SET( CSocket, tempInt2 )
DECL_GET_SET( CSocket, tempObj )
DECL_GET_SET( CSocket, tempObj2 )
DECL_GET_SET( CSocket, walkSequence )
DECL_GET_SET( CSocket, wasIdleWarned )
DECL_GET( CSocket, addID )
DECL_GET( CSocket, buffer )
DECL_GET( CSocket, clientIP )
DECL_GET( CSocket, xText )
DECL_GET( CSocket, xText2 )

// Skills Properties
DECL_GET_SET( CSkills, alchemy )
DECL_GET_SET( CSkills, allskills )
DECL_GET_SET( CSkills, anatomy )
DECL_GET_SET( CSkills, animallore )
DECL_GET_SET( CSkills, archery )
DECL_GET_SET( CSkills, armslore )
DECL_GET_SET( CSkills, begging )
DECL_GET_SET( CSkills, blacksmithing )
DECL_GET_SET( CSkills, bowcraft )
DECL_GET_SET( CSkills, bushido )
DECL_GET_SET( CSkills, camping )
DECL_GET_SET( CSkills, carpentry )
DECL_GET_SET( CSkills, cartography )
DECL_GET_SET( CSkills, chivalry )
DECL_GET_SET( CSkills, cooking )
DECL_GET_SET( CSkills, detectinghidden )
DECL_GET_SET( CSkills, enticement )
DECL_GET_SET( CSkills, evaluatingintel )
DECL_GET_SET( CSkills, fencing )
DECL_GET_SET( CSkills, fishing )
DECL_GET_SET( CSkills, focus )
DECL_GET_SET( CSkills, forensics )
DECL_GET_SET( CSkills, healing )
DECL_GET_SET( CSkills, herding )
DECL_GET_SET( CSkills, hiding )
DECL_GET_SET( CSkills, imbuing )
DECL_GET_SET( CSkills, inscription )
DECL_GET_SET( CSkills, itemid )
DECL_GET_SET( CSkills, lockpicking )
DECL_GET_SET( CSkills, lumberjacking )
DECL_GET_SET( CSkills, macefighting )
DECL_GET_SET( CSkills, magery )
DECL_GET_SET( CSkills, magicresistance )
DECL_GET_SET( CSkills, meditation )
DECL_GET_SET( CSkills, mining )
DECL_GET_SET( CSkills, musicianship )
DECL_GET_SET( CSkills, mysticism )
DECL_GET_SET( CSkills, necromancy )
DECL_GET_SET( CSkills, ninjitsu )
DECL_GET_SET( CSkills, parrying )
DECL_GET_SET( CSkills, peacemaking )
DECL_GET_SET( CSkills, poisoning )
DECL_GET_SET( CSkills, provocation )
DECL_GET_SET( CSkills, removetrap )
DECL_GET_SET( CSkills, snooping )
DECL_GET_SET( CSkills, spellweaving )
DECL_GET_SET( CSkills, spiritspeak )
DECL_GET_SET( CSkills, stealing )
DECL_GET_SET( CSkills, stealth )
DECL_GET_SET( CSkills, swordsmanship )
DECL_GET_SET( CSkills, tactics )
DECL_GET_SET( CSkills, tailoring )
DECL_GET_SET( CSkills, taming )
DECL_GET_SET( CSkills, tasteid )
DECL_GET_SET( CSkills, throwing )
DECL_GET_SET( CSkills, tinkering )
DECL_GET_SET( CSkills, tracking )
DECL_GET_SET( CSkills, veterinary )
DECL_GET_SET( CSkills, wrestling )

// Gump Properties
DECL_GET_SET( CGumpData, buttons )
DECL_GET_SET( CGumpData, IDs )

// Account Properties
DECL_GET_SET( CAccount, comment )
DECL_GET_SET( CAccount, isBanned )
DECL_GET_SET( CAccount, isCounselor )
DECL_GET_SET( CAccount, isGM )
DECL_GET_SET( CAccount, isOnline )
DECL_GET_SET( CAccount, isPublic )
DECL_GET_SET( CAccount, isSeer )
DECL_GET_SET( CAccount, isSlot1Blocked )
DECL_GET_SET( CAccount, isSlot2Blocked )
DECL_GET_SET( CAccount, isSlot3Blocked )
DECL_GET_SET( CAccount, isSlot4Blocked )
DECL_GET_SET( CAccount, isSlot5Blocked )
DECL_GET_SET( CAccount, isSlot6Blocked )
DECL_GET_SET( CAccount, isSlot7Blocked )
DECL_GET_SET( CAccount, isSuspended )
DECL_GET_SET( CAccount, isYoung )
DECL_GET_SET( CAccount, password )
DECL_GET_SET( CAccount, path )
DECL_GET_SET( CAccount, timeban )
DECL_GET_SET( CAccount, totalPlayTime )
DECL_GET_SET( CAccount, unused10 )
DECL_GET( CAccount, character1 )
DECL_GET( CAccount, character2 )
DECL_GET( CAccount, character3 )
DECL_GET( CAccount, character4 )
DECL_GET( CAccount, character5 )
DECL_GET( CAccount, character6 )
DECL_GET( CAccount, character7 )
DECL_GET( CAccount, currentChar )
DECL_GET( CAccount, firstLogin )
DECL_GET( CAccount, flags )
DECL_GET( CAccount, id )
DECL_GET( CAccount, lastIP )
DECL_GET( CAccount, username )

// Console Properties
DECL_GET_SET( CConsole, logEcho )
DECL_GET_SET( CConsole, mode )

// Script Section Properties
DECL_GET( CScriptSection, atEnd )
DECL_GET( CScriptSection, atEndTags )
DECL_GET( CScriptSection, numTags )

// Resource Properties
DECL_GET_SET( CResource, fishAmount )
DECL_GET_SET( CResource, fishTime )
DECL_GET_SET( CResource, logAmount )
DECL_GET_SET( CResource, logTime )
DECL_GET_SET( CResource, oreAmount )
DECL_GET_SET( CResource, oreTime )

// Party Properties
DECL_GET_SET( CParty, isNPC )
DECL_GET_SET( CParty, leader )
DECL_GET( CParty, memberCount )

// Script Properties
DECL_GET_SET( CScript, script_id )

// Property table

// clang-format off
inline JSPropertySpec CSpellProperties[] =
{
  UX_PSG( CSpell, action,          JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, aggressiveSpell, JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, ash,             JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, baseDmg,         JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, circle,          JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, damageDelay,     JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, delay,           JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, drake,           JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, enabled,         JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, fieldSpell,      JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, garlic,          JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, ginseng,         JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, health,          JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, highSkill,       JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, id,              JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, lowSkill,        JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, mana,            JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, mantra,          JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, moss,            JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, name,            JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, pearl,           JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, recoveryDelay,   JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, reflectable,     JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, requireChar,     JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, requireItem,     JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, requireLocation, JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, requireTarget,   JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, resistable,      JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, scrollHigh,      JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, scrollLow,       JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, shade,           JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, silk,            JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, soundEffect,     JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, stamina,         JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, strToSay,        JSPROP_ENUMANDPERM ),
  UX_PSG( CSpell, travelSpell,     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CGlobalSkillProperties[] =
{
  UX_PSG( CGlobalSkill, dexterity,    JSPROP_ENUMANDPERM ),
  UX_PSG( CGlobalSkill, intelligence, JSPROP_ENUMANDPERM ),
  UX_PSG( CGlobalSkill, madeWord,     JSPROP_ENUMANDPERM ),
  UX_PSG( CGlobalSkill, name,         JSPROP_ENUMANDPERM ),
  UX_PSG( CGlobalSkill, scriptID,     JSPROP_ENUMANDPERM ),
  UX_PSG( CGlobalSkill, skillDelay,   JSPROP_ENUMANDPERM ),
  UX_PSG( CGlobalSkill, strength,     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CCreateEntryProperties[] =
{
  UX_PSG( CCreateEntry, addItem,      JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, avgMaxSkill,  JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, avgMinSkill,  JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, colour,       JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, delay,        JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, id,           JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, maxRank,      JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, minRank,      JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, name,         JSPROP_ENUMANDPERM ),
//  UX_PSG( CCreateEntry, resources,    JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG( CCreateEntry, skills,       JSPROP_ENUMANDPERM ), // Error: G
  UX_PSG( CCreateEntry, sound,        JSPROP_ENUMANDPERM ),
  UX_PSG( CCreateEntry, spell,        JSPROP_ENUMANDPERM ),
  JS_PS_END
};

// clang-format on

inline JSPropertySpec CTimerProperties[] =
{
  // Character Timers
  UX_PSG( CTimer, ANTISPAM,             JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, CRIMFLAG,             JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, EVADETIME,            JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, FIREFIELDTICK,        JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, FLYINGTOGGLE,         JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, HUNGER,               JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, IDLEANIMTIME,         JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, INVIS,                JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, LOGOUT,               JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, LOYALTYTIME,          JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, MOVETIME,             JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, MURDERRATE,           JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, PARAFIELDTICK,        JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, PEACETIMER,           JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, POISONFIELDTICK,      JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, POISONTEXT,           JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, POISONTIME,           JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, POISONWEAROFF,        JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SPATIMER,             JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SPELLRECOVERYTIME,    JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SPELLTIME,            JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SUMMONTIME,           JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, THIRST,               JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, TIMEOUT,              JSPROP_ENUMPERMRO ),

  // Socket Timers
  UX_PSG( CTimer, SOCK_FISHING,         JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_MUTETIME,        JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_OBJDELAY,        JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_SKILLDELAY,      JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_SPIRITSPEAK,     JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_TRACKING,        JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_TRACKINGDISPLAY, JSPROP_ENUMPERMRO ),
  UX_PSG( CTimer, SOCK_TRAFFICWARDEN,   JSPROP_ENUMPERMRO ),
  JS_PS_END
};


inline JSPropertySpec CRaceProperties[] =
{
  UX_PSGS( CRace, armourClass,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, genderRestrict,   JSPROP_ENUMANDPERM ),
//  UX_PSGS( CRace, id,               JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CRace, isPlayerRace,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, languageSkillMin, JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, magicResistance,  JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, name,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, nightVision,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, poisonResistance, JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, requiresBeard,    JSPROP_ENUMANDPERM ),
  UX_PSGS( CRace, requiresNoBeard,  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CRace, skillAdjustment,  JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CRace, visibleDistance,  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CRace, weakToWeather,    JSPROP_ENUMANDPERM ), // Error: GS
  JS_PS_END
};

// clang-format off
inline JSPropertySpec CRegionProperties[] =
{
  UX_PSG(  CRegion, members,            JSPROP_ENUMERATE   ), // Looking for array jsprop old one was JSPROP_ENUMPERMIDX
  UX_PSG(  CRegion, numOrePrefs,        JSPROP_ENUMPERMRO  ),
  UX_PSG(  CRegion, owner,              JSPROP_ENUMANDPERM ),
  UX_PSG(  CRegion, population,         JSPROP_ENUMPERMRO  ),
//  UX_PSGS( CRegion, appearance,         JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CRegion, canCastAggressive,  JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, canGate,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, canMark,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, canPlaceHouse,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, canRecall,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, canTeleport,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, chanceBigOre,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, health,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, id,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, instanceID,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, isDungeon,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, isGuarded,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, isSafeZone,         JSPROP_ENUMANDPERM ),
//  UX_PSGS( CRegion, mayor,              JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CRegion, music,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, name,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, numGuards,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, race,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, reserves,           JSPROP_ENUMANDPERM ),
//  UX_PSGS( CRegion, scriptTrigger,      JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CRegion, scriptTriggers,     JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CRegion, tax,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, taxes,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, taxResource,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, weather,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CRegion, worldNumber,        JSPROP_ENUMANDPERM ),
  JS_PS_END
};
// clang-format on

inline JSPropertySpec CSpawnRegionProperties[] =
{
  UX_PSGS( CSpawnRegion, call,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, defZ,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, instanceID,  JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, isSpawner,   JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSpawnRegion, item,        JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CSpawnRegion, itemCount,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CSpawnRegion, itemList,    JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CSpawnRegion, maxItems,    JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, maxNpcs,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, maxTime,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, minTime,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, name,        JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSpawnRegion, npc,         JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CSpawnRegion, npcCount,    JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CSpawnRegion, npcList,     JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CSpawnRegion, onlyOutside, JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, prefZ,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, regionNum,   JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, world,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, x1,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, x2,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, y1,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSpawnRegion, y2,          JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CGuildProperties[] =
{
//  UX_PSG(  CGuild, members,      JSPROP_ENUMERATE   ), // Error: G
  UX_PSG(  CGuild, numMembers,   JSPROP_ENUMPERMRO  ),
  UX_PSG(  CGuild, numRecruits,  JSPROP_ENUMPERMRO  ),
//  UX_PSG(  CGuild, recruits,     JSPROP_ENUMERATE   ), // Error: G
  UX_PSGS( CGuild, abbreviation, JSPROP_ENUMANDPERM ),
  UX_PSGS( CGuild, charter,      JSPROP_ENUMANDPERM ),
//  UX_PSGS( CGuild, master,       JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CGuild, name,         JSPROP_ENUMANDPERM ),
//  UX_PSGS( CGuild, stone,        JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CGuild, type,         JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CGuild, webPage,      JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CCharacterProps[] =
{
//  UX_PSG(  CCharacter, attack,                JSPROP_ENUMPERMRO  ), // Error: G
  UX_PSG(  CCharacter, dead,                  JSPROP_ENUMPERMRO  ),
  UX_PSG(  CCharacter, flag,                  JSPROP_ENUMPERMRO  ),
  UX_PSG(  CCharacter, followerCount,         JSPROP_ENUMPERMRO  ),
//  UX_PSG(  CCharacter, hungerRate,            JSPROP_ENUMPERMRO  ), // Error: G
//  UX_PSG(  CCharacter, isAnimal,              JSPROP_ENUMPERMRO  ), // Error: G
//  UX_PSG(  CCharacter, isChar,                JSPROP_ENUMPERMRO  ), // Error: G
//  UX_PSG(  CCharacter, isHuman,               JSPROP_ENUMPERMRO  ), // Error: G
//  UX_PSG(  CCharacter, isItem,                JSPROP_ENUMPERMRO  ), // Error: G
  UX_PSG(  CCharacter, isJailed,              JSPROP_ENUMPERMRO  ),
//  UX_PSG(  CCharacter, isSpawner,             JSPROP_ENUMPERMRO  ), // Error: G
  UX_PSG(  CCharacter, murderer,              JSPROP_ENUMPERMRO  ),
//  UX_PSG(  CCharacter, online,                JSPROP_ENUMPERMRO  ), // Error: G
  UX_PSG(  CCharacter, ownedItemsCount,       JSPROP_ENUMPERMRO  ),
//  UX_PSG(  CCharacter, party,                 JSPROP_ENUMPERMRO  ), // Error: G
  UX_PSG(  CCharacter, petCount,              JSPROP_ENUMPERMRO  ),
  UX_PSG(  CCharacter, spawnSerial,           JSPROP_ENUMPERMRO  ),
//  UX_PSG(  CCharacter, thirstRate,            JSPROP_ENUMPERMRO  ), // Error: G
//  UX_PSGS( CCharacter, account,               JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, accountNum,            JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, actualDexterity,       JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, actualIntelligence,    JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, actualStrength,        JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, aitype,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, allmove,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, attacker,              JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, atWar,                 JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, baseskills,            JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, beardColor,            JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, beardColour,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, beardStyle,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, brkPeaceChance,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, canAttack,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, canBroadcast,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, canRun,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, canSnoop,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, cell,                  JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, color,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, colour,                JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, commandlevel,          JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, controlSlots,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, controlSlotsUsed,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, createdOn,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, criminal,              JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, deaths,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, dexterity,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, direction,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, emoteColour,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, fame,                  JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, fleeAt,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, fontType,              JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, foodList,              JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, frozen,                JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, fx1,                   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, fx2,                   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, fy1,                   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, fy2,                   JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, fz,                    JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, gender,                JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, guarding,              JSPROP_ENUMANDPERM ) // Error: GS
//  UX_PSGS( CCharacter, guild,                 JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, guildTitle,            JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, hairColor,             JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, hairColour,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, hairStyle,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, hasStolen,             JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, health,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, hidamage,              JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, hireling,              JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, houseicons,            JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CCharacter, housesCoOwned,         JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, housesOwned,           JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, hue,                   JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, hunger,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, hungerWildChance,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, id,                    JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, innocent,              JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, instanceID,            JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, intelligence,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, isAwake,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, isCasting,             JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, isCounselor,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, isDisguised,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, isDispellable,         JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, isFlying,              JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, isGM,                  JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, isGMPageable,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, isGuarded,             JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, isIncognito,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, isMeditating,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, isonhorse,             JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, isPolymorphed,         JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, isRunning,             JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, isShop,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, isUsingPotion,         JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, karma,                 JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, lightlevel,            JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, lodamage,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, loyalty,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, loyaltyRate,           JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, magicReflect,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, mana,                  JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, maxhp,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, maxLoyalty,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, maxmana,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, maxstamina,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, mounted,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, multi,                 JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, murdercount,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, name,                  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, neutral,               JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, nextAct,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, noNeedMana,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, noNeedReags,           JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, noSkillTitles,         JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, npc,                   JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, npCCharacter,          JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, npcFlag,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, oldWandertype,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, oldX,                  JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, oldY,                  JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, oldZ,                  JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, orgID,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, orgSkin,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, origin,                JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, origName,              JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, orneriness,            JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, owner,                 JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, ownerCount,            JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, pack,                  JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, partyLootable,         JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, permanentMagicReflect, JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, playTime,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, poison,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, poisonStrength,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, priv,                  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, race,                  JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, raceGate,              JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, reAttackAt,            JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, region,                JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, sayColour,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, scripttrigger,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, scriptTriggers,        JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, sectionID,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, serial,                JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, setPeace,              JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, shouldSave,            JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, singClickSer,          JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, skillLock,             JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, skills,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, skillsused,            JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, skillToPeace,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, skillToProv,           JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, skillToTame,           JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, skin,                  JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, socket,                JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CCharacter, spattack,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, spdelay,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, spellCast,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, split,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, splitchance,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, squelch,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, stabled,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, stamina,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, stealth,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, strength,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, tamed,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, tamedHungerRate,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, tamedThirstRate,       JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, target,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, tempdex,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, tempint,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, tempstr,               JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, thirst,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, thirstWildChance,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, title,                 JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, town,                  JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, townPriv,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, trainer,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, visible,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, vulnerable,            JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, wandertype,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, weight,                JSPROP_ENUMANDPERM ),
//  UX_PSGS( CCharacter, willhunger,            JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CCharacter, willthirst,            JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSGS( CCharacter, worldnumber,           JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CCharacter, x,                     JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, y,                     JSPROP_ENUMANDPERM ),
  UX_PSGS( CCharacter, z,                     JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CItemProps[] =
{
  UX_PSGS( CItem, ac,                  JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, ammoFX,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, ammoFXHue,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, ammoFXRender,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, ammoHue,             JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, ammoID,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, amount,              JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, att,                 JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, baseRange,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, baseWeight,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, buyvalue,            JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, canBeLockedDown,     JSPROP_ENUMPERMRO  ), // Error: S
  UX_PSGS( CItem, carveSection,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, color,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, colour,              JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, container,           JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, corpse,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, creator,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, damageCold,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, damageHeat,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, damageLight,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, damageLightning,     JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, damagePoison,        JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, damageRain,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, damageSnow,          JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, decayable,           JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, decaytime,           JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, def,                 JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, desc,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, dexterity,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, dir,                 JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, divinelock,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, entryMadeFrom,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, event,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, good,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, health,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, hidamage,            JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, hue,                 JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, id,                  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, instanceID,          JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, intelligence,        JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, isChar,              JSPROP_ENUMPERMRO  ), // Error: GS
//  UX_PSGS( CItem, isContType,          JSPROP_ENUMPERMRO  ), // Error: S
  UX_PSGS( CItem, isDamageable,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, isDispellable,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, isDoorOpen,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, isDyeable,           JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, isFieldSpell,        JSPROP_ENUMPERMRO  ), // Error: S
  UX_PSGS( CItem, isGuarded,           JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, isItem,              JSPROP_ENUMPERMRO  ), // Error: GS
  UX_PSGS( CItem, isItemHeld,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, isLeatherType,       JSPROP_ENUMPERMRO  ), // Error: S
//  UX_PSGS( CItem, isLockedDown,        JSPROP_ENUMPERMRO  ), // Error: S
  UX_PSGS( CItem, isMarkedByMaker,     JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, isMetalType,         JSPROP_ENUMPERMRO  ), // Error: S
  UX_PSGS( CItem, isNewbie,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, isPileable,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, isShieldType,        JSPROP_ENUMPERMRO  ), // Error: S
//  UX_PSGS( CItem, isSpawner,           JSPROP_ENUMPERMRO  ), // Error: G
  UX_PSGS( CItem, isWipeable,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, itemsinside,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, layer,               JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, lodamage,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, madeWith,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, maxhp,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, maxItems,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, maxRange,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, maxUses,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, more,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, more0,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, more1,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, more2,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, morex,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, morey,               JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, morez,               JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, movable,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, multi,               JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, name,                JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, name2,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, oldX,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, oldY,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, oldZ,                JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, origin,              JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, owner,               JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, poison,              JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, race,                JSPROP_ENUMANDPERM ), // Error: G
  UX_PSGS( CItem, rank,                JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, region,              JSPROP_ENUMPERMRO  ), // Error: G
//  UX_PSGS( CItem, resistCold,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, resistHeat,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, resistLight,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, resistLightning,     JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, resistPoison,        JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, resistRain,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, resistSnow,          JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, restock,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, scripttrigger,       JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, scriptTriggers,      JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, sectionID,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, sellvalue,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, serial,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, shouldSave,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, skin,                JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CItem, spawnSerial,         JSPROP_ENUMPERMRO  ),
  UX_PSGS( CItem, speed,               JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, stealable,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, strength,            JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, tempLastTraded,      JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CItem, tempTimer,           JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, title,               JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, totalItemCount,      JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, type,                JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, usesLeft,            JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, vendorPrice,         JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, visible,             JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, weight,              JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, weightMax,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, wipable,             JSPROP_ENUMANDPERM ),
//  UX_PSGS( CItem, worldnumber,         JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CItem, x,                   JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, y,                   JSPROP_ENUMANDPERM ),
  UX_PSGS( CItem, z,                   JSPROP_ENUMANDPERM ),

  // The Following vars are specific to CSpawnItem objects
//  UX_PSGS( CItem, maxinterval,         JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, mininterval,         JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, sectionalist,        JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, spawnsection,        JSPROP_ENUMANDPERM ), // Error: GS

  // The Following vars are specific to cMultiObj objects
//  UX_PSGS( CItem, bans,                JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, banX,                JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, banY,                JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, buildTimestamp,      JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, deed,                JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, friends,             JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, guests,              JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, isPublic,            JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, lockdowns,           JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxBans,             JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxFriends,          JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxGuests,           JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxLockdowns,        JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxOwners,           JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxSecureContainers, JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxTrashContainers,  JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, maxVendors,          JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, owners,              JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, secureContainers,    JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, tradeTimestamp,      JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, trashContainers,     JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CItem, vendors,             JSPROP_ENUMANDPERM ), // Error: GS
  JS_PS_END
};

inline JSPropertySpec CSocketProps[] =
{
  UX_PSG(  CSocket, addID,            JSPROP_ENUMPERMIDX ),
//  UX_PSG(  CSocket, buffer,           JSPROP_ENUMPERMIDX ), // Error: G
//  UX_PSG(  CSocket, clientIP,         JSPROP_ENUMPERMIDX ), // Error: G
  UX_PSG(  CSocket, xText,            JSPROP_ENUMPERMIDX ),
  UX_PSG(  CSocket, xText2,           JSPROP_ENUMPERMIDX ),
//  UX_PSGS( CSocket, account,          JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CSocket, bytesReceived,    JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CSocket, bytesSent,        JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CSocket, clickX,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, clickY,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, clickZ,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, clientLetterVer,  JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, clientMajorVer,   JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, clientMinorVer,   JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, clientSubVer,     JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSocket, clientType,       JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CSocket, cryptClient,      JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSocket, currentChar,      JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CSocket, currentSpellType, JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, firstPacket,      JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSocket, idleTimeout,      JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CSocket, language,         JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CSocket, logging,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, newClient,        JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSocket, pickupSerial,     JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CSocket, pickupSpot,       JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CSocket, pickupX,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, pickupY,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, pickupZ,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSocket, target,           JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CSocket, targetOK,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, tempInt,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, tempInt2,         JSPROP_ENUMANDPERM ),
//  UX_PSGS( CSocket, tempObj,          JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CSocket, tempObj2,         JSPROP_ENUMANDPERM ), // Error: GS
  UX_PSGS( CSocket, walkSequence,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSocket, wasIdleWarned,    JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CSkillsProps[] =
{
  UX_PSGS( CSkills, alchemy,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, allskills,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, anatomy,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, animallore,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, archery,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, armslore,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, begging,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, blacksmithing,   JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, bowcraft,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, bushido,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, camping,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, carpentry,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, cartography,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, chivalry,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, cooking,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, detectinghidden, JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, enticement,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, evaluatingintel, JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, fencing,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, fishing,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, focus,           JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, forensics,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, healing,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, herding,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, hiding,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, imbuing,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, inscription,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, itemid,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, lockpicking,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, lumberjacking,   JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, macefighting,    JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, magery,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, magicresistance, JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, meditation,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, mining,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, musicianship,    JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, mysticism,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, necromancy,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, ninjitsu,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, parrying,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, peacemaking,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, poisoning,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, provocation,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, removetrap,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, snooping,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, spellweaving,    JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, spiritspeak,     JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, stealing,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, stealth,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, swordsmanship,   JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, tactics,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, tailoring,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, taming,          JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, tasteid,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, throwing,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, tinkering,       JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, tracking,        JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, veterinary,      JSPROP_ENUMANDPERM ),
  UX_PSGS( CSkills, wrestling,       JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CGumpDataProperties[] =
{
//  UX_PSGS( CGumpData, buttons,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CGumpData, IDs,       JSPROP_ENUMANDPERM ), // Error: S
  JS_PS_END
};

inline JSPropertySpec CAccountProperties[] =
{
//  UX_PSG(  CAccount, character1,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, character2,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, character3,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, character4,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, character5,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, character6,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, character7,       JSPROP_ENUMANDPERM ), // Error: G
//  UX_PSG(  CAccount, currentChar,      JSPROP_ENUMANDPERM ), // Error: G
  UX_PSG(  CAccount, firstLogin,       JSPROP_ENUMANDPERM ),
  UX_PSG(  CAccount, flags,            JSPROP_ENUMANDPERM ),
  UX_PSG(  CAccount, id,               JSPROP_ENUMANDPERM ),
//  UX_PSG(  CAccount, lastIP,           JSPROP_ENUMANDPERM ), // Error: G
  UX_PSG(  CAccount, username,         JSPROP_ENUMANDPERM ),
  UX_PSGS( CAccount, comment,          JSPROP_ENUMANDPERM ),
//  UX_PSGS( CAccount, password,         JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CAccount, path,             JSPROP_ENUMANDPERM ), // Error: GS
//  UX_PSGS( CAccount, timeban,          JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CAccount, totalPlayTime,    JSPROP_ENUMANDPERM ),

  // Flags
//  UX_PSGS( CAccount, isBanned,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isCounselor,      JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isGM,             JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isOnline,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isPublic,         JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSeer,           JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot1Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot2Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot3Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot4Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot5Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot6Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSlot7Blocked,   JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isSuspended,      JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, isYoung,          JSPROP_ENUMANDPERM ), // Error: S
//  UX_PSGS( CAccount, unused10,         JSPROP_ENUMANDPERM ), // Error: S
  JS_PS_END
};

inline JSPropertySpec CConsoleProperties[] =
{
  UX_PSGS( CConsole, logEcho,    JSPROP_ENUMANDPERM ),
  UX_PSGS( CConsole, mode,       JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CScriptSectionProperties[] =
{
  UX_PSG( CScriptSection, atEnd,        JSPROP_ENUMANDPERM ),
  UX_PSG( CScriptSection, atEndTags,    JSPROP_ENUMANDPERM ),
  UX_PSG( CScriptSection, numTags,      JSPROP_ENUMANDPERM ),
  JS_PS_END
};

// clang-format off
inline JSPropertySpec CResourceProperties[] =
{
  UX_PSGS( CResource, fishAmount, JSPROP_ENUMANDPERM ),
//  UX_PSGS( CResource, fishTime,   JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CResource, logAmount,  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CResource, logTime,    JSPROP_ENUMANDPERM ), // Error: S
  UX_PSGS( CResource, oreAmount,  JSPROP_ENUMANDPERM ),
//  UX_PSGS( CResource, oreTime,    JSPROP_ENUMANDPERM ), // Error: S
  JS_PS_END
};
// clang-format on

inline JSPropertySpec CPartyProperties[] =
{
  UX_PSG(  CParty, memberCount, JSPROP_ENUMPERMRO ),
  UX_PSGS( CParty, isNPC,       JSPROP_ENUMANDPERM ),
// UX_PSGS( CParty, leader,      JSPROP_ENUMANDPERM ), // Error: GS
  JS_PS_END
};

inline JSPropertySpec CScriptProperties[] =
{
//  UX_PSGS( CScript, script_id,       JSPROP_ENUMANDPERM ), // Error: GS
  JS_PS_END
};

#endif
