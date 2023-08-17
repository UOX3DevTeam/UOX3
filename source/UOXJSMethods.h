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


// Object Constructors
JSNative Gump;
JSNative GumpData;
JSNative UOXCFile;
JSNative Packet;

// Packet Methods
JSNative CPacket_Free;
JSNative CPacket_WriteByte;
JSNative CPacket_WriteShort;
JSNative CPacket_WriteLong;
JSNative CPacket_WriteString;
JSNative CPacket_ReserveSize;

// Gump Methods
JSNative CGump_Free;
JSNative CGump_AddBackground;
JSNative CGump_AddButton;
JSNative CGump_AddButtonTileArt;
JSNative CGump_AddPageButton;
JSNative CGump_AddCheckbox;
JSNative CGump_AddCheckerTrans;
JSNative CGump_AddCroppedText;
JSNative CGump_AddGroup;
JSNative CGump_EndGroup;
JSNative CGump_AddGump;
JSNative CGump_AddGumpColor;
JSNative CGump_AddHTMLGump;
JSNative CGump_AddPage;
JSNative CGump_AddPicture;
JSNative CGump_AddPictureColor;
JSNative CGump_AddPicInPic;
JSNative CGump_AddItemProperty;
JSNative CGump_AddRadio;
JSNative CGump_AddText;
JSNative CGump_AddTextEntry;
JSNative CGump_AddTextEntryLimited;
JSNative CGump_AddTiledGump;
JSNative CGump_AddToolTip;
JSNative CGump_AddXMFHTMLGump;
JSNative CGump_AddXMFHTMLGumpColor;
JSNative CGump_AddXMFHTMLTok;
JSNative CGump_MasterGump;
JSNative CGump_NoClose;
JSNative CGump_NoDispose;
JSNative CGump_NoMove;
JSNative CGump_NoResize;
JSNative CGump_Send;

// GumpData Methods
JSNative CGumpData_Free;
JSNative CGumpData_GetId;
JSNative CGumpData_GetEdit;
JSNative CGumpData_GetButton;

// Character Methods
JSNative CChar_DoAction;
JSNative CChar_EmoteMessage;
JSNative CChar_OpenBank;
JSNative CChar_DirectionTo;
JSNative CChar_TurnToward;
JSNative CChar_ResourceCount;
JSNative CChar_CheckSkill;
JSNative CChar_FindItemLayer;
JSNative CChar_SpeechInput;
JSNative CChar_CastSpell;
JSNative CChar_MagicEffect;
JSNative CChar_Wander;
JSNative CChar_Follow;
JSNative CChar_Dismount;
JSNative CChar_SetPoisoned;
JSNative CChar_ExplodeItem;
JSNative CChar_SetInvisible;
JSNative CChar_YellMessage;
JSNative CChar_WhisperMessage;
JSNative CChar_AddSpell;
JSNative CChar_SpellFail;
JSNative CChar_ExecuteCommand;
JSNative CChar_WalkTo;
JSNative CChar_RunTo;
JSNative CChar_OpenLayer;
JSNative CChar_BoltEffect;
JSNative CChar_Gate;
JSNative CChar_Recall;
JSNative CChar_Mark;
JSNative CChar_SetRandomName;
JSNative CChar_SetSkillByName;
JSNative CChar_Kill;
JSNative CChar_Resurrect;
JSNative CChar_Dupe;
JSNative CChar_Jail;
JSNative CChar_Release;
JSNative CChar_SpellMoveEffect;
JSNative CChar_SpellStaticEffect;
JSNative CChar_BreakConcentration;
JSNative CChar_FindItemType;
JSNative CChar_FindItemSection;
JSNative CChar_InitWanderArea;
JSNative CChar_ReactOnDamage;
JSNative CChar_Damage;
JSNative CChar_InitiateCombat;
JSNative CChar_InvalidateAttacker;
JSNative CChar_AddAggressorFlag;
JSNative CChar_RemoveAggressorFlag;
JSNative CChar_CheckAggressorFlag;
JSNative CChar_UpdateAggressorFlagTimestamp;
JSNative CChar_IsAggressor;
JSNative CChar_ClearAggressorFlags;
JSNative CChar_AddPermaGreyFlag;
JSNative CChar_RemovePermaGreyFlag;
JSNative CChar_CheckPermaGreyFlag;
JSNative CChar_UpdatePermaGreyFlagTimestamp;
JSNative CChar_IsPermaGrey;
JSNative CChar_ClearPermaGreyFlags;
JSNative CChar_Heal;
JSNative CChar_Defense;
JSNative CChar_AddFriend;
JSNative CChar_RemoveFriend;
JSNative CChar_GetFriendList;
JSNative CChar_ClearFriendList;
JSNative CChar_GetPetList;
JSNative CChar_GetFollowerList;
JSNative CChar_AddFollower;
JSNative CChar_RemoveFollower;
JSNative CChar_HasBeenOwner;
JSNative CChar_CalculateControlChance;

// Item Methods
JSNative CItem_OpenPlank;
JSNative CItem_SetCont;
JSNative CItem_ApplyRank;
JSNative CItem_IsOnFoodList;
JSNative CItem_Glow;
JSNative CItem_UnGlow;
JSNative CItem_PlaceInPack;
JSNative CItem_Dupe;
JSNative CItem_LockDown;
JSNative CItem_Carve;
JSNative CItem_GetTileName;
JSNative CItem_GetMoreVar;
JSNative CItem_SetMoreVar;


// BaseObject Methods
JSNative CBase_TextMessage;
JSNative CBase_Delete;
JSNative CBase_Teleport;
JSNative CBase_StaticEffect;
JSNative CBase_GetTag;
JSNative CBase_SetTag;
JSNative CBase_GetTempTag;
JSNative CBase_SetTempTag;
JSNative CBase_GetNumTags;
JSNative CBase_GetTagMap;
JSNative CBase_GetTempTagMap;
JSNative CBase_InRange;
JSNative CBase_StartTimer;
JSNative CBase_KillTimers;
JSNative CBase_GetJSTimer;
JSNative CBase_SetJSTimer;
JSNative CBase_KillJSTimer;
JSNative CBase_ApplySection;
JSNative CBase_FirstItem;
JSNative CBase_NextItem;
JSNative CBase_FinishedItems;
JSNative CBase_DistanceTo;
JSNative CBase_GetSerial;
JSNative CBase_UpdateStats;
JSNative CBase_Resist;
JSNative CBase_IsBoat;
JSNative CBase_CanSee;
JSNative CBase_UseResource;
JSNative CBase_AddScriptTrigger;
JSNative CBase_HasScriptTrigger;
JSNative CBase_RemoveScriptTrigger;
JSNative CBase_Refresh;

// Multi Methods
JSNative CMulti_GetMultiCorner;
JSNative CMulti_SecureContainer;
JSNative CMulti_UnsecureContainer;
JSNative CMulti_IsSecureContainer;
JSNative CMulti_LockDownItem;
JSNative CMulti_ReleaseItem;
JSNative CMulti_AddTrashCont;
JSNative CMulti_RemoveTrashCont;
JSNative CMulti_KillKeys;
JSNative CItem_IsMulti;
JSNative CMulti_IsInMulti;
JSNative CMulti_IsOnBanList;
JSNative CMulti_IsOnFriendList;
JSNative CMulti_IsOnGuestList;
JSNative CMulti_IsOnOwnerList;
JSNative CMulti_IsOwner;
JSNative CMulti_AddToBanList;
JSNative CMulti_AddToFriendList;
JSNative CMulti_AddToGuestList;
JSNative CMulti_AddToOwnerList;
JSNative CMulti_RemoveFromBanList;
JSNative CMulti_RemoveFromFriendList;
JSNative CMulti_RemoveFromGuestList;
JSNative CMulti_RemoveFromOwnerList;
JSNative CMulti_ClearBanList;
JSNative CMulti_ClearFriendList;
JSNative CMulti_ClearGuestList;
JSNative CMulti_ClearOwnerList;
JSNative CMulti_FirstChar;
JSNative CMulti_NextChar;
JSNative CMulti_FinishedChars;

// Socket Methods
JSNative CSocket_Disconnect;
JSNative CSocket_OpenURL;
JSNative CSocket_GetByte;
JSNative CSocket_GetWord;
JSNative CSocket_GetDWord;
JSNative CSocket_GetSByte;
JSNative CSocket_GetSWord;
JSNative CSocket_GetSDWord;
JSNative CSocket_GetString;
JSNative CSocket_SetByte;
JSNative CSocket_SetWord;
JSNative CSocket_SetDWord;
JSNative CSocket_SetString;
JSNative CSocket_ReadBytes;
JSNative CSocket_OpenContainer;
JSNative CSocket_OpenGump;
JSNative CSocket_CloseGump;
JSNative CSocket_WhoList;
JSNative CSocket_Music;
JSNative CSocket_SendAddMenu;
JSNative CSocket_Send;
JSNative CSocket_DisplayDamage;
JSNative CSocket_Page;

// Guild Methods
JSNative CGuild_AcceptRecruit;
JSNative CGuild_IsAtPeace;

// Misc
JSNative CMisc_SoundEffect;
JSNative CMisc_SellTo;
JSNative CMisc_BuyFrom;
JSNative CMisc_HasSpell;
JSNative CMisc_RemoveSpell;
JSNative CMisc_CustomTarget;
JSNative CMisc_PopUpTarget;
JSNative CMisc_GetTimer;
JSNative CMisc_SetTimer;
JSNative CMisc_MakeMenu;
JSNative CMisc_SysMessage;

// Race methods
JSNative CRace_CanWearArmour;
JSNative CRace_IsValidHairColour;
JSNative CRace_IsValidSkinColour;
JSNative CRace_IsValidBeardColour;

// File methods

JSNative CFile_Open;
JSNative CFile_Close;
JSNative CFile_Free;
JSNative CFile_Write;
JSNative CFile_ReadUntil;
JSNative CFile_Read;
JSNative CFile_EOF;
JSNative CFile_Length;
JSNative CFile_Pos;

// Account methods
JSNative CAccount_AddAccount;
JSNative CAccount_DelAccount;

JSNative CConsole_Print;
JSNative CConsole_Log;
JSNative CConsole_Error;
JSNative CConsole_Warning;
JSNative CConsole_PrintSectionBegin;
JSNative CConsole_TurnYellow;
JSNative CConsole_TurnRed;
JSNative CConsole_TurnGreen;
JSNative CConsole_TurnBlue;
JSNative CConsole_TurnNormal;
JSNative CConsole_TurnBrightWhite;
JSNative CConsole_PrintDone;
JSNative CConsole_PrintFailed;
JSNative CConsole_PrintPassed;
JSNative CConsole_ClearScreen;
JSNative CConsole_PrintBasedOnVal;
JSNative CConsole_MoveTo;
JSNative CConsole_PrintSpecial;
JSNative CConsole_BeginShutdown;
JSNative CConsole_Reload;

JSNative CParty_GetMember;
JSNative CParty_Add;
JSNative CParty_Remove;

JSNative CSocket_FirstTriggerWord;
JSNative CSocket_NextTriggerWord;
JSNative CSocket_FinishedTriggerWords;

JSNative CRegion_AddScriptTrigger;
JSNative CRegion_RemoveScriptTrigger;
JSNative CRegion_GetOrePref;
JSNative CRegion_GetOreChance;

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
	{ "SetRandomName",		CChar_SetRandomName,	1, 0, 0 },
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
