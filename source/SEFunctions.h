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

//#ifndef __LINUX__
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

//#else
/*
JSBool SE_Gump_CreateGump(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Gump creation							***
JSBool SE_Gump_AddButton(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add button to Gump						***
JSBool SE_Gump_AddText(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add text to gump							***
JSBool SE_Gump_AddRadio(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add radio button to gump					***
JSBool SE_Gump_AddPage(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add page to gump							***
JSBool SE_Gump_AddPicture(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add picture to gump						***
JSBool SE_Gump_AddGump(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add gump to gump							***
JSBool SE_Gump_AddBackground(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Add background to gump					***
JSBool SE_Gump_SendToSocket(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Sends gump to socket						***
JSBool SE_Gump_AddTextEntry(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Adds text entry to gump					***
JSBool SE_Gump_AddCheckbox(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Adds checkbox to gump					***
JSBool SE_Gump_Delete(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Deletes a gump once done					***

// Character and Item common functions (7)
JSBool SE_GetName2(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											***
JSBool SE_SetName2(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											***
JSBool SE_DoDamage(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// Do damage to a char/item					***
JSBool SE_SetSerial(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											***


// Character related functions (9)

JSBool SE_GetTarget(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			// Get target of char						***
JSBool SE_RunTo(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				// path find to a particular location		*
JSBool SE_WalkTo(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											*
JSBool SE_SetTarget(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_SpawnNPC(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			// Create NPC								*
JSBool SE_GetCharPack(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_CalcDefense(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_CalcAttack(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***

JSBool SE_GetTown(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											**
JSBool SE_SetTown(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											**
JSBool SE_GetRegion(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											**
JSBool SE_SetRegion(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											**

// Effect related functions (3)

JSBool SE_DoStaticEffect(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		// Does a static effect						***
JSBool SE_DoMovingEffect(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		// Does moving effect						***
JSBool SE_DoTempEffect(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		// Does a temp effect						***

// Magic related functions (1)
JSBool SE_CastSpell(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			// A casts B								***

// Item related functions (2)

JSBool SE_SpawnItem(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			// Create an item							*
JSBool SE_GetLowDamage(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											***
JSBool SE_GetHiDamage(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_SetAtt(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											***
JSBool SE_GetDef(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											***
JSBool SE_SetDef(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											***
JSBool SE_SetDecayable( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool SE_IsDecayable( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool SE_SetDecayTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool SE_GetDecayTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

// Speech related functions (7)

JSBool SE_BroadcastMessage(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											***
JSBool SE_ConsoleMessage(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											***

// Attack related functions (7)
JSBool SE_AttackTarget(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	// A attacks B								***
JSBool SE_Attack(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											***
JSBool SE_FindNearestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindStrongestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindWeakestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindFirstTarget(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindLastTarget(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindSmartestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindDumbestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindQuickestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*
JSBool SE_FindSlowestTarget(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	//											*


// Random functions(5)
JSBool ScriptPrintNumber(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											***
JSBool SetShowLayer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool GetShowLayer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_CalcSockFromChar(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	// calculates a socket from a character		***
JSBool SE_RandomNumber(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		// Calculates a random number				***
JSBool SE_CalcCharFromSer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											***
JSBool SE_CalcItemFromSer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											***
JSBool SE_DistanceTo(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			// Distance between things					***

// Other functions
JSBool SE_GetCommand (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval ); 
JSBool SE_GetCommandSize (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval ); 
JSBool SE_GetString(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_GetDWord(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_CreateBuffer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_DestroyBuffer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_SendBuffer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											*
JSBool SE_SetByte(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											***
JSBool SE_SetWord(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//											***
JSBool SE_SetDWord(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_SetString(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_ReadBytes(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***

JSBool SE_UseReagant(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_UseReagants(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_AddSpell(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_RemoveSpell(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_SpellFail(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***

JSBool SE_GlowItem(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											*

JSBool SE_MakeMenu(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_MakeItem(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_CalcRank(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***
JSBool SE_CalcCharFromSock(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//											***
JSBool SE_ApplyRank(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											***

JSBool SE_ExecuteCommand(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_CommandLevelReq(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_CommandExists(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_FirstCommand(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_NextCommand(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											*
JSBool SE_FinishedCommandList(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//											*

JSBool SE_RegisterCommand(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_UnregisterCommand(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//											*
JSBool SE_RegisterKey(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//											*
JSBool SE_UnregisterKey(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//											*
JSBool SE_RegisterPacketHandler(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										*
JSBool SE_UnregisterPacketHandler(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										*

JSBool SE_GetHour(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);					//										***
JSBool SE_GetMinute(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_GetDay(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);					//										***
JSBool SE_GetSecondsPerUOMinute(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_GetLightLevel(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										*

JSBool SE_GetCurrentClock(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_SubStringSearch(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_IsCriminal(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_IsMurderer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_IsInnocent(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_GetFlag(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);					//										***
JSBool SE_MakeCriminal(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_MakeInnocent(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_GetMurderThreshold(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_GetGender(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_SetGender(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_RollDice(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_TurnToward(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_DirectionTo(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_RaceCompare(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_RaceCompareByRace(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_FindItemOnLayer(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***

JSBool SE_FindMulti(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***

JSBool SE_IsRegionGuarded(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_CanMarkInRegion(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_CanRecallInRegion(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_CanGateInRegion(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***

JSBool SE_GetGuildType(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_GetGuildName(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_GetGuildMaster(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_SetGuildType(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_SetGuildName(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_SetGuildMaster(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_GetNumGuildMembers(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										**
JSBool SE_AddToGuild(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										**
JSBool SE_RemoveFromGuild(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_CompareGuild(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_CompareGuildByGuild(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										**
JSBool SE_GetGuildStone(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**

JSBool SE_IsMemberOfTown(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_GetTownMayor(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_GetTownRace(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_SetTownRace(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_PossessTown(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_GetTownTax(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										*
JSBool SE_GetTownTaxResource(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_SetTownTax(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										*
JSBool SE_SetTownTaxResource(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										*

JSBool SE_GetRaceName(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_SetRaceName(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										*
JSBool SE_IsRaceWeakToWeather(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_RaceRequiresBeard(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_RaceRequiresNoBeard(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_IsPlayerRace(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_GetRacialGenderRestrict(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_GetRacialArmourClass(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_CanRaceWearArmour(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***
JSBool SE_GetRaceLanguageSkill(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_GetRaceSkillAdjustment(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_GetRacePoisonResistance(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_GetRaceMagicResistance(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_GetRaceVisibleDistance(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										*
JSBool SE_GetRaceNightVision(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										*
JSBool SE_IsValidRaceHairColour(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_IsValidRaceSkinColour(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***
JSBool SE_IsValidRaceBeardColour(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);	//										***

JSBool SE_GetClosestTown(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										*
JSBool SE_FirstItemInCont(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_NextItemInCont(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										***
JSBool SE_FinishedItemInCont(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);		//										***

JSBool SE_UseDoor(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);					//										***

JSBool SE_VendorSell(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***
JSBool SE_VendorBuy(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										***

JSBool SE_TriggerEvent(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										*
JSBool SE_GetGuild(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										**

JSBool SE_SkillBeingUsed(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);			//										**
JSBool SE_SetSkillUse(JSContext *cx, JSObject *obj,uintN argc,jsval *argv,jsval *rval);				//										**

JSBool SE_GetPrivateBit(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );			//										*
JSBool SE_SetPrivateBit(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );			//										*
JSBool SE_GetPrivateWord(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );			//										*
JSBool SE_SetPrivateWord(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );			//										*
JSBool SE_GetPrivateBitRange(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		//										*
JSBool SE_SetPrivateBitRange(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		//										*
JSBool SE_GetPackOwner(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );			//										***
JSBool SE_CalcTargetedItem(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		//										***
JSBool SE_CalcTargetedChar(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		//										***
JSBool SE_GetTileIDAtMapCoord(	JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		//										***
JSBool SE_GetClientVersion(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		//										***
JSBool SE_GetClientType(		JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );			//										***

JSBool SE_FileOpen(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool SE_FileClose(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool SE_Write(				JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		
JSBool SE_Read(					JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );		
JSBool SE_ReadUntil(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	
JSBool SE_EndOfFile(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	
JSBool SE_StringToNum(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	
JSBool SE_NumToString(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );	

JSBool SE_GetRaceCount(			JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );
JSBool SE_AreaCharacterFunction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval );

*/
//#endif

#endif

