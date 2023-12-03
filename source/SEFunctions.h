
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

bool SE_SpawnNPC(JSContext* cx, unsigned argc, JS::Value* vp);			// Create NPC								*

// Effect related functions (3)

bool SE_DoMovingEffect(JSContext* cx, unsigned argc, JS::Value* vp);		// Does moving effect						***
bool SE_DoTempEffect(JSContext* cx, unsigned argc, JS::Value* vp);		// Does a temp effect						***
bool SE_DoStaticEffect(JSContext* cx, unsigned argc, JS::Value* vp);		// Does a static effect at location			***

// Item related functions (2)

bool SE_CreateBlankItem(JSContext* cx, unsigned argc, JS::Value* vp);		// Create an item							***
bool SE_CreateDFNItem(JSContext* cx, unsigned argc, JS::Value* vp);		// Create an item							***
bool SE_CreateHouse(JSContext* cx, unsigned argc, JS::Value* vp);			// Create a house from houses.dfn			***
bool SE_CreateBaseMulti(JSContext* cx, unsigned argc, JS::Value* vp);		// Create a raw multi based on multi ID		***

// Speech related functions (7)
bool SE_BroadcastMessage(JSContext* cx, unsigned argc, JS::Value* vp);	//											***

// Random functions(5)
bool SE_RandomNumber(JSContext* cx, unsigned argc, JS::Value* vp);		// Calculates a random number				***
bool SE_CalcCharFromSer(JSContext* cx, unsigned argc, JS::Value* vp);		//											***
bool SE_CalcItemFromSer(JSContext* cx, unsigned argc, JS::Value* vp);		//											***
bool SE_CalcMultiFromSer(JSContext* cx, unsigned argc, JS::Value* vp);	//											***

bool SE_MakeItem(JSContext* cx, unsigned argc, JS::Value* vp);			//											***

bool SE_CommandLevelReq(JSContext* cx, unsigned argc, JS::Value* vp);		//											*
bool SE_CommandExists(JSContext* cx, unsigned argc, JS::Value* vp);		//											*
bool SE_FirstCommand(JSContext* cx, unsigned argc, JS::Value* vp);		//											*
bool SE_NextCommand(JSContext* cx, unsigned argc, JS::Value* vp);			//											*
bool SE_FinishedCommandList(JSContext* cx, unsigned argc, JS::Value* vp);	//											*

bool SE_RegisterCommand(JSContext* cx, unsigned argc, JS::Value* vp);		//											***
bool SE_EnableCommand(JSContext* cx, unsigned argc, JS::Value* vp);		//											***
bool SE_DisableCommand(JSContext* cx, unsigned argc, JS::Value* vp);		//											***

bool SE_RegisterKey(JSContext* cx, unsigned argc, JS::Value* vp);			//											***
bool SE_EnableKey(JSContext* cx, unsigned argc, JS::Value* vp);			//											***
bool SE_DisableKey(JSContext* cx, unsigned argc, JS::Value* vp);			//											***

bool SE_RegisterConsoleFunc(JSContext* cx, unsigned argc, JS::Value* vp);	//											***
bool SE_EnableConsoleFunc(JSContext* cx, unsigned argc, JS::Value* vp);	//											***
bool SE_DisableConsoleFunc(JSContext* cx, unsigned argc, JS::Value* vp);	//											***

bool SE_RegisterSpell(JSContext* cx, unsigned argc, JS::Value* vp);		//											***
bool SE_EnableSpell(JSContext* cx, unsigned argc, JS::Value* vp);			//											***
bool SE_DisableSpell(JSContext* cx, unsigned argc, JS::Value* vp);		//											***

bool SE_RegisterSkill(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_RegisterPacket(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_GetHour(JSContext* cx, unsigned argc, JS::Value* vp);					//										***
bool SE_GetMinute(JSContext* cx, unsigned argc, JS::Value* vp);				//										***
bool SE_GetDay(JSContext* cx, unsigned argc, JS::Value* vp);					//										***
bool SE_SecondsPerUOMinute(JSContext* cx, unsigned argc, JS::Value* vp);		//										***

bool SE_GetCurrentClock(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_GetStartTime(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_GetMurderThreshold(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_RollDice(JSContext* cx, unsigned argc, JS::Value* vp);				//										***
bool SE_RaceCompareByRace(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_GetRandomSOSArea(JSContext* cx, unsigned argc, JS::Value* vp);		//										***

bool SE_FindMulti(JSContext* cx, unsigned argc, JS::Value* vp);				//										***
bool SE_GetItem(JSContext* cx, unsigned argc, JS::Value* vp);					//										***
bool SE_FindItem(JSContext* cx, unsigned argc, JS::Value* vp);				//										***

bool SE_CompareGuildByGuild(JSContext* cx, unsigned argc, JS::Value* vp);		//										**

bool SE_PossessTown(JSContext* cx, unsigned argc, JS::Value* vp);				//										***

bool SE_IsRaceWeakToWeather(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_GetRaceSkillAdjustment(JSContext* cx, unsigned argc, JS::Value* vp);	//										***

bool SE_UseItem(JSContext* cx, unsigned argc, JS::Value* vp);					//										***
bool SE_TriggerTrap(JSContext* cx, unsigned argc, JS::Value* vp);				//										***

bool SE_TriggerEvent(JSContext* cx, unsigned argc, JS::Value* vp);			//										*
bool SE_DoesEventExist(JSContext* cx, unsigned argc, JS::Value* vp);			//										*

bool SE_GetPackOwner(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_FindRootContainer(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_CalcTargetedItem(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_CalcTargetedChar(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_GetTileIdAtMapCoord(JSContext* cx, unsigned argc, JS::Value* vp);		//										***

bool SE_StringToNum(JSContext* cx, unsigned argc, JS::Value* vp);				// Marked for Deletion
bool SE_NumToString(JSContext* cx, unsigned argc, JS::Value* vp);				// Marked for Deletion
bool SE_NumToHexString(JSContext* cx, unsigned argc, JS::Value* vp);			// Marked for Deletion
bool SE_EraStringToNum(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_GetRaceCount(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_AreaCharacterFunction(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_AreaItemFunction(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_Reload(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_GetDictionaryEntry(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_Yell(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_SendStaticStats(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetTileHeight(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_IterateOver(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_IterateOverSpawnRegions(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_WorldBrightLevel(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_WorldDarkLevel(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_WorldDungeonLevel(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetSpawnRegionFacetStatus(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_SetSpawnRegionFacetStatus(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetMoongateFacetStatus(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_SetMoongateFacetStatus(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetSocketFromIndex(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_ResourceArea(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_ResourceAmount(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_ResourceTime(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_ResourceRegion(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_Moon(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_GetTownRegion(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetSpawnRegion(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetSpawnRegionCount(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_ReloadJSFile(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_StaticInRange(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_StaticAt(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetMapElevation(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_IsInBuilding(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_CheckStaticFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_CheckDynamicFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_CheckTileFlag(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_DoesStaticBlock(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_DoesDynamicBlock(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_DoesMapBlock(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_DoesCharacterBlock(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_DistanceBetween(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_ValidateObject(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_ApplyDamageBonuses(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_ApplyDefenseModifiers(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_WillResultInCriminal(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_CreateParty(JSContext* cx, unsigned argc, JS::Value* vp);

// Server settings
bool SE_GetClientFeature(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetServerFeature(JSContext* cx, unsigned argc, JS::Value* vp);
bool SE_GetServerSetting(JSContext* cx, unsigned argc, JS::Value* vp);

bool SE_DeleteFile(JSContext* cx, unsigned argc, JS::Value* vp);

// Account 
bool SE_GetAccountCount(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_GetPlayerCount(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_GetItemCount(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_GetMultiCount(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_GetCharacterCount(JSContext* cx, unsigned argc, JS::Value* vp);		//										***
bool SE_GetServerVersionString(JSContext* cx, unsigned argc, JS::Value* vp);	//										***

// Server constants
bool SE_BASEITEMSERIAL(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_INVALIDSERIAL(JSContext* cx, unsigned argc, JS::Value* vp);			//										***
bool SE_INVALIDID(JSContext* cx, unsigned argc, JS::Value* vp);				//										***
bool SE_INVALIDCOLOUR(JSContext* cx, unsigned argc, JS::Value* vp);			//										***


#endif
