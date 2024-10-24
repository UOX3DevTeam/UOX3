// Version History
// 1.0		 		14th December, 2001
//			Initial implementation
//			Defines the JSClass objects for Char, Item, Race, Guild and Region



#ifndef __UOXJSClasses__
#define __UOXJSClasses__
#include "UOXJSPropertyFuncs.h"
#include "CChar.h"  // Ensure you include the header for CChar
#include <js/TypeDecls.h>  // Ensure proper definitions for JSFreeOp and JSObject
#include <jsapi.h>  // Make sure you include this for SpiderMonkey APIs


// Correct signature for JSFinalizeOp
void UOXClassFinalize(JS::GCContext* gcx, JSObject* obj)
{
    // Use JS::GetReservedSlot to retrieve the reserved slot data (index 0)
    JS::Value privateValue = JS::GetReservedSlot(obj, 0);  
    CChar* privateData = static_cast<CChar*>(privateValue.toPrivate());  // Convert to private data

    if (privateData != nullptr)
    {
        delete privateData;  // Clean up memory
        JS::SetReservedSlot(obj, 0, JS::NullValue());  // Clear the reserved slot to prevent issues
    }
}

// Class operations with finalize
static constexpr JSClassOps classOpsWithFinalize = {
    nullptr,  // addProperty
    nullptr,  // deleteProperty
    nullptr,  // enumerate
    nullptr,  // newEnumerate
    nullptr,  // resolve
    nullptr,  // mayResolve
    UOXClassFinalize,  // finalize
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
	0,
	&classOpsWithFinalize 
};

inline JSClass UOXSpell_class =
{
	"UOXSpell",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};

inline JSClass UOXSpells_class =
{
	"UOXSpells",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
// CSpellsProps_getProperty,

inline JSClass UOXGlobalSkill_class =
{
	"UOXGlobalSkill",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};

inline  JSClass UOXGlobalSkills_class =
{
	"UOXGlobalSkills",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CGlobalSkillsProps_getProperty,

inline JSClass UOXCreateEntry_class =
{
	"UOXCreateEntry",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CCreateEntryProps_getProperty,
//	CCreateEntryProps_setProperty,

inline JSClass UOXCreateEntries_class =
{
	"UOXCreateEntries",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CCreateEntriesProps_getProperty,

inline JSClass UOXTimer_class =
{
	"UOXTimer",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};

inline JSClass UOXChar_class =
{
	"UOXChar",
	JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
// CCharacterProps_getProperty,
// CCharacterProps_setProperty,
// CBaseObject_equality,

inline JSClass UOXItem_class =
{
	"UOXItem",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
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
  &classOpsWithFinalize 
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXBaseSkills_class =
{
	"UOXBaseSkills",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXSkillsUsed_class =
{
	"UOXSkillsUsed",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXSkillsLock_class =
{
	"UOXSkillsLock",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CSkillsProps_getProperty,
//	CSkillsProps_setProperty,

inline JSClass UOXRace_class =
{
	"UOXRace",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CRaceProps_setProperty,

inline JSClass UOXGuild_class =
{
	"UOXGuild",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CGuildProps_getProperty,
//	CGuildProps_setProperty,

inline JSClass UOXRegion_class =
{
	"UOXRegion",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CRegionProps_getProperty,
//	CRegionProps_setProperty,

inline JSClass UOXSpawnRegion_class =
{
	"UOXSpawnRegion",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CSpawnRegionProps_getProperty,
//	CSpawnRegionProps_setProperty,

inline JSClass UOXSocket_class =
{
	"UOXSocket",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//		CSocketProps_getProperty,
//		CSocketProps_setProperty,
//	CSocket_equality,

inline JSClass UOXFile_class =
{
	"UOXCFile",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};

inline JSClass UOXGump_class =
{
	"Gump",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};

//o------------------------------------------------------------------------------------------------o
//|	Class		-	static JSClass UOXGumpData_class =
//|	Date		-	1/21/2003 7:35:37 AM
//o------------------------------------------------------------------------------------------------o
inline JSClass UOXGumpData_class =
{
	"GumpData",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CGumpDataProps_getProperty,

inline JSClass UOXAccount_class =
{
	"CAccountClass",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CAccountProps_getProperty,
//	CAccountProps_setProperty,

inline JSClass UOXConsole_class =
{
	"CConsoleClass",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CConsoleProps_setProperty,

inline JSClass UOXResource_class =
{
	"UOXResource",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//	CResourceProps_getProperty,
//	CResourceProps_setProperty,

inline JSClass UOXPacket_class =
{
	"Packet",
  JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};

inline JSClass UOXParty_class =
{
	"UOXParty",
	JSCLASS_HAS_RESERVED_SLOTS(1),
  &classOpsWithFinalize 
};
//		CPartyProps_getProperty,
//		CPartyProps_setProperty,
//	CParty_equality,

/*inline std::string convertToString(JSContext *cx, JSString *string) {
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
}*/

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

/*inline JSString *convertFromString(JSContext* cx, const std::string& value) {
  return JS_NewStringCopyZ(cx, value.c_str());
}*/
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
