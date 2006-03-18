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
#include "JSEncapsulate.h"
#include "CJSEngine.h"

namespace UOX
{

void		LoadTeleportLocations( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		UnloadRegions( void );
void		UnloadSpawnRegions( void );
void		LoadSkills( void );

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
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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
		JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, newChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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
	JSEncapsulate loVal( cx, &(argv[0]) );
	JSEncapsulate hiVal( cx, &(argv[1]) );
	*rval = INT_TO_JSVAL( RandomNum( loVal.toInt(), hiVal.toInt() ) );
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

	Commands->Register( toRegister, scriptID, execLevel, isEnabled );
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
		Console.Print( " " );
		Console.MoveTo( 15 );
		Console.Print( "Registering skill number " );
		Console.TurnYellow();
		Console.Print( "%i\n", skillNumber );
		Console.TurnNormal();
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

JSBool SE_RegisterKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterKey: Invalid number of arguments (takes 2)" );
 		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &(argv[0]) );
	std::string toRegister	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	UI16 scriptID			= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );

	if( scriptID == 0xFFFF )
	{
		DoSEErrorMessage( "RegisterKey: JS Script has an Invalid ScriptID" );
		return JS_FALSE;
	}	
	int toPass = 0;
	if( encaps.isType( JSOT_STRING ) )
	{
		std::string enStr = encaps.toString();
		if( enStr.length() != 0 )
			toPass  = enStr[0];
		else
		{
			DoSEErrorMessage( "RegisterKey: JS Script passed an invalid key to register" );
			return JS_FALSE;
		}
	}
	else
		toPass = encaps.toInt();
	Console.RegisterKey( toPass, toRegister, scriptID );
 	return JS_TRUE;
}

JSBool SE_RegisterConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterConsoleFunc: Invalid number of arguments (takes 2)" );
 		return JS_FALSE;
	}
	std::string funcToRegister	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	std::string toRegister		= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	UI16 scriptID				= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );

	if( scriptID == 0xFFFF )
	{
		DoSEErrorMessage( "RegisterConsoleFunc: JS Script has an Invalid ScriptID" );
		return JS_FALSE;
	}	

	Console.RegisterFunc( funcToRegister, toRegister, scriptID );
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

JSBool SE_DisableKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableKey: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	int toDisable	= JSVAL_TO_INT( argv[0] );
	Console.SetKeyStatus( toDisable, false );
 	return JS_TRUE;
}

JSBool SE_DisableConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableConsoleFunc: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	std::string toDisable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Console.SetFuncStatus( toDisable, false );
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

JSBool SE_EnableKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableKey: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	int toEnable	= JSVAL_TO_INT( argv[0] );
	Console.SetKeyStatus( toEnable, true );
 	return JS_TRUE;
}

JSBool SE_EnableConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableConsoleFunc: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}
	std::string toEnable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Console.SetFuncStatus( toEnable, false );
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
		JSObject *myobj		= JSEngine->AcquireObject( IUE_CHAR, cMade, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
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
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_GetItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		DoSEErrorMessage( "GetItem: Invalid number of parameters (4)" );
		return JS_FALSE;
	}
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	
	xLoc		= (SI16)JSVAL_TO_INT( argv[0] );
	yLoc		= (SI16)JSVAL_TO_INT( argv[1] );
	zLoc		= (SI08)JSVAL_TO_INT( argv[2] );
	worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
	
	CItem *item = GetItemAtXYZ( xLoc, yLoc, zLoc, worldNumber );
	if( ValidateObject( item ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, item, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool SE_FindItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "FindItem: Invalid number of parameters (5)" );
		return JS_FALSE;
	}
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 id = 0;
	
	xLoc		= (SI16)JSVAL_TO_INT( argv[0] );
	yLoc		= (SI16)JSVAL_TO_INT( argv[1] );
	zLoc		= (SI08)JSVAL_TO_INT( argv[2] );
	worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
	id			= (UI16)JSVAL_TO_INT( argv[4] );
	
	CItem *item = FindItemNearXYZ( xLoc, yLoc, zLoc, worldNumber, id );
	if( ValidateObject( item ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, item, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
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

JSBool SE_IsRaceWeakToWeather( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race		= (RACEID)JSVAL_TO_INT( argv[0] );
	weathID toCheck = (weathID)JSVAL_TO_INT( argv[1] );
	CRace *tRace	= Races->Race( race );
	if( tRace == NULL || toCheck < 0 || toCheck >= WEATHNUM )
	{
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedBy( (WeatherType)toCheck ) );
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
		JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, pOwner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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
		*rval = JSVAL_NULL;
	else
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, calcedItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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
		*rval = JSVAL_NULL;
	else
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, calcedChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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

	UI16 xLoc			= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc			= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber		= (UI08)JSVAL_TO_INT( argv[2] );
	const map_st mMap	= Map->SeekMap( xLoc, yLoc, wrldNumber );
	*rval				= INT_TO_JSVAL( mMap.id );
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
			CStaticIterator msi( xLoc, yLoc, wrldNumber );
			for( Static_st *mRec = msi.First(); mRec != NULL; mRec = msi.Next() )
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

	CStaticIterator msi( xLoc, yLoc, wrldNumber );
	for( Static_st *mRec = msi.First(); mRec != NULL; mRec = msi.Next() )
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

	JSObject *srcBaseObj	= JSVAL_TO_OBJECT( argv[1] );
	CBaseObject *srcObject		= (CBaseObject *)JS_GetPrivate( cx, srcBaseObj );

	if( !ValidateObject( srcObject ) )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 1 not a valid object" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		srcSocket		= (CSocket *)JS_GetPrivate( cx, srcSocketObj );
	}
	
	UI16 retCounter				= 0;
	cScript *myScript			= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	REGIONLIST nearbyRegions	= MapRegion->PopulateList( srcObject );
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
			if( objInRange( srcObject, tempChar, (UI16)distance ) )
			{
				if( myScript->AreaObjFunc( trgFunc, srcObject, tempChar, srcSocket ) )
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


	JSObject *srcBaseObj	= JSVAL_TO_OBJECT( argv[1] );
	CBaseObject *srcObject		= (CBaseObject *)JS_GetPrivate( cx, srcBaseObj );

	if( !ValidateObject( srcObject ) )
	{
		DoSEErrorMessage( "AreaItemFunction: Argument 1 not a valid object" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		srcSocket		= (CSocket *)JS_GetPrivate( cx, srcSocketObj );
	}
	
	UI16 retCounter					= 0;
	cScript *myScript				= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	REGIONLIST nearbyRegions		= MapRegion->PopulateList( srcObject );
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
			if( objInRange( srcObject, tempItem, (UI16)distance ) )
			{
				if( myScript->AreaObjFunc( trgFunc, srcObject, tempItem, srcSocket ) )
					++retCounter;
			}
		}
		regItems->Pop();
	}
	*rval = INT_TO_JSVAL( retCounter );
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
			LoadTeleportLocations();
			LoadSkills();
			Skills->Load();
			break;
	case 5: // Reload JScripts
			messageLoop << MSG_RELOADJS;
			break;
	case 6: // Reload HTMLTemplates
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			break;
	case 7:	// Reload INI
			cwmWorldState->ServerData()->Load();
			break;
	case 8: // Reload Everything
			FileLookup->Reload();
			UnloadRegions();
			LoadRegions();
			UnloadSpawnRegions();
			LoadSpawnRegions();
			Magic->LoadScript();
			Commands->Load();
			LoadSkills();
			Skills->Load();
			messageLoop << MSG_RELOADJS;
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			cwmWorldState->ServerData()->Load();
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

	if( mySock->GetDWord( 7 ) == 0 )
	{
		UI08 worldNumber	= myChar->WorldNumber();
		UI16 targetID		= mySock->GetWord( 0x11 );
		SI16 targetX		= mySock->GetWord( 0x0B );		// store our target x y and z locations
		SI16 targetY		= mySock->GetWord( 0x0D );
		SI08 targetZ		= mySock->GetByte( 0x10 );
		if( targetID != 0 )	// we might have a static rock or mountain
		{
			CStaticIterator msi( targetX, targetY, worldNumber );
			for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
			{
				CTile& tile = Map->SeekTile( stat->itemid );
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
			const map_st map1 = Map->SeekMap( targetX, targetY, worldNumber );
			CLand& land = Map->SeekLand( map1.id );
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

	JS_MaybeGC( cx );

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

	JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, mChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
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

JSBool SE_ValidateObject( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ValidateObject: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, &(argv[0]) );

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" ) 
	{
		CBaseObject *myObj	= (CBaseObject *)myClass.toObject();
		*rval				= BOOLEAN_TO_JSVAL( ValidateObject( myObj ) );
	}
	else
		*rval = JSVAL_FALSE;

 	return JS_TRUE;
}

JSBool SE_ApplyDamageBonuses( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Invalid number of arguments (takes 6)" );
 		return JS_FALSE;
	}
	
	CChar *attacker	= NULL, *defender = NULL;
	SI16 damage = 0;

	JSEncapsulate damageType( cx, &( argv[0] ) );
	JSEncapsulate getFightSkill( cx, &( argv[3] ) );
	JSEncapsulate hitLoc( cx, &( argv[4] ) );
	JSEncapsulate baseDamage( cx, &(argv[5]) );

	JSEncapsulate attackerClass( cx, &(argv[1]) );
	if( attackerClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_FALSE;
	}

	if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ) )
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_TRUE;
	}
	else
	{
		attacker	= static_cast<CChar *>(attackerClass.toObject());
		if( !ValidateObject( attacker )  )
		{
			DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
			return JS_TRUE;
		}
	}

	JSEncapsulate defenderClass( cx, &(argv[1]) );
	if( defenderClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_FALSE;
	}

	if( defenderClass.isType( JSOT_VOID ) || defenderClass.isType( JSOT_NULL ) )
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_TRUE;
	}
	else
	{
		defender	= static_cast<CChar *>(defenderClass.toObject());
		if( !ValidateObject( defender )  )
		{
			DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
			return JS_TRUE;
		}
	}

	damage = Combat->ApplyDamageBonuses( (WeatherType)damageType.toInt(), attacker, defender, (UI08)getFightSkill.toInt(), (UI08)hitLoc.toInt(), (SI16)baseDamage.toInt() );
	
	*rval				= INT_TO_JSVAL( damage );

	return JS_TRUE;
}

JSBool SE_ApplyDefenseModifiers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		DoSEErrorMessage( "ApplyDefenseModifiers: Invalid number of arguments (takes 7)" );
 		return JS_FALSE;
	}
	
	CChar *attacker	= NULL, *defender = NULL;
	SI16 damage = 0;

	JSEncapsulate damageType( cx, &( argv[0] ) );
	JSEncapsulate getFightSkill( cx, &( argv[3] ) );
	JSEncapsulate hitLoc( cx, &( argv[4] ) );
	JSEncapsulate baseDamage( cx, &(argv[5]) );
	JSEncapsulate doArmorDamage(cx, &( argv[6] ) );

	JSEncapsulate attackerClass( cx, &(argv[1]) );
	if( attackerClass.ClassName() == "UOXChar" )
	{
		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ) )
		{
			attacker = NULL;
		}
		else
		{
			attacker	= static_cast<CChar *>(attackerClass.toObject());
			if( !ValidateObject( attacker )  )
			{
				attacker = NULL;
			}
		}
	}

	JSEncapsulate defenderClass( cx, &(argv[1]) );
	if( defenderClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		DoSEErrorMessage( "ApplyDefenseModifiers: Passed an invalid Character" );
		return JS_FALSE;
	}

	if( defenderClass.isType( JSOT_VOID ) || defenderClass.isType( JSOT_NULL ) )
	{
		DoSEErrorMessage( "ApplyDefenseModifiers: Passed an invalid Character" );
		return JS_TRUE;
	}
	else
	{
		defender	= static_cast<CChar *>(defenderClass.toObject());
		if( !ValidateObject( defender )  )
		{
			DoSEErrorMessage( "ApplyDefenseModifiers: Passed an invalid Character" );
			return JS_TRUE;
		}
	}

	damage = Combat->ApplyDefenseModifiers( (WeatherType)damageType.toInt(), attacker, defender, (UI08)getFightSkill.toInt(), (UI08)hitLoc.toInt(), (SI16)baseDamage.toInt(), doArmorDamage.toBool() );
	
	*rval				= INT_TO_JSVAL( damage );

	return JS_TRUE;
}

}
