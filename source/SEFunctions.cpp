// Here are the functions that are exposed to the Script Engine
#include "uox3.h"
#include "cdice.h"
#include "SEFunctions.h"

#include "cGuild.h"
#include "combat.h"
#include "movement.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cRaces.h"
#include "commands.h"
#include "cMagic.h"
#include "skills.h"
#include "targeting.h"
#include "trigger.h"
#include "mapstuff.h"
#include "cScript.h"
#include "cEffects.h"

#ifndef va_start
	#include <cstdarg>
	//using namespace std;
#endif

#define __EXTREMELY_VERBOSE__

#ifdef __EXTREMELY_VERBOSE__
void DoSEErrorMessage( char *txt, ... )
{
	va_list argptr;
	char msg[512];
#ifdef __NONANSI_VASTART__
	va_start( argptr );
#else
	va_start( argptr, txt );
#endif
	vsprintf( msg, txt, argptr );
	va_end( argptr );
	Console.Error( 2, msg );
}
#else
void DoSEErrorMessage( char *txt, ... )
{
	return;
}
#endif

JSBool SE_ConsoleMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ConsoleMessage: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
 	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
 	// print the string to the console 
	if( test != NULL )
 		Console << test;
 	return JS_TRUE;
}
JSBool ScriptPrintNumber( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ScriptPrintNumber: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
 	long test = JSVAL_TO_INT( argv[0] );
 	// print the string to the console
 	Console << (SI32)test;
 	return JS_TRUE;
}
JSBool SetShowLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "SetShowLayer: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	cwmWorldState->SetDisplayLayers( ( JSVAL_TO_BOOLEAN( argv[0] ) == JS_TRUE ) );
	if( cwmWorldState->GetDisplayLayers() )
		Console << "Layer display enabled" << myendl;
	else
		Console << "Layer display disabled" << myendl;

	return JS_TRUE;
}
JSBool GetShowLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = BOOLEAN_TO_JSVAL( cwmWorldState->GetDisplayLayers() );
	return JS_TRUE;
}
JSBool SE_CalcSockFromChar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcSockFromChar: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}
	CHARACTER targChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "CalcSockFromChar: Invalid argument, invalid character (%i)", targChar );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( calcSocketFromChar( targChar ) );
	return JS_TRUE;
}


// Gump related functions

JSBool SE_Gump_CreateGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
		*rval = INT_TO_JSVAL( toGet->NewGumpList() );
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSBool SE_Gump_Delete( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "Gump_Delete: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
		toGet->RemoveGumpList( handle );
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSBool SE_Gump_AddCheckbox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "Gump_AddCheckbox: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[3] );
	SI16 x1 = (SI16)JSVAL_TO_INT( argv[4] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[5] );

	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddCheckbox: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "checkbox %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, x1, x2 );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}

JSBool SE_Gump_AddButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		DoSEErrorMessage( "Gump_AddButton: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[3] );
	SI16 x1 = (SI16)JSVAL_TO_INT( argv[4] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[5] );
	SI16 x3 = (SI16)JSVAL_TO_INT( argv[6] );

	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddButton: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "button %i %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, x1, x2, x3 );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}
JSBool SE_Gump_AddText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "Gump_AddText: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	SI16 x1 = (SI16)JSVAL_TO_INT( argv[3] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[4] );
 	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[5] ) );

	if( test == NULL )
	{
		DoSEErrorMessage( "Gump_AddText: Text is required" );
		return JS_FALSE;
	}

	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddText: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "text %i %i %i %i", tL, tR, x1, x2 );
		gList->one->push_back( temp );
		gList->two->push_back( test );
	}
	return JS_TRUE;
}

JSBool SE_Gump_AddTextEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 9 )
	{
		DoSEErrorMessage( "Gump_AddTextEntry: Invalid number of arguments (takes 9)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	SI16 x1 = (SI16)JSVAL_TO_INT( argv[3] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[4] );
	SI16 x3 = (SI16)JSVAL_TO_INT( argv[5] );
	SI16 x4 = (SI16)JSVAL_TO_INT( argv[6] );
	SI16 x5 = (SI16)JSVAL_TO_INT( argv[7] );
 	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[8] ) );

	if( test == NULL )
	{
		DoSEErrorMessage( "Gump_AddTextEntry: Text is required" );
		return JS_FALSE;
	}

	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddTextEntry: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "textentry %i %i %i %i %i %i %i", tL, tR, x1, x2, x3, x4, x5 );
		gList->one->push_back( temp );
		gList->two->push_back( test );
	}
	return JS_TRUE;
}

JSBool SE_Gump_AddRadio( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "Gump_AddRadio: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[3] );
	SI16 bL = (SI16)JSVAL_TO_INT( argv[4] );
	SI16 bR = (SI16)JSVAL_TO_INT( argv[5] );

	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddRadio: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "radio %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, bL, bR );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}
JSBool SE_Gump_AddPage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "Gump_AddPage: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 pNum = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddPage: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "page %i", pNum );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}
JSBool SE_Gump_AddPicture( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "Gump_AddPicture: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[3] );
	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddPicture: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "tilepic %i %i %i", tL, tR, gImage );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}
JSBool SE_Gump_AddGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "Gump_AddGump: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[3] );
	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddGump: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "gumppic %i %i %i", tL, tR, gImage );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}
JSBool SE_Gump_AddBackground( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "Gump_AddBackground: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI32 handle = (SI32)JSVAL_TO_INT( argv[0] );
	SI16 tL = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[2] );
	SI16 bL = (SI16)JSVAL_TO_INT( argv[3] );
	SI16 bR = (SI16)JSVAL_TO_INT( argv[4] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[5] );
	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
	{
		SEGump *gList = toGet->GetGumpList( handle );
		if( gList == NULL )
		{
			DoSEErrorMessage( "Gump_AddBackground: Couldn't find gump associated with handle (%i)", handle );
			return JS_FALSE;
		}
		char temp[256];
		sprintf( temp, "resizepic %i %i %i %i %i", tL, tR, gImage, bL, bR );
		gList->one->push_back( temp );
	}
	return JS_TRUE;
}

JSBool SE_Gump_SendToSocket( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "Gump_SendToSock: Invalid number of arguments (takes 2, index, socket)" );
		return JS_FALSE;
	}
	SI32 index = (SI32)JSVAL_TO_INT( argv[0] );
	UOXSOCKET s = (UOXSOCKET)JSVAL_TO_INT( argv[1] );
	if( s == -1 || s >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "Gump_SendToSock: Invalid socket (%i)", s );
		return JS_FALSE;
	}

	UI16 mTrig = Trigger->FindObject( obj );
	cScript *toGet = Trigger->GetScript( mTrig );
	if( toGet != NULL )
		toGet->SendGumpList( index, s );
	return JS_TRUE;
}

// Character related functions
JSBool SE_GetTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetTarget: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}
	CHARACTER targChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "GetTarget: Invalid argument, invalid character (%i)", targChar );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( chars[targChar].GetTarg() );
	return JS_TRUE;
}
JSBool SE_RunTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "RunTo: Invalid number of arguments (takes 4, character, targ x and y, and maxSteps)" );
		return JS_FALSE;
	}
	CHARACTER cMove = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( cMove == INVALIDSERIAL || cMove >= cwmWorldState->GetCMem() )
	{
		return JS_FALSE;
	}
	UI16 gx = (UI16)JSVAL_TO_INT( argv[1] );
	UI16 gy = (UI16)JSVAL_TO_INT( argv[2] );
	UI08 maxSteps = (UI08)JSVAL_TO_INT( argv[3] );

	Movement->PathFind( &chars[cMove], gx, gy, true, maxSteps );
	return JS_TRUE;
}

// Effect related functions
JSBool SE_DoStaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 5 )
	{
		DoSEErrorMessage( "DoStaticEffect: Invalid number of arguments (takes 5 as char or 6 as item)" );
		return JS_FALSE;
	}

	UI08 targType = (UI08)JSVAL_TO_INT( argv[4] );
	if( ( targType == 0 && argc != 5 ) || ( targType == 1 && argc != 6 ) )	// a char, and not 5 params or an item and not 6
	{
		DoSEErrorMessage( "DoStaticEffect: Invalid number of arguments (takes 5 as char or 6 as item)" );
		return JS_FALSE;
	}
	UI32 targThing = JSVAL_TO_INT( argv[0] );
	
	if( targType == 0 && ( targThing == INVALIDSERIAL || targThing > cwmWorldState->GetCMem() ) )
	{
		DoSEErrorMessage( "DoStaticEffect: Invalid object (character %i)", targThing );
		return JS_FALSE;
	}
	if( targType == 1 && ( targThing == INVALIDSERIAL || targThing > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "DoStaticEffect: Invalid object (item %i)", targThing );
		return JS_FALSE;
	}

	UI16 effectID = (UI16)JSVAL_TO_INT( argv[1] );
	UI08 speed = (UI08)JSVAL_TO_INT( argv[2] );
	UI08 loop = (UI08)JSVAL_TO_INT( argv[3] );

	if( argc == 6 )
	{
		bool explode = ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
		Effects->staticeffect( &items[targThing], effectID, speed, loop, explode );
	}
	else
		Effects->staticeffect( &chars[targThing], effectID, speed, loop );

	return JS_TRUE;
}
JSBool SE_DoTempEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 7 )
	{
		DoSEErrorMessage( "DoTempEffect: Invalid number of arguments (takes 7 or 8)" );
		return JS_FALSE;
	}
	long iType = JSVAL_TO_INT( argv[0] );
	long targNum = JSVAL_TO_INT( argv[3] );
	UI08 more1 = (UI08)JSVAL_TO_INT( argv[4] );
	UI08 more2 = (UI08)JSVAL_TO_INT( argv[5] );
	UI08 more3 = (UI08)JSVAL_TO_INT( argv[6] );

	JSObject *myitemptr = NULL;
	CItem *myItemPtr = NULL;

	if( argc == 8 )
	{
		myitemptr = JSVAL_TO_OBJECT( argv[7] );
		myItemPtr = (CItem *)JS_GetPrivate( cx, myitemptr );
	}

	JSObject *mysrc = JSVAL_TO_OBJECT( argv[1] );
	CChar *mysrcChar = (CChar*)JS_GetPrivate( cx, mysrc );

	if( mysrcChar == NULL )
	{
		DoSEErrorMessage( "DoTempEffect: Invalid src" );
		return JS_FALSE;
	}
	
	if( iType == 0 )	// character
	{
		JSObject *mydestc = JSVAL_TO_OBJECT( argv[2] );
		CChar *mydestChar = (CChar*)JS_GetPrivate( cx, mydestc );

		if( mydestChar == NULL )
		{
			DoSEErrorMessage( "DoTempEffect: Invalid target " );
			return JS_FALSE;
		}
		if( argc == 8 )
			Effects->tempeffect( mysrcChar, mydestChar, static_cast<SI08>(targNum), more1, more2, more3, myItemPtr );
		else
			Effects->tempeffect( mysrcChar, mydestChar, static_cast<SI08>(targNum), more1, more2, more3 );
	}
	else
	{
		JSObject *mydesti = JSVAL_TO_OBJECT( argv[2] );
		CItem *mydestItem = (CItem *)JS_GetPrivate( cx, mydesti );

		if( mydestItem == NULL )
		{
			DoSEErrorMessage( "DoTempEffect: Invalid target " );
			return JS_FALSE;
		}
		if( argc == 8 )
			Effects->tempeffect( mysrcChar, mydestItem, static_cast<SI08>(targNum), more1, more2, more3 );
		else
			Effects->tempeffect( mysrcChar, mydestItem, static_cast<SI08>(targNum), more1, more2, more3 );
	}
	return JS_TRUE;
}

// Magic related functions
JSBool SE_CastSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "CastSpell: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UI32 iObject = JSVAL_TO_INT( argv[0] );
	if( iObject == INVALIDSERIAL || iObject > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "CastSpell: Invalid character (%i)", iObject );
		return JS_FALSE;
	}
	SI08 spellCast = (SI08)JSVAL_TO_INT( argv[1] );
	if( chars[iObject].IsNpc() )
	{
		chars[iObject].SetSpellCast( spellCast );
		Magic->CastSpell( NULL, &chars[iObject] );
	}
	else
	{
		cSocket *sock = calcSocketObjFromChar( iObject );
		Magic->SelectSpell( sock, spellCast );
	}
	return JS_TRUE;
}

// Speech related functions
JSBool SE_BroadcastMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "BroadcastMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSString *targMessage = JS_ValueToString( cx, argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );
	if( trgMessage == NULL )
	{
		DoSEErrorMessage( "BroadcastMessage: Invalid string (%s)", trgMessage );
		return JS_FALSE;
	}
	sysbroadcast( trgMessage );
	return JS_TRUE;
}

// Attack related functions
JSBool SE_AttackTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "AttackTarget: Invalid number of arguments (takes 1, CHARACTER)" );
		return JS_FALSE;
	}
	CHARACTER attacking = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( attacking == INVALIDSERIAL || attacking >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "AttackTarget: Invalid character (%i)", attacking );
		return JS_FALSE;
	}
	npcAttackTarget( &chars[chars[attacking].GetTarg()], &chars[attacking] );
	return JS_TRUE;
}
JSBool SE_Attack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "Attack: Invalid number of arguments (takes 2, both CHARACTERs)" );
		return JS_FALSE;
	}
	CHARACTER attacking = (CHARACTER)JSVAL_TO_INT( argv[0] );
	CHARACTER toAttack  = (CHARACTER)JSVAL_TO_INT( argv[1] );
	if( attacking == INVALIDSERIAL || attacking >= cwmWorldState->GetCMem() || toAttack == INVALIDSERIAL || toAttack >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "Attack: Invalid attacker (%i) or target (%i)", attacking, toAttack );
		return JS_FALSE;
	}
	npcAttackTarget( &chars[toAttack], &chars[attacking] );
	return JS_TRUE;
}


JSBool SE_FindNearestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindNearestTarget: Invalid number of arguments (takes 1, CHARACTER)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindNearestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	R32 distance = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset+counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					distance = static_cast<R32>(fabs( (tempChar->GetLocation() - src->GetLocation()).MagSquared() ));
				}
				else if( tempChar != src )
				{
					R32 nextDistance = static_cast<R32>(fabs( (tempChar->GetLocation() - src->GetLocation()).MagSquared() ));
					if( nextDistance < distance )
					{
						distance = nextDistance;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}
JSBool SE_FindStrongestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindStrongestTarget: Invalid number of arguments (takes 1, CHARACTER)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindStrongestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int strength = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{

			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					strength = tempChar->GetStrength();
				}
				else if( tempChar != src )
				{
					int nextStrength = tempChar->GetStrength();
					if( nextStrength > strength )
					{
						strength = nextStrength;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}
JSBool SE_FindWeakestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindWeakestTarget: Invalid number of arguments (takes 1, CHARACTER)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindWeakestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int strength = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{

			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					strength = tempChar->GetStrength();
				}
				else if( tempChar != src )
				{
					int nextStrength = tempChar->GetStrength();
					if( nextStrength < strength )
					{
						strength = nextStrength;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}
JSBool SE_FindFirstTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindFirstTarget: Invalid number of arguments (takes 1, CHARACTER)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindFirstTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( tempChar != src )
				{
					*rval = INT_TO_JSVAL( calcCharFromSer( tempChar->GetSerial() ) );
					MapArea->PopChar();
					return JS_TRUE;
				}
			}
			MapArea->PopChar();
		}
	}
	*rval = INT_TO_JSVAL( INVALIDSERIAL );
	return JS_TRUE;
}
JSBool SE_FindLastTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindLastTarget: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindLastTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				nearest = tempChar;
			}
			MapArea->PopChar();
		}
	}
	if( nearest != NULL )
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	else
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	return JS_TRUE;
}
JSBool SE_FindSmartestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindSmartestTarget: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindSmartestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int strength = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					strength = tempChar->GetIntelligence();
				}
				else if( tempChar != src )
				{
					int nextStrength = tempChar->GetIntelligence();
					if( nextStrength > strength )
					{
						strength = nextStrength;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}
JSBool SE_FindDumbestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindDumbestTarget: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindDumbestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int strength = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					strength = tempChar->GetIntelligence();
				}
				else if( tempChar != src )
				{
					int nextStrength = tempChar->GetIntelligence();
					if( nextStrength < strength )
					{
						strength = nextStrength;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}
JSBool SE_FindQuickestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindQuickestTarget: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindQuickestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int strength = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					strength = tempChar->GetDexterity();
				}
				else if( tempChar != src )
				{
					int nextStrength = tempChar->GetDexterity();
					if( nextStrength > strength )
					{
						strength = nextStrength;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}
JSBool SE_FindSlowestTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FindSlowestTarget: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "FindSlowestTarget: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	cBaseObject *src = &chars[toCheck];
	cBaseObject *nearest = NULL;
	int strength = 0;
	int xOffset = MapRegion->GetGridX( src->GetX() );
	int yOffset = MapRegion->GetGridY( src->GetY() );
	UI08 worldNumber = src->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( nearest == NULL && tempChar != src )
				{
					nearest = tempChar;
					strength = tempChar->GetDexterity();
				}
				else if( tempChar != src )
				{
					int nextStrength = tempChar->GetDexterity();
					if( nextStrength < strength )
					{
						strength = nextStrength;
						nearest = tempChar;
					}
				}
			}
			MapArea->PopChar();
		}
	}
	if( nearest == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( calcCharFromSer( nearest->GetSerial() ) );
	return JS_TRUE;
}


JSBool SE_GetName2( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "GetName2: Invalid number of arguments (takes 2, object and object type)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetIMem() )
	{
		DoSEErrorMessage( "GetName2: Invalid object (%i)", targChar );
		return JS_FALSE;
	}

	JSString *strSpeech = NULL;
	strSpeech = JS_NewStringCopyZ( cx, items[targChar].GetName2() );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

JSBool SE_SetName2( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetName2: Invalid number of arguments (takes 3, object, new name and object type)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( test == NULL )
	{
		DoSEErrorMessage( "SetName2: No name!" );
		return JS_FALSE;
	}
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetIMem() )
	{
		DoSEErrorMessage( "SetName2: Invalid object (%i)", targChar );
		return JS_FALSE;
	}
	items[targChar].SetName2( test );
	return JS_TRUE;
}


JSBool SE_GetCharPack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetCharPack: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}
	CHARACTER targChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetCMem() ) 
	{
		DoSEErrorMessage( "GetCharPack: Invalid character (%i)", targChar );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( getPack( &chars[targChar] ) );
	return JS_TRUE;
}

JSBool SE_GetLowDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetLowDamage: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}
	UI32 targItem = (UI32)JSVAL_TO_INT( argv[0] );
	UI08 targType = (UI08)JSVAL_TO_INT( argv[1] );
	if( targItem == INVALIDSERIAL || ( targType && targItem > cwmWorldState->GetIMem() ) || ( !targType && targItem > cwmWorldState->GetCMem() ) )
	{
		DoSEErrorMessage( "GetLowDamage: Invalid object (%i) or type ", targItem, targType );
		return JS_FALSE;
	}
	if( targType )
		*rval = INT_TO_JSVAL( items[targItem].GetLoDamage() );
	else
		*rval = INT_TO_JSVAL( chars[targItem].GetLoDamage() );
	return JS_TRUE;
}

JSBool SE_GetHiDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetHiDamage: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}
	UI32 targItem = (UI32)JSVAL_TO_INT( argv[0] );
	UI08 targType = (UI08)JSVAL_TO_INT( argv[1] );
	if( targItem == INVALIDSERIAL || ( targType && targItem > cwmWorldState->GetIMem() ) || ( !targType && targItem > cwmWorldState->GetCMem() ) )
	{
		DoSEErrorMessage( "GetHiDamage: Invalid object (%i) or type ", targItem, targType );
		return JS_FALSE;
	}
	if( targType )
		*rval = INT_TO_JSVAL( items[targItem].GetHiDamage() );
	else
		*rval = INT_TO_JSVAL( chars[targItem].GetHiDamage() );
	return JS_TRUE;
}

JSBool SE_GetDef( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetDef: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}
	ITEM targItem = (ITEM)JSVAL_TO_INT( argv[0] );
	if( targItem == INVALIDSERIAL || targItem > cwmWorldState->GetIMem() ) 
	{
		DoSEErrorMessage( "GetDef: Invalid item (%i)", targItem );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( items[targItem].GetDef() );
	return JS_TRUE;
}

JSBool SE_CalcItemFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		DoSEErrorMessage( "CalcItemFromSer: Invalid number of arguments (takes 1 or 4)" );
		return JS_FALSE;
	}
	SERIAL targSerial;
	if( argc == 1 )
		targSerial = (SERIAL)JSVAL_TO_INT( argv[0] );
	else
		targSerial = calcserial( (UI08)JSVAL_TO_INT( argv[0] ), (UI08)JSVAL_TO_INT( argv[1] ), (UI08)JSVAL_TO_INT( argv[2] ), (UI08)JSVAL_TO_INT( argv[3] ) );

	CItem *newItem = calcItemObjFromSer( targSerial );
	UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
	cScript *myScript = Trigger->GetScript( myScpTrig );
	JSObject *myObj = myScript->AcquireObject( IUE_ITEM );
	JS_SetPrivate( cx, myObj, newItem );
	*rval = OBJECT_TO_JSVAL( myObj );
	return JS_TRUE;
}

JSBool SE_CalcCharFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		DoSEErrorMessage( "CalcCharFromSer: Invalid number of arguments (takes 1 or 4)" );
		return JS_FALSE;
	}
	SERIAL targSerial = INVALIDSERIAL;
	if( argc == 1 )
		targSerial = (SERIAL)JSVAL_TO_INT( argv[0] );
	else
		targSerial = calcserial( (UI08)JSVAL_TO_INT( argv[0] ), (UI08)JSVAL_TO_INT( argv[1] ), (UI08)JSVAL_TO_INT( argv[2] ), (UI08)JSVAL_TO_INT( argv[3] ) );

	CChar *newChar = calcCharObjFromSer( targSerial );
	UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
	cScript *myScript = Trigger->GetScript( myScpTrig );
	JSObject *myObj = myScript->AcquireObject( IUE_CHAR );
	JS_SetPrivate( cx, myObj, newChar );
	*rval = OBJECT_TO_JSVAL( myObj );
	return JS_TRUE;
}

JSBool SE_DistanceTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "DistanceTo: Invalid number of arguments (takes 4, object, object, object type, object type)" );
		return JS_FALSE;
	}
	UI32 objects[2];
	long objTypes[2];
	objects[0] = JSVAL_TO_INT( argv[0] );
	objects[1] = JSVAL_TO_INT( argv[1] );
	objTypes[0] = JSVAL_TO_INT( argv[2] );
	objTypes[1] = JSVAL_TO_INT( argv[3] );
	for( UI08 iCounter = 0; iCounter <= 1; iCounter++ )
		if( objects[iCounter] == INVALIDSERIAL || ( objTypes[iCounter] == 0 && objects[iCounter]> cwmWorldState->GetCMem() ) || ( objTypes[iCounter] && objects[iCounter]> cwmWorldState->GetIMem() ) )
		{
			DoSEErrorMessage( "DistanceTo: Invalid object (%i) or object type (%i)", objects[iCounter], objTypes[iCounter] );
			return JS_FALSE;
		}
	cBaseObject *objPointers[2];
	for( UI08 oCounter = 0; oCounter <= 1; oCounter++ )
	{
		if( objTypes[oCounter] == 0 )
			objPointers[oCounter] = &chars[objects[oCounter]];
		else
			objPointers[oCounter] = &items[objects[oCounter]];
	}
	*rval = INT_TO_JSVAL( getDist( objPointers[0], objPointers[1] ) );
	return JS_TRUE;
}
JSBool SE_DoDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "DoDamage: Invalid number of arguments (takes 3, object, damage, object type)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	UI08 targType = (UI08)JSVAL_TO_INT( argv[2] );
	if( targChar == INVALIDSERIAL || ( targType == 0 && targChar > cwmWorldState->GetCMem() ) || ( targType && targChar > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "DoDamage: Invalid object (%i) or object type (%i)", targChar, targType );
		return JS_FALSE;
	}
	SI16 targDamage = (SI16)JSVAL_TO_INT( argv[1] );
	if( targType )
		items[targChar].IncHP( -targDamage );
	else
		chars[targChar].IncHP( -targDamage );
	return JS_TRUE;
}

JSBool SE_SetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetSerial: Invalid number of arguments (takes 3, object, serial, object type)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	SERIAL targSerial = (SERIAL)JSVAL_TO_INT( argv[1] );
	UI08 targType = (UI08)JSVAL_TO_INT( argv[2] );
	if( targChar == INVALIDSERIAL || ( targType == 0 && targChar > cwmWorldState->GetCMem() ) || ( targType && targChar > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "SetSerial: Invalid object (%i) or object type (%i)", targChar, targType );
		return JS_FALSE;
	}
	if( targType )
		items[targChar].SetSerial( targSerial, targChar );
	else
		chars[targChar].SetSerial( targSerial, targChar );
	return JS_TRUE;
}

JSBool SE_SetTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SetTarget: Invalid number of arguments (takes 2, character, target)" );
		return JS_FALSE;
	}
	CHARACTER targChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	CHARACTER target = (CHARACTER)JSVAL_TO_INT( argv[1] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "SetTarget: Invalid character (%i)", targChar );
		return JS_FALSE;
	}
	chars[targChar].SetTarg( target );
	return JS_TRUE;
}

JSBool SE_CalcDefense( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcDefense: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}
	CHARACTER targChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "CalcDefense: Invalid character (%i)", targChar );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( Combat->calcDef( &chars[targChar], 0, true ) );
	return JS_TRUE;
}

JSBool SE_CalcAttack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcAttack: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER targChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || targChar > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "CalcAttack: Invalid character (%i)", targChar );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( Combat->calcAtt( &chars[targChar] ) );
	return JS_TRUE;
}

JSBool SE_DoMovingEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 6 )
	{
		DoSEErrorMessage( "DoMovingEffect: Invalid number of arguments (takes 6->8 or 8->10)" );
		return JS_FALSE;
	}
	JSObject *srcObj	= JSVAL_TO_OBJECT( argv[0] );
	cBaseObject *src	= (cBaseObject *)JS_GetPrivate( cx, srcObj );
	if( src == NULL )
	{
		DoSEErrorMessage( "DoMovingEffect: Invalid source object" );
		return JS_FALSE;
	}
	bool targLocation = false;
	UI08 offset = 0;
	UI16 targX = 0, targY = 0;
	SI08 targZ = 0;
	cBaseObject *trg = NULL;
	if( JSVAL_IS_INT( argv[1] ) )
	{	// Location moving effect
		targLocation = true;
		offset = true;
		targX = (UI16)JSVAL_TO_INT( argv[1] );
		targY = (UI16)JSVAL_TO_INT( argv[2] );
		targZ = (SI08)JSVAL_TO_INT( argv[3] );
	}
	else
	{
		JSObject *trgObj	= JSVAL_TO_OBJECT( argv[1] );
		trg					= (cBaseObject *)JS_GetPrivate( cx, trgObj );
		if( trg == NULL )
		{
			DoSEErrorMessage( "DoMovingEffect: Invalid target object" );
			return JS_FALSE;
		}
	}
	UI16 effect = (UI16)JSVAL_TO_INT( argv[2+offset] );
	UI08 speed = (UI08)JSVAL_TO_INT( argv[3+offset] );
	UI08 loop = (UI08)JSVAL_TO_INT( argv[4+offset] );
	bool explode = ( JSVAL_TO_BOOLEAN( argv[5+offset] ) == JS_TRUE );
	UI32 hue = 0, renderMode = 0;
	if( argc - offset >= 7 ) // there's at least 7/9 parameters
		hue = (UI32)JSVAL_TO_INT( argv[6+offset] );
	if( argc - offset >= 8 ) // there's at least 8/10 parameters
		renderMode = (UI32)JSVAL_TO_INT( argv[7+offset] );

	if( targLocation )
		Effects->movingeffect( src, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode );
	else
		Effects->movingeffect( src, trg, effect, speed, loop, explode, hue, renderMode );
	return JS_TRUE;
}

JSBool SE_SetAtt( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetAtt: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	ITEM targItem = (ITEM)JSVAL_TO_INT( argv[0] );
	SI16 minAtt = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 maxAtt = (SI16)JSVAL_TO_INT( argv[2] );
	if( targItem == INVALIDSERIAL || targItem > cwmWorldState->GetIMem() )
	{
		DoSEErrorMessage( "SetAtt: Invalid item (%i)", targItem );
		return JS_FALSE;
	}
	items[targItem].SetLoDamage( minAtt );
	items[targItem].SetHiDamage( maxAtt );
	return JS_TRUE;
}

JSBool SE_SetDef( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SetDef: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	ITEM targItem = (ITEM)JSVAL_TO_INT( argv[0] );
	UI16 targDef = (UI16)JSVAL_TO_INT( argv[1] );
	if( targItem == INVALIDSERIAL || targItem > cwmWorldState->GetIMem() )
	{
		DoSEErrorMessage( "SetDef: Invalid item (%i)", targItem );
		return JS_FALSE;
	}
	items[targItem].SetDef( targDef );
	return JS_TRUE;
}

JSBool SE_RandomNumber( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RandomNumber: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	long loNum = JSVAL_TO_INT( argv[0] );
	long hiNum = JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( RandomNum( loNum, hiNum ) );
	return JS_TRUE;
}

JSBool SE_GetString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		DoSEErrorMessage( "GetString: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	int length = -1;
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	int offset = JSVAL_TO_INT( argv[1] );
	if( argc == 3 )
		length = JSVAL_TO_INT( argv[2] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "GetString: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	char toReturn[128];
	cSocket *mSock = calcSocketObjFromSock( sock );
	if( length != -1 )
	{
		strncpy( toReturn, (char *)&(mSock->Buffer())[offset], length );
		toReturn[length] = 0;
	}
	else
		strcpy( toReturn, (char *)&(mSock->Buffer())[offset] );

	JSString *strSpeech = NULL;
	strSpeech = JS_NewStringCopyZ( cx, toReturn );
	*rval = STRING_TO_JSVAL( strSpeech );

	return JS_TRUE;
}

JSBool SE_GetDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "GetDWord: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "GetDWord: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	int offset = JSVAL_TO_INT( argv[1] );
	cSocket *mSock = calcSocketObjFromSock( sock );
	*rval = INT_TO_JSVAL( mSock->GetDWord( offset ) );
	return JS_TRUE;
}

JSBool SE_CreateBuffer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_DestroyBuffer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_SendBuffer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_SetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetByte: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "SetByte: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	int offset = JSVAL_TO_INT( argv[1] );
	UI08 byteToSet = (UI08)JSVAL_TO_INT( argv[2] );
	cSocket *mSock = calcSocketObjFromSock( sock );
	mSock->SetByte( offset, byteToSet );
	return JS_TRUE;
}

JSBool SE_SetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "SetWord: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	int offset = JSVAL_TO_INT( argv[1] );
	short byteToSet = (short)JSVAL_TO_INT( argv[2] );
	cSocket *mSock = calcSocketObjFromSock( sock );
	mSock->SetWord( offset, byteToSet );
	return JS_TRUE;
}

JSBool SE_SetDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetDWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "SetDWord: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	int offset = JSVAL_TO_INT( argv[1] );
	long byteToSet = JSVAL_TO_INT( argv[2] );
	cSocket *mSock = calcSocketObjFromSock( sock );
	mSock->SetDWord( offset, byteToSet );
	return JS_TRUE;
}

JSBool SE_SetString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetString: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "SetString: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	int offset = JSVAL_TO_INT( argv[1] );
 	char *trgMessage = JS_GetStringBytes( JS_ValueToString( cx, argv[2] ) );
	if( trgMessage == NULL )
	{
		DoSEErrorMessage( "SetString: No string to set" );
		return JS_FALSE;
	}
	cSocket *mSock = calcSocketObjFromSock( sock );
	strcpy( (char *)&(mSock->Buffer())[offset], trgMessage );
	return JS_TRUE;
}

JSBool SE_ReadBytes( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "ReadBytes: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "ReadBytes: invalid socket (%i)", sock );
		return JS_FALSE;
	}
	int bCount = JSVAL_TO_INT( argv[1] );
	cSocket *nSock = calcSocketObjFromSock( sock );
	if( nSock != NULL )
		nSock->Receive( bCount );
	return JS_TRUE;
}

const UI08 reagASH		= 0;
const UI08 reagDRAKE	= 1;
const UI08 reagSHADE	= 2;
const UI08 reagMOSS		= 3;
const UI08 reagSILK		= 4;
const UI08 reagGINSENG	= 5;
const UI08 reagGARLIC	= 6;
const UI08 reagPEARL	= 7;

JSBool SE_UseReagant( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "UseReagant: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	UI08 reagID = (UI08)JSVAL_TO_INT( argv[1] );
	UI16 amount = (UI16)JSVAL_TO_INT( argv[2] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "UseReagant: invalid character (%i)", player );
		return JS_FALSE;
	}
	UI16 realID = 0;
	switch( reagID )
	{
	case reagASH:		realID = 0x0F8C;	break;
	case reagDRAKE:		realID = 0x0F86;	break;
	case reagSHADE:		realID = 0x0F88;	break;
	case reagMOSS:		realID = 0x0F7B;	break;
	case reagSILK:		realID = 0x0F8D;	break;
	case reagGINSENG:	realID = 0x0F85;	break;
	case reagGARLIC:	realID = 0x0F84;	break;
	case reagPEARL:		realID = 0x0F7A;	break;
	}
	if( realID != 0 )
	{
		UI32 foundAmount = GetAmount( &chars[player], realID );
		if( foundAmount >= amount )
		{
			DeleteQuantity( &chars[player], realID, amount );
			*rval = JS_TRUE;
		}
		else
			*rval = JS_FALSE;
	}
	else
		*rval = JS_FALSE;
	return JS_TRUE;
}

JSBool SE_UseReagants( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 9 )
	{
		DoSEErrorMessage( "UseReagants: Invalid number of arguments (takes 9)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "UseReagants: Invalid character (%i)", player );
		return JS_FALSE;
	}
	UI08 reqAmount;
	reag_st reqReags;
	for( UI08 iCounter = 0; iCounter < 8; iCounter++ )
	{
		reqAmount = (UI08)JSVAL_TO_INT( argv[1+iCounter] );
		if( reqAmount == 0 )
			continue;
		switch( iCounter )
		{
		case reagASH:		reqReags.ash = reqAmount;		break;
		case reagDRAKE:		reqReags.drake = reqAmount;		break;
		case reagSHADE:		reqReags.shade = reqAmount;		break;
		case reagMOSS:		reqReags.moss = reqAmount;		break;
		case reagSILK:		reqReags.silk = reqAmount;		break;
		case reagGINSENG:	reqReags.ginseng = reqAmount;	break;
		case reagGARLIC:	reqReags.garlic = reqAmount;	break;
		case reagPEARL:		reqReags.pearl = reqAmount;		break;
		}
	}
	if( !Magic->CheckReagents( &chars[player], &reqReags ) )
		*rval = JS_FALSE;
	else
	{
		Magic->DelReagents( &chars[player], reqReags );
		*rval = JS_TRUE;
	}
	return JS_TRUE;
}

JSBool SE_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "AddSpell: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "AddSpell: Invalid character (%i)", player );
		return JS_FALSE;
	}
	UI08 spellNum = (UI08)JSVAL_TO_INT( argv[1] );
	CItem *sBook = FindItemOfType( &chars[player], 9 );
	if( sBook != NULL )
		Magic->AddSpell( sBook, spellNum );
	return JS_TRUE;
}

JSBool SE_RemoveSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RemoveSpell: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "RemoveSpell: Invalid character (%i)", player );
		return JS_FALSE;
	}
	UI08 spellNum = (UI08)JSVAL_TO_INT( argv[1] );
	CItem *sBook = FindItemOfType( &chars[player], 9 );
	if( sBook != NULL )
		Magic->RemoveSpell( sBook, spellNum );
	return JS_TRUE;
}

JSBool SE_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SpellFail: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UOXSOCKET socket = JSVAL_TO_INT( argv[0] );
	CHARACTER player = JSVAL_TO_INT( argv[1] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "SpellFail: Invalid character (%i)", player );
		return JS_FALSE;
	}
	// can't use the magic subsystem stuff, it deletes reags :<
	Effects->staticeffect( &chars[player], 0x3735, 0, 30 );
	Effects->PlaySound( &chars[player], 0x005C );
	if( socket != -1 )
		npcEmote( calcSocketObjFromSock( socket ), &chars[player], 771, false );
	return JS_TRUE;
}

JSBool SE_GlowItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_MakeMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "MakeMenu: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSObject *mSock = JSVAL_TO_OBJECT( argv[0] );
	cSocket *mySock = (cSocket *)JS_GetPrivate( cx, mSock );
	if( mySock == NULL )
	{
		DoSEErrorMessage( "MakeMenu: invalid socket" );
		return JS_FALSE;
	}
	int menu = JSVAL_TO_INT( argv[1] );
	Skills->NewMakeMenu( mySock, menu, 0 );
	return JS_TRUE;
}

JSBool SE_MakeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "MakeItem: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	JSObject *mSock = JSVAL_TO_OBJECT( argv[0] );
	cSocket *sock = (cSocket *)JS_GetPrivate( cx, mSock );
	JSObject *mChar = JSVAL_TO_OBJECT( argv[1] );
	CChar *player = (CChar *)JS_GetPrivate( cx, mChar );
	if( player == NULL )
	{
		DoSEErrorMessage( "MakeItem: Invalid character" );
		return JS_FALSE;
	}
	UI16 itemMenu = (UI16)JSVAL_TO_INT( argv[2] );
	createEntry *toFind = Skills->FindItem( itemMenu );
	if( toFind == NULL )
	{
		DoSEErrorMessage( "MakeItem: Invalid make item (%i)", itemMenu );
		return JS_FALSE;
	}
	Skills->MakeItem( *toFind, player, sock, itemMenu );
	return JS_TRUE;
}

JSBool SE_CalcRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "CalcRank: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[4] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "CalcRank: Invalid character (%i)", player );
		return JS_FALSE;
	}
	int minRank = JSVAL_TO_INT( argv[0] );
	int maxRank = JSVAL_TO_INT( argv[1] );
	int minSkill = JSVAL_TO_INT( argv[2] );
	int skillNum = JSVAL_TO_INT( argv[3] );

	int rk_range, rank, randnum;
	R32 sk_range, randnum1;
	
	rk_range = maxRank - minRank;
	sk_range = (R32) 50.00 + chars[player].GetSkill( skillNum ) - minSkill;
	if( sk_range <= 0 ) 
		rank = minRank;
	else if( sk_range >= 1000 ) 
		rank = maxRank;
	randnum = RandomNum( 0, 1000 );
	if( randnum <= sk_range ) 
		rank = maxRank;
	else
	{
		randnum1 = (R32)( RandomNum( 0, 1000 ) ) - (( randnum - sk_range ) / ( 11 - cwmWorldState->ServerData()->GetSkillLevel() ) );
		rank = (int)( randnum1*rk_range)/1000;
		rank += minRank - 1;
		if( rank > maxRank ) 
			rank = maxRank;
		if( rank < minRank ) 
			rank = minRank;
	}
	*rval = INT_TO_JSVAL( rank );
	return JS_TRUE;
}

JSBool SE_CalcCharFromSock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcCharFromSock: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXSOCKET sock = JSVAL_TO_INT( argv[0] );
	if( sock == -1 || sock > cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "CalcCharFromSock: invalid socket (%i)", sock );
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
		return JS_TRUE;
	}
	cSocket *mSock = calcSocketObjFromSock( sock );
	*rval = INT_TO_JSVAL( mSock->Currchar() );
	return JS_TRUE;
}

JSBool SE_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "ApplyRank: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	ITEM i = JSVAL_TO_INT( argv[0] );
	if( i == INVALIDSERIAL || i > cwmWorldState->GetIMem() )
	{
		DoSEErrorMessage( "ApplyRank: Invalid item (%i)", i );
		return JS_FALSE;
	}
	int rank = JSVAL_TO_INT( argv[1] );
	Skills->ApplyRank( NULL, &items[i], rank );
	return JS_TRUE;
}

JSBool SE_ExecuteCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_CommandLevelReq( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CommandLevelReq: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( test == NULL )
	{
		DoSEErrorMessage( "CommandLevelReq: Invalid command name" );
		return JS_FALSE;
	}
	cmdtable_mapentry *details = Commands->CommandDetails( test );
	if( details == NULL )
		*rval = INT_TO_JSVAL( 255 );
	else
		*rval = INT_TO_JSVAL( details->cmdLevelReq );
	return JS_TRUE;
}

JSBool SE_CommandExists( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CommandExists: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( test == NULL )
	{
		DoSEErrorMessage( "CommandExists: Invalid command name" );
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( Commands->CommandExists( test ) );
	return JS_TRUE;
}

JSBool SE_FirstCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	const char *tVal = Commands->FirstCommand();
	JSString *strSpeech = NULL;
	if( tVal == NULL )
		strSpeech = JS_NewStringCopyZ( cx, "" );
	else
		strSpeech = JS_NewStringCopyZ( cx, tVal );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

JSBool SE_NextCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	const char *tVal = Commands->NextCommand();
	JSString *strSpeech = NULL;
	if( tVal == NULL )
		strSpeech = JS_NewStringCopyZ( cx, "" );
	else
		strSpeech = JS_NewStringCopyZ( cx, tVal );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

JSBool SE_FinishedCommandList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = BOOLEAN_TO_JSVAL( Commands->FinishedCommandList() );
	return JS_TRUE;
}

JSBool SE_RegisterCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_UnregisterCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_RegisterKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_UnregisterKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_RegisterPacketHandler( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_UnregisterPacketHandler( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_GetHour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	bool ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();
	UI08 hour = cwmWorldState->ServerData()->GetServerTimeHours();
	if( ampm )
		*rval = INT_TO_JSVAL( hour + 12 );
	else
		*rval = INT_TO_JSVAL( hour );
	return JS_TRUE;
}

JSBool SE_GetMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI08 minute = cwmWorldState->ServerData()->GetServerTimeMinutes();
	*rval = INT_TO_JSVAL( minute );
	return JS_TRUE;
}

JSBool SE_GetDay( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	short day = cwmWorldState->ServerData()->GetServerTimeDay();
	*rval = INT_TO_JSVAL( day );
	return JS_TRUE;
}

JSBool SE_GetSecondsPerUOMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->GetSecondsPerUOMinute() );
	return JS_TRUE;
}

JSBool SE_GetLightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetLightLevel: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "GetLightLevel: Invalid character (%i)", toCheck );
		return JS_FALSE;
	}
	if( chars[toCheck].GetFixedLight() != 255 )
		*rval = INT_TO_JSVAL( chars[toCheck].GetFixedLight() );
	else
	{
		UI08 charRegion = chars[toCheck].GetRegion();
		weathID toExamine = region[charRegion]->GetWeather();
		if( toExamine != 255 )
		{
			CWeather *weathCheck = Weather->Weather( toExamine );
			if( weathCheck == NULL )
				*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->GetWorldLightCurrentLevel() );
			else
				*rval = INT_TO_JSVAL( weathCheck->CurrentLight() );
		}
		else
			*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->GetWorldLightCurrentLevel() );
	}
	return JS_TRUE;
}

JSBool SE_GetCurrentClock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->GetUICurrentTime() );
	return JS_TRUE;
}

JSBool SE_WalkTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "WalkTo: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	CHARACTER cMove = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( cMove == INVALIDSERIAL || cMove >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "WalkTo: Invalid character (%i)", cMove );
		return JS_FALSE;
	}
	UI16 gx = (UI16)JSVAL_TO_INT( argv[1] );
	UI16 gy = (UI16)JSVAL_TO_INT( argv[2] );
	UI08 maxSteps = (UI08)JSVAL_TO_INT( argv[3] );

	Movement->PathFind( &chars[cMove], gx, gy, false, maxSteps );
	return JS_TRUE;
}

JSBool SE_SpawnNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 && argc != 6 )
	{
		DoSEErrorMessage( "SpawnNPC: Invalid number of arguments (takes 4 or 6)" );
		return JS_FALSE;
	}
	cSocket *s = NULL;
	CChar *cMade = NULL;
	if( argc == 4 )
	{
		UOXSOCKET bpSocket = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
		int region = (int)JSVAL_TO_INT( argv[1] );
		cSpawnRegion *spawnReg = NULL;
		if( region != -1 )
			spawnReg = spawnregion[region];
		char *npcNum = JS_GetStringBytes( JS_ValueToString( cx, argv[2] ) );
		s = calcSocketObjFromSock( bpSocket );
		UI08 worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
		cMade = Npcs->AddNPC(s,spawnReg,npcNum,worldNumber);
	}
	else
	{
		UOXSOCKET Socket = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
		char *nnpcNum = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
		UI16 x = (UI16)JSVAL_TO_INT( argv[2] );
		UI16 y = (UI16)JSVAL_TO_INT( argv[3] );
		SI08 z = (SI08)JSVAL_TO_INT( argv[4] );
		UI08 wrld = (UI08)JSVAL_TO_INT( argv[5] );
		s = calcSocketObjFromSock( Socket );
		cMade = Npcs->AddNPCxyz( s, nnpcNum, x, y, z, wrld );
	}
	if( cMade != NULL )
	{
		UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
		cScript *myScript = Trigger->GetScript( myScpTrig );
		JSObject *myobj = myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myobj, cMade );
		*rval = OBJECT_TO_JSVAL( myobj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_SpawnItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 && argc != 9 )
	{
		DoSEErrorMessage( "SpawnItem: Invalid number of arguments (takes 4 or 9)" );
		return JS_FALSE;
	}
	CItem *newItem = NULL;
	JSObject *mSock = JSVAL_TO_OBJECT( argv[0] );
	JSObject *mChar = JSVAL_TO_OBJECT( argv[1] );
	cSocket *mySock = (cSocket *)JS_GetPrivate( cx, mSock );
	CChar *myChar	= (CChar *)JS_GetPrivate( cx, mChar );
	if( argc == 4 )
	{
	 	char *bpSectNumber = JS_GetStringBytes( JS_ValueToString( cx, argv[2] ) );
		bool bpRandValue = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );
		newItem = Items->SpawnItemToPack( mySock, myChar, bpSectNumber, bpRandValue );
	}
	else
	{
		int amount = (int)JSVAL_TO_INT( argv[2] );
	 	char *itemName = JS_GetStringBytes( JS_ValueToString( cx, argv[3] ) );
		bool stackable = ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );	
		UI16 itemID = (UI16)JSVAL_TO_INT( argv[5] );
		UI16 colour = (UI16)JSVAL_TO_INT( argv[6] );
		bool inPack = ( JSVAL_TO_BOOLEAN( argv[7] ) == JS_TRUE );
		bool bSend = ( JSVAL_TO_BOOLEAN( argv[8] ) == JS_TRUE );
		newItem = Items->SpawnItem( mySock, myChar, amount, itemName, stackable, itemID, colour, inPack, bSend );
	}
	if( newItem != NULL )
	{
		UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
		cScript *myScript = Trigger->GetScript( myScpTrig );

		JSObject *myObj = myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, newItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_SubStringSearch( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SubStringSearch: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
 	char *ssSearching = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
 	char *ssSearch    = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( ssSearching == NULL || ssSearch == NULL )	// no valid search string, or string to search
	{
		DoSEErrorMessage( "SubStringSearch: Invalid search or sub string" );
		return JS_FALSE;
	}
	const long ingLength = strlen( ssSearching );
	const long chLength = strlen( ssSearch );
	char *uSearching = new char[ingLength + 1];
	char *uSearch = new char[chLength + 1];
	strcpy( uSearching, ssSearching );
	strupr( uSearching );
	strcpy( uSearch, ssSearch );
	strupr( uSearch );
	*rval = BOOLEAN_TO_JSVAL( strstr( uSearching, uSearch ) != NULL );
	delete [] uSearching;
	delete [] uSearch;
	return JS_TRUE;
}
JSBool SE_IsCriminal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "IsCriminal: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "IsCriminal: Invalid character (%i)", player );
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( chars[player].IsCriminal() );
	return JS_TRUE;
}
JSBool SE_IsMurderer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "IsMurderer: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "IsMurderer: Invalid character (%i)", player );
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( chars[player].IsMurderer() );
	return JS_TRUE;
}
JSBool SE_IsInnocent( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "IsInnocent: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "IsInnocent: Invalid character (%i)", player );
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( chars[player].IsInnocent() );
	return JS_TRUE;
}
JSBool SE_GetFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetFlag: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "GetFlag: Invalid character (%i)", player );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( chars[player].GetFlag() );
	return JS_TRUE;
}
JSBool SE_MakeCriminal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "MakeCriminal: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "MakeCriminal: Invalid character (%i)", player );
		return JS_FALSE;
	}
	criminal( &chars[player] );
	return JS_TRUE;
}
JSBool SE_MakeInnocent( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "MakeInnocent: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "MakeInnocent: Invalid character (%i)", player );
		return JS_FALSE;
	}
	chars[player].SetCrimFlag( -1 );
	setcharflag( &chars[player] );
	return JS_TRUE;
}
JSBool SE_GetMurderThreshold( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->GetRepMaxKills() );
	return JS_TRUE;
}
JSBool SE_GetGender( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetGender: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "GetGender: Invalid character (%i)", player );
		return JS_FALSE;
	}
	UI16 plID = chars[player].GetID();
	switch( plID )
	{
	case 0x0190:
	case 0x0192:
		*rval = INT_TO_JSVAL( 0 );
		break;
	case 0x0191:
	case 0x0193:
		*rval = INT_TO_JSVAL( 1 );
		break;
	default:
		*rval = INT_TO_JSVAL( 2 );
		break;
	}
	return JS_TRUE;
}
JSBool SE_SetGender( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SetGender: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	UI08 gender = (UI08)JSVAL_TO_INT( argv[1] );
	if( player == INVALIDSERIAL || player > cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "SetGender: Invalid character (%i)", player );
		return JS_FALSE;
	}
	switch( gender )
	{
	case 0:	// male
		if( !chars[player].IsDead() )
			chars[player].SetID( 0x0190 );
		else
			chars[player].SetID( 0x0192 );
		chars[player].Teleport();
		break;
	case 1:	// female
		if( !chars[player].IsDead() )
			chars[player].SetID( 0x0191 );
		else
			chars[player].SetID( 0x0193 );
		chars[player].Teleport();
		break;
	default:	// can't do
		break;
	}
	return JS_TRUE;
}

JSBool SE_RollDice( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 3 )
	{
		DoSEErrorMessage( "RollDice: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	long numDice = JSVAL_TO_INT( argv[0] );
	long numFace = JSVAL_TO_INT( argv[1] );
	long numAdd  = JSVAL_TO_INT( argv[2] );

	cDice toRoll( numDice, numFace, numAdd );

	*rval = INT_TO_JSVAL( toRoll.roll() );
	return JS_TRUE;
}

JSBool SE_TurnToward( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "TurnToward: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	CHARACTER tChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	SI16 xLoc = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 yLoc = (SI16)JSVAL_TO_INT( argv[2] );
	UI08 newDir = getCharDir( &chars[tChar], xLoc, yLoc );
	chars[tChar].SetDir( newDir );
	chars[tChar].Update();
	return JS_TRUE;
}

JSBool SE_DirectionTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "DirectionTo: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	CHARACTER tChar = JSVAL_TO_INT( argv[0] );
	SI16 xLoc = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 yLoc = (SI16)JSVAL_TO_INT( argv[2] );
	*rval = INT_TO_JSVAL( getCharDir( &chars[tChar], xLoc, yLoc ) );
	return JS_TRUE;
}

JSBool SE_RaceCompare( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	CHARACTER char0 = JSVAL_TO_INT( argv[0] );
	CHARACTER char1 = JSVAL_TO_INT( argv[1] );
	if( char0 == INVALIDSERIAL || char1 == INVALIDSERIAL || char0 >= cwmWorldState->GetCMem() || char1 >= cwmWorldState->GetCMem() )
	{
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( Races->Compare( char0, char1 ) );
	return JS_TRUE;
}

JSBool SE_RaceCompareByRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID r0 = (RACEID)JSVAL_TO_INT( argv[0] );
	RACEID r1 = (RACEID)JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( Races->CompareByRace( r0, r1 ) );

	return JS_TRUE;
}

JSBool SE_FindItemOnLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	CHARACTER player = JSVAL_TO_INT( argv[0] );
	UI08 layerNum = (UI08)JSVAL_TO_INT( argv[1] );
	if( player == INVALIDSERIAL || player >= cwmWorldState->GetCMem() )
	{
		return JS_FALSE;
	}
	CItem *tItem = chars[player].GetItemAtLayer( layerNum );
	*rval = INT_TO_JSVAL( calcItemFromSer( tItem->GetSerial() ) );

	return JS_TRUE;
}

JSBool SE_FindMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 && argc != 1 )
	{
		DoSEErrorMessage( "FindMulti: Invalid number of parameters (1 or 4)" );
		return JS_FALSE;
	}
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	if( argc == 1 )
	{
		JSObject *myitemptr = JSVAL_TO_OBJECT( argv[0] );
		cBaseObject *myItemPtr = (cBaseObject *)JS_GetPrivate( cx, myitemptr );
		if( myItemPtr != NULL )
		{
			xLoc		= myItemPtr->GetX();
			yLoc		= myItemPtr->GetY();
			zLoc		= myItemPtr->GetZ();
			worldNumber = myItemPtr->WorldNumber();
		}
		else
		{
			DoSEErrorMessage( "FindMulti: Invalid object type" );
			return JS_FALSE;
		}
	}
	else
	{
		xLoc		= (SI16)JSVAL_TO_INT( argv[0] );
		yLoc		= (SI16)JSVAL_TO_INT( argv[1] );
		zLoc		= (SI08)JSVAL_TO_INT( argv[2] );
		worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
	}
	CMultiObj *multi = findMulti( xLoc, yLoc, zLoc, worldNumber );
	if( multi != NULL )
	{
		UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
		cScript *myScript = Trigger->GetScript( myScpTrig );

		JSObject *myObj = myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, multi );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_GetTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
		return JS_FALSE;
	CHARACTER toCheck = JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
		return JS_FALSE;
	*rval = INT_TO_JSVAL( chars[toCheck].GetTown() );
	return JS_TRUE;
}

JSBool SE_SetTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
		return JS_FALSE;
	CHARACTER toCheck = JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
		return JS_FALSE;
	UI08 tRegion = (UI08)JSVAL_TO_INT( argv[1] );
	UI08 cRegion = chars[toCheck].GetTown();
	region[cRegion]->RemoveTownMember( chars[toCheck] );
	region[tRegion]->AddAsTownMember( chars[toCheck] );
	return JS_TRUE;
}

JSBool SE_GetRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
		return JS_FALSE;
	CHARACTER toCheck = JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
		return JS_FALSE;
	*rval = INT_TO_JSVAL( chars[toCheck].GetRegion() );
	return JS_TRUE;
}

JSBool SE_SetRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
		return JS_FALSE;
	CHARACTER toCheck = JSVAL_TO_INT( argv[0] );
	if( toCheck == INVALIDSERIAL || toCheck >= cwmWorldState->GetCMem() )
		return JS_FALSE;
	UI08 tRegion = (UI08)JSVAL_TO_INT( argv[1] );
	chars[toCheck].SetRegion( tRegion );
	return JS_TRUE;
}

JSBool SE_IsRegionGuarded( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
		return JS_FALSE;
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( region[toCheck]->IsGuarded() );
	return JS_TRUE;
}

JSBool SE_CanMarkInRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( region[toCheck]->CanMark() );
	return JS_TRUE;
}

JSBool SE_CanRecallInRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( region[toCheck]->CanRecall() );
	return JS_TRUE;
}

JSBool SE_CanGateInRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( region[toCheck]->CanGate() );
	return JS_TRUE;
}

JSBool SE_GetGuildType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
		*rval = INT_TO_JSVAL( GT_STANDARD );
	else 
		*rval = INT_TO_JSVAL( mGuild->Type() );
	return JS_TRUE;
}

JSBool SE_GetGuildName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}

	JSString *strSpeech = NULL;
	strSpeech = JS_NewStringCopyZ( cx, mGuild->Name() );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

JSBool SE_GetGuildMaster( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else 
		*rval = INT_TO_JSVAL( calcCharFromSer( mGuild->Master() ) );
	return JS_TRUE;
}

JSBool SE_SetGuildType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}

	SI08 newType = (SI08)JSVAL_TO_INT( argv[1] );
	if( newType < GT_STANDARD || newType > GT_COUNT )
		newType = GT_UNKNOWN;
	mGuild->Type( (GuildType)newType );
	return JS_TRUE;
}

JSBool SE_SetGuildName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GuildID targGuild = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( targGuild );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}
	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( test == NULL )
	{
		return JS_FALSE;
	}
	mGuild->Name( test );
	return JS_TRUE;
}

JSBool SE_SetGuildMaster( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}

	mGuild->Master( JSVAL_TO_INT( argv[1] ) );
	return JS_TRUE;
}

JSBool SE_GetNumGuildMembers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	UI08 memberType = (UI08)JSVAL_TO_INT( argv[1] );
	if( memberType != 0 && memberType != 1 )
		memberType = 0;

	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
		*rval = INT_TO_JSVAL( 0 );
	else if( memberType == 0 )
		*rval = INT_TO_JSVAL( mGuild->NumMembers() );
	else if( memberType == 1 )
		*rval = INT_TO_JSVAL( mGuild->NumRecruits() );
	else
		*rval = INT_TO_JSVAL( mGuild->NumRecruits() + mGuild->NumMembers() );
	return JS_TRUE;
}

JSBool SE_AddToGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CHARACTER toAdd = (CHARACTER)JSVAL_TO_INT( argv[1] );
	SI08 nType = (SI08)JSVAL_TO_INT( argv[2] );
	if( toAdd == INVALIDSERIAL || toAdd > cwmWorldState->GetCMem() || toCheck == -1 || ( nType != 0 && nType != 1 ) )
	{
		return JS_FALSE;
	}
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}
	if( nType == 0 )
	{
		mGuild->NewMember( chars[toAdd] );
		chars[toAdd].SetGuildNumber( toCheck );
	}
	else if( nType == 1 )
		mGuild->NewRecruit( chars[toAdd] );
	return JS_TRUE;
}

JSBool SE_RemoveFromGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CHARACTER toAdd = (CHARACTER)JSVAL_TO_INT( argv[1] );
	SI08 nType = (SI08)JSVAL_TO_INT( argv[2] );
	if( toAdd == INVALIDSERIAL || toAdd > cwmWorldState->GetCMem() || toCheck == -1 || ( nType != 0 && nType != 1 ) )
	{
		return JS_FALSE;
	}
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}
	if( nType == 0 )
	{
		mGuild->RemoveMember( chars[toAdd] );
		chars[toAdd].SetGuildNumber( -1 );
	}
	else if( nType == 1 )
		mGuild->RemoveRecruit( chars[toAdd] );
	return JS_TRUE;
}

JSBool SE_CompareGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	CHARACTER toCheck = (CHARACTER)JSVAL_TO_INT( argv[0] );
	CHARACTER toCheck2 = (CHARACTER)JSVAL_TO_INT( argv[1] );
	if( toCheck == INVALIDSERIAL || toCheck2 == INVALIDSERIAL || toCheck > cwmWorldState->GetCMem() || toCheck2 > cwmWorldState->GetCMem() )
		*rval = INT_TO_JSVAL( GR_UNKNOWN );
	else
		*rval = INT_TO_JSVAL( GuildSys->Compare( &chars[toCheck], &chars[toCheck2] ) );
	return JS_TRUE;
}

JSBool SE_CompareGuildByGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	GuildID toCheck2 = (GuildID)JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( GuildSys->Compare( toCheck, toCheck2 ) );
	return JS_TRUE;
}

JSBool SE_GetGuildStone( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	GuildID toCheck = (GuildID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( mGuild->Stone() );
	return JS_TRUE;
}

JSBool SE_IsMemberOfTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	*rval = JS_FALSE;
	CHARACTER mChar = JSVAL_TO_INT( argv[0] );
	if( mChar == INVALIDSERIAL || mChar >= cwmWorldState->GetCMem() )
		return JS_TRUE;
	UI08 town = (UI08)JSVAL_TO_INT( argv[1] );
	*rval = BOOLEAN_TO_JSVAL( region[town]->IsMemberOfTown( &chars[mChar] ) );
	return JS_TRUE;
}

JSBool SE_GetTownMayor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 town = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( region[town]->GetMayor() );
	return JS_TRUE;
}

JSBool SE_GetTownRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 town = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( region[town]->GetRace() );
	return JS_TRUE;
}

JSBool SE_SetTownRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	UI08 town = (UI08)JSVAL_TO_INT( argv[0] );
	RACEID nRace = (RACEID)JSVAL_TO_INT( argv[1] );
	region[town]->SetRace( nRace );
	return JS_TRUE;
}

JSBool SE_PossessTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	UI08 town = (UI08)JSVAL_TO_INT( argv[0] );
	UI08 sTown = (UI08)JSVAL_TO_INT( argv[1] );
	region[town]->Possess( sTown );
	return JS_TRUE;
}

JSBool SE_GetTownTax( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_GetTownTaxResource( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 town = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( region[town]->GetResourceID() );
	return JS_TRUE;
}

JSBool SE_SetTownTax( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_SetTownTaxResource( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_TRUE;
}

JSBool SE_GetRaceName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	JSString *strName = NULL;
	strName = JS_NewStringCopyZ( cx, Races->Name( race ) );
	*rval = STRING_TO_JSVAL( strName );
	return JS_TRUE;
}

JSBool SE_SetRaceName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 2 )
	{
		return JS_FALSE;
	}
	RACEID rNew = (RACEID)JSVAL_TO_INT( argv[0] );
	if( rNew >= Races->Count() )
	{
		return JS_FALSE;
	}
	CRace *mRace = Races->Race( rNew );
	if( mRace == NULL )
	{
		return JS_FALSE;
	}
 	char *newName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	if( newName == NULL )
	{
		return JS_FALSE;
	}
	mRace->Name( newName );
	return JS_TRUE;
}

JSBool SE_IsRaceWeakToWeather( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	weathID toCheck = (weathID)JSVAL_TO_INT( argv[1] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	switch( toCheck )
	{
	case LIGHT:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByLight() );	break;
	case RAIN:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByRain() );	break;
	case COLD:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByCold() );	break;
	case HEAT:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByHeat() );	break;
	case LIGHTNING:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByLightning() );	break;
	case SNOW:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedBySnow() );	break;
	default:
		return JS_FALSE;
	};
	return JS_TRUE;
}

JSBool SE_RaceRequiresBeard( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( Races->RequireBeard( race ) );
	return JS_TRUE;
}

JSBool SE_RaceRequiresNoBeard( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( Races->NoBeard( race ) );
	return JS_TRUE;
}

JSBool SE_IsPlayerRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( Races->IsPlayerRace( race ) );
	return JS_TRUE;
}

JSBool SE_GetRacialGenderRestrict( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( Races->GenderRestrict( race ) );
	return JS_TRUE;
}

JSBool SE_GetRacialArmourClass( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( Races->ArmorRestrict( race ) );
	return JS_TRUE;
}

JSBool SE_CanRaceWearArmour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	ITEM tItem = (ITEM)JSVAL_TO_INT( argv[1] );
	if( tItem == INVALIDSERIAL || tItem >= cwmWorldState->GetCMem() )
	{
		return JS_FALSE;
	}
	ARMORCLASS srcClass = Races->ArmorRestrict( race );
	ARMORCLASS trgClass = items[tItem].GetArmourClass();
	*rval = BOOLEAN_TO_JSVAL( ( trgClass == 0 ) || ( ( srcClass & trgClass ) != 0 ) );	// they have a matching class
	return JS_TRUE;
}

JSBool SE_GetRaceLanguageSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( Races->LanguageMin( race ) );
	return JS_TRUE;
}

JSBool SE_GetRaceSkillAdjustment( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	int skill = JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( Races->DamageFromSkill( skill, race ) );
	return JS_TRUE;
}

JSBool SE_GetRacePoisonResistance( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = DOUBLE_TO_JSVAL( tRace->PoisonResistance() );
	return JS_TRUE;
}

JSBool SE_GetRaceMagicResistance( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = DOUBLE_TO_JSVAL( tRace->MagicResistance() );
	return JS_TRUE;
}

JSBool SE_GetRaceVisibleDistance( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( tRace->VisibilityRange() );
	return JS_TRUE;
}

JSBool SE_GetRaceNightVision( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[1] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( tRace->NightVision() );
	return JS_TRUE;
}

JSBool SE_IsValidRaceHairColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	COLOUR colour = (COLOUR)JSVAL_TO_INT( argv[1] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( tRace->IsValidHair( colour ) );
	return JS_TRUE;
}

JSBool SE_IsValidRaceSkinColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	COLOUR colour = (COLOUR)JSVAL_TO_INT( argv[1] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( tRace->IsValidSkin( colour ) );
	return JS_TRUE;
}

JSBool SE_IsValidRaceBeardColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	COLOUR colour = (COLOUR)JSVAL_TO_INT( argv[1] );
	CRace *tRace = Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( tRace->IsValidBeard( colour ) );
	return JS_TRUE;
}

JSBool SE_GetClosestTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_FALSE;
}

JSBool SE_FirstItemInCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI32 item = JSVAL_TO_INT( argv[0] );
	if( item == INVALIDSERIAL || item >= cwmWorldState->GetIMem() )
	{
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( items[item].FirstItem() );
	return JS_TRUE;
}

JSBool SE_NextItemInCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI32 item = JSVAL_TO_INT( argv[0] );
	if( item == INVALIDSERIAL || item >= cwmWorldState->GetIMem() )
	{
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( items[item].NextItem() );
	return JS_TRUE;
}

JSBool SE_FinishedItemInCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI32 item = JSVAL_TO_INT( argv[0] );
	if( item == INVALIDSERIAL || item >= cwmWorldState->GetIMem() )
	{
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( items[item].FinishedItems() );
	return JS_TRUE;
}

JSBool SE_UseDoor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "UseDoor: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UOXSOCKET iSock = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
	ITEM iDoor = (ITEM)JSVAL_TO_INT( argv[1] );
	if( iDoor == INVALIDSERIAL || iDoor >= cwmWorldState->GetIMem() )
	{
		DoSEErrorMessage( "UseDoor: Invalid door (%i)", iDoor );
		return JS_FALSE;
	}
	cSocket *tSock = calcSocketObjFromSock( iSock );
	useDoor( tSock, &items[iDoor] );
	return JS_TRUE;
}

JSBool SE_VendorSell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "VendorSell: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UOXSOCKET sock = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
	CHARACTER npc = (CHARACTER)JSVAL_TO_INT( argv[1] );

	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "VendorSell: Invalid socket (%i)", sock );
		return JS_FALSE;
	}
	if( npc == INVALIDSERIAL || npc >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "VendorSell: Invalid character (%i)", npc );
		return JS_FALSE;
	}

	chars[npc].SetNpcMoveTime( BuildTimeValue( 60.0f ) );
	sendSellStuff( calcSocketObjFromSock( sock ), &chars[npc] );
	return JS_TRUE;
}

JSBool SE_VendorBuy( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "VendorBuy: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UOXSOCKET sock = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
	CHARACTER npc = (CHARACTER)JSVAL_TO_INT( argv[1] );

	if( sock == -1 || sock >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "VendorBuy: Invalid socket (%i)", sock );
		return JS_FALSE;
	}
	if( npc == INVALIDSERIAL || npc >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "VendorBuy: Invalid character (%i)", npc );
		return JS_FALSE;
	}

	cSocket *mSock = calcSocketObjFromSock( sock );
	if( mSock == NULL )
	{
		DoSEErrorMessage( "VendorBuy: Invalid socket (%i)", sock );
		return JS_FALSE;
	}

	if( chars[npc].GetNPCAiType() == 17 )
	{
		mSock->AddX( npc );
		npcTalk( calcSocketObjFromSock( sock ), &chars[npc], 772, false );
		target( mSock, 0, 224, " ");
	} 
	else
		Targ->BuyShop( mSock, &chars[npc] );
	return JS_TRUE;
}

JSBool SE_TriggerEvent( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Takes at least 2 parameters, which is the script number to trigger and the function name to call
	// Any extra parameters are extra parameters to the JS event
	if( argc < 2 )
	{
		return JS_FALSE;
	}

	UI16 scriptNumberToFire = (UI16)JSVAL_TO_INT( argv[0] );
 	char *eventToFire = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	cScript *toExecute = Trigger->GetScript( scriptNumberToFire );

	if( toExecute == NULL || eventToFire == NULL )
		return JS_FALSE;

	if( toExecute->CallParticularEvent( eventToFire, &argv[2], argc - 2 ) )
		return JS_TRUE;
	else
		return JS_FALSE;
}

JSBool SE_GetGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetGuild: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CHARACTER mChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( mChar == INVALIDSERIAL || mChar >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "GetGuild: Invalid character (%i)", mChar );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( chars[mChar].GetGuildNumber() );
	return JS_TRUE;
}

JSBool SE_SkillBeingUsed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SkillBeingUsed: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	CHARACTER mChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( mChar == INVALIDSERIAL || mChar >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "SkillBeingUsed: Invalid character (%i)", mChar );
		return JS_FALSE;
	}
	UI08 skillUsed = (UI08)JSVAL_TO_INT( argv[1] );
	*rval = BOOLEAN_TO_JSVAL( chars[mChar].SkillUsed( skillUsed ) );
	return JS_TRUE;
}

JSBool SE_SetSkillUse( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "SetSkillUse: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	CHARACTER mChar = (CHARACTER)JSVAL_TO_INT( argv[0] );
	if( mChar == INVALIDSERIAL || mChar >= cwmWorldState->GetCMem() )
	{
		DoSEErrorMessage( "SetSkillUse: Invalid character (%i)", mChar );
		return JS_FALSE;
	}
	UI08 skillUsed = (UI08)JSVAL_TO_INT( argv[1] );
	bool skillStatus = ( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE );
	chars[mChar].SkillUsed( skillStatus, skillUsed );
	return JS_TRUE;
}

JSBool SE_GetPrivateBit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "GetPrivateBit: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	UI32 mCheck = JSVAL_TO_INT( argv[0] );
	int mType = JSVAL_TO_INT( argv[1] );

	if( mCheck == INVALIDSERIAL || (mType == 0 && mCheck > cwmWorldState->GetCMem()) || (mType == 1 && mCheck > cwmWorldState->GetIMem()) )
	{
		DoSEErrorMessage( "GetPrivateBit: Invalid object (%i) of type (%i)", mCheck, mType );
		return JS_FALSE;
	}

	UI08 mWord = (UI08)JSVAL_TO_INT( argv[2] );
	UI08 mBit = (UI08)JSVAL_TO_INT( argv[3] );

	if( mType == 0 )
		*rval = BOOLEAN_TO_JSVAL( chars[mCheck].GetBit( mWord, mBit ) );
	else
		*rval = BOOLEAN_TO_JSVAL( items[mCheck].GetBit( mWord, mBit ) );
	return JS_TRUE;
}

JSBool SE_SetPrivateBit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "SetPrivateBit: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	UI32 mCheck = JSVAL_TO_INT( argv[0] );
	int mType = JSVAL_TO_INT( argv[1] );

	if( mCheck == INVALIDSERIAL || (mType == 0 && mCheck > cwmWorldState->GetCMem()) || (mType == 1 && mCheck > cwmWorldState->GetIMem()) )
	{
		DoSEErrorMessage( "SetPrivateBit: Invalid object (%i) of type (%i)", mCheck, mType );
		return JS_FALSE;
	}

	UI08 mWord = (UI08)JSVAL_TO_INT( argv[2] );
	UI08 mBit = (UI08)JSVAL_TO_INT( argv[3] );

	bool bVal = ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );

	if( mType == 0 )
		chars[mCheck].SetBit( mWord, mBit, bVal );
	else
		items[mCheck].SetBit( mWord, mBit, bVal );
	return JS_TRUE;
}

JSBool SE_GetPrivateWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "GetPrivateWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	UI32 mCheck = JSVAL_TO_INT( argv[0] );
	int mType = JSVAL_TO_INT( argv[1] );

	if( mCheck == INVALIDSERIAL || (mType == 0 && mCheck > cwmWorldState->GetCMem()) || (mType == 1 && mCheck > cwmWorldState->GetIMem()) )
	{
		DoSEErrorMessage( "GetPrivateWord: Invalid object (%i) of type (%i)", mCheck, mType );
		return JS_FALSE;
	}

	UI08 mWord = (UI08)JSVAL_TO_INT( argv[2] );

	if( mType == 0 )
		*rval = INT_TO_JSVAL( chars[mCheck].GetWord( mWord ) );
	else
		*rval = INT_TO_JSVAL( items[mCheck].GetWord( mWord ) );
	return JS_TRUE;
}

JSBool SE_SetPrivateWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "SetPrivateWord: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	UI32 mCheck = JSVAL_TO_INT( argv[0] );
	int mType = JSVAL_TO_INT( argv[1] );

	if( mCheck == INVALIDSERIAL || (mType == 0 && mCheck > cwmWorldState->GetCMem()) || (mType == 1 && mCheck > cwmWorldState->GetIMem()) )
	{
		DoSEErrorMessage( "SetPrivateWord: Invalid object (%i) of type (%i)", mCheck, mType );
		return JS_FALSE;
	}

	UI08 mWord = (UI08)JSVAL_TO_INT( argv[2] );
	UI08 mVal = (UI08)JSVAL_TO_INT( argv[3] );

	if( mType == 0 )
		chars[mCheck].SetWord( mWord, mVal );
	else
		items[mCheck].SetWord( mWord, mVal );
	return JS_TRUE;
}

JSBool SE_GetPrivateBitRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "GetPrivateBitRange: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	UI32 mCheck = JSVAL_TO_INT( argv[0] );
	int mType = JSVAL_TO_INT( argv[1] );

	if( mCheck == INVALIDSERIAL || (mType == 0 && mCheck > cwmWorldState->GetCMem()) || (mType == 1 && mCheck > cwmWorldState->GetIMem()) )
	{
		DoSEErrorMessage( "GetPrivateBitRange: Invalid object (%i) of type (%i)", mCheck, mType );
		return JS_FALSE;
	}

	UI08 mWord = (UI08)JSVAL_TO_INT( argv[2] );
	UI08 mLow  = (UI08)JSVAL_TO_INT( argv[3] );
	UI08 mHigh = (UI08)JSVAL_TO_INT( argv[4] );

	if( mType == 0 )
		*rval = INT_TO_JSVAL( chars[mCheck].GetBitRange( mWord, mLow, mHigh ) );
	else
		*rval = INT_TO_JSVAL( items[mCheck].GetBitRange( mWord, mLow, mHigh ) );
	return JS_TRUE;
}

JSBool SE_SetPrivateBitRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "SetPrivateBitRange: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}

	UI32 mCheck = JSVAL_TO_INT( argv[0] );
	int mType = JSVAL_TO_INT( argv[1] );

	if( mCheck == INVALIDSERIAL || (mType == 0 && mCheck > cwmWorldState->GetCMem()) || (mType == 1 && mCheck > cwmWorldState->GetIMem()) )
	{
		DoSEErrorMessage( "SetPrivateBitRange: Invalid object (%i) of type (%i)", mCheck, mType );
		return JS_FALSE;
	}

	UI08 mWord = (UI08)JSVAL_TO_INT( argv[2] );
	UI08 mLow  = (UI08)JSVAL_TO_INT( argv[3] );
	UI08 mHigh = (UI08)JSVAL_TO_INT( argv[4] );
	UI32 mVal  = (UI32)JSVAL_TO_INT( argv[5] );

	if( mType == 0 )
		chars[mCheck].SetBitRange( mWord, mLow, mHigh, mVal );
	else
		items[mCheck].SetBitRange( mWord, mLow, mHigh, mVal );
	return JS_TRUE;
}

JSBool SE_GetPackOwner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "GetPackOwner: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UI08 mType = (UI08)JSVAL_TO_INT( argv[1] );
	CChar *pOwner = NULL;

	if( mType == 0 )	// item
	{
		JSObject *mItem = JSVAL_TO_OBJECT( argv[0] );
		CItem *myItem = (CItem *)JS_GetPrivate( cx, mItem );
		pOwner = getPackOwner( myItem );
	}
	else				// serial
	{
		SI32 mSerItem = JSVAL_TO_INT( argv[0] );
		pOwner = getPackOwner( calcItemObjFromSer( mSerItem ) );
	}
	if( pOwner != NULL )
	{
		UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
		cScript *myScript = Trigger->GetScript( myScpTrig );

		JSObject *myObj = myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myObj, pOwner );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_CalcTargetedItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcTargetedItem: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSObject *mysockptr = JSVAL_TO_OBJECT( argv[0] );
	cSocket *sChar = (cSocket *)JS_GetPrivate( cx, mysockptr );
	if( sChar == NULL )
	{
		DoSEErrorMessage( "CalcTargetedItem: Invalid socket" );
		return JS_FALSE;
	}
	
	CItem *calcedItem = calcItemObjFromSer( sChar->GetDWord( 7 ) );
	if( calcedItem == NULL )
	{
		*rval = JSVAL_NULL;
	}
	else
	{
		UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
		cScript *myScript = Trigger->GetScript( myScpTrig );

		JSObject *myObj = myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, calcedItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	return JS_TRUE;
}

JSBool SE_CalcTargetedChar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcTargetedChar: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	UOXSOCKET sChar = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
	if( sChar == -1 || sChar >= cwmWorldState->GetPlayersOnline() )
	{
		DoSEErrorMessage( "CalcTargetedChar: Invalid socket (%i)", sChar );
		return JS_FALSE;
	}
	cSocket *mSock = calcSocketObjFromSock( sChar );
	if( mSock == NULL )
	{
		DoSEErrorMessage( "CalcTargetedChar: Invalid socket (%i)", sChar );
		return JS_FALSE;
	}
	
	*rval = INT_TO_JSVAL( calcCharFromSer( mSock->GetDWord( 7 ) ) );
	return JS_TRUE;
}

JSBool SE_GetTileIDAtMapCoord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "GetTileIDAtMapCoord: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UI16 xLoc = (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc = (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber = (UI08)JSVAL_TO_INT( argv[2] );

	map_st mMap = Map->SeekMap0( xLoc, yLoc, wrldNumber );
	*rval = INT_TO_JSVAL( mMap.id );
	return JS_TRUE;
}

JSBool SE_GetClientVersion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetClientVersion: Invalid number of arguments (takes 1, socket)" );
		return JS_FALSE;
	}

	UOXSOCKET tSock = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
	cSocket *mSock = calcSocketObjFromSock( tSock );
	if( mSock == NULL )
	{
		DoSEErrorMessage( "GetClientVersion: Invalid socket (%i)", tSock );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( mSock->ClientVersion() );
	return JS_TRUE;
}

JSBool SE_GetClientType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetClientType: Invalid number of arguments (takes 1, socket)" );
		return JS_FALSE;
	}

	UOXSOCKET tSock = (UOXSOCKET)JSVAL_TO_INT( argv[0] );
	cSocket *mSock = calcSocketObjFromSock( tSock );
	if( mSock == NULL )
	{
		DoSEErrorMessage( "GetClientType: Invalid socket (%i)", tSock );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( mSock->ClientType() );
	return JS_TRUE;
}

JSBool SE_FileOpen( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	char *filename, *mode;
	if( argc != 2 )
	{
		DoSEErrorMessage( "FileOpen: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	filename = JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
	mode = JS_GetStringBytes( JSVAL_TO_STRING( argv[1] ) );
	strlwr( mode );
	if( mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a' )
	{
		DoSEErrorMessage( "FileOpen: Invalid mode must be \"read\", \"write\", or \"append\"!" );
		return JS_FALSE;
	}
	if( strstr( filename, ".." ) || strstr( filename, "\\" ) || strstr( filename, "/" ) )
	{
		DoSEErrorMessage( "FileOpen: file names may not contain \"..\", \"\\\", or \"/\"." );
		return JS_FALSE;
	}
	mode[1] = 0;

	*rval = INT_TO_JSVAL( (int)(fopen( filename, mode )) );
	return JS_TRUE;
}

JSBool SE_FileClose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "FileClose: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	FILE *fp = (FILE *)JSVAL_TO_INT( argv[0] );
	if( !fp ) 
		return JS_TRUE;
	fclose( fp );
	return JS_TRUE;
}

JSBool SE_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "Write: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	FILE *fp =  (FILE *)JSVAL_TO_INT( argv[0] );
	if( !fp || feof( fp ) )
	{
		DoSEErrorMessage( "Write: Error writing to file, file was not opened sucessfully!" );
		return JS_FALSE;
	} 
	else if( ftell( fp ) > 1049600 ) 
	{
		DoSEErrorMessage( "Write: Error writing to file.  File my not exceed 1mb." );
		return JS_FALSE;
	}

	char *str = JS_GetStringBytes( JSVAL_TO_STRING( argv[1] ) );
	fprintf( fp, str );
	
	return JS_TRUE;
}

JSBool SE_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "ReadUntil: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	FILE *fp = (FILE *)(JSVAL_TO_INT(argv[0]));
	if( !fp || feof( fp ) )
	{
		DoSEErrorMessage( "ReadUntil: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	char *until = JS_GetStringBytes( JSVAL_TO_STRING( argv[1] ) );
	char line[512];
	int c;

	if( until[0] == '\\' && strlen( until ) > 1 )
	{
		switch (until[1])
		{
		case '\\': until[0] = '\\'; break;
		case '\'': until[0] = '\''; break;
		case '\"': until[0] = '\"'; break;
		case 'n':  until[0] = '\n'; break;
		default: DoSEErrorMessage( "ReadUntil: Unsupported character escape sequence %s", until );
		}
	}

	for( c = 0; c < 512 && !feof( fp ); c++ )
	{
		line[c] = fgetc( fp );
		if( line[c] == until[0] || line[c] == '\n' )
			break;
	}
	line[c<512 ? c : 511] = 0;

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, line ) );
	return JS_TRUE;
}

JSBool SE_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "Read: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	FILE *fp = (FILE *)(JSVAL_TO_INT(argv[0]));
	if( !fp || feof( fp ) )
	{
		DoSEErrorMessage( "Read: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	int bytes = JSVAL_TO_INT( argv[1] );
	char data[512];

	if( bytes > 512 || bytes < 1 )
	{
		DoSEErrorMessage( "Read: Invalid byte count, must be from 1 to 512!" );
		return JS_FALSE;
	}

	fread( data, 1, bytes, fp );

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, data ) );
	return JS_TRUE;
}

JSBool SE_EndOfFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EndOfFile: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	FILE *fp = (FILE *)JSVAL_TO_INT( argv[0] );

	if( fp ) 
		*rval = feof( fp );
	else *
		rval = 1;
	return JS_TRUE;
}

JSBool SE_StringToNum( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "StringToNum: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	char *str = JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );

	*rval = INT_TO_JSVAL( makeNum( str ) );
	return JS_TRUE;
}

JSBool SE_NumToString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "NumToString: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	int num = JSVAL_TO_INT( argv[0] );
	char str[32];

	numtostr( num, str );
	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, str ) );
	return JS_TRUE;
}

JSBool SE_SetDecayTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SetDecayTime: Invalid number of arguments (takes 2, item and decay time)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || ( targChar > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "SetDecayTime: Invalid object (%i)", targChar );
		return JS_FALSE;
	}
	UI32 decaytime = (UI32)JSVAL_TO_INT( argv[1] );
	items[targChar].SetDecayTime( decaytime );
	return JS_TRUE;
}
JSBool SE_GetDecayTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetDecayTime: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || ( targChar > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "GetDecayTime: Invalid type (%i) ", targChar); 
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( items[targChar].GetDecayTime() );
	
	return JS_TRUE;
}

JSBool SE_SetDecayable( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "SetDecayable: Invalid number of arguments (takes 2, item and decayable)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || ( targChar > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "SetDecayTime: Invalid object (%i)", targChar );
		return JS_FALSE;
	}
	bool decayable = ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );
	items[targChar].SetDecayable( decayable );
	return JS_TRUE;
}
JSBool SE_IsDecayable( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "IsDecayable: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}
	UI32 targChar = JSVAL_TO_INT( argv[0] );
	if( targChar == INVALIDSERIAL || ( targChar > cwmWorldState->GetIMem() ) )
	{
		DoSEErrorMessage( "IsDecayable: Invalid type (%i) ", targChar); 
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( items[targChar].isDecayable() );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------
//|	Function	-	JSBool SE_GetRaceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	November 9, 2001
//|	Programmer	-	DarkStorm
//|	Modified	-
//o--------------------------------------------------------------------------
//|	Purpose		-	Returns the total number of races found in the server
//o--------------------------------------------------------------------------
JSBool SE_GetRaceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		DoSEErrorMessage( "GetRaceCount: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( Races->Count() );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------
//|	Function	-	JSBool SE_AreaCharacterFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	January 27, 2003
//|	Programmer	-	Maarc
//|	Modified	-
//o--------------------------------------------------------------------------
//|	Purpose		-	Using a passed in function name, executes a JS function
//|				-	on an area of characters
//o--------------------------------------------------------------------------
JSBool SE_AreaCharacterFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		// function name, source character, range
		DoSEErrorMessage( "AreaCharacterFunction: Invalid number of arguments (takes 3/4, function name, source character, range, optional socket)" );
		return JS_FALSE;
	}

	// Do parameter validation here
	JSObject *srcCharacterObj = NULL;
	CChar *srcChar = NULL;

	JSObject *srcSocketObj = NULL;
	cSocket *srcSocket = NULL;

 	char *trgFunc = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgFunc == NULL )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 0 not a valid string" );
		return JS_FALSE;
	}

	srcCharacterObj	= JSVAL_TO_OBJECT( argv[1] );
	srcChar			= (CChar *)JS_GetPrivate( cx, srcCharacterObj );

	if( srcChar == NULL )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 1 not a valid character" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		srcSocket		= (cSocket *)JS_GetPrivate( cx, srcCharacterObj );
	}
	
	int xOffset = MapRegion->GetGridX( srcChar->GetX() );
	int yOffset = MapRegion->GetGridY( srcChar->GetY() );

	UI08 worldNumber = srcChar->WorldNumber();

	UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
	cScript *myScript = Trigger->GetScript( myScpTrig );

	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset+counter2, worldNumber );	// check 3 cols... do we really NEED to?
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( objInRange( srcChar, tempChar, (UI16)distance ) )
					myScript->AreaCharFunc( trgFunc, srcChar, tempChar, srcSocket );
			}
			MapArea->PopChar();
		}
	}
	return JS_TRUE;
}
//o--------------------------------------------------------------------------o
//|	Function			-	JSBool SE_GetCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date					-	1/13/2003 11:09:39 PM
//|	Developers		-	Brakhtus
//|	Organization	-	Forum Submission
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Extend the UOX# JSE implementation to support scriptable 
//|									commands that players, and daminstration for a shard may
//|									use to performs specialized tasks.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
JSBool SE_GetCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
 	if( argc != 1 )
 	{
		DoSEErrorMessage( "GetCommand: needs an argument" );
		return JS_FALSE; 	
	} 	
	short idx = static_cast<short>((JSVAL_TO_INT( argv[0] )));
 	if (idx>=Commands->GetNumArguments()) 
	{ 		
		DoSEErrorMessage( "GetCommand: Index exeeds the commando-array!" ); 		
		return JS_FALSE; 	
	} 	
	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, comm[idx] ) ); 
	return JS_TRUE; 
}

//o--------------------------------------------------------------------------o
//|	Function			-	JSBool SE_GetCommandSize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date					-	1/13/2003 11:11:46 PM
//|	Developers		-	Brakhtus
//|	Organization	-	Forum Submission
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
JSBool SE_GetCommandSize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval ) 
{ 	
	if( argc != 0 ) 	
	{ 		
		DoSEErrorMessage( "GetCommandSize: doesnt need any argument!" ); 		
		return JS_FALSE; 	
	} 	
	*rval = INT_TO_JSVAL( Commands->GetNumArguments() ); 	
	return JS_TRUE; 
} 

