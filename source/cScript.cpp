#include "uox3.h"
#include "cScript.h"
#include "power.h"
#include "UOXJSClasses.h"
#include "SEFunctions.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"
#include "CJSMapping.h"
#include "CPacketReceive.h"

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
	{ "GetCommand",					SE_GetCommand,				1, 0, 0 }, 
	{ "GetCommandSize",				SE_GetCommandSize,			0, 0, 0 }, 	
	{ "StringToNum",				SE_StringToNum,				1, 0, 0 },
	{ "NumToString",				SE_NumToString,				1, 0, 0 },
	{ "NumToHexString",				SE_NumToHexString,			1, 0, 0 },
	{ "DoMovingEffect", 			SE_DoMovingEffect,			6, 0, 0 },
	{ "BroadcastMessage",			SE_BroadcastMessage,		1, 0, 0 },
	{ "ConsoleMessage",				SE_ConsoleMessage,			1, 0, 0 }, 
	{ "ScriptPrintNumber",			SE_ScriptPrintNumber,		1, 0, 0 },
	{ "RandomNumber",				SE_RandomNumber,			2, 0, 0 },
	{ "CalcCharFromSer",			SE_CalcCharFromSer,			1, 0, 0 },
	{ "CalcItemFromSer",			SE_CalcItemFromSer,			1, 0, 0 },
	{ "GetHour",					SE_GetHour,					0, 0, 0 },
	{ "GetMinute",					SE_GetMinute,				0, 0, 0 },
	{ "GetDay",						SE_GetDay,					0, 0, 0 },
	{ "GetSecondsPerUOMinute",		SE_SecondsPerUOMinute,		0, 0, 0 },
	{ "GetCurrentClock",			SE_GetCurrentClock,			0, 0, 0 },
	{ "SubStringSearch",			SE_SubStringSearch,			2, 0, 0 },
	{ "GetMurderThreshold",			SE_GetMurderThreshold,		0, 0, 0 },
	{ "RollDice",					SE_RollDice,				3, 0, 0 },
	{ "RaceCompareByRace",			SE_RaceCompareByRace,		2, 0, 0 },
	{ "DoTempEffect",				SE_DoTempEffect,			7, 0, 0 },
	{ "MakeItem",					SE_MakeItem,				3, 0, 0 },
	{ "FindMulti",					SE_FindMulti,				4, 0, 0 },
	{ "IsRegionGuarded",			SE_IsRegionGuarded,			1, 0, 0 },
	{ "CanMarkInRegion",			SE_CanMarkInRegion,			1, 0, 0 },
	{ "CanGateInRegion",			SE_CanGateInRegion,			1, 0, 0 },
	{ "CanRecallInRegion",			SE_CanRecallInRegion,		1, 0, 0 },
	{ "GetTownMayor",				SE_GetTownMayor,			1, 0, 0 },
	{ "GetTownRace",				SE_GetTownRace,				1, 0, 0 },
	{ "SetTownRace",				SE_SetTownRace,				2, 0, 0 },
	{ "PossessTown",				SE_PossessTown,				2, 0, 0 },
	{ "GetTownTaxResource",			SE_GetTownTaxResource,		1, 0, 0 },
	{ "IsRaceWeakToWeather",		SE_IsRaceWeakToWeather,		2, 0, 0 },
	{ "GetRaceSkillAdjustment",		SE_GetRaceSkillAdjustment,	2, 0, 0 },
	{ "UseDoor",					SE_UseDoor,					2, 0, 0 },
	{ "GetGuildType",				SE_GetGuildType,			1, 0, 0 },
	{ "GetGuildName",				SE_GetGuildName,			1, 0, 0 },
	{ "SetGuildType",				SE_SetGuildType,			2, 0, 0 },
	{ "SetGuildName",				SE_SetGuildName,			2, 0, 0 },
	{ "SetGuildMaster",				SE_SetGuildMaster,			2, 0, 0 },
	{ "GetNumGuildMembers",			SE_GetNumGuildMembers,		2, 0, 0 },
	{ "CompareGuildByGuild",		SE_CompareGuildByGuild,		2, 0, 0 },
	{ "GetGuildStone",				SE_GetGuildStone,			1, 0, 0 },
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
	{ "RaceGate",					SE_RaceGate,				2, 0, 0 },
	{ "Yell",						SE_Yell,					3, 0, 0 },
	
	// Added by DarkStorm
	{ "GetRaceCount",				SE_GetRaceCount,			0, 0, 0 },

	{ "WorldBrightLevel",			SE_WorldBrightLevel,		0, 0, 0 },
	{ "WorldDarkLevel",				SE_WorldDarkLevel,			0, 0, 0 },
	{ "WorldDungeonLevel",			SE_WorldDungeonLevel,		0, 0, 0 },
	{ "AreaCharacterFunction",		SE_AreaCharacterFunction,	3, 0, 0 },
	{ "TriggerEvent",				SE_TriggerEvent,			3, 0, 0 },
	{ "Reload",						SE_Reload,					1, 0, 0 },
	{ "SendStaticStats",			SE_SendStaticStats,			1, 0, 0 },
	{ "GetTileHeight",				SE_GetTileHeight,			1, 0, 0 },
	{ "IterateOver",				SE_IterateOver,				1, 0, 0 },
	{ "GetSocketFromIndex",			SE_GetSocketFromIndex,		1, 0, 0 },
 	
	{ "RegisterCommand",			SE_RegisterCommand,			3, 0, 0 },
	{ "DisableCommand",				SE_DisableCommand,			1, 0, 0 },
	{ "EnableCommand",				SE_EnableCommand,			1, 0, 0 },

	{ "RegisterSpell",				SE_RegisterSpell,			2, 0, 0 },
	{ "DisableSpell",				SE_DisableSpell,			1, 0, 0 },
	{ "EnableSpell",				SE_EnableSpell,				1, 0, 0 },

	{ NULL,							NULL,						0, 0, 0 }, 
};

void UOX3ErrorReporter( JSContext *cx, const char *message, JSErrorReport *report )
{
	// If we're loading the world then do NOT print out anything!
//	if( cwmWorldState->GetLoaded() != true ) 
//		return;

	Console.Error( 2, "JS script failure: Message (%s), detailed data is", message );
	if( report == NULL )
	{
		Console.Error( 2, "No detailed data" );
		return;
	}
	Console.Error( 2, "Filename: %s\nLine Number: %i", report->filename, report->lineno );
	Console.Error( 2, "Erroneous Line: %s\nToken Ptr: %s", report->linebuf, report->tokenptr );
}

cScript::cScript( std::string targFile, SCRIPTTYPE sType ) : isFiring( false ), scpType( sType )
{
	eventPresence[0] = eventPresence[1] = 0xFFFFFFFF;
	needsChecking[0] = needsChecking[1] = 0xFFFFFFFF;
	targContext = JS_NewContext( jsRuntime, 0x2000 );
	if( targContext == NULL )
		return;
	targObject = JS_NewObject( targContext, &uox_class, NULL, NULL ); 
	if( targObject == NULL )
	{
		JS_DestroyContext( targContext );
		targContext = NULL;//crash fix
		return;
	}

	// Moved here so it reports errors during script-startup too
	JS_SetErrorReporter( targContext, UOX3ErrorReporter );

	JS_InitStandardClasses( targContext, targObject );
	JS_DefineFunctions( targContext, targObject, my_functions );
	targScript = JS_CompileFile( targContext, targObject, targFile.c_str() );
	if( targScript == NULL )
	{
		JS_DestroyContext( targContext );
		targContext = NULL;//crash fix
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

	raceObjects.resize( 0 );
	charObjects.resize( 0 );
	itemObjects.resize( 0 );
	sockObjects.resize( 0 );
	guildObjects.resize( 0 );
	regionObjects.resize( 0 );
	gumpDisplays.resize( 0 );

	// Expose Classes to the JS Engine
	// We may NOT define Constructors here
	GumpProto			=	JS_InitClass( targContext, targObject, targObject, &UOXGump_class, Gump, 0, NULL, CGump_Methods, NULL, CGump_Methods );
	GumpDataProto		=	JS_InitClass( targContext, targObject, targObject, &UOXGumpData_class, NULL, 0, NULL, CGumpData_Methods, NULL,CGumpData_Methods);
	CharProto			=	JS_InitClass( targContext, targObject, targObject, &UOXChar_class, NULL, 0, CCharacterProps, CChar_Methods, CCharacterProps, CChar_Methods );
	ItemProto			=	JS_InitClass( targContext, targObject, targObject, &UOXItem_class, NULL, 0, CItemProps, CItem_Methods, CItemProps, CItem_Methods );
	SpellProto			=	JS_InitClass( targContext, targObject, targObject, &UOXSpell_class, NULL, 0, CSpellProperties, NULL, CSpellProperties, NULL );
	SpellsProto			=	JS_InitClass( targContext, targObject, targObject, &UOXSpells_class, NULL, 0, NULL, NULL, NULL, NULL );
	SocketProto			=	JS_InitClass( targContext, targObject, targObject, &UOXSocket_class, NULL, 0, CSocketProps, CSocket_Methods, CSocketProps, CSocket_Methods );
	UOXCFileProto		=	JS_InitClass( targContext, targObject, targObject, &UOXFile_class, UOXCFile, 0, CFileProperties, CFile_Methods, CFileProperties, CFile_Methods );
	CAccountProto		=	JS_InitClass( targContext, targObject, targObject, &UOXAccount_class, NULL, 0, CAccountProperties, CAccount_Methods, CAccountProperties, CAccount_Methods );
	CConsoleProto		=	JS_InitClass( targContext, targObject, targObject, &UOXConsole_class, NULL, 0, CConsoleProperties, CConsole_Methods, CConsoleProperties, CConsole_Methods );
	RegionProto			=	JS_InitClass( targContext, targObject, targObject, &UOXRegion_class, NULL, 0, CRegionProperties, NULL, CRegionProperties, NULL );
	// Init the global Spells[] object
	JS_DefineObject( targContext, targObject, "Spells", &UOXSpells_class, SpellsProto, 0 );
	JS_DefineObject( targContext, targObject, "Accounts", &UOXAccount_class, CAccountProto, 0 );
	JS_DefineObject( targContext, targObject, "Console", &UOXConsole_class, CConsoleProto, 0 );

	// let's acquire items 0 and 1, and chars 0 and 1, by default, so that
	// we can reuse these for parameter stuff
	AcquireObject( IUE_CHAR );
	AcquireObject( IUE_CHAR );
	AcquireObject( IUE_ITEM );
	AcquireObject( IUE_ITEM );
	AcquireObject( IUE_SOCK );
}

void cScript::Cleanup( void )
{
	size_t i = 0;
	for( i = 0; i < gumpDisplays.size(); ++i )
		delete gumpDisplays[i];
	gumpDisplays.resize( 0 );

	// This was the body of RemoveFromRoot, but we're doing Cleanup separately

	for( i = 0; i < raceObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, raceObjects[i].toUse );
	}

	for( i = 0; i < charObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, charObjects[i].toUse );
	}

	for( i = 0; i < itemObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, itemObjects[i].toUse );
	}

	for( i = 0; i < sockObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, sockObjects[i].toUse );
	}

	for( i = 0; i < guildObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, guildObjects[i].toUse );
	}

	for( i = 0; i < regionObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, regionObjects[i].toUse );
	}

	raceObjects.resize( 0 );
	charObjects.resize( 0 );
	itemObjects.resize( 0 );
	sockObjects.resize( 0 );
	guildObjects.resize( 0 );
	regionObjects.resize( 0 );
}
cScript::~cScript()
{
	RemoveFromRoot();
	JS_GC( targContext );

	// Need count, command, and magic clean up code now that cTriggerScripts is gone.
	switch( scpType )
	{
	default:
	case SCPT_NORMAL:
	case SCPT_COUNT:		//Trigger->UnregisterObject( targObject );		break;
	case SCPT_COMMAND:	//Trigger->GetCommandScripts()->UnregisterObject( targObject );	break;
	case SCPT_MAGIC:		//Trigger->GetMagicScripts()->UnregisterObject( targObject );	break;
		break;
	}
	if( targScript != NULL )
		JS_DestroyScript( targContext, targScript );
	JS_GC( targContext );
	if( targContext != NULL )
		JS_DestroyContext( targContext );
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

bool cScript::OnCreate( CBaseObject *thingCreated )
{
	if( !ValidateObject( thingCreated ) )
		return false;
	if( !ExistAndVerify( seOnCreate, "onCreate" ) )
		return false;
	
	jsval rval, params[2];
	UI08 paramType = 0;
	if( thingCreated->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, thingCreated );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		paramType = 1;
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, thingCreated );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCreate", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnCreate, false );
	if( thingCreated->GetObjType() != OT_CHAR )
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	
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
	if( thingDestroyed->GetObjType() != OT_CHAR )
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, thingDestroyed );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}
	else
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, thingDestroyed );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		paramType = 1;
	}
	params[1] = INT_TO_JSVAL( paramType );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDelete", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDelete, false );
	if( thingDestroyed->GetObjType() != OT_CHAR )
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
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
	
	char *lwrSpeech = new char[strlen(speech)+1];
	strcpy( lwrSpeech, speech );
	strlwr( lwrSpeech );

	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );

	JS_SetPrivate( targContext, charObjects[0].toUse, personTalking );
	JS_SetPrivate( targContext, charObjects[1].toUse, talkingTo );

	params[0] = STRING_TO_JSVAL( strSpeech );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[2] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpeech", 3, params, &rval );

	delete[] lwrSpeech;

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );

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

bool cScript::InRange( CChar *person, CChar *targPlayer )
{
	if( !ValidateObject( person ) || !ValidateObject( targPlayer ) )
		return false;
	if( !ExistAndVerify( seInRange, "inRange" ) )
		return false;

	jsval params[3], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, person );
	JS_SetPrivate( targContext, charObjects[1].toUse, targPlayer );
	
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	params[2] = INT_TO_JSVAL( 0 );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "inRange", 3, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	
	if( retVal == JS_FALSE )
		SetEventExists( seInRange, false );
	return ( retVal == JS_TRUE );
}
bool cScript::InRange( CChar *person, CItem *targItem )
{
	if( !ValidateObject( person ) || !ValidateObject( targItem ) )
		return false;
	if( !ExistAndVerify( seInRange, "inRange" ) )
		return false;

	jsval params[3], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, person );
	JS_SetPrivate( targContext, itemObjects[0].toUse, targItem );
	
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[2] = INT_TO_JSVAL( 1 );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "inRange", 3, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );

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
	JS_SetPrivate( targContext, sockObjects[0].toUse, tSock );
	JS_SetPrivate( targContext, charObjects[0].toUse, objColliding );
	if( objCollideWith->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[1].toUse, objCollideWith );
	else
		JS_SetPrivate( targContext, itemObjects[0].toUse, objCollideWith );

	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[2] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onCollide", 3, params, &rval );

	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	if( objCollideWith->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );

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

	JS_SetPrivate( targContext, charObjects[0].toUse, thief );
	JS_SetPrivate( targContext, itemObjects[0].toUse, theft );

	jsval params[2], rval;
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSteal", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSteal, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnDispel( CBaseObject *dispelled )
{
	if( !ValidateObject( dispelled ) )
		return false;
	if( !ExistAndVerify( seOnDispel, "onDispel" ) )
		return false;

	jsval params[2], rval;
	if( dispelled->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, dispelled );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		params[1] = INT_TO_JSVAL( 0 );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, dispelled );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
		params[1] = INT_TO_JSVAL( 1 );
	}

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDispel", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDispel, false );
	if( dispelled->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSkill( CBaseObject *skillUse, SI08 skillUsed )
{
	if( !ValidateObject( skillUse ) )
		return false;
	if( !ExistAndVerify( seOnSkill, "onSkill" ) )
		return false;

	jsval rval, params[3];
	if( skillUse->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, skillUse );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, skillUse );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
		params[2] = INT_TO_JSVAL( 1 );
	}
	params[1] = INT_TO_JSVAL( skillUsed );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkill", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkill, false );
	if( skillUse->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnAttack( CChar *attacker, CChar *defender )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return false;
	if( !ExistAndVerify( seOnAttack, "onAttack" ) )
		return false;

	jsval rval, params[2];
	JS_SetPrivate( targContext, charObjects[0].toUse, attacker );
	JS_SetPrivate( targContext, charObjects[1].toUse, defender );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAttack", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnAttack, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnDefense( CChar *attacker, CChar *defender )
{
	if( !ValidateObject( attacker ) || !ValidateObject( defender ) )
		return false;
	if( !ExistAndVerify( seOnDefense, "onDefense" ) )
		return false;

	jsval rval, params[2];
	JS_SetPrivate( targContext, charObjects[0].toUse, attacker );
	JS_SetPrivate( targContext, charObjects[1].toUse, defender );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDefense", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDefense, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSkillGain( CChar *player, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnSkillGain, "onSkillGain" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillGain", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillGain, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnStatGained( CChar *player, UI32 stat, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatGained, "onStatGained" ) )
		return false;

	jsval rval, params[3];
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatGained", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatGained, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnStatGain( CChar *player, UI32 stat, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatGain, "onStatGain" ) )
		return false;

	jsval rval, params[3];
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( stat );
	params[2] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatGain", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatGain, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnDrop( CItem *item, CChar *dropper )
{
	if( !ValidateObject( item ) || !ValidateObject( dropper ) )
		return false;
	if( !ExistAndVerify( seOnDrop, "onDrop" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, dropper );
	JS_SetPrivate( targContext, itemObjects[0].toUse, item );

	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDrop", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDrop, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );

	return ( retVal == JS_TRUE );
}

bool cScript::OnPickup( CItem *item, CChar *pickerUpper )
{
	if( !ValidateObject( item ) || !ValidateObject( pickerUpper ) )
		return false;
	if( !ExistAndVerify( seOnPickup, "onPickup" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pickerUpper );
	JS_SetPrivate( targContext, itemObjects[0].toUse, item );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onPickup", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnPickup, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg )
{
	if( !ValidateObject( swinging ) || !ValidateObject( swinger ) || !ValidateObject( swingTarg ) )
		return false;
	if( !ExistAndVerify( seOnSwing, "onSwing" ) )
		return false;

	JS_SetPrivate( targContext, itemObjects[0].toUse, swinging );
	JS_SetPrivate( targContext, charObjects[0].toUse, swinger );
	JS_SetPrivate( targContext, charObjects[1].toUse, swingTarg );

	jsval params[3], rval;
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[2] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSwing", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSwing, false );

	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnDecay( CItem *decaying )
{
	if( !ValidateObject( decaying ) )
		return false;
	if( !ExistAndVerify( seOnDecay, "onDecay" ) )
		return false;

	jsval params[1], rval;
	JS_SetPrivate( targContext, itemObjects[0].toUse, decaying );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDecay", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDecay, false );

	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnTransfer( CItem *transferred, CChar *source, CChar *target )
{
	if( !ValidateObject( transferred ) || !ValidateObject( source ) || !ValidateObject( target ) )
		return false;
	if( !ExistAndVerify( seOnTransfer, "onTransfer" ) )
		return false;

	jsval params[3], rval;
	JS_SetPrivate( targContext, itemObjects[0].toUse, transferred );
	JS_SetPrivate( targContext, charObjects[0].toUse, source );
	JS_SetPrivate( targContext, charObjects[1].toUse, target );

	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[2] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTransfer", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnTransfer, false );

	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );

	return ( retVal == JS_TRUE );
}

bool cScript::OnLeaving( CMultiObj *left, CBaseObject *leaving )
{
	if( !ValidateObject( left ) || !ValidateObject( leaving ) )
		return false;
	if( !ExistAndVerify( seOnLeaving, "onLeaving" ) )
		return false;

	jsval params[3], rval;
	if( leaving->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, leaving );
		params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[1].toUse, leaving );
		params[1] = OBJECT_TO_JSVAL( itemObjects[1].toUse );
		params[2] = INT_TO_JSVAL( 1 );
	}
	JS_SetPrivate( targContext, itemObjects[0].toUse, left );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLeaving", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnLeaving, false );

	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	if( leaving->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[1].toUse, NULL );

	return ( retVal == JS_TRUE );
}

bool cScript::OnEquip( CChar *equipper, CItem *equipping )
{
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return false;

	if( !ExistAndVerify( seOnEquip, "onEquip" ) )
		return false;

	jsval rval, params[2];
	JS_SetPrivate( targContext, charObjects[0].toUse, equipper );
	JS_SetPrivate( targContext, itemObjects[0].toUse, equipping );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEquip", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnEquip, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnUnequip( CChar *equipper, CItem *equipping )
{
	if( !ValidateObject( equipper ) || !ValidateObject( equipping ) )
		return false;
	if( !ExistAndVerify( seOnUnequip, "onUnequip" ) )
		return false;

	jsval rval, params[2];
	JS_SetPrivate( targContext, charObjects[0].toUse, equipper );
	JS_SetPrivate( targContext, itemObjects[0].toUse, equipping );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUnequip", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUnequip, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
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

	JS_SetPrivate( targContext, charObjects[0].toUse, user );
	JS_SetPrivate( targContext, itemObjects[0].toUse, iUsing );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUse", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUse, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );

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
UI08 cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item)
{
	if( !ValidateObject( srcChar ) || !ValidateObject( dstChar ) || !ValidateObject( item ) )
		return 0;
	if( !ExistAndVerify( seOnDropItemOnNpc, "onDropItemOnNpc" ) )
		return 0;
		
	jsval rval, params[2];

	JS_SetPrivate( targContext, charObjects[0].toUse, srcChar );
	JS_SetPrivate( targContext, charObjects[1].toUse, dstChar );
	JS_SetPrivate( targContext, itemObjects[0].toUse, item );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	params[2] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDropItemOnNpc", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDropItemOnNpc, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );

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
	if( leaving->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, leaving );
		params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[1].toUse, leaving );
		params[1] = OBJECT_TO_JSVAL( itemObjects[1].toUse );
		params[2] = INT_TO_JSVAL( 1 );
	}
	JS_SetPrivate( targContext, itemObjects[0].toUse, left );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEntrance", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnEntrance, false );

	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	if( leaving->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[1].toUse, NULL );

	return ( retVal == JS_TRUE );
}

bool cScript::OutOfRange( CChar *person, CBaseObject *objVanish )
{
	if( !ValidateObject( person ) || !ValidateObject( objVanish ) )
		return false;
	if( !ExistAndVerify( seOutOfRange, "outOfRange" ) )
		return false;

	jsval params[3], rval;
	if( objVanish->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[1].toUse, objVanish );
		params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
		params[2] = INT_TO_JSVAL( 0 );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[1].toUse, objVanish );
		params[1] = OBJECT_TO_JSVAL( itemObjects[1].toUse );
		params[2] = INT_TO_JSVAL( 1 );
	}
	JS_SetPrivate( targContext, charObjects[0].toUse, person );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "outOfRange", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOutOfRange, false );

	if( objVanish->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnLogin( CSocket *sockPlayer, CChar *pPlayer )
{
	if( !ValidateObject( pPlayer ) )
		return false;
	if( !ExistAndVerify( seOnLogin, "onLogin" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, sockObjects[0].toUse, sockPlayer );
	JS_SetPrivate( targContext, charObjects[0].toUse, pPlayer );

	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLogin", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnLogin, false );

	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
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

	JS_SetPrivate( targContext, sockObjects[0].toUse, sockPlayer ); 
	JS_SetPrivate( targContext, charObjects[0].toUse, pPlayer );

	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse ); 
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse ); 
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLogout", 2, params, &rval ); 
	if( retVal == JS_FALSE ) 
		SetEventExists( seOnLogout, false );

	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL ); 
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL ); 
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
	JS_SetPrivate( targContext, sockObjects[0].toUse, sockPlayer );
	JS_SetPrivate( targContext, itemObjects[0].toUse, iClicked );
	params[0] = OBJECT_TO_JSVAL(sockObjects[0].toUse);
	params[1] = OBJECT_TO_JSVAL(itemObjects[0].toUse);
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onClick", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnClick, false );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnFall( CChar *pFall, SI08 fallDistance )
{
	if( !ValidateObject( pFall ) )
		return false;
	if( !ExistAndVerify( seOnFall, "onFall" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pFall );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( fallDistance );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFall", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnFall, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSell( void )
{
	if( !EventExists( seOnSell ) )
		return false;
	return false;
}
bool cScript::OnBuy( void )
{
	if( !EventExists( seOnBuy ) )
		return false;
	return false;
}

bool cScript::OnAISliver( CChar *pSliver )
{
	if( !ValidateObject( pSliver ) )
		return false;
	if( !ExistAndVerify( seOnAISliver, "onAISliver" ) )
		return false;

	jsval params[1], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pSliver );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAISliver", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnAISliver, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );

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
bool cScript::OnUnknownTrigger( void )
{
	if( !EventExists( seOnUnknownTrigger ) )
		return false;
	return false;
}
bool cScript::OnLightChange( void )
{
	if( !EventExists( seOnLightChange ) )
		return false;
	return false;
}
bool cScript::OnXYZEvent( void )
{
	if( !EventExists( seOnXYZEvent ) )
		return false;
	return false;
}
bool cScript::OnPortal( void )
{
	if( !EventExists( seOnPortal ) )
		return false;
	return false;
}

bool cScript::OnTimer( CBaseObject *tObject, UI08 timerID )
{
	if( !ValidateObject( tObject ) )
		return false;
	if( !ExistAndVerify( seOnTimer, "onTimer" ) )
		return false;

	jsval rval, params[2];
	if( tObject->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, tObject );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, tObject );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}
	
	params[1] = INT_TO_JSVAL( timerID );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTimer", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnTimer, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnStatLoss( CChar *player, UI32 stat )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatLoss, "onStatLoss" ) )
		return false;

	jsval rval, params[2];
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( stat );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatLoss, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnStatChange( CChar *player, UI32 stat )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnStatChange, "onStatChange" ) )
		return false;

	jsval rval, params[2];
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( stat );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStatChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStatChange, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSkillLoss( CChar *player, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnSkillLoss, "onSkillLoss" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillLoss, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSkillChange( CChar *player, SI08 skill )
{
	if( !ValidateObject( player ) )
		return false;
	if( !ExistAndVerify( seOnSkillChange, "onSkillChange" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, player );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( skill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillChange, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::EventExists( ScriptEvent eventNum ) const
{
	UI32 index = eventNum / 32;
	if( index > 1 )
		return false;
	UI32 flagToSet = power( 2, (eventNum % 32) );
	return ( (eventPresence[index]&flagToSet) == flagToSet );
}
void cScript::SetEventExists( ScriptEvent eventNum, bool status )
{
	UI32 index = eventNum / 32;
	if( index > 1 )
		return;
	UI32 flagToSet = power( 2, (eventNum % 32) );
	if( status )
		eventPresence[index] |= flagToSet;
	else
	{
		UI32 flagMask = 0xFFFFFFFF;
		flagMask ^= flagToSet;
		eventPresence[index] &= flagMask;
	}
}

bool cScript::NeedsChecking( ScriptEvent eventNum ) const
{
	UI32 index = eventNum / 32;
	if( index > 1 )
		return false;
	UI32 flagToSet = power( 2, (eventNum % 32) );
	return ( (needsChecking[index]&flagToSet) == flagToSet );
}
void cScript::SetNeedsChecking( ScriptEvent eventNum, bool status )
{
	UI32 index = eventNum / 32;
	if( index > 1 )
		return;
	UI32 flagToSet = power( 2, (eventNum % 32) );
	if( status )
		needsChecking[index] |= flagToSet;
	else
	{
		UI32 flagMask = 0xFFFFFFFF;
		flagMask ^= flagToSet;
		needsChecking[index] &= flagMask;
	}
}

bool cScript::OnDeath( CChar *pDead )
{
	if( !ValidateObject( pDead ) )
		return false;
	if( !ExistAndVerify( seOnDeath, "onDeath" ) )
		return false;

	jsval params[1], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pDead );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeath", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDeath, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnResurrect( CChar *pAlive )
{
	if( !ValidateObject( pAlive ) )
		return false;
	if( !ExistAndVerify( seOnResurrect, "onResurrect" ) )
		return false;

	jsval params[1], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pAlive );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onResurrect", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnResurrect, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
{
	if( !ValidateObject( pChanging ) )
		return false;
	if( !ExistAndVerify( seOnFlagChange, "onFlagChange" ) )
		return false;

	jsval params[3], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pChanging );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( newStatus );
	params[1] = INT_TO_JSVAL( oldStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onFlagChange", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnFlagChange, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum )
{
	if( tSock == NULL )
		return false;
	jsval params[2], rval;
	JS_SetPrivate( targContext, sockObjects[0].toUse, tSock );
	int objType			= 2;	// 2 == null, 1 == char, 0 == item
	CBaseObject *mObj	= NULL;
	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
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
		{
			JS_SetPrivate( targContext, itemObjects[0].toUse, mObj );
			params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
		}
		else
		{
			JS_SetPrivate( targContext, charObjects[0].toUse, mObj );
			params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		}
	}
	char targetFunction[32];
	sprintf( targetFunction, "onCallback%i", callNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, targetFunction, 2, params, &rval );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
	switch( objType )
	{
	case 0:		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );	break;
	case 1:		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );	break;
	}
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
	JS_SetPrivate( targContext, charObjects[0].toUse, pChanging );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( newStatus );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onHungerChange", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnHungerChange, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen )
{
	if( !ValidateObject( stealing ) || !ValidateObject( stolenFrom ) || !ValidateObject( stolen ) )
		return false;
	if( !ExistAndVerify( seOnStolenFrom, "onStolenFrom" ) )
		return false;

	jsval params[3], rval;

	JS_SetPrivate( targContext, charObjects[0].toUse, stealing );
	JS_SetPrivate( targContext, charObjects[1].toUse, stolenFrom );
	JS_SetPrivate( targContext, itemObjects[0].toUse, stolen );
	
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	params[2] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onStolenFrom", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnStolenFrom, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );

	return ( retVal == JS_TRUE );
}

bool cScript::OnSnooped( CChar *snooped, CChar *snooper, bool success )
{
	if( !ValidateObject( snooped ) || !ValidateObject( snooper ) )
		return false;
	if( !ExistAndVerify( seOnSnooped, "onSnooped" ) )
		return false;

	jsval params[3], rval;

	JS_SetPrivate( targContext, charObjects[0].toUse, snooped );
	JS_SetPrivate( targContext, charObjects[1].toUse, snooper );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	params[2] = BOOLEAN_TO_JSVAL( success );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSnooped", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSnooped, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );

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
	if( packet == NULL )
		return;
	CSocket *pressing = packet->GetSocket();
	if( pressing == NULL )
		return;

	UI32 button		= packet->ButtonID();
	UI16 nButtons	= static_cast<UI16>(packet->SwitchCount());
	UI16 nText		= static_cast<UI16>(packet->TextCount());

	SEGumpData *segdGumpData	= new SEGumpData;
	JSObject *jsoObject			= JS_NewObject( targContext, &UOXGumpData_class, GumpDataProto, NULL );
	JS_DefineFunctions( targContext, jsoObject, CGumpData_Methods );
	JS_DefineProperties( targContext, jsoObject, CGumpDataProperties );
	JS_SetPrivate( targContext, jsoObject, segdGumpData );

	int i;
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
	JS_SetPrivate( targContext, sockObjects[0].toUse, pressing );
	jsvParams[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	jsvParams[1] = INT_TO_JSVAL( button );
	jsvParams[2] = OBJECT_TO_JSVAL( jsoObject );
	JS_CallFunctionName( targContext, targObject, "onGumpPress", 7, jsvParams, &jsvRVal );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
}

void cScript::HandleGumpInput( CPIGumpInput *pressing )
{
	if( pressing == NULL )
		return;
	jsval params[1], rval;
	JS_SetPrivate( targContext, sockObjects[0].toUse, pressing->GetSocket() );
	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[0] = INT_TO_JSVAL( pressing );
	JS_CallFunctionName( targContext, targObject, "onGumpInput", 1, params, &rval );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
}

bool cScript::OnEnterRegion( CChar *entering, SI16 region )
{
	if( !ValidateObject( entering ) )
		return false;
	if( !ExistAndVerify( seOnEnterRegion, "onEnterRegion" ) )
		return false;

	jsval params[2], rval;

	JS_SetPrivate( targContext, charObjects[0].toUse, entering );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( region );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onEnterRegion", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnEnterRegion, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnLeaveRegion( CChar *leaving, SI16 region )
{
	if( !ValidateObject( leaving ) )
		return false;
	if( !ExistAndVerify( seOnLeaveRegion, "onLeaveRegion" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, leaving );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( region );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onLeaveRegion", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnLeaveRegion, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSpellTarget( CChar *target, CChar *caster, UI08 spellNum )
{
	if( !ValidateObject( target ) || !ValidateObject( caster ) )
		return false;
	if( !ExistAndVerify( seOnSpellTarget, "onSpellTarget" ) )
		return false;

	jsval params[4], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, caster );
	JS_SetPrivate( targContext, charObjects[1].toUse, target );
	params[0] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	params[1] = INT_TO_JSVAL( 0 );
	params[2] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[3] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellTarget", 4, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellTarget, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSpellTarget( CItem *target, CChar *caster, UI08 spellNum )
{
	if( !ValidateObject( target ) || !ValidateObject( caster ) )
		return false;
	if( !ExistAndVerify( seOnSpellTarget, "onSpellTarget" ) )
		return false;

	jsval params[4], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, caster );
	JS_SetPrivate( targContext, itemObjects[0].toUse, target );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = INT_TO_JSVAL( 1 );
	params[2] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[3] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellTarget", 4, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellTarget, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 FindFreePosition( IUEEntries iType ) const
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns a free object of type iType
//o---------------------------------------------------------------------------o
UI32 cScript::FindFreePosition( IUEEntries iType ) const
{
	size_t toSearch = 0;
	switch( iType )
	{
		case IUE_RACE:
			for( toSearch = 0; toSearch < raceObjects.size(); ++toSearch )
			{
				if( !raceObjects[toSearch].inUse )
					return toSearch;
			}
			break;
		case IUE_CHAR:
			for( toSearch = 0; toSearch < charObjects.size(); ++toSearch )
			{
				if( !charObjects[toSearch].inUse )
					return toSearch;
			}
			break;
		case IUE_ITEM:
			for( toSearch = 0; toSearch < itemObjects.size(); ++toSearch )
			{
				if( !itemObjects[toSearch].inUse )
					return toSearch;
			}
			break;
		case IUE_SOCK:
			for( toSearch = 0; toSearch < sockObjects.size(); ++toSearch )
			{
				if( !sockObjects[toSearch].inUse )
					return toSearch;
			}
			break;
		case IUE_GUILD:
			for( toSearch = 0; toSearch < guildObjects.size(); ++toSearch )
			{
				if( !guildObjects[toSearch].inUse )
					return toSearch;
			}
			break;
		case IUE_REGION:
			for( toSearch = 0; toSearch < regionObjects.size(); ++toSearch )
			{
				if( !regionObjects[toSearch].inUse )
					return toSearch;
			}
			break;
		default:
		case IUE_UNKNOWN:
		case IUE_COUNT:
			return INVALIDSERIAL;
			break;
	}
	return INVALIDSERIAL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 FindUsedObject( IUEEntries iType, JSObject *toFind ) const
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the position of the object toFind, if it exists
//o---------------------------------------------------------------------------o
UI32 cScript::FindUsedObject( IUEEntries iType, JSObject *toFind ) const
{
	if( toFind == NULL )
		return INVALIDSERIAL;
	UI32 toSearch = 0;
	switch( iType )
	{
		case IUE_RACE:
			for( toSearch = 0; toSearch < raceObjects.size(); ++toSearch )
			{
				if( raceObjects[toSearch].toUse == toFind )
					return toSearch;
			}
			break;
		case IUE_CHAR:
			for( toSearch = 0; toSearch < charObjects.size(); ++toSearch )
			{
				if( charObjects[toSearch].toUse == toFind )
					return toSearch;
			}
			break;
		case IUE_ITEM:
			for( toSearch = 0; toSearch < itemObjects.size(); ++toSearch )
			{
				if( itemObjects[toSearch].toUse == toFind )
					return toSearch;
			}
			break;
		case IUE_SOCK:
			for( toSearch = 0; toSearch < sockObjects.size(); ++toSearch )
			{
				if( sockObjects[toSearch].toUse == toFind )
					return toSearch;
			}
			break;
		case IUE_GUILD:
			for( toSearch = 0; toSearch < guildObjects.size(); ++toSearch )
			{
				if( guildObjects[toSearch].toUse == toFind )
					return toSearch;
			}
			break;
		case IUE_REGION:
			for( toSearch = 0; toSearch < regionObjects.size(); ++toSearch )
			{
				if( regionObjects[toSearch].toUse == toFind )
					return toSearch;
			}
			break;
		default:
		case IUE_UNKNOWN:
		case IUE_COUNT:
			return INVALIDSERIAL;
			break;
	}
	return INVALIDSERIAL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void ReleaseObject( JSObject *toRelease, IUEEntries iType )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Releases the object toRelease of type iType, so it can be
//|					reused at a later execution without the need for creation
//o---------------------------------------------------------------------------o
void cScript::ReleaseObject( JSObject *toRelease, IUEEntries iType )
{
	if( toRelease == NULL )
		return;
	UI32 tFound = FindUsedObject( iType, toRelease );
	if( tFound == INVALIDSERIAL )
		return;
	switch( iType )
	{
		case IUE_RACE:		raceObjects[tFound].inUse = false;		break;
		case IUE_CHAR:		charObjects[tFound].inUse = false;		break;
		case IUE_ITEM:		itemObjects[tFound].inUse = false;		break;
		case IUE_SOCK:		sockObjects[tFound].inUse = false;		break;
		case IUE_GUILD:		guildObjects[tFound].inUse = false;		break;
		case IUE_REGION:	regionObjects[tFound].inUse = false;	break;
		default:
		case IUE_UNKNOWN:
		case IUE_COUNT:
			break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	JSObject *MakeNewObject( IUEEntries iType )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Private function that creates and initializes an object of
//|					a particular type (iType)
//o---------------------------------------------------------------------------o
JSObject *cScript::MakeNewObject( IUEEntries iType )
{
	JSObject *toMake = NULL;
	switch( iType )
	{
		case IUE_RACE:
			toMake = JS_NewObject( targContext, &UOXRace_class, NULL, targObject ); 
			if( toMake == NULL )
				return NULL;
			JS_DefineFunctions( targContext, toMake, CRace_Methods );
			JS_DefineProperties( targContext, toMake, CRaceProperties );
			break;
		case IUE_CHAR:
			toMake = JS_NewObject( targContext, &UOXChar_class, CharProto, targObject ); 
			if( toMake == NULL )
				return NULL;
			JS_DefineProperties( targContext, toMake, CCharacterProps );
			JS_DefineFunctions( targContext, toMake, CChar_Methods );
			break;
		case IUE_ITEM:
			toMake = JS_NewObject( targContext, &UOXItem_class, ItemProto, targObject ); 
			if( toMake == NULL )
				return NULL;
			JS_DefineObject( targContext, toMake, "items", &UOXItems_class, NULL, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY );
			JS_DefineFunctions( targContext, toMake, CItem_Methods );
			JS_DefineProperties( targContext, toMake, CItemProps );
			break;
		case IUE_SOCK:
			toMake = JS_NewObject( targContext, &UOXSocket_class, SocketProto, targObject ); 
			if( toMake == NULL )
				return NULL;
			JS_DefineFunctions( targContext, toMake, CSocket_Methods );
			JS_DefineProperties( targContext, toMake, CSocketProps );
			break;
		case IUE_GUILD:
			toMake = JS_NewObject( targContext, &UOXGuild_class, NULL, targObject ); 
			if( toMake == NULL )
				return NULL;
			JS_DefineFunctions( targContext, toMake, CGuild_Methods );
			JS_DefineProperties( targContext, toMake, CGuildProperties );
			break;
		case IUE_REGION:
			toMake = JS_NewObject( targContext, &UOXRegion_class, RegionProto, targObject ); 
			if( toMake == NULL )
				return NULL;
			JS_DefineFunctions( targContext, toMake, CRegion_Methods );
			JS_DefineProperties( targContext, toMake, CRegionProperties );
			break;
		default:
		case IUE_UNKNOWN:
		case IUE_COUNT:
			return NULL;
	}

	// DAMN! Using the deprecated function it works!
	JS_LockGCThing( targContext, toMake );
	//JS_AddNamedRoot( targContext, toMake, "asd" );
	return toMake;
}

//o---------------------------------------------------------------------------o
//|	Function	-	JSObject *AcquireObject( IUEEntries iType )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns an object pointer for an object of type iType, 
//|					creating a new one if there is none free
//o---------------------------------------------------------------------------o
JSObject *cScript::AcquireObject( IUEEntries iType )
{
	UI32 tFound = FindFreePosition( iType );
	if( tFound == INVALIDSERIAL )
	{
		size_t currentPos;
		JSObject *makeIt = MakeNewObject( iType );
		if( makeIt == NULL )
			return NULL;
		switch( iType )
		{
			case IUE_RACE:		
				currentPos = raceObjects.size();
				raceObjects.push_back( InUseEntry( true, makeIt ) );
				return makeIt;
				break;
			case IUE_CHAR:		
				currentPos = charObjects.size();
				charObjects.push_back( InUseEntry( true, makeIt ) );
				return makeIt;
				break;
			case IUE_ITEM:		
				currentPos = itemObjects.size();
				itemObjects.push_back( InUseEntry( true, makeIt ) );
				return makeIt;
				break;
			case IUE_SOCK:		
				currentPos = sockObjects.size();
				sockObjects.push_back( InUseEntry( true, makeIt ) );
				return makeIt;
				break;
			case IUE_GUILD:		
				currentPos = guildObjects.size();
				guildObjects.push_back( InUseEntry( true, makeIt ) );
				return makeIt;
				break;
			case IUE_REGION:	
				currentPos = regionObjects.size();
				regionObjects.push_back( InUseEntry( true, makeIt ) );
				return makeIt;
				break;
			default:
			case IUE_UNKNOWN:
			case IUE_COUNT:
				return NULL;
				break;
		}
	}
	else
	{
		switch( iType )
		{
			case IUE_RACE:		raceObjects[tFound].inUse = true;	return raceObjects[tFound].toUse;	break;
			case IUE_CHAR:		charObjects[tFound].inUse = true;	return charObjects[tFound].toUse;	break;
			case IUE_ITEM:		itemObjects[tFound].inUse = true;	return itemObjects[tFound].toUse;	break;
			case IUE_SOCK:		sockObjects[tFound].inUse = true;	return sockObjects[tFound].toUse;	break;
			case IUE_GUILD:		guildObjects[tFound].inUse = true;	return guildObjects[tFound].toUse;	break;
			case IUE_REGION:	regionObjects[tFound].inUse = true;	return regionObjects[tFound].toUse;	break;
			default:
			case IUE_UNKNOWN:
			case IUE_COUNT:
				return NULL;
				break;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CallParticularEvent( char *eventToCall, jsval *params, SI32 numParams )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o---------------------------------------------------------------------------o
bool cScript::CallParticularEvent( char *eventToCall, jsval *params, SI32 numParams )
{
	if( eventToCall == NULL )
		return false;

	jsval rval;
	JSBool retVal = JS_CallFunctionName( targContext, targObject, eventToCall, numParams, params, &rval );
	return ( retVal == JS_TRUE );
}

//o---------------------------------------------------------------------------o
//|	Function	-	JSObject *AcquireObject( IUEEntries iType )
//|	Programmer	-	Abaddon
//|	Date		-	24th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Removes all objects that have been added to the root
//o---------------------------------------------------------------------------o
void cScript::RemoveFromRoot( void )
{
	size_t i = 0;
	for( i = 0; i < raceObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, raceObjects[i].toUse );
	}

	for( i = 0; i < charObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, charObjects[i].toUse );
	}

	for( i = 0; i < itemObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, itemObjects[i].toUse );
	}

	for( i = 0; i < sockObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, sockObjects[i].toUse );
	}

	for( i = 0; i < guildObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, guildObjects[i].toUse );
	}

	for( i = 0; i < regionObjects.size(); ++i )
	{
		JS_UnlockGCThing( targContext, regionObjects[i].toUse );
	}

	raceObjects.resize( 0 );
	charObjects.resize( 0 );
	itemObjects.resize( 0 );
	sockObjects.resize( 0 );
	guildObjects.resize( 0 );
	regionObjects.resize( 0 );
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
	JS_SetPrivate( targContext, charObjects[0].toUse, tChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellCast", 2, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );	

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
	JS_SetPrivate( targContext, charObjects[0].toUse, tChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onScrollCast", 2, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );	

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
	JS_SetPrivate( targContext, charObjects[0].toUse, tChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellSuccess", 2, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );

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
	char *lwrSpeech		= new char[strlen(mySpeech)+1];
	strcpy( lwrSpeech, mySpeech );
	strlwr( lwrSpeech );

	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );

	JS_SetPrivate( targContext, charObjects[0].toUse, myChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = STRING_TO_JSVAL( strSpeech );
	//params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTalk", 2, params, &rval );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );

	delete [] lwrSpeech;
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

	JS_SetPrivate( targContext, charObjects[0].toUse, myChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );

	if( !ValidateObject( myItem ) )
		params[1] = JSVAL_NULL;
	else
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, myItem ); // If it's null it's still ok
		params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}
	
	params[2] = STRING_TO_JSVAL( strSpeech );
	params[3] = INT_TO_JSVAL( myChar->GetSpeechID() );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpeechInput", 4, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );

	if( ValidateObject( myItem ) )
		JS_SetPrivate( targContext, itemObjects[0].toUse, myItem ); // If it's null it's still ok

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
	JS_SetPrivate( targContext, itemObjects[0].toUse, book );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellGain", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellGain, false );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}
bool cScript::OnSpellLoss( CItem *book, const UI08 spellNum )
{
	if( !ValidateObject( book ) )
		return false;
	if( !ExistAndVerify( seOnSpellLoss, "onSpellLoss" ) )
		return false;

	jsval params[2], rval;
	JS_SetPrivate( targContext, itemObjects[0].toUse, book );
	params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	params[1] = INT_TO_JSVAL( spellNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellLoss", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSpellLoss, false );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill )
{
	if( !ValidateObject( myChar ) || skill > ALLSKILLS )
		return false;
	if( !ExistAndVerify( seOnSkillCheck, "onSkillCheck" ) )
		return false;

	jsval params[4], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, myChar );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = INT_TO_JSVAL( skill );
	params[2] = INT_TO_JSVAL( lowSkill );
	params[3] = INT_TO_JSVAL( highSkill );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSkillCheck", 4, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnSkillCheck, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

//o--------------------------------------------------------------------------
//|	Function	-	bool AreaCharFunc( char *funcName, CChar *srcChar, CChar *tmpChar )
//|	Date		-	January 27, 2003
//|	Programmer	-	Maarc
//|	Modified	-
//o--------------------------------------------------------------------------
//|	Purpose		-	Calls the function represented in funcName for the script
//|				-	passing in two character parameters
//o--------------------------------------------------------------------------
bool cScript::AreaCharFunc( char *funcName, CChar *srcChar, CChar *tmpChar, CSocket *s )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( tmpChar ) || funcName == NULL )
		return false;

	jsval params[3], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, srcChar );
	JS_SetPrivate( targContext, charObjects[1].toUse, tmpChar );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );

	if( s != NULL )
	{
		JS_SetPrivate( targContext, sockObjects[0].toUse, s );
		params[2] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	}
	else
	{
		params[2] = JSVAL_NULL;
	}

	JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName, 3, params, &rval );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	if( s != NULL )
		JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
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
	JS_SetPrivate( targContext, sockObjects[0].toUse, mSock ); 	
	params[0]		= OBJECT_TO_JSVAL( sockObjects[0].toUse ); 	
	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onCommand", 1, params, &rval ); 	
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL ); 	
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
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CallParticularEvent( char *eventToCall, jsval *params, SI32 numParams )
//|	Programmer	-	Abaddon
//|	Date		-	20th December, 2001
//o---------------------------------------------------------------------------o
//|	Purpose		-	Calls a particular script event, passing parameters
//o---------------------------------------------------------------------------o
bool cScript::commandRegistration( void )
{
	jsval params[1], rval;
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "CommandRegistration", 1, params, &rval );
	return ( retVal == JS_TRUE );
}

bool cScript::spellRegistration( void )
{
	jsval params[1], rval;
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "SpellRegistration", 0, params, &rval );
	return ( retVal == JS_TRUE );
}

bool cScript::executeCommand( CSocket *s, std::string funcName, std::string executedString )
{
	jsval params[2], rval; 	
	JSString *execString = JS_NewStringCopyZ( targContext, executedString.c_str() );
	JS_SetPrivate( targContext, sockObjects[0].toUse, s );
	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[1] = STRING_TO_JSVAL( execString );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, funcName.c_str(), 2, params, &rval ); 	
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL ); 	
	return ( retVal == JS_TRUE ); 
}

bool cScript::MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, int spellNum )
{
	if( !ValidateObject( tChar ) )
		return false;
	
	if( !ExistAndVerify( seOnSpellCast, "onSpellCast" ) )
		return false;

	jsval params[4], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, tChar );
	JS_SetPrivate( targContext, sockObjects[0].toUse, mSock );

	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[2] = BOOLEAN_TO_JSVAL( directCast );
	params[3] = INT_TO_JSVAL( spellNum );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onSpellCast", 4, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );	
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );	

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

	if( a->GetObjType() == OT_CHAR )
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, a );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	}
	else
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, a );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}

	JSBool retVal	= JS_CallFunctionName( targContext, targObject, "onIterate", 1, params, &rval ); 
	
	if( a->GetObjType() == OT_CHAR )
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	
	if( retVal == JS_FALSE )
		SetEventExists( seOnIterate, false ); 	
	else if( JSVAL_TO_BOOLEAN( rval ) )
		++b;

	return ( retVal == JS_TRUE ); 
}

}
