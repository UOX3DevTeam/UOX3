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
#include "skills.h"
#include "commands.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "classes.h"
#include "regions.h"
#include "magic.h"
#include "ssection.h"
#include "cThreadQueue.h"
#include "cHTMLSystem.h"
#include "cServerDefinitions.h"
#include "Dictionary.h"
#include "speech.h"
#include "gump.h"
#include "ObjectFactory.h"
#include "network.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"

namespace UOX
{

void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		UnloadRegions( void );
void		UnloadSpawnRegions( void );

#define __EXTREMELY_VERBOSE__

#ifdef __EXTREMELY_VERBOSE__
void DoSEErrorMessage( const char *txt, ... )
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

std::map< std::string, ObjectType > stringToObjType;

void InitStringToObjType( void )
{
	stringToObjType["BASEOBJ"]		= OT_CBO;
	stringToObjType["CHARACTER"]	= OT_CHAR;
	stringToObjType["ITEM"]			= OT_ITEM;
	stringToObjType["SPAWNER"]		= OT_SPAWNER;
	stringToObjType["MULTI"]		= OT_MULTI;
	stringToObjType["BOAT"]			= OT_BOAT;
}

ObjectType FindObjTypeFromString( UString strToFind )
{
	if( stringToObjType.size() == 0 )	// if we haven't built our array yet
		InitStringToObjType();
	std::map< std::string, ObjectType >::const_iterator toFind = stringToObjType.find( strToFind.upper() );
	if( toFind != stringToObjType.end() )
		return toFind->second;
	return OT_CBO;
}

JSBool SE_ConsoleMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ConsoleMessage: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	std::string test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
 	// print the string to the console 
	if( !( test.empty() || test.length() == 0 ) )
 		Console << test;
 	return JS_TRUE;
}
JSBool SE_ScriptPrintNumber( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
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

// Effect related functions
JSBool SE_DoTempEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 7 )
	{
		DoSEErrorMessage( "DoTempEffect: Invalid number of arguments (takes 7 or 8)" );
		return JS_FALSE;
	}
	UI08 iType			= static_cast<UI08>(JSVAL_TO_INT( argv[0] ));
	long targNum		= JSVAL_TO_INT( argv[3] );
	UI08 more1			= (UI08)JSVAL_TO_INT( argv[4] );
	UI08 more2			= (UI08)JSVAL_TO_INT( argv[5] );
	UI08 more3			= (UI08)JSVAL_TO_INT( argv[6] );

	JSObject *myitemptr = NULL;
	CItem *myItemPtr	= NULL;

	if( argc == 8 )
	{
		myitemptr = JSVAL_TO_OBJECT( argv[7] );
		myItemPtr = (CItem *)JS_GetPrivate( cx, myitemptr );
	}

	JSObject *mysrc		= JSVAL_TO_OBJECT( argv[1] );
	CChar *mysrcChar	= (CChar*)JS_GetPrivate( cx, mysrc );

	if( !ValidateObject( mysrcChar ) )
	{
		DoSEErrorMessage( "DoTempEffect: Invalid src" );
		return JS_FALSE;
	}
	
	if( iType == 0 )	// character
	{
		JSObject *mydestc = JSVAL_TO_OBJECT( argv[2] );
		CChar *mydestChar = (CChar*)JS_GetPrivate( cx, mydestc );

		if( !ValidateObject( mydestChar ) )
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

		if( !ValidateObject( mydestItem ) )
		{
			DoSEErrorMessage( "DoTempEffect: Invalid target " );
			return JS_FALSE;
		}
			Effects->tempeffect( mysrcChar, mydestItem, static_cast<SI08>(targNum), more1, more2, more3 );
	}
	return JS_TRUE;
}

// Speech related functions
void sysBroadcast( const std::string txt );
JSBool SE_BroadcastMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "BroadcastMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string trgMessage = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgMessage.empty() || trgMessage.length() == 0 )
	{
		DoSEErrorMessage( "BroadcastMessage: Invalid string (%s)", trgMessage.c_str() );
		return JS_FALSE;
	}
	sysBroadcast( trgMessage );
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

	CItem *newItem		= calcItemObjFromSer( targSerial );
	if( newItem != NULL )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, newItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
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

	CChar *newChar		= calcCharObjFromSer( targSerial );
	if( newChar != NULL )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myObj, newChar );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
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
	CBaseObject *src	= (CBaseObject *)JS_GetPrivate( cx, srcObj );
	if( !ValidateObject( src ) )
	{
		DoSEErrorMessage( "DoMovingEffect: Invalid source object" );
		return JS_FALSE;
	}
	bool targLocation	= false;
	UI08 offset			= 0;
	UI16 targX			= 0;
	UI16 targY			= 0;
	SI08 targZ			= 0;
	CBaseObject *trg	= NULL;
	if( JSVAL_IS_INT( argv[1] ) )
	{	// Location moving effect
		targLocation	= true;
		offset			= true;
		targX			= (UI16)JSVAL_TO_INT( argv[1] );
		targY			= (UI16)JSVAL_TO_INT( argv[2] );
		targZ			= (SI08)JSVAL_TO_INT( argv[3] );
	}
	else
	{
		JSObject *trgObj	= JSVAL_TO_OBJECT( argv[1] );
		trg					= (CBaseObject *)JS_GetPrivate( cx, trgObj );
		if( !ValidateObject( trg ) )
		{
			DoSEErrorMessage( "DoMovingEffect: Invalid target object" );
			return JS_FALSE;
		}
	}
	UI16 effect		= (UI16)JSVAL_TO_INT( argv[2+offset] );
	UI08 speed		= (UI08)JSVAL_TO_INT( argv[3+offset] );
	UI08 loop		= (UI08)JSVAL_TO_INT( argv[4+offset] );
	bool explode	= ( JSVAL_TO_BOOLEAN( argv[5+offset] ) == JS_TRUE );
	UI32 hue = 0, renderMode = 0;
	if( argc - offset >= 7 ) // there's at least 7/9 parameters
		hue = (UI32)JSVAL_TO_INT( argv[6+offset] );
	if( argc - offset >= 8 ) // there's at least 8/10 parameters
		renderMode = (UI32)JSVAL_TO_INT( argv[7+offset] );

	if( targLocation )
		Effects->PlayMovingAnimation( src, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode );
	else
		Effects->PlayMovingAnimation( src, trg, effect, speed, loop, explode, hue, renderMode );
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

JSBool SE_MakeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "MakeItem: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	JSObject *mSock = JSVAL_TO_OBJECT( argv[0] );
	CSocket *sock	= (CSocket *)JS_GetPrivate( cx, mSock );
	JSObject *mChar = JSVAL_TO_OBJECT( argv[1] );
	CChar *player	= (CChar *)JS_GetPrivate( cx, mChar );
	if( !ValidateObject( player ) )
	{
		DoSEErrorMessage( "MakeItem: Invalid character" );
		return JS_FALSE;
	}
	UI16 itemMenu		= (UI16)JSVAL_TO_INT( argv[2] );
	createEntry *toFind = Skills->FindItem( itemMenu );
	if( toFind == NULL )
	{
		DoSEErrorMessage( "MakeItem: Invalid make item (%i)", itemMenu );
		return JS_FALSE;
	}
	Skills->MakeItem( *toFind, player, sock, itemMenu );
	return JS_TRUE;
}

JSBool SE_CommandLevelReq( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CommandLevelReq: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( test.empty() || test.length() == 0 )
	{
		DoSEErrorMessage( "CommandLevelReq: Invalid command name" );
		return JS_FALSE;
	}
	CommandMapEntry *details = Commands->CommandDetails( test );
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
	std::string test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( test.empty() || test.length() == 0 )
	{
		DoSEErrorMessage( "CommandExists: Invalid command name" );
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( Commands->CommandExists( test ) );
	return JS_TRUE;
}

JSBool SE_FirstCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	const std::string tVal = Commands->FirstCommand();
	JSString *strSpeech = NULL;
	if( tVal.empty() )
		strSpeech = JS_NewStringCopyZ( cx, "" );
	else
		strSpeech = JS_NewStringCopyZ( cx, tVal.c_str() );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

JSBool SE_NextCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	const std::string tVal = Commands->NextCommand();
	JSString *strSpeech = NULL;
	if( tVal.empty() )
		strSpeech = JS_NewStringCopyZ( cx, "" );
	else
		strSpeech = JS_NewStringCopyZ( cx, tVal.c_str() );

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
	if( argc != 3 )
	{
		DoSEErrorMessage( "  RegisterCommand: Invalid number of arguments (takes 4)" );
 		return JS_FALSE;
	}
	std::string toRegister	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
 	UI08 execLevel			= static_cast<UI08>(JSVAL_TO_INT( argv[1] ));
	bool isEnabled			= ( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE );
	UI16 scriptID			= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );

	if( scriptID == 0xFFFF )
	{
		DoSEErrorMessage( " RegisterCommand: JS Script has an Invalid ScriptID" );
		return JS_FALSE;
	}	

	Commands->Register( toRegister, JSMapping->GetScriptID( JS_GetGlobalObject( cx ) ), execLevel, isEnabled );
 	return JS_TRUE;
}

JSBool SE_RegisterSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterSpell: Invalid number of arguments (takes 2)" );
 		return JS_FALSE;
	}
	int spellNumber			= JSVAL_TO_INT( argv[0] );
	bool isEnabled			= ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );
	cScript *myScript		= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	Magic->Register( myScript, spellNumber, isEnabled );
 	return JS_TRUE;
}

JSBool SE_RegisterSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterSkill: Invalid number of arguments (takes 2)" );
 		return JS_FALSE;
	}
	int skillNumber			= JSVAL_TO_INT( argv[0] );
	bool isEnabled			= ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );
	UI16 scriptID			= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	if( scriptID != 0xFFFF )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Print( "Registering skill number %i\n", skillNumber );
#endif
		if( skillNumber < 0 || skillNumber >= ALLSKILLS )
			return JS_TRUE;
		cwmWorldState->skill[skillNumber].jsScript = scriptID;
	}
 	return JS_TRUE;
}

JSBool SE_RegisterPacket( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterPacket: Invalid number of arguments (takes 2)" );
 		return JS_FALSE;
	}
	UI08 packet			= static_cast<UI08>(JSVAL_TO_INT( argv[0] ));
	UI08 subCmd			= static_cast<UI08>(JSVAL_TO_INT( argv[1] ));
	UI16 scriptID		= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	if( scriptID != 0xFFFF )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Print( "Registering packet number 0x%X, subcommand 0x%x\n", packet, subCmd );
#endif
		Network->RegisterPacket( packet, subCmd, scriptID );
	}
 	return JS_TRUE;
}

JSBool SE_DisableCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableCommand: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	std::string toDisable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Commands->SetCommandStatus( toDisable, false );
 	return JS_TRUE;
}

JSBool SE_DisableSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableSpell: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	int spellNumber	= JSVAL_TO_INT( argv[0] );
	Magic->SetSpellStatus( spellNumber, false );
 	return JS_TRUE;
}

JSBool SE_EnableCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableCommand: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	std::string toEnable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Commands->SetCommandStatus( toEnable, true );
 	return JS_TRUE;
}

JSBool SE_EnableSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableSpell: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	int spellNumber	= JSVAL_TO_INT( argv[0] );
	Magic->SetSpellStatus( spellNumber, true );
 	return JS_TRUE;
}

JSBool SE_GetHour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	bool ampm = cwmWorldState->ServerData()->ServerTimeAMPM();
	UI08 hour = cwmWorldState->ServerData()->ServerTimeHours();
	if( ampm )
		*rval = INT_TO_JSVAL( hour + 12 );
	else
		*rval = INT_TO_JSVAL( hour );
	return JS_TRUE;
}

JSBool SE_GetMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI08 minute = cwmWorldState->ServerData()->ServerTimeMinutes();
	*rval = INT_TO_JSVAL( minute );
	return JS_TRUE;
}

JSBool SE_GetDay( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SI16 day = cwmWorldState->ServerData()->ServerTimeDay();
	*rval = INT_TO_JSVAL( day );
	return JS_TRUE;
}

JSBool SE_SecondsPerUOMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( "SecondsPerUOMinute: Invalid number of arguments (takes 0 or 1)" );
 		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		UI16 secondsPerUOMinute = (UI16)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->ServerSecondsPerUOMinute( secondsPerUOMinute );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ServerSecondsPerUOMinute() );
	return JS_TRUE;
}

JSBool SE_GetCurrentClock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->GetUICurrentTime() );
	return JS_TRUE;
}

JSBool SE_SpawnNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "SpawnNPC: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}
	CChar *cMade		= NULL;
	std::string nnpcNum	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	UI16 x				= (UI16)JSVAL_TO_INT( argv[1] );
	UI16 y				= (UI16)JSVAL_TO_INT( argv[2] );
	SI08 z				= (SI08)JSVAL_TO_INT( argv[3] );
	UI08 wrld			= (UI08)JSVAL_TO_INT( argv[4] );
	cMade				= Npcs->CreateNPCxyz( nnpcNum, x, y, z, wrld );
	if( cMade != NULL )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myobj		= myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myobj, cMade );
		*rval				= OBJECT_TO_JSVAL( myobj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_CreateDFNItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 3 )
	{
		DoSEErrorMessage( "CreateDFNItem: Invalid number of arguments (takes at least 3)" );
		return JS_FALSE;
	}

	JSObject *mSock				= JSVAL_TO_OBJECT( argv[0] );
	JSObject *mChar				= JSVAL_TO_OBJECT( argv[1] );
	CSocket *mySock				= (CSocket *)JS_GetPrivate( cx, mSock );
	CChar *myChar				= (CChar *)JS_GetPrivate( cx, mChar );

	std::string bpSectNumber	= JS_GetStringBytes( JS_ValueToString( cx, argv[2] ) );
	bool bInPack				= true;
	UI16 iAmount				= 1;
	ObjectType itemType			= OT_ITEM;

	if( argc > 3 )
		iAmount					= static_cast< UI16 >(JSVAL_TO_INT( argv[3] ));
	if( argc > 4 )
	{
		std::string objType		= JS_GetStringBytes( JS_ValueToString( cx, argv[4] ) );
		itemType				= FindObjTypeFromString( objType );
	}
	if( argc > 5 )
		bInPack					= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	
	CItem *newItem = Items->CreateScriptItem( mySock, myChar, bpSectNumber, iAmount, itemType, bInPack );
	if( newItem != NULL )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, newItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}
JSBool SE_CreateBlankItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		DoSEErrorMessage( "CreateBlankItem: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}
	CItem *newItem			= NULL;
	JSObject *mSock			= JSVAL_TO_OBJECT( argv[0] );
	JSObject *mChar			= JSVAL_TO_OBJECT( argv[1] );
	CSocket *mySock			= (CSocket *)JS_GetPrivate( cx, mSock );
	CChar *myChar			= (CChar *)JS_GetPrivate( cx, mChar );
	int amount				= (int)JSVAL_TO_INT( argv[2] );
	std::string itemName	= JS_GetStringBytes( JS_ValueToString( cx, argv[3] ) );
	bool isString			= false;
	std::string szItemName;
	UI16 itemID				= INVALIDID;
	if( JSVAL_IS_STRING( argv[4] ) )
	{
		szItemName = JS_GetStringBytes( JS_ValueToString( cx, argv[4] ) );
		isString = true;
	}
	else
		itemID				= (UI16)JSVAL_TO_INT( argv[4] );
	UI16 colour				= (UI16)JSVAL_TO_INT( argv[5] );
	std::string objType		= JS_GetStringBytes( JS_ValueToString( cx, argv[6] ) );
	ObjectType itemType		= FindObjTypeFromString( objType );
	bool inPack				= ( JSVAL_TO_BOOLEAN( argv[7] ) == JS_TRUE );

	newItem = Items->CreateItem( mySock, myChar, itemID, amount, colour, itemType, inPack );
	if( newItem != NULL )
	{
		newItem->SetName( itemName );
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
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
 	UString ssSearching = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
 	UString ssSearch    = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( ssSearching.empty() || ssSearching.length() == 0 || ssSearch.empty() || ssSearch.length() == 0 )	// no valid search string, or string to search
	{
		DoSEErrorMessage( "SubStringSearch: Invalid search or sub string" );
		return JS_FALSE;
	}
	bool result = ( ssSearching.upper().find( ssSearch.upper() ) != std::string::npos );
	*rval = BOOLEAN_TO_JSVAL( result );
	return JS_TRUE;
}
JSBool SE_GetMurderThreshold( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->RepMaxKills() );
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

JSBool SE_RaceGate( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "RaceGate: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	JSObject *mSock		= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock		= (CSocket *)JS_GetPrivate( cx, mSock );
	JSObject *mChar		= JSVAL_TO_OBJECT( argv[1] );
	CChar *myChar		= (CChar *)JS_GetPrivate( cx, mChar );
	RACEID race			= (RACEID)JSVAL_TO_INT( argv[2] );
	if( race < Races->Count() || race == 65535 )
		Races->gate( myChar, race, true );
	else
		mySock->sysmessage( 13 );
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
		CBaseObject *myItemPtr = (CBaseObject *)JS_GetPrivate( cx, myitemptr );
		if( ValidateObject( myItemPtr ) )
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
	if( ValidateObject( multi ) )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, multi );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_IsRegionGuarded( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
		return JS_FALSE;
	UI08 toCheck	= (UI08)JSVAL_TO_INT( argv[0] );
	*rval			= BOOLEAN_TO_JSVAL( cwmWorldState->townRegions[toCheck]->IsGuarded() );
	return JS_TRUE;
}

JSBool SE_CanMarkInRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( cwmWorldState->townRegions[toCheck]->CanMark() );
	return JS_TRUE;
}

JSBool SE_CanRecallInRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( cwmWorldState->townRegions[toCheck]->CanRecall() );
	return JS_TRUE;
}

JSBool SE_CanGateInRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 toCheck = (UI08)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( cwmWorldState->townRegions[toCheck]->CanGate() );
	return JS_TRUE;
}

JSBool SE_GetGuildType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	GUILDID toCheck = (GUILDID)JSVAL_TO_INT( argv[0] );
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
	GUILDID toCheck = (GUILDID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild	= GuildSys->Guild( toCheck );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}

	JSString *strSpeech = JS_NewStringCopyZ( cx, mGuild->Name().c_str() );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

JSBool SE_SetGuildType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GUILDID toCheck = (GUILDID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild	= GuildSys->Guild( toCheck );
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
	GUILDID targGuild	= (GUILDID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild		= GuildSys->Guild( targGuild );
	if( mGuild == NULL )
	{
		return JS_FALSE;
	}
	std::string test = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( test.empty() || test.length() == 0 )
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
	GUILDID toCheck = (GUILDID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild	= GuildSys->Guild( toCheck );
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
	GUILDID toCheck = (GUILDID)JSVAL_TO_INT( argv[0] );
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

JSBool SE_CompareGuildByGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GUILDID toCheck		= (GUILDID)JSVAL_TO_INT( argv[0] );
	GUILDID toCheck2	= (GUILDID)JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( GuildSys->Compare( toCheck, toCheck2 ) );
	return JS_TRUE;
}

JSBool SE_GetGuildStone( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	GUILDID toCheck = (GUILDID)JSVAL_TO_INT( argv[0] );
	CGuild *mGuild = GuildSys->Guild( toCheck );
	if( mGuild == NULL )
		*rval = INT_TO_JSVAL( INVALIDSERIAL );
	else
		*rval = INT_TO_JSVAL( mGuild->Stone() );
	return JS_TRUE;
}

JSBool SE_GetTownMayor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 town	= (UI08)JSVAL_TO_INT( argv[0] );
	CChar *mayor		= cwmWorldState->townRegions[town]->GetMayor();
	if( ValidateObject( mayor ) )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myObj, mayor );
		*rval				= OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval				= JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_GetTownRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		return JS_FALSE;
	}
	UI08 town	= (UI08)JSVAL_TO_INT( argv[0] );
	*rval		= INT_TO_JSVAL( cwmWorldState->townRegions[town]->GetRace() );
	return JS_TRUE;
}

JSBool SE_SetTownRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	UI08 town		= (UI08)JSVAL_TO_INT( argv[0] );
	RACEID nRace	= (RACEID)JSVAL_TO_INT( argv[1] );
	cwmWorldState->townRegions[town]->SetRace( nRace );
	return JS_TRUE;
}

JSBool SE_PossessTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	UI08 town	= (UI08)JSVAL_TO_INT( argv[0] );
	UI08 sTown	= (UI08)JSVAL_TO_INT( argv[1] );
	cwmWorldState->townRegions[town]->Possess( cwmWorldState->townRegions[sTown] );
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
	*rval = INT_TO_JSVAL( cwmWorldState->townRegions[town]->GetResourceID() );
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

JSBool SE_IsRaceWeakToWeather( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race		= (RACEID)JSVAL_TO_INT( argv[0] );
	weathID toCheck = (weathID)JSVAL_TO_INT( argv[1] );
	CRace *tRace	= Races->Race( race );
	if( tRace == NULL )
	{
		return JS_FALSE;
	}
	switch( toCheck )
	{
		case LIGHT:		*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByLight() );		break;
		case RAIN:		*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByRain() );		break;
		case COLD:		*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByCold() );		break;
		case HEAT:		*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByHeat() );		break;
		case LIGHTNING:	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedByLightning() );	break;
		case SNOW:		*rval = BOOLEAN_TO_JSVAL( tRace->AffectedBySnow() );		break;
		default:
			return JS_FALSE;
	};
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

JSBool SE_GetClosestTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	return JS_FALSE;
}

JSBool SE_UseDoor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "UseDoor: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSObject *mSock	= JSVAL_TO_OBJECT( argv[0] );
	JSObject *mDoor	= JSVAL_TO_OBJECT( argv[1] );

	CSocket *mySock	= (CSocket *)JS_GetPrivate( cx, mSock );
	CItem *myDoor	= (CItem *)JS_GetPrivate( cx, mDoor );

	if( !ValidateObject( myDoor ) )
	{
		DoSEErrorMessage( "UseDoor: Invalid door" );
		return JS_FALSE;
	}
	useDoor( mySock, myDoor );
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
 	char *eventToFire		= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	cScript *toExecute		= JSMapping->GetScript( scriptNumberToFire );

	if( toExecute == NULL || eventToFire == NULL )
		return JS_FALSE;

	if( toExecute->CallParticularEvent( eventToFire, &argv[2], argc - 2 ) )
		return JS_TRUE;
	else
		return JS_FALSE;
}

JSBool SE_GetPackOwner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "GetPackOwner: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UI08 mType		= (UI08)JSVAL_TO_INT( argv[1] );
	CChar *pOwner	= NULL;

	if( mType == 0 )	// item
	{
		JSObject *mItem	= JSVAL_TO_OBJECT( argv[0] );
		CItem *myItem	= (CItem *)JS_GetPrivate( cx, mItem );
		pOwner			= FindItemOwner( myItem );
	}
	else				// serial
	{
		SI32 mSerItem	= JSVAL_TO_INT( argv[0] );
		pOwner			= FindItemOwner( calcItemObjFromSer( mSerItem ) );
	}
	if( ValidateObject( pOwner ) )
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_CHAR );
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
	CSocket *sChar = (CSocket *)JS_GetPrivate( cx, mysockptr );
	if( sChar == NULL )
	{
		DoSEErrorMessage( "CalcTargetedItem: Invalid socket" );
		return JS_FALSE;
	}
	
	CItem *calcedItem = calcItemObjFromSer( sChar->GetDWord( 7 ) );
	if( !ValidateObject( calcedItem ) )
	{
		*rval = JSVAL_NULL;
	}
	else
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
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

	JSObject *mysockptr = JSVAL_TO_OBJECT( argv[0] );
	CSocket *sChar		= (CSocket *)JS_GetPrivate( cx, mysockptr );
	if( sChar == NULL )
	{
		DoSEErrorMessage( "CalcTargetedItem: Invalid socket" );
		return JS_FALSE;
	}
	
	CChar *calcedChar = calcCharObjFromSer( sChar->GetDWord( 7 ) );
	if( !ValidateObject( calcedChar ) )
	{
		*rval = JSVAL_NULL;
	}
	else
	{
		cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myObj, calcedChar );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	return JS_TRUE;
}

JSBool SE_GetTileIDAtMapCoord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "GetTileIDAtMapCoord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	UI16 xLoc		= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc		= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber = (UI08)JSVAL_TO_INT( argv[2] );
	map_st mMap		= Map->SeekMap0( xLoc, yLoc, wrldNumber );
	*rval			= INT_TO_JSVAL( mMap.id );
	return JS_TRUE;
}

JSBool SE_StaticInRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "StaticInRange: Invalid number of arguments (takes 5, x, y, world, radius, tile)" );
		return JS_FALSE;
	}

	UI16 xLoc		= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc		= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber = (UI08)JSVAL_TO_INT( argv[2] );
	UI16 radius		= (UI16)JSVAL_TO_INT( argv[3] );
	UI16 tileID		= (UI16)JSVAL_TO_INT( argv[4] );
	bool tileFound	= false;

	for( int i = xLoc - radius; i <= (xLoc + radius); ++i )
	{
		for( int j = yLoc - radius; j <= (yLoc + radius); ++j )
		{
			MapStaticIterator msi( xLoc, yLoc, wrldNumber );
			for( staticrecord *mRec = msi.First(); mRec != NULL; mRec = msi.Next() )
			{
				if( mRec != NULL && mRec->itemid == tileID )
				{
					tileFound = true;
					break;
				}
			}
		}
	}

	*rval			= BOOLEAN_TO_JSVAL( tileFound );
	return JS_TRUE;
}

JSBool SE_StaticAt( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 && argc != 3 )
	{
		DoSEErrorMessage( "StaticAt: Invalid number of arguments (takes 4, x, y, world, tile)" );
		return JS_FALSE;
	}

	UI16 xLoc		= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc		= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber = (UI08)JSVAL_TO_INT( argv[2] );
	UI16 tileID		= 0xFFFF;
	bool tileMatch	= false;
	if( argc == 4 )
	{
		tileID		= (UI16)JSVAL_TO_INT( argv[3] );
		tileMatch	= true;
	}
	bool tileFound	= false;

	MapStaticIterator msi( xLoc, yLoc, wrldNumber );

	for( staticrecord *mRec = msi.First(); mRec != NULL; mRec = msi.Next() )
	{
		if( mRec != NULL && (!tileMatch || (tileMatch && mRec->itemid == tileID) ) )
		{
			tileFound = true;
			break;
		}
	}
	*rval			= BOOLEAN_TO_JSVAL( tileFound );
	return JS_TRUE;
}

JSBool SE_StringToNum( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "StringToNum: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	UString str = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );

	*rval = INT_TO_JSVAL( str.toLong() );
	return JS_TRUE;
}

JSBool SE_NumToString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "NumToString: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	int num		= JSVAL_TO_INT( argv[0] );
	UString str = UString::number( num );
	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, str.c_str() ) );
	return JS_TRUE;
}

JSBool SE_NumToHexString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "NumToHexString: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	int num		= JSVAL_TO_INT( argv[0] );
	UString str = UString::number( num, 16 );
	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, str.c_str() ) );
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
	JSObject *srcSocketObj		= NULL;
	CSocket *srcSocket			= NULL;
 	char *trgFunc				= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgFunc == NULL )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 0 not a valid string" );
		return JS_FALSE;
	}

	JSObject *srcCharacterObj	= JSVAL_TO_OBJECT( argv[1] );
	CChar *srcChar				= (CChar *)JS_GetPrivate( cx, srcCharacterObj );

	if( !ValidateObject( srcChar ) )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 1 not a valid character" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		srcSocket		= (CSocket *)JS_GetPrivate( cx, srcCharacterObj );
	}
	
	UI16 retCounter				= 0;
	cScript *myScript			= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	REGIONLIST nearbyRegions	= MapRegion->PopulateList( srcChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) )
				continue;
			if( objInRange( srcChar, tempChar, (UI16)distance ) )
			{
				if( myScript->AreaObjFunc( trgFunc, srcChar, tempChar, srcSocket ) )
					++retCounter;
			}
		}
		regChars->Pop();
	}
	*rval = INT_TO_JSVAL( retCounter );
	return JS_TRUE;
}

JSBool SE_AreaItemFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		// function name, source character, range
		DoSEErrorMessage( "AreaItemFunction: Invalid number of arguments (takes 3/4, function name, source character, range, optional socket)" );
		return JS_FALSE;
	}

	// Do parameter validation here
	JSObject *srcSocketObj		= NULL;
	CSocket *srcSocket			= NULL;
 	char *trgFunc				= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgFunc == NULL )
	{
		DoSEErrorMessage( "AreaItemFunction: Argument 0 not a valid string" );
		return JS_FALSE;
	}

	JSObject *srcCharacterObj	= JSVAL_TO_OBJECT( argv[1] );
	CChar *srcChar				= (CChar *)JS_GetPrivate( cx, srcCharacterObj );

	if( !ValidateObject( srcChar ) )
	{
		DoSEErrorMessage( "AreaItemFunction: Argument 1 not a valid character" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		srcSocket		= (CSocket *)JS_GetPrivate( cx, srcCharacterObj );
	}
	
	UI16 retCounter					= 0;
	cScript *myScript				= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	REGIONLIST nearbyRegions		= MapRegion->PopulateList( srcChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
		{
			if( !ValidateObject( tempItem ) )
				continue;
			if( objInRange( srcChar, tempItem, (UI16)distance ) )
			{
				if( myScript->AreaObjFunc( trgFunc, srcChar, tempItem, srcSocket ) )
					++retCounter;
			}
		}
		regItems->Pop();
	}
	*rval = INT_TO_JSVAL( retCounter );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool SE_GetCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	1/13/2003 11:09:39 PM
//|	Developers		-	Brakhtus
//|	Organization	-	Forum Submission
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Extend the UOX3 JSE implementation to support scriptable 
//|						commands that players, and daminstration for a shard may
//|						use to performs specialized tasks.
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
	UI08 idx = static_cast<UI08>((JSVAL_TO_INT( argv[0] )));
 	if( idx >= Commands->NumArguments() ) 
	{ 		
		DoSEErrorMessage( "GetCommand: Index exeeds the command-array!" ); 		
		return JS_FALSE; 	
	} 	
	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, Commands->CommandString( idx ).c_str() ) ); 
	return JS_TRUE; 
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool SE_GetCommandSize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	1/13/2003 11:11:46 PM
//|	Developers		-	Brakhtus
//|	Organization	-	Forum Submission
//|	Status			-	Currently under development
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
	*rval = INT_TO_JSVAL( Commands->NumArguments() ); 	
	return JS_TRUE; 
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool SE_GetDictionaryEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	7/17/2004
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Allows the JSScripts to pull entries from the dictionaries
//|						and convert them to a string.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
JSBool SE_GetDictionaryEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 )
	{
		DoSEErrorMessage( "GetDictionaryEntry: Invalid number of arguments (takes at least 1)" );
		return JS_FALSE;
	}

	SI32 dictEntry = (SI32)JSVAL_TO_INT( argv[0] );
	UnicodeTypes language = ZERO;
	if( argc == 2 )
		language = (UnicodeTypes)JSVAL_TO_INT( argv[1] );
	std::string txt = Dictionary->GetEntry( dictEntry, language );

	JSString *strTxt = NULL;
	strTxt = JS_NewStringCopyZ( cx, txt.c_str() );
	*rval = STRING_TO_JSVAL( strTxt );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool SE_Yell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	7/17/2004
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Globally yell a message from JS (Based on Commandlevel)
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
JSBool SE_Yell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 3 )
	{
		DoSEErrorMessage( "Yell: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	JSObject *mSock			= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock			= (CSocket *)JS_GetPrivate( cx, mSock );
	CChar *myChar			= mySock->CurrcharObj();
	std::string textToYell	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	UI08 commandLevel		= (UI08)JSVAL_TO_INT( argv[2] );

	std::string yellTo = "";
	switch( (CommandLevels)commandLevel )
	{
	case PLAYER_CMDLEVEL:	yellTo = " (GLOBAL YELL): ";	break;
	case CNS_CMDLEVEL:		yellTo = " (CNS YELL): ";		break;
	case GM_CMDLEVEL:		yellTo = " (GM YELL): ";		break;
	case ADMIN_CMDLEVEL:	yellTo = " (ADMIN YELL): ";		break;
	}

	UString tmpString = myChar->GetName() + yellTo + textToYell;

	CSpeechEntry& toAdd = SpeechSys->Add();
	toAdd.Speech( tmpString );
	toAdd.Font( (FontType)myChar->GetFontType() );
	toAdd.Speaker( INVALIDSERIAL );
	toAdd.Colour( mySock->GetWord( 4 ) );
	toAdd.Type( SYSTEM );
	toAdd.At( cwmWorldState->GetUICurrentTime() );
	toAdd.TargType( SPTRG_BROADCASTALL );
	toAdd.CmdLevel( commandLevel );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool SE_Reload( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	29 Dec 2003
//|	Developers		-	Maarc
//|	Organization	-	Independent
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Reloads certain subsystems
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
JSBool SE_Reload( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval ) 
{ 	
	if( argc != 1 ) 	
	{ 		
		DoSEErrorMessage( "Reload: needs 1 argument!" ); 		
		return JS_FALSE; 	
	} 	

	int toLoad = JSVAL_TO_INT( argv[0] );

	switch( toLoad )
	{
	case 0:	// Reload regions
			UnloadRegions();
			LoadRegions();
			break;
	case 1:	// Reload spawn regions
			UnloadSpawnRegions();
			LoadSpawnRegions();
			break;
	case 2:	// Reload Spells
			Magic->LoadScript();
			break;
	case 3: // Reload Commands
			Commands->Load();
			break;
	case 4:	// Reload DFNs
			FileLookup->Reload();
			break;
	case 5: // Reload JScripts
			messageLoop << MSG_RELOADJS;
			break;
	case 6: // Reload HTMLTemplates
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			break;
	case 7:	// Reload INI
			cwmWorldState->ServerData()->load();
			break;
	case 8: // Reload Everything
			FileLookup->Reload();
			UnloadRegions();
			LoadRegions();
			UnloadSpawnRegions();
			LoadSpawnRegions();
			Magic->LoadScript();
			Commands->Load();
			messageLoop << MSG_RELOADJS;
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			cwmWorldState->ServerData()->load();
			break;
	case 9: // Reload Accounts
			Accounts->Load();
			break;
	default:
		break;
	}
	return JS_TRUE; 
}

JSBool SE_SendStaticStats( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 	
	if( argc != 1 ) 	
	{ 		
		DoSEErrorMessage( "SendStaticStats: needs 1 argument!" ); 		
		return JS_FALSE; 	
	} 

	JSObject *mSock			= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock			= (CSocket *)JS_GetPrivate( cx, mSock );
	if( mySock == NULL )
	{
		DoSEErrorMessage( "SendStaticStats: passed an invalid socket!" );
		return JS_FALSE;
	}
	CChar *myChar			= mySock->CurrcharObj();
	if( !ValidateObject( myChar ) )
		return JS_TRUE;

	CTile tile;
	if( mySock->GetDWord( 7 ) == 0 )
	{
		UI08 worldNumber	= myChar->WorldNumber();
		UI16 targetID		= mySock->GetWord( 0x11 );
		SI16 targetX		= mySock->GetWord( 0x0B );		// store our target x y and z locations
		SI16 targetY		= mySock->GetWord( 0x0D );
		SI08 targetZ		= mySock->GetByte( 0x10 );
		if( targetID != 0 )	// we might have a static rock or mountain
		{
			MapStaticIterator msi( targetX, targetY, worldNumber );
			staticrecord *stat = NULL;
			while( ( ( stat = msi.Next() ) != NULL ) )
			{
				msi.GetTile(&tile);
				if( targetZ == stat->zoff )
				{
					GumpDisplay staticStat( mySock, 300, 300 );
					staticStat.SetTitle( "Item [Static]" );
					staticStat.AddData( "ID", targetID, 5 );
					staticStat.AddData( "Height", tile.Height() );
					staticStat.AddData( "Name", tile.Name() );
					staticStat.Send( 4, false, INVALIDSERIAL );
				}
			}
		}
		else		// or it could be a map only
		{
			// manually calculating the ID's if a maptype
			map_st map1;
			CLand land;
			map1 = Map->SeekMap0( targetX, targetY, worldNumber );
			Map->SeekLand( map1.id, &land );
			GumpDisplay mapStat( mySock, 300, 300 );
			mapStat.SetTitle( "Item [Map]" );
			mapStat.AddData( "ID", targetID, 5 );
			mapStat.AddData( "Name", land.Name() );
			mapStat.Send( 4, false, INVALIDSERIAL );
		}
	}
	return JS_TRUE;
}

JSBool SE_GetTileHeight( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) 	
	{ 		
		DoSEErrorMessage( "GetTileHeight: needs 1 argument!" ); 		
		return JS_FALSE; 	
	}

	UI16 tileNum = (UI16)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( Map->TileHeight( tileNum ) );
	return JS_TRUE;
}

bool SE_IterateFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	cScript *myScript = (cScript *)extraData;
	return myScript->OnIterate( a, b );
}

JSBool SE_IterateOver( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 ) 	
	{ 		
		DoSEErrorMessage( "IterateOver: needs 1 argument!" ); 		
		return JS_FALSE; 	
	}

	UI32 b				= 0;
	std::string objType = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	ObjectType toCheck	= FindObjTypeFromString( objType );
	cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	if( myScript != NULL )
		ObjectFactory::getSingleton().IterateOver( toCheck, b, myScript, &SE_IterateFunctor );
	*rval = INT_TO_JSVAL( b );
	return JS_TRUE;
}

JSBool SE_WorldBrightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( "WorldBrightLevel: Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL brightLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->WorldLightBrightLevel( brightLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->WorldLightBrightLevel() );
	return JS_TRUE;
}

JSBool SE_WorldDarkLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( "WorldDarkLevel: Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL darkLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->WorldLightDarkLevel( darkLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->WorldLightDarkLevel() );
	return JS_TRUE;
}

JSBool SE_WorldDungeonLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( "WorldDungeonLevel: Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL dungeonLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->DungeonLightLevel( dungeonLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->DungeonLightLevel() );
	return JS_TRUE;
}

JSBool SE_GetSocketFromIndex( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetSocketFromIndex: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXSOCKET index = (UOXSOCKET)JSVAL_TO_INT( argv[0] );

	CSocket *mSock	= Network->GetSockPtr( index );
	CChar *mChar	= NULL;
	if( mSock != NULL )
		mChar = mSock->CurrcharObj();

	if( !ValidateObject( mChar ) )
	{
		*rval = JSVAL_NULL;
		return JS_FALSE;
	}

	cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	JSObject *myObj		= myScript->AcquireObject( IUE_CHAR );
	JS_SetPrivate( cx, myObj, mChar );
	*rval = OBJECT_TO_JSVAL( myObj );
	return JS_TRUE;
}

JSBool SE_ReloadJSFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ReloadJSFile: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	UI16 scriptID			= static_cast<UI16>(JSVAL_TO_INT( argv[0] ));
	if( scriptID == JSMapping->GetScriptID( JS_GetGlobalObject( cx ) ) )
	{
		DoSEErrorMessage( "ReloadJSFile: JS Script attempted to reload itself, crash avoided (ScriptID %u)", scriptID );
		return JS_FALSE;
	}

	JSMapping->Reload( scriptID );

 	return JS_TRUE;
}

JSBool SE_ResourceArea( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( "ResourceArea: Invalid Count of Arguments: %d", argc );
		return JS_FALSE;
	}

	UString resType = UString( JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) ) ).stripWhiteSpace().upper();
	if( argc == 2 )
	{
		UI16 newVal = static_cast<UI16>(JSVAL_TO_INT( argv[1] ));
		if( resType == "LOGS" )	// Logs
			cwmWorldState->ServerData()->ResLogArea( newVal );
		else if( resType == "ORE" )	// Ore
			cwmWorldState->ServerData()->ResOreArea( newVal );
	}

	if( resType == "LOGS" )
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResLogArea() );
	else if( resType == "ORE" )
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResOreArea() );

	return JS_TRUE;
}

JSBool SE_ResourceAmount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( "ResourceAmount: Invalid Count of Arguments: %d", argc );
		return JS_FALSE;
	}

	UString resType = UString( JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) ) ).stripWhiteSpace().upper();
	if( argc == 2 )
	{
		SI16 newVal = static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
		if( resType == "LOGS" )
			cwmWorldState->ServerData()->ResLogs( newVal );
		else if( resType == "ORE" )
			cwmWorldState->ServerData()->ResOre( newVal );
	}

	if( resType == "LOGS" )
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResLogs() );
	else if( resType == "ORE" )
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResOre() );

	return JS_TRUE;
}

JSBool SE_ResourceTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( "ResourceTime: Invalid Count of Arguments: %d", argc );
		return JS_FALSE;
	}

	UString resType = UString( JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) ) ).stripWhiteSpace().upper();
	if( argc == 2 )
	{
		UI16 newVal = static_cast<UI16>(JSVAL_TO_INT( argv[1] ));
		if( resType == "LOGS" )
			cwmWorldState->ServerData()->ResLogTime( newVal );
		else if( resType == "ORE" )
			cwmWorldState->ServerData()->ResOreTime( newVal );
	}

	if( resType == "LOGS" )
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResLogTime() );
	else if( resType == "ORE" )
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResOreTime() );

	return JS_TRUE;
}

JSBool SE_ResourceRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "ResourceRegion: Invalid number of arguments (takes 3)" );
 		return JS_FALSE;
	}
	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[2] ));
	MapResource_st *mRes = MapRegion->GetResource( x, y, worldNum );
	if( mRes == NULL )
	{
		DoSEErrorMessage( "ResourceRegion: Invalid Resource Region" );
		return JS_FALSE;
	}

	JSObject *jsResource = JS_NewObject( cx, &UOXResource_class, NULL, obj );
	JS_DefineProperties( cx, jsResource, CResourceProperties );
	JS_SetPrivate( cx, jsResource, mRes );

	*rval = OBJECT_TO_JSVAL( jsResource );

 	return JS_TRUE;
}

}
