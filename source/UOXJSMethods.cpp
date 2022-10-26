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
#include "JSEncapsulate.h"
#include "CJSEngine.h"

#include "cMagic.h"
#include "cGuild.h"
#include "skills.h"
#include "speech.h"
#include "gump.h"
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

void BuildAddMenuGump( CSocket *s, UI16 m );	// Menus for item creation
void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld );
bool BuyShop( CSocket *s, CChar *c );
void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway );

void MethodError( const char *txt, ... )
{
	va_list argptr;
	va_start( argptr, txt );
	Console.Error( oldstrutil::format( txt, argptr ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MethodSpeech()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds speech entry of specified type, font, color, etc to the speech queue
//| Notes		-	Copied that here from SEFunctions.cpp. Default paramters weren't working !?
//o------------------------------------------------------------------------------------------------o
void MethodSpeech( CBaseObject &speaker, char *message, SpeechType sType, COLOUR sColour = 0x005A, 
		FontType fType = FNT_NORMAL, SpeechTarget spTrg = SPTRG_PCNPC, SERIAL spokenTo = INVALIDSERIAL, bool useUnicode = false )
{
	if( useUnicode )
	{
		bool sendAll = true;
		if( spTrg == SPTRG_INDIVIDUAL || spTrg == SPTRG_ONLYRECEIVER )
		{
			sendAll = false;
		}

		std::string fromChar( message );
		std::string speakerName = "";
		if( speaker.CanBeObjType( OT_CHAR ))
		{
			CChar *speakerChar = CalcCharObjFromSer( speaker.GetSerial() );
			speakerName = GetNpcDictName( speakerChar );
		}
		else
		{
			speakerName = speaker.GetName();
		}
		speakerName = oldstrutil::stringToWstringToString( speakerName );
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( fromChar );
		unicodeMessage.Font( fType );
		unicodeMessage.Colour( sColour );
		unicodeMessage.Type( sType );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( speakerName );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( speaker.GetSerial() );

		if( sendAll )
		{
			UI16 searchDistance = DIST_SAMESCREEN;
			if( sType == WHISPER || sType == ASCIIWHISPER )
			{
				searchDistance = DIST_SAMETILE;
			}
			else if( sType == YELL || sType == ASCIIYELL )
			{
				searchDistance = DIST_SAMESCREEN * 1.5;
			}
			else if( sType == EMOTE || sType == ASCIIEMOTE )
			{
				searchDistance = DIST_INRANGE;
			}
			else
			{
				searchDistance = DIST_SAMESCREEN;
			}

			for( auto &tSock: FindNearbyPlayers( &speaker, searchDistance ))
			{
				tSock->Send( &unicodeMessage );
			}
		}
		else
		{
			CChar *targChar = CalcCharObjFromSer( spokenTo );
			if( ValidateObject( targChar ))
			{
				CSocket *targSock = targChar->GetSocket();
				if( targSock != nullptr )
				{
					targSock->Send( &unicodeMessage );
				}
			}
		}
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Font( fType );
		toAdd.Speech( message );
		toAdd.Speaker( speaker.GetSerial() );
		toAdd.Type( sType );
		toAdd.TargType( spTrg );
		if( spTrg == SPTRG_INDIVIDUAL || spTrg == SPTRG_ONLYRECEIVER )
		{
			toAdd.SpokenTo( spokenTo );
		}

		// Added that because transparent text could cause trouble
		if( sColour != 0x1700 && sColour != 0x0)
		{
			toAdd.Colour( sColour );
		}
		else
		{
			toAdd.Colour( 0x0058 );
		}

		toAdd.At( cwmWorldState->GetUICurrentTime() );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Packet()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prepares a new packet stream, ready for network shenanigans in JS
//o------------------------------------------------------------------------------------------------o
JSBool Packet( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CPUOXBuffer *toAdd = new CPUOXBuffer;

	JS_DefineFunctions( cx, obj, CPacket_Methods );
	JS_SetPrivate( cx, obj, toAdd);
	JS_LockGCThing( cx, obj );
	//JS_AddRoot( cx, &obj );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_Free()
//|	Prototype	-	void Free()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes a packet stream
//o------------------------------------------------------------------------------------------------o
JSBool CPacket_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CPUOXBuffer *toDelete = static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, obj ));

	if( toDelete == nullptr )
		return JS_FALSE;

	delete toDelete;
	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, obj );
	JS_SetPrivate( cx, obj, nullptr );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteByte()
//|	Prototype	-	void WriteByte( position, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes a single byte at the specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
JSBool CPacket_WriteByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CPacket_WriteByte) Invalid Number of Arguments %d, needs: 2 ", argc );
		return JS_TRUE;
	}

	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, obj ));
	if( myPacket == nullptr )
	{
		MethodError( "(CPacket_WriteByte) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>( JSVAL_TO_INT( argv[0] ));
	UI08	toWrite		= static_cast<UI08>( JSVAL_TO_INT( argv[1] ));

	myPacket->GetPacketStream().WriteByte( position, toWrite );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteShort()
//|	Prototype	-	void WriteShort( position, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes two bytes at the specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
JSBool CPacket_WriteShort( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CPacket_WriteShort) Invalid Number of Arguments %d, needs: 2 ", argc );
		return JS_TRUE;
	}

	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, obj ));
	if( myPacket == nullptr )
	{
		MethodError( "(CPacket_WriteShort) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>( JSVAL_TO_INT( argv[0] ));
	UI16	toWrite		= static_cast<UI16>( JSVAL_TO_INT( argv[1] ));

	myPacket->GetPacketStream().WriteShort( position, toWrite );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteLong()
//|	Prototype	-	void WriteLong( position, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes four bytes at the specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
JSBool CPacket_WriteLong( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CPacket_WriteLong) Invalid Number of Arguments %d, needs: 2 ", argc );
		return JS_TRUE;
	}

	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, obj ));
	if( myPacket == nullptr )
	{
		MethodError( "(CPacket_WriteLong) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>( JSVAL_TO_INT( argv[0] ));
	//UI32	toWrite		= static_cast<UI32>( JSVAL_TO_INT( argv[1] ));
	char *	toWriteChar	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ));
	UI32 toWrite = oldstrutil::value<UI32>( toWriteChar );

	myPacket->GetPacketStream().WriteLong( position, toWrite );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_WriteString()
//|	Prototype	-	void WriteString( position, stringToWrite, length )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes variable number of bytes at specified position in the packet stream
//o------------------------------------------------------------------------------------------------o
JSBool CPacket_WriteString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "(CPacket_WriteString) Invalid Number of Arguments %d, needs: 3 ", argc );
		return JS_TRUE;
	}

	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, obj ));
	if( myPacket == nullptr )
	{
		MethodError( "(CPacket_WriteString) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>( JSVAL_TO_INT( argv[0] ));
	char *	toWrite		= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ));
	size_t	len			= static_cast<size_t>( JSVAL_TO_INT( argv[2] ));

	myPacket->GetPacketStream().WriteString( position, toWrite, len );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPacket_ReserveSize()
//|	Prototype	-	void ReserveSize( packetSize )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reserves a specific size for the packet stream
//o------------------------------------------------------------------------------------------------o
JSBool CPacket_ReserveSize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(CPacket_ReserveSize) Invalid Number of Arguments %d, needs: 1 ", argc );
		return JS_TRUE;
	}

	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, obj ));
	if( myPacket == nullptr )
	{
		MethodError( "(CPacket_ReserveSize) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t len = static_cast<size_t>( JSVAL_TO_INT( argv[0] ));


	myPacket->GetPacketStream().ReserveSize( len );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Send()
//|	Prototype	-	void Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends prepared packet stream to network socket
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_Send( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(CSocket_Send) Invalid Number of Arguments %d, needs: 1 ", argc );
		return JS_TRUE;
	}

	CSocket *mSock			= static_cast<CSocket *>( JS_GetPrivate( cx, obj ));
	JSObject *jsObj			= JSVAL_TO_OBJECT( argv[0] );
	CPUOXBuffer *myPacket	= static_cast<CPUOXBuffer *>( JS_GetPrivate( cx, jsObj ));

	if( mSock == nullptr || myPacket == nullptr )
	{
		MethodError( "(CPacket_WriteString) Invalid Object Passed" );
		return JS_TRUE;
	}

	mSock->Send( myPacket );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Gump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Cosntructor for creating a new JS Gump object
//o------------------------------------------------------------------------------------------------o
JSBool Gump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Allocate the GumpList here and "SetPrivate" it to the Object
	SEGump_st *toAdd = new SEGump_st;
	toAdd->one = new std::vector<std::string>();
	toAdd->two = new std::vector<std::string>();
	toAdd->textId = 0;

	JS_DefineFunctions( cx, obj, CGump_Methods );
	JS_SetPrivate( cx, obj, toAdd);
	JS_LockGCThing( cx, obj );
	//JS_AddRoot( cx, &obj );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_Free()
//|	Prototype	-	void Free()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes JS Gump object and frees up associated memory
//o------------------------------------------------------------------------------------------------o
JSBool CGump_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SEGump_st *toDelete = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));

	if( toDelete == nullptr )
		return JS_FALSE;

	delete toDelete->one;
	delete toDelete->two;
	delete toDelete;

	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, obj );
	JS_SetPrivate( cx, obj, nullptr );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_Free()
//|	Prototype	-	void Free()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes JS Gump Data object and frees up associated memory
//o------------------------------------------------------------------------------------------------o
JSBool CGumpData_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SEGumpData_st *toDelete = static_cast<SEGumpData_st*>( JS_GetPrivate( cx, obj ));

	if( toDelete == nullptr )
		return JS_FALSE;

	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, &obj );
	JS_SetPrivate( cx, obj, nullptr );

	delete toDelete;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_GetEdit()
//|	Prototype	-	void getEdit( index )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets data input from TextEntr(y/ies) in JS Gump
//o------------------------------------------------------------------------------------------------o
JSBool CGumpData_GetEdit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getEdit) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = STRING_TO_JSVAL( "" );
		return JS_TRUE;
	}

	SEGumpData_st *myItem = static_cast<SEGumpData_st *>( JS_GetPrivate( cx, obj ));

	if( myItem == nullptr  )
	{
		MethodError( "(DataGump-getEdit) Invalid object assigned" );
		*rval = STRING_TO_JSVAL( "" );
		return JS_TRUE;
	}

	size_t index = JSVAL_TO_INT( argv[0] );

	if( index < myItem->sEdits.size() )
	{
		*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, myItem->sEdits[index].c_str() ));
	}
	else
	{
		*rval = STRING_TO_JSVAL( "" );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_GetId()
//|	Prototype	-	void getID( index )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets nID at specified index in Gump Data
//o------------------------------------------------------------------------------------------------o
JSBool CGumpData_GetId( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getID) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = INT_TO_JSVAL( -1 );
		return JS_TRUE;
	}

	SEGumpData_st *myItem = static_cast<SEGumpData_st *>( JS_GetPrivate( cx, obj ));

	if( myItem == nullptr  )
	{
		MethodError( "(DataGump_getID) Invalid object assigned" );
		*rval = INT_TO_JSVAL( -1 );
		return JS_TRUE;
	}
	size_t index = JSVAL_TO_INT( argv[0] );

	if( index < myItem->nIDs.size() )
	{
		*rval = INT_TO_JSVAL( myItem->nIDs[index] );
	}
	else
	{
		*rval = INT_TO_JSVAL( -1 );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpData_GetButton()
//|	Prototype	-	void getButton( index )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets value of button at specified index in Gump Data
//o------------------------------------------------------------------------------------------------o
JSBool CGumpData_GetButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getbutton) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = INT_TO_JSVAL( -1 );
		return JS_TRUE;
	}

	SEGumpData_st *myItem = static_cast<SEGumpData_st *>( JS_GetPrivate( cx, obj ));

	if( myItem == nullptr  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned" );
		*rval = INT_TO_JSVAL( -1 );
		return JS_TRUE;
	}
	size_t index = JSVAL_TO_INT( argv[0] );
	if( index < myItem->nButtons.size() )
	{
		*rval = INT_TO_JSVAL( myItem->nButtons[index] );
	}
	else
	{
		*rval = INT_TO_JSVAL( -1 );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddCheckbox()
//|	Prototype	-	void AddCheckbox( topHeight, topLeft, checkImage, defaultStatus, unk2 )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a checkbox gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddCheckbox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 5 || argc > 6 )
	{
		MethodError( "AddCheckbox: Wrong count of Parameters, needs 5 or 6" );
		return JS_FALSE;
	}

	SI16 tL = 0;
	SI16 tR = 0;
	UI16 gImage = 0;
	UI16 gImageChk = 0;
	SI16 initState = 0;
	SI16 relay = 0;

	if( argc == 5 )
	{
		tL			= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		tR			= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
		gImage		= static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
		gImageChk	= gImage + 1;
		initState	= static_cast<SI16>( JSVAL_TO_INT( argv[3] ));
		relay		= static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
	}
	else
	{
		tL			= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		tR			= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
		gImage		= static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
		gImageChk	= static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
		initState	= static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
		relay		= static_cast<SI16>( JSVAL_TO_INT( argv[5] ));
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));

	if( gList == nullptr )
	{
		MethodError( "Gump_AddCheckbox: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "checkbox %i %i %i %i %i %i", tL, tR, gImage, gImageChk, initState, relay ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoClose()
//|	Prototype	-	void NoClose()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds noclose element to gump stream; specifies that gump cannot be closed by
//|					clicking the right mouse button
//o------------------------------------------------------------------------------------------------o
JSBool CGump_NoClose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoClose: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));

	if( gList == nullptr )
	{
		MethodError( "NoClose: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "noclose" );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoMove()
//|	Prototype	-	void NoMove()
//|	Date		-	9th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds nomove element to gump stream; specifies that gump cannot be moved
//o------------------------------------------------------------------------------------------------o
JSBool CGump_NoMove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoMove: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));

	if( gList == nullptr )
	{
		MethodError( "NoMove: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "nomove" );

	return JS_TRUE;
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
JSBool CGump_NoDispose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoDispose: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));

	if( gList == nullptr )
	{
		MethodError( "NoDispose: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "nodispose" );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_NoResize()
//|	Prototype	-	void NoResize()
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds noresize element to gump stream; specifies that gump cannot be resized
//o------------------------------------------------------------------------------------------------o
JSBool CGump_NoResize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoResize: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st *>( JS_GetPrivate( cx, obj ));

	if( gList == nullptr )
	{
		MethodError( "NoResize: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "noresize" );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_MasterGump()
//|	Date		-	24th March, 2012
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Possible that the mastergump command itself only has any effect with client versions between 4.0.4d and 5.0.5b?
//o------------------------------------------------------------------------------------------------o
JSBool CGump_MasterGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "MasterGump: Wrong count of Parameters, needs 1" );
		return JS_FALSE;
	}

	SI32 masterGumpId = static_cast<SI32>( JSVAL_TO_INT( argv[0] ));
	SEGump_st *gList = static_cast<SEGump_st *>( JS_GetPrivate( cx, obj ));

	if( gList == nullptr )
	{
		MethodError( "MasterGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	// Also send mastergump command with new gumpId
	gList->one->push_back( oldstrutil::format( "mastergump %i %i %i %i %i", masterGumpId ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddBackground()
//|	Prototype	-	void AddBackground( topHeight, topLeft, bottomHeight, bottomLeft, backImage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds background gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddBackground( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "AddBackground: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	SI16 tL = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	SI16 bL = static_cast<SI16>( JSVAL_TO_INT( argv[2] ));
	SI16 bR = static_cast<SI16>( JSVAL_TO_INT( argv[3] ));
	UI16 gImage = static_cast<UI16>( JSVAL_TO_INT( argv[4] ));

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddBackground: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "resizepic %i %i %i %i %i", tL, tR, gImage, bL, bR ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddButton()
//|	Prototype	-	void AddButton( topHeight, topLeft, buttonImage, (buttonPressedImage), unk1, unk2, unk3 )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds button gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 6 || argc > 7 )
	{
		MethodError( "AddButton: Invalid number of arguments (takes 6 or 7)" );
		return JS_FALSE;
	}
	SI16 tL = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 gImage = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	UI16 gImage2 = ( argc == 6 ? (gImage + 1) : static_cast<UI16>( JSVAL_TO_INT( argv[3] )));
	SI16 x1 = ( argc == 6 ? static_cast<SI16>( JSVAL_TO_INT( argv[3] )) : static_cast<SI16>( JSVAL_TO_INT( argv[4] )));
	SI16 x2 = ( argc == 6 ? static_cast<SI16>( JSVAL_TO_INT( argv[4] )) : static_cast<SI16>( JSVAL_TO_INT( argv[5] )));
	SI16 x3 = ( argc == 6 ? static_cast<SI16>( JSVAL_TO_INT( argv[5] )) : static_cast<SI16>( JSVAL_TO_INT( argv[6] )));

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddButton: Couldn't find gump associated with object" );
		return JS_FALSE;
	}
	
	gList->one->push_back( oldstrutil::format( "button %i %i %i %i %i %i %i", tL, tR, gImage, gImage2, x1, x2, x3 ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddButtonTileArt()
//|	Prototype	-	void AddButtonTileArt( topHeight, topLeft, buttonImage,
//|								buttonPressedImage, unk1, unk2, unk3, tileId, hue, tileX, tileY )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds button gump with tileart to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddButtonTileArt( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 11 )
	{
		MethodError( "AddButtonTileArt: Invalid number of arguments (takes 11)" );
		return JS_FALSE;
	}
	SI16 tL = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 tileIdNorm = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	UI16 tileIdPush = static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
	SI16 buttonType = static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
	SI16 pageNum = static_cast<SI16>( JSVAL_TO_INT( argv[5] ));
	SI16 buttonId = static_cast<SI16>( JSVAL_TO_INT( argv[6] ));
	SI16 tileId = static_cast<SI16>( JSVAL_TO_INT( argv[7] ));
	SI16 hue = static_cast<SI16>( JSVAL_TO_INT( argv[8] ));
	SI16 tileX = static_cast<SI16>( JSVAL_TO_INT( argv[9] ));
	SI16 tileY = static_cast<SI16>( JSVAL_TO_INT( argv[10] ));
	//SI32 cliloc = 0;

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddButtonTileArt: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "buttontileart %i %i %i %i %i %i %i %i %i %i %i", tL, tR, tileIdNorm, tileIdPush, buttonType, pageNum, buttonId, tileId, hue, tileX, tileY ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPageButton()
//|	Prototype	-	void AddPageButton( topHeight, topLeft, buttonImage, (buttonPressedImage), pageNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds button gump for navigating gump pages to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddPageButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 4 || argc > 5 )
	{
		MethodError( "AddPageButton: Invalid number of arguments (takes 4 or 5)" );
		return JS_FALSE;
	}
	SI16 tL = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 gImage = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	UI16 gImage2 = ( argc == 4 ? ( gImage + 1 ) : static_cast<UI16>( JSVAL_TO_INT( argv[3] )));
	SI16 pageNum = ( argc == 4 ? static_cast<SI16>( JSVAL_TO_INT( argv[3] )) : static_cast<SI16>( JSVAL_TO_INT( argv[4] )));

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddPageButton: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "button %i %i %i %i 0 %i", tL, tR, gImage, gImage2, pageNum ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddCheckerTrans()
//|	Prototype	-	void AddCheckerTrans( left, top, width, height )
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds transparent area gump to gump stream, turns underlying elements transparent
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddCheckerTrans( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddCheckerTrans: Invalid number of arguments (takes 5 x y width height)" );
		return JS_FALSE;
	}

	SI16 x		= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 y		= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 width	= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // width
	SI16 height	= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // height

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddCheckerTrans: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "checkertrans %i %i %i %i", x, y, width, height ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddCroppedText()
//|	Prototype	-	void AddCroppedText( left, top, hue, width, height, actualText)
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds text field gump to gump stream, cropped to certain dimensions
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddCroppedText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		MethodError( "AddCroppedText: Invalid number of arguments (takes 6 x y hue width height text)" );
		return JS_FALSE;
	}

	SI16 TextX		= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 TextY		= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 TextHue	= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // Hue
	SI16 TextWidth	= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // width
	SI16 TextHeight	= static_cast<SI16>( JSVAL_TO_INT( argv[4] )); // height

	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[5] ));

	if( TextString == nullptr )
	{
		MethodError( "AddCroppedText: Text is required" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddCroppedText: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	UI32 textId = gList->textId;
	++gList->textId;

	gList->one->push_back( oldstrutil::format( "croppedtext %i %i %i %i %i %u", TextX, TextY, TextWidth, TextHeight, TextHue, textId ));
	gList->two->push_back( TextString );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddGroup()
//|	Prototype	-	void AddGroup( groupNum )
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds new group element to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddGroup( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddGroup: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddGroup: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "group %d", JSVAL_TO_INT( argv[0] )));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_EndGroup()
//|	Prototype	-	void EndGroup( groupNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Ends a previously started group element
//o------------------------------------------------------------------------------------------------o
JSBool CGump_EndGroup( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "EndGroup: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "EndGroup: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "endgroup", JSVAL_TO_INT( argv[0] )));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddGump()
//|	Prototype	-	void AddGump( topHeight, topLeft, gumpImage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds image gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		MethodError( "AddGump: Invalid number of arguments (takes 3 or 4 - topLeft, topRight, imageID and hue (optional))" );
		return JS_FALSE;
	}

	SI16 tL = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 gImage = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	SI32 rgbColor = 0;
	if( argc == 4 )
	{
		rgbColor = static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	if( rgbColor == 0 )
	{
		gList->one->push_back( oldstrutil::format( "gumppic %i %i %i", tL, tR, gImage ));
	}
	else
	{
		gList->one->push_back( oldstrutil::format( "gumppic %i %i %i hue=%i", tL, tR, gImage, rgbColor ));
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddGumpColor()
//|	Prototype	-	void AddGumpColor( topHeight, topLeft, gumpImage, hue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds image gump to gump stream, with additional parameter for hue
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddGumpColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddGumpColor: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	SI16 tL			= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR			= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 gImage		= static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	SI32 rgbColour	= static_cast<SI32>( JSVAL_TO_INT( argv[3] ));	// colour

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddGumpColor: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "gumppic %i %i %i hue=%i", tL, tR, gImage, rgbColour ));

	return JS_TRUE;
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
JSBool CGump_AddToolTip( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 11 )
	{
		MethodError( "AddToolTip: Invalid number of arguments (takes at least 1, maximum 11)" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddToolTip: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 tooltip = static_cast<SI32>( JSVAL_TO_INT( argv[0] ));
	std::stringstream temp;
	if( argc > 1 )
	{
		// Additional arguments were provided along with the cliloc number
		// Only supported by client versions above ~7.0.16.0
		temp << "tooltip " << tooltip << " @";
		std::string tempArg;
		for( UI32 i = 2; i < argc; i++ )
		{
			tempArg = JS_GetStringBytes( JS_ValueToString( cx, argv[i] ));
			temp << ( i == 2 ? tempArg : ( "\t" + tempArg ));
		}
		temp << "@";
	}
	else
	{
		// Cliloc number was provided with no additional arguments,
		// or client version doesn't support arguments for tooltip
		temp << "tooltip " << tooltip;
	}

	gList->one->push_back( temp.str() );
	return JS_TRUE;
}

//o-------------------------------------------------------------------------------- ---------------o
//|	Function	-	CGump_AddHTMLGump()
//|	Prototype	-	void AddHTMLGump( topHeight, topLeft, width, height, background, scrollbar, HTMLText)
//|	Date		-	16th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new HTML gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddHTMLGump: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x				= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 y				= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 width			= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // width
	SI16 height			= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // height
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );

	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[6] ));

	if( TextString == nullptr )
	{
		MethodError( "AddHTMLGump: Text is required" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddHTMLGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	UI32 textId = gList->textId;
	++gList->textId;

	SI32 iBrd	= ( hasBorder ? 1 : 0 );
	SI32 iScrl	= ( hasScrollbar ? 1 : 0 );

	gList->one->push_back( oldstrutil::format( "htmlgump %i %i %i %i %u %i %i", x, y, width, height, textId, iBrd, iScrl ));
	gList->two->push_back( TextString );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPage()
//|	Prototype	-	void AddPage( pageNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new page element to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddPage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddPage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddPage: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "page %d", JSVAL_TO_INT( argv[0] )));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPicture()
//|	Prototype	-	void AddPicture( topHeight, topLeft, tilePic )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a tile image gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddPicture( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "AddPicture: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	SI16 tL		= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR		= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 gImage = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddPicture: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "tilepic %i %i %i", tL, tR, gImage ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPictureColor()
//|	Prototype	-	void AddPictureColor( topHeight, topLeft, tilePic, hue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a tile image gump to gump stream, with additional parameter for hue
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddPictureColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddPicture: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	SI16 tL			= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR			= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	UI16 gImage		= static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	SI32 rgbColour	= static_cast<SI32>( JSVAL_TO_INT( argv[3] ));	// colour

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddPictureColor: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "tilepichue %i %i %i %i", tL, tR, gImage, rgbColour ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddPicInPic()
//|	Prototype	-	void AddPicInPic( topLeft, topHeight, gumpImage, spriteX, spriteY, width, height )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a picinpic gump to the gump stream
//|	Notes		-	Requires client v7.0.80.0 or above
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddPicInPic( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddPicInPic: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // starting x
	SI16 y			= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // starting y
	UI16 gImage		= static_cast<UI16>( JSVAL_TO_INT( argv[2] )); // GumpId
	SI16 spriteX	= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // spriteX
	SI16 spriteY	= static_cast<SI16>( JSVAL_TO_INT( argv[4] )); // spriteY
	SI16 width		= static_cast<SI16>( JSVAL_TO_INT( argv[5] )); // width
	SI16 height		= static_cast<SI16>( JSVAL_TO_INT( argv[6] )); // height

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddPicInPic: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "picinpic %i %i %i %i %i %i %i", x, y, gImage, spriteX, spriteY, width, height ));

	return JS_TRUE;
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
JSBool CGump_AddItemProperty( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddItemProperty: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *trgObj = static_cast<CBaseObject *>( JS_GetPrivate( cx, tObj ));

	if( !ValidateObject( trgObj ) || ( trgObj->GetSerial() == INVALIDSERIAL ))
	{
		MethodError( "SetCont: Invalid Object/Argument, takes 1 arg: item" );
		return JS_FALSE;
	}

	SERIAL trgSer = trgObj->GetSerial();

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddItemProperty: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "itemproperty %i", trgSer ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddRadio()
//|	Prototype	-	void AddRadio( topHeight, topLeft, radioImage, pressed, id )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a radio button gump to gumps stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddRadio( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 5 || argc > 6 )
	{
		MethodError( "Gump_AddRadio: Invalid number of arguments (takes 5 or 6)" );
		return JS_FALSE;
	}

	SI16 tL = 0;
	SI16 tR = 0;
	UI16 gImage = 0;
	UI16 gImageChk = 0;
	SI16 initialState = 0;
	SI16 relay = 0;

	if( argc == 5 )
	{
		tL				= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		tR				= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
		gImage			= static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
		gImageChk		= gImage + 1;
		initialState	= static_cast<SI16>( JSVAL_TO_INT( argv[3] ));
		relay			= static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
	}
	else
	{
		tL				= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		tR				= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
		gImage			= static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
		gImageChk		= static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
		initialState	= static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
		relay			= static_cast<SI16>( JSVAL_TO_INT( argv[5] ));
	}


	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddRadio: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "radio %i %i %i %i %i %i", tL, tR, gImage, gImageChk, initialState, relay ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddText()
//|	Prototype	-	void AddText( topHeight, topLeft, textColour, text )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a text gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddText: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	UI32 textId;

	SI16 TextX		= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 TextY		= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 TextHue	= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // Hue

	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[3] ));

	if( TextString == nullptr )
	{
		MethodError( "AddText: Text is required" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddText: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	textId = gList->textId;
	++gList->textId;

	gList->one->push_back( oldstrutil::format( "text %i %i %i %u", TextX, TextY, TextHue, textId ));
	gList->two->push_back( TextString );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddTextEntry()
//|	Prototype	-	void AddTextEntry( topHeight, topLeft, width, height, hue, unk4, textID, defaultText )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a text entry gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddTextEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		MethodError( "AddTextEntry: Invalid number of arguments (takes 8)" );
		return JS_FALSE;
	}

	SI16 tL					= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR					= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	SI16 width				= static_cast<SI16>( JSVAL_TO_INT( argv[2] ));
	SI16 height				= static_cast<SI16>( JSVAL_TO_INT( argv[3] ));
	SI16 hue				= static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
	SI16 relay				= static_cast<SI16>( JSVAL_TO_INT( argv[5] ));
	SI16 initialTextIndex	= static_cast<SI16>( JSVAL_TO_INT( argv[6] ));
	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[7] ));

	if( test == nullptr )
	{
		MethodError( "AddTextEntry: Text is required" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddTextEntry: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "textentry %i %i %i %i %i %i %i", tL, tR, width, height, hue, relay, initialTextIndex ));
	gList->two->push_back( test );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddTextEntryLimited()
//|	Prototype	-	void AddTextEntryLimited( topHeight, topLeft, width, height, hue, unk4, textID, defaultText, maxLength )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a text entry gump with maximum length to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddTextEntryLimited( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 9 )
	{
		MethodError( "AddTextEntryLimited: Invalid number of arguments (takes 9)" );
		return JS_FALSE;
	}

	SI16 tL					= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 tR					= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	SI16 width				= static_cast<SI16>( JSVAL_TO_INT( argv[2] ));
	SI16 height				= static_cast<SI16>( JSVAL_TO_INT( argv[3] ));
	SI16 hue				= static_cast<SI16>( JSVAL_TO_INT( argv[4] ));
	SI16 relay				= static_cast<SI16>( JSVAL_TO_INT( argv[5] ));
	SI16 initialTextIndex	= static_cast<SI16>( JSVAL_TO_INT( argv[6] ));
	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[7] ));
	SI16 textEntrySize		= static_cast<SI16>( JSVAL_TO_INT( argv[8] ));

	if( test == nullptr )
	{
		MethodError( "AddTextEntryLimited: Text is required" );
		return JS_FALSE;
	}

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddTextEntryLimited: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "textentrylimited %i %i %i %i %i %i %i %i", tL, tR, width, height, hue, relay, initialTextIndex, textEntrySize ));
	gList->two->push_back( test );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddTiledGump()
//|	Prototype	-	void AddTiledGump( left, top, width, height, gumpId )
//|	Date		-	14th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds tiled gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddTiledGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "AddTiledGump: Invalid number of arguments (takes 5 x y width height gump)" );
		return JS_FALSE;
	}

	SI16 x		= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 y		= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 width	= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // width
	SI16 height	= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // height
	SI16 gumpId = static_cast<SI16>( JSVAL_TO_INT( argv[4] )); // gump to tile

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddTiledGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( oldstrutil::format( "gumppictiled %i %i %i %i %i", x, y, width, height, gumpId ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddXMFHTMLGump()
//|	Prototype	-	void AddXMFHTMLGump( left, top, width, height, number, hasBorder, hasScrollbar )
//|	Date		-	16th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new XMFHTML gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddXMFHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddXMFHTMLGump: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x				= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 y				= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 width			= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // width
	SI16 height			= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // height
	SI32 number			= static_cast<SI32>( JSVAL_TO_INT( argv[4] )); // number
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddXMFHTMLGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= ( hasBorder ? 1 : 0 );
	SI32 iScrl	= ( hasScrollbar ? 1 : 0 );

	gList->one->push_back( oldstrutil::format( "xmfhtmlgump %i %i %i %i %i %i %i", x, y, width, height, number, iBrd, iScrl ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddXMFHTMLGumpColor()
//|	Prototype	-	void AddXMFHTMLGumpColor( left, top, width, height, number, hasBorder, hasScrollbar, rgbColour )
//|	Date		-	16th February, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new cliloc based XMFHTML gump to gump stream
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddXMFHTMLGumpColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		MethodError( "AddXMFHTMLGumpColor: Invalid number of arguments (takes 8)" );
		return JS_FALSE;
	}

	SI16 x				= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 y				= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 width			= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // width
	SI16 height			= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // height
	SI32 number			= static_cast<SI32>( JSVAL_TO_INT( argv[4] )); // number
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );
	SI32 rgbColour		= static_cast<SI32>( JSVAL_TO_INT( argv[7] ));	// colour

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddXMFHTMLGumpColor: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= ( hasBorder ? 1 : 0 );
	SI32 iScrl	= ( hasScrollbar ? 1 : 0 );

	gList->one->push_back( oldstrutil::format( "xmfhtmlgumpcolor %i %i %i %i %i %i %i %i", x, y, width, height, number, iBrd, iScrl, rgbColour ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_AddXMFHTMLTok()
//|	Prototype	-	void AddXMFHTMLGumpColor( left, top, width, height, number, hasBorder, hasScrollbar, rgbColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a new cliloc based XMFHTML gump to gump stream, with optional cliloc arguments
//o------------------------------------------------------------------------------------------------o
JSBool CGump_AddXMFHTMLTok( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 8 )
	{
		MethodError( "AddXMFHTMLTok: Invalid number of arguments (takes at least 8)" );
		return JS_FALSE;
	}

	SI16 x				= static_cast<SI16>( JSVAL_TO_INT( argv[0] )); // x
	SI16 y				= static_cast<SI16>( JSVAL_TO_INT( argv[1] )); // y
	SI16 width			= static_cast<SI16>( JSVAL_TO_INT( argv[2] )); // width
	SI16 height			= static_cast<SI16>( JSVAL_TO_INT( argv[3] )); // height
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	SI32 rgbColour		= static_cast<SI32>( JSVAL_TO_INT( argv[6] ));	// colour
	SI32 number			= static_cast<SI32>( JSVAL_TO_INT( argv[7] )); // number

	char *TextString1	= JS_GetStringBytes( JS_ValueToString( cx, argv[8] )); // ClilocArgument1
	char *TextString2	= JS_GetStringBytes( JS_ValueToString( cx, argv[9] )); // ClilocArgument2
	char *TextString3	= JS_GetStringBytes( JS_ValueToString( cx, argv[10] )); // ClilocArgument3

	SEGump_st *gList = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));
	if( gList == nullptr )
	{
		MethodError( "AddXMFHTMLTok: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= ( hasBorder ? 1 : 0 );
	SI32 iScrl	= ( hasScrollbar ? 1 : 0 );

	gList->one->push_back( oldstrutil::format( "xmfhtmltok %i %i %i %i %i %i %i %i @%s\t%s\t%s@", x, y, width, height, iBrd, iScrl, rgbColour, number, TextString1, TextString2, TextString3 ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump_Send()
//|	Prototype	-	void Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump stream to socket
//o------------------------------------------------------------------------------------------------o
JSBool CGump_Send( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Send: Invalid number of arguments (takes 1, socket or char)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "You have to pass a valid Socket or Character" );
	}

	JSEncapsulate myClass( cx, &( argv[0] ));

	SEGump_st *myGump = static_cast<SEGump_st*>( JS_GetPrivate( cx, obj ));

	if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = static_cast<CSocket *>( myClass.toObject() );
		if( mySock == nullptr )
		{
			MethodError( "Send: Passed an invalid Socket" );
			return JS_FALSE;
		}
		UI32 gumpId = ( 0xFFFF + JSMapping->GetScriptId( JS_GetGlobalObject( cx )));
		SendVecsAsGump( mySock, *( myGump->one ), *( myGump->two ), gumpId, INVALIDSERIAL );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( myChar ))
		{
			MethodError( "Send: Passed an invalid Character" );
			return JS_FALSE;
		}

		CSocket *mySock = myChar->GetSocket();
		UI32 gumpId = ( 0xFFFF + JSMapping->GetScriptId( JS_GetGlobalObject( cx )));
		SendVecsAsGump( mySock, *( myGump->one ), *( myGump->two ), gumpId, INVALIDSERIAL );
	}
	else
	{
		MethodError( "Send: Unknown Object has been passed" );
		return JS_FALSE;
	}

	return JS_TRUE;
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
JSBool CBase_TextMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 7 )
	{
		MethodError( "TextMessage: Invalid number of arguments (takes 1 - 7)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CBaseObject *myObj		= static_cast<CBaseObject*>( myClass.toObject() );

	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );
	if( trgMessage == nullptr )
	{
		MethodError( "You have to supply a message-text" );
		return JS_FALSE;
	}

	UI16 txtHue = 0x0000;
	if( argc >= 3 )
	{
		txtHue = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	}

	SpeechTarget speechTarget = SPTRG_NULL;
	if( argc >= 4 )
	{
		speechTarget = static_cast<SpeechTarget>( JSVAL_TO_INT( argv[3] ));
	}
	if( speechTarget == SPTRG_NULL )
	{
		speechTarget = SPTRG_PCNPC;
	}

	SERIAL speechTargetSerial = INVALIDSERIAL;
	if( argc >= 5 )
	{
		speechTargetSerial = static_cast<SERIAL>( JSVAL_TO_INT( argv[4] ));
	}

	FontType speechFontType = FNT_NULL;
	if( argc == 6 )
	{
		speechFontType = static_cast<FontType>( JSVAL_TO_INT( argv[5] ));
	}

	SpeechType speechType = UNKNOWN;
	if( argc == 7 )
	{
		speechType = static_cast<SpeechType>( JSVAL_TO_INT( argv[6] ));
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = static_cast<CItem *>( myObj );
		if( !ValidateObject( myItem ))
		{
			MethodError( "TextMessage: Invalid Item" );
			return JS_FALSE;
		}
		if( !txtHue )
		{
			txtHue = 0x047F;
		}
		if( speechFontType == FNT_NULL )
		{
			speechFontType = FNT_NORMAL;
		}
		if( speechType == UNKNOWN )
		{
			speechType = OBJ;
		}
		MethodSpeech( *myItem, trgMessage, speechType, txtHue, speechFontType, speechTarget, speechTargetSerial, useUnicode );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar *>( myObj );
		if( !ValidateObject( myChar ))
		{
			MethodError( "TextMessage: Invalid Character" );
			return JS_FALSE;
		}

		if( argc >= 2 && argc <= 3 && JSVAL_TO_BOOLEAN( argv[1] ) != JS_TRUE )
		{
			speechTarget = SPTRG_INDIVIDUAL;
			if( speechTargetSerial == INVALIDSERIAL )
			{
				speechTargetSerial = myChar->GetSerial();
			}
		}

		if( speechFontType == FNT_NULL )
		{
			speechFontType = static_cast<FontType>( myChar->GetFontType() );
		}
		if( speechType == UNKNOWN )
		{
			speechType = SAY;
		}

		if( myChar->GetNpcAiType() == AI_EVIL || myChar->GetNpcAiType() == AI_EVIL_CASTER )
		{
			if( !txtHue )
			{
				txtHue = 0x0026;
			}
			MethodSpeech( *myChar, trgMessage, speechType, txtHue, speechFontType, speechTarget, speechTargetSerial, useUnicode );
		}
		else
		{
			if( !txtHue )
			{
				txtHue = myChar->GetSayColour();
			}
			MethodSpeech( *myChar, trgMessage, speechType, txtHue, speechFontType, speechTarget, speechTargetSerial, useUnicode );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_KillTimers()
//|	Prototype	-	void KillTimers()
//|					void KillTimers( scriptId )
//|	Date		-	04/20/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kill all related timers that have been associated with an item or character
//o------------------------------------------------------------------------------------------------o
JSBool CBase_KillTimers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc > 1 )
	{
		MethodError( "KillTimers: Invalid count of arguments :%d, needs :0 or 1", argc );
		return JS_FALSE;
	}
	auto myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( myObj == nullptr )
	{
		MethodError( "KillTimers: Invalid object assigned." );
		return JS_FALSE;
	}
	SI32 triggerNum = -1;
	if( argc == 1 )
	{
		triggerNum = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	}

	SERIAL mySer = myObj->GetSerial();
	std::vector<CTEffect *> removeEffect;

	for( const auto &Effect : cwmWorldState->tempEffects.collection() )
	{
		if( triggerNum != -1 )
		{
			if( mySer == Effect->Destination() && Effect->More1() == triggerNum )
			{
				removeEffect.push_back( Effect );
			}
		}
		else if( mySer == Effect->Destination() )
		{
			removeEffect.push_back( Effect );
		}
	}
	for( const auto &Effect :removeEffect )
	{
		cwmWorldState->tempEffects.Remove( Effect, true );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetJSTimer()
//|	Prototype	-	void GetJSTimer( timerId, scriptId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get JS timer with specific timerId association with an item or character, and return time left
//o------------------------------------------------------------------------------------------------o
JSBool CBase_GetJSTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc != 2 )
	{
		MethodError( "GetJSTimer: Invalid count of arguments :%d, needs 2 (timerId, scriptId)", argc );
		return JS_FALSE;
	}

	auto myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( myObj == nullptr )
	{
		MethodError( "GetJSTimer: Invalid object assigned." );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( 0 ); // Return value 0 by default, to indicate no valid timer found
	UI16 timerId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[1] ));

	SERIAL myObjSerial = myObj->GetSerial();
	for( const auto &Effect : cwmWorldState->tempEffects.collection() )
	{
		// We only want results that have same object serial and timerId as specified
		if( myObjSerial == Effect->Destination() && Effect->More1() == timerId )
		{
			// Check for a valid script associated with Effect
			cScript *tScript	= JSMapping->GetScript( Effect->AssocScript() );
			if( tScript == nullptr && Effect->More2() != 0xFFFF )
			{
				// If no default script was associated with effect, check if another script was stored in More2
				tScript = JSMapping->GetScript( Effect->More2() );
			}

			// If a valid script is associated with Effect, and the Effect's scriptId matches the provided scriptId...
			if( tScript != nullptr && ( scriptId == Effect->AssocScript() || scriptId == Effect->More2() ))
			{
				// Return the timestamp for when the Effect timer expires
				//*rval = INT_TO_JSVAL( Effect->ExpireTime() );
				JS_NewNumberValue( cx, Effect->ExpireTime(), rval );
			}
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_SetJSTimer()
//|	Prototype	-	void SetJSTimer( timerId, timeInMilliseconds, scriptId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set expiration time for JS timer with specific timerId association with an item or character
//o------------------------------------------------------------------------------------------------o
JSBool CBase_SetJSTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc != 3 )
	{
		MethodError( "SetJSTimer: Invalid count of arguments :%d, needs 3 (timerId, timeInMilliseconds, scriptId)", argc );
		return JS_FALSE;
	}

	auto myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( myObj == nullptr )
	{
		MethodError( "SetJSTimer: Invalid object assigned." );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( 0 ); // Return value is 0 by default, indicating no timer was found or updated
	UI16 timerId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	UI32 expireTime = BuildTimeValue( JSVAL_TO_INT( argv[1] ) / 1000.0f );
	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));

	SERIAL myObjSerial = myObj->GetSerial();
	for( const auto &Effect : cwmWorldState->tempEffects.collection() )
	{
		// We only want to modify the Effect that have same object serial and timerId as specified
		if( myObjSerial == Effect->Destination() && Effect->More1() == timerId )
		{
			// Check for a valid script associated with Effect
			cScript *tScript = JSMapping->GetScript( Effect->AssocScript() );
			if( tScript == nullptr && Effect->More2() != 0xFFFF )
			{
				// If no default script was associated with effect, check if another script was stored in More2
				tScript = JSMapping->GetScript( Effect->More2() );
			}

			// If a valid script is associated with Effect, and the Effect's scriptId matches the provided scriptId...
			if( tScript != nullptr && ( scriptId == Effect->AssocScript() || scriptId == Effect->More2() ))
			{
				// Set the timestamp for when the Effect timer expires to that specified in parameters
				Effect->ExpireTime( expireTime );
				*rval = INT_TO_JSVAL( 1 ); // Return 1 indicating timer was found and updated
			}
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_KillJSTimer()
//|	Prototype	-	void KillJSTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kill JS timer on item or character based on specified scriptId and timerId
//o------------------------------------------------------------------------------------------------o
JSBool CBase_KillJSTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc != 2 )
	{
		MethodError( "KillJSTimer: Invalid count of arguments :%d, needs 2 (timerId, scriptId)", argc );
		return JS_FALSE;
	}
	auto myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( myObj == nullptr )
	{
		MethodError( "KillJSTimer: Invalid object assigned." );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( 0 ); // Return value 0 by default, to indicate no valid timer found
	UI16 timerId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[1] ));

	SERIAL myObjSerial = myObj->GetSerial();
	CTEffect *removeEffect = nullptr;

	for( auto &Effect : cwmWorldState->tempEffects.collection() )
	{
		if( myObjSerial == Effect->Destination() && Effect->More1() == timerId )
		{
			// Check for a valid script associated with Effect
			cScript *tScript = JSMapping->GetScript( Effect->AssocScript() );
			if( tScript == nullptr && Effect->More2() != 0xFFFF )
			{
				// If no default script was associated with effect, check if another script was stored in More2
				tScript = JSMapping->GetScript( Effect->More2() );
			}

			// If a valid script is associated with Effect, and the Effect's scriptId matches the provided scriptId...
			if( tScript != nullptr && ( scriptId == Effect->AssocScript() || scriptId == Effect->More2() ))
			{
				// Found our timer! Keep track of it for removal outside loop
				removeEffect = Effect;
				break;
			}
		}
	}

	if( removeEffect != nullptr )
	{
		cwmWorldState->tempEffects.Remove( removeEffect, true );
		*rval = INT_TO_JSVAL( 1 ); // Return 1 indicating timer was found and removed
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_Delete()
//|	Prototype	-	void Delete()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes object
//o------------------------------------------------------------------------------------------------o
JSBool CBase_Delete( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "Delete: Invalid object" );
		return JS_FALSE;
	}

	myObj->Delete();

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Wander()
//|	Prototype	-	void Wander( x1, y1, x2, y2 )
//|					void Wander( x1, y1, radius )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Specifies a wander area for an NPC, as either a box or a circle
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Wander( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		MethodError( "Wander: Invalid number of arguments (takes 3-4, coordinates)" );
		return JS_FALSE;
	}

	SI16 x1 = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
	SI16 y1 = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	SI16 x2 = static_cast<SI16>( JSVAL_TO_INT( argv[2] ));
	SI16 y2 = 0;
	if( argc > 3 )
	{
		y2 = static_cast<SI16>( JSVAL_TO_INT( argv[3] ));
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ) || !myChar->IsNpc() )
	{
		MethodError( "WanderBox: Invalid character" );
		return JS_FALSE;
	}
	myChar->SetFx( x1, 0 );
	myChar->SetFy( y1, 0 );
	myChar->SetFz( -1 );
	myChar->SetFx( x2, 1 );
	if( argc > 3 )
	{
		myChar->SetFy( y2, 1 );
		myChar->SetNpcWander( WT_BOX );
	}
	else
	{
		myChar->SetNpcWander( WT_CIRCLE );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Follow()
//|	Prototype	-	void Follow( followTarget )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces NPC to follow specified target
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Follow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Follow: Invalid number of arguments (takes 1, char object or null)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myChar ) || !myChar->IsNpc() )
	{
		MethodError( "Follow: Invalid NPC character object referenced!" );
		return JS_FALSE;
	}

	if( argv[0] == JSVAL_NULL )
	{
		// Clear follow target if null was provided instead of a character object to follow
		myChar->SetFTarg( nullptr );
		myChar->SetNpcWander( WT_NONE );
		return JS_TRUE;
	}

	JSObject *jsObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *myObj = static_cast<CBaseObject *>( JS_GetPrivate( cx, jsObj ));

	if( !ValidateObject( myObj ) || myObj->GetSerial() >= BASEITEMSERIAL )
	{
		MethodError( "Follow: Invalid character parameter provided!" );
		return JS_FALSE;
	}

	myChar->SetFTarg( static_cast<CChar*>( myObj ));
	myChar->FlushPath();
	myChar->SetNpcWander( WT_FOLLOW );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_DoAction()
//|	Prototype	-	void DoAction( trgAction )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes character do specified action
//o------------------------------------------------------------------------------------------------o
JSBool CChar_DoAction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 5 )
	{
		MethodError( "DoAction: Invalid number of arguments (takes 1 - (actionID), 2 (actionID, subActionID), 3 (actionID, null, frameCount), 4 (actionID, null, frameCount, frameDelay ) or 5 (actionID, null, frameCount, frameDelay, playBackwards)" );
		return JS_FALSE;
	}

	UI16 targAction = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	SI16 targSubAction = -1;
	UI16 frameCount = 7;
	UI16 frameDelay = 0;
	bool playBackwards = false;
	if( argc > 1 )
	{
		if( argv[1] != JSVAL_NULL )
		{
			targSubAction = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
		}
	}
	if( argc > 2 )
	{
		frameCount = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	}
	if( argc > 3 )
	{
		frameDelay = static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
	}
	if( argc > 4 )
	{
		playBackwards = JSVAL_TO_BOOLEAN( argv[4] );
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "Action: Invalid character" );
		return JS_FALSE;
	}

	// Reset idle anim timer so it doesn't interrupt the DoAction anim
	myChar->SetTimer( tNPC_IDLEANIMTIME, BuildTimeValue( RandomNum( 5, 20 )));

	// Play the requested animation
	if( myChar->GetBodyType() == BT_GARGOYLE || targSubAction != -1 )
	{
		Effects->PlayNewCharacterAnimation( myChar, targAction, static_cast<UI16>( targSubAction ));
	}
	else
	{
		Effects->PlayCharacterAnimation( myChar, targAction, frameDelay, frameCount, playBackwards );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_EmoteMessage()
//|	Prototype	-	void EmoteMessage( message )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes character to emote specified text
//o------------------------------------------------------------------------------------------------o
JSBool CChar_EmoteMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 5 )
	{
		MethodError( "EmoteMessage: Invalid number of arguments (takes 1 - 5: text, allSay, hue, speech target and speech serial)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	JSString *targMessage = JS_ValueToString( cx, argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );

	if( !ValidateObject( myChar ) || trgMessage == nullptr )
	{
		MethodError( "EmoteMessage: Invalid character or speech" );
		return JS_FALSE;
	}

	UI16 txtHue = 0x0000;
	if( argc >= 3 )
	{
		txtHue = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	}
	if( txtHue == 0x0000 )
	{
		txtHue = myChar->GetEmoteColour();
	}

	SpeechTarget speechTarget = SPTRG_NULL;
	if( argc >= 4 )
	{
		speechTarget = static_cast<SpeechTarget>( JSVAL_TO_INT( argv[3] ));
	}
	if( speechTarget == SPTRG_NULL )
	{
		speechTarget = SPTRG_PCNPC;
	}

	SERIAL speechTargetSerial = INVALIDSERIAL;
	if( argc >= 5 )
	{
		speechTargetSerial = static_cast<SERIAL>( JSVAL_TO_INT( argv[4] ));
	}

	if( argc >= 2 && argc <= 3 && JSVAL_TO_BOOLEAN( argv[1] ) != JS_TRUE )
	{
		speechTarget = SPTRG_INDIVIDUAL;
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	MethodSpeech( *myChar, trgMessage, EMOTE, txtHue, static_cast<FontType>( myChar->GetFontType() ), speechTarget, speechTargetSerial, useUnicode );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Dismount()
//|	Prototype	-	void Dismount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dismounts character, if mounted
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Dismount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Dismount: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "Dismount: Invalid character" );
		return JS_FALSE;
	}
	if( myChar->IsOnHorse() )
	{
		DismountCreature( myChar );
	}
	else if( myChar->IsFlying() )
	{
		myChar->ToggleFlying();
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SysMessage()
//|	Prototype	-	void SysMessage( message )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a system message to the player
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 0 || argc > 11 )
	{
		MethodError( "SysMessage: Invalid number of arguments (takes at least 1, and at most 11)" );
		return JS_FALSE;
	}

	CSocket *mySock = nullptr;
	JSEncapsulate myClass( cx, obj );

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );
		mySock = myChar->GetSocket();
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		mySock = static_cast<CSocket*>( myClass.toObject() );
	}

	if( mySock == nullptr )
	{
		MethodError( "SysMessage: invalid socket" );
		return JS_FALSE;
	}

	UI16 msgColor = 0;
	UI08 argCounter = 0;
	if( argc > 1 && JSVAL_IS_INT( argv[0] ))
	{
		msgColor = static_cast<UI16>( JSVAL_TO_INT( argv[argCounter++] ));
	}

	JSString *targMessage = JS_ValueToString( cx, argv[argCounter++] );
	char *trgMessage = JS_GetStringBytes( targMessage );

	if( trgMessage == nullptr )
	{
		MethodError( "SysMessage: Invalid speech (%s)", targMessage );
		return JS_FALSE;
	}

	std::string msgArg;
	for( UI32 i = argCounter; i < argc; i++ )
	{
		if( msgArg.empty() )
		{
			msgArg += JS_GetStringBytes( JS_ValueToString( cx, argv[i] ));
		}
		else
		{
			msgArg += std::string(",") + JS_GetStringBytes( JS_ValueToString( cx, argv[i] ));
		}
	}

	mySock->SysMessageJS( trgMessage, msgColor, msgArg );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Disconnect()
//|	Prototype	-	void Disconnect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disconnects specified client
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_Disconnect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Disconnect: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CSocket *targSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( targSock == nullptr )
	{
		MethodError( "SysMessage: Invalid socket" );
		return JS_FALSE;
	}

	Network->Disconnect( targSock );
	JS_SetPrivate( cx, obj, nullptr ); // yes we should do that...

	return JS_TRUE;
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
JSBool CBase_Teleport( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSEncapsulate myClass( cx, obj );

	CBaseObject *myObj	= static_cast<CBaseObject*>( myClass.toObject() );
	SI16 x				= -1;
	SI16 y				= -1;
	SI08 z				= myObj->GetZ();
	UI08 world			= myObj->WorldNumber();
	UI16 instanceId		= myObj->GetInstanceId();

	switch( argc )
	{
			// Just Teleport...
		case 0:
			if( myClass.ClassName() == "UOXChar" )
			{
				( static_cast<CChar*>( myObj ))->Teleport();
				return JS_TRUE;
			}
			MethodError( "For Items you need at least one parameter for Teleport" );
			break;

			// Parameters as a string
		case 1:
			if( JSVAL_IS_OBJECT( argv[0] ))
			{	// we can work with this, it should be either a character or item, hopefully
				JSEncapsulate jsToGoTo( cx, &( argv[0] ));
				if( jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar" )
				{
					CBaseObject *toGoTo = static_cast<CBaseObject *>( jsToGoTo.toObject() );
					if( !ValidateObject( toGoTo ))
					{
						MethodError( "No object associated with this object" );
						break;
					}

					x		= toGoTo->GetX();
					y		= toGoTo->GetY();
					z		= toGoTo->GetZ();
					world	= toGoTo->WorldNumber();
					instanceId = toGoTo->GetInstanceId();
				}
				else if( jsToGoTo.ClassName() == "UOXSocket" )
				{
					CSocket *mySock		= static_cast<CSocket *>( jsToGoTo.toObject() );
					CChar *mySockChar	= mySock->CurrcharObj();
					x					= mySockChar->GetX();
					y					= mySockChar->GetY();
					z					= mySockChar->GetZ();
					world				= mySockChar->WorldNumber();
					instanceId			= mySockChar->GetInstanceId();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
			}
			else if( JSVAL_IS_INT( argv[0] ))
			{
				UI16 placeNum = JSVAL_TO_INT( argv[0] );
				if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
				{
					GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
					x		= toGoTo.x;
					y		= toGoTo.y;
					z		= toGoTo.z;
					world	= toGoTo.worldNum;
				}
			}
			else	// Needs to be implemented
			{
				MethodError( "Text-styled Parameters may be added later" );
			}
			break;

			// 2 Parameters, x + y
		case 2:
			x = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
			y = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
			break;

			// x,y,z
		case 3:
			x = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
			y = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
			z = static_cast<SI08>( JSVAL_TO_INT( argv[2] ));
			break;

			// x,y,z,world
		case 4:
			x		= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
			y		= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
			z		= static_cast<SI08>( JSVAL_TO_INT( argv[2] ));
			world	= static_cast<UI08>( JSVAL_TO_INT( argv[3] ));
			break;

			// x,y,z,world,instanceId
		case 5:
			x = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
			y = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
			z = static_cast<SI08>( JSVAL_TO_INT( argv[2] ));
			world = static_cast<UI08>( JSVAL_TO_INT( argv[3] ));
			instanceId = static_cast<UI16>( JSVAL_TO_INT( argv[4] ));
			break;

		default:
			MethodError( "Invalid number of arguments passed to Teleport, needs either 1, 2, 3, 4 or 5" );
			break;
	}

	if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = static_cast<CItem*>( myObj );
		if( !ValidateObject( myItem ))
		{
			MethodError( "Teleport: Invalid Item" );
			return JS_FALSE;
		}

		// Update old location of item, which will be used by RemoveFromSight()
		myItem->SetOldLocation( myItem->GetX(), myItem->GetY(), myItem->GetZ() );
		myItem->RemoveFromSight();
		myItem->SetLocation( x, y, z, world, instanceId );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myObj );
		if( !ValidateObject( myChar ))
		{
			MethodError( "Teleport: Invalid Character" );
			return JS_FALSE;
		}

		if(( world != myChar->WorldNumber() || instanceId != myChar->GetInstanceId() ) && !myChar->IsNpc() )
		{
			CSocket *mySock = myChar->GetSocket();
			if( mySock == nullptr )
				return JS_TRUE;

			if( !Map->InsideValidWorld( x, y, world) )
			{
				MethodError( "Teleport: Not a valid World" );
				return JS_FALSE;
			}
			if( myChar->GetInstanceId() != instanceId )
			{
				// Remove all objects in visible range from sight
				myChar->RemoveAllObjectsFromSight( mySock );
			}

			// Remove character being teleported from nearby players sight
			myChar->RemoveFromSight();
			myChar->SetLocation( x, y, z, world, instanceId );
			SendMapChange( world, mySock );

			// Extra update needed for regular UO client
			myChar->Update();
		}
		else
		{
			if( myChar->GetInstanceId() != instanceId)
			{
				myChar->RemoveFromSight();
			}
			myChar->SetLocation( x, y, z, world, instanceId );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_StaticEffect()
//|	Prototype	-	void StaticEffect( Effect, speed, loop )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a static effect for character
//o------------------------------------------------------------------------------------------------o
JSBool CBase_StaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI16 effectId		= static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	UI08 speed			= static_cast<UI08>( JSVAL_TO_INT( argv[1] ));
	UI08 loop			= static_cast<UI08>( JSVAL_TO_INT( argv[2] ));

	JSEncapsulate		myClass( cx, obj );
	CBaseObject *myObj	= static_cast<CBaseObject*>( myClass.toObject() );

	if( !ValidateObject( myObj ))
	{
		MethodError( "StaticEffect: Invalid Object" );
		return JS_FALSE;
	}

	if( myClass.ClassName() == "UOXItem" )
	{
		bool explode = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );
		Effects->PlayStaticAnimation( myObj, effectId, speed, loop, explode );
	}
	else
	{
		Effects->PlayStaticAnimation( myObj, effectId, speed, loop );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_MakeMenu()
//|	Prototype	-	void MakeMenu( mnu )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends specified make menu to player
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_MakeMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "MakeMenu: Invalid number of arguments (takes 2, number of menu, skill used)" );
		return JS_FALSE;
	}

	CSocket *mySock		= nullptr;
	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar	= static_cast<CChar*>( myClass.toObject() );
		mySock			= myChar->GetSocket();
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		mySock			= static_cast<CSocket*>( myClass.toObject() );
	}

	if( mySock == nullptr )
	{
		MethodError( "MakeMenu: invalid socket" );
		return JS_FALSE;
	}
	SI32 menu		= JSVAL_TO_INT( argv[0] );
	UI08 skillNum	= static_cast<UI08>( JSVAL_TO_INT( argv[1] ));
	Skills->NewMakeMenu( mySock, menu, skillNum );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SoundEffect()
//|	Prototype	-	void SoundEffect( soundId, bAllHear )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a sound effect at object's location
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_SoundEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SoundEffect: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );

	UI16 soundId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	bool allHear = ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" )
	{
		CBaseObject *myObj = static_cast<CBaseObject*>( myClass.toObject() );

		if( ValidateObject( myObj ))
		{
			Effects->PlaySound( myObj, soundId, allHear );
		}
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = static_cast<CSocket*>( myClass.toObject() );

		if( mySock != nullptr )
		{
			Effects->PlaySound( mySock, soundId, allHear );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SellTo()
//|	Prototype	-	bool SellTo( vendorNPC )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up the shopkeeper gump for selling to specified vendor NPC
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_SellTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = JSVAL_FALSE;
	if( argc != 1 )
	{
		MethodError( "SellTo: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *myNPC = static_cast<CChar*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( myNPC ))
	{
		MethodError( "SellTo: Invalid NPC" );
		return JS_FALSE;
	}

	CPSellList toSend;
	if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = static_cast<CSocket*>( myClass.toObject() );
		if( mySock == nullptr )
		{
			MethodError( "Passed an invalid socket to SellTo" );
			return JS_FALSE;
		}

		CChar *mChar = mySock->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			myNPC->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60.0f ));
			if( toSend.CanSellItems(( *mChar ), ( *myNPC )))
			{
				mySock->Send( &toSend );
				*rval = JSVAL_TRUE;
			}
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( myChar ))
		{
			MethodError( "Passed an invalid char to SellTo" );
			return JS_FALSE;
		}

		myNPC->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60.0f ));
		CSocket *mSock = myChar->GetSocket();
		if( toSend.CanSellItems(( *myChar ), ( *myNPC )))
		{
			mSock->Send( &toSend );
			*rval = JSVAL_TRUE;
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_BuyFrom()
//|	Prototype	-	void BuyFrom( vendorNPC )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up the shopkeeper gump for buying from specified vendor NPC
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_BuyFrom( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SellTo: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *myNPC = static_cast<CChar*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( myNPC ))
	{
		MethodError( "BuyFrom: Invalid NPC" );
		return JS_FALSE;
	}

	if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = static_cast<CSocket *>( myClass.toObject() );
		if( mySock == nullptr )
		{
			MethodError( "Invalid source socket in BuyFrom" );
			return JS_FALSE;
		}

		if( myNPC->GetNpcAiType() == AI_PLAYERVENDOR )
		{
			mySock->TempObj( myNPC );
			myNPC->TextMessage( mySock, 772, TALK, false );
			mySock->SendTargetCursor( 0, TARGET_PLVBUY, " " );
		}
		else
		{
			BuyShop( mySock, myNPC );
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( myChar ))
		{
			MethodError( "Passed an invalid char to BuyFrom" );
			return JS_FALSE;
		}

		CSocket *mySock = myChar->GetSocket();

		if( myNPC->GetNpcAiType() == AI_PLAYERVENDOR )
		{
			mySock->TempObj( myNPC );
			myNPC->TextMessage( mySock, 772, TALK, false );
			mySock->SendTargetCursor( 0, TARGET_PLVBUY, " " );
		}
		else
		{
			BuyShop( mySock, myNPC );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_HasSpell()
//|	Prototype	-	bool HasSpell( spellNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for specified spell in first spellbook found in player's inventory
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_HasSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "HasSpell: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	UI08 spellId = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( myChar ))
		{
			MethodError( "Invalid char for HasSpell" );
			return JS_FALSE;
		}

		CItem *myItem = FindItemOfType( myChar, IT_SPELLBOOK );

		if( !ValidateObject( myItem ))
		{
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
			return JS_TRUE;
		}

		if( Magic->HasSpell( myItem, spellId ))
		{
			*rval = BOOLEAN_TO_JSVAL( JS_TRUE );
		}
		else
		{
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
		}
	}
	else if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = static_cast<CItem*>( myClass.toObject() );
		if( !ValidateObject( myItem ))
		{
			MethodError( "Invalid item for HasSpell" );
			return JS_FALSE;
		}

		if( Magic->HasSpell( myItem, spellId ))
		{
			*rval = BOOLEAN_TO_JSVAL( JS_TRUE );
		}
		else
		{
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_RemoveSpell()
//|	Prototype	-	void RemoveSpell( spellNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specified spell from first spellbook found in player's inventory
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_RemoveSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveSpell: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	UI08 spellId = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( myChar ))
		{
			MethodError( "Invalid char for HasSpell" );
			return JS_FALSE;
		}

		CItem *myItem = FindItemOfType( myChar, IT_SPELLBOOK );

		if( ValidateObject( myItem ))
		{
			Magic->RemoveSpell( myItem, spellId );
		}
	}
	else if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = static_cast<CItem*>( myClass.toObject() );
		if( !ValidateObject( myItem ))
		{
			MethodError( "Invalid item for RemoveSpell" );
			return JS_FALSE;
		}

		Magic->RemoveSpell( myItem, spellId );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetTag()
//|	Prototype	-	tagdata GetTag( "TagName" )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns value for tag with specified name, if tag has been stored on the object
//o------------------------------------------------------------------------------------------------o
JSBool CBase_GetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "GetTag: Invalid Count of Parameters: %d, need: 1", argc );
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "GetTag: Invalid Object assigned" );
		return JS_FALSE;
	}

	std::string localString		= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	TAGMAPOBJECT localObject	= myObj->GetTag( localString );
	if( localObject.m_ObjectType == TAGMAP_TYPE_STRING )
	{
		JSString *localJSString = JS_NewStringCopyN( cx, ( const char* )localObject.m_StringValue.c_str(), localObject.m_StringValue.length() );
		*rval = static_cast<jsval>( STRING_TO_JSVAL( localJSString ));
	}
	else if( localObject.m_ObjectType == TAGMAP_TYPE_BOOL )
	{
		*rval = static_cast<jsval>( BOOLEAN_TO_JSVAL(( localObject.m_IntValue == 1 )));
	}
	else
	{
		*rval = static_cast<jsval>( INT_TO_JSVAL( localObject.m_IntValue ));
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_SetTag()
//|	Prototype	-	void SetTag( "TagName", "TagStringValue" )
//|					void SetTag( "TagName", TagIntValue )
//|					void SetTag( "TagName", TagBoolValue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Stores persistent tag with specified name and value on object
//o------------------------------------------------------------------------------------------------o
JSBool CBase_SetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc != 2 ) && ( argc != 1 ))
	{
		MethodError( "SetTag: Invalid Count of Parameters: %d, need: 2", argc );
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "SetTag: Invalid Object assigned (SetTag)" );
		return JS_FALSE;
	}

	std::string localString = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	TAGMAPOBJECT localObject;
	if( argc == 2 )
	{
		JSEncapsulate encaps( cx, &( argv[1] ));
		if( encaps.isType( JSOT_STRING ))
		{			// String value handling
			const std::string stringVal = encaps.toString();
			if( stringVal == "" )
			{
				localObject.m_Destroy		= true;
				localObject.m_IntValue		= 0;
				localObject.m_ObjectType	= TAGMAP_TYPE_INT;
				localObject.m_StringValue	= "";
			}
			else
			{
				localObject.m_Destroy		= false;
				localObject.m_StringValue	= stringVal;
				localObject.m_IntValue		= static_cast<SI32>( localObject.m_StringValue.length() );
				localObject.m_ObjectType	= TAGMAP_TYPE_STRING;
			}
		}
		else if( encaps.isType( JSOT_BOOL ))
		{
			const bool boolVal = encaps.toBool();
			if( !boolVal )
			{
				localObject.m_Destroy		= true;
				localObject.m_IntValue		= 0;
			}
			else
			{
				localObject.m_Destroy		= false;
				localObject.m_IntValue		= 1;
			}
			localObject.m_ObjectType	= TAGMAP_TYPE_BOOL;
			localObject.m_StringValue	= "";
		}
		else if( encaps.isType( JSOT_INT ))
		{
			const SI32 intVal = encaps.toInt();
			if( !intVal )
			{
				localObject.m_Destroy		= true;
				localObject.m_IntValue		= 0;
			}
			else
			{
				localObject.m_Destroy		= false;
				localObject.m_IntValue		= intVal;
			}
			localObject.m_ObjectType	= TAGMAP_TYPE_INT;
			localObject.m_StringValue	= "";
		}
		else if( encaps.isType( JSOT_NULL ))
		{
			localObject.m_Destroy		= true;
			localObject.m_IntValue		= 0;
			localObject.m_ObjectType	= TAGMAP_TYPE_INT;
			localObject.m_StringValue	= "";
		}
		else
		{
			return JS_TRUE;
		}
		myObj->SetTag( localString, localObject );
	}
	else
	{
		localObject.m_Destroy		= true;
		localObject.m_ObjectType	= TAGMAP_TYPE_INT;
		localObject.m_IntValue		= 0;
		localObject.m_StringValue	= "";
		myObj->SetTag( localString, localObject );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetTempTag()
//|	Prototype	-	tagdata GetTempTag( "TagName" )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns value for temporary tag with specified name, if tag has been stored on the object
//o------------------------------------------------------------------------------------------------o
JSBool CBase_GetTempTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "GetTempTag: Invalid Count of Parameters: %d, need: 1", argc );
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "GetTempTag: Invalid Object assigned" );
		return JS_FALSE;
	}

	std::string localString		= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	TAGMAPOBJECT localObject	= myObj->GetTempTag( localString );
	if( localObject.m_ObjectType == TAGMAP_TYPE_STRING )
	{
		JSString *localJSString = JS_NewStringCopyN( cx, ( const char* )localObject.m_StringValue.c_str(), localObject.m_StringValue.length() );
		*rval = static_cast<jsval>( STRING_TO_JSVAL( localJSString ));
	}
	else if( localObject.m_ObjectType == TAGMAP_TYPE_BOOL )
	{
		*rval = static_cast<jsval>( BOOLEAN_TO_JSVAL(( localObject.m_IntValue == 1 )));
	}
	else
	{
		*rval = static_cast<jsval>( INT_TO_JSVAL( localObject.m_IntValue ));
	}
	return JS_TRUE;
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
JSBool CBase_SetTempTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc != 2 ) && ( argc != 1 ))
	{
		MethodError( "SetTempTag: Invalid Count of Parameters: %d, need: 2", argc );
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "SetTempTag: Invalid Object assigned" );
		return JS_FALSE;
	}

	std::string localString = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	TAGMAPOBJECT localObject;
	if( argc == 2 )
	{
		JSEncapsulate encaps( cx, &( argv[1] ));
		if( encaps.isType( JSOT_STRING ))
		{			// String value handling
			const std::string stringVal = encaps.toString();
			if( stringVal == "" )
			{
				localObject.m_Destroy		= true;
				localObject.m_IntValue		= 0;
				localObject.m_ObjectType	= TAGMAP_TYPE_INT;
				localObject.m_StringValue	= "";
			}
			else
			{
				localObject.m_Destroy		= false;
				localObject.m_StringValue	= stringVal;
				localObject.m_IntValue		= static_cast<SI32>( localObject.m_StringValue.length() );
				localObject.m_ObjectType	= TAGMAP_TYPE_STRING;
			}
		}
		else if( encaps.isType( JSOT_BOOL ))
		{
			const bool boolVal = encaps.toBool();
			if( !boolVal )
			{
				localObject.m_Destroy		= true;
				localObject.m_IntValue		= 0;
			}
			else
			{
				localObject.m_Destroy		= false;
				localObject.m_IntValue		= 1;
			}
			localObject.m_ObjectType	= TAGMAP_TYPE_BOOL;
			localObject.m_StringValue	= "";
		}
		else if( encaps.isType( JSOT_INT ))
		{
			const SI32 intVal = encaps.toInt();
			if( !intVal )
			{
				localObject.m_Destroy		= true;
				localObject.m_IntValue		= 0;
			}
			else
			{
				localObject.m_Destroy		= false;
				localObject.m_IntValue		= intVal;
			}
			localObject.m_ObjectType	= TAGMAP_TYPE_INT;
			localObject.m_StringValue	= "";
		}
		else if( encaps.isType( JSOT_NULL ))
		{
			localObject.m_Destroy		= true;
			localObject.m_IntValue		= 0;
			localObject.m_ObjectType	= TAGMAP_TYPE_INT;
			localObject.m_StringValue	= "";
		}
		else
		{
			return JS_TRUE;
		}
		myObj->SetTempTag( localString, localObject );
	}
	else
	{
		localObject.m_Destroy		= true;
		localObject.m_ObjectType	= TAGMAP_TYPE_INT;
		localObject.m_IntValue		= 0;
		localObject.m_StringValue	= "";
		myObj->SetTempTag( localString, localObject );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetNumTags()
//|	Prototype	-	int GetNumTags()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns number of tags stored on the object
//o------------------------------------------------------------------------------------------------o
JSBool CBase_GetNumTags( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Invalid Count of Parameters: %d, need: 0", argc );
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "Invalid Object assigned (GetTag)" );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetNumTags() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_OpenBank()
//|	Prototype	-	void OpenBank( trgSock )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens bankbox of character for the specified socket
//o------------------------------------------------------------------------------------------------o
JSBool CChar_OpenBank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myChar ))
	{
		MethodError( "OpenBank: Invalid Character object assigned" );
		return JS_FALSE;
	}

	CSocket *mySock = nullptr;
	// Open the bank of myChar to myChar
	if( argc == 0 )
	{
		mySock = myChar->GetSocket();
		if( mySock != nullptr )
		{
			mySock->OpenBank( myChar );
		}
	}
	// Open it to the passed socket
	else if( argc == 1 )
	{
		mySock = static_cast<CSocket*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
		if( mySock != nullptr )
		{
			mySock->OpenBank( myChar );
		}
	}
	else
	{
		MethodError( "OpenBank, Invalid count of Paramters: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_OpenLayer()
//|	Prototype	-	void OpenLayer( socket, layerToOpen )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified layer of character for the specified socket
//o------------------------------------------------------------------------------------------------o
JSBool CChar_OpenLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myChar ))
	{
		MethodError( "OpenLayer: Invalid Character object assigned" );
		return JS_FALSE;
	}

	if( argc != 2 )
	{
		MethodError( "OpenLayer, Invalid count of Paramters: %d", argc );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( mySock != nullptr )
	{
		CItem *iLayer = myChar->GetItemAtLayer( static_cast<ItemLayers>( JSVAL_TO_INT( argv[1] )));
		if( ValidateObject( iLayer ))
		{
			mySock->OpenPack( iLayer );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_TurnToward()
//|	Prototype	-	void TurnToward( targetObject )
//|					void TurnToward( x, y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Turns character to face object/location
//o------------------------------------------------------------------------------------------------o
JSBool CChar_TurnToward( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "(TurnToward) Wrong object assigned" );
		return JS_FALSE;
	}

	SI16 x, y;

	if( argc == 1 )
	{
		if( !JSVAL_IS_OBJECT( argv[0] ))
		{
			MethodError( "(TurnToward) Invalid Object passed" );
			return JS_FALSE;
		}

		CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
		if( !ValidateObject( myObj ))
		{
			MethodError( "(TurnToward) Invalid Object passed" );
			return JS_FALSE;
		}

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if( argc == 2 )
	{
		// x + y
		x = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		y = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	}
	else
	{
		MethodError( "(TurnToward) Wrong paramter count: %d, needs either one char/item or x+y", argc );
		return JS_FALSE;
	}

	// Just don't do anything if NewDir eq OldDir

	UI08 newDir = Movement->Direction( myChar, x, y );

	if( newDir != myChar->GetDir() )
	{
		CSocket *mySock = myChar->GetSocket();
		if( mySock != nullptr )
		{
			// Update direction, but don't mark character as "dirty" to add them to update queue - we want to send the update straight away
			myChar->SetDir( newDir, false );
			CPDrawGamePlayer gpToSend(( *myChar ));
			myChar->GetSocket()->Send( &gpToSend );
			myChar->Update( nullptr, false, false );
		}
		else
		{
			// NPCs have no socket, treat slightly different!
			myChar->SetDir( newDir, false );
			myChar->Update( myChar->GetSocket(), true );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_DirectionTo()
//|	Prototype	-	unsigned char DirectionTo( trgObj )
//|					unsigned char DirectionTo( x, y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets direction from character to target object/location
//o------------------------------------------------------------------------------------------------o
JSBool CChar_DirectionTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "(DirectionTo) Wrong object assigned" );
		return JS_FALSE;
	}

	SI16 x, y;

	if( argc == 1 )
	{
		if( !JSVAL_IS_OBJECT( argv[0] ))
		{
			MethodError( "(DirectionTo) Invalid Object passed" );
			return JS_FALSE;
		}

		CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if( argc == 2 )
	{
		// x + y
		x = static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		y = static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
	}
	else
	{
		MethodError( "(DirectionTo) Wrong paramter count: %d, needs either one char/item or x+y", argc );
		return JS_FALSE;
	}

	// Just don't do anything if NewDir eq OldDir

	UI08 NewDir = Movement->Direction( myChar, x, y );

	*rval = INT_TO_JSVAL( NewDir );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ExecuteCommand()
//|	Prototype	-	void ExecuteCommand( cmdString )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes specified command (with cmd params as part of cmdString) for character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_ExecuteCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ExecuteCommand: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	CChar *myChar			= static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	char *trgMessage		= JS_GetStringBytes( targMessage );
	CSocket *targSock		= myChar->GetSocket();
	if( targSock == nullptr || trgMessage == nullptr )
	{
		MethodError( "ExecuteCommand: Invalid socket or speech (%s)", targMessage );
		return JS_FALSE;
	}
	Commands->Command( targSock, myChar, trgMessage );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild_AcceptRecruit()
//|	Prototype	-	void AcceptRecruit()
//|					void AcceptRecruit( trgChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Accepts specified character as a recruit in the guild
//o------------------------------------------------------------------------------------------------o
JSBool CGuild_AcceptRecruit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CGuild *myGuild = static_cast<CGuild*>( JS_GetPrivate( cx, obj ));

	if( myGuild == nullptr )
	{
		MethodError( "(AcceptRecruit) Invalid Object assigned" );
		return JS_FALSE;
	}

	// Two choices here... 0 paramteres = accept the JS_GetParent object
	// 1 parameter = get the cchar from there
	if( argc == 0 )
	{
		JSObject *Parent = JS_GetParent( cx, obj );
		CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, Parent ));
		myGuild->RecruitToMember( *myChar );
	}
	else if( argc == 1 )
	{
		CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
		myGuild->RecruitToMember( *myChar );
	}
	else
	{
		MethodError( "(AcceptRecruit) Invalid Parameter Count: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ResourceCount()
//|	Prototype	-	int ResourceCount( realitemid, colour )
//|					int ResourceCount( realitemid, colour, moreVal )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the amount of the items of given ID, colour and moreVal character has in packs
//o------------------------------------------------------------------------------------------------o
JSBool CChar_ResourceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "(ResourceCount) Invalid Object assigned" );
		return JS_FALSE;
	}

	UI16 realId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	SI32 itemColour = 0;
	UI32 moreVal = 0;

	if(( argc < 1 ) || ( argc > 3 ))
	{
		MethodError( "(ResourceCount) Invalid count of parameters: %d, either needs 1, 2 or 3", argc );
		return JS_FALSE;
	}

	if( argc >= 2 )
	{
		itemColour = static_cast<SI32>( JSVAL_TO_INT( argv[1] ));
	}
	if( argc >= 3 )
	{
		moreVal = static_cast<UI32>( JSVAL_TO_INT( argv[2] ));
	}

	bool colorCheck = ( itemColour != -1 ? true : false );

	*rval = INT_TO_JSVAL( GetItemAmount( myChar, realId, static_cast<UI16>( itemColour ), moreVal, colorCheck ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_UseResource()
//|	Prototype	-	int UseResource( amount, realitemid )
//|					int UseResource( amount, realitemid, colour )
//|					int UseResource( amount, realitemid, colour, moreVal )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specified amount of items of given ID, colour and MORE value from
//|					char's packs, and returns amount deleted
//o------------------------------------------------------------------------------------------------o
JSBool CBase_UseResource( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSEncapsulate myClass( cx, obj );
	CBaseObject *myObj = static_cast<CBaseObject*>( myClass.toObject() );

	if( !ValidateObject( myObj ))
	{
		MethodError( "(UseResource) Invalid Object assigned" );
		return JS_FALSE;
	}

	UI32 amount		= static_cast<UI32>( JSVAL_TO_INT( argv[0] ));
	UI16 realId		= static_cast<UI16>( JSVAL_TO_INT( argv[1] ));
	UI16 itemColour = 0;
	UI32 moreVal	= 0;

	// Min. 2 Arguments (amount + id) or max 4 (amount + id + color + moreVal)
	if(( argc < 2 ) || ( argc > 4 ))
	{
		MethodError( "(UseResource) Invalid count of parameters: %d, either needs 2, 3 or 4", argc );
		return JS_FALSE;
	}

	if( argc >= 3 )
	{
		itemColour = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	}
	if( argc >= 4 )
	{
		moreVal = static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
	}

	UI32 retVal = 0;

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar	= static_cast<CChar *>( myObj );
		retVal			= DeleteItemAmount( myChar, amount, realId, itemColour, moreVal );
	}
	else
	{
		CItem *myItem	= static_cast<CItem *>( myObj );
		retVal			= DeleteSubItemAmount( myItem, amount, realId, itemColour, moreVal );
	}
	*rval = INT_TO_JSVAL( retVal );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_BoltEffect()
//|	Prototype	-	void BoltEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the lightning bolt effect on specified character to all nearby
//o------------------------------------------------------------------------------------------------o
JSBool CChar_BoltEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( ValidateObject( myChar ))
	{
		Effects->Bolteffect( myChar );
	}

	return JS_TRUE;
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
JSBool CMisc_CustomTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSEncapsulate myClass( cx, obj );

	if(( argc > 3 ) || ( argc < 1 ))
	{
		MethodError( "(CustomTarget) Invalid count of parameters: %d, either needs 1, 2 or 3 (targetID, textToShow, cursorType", argc );
		return JS_FALSE;
	}

	CSocket *mySock = nullptr;

	if( myClass.ClassName() == "UOXChar" )
	{
		// Character
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );

		if( !ValidateObject( myChar ))
		{
			MethodError( "(CustomTarget) Invalid Character assigned" );
			return JS_FALSE;
		}

		mySock = myChar->GetSocket();
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		// We have a socket here
		mySock = static_cast<CSocket*>( myClass.toObject() );
	}

	if( mySock == nullptr )
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return JS_TRUE;
	}
	
	mySock->scriptForCallBack = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	//mySock->TempInt( static_cast<SI64>( JSMapping->GetScript( JS_GetGlobalObject( cx ))));
	UI08 tNum = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	constexpr auto maxsize = 512; // Could become long (make sure it's nullptr )
	std::string toSay;
	if( argc >= 2 )
	{
		toSay = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ));
		if( toSay.size() > maxsize )
		{
			toSay = toSay.substr( 0, maxsize );
		}
	}

	UI08 cursorType = 0;
	if( argc == 3 )
	{
		cursorType = static_cast<UI08>( JSVAL_TO_INT( argv[2] ));
	}

	if( cursorType == 3 )
	{
		mySock->SendTargetCursor( 1, 0x00000000, toSay, 3 );
	}
	else
	{
		mySock->SendTargetCursor( 1, tNum, toSay, cursorType );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_PopUpTarget()
//|	Prototype	-	void PopUpTarget( tNum, toSay )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Provides a call to an inbuilt popup target. tNum must be between 0 and 255
//|					inclusive. Says toSay, and shows a cursor. Note that this allows access
//|					potentially to GM functions.
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_PopUpTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc > 2 ) || ( argc < 1 ))
	{
		MethodError( "(PopUpTarget) Invalid count of parameters: %d, either needs 1 or 2", argc );
		return JS_FALSE;
	}

	// Either useable with sockets OR characters
	JSEncapsulate myClass( cx, obj );
	CSocket *mySock = nullptr;

	if( myClass.ClassName() == "UOXChar" )
	{
		// Character
		CChar *myChar = static_cast<CChar*>( myClass.toObject() );

		if( !ValidateObject( myChar ))
		{
			MethodError( "(PopUpTarget) Invalid Character assigned" );
			return JS_FALSE;
		}

		mySock = myChar->GetSocket();
	}

	else if( myClass.ClassName() == "UOXSocket" )
	{
		// We have a socket here
		mySock = static_cast<CSocket*>( myClass.toObject() );
	}

	if( mySock == nullptr )
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return JS_TRUE;
	}

	UI08 tNum = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	std::string toSay;
	if( argc == 2 )
	{
		toSay = oldstrutil::format( 512, "%s", JS_GetStringBytes( JS_ValueToString( cx, argv[1] )));
	}

	mySock->SendTargetCursor( 0, tNum, toSay );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_InRange()
//|	Prototype	-	bool InRange( trgObj, distance )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the distance to trgObj is less than distance
//o------------------------------------------------------------------------------------------------o
JSBool CBase_InRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(InRange): Invalid count of parameters: %d needs 2 (Item/Char and distance)", argc );
		return JS_FALSE;
	}

	UI16 distance = static_cast<UI16>( JSVAL_TO_INT( argv[1] ));

	CBaseObject *me = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( me ))
	{
		MethodError( "(InRange) Invalid Object assigned to self" );
		return JS_FALSE;
	}

	CBaseObject *them = static_cast<CBaseObject*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( them ))
	{
		MethodError( "(InRange) Invalid Object assigned to target" );
		return JS_FALSE;
	}

	if(( them->GetObjType() == OT_ITEM ) && ( me->GetObjType() == OT_CHAR ))
	{
		CItem *myItem = static_cast<CItem *>( them );
		if( myItem->GetCont() != nullptr )
		{
			*rval = BOOLEAN_TO_JSVAL( FindItemOwner( myItem ) == me );
			return JS_TRUE;
		}
	}

	*rval = BOOLEAN_TO_JSVAL( ObjInRange( me, them, distance ));
	return JS_TRUE;
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
JSBool CBase_StartTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "(StartTimer) Invalid Object assigned" );
		return JS_FALSE;
	}

	if( argc != 2 && argc != 3 )
	{
		MethodError( "(StartTimer) Invalid count of parameter: %d (needs 2 or 3)", argc );
		return JS_FALSE;
	}

	// 1. Parameter Delay, 2. Parameter Callback
	UI32 ExpireTime = BuildTimeValue( JSVAL_TO_INT( argv[0] ) / 1000.0f );
	UI16 TriggerNum = static_cast<UI16>( JSVAL_TO_INT( argv[1] ));

	CTEffect *Effect = new CTEffect;

	if( argc == 3 )
	{
		if( JSVAL_IS_BOOLEAN( argv[2] ))	// Is it a boolean?  If so, might be calling back into here
		{
			if( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE )
			{
				Effect->AssocScript( JSMapping->GetScriptId( JS_GetGlobalObject( cx )));
			}
			else
			{
				Effect->More2( 0xFFFF );
			}
		}
		else
		{
			Effect->More2( static_cast<UI16>( JSVAL_TO_INT( argv[2] )));
		}
	}
	else
	{
		Effect->More2( 0xFFFF );
	}

	Effect->Destination( myObj->GetSerial() );
	Effect->ExpireTime( ExpireTime );
	Effect->Source( myObj->GetSerial() );
	Effect->Number( 40 );
	Effect->More1( TriggerNum );

	cwmWorldState->tempEffects.Add( Effect );

	return JS_TRUE;
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
JSBool CChar_CheckSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		MethodError( "CheckSkill: Invalid number of arguments (takes 3 or 4, skillNum, minSkill, maxSkill and isCraftSkill (optional))" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "CheckSkill: Invalid character" );
		return JS_FALSE;
	}

	UI08 skillNum = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));
	UI16 minSkill = static_cast<UI16>( JSVAL_TO_INT( argv[1] ));
	UI16 maxSkill = static_cast<UI16>( JSVAL_TO_INT( argv[2] ));
	bool isCraftSkill = false;
	if( argc == 4 )
	{
		isCraftSkill = JSVAL_TO_BOOLEAN( argv[3] );
	}
	*rval = BOOLEAN_TO_JSVAL( Skills->CheckSkill( myChar, skillNum, minSkill, maxSkill, isCraftSkill ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_FindItemLayer()
//|	Prototype	-	Item FindItemLayer( layer )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for item found on specified layer of character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_FindItemLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myItem = nullptr;
	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "(FindItemLayer) Invalid Charobject assigned" );
		return JS_FALSE;
	}

	if( argc == 1 )
	{
		myItem = myChar->GetItemAtLayer( static_cast<ItemLayers>( JSVAL_TO_INT( argv[0] )));
	}
	else
	{
		MethodError( "(FindItemLayer) Unknown Count of Arguments: %d, needs: 1", argc );
		return JS_FALSE;
	}

	if( !ValidateObject( myItem ))
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	JSObject *myJSItem = JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));

	*rval = OBJECT_TO_JSVAL( myJSItem );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_FindItemType()
//|	Prototype	-	Item FindItemType( itemType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Looks for item of specific item type in character's backpack
//o------------------------------------------------------------------------------------------------o
JSBool CChar_FindItemType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(FindItemType) Invalid Count of Arguments, takes 1" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myChar ))
	{
		MethodError( "(FindItemType) Invalid Charobject assigned" );
		return JS_FALSE;
	}

	UI08 iType = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	CItem *myItem = FindItemOfType( myChar, static_cast<ItemTypes>( iType ));
	if( !ValidateObject( myItem ))
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	JSObject *myJSItem	= JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));

	*rval = OBJECT_TO_JSVAL( myJSItem );

	return JS_TRUE;
}

void OpenPlank( CItem *p );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_OpenPlank()
//|	Prototype	-	void OpenPlank()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens plank for item (boat)
//o------------------------------------------------------------------------------------------------o
JSBool CItem_OpenPlank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(OpenPlank) Invalid Count of Arguments: %d, needs: 0", argc );
		return JS_FALSE;
	}

	CItem *myItem = static_cast<CItem*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ))
	{
		MethodError( "(OpenPlank) Invalid Object assigned" );
		return JS_FALSE;
	}

	OpenPlank( myItem );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpeechInput()
//|	Prototype	-	void SpeechInput( speechId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls up the onSpeechInput event using specified ID, with the text the user types
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SpeechInput( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Get our own Script ID
	UI08 speechId		= 0;
	CItem *speechItem	= nullptr;

	if( argc == 1 ) // Just the ID has been passed
	{
		speechId = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));
	}
	else if( argc == 2 ) // We got an "affected" item as well
	{
		speechId = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

		if( argv[1] != JSVAL_NULL )
		{
			JSObject *myObj = JSVAL_TO_OBJECT( argv[1] );
			speechItem = static_cast<CItem*>( JS_GetPrivate( cx, myObj ));
		}
	}
	else
	{
		MethodError( "(SpeechInput) Invalid Count of parameters: %d, needs: either 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "(SpeechInput) Invalid object assigned" );
		return JS_FALSE;
	}

	myChar->SetSpeechMode( 9 );
	if( ValidateObject( speechItem ))
	{
		myChar->SetSpeechItem( speechItem );
	}
	else
	{
		myChar->SetSpeechItem( nullptr );
	}

	myChar->SetSpeechId( speechId );
	myChar->SetSpeechCallback( JSMapping->GetScript( JS_GetGlobalObject( cx )));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CastSpell()
//|	Prototype	-	bool CastSpell( spellNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes character to cast specified spell
//o------------------------------------------------------------------------------------------------o
JSBool CChar_CastSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc != 1 ) && ( argc != 2 ))
	{
		MethodError( "(CastSpell) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ))
	{
		MethodError( "(CastSpell) Invalid object assigned" );
		return JS_FALSE;
	}

	SI08 spellCast = static_cast<SI08>( JSVAL_TO_INT( argv[0] ));

	if( myChar->IsNpc() )
	{
		myChar->SetSpellCast( spellCast );
		Magic->CastSpell( nullptr, myChar );
	}
	else
	{
		CSocket *sock = myChar->GetSocket();

		if(( argc == 2 ) && ( argv[1] == JSVAL_TRUE ))
		{
			// Next...
			myChar->SetSpellCast( spellCast );
			Magic->CastSpell( sock, myChar );
		}
		else
		{
			bool spellSuccess = Magic->SelectSpell( sock, spellCast );
			*rval = BOOLEAN_TO_JSVAL( spellSuccess );
		}
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_MagicEffect()
//|	Prototype	-	void MagicEffect( spellId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies spell effects of specified spell to character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_MagicEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SI08 spellId = static_cast<SI08>( JSVAL_TO_INT( argv[0] ));

	CChar *myObj = static_cast<CChar*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "StaticEffect: Invalid Object" );
		return JS_FALSE;
	}

	Magic->DoStaticEffect( myObj, spellId );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetSerial()
//|	Prototype	-	int GetSerial( part )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a part (1-4) of a character's serial
//o------------------------------------------------------------------------------------------------o
JSBool CChar_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myObj = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	UI08 part = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	if( !ValidateObject( myObj ) || ( part == 0 ) || ( part > 4 ))
	{
		MethodError( "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)" );
		*rval = INT_TO_JSVAL( 0 );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetSerial( part ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_GetSerial()
//|	Prototype	-	int GetSerial( part )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a part (1-4) of an object's serial
//o------------------------------------------------------------------------------------------------o
JSBool CBase_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	UI08 part = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	if( !ValidateObject( myObj ) || ( part == 0 ) || ( part > 4 ))
	{
		MethodError( "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)" );
		*rval = INT_TO_JSVAL( 0 );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetSerial( part ));

	return JS_TRUE;
}

void UpdateStats( CBaseObject *mObj, UI08 x );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_UpdateStats()
//|	Prototype	-	void UpdateStats( statType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends update to client with specified stat (health, mana or stamina) for object
//| Notes		-	Can be used with any character, as well as with items/multis with damageable flag enabled
//o------------------------------------------------------------------------------------------------o
JSBool CBase_UpdateStats(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc != 1 )
	{
		MethodError( "(UpdateStats) Invalid Number of Arguments %d, needs: 1 (stat type - 0, 1 or 2 for Health, Mana or Stamina)", argc );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	UI08 statType = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "UpdateStats: Invalid object assigned" );
		return JS_FALSE;
	}

	if( myObj->CanBeObjType( OT_MULTI ) || myObj->CanBeObjType( OT_ITEM ))
	{
		if( statType != 0 )
		{
			MethodError( "UpdateStatus: For Items/Multis, only the Health stat (type 0) can be sent as an update to the client" );
			return JS_FALSE;
		}

		if( !myObj->IsDamageable() )
		{
			MethodError( "UpdateStatus: Can only be used with characters, or Items/Multis with damagable flag set to true" );
			return JS_FALSE;
		}
	}

	switch( statType )
	{
		case 0: // Health
			UpdateStats( myObj, 0 );
			break;
		case 1: // Mana - only relevant for characters
			UpdateStats( myObj, 1 );
			break;
		case 2: // Stamina - only relevant for characters
			UpdateStats( myObj, 2 );
			break;
		default:
			MethodError( "UpdateStats: Argument can only contain values 0, 1 or 2 for Health, Mana or Stamina respectively" );
			return JS_FALSE;
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SetPoisoned()
//|	Prototype	-	void SetPoisoned( poisonLevel, Length )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies a specified level of poison to the character for a specified amount of
//|					time (in milliseconds).
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SetPoisoned( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 2 || argc > 3 )
	{
		MethodError( "(SetPoisoned) Invalid Number of Arguments %d, needs: 2 or 3", argc );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ) || myChar->GetObjType() != OT_CHAR )
	{
		MethodError( "(SetPoisoned) Invalid object assigned" );
		return JS_FALSE;
	}

	SI08 newVal = static_cast<SI08>( JSVAL_TO_INT( argv[0] ));

	if( newVal > 0 && argc > 1 )
	{
		SI32 wearOff = static_cast<SI32>( JSVAL_TO_INT( argv[1] ));

		if( argc == 2 || ( argc == 3 && JSVAL_TO_BOOLEAN( argv[2] )))
		{
			if( myChar->GetPoisoned() > newVal )
			{
				newVal = myChar->GetPoisoned();
			}
		}
		myChar->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( static_cast<R32>( wearOff ) / 1000.0f ));
	}

	//myChar->SetPoisonStrength( newVal );
	myChar->SetPoisoned( newVal );
	return JS_TRUE;
}

void ExplodeItem( CSocket *mSock, CItem *nItem, UI32 damage, UI08 damageType, bool explodeNearby );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ExplodeItem()
//|	Prototype	-	void ExplodeItem( myItem, damage, damageType, explodeNearby )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes specified item by exploding it, dealing 5-10 dmg to nearby characters
//o------------------------------------------------------------------------------------------------o
JSBool CChar_ExplodeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "(ExplodeItem) Invalid Number of Arguments %d, needs: 4", argc );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *trgObj = static_cast<CBaseObject *>( JS_GetPrivate( cx, tObj ));

	if( !ValidateObject( trgObj ) || trgObj->GetObjType() != OT_ITEM || myChar->GetSocket() == nullptr )
	{
		MethodError( "(ExplodeItem) Invalid object" );
		return JS_FALSE;
	}

	UI32 damage = static_cast<UI32>( JSVAL_TO_INT( argv[1] ));
	UI08 damageType = static_cast<UI08>( JSVAL_TO_INT( argv[2] ));
	bool explodeNearby = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );

	ExplodeItem( myChar->GetSocket(), static_cast<CItem *>( trgObj ), damage, damageType, explodeNearby );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SetInvisible()
//|	Prototype	-	void SetInvisible( visibility, timeLength )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets character to the specified visibility level for the specified amount of
//|					time (in milliseconds).
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SetInvisible( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 2 )
	{
		MethodError( "(SetInvisible) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	UI08 newVal = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));

	myChar->SetVisible( static_cast<VisibleTypes>( newVal ));
	if( argc == 2 )
	{
		UI32 TimeOut = static_cast<UI32>( JSVAL_TO_INT( argv[1] ));
		myChar->SetTimer( tCHAR_INVIS, BuildTimeValue( static_cast<R32>( TimeOut ) / 1000.0f ));
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_SetCont()
//|	Prototype	-	bool SetCont( object )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets container of item to be the specified object
//o------------------------------------------------------------------------------------------------o
JSBool CItem_SetCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myItem = static_cast<CItem*>( JS_GetPrivate( cx, obj ));
	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *trgObj = static_cast<CBaseObject *>( JS_GetPrivate( cx, tObj ));

	if( !ValidateObject( myItem ) || !ValidateObject( trgObj ) || ( trgObj->GetSerial() == INVALIDSERIAL ))
	{
		MethodError( "SetCont: Invalid Object/Argument, takes 1 arg: containerobject" );
		return JS_FALSE;
	}

	// return true if the change was successful, false otherwise
	*rval = BOOLEAN_TO_JSVAL( myItem->SetCont( trgObj ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_IsMulti()
//|	Prototype	-	bool IsMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if item is a multi
//o------------------------------------------------------------------------------------------------o
JSBool CItem_IsMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(IsMulti) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CItem *myItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ))
	{
		MethodError( "(IsMulti) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->CanBeObjType( OT_MULTI ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_IsBoat()
//|	Prototype	-	bool IsBoat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if item is a boat
//o------------------------------------------------------------------------------------------------o
JSBool CBase_IsBoat( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(IsBoat) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CBaseObject *myObject = static_cast<CBaseObject *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObject ))
	{
		MethodError( "(IsMulti) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myObject->CanBeObjType( OT_BOAT ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsInMulti()
//|	Prototype	-	bool IsInMulti( object )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the object is in the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsInMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsInMulti) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsInMulti) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CBaseObject *toFind = static_cast<CBaseObject *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(IsInMulti) Invalid object in house" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL(( toFind->GetMultiObj() == myItem ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnBanList()
//|	Prototype	-	bool IsOnBanList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the ban-list of the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsOnBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnBanList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsOnBanList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(IsOnBanList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->IsOnBanList( toFind ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnFriendList()
//|	Prototype	-	bool IsOnFriendList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the friend-list of the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsOnFriendList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnFriendList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsOnFriendList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(IsOnFriendList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->IsOnFriendList( toFind ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnGuestList()
//|	Prototype	-	bool IsOnGuestList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the guest-list of the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsOnGuestList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnGuestList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsOnGuestList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(IsOnGuestList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->IsOnGuestList( toFind ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOnOwnerList()
//|	Prototype	-	bool IsOnOwnerList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is on the owner-list of the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsOnOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnOwnerList) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsOnOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(IsOnOwnerList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->IsOnOwnerList( toFind ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsOwner()
//|	Prototype	-	bool IsOwner( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if pChar is the actual owner of the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsOwner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOwner) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsOwner) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(IsOwner) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->IsOwner( toFind ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToBanList()
//|	Prototype	-	bool AddToBanList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to banlist of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_AddToBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToBanList) Invalid Number of Arguments %d, needs: 1", argc );
		return JS_FALSE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(AddToBanList) Invalid object assigned" );
		return JS_FALSE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(AddToBanList) Invalid character" );
		return JS_FALSE;
	}

	*rval = JSVAL_TRUE;
	myItem->AddToBanList( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToFriendList()
//|	Prototype	-	bool AddToFriendList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to the friend-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_AddToFriendList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToFriendList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(AddToFriendList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(AddToFriendList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->AddAsFriend( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToGuestList()
//|	Prototype	-	bool AddToGuestList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to the guest-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_AddToGuestList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToGuestList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(AddToGuestList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(AddToGuestList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->AddAsGuest( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddToOwnerList()
//|	Prototype	-	bool AddToOwnerList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds character pChar to the owner-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_AddToOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToOwnerList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(AddToOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(AddToOwnerList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->AddAsOwner( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromBanList()
//|	Prototype	-	bool RemoveFromBanList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the ban-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_RemoveFromBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromBanList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(RemoveFromBanList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(RemoveFromBanList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->RemoveFromBanList( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromFriendList()
//|	Prototype	-	bool RemoveFromFriendList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the friend-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_RemoveFromFriendList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromFriendList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(RemoveFromFriendList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(RemoveFromFriendList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->RemoveAsFriend( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromGuestList()
//|	Prototype	-	bool RemoveFromGuestList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the guest-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_RemoveFromGuestList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromGuestList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(RemoveFromGuestList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(RemoveFromGuestList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->RemoveAsGuest( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveFromOwnerList()
//|	Prototype	-	bool RemoveFromOwnerList( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes character pChar from the owner-list of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_RemoveFromOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromOwnerList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(RemoveFromOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = static_cast<CChar *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "(RemoveFromOwnerList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->RemoveAsOwner( toFind );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearBanList()
//|	Prototype	-	bool ClearBanList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears multi's list of banned players
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_ClearBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(ClearBanList) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(ClearBanList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->ClearBanList();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearFriendList()
//|	Prototype	-	bool ClearFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the multi's list of friends
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_ClearFriendList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(ClearFriendList) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(ClearFriendList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->ClearFriendList();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearGuestList()
//|	Prototype	-	bool ClearGuestList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the multi's list of guests
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_ClearGuestList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(ClearGuestList) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(ClearGuestList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->ClearGuestList();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ClearOwnerList()
//|	Prototype	-	bool ClearOwnerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the multi's list of co-owners
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_ClearOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(ClearOwnerList) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(ClearOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->ClearOwnerList();
	return JS_TRUE;
}

UI16 HandleAutoStack( CItem *mItem, CItem *mCont, CSocket *mSock = nullptr, CChar *mChar = nullptr );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_PlaceInPack()
//|	Prototype	-	void PlaceInPack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Randomly sets the placement/position of an item inside the container it is in.
//|					If second parameter is true, UOX3 will attempt to stack the item with an existing item
//|					instead.
//o------------------------------------------------------------------------------------------------o
JSBool CItem_PlaceInPack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "(PlaceInPack) Invalid Number of Arguments %d, needs: 0 or 1", argc );
		return JS_FALSE;
	}
	CItem *myItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_ITEM ))
	{
		MethodError( "(PlaceInPack) Invalid object assigned" );
		return JS_FALSE;
	}

	auto autoStack = ( JSVAL_TO_BOOLEAN( argv[0] ) == JS_TRUE );
	if( autoStack && ValidateObject( myItem->GetCont() ))
	{
		// Attempt to stack the item with existing items. If item has any left-over amount after, it
		// will be placed randomly in pack
		CItem *myCont = static_cast<CItem *>( myItem->GetCont() );
		if( ValidateObject( myCont ))
		{
			*rval = INT_TO_JSVAL( HandleAutoStack( myItem, myCont, nullptr, nullptr ));
		}
	}

	// Place item (or left-over item) randomly in pack
	myItem->PlaceInPack();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_OpenURL()
//|	Prototype	-	void OpenURL( UrlString )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified URL in player's browser
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_OpenURL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "OpenURL: Invalid Number of Arguments %d, needs: 1" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "OpenURL: Invalid socket!" );
		return JS_FALSE;
	}
	std::string url = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	mySock->OpenURL( url );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetByte()
//|	Prototype	-	unsigned char GetByte( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns value of a byte from the socket at offset, assumes 0 to 255 as values
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "GetByte: Invalid Number of Arguments %d, needs: 1 (offset)" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "GetByte: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset	= JSVAL_TO_INT( argv[0] );
	*rval		= INT_TO_JSVAL( mySock->GetByte( offset ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetSByte()
//|	Prototype	-	signed char GetSByte( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 1 byte of data from socket buffer at offset, assumes -127 to 127 as values
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetSByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "GetSByte: Invalid Number of Arguments %d, needs: 1 (offset)" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "GetSByte: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset	= JSVAL_TO_INT( argv[0] );
	*rval		= INT_TO_JSVAL( static_cast<SI08>( mySock->GetByte( offset )));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetWord()
//|	Prototype	-	unsigned int GetWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 2 bytes of data from socket buffer at offset, assumes positive values
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "GetWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "GetWord: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( mySock->GetWord( offset ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetSWord()
//|	Prototype	-	signed int GetSWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 2 bytes of data from socket buffer at offset, negative values accepted
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetSWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "GetSWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "GetSWord: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( static_cast<SI16>(mySock->GetWord( offset )));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetDWord()
//|	Prototype	-	unsigned int GetDWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 4 bytes of data from socket buffer at offset, positive values assumed
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "GetDWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "GetDWord: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset = JSVAL_TO_INT( argv[0] );
	JS_NewNumberValue( cx, mySock->GetDWord( offset ), rval );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetSDWord()
//|	Prototype	-	signed int GetSDWord( offset )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns 4 bytes of data from socket buffer at offset, negative values accepted
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetSDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) // 1 parameters
	{
		MethodError( "GetSDWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "GetSDWord: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( static_cast<SI32>(mySock->GetDWord( offset )));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_GetString()
//|	Prototype	-	string GetString( offset [, length] )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns data from socket buffer at offset with optional length param, string assumed
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_GetString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "GetString: Invalid number of arguments (takes 1 or 2)" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "GetString: Invalid socket!" );
		return JS_FALSE;
	}

	SI32 length		= -1;
	SI32 offset		= JSVAL_TO_INT( argv[0] );
	if( argc == 2 )
	{
		length = JSVAL_TO_INT( argv[1] );
	}

	char toReturn[128];
	if( length != -1 )
	{
		strncopy( toReturn, 128, ( char * ) & ( mSock->Buffer() )[offset], length );
		toReturn[length] = 0;
	}
	else
		strcopy( toReturn, 128, ( char * ) & ( mSock->Buffer() )[offset] );

	JSString *strSpeech = nullptr;
	strSpeech = JS_NewStringCopyZ( cx, toReturn );
	*rval = STRING_TO_JSVAL( strSpeech );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetByte()
//|	Prototype	-	void SetByte( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets 1 byte at socket stream's offset to equal 8-bit value
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_SetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetByte: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "SetByte: Invalid socket!" );
		return JS_FALSE;
	}
	SI32 offset = JSVAL_TO_INT( argv[0] );
	UI08 byteToSet = static_cast<UI08>( JSVAL_TO_INT( argv[1] ));

	mSock->SetByte( offset, byteToSet );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetWord()
//|	Prototype	-	void SetWord( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets 2 bytes at socket stream's offset to equal 16-bit value
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_SetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "SetWord: Invalid socket!" );
		return JS_FALSE;
	}

	SI32 offset		= JSVAL_TO_INT( argv[0] );
	UI16 byteToSet	= static_cast<UI16>( JSVAL_TO_INT( argv[1] ));

	mSock->SetWord( offset, byteToSet );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetDWord()
//|	Prototype	-	void SeDtWord( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets 4 bytes at socket stream's offset to equal 32-bit value
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_SetDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetDWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "SetDWord: Invalid socket!" );
		return JS_FALSE;
	}

	SI32 offset		= JSVAL_TO_INT( argv[0] );
	UI32 byteToSet	= JSVAL_TO_INT( argv[1] );

	mSock->SetDWord( offset, byteToSet );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SetString()
//|	Prototype	-	void SetString( offset, value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets data at socket stream's offset to equal string value
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_SetString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetString: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "SetString: Invalid socket!" );
		return JS_FALSE;
	}

	SI32 offset = JSVAL_TO_INT( argv[0] );
	char *trgMessage = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ));
	if( trgMessage == nullptr )
	{
		MethodError( "SetString: No string to set" );
		return JS_FALSE;
	}
	// FIXME
	auto size = strlen( trgMessage );
	strcopy(( char * ) & ( mSock->Buffer() )[offset], size + 1, trgMessage );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_ReadBytes()
//|	Prototype	-	void ReadBytes( bytecount )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads specified amount of bytes from socket
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_ReadBytes( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ReadBytes: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "ReadBytes: Invalid socket!" );
		return JS_FALSE;
	}

	SI32 bCount		= JSVAL_TO_INT( argv[0] );
	mSock->Receive( bCount );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_WhoList()
//|	Prototype	-	void WhoList( bSendOnList )
//|					void WhoList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a gump populated with all online or offline (if param is false) players
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_WhoList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mySock == nullptr )
	{
		MethodError( "WhoList: Invalid socket!" );
		return JS_FALSE;
	}

	bool sendOnList = true;
	if( argc == 1 )
	{
		sendOnList = ( JSVAL_TO_BOOLEAN( argv[0] ) == JS_TRUE );
	}

	if( sendOnList )
	{
		WhoList->SendSocket( mySock );
	}
	else
	{
		OffList->SendSocket( mySock );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Music()
//|	Prototype	-	void Music( musicNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to socket to play specified midi/mp3
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_Music( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Music: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	UI16 music = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));

	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock != nullptr )
	{
		Effects->PlayMusic( mySock, music );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_YellMessage()
//|	Prototype	-	void YellMessage( message )
//|	Date		-	January 30, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Yells a text message to those in range
//o------------------------------------------------------------------------------------------------o
JSBool CChar_YellMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "YellMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );

	if( trgMessage == nullptr )
	{
		MethodError( "YellMessage: You have to supply a messagetext" );
	}

	CChar *myChar = static_cast<CChar*>( myObj );
	if( !ValidateObject( myChar ))
	{
		MethodError( "YellMessage: Invalid Character" );
		return JS_FALSE;
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	if( myChar->GetNpcAiType() == AI_EVIL || myChar->GetNpcAiType() == AI_EVIL_CASTER )
	{
		MethodSpeech( *myChar, trgMessage, YELL, 0x0026, static_cast<FontType>( myChar->GetFontType() ), SPTRG_PCNPC, INVALIDSERIAL, useUnicode );
	}
	else
	{
		MethodSpeech( *myChar, trgMessage, YELL, myChar->GetSayColour(), static_cast<FontType>( myChar->GetFontType() ), SPTRG_PCNPC, INVALIDSERIAL, useUnicode );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_WhisperMessage()
//|	Prototype	-	void WhisperMessage( message )
//|	Date		-	January 30, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Whispers a text message to those in range
//o------------------------------------------------------------------------------------------------o
JSBool CChar_WhisperMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "WhisperMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	JSString *targMessage = JS_ValueToString( cx, argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );

	if( trgMessage == nullptr )
	{
		MethodError( "WhisperMessage: You have to supply a messagetext" );
	}

	CChar *myChar = static_cast<CChar*>( myObj );
	if( !ValidateObject( myChar ))
	{
		MethodError( "WhisperMessage: Invalid Character" );
		return JS_FALSE;
	}

	bool useUnicode = cwmWorldState->ServerData()->UseUnicodeMessages();

	if( myChar->GetNpcAiType() == AI_EVIL || myChar->GetNpcAiType() == AI_EVIL_CASTER  )
	{
		MethodSpeech( *myChar, trgMessage, WHISPER, 0x0026, static_cast<FontType>( myChar->GetFontType() ), SPTRG_PCNPC, INVALIDSERIAL, useUnicode );
	}
	else
	{
		MethodSpeech( *myChar, trgMessage, WHISPER, myChar->GetSayColour(), static_cast<FontType>( myChar->GetFontType() ), SPTRG_PCNPC, INVALIDSERIAL, useUnicode );
	}

	return JS_TRUE;
}

void BuildGumpFromScripts( CSocket *s, UI16 m );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_OpenGump()
//|	Prototype	-	void OpenGump( gumpMenuId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens specified gumpmenu from dfnata/misc/gumps.dfn for socket
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_OpenGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "OpenGump: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ))
	{
		MethodError( "You have to pass a valid menu number" );
	}

	UI16 menuNumber = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	CSocket *mySock = static_cast<CSocket *>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "OpenGump: Unknown Object has been passed" );
		return JS_FALSE;
	}

	BuildGumpFromScripts( mySock, menuNumber );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_CloseGump()
//|	Prototype	-	void CloseGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes specified generic gump based on its ID, and provides a button ID response
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_CloseGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "CloseGump: Invalid number of arguments (takes 2 - gumpId to close, and buttonId to send as response)" );
		return JS_FALSE;
	}

	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));

	if( mySock == nullptr )
	{
		MethodError( "SysMessage: Invalid socket" );
		return JS_FALSE;
	}

	UI32 gumpId = static_cast<UI32>( JSVAL_TO_INT( argv[0] ));
	UI32 buttonId = static_cast<UI32>( JSVAL_TO_INT( argv[1] ));

	CPCloseGump gumpToClose( gumpId, buttonId );
	mySock->Send( &gumpToClose );

	return JS_TRUE;
}

// Race methods

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_CanWearArmour()
//|	Prototype	-	bool CanWearArmour( tItem )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if members of race can equip specified item
//o------------------------------------------------------------------------------------------------o
JSBool CRace_CanWearArmour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "CanWearArmour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = static_cast<CRace *>( JS_GetPrivate( cx, obj ));
	if( myRace == nullptr )
	{
		MethodError( "CanWearArmour: Unknown Object has been passed" );
		return JS_FALSE;
	}

	CItem *toFind = static_cast<CItem *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( toFind ))
	{
		MethodError( "CanWearArmour: Invalid item passed" );
		return JS_TRUE;
	}

	ARMORCLASS srcClass = myRace->ArmourClassRestriction();
	ARMORCLASS trgClass = toFind->GetArmourClass();
	*rval = BOOLEAN_TO_JSVAL(( trgClass == 0 ) || (( srcClass & trgClass ) != 0 ));	// they have a matching class
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_IsValidHairColour()
//|	Prototype	-	bool IsValidRaceHairColour( tColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified hair-colour is allowed for members of race
//o------------------------------------------------------------------------------------------------o
JSBool CRace_IsValidHairColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidHairColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = static_cast<CRace *>( JS_GetPrivate( cx, obj ));
	if( myRace == nullptr )
	{
		MethodError( "IsValidHairColour: Unknown Object has been passed" );
		return JS_FALSE;
	}

	COLOUR cVal = static_cast<COLOUR>( JSVAL_TO_INT( argv[0] ));
	*rval = BOOLEAN_TO_JSVAL(( myRace->IsValidHair( cVal )));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_IsValidSkinColour()
//|	Prototype	-	bool IsValidSkinColour( tColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified skin-colour is allowed for members of race
//o------------------------------------------------------------------------------------------------o
JSBool CRace_IsValidSkinColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidSkinColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = static_cast<CRace *>( JS_GetPrivate( cx, obj ));
	if( myRace == nullptr )
	{
		MethodError( "IsValidSkinColour: Unknown Object has been passed" );
		return JS_FALSE;
	}

	COLOUR cVal = static_cast<COLOUR>( JSVAL_TO_INT( argv[0] ));
	*rval = BOOLEAN_TO_JSVAL(( myRace->IsValidSkin( cVal )));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace_IsValidBeardColour()
//|	Prototype	-	bool IsValidBeardColour( tColour )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified beard-colour is allowed for members of race
//o------------------------------------------------------------------------------------------------o
JSBool CRace_IsValidBeardColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidBeardColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = static_cast<CRace *>( JS_GetPrivate( cx, obj ));
	if( myRace == nullptr )
	{
		MethodError( "IsValidBeardColour: Unknown Object has been passed" );
		return JS_FALSE;
	}

	COLOUR cVal = static_cast<COLOUR>( JSVAL_TO_INT( argv[0] ));
	*rval = BOOLEAN_TO_JSVAL(( myRace->IsValidBeard( cVal )));
	return JS_TRUE;
}


bool ApplyItemSection( CItem *applyTo, CScriptSection *toApply, std::string sectionId );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_ApplySection()
//|	Prototype	-	void ApplySection( scriptSection )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies the values from a DFN section to an Item/Character
//o------------------------------------------------------------------------------------------------o
JSBool CBase_ApplySection( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ApplySection: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CBaseObject *myObj		= static_cast<CBaseObject*>( myClass.toObject() );
	std::string trgSection	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));

	if( trgSection.empty() || trgSection.length() == 0 )
	{
		MethodError( "You have to supply a section to apply" );
		return JS_FALSE;
	}

	if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = static_cast<CItem*>( myObj );
		if( !ValidateObject( myItem ))
		{
			MethodError( "ApplySection: Invalid Item" );
			return JS_FALSE;
		}
		CScriptSection *toFind = FileLookup->FindEntry( trgSection, items_def );
		ApplyItemSection( myItem, toFind, trgSection );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = static_cast<CChar*>( myObj );
		if( !ValidateObject( myChar ))
		{
			MethodError( "ApplySection: Invalid Character" );
			return JS_FALSE;
		}

		CScriptSection *toFind = FileLookup->FindEntry( trgSection, npc_def );
		Npcs->ApplyNpcSection( myChar, toFind, trgSection );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddSpell()
//|	Prototype	-	void AddSpell( spellNum )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a spell to the first spell book found in character's pack
//o------------------------------------------------------------------------------------------------o
JSBool CChar_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddSpell: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CChar *myChar	= static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	UI08 spellNum	= static_cast<UI08>( JSVAL_TO_INT( argv[0] ));
	CItem *sBook	= FindItemOfType( myChar, IT_SPELLBOOK );
	if( ValidateObject( sBook ))
	{
		Magic->AddSpell( sBook, spellNum );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpellFail()
//|	Prototype	-	void SpellFail()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Does the actions associated with spell failure, called after the failure happens
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "SpellFail: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));

	Effects->PlayStaticAnimation( myChar, 0x3735, 0, 30 );
	Effects->PlaySound( myChar, 0x005C );
	if( !myChar->IsNpc() )
	{
		CSocket *mSock = myChar->GetSocket();
		if( mSock != nullptr )
		{
			myChar->TextMessage( mSock, 771, EMOTE, false );
		}
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_Refresh()
//|	Prototype	-	void Refresh()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Causes the item to be refreshed to sockets that can see it
//o------------------------------------------------------------------------------------------------o
JSBool CBase_Refresh( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Refresh: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myObj ))
	{
		MethodError( "Refresh: Invalid object assigned - only Charaacters or Items accepted" );
		return JS_FALSE;
	}

	if( myObj->CanBeObjType( OT_CHAR ))
	{
		CChar *myChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
		if( ValidateObject( myChar ))
		{
			myChar->Update();
		}
	}
	else if( myObj->CanBeObjType( OT_ITEM ))
	{
		CItem *myItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
		if( ValidateObject( myItem ))
		{
			myItem->Update();
		}
	}
	else
	{
		MethodError( "Refresh: Invalid object assigned - only Charaacters or Items accepted" );
		return JS_FALSE;
	}

	return JS_TRUE;
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
JSBool CItem_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "ApplyRank: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	CItem *myItem	= static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	SI32 rank		= JSVAL_TO_INT( argv[0] );
	SI32 maxrank	= JSVAL_TO_INT( argv[1] );

	Skills->ApplyRank( nullptr, myItem, rank, maxrank );
	return JS_TRUE;
}

bool IsOnFoodList( const std::string& sFoodList, const UI16 sItemId );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_IsOnFoodList()
//|	Prototype	-	bool IsOnFoodList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if item is on a specified food list
//o------------------------------------------------------------------------------------------------o
JSBool CItem_IsOnFoodList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 || argc > 7 )
	{
		MethodError( "(IsOnFoodList) Invalid Number of Arguments %d, needs: 1 - foodlist name", argc );
		return JS_TRUE;
	}

	CItem *myItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myItem ))
	{
		MethodError( "(IsOnFoodList) Invalid object assigned" );
		return JS_TRUE;
	}

	if( !JSVAL_IS_STRING( argv[0] ))
	{
		MethodError( "IsOnFoodList: Invalid parameter specifled, string required!" );
		return JS_FALSE;
	}
	std::string foodList = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));

	*rval = BOOLEAN_TO_JSVAL( IsOnFoodList( foodList, myItem->GetId() ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CAccount_GetAccount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	NOT IMPLEMENTED
//o------------------------------------------------------------------------------------------------o
JSBool CAccount_GetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_FALSE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CAccount_SetAccount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	NOT IMPLEMENTED
//o------------------------------------------------------------------------------------------------o
JSBool CAccount_SetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_FALSE;
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
JSBool CAccount_AddAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "Account.AddAccount(user,pass,email,flags): Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	// Ok get our object from the global context
	if( !JSVAL_IS_STRING( argv[0] ) || !JSVAL_IS_STRING( argv[1] ) || !JSVAL_IS_STRING( argv[2] ) || !( JSVAL_IS_INT( argv[3] ) || JSVAL_IS_STRING( argv[3] )))
	{
		MethodError( "Account.AddAccount(user,pass,email,flags): Invalid parameter specifled, please check param types." );
		return JS_FALSE;
	}
	std::string lpszUsername	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	std::string lpszPassword	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ));
	std::string lpszComment		= JS_GetStringBytes( JS_ValueToString( cx, argv[2] ));
	UI16 u16Flags		= 0;

	if( JSVAL_IS_INT( argv[3] ))
	{
		u16Flags = static_cast<UI16>( JSVAL_TO_INT( argv[3] ));
	}
	else
	{
		u16Flags = static_cast<UI16>( std::stoul( JS_GetStringBytes( JS_ValueToString( cx, argv[3] )), nullptr, 0 ));
	}

	if( lpszUsername.empty() || lpszPassword.empty() || lpszComment.empty() || lpszUsername.length() == 0 || lpszPassword.length() == 0 || lpszComment.length() == 0 )
		return JS_FALSE;


	if( !Accounts->AddAccount( lpszUsername, lpszPassword, lpszComment, u16Flags ))
	{
		MethodError( "Account.AddAccount( user, pass, email, flags ): Account was not added. Duplicate expected." );
		return JS_FALSE;
	}
	return JS_TRUE;
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
JSBool CAccount_DelAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Account.DelAccount([username/id]): Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	// Ok get out object from the global context
	if( JSVAL_IS_STRING( argv[0] ))
	{
		char *lpszUsername = nullptr;
		lpszUsername = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
		if( !Accounts->DelAccount( lpszUsername ))
		{
			MethodError( " Account.DelAccount(username): Unable to remove account specified." );
			return JS_FALSE;
		}

	}
	else if( JSVAL_IS_INT( argv[0] ))
	{
		UI16 ui16AccountId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
		if( !Accounts->DelAccount( ui16AccountId ))
		{
			MethodError( " Account.DelAccount(accountID): Unable to remove account specified." );
			return JS_FALSE;
		}
	}
	else
	{
		return JS_FALSE;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CAccount_ModAccount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	NOT IMPLEMENTED
//o------------------------------------------------------------------------------------------------o
JSBool CAccount_ModAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_FALSE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CAccount_SaveAccounts()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	NOT IMPLEMENTED
//o------------------------------------------------------------------------------------------------o
JSBool CAccount_SaveAccounts( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_FALSE;
}

// Basic file wrapping structure for abstracting away file IO for the JS file funcs
// UOXCFile constructor !
JSBool UOXCFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UOXFileWrapper_st *toAdd = new UOXFileWrapper_st;
	toAdd->mWrap = nullptr;

	//JSBool myVal = JS_DefineFunctions( cx, obj, CFile_Methods );
	JS_DefineFunctions( cx, obj, CFile_Methods );
	JS_SetPrivate( cx, obj, toAdd );
	JS_LockGCThing( cx, obj );
	//JS_AddRoot( cx, &obj );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Free()
//|	Prototype	-	void Free()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Frees memory allocated by file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Free: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));
	delete mFile;
	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, &obj );
	JS_SetPrivate( cx, obj, nullptr );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Open()
//|	Prototype	-	void Open( string filename, string accessMethod )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a file for reading, writing or appending
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Open( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 2 || argc > 4 )
	{
		MethodError( "Open: Invalid number of arguments (takes 2 to 4 - filename, file mode and - optionally - folderName and useScriptDataDir bool)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	char *fileName = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	std::string mode = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ));
	char *folderName = nullptr;
	if( argc >= 3 )
	{
		folderName = JS_GetStringBytes( JS_ValueToString( cx, argv[2] ));
	}
	bool useScriptDataDir = false;
	if( argc >= 4 )
	{
		useScriptDataDir = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );
	}

	if( oldstrutil::lower( mode ).find_first_of( "rwa", 0, 3 ) == std::string::npos )
	{
		MethodError( "Open: Invalid mode must be \"read\", \"write\", or \"append\"!" );
		return JS_FALSE;
	}
	if( strstr( fileName, ".." ) || strstr( fileName, "\\" ) || strstr( fileName, "/" ))
	{
		MethodError( "Open: file names may not contain \"..\", \"\\\", or \"/\"." );
		return JS_FALSE;
	}

	std::string filePath = cwmWorldState->ServerData()->Directory( CSDDP_SHARED );

	// if folderName argument was supplied, use it, and create the appropriate folder under the /shared/ folder
	if( folderName != nullptr )
	{
		// However, don't allow special characters in the folder name
		if( strstr( folderName, ".." ) || strstr( folderName, "\\" ) || strstr( folderName, "/" ))
		{
			MethodError( "Open: folder names may not contain \"..\", \"\\\", or \"/\"." );
			return JS_FALSE;
		}

		// If script wants to look in script-data folder instead of shared folder, let it
		if( useScriptDataDir )
		{
			filePath = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTDATA );
		}

		// Append the provided folder name
		filePath.append( folderName );

		if( !std::filesystem::exists( filePath ))
		{
			// Create missing directory
			std::filesystem::create_directory( filePath );
		}

		filePath += std::filesystem::path::preferred_separator;
	}

	filePath.append( fileName );
	FILE* stream;

	mFile->mWrap = mfopen( &stream, filePath.c_str(), oldstrutil::lower( mode ).substr( 0, 1 ).c_str() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Close()
//|	Prototype	-	void Close()
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Closes a file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Open: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	fclose( mFile->mWrap );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Read()
//|	Prototype	-	string Read( int numBytes )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a string of length numBytes, reading numBytes from the opened file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Read: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ))
	{
		MethodError( "Read: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	SI32 bytes = JSVAL_TO_INT( argv[0] );
	char data[512];

	if( bytes > 512 || bytes < 1 )
	{
		MethodError( "Read: Invalid byte count, must be from 1 to 512!" );
		return JS_FALSE;
	}

	// We don't care about return value, so suppress compiler warning
	[[maybe_unused]] size_t bytesRead = fread( data, 1, bytes, mFile->mWrap );

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, data ));
	return JS_TRUE;
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_ReadUntil()
//|	Prototype	-	string ReadUntil( string delimeter )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads a string until it encounters a newline or the string specified by delimeter.
//o------------------------------------------------------------------------------------------------o
JSBool CFile_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ReadUntil: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ))
	{
		MethodError( "ReadUntil: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	char *until = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	char line[512];
	SI32 c;

	if( until[0] == '\\' && strlen( until ) > 1 )
	{
		switch( until[1] )
		{
			case '\\': until[0] = '\\'; break;
			case '\'': until[0] = '\''; break;
			case '\"': until[0] = '\"'; break;
			case 'n':  until[0] = '\n'; break;
			default: MethodError( "ReadUntil: Unsupported character escape sequence %s", until );	break;
		}
	}

	for( c = 0; c < 512 && !feof( mFile->mWrap ); ++c )
	{
		line[c] = fgetc( mFile->mWrap );
		if( line[c] == until[0] || line[c] == '\n' )
			break;
	}
	line[c < 512 ? c : 511] = 0;

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, line ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Write()
//|	Prototype	-	void Write( string toWrite )
//|	Date		-	23 June, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes a string out to the file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Write: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ))
	{
		MethodError( "Write: Error writing to file, file was not opened sucessfully!" );
		return JS_FALSE;
	}
	else if( ftell( mFile->mWrap ) > ( 10 * 1024 * 1024 ))
	{
		MethodError( "Write: Error writing to file.  File may not exceed 10mb." );
		return JS_FALSE;
	}

	char *str = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	if( str != nullptr )
	{
		fprintf( mFile->mWrap, "%s", str );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_EOF()
//|	Prototype	-	bool eof()
//|	Date		-	3/05/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns if we have read to the end of a file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_EOF( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "EOF: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	if( !mFile || !mFile->mWrap )
		return JS_FALSE;

	*rval = BOOLEAN_TO_JSVAL(( feof( mFile->mWrap ) != 0 ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Length()
//|	Prototype	-	int Length()
//|	Date		-	3/05/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the length of the file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Length( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Length: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	if( !mFile || !mFile->mWrap )
	{
		*rval = INT_TO_JSVAL( -1 );
		return JS_TRUE;
	}

	long fpos = ftell( mFile->mWrap );
	fseek( mFile->mWrap, 0, SEEK_END );
	*rval = INT_TO_JSVAL( ftell( mFile->mWrap ));

	if( fpos > -1 )
	{
		[[maybe_unused]] int newFPos = fseek( mFile->mWrap, fpos, SEEK_SET ); // We don't care about return value, so suppress compiler warning
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CFile_Pos()
//|	Prototype	-	int Post()
//|	Date		-	3/05/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns or sets the position we are at in the file
//o------------------------------------------------------------------------------------------------o
JSBool CFile_Pos( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 && argc != 1 )
	{
		MethodError( "Pos: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper_st *mFile = static_cast<UOXFileWrapper_st *>( JS_GetPrivate( cx, obj ));

	if( !mFile || !mFile->mWrap )
		return JS_FALSE;

	if( argc == 1 )
	{
		[[maybe_unused]] int newFPos = fseek( mFile->mWrap, JSVAL_TO_INT( argv[0] ), SEEK_SET );
	}

	*rval = INT_TO_JSVAL( ftell( mFile->mWrap ));

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_FirstItem()
//|	Prototype	-	bool FirstItem()
//|	Date		-	02 Aug 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns first object in the object's (container's) list
//o------------------------------------------------------------------------------------------------o
JSBool CBase_FirstItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "FirstItem: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "FirstItem: Invalid object assigned." );
		return JS_FALSE;
	}
	CItem *firstItem = nullptr;
	if( myObj->GetObjType() == OT_CHAR )
	{
		firstItem = ( static_cast<CChar *>( myObj ))->FirstItem();
	}
	else if( myObj->GetObjType() == OT_ITEM )
	{
		firstItem = ( static_cast<CItem *>( myObj ))->GetContainsList()->First();
	}
	else if( myObj->GetObjType() == OT_MULTI )
	{
		firstItem = ( static_cast<CMultiObj *>( myObj ))->GetItemsInMultiList()->First();
	}
	else
	{
		MethodError( "FirstItem: Unknown object type assigned." );
		return JS_FALSE;
	}

	if( ValidateObject( firstItem ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, firstItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
	{
		*rval = JSVAL_NULL;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_NextItem()
//|	Prototype	-	bool NextItem()
//|	Date		-	02 Aug 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns next object in the object's (container's) list
//o------------------------------------------------------------------------------------------------o
JSBool CBase_NextItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NextItem: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "NextItem: Invalid object assigned." );
		return JS_FALSE;
	}
	CItem *nextItem = nullptr;
	if( myObj->GetObjType() == OT_CHAR )
	{
		nextItem = ( static_cast<CChar *>( myObj ))->NextItem();
	}
	else if( myObj->GetObjType() == OT_ITEM )
	{
		nextItem = ( static_cast<CItem *>( myObj ))->GetContainsList()->Next();
	}
	else if( myObj->GetObjType() == OT_MULTI )
	{
		nextItem = ( static_cast<CMultiObj *>( myObj ))->GetItemsInMultiList()->Next();
	}
	else
	{
		MethodError( "NextItem: Unknown object type assigned." );
		return JS_FALSE;
	}

	if( ValidateObject( nextItem ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, nextItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
	{
		*rval = JSVAL_NULL;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_FinishedItems()
//|	Prototype	-	bool FinishedItems()
//|	Date		-	02 Aug 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if finished all items in object's list
//o------------------------------------------------------------------------------------------------o
JSBool CBase_FinishedItems( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "FinishedItems: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "NextItem: Invalid object assigned." );
		return JS_FALSE;
	}
	if( myObj->GetObjType() == OT_CHAR )
	{
		*rval = BOOLEAN_TO_JSVAL(( static_cast<CChar *>( myObj ))->FinishedItems() );
	}
	else if( myObj->GetObjType() == OT_ITEM )
	{
		*rval = BOOLEAN_TO_JSVAL(( static_cast<CItem *>( myObj ))->GetContainsList()->Finished() );
	}
	else if( myObj->GetObjType() == OT_MULTI )
	{
		*rval = BOOLEAN_TO_JSVAL(( static_cast<CMultiObj *>( myObj ))->GetItemsInMultiList()->Finished() );
	}
	return JS_TRUE;
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
JSBool CChar_WalkTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		MethodError( "WalkTo: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( cMove ))
	{
		MethodError( "WalkTo: Invalid source character" );
		return JS_FALSE;
	}
	SI16 gx			= 0;
	SI16 gy			= 0;
	UI08 maxSteps	= 0;
	switch( argc )
	{
		case 2:
			if( JSVAL_IS_OBJECT( argv[0] ))
			{	// we can work with this, it should be either a character or item, hopefully
				JSEncapsulate jsToGoTo( cx, &( argv[0] ));
				if( jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar" )
				{
					CBaseObject *toGoTo = static_cast<CBaseObject *>( jsToGoTo.toObject() );
					if( !ValidateObject( toGoTo ))
					{
						MethodError( "No object associated with this object" );
						break;
					}
					gx = toGoTo->GetX();
					gy = toGoTo->GetY();
				}
				else if( jsToGoTo.ClassName() == "UOXSocket" )
				{
					CSocket *mySock		= static_cast<CSocket *>( jsToGoTo.toObject() );
					CChar *mySockChar	= mySock->CurrcharObj();
					gx					= mySockChar->GetX();
					gy					= mySockChar->GetY();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
				maxSteps = static_cast<UI08>( JSVAL_TO_INT( argv[1] ));
				break;
			}
			MethodError( "Text-styled Parameters may be added later" );
			break;

			// 2 Parameters, x + y
		case 3:
			gx			= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
			gy			= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
			maxSteps	= static_cast<UI08>( JSVAL_TO_INT( argv[2] ));
			break;
		default:
			MethodError( "Invalid number of arguments passed to WalkTo, needs either 2 or 3" );
			break;
	}

	cMove->FlushPath();
#if defined( UOX_DEBUG_MODE )
	Console.Print( oldstrutil::format( "WalkTo: Moving character 0x%X to (%i,%i) with a maximum of %i steps\n", cMove->GetSerial(), gx, gy, maxSteps ));
#endif
	if( cMove->GetNpcWander() != WT_PATHFIND )
	{
		// Only update oldNpcWander if NPC is not already pathfinding
		cMove->SetOldNpcWander( cMove->GetNpcWander() );
	}
	cMove->SetNpcWander( WT_PATHFIND );
	if( cwmWorldState->ServerData()->AdvancedPathfinding() )
	{
		Movement->AdvancedPathfinding( cMove, gx, gy, false, maxSteps );
	}
	else
	{
		Movement->PathFind( cMove, gx, gy, false, maxSteps );
	}
	return JS_TRUE;
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
JSBool CChar_RunTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		MethodError( "RunTo: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( cMove ))
	{
		MethodError( "RunTo: Invalid source character" );
		return JS_FALSE;
	}
	UI16 gx			= 0;
	UI16 gy			= 0;
	UI08 maxSteps	= 0;
	switch( argc )
	{
		case 2:
			if( JSVAL_IS_OBJECT( argv[0] ))
			{	// we can work with this, it should be either a character or item, hopefully
				JSEncapsulate jsToGoTo( cx, &( argv[0] ));
				if( jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar" )
				{
					CBaseObject *toGoTo = static_cast<CBaseObject *>( jsToGoTo.toObject() );
					if( !ValidateObject( toGoTo ))
					{
						MethodError( "No object associated with this object" );
						break;
					}
					gx = toGoTo->GetX();
					gy = toGoTo->GetY();
				}
				else if( jsToGoTo.ClassName() == "UOXSocket" )
				{
					CSocket *mySock		= static_cast<CSocket *>( jsToGoTo.toObject() );
					CChar *mySockChar	= mySock->CurrcharObj();
					gx					= mySockChar->GetX();
					gy					= mySockChar->GetY();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
				maxSteps = static_cast<UI08>( JSVAL_TO_INT( argv[1] ));
				break;
			}
			MethodError( "Text-styled Parameters may be added later" );
			break;

			// 2 Parameters, x + y
		case 3:
			gx			= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
			gy			= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
			maxSteps	= static_cast<UI08>( JSVAL_TO_INT( argv[2] ));
			break;
		default:
			MethodError( "Invalid number of arguments passed to RunTo, needs either 2 or 3" );
			break;
	}

	cMove->FlushPath();
#if defined( UOX_DEBUG_MODE )
	Console.Print( oldstrutil::format( "RunTo: Moving character %i to (%i,%i) with a maximum of %i steps", cMove->GetSerial(), gx, gy, maxSteps ));
#endif
	if( cMove->GetNpcWander() != WT_PATHFIND )
	{
		// Only update oldNpcWander if NPC is not already pathfinding
		cMove->SetOldNpcWander( cMove->GetNpcWander() );
	}
	cMove->SetNpcWander( WT_PATHFIND );

	if( cwmWorldState->ServerData()->AdvancedPathfinding() )
	{
		Movement->AdvancedPathfinding( cMove, gx, gy, true );
	}
	else
	{
		Movement->PathFind( cMove, gx, gy, true, maxSteps );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_GetTimer()
//|	Prototype	-	int GetTimer( timerId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the specified timer value
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_GetTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "GetTimer: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &( argv[0] ));
	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *cMove = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( cMove ))
		{
			MethodError( "GetTimer: Invalid source character" );
			return JS_FALSE;
		}

		JS_NewNumberValue( cx, cMove->GetTimer( static_cast<cC_TID>( encaps.toInt() )), rval );
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mSock = static_cast<CSocket *>( myClass.toObject() );
		if( mSock == nullptr )
		{
			MethodError( "GetTimer: Invalid source socket" );
			return JS_FALSE;
		}

		JS_NewNumberValue( cx, mSock->GetTimer( static_cast<cS_TID>( encaps.toInt() )), rval );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMisc_SetTimer()
//|	Prototype	-	bool SetTimer( timerId, numMilliSeconds )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the specified timer with the amount of miliseconds until it expires
//o------------------------------------------------------------------------------------------------o
JSBool CMisc_SetTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetTimer: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &( argv[0] ));
	JSEncapsulate encaps2( cx, &( argv[1] ));
	JSEncapsulate myClass( cx, obj );

	R32 timerVal = encaps2.toFloat();
	if( timerVal != 0 )
	{
		timerVal = BuildTimeValue( timerVal / 1000.0f );
	}
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *cMove = static_cast<CChar*>( myClass.toObject() );
		if( !ValidateObject( cMove ))
		{
			MethodError( "SetTimer: Invalid source character" );
			return JS_FALSE;
		}

		cMove->SetTimer( static_cast<cC_TID>( encaps.toInt() ), static_cast<TIMERVAL>( timerVal ));
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mSock = static_cast<CSocket *>( myClass.toObject() );
		if( mSock == nullptr )
		{
			MethodError( "SetTimer: Invalid source socket" );
			return JS_FALSE;
		}

		mSock->SetTimer( static_cast<cS_TID>( encaps.toInt() ), static_cast<TIMERVAL>( timerVal ));
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_DistanceTo()
//|	Prototype	-	unsigned short DistanceTo( object )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the distance to the object
//o------------------------------------------------------------------------------------------------o
JSBool CBase_DistanceTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "DistanceTo: Invalid number of arguments (takes 1, game object)" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *myObj	= static_cast<CBaseObject *>( JS_GetPrivate( cx, jsObj ));

	CBaseObject *thisObj	= static_cast<CBaseObject *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( thisObj ) || !ValidateObject( myObj ))
	{
		MethodError( "DistanceTo: Invalid character" );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( GetDist( thisObj, myObj ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_Glow()
//|	Prototype	-	void Glow( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Attaches a light-emitting object to the object
//o------------------------------------------------------------------------------------------------o
JSBool CItem_Glow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSObject *mSock	= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock	= static_cast<CSocket *>( JS_GetPrivate( cx, mSock ));

	CItem *mItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	
	if( !ValidateObject( mItem ))
	{
		MethodError( "Glow: Invalid object" );
		mySock->SysMessage( 1095 ); // No item found there, only items can be made to glow.
		return JS_FALSE;
	}

	CChar *mChar = mySock->CurrcharObj();
	if( !ValidateObject( mChar ))
		return JS_FALSE;

	if( mItem->GetGlow() != INVALIDSERIAL )
	{
		mySock->SysMessage( 1097 ); // That object already glows!
		return JS_FALSE;
	}
	if( mItem->GetCont() == nullptr && FindItemOwner( mItem ) != mChar )
	{
		mySock->SysMessage( 1096 ); // You can't create glowing items in other people's packs or hands!
		return JS_FALSE;
	}

	mItem->SetGlowColour( mItem->GetColour() );

	CItem *glowItem = Items->CreateItem( mySock, mChar, 0x1647, 1, 0, OT_ITEM ); // spawn light emitting object
	if( glowItem == nullptr )
		return JS_FALSE;

	glowItem->SetDecayable( mItem->IsDecayable() );
	glowItem->SetDecayTime( mItem->GetDecayTime() );
	glowItem->SetName( "glower" );
	glowItem->SetMovable( 2 );

	mItem->SetGlow( glowItem->GetSerial() );
	Items->GlowItem( mItem );

	mChar->Update( mySock );
	mySock->SysMessage( 1098 ); // Item is now glowing.

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_UnGlow()
//|	Prototype	-	void UnGlow( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes light-emitting object from the object
//o------------------------------------------------------------------------------------------------o
JSBool CItem_UnGlow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSObject *mSock	= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock	= static_cast<CSocket *>( JS_GetPrivate( cx, mSock ));

	CItem *mItem	= static_cast<CItem *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( mItem ))
	{
		MethodError( "UnGlow: Invalid item" );
		mySock->SysMessage( 1099 ); // No item found, only items can be made to unglow.
		return JS_FALSE;
	}

	CChar *mChar = mySock->CurrcharObj();
	if( !ValidateObject( mChar ))
		return JS_FALSE;

	if( mItem->GetCont() == nullptr && FindItemOwner( mItem ) != mChar )
	{
		mySock->SysMessage( 1100 ); // You can't unglow items in other people's packs or hands!
		return JS_FALSE;
	}

	CItem *glowItem = CalcItemObjFromSer( mItem->GetGlow() );
	if( mItem->GetGlow() == INVALIDSERIAL || !ValidateObject( glowItem ))
	{
		mySock->SysMessage( 1101 ); // That object doesn't glow!
		return JS_FALSE;
	}

	mItem->SetColour( mItem->GetGlowColour() );

	glowItem->Delete();
	mItem->SetGlow( INVALIDSERIAL );

	mChar->Update( mySock );
	mySock->SysMessage( 1102 ); // Item is no longer glowing.

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Gate()
//|	Prototype	-	void Gate( item )
//|					void Gate( x, y, z, world )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a gate to the location marked on an item, or to a specified set of coords
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Gate( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		MethodError( "Gate: Invalid number of arguments (takes 1; item/place, or 4; x y z world)" );
		return JS_FALSE;
	}

	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "Gate: Invalid source character" );
		return JS_FALSE;
	}

	SI16 destX = -1, destY = -1;
	SI08 destZ = -1;
	UI08 destWorld = 0;

	if( argc == 1 )
	{
		if( JSVAL_IS_OBJECT( argv[0] ))
		{
			JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
			CItem *mItem		= static_cast<CItem *>( JS_GetPrivate( cx, jsObj ));
			if( !ValidateObject( mItem ))
			{
				MethodError( "Gate: Invalid item passed" );
				return JS_FALSE;
			}

			destX		= mItem->GetTempVar( CITV_MOREX );
			destY		= mItem->GetTempVar( CITV_MOREY );
			destZ		= mItem->GetTempVar( CITV_MOREZ );
			destWorld	= mItem->GetTempVar( CITV_MORE );
		}
		else
		{
			UI16 placeNum = JSVAL_TO_INT( argv[0] );
			if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
			{
				GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
				destX		= toGoTo.x;
				destY		= toGoTo.y;
				destZ		= toGoTo.z;
				destWorld	= toGoTo.worldNum;
			}
		}
	}
	else
	{
		destX		= JSVAL_TO_INT( argv[0] );
		destY		= JSVAL_TO_INT( argv[1] );
		destZ		= JSVAL_TO_INT( argv[2] );
		destWorld	= JSVAL_TO_INT( argv[3] );
	}

	if( !Map->MapExists( destWorld ))
	{
		destWorld = mChar->WorldNumber();
	}

	SpawnGate( mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber(), destX, destY, destZ, destWorld );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Recall()
//|	Prototype	-	void Recall( item )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Character recalls to the location marked on an item
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Recall( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Recall: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}

	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "Recall: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= static_cast<CItem *>( JS_GetPrivate( cx, jsObj ));
	if( !ValidateObject( mItem ))
	{
		MethodError( "Recall: Invalid item passed" );
		return JS_FALSE;
	}

	SI16 destX = mItem->GetTempVar( CITV_MOREX ), destY = mItem->GetTempVar( CITV_MOREY );
	SI08 destZ = mItem->GetTempVar( CITV_MOREZ );
	UI08 destWorld = mItem->GetTempVar( CITV_MORE );

	if( !Map->MapExists( destWorld ))
	{
		destWorld = mChar->WorldNumber();
	}

	if( mChar->WorldNumber() != destWorld && mChar->GetSocket() != nullptr )
	{
		mChar->SetLocation( destX, destY, destZ, destWorld, mChar->GetInstanceId() );
		SendMapChange( destWorld, mChar->GetSocket() );
	}
	else
	{
		mChar->SetLocation( destX, destY, destZ, destWorld, mChar->GetInstanceId() );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Mark()
//|	Prototype	-	void Mark( item )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Marks item with character's current location
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Mark( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Mark: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}

	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "Mark: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= static_cast<CItem *>( JS_GetPrivate( cx, jsObj ));
	if( !ValidateObject( mItem ))
	{
		MethodError( "Mark: Invalid item passed" );
		return JS_FALSE;
	}

	mItem->SetTempVar( CITV_MOREX, mChar->GetX() );
	mItem->SetTempVar( CITV_MOREY, mChar->GetY() );
	mItem->SetTempVar( CITV_MOREZ, mChar->GetZ() );
	mItem->SetTempVar( CITV_MORE, mChar->WorldNumber() );


	if( mChar->GetRegion()->GetName()[0] != 0 )
	{
		mItem->SetName( oldstrutil::format( Dictionary->GetEntry( 684 ), mChar->GetRegion()->GetName().c_str() ));
	}
	else
	{
		mItem->SetName( Dictionary->GetEntry( 685 ));
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SetSkillByName()
//|	Prototype	-	bool SetSkillByName( "skillName", value )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the skill specified by name to the value specified (name must be the same
//|					as in skills.dfn, "ALLSKILLS" is also applicable.
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SetSkillByName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetSkillByName: Invalid number of arguments (takes 2, string, value)" );
		return JS_FALSE;
	}

	CChar *mChar			= static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	std::string skillName	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	UI16 value				= JSVAL_TO_INT( argv[1] );
	CSocket *mSock			= nullptr;
	if( !mChar->IsNpc() )
	{
		mSock = mChar->GetSocket();
	}
	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		if( skillName == cwmWorldState->skill[i].name )
		{
			mChar->SetBaseSkill( value, i );
			Skills->UpdateSkillLevel( mChar, i );

			if( mSock != nullptr )
			{
				mSock->UpdateSkill( i );
			}
			*rval = JSVAL_TRUE;
			return JS_TRUE;
		}
	}
	*rval = JSVAL_FALSE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Kill()
//|	Prototype	-	void Kill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kills the character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Kill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Kill: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "Kill: Invalid character passed" );
		return JS_FALSE;
	}

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( i );
		if( toExecute != nullptr )
		{
			SI08 retStatus = toExecute->OnDeathBlow( mChar, nullptr );

			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( retStatus == 0 )
				return JS_TRUE;
		}
	}

	HandleDeath( mChar, nullptr );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Resurrect()
//|	Prototype	-	void Resurrect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resurrects the character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Resurrect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Resurrect: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "Resurrect: Invalid character passed" );
		return JS_FALSE;
	}
	NpcResurrectTarget( mChar );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_Dupe()
//|	Prototype	-	void Dupe( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a duplicate of the item in character's pack
//o------------------------------------------------------------------------------------------------o
JSBool CItem_Dupe( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Dupe: Invalid number of arguments (takes 1 - socket/null)" );
		return JS_FALSE;
	}

	CItem *mItem	= static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	JSObject *jsObj	= JSVAL_TO_OBJECT( argv[0] );

	CSocket *mSock = nullptr;
	bool dupeInPack = true;

	if( jsObj == nullptr )
	{
		dupeInPack = false;
	}
	else
	{
		mSock = static_cast<CSocket *>( JS_GetPrivate( cx, jsObj ));
	}

	if( !ValidateObject( mItem ) || ( mSock == nullptr && dupeInPack ))
	{
		MethodError( "Dupe: Bad parameters passed. Either item or socket is invalid!" );
		return JS_FALSE;
	}

	JSObject *dupeItem = nullptr;
	if( dupeInPack && mSock != nullptr )
	{
		dupeItem = JSEngine->AcquireObject( IUE_ITEM, Items->DupeItem( mSock, mItem, mItem->GetAmount() ), JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	}
	else
	{
		CItem *dupeItemTemp = mItem->Dupe();
		dupeItem = JSEngine->AcquireObject( IUE_ITEM, dupeItemTemp, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	}

	*rval = OBJECT_TO_JSVAL( dupeItem );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Dupe()
//|	Prototype	-	void Dupe()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dupes specified character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Dupe( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Dupe: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	// Original character object
	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "Dupe: Invalid character object passed?" );
		return JS_FALSE;
	}

	// Duped character object
	CChar *dupeCharTemp = mChar->Dupe();
	if( !ValidateObject( dupeCharTemp ))
	{
		MethodError( "Dupe: Unable to duplicate character due to unknown error!" );
		return JS_FALSE;
	}

	// JS Object for duped character
	JSObject *dupeChar = nullptr;
	dupeChar = JSEngine->AcquireObject( IUE_CHAR, dupeCharTemp, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));

	*rval = OBJECT_TO_JSVAL( dupeChar );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Jail()
//|	Prototype	-	void Jail()
//|					void Jail( numSecsToJail )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Jails character, either for ~27 hours or for specified amount of time in seconds
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Jail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Jail: Invalid number of arguments (takes 0 or 1, seconds to Jail)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myChar ))
	{
		MethodError( "Jail: Invalid character" );
		return JS_FALSE;
	}

	SI32 numSecsToJail = 86400;
	if( argc == 1 )
	{
		numSecsToJail = static_cast<SI32>( JSVAL_TO_INT( argv[0] ));
	}

	JailSys->JailPlayer( myChar, numSecsToJail );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Release()
//|	Prototype	-	void Release()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Releases character from jail
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Release( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Release: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myChar ))
	{
		MethodError( "Jail: Invalid character" );
		return JS_FALSE;
	}

	JailSys->ReleasePlayer( myChar );
	return JS_TRUE;
}

void CPage( CSocket *s, const std::string& reason );
void GMPage( CSocket *s, const std::string& reason );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_Page()
//|	Prototype	-	void Page( pageType )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers a page based on provided pageType
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_Page( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Page: Invalid number of arguments (takes 1, pageType)" );
		return JS_FALSE;
	}

	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mySock == nullptr )
	{
		MethodError( "SysMessage: Invalid socket" );
		return JS_FALSE;
	}

	UI08 pageType = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));
	switch( pageType )
	{
		case 0: // Counselor page - Free Text
			CPage( mySock, "OTHER" );
			break;
		case 1: // GM page - Free Text
			GMPage( mySock, "OTHER" );
			break;
		case 2: // GM page, stuck
			GMPage( mySock, "Stuck" );
			break;
		case 3: // GM page, item problem
			GMPage( mySock, "Item Problem" );
			break;
		case 4: // GM page, harassment
			GMPage( mySock, "Harassment" );
			break;
		case 5: // GM page, report exploit
			GMPage( mySock, "Exploit" );
			break;
		default:
			break;
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Print()
//|	Prototype	-	void Print( string )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints a message in UOX3 console. Message should end with \n
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_Print( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Print: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	Console.Print( arg0.toString() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Log()
//|	Prototype	-	void Log( logMsg )
//|					void Log( logMsg, filename )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs a message either in default log file or in specified file
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_Log( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "Log: Invalid number of arguments (takes 1 or 2)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	JSEncapsulate arg1;
	if( argc == 1 )
	{
		Console.Log( arg0.toString() );
	}
	else
	{
		arg1.SetContext( cx, &( argv[1] ));
		Console.Log( arg0.toString(), arg1.toString() );
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Error()
//|	Prototype	-	void Error( errorMsg )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs an error-message in default error log file
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_Error( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Error: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	Console.Error( arg0.toString() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_Warning()
//|	Prototype	-	void Warning( errorMsg )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Logs a warning-message in default warnings log file
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_Warning( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Warning: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	Console.Warning( arg0.toString() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintSectionBegin()
//|	Prototype	-	void PrintSectionBegin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints a section separator in the console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_PrintSectionBegin( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "PrintSectionBegin: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.PrintSectionBegin();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnYellow()
//|	Prototype	-	void TurnYellow()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_TurnYellow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnYellow: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnYellow();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnRed()
//|	Prototype	-	void TurnRed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates yellow text in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_TurnRed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnRed: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnRed();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnGreen()
//|	Prototype	-	void TurnGreen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates green text in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_TurnGreen( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnGreen: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnGreen();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnBlue()
//|	Prototype	-	void TurnBlue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates blue text in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_TurnBlue( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnBlue: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnBlue();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnNormal()
//|	Prototype	-	void TurnNormal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates normal text in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_TurnNormal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnNormal: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnNormal();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_TurnBrightWhite()
//|	Prototype	-	void TurnBrightWhite()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Activates bright white text in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_TurnBrightWhite( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnBrightWhite: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnBrightWhite();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintDone()
//|	Prototype	-	void PrintDone()
//|					void PrintDone( bool )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints colored [done] message in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_PrintDone( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 && argc != 1 )
	{
		MethodError( "PrintDone: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	bool normalDone = true;
	if( argc != 0 )
	{
		JSEncapsulate encaps( cx, &( argv[0] ));
		normalDone = encaps.toBool();
	}
	if( normalDone )
	{
		Console.PrintDone();
	}
	else
	{
		messageLoop << MSG_PRINTDONE;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintFailed()
//|	Prototype	-	void PrintFailed()
//|					void PrintFailed( bool )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints colored [failed] message in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_PrintFailed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 && argc != 1 )
	{
		MethodError( "PrintFailed: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	bool normalFailed = true;
	if( argc != 0 )
	{
		JSEncapsulate encaps( cx, &( argv[0] ));
		normalFailed = encaps.toBool();
	}
	if( normalFailed )
	{
		Console.PrintFailed();
	}
	else
	{
		messageLoop << MSG_PRINTFAILED;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintPassed()
//|	Prototype	-	void PrintPassed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints colored [skipped] message in console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_PrintPassed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "PrintPassed: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.PrintPassed();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_ClearScreen()
//|	Prototype	-	void ClearScreen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears the console
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_ClearScreen( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "ClearScreen: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.ClearScreen();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_PrintBasedOnVal()
//|	Prototype	-	void PrintBasedOnVal( bool )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Prints [done] or [failed] based on provided boolean
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_PrintBasedOnVal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "PrintBasedOnVal: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	Console.PrintBasedOnVal( arg0.toBool() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_MoveTo()
//|	Prototype	-	void MoveTo( x, y )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Moves console cursor position to specified x, y location
//|	Notes		-	If you want the same line,  y == -1
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_MoveTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "MoveTo: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	JSEncapsulate arg1( cx, &( argv[1] ));
	Console.MoveTo( arg0.toInt(), arg1.toInt() );
	return JS_TRUE;
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
JSBool CConsole_PrintSpecial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "PrintSpecial: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	JSEncapsulate arg1( cx, &( argv[1] ));
	Console.PrintSpecial( arg0.toInt(), arg1.toString().c_str() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CConsole_BeginShutdown()
//|	Prototype	-	void BeginShutdown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Start the UOX3 shutdown sequence
//o------------------------------------------------------------------------------------------------o
JSBool CConsole_BeginShutdown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "BeginShutdown: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	messageLoop << MSG_SHUTDOWN;
	return JS_TRUE;
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
JSBool CConsole_Reload( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Reload: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &( argv[0] ));
	SI32 mArg = arg0.toInt();
	if( mArg < 0 || mArg > 8 )
	{
		MethodError( "Reload: Section to reload must be between 0 and 8" );
		return JS_FALSE;
	}
	messageLoop.NewMessage( MSG_RELOAD, oldstrutil::number( mArg ).c_str() );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpellMoveEffect()
//|	Prototype	-	void SpellMoveEffect( targetChar, mSpell )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the MOVEFX effect of the specified spell in SPELLS.DFN, going from the
//|					character to the target
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SpellMoveEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SpellMoveEffect: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	CSpellInfo *mySpell = static_cast<CSpellInfo *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[1] )));
	if( mySpell == nullptr )
	{
		MethodError( "SpellMoveEffect: Invalid spell" );
		return JS_FALSE;
	}

	CChar *source = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	CBaseObject *target = static_cast<CBaseObject *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( source ) || !ValidateObject( target ))
	{
		MethodError( "SpellMoveEffect: Invalid object passed" );
		return JS_FALSE;
	}

	CMagicMove temp = mySpell->MoveEffect();

	if( temp.Effect() != INVALIDID )
	{
		Effects->PlayMovingAnimation( source, target, temp.Effect(), temp.Speed(), temp.Loop(), ( temp.Explode() == 1 ));
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_SpellStaticEffect()
//|	Prototype	-	void SpellStaticEffect( mSpell )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays the STATFX effect of the specified spell in SPELLS.DFN, on the character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_SpellStaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SpellStaticEffect: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CChar *source = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( source ))
	{
		MethodError( "SpellStaticEffect: Invalid character passed" );
		return JS_FALSE;
	}

	CSpellInfo *mySpell = static_cast<CSpellInfo *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( mySpell == nullptr )
	{
		MethodError( "SpellStaticEffect: Invalid spell" );
		return JS_FALSE;
	}

	CMagicStat temp = mySpell->StaticEffect();

	if( temp.Effect() != INVALIDID )
	{
		Effects->PlayStaticAnimation( source, temp.Effect(), temp.Speed(), temp.Loop() );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_BreakConcentration()
//|	Prototype	-	void BreakConcentration()
//|					void BreakConcentration( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Break a caster's concentration. Provide socket as extra argument for player chars
//o------------------------------------------------------------------------------------------------o
JSBool CChar_BreakConcentration( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "BreakConcentration: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}

	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "BreakConcentration: Invalid character" );
		return JS_FALSE;
	}

	CSocket *mSock = nullptr;
	if( argc == 1 )
	{
		mSock = static_cast<CSocket *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
		if( mSock == nullptr )
		{
			MethodError( "BreakConcentration: Invalid socket" );
			return JS_FALSE;
		}
	}

	mChar->BreakConcentration( mSock );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_SendAddMenu()
//|	Prototype	-	bool SendAddMenu( menuNum )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send the Add-menu to the character
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_SendAddMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SendAddMenu: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mSock == nullptr )
	{
		MethodError( "SendAddMenu: Invalid socket" );
		return JS_FALSE;
	}

	UI16 menuNum = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));

	BuildAddMenuGump( mSock, menuNum );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_LockDown()
//|	Prototype	-	void LockDown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks item down (sets movable value to 3)
//o------------------------------------------------------------------------------------------------o
JSBool CItem_LockDown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "LockDown: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CItem *mItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	if( mItem == nullptr )
	{
		MethodError( "LockDown: Invalid item" );
		return JS_FALSE;
	}

	mItem->LockDown();
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_InitWanderArea()
//|	Prototype	-	void InitWanderArea()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes a WanderArea (10x10 box, or 10 radius circle) for the NPC.
//|					Will only work if they already have a wandermode set to box or circle.
//o------------------------------------------------------------------------------------------------o
JSBool CChar_InitWanderArea( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ) || !mChar->IsNpc() )
	{
		MethodError( "InitWanderArea: Invalid character" );
		return JS_FALSE;
	}
	InitializeWanderArea( mChar, 10, 10 );
	return JS_TRUE;
}

void NewCarveTarget( CSocket *s, CItem *i );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_Carve()
//|	Prototype	-	void Carve( socket )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes the character belonging to socket carve up a corpse
//o------------------------------------------------------------------------------------------------o
JSBool CItem_Carve( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Carve: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	CItem *toCarve = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( toCarve ))
	{
		MethodError( "Carve: Invalid item" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket *>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( mSock == nullptr )
	{
		MethodError( "Carve: Invalid socket" );
		return JS_FALSE;
	}

	NewCarveTarget( mSock, toCarve );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_GetTileName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the tile name of a specified tile (item)
//o------------------------------------------------------------------------------------------------o
JSBool CItem_GetTileName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "GetTileName: needs 0 arguments!" );
		return JS_FALSE;
	}

	CItem *mItem = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mItem ))
	{
		MethodError( "GetTileName: Invalid item" );
		return JS_FALSE;
	}

	std::string itemName = "";
	GetTileName(( *mItem ), itemName );

	JSString *tString;
	tString = JS_NewStringCopyZ( cx, itemName.c_str() );
	*rval = STRING_TO_JSVAL( tString );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_GetMultiCorner()
//|	Prototype	-	void GetMultiCorner( cornerId )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets coordinates for specified corner ("NW", "NE", "SW" or "SE") of multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_GetMultiCorner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "GetMultiCorner: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(GetMultiCorner) Invalid object referenced - multi required" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	UI08 cornerToFind = static_cast<UI08>( JSVAL_TO_INT( argv[0] ));
	SI16 x1 = 0;
	SI16 y1 = 0;
	SI16 x2 = 0;
	SI16 y2 = 0;

	Map->MultiArea( multiObject, x1, y1, x2, y2 );
	switch( cornerToFind )
	{
		case 0: // NW
			*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, ( std::to_string( x1 ) + "," + std::to_string( y1 )).c_str() ));
			break;
		case 1: // NE
			*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, ( std::to_string( x2 ) + "," + std::to_string( y1 )).c_str() ));
			break;
		case 2: // SW
			*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, ( std::to_string( x1 ) + "," + std::to_string( y2 )).c_str() ));
			break;
		case 3: // SE
		{
			*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, ( std::to_string( x2 ) + "," + std::to_string( y2 )).c_str() ));
			break;
		}
		default:
			break;
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_SecureContainer()
//|	Prototype	-	void SecureContainer( itemToSecure )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Secures a container in a multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_SecureContainer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SecureContainer: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(SecureContainer) Invalid object referenced - multi required" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(SecureContainer) Invalid Object passed" );
		return JS_FALSE;
	}

	CItem *itemToSecure = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToSecure ))
	{
		MethodError( "(SecureContainer) Invalid Object passed" );
		return JS_FALSE;
	}

	multiObject->SecureContainer( itemToSecure );
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_UnsecureContainer()
//|	Prototype	-	void UnsecureContainer( itemToUnsecure )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unsecures a secured container in a multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_UnsecureContainer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "UnsecureContainer: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(UnsecureContainer) Invalid object referenced - multi required" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(UnsecureContainer) Invalid Object passed" );
		return JS_FALSE;
	}

	CItem *itemToUnsecure = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToUnsecure ))
	{
		MethodError( "(UnsecureContainer) Invalid Object passed" );
		return JS_FALSE;
	}

	multiObject->UnsecureContainer( itemToUnsecure );
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_IsSecureContainer()
//|	Prototype	-	void IsSecureContainer( itemToCheck )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified item is a secure container in the multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_IsSecureContainer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsSecureContainer: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(IsSecureContainer) Invalid object referenced - multi required" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(IsSecureContainer) Invalid Object passed" );
		return JS_FALSE;
	}

	CItem *itemToCheck = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToCheck ))
	{
		MethodError( "(IsSecureContainer) Invalid Object passed" );
		return JS_FALSE;
	}

	bool isSecureContainer = multiObject->IsSecureContainer( itemToCheck );
	*rval = BOOLEAN_TO_JSVAL( isSecureContainer );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_LockDownItem()
//|	Prototype	-	void LockDownItem( itemToLockDown )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item in a multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_LockDownItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "LockDownItem: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(LockDownItem) Invalid multi object referenced" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(LockDownItem) Invalid item object passed" );
		return JS_FALSE;
	}

	CItem *itemToLockDown = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToLockDown ))
	{
		MethodError( "(LockDownItem) Invalid item object passed" );
		return JS_FALSE;
	}

	multiObject->LockDownItem( itemToLockDown );
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_ReleaseItem()
//|	Prototype	-	void ReleaseItem( itemToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Releases a locked down item in a multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_ReleaseItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ReleaseItem: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(ReleaseItem) Invalid multi object referenced" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(ReleaseItem) Invalid item object passed" );
		return JS_FALSE;
	}

	CItem *itemToRemove = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToRemove ))
	{
		MethodError( "(ReleaseItem) Invalid item object passed" );
		return JS_FALSE;
	}

	multiObject->ReleaseItem( itemToRemove );
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_AddTrashCont()
//|	Prototype	-	void AddTrashCont( itemToLockDown )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item in a multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_AddTrashCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddTrashCont: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(AddTrashCont) Invalid multi object referenced" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(AddTrashCont) Invalid item object passed" );
		return JS_FALSE;
	}

	CItem *itemToLockDown = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToLockDown ))
	{
		MethodError( "(AddTrashCont) Invalid item object passed" );
		return JS_FALSE;
	}

	multiObject->AddTrashContainer( itemToLockDown );
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_RemoveTrashCont()
//|	Prototype	-	void RemoveTrashCont( itemToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Releases a locked down item in a multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_RemoveTrashCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveTrashCont: Invalid number of arguments (1 required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(RemoveTrashCont) Invalid multi object referenced" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ))
	{
		MethodError( "(RemoveTrashCont) Invalid item object passed" );
		return JS_FALSE;
	}

	CItem *itemToRemove = static_cast<CItem*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( itemToRemove ))
	{
		MethodError( "(RemoveTrashCont) Invalid item object passed" );
		return JS_FALSE;
	}

	multiObject->RemoveTrashContainer( itemToRemove );
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

void KillKeys( SERIAL targSerial, SERIAL charSerial = INVALIDSERIAL );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_KillKeys()
//|	Prototype	-	void KillKeys()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kills all keys in the world associated with the particular multi
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_KillKeys( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 && argc != 1 )
	{
		MethodError( "KillKeys: Invalid number of arguments (0 or 1 (character) required)" );
		return JS_FALSE;
	}

	*rval = JSVAL_FALSE;
	CMultiObj *multiObject = static_cast<CMultiObj *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( multiObject ) || !multiObject->CanBeObjType( OT_MULTI ))
	{
		MethodError( "(KillKeys) Invalid multi object referenced" );
		return JS_FALSE;
	}

	if( argc == 1 )
	{
		JSObject *jsObj = JSVAL_TO_OBJECT( argv[0] );
		CChar *myObj = static_cast<CChar *>( JS_GetPrivate( cx, jsObj ));
		
		if( !ValidateObject( myObj ))
		{
			MethodError( "(KillKeys) Invalid character object provided" );
			return JS_FALSE;
		}
		KillKeys( multiObject->GetSerial(), myObj->GetSerial() );
	}
	else
	{
		KillKeys( multiObject->GetSerial() );
	}

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_FirstChar()
//|	Prototype	-	bool FirstChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns first char in the multi's list
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_FirstChar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "FirstChar: Invalid count of arguments :%d, needs :0 or 1", argc );
		return JS_FALSE;
	}
	CMultiObj *myObj = static_cast<CMultiObj*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "FirstChar: Invalid object assigned - multi required." );
		return JS_FALSE;
	}

	std::string listType;
	if( argc == 1 )
	{
		listType = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	}
	else
	{
		listType = "visitor"; // default for existing scripts that provide no argument
	}

	CChar *firstChar = nullptr;
	if( listType == "visitor" ) // All chars inside the multi
	{
		firstChar = myObj->GetCharsInMultiList()->First();
	}
	else if( listType == "owner" ) // Owners
	{
		firstChar = myObj->GetOwnersOfMultiList( true )->First(); // true to clear list before it is regenerated
	}
	else if( listType == "friend" ) // Friends
	{
		firstChar = myObj->GetFriendsOfMultiList( true )->First(); // true to clear list before it is regenerated
	}
	else if( listType == "guest" ) // Guests
	{
		firstChar = myObj->GetGuestsOfMultiList( true )->First(); // true to clear list before it is regenerated
	}
	else if( listType == "banned" ) // Banned
	{
		firstChar = myObj->GetBannedFromMultiList( true )->First(); // true to clear list before it is regenerated
	}
	else
	{
		MethodError( "FirstChar: Unknown listType provided. Supported listTypes: visitor (default), owner, friend or banned" );
		return JS_FALSE;
	}

	if( ValidateObject( firstChar ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_CHAR, firstChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
	{
		*rval = JSVAL_NULL;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_NextChar()
//|	Prototype	-	bool NextChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns next char in the multi's list
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_NextChar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "NextChar: Invalid count of arguments :%d, needs :0 or 1", argc );
		return JS_FALSE;
	}
	CMultiObj *myObj = static_cast<CMultiObj*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "NextChar: Invalid object assigned - multi required." );
		return JS_FALSE;
	}

	std::string listType;
	if( argc == 1 )
	{
		listType = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	}
	else
	{
		listType = "visitor"; // default for existing scripts that provide no argument
	}

	CChar *nextChar = nullptr;
	if( listType == "visitor" ) // All chars inside the multi
	{
		nextChar = myObj->GetCharsInMultiList()->Next();
	}
	else if( listType == "owner" ) // Owners
	{
		nextChar = myObj->GetOwnersOfMultiList()->Next();
	}
	else if( listType == "friend" ) // Friends
	{
		nextChar = myObj->GetFriendsOfMultiList()->Next();
	}
	else if( listType == "guest" ) // Guests
	{
		nextChar = myObj->GetGuestsOfMultiList()->Next();
	}
	else if( listType == "banned" ) // Banned
	{
		nextChar = myObj->GetBannedFromMultiList()->Next();
	}
	else
	{
		MethodError( "FinishedChars: Unknown listType provided. Supported listTypes: visitor (default), owner, friend or banned" );
		return JS_FALSE;
	}

	if( ValidateObject( nextChar ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_CHAR, nextChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
	{
		*rval = JSVAL_NULL;
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CMulti_FinishedChars()
//|	Prototype	-	bool FinishedChars()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if finished all characters in multi's list
//o------------------------------------------------------------------------------------------------o
JSBool CMulti_FinishedChars( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "FinishedChars: Invalid count of arguments :%d, needs :0 or 1", argc );
		return JS_FALSE;
	}
	CMultiObj *myObj = static_cast<CMultiObj*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "FinishedChars: Invalid object assigned - multi required." );
		return JS_FALSE;
	}

	//char *listType;
	std::string listType;
	if( argc == 1 )
	{
		listType = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	}
	else
	{
		listType = "visitor"; // default for existing scripts that provide no argument
	}

	if( listType == "visitor" ) // All chars inside the multi
	{
		*rval = BOOLEAN_TO_JSVAL( myObj->GetCharsInMultiList()->Finished() );
	}
	else if( listType == "owner" ) // Owners
	{
		*rval = BOOLEAN_TO_JSVAL( myObj->GetOwnersOfMultiList()->Finished() );
	}
	else if( listType == "friend" ) // Friends
	{
		*rval = BOOLEAN_TO_JSVAL( myObj->GetFriendsOfMultiList()->Finished() );
	}
	else if( listType == "guest" ) // Guests
	{
		*rval = BOOLEAN_TO_JSVAL( myObj->GetGuestsOfMultiList()->Finished() );
	}
	else if( listType == "banned" ) // Banned
	{
		*rval = BOOLEAN_TO_JSVAL( myObj->GetBannedFromMultiList()->Finished() );
	}
	else
	{
		MethodError( "FinishedChars: Unknown listType provided. Supported listTypes: visitor (default), owner, friend or banned" );
		return JS_FALSE;
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_CanSee()
//|	Prototype	-	bool CanSee( object )
//|					bool CanSee( x, y, z )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks Line of Sight from character to target object or location
//o------------------------------------------------------------------------------------------------o
JSBool CBase_CanSee( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 3 )
	{
		MethodError( "CanSee: Invalid number of arguments (takes 1, a char/item or 3, an x/y/z)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CSocket *mSock		= nullptr;
	CChar *mChar		= nullptr;

	// Let's validate the person seeing (socket/char)

	if( myClass.ClassName() == "UOXSocket" )
	{
		mSock = static_cast<CSocket *>( myClass.toObject() );
		if( mSock == nullptr )
		{
			MethodError( "CanSee: Passed an invalid Socket" );
			return JS_FALSE;
		}
		mChar = mSock->CurrcharObj();
		if( !ValidateObject( mChar ))
		{
			MethodError( "CanSee: Socket to look from has an invalid Character attached" );
			return JS_FALSE;
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "CanSee: Passed an invalid Character" );
			return JS_FALSE;
		}
		mSock = mChar->GetSocket();
	}

	// If we've reached this far, the socket/char is valid
	// Now let's validate the input!

	SI16 x = -1, y = -1;
	SI08 z = 0;
	SI08 zTop = 0;
	if( argc == 1 )	// we've been passed an item, character, or socket
	{
		JSEncapsulate myClass( cx, &( argv[0] ));

		if( myClass.ClassName() == "UOXSocket" )
		{
			CSocket *tSock = static_cast<CSocket *>( myClass.toObject() );
			if( tSock == nullptr )
			{
				MethodError( "CanSee: Passed an invalid Socket to look at" );
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			CChar *tChar = tSock->CurrcharObj();
			if( !ValidateObject( tChar ))
			{
				MethodError( "CanSee: Socket to look at has invalid Character attached" );
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			if( tChar->WorldNumber() != mChar->WorldNumber() )
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			x = tChar->GetX();
			y = tChar->GetY();
			z = tChar->GetZ();
		}
		else if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" )
		{
			CBaseObject *tObj = static_cast<CBaseObject *>( myClass.toObject() );
			if( !ValidateObject( tObj ))
			{
				MethodError( "CanSee: Object to look at is invalid" );
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			if( tObj->WorldNumber() != mChar->WorldNumber() )
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			if( tObj->CanBeObjType( OT_ITEM ))
			{
				if(( static_cast<CItem *>( tObj ))->GetCont() != nullptr )
				{
					MethodError( "CanSee: Object to look at cannot be in a container" );
					*rval = JSVAL_FALSE;
					return JS_TRUE;
				}

				// Include top of item
				zTop = Map->TileHeight( tObj->GetId() );
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
		x	= static_cast<SI16>( JSVAL_TO_INT( argv[0] ));
		y	= static_cast<SI16>( JSVAL_TO_INT( argv[1] ));
		z	= static_cast<SI08>( JSVAL_TO_INT( argv[2] ));
	}

	if( ValidateObject( mChar ))
	{
		*rval = BOOLEAN_TO_JSVAL( LineOfSight( mSock, mChar, x, y, z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false, zTop ));
	}
	else
	{
		*rval = BOOLEAN_TO_JSVAL( false );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_DisplayDamage()
//|	Prototype	-	void DisplayDamage( pUser, damage )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays specified damage value over character's head
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_DisplayDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CSocket_DisplayDamage) Invalid Number of Arguments %d, needs: 2", argc );
		return JS_TRUE;
	}

	CSocket *mSock = static_cast<CSocket *>( JS_GetPrivate( cx, obj ));
	JSEncapsulate myClass( cx, &( argv[0] ));

	if( myClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		MethodError( "CSocket_DisplayDamage: Passed an invalid Character" );
		return JS_FALSE;
	}

	CChar *mChar = static_cast<CChar *>( myClass.toObject() );
	if( !ValidateObject( mChar ))
	{
		MethodError( "(CSocket_DisplayDamage): Passed an invalid Character" );
		return JS_TRUE;
	}

	JSEncapsulate damage( cx, &( argv[1] ));

	CPDisplayDamage dispDamage(( *mChar ), static_cast<UI16>( damage.toInt() ));
	mSock->Send( &dispDamage );

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ReactOnDamage()
//|	Prototype	-	void ReactOnDamage( damageType )
//|					void ReactOnDamage( damageType, attacker )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Lets character react to damage taken
//o------------------------------------------------------------------------------------------------o
JSBool CChar_ReactOnDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "(CChar_ReactOnDamage) Invalid Number of Arguments %d, needs: 1 (damageType) or 2 (damageType and attacker)", argc );
		return JS_TRUE;
	}

	CChar *attacker	= nullptr;
	CChar *mChar	= static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "(CChar_ReactOnDamage): Operating on an invalid Character" );
		return JS_TRUE;
	}
	JSEncapsulate damage( cx, &( argv[0] ));

	if( argc >= 2 )
	{
		JSEncapsulate attackerClass( cx, &( argv[1] ));
		if( attackerClass.ClassName() != "UOXChar" ) // It must be a character!
		{
			MethodError( "CChar_ReactOnDamage: Passed an invalid Character" );
			return JS_FALSE;
		}

		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ))
		{
			attacker = nullptr;
		}
		else
		{
			attacker = static_cast<CChar *>( attackerClass.toObject() );
			if( !ValidateObject( attacker ))
			{
				MethodError( "(CChar_ReactOnDamage): Passed an invalid Character" );
				return JS_TRUE;
			}
		}
	}
	mChar->ReactOnDamage( static_cast<WeatherType>( damage.toInt() ), attacker );
	return JS_TRUE;
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
JSBool CChar_Damage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 4 )
	{
		MethodError( "(CChar_Damage) Invalid Number of Arguments %d, needs: 1 (amount), 2 (amount, damageType), 3 (amount, damageType and attacker) or 4 (amount, damageType, attacker and doRepsys)", argc );
		return JS_TRUE;
	}

	CChar *attacker	= nullptr;
	CChar *mChar	= static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "(CChar_Damage): Operating on an invalid Character" );
		return JS_TRUE;
	}
	JSEncapsulate damage( cx, &( argv[0] ));

	WeatherType element = PHYSICAL;
	if( argc >= 2 )
	{
		element = static_cast<WeatherType>( JSVAL_TO_INT( argv[1] ));
	}

	if( argc >= 3 )
	{
		JSEncapsulate attackerClass( cx, &( argv[2] ));
		if( attackerClass.ClassName() != "UOXChar" )	// It must be a character!
		{
			MethodError( "CChar_Damage: Passed an invalid Character" );
			return JS_FALSE;
		}

		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ))
		{
			attacker = nullptr;
		}
		else
		{
			attacker = static_cast<CChar *>( attackerClass.toObject() );
			if( !ValidateObject( attacker ))
			{
				MethodError( "(CChar_Damage): Passed an invalid Character" );
				return JS_TRUE;
			}
		}
	}
	bool doRepsys = false;
	if( argc >= 4 )
	{
		doRepsys = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );
	}
	[[maybe_unused]] bool retVal = mChar->Damage( damage.toInt(), element, attacker, doRepsys );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_InitiateCombat()
//|	Prototype	-	bool InitiateCombat( targetChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Attempts to initiate combat with target character
//o------------------------------------------------------------------------------------------------o
JSBool CChar_InitiateCombat( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(InitiateCombat) Invalid Number of Arguments %d, takes: 1 (targetChar))", argc );
		return JS_TRUE;
	}

	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "(InitiateCombat): Operating on an invalid Character" );
		return JS_TRUE;
	}

	CChar *ourTarget = static_cast<CChar*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( ourTarget ))
	{
		MethodError( "(InitiateCombat): Operating on an invalid Character" );
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( Combat->StartAttack( mChar, ourTarget ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_InvalidateAttacker()
//|	Prototype	-	bool InvalidateAttacker()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets the attacker attack so that it cancels attack setup.
//o------------------------------------------------------------------------------------------------o
JSBool CChar_InvalidateAttacker( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(InvalidateAttacker) Invalid Number of Arguments %d, takes: 0)", argc );
		return JS_TRUE;
	}

	CChar *mChar = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "(InvalidateAttacker): Operating on an invalid Character" );
		return JS_TRUE;
	}

	Combat->InvalidateAttacker( mChar );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_Heal()
//|	Prototype	-	void Heal( amount );
//|					void Heal( amount, healer );
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Heals a character, with optional argument to provide character who healed
//o------------------------------------------------------------------------------------------------o
JSBool CChar_Heal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "(CChar_Heal) Invalid Number of Arguments %d, needs: 1 (amount) or 2 (amount and healer)", argc );
		return JS_TRUE;
	}

	CChar *healer	= nullptr;
	CChar *mChar	= static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ))
	{
		MethodError( "(CChar_Heal): Operating on an invalid Character" );
		return JS_TRUE;
	}
	JSEncapsulate Heal( cx, &( argv[0] ));

	if( argc == 2 )
	{
		JSEncapsulate healerClass( cx, &( argv[1] ));
		if( healerClass.ClassName() != "UOXChar" )	// It must be a character!
		{
			MethodError( "CChar_Heal: Passed an invalid Character" );
			return JS_FALSE;
		}

		if( healerClass.isType( JSOT_VOID ) || healerClass.isType( JSOT_NULL ))
		{
			healer = nullptr;
		}
		else
		{
			healer	= static_cast<CChar *>( healerClass.toObject() );
			if( !ValidateObject( healer ))
			{
				MethodError( "(CChar_Heal): Passed an invalid Character" );
				return JS_TRUE;
			}
		}
	}

	mChar->Heal( static_cast<SI16>( Heal.toInt() ), healer );
	return JS_TRUE;
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
JSBool CBase_Resist( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "Resist: Invalid number of arguments (takes 1, the resist type or 2, the resist type and value to set)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar		= nullptr;
	CItem *mItem		= nullptr;

	// Let's validate the char/item

	if( myClass.ClassName() == "UOXItem" )
	{
		mItem	= static_cast<CItem *>( myClass.toObject() );
		if( !ValidateObject( mItem ))
		{
			MethodError( "Resist: Passed an invalid Item" );
			return JS_FALSE;
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		mChar	= static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "Resist: Passed an invalid Character" );
			return JS_FALSE;
		}
	}

	JSEncapsulate resistType( cx, &( argv[0] ));

	if( argc == 1 )
	{
		if( ValidateObject( mChar ))
		{
			*rval = INT_TO_JSVAL( mChar->GetResist( static_cast<WeatherType>( resistType.toInt() )));
		}
		else if( ValidateObject( mItem ))
		{
			*rval = INT_TO_JSVAL( mItem->GetResist( static_cast<WeatherType>( resistType.toInt() )));
		}
		else
		{
			*rval = JS_FALSE;
		}
	}
	if( argc == 2 )
	{
		*rval = JS_TRUE;
		JSEncapsulate value( cx, &( argv[1] ));
		if( ValidateObject( mChar ))
		{
			mChar->SetResist( static_cast<UI16>( value.toInt() ), static_cast<WeatherType>( resistType.toInt() ));
		}
		else if( ValidateObject( mItem ))
		{
			mItem->SetResist( static_cast<UI16>( value.toInt() ), static_cast<WeatherType>( resistType.toInt() ));
		}
		else
		{
			*rval = JS_FALSE;
		}
	}
	return JS_TRUE;
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
JSBool CChar_Defense( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3)
	{
		MethodError( "Defense: Invalid number of arguments (takes 3, the hit location, the resist type and if the armor should get damaged)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the char
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar	= static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "Defense: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "Defense: Passed an invalid Character" );
		return JS_FALSE;
	}

	JSEncapsulate hitLoc( cx, &( argv[0] ));
	JSEncapsulate resistType( cx, &( argv[1] ));
	JSEncapsulate doArmorDamage( cx, &( argv[2] ));

	*rval = INT_TO_JSVAL( Combat->CalcDef( mChar, static_cast<UI08>( hitLoc.toInt() ), doArmorDamage.toBool(), static_cast<WeatherType>( resistType.toInt() )));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_GetMoreVar()
//|	Prototype	-	int GetMoreVar( moreVarName, moreVarPart )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the value of the specified moreVarName's moreVarPart
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Valid moreVarName values: "more", "morex", "morey", "morez
//|					Valid moreVarPart values: 1, 2, 3, 4
//o------------------------------------------------------------------------------------------------o
JSBool CItem_GetMoreVar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "GetMoreVar: Invalid number of arguments (takes 2, the moreVarName (more, morex, morey or morez - and the moreVarPart (1 to 4))" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CItem *mItem = nullptr;

	// Let's validate the item
	if( myClass.ClassName() == "UOXItem" )
	{
		mItem	= static_cast<CItem *>( myClass.toObject() );
		if( !ValidateObject( mItem ))
		{
			MethodError( "GetMoreVar: Passed an invalid Item" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "GetMoreVar: Passed an invalid Item" );
		return JS_FALSE;
	}

	// Fetch data from the function arguments, and figure out which tempVar to get data from
	UI08 moreVar			= 0;
	UI08 moreVarPart		= static_cast<UI08>( JSVAL_TO_INT( argv[1] ));
	std::string moreVarName	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	if( moreVarName == "more" )
	{
		moreVar = 0;
	}
	else if( moreVarName == "morex" )
	{
		moreVar = 1;
	}
	else if( moreVarName == "morey" )
	{
		moreVar = 2;
	}
	else if( moreVarName == "morez" )
	{
		moreVar = 3;
	}
	else
	{
		MethodError( "GetMoreVar: Passed an invalid argument: tempVarName" );
		return JS_FALSE;
	}

	// Fetch the value of the moreVarPart and return it to the script
	*rval = INT_TO_JSVAL( mItem->GetTempVar( static_cast<CITempVars>( moreVar ), moreVarPart ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem_SetMoreVar()
//|	Prototype	-	int SetMoreVar( moreVar, moreVarPart, tempVarValue )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the value of the specified moreVar's moreVarPart to moreVarValue
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Valid moreVarName values: "more", "morex", "morey", "morez
//|					Valid moreVarPart values: 1, 2, 3, 4
//|					Valid moreVarValue values: 0 - 255
//o------------------------------------------------------------------------------------------------o
JSBool CItem_SetMoreVar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "SetMoreVar: Invalid number of arguments (takes 3, the moreVarName (more, morex, morey or morez); the moreVarPart (1 to 4) and the moreVarValue (0-255)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CItem *mItem = nullptr;

	// Let's validate the item
	if( myClass.ClassName() == "UOXItem" )
	{
		mItem = static_cast<CItem *>( myClass.toObject() );
		if( !ValidateObject( mItem ))
		{
			MethodError( "SetMoreVar: Passed an invalid Item" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "SetMoreVar: Passed an invalid Item" );
		return JS_FALSE;
	}

	// Fetch data from the function arguments, and figure out which moreVar to set data for
	UI08 moreVar			= 0;
	UI08 moreVarPart		= static_cast<UI08>( JSVAL_TO_INT( argv[1] ));
	UI08 moreVarValue		= static_cast<UI08>( JSVAL_TO_INT( argv[2] ));
	std::string moreVarName	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ));
	if( moreVarName == "more" )
	{
		moreVar = 0;
	}
	else if( moreVarName == "morex" )
	{
		moreVar = 1;
	}
	else if( moreVarName == "morey" )
	{
		moreVar = 2;
	}
	else if( moreVarName == "morez" )
	{
		moreVar = 3;
	}
	else
	{
		MethodError( "SetMoreVar: Passed an invalid argument: moreVarName" );
		return JS_FALSE;
	}

	// Fetch the value of the moreVarPart and return it to the script
	mItem->SetTempVar( static_cast<CITempVars>( moreVar ), moreVarPart, moreVarValue );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_AddScriptTrigger()
//|	Prototype	-	void AddScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a scriptTrigger to an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
JSBool CBase_AddScriptTrigger( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddScriptTrigger: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "Invalid Object assigned (AddScriptTrigger)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ))
	{
		MethodError( "That is not a valid script trigger! Only integers between 0-65535 are accepted." );
	}

	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	if( scriptId > 0 )
	{
		cScript *toExecute	= JSMapping->GetScript( scriptId );
		if( toExecute == nullptr )
		{
			MethodError( oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
			return JS_FALSE;
		}
		else
		{
			myObj->AddScriptTrigger( scriptId );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_RemoveScriptTrigger()
//|	Prototype	-	void RemoveScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a scriptTrigger from an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
JSBool CBase_RemoveScriptTrigger( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveScriptTrigger: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "Invalid Object assigned (RemoveScriptTrigger)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ))
	{
		MethodError( "That is not a valid script trigger! Only integers between 0-65535 are accepted." );
	}

	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	if( scriptId > 0 )
	{
		myObj->RemoveScriptTrigger( scriptId );
	}
	else
	{
		myObj->ClearScriptTriggers();
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBase_HasScriptTrigger()
//|	Prototype	-	void HasScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if object has a specific scriptTrigger in it's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
JSBool CBase_HasScriptTrigger( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "HasScriptTrigger: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( myObj ))
	{
		MethodError( "Invalid Object assigned (HasScriptTrigger)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ))
	{
		MethodError( "That is not a valid script trigger! Only integers between 0-65535 are accepted." );
	}

	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	if( scriptId > 0 && myObj->HasScriptTrigger( scriptId ))
	{
		*rval = BOOLEAN_TO_JSVAL( true );
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRegion_AddScriptTrigger()
//|	Prototype	-	void AddScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a scriptTrigger to an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
JSBool CRegion_AddScriptTrigger( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddScriptTrigger: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CTownRegion *myObj = static_cast<CTownRegion *>( JS_GetPrivate( cx, obj ));
	if( myObj == nullptr )
	{
		MethodError( "Invalid Object assigned (AddScriptTrigger)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ))
	{
		MethodError( "That is not a valid script trigger! Only integers between 0-65535 are accepted." );
		return JS_FALSE;
	}

	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	if( scriptId > 0 )
	{
		cScript *toExecute	= JSMapping->GetScript( scriptId );
		if( toExecute == nullptr )
		{
			MethodError( oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
			return JS_FALSE;
		}
		else
		{
			myObj->AddScriptTrigger( scriptId );
		}
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRegion_RemoveScriptTrigger()
//|	Prototype	-	void RemoveScriptTrigger( scriptTrigger )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a scriptTrigger from an object's list of scriptTriggers
//o------------------------------------------------------------------------------------------------o
JSBool CRegion_RemoveScriptTrigger( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveScriptTrigger: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CTownRegion *myObj = static_cast<CTownRegion *>( JS_GetPrivate( cx, obj ));
	if( myObj == nullptr )
	{
		MethodError( "Invalid Object assigned (RemoveScriptTrigger)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ))
	{
		MethodError( "That is not a valid script trigger! Only integers between 0-65535 are accepted." );
	}

	UI16 scriptId = static_cast<UI16>( JSVAL_TO_INT( argv[0] ));
	if( scriptId > 0 )
	{
		myObj->RemoveScriptTrigger( scriptId );
	}
	else
	{
		myObj->ClearScriptTriggers();
	}

	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_AddFriend()
//|	Prototype	-	bool Add( playerToAdd )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds player to an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
JSBool CChar_AddFriend( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddFriend: Invalid number of arguments (takes 1 - playerObject)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "AddFriend: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "AddFriend: Passed an invalid Character" );
		return JS_FALSE;
	}

	CChar *newFriend = static_cast<CChar*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( newFriend ))
	{
		MethodError( "(AddFriend) Invalid Object passed as function parameter" );
		return JS_FALSE;
	}

	*rval = BOOLEAN_TO_JSVAL( mChar->AddFriend( newFriend ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_RemoveFriend()
//|	Prototype	-	bool Add( playerToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes player from an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
JSBool CChar_RemoveFriend( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveFriend: Invalid number of arguments (takes 1 - playerObject)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "RemoveFriend: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "RemoveFriend: Passed an invalid Character" );
		return JS_FALSE;
	}

	CChar *friendToRemove = static_cast<CChar*>( JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] )));
	if( !ValidateObject( friendToRemove ))
	{
		MethodError( "(AddFriend) Invalid Object passed as function parameter" );
		return JS_FALSE;
	}

	*rval = BOOLEAN_TO_JSVAL( mChar->RemoveFriend( friendToRemove ));
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetFriendList()
//|	Prototype	-	bool GetFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
JSBool CChar_GetFriendList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "GetFriendList: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "GetFriendList: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "GetFriendList: Passed an invalid Character" );
		return JS_FALSE;
	}

	// Fetch actual friend list
	auto friendList = mChar->GetFriendList();

	// Prepare some temporary helper variables
	JSObject *jsFriendList = JS_NewArrayObject( cx, 0, nullptr );
	jsval jsTempFriend;

	// Loop through list of friends, and add each one to the JS ArrayObject
	int i = 0;
	for( auto &tempFriend : *friendList )
	{
		// Create a new JS Object based on character
		JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, tempFriend, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));

		// Convert JS Object to jsval
		jsTempFriend = OBJECT_TO_JSVAL( myObj );

		// Add jsval to ArrayObject
		JS_SetElement( cx, jsFriendList, i, &jsTempFriend );
		i++;
	}

	// Convert ArrayObject to jsval and pass it to script
	*rval = OBJECT_TO_JSVAL( jsFriendList );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_ClearFriendList()
//|	Prototype	-	bool ClearFriendList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears an NPC pet/follower's friend list
//o------------------------------------------------------------------------------------------------o
JSBool CChar_ClearFriendList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "ClearFriendList: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "ClearFriendList: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "ClearFriendList: Passed an invalid Character" );
		return JS_FALSE;
	}

	// Clear friend list
	mChar->ClearFriendList();

	*rval = BOOLEAN_TO_JSVAL( true );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_GetPetList()
//|	Prototype	-	bool GetPetList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of character's pets/followers
//o------------------------------------------------------------------------------------------------o
JSBool CChar_GetPetList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "GetPetList: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "GetPetList: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "GetPetList: Passed an invalid Character" );
		return JS_FALSE;
	}

	// Fetch actual friend list
	auto petList = mChar->GetPetList();

	// Prepare some temporary helper variables
	JSObject *jsPetList = JS_NewArrayObject( cx, 0, nullptr );
	jsval jsTempPet;

	// Loop through list of friends, and add each one to the JS ArrayObject
	int i = 0;
	for( const auto &pet : petList->collection() )
	{
		if( ValidateObject( pet ))
		{
			if( pet->GetOwnerObj() == mChar )
			{
				// Create a new JS Object based on character
				JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, pet, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));

				// Convert JS Object to jsval
				jsTempPet = OBJECT_TO_JSVAL( myObj );

				// Add jsval to ArrayObject
				JS_SetElement( cx, jsPetList, i, &jsTempPet );
				i++;
			}
		}
	}

	// Convert ArrayObject to jsval and pass it to script
	*rval = OBJECT_TO_JSVAL( jsPetList );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_HasBeenOwner()
//|	Prototype	-	bool HasBeenOwner( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether character pChar has previously owned the pet (is on pet owner list)
//o------------------------------------------------------------------------------------------------o
JSBool CChar_HasBeenOwner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "HasBeenOwner: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "HasBeenOwner: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "HasBeenOwner: Passed an invalid Character" );
		return JS_FALSE;
	}
	
	JSEncapsulate toCheck( cx, &( argv[0] ));
	CChar *pChar = static_cast<CChar *>( toCheck.toObject() );
	if( !ValidateObject( pChar ))
	{
		MethodError( "HasBeenOwner: Invalid Character passed as parameter" );
		return JS_FALSE;
	}

	bool hasBeenOwner = mChar->IsOnPetOwnerList( pChar );

	*rval = BOOLEAN_TO_JSVAL( hasBeenOwner );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar_CalculateControlChance()
//|	Prototype	-	int CalculateControlChance( pChar )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns chance (in values ranging from 0 to 1000) of pChar (player) successfully controlling mChar (pet)
//o------------------------------------------------------------------------------------------------o
JSBool CChar_CalculateControlChance( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "CalculateControlChance: Invalid number of arguments (takes 1 - pChar)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar = nullptr;

	// Let's validate the character
	if( myClass.ClassName() == "UOXChar" )
	{
		mChar = static_cast<CChar *>( myClass.toObject() );
		if( !ValidateObject( mChar ))
		{
			MethodError( "CalculateControlChance: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "CalculateControlChance: Passed an invalid Character" );
		return JS_FALSE;
	}

	JSEncapsulate toCheck( cx, &( argv[0] ));
	CChar *pChar = static_cast<CChar *>( toCheck.toObject() );
	if( !ValidateObject( pChar ))
	{
		MethodError( "CalculateControlChance: Invalid Character passed as parameter" );
		return JS_FALSE;
	}

	UI16 petControlChance = Skills->CalculatePetControlChance( mChar, pChar );

	*rval = INT_TO_JSVAL( petControlChance );
	return JS_TRUE;
}

// Party Methods
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CParty_Remove()
//|	Prototype	-	bool Remove( playerToRemove )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes player from party
//o------------------------------------------------------------------------------------------------o
JSBool CParty_Remove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Remove: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXParty" )
	{
		Party *ourParty = static_cast<Party *>( myClass.toObject() );
		if( ourParty == nullptr )
		{
			MethodError( "Remove: Invalid party" );
			return JS_FALSE;
		}

		JSEncapsulate toRemove( cx, &( argv[0] ));
		CChar *charToRemove = static_cast<CChar *>( toRemove.toObject() );
		if( !ValidateObject( charToRemove ))
		{
			MethodError( "Remove: Invalid character to remove" );
			return JS_FALSE;
		}
		*rval = BOOLEAN_TO_JSVAL( ourParty->RemoveMember( charToRemove ));
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CParty_Add()
//|	Prototype	-	bool Add( playerToAdd )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds player to party
//o------------------------------------------------------------------------------------------------o
JSBool CParty_Add( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Add: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );

	// let's setup our default return value here
	*rval = BOOLEAN_TO_JSVAL( false );

	if( myClass.ClassName() == "UOXParty" )
	{
		Party *ourParty = static_cast<Party *>( myClass.toObject() );
		if( ourParty == nullptr )
		{
			MethodError( "Add: Invalid party" );
			return JS_FALSE;
		}

		JSEncapsulate toAdd( cx, &( argv[0] ));
		CChar *charToAdd = static_cast<CChar *>( toAdd.toObject() );
		if( !ValidateObject( charToAdd ))
		{
			MethodError( "Add: Invalid character to add" );
			return JS_FALSE;
		}
		if( charToAdd->IsNpc() )
		{
			if( ourParty->IsNPC() )
			{
				*rval = BOOLEAN_TO_JSVAL( ourParty->AddMember( charToAdd ));
			}
			else
			{
				*rval = BOOLEAN_TO_JSVAL( false );
				MethodError( "Add: Adding NPC to a PC party attempted" );
			}
		}
		else
		{
			if( ourParty->IsNPC() )
			{
				*rval = BOOLEAN_TO_JSVAL( false );
				MethodError( "Add: Adding PC to a NPC party attempted" );
			}
			else
			{	// Send PC the invite
				// OK, what this means is that it's a PC party, and because it exists
				// we know that the leader needs to be active!  And if he's active,
				// he should ALSO be online

				CSocket *targSock = charToAdd->GetSocket();
				if( targSock != nullptr )
				{
					CChar *leader		= ourParty->Leader();
					CSocket *leaderSock = leader->GetSocket();

					if( leaderSock != nullptr )
					{
						CPPartyInvitation toSend;
						toSend.Leader( leader );
						targSock->Send( &toSend );
						targSock->SysMessage( 9002 ); // You have been invited to join a party, type /accept or /decline to deal with the invitation
						*rval = BOOLEAN_TO_JSVAL( true );
					}
					else
					{
						*rval = BOOLEAN_TO_JSVAL( false );
						MethodError( "Add: PC selected is not online" );
					}
				}
				else
				{
					*rval = BOOLEAN_TO_JSVAL( false );
					MethodError( "Add: PC selected is not online" );
				}
			}
		}
	}
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CParty_GetMember()
//|	Prototype	-	bool GetMember( indexPos )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets party member at specified index in list of party members
//o------------------------------------------------------------------------------------------------o
JSBool CParty_GetMember( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "GetMember: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXParty" )
	{
		Party *ourParty = static_cast<Party *>( myClass.toObject() );
		if( ourParty == nullptr )
		{
			MethodError( "GetMember: Invalid party" );
			return JS_FALSE;
		}

		JSEncapsulate toGetMember( cx, &( argv[0] ));
		size_t memberOffset = toGetMember.toInt();
		if( memberOffset < 0 || memberOffset >= ourParty->MemberList()->size() )
		{
			MethodError( "GetMember: Invalid character to get, index out of bounds" );
			*rval = JSVAL_NULL;
			return JS_TRUE;
		}
		CChar *mChar = (*( ourParty->MemberList() ))[memberOffset]->Member();
		if( mChar == nullptr )
		{
			*rval = JSVAL_NULL;
		}
		else
		{
			JSObject *myJSChar	= JSEngine->AcquireObject( IUE_CHAR, mChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
			*rval = OBJECT_TO_JSVAL( myJSChar );
		}
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_FirstTriggerWord()
//|	Prototype	-	int FirstTriggerWord()
//|	Date		-	June 12, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns first trigger word in the socket's list
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_FirstTriggerWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "FirstTriggerWord: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mySock == nullptr )
	{
		MethodError( "FirstTriggerWord: Invalid socket assigned." );
		return JS_FALSE;
	}
	UI16 trigWord = mySock->FirstTrigWord();
	*rval = INT_TO_JSVAL( trigWord );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_NextTriggerWord()
//|	Prototype	-	int FirstTriggerWord()
//|	Date		-	June 12, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns next trigger word in the socket's list
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_NextTriggerWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NextTriggerWord: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mySock == nullptr )
	{
		MethodError( "NextTriggerWord: Invalid socket assigned." );
		return JS_FALSE;
	}
	UI16 trigWord = mySock->NextTrigWord();
	*rval = INT_TO_JSVAL( trigWord );
	return JS_TRUE;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSocket_FinishedTriggerWords()
//|	Prototype	-	int FinishedTriggerWords()
//|	Date		-	June 12, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if finished all trigger words in the socket's list
//o------------------------------------------------------------------------------------------------o
JSBool CSocket_FinishedTriggerWords( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "FinishedTriggerWords: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CSocket *mySock = static_cast<CSocket*>( JS_GetPrivate( cx, obj ));
	if( mySock == nullptr )
	{
		MethodError( "FinishedTriggerWords: Invalid socket assigned." );
		return JS_FALSE;
	}

	*rval = BOOLEAN_TO_JSVAL( mySock->FinishedTrigWords() );
	return JS_TRUE;
}

