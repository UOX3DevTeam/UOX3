//o--------------------------------------------------------------------------o
//|	File			-	UOXJSMethods.cpp
//|	Date			-	
//|	Developers		-	DarkStorm
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|						1.0		Dark-Storm	20th December, 2001 Initial implementation
//|								Methods for the javascript objects
//|									
//|								List of supported Classes:
//|									- Gumps
//|									- Char
//|									- Item
//|									- Socket
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __UOXJSMethods__
#define __UOXJSMethods__

namespace UOX
{

// Object Constructors
JSBool Gump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool GumpData( JSContext *cx, JSObject *obj, uintN argc, jsval *argv,jsval *rval );
JSBool UOXCFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv,jsval *rval );

// Gump Methods
JSBool CGump_Free(					JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddBackground(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddButton(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddCheckbox(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddCheckerTrans(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddCroppedText(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddGroup(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddGump(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddHTMLGump(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddPage(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddPicture(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddRadio(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddText(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddTextEntry(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddTiledGump(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddXMFHTMLGump(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddXMFHTMLGumpColor(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_NoClose(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_NoDispose(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_NoMove(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_NoResize(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_Send(					JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// GumpData Methods
JSBool CGumpData_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGumpData_getID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGumpData_getEdit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGumpData_getButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Character Methods
JSBool CChar_Action( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_EmoteMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Freeze( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Unfreeze( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_OpenBank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_DirectionTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_TurnToward( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_UseResource( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_ResourceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_CheckSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_FindItemLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_SpeechInput( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_CastSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_MagicEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_MakeMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_WanderBox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Follow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Dismount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_SetPoisoned( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_ExplodeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_SetInvisible( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_YellMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_WhisperMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_ExecuteCommand( JSContext *cs, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_WalkTo( JSContext *cs, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_RunTo( JSContext *cs, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_GetTimer( JSContext *cs, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_SetTimer( JSContext *cs, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_OpenLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_BoltEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Gate( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Recall( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CChar_Mark( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Item Methods
JSBool CItem_OpenPlank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_IsMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_IsInMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_IsOnBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_IsOnOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_AddToBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_AddToOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_RemoveFromBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_RemoveFromOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_SetCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_SetPoison( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_Refresh( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_Glow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_UnGlow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CItem_PlaceInPack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );


// BaseObject Methods
JSBool CBase_TextMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_Delete( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_Teleport( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_StaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_GetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_SetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_GetNumTags( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_InRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_StartTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_KillTimers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_ApplySection( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_FirstItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_NextItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_FinishedItems( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_DistanceTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Socket Methods
JSBool CSocket_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_Disconnect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_OpenURL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_GetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_GetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_OpenGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_WhoList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_Midi( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Guild Methods
JSBool CGuild_AcceptRecruit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Misc
JSBool CMisc_SoundEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_SellTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_BuyFrom( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_HasSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_RemoveSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_CustomTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_PopUpTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Functions
JSBool JS_CharbySerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_ItembySerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_AddNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_AddItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_WorldBrightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_WorldDarkLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_WorldDungeonLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Race methods
JSBool CRace_CanWearArmour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CRace_IsValidHairColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CRace_IsValidSkinColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CRace_IsValidBeardColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// File methods

JSBool CFile_Open( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CFile_Close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CFile_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CFile_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CFile_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CFile_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Account methods
//JSBool CAccount_GetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
//JSBool CAccount_SetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CAccount_AddAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CAccount_DelAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
//JSBool CAccount_ModAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
//JSBool CAccount_SaveAccounts( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );


static JSFunctionSpec CGump_Methods[] =  
{ 	
	{ "Free",					CGump_Free,					0, 0, 0 },
	{ "AddBackground",			CGump_AddBackground,		5, 0, 0 },
	{ "AddButton",				CGump_AddButton,			6, 0, 0 },
	{ "AddCheckbox",			CGump_AddCheckbox,			5, 0, 0 },
	{ "AddCheckerTrans",		CGump_AddCheckerTrans,		4, 0, 0 },
	{ "AddCroppedText",			CGump_AddCroppedText,		6, 0, 0 },
	{ "AddGroup",				CGump_AddGroup,				1, 0, 0 },
	{ "AddGump",				CGump_AddGump,				3, 0, 0 },
	{ "AddHTMLGump",			CGump_AddHTMLGump,			7, 0, 0 },
	{ "AddPage",				CGump_AddPage,				1, 0, 0 },
	{ "AddPicture",				CGump_AddPicture,			3, 0, 0 },
	{ "AddRadio",				CGump_AddRadio,				5, 0, 0 },
	{ "AddText",				CGump_AddText,				4, 0, 0 },
	{ "AddTextEntry",			CGump_AddTextEntry,			8, 0, 0 },
	{ "AddTiledGump",			CGump_AddTiledGump,			5, 0, 0 },
	{ "AddXMFHTMLGump",			CGump_AddXMFHTMLGump,		7, 0, 0 },
	{ "AddXMFHTMLGumpColor",	CGump_AddXMFHTMLGumpColor,	8, 0, 0 },
	{ "NoClose",				CGump_NoClose,				0, 0, 0 },
	{ "NoDispose",				CGump_NoDispose,			0, 0, 0 },
	{ "NoMove",					CGump_NoMove,				0, 0, 0 },
	{ "NoResize",				CGump_NoResize,				0, 0, 0 },
	{ "Send",					CGump_Send,					1, 0, 0 },
	{ NULL,						NULL,						0, 0, 0 }
};

static JSFunctionSpec CChar_Methods[] =  
{ 	
	{ "TextMessage",		CBase_TextMessage,		1, 0, 0 },
	{ "YellMessage",		CChar_YellMessage,		1, 0, 0 },
	{ "WhisperMessage",		CChar_WhisperMessage,	1, 0, 0 },
	{ "EmoteMessage",		CChar_EmoteMessage,		1, 0, 0 },
	{ "Delete",				CBase_Delete,			0, 0, 0 },
	{ "DoAction",			CChar_Action,			1, 0, 0 },
	{ "StaticEffect",		CBase_StaticEffect,		3, 0, 0 },
	{ "Teleport",			CBase_Teleport,			3, 0, 0 },
	{ "SetLocation",		CBase_Teleport,			3, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,		2, 0, 0 },
	{ "SellTo",				CMisc_SellTo,			1, 0, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,			1, 0, 0 },
	{ "HasSpell",			CMisc_HasSpell,			1, 0, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,		1, 0, 0 },
	{ "GetTag",				CBase_GetTag,			1, 0, 0 },
	{ "SetTag",				CBase_SetTag,			2, 0, 0 },
	{ "GetNumTags",			CBase_GetNumTags,		0, 0, 0 },
	{ "OpenBank",			CChar_OpenBank,			1, 0, 0 },
	{ "DirectionTo",		CChar_DirectionTo,		1, 0, 0 },
	{ "TurnToward",			CChar_TurnToward,		1, 0, 0 },
	{ "ResourceCount",		CChar_ResourceCount,	2, 0, 0 },
	{ "UseResource",		CChar_UseResource,		3, 0, 0 },
	{ "CustomTarget",		CMisc_CustomTarget,		1, 0, 0 },
	{ "PopUpTarget",		CMisc_PopUpTarget,		1, 0, 0 },
	{ "InRange",			CBase_InRange,			1, 0, 0 },
	{ "FindItemLayer",		CChar_FindItemLayer,	1, 0, 0 },
	{ "StartTimer",			CBase_StartTimer,		2, 0, 0 },
	{ "CheckSkill",			CChar_CheckSkill,		3, 0, 0 },
	{ "SpeechInput",		CChar_SpeechInput,		1, 0, 0 },
	{ "Freeze",				CChar_Freeze,			0, 0, 0 },
	{ "Unfreeze",			CChar_Unfreeze,			0, 0, 0 },
	{ "CastSpell",			CChar_CastSpell,		2, 0, 0 },	
	{ "SysMessage",			CChar_SysMessage,		1, 0, 0 },
	{ "MagicEffect",		CChar_MagicEffect,		2, 0, 0 },
	{ "GetSerial",			CChar_GetSerial,		1, 0, 0 },
	{ "MakeMenu",			CChar_MakeMenu,			1, 0, 0 },
	{ "WanderBox",			CChar_WanderBox,		4, 0, 0 },
	{ "Follow",				CChar_Follow,			1, 0, 0 },
	{ "Dismount",			CChar_Dismount,			0, 0, 0 },
	{ "SetPoisoned",		CChar_SetPoisoned,		2, 0, 0 },
	{ "ExplodeItem",		CChar_ExplodeItem,		1, 0, 0 },
	{ "SetInvisible",		CChar_SetInvisible,		2, 0, 0 },
	{ "ApplySection",		CBase_ApplySection,		1, 0, 0 },
	{ "AddSpell",			CChar_AddSpell,			1, 0, 0 },
	{ "SpellFail",			CChar_SpellFail,		0, 0, 0 },
	{ "FirstItem",			CBase_FirstItem,		0, 0, 0 },
	{ "NextItem",			CBase_NextItem,			0, 0, 0 },
	{ "FinishedItems",		CBase_FinishedItems,	0, 0, 0 },
	{ "ExecuteCommand",     CChar_ExecuteCommand,   0, 0, 0 },
	{ "WalkTo",				CChar_WalkTo,			1, 0, 0 },
	{ "RunTo",				CChar_RunTo,			1, 0, 0 },
	{ "DistanceTo",			CBase_DistanceTo,		1, 0, 0 },
	{ "OpenLayer",			CChar_OpenLayer,		2, 0, 0 },
	{ "BoltEffect",			CChar_BoltEffect,		0, 0, 0 },
	{ "Gate",				CChar_Gate,				1, 0, 0 },
	{ "Recall",				CChar_Recall,			1, 0, 0 },
	{ "Mark",				CChar_Mark,				1, 0, 0 },

	{ "GetTimer",			CChar_GetTimer,			1, 0, 0 },
	{ "SetTimer",			CChar_SetTimer,			2, 0, 0 },

	{ NULL,					NULL,					0, 0, 0 }
};

static JSFunctionSpec CItem_Methods[] =  
{ 	
	{ "KillTimers",			CBase_KillTimers,			0, 0, 0 },
	{ "Delete",				CBase_Delete,				0, 0, 0 },
	{ "TextMessage",		CBase_TextMessage,			1, 0, 0 },
	{ "Teleport",			CBase_Teleport,				3, 0, 0 },
	{ "StaticEffect",		CBase_StaticEffect,			4, 0, 0 },
	{ "SetLocation",		CBase_Teleport,				3, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,			2, 0, 0 },
	{ "HasSpell",			CMisc_HasSpell,				1, 0, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,			1, 0, 0 },
	{ "GetTag",				CBase_GetTag,				1, 0, 0 },
	{ "SetTag",				CBase_SetTag,				1, 0, 0 },
	{ "GetNumTags",			CBase_GetNumTags,			0, 0, 0 },
	{ "InRange",			CBase_InRange,				1, 0, 0 },
	{ "StartTimer",			CBase_StartTimer,			2, 0, 0 },
	{ "OpenPlank",			CItem_OpenPlank,			0, 0, 0 },
	{ "IsMulti",			CItem_IsMulti,				0, 0, 0 },
	{ "IsInMulti",			CItem_IsInMulti,			1, 0, 0 },
	{ "IsOnBanList",		CItem_IsOnBanList,			1, 0, 0 },
	{ "IsOnOwnerList",		CItem_IsOnOwnerList,		1, 0, 0 },
	{ "AddToBanList",		CItem_AddToBanList,			1, 0, 0 },
	{ "AddToOwnerList",		CItem_AddToOwnerList,		1, 0, 0 },
	{ "RemoveFromBanList",	CItem_RemoveFromBanList,	1, 0, 0 },
	{ "RemoveFromOwnerList",CItem_RemoveFromOwnerList,	1, 0, 0 },
	{ "GetSerial",			CItem_GetSerial,			1, 0, 0 },
	{ "SetCont",			CItem_SetCont,				1, 0, 0 },
	{ "SetPoison",			CItem_SetPoison,			2, 0, 0 },
	{ "ApplySection",		CBase_ApplySection,			1, 0, 0 },
	{ "Refresh",			CItem_Refresh,				0, 0, 0 },
	{ "ApplyRank",			CItem_ApplyRank,			1, 0, 0 },
	{ "FirstItem",			CBase_FirstItem,			0, 0, 0 },
	{ "NextItem",			CBase_NextItem,				0, 0, 0 },
	{ "FinishedItems",		CBase_FinishedItems,		0, 0, 0 },
	{ "DistanceTo",			CBase_DistanceTo,			1, 0, 0 },
	{ "Glow",				CItem_Glow,					1, 0, 0 },
	{ "UnGlow",				CItem_UnGlow,				1, 0, 0 },
	{ "PlaceInPack",		CItem_PlaceInPack,			1, 0, 0 },
	{ NULL,					NULL,						0, 0, 0 }
};

static JSFunctionSpec CRegion_Methods[] =  
{ 	
	{ NULL,					NULL,				0, 0, 0 },
};

static JSFunctionSpec CSocket_Methods[] =  
{ 	
	{ "SysMessage",			CSocket_SysMessage,	1, 0, 0 },
	{ "Disconnect",			CSocket_Disconnect,	0, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,	2, 0, 0 },
	{ "CustomTarget",		CMisc_CustomTarget, 1, 0, 0 },
	{ "PopUpTarget",		CMisc_PopUpTarget,	1, 0, 0 },
	{ "GetByte",			CSocket_GetByte,	1, 0, 0 },
	{ "GetWord",			CSocket_GetWord,	1, 0, 0 },
	{ "OpenGump",			CSocket_OpenGump,	1, 0, 0 },
	{ "OpenURL",			CSocket_OpenURL,    1, 0, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,		1, 0, 0 },
	{ "SellTo",				CMisc_SellTo,		1, 0, 0 },
	{ "WhoList",			CSocket_WhoList,	0, 0, 0 },
	{ "Midi",				CSocket_Midi,		1, 0, 0 },
	{ NULL,					NULL,				0, 0, 0 }
};

static JSFunctionSpec CGuild_Methods[] =  
{ 	
	{ "AcceptRecruit",		CGuild_AcceptRecruit,	1, 0, 0 },
	{ NULL,					NULL,					0, 0, 0 }
};

static JSFunctionSpec CRace_Methods[] =  
{ 	
	{ "CanWearArmour",		CRace_CanWearArmour,		1, 0, 0 },
	{ "IsValidHairColour",	CRace_IsValidHairColour,	1, 0, 0 },
	{ "IsValidSkinColour",	CRace_IsValidSkinColour,	1, 0, 0 },
	{ "IsValidBeardColour",	CRace_IsValidBeardColour,	1, 0, 0 },
	{ NULL,					NULL,						0, 0, 0 }
};

static JSFunctionSpec CGumpData_Methods[] =  
{ 	
	{ "Free",		CGumpData_Free,			0, 0, 0 },
	{ "getButton",	CGumpData_getButton,	1, 0, 0 },
	{ "getID",		CGumpData_getID,		1, 0, 0 },
	{ "getEdit",	CGumpData_getEdit,		1, 0, 0 },
	{ NULL,			NULL,					0, 0, 0 }
};

static JSFunctionSpec CFile_Methods[] = 
{
	{ "Free",		CFile_Free,				0, 0, 0 },
	{ "Open",		CFile_Open,				2, 0, 0 },
	{ "Close",		CFile_Close,			0, 0, 0 },
	{ "Write",		CFile_Write,			1, 0, 0 },
	{ "Read",		CFile_Read,				1, 0, 0 },
	{ "ReadUntil",	CFile_ReadUntil,		1, 0, 0 },
	{ NULL,			NULL,					0, 0, 0 }
};

static JSFunctionSpec CAccount_Methods[] =
{
	//{ "GetAccount",		CAccount_GetAccount,		1, 0, 0 },
	//{ "SetAccount",   CAccount_SetAccount,		1, 0, 0 },
	{ "AddAccount",		CAccount_AddAccount,		4, 0, 0 },
	{ "DelAccount",		CAccount_DelAccount,		1, 0, 0 },
	//{ "ModAccount",		CAccount_ModAccount,		1, 0, 0 },
	//{	"SaveAccounts",	CAccount_SaveAccounts,	1, 0, 0 },
	{ NULL,			NULL,					0, 0, 0 }
};

}

#endif

