//o------------------------------------------------------------------------------------------------o
//|	File		-	UOXJSMethods.cpp
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Version History
//|					1.0		Dark-Storm	20th December, 2001 Initial implementation
//|							Methods for the javascript objects
//|
//|							List of supported Classes:
//|								- Gumps
//|								- Char
//|								- Item
//|								- Socket
//o------------------------------------------------------------------------------------------------o
#ifndef __UOXJSMethods__
#define __UOXJSMethods__


typedef JSBool ( JSMethodFunc )( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Object Constructors
JSMethodFunc Gump;
JSMethodFunc GumpData;
JSMethodFunc UOXCFile;
JSMethodFunc Packet;

// Packet Methods
JSMethodFunc CPacket_Free;
JSMethodFunc CPacket_WriteByte;
JSMethodFunc CPacket_WriteShort;
JSMethodFunc CPacket_WriteLong;
JSMethodFunc CPacket_WriteString;
JSMethodFunc CPacket_ReserveSize;

// Gump Methods
JSMethodFunc CGump_Free;
JSMethodFunc CGump_AddBackground;
JSMethodFunc CGump_AddButton;
JSMethodFunc CGump_AddButtonTileArt;
JSMethodFunc CGump_AddPageButton;
JSMethodFunc CGump_AddCheckbox;
JSMethodFunc CGump_AddCheckerTrans;
JSMethodFunc CGump_AddCroppedText;
JSMethodFunc CGump_AddGroup;
JSMethodFunc CGump_EndGroup;
JSMethodFunc CGump_AddGump;
JSMethodFunc CGump_AddGumpColor;
JSMethodFunc CGump_AddHTMLGump;
JSMethodFunc CGump_AddPage;
JSMethodFunc CGump_AddPicture;
JSMethodFunc CGump_AddPictureColor;
JSMethodFunc CGump_AddPicInPic;
JSMethodFunc CGump_AddItemProperty;
JSMethodFunc CGump_AddRadio;
JSMethodFunc CGump_AddText;
JSMethodFunc CGump_AddTextEntry;
JSMethodFunc CGump_AddTextEntryLimited;
JSMethodFunc CGump_AddTiledGump;
JSMethodFunc CGump_AddToolTip;
JSMethodFunc CGump_AddXMFHTMLGump;
JSMethodFunc CGump_AddXMFHTMLGumpColor;
JSMethodFunc CGump_AddXMFHTMLTok;
JSMethodFunc CGump_MasterGump;
JSMethodFunc CGump_NoClose;
JSMethodFunc CGump_NoDispose;
JSMethodFunc CGump_NoMove;
JSMethodFunc CGump_NoResize;
JSMethodFunc CGump_Send;

// GumpData Methods
JSMethodFunc CGumpData_Free;
JSMethodFunc CGumpData_GetId;
JSMethodFunc CGumpData_GetEdit;
JSMethodFunc CGumpData_GetButton;

// Character Methods
JSMethodFunc CChar_DoAction;
JSMethodFunc CChar_EmoteMessage;
JSMethodFunc CChar_OpenBank;
JSMethodFunc CChar_DirectionTo;
JSMethodFunc CChar_TurnToward;
JSMethodFunc CChar_ResourceCount;
JSMethodFunc CChar_CheckSkill;
JSMethodFunc CChar_FindItemLayer;
JSMethodFunc CChar_SpeechInput;
JSMethodFunc CChar_CastSpell;
JSMethodFunc CChar_MagicEffect;
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
JSMethodFunc CChar_Dupe;
JSMethodFunc CChar_Jail;
JSMethodFunc CChar_Release;
JSMethodFunc CChar_SpellMoveEffect;
JSMethodFunc CChar_SpellStaticEffect;
JSMethodFunc CChar_BreakConcentration;
JSMethodFunc CChar_FindItemType;
JSMethodFunc CChar_FindItemSection;
JSMethodFunc CChar_InitWanderArea;
JSMethodFunc CChar_ReactOnDamage;
JSMethodFunc CChar_Damage;
JSMethodFunc CChar_InitiateCombat;
JSMethodFunc CChar_InvalidateAttacker;
JSMethodFunc CChar_AddAggressorFlag;
JSMethodFunc CChar_RemoveAggressorFlag;
JSMethodFunc CChar_CheckAggressorFlag;
JSMethodFunc CChar_UpdateAggressorFlagTimestamp;
JSMethodFunc CChar_IsAggressor;
JSMethodFunc CChar_ClearAggressorFlags;
JSMethodFunc CChar_AddPermaGreyFlag;
JSMethodFunc CChar_RemovePermaGreyFlag;
JSMethodFunc CChar_CheckPermaGreyFlag;
JSMethodFunc CChar_UpdatePermaGreyFlagTimestamp;
JSMethodFunc CChar_IsPermaGrey;
JSMethodFunc CChar_ClearPermaGreyFlags;
JSMethodFunc CChar_Heal;
JSMethodFunc CChar_Defense;
JSMethodFunc CChar_AddFriend;
JSMethodFunc CChar_RemoveFriend;
JSMethodFunc CChar_GetFriendList;
JSMethodFunc CChar_ClearFriendList;
JSMethodFunc CChar_GetPetList;
JSMethodFunc CChar_GetFollowerList;
JSMethodFunc CChar_AddFollower;
JSMethodFunc CChar_RemoveFollower;
JSMethodFunc CChar_HasBeenOwner;
JSMethodFunc CChar_CalculateControlChance;

// Item Methods
JSMethodFunc CItem_OpenPlank;
JSMethodFunc CItem_SetCont;
JSMethodFunc CItem_ApplyRank;
JSMethodFunc CItem_IsOnFoodList;
JSMethodFunc CItem_Glow;
JSMethodFunc CItem_UnGlow;
JSMethodFunc CItem_PlaceInPack;
JSMethodFunc CItem_Dupe;
JSMethodFunc CItem_LockDown;
JSMethodFunc CItem_Carve;
JSMethodFunc CItem_GetTileName;
JSMethodFunc CItem_GetMoreVar;
JSMethodFunc CItem_SetMoreVar;


// BaseObject Methods
JSMethodFunc CBase_TextMessage;
JSMethodFunc CBase_Delete;
JSMethodFunc CBase_Teleport;
JSMethodFunc CBase_StaticEffect;
JSMethodFunc CBase_GetTag;
JSMethodFunc CBase_SetTag;
JSMethodFunc CBase_GetTempTag;
JSMethodFunc CBase_SetTempTag;
JSMethodFunc CBase_GetNumTags;
JSMethodFunc CBase_GetTagMap;
JSMethodFunc CBase_GetTempTagMap;
JSMethodFunc CBase_InRange;
JSMethodFunc CBase_StartTimer;
JSMethodFunc CBase_KillTimers;
JSMethodFunc CBase_GetJSTimer;
JSMethodFunc CBase_SetJSTimer;
JSMethodFunc CBase_KillJSTimer;
JSMethodFunc CBase_ApplySection;
JSMethodFunc CBase_FirstItem;
JSMethodFunc CBase_NextItem;
JSMethodFunc CBase_FinishedItems;
JSMethodFunc CBase_DistanceTo;
JSMethodFunc CBase_GetSerial;
JSMethodFunc CBase_UpdateStats;
JSMethodFunc CBase_Resist;
JSMethodFunc CBase_IsBoat;
JSMethodFunc CBase_CanSee;
JSMethodFunc CBase_UseResource;
JSMethodFunc CBase_AddScriptTrigger;
JSMethodFunc CBase_HasScriptTrigger;
JSMethodFunc CBase_RemoveScriptTrigger;
JSMethodFunc CBase_Refresh;
JSMethodFunc CBase_SetRandomName;
JSMethodFunc CBase_SetRandomColor;

// Multi Methods
JSMethodFunc CMulti_GetMultiCorner;
JSMethodFunc CMulti_SecureContainer;
JSMethodFunc CMulti_UnsecureContainer;
JSMethodFunc CMulti_IsSecureContainer;
JSMethodFunc CMulti_LockDownItem;
JSMethodFunc CMulti_ReleaseItem;
JSMethodFunc CMulti_AddTrashCont;
JSMethodFunc CMulti_RemoveTrashCont;
JSMethodFunc CMulti_KillKeys;
JSMethodFunc CItem_IsMulti;
JSMethodFunc CMulti_IsInMulti;
JSMethodFunc CMulti_IsOnBanList;
JSMethodFunc CMulti_IsOnFriendList;
JSMethodFunc CMulti_IsOnGuestList;
JSMethodFunc CMulti_IsOnOwnerList;
JSMethodFunc CMulti_IsOwner;
JSMethodFunc CMulti_AddToBanList;
JSMethodFunc CMulti_AddToFriendList;
JSMethodFunc CMulti_AddToGuestList;
JSMethodFunc CMulti_AddToOwnerList;
JSMethodFunc CMulti_RemoveFromBanList;
JSMethodFunc CMulti_RemoveFromFriendList;
JSMethodFunc CMulti_RemoveFromGuestList;
JSMethodFunc CMulti_RemoveFromOwnerList;
JSMethodFunc CMulti_ClearBanList;
JSMethodFunc CMulti_ClearFriendList;
JSMethodFunc CMulti_ClearGuestList;
JSMethodFunc CMulti_ClearOwnerList;
JSMethodFunc CMulti_FirstChar;
JSMethodFunc CMulti_NextChar;
JSMethodFunc CMulti_FinishedChars;

// Socket Methods
JSMethodFunc CSocket_Disconnect;
JSMethodFunc CSocket_OpenURL;
JSMethodFunc CSocket_GetByte;
JSMethodFunc CSocket_GetWord;
JSMethodFunc CSocket_GetDWord;
JSMethodFunc CSocket_GetSByte;
JSMethodFunc CSocket_GetSWord;
JSMethodFunc CSocket_GetSDWord;
JSMethodFunc CSocket_GetString;
JSMethodFunc CSocket_SetByte;
JSMethodFunc CSocket_SetWord;
JSMethodFunc CSocket_SetDWord;
JSMethodFunc CSocket_SetString;
JSMethodFunc CSocket_ReadBytes;
JSMethodFunc CSocket_OpenContainer;
JSMethodFunc CSocket_OpenGump;
JSMethodFunc CSocket_CloseGump;
JSMethodFunc CSocket_WhoList;
JSMethodFunc CSocket_Music;
JSMethodFunc CSocket_SendAddMenu;
JSMethodFunc CSocket_Send;
JSMethodFunc CSocket_DisplayDamage;
JSMethodFunc CSocket_Page;

// Guild Methods
JSMethodFunc CGuild_AcceptRecruit;
JSMethodFunc CGuild_IsAtPeace;

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
JSMethodFunc CMisc_MakeMenu;
JSMethodFunc CMisc_SysMessage;

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
JSMethodFunc CFile_EOF;
JSMethodFunc CFile_Length;
JSMethodFunc CFile_Pos;

// Account methods
JSMethodFunc CAccount_AddAccount;
JSMethodFunc CAccount_DelAccount;

JSMethodFunc CConsole_Print;
JSMethodFunc CConsole_Log;
JSMethodFunc CConsole_Error;
JSMethodFunc CConsole_Warning;
JSMethodFunc CConsole_PrintSectionBegin;
JSMethodFunc CConsole_TurnYellow;
JSMethodFunc CConsole_TurnRed;
JSMethodFunc CConsole_TurnGreen;
JSMethodFunc CConsole_TurnBlue;
JSMethodFunc CConsole_TurnNormal;
JSMethodFunc CConsole_TurnBrightWhite;
JSMethodFunc CConsole_PrintDone;
JSMethodFunc CConsole_PrintFailed;
JSMethodFunc CConsole_PrintPassed;
JSMethodFunc CConsole_ClearScreen;
JSMethodFunc CConsole_PrintBasedOnVal;
JSMethodFunc CConsole_MoveTo;
JSMethodFunc CConsole_PrintSpecial;
JSMethodFunc CConsole_BeginShutdown;
JSMethodFunc CConsole_Reload;

JSMethodFunc CParty_GetMember;
JSMethodFunc CParty_Add;
JSMethodFunc CParty_Remove;

JSMethodFunc CSocket_FirstTriggerWord;
JSMethodFunc CSocket_NextTriggerWord;
JSMethodFunc CSocket_FinishedTriggerWords;

JSMethodFunc CRegion_AddScriptTrigger;
JSMethodFunc CRegion_RemoveScriptTrigger;
JSMethodFunc CRegion_GetOrePref;
JSMethodFunc CRegion_GetOreChance;

inline JSFunctionSpec CGump_Methods[] =
{
	{ "Free",						CGump_Free,							0, 0, 0 },
	{ "AddBackground",				CGump_AddBackground,				5, 0, 0 },
	{ "AddButton",					CGump_AddButton,					6, 0, 0 },
	{ "AddButtonTileArt",			CGump_AddButtonTileArt,				11, 0, 0 },
	{ "AddPageButton",				CGump_AddPageButton,				4, 0, 0 },
	{ "AddCheckbox",				CGump_AddCheckbox,					6, 0, 0 },
	{ "AddCheckerTrans",			CGump_AddCheckerTrans,				4, 0, 0 },
	{ "AddCroppedText",				CGump_AddCroppedText,				6, 0, 0 },
	{ "AddGroup",					CGump_AddGroup,						1, 0, 0 },
	{ "EndGroup",					CGump_EndGroup,						0, 0, 0 },
	{ "AddGump",					CGump_AddGump,						4, 0, 0 },
	{ "AddGumpColor",				CGump_AddGumpColor,					4, 0, 0 },
	{ "AddHTMLGump",				CGump_AddHTMLGump,					7, 0, 0 },
	{ "AddPage",					CGump_AddPage,						1, 0, 0 },
	{ "AddPicture",					CGump_AddPicture,					3, 0, 0 },
	{ "AddPictureColor",			CGump_AddPictureColor,				4, 0, 0 },
	{ "AddPicInPic",				CGump_AddPicInPic,					7, 0, 0 },
	{ "AddItemProperty",			CGump_AddItemProperty,				1, 0, 0 },
	{ "AddRadio",					CGump_AddRadio,						6, 0, 0 },
	{ "AddText",					CGump_AddText,						4, 0, 0 },
	{ "AddTextEntry",				CGump_AddTextEntry,					8, 0, 0 },
	{ "AddTextEntryLimited",		CGump_AddTextEntryLimited,			9, 0, 0 },
	{ "AddTiledGump",				CGump_AddTiledGump,					5, 0, 0 },
	{ "AddToolTip",					CGump_AddToolTip,					11, 0, 0 },
	{ "AddXMFHTMLGump",				CGump_AddXMFHTMLGump,				7, 0, 0 },
	{ "AddXMFHTMLGumpColor",		CGump_AddXMFHTMLGumpColor,			8, 0, 0 },
	{ "AddXMFHTMLTok",				CGump_AddXMFHTMLTok,				8, 0, 0 },
	{ "MasterGump",					CGump_MasterGump,					1, 0, 0 },
	{ "NoClose",					CGump_NoClose,						0, 0, 0 },
	{ "NoDispose",					CGump_NoDispose,					0, 0, 0 },
	{ "NoMove",						CGump_NoMove,						0, 0, 0 },
	{ "NoResize",					CGump_NoResize,						0, 0, 0 },
	{ "Send",						CGump_Send,							1, 0, 0 },
	{ nullptr,						nullptr,							0, 0, 0 }
};

inline JSFunctionSpec CChar_Methods[] =
{
	{ "KillTimers",			CBase_KillTimers,		1, 0, 0 },
	{ "GetJSTimer",			CBase_GetJSTimer,		2, 0, 0 },
	{ "SetJSTimer",			CBase_SetJSTimer,		3, 0, 0 },
	{ "KillJSTimer",		CBase_KillJSTimer,		2, 0, 0 },
	{ "TextMessage",		CBase_TextMessage,		1, 0, 0 },
	{ "YellMessage",		CChar_YellMessage,		1, 0, 0 },
	{ "WhisperMessage",		CChar_WhisperMessage,	1, 0, 0 },
	{ "EmoteMessage",		CChar_EmoteMessage,		1, 0, 0 },
	{ "Delete",				CBase_Delete,			0, 0, 0 },
	{ "DoAction",			CChar_DoAction,			1, 0, 0 },
	{ "StaticEffect",		CBase_StaticEffect,		3, 0, 0 },
	{ "Teleport",			CBase_Teleport,			5, 0, 0 },
	{ "SetLocation",		CBase_Teleport,			5, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,		2, 0, 0 },
	{ "SellTo",				CMisc_SellTo,			1, 0, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,			1, 0, 0 },
	{ "HasSpell",			CMisc_HasSpell,			1, 0, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,		1, 0, 0 },
	{ "GetTag",				CBase_GetTag,			1, 0, 0 },
	{ "SetTag",				CBase_SetTag,			2, 0, 0 },
	{ "GetTempTag",			CBase_GetTempTag,		1, 0, 0 },
	{ "SetTempTag",			CBase_SetTempTag,		2, 0, 0 },
	{ "GetNumTags",			CBase_GetNumTags,		0, 0, 0 },
	{ "GetTagMap",			CBase_GetTagMap,		0, 0, 0 },
	{ "GetTempTagMap",		CBase_GetTempTagMap,	0, 0, 0 },
	{ "OpenBank",			CChar_OpenBank,			1, 0, 0 },
	{ "DirectionTo",		CChar_DirectionTo,		1, 0, 0 },
	{ "TurnToward",			CChar_TurnToward,		1, 0, 0 },
	{ "ResourceCount",		CChar_ResourceCount,	2, 0, 0 },
	{ "UseResource",		CBase_UseResource,		3, 0, 0 },
	{ "CustomTarget",		CMisc_CustomTarget,		1, 0, 0 },
	{ "PopUpTarget",		CMisc_PopUpTarget,		1, 0, 0 },
	{ "InRange",			CBase_InRange,			2, 0, 0 },
	{ "FindItemLayer",		CChar_FindItemLayer,	1, 0, 0 },
	{ "StartTimer",			CBase_StartTimer,		2, 0, 0 },
	{ "CheckSkill",			CChar_CheckSkill,		4, 0, 0 },
	{ "SpeechInput",		CChar_SpeechInput,		1, 0, 0 },
	{ "CastSpell",			CChar_CastSpell,		2, 0, 0 },
	{ "SysMessage",			CMisc_SysMessage,		10, 0, 0 },
	{ "MagicEffect",		CChar_MagicEffect,		2, 0, 0 },
	{ "GetSerial",			CBase_GetSerial,		1, 0, 0 },
	{ "UpdateStats",		CBase_UpdateStats,		1, 0, 0 },
	{ "MakeMenu",			CMisc_MakeMenu,			2, 0, 0 },
	{ "Wander",				CChar_Wander,			3, 0, 0 },
	{ "Follow",				CChar_Follow,			1, 0, 0 },
	{ "Dismount",			CChar_Dismount,			0, 0, 0 },
	{ "SetPoisoned",		CChar_SetPoisoned,		2, 0, 0 },
	{ "ExplodeItem",		CChar_ExplodeItem,		3, 0, 0 },
	{ "SetInvisible",		CChar_SetInvisible,		2, 0, 0 },
	{ "ApplySection",		CBase_ApplySection,		1, 0, 0 },
	{ "Refresh",			CBase_Refresh,			0, 0, 0 },
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
	{ "SetRandomName",		CBase_SetRandomName,	1, 0, 0 },
	{ "SetRandomColor",		CBase_SetRandomColor,	1, 0, 0 },
	{ "SetSkillByName",		CChar_SetSkillByName,	2, 0, 0 },
	{ "Kill",				CChar_Kill,				0, 0, 0 },
	{ "Resurrect",			CChar_Resurrect,		0, 0, 0 },
	{ "Dupe",				CChar_Dupe,				0, 0, 0 },
	{ "Jail",				CChar_Jail,				0, 0, 0 },
	{ "Release",			CChar_Release,			0, 0, 0 },
	{ "GetTimer",			CMisc_GetTimer,			1, 0, 0 },
	{ "SetTimer",			CMisc_SetTimer,			2, 0, 0 },
	{ "SpellMoveEffect",	CChar_SpellMoveEffect,	2, 0, 0 },
	{ "SpellStaticEffect",	CChar_SpellStaticEffect,1, 0, 0 },
	{ "BreakConcentration",	CChar_BreakConcentration,0, 0, 0 },
	{ "FindItemType",		CChar_FindItemType,		1, 0, 0 },
	{ "FindItemSection",	CChar_FindItemSection,	1, 0, 0 },
	{ "InitWanderArea",		CChar_InitWanderArea,	0, 0, 0 },
	{ "CanSee",				CBase_CanSee,			1, 0, 0 },
	{ "ReactOnDamage",		CChar_ReactOnDamage,	1, 0, 0 },
	{ "Damage",				CChar_Damage,			1, 0, 0 },
	{ "InitiateCombat",		CChar_InitiateCombat,	1, 0, 0 },
	{ "InvalidateAttacker",		CChar_InvalidateAttacker,				0, 0, 0 },
	{ "AddAggressorFlag",		CChar_AddAggressorFlag,					1, 0, 0 },
	{ "RemoveAggressorFlag",	CChar_RemoveAggressorFlag,				1, 0, 0 },
	{ "CheckAggressorFlag",		CChar_CheckAggressorFlag,					1, 0, 0 },
	{ "UpdateAggressorFlagTimestamp",	CChar_UpdateAggressorFlagTimestamp,	1, 0, 0 },
	{ "IsAggressor",			CChar_IsAggressor,						1, 0, 0 },
	{ "ClearAggressorFlags",	CChar_ClearAggressorFlags,				0, 0, 0 },
	{ "AddPermaGreyFlag",		CChar_AddPermaGreyFlag,					1, 0, 0 },
	{ "RemovePermaGreyFlag",	CChar_RemovePermaGreyFlag,				1, 0, 0 },
	{ "CheckPermaGreyFlag",		CChar_CheckPermaGreyFlag,					1, 0, 0 },
	{ "UpdatePermaGreyFlagTimestamp",	CChar_UpdatePermaGreyFlagTimestamp,	1, 0, 0 },
	{ "IsPermaGrey",			CChar_IsPermaGrey,						1, 0, 0 },
	{ "ClearPermaGreyFlags",	CChar_ClearPermaGreyFlags,				0, 0, 0 },
	{ "Heal",				CChar_Heal,				1, 0, 0 },
	{ "Resist",				CBase_Resist,			1, 0, 0 },
	{ "Defense",			CChar_Defense,			3, 0, 0 },
	{ "AddScriptTrigger",	CBase_AddScriptTrigger,		1, 0, 0 },
	{ "HasScriptTrigger",	CBase_HasScriptTrigger,		1, 0, 0 },
	{ "RemoveScriptTrigger",CBase_RemoveScriptTrigger,	1, 0, 0 },
	{ "AddFriend",			CChar_AddFriend,			1, 0, 0 },
	{ "RemoveFriend",		CChar_RemoveFriend,			1, 0, 0 },
	{ "GetFriendList",		CChar_GetFriendList,		0, 0, 0 },
	{ "ClearFriendList",	CChar_ClearFriendList,		0, 0, 0 },
	{ "GetPetList",			CChar_GetPetList,			0, 0, 0 },
	{ "GetFollowerList",	CChar_GetFollowerList,		0, 0, 0 },
	{ "AddFollower",		CChar_AddFollower,			1, 0, 0 },
	{ "RemoveFollower",		CChar_RemoveFollower,		1, 0, 0 },
	{ "HasBeenOwner",		CChar_HasBeenOwner,			1, 0, 0 },
	{ "CalculateControlChance",	CChar_CalculateControlChance,	1, 0, 0 },
	{ nullptr,				nullptr,				0, 0, 0 }
};

inline JSFunctionSpec CItem_Methods[] =
{
	{ "KillTimers",			CBase_KillTimers,			1, 0, 0 },
	{ "GetJSTimer",			CBase_GetJSTimer,			2, 0, 0 },
	{ "SetJSTimer",			CBase_SetJSTimer,			3, 0, 0 },
	{ "KillJSTimer",		CBase_KillJSTimer,			2, 0, 0 },
	{ "Delete",				CBase_Delete,				0, 0, 0 },
	{ "TextMessage",		CBase_TextMessage,			1, 0, 0 },
	{ "Teleport",			CBase_Teleport,				5, 0, 0 },
	{ "StaticEffect",		CBase_StaticEffect,			4, 0, 0 },
	{ "SetLocation",		CBase_Teleport,				5, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,			2, 0, 0 },
	{ "HasSpell",			CMisc_HasSpell,				1, 0, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,			1, 0, 0 },
	{ "GetTag",				CBase_GetTag,				1, 0, 0 },
	{ "SetTag",				CBase_SetTag,				1, 0, 0 },
	{ "GetTempTag",			CBase_GetTempTag,			1, 0, 0 },
	{ "SetTempTag",			CBase_SetTempTag,			2, 0, 0 },
	{ "GetNumTags",			CBase_GetNumTags,			0, 0, 0 },
	{ "GetTagMap",			CBase_GetTagMap,			0, 0, 0 },
	{ "GetTempTagMap",		CBase_GetTempTagMap,		0, 0, 0 },
	{ "InRange",			CBase_InRange,				2, 0, 0 },
	{ "StartTimer",			CBase_StartTimer,			2, 0, 0 },
	{ "OpenPlank",			CItem_OpenPlank,			0, 0, 0 },
	{ "IsMulti",			CItem_IsMulti,				0, 0, 0 },
	{ "IsBoat",				CBase_IsBoat,				0, 0, 0 },
	{ "IsInMulti",			CMulti_IsInMulti,			1, 0, 0 },
	{ "IsOnBanList",		CMulti_IsOnBanList,			1, 0, 0 },
	{ "IsOnFriendList",		CMulti_IsOnFriendList,		1, 0, 0 },
	{ "IsOnGuestList",		CMulti_IsOnGuestList,		1, 0, 0 },
	{ "IsOwner",			CMulti_IsOwner,				1, 0, 0 },
	{ "IsOnOwnerList",		CMulti_IsOnOwnerList,		1, 0, 0 },
	{ "AddToBanList",		CMulti_AddToBanList,		1, 0, 0 },
	{ "AddToFriendList",	CMulti_AddToFriendList,		1, 0, 0 },
	{ "AddToGuestList",		CMulti_AddToGuestList,		1, 0, 0 },
	{ "AddToOwnerList",		CMulti_AddToOwnerList,		1, 0, 0 },
	{ "RemoveFromBanList",		CMulti_RemoveFromBanList,		1, 0, 0 },
	{ "RemoveFromFriendList",	CMulti_RemoveFromFriendList,	1, 0, 0 },
	{ "RemoveFromGuestList",	CMulti_RemoveFromGuestList,		1, 0, 0 },
	{ "RemoveFromOwnerList",	CMulti_RemoveFromOwnerList,		1, 0, 0 },
	{ "ClearBanList",		CMulti_ClearBanList,		1, 0, 0 },
	{ "ClearFriendList",	CMulti_ClearFriendList,		1, 0, 0 },
	{ "ClearGuestList",		CMulti_ClearGuestList,		1, 0, 0 },
	{ "ClearOwnerList",		CMulti_ClearOwnerList,		1, 0, 0 },
	{ "GetSerial",			CBase_GetSerial,			1, 0, 0 },
	{ "UpdateStats",		CBase_UpdateStats,			1, 0, 0 },
	{ "SetCont",			CItem_SetCont,				1, 0, 0 },
	{ "ApplySection",		CBase_ApplySection,			1, 0, 0 },
	{ "Refresh",			CBase_Refresh,				0, 0, 0 },
	{ "ApplyRank",			CItem_ApplyRank,			2, 0, 0 },
	{ "IsOnFoodList",		CItem_IsOnFoodList,			1, 0, 0 },
	{ "FirstItem",			CBase_FirstItem,			0, 0, 0 },
	{ "NextItem",			CBase_NextItem,				0, 0, 0 },
	{ "FinishedItems",		CBase_FinishedItems,		0, 0, 0 },
	{ "DistanceTo",			CBase_DistanceTo,			1, 0, 0 },
	{ "Glow",				CItem_Glow,					1, 0, 0 },
	{ "UnGlow",				CItem_UnGlow,				1, 0, 0 },
	{ "PlaceInPack",		CItem_PlaceInPack,			0, 0, 0 },
	{ "Dupe",				CItem_Dupe,					1, 0, 0 },
	{ "LockDown",			CItem_LockDown,				0, 0, 0 },
	{ "Carve",				CItem_Carve,				1, 0, 0 },
	{ "GetTileName",		CItem_GetTileName,			0, 0, 0 },
	{ "GetMoreVar",			CItem_GetMoreVar,			2, 0, 0 },
	{ "SetMoreVar",			CItem_SetMoreVar,			3, 0, 0 },
	{ "Resist",				CBase_Resist,				1, 0, 0 },
	{ "UseResource",		CBase_UseResource,			3, 0, 0 },
	{ "AddScriptTrigger",	CBase_AddScriptTrigger,		1, 0, 0 },
	{ "HasScriptTrigger",	CBase_HasScriptTrigger,		1, 0, 0 },
	{ "RemoveScriptTrigger",CBase_RemoveScriptTrigger,	1, 0, 0 },
	{ "GetMultiCorner",		CMulti_GetMultiCorner,		1, 0, 0 },
	{ "SecureContainer",	CMulti_SecureContainer,		1, 0, 0 },
	{ "UnsecureContainer",	CMulti_UnsecureContainer,	1, 0, 0 },
	{ "IsSecureContainer",	CMulti_IsSecureContainer,	1, 0, 0 },
	{ "LockDownItem",		CMulti_LockDownItem,		1, 0, 0 },
	{ "ReleaseItem",		CMulti_ReleaseItem,			1, 0, 0 },
	{ "AddTrashCont",		CMulti_AddTrashCont,		1, 0, 0 },
	{ "RemoveTrashCont",	CMulti_RemoveTrashCont,		1, 0, 0 },

	{ "KillKeys",			CMulti_KillKeys,			1, 0, 0 },

	{ "FirstChar",			CMulti_FirstChar,			1, 0, 0 },
	{ "NextChar",			CMulti_NextChar,			1, 0, 0 },
	{ "FinishedChars",		CMulti_FinishedChars,		1, 0, 0 },

	//{ "SetMoreSerial",		CBase_SetMoreSerial,		1, 0, 0 },
	{ "SetRandomName",		CBase_SetRandomName,		1, 0, 0 },
	{ "SetRandomColor",		CBase_SetRandomColor,		1, 0, 0 },
	{ nullptr,				nullptr,					0, 0, 0 }
};

inline JSFunctionSpec CRegion_Methods[] =
{
	{ "AddScriptTrigger",		CRegion_AddScriptTrigger,		1, 0, 0 },
	{ "RemoveScriptTrigger",	CRegion_RemoveScriptTrigger,	1, 0, 0 },
	{ "GetOrePref",				CRegion_GetOrePref,				1, 0, 0 },
	{ "GetOreChance",			CRegion_GetOreChance,			0, 0, 0 },
	{ nullptr,					nullptr,						0, 0, 0 }
};

inline JSFunctionSpec CSocket_Methods[] =
{
	{ "SysMessage",			CMisc_SysMessage,	10, 0, 0 },
	{ "Disconnect",			CSocket_Disconnect,	0, 0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,	2, 0, 0 },
	{ "CustomTarget",		CMisc_CustomTarget, 1, 0, 0 },
	{ "PopUpTarget",		CMisc_PopUpTarget,	1, 0, 0 },
	{ "GetByte",			CSocket_GetByte,	1, 0, 0 },
	{ "GetWord",			CSocket_GetWord,	1, 0, 0 },
	{ "GetDWord",			CSocket_GetDWord,	1, 0, 0 },
	{ "GetSByte",			CSocket_GetSByte,	1, 0, 0 },
	{ "GetSWord",			CSocket_GetSWord,	1, 0, 0 },
	{ "GetSDWord",			CSocket_GetSDWord,	1, 0, 0 },
	{ "GetString",			CSocket_GetString,	1, 0, 0 },
	{ "SetByte",			CSocket_SetByte,	2, 0, 0 },
	{ "SetWord",			CSocket_SetWord,	2, 0, 0 },
	{ "SetDWord",			CSocket_SetDWord,	2, 0, 0 },
	{ "SetString",			CSocket_SetString,	2, 0, 0 },
	{ "ReadBytes",			CSocket_ReadBytes,	1, 0, 0 },
	{ "OpenContainer",		CSocket_OpenContainer,	1, 0, 0 },
	{ "OpenGump",			CSocket_OpenGump,	1, 0, 0 },
	{ "CloseGump",			CSocket_CloseGump,	2, 0, 0 },
	{ "OpenURL",			CSocket_OpenURL,    1, 0, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,		1, 0, 0 },
	{ "SellTo",				CMisc_SellTo,		1, 0, 0 },
	{ "WhoList",			CSocket_WhoList,	0, 0, 0 },
	{ "Music",				CSocket_Music,		1, 0, 0 },
	{ "GetTimer",			CMisc_GetTimer,		1, 0, 0 },
	{ "SetTimer",			CMisc_SetTimer,		2, 0, 0 },
	{ "SendAddMenu",		CSocket_SendAddMenu,1, 0, 0 },
	{ "Page",				CSocket_Page,		1, 0, 0 },
	{ "MakeMenu",			CMisc_MakeMenu,		2, 0, 0 },
	{ "Send",				CSocket_Send,		1, 0, 0 },
	{ "CanSee",				CBase_CanSee,		1, 0, 0 },
	{ "DisplayDamage",		CSocket_DisplayDamage, 2, 0, 0 },
	{ "FirstTriggerWord",	CSocket_FirstTriggerWord, 0, 0, 0 },
	{ "NextTriggerWord",	CSocket_NextTriggerWord, 0, 0, 0 },
	{ "FinishedTriggerWords",	CSocket_FinishedTriggerWords, 0, 0, 0 },
	{ nullptr,				nullptr,			0, 0, 0 }
};

inline JSFunctionSpec CGuild_Methods[] =
{
	{ "AcceptRecruit",		CGuild_AcceptRecruit,		1, 0, 0 },
	{ "IsAtPeace",			CGuild_IsAtPeace,			0, 0, 0 },
	{ nullptr,				nullptr,					0, 0, 0 }
};

inline JSFunctionSpec CRace_Methods[] =
{
	{ "CanWearArmour",		CRace_CanWearArmour,		1, 0, 0 },
	{ "IsValidHairColour",	CRace_IsValidHairColour,	1, 0, 0 },
	{ "IsValidSkinColour",	CRace_IsValidSkinColour,	1, 0, 0 },
	{ "IsValidBeardColour",	CRace_IsValidBeardColour,	1, 0, 0 },
	{ nullptr,				nullptr,					0, 0, 0 }
};

inline JSFunctionSpec CGumpData_Methods[] =
{
	{ "Free",		CGumpData_Free,			0, 0, 0 },
	{ "getButton",	CGumpData_GetButton,	1, 0, 0 },
	{ "getID",		CGumpData_GetId,		1, 0, 0 },
	{ "getEdit",	CGumpData_GetEdit,		1, 0, 0 },
	{ nullptr,		nullptr,				0, 0, 0 }
};

inline JSFunctionSpec CFile_Methods[] =
{
	{ "Free",		CFile_Free,				0, 0, 0 },
	{ "Open",		CFile_Open,				2, 0, 0 },
	{ "Close",		CFile_Close,			0, 0, 0 },
	{ "Write",		CFile_Write,			1, 0, 0 },
	{ "Read",		CFile_Read,				1, 0, 0 },
	{ "ReadUntil",	CFile_ReadUntil,		1, 0, 0 },
	{ "EOF",		CFile_EOF,				0, 0, 0 },
	{ "Length",		CFile_Length,			0, 0, 0 },
	{ "Pos",		CFile_Pos,				1, 0, 0 },
	{ nullptr,		nullptr,				0, 0, 0 }
};

inline JSFunctionSpec CAccount_Methods[] =
{
	{ "AddAccount",		CAccount_AddAccount,		4, 0, 0 },
	{ "DelAccount",		CAccount_DelAccount,		1, 0, 0 },
	{ nullptr,		nullptr,				0, 0, 0 }
};

inline JSFunctionSpec CConsole_Methods[] =
{
	{ "Print",				CConsole_Print,				1, 0, 0 },
	{ "Log",				CConsole_Log,				1, 0, 0 },
	{ "Error",				CConsole_Error,				1, 0, 0 },
	{ "Warning",			CConsole_Warning,			1, 0, 0 },
	{ "PrintSectionBegin",	CConsole_PrintSectionBegin,	0, 0, 0 },
	{ "TurnYellow",			CConsole_TurnYellow,		0, 0, 0 },
	{ "TurnRed",			CConsole_TurnRed,			0, 0, 0 },
	{ "TurnGreen",			CConsole_TurnGreen,			0, 0, 0 },
	{ "TurnBlue",			CConsole_TurnBlue,			0, 0, 0 },
	{ "TurnNormal",			CConsole_TurnNormal,		0, 0, 0 },
	{ "TurnBrightWhite",	CConsole_TurnBrightWhite,	0, 0, 0 },
	{ "PrintDone",			CConsole_PrintDone,			0, 0, 0 },
	{ "PrintFailed",		CConsole_PrintFailed,		0, 0, 0 },
	{ "PrintPassed",		CConsole_PrintPassed,		0, 0, 0 },
	{ "ClearScreen",		CConsole_ClearScreen,		0, 0, 0 },
	{ "PrintBasedOnVal",	CConsole_PrintBasedOnVal,	1, 0, 0 },
	{ "MoveTo",				CConsole_MoveTo,			2, 0, 0 },
	{ "PrintSpecial",		CConsole_PrintSpecial,		2, 0, 0 },
	{ "BeginShutdown",		CConsole_BeginShutdown,		0, 0, 0 },
	{ "Reload",				CConsole_Reload,			1, 0, 0 },
	{ nullptr,				nullptr,								0, 0, 0 }
};

inline JSFunctionSpec CPacket_Methods[] =
{
	{ "Free",			CPacket_Free,				0, 0, 0 },
	{ "WriteByte",		CPacket_WriteByte,			2, 0, 0 },
	{ "WriteShort",		CPacket_WriteShort,			2, 0, 0 },
	{ "WriteLong",		CPacket_WriteLong,			2, 0, 0 },
	{ "WriteString",	CPacket_WriteString,		3, 0, 0 },
	{ "ReserveSize",	CPacket_ReserveSize,		1, 0, 0 },
	{ nullptr,			nullptr,						0, 0, 0 }
};

inline JSFunctionSpec CParty_Methods[] =
{
	{ "GetMember",		CParty_GetMember,			1, 0, 0 },
	{ "Add",			CParty_Add,					1, 0, 0 },
	{ "Remove",			CParty_Remove,				1, 0, 0 },
	{ nullptr,			nullptr,						0, 0, 0 }
};

#endif
