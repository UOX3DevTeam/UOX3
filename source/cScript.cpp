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


//o-----------------------------------------------------------------------------------------------o
//|	File		-	cScript.cpp
//|	Date		-	August 26th, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles JS events
//o-----------------------------------------------------------------------------------------------o
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
//o-----------------------------------------------------------------------------------------------o

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
	{ "GetHour",					SE_GetHour,					0, 0, 0 },
	{ "GetMinute",					SE_GetMinute,				0, 0, 0 },
	{ "GetDay",						SE_GetDay,					0, 0, 0 },
	{ "GetSecondsPerUOMinute",		SE_SecondsPerUOMinute,		0, 0, 0 },
	{ "GetCurrentClock",			SE_GetCurrentClock,			0, 0, 0 },
	{ "GetMurderThreshold",			SE_GetMurderThreshold,		0, 0, 0 },
	{ "RollDice",					SE_RollDice,				3, 0, 0 },
	{ "RaceCompareByRace",			SE_RaceCompareByRace,		2, 0, 0 },
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
	{ "SpawnNPC",					SE_SpawnNPC,				5, 0, 0 },
	{ "GetPackOwner",				SE_GetPackOwner,			2, 0, 0 },
	{ "FindRootContainer",			SE_FindRootContainer,		2, 0, 0 },
	{ "CalcTargetedItem",			SE_CalcTargetedItem,		1, 0, 0 },
	{ "CalcTargetedChar",			SE_CalcTargetedChar,		1, 0, 0 },
	{ "GetTileIDAtMapCoord",		SE_GetTileIDAtMapCoord,		3, 0, 0 },
	{ "GetDictionaryEntry",			SE_GetDictionaryEntry,		2, 0, 0 },
	{ "Yell",						SE_Yell,					3, 0, 0 },
	{ "GetRaceCount",				SE_GetRaceCount,			0, 0, 0 },
	{ "WorldBrightLevel",			SE_WorldBrightLevel,		0, 0, 0 },
	{ "WorldDarkLevel",				SE_WorldDarkLevel,			0, 0, 0 },
	{ "WorldDungeonLevel",			SE_WorldDungeonLevel,		0, 0, 0 },
	{ "AreaCharacterFunction",		SE_AreaCharacterFunction,	3, 0, 0 },
	{ "AreaItemFunction",			SE_AreaItemFunction,		3, 0, 0 },
	{ "TriggerEvent",				SE_TriggerEvent,			3, 0, 0 },
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
	{ "DoesDynamicBlock",			SE_DoesDynamicBlock,		7, 0, 0 },
	{ "DoesStaticBlock",			SE_DoesStaticBlock,			7, 0, 0 },
	{ "DoesMapBlock",				SE_DoesMapBlock,			8, 0, 0 },
	{ "DistanceBetween",			SE_DistanceBetween,			4, 0, 0 },

	{ "ResourceArea",				SE_ResourceArea,			2, 0, 0 },
	{ "ResourceAmount",				SE_ResourceAmount,			2, 0, 0 },
	{ "ResourceTime",				SE_ResourceTime,			2, 0, 0 },
	{ "ResourceRegion",				SE_ResourceRegion,			3, 0, 0 },
	{ "Moon",						SE_Moon,					2, 0, 0 },

	{ "GetTownRegion",				SE_GetTownRegion,			1, 0, 0 },
	{ "GetSpawnRegion",				SE_GetSpawnRegion,			1, 0, 0 },
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

	{ "CreateParty",				SE_CreateParty,				1, 0, 0 },

	{ "GetClientFeature",			SE_GetClientFeature,		1, 0, 0 },
	{ "GetServerFeature",			SE_GetServerFeature,		1, 0, 0 },
	{ "GetServerSetting",			SE_GetServerSetting,		1, 0, 0 },

	{ "DeleteFile",					SE_DeleteFile,				2, 0, 0 },

	{ "GetAccountCount",			SE_GetAccountCount,			0, 0, 0 },
	{ "GetPlayerCount",				SE_GetPlayerCount,			0, 0, 0 },

	{ "BASEITEMSERIAL",				SE_BASEITEMSERIAL,			0, 0, 0 },
	{ "INVALIDSERIAL",				SE_INVALIDSERIAL,			0, 0, 0 },
	{ "INVALIDID",					SE_INVALIDID,				0, 0, 0 },
	{ "INVALIDCOLOUR",				SE_INVALIDCOLOUR,			0, 0, 0 },

	{ nullptr,						nullptr,					0, 0, 0 },
};

void UOX3ErrorReporter( JSContext *cx, const char *message, JSErrorReport *report )
{
	UI16 scriptNum = JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	// If we're loading the world then do NOT print out anything!
	Console.error( strutil::format("JS script failure: Script Number (%u) Message (%s)", scriptNum, message ));
	if( report == nullptr || report->filename == nullptr )
	{
		Console.error( "No detailed data" );
		return;
	}
	Console.error( strutil::format("Filename: %s\n| Line Number: %i", report->filename, report->lineno) );
	Console.error( strutil::format("Erroneous Line: %s\n| Token Ptr: %s", report->linebuf, report->tokenptr ));
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 TryParseJSVal( jsval toParse )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper function to parse jsval returned from script
//o-----------------------------------------------------------------------------------------------o
SI08 TryParseJSVal( jsval toParse )
{
	if( JSVAL_IS_NULL( toParse ) || ( !JSVAL_IS_BOOLEAN( toParse ) && !JSVAL_IS_INT( toParse ) ))
	{
		// jsval is neither a bool nor an int - possibly an object!
		return 0;
	}
	else if( JSVAL_IS_BOOLEAN( toParse ) == JS_FALSE && JSVAL_IS_INT( toParse ) )
	{
		// jsval is an int!
		return static_cast<SI08>(JSVAL_TO_INT( toParse ));
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
	{
		return;
	}
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
		delete gumpDisplays[i];
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
		JS_DestroyScript( targContext, targScript );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool cScript::OnStart( void )
//|	Date		-	8/16/2003 3:44:50 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	The OnStart event is provided to allow a script to process
//|					and read in any state information that has been saved from
//|					a previous server shut down. If a a script come with an
//|					OnStart event the code that is provided will be executed
//|					just following the loading of the script.
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStart( void )
{
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool cScript::OnStop( void )
//|	Date		-	8/16/2003 3:44:44 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	The OnStop event is provided to allow a script to perform
//|					any special cleanup, or state saving as a server shuts
//|					down. If a script has an OnStop event then any code that
//|					is provided will be executed just prior to the JSE shut
//|					down.
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStop( void )
{
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnCreate( CBaseObject *thingCreated, bool dfnCreated )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs when an object is added/created
//|	Notes		-	Checks for the presence of onCreateDFN by default, but onCreateTile can also
//|					be used to intercept creation of items directly from tiles/harditems.dfn
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnCreate( CBaseObject *thingCreated, bool dfnCreated )
{
	if( !ValidateObject( thingCreated ) )
		return false;

	std::string functionName = "onCreateDFN";
	if( !dfnCreated )
	{
		functionName = "onCreateTile";
		if( !ExistAndVerify( seOnCreateTile, functionName ) )
			return false;
	}
	else
		if( !ExistAndVerify( seOnCreateDFN, functionName ) )
			return false;

	jsval rval, params[2];
	UI08 paramType = 0;
	JSObject *myObj;
	if( thingCreated->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingCreated, runTime );
		paramType = 1;
	}
	else {
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingCreated, runTime );
	}

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, functionName.c_str(), 2, params, &rval );
	if( retVal == JS_FALSE ){
		if( !dfnCreated )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnDelete( CBaseObject *thingDestroyed )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs when an object is deleted
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnDelete( CBaseObject *thingDestroyed )
{
	if( !ValidateObject( thingDestroyed ) )
		return false;
	if( !ExistAndVerify( seOnDelete, "onDelete" ) )
		return false;

	jsval rval, params[2];
	UI08 paramType = 0;
	JSObject *myObj;
	if( thingDestroyed->GetObjType() != OT_CHAR )
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingDestroyed, runTime );
	else
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingDestroyed, runTime );
		paramType = 1;
	}
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDelete", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDelete, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSpeech( const char *speech, CChar *personTalking, CChar *talkingTo )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when a character talks in range of the character with event attached
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 4 possible values
//|					-1	=> No such function or bad call
//|					0	=> Let other NPCs and PCs see it
//|					1	=> Let other PCs see it
//|					2	=> Let no one else see it
//|					If JS returns non-int and non-bool, default to 0
//|					If JS returns bool, true == 2, false == 0
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	22 June, 2003 17:30 (making it version 3)
//|						Changed return values from bool to SI08
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSpeech( const char *speech, CChar *personTalking, CBaseObject *talkingTo )
{
	const SI08 RV_NOFUNC = -1;
	if( speech == nullptr || !ValidateObject( personTalking ) || !ValidateObject( talkingTo ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSpeech, "onSpeech" ) )
		return RV_NOFUNC;

	jsval params[3], rval;
	JSString *strSpeech 	= nullptr;
	std::string lwrSpeech	= speech;

	strSpeech = JS_NewStringCopyZ( targContext, strutil::tolower( lwrSpeech ).c_str() );

	JSObject *ptObj = JSEngine->AcquireObject( IUE_CHAR, personTalking, runTime );
	JSObject *ttObj = nullptr;
	if( talkingTo->CanBeObjType( OT_CHAR ) )
	{
		ttObj = JSEngine->AcquireObject( IUE_CHAR, talkingTo, runTime );
	}
	else if( talkingTo->CanBeObjType( OT_ITEM ) )
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

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return static_cast< SI08 >(JSVAL_TO_INT( rval ));
		else if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				return 2;
		}
	}

	return 0;	// return default
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InRange( CChar *person, CBaseObject *objInRange )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when an object comes in range of a character with the event attached
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	A third parameter is provided to let script know whether object that comes into
//|					range is a character or an item (Multi not supported yet)
//o-----------------------------------------------------------------------------------------------o
bool cScript::InRange( CChar *person, CBaseObject *objInRange )
{
	if( !ValidateObject( person ) || !ValidateObject( objInRange ) )
		return false;
	if( !ExistAndVerify( seInRange, "inRange" ) )
		return false;

	jsval params[3], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, person, runTime );

	JSObject *myObj2;
	if( objInRange->CanBeObjType( OT_CHAR ) )
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objInRange, runTime );
	else
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objInRange, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( myObj2 );
	if( objInRange->CanBeObjType( OT_CHAR ) )
		params[2] = INT_TO_JSVAL( 0 );
	else
		params[2] = INT_TO_JSVAL( 1 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "inRange", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seInRange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnCollide( CSocket *tSock, CChar *objColliding, CBaseObject *objCollideWith )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when a character collides with it
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnCollide( CSocket *tSock, CChar *objColliding, CBaseObject *objCollideWith )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objColliding ) || !ValidateObject( objCollideWith ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCollide, "onCollide" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objColliding, runTime );
	JSObject *myObj2	= nullptr;
	if( objCollideWith->GetObjType() == OT_CHAR )
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objCollideWith, runTime );
	else
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objCollideWith, runTime );

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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSteal( CChar *thief, CItem *theft )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when stolen
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSteal( CChar *thief, CItem *theft, CChar *victim )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( thief ) || !ValidateObject( theft ) || !ValidateObject( victim ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSteal, "onSteal" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDispel( CBaseObject *dispelled )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when dispelled
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDispel( CBaseObject *dispelled )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( dispelled ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDispel, "onDispel" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSkill( CBaseObject *skillUse, SI08 skillUsed )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when using a skill
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	If used WITH RegisterSkill() in a script listed under SKILLUSE_SCRIPTS in
//|					JSE_FILEASSOCIATIONS.SCP, event will trigger for only the specified skill.
//|					If used WITHOUT RegisterSkill() in a general purpose script listed under
//|					SCRIPT_LIST instead, event will be a global listener for ALL skills used
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSkill( CBaseObject *skillUse, SI08 skillUsed )
{
	if( !ValidateObject( skillUse ) )
		return false;
	if( !ExistAndVerify( seOnSkill, "onSkill" ) )
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
		SetEventExists( seOnSkill, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnAttack( CChar *attacker, CChar *defender )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when attacking someone
//|					Will also trigger the onDefense event for the character being attacked
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnAttack( CChar *attacker, CChar *defender )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return false;
	if( !ExistAndVerify( seOnAttack, "onAttack" ) )
		return false;

	jsval rval, params[2];
	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAttack", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnAttack, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnDefense( CChar *attacker, CChar *defender )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being attacked
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnDefense( CChar *attacker, CChar *defender )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return false;
	if( !ExistAndVerify( seOnDefense, "onDefense" ) )
		return false;

	jsval rval, params[2];
	JSObject *attObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attObj );
	params[1] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDefense", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDefense, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSkillGain( CChar *player, SI08 skill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when gaining skillpoints
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSkillGain( CChar *player, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnSkillGain, "onSkillGain" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillGain", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillGain, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnStatGained( CChar *player, UI32 stat, SI08 skill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when gaining stats
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStatGained( CChar *player, UI32 stat, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatGained, "onStatGained" ) )
		return false;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatGained", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatGained, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnStatGain( CChar *player, UI32 stat, SI08 skill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStatGain( CChar *player, UI32 stat, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatGain, "onStatGain" ) )
		return false;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatGain", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatGain, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonID )
//|	Date		-	19/01/2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when activating Virtue Gump icon
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonID )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ) || !ValidateObject( tChar ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnVirtueGumpPress, "onVirtueGumpPress" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );
	JSObject *targObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( targObj );
	params[2] = INT_TO_JSVAL( buttonID );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onVirtueGumpPress", 3, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnVirtueGumpPress, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnQuestGump( CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who activate Quest button in paperdoll
//|					Return true to prevent additional onQuestGump events from triggering
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnQuestGump( CChar *mChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnQuestGump, "onQuestGump" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SI08( CChar *mChar, UI08 abilityID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character who activate special abilities in combat books etc
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSpecialMove( CChar *mChar, UI08 abilityID )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mChar ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSpecialMove, "onSpecialMove" ) )
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( abilityID );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpecialMove", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpecialMove, false );
		return RV_NOFUNC;
	}
	
	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDrop( CItem *item, CChar *dropper )
//|	Date		-	02/07/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropped by character
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDrop( CItem *item, CChar *dropper )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( item ) || !ValidateObject( dropper ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDrop, "onDrop" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropping it on another item, or when
//|					another item is dropped on said item
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( item ) || !ValidateObject( dropper ) || !ValidateObject( dest ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDropItemOnItem, "onDropItemOnItem" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnPickup( CItem *item, CChar *pickerUpper )
//|	Date		-	25/01/2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when picked up by character
//o-----------------------------------------------------------------------------------------------o
//|	Changes		-	16/07/2008
//|						Adjustments made to fix event, which didn't trigger
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnPickup( CItem *item, CChar *pickerUpper )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( item ) || !ValidateObject( pickerUpper ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnPickup, "onPickup" ) )
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pickerUpper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onPickup", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnPickup, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when swung in combat
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( swinging ) || !ValidateObject( swinger ) || !ValidateObject( swingTarg ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSwing, "onSwing" ) )
		return RV_NOFUNC;

	JSObject *itemObj	= JSEngine->AcquireObject( IUE_ITEM, swinging, runTime );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDecay( CItem *decaying )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when about to decay
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDecay( CItem *decaying )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( decaying ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDecay, "onDecay" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnLeaving( CMultiObj *left, CBaseObject *leaving )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when leaving a multi
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnLeaving( CMultiObj *left, CBaseObject *leaving )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( left ) || !ValidateObject( leaving ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnLeaving, "onLeaving" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnEquipAttempt( CChar *equipper, CItem *equipping )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when character tries to equip it
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnEquipAttempt( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEquipAttempt, "onEquipAttempt" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnEquip( CChar *equipper, CItem *equipping )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when equipped by a character
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnEquip( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnEquip, "onEquip" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUnequipAttempt( CChar *equipper, CItem *equipping )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when unequipped by a character
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUnequipAttempt( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnUnequipAttempt, "onUnequipAttempt" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUnequip( CChar *equipper, CItem *equipping )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when unequipped by a character
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUnequip( CChar *equipper, CItem *equipping )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnUnequip, "onUnequip" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUseChecked( CChar *user, CItem *iUsing )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers (after hardcoded checks) for item with event attached, when used
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 3 possible values
//|					-1	=> No such function or bad call
//|					0	=> Don't execute hard coded implementation
//|					1	=> Execute hard coded implementations as well
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	31 July, 2003 15:39 ( making it version 3)
//|						Changed return values from bool to SI08
//|					27 October, 2007
//|						Split onUse into onUseChecked and onUseUnChecked
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUseChecked( CChar *user, CItem *iUsing )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( user ) || !ValidateObject( iUsing ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnUseChecked, "onUseChecked" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUseUnChecked( CChar *user, CItem *iUsing )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers (before hardcoded checks) for item with event attached, when used
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 3 possible values
//|					-1	=> No such function or bad call
//|					0	=> Don't execute hard coded implementation
//|					1	=> Execute hard coded implementations as well
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	31 July, 2003 15:39 ( making it version 3)
//|						Changed return values from bool to SI08
//|					27 October, 2007
//|						Split onUse into onUseChecked and onUseUnChecked
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUseUnChecked( CChar *user, CItem *iUsing )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( user ) || !ValidateObject( iUsing ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnUseUnChecked, "onUseUnChecked" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropped on an NPC, and for NPC with
//|					event attached if item is dropped on it and doesn't bounce in item's script
//o-----------------------------------------------------------------------------------------------o
//|	Changes		-	V2 -
//|						Returns
//|							-1 if no function exists
//|							0 if should bounce
//|							1 if should not bounce and use code
//|							2 if should not bounce and not use code
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( srcChar ) || !ValidateObject( dstChar ) || !ValidateObject( item ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDropItemOnNpc, "onDropItemOnNpc" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnEntrance( CMultiObj *left, CBaseObject *leaving )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when entering a multi
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnEntrance( CMultiObj *left, CBaseObject *leaving )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( left ) || !ValidateObject( leaving ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnEntrance, "onEntrance" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OutOfRange( CChar *person, CBaseObject *objVanish )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when objects go out of range
//|	Notes		-	A third parameter in the event can be used to determine whether the object
//|					going out of range is an ITEM or a CHARACTER
//o-----------------------------------------------------------------------------------------------o
bool cScript::OutOfRange( CChar *person, CBaseObject *objVanish )
{
	if( !ValidateObject( person ) || !ValidateObject( objVanish ) )
		return false;
	if( !ExistAndVerify( seOutOfRange, "outOfRange" ) )
		return false;

	jsval params[3], rval;
	JSObject *myObj;
	if( objVanish->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, objVanish, runTime );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		myObj = JSEngine->AcquireObject( IUE_ITEM, objVanish, runTime );
		params[2] = INT_TO_JSVAL( 1 );
	}
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, person, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( myObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "outOfRange", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOutOfRange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnLogin( CSocket *sockPlayer, CChar *pPlayer )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when logging on to server
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnLogin( CSocket *sockPlayer, CChar *pPlayer )
{
	if( !ValidateObject( pPlayer ) )
		return false;
	if( !ExistAndVerify( seOnLogin, "onLogin" ) )
		return false;

	jsval params[2], rval;
	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime );
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pPlayer, runTime );

	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLogin", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnLogin, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnLogout( CSocket *sockPlayer, CChar *pPlayer )
//|	Date		-	10/06/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when logging out of server
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnLogout( CSocket *sockPlayer, CChar *pPlayer )
{
	if( !ValidateObject( pPlayer ) )
		return false;
	if( !ExistAndVerify( seOnLogout, "onLogout" ) )
		return false;

	jsval params[2], rval;

	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime );
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pPlayer, runTime );

	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLogout", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnLogout, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnClick( CSocket *sockPlayer, CBaseObject *objClicked )
//|	Date		-	10/06/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when a player single-clicks on it
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnClick( CSocket *sockPlayer, CBaseObject *objClicked )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objClicked ) )
		return false;
	if( !ExistAndVerify( seOnClick, "onClick" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnFall( CChar *pFall, SI08 fallDistance )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when fallDistance is over 20
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnFall( CChar *pFall, SI08 fallDistance )
{
	if( !ValidateObject( pFall ) )
		return false;
	if( !ExistAndVerify( seOnFall, "onFall" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pFall, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( fallDistance );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFall", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnFall, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnAISliver( CChar *pSliver )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers on every AI loop for character with event attached
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnAISliver( CChar *pSliver )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pSliver ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnAISliver, "onAISliver" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSystemSlice( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSystemSlice( void )
{
	if( !ExistAndVerify( seOnSystemSlice, "onSystemSlice" ) )
		return false;

	jsval rval;
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSystemSlice", 0, nullptr, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSystemSlice, false );
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnLightChange( CBaseObject *tObject, UI08 lightLevel )
//|	Date		-	17/02/2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when lightlevel changes
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnLightChange( CBaseObject *tObject, UI08 lightLevel )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( tObject ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnLightChange, "onLightChange" ) )
		return RV_NOFUNC;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	else
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );

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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnWeatherChange( CBaseObject *tObject, WeatherType element )
//|	Date		-	17/02/2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when weather changes
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnWeatherChange( CBaseObject *tObject, WeatherType element )
{
	if( !ValidateObject( tObject ) )
		return false;
	if( !ExistAndVerify( seOnWeatherChange, "onWeatherChange" ) )
		return false;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	else
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( element );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onWeatherChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnWeatherChange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool cScript::OnTempChange( CBaseObject *tObject, SI08 temp )
//|	Date		-	17/02/2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when temperature changes
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnTempChange( CBaseObject *tObject, SI08 temp )
{
	if( !ValidateObject( tObject ) )
		return false;
	if( !ExistAndVerify( seOnTempChange, "onTempChange" ) )
		return false;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	else
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( temp );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTempChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnTempChange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnTimer( CBaseObject *tObject, UI08 timerID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached, when custom timers started on said
//|					object using StartTimer function update
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnTimer( CBaseObject *tObject, UI08 timerID )
{
	if( !ValidateObject( tObject ) )
		return false;
	if( !ExistAndVerify( seOnTimer, "onTimer" ) )
		return false;

	jsval rval, params[2];
	JSObject *myObj;
	if( tObject->GetObjType() == OT_CHAR )
		myObj = JSEngine->AcquireObject( IUE_CHAR, tObject, runTime );
	else
		myObj = JSEngine->AcquireObject( IUE_ITEM, tObject, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( timerID );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTimer", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnTimer, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnStatLoss( CChar *player, UI32 stat )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when losing stats
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStatLoss( CChar *player, UI32 stat )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatLoss, "onStatLoss" ) )
		return false;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatLoss, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnStatChange( CChar *player, UI32 stat )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when stats change
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStatChange( CChar *player, UI32 stat )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatChange, "onStatChange" ) )
		return false;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( stat );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatChange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSkillLoss( CChar *player, SI08 skill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when losing skillpoints
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSkillLoss( CChar *player, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnSkillLoss, "onSkillLoss" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillLoss, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSkillChange( CChar *player, SI08 skill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when skillpoints change
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSkillChange( CChar *player, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnSkillChange, "onSkillChange" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, player, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillChange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDeath( CChar *pDead, CItem *iCorpse )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached after dying
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDeath( CChar *pDead, CItem *iCorpse )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pDead ) || !ValidateObject( iCorpse ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDeath, "onDeath" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnResurrect( CChar *pAlive )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being resurrected
//|	Notes		-	If script returns false when event triggers, resurrection is blocked
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnResurrect( CChar *pAlive )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pAlive ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnResurrect, "onResurrect" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when the flag status changes
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnFlagChange, "onFlagChange" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles callbacks for custom target cursors triggered from scripts
//o-----------------------------------------------------------------------------------------------o
bool cScript::DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
{
	if( tSock == nullptr )
		return false;
	jsval params[2], rval;
	SI32 objType			= 2;	// 2 == null, 1 == char, 0 == item
	CBaseObject *mObj	= nullptr;
	JSObject *myObj2	= nullptr;
	try
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
		if(myObj == nullptr)
			return false;
		params[0] = OBJECT_TO_JSVAL( myObj );
		if( targeted >= BASEITEMSERIAL )
		{
			mObj	= calcItemObjFromSer( targeted );
			objType = 0;
		}
		else
		{
			mObj	= calcCharObjFromSer( targeted );
			objType	= 1;
		}
		if( !ValidateObject( mObj ) )
		{
			objType		= 2;
			params[1]	= JSVAL_NULL;
		}
		else
		{
			if( objType == 0 )
				myObj2 = JSEngine->AcquireObject( IUE_ITEM, mObj, runTime );
			else
				myObj2 = JSEngine->AcquireObject( IUE_CHAR, mObj, runTime );
			params[1] = OBJECT_TO_JSVAL( myObj2 );
		}
		// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
		JS_SetGlobalObject( targContext, targObject );


		JSBool retVal = JS_CallFunctionName( targContext, targObject, strutil::format( "onCallback%i", callNum ).c_str(), 2, params, &rval );
		return ( retVal == JS_TRUE );
	}
	catch( ... )
	{
		Console.error( "Handled exception in cScript.cpp DoCallback()");
	}
	return false;
}

JSObject *cScript::Object( void ) const
{
	return targObject;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnHungerChange( CChar *pChanging, SI08 newStatus )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when hunger level changes
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnHungerChange( CChar *pChanging, SI08 newStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnHungerChange, "onHungerChange" ) )
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

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool OnThirstChange( CChar *pChanging, SI08 newStatus )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Triggers for character with event attached when thirst level changes
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnThirstChange( CChar* pChanging, SI08 newStatus )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( pChanging ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnThirstChange, "onThirstChange") )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being stolen from
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( stealing ) || !ValidateObject( stolenFrom ) || !ValidateObject( stolen ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnStolenFrom, "onStolenFrom" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSnooped( CChar *snooped, CChar *snooper, bool success )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when someone snoops their backpack
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSnooped( CChar *snooped, CChar *snooper, bool success )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSnooped, "onSnooped" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSnoopAttempt( CChar *snooped, CChar *snooper )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached if they attempt to snoop someone's backpack
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSnoopAttempt( CChar *snooped, CChar *snooper )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSnoopAttempt, "onSnoopAttempt" ) )
		return RV_NOFUNC;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, snooped, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, snooper, runTime );

	params[0] = OBJECT_TO_JSVAL( thiefObj );
	params[1] = OBJECT_TO_JSVAL( victimObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSnoopAttempt", 2, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSnoopAttempt, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t NewGumpList( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
size_t cScript::NewGumpList( void )
{
	size_t retVal	= gumpDisplays.size();
	SEGump *toAdd	= new SEGump;
	toAdd->one		= new STRINGLIST;
	toAdd->two		= new STRINGLIST;

	gumpDisplays.push_back( toAdd );
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SEGump * GetGumpList( SI32 index )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
SEGump * cScript::GetGumpList( SI32 index )
{
	if( index < 0 || (size_t)index >= gumpDisplays.size() )
		return nullptr;
	return gumpDisplays[index];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveGumpList( SI32 index )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
void cScript::RemoveGumpList( SI32 index )
{
	if( index < 0 || (size_t)index >= gumpDisplays.size() )
		return;

	delete gumpDisplays[index]->one;
	delete gumpDisplays[index]->two;
	delete gumpDisplays[index];

	gumpDisplays.erase( gumpDisplays.begin() + index );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendGumpList( SI32 index, CSocket *toSendTo )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	UNUSED
//o-----------------------------------------------------------------------------------------------o
void cScript::SendGumpList( SI32 index, CSocket *toSendTo )
{
	if( index < 0 || (size_t)index >= gumpDisplays.size() )
		return;

	UI32 gumpID = (0xFFFF + JSMapping->GetScriptID( targObject ));
	SendVecsAsGump( toSendTo, *(gumpDisplays[index]->one), *(gumpDisplays[index]->two), gumpID, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleGumpPress( CPIGumpMenuSelect *packet )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Process a gump button press
//o-----------------------------------------------------------------------------------------------o
//|	Changes		-	1/21/2003 - Implemented the code needed to support enhanced
//|									gump response processing
//o-----------------------------------------------------------------------------------------------o
void cScript::HandleGumpPress( CPIGumpMenuSelect *packet )
{
	if( !ExistAndVerify( seOnGumpPress, "onGumpPress" ) )
		return;

	if( packet == nullptr )
		return;
	CSocket *pressing = packet->GetSocket();
	if( pressing == nullptr )
		return;

	UI32 button		= packet->ButtonID();
	UI16 nButtons	= static_cast<UI16>(packet->SwitchCount());
	UI16 nText		= static_cast<UI16>(packet->TextCount());

	SEGumpData *segdGumpData	= new SEGumpData;
	JSObject *jsoObject			= JS_NewObject( targContext, &UOXGumpData_class, nullptr, nullptr );
	JS_DefineFunctions( targContext, jsoObject, CGumpData_Methods );
	JS_DefineProperties( targContext, jsoObject, CGumpDataProperties );
	JS_SetPrivate( targContext, jsoObject, segdGumpData );
	JS_LockGCThing( targContext, jsoObject );
	//JS_AddRoot( targContext, &jsoObject );

	UI16 i;
	// Loop through Buttons
	for( i = 0; i < nButtons; ++i )
		segdGumpData->nButtons.push_back( packet->SwitchValue( i ) );
	// Process text for the buttons?
	// Loop grabbing text
	for( i = 0; i < nText; ++i )
	{
		segdGumpData->nIDs.push_back( packet->GetTextID( i ) );
		segdGumpData->sEdits.push_back( packet->GetTextString( i ) );
	}
	jsval jsvParams[3], jsvRVal;

	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, pressing, runTime );
	jsvParams[0] = OBJECT_TO_JSVAL( myObj );
	jsvParams[1] = INT_TO_JSVAL( button );
	jsvParams[2] = OBJECT_TO_JSVAL( jsoObject );
	[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, "onGumpPress", 3, jsvParams, &jsvRVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleGumpInput( CPIGumpInput *pressing )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle gump inputs?
//o-----------------------------------------------------------------------------------------------o
void cScript::HandleGumpInput( CPIGumpInput *pressing )
{
	if( pressing == nullptr )
		return;
	if( !ExistAndVerify( seOnGumpInput, "onGumpInput" ) )
		return;

	jsval params[2], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, pressing->GetSocket(), runTime );
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( pressing->Index() );
	[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, "onGumpInput", 2, params, &rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnEnterRegion( CChar *entering, UI16 region )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when entering a region
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnEnterRegion( CChar *entering, UI16 region )
{
	if( !ValidateObject( entering ) )
		return false;
	if( !ExistAndVerify( seOnEnterRegion, "onEnterRegion" ) )
		return false;

	jsval params[2], rval;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, entering, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( region );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEnterRegion", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnEnterRegion, false );
	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnLeaveRegion( CChar *leaving, UI16 region )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when leaving a region
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnLeaveRegion( CChar *leaving, UI16 region )
{
	if( !ValidateObject( leaving ) )
		return false;
	if( !ExistAndVerify( seOnLeaveRegion, "onLeaveRegion" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, leaving, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( region );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLeaveRegion", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnLeaveRegion, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when targeting something with a spell
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( target ) || !ValidateObject( caster ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSpellTarget, "onSpellTarget" ) )
		return RV_NOFUNC;

	jsval params[4], rval;
	JSObject *castObj = JSEngine->AcquireObject( IUE_CHAR, caster, runTime );
	JSObject *targObj;
	if( target->CanBeObjType( OT_CHAR ) )
	{
		targObj = JSEngine->AcquireObject( IUE_CHAR, target, runTime );
		params[1] = INT_TO_JSVAL( 0 );
	}
	else
	{
		targObj = JSEngine->AcquireObject( IUE_ITEM, target, runTime );
		params[1] = INT_TO_JSVAL( 1 );
	}
	params[0] = OBJECT_TO_JSVAL( targObj );
	params[2] = OBJECT_TO_JSVAL( castObj );
	params[3] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellTarget", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellTarget, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams )
//|	Date		-	20th December, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o-----------------------------------------------------------------------------------------------o
bool cScript::CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams )
{
	if( eventToCall == nullptr )
		return false;

	jsval rval;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );
	//JSBool retVal = JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, &rval );
	[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, &rval );
	return ( JSVAL_TO_BOOLEAN( rval ) != JS_FALSE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 OnSpellCast( CChar *tChar, UI08 SpellID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached casts a spell
//|	Notes		-	Return value table
//|						-2: use NORMAL non-JS casting
//|						-1: CANCEL spellcasting
//|						0->inf: Spell delay in ms
//o-----------------------------------------------------------------------------------------------o
SI16 cScript::OnSpellCast( CChar *tChar, UI08 SpellID )
{
	if( !ValidateObject( tChar ) )
		return -2;

	if( !ExistAndVerify( seOnSpellCast, "onSpellCast" ) )
		return -2;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellCast", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellCast, false );
		return -2;
	}

	return (SI16)JSVAL_TO_INT( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 OnScrollCast( CChar *tChar, UI08 SpellID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached casts a spell through a scroll
//|	Notes		-	Return value table
//|						-2: use NORMAL non-JS casting
//|						-1: CANCEL spellcasting
//|						0->inf: Spell delay in ms
//o-----------------------------------------------------------------------------------------------o
SI16 cScript::OnScrollCast( CChar *tChar, UI08 SpellID )
{
	if( !ValidateObject( tChar ) )
		return -2;

	if( !ExistAndVerify( seOnScrollCast, "onScrollCast" ) )
		return -2;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onScrollCast", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnScrollCast, false );
		return -2;
	}

	return (SI16)JSVAL_TO_INT( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSpellSuccess( CChar *tChar, UI08 SpellID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers after character with event attached successfully casts a spell
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellSuccess( CChar *tChar, UI08 SpellID )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( tChar ) )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnSpellSuccess, "onSpellSuccess" ) )
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellSuccess", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpellSuccess, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnTalk( CChar *myChar, const char *mySpeech )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached says something
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnTalk( CChar *myChar, const char *mySpeech )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( myChar ) )
		return RV_NOFUNC;

	if( !ExistAndVerify( seOnTalk, "onTalk" ) )
		return RV_NOFUNC;

	jsval params[2], rval;

	JSString *strSpeech		= nullptr;
	std::string lwrSpeech	= mySpeech;

	strSpeech = JS_NewStringCopyZ( targContext, strutil::tolower( lwrSpeech ).c_str() );

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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSpeechInput( CChar *myChar, CItem *myItem, const char *mySpeech )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers after player with event attached inputs speech after a request for
//|					speech input has been made.
//|	Notes		-	This function is called out of network.cpp if a speechmode(9) was previously set
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSpeechInput( CChar *myChar, CItem *myItem, const char *mySpeech )
{
	if( !ValidateObject( myChar ) )
		return true;

	if( !ExistAndVerify( seOnSpeechInput, "onSpeechInput" ) )
		return true;

	jsval params[4], rval;
	JSString *strSpeech = nullptr;

	char *lwrSpeech = new char[strlen(mySpeech)+1];
	strcpy( lwrSpeech, mySpeech );
	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );
	delete[] lwrSpeech;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );

	if( !ValidateObject( myItem ) )
		params[1] = JSVAL_NULL;
	else
	{
		JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, myItem, runTime );
		params[1] = OBJECT_TO_JSVAL( itemObj );
	}

	params[2] = STRING_TO_JSVAL( strSpeech );
	params[3] = INT_TO_JSVAL( myChar->GetSpeechID() );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpeechInput", 4, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpeechInput, false );
		return true;
	}

	return ( rval == JSVAL_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSpellGain( CItem *book, const UI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are added to them
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellGain( CItem *book, const UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( book ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSpellGain, "onSpellGain" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSpellLoss( CItem *book, const UI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are removed from them
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSpellLoss( CItem *book, const UI08 spellNum )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( book ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSpellLoss, "onSpellLoss" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when a skillcheck is performed
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( myChar ) || skill > ALLSKILLS )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSkillCheck, "onSkillCheck" ) )
		return RV_NOFUNC;

	jsval params[4], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( lowSkill );
	params[3] = INT_TO_JSVAL( highSkill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillCheck", 4, params, &rval );
	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillCheck, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s )
//|	Date		-	January 27, 2003
//|	Changes		-	August 17 2005
//|						Renamed to AreaObjFunc from AreaCharFunc
//|						Added support for other object types
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls the function represented in funcName for the script
//|				-	passing in two character parameters
//o-----------------------------------------------------------------------------------------------o
bool cScript::AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s )
{
	if( !ValidateObject( srcObject ) || !ValidateObject( tmpObject ) || funcName == nullptr )
		return false;

	jsval params[3], rval;

	JSObject *srcObj = nullptr;
	JSObject *tmpObj = nullptr;

	if( srcObject == nullptr || tmpObject == nullptr )
	{
		return false;
	}

	if( srcObject->CanBeObjType( OT_ITEM ) )
	{
		srcObj = JSEngine->AcquireObject( IUE_ITEM, srcObject, runTime );
	}
	else if( srcObject->CanBeObjType( OT_CHAR ) )
	{
		srcObj = JSEngine->AcquireObject( IUE_CHAR, srcObject, runTime );
	}

	if( tmpObject->CanBeObjType( OT_ITEM ) )
	{
		tmpObj = JSEngine->AcquireObject( IUE_ITEM, tmpObject, runTime );
	}
	else if( tmpObject->CanBeObjType( OT_CHAR ) )
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
		params[2]	= JSVAL_NULL;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );

	//FIXME === do we need this retvalue?
	//JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );
	[[maybe_unused]] JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );

	return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnCommand( CSocket *mSock, std::string command )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Quick and dirty way to setup custom commands
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnCommand( CSocket *mSock, std::string command )
{
	const SI08 RV_NOFUNC = -1;
	if( mSock  == nullptr || command == "" )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCommand, "onCommand" ) )
		return RV_NOFUNC;

	jsval params[2], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	JSString *strCmd = nullptr;
	strCmd = JS_NewStringCopyZ( targContext, strutil::tolower( command ).c_str() );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExistAndVerify( ScriptEvent eventNum, std::string functionName )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o-----------------------------------------------------------------------------------------------o
bool cScript::ExistAndVerify( ScriptEvent eventNum, std::string functionName )
{
	if( !EventExists( eventNum ) )
		return false;

	if( NeedsChecking( eventNum ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ScriptRegistration( std::string scriptType )
//|	Date		-	20th December, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Registers a script with the JS engine
//|	Notes		-	Also requires a <scriptType>Registration() event with a Register<scriptType>()
//|					function, and an onSkill() event, both in the same script
//o-----------------------------------------------------------------------------------------------o
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
		Console.warning( strutil::format("Script Number (%u) does not have a %s function", JSMapping->GetScriptID( targObject ), scriptType.c_str() ));
		return false;
	}

	JSBool retVal = JS_CallFunctionName( targContext, targObject, scriptType.c_str(), 0, params, &rval );
	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool executeCommand( CSocket *s, std::string funcName, std::string executedString )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute a player-initiated JS-based command
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, SI32 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers before a spellcast attempt for characters with onSpellCast event attached
//o-----------------------------------------------------------------------------------------------o
bool cScript::MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, SI32 spellNum )
{
	if( !ValidateObject( tChar ) )
		return false;

	if( !ExistAndVerify( seOnSpellCast, "onSpellCast" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnIterate( CBaseObject *a, UI32 &b )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Called after IterateOver JS function is used, and iterates over all items or
//|					characters (as specified) in the game
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnIterate( CBaseObject *a, UI32 &b )
{
	if( !ValidateObject( a ) )
		return true;
	if( !ExistAndVerify( seOnIterate, "onIterate" ) )
		return false;

	jsval params[1], rval;

	JSObject *myObj = nullptr;
	if( a->GetObjType() == OT_CHAR )
		myObj = JSEngine->AcquireObject( IUE_CHAR, a, runTime );
	else
		myObj = JSEngine->AcquireObject( IUE_ITEM, a, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );

	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onIterate", 1, params, &rval );

	/*	if( ValidateObject( a ) )
	{
		if( a->GetObjType() == OT_CHAR )
			JSEngine->ReleaseObject( IUE_CHAR, a );
		else
			JSEngine->ReleaseObject( IUE_ITEM, a );
	}
*/
	if( retVal == JS_FALSE )
		SetEventExists( seOnIterate, false );
	else if( JSVAL_TO_BOOLEAN( rval ) )
		++b;

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnIterateSpawnRegions( CSpawnRegion *a, UI32 &b )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Called after IterateOverSpawnRegions JS function is used, and iterates over
//|					all spawn regions in game
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnIterateSpawnRegions( CSpawnRegion *a, UI32 &b )
{
	if( a == nullptr )
		return true;
	if( !ExistAndVerify( seOnIterateSpawnRegions, "onIterateSpawnRegions" ) )
		return false;

	jsval params[1], rval;

	JSObject *myObj = nullptr;
	myObj = JSEngine->AcquireObject( IUE_SPAWNREGION, a, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onIterateSpawnRegions", 1, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnIterateSpawnRegions, false );
	else if( JSVAL_TO_BOOLEAN( rval ) )
		++b;

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnPacketReceive( CSocket *mSock, UI16 packetNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	"Packet hook" event that allows interception of incoming network packets
//|	Notes		-	Requires OVERLOADPACKETS ini setting to be enabled, and incoming packet must
//|					be registered in JSE_FILEASSOCIATIONS.SCP under the [PACKET_SCRIPTS] section
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnPacketReceive( CSocket *mSock, UI16 packetNum )
{
	if( mSock == nullptr )
		return false;
	if( !ExistAndVerify( seOnPacketReceive, "onPacketReceive" ) )
		return false;

	jsval rval, params[3];
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	params[0]		= OBJECT_TO_JSVAL( myObj );
	params[1]		= INT_TO_JSVAL( (UI08)(packetNum%256) );
	params[2]		= INT_TO_JSVAL( (UI08)(packetNum>>8) );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onPacketReceive", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnPacketReceive, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnCharDoubleClick( CChar *currChar, CChar *targChar )
//|	Date		-	23rd January, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding events that happen when doubleclicking characters, such as
//|					open paperdoll, mounting horses, etc
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnCharDoubleClick( CChar *currChar, CChar *targChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( currChar ) || !ValidateObject( targChar ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCharDoubleClick, "onCharDoubleClick" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSkillGump( CSocket *mSock )
//|	Date		-	23rd January, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding the client's request to open the default skillgump, and
//|					instead do something else (like opening a custom skillgump instead).
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSkillGump( CChar *currChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( currChar ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSkillGump, "onSkillGump" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUseBandageMacro( CSocket *mSock, CChar *targChar, CItem *bandageItem )
//|	Date		-	12th May, 2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Expose bandage macro usage to JS engine so server admins can override the effects
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUseBandageMacro( CSocket *mSock, CChar *targChar, CItem *bandageItem )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( targChar ) || mSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnUseBandageMacro, "onUseBandageMacro" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnCombatStart( CChar *attacker, CChar *defender )
//|	Date		-	23rd January, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when initiating combat
//|	Notes		-	Returning FALSE will also run hard code for this scenario
//|					Returning TRUE will override code's default handling of this scenario
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnCombatStart( CChar *attacker, CChar *defender )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCombatStart, "onCombatStart" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnCombatEnd( CChar *currChar, CChar *targChar )
//|	Date		-	23rd January, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when combat ends
//|	Notes		-	Returning FALSE will also run hard code for this scenario
//|					Returning TRUE will override code's default handling of this scenario
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnCombatEnd( CChar *currChar, CChar *targChar )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( currChar ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCombatEnd, "onCombatEnd" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDeathBlow( CChar *mKilled, CChar *mKiller )
//|	Date		-	8th February, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached when performing a death blow in combat
//|	Notes		-	Returning FALSE will also run hard code for this scenario
//|					Returning TRUE will override code's default handling of this scenario
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDeathBlow( CChar *mKilled, CChar *mKiller )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( mKilled ) ) // || !ValidateObject( mKiller ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnDeathBlow, "onDeathBlow" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill, UI08 hitLoc )
//|	Date		-	21st March, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached every time combat damage is calculated
//|	Notes		-	Returning -1 will default to hard code handling of event
//|					Returning another value will override code's default handling of event
//o-----------------------------------------------------------------------------------------------o
SI16 cScript::OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill, UI08 hitLoc )
{
	const SI16 RV_NOFUNC = -1;
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCombatDamageCalc, "onCombatDamageCalc" ) )
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

	if( damage.isType( JSOT_INT ) || damage.isType( JSOT_DOUBLE ) )	// They returned some sort of value
	{
		return (SI16)damage.toInt();
	}
	else
		funcRetVal = -1;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnDamage( CChar *damaged, CChar *attacker, SI16 damageValue )
//|	Date		-	22nd March, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached takes damage
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnDamage( CChar *damaged, CChar *attacker, SI16 damageValue )
{
	if( !ValidateObject( damaged ) )
		return false;
	if( !ExistAndVerify( seOnDamage, "onDamage" ) )
		return false;

	jsval rval, params[3];
	JSObject *damagedObj = JSEngine->AcquireObject( IUE_CHAR, damaged, runTime );
	params[0] = OBJECT_TO_JSVAL( damagedObj );

	if( ValidateObject( attacker ) )
	{
		JSObject *attackerObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
		params[1] = OBJECT_TO_JSVAL( attackerObj );
	}
	else
		params[1] = JSVAL_NULL;

	params[2] = INT_TO_JSVAL( damageValue );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDamage", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDamage, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnBuy( CSocket *tSock, CChar *objVendor )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs on vendors, triggered before vendor trade-gump is opened
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnBuy( CSocket *tSock, CChar *objVendor )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnBuy, "onBuy" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSell( CSocket *tSock, CChar *objVendor )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs on vendors, triggered before vendor trade-gump is opened
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSell( CSocket *tSock, CChar *objVendor )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSell, "onSell" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnBuyFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens when an item is in the process of being bought 
//|					from an NPC vendor. Returning false/0 from the script will halt the purchase
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnBuyFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnBuyFromVendor, "onBuyFromVendor" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemBought->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBuyFromVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnBuyFromVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSellToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens when an item is in the process of being sold to
//|					an NPC vendor. Returning false/0 from script will halt the sale
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSellToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSellToVendor, "onSellToVendor" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemSold->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSellToVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSellToVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnBoughtFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens AFTER an item has been
//|					bought from an NPC vendor
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnBoughtFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnBoughtFromVendor, "onBoughtFromVendor" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemBought->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBoughtFromVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnBoughtFromVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSoldToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens AFTER an item has been
//|					sold to an NPC vendor
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSoldToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSoldToVendor, "onSoldToVendor" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= nullptr;
	if( objItemSold->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSoldToVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSoldToVendor, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnHouseCommand( CSocket *tSock, CMultiObj *objMulti )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding house commands via JS script attached to multi
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnHouseCommand( CSocket *tSock, CMultiObj *objMulti, UI08 cmdID )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( objMulti ) || tSock == nullptr )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnHouseCommand, "onHouseCommand" ) )
		return RV_NOFUNC;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *multiObj	= JSEngine->AcquireObject( IUE_ITEM, objMulti, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( multiObj );
	params[2] = INT_TO_JSVAL( cmdID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHouseCommand", 3, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnHouseCommand, false );
		return RV_NOFUNC;
	}

	return TryParseJSVal( rval );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnPathfindEnd( CChar *ncp, SI08 pathfindResult )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPCs after their pathfinding efforts come to and end
//|	Notes		-	pathfindResult gives a value that represents how the pathfinding ended				
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnPathfindEnd( CChar *npc, SI08 pathfindResult )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( npc ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnPathfindEnd, "onPathfindEnd" ) )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnEnterEvadeState( CChar *npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPCs when they enter the evade state after failing to pathfind to
//|					a target in combat
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnEnterEvadeState( CChar *npc, CChar *enemy )
{
	const SI08 RV_NOFUNC = -1;
	if( !ValidateObject( npc ) || !ValidateObject( enemy ))
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnEnterEvadeState, "onEnterEvadeState" ) )
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

bool cScript::EventExists( ScriptEvent eventNum ) const
{
	UI32 index = eventNum / 32;
	if( index > 2 )
		return false;
	return eventPresence[index].test( (eventNum % 32) );
}
void cScript::SetEventExists( ScriptEvent eventNum, bool status )
{
	UI32 index = eventNum / 32;
	if( index > 2 )
		return;
	eventPresence[index].set( (eventNum % 32), status );
}

bool cScript::NeedsChecking( ScriptEvent eventNum ) const
{
	UI32 index = eventNum / 32;
	if( index > 2 )
		return false;
	return needsChecking[index].test( (eventNum % 32) );
}
void cScript::SetNeedsChecking( ScriptEvent eventNum, bool status )
{
	UI32 index = eventNum / 32;
	if( index > 2 )
		return;
	needsChecking[index].set( (eventNum % 32), status );
}
