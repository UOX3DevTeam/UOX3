#include "uox3.h"
#include "cScript.h"
#include "power.h"
#include "UOXJSClasses.h"
#include "SEFunctions.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"
#include "CJSMapping.h"
#include "CPacketReceive.h"
#include "CJSEngine.h"
#include "JSEncapsulate.h"
#include "cSpawnRegion.h"
#include "StringUtility.hpp"
#include "osunique.hpp"

//o------------------------------------------------------------------------------------------------o
//|	File		-	cScript.cpp
//|	Date		-	August 26th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles JS events
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|					1.0		26th August, 2000
//|					Initial implementation
//|					The basic event model is in, along with some fairly simple error handling
//|					Note that parameters for each event is NOT correct at all, it was just to get the basic layout right
//|					Support for CS type stuff with IsFiring(), Firing(), and Stop()
//|
//|					1.1		6th September, 2000
//|					Each function has params and passes it along into the the script as required
//|
//|					1.2		18th September, 2000
//|					Addition of a number of more events, not all with params
//|					More functions added to the functions table
//|					Support for dynamic reloading of scripts
//|
//|					1.3		2nd November, 2000
//|					Addition of a number of events.  Caching of function presence added
//|					All functions assumed present, only failure on a call will flag not present
//|
//|					1.4		1st July, 2004
//|					Added private property of SCRIPTTYPE to store which type of script we've got
//|					Preparation for JS scriptable spells
//o------------------------------------------------------------------------------------------------o

static JSFunctionSpec my_functions[] =
{ 	// ScriptName, Func Ptr, num args, ?, ?
	{ "StringToNum",				SE_StringToNum,				1, 0, 0 },	// This function will be depreciated
	{ "NumToString",				SE_NumToString,				1, 0, 0 },	// This function will be depreciated
	{ "NumToHexString",				SE_NumToHexString,			1, 0, 0 },	// This function will be depreciated
	{ "DoMovingEffect", 			SE_DoMovingEffect,			6, 0, 0 },
	{ "DoStaticEffect", 			SE_DoStaticEffect,			7, 0, 0 },
	{ "BroadcastMessage",			SE_BroadcastMessage,		1, 0, 0 },
	{ "RandomNumber",				SE_RandomNumber,			2, 0, 0 },
	{ "CalcCharFromSer",			SE_CalcCharFromSer,			1, 0, 0 },
	{ "CalcItemFromSer",			SE_CalcItemFromSer,			1, 0, 0 },
	{ "CalcMultiFromSer",			SE_CalcMultiFromSer,		1, 0, 0 },
	{ "CheckInstaLog",				SE_CheckInstaLog,			4, 0, 0 },
	{ "GetHour",					SE_GetHour,					0, 0, 0 },
	{ "GetMinute",					SE_GetMinute,				0, 0, 0 },
	{ "GetDay",						SE_GetDay,					0, 0, 0 },
	{ "SecondsPerUOMinute",			SE_SecondsPerUOMinute,		0, 0, 0 },
	{ "GetCurrentClock",			SE_GetCurrentClock,			0, 0, 0 },
	{ "GetStartTime",				SE_GetStartTime,			0, 0, 0 },
	{ "GetMurderThreshold",			SE_GetMurderThreshold,		0, 0, 0 },
	{ "RollDice",					SE_RollDice,				3, 0, 0 },
	{ "RaceCompareByRace",			SE_RaceCompareByRace,		2, 0, 0 },
	{ "GetRandomSOSArea",			SE_GetRandomSOSArea,		2, 0, 0 },
	{ "DoTempEffect",				SE_DoTempEffect,			7, 0, 0 },
	{ "MakeItem",					SE_MakeItem,				3, 0, 0 },
	{ "FindMulti",					SE_FindMulti,				4, 0, 0 },
	{ "GetItem",					SE_GetItem,					4, 0, 0 },
	{ "FindItem",					SE_FindItem,				5, 0, 0 },
	{ "PossessTown",				SE_PossessTown,				2, 0, 0 },
	{ "IsRaceWeakToWeather",		SE_IsRaceWeakToWeather,		2, 0, 0 },
	{ "GetRaceSkillAdjustment",		SE_GetRaceSkillAdjustment,	2, 0, 0 },
	{ "UseItem",					SE_UseItem,					2, 0, 0 },
	{ "TriggerTrap",				SE_TriggerTrap,				2, 0, 0 },
	{ "CompareGuildByGuild",		SE_CompareGuildByGuild,		2, 0, 0 },
	{ "CommandLevelReq",			SE_CommandLevelReq,			1, 0, 0 },
	{ "CommandExists",				SE_CommandExists,			1, 0, 0 },
	{ "FirstCommand",				SE_FirstCommand,			0, 0, 0 },
	{ "NextCommand",				SE_NextCommand,				0, 0, 0 },
	{ "FinishedCommandList",		SE_FinishedCommandList,		0, 0, 0 },
	{ "CreateDFNItem",				SE_CreateDFNItem,			3, 0, 0 },
	{ "CreateBlankItem",			SE_CreateBlankItem,			8, 0, 0 },
	{ "CreateHouse",				SE_CreateHouse,				8, 0, 0 },
	{ "CreateBaseMulti",			SE_CreateBaseMulti,			8, 0, 0 },
	{ "SpawnNPC",					SE_SpawnNPC,				6, 0, 0 },
	{ "GetPackOwner",				SE_GetPackOwner,			2, 0, 0 },
	{ "FindRootContainer",			SE_FindRootContainer,		2, 0, 0 },
	{ "CalcTargetedItem",			SE_CalcTargetedItem,		1, 0, 0 },
	{ "CalcTargetedChar",			SE_CalcTargetedChar,		1, 0, 0 },
	{ "GetTileIDAtMapCoord",		SE_GetTileIdAtMapCoord,		3, 0, 0 },
	{ "GetDictionaryEntry",			SE_GetDictionaryEntry,		2, 0, 0 },
	{ "Yell",						SE_Yell,					3, 0, 0 },
	{ "GetRaceCount",				SE_GetRaceCount,			0, 0, 0 },
	{ "WorldBrightLevel",			SE_WorldBrightLevel,		0, 0, 0 },
	{ "WorldDarkLevel",				SE_WorldDarkLevel,			0, 0, 0 },
	{ "WorldDungeonLevel",			SE_WorldDungeonLevel,		0, 0, 0 },
	{ "GetSpawnRegionFacetStatus",	SE_GetSpawnRegionFacetStatus,	1, 0, 0 },
	{ "SetSpawnRegionFacetStatus",	SE_SetSpawnRegionFacetStatus,	2, 0, 0 },
	{ "GetMoongateFacetStatus",		SE_GetMoongateFacetStatus,	1, 0, 0 },
	{ "SetMoongateFacetStatus",		SE_SetMoongateFacetStatus,	2, 0, 0 },
	{ "AreaCharacterFunction",		SE_AreaCharacterFunction,	3, 0, 0 },
	{ "AreaItemFunction",			SE_AreaItemFunction,		3, 0, 0 },
	{ "TriggerEvent",				SE_TriggerEvent,			3, 0, 0 },
	{ "DoesEventExist",				SE_DoesEventExist,			2, 0, 0 },
	{ "Reload",						SE_Reload,					1, 0, 0 },
	{ "SendStaticStats",			SE_SendStaticStats,			1, 0, 0 },
	{ "GetTileHeight",				SE_GetTileHeight,			1, 0, 0 },
	{ "IterateOver",				SE_IterateOver,				1, 0, 0 },
	{ "IterateOverSpawnRegions",	SE_IterateOverSpawnRegions,	1, 0, 0 },
	{ "GetSocketFromIndex",			SE_GetSocketFromIndex,		1, 0, 0 },
	{ "StaticAt",					SE_StaticAt,				4, 0, 0 },
	{ "StaticInRange",				SE_StaticInRange,			5, 0, 0 },

	{ "GetMapElevation",			SE_GetMapElevation,			3, 0, 0 },
	{ "IsInBuilding",				SE_IsInBuilding,			6, 0, 0 },
	{ "CheckStaticFlag",			SE_CheckStaticFlag,			5, 0, 0 },
	{ "CheckDynamicFlag",			SE_CheckDynamicFlag,		6, 0, 0 },
	{ "CheckTileFlag",				SE_CheckTileFlag,			2, 0, 0 },
	{ "DoesDynamicBlock",			SE_DoesDynamicBlock,		9, 0, 0 },
	{ "DoesStaticBlock",			SE_DoesStaticBlock,			5, 0, 0 },
	{ "DoesMapBlock",				SE_DoesMapBlock,			8, 0, 0 },
	{ "DoesCharacterBlock",			SE_DoesCharacterBlock,		5, 0, 0 },
	{ "DistanceBetween",			SE_DistanceBetween,			4, 0, 0 },

	{ "ResourceArea",				SE_ResourceArea,			2, 0, 0 },
	{ "ResourceAmount",				SE_ResourceAmount,			2, 0, 0 },
	{ "ResourceTime",				SE_ResourceTime,			2, 0, 0 },
	{ "ResourceRegion",				SE_ResourceRegion,			3, 0, 0 },
	{ "Moon",						SE_Moon,					2, 0, 0 },

	{ "GetTownRegion",				SE_GetTownRegion,			1, 0, 0 },
	{ "GetTownRegionFromXY",		SE_GetTownRegionFromXY,		4, 0, 0 },
	{ "GetSpawnRegion",				SE_GetSpawnRegion,			4, 0, 0 },
	{ "GetSpawnRegionCount",		SE_GetSpawnRegionCount,		0, 0, 0 },


	{ "RegisterCommand",			SE_RegisterCommand,			3, 0, 0 },
	{ "DisableCommand",				SE_DisableCommand,			1, 0, 0 },
	{ "EnableCommand",				SE_EnableCommand,			1, 0, 0 },

	{ "RegisterKey",				SE_RegisterKey,				2, 0, 0 },
	{ "DisableKey",					SE_DisableKey,				1, 0, 0 },
	{ "EnableKey",					SE_EnableKey,				1, 0, 0 },

	{ "RegisterConsoleFunc",		SE_RegisterConsoleFunc,		2, 0, 0 },
	{ "DisableConsoleFunc",			SE_DisableConsoleFunc,		1, 0, 0 },
	{ "EnableConsoleFunc",			SE_EnableConsoleFunc,		1, 0, 0 },

	{ "RegisterSpell",				SE_RegisterSpell,			2, 0, 0 },
	{ "DisableSpell",				SE_DisableSpell,			1, 0, 0 },
	{ "EnableSpell",				SE_EnableSpell,				1, 0, 0 },

	{ "RegisterSkill",				SE_RegisterSkill,			2, 0, 0 },
	{ "RegisterPacket",				SE_RegisterPacket,			2, 0, 0 },
	{ "ReloadJSFile",				SE_ReloadJSFile,			1, 0, 0 },

	{ "ValidateObject",				SE_ValidateObject,			1, 0, 0 },

	{ "ApplyDamageBonuses",			SE_ApplyDamageBonuses,		6, 0, 0 },
	{ "ApplyDefenseModifiers",		SE_ApplyDefenseModifiers,	7, 0, 0 },
	{ "WillResultInCriminal",		SE_WillResultInCriminal,	2, 0, 0 },

	{ "CreateParty",				SE_CreateParty,				1, 0, 0 },

	{ "GetClientFeature",			SE_GetClientFeature,		1, 0, 0 },
	{ "GetServerFeature",			SE_GetServerFeature,		1, 0, 0 },
	{ "GetServerSetting",			SE_GetServerSetting,		1, 0, 0 },

	{ "DeleteFile",					SE_DeleteFile,				2, 0, 0 },

	{ "GetAccountCount",			SE_GetAccountCount,			0, 0, 0 },
	{ "GetPlayerCount",				SE_GetPlayerCount,			0, 0, 0 },
	{ "GetItemCount",				SE_GetItemCount,			0, 0, 0 },
	{ "GetMultiCount",				SE_GetMultiCount,			0, 0, 0 },
	{ "GetCharacterCount",			SE_GetCharacterCount,		0, 0, 0 },
	{ "GetServerVersionString",		SE_GetServerVersionString,	0, 0, 0 },
	{ "EraStringToNum",				SE_EraStringToNum,			1, 0, 0 },

	{ "BASEITEMSERIAL",				SE_BASEITEMSERIAL,			0, 0, 0 },
	{ "INVALIDSERIAL",				SE_INVALIDSERIAL,			0, 0, 0 },
	{ "INVALIDID",					SE_INVALIDID,				0, 0, 0 },
	{ "INVALIDCOLOUR",				SE_INVALIDCOLOUR,			0, 0, 0 },

	{ nullptr,						nullptr,					0, 0, 0 },
};

void UOX3ErrorReporter( JSContext *cx, const char *message, JSErrorReport *report )
{
	UI16 scriptNum = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));
	// If we're loading the world then do NOT print out anything!
	Console.Error( oldstrutil::format( "JS script failure: Script Number (%u) Message (%s)", scriptNum, message ));
	if( report == nullptr || report->filename == nullptr )
	{
		Console.Error( "No detailed data" );
		return;
	}
	Console.Error( oldstrutil::format( "Filename: %s\n| Line Number: %i", report->filename, report->lineno ));
	if( report->linebuf != nullptr || report->tokenptr != nullptr )
	{
		Console.Error( oldstrutil::format( "Erroneous Line: %s\n| Token Ptr: %s", report->linebuf, report->tokenptr ));
	}
}

// Global error message variable used to pass error message from MethodError() to the custom JSError callback function
std::string g_errorMessage;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ScriptErrorCallback()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Callback for custom JSError
//| Notes		-	Relies on global variable g_errorMessage to pass in error message from
//|					MethodError function.
//o------------------------------------------------------------------------------------------------o
const JSErrorFormatString* ScriptErrorCallback( [[maybe_unused]] void *userRef, [[maybe_unused]] const char *locale, [[maybe_unused]] const uintN errorNumber )
{
	// Return a pointer to a JSErrorFormatString, to the UOX3ErrorReporter function in cScript.cpp
	static JSErrorFormatString errorFormat;
	errorFormat.format = g_errorMessage.c_str();
	errorFormat.argCount = 0;
	errorFormat.exnType = JSEXN_ERR;
	return &errorFormat;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ScriptError()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Spit out a custom error message related to JS Methods or their parameters
//| Notes		-	Piggybacks on the internal error reporting mechanism in SpiderMonkey to get
//|					the filename and relevant line number from associated script
//o------------------------------------------------------------------------------------------------o
void ScriptError( JSContext *cx, const char *txt, ... )
{
	// Combine error message with any potential additional arguments provided, store in g_errorMessage
	va_list argptr;
	va_start( argptr, txt );
	g_errorMessage = oldstrutil::format( txt, argptr );
	va_end( argptr );

	// Define a custom error number. Needed, but not really used for anything
	const uintN customErrorNumber = 1000;

	// Manually trigger an error using SpiderMonkey's internal error reporting,
	// which makes use of JSErrorFormatString from ScriptErrorCallback function
	// to call upon UOX3ErrorReporter function in cScript.cpp
	JS_ReportErrorNumber( cx, ScriptErrorCallback, nullptr, customErrorNumber, "" );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SI32 TryParseJSVal( jsval toParse )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper function to parse jsval returned from script
//o------------------------------------------------------------------------------------------------o
SI32 TryParseJSVal( jsval toParse )
{
	if( JSVAL_IS_NULL( toParse ) || ( !JSVAL_IS_BOOLEAN( toParse ) && !JSVAL_IS_INT( toParse )))
	{
		// jsval is neither a bool nor an int - possibly an object!
		return 0;
	}
	else if( JSVAL_IS_BOOLEAN( toParse ) == JS_FALSE && JSVAL_IS_INT( toParse ))
	{
		// jsval is an int!
		return static_cast<SI32>( JSVAL_TO_INT( toParse ));
	}
	else if( JSVAL_IS_BOOLEAN( toParse ) == JS_TRUE )
	{
		// jsval is a bool! convert it to int
		if( JSVAL_TO_BOOLEAN( toParse ) == JS_TRUE )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

cScript::cScript( std::string targFile, UI08 rT ) : isFiring( false ), runTime( rT )
{
	for( SI32 i = 0; i < 3; ++i )
	{
		eventPresence[i].set();
		needsChecking[i].set();
	}

	targContext = JSEngine->GetContext( runTime ); //JS_NewContext( JSEngine->GetRuntime( runTime ), 0x2000 );
	if( targContext == nullptr )
		return;

	targObject = JS_NewObject( targContext, &uox_class, nullptr, nullptr );
	if( targObject == nullptr )
		return;

	JS_LockGCThing( targContext, targObject );
	//JS_AddRoot( targContext, &targObject );

	// Moved here so it reports errors during script-startup too
	JS_SetErrorReporter( targContext, UOX3ErrorReporter );

	JS_SetGlobalObject( targContext, targObject );

	JS_InitStandardClasses( targContext, targObject );
	JS_DefineFunctions( targContext, targObject, my_functions );
	targScript = JS_CompileFile( targContext, targObject, targFile.c_str() );
	if( targScript == nullptr )
	{
		throw std::runtime_error( "Compilation failed" );
	}
	jsval rval;
	JSBool ok = JS_ExecuteScript( targContext, targObject, targScript, &rval );
	if( ok != JS_TRUE )
	{
		JSString *str = JS_ValueToString( targContext, rval );
		Console << "script result: " << JS_GetStringBytes( str ) << myendl;
	}
}

void cScript::Cleanup( void )
{
	size_t i = 0;
	for( i = 0; i < gumpDisplays.size(); ++i )
	{
		delete gumpDisplays[i];
	}
	gumpDisplays.resize( 0 );

	JS_UnlockGCThing( targContext, targObject );
	//JS_RemoveRoot( targContext, &targObject );
}
void cScript::CollectGarbage( void )
{
	//	if( JS_IsRunning( targContext ) == JS_FALSE )
	//	{
	Cleanup();
	JS_LockGCThing( targContext, targObject );
	//JS_AddRoot( targContext, &targObject );
	//	}
}
cScript::~cScript()
{
	JS_GC( targContext );
	if( targScript != nullptr )
	{
		JS_DestroyScript( targContext, targScript );
	}
	Cleanup();
	JS_GC( targContext );
	//	if( targContext != nullptr )
	//		JS_DestroyContext( targContext );
}

bool cScript::IsFiring( void )
{
	return isFiring;
}
void cScript::Firing( void )
{
	isFiring = true;
}
void cScript::Stop( void )
{
	isFiring = false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStart()
//|	Date		-	8/16/2003 3:44:50 AM
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	The OnStart event is provided to allow a script to process
//|					and read in any state information that has been saved from
//|					a previous server shut down. If a a script come with an
//|					OnStart event the code that is provided will be executed
//|					just following the loading of the script.
//o------------------------------------------------------------------------------------------------o
bool cScript::OnStart( void )
{
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStop()
//|	Date		-	8/16/2003 3:44:44 AM
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	The OnStop event is provided to allow a script to perform
//|					any special cleanup, or state saving as a server shuts
//|					down. If a script has an OnStop event then any code that
//|					is provided will be executed just prior to the JSE shut
//|					down.
//o------------------------------------------------------------------------------------------------o
bool cScript::OnStop( void )
{
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::DoesEventExist()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows scripters to check if a particular JS event (or function) exists in a
//|					script before attempting to call it via TriggerEvent
//o------------------------------------------------------------------------------------------------o
bool cScript::DoesEventExist( char *eventToFind )
{
	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, eventToFind, &Func );
	if( Func == JSVAL_VOID )
	{
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCreate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs when an object is added/created
//|	Notes		-	Checks for the presence of onCreateDFN by default, but onCreateTile can also
//|					be used to intercept creation of items directly from tiles/harditems.dfn
//o------------------------------------------------------------------------------------------------o
bool cScript::OnCreate( CBaseObject *thingCreated, bool dfnCreated, bool isPlayer)
{
	if( !ValidateObject( thingCreated ))
		return false;

	std::string functionName = "onCreateDFN";
	if( isPlayer )
	{
		functionName = "onCreatePlayer";
		if( !ExistAndVerify( seOnCreatePlayer, functionName ))
			return false;
	}
	else if( !dfnCreated )
	{
		functionName = "onCreateTile";
		if( !ExistAndVerify( seOnCreateTile, functionName ))
			return false;
	}
	else
	{
		if( !ExistAndVerify( seOnCreateDFN, functionName ))
			return false;
	}

	jsval rval, params[2];
	UI08 paramType = 0;
	JSObject *myObj;
	if( thingCreated->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingCreated, runTime );
		paramType = 1;
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingCreated, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, functionName.c_str(), 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		if( isPlayer )
		{
			SetEventExists( seOnCreatePlayer, false );
		}
		else if( !dfnCreated )
		{
			SetEventExists( seOnCreateTile, false );
		}
		else
		{
			SetEventExists( seOnCreateDFN, false );
		}
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDelete()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs when an object is deleted
//o------------------------------------------------------------------------------------------------o
bool cScript::OnDelete( CBaseObject *thingDestroyed )
{
	if( !ValidateObject( thingDestroyed ))
		return false;

	if( !ExistAndVerify( seOnDelete, "onDelete" ))
		return false;

	jsval rval, params[2];
	UI08 paramType = 0;
	JSObject *myObj;
	if( thingDestroyed->GetObjType() != OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingDestroyed, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingDestroyed, runTime );
		paramType = 1;
	}
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDelete", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDelete, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpeech()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when a character talks in range of the character with event attached
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 4 possible values
//|					-1	=> No such function or bad call
//|					0	=> Let other NPCs and PCs see it
//|					1	=> Let other PCs see it
//|					2	=> Let no one else see it
//|					If JS returns non-int and non-bool, default to 0
//|					If JS returns bool, true == 2, false == 0
//o------------------------------------------------------------------------------------------------o
//| Changes		-	22 June, 2003 17:30 (making it version 3)
//|						Changed return values from bool to SI08
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpeech( const char *speech, CChar *personTalking, CBaseObject *talkingTo )
{
	const SI08 RV_NOFUNC = -1;
	if( speech == nullptr || !ValidateObject( personTalking ) || !ValidateObject( talkingTo ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpeech, "onSpeech" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSString *strSpeech 	= nullptr;
	std::string lwrSpeech	= speech;

	strSpeech = JS_NewStringCopyZ( targContext, oldstrutil::lower( lwrSpeech ).c_str() );

	JSObject *ptObj = JSEngine->AcquireObject( IUE_CHAR, personTalking, runTime );
	JSObject *ttObj = nullptr;
	if( talkingTo->CanBeObjType( OT_CHAR ))
	{
		ttObj = JSEngine->AcquireObject( IUE_CHAR, talkingTo, runTime );
	}
	else if( talkingTo->CanBeObjType( OT_ITEM ))
	{
		ttObj = JSEngine->AcquireObject( IUE_ITEM, talkingTo, runTime );
	}

	params[0] = STRING_TO_JSVAL( strSpeech );
	params[1] = OBJECT_TO_JSVAL( ptObj );
	params[2] = OBJECT_TO_JSVAL( ttObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpeech", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpeech, false );
		return RV_NOFUNC;
	}

	if( !( JSVAL_IS_NULL( rval )))	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ))
		{
			return static_cast<SI08>(JSVAL_TO_INT( rval ));
		}
		else if( JSVAL_IS_BOOLEAN( rval ))
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				return 2;
		}
	}

	return 0;	// return default
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::InRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when an object comes in range of a character with the event attached
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	A third parameter is provided to let script know whether object that comes into
//|					range is a character or an item (Multi not supported yet)
//o------------------------------------------------------------------------------------------------o
bool cScript::InRange( CBaseObject *srcObj, CBaseObject *objInRange )
{
	if( !ValidateObject( srcObj ) || !ValidateObject( objInRange ))
		return false;

	if( !ExistAndVerify( seInRange, "inRange" ))
		return false;

	jsval params[3], rval;
	JSObject *myObj;
	if( srcObj->CanBeObjType( OT_CHAR ))
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, srcObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, srcObj, runTime );
	}

	JSObject *myObj2;
	if( objInRange->CanBeObjType( OT_CHAR ))
	{
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objInRange, runTime );
	}
	else
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objInRange, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "inRange", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seInRange, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCollide()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when a character collides with it
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCollide( CSocket *tSock, CChar *objColliding, CBaseObject *objCollideWith )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objColliding ) || !ValidateObject( objCollideWith ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCollide, "onCollide" ))
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objColliding, runTime );
	JSObject *myObj2	= nullptr;
	if( objCollideWith->GetObjType() == OT_CHAR )
	{
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objCollideWith, runTime );
	}
	else
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objCollideWith, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	if( JS_CallFunctionName( targContext, targObject, "onCollide", 3, params, &rval ) == JS_FALSE )
	{
		SetEventExists( seOnCollide, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnMoveDetect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when movement is detected within 5 tiles
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnMoveDetect( CBaseObject *sourceObj, CChar *charInRange, UI08 rangeToChar, UI16 oldCharX, UI16 oldCharY )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( sourceObj ) || !ValidateObject( charInRange ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnMoveDetect, "onMoveDetect" ))
		return RV_NOFUNC;

	jsval rval, params[5];
	JSObject *myObj	= nullptr;
	if( sourceObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, sourceObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, sourceObj, runTime );
	}
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, charInRange, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = INT_TO_JSVAL( rangeToChar );
	params[3] = INT_TO_JSVAL( oldCharX );
	params[4] = INT_TO_JSVAL( oldCharY );

	if( JS_CallFunctionName( targContext, targObject, "onMoveDetect", 5, params, &rval ) == JS_FALSE )
	{
		SetEventExists( seOnMoveDetect, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSteal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when stolen
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSteal( CChar *thief, CItem *theft, CChar *victim )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( thief ) || !ValidateObject( theft ) || !ValidateObject( victim ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSteal, "onSteal" ))
		return RV_NOFUNC;

	JSObject *thiefCharObj = JSEngine->AcquireObject( IUE_CHAR, thief, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, theft, runTime );
	JSObject *victimCharObj = JSEngine->AcquireObject( IUE_CHAR, victim, runTime );

	jsval params[3], rval;
	params[0] = OBJECT_TO_JSVAL( thiefCharObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	params[2] = OBJECT_TO_JSVAL( victimCharObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSteal", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSteal, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDispel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when dispelled
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDispel( CBaseObject *dispelled )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( dispelled ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDispel, "onDispel" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *myObj;
	if( dispelled->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, dispelled, runTime );
		params[1] = INT_TO_JSVAL( 0 );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, dispelled, runTime );
		params[1] = INT_TO_JSVAL( 1 );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDispel", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDispel, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when using a skill
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	If used WITH RegisterSkill() in a script listed under SKILLUSE_SCRIPTS in
//|					JSE_FILEASSOCIATIONS.SCP, event will trigger for only the specified skill.
//|					If used WITHOUT RegisterSkill() in a general purpose script listed under
//|					SCRIPT_LIST instead, event will be a global listener for ALL skills used
//o------------------------------------------------------------------------------------------------o
bool cScript::OnSkill( CBaseObject *skillUse, SI08 skillUsed )
{
	if( !ValidateObject( skillUse ))
		return false;

	if( !ExistAndVerify( seOnSkill, "onSkill" ))
		return false;

	jsval rval, params[3];
	JSObject *myObj;
	if( skillUse->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, skillUse, runTime );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, skillUse, runTime );
		params[2] = INT_TO_JSVAL( 1 );
	}
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( skillUsed );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkill", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkill, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnTooltip()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for objects which server is about to send tooltip to client for
//o------------------------------------------------------------------------------------------------o
std::string cScript::OnTooltip( CBaseObject *myObj, CSocket *pSocket )
{
	if( !ValidateObject( myObj ))
		return "";

	if( !ExistAndVerify( seOnTooltip, "onTooltip" ))
		return "";

	jsval rval, params[2];
	JSObject *tooltipObj = nullptr;
	if( myObj->CanBeObjType( OT_CHAR ))
	{
		tooltipObj = JSEngine->AcquireObject( IUE_CHAR, myObj, runTime );
	}
	else if( myObj->CanBeObjType( OT_ITEM ))
	{
		tooltipObj = JSEngine->AcquireObject( IUE_ITEM, myObj, runTime );
	}
	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, pSocket, runTime );

	params[0] = OBJECT_TO_JSVAL( tooltipObj );
	params[1] = OBJECT_TO_JSVAL( sockObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTooltip", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnTooltip, false );
	}

	// If rval is negative, it's possible some other function/method called from within Ontooltip() encountered
	// an error. Abort attempt to turn it into a string - it might crash the server!
	if( rval < 0 )
	{
		Console.Error( "Handled exception in cScript.cpp OnTooltip() - invalid return value/error encountered!" );
		return "";
	}

	try
	{
		JSString *str = JS_ValueToString( targContext, rval );
		std::string returnString = JS_GetStringBytes( str );

		return returnString;
	}
	catch( ... )
	{
		Console.Error( "Handled exception in cScript.cpp OnTooltip()" );
		return "";
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnNameRequest()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for objects which client has requested the name for
//o------------------------------------------------------------------------------------------------o
std::string cScript::OnNameRequest( CBaseObject *myObj, CChar *nameRequester, UI08 requestSource )
{
	if( !ValidateObject( myObj ))
		return "";

	if( !ExistAndVerify( seOnNameRequest, "onNameRequest" ))
		return "";

	// Prevent infinite loop
	if( myObj->NameRequestActive() )
		return "";

	// Mark object as having an active name lookup via onNameRequest
	myObj->NameRequestActive( true );

	try
	{
		jsval rval, params[3];

		// Create JS object reference for myObj, based on whether it's an item or character
		JSObject *nameRequestObj = nullptr;
		if( myObj->CanBeObjType( OT_CHAR ))
		{
			nameRequestObj = JSEngine->AcquireObject( IUE_CHAR, myObj, runTime );
		}
		else if( myObj->CanBeObjType( OT_ITEM ))
		{
			nameRequestObj = JSEngine->AcquireObject( IUE_ITEM, myObj, runTime );
		}

		// Create JS object reference for the name requester (which might be nullptr!)
		JSObject *nameRequesterObj = nullptr;
		if( nameRequester != nullptr )
		{
			nameRequesterObj = JSEngine->AcquireObject( IUE_CHAR, nameRequester, runTime );
		}

		params[0] = OBJECT_TO_JSVAL( nameRequestObj );
		params[1] = OBJECT_TO_JSVAL( nameRequesterObj );
		params[2] = INT_TO_JSVAL( requestSource );
		JSBool retVal = JS_CallFunctionName( targContext, targObject, "onNameRequest", 3, params, &rval );
		if( retVal == JS_FALSE )
		{
			SetEventExists( seOnNameRequest, false );
		}

		// If rval is negative, it's possible some other function/method called from within onNameRequest() encountered
		// an error. Abort attempt to turn it into a string - it might crash the server!
		if( rval < 0 )
		{
			Console.Error( "Handled exception in cScript.cpp OnNameRequest() - invalid return value/error encountered!" );
			return "";
		}

		JSString *str = JS_ValueToString( targContext, rval );
		std::string returnString = JS_GetStringBytes( str );

		// If no string was returned from the event, make sure we return an empty string instead of "undefined", "false" or "true"
		if( returnString == "undefined" || returnString == "false" || returnString == "true" )
		{
			returnString = "";
		}

		// Clear flag that marks object as having an active name lookup via onNameRequest
		myObj->NameRequestActive( false );

		return returnString;
	}
	catch(...)
	{
		Console.Error( "Handled exception in cScript.cpp OnNameRequest()" );

		// Clear flag that marks object as having an active name lookup via onNameRequest
		myObj->NameRequestActive( false );
	}

	return "";
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnAttack()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when attacking someone
//|					Will also trigger the onDefense event for the character being attacked
//o------------------------------------------------------------------------------------------------o
bool cScript::OnAttack( CChar *attacker, CChar *defender )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return false;

	if( !ExistAndVerify( seOnAttack, "onAttack" ))
		return false;

	jsval rval, params[2];
	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAttack", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnAttack, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDefense()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being attacked
//o------------------------------------------------------------------------------------------------o
bool cScript::OnDefense( CChar *attacker, CChar *defender )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return false;

	if( !ExistAndVerify( seOnDefense, "onDefense" ))
		return false;

	jsval rval, params[2];
	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDefense", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDefense, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillGain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when gaining skillpoints
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillGain( CChar *player, SI08 skill, UI32 skillGainAmount )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillGain, "onSkillGain" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( skillGainAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillGain", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillGain, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStatGained()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when gaining stats
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnStatGained( CChar *player, UI32 stat, SI08 skill, UI32 statGainedAmount )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnStatGained, "onStatGained" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( skill );
	params[3] = INT_TO_JSVAL( statGainedAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatGained", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnStatGained, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStatGain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
bool cScript::OnStatGain( CChar *player, UI32 stat, SI08 skill, UI32 statGainAmount )
{
	if( !ValidateObject( player ))
		return false;

	if( !ExistAndVerify( seOnStatGain, "onStatGain" ))
		return false;

	jsval rval, params[4];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( skill );
	params[3] = INT_TO_JSVAL( statGainAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatGain", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnStatGain, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnVirtueGumpPress()
//|	Date		-	19/01/2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when activating Virtue Gump icon
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonId )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ) || !ValidateObject( tChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnVirtueGumpPress, "onVirtueGumpPress" ))
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );
	JSObject *targObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( targObj );
	params[2] = INT_TO_JSVAL( buttonId );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onVirtueGumpPress", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnVirtueGumpPress, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnQuestGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who activate Quest button in paperdoll
//|					Return true to prevent additional onQuestGump events from triggering
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnQuestGump( CChar *mChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnQuestGump, "onQuestGump" ))
		return RV_NOFUNC;

	jsval rval, params[1];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onQuestGump", 1, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnQuestGump, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnHelpButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who activate Help button in paperdoll
//|					Return false to prevent additional onHelpButton events from triggering
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnHelpButton( CChar *mChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnHelpButton, "onHelpButton" ))
		return RV_NOFUNC;

	jsval rval, params[1];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHelpButton", 1, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnHelpButton, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	cScript::OnContextMenuRequest()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Triggers when player activates context menu on another character
//|					Return false to prevent additional onContextMenu events from triggering
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnContextMenuRequest( CSocket *tSock, CBaseObject *baseObj )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( baseObj ) || tSock == nullptr )
		return RV_NOFUNC;
 
	if( !ExistAndVerify( seOnContextMenuRequest, "onContextMenuRequest" ))
		return RV_NOFUNC;
 
	jsval rval, params[2];
	JSObject *mySockObj = JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *myObj = nullptr;
	if( baseObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, baseObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, baseObj, runTime );
	}
 
	params[0] = OBJECT_TO_JSVAL( mySockObj );
	params[1] = OBJECT_TO_JSVAL( myObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onContextMenuRequest", 2, params, &rval );
 
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnContextMenuRequest, false );
		return RV_NOFUNC;
    }
 
	return TryParseJSVal( rval );
}
 
//o------------------------------------------------------------------------------------------------o
//| Function	-	cScript::OnContextMenuSelect()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Triggers when player selects an entry from a popup context menu
//|					Return false to prevent additional onContextMenuSelect events from triggering
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnContextMenuSelect( CSocket *tSock, CBaseObject *baseObj, UI16 popupEntry )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( baseObj ) || tSock == nullptr )
		return RV_NOFUNC;
 
	if( !ExistAndVerify( seOnContextMenuSelect, "onContextMenuSelect" ))
		return RV_NOFUNC;
 
	jsval rval, params[3];
	JSObject *mySockObj = JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *myObj = nullptr;
	if( baseObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, baseObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, baseObj, runTime );
	}
 
	params[0] = OBJECT_TO_JSVAL( mySockObj );
	params[1] = OBJECT_TO_JSVAL( myObj );
	params[2] = INT_TO_JSVAL( popupEntry );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onContextMenuSelect", 3, params, &rval );
 
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnContextMenuSelect, false );
		return RV_NOFUNC;
	}
 
	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnWarModeToggle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who toggle War Mode
//|					Return false to prevent character from entering War Mode
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnWarModeToggle( CChar *mChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnWarModeToggle, "onWarModeToggle" ))
		return RV_NOFUNC;

	jsval rval, params[1];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onWarModeToggle", 1, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnWarModeToggle, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::SI08()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who activate special abilities in combat books etc
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpecialMove( CChar *mChar, UI08 abilityId )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpecialMove, "onSpecialMove" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( abilityId );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpecialMove", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpecialMove, false );
		return RV_NOFUNC;
	}
	
	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDrop()
//|	Date		-	02/07/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropped by character
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDrop( CItem *item, CChar *dropper )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( item ) || !ValidateObject( dropper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDrop, "onDrop" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, dropper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDrop", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDrop, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDropItemOnItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropping it on another item, or when
//|					another item is dropped on said item
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( item ) || !ValidateObject( dropper ) || !ValidateObject( dest ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDropItemOnItem, "onDropItemOnItem" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, dropper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );
	JSObject *destObj = JSEngine->AcquireObject( IUE_ITEM, dest, runTime );

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( destObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDropItemOnItem", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDropItemOnItem, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnPickup()
//|	Date		-	25/01/2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when picked up by character
//o------------------------------------------------------------------------------------------------o
//|	Changes		-	16/07/2008
//|						Adjustments made to fix event, which didn't trigger
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnPickup( CItem *item, CChar *pickerUpper, CBaseObject *objCont )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( item ) || !ValidateObject( pickerUpper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnPickup, "onPickup" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pickerUpper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );
	JSObject *objContObj = nullptr;
	if( objCont != nullptr )
	{
		if( objCont->GetObjType() == OT_CHAR )
		{
			objContObj = JSEngine->AcquireObject( IUE_CHAR, objCont, runTime );
		}
		else
		{
			objContObj = JSEngine->AcquireObject( IUE_ITEM, objCont, runTime );
		}
	}

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( objContObj );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onPickup", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnPickup, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnContRemoveItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers on containers after an item has been removed from it
//o------------------------------------------------------------------------------------------------o
bool cScript::OnContRemoveItem( CItem *contItem, CItem *item, CChar *itemRemover )
{
	if( !ValidateObject( contItem ) || !ValidateObject( item ))
		return false;

	if( !ExistAndVerify( seOnContRemoveItem, "onContRemoveItem" ))
		return false;

	jsval params[3], rval;

	JSObject *contObj = JSEngine->AcquireObject( IUE_ITEM, contItem, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, itemRemover, runTime );

	params[0] = OBJECT_TO_JSVAL( contObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	params[2] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onContRemoveItem", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnContRemoveItem, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSwing()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when swung in combat
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( swinger ) || !ValidateObject( swingTarg ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSwing, "onSwing" ))
		return RV_NOFUNC;

	JSObject *itemObj	= ( ValidateObject( swinging ) ? JSEngine->AcquireObject( IUE_ITEM, swinging, runTime ) : nullptr );
	JSObject *attObj	= JSEngine->AcquireObject( IUE_CHAR, swinger, runTime );
	JSObject *defObj	= JSEngine->AcquireObject( IUE_CHAR, swingTarg, runTime );

	jsval params[3], rval;
	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( attObj );
	params[2] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSwing", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSwing, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDecay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when about to decay
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDecay( CItem *decaying )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( decaying ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDecay, "onDecay" ))
		return RV_NOFUNC;

	jsval params[1], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_ITEM, decaying, runTime );
	params[0] = OBJECT_TO_JSVAL( myObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDecay", 1, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDecay, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLeaving()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when leaving a multi
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnLeaving( CMultiObj *left, CBaseObject *leaving )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( left ) || !ValidateObject( leaving ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnLeaving, "onLeaving" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *myObj;
	JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, left, runTime );
	if( leaving->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, leaving, runTime );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, leaving, runTime );
		params[2] = INT_TO_JSVAL( 1 );
	}
	params[0] = OBJECT_TO_JSVAL( myItem );
	params[1] = OBJECT_TO_JSVAL( myObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLeaving", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnLeaving, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnMultiLogout()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for multi when a player logs out inside the multi
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnMultiLogout( CMultiObj *iMulti, CChar *cPlayer )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( iMulti ) || !ValidateObject( cPlayer ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnMultiLogout, "onMultiLogout" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *myMulti = JSEngine->AcquireObject( IUE_ITEM, iMulti, runTime );
	JSObject *myPlayer = JSEngine->AcquireObject( IUE_CHAR, cPlayer, runTime );

	params[0] = OBJECT_TO_JSVAL( myMulti );
	params[1] = OBJECT_TO_JSVAL( myPlayer );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onMultiLogout", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnMultiLogout, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnEquipAttempt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when character tries to equip it
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnEquipAttempt( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEquipAttempt, "onEquipAttempt" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, equipper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, equipping, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEquipAttempt", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnEquipAttempt, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnEquip()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when equipped by a character
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnEquip( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEquip, "onEquip" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, equipper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, equipping, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEquip", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnEquip, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnUnequipAttempt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when unequipped by a character
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnUnequipAttempt( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUnequipAttempt, "onUnequipAttempt" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, equipper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, equipping, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUnequipAttempt", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnUnequipAttempt, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnUnequip()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when unequipped by a character
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnUnequip( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUnequip, "onUnequip" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, equipper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, equipping, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUnequip", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnUnequip, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnUseChecked()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers (after hardcoded checks) for item with event attached, when used
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 3 possible values
//|					-1	=> No such function or bad call
//|					0	=> Don't execute hard coded implementation
//|					1	=> Execute hard coded implementations as well
//o------------------------------------------------------------------------------------------------o
//| Changes		-	31 July, 2003 15:39 ( making it version 3)
//|						Changed return values from bool to SI08
//|					27 October, 2007
//|						Split onUse into onUseChecked and onUseUnChecked
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnUseChecked( CChar *user, CItem *iUsing )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( user ) || !ValidateObject( iUsing ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUseChecked, "onUseChecked" ))
		return RV_NOFUNC;

	jsval rval, params[2];

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, user, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUseChecked", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnUseChecked, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnUseUnChecked()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers (before hardcoded checks) for item with event attached, when used
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 3 possible values
//|					-1	=> No such function or bad call
//|					0	=> Don't execute hard coded implementation
//|					1	=> Execute hard coded implementations as well
//o------------------------------------------------------------------------------------------------o
//| Changes		-	31 July, 2003 15:39 ( making it version 3)
//|						Changed return values from bool to SI08
//|					27 October, 2007
//|						Split onUse into onUseChecked and onUseUnChecked
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnUseUnChecked( CChar *user, CItem *iUsing )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( user ) || !ValidateObject( iUsing ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUseUnChecked, "onUseUnChecked" ))
		return RV_NOFUNC;

	jsval rval, params[2];

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, user, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUseUnChecked", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnUseUnChecked, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDropItemOnNpc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropped on an NPC, and for NPC with
//|					event attached if item is dropped on it and doesn't bounce in item's script
//o------------------------------------------------------------------------------------------------o
//|	Changes		-	V2 -
//|						Returns
//|							-1 if no function exists
//|							0 if should bounce
//|							1 if should not bounce and use code
//|							2 if should not bounce and not use code
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( srcChar ) || !ValidateObject( dstChar ) || !ValidateObject( item ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDropItemOnNpc, "onDropItemOnNpc" ))
		return RV_NOFUNC;

	jsval rval, params[3];

	JSObject *srcObj	= JSEngine->AcquireObject( IUE_CHAR, srcChar, runTime );
	JSObject *dstObj	= JSEngine->AcquireObject( IUE_CHAR, dstChar, runTime );
	JSObject *itemObj	= JSEngine->AcquireObject( IUE_ITEM, item, runTime );
	params[0] = OBJECT_TO_JSVAL( srcObj );
	params[1] = OBJECT_TO_JSVAL( dstObj );
	params[2] = OBJECT_TO_JSVAL( itemObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDropItemOnNpc", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDropItemOnNpc, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnEntrance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when entering a multi
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnEntrance( CMultiObj *left, CBaseObject *leaving )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( left ) || !ValidateObject( leaving ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEntrance, "onEntrance" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *myObj;
	JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, left, runTime );
	if( leaving->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, leaving, runTime );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, leaving, runTime );
		params[2] = INT_TO_JSVAL( 1 );
	}
	params[0] = OBJECT_TO_JSVAL( myItem );
	params[1] = OBJECT_TO_JSVAL( myObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEntrance", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnEntrance, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OutOfRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when objects go out of range
//|	Notes		-	A third parameter in the event can be used to determine whether the object
//|					going out of range is an ITEM or a CHARACTER
//o------------------------------------------------------------------------------------------------o
bool cScript::OutOfRange( CBaseObject *srcObj, CBaseObject *objVanish )
{
	if( !ValidateObject( srcObj ) || !ValidateObject( objVanish ))
		return false;

	if( !ExistAndVerify( seOutOfRange, "outOfRange" ))
		return false;

	jsval params[2], rval;

	JSObject *myObj;
	if( srcObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, srcObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, srcObj, runTime );
	}

	JSObject *myObj2;
	if( objVanish->GetObjType() == OT_CHAR )
	{
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objVanish, runTime );
	}
	else
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objVanish, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( myObj2 );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "outOfRange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOutOfRange, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLogin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when logging on to server
//o------------------------------------------------------------------------------------------------o
bool cScript::OnLogin( CSocket *sockPlayer, CChar *pPlayer )
{
	if( !ValidateObject( pPlayer ))
		return false;

	if( !ExistAndVerify( seOnLogin, "onLogin" ))
		return false;

	jsval params[2], rval;
	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime );
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pPlayer, runTime );

	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLogin", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnLogin, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLogout()
//|	Date		-	10/06/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when logging out of server
//o------------------------------------------------------------------------------------------------o
bool cScript::OnLogout( CSocket *sockPlayer, CChar *pPlayer )
{
	if( !ValidateObject( pPlayer ))
		return false;

	if( !ExistAndVerify( seOnLogout, "onLogout" ))
		return false;

	jsval params[2], rval;

	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime );
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pPlayer, runTime );

	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLogout", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnLogout, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnClick()
//|	Date		-	10/06/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when a player single-clicks on it
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnClick( CSocket *sockPlayer, CBaseObject *objClicked )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objClicked ))
		return false;

	if( !ExistAndVerify( seOnClick, "onClick" ))
		return RV_NOFUNC;

	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime );
	JSObject *myObj;
	if( objClicked->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, objClicked, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, objClicked, runTime );
	}

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( myObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onClick", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnClick, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnFall()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when fallDistance is over 20
//o------------------------------------------------------------------------------------------------o
bool cScript::OnFall( CChar *pFall, SI08 fallDistance )
{
	if( !ValidateObject( pFall ))
		return false;

	if( !ExistAndVerify( seOnFall, "onFall" ))
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pFall, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( fallDistance );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFall", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnFall, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnAISliver()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers on every AI loop for character with event attached
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnAISliver( CChar *pSliver )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pSliver ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnAISliver, "onAISliver" ))
		return RV_NOFUNC;

	jsval params[1], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pSliver, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAISliver", 1, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnAISliver, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSystemSlice()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
bool cScript::OnSystemSlice( void )
{
	if( !ExistAndVerify( seOnSystemSlice, "onSystemSlice" ))
		return false;

	jsval rval;
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSystemSlice", 0, nullptr, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSystemSlice, false );
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLightChange()
//|	Date		-	17/02/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when lightlevel changes
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnLightChange( CBaseObject *tObject, UI08 lightLevel )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( tObject ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnLightChange, "onLightChange" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( lightLevel );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLightChange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnLightChange, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnWeatherChange()
//|	Date		-	17/02/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when weather changes
//o------------------------------------------------------------------------------------------------o
bool cScript::OnWeatherChange( CBaseObject *tObject, WeatherType element )
{
	if( !ValidateObject( tObject ))
		return false;

	if( !ExistAndVerify( seOnWeatherChange, "onWeatherChange" ))
		return false;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( element );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onWeatherChange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnWeatherChange, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::cScript::OnTempChange()
//|	Date		-	17/02/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when temperature changes
//o------------------------------------------------------------------------------------------------o
bool cScript::OnTempChange( CBaseObject *tObject, SI08 temp )
{
	if( !ValidateObject( tObject ))
		return false;

	if( !ExistAndVerify( seOnTempChange, "onTempChange" ))
		return false;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( temp );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTempChange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnTempChange, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached, when custom timers started on said
//|					object using StartTimer function update
//o------------------------------------------------------------------------------------------------o
bool cScript::OnTimer( CBaseObject *tObject, UI16 timerId )
{
	if( !ValidateObject( tObject ))
		return false;

	if( !ExistAndVerify( seOnTimer, "onTimer" ))
		return false;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( timerId );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTimer", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnTimer, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStatLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when losing stats
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnStatLoss( CChar *player, UI32 stat, UI32 statLossAmount )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnStatLoss, "onStatLoss" ))
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( statLossAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatLoss", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnStatLoss, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStatChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when stats change
//o------------------------------------------------------------------------------------------------o
bool cScript::OnStatChange( CChar *player, UI32 stat, SI32 statChangeAmount )
{
	if( !ValidateObject( player ))
		return false;

	if( !ExistAndVerify( seOnStatChange, "onStatChange" ))
		return false;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( statChangeAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatChange", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnStatChange, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when losing skillpoints
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillLoss( CChar *player, SI08 skill, UI32 skillLossAmount )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillLoss, "onSkillLoss" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( skillLossAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillLoss", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillLoss, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when skillpoints change
//o------------------------------------------------------------------------------------------------o
bool cScript::OnSkillChange( CChar *player, SI08 skill, SI32 skillChangeAmount )
{
	if( !ValidateObject( player ))
		return false;

	if( !ExistAndVerify( seOnSkillChange, "onSkillChange" ))
		return false;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( skillChangeAmount );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillChange", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillChange, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDeath()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached after dying
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDeath( CChar *pDead, CItem *iCorpse )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pDead ) || !ValidateObject( iCorpse ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDeath, "onDeath" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pDead, runTime );
	JSObject *corpseObj = JSEngine->AcquireObject( IUE_ITEM, iCorpse, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( corpseObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeath", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDeath, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnResurrect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being resurrected
//|	Notes		-	If script returns false when event triggers, resurrection is blocked
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnResurrect( CChar *pAlive )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pAlive ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnResurrect, "onResurrect" ))
		return RV_NOFUNC;

	jsval params[1], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pAlive, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onResurrect", 1, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnResurrect, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnFlagChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when the flag status changes
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnFlagChange, "onFlagChange" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( newStatus );
	params[2] = INT_TO_JSVAL( oldStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFlagChange", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnFlagChange, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::DoCallback()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles callbacks for custom target cursors triggered from scripts
//o------------------------------------------------------------------------------------------------o
bool cScript::DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
{
	if( tSock == nullptr )
		return false;

	jsval params[2], rval;
	SI32 objType		= 2;	// 2 == null, 1 == char, 0 == item
	CBaseObject *mObj	= nullptr;
	JSObject *myObj2	= nullptr;
	try
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
		if( myObj == nullptr )
			return false;

		params[0] = OBJECT_TO_JSVAL( myObj );
		if( targeted >= BASEITEMSERIAL )
		{
			mObj	= CalcItemObjFromSer( targeted );
			objType = 0;
		}
		else
		{
			mObj	= CalcCharObjFromSer( targeted );
			objType	= 1;
		}
		if( !ValidateObject( mObj ))
		{
			objType		= 2;
			params[1]	= JSVAL_NULL;
		}
		else
		{
			if( objType == 0 )
			{
				myObj2 = JSEngine->AcquireObject( IUE_ITEM, mObj, runTime );
			}
			else
			{
				myObj2 = JSEngine->AcquireObject( IUE_CHAR, mObj, runTime );
			}
			params[1] = OBJECT_TO_JSVAL( myObj2 );
		}
		// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
		JS_SetGlobalObject( targContext, targObject );


		JSBool retVal = JS_CallFunctionName( targContext, targObject, oldstrutil::format( "onCallback%i", callNum ).c_str(), 2, params, &rval );
		return ( retVal == JS_TRUE );
	}
	catch( ... )
	{
		Console.Error( "Handled exception in cScript.cpp DoCallback()" );
	}
	return false;
}

JSObject *cScript::Object( void ) const
{
	return targObject;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLoyaltyChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPC character with event attached when loyalty level changes
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnLoyaltyChange( CChar *pChanging, SI08 newStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnLoyaltyChange, "onLoyaltyChange" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( newStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLoyaltyChange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnLoyaltyChange, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnHungerChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when hunger level changes
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnHungerChange( CChar *pChanging, SI08 newStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnHungerChange, "onHungerChange" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( newStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHungerChange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnHungerChange, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   cScript::OnThirstChange()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Triggers for character with event attached when thirst level changes
//o------------------------------------------------------------------------------------------------o
bool cScript::OnThirstChange( CChar* pChanging, SI08 newStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnThirstChange, "onThirstChange" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject* charObj = JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( newStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onThirstChange", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnThirstChange, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStolenFrom()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being stolen from
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( stealing ) || !ValidateObject( stolenFrom ) || !ValidateObject( stolen ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnStolenFrom, "onStolenFrom" ))
		return RV_NOFUNC;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, stealing, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, stolenFrom, runTime );
	JSObject *itemObj	= JSEngine->AcquireObject( IUE_ITEM, stolen, runTime );

	params[0] = OBJECT_TO_JSVAL( thiefObj );
	params[1] = OBJECT_TO_JSVAL( victimObj );
	params[2] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStolenFrom", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnStolenFrom, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSnooped()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when someone snoops their backpack
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSnooped( CChar *snooped, CChar *snooper, bool success )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSnooped, "onSnooped" ))
		return RV_NOFUNC;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, snooped, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, snooper, runTime );

	params[0] = OBJECT_TO_JSVAL( thiefObj );
	params[1] = OBJECT_TO_JSVAL( victimObj );
	params[2] = BOOLEAN_TO_JSVAL( success );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSnooped", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSnooped, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSnoopAttempt()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached if they attempt to snoop someone's backpack
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSnoopAttempt( CChar *snooped, CItem *pack, CChar *snooper )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( snooped ) || !ValidateObject( pack ) || !ValidateObject( snooper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSnoopAttempt, "onSnoopAttempt" ))
		return RV_NOFUNC;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, snooped, runTime );
	JSObject *packObj	= JSEngine->AcquireObject( IUE_ITEM, pack, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, snooper, runTime );

	params[0] = OBJECT_TO_JSVAL( thiefObj );
	params[1] = OBJECT_TO_JSVAL( packObj );
	params[2] = OBJECT_TO_JSVAL( victimObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSnoopAttempt", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSnoopAttempt, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::NewGumpList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
size_t cScript::NewGumpList( void )
{
	size_t retVal	= gumpDisplays.size();
	SEGump_st *toAdd	= new SEGump_st;
	toAdd->one		= new std::vector<std::string>();
	toAdd->two		= new std::vector<std::string>();

	gumpDisplays.push_back( toAdd );
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::GetGumpList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
SEGump_st * cScript::GetGumpList( SI32 index )
{
	if( index < 0 || static_cast<size_t>( index ) >= gumpDisplays.size() )
		return nullptr;

	return gumpDisplays[index];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::RemoveGumpList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
void cScript::RemoveGumpList( SI32 index )
{
	if( index < 0 || static_cast<size_t>( index ) >= gumpDisplays.size() )
		return;

	delete gumpDisplays[index]->one;
	delete gumpDisplays[index]->two;
	delete gumpDisplays[index];

	gumpDisplays.erase( gumpDisplays.begin() + index );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::SendGumpList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o------------------------------------------------------------------------------------------------o
void cScript::SendGumpList( SI32 index, CSocket *toSendTo )
{
	if( index < 0 || static_cast<size_t>( index ) >= gumpDisplays.size() )
		return;

	UI32 gumpId = (0xFFFF + JSMapping->GetScriptId( targObject ));
	SendVecsAsGump( toSendTo, *( gumpDisplays[index]->one ), *( gumpDisplays[index]->two ), gumpId, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::HandleGumpPress()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Process a gump button press
//o------------------------------------------------------------------------------------------------o
//|	Changes		-	1/21/2003 - Implemented the code needed to support enhanced
//|									gump response processing
//o------------------------------------------------------------------------------------------------o
void cScript::HandleGumpPress( CPIGumpMenuSelect *packet )
{
	if( !ExistAndVerify( seOnGumpPress, "onGumpPress" ))
		return;

	if( packet == nullptr )
		return;

	CSocket *pressing = packet->GetSocket();
	if( pressing == nullptr )
		return;

	UI32 button		= packet->ButtonId();
	UI16 nButtons	= static_cast<UI16>( packet->SwitchCount() );
	UI16 nText		= static_cast<UI16>( packet->TextCount() );

	SEGumpData_st *segdGumpData	= new SEGumpData_st;
	JSObject *jsoObject			= JS_NewObject( targContext, &UOXGumpData_class, nullptr, nullptr );
	JS_DefineFunctions( targContext, jsoObject, CGumpData_Methods );
	JS_DefineProperties( targContext, jsoObject, CGumpDataProperties );
	JS_SetPrivate( targContext, jsoObject, segdGumpData );
	JS_LockGCThing( targContext, jsoObject );
	//JS_AddRoot( targContext, &jsoObject );

	UI16 i;
	// Loop through Buttons
	for( i = 0; i < nButtons; ++i )
	{
		segdGumpData->nButtons.push_back( packet->SwitchValue( i ));
	}
	// Process text for the buttons?
	// Loop grabbing text
	for( i = 0; i < nText; ++i )
	{
		segdGumpData->nIDs.push_back( packet->GetTextId( i ));
		segdGumpData->sEdits.push_back( packet->GetTextString( i ));
	}
	jsval jsvParams[3], jsvRVal;

	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, pressing, runTime );
	jsvParams[0] = OBJECT_TO_JSVAL( myObj );
	jsvParams[1] = INT_TO_JSVAL( button );
	jsvParams[2] = OBJECT_TO_JSVAL( jsoObject );
	[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, "onGumpPress", 3, jsvParams, &jsvRVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::HandleGumpInput()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle gump inputs?
//o------------------------------------------------------------------------------------------------o
void cScript::HandleGumpInput( CPIGumpInput *pressing )
{
	if( pressing == nullptr )
		return;

	if( !ExistAndVerify( seOnGumpInput, "onGumpInput" ))
		return;

	jsval params[3], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, pressing->GetSocket(), runTime );
	JSString *gumpReply = nullptr;
	gumpReply = JS_NewStringCopyZ( targContext, pressing->Reply().c_str() );
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( pressing->Index() );
	params[2] = STRING_TO_JSVAL( gumpReply );
	[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, "onGumpInput", 3, params, &rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnScrollingGumpPress()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when clicking the old school horizontally scrolling gump
//|					if ID of this gump is 0
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnScrollingGumpPress( CSocket *tSock, UI16 gumpId, UI16 buttonId )
{
	const SI08 RV_NOFUNC = -1;
	if( tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnScrollingGumpPress, "onScrollingGumpPress" ))
		return RV_NOFUNC;

	jsval params[3], rval;
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( gumpId );
	params[2] = INT_TO_JSVAL( buttonId );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onScrollingGumpPress", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnScrollingGumpPress, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnEnterRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when entering a region
//o------------------------------------------------------------------------------------------------o
bool cScript::OnEnterRegion( CChar *entering, UI16 region )
{
	if( !ValidateObject( entering ))
		return false;

	if( !ExistAndVerify( seOnEnterRegion, "onEnterRegion" ))
		return false;

	jsval params[2], rval;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, entering, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( region );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEnterRegion", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnEnterRegion, false );
	}
	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLeaveRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when leaving a region
//o------------------------------------------------------------------------------------------------o
bool cScript::OnLeaveRegion( CChar *leaving, UI16 region )
{
	if( !ValidateObject( leaving ))
		return false;

	if( !ExistAndVerify( seOnLeaveRegion, "onLeaveRegion" ))
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, leaving, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( region );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLeaveRegion", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnLeaveRegion, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnFacetChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when switching to a different facet
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnFacetChange( CChar *mChar, const UI08 oldFacet, const UI08 newFacet )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnFacetChange, "onFacetChange" ))
		return RV_NOFUNC;

	jsval params[3], rval;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( oldFacet );
	params[2] = INT_TO_JSVAL( newFacet );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFacetChange", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnFacetChange, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellTargetSelect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached who targets someone with a spell
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellTargetSelect(  CChar *caster, CBaseObject *target, UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( target ) || !ValidateObject( caster ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellTargetSelect, "onSpellTargetSelect" ))
		return RV_NOFUNC;

	jsval params[4], rval;
	JSObject *castObj = JSEngine->AcquireObject( IUE_CHAR, caster, runTime );
	JSObject *targObj;
	if( target->CanBeObjType( OT_CHAR ))
	{
		targObj = JSEngine->AcquireObject( IUE_CHAR, target, runTime );
	}
	else
	{
		targObj = JSEngine->AcquireObject( IUE_ITEM, target, runTime );
	}
	params[0] = OBJECT_TO_JSVAL( targObj );
	params[1] = OBJECT_TO_JSVAL( castObj );
	params[2] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellTargetSelect", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellTargetSelect, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached who is the target of a spell
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( target ) || !ValidateObject( caster ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellTarget, "onSpellTarget" ))
		return RV_NOFUNC;

	jsval params[4], rval;
	JSObject *castObj = JSEngine->AcquireObject( IUE_CHAR, caster, runTime );
	JSObject *targObj;
	if( target->CanBeObjType( OT_CHAR ))
	{
		targObj = JSEngine->AcquireObject( IUE_CHAR, target, runTime );
	}
	else
	{
		targObj = JSEngine->AcquireObject( IUE_ITEM, target, runTime );
	}
	params[0] = OBJECT_TO_JSVAL( targObj );
	params[1] = OBJECT_TO_JSVAL( castObj );
	params[2] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellTarget", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellTarget, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::CallParticularEvent()
//|	Date		-	20th December, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o------------------------------------------------------------------------------------------------o
bool cScript::CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams, jsval *eventRetVal )
{
	if( eventToCall == nullptr )
		return false;

	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, eventRetVal );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellSuccess, false );
		return false;
	}
	
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellCast()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached casts a spell
//|	Notes		-	Return value table
//|						-2: use NORMAL non-JS casting
//|						-1: CANCEL spellcasting
//|						0->inf: Spell delay in ms
//o------------------------------------------------------------------------------------------------o
SI16 cScript::OnSpellCast( CChar *tChar, UI08 SpellId )
{
	if( !ValidateObject( tChar ))
		return -2;

	if( !ExistAndVerify( seOnSpellCast, "onSpellCast" ))
		return -2;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellId );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellCast", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellCast, false );
		return -2;
	}

	return static_cast<SI16>( JSVAL_TO_INT( rval ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnScrollCast()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached casts a spell through a scroll
//|	Notes		-	Return value table
//|						-2: use NORMAL non-JS casting
//|						-1: CANCEL spellcasting
//|						0->inf: Spell delay in ms
//o------------------------------------------------------------------------------------------------o
SI16 cScript::OnScrollCast( CChar *tChar, UI08 SpellId )
{
	if( !ValidateObject( tChar ))
		return -2;

	if( !ExistAndVerify( seOnScrollCast, "onScrollCast" ))
		return -2;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellId );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onScrollCast", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnScrollCast, false );
		return -2;
	}

	return static_cast<SI16>( JSVAL_TO_INT( rval ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellSuccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers after character with event attached successfully casts a spell
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellSuccess( CChar *tChar, UI08 SpellId )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( tChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellSuccess, "onSpellSuccess" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellId );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellSuccess", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellSuccess, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnTalk()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached says something
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnTalk( CChar *myChar, const char *mySpeech )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( myChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnTalk, "onTalk" ))
		return RV_NOFUNC;

	jsval params[2], rval;

	JSString *strSpeech		= nullptr;
	std::string lwrSpeech	= mySpeech;

	strSpeech = JS_NewStringCopyZ( targContext, oldstrutil::lower( lwrSpeech ).c_str() );

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = STRING_TO_JSVAL( strSpeech );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTalk", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnTalk, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpeechInput()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers after player with event attached inputs speech after a request for
//|					speech input has been made.
//|	Notes		-	This function is called out of network.cpp if a speechmode(9) was previously set
//o------------------------------------------------------------------------------------------------o
bool cScript::OnSpeechInput( CChar *myChar, CItem *myItem, const char *mySpeech )
{
	if( !ValidateObject( myChar ))
		return true;

	if( !ExistAndVerify( seOnSpeechInput, "onSpeechInput" ))
		return true;

	jsval params[4], rval;
	JSString *strSpeech = nullptr;

	char *lwrSpeech = new char[strlen( mySpeech ) + 1];
	strcopy( lwrSpeech, strlen( mySpeech ) + 1, mySpeech );
	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );
	delete[] lwrSpeech;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );

	if( !ValidateObject( myItem ))
	{
		params[1] = JSVAL_NULL;
	}
	else
	{
		JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, myItem, runTime );
		params[1] = OBJECT_TO_JSVAL( itemObj );
	}

	params[2] = STRING_TO_JSVAL( strSpeech );
	params[3] = INT_TO_JSVAL( myChar->GetSpeechId() );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpeechInput", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpeechInput, false );
		return true;
	}

	return ( rval == JSVAL_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellGain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are added to them
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellGain( CItem *book, const UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( book ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellGain, "onSpellGain" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, book, runTime );
	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellGain", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellGain, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are removed from them
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellLoss( CItem *book, const UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( book ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellLoss, "onSpellLoss" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, book, runTime );
	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellLoss, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when a skillcheck is performed
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill, bool isCraftSkill )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( myChar ) || skill > ALLSKILLS )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillCheck, "onSkillCheck" ))
		return RV_NOFUNC;

	jsval params[5], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( lowSkill );
	params[3] = INT_TO_JSVAL( highSkill );
	params[4] = BOOLEAN_TO_JSVAL( isCraftSkill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillCheck", 5, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillCheck, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::AreaObjFunc()
//|	Date		-	January 27, 2003
//|	Changes		-	August 17 2005
//|						Renamed to AreaObjFunc from AreaCharFunc
//|						Added support for other object types
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls the function represented in funcName for the script
//|				-	passing in two character parameters
//o------------------------------------------------------------------------------------------------o
bool cScript::AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s )
{
	if( !ValidateObject( srcObject ) || !ValidateObject( tmpObject ) || funcName == nullptr )
		return false;

	jsval params[3], rval;

	JSObject *srcObj = nullptr;
	JSObject *tmpObj = nullptr;

	if( srcObject == nullptr || tmpObject == nullptr )
		return false;

	if( srcObject->CanBeObjType( OT_ITEM ))
	{
		srcObj = JSEngine->AcquireObject( IUE_ITEM, srcObject, runTime );
	}
	else if( srcObject->CanBeObjType( OT_CHAR ))
	{
		srcObj = JSEngine->AcquireObject( IUE_CHAR, srcObject, runTime );
	}

	if( tmpObject->CanBeObjType( OT_ITEM ))
	{
		tmpObj = JSEngine->AcquireObject( IUE_ITEM, tmpObject, runTime );
	}
	else if( tmpObject->CanBeObjType( OT_CHAR ))
	{
		tmpObj = JSEngine->AcquireObject( IUE_CHAR, tmpObject, runTime );
	}

	if( srcObj == nullptr || tmpObj == nullptr )
	{
		return false;
	}

	params[0]			= OBJECT_TO_JSVAL( srcObj );
	params[1]			= OBJECT_TO_JSVAL( tmpObj );

	if( s != nullptr )
	{
		JSObject *sockObj	= nullptr;
		sockObj		= JSEngine->AcquireObject( IUE_SOCK, s, runTime );
		params[2]	= OBJECT_TO_JSVAL( sockObj );
	}
	else
	{
		params[2]	= JSVAL_NULL;
	}
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );

	//FIXME === do we need this retvalue?
	//JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );
	try
	{
		[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );
	}
	catch( ... )
	{
		Console.Error( "Some error!" );
	}

	return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Quick and dirty way to setup custom commands
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCommand( CSocket *mSock, std::string command )
{
	const SI08 RV_NOFUNC = -1;
	if( mSock  == nullptr || command == "" )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCommand, "onCommand" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	JSString *strCmd = nullptr;
	strCmd = JS_NewStringCopyZ( targContext, oldstrutil::lower( command ).c_str() );
	params[0]	= OBJECT_TO_JSVAL( myObj );
	params[1]	= STRING_TO_JSVAL( strCmd );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onCommand", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnCommand, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::ExistAndVerify()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
bool cScript::ExistAndVerify( ScriptEvent eventNum, std::string functionName )
{
	if( !EventExists( eventNum ))
		return false;

	if( NeedsChecking( eventNum ))
	{
		SetNeedsChecking( eventNum, false );
		jsval Func = JSVAL_NULL;
		JS_GetProperty( targContext, targObject, functionName.c_str(), &Func );
		if( Func == JSVAL_VOID )
		{
			SetEventExists( eventNum, false );
			return false;
		}
	}

	JS_SetGlobalObject( targContext, targObject );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::ScriptRegistration()
//|	Date		-	20th December, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers a script with the JS engine
//|	Notes		-	Also requires a <scriptType>Registration() event with a Register<scriptType>()
//|					function, and an onSkill() event, both in the same script
//o------------------------------------------------------------------------------------------------o
bool cScript::ScriptRegistration( std::string scriptType )
{
	scriptType += "Registration";
	jsval params[1], rval;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, scriptType.c_str(), &Func );
	if( Func == JSVAL_VOID )
	{
		Console.Warning( oldstrutil::format( "Script Number (%u) does not have a %s function", JSMapping->GetScriptId( targObject ), scriptType.c_str() ));
		return false;
	}

	JSBool retVal = JS_CallFunctionName( targContext, targObject, scriptType.c_str(), 0, params, &rval );
	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::executeCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute a player-initiated JS-based command
//o------------------------------------------------------------------------------------------------o
bool cScript::executeCommand( CSocket *s, std::string funcName, std::string executedString )
{
	jsval params[2], rval;
	JSString *execString = JS_NewStringCopyZ( targContext, executedString.c_str() );
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, s, runTime );
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = STRING_TO_JSVAL( execString );
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName.c_str(), 2, params, &rval );

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::MagicSpellCast()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers before a spellcast attempt for characters with onSpellCast event attached
//o------------------------------------------------------------------------------------------------o
bool cScript::MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, SI32 spellNum )
{
	if( !ValidateObject( tChar ))
		return false;

	if( !ExistAndVerify( seOnSpellCast, "onSpellCast" ))
		return false;

	jsval params[4], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );
	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );

	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = BOOLEAN_TO_JSVAL( directCast );
	params[3] = INT_TO_JSVAL( spellNum );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellCast", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellCast, false );
		return false;
	}

	return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnIterate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called after IterateOver JS function is used, and iterates over all items or
//|					characters (as specified) in the game
//o------------------------------------------------------------------------------------------------o
bool cScript::OnIterate( CBaseObject *a, UI32 &b )
{
	if( !ValidateObject( a ))
		return true;

	if( !ExistAndVerify( seOnIterate, "onIterate" ))
		return false;

	jsval params[1], rval;

	JSObject *myObj = nullptr;
	if( a->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, a, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, a, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );

	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onIterate", 1, params, &rval );

	/*	if( ValidateObject( a ))
	{
		if( a->GetObjType() == OT_CHAR )
		{
			JSEngine->ReleaseObject( IUE_CHAR, a );
		}
		else
		{
			JSEngine->ReleaseObject( IUE_ITEM, a );
		}
	}
*/
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnIterate, false );
	}
	else if( JSVAL_TO_BOOLEAN( rval ))
	{
		++b;
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnIterateSpawnRegions()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called after IterateOverSpawnRegions JS function is used, and iterates over
//|					all spawn regions in game
//o------------------------------------------------------------------------------------------------o
bool cScript::OnIterateSpawnRegions( CSpawnRegion *a, UI32 &b )
{
	if( a == nullptr )
		return true;

	if( !ExistAndVerify( seOnIterateSpawnRegions, "onIterateSpawnRegions" ))
		return false;

	jsval params[1], rval;

	JSObject *myObj = nullptr;
	myObj = JSEngine->AcquireObject( IUE_SPAWNREGION, a, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onIterateSpawnRegions", 1, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnIterateSpawnRegions, false );
	}
	else if( JSVAL_TO_BOOLEAN( rval ))
	{
		++b;
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnPacketReceive()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	"Packet hook" event that allows interception of incoming network packets
//|	Notes		-	Requires OVERLOADPACKETS ini setting to be enabled, and incoming packet must
//|					be registered in JSE_FILEASSOCIATIONS.SCP under the [PACKET_SCRIPTS] section
//o------------------------------------------------------------------------------------------------o
bool cScript::OnPacketReceive( CSocket *mSock, UI16 packetNum )
{
	if( mSock == nullptr )
		return false;

	if( !ExistAndVerify( seOnPacketReceive, "onPacketReceive" ))
		return false;

	jsval rval, params[3];
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	params[0]		= OBJECT_TO_JSVAL( myObj );
	params[1]		= INT_TO_JSVAL( static_cast<UI08>( packetNum % 256 ));
	params[2]		= INT_TO_JSVAL( static_cast<UI08>( packetNum >> 8 ));
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onPacketReceive", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnPacketReceive, false );
	}

	return ( retVal == JS_TRUE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCharDoubleClick()
//|	Date		-	23rd January, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding events that happen when doubleclicking characters, such as
//|					open paperdoll, mounting horses, etc
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCharDoubleClick( CChar *currChar, CChar *targChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( currChar ) || !ValidateObject( targChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCharDoubleClick, "onCharDoubleClick" ))
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *srcObj = JSEngine->AcquireObject( IUE_CHAR, currChar, runTime );
	JSObject *trgObj = JSEngine->AcquireObject( IUE_CHAR, targChar, runTime );

	params[0] = OBJECT_TO_JSVAL( srcObj );
	params[1] = OBJECT_TO_JSVAL( trgObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCharDoubleClick", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnCharDoubleClick, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillGump()
//|	Date		-	23rd January, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding the client's request to open the default skillgump, and
//|					instead do something else (like opening a custom skillgump instead).
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillGump( CChar *currChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( currChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillGump, "onSkillGump" ))
		return RV_NOFUNC;

	jsval params[1], rval;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, currChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onSkillGump", 1, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillGump, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnUseBandageMacro()
//|	Date		-	12th May, 2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Expose bandage macro usage to JS engine so server admins can override the effects
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnUseBandageMacro( CSocket *mSock, CChar *targChar, CItem *bandageItem )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( targChar ) || mSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUseBandageMacro, "onUseBandageMacro" ))
		return RV_NOFUNC;

	jsval params[3], rval;

	JSObject *mSockObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	JSObject *targCharObj = JSEngine->AcquireObject( IUE_CHAR, targChar, runTime );
	JSObject *bandageItemObj = JSEngine->AcquireObject( IUE_ITEM, bandageItem, runTime );
	params[0] = OBJECT_TO_JSVAL( mSockObj );
	params[1] = OBJECT_TO_JSVAL( targCharObj );
	params[2] = OBJECT_TO_JSVAL( bandageItemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUseBandageMacro", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnUseBandageMacro, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnAICombatTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when selecting a target as part of
//|					default AI behaviour
//|	Notes		-	Returning FALSE will deem a target invalid, and it will be skipped
//|					Returning TRUE will deem a target valid, and it will be selected
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnAICombatTarget( CChar *attacker, CChar *target )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( attacker ) || !ValidateObject( target ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnAICombatTarget, "onAICombatTarget" ))
		return RV_NOFUNC;

	jsval params[2], rval;

	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *targObj = JSEngine->AcquireObject( IUE_CHAR, target, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( targObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAICombatTarget", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnAICombatTarget, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCombatStart()
//|	Date		-	23rd January, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when initiating combat
//|	Notes		-	Returning FALSE will also run hard code for this scenario
//|					Returning TRUE will override code's default handling of this scenario
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCombatStart( CChar *attacker, CChar *defender )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCombatStart, "onCombatStart" ))
		return RV_NOFUNC;

	jsval params[2], rval;

	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCombatStart", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnCombatStart, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCombatEnd()
//|	Date		-	23rd January, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when combat ends
//|	Notes		-	Returning FALSE will also run hard code for this scenario
//|					Returning TRUE will override code's default handling of this scenario
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCombatEnd( CChar *currChar, CChar *targChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( currChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCombatEnd, "onCombatEnd" ))
		return RV_NOFUNC;

	jsval params[2], rval;

	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, currChar, runTime );
	JSObject *defObj = JSEngine->AcquireObject( IUE_CHAR, targChar, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCombatEnd", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnCombatEnd, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDeathBlow()
//|	Date		-	8th February, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when performing a death blow in combat
//|	Notes		-	Returning FALSE will also run hard code for this scenario
//|					Returning TRUE will override code's default handling of this scenario
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDeathBlow( CChar *mKilled, CChar *mKiller )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mKilled )) // || !ValidateObject( mKiller ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDeathBlow, "onDeathBlow" ))
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *killedObj = JSEngine->AcquireObject( IUE_CHAR, mKilled, runTime );
	JSObject *killerObj = JSEngine->AcquireObject( IUE_CHAR, mKiller, runTime );

	params[0] = OBJECT_TO_JSVAL( killedObj );
	params[1] = OBJECT_TO_JSVAL( killerObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeathBlow", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDeathBlow, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCombatDamageCalc()
//|	Date		-	21st March, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached every time combat damage is calculated
//|	Notes		-	Returning -1 will default to hard code handling of event
//|					Returning another value will override code's default handling of event
//o------------------------------------------------------------------------------------------------o
SI16 cScript::OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill, UI08 hitLoc )
{
	const SI16 RV_NOFUNC = -1;
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCombatDamageCalc, "onCombatDamageCalc" ))
		return RV_NOFUNC;

	SI16 funcRetVal	= -1;

	jsval rval, params[4];
	JSObject *attackerObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defenderObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attackerObj );
	params[1] = OBJECT_TO_JSVAL( defenderObj );
	params[2] = INT_TO_JSVAL( getFightSkill );
	params[3] = INT_TO_JSVAL( hitLoc );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCombatDamageCalc", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnCombatDamageCalc, false );
		return RV_NOFUNC;
	}
	JSEncapsulate damage( targContext, &rval );

	if( damage.isType( JSOT_INT ) || damage.isType( JSOT_DOUBLE ))	// They returned some sort of value
	{
		return static_cast<SI16>( damage.toInt() );
	}
	else
	{
		funcRetVal = -1;	// default to hard code
	}

	return funcRetVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDamage()
//|	Date		-	22nd March, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached takes damage
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDamage( CChar *damaged, CChar *attacker, SI16 damageValue, WeatherType damageType )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( damaged ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDamage, "onDamage" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *damagedObj = JSEngine->AcquireObject( IUE_CHAR, damaged, runTime );
	params[0] = OBJECT_TO_JSVAL( damagedObj );

	if( ValidateObject( attacker ))
	{
		JSObject *attackerObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
		params[1] = OBJECT_TO_JSVAL( attackerObj );
	}
	else
	{
		params[1] = JSVAL_NULL;
	}

	params[2] = INT_TO_JSVAL( damageValue );
	params[3] = INT_TO_JSVAL( damageType );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDamage", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDamage, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDamageDeal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached deals damage
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDamageDeal( CChar *attacker, CChar *damaged, SI16 damageValue, WeatherType damageType )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( damaged ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDamageDeal, "onDamageDeal" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *attackerObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	params[0] = OBJECT_TO_JSVAL( attackerObj );

	JSObject *damagedObj = JSEngine->AcquireObject( IUE_CHAR, damaged, runTime );
	params[1] = OBJECT_TO_JSVAL( damagedObj );


	params[2] = INT_TO_JSVAL( damageValue );
	params[3] = INT_TO_JSVAL( damageType );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDamageDeal", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDamageDeal, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnBuy()
//|	Date		-	26th November, 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs on vendors, triggered before vendor trade-gump is opened
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnBuy( CSocket *tSock, CChar *objVendor )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBuy, "onBuy" ))
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBuy", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnBuy, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSell()
//|	Date		-	26th November, 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs on vendors, triggered before vendor trade-gump is opened
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSell( CSocket *tSock, CChar *objVendor )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSell, "onSell" ))
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSell", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSell, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnBuyFromVendor()
//|	Date		-	26th November, 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens when an item is in the process of being bought 
//|					from an NPC vendor. Returning false/0 from the script will halt the purchase
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnBuyFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought, UI16 numItemsBuying )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == nullptr || numItemsBuying == 0 )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBuyFromVendor, "onBuyFromVendor" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemBought->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );
	params[3] = INT_TO_JSVAL( numItemsBuying );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBuyFromVendor", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnBuyFromVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSellToVendor()
//|	Date		-	26th November, 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens when an item is in the process of being sold to
//|					an NPC vendor. Returning false/0 from script will halt the sale
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSellToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold, UI16 numItemsSelling )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == nullptr || numItemsSelling == 0 )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSellToVendor, "onSellToVendor" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemSold->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );
	params[3] = INT_TO_JSVAL( numItemsSelling );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSellToVendor", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSellToVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnBoughtFromVendor()
//|	Date		-	26th November, 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens AFTER an item has been
//|					bought from an NPC vendor
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnBoughtFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought, UI16 numItemsBought )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBoughtFromVendor, "onBoughtFromVendor" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemBought->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );
	params[3] = INT_TO_JSVAL( numItemsBought );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBoughtFromVendor", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnBoughtFromVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSoldToVendor()
//|	Date		-	26th November, 2011
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens AFTER an item has been
//|					sold to an NPC vendor
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSoldToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold, UI16 numItemsSold )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSoldToVendor, "onSoldToVendor" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemSold->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );
	params[3] = INT_TO_JSVAL( numItemsSold );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSoldToVendor", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSoldToVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnHouseCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding house commands via JS script attached to multi
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnHouseCommand( CSocket *tSock, CMultiObj *objMulti, UI08 cmdId )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objMulti ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnHouseCommand, "onHouseCommand" ))
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *multiObj	= JSEngine->AcquireObject( IUE_ITEM, objMulti, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( multiObj );
	params[2] = INT_TO_JSVAL( cmdId );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHouseCommand", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnHouseCommand, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnMakeItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows doing additional stuff with newly crafted items
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnMakeItem( CSocket *mSock, CChar *objChar, CItem *objItem, UI16 createEntryId )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objChar ) || mSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnMakeItem, "onMakeItem" ))
		return RV_NOFUNC;

	jsval rval, params[4];
	JSObject *mySock	= JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, objChar, runTime );
	JSObject *myItem	= JSEngine->AcquireObject( IUE_ITEM, objItem, runTime );

	params[0] = OBJECT_TO_JSVAL( mySock );
	params[1] = OBJECT_TO_JSVAL( myChar );
	params[2] = OBJECT_TO_JSVAL( myItem );
	params[3] = INT_TO_JSVAL( createEntryId );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onMakeItem", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnMakeItem, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnPathfindEnd()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPCs after their pathfinding efforts come to and end
//|	Notes		-	pathfindResult gives a value that represents how the pathfinding ended				
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnPathfindEnd( CChar *npc, SI08 pathfindResult )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( npc ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnPathfindEnd, "onPathfindEnd" ))
		return RV_NOFUNC;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, npc, runTime );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( pathfindResult );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onPathfindEnd", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnPathfindEnd, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnEnterEvadeState()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPCs when they enter the evade state after failing to pathfind to
//|					a target in combat
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnEnterEvadeState( CChar *npc, CChar *enemy )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( npc ) || !ValidateObject( enemy ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEnterEvadeState, "onEnterEvadeState" ))
		return RV_NOFUNC;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, npc, runTime );
	JSObject *enemyObj = JSEngine->AcquireObject( IUE_CHAR, enemy, runTime );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( enemyObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEnterEvadeState", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnEnterEvadeState, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCarveCorpse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for corpse of character when player attempts to carve said corpse
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCarveCorpse( CChar *player, CItem *corpse )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( player ) || !ValidateObject( corpse ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCarveCorpse, "onCarveCorpse" ))
		return RV_NOFUNC;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	JSObject *corpseObj = JSEngine->AcquireObject( IUE_ITEM, corpse, runTime );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( corpseObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCarveCorpse", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnCarveCorpse, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDyeTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for dye tub when player attempts to dye something with it
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDyeTarget( CChar *player, CItem *dyeTub, CItem *target )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( player ) || !ValidateObject( dyeTub ) || !ValidateObject( target ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDyeTarget, "onDyeTarget" ))
		return RV_NOFUNC;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	JSObject *dyeTubObj = JSEngine->AcquireObject( IUE_ITEM, dyeTub, runTime );
	JSObject *targObj = JSEngine->AcquireObject( IUE_ITEM, target, runTime );

	jsval params[3], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( dyeTubObj );
	params[2] = OBJECT_TO_JSVAL( targObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDyeTarget", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnDyeTarget, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

bool cScript::EventExists( ScriptEvent eventNum ) const
{
	UI32 index = eventNum / 64;
	if( index > 2 )
		return false;

	return eventPresence[index].test(( eventNum % 64 ));
}
void cScript::SetEventExists( ScriptEvent eventNum, bool status )
{
	UI32 index = eventNum / 64;
	if( index > 2 )
		return;

	eventPresence[index].set(( eventNum % 64 ), status );
}

bool cScript::NeedsChecking( ScriptEvent eventNum ) const
{
	UI32 index = eventNum / 64;
	if( index > 2 )
		return false;

	return needsChecking[index].test(( eventNum % 64 ));
}
void cScript::SetNeedsChecking( ScriptEvent eventNum, bool status )
{
	UI32 index = eventNum / 64;
	if( index > 2 )
		return;

	needsChecking[index].set(( eventNum % 64 ), status );
}
