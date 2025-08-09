// Version History
// 1.0		 		14th December, 2001
//			Initial implementation
//			Defines the JSClass objects for Char, Item, Race, Guild and Region



#ifndef __UOXJSClasses__
#define __UOXJSClasses__
#include "UOXJSPropertyFuncs.h"

inline JSClass global_class =
{
	"global",
	JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub,   // addProperty;
	JS_PropertyStub,	 // delProperty;
	JS_PropertyStub,	 // getProperty;
	JS_StrictPropertyStub,	 // setProperty;
	JS_EnumerateStub,	 // enumerate;
	JS_ResolveStub,		 // resolve;
	JS_ConvertStub,		 // convert;
	JS_FinalizeStub,	 // finalize;
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass uox_class =
{
	"uoxscript",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CScriptProps_getProperty,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSpell_class =
{
	"UOXSpell",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSpellProps_getProperty,
	CSpellProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSpells_class =
{
	"UOXSpells",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSpellsProps_getProperty,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXGlobalSkill_class =
{
	"UOXGlobalSkill",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CGlobalSkillProps_getProperty,
	CGlobalSkillProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline  JSClass UOXGlobalSkills_class =
{
	"UOXGlobalSkills",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CGlobalSkillsProps_getProperty,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXCreateEntry_class =
{
	"UOXCreateEntry",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CCreateEntryProps_getProperty,
	CCreateEntryProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXCreateEntries_class =
{
	"UOXCreateEntries",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CCreateEntriesProps_getProperty,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXTimer_class =
{
	"UOXTimer",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CTimerProps_getProperty,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXChar_class =
{
	"UOXChar",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CCharacterProps_getProperty,
	CCharacterProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXItem_class =
{
	"UOXItem",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CItemProps_getProperty,
	CItemProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

//
// What this class does:
// Returns the skill-values for a character
// and is able to set them too
//
inline JSClass UOXSkills_class =
{
	"UOXSkills",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSkillsProps_getProperty,
	CSkillsProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXBaseSkills_class =
{
	"UOXBaseSkills",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSkillsProps_getProperty,
	CSkillsProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSkillsUsed_class =
{
	"UOXSkillsUsed",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSkillsProps_getProperty,
	CSkillsProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSkillsLock_class =
{
	"UOXSkillsLock",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSkillsProps_getProperty,
	CSkillsProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSkillsCap_class =
{
    "UOXSkillsCap",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    CSkillsProps_getProperty,
    CSkillsProps_setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXRace_class =
{
	"UOXRace",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CRaceProps_getProperty,
	CRaceProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXGuild_class =
{
	"UOXGuild",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CGuildProps_getProperty,
	CGuildProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXRegion_class =
{
	"UOXRegion",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CRegionProps_getProperty,
	CRegionProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSpawnRegion_class =
{
	"UOXSpawnRegion",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSpawnRegionProps_getProperty,
	CSpawnRegionProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXSocket_class =
{
	"UOXSocket",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSocketProps_getProperty,
	CSocketProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXFile_class =
{
	"UOXCFile",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

// var myGump = new Gump; // should be possible
inline JSClass UOXGump_class =
{
	"Gump",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,			// addProperty
	JS_PropertyStub,			// delProperty
	JS_PropertyStub,			// getProperty
	JS_StrictPropertyStub,			// setProperty
	JS_EnumerateStub,			// enumerate
	JS_ResolveStub,				// resolve
	JS_ConvertStub,				// convert
	JS_FinalizeStub,			// finalize
	JSCLASS_NO_OPTIONAL_MEMBERS		// reserved slots, checkAccess, call, construct, xdrObject, hasInstance, etc...
};

//o------------------------------------------------------------------------------------------------o
//|	Class		-	static JSClass UOXGumpData_class =
//|	Date		-	1/21/2003 7:35:37 AM
//o------------------------------------------------------------------------------------------------o
inline JSClass UOXGumpData_class =
{
	"GumpData",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CGumpDataProps_getProperty,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXAccount_class =
{
	"CAccountClass",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CAccountProps_getProperty,
	CAccountProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXConsole_class =
{
	"CConsoleClass",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CConsoleProps_getProperty,
	CConsoleProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXResource_class =
{
	"UOXResource",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CResourceProps_getProperty,
	CResourceProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXPacket_class =
{
	"Packet",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

inline JSClass UOXParty_class =
{
	"UOXParty",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CPartyProps_getProperty,
	CPartyProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

#endif
