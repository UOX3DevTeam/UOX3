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

	typedef JSBool (JSMethodFunc)( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Object Constructors
JSMethodFunc Gump;
JSMethodFunc GumpData;
JSMethodFunc UOXCFile;

// Gump Methods
JSMethodFunc CGump_Free;
JSMethodFunc CGump_AddBackground;
JSMethodFunc CGump_AddButton;
JSMethodFunc CGump_AddPageButton;
JSMethodFunc CGump_AddCheckbox;
JSMethodFunc CGump_AddCheckerTrans;
JSMethodFunc CGump_AddCroppedText;
JSMethodFunc CGump_AddGroup;
JSMethodFunc CGump_AddGump;
JSMethodFunc CGump_AddHTMLGump;
JSMethodFunc CGump_AddPage;
JSMethodFunc CGump_AddPicture;
JSMethodFunc CGump_AddRadio;
JSMethodFunc CGump_AddText;
JSMethodFunc CGump_AddTextEntry;
JSMethodFunc CGump_AddTiledGump;
JSMethodFunc CGump_AddXMFHTMLGump;
JSMethodFunc CGump_AddXMFHTMLGumpColor;
JSMethodFunc CGump_NoClose;
JSMethodFunc CGump_NoDispose;
JSMethodFunc CGump_NoMove;
JSMethodFunc CGump_NoResize;
JSMethodFunc CGump_Send;

// GumpData Methods
JSMethodFunc CGumpData_Free;
JSMethodFunc CGumpData_getID;
JSMethodFunc CGumpData_getEdit;
JSMethodFunc CGumpData_getButton;

// Character Methods
JSMethodFunc CChar_Action;
JSMethodFunc CChar_EmoteMessage;
JSMethodFunc CChar_Freeze;
JSMethodFunc CChar_Unfreeze;
JSMethodFunc CChar_OpenBank;
JSMethodFunc CChar_DirectionTo;
JSMethodFunc CChar_TurnToward;
JSMethodFunc CChar_UseResource;
JSMethodFunc CChar_ResourceCount;
JSMethodFunc CChar_CheckSkill;
JSMethodFunc CChar_FindItemLayer;
JSMethodFunc CChar_SpeechInput;
JSMethodFunc CChar_CastSpell;
JSMethodFunc CChar_SysMessage;
JSMethodFunc CChar_MagicEffect;
JSMethodFunc CChar_MakeMenu;
JSMethodFunc CChar_Wander;
JSMethodFunc CChar_Follow;
JSMethodFunc CChar_Dismount;
JSMethodFunc CChar_SetPoisoned;
JSMethodFunc CChar_ExplodeItem;
JSMethodFunc CChar_SetInvisible;
JSMethodFunc CChar_YellMessage;
JSMethodFunc CChar_WhisperMessage;
JSMethodFunc CChar_AddSpell;
JSMethodFunc CChar_SpellFail;
JSMethodFunc CChar_ExecuteCommand;
JSMethodFunc CChar_WalkTo;
JSMethodFunc CChar_RunTo;
JSMethodFunc CChar_OpenLayer;
JSMethodFunc CChar_BoltEffect;
JSMethodFunc CChar_Gate;
JSMethodFunc CChar_Recall;
JSMethodFunc CChar_Mark;
JSMethodFunc CChar_SetSkillByName;
JSMethodFunc CChar_Kill;
JSMethodFunc CChar_Resurrect;
JSMethodFunc CChar_Jail;
JSMethodFunc CChar_Release;

// Item Methods
JSMethodFunc CItem_OpenPlank;
JSMethodFunc CItem_IsMulti;
JSMethodFunc CItem_IsInMulti;
JSMethodFunc CItem_IsOnBanList;
JSMethodFunc CItem_IsOnOwnerList;
JSMethodFunc CItem_AddToBanList;
JSMethodFunc CItem_AddToOwnerList;
JSMethodFunc CItem_RemoveFromBanList;
JSMethodFunc CItem_RemoveFromOwnerList;
JSMethodFunc CItem_SetCont;
JSMethodFunc CItem_SetPoison;
JSMethodFunc CItem_Refresh;
JSMethodFunc CItem_ApplyRank;
JSMethodFunc CItem_Glow;
JSMethodFunc CItem_UnGlow;
JSMethodFunc CItem_PlaceInPack;
JSMethodFunc CItem_Dupe;


// BaseObject Methods
JSMethodFunc CBase_TextMessage;
JSMethodFunc CBase_Delete;
JSMethodFunc CBase_Teleport;
JSMethodFunc CBase_StaticEffect;
JSMethodFunc CBase_GetTag;
JSMethodFunc CBase_SetTag;
JSMethodFunc CBase_GetNumTags;
JSMethodFunc CBase_InRange;
JSMethodFunc CBase_StartTimer;
JSMethodFunc CBase_KillTimers;
JSMethodFunc CBase_ApplySection;
JSMethodFunc CBase_FirstItem;
JSMethodFunc CBase_NextItem;
JSMethodFunc CBase_FinishedItems;
JSMethodFunc CBase_DistanceTo;
JSMethodFunc CBase_GetSerial;

// Socket Methods
JSMethodFunc CSocket_SysMessage;
JSMethodFunc CSocket_Disconnect;
JSMethodFunc CSocket_OpenURL;
JSMethodFunc CSocket_GetByte;
JSMethodFunc CSocket_GetWord;
JSMethodFunc CSocket_GetDWord;
JSMethodFunc CSocket_GetString;
JSMethodFunc CSocket_SetByte;
JSMethodFunc CSocket_SetWord;
JSMethodFunc CSocket_SetDWord;
JSMethodFunc CSocket_SetString;
JSMethodFunc CSocket_ReadBytes;
JSMethodFunc CSocket_OpenGump;
JSMethodFunc CSocket_WhoList;
JSMethodFunc CSocket_Midi;

// Guild Methods
JSMethodFunc CGuild_AcceptRecruit;

// Misc
JSMethodFunc CMisc_SoundEffect;
JSMethodFunc CMisc_SellTo;
JSMethodFunc CMisc_BuyFrom;
JSMethodFunc CMisc_HasSpell;
JSMethodFunc CMisc_RemoveSpell;
JSMethodFunc CMisc_CustomTarget;
JSMethodFunc CMisc_PopUpTarget;
JSMethodFunc CMisc_GetTimer;
JSMethodFunc CMisc_SetTimer;

// Race methods
JSMethodFunc CRace_CanWearArmour;
JSMethodFunc CRace_IsValidHairColour;
JSMethodFunc CRace_IsValidSkinColour;
JSMethodFunc CRace_IsValidBeardColour;

// File methods

JSMethodFunc CFile_Open;
JSMethodFunc CFile_Close;
JSMethodFunc CFile_Free;
JSMethodFunc CFile_Write;
JSMethodFunc CFile_ReadUntil;
JSMethodFunc CFile_Read;

// Account methods
//JSMethodFunc CAccount_GetAccount;
//JSMethodFunc CAccount_SetAccount;
JSMethodFunc CAccount_AddAccount;
JSMethodFunc CAccount_DelAccount;
//JSMethodFunc CAccount_ModAccount;
//JSMethodFunc CAccount_SaveAccounts;


static JSFunctionSpec CGump_Methods[] =  
{ 	
	{ "Free",					CGump_Free,					0, 0, 0 },
	{ "AddBackground",			CGump_AddBackground,		5, 0, 0 },
	{ "AddButton",				CGump_AddButton,			6, 0, 0 },
	{ "AddPageButton",			CGump_AddPageButton,		4, 0, 0 },
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
	{ "GetSerial",			CBase_GetSerial,		1, 0, 0 },
	{ "MakeMenu",			CChar_MakeMenu,			1, 0, 0 },
	{ "Wander",				CChar_Wander,			3, 0, 0 },
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
	{ "SetSkillByName",		CChar_SetSkillByName,	2, 0, 0 },
	{ "Kill",				CChar_Kill,				0, 0, 0 },
	{ "Resurrect",			CChar_Resurrect,		0, 0, 0 },
	{ "Jail",				CChar_Jail,				0, 0, 0 },
	{ "Release",			CChar_Release,			0, 0, 0 },
	{ "GetTimer",			CMisc_GetTimer,			1, 0, 0 },
	{ "SetTimer",			CMisc_SetTimer,			2, 0, 0 },

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
	{ "GetSerial",			CBase_GetSerial,			1, 0, 0 },
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
	{ "PlaceInPack",		CItem_PlaceInPack,			0, 0, 0 },
	{ "Dupe",				CItem_Dupe,					1, 0, 0 },
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
	{ "GetDWord",			CSocket_GetDWord,	1, 0, 0 },
	{ "GetString",			CSocket_GetString,	1, 0, 0 },
	{ "SetByte",			CSocket_SetByte,	2, 0, 0 },
	{ "SetWord",			CSocket_SetWord,	2, 0, 0 },
	{ "SetDWord",			CSocket_SetDWord,	2, 0, 0 },
	{ "SetString",			CSocket_SetString,	2, 0, 0 },
	{ "ReadBytes",			CSocket_ReadBytes,	1, 0, 0 },
	{ "OpenGump",			CSocket_OpenGump,	1, 0, 0 },
	{ "OpenURL",			CSocket_OpenURL,    1, 0, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,		1, 0, 0 },
	{ "SellTo",				CMisc_SellTo,		1, 0, 0 },
	{ "WhoList",			CSocket_WhoList,	0, 0, 0 },
	{ "Midi",				CSocket_Midi,		1, 0, 0 },
	{ "GetTimer",			CMisc_GetTimer,		1, 0, 0 },
	{ "SetTimer",			CMisc_SetTimer,		2, 0, 0 },
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

