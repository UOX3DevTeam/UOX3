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
bool Gump(JSContext* cx, unsigned argc, JS::Value* vp);
bool GumpData(JSContext* cx, unsigned argc, JS::Value* vp);
bool UOXCFile(JSContext* cx, unsigned argc, JS::Value* vp);
bool Packet(JSContext* cx, unsigned argc, JS::Value* vp);

// Packet Methods
bool CPacket_Free(JSContext* cx, unsigned argc, JS::Value* vp);
bool CPacket_WriteByte(JSContext* cx, unsigned argc, JS::Value* vp);
bool CPacket_WriteShort(JSContext* cx, unsigned argc, JS::Value* vp);
bool CPacket_WriteLong(JSContext* cx, unsigned argc, JS::Value* vp);
bool CPacket_WriteString(JSContext* cx, unsigned argc, JS::Value* vp);
bool CPacket_ReserveSize(JSContext* cx, unsigned argc, JS::Value* vp);

// Gump Methods
bool CGump_Free(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddBackground(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddButton(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddButtonTileArt(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddPageButton(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddCheckbox(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddCheckerTrans(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddCroppedText(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddGroup(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_EndGroup(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddGumpColor(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddHTMLGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddPage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddPicture(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddPictureColor(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddPicInPic(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddItemProperty(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddRadio(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddText(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddTextEntry(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddTextEntryLimited(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddTiledGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddToolTip(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddXMFHTMLGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddXMFHTMLGumpColor(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_AddXMFHTMLTok(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_MasterGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_NoClose(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_NoDispose(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_NoMove(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_NoResize(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGump_Send(JSContext* cx, unsigned argc, JS::Value* vp);

// GumpData Methods
bool CGumpData_Free(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGumpData_GetId(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGumpData_GetEdit(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGumpData_GetButton(JSContext* cx, unsigned argc, JS::Value* vp);

// Character Methods
bool CChar_DoAction(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_EmoteMessage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_OpenBank(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_DirectionTo(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_TurnToward(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ResourceCount(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_CheckSkill(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_FindItemLayer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SpeechInput(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_CastSpell(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_MagicEffect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Wander(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Follow(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Dismount(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SetPoisoned(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ExplodeItem(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SetInvisible(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_YellMessage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_WhisperMessage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_AddSpell(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SpellFail(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ExecuteCommand(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_WalkTo(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_RunTo(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_OpenLayer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_BoltEffect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Gate(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Recall(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Mark(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SetSkillByName(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Kill(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Resurrect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Dupe(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Jail(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Release(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SpellMoveEffect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_SpellStaticEffect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_BreakConcentration(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_FindItemType(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_FindItemSection(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_InitWanderArea(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ReactOnDamage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Damage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_InitiateCombat(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_InvalidateAttacker(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_AddAggressorFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_RemoveAggressorFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_CheckAggressorFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_UpdateAggressorFlagTimestamp(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_IsAggressor(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ClearAggressorFlags(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_AddPermaGreyFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_RemovePermaGreyFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_CheckPermaGreyFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_UpdatePermaGreyFlagTimestamp(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_IsPermaGrey(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ClearPermaGreyFlags(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Heal(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_Defense(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_AddFriend(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_RemoveFriend(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_GetFriendList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_ClearFriendList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_GetPetList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_GetFollowerList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_AddFollower(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_RemoveFollower(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_HasBeenOwner(JSContext* cx, unsigned argc, JS::Value* vp);
bool CChar_CalculateControlChance(JSContext* cx, unsigned argc, JS::Value* vp);


// Item Methods
bool CItem_OpenPlank(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_SetCont(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_ApplyRank(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_IsOnFoodList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_Glow(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_UnGlow(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_PlaceInPack(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_Dupe(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_LockDown(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_Carve(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_GetTileName(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_GetMoreVar(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_SetMoreVar(JSContext* cx, unsigned argc, JS::Value* vp);


// BaseObject Methods
bool CBase_TextMessage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_Delete(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_Teleport(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_StaticEffect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetTag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_SetTag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetTempTag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_SetTempTag(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetNumTags(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetTagMap(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetTempTagMap(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_InRange(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_StartTimer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_KillTimers(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetJSTimer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_SetJSTimer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_KillJSTimer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_ApplySection(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_FirstItem(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_NextItem(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_FinishedItems(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_DistanceTo(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_GetSerial(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_UpdateStats(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_Resist(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_IsBoat(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_CanSee(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_UseResource(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_AddScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_HasScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_RemoveScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_Refresh(JSContext* cx, unsigned argc, JS::Value* vp);
bool CBase_SetRandomName(JSContext* cx, unsigned argc, JS::Value* vp);

// Multi Methods
bool CMulti_GetMultiCorner(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_SecureContainer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_UnsecureContainer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsSecureContainer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_LockDownItem(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_ReleaseItem(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_AddTrashCont(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_RemoveTrashCont(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_KillKeys(JSContext* cx, unsigned argc, JS::Value* vp);
bool CItem_IsMulti(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsInMulti(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsOnBanList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsOnFriendList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsOnGuestList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsOnOwnerList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_IsOwner(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_AddToBanList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_AddToFriendList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_AddToGuestList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_AddToOwnerList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_RemoveFromBanList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_RemoveFromFriendList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_RemoveFromGuestList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_RemoveFromOwnerList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_ClearBanList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_ClearFriendList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_ClearGuestList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_ClearOwnerList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_FirstChar(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_NextChar(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMulti_FinishedChars(JSContext* cx, unsigned argc, JS::Value* vp);

// Socket Methods
bool CSocket_Disconnect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_OpenURL(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetByte(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetDWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetSByte(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetSWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetSDWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_GetString(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_SetByte(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_SetWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_SetDWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_SetString(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_ReadBytes(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_OpenContainer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_OpenGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_CloseGump(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_WhoList(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_Music(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_SendAddMenu(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_Send(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_DisplayDamage(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_Page(JSContext* cx, unsigned argc, JS::Value* vp);

// Guild Methods
bool CGuild_AcceptRecruit(JSContext* cx, unsigned argc, JS::Value* vp);
bool CGuild_IsAtPeace(JSContext* cx, unsigned argc, JS::Value* vp);

// Misc
bool CMisc_SoundEffect(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_SellTo(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_BuyFrom(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_HasSpell(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_RemoveSpell(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_CustomTarget(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_PopUpTarget(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_GetTimer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_SetTimer(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_MakeMenu(JSContext* cx, unsigned argc, JS::Value* vp);
bool CMisc_SysMessage(JSContext* cx, unsigned argc, JS::Value* vp);

// Race methods
bool CRace_CanWearArmour(JSContext* cx, unsigned argc, JS::Value* vp);
bool CRace_IsValidHairColour(JSContext* cx, unsigned argc, JS::Value* vp);
bool CRace_IsValidSkinColour(JSContext* cx, unsigned argc, JS::Value* vp);
bool CRace_IsValidBeardColour(JSContext* cx, unsigned argc, JS::Value* vp);

// File methods

bool CFile_Open(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_Close(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_Free(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_Write(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_ReadUntil(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_Read(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_EOF(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_Length(JSContext* cx, unsigned argc, JS::Value* vp);
bool CFile_Pos(JSContext* cx, unsigned argc, JS::Value* vp);

// Account methods
bool CAccount_AddAccount(JSContext* cx, unsigned argc, JS::Value* vp);
bool CAccount_DelAccount(JSContext* cx, unsigned argc, JS::Value* vp);

bool CConsole_Print(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_Log(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_Error(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_Warning(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_PrintSectionBegin(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_TurnYellow(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_TurnRed(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_TurnGreen(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_TurnBlue(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_TurnNormal(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_TurnBrightWhite(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_PrintDone(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_PrintFailed(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_PrintPassed(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_ClearScreen(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_PrintBasedOnVal(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_MoveTo(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_PrintSpecial(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_BeginShutdown(JSContext* cx, unsigned argc, JS::Value* vp);
bool CConsole_Reload(JSContext* cx, unsigned argc, JS::Value* vp);

bool CParty_GetMember(JSContext* cx, unsigned argc, JS::Value* vp);
bool CParty_Add(JSContext* cx, unsigned argc, JS::Value* vp);
bool CParty_Remove(JSContext* cx, unsigned argc, JS::Value* vp);

bool CSocket_FirstTriggerWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_NextTriggerWord(JSContext* cx, unsigned argc, JS::Value* vp);
bool CSocket_FinishedTriggerWords(JSContext* cx, unsigned argc, JS::Value* vp);

bool CRegion_AddScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp);
bool CRegion_RemoveScriptTrigger(JSContext* cx, unsigned argc, JS::Value* vp);
bool CRegion_GetOrePref(JSContext* cx, unsigned argc, JS::Value* vp);
bool CRegion_GetOreChance(JSContext* cx, unsigned argc, JS::Value* vp);

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
	JS_FN( "SetRandomName",		CBase_SetRandomName,	1, 0),
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
	JS_FN( "SetRandomName",		CBase_SetRandomName,	1, 0),

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
