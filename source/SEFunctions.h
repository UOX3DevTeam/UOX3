
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

typedef JSBool ( SEngineFunc )( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// 90 functions so far
// *	== prototype
// **	== active function

// Character related functions (9)

SEngineFunc SE_SpawnNPC;			// Create NPC								*

// Effect related functions (3)

SEngineFunc SE_DoMovingEffect;		// Does moving effect						***
SEngineFunc SE_DoTempEffect;		// Does a temp effect						***
SEngineFunc SE_DoStaticEffect;		// Does a static effect at location			***

// Item related functions (2)

SEngineFunc SE_CreateBlankItem;		// Create an item							***
SEngineFunc SE_CreateDFNItem;		// Create an item							***
SEngineFunc SE_CreateHouse;			// Create a house from houses.dfn			***
SEngineFunc SE_CreateBaseMulti;		// Create a raw multi based on multi ID		***

// Speech related functions (7)
SEngineFunc SE_BroadcastMessage;	//											***

// Random functions(5)
SEngineFunc SE_RandomNumber;		// Calculates a random number				***
SEngineFunc SE_CalcCharFromSer;		//											***
SEngineFunc SE_CalcItemFromSer;		//											***
SEngineFunc SE_CalcMultiFromSer;	//											***

SEngineFunc SE_CheckInstaLog;

SEngineFunc SE_MakeItem;			//											***

SEngineFunc SE_CommandLevelReq;		//											*
SEngineFunc SE_CommandExists;		//											*
SEngineFunc SE_FirstCommand;		//											*
SEngineFunc SE_NextCommand;			//											*
SEngineFunc SE_FinishedCommandList;	//											*

SEngineFunc SE_RegisterCommand;		//											***
SEngineFunc SE_EnableCommand;		//											***
SEngineFunc SE_DisableCommand;		//											***

SEngineFunc SE_RegisterKey;			//											***
SEngineFunc SE_EnableKey;			//											***
SEngineFunc SE_DisableKey;			//											***

SEngineFunc SE_RegisterConsoleFunc;	//											***
SEngineFunc SE_EnableConsoleFunc;	//											***
SEngineFunc SE_DisableConsoleFunc;	//											***

SEngineFunc SE_RegisterSpell;		//											***
SEngineFunc SE_EnableSpell;			//											***
SEngineFunc SE_DisableSpell;		//											***

SEngineFunc SE_RegisterSkill;
SEngineFunc SE_RegisterPacket;

SEngineFunc SE_GetHour;					//										***
SEngineFunc SE_GetMinute;				//										***
SEngineFunc SE_GetDay;					//										***
SEngineFunc SE_SecondsPerUOMinute;		//										***

SEngineFunc SE_GetCurrentClock;			//										***
SEngineFunc SE_GetStartTime;			//										***
SEngineFunc SE_GetMurderThreshold;		//										***
SEngineFunc SE_RollDice;				//										***
SEngineFunc SE_RaceCompareByRace;		//										***
SEngineFunc SE_GetRandomSOSArea;		//										***

SEngineFunc SE_FindMulti;				//										***
SEngineFunc SE_GetItem;					//										***
SEngineFunc SE_FindItem;				//										***

SEngineFunc SE_CompareGuildByGuild;		//										**

SEngineFunc SE_PossessTown;				//										***

SEngineFunc SE_IsRaceWeakToWeather;		//										***
SEngineFunc SE_GetRaceSkillAdjustment;	//										***

SEngineFunc SE_UseItem;					//										***
SEngineFunc SE_TriggerTrap;				//										***

SEngineFunc SE_TriggerEvent;			//										*
SEngineFunc SE_DoesEventExist;			//										*

SEngineFunc SE_GetPackOwner;			//										***
SEngineFunc SE_FindRootContainer;		//										***
SEngineFunc SE_CalcTargetedItem;		//										***
SEngineFunc SE_CalcTargetedChar;		//										***
SEngineFunc SE_GetTileIdAtMapCoord;		//										***

SEngineFunc SE_StringToNum;				// Marked for Deletion
SEngineFunc SE_NumToString;				// Marked for Deletion
SEngineFunc SE_NumToHexString;			// Marked for Deletion
SEngineFunc SE_EraStringToNum;

SEngineFunc SE_GetRaceCount;
SEngineFunc SE_AreaCharacterFunction;
SEngineFunc SE_AreaItemFunction;
SEngineFunc SE_Reload;

SEngineFunc SE_GetDictionaryEntry;
SEngineFunc SE_Yell;
SEngineFunc SE_SendStaticStats;
SEngineFunc SE_GetTileHeight;
SEngineFunc SE_IterateOver;
SEngineFunc SE_IterateOverSpawnRegions;
SEngineFunc SE_WorldBrightLevel;
SEngineFunc SE_WorldDarkLevel;
SEngineFunc SE_WorldDungeonLevel;
SEngineFunc SE_GetSpawnRegionFacetStatus;
SEngineFunc SE_SetSpawnRegionFacetStatus;
SEngineFunc SE_GetMoongateFacetStatus;
SEngineFunc SE_SetMoongateFacetStatus;
SEngineFunc SE_GetSocketFromIndex;
SEngineFunc SE_ResourceArea;
SEngineFunc SE_ResourceAmount;
SEngineFunc SE_ResourceTime;
SEngineFunc SE_ResourceRegion;
SEngineFunc SE_Moon;

SEngineFunc SE_GetTownRegion;
SEngineFunc SE_GetTownRegionFromXY;
SEngineFunc SE_GetSpawnRegion;
SEngineFunc SE_GetSpawnRegionCount;

SEngineFunc SE_ReloadJSFile;
SEngineFunc SE_StaticInRange;
SEngineFunc SE_StaticAt;
SEngineFunc SE_GetMapElevation;
SEngineFunc SE_IsInBuilding;
SEngineFunc SE_CheckStaticFlag;
SEngineFunc SE_CheckDynamicFlag;
SEngineFunc SE_CheckTileFlag;
SEngineFunc SE_DoesStaticBlock;
SEngineFunc SE_DoesDynamicBlock;
SEngineFunc SE_DoesMapBlock;
SEngineFunc SE_DoesCharacterBlock;
SEngineFunc SE_DistanceBetween;

SEngineFunc SE_ValidateObject;
SEngineFunc SE_ApplyDamageBonuses;
SEngineFunc SE_ApplyDefenseModifiers;
SEngineFunc SE_WillResultInCriminal;

SEngineFunc SE_CreateParty;

// Server settings
SEngineFunc SE_GetClientFeature;
SEngineFunc SE_GetServerFeature;
SEngineFunc SE_GetServerSetting;

SEngineFunc SE_DeleteFile;

// Account 
SEngineFunc SE_GetAccountCount;			//										***
SEngineFunc SE_GetPlayerCount;			//										***
SEngineFunc SE_GetItemCount;			//										***
SEngineFunc SE_GetMultiCount;			//										***
SEngineFunc SE_GetCharacterCount;		//										***
SEngineFunc SE_GetServerVersionString;	//										***

// Server constants
SEngineFunc SE_BASEITEMSERIAL;			//										***
SEngineFunc SE_INVALIDSERIAL;			//										***
SEngineFunc SE_INVALIDID;				//										***
SEngineFunc SE_INVALIDCOLOUR;			//										***


#endif
