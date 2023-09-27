// Here are the functions that are exposed to the Script Engine
#include "sefunctions.h"

#include <filesystem>
#include <limits>
#include <map>

#include "cbaseobject.h"
#include "cchar.h"
#include "cdice.h"
#include "ceffects.h"
#include "cguild.h"
#include "cgump.h"
#include "chtmlsystem.h"
#include "citem.h"
#include "cjsengine.h"
#include "cjsmapping.h"
#include "classes.h"
#include "cmagic.h"
#include "cmultiobj.h"
#include "combat.h"
#include "commands.h"
#include "cpacketsend.h"
#include "craces.h"
#include "cscript.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "cspawnregion.h"
#include "cthreadqueue.h"
#include "dictionary.h"
#include "type/era.hpp"
#include "funcdecl.h"
#include "jsencapsulate.h"
#include "magic.h"
#include "mapstuff.h"
#include "movement.h"
#include "network.h"
#include "objectfactory.h"
#include "ostype.h"
#include "partysystem.h"
#include "regions.h"
#include "configuration/serverconfig.hpp"
#include "skills.h"
#include "speech.h"
#include "ssection.h"
#include "utility/strutil.hpp"
#include "townregion.h"
#include "uodata/uoflag.hpp"
#include "uoxjsclasses.h"
#include "uoxjspropertyspecs.h"
#include "other/uoxversion.hpp"

extern CDictionaryContainer worldDictionary ;
extern CHandleCombat worldCombat ;
extern WorldItem worldItem ;
extern CCharStuff worldNPC ;
extern CSkills worldSkill ;
extern CMagic worldMagic ;
extern cRaces worldRace ;
extern CJSMapping worldJSMapping ;
extern cEffects worldEffect ;
extern cHTMLTemplates worldHTMLTemplate;
extern CGuildCollection worldGuildSystem ;
extern CSpeechQueue worldSpeechSystem ;
extern CJSEngine worldJSEngine ;
extern CServerDefinitions worldFileLookup ;
extern CCommands serverCommands;
extern CMulHandler worldMULHandler ;
extern CNetworkStuff worldNetwork ;
extern CMapHandler worldMapHandler ;

using namespace std::string_literals;

void loadTeleportLocations();
void loadSpawnRegions();
void loadRegions();
void UnloadRegions();
void UnloadSpawnRegions();
void loadSkills();
void ScriptError(JSContext *cx, const char *txt, ...);

#define __EXTREMELY_VERBOSE__

std::map<std::string, CBaseObject::type_t> stringToObjType;

void InitStringToObjType() {
    stringToObjType["BASEOBJ"] = CBaseObject::OT_CBO;
    stringToObjType["CHARACTER"] = CBaseObject::OT_CHAR;
    stringToObjType["ITEM"] = CBaseObject::OT_ITEM;
    stringToObjType["SPAWNER"] = CBaseObject::OT_SPAWNER;
    stringToObjType["MULTI"] = CBaseObject::OT_MULTI;
    stringToObjType["BOAT"] = CBaseObject::OT_BOAT;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	FindObjTypeFromString()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine object type (ITEM, CHARACTER, MULTI, etc) based on
// provided string
// o------------------------------------------------------------------------------------------------o
CBaseObject::type_t FindObjTypeFromString(std::string strToFind) {
    if (stringToObjType.empty()) {// if we haven't built our array yet
        InitStringToObjType();
    }
    std::map<std::string, CBaseObject::type_t>::const_iterator toFind =
    stringToObjType.find(util::upper(strToFind));
    if (toFind != stringToObjType.end()) {
        return toFind->second;
    }
    return CBaseObject::OT_CBO;
}

// Effect related functions
// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoTempEffect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Does a temporary effect (things like protection, night sight, and
// what not) frm |					src to trg. If iType = 0, then it's a
// character, otherwise it's an item.
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoTempEffect(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc < 7) {
        ScriptError(cx, "DoTempEffect: Invalid number of arguments (takes 7 or 8)");
        return JS_FALSE;
    }
    std::uint8_t iType = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0]));
    std::uint32_t targNum = JSVAL_TO_INT(argv[3]);
    std::uint8_t more1 = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    std::uint8_t more2 = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[5]));
    std::uint8_t more3 = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[6]));
    
    CItem *myItemPtr = nullptr;
    
    if (argc == 8) {
        JSObject *myitemptr = nullptr;
        myitemptr = JSVAL_TO_OBJECT(argv[7]);
        myItemPtr = static_cast<CItem *>(JS_GetPrivate(cx, myitemptr));
    }
    
    JSObject *mysrc = JSVAL_TO_OBJECT(argv[1]);
    CChar *mysrcChar = nullptr;
    
    // Check if mysrc is null before continuing - it could be this temp effect as no character-based
    // source!
    if (mysrc != nullptr) {
        mysrcChar = static_cast<CChar *>(JS_GetPrivate(cx, mysrc));
        if (!ValidateObject(mysrcChar)) {
            ScriptError(cx, "DoTempEffect: Invalid src");
            return JS_FALSE;
        }
    }
    
    if (iType == 0){ // character
        JSObject *mydestc = JSVAL_TO_OBJECT(argv[2]);
        CChar *mydestChar = static_cast<CChar *>(JS_GetPrivate(cx, mydestc));
        
        if (!ValidateObject(mydestChar)) {
            ScriptError(cx, "DoTempEffect: Invalid target ");
            return JS_FALSE;
        }
        if (argc == 8) {
            worldEffect.TempEffect(mysrcChar, mydestChar, static_cast<std::int8_t>(targNum), more1, more2, more3, myItemPtr);
        }
        else {
            worldEffect.TempEffect(mysrcChar, mydestChar, static_cast<std::int8_t>(targNum), more1, more2, more3);
        }
    }
    else {
        JSObject *mydesti = JSVAL_TO_OBJECT(argv[2]);
        CItem *mydestItem = static_cast<CItem *>(JS_GetPrivate(cx, mydesti));
        
        if (!ValidateObject(mydestItem)) {
            ScriptError(cx, "DoTempEffect: Invalid target ");
            return JS_FALSE;
        }
        worldEffect.TempEffect(mysrcChar, mydestItem, static_cast<std::int8_t>(targNum), more1, more2, more3);
    }
    return JS_TRUE;
}

// Speech related functions
void sysBroadcast(const std::string &txt);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_BroadcastMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Broadcasts specified string to all online players
// o------------------------------------------------------------------------------------------------o
JSBool SE_BroadcastMessage(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "BroadcastMessage: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string trgMessage = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    if (trgMessage.empty() || trgMessage.length() == 0) {
        ScriptError(cx, util::format("BroadcastMessage: Invalid string (%s)", trgMessage.c_str()).c_str());
        return JS_FALSE;
    }
    sysBroadcast(trgMessage);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcItemFromSer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns item object based on provided serial
// o------------------------------------------------------------------------------------------------o
JSBool SE_CalcItemFromSer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "CalcItemFromSer: Invalid number of arguments (takes 1 or 4)");
        return JS_FALSE;
    }
    serial_t targSerial;
    if (argc == 1) {
        std::string str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
        targSerial = util::ston<serial_t>(str);
    }
    else {
        targSerial = CalcSerial(static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3])));
    }
    
    CItem *newItem = CalcItemObjFromSer(targSerial);
    if (newItem != nullptr) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, newItem, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcMultiFromSer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns item object based on provided serial
// o------------------------------------------------------------------------------------------------o
JSBool SE_CalcMultiFromSer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "CalcMultiFromSer: Invalid number of arguments (takes 1 or 4)");
        return JS_FALSE;
    }
    serial_t targSerial;
    if (argc == 1) {
        targSerial = static_cast<serial_t>(JSVAL_TO_INT(argv[0]));
    }
    else {
        targSerial = CalcSerial(static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3])));
    }
    
    CItem *newMulti = CalcMultiFromSer(targSerial);
    if (newMulti != nullptr) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, newMulti, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcCharFromSer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns character object based on provided serial
// o------------------------------------------------------------------------------------------------o
JSBool SE_CalcCharFromSer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "CalcCharFromSer: Invalid number of arguments (takes 1 or 4)");
        return JS_FALSE;
    }
    auto targSerial = INVALIDSERIAL;
    if (argc == 1) {
        targSerial = static_cast<serial_t>(JSVAL_TO_INT(argv[0]));
    }
    else {
        targSerial = CalcSerial( static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3])));
    }
    
    CChar *newChar = CalcCharObjFromSer(targSerial);
    if (newChar != nullptr) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_CHAR, newChar,worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoMovingEffect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a moving effect from source object to target object or
// location
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoMovingEffect(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc < 6) {
        ScriptError(cx,"DoMovingEffect: Invalid number of arguments (takes 6->8 or 8->10, or 10->12)");
        return JS_FALSE;
    }
    
    CBaseObject *src = nullptr;
    CBaseObject *trg = nullptr;
    bool srcLocation = false;
    bool targLocation = false;
    std::uint16_t srcX = 0;
    std::uint16_t srcY = 0;
    std::int8_t srcZ = 0;
    std::uint16_t targX = 0;
    std::uint16_t targY = 0;
    std::int8_t targZ = 0;
    std::uint16_t effect = 0;
    std::uint8_t speed = 0;
    std::uint8_t loop = 0;
    bool explode = 0;
    std::uint32_t hue = 0;
    std::uint32_t renderMode = 0;
    
    if (JSVAL_IS_INT(argv[0])) {
        // 10, 11 or 12 arguments
        // srcX, srcY, srcZ, targX, targY, targZ, effect, speed, loop, explode, [hue], [renderMode]
        srcLocation = true;
        targLocation = true;
        targLocation = true;
        srcX = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
        srcY = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
        srcZ = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
        targX = static_cast<std::int16_t>(JSVAL_TO_INT(argv[3]));
        targY = static_cast<std::int16_t>(JSVAL_TO_INT(argv[4]));
        targZ = static_cast<std::int8_t>(JSVAL_TO_INT(argv[5]));
        effect = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[6]));
        speed = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[7]));
        loop = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[8]));
        explode = (JSVAL_TO_BOOLEAN(argv[9]) == JS_TRUE);
        if (argc >= 11) {
            hue = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[10]));
        }
        if (argc >= 12) {
            renderMode = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[11]));
        }
    }
    else {
        JSObject *srcObj = JSVAL_TO_OBJECT(argv[0]);
        src = static_cast<CBaseObject *>(JS_GetPrivate(cx, srcObj));
        if (!ValidateObject(src)) {
            ScriptError(cx, "DoMovingEffect: Invalid source object");
            return JS_FALSE;
        }
        
        if (JSVAL_IS_INT(argv[1])) {
            // 8, 9 or 10 arguments
            // srcObj, targX, targY, targZ, effect, speed, loop, explode, [hue], [renderMode]
            targLocation = true;
            targX = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
            targY = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[2]));
            targZ = static_cast<std::int8_t>(JSVAL_TO_INT(argv[3]));
            effect = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
            speed = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[5]));
            loop = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[6]));
            explode = (JSVAL_TO_BOOLEAN(argv[7]) == JS_TRUE);
            if (argc >= 9) {
                hue = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[8]));
            }
            if (argc >= 10) {
                renderMode = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[9]));
            }
        }
        else {
            // 6, 7 or 8 arguments
            // srcObj, targObj, effect, speed, loop, explode, [hue], [renderMode]
            if (!JSVAL_IS_OBJECT(argv[1])) {
                ScriptError(cx, "DoMovingEffect: Invalid target object");
                return JS_FALSE;
            }
            
            JSObject *trgObj = JSVAL_TO_OBJECT(argv[1]);
            trg = static_cast<CBaseObject *>(JS_GetPrivate(cx, trgObj));
            if (!ValidateObject(trg)) {
                ScriptError(cx, "DoMovingEffect: Invalid target object");
                return JS_FALSE;
            }
            
            effect = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[2]));
            speed = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
            loop = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
            explode = (JSVAL_TO_BOOLEAN(argv[5]) == JS_TRUE);
            if (argc >= 7) {
                hue = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[6]));
            }
            if (argc >= 8) {
                renderMode = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[7]));
            }
        }
    }
    
    if (srcLocation && targLocation) {
        worldEffect.PlayMovingAnimation(srcX, srcY, srcZ, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode);
    }
    else if (!srcLocation && targLocation) {
        worldEffect.PlayMovingAnimation(src, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode);
    }
    else {
        worldEffect.PlayMovingAnimation(src, trg, effect, speed, loop, explode, hue, renderMode);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoStaticEffect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a static effect at a target location
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoStaticEffect([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 7) {
        ScriptError(cx, "StaticEffect: Invalid number of arguments (takes 7 - targX, targY, targZ, effectID, speed, loop, explode)");
        return JS_FALSE;
    }
    
    std::int16_t targX = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t targY = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int16_t targZ = static_cast<std::int16_t>(JSVAL_TO_INT(argv[2]));
    std::uint16_t effectId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t speed = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    std::uint8_t loop = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[5]));
    bool explode = (JSVAL_TO_BOOLEAN(argv[6]) == JS_TRUE);
    
    worldEffect.PlayStaticAnimation(targX, targY, targZ, effectId, speed, loop, explode);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RandomNumber()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random number between loNum and hiNum
// o------------------------------------------------------------------------------------------------o
JSBool SE_RandomNumber(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                       jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RandomNumber: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    JSEncapsulate loVal(cx, &(argv[0]));
    JSEncapsulate hiVal(cx, &(argv[1]));
    *rval = INT_TO_JSVAL(RandomNum(loVal.toInt(), hiVal.toInt()));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_MakeItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Character creates specified item based on entry in CREATE DFNs
// o------------------------------------------------------------------------------------------------o
JSBool SE_MakeItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 3 && argc != 4) {
        ScriptError(cx, "MakeItem: Invalid number of arguments (takes 3, or 4 - socket, character, createID - and optionally - resourceColour)");
        return JS_FALSE;
    }
    JSObject *mSock = JSVAL_TO_OBJECT(argv[0]);
    CSocket *sock = static_cast<CSocket *>(JS_GetPrivate(cx, mSock));
    JSObject *mChar = JSVAL_TO_OBJECT(argv[1]);
    CChar *player = static_cast<CChar *>(JS_GetPrivate(cx, mChar));
    if (!ValidateObject(player)) {
        ScriptError(cx, "MakeItem: Invalid character");
        return JS_FALSE;
    }
    std::uint16_t itemMenu = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[2]));
    CreateEntry_st *toFind = worldSkill.FindItem(itemMenu);
    if (toFind == nullptr) {
        ScriptError(cx, util::format("MakeItem: Invalid make item (%i)", itemMenu).c_str());
        return JS_FALSE;
    }
    std::uint16_t resourceColour = 0;
    if (argc == 4) {
        resourceColour = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
    }
    
    worldSkill.MakeItem(*toFind, player, sock, itemMenu, resourceColour);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CommandLevelReq()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the command level required to execute the specified command
// o------------------------------------------------------------------------------------------------o
JSBool SE_CommandLevelReq(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "CommandLevelReq: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string test = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    if (test.empty() || test.length() == 0) {
        ScriptError(cx, "CommandLevelReq: Invalid command name");
        return JS_FALSE;
    }
    CommandMapEntry *details = serverCommands.commandDetails(test);
    if (details == nullptr) {
        *rval = INT_TO_JSVAL(255);
    }
    else {
        *rval = INT_TO_JSVAL(details->cmdLevelReq);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CommandExists()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specified command exists in command table or not
// o------------------------------------------------------------------------------------------------o
JSBool SE_CommandExists(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                        jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "CommandExists: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string test = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    if (test.empty() || test.length() == 0) {
        ScriptError(cx, "CommandExists: Invalid command name");
        return JS_FALSE;
    }
    *rval = BOOLEAN_TO_JSVAL(serverCommands.commandExists(test));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FirstCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the first command in the table. If nothing, it's
// a 0 length string.
// o------------------------------------------------------------------------------------------------o
JSBool SE_FirstCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    const std::string tVal = serverCommands.firstCommand();
    JSString *strSpeech = nullptr;
    if (tVal.empty()) {
        strSpeech = JS_NewStringCopyZ(cx, "");
    }
    else {
        strSpeech = JS_NewStringCopyZ(cx, tVal.c_str());
    }
    
    *rval = STRING_TO_JSVAL(strSpeech);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_NextCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the next command in the table. If nothing, it's
// a 0 length string.
// o------------------------------------------------------------------------------------------------o
JSBool SE_NextCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    const std::string tVal = serverCommands.nextCommand();
    JSString *strSpeech = nullptr;
    if (tVal.empty()) {
        strSpeech = JS_NewStringCopyZ(cx, "");
    }
    else {
        strSpeech = JS_NewStringCopyZ(cx, tVal.c_str());
    }
    
    *rval = STRING_TO_JSVAL(strSpeech);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FinishedCommandList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if there are no more commands left in the table.
// o------------------------------------------------------------------------------------------------o
JSBool SE_FinishedCommandList([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv,  jsval *rval) {
    *rval = BOOLEAN_TO_JSVAL(serverCommands.finishedCommandList());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If called from within CommandRegistration() function in a script
// registered
//|					under the COMMAND_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP,
//will register |					the specified command in the command table
//and call the function in the same
//|					script whose name corresponds with the command name, in the
//shape of |						function command_CMDNAME( socket, cmdString
// )
// o------------------------------------------------------------------------------------------------o
JSBool SE_RegisterCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 3) {
        ScriptError(cx, "  RegisterCommand: Invalid number of arguments (takes 4)");
        return JS_FALSE;
    }
    std::string toRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    std::uint8_t execLevel = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    bool isEnabled = (JSVAL_TO_BOOLEAN(argv[2]) == JS_TRUE);
    std::uint16_t scriptId = worldJSMapping.GetScriptId(JS_GetGlobalObject(cx));
    
    if (scriptId == 0xFFFF) {
        ScriptError(cx, " RegisterCommand: JS Script has an Invalid ScriptID");
        return JS_FALSE;
    }
    
    serverCommands.registerCommand(toRegister, scriptId, execLevel, isEnabled);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterSpell()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If called from within SpellRegistration() function in a script
// registered under
//|					the MAGIC_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP, will
// register the
//|					onSpellCast() event in the same script as a global listener for
//use of the |					specified magic spell.
// o------------------------------------------------------------------------------------------------o
JSBool SE_RegisterSpell(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterSpell: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::int32_t spellNumber = JSVAL_TO_INT(argv[0]);
    bool isEnabled = (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE);
    cScript *myScript = worldJSMapping.GetScript(JS_GetGlobalObject(cx));
    worldMagic.registerSpell(myScript, spellNumber, isEnabled);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterSkill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for use of specified skill,
// and
//|					triggers onSkill() event in same script when specified skill
//is used, if |					script is added under the SKILLUSE_SCRIPTS section
//of JSE_FILEASSOCIATIONS.SCP |					and this function is called from a
// SkillRegistration() function
// o------------------------------------------------------------------------------------------------o
JSBool SE_RegisterSkill(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterSkill: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::int32_t skillNumber = JSVAL_TO_INT(argv[0]);
    bool isEnabled = (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE);
    std::uint16_t scriptId = worldJSMapping.GetScriptId(JS_GetGlobalObject(cx));
    if (scriptId != 0xFFFF) {
#if defined(UOX_DEBUG_MODE)
        Console::shared().print(" ");
        Console::shared().moveTo(15);
        Console::shared().print("Registering skill number ");
        Console::shared().turnYellow();
        Console::shared().print(util::format("%i", skillNumber));
        if (!isEnabled) {
            Console::shared().turnRed();
            Console::shared().print(" [DISABLED]");
        }
        Console::shared().print("\n");
        Console::shared().turnNormal();
#endif
        // If skill is not enabled, unset scriptId from skill data
        if (!isEnabled) {
            worldMain.skill[skillNumber].jsScript = 0;
            return JS_FALSE;
        }
        
        // Skillnumber above ALLSKILLS refers to STR, INT, DEX, Fame and Karma,
        if (skillNumber < 0 || skillNumber >= ALLSKILLS)
            return JS_TRUE;
        
        // Both scriptId and skillNumber seem valid; assign scriptId to this skill
        worldMain.skill[skillNumber].jsScript = scriptId;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterPacket()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for a specific network
// packet, and
//|					triggers onPacketReceive() event in same script when this
//network packet is sent,
//|					if script is added under the PACKET_SCRIPTS section of
// JSE_FILEASSOCIATIONS.SCP |					and this function is called from a
// PacketRegistration() function
// o------------------------------------------------------------------------------------------------o
JSBool SE_RegisterPacket(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterPacket: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::uint8_t packet = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0]));
    std::uint8_t subCmd = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    std::uint16_t scriptId = worldJSMapping.GetScriptId(JS_GetGlobalObject(cx));
    if (scriptId != 0xFFFF) {
#if defined(UOX_DEBUG_MODE)
        Console::shared().print(util::format("Registering packet number 0x%X, subcommand 0x%x\n", packet, subCmd));
#endif
        worldNetwork.RegisterPacket(packet, subCmd, scriptId);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterKey()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for a specific keypress in
// UOX3 console, |					and triggers specified function in same
// script when key is pressed, if script |					is added under the
// CONSOLE_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a ConsoleRegistration()
// function
// o------------------------------------------------------------------------------------------------o
JSBool SE_RegisterKey(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,[[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterKey: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    JSEncapsulate encaps(cx, &(argv[0]));
    std::string toRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    std::uint16_t scriptId = worldJSMapping.GetScriptId(JS_GetGlobalObject(cx));
    
    if (scriptId == 0xFFFF) {
        ScriptError(cx, "RegisterKey: JS Script has an Invalid ScriptID");
        return JS_FALSE;
    }
    std::int32_t toPass = 0;
    if (encaps.isType(JSEncapsulate::JSOT_STRING)) {
        std::string enStr = encaps.toString();
        if (enStr.length() != 0) {
            toPass = enStr[0];
        }
        else {
            ScriptError(cx, "RegisterKey: JS Script passed an invalid key to register");
            return JS_FALSE;
        }
    }
    else {
        toPass = encaps.toInt();
    }
    Console::shared().registerKey(toPass, toRegister, scriptId);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterConsoleFunc()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
// o------------------------------------------------------------------------------------------------o
JSBool SE_RegisterConsoleFunc(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterConsoleFunc: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::string funcToRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    std::string toRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    std::uint16_t scriptId = worldJSMapping.GetScriptId(JS_GetGlobalObject(cx));
    
    if (scriptId == 0xFFFF) {
        ScriptError(cx, "RegisterConsoleFunc: JS Script has an Invalid ScriptID");
        return JS_FALSE;
    }
    
    Console::shared().registerFunc(funcToRegister, toRegister, scriptId);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables a specified command on the server
// o------------------------------------------------------------------------------------------------o
JSBool SE_DisableCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "DisableCommand: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string toDisable = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    serverCommands.setCommandStatus(toDisable, false);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableKey()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables specified key in console
// o------------------------------------------------------------------------------------------------o
JSBool SE_DisableKey([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "DisableKey: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::int32_t toDisable = JSVAL_TO_INT(argv[0]);
    Console::shared().setKeyStatus(toDisable, false);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableConsoleFunc()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
// o------------------------------------------------------------------------------------------------o
JSBool SE_DisableConsoleFunc(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "DisableConsoleFunc: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string toDisable = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    Console::shared().setFuncStatus(toDisable, false);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableSpell()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables specified spell on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_DisableSpell([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "DisableSpell: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::int32_t spellNumber = JSVAL_TO_INT(argv[0]);
    worldMagic.SetSpellStatus(spellNumber, false);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified command on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_EnableCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "EnableCommand: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string toEnable = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    serverCommands.setCommandStatus(toEnable, true);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableSpell()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified spell on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_EnableSpell([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "EnableSpell: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::int32_t spellNumber = JSVAL_TO_INT(argv[0]);
    worldMagic.SetSpellStatus(spellNumber, true);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableKey()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified key in console
// o------------------------------------------------------------------------------------------------o
JSBool SE_EnableKey([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "EnableKey: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::int32_t toEnable = JSVAL_TO_INT(argv[0]);
    Console::shared().setKeyStatus(toEnable, true);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableConsoleFunc()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
// o------------------------------------------------------------------------------------------------o
JSBool SE_EnableConsoleFunc(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "EnableConsoleFunc: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string toEnable = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    Console::shared().setFuncStatus(toEnable, false);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetHour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the hour of the current UO day
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetHour([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    bool ampm =    worldMain.uoTime.ampm;
    std::uint8_t hour = worldMain.uoTime.hours ;
    if (ampm) {
        *rval = INT_TO_JSVAL(static_cast<std::uint64_t>(hour) + 12);
    }
    else {
        *rval = INT_TO_JSVAL(hour);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMinute()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the minute of the current UO day
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetMinute([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    std::uint8_t minute = worldMain.uoTime.minutes ;
    *rval = INT_TO_JSVAL(minute);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetDay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the day number of the server (UO days since server start)
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetDay([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    std::int16_t day = worldMain.uoTime.days ;
    *rval = INT_TO_JSVAL(day);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SecondsPerUOMinute()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets the amonut of real life seconds associated with minute
// in the game
// o------------------------------------------------------------------------------------------------o
JSBool SE_SecondsPerUOMinute([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx, "SecondsPerUOMinute: Invalid number of arguments (takes 0 or 1)");
        return JS_FALSE;
    }
    else if (argc == 1) {
        std::uint16_t secondsPerUOMinute = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
        ServerConfig::shared().ushortValues[UShortValue::SECONDSPERUOMINUTE] = secondsPerUOMinute ;
    }
    *rval = INT_TO_JSVAL(ServerConfig::shared().ushortValues[UShortValue::SECONDSPERUOMINUTE]);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetCurrentClock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets timestamp for current server clock
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetCurrentClock(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, worldMain.GetUICurrentTime(), rval);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetStartTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets timestamp for server startup time
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetStartTime(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, worldMain.GetStartTime(), rval);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRandomSOSArea()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a random SOS area from list of such areas loaded from
//[SOSAREAS] section of regions.dfn
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetRandomSOSArea(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "GetRandomSOSArea: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    
    std::uint8_t worldNum = 0;
    std::uint16_t instanceId = 0;
    if (JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1])) {
        worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0]));
        instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    }
    else {
        ScriptError(cx, "GetRandomSOSArea: Invalid values passed in for worldNum and instanceId - must be integers!");
        return JS_FALSE;
    }
    
    // Fetch vector of SOS locations
    auto sosLocs = worldMain.sosLocs;
    if (sosLocs.size() == 0) {
        ScriptError(cx, "GetRandomSOSArea: No valid SOS areas found. Is the [SOSAREAS] section of regions.dfn setup correctly?");
        return JS_FALSE;
    }
    
    // Prepare a vector to hold the areas with correct worldNum and instanceId
    std::vector<SOSLocationEntry> validSOSLocs;
    
    // Loop through all SOS areas and cherry-pick the ones with correct worldNum and instanceId
    for (size_t i = 0; i < sosLocs.size(); ++i) {
        if (sosLocs[i].worldNum == worldNum && sosLocs[i].instanceId == instanceId) {
            validSOSLocs.push_back(sosLocs[i]);
        }
    }
    
    // Choose a random SOS area from the generated list of such areas
    auto rndSosLoc = validSOSLocs[RandomNum(static_cast<size_t>(0), validSOSLocs.size() - 1)];
    
    // Convert properties of chosen SOS area to jsvals, so we can pass them to a JSObject
    jsval jsX1 = INT_TO_JSVAL(rndSosLoc.x1);
    jsval jsY1 = INT_TO_JSVAL(rndSosLoc.y1);
    jsval jsX2 = INT_TO_JSVAL(rndSosLoc.x2);
    jsval jsY2 = INT_TO_JSVAL(rndSosLoc.y2);
    jsval jsWorldNum = INT_TO_JSVAL(rndSosLoc.worldNum);
    jsval jsInstanceId = INT_TO_JSVAL(rndSosLoc.instanceId);
    
    // Construct a JS Object with the properties of the chosen SOS area
    JSObject *rndSosLocObj = JS_NewArrayObject(cx, 0, nullptr);
    JS_SetElement(cx, rndSosLocObj, 0, &jsX1);
    JS_SetElement(cx, rndSosLocObj, 1, &jsY1);
    JS_SetElement(cx, rndSosLocObj, 2, &jsX2);
    JS_SetElement(cx, rndSosLocObj, 3, &jsY2);
    JS_SetElement(cx, rndSosLocObj, 4, &jsWorldNum);
    JS_SetElement(cx, rndSosLocObj, 5, &jsInstanceId);
    
    // Pass the JS object to script
    *rval = OBJECT_TO_JSVAL(rndSosLocObj);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SpawnNPC()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns NPC based on definition in NPC DFNs (or an NPCLIST)
// o------------------------------------------------------------------------------------------------o
JSBool SE_SpawnNPC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 5 || argc > 7) {
        ScriptError(cx, "SpawnNPC: Invalid number of arguments (takes 5, 6 or 7)");
        return JS_FALSE;
    }
    
    CChar *cMade = nullptr;
    std::string nnpcNum = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    std::uint16_t x = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    std::uint16_t y = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[2]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t world = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    std::uint16_t instanceId = (argc == 6 ? static_cast<std::int16_t>(JSVAL_TO_INT(argv[5])) : 0);
    bool useNpcList = (argc == 7 ? (JSVAL_TO_BOOLEAN(argv[6]) == JS_TRUE) : false);
    
    // Store original script context and object, in case NPC spawned has some event that triggers on
    // spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    cMade = worldNPC.CreateNPCxyz(nnpcNum, x, y, z, world, instanceId, useNpcList);
    if (cMade != nullptr) {
        JSObject *myobj = worldJSEngine.AcquireObject(IUE_CHAR, cMade, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myobj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateDFNItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates item based on definition in item DFNs
// o------------------------------------------------------------------------------------------------o
JSBool SE_CreateDFNItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 3) {
        ScriptError(cx, "CreateDFNItem: Invalid number of arguments (takes at least 3, max 8)");
        return JS_FALSE;
    }
    
    CSocket *mySock = nullptr;
    if (argv[0] != JSVAL_NULL) {
        JSObject *mSock = JSVAL_TO_OBJECT(argv[0]);
        mySock = static_cast<CSocket *>(JS_GetPrivate(cx, mSock));
    }
    
    CChar *myChar = nullptr;
    if (argv[1] != JSVAL_NULL) {
        JSObject *mChar = JSVAL_TO_OBJECT(argv[1]);
        myChar = static_cast<CChar *>(JS_GetPrivate(cx, mChar));
    }
    
    std::string bpSectNumber = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
    bool bInPack = true;
    std::uint16_t iAmount = 1;
    auto itemType = CBaseObject::OT_ITEM;
    std::uint16_t iColor = 0xFFFF;
    std::uint8_t worldNumber = 0;
    std::uint16_t instanceId = 0;
    
    if (argc > 3) {
        iAmount = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
    }
    if (argc > 4) {
        std::string objType = JS_GetStringBytes(JS_ValueToString(cx, argv[4]));
        itemType = FindObjTypeFromString(objType);
    }
    if (argc > 5) {
        bInPack = (JSVAL_TO_BOOLEAN(argv[5]) == JS_TRUE);
    }
    if (argc > 6) {
        iColor = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[6]));
    }
    if (argc > 7) {
        worldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[7]));
    }
    if (argc > 8) {
        instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[8]));
    }
    
    // Store original script context and object, in case Item spawned has some event that triggers
    // on spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    CItem *newItem = nullptr;
    if (myChar != nullptr) {
        newItem = worldItem.CreateScriptItem(mySock, myChar, bpSectNumber, iAmount, itemType, bInPack, iColor);
    }
    else {
        newItem = worldItem.CreateBaseScriptItem(nullptr, bpSectNumber, worldNumber, iAmount, instanceId, itemType, iColor);
    }
    
    if (newItem != nullptr) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, newItem, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateBlankItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a "blank" item with default values from client's tiledata
//|	Notes		-	Default values can be overridden through harditems.dfn
// o------------------------------------------------------------------------------------------------o
JSBool SE_CreateBlankItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 8) {
        ScriptError(cx, "CreateBlankItem: Invalid number of arguments (takes 7)");
        return JS_FALSE;
    }
    CItem *newItem = nullptr;
    CSocket *mySock = nullptr;
    if (argv[0] != JSVAL_NULL) {
        JSObject *mSock = JSVAL_TO_OBJECT(argv[0]);
        mySock = static_cast<CSocket *>(JS_GetPrivate(cx, mSock));
    }
    
    CChar *myChar = nullptr;
    if (argv[1] != JSVAL_NULL) {
        JSObject *mChar = JSVAL_TO_OBJECT(argv[1]);
        myChar = static_cast<CChar *>(JS_GetPrivate(cx, mChar));
    }
    
    std::int32_t amount = static_cast<std::int32_t>(JSVAL_TO_INT(argv[2]));
    std::string itemName = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
    std::uint16_t itemId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
    std::uint16_t colour = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[5]));
    std::string objType = JS_GetStringBytes(JS_ValueToString(cx, argv[6]));
    auto itemType = FindObjTypeFromString(objType);
    bool inPack = (JSVAL_TO_BOOLEAN(argv[7]) == JS_TRUE);
    
    // Store original script context and object, in case NPC spawned has some event that triggers on
    // spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    newItem = worldItem.CreateItem(mySock, myChar, itemId, amount, colour, itemType, inPack);
    if (newItem != nullptr) {
        if (itemName != "") {
            newItem->SetName(itemName);
        }
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, newItem, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    
    return JS_TRUE;
}

CMultiObj *BuildHouse(CSocket *s, std::uint16_t houseEntry, bool checkLocation = true, std::int16_t xLoc = -1, std::int16_t yLoc = -1, std::int8_t zLoc = -1, std::uint8_t worldNumber = 0, std::uint16_t instanceId = 0);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateHouse()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates multi based on definition in house DFNs
// o------------------------------------------------------------------------------------------------o
JSBool SE_CreateHouse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 4) {
        ScriptError(cx, "CreateHouse: Invalid number of arguments (takes at least 4, max 8)");
        return JS_FALSE;
    }
    
    std::uint16_t houseId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t xLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int16_t yLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[2]));
    std::int8_t zLoc = static_cast<std::int8_t>(JSVAL_TO_INT(argv[3]));
    std::uint16_t iColor = 0xFFFF;
    std::uint8_t worldNumber = 0;
    std::uint16_t instanceId = 0;
    bool checkLocation = true;
    
    if (argc > 4) {
        worldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    }
    if (argc > 5) {
        instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[5]));
    }
    if (argc > 6) {
        iColor = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[6]));
    }
    if (argc > 7) {
        checkLocation = (JSVAL_TO_BOOLEAN(argv[7]) == JS_TRUE);
    }
    
    // Store original script context and object, in case Item spawned has some event that triggers
    // on spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    CMultiObj *newMulti =
    BuildHouse(nullptr, houseId, checkLocation, xLoc, yLoc, zLoc, worldNumber, instanceId);
    if (newMulti != nullptr) {
        // Apply color to house, if it was provided
        if (iColor != 0xFFFF) {
            newMulti->SetColour(iColor);
        }
        
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, newMulti, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    return JS_TRUE;
}

CMultiObj *BuildBaseMulti(std::uint16_t multiId, std::int16_t xLoc = -1, std::int16_t yLoc = -1, std::int8_t zLoc = 127, std::uint8_t worldNumber = 0, std::uint16_t instanceId = 0);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateBaseMulti()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates base multi based on ID from multi file
// o------------------------------------------------------------------------------------------------o
JSBool SE_CreateBaseMulti(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 4) {
        ScriptError(cx, "CreateBaseMulti: Invalid number of arguments (takes at least 4, max 8)");
        return JS_FALSE;
    }
    
    std::uint16_t multiId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t xLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int16_t yLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[2]));
    std::int8_t zLoc = static_cast<std::int8_t>(JSVAL_TO_INT(argv[3]));
    std::uint16_t iColor = 0xFFFF;
    std::uint8_t worldNumber = 0;
    std::uint16_t instanceId = 0;
    bool checkLocation = true;
    
    
    if (argc > 4) {
        worldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    }
    if (argc > 5) {
        instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[5]));
    }
    if (argc > 6) {
        iColor = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[6]));
    }
    if (argc > 7) {
        checkLocation = (JSVAL_TO_BOOLEAN(argv[7]) == JS_TRUE);
    }
    
    // Store original script context and object, in case Item spawned has some event that triggers
    // on spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    CMultiObj *newMulti = BuildBaseMulti(multiId, xLoc, yLoc, zLoc, worldNumber, instanceId);
    if (newMulti != nullptr) {
        // Apply color to house, if it was provided
        if (iColor != 0xFFFF) {
            newMulti->SetColour(iColor);
        }
        
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, newMulti, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMurderThreshold()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the max amount of kills allowed before a player turns red
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetMurderThreshold([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,  [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(ServerConfig::shared().ushortValues[UShortValue::MAXKILL]);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RollDice()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Rolls a die with specified number of sides, and adds a fixed value
// o------------------------------------------------------------------------------------------------o
JSBool SE_RollDice([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 3) {
        ScriptError(cx, "RollDice: Invalid number of arguments (takes 3)");
        return JS_FALSE;
    }
    std::uint32_t numDice = JSVAL_TO_INT(argv[0]);
    std::uint32_t numFace = JSVAL_TO_INT(argv[1]);
    std::uint32_t numAdd = JSVAL_TO_INT(argv[2]);
    
    cDice toRoll(numDice, numFace, numAdd);
    
    *rval = INT_TO_JSVAL(toRoll.RollDice());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RaceCompareByRace()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compares the relations between two races
//|	Notes		-	0 - neutral
//|					1 to 100 - allies
//|					-1 to -100 - enemies
// o------------------------------------------------------------------------------------------------o
JSBool SE_RaceCompareByRace([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    auto r0 = static_cast<raceid_t>(JSVAL_TO_INT(argv[0]));
    auto r1 = static_cast<raceid_t>(JSVAL_TO_INT(argv[1]));
    *rval = INT_TO_JSVAL(worldRace.CompareByRace(r0, r1));
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindMulti()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns multi at given coordinates, world and instanceId
// o------------------------------------------------------------------------------------------------o
JSBool SE_FindMulti(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1 && argc != 4 && argc != 5) {
        ScriptError(cx, "FindMulti: Invalid number of parameters (1, 4 or 5)");
        return JS_FALSE;
    }
    std::int16_t xLoc = 0, yLoc = 0;
    std::int8_t zLoc = 0;
    std::uint8_t worldNumber = 0;
    std::uint16_t instanceId = 0;
    if (argc == 1) {
        JSObject *myitemptr = JSVAL_TO_OBJECT(argv[0]);
        CBaseObject *myItemPtr = static_cast<CBaseObject *>(JS_GetPrivate(cx, myitemptr));
        if (ValidateObject(myItemPtr)) {
            xLoc = myItemPtr->GetX();
            yLoc = myItemPtr->GetY();
            zLoc = myItemPtr->GetZ();
            worldNumber = myItemPtr->WorldNumber();
            instanceId = myItemPtr->GetInstanceId();
        }
        else {
            ScriptError(cx, "FindMulti: Invalid object type");
            return JS_FALSE;
        }
    }
    else {
        xLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
        yLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
        zLoc = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
        worldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
        if (argc == 5) {
            instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
        }
    }
    CMultiObj *multi = FindMulti(xLoc, yLoc, zLoc, worldNumber, instanceId);
    if (ValidateObject(multi)) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, multi, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetItem()
//|	Date		-	12 February, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns item closest to specified coordinates
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 4 && argc != 5) {
        ScriptError(cx, "GetItem: Invalid number of parameters (4 or 5)");
        return JS_FALSE;
    }
    std::int16_t xLoc = 0, yLoc = 0;
    std::int8_t zLoc = 0;
    std::uint8_t worldNumber = 0;
    std::uint16_t instanceId = 0;
    
    xLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    yLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    zLoc = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    worldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    if (argc == 5) {
        instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
    }
    
    CItem *item = GetItemAtXYZ(xLoc, yLoc, zLoc, worldNumber, instanceId);
    if (ValidateObject(item)) {
        JSObject *myObj =
        worldJSEngine.AcquireObject(IUE_ITEM, item, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindItem()
//|	Date		-	12 February, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns item of given ID that is closest to specified coordinates
// o------------------------------------------------------------------------------------------------o
JSBool SE_FindItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5 && argc != 6) {
        ScriptError(cx, "FindItem: Invalid number of parameters (5 or 6)");
        return JS_FALSE;
    }
    std::int16_t xLoc = 0, yLoc = 0;
    std::int8_t zLoc = 0;
    std::uint8_t worldNumber = 0;
    std::uint16_t id = 0;
    std::uint16_t instanceId = 0;
    
    xLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    yLoc = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    zLoc = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    worldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    id = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
    if (argc == 6) {
        instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[5]));
    }
    
    CItem *item = FindItemNearXYZ(xLoc, yLoc, zLoc, worldNumber, id, instanceId);
    if (ValidateObject(item)) {
        JSObject *myObj =
        worldJSEngine.AcquireObject(IUE_ITEM, item, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CompareGuildByGuild()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compares the relations between two guilds
//|	Notes		-	0 - Neutral
//|					1 - War
//|					2 - Ally
//|					3 - Unknown
//|					4 - Same
//|					5 - Count
// o------------------------------------------------------------------------------------------------o
JSBool SE_CompareGuildByGuild([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    guildid_t toCheck = static_cast<guildid_t>(JSVAL_TO_INT(argv[0]));
    guildid_t toCheck2 = static_cast<guildid_t>(JSVAL_TO_INT(argv[1]));
    *rval = INT_TO_JSVAL(worldGuildSystem.Compare(toCheck, toCheck2));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_PossessTown()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Source town takes control over target town
// o------------------------------------------------------------------------------------------------o
JSBool SE_PossessTown([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    std::uint16_t town = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::uint16_t sTown = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    worldMain.townRegions[town]->Possess(worldMain.townRegions[sTown]);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IsRaceWeakToWeather()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character's race is affected by given type of weather
// o------------------------------------------------------------------------------------------------o
JSBool SE_IsRaceWeakToWeather([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,  uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    auto race = static_cast<raceid_t>(JSVAL_TO_INT(argv[0]));
    auto toCheck = static_cast<weathid_t>(JSVAL_TO_INT(argv[1]));
    CRace *tRace = worldRace.Race(race);
    if (tRace == nullptr || toCheck >= Weather::numberweather) {
        return JS_FALSE;
    }
    *rval = BOOLEAN_TO_JSVAL(tRace->AffectedBy(static_cast<Weather::type_t>(toCheck)));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRaceSkillAdjustment()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage modifier for specified skill based on race
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetRaceSkillAdjustment([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    auto race = static_cast<raceid_t>(JSVAL_TO_INT(argv[0]));
    std::int32_t skill = JSVAL_TO_INT(argv[1]);
    *rval = INT_TO_JSVAL(worldRace.DamageFromSkill(skill, race));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_UseItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
// o------------------------------------------------------------------------------------------------o
JSBool SE_UseItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx,
                    "UseItem: Invalid number of arguments (takes 2 - socket/char, and item used)");
        return JS_FALSE;
    }
    
    CChar *mChar = nullptr;
    CSocket *mySocket = nullptr;
    
    JSEncapsulate myClass(cx, &(argv[0]));
    if (myClass.ClassName() == "UOXChar") {
        if (myClass.isType(JSEncapsulate::JSOT_OBJECT)) {
            mChar = static_cast<CChar *>(myClass.toObject());
            if (!ValidateObject(mChar)) {
                mChar = nullptr;
            }
        }
    }
    else if (myClass.ClassName() == "UOXSocket") {
        if (myClass.isType(JSEncapsulate::JSOT_OBJECT)) {
            mySocket = static_cast<CSocket *>(myClass.toObject());
            if (mySocket != nullptr) {
                mChar = mySocket->CurrcharObj();
            }
        }
    }
    
    JSObject *mItem = JSVAL_TO_OBJECT(argv[1]);
    CItem *myItem = static_cast<CItem *>(JS_GetPrivate(cx, mItem));
    
    if (!ValidateObject(myItem)) {
        ScriptError(cx, "UseItem: Invalid item");
        return JS_FALSE;
    }
    
    if (!ValidateObject(mChar)) {
        ScriptError(cx, "UseItem: Invalid character");
        return JS_FALSE;
    }
    
    // Store original script context and object, in case NPC spawned has some event that triggers on
    // spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    bool scriptExecuted = false;
    std::vector<std::uint16_t> scriptTriggers = myItem->GetScriptTriggers();
    for (auto i : scriptTriggers) {
        // Loop through all scriptIDs registered for item, check for scripts
        cScript *toExecute = worldJSMapping.GetScript(i);
        if (toExecute != nullptr) {
            // Script was found, let's check for OnUseUnChecked event
            std::int8_t retVal = toExecute->OnUseUnChecked(mChar, myItem);
            if (retVal != -1) {
                scriptExecuted = true;
                if (retVal == 0) {
                    // Script returned 0 - don't continue with other scripts
                    break;
                }
            }
            
            // No OnUseUnChecked event returned 0, check OnUseChecked event as well!
            // We don't actually do any checking in this case, as this should be
            // handled in script that calls UseItem
            retVal = toExecute->OnUseChecked(mChar, myItem);
            if (retVal != -1) {
                scriptExecuted = true;
                if (retVal == 0) {
                    // Script returned 0 - don't continue with other scripts
                    break;
                }
            }
        }
    }
    
    // Handle envoke stuff outside for loop, as we only want this to execute once
    if (scriptTriggers.size() == 0 || !scriptExecuted) {
        itemtypes_t iType = myItem->GetType();
        std::uint16_t itemId = myItem->GetId();
        std::uint16_t envTrig = 0;
        cScript *toExecute = nullptr;
        if (worldJSMapping.GetEnvokeByType()->Check(static_cast<std::uint16_t>(iType))) {
            envTrig = worldJSMapping.GetEnvokeByType()->GetScript(static_cast<std::uint16_t>(iType));
            toExecute = worldJSMapping.GetScript(envTrig);
        }
        else if (worldJSMapping.GetEnvokeById()->Check(itemId)) {
            envTrig = worldJSMapping.GetEnvokeById()->GetScript(itemId);
            toExecute = worldJSMapping.GetScript(envTrig);
        }
        
        // Check for the onUse events in envoke scripts!
        if (toExecute != nullptr) {
            if (toExecute->OnUseUnChecked(mChar, myItem) != 0) {
                // If onUseUnChecked wasn't found, or did not return false, try onUseChecked too
                toExecute->OnUseChecked(mChar, myItem);
            }
        }
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_TriggerTrap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
// o------------------------------------------------------------------------------------------------o
JSBool SE_TriggerTrap(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "UseItem: Invalid number of arguments (takes 2 - socket/char, and item (trap) triggered)");
        return JS_FALSE;
    }
    
    CChar *mChar = nullptr;
    CSocket *mySocket = nullptr;
    
    JSEncapsulate myClass(cx, &(argv[0]));
    if (myClass.ClassName() == "UOXChar") {
        if (myClass.isType(JSEncapsulate::JSOT_OBJECT)) {
            mChar = static_cast<CChar *>(myClass.toObject());
            if (!ValidateObject(mChar)) {
                mChar = nullptr;
            }
        }
    }
    else if (myClass.ClassName() == "UOXSocket") {
        if (myClass.isType(JSEncapsulate::JSOT_OBJECT)) {
            mySocket = static_cast<CSocket *>(myClass.toObject());
            if (mySocket != nullptr) {
                mChar = mySocket->CurrcharObj();
            }
        }
    }
    
    if (!ValidateObject(mChar)) {
        ScriptError(cx, "TriggerTrap: Invalid character");
        return JS_FALSE;
    }
    
    JSObject *mItem = JSVAL_TO_OBJECT(argv[1]);
    CItem *myItem = static_cast<CItem *>(JS_GetPrivate(cx, mItem));
    
    if (!ValidateObject(myItem)) {
        ScriptError(cx, "TriggerTrap: Invalid item");
        return JS_FALSE;
    }
    
    // Store original script context and object, in case NPC spawned has some event that triggers on
    // spawn and grabs context
    auto origContext = cx;
    auto origObject = obj;
    
    if (myItem->GetTempVar(CITV_MOREZ, 1) == 1 && myItem->GetTempVar(CITV_MOREZ, 2) > 0) { // Is trapped and set to deal more than 0 damage
        worldMagic.MagicTrap(mChar, myItem);
    }
    
    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_TriggerEvent()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers a an event/function in a different JS
//|	Notes		-	Takes at least 2 parameters, which is the script number to trigger
// and the
//|					function name to call. Any extra parameters are extra parameters
//to the JS event
// o------------------------------------------------------------------------------------------------o
JSBool SE_TriggerEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 2) {
        return JS_FALSE;
    }
    
    std::uint16_t scriptNumberToFire = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    char *eventToFire = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    cScript *toExecute = worldJSMapping.GetScript(scriptNumberToFire);
    
    if (toExecute == nullptr || eventToFire == nullptr)
        return JS_FALSE;
    
    auto origContext = cx;
    auto origObject = obj;
    
    JSBool retVal = toExecute->CallParticularEvent(eventToFire, &argv[2], argc - 2, rval);
    
    if (retVal) {
        JS_SetGlobalObject(origContext, origObject);
        return JS_TRUE;
    }
    else {
        JS_SetGlobalObject(origContext, origObject);
        return JS_FALSE;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesEventExist()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for the existence of a JS event in a different JS file
//|	Notes		-	Takes 2 parameters, which is the script number to check and the
//|					event name to check for
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesEventExist(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    
    *rval = INT_TO_JSVAL(1);
    std::uint16_t scriptNumberToCheck = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    char *eventToCheck = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    cScript *toExecute = worldJSMapping.GetScript(scriptNumberToCheck);
    
    if (toExecute == nullptr || eventToCheck == nullptr)
        return JS_FALSE;
    
    bool retVal = toExecute->DoesEventExist(eventToCheck);
    if (!retVal) {
        *rval = INT_TO_JSVAL(0);
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetPackOwner()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns owner of container item is contained in (if any)
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetPackOwner(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "GetPackOwner: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    
    std::uint8_t mType = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    CChar *pOwner = nullptr;
    
    if (mType == 0) {// item
        JSObject *mItem = JSVAL_TO_OBJECT(argv[0]);
        CItem *myItem = static_cast<CItem *>(JS_GetPrivate(cx, mItem));
        pOwner = FindItemOwner(myItem);
    }
    else {// serial
        std::int32_t mSerItem = JSVAL_TO_INT(argv[0]);
        pOwner = FindItemOwner(CalcItemObjFromSer(mSerItem));
    }
    if (ValidateObject(pOwner)) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_CHAR, pOwner, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindRootContainer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns root container an item is contained in (if any)
// o------------------------------------------------------------------------------------------------o
JSBool SE_FindRootContainer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "FindRootContainer: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    
    std::uint8_t mType = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    CItem *iRoot = nullptr;
    
    if (mType == 0) {// item
        JSObject *mItem = JSVAL_TO_OBJECT(argv[0]);
        CItem *myItem = static_cast<CItem *>(JS_GetPrivate(cx, mItem));
        iRoot = FindRootContainer(myItem);
    }
    else {// serial
        std::int32_t mSerItem = JSVAL_TO_INT(argv[0]);
        iRoot = FindRootContainer(CalcItemObjFromSer(mSerItem));
    }
    if (ValidateObject(iRoot)) {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, iRoot, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcTargetedItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted item stored on socket
// o------------------------------------------------------------------------------------------------o
JSBool SE_CalcTargetedItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "CalcTargetedItem: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    
    JSObject *mysockptr = JSVAL_TO_OBJECT(argv[0]);
    CSocket *sChar = static_cast<CSocket *>(JS_GetPrivate(cx, mysockptr));
    if (sChar == nullptr) {
        ScriptError(cx, "CalcTargetedItem: Invalid socket");
        return JS_FALSE;
    }
    
    CItem *calcedItem = CalcItemObjFromSer(sChar->GetDWord(7));
    if (!ValidateObject(calcedItem)) {
        *rval = JSVAL_NULL;
    }
    else {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_ITEM, calcedItem, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcTargetedChar()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted character stored on socket
// o------------------------------------------------------------------------------------------------o
JSBool SE_CalcTargetedChar(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,  jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "CalcTargetedChar: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    
    JSObject *mysockptr = JSVAL_TO_OBJECT(argv[0]);
    CSocket *sChar = static_cast<CSocket *>(JS_GetPrivate(cx, mysockptr));
    if (sChar == nullptr) {
        ScriptError(cx, "CalcTargetedChar: Invalid socket");
        return JS_FALSE;
    }
    
    CChar *calcedChar = CalcCharObjFromSer(sChar->GetDWord(7));
    if (!ValidateObject(calcedChar)) {
        *rval = JSVAL_NULL;
    }
    else {
        JSObject *myObj = worldJSEngine.AcquireObject(IUE_CHAR, calcedChar, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTileIdAtMapCoord()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the map tile ID at given coordinates
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetTileIdAtMapCoord([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,  uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3) {
        ScriptError(cx, "GetTileIDAtMapCoord: Invalid number of arguments (takes 3)");
        return JS_FALSE;
    }
    
    std::uint16_t xLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::uint16_t yLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t wrldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    auto mMap = worldMULHandler.SeekMap(xLoc, yLoc, wrldNumber);
    *rval = INT_TO_JSVAL(mMap.tileId);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StaticInRange()
//|	Date		-	17th August, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static within specified range of given location
// o------------------------------------------------------------------------------------------------o
JSBool SE_StaticInRange([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "StaticInRange: Invalid number of arguments (takes 5, x, y, world, radius, tile)");
        return JS_FALSE;
    }
    
    std::uint16_t xLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::uint16_t yLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t wrldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    std::uint16_t radius = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
    std::uint16_t tileId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
    bool tileFound = false;
    
    for (std::int32_t i = xLoc - radius; i <= (xLoc + radius); ++i) {
        for (std::int32_t j = yLoc - radius; j <= (yLoc + radius); ++j) {
            auto artwork = worldMULHandler.ArtAt(xLoc, yLoc, wrldNumber);
            auto iter = std::find_if(artwork.begin(), artwork.end(), [tileId](const Tile_st &tile) {
                return tile.tileId == tileId;
            });
            if (iter != artwork.end()) {
                tileFound = true;
                break;
            }
        }
        if (tileFound == true) {
            break;
        }
    }
    
    *rval = BOOLEAN_TO_JSVAL(tileFound);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StaticAt()
//|	Date		-	17th August, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static at specified location
//|	Notes		-	tile argument is optional; if not specified, will match ANY static
// found at location
// o------------------------------------------------------------------------------------------------o
JSBool SE_StaticAt([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 4 && argc != 3) {
        ScriptError(cx, "StaticAt: Invalid number of arguments (takes 4, x, y, world, tile)");
        return JS_FALSE;
    }
    
    std::uint16_t xLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::uint16_t yLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t wrldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    std::uint16_t tileId = 0xFFFF;
    [[maybe_unused]] bool tileMatch = false;
    if (argc == 4) {
        tileId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
        tileMatch = true;
    }
    bool tileFound = false;
    
    auto artwork = worldMULHandler.ArtAt(xLoc, yLoc, wrldNumber);
    auto iter = std::find_if(artwork.begin(), artwork.end(),
                             [tileId](const Tile_st &tile) { return tile.tileId == tileId; });
    tileFound = iter != artwork.end();
    *rval = BOOLEAN_TO_JSVAL(tileFound);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StringToNum()
//|	Date		-	27th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts string to number
// o------------------------------------------------------------------------------------------------o
JSBool SE_StringToNum(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "StringToNum: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    
    std::string str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    
    *rval = INT_TO_JSVAL(std::stoi(str, nullptr, 0));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_NumToString()
//|	Date		-	27th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts number to string
// o------------------------------------------------------------------------------------------------o
JSBool SE_NumToString(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                      jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "NumToString: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    
    std::int32_t num = JSVAL_TO_INT(argv[0]);
    auto str = util::ntos(num);
    *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str.c_str()));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_NumToHexString()
//|	Date		-	27th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts number to hex string
// o------------------------------------------------------------------------------------------------o
JSBool SE_NumToHexString(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "NumToHexString: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    
    std::int32_t num = JSVAL_TO_INT(argv[0]);
    auto str = util::ntos(num, 16);
    
    *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str.c_str()));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRaceCount()
//|	Date		-	November 9, 2001
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the total number of races found in the server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetRaceCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    if (argc != 0) {
        ScriptError(cx, "GetRaceCount: Invalid number of arguments (takes 0)");
        return JS_FALSE;
    }
    *rval = INT_TO_JSVAL(worldRace.Count());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_AreaCharacterFunction()
//|	Date		-	January 27, 2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area
// of characters
// o------------------------------------------------------------------------------------------------o
JSBool SE_AreaCharacterFunction(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3 && argc != 4) {
        // function name, source character, range
        ScriptError(cx, "AreaCharacterFunction: Invalid number of arguments (takes 3/4, function name, source character, range, optional socket)");
        return JS_FALSE;
    }
    
    // Do parameter validation here
    JSObject *srcSocketObj = nullptr;
    CSocket *srcSocket = nullptr;
    char *trgFunc = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    if (trgFunc == nullptr) {
        ScriptError(cx, "AreaCharacterFunction: Argument 0 not a valid string");
        return JS_FALSE;
    }
    
    JSObject *srcBaseObj = JSVAL_TO_OBJECT(argv[1]);
    CBaseObject *srcObject = static_cast<CBaseObject *>(JS_GetPrivate(cx, srcBaseObj));
    
    if (!ValidateObject(srcObject)) {
        ScriptError(cx, "AreaCharacterFunction: Argument 1 not a valid object");
        return JS_FALSE;
    }
    float distance = static_cast<float>(JSVAL_TO_INT(argv[2]));
    if (argc == 4 && argv[3] != JSVAL_NULL) {
        srcSocketObj = JSVAL_TO_OBJECT(argv[3]);
        srcSocket = static_cast<CSocket *>(JS_GetPrivate(cx, srcSocketObj));
    }
    
    std::vector<CChar *> charsFound;
    std::uint16_t retCounter = 0;
    cScript *myScript = worldJSMapping.GetScript(JS_GetGlobalObject(cx));
    for (auto &MapArea : worldMapHandler.PopulateList(srcObject)) {
        if (MapArea) {
            auto regChars = MapArea->GetCharList();
            for (const auto &tempChar : regChars->collection()) {
                if (ValidateObject(tempChar)) {
                    if (ObjInRange(srcObject, tempChar, static_cast<std::uint16_t>(distance))) {
                        // Store character reference for later
                        charsFound.push_back(tempChar);
                    }
                }
            }
        }
    }
    
    // Now iterate over all the characters that we found previously, and run the area function for
    // each
    try {
        std::for_each(charsFound.begin(), charsFound.end(),
                      [myScript, trgFunc, srcObject, srcSocket, &retCounter](CChar *tempChar) {
            if (myScript->AreaObjFunc(trgFunc, srcObject, tempChar, srcSocket)) {
                ++retCounter;
            }
        });
    } catch (const std::out_of_range &e) {
        ScriptError(cx, util::format("Critical error encountered in AreaObjFunc!", e.what()).c_str());
    }
    
    *rval = INT_TO_JSVAL(retCounter);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_AreaItemFunction()
//|	Date		-	17th August, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area
// of items
// o------------------------------------------------------------------------------------------------o
JSBool SE_AreaItemFunction(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3 && argc != 4) {
        // function name, source character, range
        ScriptError(cx, "AreaItemFunction: Invalid number of arguments (takes 3/4, function name, source object, range, optional socket)");
        return JS_FALSE;
    }
    
    // Do parameter validation here
    JSObject *srcSocketObj = nullptr;
    CSocket *srcSocket = nullptr;
    char *trgFunc = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    if (trgFunc == nullptr) {
        ScriptError(cx, "AreaItemFunction: Argument 0 not a valid string");
        return JS_FALSE;
    }
    
    JSObject *srcBaseObj = JSVAL_TO_OBJECT(argv[1]);
    CBaseObject *srcObject = static_cast<CBaseObject *>(JS_GetPrivate(cx, srcBaseObj));
    
    if (!ValidateObject(srcObject)) {
        ScriptError(cx, "AreaItemFunction: Argument 1 not a valid object");
        return JS_FALSE;
    }
    float distance = static_cast<float>(JSVAL_TO_INT(argv[2]));
    if (argc == 4 && argv[3] != JSVAL_NULL) {
        srcSocketObj = JSVAL_TO_OBJECT(argv[3]);
        if (srcSocketObj != nullptr) {
            srcSocket = static_cast<CSocket *>(JS_GetPrivate(cx, srcSocketObj));
        }
    }
    
    std::vector<CItem *> itemsFound;
    std::uint16_t retCounter = 0;
    cScript *myScript = worldJSMapping.GetScript(JS_GetGlobalObject(cx));
    for (auto &MapArea : worldMapHandler.PopulateList(srcObject)) {
        if (MapArea) {
            auto regItems = MapArea->GetItemList();
            for (const auto &tempItem : regItems->collection()) {
                if (ValidateObject(tempItem)) {
                    if (ObjInRange(srcObject, tempItem, static_cast<std::uint16_t>(distance))) {
                        // Store item reference for later
                        itemsFound.push_back(tempItem);
                    }
                }
            }
        }
    }
    
    // Now iterate over all the characters that we found previously, and run the area function for
    // each
    std::for_each(itemsFound.begin(), itemsFound.end(), [myScript, trgFunc, srcObject, srcSocket, &retCounter](CItem *tempItem) {
        if (myScript->AreaObjFunc(trgFunc, srcObject, tempItem, srcSocket)) {
            ++retCounter;
        }
    });
    
    *rval = INT_TO_JSVAL(retCounter);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetDictionaryEntry()
//|	Date		-	7/17/2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows the JSScripts to pull entries from the dictionaries and
// convert them to a string.
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetDictionaryEntry(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 1) {
        ScriptError(cx, "GetDictionaryEntry: Invalid number of arguments (takes at least 1)");
        return JS_FALSE;
    }
    
    std::int32_t dictEntry = static_cast<std::int32_t>(JSVAL_TO_INT(argv[0]));
    unicodetypes_t language = ZERO;
    if (argc == 2) {
        language = static_cast<unicodetypes_t>(JSVAL_TO_INT(argv[1]));
    }
    std::string txt = worldDictionary.GetEntry(dictEntry, language);
    txt = oldstrutil::stringToWstringToString(txt);
    
    JSString *strTxt = nullptr;
    strTxt = JS_NewStringCopyZ(cx, txt.c_str());
    *rval = STRING_TO_JSVAL(strTxt);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_Yell()
//|	Date		-	7/17/2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Globally yell a message from JS (Based on Commandlevel)
// o------------------------------------------------------------------------------------------------o
JSBool SE_Yell(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc < 3) {
        ScriptError(cx, "Yell: Invalid number of arguments (takes 3)");
        return JS_FALSE;
    }
    
    JSObject *mSock = JSVAL_TO_OBJECT(argv[0]);
    CSocket *mySock = static_cast<CSocket *>(JS_GetPrivate(cx, mSock));
    CChar *myChar = mySock->CurrcharObj();
    std::string textToYell = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    std::uint8_t commandLevel = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    
    std::string yellTo = "";
    switch (static_cast<CommandLevels>(commandLevel)) {
        case CL_PLAYER:
            yellTo = " (GLOBAL YELL): ";
            break;
        case CL_CNS:
            yellTo = " (CNS YELL): ";
            break;
        case CL_GM:
            yellTo = " (GM YELL): ";
            break;
        case CL_ADMIN:
            yellTo = " (ADMIN YELL): ";
            break;
    }
    
    std::string tmpString = GetNpcDictName(myChar, mySock, NRS_SPEECH) + yellTo + textToYell;
    
    if (ServerConfig::shared().enabled(ServerSwitch::UNICODEMESSAGE)) {
        CPUnicodeMessage unicodeMessage;
        unicodeMessage.Message(tmpString);
        unicodeMessage.Font(static_cast<fonttype_t>(myChar->GetFontType()));
        if (mySock->GetWord(4) == 0x1700) {
            unicodeMessage.Colour(0x5A);
        }
        else if (mySock->GetWord(4) == 0x0) {
            unicodeMessage.Colour(0x5A);
        }
        else {
            unicodeMessage.Colour(mySock->GetWord(4));
        }
        unicodeMessage.Type(YELL);
        unicodeMessage.Language("ENG");
        unicodeMessage.Name("SYSTEM");
        unicodeMessage.ID(INVALIDID);
        unicodeMessage.Serial(INVALIDSERIAL);
        
        mySock->Send(&unicodeMessage);
    }
    else {
        CSpeechEntry &toAdd = worldSpeechSystem.Add();
        toAdd.Speech(tmpString);
        toAdd.Font(static_cast<fonttype_t>(myChar->GetFontType()));
        toAdd.Speaker(INVALIDSERIAL);
        if (mySock->GetWord(4) == 0x1700) {
            toAdd.Colour(0x5A);
        }
        else if (mySock->GetWord(4) == 0x0) {
            toAdd.Colour(0x5A);
        }
        else {
            toAdd.Colour(mySock->GetWord(4));
        }
        toAdd.Type(SYSTEM);
        toAdd.At(worldMain.GetUICurrentTime());
        toAdd.TargType(SPTRG_BROADCASTALL);
        toAdd.CmdLevel(commandLevel);
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_Reload()
//|	Date		-	29 Dec 2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads certain server subsystems
// o------------------------------------------------------------------------------------------------o
JSBool SE_Reload([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "Reload: needs 1 argument!");
        return JS_FALSE;
    }
    
    std::int32_t toLoad = JSVAL_TO_INT(argv[0]);
    
    switch (toLoad) {
        case 0: // Reload regions
            UnloadRegions();
            loadRegions();
            loadTeleportLocations();
            break;
        case 1: // Reload spawn regions
            UnloadSpawnRegions();
            loadSpawnRegions();
            break;
        case 2: // Reload Spells
            worldMagic.LoadScript();
            break;
        case 3: // Reload serverCommands
            serverCommands.load();
            break;
        case 4: // Reload DFNs
            worldFileLookup.Reload();
            loadSkills();
            worldSkill.load();
            break;
        case 5: // Reload JScripts
            messageLoop << MSG_RELOADJS;
            break;
        case 6: // Reload HTMLTemplates
            worldHTMLTemplate.Unload();
            worldHTMLTemplate.load();
            break;
        case 7: // Reload INI
            ServerConfig::shared().loadConfig(std::filesystem::path()) ;
            break;
        case 8: // Reload Everything
            worldFileLookup.Reload();
            UnloadRegions();
            loadRegions();
            UnloadSpawnRegions();
            loadSpawnRegions();
            worldMagic.LoadScript();
            serverCommands.load();
            loadSkills();
            worldSkill.load();
            messageLoop << MSG_RELOADJS;
            worldHTMLTemplate.Unload();
            worldHTMLTemplate.load();
            ServerConfig::shared().loadConfig(std::filesystem::path()) ;
            break;
        case 9: // Reload Accounts
            Account::shared().load();
            break;
        case 10: // Reload Dictionaries
            worldDictionary.LoadDictionaries();
            break;
        default:
            break;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SendStaticStats()
//|	Date		-	25th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Builds an info gump for specified static or map tile
// o------------------------------------------------------------------------------------------------o
JSBool SE_SendStaticStats(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "SendStaticStats: needs 1 argument!");
        return JS_FALSE;
    }
    
    JSObject *mSock = JSVAL_TO_OBJECT(argv[0]);
    CSocket *mySock = static_cast<CSocket *>(JS_GetPrivate(cx, mSock));
    if (mySock == nullptr) {
        ScriptError(cx, "SendStaticStats: passed an invalid socket!");
        return JS_FALSE;
    }
    CChar *myChar = mySock->CurrcharObj();
    if (!ValidateObject(myChar))
        return JS_TRUE;
    
    if (mySock->GetDWord(7) == 0) {
        std::uint8_t worldNumber = myChar->WorldNumber();
        std::uint16_t targetId = mySock->GetWord(0x11);
        std::int16_t targetX = mySock->GetWord(0x0B); // store our target x y and z locations
        std::int16_t targetY = mySock->GetWord(0x0D);
        std::int8_t targetZ = mySock->GetByte(0x10);
        if (targetId != 0) // we might have a static rock or mountain
        {
            auto artwork = worldMULHandler.ArtAt(targetX, targetY, worldNumber);
            for (auto &tile : artwork) {
                if (targetZ == tile.altitude) {
                    CGumpDisplay staticStat(mySock, 300, 300);
                    staticStat.setTitle("Item [Static]");
                    staticStat.AddData("ID", targetId, 5);
                    staticStat.AddData("Height", tile.height());
                    staticStat.AddData("Name", tile.artInfo->Name());
                    staticStat.Send(4, false, INVALIDSERIAL);
                }
            }
        }
        else // or it could be a map only
        {
            // manually calculating the ID's if a maptype
            auto map1 = worldMULHandler.SeekMap(targetX, targetY, worldNumber);
            CGumpDisplay mapStat(mySock, 300, 300);
            mapStat.setTitle("Item [Map]");
            mapStat.AddData("ID", targetId, 5);
            mapStat.AddData("Name", map1.name());
            mapStat.Send(4, false, INVALIDSERIAL);
        }
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTileHeight()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the tile height of a specified tile (item)
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetTileHeight([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "GetTileHeight: needs 1 argument!");
        return JS_FALSE;
    }
    
    std::uint16_t tileNum = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    *rval = INT_TO_JSVAL(worldMULHandler.TileHeight(tileNum));
    return JS_TRUE;
}

bool SE_IterateFunctor(CBaseObject *a, std::uint32_t &b, void *extraData) {
    cScript *myScript = static_cast<cScript *>(extraData);
    return myScript->OnIterate(a, b);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IterateOver()
//|	Date		-	July 25th, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all world objects
// o------------------------------------------------------------------------------------------------o
JSBool SE_IterateOver(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "IterateOver: needs 1 argument!");
        return JS_FALSE;
    }
    
    std::uint32_t b = 0;
    std::string objType = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    auto toCheck = FindObjTypeFromString(objType);
    cScript *myScript = worldJSMapping.GetScript(JS_GetGlobalObject(cx));
    if (myScript != nullptr) {
        ObjectFactory::shared().IterateOver(toCheck, b, myScript, &SE_IterateFunctor);
    }
    
    JS_MaybeGC(cx);
    
    *rval = INT_TO_JSVAL(b);
    return JS_TRUE;
}

bool SE_IterateSpawnRegionsFunctor(CSpawnRegion *a, std::uint32_t &b, void *extraData) {
    cScript *myScript = static_cast<cScript *>(extraData);
    return myScript->OnIterateSpawnRegions(a, b);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IterateOverSpawnRegions()
//|	Date		-	July, 2020
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops over all spawn regions in the world
// o------------------------------------------------------------------------------------------------o
JSBool SE_IterateOverSpawnRegions(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    std::uint32_t b = 0;
    cScript *myScript = worldJSMapping.GetScript(JS_GetGlobalObject(cx));
    
    if (myScript != nullptr) {
        std::for_each(worldMain.spawnRegions.begin(), worldMain.spawnRegions.end(), [&myScript, &b](std::pair<std::uint16_t, CSpawnRegion *> entry) {
            if (entry.second) {
                SE_IterateSpawnRegionsFunctor(entry.second, b, myScript);
            }
        });
    }
    
    JS_MaybeGC(cx);
    
    *rval = INT_TO_JSVAL(b);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldBrightLevel()
//|	Date		-	18th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world bright level - brightest part of the regular
// day/night cycle
// o------------------------------------------------------------------------------------------------o
JSBool SE_WorldBrightLevel([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,  uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx, util::format("WorldBrightLevel: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        auto brightLevel = static_cast<lightlevel_t>(JSVAL_TO_INT(argv[0]));
        ServerConfig::shared().ushortValues[UShortValue::BRIGHTLEVEL] = brightLevel;
    }
    *rval = INT_TO_JSVAL( ServerConfig::shared().ushortValues[UShortValue::BRIGHTLEVEL]);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldDarkLevel()
//|	Date		-	18th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world dark level - darkest part of the regular
// day/night cycle
// o------------------------------------------------------------------------------------------------o
JSBool SE_WorldDarkLevel([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx, util::format("WorldDarkLevel: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        auto darkLevel = static_cast<lightlevel_t>(JSVAL_TO_INT(argv[0]));
        ServerConfig::shared().ushortValues[UShortValue::DARKLEVEL] = darkLevel;
    }
    *rval = ServerConfig::shared().ushortValues[UShortValue::DARKLEVEL];
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldDungeonLevel()
//|	Date		-	18th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets default light level in dungeons
// o------------------------------------------------------------------------------------------------o
JSBool SE_WorldDungeonLevel([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError( cx, util::format("WorldDungeonLevel: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        auto dungeonLevel = static_cast<lightlevel_t>(JSVAL_TO_INT(argv[0]));
        ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT] = dungeonLevel;
        
    }
    *rval = INT_TO_JSVAL(ServerConfig::shared().ushortValues[UShortValue::DUNGEONLIGHT]);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegionFacetStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets enabled state of given spawn region
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetSpawnRegionFacetStatus([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx, util::format("GetSpawnRegionFacetStatus: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        std::uint32_t spawnRegionFacet = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[0]));
        
        bool spawnRegionFacetStatus =  ServerConfig::shared().spawnFacet.test(spawnRegionFacet);
        if (spawnRegionFacetStatus) {
            *rval = INT_TO_JSVAL(1);
        }
        else {
            *rval = INT_TO_JSVAL(0);
        }
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SetSpawnRegionFacetStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets enabled state of spawn regions
// o------------------------------------------------------------------------------------------------o
JSBool SE_SetSpawnRegionFacetStatus([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc > 2) {
        ScriptError(cx, util::format("SetSpawnRegionFacetStatus: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        std::uint32_t spawnRegionFacetVal = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[0]));
        // This should be a generic function in ServerConfig, but for now
        ServerConfig::shared().spawnFacet = spawnRegionFacetVal ;
    }
    else if (argc == 2) {
        std::uint32_t spawnRegionFacet = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[0]));
        bool spawnRegionFacetStatus = (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE);
        ServerConfig::shared().spawnFacet.set(spawnRegionFacet, spawnRegionFacetStatus);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSocketFromIndex()
//|	Date		-	3rd August, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns socket based on provided index, from list of connected
// clients
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetSocketFromIndex(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "GetSocketFromIndex: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    auto index = static_cast<uoxsocket_t>(JSVAL_TO_INT(argv[0]));
    
    CSocket *mSock = worldNetwork.GetSockPtr(index);
    CChar *mChar = nullptr;
    if (mSock != nullptr) {
        mChar = mSock->CurrcharObj();
    }
    
    if (!ValidateObject(mChar)) {
        *rval = JSVAL_NULL;
        return JS_FALSE;
    }
    
    JSObject *myObj =
    worldJSEngine.AcquireObject(IUE_CHAR, mChar, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
    *rval = OBJECT_TO_JSVAL(myObj);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ReloadJSFile()
//|	Date		-	5th February, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload specified JS file by scriptId
// o------------------------------------------------------------------------------------------------o
JSBool SE_ReloadJSFile(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "ReloadJSFile: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::uint16_t scriptId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    if (scriptId == worldJSMapping.GetScriptId(JS_GetGlobalObject(cx))) {
        ScriptError(cx, util::format("ReloadJSFile: JS Script attempted to reload itself, crash avoided (ScriptID %u)",scriptId).c_str());
        return JS_FALSE;
    }
    
    worldJSMapping.Reload(scriptId);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceArea()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resource areas to split the world into
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceArea(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 0) {
        ScriptError(cx, util::format("ResourceArea: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    
    auto resType = std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    resType = util::upper(util::trim(util::strip(resType, "//")));
    
    *rval = INT_TO_JSVAL( ServerConfig::shared().ushortValues[UShortValue::RESOURCEAREASIZE]);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceAmount()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resources (logs/ore/fish) in each resource area
// on the server
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceAmount(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,  jsval *rval) {
    if (argc > 2 || argc == 0) {
        ScriptError(cx,util::format("ResourceAmount: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    auto resType = std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    resType = util::upper(util::trim(util::strip(resType, "//")));
    
    if (argc == 2) {
        std::int16_t newVal = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
        if (resType == "LOGS") {
            ServerConfig::shared().ushortValues[UShortValue::LOGPERAREA] = newVal ;
        }
        else if (resType == "ORE") {
            ServerConfig::shared().ushortValues[UShortValue::OREPERAREA] = newVal ;
        }
        else if (resType == "FISH") {
            ServerConfig::shared().ushortValues[UShortValue::FISHPERAREA] = newVal ;
        }
    }
    
    if (resType == "LOGS") {
        *rval = INT_TO_JSVAL(ServerConfig::shared().ushortValues[UShortValue::LOGPERAREA]);
    }
    else if (resType == "ORE") {
        *rval = INT_TO_JSVAL(ServerConfig::shared().ushortValues[UShortValue::OREPERAREA]);
    }
    else if (resType == "FISH") {
        
        *rval = INT_TO_JSVAL(ServerConfig::shared().ushortValues[UShortValue::FISHPERAREA]);
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceTime()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets respawn timers for ore/log resources on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceTime(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc > 2 || argc == 0) {
        ScriptError(cx, util::format("ResourceTime: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    
    auto resType = std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    resType = util::upper(util::trim(util::strip(resType, "//")));
    if (argc == 2) {
        std::uint16_t newVal = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
        if (resType == "LOGS") {
            ServerConfig::shared().timerSetting[TimerSetting::LOG] = newVal;
        }
        else if (resType == "ORE") {
            ServerConfig::shared().timerSetting[TimerSetting::ORE] = newVal;
        }
        else if (resType == "FISH") {
            ServerConfig::shared().timerSetting[TimerSetting::FISH] = newVal;
        }
    }
    
    if (resType == "LOGS") {
        *rval = INT_TO_JSVAL(ServerConfig::shared().timerSetting[TimerSetting::LOG]);
    }
    else if (resType == "ORE") {
        *rval = INT_TO_JSVAL(ServerConfig::shared().timerSetting[TimerSetting::ORE]);
    }
    else if (resType == "FISH") {
        *rval = INT_TO_JSVAL(ServerConfig::shared().timerSetting[TimerSetting::FISH]);
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceRegion()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a resource object allowing JS to modify resource data.
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceRegion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3) {
        ScriptError(cx, "ResourceRegion: Invalid number of arguments (takes 3)");
        return JS_FALSE;
    }
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    MapResource_st *mRes = worldMapHandler.GetResource(x, y, worldNum);
    if (mRes == nullptr) {
        ScriptError(cx, "ResourceRegion: Invalid Resource Region");
        return JS_FALSE;
    }
    
    JSObject *jsResource = JS_NewObject(cx, &UOXResource_class, nullptr, obj);
    JS_DefineProperties(cx, jsResource, cresourceproperties_t);
    JS_SetPrivate(cx, jsResource, mRes);
    
    *rval = OBJECT_TO_JSVAL(jsResource);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ValidateObject()
//|	Date		-	26th January, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if object is a valid and not nullptr or marked for deletion
// o------------------------------------------------------------------------------------------------o
JSBool SE_ValidateObject(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "ValidateObject: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    
    JSEncapsulate myClass(cx, &(argv[0]));
    
    if (myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem") {
        CBaseObject *myObj = static_cast<CBaseObject *>(myClass.toObject());
        *rval = BOOLEAN_TO_JSVAL(ValidateObject(myObj));
    }
    else {
        *rval = JSVAL_FALSE;
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ApplyDamageBonuses()
//|	Date		-	17th March, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage bonuses based on race/weather weakness and character
// skills
// o------------------------------------------------------------------------------------------------o
JSBool SE_ApplyDamageBonuses(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 6) {
        ScriptError(cx, "ApplyDamageBonuses: Invalid number of arguments (takes 6)");
        return JS_FALSE;
    }
    
    CChar *attacker = nullptr, *defender = nullptr;
    std::int16_t damage = 0;
    
    JSEncapsulate damageType(cx, &(argv[0]));
    JSEncapsulate getFightSkill(cx, &(argv[3]));
    JSEncapsulate hitLoc(cx, &(argv[4]));
    JSEncapsulate baseDamage(cx, &(argv[5]));
    
    JSEncapsulate attackerClass(cx, &(argv[1]));
    if (attackerClass.ClassName() != "UOXChar") {// It must be a character!
        ScriptError(cx, "ApplyDamageBonuses: Passed an invalid Character");
        return JS_FALSE;
    }
    
    if (attackerClass.isType(JSEncapsulate::JSOT_VOID) || attackerClass.isType(JSEncapsulate::JSOT_NULL)) {
        ScriptError(cx, "ApplyDamageBonuses: Passed an invalid Character");
        return JS_TRUE;
    }
    else {
        attacker = static_cast<CChar *>(attackerClass.toObject());
        if (!ValidateObject(attacker)) {
            ScriptError(cx, "ApplyDamageBonuses: Passed an invalid Character");
            return JS_TRUE;
        }
    }
    
    JSEncapsulate defenderClass(cx, &(argv[2]));
    if (defenderClass.ClassName() != "UOXChar") {// It must be a character!
        ScriptError(cx, "ApplyDamageBonuses: Passed an invalid Character");
        return JS_FALSE;
    }
    
    if (defenderClass.isType(JSEncapsulate::JSOT_VOID) || defenderClass.isType(JSEncapsulate::JSOT_NULL)) {
        ScriptError(cx, "ApplyDamageBonuses: Passed an invalid Character");
        return JS_TRUE;
    }
    else {
        defender = static_cast<CChar *>(defenderClass.toObject());
        if (!ValidateObject(defender)) {
            ScriptError(cx, "ApplyDamageBonuses: Passed an invalid Character");
            return JS_TRUE;
        }
    }
    
    damage = worldCombat.ApplyDamageBonuses(static_cast<Weather::type_t>(damageType.toInt()), attacker, defender, static_cast<std::uint8_t>(getFightSkill.toInt()), static_cast<std::uint8_t>(hitLoc.toInt()), static_cast<std::int16_t>(baseDamage.toInt()));
    
    *rval = INT_TO_JSVAL(damage);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ApplyDefenseModifiers()
//|	Date		-	17th March, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns defense modifiers based on shields/parrying, armor values
// and elemental damage
// o------------------------------------------------------------------------------------------------o
JSBool SE_ApplyDefenseModifiers(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 7) {
        ScriptError(cx, "ApplyDefenseModifiers: Invalid number of arguments (takes 7)");
        return JS_FALSE;
    }
    
    CChar *attacker = nullptr, *defender = nullptr;
    std::int16_t damage = 0;
    
    JSEncapsulate damageType(cx, &(argv[0]));
    JSEncapsulate getFightSkill(cx, &(argv[3]));
    JSEncapsulate hitLoc(cx, &(argv[4]));
    JSEncapsulate baseDamage(cx, &(argv[5]));
    JSEncapsulate doArmorDamage(cx, &(argv[6]));
    
    JSEncapsulate attackerClass(cx, &(argv[1]));
    if (attackerClass.ClassName() == "UOXChar") {
        if (attackerClass.isType(JSEncapsulate::JSOT_VOID) || attackerClass.isType(JSEncapsulate::JSOT_NULL)) {
            attacker = nullptr;
        }
        else {
            attacker = static_cast<CChar *>(attackerClass.toObject());
            if (!ValidateObject(attacker)) {
                attacker = nullptr;
            }
        }
    }
    
    JSEncapsulate defenderClass(cx, &(argv[2]));
    if (defenderClass.ClassName() != "UOXChar") // It must be a character!
    {
        ScriptError(cx, "ApplyDefenseModifiers: Passed an invalid Character");
        return JS_FALSE;
    }
    
    if (defenderClass.isType(JSEncapsulate::JSOT_VOID) || defenderClass.isType(JSEncapsulate::JSOT_NULL)) {
        ScriptError(cx, "ApplyDefenseModifiers: Passed an invalid Character");
        return JS_TRUE;
    }
    else {
        defender = static_cast<CChar *>(defenderClass.toObject());
        if (!ValidateObject(defender)) {
            ScriptError(cx, "ApplyDefenseModifiers: Passed an invalid Character");
            return JS_TRUE;
        }
    }
    
    damage = worldCombat.ApplyDefenseModifiers(static_cast<Weather::type_t>(damageType.toInt()), attacker, defender, static_cast<std::uint8_t>(getFightSkill.toInt()), static_cast<std::uint8_t>(hitLoc.toInt()), static_cast<std::int16_t>(baseDamage.toInt()), doArmorDamage.toBool());
    
    *rval = INT_TO_JSVAL(damage);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WillResultInCriminal()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if hostile action done by one character versus another will
// result in criminal flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_WillResultInCriminal(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "WillResultInCriminal: Invalid number of arguments (takes 2: srcChar and trgChar)");
        return JS_FALSE;
    }
    
    bool result = false;
    CChar *srcChar = nullptr;
    CChar *trgChar = nullptr;
    
    if (argv[0] != JSVAL_NULL && argv[1] != JSVAL_NULL) {
        JSObject *srcCharObj = JSVAL_TO_OBJECT(argv[0]);
        srcChar = static_cast<CChar *>(JS_GetPrivate(cx, srcCharObj));
        
        JSObject *trgCharObj = JSVAL_TO_OBJECT(argv[1]);
        trgChar = static_cast<CChar *>(JS_GetPrivate(cx, trgCharObj));
        
        if (ValidateObject(srcChar) && ValidateObject(trgChar)) {
            result = WillResultInCriminal(srcChar, trgChar);
        }
    }
    else {
        ScriptError(cx, "WillResultInCriminal: Invalid objects passed - characters required)");
        return JS_FALSE;
    }
    
    *rval = BOOLEAN_TO_JSVAL(result);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateParty()
//|	Date		-	21st September, 2006
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a party with specified character as the party leader
// o------------------------------------------------------------------------------------------------o
JSBool SE_CreateParty(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "CreateParty: Invalid number of arguments (takes 1, the leader)");
        return JS_FALSE;
    }
    
    JSEncapsulate myClass(cx, &(argv[0]));
    
    if (myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXSocket") { // it's a character or socket, fantastic
        CChar *leader = nullptr;
        CSocket *leaderSock = nullptr;
        if (myClass.ClassName() == "UOXChar") {
            leader = static_cast<CChar *>(myClass.toObject());
            leaderSock = leader->GetSocket();
        }
        else {
            leaderSock = static_cast<CSocket *>(myClass.toObject());
            leader = leaderSock->CurrcharObj();
        }
        
        if (PartyFactory::shared().Get(leader) != nullptr) {
            *rval = JSVAL_NULL;
        }
        else {
            Party *tParty = PartyFactory::shared().Create(leader);
            JSObject *myObj = worldJSEngine.AcquireObject(IUE_PARTY, tParty, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
            *rval = OBJECT_TO_JSVAL(myObj);
        }
    }
    else // anything else isn't a valid leader people
    {
        *rval = JSVAL_NULL;
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_Moon()
//|	Date		-	25th May, 2007
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets or sets Moon phases on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_Moon([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc > 2 || argc == 0) {
        ScriptError(cx, util::format("Moon: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    
    std::int16_t slot = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    if (argc == 2) {
        std::int16_t newVal = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
        worldMain.uoTime.moon[slot] = newVal ;
    }
    
    *rval = INT_TO_JSVAL(worldMain.uoTime.moon[slot]);
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTownRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified region object
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetTownRegion(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "GetTownRegion: Invalid number of parameters (1)");
        return JS_FALSE;
    }
    
    std::uint16_t regNum = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    if (worldMain.townRegions.find(regNum) != worldMain.townRegions.end()) {
        CTownRegion *townReg = worldMain.townRegions[regNum];
        if (townReg != nullptr) {
            JSObject *myObj = worldJSEngine.AcquireObject(IUE_REGION, townReg, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
            *rval = OBJECT_TO_JSVAL(myObj);
        }
        else {
            *rval = JSVAL_NULL;
        }
    }
    else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegion()
//|	Date		-	June 22, 2020
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified spawn region object
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetSpawnRegion(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "GetSpawnRegion: Invalid number of parameters (1 - spawnRegionID, or 4 - x, y, world and instanceID)");
        return JS_FALSE;
    }
    
    if (argc == 1) {
        // Assume spawn region number was provided
        std::uint16_t spawnRegNum = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
        if (worldMain.spawnRegions.find(spawnRegNum) != worldMain.spawnRegions.end()) {
            CSpawnRegion *spawnReg = worldMain.spawnRegions[spawnRegNum];
            if (spawnReg != nullptr) {
                JSObject *myObj = worldJSEngine.AcquireObject(IUE_SPAWNREGION, spawnReg, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
                *rval = OBJECT_TO_JSVAL(myObj);
            }
            else {
                *rval = JSVAL_NULL;
            }
        }
        else {
            *rval = JSVAL_NULL;
        }
    }
    else {
        // Assume coordinates were provided
        std::uint16_t x = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
        std::uint16_t y = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
        std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
        std::uint16_t instanceID = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
        
        // Iterate over each spawn region to find the right one
        auto iter = std::find_if(worldMain.spawnRegions.begin(), worldMain.spawnRegions.end(), [&x, &y, &worldNum, &instanceID, &cx, &rval](std::pair<std::uint16_t, CSpawnRegion *> entry) {
            if (entry.second && x >= entry.second->GetX1() && x <= entry.second->GetX2() && y >= entry.second->GetY1() && y <= entry.second->GetY2() && entry.second->GetInstanceId() == instanceID && entry.second->WorldNumber() == worldNum) {
                JSObject *myObj = worldJSEngine.AcquireObject(IUE_SPAWNREGION, entry.second, worldJSEngine.FindActiveRuntime(JS_GetRuntime(cx)));
                *rval = OBJECT_TO_JSVAL(myObj);
                return true;
            }
            return false;
        });
        
        if (iter == worldMain.spawnRegions.end()) {
            *rval = JSVAL_NULL;
        }
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegionCount()
//|	Date		-	June 22, 2020
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the total number of spawn regions found in the server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetSpawnRegionCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    if (argc != 0) {
        ScriptError(cx, "GetSpawnRegionCount: Invalid number of arguments (takes 0)");
        return JS_FALSE;
    }
    *rval = INT_TO_JSVAL(worldMain.spawnRegions.size());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMapElevation()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map elevation at given coordinates
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetMapElevation([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,  uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3) {
        ScriptError(cx, "GetMapElevation: Invalid number of arguments (takes 3: X, Y and WorldNumber)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    std::int8_t mapElevation = worldMULHandler.MapElevation(x, y, worldNum);
    *rval = INT_TO_JSVAL(mapElevation);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IsInBuilding()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified location is inside a building
//| Notes		-	First checks if player is in a static building. If false, checks if
// there's a multi
//|					at the same location as the player, and assumes they are in
//the building if true
// o------------------------------------------------------------------------------------------------o
JSBool SE_IsInBuilding([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,  jsval *argv, jsval *rval) {
    if (argc != 6) {
        ScriptError(cx, "IsInBuilding: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber and instanceId)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint16_t instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
    bool checkHeight = (JSVAL_TO_BOOLEAN(argv[5]) == JS_TRUE);
    bool isInBuilding = worldMULHandler.InBuilding(x, y, z, worldNum, instanceId);
    if (!isInBuilding) {
        // No static building was detected. How about a multi?
        CMultiObj *multi = FindMulti(x, y, z, worldNum, instanceId);
        if (ValidateObject(multi)) {
            if (checkHeight) {
                // Check if there's multi-items over the player's head
                std::int8_t multiZ = worldMULHandler.MultiHeight(multi, x, y, z, 127, checkHeight);
                if (multiZ > z) {
                    isInBuilding = true;
                }
            }
            else {
                isInBuilding = true;
            }
        }
    }
    *rval = BOOLEAN_TO_JSVAL(isInBuilding);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckStaticFlag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a
// specific flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_CheckStaticFlag([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "CheckStaticFlag: Invalid number of arguments (takes 5: X, Y, Z, WorldNumber and TileFlagID)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    auto toCheck = static_cast<uo::flag_t>(JSVAL_TO_INT(argv[4]));
    [[maybe_unused]] std::uint16_t ignoreMe = 0;
    bool hasStaticFlag = worldMULHandler.CheckStaticFlag(x, y, z, worldNum, toCheck, ignoreMe, false);
    *rval = BOOLEAN_TO_JSVAL(hasStaticFlag);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckDynamicFlag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a
// specific flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_CheckDynamicFlag([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 6) {
        ScriptError(cx, "CheckDynamicFlag: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber, instanceId and TileFlagID)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t instanceId = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    auto toCheck = static_cast<uo::flag_t>(JSVAL_TO_INT(argv[5]));
    [[maybe_unused]] std::uint16_t ignoreMe = 0;
    bool hasDynamicFlag = worldMULHandler.CheckDynamicFlag(x, y, z, worldNum, instanceId, toCheck, ignoreMe);
    *rval = BOOLEAN_TO_JSVAL(hasDynamicFlag);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckTileFlag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether tile with given ID has a specific flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_CheckTileFlag([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "CheckTileFlag: Invalid number of arguments (takes 2: itemId and tileFlagID)");
        return JS_FALSE;
    }
    
    std::uint16_t itemId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    auto flagToCheck = static_cast<uo::flag_t>(JSVAL_TO_INT(argv[1]));
    
    bool tileHasFlag = worldMULHandler.CheckTileFlag(itemId, flagToCheck);
    *rval = BOOLEAN_TO_JSVAL(tileHasFlag);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesStaticBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesStaticBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "DoesStaticBlock: Invalid number of arguments (takes 5: X, Y, Z, WorldNumber and checkWater)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    bool checkWater = (JSVAL_TO_BOOLEAN(argv[4]) == JS_TRUE);
    bool staticBlocks = worldMULHandler.DoesStaticBlock(x, y, z, worldNum, checkWater);
    *rval = BOOLEAN_TO_JSVAL(staticBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesDynamicBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if dynamics at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesDynamicBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,  uintN argc, jsval *argv, jsval *rval) {
    if (argc != 9) {
        ScriptError(cx,"DoesDynamicBlock: Invalid number of arguments (takes 9: X, Y, Z, WorldNumber, instanceId, checkWater, waterWalk, checkOnlyMultis and checkOnlyNonMultis)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t instanceId = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    bool checkWater = (JSVAL_TO_BOOLEAN(argv[5]) == JS_TRUE);
    bool waterWalk = (JSVAL_TO_BOOLEAN(argv[6]) == JS_TRUE);
    bool checkOnlyMultis = (JSVAL_TO_BOOLEAN(argv[7]) == JS_TRUE);
    bool checkOnlyNonMultis = (JSVAL_TO_BOOLEAN(argv[8]) == JS_TRUE);
    bool dynamicBlocks = worldMULHandler.DoesDynamicBlock(x, y, z, worldNum, instanceId, checkWater, waterWalk, checkOnlyMultis, checkOnlyNonMultis);
    *rval = BOOLEAN_TO_JSVAL(dynamicBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesMapBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if map tile at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesMapBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 8) {
        ScriptError(cx, "DoesMapBlock: Invalid number of arguments (takes 8: X, Y, Z, WorldNumber, checkWater, waterWalk, checkMultiPlacement and checkForRoad)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    bool checkWater = (JSVAL_TO_BOOLEAN(argv[4]) == JS_TRUE);
    bool waterWalk = (JSVAL_TO_BOOLEAN(argv[5]) == JS_TRUE);
    bool checkMultiPlacement = (JSVAL_TO_BOOLEAN(argv[6]) == JS_TRUE);
    bool checkForRoad = (JSVAL_TO_BOOLEAN(argv[7]) == JS_TRUE);
    bool mapBlocks = worldMULHandler.DoesMapBlock(x, y, z, worldNum, checkWater, waterWalk, checkMultiPlacement, checkForRoad);
    *rval = BOOLEAN_TO_JSVAL(mapBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesCharacterBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if characters at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesCharacterBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "DoesCharacterBlock: Invalid number of arguments (takes 5: X, Y, z, WorldNumber, instanceId)");
        return JS_FALSE;
    }
    
    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t instanceId = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    bool characterBlocks = worldMULHandler.DoesCharacterBlock(x, y, z, worldNum, instanceId);
    *rval = BOOLEAN_TO_JSVAL(characterBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DeleteFile()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes a file from the server's harddrive...
// o------------------------------------------------------------------------------------------------o
JSBool SE_DeleteFile(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc < 1 || argc > 3) {
        ScriptError(cx, "DeleteFile: Invalid number of arguments (takes 1 to 3 - fileName and (optionally) subFolderName and useScriptDataDir bool)");
        return JS_FALSE;
    }
    
    char *fileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    char *subFolderName = nullptr;
    if (argc >= 2) {
        subFolderName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    }
    bool useScriptDataDir = false;
    if (argc >= 3) {
        useScriptDataDir = (JSVAL_TO_BOOLEAN(argv[2]) == JS_TRUE);
    }
    
    if (strstr(fileName, "..") || strstr(fileName, "\\") || strstr(fileName, "/")) {
        ScriptError(cx, "DeleteFile: file names may not contain \".\", \"..\", \"\\\", or \"/\".");
        return JS_FALSE;
    }
    
    auto pathString = ServerConfig::shared().directoryFor(dirlocation_t::SAVE) ;
    
    // if subFolderName argument was supplied, use it
    if (subFolderName != nullptr) {
        // However, don't allow special characters in the folder name
        if (strstr(subFolderName, "..") || strstr(subFolderName, "\\") || strstr(subFolderName, "/")) {
            ScriptError( cx, "DeleteFile: folder names may not contain \".\", \"..\", \"\\\", or \"/\".");
            return JS_FALSE;
        }
        
        // If script wants to look in script-data folder instead of shared folder, let it
        if (useScriptDataDir) {
            pathString = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPTDATA) ;
        }
        
        // Append subfolder name to path
        pathString /= std::filesystem::path(subFolderName);
        
        if (!std::filesystem::exists(pathString)) {
            // Return JS_TRUE to allow script to continue running even if file was not found for
            // deletion, but set return value to false
            *rval = false;
            return JS_TRUE;
        }
        
    }
    
    pathString /= std::filesystem::path(fileName);
    *rval = std::filesystem::remove(pathString);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EraStringToNum()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts an UO era string to an int value for easier comparison in
// JavaScripts
// o------------------------------------------------------------------------------------------------o
JSBool SE_EraStringToNum(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    *rval = reinterpret_cast<long>(nullptr);
    
    if (argc != 1) {
        ScriptError(cx, "EraStringToNum: Invalid number of arguments (takes 1 - era string)");
        return JS_FALSE;
    }
    
    std::string eraString = util::upper(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    if (!eraString.empty()) {
        auto era = ServerConfig::shared().ruleSets.normalizedEraString(eraString,false,false);
        if (era.value != Era::CORE){
            *rval = INT_TO_JSVAL(static_cast<int>(era.value));
        }
        else {
            ScriptError( cx, "EraStringToNum: Provided argument not valid era string (uo, t2a, uor, td, lbr, aos, se, ml, sa, hs or tol)" );
        }
    }
    else {
        ScriptError(cx, "EraStringToNum: Provided argument contained no valid string data");
        return JS_FALSE;
    }
    return JS_TRUE;
}




JSBool SE_GetServerSetting(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    *rval = reinterpret_cast<long>(nullptr);
    
    if (argc != 1) {
        ScriptError(cx, "GetServerSetting: Invalid number of arguments (takes 1 - serverSettingName)");
        return JS_FALSE;
    }
    
    std::string settingName = util::upper(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    if (!settingName.empty()) {
        auto data = ServerConfig::shared().dataForKeyword(settingName) ;
        if (data){
            auto vdata = data.value() ;
            switch(vdata.type){
                case AllDataType::T_STRING:{
                    JSString *tString = JS_NewStringCopyZ(cx, std::get<std::string>(vdata.value).c_str());
                    *rval = STRING_TO_JSVAL(tString);
                    return JS_TRUE;
                }
                case AllDataType::T_BOOL:
                    *rval = BOOLEAN_TO_JSVAL(std::get<bool>(vdata.value));
                    return JS_TRUE;;
                case AllDataType::T_UINT64:{
                    if (INT_FITS_IN_JSVAL(std::get<std::uint64_t>(vdata.value))) {
                        *rval = INT_TO_JSVAL(std::get<std::uint64_t>(vdata.value));
                    }
                    else {
                        JS_NewNumberValue(cx, std::get<std::uint64_t>(vdata.value), rval);
                    }
                    return JS_TRUE ;
                }
                    break;
                case AllDataType::T_INT64:{
                    if (INT_FITS_IN_JSVAL(std::get<std::int64_t>(vdata.value))) {
                        *rval = INT_TO_JSVAL(std::get<std::int64_t>(vdata.value));
                    }
                    else {
                        JS_NewNumberValue(cx, std::get<std::int64_t>(vdata.value), rval);
                    }
                    return JS_TRUE ;
                }
                case AllDataType::T_UINT16:
                    *rval = INT_TO_JSVAL(std::get<std::uint16_t>(vdata.value));
                    return JS_TRUE ;
                case AllDataType::T_INT16:
                    *rval = INT_TO_JSVAL(std::get<std::int16_t>(vdata.value));
                    return JS_TRUE ;
                case AllDataType::T_UINT32:{
                    if (INT_FITS_IN_JSVAL(std::get<std::uint32_t>(vdata.value))) {
                        *rval = INT_TO_JSVAL(std::get<std::uint32_t>(vdata.value));
                    }
                    else {
                        JS_NewNumberValue(cx, std::get<std::uint32_t>(vdata.value), rval);
                    }
                    return JS_TRUE ;
                }
                case AllDataType::T_DOUBLE:
                    *rval = DOUBLE_TO_JSVAL(std::get<double>(vdata.value));
                    return JS_TRUE ;
                default:
                    ScriptError(cx, ("GetServerSetting: Unexpected data type returned for "s+ settingName).c_str());
                    return JS_FALSE ;
            }
        }
        else {
            ScriptError(cx, ("GetServerSetting: Invalid server setting name: "s+ settingName).c_str());
            return false;
        }
    }
    else {
        ScriptError(cx, "GetServerSetting: Provided argument contained no valid string data");
    }
    return JS_FALSE;
    
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetClientFeature()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific client feature is enabled on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetClientFeature([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "getClientFeature: Invalid number of arguments (takes 1 - feature ID)");
        return JS_FALSE;
    }
    auto clientFeature = static_cast<ClientFeature::feature_t>(JSVAL_TO_INT(argv[0]));
    *rval = BOOLEAN_TO_JSVAL(ServerConfig::shared().clientFeature.test(clientFeature));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerFeature()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific Server feature is enabled on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetServerFeature([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "getServerFeature: Invalid number of arguments (takes 1 - feature ID)");
        return JS_FALSE;
    }
    auto serverFeature = static_cast<ServerFeature::feature_t>(JSVAL_TO_INT(argv[0]));
    *rval = BOOLEAN_TO_JSVAL(ServerConfig::shared().serverFeature.test(serverFeature));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetAccountCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of accounts on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetAccountCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(Account::shared().size());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetPlayerCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of players online on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetPlayerCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(worldMain.GetPlayersOnline());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetItemCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of items on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetItemCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMultiCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of multis on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetMultiCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_MULTI));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetCharacterCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of characters on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetCharacterCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerVersionString()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets server version as a string
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetServerVersionString(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    std::string versionString = UOXVersion::version + "." + UOXVersion::build + " [" + OS_STR + "]";
    JSString *tString = JS_NewStringCopyZ(cx, versionString.c_str());
    *rval = STRING_TO_JSVAL(tString);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DistanceBetween()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the distance between two locations, or two objects - or a
// combination of both
// o------------------------------------------------------------------------------------------------o
JSBool SE_DistanceBetween(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2 && argc != 3 && argc != 4 && argc != 6) {
        ScriptError( cx, "DistanceBetween: needs 2, 3, 4 or 6 arguments - object a, object b - or object a, object b, (bool)checkZ - or x1, y1 and x2, y2 - or x1, y1, z1 and x2, y2, z2!");
        return JS_FALSE;
    }
    
    if (argc <= 3) {
        // 2 or 3 arguments - find dinstance between two objects in 2D or 3D
        JSObject *srcObj = JSVAL_TO_OBJECT(argv[0]);
        JSObject *trgObj = JSVAL_TO_OBJECT(argv[1]);
        bool checkZ = argc == 3 ? (JSVAL_TO_BOOLEAN(argv[2]) == JS_TRUE) : false;
        CBaseObject *srcBaseObj = static_cast<CBaseObject *>(JS_GetPrivate(cx, srcObj));
        CBaseObject *trgBaseObj = static_cast<CBaseObject *>(JS_GetPrivate(cx, trgObj));
        if (!ValidateObject(srcBaseObj) || !ValidateObject(trgBaseObj)) {
            ScriptError(cx, "DistanceBetween: Invalid source or target object");
            *rval = INT_TO_JSVAL(-1);
            return JS_FALSE;
        }
        
        if (checkZ) {
            *rval = INT_TO_JSVAL(GetDist3D(srcBaseObj, trgBaseObj));
        }
        else {
            *rval = INT_TO_JSVAL(GetDist(srcBaseObj, trgBaseObj));
        }
    }
    else {
        std::uint16_t x1 = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
        std::uint16_t y1 = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
        std::uint16_t x2 = 0;
        std::uint16_t y2 = 0;
        if (argc == 4) {
            // 4 arguments - find distance in 2D
            x2 = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[2]));
            y2 = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
            *rval = INT_TO_JSVAL(GetDist(Point3(x1, y1, 0), Point3(x2, y2, 0)));
        }
        else {
            // 6 arguments - find distance in 3D
            std::int8_t z1 = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
            x2 = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
            y2 = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
            std::int8_t z2 = static_cast<std::int8_t>(JSVAL_TO_INT(argv[5]));
            *rval = INT_TO_JSVAL(GetDist3D(Point3(x1, y1, z1), Point3(x2, y2, z2)));
        }
    }
    
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_BASEITEMSERIAL()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the BASEITEMSERIAL constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_BASEITEMSERIAL(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, BASEITEMSERIAL, rval);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDSERIAL()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDSERIAL constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_INVALIDSERIAL(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, INVALIDSERIAL, rval);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDID()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDID constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_INVALIDID(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, INVALIDID, rval);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDCOLOUR()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDCOLOUR constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_INVALIDCOLOUR(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, INVALIDCOLOUR, rval);
    return JS_TRUE;
}
