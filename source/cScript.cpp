#include "uox3.h"
#include "power.h"
#include "cScript.h"
#include "UOXJSClasses.h"
#include "SEFunctions.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"

// Version History
// 1.0		Abaddon		26th August, 2000
//			Initial implementation
//			The basic event model is in, along with some fairly simple error handling
//			Note that parameters for each event is NOT correct at all, it was just to get the basic layout right
//			Support for CS type stuff with IsFiring(), Firing(), and Stop()
// 1.1		Abaddon		6th September, 2000
//			Each function has params and passes it along into the the script as required
// 1.2		Abaddon		18th September, 2000
//			Addition of a number of more events, not all with params
//			More functions added to the functions table
//			Support for dynamic reloading of scripts
// 1.3		Abaddon		2nd November, 2000
//			Addition of a number of events.  Caching of function presence added
//			All functions assumed present, only failure on a call will flag not present

static JSFunctionSpec my_functions[] =  
{ 	// ScriptName, Func Ptr, num args, ?, ? 
	{ "IsDecayable",				SE_IsDecayable,				1, 0, 0 },
	{ "SetDecayable",				SE_SetDecayable,			2, 0, 0 },
	{ "GetDecayTime",				SE_GetDecayTime,			1, 0, 0 },
	{ "SetDecayTime",				SE_SetDecayTime,			2, 0, 0 },	
	{ "StringToNum",				SE_StringToNum,				1, 0, 0 },
	{ "NumToString",				SE_NumToString,				1, 0, 0 },
	{ "SetTag",						SE_SetTag,					4, 0, 0 },
	{ "GetTag",						SE_GetTag,					3, 0, 0 },
	{ "GetNumTags",					SE_GetNumTags,				2, 0, 0 },
	{ "GetName2", 					SE_GetName2,				1, 0, 0 },
	{ "SetName2", 					SE_SetName2,				2, 0, 0 },
	{ "DoDamage",					SE_DoDamage,				3, 0, 0 },
	{ "SetSerial",					SE_SetSerial,				3, 0, 0 },
	{ "GetTarget",					SE_GetTarget,				1, 0, 0 },
	{ "SetTarget",					SE_SetTarget,				2, 0, 0 },
	{ "GetCharPack", 				SE_GetCharPack,				1, 0, 0 },
	{ "CalcDefense", 				SE_CalcDefense,				1, 0, 0 },
	{ "CalcAttack", 				SE_CalcAttack,				1, 0, 0 },
	{ "OpenBank", 					SE_OpenBank,				2, 0, 0 },
	{ "DoStaticEffect", 			SE_DoStaticEffect,			5, 0, 0 },
	{ "DoMovingEffect", 			SE_DoMovingEffect,			8, 0, 0 },
	{ "GetLowDamage", 				SE_GetLowDamage,			1, 0, 0 },
	{ "GetHiDamage", 				SE_GetHiDamage,				1, 0, 0 },
	{ "SetAtt", 					SE_SetAtt,					3, 0, 0 },
	{ "GetDef", 					SE_GetDef,					1, 0, 0 },
	{ "SetDef", 					SE_SetDef,					2, 0, 0 },
	{ "BroadcastMessage",			SE_BroadcastMessage,		1, 0, 0 },
	{ "ConsoleMessage",				SE_ConsoleMessage,			1, 0, 0 }, 
	{ "YellMessage",				SE_YellMessage,				2, 0, 0 },
	{ "WhisperMessage",				SE_WhisperMessage,			2, 0, 0 },
	{ "ScriptPrintNumber",			ScriptPrintNumber,			1, 0, 0 },
	{ "SetShowLayer",				SetShowLayer,				1, 0, 0 },
	{ "GetShowLayer",				GetShowLayer,				0, 0, 0 },
	{ "CalcSockFromChar",			SE_CalcSockFromChar,		1, 0, 0 },
	{ "RandomNumber",				SE_RandomNumber,			2, 0, 0 },
	{ "CalcCharFromSer",			SE_CalcCharFromSer,			1, 0, 0 },
	{ "CalcItemFromSer",			SE_CalcItemFromSer,			1, 0, 0 },
	{ "DistanceTo",					SE_DistanceTo,				4, 0, 0 },
	{ "GetHour",					SE_GetHour,					0, 0, 0 },
	{ "GetMinute",					SE_GetMinute,				0, 0, 0 },
	{ "GetDay",						SE_GetDay,					0, 0, 0 },
	{ "GetSecondsPerUOMinute",		SE_GetSecondsPerUOMinute,	0, 0, 0 },
	{ "GetCurrentClock",			SE_GetCurrentClock,			0, 0, 0 },
	{ "SubStringSearch",			SE_SubStringSearch,			2, 0, 0 },
	{ "IsCriminal",					SE_IsCriminal,				1, 0, 0 },
	{ "IsMurderer",					SE_IsMurderer,				1, 0, 0 },
	{ "IsInnocent",					SE_IsInnocent,				1, 0, 0 },
	{ "GetFlag",					SE_GetFlag,					1, 0, 0 },
	{ "MakeCriminal",				SE_MakeCriminal,			1, 0, 0 },
	{ "MakeInnocent",				SE_MakeInnocent,			1, 0, 0 },
	{ "GetMurderThreshold",			SE_GetMurderThreshold,		0, 0, 0 },
	{ "GetGender",					SE_GetGender,				1, 0, 0 },
	{ "SetGender",					SE_SetGender,				2, 0, 0 },
	{ "RollDice",					SE_RollDice,				3, 0, 0 },
	{ "StartTimer",					SE_StartTimer,				4, 0, 0 },
	{ "TurnToward",					SE_TurnToward,				3, 0, 0 },
	{ "DirectionTo",				SE_DirectionTo,				3, 0, 0 },
	{ "RaceCompare",				SE_RaceCompare,				2, 0, 0 },
	{ "RaceCompareByRace",			SE_RaceCompareByRace,		2, 0, 0 },
	{ "FindItemOnLayer",			SE_FindItemOnLayer,			2, 0, 0 },
	{ "GetString",					SE_GetString,				2, 0, 0 },
	{ "GetByte",					SE_GetByte,					2, 0, 0 },
	{ "GetWord",					SE_GetWord,					2, 0, 0 },
	{ "GetDWord",					SE_GetDWord,				2, 0, 0 },
	{ "SetByte",					SE_SetByte,					3, 0, 0 },
	{ "SetWord",					SE_SetWord,					3, 0, 0 },
	{ "SetDWord",					SE_SetDWord,				3, 0, 0 },
	{ "SetString",					SE_SetString,				3, 0, 0 },
	{ "ReadBytes",					SE_ReadBytes,				2, 0, 0 },
	{ "DoTempEffect",				SE_DoTempEffect,			7, 0, 0 },
	{ "CastSpell",					SE_CastSpell,				2, 0, 0 },
	{ "UseReagant",					SE_UseReagant,				3, 0, 0 },
	{ "UseReagants",				SE_UseReagants,				9, 0, 0 },
	{ "AddSpell",					SE_AddSpell,				2, 0, 0 },
	{ "RemoveSpell",				SE_RemoveSpell,				2, 0, 0 },
	{ "SpellFail",					SE_SpellFail,				2, 0, 0 },
	{ "MakeMenu",					SE_MakeMenu,				2, 0, 0 },
	{ "MakeItem",					SE_MakeItem,				3, 0, 0 },
	{ "CalcRank",					SE_CalcRank,				5, 0, 0 },
	{ "CalcCharFromSock",			SE_CalcCharFromSock,		1, 0, 0 },
	{ "ApplyRank",					SE_ApplyRank,				2, 0, 0 },
	{ "FindMulti",					SE_FindMulti,				4, 0, 0 },
	{ "IsRegionGuarded",			SE_IsRegionGuarded,			1, 0, 0 },
	{ "CanMarkInRegion",			SE_CanMarkInRegion,			1, 0, 0 },
	{ "CanGateInRegion",			SE_CanGateInRegion,			1, 0, 0 },
	{ "CanRecallInRegion",			SE_CanRecallInRegion,		1, 0, 0 },
	{ "IsMemberOfTown",				SE_IsMemberOfTown,			2, 0, 0 },
	{ "GetTownMayor",				SE_GetTownMayor,			1, 0, 0 },
	{ "GetTownRace",				SE_GetTownRace,				1, 0, 0 },
	{ "SetTownRace",				SE_SetTownRace,				2, 0, 0 },
	{ "PossessTown",				SE_PossessTown,				2, 0, 0 },
	{ "GetTownTaxResource",			SE_GetTownTaxResource,		1, 0, 0 },
	{ "GetRaceName",				SE_GetRaceName,				1, 0, 0 },
	{ "IsRaceWeakToWeather",		SE_IsRaceWeakToWeather,		2, 0, 0 },
	{ "RaceRequiresBeard",			SE_RaceRequiresBeard,		1, 0, 0 },
	{ "RaceRequiresNoBeard",		SE_RaceRequiresNoBeard,		1, 0, 0 },
	{ "IsPlayerRace",				SE_IsPlayerRace,			1, 0, 0 },
	{ "GetRacialGenderRestrict",	SE_GetRacialGenderRestrict, 1, 0, 0 },
	{ "GetRacialArmourClass",		SE_GetRacialArmourClass,	1, 0, 0 },
	{ "CanRaceWearArmour",			SE_CanRaceWearArmour,		2, 0, 0 },
	{ "GetRaceLanguageSkill",		SE_GetRaceLanguageSkill,	1, 0, 0 },
	{ "GetRaceSkillAdjustment",		SE_GetRaceSkillAdjustment,	2, 0, 0 },
	{ "GetRacePoisonResistance",	SE_GetRacePoisonResistance, 1, 0, 0 },
	{ "GetRaceMagicResistance",		SE_GetRaceMagicResistance,	2, 0, 0 },
	{ "IsValidRaceHairColour",		SE_IsValidRaceHairColour,	2, 0, 0 },
	{ "IsValidRaceSkinColour",		SE_IsValidRaceSkinColour,	2, 0, 0 },
	{ "IsValidRaceBeardColour",		SE_IsValidRaceBeardColour,	2, 0, 0 },
	{ "Attack",						SE_Attack,					2, 0, 0 },
	{ "AttackTarget",				SE_AttackTarget,			1, 0, 0 },
	{ "FirstItemInCont",			SE_FirstItemInCont,			1, 0, 0 },
	{ "NextItemInCont",				SE_NextItemInCont,			1, 0, 0 },
	{ "FinishedItemInCont",			SE_FinishedItemInCont,		1, 0, 0 },
	{ "UseDoor",					SE_UseDoor,					2, 0, 0 },
	{ "GetRaceVisibleDistance",		SE_GetRaceVisibleDistance,	1, 0, 0 },
	{ "GetRaceNightVision",			SE_GetRaceNightVision,		1, 0, 0 },
	{ "GetGuildType",				SE_GetGuildType,			1, 0, 0 },
	{ "GetGuildName",				SE_GetGuildName,			1, 0, 0 },
	{ "GetGuildMaster",				SE_GetGuildMaster,			1, 0, 0 },
	{ "SetGuildType",				SE_SetGuildType,			2, 0, 0 },
	{ "SetGuildName",				SE_SetGuildName,			2, 0, 0 },
	{ "SetGuildMaster",				SE_SetGuildMaster,			2, 0, 0 },
	{ "GetNumGuildMembers",			SE_GetNumGuildMembers,		2, 0, 0 },
	{ "AddToGuild",					SE_AddToGuild,				3, 0, 0 },
	{ "RemoveFromGuild",			SE_RemoveFromGuild,			2, 0, 0 },
	{ "CompareGuild",				SE_CompareGuild,			2, 0, 0 },
	{ "CompareGuildByGuild",		SE_CompareGuildByGuild,		2, 0, 0 },
	{ "GetGuildStone",				SE_GetGuildStone,			1, 0, 0 },
	{ "VendorSell",					SE_VendorSell,				2, 0, 0 },
	{ "VendorBuy",					SE_VendorBuy,				2, 0 ,0 },
	{ "GetGuild",					SE_GetGuild,				1, 0, 0 },
	{ "SkillBeingUsed",				SE_SkillBeingUsed,			2, 0, 0 },
	{ "SetSkillUse",				SE_SetSkillUse,				3, 0, 0 },
	{ "CommandLevelReq",			SE_CommandLevelReq,			1, 0, 0 },
	{ "CommandExists",				SE_CommandExists,			1, 0, 0 },
	{ "FirstCommand",				SE_FirstCommand,			0, 0, 0 },
	{ "NextCommand",				SE_NextCommand,				0, 0, 0 },
	{ "FinishedCommandList",		SE_FinishedCommandList,		0, 0, 0 },
	{ "GetPrivateBit",				SE_GetPrivateBit,			4, 0, 0 },
	{ "SetPrivateBit",				SE_SetPrivateBit,			5, 0, 0 },
	{ "GetPrivateWord",				SE_GetPrivateWord,			3, 0, 0 },
	{ "SetPrivateWord",				SE_SetPrivateWord,			4, 0, 0 },
	{ "GetPrivateBitRange",			SE_GetPrivateBitRange,		5, 0, 0 },
	{ "SetPrivateBitRange",			SE_SetPrivateBitRange,		6, 0, 0 },
	{ "PopUpTarget",				SE_PopUpTarget,				3, 0, 0 },
	{ "SpawnItem",					SE_SpawnItem,				4, 0, 0 },
	{ "SpawnNPC",					SE_SpawnNPC,				4, 0, 0 },
	{ "GetPackOwner",				SE_GetPackOwner,			2, 0, 0 },
	{ "CalcTargetedItem",			SE_CalcTargetedItem,		1, 0, 0 },
	{ "CalcTargetedChar",			SE_CalcTargetedChar,		1, 0, 0 },
	{ "Gump_CreateGump",			SE_Gump_CreateGump,			0, 0, 0 },
	{ "Gump_Delete",				SE_Gump_Delete,				1, 0, 0 },
	{ "Gump_AddCheckbox",			SE_Gump_AddCheckbox,		6, 0, 0 },
	{ "Gump_AddButton",				SE_Gump_AddButton,			7, 0, 0 },
	{ "Gump_AddText",				SE_Gump_AddText,			6, 0, 0 },
	{ "Gump_AddTextEntry",			SE_Gump_AddTextEntry,		9, 0, 0 },
	{ "Gump_AddRadio",				SE_Gump_AddRadio,			6, 0, 0 },
	{ "Gump_AddPage",				SE_Gump_AddPage,			2, 0, 0 },
	{ "Gump_AddPicture",			SE_Gump_AddPicture,			4, 0, 0 },
	{ "Gump_AddGump",				SE_Gump_AddGump,			4, 0, 0 },
	{ "Gump_AddBackground",			SE_Gump_AddBackground,		6, 0, 0 },
	{ "Gump_SendToSocket",			SE_Gump_SendToSocket,		2, 0, 0 },
	{ "GetTileIDAtMapCoord",		SE_GetTileIDAtMapCoord,		3, 0, 0 },
	{ "GetLightLevel",				SE_GetLightLevel,			1, 0, 0 },
	
	{ "FileOpen",					SE_FileOpen,				2, 0, 0 },
	{ "FileClose",					SE_FileClose,				1, 0, 0 },
	{ "Write",						SE_Write,					2, 0, 0 },
	{ "Read",						SE_Read,					2, 0, 0 },
	{ "ReadUntil",					SE_ReadUntil,				2, 0, 0 },

	// Added by DarkStorm
	{ "GetRaceCount",				SE_GetRaceCount,			0, 0, 0 },

	// Object Based Functions
	{ "AddNPC",						JS_AddNPC,					1, 0, 0 },
	{ "AddItem",					JS_AddItem,					1, 0, 0 },
	{ "ItembySerial",				JS_ItembySerial,			1, 0, 0 },
	{ "CharbySerial",				JS_CharbySerial,			1, 0, 0 },

	{ "GetWorldBrightLevel",		JS_GetWorldBrightLevel,		0, 0, 0 },
	
	{ NULL,							NULL,						0, 0, 0 }, 
};

void UOX3ErrorReporter( JSContext *cx, const char *message, JSErrorReport *report )
{
	// If we're loading the world then do NOT print out anything!
//	if( Loaded != 1 ) 
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

cScript::cScript( string targFile ) : isFiring( false )
{
	eventPresence[0] = eventPresence[1] = 0xFFFFFFFF;
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
		return;
	}
	jsval rval;
	JSBool ok = JS_ExecuteScript( targContext, targObject, targScript, &rval );
	if( ok != JS_TRUE )
	{
		JSString *str = JS_ValueToString( targContext, rval ); 
		Console << "script result: " << JS_GetStringBytes( str ) << myendl;
	}

	// Expose Classes to the JS Engine
	// We may NOT define Constructors here6
	GumpProto   =	JS_InitClass( targContext, targObject, targObject, &UOXGump_class, Gump, 0, NULL, CGump_Methods, NULL, CGump_Methods );
	CharProto   =	JS_InitClass( targContext, targObject, targObject, &UOXChar_class, NULL, 0, CCharacterProps, CChar_Methods, CCharacterProps, CChar_Methods );
	ItemProto   =	JS_InitClass( targContext, targObject, targObject, &UOXItem_class, NULL, 0, CItemProps, CItem_Methods, CItemProps, CItem_Methods );
	SpellProto  =	JS_InitClass( targContext, targObject, targObject, &UOXSpell_class, NULL, 0, CSpellProperties, NULL, CSpellProperties, NULL );
	SpellsProto =	JS_InitClass( targContext, targObject, targObject, &UOXSpells_class, NULL, 0, NULL, NULL, NULL, NULL );
	SocketProto =	JS_InitClass( targContext, targObject, targObject, &UOXSocket_class, NULL, 0, CSocketProps, CSocket_Methods, CSocketProps, CSocket_Methods );

	// Init the global Spells[] object
	JS_DefineObject(targContext, targObject, "Spells", &UOXSpells_class, SpellsProto, 0 );

	// let's acquire items 0 and 1, and chars 0 and 1, by default, so that
	// we can reuse these for parameter stuff
	AcquireObject( IUE_CHAR );
	AcquireObject( IUE_CHAR );
	AcquireObject( IUE_ITEM );
	AcquireObject( IUE_ITEM );
	AcquireObject( IUE_SOCK );
	
}

cScript::~cScript()
{
	RemoveFromRoot();
	JS_GC( targContext );

	Trigger->UnregisterObject( targObject );
	if( targScript != NULL )
		JS_DestroyScript( targContext, targScript );
	JS_GC( targContext );
	if( targContext != NULL )
		JS_DestroyContext( targContext );
	for( UI32 i = 0; i < gumpDisplays.size(); i++ )
		delete gumpDisplays[i];
	gumpDisplays.resize( 0 );
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

bool cScript::OnCreate( cBaseObject *thingCreated )
{
	if( thingCreated == NULL )
		return false;
	if( !EventExists( seOnCreate ) )
		return false;
	
	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onCreate", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnCreate, false );
		return false;
	}

	jsval rval, params[2];
	UI08 paramType = 0;
	if( thingCreated->GetObjType() != OT_CHAR )
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, thingCreated );
		params[0] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}
	else
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, thingCreated );
		params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
		paramType = 1;
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

bool cScript::OnDelete( cBaseObject *thingDestroyed )
{
	if( thingDestroyed == NULL )
		return false;
	if( !EventExists( seOnDelete ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDelete", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnDelete, false );
		return false;
	}

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

bool cScript::OnSpeech( const char *speech, CChar *personTalking, CChar *talkingTo )
{
	if( speech == NULL || personTalking == NULL || talkingTo == NULL )
		return false;
	if( !EventExists( seOnSpeech ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpeech", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpeech, false );
		return false;
	}

	jsval params[3], rval;
	JSString *strSpeech = NULL;
	
	char *lwrSpeech = new char[strlen(speech)+1];
	strcpy( lwrSpeech, speech );
	strlwr( lwrSpeech );

	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );// speech );

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
		SetEventExists( seOnSpeech, false );
	return ( retVal == JS_TRUE );
}

bool cScript::InRange( CChar *person, CChar *targPlayer )
{
	if( person == NULL || targPlayer == NULL )
		return false;
	if( !EventExists( seInRange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "inRange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seInRange, false );
		return false;
	}

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
	if( person == NULL || targItem == NULL )
		return false;
	if( !EventExists( seInRange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "inRange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seInRange, false );
		return false;
	}

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

bool cScript::OnCollide( cSocket *tSock, CChar *objColliding, cBaseObject *objCollideWith )
{
	if( !EventExists( seOnCollide ) )
		return false;
	if( objColliding == NULL || objCollideWith == NULL || tSock == NULL )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onCollide", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnCollide, false );
		return false;
	}

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
	if( thief == NULL || theft == NULL )
		return false;
	if( !EventExists( seOnSteal ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSteal", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSteal, false );
		return false;
	}

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

bool cScript::OnDispel( cBaseObject *dispelled )
{
	if( dispelled == NULL )
		return false;
	if( !EventExists( seOnDispel ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDispel", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnDispel, false );
		return false;
	}

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

bool cScript::OnSkill( cBaseObject *skillUse, SI08 skillUsed )
{
	if( skillUse == NULL )
		return false;
	if( !EventExists( seOnSkill ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSkill", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSkill, false );
		return false;
	}

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
	if( attacker == NULL || defender == NULL )
		return false;
	if( !EventExists( seOnAttack ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onAttack", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnAttack, false );
		return false;
	}

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
	if( attacker == NULL || defender == NULL )
		return false;
	if( !EventExists( seOnDefense ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDefense", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnDefense, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnSkillGain ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSkillGain", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSkillGain, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnStatGained ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onStatGained", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnStatGained, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnStatGain ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onStatGain", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnStatGain, false );
		return false;
	}

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
	if( item == NULL || dropper == NULL )
		return false;
	if( !EventExists( seOnDrop ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDrop", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnDrop, false );
		return false;
	}

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
	if( item == NULL || pickerUpper == NULL )
		return false;
	if( !EventExists( seOnPickup ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onPickup", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnPickup, false );
		return false;
	}

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
	if( swinging == NULL || swinger == NULL || swingTarg == NULL )
		return false;
	if( !EventExists( seOnSwing ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSwing", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSwing, false );
		return false;
	}

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
	if( decaying == NULL )
		return false;
	if( !EventExists( seOnDecay ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDecay", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnDecay, false );
		return false;
	}

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
	if( transferred == NULL || source == NULL || target == NULL )
		return false;
	if( !EventExists( seOnTransfer ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onTransfer", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnTransfer, false );
		return false;
	}

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

bool cScript::OnLeaving( CMultiObj *left, cBaseObject *leaving )
{
	if( left == NULL || leaving == NULL )
		return false;
	if( !EventExists( seOnLeaving ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onLeaving", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnLeaving, false );
		return false;
	}

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
	if( equipper == NULL || equipping == NULL )
		return false;

	if( !EventExists( seOnEquip ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onEquip", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnEquip, false );
		return false;
	}

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
	if( equipper == NULL || equipping == NULL )
		return false;
	if( !EventExists( seOnUnequip ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onUnequip", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnUnequip, false );
		return false;
	}

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

bool cScript::OnUse( CChar *user, CItem *iUsing )
{
	if( user == NULL || iUsing == NULL )
		return false;
	if( !EventExists( seOnUse ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onUse", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnUse, false );
		return false;
	}

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
	return ( retVal == JS_TRUE );
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item)
//|	Date					-	04/18/2002
//|	Developer(s)	-	MACTEP
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Event to signal when an item is dropped on an NPC
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o	
bool cScript::OnDropItemOnNpc( CChar *srcChar, CChar *dstChar, CItem *item)
{
	if( srcChar == NULL || dstChar == NULL || item == NULL)
		return false;
	if( !EventExists( seOnDropItemOnNpc ) )
		return false;
		
	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDropItemOnNpc", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnUse, false );
		return false;
	}

	jsval rval, params[2];

	JS_SetPrivate( targContext, charObjects[0].toUse, srcChar );
	JS_SetPrivate( targContext, charObjects[1].toUse, dstChar );
	JS_SetPrivate( targContext, itemObjects[0].toUse, item );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( charObjects[1].toUse );
	params[2] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDropItemOnNpc", 3, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUse, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, charObjects[1].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE ) && (JSVAL_TO_BOOLEAN(rval) == JS_TRUE);
	
}

bool cScript::OnEntrance( CMultiObj *left, cBaseObject *leaving )
{
	if( left == NULL || leaving == NULL )
		return false;
	if( !EventExists( seOnEntrance ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onEntrance", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnEntrance, false );
		return false;
	}

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

bool cScript::OnUsage( CChar *user, CItem *iUsing )
{
	if( user == NULL || iUsing == NULL )
		return false;
	if( !EventExists( seOnUsage ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onUsage", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnUsage, false );
		return false;
	}

	jsval rval, params[2];

	JS_SetPrivate( targContext, charObjects[0].toUse, user );
	JS_SetPrivate( targContext, itemObjects[0].toUse, iUsing );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onUsage", 2, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnUsage, false );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OutOfRange( CChar *person, cBaseObject *objVanish )
{
	if( person == NULL || objVanish == NULL )
		return false;
	if( !EventExists( seOutOfRange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "outOfRange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOutOfRange, false );
		return false;
	}

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

bool cScript::OnLogin( cSocket *sockPlayer, CChar *pPlayer )
{
	if( pPlayer == NULL )
		return false;
	if( !EventExists( seOnLogin ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onLogin", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnLogin, false );
		return false;
	}

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
//|	Function/Class-	
//|	Date					-	10/06/2002
//|	Developer(s)	-	Brakhtus
//|	Company/Team	-	
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
bool cScript::OnLogout( cSocket *sockPlayer, CChar *pPlayer ) 
{ 
	if( pPlayer == NULL ) 
		return false; 
	if( !EventExists( seOnLogout ) ) 
		return false;

	jsval Func = JSVAL_NULL; 
	JS_GetProperty( targContext, targObject, "onLogout", &Func ); 

	if( Func == JSVAL_VOID ) 
	{ 
		SetEventExists( seOnLogout, false ); 
		return false; 
	}

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
//|	Function/Class-	
//|	Date					-	10/06/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
bool cScript::OnClick(cSocket *sockPlayer, CItem *iClicked)
{
	if(!EventExists(seOnClick))
		return false;
	jsval Func = JSVAL_NULL;
	JS_GetProperty(targContext, targObject, "onClick", &Func);
	if(Func==JSVAL_VOID)
	{
		SetEventExists(seOnClick,false);
		return false;
	}
	jsval params[2], rval;
	JS_SetPrivate(targContext, sockObjects[0].toUse, sockPlayer);
	JS_SetPrivate(targContext, itemObjects[0].toUse, iClicked);
	params[0]=OBJECT_TO_JSVAL(sockObjects[0].toUse);
	params[1]=OBJECT_TO_JSVAL(itemObjects[0].toUse);
	JSBool retVal = JS_CallFunctionName(targContext, targObject,"onClick",2,params,&rval);
	if(retVal==JS_FALSE)
		SetEventExists(seOnClick,false);
	JS_SetPrivate(targContext,sockObjects[0].toUse,NULL);
	JS_SetPrivate(targContext,itemObjects[0].toUse,NULL);
	return (retVal == JS_TRUE);
}

bool cScript::OnFall( CChar *pFall, SI08 fallDistance )
{
	if( pFall == NULL )
		return false;
	if( !EventExists( seOnFall ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onFall", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnFall, false );
		return false;
	}

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
	if( pSliver == NULL )
		return false;
	if( !EventExists( seOnAISliver ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onAISliver", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnAISliver, false );
		return false;
	}

	jsval params[1], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pSliver );
	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onAISliver", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnAISliver, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnSystemSlice( void )
{
	if( !EventExists( seOnSystemSlice ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSystemSlice", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSystemSlice, false );
		return false;
	}

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

bool cScript::OnTimer( cBaseObject *tObject, UI08 timerID )
{
	if( tObject == NULL )
		return false;
	if( !EventExists( seOnTimer ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onTimer", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnTimer, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnStatLoss ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onStatLoss", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnStatLoss, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnStatChange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onStatChange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnStatChange, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnSkillLoss ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSkillLoss", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSkillLoss, false );
		return false;
	}

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
	if( player == NULL )
		return false;
	if( !EventExists( seOnSkillChange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSkillChange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSkillChange, false );
		return false;
	}

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
	return( (eventPresence[index]&flagToSet) == flagToSet );
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

bool cScript::OnDeath( CChar *pDead )
{
	if( pDead == NULL )
		return false;
	if( !EventExists( seOnDeath ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onDeath", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnDeath, false );
		return false;
	}

	jsval params[1], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pDead );
	params[0] = INT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onDeath", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnDeath, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnResurrect( CChar *pAlive )
{
	if( pAlive == NULL )
		return false;
	if( !EventExists( seOnResurrect ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onResurrect", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnResurrect, false );
		return false;
	}

	jsval params[1], rval;
	JS_SetPrivate( targContext, charObjects[0].toUse, pAlive );
	params[0] = INT_TO_JSVAL( charObjects[0].toUse );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onResurrect", 1, params, &rval );
	if( retVal == JS_FALSE )
		SetEventExists( seOnResurrect, false );
	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

bool cScript::OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus )
{
	if( pChanging == NULL )
		return false;
	if( !EventExists( seOnFlagChange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onFlagChange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnFlagChange, false );
		return false;
	}

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

bool cScript::DoCallback( cSocket *tSock, SERIAL targeted, UI08 callNum )
{
	if( tSock == NULL )
		return false;
	jsval params[2], rval;
	JS_SetPrivate( targContext, sockObjects[0].toUse, tSock );
	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	if( targeted >= 0x40000000 )
	{
		JS_SetPrivate( targContext, itemObjects[0].toUse, calcItemObjFromSer( targeted ) );
		params[1] = OBJECT_TO_JSVAL( itemObjects[0].toUse );
	}
	else
	{
		JS_SetPrivate( targContext, charObjects[0].toUse, calcCharObjFromSer( targeted ) );
		params[1] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	}
	char targetFunction[32];
	sprintf( targetFunction, "onCallback%i", callNum );
	JSBool retVal = JS_CallFunctionName( targContext, targObject, targetFunction, 2, params, &rval );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
	if( targeted >= 0x40000000 )
		JS_SetPrivate( targContext, itemObjects[0].toUse, NULL );
	else
		JS_SetPrivate( targContext, charObjects[0].toUse, NULL );
	return ( retVal == JS_TRUE );
}

JSObject *cScript::Object( void ) const
{
	return targObject;
}

bool cScript::OnHungerChange( CChar *pChanging, SI08 newStatus )
{
	if( pChanging == NULL )
		return false;
	if( !EventExists( seOnHungerChange ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onHungerChange", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnHungerChange, false );
		return false;
	}

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
	if( stealing == NULL || stolenFrom == NULL || stolen == NULL )
		return false;
	if( !EventExists( seOnStolenFrom ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onStolenFrom", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnStolenFrom, false );
		return false;
	}

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
	if( snooped == NULL || snooper == NULL )
		return false;
	if( !EventExists( seOnSnooped ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSnooped", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSnooped, false );
		return false;
	}

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

SI32 cScript::NewGumpList( void )
{
	SI32 retVal = gumpDisplays.size();
	
	SEGump *toAdd = new SEGump;
	toAdd->one = new stringList;
	toAdd->two = new stringList;

	gumpDisplays.push_back( toAdd );
	return retVal;
}
SEGump * cScript::GetGumpList( SI32 index )
{
	if( index < 0 || (UI32) index >= gumpDisplays.size() )
		return NULL;
	return gumpDisplays[index];
}
void cScript::RemoveGumpList( SI32 index )
{
	if( index < 0 || (UI32) index >= gumpDisplays.size() )
		return;

	delete gumpDisplays[index]->one;
	delete gumpDisplays[index]->two;
	delete gumpDisplays[index];

	for( SI32 i = index; i < (SI32) gumpDisplays.size() - 1; i++ )
		gumpDisplays[i] = gumpDisplays[i+1];
	gumpDisplays.resize( gumpDisplays.size() - 1 );
}
void cScript::SendGumpList( SI32 index, UOXSOCKET toSendTo )
{
	if( index < 0 || (UI32) index >= gumpDisplays.size() )
		return;
	cSocket *mSock = calcSocketObjFromSock( toSendTo );
	if( mSock == NULL )
		return;
	UI16 mTrig = Trigger->FindObject( targObject );
	mSock->TempInt( mTrig );
	SendVecsAsGump( mSock, *(gumpDisplays[index]->one), *(gumpDisplays[index]->two), 20, INVALIDSERIAL );
}

void cScript::HandleGumpPress( cSocket *pressing, long button )
{
	if( pressing == NULL )
		return;
	jsval params[2], rval;
	JS_SetPrivate( targContext, sockObjects[0].toUse, pressing );
	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[1] = INT_TO_JSVAL( button );
	JS_CallFunctionName( targContext, targObject, "onGumpPress", 2, params, &rval );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
}
void cScript::HandleGumpInput( cSocket *pressing )
{
	if( pressing == NULL )
		return;
	jsval params[1], rval;
	JS_SetPrivate( targContext, sockObjects[0].toUse, pressing );
	params[0] = OBJECT_TO_JSVAL( sockObjects[0].toUse );
	params[0] = INT_TO_JSVAL( pressing );
	JS_CallFunctionName( targContext, targObject, "onGumpInput", 1, params, &rval );
	JS_SetPrivate( targContext, sockObjects[0].toUse, NULL );
}

bool cScript::OnEnterRegion( CChar *entering, SI16 region )
{
	if( entering == NULL )
		return false;
	if( !EventExists( seOnEnterRegion ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onEnterRegion", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnEnterRegion, false );
		return false;
	}

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
	if( leaving == NULL )
		return false;
	if( !EventExists( seOnLeaveRegion ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onLeaveRegion", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnLeaveRegion, false );
		return false;
	}

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
	if( target == NULL || caster == NULL )
		return false;
	if( !EventExists( seOnSpellTarget ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpellTarget", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpellTarget, false );
		return false;
	}

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
	if( target == NULL || caster == NULL )
		return false;
	if( !EventExists( seOnSpellTarget ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpellTarget", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpellTarget, false );
		return false;
	}

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
	UI32 toSearch = 0;
	switch( iType )
	{
	case IUE_RACE:
		for( toSearch = 0; toSearch < raceObjects.size(); toSearch++ )
		{
			if( !raceObjects[toSearch].inUse )
				return toSearch;
		}
		break;
	case IUE_CHAR:
		for( toSearch = 0; toSearch < charObjects.size(); toSearch++ )
		{
			if( !charObjects[toSearch].inUse )
				return toSearch;
		}
		break;
	case IUE_ITEM:
		for( toSearch = 0; toSearch < itemObjects.size(); toSearch++ )
		{
			if( !itemObjects[toSearch].inUse )
				return toSearch;
		}
		break;
	case IUE_SOCK:
		for( toSearch = 0; toSearch < sockObjects.size(); toSearch++ )
		{
			if( !sockObjects[toSearch].inUse )
				return toSearch;
		}
		break;
	case IUE_GUILD:
		for( toSearch = 0; toSearch < guildObjects.size(); toSearch++ )
		{
			if( !guildObjects[toSearch].inUse )
				return toSearch;
		}
		break;
	case IUE_REGION:
		for( toSearch = 0; toSearch < regionObjects.size(); toSearch++ )
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
		for( toSearch = 0; toSearch < raceObjects.size(); toSearch++ )
		{
			if( raceObjects[toSearch].toUse == toFind )
				return toSearch;
		}
		break;
	case IUE_CHAR:
		for( toSearch = 0; toSearch < charObjects.size(); toSearch++ )
		{
			if( charObjects[toSearch].toUse == toFind )
				return toSearch;
		}
		break;
	case IUE_ITEM:
		for( toSearch = 0; toSearch < itemObjects.size(); toSearch++ )
		{
			if( itemObjects[toSearch].toUse == toFind )
				return toSearch;
		}
		break;
	case IUE_SOCK:
		for( toSearch = 0; toSearch < sockObjects.size(); toSearch++ )
		{
			if( sockObjects[toSearch].toUse == toFind )
				return toSearch;
		}
		break;
	case IUE_GUILD:
		for( toSearch = 0; toSearch < guildObjects.size(); toSearch++ )
		{
			if( guildObjects[toSearch].toUse == toFind )
				return toSearch;
		}
		break;
	case IUE_REGION:
		for( toSearch = 0; toSearch < regionObjects.size(); toSearch++ )
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
		UI32 currentPos;
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
	UI32 i = 0;
	for( i = 0; i < raceObjects.size(); i++ )
	{
		JS_UnlockGCThing( targContext, raceObjects[i].toUse );
	}

	for( i = 0; i < charObjects.size(); i++ )
	{
		JS_UnlockGCThing( targContext, charObjects[i].toUse );
	}

	for( i = 0; i < itemObjects.size(); i++ )
	{
		JS_UnlockGCThing( targContext, itemObjects[i].toUse );
	}

	for( i = 0; i < sockObjects.size(); i++ )
	{
		JS_UnlockGCThing( targContext, sockObjects[i].toUse );
	}

	for( i = 0; i < guildObjects.size(); i++ )
	{
		JS_UnlockGCThing( targContext, guildObjects[i].toUse );
	}

	for( i = 0; i < regionObjects.size(); i++ )
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
	if( tChar == NULL )
		return -2;
	
	if( !EventExists( seOnSpellCast ) )
		return -2;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpellCast", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpellCast, false );
		return -2;
	}

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
	if( tChar == NULL )
		return -2;
	
	if( !EventExists( seOnScrollCast ) )
		return -2;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onScrollCast", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnScrollCast, false );
		return -2;
	}

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
	if( tChar == NULL )
		return false;
	
	if( !EventExists( seOnSpellSuccess ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpellSuccess", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpellSuccess, false );
		return false;
	}

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
	if( myChar == NULL )
		return true;
	
	if( !EventExists( seOnTalk ) )
		return true;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onTalk", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnTalk, false );
		return true;
	}

	jsval params[2], rval;
	JSString *strSpeech = NULL;
	
	char *lwrSpeech = new char[strlen(mySpeech)+1];
	strcpy( lwrSpeech, mySpeech );
	strlwr( lwrSpeech );

	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );

	JS_SetPrivate( targContext, charObjects[0].toUse, myChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );
	params[1] = STRING_TO_JSVAL( strSpeech );
	//params[1] = INT_TO_JSVAL( SpellID );

	JSBool retVal = JS_CallFunctionName( targContext, targObject, "onTalk", 2, params, &rval );

	JS_SetPrivate( targContext, charObjects[0].toUse, NULL );

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
	if( myChar == NULL )
		return true;
	
	if( !EventExists( seOnSpeechInput ) )
		return true;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpeechInput", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpeechInput, false );
		return true;
	}

	jsval params[4], rval;
	JSString *strSpeech = NULL;
	
	char *lwrSpeech = new char[strlen(mySpeech)+1];
	strcpy( lwrSpeech, mySpeech );
	strSpeech = JS_NewStringCopyZ( targContext, lwrSpeech );

	JS_SetPrivate( targContext, charObjects[0].toUse, myChar );

	params[0] = OBJECT_TO_JSVAL( charObjects[0].toUse );

	if( myItem == NULL )
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

	if( myItem != NULL )
		JS_SetPrivate( targContext, itemObjects[0].toUse, myItem ); // If it's null it's still ok

	if( retVal == JS_FALSE )
	{
		SetEventExists( seOnSpeechInput, false );
		return true;
	}

	return( rval == JSVAL_TRUE );
}

bool cScript::OnSpellGain( CItem *book, const UI08 spellNum )
{
	if( book == NULL )
		return false;
	if( !EventExists( seOnSpellGain ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpellGain", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpellGain, false );
		return false;
	}

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
	if( book == NULL )
		return false;
	if( !EventExists( seOnSpellLoss ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSpellLoss", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSpellLoss, false );
		return false;
	}

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
	if( myChar == NULL || skill > ALLSKILLS )
		return false;
	if( !EventExists( seOnSkillCheck ) )
		return false;

	jsval Func = JSVAL_NULL;
	JS_GetProperty( targContext, targObject, "onSkillCheck", &Func );
	
	if( Func == JSVAL_VOID )
	{
		SetEventExists( seOnSkillCheck, false );
		return false;
	}

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
