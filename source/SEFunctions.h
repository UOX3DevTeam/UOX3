#ifndef __SEFuncs_h__
#define __SEFuncs_h__
// Function prototypes exposed to the Script Engine

// Version History
// 1.0		Abaddon		12th September, 2000
//			Initial implementation, not many functions added
// 1.1		Abaddon		14th September, 2000
//			Declaration of 90 API functions and implementation of around 20 of them
// 1.2		Abaddon		18th September, 2000
//			Addition of more API functions and declarations of them
// 1.3		Zippy		28th October, 2001
//			Addition of File i/o functions.
// 1.4 		Shadowlord	3rd  November, 2001
//			Addition of Tag functions.

typedef JSBool (SEngineFunc)( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// 90 functions so far
// *	== prototype 
// **	== active function

// Gump Related Functions (10)

SEngineFunc SE_Gump_CreateGump;		// Gump creation							***
SEngineFunc SE_Gump_AddButton;		// Add button to Gump						***
SEngineFunc SE_Gump_AddText;		// Add text to gump							***
SEngineFunc SE_Gump_AddRadio;		// Add radio button to gump					***
SEngineFunc SE_Gump_AddPage;		// Add page to gump							***
SEngineFunc SE_Gump_AddPicture;		// Add picture to gump						***
SEngineFunc SE_Gump_AddGump;		// Add gump to gump							***
SEngineFunc SE_Gump_AddBackground;	// Add background to gump					***
SEngineFunc SE_Gump_SendToSocket;	// Sends gump to socket						***
SEngineFunc SE_Gump_AddTextEntry;	// Adds text entry to gump					***
SEngineFunc SE_Gump_AddCheckbox;	// Adds checkbox to gump					***
SEngineFunc SE_Gump_Delete;			// Deletes a gump once done					***

// Character and Item common functions (7)
SEngineFunc SE_GetName2;			//	Retrieve item's second name				***
SEngineFunc SE_SetName2;			//	Sets item's second name					***
SEngineFunc SE_DoDamage;			// Do damage to a char/item					***
SEngineFunc SE_SetSerial;			//											***


// Character related functions (9)

SEngineFunc SE_GetTarget;			// Get target of char						***
SEngineFunc SE_RunTo;				// path find to a particular location		*
SEngineFunc SE_WalkTo;				//											*
SEngineFunc SE_SetTarget;			//											***
SEngineFunc SE_SpawnNPC;			// Create NPC								*
SEngineFunc SE_GetCharPack;			//											***
SEngineFunc SE_CalcDefense;			//											***
SEngineFunc SE_CalcAttack;			//											***

SEngineFunc SE_GetTown;				//											**
SEngineFunc SE_SetTown;				//											**
SEngineFunc SE_GetRegion;			//											**
SEngineFunc SE_SetRegion;			//											**

// Effect related functions (3)

SEngineFunc SE_DoStaticEffect;		// Does a static effect						***
SEngineFunc SE_DoMovingEffect;		// Does moving effect						***
SEngineFunc SE_DoTempEffect;		// Does a temp effect						***

// Magic related functions (1)
SEngineFunc SE_CastSpell;			// A casts B								***

// Item related functions (2)

SEngineFunc SE_SpawnItem;			// Create an item							*
SEngineFunc SE_GetLowDamage;				//											***
SEngineFunc SE_GetHiDamage;				//											***
SEngineFunc SE_SetAtt;				//											***
SEngineFunc SE_GetDef;				//											***
SEngineFunc SE_SetDef;				//											***
SEngineFunc SE_SetDecayable;		//											***
SEngineFunc SE_IsDecayable;			//											***
SEngineFunc SE_SetDecayTime;		//											***
SEngineFunc SE_GetDecayTime;		//											***

// Speech related functions (7)

SEngineFunc SE_BroadcastMessage;	//											***
SEngineFunc SE_ConsoleMessage;		//											***

// Attack related functions (7)
SEngineFunc SE_AttackTarget;		// A attacks B								***
SEngineFunc SE_Attack;				//											***
SEngineFunc SE_FindNearestTarget;	//											*
SEngineFunc SE_FindStrongestTarget;	//											*
SEngineFunc SE_FindWeakestTarget;	//											*
SEngineFunc SE_FindFirstTarget;		//											*
SEngineFunc SE_FindLastTarget;		//											*
SEngineFunc SE_FindSmartestTarget;	//											*
SEngineFunc SE_FindDumbestTarget;	//											*
SEngineFunc SE_FindQuickestTarget;	//											*
SEngineFunc SE_FindSlowestTarget;	//											*


// Random functions(5)
SEngineFunc ScriptPrintNumber;		//											***
SEngineFunc SetShowLayer;			//											***
SEngineFunc GetShowLayer;			//											***
SEngineFunc SE_CalcSockFromChar;	// calculates a socket from a character		***
SEngineFunc SE_RandomNumber;		// Calculates a random number				***
SEngineFunc SE_CalcCharFromSer;		//											***
SEngineFunc SE_CalcItemFromSer;		//											***
SEngineFunc SE_DistanceTo;			// Distance between things					***

// Other functions
SEngineFunc	SE_GetCommand; 
SEngineFunc	SE_GetCommandSize; 
SEngineFunc SE_GetString;			//											***
SEngineFunc SE_GetDWord;			//											***
SEngineFunc SE_CreateBuffer;		//											*
SEngineFunc SE_DestroyBuffer;		//											*
SEngineFunc SE_SendBuffer;			//											*
SEngineFunc SE_SetByte;				//											***
SEngineFunc SE_SetWord;				//											***
SEngineFunc SE_SetDWord;			//											***
SEngineFunc SE_SetString;			//											***
SEngineFunc SE_ReadBytes;			//											***

SEngineFunc SE_UseReagant;			//											***
SEngineFunc SE_UseReagants;			//											***
SEngineFunc SE_AddSpell;			//											***
SEngineFunc SE_RemoveSpell;			//											***
SEngineFunc SE_SpellFail;			//											***

SEngineFunc SE_GlowItem;			//											*

SEngineFunc SE_MakeMenu;			//											***
SEngineFunc SE_MakeItem;			//											***
SEngineFunc SE_CalcRank;			//											***
SEngineFunc SE_CalcCharFromSock;	//											***
SEngineFunc SE_ApplyRank;			//											***

SEngineFunc SE_ExecuteCommand;		//											*
SEngineFunc SE_CommandLevelReq;		//											*
SEngineFunc SE_CommandExists;		//											*
SEngineFunc SE_FirstCommand;		//											*
SEngineFunc SE_NextCommand;			//											*
SEngineFunc SE_FinishedCommandList;	//											*

SEngineFunc SE_RegisterCommand;		//											*
SEngineFunc SE_UnregisterCommand;	//											*
SEngineFunc SE_RegisterKey;			//											*
SEngineFunc SE_UnregisterKey;		//											*
SEngineFunc SE_RegisterPacketHandler;	//										*
SEngineFunc SE_UnregisterPacketHandler;	//										*

SEngineFunc SE_GetHour;					//										***
SEngineFunc SE_GetMinute;				//										***
SEngineFunc SE_GetDay;					//										***
SEngineFunc SE_GetSecondsPerUOMinute;	//										***
SEngineFunc SE_GetLightLevel;			//										*

SEngineFunc SE_GetCurrentClock;			//										***
SEngineFunc SE_SubStringSearch;			//										***
SEngineFunc SE_IsCriminal;				//										***
SEngineFunc SE_IsMurderer;				//										***
SEngineFunc SE_IsInnocent;				//										***
SEngineFunc SE_GetFlag;					//										***
SEngineFunc SE_MakeCriminal;			//										***
SEngineFunc SE_MakeInnocent;			//										***
SEngineFunc SE_GetMurderThreshold;		//										***
SEngineFunc SE_GetGender;				//										***
SEngineFunc SE_SetGender;				//										***
SEngineFunc SE_RollDice;				//										***
SEngineFunc SE_TurnToward;				//										***
SEngineFunc SE_DirectionTo;				//										***
SEngineFunc SE_RaceCompare;				//										***
SEngineFunc SE_RaceCompareByRace;		//										***
SEngineFunc SE_FindItemOnLayer;			//										***

SEngineFunc SE_FindMulti;				//										***

SEngineFunc SE_IsRegionGuarded;			//										***
SEngineFunc SE_CanMarkInRegion;			//										***
SEngineFunc SE_CanRecallInRegion;		//										***
SEngineFunc SE_CanGateInRegion;			//										***

SEngineFunc SE_GetGuildType;			//										**
SEngineFunc SE_GetGuildName;			//										**
SEngineFunc SE_GetGuildMaster;			//										**
SEngineFunc SE_SetGuildType;			//										**
SEngineFunc SE_SetGuildName;			//										**
SEngineFunc SE_SetGuildMaster;			//										**
SEngineFunc SE_GetNumGuildMembers;		//										**
SEngineFunc SE_AddToGuild;				//										**
SEngineFunc SE_RemoveFromGuild;			//										**
SEngineFunc SE_CompareGuild;			//										**
SEngineFunc SE_CompareGuildByGuild;		//										**
SEngineFunc SE_GetGuildStone;			//										**

SEngineFunc SE_IsMemberOfTown;			//										***
SEngineFunc SE_GetTownMayor;			//										***
SEngineFunc SE_GetTownRace;				//										***
SEngineFunc SE_SetTownRace;				//										***
SEngineFunc SE_PossessTown;				//										***
SEngineFunc SE_GetTownTax;				//										*
SEngineFunc SE_GetTownTaxResource;		//										***
SEngineFunc SE_SetTownTax;				//										*
SEngineFunc SE_SetTownTaxResource;		//										*

SEngineFunc SE_GetRaceName;				//										***
SEngineFunc SE_SetRaceName;				//										*
SEngineFunc SE_IsRaceWeakToWeather;		//										***
SEngineFunc SE_RaceRequiresBeard;		//										***
SEngineFunc SE_RaceRequiresNoBeard;		//										***
SEngineFunc SE_IsPlayerRace;			//										***
SEngineFunc SE_GetRacialGenderRestrict;	//										***
SEngineFunc SE_GetRacialArmourClass;	//										***
SEngineFunc SE_CanRaceWearArmour;		//										***
SEngineFunc SE_GetRaceLanguageSkill;	//										***
SEngineFunc SE_GetRaceSkillAdjustment;	//										***
SEngineFunc SE_GetRacePoisonResistance;	//										***
SEngineFunc SE_GetRaceMagicResistance;	//										***
SEngineFunc SE_GetRaceVisibleDistance;	//										*
SEngineFunc SE_GetRaceNightVision;		//										*
SEngineFunc SE_IsValidRaceHairColour;	//										***
SEngineFunc SE_IsValidRaceSkinColour;	//										***
SEngineFunc SE_IsValidRaceBeardColour;	//										***

SEngineFunc SE_GetClosestTown;			//										*
SEngineFunc SE_FirstItemInCont;			//										***
SEngineFunc SE_NextItemInCont;			//										***
SEngineFunc SE_FinishedItemInCont;		//										***

SEngineFunc SE_UseDoor;					//										***

SEngineFunc SE_VendorSell;				//										***
SEngineFunc SE_VendorBuy;				//										***

SEngineFunc SE_TriggerEvent;			//										*
SEngineFunc SE_GetGuild;				//										**

SEngineFunc SE_SkillBeingUsed;			//										**
SEngineFunc SE_SetSkillUse;				//										**

SEngineFunc SE_GetPrivateBit;			//										*
SEngineFunc SE_SetPrivateBit;			//										*
SEngineFunc SE_GetPrivateWord;			//										*
SEngineFunc SE_SetPrivateWord;			//										*
SEngineFunc SE_GetPrivateBitRange;		//										*
SEngineFunc SE_SetPrivateBitRange;		//										*
SEngineFunc SE_GetPackOwner;			//										***
SEngineFunc SE_CalcTargetedItem;		//										***
SEngineFunc SE_CalcTargetedChar;		//										***
SEngineFunc SE_GetTileIDAtMapCoord;		//										***
SEngineFunc SE_GetClientVersion;		//										***
SEngineFunc SE_GetClientType;			//										***

SEngineFunc SE_FileOpen;
SEngineFunc SE_FileClose;
SEngineFunc SE_Write;
SEngineFunc SE_Read;
SEngineFunc SE_ReadUntil;
SEngineFunc SE_EndOfFile;
SEngineFunc SE_StringToNum;
SEngineFunc SE_NumToString;

SEngineFunc SE_GetRaceCount;
SEngineFunc SE_AreaCharacterFunction;

#endif

