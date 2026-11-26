/// <reference path="../../definitions.d.ts" />
// @ts-check

// This script handles BOD offers from shopkeepers

var offerBodsFromItemSales = GetServerSetting( "OfferBODsFromItemSales" ); // If true, BODs can be offered by vendor upon selling items to them
var offerBodsFromContextMenu = GetServerSetting( "OfferBODsFromContextMenu" ); // Not present in LBR, disabled by default. Needs to be a server setting
var onlyOfferBodsFromCraftedItems = GetServerSetting( "BodsFromCraftedItemsOnly" ); // If true, only crafted items being sold can trigger BOD offers

// Multiplier for gold rewards given upon completion of BOD (2.0 equals 2 times the gold rewards)
var goldRewardMultiplier = GetServerSetting( "BODGoldRewardMultiplier" ); // Defaults to 1.0

// Multiplier for fame rewards given upon completion of BOD (2.0 would equal 2 times the fame rewards)
var fameRewardMultiplier = GetServerSetting( "BODFameRewardMultiplier" ); // Defaults to 1.0

// If crafting coloured weapons is allowed, also include them in the BOD requests
var canCraftColouredWeapons = GetServerSetting( "CraftColouredWeapons" );

// Toggle large BODs on/off globally
var offerLargeBODs = true;

// Runtime offer storage keyed by player serial
var activeBodOffers = {};

// Weapon CreateEntries Table
var weaponCreateEntries = [
	// Iron, Dull Copper, Shadow Iron, Copper, Bronze, Gold, Agapite, Verite, Valorite
	[20, 20, 20, 20, 20, 20, 20, 20, 20], // Dagger
	[40, 40, 40, 40, 40, 40, 40, 40, 40], // Mace
	[41, 41, 41, 41, 41, 41, 41, 41, 41], // Maul
	[21, 21, 21, 21, 21, 21, 21, 21, 21], // Cutlass
	[27, 27, 27, 27, 27, 27, 27, 27, 27], // Viking Sword
	[44, 44, 44, 44, 44, 44, 44, 44, 44], // Hammer Pick
	[26, 26, 26, 26, 26, 26, 26, 26, 26], // Longsword
	[42, 42, 42, 42, 42, 42, 42, 42, 42], // War Mace
	[32, 32, 32, 32, 32, 32, 32, 32, 32], // Double Axe
	[33, 33, 33, 33, 33, 33, 33, 33, 33], // Large Battle Axe
	[28, 28, 28, 28, 28, 28, 28, 28, 28], // Battle Axe
	[24, 24, 24, 24, 24, 24, 24, 24, 24], // Scimitar
	[31, 31, 31, 31, 31, 31, 31, 31, 31], // Two Handed Axe
	[43, 43, 43, 43, 43, 43, 43, 43, 43], // War Hammer
	[25, 25, 25, 25, 25, 25, 25, 25, 25], // Broadsword
	[29, 29, 29, 29, 29, 29, 29, 29, 29], // Axe
	[30, 30, 30, 30, 30, 30, 30, 30, 30], // Executioner's Axe
	[23, 23, 23, 23, 23, 23, 23, 23, 23], // Kryss
	[37, 37, 37, 37, 37, 37, 37, 37, 37], // War Fork
	[34, 34, 34, 34, 34, 34, 34, 34, 34], // War Axe
	[22, 22, 22, 22, 22, 22, 22, 22, 22], // Katana
	[35, 35, 35, 35, 35, 35, 35, 35, 35], // Short Spear
	[38, 38, 38, 38, 38, 38, 38, 38, 38], // Bardiche
	[36, 36, 36, 36, 36, 36, 36, 36, 36], // Spear
	[39, 39, 39, 39, 39, 39, 39, 39, 39]  // Halberd
];

var armorCreateEntries = [
	// Iron, Dull Copper, Shadow Iron, Copper, Bronze, Gold, Agapite, Verite, Valorite
	[1, 500, 600, 700, 800, 900, 1200, 1000, 1100],   // Buckler
	[2, 501, 601, 701, 801, 901, 1201, 1001, 1101],   // Bronze Shield
	[3, 502, 602, 702, 802, 902, 1202, 1002, 1102],   // Metal Shield
	[4, 503, 603, 703, 803, 903, 1203, 1003, 1103],   // Tear Kite Shield
	[5, 504, 604, 704, 804, 904, 1204, 1004, 1104],   // Metal Kite Shield
	[6, 505, 605, 705, 805, 905, 1205, 1005, 1105],   // Heater Shield
	[7, 506, 606, 706, 806, 906, 1206, 1006, 1106],   // Ringmail Gloves
	[8, 507, 607, 707, 807, 907, 1207, 1007, 1107],   // Ringmail Sleeves
	[9, 508, 608, 708, 808, 908, 1208, 1008, 1108],   // Ringmail Leggings
	[10, 509, 609, 709, 809, 909, 1209, 1009, 1109],  // Ringmail Tunic
	[11, 510, 610, 710, 810, 910, 1210, 1010, 1110],  // Chainmail Coif
	[12, 511, 611, 711, 811, 911, 1211, 1011, 1111],  // Chainmail Leggings
	[13, 512, 612, 712, 812, 912, 1212, 1012, 1112],  // Chainmail Tunic
	[14, 513, 613, 713, 813, 913, 1213, 1013, 1113],  // Platemail Gorget
	[15, 514, 614, 714, 814, 914, 1214, 1014, 1114],  // Platemail Gloves
	[16, 515, 615, 715, 815, 915, 1215, 1015, 1115],  // Platemail Arms
	[17, 516, 616, 716, 816, 916, 1216, 1016, 1116],  // Platemail Legs
	[18, 517, 617, 717, 817, 917, 1217, 1017, 1117],  // Platemail Tunic
	[19, 518, 618, 718, 818, 918, 1218, 1018, 1118],  // Platemail (female)
	[45, 519, 619, 719, 819, 919, 1219, 1019, 1119],  // Helmet
	[46, 520, 620, 720, 820, 920, 1220, 1020, 1120],  // Bascinet
	[47, 521, 621, 721, 821, 921, 1221, 1021, 1121],  // Norse Helm
	[48, 522, 622, 722, 822, 922, 1222, 1022, 1122],  // Close Helm
	[49, 523, 623, 723, 823, 923, 1223, 1023, 1123]   // Plate Helm
];

var clothCreateEntries = [
	[130], // Skullcap
	[131], // Bandana
	[132], // Floppy Hat
	[133], // Wide Brim Hat
	[134], // Cap
	[135], // Tall Straw Hat
	[136], // Straw Hat
	[137], // Wizard's Hat
	[138], // Bonnet
	[139], // Feathered Hat
	[140], // Tricorne Hat
	[141], // Jester Hat
	[142], // Doublet
	[143], // Shirt
	[144], // Fancy Shirt
	[145], // Tunic
	[146], // Surcoat
	[147], // Plain Dress
	[148], // Fancy Dress
	[149], // Cloak
	[150], // Robe
	[151], // Jester Suit
	[152], // Long Pants
	[153], // Kilt
	[154], // Skirt
	[155], // Body Sash
	[156], // Half Apron
	[157], // Full Apron
	[158], // Oil Cloth
	[160]  // Shoes
];

// Outer properties correspond to bodType tag.
// Inner properties correspond to bodSubtype tag.
var BODTypesToCreateEntries = {
	1: { // Blacksmithing
		1: weaponCreateEntries,
		2: armorCreateEntries,
		3: weaponCreateEntries.concat( armorCreateEntries )
	},
	2: { // Tailoring
		1: clothCreateEntries
	}
};

var BODTypeToDFNSectionID = {
	1: "smallbod_blacksmith",
	2: "smallbod_tailor"
};

var BODTypeToLargeDFNSectionID = {
	1: "largebod_blacksmith",
	2: "largebod_tailor"
};

var BODTypesToSkillNames = {
	1: "blacksmithing",
	2: "tailoring"
};

// A higher index in this list means the reward is less likely to be given.
var BlacksmithRewardTiersToItems = [
	{
		items: [
			{ itemName: "sturdy_pickaxe", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "sturdy_shovel", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "sturdy_pickaxe", props: [["maxUses", 150], ["usesLeft", 150]] },
			{ itemName: "sturdy_shovel", props: [["maxUses", 150], ["usesLeft", 150]] }
		],
		selectType: "random"
	},
	{ items: [{ itemName: "mining_gloves_1" }] },
	{ items: [{ itemName: "mining_gloves_3" }] },
	{ items: [{ itemName: "mining_gloves_5" }] },
	{ items: [{ itemName: "dull_copper_runic_hammer" }] },
	{ items: [{ itemName: "shadow_iron_runic_hammer" }] },
	{
		items: [
			{ itemName: "dc_anvil_deed" },
			{ itemName: "si_anvil_deed" },
			{ itemName: "c_anvil_deed" },
			{ itemName: "b_anvil_deed" },
			{ itemName: "g_anvil_deed" },
			{ itemName: "a_anvil_deed" },
			{ itemName: "ve_anvil_deed" },
			{ itemName: "va_anvil_deed" }
		],
		selectType: "weighted"
	},
	{ items: [{ itemName: "copper_runic_hammer" }] },
	{ items: [{ itemName: "bronze_runic_hammer" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_10" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_15" }] },
	{ items: [{ itemName: "gold_runic_hammer" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_30" }] },
	{ items: [{ itemName: "agapite_runic_hammer" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_60" }] },
	{ items: [{ itemName: "verite_runic_hammer" }] },
	{ items: [{ itemName: "valorite_runic_hammer" }] }
];

var BlacksmithRewardTiersToItemsAoS = [
	{
		items: [
			{ itemName: "sturdy_pickaxe", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "sturdy_shovel", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "sturdy_pickaxe", props: [["maxUses", 150], ["usesLeft", 150]] },
			{ itemName: "sturdy_shovel", props: [["maxUses", 150], ["usesLeft", 150]] }
		],
		selectType: "random"
	},
	{ items: [{ itemName: "mining_gloves_1" }] },
	{
		items: [
			{ itemName: "gargoyles_pickaxe", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "prospectors_tool", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "gargoyles_pickaxe", props: [["maxUses", 150], ["usesLeft", 150]] },
			{ itemName: "prospectors_tool", props: [["maxUses", 150], ["usesLeft", 150]] }
		],
		selectType: "random"
	},
	{ items: [{ itemName: "mining_gloves_3" }] },
	{ items: [{ itemName: "powder_of_temperament" }] },
	{ items: [{ itemName: "mining_gloves_5" }] },
	{ items: [{ itemName: "dull_copper_runic_hammer" }] },
	{ items: [{ itemName: "shadow_iron_runic_hammer" }] },
	{ items: [{ itemName: "powerscroll_smith_5" }] },
	{ items: [{ itemName: "copper_runic_hammer" }] },
	{
		items: [
			{ itemName: "dc_anvil_deed" },
			{ itemName: "si_anvil_deed" },
			{ itemName: "c_anvil_deed" },
			{ itemName: "b_anvil_deed" },
			{ itemName: "g_anvil_deed" },
			{ itemName: "a_anvil_deed" },
			{ itemName: "ve_anvil_deed" },
			{ itemName: "va_anvil_deed" }
		],
		selectType: "weighted"
	},
	{ items: [{ itemName: "powerscroll_smith_10" }] },
	{ items: [{ itemName: "bronze_runic_hammer" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_10" }] },
	{ items: [{ itemName: "powerscroll_smith_15" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_15" }] },
	{ items: [{ itemName: "powerscroll_smith_20" }] },
	{ items: [{ itemName: "gold_runic_hammer" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_30" }] },
	{ items: [{ itemName: "agapite_runic_hammer" }] },
	{ items: [{ itemName: "ancient_smithy_hammer_60" }] },
	{ items: [{ itemName: "verite_runic_hammer" }] },
	{ items: [{ itemName: "valorite_runic_hammer" }] }
];

var TailorRewardTiersToItems = [
	{
		items: [
			{ itemName: "sewing_kit", props: [["maxUses", 250], ["usesLeft", 250]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x483]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x48C]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x488]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x48A]] }
		],
		selectType: "random"
	},
	{
		items: [
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x495]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x48B]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x486]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x485]] }
		],
		selectType: "random"
	},
	{
		items: [
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x48D]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x490]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x48E]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x491]] }
		],
		selectType: "random"
	},
	{
		items: [
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x48F]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x494]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x484]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x497]] },
			{ itemName: "sandals", props: [["colour", 0x489]] },
			{ itemName: "sandals", props: [["colour", 0x47F]] },
			{ itemName: "sandals", props: [["colour", 0x482]] },
			{ itemName: "sandals", props: [["colour", 0x47E]] },
			{ itemName: "sandals", props: [["colour", 0x48F]] },
			{ itemName: "sandals", props: [["colour", 0x494]] },
			{ itemName: "sandals", props: [["colour", 0x484]] },
			{ itemName: "sandals", props: [["colour", 0x497]] }
		],
		selectType: "random"
	},
	{
		items: [
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x489]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x47F]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x482]] },
			{ itemName: "folded_cloth", props: [["amount", 100], ["colour", 0x47E]] }
		],
		selectType: "random"
	},
	{ items: [{ itemName: "spined_runic_sewing_kit" }] },
	{ items: [{ itemName: "clothing_bless_deed" }] },
	{ items: [{ itemName: "horned_runic_sewing_kit" }] },
	{ items: [{ itemName: "barbed_runic_sewing_kit" }] }
];

var BODTypesToRewards = {
	1: BlacksmithRewardTiersToItems,
	2: TailorRewardTiersToItems
};

var BODTypesToRewardsAoS = {
	1: BlacksmithRewardTiersToItemsAoS,
	2: TailorRewardTiersToItems
};

/** @type { ( targSock: Socket, objVendor: Character, objItemSold: BaseObject, numItemsSold: number ) => boolean } */
function onSoldToVendor( pSock, npcVendor, iSold, numItemsSold )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) || !ValidateObject( npcVendor ) || !ValidateObject( iSold ))
		return false;

	if( offerBodsFromItemSales && CheckBodTimers( pUser, npcVendor.GetTag( "bodType" )))
	{
		if( !onlyOfferBodsFromCraftedItems || iSold.madeWith != -1 )
		{
			if( RandomNumber( 1, 5 ) == 5 )
				OfferBODFromVendor( pUser, npcVendor );
		}
	}

	return false;
}

/** @param {Character} pUser @param {number} bodType @returns {boolean} */
function CheckBodTimers( pUser, bodType )
{
	var socket = pUser.socket;
	if( socket == null )
		return false;

	var bodOfferCooldown = pUser.GetJSTimer( bodType, 3214 );
	if( bodOfferCooldown != 0 )
	{
		pUser.SysMessage(
			GetDictionaryEntry( 17265, socket.language ) + " " +
			(( bodOfferCooldown - GetCurrentClock()) / 60000 ).toFixed( 0 ) + " " +
			GetDictionaryEntry( 17266, socket.language )
		);
		return false;
	}

	return true;
}

/** @type { ( speech: string, personTalking: Character, talkingTo: BaseObject ) => null | undefined | number | boolean } */
function onSpeech( myString, pUser, myNPC )
{
	if( !ValidateObject( pUser ) || !ValidateObject( myNPC ) || !offerBodsFromContextMenu || !pUser.InRange( myNPC, 8 ))
		return false;

	var socket = pUser.socket;
	if( socket == null )
		return false;

	var storedShopkeeperSerial = pUser.GetTempTag( "bodShopkeeperSerial" );
	var targetShopkeeper = CalcCharFromSer( storedShopkeeperSerial );
	if( !ValidateObject( targetShopkeeper ) || targetShopkeeper != myNPC )
		return false;

	pUser.SetTempTag( "bodShopkeeperSerial", null );

	for( var triggerWord = socket.FirstTriggerWord(); !socket.FinishedTriggerWords(); triggerWord = socket.NextTriggerWord())
	{
		switch( triggerWord )
		{
			case 0x5000:
			{
				if( CheckBodTimers( pUser, myNPC.GetTag( "bodType" )))
				{
					if( EraStringToNum( GetServerSetting( "CoreShardEra" )) <= EraStringToNum( "lbr" ))
					{
						myNPC.SetTimer( Timer.MOVETIME, 1000 );
						myNPC.TurnToward( pUser );
						OfferBODFromVendor( pUser, myNPC );
						return 1;
					}
				}
				break;
			}
		}
	}

	return false;
}

/** @param {Character} pUser @param {Character} myNPC @returns {void} */
function SmallBODAcceptGump( pUser, myNPC )
{
	var socket = pUser.socket;
	if( socket == null || !ValidateObject( myNPC ))
		return;

	var bodType = myNPC.GetTag( "bodType" );
	var bodSubtype = myNPC.GetTag( "bodSubtype" );
	var playerSkill = pUser.skills[BODTypesToSkillNames[bodType]];

	var bodEntry = SelectBodEntry( bodType, bodSubtype, true, playerSkill );
	if( !bodEntry )
	{
		socket.SysMessage( "No valid BOD entry could be selected." );
		return;
	}

	var itemName = bodEntry.name;
	var graphicID = bodEntry.id;
	var materialColor = 0;
	if( bodEntry.resources && bodEntry.resources[0] )
		materialColor = bodEntry.resources[0][1];

	var amountMax = 0;
	var reqExceptional = false;

	if( playerSkill >= 700 )
	{
		if( (( playerSkill + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
			reqExceptional = true;

		var valuesOverSeventy = [10, 15, 20, 20];
		amountMax = valuesOverSeventy[Math.floor( Math.random() * valuesOverSeventy.length )];
	}
	else if( playerSkill >= 500 )
	{
		var valuesOverFifty = [10, 15, 15, 20];
		amountMax = valuesOverFifty[Math.floor( Math.random() * valuesOverFifty.length )];
	}
	else
	{
		var valuesLow = [10, 10, 15, 20];
		amountMax = valuesLow[Math.floor( Math.random() * valuesLow.length )];
	}

	activeBodOffers[pUser.serial] = {
		offerType: "small",
		npcSerial: myNPC.serial,
		bodType: bodType,
		bodSubtype: bodSubtype,
		amountMax: amountMax,
		reqExceptional: reqExceptional,
		graphicID: graphicID,
		itemName: itemName,
		materialColor: materialColor,
		bodSectionID: bodEntry.addItem ? bodEntry.addItem : ""
	};

	var bodGump = new Gump();
	bodGump.AddPage( 0 );
	bodGump.AddBackground( 25, 10, 430, 264, 5054 );

	bodGump.AddTiledGump( 33, 20, 413, 245, 2624 );
	bodGump.AddCheckerTrans( 33, 20, 413, 245 );

	bodGump.AddGump( 20, 5, 10460 );
	bodGump.AddGump( 430, 5, 10460 );
	bodGump.AddGump( 20, 249, 10460 );
	bodGump.AddGump( 430, 249, 10460 );

	bodGump.AddHTMLGump( 190, 25, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17250, socket.language ) + "</basefont>" );
	bodGump.AddHTMLGump( 40, 48, 350, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17267, socket.language ) + "</basefont>" );

	bodGump.AddHTMLGump( 40, 72, 210, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>" );
	bodGump.AddText( 250, 72, 1152, amountMax );

	bodGump.AddHTMLGump( 40, 96, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" );
	bodGump.AddPicture( 325, 96, graphicID );
	bodGump.AddHTMLGump( 40, 120, 210, 20, false, false, "<basefont color=#ffffff>" + itemName + "</basefont>" );

	if( reqExceptional || materialColor > 0 )
	{
		bodGump.AddHTMLGump( 40, 144, 210, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" );
	}

	if( reqExceptional )
	{
		bodGump.AddHTMLGump( 40, 168, 350, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>" );
	}

	if( materialColor > 0 )
	{
		var materialRequirementY = 168;
		if( reqExceptional )
			materialRequirementY = 192;

		var materialName = "";
		switch( materialColor )
		{
			case 0:
				materialName = "iron";
				break;
			case 2419:
				materialName = "dull copper";
				break;
			case 2406:
				materialName = "shadow iron";
				break;
			case 2414:
				materialName = "copper";
				break;
			case 1750:
				materialName = "bronze";
				break;
			case 2213:
				materialName = "gold";
				break;
			case 2425:
				materialName = "agapite";
				break;
			case 2207:
				materialName = "verite";
				break;
			case 2219:
				materialName = "valorite";
				break;
		}

		if( materialName !== "" )
		{
			bodGump.AddHTMLGump( 40, materialRequirementY, 350, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" );
		}
	}

	bodGump.AddHTMLGump( 40, 216, 350, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17268, socket.language ) + "</basefont>" );

	bodGump.AddButton( 100, 240, 4005, 4007, 1, 0, 1 );
	bodGump.AddHTMLGump( 135, 240, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17269, socket.language ) + "</basefont>" );

	bodGump.AddButton( 275, 240, 4005, 4007, 1, 0, 0 );
	bodGump.AddHTMLGump( 310, 240, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 2709, socket.language ) + "</basefont>" );

	bodGump.Send( socket );
	bodGump.Free();
}

/** @param {Character} pUser @param {Character} myNPC @returns {void} */
function LargeBODAcceptGump( pUser, myNPC )
{
	var socket = pUser.socket;
	if( socket == null || !ValidateObject( myNPC ))
		return;

	var bodType = myNPC.GetTag( "bodType" );
	var bodSubtype = myNPC.GetTag( "bodSubtype" );
	var playerSkill = pUser.skills[BODTypesToSkillNames[bodType]];

	var amountMax = 0;
	var reqExceptional = false;

	if( playerSkill >= 700 )
	{
		if( (( playerSkill + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
			reqExceptional = true;

		var valuesOverSeventy = [10, 15, 20, 20];
		amountMax = valuesOverSeventy[Math.floor( Math.random() * valuesOverSeventy.length )];
	}
	else if( playerSkill >= 500 )
	{
		var valuesOverFifty = [10, 15, 15, 20];
		amountMax = valuesOverFifty[Math.floor( Math.random() * valuesOverFifty.length )];
	}
	else
	{
		var valuesLow = [10, 10, 15, 20];
		amountMax = valuesLow[Math.floor( Math.random() * valuesLow.length )];
	}

	activeBodOffers[pUser.serial] = {
		offerType: "large",
		npcSerial: myNPC.serial,
		bodType: bodType,
		bodSubtype: bodSubtype,
		amountMax: amountMax,
		reqExceptional: reqExceptional,
		graphicID: 0,
		itemName: "",
		materialColor: 0,
		bodSectionID: ""
	};

	var bodGump = new Gump();
	bodGump.AddPage( 0 );
	bodGump.AddBackground( 25, 10, 430, 264, 5054 );

	bodGump.AddTiledGump( 33, 20, 413, 245, 2624 );
	bodGump.AddCheckerTrans( 33, 20, 413, 245 );

	bodGump.AddGump( 20, 5, 10460 );
	bodGump.AddGump( 430, 5, 10460 );
	bodGump.AddGump( 20, 249, 10460 );
	bodGump.AddGump( 430, 249, 10460 );

	bodGump.AddHTMLGump( 190, 25, 160, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17285, socket.language ) + "</basefont>" );
	bodGump.AddHTMLGump( 40, 48, 350, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17267, socket.language ) + "</basefont>" );

	bodGump.AddHTMLGump( 40, 80, 360, 60, false, false, "<basefont color=#ffffff>You are being offered a large bulk order deed. The exact requested items will be generated when you first open the deed.</basefont>" );

	bodGump.AddHTMLGump( 40, 150, 350, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17268, socket.language ) + "</basefont>" );

	bodGump.AddButton( 100, 240, 4005, 4007, 1, 0, 1 );
	bodGump.AddHTMLGump( 135, 240, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17269, socket.language ) + "</basefont>" );

	bodGump.AddButton( 275, 240, 4005, 4007, 1, 0, 0 );
	bodGump.AddHTMLGump( 310, 240, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 2709, socket.language ) + "</basefont>" );

	bodGump.Send( socket );
	bodGump.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( socket, pButton, gumpData )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var activeOfferKey = pUser.serial;
	var activeOffer = activeBodOffers[activeOfferKey];
	if( !activeOffer )
		return;

	if( activeOffer.offerType == "small" )
	{
		HandleSmallBODOfferResponse( socket, pUser, pButton, activeOffer );
		return;
	}

	if( activeOffer.offerType == "large" )
	{
		HandleLargeBODOfferResponse( socket, pUser, pButton, activeOffer );
		return;
	}
}

/** @param {Socket} socket @param {Character} pUser @param {number} pButton @param {{ offerType: string, npcSerial: number, bodType: number, bodSubtype: number, amountMax: number, reqExceptional: boolean, graphicID: number, itemName: string, materialColor: number, bodSectionID: string }} activeOffer @returns {void} */
function HandleSmallBODOfferResponse( socket, pUser, pButton, activeOffer )
{
	var bodType = activeOffer.bodType;
	var bodSubtype = activeOffer.bodSubtype;
	var amountMax = activeOffer.amountMax;
	var reqExceptional = activeOffer.reqExceptional;
	var graphicID = activeOffer.graphicID;
	var itemName = activeOffer.itemName;
	var materialColor = activeOffer.materialColor;
	var bodSectionID = activeOffer.bodSectionID;
	var activeOfferKey = pUser.serial;

	switch( pButton )
	{
		case 0:
			break;

		case 1:
		{
			if( !ValidateObject( pUser.pack ))
			{
				socket.SysMessage( "You need a backpack to receive this item." );
				delete activeBodOffers[activeOfferKey];
				return;
			}

			if( pUser.pack.totalItemCount >= pUser.pack.maxItems )
			{
				socket.SysMessage( GetDictionaryEntry( 1819, socket.language ));
				delete activeBodOffers[activeOfferKey];
				return;
			}

			var dfnSection = BODTypeToDFNSectionID[bodType];
			if( !dfnSection )
			{
				socket.SysMessage( "No DFN section defined for this BOD type." );
				delete activeBodOffers[activeOfferKey];
				return;
			}

			var smallBOD = CreateDFNItem( socket, pUser, dfnSection, 1, "ITEM", true );
			if( ValidateObject( smallBOD ))
			{
				smallBOD.SetTag( "itemName", itemName );
				smallBOD.SetTag( "graphicID", graphicID );
				smallBOD.SetTag( "amountMax", amountMax );
				smallBOD.SetTag( "reqExceptional", reqExceptional );
				smallBOD.SetTag( "materialColor", materialColor );
				smallBOD.SetTag( "sectionID", bodSectionID );
				smallBOD.SetTag( "bodSubtype", bodSubtype );
				smallBOD.SetTag( "bodType", bodType );
				smallBOD.SetTag( "amountCur", 0 );
				smallBOD.SetTag( "goldValue", 0 );
				smallBOD.SetTag( "fameValue", 0 );
				smallBOD.SetTag( "qualityValue", 0 );
				smallBOD.SetTag( "init", 1 );

				pUser.TextMessage( GetDictionaryEntry( 17274, socket.language ), false, 0x3b2, 0, pUser.serial );
			}
			break;
		}
	}

	delete activeBodOffers[activeOfferKey];
	SetBODAcceptanceCooldown( pUser, bodType );
}

/** @param {Socket} socket @param {Character} pUser @param {number} pButton @param {{ offerType: string, npcSerial: number, bodType: number, bodSubtype: number, amountMax: number, reqExceptional: boolean, graphicID: number, itemName: string, materialColor: number, bodSectionID: string }} activeOffer @returns {void} */
function HandleLargeBODOfferResponse( socket, pUser, pButton, activeOffer )
{
	var bodType = activeOffer.bodType;
	var bodSubtype = activeOffer.bodSubtype;
	var activeOfferKey = pUser.serial;

	switch( pButton )
	{
		case 0:
			break;

		case 1:
		{
			if( !ValidateObject( pUser.pack ))
			{
				socket.SysMessage( "You need a backpack to receive this item." );
				delete activeBodOffers[activeOfferKey];
				return;
			}

			if( pUser.pack.totalItemCount >= pUser.pack.maxItems )
			{
				socket.SysMessage( GetDictionaryEntry( 1819, socket.language ));
				delete activeBodOffers[activeOfferKey];
				return;
			}

			var dfnSection = BODTypeToLargeDFNSectionID[bodType];
			if( !dfnSection )
			{
				socket.SysMessage( "No DFN section defined for this large BOD type." );
				delete activeBodOffers[activeOfferKey];
				return;
			}

			var largeBOD = CreateDFNItem( socket, pUser, dfnSection, 1, "ITEM", true );
			if( ValidateObject( largeBOD ))
			{
				largeBOD.SetTag( "bodType", bodType );
				largeBOD.SetTag( "bodSubtype", bodSubtype );
				largeBOD.SetTag( "amountCur", 0 );
				largeBOD.SetTag( "goldValue", 0 );
				largeBOD.SetTag( "fameValue", 0 );
				largeBOD.SetTag( "qualityValue", 0 );
				largeBOD.SetTag( "complete", 0 );
				largeBOD.SetTag( "init", 0 );

				pUser.TextMessage( GetDictionaryEntry( 17274, socket.language ), false, 0x3b2, 0, pUser.serial );
			}
			else
			{
				socket.SysMessage( "Failed to create the large BOD deed." );
			}
			break;
		}
	}

	delete activeBodOffers[activeOfferKey];
	SetBODAcceptanceCooldown( pUser, bodType );
}

/** @param {Character} pUser @param {number} bodType @returns {void} */
function SetBODAcceptanceCooldown( pUser, bodType )
{
	var skillName = BODTypesToSkillNames[bodType];
	if( !skillName )
		return;

	var playerSkill = pUser.skills[skillName];
	var bodTimer = 0;

	if( playerSkill >= 700 )
		bodTimer = 21600000;
	else if( playerSkill >= 501 )
		bodTimer = 7200000;
	else
		bodTimer = 3600000;

	pUser.StartTimer( bodTimer, bodType, true );
}

/** @param {number} bodType @param {number} bodSubtype @param {boolean} considerPlayerSkill @param {number} playerSkill @returns {CreateEntry|null} */
function SelectBodEntry( bodType, bodSubtype, considerPlayerSkill, playerSkill )
{
	if( !BODTypesToCreateEntries[bodType] || !BODTypesToCreateEntries[bodType][bodSubtype] )
		return null;

	var bodItemEntries = BODTypesToCreateEntries[bodType][bodSubtype];
	if( !bodItemEntries || bodItemEntries.length == 0 || !bodItemEntries[0] )
		return null;

	var maxMaterialIndex = bodItemEntries[0].length - 1;
	var materialIndex = 0;

	if( bodType == 1 && canCraftColouredWeapons )
	{
		materialIndex = RandomNumber( 0, maxMaterialIndex );
		if( considerPlayerSkill && playerSkill >= ( 1000 - ( 50 * ( maxMaterialIndex - materialIndex ))))
			materialIndex = 0;
	}

	var validItems = [];
	for( var entryIndex = 0; entryIndex < bodItemEntries.length; entryIndex++ )
	{
		var itemRow = bodItemEntries[entryIndex];
		if( !itemRow || typeof itemRow[materialIndex] === "undefined" )
			continue;

		var createEntryIndex = itemRow[materialIndex];
		var createEntry = CreateEntries[createEntryIndex];
		if( !createEntry )
			continue;

		if( considerPlayerSkill && playerSkill < 700 )
		{
			if( createEntry.avgMinSkill <= playerSkill && playerSkill <= createEntry.avgMaxSkill )
				validItems.push( createEntryIndex );
		}
		else
		{
			validItems.push( createEntryIndex );
		}
	}

	if( validItems.length == 0 )
		return null;

	var randomCreateIndex = Math.floor( Math.random() * validItems.length );
	return CreateEntries[validItems[randomCreateIndex]];
}

/** @param {number} bodType @param {number} playerSkill @returns {boolean} */
function ShouldOfferLargeBOD( bodType, playerSkill )
{
	if( bodType != 1 && bodType != 2 )
		return false;

	if( playerSkill < 700 )
		return false;

	var skillRatio = ( playerSkill - 700 ) / 300;
	if( skillRatio < 0 )
		skillRatio = 0;
	if( skillRatio > 1 )
		skillRatio = 1;

	var chance = 0.10 + 0.30 * skillRatio;
	return Math.random() < chance;
}

/** @param {Character} pUser @param {Character} myNPC @returns {void} */
function OfferBODFromVendor( pUser, myNPC )
{
	if( !ValidateObject( pUser ) || !ValidateObject( myNPC ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	var bodType = myNPC.GetTag( "bodType" );
	if( !bodType || !BODTypesToSkillNames[bodType] )
	{
		socket.SysMessage( GetDictionaryEntry( 17286, socket.language ));
		return;
	}

	var playerSkill = pUser.skills[BODTypesToSkillNames[bodType]];
	delete activeBodOffers[pUser.serial];

	if( offerLargeBODs && ShouldOfferLargeBOD( bodType, playerSkill ))
		LargeBODAcceptGump( pUser, myNPC );
	else
		SmallBODAcceptGump( pUser, myNPC );
}

/** @type { ( srcChar: Character, targChar: Character, i: Item ) => number } */
function onDropItemOnNpc( pDropper, npcDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	if( socket == null )
		return false;

	if( iDropped.id == 0x0eed )
		return true;

	var amountMax = iDropped.GetTag( "amountMax" );
	var amountCur = iDropped.GetTag( "amountCur" );
	var itemBodType = iDropped.GetTag( "bodType" );
	var npcBodType = npcDroppedOn.GetTag( "bodType" );

	var sectionRoot = "";
	if( iDropped.sectionID && typeof iDropped.sectionID === "string" )
		sectionRoot = iDropped.sectionID.split( "_" )[0];

	if( sectionRoot == "smallbod" && npcBodType > 0 )
	{
		if( itemBodType != npcBodType )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17272, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		if( amountCur < amountMax )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17270, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		var bodRewardCooldown = pDropper.GetJSTimer( itemBodType * 10, 3214 );
		if( bodRewardCooldown != 0 )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17273, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		if( !ValidateObject( pDropper.pack ))
		{
			socket.SysMessage( "You need a backpack to receive rewards." );
			return false;
		}

		if( pDropper.pack.totalItemCount >= ( pDropper.pack.maxItems - 2 ))
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17275, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		if( DispenseBODRewards( pDropper, npcDroppedOn, iDropped ))
		{
			pDropper.StartTimer( 10000, itemBodType * 10, true );

			var offerCooldown = pDropper.GetJSTimer( itemBodType, 3214 );
			if( offerCooldown != 0 )
				pDropper.KillJSTimer( itemBodType, 3214 );

			iDropped.Delete();
			return 2;
		}

		return false;
	}

	if( sectionRoot == "largebod" && npcBodType > 0 )
	{
		if( itemBodType != npcBodType )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17272, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		var entryCount = iDropped.GetTag( "entryCount" );
		var amountMaxLarge = amountMax;
		var allComplete = true;

		for( var entryIndex = 0; entryIndex < entryCount; ++entryIndex )
		{
			var entryAmount = iDropped.GetTag( "entry" + entryIndex + "_amount" );
			if( entryAmount < amountMaxLarge )
			{
				allComplete = false;
				break;
			}
		}

		if( !allComplete )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17270, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		var largeRewardCooldown = pDropper.GetJSTimer( itemBodType * 10, 3214 );
		if( largeRewardCooldown != 0 )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17273, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		if( !ValidateObject( pDropper.pack ))
		{
			socket.SysMessage( "You need a backpack to receive rewards." );
			return false;
		}

		if( pDropper.pack.totalItemCount >= ( pDropper.pack.maxItems - 2 ))
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17275, socket.language ), false, 0x3b2, 0, pDropper.serial );
			return false;
		}

		if( DispenseBODRewards( pDropper, npcDroppedOn, iDropped ))
		{
			pDropper.StartTimer( 10000, itemBodType * 10, true );

			var offerCooldownLarge = pDropper.GetJSTimer( itemBodType, 3214 );
			if( offerCooldownLarge != 0 )
				pDropper.KillJSTimer( itemBodType, 3214 );

			iDropped.Delete();
			return 2;
		}

		return false;
	}

	return false;
}

/** @param {number} min @param {number} max @param {number} weight @returns {number} */
function WeightedRandom( min, max, weight )
{
	var randomValue = Math.random();
	return Math.round( min + (( max - min ) * Math.pow( randomValue, weight )));
}

/** @param {Character} pDropper @param {Character} npcDroppedOn @param {Item} iDropped @returns {boolean} */
function DispenseBODRewards( pDropper, npcDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	if( socket == null )
		return false;

	npcDroppedOn.TextMessage( GetDictionaryEntry( 17271, socket.language ), false, 0x3b2, 0, pDropper.serial );

	var fameToGive = iDropped.GetTag( "fameValue" ) * fameRewardMultiplier;

	var goldToGive = iDropped.GetTag( "goldValue" ) * goldRewardMultiplier;
	var goldReward = CreateDFNItem( socket, pDropper, "0x0EED", goldToGive, "ITEM", false );
	var goldWeight = 0;
	if( ValidateObject( goldReward ))
		goldWeight = goldReward.weight;

	if( !ValidateObject( pDropper.pack ))
	{
		if( ValidateObject( goldReward ))
			goldReward.Delete();

		socket.SysMessage( "You need a backpack to receive rewards." );
		return false;
	}

	var playerPack = pDropper.pack;
	var giveGoldAsCheck = false;
	var placeCheckInBank = false;
	var bankBox = null;

	if( goldToGive >= 5000 || ( playerPack.weight + goldWeight > playerPack.maxWeight ))
	{
		if( playerPack.weight + 100 > playerPack.maxWeight )
		{
			bankBox = pDropper.FindItemLayer( 29 );
			if( ValidateObject( bankBox ) && bankBox.isItem && ( bankBox.weight + 100 <= bankBox.maxWeight ))
				placeCheckInBank = true;
			else
			{
				socket.SysMessage( GetDictionaryEntry( 1385, socket.language ));
				if( ValidateObject( goldReward ))
					goldReward.Delete();
				return false;
			}
		}

		giveGoldAsCheck = true;
	}

	var amountCurrent = iDropped.GetTag( "amountCur" );
	if( amountCurrent <= 0 )
		amountCurrent = 1;

	var qualityValue = iDropped.GetTag( "qualityValue" );
	var averageBodItemQuality = Math.round( qualityValue / amountCurrent );
	var weightValue = ( 4 - (( averageBodItemQuality / 10 ) * 3.0 ));

	var coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));
	var droppedBodType = iDropped.GetTag( "bodType" );
	var rewards = null;

	if( coreShardEra <= EraStringToNum( "lbr" ))
		rewards = BODTypesToRewards[droppedBodType];
	else
		rewards = BODTypesToRewardsAoS[droppedBodType];

	if( !rewards || rewards.length == 0 )
	{
		console.error( "No reward table found for BOD type " + droppedBodType + "." );
		if( ValidateObject( goldReward ))
			goldReward.Delete();
		return false;
	}

	var minMaxModifiers = MinMaxRewardModifiers( iDropped, droppedBodType, rewards.length );

	var minReward = minMaxModifiers[0];
	var maxReward = ( rewards.length - 1 ) + minMaxModifiers[1];

	if( minReward < 0 )
		minReward = 0;
	if( maxReward < 0 )
		maxReward = 0;
	if( maxReward >= rewards.length )
		maxReward = rewards.length - 1;
	if( minReward > maxReward )
		minReward = maxReward;

	var rewardIndex = WeightedRandom( minReward, maxReward, weightValue );
	if( rewardIndex < 0 )
		rewardIndex = 0;
	if( rewardIndex >= rewards.length )
		rewardIndex = rewards.length - 1;

	var rewardTier = rewards[rewardIndex];
	if( !rewardTier || !rewardTier.items || rewardTier.items.length == 0 )
	{
		console.error( "Invalid BOD reward tier selected for player " + pDropper.serial + "." );
		if( ValidateObject( goldReward ))
			goldReward.Delete();
		return false;
	}

	var rewardItemIndex = 0;
	if( rewardTier.items.length > 1 )
	{
		switch( rewardTier.selectType )
		{
			case "random":
				rewardItemIndex = RandomNumber( 0, rewardTier.items.length - 1 );
				break;
			case "weighted":
				rewardItemIndex = WeightedRandom( 0, rewardTier.items.length - 1, weightValue );
				break;
		}
	}

	if( rewardItemIndex < 0 )
		rewardItemIndex = 0;
	if( rewardItemIndex >= rewardTier.items.length )
		rewardItemIndex = rewardTier.items.length - 1;

	var rewardItem = rewardTier.items[rewardItemIndex];
	if( !rewardItem || !rewardItem.itemName )
	{
		console.error( "Invalid BOD reward item selected for player " + pDropper.serial + "." );
		if( ValidateObject( goldReward ))
			goldReward.Delete();
		return false;
	}

	var rewardDFNItem = CreateDFNItem( socket, pDropper, rewardItem.itemName, 1, "ITEM", false );
	if( rewardItem.props && ValidateObject( rewardDFNItem ))
	{
		for( var propertyIndex = 0; propertyIndex < rewardItem.props.length; propertyIndex++ )
		{
			var propertyModifier = rewardItem.props[propertyIndex];
			var propertyToModify = propertyModifier[0];
			var propertyValue = propertyModifier[1];
			rewardDFNItem[propertyToModify] = propertyValue;
		}
	}

	var errorFound = false;
	if( !ValidateObject( rewardDFNItem ))
	{
		console.error( "Error occurred when attempting to create BOD item reward for player with serial " + pDropper.serial + "!" );
		errorFound = true;
	}

	if( !errorFound )
	{
		if( playerPack.weight + rewardDFNItem.weight + goldWeight > playerPack.maxWeight )
		{
			if( playerPack.weight + rewardDFNItem.weight + 100 > playerPack.maxWeight )
			{
				socket.SysMessage( GetDictionaryEntry( 1385, socket.language ));
				errorFound = true;
			}
			else
			{
				giveGoldAsCheck = true;
			}
		}
	}

	if( !errorFound )
	{
		rewardDFNItem.container = pDropper.pack;
		rewardDFNItem.PlaceInPack();

		console.log( "[BOD Reward] Special item reward (" + rewardDFNItem.name + " - " + rewardDFNItem.serial + ") given to player (" + pDropper.name + " - " + pDropper.serial + ")." );

		pDropper.fame = ( pDropper.fame + fameToGive > 10000 ? 10000 : pDropper.fame + fameToGive );
		if( fameToGive > 50 )
			socket.SysMessage( GetDictionaryEntry( 1377, socket.language ));
		else if( fameToGive > 25 )
			socket.SysMessage( GetDictionaryEntry( 1375, socket.language ));
		else if( fameToGive > 0 )
			socket.SysMessage( GetDictionaryEntry( 1373, socket.language ));

		if( !giveGoldAsCheck )
		{
			if( ValidateObject( goldReward ))
			{
				goldReward.container = pDropper.pack;
				goldReward.PlaceInPack();
				pDropper.SoundEffect( 0x3D, true );
			}
		}
		else
		{
			var newCheck = CreateDFNItem( socket, pDropper, "0x14F0", 1, "ITEM", !placeCheckInBank );
			if( ValidateObject( newCheck ))
			{
				newCheck.SetTag( "CheckSize", goldToGive );
				newCheck.name = "A bank check";
				newCheck.colour = 0x34;
				newCheck.AddScriptTrigger( bankCheckTrigger );
				newCheck.isNewbie = true;
				newCheck.weight = 100;

				if( placeCheckInBank && ValidateObject( bankBox ))
				{
					newCheck.container = bankBox;
					newCheck.PlaceInPack();
				}
			}
		}

		console.log( "[BOD Reward] Gold reward (" + goldToGive + ") given to player (" + pDropper.name + " - " + pDropper.serial + ")." );
	}

	if( errorFound )
	{
		if( ValidateObject( goldReward ))
			goldReward.Delete();

		if( ValidateObject( rewardDFNItem ))
			rewardDFNItem.Delete();

		return false;
	}

	return true;
}

/** @param {Item} iDropped @param {number} bodType @param {number} numTiers @returns {[number, number]} */
function MinMaxRewardModifiers( iDropped, bodType, numTiers )
{
	var minModifierPercent = 0;
	var maxModifierPercent = 0;

	var amountMax = iDropped.GetTag( "amountMax" );
	switch( amountMax )
	{
		case 10:
			maxModifierPercent -= 0.125;
			break;
		case 20:
			minModifierPercent += 0.0625;
			break;
	}

	var reqExceptional = iDropped.GetTag( "reqExceptional" );
	if( reqExceptional )
		minModifierPercent += 0.0625;
	else
		maxModifierPercent -= 0.125;

	if( bodType == 1 )
	{
		var materialColor = iDropped.GetTag( "materialColor" );
		switch( materialColor )
		{
			case 0: maxModifierPercent -= 0.25; break;
			case 0x973: maxModifierPercent -= 0.1875; break;
			case 0x966: maxModifierPercent -= 0.125; break;
			case 0x96E: maxModifierPercent -= 0.0625; break;
			case 0x8A5: minModifierPercent += 0.0625; break;
			case 0x979: minModifierPercent += 0.125; break;
			case 0x89F: minModifierPercent += 0.1875; break;
			case 0x8AB: minModifierPercent += 0.25; break;
		}
	}

	return [
		Math.floor( minModifierPercent * numTiers ),
		Math.floor( maxModifierPercent * numTiers )
	];
}