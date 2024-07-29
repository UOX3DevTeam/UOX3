// Version History
// 1.0		 		14th December, 2001
//			Initial implementation
//			Defines the JSClass objects for Char, Item, Race, Guild and Region



#ifndef __UOXJSClasses__
#define __UOXJSClasses__
#include "UOXJSPropertyFuncs.h"

static constexpr JSClassOps defaultClassOps = {
    nullptr, // addProperty
    nullptr, // deleteProperty
    nullptr, // enumerate
    nullptr, // newEnumerate
    nullptr, // resolve
    nullptr, // mayResolve
    nullptr, // finalize
    nullptr, // call
    nullptr, // construct
    nullptr, // trace
};

inline JSClass global_class =
{
	"global",
	JSCLASS_GLOBAL_FLAGS, 
	&JS::DefaultGlobalClassOps
};

inline JSClass uox_class =
{
	"uoxscript",
	0,
	&defaultClassOps
};

inline JSClass UOXSpell_class =
{
	"UOXSpell",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};

inline JSClass UOXSpells_class =
{
	"UOXSpells",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
// CSpellsProps_getProperty,

inline JSClass UOXGlobalSkill_class =
{
	"UOXGlobalSkill",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};

inline  JSClass UOXGlobalSkills_class =
{
	"UOXGlobalSkills",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CGlobalSkillsProps_getProperty,

inline JSClass UOXCreateEntry_class =
{
	"UOXCreateEntry",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CCreateEntryProps_getProperty,
//	CCreateEntryProps_setProperty,

inline JSClass UOXCreateEntries_class =
{
	"UOXCreateEntries",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CCreateEntriesProps_getProperty,

inline JSClass UOXTimer_class =
{
	"UOXTimer",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};

inline JSClass UOXChar_class =
{
	"UOXChar",
	JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
// CCharacterProps_getProperty,
// CCharacterProps_setProperty,
// CBaseObject_equality,

inline JSClass UOXItem_class =
{
	"UOXItem",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
// CItemProps_getProperty,
// CItemProps_setProperty,
// CBaseObject_equality,

//
// What this class does:
// Returns the skill-values for a character
// and is able to set them too
//
inline JSClass UOXSkills_class =
{
	"UOXSkills",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXBaseSkills_class =
{
	"UOXBaseSkills",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXSkillsUsed_class =
{
	"UOXSkillsUsed",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXSkillsLock_class =
{
	"UOXSkillsLock",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXRace_class =
{
	"UOXRace",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CRaceProps_setProperty,

inline JSClass UOXGuild_class =
{
	"UOXGuild",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CGuildProps_getProperty,
//	CGuildProps_setProperty,

inline JSClass UOXRegion_class =
{
	"UOXRegion",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CRegionProps_getProperty,
//	CRegionProps_setProperty,

inline JSClass UOXSpawnRegion_class =
{
	"UOXSpawnRegion",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CSpawnRegionProps_getProperty,
//	CSpawnRegionProps_setProperty,

inline JSClass UOXSocket_class =
{
	"UOXSocket",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//		CSocketProps_getProperty,
//		CSocketProps_setProperty,
//	CSocket_equality,

inline JSClass UOXFile_class =
{
	"UOXCFile",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};

inline JSClass UOXGump_class =
{
	"Gump",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};

//o------------------------------------------------------------------------------------------------o
//|	Class		-	static JSClass UOXGumpData_class =
//|	Date		-	1/21/2003 7:35:37 AM
//o------------------------------------------------------------------------------------------------o
inline JSClass UOXGumpData_class =
{
	"GumpData",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CGumpDataProps_getProperty,

inline JSClass UOXAccount_class =
{
	"CAccountClass",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CAccountProps_getProperty,
//	CAccountProps_setProperty,

inline JSClass UOXConsole_class =
{
	"CConsoleClass",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CConsoleProps_setProperty,

inline JSClass UOXResource_class =
{
	"UOXResource",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//	CResourceProps_getProperty,
//	CResourceProps_setProperty,

inline JSClass UOXPacket_class =
{
	"Packet",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};

inline JSClass UOXParty_class =
{
	"UOXParty",
	JSCLASS_HAS_RESERVED_SLOTS(1),
  &defaultClassOps
};
//		CPartyProps_getProperty,
//		CPartyProps_setProperty,
//	CParty_equality,

inline std::string convertToString(JSContext *cx, JSString *string) {
  // For some reason, no matter what I do here, I'm crashing ... do I need to have it rooted?
    //auto chars = JS_EncodeStringToASCII(cx, string);
    //JS::Rooted< JSString* > str(cx, string);
    //JS::UniqueChars message_ascii(JS_EncodeStringToASCII(cx, str));
    //std::string rVal = message_ascii.get();
    //return rVal;
  JS::Rooted<JSString*> rootedStr(cx, string);
  JS::UniqueChars asciiChars = JS_EncodeStringToASCII(cx, rootedStr);
  if (!asciiChars) {
    // Handle encoding error
    return "";
  }
  std::string asciiString(asciiChars.get());
  return asciiString;
}

inline JSString *convertFromString(JSContext* cx, const std::string& value) {
  return JS_NewStringCopyZ(cx, value.c_str());
}

#endif
