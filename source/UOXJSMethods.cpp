//o------------------------------------------------------------------------------------------------o
//|	File		-	UOXJSMethods.cpp
//|	Date		-
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|						1.0		20th December, 2001 Initial implementation
//|								Methods for the javascript objects
//|
//|								List of supported Classes:
//|									- Gumps
//|									- Char
//|									- Item
//|									- Socket
//o------------------------------------------------------------------------------------------------o
#include <variant>
#include <filesystem>
#include "uox3.h"
#include "UOXJSPropertySpecs.h"
#include "SEFunctions.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "CJSEngine.h"

#include "cMagic.h"
#include "cGuild.h"
#include "skills.h"
#include "speech.h"
#include "CGump.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "network.h"
#include "classes.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "commands.h"
#include "movement.h"
#include "wholist.h"
#include "townregion.h"
#include "Dictionary.h"
#include "jail.h"
#include "magic.h"
#include "CPacketSend.h"
#include "mapstuff.h"
#include "cThreadQueue.h"
#include "combat.h"
#include "PartySystem.h"
#include "osunique.hpp"
#include <js/Object.h>

void BuildAddMenuGump(CSocket* s, UI16 m);	// Menus for item creation
void SpawnGate(CChar* caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld, UI16 trgInstanceId = 0);
bool BuyShop(CSocket* s, CChar* c);
void InitializeWanderArea(CChar* c, SI16 xAway, SI16 yAway);
void ScriptError(JSContext* cx, const char* txt, ...);

void restoreContext(cScript* origScript, const std::string& area, UI16 origScriptID) {
	JSContext* context = origScript->getContext();
	JS::RootedValue rval(context);
	JS::RootedValueArray< 0 > nullArgs(context);
	if (!origScript->CallParticularEvent("_restorecontext_", nullArgs, 0, &rval))
	{
		// Dummy function not found, let shard admin know!
		Console.Warning(oldstrutil::format("Script context lost after using %s. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", area.c_str(), origScriptID));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MethodSpeech()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds speech entry of specified type, font, color, etc to the speech queue
//| Notes		-	Copied that here from SEFunctions.cpp. Default paramters weren't working !?
//o------------------------------------------------------------------------------------------------o
void MethodSpeech(CBaseObject& speaker, const char* message, SpeechType sType, COLOUR sColour = 0x005A,
	FontType fType = FNT_NORMAL, SpeechTarget spTrg = SPTRG_PCNPC, SERIAL spokenTo = INVALIDSERIAL, bool useUnicode = false)
{
	if (useUnicode)
	{
		bool sendAll = true;
		if (spTrg == SPTRG_INDIVIDUAL || spTrg == SPTRG_ONLYRECEIVER)
		{
			sendAll = false;
		}

		std::string fromChar(message);
		std::string speakerName = "";
		if (speaker.CanBeObjType(OT_CHAR))
		{
			CChar* speakerChar = CalcCharObjFromSer(speaker.GetSerial());
			speakerName = GetNpcDictName(speakerChar, nullptr, NRS_SPEECH);
		}
		else
		{
			speakerName = speaker.GetName();
		}
		speakerName = oldstrutil::stringToWstringToString(speakerName);
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message(fromChar);
		unicodeMessage.Font(fType);
		unicodeMessage.Colour(sColour);
		unicodeMessage.Type(sType);
		unicodeMessage.Language("ENG");
		unicodeMessage.Name(speakerName);
		unicodeMessage.ID(INVALIDID);
		unicodeMessage.Serial(speaker.GetSerial());

		if (sendAll)
		{
			UI16 searchDistance = DIST_SAMESCREEN;
			if (sType == WHISPER || sType == ASCIIWHISPER)
			{
				searchDistance = DIST_SAMETILE;
			}
			else if (sType == YELL || sType == ASCIIYELL)
			{
				searchDistance = DIST_SAMESCREEN * 1.5;
			}
			else if (sType == EMOTE || sType == ASCIIEMOTE)
			{
				searchDistance = DIST_INRANGE;
			}
			else
			{
				searchDistance = DIST_SAMESCREEN;
			}

			for (auto& tSock : FindNearbyPlayers(&speaker, searchDistance))
			{
				tSock->Send(&unicodeMessage);
			}
		}
		else
		{
			CChar* targChar = CalcCharObjFromSer(spokenTo);
			if (ValidateObject(targChar))
			{
				CSocket* targSock = targChar->GetSocket();
				if (targSock != nullptr)
				{
					targSock->Send(&unicodeMessage);
				}
			}
		}
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Font(fType);
		toAdd.Speech(message);
		toAdd.Speaker(speaker.GetSerial());
		toAdd.Type(sType);
		toAdd.TargType(spTrg);
		if (spTrg == SPTRG_INDIVIDUAL || spTrg == SPTRG_ONLYRECEIVER)
		{
			toAdd.SpokenTo(spokenTo);
		}

		// Added that because transparent text could cause trouble
		if (sColour != 0x1700 && sColour != 0x0)
		{
			toAdd.Colour(sColour);
		}
		else
		{
			toAdd.Colour(0x0058);
		}

		toAdd.At(cwmWorldState->GetUICurrentTime());
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Packet()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prepares a new packet stream, ready for network shenanigans in JS
//o------------------------------------------------------------------------------------------------o
static bool Packet(JSContext* cx, unsigned argc, JS::Value* vp)
{
	// Create a new CPUOXBuffer instance.
	CPUOXBuffer* toAdd = new CPUOXBuffer;

	// Get the 'this' object from the call stack.
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx, &args.thisv().toObject());

	// Define functions on the object using CPacket_Methods.
	if (!JS_DefineFunctions(cx, obj, CPacket_Methods))
		return false;

	// Set the first reserved slot of the object to store 'toAdd'.
	JS::SetReservedSlot(obj, 0, JS::PrivateValue(toAdd));

	// Return 'true' to indicate success.
	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_Free()
//|	Prototype	-	void Free()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes a packet stream
//o------------------------------------------------------------------------------------------------o
bool CPacket_Free(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CPUOXBuffer* toDelete = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(obj, 0);

	if (toDelete == nullptr)
		return false;

	delete toDelete;
	JS::SetReservedSlot(obj, 0, JS::UndefinedValue());

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteByte()
//|	Prototype	-	void WriteByte( position, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes a single byte at the specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
static bool CPacket_WriteByte(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "(CPacket_WriteByte) Invalid Number of Arguments %d, needs: 2 ", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CPUOXBuffer* myPacket = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(obj, 0);
	if (myPacket == nullptr)
	{
		ScriptError(cx, "(CPacket_WriteByte) Invalid Object Passed");
		return true;
	}

	size_t	position = static_cast<size_t>(args.get(0).toInt32());
	UI08		toWrite = static_cast<UI08>(args.get(1).toInt32());

	myPacket->GetPacketStream().WriteByte(position, toWrite);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteShort()
//|	Prototype	-	void WriteShort( position, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes two bytes at the specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
bool CPacket_WriteShort(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "(CPacket_WriteShort) Invalid Number of Arguments %d, needs: 2 ", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	CPUOXBuffer* myPacket = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(obj, 0);

	if (myPacket == nullptr)
	{
		ScriptError(cx, "(CPacket_WriteShort) Invalid Object Passed");
		return true;
	}

	size_t	position = static_cast<size_t>(args.get(0).toInt32());
	UI16		toWrite = static_cast<UI16>(args.get(1).toInt32());

	myPacket->GetPacketStream().WriteShort(position, toWrite);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteLong()
//|	Prototype	-	void WriteLong( position, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes four bytes at the specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
bool CPacket_WriteLong(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "(CPacket_WriteLong) Invalid Number of Arguments %d, needs: 2 ", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CPUOXBuffer* myPacket = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(obj, 0);
	if (myPacket == nullptr)
	{
		ScriptError(cx, "(CPacket_WriteLong) Invalid Object Passed");
		return true;
	}

	size_t	position = static_cast<size_t>(args.get(0).toInt32());
	auto toWriteChar = convertToString(cx, args.get(1).toString());
	UI32 toWrite = oldstrutil::value<UI32>(toWriteChar);

	myPacket->GetPacketStream().WriteLong(position, toWrite);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteString()
//|	Prototype	-	void WriteString( position, stringToWrite, length )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes variable number of bytes at specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
bool CPacket_WriteString(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3)
	{
		ScriptError(cx, "(CPacket_WriteString) Invalid Number of Arguments %d, needs: 3 ", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CPUOXBuffer* myPacket = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(obj, 0);
	if (myPacket == nullptr)
	{
		ScriptError(cx, "(CPacket_WriteString) Invalid Object Passed");
		return true;
	}

	size_t	position = static_cast<size_t>(args.get(0).toInt32());
	auto toWrite = convertToString(cx, args.get(1).toString());
	size_t	len = static_cast<size_t>(args.get(2).toInt32());

	myPacket->GetPacketStream().WriteString(position, toWrite, len);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_ReserveSize()
//|	Prototype	-	void ReserveSize( packetSize )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reserves a specific size for the packet stream
//o------------------------------------------------------------------------------------------------o
bool CPacket_ReserveSize(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(CPacket_ReserveSize) Invalid Number of Arguments %d, needs: 1 ", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CPUOXBuffer* myPacket = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(obj, 0);
	if (myPacket == nullptr)
	{
		ScriptError(cx, "(CPacket_ReserveSize) Invalid Object Passed");
		return true;
	}

	size_t len = static_cast<size_t>(args.get(0).toInt32());
	myPacket->GetPacketStream().ReserveSize(len);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Send()
//|	Prototype	-	void Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends prepared packet stream to network socket
//o------------------------------------------------------------------------------------------------o
bool CSocket_Send(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(CSocket_Send) Invalid Number of Arguments %d, needs: 1 ", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	JSObject* jsObj = args.get(0).toObjectOrNull();
	CPUOXBuffer* myPacket = JS::GetMaybePtrFromReservedSlot<CPUOXBuffer>(jsObj, 0);

	if (mSock == nullptr || myPacket == nullptr)
	{
		ScriptError(cx, "(CPacket_WriteString) Invalid Object Passed");
		return true;
	}

	mSock->Send(myPacket);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Gump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for creating a new JS Gump object
//o------------------------------------------------------------------------------------------------o
static bool Gump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	// Get the 'this' object from the call stack.
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx, &args.thisv().toObject());

	// Allocate the GumpList here and "SetPrivate" it to the Object
	SEGump_st* toAdd = new SEGump_st;
	toAdd->one = new std::vector<std::string>();
	toAdd->two = new std::vector<std::string>();
	toAdd->textId = 0;

	JS_DefineFunctions(cx, obj, CGump_Methods);
	JS::SetReservedSlot(obj, 0, JS::PrivateValue(toAdd));
	//JS_LockGCThing( cx, obj );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_Free()
//|	Prototype	-	void Free()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes JS Gump object and frees up associated memory
//o------------------------------------------------------------------------------------------------o
static bool CGump_Free(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	SEGump_st* toDelete = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (toDelete == nullptr)
		return false;

	delete toDelete->one;
	delete toDelete->two;
	delete toDelete;

	//JS_UnlockGCThing( cx, obj );
	JS::SetReservedSlot(obj, 0, JS::UndefinedValue());

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_Free()
//|	Prototype	-	void Free()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes JS Gump Data object and frees up associated memory
//o------------------------------------------------------------------------------------------------o
static bool CGumpData_Free(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	SEGumpData_st* toDelete = JS::GetMaybePtrFromReservedSlot<SEGumpData_st>(obj, 0);

	if (toDelete == nullptr)
		return false;

	//JS_UnlockGCThing( cx, obj );
	JS::SetReservedSlot(obj, 0, JS::UndefinedValue());

	delete toDelete;
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_GetEdit()
//|	Prototype	-	void getEdit( index )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets data input from TextEntr(y/ies) in JS Gump
//o------------------------------------------------------------------------------------------------o

static bool CGumpData_GetEdit(JSContext* cx, unsigned argc, JS::Value* vp) {

	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	if (args.length() == 0) {
		ScriptError(cx, "(GumpData_getEdit) Invalid number of arguments");
		args.rval().setString(JS_NewStringCopyN(cx, "", 0));
		return true;
	}

	SEGumpData_st* myItem = JS::GetMaybePtrFromReservedSlot<SEGumpData_st >(obj, 0);

	if (myItem == nullptr)
	{
		ScriptError(cx, "(DataGump-getEdit) Invalid object assigned");
		args.rval().setString(JS_NewStringCopyN(cx, "", 0));
		return true;
	}

	if (args[0].isInt32())
	{
		int32_t index = args[0].toInt32();
		if (index >= 0 && static_cast<size_t>(index) < myItem->sEdits.size())
		{
			args.rval().setString(JS_NewStringCopyZ(cx, myItem->sEdits[index].c_str()));
		}
		else
		{
			args.rval().setString(JS_NewStringCopyN(cx, "", 0));
		}
	}
	else
	{
		ScriptError(cx, "(DataGump-getEdit) Invalid argument type, expected an integer");
		args.rval().setString(JS_NewStringCopyN(cx, "", 0));
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_GetId()
//|	Prototype	-	void getID( index )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets nID at specified index in Gump Data
//o------------------------------------------------------------------------------------------------o
static bool CGumpData_GetId(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc == 0)
	{
		ScriptError(cx, "(GumpData_getID) Invalid Number of Arguments %d, needs: 1 ", argc);
		args.rval().setInt32(-1);
		return true;
	}

	SEGumpData_st* myItem = JS::GetMaybePtrFromReservedSlot<SEGumpData_st>(obj, 0);

	if (myItem == nullptr)
	{
		ScriptError(cx, "(DataGump_getID) Invalid object assigned");
		args.rval().setInt32(-1);
		return true;
	}
	size_t index = args.get(0).toInt32();

	if (index < myItem->nIDs.size())
	{
		args.rval().setInt32(myItem->nIDs[index]);
	}
	else
	{
		args.rval().setInt32(-1);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_GetButton()
//|	Prototype	-	void getButton( index )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets value of button at specified index in Gump Data
//o------------------------------------------------------------------------------------------------o
static bool CGumpData_GetButton(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc == 0)
	{
		ScriptError(cx, "(GumpData_getbutton) Invalid Number of Arguments %d, needs: 1 ", argc);
		args.rval().setInt32(-1);
		return true;
	}

	SEGumpData_st* myItem = JS::GetMaybePtrFromReservedSlot<SEGumpData_st>(obj, 0);

	if (myItem == nullptr)
	{
		ScriptError(cx, "(DataGump-getID) Invalid object assigned");
		args.rval().setInt32(-1);
		return true;
	}
	size_t index = args.get(0).toInt32();
	if (index < myItem->nButtons.size())
	{
		args.rval().setInt32(myItem->nButtons[index]);
	}
	else
	{
		args.rval().setInt32(-1);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddCheckbox()
//|	Prototype	-	void AddCheckbox( topHeight, topLeft, checkImage, defaultStatus, unk2 )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a checkbox gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddCheckbox(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 5 || argc > 6)
	{
		ScriptError(cx, "AddCheckbox: Wrong count of Parameters, needs 5 or 6");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = 0;
	SI16 tR = 0;
	UI16 gImage = 0;
	UI16 gImageChk = 0;
	SI16 initState = 0;
	SI16 relay = 0;

	if (argc == 5)
	{
		tL = static_cast<SI16>(args.get(0).toInt32());
		tR = static_cast<SI16>(args.get(1).toInt32());
		gImage = static_cast<UI16>(args.get(2).toInt32());
		gImageChk = gImage + 1;
		initState = static_cast<SI16>(args.get(3).toInt32());
		relay = static_cast<SI16>(args.get(4).toInt32());
	}
	else
	{
		tL = static_cast<SI16>(args.get(0).toInt32());
		tR = static_cast<SI16>(args.get(1).toInt32());
		gImage = static_cast<UI16>(args.get(2).toInt32());
		gImageChk = static_cast<UI16>(args.get(3).toInt32());
		initState = static_cast<SI16>(args.get(4).toInt32());
		relay = static_cast<SI16>(args.get(5).toInt32());
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (gList == nullptr)
	{
		ScriptError(cx, "Gump_AddCheckbox: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("checkbox %i %i %i %i %i %i", tL, tR, gImage, gImageChk, initState, relay));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoClose()
//|	Prototype	-	void NoClose()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds noclose element to gump stream; specifies that gump cannot be closed by
//|					clicking the right mouse button
//o------------------------------------------------------------------------------------------------o
static bool CGump_NoClose(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "NoClose: Wrong count of Parameters, needs 0");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (gList == nullptr)
	{
		ScriptError(cx, "NoClose: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back("noclose");

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoMove()
//|	Prototype	-	void NoMove()
//|	Date		-	9th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds nomove element to gump stream; specifies that gump cannot be moved
//o------------------------------------------------------------------------------------------------o
static bool CGump_NoMove(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "NoMove: Wrong count of Parameters, needs 0");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (gList == nullptr)
	{
		ScriptError(cx, "NoMove: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back("nomove");

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoDispose()
//|	Prototype	-	void NoDispose()
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds nodispose element to gump stream; specifies that gump cannot be disposed
//|					of/closed using the Escape button
//| Notes		-	It's also possible it is used to prevent gumps from being closed
//|					using the "Close Dialogs" client macro
//|					No response is sent to server upon closing the Gump in this manner.
//o------------------------------------------------------------------------------------------------o
static bool CGump_NoDispose(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "NoDispose: Wrong count of Parameters, needs 0");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (gList == nullptr)
	{
		ScriptError(cx, "NoDispose: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back("nodispose");

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoResize()
//|	Prototype	-	void NoResize()
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds noresize element to gump stream; specifies that gump cannot be resized
//o------------------------------------------------------------------------------------------------o
static bool CGump_NoResize(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "NoResize: Wrong count of Parameters, needs 0");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (gList == nullptr)
	{
		ScriptError(cx, "NoResize: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back("noresize");

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_MasterGump()
//|	Date		-	24th March, 2012
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Possible that the mastergump command itself only has any effect with client versions between 4.0.4d and 5.0.5b?
//o------------------------------------------------------------------------------------------------o
static bool CGump_MasterGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "MasterGump: Wrong count of Parameters, needs 1");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI32 masterGumpId = static_cast<SI32>(args.get(0).toInt32());
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (gList == nullptr)
	{
		ScriptError(cx, "MasterGump: Couldn't find gump associated with object");
		return false;
	}

	// Also send mastergump command with new gumpId
	gList->one->push_back(oldstrutil::format("mastergump %i %i %i %i %i", masterGumpId));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddBackground()
//|	Prototype	-	void AddBackground( topHeight, topLeft, bottomHeight, bottomLeft, backImage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds background gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddBackground(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 5)
	{
		ScriptError(cx, "AddBackground: Invalid number of arguments (takes 5)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	SI16 bL = static_cast<SI16>(args.get(2).toInt32());
	SI16 bR = static_cast<SI16>(args.get(3).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(4).toInt32());

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddBackground: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("resizepic %i %i %i %i %i", tL, tR, gImage, bL, bR));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddButton()
//|	Prototype	-	void AddButton( topHeight, topLeft, buttonImage, (buttonPressedImage), unk1, unk2, unk3 )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds button gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddButton(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 6 || argc > 7)
	{
		ScriptError(cx, "AddButton: Invalid number of arguments (takes 6 or 7)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32());
	UI16 gImage2 = (argc == 6 ? (gImage + 1) : static_cast<UI16>(args.get(3).toInt32()));
	SI16 x1 = (argc == 6 ? static_cast<SI16>(args.get(3).toInt32()) : static_cast<SI16>(args.get(4).toInt32()));
	SI16 x2 = (argc == 6 ? static_cast<SI16>(args.get(4).toInt32()) : static_cast<SI16>(args.get(5).toInt32()));
	SI16 x3 = (argc == 6 ? static_cast<SI16>(args.get(5).toInt32()) : static_cast<SI16>(args.get(6).toInt32()));

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddButton: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("button %i %i %i %i %i %i %i", tL, tR, gImage, gImage2, x1, x2, x3));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddButtonTileArt()
//|	Prototype	-	void AddButtonTileArt( topHeight, topLeft, buttonImage,
//|								buttonPressedImage, unk1, unk2, unk3, tileId, hue, tileX, tileY )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds button gump with tileart to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddButtonTileArt(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 11)
	{
		ScriptError(cx, "AddButtonTileArt: Invalid number of arguments (takes 11)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 tileIdNorm = static_cast<UI16>(args.get(2).toInt32());
	UI16 tileIdPush = static_cast<UI16>(args.get(3).toInt32());
	SI16 buttonType = static_cast<SI16>(args.get(4).toInt32());
	SI16 pageNum = static_cast<SI16>(args.get(5).toInt32());
	SI16 buttonId = static_cast<SI16>(args.get(6).toInt32());
	SI16 tileId = static_cast<SI16>(args.get(7).toInt32());
	SI16 hue = static_cast<SI16>(args.get(8).toInt32());
	SI16 tileX = static_cast<SI16>(args.get(9).toInt32());
	SI16 tileY = static_cast<SI16>(args.get(10).toInt32());
	//SI32 cliloc = 0;

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddButtonTileArt: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("buttontileart %i %i %i %i %i %i %i %i %i %i %i", tL, tR, tileIdNorm, tileIdPush, buttonType, pageNum, buttonId, tileId, hue, tileX, tileY));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPageButton()
//|	Prototype	-	void AddPageButton( topHeight, topLeft, buttonImage, (buttonPressedImage), pageNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds button gump for navigating gump pages to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddPageButton(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 4 || argc > 5)
	{
		ScriptError(cx, "AddPageButton: Invalid number of arguments (takes 4 or 5)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32());
	UI16 gImage2 = (argc == 4 ? (gImage + 1) : static_cast<UI16>(args.get(3).toInt32()));
	SI16 pageNum = (argc == 4 ? static_cast<SI16>(args.get(3).toInt32()) : static_cast<SI16>(args.get(4).toInt32()));

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddPageButton: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("button %i %i %i %i 0 %i", tL, tR, gImage, gImage2, pageNum));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddCheckerTrans()
//|	Prototype	-	void AddCheckerTrans( left, top, width, height )
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds transparent area gump to gump stream, turns underlying elements transparent
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddCheckerTrans(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 4)
	{
		ScriptError(cx, "AddCheckerTrans: Invalid number of arguments (takes 5 x y width height)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 width = static_cast<SI16>(args.get(2).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(3).toInt32()); // height

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddCheckerTrans: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("checkertrans %i %i %i %i", x, y, width, height));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddCroppedText()
//|	Prototype	-	void AddCroppedText( left, top, hue, width, height, actualText)
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds text field gump to gump stream, cropped to certain dimensions
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddCroppedText(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 6)
	{
		ScriptError(cx, "AddCroppedText: Invalid number of arguments (takes 6 x y hue width height text)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 TextX = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 TextY = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 TextHue = static_cast<SI16>(args.get(2).toInt32()); // Hue
	SI16 TextWidth = static_cast<SI16>(args.get(3).toInt32()); // width
	SI16 TextHeight = static_cast<SI16>(args.get(4).toInt32()); // height

	auto TextString = convertToString(cx, args.get(5).toString());

	if (TextString.empty())
	{
		ScriptError(cx, "AddCroppedText: Text is required");
		return false;
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddCroppedText: Couldn't find gump associated with object");
		return false;
	}

	UI32 textId = gList->textId;
	++gList->textId;

	gList->one->push_back(oldstrutil::format("croppedtext %i %i %i %i %i %u", TextX, TextY, TextWidth, TextHeight, TextHue, textId));
	gList->two->push_back(TextString);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddGroup()
//|	Prototype	-	void AddGroup( groupNum )
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds new group element to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddGroup(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddGroup: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddGroup: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("group %d", args.get(0).toInt32()));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_EndGroup()
//|	Prototype	-	void EndGroup( groupNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Ends a previously started group element
//o------------------------------------------------------------------------------------------------o
static bool CGump_EndGroup(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "EndGroup: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "EndGroup: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("endgroup", args.get(0).toInt32()));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddGump()
//|	Prototype	-	void AddGump( topHeight, topLeft, gumpImage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds image gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3 && argc != 4)
	{
		ScriptError(cx, "AddGump: Invalid number of arguments (takes 3 or 4 - topLeft, topRight, imageID and hue (optional))");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32());
	SI32 rgbColor = 0;
	if (argc == 4)
	{
		rgbColor = static_cast<UI16>(args.get(3).toInt32());
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddGump: Couldn't find gump associated with object");
		return false;
	}

	if (rgbColor == 0)
	{
		gList->one->push_back(oldstrutil::format("gumppic %i %i %i", tL, tR, gImage));
	}
	else
	{
		gList->one->push_back(oldstrutil::format("gumppic %i %i %i hue=%i", tL, tR, gImage, rgbColor));
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddGumpColor()
//|	Prototype	-	void AddGumpColor( topHeight, topLeft, gumpImage, hue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds image gump to gump stream, with additional parameter for hue
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddGumpColor(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 4)
	{
		ScriptError(cx, "AddGumpColor: Invalid number of arguments (takes 4)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32());
	SI32 rgbColour = static_cast<SI32>(args.get(3).toInt32());	// colour

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddGumpColor: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("gumppic %i %i %i hue=%i", tL, tR, gImage, rgbColour));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddToolTip()
//|	Prototype	-	void AddToolTip( clilocNum, ... )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds cliloc-based tooltip to previous gump (Gump, Image, Button) in gump stream
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Additional arguments (up to 10) can be provided if the cliloc in question needs
//|					them. This could also be used to show custom tooltips in the client, if using a
//|					cliloc (like 1042971) that only contain an argument and no additional text!
//|
//|					For example, if you use myGump.AddToolTip( 1042971, "My Custom Text" ) in JS
//|					UOX3 will send the following gump command to add a tooltip element to previous
//|					gump element: "tooltip 1042971 @My Custom Text@"
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddToolTip(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 1 || argc > 11)
	{
		ScriptError(cx, "AddToolTip: Invalid number of arguments (takes at least 1, maximum 11)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddToolTip: Couldn't find gump associated with object");
		return false;
	}

	SI32 tooltip = static_cast<SI32>(args.get(0).toInt32());
	std::stringstream temp;
	if (argc > 1)
	{
		// Additional arguments were provided along with the cliloc number
		// Only supported by client versions above ~7.0.16.0
		temp << "tooltip " << tooltip << " @";
		std::string tempArg;
		for (UI32 i = 2; i < argc; i++)
		{
			tempArg = convertToString(cx, args.get(i).toString());
			temp << (i == 2 ? tempArg : ("\t" + tempArg));
		}
		temp << "@";
	}
	else
	{
		// Cliloc number was provided with no additional arguments,
		// or client version doesn't support arguments for tooltip
		temp << "tooltip " << tooltip;
	}

	gList->one->push_back(temp.str());
	return true;
}

//o-------------------------------------------------------------------------------- ---------------o
//|	Function	-	CGump_AddHTMLGump()
//|	Prototype	-	void AddHTMLGump( topHeight, topLeft, width, height, background, scrollbar, HTMLText)
//|	Date		-	16th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new HTML gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddHTMLGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 7)
	{
		ScriptError(cx, "AddHTMLGump: Invalid number of arguments (takes 7)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 width = static_cast<SI16>(args.get(2).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(3).toInt32()); // height
	bool hasBorder = (args.get(4).toBoolean());
	bool hasScrollbar = (args.get(5).toBoolean());

	auto TextString = convertToString(cx, args.get(6).toString());

	if (TextString.empty())
	{
		ScriptError(cx, "AddHTMLGump: Text is required");
		return false;
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddHTMLGump: Couldn't find gump associated with object");
		return false;
	}

	UI32 textId = gList->textId;
	++gList->textId;

	SI32 iBrd = (hasBorder ? 1 : 0);
	SI32 iScrl = (hasScrollbar ? 1 : 0);

	gList->one->push_back(oldstrutil::format("htmlgump %i %i %i %i %u %i %i", x, y, width, height, textId, iBrd, iScrl));
	gList->two->push_back(TextString);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPage()
//|	Prototype	-	void AddPage( pageNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new page element to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddPage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddPage: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddPage: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("page %d", args.get(0).toInt32()));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPicture()
//|	Prototype	-	void AddPicture( topHeight, topLeft, tilePic )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a tile image gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddPicture(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3)
	{
		ScriptError(cx, "AddPicture: Invalid number of arguments (takes 3)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32());

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddPicture: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("tilepic %i %i %i", tL, tR, gImage));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPictureColor()
//|	Prototype	-	void AddPictureColor( topHeight, topLeft, tilePic, hue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a tile image gump to gump stream, with additional parameter for hue
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddPictureColor(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 4)
	{
		ScriptError(cx, "AddPicture: Invalid number of arguments (takes 4)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32());
	SI32 rgbColour = static_cast<SI32>(args.get(3).toInt32());	// colour

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddPictureColor: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("tilepichue %i %i %i %i", tL, tR, gImage, rgbColour));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPicInPic()
//|	Prototype	-	void AddPicInPic( topLeft, topHeight, gumpImage, spriteX, spriteY, width, height )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a picinpic gump to the gump stream
//|	Notes		-	Requires client v7.0.80.0 or above
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddPicInPic(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 7)
	{
		ScriptError(cx, "AddPicInPic: Invalid number of arguments (takes 7)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // starting x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // starting y
	UI16 gImage = static_cast<UI16>(args.get(2).toInt32()); // GumpId
	SI16 spriteX = static_cast<SI16>(args.get(3).toInt32()); // spriteX
	SI16 spriteY = static_cast<SI16>(args.get(4).toInt32()); // spriteY
	SI16 width = static_cast<SI16>(args.get(5).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(6).toInt32()); // height

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddPicInPic: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("picinpic %i %i %i %i %i %i %i", x, y, gImage, spriteX, spriteY, width, height));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddItemProperty()
//|	Prototype	-	void AddImageProperty( object )
//|	Date		-	6th July, 2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Attaches item property element to previous element added to JS Gump, which
//|					causes client to display the known properties of an object based on the
//|					object serial that was provided as a regular tooltip on cursor. Could be used to
//|					show item stats for items on a custom paperdoll, for instance.
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddItemProperty(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddItemProperty: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JSObject* tObj = args.get(0).toObjectOrNull();
	CBaseObject* trgObj = JS::GetMaybePtrFromReservedSlot<CBaseObject >(tObj, 0);

	if (!ValidateObject(trgObj) || (trgObj->GetSerial() == INVALIDSERIAL))
	{
		ScriptError(cx, "SetCont: Invalid Object/Argument, takes 1 arg: item");
		return false;
	}

	SERIAL trgSer = trgObj->GetSerial();

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddItemProperty: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("itemproperty %i", trgSer));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddRadio()
//|	Prototype	-	void AddRadio( topHeight, topLeft, radioImage, pressed, id )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a radio button gump to gumps stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddRadio(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 5 || argc > 6)
	{
		ScriptError(cx, "Gump_AddRadio: Invalid number of arguments (takes 5 or 6)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = 0;
	SI16 tR = 0;
	UI16 gImage = 0;
	UI16 gImageChk = 0;
	SI16 initialState = 0;
	SI16 relay = 0;

	if (argc == 5)
	{
		tL = static_cast<SI16>(args.get(0).toInt32());
		tR = static_cast<SI16>(args.get(1).toInt32());
		gImage = static_cast<UI16>(args.get(2).toInt32());
		gImageChk = gImage + 1;
		initialState = static_cast<SI16>(args.get(3).toInt32());
		relay = static_cast<SI16>(args.get(4).toInt32());
	}
	else
	{
		tL = static_cast<SI16>(args.get(0).toInt32());
		tR = static_cast<SI16>(args.get(1).toInt32());
		gImage = static_cast<UI16>(args.get(2).toInt32());
		gImageChk = static_cast<UI16>(args.get(3).toInt32());
		initialState = static_cast<SI16>(args.get(4).toInt32());
		relay = static_cast<SI16>(args.get(5).toInt32());
	}


	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddRadio: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("radio %i %i %i %i %i %i", tL, tR, gImage, gImageChk, initialState, relay));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddText()
//|	Prototype	-	void AddText( topHeight, topLeft, textColour, text )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a text gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddText(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 4)
	{
		ScriptError(cx, "AddText: Invalid number of arguments (takes 4)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UI32 textId;

	SI16 TextX = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 TextY = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 TextHue = static_cast<SI16>(args.get(2).toInt32()); // Hue

	auto TextString = convertToString(cx, args.get(3).toString());

	if (TextString.empty())
	{
		ScriptError(cx, "AddText: Text is required");
		return false;
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddText: Couldn't find gump associated with object");
		return false;
	}

	textId = gList->textId;
	++gList->textId;

	gList->one->push_back(oldstrutil::format("text %i %i %i %u", TextX, TextY, TextHue, textId));
	gList->two->push_back(TextString);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddTextEntry()
//|	Prototype	-	void AddTextEntry( topHeight, topLeft, width, height, hue, unk4, textID, defaultText )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a text entry gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddTextEntry(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 8)
	{
		ScriptError(cx, "AddTextEntry: Invalid number of arguments (takes 8)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	SI16 width = static_cast<SI16>(args.get(2).toInt32());
	SI16 height = static_cast<SI16>(args.get(3).toInt32());
	SI16 hue = static_cast<SI16>(args.get(4).toInt32());
	SI16 relay = static_cast<SI16>(args.get(5).toInt32());
	SI16 initialTextIndex = static_cast<SI16>(args.get(6).toInt32());
	auto test = convertToString(cx, args.get(7).toString());

	if (test.empty())
	{
		ScriptError(cx, "AddTextEntry: Text is required");
		return false;
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddTextEntry: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("textentry %i %i %i %i %i %i %i", tL, tR, width, height, hue, relay, initialTextIndex));
	gList->two->push_back(test);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddTextEntryLimited()
//|	Prototype	-	void AddTextEntryLimited( topHeight, topLeft, width, height, hue, unk4, textID, defaultText, maxLength )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a text entry gump with maximum length to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddTextEntryLimited(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 9)
	{
		ScriptError(cx, "AddTextEntryLimited: Invalid number of arguments (takes 9)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 tL = static_cast<SI16>(args.get(0).toInt32());
	SI16 tR = static_cast<SI16>(args.get(1).toInt32());
	SI16 width = static_cast<SI16>(args.get(2).toInt32());
	SI16 height = static_cast<SI16>(args.get(3).toInt32());
	SI16 hue = static_cast<SI16>(args.get(4).toInt32());
	SI16 relay = static_cast<SI16>(args.get(5).toInt32());
	SI16 initialTextIndex = static_cast<SI16>(args.get(6).toInt32());
	auto test = convertToString(cx, args.get(7).toString());
	SI16 textEntrySize = static_cast<SI16>(args.get(8).toInt32());

	if (test.empty())
	{
		ScriptError(cx, "AddTextEntryLimited: Text is required");
		return false;
	}

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddTextEntryLimited: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("textentrylimited %i %i %i %i %i %i %i %i", tL, tR, width, height, hue, relay, initialTextIndex, textEntrySize));
	gList->two->push_back(test);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddTiledGump()
//|	Prototype	-	void AddTiledGump( left, top, width, height, gumpId )
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds tiled gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddTiledGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 5)
	{
		ScriptError(cx, "AddTiledGump: Invalid number of arguments (takes 5 x y width height gump)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 width = static_cast<SI16>(args.get(2).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(3).toInt32()); // height
	SI16 gumpId = static_cast<SI16>(args.get(4).toInt32()); // gump to tile

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddTiledGump: Couldn't find gump associated with object");
		return false;
	}

	gList->one->push_back(oldstrutil::format("gumppictiled %i %i %i %i %i", x, y, width, height, gumpId));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddXMFHTMLGump()
//|	Prototype	-	void AddXMFHTMLGump( left, top, width, height, number, hasBorder, hasScrollbar )
//|	Date		-	16th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new XMFHTML gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddXMFHTMLGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 7)
	{
		ScriptError(cx, "AddXMFHTMLGump: Invalid number of arguments (takes 7)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 width = static_cast<SI16>(args.get(2).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(3).toInt32()); // height
	SI32 number = static_cast<SI32>(args.get(4).toInt32()); // number
	bool hasBorder = (args.get(5).toBoolean());
	bool hasScrollbar = (args.get(6).toBoolean());

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddXMFHTMLGump: Couldn't find gump associated with object");
		return false;
	}

	SI32 iBrd = (hasBorder ? 1 : 0);
	SI32 iScrl = (hasScrollbar ? 1 : 0);

	gList->one->push_back(oldstrutil::format("xmfhtmlgump %i %i %i %i %i %i %i", x, y, width, height, number, iBrd, iScrl));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddXMFHTMLGumpColor()
//|	Prototype	-	void AddXMFHTMLGumpColor( left, top, width, height, number, hasBorder, hasScrollbar, rgbColour )
//|	Date		-	16th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new cliloc based XMFHTML gump to gump stream
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddXMFHTMLGumpColor(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 8)
	{
		ScriptError(cx, "AddXMFHTMLGumpColor: Invalid number of arguments (takes 8)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 width = static_cast<SI16>(args.get(2).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(3).toInt32()); // height
	SI32 number = static_cast<SI32>(args.get(4).toInt32()); // number
	bool hasBorder = (args.get(5).toBoolean());
	bool hasScrollbar = (args.get(6).toBoolean());
	SI32 rgbColour = static_cast<SI32>(args.get(7).toInt32());	// colour

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddXMFHTMLGumpColor: Couldn't find gump associated with object");
		return false;
	}

	SI32 iBrd = (hasBorder ? 1 : 0);
	SI32 iScrl = (hasScrollbar ? 1 : 0);

	gList->one->push_back(oldstrutil::format("xmfhtmlgumpcolor %i %i %i %i %i %i %i %i", x, y, width, height, number, iBrd, iScrl, rgbColour));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddXMFHTMLTok()
//|	Prototype	-	void AddXMFHTMLGumpColor( left, top, width, height, number, hasBorder, hasScrollbar, rgbColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new cliloc based XMFHTML gump to gump stream, with optional cliloc arguments
//o------------------------------------------------------------------------------------------------o
static bool CGump_AddXMFHTMLTok(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 8)
	{
		ScriptError(cx, "AddXMFHTMLTok: Invalid number of arguments (takes at least 8)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x = static_cast<SI16>(args.get(0).toInt32()); // x
	SI16 y = static_cast<SI16>(args.get(1).toInt32()); // y
	SI16 width = static_cast<SI16>(args.get(2).toInt32()); // width
	SI16 height = static_cast<SI16>(args.get(3).toInt32()); // height
	bool hasBorder = (args.get(4).toBoolean());
	bool hasScrollbar = (args.get(5).toBoolean());
	SI32 rgbColour = static_cast<SI32>(args.get(6).toInt32());	// colour
	SI32 number = static_cast<SI32>(args.get(7).toInt32()); // number

	auto TextString1 = convertToString(cx, args.get(8).toString()); // ClilocArgument1
	auto TextString2 = convertToString(cx, args.get(9).toString()); // ClilocArgument2
	auto TextString3 = convertToString(cx, args.get(10).toString()); // ClilocArgument3

	SEGump_st* gList = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);
	if (gList == nullptr)
	{
		ScriptError(cx, "AddXMFHTMLTok: Couldn't find gump associated with object");
		return false;
	}

	SI32 iBrd = (hasBorder ? 1 : 0);
	SI32 iScrl = (hasScrollbar ? 1 : 0);

	gList->one->push_back(oldstrutil::format("xmfhtmltok %i %i %i %i %i %i %i %i @%s\t%s\t%s@", x, y, width, height, iBrd, iScrl, rgbColour, number, TextString1.c_str(), TextString2.c_str(), TextString3.c_str()));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_Send()
//|	Prototype	-	void Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump stream to socket
//o------------------------------------------------------------------------------------------------o
static bool CGump_Send(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Send: Invalid number of arguments (takes 1, socket or char)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (!args.get(0).isObject())
	{
		ScriptError(cx, "You have to pass a valid Socket or Character");
	}

	auto myClass = args.get(0);
	std::string className = JS::GetClass(myClass.toObjectOrNull())->name;
	SEGump_st* myGump = JS::GetMaybePtrFromReservedSlot<SEGump_st>(obj, 0);

	if (className == "UOXSocket")
	{
		CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(myClass.toObjectOrNull(), 0);
		if (mySock == nullptr)
		{
			ScriptError(cx, "Send: Passed an invalid Socket");
			return false;
		}
		UI32 gumpId = (0xFFFF + JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx)));
		SendVecsAsGump(mySock, *(myGump->one), *(myGump->two), gumpId, INVALIDSERIAL);
	}
	else if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(myClass.toObjectOrNull(), 0);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "Send: Passed an invalid Character");
			return false;
		}

		CSocket* mySock = myChar->GetSocket();
		UI32 gumpId = (0xFFFF + JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx)));
		SendVecsAsGump(mySock, *(myGump->one), *(myGump->two), gumpId, INVALIDSERIAL);
	}
	else
	{
		ScriptError(cx, "Send: Unknown Object has been passed");
		return false;
	}

	return true;
}

// Character related methods!
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_TextMessage()
//|	Prototype	-	void TextMessage( message )
//|					void TextMessage( message, allHear, txtHue )
//|					void TextMessage( message, allHear, txtHue, speechTarget, speechTargetSerial, speechFontType );
//|					void TextMessage( message, allHear, txtHue, speechTarget, speechTargetSerial, speechFontType, speechType );
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes character to say a message
//o------------------------------------------------------------------------------------------------o
static bool CBase_TextMessage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 1 || argc > 7)
	{
		ScriptError(cx, "TextMessage: Invalid number of arguments (takes 1 - 7)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	std::string trgMessage = convertToString(cx, args.get(0).toString());
	if (trgMessage.empty())
	{
		ScriptError(cx, "You have to supply a message-text");
		return false;
	}

	UI16 txtHue = 0x0000;
	if (argc >= 3)
	{
		txtHue = static_cast<UI16>(args.get(2).toInt32());
	}

	SpeechTarget speechTarget = SPTRG_NULL;
	if (argc >= 4)
	{
		speechTarget = static_cast<SpeechTarget>(args.get(3).toInt32());
	}
	if (speechTarget == SPTRG_NULL)
	{
		speechTarget = SPTRG_PCNPC;
	}

	SERIAL speechTargetSerial = INVALIDSERIAL;
	if (argc >= 5)
	{
		speechTargetSerial = static_cast<SERIAL>(args.get(4).toInt32());
	}

	FontType speechFontType = FNT_NULL;
	if (argc == 6)
	{
		speechFontType = static_cast<FontType>(args.get(5).toInt32());
	}

	SpeechType speechType = UNKNOWN;
	if (argc == 7)
	{
		speechType = static_cast<SpeechType>(args.get(6).toInt32());
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (myObj->CanBeObjType(ObjectType::OT_ITEM))
	{
		CItem* myItem = static_cast<CItem*>(myObj);
		if (!ValidateObject(myItem))
		{
			ScriptError(cx, "TextMessage: Invalid Item");
			return false;
		}
		if (!txtHue)
		{
			txtHue = 0x047F;
		}
		if (speechFontType == FNT_NULL)
		{
			speechFontType = FNT_NORMAL;
		}
		if (speechType == UNKNOWN)
		{
			speechType = OBJ;
		}
		MethodSpeech(*myItem, trgMessage.c_str(), speechType, txtHue, speechFontType, speechTarget, speechTargetSerial, useUnicode);
	}
	else if (myObj->CanBeObjType(ObjectType::OT_CHAR))
	{
		CChar* myChar = static_cast<CChar*>(myObj);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "TextMessage: Invalid Character");
			return false;
		}

		if (argc >= 2 && argc <= 3 && !args.get(1).toBoolean())
		{
			speechTarget = SPTRG_INDIVIDUAL;
			if (speechTargetSerial == INVALIDSERIAL)
			{
				speechTargetSerial = myChar->GetSerial();
			}
		}

		if (speechFontType == FNT_NULL)
		{
			speechFontType = static_cast<FontType>(myChar->GetFontType());
		}
		if (speechType == UNKNOWN)
		{
			speechType = SAY;
		}

		if (myChar->GetNpcAiType() == AI_EVIL || myChar->GetNpcAiType() == AI_EVIL_CASTER)
		{
			if (!txtHue)
			{
				txtHue = 0x0026;
			}
			MethodSpeech(*myChar, trgMessage.c_str(), speechType, txtHue, speechFontType, speechTarget, speechTargetSerial, useUnicode);
		}
		else
		{
			if (!txtHue)
			{
				txtHue = myChar->GetSayColour();
			}
			MethodSpeech(*myChar, trgMessage.c_str(), speechType, txtHue, speechFontType, speechTarget, speechTargetSerial, useUnicode);
		}
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Item JS Method .TextMessage()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_KillTimers()
//|	Prototype	-	void KillTimers()
//|					void KillTimers( scriptId )
//|	Date		-	04/20/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kill all related timers that have been associated with an item or character
//o------------------------------------------------------------------------------------------------o
bool CBase_KillTimers(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "KillTimers: Invalid count of arguments :%d, needs :0 or 1", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	auto myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "KillTimers: Invalid object assigned.");
		return false;
	}
	SI32 triggerNum = -1;
	if (argc == 1)
	{
		triggerNum = static_cast<UI16>(args.get(0).toInt32());
	}

	SERIAL mySer = myObj->GetSerial();
	std::vector<CTEffect*> removeEffect;

	for (const auto& Effect : cwmWorldState->tempEffects.collection())
	{
		if (triggerNum != -1)
		{
			if (mySer == Effect->Destination() && Effect->More1() == triggerNum)
			{
				removeEffect.push_back(Effect);
			}
		}
		else if (mySer == Effect->Destination())
		{
			removeEffect.push_back(Effect);
		}
	}
	for (const auto& Effect : removeEffect)
	{
		cwmWorldState->tempEffects.Remove(Effect, true);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetJSTimer()
//|	Prototype	-	void GetJSTimer( timerId, scriptId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get JS timer with specific timerId association with an item or character, and return time left
//o------------------------------------------------------------------------------------------------o
bool CBase_GetJSTimer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "GetJSTimer: Invalid count of arguments :%d, needs 2 (timerId, scriptId)", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	auto myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "GetJSTimer: Invalid object assigned.");
		return false;
	}

	args.rval().setInt32(0); // Return value 0 by default, to indicate no valid timer found
	UI16 timerId = static_cast<UI16>(args.get(0).toInt32());
	UI16 scriptId = static_cast<UI16>(args.get(1).toInt32());

	SERIAL myObjSerial = myObj->GetSerial();
	for (const auto& Effect : cwmWorldState->tempEffects.collection())
	{
		// We only want results that have same object serial and timerId as specified
		if (myObjSerial == Effect->Destination() && Effect->More1() == timerId)
		{
			// Check for a valid script associated with Effect
			cScript* tScript = JSMapping->GetScript(Effect->AssocScript());
			if (tScript == nullptr && Effect->More2() != 0xFFFF)
			{
				// If no default script was associated with effect, check if another script was stored in More2
				tScript = JSMapping->GetScript(Effect->More2());
			}

			// If a valid script is associated with Effect, and the Effect's scriptId matches the provided scriptId...
			if (tScript != nullptr && (scriptId == Effect->AssocScript() || scriptId == Effect->More2()))
			{
				// Return the timestamp for when the Effect timer expires
				//args.rval().setInt32( Effect->ExpireTime() );
				args.rval().setInt32(Effect->ExpireTime());
			}
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_SetJSTimer()
//|	Prototype	-	void SetJSTimer( timerId, timeInMilliseconds, scriptId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set expiration time for JS timer with specific timerId association with an item or character
//o------------------------------------------------------------------------------------------------o
bool CBase_SetJSTimer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3)
	{
		ScriptError(cx, "SetJSTimer: Invalid count of arguments :%d, needs 3 (timerId, timeInMilliseconds, scriptId)", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	auto myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "SetJSTimer: Invalid object assigned.");
		return false;
	}

	args.rval().setInt32(0); // Return value is 0 by default, indicating no timer was found or updated
	UI16 timerId = static_cast<UI16>(args.get(0).toInt32());
	UI32 expireTime = BuildTimeValue(args.get(1).toInt32() / 1000.0f);
	UI16 scriptId = static_cast<UI16>(args.get(2).toInt32());

	SERIAL myObjSerial = myObj->GetSerial();
	for (const auto& Effect : cwmWorldState->tempEffects.collection())
	{
		// We only want to modify the Effect that have same object serial and timerId as specified
		if (myObjSerial == Effect->Destination() && Effect->More1() == timerId)
		{
			// Check for a valid script associated with Effect
			cScript* tScript = JSMapping->GetScript(Effect->AssocScript());
			if (tScript == nullptr && Effect->More2() != 0xFFFF)
			{
				// If no default script was associated with effect, check if another script was stored in More2
				tScript = JSMapping->GetScript(Effect->More2());
			}

			// If a valid script is associated with Effect, and the Effect's scriptId matches the provided scriptId...
			if (tScript != nullptr && (scriptId == Effect->AssocScript() || scriptId == Effect->More2()))
			{
				// Set the timestamp for when the Effect timer expires to that specified in parameters
				Effect->ExpireTime(expireTime);
				args.rval().setInt32(1); // Return 1 indicating timer was found and updated
			}
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_KillJSTimer()
//|	Prototype	-	void KillJSTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kill JS timer on item or character based on specified scriptId and timerId
//o------------------------------------------------------------------------------------------------o
bool CBase_KillJSTimer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "KillJSTimer: Invalid count of arguments :%d, needs 2 (timerId, scriptId)", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	auto myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "KillJSTimer: Invalid object assigned.");
		return false;
	}

	args.rval().setInt32(0); // Return value 0 by default, to indicate no valid timer found
	UI16 timerId = static_cast<UI16>(args.get(0).toInt32());
	UI16 scriptId = static_cast<UI16>(args.get(1).toInt32());

	SERIAL myObjSerial = myObj->GetSerial();
	CTEffect* removeEffect = nullptr;

	for (auto& Effect : cwmWorldState->tempEffects.collection())
	{
		if (myObjSerial == Effect->Destination() && Effect->More1() == timerId)
		{
			// Check for a valid script associated with Effect
			cScript* tScript = JSMapping->GetScript(Effect->AssocScript());
			if (tScript == nullptr && Effect->More2() != 0xFFFF)
			{
				// If no default script was associated with effect, check if another script was stored in More2
				tScript = JSMapping->GetScript(Effect->More2());
			}

			// If a valid script is associated with Effect, and the Effect's scriptId matches the provided scriptId...
			if (tScript != nullptr && (scriptId == Effect->AssocScript() || scriptId == Effect->More2()))
			{
				// Found our timer! Keep track of it for removal outside loop
				removeEffect = Effect;
				break;
			}
		}
	}

	if (removeEffect != nullptr)
	{
		cwmWorldState->tempEffects.Remove(removeEffect, true);
		args.rval().setInt32(1); // Return 1 indicating timer was found and removed
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_Delete()
//|	Prototype	-	void Delete()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes object
//o------------------------------------------------------------------------------------------------o
static bool CBase_Delete(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Delete: Invalid object");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	myObj->Delete();

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Item JS Method .Delete()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Wander()
//|	Prototype	-	void Wander( x1, y1, x2, y2 )
//|					void Wander( x1, y1, radius )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Specifies a wander area for an NPC, as either a box or a circle
//o------------------------------------------------------------------------------------------------o
static bool CChar_Wander(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3 && argc != 4)
	{
		ScriptError(cx, "Wander: Invalid number of arguments (takes 3-4, coordinates)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI16 x1 = static_cast<SI16>(args.get(0).toInt32());
	SI16 y1 = static_cast<SI16>(args.get(1).toInt32());
	SI16 x2 = static_cast<SI16>(args.get(2).toInt32());
	SI16 y2 = 0;
	if (argc > 3)
	{
		y2 = static_cast<SI16>(args.get(3).toInt32());
	}

	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar) || !myChar->IsNpc())
	{
		ScriptError(cx, "WanderBox: Invalid character");
		return false;
	}
	myChar->SetFx(x1, 0);
	myChar->SetFy(y1, 0);
	myChar->SetFz(-1);
	myChar->SetFx(x2, 1);
	if (argc > 3)
	{
		myChar->SetFy(y2, 1);
		myChar->SetNpcWander(WT_BOX);
	}
	else
	{
		myChar->SetNpcWander(WT_CIRCLE);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Follow()
//|	Prototype	-	void Follow( followTarget )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces NPC to follow specified target
//o------------------------------------------------------------------------------------------------o
static bool CChar_Follow(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Follow: Invalid number of arguments (takes 1, char object or null)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar) || !myChar->IsNpc())
	{
		ScriptError(cx, "Follow: Invalid NPC character object referenced!");
		return false;
	}

	if (args.get(0).isNullOrUndefined())
	{
		// Clear follow target if null was provided instead of a character object to follow
		myChar->SetFTarg(nullptr);
		myChar->SetNpcWander(WT_NONE);
		return true;
	}

	JSObject* jsObj = args.get(0).toObjectOrNull();
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject >(jsObj, 0);

	if (!ValidateObject(myObj) || myObj->GetSerial() >= BASEITEMSERIAL)
	{
		ScriptError(cx, "Follow: Invalid character parameter provided!");
		return false;
	}

	myChar->SetFTarg(static_cast<CChar*>(myObj));
	myChar->FlushPath();
	myChar->SetNpcWander(WT_FOLLOW);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_DoAction()
//|	Prototype	-	void DoAction( trgAction )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes character do specified action
//o------------------------------------------------------------------------------------------------o
static bool CChar_DoAction(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 1 || argc > 5)
	{
		ScriptError(cx, "DoAction: Invalid number of arguments (takes 1 - (actionID), 2 (actionID, subActionID), 3 (actionID, null, frameCount), 4 (actionID, null, frameCount, frameDelay ) or 5 (actionID, null, frameCount, frameDelay, playBackwards)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UI16 targAction = static_cast<UI16>(args.get(0).toInt32());
	SI16 targSubAction = -1;
	UI16 frameCount = 7;
	UI16 frameDelay = 0;
	bool playBackwards = false;
	if (argc > 1)
	{
		if (!args.get(1).isNullOrUndefined())
		{
			targSubAction = static_cast<SI16>(args.get(1).toInt32());
		}
	}
	if (argc > 2)
	{
		frameCount = static_cast<UI16>(args.get(2).toInt32());
	}
	if (argc > 3)
	{
		frameDelay = static_cast<UI16>(args.get(3).toInt32());
	}
	if (argc > 4)
	{
		playBackwards = args.get(4).toBoolean();
	}

	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "Action: Invalid character");
		return false;
	}

	// Reset idle anim timer so it doesn't interrupt the DoAction anim
	myChar->SetTimer(tNPC_IDLEANIMTIME, BuildTimeValue(RandomNum(5, 20)));

	// Play the requested animation
	if (myChar->GetBodyType() == BT_GARGOYLE || targSubAction != -1)
	{
		Effects->PlayNewCharacterAnimation(myChar, targAction, static_cast<UI16>(targSubAction));
	}
	else
	{
		Effects->PlayCharacterAnimation(myChar, targAction, frameDelay, frameCount, playBackwards);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_EmoteMessage()
//|	Prototype	-	void EmoteMessage( message )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes character to emote specified text
//o------------------------------------------------------------------------------------------------o
static bool CChar_EmoteMessage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 1 || argc > 5)
	{
		ScriptError(cx, "EmoteMessage: Invalid number of arguments (takes 1 - 5: text, allSay, hue, speech target and speech serial)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	auto trgMessage = convertToString(cx, args.get(0).toString());

	if (!ValidateObject(myChar) || trgMessage.empty())
	{
		ScriptError(cx, "EmoteMessage: Invalid character or speech");
		return false;
	}

	UI16 txtHue = 0x0000;
	if (argc >= 3)
	{
		txtHue = static_cast<UI16>(args.get(2).toInt32());
	}
	if (txtHue == 0x0000)
	{
		txtHue = myChar->GetEmoteColour();
	}

	SpeechTarget speechTarget = SPTRG_NULL;
	if (argc >= 4)
	{
		speechTarget = static_cast<SpeechTarget>(args.get(3).toInt32());
	}
	if (speechTarget == SPTRG_NULL)
	{
		speechTarget = SPTRG_PCNPC;
	}

	SERIAL speechTargetSerial = INVALIDSERIAL;
	if (argc >= 5)
	{
		speechTargetSerial = static_cast<SERIAL>(args.get(4).toInt32());
	}

	if (argc >= 2 && argc <= 3 && !args.get(1).toBoolean())
	{
		speechTarget = SPTRG_INDIVIDUAL;
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	MethodSpeech(*myChar, trgMessage.c_str(), EMOTE, txtHue, static_cast<FontType>(myChar->GetFontType()), speechTarget, speechTargetSerial, useUnicode);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Dismount()
//|	Prototype	-	void Dismount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dismounts character, if mounted
//o------------------------------------------------------------------------------------------------o
static bool CChar_Dismount(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Dismount: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "Dismount: Invalid character");
		return false;
	}
	if (myChar->IsOnHorse())
	{
		DismountCreature(myChar);
	}
	else if (myChar->IsFlying())
	{
		myChar->ToggleFlying();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SysMessage()
//|	Prototype	-	void SysMessage( message )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a system message to the player
//o------------------------------------------------------------------------------------------------o
static bool CMisc_SysMessage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc == 0 || argc > 11)
	{
		ScriptError(cx, "SysMessage: Invalid number of arguments (takes at least 1, and at most 11)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = nullptr;
	std::string className = JS::GetClass(obj.get())->name;

	if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		mySock = myChar->GetSocket();
	}
	else if (className == "UOXSocket")
	{
		mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
	}

	if (mySock == nullptr)
	{
		ScriptError(cx, "SysMessage: invalid socket");
		return false;
	}

	UI16 msgColor = 0;
	UI08 argCounter = 0;
	if (argc > 1 && args.get(0).isInt32())
	{
		msgColor = static_cast<UI16>(args.get(argCounter++).toInt32());
	}

	std::string trgMessage = convertToString(cx, args.get(argCounter++).toString());

	if (trgMessage.empty() || trgMessage.length() == 0)
	{
		ScriptError(cx, "SysMessage: Invalid speech (%s)", trgMessage.c_str());
		return false;
	}

	std::string msgArg;
	for (UI32 i = argCounter; i < argc; i++)
	{
		if (msgArg.empty())
		{
			msgArg += convertToString(cx, args.get(i).toString());
		}
		else
		{
			msgArg += std::string(",") + convertToString(cx, args.get(i).toString());
		}
	}

	mySock->SysMessageJS(trgMessage, msgColor, msgArg);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Disconnect()
//|	Prototype	-	void Disconnect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disconnects specified client
//o------------------------------------------------------------------------------------------------o
static bool CSocket_Disconnect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Disconnect: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* targSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (targSock == nullptr)
	{
		ScriptError(cx, "SysMessage: Invalid socket");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	Network->Disconnect(targSock);
	JS::SetReservedSlot(obj, 0, JS::UndefinedValue()); // yes we should do that...

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Socket JS Method .Disconnect()", origScriptID);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_Teleport()
//|	Prototype	-	void Teleport( togoto )
//|					void Teleport( x, y )
//|					void Teleport( x, y, z )
//|					void Teleport( x, y, z, world )
//|					void Teleport( x, y, z, world, instanceId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Teleports object to specified location
//o------------------------------------------------------------------------------------------------o
static bool CBase_Teleport(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	std::string className = JS::GetClass(obj.get())->name;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj.get(), 0);
	SI16 x = -1;
	SI16 y = -1;
	SI08 z = myObj->GetZ();
	UI08 world = myObj->WorldNumber();
	UI16 instanceId = myObj->GetInstanceId();

	switch (argc)
	{
		// Just Teleport...
	case 0:
		if (className == "UOXChar")
		{
			(static_cast<CChar*>(myObj))->Teleport();
			return true;
		}
		ScriptError(cx, "For Items you need at least one parameter for Teleport");
		break;

		// Parameters as a string
	case 1:
		if (args.get(0).isObject())
		{	// we can work with this, it should be either a character or item, hopefully
			JS::HandleValue jsToGoTo = args.get(0);
			std::string jsToGoToClassName = JS::GetClass(&jsToGoTo.toObject())->name;
			if (jsToGoToClassName == "UOXItem" || jsToGoToClassName == "UOXChar")
			{
				CBaseObject* toGoTo = JS::GetMaybePtrFromReservedSlot<CBaseObject>(&jsToGoTo.toObject(), 0);
				if (!ValidateObject(toGoTo))
				{
					ScriptError(cx, "No object associated with this object");
					break;
				}

				x = toGoTo->GetX();
				y = toGoTo->GetY();
				z = toGoTo->GetZ();
				world = toGoTo->WorldNumber();
				instanceId = toGoTo->GetInstanceId();
			}
			else if (jsToGoToClassName == "UOXSocket")
			{
				CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(&jsToGoTo.toObject(), 0);
				CChar* mySockChar = mySock->CurrcharObj();
				x = mySockChar->GetX();
				y = mySockChar->GetY();
				z = mySockChar->GetZ();
				world = mySockChar->WorldNumber();
				instanceId = mySockChar->GetInstanceId();
			}
			else
			{
				ScriptError(cx, "Invalid class of object");
				break;
			}
		}
		else if (args.get(0).isInt32())
		{
			UI16 placeNum = args.get(0).toInt32();
			if (cwmWorldState->goPlaces.find(placeNum) != cwmWorldState->goPlaces.end())
			{
				GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
				x = toGoTo.x;
				y = toGoTo.y;
				z = toGoTo.z;
				world = toGoTo.worldNum;
			}
		}
		else	// Needs to be implemented
		{
			ScriptError(cx, "Text-styled Parameters may be added later");
		}
		break;

		// 2 Parameters, x + y
	case 2:
		if (args.get(0).isInt32() && args.get(1).isInt32())
		{
			x = static_cast<SI16>(args.get(0).toInt32());
			y = static_cast<SI16>(args.get(1).toInt32());
		}
		break;

		// x,y,z
	case 3:
		if (args.get(0).isInt32() && args.get(1).isInt32() && args.get(2).isInt32())
		{
			x = static_cast<SI16>(args.get(0).toInt32());
			y = static_cast<SI16>(args.get(1).toInt32());
			z = static_cast<SI08>(args.get(2).toInt32());
		}
		break;

		// x,y,z,world
	case 4:
		if (args.get(0).isInt32() && args.get(1).isInt32() && args.get(2).isInt32() && args.get(3).isInt32())
		{
			x = static_cast<SI16>(args.get(0).toInt32());
			y = static_cast<SI16>(args.get(1).toInt32());
			z = static_cast<SI08>(args.get(2).toInt32());
			world = static_cast<UI08>(args.get(3).toInt32());
		}
		break;

		// x,y,z,world,instanceId
	case 5:
		if (args.get(0).isInt32() && args.get(1).isInt32() && args.get(2).isInt32() && args.get(3).isInt32() && args.get(4).isInt32())
		{
			x = static_cast<SI16>(args.get(0).toInt32());
			y = static_cast<SI16>(args.get(1).toInt32());
			z = static_cast<SI08>(args.get(2).toInt32());
			world = static_cast<UI08>(args.get(3).toInt32());
			instanceId = static_cast<UI16>(args.get(4).toInt32());
		}
		break;

	default:
		ScriptError(cx, "Invalid number of arguments passed to Teleport, needs either 1, 2, 3, 4 or 5");
		break;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (className == "UOXItem")
	{
		CItem* myItem = static_cast<CItem*>(myObj);
		if (!ValidateObject(myItem))
		{
			ScriptError(cx, "Teleport: Invalid Item");
			return false;
		}

		// Update old location of item, which will be used by RemoveFromSight()
		myItem->SetOldLocation(myItem->GetX(), myItem->GetY(), myItem->GetZ());
		myItem->RemoveFromSight();
		myItem->SetLocation(x, y, z, world, instanceId);
	}
	else if (className == "UOXChar")
	{
		CChar* myChar = static_cast<CChar*>(myObj);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "Teleport: Invalid Character");
			return false;
		}

		if ((world != myChar->WorldNumber() || instanceId != myChar->GetInstanceId()) && !myChar->IsNpc())
		{
			CSocket* mySock = myChar->GetSocket();
			if (mySock == nullptr)
				return true;

			if (!Map->InsideValidWorld(x, y, world))
			{
				ScriptError(cx, "Teleport: Not a valid World");
				return false;
			}
			if (myChar->GetInstanceId() != instanceId)
			{
				// Remove all objects in visible range from sight
				myChar->RemoveAllObjectsFromSight(mySock);
			}

			// Remove character being teleported from nearby players sight
			myChar->RemoveFromSight();
			myChar->SetLocation(x, y, z, world, instanceId);
			SendMapChange(world, mySock);

			// Extra update needed for regular UO client
			myChar->Update();
		}
		else
		{
			if (myChar->GetInstanceId() != instanceId)
			{
				myChar->RemoveFromSight();
			}
			myChar->SetLocation(x, y, z, world, instanceId);
		}
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Item JS Method .Teleport()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_StaticEffect()
//|	Prototype	-	void StaticEffect( Effect, speed, loop )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a static effect for character
//o------------------------------------------------------------------------------------------------o
static bool CBase_StaticEffect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UI16 effectId = static_cast<UI16>(args.get(0).toInt32());
	UI08 speed = static_cast<UI08>(args.get(1).toInt32());
	UI08 loop = static_cast<UI08>(args.get(2).toInt32());

	std::string className = JS::GetClass(obj.get())->name;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj.get(), 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "StaticEffect: Invalid Object");
		return false;
	}

	if (className == "UOXItem")
	{
		bool explode = (args.get(3).toBoolean());
		Effects->PlayStaticAnimation(myObj, effectId, speed, loop, explode);
	}
	else
	{
		Effects->PlayStaticAnimation(myObj, effectId, speed, loop);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_MakeMenu()
//|	Prototype	-	void MakeMenu( mnu )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends specified make menu to player
//o------------------------------------------------------------------------------------------------o
static bool CMisc_MakeMenu(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "MakeMenu: Invalid number of arguments (takes 2, number of menu, skill used)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = nullptr;
	std::string className = JS::GetClass(obj.get())->name;
	if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		mySock = myChar->GetSocket();
	}
	else if (className == "UOXSocket")
	{
		mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
	}

	if (mySock == nullptr)
	{
		ScriptError(cx, "MakeMenu: invalid socket");
		return false;
	}
	SI32 menu = args.get(0).toInt32();
	UI08 skillNum = static_cast<UI08>(args.get(1).toInt32());
	Skills->NewMakeMenu(mySock, menu, skillNum);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SoundEffect()
//|	Prototype	-	void SoundEffect( soundId, bAllHear )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a sound effect at object's location
//o------------------------------------------------------------------------------------------------o
static bool CMisc_SoundEffect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 2 || argc > 3)
	{
		ScriptError(cx, "SoundEffect: Invalid number of arguments (takes min 2, max 3)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	std::string className = JS::GetClass(obj.get())->name;
	UI16 soundId = static_cast<UI16>(args.get(0).toInt32());
	SI16 tmpMonsterSound = -1;
	if (argc == 3)
	{
		tmpMonsterSound = static_cast<UI08>(args.get(1).toInt32());
	}
	bool allHear = (args.get(1).toBoolean());

	if (className == "UOXChar" || className == "UOXItem")
	{
		CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj.get(), 0);

		if (ValidateObject(myObj))
		{
			if (className == "UOXChar" && tmpMonsterSound > -1)
			{
				CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
				UI16 monsterSoundToPlay = cwmWorldState->creatures[myChar->GetId()].GetSound(static_cast<monsterSound>(tmpMonsterSound));
				if (monsterSoundToPlay != 0)
				{
					Effects->PlaySound(myChar, monsterSoundToPlay, allHear);
				}
			}
			else
			{
				Effects->PlaySound(myObj, soundId, allHear);
			}
		}
	}
	else if (className == "UOXSocket")
	{
		CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);

		if (mySock != nullptr)
		{
			Effects->PlaySound(mySock, soundId, allHear);
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SellTo()
//|	Prototype	-	bool SellTo( vendorNPC )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up the shopkeeper gump for selling to specified vendor NPC
//o------------------------------------------------------------------------------------------------o
static bool CMisc_SellTo(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	if (argc != 1)
	{
		ScriptError(cx, "SellTo: Invalid Number of Arguments: %d", argc);
		return false;
	}

	std::string className = JS::GetClass(obj.get())->name;
	CChar* myNPC = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(myNPC))
	{
		ScriptError(cx, "SellTo: Invalid NPC");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	CPSellList toSend;
	if (className == "UOXSocket")
	{
		CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
		if (mySock == nullptr)
		{
			ScriptError(cx, "Passed an invalid socket to SellTo");
			return false;
		}

		CChar* mChar = mySock->CurrcharObj();
		if (ValidateObject(mChar))
		{
			myNPC->SetTimer(tNPC_MOVETIME, BuildTimeValue(60.0f));
			if (toSend.CanSellItems((*mChar), (*myNPC)))
			{
				mySock->Send(&toSend);
				args.rval().setBoolean(true);
			}
		}
	}
	else if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "Passed an invalid char to SellTo");
			return false;
		}

		myNPC->SetTimer(tNPC_MOVETIME, BuildTimeValue(60.0f));
		CSocket* mSock = myChar->GetSocket();
		if (toSend.CanSellItems((*myChar), (*myNPC)))
		{
			mSock->Send(&toSend);
			args.rval().setBoolean(true);
		}
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Socket JS Method .SellTo()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_BuyFrom()
//|	Prototype	-	void BuyFrom( vendorNPC )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up the shopkeeper gump for buying from specified vendor NPC
//o------------------------------------------------------------------------------------------------o
static bool CMisc_BuyFrom(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "SellTo: Invalid Number of Arguments: %d", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* myNPC = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(myNPC))
	{
		ScriptError(cx, "BuyFrom: Invalid NPC");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (className == "UOXSocket")
	{
		CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
		if (mySock == nullptr)
		{
			ScriptError(cx, "Invalid source socket in BuyFrom");
			return false;
		}

		if (myNPC->GetNpcAiType() == AI_PLAYERVENDOR)
		{
			mySock->TempObj(myNPC);
			myNPC->TextMessage(mySock, 772, TALK, false);
			mySock->SendTargetCursor(0, TARGET_PLVBUY, " ");
		}
		else
		{
			BuyShop(mySock, myNPC);
		}
	}
	else if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "Passed an invalid char to BuyFrom");
			return false;
		}

		CSocket* mySock = myChar->GetSocket();

		if (myNPC->GetNpcAiType() == AI_PLAYERVENDOR)
		{
			mySock->TempObj(myNPC);
			myNPC->TextMessage(mySock, 772, TALK, false);
			mySock->SendTargetCursor(0, TARGET_PLVBUY, " ");
		}
		else
		{
			BuyShop(mySock, myNPC);
		}
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Socket JS Method .BuyFrom()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_HasSpell()
//|	Prototype	-	bool HasSpell( spellNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for specified spell in first spellbook found in player's inventory
//o------------------------------------------------------------------------------------------------o
static bool CMisc_HasSpell(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "HasSpell: Invalid Number of Arguments: %d", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;

	std::string className = JS::GetClass(obj.get())->name;
	UI08 spellId = static_cast<UI08>(args.get(0).toInt32());

	if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "Invalid char for HasSpell");
			return false;
		}

		CItem* myItem = FindItemOfType(myChar, IT_SPELLBOOK);

		if (!ValidateObject(myItem))
		{
			args.rval().setBoolean(false);
			return true;
		}

		// Code checks for spell based on index starting at 0, while spells have spellIDs starting from 1
		if (Magic->HasSpell(myItem, spellId - 1))
		{
			args.rval().setBoolean(true);
		}
		else
		{
			args.rval().setBoolean(false);
		}
	}
	else if (className == "UOXItem")
	{
		CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj.get(), 0);
		if (!ValidateObject(myItem))
		{
			ScriptError(cx, "Invalid item for HasSpell");
			return false;
		}

		if (Magic->HasSpell(myItem, spellId))
		{
			args.rval().setBoolean(true);
		}
		else
		{
			args.rval().setBoolean(false);
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_RemoveSpell()
//|	Prototype	-	void RemoveSpell( spellNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specified spell from first spellbook found in player's inventory
//o------------------------------------------------------------------------------------------------o
static bool CMisc_RemoveSpell(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "RemoveSpell: Invalid Number of Arguments: %d", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	UI08 spellId = static_cast<UI08>(args.get(0).toInt32());

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (className == "UOXChar")
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "Invalid char for HasSpell");
			return false;
		}

		CItem* myItem = FindItemOfType(myChar, IT_SPELLBOOK);

		if (ValidateObject(myItem))
		{
			Magic->RemoveSpell(myItem, spellId);
		}
	}
	else if (className == "UOXItem")
	{
		CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj.get(), 0);
		if (!ValidateObject(myItem))
		{
			ScriptError(cx, "Invalid item for RemoveSpell");
			return false;
		}

		Magic->RemoveSpell(myItem, spellId);
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Item JS Method .RemoveSpell()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetTag()
//|	Prototype	-	tagdata GetTag( "TagName" )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns value for tag with specified name, if tag has been stored on the object
//o------------------------------------------------------------------------------------------------o
static bool CBase_GetTag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "GetTag: Invalid Count of Parameters: %d, need: 1", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "GetTag: Invalid Object assigned");
		return false;
	}

	std::string localString = convertToString(cx, args.get(0).toString());
	TAGMAPOBJECT localObject = myObj->GetTag(localString);
	if (localObject.m_ObjectType == TAGMAP_TYPE_STRING)
	{
		args.rval().setString(convertFromString(cx, localObject.m_StringValue));
	}
	else if (localObject.m_ObjectType == TAGMAP_TYPE_BOOL)
	{
		args.rval().setBoolean( localObject.m_IntValue == 1 );
	}
	else
	{
		args.rval().setInt32( localObject.m_IntValue );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_SetTag()
//|	Prototype	-	void SetTag( "TagName", "TagStringValue" )
//|					void SetTag( "TagName", TagIntValue )
//|					void SetTag( "TagName", TagBoolValue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Stores persistent tag with specified name and value on object
//o------------------------------------------------------------------------------------------------o
static bool CBase_SetTag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if ((argc != 2) && (argc != 1))
	{
		ScriptError(cx, "SetTag: Invalid Count of Parameters: %d, need: 2", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "SetTag: Invalid Object assigned (SetTag)");
		return false;
	}

	std::string localString = convertToString(cx, args.get(0).toString());
	TAGMAPOBJECT localObject;
	if (argc == 2)
	{
		JS::HandleValue encaps = args.get(1);
		if (encaps.isString())
		{			// String value handling
			const std::string stringVal = convertToString(cx, encaps.toString());
			if (stringVal == "")
			{
				localObject.m_Destroy = true;
				localObject.m_IntValue = 0;
				localObject.m_ObjectType = TAGMAP_TYPE_INT;
				localObject.m_StringValue = "";
			}
			else
			{
				localObject.m_Destroy = false;
				localObject.m_StringValue = stringVal;
				localObject.m_IntValue = static_cast<SI32>(localObject.m_StringValue.length());
				localObject.m_ObjectType = TAGMAP_TYPE_STRING;
			}
		}
		else if (encaps.isBoolean())
		{
			const bool boolVal = encaps.toBoolean();
			if (!boolVal)
			{
				localObject.m_Destroy = true;
				localObject.m_IntValue = 0;
			}
			else
			{
				localObject.m_Destroy = false;
				localObject.m_IntValue = 1;
			}
			localObject.m_ObjectType = TAGMAP_TYPE_BOOL;
			localObject.m_StringValue = "";
		}
		else if (encaps.isInt32())
		{
			const SI32 intVal = encaps.toInt32();
			if (!intVal)
			{
				localObject.m_Destroy = true;
				localObject.m_IntValue = 0;
			}
			else
			{
				localObject.m_Destroy = false;
				localObject.m_IntValue = intVal;
			}
			localObject.m_ObjectType = TAGMAP_TYPE_INT;
			localObject.m_StringValue = "";
		}
		else if (encaps.isNullOrUndefined())
		{
			localObject.m_Destroy = true;
			localObject.m_IntValue = 0;
			localObject.m_ObjectType = TAGMAP_TYPE_INT;
			localObject.m_StringValue = "";
		}
		else
		{
			return true;
		}
		myObj->SetTag(localString, localObject);
	}
	else
	{
		localObject.m_Destroy = true;
		localObject.m_ObjectType = TAGMAP_TYPE_INT;
		localObject.m_IntValue = 0;
		localObject.m_StringValue = "";
		myObj->SetTag(localString, localObject);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetTempTag()
//|	Prototype	-	tagdata GetTempTag( "TagName" )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns value for temporary tag with specified name, if tag has been stored on the object
//o------------------------------------------------------------------------------------------------o
static bool CBase_GetTempTag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "GetTempTag: Invalid Count of Parameters: %d, need: 1", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "GetTempTag: Invalid Object assigned");
		return false;
	}

	std::string localString = convertToString(cx, args.get(0).toString());
	TAGMAPOBJECT localObject = myObj->GetTempTag(localString);
	if (localObject.m_ObjectType == TAGMAP_TYPE_STRING)
	{
		args.rval().setString(convertFromString(cx, localObject.m_StringValue));
	}
	else if (localObject.m_ObjectType == TAGMAP_TYPE_BOOL)
	{
		args.rval().setBoolean( (localObject.m_IntValue == 1) );
	}
	else
	{
		args.rval().setInt32( localObject.m_IntValue );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_SetTempTag()
//|	Prototype	-	void SetTempTag( "TagName", "TagStringValue" )
//|					void SetTempTag( "TagName", TagIntValue )
//|					void SetTempTag( "TagName", TagBoolValue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Stores temporary tag with specified name and value on object, does not persist
//|					across server restart (or character reconnect)
//o------------------------------------------------------------------------------------------------o
static bool CBase_SetTempTag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if ((argc != 2) && (argc != 1))
	{
		ScriptError(cx, "SetTempTag: Invalid Count of Parameters: %d, need: 2", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "SetTempTag: Invalid Object assigned");
		return false;
	}

	std::string localString = convertToString(cx, args.get(0).toString());
	TAGMAPOBJECT localObject;
	if (argc == 2)
	{
		JS::HandleValue encaps = args.get(1);
		if (encaps.isString())
		{			// String value handling
			const std::string stringVal = convertToString( cx, encaps.toString() );
			if (stringVal == "")
			{
				localObject.m_Destroy = true;
				localObject.m_IntValue = 0;
				localObject.m_ObjectType = TAGMAP_TYPE_INT;
				localObject.m_StringValue = "";
			}
			else
			{
				localObject.m_Destroy = false;
				localObject.m_StringValue = stringVal;
				localObject.m_IntValue = static_cast<SI32>(localObject.m_StringValue.length());
				localObject.m_ObjectType = TAGMAP_TYPE_STRING;
			}
		}
		else if (encaps.isBoolean())
		{
			const bool boolVal = encaps.toBoolean();
			if (!boolVal)
			{
				localObject.m_Destroy = true;
				localObject.m_IntValue = 0;
			}
			else
			{
				localObject.m_Destroy = false;
				localObject.m_IntValue = 1;
			}
			localObject.m_ObjectType = TAGMAP_TYPE_BOOL;
			localObject.m_StringValue = "";
		}
		else if (encaps.isInt32())
		{
			const SI32 intVal = encaps.toInt32();
			if (!intVal)
			{
				localObject.m_Destroy = true;
				localObject.m_IntValue = 0;
			}
			else
			{
				localObject.m_Destroy = false;
				localObject.m_IntValue = intVal;
			}
			localObject.m_ObjectType = TAGMAP_TYPE_INT;
			localObject.m_StringValue = "";
		}
		else if (encaps.isNullOrUndefined())
		{
			localObject.m_Destroy = true;
			localObject.m_IntValue = 0;
			localObject.m_ObjectType = TAGMAP_TYPE_INT;
			localObject.m_StringValue = "";
		}
		else
		{
			return true;
		}
		myObj->SetTempTag(localString, localObject);
	}
	else
	{
		localObject.m_Destroy = true;
		localObject.m_ObjectType = TAGMAP_TYPE_INT;
		localObject.m_IntValue = 0;
		localObject.m_StringValue = "";
		myObj->SetTempTag(localString, localObject);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetNumTags()
//|	Prototype	-	int GetNumTags()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns number of tags stored on the object
//o------------------------------------------------------------------------------------------------o
static bool CBase_GetNumTags(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Invalid Count of Parameters: %d, need: 0", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Invalid Object assigned (GetNumTags)");
		return false;
	}

	args.rval().setInt32(myObj->GetNumTags());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetTagMap()
//|	Prototype	-	array GetTagMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map of tags stored on object
//o------------------------------------------------------------------------------------------------o
static bool CBase_GetTagMap(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Invalid Count of Parameters: %d, need: 0", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Invalid Object assigned (GetTagMap)");
		return false;
	}

	// Fetch tag map from object
	TAGMAP2 tagMap = myObj->GetTagMap();

	// Create main JSObject to store full list of tags
	JSObject* jsTagMap = JS_NewArrayObject(cx, 0, nullptr);

	// Iterate over tag map to fetch details on each tag
	int i = 0;
	for (auto& tagObj : tagMap)
	{
		// Create JSObject for current tag
		JSObject* jsTag = JS_NewArrayObject(cx, 0, nullptr);

		// Convert tag name to JSString
		JSString* tagName = JS_NewStringCopyZ(cx, tagObj.first.c_str());
		jsval jsTagName = STRING_TO_JSVAL(tagName);

		// Add tag name to JSObject for tag
		JS_SetElement(cx, jsTag, 0, &jsTagName);

		// Fetch type of tag, and value of tag
		jsval jsType;
		jsval jsValue;
		switch (tagObj.second.m_ObjectType)
		{
		case TAGMAP_TYPE_INT:
			jsType = INT_TO_JSVAL(TAGMAP_TYPE_INT);
			jsValue = INT_TO_JSVAL(tagObj.second.m_IntValue);
			break;
		case TAGMAP_TYPE_STRING:
		{
			jsType = INT_TO_JSVAL(TAGMAP_TYPE_STRING);
			JSString* jsStringVal = JS_NewStringCopyZ(cx, tagObj.second.m_StringValue.c_str());
			jsValue = STRING_TO_JSVAL(jsStringVal);
			break;
		}
		case TAGMAP_TYPE_BOOL:
			jsType = INT_TO_JSVAL(TAGMAP_TYPE_BOOL);
			jsValue = BOOLEAN_TO_JSVAL(tagObj.second.m_IntValue != 0);
			break;
		default:
			// Unhandled tag type detected!
			jsType = JSVAL_NULL;
			jsValue = JSVAL_NULL;
			break;
		}

		// Add tag type and value to JSObject for tag
		JS_SetElement(cx, jsTag, 1, &jsType);
		JS_SetElement(cx, jsTag, 2, &jsValue);

		// Add JSObject for tag to main jsTagMap object
		jsval subTagObj = OBJECT_TO_JSVAL(jsTag);
		JS_SetElement(cx, jsTagMap, i, &subTagObj);
		i++;
	}

	args.rval().setObjectOrNull( jsTagMap );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetTempTagMap()
//|	Prototype	-	array GetTempTagMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map of temporary tags stored on object
//o------------------------------------------------------------------------------------------------o
static bool CBase_GetTempTagMap(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Invalid Count of Parameters: %d, need: 0", argc);
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Invalid Object assigned (GetTempTagMap)");
		return false;
	}

	// Fetch tag map from object
	TAGMAP2 tagMap = myObj->GetTempTagMap();

	// Create main JSObject to store full list of tags
	JSObject* jsTagMap = JS_NewArrayObject(cx, 0, nullptr);

	// Iterate over tag map to fetch details on each tag
	int i = 0;
	for (auto& tagObj : tagMap)
	{
		// Create JSObject for current tag
		JSObject* jsTag = JS_NewArrayObject(cx, 0, nullptr);

		// Convert tag name to JSString
		JSString* tagName = JS_NewStringCopyZ(cx, tagObj.first.c_str());
		jsval jsTagName = STRING_TO_JSVAL(tagName);

		// Add tag name to JSObject for tag
		JS_SetElement(cx, jsTag, 0, &jsTagName);

		// Fetch type of tag, and value of tag
		jsval jsType;
		jsval jsValue;
		switch (tagObj.second.m_ObjectType)
		{
		case TAGMAP_TYPE_INT:
			jsType = INT_TO_JSVAL(TAGMAP_TYPE_INT);
			jsValue = INT_TO_JSVAL(tagObj.second.m_IntValue);
			break;
		case TAGMAP_TYPE_STRING:
		{
			jsType = INT_TO_JSVAL(TAGMAP_TYPE_STRING);
			JSString* jsStringVal = JS_NewStringCopyZ(cx, tagObj.second.m_StringValue.c_str());
			jsValue = STRING_TO_JSVAL(jsStringVal);
			break;
		}
		case TAGMAP_TYPE_BOOL:
			jsType = INT_TO_JSVAL(TAGMAP_TYPE_BOOL);
			jsValue = BOOLEAN_TO_JSVAL(tagObj.second.m_IntValue != 0);
			break;
		default:
			// Unhandled tag type detected!
			jsType = JSVAL_NULL;
			jsValue = JSVAL_NULL;
			break;
		}

		// Add tag type and value to JSObject for tag
		JS_SetElement(cx, jsTag, 1, &jsType);
		JS_SetElement(cx, jsTag, 2, &jsValue);

		// Add JSObject for tag to main jsTagMap object
		jsval subTagObj = OBJECT_TO_JSVAL(jsTag);
		JS_SetElement(cx, jsTagMap, i, &subTagObj);
		i++;
	}

	args.rval().setObjectOrNull( jsTagMap );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_OpenBank()
//|	Prototype	-	void OpenBank( trgSock )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens bankbox of character for the specified socket
//o------------------------------------------------------------------------------------------------o
static bool CChar_OpenBank(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "OpenBank: Invalid Character object assigned");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	CSocket* mySock = nullptr;
	// Open the bank of myChar to myChar
	if (argc == 0)
	{
		mySock = myChar->GetSocket();
		if (mySock != nullptr)
		{
			mySock->OpenBank(myChar);
		}
	}
	// Open it to the passed socket
	else if (argc == 1)
	{
		mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(args.get(0).toObjectOrNull(), 0);
		if (mySock != nullptr)
		{
			mySock->OpenBank(myChar);
		}
	}
	else
	{
		ScriptError(cx, "OpenBank, Invalid count of Paramters: %d", argc);
		return false;
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char JS Method .OpenBank()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_OpenContainer()
//|	Prototype	-	void OpenContainer( iCont )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified container for specified socket
//o------------------------------------------------------------------------------------------------o
static bool CSocket_OpenContainer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "OpenContainer: Invalid socket");
		return false;
	}

	if (argc < 1 || argc > 2)
	{
		ScriptError(cx, "OpenContainer, Invalid count of Paramters: %d", argc);
		return false;
	}

	CItem* contToOpen = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (ValidateObject(contToOpen))
	{
		// Keep track of original script that's executing
		auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
		auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

		mSock->OpenPack(contToOpen, false);

		// Active script-context might have been lost, so restore it...
		if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
		{
			// ... by calling a dummy function in original script!
			restoreContext(origScript, "Socket JS Method .OpenContainer()", origScriptID);
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_OpenLayer()
//|	Prototype	-	void OpenLayer( socket, layerToOpen )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified layer of character for the specified socket
//o------------------------------------------------------------------------------------------------o
static bool CChar_OpenLayer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "OpenLayer: Invalid Character object assigned");
		return false;
	}

	if (argc != 2)
	{
		ScriptError(cx, "OpenLayer, Invalid count of Paramters: %d", argc);
		return false;
	}
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(args.get(0).toObjectOrNull(), 0);
	if (mySock != nullptr)
	{
		CItem* iLayer = myChar->GetItemAtLayer(static_cast<ItemLayers>(args.get(1).toInt32()));
		if (ValidateObject(iLayer))
		{
			// Keep track of original script that's executing
			auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
			auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

			mySock->OpenPack(iLayer);

			// Active script-context might have been lost, so restore it...
			if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
			{
				// ... by calling a dummy function in original script!
				restoreContext(origScript, "Char JS Method .OpenLayer()", origScriptID);
			}
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_TurnToward()
//|	Prototype	-	void TurnToward( targetObject )
//|					void TurnToward( x, y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Turns character to face object/location
//o------------------------------------------------------------------------------------------------o
static bool CChar_TurnToward(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(TurnToward) Wrong object assigned");
		return false;
	}

	SI16 x, y;

	if (argc == 1)
	{
		if (!args.get(0).isObject())
		{
			ScriptError(cx, "(TurnToward) Invalid Object passed");
			return false;
		}

		CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(args.get(0).toObjectOrNull(), 0);
		if (!ValidateObject(myObj))
		{
			ScriptError(cx, "(TurnToward) Invalid Object passed");
			return false;
		}

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if (argc == 2)
	{
		// x + y
		x = static_cast<SI16>(args.get(0).toInt32());
		y = static_cast<SI16>(args.get(1).toInt32());
	}
	else
	{
		ScriptError(cx, "(TurnToward) Wrong paramter count: %d, needs either one char/item or x+y", argc);
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	// Just don't do anything if NewDir eq OldDir
	UI08 newDir = Movement->Direction(myChar, x, y);

	if (newDir != myChar->GetDir())
	{
		CSocket* mySock = myChar->GetSocket();
		if (mySock != nullptr)
		{
			// Update direction, but don't mark character as "dirty" to add them to update queue - we want to send the update straight away
			myChar->SetDir(newDir, false);
			CPDrawGamePlayer gpToSend((*myChar));
			myChar->GetSocket()->Send(&gpToSend);
			myChar->Update(nullptr, false, false);
		}
		else
		{
			// NPCs have no socket, treat slightly different!
			myChar->SetDir(newDir, false);
			myChar->Update(myChar->GetSocket(), true);
		}
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .TurnToward()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_DirectionTo()
//|	Prototype	-	unsigned char DirectionTo( trgObj )
//|					unsigned char DirectionTo( x, y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets direction from character to target object/location
//o------------------------------------------------------------------------------------------------o
static bool CChar_DirectionTo(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(DirectionTo) Wrong object assigned");
		return false;
	}

	SI16 x, y;

	if (argc == 1)
	{
		if (!args.get(0).isObject())
		{
			ScriptError(cx, "(DirectionTo) Invalid Object passed");
			return false;
		}

		CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(args.get(0).toObjectOrNull(), 0);

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if (argc == 2)
	{
		// x + y
		x = static_cast<SI16>(args.get(0).toInt32());
		y = static_cast<SI16>(args.get(1).toInt32());
	}
	else
	{
		ScriptError(cx, "(DirectionTo) Wrong paramter count: %d, needs either one char/item or x+y", argc);
		return false;
	}

	// Just don't do anything if NewDir eq OldDir

	UI08 NewDir = Movement->Direction(myChar, x, y);

	args.rval().setInt32(NewDir);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ExecuteCommand()
//|	Prototype	-	void ExecuteCommand( cmdString )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes specified command (with cmd params as part of cmdString) for character
//o------------------------------------------------------------------------------------------------o
static bool CChar_ExecuteCommand(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "ExecuteCommand: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JSString* targMessage = JS_ValueToString(cx, argv[0]);
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	char* trgMessage = JS_GetStringBytes(targMessage);
	CSocket* targSock = myChar->GetSocket();
	if (targSock == nullptr || trgMessage == nullptr)
	{
		ScriptError(cx, "ExecuteCommand: Invalid socket or speech (%s)", targMessage);
		return false;
	}
	Commands->Command(targSock, myChar, trgMessage);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild_AcceptRecruit()
//|	Prototype	-	void AcceptRecruit()
//|					void AcceptRecruit( trgChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Accepts specified character as a recruit in the guild
//o------------------------------------------------------------------------------------------------o
static bool CGuild_AcceptRecruit(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CGuild* myGuild = JS::GetMaybePtrFromReservedSlot<CGuild>(obj, 0);

	if (myGuild == nullptr)
	{
		ScriptError(cx, "(AcceptRecruit) Invalid Object assigned");
		return false;
	}

	// Two choices here... 0 paramteres = accept the JS_GetParent object
	// 1 parameter = get the cchar from there
	if (argc == 0)
	{
		JSObject* Parent = JS_GetParent(cx, obj);
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(Parent, 0);
		myGuild->RecruitToMember(*myChar);
	}
	else if (argc == 1)
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
		myGuild->RecruitToMember(*myChar);
	}
	else
	{
		ScriptError(cx, "(AcceptRecruit) Invalid Parameter Count: %d", argc);
		return false;
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild_IsAtPeace()
//|	Prototype	-	bool IsAtPeace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if guild is at peace, i.e. not at war with any other guilds
//o------------------------------------------------------------------------------------------------o
static bool CGuild_IsAtPeace(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "(IsAtPeace) Invalid Parameter Count: %d", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CGuild* myGuild = JS::GetMaybePtrFromReservedSlot<CGuild>(obj, 0);
	if (myGuild == nullptr)
	{
		ScriptError(cx, "(IsAtPeace) Invalid Object assigned");
		return false;
	}

	args.rval().setBoolean(myGuild->IsAtPeace());

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ResourceCount()
//|	Prototype	-	int ResourceCount( realId, colour )
//|					int ResourceCount( realId, colour, moreVal )
//|					int ResourceCount( realId, colour, moreVal, sectionId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the amount of the items of given ID, colour and moreVal character has in packs
//o------------------------------------------------------------------------------------------------o
static bool CChar_ResourceCount(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(ResourceCount) Invalid Object assigned");
		return false;
	}

	UI16 realId = static_cast<UI16>(args.get(0).toInt32());
	SI32 itemColour = 0;
	SI64 moreVal = -1;
	std::string sectionId = "";

	if ((argc < 1) || (argc > 4))
	{
		ScriptError(cx, "(ResourceCount) Invalid count of parameters: %d, needs from 1 to 4 parameters", argc);
		return false;
	}

	if (argc >= 2)
	{
		itemColour = static_cast<SI32>(args.get(1).toInt32());
	}
	if (argc >= 3)
	{
		moreVal = static_cast<SI64>(args.get(2).toInt32());
	}
	if (argc >= 4)
	{
		sectionId = convertToString(cx, args.get(3).toString());
	}

	bool colorCheck = (itemColour != -1 ? true : false);
	bool moreCheck = (moreVal != -1 ? true : false);

	args.rval().setInt32(GetItemAmount(myChar, realId, static_cast<UI16>(itemColour), static_cast<UI32>(moreVal), colorCheck, moreCheck, sectionId));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_UseResource()
//|	Prototype	-	int UseResource( amount, realItemId )
//|					int UseResource( amount, realItemId, colour )
//|					int UseResource( amount, realItemId, colour, moreVal )
//|					int UseResource( amount, realItemId, colour, moreVal, sectionId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specified amount of items of given ID, colour and MORE value from
//|					char's packs, and returns amount deleted
//o------------------------------------------------------------------------------------------------o
static bool CBase_UseResource(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CBaseObject* myObj = static_cast<CBaseObject*>(myClass.toObject());

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "(UseResource) Invalid Object assigned");
		return false;
	}

	UI32 amount = static_cast<UI32>(args.get(0).toInt32());
	UI16 realId = static_cast<UI16>(args.get(1).toInt32());
	SI32 itemColour = 0;
	SI64 moreVal = -1;
	std::string sectionId = "";

	// Min. 2 Arguments (amount + id) or max 5 (amount + id + color + moreVal + sectionId)
	if ((argc < 2) || (argc > 5))
	{
		ScriptError(cx, "(UseResource) Invalid count of parameters: %d, needs from 2 to 5 parameters", argc);
		return false;
	}

	if (argc >= 3)
	{
		itemColour = static_cast<SI32>(args.get(2).toInt32());
	}
	if (argc >= 4)
	{
		moreVal = static_cast<SI64>(args.get(3).toInt32());
	}
	if (argc >= 5)
	{
		sectionId = convertToString(cx, args.get(4).toString());
	}

	bool colorCheck = (itemColour != -1 ? true : false);
	bool moreCheck = (moreVal != -1 ? true : false);

	UI32 retVal = 0;

	if (className == "UOXChar")
	{
		CChar* myChar = static_cast<CChar*>(myObj);
		retVal = DeleteItemAmount(myChar, amount, realId, static_cast<UI16>(itemColour), static_cast<UI32>(moreVal), colorCheck, moreCheck, sectionId);
	}
	else
	{
		CItem* myItem = static_cast<CItem*>(myObj);
		retVal = DeleteSubItemAmount(myItem, amount, realId, static_cast<UI16>(itemColour), static_cast<UI32>(moreVal), colorCheck, moreCheck, sectionId);
	}
	args.rval().setInt32(retVal);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_BoltEffect()
//|	Prototype	-	void BoltEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the lightning bolt effect on specified character to all nearby
//o------------------------------------------------------------------------------------------------o
static bool CChar_BoltEffect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (ValidateObject(myChar))
	{
		Effects->Bolteffect(myChar);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_CustomTarget()
//|	Prototype	-	void CustomTarget( tNum, toSay )
//|					function onCallback{tNum}( tSock, targSerial )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Provides player with a custom target cursor that calls back into a specific
//|					function in script when a target has been selected.
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Very similar to PopUpTarget. It's a callback situation instead. tNum must be
//|					between 0 and 255. Says toSay, and shows a cursor. Reenters the script
//|					associated with the socket's player (ie player who gets the cursor).
//|					Enters function name based on the value of tNum. if tNum is 0, then the
//|					function would be onCallback0.
//|
//|					Useable with both sockets and characters.
//o------------------------------------------------------------------------------------------------o
static bool CMisc_CustomTarget(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;

	if ((argc > 3) || (argc < 1))
	{
		ScriptError(cx, "(CustomTarget) Invalid count of parameters: %d, either needs 1, 2 or 3 (targetID, textToShow, cursorType", argc);
		return false;
	}

	CSocket* mySock = nullptr;

	if (className == "UOXChar")
	{
		// Character
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);

		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "(CustomTarget) Invalid Character assigned");
			return false;
		}

		mySock = myChar->GetSocket();
	}
	else if (className == "UOXSocket")
	{
		// We have a socket here
		mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
	}

	if (mySock == nullptr)
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return true;
	}

	mySock->scriptForCallBack = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	//mySock->TempInt( static_cast<SI64>( JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ))));
	UI08 tNum = static_cast<UI08>(args.get(0).toInt32());

	constexpr auto maxsize = 512; // Could become long (make sure it's nullptr )
	std::string toSay;
	if (argc >= 2)
	{
		toSay = convertToString(cx, args.get(1).toString());
		if (toSay.size() > maxsize)
		{
			toSay = toSay.substr(0, maxsize);
		}
	}

	UI08 cursorType = 0;
	if (argc == 3)
	{
		cursorType = static_cast<UI08>(args.get(2).toInt32());
	}

	if (cursorType == 3)
	{
		mySock->SendTargetCursor(1, 0x00000000, toSay, 3);
	}
	else
	{
		mySock->SendTargetCursor(1, tNum, toSay, cursorType);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_PopUpTarget()
//|	Prototype	-	void PopUpTarget( tNum, toSay )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Provides a call to an inbuilt popup target. tNum must be between 0 and 255
//|					inclusive. Says toSay, and shows a cursor. Note that this allows access
//|					potentially to GM functions.
//o------------------------------------------------------------------------------------------------o
static bool CMisc_PopUpTarget(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if ((argc > 2) || (argc < 1))
	{
		ScriptError(cx, "(PopUpTarget) Invalid count of parameters: %d, either needs 1 or 2", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	// Either useable with sockets OR characters
	std::string className = JS::GetClass(obj.get())->name;
	CSocket* mySock = nullptr;

	if (className == "UOXChar")
	{
		// Character
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);

		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "(PopUpTarget) Invalid Character assigned");
			return false;
		}

		mySock = myChar->GetSocket();
	}

	else if (className == "UOXSocket")
	{
		// We have a socket here
		mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
	}

	if (mySock == nullptr)
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return true;
	}

	UI08 tNum = static_cast<UI08>(args.get(0).toInt32());

	std::string toSay;
	if (argc == 2)
	{
		toSay = oldstrutil::format(512, "%s", convertToString( cx, args.get( 1 ).toString() ).c_str() );
	}

	mySock->SendTargetCursor(0, tNum, toSay);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_InRange()
//|	Prototype	-	bool InRange( trgObj, distance )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the distance to trgObj is less than distance
//o------------------------------------------------------------------------------------------------o
static bool CBase_InRange(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "(InRange): Invalid count of parameters: %d needs 2 (Item/Char and distance)", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UI16 distance = static_cast<UI16>(args.get(1).toInt32());

	CBaseObject* me = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(me))
	{
		ScriptError(cx, "(InRange) Invalid Object assigned to self");
		return false;
	}

	CBaseObject* them = JS::GetMaybePtrFromReservedSlot<CBaseObject>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(them))
	{
		ScriptError(cx, "(InRange) Invalid Object assigned to target");
		return false;
	}

	if ((them->GetObjType() == OT_ITEM) && (me->GetObjType() == OT_CHAR))
	{
		CItem* myItem = static_cast<CItem*>(them);
		if (myItem->GetCont() != nullptr)
		{
			args.rval().setBoolean(FindItemOwner(myItem) == me);
			return true;
		}
	}

	args.rval().setBoolean(ObjInRange(me, them, distance));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_StartTimer()
//|	Prototype	-	void StartTimer( numMiliSec, timerId, callback )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Starts timer for object that lasts specified amount of milliseconds.
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	timerId must be between 0 and 100. Resulting onTimer event will be triggered.
//|					Which script it triggers in depends on the (optional, defaults to false) third
//|					parameter, which can be
//|						false (DEFAULT if not specified, calls back into object's script)
//|						true (calls back to existing script)
//|						int (scriptId to call back to)
//o------------------------------------------------------------------------------------------------o
static bool CBase_StartTimer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "(StartTimer) Invalid Object assigned");
		return false;
	}

	if (argc != 2 && argc != 3)
	{
		ScriptError(cx, "(StartTimer) Invalid count of parameter: %d (needs 2 or 3)", argc);
		return false;
	}

	// 1. Parameter Delay, 2. Parameter Callback
	UI32 ExpireTime = BuildTimeValue(args.get(0).toInt32() / 1000.0f);
	UI16 TriggerNum = static_cast<UI16>(args.get(1).toInt32());

	CTEffect* Effect = new CTEffect;

	if (argc == 3)
	{
		if (args.get(2).isBoolean() )	// Is it a boolean?  If so, might be calling back into here
		{
			if (args.get(2).toBoolean())
			{
				Effect->AssocScript(JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx)));
			}
			else
			{
				Effect->More2(0xFFFF);
			}
		}
		else
		{
			Effect->More2(static_cast<UI16>(args.get(2).toInt32()));
		}
	}
	else
	{
		Effect->More2(0xFFFF);
	}

	Effect->Destination(myObj->GetSerial());
	Effect->ExpireTime(ExpireTime);
	Effect->Source(myObj->GetSerial());
	Effect->Number(40);
	Effect->More1(TriggerNum);

	cwmWorldState->tempEffects.Add(Effect);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CheckSkill()
//|	Prototype	-	bool CheckSkill( skillnum, minskill, maxskill[, isCraftSkill] )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Performs a skillcheck for character based on specified skill. Returns true
//|					if result of skillcheck is between provided minimum and maximum values.
//|					isCraftSkill is a boolean flag to indicate if skill check is done for a craft skill;
//|					if true, an alternate skill check formula is used that gives player a minimum 50% chance
//|					if they at least meat the minimum requirements for crafting an item
//o------------------------------------------------------------------------------------------------o
static bool CChar_CheckSkill(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3 && argc != 4)
	{
		ScriptError(cx, "CheckSkill: Invalid number of arguments (takes 3 or 4, skillNum, minSkill, maxSkill and isCraftSkill (optional))");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "CheckSkill: Invalid character");
		return false;
	}

	UI08 skillNum = static_cast<UI08>(args.get(0).toInt32());
	UI16 minSkill = static_cast<UI16>(args.get(1).toInt32());
	UI16 maxSkill = static_cast<UI16>(args.get(2).toInt32());
	bool isCraftSkill = false;
	if (argc == 4)
	{
		isCraftSkill = args.get(3).toBoolean();
	}
	args.rval().setBoolean(Skills->CheckSkill(myChar, skillNum, minSkill, maxSkill, isCraftSkill));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_FindItemLayer()
//|	Prototype	-	Item FindItemLayer( layer )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for item found on specified layer of character
//o------------------------------------------------------------------------------------------------o
static bool CChar_FindItemLayer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* myItem = nullptr;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(FindItemLayer) Invalid Charobject assigned");
		return false;
	}

	if (argc == 1)
	{
		myItem = myChar->GetItemAtLayer(static_cast<ItemLayers>(args.get(0).toInt32()));
	}
	else
	{
		ScriptError(cx, "(FindItemLayer) Unknown Count of Arguments: %d, needs: 1", argc);
		return false;
	}

	if (!ValidateObject(myItem))
	{
		args.rval().setNull();
		return true;
	}

	JSObject* myJSItem = JSEngine->AcquireObject(IUE_ITEM, myItem, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));

	args.rval().setObjectOrNull(myJSItem);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_FindItemType()
//|	Prototype	-	Item FindItemType( itemType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for item of specific item type in character's backpack
//o------------------------------------------------------------------------------------------------o
static bool CChar_FindItemType(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(FindItemType) Invalid Count of Arguments, takes 1");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(FindItemType) Invalid Charobject assigned");
		return false;
	}

	UI08 iType = static_cast<UI08>(args.get(0).toInt32());

	CItem* myItem = FindItemOfType(myChar, static_cast<ItemTypes>(iType));
	if (!ValidateObject(myItem))
	{
		args.rval().setNull();
		return true;
	}

	JSObject* myJSItem = JSEngine->AcquireObject(IUE_ITEM, myItem, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
	args.rval().setObjectOrNull(myJSItem);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_FindItemSection()
//|	Prototype	-	Item FindItemSection( sectionId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for item with specific sectionId in character's backpack
//o------------------------------------------------------------------------------------------------o
static bool CChar_FindItemSection(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(FindItemSection) Invalid Count of Arguments, takes 1");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(FindItemSection) Invalid Charobject assigned");
		return false;
	}

	std::string sectionID = convertToString(cx, args.get(0).toString());

	CItem* myItem = FindItemOfSectionId(myChar, sectionID);
	if (!ValidateObject(myItem))
	{
		args.rval().setNull();
		return true;
	}

	JSObject* myJSItem = JSEngine->AcquireObject(IUE_ITEM, myItem, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));

	args.rval().setObjectOrNull(myJSItem);

	return true;
}

void OpenPlank(CItem* p);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_OpenPlank()
//|	Prototype	-	void OpenPlank()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens plank for item (boat)
//o------------------------------------------------------------------------------------------------o
static bool CItem_OpenPlank(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "(OpenPlank) Invalid Count of Arguments: %d, needs: 0", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);

	if (!ValidateObject(myItem))
	{
		ScriptError(cx, "(OpenPlank) Invalid Object assigned");
		return false;
	}

	OpenPlank(myItem);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpeechInput()
//|	Prototype	-	void SpeechInput( speechId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls up the onSpeechInput event using specified ID, with the text the user types
//o------------------------------------------------------------------------------------------------o
static bool CChar_SpeechInput(JSContext* cx, unsigned argc, JS::Value* vp)
{
	// Get our own Script ID
	UI08 speechId = 0;
	CItem* speechItem = nullptr;

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc == 1) // Just the ID has been passed
	{
		speechId = static_cast<UI08>(args.get(0).toInt32());
	}
	else if (argc == 2) // We got an "affected" item as well
	{
		speechId = static_cast<UI08>(args.get(0).toInt32());

		if ( !args.get( 1 ).isNullOrUndefined() )
		{
			JSObject* myObj = args.get(1).toObjectOrNull();
			speechItem = JS::GetMaybePtrFromReservedSlot<CItem>(myObj, 0);
		}
	}
	else
	{
		ScriptError(cx, "(SpeechInput) Invalid Count of parameters: %d, needs: either 1 or 2", argc);
		return false;
	}

	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(SpeechInput) Invalid object assigned");
		return false;
	}

	myChar->SetSpeechMode(9);
	if (ValidateObject(speechItem))
	{
		myChar->SetSpeechItem(speechItem);
	}
	else
	{
		myChar->SetSpeechItem(nullptr);
	}

	myChar->SetSpeechId(speechId);
	myChar->SetSpeechCallback(JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CastSpell()
//|	Prototype	-	bool CastSpell( spellNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes character to cast specified spell
//o------------------------------------------------------------------------------------------------o
static bool CChar_CastSpell(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if ((argc != 1) && (argc != 2))
	{
		ScriptError(cx, "(CastSpell) Invalid Number of Arguments %d, needs: 1 or 2", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "(CastSpell) Invalid object assigned");
		return false;
	}

	SI08 spellCast = static_cast<SI08>(args.get(0).toInt32());

	if (myChar->IsNpc())
	{
		myChar->SetSpellCast(spellCast);
		Magic->CastSpell(nullptr, myChar);
	}
	else
	{
		CSocket* sock = myChar->GetSocket();

		if ((argc == 2) && args.get( 1 ).toBoolean() )
		{
			// Next...
			myChar->SetSpellCast(spellCast);
			Magic->CastSpell(sock, myChar);
		}
		else
		{
			bool spellSuccess = Magic->SelectSpell(sock, spellCast);
			args.rval().setBoolean(spellSuccess);
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_MagicEffect()
//|	Prototype	-	void MagicEffect( spellId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies spell effects of specified spell to character
//o------------------------------------------------------------------------------------------------o
static bool CChar_MagicEffect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI08 spellId = static_cast<SI08>(args.get(0).toInt32());

	CChar* myObj = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "StaticEffect: Invalid Object");
		return false;
	}

	Magic->DoStaticEffect(myObj, spellId);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetSerial()
//|	Prototype	-	int GetSerial( part )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a part (1-4) of a character's serial
//o------------------------------------------------------------------------------------------------o
static bool CChar_GetSerial(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myObj = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	UI08 part = static_cast<UI08>(args.get(0).toInt32());

	if (!ValidateObject(myObj) || (part == 0) || (part > 4))
	{
		ScriptError(cx, "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)");
		args.rval().setInt32(0);
		return false;
	}

	args.rval().setInt32(myObj->GetSerial(part));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetSerial()
//|	Prototype	-	int GetSerial( part )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a part (1-4) of an object's serial
//o------------------------------------------------------------------------------------------------o
static bool CBase_GetSerial(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	UI08 part = static_cast<UI08>(args.get(0).toInt32());

	if (!ValidateObject(myObj) || (part == 0) || (part > 4))
	{
		ScriptError(cx, "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)");
		args.rval().setInt32(0);
		return false;
	}

	args.rval().setInt32(myObj->GetSerial(part));

	return true;
}

void UpdateStats(CBaseObject* mObj, UI08 x);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_UpdateStats()
//|	Prototype	-	void UpdateStats( statType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends update to client with specified stat (health, mana or stamina) for object
//| Notes		-	Can be used with any character, as well as with items/multis with damageable flag enabled
//o------------------------------------------------------------------------------------------------o
bool CBase_UpdateStats(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(UpdateStats) Invalid Number of Arguments %d, needs: 1 (stat type - 0, 1 or 2 for Health, Mana or Stamina)", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	UI08 statType = static_cast<UI08>(args.get(0).toInt32());

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "UpdateStats: Invalid object assigned");
		return false;
	}

	if (myObj->CanBeObjType(OT_MULTI) || myObj->CanBeObjType(OT_ITEM))
	{
		if (statType != 0)
		{
			ScriptError(cx, "UpdateStatus: For Items/Multis, only the Health stat (type 0) can be sent as an update to the client");
			return false;
		}

		if (!myObj->IsDamageable())
		{
			ScriptError(cx, "UpdateStatus: Can only be used with characters, or Items/Multis with damagable flag set to true");
			return false;
		}
	}

	switch (statType)
	{
	case 0: // Health
		UpdateStats(myObj, 0);
		break;
	case 1: // Mana - only relevant for characters
		UpdateStats(myObj, 1);
		break;
	case 2: // Stamina - only relevant for characters
		UpdateStats(myObj, 2);
		break;
	default:
		ScriptError(cx, "UpdateStats: Argument can only contain values 0, 1 or 2 for Health, Mana or Stamina respectively");
		return false;
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SetPoisoned()
//|	Prototype	-	void SetPoisoned( poisonLevel, Length )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies a specified level of poison to the character for a specified amount of
//|					time (in milliseconds).
//o------------------------------------------------------------------------------------------------o
static bool CChar_SetPoisoned(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 2 || argc > 3)
	{
		ScriptError(cx, "(SetPoisoned) Invalid Number of Arguments %d, needs: 2 or 3", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	if (!ValidateObject(myChar) || myChar->GetObjType() != OT_CHAR)
	{
		ScriptError(cx, "(SetPoisoned) Invalid object assigned");
		return false;
	}

	SI08 newVal = static_cast<SI08>(args.get(0).toInt32());

	if (newVal > 0 && argc > 1)
	{
		SI32 wearOff = static_cast<SI32>(args.get(1).toInt32());

		if (argc == 2 || (argc == 3 && args.get(2).toBoolean()))
		{
			if (myChar->GetPoisoned() > newVal)
			{
				newVal = myChar->GetPoisoned();
			}
		}
		myChar->SetTimer(tCHAR_POISONWEAROFF, BuildTimeValue(static_cast<R32>(wearOff) / 1000.0f));
	}

	//myChar->SetPoisonStrength( newVal );
	myChar->SetPoisoned(newVal);
	return true;
}

void ExplodeItem(CSocket* mSock, CItem* nItem, UI32 damage, UI08 damageType, bool explodeNearby);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ExplodeItem()
//|	Prototype	-	void ExplodeItem( myItem, damage, damageType, explodeNearby )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes specified item by exploding it, dealing 5-10 dmg to nearby characters
//o------------------------------------------------------------------------------------------------o
static bool CChar_ExplodeItem(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 4)
	{
		ScriptError(cx, "(ExplodeItem) Invalid Number of Arguments %d, needs: 4", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	JSObject* tObj = args.get(0).toObjectOrNull();
	CBaseObject* trgObj = JS::GetMaybePtrFromReservedSlot<CBaseObject >(tObj, 0);

	if (!ValidateObject(trgObj) || trgObj->GetObjType() != OT_ITEM || myChar->GetSocket() == nullptr)
	{
		ScriptError(cx, "(ExplodeItem) Invalid object");
		return false;
	}

	UI32 damage = static_cast<UI32>(args.get(1).toInt32());
	UI08 damageType = static_cast<UI08>(args.get(2).toInt32());
	bool explodeNearby = (args.get(3).toBoolean());

	ExplodeItem(myChar->GetSocket(), static_cast<CItem*>(trgObj), damage, damageType, explodeNearby);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SetInvisible()
//|	Prototype	-	void SetInvisible( visibility, timeLength )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets character to the specified visibility level for the specified amount of
//|					time (in milliseconds).
//o------------------------------------------------------------------------------------------------o
static bool CChar_SetInvisible(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 1 || argc > 2)
	{
		ScriptError(cx, "(SetInvisible) Invalid Number of Arguments %d, needs: 1 or 2", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	UI08 newVal = static_cast<UI08>(args.get(0).toInt32());

	myChar->SetVisible(static_cast<VisibleTypes>(newVal));
	if (argc == 2)
	{
		UI32 TimeOut = static_cast<UI32>(args.get(1).toInt32());
		myChar->SetTimer(tCHAR_INVIS, BuildTimeValue(static_cast<R32>(TimeOut) / 1000.0f));
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_SetCont()
//|	Prototype	-	bool SetCont( object )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets container of item to be the specified object
//o------------------------------------------------------------------------------------------------o
static bool CItem_SetCont(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	JSObject* tObj = args.get(0).toObjectOrNull();
	CBaseObject* trgObj = JS::GetMaybePtrFromReservedSlot<CBaseObject >(tObj, 0);

	if (!ValidateObject(myItem) || !ValidateObject(trgObj) || (trgObj->GetSerial() == INVALIDSERIAL))
	{
		ScriptError(cx, "SetCont: Invalid Object/Argument, takes 1 arg: containerobject");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	// return true if the change was successful, false otherwise
	args.rval().setBoolean(myItem->SetCont(trgObj));

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Item JS Method .SetCont()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_IsMulti()
//|	Prototype	-	bool IsMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if item is a multi
//o------------------------------------------------------------------------------------------------o
static bool CItem_IsMulti(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 0)
	{
		ScriptError(cx, "(IsMulti) Invalid Number of Arguments %d, needs: 0", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);

	if (!ValidateObject(myItem))
	{
		ScriptError(cx, "(IsMulti) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myItem->CanBeObjType(OT_MULTI));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_IsBoat()
//|	Prototype	-	bool IsBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if item is a boat
//o------------------------------------------------------------------------------------------------o
static bool CBase_IsBoat(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 0)
	{
		ScriptError(cx, "(IsBoat) Invalid Number of Arguments %d, needs: 0", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CBaseObject* myObject = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObject))
	{
		ScriptError(cx, "(IsMulti) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myObject->CanBeObjType(OT_BOAT));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsInMulti()
//|	Prototype	-	bool IsInMulti( object )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the object is in the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsInMulti(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(IsInMulti) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsInMulti) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CBaseObject* toFind = JS::GetMaybePtrFromReservedSlot<CBaseObject >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(IsInMulti) Invalid object in house");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean((toFind->GetMultiObj() == myItem));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnBanList()
//|	Prototype	-	bool IsOnBanList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the ban-list of the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsOnBanList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(IsOnBanList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsOnBanList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(IsOnBanList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myItem->IsOnBanList(toFind));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnFriendList()
//|	Prototype	-	bool IsOnFriendList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the friend-list of the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsOnFriendList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(IsOnFriendList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsOnFriendList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(IsOnFriendList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myItem->IsOnFriendList(toFind));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnGuestList()
//|	Prototype	-	bool IsOnGuestList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the guest-list of the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsOnGuestList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(IsOnGuestList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsOnGuestList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(IsOnGuestList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myItem->IsOnGuestList(toFind));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnOwnerList()
//|	Prototype	-	bool IsOnOwnerList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the owner-list of the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsOnOwnerList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(IsOnOwnerList) Invalid Number of Arguments %d, needs: 1 or 2", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsOnOwnerList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(IsOnOwnerList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myItem->IsOnOwnerList(toFind));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOwner()
//|	Prototype	-	bool IsOwner( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is the actual owner of the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsOwner(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(IsOwner) Invalid Number of Arguments %d, needs: 1 or 2", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsOwner) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(IsOwner) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(myItem->IsOwner(toFind));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToBanList()
//|	Prototype	-	bool AddToBanList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to banlist of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_AddToBanList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(AddToBanList) Invalid Number of Arguments %d, needs: 1", argc);
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(AddToBanList) Invalid object assigned");
		return false;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(AddToBanList) Invalid character");
		return false;
	}

	args.rval().setBoolean(true);
	myItem->AddToBanList(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToFriendList()
//|	Prototype	-	bool AddToFriendList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to the friend-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_AddToFriendList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(AddToFriendList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(AddToFriendList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(AddToFriendList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->AddAsFriend(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToGuestList()
//|	Prototype	-	bool AddToGuestList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to the guest-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_AddToGuestList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(AddToGuestList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(AddToGuestList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(AddToGuestList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->AddAsGuest(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToOwnerList()
//|	Prototype	-	bool AddToOwnerList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to the owner-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_AddToOwnerList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(AddToOwnerList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(AddToOwnerList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(AddToOwnerList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->AddAsOwner(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromBanList()
//|	Prototype	-	bool RemoveFromBanList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the ban-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_RemoveFromBanList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(RemoveFromBanList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(RemoveFromBanList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(RemoveFromBanList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->RemoveFromBanList(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromFriendList()
//|	Prototype	-	bool RemoveFromFriendList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the friend-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_RemoveFromFriendList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(RemoveFromFriendList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(RemoveFromFriendList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(RemoveFromFriendList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->RemoveAsFriend(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromGuestList()
//|	Prototype	-	bool RemoveFromGuestList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the guest-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_RemoveFromGuestList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(RemoveFromGuestList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(RemoveFromGuestList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(RemoveFromGuestList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->RemoveAsGuest(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromOwnerList()
//|	Prototype	-	bool RemoveFromOwnerList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the owner-list of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_RemoveFromOwnerList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 1)
	{
		ScriptError(cx, "(RemoveFromOwnerList) Invalid Number of Arguments %d, needs: 1", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(RemoveFromOwnerList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}
	CChar* toFind = JS::GetMaybePtrFromReservedSlot<CChar >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "(RemoveFromOwnerList) Invalid character");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->RemoveAsOwner(toFind);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearBanList()
//|	Prototype	-	bool ClearBanList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears multi's list of banned players
//o------------------------------------------------------------------------------------------------o
static bool CMulti_ClearBanList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 0)
	{
		ScriptError(cx, "(ClearBanList) Invalid Number of Arguments %d, needs: 0", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(ClearBanList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->ClearBanList();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearFriendList()
//|	Prototype	-	bool ClearFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the multi's list of friends
//o------------------------------------------------------------------------------------------------o
static bool CMulti_ClearFriendList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 0)
	{
		ScriptError(cx, "(ClearFriendList) Invalid Number of Arguments %d, needs: 0", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(ClearFriendList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->ClearFriendList();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearGuestList()
//|	Prototype	-	bool ClearGuestList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the multi's list of guests
//o------------------------------------------------------------------------------------------------o
static bool CMulti_ClearGuestList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 0)
	{
		ScriptError(cx, "(ClearGuestList) Invalid Number of Arguments %d, needs: 0", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(ClearGuestList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->ClearGuestList();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearOwnerList()
//|	Prototype	-	bool ClearOwnerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the multi's list of co-owners
//o------------------------------------------------------------------------------------------------o
static bool CMulti_ClearOwnerList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (argc != 0)
	{
		ScriptError(cx, "(ClearOwnerList) Invalid Number of Arguments %d, needs: 0", argc);
		args.rval().setBoolean(false);
		return true;
	}

	CMultiObj* myItem = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(ClearOwnerList) Invalid object assigned");
		args.rval().setBoolean(false);
		return true;
	}

	args.rval().setBoolean(true);
	myItem->ClearOwnerList();
	return true;
}

UI16 HandleAutoStack(CItem* mItem, CItem* mCont, CSocket* mSock = nullptr, CChar* mChar = nullptr);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_PlaceInPack()
//|	Prototype	-	void PlaceInPack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Randomly sets the placement/position of an item inside the container it is in.
//|					If second parameter is true, UOX3 will attempt to stack the item with an existing item
//|					instead.
//o------------------------------------------------------------------------------------------------o
static bool CItem_PlaceInPack(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "(PlaceInPack) Invalid Number of Arguments %d, needs: 0 or 1", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	if (!ValidateObject(myItem) || !myItem->CanBeObjType(OT_ITEM))
	{
		ScriptError(cx, "(PlaceInPack) Invalid object assigned");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	auto autoStack = (args.get(0).toBoolean());
	if (autoStack && ValidateObject(myItem->GetCont()))
	{
		// Attempt to stack the item with existing items. If item has any left-over amount after, it
		// will be placed randomly in pack
		CItem* myCont = static_cast<CItem*>(myItem->GetCont());
		if (ValidateObject(myCont))
		{
			args.rval().setInt32(HandleAutoStack(myItem, myCont, nullptr, nullptr));
		}
	}

	// Place item (or left-over item) randomly in pack
	if (ValidateObject(myItem))
	{
		myItem->PlaceInPack();
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Item JS Method .PlaceInPack()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_OpenURL()
//|	Prototype	-	void OpenURL( UrlString )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified URL in player's browser
//o------------------------------------------------------------------------------------------------o
static bool CSocket_OpenURL(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "OpenURL: Invalid Number of Arguments %d, needs: 1");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "OpenURL: Invalid socket!");
		return false;
	}
	std::string url = convertToString(cx, args.get(0).toString());
	mySock->OpenURL(url);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetByte()
//|	Prototype	-	unsigned char GetByte( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns value of a byte from the socket at offset, assumes 0 to 255 as values
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetByte(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "GetByte: Invalid Number of Arguments %d, needs: 1 (offset)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "GetByte: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	args.rval().setInt32(mySock->GetByte(offset));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetSByte()
//|	Prototype	-	signed char GetSByte( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 1 byte of data from socket buffer at offset, assumes -127 to 127 as values
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetSByte(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "GetSByte: Invalid Number of Arguments %d, needs: 1 (offset)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "GetSByte: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	args.rval().setInt32(static_cast<SI08>(mySock->GetByte(offset)));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetWord()
//|	Prototype	-	unsigned int GetWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 2 bytes of data from socket buffer at offset, assumes positive values
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "GetWord: Invalid Number of Arguments %d, needs: 1 (offset)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "GetWord: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	args.rval().setInt32(mySock->GetWord(offset));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetSWord()
//|	Prototype	-	signed int GetSWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 2 bytes of data from socket buffer at offset, negative values accepted
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetSWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "GetSWord: Invalid Number of Arguments %d, needs: 1 (offset)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "GetSWord: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	args.rval().setInt32(static_cast<SI16>(mySock->GetWord(offset)));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetDWord()
//|	Prototype	-	unsigned int GetDWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 4 bytes of data from socket buffer at offset, positive values assumed
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetDWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "GetDWord: Invalid Number of Arguments %d, needs: 1 (offset)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "GetDWord: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	args.rval().setInt32(static_cast<UI32>(mySock->GetDWord(offset)));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetSDWord()
//|	Prototype	-	signed int GetSDWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 4 bytes of data from socket buffer at offset, negative values accepted
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetSDWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1) // 1 parameters
	{
		ScriptError(cx, "GetSDWord: Invalid Number of Arguments %d, needs: 1 (offset)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "GetSDWord: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	args.rval().setInt32(static_cast<SI32>(mySock->GetDWord(offset)));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetString()
//|	Prototype	-	string GetString( offset [, length] )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns data from socket buffer at offset with optional length param, string assumed
//|					If no length param is provided, reads until next null terminator
//o------------------------------------------------------------------------------------------------o
static bool CSocket_GetString(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 2)
	{
		ScriptError(cx, "GetString: Invalid number of arguments. Takes 1 (offset) or 2 (offset, length)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "GetString: Invalid socket!");
		return false;
	}

	SI32 length = -1;
	SI32 offset = args.get(0).toInt32();
	if (argc == 2)
	{
		length = args.get(1).toInt32();
	}

	char toReturn[128];
	if (length != -1)
	{
		strncopy(toReturn, 128, (char*)&(mSock->Buffer())[offset], length);
		toReturn[length] = 0;
	}
	else
	{
		// If length is not defined, read until next null terminator
		strcopy(toReturn, 128, (char*)&(mSock->Buffer())[offset]);
	}

	args.rval().setString( convertFromString( cx, toReturn ) );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetByte()
//|	Prototype	-	void SetByte( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets 1 byte at socket stream's offset to equal 8-bit value
//o------------------------------------------------------------------------------------------------o
static bool CSocket_SetByte(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SetByte: Invalid number of arguments (takes 3)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "SetByte: Invalid socket!");
		return false;
	}
	SI32 offset = args.get(0).toInt32();
	UI08 byteToSet = static_cast<UI08>(args.get(1).toInt32());

	mSock->SetByte(offset, byteToSet);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetWord()
//|	Prototype	-	void SetWord( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets 2 bytes at socket stream's offset to equal 16-bit value
//o------------------------------------------------------------------------------------------------o
static bool CSocket_SetWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SetWord: Invalid number of arguments (takes 3)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "SetWord: Invalid socket!");
		return false;
	}

	SI32 offset = args.get(0).toInt32();
	UI16 byteToSet = static_cast<UI16>(args.get(1).toInt32());

	mSock->SetWord(offset, byteToSet);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetDWord()
//|	Prototype	-	void SeDtWord( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets 4 bytes at socket stream's offset to equal 32-bit value
//o------------------------------------------------------------------------------------------------o
static bool CSocket_SetDWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SetDWord: Invalid number of arguments (takes 3)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "SetDWord: Invalid socket!");
		return false;
	}

	SI32 offset = args.get(0).toInt32();
	UI32 byteToSet = args.get(1).toInt32();

	mSock->SetDWord(offset, byteToSet);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetString()
//|	Prototype	-	void SetString( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets data at socket stream's offset to equal string value
//o------------------------------------------------------------------------------------------------o
static bool CSocket_SetString(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SetString: Invalid number of arguments (takes 3)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "SetString: Invalid socket!");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	SI32 offset = args.get(0).toInt32();
	auto trgMessage = convertToString(cx, args.get(1).toString());
	if (trgMessage.empty())
	{
		ScriptError(cx, "SetString: No string to set");
		return false;
	}
	// FIXME
	auto size = trgMessage.length();
	strcopy((char*)&(mSock->Buffer())[offset], size + 1, trgMessage.c_str());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_ReadBytes()
//|	Prototype	-	void ReadBytes( bytecount )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads specified amount of bytes from socket
//o------------------------------------------------------------------------------------------------o
static bool CSocket_ReadBytes(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "ReadBytes: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "ReadBytes: Invalid socket!");
		return false;
	}

	SI32 bCount = args.get(0).toInt32();
	mSock->Receive(bCount);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_WhoList()
//|	Prototype	-	void WhoList( bSendOnList )
//|					void WhoList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a gump populated with all online or offline (if param is false) players
//o------------------------------------------------------------------------------------------------o
static bool CSocket_WhoList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mySock == nullptr)
	{
		ScriptError(cx, "WhoList: Invalid socket!");
		return false;
	}

	bool sendOnList = true;
	if (argc == 1)
	{
		sendOnList = (args.get(0).toBoolean());
	}

	if (sendOnList)
	{
		WhoList->FlagUpdate(); // Always update the list of online players... 
		WhoList->SendSocket(mySock);
	}
	else
	{
		OffList->SendSocket(mySock);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Music()
//|	Prototype	-	void Music( musicNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to socket to play specified midi/mp3
//o------------------------------------------------------------------------------------------------o
static bool CSocket_Music(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Music: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UI16 music = static_cast<UI16>(args.get(0).toInt32());

	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock != nullptr)
	{
		Effects->PlayMusic(mySock, music);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_YellMessage()
//|	Prototype	-	void YellMessage( message )
//|	Date		-	January 30, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Yells a text message to those in range
//o------------------------------------------------------------------------------------------------o
static bool CChar_YellMessage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "YellMessage: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	std::string trgMessage = convertToString(cx, args.get(0).toString());

	if (trgMessage.empty() || trgMessage == "")
	{
		ScriptError(cx, "YellMessage: You have to supply a messagetext");
	}

	CChar* myChar = static_cast<CChar*>(myObj);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "YellMessage: Invalid Character");
		return false;
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (myChar->GetNpcAiType() == AI_EVIL || myChar->GetNpcAiType() == AI_EVIL_CASTER)
	{
		MethodSpeech(*myChar, trgMessage.c_str(), YELL, 0x0026, static_cast<FontType>(myChar->GetFontType()), SPTRG_PCNPC, INVALIDSERIAL, useUnicode);
	}
	else
	{
		MethodSpeech(*myChar, trgMessage.c_str(), YELL, myChar->GetSayColour(), static_cast<FontType>(myChar->GetFontType()), SPTRG_PCNPC, INVALIDSERIAL, useUnicode);
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char JS Method .YellMessage()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_WhisperMessage()
//|	Prototype	-	void WhisperMessage( message )
//|	Date		-	January 30, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Whispers a text message to those in range
//o------------------------------------------------------------------------------------------------o
static bool CChar_WhisperMessage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "WhisperMessage: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	std::string trgMessage = convertToString(cx, args.get(0).toString());

	if (trgMessage.empty() || trgMessage == "")
	{
		ScriptError(cx, "WhisperMessage: You have to supply a messagetext");
	}

	CChar* myChar = static_cast<CChar*>(myObj);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "WhisperMessage: Invalid Character");
		return false;
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (myChar->GetNpcAiType() == AI_EVIL || myChar->GetNpcAiType() == AI_EVIL_CASTER)
	{
		MethodSpeech(*myChar, trgMessage.c_str(), WHISPER, 0x0026, static_cast<FontType>(myChar->GetFontType()), SPTRG_PCNPC, INVALIDSERIAL, useUnicode);
	}
	else
	{
		MethodSpeech(*myChar, trgMessage.c_str(), WHISPER, myChar->GetSayColour(), static_cast<FontType>(myChar->GetFontType()), SPTRG_PCNPC, INVALIDSERIAL, useUnicode);
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char JS Method .WhisperMessage()", origScriptID);
	}

	return true;
}

void BuildGumpFromScripts(CSocket* s, UI16 m);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_OpenGump()
//|	Prototype	-	void OpenGump( gumpMenuId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified gumpmenu from dfnata/misc/gumps.dfn for socket
//o------------------------------------------------------------------------------------------------o
static bool CSocket_OpenGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "OpenGump: Invalid number of arguments (takes 1, number)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "You have to pass a valid menu number");
	}

	UI16 menuNumber = static_cast<UI16>(args.get(0).toInt32());
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "OpenGump: Unknown Object has been passed");
		return false;
	}

	BuildGumpFromScripts(mySock, menuNumber);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_CloseGump()
//|	Prototype	-	void CloseGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes specified generic gump based on its ID, and provides a button ID response
//o------------------------------------------------------------------------------------------------o
static bool CSocket_CloseGump(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "CloseGump: Invalid number of arguments (takes 2 - gumpId to close, and buttonId to send as response)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);

	if (mySock == nullptr)
	{
		ScriptError(cx, "SysMessage: Invalid socket");
		return false;
	}

	UI32 gumpId = static_cast<UI32>(args.get(0).toInt32());
	UI32 buttonId = static_cast<UI32>(args.get(1).toInt32());

	CPCloseGump gumpToClose(gumpId, buttonId);
	mySock->Send(&gumpToClose);

	return true;
}

// Race methods

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_CanWearArmour()
//|	Prototype	-	bool CanWearArmour( tItem )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if members of race can equip specified item
//o------------------------------------------------------------------------------------------------o
static bool CRace_CanWearArmour(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "CanWearArmour: Invalid number of arguments (takes 1, number)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CRace* myRace = JS::GetMaybePtrFromReservedSlot<CRace>(obj, 0);
	if (myRace == nullptr)
	{
		ScriptError(cx, "CanWearArmour: Unknown Object has been passed");
		return false;
	}

	CItem* toFind = JS::GetMaybePtrFromReservedSlot<CItem >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(toFind))
	{
		ScriptError(cx, "CanWearArmour: Invalid item passed");
		return true;
	}

	ARMORCLASS srcClass = myRace->ArmourClassRestriction();
	ARMORCLASS trgClass = toFind->GetArmourClass();
	args.rval().setBoolean((trgClass == 0) || ((srcClass & trgClass) != 0));	// they have a matching class
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_IsValidHairColour()
//|	Prototype	-	bool IsValidRaceHairColour( tColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified hair-colour is allowed for members of race
//o------------------------------------------------------------------------------------------------o
static bool CRace_IsValidHairColour(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "IsValidHairColour: Invalid number of arguments (takes 1, number)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CRace* myRace = JS::GetMaybePtrFromReservedSlot<CRace>(obj, 0);
	if (myRace == nullptr)
	{
		ScriptError(cx, "IsValidHairColour: Unknown Object has been passed");
		return false;
	}

	COLOUR cVal = static_cast<COLOUR>(args.get(0).toInt32());
	args.rval().setBoolean((myRace->IsValidHair(cVal)));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_IsValidSkinColour()
//|	Prototype	-	bool IsValidSkinColour( tColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified skin-colour is allowed for members of race
//o------------------------------------------------------------------------------------------------o
static bool CRace_IsValidSkinColour(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "IsValidSkinColour: Invalid number of arguments (takes 1, number)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CRace* myRace = JS::GetMaybePtrFromReservedSlot<CRace>(obj, 0);
	if (myRace == nullptr)
	{
		ScriptError(cx, "IsValidSkinColour: Unknown Object has been passed");
		return false;
	}

	COLOUR cVal = static_cast<COLOUR>(args.get(0).toInt32());
	args.rval().setBoolean((myRace->IsValidSkin(cVal)));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_IsValidBeardColour()
//|	Prototype	-	bool IsValidBeardColour( tColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified beard-colour is allowed for members of race
//o------------------------------------------------------------------------------------------------o
static bool CRace_IsValidBeardColour(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "IsValidBeardColour: Invalid number of arguments (takes 1, number)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CRace* myRace = JS::GetMaybePtrFromReservedSlot<CRace>(obj, 0);
	if (myRace == nullptr)
	{
		ScriptError(cx, "IsValidBeardColour: Unknown Object has been passed");
		return false;
	}

	COLOUR cVal = static_cast<COLOUR>(args.get(0).toInt32());
	args.rval().setBoolean((myRace->IsValidBeard(cVal)));
	return true;
}


bool ApplyItemSection(CItem* applyTo, CScriptSection* toApply, std::string sectionId);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_ApplySection()
//|	Prototype	-	void ApplySection( scriptSection )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies the values from a DFN section to an Item/Character
//o------------------------------------------------------------------------------------------------o
static bool CBase_ApplySection(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "ApplySection: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	std::string trgSection = convertToString(cx, args.get(0).toString());

	if (trgSection.empty() || trgSection.length() == 0)
	{
		ScriptError(cx, "You have to supply a section to apply");
		return false;
	}

	if (className == "UOXItem")
	{
		CItem* myItem = static_cast<CItem*>(myObj);
		if (!ValidateObject(myItem))
		{
			ScriptError(cx, "ApplySection: Invalid Item");
			return false;
		}
		CScriptSection* toFind = FileLookup->FindEntry(trgSection, items_def);
		ApplyItemSection(myItem, toFind, trgSection);
	}
	else if (className == "UOXChar")
	{
		CChar* myChar = static_cast<CChar*>(myObj);
		if (!ValidateObject(myChar))
		{
			ScriptError(cx, "ApplySection: Invalid Character");
			return false;
		}

		CScriptSection* toFind = FileLookup->FindEntry(trgSection, npc_def);
		Npcs->ApplyNpcSection(myChar, toFind, trgSection);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddSpell()
//|	Prototype	-	void AddSpell( spellNum )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a spell to the first spell book found in character's pack
//o------------------------------------------------------------------------------------------------o
static bool CChar_AddSpell(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddSpell: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	UI08 spellNum = static_cast<UI08>(args.get(0).toInt32());
	CItem* sBook = FindItemOfType(myChar, IT_SPELLBOOK);
	if (ValidateObject(sBook))
	{
		// Keep track of original script that's executing
		auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
		auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

		Magic->AddSpell(sBook, spellNum);

		// Active script-context might have been lost, so restore it...
		if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
		{
			// ... by calling a dummy function in original script!
			restoreContext(origScript, "Char JS Method .AddSpell()", origScriptID);
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpellFail()
//|	Prototype	-	void SpellFail()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Does the actions associated with spell failure, called after the failure happens
//o------------------------------------------------------------------------------------------------o
static bool CChar_SpellFail(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "SpellFail: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);

	Effects->PlayStaticAnimation(myChar, 0x3735, 0, 30);
	Effects->PlaySound(myChar, 0x005C);
	if (!myChar->IsNpc())
	{
		CSocket* mSock = myChar->GetSocket();
		if (mSock != nullptr)
		{
			myChar->TextMessage(mSock, 771, EMOTE, false);
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_Refresh()
//|	Prototype	-	void Refresh()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes the item to be refreshed to sockets that can see it
//o------------------------------------------------------------------------------------------------o
static bool CBase_Refresh(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Refresh: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Refresh: Invalid object assigned - only Charaacters or Items accepted");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (myObj->CanBeObjType(OT_CHAR))
	{
		CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
		if (ValidateObject(myChar))
		{
			myChar->Update();
		}
	}
	else if (myObj->CanBeObjType(OT_ITEM))
	{
		CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
		if (ValidateObject(myItem))
		{
			myItem->Update();
		}
	}
	else
	{
		ScriptError(cx, "Refresh: Invalid object assigned - only Charaacters or Items accepted");
		return false;
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char/Item JS Method .Refresh()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_ApplyRank()
//|	Prototype	-	void ApplyRank( rank, maxRank )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Modifies item properties based on item's rank (could be calculated based on
//|					player's crafting skill, but is always 10 if rank system not enabled. Higher
//|					values will increase base properties of item, lower will decrease base
//|					properties). maxRank is the maximum amount of ranks in the rank system (10 by default).
//o------------------------------------------------------------------------------------------------o
static bool CItem_ApplyRank(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "ApplyRank: Invalid number of arguments (takes 2)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	SI32 rank = args.get(0).toInt32();
	SI32 maxrank = args.get(1).toInt32();

	Skills->ApplyRank(nullptr, myItem, rank, maxrank);
	return true;
}

bool IsOnFoodList(const std::string& sFoodList, const UI16 sItemId);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_IsOnFoodList()
//|	Prototype	-	bool IsOnFoodList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if item is on a specified food list
//o------------------------------------------------------------------------------------------------o
static bool CItem_IsOnFoodList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 || argc > 7)
	{
		ScriptError(cx, "(IsOnFoodList) Invalid Number of Arguments %d, needs: 1 - foodlist name", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* myItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);

	if (!ValidateObject(myItem))
	{
		ScriptError(cx, "(IsOnFoodList) Invalid object assigned");
		return true;
	}

	if (!args.get(0).isString())
	{
		ScriptError(cx, "IsOnFoodList: Invalid parameter specifled, string required!");
		return false;
	}
	std::string foodList = convertToString(cx, args.get(0).toString());

	args.rval().setBoolean(IsOnFoodList(foodList, myItem->GetId()));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CAccount_AddAccount()
//|	Prototype	-	void AddAccount( username, password, email, flags )
//|	Date		-	8/15/2003 10:43:18 PM
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add an account through the JS. The calling script will be required to specifiy
//|					the username, password, flags, and comment/contact information.
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Removed UOXAccountWrapper and exposed global var Accounts
//o------------------------------------------------------------------------------------------------o
static bool CAccount_AddAccount(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 4)
	{
		ScriptError(cx, "Account.AddAccount(user,pass,email,flags): Invalid number of arguments (takes 4)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	// Ok get our object from the global context
	if (!args.get( 0 ).isString() || !args.get( 1 ).isString() || !args.get( 2 ).isString() || !(args.get(3).isInt32() || args.get( 3 ).isString()))
	{
		ScriptError(cx, "Account.AddAccount(user,pass,email,flags): Invalid parameter specifled, please check param types.");
		return false;
	}
	std::string lpszUsername = convertToString(cx, args.get(0).toString());
	std::string lpszPassword = convertToString(cx, args.get(1).toString());
	std::string lpszComment = convertToString(cx, args.get(2).toString());
	UI16 u16Flags = 0;

	if (args.get(3).isInt32())
	{
		u16Flags = static_cast<UI16>(args.get(3).toInt32());
	}
	else
	{
		u16Flags = static_cast<UI16>(std::stoul(convertToString(cx, args.get( 3 ).toString())), nullptr, 0);
	}

	if (lpszUsername.empty() || lpszPassword.empty() || lpszComment.empty() || lpszUsername.length() == 0 || lpszPassword.length() == 0 || lpszComment.length() == 0)
		return false;


	if (!Accounts->AddAccount(lpszUsername, lpszPassword, lpszComment, u16Flags))
	{
		ScriptError(cx, "Account.AddAccount( user, pass, email, flags ): Account was not added. Duplicate expected.");
		return false;
	}
	return true;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CAccount_DelAccount()
//|	Prototype	-	void DelAccount( username )
//|					void DelAccount( accountId )
//|	Date		-	8/21/2003 2:39:27 PM
///o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove an account from the memory. This is a permanent action that CAN NOT be
//|					undone. After a call to this function the account will be immidiatly removed
//|					from memory, and will not be written out to the accounts.adm file, and will
//|					have the associated directory tagged and renamed.
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Removed UOXAccountWrapper and exposed global var Accounts
//o------------------------------------------------------------------------------------------------o
static bool CAccount_DelAccount(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Account.DelAccount([username/id]): Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	// Ok get out object from the global context
	if (args.get(0).isString())
	{
		auto lpszUsername = convertToString(cx, args.get(0).toString());
		if (!Accounts->DelAccount(lpszUsername))
		{
			ScriptError(cx, " Account.DelAccount(username): Unable to remove account specified.");
			return false;
		}

	}
	else if (args.get(0).isInt32())
	{
		UI16 ui16AccountId = static_cast<UI16>(args.get(0).toInt32());
		if (!Accounts->DelAccount(ui16AccountId))
		{
			ScriptError(cx, " Account.DelAccount(accountID): Unable to remove account specified.");
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

// Basic file wrapping structure for abstracting away file IO for the JS file funcs
// UOXCFile constructor !
static bool UOXCFile(JSContext* cx, unsigned argc, JS::Value* vp)
{
	UOXFileWrapper_st* toAdd = new UOXFileWrapper_st;
	toAdd->mWrap = nullptr;

	//JSBool myVal = JS_DefineFunctions( cx, obj, CFile_Methods );
	JS_DefineFunctions(cx, obj, CFile_Methods);
	JS::SetReservedSlot(obj, 0, JS::PrivateValue(toAdd));
	JS_LockGCThing(cx, obj);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Free()
//|	Prototype	-	void Free()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Frees memory allocated by file
//o------------------------------------------------------------------------------------------------o
static bool CFile_Free(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Free: Invalid number of arguments (takes 0)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);
	delete mFile;
	JS::SetReservedSlot(obj, 0, JS::UndefinedValue());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Open()
//|	Prototype	-	void Open( string filename, string accessMethod )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a file for reading, writing or appending
//o------------------------------------------------------------------------------------------------o
static bool CFile_Open(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 2 || argc > 4)
	{
		ScriptError(cx, "Open: Invalid number of arguments (takes 2 to 4 - filename, file mode and - optionally - folderName and useScriptDataDir bool)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	auto fileName = convertToString(cx, args.get(0).toString());
	std::string mode = convertToString(cx, args.get(1).toString());
	std::string folderName;
	if (argc >= 3)
	{
		folderName = convertToString(cx, args.get(2).toString());
	}
	bool useScriptDataDir = false;
	if (argc >= 4)
	{
		useScriptDataDir = (args.get(3).toBoolean());
	}

	if (oldstrutil::lower(mode).find_first_of("rwa", 0, 3) == std::string::npos)
	{
		ScriptError(cx, "Open: Invalid mode must be \"read\", \"write\", or \"append\"!");
		return false;
	}
	if (fileName.find( ".." ) != std::string::npos || fileName.find("\\") != std::string::npos  || fileName.find("/") != std::string::npos )
	{
		ScriptError(cx, "Open: file names may not contain \"..\", \"\\\", or \"/\".");
		return false;
	}

	std::string filePath = cwmWorldState->ServerData()->Directory(CSDDP_SHARED);

	// if folderName argument was supplied, use it, and create the appropriate folder under the /shared/ folder
	if (!folderName.empty() && folderName.length() != 0 )
	{
		// However, don't allow special characters in the folder name
		if (folderName.find("..") != std::string::npos || folderName.find("\\") != std::string::npos || folderName.find("/") != std::string::npos)
		{
			ScriptError(cx, "Open: folder names may not contain \"..\", \"\\\", or \"/\".");
			return false;
		}

		// If script wants to look in script-data folder instead of shared folder, let it
		if (useScriptDataDir)
		{
			filePath = cwmWorldState->ServerData()->Directory(CSDDP_SCRIPTDATA);
		}

		// Append the provided folder name
		filePath.append(folderName);

		if (!std::filesystem::exists(filePath))
		{
			// Create missing directory
			std::filesystem::create_directory(filePath);
		}

		filePath += std::filesystem::path::preferred_separator;
	}

	filePath.append(fileName);
	FILE* stream;

	mFile->mWrap = mfopen(&stream, filePath.c_str(), oldstrutil::lower(mode).substr(0, 1).c_str());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Close()
//|	Prototype	-	void Close()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes a file
//o------------------------------------------------------------------------------------------------o
static bool CFile_Close(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Open: Invalid number of arguments (takes 0)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	fclose(mFile->mWrap);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Read()
//|	Prototype	-	string Read( int numBytes )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a string of length numBytes, reading numBytes from the opened file
//o------------------------------------------------------------------------------------------------o
static bool CFile_Read(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Read: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	if (!mFile || !mFile->mWrap || feof(mFile->mWrap))
	{
		ScriptError(cx, "Read: Error reading file, is not opened or has reached EOF");
		return false;
	}
	SI32 bytes = args.get(0).toInt32();
	char data[512];

	if (bytes > 512 || bytes < 1)
	{
		ScriptError(cx, "Read: Invalid byte count, must be from 1 to 512!");
		return false;
	}

	// We don't care about return value, so suppress compiler warning
	size_t bytesRead = fread(data, 1, bytes, mFile->mWrap);

	args.rval().setString( convertFromString( cx, data ) );
	return true;
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_ReadUntil()
//|	Prototype	-	string ReadUntil( string delimeter )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads a string until it encounters a newline or the string specified by delimeter.
//o------------------------------------------------------------------------------------------------o
static bool CFile_ReadUntil(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "ReadUntil: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	if (!mFile || !mFile->mWrap || feof(mFile->mWrap))
	{
		ScriptError(cx, "ReadUntil: Error reading file, is not opened or has reached EOF");
		return false;
	}
	auto until = convertToString(cx, args.get(0).toString());
	char line[512];
	SI32 c;

	if (until[0] == '\\' && until.length() > 1)
	{
		switch (until[1])
		{
		case '\\': until[0] = '\\'; break;
		case '\'': until[0] = '\''; break;
		case '\"': until[0] = '\"'; break;
		case 'n':  until[0] = '\n'; break;
		default: ScriptError(cx, "ReadUntil: Unsupported character escape sequence %s", until);	break;
		}
	}

	for (c = 0; c < 512 && !feof(mFile->mWrap); ++c)
	{
		line[c] = fgetc(mFile->mWrap);
		if (line[c] == until[0] || line[c] == '\n')
			break;
	}
	line[c < 512 ? c : 511] = 0;

	args.rval().setString( convertFromString( cx, line ) );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Write()
//|	Prototype	-	void Write( string toWrite )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes a string out to the file
//o------------------------------------------------------------------------------------------------o
static bool CFile_Write(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Write: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	if (!mFile || !mFile->mWrap || feof(mFile->mWrap))
	{
		ScriptError(cx, "Write: Error writing to file, file was not opened successfully!");
		return false;
	}
	else if (ftell(mFile->mWrap) > (10 * 1024 * 1024))
	{
		ScriptError(cx, "Write: Error writing to file.  File may not exceed 10mb.");
		return false;
	}

	auto str = convertToString(cx, args.get(0).toString());
	if (str.empty())
	{
		fprintf(mFile->mWrap, "%s", str.c_str());
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_EOF()
//|	Prototype	-	bool eof()
//|	Date		-	3/05/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns if we have read to the end of a file
//o------------------------------------------------------------------------------------------------o
static bool CFile_EOF(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "EOF: Invalid number of arguments (takes 0)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	if (!mFile || !mFile->mWrap)
		return false;

	args.rval().setBoolean((feof(mFile->mWrap) != 0));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Length()
//|	Prototype	-	int Length()
//|	Date		-	3/05/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of the file
//o------------------------------------------------------------------------------------------------o
static bool CFile_Length(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Length: Invalid number of arguments (takes 0)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	if (!mFile || !mFile->mWrap)
	{
		args.rval().setInt32(-1);
		return true;
	}

	long fpos = ftell(mFile->mWrap);
	fseek(mFile->mWrap, 0, SEEK_END);
	args.rval().setInt32(ftell(mFile->mWrap));

	if (fpos > -1)
	{
		int newFPos = fseek(mFile->mWrap, fpos, SEEK_SET); // We don't care about return value, so suppress compiler warning
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Pos()
//|	Prototype	-	int Post()
//|	Date		-	3/05/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns or sets the position we are at in the file
//o------------------------------------------------------------------------------------------------o
static bool CFile_Pos(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0 && argc != 1)
	{
		ScriptError(cx, "Pos: Invalid number of arguments (takes 0 or 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	UOXFileWrapper_st* mFile = JS::GetMaybePtrFromReservedSlot<UOXFileWrapper_st>(obj, 0);

	if (!mFile || !mFile->mWrap)
		return false;

	if (argc == 1)
	{
		int newFPos = fseek(mFile->mWrap, args.get(0).toInt32(), SEEK_SET);
	}

	args.rval().setInt32(ftell(mFile->mWrap));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_FirstItem()
//|	Prototype	-	bool FirstItem()
//|	Date		-	02 Aug 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns first object in the object's (container's) list
//o------------------------------------------------------------------------------------------------o
static bool CBase_FirstItem(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "FirstItem: Invalid count of arguments :%d, needs :0", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "FirstItem: Invalid object assigned.");
		return false;
	}
	CItem* firstItem = nullptr;
	if (myObj->GetObjType() == OT_CHAR)
	{
		firstItem = (static_cast<CChar*>(myObj))->FirstItem();
	}
	else if (myObj->GetObjType() == OT_ITEM)
	{
		firstItem = (static_cast<CItem*>(myObj))->GetContainsList()->First();
	}
	else if (myObj->GetObjType() == OT_MULTI)
	{
		firstItem = (static_cast<CMultiObj*>(myObj))->GetItemsInMultiList()->First();
	}
	else
	{
		ScriptError(cx, "FirstItem: Unknown object type assigned.");
		return false;
	}

	if (ValidateObject(firstItem))
	{
		JSObject* myObj = JSEngine->AcquireObject(IUE_ITEM, firstItem, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
		args.rval().setObjectOrNull(myObj);
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_NextItem()
//|	Prototype	-	bool NextItem()
//|	Date		-	02 Aug 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns next object in the object's (container's) list
//o------------------------------------------------------------------------------------------------o
static bool CBase_NextItem(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "NextItem: Invalid count of arguments :%d, needs :0", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "NextItem: Invalid object assigned.");
		return false;
	}
	CItem* nextItem = nullptr;
	if (myObj->GetObjType() == OT_CHAR)
	{
		nextItem = (static_cast<CChar*>(myObj))->NextItem();
	}
	else if (myObj->GetObjType() == OT_ITEM)
	{
		nextItem = (static_cast<CItem*>(myObj))->GetContainsList()->Next();
	}
	else if (myObj->GetObjType() == OT_MULTI)
	{
		nextItem = (static_cast<CMultiObj*>(myObj))->GetItemsInMultiList()->Next();
	}
	else
	{
		ScriptError(cx, "NextItem: Unknown object type assigned.");
		return false;
	}

	if (ValidateObject(nextItem))
	{
		JSObject* myObj = JSEngine->AcquireObject(IUE_ITEM, nextItem, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
		args.rval().setObjectOrNull(myObj);
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_FinishedItems()
//|	Prototype	-	bool FinishedItems()
//|	Date		-	02 Aug 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if finished all items in object's list
//o------------------------------------------------------------------------------------------------o
static bool CBase_FinishedItems(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "FinishedItems: Invalid count of arguments :%d, needs :0", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "NextItem: Invalid object assigned.");
		return false;
	}
	if (myObj->GetObjType() == OT_CHAR)
	{
		args.rval().setBoolean((static_cast<CChar*>(myObj))->FinishedItems());
	}
	else if (myObj->GetObjType() == OT_ITEM)
	{
		args.rval().setBoolean((static_cast<CItem*>(myObj))->GetContainsList()->Finished());
	}
	else if (myObj->GetObjType() == OT_MULTI)
	{
		args.rval().setBoolean((static_cast<CMultiObj*>(myObj))->GetItemsInMultiList()->Finished());
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_WalkTo()
//|	Prototype	-	void WalkTo( object, maxsteps )
//|					void WalkTo( x, y, maxsteps )
//|	Date		-	06 Sep 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begins pathfinding for a character, making them walk to target location,
//|					halting if max amount of steps is reached before reaching the location
//o------------------------------------------------------------------------------------------------o
static bool CChar_WalkTo(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2 && argc != 3)
	{
		ScriptError(cx, "WalkTo: Invalid number of arguments (takes 2 or 3)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* cMove = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(cMove))
	{
		ScriptError(cx, "WalkTo: Invalid source character");
		return false;
	}
	SI16 gx = 0;
	SI16 gy = 0;
	UI08 maxSteps = 0;
	switch (argc)
	{
	case 2:
		if (args.get(0).isObject())
		{	// we can work with this, it should be either a character or item, hopefully
			JS::HandleValue jsToGoTo = args.get(0);
			if (jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar")
			{
				CBaseObject* toGoTo = static_cast<CBaseObject*>(jsToGoTo.toObject());
				if (!ValidateObject(toGoTo))
				{
					ScriptError(cx, "No object associated with this object");
					return false;
				}
				gx = toGoTo->GetX();
				gy = toGoTo->GetY();
			}
			else if (jsToGoTo.ClassName() == "UOXSocket")
			{
				CSocket* mySock = static_cast<CSocket*>(jsToGoTo.toObject());
				CChar* mySockChar = mySock->CurrcharObj();
				gx = mySockChar->GetX();
				gy = mySockChar->GetY();
			}
			else
			{
				ScriptError(cx, "Invalid class of object");
				return false;
			}
			maxSteps = static_cast<UI08>(args.get(1).toInt32());
			break;
		}
		return false;
		// 2 Parameters, x + y
	case 3:
		gx = static_cast<SI16>(args.get(0).toInt32());
		gy = static_cast<SI16>(args.get(1).toInt32());
		maxSteps = static_cast<UI08>(args.get(2).toInt32());
		break;
	default:
		ScriptError(cx, "Invalid number of arguments passed to WalkTo, needs either 2 or 3");
		return false;
	}

	if (maxSteps == 0)
	{
		ScriptError(cx, "WalkTo: Invalid number of maxSteps provided. Must be higher than 0!");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	cMove->FlushPath();
#if defined( UOX_DEBUG_MODE )
	Console.Print(oldstrutil::format("WalkTo: Moving character 0x%X to (%i,%i) with a maximum of %i steps\n", cMove->GetSerial(), gx, gy, maxSteps));
#endif
	if (cMove->GetNpcWander() != WT_PATHFIND)
	{
		// Only update oldNpcWander if NPC is not already pathfinding
		cMove->SetOldNpcWander(cMove->GetNpcWander());
	}
	cMove->SetNpcWander(WT_PATHFIND);
	if (cwmWorldState->ServerData()->AdvancedPathfinding())
	{
		Movement->AdvancedPathfinding(cMove, gx, gy, false, maxSteps);
	}
	else
	{
		Movement->PathFind(cMove, gx, gy, false, maxSteps);
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char JS Method .WalkTo()", origScriptID);
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_RunTo()
//|	Prototype	-	void RunTo( object, maxsteps )
//|					void RunTo( x, y, maxsteps )
//|	Date		-	06 Sep 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begins pathfinding for a character, making them run to target location,
//|					halting if max amount of steps is reached before reaching the location
//o------------------------------------------------------------------------------------------------o
static bool CChar_RunTo(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2 && argc != 3)
	{
		ScriptError(cx, "RunTo: Invalid number of arguments (takes 2 or 3)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* cMove = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(cMove))
	{
		ScriptError(cx, "RunTo: Invalid source character");
		return false;
	}
	UI16 gx = 0;
	UI16 gy = 0;
	UI08 maxSteps = 0;
	switch (argc)
	{
	case 2:
		if (args.get(0).isObject())
		{	// we can work with this, it should be either a character or item, hopefully
			JS::HandleValue jsToGoTo = args.get(0);
			if (jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar")
			{
				CBaseObject* toGoTo = static_cast<CBaseObject*>(jsToGoTo.toObject());
				if (!ValidateObject(toGoTo))
				{
					ScriptError(cx, "No object associated with this object");
					return false;
				}
				gx = toGoTo->GetX();
				gy = toGoTo->GetY();
			}
			else if (jsToGoTo.ClassName() == "UOXSocket")
			{
				CSocket* mySock = static_cast<CSocket*>(jsToGoTo.toObject());
				CChar* mySockChar = mySock->CurrcharObj();
				gx = mySockChar->GetX();
				gy = mySockChar->GetY();
			}
			else
			{
				ScriptError(cx, "Invalid class of object");
				return false;
			}
			maxSteps = static_cast<UI08>(args.get(1).toInt32());
			break;
		}
		return false;

		// 2 Parameters, x + y
	case 3:
		gx = static_cast<SI16>(args.get(0).toInt32());
		gy = static_cast<SI16>(args.get(1).toInt32());
		maxSteps = static_cast<UI08>(args.get(2).toInt32());
		break;
	default:
		ScriptError(cx, "Invalid number of arguments passed to RunTo, needs either 2 or 3");
		return false;
	}

	if (maxSteps == 0)
	{
		ScriptError(cx, "RunTo: Invalid number of maxSteps provided. Must be higher than 0!");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	cMove->FlushPath();
#if defined( UOX_DEBUG_MODE )
	Console.Print(oldstrutil::format("RunTo: Moving character %i to (%i,%i) with a maximum of %i steps", cMove->GetSerial(), gx, gy, maxSteps));
#endif
	if (cMove->GetNpcWander() != WT_PATHFIND)
	{
		// Only update oldNpcWander if NPC is not already pathfinding
		cMove->SetOldNpcWander(cMove->GetNpcWander());
	}
	cMove->SetNpcWander(WT_PATHFIND);

	if (cwmWorldState->ServerData()->AdvancedPathfinding())
	{
		Movement->AdvancedPathfinding(cMove, gx, gy, true);
	}
	else
	{
		Movement->PathFind(cMove, gx, gy, true, maxSteps);
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Char JS Method .RunTo()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_GetTimer()
//|	Prototype	-	int GetTimer( timerId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the specified timer value
//o------------------------------------------------------------------------------------------------o
static bool CMisc_GetTimer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "GetTimer: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue encaps = args.get(0);
	std::string className = JS::GetClass(obj.get())->name;
	if (className == "UOXChar")
	{
		CChar* cMove = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(cMove))
		{
			ScriptError(cx, "GetTimer: Invalid source character");
			return false;
		}

		JS_NewNumberValue(cx, cMove->GetTimer(static_cast<cC_TID>(encaps.toInt())), rval);
	}
	else if (className == "UOXSocket")
	{
		CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
		if (mSock == nullptr)
		{
			ScriptError(cx, "GetTimer: Invalid source socket");
			return false;
		}

		JS_NewNumberValue(cx, mSock->GetTimer(static_cast<cS_TID>(encaps.toInt())), rval);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SetTimer()
//|	Prototype	-	bool SetTimer( timerId, numMilliSeconds )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the specified timer with the amount of miliseconds until it expires
//o------------------------------------------------------------------------------------------------o
static bool CMisc_SetTimer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SetTimer: Invalid number of arguments (takes 2)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue encaps = args.get(0);
	JS::HandleValue encaps2 = args.get(1);
	std::string className = JS::GetClass(obj.get())->name;

	R32 timerVal = encaps2.toDouble();
	if (timerVal != 0)
	{
		timerVal = BuildTimeValue(timerVal / 1000.0f);
	}
	if (className == "UOXChar")
	{
		CChar* cMove = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(cMove))
		{
			ScriptError(cx, "SetTimer: Invalid source character");
			return false;
		}

		cMove->SetTimer(static_cast<cC_TID>(encaps.toInt32()), static_cast<TIMERVAL>(timerVal));
	}
	else if (className == "UOXSocket")
	{
		CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
		if (mSock == nullptr)
		{
			ScriptError(cx, "SetTimer: Invalid source socket");
			return false;
		}

		mSock->SetTimer(static_cast<cS_TID>(encaps.toInt32()), static_cast<TIMERVAL>(timerVal));
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_DistanceTo()
//|	Prototype	-	unsigned short DistanceTo( object )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the distance to the object
//o------------------------------------------------------------------------------------------------o
static bool CBase_DistanceTo(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "DistanceTo: Invalid number of arguments (takes 1, game object)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JSObject* jsObj = args.get(0).toObjectOrNull();
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject >(jsObj, 0);

	CBaseObject* thisObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);

	if (!ValidateObject(thisObj) || !ValidateObject(myObj))
	{
		ScriptError(cx, "DistanceTo: Invalid character");
		return false;
	}

	args.rval().setInt32(GetDist(thisObj, myObj));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_Glow()
//|	Prototype	-	void Glow( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Attaches a light-emitting object to the object
//o------------------------------------------------------------------------------------------------o
static bool CItem_Glow(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JSObject* mSock = args.get(0).toObjectOrNull();
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket >(mSock, 0);

	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);

	if (!ValidateObject(mItem))
	{
		ScriptError(cx, "Glow: Invalid object");
		mySock->SysMessage(1095); // No item found there, only items can be made to glow.
		return false;
	}

	CChar* mChar = mySock->CurrcharObj();
	if (!ValidateObject(mChar))
		return false;

	if (mItem->GetGlow() != INVALIDSERIAL)
	{
		mySock->SysMessage(1097); // That object already glows!
		return false;
	}
	if (mItem->GetCont() == nullptr && FindItemOwner(mItem) != mChar)
	{
		mySock->SysMessage(1096); // You can't create glowing items in other people's packs or hands!
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	mItem->SetGlowColour(mItem->GetColour());

	CItem* glowItem = Items->CreateItem(mySock, mChar, 0x1647, 1, 0, OT_ITEM); // spawn light emitting object
	if (glowItem == nullptr)
		return false;

	glowItem->SetDecayable(mItem->IsDecayable());
	glowItem->SetDecayTime(mItem->GetDecayTime());
	glowItem->SetName("glower");
	glowItem->SetMovable(2);

	mItem->SetGlow(glowItem->GetSerial());
	Items->GlowItem(mItem);

	mChar->Update(mySock);
	mySock->SysMessage(1098); // Item is now glowing.

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Item JS Method .Glow()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_UnGlow()
//|	Prototype	-	void UnGlow( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes light-emitting object from the object
//o------------------------------------------------------------------------------------------------o
static bool CItem_UnGlow(JSContext* cx, unsigned argc, JS::Value* vp)
{
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JSObject* mSock = args.get(0).toObjectOrNull();
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket >(mSock, 0);

	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);

	if (!ValidateObject(mItem))
	{
		ScriptError(cx, "UnGlow: Invalid item");
		mySock->SysMessage(1099); // No item found, only items can be made to unglow.
		return false;
	}

	CChar* mChar = mySock->CurrcharObj();
	if (!ValidateObject(mChar))
		return false;

	if (mItem->GetCont() == nullptr && FindItemOwner(mItem) != mChar)
	{
		mySock->SysMessage(1100); // You can't unglow items in other people's packs or hands!
		return false;
	}

	CItem* glowItem = CalcItemObjFromSer(mItem->GetGlow());
	if (mItem->GetGlow() == INVALIDSERIAL || !ValidateObject(glowItem))
	{
		mySock->SysMessage(1101); // That object doesn't glow!
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	mItem->SetColour(mItem->GetGlowColour());

	glowItem->Delete();
	mItem->SetGlow(INVALIDSERIAL);

	mChar->Update(mySock);
	mySock->SysMessage(1102); // Item is no longer glowing.

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Item JS Method .UnGlow()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Gate()
//|	Prototype	-	void Gate( item )
//|					void Gate( x, y, z, world )
//|					void Gate( x, y, z, world, instanceID )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a gate to the location marked on an item, or to a specified set of coords
//o------------------------------------------------------------------------------------------------o
static bool CChar_Gate(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 4 && argc != 5)
	{
		ScriptError(cx, "Gate: Invalid number of arguments (takes 1: item/place; 4: x y z worldNumber; or 5: x y z worldNumber instanceID)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "Gate: Invalid source character");
		return false;
	}

	SI16 destX = -1, destY = -1;
	SI08 destZ = -1;
	UI08 destWorld = 0;
	UI16 destInstanceId = 0;

	if (argc == 1)
	{
		if (args.get(0).isObject())
		{
			JSObject* jsObj = args.get(0).toObjectOrNull();
			CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem >(jsObj, 0);
			if (!ValidateObject(mItem))
			{
				ScriptError(cx, "Gate: Invalid item passed");
				return false;
			}

			destX = mItem->GetTempVar(CITV_MOREX);
			destY = mItem->GetTempVar(CITV_MOREY);
			destZ = mItem->GetTempVar(CITV_MOREZ);
			destWorld = mItem->GetTempVar(CITV_MORE);
			destInstanceId = mItem->GetTempVar(CITV_MORE0);
		}
		else
		{
			UI16 placeNum = args.get(0).toInt32();
			if (cwmWorldState->goPlaces.find(placeNum) != cwmWorldState->goPlaces.end())
			{
				GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
				destX = toGoTo.x;
				destY = toGoTo.y;
				destZ = toGoTo.z;
				destWorld = toGoTo.worldNum;
			}
		}
	}
	else
	{
		destX = args.get(0).toInt32();
		destY = args.get(1).toInt32();
		destZ = args.get(2).toInt32();
		destWorld = args.get(3).toInt32();
		if (argc == 5)
		{
			destInstanceId = args.get(4).toInt32();
		}
	}

	if (!Map->MapExists(destWorld))
	{
		destWorld = mChar->WorldNumber();
	}

	SpawnGate(mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber(), destX, destY, destZ, destWorld, destInstanceId);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Recall()
//|	Prototype	-	void Recall( item )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Character recalls to the location marked on an item
//o------------------------------------------------------------------------------------------------o
static bool CChar_Recall(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Recall: Invalid number of arguments (takes 1, item)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "Recall: Invalid source character");
		return false;
	}

	JSObject* jsObj = args.get(0).toObjectOrNull();
	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem >(jsObj, 0);
	if (!ValidateObject(mItem))
	{
		ScriptError(cx, "Recall: Invalid item passed");
		return false;
	}

	SI16 destX = mItem->GetTempVar(CITV_MOREX), destY = mItem->GetTempVar(CITV_MOREY);
	SI08 destZ = mItem->GetTempVar(CITV_MOREZ);
	UI08 destWorld = mItem->GetTempVar(CITV_MORE);
	UI16 destInstanceId = mItem->GetTempVar(CITV_MORE0);

	if (!Map->MapExists(destWorld))
	{
		destWorld = mChar->WorldNumber();
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	if (mChar->WorldNumber() != destWorld && mChar->GetSocket() != nullptr)
	{
		mChar->SetLocation(destX, destY, destZ, destWorld, destInstanceId);
		SendMapChange(destWorld, mChar->GetSocket());
	}
	else
	{
		mChar->SetLocation(destX, destY, destZ, destWorld, destInstanceId);
	}

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .Recall()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Mark()
//|	Prototype	-	void Mark( item )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Marks item with character's current location
//o------------------------------------------------------------------------------------------------o
static bool CChar_Mark(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Mark: Invalid number of arguments (takes 1, character)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "Mark: Invalid source character");
		return false;
	}

	JSObject* jsObj = args.get(0).toObjectOrNull();
	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem >(jsObj, 0);
	if (!ValidateObject(mItem))
	{
		ScriptError(cx, "Mark: Invalid item passed");
		return false;
	}

	mItem->SetTempVar(CITV_MOREX, mChar->GetX());
	mItem->SetTempVar(CITV_MOREY, mChar->GetY());
	mItem->SetTempVar(CITV_MOREZ, mChar->GetZ());
	mItem->SetTempVar(CITV_MORE, mChar->WorldNumber());
	mItem->SetTempVar(CITV_MORE, mChar->GetInstanceId());


	if (mChar->GetRegion()->GetName()[0] != 0)
	{
		mItem->SetName(oldstrutil::format(Dictionary->GetEntry(684), mChar->GetRegion()->GetName().c_str()));
	}
	else
	{
		mItem->SetName(Dictionary->GetEntry(685));
	}

	return true;
}

void SetRandomName(CBaseObject* s, const std::string& namelist);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_SetRandomName()
//|	Prototype	-	bool SetRandomName( "namelist" )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies a random name from specified namelist to character or item
//o------------------------------------------------------------------------------------------------o
static bool CBase_SetRandomName(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "SetRandomName: Invalid number of arguments (takes 1, namelist string)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* mObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	std::string namelist = convertToString(cx, args.get(0).toString());

	if (!namelist.empty())
	{
		SetRandomName(mObj, namelist);
		args.rval().setBoolean(true);
		return true;
	}

	args.rval().setBoolean(false);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SetSkillByName()
//|	Prototype	-	bool SetSkillByName( "skillName", value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the skill specified by name to the value specified (name must be the same
//|					as in skills.dfn, "ALLSKILLS" is also applicable.
//o------------------------------------------------------------------------------------------------o
static bool CChar_SetSkillByName(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SetSkillByName: Invalid number of arguments (takes 2, string, value)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	std::string skillName = convertToString(cx, args.get(0).toString());
	UI16 value = args.get(1).toInt32();
	CSocket* mSock = nullptr;
	if (!mChar->IsNpc())
	{
		mSock = mChar->GetSocket();
	}
	for (UI08 i = 0; i < ALLSKILLS; ++i)
	{
		if (skillName == cwmWorldState->skill[i].name)
		{
			mChar->SetBaseSkill(value, i);
			Skills->UpdateSkillLevel(mChar, i);

			if (mSock != nullptr)
			{
				mSock->UpdateSkill(i);
			}
			args.rval().setBoolean(true);
			return true;
		}
	}
	args.rval().setBoolean(false);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Kill()
//|	Prototype	-	void Kill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kills the character
//o------------------------------------------------------------------------------------------------o
static bool CChar_Kill(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Kill: Invalid number of arguments (takes 0)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "Kill: Invalid character passed");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for (auto i : scriptTriggers)
	{
		cScript* toExecute = JSMapping->GetScript(i);
		if (toExecute != nullptr)
		{
			SI08 retStatus = toExecute->OnDeathBlow(mChar, nullptr);

			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if (retStatus == 0)
				return true;
		}
	}

	HandleDeath(mChar, nullptr);

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .Kill()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Resurrect()
//|	Prototype	-	void Resurrect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resurrects the character
//o------------------------------------------------------------------------------------------------o
static bool CChar_Resurrect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Resurrect: Invalid number of arguments (takes 0)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "Resurrect: Invalid character passed");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	NpcResurrectTarget(mChar);

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .Resurrect()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_Dupe()
//|	Prototype	-	void Dupe( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a duplicate of the item in character's pack
//o------------------------------------------------------------------------------------------------o
static bool CItem_Dupe(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Dupe: Invalid number of arguments (takes 1 - socket/null)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	JSObject* jsObj = args.get(0).toObjectOrNull();

	CSocket* mSock = nullptr;
	bool dupeInPack = true;

	if (jsObj == nullptr)
	{
		dupeInPack = false;
	}
	else
	{
		mSock = JS::GetMaybePtrFromReservedSlot<CSocket >(jsObj, 0);
	}

	if (!ValidateObject(mItem) || (mSock == nullptr && dupeInPack))
	{
		ScriptError(cx, "Dupe: Bad parameters passed. Either item or socket is invalid!");
		return false;
	}

	JSObject* dupeItem = nullptr;
	if (dupeInPack && mSock != nullptr)
	{
		dupeItem = JSEngine->AcquireObject(IUE_ITEM, Items->DupeItem(mSock, mItem, mItem->GetAmount()), JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
	}
	else
	{
		CItem* dupeItemTemp = mItem->Dupe();
		dupeItem = JSEngine->AcquireObject(IUE_ITEM, dupeItemTemp, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
	}

	args.rval().setObjectOrNull( dupeItem );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Dupe()
//|	Prototype	-	void Dupe()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dupes specified character
//o------------------------------------------------------------------------------------------------o
static bool CChar_Dupe(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Dupe: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	// Original character object
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "Dupe: Invalid character object passed?");
		return false;
	}

	// Duped character object
	CChar* dupeCharTemp = mChar->Dupe();
	if (!ValidateObject(dupeCharTemp))
	{
		ScriptError(cx, "Dupe: Unable to duplicate character due to unknown error!");
		return false;
	}

	// JS Object for duped character
	JSObject* dupeChar = nullptr;
	dupeChar = JSEngine->AcquireObject(IUE_CHAR, dupeCharTemp, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));

	args.rval().setObjectOrNull( dupeChar );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Jail()
//|	Prototype	-	void Jail()
//|					void Jail( numSecsToJail )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Jails character, either for ~27 hours or for specified amount of time in seconds
//o------------------------------------------------------------------------------------------------o
static bool CChar_Jail(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "Jail: Invalid number of arguments (takes 0 or 1, seconds to Jail)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "Jail: Invalid character");
		return false;
	}

	SI32 numSecsToJail = 86400;
	if (argc == 1)
	{
		numSecsToJail = static_cast<SI32>(args.get(0).toInt32());
	}

	JailSys->JailPlayer(myChar, numSecsToJail);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Release()
//|	Prototype	-	void Release()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Releases character from jail
//o------------------------------------------------------------------------------------------------o
static bool CChar_Release(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "Release: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* myChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(myChar))
	{
		ScriptError(cx, "Jail: Invalid character");
		return false;
	}

	JailSys->ReleasePlayer(myChar);
	return true;
}

void CPage(CSocket* s, const std::string& reason);
void GMPage(CSocket* s, const std::string& reason);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Page()
//|	Prototype	-	void Page( pageType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers a page based on provided pageType
//o------------------------------------------------------------------------------------------------o
static bool CSocket_Page(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "Page: Invalid number of arguments (takes 1, pageType)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mySock == nullptr)
	{
		ScriptError(cx, "SysMessage: Invalid socket");
		return false;
	}

	UI08 pageType = static_cast<UI08>(args.get(0).toInt32());
	switch (pageType)
	{
	case 0: // Counselor page - Free Text
		CPage(mySock, "OTHER");
		break;
	case 1: // GM page - Free Text
		GMPage(mySock, "OTHER");
		break;
	case 2: // GM page, stuck
		GMPage(mySock, "Stuck");
		break;
	case 3: // GM page, item problem
		GMPage(mySock, "Item Problem");
		break;
	case 4: // GM page, harassment
		GMPage(mySock, "Harassment");
		break;
	case 5: // GM page, report exploit
		GMPage(mySock, "Exploit");
		break;
	default:
		break;
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Print()
//|	Prototype	-	void Print( string )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints a message in UOX3 console. Message should end with \n
//o------------------------------------------------------------------------------------------------o
static bool CConsole_Print(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Print: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	Console.Print(arg0.toString());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Log()
//|	Prototype	-	void Log( logMsg )
//|					void Log( logMsg, filename )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs a message either in default log file or in specified file
//o------------------------------------------------------------------------------------------------o
static bool CConsole_Log(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 2)
	{
		ScriptError(cx, "Log: Invalid number of arguments (takes 1 or 2)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	JSEncapsulate arg1;
	if (argc == 1)
	{
		Console.Log(arg0.toString());
	}
	else
	{
		arg1.SetContext(cx, &(argv[1]));
		Console.Log(arg0.toString(), arg1.toString());
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Error()
//|	Prototype	-	void Error( errorMsg )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs an error-message in default error log file
//o------------------------------------------------------------------------------------------------o
static bool CConsole_Error(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Error: Invalid number of arguments (takes 1)");
		return false;
	}
	JS::HandleValue arg0 = args.get(0);
	Console.Error(arg0.toString());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Warning()
//|	Prototype	-	void Warning( errorMsg )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs a warning-message in default warnings log file
//o------------------------------------------------------------------------------------------------o
static bool CConsole_Warning(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Warning: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	Console.Warning(arg0.toString());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintSectionBegin()
//|	Prototype	-	void PrintSectionBegin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints a section separator in the console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_PrintSectionBegin(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "PrintSectionBegin: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.PrintSectionBegin();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnYellow()
//|	Prototype	-	void TurnYellow()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-
//o------------------------------------------------------------------------------------------------o
static bool CConsole_TurnYellow(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "TurnYellow: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.TurnYellow();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnRed()
//|	Prototype	-	void TurnRed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates yellow text in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_TurnRed(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "TurnRed: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.TurnRed();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnGreen()
//|	Prototype	-	void TurnGreen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates green text in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_TurnGreen(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "TurnGreen: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.TurnGreen();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnBlue()
//|	Prototype	-	void TurnBlue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates blue text in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_TurnBlue(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "TurnBlue: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.TurnBlue();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnNormal()
//|	Prototype	-	void TurnNormal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates normal text in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_TurnNormal(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "TurnNormal: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.TurnNormal();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnBrightWhite()
//|	Prototype	-	void TurnBrightWhite()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates bright white text in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_TurnBrightWhite(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "TurnBrightWhite: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.TurnBrightWhite();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintDone()
//|	Prototype	-	void PrintDone()
//|					void PrintDone( bool )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints colored [done] message in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_PrintDone(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0 && argc != 1)
	{
		ScriptError(cx, "PrintDone: Invalid number of arguments (takes 0 or 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	bool normalDone = true;
	if (argc != 0)
	{
		JS::HandleValue encaps = args.get(0);
		normalDone = encaps.toBool();
	}
	if (normalDone)
	{
		Console.PrintDone();
	}
	else
	{
		messageLoop << MSG_PRINTDONE;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintFailed()
//|	Prototype	-	void PrintFailed()
//|					void PrintFailed( bool )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints colored [failed] message in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_PrintFailed(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0 && argc != 1)
	{
		ScriptError(cx, "PrintFailed: Invalid number of arguments (takes 0 or 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	bool normalFailed = true;
	if (argc != 0)
	{
		JS::HandleValue encaps = args.get(0);
		normalFailed = encaps.toBool();
	}
	if (normalFailed)
	{
		Console.PrintFailed();
	}
	else
	{
		messageLoop << MSG_PRINTFAILED;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintPassed()
//|	Prototype	-	void PrintPassed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints colored [skipped] message in console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_PrintPassed(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "PrintPassed: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.PrintPassed();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_ClearScreen()
//|	Prototype	-	void ClearScreen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the console
//o------------------------------------------------------------------------------------------------o
static bool CConsole_ClearScreen(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "ClearScreen: Invalid number of arguments (takes 0)");
		return false;
	}
	Console.ClearScreen();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintBasedOnVal()
//|	Prototype	-	void PrintBasedOnVal( bool )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints [done] or [failed] based on provided boolean
//o------------------------------------------------------------------------------------------------o
static bool CConsole_PrintBasedOnVal(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "PrintBasedOnVal: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	Console.PrintBasedOnVal(arg0.toBool());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_MoveTo()
//|	Prototype	-	void MoveTo( x, y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves console cursor position to specified x, y location
//|	Notes		-	If you want the same line,  y == -1
//o------------------------------------------------------------------------------------------------o
static bool CConsole_MoveTo(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "MoveTo: Invalid number of arguments (takes 2)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	JS::HandleValue arg1 = args.get(1);
	Console.MoveTo(arg0.toInt(), arg1.toInt());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintSpecial()
//|	Prototype	-	void PrintSpecial( textColour, string )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints a special tag in console similar to [done] and [failed], but with custom
//|					text and colour
//|	Notes		-	Available colours:
//|					0 - Normal
//|					1 - Blue
//|					2 - Red
//|					3 - Green
//|					4 - Yellow
//|					5 - Bright White
//o------------------------------------------------------------------------------------------------o
static bool CConsole_PrintSpecial(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "PrintSpecial: Invalid number of arguments (takes 2)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	JS::HandleValue arg1 = args.get(1);
	Console.PrintSpecial(arg0.toInt(), arg1.toString().c_str());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_BeginShutdown()
//|	Prototype	-	void BeginShutdown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Start the UOX3 shutdown sequence
//o------------------------------------------------------------------------------------------------o
static bool CConsole_BeginShutdown(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "BeginShutdown: Invalid number of arguments (takes 0)");
		return false;
	}
	messageLoop << MSG_SHUTDOWN;
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Reload()
//|	Prototype	-	void Reload( reloadType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads server data from console. Available reload types:
//|						0 Reload INI
//|						1 Reload Accounts
//|						2 Reload Regions
//|						3 Reload Spawn Regions
//|						4 Reload Spells
//|						5 Reload Commands
//|						6 Reload Definition Files
//|						7 Reload JS
//|						8 Reload HTML
//o------------------------------------------------------------------------------------------------o
static bool CConsole_Reload(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Reload: Invalid number of arguments (takes 1)");
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	JS::HandleValue arg0 = args.get(0);
	SI32 mArg = arg0.toInt();
	if (mArg < 0 || mArg > 8)
	{
		ScriptError(cx, "Reload: Section to reload must be between 0 and 8");
		return false;
	}
	messageLoop.NewMessage(MSG_RELOAD, oldstrutil::number(mArg).c_str());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpellMoveEffect()
//|	Prototype	-	void SpellMoveEffect( targetChar, mSpell )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the MOVEFX effect of the specified spell in SPELLS.DFN, going from the
//|					character to the target
//o------------------------------------------------------------------------------------------------o
static bool CChar_SpellMoveEffect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "SpellMoveEffect: Invalid number of arguments (takes 2)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSpellInfo* mySpell = JS::GetMaybePtrFromReservedSlot<CSpellInfo >(args.get(1).toObjectOrNull(), 0);
	if (mySpell == nullptr)
	{
		ScriptError(cx, "SpellMoveEffect: Invalid spell");
		return false;
	}

	CChar* source = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	CBaseObject* target = JS::GetMaybePtrFromReservedSlot<CBaseObject >(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(source) || !ValidateObject(target))
	{
		ScriptError(cx, "SpellMoveEffect: Invalid object passed");
		return false;
	}

	CMagicMove temp = mySpell->MoveEffect();

	if (temp.Effect() != INVALIDID)
	{
		Effects->PlayMovingAnimation(source, target, temp.Effect(), temp.Speed(), temp.Loop(), (temp.Explode() == 1));
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpellStaticEffect()
//|	Prototype	-	void SpellStaticEffect( mSpell )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the STATFX effect of the specified spell in SPELLS.DFN, on the character
//o------------------------------------------------------------------------------------------------o
static bool CChar_SpellStaticEffect(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "SpellStaticEffect: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* source = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(source))
	{
		ScriptError(cx, "SpellStaticEffect: Invalid character passed");
		return false;
	}

	CSpellInfo* mySpell = JS::GetMaybePtrFromReservedSlot<CSpellInfo >(args.get(0).toObjectOrNull(), 0);
	if (mySpell == nullptr)
	{
		ScriptError(cx, "SpellStaticEffect: Invalid spell");
		return false;
	}

	CMagicStat temp = mySpell->StaticEffect();

	if (temp.Effect() != INVALIDID)
	{
		Effects->PlayStaticAnimation(source, temp.Effect(), temp.Speed(), temp.Loop());
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_BreakConcentration()
//|	Prototype	-	void BreakConcentration()
//|					void BreakConcentration( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Break a caster's concentration. Provide socket as extra argument for player chars
//o------------------------------------------------------------------------------------------------o
static bool CChar_BreakConcentration(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "BreakConcentration: Invalid number of arguments (takes 0 or 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "BreakConcentration: Invalid character");
		return false;
	}

	CSocket* mSock = nullptr;
	if (argc == 1)
	{
		mSock = JS::GetMaybePtrFromReservedSlot<CSocket >(args.get(0).toObjectOrNull(), 0);
		if (mSock == nullptr)
		{
			ScriptError(cx, "BreakConcentration: Invalid socket");
			return false;
		}
	}

	mChar->BreakConcentration(mSock);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SendAddMenu()
//|	Prototype	-	bool SendAddMenu( menuNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send the Add-menu to the character
//o------------------------------------------------------------------------------------------------o
static bool CSocket_SendAddMenu(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "SendAddMenu: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "SendAddMenu: Invalid socket");
		return false;
	}

	UI16 menuNum = static_cast<UI16>(args.get(0).toInt32());

	BuildAddMenuGump(mSock, menuNum);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_LockDown()
//|	Prototype	-	void LockDown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks item down (sets movable value to 3)
//o------------------------------------------------------------------------------------------------o
static bool CItem_LockDown(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "LockDown: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	if (mItem == nullptr)
	{
		ScriptError(cx, "LockDown: Invalid item");
		return false;
	}

	mItem->LockDown();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_InitWanderArea()
//|	Prototype	-	void InitWanderArea()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes a WanderArea (10x10 box, or 10 radius circle) for the NPC.
//|					Will only work if they already have a wandermode set to box or circle.
//o------------------------------------------------------------------------------------------------o
static bool CChar_InitWanderArea(JSContext* cx, unsigned argc, JS::Value* vp)
{
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar) || !mChar->IsNpc())
	{
		ScriptError(cx, "InitWanderArea: Invalid character");
		return false;
	}
	InitializeWanderArea(mChar, 10, 10);
	return true;
}

auto NewCarveTarget(CSocket* s, CItem* i) -> bool;
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_Carve()
//|	Prototype	-	void Carve( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes the character belonging to socket carve up a corpse
//o------------------------------------------------------------------------------------------------o
static bool CItem_Carve(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "Carve: Invalid number of arguments (1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* toCarve = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	if (!ValidateObject(toCarve))
	{
		ScriptError(cx, "Carve: Invalid item");
		return false;
	}

	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket >(args.get(0).toObjectOrNull(), 0);
	if (mSock == nullptr)
	{
		ScriptError(cx, "Carve: Invalid socket");
		return false;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	args.rval().setBoolean(NewCarveTarget(mSock, toCarve));

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Item JS Method .Carve()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_GetTileName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the tile name of a specified tile (item)
//o------------------------------------------------------------------------------------------------o
static bool CItem_GetTileName(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "GetTileName: needs 0 arguments!");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CItem* mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj, 0);
	if (!ValidateObject(mItem))
	{
		ScriptError(cx, "GetTileName: Invalid item");
		return false;
	}

	std::string itemName = "";
	GetTileName((*mItem), itemName);

	args.rval().setString( convertFromString( cx, itemName ) );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_GetMultiCorner()
//|	Prototype	-	void GetMultiCorner( cornerId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets coordinates for specified corner ("NW", "NE", "SW" or "SE") of multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_GetMultiCorner(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "GetMultiCorner: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(GetMultiCorner) Invalid object referenced - multi required");
		args.rval().setBoolean(false);
		return true;
	}

	UI08 cornerToFind = static_cast<UI08>(args.get(0).toInt32());
	SI16 x1 = 0;
	SI16 y1 = 0;
	SI16 x2 = 0;
	SI16 y2 = 0;

	Map->MultiArea(multiObject, x1, y1, x2, y2);
	switch (cornerToFind)
	{
	case 0: // NW
		args.rval().setString( convertFromString(cx, (std::to_string(x1) + "," + std::to_string(y1))));
		break;
	case 1: // NE
		args.rval().setString(convertFromString(cx, (std::to_string(x2) + "," + std::to_string(y1))));
		break;
	case 2: // SW
		args.rval().setString(convertFromString(cx, (std::to_string(x1) + "," + std::to_string(y2))));
		break;
	case 3: // SE
		args.rval().setString(convertFromString(cx, (std::to_string(x2) + "," + std::to_string(y2))));
		break;
	default:
		break;
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_SecureContainer()
//|	Prototype	-	void SecureContainer( itemToSecure )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Secures a container in a multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_SecureContainer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "SecureContainer: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(SecureContainer) Invalid object referenced - multi required");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(SecureContainer) Invalid Object passed");
		return false;
	}

	CItem* itemToSecure = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToSecure))
	{
		ScriptError(cx, "(SecureContainer) Invalid Object passed");
		return false;
	}

	multiObject->SecureContainer(itemToSecure);
	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_UnsecureContainer()
//|	Prototype	-	void UnsecureContainer( itemToUnsecure )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unsecures a secured container in a multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_UnsecureContainer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "UnsecureContainer: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(UnsecureContainer) Invalid object referenced - multi required");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(UnsecureContainer) Invalid Object passed");
		return false;
	}

	CItem* itemToUnsecure = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToUnsecure))
	{
		ScriptError(cx, "(UnsecureContainer) Invalid Object passed");
		return false;
	}

	multiObject->UnsecureContainer(itemToUnsecure);
	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsSecureContainer()
//|	Prototype	-	void IsSecureContainer( itemToCheck )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified item is a secure container in the multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_IsSecureContainer(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "IsSecureContainer: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(IsSecureContainer) Invalid object referenced - multi required");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(IsSecureContainer) Invalid Object passed");
		return false;
	}

	CItem* itemToCheck = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToCheck))
	{
		ScriptError(cx, "(IsSecureContainer) Invalid Object passed");
		return false;
	}

	bool isSecureContainer = multiObject->IsSecureContainer(itemToCheck);
	args.rval().setBoolean(isSecureContainer);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_LockDownItem()
//|	Prototype	-	void LockDownItem( itemToLockDown )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item in a multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_LockDownItem(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "LockDownItem: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(LockDownItem) Invalid multi object referenced");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(LockDownItem) Invalid item object passed");
		return false;
	}

	CItem* itemToLockDown = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToLockDown))
	{
		ScriptError(cx, "(LockDownItem) Invalid item object passed");
		return false;
	}

	multiObject->LockDownItem(itemToLockDown);
	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ReleaseItem()
//|	Prototype	-	void ReleaseItem( itemToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Releases a locked down item in a multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_ReleaseItem(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "ReleaseItem: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(ReleaseItem) Invalid multi object referenced");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(ReleaseItem) Invalid item object passed");
		return false;
	}

	CItem* itemToRemove = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToRemove))
	{
		ScriptError(cx, "(ReleaseItem) Invalid item object passed");
		return false;
	}

	multiObject->ReleaseItem(itemToRemove);
	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddTrashCont()
//|	Prototype	-	void AddTrashCont( itemToLockDown )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item in a multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_AddTrashCont(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddTrashCont: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(AddTrashCont) Invalid multi object referenced");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(AddTrashCont) Invalid item object passed");
		return false;
	}

	CItem* itemToLockDown = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToLockDown))
	{
		ScriptError(cx, "(AddTrashCont) Invalid item object passed");
		return false;
	}

	multiObject->AddTrashContainer(itemToLockDown);
	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveTrashCont()
//|	Prototype	-	void RemoveTrashCont( itemToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Releases a locked down item in a multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_RemoveTrashCont(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "RemoveTrashCont: Invalid number of arguments (1 required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(RemoveTrashCont) Invalid multi object referenced");
		return false;
	}

	if (!args.get(0).isObject())
	{
		ScriptError(cx, "(RemoveTrashCont) Invalid item object passed");
		return false;
	}

	CItem* itemToRemove = JS::GetMaybePtrFromReservedSlot<CItem>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(itemToRemove))
	{
		ScriptError(cx, "(RemoveTrashCont) Invalid item object passed");
		return false;
	}

	multiObject->RemoveTrashContainer(itemToRemove);
	args.rval().setBoolean(true);
	return true;
}

void KillKeys(SERIAL targSerial, SERIAL charSerial = INVALIDSERIAL);
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_KillKeys()
//|	Prototype	-	void KillKeys()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kills all keys in the world associated with the particular multi
//o------------------------------------------------------------------------------------------------o
static bool CMulti_KillKeys(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0 && argc != 1)
	{
		ScriptError(cx, "KillKeys: Invalid number of arguments (0 or 1 (character) required)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	args.rval().setBoolean(false);
	CMultiObj* multiObject = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);

	if (!ValidateObject(multiObject) || !multiObject->CanBeObjType(OT_MULTI))
	{
		ScriptError(cx, "(KillKeys) Invalid multi object referenced");
		return false;
	}

	if (argc == 1)
	{
		JSObject* jsObj = args.get(0).toObjectOrNull();
		CChar* myObj = JS::GetMaybePtrFromReservedSlot<CChar >(jsObj, 0);

		if (!ValidateObject(myObj))
		{
			ScriptError(cx, "(KillKeys) Invalid character object provided");
			return false;
		}
		KillKeys(multiObject->GetSerial(), myObj->GetSerial());
	}
	else
	{
		KillKeys(multiObject->GetSerial());
	}

	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_FirstChar()
//|	Prototype	-	bool FirstChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns first char in the multi's list
//o------------------------------------------------------------------------------------------------o
static bool CMulti_FirstChar(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "FirstChar: Invalid count of arguments :%d, needs :0 or 1", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CMultiObj* myObj = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "FirstChar: Invalid object assigned - multi required.");
		return false;
	}

	std::string listType;
	if (argc == 1)
	{
		listType = convertToString(cx, args.get(0).toString());
	}
	else
	{
		listType = "visitor"; // default for existing scripts that provide no argument
	}

	CChar* firstChar = nullptr;
	if (listType == "visitor") // All chars inside the multi
	{
		firstChar = myObj->GetCharsInMultiList()->First();
	}
	else if (listType == "owner") // Owners
	{
		firstChar = myObj->GetOwnersOfMultiList(true)->First(); // true to clear list before it is regenerated
	}
	else if (listType == "friend") // Friends
	{
		firstChar = myObj->GetFriendsOfMultiList(true)->First(); // true to clear list before it is regenerated
	}
	else if (listType == "guest") // Guests
	{
		firstChar = myObj->GetGuestsOfMultiList(true)->First(); // true to clear list before it is regenerated
	}
	else if (listType == "banned") // Banned
	{
		firstChar = myObj->GetBannedFromMultiList(true)->First(); // true to clear list before it is regenerated
	}
	else
	{
		ScriptError(cx, "FirstChar: Unknown listType provided. Supported listTypes: visitor (default), owner, friend or banned");
		return false;
	}

	if (ValidateObject(firstChar))
	{
		JSObject* myObj = JSEngine->AcquireObject(IUE_CHAR, firstChar, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_NextChar()
//|	Prototype	-	bool NextChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns next char in the multi's list
//o------------------------------------------------------------------------------------------------o
static bool CMulti_NextChar(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "NextChar: Invalid count of arguments :%d, needs :0 or 1", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CMultiObj* myObj = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "NextChar: Invalid object assigned - multi required.");
		return false;
	}

	std::string listType;
	if (argc == 1)
	{
		listType = convertToString(cx, args.get(0).toString());
	}
	else
	{
		listType = "visitor"; // default for existing scripts that provide no argument
	}

	CChar* nextChar = nullptr;
	if (listType == "visitor") // All chars inside the multi
	{
		nextChar = myObj->GetCharsInMultiList()->Next();
	}
	else if (listType == "owner") // Owners
	{
		nextChar = myObj->GetOwnersOfMultiList()->Next();
	}
	else if (listType == "friend") // Friends
	{
		nextChar = myObj->GetFriendsOfMultiList()->Next();
	}
	else if (listType == "guest") // Guests
	{
		nextChar = myObj->GetGuestsOfMultiList()->Next();
	}
	else if (listType == "banned") // Banned
	{
		nextChar = myObj->GetBannedFromMultiList()->Next();
	}
	else
	{
		ScriptError(cx, "FinishedChars: Unknown listType provided. Supported listTypes: visitor (default), owner, friend or banned");
		return false;
	}

	if (ValidateObject(nextChar))
	{
		JSObject* myObj = JSEngine->AcquireObject(IUE_CHAR, nextChar, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_FinishedChars()
//|	Prototype	-	bool FinishedChars()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if finished all characters in multi's list
//o------------------------------------------------------------------------------------------------o
static bool CMulti_FinishedChars(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "FinishedChars: Invalid count of arguments :%d, needs :0 or 1", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CMultiObj* myObj = JS::GetMaybePtrFromReservedSlot<CMultiObj>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "FinishedChars: Invalid object assigned - multi required.");
		return false;
	}

	//char *listType;
	std::string listType;
	if (argc == 1)
	{
		listType = convertToString(cx, args.get(0).toString());
	}
	else
	{
		listType = "visitor"; // default for existing scripts that provide no argument
	}

	if (listType == "visitor") // All chars inside the multi
	{
		args.rval().setBoolean(myObj->GetCharsInMultiList()->Finished());
	}
	else if (listType == "owner") // Owners
	{
		args.rval().setBoolean(myObj->GetOwnersOfMultiList()->Finished());
	}
	else if (listType == "friend") // Friends
	{
		args.rval().setBoolean(myObj->GetFriendsOfMultiList()->Finished());
	}
	else if (listType == "guest") // Guests
	{
		args.rval().setBoolean(myObj->GetGuestsOfMultiList()->Finished());
	}
	else if (listType == "banned") // Banned
	{
		args.rval().setBoolean(myObj->GetBannedFromMultiList()->Finished());
	}
	else
	{
		ScriptError(cx, "FinishedChars: Unknown listType provided. Supported listTypes: visitor (default), owner, friend or banned");
		return false;
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_CanSee()
//|	Prototype	-	bool CanSee( object )
//|					bool CanSee( x, y, z )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks Line of Sight from character to target object or location
//o------------------------------------------------------------------------------------------------o
static bool CBase_CanSee(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 3)
	{
		ScriptError(cx, "CanSee: Invalid number of arguments (takes 1, a char/item or 3, an x/y/z)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CSocket* mSock = nullptr;
	CChar* mChar = nullptr;

	// Let's validate the person seeing (socket/char)

	if (className == "UOXSocket")
	{
		mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
		if (mSock == nullptr)
		{
			ScriptError(cx, "CanSee: Passed an invalid Socket");
			return false;
		}
		mChar = mSock->CurrcharObj();
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "CanSee: Socket to look from has an invalid Character attached");
			return false;
		}
	}
	else if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "CanSee: Passed an invalid Character");
			return false;
		}
		mSock = mChar->GetSocket();
	}

	// If we've reached this far, the socket/char is valid
	// Now let's validate the input!

	SI16 x = -1, y = -1;
	SI08 z = 0;
	SI08 zTop = 0;
	if (argc == 1)	// we've been passed an item, character, or socket
	{
		JS::HandleValue myClass = args.get(0);

		if (className == "UOXSocket")
		{
			CSocket* tSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj.get(), 0);
			if (tSock == nullptr)
			{
				ScriptError(cx, "CanSee: Passed an invalid Socket to look at");
				args.rval().setBoolean(false);
				return true;
			}
			CChar* tChar = tSock->CurrcharObj();
			if (!ValidateObject(tChar))
			{
				ScriptError(cx, "CanSee: Socket to look at has invalid Character attached");
				args.rval().setBoolean(false);
				return true;
			}
			if (tChar->WorldNumber() != mChar->WorldNumber() || tChar->GetInstanceId() != mChar->GetInstanceId())
			{
				args.rval().setBoolean(false);
				return true;
			}
			x = tChar->GetX();
			y = tChar->GetY();
			z = tChar->GetZ();
		}
		else if (className == "UOXChar" || className == "UOXItem")
		{
			CBaseObject* tObj = static_cast<CBaseObject*>(myClass.toObject());
			if (!ValidateObject(tObj))
			{
				ScriptError(cx, "CanSee: Object to look at is invalid");
				args.rval().setBoolean(false);
				return true;
			}
			if (tObj->WorldNumber() != mChar->WorldNumber() || tObj->GetInstanceId() != mChar->GetInstanceId())
			{
				args.rval().setBoolean(false);
				return true;
			}
			if (tObj->CanBeObjType(OT_ITEM))
			{
				if ((static_cast<CItem*>(tObj))->GetCont() != nullptr)
				{
					ScriptError(cx, "CanSee: Object to look at cannot be in a container");
					args.rval().setBoolean(false);
					return true;
				}

				// Include top of item
				zTop = Map->TileHeight(tObj->GetId());
			}
			else
			{
				// Include top of head of character. Also, assume all characters are equally tall
				zTop = 15;
			}
			x = tObj->GetX();
			y = tObj->GetY();
			z = tObj->GetZ();
		}
	}
	else	// we've been passed an x, y and z
	{
		x = static_cast<SI16>(args.get(0).toInt32());
		y = static_cast<SI16>(args.get(1).toInt32());
		z = static_cast<SI08>(args.get(2).toInt32());
	}

	if (ValidateObject(mChar))
	{
		args.rval().setBoolean(LineOfSight(mSock, mChar, x, y, z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false, zTop));
	}
	else
	{
		args.rval().setBoolean(false);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_DisplayDamage()
//|	Prototype	-	void DisplayDamage( pUser, damage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays specified damage value over character's head
//o------------------------------------------------------------------------------------------------o
static bool CSocket_DisplayDamage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "(CSocket_DisplayDamage) Invalid Number of Arguments %d, needs: 2", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mSock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	JS::HandleValue myClass = args.get(0);

	if (className != "UOXChar")	// It must be a character!
	{
		ScriptError(cx, "CSocket_DisplayDamage: Passed an invalid Character");
		return false;
	}

	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(CSocket_DisplayDamage): Passed an invalid Character");
		return true;
	}

	JS::HandleValue damage = args.get(1);

	CPDisplayDamage dispDamage((*mChar), static_cast<UI16>(damage.toInt()));
	mSock->Send(&dispDamage);

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ReactOnDamage()
//|	Prototype	-	void ReactOnDamage( damageType )
//|					void ReactOnDamage( damageType, attacker )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Lets character react to damage taken
//o------------------------------------------------------------------------------------------------o
static bool CChar_ReactOnDamage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 2)
	{
		ScriptError(cx, "(CChar_ReactOnDamage) Invalid Number of Arguments %d, needs: 1 (damageType) or 2 (damageType and attacker)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* attacker = nullptr;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(CChar_ReactOnDamage): Operating on an invalid Character");
		return true;
	}
	JS::HandleValue damage = args.get(0);

	if (argc >= 2)
	{
		JS::HandleValue attackerClass = args.get(1);
		if (attackerClass.ClassName() != "UOXChar") // It must be a character!
		{
			ScriptError(cx, "CChar_ReactOnDamage: Passed an invalid Character");
			return false;
		}

		if (attackerClass.isType(JSOT_VOID) || attackerClass.isType(JSOT_NULL))
		{
			attacker = nullptr;
		}
		else
		{
			attacker = static_cast<CChar*>(attackerClass.toObject());
			if (!ValidateObject(attacker))
			{
				ScriptError(cx, "(CChar_ReactOnDamage): Passed an invalid Character");
				return true;
			}
		}
	}
	mChar->ReactOnDamage(static_cast<WeatherType>(damage.toInt()), attacker);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Damage()
//|	Prototype	-	void Damage( amount )
//|					void Damage( amount, damageType )
//|					void Damage( amount, damageType, attacker )
//|					void Damage( amount, damageType, attacker, doRepsys )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deals damage of a specified damageType to a character, with optional parameters to include attacker and
//|					whether or not attacker should be flagged as a criminal
//o------------------------------------------------------------------------------------------------o
static bool CChar_Damage(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc < 1 || argc > 4)
	{
		ScriptError(cx, "(CChar_Damage) Invalid Number of Arguments %d, needs: 1 (amount), 2 (amount, damageType), 3 (amount, damageType and attacker) or 4 (amount, damageType, attacker and doRepsys)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* attacker = nullptr;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(CChar_Damage): Operating on an invalid Character");
		return true;
	}
	JS::HandleValue damage = args.get(0);

	WeatherType element = PHYSICAL;
	if (argc >= 2)
	{
		element = static_cast<WeatherType>(args.get(1).toInt32());
	}

	if (argc >= 3)
	{
		JS::HandleValue attackerClass = args.get(2);
		if (attackerClass.ClassName() != "UOXChar")	// It must be a character!
		{
			ScriptError(cx, "CChar_Damage: Passed an invalid Character");
			return false;
		}

		if (attackerClass.isType(JSOT_VOID) || attackerClass.isType(JSOT_NULL))
		{
			attacker = nullptr;
		}
		else
		{
			attacker = static_cast<CChar*>(attackerClass.toObject());
			if (!ValidateObject(attacker))
			{
				ScriptError(cx, "(CChar_Damage): Passed an invalid Character");
				return true;
			}
		}
	}
	bool doRepsys = false;
	if (argc >= 4)
	{
		doRepsys = (args.get(3).toBoolean());
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	bool retVal = mChar->Damage(damage.toInt(), element, attacker, doRepsys);

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .Damage()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_InitiateCombat()
//|	Prototype	-	bool InitiateCombat( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Attempts to initiate combat with target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_InitiateCombat(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(InitiateCombat) Invalid Number of Arguments %d, takes: 1 (targetChar))", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(InitiateCombat): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(InitiateCombat): Operating on an invalid Character");
		return true;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	args.rval().setBoolean(Combat->StartAttack(mChar, ourTarget));

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .InitiateCombat()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_InvalidateAttacker()
//|	Prototype	-	bool InvalidateAttacker()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets the attacker attack so that it cancels attack setup.
//o------------------------------------------------------------------------------------------------o
static bool CChar_InvalidateAttacker(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "(InvalidateAttacker) Invalid Number of Arguments %d, takes: 0)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(InvalidateAttacker): Operating on an invalid Character");
		return true;
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	Combat->InvalidateAttacker(mChar);

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .InvalidateAttacker()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddAggressorFlag()
//|	Prototype	-	void AddAggressorFlag( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds aggressor flag for character towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_AddAggressorFlag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(AddAggressorFlag) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(AddAggressorFlag): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(AddAggressorFlag): Operating on an invalid Character (arg 0)");
		return true;
	}

	mChar->AddAggressorFlag(ourTarget->GetSerial());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_RemoveAggressorFlag()
//|	Prototype	-	void RemoveAggressorFlag( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character's aggressor flag towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_RemoveAggressorFlag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(RemoveAggressorFlag) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(RemoveAggressorFlag): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(RemoveAggressorFlag): Operating on an invalid Character (arg 0)");
		return true;
	}

	mChar->RemoveAggressorFlag(ourTarget->GetSerial());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CheckAggressorFlag()
//|	Prototype	-	void CheckAggressorFlag( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has an aggressor flag towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_CheckAggressorFlag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(CheckAggressorFlag) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(CheckAggressorFlag): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(CheckAggressorFlag): Operating on an invalid Character (arg 0)");
		return true;
	}

	args.rval().setBoolean(mChar->CheckAggressorFlag(ourTarget->GetSerial()));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_UpdateAggressorFlagTimestamp()
//|	Prototype	-	void UpdateAggressorFlagTimestamp( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the expiry timestamp of character's aggressor flag towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_UpdateAggressorFlagTimestamp(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(UpdateAggressorFlagTimestamp) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(UpdateAggressorFlagTimestamp): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(UpdateAggressorFlagTimestamp): Operating on an invalid Character (arg 0)");
		return true;
	}

	mChar->UpdateAggressorFlagTimestamp(ourTarget->GetSerial());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ClearAggressorFlags()
//|	Prototype	-	void ClearAggressorFlags()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears all the character's aggressor flags towards other characters
//o------------------------------------------------------------------------------------------------o
static bool CChar_ClearAggressorFlags(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "(ClearAggressorFlags) Invalid Number of Arguments %d, takes: 0)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(ClearAggressorFlags): Operating on an invalid Character");
		return true;
	}

	mChar->ClearAggressorFlags();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_IsAggressor()
//|	Prototype	-	void IsAggressor( [bool]checkForPlayerOnly )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true/false depending on whether character has any active aggressor flags
//|					Optional parameter supported to check only flags towards players and ignore NPCs
//o------------------------------------------------------------------------------------------------o
static bool CChar_IsAggressor(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "(IsAggressor) Invalid Number of Arguments %d, takes: 0 or 1)", argc);
		return true;
	}

	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(IsAggressor): Operating on an invalid Character");
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	bool checkForPlayerOnly = (args.get(0).toBoolean());

	args.rval().setBoolean(mChar->IsAggressor(checkForPlayerOnly));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddPermaGreyFlag()
//|	Prototype	-	void AddPermaGreyFlag( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds aggressor flag for character towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_AddPermaGreyFlag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(AddPermaGreyFlag) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(AddPermaGreyFlag): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(AddPermaGreyFlag): Operating on an invalid Character (arg 0)");
		return true;
	}

	mChar->AddPermaGreyFlag(ourTarget->GetSerial());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_RemovePermaGreyFlag()
//|	Prototype	-	void RemovePermaGreyFlag( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character's permagrey flag towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_RemovePermaGreyFlag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(RemovePermaGreyFlag) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(RemovePermaGreyFlag): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(RemovePermaGreyFlag): Operating on an invalid Character (arg 0)");
		return true;
	}

	mChar->RemovePermaGreyFlag(ourTarget->GetSerial());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CheckPermaGreyFlag()
//|	Prototype	-	void CheckPermaGreyFlag( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character has an active permagrey flag towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_CheckPermaGreyFlag(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(CheckPermaGreyFlag) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(CheckPermaGreyFlag): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(CheckPermaGreyFlag): Operating on an invalid Character (arg 0)");
		return true;
	}

	args.rval().setBoolean(mChar->CheckPermaGreyFlag(ourTarget->GetSerial()));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_UpdatePermaGreyFlagTimestamp()
//|	Prototype	-	void UpdatePermaGreyFlagTimestamp( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the expiry timestamp of character's permagrey flag towards target character
//o------------------------------------------------------------------------------------------------o
static bool CChar_UpdatePermaGreyFlagTimestamp(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "(UpdatePermaGreyFlagTimestamp) Invalid Number of Arguments %d, takes: 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(UpdatePermaGreyFlagTimestamp): Operating on an invalid Character");
		return true;
	}

	CChar* ourTarget = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(ourTarget))
	{
		ScriptError(cx, "(UpdatePermaGreyFlagTimestamp): Operating on an invalid Character (arg 0)");
		return true;
	}

	mChar->UpdatePermaGreyFlagTimestamp(ourTarget->GetSerial());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ClearPermaGreyFlags()
//|	Prototype	-	void ClearPermaGreyFlags()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears all the character's aggressor flags towards other characters
//o------------------------------------------------------------------------------------------------o
static bool CChar_ClearPermaGreyFlags(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "(ClearPermaGreyFlags) Invalid Number of Arguments %d, takes: 0)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(ClearPermaGreyFlags): Operating on an invalid Character");
		return true;
	}

	mChar->ClearPermaGreyFlags();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_IsPermaGrey()
//|	Prototype	-	void IsPermaGrey( [bool]checkForPlayerOnly )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true/false depending on whether character has any active permagrey flags
//|					Optional parameter supported to check only flags towards players and ignore NPCs
//o------------------------------------------------------------------------------------------------o
static bool CChar_IsPermaGrey(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "(IsPermaGrey) Invalid Number of Arguments %d, takes: 0 or 1)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(IsPermaGrey): Operating on an invalid Character");
		return true;
	}

	bool checkForPlayerOnly = (args.get(0).toBoolean());

	args.rval().setBoolean(mChar->IsPermaGrey(checkForPlayerOnly));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Heal()
//|	Prototype	-	void Heal( amount );
//|					void Heal( amount, healer );
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Heals a character, with optional argument to provide character who healed
//o------------------------------------------------------------------------------------------------o
static bool CChar_Heal(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 2)
	{
		ScriptError(cx, "(CChar_Heal) Invalid Number of Arguments %d, needs: 1 (amount) or 2 (amount and healer)", argc);
		return true;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CChar* healer = nullptr;
	CChar* mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj, 0);
	if (!ValidateObject(mChar))
	{
		ScriptError(cx, "(CChar_Heal): Operating on an invalid Character");
		return true;
	}
	JS::HandleValue Heal = args.get(0);

	if (argc == 2)
	{
		JS::HandleValue healerClass = args.get(1);
		if (healerClass.ClassName() != "UOXChar")	// It must be a character!
		{
			ScriptError(cx, "CChar_Heal: Passed an invalid Character");
			return false;
		}

		if (healerClass.isType(JSOT_VOID) || healerClass.isType(JSOT_NULL))
		{
			healer = nullptr;
		}
		else
		{
			healer = static_cast<CChar*>(healerClass.toObject());
			if (!ValidateObject(healer))
			{
				ScriptError(cx, "(CChar_Heal): Passed an invalid Character");
				return true;
			}
		}
	}

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript(JS::CurrentGlobalOrNull(cx));
	auto origScriptID = JSMapping->GetScriptId(JS::CurrentGlobalOrNull(cx));

	mChar->Heal(static_cast<SI16>(Heal.toInt()), healer);

	// Active script-context might have been lost, so restore it...
	if (origScript != JSMapping->GetScript(JS::CurrentGlobalOrNull(cx)))
	{
		// ... by calling a dummy function in original script!
		restoreContext(origScript, "Character JS Method .Heal()", origScriptID);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_Resist()
//|	Prototype	-	int Resist( resistanceType );
//|					void Resist( resistanceType, amount );
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to get/set the resistance type and amount of the object
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Current resistanceTypes are:
//|						1 - Armor
//|						2 - Light
//|						3 - Water
//|						4 - Cold
//|						5 - Heat
//|						6 - Energy
//|						7 - Poison
//o------------------------------------------------------------------------------------------------o
static bool CBase_Resist(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1 && argc != 2)
	{
		ScriptError(cx, "Resist: Invalid number of arguments (takes 1, the resist type or 2, the resist type and value to set)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;
	CItem* mItem = nullptr;

	// Let's validate the char/item

	if (className == "UOXItem")
	{
		mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj.get(), 0);
		if (!ValidateObject(mItem))
		{
			ScriptError(cx, "Resist: Passed an invalid Item");
			return false;
		}
	}
	else if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "Resist: Passed an invalid Character");
			return false;
		}
	}

	JS::HandleValue resistType = args.get(0);

	if (argc == 1)
	{
		if (ValidateObject(mChar))
		{
			args.rval().setInt32(mChar->GetResist(static_cast<WeatherType>(resistType.toInt())));
		}
		else if (ValidateObject(mItem))
		{
			args.rval().setInt32(mItem->GetResist(static_cast<WeatherType>(resistType.toInt())));
		}
		else
		{
			args.rval().setBoolean(false);
		}
	}
	if (argc == 2)
	{
		args.rval().setBoolean(true);
		JS::HandleValue value = args.get(1);
		if (ValidateObject(mChar))
		{
			mChar->SetResist(static_cast<UI16>(value.toInt()), static_cast<WeatherType>(resistType.toInt()));
		}
		else if (ValidateObject(mItem))
		{
			mItem->SetResist(static_cast<UI16>(value.toInt()), static_cast<WeatherType>(resistType.toInt()));
		}
		else
		{
			args.rval().setBoolean(false);
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Defense()
//|	Prototype	-	int Defense( hitLoc, damageType, doArmorDamage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the defense value against damageType of the armor item at the location
//|					hitLoc and does armor damage if needed
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	hitLoc = the hit location
//|						0 the whole char
//|						1 the body
//|						2 the arms
//|						3 the head
//|						4 the legs
//|						5 the neck
//|						6 the rest
//o------------------------------------------------------------------------------------------------o
static bool CChar_Defense(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3)
	{
		ScriptError(cx, "Defense: Invalid number of arguments (takes 3, the hit location, the resist type and if the armor should get damaged)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the char
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "Defense: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "Defense: Passed an invalid Character");
		return false;
	}

	JS::HandleValue hitLoc = args.get(0);
	JS::HandleValue resistType = args.get(1);
	JS::HandleValue doArmorDamage = args.get(2);

	args.rval().setInt32(Combat->CalcDef(mChar, static_cast<UI08>(hitLoc.toInt()), doArmorDamage.toBool(), static_cast<WeatherType>(resistType.toInt())));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_GetMoreVar()
//|	Prototype	-	int GetMoreVar( moreVarName, moreVarPart )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the value of the specified moreVarName's moreVarPart
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Valid moreVarName values: "more", "more0", "more1", "more2", "morex", "morey", "morez"
//|					Valid moreVarPart values: 1, 2, 3, 4
//o------------------------------------------------------------------------------------------------o
static bool CItem_GetMoreVar(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 2)
	{
		ScriptError(cx, "GetMoreVar: Invalid number of arguments (takes 2, the moreVarName (more, more0, more1, more2, morex, morey or morez - and the moreVarPart (1 to 4))");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CItem* mItem = nullptr;

	// Let's validate the item
	if (className == "UOXItem")
	{
		mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj.get(), 0);
		if (!ValidateObject(mItem))
		{
			ScriptError(cx, "GetMoreVar: Passed an invalid Item");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "GetMoreVar: Passed an invalid Item");
		return false;
	}

	// Fetch data from the function arguments, and figure out which tempVar to get data from
	UI08 moreVar = 0;
	UI08 moreVarPart = static_cast<UI08>(args.get(1).toInt32());
	std::string moreVarName = convertToString(cx, args.get(0).toString());
	if (moreVarName == "more")
	{
		moreVar = 0;
	}
	else if (moreVarName == "morex")
	{
		moreVar = 1;
	}
	else if (moreVarName == "morey")
	{
		moreVar = 2;
	}
	else if (moreVarName == "morez")
	{
		moreVar = 3;
	}
	else if (moreVarName == "more0")
	{
		moreVar = 4;
	}
	else if (moreVarName == "more1")
	{
		moreVar = 5;
	}
	else if (moreVarName == "more2")
	{
		moreVar = 6;
	}
	else
	{
		ScriptError(cx, "GetMoreVar: Passed an invalid argument: tempVarName");
		return false;
	}

	// Fetch the value of the moreVarPart and return it to the script
	args.rval().setInt32(mItem->GetTempVar(static_cast<CITempVars>(moreVar), moreVarPart));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_SetMoreVar()
//|	Prototype	-	int SetMoreVar( moreVar, moreVarPart, tempVarValue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the value of the specified moreVar's moreVarPart to moreVarValue
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Valid moreVarName values: "more", "more0", "more1", "more2", "morex", "morey", "morez"
//|					Valid moreVarPart values: 1, 2, 3, 4
//|					Valid moreVarValue values: 0 - 255
//o------------------------------------------------------------------------------------------------o
static bool CItem_SetMoreVar(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 3)
	{
		ScriptError(cx, "SetMoreVar: Invalid number of arguments (takes 3, the moreVarName (more, more0, more1, more2, morex, morey or morez); the moreVarPart (1 to 4) and the moreVarValue (0-255)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CItem* mItem = nullptr;

	// Let's validate the item
	if (className == "UOXItem")
	{
		mItem = JS::GetMaybePtrFromReservedSlot<CItem>(obj.get(), 0);
		if (!ValidateObject(mItem))
		{
			ScriptError(cx, "SetMoreVar: Passed an invalid Item");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "SetMoreVar: Passed an invalid Item");
		return false;
	}

	// Fetch data from the function arguments, and figure out which moreVar to set data for
	UI08 moreVar = 0;
	UI08 moreVarPart = static_cast<UI08>(args.get(1).toInt32());
	UI08 moreVarValue = static_cast<UI08>(args.get(2).toInt32());
	std::string moreVarName = convertToString(cx, args.get(0).toString());
	if (moreVarName == "more")
	{
		moreVar = 0;
	}
	else if (moreVarName == "morex")
	{
		moreVar = 1;
	}
	else if (moreVarName == "morey")
	{
		moreVar = 2;
	}
	else if (moreVarName == "morez")
	{
		moreVar = 3;
	}
	else if (moreVarName == "more0")
	{
		moreVar = 4;
	}
	else if (moreVarName == "more1")
	{
		moreVar = 5;
	}
	else if (moreVarName == "more2")
	{
		moreVar = 6;
	}
	else
	{
		ScriptError(cx, "SetMoreVar: Passed an invalid argument: moreVarName");
		return false;
	}

	// Fetch the value of the moreVarPart and return it to the script
	mItem->SetTempVar(static_cast<CITempVars>(moreVar), moreVarPart, moreVarValue);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_AddScriptTrigger()
//|	Prototype	-	void AddScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a scriptTrigger to an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
static bool CBase_AddScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddScriptTrigger: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Invalid Object assigned (AddScriptTrigger)");
		return false;
	}

	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "That is not a valid script trigger! Only integers between 0-65535 are accepted.");
	}

	UI16 scriptId = static_cast<UI16>(args.get(0).toInt32());
	if (scriptId > 0)
	{
		cScript* toExecute = JSMapping->GetScript(scriptId);
		if (toExecute == nullptr)
		{
			ScriptError(cx, oldstrutil::format("Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId).c_str());
			return false;
		}
		else
		{
			myObj->AddScriptTrigger(scriptId);
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_RemoveScriptTrigger()
//|	Prototype	-	void RemoveScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a scriptTrigger from an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
static bool CBase_RemoveScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "RemoveScriptTrigger: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Invalid Object assigned (RemoveScriptTrigger)");
		return false;
	}

	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "That is not a valid script trigger! Only integers between 0-65535 are accepted.");
	}

	UI16 scriptId = static_cast<UI16>(args.get(0).toInt32());
	if (scriptId > 0)
	{
		myObj->RemoveScriptTrigger(scriptId);
	}
	else
	{
		myObj->ClearScriptTriggers();
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_HasScriptTrigger()
//|	Prototype	-	void HasScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if object has a specific scriptTrigger in it's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
static bool CBase_HasScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "HasScriptTrigger: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CBaseObject* myObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>(obj, 0);
	if (!ValidateObject(myObj))
	{
		ScriptError(cx, "Invalid Object assigned (HasScriptTrigger)");
		return false;
	}

	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "That is not a valid script trigger! Only integers between 0-65535 are accepted.");
	}

	UI16 scriptId = static_cast<UI16>(args.get(0).toInt32());
	if (scriptId > 0 && myObj->HasScriptTrigger(scriptId))
	{
		args.rval().setBoolean(true);
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRegion_AddScriptTrigger()
//|	Prototype	-	void AddScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a scriptTrigger to an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
static bool CRegion_AddScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddScriptTrigger: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CTownRegion* myObj = JS::GetMaybePtrFromReservedSlot<CTownRegion>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "Invalid Object assigned (AddScriptTrigger)");
		return false;
	}

	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "That is not a valid script trigger! Only integers between 0-65535 are accepted.");
		return false;
	}

	UI16 scriptId = static_cast<UI16>(args.get(0).toInt32());
	if (scriptId > 0)
	{
		cScript* toExecute = JSMapping->GetScript(scriptId);
		if (toExecute == nullptr)
		{
			ScriptError(cx, oldstrutil::format("Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId).c_str());
			return false;
		}
		else
		{
			myObj->AddScriptTrigger(scriptId);
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRegion_RemoveScriptTrigger()
//|	Prototype	-	void RemoveScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a scriptTrigger from an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
static bool CRegion_RemoveScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "RemoveScriptTrigger: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CTownRegion* myObj = JS::GetMaybePtrFromReservedSlot<CTownRegion>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "Invalid Object assigned (RemoveScriptTrigger)");
		return false;
	}

	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "That is not a valid script trigger! Only integers between 0-65535 are accepted.");
	}

	UI16 scriptId = static_cast<UI16>(args.get(0).toInt32());
	if (scriptId > 0)
	{
		myObj->RemoveScriptTrigger(scriptId);
	}
	else
	{
		myObj->ClearScriptTriggers();
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRegion_GetOrePref()
//|	Prototype	-	void GetOrePref( oreType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get ore preference data for specified ore type
//o------------------------------------------------------------------------------------------------o
static bool CRegion_GetOrePref(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "GetOrePref: Invalid number of arguments (takes 1 - oreId)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CTownRegion* myObj = JS::GetMaybePtrFromReservedSlot<CTownRegion>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "Invalid Object assigned (GetOrePref)");
		return false;
	}

	if (!args.get(0).isInt32())
	{
		ScriptError(cx, "That is not a valid ore ID! Only integers between 0-65535 are accepted.");
	}

	// OrePref_st
	//	MiningData_st oreIndex
	//	UI16 percentChance

	// MiningData_st
	//	std::string oreName - ore name from ORE_LIST in skills.dfn
	//	UI16 colour
	//	UI16 minSkill - min skill needed to make ingot from ore
	//	std::string name - name of ingot
	//	SI32 makemenu
	//	UI16 oreChance - default chance of finding ore type if nothing else is specified
	//	UI16 scriptID

	// Fetch region's ore preference details for specified ore type
	size_t oreType = static_cast<size_t>(args.get(0).toInt32());
	auto orePrefs = myObj->GetOrePreference(oreType);

	// Prepare some temporary helper variables
	JSObject* jsOrePref = JS_NewArrayObject(cx, 0, nullptr);
	JSObject* jsMiningData = JS_NewArrayObject(cx, 0, nullptr);

	// Set up the mining data info
	// Start with name of ore
	JSString* oreName = nullptr;
	oreName = JS_NewStringCopyZ(cx, orePrefs->oreIndex->oreName.c_str());
	auto jsOreName = STRING_TO_JSVAL(oreName);
	JS_SetElement(cx, jsMiningData, 0, &jsOreName);

	// Name of ingot
	JSString* ingotName = nullptr;
	ingotName = JS_NewStringCopyZ(cx, orePrefs->oreIndex->name.c_str());
	auto jsIngotName = STRING_TO_JSVAL(ingotName);
	JS_SetElement(cx, jsMiningData, 3, &jsIngotName);

	// Ore colour, min skill, Makemenu entry, oreChance, scriptID
	auto jsOreColor = INT_TO_JSVAL(orePrefs->oreIndex->colour);
	auto jsOreMinSkill = INT_TO_JSVAL(orePrefs->oreIndex->minSkill);
	auto jsOreMakemenu = INT_TO_JSVAL(orePrefs->oreIndex->makemenu);
	auto jsOreChance = INT_TO_JSVAL(orePrefs->oreIndex->oreChance);
	auto jsOreScriptID = INT_TO_JSVAL(orePrefs->oreIndex->scriptID);
	JS_SetElement(cx, jsMiningData, 1, &jsOreColor);
	JS_SetElement(cx, jsMiningData, 2, &jsOreMinSkill);
	JS_SetElement(cx, jsMiningData, 4, &jsOreMakemenu);
	JS_SetElement(cx, jsMiningData, 5, &jsOreChance);
	JS_SetElement(cx, jsMiningData, 6, &jsOreScriptID);

	// Add mining data to the orePref array
	jsval miningDataVal = OBJECT_TO_JSVAL(jsMiningData);
	JS_SetElement(cx, jsOrePref, 0, &miningDataVal);

	// Add percent chance to orePref array
	jsval jsOrePrefChance = INT_TO_JSVAL(orePrefs->percentChance);
	JS_SetElement(cx, jsOrePref, 1, &jsOrePrefChance);

	// Convert orePref array object to jsval and pass it to script
	args.rval().setObjectOrNull( jsOrePref );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRegion_GetOreChance()
//|	Prototype	-	void GetOreChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get chance to find ore when mining in townregion
//o------------------------------------------------------------------------------------------------o
static bool CRegion_GetOreChance(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "GetOreChance: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CTownRegion* myObj = JS::GetMaybePtrFromReservedSlot<CTownRegion>(obj, 0);
	if (myObj == nullptr)
	{
		ScriptError(cx, "Invalid Object assigned (GetOreChance)");
		return false;
	}

	args.rval().setInt32(myObj->GetOreChance());
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddFriend()
//|	Prototype	-	bool Add( playerToAdd )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds player to an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
static bool CChar_AddFriend(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddFriend: Invalid number of arguments (takes 1 - playerObject)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "AddFriend: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "AddFriend: Passed an invalid Character");
		return false;
	}

	CChar* newFriend = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(newFriend))
	{
		ScriptError(cx, "(AddFriend) Invalid Object passed as function parameter");
		return false;
	}

	args.rval().setBoolean(mChar->AddFriend(newFriend));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_RemoveFriend()
//|	Prototype	-	bool Add( playerToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes player from an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
static bool CChar_RemoveFriend(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "RemoveFriend: Invalid number of arguments (takes 1 - playerObject)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "RemoveFriend: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "RemoveFriend: Passed an invalid Character");
		return false;
	}

	CChar* friendToRemove = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(friendToRemove))
	{
		ScriptError(cx, "(AddFriend) Invalid Object passed as function parameter");
		return false;
	}

	args.rval().setBoolean(mChar->RemoveFriend(friendToRemove));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetFriendList()
//|	Prototype	-	bool GetFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
static bool CChar_GetFriendList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "GetFriendList: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "GetFriendList: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "GetFriendList: Passed an invalid Character");
		return false;
	}

	// Fetch actual friend list
	auto friendList = mChar->GetFriendList();

	// Prepare some temporary helper variables
	JSObject* jsFriendList = JS_NewArrayObject(cx, 0, nullptr);
	jsval jsTempFriend;

	// Loop through list of friends, and add each one to the JS ArrayObject
	int i = 0;
	for (auto& tempFriend : *friendList)
	{
		// Create a new JS Object based on character
		JSObject* myObj = JSEngine->AcquireObject(IUE_CHAR, tempFriend, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));

		// Convert JS Object to jsval
		jsTempFriend = OBJECT_TO_JSVAL(myObj);

		// Add jsval to ArrayObject
		JS_SetElement(cx, jsFriendList, i, &jsTempFriend);
		i++;
	}

	// Convert ArrayObject to jsval and pass it to script
	args.rval().setObjectOrNull( jsFriendList );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ClearFriendList()
//|	Prototype	-	bool ClearFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
static bool CChar_ClearFriendList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "ClearFriendList: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "ClearFriendList: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "ClearFriendList: Passed an invalid Character");
		return false;
	}

	// Clear friend list
	mChar->ClearFriendList();

	args.rval().setBoolean(true);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetPetList()
//|	Prototype	-	bool GetPetList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of character's owned pets
//o------------------------------------------------------------------------------------------------o
static bool CChar_GetPetList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "GetPetList: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "GetPetList: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "GetPetList: Passed an invalid Character");
		return false;
	}

	// Fetch actual pet list
	auto petList = mChar->GetPetList();

	// Prepare some temporary helper variables
	JSObject* jsPetList = JS_NewArrayObject(cx, 0, nullptr);
	jsval jsTempPet;

	// Loop through list of pets, and add each one to the JS ArrayObject
	int i = 0;
	for (const auto& pet : petList->collection())
	{
		if (ValidateObject(pet))
		{
			if (pet->GetOwnerObj() == mChar)
			{
				// Create a new JS Object based on character
				JSObject* myObj = JSEngine->AcquireObject(IUE_CHAR, pet, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));

				// Convert JS Object to jsval
				jsTempPet = OBJECT_TO_JSVAL(myObj);

				// Add jsval to ArrayObject
				JS_SetElement(cx, jsPetList, i, &jsTempPet);
				i++;
			}
		}
	}

	// Convert ArrayObject to jsval and pass it to script
	args.rval().setObjectOrNull( jsPetList );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_HasBeenOwner()
//|	Prototype	-	bool HasBeenOwner( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether character pChar has previously owned the pet (is on pet owner list)
//o------------------------------------------------------------------------------------------------o
static bool CChar_HasBeenOwner(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "HasBeenOwner: Invalid number of arguments (takes 1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "HasBeenOwner: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "HasBeenOwner: Passed an invalid Character");
		return false;
	}

	JS::HandleValue toCheck = args.get(0);
	CChar* pChar = static_cast<CChar*>(toCheck.toObject());
	if (!ValidateObject(pChar))
	{
		ScriptError(cx, "HasBeenOwner: Invalid Character passed as parameter");
		return false;
	}

	bool hasBeenOwner = mChar->IsOnPetOwnerList(pChar);

	args.rval().setBoolean(hasBeenOwner);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CalculateControlChance()
//|	Prototype	-	int CalculateControlChance( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns chance (in values ranging from 0 to 1000) of pChar (player) successfully controlling mChar (pet)
//o------------------------------------------------------------------------------------------------o
static bool CChar_CalculateControlChance(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "CalculateControlChance: Invalid number of arguments (takes 1 - pChar)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "CalculateControlChance: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "CalculateControlChance: Passed an invalid Character");
		return false;
	}

	JS::HandleValue toCheck = args.get(0);
	CChar* pChar = static_cast<CChar*>(toCheck.toObject());
	if (!ValidateObject(pChar))
	{
		ScriptError(cx, "CalculateControlChance: Invalid Character passed as parameter");
		return false;
	}

	UI16 petControlChance = Skills->CalculatePetControlChance(mChar, pChar);

	args.rval().setInt32(petControlChance);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddFollower()
//|	Prototype	-	bool AddFollower( npcToAdd )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds NPC to player's list of active followers
//o------------------------------------------------------------------------------------------------o
static bool CChar_AddFollower(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "AddFollower: Invalid number of arguments (takes 1 - npcObject)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "AddFollower: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "AddFollower: Passed an invalid Character");
		return false;
	}

	CChar* newFollower = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(newFollower))
	{
		ScriptError(cx, "(AddFollower) Invalid Object passed as function parameter");
		return false;
	}

	args.rval().setBoolean(mChar->AddFollower(newFollower));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_RemoveFollower()
//|	Prototype	-	bool Remove( followerToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a follower from player's follower list
//o------------------------------------------------------------------------------------------------o
static bool CChar_RemoveFollower(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "RemoveFollower: Invalid number of arguments (takes 1 - followerObject)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "RemoveFollower: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "RemoveFollower: Passed an invalid Character");
		return false;
	}

	CChar* followerToRemove = JS::GetMaybePtrFromReservedSlot<CChar>(args.get(0).toObjectOrNull(), 0);
	if (!ValidateObject(followerToRemove))
	{
		ScriptError(cx, "(RemoveFollower) Invalid Object passed as function parameter");
		return false;
	}

	args.rval().setBoolean(mChar->RemoveFollower(followerToRemove));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetFollowerList()
//|	Prototype	-	bool GetFollowerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of character's active followers
//o------------------------------------------------------------------------------------------------o
static bool CChar_GetFollowerList(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "GetFollowerList: Invalid number of arguments (takes 0)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	CChar* mChar = nullptr;

	// Let's validate the character
	if (className == "UOXChar")
	{
		mChar = JS::GetMaybePtrFromReservedSlot<CChar>(obj.get(), 0);
		if (!ValidateObject(mChar))
		{
			ScriptError(cx, "GetFollowerList: Passed an invalid Character");
			return false;
		}
	}
	else
	{
		ScriptError(cx, "GetFollowerList: Passed an invalid Character");
		return false;
	}

	// Fetch actual active follower list
	auto followerList = mChar->GetFollowerList();

	// Prepare some temporary helper variables
	JSObject* jsFollowerList = JS_NewArrayObject(cx, 0, nullptr);
	jsval jsTempFollower;

	// Loop through list of friends, and add each one to the JS ArrayObject
	int i = 0;
	for (const auto& follower : followerList->collection())
	{
		if (ValidateObject(follower))
		{
			if (follower->GetOwnerObj() == mChar)
			{
				// Create a new JS Object based on character
				JSObject* myObj = JSEngine->AcquireObject(IUE_CHAR, follower, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));

				// Convert JS Object to jsval
				jsTempFollower = OBJECT_TO_JSVAL(myObj);

				// Add jsval to ArrayObject
				JS_SetElement(cx, jsFollowerList, i, &jsTempFollower);
				i++;
			}
		}
	}

	// Convert ArrayObject to jsval and pass it to script
	args.rval().setObjectOrNull( jsFollowerList );
	return true;
}

// Party Methods
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CParty_Remove()
//|	Prototype	-	bool Remove( playerToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes player from party
//o------------------------------------------------------------------------------------------------o
static bool CParty_Remove(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "Remove: Invalid number of arguments (1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	if (className == "UOXParty")
	{
		Party* ourParty = static_cast<Party*>(myClass.toObject());
		if (ourParty == nullptr)
		{
			ScriptError(cx, "Remove: Invalid party");
			return false;
		}

		JS::HandleValue toRemove = args.get(0);
		CChar* charToRemove = static_cast<CChar*>(toRemove.toObject());
		if (!ValidateObject(charToRemove))
		{
			ScriptError(cx, "Remove: Invalid character to remove");
			return false;
		}
		args.rval().setBoolean(ourParty->RemoveMember(charToRemove));
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CParty_Add()
//|	Prototype	-	bool Add( playerToAdd )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds player to party
//o------------------------------------------------------------------------------------------------o
static bool CParty_Add(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 1)
	{
		ScriptError(cx, "Add: Invalid number of arguments (1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;

	// let's setup our default return value here
	args.rval().setBoolean(false);

	if (className == "UOXParty")
	{
		Party* ourParty = static_cast<Party*>(myClass.toObject());
		if (ourParty == nullptr)
		{
			ScriptError(cx, "Add: Invalid party");
			return false;
		}

		JS::HandleValue toAdd = args.get(0);
		CChar* charToAdd = static_cast<CChar*>(toAdd.toObject());
		if (!ValidateObject(charToAdd))
		{
			ScriptError(cx, "Add: Invalid character to add");
			return false;
		}
		if (charToAdd->IsNpc())
		{
			if (ourParty->IsNPC())
			{
				args.rval().setBoolean(ourParty->AddMember(charToAdd));
			}
			else
			{
				args.rval().setBoolean(false);
				ScriptError(cx, "Add: Adding NPC to a PC party attempted");
			}
		}
		else
		{
			if (ourParty->IsNPC())
			{
				args.rval().setBoolean(false);
				ScriptError(cx, "Add: Adding PC to a NPC party attempted");
			}
			else
			{	// Send PC the invite
				// OK, what this means is that it's a PC party, and because it exists
				// we know that the leader needs to be active!  And if he's active,
				// he should ALSO be online

				CSocket* targSock = charToAdd->GetSocket();
				if (targSock != nullptr)
				{
					CChar* leader = ourParty->Leader();
					CSocket* leaderSock = leader->GetSocket();

					if (leaderSock != nullptr)
					{
						CPPartyInvitation toSend;
						toSend.Leader(leader);
						targSock->Send(&toSend);
						targSock->SysMessage(9002); // You have been invited to join a party, type /accept or /decline to deal with the invitation
						args.rval().setBoolean(true);
					}
					else
					{
						args.rval().setBoolean(false);
						ScriptError(cx, "Add: PC selected is not online");
					}
				}
				else
				{
					args.rval().setBoolean(false);
					ScriptError(cx, "Add: PC selected is not online");
				}
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CParty_GetMember()
//|	Prototype	-	bool GetMember( indexPos )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets party member at specified index in list of party members
//o------------------------------------------------------------------------------------------------o
static bool CParty_GetMember(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc > 1)
	{
		ScriptError(cx, "GetMember: Invalid number of arguments (1)");
		return false;
	}

	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	std::string className = JS::GetClass(obj.get())->name;
	if (className == "UOXParty")
	{
		Party* ourParty = static_cast<Party*>(myClass.toObject());
		if (ourParty == nullptr)
		{
			ScriptError(cx, "GetMember: Invalid party");
			return false;
		}

		JS::HandleValue toGetMember = args.get(0);
		size_t memberOffset = toGetMember.toInt();
		if (memberOffset >= ourParty->MemberList()->size())
		{
			ScriptError(cx, "GetMember: Invalid character to get, index out of bounds");
			args.rval().setNull();
			return true;
		}
		CChar* mChar = (*(ourParty->MemberList()))[memberOffset]->Member();
		if (mChar == nullptr)
		{
			args.rval().setNull();
		}
		else
		{
			JSObject* myJSChar = JSEngine->AcquireObject(IUE_CHAR, mChar, JSEngine->FindActiveRuntime(JS_GetRuntime(cx)));
			args.rval().setObjectOrNull( myJSChar );
		}
	}
	else
		args.rval().setNull();
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_FirstTriggerWord()
//|	Prototype	-	int FirstTriggerWord()
//|	Date		-	June 12, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns first trigger word in the socket's list
//o------------------------------------------------------------------------------------------------o
static bool CSocket_FirstTriggerWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "FirstTriggerWord: Invalid count of arguments :%d, needs :0", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mySock == nullptr)
	{
		ScriptError(cx, "FirstTriggerWord: Invalid socket assigned.");
		return false;
	}
	UI16 trigWord = mySock->FirstTrigWord();
	args.rval().setInt32(trigWord);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_NextTriggerWord()
//|	Prototype	-	int FirstTriggerWord()
//|	Date		-	June 12, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns next trigger word in the socket's list
//o------------------------------------------------------------------------------------------------o
static bool CSocket_NextTriggerWord(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "NextTriggerWord: Invalid count of arguments :%d, needs :0", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mySock == nullptr)
	{
		ScriptError(cx, "NextTriggerWord: Invalid socket assigned.");
		return false;
	}
	UI16 trigWord = mySock->NextTrigWord();
	args.rval().setInt32(trigWord);
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_FinishedTriggerWords()
//|	Prototype	-	int FinishedTriggerWords()
//|	Date		-	June 12, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if finished all trigger words in the socket's list
//o------------------------------------------------------------------------------------------------o
static bool CSocket_FinishedTriggerWords(JSContext* cx, unsigned argc, JS::Value* vp)
{
	if (argc != 0)
	{
		ScriptError(cx, "FinishedTriggerWords: Invalid count of arguments :%d, needs :0", argc);
		return false;
	}
	auto args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	if (!args.computeThis(cx, &obj))
		return false;
	CSocket* mySock = JS::GetMaybePtrFromReservedSlot<CSocket>(obj, 0);
	if (mySock == nullptr)
	{
		ScriptError(cx, "FinishedTriggerWords: Invalid socket assigned.");
		return false;
	}

	args.rval().setBoolean(mySock->FinishedTrigWords());
	return true;
}

