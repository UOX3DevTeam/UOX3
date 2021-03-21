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
	{ "CheckTileFlag",				SE_CheckTileFlag,			2, 0, 0 },
	{ "DoesDynamicBlock",			SE_DoesDynamicBlock,		7, 0, 0 },
	{ "DoesStaticBlock",			SE_DoesStaticBlock,			7, 0, 0 },
	{ "DoesMapBlock",				SE_DoesMapBlock,			8, 0, 0 },

	{ "ResourceArea",				SE_ResourceArea,			2, 0, 0 },
	{ "ResourceAmount",				SE_ResourceAmount,			2, 0, 0 },
	{ "ResourceTime",				SE_ResourceTime,			2, 0, 0 },
	{ "ResourceRegion",				SE_ResourceRegion,			3, 0, 0 },
	{ "Moon",						SE_Moon,					2, 0, 0 },

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

	{ NULL,							NULL,						0, 0, 0 },
};

void UOX3ErrorReporter( JSContext *cx, const char *message, JSErrorReport *report )
{
	UI16 scriptNum = JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	// If we're loading the world then do NOT print out anything!
	Console.error( format("JS script failure: Script Number (%u) Message (%s)", scriptNum, message ));
	if( report == NULL || report->filename == NULL )
	{
		Console.error( "No detailed data" );
		return;
	}
	Console.error( format("Filename: %s\n| Line Number: %i", report->filename, report->lineno) );
	Console.error( format("Erroneous Line: %s\n| Token Ptr: %s", report->linebuf, report->tokenptr ));
}

cScript::cScript( std::string targFile, UI08 rT ) : isFiring( false ), runTime( rT )
{
	for( SI32 i = 0; i < 3; ++i )
	{
		eventPresence[i].set();
		needsChecking[i].set();
	}

	targContext = JSEngine->GetContext( runTime ); //JS_NewContext( JSEngine->GetRuntime( runTime ), 0x2000 );
	if( targContext == NULL )
		return;

	targObject = JS_NewObject( targContext, &uox_class, NULL, NULL );
	if( targObject == NULL )
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
	if( targScript == NULL )
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
	if( targScript != NULL )
		JS_DestroyScript( targContext, targScript );
	Cleanup();
	JS_GC( targContext );
	//	if( targContext != NULL )
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
	if( speech == NULL || !ValidateObject( personTalking ) || !ValidateObject( talkingTo ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnSpeech, "onSpeech" ) )
		return RV_NOFUNC;

	jsval params[3], rval;
	JSString *strSpeech = NULL;
	UString lwrSpeech	= speech;

	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech.lower().c_str() );

	JSObject *ptObj = JSEngine->AcquireObject( IUE_CHAR, personTalking, runTime );
	JSObject *ttObj;
	if( talkingTo->CanBeObjType( OT_CHAR ) )
		ttObj = JSEngine->AcquireObject( IUE_CHAR, talkingTo, runTime );
	else if( talkingTo->CanBeObjType( OT_ITEM ) )
		ttObj = JSEngine->AcquireObject( IUE_ITEM, talkingTo, runTime );

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
			else
				return 0;
		}
		else
			return 0;	// some unknown value
	}
	else
		return 0;	// return default

	return ( retVal == JS_TRUE );
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
//|	Function	-	bool OnCollide( CSocket *tSock, CChar *objColliding, CBaseObject *objCollideWith )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object event is attached to when a character collides with it
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnCollide( CSocket *tSock, CChar *objColliding, CBaseObject *objCollideWith )
{
	if( !ValidateObject( objColliding ) || !ValidateObject( objCollideWith ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnCollide, "onCollide" ) )
		return false;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objColliding, runTime );
	JSObject *myObj2	= NULL;
	if( objCollideWith->GetObjType() == OT_CHAR )
		myObj2 = JSEngine->AcquireObject( IUE_CHAR, objCollideWith, runTime );
	else
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objCollideWith, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCollide", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnCollide, false );
	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSteal( CChar *thief, CItem *theft )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when stolen
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSteal( CChar *thief, CItem *theft )
{
	if( !ValidateObject( thief ) || !ValidateObject( theft ) )
		return false;
	if( !ExistAndVerify( seOnSteal, "onSteal" ) )
		return false;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, thief, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, theft, runTime );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSteal", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSteal, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnDispel( CBaseObject *dispelled )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when dispelled
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnDispel( CBaseObject *dispelled )
{
	if( !ValidateObject( dispelled ) )
		return false;
	if( !ExistAndVerify( seOnDispel, "onDispel" ) )
		return false;
	UI08 funcRetVal = 0;

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
		const SI08 RV_NOFUNC = -1;
		SetEventExists( seOnDispel, false );
		return RV_NOFUNC;
	}

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_IS_INT( rval ) )
				return static_cast< SI08 >(JSVAL_TO_INT( rval ));
			else if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
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
bool cScript::OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonID )
{
	if( !ValidateObject( mChar ) || !ValidateObject( tChar ) )
		return false;
	if( !ExistAndVerify( seOnVirtueGumpPress, "onVirtueGumpPress" ) )
		return false;

	jsval rval, params[3];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, mChar, runTime );
	JSObject *targObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( targObj );
	params[2] = INT_TO_JSVAL( buttonID );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onVirtueGumpPress", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnVirtueGumpPress, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnDrop( CItem *item, CChar *dropper )
//|	Date		-	02/07/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropped by character
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnDrop( CItem *item, CChar *dropper )
{
	if( !ValidateObject( item ) || !ValidateObject( dropper ) )
		return false;
	if( !ExistAndVerify( seOnDrop, "onDrop" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, dropper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDrop", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDrop, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest )
//|	Date		-	19/01/2009
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropping it on another item, or when
//|					another item is dropped on said item
//o-----------------------------------------------------------------------------------------------o
UI08 cScript::OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest )
{
	if( !ValidateObject( item ) || !ValidateObject( dropper ) || !ValidateObject( dest ) )
		return 0;
	if( !ExistAndVerify( seOnDropItemOnItem, "onDropItemOnItem" ) )
		return 0;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, dropper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );
	JSObject *destObj = JSEngine->AcquireObject( IUE_ITEM, dest, runTime );

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( destObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDropItemOnItem", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDropItemOnItem, false );

	UI08 funcRetVal = 0;
	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
		{
			// script returns
			// 0 == bounce
			// 1 == don't bounce, use code
			// 2 == don't bounce, don't use code
			// Our func returns values 1 higher
			funcRetVal = static_cast< UI08 >(JSVAL_TO_INT( rval ) + 1);
			if( funcRetVal < 1 || funcRetVal > 3 )
				funcRetVal = 2;	// don't bounce, use code
		}
		else
			funcRetVal = 2;	// don't bounce, use code
	}
	else
		funcRetVal = 2;	// don't bounce, use code
	return funcRetVal;
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
	SI08 funcRetVal	= -1;

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

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return static_cast< SI08 >(JSVAL_TO_INT( rval ));
		else if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when swung in combat
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
{
	if( !ValidateObject( swinging ) || !ValidateObject( swinger ) || !ValidateObject( swingTarg ) )
		return false;
	if( !ExistAndVerify( seOnSwing, "onSwing" ) )
		return false;

	JSObject *itemObj	= JSEngine->AcquireObject( IUE_ITEM, swinging, runTime );
	JSObject *attObj	= JSEngine->AcquireObject( IUE_CHAR, swinger, runTime );
	JSObject *defObj	= JSEngine->AcquireObject( IUE_CHAR, swingTarg, runTime );

	jsval params[3], rval;
	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( attObj );
	params[2] = OBJECT_TO_JSVAL( defObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSwing", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSwing, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnDecay( CItem *decaying )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item event is attached to when about to decay
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnDecay( CItem *decaying )
{
	if( !ValidateObject( decaying ) )
		return false;
	if( !ExistAndVerify( seOnDecay, "onDecay" ) )
		return false;

	jsval params[1], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_ITEM, decaying, runTime );
	params[0] = OBJECT_TO_JSVAL( myObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDecay", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDecay, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnLeaving( CMultiObj *left, CBaseObject *leaving )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when leaving a multi
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnLeaving( CMultiObj *left, CBaseObject *leaving )
{
	if( !ValidateObject( left ) || !ValidateObject( leaving ) )
		return false;
	if( !ExistAndVerify( seOnLeaving, "onLeaving" ) )
		return false;

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
		SetEventExists( seOnLeaving, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnEquip( CChar *equipper, CItem *equipping )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when equipped by a character
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnEquip( CChar *equipper, CItem *equipping )
{
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return false;

	if( !ExistAndVerify( seOnEquip, "onEquip" ) )
		return false;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, equipper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, equipping, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEquip", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnEquip, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnUnequip( CChar *equipper, CItem *equipping )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when unequipped by a character
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnUnequip( CChar *equipper, CItem *equipping )
{
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return false;
	if( !ExistAndVerify( seOnUnequip, "onUnequip" ) )
		return false;

	jsval rval, params[2];
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, equipper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, equipping, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUnequip", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUnequip, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUseChecked( CChar *user, CItem *iUsing )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers (after hardcoded checks) for item with event attached, when used
//o-----------------------------------------------------------------------------------------------o
//|	Notes		-	The function returns 3 possible values
//|					-1	=> No such function or bad call
//|					0	=> Execute hard coded implementations as well
//|					1	=> Don't execute hard coded implementation
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	31 July, 2003 15:39 ( making it version 3)
//|						Changed return values from bool to SI08
//|					27 October, 2007
//|						Split onUse into onUseChecked and onUseUnChecked
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUseChecked( CChar *user, CItem *iUsing )
{
	if( !ValidateObject( user ) || !ValidateObject( iUsing ) )
		return -1;
	if( !ExistAndVerify( seOnUseChecked, "onUseChecked" ) )
		return -1;

	SI08 funcRetVal	= -1;

	jsval rval, params[2];

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, user, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUseChecked", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUseChecked, false );

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnUseUnChecked( CChar *user, CItem *iUsing )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers (before hardcoded checks) for item with event attached, when used
//|	Notes		-	The function returns 3 possible values
//|					-1	=> No such function or bad call
//|					0	=> Execute hard coded implementations as well
//|					1	=> Don't execute hard coded implementation
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	31 July, 2003 15:39 ( making it version 3)
//|						Changed return values from bool to SI08
//|					27 October, 2007
//|						Split onUse into onUseChecked and onUseUnChecked
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnUseUnChecked( CChar *user, CItem *iUsing )
{
	if( !ValidateObject( user ) || !ValidateObject( iUsing ) )
		return -1;
	if( !ExistAndVerify( seOnUseUnChecked, "onUseUnChecked" ) )
		return -1;

	SI08 funcRetVal	= -1;

	jsval rval, params[2];

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, user, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUseUnChecked", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUseUnChecked, false );

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item )
//|	Date		-	04/18/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when dropped on an NPC, and for NPC with
//|					event attached if item is dropped on it and doesn't bounce in item's script
//o-----------------------------------------------------------------------------------------------o
//|	Changes		-	V2 -
//|						Returns
//|							0 if no function exists
//|							1 if should bounce
//|							2 if should not bounce and use code
//|							3 if should not bounce and not use code
//o-----------------------------------------------------------------------------------------------o
UI08 cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( dstChar ) || !ValidateObject( item ) )
		return 0;
	if( !ExistAndVerify( seOnDropItemOnNpc, "onDropItemOnNpc" ) )
		return 0;

	jsval rval, params[3];

	JSObject *srcObj	= JSEngine->AcquireObject( IUE_CHAR, srcChar, runTime );
	JSObject *dstObj	= JSEngine->AcquireObject( IUE_CHAR, dstChar, runTime );
	JSObject *itemObj	= JSEngine->AcquireObject( IUE_ITEM, item, runTime );
	params[0] = OBJECT_TO_JSVAL( srcObj );
	params[1] = OBJECT_TO_JSVAL( dstObj );
	params[2] = OBJECT_TO_JSVAL( itemObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDropItemOnNpc", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDropItemOnNpc, false );

	UI08 funcRetVal = 0;
	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
		{
			// script returns
			// 0 == bounce
			// 1 == don't bounce, use code
			// 2 == don't bounce, don't use code
			// Our func returns values 1 higher
			funcRetVal = static_cast< UI08 >(JSVAL_TO_INT( rval ) + 1);
			if( funcRetVal < 1 || funcRetVal > 3 )
				funcRetVal = 2;	// don't bounce, use code
		}
		else
			funcRetVal = 2;	// don't bounce, use code
	}
	else
		funcRetVal = 2;	// don't bounce, use code
	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnEntrance( CMultiObj *left, CBaseObject *leaving )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when entering a multi
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnEntrance( CMultiObj *left, CBaseObject *leaving )
{
	if( !ValidateObject( left ) || !ValidateObject( leaving ) )
		return false;
	if( !ExistAndVerify( seOnEntrance, "onEntrance" ) )
		return false;

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
		SetEventExists( seOnEntrance, false );

	return ( retVal == JS_TRUE );
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
//|	Function	-	bool OnClick( CSocket *sockPlayer, CItem *iClicked )
//|	Date		-	10/06/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for item with event attached when a player single-clicks on it
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnClick( CSocket *sockPlayer, CItem *iClicked )
{
	if( !ExistAndVerify( seOnClick, "onClick" ) )
		return false;
	jsval params[2], rval;
	JSObject *sockObj = JSEngine->AcquireObject( IUE_SOCK, sockPlayer, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, iClicked, runTime );

	params[0] = OBJECT_TO_JSVAL( sockObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onClick", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnClick, false );

	return ( retVal == JS_TRUE );
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
//|	Function	-	bool OnAISliver( CChar *pSliver )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers on every AI loop for character with event attached
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnAISliver( CChar *pSliver )
{
	if( !ValidateObject( pSliver ) )
		return false;
	if( !ExistAndVerify( seOnAISliver, "onAISliver" ) )
		return false;

	jsval params[1], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pSliver, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAISliver", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnAISliver, false );

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return ( JSVAL_TO_INT( rval ) != 0 );
		else if( JSVAL_IS_BOOLEAN( rval ) )
			return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
		else
			return false;	// some unknown value
	}
	else
		return false;	// return default
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
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSystemSlice", 0, NULL, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSystemSlice, false );
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnLightChange( CBaseObject *tObject, UI08 lightLevel )
//|	Date		-	17/02/2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for object with event attached when lightlevel changes
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnLightChange( CBaseObject *tObject, UI08 lightLevel )
{
	if( !ValidateObject( tObject ) )
		return false;
	if( !ExistAndVerify( seOnLightChange, "onLightChange" ) )
		return false;

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
		SetEventExists( seOnLightChange, false );

	return ( retVal == JS_TRUE );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnDeath( CChar *pDead, CItem *iCorpse )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached after dying
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnDeath( CChar *pDead, CItem *iCorpse )
{
	if( !ValidateObject( pDead ) || !ValidateObject( iCorpse ))
		return false;
	if( !ExistAndVerify( seOnDeath, "onDeath" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pDead, runTime );
	JSObject *corpseObj = JSEngine->AcquireObject( IUE_ITEM, iCorpse, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( corpseObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeath", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDeath, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnResurrect( CChar *pAlive )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being resurrected
//|	Notes		-	If script returns false when event triggers, resurrection is blocked
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnResurrect( CChar *pAlive )
{
	if( !ValidateObject( pAlive ) )
		return false;
	if( !ExistAndVerify( seOnResurrect, "onResurrect" ) )
		return false;

	bool funcRetVal;
	jsval params[1], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pAlive, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onResurrect", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnResurrect, false );

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = false;		// we do want hard code to execute
			else
				funcRetVal = true;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = false;	// default to hard code
	}
	else
		funcRetVal = false;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when the flag status changes
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
{
	if( !ValidateObject( pChanging ) )
		return false;
	if( !ExistAndVerify( seOnFlagChange, "onFlagChange" ) )
		return false;

	jsval params[3], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( newStatus );
	params[2] = INT_TO_JSVAL( oldStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFlagChange", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnFlagChange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles callbacks for custom target cursors triggered from scripts
//o-----------------------------------------------------------------------------------------------o
bool cScript::DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
{
	if( tSock == NULL )
		return false;
	jsval params[2], rval;
	SI32 objType			= 2;	// 2 == null, 1 == char, 0 == item
	CBaseObject *mObj	= NULL;
	JSObject *myObj2	= NULL;
	try
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
		if(myObj == NULL)
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


		JSBool retVal = JS_CallFunctionName( targContext, targObject, format( "onCallback%i", callNum ).c_str(), 2, params, &rval );
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
//|	Function	-	bool OnHungerChange( CChar *pChanging, SI08 newStatus )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when hunger level changes
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnHungerChange( CChar *pChanging, SI08 newStatus )
{
	if( !ValidateObject( pChanging ) )
		return false;
	if( !ExistAndVerify( seOnHungerChange, "onHungerChange" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pChanging, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( newStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHungerChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnHungerChange, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when being stolen from
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen )
{
	if( !ValidateObject( stealing ) || !ValidateObject( stolenFrom ) || !ValidateObject( stolen ) )
		return false;
	if( !ExistAndVerify( seOnStolenFrom, "onStolenFrom" ) )
		return false;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, stealing, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, stolenFrom, runTime );
	JSObject *itemObj	= JSEngine->AcquireObject( IUE_ITEM, stolen, runTime );

	params[0] = OBJECT_TO_JSVAL( thiefObj );
	params[1] = OBJECT_TO_JSVAL( victimObj );
	params[2] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStolenFrom", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStolenFrom, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSnooped( CChar *snooped, CChar *snooper, bool success )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when someone snoops their backpack
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSnooped( CChar *snooped, CChar *snooper, bool success )
{
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ) )
		return false;
	if( !ExistAndVerify( seOnSnooped, "onSnooped" ) )
		return false;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, snooped, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, snooper, runTime );

	params[0] = OBJECT_TO_JSVAL( thiefObj );
	params[1] = OBJECT_TO_JSVAL( victimObj );
	params[2] = BOOLEAN_TO_JSVAL( success );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSnooped", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSnooped, false );

	return ( retVal == JS_TRUE );
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
		return NULL;
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

	if( packet == NULL )
		return;
	CSocket *pressing = packet->GetSocket();
	if( pressing == NULL )
		return;

	UI32 button		= packet->ButtonID();
	UI16 nButtons	= static_cast<UI16>(packet->SwitchCount());
	UI16 nText		= static_cast<UI16>(packet->TextCount());

	SEGumpData *segdGumpData	= new SEGumpData;
	JSObject *jsoObject			= JS_NewObject( targContext, &UOXGumpData_class, NULL, NULL );
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
		segdGumpData->sEdits.push_back( packet->GetTextUString( i ) );
	}
	jsval jsvParams[3], jsvRVal;

	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, pressing, runTime );
	jsvParams[0] = OBJECT_TO_JSVAL( myObj );
	jsvParams[1] = INT_TO_JSVAL( button );
	jsvParams[2] = OBJECT_TO_JSVAL( jsoObject );
	JS_CallFunctionName( targContext, targObject, "onGumpPress", 3, jsvParams, &jsvRVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleGumpInput( CPIGumpInput *pressing )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle gump inputs?
//o-----------------------------------------------------------------------------------------------o
void cScript::HandleGumpInput( CPIGumpInput *pressing )
{
	if( pressing == NULL )
		return;
	if( !ExistAndVerify( seOnGumpInput, "onGumpInput" ) )
		return;

	jsval params[2], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, pressing->GetSocket(), runTime );
	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( pressing->Index() );
	JS_CallFunctionName( targContext, targObject, "onGumpInput", 2, params, &rval );
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
//|	Function	-	bool OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when targeting something with a spell
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum )
{
	if( !ValidateObject( target ) || !ValidateObject( caster ) )
		return false;
	if( !ExistAndVerify( seOnSpellTarget, "onSpellTarget" ) )
		return false;

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
		SetEventExists( seOnSpellTarget, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams )
//|	Date		-	20th December, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o-----------------------------------------------------------------------------------------------o
bool cScript::CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams )
{
	if( eventToCall == NULL )
		return false;

	jsval rval;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );
	//JSBool retVal = JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, &rval );
	JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, &rval );
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
//|	Function	-	bool OnSpellSuccess( CChar *tChar, UI08 SpellID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers after character with event attached successfully casts a spell
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSpellSuccess( CChar *tChar, UI08 SpellID )
{
	if( !ValidateObject( tChar ) )
		return false;

	if( !ExistAndVerify( seOnSpellSuccess, "onSpellSuccess" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, tChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellSuccess", 2, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellSuccess, false );

	return (retVal == JS_TRUE);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnTalk( CChar *myChar, const char *mySpeech )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers when character with event attached says something
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnTalk( CChar *myChar, const char *mySpeech )
{
	if( !ValidateObject( myChar ) )
		return true;

	if( !ExistAndVerify( seOnTalk, "onTalk" ) )
		return true;

	jsval params[2], rval;

	JSString *strSpeech	= NULL;
	UString lwrSpeech	= mySpeech;

	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech.lower().c_str() );

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = STRING_TO_JSVAL( strSpeech );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTalk", 2, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnTalk, false );
		return true; // We want the speech to be said if there is no event
	}
	return ( rval == JSVAL_TRUE );
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
	JSString *strSpeech = NULL;

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
//|	Function	-	bool OnSpellGain( CItem *book, const UI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are added to them
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSpellGain( CItem *book, const UI08 spellNum )
{
	if( !ValidateObject( book ) )
		return false;
	if( !ExistAndVerify( seOnSpellGain, "onSpellGain" ) )
		return false;

	jsval params[2], rval;
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, book, runTime );
	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellGain", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellGain, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSpellLoss( CItem *book, const UI08 spellNum )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for spellbooks with event attached when spells are removed from them
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSpellLoss( CItem *book, const UI08 spellNum )
{
	if( !ValidateObject( book ) )
		return false;
	if( !ExistAndVerify( seOnSpellLoss, "onSpellLoss" ) )
		return false;

	jsval params[2], rval;
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, book, runTime );
	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellLoss, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for character with event attached when a skillcheck is performed
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill )
{
	if( !ValidateObject( myChar ) || skill > ALLSKILLS )
		return false;
	if( !ExistAndVerify( seOnSkillCheck, "onSkillCheck" ) )
		return false;

	jsval params[4], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, myChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( lowSkill );
	params[3] = INT_TO_JSVAL( highSkill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillCheck", 4, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillCheck, false );

	return ( retVal == JS_TRUE );
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
	if( !ValidateObject( srcObject ) || !ValidateObject( tmpObject ) || funcName == NULL )
		return false;

	jsval params[3], rval;

	JSObject *srcObj = nullptr;
	JSObject *tmpObj = nullptr;

	if( srcObject->CanBeObjType( OT_ITEM ) ) {
		srcObj = JSEngine->AcquireObject( IUE_ITEM, srcObject, runTime );
	}
	else if( srcObject->CanBeObjType( OT_CHAR ) ) {
		srcObj = JSEngine->AcquireObject( IUE_CHAR, srcObject, runTime );
	}
	if (srcObject == nullptr) {
		return false ;
	}

	if( tmpObject->CanBeObjType( OT_ITEM ) ) {
		tmpObj = JSEngine->AcquireObject( IUE_ITEM, tmpObject, runTime );
	}
	else if( tmpObject->CanBeObjType( OT_CHAR ) ) {
		tmpObj = JSEngine->AcquireObject( IUE_CHAR, tmpObject, runTime );
	}

	if (tmpObject == nullptr){
		return false ;
	}


	params[0]			= OBJECT_TO_JSVAL( srcObj );
	params[1]			= OBJECT_TO_JSVAL( tmpObj );

	if( s != NULL )
	{
		JSObject *sockObj	= NULL;
		sockObj		= JSEngine->AcquireObject( IUE_SOCK, s, runTime );
		params[2]	= OBJECT_TO_JSVAL( sockObj );
	}
	else
		params[2]	= JSVAL_NULL;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );

	//FIXME === do we need this retvalue?
	//JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );
	JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );

	return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnCommand( CSocket *mSock )
//|	Date		-	1/13/2003 11:17:48 PM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnCommand( CSocket *mSock )
{
	if( mSock  == NULL )
		return false;
	if( !ExistAndVerify( seOnCommand, "onCommand" ) )
		return false;

	jsval params[1], rval;
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	params[0]		= OBJECT_TO_JSVAL( myObj );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onCommand", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnCommand, false );

	return ( retVal == JS_TRUE );
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
		Console.warning( format("Script Number (%u) does not have a %s function", JSMapping->GetScriptID( targObject ), scriptType.c_str() ));
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

	JSObject *myObj = NULL;
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
	if( a == NULL )
		return true;
	if( !ExistAndVerify( seOnIterateSpawnRegions, "onIterateSpawnRegions" ) )
		return false;

	jsval params[1], rval;

	JSObject *myObj = NULL;
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
	if( mSock == NULL )
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
	SI08 funcRetVal	= -1;
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

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return static_cast< SI08 >(JSVAL_TO_INT( rval ));
		else if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
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
	SI08 funcRetVal	= -1;
	jsval params[1], rval;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, currChar, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onSkillGump", 1, params, &rval );

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSkillGump, false );
		return RV_NOFUNC;
	}

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return static_cast< SI08 >(JSVAL_TO_INT( rval ));
		else if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
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
	if( !ValidateObject( targChar ) || mSock == NULL )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnUseBandageMacro, "onUseBandageMacro" ) )
		return RV_NOFUNC;
	//SI08 funcRetVal = -1;
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

	return -1;
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
	SI08 funcRetVal	= -1;
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

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return static_cast< SI08 >(JSVAL_TO_INT( rval ));
		else if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
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
	SI08 funcRetVal	= -1;
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

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_INT( rval ) )
			return static_cast< SI08 >(JSVAL_TO_INT( rval ));
		else if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
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

	SI08 funcRetVal	= -1;
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

	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_IS_INT( rval ) )
				return static_cast< SI08 >(JSVAL_TO_INT( rval ));
			else if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill )

//|	Date		-	21st March, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for characters with event attached every time combat damage is calculated
//|	Notes		-	Returning -1 will default to hard code handling of event
//|					Returning another value will override code's default handling of event
//o-----------------------------------------------------------------------------------------------o
SI16 cScript::OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill )
{
	const SI16 RV_NOFUNC = -1;
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return RV_NOFUNC;
	if( !ExistAndVerify( seOnCombatDamageCalc, "onCombatDamageCalc" ) )
		return RV_NOFUNC;

	SI16 funcRetVal	= -1;

	jsval rval, params[3];
	JSObject *attackerObj = JSEngine->AcquireObject( IUE_CHAR, attacker, runTime );
	JSObject *defenderObj = JSEngine->AcquireObject( IUE_CHAR, defender, runTime );

	params[0] = OBJECT_TO_JSVAL( attackerObj );
	params[1] = OBJECT_TO_JSVAL( defenderObj );
	params[2] = INT_TO_JSVAL( getFightSkill );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCombatDamageCalc", 3, params, &rval );
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
	if( !ValidateObject( objVendor ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnBuy, "onBuy" ) )
		return false;

	SI08 funcRetVal	= -1;
	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBuy", 2, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnBuy, false );
	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSell( CSocket *tSock, CChar *objVendor )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs on vendors, triggered before vendor trade-gump is opened
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSell( CSocket *tSock, CChar *objVendor )
{
	if( !ValidateObject( objVendor ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnSell, "onSell" ) )
		return false;

	SI08 funcRetVal	= -1;
	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSell", 2, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnSell, false );
	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnBuyFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens when an item is in the
//|					process of being bought from an NPC vendor
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnBuyFromVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemBought )
{
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnBuyFromVendor, "onBuyFromVendor" ) )
		return false;

	SI08 funcRetVal	= -1;
	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= NULL;
	if( objItemBought->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBuyFromVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnBuyFromVendor, false );
	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnSellToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold )
//|	Date		-	26th November, 2011
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows determining what happens when an item is in the
//|					process of being sold to an NPC vendor
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnSellToVendor( CSocket *tSock, CChar *objVendor, CBaseObject *objItemSold )
{
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnSellToVendor, "onSellToVendor" ) )
		return false;

	SI08 funcRetVal	= -1;
	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= NULL;
	if( objItemSold->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSellToVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnSellToVendor, false );
	if( !( JSVAL_IS_NULL( rval ) ) )	// They returned some sort of value
	{
		if( JSVAL_IS_BOOLEAN( rval ) )
		{
			if( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE )
				funcRetVal = 0;		// we do want hard code to execute
			else
				funcRetVal = 1;		// we DON'T want hard code to execute
		}
		else
			funcRetVal = 0;	// default to hard code
	}
	else
		funcRetVal = 0;	// default to hard code

	return funcRetVal;
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
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemBought ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnBoughtFromVendor, "onBoughtFromVendor" ) )
		return false;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= NULL;
	if( objItemBought->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemBought, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onBoughtFromVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnBoughtFromVendor, false );
	return ( retVal == JS_TRUE );
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
	if( !ValidateObject( objVendor ) || !ValidateObject( objItemSold ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnSoldToVendor, "onSoldToVendor" ) )
		return false;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *charObj	= JSEngine->AcquireObject( IUE_CHAR, objVendor, runTime );
	JSObject *myObj2	= NULL;
	if( objItemSold->GetObjType() == OT_ITEM )
		myObj2 = JSEngine->AcquireObject( IUE_ITEM, objItemSold, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	params[2] = OBJECT_TO_JSVAL( myObj2 );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSoldToVendor", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnSoldToVendor, false );
	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 OnHouseCommand( CSocket *tSock, CMultiObj *objMulti )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding house commands via JS script attached to multi
//o-----------------------------------------------------------------------------------------------o
SI08 cScript::OnHouseCommand( CSocket *tSock, CMultiObj *objMulti, UI08 cmdID )
{
	if( !ValidateObject( objMulti ) || tSock == NULL )
		return false;
	if( !ExistAndVerify( seOnHouseCommand, "onHouseCommand" ) )
		return false;

	jsval rval, params[3];
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
	JSObject *multiObj	= JSEngine->AcquireObject( IUE_ITEM, objMulti, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = OBJECT_TO_JSVAL( multiObj );
	params[2] = INT_TO_JSVAL( cmdID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHouseCommand", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnHouseCommand, false );
	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnPathfindEnd( CChar *ncp, SI08 pathfindResult )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPCs after their pathfinding efforts come to and end
//|	Notes		-	pathfindResult gives a value that represents how the pathfinding ended				
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnPathfindEnd( CChar *npc, SI08 pathfindResult )
{
	if( !ValidateObject( npc ) )
		return false;
	if( !ExistAndVerify( seOnPathfindEnd, "onPathfindEnd" ) )
		return false;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, npc, runTime );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = INT_TO_JSVAL( pathfindResult );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onPathfindEnd", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnPathfindEnd, false );

	return ( retVal == JS_TRUE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool OnEnterEvadeState( CChar *npc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers for NPCs when they enter the evade state after failing to pathfind to
//|					a target in combat
//o-----------------------------------------------------------------------------------------------o
bool cScript::OnEnterEvadeState( CChar *npc, CChar *enemy )
{
	if( !ValidateObject( npc ) || !ValidateObject( enemy ))
		return false;
	if( !ExistAndVerify( seOnEnterEvadeState, "onEnterEvadeState" ) )
		return false;

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, npc, runTime );
	JSObject *enemyObj = JSEngine->AcquireObject( IUE_CHAR, enemy, runTime );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( enemyObj );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEnterEvadeState", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnEnterEvadeState, false );

	return ( retVal == JS_TRUE );
}