// Here are the functions that are exposed to the Script Engine
#include "sefunctions.h"

#include <filesystem>

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
#include "cweather.hpp"
#include "dictionary.h"
#include "funcdecl.h"
#include "jsencapsulate.h"
#include "magic.h"
#include "mapstuff.h"
#include "movement.h"
#include "network.h"
#include "objectfactory.h"
#include "ostype.h"
#include "other/uoxversion.hpp"
#include "partysystem.h"
#include "regions.h"
#include "skills.h"
#include "speech.h"
#include "ssection.h"
#include "townregion.h"
#include "uoxjsclasses.h"
#include "uoxjspropertyspecs.h"
#include "utility/strutil.hpp"

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
    if (stringToObjType.empty()) // if we haven't built our array yet
    {
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
JSBool SE_DoTempEffect(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                       [[maybe_unused]] jsval *rval) {
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

    if (iType == 0) // character
    {
        JSObject *mydestc = JSVAL_TO_OBJECT(argv[2]);
        CChar *mydestChar = static_cast<CChar *>(JS_GetPrivate(cx, mydestc));

        if (!ValidateObject(mydestChar)) {
            ScriptError(cx, "DoTempEffect: Invalid target ");
            return JS_FALSE;
        }
        if (argc == 8) {
            Effects->TempEffect(mysrcChar, mydestChar, static_cast<std::int8_t>(targNum), more1, more2,
                                more3, myItemPtr);
        }
        else {
            Effects->TempEffect(mysrcChar, mydestChar, static_cast<std::int8_t>(targNum), more1, more2,
                                more3);
        }
    }
    else {
        JSObject *mydesti = JSVAL_TO_OBJECT(argv[2]);
        CItem *mydestItem = static_cast<CItem *>(JS_GetPrivate(cx, mydesti));

        if (!ValidateObject(mydestItem)) {
            ScriptError(cx, "DoTempEffect: Invalid target ");
            return JS_FALSE;
        }
        Effects->TempEffect(mysrcChar, mydestItem, static_cast<std::int8_t>(targNum), more1, more2, more3);
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
JSBool SE_BroadcastMessage(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "BroadcastMessage: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::string trgMessage = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    if (trgMessage.empty() || trgMessage.length() == 0) {
        ScriptError(
            cx, util::format("BroadcastMessage: Invalid string (%s)", trgMessage.c_str()).c_str());
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
JSBool SE_CalcItemFromSer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          jsval *rval) {
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
        targSerial = CalcSerial(
            static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1])),
            static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3])));
    }

    CItem *newItem = CalcItemObjFromSer(targSerial);
    if (newItem != nullptr) {
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, newItem,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_CalcMultiFromSer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "CalcMultiFromSer: Invalid number of arguments (takes 1 or 4)");
        return JS_FALSE;
    }
    serial_t targSerial;
    if (argc == 1) {
        targSerial = static_cast<serial_t>(JSVAL_TO_INT(argv[0]));
    }
    else {
        targSerial = CalcSerial(
            static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1])),
            static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3])));
    }

    CItem *newMulti = CalcMultiFromSer(targSerial);
    if (newMulti != nullptr) {
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, newMulti,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_CalcCharFromSer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "CalcCharFromSer: Invalid number of arguments (takes 1 or 4)");
        return JS_FALSE;
    }
    auto targSerial = INVALIDSERIAL;
    if (argc == 1) {
        targSerial = static_cast<serial_t>(JSVAL_TO_INT(argv[0]));
    }
    else {
        targSerial = CalcSerial(
            static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1])),
            static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2])), static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3])));
    }

    CChar *newChar = CalcCharObjFromSer(targSerial);
    if (newChar != nullptr) {
        JSObject *myObj = JSEngine->AcquireObject(IUE_CHAR, newChar,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_DoMovingEffect(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         [[maybe_unused]] jsval *rval) {
    if (argc < 6) {
        ScriptError(cx,
                    "DoMovingEffect: Invalid number of arguments (takes 6->8 or 8->10, or 10->12)");
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
        Effects->PlayMovingAnimation(srcX, srcY, srcZ, targX, targY, targZ, effect, speed, loop,
                                     explode, hue, renderMode);
    }
    else if (!srcLocation && targLocation) {
        Effects->PlayMovingAnimation(src, targX, targY, targZ, effect, speed, loop, explode, hue,
                                     renderMode);
    }
    else {
        Effects->PlayMovingAnimation(src, trg, effect, speed, loop, explode, hue, renderMode);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoStaticEffect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a static effect at a target location
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoStaticEffect([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                         jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 7) {
        ScriptError(cx, "StaticEffect: Invalid number of arguments (takes 7 - targX, targY, targZ, "
                        "effectID, speed, loop, explode)");
        return JS_FALSE;
    }

    std::int16_t targX = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t targY = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int16_t targZ = static_cast<std::int16_t>(JSVAL_TO_INT(argv[2]));
    std::uint16_t effectId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t speed = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    std::uint8_t loop = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[5]));
    bool explode = (JSVAL_TO_BOOLEAN(argv[6]) == JS_TRUE);

    Effects->PlayStaticAnimation(targX, targY, targZ, effectId, speed, loop, explode);

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
JSBool SE_MakeItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                   [[maybe_unused]] jsval *rval) {
    if (argc != 3 && argc != 4) {
        ScriptError(cx, "MakeItem: Invalid number of arguments (takes 3, or 4 - socket, character, "
                        "createID - and optionally - resourceColour)");
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
    CreateEntry_st *toFind = Skills->FindItem(itemMenu);
    if (toFind == nullptr) {
        ScriptError(cx, util::format("MakeItem: Invalid make item (%i)", itemMenu).c_str());
        return JS_FALSE;
    }
    std::uint16_t resourceColour = 0;
    if (argc == 4) {
        resourceColour = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[3]));
    }

    Skills->MakeItem(*toFind, player, sock, itemMenu, resourceColour);

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
JSBool SE_FirstCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                       [[maybe_unused]] jsval *argv, jsval *rval) {
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
JSBool SE_NextCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                      [[maybe_unused]] jsval *argv, jsval *rval) {
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
JSBool SE_FinishedCommandList([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                              [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv,
                              jsval *rval) {
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
JSBool SE_RegisterCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          [[maybe_unused]] jsval *rval) {
    if (argc != 3) {
        ScriptError(cx, "  RegisterCommand: Invalid number of arguments (takes 4)");
        return JS_FALSE;
    }
    std::string toRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    std::uint8_t execLevel = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    bool isEnabled = (JSVAL_TO_BOOLEAN(argv[2]) == JS_TRUE);
    std::uint16_t scriptId = JSMapping->GetScriptId(JS_GetGlobalObject(cx));

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
JSBool SE_RegisterSpell(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                        [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterSpell: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::int32_t spellNumber = JSVAL_TO_INT(argv[0]);
    bool isEnabled = (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE);
    cScript *myScript = JSMapping->GetScript(JS_GetGlobalObject(cx));
    Magic->registerSpell(myScript, spellNumber, isEnabled);
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
JSBool SE_RegisterSkill(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                        [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterSkill: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::int32_t skillNumber = JSVAL_TO_INT(argv[0]);
    bool isEnabled = (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE);
    std::uint16_t scriptId = JSMapping->GetScriptId(JS_GetGlobalObject(cx));
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
            cwmWorldState->skill[skillNumber].jsScript = 0;
            return JS_FALSE;
        }

        // Skillnumber above ALLSKILLS refers to STR, INT, DEX, Fame and Karma,
        if (skillNumber < 0 || skillNumber >= ALLSKILLS)
            return JS_TRUE;

        // Both scriptId and skillNumber seem valid; assign scriptId to this skill
        cwmWorldState->skill[skillNumber].jsScript = scriptId;
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
JSBool SE_RegisterPacket(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterPacket: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::uint8_t packet = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[0]));
    std::uint8_t subCmd = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    std::uint16_t scriptId = JSMapping->GetScriptId(JS_GetGlobalObject(cx));
    if (scriptId != 0xFFFF) {
#if defined(UOX_DEBUG_MODE)
        Console::shared().print(
            util::format("Registering packet number 0x%X, subcommand 0x%x\n", packet, subCmd));
#endif
        Network->RegisterPacket(packet, subCmd, scriptId);
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
JSBool SE_RegisterKey(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                      [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterKey: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    JSEncapsulate encaps(cx, &(argv[0]));
    std::string toRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    std::uint16_t scriptId = JSMapping->GetScriptId(JS_GetGlobalObject(cx));

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
JSBool SE_RegisterConsoleFunc(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                              jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "RegisterConsoleFunc: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }
    std::string funcToRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    std::string toRegister = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    std::uint16_t scriptId = JSMapping->GetScriptId(JS_GetGlobalObject(cx));

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
JSBool SE_DisableCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         [[maybe_unused]] jsval *rval) {
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
JSBool SE_DisableKey([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                     jsval *argv, [[maybe_unused]] jsval *rval) {
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
JSBool SE_DisableConsoleFunc(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                             [[maybe_unused]] jsval *rval) {
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
JSBool SE_DisableSpell([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                       jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "DisableSpell: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::int32_t spellNumber = JSVAL_TO_INT(argv[0]);
    Magic->SetSpellStatus(spellNumber, false);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableCommand()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified command on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_EnableCommand(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                        [[maybe_unused]] jsval *rval) {
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
JSBool SE_EnableSpell([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                      jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "EnableSpell: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::int32_t spellNumber = JSVAL_TO_INT(argv[0]);
    Magic->SetSpellStatus(spellNumber, true);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableKey()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified key in console
// o------------------------------------------------------------------------------------------------o
JSBool SE_EnableKey([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                    jsval *argv, [[maybe_unused]] jsval *rval) {
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
JSBool SE_EnableConsoleFunc(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                            [[maybe_unused]] jsval *rval) {
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
JSBool SE_GetHour([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                  [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    bool ampm = cwmWorldState->ServerData()->ServerTimeAMPM();
    std::uint8_t hour = cwmWorldState->ServerData()->ServerTimeHours();
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
JSBool SE_GetMinute([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                    [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    std::uint8_t minute = cwmWorldState->ServerData()->ServerTimeMinutes();
    *rval = INT_TO_JSVAL(minute);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetDay()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the day number of the server (UO days since server start)
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetDay([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                 [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    std::int16_t day = cwmWorldState->ServerData()->ServerTimeDay();
    *rval = INT_TO_JSVAL(day);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SecondsPerUOMinute()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets the amonut of real life seconds associated with minute
// in the game
// o------------------------------------------------------------------------------------------------o
JSBool SE_SecondsPerUOMinute([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                             uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx, "SecondsPerUOMinute: Invalid number of arguments (takes 0 or 1)");
        return JS_FALSE;
    }
    else if (argc == 1) {
        std::uint16_t secondsPerUOMinute = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
        cwmWorldState->ServerData()->ServerSecondsPerUOMinute(secondsPerUOMinute);
    }
    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ServerSecondsPerUOMinute());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetCurrentClock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets timestamp for current server clock
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetCurrentClock(JSContext *cx, [[maybe_unused]] JSObject *obj,
                          [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, cwmWorldState->GetUICurrentTime(), rval);

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetStartTime()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets timestamp for server startup time
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetStartTime(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                       [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, cwmWorldState->GetStartTime(), rval);

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRandomSOSArea()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a random SOS area from list of such areas loaded from
//[SOSAREAS] section of regions.dfn
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetRandomSOSArea(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           jsval *rval) {
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
        ScriptError(cx, "GetRandomSOSArea: Invalid values passed in for worldNum and instanceId - "
                        "must be integers!");
        return JS_FALSE;
    }

    // Fetch vector of SOS locations
    auto sosLocs = cwmWorldState->sosLocs;
    if (sosLocs.size() == 0) {
        ScriptError(cx, "GetRandomSOSArea: No valid SOS areas found. Is the [SOSAREAS] section of "
                        "regions.dfn setup correctly?");
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

    cMade = Npcs->CreateNPCxyz(nnpcNum, x, y, z, world, instanceId, useNpcList);
    if (cMade != nullptr) {
        JSObject *myobj = JSEngine->AcquireObject(IUE_CHAR, cMade,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
        newItem = Items->CreateScriptItem(mySock, myChar, bpSectNumber, iAmount, itemType, bInPack,
                                          iColor);
    }
    else {
        newItem = Items->CreateBaseScriptItem(nullptr, bpSectNumber, worldNumber, iAmount,
                                              instanceId, itemType, iColor);
    }

    if (newItem != nullptr) {
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, newItem,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_CreateBlankItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          jsval *rval) {
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

    newItem = Items->CreateItem(mySock, myChar, itemId, amount, colour, itemType, inPack);
    if (newItem != nullptr) {
        if (itemName != "") {
            newItem->SetName(itemName);
        }
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, newItem,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }

    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);

    return JS_TRUE;
}

CMultiObj *BuildHouse(CSocket *s, std::uint16_t houseEntry, bool checkLocation = true, std::int16_t xLoc = -1,
                      std::int16_t yLoc = -1, std::int8_t zLoc = -1, std::uint8_t worldNumber = 0, std::uint16_t instanceId = 0);
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

        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, newMulti,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    else {
        *rval = JSVAL_NULL;
    }

    // Restore original script context and object
    JS_SetGlobalObject(origContext, origObject);
    return JS_TRUE;
}

CMultiObj *BuildBaseMulti(std::uint16_t multiId, std::int16_t xLoc = -1, std::int16_t yLoc = -1, std::int8_t zLoc = 127,
                          std::uint8_t worldNumber = 0, std::uint16_t instanceId = 0);
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

        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, newMulti,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_GetMurderThreshold([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                             [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv,
                             jsval *rval) {
    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->RepMaxKills());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RollDice()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Rolls a die with specified number of sides, and adds a fixed value
// o------------------------------------------------------------------------------------------------o
JSBool SE_RollDice([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                   jsval *argv, jsval *rval) {
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
JSBool SE_RaceCompareByRace([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                            uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    auto r0 = static_cast<raceid_t>(JSVAL_TO_INT(argv[0]));
    auto r1 = static_cast<raceid_t>(JSVAL_TO_INT(argv[1]));
    *rval = INT_TO_JSVAL(Races->CompareByRace(r0, r1));

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindMulti()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns multi at given coordinates, world and instanceId
// o------------------------------------------------------------------------------------------------o
JSBool SE_FindMulti(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                    jsval *rval) {
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
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, multi,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_GetItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                  jsval *rval) {
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
            JSEngine->AcquireObject(IUE_ITEM, item, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_FindItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                   jsval *rval) {
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
            JSEngine->AcquireObject(IUE_ITEM, item, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_CompareGuildByGuild([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                              uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    guildid_t toCheck = static_cast<guildid_t>(JSVAL_TO_INT(argv[0]));
    guildid_t toCheck2 = static_cast<guildid_t>(JSVAL_TO_INT(argv[1]));
    *rval = INT_TO_JSVAL(GuildSys->Compare(toCheck, toCheck2));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_PossessTown()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Source town takes control over target town
// o------------------------------------------------------------------------------------------------o
JSBool SE_PossessTown([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                      jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    std::uint16_t town = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::uint16_t sTown = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    cwmWorldState->townRegions[town]->Possess(cwmWorldState->townRegions[sTown]);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IsRaceWeakToWeather()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character's race is affected by given type of weather
// o------------------------------------------------------------------------------------------------o
JSBool SE_IsRaceWeakToWeather([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                              uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    auto race = static_cast<raceid_t>(JSVAL_TO_INT(argv[0]));
    auto toCheck = static_cast<weathid_t>(JSVAL_TO_INT(argv[1]));
    CRace *tRace = Races->Race(race);
    if (tRace == nullptr || toCheck >= WEATHNUM) {
        return JS_FALSE;
    }
    *rval = BOOLEAN_TO_JSVAL(tRace->AffectedBy(static_cast<weathertype_t>(toCheck)));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRaceSkillAdjustment()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage modifier for specified skill based on race
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetRaceSkillAdjustment([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                                 uintN argc, jsval *argv, jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    auto race = static_cast<raceid_t>(JSVAL_TO_INT(argv[0]));
    std::int32_t skill = JSVAL_TO_INT(argv[1]);
    *rval = INT_TO_JSVAL(Races->DamageFromSkill(skill, race));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_UseItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
// o------------------------------------------------------------------------------------------------o
JSBool SE_UseItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                  [[maybe_unused]] jsval *rval) {
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
        cScript *toExecute = JSMapping->GetScript(i);
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
        if (JSMapping->GetEnvokeByType()->Check(static_cast<std::uint16_t>(iType))) {
            envTrig = JSMapping->GetEnvokeByType()->GetScript(static_cast<std::uint16_t>(iType));
            toExecute = JSMapping->GetScript(envTrig);
        }
        else if (JSMapping->GetEnvokeById()->Check(itemId)) {
            envTrig = JSMapping->GetEnvokeById()->GetScript(itemId);
            toExecute = JSMapping->GetScript(envTrig);
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
JSBool SE_TriggerTrap(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                      [[maybe_unused]] jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "UseItem: Invalid number of arguments (takes 2 - socket/char, and item "
                        "(trap) triggered)");
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

    if (myItem->GetTempVar(CITV_MOREZ, 1) == 1 &&
        myItem->GetTempVar(CITV_MOREZ, 2) > 0) // Is trapped and set to deal more than 0 damage
    {
        Magic->MagicTrap(mChar, myItem);
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
    cScript *toExecute = JSMapping->GetScript(scriptNumberToFire);

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
JSBool SE_DoesEventExist(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         jsval *rval) {
    if (argc != 2) {
        return JS_FALSE;
    }

    *rval = INT_TO_JSVAL(1);
    std::uint16_t scriptNumberToCheck = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    char *eventToCheck = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    cScript *toExecute = JSMapping->GetScript(scriptNumberToCheck);

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
JSBool SE_GetPackOwner(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                       jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "GetPackOwner: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }

    std::uint8_t mType = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    CChar *pOwner = nullptr;

    if (mType == 0) // item
    {
        JSObject *mItem = JSVAL_TO_OBJECT(argv[0]);
        CItem *myItem = static_cast<CItem *>(JS_GetPrivate(cx, mItem));
        pOwner = FindItemOwner(myItem);
    }
    else // serial
    {
        std::int32_t mSerItem = JSVAL_TO_INT(argv[0]);
        pOwner = FindItemOwner(CalcItemObjFromSer(mSerItem));
    }
    if (ValidateObject(pOwner)) {
        JSObject *myObj = JSEngine->AcquireObject(IUE_CHAR, pOwner,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_FindRootContainer(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                            jsval *rval) {
    if (argc != 2) {
        ScriptError(cx, "FindRootContainer: Invalid number of arguments (takes 2)");
        return JS_FALSE;
    }

    std::uint8_t mType = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[1]));
    CItem *iRoot = nullptr;

    if (mType == 0) // item
    {
        JSObject *mItem = JSVAL_TO_OBJECT(argv[0]);
        CItem *myItem = static_cast<CItem *>(JS_GetPrivate(cx, mItem));
        iRoot = FindRootContainer(myItem);
    }
    else // serial
    {
        std::int32_t mSerItem = JSVAL_TO_INT(argv[0]);
        iRoot = FindRootContainer(CalcItemObjFromSer(mSerItem));
    }
    if (ValidateObject(iRoot)) {
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, iRoot,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_CalcTargetedItem(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           jsval *rval) {
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
        JSObject *myObj = JSEngine->AcquireObject(IUE_ITEM, calcedItem,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcTargetedChar()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted character stored on socket
// o------------------------------------------------------------------------------------------------o
JSBool SE_CalcTargetedChar(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           jsval *rval) {
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
        JSObject *myObj = JSEngine->AcquireObject(IUE_CHAR, calcedChar,
                                                  JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
        *rval = OBJECT_TO_JSVAL(myObj);
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTileIdAtMapCoord()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the map tile ID at given coordinates
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetTileIdAtMapCoord([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                              uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3) {
        ScriptError(cx, "GetTileIDAtMapCoord: Invalid number of arguments (takes 3)");
        return JS_FALSE;
    }

    std::uint16_t xLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    std::uint16_t yLoc = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t wrldNumber = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    auto mMap = Map->SeekMap(xLoc, yLoc, wrldNumber);
    *rval = INT_TO_JSVAL(mMap.tileId);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StaticInRange()
//|	Date		-	17th August, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static within specified range of given location
// o------------------------------------------------------------------------------------------------o
JSBool SE_StaticInRange([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                        jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(
            cx, "StaticInRange: Invalid number of arguments (takes 5, x, y, world, radius, tile)");
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
            auto artwork = Map->ArtAt(xLoc, yLoc, wrldNumber);
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
JSBool SE_StaticAt([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                   jsval *argv, jsval *rval) {
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

    auto artwork = Map->ArtAt(xLoc, yLoc, wrldNumber);
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
JSBool SE_StringToNum(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                      jsval *rval) {
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
JSBool SE_NumToHexString(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         jsval *rval) {
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
JSBool SE_GetRaceCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                       [[maybe_unused]] jsval *argv, jsval *rval) {
    if (argc != 0) {
        ScriptError(cx, "GetRaceCount: Invalid number of arguments (takes 0)");
        return JS_FALSE;
    }
    *rval = INT_TO_JSVAL(Races->Count());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_AreaCharacterFunction()
//|	Date		-	January 27, 2003
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area
// of characters
// o------------------------------------------------------------------------------------------------o
JSBool SE_AreaCharacterFunction(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                                jsval *argv, jsval *rval) {
    if (argc != 3 && argc != 4) {
        // function name, source character, range
        ScriptError(cx, "AreaCharacterFunction: Invalid number of arguments (takes 3/4, function "
                        "name, source character, range, optional socket)");
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
    R32 distance = static_cast<R32>(JSVAL_TO_INT(argv[2]));
    if (argc == 4 && argv[3] != JSVAL_NULL) {
        srcSocketObj = JSVAL_TO_OBJECT(argv[3]);
        srcSocket = static_cast<CSocket *>(JS_GetPrivate(cx, srcSocketObj));
    }

    std::vector<CChar *> charsFound;
    std::uint16_t retCounter = 0;
    cScript *myScript = JSMapping->GetScript(JS_GetGlobalObject(cx));
    for (auto &MapArea : MapRegion->PopulateList(srcObject)) {
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
        ScriptError(cx,
                    util::format("Critical error encountered in AreaObjFunc!", e.what()).c_str());
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
JSBool SE_AreaItemFunction(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           jsval *rval) {
    if (argc != 3 && argc != 4) {
        // function name, source character, range
        ScriptError(cx, "AreaItemFunction: Invalid number of arguments (takes 3/4, function name, "
                        "source object, range, optional socket)");
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
    R32 distance = static_cast<R32>(JSVAL_TO_INT(argv[2]));
    if (argc == 4 && argv[3] != JSVAL_NULL) {
        srcSocketObj = JSVAL_TO_OBJECT(argv[3]);
        if (srcSocketObj != nullptr) {
            srcSocket = static_cast<CSocket *>(JS_GetPrivate(cx, srcSocketObj));
        }
    }

    std::vector<CItem *> itemsFound;
    std::uint16_t retCounter = 0;
    cScript *myScript = JSMapping->GetScript(JS_GetGlobalObject(cx));
    for (auto &MapArea : MapRegion->PopulateList(srcObject)) {
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
    std::for_each(itemsFound.begin(), itemsFound.end(),
                  [myScript, trgFunc, srcObject, srcSocket, &retCounter](CItem *tempItem) {
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
JSBool SE_GetDictionaryEntry(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                             jsval *rval) {
    if (argc < 1) {
        ScriptError(cx, "GetDictionaryEntry: Invalid number of arguments (takes at least 1)");
        return JS_FALSE;
    }

    std::int32_t dictEntry = static_cast<std::int32_t>(JSVAL_TO_INT(argv[0]));
    unicodetypes_t language = ZERO;
    if (argc == 2) {
        language = static_cast<unicodetypes_t>(JSVAL_TO_INT(argv[1]));
    }
    std::string txt = Dictionary->GetEntry(dictEntry, language);
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
JSBool SE_Yell(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
               [[maybe_unused]] jsval *rval) {
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

    if (cwmWorldState->ServerData()->useUnicodeMessages()) {
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
        CSpeechEntry &toAdd = SpeechSys->Add();
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
        toAdd.At(cwmWorldState->GetUICurrentTime());
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
JSBool SE_Reload([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                 jsval *argv, [[maybe_unused]] jsval *rval) {
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
        Magic->LoadScript();
        break;
    case 3: // Reload serverCommands
        serverCommands.load();
        break;
    case 4: // Reload DFNs
        FileLookup->Reload();
        loadSkills();
        Skills->load();
        break;
    case 5: // Reload JScripts
        messageLoop << MSG_RELOADJS;
        break;
    case 6: // Reload HTMLTemplates
        HTMLTemplates->Unload();
        HTMLTemplates->load();
        break;
    case 7: // Reload INI
        cwmWorldState->ServerData()->load();
        break;
    case 8: // Reload Everything
        FileLookup->Reload();
        UnloadRegions();
        loadRegions();
        UnloadSpawnRegions();
        loadSpawnRegions();
        Magic->LoadScript();
        serverCommands.load();
        loadSkills();
        Skills->load();
        messageLoop << MSG_RELOADJS;
        HTMLTemplates->Unload();
        HTMLTemplates->load();
        cwmWorldState->ServerData()->load();
        break;
    case 9: // Reload Accounts
        Account::shared().load();
        break;
    case 10: // Reload Dictionaries
        Dictionary->LoadDictionaries();
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
JSBool SE_SendStaticStats(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          [[maybe_unused]] jsval *rval) {
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
            auto artwork = Map->ArtAt(targetX, targetY, worldNumber);
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
            auto map1 = Map->SeekMap(targetX, targetY, worldNumber);
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
JSBool SE_GetTileHeight([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                        jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "GetTileHeight: needs 1 argument!");
        return JS_FALSE;
    }

    std::uint16_t tileNum = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    *rval = INT_TO_JSVAL(Map->TileHeight(tileNum));
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
JSBool SE_IterateOver(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                      jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "IterateOver: needs 1 argument!");
        return JS_FALSE;
    }

    std::uint32_t b = 0;
    std::string objType = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
    auto toCheck = FindObjTypeFromString(objType);
    cScript *myScript = JSMapping->GetScript(JS_GetGlobalObject(cx));
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
JSBool SE_IterateOverSpawnRegions(JSContext *cx, [[maybe_unused]] JSObject *obj,
                                  [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv,
                                  jsval *rval) {
    std::uint32_t b = 0;
    cScript *myScript = JSMapping->GetScript(JS_GetGlobalObject(cx));

    if (myScript != nullptr) {
        std::for_each(cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(),
                      [&myScript, &b](std::pair<std::uint16_t, CSpawnRegion *> entry) {
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
JSBool SE_WorldBrightLevel([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                           uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx,
                    util::format("WorldBrightLevel: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        auto brightLevel = static_cast<lightlevel_t>(JSVAL_TO_INT(argv[0]));
        cwmWorldState->ServerData()->worldLightBrightLevel(brightLevel);
    }
    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->worldLightBrightLevel());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldDarkLevel()
//|	Date		-	18th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world dark level - darkest part of the regular
// day/night cycle
// o------------------------------------------------------------------------------------------------o
JSBool SE_WorldDarkLevel([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                         jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx,
                    util::format("WorldDarkLevel: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        auto darkLevel = static_cast<lightlevel_t>(JSVAL_TO_INT(argv[0]));
        cwmWorldState->ServerData()->worldLightDarkLevel(darkLevel);
    }
    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->worldLightDarkLevel());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldDungeonLevel()
//|	Date		-	18th July, 2004
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets default light level in dungeons
// o------------------------------------------------------------------------------------------------o
JSBool SE_WorldDungeonLevel([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                            uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(
            cx, util::format("WorldDungeonLevel: Unknown Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        auto dungeonLevel = static_cast<lightlevel_t>(JSVAL_TO_INT(argv[0]));
        cwmWorldState->ServerData()->dungeonLightLevel(dungeonLevel);
    }
    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->dungeonLightLevel());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegionFacetStatus()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets enabled state of given spawn region
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetSpawnRegionFacetStatus([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                                    uintN argc, jsval *argv, jsval *rval) {
    if (argc > 1) {
        ScriptError(cx,
                    util::format("GetSpawnRegionFacetStatus: Unknown Count of Arguments: %d", argc)
                        .c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        std::uint32_t spawnRegionFacet = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[0]));
        bool spawnRegionFacetStatus =
            cwmWorldState->ServerData()->getSpawnRegionsFacetStatus(spawnRegionFacet);
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
JSBool SE_SetSpawnRegionFacetStatus([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                                    uintN argc, jsval *argv, [[maybe_unused]] jsval *rval) {
    if (argc > 2) {
        ScriptError(cx,
                    util::format("SetSpawnRegionFacetStatus: Unknown Count of Arguments: %d", argc)
                        .c_str());
        return JS_FALSE;
    }
    else if (argc == 1) {
        std::uint32_t spawnRegionFacetVal = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[0]));
        cwmWorldState->ServerData()->setSpawnRegionsFacetStatus(spawnRegionFacetVal);
    }
    else if (argc == 2) {
        std::uint32_t spawnRegionFacet = static_cast<std::uint32_t>(JSVAL_TO_INT(argv[0]));
        bool spawnRegionFacetStatus = (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE);
        cwmWorldState->ServerData()->setSpawnRegionsFacetStatus(spawnRegionFacet,
                                                                spawnRegionFacetStatus);
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
JSBool SE_GetSocketFromIndex(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                             jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "GetSocketFromIndex: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    auto index = static_cast<uoxsocket_t>(JSVAL_TO_INT(argv[0]));

    CSocket *mSock = Network->GetSockPtr(index);
    CChar *mChar = nullptr;
    if (mSock != nullptr) {
        mChar = mSock->CurrcharObj();
    }

    if (!ValidateObject(mChar)) {
        *rval = JSVAL_NULL;
        return JS_FALSE;
    }

    JSObject *myObj =
        JSEngine->AcquireObject(IUE_CHAR, mChar, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
    *rval = OBJECT_TO_JSVAL(myObj);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ReloadJSFile()
//|	Date		-	5th February, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload specified JS file by scriptId
// o------------------------------------------------------------------------------------------------o
JSBool SE_ReloadJSFile(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                       [[maybe_unused]] jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "ReloadJSFile: Invalid number of arguments (takes 1)");
        return JS_FALSE;
    }
    std::uint16_t scriptId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    if (scriptId == JSMapping->GetScriptId(JS_GetGlobalObject(cx))) {
        ScriptError(
            cx,
            util::format(
                "ReloadJSFile: JS Script attempted to reload itself, crash avoided (ScriptID %u)",
                scriptId)
                .c_str());
        return JS_FALSE;
    }

    JSMapping->Reload(scriptId);

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceArea()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resource areas to split the world into
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceArea(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                       jsval *rval) {
    if (argc != 0) {
        ScriptError(cx, util::format("ResourceArea: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }

    auto resType = std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    resType = util::upper(util::trim(util::strip(resType, "//")));

    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResourceAreaSize());

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceAmount()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resources (logs/ore/fish) in each resource area
// on the server
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceAmount(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         jsval *rval) {
    if (argc > 2 || argc == 0) {
        ScriptError(cx,
                    util::format("ResourceAmount: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }
    auto resType = std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    resType = util::upper(util::trim(util::strip(resType, "//")));

    if (argc == 2) {
        std::int16_t newVal = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
        if (resType == "LOGS") {
            cwmWorldState->ServerData()->ResLogs(newVal);
        }
        else if (resType == "ORE") {
            cwmWorldState->ServerData()->ResOre(newVal);
        }
        else if (resType == "FISH") {
            cwmWorldState->ServerData()->ResFish(newVal);
        }
    }

    if (resType == "LOGS") {
        *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResLogs());
    }
    else if (resType == "ORE") {
        *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResOre());
    }
    else if (resType == "FISH") {
        *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResFish());
    }

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceTime()
//|	Date		-	18th September, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets respawn timers for ore/log resources on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_ResourceTime(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                       jsval *rval) {
    if (argc > 2 || argc == 0) {
        ScriptError(cx, util::format("ResourceTime: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }

    auto resType = std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    resType = util::upper(util::trim(util::strip(resType, "//")));
    if (argc == 2) {
        std::uint16_t newVal = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[1]));
        if (resType == "LOGS") {
            cwmWorldState->ServerData()->ResLogTime(newVal);
        }
        else if (resType == "ORE") {
            cwmWorldState->ServerData()->ResOreTime(newVal);
        }
        else if (resType == "FISH") {
            cwmWorldState->ServerData()->ResFishTime(newVal);
        }
    }

    if (resType == "LOGS") {
        *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResLogTime());
    }
    else if (resType == "ORE") {
        *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResOreTime());
    }
    else if (resType == "FISH") {
        *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->ResFishTime());
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
    MapResource_st *mRes = MapRegion->GetResource(x, y, worldNum);
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
JSBool SE_ValidateObject(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         jsval *rval) {
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
JSBool SE_ApplyDamageBonuses(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                             jsval *rval) {
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
    if (attackerClass.ClassName() != "UOXChar") // It must be a character!
    {
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
    if (defenderClass.ClassName() != "UOXChar") // It must be a character!
    {
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

    damage = Combat->ApplyDamageBonuses(static_cast<weathertype_t>(damageType.toInt()), attacker,
                                        defender, static_cast<std::uint8_t>(getFightSkill.toInt()),
                                        static_cast<std::uint8_t>(hitLoc.toInt()),
                                        static_cast<std::int16_t>(baseDamage.toInt()));

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
JSBool SE_ApplyDefenseModifiers(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                                jsval *argv, jsval *rval) {
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

    damage = Combat->ApplyDefenseModifiers(
        static_cast<weathertype_t>(damageType.toInt()), attacker, defender,
        static_cast<std::uint8_t>(getFightSkill.toInt()), static_cast<std::uint8_t>(hitLoc.toInt()),
        static_cast<std::int16_t>(baseDamage.toInt()), doArmorDamage.toBool());

    *rval = INT_TO_JSVAL(damage);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WillResultInCriminal()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if hostile action done by one character versus another will
// result in criminal flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_WillResultInCriminal(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                               jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(
            cx, "WillResultInCriminal: Invalid number of arguments (takes 2: srcChar and trgChar)");
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
JSBool SE_CreateParty(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                      jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "CreateParty: Invalid number of arguments (takes 1, the leader)");
        return JS_FALSE;
    }

    JSEncapsulate myClass(cx, &(argv[0]));

    if (myClass.ClassName() == "UOXChar" ||
        myClass.ClassName() == "UOXSocket") { // it's a character or socket, fantastic
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
            JSObject *myObj = JSEngine->AcquireObject(
                IUE_PARTY, tParty, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_Moon([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
               jsval *argv, jsval *rval) {
    if (argc > 2 || argc == 0) {
        ScriptError(cx, util::format("Moon: Invalid Count of Arguments: %d", argc).c_str());
        return JS_FALSE;
    }

    std::int16_t slot = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    if (argc == 2) {
        std::int16_t newVal = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
        cwmWorldState->ServerData()->serverMoon(slot, newVal);
    }

    *rval = INT_TO_JSVAL(cwmWorldState->ServerData()->serverMoon(slot));

    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTownRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified region object
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetTownRegion(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                        jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "GetTownRegion: Invalid number of parameters (1)");
        return JS_FALSE;
    }

    std::uint16_t regNum = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    if (cwmWorldState->townRegions.find(regNum) != cwmWorldState->townRegions.end()) {
        CTownRegion *townReg = cwmWorldState->townRegions[regNum];
        if (townReg != nullptr) {
            JSObject *myObj = JSEngine->AcquireObject(
                IUE_REGION, townReg, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
JSBool SE_GetSpawnRegion(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         jsval *rval) {
    if (argc != 1 && argc != 4) {
        ScriptError(cx, "GetSpawnRegion: Invalid number of parameters (1 - spawnRegionID, or 4 - "
                        "x, y, world and instanceID)");
        return JS_FALSE;
    }

    if (argc == 1) {
        // Assume spawn region number was provided
        std::uint16_t spawnRegNum = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
        if (cwmWorldState->spawnRegions.find(spawnRegNum) != cwmWorldState->spawnRegions.end()) {
            CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
            if (spawnReg != nullptr) {
                JSObject *myObj = JSEngine->AcquireObject(
                    IUE_SPAWNREGION, spawnReg, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
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
        auto iter = std::find_if(
            cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(),
            [&x, &y, &worldNum, &instanceID, &cx, &rval](std::pair<std::uint16_t, CSpawnRegion *> entry) {
                if (entry.second && x >= entry.second->GetX1() && x <= entry.second->GetX2() &&
                    y >= entry.second->GetY1() && y <= entry.second->GetY2() &&
                    entry.second->GetInstanceId() == instanceID &&
                    entry.second->WorldNumber() == worldNum) {
                    JSObject *myObj =
                        JSEngine->AcquireObject(IUE_SPAWNREGION, entry.second,
                                                JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
                    *rval = OBJECT_TO_JSVAL(myObj);
                    return true;
                }
                return false;
            });

        if (iter == cwmWorldState->spawnRegions.end()) {
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
JSBool SE_GetSpawnRegionCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                              uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    if (argc != 0) {
        ScriptError(cx, "GetSpawnRegionCount: Invalid number of arguments (takes 0)");
        return JS_FALSE;
    }
    *rval = INT_TO_JSVAL(cwmWorldState->spawnRegions.size());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMapElevation()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map elevation at given coordinates
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetMapElevation([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                          uintN argc, jsval *argv, jsval *rval) {
    if (argc != 3) {
        ScriptError(cx,
                    "GetMapElevation: Invalid number of arguments (takes 3: X, Y and WorldNumber)");
        return JS_FALSE;
    }

    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[2]));
    std::int8_t mapElevation = Map->MapElevation(x, y, worldNum);
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
JSBool SE_IsInBuilding([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                       jsval *argv, jsval *rval) {
    if (argc != 6) {
        ScriptError(cx, "IsInBuilding: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber "
                        "and instanceId)");
        return JS_FALSE;
    }

    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint16_t instanceId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[4]));
    bool checkHeight = (JSVAL_TO_BOOLEAN(argv[5]) == JS_TRUE);
    bool isInBuilding = Map->InBuilding(x, y, z, worldNum, instanceId);
    if (!isInBuilding) {
        // No static building was detected. How about a multi?
        CMultiObj *multi = FindMulti(x, y, z, worldNum, instanceId);
        if (ValidateObject(multi)) {
            if (checkHeight) {
                // Check if there's multi-items over the player's head
                std::int8_t multiZ = Map->MultiHeight(multi, x, y, z, 127, checkHeight);
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
JSBool SE_CheckStaticFlag([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                          uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "CheckStaticFlag: Invalid number of arguments (takes 5: X, Y, Z, "
                        "WorldNumber and TileFlagID)");
        return JS_FALSE;
    }

    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    auto toCheck = static_cast<tileflags_t>(JSVAL_TO_INT(argv[4]));
    [[maybe_unused]] std::uint16_t ignoreMe = 0;
    bool hasStaticFlag = Map->CheckStaticFlag(x, y, z, worldNum, toCheck, ignoreMe, false);
    *rval = BOOLEAN_TO_JSVAL(hasStaticFlag);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckDynamicFlag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a
// specific flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_CheckDynamicFlag([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                           uintN argc, jsval *argv, jsval *rval) {
    if (argc != 6) {
        ScriptError(cx, "CheckDynamicFlag: Invalid number of arguments (takes 6: X, Y, Z, "
                        "WorldNumber, instanceId and TileFlagID)");
        return JS_FALSE;
    }

    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t instanceId = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    auto toCheck = static_cast<tileflags_t>(JSVAL_TO_INT(argv[5]));
    [[maybe_unused]] std::uint16_t ignoreMe = 0;
    bool hasDynamicFlag = Map->CheckDynamicFlag(x, y, z, worldNum, instanceId, toCheck, ignoreMe);
    *rval = BOOLEAN_TO_JSVAL(hasDynamicFlag);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckTileFlag()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether tile with given ID has a specific flag
// o------------------------------------------------------------------------------------------------o
JSBool SE_CheckTileFlag([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                        jsval *argv, jsval *rval) {
    if (argc != 2) {
        ScriptError(cx,
                    "CheckTileFlag: Invalid number of arguments (takes 2: itemId and tileFlagID)");
        return JS_FALSE;
    }

    std::uint16_t itemId = static_cast<std::uint16_t>(JSVAL_TO_INT(argv[0]));
    auto flagToCheck = static_cast<tileflags_t>(JSVAL_TO_INT(argv[1]));

    bool tileHasFlag = Map->CheckTileFlag(itemId, flagToCheck);
    *rval = BOOLEAN_TO_JSVAL(tileHasFlag);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesStaticBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesStaticBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                          uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "DoesStaticBlock: Invalid number of arguments (takes 5: X, Y, Z, "
                        "WorldNumber and checkWater)");
        return JS_FALSE;
    }

    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    bool checkWater = (JSVAL_TO_BOOLEAN(argv[4]) == JS_TRUE);
    bool staticBlocks = Map->DoesStaticBlock(x, y, z, worldNum, checkWater);
    *rval = BOOLEAN_TO_JSVAL(staticBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesDynamicBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if dynamics at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesDynamicBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                           uintN argc, jsval *argv, jsval *rval) {
    if (argc != 9) {
        ScriptError(cx,
                    "DoesDynamicBlock: Invalid number of arguments (takes 9: X, Y, Z, WorldNumber, "
                    "instanceId, checkWater, waterWalk, checkOnlyMultis and checkOnlyNonMultis)");
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
    bool dynamicBlocks = Map->DoesDynamicBlock(x, y, z, worldNum, instanceId, checkWater, waterWalk,
                                               checkOnlyMultis, checkOnlyNonMultis);
    *rval = BOOLEAN_TO_JSVAL(dynamicBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesMapBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if map tile at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesMapBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc,
                       jsval *argv, jsval *rval) {
    if (argc != 8) {
        ScriptError(cx, "DoesMapBlock: Invalid number of arguments (takes 8: X, Y, Z, WorldNumber, "
                        "checkWater, waterWalk, checkMultiPlacement and checkForRoad)");
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
    bool mapBlocks = Map->DoesMapBlock(x, y, z, worldNum, checkWater, waterWalk,
                                       checkMultiPlacement, checkForRoad);
    *rval = BOOLEAN_TO_JSVAL(mapBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesCharacterBlock()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if characters at/above given coordinates blocks movement, etc
// o------------------------------------------------------------------------------------------------o
JSBool SE_DoesCharacterBlock([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                             uintN argc, jsval *argv, jsval *rval) {
    if (argc != 5) {
        ScriptError(cx, "DoesCharacterBlock: Invalid number of arguments (takes 5: X, Y, z, "
                        "WorldNumber, instanceId)");
        return JS_FALSE;
    }

    std::int16_t x = static_cast<std::int16_t>(JSVAL_TO_INT(argv[0]));
    std::int16_t y = static_cast<std::int16_t>(JSVAL_TO_INT(argv[1]));
    std::int8_t z = static_cast<std::int8_t>(JSVAL_TO_INT(argv[2]));
    std::uint8_t worldNum = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[3]));
    std::uint8_t instanceId = static_cast<std::uint8_t>(JSVAL_TO_INT(argv[4]));
    bool characterBlocks = Map->DoesCharacterBlock(x, y, z, worldNum, instanceId);
    *rval = BOOLEAN_TO_JSVAL(characterBlocks);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DeleteFile()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes a file from the server's harddrive...
// o------------------------------------------------------------------------------------------------o
JSBool SE_DeleteFile(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                     jsval *rval) {
    if (argc < 1 || argc > 3) {
        ScriptError(cx, "DeleteFile: Invalid number of arguments (takes 1 to 3 - fileName and "
                        "(optionally) subFolderName and useScriptDataDir bool)");
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

    std::string pathString = cwmWorldState->ServerData()->Directory(CSDDP_SHARED);

    // if subFolderName argument was supplied, use it
    if (subFolderName != nullptr) {
        // However, don't allow special characters in the folder name
        if (strstr(subFolderName, "..") || strstr(subFolderName, "\\") ||
            strstr(subFolderName, "/")) {
            ScriptError(
                cx, "DeleteFile: folder names may not contain \".\", \"..\", \"\\\", or \"/\".");
            return JS_FALSE;
        }

        // If script wants to look in script-data folder instead of shared folder, let it
        if (useScriptDataDir) {
            pathString = cwmWorldState->ServerData()->Directory(CSDDP_SCRIPTDATA);
        }

        // Append subfolder name to path
        pathString.append(subFolderName);

        if (!std::filesystem::exists(pathString)) {
            // Return JS_TRUE to allow script to continue running even if file was not found for
            // deletion, but set return value to false
            *rval = false;
            return JS_TRUE;
        }

        pathString += std::filesystem::path::preferred_separator;
    }

    pathString.append(fileName);

    std::filesystem::path filePath = pathString;
    *rval = std::filesystem::remove(filePath);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EraStringToNum()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts an UO era string to an int value for easier comparison in
// JavaScripts
// o------------------------------------------------------------------------------------------------o
JSBool SE_EraStringToNum(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                         jsval *rval) {
    *rval = reinterpret_cast<long>(nullptr);

    if (argc != 1) {
        ScriptError(cx, "EraStringToNum: Invalid number of arguments (takes 1 - era string)");
        return JS_FALSE;
    }

    std::string eraString = util::upper(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    if (!eraString.empty()) {
        std::uint8_t eraNum = static_cast<std::uint8_t>(
            cwmWorldState->ServerData()->EraStringToEnum(eraString, false, false));
        if (eraNum != 0) {
            *rval = INT_TO_JSVAL(eraNum);
        }
        else {
            ScriptError(cx, "EraStringToNum: Provided argument not valid era string (uo, t2a, uor, "
                            "td, lbr, aos, se, ml, sa, hs or tol)");
            return JS_FALSE;
        }
    }
    else {
        ScriptError(cx, "EraStringToNum: Provided argument contained no valid string data");
        return JS_FALSE;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerSetting()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets value of specified server setting
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetServerSetting(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                           jsval *rval) {
    *rval = reinterpret_cast<long>(nullptr);

    if (argc != 1) {
        ScriptError(cx,
                    "GetServerSetting: Invalid number of arguments (takes 1 - serverSettingName)");
        return JS_FALSE;
    }

    JSString *tString;
    std::string settingName = util::upper(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
    if (!settingName.empty()) {
        auto settingId = cwmWorldState->ServerData()->lookupINIValue(settingName);
        switch (settingId) {
        case 1: // SERVERNAME
        {
            std::string tempString = {cwmWorldState->ServerData()->ServerName()};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 2: // CONSOLELOG
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerConsoleLog());
            break;
        case 3: // COMMANDPREFIX
        {
            std::string tempString = {cwmWorldState->ServerData()->ServerCommandPrefix()};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 4: // ANNOUNCEWORLDSAVES
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerAnnounceSavesStatus());
            break;
        case 26: // JOINPARTMSGS
            *rval =
                BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus());
            break;
        case 5: // BACKUPSENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerBackupStatus());
            break;
        case 6: // SAVESTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint32_t>(cwmWorldState->ServerData()->ServerSavesTimerStatus()));
            break;
        case 7: // SKILLCAP
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->ServerSkillTotalCapStatus()));
            break;
        case 8: // SKILLDELAY
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->ServerSkillDelayStatus()));
            break;
        case 9: // STATCAP
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ServerStatCapStatus()));
            break;
        case 10: // MAXSTEALTHMOVEMENTS
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->MaxStealthMovement()));
            break;
        case 11: // MAXSTAMINAMOVEMENTS
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->MaxStaminaMovement()));
            break;
        case 12: // ARMORAFFECTMANAREGEN
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ArmorAffectManaRegen());
            break;
        case 13: // CORPSEDECAYTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_CORPSEDECAY)));
            break;
        case 14: // WEATHERTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_WEATHER)));
            break;
        case 15: // SHOPSPAWNTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_SHOPSPAWN)));
            break;
        case 16: // DECAYTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_DECAY)));
            break;
        case 17: // INVISIBILITYTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_INVISIBILITY)));
            break;
        case 18: // OBJECTUSETIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_OBJECTUSAGE)));
            break;
        case 19: // GATETIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_GATE)));
            break;
        case 20: // POISONTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_POISON)));
            break;
        case 21: // LOGINTIMEOUT
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_LOGINTIMEOUT)));
            break;
        case 22: // HITPOINTREGENTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_HITPOINTREGEN)));
            break;
        case 23: // STAMINAREGENTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_STAMINAREGEN)));
            break;
        case 37: // MANAREGENTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_MANAREGEN)));
            break;
        case 24: // BASEFISHINGTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_FISHINGBASE)));
            break;
        case 34: // MAXPETOWNERS
            *rval = INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->MaxPetOwners()));
            break;
        case 35: // MAXFOLLOWERS
            *rval = INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->MaxFollowers()));
            break;
        case 36: // MAXCONTROLSLOTS
            *rval = INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->MaxControlSlots()));
            break;
        case 38: // RANDOMFISHINGTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_FISHINGRANDOM)));
            break;
        case 39: // SPIRITSPEAKTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_SPIRITSPEAK)));
            break;
        case 40: // DIRECTORY
        {
            JSString *tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_ROOT).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 41: // DATADIRECTORY
        {
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_DATA).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 42: // DEFSDIRECTORY
        {
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_DEFS).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 43: // ACTSDIRECTORY
        {
            tString = JS_NewStringCopyZ(
                cx, cwmWorldState->ServerData()->Directory(CSDDP_ACCOUNTS).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 25: // SCRIPTSDIRECTORY
        {
            tString = JS_NewStringCopyZ(
                cx, cwmWorldState->ServerData()->Directory(CSDDP_SCRIPTS).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 44: // BACKUPDIRECTORY
        {
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_BACKUP).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 45: // MSGBOARDDIRECTORY
        {
            tString = JS_NewStringCopyZ(
                cx, cwmWorldState->ServerData()->Directory(CSDDP_MSGBOARD).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 46: // SHAREDDIRECTORY
        {
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_SHARED).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 47: // LOOTDECAYSWITHCORPSE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CorpseLootDecay());
            break;
        case 49: // GUARDSACTIVE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->GuardsStatus());
            break;
        case 27: // DEATHANIMATION
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->DeathAnimationStatus());
            break;
        case 50: // AMBIENTSOUNDS
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->WorldAmbientSounds()));
            break;
        case 51: // AMBIENTFOOTSTEPS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->AmbientFootsteps());
            break;
        case 52: // INTERNALACCOUNTCREATION
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->InternalAccountStatus());
            break;
        case 53: // SHOWOFFLINEPCS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowOfflinePCs());
            break;
        case 54: // ROGUESENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->RogueStatus());
            break;
        case 55: // PLAYERPERSECUTION
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->PlayerPersecutionStatus());
            break;
        case 56: // ACCOUNTFLUSH
            *rval =
                INT_TO_JSVAL(static_cast<R64>(cwmWorldState->ServerData()->AccountFlushTimer()));
            break;
        case 57: // HTMLSTATUSENABLED
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->HtmlStatsStatus()));
            break;
        case 58: // SELLBYNAME
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->SellByNameStatus());
            break;
        case 59: // SELLMAXITEMS
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->SellMaxItemsStatus()));
            break;
        case 60: // TRADESYSTEM
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TradeSystemStatus());
            break;
        case 61: // RANKSYSTEM
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->RankSystemStatus());
            break;
        case 62: // CUTSCROLLREQUIREMENTS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CutScrollRequirementStatus());
            break;
        case 63: // CHECKITEMS
            *rval = INT_TO_JSVAL(static_cast<R64>(cwmWorldState->ServerData()->CheckItemsSpeed()));
            break;
        case 64: // CHECKBOATS
            *rval = INT_TO_JSVAL(static_cast<R64>(cwmWorldState->ServerData()->CheckBoatSpeed()));
            break;
        case 65: // CHECKNPCAI
            *rval = INT_TO_JSVAL(static_cast<R64>(cwmWorldState->ServerData()->CheckNpcAISpeed()));
            break;
        case 66: // CHECKSPAWNREGIONS
            *rval = INT_TO_JSVAL(
                static_cast<R64>(cwmWorldState->ServerData()->CheckSpawnRegionSpeed()));
            break;
        case 67: // POSTINGLEVEL
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->MsgBoardPostingLevel()));
            break;
        case 68: // REMOVALLEVEL
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->MsgBoardPostRemovalLevel()));
            break;
        case 69: // ESCORTENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->EscortsEnabled());
            break;
        case 70: // ESCORTINITEXPIRE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_ESCORTWAIT)));
            break;
        case 71: // ESCORTACTIVEEXPIRE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_ESCORTACTIVE)));
            break;
        case 72: // MOON1
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->serverMoon(0)));
            break;
        case 73: // MOON2
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->serverMoon(1)));
            break;
        case 74: // DUNGEONLEVEL
            *rval = INT_TO_JSVAL(
                static_cast<lightlevel_t>(cwmWorldState->ServerData()->dungeonLightLevel()));
            break;
        case 75: // CURRENTLEVEL
            *rval = INT_TO_JSVAL(
                static_cast<lightlevel_t>(cwmWorldState->ServerData()->worldLightCurrentLevel()));
            break;
        case 76: // BRIGHTLEVEL
            *rval = INT_TO_JSVAL(
                static_cast<lightlevel_t>(cwmWorldState->ServerData()->worldLightBrightLevel()));
            break;
        case 77: // BASERANGE
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->TrackingBaseRange()));
            break;
        case 78: // BASETIMER
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->TrackingBaseTimer()));
            break;
        case 79: // MAXTARGETS
            *rval =
                INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->TrackingMaxTargets()));
            break;
        case 80: // MSGREDISPLAYTIME
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->TrackingRedisplayTime()));
            break;
        case 81: // MURDERDECAYTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_MURDERDECAY)));
            break;
        case 82: // MAXKILLS
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->RepMaxKills()));
            break;
        case 83: // CRIMINALTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_CRIMINAL)));
            break;
        case 84: // MINECHECK
            *rval = INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->MineCheck()));
            break;
        case 85: // OREPERAREA
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->ResOre()));
            break;
        case 86: // ORERESPAWNTIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ResOreTime()));
            break;
        case 87: // RESOURCEAREASIZE
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ResourceAreaSize()));
            break;
        case 88: // LOGSPERAREA
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->ResLogs()));
            break;
        case 89: // LOGSRESPAWNTIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ResLogTime()));
            break;
        case 90: // STATSAFFECTSKILLCHECKS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->StatsAffectSkillChecks());
            break;
        case 91: // HUNGERRATE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_HUNGERRATE)));
            break;
        case 92: // HUNGERDMGVAL
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->HungerDamage()));
            break;
        case 93: // MAXRANGE
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatMaxRange()));
            break;
        case 94: // SPELLMAXRANGE
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatMaxSpellRange()));
            break;
        case 95: // DISPLAYHITMSG
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CombatDisplayHitMessage());
            break;
        case 96: // MONSTERSVSANIMALS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CombatMonstersVsAnimals());
            break;
        case 97: // ANIMALATTACKCHANCE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->CombatAnimalsAttackChance()));
            break;
        case 98: // ANIMALSGUARDED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CombatAnimalsGuarded());
            break;
        case 99: // NPCDAMAGERATE
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatNpcDamageRate()));
            break;
        case 100: // NPCBASEFLEEAT
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatNPCBaseFleeAt()));
            break;
        case 101: // NPCBASEREATTACKAT
            *rval = INT_TO_JSVAL(
                static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatNPCBaseReattackAt()));
            break;
        case 102: // ATTACKSTAMINA
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatAttackStamina()));
            break;
        // case 103:	 // LOCATION
        // break;
        case 104: // STARTGOLD
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->ServerStartGold()));
            break;
        case 105: // STARTPRIVS
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->serverStartPrivs()));
            break;
        case 106: // ESCORTDONEEXPIRE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_ESCORTDONE)));
            break;
        case 107: // DARKLEVEL
            *rval = INT_TO_JSVAL(
                static_cast<lightlevel_t>(cwmWorldState->ServerData()->worldLightDarkLevel()));
            break;
        case 108: // TITLECOLOUR
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->TitleColour()));
            break;
        case 109: // LEFTTEXTCOLOUR
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->LeftTextColour()));
            break;
        case 110: // RIGHTTEXTCOLOUR
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->RightTextColour()));
            break;
        case 111: // BUTTONCANCEL
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ButtonCancel()));
            break;
        case 112: // BUTTONLEFT
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ButtonLeft()));
            break;
        case 113: // BUTTONRIGHT
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ButtonRight()));
            break;
        case 114: // BACKGROUNDPIC
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->BackgroundPic()));
            break;
        case 115: // POLLTIME
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->TownNumSecsPollOpen()));
            break;
        case 116: // MAYORTIME
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->TownNumSecsAsMayor()));
            break;
        case 117: // TAXPERIOD
            *rval = INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->TownTaxPeriod()));
            break;
        case 118: // GUARDSPAID
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->TownGuardPayment()));
            break;
        case 119: // DAY
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->ServerTimeDay()));
            break;
        case 120: // HOURS
            *rval = INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->ServerTimeHours()));
            break;
        case 121: // MINUTES
            *rval =
                INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->ServerTimeMinutes()));
            break;
        case 122: // SECONDS
            *rval =
                INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->ServerTimeSeconds()));
            break;
        case 123: // AMPM
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerTimeAMPM());
            break;
        case 124: // SKILLLEVEL
            *rval = INT_TO_JSVAL(static_cast<std::uint8_t>(cwmWorldState->ServerData()->SkillLevel()));
            break;
        case 125: // SNOOPISCRIME
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->SnoopIsCrime());
            break;
        case 126: // BOOKSDIRECTORY
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_BOOKS).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        // case 127:	 // SERVERLIST
        // break;
        case 128: // PORT
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ServerPort()));
            break;
        case 129: // ACCESSDIRECTORY
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_ACCESS).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        case 130: // LOGSDIRECTORY
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_LOGS).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        case 132: // HTMLDIRECTORY
            tString =
                JS_NewStringCopyZ(cx, cwmWorldState->ServerData()->Directory(CSDDP_HTML).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        case 133: // SHOOTONANIMALBACK
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShootOnAnimalBack());
            break;
        case 134: // NPCTRAININGENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->NPCTrainingStatus());
            break;
        case 135: // DICTIONARYDIRECTORY
            tString = JS_NewStringCopyZ(
                cx, cwmWorldState->ServerData()->Directory(CSDDP_DICTIONARIES).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        case 136: // BACKUPSAVERATIO
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->BackupRatio()));
            break;
        case 137: // HIDEWHILEMOUNTED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CharHideWhileMounted());
            break;
        case 138: // SECONDSPERUOMINUTE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->ServerSecondsPerUOMinute()));
            break;
        case 139: // WEIGHTPERSTR
            *rval = INT_TO_JSVAL(static_cast<R32>(cwmWorldState->ServerData()->WeightPerStr()));
            break;
        case 140: // POLYDURATION
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_POLYMORPH)));
            break;
        case 141: // UOGENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerUOGEnabled());
            break;
        case 142: // NETRCVTIMEOUT
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->ServerNetRcvTimeout()));
            break;
        case 143: // NETSNDTIMEOUT
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->ServerNetSndTimeout()));
            break;
        case 144: // NETRETRYCOUNT
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->ServerNetRetryCount()));
            break;
        case 145: // CLIENTFEATURES
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->getClientFeatures()));
            break;
        case 146: // PACKETOVERLOADS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerOverloadPackets());
            break;
        case 147: // NPCMOVEMENTSPEED
            *rval = INT_TO_JSVAL(static_cast<R32>(cwmWorldState->ServerData()->NPCWalkingSpeed()));
            break;
        case 148: // PETHUNGEROFFLINE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->PetHungerOffline());
            break;
        case 149: // PETOFFLINETIMEOUT
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->PetOfflineTimeout()));
            break;
        case 150: // PETOFFLINECHECKTIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(
                static_cast<csd_tid_t>(tSERVER_PETOFFLINECHECK))));
            break;
        case 152: // ADVANCEDPATHFINDING
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->AdvancedPathfinding());
            break;
        case 153: // SERVERFEATURES
            *rval =
                INT_TO_JSVAL(static_cast<size_t>(cwmWorldState->ServerData()->getServerFeatures()));
            break;
        case 154: // LOOTINGISCRIME
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->LootingIsCrime());
            break;
        case 155: // NPCRUNNINGSPEED
            *rval = INT_TO_JSVAL(static_cast<R32>(cwmWorldState->ServerData()->NPCRunningSpeed()));
            break;
        case 156: // NPCFLEEINGSPEED
            *rval = INT_TO_JSVAL(static_cast<R32>(cwmWorldState->ServerData()->NPCFleeingSpeed()));
            break;
        case 157: // BASICTOOLTIPSONLY
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->BasicTooltipsOnly());
            break;
        case 158: // GLOBALITEMDECAY
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->GlobalItemDecay());
            break;
        case 159: // SCRIPTITEMSDECAYABLE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ScriptItemsDecayable());
            break;
        case 160: // BASEITEMSDECAYABLE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->BaseItemsDecayable());
            break;
        case 161: // ITEMDECAYINHOUSES
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ItemDecayInHouses());
            break;
        case 162: // SPAWNREGIONSFACETS
            *rval = INT_TO_JSVAL(
                static_cast<std::uint32_t>(cwmWorldState->ServerData()->getSpawnRegionsFacetStatus()));
            break;
        case 163: // PAPERDOLLGUILDBUTTON
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->PaperdollGuildButton());
            break;
        case 164: // ATTACKSPEEDFROMSTAMINA
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CombatAttackSpeedFromStamina());
            break;
        case 169: // DISPLAYDAMAGENUMBERS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CombatDisplayDamageNumbers());
            break;
        case 170: // CLIENTSUPPORT4000
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport4000());
            break;
        case 171: // CLIENTSUPPORT5000
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport5000());
            break;
        case 172: // CLIENTSUPPORT6000
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport6000());
            break;
        case 173: // CLIENTSUPPORT6050
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport6050());
            break;
        case 174: // CLIENTSUPPORT7000
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport7000());
            break;
        case 175: // CLIENTSUPPORT7090
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport7090());
            break;
        case 176: // CLIENTSUPPORT70160
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport70160());
            break;
        case 177: // CLIENTSUPPORT70240
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport70240());
            break;
        case 178: // CLIENTSUPPORT70300
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport70300());
            break;
        case 179: // CLIENTSUPPORT70331
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport70331());
            break;
        case 180: // CLIENTSUPPORT704565
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport704565());
            break;
        case 181: // CLIENTSUPPORT70610
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->clientSupport70610());
            break;
        case 182: // EXTENDEDSTARTINGSTATS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ExtendedStartingStats());
            break;
        case 183: // EXTENDEDSTARTINGSKILLS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ExtendedStartingSkills());
            break;
        case 184: // WEAPONDAMAGECHANCE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatWeaponDamageChance()));
            break;
        case 185: // ARMORDAMAGECHANCE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatArmorDamageChance()));
            break;
        case 186: // WEAPONDAMAGEMIN
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatWeaponDamageMin()));
            break;
        case 187: // WEAPONDAMAGEMAX
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatWeaponDamageMax()));
            break;
        case 188: // ARMORDAMAGEMIN
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatArmorDamageMin()));
            break;
        case 189: // ARMORDAMAGEMAX
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatArmorDamageMax()));
            break;
        case 190: // GLOBALATTACKSPEED
            *rval =
                INT_TO_JSVAL(static_cast<R32>(cwmWorldState->ServerData()->GlobalAttackSpeed()));
            break;
        case 191: // NPCSPELLCASTSPEED
            *rval =
                INT_TO_JSVAL(static_cast<R32>(cwmWorldState->ServerData()->NPCSpellCastSpeed()));
            break;
        case 192: // FISHINGSTAMINALOSS
            *rval =
                INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->FishingStaminaLoss()));
            break;
        case 193: // RANDOMSTARTINGLOCATION
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerRandomStartingLocation());
            break;
        case 194: // ASSISTANTNEGOTIATION
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->getAssistantNegotiation());
            break;
        case 195: // KICKONASSISTANTSILENCE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->KickOnAssistantSilence());
            break;
        case 196: // AF_FILTERWEATHER
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_FILTERWEATHER));
            break;
        case 197: // AF_FILTERLIGHT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_FILTERLIGHT));
            break;
        case 198: // AF_SMARTTARGET
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_SMARTTARGET));
            break;
        case 199: // AF_RANGEDTARGET
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_RANGEDTARGET));
            break;
        case 200: // AF_AUTOOPENDOORS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_AUTOOPENDOORS));
            break;
        case 201: // AF_DEQUIPONCAST
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_DEQUIPONCAST));
            break;
        case 202: // AF_AUTOPOTIONEQUIP
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_AUTOPOTIONEQUIP));
            break;
        case 203: // AF_POISONEDCHECKS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_POISONEDCHECKS));
            break;
        case 204: // AF_LOOPEDMACROS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_LOOPEDMACROS));
            break;
        case 205: // AF_USEONCEAGENT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_USEONCEAGENT));
            break;
        case 206: // AF_RESTOCKAGENT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_RESTOCKAGENT));
            break;
        case 207: // AF_SELLAGENT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_SELLAGENT));
            break;
        case 208: // AF_BUYAGENT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_BUYAGENT));
            break;
        case 209: // AF_POTIONHOTKEYS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_POTIONHOTKEYS));
            break;
        case 210: // AF_RANDOMTARGETS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_RANDOMTARGETS));
            break;
        case 211: // AF_CLOSESTTARGETS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_CLOSESTTARGETS));
            break;
        case 212: // AF_OVERHEADHEALTH
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_OVERHEADHEALTH));
            break;
        case 213: // AF_AUTOLOOTAGENT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_AUTOLOOTAGENT));
            break;
        case 214: // AF_BONECUTTERAGENT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_BONECUTTERAGENT));
            break;
        case 215: // AF_JSCRIPTMACROS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_JSCRIPTMACROS));
            break;
        case 216: // AF_AUTOREMOUNT
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_AUTOREMOUNT));
            break;
        case 217: // AF_ALL
            *rval =
                BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_ALL));
            break;
        case 218: // CLASSICUOMAPTRACKER
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->getClassicUOMapTracker());
            break;
        case 219: // DECAYTIMERINHOUSE
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(
                static_cast<csd_tid_t>(tSERVER_DECAYINHOUSE))));
            break;
        case 220: // PROTECTPRIVATEHOUSES
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ProtectPrivateHouses());
            break;
        case 221: // TRACKHOUSESPERACCOUNT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TrackHousesPerAccount());
            break;
        case 222: // MAXHOUSESOWNABLE
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->MaxHousesOwnable()));
            break;
        case 223: // MAXHOUSESCOOWNABLE
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->MaxHousesCoOwnable()));
            break;
        case 224: // CANOWNANDCOOWNHOUSES
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CanOwnAndCoOwnHouses());
            break;
        case 225: // COOWNHOUSESONSAMEACCOUNT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CoOwnHousesOnSameAccount());
            break;
        case 226: // ITEMSDETECTSPEECH
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ItemsDetectSpeech());
            break;
        case 227: // MAXPLAYERPACKITEMS
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->MaxPlayerPackItems()));
            break;
        case 228: // MAXPLAYERBANKITEMS
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->MaxPlayerBankItems()));
            break;
        case 229: // FORCENEWANIMATIONPACKET
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ForceNewAnimationPacket());
            break;
        case 230: // MAPDIFFSENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->MapDiffsEnabled());
            break;
        case 231: // CORESHARDERA
        {
            std::string tempString = {cwmWorldState->ServerData()->EraEnumToString(
                static_cast<expansionruleset_t>(cwmWorldState->ServerData()->ExpansionCoreShardEra()),
                true)};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 232: // ARMORCALCULATION
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionArmorCalculation()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 233: // STRENGTHDAMAGEBONUS
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionStrengthDamageBonus()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 234: // TACTICSDAMAGEBONUS
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionTacticsDamageBonus()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 235: // ANATOMYDAMAGEBONUS
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionAnatomyDamageBonus()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 236: // LUMBERJACKDAMAGEBONUS
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionLumberjackDamageBonus()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 237: // RACIALDAMAGEBONUS
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionRacialDamageBonus()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 238: // DAMAGEBONUSCAP
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionDamageBonusCap()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 239: // SHIELDPARRY
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionShieldParry()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 240: // PARRYDAMAGECHANCE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatParryDamageChance()));
            break;
        case 241: // PARRYDAMAGEMIN
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->CombatParryDamageMin()));
            break;
        case 242: // PARRYDAMAGEMAX
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->CombatParryDamageMax()));
            break;
        case 243: // ARMORCLASSDAMAGEBONUS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CombatArmorClassDamageBonus());
            break;
        case 244: // FREESHARDSERVERPOLL
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->FreeshardServerPoll());
            break;
        case 245: // ALCHEMYBONUSENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->AlchemyDamageBonusEnabled());
            break;
        case 246: // ALCHEMYBONUSMODIFIER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->AlchemyDamageBonusModifier()));
            break;
        case 247: // NPCFLAGUPDATETIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(
                static_cast<csd_tid_t>(tSERVER_NPCFLAGUPDATETIMER))));
            break;
        case 248: // JSENGINESIZE
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->GetJSEngineSize()));
            break;
        case 249: // USEUNICODEMESSAGES
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->useUnicodeMessages()));
            break;
        case 250: // SCRIPTDATADIRECTORY
        {
            tString = JS_NewStringCopyZ(
                cx, cwmWorldState->ServerData()->Directory(CSDDP_SCRIPTDATA).c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 251: // THIRSTRATE
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(
                static_cast<csd_tid_t>(tSERVER_THIRSTRATE))));
            break;
        case 252: // THIRSTDRAINVAL
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->ThirstDrain()));
            break;
        case 253: // PETTHIRSTOFFLINE
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->PetThirstOffline());
            break;
        case 255: // BLOODDECAYTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_BLOODDECAY)));
            break;
        case 256: // BLOODDECAYCORPSETIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(
                cwmWorldState->ServerData()->SystemTimer(tSERVER_BLOODDECAYCORPSE)));
            break;
        case 257: // BLOODEFFECTCHANCE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->CombatBloodEffectChance()));
            break;
        case 258: // NPCCORPSEDECAYTIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(
                cwmWorldState->ServerData()->SystemTimer(tSERVER_NPCCORPSEDECAY)));
            break;
        case 259: // HUNGERENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->HungerSystemEnabled());
            break;
        case 260: // THIRSTENABLED
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ThirstSystemEnabled());
            break;
        case 261: // TRAVELSPELLSFROMBOATKEYS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TravelSpellsFromBoatKeys());
            break;
        case 262: // TRAVELSPELLSWHILEOVERWEIGHT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TravelSpellsWhileOverweight());
            break;
        case 263: // MARKRUNESINMULTIS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->MarkRunesInMultis());
            break;
        case 264: // TRAVELSPELLSBETWEENWORLDS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TravelSpellsBetweenWorlds());
            break;
        case 265: // TRAVELSPELLSWHILEAGGRESSOR
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TravelSpellsWhileAggressor());
            break;
        case 266: // BANKBUYTHRESHOLD
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->BuyThreshold()));
            break;
        case 267: // NETWORKLOG
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerNetworkLog());
            break;
        case 268: // SPEECHLOG
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerSpeechLog());
            break;
        case 269: // NPCMOUNTEDWALKINGSPEED
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->NPCMountedWalkingSpeed()));
            break;
        case 270: // NPCMOUNTEDRUNNINGSPEED
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->NPCMountedRunningSpeed()));
            break;
        case 271: // NPCMOUNTEDFLEEINGSPEED
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->NPCMountedFleeingSpeed()));
            break;
        case 272: // CONTEXTMENUS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ServerContextMenus());
            break;
        case 273: // SERVERLANGUAGE
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ServerLanguage()));
            break;
        case 274: // CHECKPETCONTROLDIFFICULTY
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CheckPetControlDifficulty());
            break;
        case 275: // PETLOYALTYGAINONSUCCESS
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->GetPetLoyaltyGainOnSuccess()));
            break;
        case 276: // PETLOYALTYLOSSONFAILURE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->GetPetLoyaltyLossOnFailure()));
            break;
        case 277: // PETLOYALTYRATE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_LOYALTYRATE)));
            break;
        case 278: // SHOWNPCTITLESINTOOLTIPS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowNpcTitlesInTooltips());
            break;
        case 279: // FISHPERAREA
            *rval = INT_TO_JSVAL(static_cast<std::int16_t>(cwmWorldState->ServerData()->ResFish()));
            break;
        case 280: // FISHRESPAWNTIMER
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->ResFishTime()));
            break;
        case 281: // ARCHERYHITBONUS
            *rval = INT_TO_JSVAL(
                static_cast<std::int16_t>(cwmWorldState->ServerData()->CombatArcheryHitBonus()));
            break;
        case 282: // ITEMSINTERRUPTCASTING
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ItemsInterruptCasting());
            break;
        case 283: // SYSMESSAGECOLOUR
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->SysMsgColour()));
            break;
        case 284: // AF_AUTOBANDAGE
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_AUTOBANDAGE));
            break;
        case 285: // AF_ENEMYTARGETSHARE
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_ENEMYTARGETSHARE));
            break;
        case 286: // AF_FILTERSEASON
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_FILTERSEASON));
            break;
        case 287: // AF_SPELLTARGETSHARE
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_SPELLTARGETSHARE));
            break;
        case 288: // AF_HUMANOIDHEALTHCHECKS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_HUMANOIDHEALTHCHECKS));
            break;
        case 289: // AF_SPEECHJOURNALCHECKS
            *rval = BOOLEAN_TO_JSVAL(
                cwmWorldState->ServerData()->getDisabledAssistantFeature(AF_SPEECHJOURNALCHECKS));
            break;
        case 290: // ARCHERYSHOOTDELAY
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->CombatArcheryShootDelay()));
            break;
        case 291: // MAXCLIENTBYTESIN
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->MaxClientBytesIn()));
            break;
        case 292: // MAXCLIENTBYTESOUT
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->MaxClientBytesOut()));
            break;
        case 293: // NETTRAFFICTIMEBAN
            *rval =
                INT_TO_JSVAL(static_cast<std::uint32_t>(cwmWorldState->ServerData()->NetTrafficTimeban()));
            break;
        case 294: // TOOLUSELIMIT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ToolUseLimit());
            break;
        case 295: // TOOLUSEBREAK
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ToolUseBreak());
            break;
        case 296: // ITEMREPAIRDURABILITYLOSS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ItemRepairDurabilityLoss());
            break;
        case 297: // HIDESTATSFORUNKNOWNMAGICITEMS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->HideStatsForUnknownMagicItems());
            break;
        case 298: // CRAFTCOLOUREDWEAPONS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CraftColouredWeapons());
            break;
        case 299: // MAXSAFETELEPORTSPERDAY
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->MaxSafeTeleportsPerDay()));
            break;
        case 300: // TELEPORTONEARESTSAFELOCATION
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->TeleportToNearestSafeLocation());
            break;
        case 301: // ALLOWAWAKENPCS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->AllowAwakeNPCs());
            break;
        case 302: // DISPLAYMAKERSMARK
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->DisplayMakersMark());
            break;
        case 303: // SHOWNPCTITLESOVERHEAD
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowNpcTitlesOverhead());
            break;
        case 304: // SHOWINVULNERABLETAGOVERHEAD
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowInvulnerableTagOverhead());
            break;
        case 305: // PETCOMBATTRAINING
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->PetCombatTraining());
            break;
        case 306: // HIRELINGCOMBATTRAINING
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->HirelingCombatTraining());
            break;
        case 307: // NPCCOMBATTRAINING
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->NpcCombatTraining());
            break;
        case 308: // GLOBALRESTOCKMULTIPLIER
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->GlobalRestockMultiplier()));
            break;
        case 309: // SHOWITEMRESISTSTATS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowItemResistStats());
            break;
        case 310: // SHOWWEAPONDAMAGETYPES
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowWeaponDamageTypes());
            break;
        case 311: // SHOWRACEWITHNAME
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowRaceWithName());
            break;
        case 312: // SHOWRACEINPAPERDOLL
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowRaceInPaperdoll());
            break;
        case 313: // WEAPONPARRY
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionWeaponParry()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 314: // WRESTLINGPARRY
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionWrestlingParry()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 315: // COMBATHITCHANCE
        {
            std::string tempString = {
                cwmWorldState->ServerData()->EraEnumToString(static_cast<expansionruleset_t>(
                    cwmWorldState->ServerData()->ExpansionCombatHitChance()))};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        case 316: // CASTSPELLSWHILEMOVING
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->CastSpellsWhileMoving());
            break;
        case 317: // SHOWREPUTATIONTITLEINTOOLTIP
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowReputationTitleInTooltip());
            break;
        case 318: // SHOWGUILDINFOINTOOLTIP
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->ShowGuildInfoInTooltip());
            break;
        case 319: // MAXPLAYERPACKWEIGHT
            *rval =
                INT_TO_JSVAL(static_cast<std::int32_t>(cwmWorldState->ServerData()->MaxPlayerPackWeight()));
            break;
        case 320: // MAXPLAYERBANKWEIGHT
            *rval =
                INT_TO_JSVAL(static_cast<std::int32_t>(cwmWorldState->ServerData()->MaxPlayerBankWeight()));
            break;
        case 321: // SAFECOOWNERLOGOUT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->SafeCoOwnerLogout());
            break;
        case 322: // SAFEFRIENDLOGOUT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->SafeFriendLogout());
            break;
        case 323: // SAFEGUESTLOGOUT
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->SafeGuestLogout());
            break;
        case 324: // KEYLESSOWNERACCESS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->KeylessOwnerAccess());
            break;
        case 325: // KEYLESSCOOWNERACCESS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->KeylessCoOwnerAccess());
            break;
        case 326: // KEYLESSFRIENDACCESS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->KeylessFriendAccess());
            break;
        case 327: // KEYLESSGUESTACCESS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->KeylessGuestAccess());
            break;
        case 328: // WEAPONDAMAGEBONUSTYPE
            *rval = INT_TO_JSVAL(
                static_cast<std::uint8_t>(cwmWorldState->ServerData()->WeaponDamageBonusType()));
            break;
        case 329: // OFFERBODSFROMITEMSALES
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->OfferBODsFromItemSales());
            break;
        case 330: // OFFERBODSFROMCONTEXTMENU
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->OfferBODsFromContextMenu());
            break;
        case 331: // BODSFROMCRAFTEDITEMSONLY
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->BODsFromCraftedItemsOnly());
            break;
        case 332: // BODGOLDREWARDMULTIPLIER
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->BODGoldRewardMultiplier()));
            break;
        case 333: // BODFAMEREWARDMULTIPLIER
            *rval = INT_TO_JSVAL(
                static_cast<R32>(cwmWorldState->ServerData()->BODFameRewardMultiplier()));
            break;
        case 334: // ENABLENPCGUILDDISCOUNTS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->EnableNPCGuildDiscounts());
            break;
        case 335: // ENABLENPCGUILDPREMIUMS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->EnableNPCGuildPremiums());
            break;
        case 336: // AGGRESSORFLAGTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_AGGRESSORFLAG)));
            break;
        case 337: // PERMAGREYFLAGTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_PERMAGREYFLAG)));
            break;
        case 338: // STEALINGFLAGTIMER
            *rval = INT_TO_JSVAL(
                static_cast<std::uint16_t>(cwmWorldState->ServerData()->SystemTimer(tSERVER_STEALINGFLAG)));
            break;
        case 339: // SNOOPAWARENESS
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->SnoopAwareness());
            break;
        case 340: // APSPERFTHRESHOLD
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->APSPerfThreshold()));
            break;
        case 341: // APSINTERVAL
            *rval =
                INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->APSPerfThreshold()));
            break;
        case 342: // APSDELAYSTEP
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->APSDelayStep()));
            break;
        case 343: // APSDELAYMAXCAP
            *rval = INT_TO_JSVAL(static_cast<std::uint16_t>(cwmWorldState->ServerData()->APSDelayMaxCap()));
            break;
        case 344: // YOUNGPLAYERSYSTEM
            *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->YoungPlayerSystem());
            break;
        // case 345:	 // YOUNGLOCATION
        // break;
        case 346: // SECRETSHARDKEY
        {
            std::string tempString = {cwmWorldState->ServerData()->SecretShardKey()};
            tString = JS_NewStringCopyZ(cx, tempString.c_str());
            *rval = STRING_TO_JSVAL(tString);
            break;
        }
        default:
            ScriptError(cx, "GetServerSetting: Invalid server setting name provided");
            return false;
        }
    }
    else {
        ScriptError(cx, "GetServerSetting: Provided argument contained no valid string data");
        return JS_FALSE;
    }
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetClientFeature()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific client feature is enabled on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetClientFeature([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                           uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "getClientFeature: Invalid number of arguments (takes 1 - feature ID)");
        return JS_FALSE;
    }

    auto clientFeature = static_cast<clientfeatures_t>(JSVAL_TO_INT(argv[0]));
    *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->getClientFeature(clientFeature));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerFeature()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific Server feature is enabled on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetServerFeature([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                           uintN argc, jsval *argv, jsval *rval) {
    if (argc != 1) {
        ScriptError(cx, "getServerFeature: Invalid number of arguments (takes 1 - feature ID)");
        return JS_FALSE;
    }

    auto serverFeature = static_cast<serverfeatures_t>(JSVAL_TO_INT(argv[0]));
    *rval = BOOLEAN_TO_JSVAL(cwmWorldState->ServerData()->getServerFeature(serverFeature));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetAccountCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of accounts on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetAccountCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                          [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(Account::shared().size());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetPlayerCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of players online on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetPlayerCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                         [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(cwmWorldState->GetPlayersOnline());
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetItemCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of items on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetItemCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                       [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMultiCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of multis on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetMultiCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                        [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv, jsval *rval) {
    *rval = INT_TO_JSVAL(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_MULTI));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetCharacterCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of characters on server
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetCharacterCount([[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj,
                            [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv,
                            jsval *rval) {
    *rval = INT_TO_JSVAL(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerVersionString()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets server version as a string
// o------------------------------------------------------------------------------------------------o
JSBool SE_GetServerVersionString(JSContext *cx, [[maybe_unused]] JSObject *obj,
                                 [[maybe_unused]] uintN argc, [[maybe_unused]] jsval *argv,
                                 jsval *rval) {
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
JSBool SE_DistanceBetween(JSContext *cx, [[maybe_unused]] JSObject *obj, uintN argc, jsval *argv,
                          jsval *rval) {
    if (argc != 2 && argc != 3 && argc != 4 && argc != 6) {
        ScriptError(
            cx, "DistanceBetween: needs 2, 3, 4 or 6 arguments - object a, object b - or object a, "
                "object b, (bool)checkZ - or x1, y1 and x2, y2 - or x1, y1, z1 and x2, y2, z2!");
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
JSBool SE_BASEITEMSERIAL(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                         [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, BASEITEMSERIAL, rval);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDSERIAL()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDSERIAL constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_INVALIDSERIAL(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                        [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, INVALIDSERIAL, rval);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDID()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDID constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_INVALIDID(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                    [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, INVALIDID, rval);
    return JS_TRUE;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDCOLOUR()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDCOLOUR constant
// o------------------------------------------------------------------------------------------------o
JSBool SE_INVALIDCOLOUR(JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] uintN argc,
                        [[maybe_unused]] jsval *argv, jsval *rval) {
    JS_NewNumberValue(cx, INVALIDCOLOUR, rval);
    return JS_TRUE;
}
