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

#define DECL_GET( main, attr ) JSNative JS##main ## _get_ ## attr;
#define DECL_SET( main, attr ) JSNative JS##main ## _set_ ## attr;
#define IMPL_GET( main, attr, type, method, accessor ) \
bool JS##main##_get_##attr(JSContext *cx, unsigned int argc, JS::Value *vp) { \
  auto args = JS::CallArgsFromVp(argc, vp); \
  JS::RootedObject thisObj(cx); \
  auto priv = JS::GetMaybePtrFromReservedSlot< type >(thisObj, 0); \
  args.rval().method(priv->accessor()); \
  return true; \
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

// Property table

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
  JS_PSGS( "name",             JSCGlobalSkill_get_name,         JSCGlobalSkill_set_name,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "madeWord",		   JSCGlobalSkill_get_madeWord,	    JSCGlobalSkill_set_madeWord,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "strength",		   JSCGlobalSkill_get_strength,	    JSCGlobalSkill_set_strength,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "dexterity",		   JSCGlobalSkill_get_dexterity,    JSCGlobalSkill_set_dexterity,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "intelligence",	   JSCGlobalSkill_get_intelligence, JSCGlobalSkill_set_intelligence, JSPROP_ENUMANDPERM ),
  JS_PSGS( "skillDelay",	   JSCGlobalSkill_get_skillDelay,	JSCGlobalSkill_set_skillDelay,	 JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptID",		   JSCGlobalSkill_get_scriptID,	    JSCGlobalSkill_set_scriptID,	 JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CCreateEntryProperties[] =
{
  JS_PSGS( "id",             JSCCreateEntry_get_id,             JSCCreateEntry_set_id,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "name",		     JSCCreateEntry_get_name,	        JSCCreateEntry_set_name,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "addItem",		 JSCCreateEntry_get_addItem,	    JSCCreateEntry_set_addItem,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "colour",		 JSCCreateEntry_get_colour,         JSCCreateEntry_set_colour,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "sound",	         JSCCreateEntry_get_sound,          JSCCreateEntry_set_sound,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "minRank",	     JSCCreateEntry_get_minRank,	    JSCCreateEntry_set_minRank,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "maxRank",		 JSCCreateEntry_get_maxRank,	    JSCCreateEntry_set_maxRank,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "delay",		     JSCCreateEntry_get_delay,	        JSCCreateEntry_set_delay,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "spell",		     JSCCreateEntry_get_spell,	        JSCCreateEntry_set_spell,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "resources",		 JSCCreateEntry_get_resources,	    JSCCreateEntry_set_resources,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "skills",		 JSCCreateEntry_get_skills,	        JSCCreateEntry_set_skills,	     JSPROP_ENUMANDPERM ),
  JS_PSGS( "avgMinSkill",    JSCCreateEntry_get_avgMinSkill,	JSCCreateEntry_set_avgMinSkill,	 JSPROP_ENUMANDPERM ),
  JS_PSGS( "avgMaxSkill",	 JSCCreateEntry_get_avgMaxSkill,	JSCCreateEntry_set_avgMaxSkill,	 JSPROP_ENUMANDPERM ),
  JS_PS_END
};

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

inline JSPropertySpec CRegionProperties[] =
{
  JS_PSGS( "name",				JSCRegion_get_name,                JSCRegion_set_name,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "mayor",				JSCRegion_get_mayor,	           JSCRegion_set_mayor,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "race",		        JSCRegion_get_race,				   JSCRegion_set_race,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "tax",		        JSCRegion_get_tax,				   JSCRegion_set_tax,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "taxResource",	    JSCRegion_get_taxResource,		   JSCRegion_set_taxResource,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "canMark",	        JSCRegion_get_canMark,			   JSCRegion_set_canMark,	  JSPROP_ENUMANDPERM ),
  JS_PSGS( "canRecall",			JSCRegion_get_canRecall,		   JSCRegion_set_canRecall,	  JSPROP_ENUMANDPERM ),
  JS_PSGS( "canGate",			JSCRegion_get_canGate,			   JSCRegion_set_canGate,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "canTeleport",		JSCRegion_get_canTeleport,         JSCRegion_set_canTeleport,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "canPlaceHouse",	    JSCRegion_get_canPlaceHouse,	   JSCRegion_set_canPlaceHouse,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGuarded",			JSCRegion_get_isGuarded,		   JSCRegion_set_isGuarded,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "canCastAggressive", JSCRegion_get_canCastAggressive,   JSCRegion_set_canCastAggressive,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSafeZone",		JSCRegion_get_isSafeZone,		   JSCRegion_set_isSafeZone,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "health",			JSCRegion_get_health,			   JSCRegion_set_health,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDungeon",			JSCRegion_get_isDungeon,		   JSCRegion_set_isDungeon,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "worldNumber",	    JSCRegion_get_worldNumber,		   JSCRegion_set_worldNumber,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "instanceID",	    JSCRegion_get_instanceID,		   JSCRegion_set_instanceID,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "chanceBigOre",	    JSCRegion_get_chanceBigOre,		   JSCRegion_set_chanceBigOre,  JSPROP_ENUMANDPERM ),
  JS_PSG( "numOrePrefs",		JSCRegion_get_numOrePrefs,	       JSPROP_ENUMPERMRO ),
  JS_PSG( "population",			JSCRegion_get_population,		   JSPROP_ENUMPERMRO ),
  JS_PSG( "members",			JSCRegion_get_members,			   JSPROP_ENUMERATE ), // Looking for array jsprop old one was JSPROP_ENUMPERMIDX
  JS_PSGS( "id",				JSCRegion_get_id,				   JSCRegion_set_id,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTrigger",	    JSCRegion_get_scriptTrigger,	   JSCRegion_set_scriptTrigger,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTriggers",	JSCRegion_get_scriptTriggers,	   JSCRegion_set_scriptTriggers,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "numGuards",			JSCRegion_get_numGuards,	       JSCRegion_set_numGuards,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "taxes",				JSCRegion_get_taxes,			   JSCRegion_set_taxes,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "reserves",			JSCRegion_get_reserves,			   JSCRegion_set_reserves,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "appearance",	    JSCRegion_get_appearance,		   JSCRegion_set_appearance,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "music",				JSCRegion_get_music,			   JSCRegion_set_music,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "weather",			JSCRegion_get_weather,			   JSCRegion_set_weather,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "owner",				JSCRegion_get_owner,			   JSCRegion_set_owner,  JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CSpawnRegionProperties[] =
{
  JS_PSGS( "name",				JSCSpawnRegion_get_name,	   JSCSpawnRegion_set_name,					 JSPROP_ENUMANDPERM ),
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
  { "sectionID",		CIP_SECTIONID,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "name",			CIP_NAME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "title",			CIP_TITLE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "x",				CIP_X,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "y",				CIP_Y,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "z",				CIP_Z,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldX",			CIP_OLDX,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldY",			CIP_OLDY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldZ",			CIP_OLDZ,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "id",				CIP_ID,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "colour",			CIP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "color",			CIP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skin",			CIP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hue",			CIP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "owner",			CIP_OWNER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "visible",		CIP_VISIBLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "serial",			CIP_SERIAL,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "health",			CIP_HEALTH,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "scripttrigger",	CIP_SCRIPTTRIGGER,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "scriptTriggers",	CIP_SCRIPTTRIGGERS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "worldnumber",	CIP_WORLDNUMBER,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "instanceID",		CIP_INSTANCEID,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "amount",			CIP_AMOUNT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "container",		CIP_CONTAINER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "type",			CIP_TYPE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "more",			CIP_MORE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "more0",			CIP_MORE0,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "more1",			CIP_MORE1,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "more2",			CIP_MORE2,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "morex",			CIP_MOREX,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "morey",			CIP_MOREY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "morez",			CIP_MOREZ,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "movable",		CIP_MOVABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "att",			CIP_ATT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "layer",			CIP_LAYER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "itemsinside",	CIP_ITEMSINSIDE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "totalItemCount",	CIP_TOTALITEMCOUNT,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "decayable",		CIP_DECAYABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "decaytime",		CIP_DECAYTIME,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "lodamage",		CIP_LODAMAGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hidamage",		CIP_HIDAMAGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ac",				CIP_AC,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "def",			CIP_DEF,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistCold",		CIP_RESISTCOLD,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistHeat",		CIP_RESISTHEAT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistLight",	CIP_RESISTLIGHT,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistLightning",CIP_RESISTLIGHTNING,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistPoison",	CIP_RESISTPOISON,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistRain",		CIP_RESISTRAIN,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "resistSnow",		CIP_RESISTSNOW,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damageHeat",		CIP_DAMAGEHEAT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damageCold",		CIP_DAMAGECOLD,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damageLight",	CIP_DAMAGELIGHT,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damageLightning",CIP_DAMAGELIGHTNING,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damagePoison",	CIP_DAMAGEPOISON,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damageRain",		CIP_DAMAGERAIN,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "damageSnow",		CIP_DAMAGESNOW,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "name2",			CIP_NAME2,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isChar",			CIP_ISCHAR,			JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isItem",			CIP_ISITEM,			JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isSpawner",		CIP_ISSPAWNER,		JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "race",			CIP_RACE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxhp",			CIP_MAXHP,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxUses",		CIP_MAXUSES,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "usesLeft",		CIP_USESLEFT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "rank",			CIP_RANK,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "creator",		CIP_CREATOR,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "poison",			CIP_POISON,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "dir",			CIP_DIR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "wipable",		CIP_WIPABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "buyvalue",		CIP_BUYVALUE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "sellvalue",		CIP_SELLVALUE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "vendorPrice",	CIP_VENDORPRICE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "restock",		CIP_RESTOCK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "divinelock",		CIP_DIVINELOCK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "baseWeight",		CIP_BASEWEIGHT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "weight",			CIP_WEIGHT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "weightMax",		CIP_WEIGHTMAX,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxItems",		CIP_MAXITEMS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "strength",		CIP_STRENGTH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "dexterity",		CIP_DEXTERITY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "intelligence",	CIP_INTELLIGENCE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "corpse",			CIP_CORPSE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "desc",			CIP_DESC,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "event",			CIP_EVENT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempLastTraded",	CIP_TEMPLASTTRADED,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempTimer",		CIP_TEMPTIMER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "shouldSave",		CIP_SHOULDSAVE,		JSPROP_ENUMANDPERM, nullptr, nullptr },

  { "isNewbie",		CIP_ISNEWBIE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isDispellable",	CIP_ISDISPELLABLE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "madeWith",		CIP_MADEWITH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "entryMadeFrom",	CIP_ENTRYMADEFROM,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isPileable",		CIP_ISPILEABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isMarkedByMaker",CIP_ISMARKEDBYMAKER,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isDyeable",		CIP_ISDYEABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isDamageable",	CIP_ISDAMAGEABLE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isWipeable",		CIP_ISWIPEABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isGuarded",		CIP_ISGUARDED,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isDoorOpen",		CIP_ISDOOROPEN,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isFieldSpell",	CIP_ISFIELDSPELL,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isLockedDown",	CIP_ISLOCKEDDOWN,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isShieldType",	CIP_ISSHIELDTYPE,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isMetalType",	CIP_ISMETALTYPE,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isLeatherType",	CIP_ISLEATHERTYPE,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "canBeLockedDown",CIP_CANBELOCKEDDOWN,JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isContType",		CIP_ISCONTTYPE,		JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "carveSection",	CIP_CARVESECTION,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ammoID",			CIP_AMMOID,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ammoHue",		CIP_AMMOHUE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ammoFX",			CIP_AMMOFX,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ammoFXHue",		CIP_AMMOFXHUE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ammoFXRender",	CIP_AMMOFXRENDER,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "speed",			CIP_SPEED,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "multi",			CIP_MULTI,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxRange",		CIP_MAXRANGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "baseRange",		CIP_BASERANGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "region",			CIP_REGION,			JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "spawnSerial",	CIP_SPAWNSERIAL,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "origin",			CIP_ORIGIN,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isItemHeld",		CIP_ISITEMHELD,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stealable",		CIP_STEALABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },

  // The Following vars are specific to CSpawnItem objects
  { "spawnsection",	CIP_SPAWNSECTION,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "sectionalist",	CIP_SECTIONALIST,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "mininterval",	CIP_MININTERVAL,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxinterval",	CIP_MAXINTERVAL,	JSPROP_ENUMANDPERM, nullptr, nullptr },

  // The Following vars are specific to cMultiObj objects
  { "lockdowns",		CIP_LOCKDDOWNS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxLockdowns",	CIP_MAXLOCKDOWNS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "trashContainers",	CIP_TRASHCONTAINERS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxTrashContainers",	CIP_MAXTRASHCONTAINERS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "secureContainers",	CIP_SECURECONTAINERS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxSecureContainers",CIP_MAXSECURECONTAINERS,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "friends",		CIP_FRIENDS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxFriends",		CIP_MAXFRIENDS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "guests",			CIP_GUESTS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxGuests",		CIP_MAXGUESTS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "owners",			CIP_OWNERS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxOwners",		CIP_MAXOWNERS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "bans",			CIP_BANS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxBans",		CIP_MAXBANS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "vendors",		CIP_VENDORS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxVendors",		CIP_MAXVENDORS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "deed",			CIP_DEED,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isPublic",		CIP_ISPUBLIC,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "buildTimestamp",	CIP_BUILDTIMESTAMP,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tradeTimestamp",	CIP_TRADETIMESTAMP,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "banX",			CIP_BANX,			JSPROP_ENUMANDPERM, nullptr, nullptr},
  { "banY",			CIP_BANY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CSocketProps[] =
{
  { "account",			CSOCKP_ACCOUNT,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "currentChar",		CSOCKP_CURRENTCHAR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "idleTimeout",		CSOCKP_IDLETIMEOUT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "wasIdleWarned",		CSOCKP_WASIDLEWARNED,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempInt",			CSOCKP_TEMPINT,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempInt2",			CSOCKP_TEMPINT2,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "buffer",				CSOCKP_BUFFER,				JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "xText",				CSOCKP_XTEXT,				JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "xText2",				CSOCKP_XTEXT2,				JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "clickZ",				CSOCKP_CLICKZ,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "addID",				CSOCKP_ADDID,				JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "newClient",			CSOCKP_NEWCLIENT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "firstPacket",		CSOCKP_FIRSTPACKET,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "cryptClient",		CSOCKP_CRYPTCLIENT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clientIP",			CSOCKP_CLIENTIP,			JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "walkSequence",		CSOCKP_WALKSEQUENCE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "currentSpellType",	CSOCKP_CURRENTSPELLTYPE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "logging",			CSOCKP_LOGGING,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "bytesSent",			CSOCKP_BYTESSENT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "bytesReceived",		CSOCKP_BYTESRECEIVED,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "targetOK",			CSOCKP_TARGETOK,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clickX",				CSOCKP_CLICKX,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clickY",				CSOCKP_CLICKY,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "pickupX",			CSOCKP_PICKUPX,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "pickupY",			CSOCKP_PICKUPY,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "pickupZ",			CSOCKP_PICKUPZ,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "pickupSpot",			CSOCKP_PICKUPSPOT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "pickupSerial",		CSOCKP_PICKUPSERIAL,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "language",			CSOCKP_LANGUAGE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clientMajorVer",		CSOCKP_CLIENTMAJORVER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clientMinorVer",		CSOCKP_CLIENTMINORVER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clientSubVer",		CSOCKP_CLIENTSUBVER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clientLetterVer",	CSOCKP_CLIENTLETTERVER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "clientType",			CSOCKP_CLIENTTYPE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "target",				CSOCKP_TARGET,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempObj",			CSOCKP_TEMPOBJ,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempObj2",			CSOCKP_TEMPOBJ2,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CSkillsProps[] =
{
  { "alchemy",			ALCHEMY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "anatomy",			ANATOMY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "animallore",			ANIMALLORE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "itemid",				ITEMID,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "armslore",			ARMSLORE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "parrying",			PARRYING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "begging",			BEGGING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "blacksmithing",		BLACKSMITHING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "bowcraft",			BOWCRAFT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "peacemaking",		PEACEMAKING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "camping",			CAMPING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "carpentry",			CARPENTRY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "cartography",		CARTOGRAPHY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "cooking",			COOKING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "detectinghidden",	DETECTINGHIDDEN,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "enticement",			ENTICEMENT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "evaluatingintel",	EVALUATINGINTEL,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "healing",			HEALING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fishing",			FISHING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "forensics",			FORENSICS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "herding",			HERDING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hiding",				HIDING,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "provocation",		PROVOCATION,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "inscription",		INSCRIPTION,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "lockpicking",		LOCKPICKING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "magery",				MAGERY,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "magicresistance",	MAGICRESISTANCE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tactics",			TACTICS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "snooping",			SNOOPING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "musicianship",		MUSICIANSHIP,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "poisoning",			POISONING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "archery",			ARCHERY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "spiritspeak",		SPIRITSPEAK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stealing",			STEALING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tailoring",			TAILORING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "taming",				TAMING,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tasteid",			TASTEID,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tinkering",			TINKERING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tracking",			TRACKING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "veterinary",			VETERINARY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "swordsmanship",		SWORDSMANSHIP,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "macefighting",		MACEFIGHTING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fencing",			FENCING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "wrestling",			WRESTLING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "lumberjacking",		LUMBERJACKING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "mining",				MINING,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "meditation",			MEDITATION,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stealth",			STEALTH,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "removetrap",			REMOVETRAP,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "necromancy",			NECROMANCY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "focus",				FOCUS,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "chivalry",			CHIVALRY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "bushido",			BUSHIDO,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ninjitsu",			NINJITSU,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "spellweaving",		SPELLWEAVING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "imbuing",			IMBUING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "mysticism",			MYSTICISM,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "throwing",			THROWING,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "allskills",			ALLSKILLS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CGumpDataProperties[] =
{
  {	"buttons",	CGumpData_Button,		JSPROP_ENUMANDPERM, nullptr, nullptr	},
  {	"IDs",		CGumpData_ID ,			JSPROP_ENUMANDPERM, nullptr, nullptr	},
  JS_PS_END
};

inline JSPropertySpec CAccountProperties[] =
{
  { "id",			CACCOUNT_ID,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "username",	CACCOUNT_USERNAME,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "password",	CACCOUNT_PASSWORD,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "flags",		CACCOUNT_FLAGS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "path",		CACCOUNT_PATH,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "comment",	CACCOUNT_COMMENT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "timeban",	CACCOUNT_TIMEBAN,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "firstLogin",	CACCOUNT_FIRSTLOGIN, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "totalPlayTime",	CACCOUNT_TOTALPLAYTIME,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character1",	CACCOUNT_CHARACTER1, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character2",	CACCOUNT_CHARACTER2, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character3",	CACCOUNT_CHARACTER3, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character4",	CACCOUNT_CHARACTER4, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character5",	CACCOUNT_CHARACTER5, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character6",	CACCOUNT_CHARACTER6, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "character7",	CACCOUNT_CHARACTER7, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "currentChar",	CACCOUNT_CURRENTCHAR,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "lastIP",			CACCOUNT_LASTIP, 		JSPROP_ENUMANDPERM, nullptr, nullptr },

  // Flags
  { "isBanned",		CACCOUNT_BANNED,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSuspended",	CACCOUNT_SUSPENDED,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isPublic",		CACCOUNT_PUBLIC,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isOnline",		CACCOUNT_ONLINE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot1Blocked",	CACCOUNT_CHARSLOT1BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot2Blocked",	CACCOUNT_CHARSLOT2BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot3Blocked",	CACCOUNT_CHARSLOT3BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot4Blocked",	CACCOUNT_CHARSLOT4BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot5Blocked",	CACCOUNT_CHARSLOT5BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot6Blocked",	CACCOUNT_CHARSLOT6BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSlot7Blocked",	CACCOUNT_CHARSLOT7BLOCKED, 	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isYoung",		CACCOUNT_YOUNG,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "unused10",		CACCOUNT_UNUSED10,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSeer",			CACCOUNT_SEER,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isCounselor",	CACCOUNT_COUNSELOR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isGM",			CACCOUNT_GM,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CConsoleProperties[] =
{
  { "mode",		CCONSOLE_MODE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "logEcho",	CCONSOLE_LOGECHO,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CScriptSectionProperties[] =
{
  { "numTags",	CSS_NUMTAGS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "atEnd",		CSS_ATEND,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "atEndTags",	CSS_ATENDTAGS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CResourceProperties[] =
{
  { "logAmount",	CRESP_LOGAMT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "logTime",	CRESP_LOGTIME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oreAmount",	CRESP_OREAMT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oreTime",	CRESP_ORETIME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fishAmount",	CRESP_FISHAMT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fishTime",	CRESP_FISHTIME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CPartyProperties[] =
{
  { "leader",			CPARTYP_LEADER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "memberCount",	CPARTYP_MEMBERCOUNT,	JSPROP_ENUMPERMRO, nullptr, nullptr	},
  { "isNPC",			CPARTYP_ISNPC,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CScriptProperties[] =
{
  { "script_id",		CSCRIPT_SCRIPTID,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

#endif
