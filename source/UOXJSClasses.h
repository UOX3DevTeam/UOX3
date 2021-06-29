// Version History
// 1.0		 		14th December, 2001
//			Initial implementation
//			Defines the JSClass objects for Char, Item, Race, Guild and Region



#ifndef __UOXJSClasses__
#define __UOXJSClasses__
#include "UOXJSPropertyFuncs.h"

static JSClass global_class =
{
	"global",
	0,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};
static JSClass uox_class =
{
	"uoxscript",
	0,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSClass UOXSpell_class =
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
	JS_FinalizeStub
};

static  JSClass UOXSpells_class =
{
	"UOXSpells",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CSpellsProps_getProperty,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static  JSClass UOXCreateEntry_class =
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
	JS_FinalizeStub
};

static  JSClass UOXCreateEntries_class =
{
	"UOXCreateEntries",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CCreateEntriesProps_getProperty,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSExtendedClass UOXChar_class =
{
	{	"UOXChar",
		JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
		JS_PropertyStub,
		JS_PropertyStub,
		CCharacterProps_getProperty,
		CCharacterProps_setProperty,
		JS_EnumerateStub,
		JS_ResolveStub,
		JS_ConvertStub,
		JS_FinalizeStub,
		nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr
	},
	CBaseObject_equality,
	nullptr,
	nullptr,
	JSCLASS_NO_RESERVED_MEMBERS
};

static JSExtendedClass UOXItem_class =
{
	{	"UOXItem",
		JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
		JS_PropertyStub,
		JS_PropertyStub,
		CItemProps_getProperty,
		CItemProps_setProperty,
		JS_EnumerateStub,
		JS_ResolveStub,
		JS_ConvertStub,
		JS_FinalizeStub,
		nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr
	},
	CBaseObject_equality,
	nullptr,
	nullptr,
	JSCLASS_NO_RESERVED_MEMBERS
};

//
// What this class does:
// Returns the skill-values for a character
// and is able to set them too
//
static JSClass UOXSkills_class =
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
	JS_FinalizeStub
};

static JSClass UOXBaseSkills_class =
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
	JS_FinalizeStub
};

static JSClass UOXSkillsUsed_class =
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
	JS_FinalizeStub
};

static JSClass UOXSkillsLock_class =
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
	JS_FinalizeStub
};

static JSClass UOXRace_class =
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
	JS_FinalizeStub
};

static JSClass UOXGuild_class =
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
	JS_FinalizeStub
};

static JSClass UOXGuilds_class =
{
	"UOXGuilds",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CGuildsProps_getProperty,
	CGuildsProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSClass UOXRegion_class =
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
	JS_FinalizeStub
};

static JSClass UOXSpawnRegion_class =
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
	JS_FinalizeStub
};

static JSExtendedClass UOXSocket_class =
{
	{	"UOXSocket",
		JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
		JS_PropertyStub,
		JS_PropertyStub,
		CSocketProps_getProperty,
		CSocketProps_setProperty,
		JS_EnumerateStub,
		JS_ResolveStub,
		JS_ConvertStub,
		JS_FinalizeStub,
		nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr
	},
	CSocket_equality,
	nullptr,
	nullptr,
	JSCLASS_NO_RESERVED_MEMBERS
};

static JSClass UOXFile_class =
{
	"UOXCFile",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

// var myGump = new Gump; // should be possible
static JSClass UOXGump_class =
{
	"Gump",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

//o-----------------------------------------------------------------------------------------------o
//|	Class		-	static JSClass UOXGumpData_class =
//|	Date		-	1/21/2003 7:35:37 AM
//o-----------------------------------------------------------------------------------------------o
static JSClass UOXGumpData_class =
{
	"GumpData",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CGumpDataProps_getProperty,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSClass UOXAccount_class =
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
	JS_FinalizeStub
};

static JSClass UOXConsole_class =
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
	JS_FinalizeStub
};

static JSClass UOXScriptSection_class =
{
	"UOXScriptSection",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	CScriptSectionProps_getProperty,
	CScriptSectionProps_setProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSClass UOXResource_class =
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
	JS_FinalizeStub
};

static JSClass UOXPacket_class =
{
	"Packet",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSExtendedClass UOXParty_class =
{
	{	"UOXParty",
		JSCLASS_HAS_PRIVATE | JSCLASS_IS_EXTENDED,
		JS_PropertyStub,
		JS_PropertyStub,
		CPartyProps_getProperty,
		CPartyProps_setProperty,
		JS_EnumerateStub,
		JS_ResolveStub,
		JS_ConvertStub,
		JS_FinalizeStub,
		nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr
	},
	CParty_equality,
	nullptr,
	nullptr,
	JSCLASS_NO_RESERVED_MEMBERS
};

#endif
