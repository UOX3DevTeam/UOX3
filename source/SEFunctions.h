
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

JSNative SE_SpawnNPC;			// Create NPC								*

// Effect related functions (3)

JSNative SE_DoMovingEffect;		// Does moving effect						***
JSNative SE_DoTempEffect;		// Does a temp effect						***
JSNative SE_DoStaticEffect;		// Does a static effect at location			***

// Item related functions (2)

JSNative SE_CreateBlankItem;		// Create an item							***
JSNative SE_CreateDFNItem;		// Create an item							***
JSNative SE_CreateHouse;			// Create a house from houses.dfn			***
JSNative SE_CreateBaseMulti;		// Create a raw multi based on multi ID		***

// Speech related functions (7)
JSNative SE_BroadcastMessage;	//											***

// Random functions(5)
JSNative SE_RandomNumber;		// Calculates a random number				***
JSNative SE_CalcCharFromSer;		//											***
JSNative SE_CalcItemFromSer;		//											***
JSNative SE_CalcMultiFromSer;	//											***

JSNative SE_CheckTimeSinceLastCombat;
JSNative SE_CheckInstaLog;

JSNative SE_MakeItem;			//											***

JSNative SE_CommandLevelReq;		//											*
JSNative SE_CommandExists;		//											*
JSNative SE_FirstCommand;		//											*
JSNative SE_NextCommand;			//											*
JSNative SE_FinishedCommandList;	//											*

JSNative SE_RegisterCommand;		//											***
JSNative SE_EnableCommand;		//											***
JSNative SE_DisableCommand;		//											***

JSNative SE_RegisterKey;			//											***
JSNative SE_EnableKey;			//											***
JSNative SE_DisableKey;			//											***

JSNative SE_RegisterConsoleFunc;	//											***
JSNative SE_EnableConsoleFunc;	//											***
JSNative SE_DisableConsoleFunc;	//											***

JSNative SE_RegisterSpell;		//											***
JSNative SE_EnableSpell;			//											***
JSNative SE_DisableSpell;		//											***

JSNative SE_RegisterSkill;
JSNative SE_RegisterPacket;

JSNative SE_GetHour;					//										***
JSNative SE_GetMinute;				//										***
JSNative SE_GetDay;					//										***
JSNative SE_SecondsPerUOMinute;		//										***

JSNative SE_GetCurrentClock;			//										***
JSNative SE_GetStartTime;			//										***
JSNative SE_GetMurderThreshold;		//										***
JSNative SE_RollDice;				//										***
JSNative SE_RaceCompareByRace;		//										***
JSNative SE_GetRandomSOSArea;		//										***

JSNative SE_FindMulti;				//										***
JSNative SE_GetItem;					//										***
JSNative SE_FindItem;				//										***

JSNative SE_CompareGuildByGuild;		//										**

JSNative SE_PossessTown;				//										***

JSNative SE_IsRaceWeakToWeather;		//										***
JSNative SE_GetRaceSkillAdjustment;	//										***

JSNative SE_UseItem;					//										***
JSNative SE_TriggerTrap;				//										***

JSNative SE_TriggerEvent;			//										*
JSNative SE_DoesEventExist;			//										*

JSNative SE_GetPackOwner;			//										***
JSNative SE_FindRootContainer;		//										***
JSNative SE_CalcTargetedItem;		//										***
JSNative SE_CalcTargetedChar;		//										***
JSNative SE_GetTileIdAtMapCoord;		//										***

JSNative SE_StringToNum;				// Marked for Deletion
JSNative SE_NumToString;				// Marked for Deletion
JSNative SE_NumToHexString;			// Marked for Deletion
JSNative SE_EraStringToNum;

JSNative SE_GetRaceCount;
JSNative SE_AreaCharacterFunction;
JSNative SE_AreaItemFunction;
JSNative SE_Reload;

JSNative SE_GetDictionaryEntry;
JSNative SE_Yell;
JSNative SE_SendStaticStats;
JSNative SE_GetTileHeight;
JSNative SE_IterateOver;
JSNative SE_IterateOverSpawnRegions;
JSNative SE_WorldBrightLevel;
JSNative SE_WorldDarkLevel;
JSNative SE_WorldDungeonLevel;
JSNative SE_GetSpawnRegionFacetStatus;
JSNative SE_SetSpawnRegionFacetStatus;
JSNative SE_GetMoongateFacetStatus;
JSNative SE_SetMoongateFacetStatus;
JSNative SE_GetSocketFromIndex;
JSNative SE_ResourceArea;
JSNative SE_ResourceAmount;
JSNative SE_ResourceTime;
JSNative SE_ResourceRegion;
JSNative SE_Moon;

JSNative SE_GetTownRegion;
JSNative SE_GetTownRegionFromXY;
JSNative SE_GetSpawnRegion;
JSNative SE_GetSpawnRegionCount;

JSNative SE_ReloadJSFile;
JSNative SE_StaticInRange;
JSNative SE_StaticAt;
JSNative SE_GetMapElevation;
JSNative SE_IsInBuilding;
JSNative SE_CheckStaticFlag;
JSNative SE_CheckDynamicFlag;
JSNative SE_CheckTileFlag;
JSNative SE_DoesStaticBlock;
JSNative SE_DoesDynamicBlock;
JSNative SE_DoesMapBlock;
JSNative SE_DoesCharacterBlock;
JSNative SE_DistanceBetween;

JSNative SE_ValidateObject;
JSNative SE_ApplyDamageBonuses;
JSNative SE_ApplyDefenseModifiers;
JSNative SE_WillResultInCriminal;

JSNative SE_CreateParty;

// Server settings
JSNative SE_GetClientFeature;
JSNative SE_GetServerFeature;
JSNative SE_GetServerSetting;

JSNative SE_DeleteFile;

// Account 
JSNative SE_GetAccountCount;			//										***
JSNative SE_GetPlayerCount;			//										***
JSNative SE_GetItemCount;			//										***
JSNative SE_GetMultiCount;			//										***
JSNative SE_GetCharacterCount;		//										***
JSNative SE_GetServerVersionString;	//										***

// Server constants
JSNative SE_BASEITEMSERIAL;			//										***
JSNative SE_INVALIDSERIAL;			//										***
JSNative SE_INVALIDID;				//										***
JSNative SE_INVALIDCOLOUR;			//										***


#endif
