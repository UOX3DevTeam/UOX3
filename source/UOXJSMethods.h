// Version History
// 1.0		Dark-Storm	20th December, 2001
//			Initial implementation
//			Methods for the javascript objects

#ifndef __UOXJSMethods__
#define __UOXJSMethods__

// Object Constructors
JSBool Gump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Gump Methods
JSBool CGump_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddCheckbox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddTextEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddRadio( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddPage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddPicture( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_AddBackground( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_NoClose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CGump_Send( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

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


// BaseObject Methods
JSBool CBase_TextMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_Delete( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_Teleport( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_StaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_GetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_SetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_InRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CBase_StartTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Socket Methods
JSBool CSocket_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CSocket_Disconnect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Guild Methods
JSBool CGuild_AcceptRecruit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Misc
JSBool CMisc_SoundEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_SellTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_BuyFrom( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_HasSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_RemoveSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool CMisc_CustomTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Functions
JSBool JS_CharbySerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_ItembySerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_AddNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_AddItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool JS_GetWorldBrightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

static JSFunctionSpec CGump_Methods[] =  
{ 	
	{ "Free",				CGump_Free,				0, 0, 0 },
	{ "AddCheckbox",		CGump_AddCheckbox,		0, 0, 0 },
	{ "AddButton",			CGump_AddButton,		0, 0, 0 },
	{ "AddText",			CGump_AddText,			0, 0, 0 },
	{ "AddTextEntry",		CGump_AddTextEntry,		0, 0, 0 },
	{ "AddRadio",			CGump_AddRadio,			0, 0, 0 },
	{ "AddPage",			CGump_AddPage,			0, 0, 0 },
	{ "AddPicture",			CGump_AddPicture,		0, 0, 0 },
	{ "AddGump",			CGump_AddGump,			0, 0, 0 },
	{ "AddBackground",		CGump_AddBackground,	0, 0, 0 },
	{ "NoClose",			CGump_NoClose,			0, 0, 0 },
	{ "Send",				CGump_Send,				1, 0, 0 },
	{ NULL,					NULL,					0, 0, 0 },
};

static JSFunctionSpec CChar_Methods[] =  
{ 	
	{ "TextMessage",		CBase_TextMessage,	1, 0, 0 },
	{ "EmoteMessage",		CChar_EmoteMessage, 1, 0, 0 },
	{ "Delete",				CBase_Delete,		0, 0, 0 },
	{ "DoAction",			CChar_Action,		1, 0, 0 },
	{ "StaticEffect",		CBase_StaticEffect,	4, 0, 0 },
	{ "Teleport",			CBase_Teleport,		3, 0, 0 },
	{ "SetLocation",		CBase_Teleport,		3, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,	2, 0, 0 },
	{ "SellTo",				CMisc_SellTo,		1, 0, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,		1, 0, 0 },
	{ "HasSpell",			CMisc_HasSpell,		1, 0, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,	1, 0, 0 },
	{ "GetTag",				CBase_GetTag,		1, 0, 0 },
	{ "SetTag",				CBase_SetTag,		2, 0, 0 },
	{ "OpenBank",			CChar_OpenBank,		1, 0, 0 },
	{ "DirectionTo",		CChar_DirectionTo,	1, 0, 0 },
	{ "TurnToward",			CChar_TurnToward,	1, 0, 0 },
	{ "ResourceCount",		CChar_ResourceCount,2, 0, 0 },
	{ "UseResource",		CChar_UseResource,  3, 0, 0 },
	{ "CustomTarget",		CMisc_CustomTarget, 1, 0, 0 },
	{ "InRange",			CBase_InRange,		1, 0, 0 },
	{ "FindItemLayer",		CChar_FindItemLayer,1, 0, 0 },
	{ "StartTimer",			CBase_StartTimer,	2, 0, 0 },
	{ "CheckSkill",			CChar_CheckSkill,	3, 0, 0 },
	{ "SpeechInput",		CChar_SpeechInput,	1, 0, 0 },
	{ "Freeze",				CChar_Freeze,		0, 0, 0 },
	{ "Unfreeze",			CChar_Unfreeze,		0, 0, 0 },
	{ "CastSpell",			CChar_CastSpell,	2, 0, 0 },	
	{ "SysMessage",			CChar_SysMessage,	1, 0, 0 },
	{ "MagicEffect",		CChar_MagicEffect,	2, 0, 0 },
	{ "GetSerial",			CChar_GetSerial,	1, 0, 0 },
	{ "MakeMenu",			CChar_MakeMenu,		1, 0, 0 },
	{ "WanderBox",			CChar_WanderBox,	4, 0, 0 },
	{ "Follow",				CChar_Follow,		1, 0, 0 },
	{ "Dismount",			CChar_Dismount,		0, 0, 0 },
	{ "SetPoisoned",		CChar_SetPoisoned,	2, 0, 0 },
	{ "ExplodeItem",		CChar_ExplodeItem,	1, 0, 0 },
	{ "SetInvisible",		CChar_SetInvisible,	2, 0, 0 },
	{ NULL,					NULL,				0, 0, 0 },
};

static JSFunctionSpec CItem_Methods[] =  
{ 	
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
	{ NULL,					NULL,						0, 0, 0 },
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
	{ NULL,					NULL,				0, 0, 0 },
};

static JSFunctionSpec CGuild_Methods[] =  
{ 	
	{ "AcceptRecruit",		CGuild_AcceptRecruit,	1, 0, 0 },
	{ NULL,					NULL,					0, 0, 0 },
};

static JSFunctionSpec CRace_Methods[] =  
{ 	
	{ NULL,					NULL,				0, 0, 0 },
};

#endif


