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

namespace UOX
{

// Version History
//	1.0		Abaddon		26th August, 2000
//			Initial implementation
//			The basic event model is in, along with some fairly simple error handling
//			Note that parameters for each event is NOT correct at all, it was just to get the basic layout right
//			Support for CS type stuff with IsFiring(), Firing(), and Stop()
//	1.1		Abaddon		6th September, 2000
//			Each function has params and passes it along into the the script as required
//	1.2		Abaddon		18th September, 2000
//			Addition of a number of more events, not all with params
//			More functions added to the functions table
//			Support for dynamic reloading of scripts
//	1.3		Abaddon		2nd November, 2000
//			Addition of a number of events.  Caching of function presence added
//			All functions assumed present, only failure on a call will flag not present
//	1.4		Maarc		1st July, 2004
//			Added private property of SCRIPTTYPE to store which type of script we've got
//			Preparation for JS scriptable spells

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
	{ "UseDoor",					SE_UseDoor,					2, 0, 0 },
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
	{ "CalcTargetedItem",			SE_CalcTargetedItem,		1, 0, 0 },
	{ "CalcTargetedChar",			SE_CalcTargetedChar,		1, 0, 0 },
	{ "GetTileIDAtMapCoord",		SE_GetTileIDAtMapCoord,		3, 0, 0 },
	{ "GetDictionaryEntry",			SE_GetDictionaryEntry,		2, 0, 0 },
	{ "Yell",						SE_Yell,					3, 0, 0 },
	
	// Added by DarkStorm
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
	{ "GetSocketFromIndex",			SE_GetSocketFromIndex,		1, 0, 0 },
	{ "StaticAt",					SE_StaticAt,				4, 0, 0 },
	{ "StaticInRange",				SE_StaticInRange,			5, 0, 0 },
	{ "ResourceArea",				SE_ResourceArea,			2, 0, 0 },
	{ "ResourceAmount",				SE_ResourceAmount,			2, 0, 0 },
	{ "ResourceTime",				SE_ResourceTime,			2, 0, 0 },
	{ "ResourceRegion",				SE_ResourceRegion,			3, 0, 0 },
 	
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

	{ NULL,							NULL,						0, 0, 0 }, 
};

void UOX3ErrorReporter( JSContext *cx, const char *message, JSErrorReport *report )
{
	UI16 scriptNum = JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	// If we're loading the world then do NOT print out anything!
	Console.Error( "JS script failure: Script Number (%u) Message (%s)", scriptNum, message );
	if( report == NULL || report->filename == NULL )
	{
		Console.Error( "No detailed data" );
		return;
	}
	Console.Error( "Filename: %s\n| Line Number: %i", report->filename, report->lineno );
	Console.Error( "Erroneous Line: %s\n| Token Ptr: %s", report->linebuf, report->tokenptr );
}

cScript::cScript( std::string targFile, UI08 rT ) : isFiring( false ), runTime( rT )
{
	for( int i = 0; i < 3; ++i )
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
		return;
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

//o--------------------------------------------------------------------------o
//|	Function		-	bool cScript::OnStart( void )
//|	Date			-	8/16/2003 3:44:50 AM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / MyndTrip Studios / EAWare
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	The OnStart event is provided to allow a script to process
//|						and read in any state information that has been saved from
//|						a previous server shut down. If a a script come with an 
//|						OnStart event the code that is provided will be executed
//|						just following the loading of the script.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cScript::OnStart( void )
{
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cScript::OnStop( void )
//|	Date			-	8/16/2003 3:44:44 AM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / MyndTrip Studios / EAWare
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	The OnStop event is provided to allow a script to perform
//|						any special cleanup, or state saving as a server shuts
//|						down. If a script has an OnStop event then any code that
//|						is provided will be executed just prior to the JSE shut
//|						down.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cScript::OnStop( void )
{ 
	return false;
}

bool cScript::OnCreate( CBaseObject *thingCreated, bool dfnCreated )
{
	if( !ValidateObject( thingCreated ) )
		return false;

	std::string functionName = "onCreateDFN";
	if( !dfnCreated )
		functionName = "onCreateTile";

	if( !ExistAndVerify( seOnCreate, functionName ) )
		return false;
	
	jsval rval, params[2];
	UI08 paramType = 0;
	JSObject *myObj;
	if( thingCreated->GetObjType() == OT_CHAR )
	{
		myObj = JSEngine->AcquireObject( IUE_CHAR, thingCreated, runTime );
		paramType = 1;
	}
	else
		myObj = JSEngine->AcquireObject( IUE_ITEM, thingCreated, runTime );

	params[0] = OBJECT_TO_JSVAL( myObj );
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, functionName.c_str(), 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnCreate, false );

	return ( retVal == JS_TRUE );
}

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


//o--------------------------------------------------------------------------o
//|	Function		-	SI08 cScript::OnSpeech( const char *speech, CChar *personTalking, CChar *talkingTo )
//|	Date			-	??????
//|	Developers		-	??????
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Version 3
//o--------------------------------------------------------------------------o
//|	Description		-	The function returns 4 possible values
//|						-1	=> No such function or bad call
//|						0	=> Let other NPCs and PCs see it
//|						1	=> Let other PCs see it
//|						2	=> Let no one else see it
//|						If JS returns non-int and non-bool, default to 0
//|						If JS returns bool, true == 2, false == 0
//o--------------------------------------------------------------------------o
//| Modifications	-	22 June, 2003 17:30 (Maarc, making it version 3)
//|						Changed return values from bool to SI08
//o--------------------------------------------------------------------------o
SI08 cScript::OnSpeech( const char *speech, CChar *personTalking, CChar *talkingTo )
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
	JSObject *ttObj = JSEngine->AcquireObject( IUE_CHAR, talkingTo, runTime );

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

bool cScript::OnDispel( CBaseObject *dispelled )
{
	if( !ValidateObject( dispelled ) )
		return false;
	if( !ExistAndVerify( seOnDispel, "onDispel" ) )
		return false;

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
		SetEventExists( seOnDispel, false );

	return ( retVal == JS_TRUE );
}

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

bool cScript::OnPickup( CItem *item, CChar *pickerUpper )
{
	if( !ValidateObject( item ) || !ValidateObject( pickerUpper ) )
		return false;
	if( !ExistAndVerify( seOnPickup, "onPickup" ) )
		return false;

	jsval params[2], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pickerUpper, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, item, runTime );

	params[0] = OBJECT_TO_JSVAL( itemObj );
	params[1] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onPickup", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnPickup, false );

	return ( retVal == JS_TRUE );
}

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

//o--------------------------------------------------------------------------o
//|	Function		-	SI08 cScript::OnUse( CChar *user, CItem *iUsing )
//|	Date			-	??????
//|	Developers		-	??????
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Version 3
//o--------------------------------------------------------------------------o
//|	Description		-	The function returns 3 possible values
//|						-1	=> No such function or bad call
//|						0	=> Execute hard coded implementations as well
//|						1	=> Don't execute hard coded implementation
//o--------------------------------------------------------------------------o
//| Modifications	-	31 July, 2003 15:39 (Maarc, making it version 3)
//|						Changed return values from bool to SI08
//o--------------------------------------------------------------------------o
SI08 cScript::OnUse( CChar *user, CItem *iUsing )
{
	if( !ValidateObject( user ) || !ValidateObject( iUsing ) )
		return -1;
	if( !ExistAndVerify( seOnUse, "onUse" ) )
		return -1;

	SI08 funcRetVal	= -1;

	jsval rval, params[2];

	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, user, runTime );
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, iUsing, runTime );

	params[0] = OBJECT_TO_JSVAL( charObj );
	params[1] = OBJECT_TO_JSVAL( itemObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUse", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUse, false );

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

//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item)
//|	Date			-	04/18/2002
//|	Developer(s)	-	MACTEP
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//|	Modified		-	V2 - Maarc
//|						Returns 
//|							0 if no function exists
//|							1 if should bounce
//|							2 if should not bounce and use code
//|							3 if should not bounce and not use code
//o--------------------------------------------------------------------------o
//|	Description		-	Event to signal when an item is dropped on an NPC
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
UI08 cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( dstChar ) || !ValidateObject( item ) )
		return 0;
	if( !ExistAndVerify( seOnDropItemOnNpc, "onDropItemOnNpc" ) )
		return 0;
		
	jsval rval, params[2];

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

//o--------------------------------------------------------------------------o
//|	Function/Class	-	
//|	Date			-	10/06/2002
//|	Developer(s)	-	Brakhtus
//|	Company/Team	-	
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o
//|	Notes			-	
//o--------------------------------------------------------------------------o	
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

//o--------------------------------------------------------------------------o
//|	Function/Class	-	
//|	Date			-	10/06/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o
//|	Notes			-	
//o--------------------------------------------------------------------------o	
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

bool cScript::OnDeath( CChar *pDead )
{
	if( !ValidateObject( pDead ) )
		return false;
	if( !ExistAndVerify( seOnDeath, "onDeath" ) )
		return false;

	jsval params[1], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pDead, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeath", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDeath, false );

	return ( retVal == JS_TRUE );
}

bool cScript::OnResurrect( CChar *pAlive )
{
	if( !ValidateObject( pAlive ) )
		return false;
	if( !ExistAndVerify( seOnResurrect, "onResurrect" ) )
		return false;

	jsval params[1], rval;
	JSObject *charObj = JSEngine->AcquireObject( IUE_CHAR, pAlive, runTime );
	params[0] = OBJECT_TO_JSVAL( charObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onResurrect", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnResurrect, false );

	return ( retVal == JS_TRUE );
}

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
	params[1] = INT_TO_JSVAL( oldStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFlagChange", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnFlagChange, false );

	return ( retVal == JS_TRUE );
}

bool cScript::DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
{
	if( tSock == NULL )
		return false;
	jsval params[2], rval;
	int objType			= 2;	// 2 == null, 1 == char, 0 == item
	CBaseObject *mObj	= NULL;
	JSObject *myObj2	= NULL;
	JSObject *myObj		= JSEngine->AcquireObject( IUE_SOCK, tSock, runTime );
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
	char targetFunction[32];
	sprintf( targetFunction, "onCallback%i", callNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, targetFunction, 2, params, &rval );
	return ( retVal == JS_TRUE );
}

JSObject *cScript::Object( void ) const
{
	return targObject;
}

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

bool cScript::OnSnooped( CChar *snooped, CChar *snooper, bool success )
{
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ) )
		return false;
	if( !ExistAndVerify( seOnSnooped, "onSnooped" ) )
		return false;

	jsval params[3], rval;

	JSObject *thiefObj	= JSEngine->AcquireObject( IUE_CHAR, snooped, runTime );
	JSObject *victimObj	= JSEngine->AcquireObject( IUE_CHAR, snooper, runTime );

	params[0] = OBJECT_TO_JSVAL( snooped );
	params[1] = OBJECT_TO_JSVAL( snooper );
	params[2] = BOOLEAN_TO_JSVAL( success );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSnooped", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSnooped, false );

	return ( retVal == JS_TRUE );
}

size_t cScript::NewGumpList( void )
{
	size_t retVal	= gumpDisplays.size();
	SEGump *toAdd	= new SEGump;
	toAdd->one		= new STRINGLIST;
	toAdd->two		= new STRINGLIST;

	gumpDisplays.push_back( toAdd );
	return retVal;
}
SEGump * cScript::GetGumpList( SI32 index )
{
	if( index < 0 || (size_t)index >= gumpDisplays.size() )
		return NULL;
	return gumpDisplays[index];
}
void cScript::RemoveGumpList( SI32 index )
{
	if( index < 0 || (size_t)index >= gumpDisplays.size() )
		return;

	delete gumpDisplays[index]->one;
	delete gumpDisplays[index]->two;
	delete gumpDisplays[index];

	gumpDisplays.erase( gumpDisplays.begin() + index );
}
void cScript::SendGumpList( SI32 index, CSocket *toSendTo )
{
	if( index < 0 || (size_t)index >= gumpDisplays.size() )
		return;

	toSendTo->TempInt( (SI32)JSMapping->GetScript( targObject ) );
	SendVecsAsGump( toSendTo, *(gumpDisplays[index]->one), *(gumpDisplays[index]->two), 20, INVALIDSERIAL );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cScript::HandleGumpPress( CSocket *pressing, long button )
//|	Date			-	
//|	Developers		-	Unknown / Punt
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Process a gump button press
//|									
//|	Modification	-	1/21/2003 - Implemented the code needed to support enhanced
//|									gump response processing
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
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

bool cScript::OnEnterRegion( CChar *entering, SI16 region )
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

bool cScript::OnLeaveRegion( CChar *leaving, SI16 region )
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

//o---------------------------------------------------------------------------o
//|	Function	-	bool CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o---------------------------------------------------------------------------o
bool cScript::CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams )
{
	if( eventToCall == NULL )
		return false;

	jsval rval;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, &rval );
	return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
}

//	
//	Return value table
//	-2: use NORMAL non-JS casting
//	-1: CANCEL spellcasting
//	0->inf: Spell delay in ms
//	
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

//	
//	Return value table
//	-2: use NORMAL non-JS casting
//	-1: CANCEL spellcasting
//	0->inf: Spell delay in ms
//	
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

// This function is called out of network.cpp
// if a speechmode(9) was previously set
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

bool cScript::OnSpellGain( CItem *book, const UI08 spellNum )
{
	if( !ValidateObject( book ) )
		return false;
	if( !ExistAndVerify( seOnSpellGain, "onSpellGain" ) )
		return false;

	jsval params[2], rval;
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, book, runTime );
	params[0] = OBJECT_TO_JSVAL( book );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellGain", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellGain, false );

	return ( retVal == JS_TRUE );
}
bool cScript::OnSpellLoss( CItem *book, const UI08 spellNum )
{
	if( !ValidateObject( book ) )
		return false;
	if( !ExistAndVerify( seOnSpellLoss, "onSpellLoss" ) )
		return false;

	jsval params[2], rval;
	JSObject *itemObj = JSEngine->AcquireObject( IUE_ITEM, book, runTime );
	params[0] = OBJECT_TO_JSVAL( book );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellLoss, false );

	return ( retVal == JS_TRUE );
}

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

//o--------------------------------------------------------------------------
//|	Function	-	bool AreaObjFunc( char *funcName, CChar *srcChar, CBaseObject *tmpObject, CSocket *s )
//|	Date		-	January 27, 2003
//|	Programmer	-	Maarc
//|	Modified	-	August 17 2005 (Maarc)
//|						Renamed to AreaObjFunc from AreaCharFunc
//|						Added support for other object types
//o--------------------------------------------------------------------------
//|	Purpose		-	Calls the function represented in funcName for the script
//|				-	passing in two character parameters
//o--------------------------------------------------------------------------
bool cScript::AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s )
{
	if( !ValidateObject( srcObject ) || !ValidateObject( tmpObject ) || funcName == NULL )
		return false;

	jsval params[3], rval;

	JSObject *srcObj;
	JSObject *tmpObj;
	
	if( srcObject->CanBeObjType( OT_ITEM ) )
		srcObj = JSEngine->AcquireObject( IUE_ITEM, srcObject, runTime );
	else if( srcObject->CanBeObjType( OT_CHAR ) )
		srcObj = JSEngine->AcquireObject( IUE_CHAR, srcObject, runTime );

	if( tmpObject->CanBeObjType( OT_ITEM ) )
		tmpObj = JSEngine->AcquireObject( IUE_ITEM, tmpObject, runTime );
	else if( tmpObject->CanBeObjType( OT_CHAR ) )
		tmpObj = JSEngine->AcquireObject( IUE_CHAR, tmpObject, runTime );

	params[0]			= OBJECT_TO_JSVAL( srcObj );
	params[1]			= OBJECT_TO_JSVAL( tmpObj );
	JSObject *sockObj	= NULL;

	if( s != NULL )
	{
		sockObj		= JSEngine->AcquireObject( IUE_SOCK, s, runTime );
		params[2]	= OBJECT_TO_JSVAL( sockObj );
	}
	else
		params[2]	= JSVAL_NULL;
	// ExistAndVerify() normally sets our Global Object, but not on custom named functions.
	JS_SetGlobalObject( targContext, targObject );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );

	return ( JSVAL_TO_BOOLEAN( rval ) == JS_TRUE );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cScript::OnCommand( CSocket *CSocket )
//|	Date			-	1/13/2003 11:17:48 PM
//|	Developers		-	Brakhtus
//|	Organization	-	Forum Submission
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cScript::OnCommand( CSocket *mSock ) 
{ 	
	if( mSock  == NULL ) 		
		return false;
	if( !ExistAndVerify( seOnCommand, "onCommand" ) )
		return false;

	jsval params[1], rval; 	
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );	
	params[0]		= OBJECT_TO_JSVAL( mSock ); 	
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onCommand", 1, params, &rval ); 	
	if( retVal == JS_FALSE ) 		
		SetEventExists( seOnCommand, false );

	return ( retVal == JS_TRUE ); 
}

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

//o---------------------------------------------------------------------------o
//|	Function	-	bool CallParticularEvent( char *eventToCall, jsval *params, SI32 numParams )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o---------------------------------------------------------------------------o
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
		Console.Warning( "Script Number (%u) does not have a %s function", JSMapping->GetScriptID( targObject ), scriptType.c_str() );
		return false;
	}

	JSBool retVal = JS_CallFunctionName( targContext, targObject, scriptType.c_str(), 0, params, &rval );
	return ( retVal == JS_TRUE );
}

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

bool cScript::MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, int spellNum )
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

bool cScript::OnPacketReceive( CSocket *mSock, UI16 packetNum )
{
	if( mSock == NULL )
		return false;
	if( !ExistAndVerify( seOnPacketReceive, "onPacketReceive" ) )
		return false;

	jsval rval, params[3];
	JSObject *myObj = JSEngine->AcquireObject( IUE_SOCK, mSock, runTime );
	params[0]		= OBJECT_TO_JSVAL( mSock );
	params[1]		= INT_TO_JSVAL( (UI08)(packetNum%256) );
	params[2]		= INT_TO_JSVAL( (UI08)(packetNum>>8) );
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onPacketReceive", 3, params, &rval );

	if( retVal == JS_FALSE )
		SetEventExists( seOnPacketReceive, false );

	return ( retVal == JS_TRUE );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI08 OnCharDoubleClick( CChar *currChar, CChar *targChar )
//|	Programmer	-	Xuri
//|	Date		-	23rd January, 2006
//o---------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding events that happen when doubleclicking,
//|				-	characters, such as open paperdoll, mounting horses, etc
//o---------------------------------------------------------------------------o

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

//o---------------------------------------------------------------------------o
//|	Function	-	SI08 OnSkillGump( CSocket *mSock )
//|	Programmer	-	Xuri
//|	Date		-	23rd January, 2006
//o---------------------------------------------------------------------------o
//|	Purpose		-	Allows overriding the client's request to open the default
//|					skillgump, and instead do something else (like opening a
//|					custom skillgump instead).
//o---------------------------------------------------------------------------o

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

bool cScript::OnDeathBlow( CChar *mKilled, CChar *mKiller )
{
	if( !ValidateObject( mKilled ) || !ValidateObject( mKiller ) )
		return false;
	if( !ExistAndVerify( seOnDeathBlow, "onDeathBlow" ) )
		return false;

	jsval rval, params[2];
	JSObject *killedObj = JSEngine->AcquireObject( IUE_CHAR, mKilled, runTime );
	JSObject *killerObj = JSEngine->AcquireObject( IUE_CHAR, mKiller, runTime );

	params[0] = OBJECT_TO_JSVAL( killedObj );
	params[1] = OBJECT_TO_JSVAL( killerObj );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeathBlow", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDeathBlow, false );

	return ( retVal == JS_TRUE );
}

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

}
