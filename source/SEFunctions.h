
#ifndef __SEFuncs_h__
#define __SEFuncs_h__
// Function prototypes exposed to the Script Engine

// Version History
// 1.0		12th September, 2000
//			Initial implementation, not many functions added
// 1.1		14th September, 2000
//			Declaration of 90 API functions and implementation of around 20 of them
// 1.2		18th September, 2000
//			Addition of more API functions and declarations of them
// 1.3		28th October, 2001
//			Addition of File i/o functions.
// 1.4 		3rd  November, 2001
//			Addition of Tag functions.

// 90 functions so far
// *	== prototype
// **	== active function

// Character related functions (9)

JSBool SE_SpawnNPC( JSContext *cx, uintN argc, jsval *vp );			// Create NPC								*

// Effect related functions (3)

JSBool SE_DoMovingEffect( JSContext *cx, uintN argc, jsval *vp );		// Does moving effect						***
JSBool SE_DoTempEffect( JSContext *cx, uintN argc, jsval *vp );		// Does a temp effect						***
JSBool SE_DoStaticEffect( JSContext *cx, uintN argc, jsval *vp );		// Does a static effect at location			***

// Item related functions (2)

JSBool SE_CreateBlankItem( JSContext *cx, uintN argc, jsval *vp );		// Create an item							***
JSBool SE_CreateDFNItem( JSContext *cx, uintN argc, jsval *vp );		// Create an item							***
JSBool SE_CreateHouse( JSContext *cx, uintN argc, jsval *vp );			// Create a house from houses.dfn			***
JSBool SE_CreateBaseMulti( JSContext *cx, uintN argc, jsval *vp );		// Create a raw multi based on multi ID		***

// Speech related functions (7)
JSBool SE_BroadcastMessage( JSContext *cx, uintN argc, jsval *vp );	//											***

// Random functions(5)
JSBool SE_RandomNumber( JSContext *cx, uintN argc, jsval *vp );		// Calculates a random number				***
JSBool SE_CalcCharFromSer( JSContext *cx, uintN argc, jsval *vp );		//											***
JSBool SE_CalcItemFromSer( JSContext *cx, uintN argc, jsval *vp );		//											***
JSBool SE_CalcMultiFromSer( JSContext *cx, uintN argc, jsval *vp );	//											***

JSBool SE_CheckTimeSinceLastCombat( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_CheckInstaLog( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_MakeItem( JSContext *cx, uintN argc, jsval *vp );			//											***

JSBool SE_CommandLevelReq( JSContext *cx, uintN argc, jsval *vp );		//											*
JSBool SE_CommandExists( JSContext *cx, uintN argc, jsval *vp );		//											*
JSBool SE_FirstCommand( JSContext *cx, uintN argc, jsval *vp );		//											*
JSBool SE_NextCommand( JSContext *cx, uintN argc, jsval *vp );			//											*
JSBool SE_FinishedCommandList( JSContext *cx, uintN argc, jsval *vp );	//											*

JSBool SE_RegisterCommand( JSContext *cx, uintN argc, jsval *vp );		//											***
JSBool SE_EnableCommand( JSContext *cx, uintN argc, jsval *vp );		//											***
JSBool SE_DisableCommand( JSContext *cx, uintN argc, jsval *vp );		//											***

JSBool SE_RegisterKey( JSContext *cx, uintN argc, jsval *vp );			//											***
JSBool SE_EnableKey( JSContext *cx, uintN argc, jsval *vp );			//											***
JSBool SE_DisableKey( JSContext *cx, uintN argc, jsval *vp );			//											***

JSBool SE_RegisterConsoleFunc( JSContext *cx, uintN argc, jsval *vp );	//											***
JSBool SE_EnableConsoleFunc( JSContext *cx, uintN argc, jsval *vp );	//											***
JSBool SE_DisableConsoleFunc( JSContext *cx, uintN argc, jsval *vp );	//											***

JSBool SE_RegisterSpell( JSContext *cx, uintN argc, jsval *vp );		//											***
JSBool SE_EnableSpell( JSContext *cx, uintN argc, jsval *vp );			//											***
JSBool SE_DisableSpell( JSContext *cx, uintN argc, jsval *vp );		//											***

JSBool SE_RegisterSkill( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_RegisterPacket( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_GetHour( JSContext *cx, uintN argc, jsval *vp );					//										***
JSBool SE_GetMinute( JSContext *cx, uintN argc, jsval *vp );				//										***
JSBool SE_GetDay( JSContext *cx, uintN argc, jsval *vp );					//										***
JSBool SE_SecondsPerUOMinute( JSContext *cx, uintN argc, jsval *vp );		//										***

JSBool SE_GetCurrentClock( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_GetStartTime( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_GetMurderThreshold( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_RollDice( JSContext *cx, uintN argc, jsval *vp );				//										***
JSBool SE_RaceCompareByRace( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_GetRandomSOSArea( JSContext *cx, uintN argc, jsval *vp );		//										***

JSBool SE_FindMulti( JSContext *cx, uintN argc, jsval *vp );				//										***
JSBool SE_GetItem( JSContext *cx, uintN argc, jsval *vp );					//										***
JSBool SE_FindItem( JSContext *cx, uintN argc, jsval *vp );				//										***

JSBool SE_CompareGuildByGuild( JSContext *cx, uintN argc, jsval *vp );		//										**
JSBool SE_CreateNewGuild( JSContext *cx, uintN argc, jsval *vp );		//										**
JSBool SE_GuildIsAtWar( JSContext *cx, uintN argc, jsval *vp );		//										**
JSBool SE_GuildIsAlly( JSContext *cx, uintN argc, jsval *vp );		//										**
JSBool SE_GuildIsNeutral( JSContext *cx, uintN argc, jsval *vp );		//										**

JSBool SE_PossessTown( JSContext *cx, uintN argc, jsval *vp );				//										***

JSBool SE_IsRaceWeakToWeather( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_GetRaceSkillAdjustment( JSContext *cx, uintN argc, jsval *vp );	//										***

JSBool SE_UseItem( JSContext *cx, uintN argc, jsval *vp );					//										***
JSBool SE_TriggerTrap( JSContext *cx, uintN argc, jsval *vp );				//										***

JSBool SE_TriggerEvent( JSContext *cx, uintN argc, jsval *vp );			//										*
JSBool SE_DoesEventExist( JSContext *cx, uintN argc, jsval *vp );			//										*

JSBool SE_GetPackOwner( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_FindRootContainer( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_CalcTargetedItem( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_CalcTargetedChar( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_GetTileIdAtMapCoord( JSContext *cx, uintN argc, jsval *vp );		//										***

JSBool SE_StringToNum( JSContext *cx, uintN argc, jsval *vp );				// Marked for Deletion
JSBool SE_NumToString( JSContext *cx, uintN argc, jsval *vp );				// Marked for Deletion
JSBool SE_NumToHexString( JSContext *cx, uintN argc, jsval *vp );			// Marked for Deletion
JSBool SE_EraStringToNum( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_GetRaceCount( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_AreaCharacterFunction( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_AreaItemFunction( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_Reload( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_GetDictionaryEntry( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_Yell( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_SendStaticStats( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetTileHeight( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_IterateOver( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_IterateOverSpawnRegions( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_WorldBrightLevel( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_WorldDarkLevel( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_WorldDungeonLevel( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetSpawnRegionFacetStatus( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_SetSpawnRegionFacetStatus( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetMoongateFacetStatus( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_SetMoongateFacetStatus( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetSocketFromIndex( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_ResourceArea( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_ResourceAmount( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_ResourceTime( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_ResourceRegion( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_Moon( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_GetTownRegion( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetTownRegionFromXY( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetSpawnRegion( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetSpawnRegionCount( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_ReloadJSFile( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_StaticInRange( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_StaticAt( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetMapElevation( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_IsInBuilding( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_CheckStaticFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_CheckDynamicFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_CheckTileFlag( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_DoesStaticBlock( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_DoesDynamicBlock( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_DoesMapBlock( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_DoesCharacterBlock( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_DistanceBetween( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_ValidateObject( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_ApplyDamageBonuses( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_ApplyDefenseModifiers( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_WillResultInCriminal( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_CreateParty( JSContext *cx, uintN argc, jsval *vp );

// Server settings
JSBool SE_GetClientFeature( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetServerFeature( JSContext *cx, uintN argc, jsval *vp );
JSBool SE_GetServerSetting( JSContext *cx, uintN argc, jsval *vp );

JSBool SE_DeleteFile( JSContext *cx, uintN argc, jsval *vp );

// Account 
JSBool SE_GetAccountCount( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_GetPlayerCount( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_GetItemCount( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_GetMultiCount( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_GetCharacterCount( JSContext *cx, uintN argc, jsval *vp );		//										***
JSBool SE_GetServerVersionString( JSContext *cx, uintN argc, jsval *vp );	//										***

// Server constants
JSBool SE_BASEITEMSERIAL( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_INVALIDSERIAL( JSContext *cx, uintN argc, jsval *vp );			//										***
JSBool SE_INVALIDID( JSContext *cx, uintN argc, jsval *vp );				//										***
JSBool SE_INVALIDCOLOUR( JSContext *cx, uintN argc, jsval *vp );			//										***

std::string JS_GetStringBytes( JSContext* cx, jsval val );

#endif
