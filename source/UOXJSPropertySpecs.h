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

#define DECL_GET_SET( main, attr ) \
DECL_GET( main, attr ) \
DECL_SET( main, attr )

// Forward declares
DECL_GET_SET( CSpell, id )
DECL_GET_SET( CSpell, action )
DECL_GET_SET( CSpell, delay )
DECL_GET_SET( CSpell, damageDelay )
DECL_GET_SET( CSpell, recoveryDelay )
DECL_GET_SET( CSpell, health )
DECL_GET_SET( CSpell, stamina )
DECL_GET_SET( CSpell, mana )
DECL_GET_SET( CSpell, mantra )
DECL_GET_SET( CSpell, name )
DECL_GET_SET( CSpell, strToSay )
DECL_GET_SET( CSpell, scrollLow )
DECL_GET_SET( CSpell, scrollHigh )
DECL_GET_SET( CSpell, circle )
DECL_GET_SET( CSpell, lowSkill )
DECL_GET_SET( CSpell, highSkill )
DECL_GET_SET( CSpell, ginseng )
DECL_GET_SET( CSpell, moss )
DECL_GET_SET( CSpell, drake )
DECL_GET_SET( CSpell, pearl )
DECL_GET_SET( CSpell, silk )
DECL_GET_SET( CSpell, ash )
DECL_GET_SET( CSpell, shade )
DECL_GET_SET( CSpell, garlic )
DECL_GET_SET( CSpell, requireTarget )
DECL_GET_SET( CSpell, requireItem )
DECL_GET_SET( CSpell, requireLocation )
DECL_GET_SET( CSpell, requireChar )
DECL_GET_SET( CSpell, travelSpell )
DECL_GET_SET( CSpell, fieldSpell )
DECL_GET_SET( CSpell, reflectable )
DECL_GET_SET( CSpell, aggressiveSpell )
DECL_GET_SET( CSpell, resistable )
DECL_GET_SET( CSpell, soundEffect )
DECL_GET_SET( CSpell, enabled )
DECL_GET_SET( CSpell, baseDmg )

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

// Property table

inline JSPropertySpec CSpellProperties[] =
{
  JS_PSGS( "id",               JSCSpell_get_id,              JSCSpell_set_id,              JSPROP_ENUMANDPERM ),
  JS_PSGS( "action",           JSCSpell_get_action,          JSCSpell_set_action,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "delay",            JSCSpell_get_delay,           JSCSpell_set_delay,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "damageDelay",      JSCSpell_get_damageDelay,     JSCSpell_set_damageDelay,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "recoveryDelay",    JSCSpell_get_recoveryDelay,   JSCSpell_set_recoveryDelay,   JSPROP_ENUMANDPERM ),
  JS_PSGS( "health",           JSCSpell_get_health,          JSCSpell_set_health,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "stamina",          JSCSpell_get_stamina,         JSCSpell_set_stamina,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "mana",             JSCSpell_get_mana,            JSCSpell_set_mana,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "mantra",           JSCSpell_get_mantra,          JSCSpell_set_mantra,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "name",             JSCSpell_get_name,            JSCSpell_set_name,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "strToSay",         JSCSpell_get_strToSay,        JSCSpell_set_strToSay,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "scrollLow",        JSCSpell_get_scrollLow,       JSCSpell_set_scrollLow,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "scrollHigh",       JSCSpell_get_scrollHigh,      JSCSpell_set_scrollHigh,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "circle",           JSCSpell_get_circle,          JSCSpell_set_circle,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "lowSkill",         JSCSpell_get_lowSkill,        JSCSpell_set_lowSkill,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "highSkill",        JSCSpell_get_highSkill,       JSCSpell_set_highSkill,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "ginseng",          JSCSpell_get_ginseng,         JSCSpell_set_ginseng,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "moss",             JSCSpell_get_moss,            JSCSpell_set_moss,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "drake",            JSCSpell_get_drake,           JSCSpell_set_drake,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "pearl",            JSCSpell_get_pearl,           JSCSpell_set_pearl,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "silk",             JSCSpell_get_silk,            JSCSpell_set_silk,            JSPROP_ENUMANDPERM ),
  JS_PSGS( "ash",              JSCSpell_get_ash,             JSCSpell_set_ash,             JSPROP_ENUMANDPERM ),
  JS_PSGS( "shade",            JSCSpell_get_shade,           JSCSpell_set_shade,           JSPROP_ENUMANDPERM ),
  JS_PSGS( "garlic",           JSCSpell_get_garlic,          JSCSpell_set_garlic,          JSPROP_ENUMANDPERM ),
  JS_PSGS( "requireTarget",    JSCSpell_get_requireTarget,   JSCSpell_set_requireTarget,   JSPROP_ENUMANDPERM ),
  JS_PSGS( "requireItem",      JSCSpell_get_requireItem,     JSCSpell_set_requireItem,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "requireLocation",  JSCSpell_get_requireLocation, JSCSpell_set_requireLocation, JSPROP_ENUMANDPERM ),
  JS_PSGS( "requireChar",      JSCSpell_get_requireChar,     JSCSpell_set_requireChar,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "travelSpell",      JSCSpell_get_travelSpell,     JSCSpell_set_travelSpell,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "fieldSpell",       JSCSpell_get_fieldSpell,      JSCSpell_set_fieldSpell,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "reflectable",      JSCSpell_get_reflectable,     JSCSpell_set_reflectable,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "aggressiveSpell",  JSCSpell_get_aggressiveSpell, JSCSpell_set_aggressiveSpell, JSPROP_ENUMANDPERM ),
  JS_PSGS( "resistable",       JSCSpell_get_resistable,      JSCSpell_set_resistable,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "soundEffect",      JSCSpell_get_soundEffect,     JSCSpell_set_soundEffect,     JSPROP_ENUMANDPERM ),
  JS_PSGS( "enabled",          JSCSpell_get_enabled,         JSCSpell_set_enabled,         JSPROP_ENUMANDPERM ),
  JS_PSGS( "baseDmg",          JSCSpell_get_baseDmg,         JSCSpell_set_baseDmg,         JSPROP_ENUMANDPERM ),
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
  JS_PSGS( "race",		JSCRegion_get_race,	   JSCRegion_set_race,      JSPROP_ENUMANDPERM ),
  JS_PSGS( "tax",		JSCRegion_get_tax,     JSCRegion_set_tax,       JSPROP_ENUMANDPERM ),
  JS_PSGS( "taxResource",	JSCRegion_get_taxResource,   JSCRegion_set_taxResource,        JSPROP_ENUMANDPERM ),
  JS_PSGS( "canMark",	    JSCRegion_get_canMark,	   JSCRegion_set_canMark,	  JSPROP_ENUMANDPERM ),
  JS_PSGS( "canRecall",	JSCRegion_get_canRecall,	   JSCRegion_set_canRecall,	  JSPROP_ENUMANDPERM ),
  JS_PSGS( "canGate",		JSCRegion_get_canGate,	   JSCRegion_set_canGate,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "canTeleport",	JSCRegion_get_canTeleport,  JSCRegion_set_canTeleport,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "canPlaceHouse",   JSCRegion_get_canPlaceHouse,   JSCRegion_set_canPlaceHouse,    JSPROP_ENUMANDPERM ),
  JS_PSGS( "isGuarded",	JSCRegion_get_isGuarded,  JSCRegion_set_isGuarded,	      JSPROP_ENUMANDPERM ),
  JS_PSGS( "canCastAggressive",   JSCRegion_get_canCastAggressive,   JSCRegion_set_canCastAggressive,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "isSafeZone",	JSCRegion_get_isSafeZone,   JSCRegion_set_isSafeZone,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "health",	    JSCRegion_get_health,	   JSCRegion_set_health,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "isDungeon",	    JSCRegion_get_isDungeon,	   JSCRegion_set_isDungeon,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "worldNumber",	    JSCRegion_get_worldNumber,	   JSCRegion_set_worldNumber,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "instanceID",	    JSCRegion_get_instanceID,	   JSCRegion_set_instanceID,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "chanceBigOre",	    JSCRegion_get_chanceBigOre,	   JSCRegion_set_chanceBigOre,  JSPROP_ENUMANDPERM ),
  JS_PSG( "numOrePrefs",		JSCRegion_get_numOrePrefs,	JSPROP_ENUMPERMRO ),
  JS_PSG( "population",			JSCRegion_get_population,			JSPROP_ENUMPERMRO ),
//  { "members",			CREGP_MEMBERS,				JSPROP_ENUMPERMIDX, nullptr, nullptr },
  JS_PSG( "members",			JSCRegion_get_members,			JSPROP_ENUMERATE ), // trying to figure this one out
  JS_PSGS( "id",	    JSCRegion_get_id,	   JSCRegion_set_id,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTrigger",	    JSCRegion_get_scriptTrigger,	   JSCRegion_set_scriptTrigger,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "scriptTriggers",	    JSCRegion_get_scriptTriggers,	   JSCRegion_set_scriptTriggers,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "numGuards",	    JSCRegion_get_numGuards,	   JSCRegion_set_numGuards,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "taxes",	    JSCRegion_get_taxes,	   JSCRegion_set_taxes,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "reserves",	    JSCRegion_get_reserves,	   JSCRegion_set_reserves,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "appearance",	    JSCRegion_get_appearance,	   JSCRegion_set_appearance,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "music",	    JSCRegion_get_music,	   JSCRegion_set_music,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "weather",	    JSCRegion_get_weather,	   JSCRegion_set_weather,  JSPROP_ENUMANDPERM ),
  JS_PSGS( "owner",	    JSCRegion_get_owner,	   JSCRegion_set_owner,  JSPROP_ENUMANDPERM ),
  JS_PS_END
};

inline JSPropertySpec CSpawnRegionProperties[] =
{
  { "name",				CSPAWNREGP_NAME,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "regionNum",			CSPAWNREGP_REGIONNUM,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "itemList",			CSPAWNREGP_ITEMLIST,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "npcList",			CSPAWNREGP_NPCLIST,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "item",				CSPAWNREGP_ITEM,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "npc",				CSPAWNREGP_NPC,						JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxItems",			CSPAWNREGP_MAXITEMS,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxNpcs",			CSPAWNREGP_MAXNPCS,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "itemCount",			CSPAWNREGP_ITEMCOUNT,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "npcCount",			CSPAWNREGP_NPCCOUNT,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "onlyOutside",		CSPAWNREGP_ONLYOUTSIDE,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isSpawner",			CSPAWNREGP_ISSPAWNER,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "defZ",				CSPAWNREGP_DEFZ,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "prefZ",				CSPAWNREGP_PREFZ,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "x1",					CSPAWNREGP_X1,						JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "y1",					CSPAWNREGP_X2,						JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "x2",					CSPAWNREGP_X2,						JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "y2",					CSPAWNREGP_Y2,						JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "world",				CSPAWNREGP_WORLD,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "instanceID",			CSPAWNREGP_INSTANCEID,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "minTime",			CSPAWNREGP_MINTIME,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxTime",			CSPAWNREGP_MAXTIME,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "call",				CSPAWNREGP_CALL,					JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CGuildProperties[] =
{
  { "name",			CGP_NAME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "type",			CGP_TYPE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "master",			CGP_MASTER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stone",			CGP_STONE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "numMembers",		CGuild_get_numMembers,		JSPROP_ENUMPERMRO ),
  JS_PSG( "numRecruits",	CGuild_get_numRecruits,	JSPROP_ENUMPERMRO ),
  { "members",		CGP_MEMBERS,		JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "recruits",		CGP_RECRUITS,		JSPROP_ENUMPERMIDX, nullptr, nullptr },
  { "charter",		CGP_CHARTER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "abbreviation",	CGP_ABBREVIATION,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "webPage",		CGP_WEBPAGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PS_END
};

inline JSPropertySpec CCharacterProps[] =
{
  { "sectionID",		CCP_SECTIONID,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "name",			CCP_NAME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "origName",		CCP_ORIGNAME,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "title",			CCP_TITLE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "x",				CCP_X,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "y",				CCP_Y,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "z",				CCP_Z,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldX",			CCP_OLDX,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldY",			CCP_OLDY,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldZ",			CCP_OLDZ,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "id",				CCP_ID,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "colour",			CCP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "color",			CCP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skin",			CCP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hue",			CCP_COLOUR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "controlSlots",	CCP_CONTROLSLOTS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "controlSlotsUsed",	CCP_CONTROLSLOTSUSED,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "orneriness",		CCP_ORNERINESS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "owner",			CCP_OWNER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "visible",		CCP_VISIBLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "serial",			CCP_SERIAL,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "health",			CCP_HEALTH,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "scripttrigger",	CCP_SCRIPTTRIGGER,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "scriptTriggers",	CCP_SCRIPTTRIGGERS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "worldnumber",	CCP_WORLDNUMBER,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "instanceID",		CCP_INSTANCEID,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "target",			CCP_TARGET,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "dexterity",		CCP_DEXTERITY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "intelligence",	CCP_INTELLIGENCE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "strength",		CCP_STRENGTH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "actualDexterity",	CCP_ACTUALDEXTERITY,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "actualIntelligence",	CCP_ACTUALINTELLIGENCE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "actualStrength",		CCP_ACTUALSTRENGTH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skills",			CCP_SKILLS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "mana",			CCP_MANA,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stamina",		CCP_STAMINA,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "pack",			CCP_CHARPACK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fame",			CCP_FAME,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "karma",			CCP_KARMA,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "attack",			CCharacter_get_attack,			JSPROP_ENUMPERMRO ),
  { "canAttack",		CCP_CANATTACK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fleeAt",			CCP_FLEEAT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "reAttackAt",		CCP_REATTACKAT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "brkPeaceChance",	CCP_BRKPEACE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "setPeace",		CCP_SETPEACE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hunger",			CCP_HUNGER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "hungerRate",		CCharacter_get_hungerRate,		JSPROP_ENUMPERMRO ),
  { "thirst",			CCP_THIRST,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "thirstRate",		CCharacter_get_thirstRate,		JSPROP_ENUMPERMRO ),
  { "frozen",			CCP_FROZEN,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "commandlevel",	CCP_COMMANDLEVEL,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "race",			CCP_RACE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hasStolen",		CCP_HASSTOLEN,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "criminal",		CCP_CRIMINAL,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "murderer",		CCharacter_get_murderer,		JSPROP_ENUMPERMRO ),
  { "innocent",		CCP_INNOCENT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "murdercount",	CCP_MURDERCOUNT,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "neutral",		CCP_NEUTRAL,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "npcFlag",		CCP_NPCFLAG,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "npcGuild",		CCP_NPCGUILD,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "gender",			CCP_GENDER,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "dead",			CCharacter_get_dead,			JSPROP_ENUMPERMRO ),
  { "npc",			CCP_NPC,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isAwake",		CCP_AWAKE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "online",			CCharacter_get_online,			JSPROP_ENUMPERMRO ),
  { "direction",		CCP_DIRECTION,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isRunning",		CCP_ISRUNNING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "region",			CCP_REGION,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "town",			CCP_TOWN,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "guild",			CCP_GUILD,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "baseskills",		CCP_BASESKILLS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skillsused",		CCP_SKILLUSE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "socket",			CCP_SOCKET,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  JS_PSG( "isChar",			CCharacter_get_isChar,			JSPROP_ENUMPERMRO ),
  JS_PSG( "isItem",			CCharacter_get_isItem,			JSPROP_ENUMPERMRO ),
  JS_PSG( "isSpawner",		CCharacter_get_isSpawner,		JSPROP_ENUMPERMRO ),
  JS_PSG( "spawnSerial",	CCharacter_get_spawnSerial,	JSPROP_ENUMPERMRO ),
  { "maxhp",			CCP_MAXHP,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxstamina",		CCP_MAXSTAMINA,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxmana",		CCP_MAXMANA,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "oldWandertype",	CCP_OLDWANDERTYPE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "wandertype",		CCP_WANDERTYPE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fx1",			CCP_FX1,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fy1",			CCP_FY1,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fx2",			CCP_FX2,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fy2",			CCP_FY2,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fz",				CCP_FZ,				JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isonhorse",		CCP_ISONHORSE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isFlying",		CCP_ISFLYING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isGuarded",		CCP_ISGUARDED,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "guarding",		CCP_GUARDING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempdex",		CCP_TDEXTERITY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempint",		CCP_TINTELLIGENCE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tempstr",		CCP_TSTRENGTH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "poison",			CCP_POISON,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "lightlevel",		CCP_LIGHTLEVEL,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "vulnerable",		CCP_VULNERABLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "willhunger",		CCP_HUNGERSTATUS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "willthirst",		CCP_THIRSTSTATUS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "lodamage",		CCP_LODAMAGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hidamage",		CCP_HIDAMAGE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "flag",			CCP_FLAG,			JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "atWar",			CCP_ATWAR,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "spellCast",		CCP_SPELLCAST,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isCasting",		CCP_ISCASTING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "priv",			CCP_PRIV,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "townPriv",		CCP_TOWNPRIV,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "guildTitle",		CCP_GUILDTITLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hairStyle",		CCP_HAIRSTYLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hairColour",		CCP_HAIRCOLOUR,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hairColor",		CCP_HAIRCOLOUR,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "beardStyle",		CCP_BEARDSTYLE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "beardColour",	CCP_BEARDCOLOUR,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "beardColor",		CCP_BEARDCOLOUR,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "fontType",		CCP_FONTTYPE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "sayColour",		CCP_SAYCOLOUR,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "emoteColour",	CCP_EMOTECOLOUR,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "attacker",		CCP_ATTACKER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "raceGate",		CCP_RACEGATE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skillLock",		CCP_SKILLLOCK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "deaths",			CCP_DEATHS,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "ownerCount",		CCP_OWNERCOUNT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "nextAct",		CCP_NEXTACT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "petCount",		CCP_PETCOUNT,		JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "followerCount",	CCP_FOLLOWERCOUNT,	JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "ownedItemsCount",CCP_OWNEDITEMSCOUNT,JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "cell",			CCP_CELL,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "allmove",		CCP_ALLMOVE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "houseicons",		CCP_HOUSEICONS,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "spattack",		CCP_SPATTACK,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "spdelay",		CCP_SPDELAY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "aitype",			CCP_AITYPE,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "split",			CCP_SPLIT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "splitchance",	CCP_SPLITCHANCE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hireling",		CCP_HIRELING,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "trainer",		CCP_TRAINER,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "weight",			CCP_WEIGHT,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "squelch",		CCP_SQUELCH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isJailed",		CCP_ISJAILED,		JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "magicReflect",	CCP_MAGICREFLECT,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "permanentMagicReflect",	CCP_PERMMAGICREFLECT,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tamed",			CCP_TAMED,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tamedHungerRate",CCP_TAMEDHUNGERRATE,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "tamedThirstRate",CCP_TAMEDTHIRSTRATE,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "hungerWildChance",CCP_HUNGERWILDCHANCE,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "thirstWildChance",CCP_THIRSTWILDCHANCE,JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "foodList",		CCP_FOODLIST,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "mounted",		CCP_MOUNTED,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stabled",		CCP_STABLED,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isUsingPotion",	CCP_USINGPOTION,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "stealth",		CCP_STEALTH,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skillToTame",	CCP_SKILLTOTAME,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skillToProv",	CCP_SKILLTOPROV,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "skillToPeace",	CCP_SKILLTOPEACE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "poisonStrength",	CCP_POISONSTRENGTH,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isPolymorphed",	CCP_ISPOLYMORPHED,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isIncognito",	CCP_ISINCOGNITO,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isDisguised",	CCP_ISDISGUISED,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "canRun",			CCP_CANRUN,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isMeditating",	CCP_ISMEDITATING,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isGM",			CCP_ISGM,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "canBroadcast",	CCP_CANBROADCAST,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "singClickSer",	CCP_SINGCLICKSER,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "noSkillTitles",	CCP_NOSKILLTITLES,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isGMPageable",	CCP_ISGMPAGEABLE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "canSnoop",		CCP_CANSNOOP,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isCounselor",	CCP_ISCOUNSELOR,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "noNeedMana",		CCP_NONEEDMANA,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isDispellable",	CCP_ISDISPELLABLE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "noNeedReags",	CCP_NONEEDREAGS,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "orgID",			CCP_ORGID,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "orgSkin",		CCP_ORGSKIN,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "isAnimal",		CCP_ISANIMAL,		JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isHuman",		CCP_ISHUMAN,		JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "isShop",			CCP_ISSHOP,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "maxLoyalty",		CCP_MAXLOYALTY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "loyalty",		CCP_LOYALTY,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "loyaltyRate",	CCP_LOYALTYRATE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "shouldSave",		CCP_SHOULDSAVE,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "origin",			CCP_ORIGIN,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "partyLootable",	CCP_PARTYLOOTABLE,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "party",			CCP_PARTY,			JSPROP_ENUMPERMRO, nullptr, nullptr },
  { "multi",			CCP_MULTI,			JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "accountNum",		CCP_ACCOUNTNUM,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "account",		CCP_ACCOUNT,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "createdOn",		CCP_CREATEDON,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "playTime",		CCP_PLAYTIME,		JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "housesOwned",	CCP_HOUSESOWNED,	JSPROP_ENUMANDPERM, nullptr, nullptr },
  { "housesCoOwned",	CCP_HOUSESCOOWNED,	JSPROP_ENUMANDPERM, nullptr, nullptr },
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
