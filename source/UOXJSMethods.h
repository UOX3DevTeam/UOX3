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
	JS_FN( "Free",						CGump_Free,							0, 0),
	JS_FN( "AddBackground",				CGump_AddBackground,				5, 0),
	JS_FN( "AddButton",					CGump_AddButton,					6, 0),
	JS_FN( "AddButtonTileArt",			CGump_AddButtonTileArt,				11, 0),
	JS_FN( "AddPageButton",				CGump_AddPageButton,				4, 0),
	JS_FN( "AddCheckbox",				CGump_AddCheckbox,					6, 0),
	JS_FN( "AddCheckerTrans",			CGump_AddCheckerTrans,				4, 0),
	JS_FN( "AddCroppedText",				CGump_AddCroppedText,				6, 0),
	JS_FN( "AddGroup",					CGump_AddGroup,						1, 0),
	JS_FN( "EndGroup",					CGump_EndGroup,						0, 0),
	JS_FN( "AddGump",					CGump_AddGump,						4, 0),
	JS_FN( "AddGumpColor",				CGump_AddGumpColor,					4, 0),
	JS_FN( "AddHTMLGump",				CGump_AddHTMLGump,					7, 0),
	JS_FN( "AddPage",					CGump_AddPage,						1, 0),
	JS_FN( "AddPicture",					CGump_AddPicture,					3, 0),
	JS_FN( "AddPictureColor",			CGump_AddPictureColor,				4, 0),
	JS_FN( "AddPicInPic",				CGump_AddPicInPic,					7, 0),
	JS_FN( "AddItemProperty",			CGump_AddItemProperty,				1, 0),
	JS_FN( "AddRadio",					CGump_AddRadio,						6, 0),
	JS_FN( "AddText",					CGump_AddText,						4, 0),
	JS_FN( "AddTextEntry",				CGump_AddTextEntry,					8, 0),
	JS_FN( "AddTextEntryLimited",		CGump_AddTextEntryLimited,			9, 0),
	JS_FN( "AddTiledGump",				CGump_AddTiledGump,					5, 0),
	JS_FN( "AddToolTip",					CGump_AddToolTip,					11, 0),
	JS_FN( "AddXMFHTMLGump",				CGump_AddXMFHTMLGump,				7, 0),
	JS_FN( "AddXMFHTMLGumpColor",		CGump_AddXMFHTMLGumpColor,			8, 0),
	JS_FN( "AddXMFHTMLTok",				CGump_AddXMFHTMLTok,				8, 0),
	JS_FN( "MasterGump",					CGump_MasterGump,					1, 0),
	JS_FN( "NoClose",					CGump_NoClose,						0, 0),
	JS_FN( "NoDispose",					CGump_NoDispose,					0, 0),
	JS_FN( "NoMove",						CGump_NoMove,						0, 0),
	JS_FN( "NoResize",					CGump_NoResize,						0, 0),
	JS_FN( "Send",						CGump_Send,							1, 0),
	JS_FS_END
};

inline JSFunctionSpec CChar_Methods[] =
{
	JS_FN( "KillTimers",			CBase_KillTimers,		1, 0),
	JS_FN( "GetJSTimer",			CBase_GetJSTimer,		2, 0),
	JS_FN( "SetJSTimer",			CBase_SetJSTimer,		3, 0),
	JS_FN( "KillJSTimer",		CBase_KillJSTimer,		2, 0),
	JS_FN( "TextMessage",		CBase_TextMessage,		1, 0),
	JS_FN( "YellMessage",		CChar_YellMessage,		1, 0),
	JS_FN( "WhisperMessage",		CChar_WhisperMessage,	1, 0),
	JS_FN( "EmoteMessage",		CChar_EmoteMessage,		1, 0),
	JS_FN( "Delete",				CBase_Delete,			0, 0),
	JS_FN( "DoAction",			CChar_DoAction,			1, 0),
	JS_FN( "StaticEffect",		CBase_StaticEffect,		3, 0),
	JS_FN( "Teleport",			CBase_Teleport,			5, 0),
	JS_FN( "SetLocation",		CBase_Teleport,			5, 0),
	JS_FN( "SoundEffect",		CMisc_SoundEffect,		2, 0),
	JS_FN( "SellTo",				CMisc_SellTo,			1, 0),
	JS_FN( "BuyFrom",			CMisc_BuyFrom,			1, 0),
	JS_FN( "HasSpell",			CMisc_HasSpell,			1, 0),
	JS_FN( "RemoveSpell",		CMisc_RemoveSpell,		1, 0),
	JS_FN( "GetTag",				CBase_GetTag,			1, 0),
	JS_FN( "SetTag",				CBase_SetTag,			2, 0),
	JS_FN( "GetTempTag",			CBase_GetTempTag,		1, 0),
	JS_FN( "SetTempTag",			CBase_SetTempTag,		2, 0),
	JS_FN( "GetNumTags",			CBase_GetNumTags,		0, 0),
	JS_FN( "GetTagMap",			CBase_GetTagMap,		0, 0),
	JS_FN( "GetTempTagMap",		CBase_GetTempTagMap,	0, 0),
	JS_FN( "OpenBank",			CChar_OpenBank,			1, 0),
	JS_FN( "DirectionTo",		CChar_DirectionTo,		1, 0),
	JS_FN( "TurnToward",			CChar_TurnToward,		1, 0),
	JS_FN( "ResourceCount",		CChar_ResourceCount,	2, 0),
	JS_FN( "UseResource",		CBase_UseResource,		3, 0),
	JS_FN( "CustomTarget",		CMisc_CustomTarget,		1, 0),
	JS_FN( "PopUpTarget",		CMisc_PopUpTarget,		1, 0),
	JS_FN( "InRange",			CBase_InRange,			2, 0),
	JS_FN( "FindItemLayer",		CChar_FindItemLayer,	1, 0),
	JS_FN( "StartTimer",			CBase_StartTimer,		2, 0),
	JS_FN( "CheckSkill",			CChar_CheckSkill,		4, 0),
	JS_FN( "SpeechInput",		CChar_SpeechInput,		1, 0),
	JS_FN( "CastSpell",			CChar_CastSpell,		2, 0),
	JS_FN( "SysMessage",			CMisc_SysMessage,		10, 0),
	JS_FN( "MagicEffect",		CChar_MagicEffect,		2, 0),
	JS_FN( "GetSerial",			CBase_GetSerial,		1, 0),
	JS_FN( "UpdateStats",		CBase_UpdateStats,		1, 0),
	JS_FN( "MakeMenu",			CMisc_MakeMenu,			2, 0),
	JS_FN( "Wander",				CChar_Wander,			3, 0),
	JS_FN( "Follow",				CChar_Follow,			1, 0),
	JS_FN( "Dismount",			CChar_Dismount,			0, 0),
	JS_FN( "SetPoisoned",		CChar_SetPoisoned,		2, 0),
	JS_FN( "ExplodeItem",		CChar_ExplodeItem,		3, 0),
	JS_FN( "SetInvisible",		CChar_SetInvisible,		2, 0),
	JS_FN( "ApplySection",		CBase_ApplySection,		1, 0),
	JS_FN( "Refresh",			CBase_Refresh,			0, 0),
	JS_FN( "AddSpell",			CChar_AddSpell,			1, 0),
	JS_FN( "SpellFail",			CChar_SpellFail,		0, 0),
	JS_FN( "FirstItem",			CBase_FirstItem,		0, 0),
	JS_FN( "NextItem",			CBase_NextItem,			0, 0),
	JS_FN( "FinishedItems",		CBase_FinishedItems,	0, 0),
	JS_FN( "ExecuteCommand",     CChar_ExecuteCommand,   0, 0),
	JS_FN( "WalkTo",				CChar_WalkTo,			1, 0),
	JS_FN( "RunTo",				CChar_RunTo,			1, 0),
	JS_FN( "DistanceTo",			CBase_DistanceTo,		1, 0),
	JS_FN( "OpenLayer",			CChar_OpenLayer,		2, 0),
	JS_FN( "BoltEffect",			CChar_BoltEffect,		0, 0),
	JS_FN( "Gate",				CChar_Gate,				1, 0),
	JS_FN( "Recall",				CChar_Recall,			1, 0),
	JS_FN( "Mark",				CChar_Mark,				1, 0),
	JS_FN( "SetRandomName",		CChar_SetRandomName,	1, 0),
	JS_FN( "SetSkillByName",		CChar_SetSkillByName,	2, 0),
	JS_FN( "Kill",				CChar_Kill,				0, 0),
	JS_FN( "Resurrect",			CChar_Resurrect,		0, 0),
	JS_FN( "Dupe",				CChar_Dupe,				0, 0),
	JS_FN( "Jail",				CChar_Jail,				0, 0),
	JS_FN( "Release",			CChar_Release,			0, 0),
	JS_FN( "GetTimer",			CMisc_GetTimer,			1, 0),
	JS_FN( "SetTimer",			CMisc_SetTimer,			2, 0),
	JS_FN( "SpellMoveEffect",	CChar_SpellMoveEffect,	2, 0),
	JS_FN( "SpellStaticEffect",	CChar_SpellStaticEffect,1, 0),
	JS_FN( "BreakConcentration",	CChar_BreakConcentration,0, 0),
	JS_FN( "FindItemType",		CChar_FindItemType,		1, 0),
	JS_FN( "FindItemSection",	CChar_FindItemSection,	1, 0),
	JS_FN( "InitWanderArea",		CChar_InitWanderArea,	0, 0),
	JS_FN( "CanSee",				CBase_CanSee,			1, 0),
	JS_FN( "ReactOnDamage",		CChar_ReactOnDamage,	1, 0),
	JS_FN( "Damage",				CChar_Damage,			1, 0),
	JS_FN( "InitiateCombat",		CChar_InitiateCombat,	1, 0),
	JS_FN( "InvalidateAttacker",		CChar_InvalidateAttacker,				0, 0),
	JS_FN( "AddAggressorFlag",		CChar_AddAggressorFlag,					1, 0),
	JS_FN( "RemoveAggressorFlag",	CChar_RemoveAggressorFlag,				1, 0),
	JS_FN( "CheckAggressorFlag",		CChar_CheckAggressorFlag,					1, 0),
	JS_FN( "UpdateAggressorFlagTimestamp",	CChar_UpdateAggressorFlagTimestamp,	1, 0),
	JS_FN( "IsAggressor",			CChar_IsAggressor,						1, 0),
	JS_FN( "ClearAggressorFlags",	CChar_ClearAggressorFlags,				0, 0),
	JS_FN( "AddPermaGreyFlag",		CChar_AddPermaGreyFlag,					1, 0),
	JS_FN( "RemovePermaGreyFlag",	CChar_RemovePermaGreyFlag,				1, 0),
	JS_FN( "CheckPermaGreyFlag",		CChar_CheckPermaGreyFlag,					1, 0),
	JS_FN( "UpdatePermaGreyFlagTimestamp",	CChar_UpdatePermaGreyFlagTimestamp,	1, 0),
	JS_FN( "IsPermaGrey",			CChar_IsPermaGrey,						1, 0),
	JS_FN( "ClearPermaGreyFlags",	CChar_ClearPermaGreyFlags,				0, 0),
	JS_FN( "Heal",				CChar_Heal,				1, 0),
	JS_FN( "Resist",				CBase_Resist,			1, 0),
	JS_FN( "Defense",			CChar_Defense,			3, 0),
	JS_FN( "AddScriptTrigger",	CBase_AddScriptTrigger,		1, 0),
	JS_FN( "HasScriptTrigger",	CBase_HasScriptTrigger,		1, 0),
	JS_FN( "RemoveScriptTrigger",CBase_RemoveScriptTrigger,	1, 0),
	JS_FN( "AddFriend",			CChar_AddFriend,			1, 0),
	JS_FN( "RemoveFriend",		CChar_RemoveFriend,			1, 0),
	JS_FN( "GetFriendList",		CChar_GetFriendList,		0, 0),
	JS_FN( "ClearFriendList",	CChar_ClearFriendList,		0, 0),
	JS_FN( "GetPetList",			CChar_GetPetList,			0, 0),
	JS_FN( "GetFollowerList",	CChar_GetFollowerList,		0, 0),
	JS_FN( "AddFollower",		CChar_AddFollower,			1, 0),
	JS_FN( "RemoveFollower",		CChar_RemoveFollower,		1, 0),
	JS_FN( "HasBeenOwner",		CChar_HasBeenOwner,			1, 0),
	JS_FN( "CalculateControlChance",	CChar_CalculateControlChance,	1, 0),
	JS_FS_END
};

inline JSFunctionSpec CItem_Methods[] =
{
	JS_FN( "KillTimers",			CBase_KillTimers,			1, 0),
	JS_FN( "GetJSTimer",			CBase_GetJSTimer,			2, 0),
	JS_FN( "SetJSTimer",			CBase_SetJSTimer,			3, 0),
	JS_FN( "KillJSTimer",		CBase_KillJSTimer,			2, 0),
	JS_FN( "Delete",				CBase_Delete,				0, 0),
	JS_FN( "TextMessage",		CBase_TextMessage,			1, 0),
	JS_FN( "Teleport",			CBase_Teleport,				5, 0),
	JS_FN( "StaticEffect",		CBase_StaticEffect,			4, 0),
	JS_FN( "SetLocation",		CBase_Teleport,				5, 0),
	JS_FN( "SoundEffect",		CMisc_SoundEffect,			2, 0),
	JS_FN( "HasSpell",			CMisc_HasSpell,				1, 0),
	JS_FN( "RemoveSpell",		CMisc_RemoveSpell,			1, 0),
	JS_FN( "GetTag",				CBase_GetTag,				1, 0),
	JS_FN( "SetTag",				CBase_SetTag,				1, 0),
	JS_FN( "GetTempTag",			CBase_GetTempTag,			1, 0),
	JS_FN( "SetTempTag",			CBase_SetTempTag,			2, 0),
	JS_FN( "GetNumTags",			CBase_GetNumTags,			0, 0),
	JS_FN( "GetTagMap",			CBase_GetTagMap,			0, 0),
	JS_FN( "GetTempTagMap",		CBase_GetTempTagMap,		0, 0),
	JS_FN( "InRange",			CBase_InRange,				2, 0),
	JS_FN( "StartTimer",			CBase_StartTimer,			2, 0),
	JS_FN( "OpenPlank",			CItem_OpenPlank,			0, 0),
	JS_FN( "IsMulti",			CItem_IsMulti,				0, 0),
	JS_FN( "IsBoat",				CBase_IsBoat,				0, 0),
	JS_FN( "IsInMulti",			CMulti_IsInMulti,			1, 0),
	JS_FN( "IsOnBanList",		CMulti_IsOnBanList,			1, 0),
	JS_FN( "IsOnFriendList",		CMulti_IsOnFriendList,		1, 0),
	JS_FN( "IsOnGuestList",		CMulti_IsOnGuestList,		1, 0),
	JS_FN( "IsOwner",			CMulti_IsOwner,				1, 0),
	JS_FN( "IsOnOwnerList",		CMulti_IsOnOwnerList,		1, 0),
	JS_FN( "AddToBanList",		CMulti_AddToBanList,		1, 0),
	JS_FN( "AddToFriendList",	CMulti_AddToFriendList,		1, 0),
	JS_FN( "AddToGuestList",		CMulti_AddToGuestList,		1, 0),
	JS_FN( "AddToOwnerList",		CMulti_AddToOwnerList,		1, 0),
	JS_FN( "RemoveFromBanList",		CMulti_RemoveFromBanList,		1, 0),
	JS_FN( "RemoveFromFriendList",	CMulti_RemoveFromFriendList,	1, 0),
	JS_FN( "RemoveFromGuestList",	CMulti_RemoveFromGuestList,		1, 0),
	JS_FN( "RemoveFromOwnerList",	CMulti_RemoveFromOwnerList,		1, 0),
	JS_FN( "ClearBanList",		CMulti_ClearBanList,		1, 0),
	JS_FN( "ClearFriendList",	CMulti_ClearFriendList,		1, 0),
	JS_FN( "ClearGuestList",		CMulti_ClearGuestList,		1, 0),
	JS_FN( "ClearOwnerList",		CMulti_ClearOwnerList,		1, 0),
	JS_FN( "GetSerial",			CBase_GetSerial,			1, 0),
	JS_FN( "UpdateStats",		CBase_UpdateStats,			1, 0),
	JS_FN( "SetCont",			CItem_SetCont,				1, 0),
	JS_FN( "ApplySection",		CBase_ApplySection,			1, 0),
	JS_FN( "Refresh",			CBase_Refresh,				0, 0),
	JS_FN( "ApplyRank",			CItem_ApplyRank,			2, 0),
	JS_FN( "IsOnFoodList",		CItem_IsOnFoodList,			1, 0),
	JS_FN( "FirstItem",			CBase_FirstItem,			0, 0),
	JS_FN( "NextItem",			CBase_NextItem,				0, 0),
	JS_FN( "FinishedItems",		CBase_FinishedItems,		0, 0),
	JS_FN( "DistanceTo",			CBase_DistanceTo,			1, 0),
	JS_FN( "Glow",				CItem_Glow,					1, 0),
	JS_FN( "UnGlow",				CItem_UnGlow,				1, 0),
	JS_FN( "PlaceInPack",		CItem_PlaceInPack,			0, 0),
	JS_FN( "Dupe",				CItem_Dupe,					1, 0),
	JS_FN( "LockDown",			CItem_LockDown,				0, 0),
	JS_FN( "Carve",				CItem_Carve,				1, 0),
	JS_FN( "GetTileName",		CItem_GetTileName,			0, 0),
	JS_FN( "GetMoreVar",			CItem_GetMoreVar,			2, 0),
	JS_FN( "SetMoreVar",			CItem_SetMoreVar,			3, 0),
	JS_FN( "Resist",				CBase_Resist,				1, 0),
	JS_FN( "UseResource",		CBase_UseResource,			3, 0),
	JS_FN( "AddScriptTrigger",	CBase_AddScriptTrigger,		1, 0),
	JS_FN( "HasScriptTrigger",	CBase_HasScriptTrigger,		1, 0),
	JS_FN( "RemoveScriptTrigger",CBase_RemoveScriptTrigger,	1, 0),
	JS_FN( "GetMultiCorner",		CMulti_GetMultiCorner,		1, 0),
	JS_FN( "SecureContainer",	CMulti_SecureContainer,		1, 0),
	JS_FN( "UnsecureContainer",	CMulti_UnsecureContainer,	1, 0),
	JS_FN( "IsSecureContainer",	CMulti_IsSecureContainer,	1, 0),
	JS_FN( "LockDownItem",		CMulti_LockDownItem,		1, 0),
	JS_FN( "ReleaseItem",		CMulti_ReleaseItem,			1, 0),
	JS_FN( "AddTrashCont",		CMulti_AddTrashCont,		1, 0),
	JS_FN( "RemoveTrashCont",	CMulti_RemoveTrashCont,		1, 0),

	JS_FN( "KillKeys",			CMulti_KillKeys,			1, 0),

	JS_FN( "FirstChar",			CMulti_FirstChar,			1, 0),
	JS_FN( "NextChar",			CMulti_NextChar,			1, 0),
	JS_FN( "FinishedChars",		CMulti_FinishedChars,		1, 0),

	//{ "SetMoreSerial",		CBase_SetMoreSerial,		1, 0),
	JS_FS_END
};

inline JSFunctionSpec CRegion_Methods[] =
{
	JS_FN( "AddScriptTrigger",		CRegion_AddScriptTrigger,		1, 0),
	JS_FN( "RemoveScriptTrigger",	CRegion_RemoveScriptTrigger,	1, 0),
	JS_FN( "GetOrePref",				CRegion_GetOrePref,				1, 0),
	JS_FN( "GetOreChance",			CRegion_GetOreChance,			0, 0),
	JS_FS_END
};

inline JSFunctionSpec CSocket_Methods[] =
{
	JS_FN( "SysMessage",			CMisc_SysMessage,	10, 0),
	JS_FN( "Disconnect",			CSocket_Disconnect,	0, 0),
	JS_FN( "SoundEffect",		CMisc_SoundEffect,	2, 0),
	JS_FN( "CustomTarget",		CMisc_CustomTarget, 1, 0),
	JS_FN( "PopUpTarget",		CMisc_PopUpTarget,	1, 0),
	JS_FN( "GetByte",			CSocket_GetByte,	1, 0),
	JS_FN( "GetWord",			CSocket_GetWord,	1, 0),
	JS_FN( "GetDWord",			CSocket_GetDWord,	1, 0),
	JS_FN( "GetSByte",			CSocket_GetSByte,	1, 0),
	JS_FN( "GetSWord",			CSocket_GetSWord,	1, 0),
	JS_FN( "GetSDWord",			CSocket_GetSDWord,	1, 0),
	JS_FN( "GetString",			CSocket_GetString,	1, 0),
	JS_FN( "SetByte",			CSocket_SetByte,	2, 0),
	JS_FN( "SetWord",			CSocket_SetWord,	2, 0),
	JS_FN( "SetDWord",			CSocket_SetDWord,	2, 0),
	JS_FN( "SetString",			CSocket_SetString,	2, 0),
	JS_FN( "ReadBytes",			CSocket_ReadBytes,	1, 0),
	JS_FN( "OpenContainer",		CSocket_OpenContainer,	1, 0),
	JS_FN( "OpenGump",			CSocket_OpenGump,	1, 0),
	JS_FN( "CloseGump",			CSocket_CloseGump,	2, 0),
	JS_FN( "OpenURL",			CSocket_OpenURL,    1, 0),
	JS_FN( "BuyFrom",			CMisc_BuyFrom,		1, 0),
	JS_FN( "SellTo",				CMisc_SellTo,		1, 0),
	JS_FN( "WhoList",			CSocket_WhoList,	0, 0),
	JS_FN( "Music",				CSocket_Music,		1, 0),
	JS_FN( "GetTimer",			CMisc_GetTimer,		1, 0),
	JS_FN( "SetTimer",			CMisc_SetTimer,		2, 0),
	JS_FN( "SendAddMenu",		CSocket_SendAddMenu,1, 0),
	JS_FN( "Page",				CSocket_Page,		1, 0),
	JS_FN( "MakeMenu",			CMisc_MakeMenu,		2, 0),
	JS_FN( "Send",				CSocket_Send,		1, 0),
	JS_FN( "CanSee",				CBase_CanSee,		1, 0),
	JS_FN( "DisplayDamage",		CSocket_DisplayDamage, 2, 0),
	JS_FN( "FirstTriggerWord",	CSocket_FirstTriggerWord, 0, 0),
	JS_FN( "NextTriggerWord",	CSocket_NextTriggerWord, 0, 0),
	JS_FN( "FinishedTriggerWords",	CSocket_FinishedTriggerWords, 0, 0),
	JS_FS_END
};

inline JSFunctionSpec CGuild_Methods[] =
{
	JS_FN( "AcceptRecruit",		CGuild_AcceptRecruit,		1, 0),
	JS_FN( "IsAtPeace",			CGuild_IsAtPeace,			0, 0),
	JS_FS_END
};

inline JSFunctionSpec CRace_Methods[] =
{
	JS_FN( "CanWearArmour",		CRace_CanWearArmour,		1, 0),
	JS_FN( "IsValidHairColour",	CRace_IsValidHairColour,	1, 0),
	JS_FN( "IsValidSkinColour",	CRace_IsValidSkinColour,	1, 0),
	JS_FN( "IsValidBeardColour",	CRace_IsValidBeardColour,	1, 0),
	JS_FS_END
};

inline JSFunctionSpec CGumpData_Methods[] =
{
	JS_FN( "Free",		CGumpData_Free,			0, 0),
	JS_FN( "getButton",	CGumpData_GetButton,	1, 0),
	JS_FN( "getID",		CGumpData_GetId,		1, 0),
	JS_FN( "getEdit",	CGumpData_GetEdit,		1, 0),
	JS_FS_END
};

inline JSFunctionSpec CFile_Methods[] =
{
	JS_FN( "Free",		CFile_Free,				0, 0),
	JS_FN( "Open",		CFile_Open,				2, 0),
	JS_FN( "Close",		CFile_Close,			0, 0),
	JS_FN( "Write",		CFile_Write,			1, 0),
	JS_FN( "Read",		CFile_Read,				1, 0),
	JS_FN( "ReadUntil",	CFile_ReadUntil,		1, 0),
	JS_FN( "EOF",		CFile_EOF,				0, 0),
	JS_FN( "Length",		CFile_Length,			0, 0),
	JS_FN( "Pos",		CFile_Pos,				1, 0),
	JS_FS_END
};

inline JSFunctionSpec CAccount_Methods[] =
{
	JS_FN( "AddAccount",		CAccount_AddAccount,		4, 0),
	JS_FN( "DelAccount",		CAccount_DelAccount,		1, 0),
	JS_FS_END
};

inline JSFunctionSpec CConsole_Methods[] =
{
	JS_FN( "Print",				CConsole_Print,				1, 0),
	JS_FN( "Log",				CConsole_Log,				1, 0),
	JS_FN( "Error",				CConsole_Error,				1, 0),
	JS_FN( "Warning",			CConsole_Warning,			1, 0),
	JS_FN( "PrintSectionBegin",	CConsole_PrintSectionBegin,	0, 0),
	JS_FN( "TurnYellow",			CConsole_TurnYellow,		0, 0),
	JS_FN( "TurnRed",			CConsole_TurnRed,			0, 0),
	JS_FN( "TurnGreen",			CConsole_TurnGreen,			0, 0),
	JS_FN( "TurnBlue",			CConsole_TurnBlue,			0, 0),
	JS_FN( "TurnNormal",			CConsole_TurnNormal,		0, 0),
	JS_FN( "TurnBrightWhite",	CConsole_TurnBrightWhite,	0, 0),
	JS_FN( "PrintDone",			CConsole_PrintDone,			0, 0),
	JS_FN( "PrintFailed",		CConsole_PrintFailed,		0, 0),
	JS_FN( "PrintPassed",		CConsole_PrintPassed,		0, 0),
	JS_FN( "ClearScreen",		CConsole_ClearScreen,		0, 0),
	JS_FN( "PrintBasedOnVal",	CConsole_PrintBasedOnVal,	1, 0),
	JS_FN( "MoveTo",				CConsole_MoveTo,			2, 0),
	JS_FN( "PrintSpecial",		CConsole_PrintSpecial,		2, 0),
	JS_FN( "BeginShutdown",		CConsole_BeginShutdown,		0, 0),
	JS_FN( "Reload",				CConsole_Reload,			1, 0),
	JS_FS_END
};

inline JSFunctionSpec CPacket_Methods[] =
{
	JS_FN( "Free",			CPacket_Free,				0, 0),
	JS_FN( "WriteByte",		CPacket_WriteByte,			2, 0),
	JS_FN( "WriteShort",		CPacket_WriteShort,			2, 0),
	JS_FN( "WriteLong",		CPacket_WriteLong,			2, 0),
	JS_FN( "WriteString",	CPacket_WriteString,		3, 0),
	JS_FN( "ReserveSize",	CPacket_ReserveSize,		1, 0),
	JS_FS_END
};

inline JSFunctionSpec CParty_Methods[] =
{
	JS_FN( "GetMember",		CParty_GetMember,			1, 0),
	JS_FN( "Add",			CParty_Add,					1, 0),
	JS_FN( "Remove",			CParty_Remove,				1, 0),
	JS_FS_END
};

#endif
