// Version History
// 1.0		 		14th December, 2001
//			Initial implementation
//			Defines the JSClass objects for Char, Item, Race, Guild and Region



#ifndef __UOXJSClasses__
#define __UOXJSClasses__
#include "UOXJSPropertyFuncs.h"
#include <js/TypeDecls.h>  // Ensure proper definitions for JSFreeOp and JSObject
#include <jsapi.h>  // Make sure you include this for SpiderMonkey APIs

static constexpr JSClassOps classOpsWithFinalize = {
    nullptr,  // addProperty
    nullptr,  // deleteProperty
    nullptr,  // enumerate
    nullptr,  // newEnumerate
    nullptr,  // resolve
    nullptr,  // mayResolve
    nullptr,  // finalize; reserved slots point to engine-owned UOX3 objects
    nullptr,  // call
    nullptr,  // construct
    nullptr   // trace
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
	JSCLASS_HAS_RESERVED_SLOTS(2),
	&classOpsWithFinalize 
};

bool ResolveSpellCollection( JSContext *cx, JS::HandleObject obj, JS::HandleId id, bool *resolved );
bool ResolveCreateEntryCollection( JSContext *cx, JS::HandleObject obj, JS::HandleId id, bool *resolved );

static constexpr JSClassOps spellCollectionClassOps = {
	nullptr, nullptr, nullptr, nullptr, ResolveSpellCollection,
	nullptr, nullptr, nullptr, nullptr, nullptr
};

static constexpr JSClassOps createEntryCollectionClassOps = {
	nullptr, nullptr, nullptr, nullptr, ResolveCreateEntryCollection,
	nullptr, nullptr, nullptr, nullptr, nullptr
};

inline JSClass UOXSpell_class =
{
	"UOXSpell",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXSpells_class =
{
	"UOXSpells",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &spellCollectionClassOps
};

inline JSClass UOXGlobalSkill_class =
{
	"UOXGlobalSkill",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline  JSClass UOXGlobalSkills_class =
{
	"UOXGlobalSkills",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXCreateEntry_class =
{
	"UOXCreateEntry",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXCreateEntries_class =
{
	"UOXCreateEntries",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &createEntryCollectionClassOps
};

inline JSClass UOXTimer_class =
{
	"UOXTimer",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXBase_class =
{
	"UOXBase",
	JSCLASS_HAS_RESERVED_SLOTS(2),
	&classOpsWithFinalize
};

inline JSClass UOXChar_class =
{
	"UOXChar",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXItem_class =
{
	"UOXItem",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

//
// What this class does:
// Returns the skill-values for a character
// and is able to set them too
//
inline JSClass UOXSkills_class =
{
	"UOXSkills",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXBaseSkills_class =
{
	"UOXBaseSkills",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXSkillsUsed_class =
{
	"UOXSkillsUsed",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXSkillsLock_class =
{
	"UOXSkillsLock",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXSkillsCap_class =
{
  "UOXSkillsCap",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXRace_class =
{
	"UOXRace",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXGuild_class =
{
	"UOXGuild",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXRegion_class =
{
	"UOXRegion",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXSpawnRegion_class =
{
	"UOXSpawnRegion",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXSocket_class =
{
	"UOXSocket",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXFile_class =
{
	"UOXCFile",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXGump_class =
{
	"Gump",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

//o------------------------------------------------------------------------------------------------o
//|	Class		-	static JSClass UOXGumpData_class =
//|	Date		-	1/21/2003 7:35:37 AM
//o------------------------------------------------------------------------------------------------o
inline JSClass UOXGumpData_class =
{
	"GumpData",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXAccount_class =
{
	"CAccountClass",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXConsole_class =
{
	"CConsoleClass",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXResource_class =
{
	"UOXResource",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXPacket_class =
{
	"Packet",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline JSClass UOXParty_class =
{
	"UOXParty",
	JSCLASS_HAS_RESERVED_SLOTS(2),
  &classOpsWithFinalize 
};

inline std::string convertToString(JSContext *cx, JSString *string)
 {
  // Ensure the JSString is rooted to prevent it from being garbage collected
  JS::Rooted<JSString*> rootedStr(cx, string);
  
  // Encode the JSString to ASCII
  JS::UniqueChars asciiChars = JS_EncodeStringToASCII(cx, rootedStr);
  if (!asciiChars) {
    // Handle encoding error
    return "";
  }
  
  // Convert the encoded C string to std::string
  return std::string(asciiChars.get());
}

inline JSString* convertFromString(JSContext* cx, const std::string& value) {
  JSString* jsStr = JS_NewStringCopyZ(cx, value.c_str());
  if (!jsStr) {
    // Handle error (e.g., out of memory)
    // This could involve throwing an exception, logging an error, etc.
    return nullptr;
  }
  return jsStr;
}

#endif
