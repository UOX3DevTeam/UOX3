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
#include "cSpawnRegion.h"
#include "StringUtility.hpp"
#include "osunique.hpp"
#include <jsapi.h>
#include <js/Object.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/ErrorReport.h>
#include <js/Exception.h>
#include <js/GCVector.h>
#include <js/ValueArray.h>
#include <js/Warnings.h>

static constexpr SI08 RV_NOFUNC = -1;

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
	JS_FN( "StringToNum",				SE_StringToNum,				1, JSPROP_ENUMERATE ),	// This function will be deprecated
	JS_FN( "NumToString",				SE_NumToString,				1, JSPROP_ENUMERATE ),	// This function will be deprecated
	JS_FN( "NumToHexString",				SE_NumToHexString,			1, JSPROP_ENUMERATE ),	// This function will be deprecated
	JS_FN( "DoMovingEffect", 			SE_DoMovingEffect,			6, JSPROP_ENUMERATE ),
	JS_FN( "DoStaticEffect", 			SE_DoStaticEffect,			7, JSPROP_ENUMERATE ),
	JS_FN( "BroadcastMessage",			SE_BroadcastMessage,		1, JSPROP_ENUMERATE ),
	JS_FN( "RandomNumber",				SE_RandomNumber,			2, JSPROP_ENUMERATE ),
	JS_FN( "CalcCharFromSer",			SE_CalcCharFromSer,			1, JSPROP_ENUMERATE ),
	JS_FN( "CalcItemFromSer",			SE_CalcItemFromSer,			1, JSPROP_ENUMERATE ),
	JS_FN( "CalcMultiFromSer",			SE_CalcMultiFromSer,		1, JSPROP_ENUMERATE ),
	JS_FN( "CheckTimeSinceLastCombat",	SE_CheckTimeSinceLastCombat,2, JSPROP_ENUMERATE ),
	JS_FN( "CheckInstaLog",				SE_CheckInstaLog,			4, JSPROP_ENUMERATE ),
	JS_FN( "GetHour",					SE_GetHour,					0, JSPROP_ENUMERATE ),
	JS_FN( "GetMinute",					SE_GetMinute,				0, JSPROP_ENUMERATE ),
	JS_FN( "GetDay",						SE_GetDay,					0, JSPROP_ENUMERATE ),
	JS_FN( "SecondsPerUOMinute",			SE_SecondsPerUOMinute,		0, JSPROP_ENUMERATE ),
	JS_FN( "GetCurrentClock",			SE_GetCurrentClock,			0, JSPROP_ENUMERATE ),
	JS_FN( "GetStartTime",				SE_GetStartTime,			0, JSPROP_ENUMERATE ),
	JS_FN( "GetMurderThreshold",			SE_GetMurderThreshold,		0, JSPROP_ENUMERATE ),
	JS_FN( "RollDice",					SE_RollDice,				3, JSPROP_ENUMERATE ),
	JS_FN( "RaceCompareByRace",			SE_RaceCompareByRace,		2, JSPROP_ENUMERATE ),
	JS_FN( "GetRandomSOSArea",			SE_GetRandomSOSArea,		2, JSPROP_ENUMERATE ),
	JS_FN( "DoTempEffect",				SE_DoTempEffect,			7, JSPROP_ENUMERATE ),
	JS_FN( "MakeItem",					SE_MakeItem,				3, JSPROP_ENUMERATE ),
	JS_FN( "FindMulti",					SE_FindMulti,				4, JSPROP_ENUMERATE ),
	JS_FN( "GetItem",					SE_GetItem,					4, JSPROP_ENUMERATE ),
	JS_FN( "FindItem",					SE_FindItem,				5, JSPROP_ENUMERATE ),
	JS_FN( "FindItemBySection",			SE_FindItemBySection,		5, JSPROP_ENUMERATE ),
	JS_FN( "PossessTown",				SE_PossessTown,				2, JSPROP_ENUMERATE ),
	JS_FN( "IsRaceWeakToWeather",		SE_IsRaceWeakToWeather,		2, JSPROP_ENUMERATE ),
	JS_FN( "GetRaceSkillAdjustment",		SE_GetRaceSkillAdjustment,	2, JSPROP_ENUMERATE ),
	JS_FN( "UseItem",					SE_UseItem,					2, JSPROP_ENUMERATE ),
	JS_FN( "TriggerTrap",				SE_TriggerTrap,				2, JSPROP_ENUMERATE ),
	JS_FN( "CompareGuildByGuild",		SE_CompareGuildByGuild,		2, JSPROP_ENUMERATE ),
	JS_FN( "CreateNewGuild",				SE_CreateNewGuild,			0, JSPROP_ENUMERATE ),
	JS_FN( "CommandLevelReq",			SE_CommandLevelReq,			1, JSPROP_ENUMERATE ),
	JS_FN( "CommandExists",				SE_CommandExists,			1, JSPROP_ENUMERATE ),
	JS_FN( "FirstCommand",				SE_FirstCommand,			0, JSPROP_ENUMERATE ),
	JS_FN( "NextCommand",				SE_NextCommand,				0, JSPROP_ENUMERATE ),
	JS_FN( "FinishedCommandList",		SE_FinishedCommandList,		0, JSPROP_ENUMERATE ),
	JS_FN( "CreateDFNItem",				SE_CreateDFNItem,			3, JSPROP_ENUMERATE ),
	JS_FN( "CreateBlankItem",			SE_CreateBlankItem,			8, JSPROP_ENUMERATE ),
	JS_FN( "CreateHouse",				SE_CreateHouse,				8, JSPROP_ENUMERATE ),
	JS_FN( "CreateBaseMulti",			SE_CreateBaseMulti,			8, JSPROP_ENUMERATE ),
	JS_FN( "SpawnNPC",					SE_SpawnNPC,				6, JSPROP_ENUMERATE ),
	JS_FN( "GetPackOwner",				SE_GetPackOwner,			2, JSPROP_ENUMERATE ),
	JS_FN( "FindRootContainer",			SE_FindRootContainer,		2, JSPROP_ENUMERATE ),
	JS_FN( "CalcTargetedItem",			SE_CalcTargetedItem,		1, JSPROP_ENUMERATE ),
	JS_FN( "CalcTargetedChar",			SE_CalcTargetedChar,		1, JSPROP_ENUMERATE ),
	JS_FN( "GetTileIDAtMapCoord",		SE_GetTileIdAtMapCoord,		3, JSPROP_ENUMERATE ),
	JS_FN( "GetDictionaryEntry",			SE_GetDictionaryEntry,		2, JSPROP_ENUMERATE ),
	JS_FN( "Yell",						SE_Yell,					3, JSPROP_ENUMERATE ),
	JS_FN( "GetRaceCount",				SE_GetRaceCount,			0, JSPROP_ENUMERATE ),
	JS_FN( "WorldBrightLevel",			SE_WorldBrightLevel,		0, JSPROP_ENUMERATE ),
	JS_FN( "WorldDarkLevel",				SE_WorldDarkLevel,			0, JSPROP_ENUMERATE ),
	JS_FN( "WorldDungeonLevel",			SE_WorldDungeonLevel,		0, JSPROP_ENUMERATE ),
	JS_FN( "GetSpawnRegionFacetStatus",	SE_GetSpawnRegionFacetStatus,	1, JSPROP_ENUMERATE ),
	JS_FN( "SetSpawnRegionFacetStatus",	SE_SetSpawnRegionFacetStatus,	2, JSPROP_ENUMERATE ),
	JS_FN( "GetMoongateFacetStatus",		SE_GetMoongateFacetStatus,	1, JSPROP_ENUMERATE ),
	JS_FN( "SetMoongateFacetStatus",		SE_SetMoongateFacetStatus,	2, JSPROP_ENUMERATE ),
	JS_FN( "AreaCharacterFunction",		SE_AreaCharacterFunction,	3, JSPROP_ENUMERATE ),
	JS_FN( "AreaItemFunction",			SE_AreaItemFunction,		3, JSPROP_ENUMERATE ),
	JS_FN( "TriggerEvent",				SE_TriggerEvent,			3, JSPROP_ENUMERATE ),
	JS_FN( "DoesEventExist",				SE_DoesEventExist,			2, JSPROP_ENUMERATE ),
	JS_FN( "Reload",						SE_Reload,					1, JSPROP_ENUMERATE ),
	JS_FN( "SendStaticStats",			SE_SendStaticStats,			1, JSPROP_ENUMERATE ),
	JS_FN( "GetTileHeight",				SE_GetTileHeight,			1, JSPROP_ENUMERATE ),
	JS_FN( "IterateOver",				SE_IterateOver,				1, JSPROP_ENUMERATE ),
	JS_FN( "IterateOverSpawnRegions",	SE_IterateOverSpawnRegions,	1, JSPROP_ENUMERATE ),
	JS_FN( "GetSocketFromIndex",			SE_GetSocketFromIndex,		1, JSPROP_ENUMERATE ),
	JS_FN( "StaticAt",					SE_StaticAt,				4, JSPROP_ENUMERATE ),
	JS_FN( "StaticInRange",				SE_StaticInRange,			5, JSPROP_ENUMERATE ),

	JS_FN( "GetMapElevation",			SE_GetMapElevation,			3, JSPROP_ENUMERATE ),
	JS_FN( "IsInBuilding",				SE_IsInBuilding,			6, JSPROP_ENUMERATE ),
	JS_FN( "CheckStaticFlag",			SE_CheckStaticFlag,			5, JSPROP_ENUMERATE ),
	JS_FN( "CheckDynamicFlag",			SE_CheckDynamicFlag,		6, JSPROP_ENUMERATE ),
	JS_FN( "CheckTileFlag",				SE_CheckTileFlag,			2, JSPROP_ENUMERATE ),
	JS_FN( "DoesDynamicBlock",			SE_DoesDynamicBlock,		9, JSPROP_ENUMERATE ),
	JS_FN( "DoesStaticBlock",			SE_DoesStaticBlock,			5, JSPROP_ENUMERATE ),
	JS_FN( "DoesMapBlock",				SE_DoesMapBlock,			8, JSPROP_ENUMERATE ),
	JS_FN( "DoesCharacterBlock",			SE_DoesCharacterBlock,		5, JSPROP_ENUMERATE ),
	JS_FN( "DistanceBetween",			SE_DistanceBetween,			4, JSPROP_ENUMERATE ),

	JS_FN( "ResourceArea",				SE_ResourceArea,			2, JSPROP_ENUMERATE ),
	JS_FN( "ResourceAmount",				SE_ResourceAmount,			2, JSPROP_ENUMERATE ),
	JS_FN( "ResourceTime",				SE_ResourceTime,			2, JSPROP_ENUMERATE ),
	JS_FN( "ResourceRegion",				SE_ResourceRegion,			3, JSPROP_ENUMERATE ),
	JS_FN( "Moon",						SE_Moon,					2, JSPROP_ENUMERATE ),

	JS_FN( "GetTownRegion",				SE_GetTownRegion,			1, JSPROP_ENUMERATE ),
	JS_FN( "GetTownRegionFromXY",		SE_GetTownRegionFromXY,		4, JSPROP_ENUMERATE ),
	JS_FN( "GetSpawnRegion",				SE_GetSpawnRegion,			1, JSPROP_ENUMERATE ),
	JS_FN( "GetSpawnRegions",			SE_GetSpawnRegions,			4, JSPROP_ENUMERATE ),
	JS_FN( "GetSpawnRegionCount",		SE_GetSpawnRegionCount,		0, JSPROP_ENUMERATE ),


	JS_FN( "RegisterCommand",			SE_RegisterCommand,			3, JSPROP_ENUMERATE ),
	JS_FN( "DisableCommand",				SE_DisableCommand,			1, JSPROP_ENUMERATE ),
	JS_FN( "EnableCommand",				SE_EnableCommand,			1, JSPROP_ENUMERATE ),

	JS_FN( "RegisterKey",				SE_RegisterKey,				2, JSPROP_ENUMERATE ),
	JS_FN( "DisableKey",					SE_DisableKey,				1, JSPROP_ENUMERATE ),
	JS_FN( "EnableKey",					SE_EnableKey,				1, JSPROP_ENUMERATE ),

	JS_FN( "RegisterConsoleFunc",		SE_RegisterConsoleFunc,		2, JSPROP_ENUMERATE ),
	JS_FN( "DisableConsoleFunc",			SE_DisableConsoleFunc,		1, JSPROP_ENUMERATE ),
	JS_FN( "EnableConsoleFunc",			SE_EnableConsoleFunc,		1, JSPROP_ENUMERATE ),

	JS_FN( "RegisterSpell",				SE_RegisterSpell,			2, JSPROP_ENUMERATE ),
	JS_FN( "DisableSpell",				SE_DisableSpell,			1, JSPROP_ENUMERATE ),
	JS_FN( "EnableSpell",				SE_EnableSpell,				1, JSPROP_ENUMERATE ),

	JS_FN( "RegisterSkill",				SE_RegisterSkill,			2, JSPROP_ENUMERATE ),
	JS_FN( "RegisterPacket",				SE_RegisterPacket,			2, JSPROP_ENUMERATE ),
	JS_FN( "ReloadJSFile",				SE_ReloadJSFile,			1, JSPROP_ENUMERATE ),

	JS_FN( "ValidateObject",				SE_ValidateObject,			1, JSPROP_ENUMERATE ),

	JS_FN( "ApplyDamageBonuses",			SE_ApplyDamageBonuses,		6, JSPROP_ENUMERATE ),
	JS_FN( "ApplyDefenseModifiers",		SE_ApplyDefenseModifiers,	7, JSPROP_ENUMERATE ),
	JS_FN( "WillResultInCriminal",		SE_WillResultInCriminal,	2, JSPROP_ENUMERATE ),

	JS_FN( "CreateParty",				SE_CreateParty,				1, JSPROP_ENUMERATE ),

	JS_FN( "GetClientFeature",			SE_GetClientFeature,		1, JSPROP_ENUMERATE ),
	JS_FN( "GetServerFeature",			SE_GetServerFeature,		1, JSPROP_ENUMERATE ),
	JS_FN( "GetServerSetting",			SE_GetServerSetting,		1, JSPROP_ENUMERATE ),

	JS_FN( "DeleteFile",					SE_DeleteFile,				2, JSPROP_ENUMERATE ),

	JS_FN( "GetAccountCount",			SE_GetAccountCount,			0, JSPROP_ENUMERATE ),
	JS_FN( "GetPlayerCount",				SE_GetPlayerCount,			0, JSPROP_ENUMERATE ),
	JS_FN( "GetItemCount",				SE_GetItemCount,			0, JSPROP_ENUMERATE ),
	JS_FN( "GetMultiCount",				SE_GetMultiCount,			0, JSPROP_ENUMERATE ),
	JS_FN( "GetCharacterCount",			SE_GetCharacterCount,		0, JSPROP_ENUMERATE ),
	JS_FN( "GetServerVersionString",		SE_GetServerVersionString,	0, JSPROP_ENUMERATE ),
	JS_FN( "EraStringToNum",				SE_EraStringToNum,			1, JSPROP_ENUMERATE ),
	JS_FN( "GetCommandLevelVal",			SE_GetCommandLevelVal,		1, JSPROP_ENUMERATE ),

	JS_FN( "BASEITEMSERIAL",				SE_BASEITEMSERIAL,			0, JSPROP_ENUMERATE ),
	JS_FN( "INVALIDSERIAL",				SE_INVALIDSERIAL,			0, JSPROP_ENUMERATE ),
	JS_FN( "INVALIDID",					SE_INVALIDID,				0, JSPROP_ENUMERATE ),
	JS_FN( "INVALIDCOLOUR",				SE_INVALIDCOLOUR,			0, JSPROP_ENUMERATE ),

	JS_FS_END
};

void UOX3ErrorReporter( JSContext *cx, JSErrorReport *report )
{
	JSErrorInfo* errorInfo = ( JSErrorInfo *)JS_GetContextPrivate( cx );
	if( report == nullptr )
	{
		if( errorInfo != nullptr )
		{
			errorInfo->message = "No detailed data";
		}
		else
		{
			Console.Error( "JS script failure: No detailed data" );
		}
		return;
	}

	if( errorInfo != nullptr )
	{
		// Silently populate errorInfo for cScript constructor during compilation
		if( report->filename == nullptr )
		{
			errorInfo->message = "No detailed data";
			return;
		}

		errorInfo->message  = report->message().c_str();
		errorInfo->filename = report->filename;
		errorInfo->lineNum  = report->lineno;
	}
	else
	{
		// Output errors directly here, triggered by runtime execution of scripts
		UI16 scriptNum = 0xFFFF;
		if( JSMapping != nullptr )
		{
			cScript *activeScript = JSMapping->currentActive( false );
			if( activeScript != nullptr )
			{
				scriptNum = activeScript->GetScriptID();
			}
		}
		Console.Error( oldstrutil::format( "JS script failure: Script Number (%u) Message (%s)", scriptNum, report->message().c_str() ));
		if( report->filename == nullptr )
		{
			Console.Error( "No detailed data" );
			return;
		}

		Console.Error( oldstrutil::format( "Filename: %s", report->filename ));
		Console.Error( oldstrutil::format( "Line Number: %i", report->lineno ));
	}
}

// Error reporters registered with JS::SetWarningReporter only receive warnings.
// Turn a pending exception into a JSErrorReport so compile/runtime failures use
// the same UOX3 reporting path as warnings.
static void ReportPendingJSException( JSContext *cx )
{
	if( !JS_IsExceptionPending( cx ) )
		return;

	JS::ExceptionStack exceptionStack( cx );
	if( JS::GetPendingExceptionStack( cx, &exceptionStack ) )
	{
		JS::ErrorReportBuilder report( cx );
		if( report.init( cx, exceptionStack, JS::ErrorReportBuilder::NoSideEffects ) )
		{
			UOX3ErrorReporter( cx, report.report() );
		}
	}
	JS_ClearPendingException( cx );
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

	JS_ReportErrorUTF8( cx, "%s", g_errorMessage.c_str() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SI32 TryParseJSVal( JS::Value toParse )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper function to parse JS::Value returned from script
//o------------------------------------------------------------------------------------------------o
SI32 TryParseJSVal( JS::Value toParse )
{
	if( toParse.isNull() || ( !toParse.isBoolean() && !toParse.isInt32()))
	{
		// JS::Value is neither a bool nor an int - possibly an object!
		return 0;
	}
	else if( !toParse.isBoolean() && toParse.isInt32() )
	{
		// JS::Value is an int!
		return static_cast<SI32>( toParse.toInt32() );
	}
	else if( toParse.isBoolean() )
	{
		// JS::Value is a bool! convert it to int
		return( toParse.toBoolean() );
	}
	else
	{
		return -1;
	}
}

cScript::cScript( std::string targFile, UI08 rT, UI16 scrID ) : isFiring( false ), runTime( rT ), scriptID( scrID )
{
	for( SI32 i = 0; i < 3; ++i )
	{
		eventPresence[i].set();
		needsChecking[i].set();
	}

	targContext = JSEngine->GetContext( runTime );
	if( targContext == nullptr )
		return;

	JS::RootedObject glob( targContext, JS::CurrentGlobalOrNull( targContext ) );
	if( glob == nullptr )
		return;

	JS::RootedObject newTargObject( targContext, JS_NewObject( targContext, &uox_class ) );
	if( newTargObject == nullptr )
		return;
	targObject.init( targContext, newTargObject );
	scriptObj.init( targContext );
	targScript.init( targContext );

	JS::SetWarningReporter( targContext, UOX3ErrorReporter );
	JS_DefineFunctions( targContext, newTargObject, my_functions );
	JS_SetReservedSlot( newTargObject, 0, JS::PrivateValue( this ) );

	// Define some global constants that can be used in any scripts
#if defined( UOX_DEBUG_MODE )
	JS_DefineProperty( targContext, newTargObject, "UOX_DEBUG_MODE", JS::TrueHandleValue, JSPROP_PERMANENT | JSPROP_READONLY );
#else
	JS_DefineProperty( targContext, newTargObject, "UOX_DEBUG_MODE", JS::FalseHandleValue, JSPROP_PERMANENT | JSPROP_READONLY );
#endif

	JS::RootedObject proto( targContext, JSEngine->GetPrototype( rT, JSP_SCRIPT ) );
	JS::RootedObject newScriptObj( targContext,
		JS_NewObjectWithGivenProto( targContext, &uox_class, proto ) );
	if( newScriptObj == nullptr ||
		!JS_DefineProperty( targContext, newTargObject, "SCRIPT", newScriptObj, 0 ) )
		return;
	scriptObj = newScriptObj;
	JS_SetReservedSlot( newScriptObj, 0, JS::PrivateValue( this ) );


	// Let's fetch some error details when we compile the script
	JSErrorInfo errorDetails;
	JS_SetContextPrivate( targContext, &errorDetails );
	std::ifstream scriptFile( targFile, std::ios::binary );
	std::string scriptSource( ( std::istreambuf_iterator<char>( scriptFile ) ),
		std::istreambuf_iterator<char>() );
	JS::CompileOptions compileOptions( targContext );
	compileOptions.setFileAndLine( targFile.c_str(), 1 )
		.setNonSyntacticScope( true );
	JS::SourceText<mozilla::Utf8Unit> sourceText;
	JS::RootedScript compiledScript( targContext );
	if( scriptFile && sourceText.init( targContext, scriptSource.c_str(), scriptSource.size(),
		JS::SourceOwnership::Borrowed ) )
	{
		compiledScript = JS::Compile( targContext, compileOptions, sourceText );
	}
	targScript = compiledScript;
	if( targScript == nullptr )
	{
		ReportPendingJSException( targContext );
	}
	JS_SetContextPrivate( targContext, nullptr );

	if( targScript == nullptr )
	{
		std::string errorMessage, errorFile, errorLineStr, tokenPtrLine;
		uint32_t errorLine = 0;
		UI16 scriptNum = scriptID;
		if( !errorDetails.message.empty() )
		{
			// Triggered during compilation of scripts at startup, or upon full reload of script engine at runtime
			errorMessage = errorDetails.message;
			errorFile = errorDetails.filename;
			errorLine = errorDetails.lineNum;
			errorLineStr = errorDetails.lineSource;
			tokenPtrLine = errorDetails.tokenPointer;
		}
		else
		{
			// Triggered when reloading individual scripts at runtime
			JS::RootedValue pendingException( targContext );
			if( JS_GetPendingException( targContext, &pendingException ) == true )
			{
				if( pendingException.isObject() && !pendingException.isNull() )
				{
					JS::RootedObject errObj( targContext, &pendingException.toObject() );

					// Get error message from pending exception
					JS::RootedValue messageVal( targContext );
					if( JS_GetProperty( targContext, errObj, "message", &messageVal ) == true )
					{
						errorMessage = JS_GetStringBytes(targContext, messageVal);
					}

					// Get line number from pending exception
					JS::RootedValue lineVal( targContext );
					if( JS_GetProperty( targContext, errObj, "lineNumber", &lineVal ) == true )
					{
						JS::RootedValue rootedValue( targContext, lineVal );
						JS::ToUint32( targContext, rootedValue, &errorLine );
					}

					// Get filename from pending exception
					JS::RootedValue fileVal( targContext );
					if( JS_GetProperty( targContext, errObj, "fileName", &fileVal ) == true )
					{
						errorFile = JS_GetStringBytes( targContext, fileVal );
					}
				}

				JS_ClearPendingException( targContext );
			}
		}

		// Spit out the distinct parts of the error message line by line
		Console.Error( oldstrutil::format( "JS script failure: Script Number (%u) Message (%s)", scriptNum, errorMessage.c_str() ));
		Console.Error( oldstrutil::format( "Filename: %s", errorFile.c_str() ));
		Console.Error( oldstrutil::format( "Line Number: %i", errorLine ));
		if( !errorLineStr.empty() )
		{
			Console.Error( oldstrutil::format( "Erroneous Line: %s", oldstrutil::trim( errorLineStr ).c_str() ));
		}
		if( !tokenPtrLine.empty() )
		{
			Console.Error( oldstrutil::format( "Token Ptr: %s", tokenPtrLine.c_str() ));
		}

		throw std::runtime_error( "Error during JS Compilation" );
	}

	JS::RootedValue rval( targContext );
	JS::RootedScript rootedScript( targContext, targScript );
	JS::RootedObjectVector environmentChain( targContext );
	if( !environmentChain.append( targObject ))
	{
		throw std::runtime_error( "Unable to create JS script environment" );
	}
	CActiveScriptGuard activeScriptGuard( JSMapping, this );
	bool ok = JS_ExecuteScript( targContext, environmentChain, rootedScript, &rval );
	if( ok != true )
	{
		ReportPendingJSException( targContext );
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

}
void cScript::CollectGarbage( void )
{
	Cleanup();
}
cScript::~cScript()
{
	Cleanup();
	targScript.reset();
	scriptObj.reset();
	targObject.reset();
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

bool cScript::InvokeEvent( const char* name, unsigned int argc, const JS::Value* argv, JS::Value* rval )
{
	CActiveScriptGuard activeScriptGuard( JSMapping, this );
#if defined UOX_DEBUG_MODE
	Console.Log( oldstrutil::format( "Triggering event '%s' from script %d", name, GetScriptID() ) );
#endif
	JS::RootedObject rootedObj( targContext, targObject );
	JS::RootedValueVector rootedArgs( targContext );
	if( argc > 0 && !rootedArgs.append( argv, argc ))
	{
		ReportPendingJSException( targContext );
		return false;
	}
	JS::RootedValue rootedResult( targContext );
	bool rVal = JS_CallFunctionName( targContext, rootedObj, name, rootedArgs, &rootedResult );
	if( rval != nullptr )
	{
		*rval = rootedResult;
	}
	if( !rVal )
	{
		ReportPendingJSException( targContext );
	}
	return rVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStart()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes for any instance of a script with onEvent that is attached to an
//|					object, as soon as shard has completed startup and world has fully loaded.
//|					Provides object context via the included parameter.
//o------------------------------------------------------------------------------------------------o
bool cScript::OnStart( void *myObj, SI32 objType )
{
	if( !ExistAndVerify( seOnStart, "onStart" ))
		return false;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<1> params( targContext );
	if( myObj != nullptr && objType != -1 )
	{
		JS::RootedObject jsObj( targContext, JSEngine->AcquireObject( static_cast<IUEEntries>( objType ), myObj, runTime ) );
		params[0].set( JS::ObjectOrNullValue( jsObj ) );
	}

	bool retVal = InvokeEvent( "onStart", 1, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnStart, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnScriptLoad()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Executes once for any script with onScriptLoad event as soon as shard has
//|					completed startup and world has fully loaded. No object context.
//o------------------------------------------------------------------------------------------------o
bool cScript::OnScriptLoad( void )
{
	if( !ExistAndVerify( seOnScriptLoad, "onScriptLoad" ))
		return false;

	JS::RootedValue rval( targContext );
	bool retVal = InvokeEvent( "onScriptLoad", 0, nullptr, rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnScriptLoad, false );
	}

	return ( retVal == true );
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
bool cScript::DoesEventExist( const char *eventToFind )
{
	JS::RootedValue Func( targContext, JS::NullValue() );
	JS_GetProperty( targContext, targObject, eventToFind, &Func );
	if( Func == JS::UndefinedValue() )
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	UI08 paramType = 0;
	JS::RootedObject myObj( targContext );
	if( thingCreated->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingCreated, runTime );
		paramType = 1;
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingCreated, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( paramType ) );
	bool retVal = InvokeEvent( functionName.c_str(), 2, params.begin(), rval.address() );
	if( !retVal )
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

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   cScript::OnSpawn()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Triggers for object with event attached when spawned from a spawn region
//| Notes       -   Calls JS onSpawn( objSpawned, spawnRegion )
//|                 spawnRegion is the REGIONSPAWN ID (UI32), or 0 if unknown
//o------------------------------------------------------------------------------------------------o
bool cScript::OnSpawn( CBaseObject *objectSpawned, UI16 spawnRegion )
{
	if( !ValidateObject( objectSpawned ))
		return false;

	if( !ExistAndVerify( seOnSpawn, "onSpawn" ))
		return false;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject jsObj( targContext );
	if( objectSpawned->GetObjType() == OT_CHAR )
	{
		jsObj = JSEngine->AcquireObject( IUE_CHAR, static_cast<CChar *>( objectSpawned ), runTime );
	}
	else
	{
		jsObj = JSEngine->AcquireObject( IUE_ITEM, static_cast<CItem *>( objectSpawned ), runTime );
	}

	params[0].set( JS::ObjectOrNullValue( jsObj ) );
	params[1].set( JS::Int32Value( spawnRegion ) );
	bool retVal = InvokeEvent( "onSpawn", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnSpawn, false );
	}

	return ( retVal == true );
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	UI08 paramType = 0;
	JS::RootedObject myObj( targContext );
	if( thingDestroyed->GetObjType() != OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingDestroyed, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingDestroyed, runTime );
		paramType = 1;
	}
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( paramType ) );
	bool retVal = InvokeEvent( "onDelete", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnDelete, false );
	}

	return ( retVal == true );
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
	if( speech == nullptr || !ValidateObject( personTalking ) || !ValidateObject( talkingTo ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpeech, "onSpeech" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	std::string lwrSpeech	= speech;
	JS::RootedString strSpeech( targContext, JS_NewStringCopyZ( targContext, oldstrutil::lower( lwrSpeech ).c_str() ));

	JS::RootedObject ptObj( targContext, JSEngine->AcquireObject( IUE_CHAR, personTalking, runTime ) );
	JS::RootedObject ttObj( targContext );
	if( talkingTo->CanBeObjType( OT_CHAR ))
	{
		ttObj = JSEngine->AcquireObject( IUE_CHAR, talkingTo, runTime );
	}
	else if( talkingTo->CanBeObjType( OT_ITEM ))
	{
		ttObj = JSEngine->AcquireObject( IUE_ITEM, talkingTo, runTime );
	}

	params[0].set( JS::StringValue( strSpeech ) );
	params[1].set( JS::ObjectOrNullValue( ptObj ) );
	params[2].set( JS::ObjectOrNullValue( ttObj ) );
	bool retVal = InvokeEvent( "onSpeech", 3, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnSpeech, false );
		return RV_NOFUNC;
	}

	if( !rval.isNull() )	// They returned some sort of value
	{
		if( rval.isInt32() )
		{
			return static_cast<SI08>(rval.toInt32());
		}
		else if( rval.isBoolean() )
		{
			if( rval.toBoolean() == true)
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	if( srcObj->CanBeObjType( OT_CHAR ))
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, srcObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, srcObj, runTime );
	}

	JS::RootedObject myObj2( targContext );
	if( objInRange->CanBeObjType( OT_CHAR ))
	{
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objInRange, runTime );
	}
	else
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objInRange, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( myObj2 ) );
	bool retVal = InvokeEvent( "inRange", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seInRange, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCollide()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when a character collides with it
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCollide( CSocket *tSock, CChar *objColliding, CBaseObject *objCollideWith )
{
	if( !ValidateObject( objColliding ) || !ValidateObject( objCollideWith ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCollide, "onCollide" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objColliding, runTime ) );
	JS::RootedObject myObj2( targContext );
	if( objCollideWith->GetObjType() == OT_CHAR )
	{
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objCollideWith, runTime );
	}
	else
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objCollideWith, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( myObj2 ) );
	if( !InvokeEvent( "onCollide", 3, params.begin(), rval.address() ) )
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
	if( !ValidateObject( sourceObj ) || !ValidateObject( charInRange ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnMoveDetect, "onMoveDetect" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<5> params( targContext );
	JS::RootedObject myObj( targContext );
	if( sourceObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, sourceObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, sourceObj, runTime );
	}
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, charInRange, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::Int32Value( rangeToChar ) );
	params[3].set( JS::Int32Value( oldCharX ) );
	params[4].set( JS::Int32Value( oldCharY ) );
	if( !InvokeEvent( "onMoveDetect", 5, params.begin(), rval.address() ) )
	{
		SetEventExists( seOnMoveDetect, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpeedHackDetect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when movement speed hack is detected
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpeedHackDetect( CChar *player, SI32 deltaTime )
{
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpeedHackDetect, "onSpeedHackDetect" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myChar( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myChar ) );
	params[1].set( JS::Int32Value( deltaTime ) );
	if( !InvokeEvent( "onSpeedHackDetect", 2, params.begin(), rval.address() ) )
	{
		SetEventExists( seOnSpeedHackDetect, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpeedHackSuspicion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when movement speed hack is detected
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSpeedHackSuspicion( CChar *player, SI32 grossDebt, SI32 deltaTime )
{
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpeedHackSuspicion, "onSpeedHackSuspicion" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject myChar( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myChar ) );
	params[1].set( JS::Int32Value( grossDebt ) );
	params[2].set( JS::Int32Value( deltaTime ) );
	if( !InvokeEvent( "onSpeedHackSuspicion", 3, params.begin(), rval.address() ) )
	{
		SetEventExists( seOnSpeedHackSuspicion, false );
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
	if( !ValidateObject( thief ) || !ValidateObject( theft ) || !ValidateObject( victim ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSteal, "onSteal" ))
		return RV_NOFUNC;

	JS::RootedObject thiefCharObj( targContext, JSEngine->AcquireObject( IUE_CHAR, thief, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, theft, runTime ) );
	JS::RootedObject victimCharObj( targContext, JSEngine->AcquireObject( IUE_CHAR, victim, runTime ) );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( thiefCharObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	params[2].set( JS::ObjectOrNullValue( victimCharObj ) );
	bool retVal = InvokeEvent( "onSteal", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( dispelled ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDispel, "onDispel" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	if( dispelled->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, dispelled, runTime );
		params[1].set( JS::Int32Value( 0 ) );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, dispelled, runTime );
		params[1].set( JS::Int32Value( 1 ) );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	bool retVal = InvokeEvent( "onDispel", 2, params.begin(), rval.address() );

	if( !retVal )
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject myObj( targContext );
	if( skillUse->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, skillUse, runTime );
		params[2].set( JS::Int32Value( 0 ) );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, skillUse, runTime );
		params[2].set( JS::Int32Value( 1 ) );
	}
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( skillUsed ) );
	bool retVal = InvokeEvent( "onSkill", 3, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnSkill, false );
	}

	return ( retVal == true );
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject tooltipObj( targContext );
	if( myObj->CanBeObjType( OT_CHAR ))
	{
		tooltipObj = JSEngine->AcquireObject( IUE_CHAR, myObj, runTime );
	}
	else if( myObj->CanBeObjType( OT_ITEM ))
	{
		tooltipObj = JSEngine->AcquireObject( IUE_ITEM, myObj, runTime );
	}
	JS::RootedObject sockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, pSocket, runTime ) );
	params[0].set( JS::ObjectOrNullValue( tooltipObj ) );
	params[1].set( JS::ObjectOrNullValue( sockObj ) );
	bool retVal = InvokeEvent( "onTooltip", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnTooltip, false );
	}

	// If rval is negative, it's possible some other function/method called from within Ontooltip() encountered
	// an error. Abort attempt to turn it into a string - it might crash the server!
	if( rval.isInt32() && rval.toInt32() < 0 )
	{
		Console.Error( "Handled exception in cScript.cpp OnTooltip() - invalid return value/error encountered!" );
		return "";
	}

	try
	{
		std::string returnString = JS_GetStringBytes( targContext, rval );

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
		JS::RootedValue rval( targContext );
		JS::RootedValueArray<3> params( targContext );
		// Create JS object reference for myObj, based on whether it's an item or character
		JS::RootedObject nameRequestObj( targContext );
		if( myObj->CanBeObjType( OT_CHAR ))
		{
			nameRequestObj = JSEngine->AcquireObject( IUE_CHAR, myObj, runTime );
		}
		else if( myObj->CanBeObjType( OT_ITEM ))
		{
			nameRequestObj = JSEngine->AcquireObject( IUE_ITEM, myObj, runTime );
		}

		// Create JS object reference for the name requester (which might be nullptr!)
		JS::RootedObject nameRequesterObj( targContext );
		if( nameRequester != nullptr )
		{
			nameRequesterObj = JSEngine->AcquireObject( IUE_CHAR, nameRequester, runTime );
		}

		params[0].set( JS::ObjectOrNullValue( nameRequestObj ) );
		params[1].set( JS::ObjectOrNullValue( nameRequesterObj ) );
		params[2].set( JS::Int32Value( requestSource ) );
		bool retVal = InvokeEvent( "onNameRequest", 3, params.begin(), rval.address() );
		if( !retVal )
		{
			SetEventExists( seOnNameRequest, false );
		}

		// If rval is negative, it's possible some other function/method called from within onNameRequest() encountered
		// an error. Abort attempt to turn it into a string - it might crash the server!
		if( rval.isInt32() && rval.toInt32() < 0 )
		{
			Console.Error( "Handled exception in cScript.cpp OnNameRequest() - invalid return value/error encountered!" );
			return "";
		}

		std::string returnString = JS_GetStringBytes( targContext, rval );

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
//|	Function	-	cScript::onQuestToggle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached and player click's the context
//|					option toggle quest item.
//o------------------------------------------------------------------------------------------------o
SI08 cScript::onQuestToggle( CChar* toggler, CItem *iUsing )
{
    if( !ValidateObject( toggler) || !ValidateObject( iUsing ))
		return RV_NOFUNC;

    if( !ExistAndVerify( seOnQuestToggle, "onQuestToggle" ))
		return RV_NOFUNC;

    JS::RootedValue rval( targContext );
    JS::RootedValueArray<2> params( targContext );
    JS::RootedObject togglerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, toggler, runTime ));
    JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime ) );
    params[0].set( JS::ObjectOrNullValue( togglerObj ) );
    params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onQuestToggle", 2, params.begin(), rval.address() );
    if( !retVal )
    {
        SetEventExists( seOnQuestToggle, false );
    }

    return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|    Function    -    cScript::OnAttack()
//o------------------------------------------------------------------------------------------------o
//|    Purpose        -    Triggers for character with event attached when attacking someone
//|                    Will also trigger the onDefense event for the character being attacked
//o------------------------------------------------------------------------------------------------o
bool cScript::OnAttack( CChar *attacker, CChar *defender, bool hitStatus, SI08 hitLoc, UI16 damageDealt )
{
    if( !ValidateObject( attacker ) || !ValidateObject( defender ))
        return false;

    if( !ExistAndVerify( seOnAttack, "onAttack" ))
        return false;

    JS::RootedValue rval( targContext );
    JS::RootedValueArray<5> params( targContext );
    JS::RootedObject attObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
    JS::RootedObject defObj( targContext, JSEngine->AcquireObject( IUE_CHAR, defender, runTime ) );
    params[0].set( JS::ObjectOrNullValue( attObj ) );
    params[1].set( JS::ObjectOrNullValue( defObj ) );
    params[2].set( JS::BooleanValue( hitStatus ) );
    params[3].set( JS::Int32Value( hitLoc ) );
    params[4].set( JS::Int32Value( damageDealt ) );
    bool retVal = InvokeEvent( "onAttack", 5, params.begin(), rval.address() );
    if( !retVal )
    {
        SetEventExists( seOnAttack, false );
    }

    return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|    Function    -    cScript::OnDefense()
//o------------------------------------------------------------------------------------------------o
//|    Purpose        -    Triggers for character with event attached when being attacked
//o------------------------------------------------------------------------------------------------o
bool cScript::OnDefense( CChar *attacker, CChar *defender, bool hitStatus, SI08 hitLoc, UI16 damageReceived )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return false;

	if( !ExistAndVerify( seOnDefense, "onDefense" ))
		return false;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<5> params( targContext );
	JS::RootedObject attObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
	JS::RootedObject defObj( targContext, JSEngine->AcquireObject( IUE_CHAR, defender, runTime ) );
	params[0].set( JS::ObjectOrNullValue( attObj ) );
	params[1].set( JS::ObjectOrNullValue( defObj ) );
	params[2].set( JS::BooleanValue( hitStatus ) );
	params[3].set( JS::Int32Value( hitLoc ) );
	params[4].set( JS::Int32Value( damageReceived ) );
	bool retVal = InvokeEvent( "onDefense", 5, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnDefense, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillGain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when gaining skillpoints
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillGain( CChar *player, SI08 skill, UI32 skillGainAmount )
{
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillGain, "onSkillGain" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( skill ) );
	params[2].set( JS::Int32Value( skillGainAmount ) );
	bool retVal = InvokeEvent( "onSkillGain", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnStatGained, "onStatGained" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( stat ) );
	params[2].set( JS::Int32Value( skill ) );
	params[3].set( JS::Int32Value( statGainedAmount ) );
	bool retVal = InvokeEvent( "onStatGained", 4, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( stat ) );
	params[2].set( JS::Int32Value( skill ) );
	params[3].set( JS::Int32Value( statGainAmount ) );
	bool retVal = InvokeEvent( "onStatGain", 4, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnStatGain, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnVirtueGumpPress()
//|	Date		-	19/01/2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when activating Virtue Gump icon
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonId )
{
	if( !ValidateObject( mChar ) || !ValidateObject( tChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnVirtueGumpPress, "onVirtueGumpPress" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	JS::RootedObject targObj( targContext, JSEngine->AcquireObject( IUE_CHAR, tChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( targObj ) );
	params[2].set( JS::Int32Value( buttonId ) );
	bool retVal = InvokeEvent( "onVirtueGumpPress", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnQuestGump, "onQuestGump" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<1> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onQuestGump", 1, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnQuestGump, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnGuildButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who activate Guild button in paperdoll
//|					Return true to prevent additional onGuildButton events from triggering
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnGuildButton( CChar *mChar )
{
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnGuildButton, "onGuildButton" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<1> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onGuildButton", 1, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnGuildButton, false );
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
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnHelpButton, "onHelpButton" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<1> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onHelpButton", 1, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( baseObj ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnContextMenuRequest, "onContextMenuRequest" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject mySockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject myObj( targContext );
	if( baseObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, baseObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, baseObj, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( mySockObj ) );
	params[1].set( JS::ObjectOrNullValue( myObj ) );
	bool retVal = InvokeEvent( "onContextMenuRequest", 2, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( baseObj ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnContextMenuSelect, "onContextMenuSelect" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject mySockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject myObj( targContext );
	if( baseObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, baseObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, baseObj, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( mySockObj ) );
	params[1].set( JS::ObjectOrNullValue( myObj ) );
	params[2].set( JS::Int32Value( popupEntry ) );
	bool retVal = InvokeEvent( "onContextMenuSelect", 3, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnWarModeToggle, "onWarModeToggle" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<1> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onWarModeToggle", 1, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpecialMove, "onSpecialMove" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( abilityId ) );
	bool retVal = InvokeEvent( "onSpecialMove", 2, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( item ) || !ValidateObject( dropper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDrop, "onDrop" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, dropper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, item, runTime ) );
	params[0].set( JS::ObjectOrNullValue( itemObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onDrop", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( item ) || !ValidateObject( dropper ) || !ValidateObject( dest ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDropItemOnItem, "onDropItemOnItem" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, dropper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, item, runTime ) );
	JS::RootedObject destObj( targContext, JSEngine->AcquireObject( IUE_ITEM, dest, runTime ) );
	params[0].set( JS::ObjectOrNullValue( itemObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( destObj ) );
	bool retVal = InvokeEvent( "onDropItemOnItem", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( item ) || !ValidateObject( pickerUpper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnPickup, "onPickup" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pickerUpper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, item, runTime ) );
	JS::RootedObject objContObj( targContext );
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

	params[0].set( JS::ObjectOrNullValue( itemObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( objContObj ) );
	bool retVal	= InvokeEvent( "onPickup", 3, params.begin(), rval.address() );

	if( !retVal )
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

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject contObj( targContext, JSEngine->AcquireObject( IUE_ITEM, contItem, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, item, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, itemRemover, runTime ) );
	params[0].set( JS::ObjectOrNullValue( contObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	params[2].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onContRemoveItem", 3, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnContRemoveItem, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSwing()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when swung in combat
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
{
	if( !ValidateObject( swinger ) || !ValidateObject( swingTarg ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSwing, "onSwing" ))
		return RV_NOFUNC;

	JS::RootedObject itemObj( targContext, ( ValidateObject( swinging ) ? JSEngine->AcquireObject( IUE_ITEM, swinging, runTime ) : nullptr ) );
	JS::RootedObject attObj( targContext, JSEngine->AcquireObject( IUE_CHAR, swinger, runTime ) );
	JS::RootedObject defObj( targContext, JSEngine->AcquireObject( IUE_CHAR, swingTarg, runTime ) );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( itemObj ) );
	params[1].set( JS::ObjectOrNullValue( attObj ) );
	params[2].set( JS::ObjectOrNullValue( defObj ) );
	bool retVal = InvokeEvent( "onSwing", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( decaying ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDecay, "onDecay" ))
		return RV_NOFUNC;

	JS::RootedValueArray<1> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_ITEM, decaying, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	bool retVal = InvokeEvent( "onDecay", 1, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnDecay, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnReleasePet()
//|	Purpose		-	Script trigger for when a player tries to release a pet
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnReleasePet( CChar *owner, CChar *pet )
{
	if( !ValidateObject( owner ) || !ValidateObject( pet ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnReleasePet, "onReleasePet" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject ownerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, owner, runTime ) );
	JS::RootedObject petObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pet, runTime ) );
	params[0].set( JS::ObjectOrNullValue( ownerObj ) );
	params[1].set( JS::ObjectOrNullValue( petObj ) );
	bool retVal = InvokeEvent( "onReleasePet", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnReleasePet, false );
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
	if( !ValidateObject( left ) || !ValidateObject( leaving ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnLeaving, "onLeaving" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	JS::RootedObject myItem( targContext, JSEngine->AcquireObject( IUE_ITEM, left, runTime ) );
	if( leaving->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, leaving, runTime );
		params[2].set( JS::Int32Value( 0 ) );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, leaving, runTime );
		params[2].set( JS::Int32Value( 1 ) );
	}
	params[0].set( JS::ObjectOrNullValue( myItem ) );
	params[1].set( JS::ObjectOrNullValue( myObj ) );
	bool retVal = InvokeEvent( "onLeaving", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( iMulti ) || !ValidateObject( cPlayer ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnMultiLogout, "onMultiLogout" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myMulti( targContext, JSEngine->AcquireObject( IUE_ITEM, iMulti, runTime ) );
	JS::RootedObject myPlayer( targContext, JSEngine->AcquireObject( IUE_CHAR, cPlayer, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myMulti ) );
	params[1].set( JS::ObjectOrNullValue( myPlayer ) );
	bool retVal = InvokeEvent( "onMultiLogout", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnMultiLogout, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|    Function    -    cScript::OnBoatTurn()
//o------------------------------------------------------------------------------------------------o
//|    Purpose        -    Triggers for Boat after it has turned successfully
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnBoatTurn( CBoatObj *iBoat, UI08 oldDir, UI08 newDir, CItem *iTiller )
{
	if( !ValidateObject( iBoat ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBoatTurn, "onBoatTurn" ))
		return RV_NOFUNC;

	JS::RootedValueArray<4> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myBoat( targContext, JSEngine->AcquireObject( IUE_ITEM, iBoat, runTime ) );
	JS::RootedObject myTiller( targContext, JSEngine->AcquireObject( IUE_ITEM, iTiller, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myBoat ) );
	params[1].set( JS::Int32Value( oldDir ) );
	params[2].set( JS::Int32Value( newDir ) );
	params[3].set( JS::ObjectOrNullValue( myTiller ) );
	bool retVal = InvokeEvent( "onBoatTurn", 4, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnBoatTurn, false );
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
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEquipAttempt, "onEquipAttempt" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, equipper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, equipping, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onEquipAttempt", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEquip, "onEquip" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, equipper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, equipping, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onEquip", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUnequipAttempt, "onUnequipAttempt" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, equipper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, equipping, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onUnequipAttempt", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUnequip, "onUnequip" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, equipper, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, equipping, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onUnequip", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( user ) || !ValidateObject( iUsing ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUseChecked, "onUseChecked" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, user, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onUseChecked", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( user ) || !ValidateObject( iUsing ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUseUnChecked, "onUseUnChecked" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, user, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onUseUnChecked", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( srcChar ) || !ValidateObject( dstChar ) || !ValidateObject( item ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDropItemOnNpc, "onDropItemOnNpc" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject srcObj( targContext, JSEngine->AcquireObject( IUE_CHAR, srcChar, runTime ) );
	JS::RootedObject dstObj( targContext, JSEngine->AcquireObject( IUE_CHAR, dstChar, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, item, runTime ) );
	params[0].set( JS::ObjectOrNullValue( srcObj ) );
	params[1].set( JS::ObjectOrNullValue( dstObj ) );
	params[2].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onDropItemOnNpc", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( left ) || !ValidateObject( leaving ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEntrance, "onEntrance" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	JS::RootedObject myItem( targContext, JSEngine->AcquireObject( IUE_ITEM, left, runTime ) );
	if( leaving->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, leaving, runTime );
		params[2].set( JS::Int32Value( 0 ) );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, leaving, runTime );
		params[2].set( JS::Int32Value( 1 ) );
	}
	params[0].set( JS::ObjectOrNullValue( myItem ) );
	params[1].set( JS::ObjectOrNullValue( myObj ) );
	bool retVal = InvokeEvent( "onEntrance", 3, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	if( srcObj->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, srcObj, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, srcObj, runTime );
	}

	JS::RootedObject myObj2( targContext );
	if( objVanish->GetObjType() == OT_CHAR )
	{
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objVanish, runTime );
	}
	else
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objVanish, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( myObj2 ) );
	bool retVal = InvokeEvent( "outOfRange", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOutOfRange, false );
	}

	return ( retVal == true );
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject sockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pPlayer, runTime ) );
	params[0].set( JS::ObjectOrNullValue( sockObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onLogin", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnLogin, false );
	}

	return ( retVal == true );
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject sockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pPlayer, runTime ) );
	params[0].set( JS::ObjectOrNullValue( sockObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onLogout", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnLogout, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnClick()
//|	Date		-	10/06/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when a player single-clicks on it
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnClick( CSocket *sockPlayer, CBaseObject *objClicked )
{
	if( !ValidateObject( objClicked ))
		return false;

	if( !ExistAndVerify( seOnClick, "onClick" ))
		return RV_NOFUNC;

	JS::RootedObject sockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime ) );
	JS::RootedObject myObj( targContext );
	if( objClicked->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, objClicked, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, objClicked, runTime );
	}

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( sockObj ) );
	params[1].set( JS::ObjectOrNullValue( myObj ) );
	bool retVal = InvokeEvent( "onClick", 2, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pFall, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( fallDistance ) );
	bool retVal = InvokeEvent( "onFall", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnFall, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnAISliver()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers on every AI loop for character with event attached
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnAISliver( CChar *pSliver )
{
	if( !ValidateObject( pSliver ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnAISliver, "onAISliver" ))
		return RV_NOFUNC;

	JS::RootedValueArray<1> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pSliver, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onAISliver", 1, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValue rval( targContext );
	bool retVal = InvokeEvent( "onSystemSlice", 0, nullptr, rval.address() );
	if( !retVal )
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
	if( !ValidateObject( tObject ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnLightChange, "onLightChange" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext );
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( lightLevel ) );
	bool retVal = InvokeEvent( "onLightChange", 2, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext );
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( element ) );
	bool retVal = InvokeEvent( "onWeatherChange", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnWeatherChange, false );
	}

	return ( retVal == true );
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext );
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( temp ) );
	bool retVal = InvokeEvent( "onTempChange", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnTempChange, false );
	}

	return ( retVal == true );
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext );
	if( tObject->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( timerId ) );
	bool retVal = InvokeEvent( "onTimer", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnTimer, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   cScript::OnTempEffectExpire()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Calls the associated script when a script-owned temporary effect reaches its
//|                 expiration time. The source can be null if it no longer exists, while target is
//|                 the character or item carrying the effect and effectId identifies the script's
//|                 specific effect.
//o------------------------------------------------------------------------------------------------o
bool cScript::OnTempEffectExpire( CBaseObject *source, CBaseObject *target, UI16 effectId )
{
	if( !ValidateObject( target ))
		return false;
	if( !ExistAndVerify( seOnTempEffectExpire, "onTempEffectExpire" ))
		return false;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	params[0].set( JS::NullValue() );
	if( ValidateObject( source ))
	{
		JS::RootedObject sourceObj( targContext, JSEngine->AcquireObject(( source->GetObjType() == OT_CHAR ? IUE_CHAR : IUE_ITEM ), source, runTime ) );
		params[0].set( JS::ObjectOrNullValue( sourceObj ) );
	}
	JS::RootedObject targetObj( targContext, JSEngine->AcquireObject(( target->GetObjType() == OT_CHAR ? IUE_CHAR : IUE_ITEM ), target, runTime ) );
	params[1].set( JS::ObjectOrNullValue( targetObj ) );
	params[2].set( JS::Int32Value( effectId ) );
	bool retVal = InvokeEvent( "onTempEffectExpire", 3, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnTempEffectExpire, false );
	}
	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   cScript::OnTempEffectRemove()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Calls the associated script when a script-owned temporary effect is manually
//|                 removed or replaced before expiration. The source can be null if it no longer
//|                 exists, while target and effectId identify the affected object and effect.
//o------------------------------------------------------------------------------------------------o
bool cScript::OnTempEffectRemove( CBaseObject *source, CBaseObject *target, UI16 effectId )
{
	if( !ValidateObject( target ))
		return false;
	if( !ExistAndVerify( seOnTempEffectRemove, "onTempEffectRemove" ))
		return false;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	params[0].set( JS::NullValue() );
	if( ValidateObject( source ))
	{
		JS::RootedObject sourceObj( targContext, JSEngine->AcquireObject(( source->GetObjType() == OT_CHAR ? IUE_CHAR : IUE_ITEM ), source, runTime ) );
		params[0].set( JS::ObjectOrNullValue( sourceObj ) );
	}
	JS::RootedObject targetObj( targContext, JSEngine->AcquireObject(( target->GetObjType() == OT_CHAR ? IUE_CHAR : IUE_ITEM ), target, runTime ) );
	params[1].set( JS::ObjectOrNullValue( targetObj ) );
	params[2].set( JS::Int32Value( effectId ) );
	bool retVal = InvokeEvent( "onTempEffectRemove", 3, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnTempEffectRemove, false );
	}
	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnStatLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when losing stats
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnStatLoss( CChar *player, UI32 stat, UI32 statLossAmount )
{
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnStatLoss, "onStatLoss" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( stat ) );
	params[2].set( JS::Int32Value( statLossAmount ) );
	bool retVal = InvokeEvent( "onStatLoss", 3, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( stat ) );
	params[2].set( JS::Int32Value( statChangeAmount ) );
	bool retVal = InvokeEvent( "onStatChange", 3, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnStatChange, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSkillLoss()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when losing skillpoints
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillLoss( CChar *player, SI08 skill, UI32 skillLossAmount )
{
	if( !ValidateObject( player ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillLoss, "onSkillLoss" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( skill ) );
	params[2].set( JS::Int32Value( skillLossAmount ) );
	bool retVal = InvokeEvent( "onSkillLoss", 3, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( skill ) );
	params[2].set( JS::Int32Value( skillChangeAmount ) );
	bool retVal = InvokeEvent( "onSkillChange", 3, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnSkillChange, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDeath()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached after dying
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDeath( CChar *pDead, CItem *iCorpse )
{
	if( !ValidateObject( pDead ) || !ValidateObject( iCorpse ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDeath, "onDeath" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pDead, runTime ) );
	JS::RootedObject corpseObj( targContext, JSEngine->AcquireObject( IUE_ITEM, iCorpse, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( corpseObj ) );
	bool retVal = InvokeEvent( "onDeath", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( pAlive ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnResurrect, "onResurrect" ))
		return RV_NOFUNC;

	JS::RootedValueArray<1> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pAlive, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onResurrect", 1, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnFlagChange, "onFlagChange" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( newStatus ) );
	params[2].set( JS::Int32Value( oldStatus ) );
	bool retVal = InvokeEvent( "onFlagChange", 3, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	SI32 objType		= 2;	// 2 == null, 1 == char, 0 == item
	CBaseObject *mObj	= nullptr;
	JS::RootedObject myObj2( targContext );
	try
	{
		JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
		if( myObj == nullptr )
			return false;

		params[0].set( JS::ObjectOrNullValue( myObj ) );
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
			params[1].set( JS::NullValue() );
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
			params[1].set( JS::ObjectOrNullValue( myObj2 ) );
		}

		bool retVal = InvokeEvent( oldstrutil::format( "onCallback%i", callNum ).c_str(), 2, params.begin(), rval.address() );
		return ( retVal == true );
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

UI16 cScript::GetScriptID( void ) const
{
	return scriptID;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnLoyaltyChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPC character with event attached when loyalty level changes
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnLoyaltyChange( CChar *pChanging, SI08 newStatus )
{
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnLoyaltyChange, "onLoyaltyChange" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( newStatus ) );
	bool retVal = InvokeEvent( "onLoyaltyChange", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnHungerChange, "onHungerChange" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( newStatus ) );
	bool retVal = InvokeEvent( "onHungerChange", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnThirstChange, "onThirstChange" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime ));
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( newStatus ) );
	bool retVal = InvokeEvent( "onThirstChange", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( stealing ) || !ValidateObject( stolenFrom ) || !ValidateObject( stolen ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnStolenFrom, "onStolenFrom" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject thiefObj( targContext, JSEngine->AcquireObject( IUE_CHAR, stealing, runTime ) );
	JS::RootedObject victimObj( targContext, JSEngine->AcquireObject( IUE_CHAR, stolenFrom, runTime ) );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, stolen, runTime ) );
	params[0].set( JS::ObjectOrNullValue( thiefObj ) );
	params[1].set( JS::ObjectOrNullValue( victimObj ) );
	params[2].set( JS::ObjectOrNullValue( itemObj ) );
	bool retVal = InvokeEvent( "onStolenFrom", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSnooped, "onSnooped" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject thiefObj( targContext, JSEngine->AcquireObject( IUE_CHAR, snooped, runTime ) );
	JS::RootedObject victimObj( targContext, JSEngine->AcquireObject( IUE_CHAR, snooper, runTime ) );
	params[0].set( JS::ObjectOrNullValue( thiefObj ) );
	params[1].set( JS::ObjectOrNullValue( victimObj ) );
	params[2].set( JS::BooleanValue( success ) );
	bool retVal = InvokeEvent( "onSnooped", 3, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( snooped ) || !ValidateObject( pack ) || !ValidateObject( snooper ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSnoopAttempt, "onSnoopAttempt" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject thiefObj( targContext, JSEngine->AcquireObject( IUE_CHAR, snooped, runTime ) );
	JS::RootedObject packObj( targContext, JSEngine->AcquireObject( IUE_ITEM, pack, runTime ) );
	JS::RootedObject victimObj( targContext, JSEngine->AcquireObject( IUE_CHAR, snooper, runTime ) );
	params[0].set( JS::ObjectOrNullValue( thiefObj ) );
	params[1].set( JS::ObjectOrNullValue( packObj ) );
	params[2].set( JS::ObjectOrNullValue( victimObj ) );
	bool retVal = InvokeEvent( "onSnoopAttempt", 3, params.begin(), rval.address() );
	if( !retVal )
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

	SEGumpData_st *segdGumpData = new SEGumpData_st;
	JS::RootedObject jsoObject( targContext, JS_NewObject( targContext, &UOXGumpData_class ) );
	if( jsoObject == nullptr ||
		!JS_DefineFunctions( targContext, jsoObject, CGumpData_Methods ) ||
		!JS_DefineProperties( targContext, jsoObject, CGumpDataProperties ) )
	{
		delete segdGumpData;
		return;
	}
	JS_SetReservedSlot( jsoObject, 0, JS::PrivateValue( segdGumpData ) );

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
	JS::RootedValueArray<3> jsvParams( targContext );
	JS::RootedValue jsvRVal( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, pressing, runTime ) );
	jsvParams[0].set( JS::ObjectOrNullValue( myObj ) );
	jsvParams[1].set( JS::Int32Value( button ) );
	jsvParams[2].set( JS::ObjectOrNullValue( jsoObject ) );
	[[maybe_unused]] bool retVal = InvokeEvent( "onGumpPress", 3, jsvParams.begin(), jsvRVal.address() );
	JS_SetReservedSlot( jsoObject, 0, JS::UndefinedValue() );
	delete segdGumpData;
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

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, pressing->GetSocket(), runTime ) );
	JS::RootedString gumpReply( targContext, JS_NewStringCopyZ( targContext, pressing->Reply().c_str() ));
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( pressing->Index() ) );
	params[2].set( JS::StringValue( gumpReply ) );
	[[maybe_unused]] bool retVal = InvokeEvent( "onGumpInput", 3, params.begin(), rval.address() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnScrollingGumpPress()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when clicking the old school horizontally scrolling gump
//|					if ID of this gump is 0
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnScrollingGumpPress( CSocket *tSock, UI16 gumpId, UI16 buttonId )
{
	if( tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnScrollingGumpPress, "onScrollingGumpPress" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( gumpId ) );
	params[2].set( JS::Int32Value( buttonId ) );
	bool retVal = InvokeEvent( "onScrollingGumpPress", 3, params.begin(), rval.address() );
	if( !retVal )
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, entering, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( region ) );
	bool retVal = InvokeEvent( "onEnterRegion", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnEnterRegion, false );
	}
	return ( retVal == true );
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

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, leaving, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( region ) );
	bool retVal = InvokeEvent( "onLeaveRegion", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnLeaveRegion, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnFacetChange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when switching to a different facet
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnFacetChange( CChar *mChar, const UI08 oldFacet, const UI08 newFacet )
{
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnFacetChange, "onFacetChange" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( oldFacet ) );
	params[2].set( JS::Int32Value( newFacet ) );
	bool retVal = InvokeEvent( "onFacetChange", 3, params.begin(), rval.address() );

	if( !retVal )
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
SI32 cScript::OnSpellTargetSelect(  CChar *caster, CBaseObject *target, SI32 spellNum )
{
	if( !ValidateObject( target ) || !ValidateObject( caster ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellTargetSelect, "onSpellTargetSelect" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject castObj( targContext, JSEngine->AcquireObject( IUE_CHAR, caster, runTime ) );
	JS::RootedObject targObj( targContext );
	if( target->CanBeObjType( OT_CHAR ))
	{
		targObj = JSEngine->AcquireObject( IUE_CHAR, target, runTime );
	}
	else
	{
		targObj = JSEngine->AcquireObject( IUE_ITEM, target, runTime );
	}
	params[0].set( JS::ObjectOrNullValue( targObj ) );
	params[1].set( JS::ObjectOrNullValue( castObj ) );
	params[2].set( JS::Int32Value( spellNum ) );
	bool retVal = InvokeEvent( "onSpellTargetSelect", 3, params.begin(), rval.address() );
	if( !retVal )
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
SI32 cScript::OnSpellTarget( CBaseObject *target, CChar *caster, SI32 spellNum )
{
	if( !ValidateObject( target ) || !ValidateObject( caster ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellTarget, "onSpellTarget" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject castObj( targContext, JSEngine->AcquireObject( IUE_CHAR, caster, runTime ) );
	JS::RootedObject targObj( targContext );
	if( target->CanBeObjType( OT_CHAR ))
	{
		targObj = JSEngine->AcquireObject( IUE_CHAR, target, runTime );
	}
	else
	{
		targObj = JSEngine->AcquireObject( IUE_ITEM, target, runTime );
	}
	params[0].set( JS::ObjectOrNullValue( targObj ) );
	params[1].set( JS::ObjectOrNullValue( castObj ) );
	params[2].set( JS::Int32Value( spellNum ) );
	bool retVal = InvokeEvent( "onSpellTarget", 3, params.begin(), rval.address() );
	if( !retVal )
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
bool cScript::CallParticularEvent( const char *eventToCall, const JS::Value *params, SI32 numParams, JS::Value *eventRetVal )
{
	if( eventToCall == nullptr )
		return false;

	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	bool retVal = InvokeEvent( eventToCall, numParams, params, eventRetVal );

	if( !retVal )
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
SI32 cScript::OnSpellCast( CChar *tChar, SI32 SpellId )
{
	if( !ValidateObject( tChar ))
		return -2;

	if( !ExistAndVerify( seOnSpellCast, "onSpellCast" ))
		return -2;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, tChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( SpellId ) );
	bool retVal = InvokeEvent( "onSpellCast", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnSpellCast, false );
		return -2;
	}

	return static_cast<SI16>( rval.toInt32() );
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
SI32 cScript::OnScrollCast( CChar *tChar, SI32 SpellId )
{
	if( !ValidateObject( tChar ))
		return -2;

	if( !ExistAndVerify( seOnScrollCast, "onScrollCast" ))
		return -2;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, tChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( SpellId ) );
	bool retVal = InvokeEvent( "onScrollCast", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnScrollCast, false );
		return -2;
	}

	return static_cast<SI16>( rval.toInt32() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellSuccess()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers after character with event attached successfully casts a spell
//o------------------------------------------------------------------------------------------------o
SI32 cScript::OnSpellSuccess( CChar *tChar, SI32 SpellId )
{
	if( !ValidateObject( tChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellSuccess, "onSpellSuccess" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, tChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( SpellId ) );
	bool retVal = InvokeEvent( "onSpellSuccess", 2, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( myChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnTalk, "onTalk" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	std::string lwrSpeech	= mySpeech;
	JS::RootedString strSpeech( targContext, JS_NewStringCopyZ( targContext, oldstrutil::lower( lwrSpeech ).c_str() ));

	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, myChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::StringValue( strSpeech ) );
	bool retVal = InvokeEvent( "onTalk", 2, params.begin(), rval.address() );

	if( !retVal )
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

	JS::RootedValueArray<4> params( targContext );
	JS::RootedValue rval( targContext );
	char *lwrSpeech = new char[strlen( mySpeech ) + 1];
	strcopy( lwrSpeech, strlen( mySpeech ) + 1, mySpeech );
	JS::RootedString strSpeech( targContext, JS_NewStringCopyZ( targContext, lwrSpeech ));
	delete[] lwrSpeech;

	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, myChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	if( !ValidateObject( myItem ))
	{
		params[1].set( JS::NullValue() );
	}
	else
	{
		JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, myItem, runTime ) );
		params[1].set( JS::ObjectOrNullValue( itemObj ) );
	}

	params[2].set( JS::StringValue( strSpeech ) );
	params[3].set( JS::Int32Value( myChar->GetSpeechId() ) );
	bool retVal = InvokeEvent( "onSpeechInput", 4, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnSpeechInput, false );
		return true;
	}

	return ( rval.toBoolean() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnSpellGain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are added to them
//o------------------------------------------------------------------------------------------------o
SI32 cScript::OnSpellGain( CItem *book, const SI32 spellNum )
{
	if( !ValidateObject( book ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellGain, "onSpellGain" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, book, runTime ) );
	params[0].set( JS::ObjectOrNullValue( itemObj ) );
	params[1].set( JS::Int32Value( spellNum ) );
	bool retVal = InvokeEvent( "onSpellGain", 2, params.begin(), rval.address() );
	if( !retVal )
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
SI32 cScript::OnSpellLoss( CItem *book, const SI32 spellNum )
{
	if( !ValidateObject( book ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellLoss, "onSpellLoss" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject itemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, book, runTime ) );
	params[0].set( JS::ObjectOrNullValue( itemObj ) );
	params[1].set( JS::Int32Value( spellNum ) );
	bool retVal = InvokeEvent( "onSpellLoss", 2, params.begin(), rval.address() );
	if( !retVal )
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
SI08 cScript::OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill, bool isCraftSkill, SI08 overrideOutcome )
{
	if( !ValidateObject( myChar ) || skill > ALLSKILLS )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillCheck, "onSkillCheck" ))
		return RV_NOFUNC;

	JS::RootedValueArray<6> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, myChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( skill ) );
	params[2].set( JS::Int32Value( lowSkill ) );
	params[3].set( JS::Int32Value( highSkill ) );
	params[4].set( JS::BooleanValue( isCraftSkill ) );
	params[5].set( JS::Int32Value( overrideOutcome ) );
    bool retVal = InvokeEvent( "onSkillCheck", 6, params.begin(), rval.address() );
	if( !retVal )
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
bool cScript::AreaObjFunc( const char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s )
{
	if( !ValidateObject( srcObject ) || !ValidateObject( tmpObject ) || funcName == nullptr )
		return false;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject srcObj( targContext );
	JS::RootedObject tmpObj( targContext );
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

	params[0].set( JS::ObjectOrNullValue( srcObj ) );
	params[1].set( JS::ObjectOrNullValue( tmpObj ) );
	if( s != nullptr )
	{
		JS::RootedObject sockObj( targContext );
		sockObj		= JSEngine->AcquireObject( IUE_SOCK, s, runTime );
		params[2].set( JS::ObjectOrNullValue( sockObj ) );
	}
	else
	{
		params[2].set( JS::NullValue() );
	}
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.

	//FIXME === do we need this retvalue?
	try
	{
		[[maybe_unused]] bool retVal = InvokeEvent( funcName, 3, params.begin(), rval.address() );
	}
	catch( ... )
	{
		Console.Error( "Some error!" );
	}

	return ( rval.toBoolean() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Quick and dirty way to setup custom commands
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCommand( CSocket *mSock, std::string command )
{
	if( mSock  == nullptr || command == "" )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCommand, "onCommand" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	JS::RootedString strCmd( targContext, JS_NewStringCopyZ( targContext, oldstrutil::lower( command ).c_str() ));
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::StringValue( strCmd ) );
	bool retVal	= InvokeEvent( "onCommand", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnCommand, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnProfileRequest()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers in global script on requests for paperdoll profiles of other players
//o------------------------------------------------------------------------------------------------o
std::string cScript::OnProfileRequest( CSocket *mSock, CChar *profileOwner )
{
	if( mSock  == nullptr || !ValidateObject( profileOwner ))
		return "";

	if( !ExistAndVerify( seOnProfileRequest, "onProfileRequest" ))
		return "";

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	JS::RootedObject profOwnerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, profileOwner, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( profOwnerObj ) );
	bool retVal	= InvokeEvent( "onProfileRequest", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnProfileRequest, false );
	}

	if( rval.isInt32() && rval.toInt32() < 0 )
	{
		Console.Error( "Handled exception in cScript.cpp OnProfileRequest() - invalid return value/error encountered!" );
		return "";
	}

	try
	{
		std::string returnString = JS_GetStringBytes( targContext, rval );

		return returnString;
	}
	catch( ... )
	{
		Console.Error( "Handled exception in cScript.cpp OnProfileRequest()" );
		return "";
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnProfileUpdate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers in global script on attempts to update paperdoll profile text
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnProfileUpdate( CSocket *mSock, std::string profileText )
{
	if( mSock == nullptr || profileText.empty() )
		return false;

	if( !ExistAndVerify( seOnProfileUpdate, "onProfileUpdate" ))
		return false;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	JS::RootedString strProfileText( targContext, JS_NewStringCopyZ( targContext, profileText.c_str() ));

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::StringValue( strProfileText ) );
	bool retVal	= InvokeEvent( "onProfileUpdate", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnProfileUpdate, false );
	}

	return ( retVal == true );
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
		JS::RootedValue Func( targContext, JS::NullValue() );
		JS_GetProperty( targContext, targObject, functionName.c_str(), &Func );
		if( Func == JS::UndefinedValue() )
		{
			SetEventExists( eventNum, false );
			return false;
		}
	}

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
	JS::RootedValue rval( targContext );
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.

	JS::RootedValue Func( targContext, JS::NullValue() );
	JS_GetProperty( targContext, targObject, scriptType.c_str(), &Func );
	if( Func == JS::UndefinedValue() )
	{
		Console.Warning( oldstrutil::format( "Script Number (%u) does not have a %s function", JSMapping->GetScriptId( targObject ), scriptType.c_str() ));
		return false;
	}

	bool retVal = InvokeEvent( scriptType.c_str(), 0, nullptr, rval.address() );
	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::executeCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute a player-initiated JS-based command
//o------------------------------------------------------------------------------------------------o
bool cScript::executeCommand( CSocket *s, std::string funcName, std::string executedString )
{
	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedString execString( targContext, JS_NewStringCopyZ( targContext, executedString.c_str() ));
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, s, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::StringValue( execString ) );
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	bool retVal = InvokeEvent( funcName.c_str(), 2, params.begin(), rval.address() );

	return ( retVal == true );
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

	JS::RootedValueArray<4> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, tChar, runTime ) );
	JS::RootedObject sockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	params[0].set( JS::ObjectOrNullValue( sockObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::BooleanValue( directCast ) );
	params[3].set( JS::Int32Value( spellNum ) );
	bool retVal = InvokeEvent( "onSpellCast", 4, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnSpellCast, false );
		return false;
	}

	return ( rval.toBoolean() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnIterate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called after IterateOver JS function is used, and iterates over all items or
//|					characters (as specified) in the game
//o------------------------------------------------------------------------------------------------o
bool cScript::OnIterate( CBaseObject *a, UI32 &b, CSocket *mSock )
{
	if( !ValidateObject( a ))
		return true;

	if( !ExistAndVerify( seOnIterate, "onIterate" ))
		return false;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	if( a->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, a, runTime );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, a, runTime );
	}

	JS::RootedObject sockObj( targContext );
	if( mSock )
	{
		sockObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( sockObj ) );
	bool retVal	= InvokeEvent( "onIterate", 2, params.begin(), rval.address() );

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
	if( !retVal )
	{
		SetEventExists( seOnIterate, false );
	}
	else if( rval.toBoolean())
	{
		++b;
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnIterateSpawnRegions()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called after IterateOverSpawnRegions JS function is used, and iterates over
//|					all spawn regions in game
//o------------------------------------------------------------------------------------------------o
bool cScript::OnIterateSpawnRegions( CSpawnRegion *a, UI32 &b, CSocket *mSock  )
{
	if( a == nullptr )
		return true;

	if( !ExistAndVerify( seOnIterateSpawnRegions, "onIterateSpawnRegions" ))
		return false;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject myObj( targContext );
	myObj = JSEngine->AcquireObject( IUE_SPAWNREGION, a, runTime );

	JS::RootedObject sockObj( targContext );
	if( mSock )
	{
		sockObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( sockObj ) );
	bool retVal = InvokeEvent( "onIterateSpawnRegions", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnIterateSpawnRegions, false );
	}
	else if( rval.toBoolean())
	{
		++b;
	}

	return ( retVal == true );
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

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::Int32Value( static_cast<UI08>( packetNum % 256 )) );
	params[2].set( JS::Int32Value( static_cast<UI08>( packetNum >> 8 )) );
	bool retVal	= InvokeEvent( "onPacketReceive", 3, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnPacketReceive, false );
	}

	return ( retVal == true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnPaperDoll()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding events that happen when open paperdoll on characters.
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnPaperDoll( CChar *currChar, CChar *targChar )
{
	if( !ValidateObject( currChar ) || !ValidateObject( targChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnPaperDoll, "onPaperDoll" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject srcObj( targContext, JSEngine->AcquireObject( IUE_CHAR, currChar, runTime ) );
	JS::RootedObject trgObj( targContext, JSEngine->AcquireObject( IUE_CHAR, targChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( srcObj ) );
	params[1].set( JS::ObjectOrNullValue( trgObj ) );
	bool retVal = InvokeEvent( "onPaperDoll", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnPaperDoll, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnCharDoubleClick()
//|	Date		-	23rd January, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding events that happen when doubleclicking characters, such as
//|					open paperdoll, mounting horses, etc
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnCharDoubleClick( CChar *currChar, CChar *targChar, bool nonMouseClickEvent )
{
	if( !ValidateObject( currChar ) || !ValidateObject( targChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCharDoubleClick, "onCharDoubleClick" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject srcObj( targContext, JSEngine->AcquireObject( IUE_CHAR, currChar, runTime ) );
	JS::RootedObject trgObj( targContext, JSEngine->AcquireObject( IUE_CHAR, targChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( srcObj ) );
	params[1].set( JS::ObjectOrNullValue( trgObj ) );
	params[2].set( JS::BooleanValue( nonMouseClickEvent ) );
	bool retVal = InvokeEvent( "onCharDoubleClick", 3, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnCharDoubleClick, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cScript::OnDismount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows triggering effects upon dismounting a mount
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnDismount( CChar *pChar, CChar *npcMount )
{
	if( !ValidateObject( pChar ) || !ValidateObject( npcMount ))
		return false;

	if( !ExistAndVerify( seOnDismount, "onDismount" ))
		return false;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject pObj( targContext, JSEngine->AcquireObject( IUE_CHAR, pChar, runTime ) );
	JS::RootedObject npcObj( targContext, JSEngine->AcquireObject( IUE_CHAR, npcMount, runTime ) );
	params[0].set( JS::ObjectOrNullValue( pObj ) );
	params[1].set( JS::ObjectOrNullValue( npcObj ) );
	bool retVal = InvokeEvent( "onDismount", 2, params.begin(), rval.address() );

	if( !retVal )
	{
		SetEventExists( seOnDismount, false );
	}

	return ( retVal == true );
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
	if( !ValidateObject( currChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSkillGump, "onSkillGump" ))
		return RV_NOFUNC;

	JS::RootedValueArray<1> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, currChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal	= InvokeEvent( "onSkillGump", 1, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( targChar ) || mSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnUseBandageMacro, "onUseBandageMacro" ))
		return RV_NOFUNC;

	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject mSockObj( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	JS::RootedObject targCharObj( targContext, JSEngine->AcquireObject( IUE_CHAR, targChar, runTime ) );
	JS::RootedObject bandageItemObj( targContext, JSEngine->AcquireObject( IUE_ITEM, bandageItem, runTime ) );
	params[0].set( JS::ObjectOrNullValue( mSockObj ) );
	params[1].set( JS::ObjectOrNullValue( targCharObj ) );
	params[2].set( JS::ObjectOrNullValue( bandageItemObj ) );
	bool retVal = InvokeEvent( "onUseBandageMacro", 3, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( attacker ) || !ValidateObject( target ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnAICombatTarget, "onAICombatTarget" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject attObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
	JS::RootedObject targObj( targContext, JSEngine->AcquireObject( IUE_CHAR, target, runTime ) );
	params[0].set( JS::ObjectOrNullValue( attObj ) );
	params[1].set( JS::ObjectOrNullValue( targObj ) );
	bool retVal = InvokeEvent( "onAICombatTarget", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCombatStart, "onCombatStart" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject attObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
	JS::RootedObject defObj( targContext, JSEngine->AcquireObject( IUE_CHAR, defender, runTime ) );
	params[0].set( JS::ObjectOrNullValue( attObj ) );
	params[1].set( JS::ObjectOrNullValue( defObj ) );
	bool retVal = InvokeEvent( "onCombatStart", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( currChar ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCombatEnd, "onCombatEnd" ))
		return RV_NOFUNC;

	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	JS::RootedObject attObj( targContext, JSEngine->AcquireObject( IUE_CHAR, currChar, runTime ) );
	JS::RootedObject defObj( targContext, JSEngine->AcquireObject( IUE_CHAR, targChar, runTime ) );
	params[0].set( JS::ObjectOrNullValue( attObj ) );
	params[1].set( JS::ObjectOrNullValue( defObj ) );
	bool retVal = InvokeEvent( "onCombatEnd", 2, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( mKilled )) // || !ValidateObject( mKiller ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDeathBlow, "onDeathBlow" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject killedObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mKilled, runTime ) );
	JS::RootedObject killerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mKiller, runTime ) );
	params[0].set( JS::ObjectOrNullValue( killedObj ) );
	params[1].set( JS::ObjectOrNullValue( killerObj ) );
	bool retVal = InvokeEvent( "onDeathBlow", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnDeathBlow, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   cScript::OnKill()
//| Date        -   30th November, 2025
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Triggers for characters with event attached when they kill another character
//| Notes       -   This is called after onDeathBlow on the victim has allowed death to proceed
//|                 Return value is currently ignored by the core, but kept for consistency
//o------------------------------------------------------------------------------------------------o
SI08 cScript::OnKill( CChar *mKiller, CChar *mKilled )
{
	if( !ValidateObject( mKiller )) // || !ValidateObject( mKilled ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnKill, "onKill" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject killerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mKiller, runTime ) );
	JS::RootedObject killedObj( targContext, JSEngine->AcquireObject( IUE_CHAR, mKilled, runTime ) );
	// JS: function onKill( killer, killed )
	params[0].set( JS::ObjectOrNullValue( killerObj ) );
	params[1].set( JS::ObjectOrNullValue( killedObj ) );
	bool retVal = InvokeEvent( "onKill", 2, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnKill, false );
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
	if( !ValidateObject( attacker ) || !ValidateObject( defender ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCombatDamageCalc, "onCombatDamageCalc" ))
		return RV_NOFUNC;

	SI16 funcRetVal	= -1;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject attackerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
	JS::RootedObject defenderObj( targContext, JSEngine->AcquireObject( IUE_CHAR, defender, runTime ) );
	params[0].set( JS::ObjectOrNullValue( attackerObj ) );
	params[1].set( JS::ObjectOrNullValue( defenderObj ) );
	params[2].set( JS::Int32Value( getFightSkill ) );
	params[3].set( JS::Int32Value( hitLoc ) );
	bool retVal = InvokeEvent( "onCombatDamageCalc", 4, params.begin(), rval.address() );
	if( !retVal )
	{
		SetEventExists( seOnCombatDamageCalc, false );
		return RV_NOFUNC;
	}
	if( rval.isInt32() )
	{
		return static_cast<SI16>( rval.toInt32() );
	}
	else if( rval.isDouble() )
	{
		return static_cast<SI16>( static_cast<SI32>( rval.toDouble() ));
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
	if( !ValidateObject( damaged ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDamage, "onDamage" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject damagedObj( targContext, JSEngine->AcquireObject( IUE_CHAR, damaged, runTime ) );
	params[0].set( JS::ObjectOrNullValue( damagedObj ) );
	if( ValidateObject( attacker ))
	{
		JS::RootedObject attackerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
		params[1].set( JS::ObjectOrNullValue( attackerObj ) );
	}
	else
	{
		params[1].set( JS::NullValue() );
	}

	params[2].set( JS::Int32Value( damageValue ) );
	params[3].set( JS::Int32Value( damageType ) );
	bool retVal = InvokeEvent( "onDamage", 4, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( damaged ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDamageDeal, "onDamageDeal" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject attackerObj( targContext, JSEngine->AcquireObject( IUE_CHAR, attacker, runTime ) );
	params[0].set( JS::ObjectOrNullValue( attackerObj ) );
	JS::RootedObject damagedObj( targContext, JSEngine->AcquireObject( IUE_CHAR, damaged, runTime ) );
	params[1].set( JS::ObjectOrNullValue( damagedObj ) );
	params[2].set( JS::Int32Value( damageValue ) );
	params[3].set( JS::Int32Value( damageType ) );
	bool retVal = InvokeEvent( "onDamageDeal", 4, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( objVendor ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBuy, "onBuy" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onBuy", 2, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objVendor ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSell, "onSell" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	bool retVal = InvokeEvent( "onSell", 2, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == nullptr || numItemsBuying == 0 )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBuyFromVendor, "onBuyFromVendor" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime ) );
	JS::RootedObject myObj2( targContext );
	if( objItemBought->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( myObj2 ) );
	params[3].set( JS::Int32Value( numItemsBuying ) );
	bool retVal = InvokeEvent( "onBuyFromVendor", 4, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == nullptr || numItemsSelling == 0 )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSellToVendor, "onSellToVendor" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime ) );
	JS::RootedObject myObj2( targContext );
	if( objItemSold->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( myObj2 ) );
	params[3].set( JS::Int32Value( numItemsSelling ) );
	bool retVal = InvokeEvent( "onSellToVendor", 4, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnBoughtFromVendor, "onBoughtFromVendor" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime ) );
	JS::RootedObject myObj2( targContext );
	if( objItemBought->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( myObj2 ) );
	params[3].set( JS::Int32Value( numItemsBought ) );
	bool retVal = InvokeEvent( "onBoughtFromVendor", 4, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSoldToVendor, "onSoldToVendor" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime ) );
	JS::RootedObject myObj2( targContext );
	if( objItemSold->GetObjType() == OT_ITEM )
	{
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );
	}

	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( charObj ) );
	params[2].set( JS::ObjectOrNullValue( myObj2 ) );
	params[3].set( JS::Int32Value( numItemsSold ) );
	bool retVal = InvokeEvent( "onSoldToVendor", 4, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objMulti ) || tSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnHouseCommand, "onHouseCommand" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedObject myObj( targContext, JSEngine->AcquireObject( IUE_SOCK, tSock, runTime ) );
	JS::RootedObject multiObj( targContext, JSEngine->AcquireObject( IUE_ITEM, objMulti, runTime ) );
	params[0].set( JS::ObjectOrNullValue( myObj ) );
	params[1].set( JS::ObjectOrNullValue( multiObj ) );
	params[2].set( JS::Int32Value( cmdId ) );
	bool retVal = InvokeEvent( "onHouseCommand", 3, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( objChar ) || mSock == nullptr )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnMakeItem, "onMakeItem" ))
		return RV_NOFUNC;

	JS::RootedValue rval( targContext );
	JS::RootedValueArray<4> params( targContext );
	JS::RootedObject mySock( targContext, JSEngine->AcquireObject( IUE_SOCK, mSock, runTime ) );
	JS::RootedObject myChar( targContext, JSEngine->AcquireObject( IUE_CHAR, objChar, runTime ) );
	JS::RootedObject myItem( targContext, JSEngine->AcquireObject( IUE_ITEM, objItem, runTime ) );
	params[0].set( JS::ObjectOrNullValue( mySock ) );
	params[1].set( JS::ObjectOrNullValue( myChar ) );
	params[2].set( JS::ObjectOrNullValue( myItem ) );
	params[3].set( JS::Int32Value( createEntryId ) );
	bool retVal = InvokeEvent( "onMakeItem", 4, params.begin(), rval.address() );

	if( !retVal )
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
	if( !ValidateObject( npc ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnPathfindEnd, "onPathfindEnd" ))
		return RV_NOFUNC;

	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, npc, runTime ) );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::Int32Value( pathfindResult ) );
	bool retVal = InvokeEvent( "onPathfindEnd", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( npc ) || !ValidateObject( enemy ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEnterEvadeState, "onEnterEvadeState" ))
		return RV_NOFUNC;

	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, npc, runTime ) );
	JS::RootedObject enemyObj( targContext, JSEngine->AcquireObject( IUE_CHAR, enemy, runTime ) );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( enemyObj ) );
	bool retVal = InvokeEvent( "onEnterEvadeState", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( player ) || !ValidateObject( corpse ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnCarveCorpse, "onCarveCorpse" ))
		return RV_NOFUNC;

	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	JS::RootedObject corpseObj( targContext, JSEngine->AcquireObject( IUE_ITEM, corpse, runTime ) );
	JS::RootedValueArray<2> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( corpseObj ) );
	bool retVal = InvokeEvent( "onCarveCorpse", 2, params.begin(), rval.address() );
	if( !retVal )
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
	if( !ValidateObject( player ) || !ValidateObject( dyeTub ) || !ValidateObject( target ))
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnDyeTarget, "onDyeTarget" ))
		return RV_NOFUNC;

	JS::RootedObject charObj( targContext, JSEngine->AcquireObject( IUE_CHAR, player, runTime ) );
	JS::RootedObject dyeTubObj( targContext, JSEngine->AcquireObject( IUE_ITEM, dyeTub, runTime ) );
	JS::RootedObject targObj( targContext, JSEngine->AcquireObject( IUE_ITEM, target, runTime ) );
	JS::RootedValueArray<3> params( targContext );
	JS::RootedValue rval( targContext );
	params[0].set( JS::ObjectOrNullValue( charObj ) );
	params[1].set( JS::ObjectOrNullValue( dyeTubObj ) );
	params[2].set( JS::ObjectOrNullValue( targObj ) );
	bool retVal = InvokeEvent( "onDyeTarget", 3, params.begin(), rval.address() );
	if( !retVal )
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
