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
JSBool Gump( JSContext *cx, uintN argc, jsval *vp );
JSBool GumpData( JSContext *cx, uintN argc, jsval *vp );
JSBool UOXCFile( JSContext *cx, uintN argc, jsval *vp );
JSBool Packet( JSContext *cx, uintN argc, jsval *vp );

// Packet Methods
JSBool CPacket_Free( JSContext *cx, uintN argc, jsval *vp );
JSBool CPacket_WriteByte( JSContext *cx, uintN argc, jsval *vp );
JSBool CPacket_WriteShort( JSContext *cx, uintN argc, jsval *vp );
JSBool CPacket_WriteLong( JSContext *cx, uintN argc, jsval *vp );
JSBool CPacket_WriteString( JSContext *cx, uintN argc, jsval *vp );
JSBool CPacket_ReserveSize( JSContext *cx, uintN argc, jsval *vp );

// Gump Methods
JSBool CGump_Free( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddBackground( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddButton( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddButtonTileArt( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddPageButton( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddCheckbox( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddCheckerTrans( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddCroppedText( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddGroup( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_EndGroup( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddGumpColor( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddHTMLGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddPage( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddPicture( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddPictureColor( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddPicInPic( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddItemProperty( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddRadio( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddText( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddTextEntry( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddTextEntryLimited( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddTiledGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddToolTip( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddXMFHTMLGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddXMFHTMLGumpColor( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_AddXMFHTMLTok( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_MasterGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_NoClose( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_NoDispose( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_NoMove( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_NoResize( JSContext *cx, uintN argc, jsval *vp );
JSBool CGump_Send( JSContext *cx, uintN argc, jsval *vp );

// GumpData Methods
JSBool CGumpData_Free( JSContext *cx, uintN argc, jsval *vp );
JSBool CGumpData_GetId( JSContext *cx, uintN argc, jsval *vp );
JSBool CGumpData_GetEdit( JSContext *cx, uintN argc, jsval *vp );
JSBool CGumpData_GetButton( JSContext *cx, uintN argc, jsval *vp );

// Character Methods
JSBool CChar_DoAction( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_EmoteMessage( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_OpenBank( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_DirectionTo( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_TurnToward( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_CheckSkill( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_FindItemLayer( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SpeechInput( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_CastSpell( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_MagicEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Wander( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Follow( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Dismount( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SetPoisoned( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_ExplodeItem( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SetInvisible( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_YellMessage( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_WhisperMessage( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_AddSpell( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SpellFail( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_ExecuteCommand( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_WalkTo( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_RunTo( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_OpenLayer( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_BoltEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Gate( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Recall( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Mark( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SetSkillByName( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Kill( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Resurrect( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Dupe( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Jail( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Release( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SpellMoveEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_SpellStaticEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_BreakConcentration( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_FindItemType( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_FindItemSection( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_InitWanderArea( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_ReactOnDamage( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Damage( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_InitiateCombat( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_InvalidateAttacker( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_AddAggressorFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_RemoveAggressorFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_CheckAggressorFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_UpdateAggressorFlagTimestamp( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_IsAggressor( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_ClearAggressorFlags( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_AddPermaGreyFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_RemovePermaGreyFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_CheckPermaGreyFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_UpdatePermaGreyFlagTimestamp( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_IsPermaGrey( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_ClearPermaGreyFlags( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Heal( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_Defense( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_AddFriend( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_RemoveFriend( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_GetFriendList( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_ClearFriendList( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_GetPetList( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_GetFollowerList( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_AddFollower( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_RemoveFollower( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_HasBeenOwner( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_CalculateControlChance( JSContext *cx, uintN argc, jsval *vp );
JSBool CChar_CheckGuild( JSContext *cx, uintN argc, jsval *vp );

// Item Methods
JSBool CItem_OpenPlank( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_SetCont( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_ApplyRank( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_IsOnFoodList( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_Glow( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_UnGlow( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_PlaceInPack( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_Dupe( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_LockDown( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_Carve( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_GetTileName( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_GetMoreVar( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_SetMoreVar( JSContext *cx, uintN argc, jsval *vp );


// BaseObject Methods
JSBool CBase_TextMessage( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_Delete( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_Teleport( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_StaticEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetTag( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_SetTag( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetTempTag( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_SetTempTag( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetNumTags( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetTagMap( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetTempTagMap( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_InRange( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_StartTimer( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_KillTimers( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetJSTimer( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_SetJSTimer( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_KillJSTimer( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_ApplySection( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_FirstItem( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_NextItem( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_FinishedItems( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_DistanceTo( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetSerial( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_UpdateStats( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_Resist( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_IsBoat( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_CanSee( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_ResourceCount( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_UseResource( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_AddScriptTrigger( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_HasScriptTrigger( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_RemoveScriptTrigger( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_Refresh( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_SetRandomName( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_SetRandomColor( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_GetTempEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CBase_ReverseEffect( JSContext *cx, uintN argc, jsval *vp );

// Multi Methods
JSBool CMulti_GetMultiCorner( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_SecureContainer( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_UnsecureContainer( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsSecureContainer( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_LockDownItem( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_ReleaseItem( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_AddTrashCont( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_RemoveTrashCont( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_KillKeys( JSContext *cx, uintN argc, jsval *vp );
JSBool CItem_IsMulti( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsInMulti( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsOnBanList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsOnFriendList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsOnGuestList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsOnOwnerList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_IsOwner( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_AddToBanList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_AddToFriendList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_AddToGuestList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_AddToOwnerList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_RemoveFromBanList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_RemoveFromFriendList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_RemoveFromGuestList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_RemoveFromOwnerList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_ClearBanList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_ClearFriendList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_ClearGuestList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_ClearOwnerList( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_FirstChar( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_NextChar( JSContext *cx, uintN argc, jsval *vp );
JSBool CMulti_FinishedChars( JSContext *cx, uintN argc, jsval *vp );

// Socket Methods
JSBool CSocket_Disconnect( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_OpenURL( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetByte( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetDWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetSByte( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetSWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetSDWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_GetString( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_SetByte( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_SetWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_SetDWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_SetString( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_ReadBytes( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_OpenContainer( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_OpenGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_CloseGump( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_WhoList( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_Music( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_SendAddMenu( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_Send( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_DisplayDamage( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_Page( JSContext *cx, uintN argc, jsval *vp );

// Guild Methods
JSBool CGuild_AcceptRecruit( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_IsAtPeace( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_NewMember( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_NewRecruit( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_RemoveRecruit( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_RemoveMember( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_RecruitToMember( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_GuildIsAtWar( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_GuildIsAlly( JSContext *cx, uintN argc, jsval *vp );
JSBool CGuild_GuildIsNeutral( JSContext *cx, uintN argc, jsval *vp );

// Misc
JSBool CMisc_SoundEffect( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_SellTo( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_BuyFrom( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_HasSpell( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_RemoveSpell( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_CustomTarget( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_PopUpTarget( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_GetTimer( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_SetTimer( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_MakeMenu( JSContext *cx, uintN argc, jsval *vp );
JSBool CMisc_SysMessage( JSContext *cx, uintN argc, jsval *vp );

// Race methods
JSBool CRace_CanWearArmour( JSContext *cx, uintN argc, jsval *vp );
JSBool CRace_IsValidHairColour( JSContext *cx, uintN argc, jsval *vp );
JSBool CRace_IsValidSkinColour( JSContext *cx, uintN argc, jsval *vp );
JSBool CRace_IsValidBeardColour( JSContext *cx, uintN argc, jsval *vp );

// File methods

JSBool CFile_Open( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_Close( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_Free( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_Write( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_ReadUntil( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_Read( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_EOF( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_Length( JSContext *cx, uintN argc, jsval *vp );
JSBool CFile_Pos( JSContext *cx, uintN argc, jsval *vp );

// Account methods
JSBool CAccount_AddAccount( JSContext *cx, uintN argc, jsval *vp );
JSBool CAccount_DelAccount( JSContext *cx, uintN argc, jsval *vp );

JSBool CConsole_Print( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_Log( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_Error( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_Warning( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_PrintSectionBegin( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_TurnYellow( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_TurnRed( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_TurnGreen( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_TurnBlue( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_TurnNormal( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_TurnBrightWhite( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_PrintDone( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_PrintFailed( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_PrintPassed( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_ClearScreen( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_PrintBasedOnVal( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_MoveTo( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_PrintSpecial( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_BeginShutdown( JSContext *cx, uintN argc, jsval *vp );
JSBool CConsole_Reload( JSContext *cx, uintN argc, jsval *vp );

JSBool CParty_GetMember( JSContext *cx, uintN argc, jsval *vp );
JSBool CParty_Add( JSContext *cx, uintN argc, jsval *vp );
JSBool CParty_Remove( JSContext *cx, uintN argc, jsval *vp );

JSBool CSocket_FirstTriggerWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_NextTriggerWord( JSContext *cx, uintN argc, jsval *vp );
JSBool CSocket_FinishedTriggerWords( JSContext *cx, uintN argc, jsval *vp );

JSBool CRegion_AddScriptTrigger( JSContext *cx, uintN argc, jsval *vp );
JSBool CRegion_RemoveScriptTrigger( JSContext *cx, uintN argc, jsval *vp );
JSBool CRegion_GetOrePref( JSContext *cx, uintN argc, jsval *vp );
JSBool CRegion_GetOreChance( JSContext *cx, uintN argc, jsval *vp );

inline JSFunctionSpec CGump_Methods[] =
{
	{ "Free",						CGump_Free,							0, 0 },
	{ "AddBackground",				CGump_AddBackground,				5, 0 },
	{ "AddButton",					CGump_AddButton,					6, 0 },
	{ "AddButtonTileArt",			CGump_AddButtonTileArt,				11, 0 },
	{ "AddPageButton",				CGump_AddPageButton,				4, 0 },
	{ "AddCheckbox",				CGump_AddCheckbox,					6, 0 },
	{ "AddCheckerTrans",			CGump_AddCheckerTrans,				4, 0 },
	{ "AddCroppedText",				CGump_AddCroppedText,				6, 0 },
	{ "AddGroup",					CGump_AddGroup,						1, 0 },
	{ "EndGroup",					CGump_EndGroup,						0, 0 },
	{ "AddGump",					CGump_AddGump,						4, 0 },
	{ "AddGumpColor",				CGump_AddGumpColor,					4, 0 },
	{ "AddHTMLGump",				CGump_AddHTMLGump,					7, 0 },
	{ "AddPage",					CGump_AddPage,						1, 0 },
	{ "AddPicture",					CGump_AddPicture,					3, 0 },
	{ "AddPictureColor",			CGump_AddPictureColor,				4, 0 },
	{ "AddPicInPic",				CGump_AddPicInPic,					7, 0 },
	{ "AddItemProperty",			CGump_AddItemProperty,				1, 0 },
	{ "AddRadio",					CGump_AddRadio,						6, 0 },
	{ "AddText",					CGump_AddText,						4, 0 },
	{ "AddTextEntry",				CGump_AddTextEntry,					8, 0 },
	{ "AddTextEntryLimited",		CGump_AddTextEntryLimited,			9, 0 },
	{ "AddTiledGump",				CGump_AddTiledGump,					5, 0 },
	{ "AddToolTip",					CGump_AddToolTip,					11, 0 },
	{ "AddXMFHTMLGump",				CGump_AddXMFHTMLGump,				7, 0 },
	{ "AddXMFHTMLGumpColor",		CGump_AddXMFHTMLGumpColor,			8, 0 },
	{ "AddXMFHTMLTok",				CGump_AddXMFHTMLTok,				8, 0 },
	{ "MasterGump",					CGump_MasterGump,					1, 0 },
	{ "NoClose",					CGump_NoClose,						0, 0 },
	{ "NoDispose",					CGump_NoDispose,					0, 0 },
	{ "NoMove",						CGump_NoMove,						0, 0 },
	{ "NoResize",					CGump_NoResize,						0, 0 },
	{ "Send",						CGump_Send,							1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CChar_Methods[] =
{
	{ "KillTimers",			CBase_KillTimers,		1, 0 },
	{ "GetJSTimer",			CBase_GetJSTimer,		2, 0 },
	{ "SetJSTimer",			CBase_SetJSTimer,		3, 0 },
	{ "KillJSTimer",		CBase_KillJSTimer,		2, 0 },
	{ "TextMessage",		CBase_TextMessage,		1, 0 },
	{ "YellMessage",		CChar_YellMessage,		1, 0 },
	{ "WhisperMessage",		CChar_WhisperMessage,	1, 0 },
	{ "EmoteMessage",		CChar_EmoteMessage,		1, 0 },
	{ "Delete",				CBase_Delete,			0, 0 },
	{ "DoAction",			CChar_DoAction,			1, 0 },
	{ "StaticEffect",		CBase_StaticEffect,		3, 0 },
	{ "Teleport",			CBase_Teleport,			5, 0 },
	{ "SetLocation",		CBase_Teleport,			5, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,		2, 0 },
	{ "SellTo",				CMisc_SellTo,			1, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,			1, 0 },
	{ "HasSpell",			CMisc_HasSpell,			1, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,		1, 0 },
	{ "GetTag",				CBase_GetTag,			1, 0 },
	{ "SetTag",				CBase_SetTag,			2, 0 },
	{ "GetTempTag",			CBase_GetTempTag,		1, 0 },
	{ "SetTempTag",			CBase_SetTempTag,		2, 0 },
	{ "GetNumTags",			CBase_GetNumTags,		0, 0 },
	{ "GetTagMap",			CBase_GetTagMap,		0, 0 },
	{ "GetTempTagMap",		CBase_GetTempTagMap,	0, 0 },
	{ "OpenBank",			CChar_OpenBank,			1, 0 },
	{ "DirectionTo",		CChar_DirectionTo,		1, 0 },
	{ "TurnToward",			CChar_TurnToward,		1, 0 },
	{ "ResourceCount",		CBase_ResourceCount,	2, 0 },
	{ "UseResource",		CBase_UseResource,		3, 0 },
	{ "CustomTarget",		CMisc_CustomTarget,		1, 0 },
	{ "PopUpTarget",		CMisc_PopUpTarget,		1, 0 },
	{ "InRange",			CBase_InRange,			2, 0 },
	{ "FindItemLayer",		CChar_FindItemLayer,	1, 0 },
	{ "StartTimer",			CBase_StartTimer,		2, 0 },
	{ "CheckSkill",			CChar_CheckSkill,		4, 0 },
	{ "SpeechInput",		CChar_SpeechInput,		1, 0 },
	{ "CastSpell",			CChar_CastSpell,		2, 0 },
	{ "SysMessage",			CMisc_SysMessage,		10, 0 },
	{ "MagicEffect",		CChar_MagicEffect,		2, 0 },
	{ "GetSerial",			CBase_GetSerial,		1, 0 },
	{ "UpdateStats",		CBase_UpdateStats,		1, 0 },
	{ "MakeMenu",			CMisc_MakeMenu,			2, 0 },
	{ "Wander",				CChar_Wander,			3, 0 },
	{ "Follow",				CChar_Follow,			1, 0 },
	{ "Dismount",			CChar_Dismount,			0, 0 },
	{ "SetPoisoned",		CChar_SetPoisoned,		2, 0 },
	{ "ExplodeItem",		CChar_ExplodeItem,		3, 0 },
	{ "SetInvisible",		CChar_SetInvisible,		2, 0 },
	{ "ApplySection",		CBase_ApplySection,		1, 0 },
	{ "Refresh",			CBase_Refresh,			0, 0 },
	{ "AddSpell",			CChar_AddSpell,			1, 0 },
	{ "SpellFail",			CChar_SpellFail,		0, 0 },
	{ "FirstItem",			CBase_FirstItem,		0, 0 },
	{ "NextItem",			CBase_NextItem,			0, 0 },
	{ "FinishedItems",		CBase_FinishedItems,	0, 0 },
	{ "ExecuteCommand",     CChar_ExecuteCommand,   0, 0 },
	{ "WalkTo",				CChar_WalkTo,			1, 0 },
	{ "RunTo",				CChar_RunTo,			1, 0 },
	{ "DistanceTo",			CBase_DistanceTo,		1, 0 },
	{ "OpenLayer",			CChar_OpenLayer,		2, 0 },
	{ "BoltEffect",			CChar_BoltEffect,		0, 0 },
	{ "Gate",				CChar_Gate,				1, 0 },
	{ "Recall",				CChar_Recall,			1, 0 },
	{ "Mark",				CChar_Mark,				1, 0 },
	{ "SetRandomName",		CBase_SetRandomName,	1, 0 },
	{ "SetRandomColor",		CBase_SetRandomColor,	1, 0 },
	{ "SetSkillByName",		CChar_SetSkillByName,	2, 0 },
	{ "Kill",				CChar_Kill,				0, 0 },
	{ "Resurrect",			CChar_Resurrect,		0, 0 },
	{ "Dupe",				CChar_Dupe,				0, 0 },
	{ "Jail",				CChar_Jail,				0, 0 },
	{ "Release",			CChar_Release,			0, 0 },
	{ "GetTimer",			CMisc_GetTimer,			1, 0 },
	{ "SetTimer",			CMisc_SetTimer,			2, 0 },
	{ "SpellMoveEffect",	CChar_SpellMoveEffect,	2, 0 },
	{ "SpellStaticEffect",	CChar_SpellStaticEffect,1, 0 },
	{ "BreakConcentration",	CChar_BreakConcentration,0, 0 },
	{ "FindItemType",		CChar_FindItemType,		1, 0 },
	{ "FindItemSection",	CChar_FindItemSection,	1, 0 },
	{ "InitWanderArea",		CChar_InitWanderArea,	0, 0 },
	{ "CanSee",				CBase_CanSee,			1, 0 },
	{ "ReactOnDamage",		CChar_ReactOnDamage,	1, 0 },
	{ "Damage",				CChar_Damage,			1, 0 },
	{ "InitiateCombat",		CChar_InitiateCombat,	1, 0 },
	{ "InvalidateAttacker",		CChar_InvalidateAttacker,				0, 0 },
	{ "AddAggressorFlag",		CChar_AddAggressorFlag,					1, 0 },
	{ "RemoveAggressorFlag",	CChar_RemoveAggressorFlag,				1, 0 },
	{ "CheckAggressorFlag",		CChar_CheckAggressorFlag,					1, 0 },
	{ "UpdateAggressorFlagTimestamp",	CChar_UpdateAggressorFlagTimestamp,	1, 0 },
	{ "IsAggressor",			CChar_IsAggressor,						1, 0 },
	{ "ClearAggressorFlags",	CChar_ClearAggressorFlags,				0, 0 },
	{ "AddPermaGreyFlag",		CChar_AddPermaGreyFlag,					1, 0 },
	{ "RemovePermaGreyFlag",	CChar_RemovePermaGreyFlag,				1, 0 },
	{ "CheckPermaGreyFlag",		CChar_CheckPermaGreyFlag,					1, 0 },
	{ "UpdatePermaGreyFlagTimestamp",	CChar_UpdatePermaGreyFlagTimestamp,	1, 0 },
	{ "IsPermaGrey",			CChar_IsPermaGrey,						1, 0 },
	{ "ClearPermaGreyFlags",	CChar_ClearPermaGreyFlags,				0, 0 },
	{ "Heal",				CChar_Heal,				1, 0 },
	{ "Resist",				CBase_Resist,			1, 0 },
	{ "Defense",			CChar_Defense,			3, 0 },
	{ "AddScriptTrigger",	CBase_AddScriptTrigger,		1, 0 },
	{ "HasScriptTrigger",	CBase_HasScriptTrigger,		1, 0 },
	{ "RemoveScriptTrigger",CBase_RemoveScriptTrigger,	1, 0 },
	{ "AddFriend",			CChar_AddFriend,			1, 0 },
	{ "RemoveFriend",		CChar_RemoveFriend,			1, 0 },
	{ "GetFriendList",		CChar_GetFriendList,		0, 0 },
	{ "ClearFriendList",	CChar_ClearFriendList,		0, 0 },
	{ "GetPetList",			CChar_GetPetList,			0, 0 },
	{ "GetFollowerList",	CChar_GetFollowerList,		0, 0 },
	{ "AddFollower",		CChar_AddFollower,			1, 0 },
	{ "RemoveFollower",		CChar_RemoveFollower,		1, 0 },
	{ "HasBeenOwner",		CChar_HasBeenOwner,			1, 0 },
	{ "CalculateControlChance",	CChar_CalculateControlChance,	1, 0 },
	{ "GetTempEffect",		CBase_GetTempEffect,		1, 0 },
	{ "ReverseTempEffect",	CBase_ReverseEffect,		1, 0 },
	{ "CheckGuild",			CChar_CheckGuild,			1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CItem_Methods[] =
{
	{ "KillTimers",			CBase_KillTimers,			1, 0 },
	{ "GetJSTimer",			CBase_GetJSTimer,			2, 0 },
	{ "SetJSTimer",			CBase_SetJSTimer,			3, 0 },
	{ "KillJSTimer",		CBase_KillJSTimer,			2, 0 },
	{ "Delete",				CBase_Delete,				0, 0 },
	{ "TextMessage",		CBase_TextMessage,			1, 0 },
	{ "Teleport",			CBase_Teleport,				5, 0 },
	{ "StaticEffect",		CBase_StaticEffect,			4, 0 },
	{ "SetLocation",		CBase_Teleport,				5, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,			2, 0 },
	{ "HasSpell",			CMisc_HasSpell,				1, 0 },
	{ "RemoveSpell",		CMisc_RemoveSpell,			1, 0 },
	{ "GetTag",				CBase_GetTag,				1, 0 },
	{ "SetTag",				CBase_SetTag,				1, 0 },
	{ "GetTempTag",			CBase_GetTempTag,			1, 0 },
	{ "SetTempTag",			CBase_SetTempTag,			2, 0 },
	{ "GetNumTags",			CBase_GetNumTags,			0, 0 },
	{ "GetTagMap",			CBase_GetTagMap,			0, 0 },
	{ "GetTempTagMap",		CBase_GetTempTagMap,		0, 0 },
	{ "InRange",			CBase_InRange,				2, 0 },
	{ "StartTimer",			CBase_StartTimer,			2, 0 },
	{ "OpenPlank",			CItem_OpenPlank,			0, 0 },
	{ "IsMulti",			CItem_IsMulti,				0, 0 },
	{ "IsBoat",				CBase_IsBoat,				0, 0 },
	{ "IsInMulti",			CMulti_IsInMulti,			1, 0 },
	{ "IsOnBanList",		CMulti_IsOnBanList,			1, 0 },
	{ "IsOnFriendList",		CMulti_IsOnFriendList,		1, 0 },
	{ "IsOnGuestList",		CMulti_IsOnGuestList,		1, 0 },
	{ "IsOwner",			CMulti_IsOwner,				1, 0 },
	{ "IsOnOwnerList",		CMulti_IsOnOwnerList,		1, 0 },
	{ "AddToBanList",		CMulti_AddToBanList,		1, 0 },
	{ "AddToFriendList",	CMulti_AddToFriendList,		1, 0 },
	{ "AddToGuestList",		CMulti_AddToGuestList,		1, 0 },
	{ "AddToOwnerList",		CMulti_AddToOwnerList,		1, 0 },
	{ "RemoveFromBanList",		CMulti_RemoveFromBanList,		1, 0 },
	{ "RemoveFromFriendList",	CMulti_RemoveFromFriendList,	1, 0 },
	{ "RemoveFromGuestList",	CMulti_RemoveFromGuestList,		1, 0 },
	{ "RemoveFromOwnerList",	CMulti_RemoveFromOwnerList,		1, 0 },
	{ "ClearBanList",		CMulti_ClearBanList,		1, 0 },
	{ "ClearFriendList",	CMulti_ClearFriendList,		1, 0 },
	{ "ClearGuestList",		CMulti_ClearGuestList,		1, 0 },
	{ "ClearOwnerList",		CMulti_ClearOwnerList,		1, 0 },
	{ "GetSerial",			CBase_GetSerial,			1, 0 },
	{ "UpdateStats",		CBase_UpdateStats,			1, 0 },
	{ "SetCont",			CItem_SetCont,				1, 0 },
	{ "ApplySection",		CBase_ApplySection,			1, 0 },
	{ "Refresh",			CBase_Refresh,				0, 0 },
	{ "ApplyRank",			CItem_ApplyRank,			2, 0 },
	{ "IsOnFoodList",		CItem_IsOnFoodList,			1, 0 },
	{ "FirstItem",			CBase_FirstItem,			0, 0 },
	{ "NextItem",			CBase_NextItem,				0, 0 },
	{ "FinishedItems",		CBase_FinishedItems,		0, 0 },
	{ "DistanceTo",			CBase_DistanceTo,			1, 0 },
	{ "Glow",				CItem_Glow,					1, 0 },
	{ "UnGlow",				CItem_UnGlow,				1, 0 },
	{ "PlaceInPack",		CItem_PlaceInPack,			0, 0 },
	{ "Dupe",				CItem_Dupe,					1, 0 },
	{ "LockDown",			CItem_LockDown,				0, 0 },
	{ "Carve",				CItem_Carve,				1, 0 },
	{ "GetTileName",		CItem_GetTileName,			0, 0 },
	{ "GetMoreVar",			CItem_GetMoreVar,			2, 0 },
	{ "SetMoreVar",			CItem_SetMoreVar,			3, 0 },
	{ "Resist",				CBase_Resist,				1, 0 },
	{ "ResourceCount",		CBase_ResourceCount,		2, 0 },
	{ "UseResource",		CBase_UseResource,			3, 0 },
	{ "AddScriptTrigger",	CBase_AddScriptTrigger,		1, 0 },
	{ "HasScriptTrigger",	CBase_HasScriptTrigger,		1, 0 },
	{ "RemoveScriptTrigger",CBase_RemoveScriptTrigger,	1, 0 },
	{ "GetMultiCorner",		CMulti_GetMultiCorner,		1, 0 },
	{ "SecureContainer",	CMulti_SecureContainer,		1, 0 },
	{ "UnsecureContainer",	CMulti_UnsecureContainer,	1, 0 },
	{ "IsSecureContainer",	CMulti_IsSecureContainer,	1, 0 },
	{ "LockDownItem",		CMulti_LockDownItem,		1, 0 },
	{ "ReleaseItem",		CMulti_ReleaseItem,			1, 0 },
	{ "AddTrashCont",		CMulti_AddTrashCont,		1, 0 },
	{ "RemoveTrashCont",	CMulti_RemoveTrashCont,		1, 0 },

	{ "KillKeys",			CMulti_KillKeys,			1, 0 },

	{ "FirstChar",			CMulti_FirstChar,			1, 0 },
	{ "NextChar",			CMulti_NextChar,			1, 0 },
	{ "FinishedChars",		CMulti_FinishedChars,		1, 0 },

	//{ "SetMoreSerial",		CBase_SetMoreSerial,		1, 0 },
	{ "SetRandomName",		CBase_SetRandomName,		1, 0 },
	{ "SetRandomColor",		CBase_SetRandomColor,		1, 0 },
	{ "GetTempEffect",		CBase_GetTempEffect,		1, 0 },
	{ "ReverseTempEffect",	CBase_ReverseEffect,		1, 0 },
	{ nullptr,				nullptr,					0, 0 }
};

inline JSFunctionSpec CRegion_Methods[] =
{
	{ "AddScriptTrigger",		CRegion_AddScriptTrigger,		1, 0 },
	{ "RemoveScriptTrigger",	CRegion_RemoveScriptTrigger,	1, 0 },
	{ "GetOrePref",				CRegion_GetOrePref,				1, 0 },
	{ "GetOreChance",			CRegion_GetOreChance,			0, 0 },
	JS_FS_END
};

inline JSFunctionSpec CSocket_Methods[] =
{
	{ "SysMessage",			CMisc_SysMessage,	10, 0 },
	{ "Disconnect",			CSocket_Disconnect,	0, 0 },
	{ "SoundEffect",		CMisc_SoundEffect,	2, 0 },
	{ "CustomTarget",		CMisc_CustomTarget, 1, 0 },
	{ "PopUpTarget",		CMisc_PopUpTarget,	1, 0 },
	{ "GetByte",			CSocket_GetByte,	1, 0 },
	{ "GetWord",			CSocket_GetWord,	1, 0 },
	{ "GetDWord",			CSocket_GetDWord,	1, 0 },
	{ "GetSByte",			CSocket_GetSByte,	1, 0 },
	{ "GetSWord",			CSocket_GetSWord,	1, 0 },
	{ "GetSDWord",			CSocket_GetSDWord,	1, 0 },
	{ "GetString",			CSocket_GetString,	1, 0 },
	{ "SetByte",			CSocket_SetByte,	2, 0 },
	{ "SetWord",			CSocket_SetWord,	2, 0 },
	{ "SetDWord",			CSocket_SetDWord,	2, 0 },
	{ "SetString",			CSocket_SetString,	2, 0 },
	{ "ReadBytes",			CSocket_ReadBytes,	1, 0 },
	{ "OpenContainer",		CSocket_OpenContainer,	1, 0 },
	{ "OpenGump",			CSocket_OpenGump,	1, 0 },
	{ "CloseGump",			CSocket_CloseGump,	2, 0 },
	{ "OpenURL",			CSocket_OpenURL,    1, 0 },
	{ "BuyFrom",			CMisc_BuyFrom,		1, 0 },
	{ "SellTo",				CMisc_SellTo,		1, 0 },
	{ "WhoList",			CSocket_WhoList,	0, 0 },
	{ "Music",				CSocket_Music,		1, 0 },
	{ "GetTimer",			CMisc_GetTimer,		1, 0 },
	{ "SetTimer",			CMisc_SetTimer,		2, 0 },
	{ "SendAddMenu",		CSocket_SendAddMenu,1, 0 },
	{ "Page",				CSocket_Page,		1, 0 },
	{ "MakeMenu",			CMisc_MakeMenu,		2, 0 },
	{ "Send",				CSocket_Send,		1, 0 },
	{ "CanSee",				CBase_CanSee,		1, 0 },
	{ "DisplayDamage",		CSocket_DisplayDamage, 2, 0 },
	{ "FirstTriggerWord",	CSocket_FirstTriggerWord, 0, 0 },
	{ "NextTriggerWord",	CSocket_NextTriggerWord, 0, 0 },
	{ "FinishedTriggerWords",	CSocket_FinishedTriggerWords, 0, 0 },
	{ nullptr,				nullptr,			0, 0 }
};

inline JSFunctionSpec CGuild_Methods[] =
{
	{ "AcceptRecruit",		CGuild_AcceptRecruit,		1, 0 },
	{ "IsAtPeace",			CGuild_IsAtPeace,			0, 0 },
	{ "NewMember",			CGuild_NewMember,			1, 0 },
	{ "NewRecruit",			CGuild_NewRecruit,			1, 0 },
	{ "RemoveRecruit",		CGuild_RemoveRecruit,		1, 0 },
	{ "RemoveMember",		CGuild_RemoveMember,		1, 0 },
	{ "RecruitToMember",	CGuild_RecruitToMember,		1, 0 },
	{ "GuildIsAtWar",		CGuild_GuildIsAtWar,		1, 0 },
	{ "GuildIsAlly",		CGuild_GuildIsAlly,			1, 0 },
	{ "GuildIsNeutral",		CGuild_GuildIsNeutral,		1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CRace_Methods[] =
{
	{ "CanWearArmour",		CRace_CanWearArmour,		1, 0 },
	{ "IsValidHairColour",	CRace_IsValidHairColour,	1, 0 },
	{ "IsValidSkinColour",	CRace_IsValidSkinColour,	1, 0 },
	{ "IsValidBeardColour",	CRace_IsValidBeardColour,	1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CGumpData_Methods[] =
{
	{ "Free",		CGumpData_Free,			0, 0 },
	{ "getButton",	CGumpData_GetButton,	1, 0 },
	{ "getID",		CGumpData_GetId,		1, 0 },
	{ "getEdit",	CGumpData_GetEdit,		1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CFile_Methods[] =
{
	{ "Free",		CFile_Free,				0, 0 },
	{ "Open",		CFile_Open,				2, 0 },
	{ "Close",		CFile_Close,			0, 0 },
	{ "Write",		CFile_Write,			1, 0 },
	{ "Read",		CFile_Read,				1, 0 },
	{ "ReadUntil",	CFile_ReadUntil,		1, 0 },
	{ "EOF",		CFile_EOF,				0, 0 },
	{ "Length",		CFile_Length,			0, 0 },
	{ "Pos",		CFile_Pos,				1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CAccount_Methods[] =
{
	{ "AddAccount",		CAccount_AddAccount,		4, 0 },
	{ "DelAccount",		CAccount_DelAccount,		1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CConsole_Methods[] =
{
	{ "Print",				CConsole_Print,				1, 0 },
	{ "Log",				CConsole_Log,				1, 0 },
	{ "Error",				CConsole_Error,				1, 0 },
	{ "Warning",			CConsole_Warning,			1, 0 },
	{ "PrintSectionBegin",	CConsole_PrintSectionBegin,	0, 0 },
	{ "TurnYellow",			CConsole_TurnYellow,		0, 0 },
	{ "TurnRed",			CConsole_TurnRed,			0, 0 },
	{ "TurnGreen",			CConsole_TurnGreen,			0, 0 },
	{ "TurnBlue",			CConsole_TurnBlue,			0, 0 },
	{ "TurnNormal",			CConsole_TurnNormal,		0, 0 },
	{ "TurnBrightWhite",	CConsole_TurnBrightWhite,	0, 0 },
	{ "PrintDone",			CConsole_PrintDone,			0, 0 },
	{ "PrintFailed",		CConsole_PrintFailed,		0, 0 },
	{ "PrintPassed",		CConsole_PrintPassed,		0, 0 },
	{ "ClearScreen",		CConsole_ClearScreen,		0, 0 },
	{ "PrintBasedOnVal",	CConsole_PrintBasedOnVal,	1, 0 },
	{ "MoveTo",				CConsole_MoveTo,			2, 0 },
	{ "PrintSpecial",		CConsole_PrintSpecial,		2, 0 },
	{ "BeginShutdown",		CConsole_BeginShutdown,		0, 0 },
	{ "Reload",				CConsole_Reload,			1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CPacket_Methods[] =
{
	{ "Free",			CPacket_Free,				0, 0 },
	{ "WriteByte",		CPacket_WriteByte,			2, 0 },
	{ "WriteShort",		CPacket_WriteShort,			2, 0 },
	{ "WriteLong",		CPacket_WriteLong,			2, 0 },
	{ "WriteString",	CPacket_WriteString,		3, 0 },
	{ "ReserveSize",	CPacket_ReserveSize,		1, 0 },
	JS_FS_END
};

inline JSFunctionSpec CParty_Methods[] =
{
	{ "GetMember",		CParty_GetMember,			1, 0 },
	{ "Add",			CParty_Add,					1, 0 },
	{ "Remove",			CParty_Remove,				1, 0 },
	JS_FS_END
};

#endif
