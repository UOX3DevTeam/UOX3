// This script handles BOD offers from shopkeepers
const offerBodsFromItemSales = GetServerSetting( "OfferBODsFromItemSales" ); // If true, BODs can be offered by vendor upon selling items to them
const offerBodsFromContextMenu = GetServerSetting( "OfferBODsFromContextMenu" ); // Not present in LBR, disabled by default. Needs to be a server setting
const onlyOfferBodsFromCraftedItems = GetServerSetting( "BodsFromCraftedItemsOnly" ); // If true, only crafted items being sold can trigger BOD offers

// Multiplier for gold rewards given upon completion of BOD (2.0 equals 2 times the gold rewards)
const goldRewardMultiplier = GetServerSetting( "BODGoldRewardMultiplier" ); // Defaults to 1.0;

// Multiplier for fame rewards given upon completion of BOD (2.0 would equal 2 times the fame rewards)
const fameRewardMultiplier = GetServerSetting( "BODFameRewardMultiplier" ); // Defaults to 1.0;

// If crafting coloured weapons is allowed, also include them in the BOD requests
const canCraftColouredWeapons = GetServerSetting( "CraftColouredWeapons" );

// Weapon CreateEntries Table
const weaponCreateEntries = [
	// Iron, Dull Copper, Shadow Iron, Copper, Bronze, Gold, Agapite, Verite, Valorite
	// TODO: Set up support for smithing colored weapons (AoS and beyond)
	[20, 20, 20, 20, 20, 20, 20, 20, 20], 		// Dagger
	[40, 40, 40, 40, 40, 40, 40, 40, 40], 		// Mace
	[41, 41, 41, 41, 41, 41, 41, 41, 41],		// Maul
	[21, 21, 21, 21, 21, 21, 21, 21, 21],		// Cutlass
	[27, 27, 27, 27, 27, 27, 27, 27, 27],		// Viking Sword
	[44, 44, 44, 44, 44, 44, 44, 44, 44],		// Hammer Pick
	[26, 26, 26, 26, 26, 26, 26, 26, 26],		// Longsword
	[42, 42, 42, 42, 42, 42, 42, 42, 42],		// War Mace
	[32, 32, 32, 32, 32, 32, 32, 32, 32],		// Double Axe
	[33, 33, 33, 33, 33, 33, 33, 33, 33],		// Large Battle Axe
	[28, 28, 28, 28, 28, 28, 28, 28, 28],		// Battle Axe
	[24, 24, 24, 24, 24, 24, 24, 24, 24],		// Scimitar
	[31, 31, 31, 31, 31, 31, 31, 31, 31],		// Two Handed Axe
	[43, 43, 43, 43, 43, 43, 43, 43, 43],		// War Hammer
	[25, 25, 25, 25, 25, 25, 25, 25, 25],		// Broadsword
	[29, 29, 29, 29, 29, 29, 29, 29, 29],		// Axe
	[30, 30, 30, 30, 30, 30, 30, 30, 30],		// Executioner's Axe
	[23, 23, 23, 23, 23, 23, 23, 23, 23],		// Kryss
	[37, 37, 37, 37, 37, 37, 37, 37, 37],		// War Fork
	[34, 34, 34, 34, 34, 34, 34, 34, 34],		// War Axe
	[22, 22, 22, 22, 22, 22, 22, 22, 22],		// Katana
	[35, 35, 35, 35, 35, 35, 35, 35, 35],		// Short Spear
	[38, 38, 38, 38, 38, 38, 38, 38, 38],		// Bardiche
	[36, 36, 36, 36, 36, 36, 36, 36, 36],		// Spear
	[39, 39, 39, 39, 39, 39, 39, 39, 39]		// Halberd
];

const armorCreateEntries = [
	// Iron, Dull Copper, Shadow Iron, Copper, Bronze, Gold, Agapite, Verite, Valorite
	[1, 500, 600, 700, 800, 900, 1200, 1000, 1100],		// Buckler
	[2, 501, 601, 701, 801, 901, 1201, 1001, 1101],		// Bronze Shield
	[3, 502, 602, 702, 802, 902, 1202, 1002, 1102], 	// Metal Shield
	[4, 503, 603, 703, 803, 903, 1203, 1003, 1103],		// Tear Kite Shield
	[5, 504, 604, 704, 804, 904, 1204, 1004, 1104],		// Metal Kite Shield
	[6, 505, 605, 705, 805, 905, 1205, 1005, 1105],		// Heater Shield
	[7, 506, 606, 706, 806, 906, 1206, 1006, 1106],		// Ringmail Gloves
	[8, 507, 607, 707, 807, 907, 1207, 1007, 1107],		// Ringmail Sleeves
	[9, 508, 608, 708, 808, 908, 1208, 1008, 1108], 	// Ringmail Leggings
	[10, 509, 609, 709, 809, 909, 1209, 1009, 1109],	// Ringmail Tunic
	[11, 510, 610, 710, 810, 910, 1210, 1010, 1110], 	// Chainmail Coif
	[12, 511, 611, 711, 811, 911, 1211, 1011, 1111], 	// Chainmail Leggings
	[13, 512, 612, 712, 812, 912, 1212, 1012, 1112],	// Chainmail Tunic
	[14, 513, 613, 713, 813, 913, 1213, 1013, 1113],	// Platemail Gorget
	[15, 514, 614, 714, 814, 914, 1214, 1014, 1114], 	// Platemail Gloves
	[16, 515, 615, 715, 815, 915, 1215, 1015, 1115],	// Platemail Arms
	[17, 516, 616, 716, 816, 916, 1216, 1016, 1116],	// Platemail Legs
	[18, 517, 617, 717, 817, 917, 1217, 1017, 1117],	// Platemail Tunic
	[19, 518, 618, 718, 818, 918, 1218, 1018, 1118],	// Platemail (female)
	[45, 519, 619, 719, 819, 919, 1219, 1019, 1119],	// Helmet
	[46, 520, 620, 720, 820, 920, 1220, 1020, 1120], 	// Bascinet
	[47, 521, 621, 721, 821, 921, 1221, 1021, 1121],	// Norse Helm
	[48, 522, 622, 722, 822, 922, 1222, 1022, 1122],	// Close Helm
	[49, 523, 623, 723, 823, 923, 1223, 1023, 1123]		// Plate Helm
];

const clothCreateEntries = [
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
const BODTypesToCreateEntries = {
	1: { // Blacksmithing
		1: weaponCreateEntries,
		2: armorCreateEntries,
		3: weaponCreateEntries.concat(armorCreateEntries),
	},
	2: { // Tailoring
		1: clothCreateEntries
	}
};

const BODTypeToDFNSectionID = {
	1: "smallbod_blacksmith",
	2: "smallbod_tailor"
};

const BODTypesToSkillNames = {
	1: "blacksmithing",
	2: "tailoring"
};

// A higher index in this list means the reward is less likely to be given.
// Each tier may have an `items` property listing the items that may be given when it is rolled
// and a `selectType` property that may either be "random" or "weighted" determining how an
// item is chosen, if there is more than one. Objects in the `items` list MUST have an
// `itemName` property and may optionally have a `props` property specifying modifications
// to be made to the item's props.
const BlacksmithRewardTiersToItems = [
	{
		items: [
			{ itemName: "sturdy_pickaxe", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "sturdy_shovel", props: [["maxUses", 200], ["usesLeft", 200]] },
			{ itemName: "sturdy_pickaxe", props: [["maxUses", 150], ["usesLeft", 150]] },
			{ itemName: "sturdy_shovel", props: [["maxUses", 150], ["usesLeft", 150]] },
		],
		selectType: "random",
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

// See comment above BlacksmithRewardTiersToItems for explanation of object properties.
const TailorRewardTiersToItems = [
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

const BODTypesToRewards = {
	1: BlacksmithRewardTiersToItems,
	2: TailorRewardTiersToItems
};

function onSoldToVendor( pSock, npcVendor, iSold )
{
	var pUser = pSock.currentChar;
	if( offerBodsFromItemSales && CheckBodTimers( pUser, npcVendor.GetTag( "bodType" ) ) )
	{
		if( !onlyOfferBodsFromCraftedItems || iSold.madeWith != -1 ) // what is madeWith property for non-crafted items?
		{
			// Player can receive another BOD
			// Roll the dice!
			if( RandomNumber( 1, 5 ) == 5 )
			{
				SmallBODAcceptGump( pUser, npcVendor );
			}
		}
	}

	return false;
}

function CheckBodTimers( pUser, bodType )
{
	var bodOfferCD = pUser.GetJSTimer( bodType, 3214 ); // Fetch timer for BOD offer cooldown
	if( bodOfferCD != 0 )
	{
		// BOD cooldown still in effect - let's check the timer
		pUser.SysMessage( GetDictionaryEntry( 17265, pUser.socket.language ) + " " + (( bodOfferCD - GetCurrentClock() ) / 60000 ).toFixed( 0 ) + " " + GetDictionaryEntry( 17266, pUser.socket.language )); // An Offer may be available in about X minutes
		return false;
	}
	else
	{
		// Player is ready to accept another BOD
		return true;
	}
}

function onSpeech( myString, pUser, myNPC )
{
	if( !ValidateObject( pUser ) || !ValidateObject( myNPC ) || !offerBodsFromContextMenu || !pUser.InRange( myNPC, 8 ))
		return false;

	var socket = pUser.socket;
	if( socket == null )
		return false;

	// See if a vendor serial was stored via hard-coded context menu, to exclude other NPCs reaction to speech trigger
	var targetShopkeeper = CalcCharFromSer( parseInt( pUser.GetTempTag( "bodShopkeeperSerial" )));
	if( !ValidateObject( targetShopkeeper ) || targetShopkeeper != myNPC )
	{
		return false;
	}
	pUser.SetTempTag( "bodShopkeeperSerial", null );

	// Check the trigger words to see if the Bulk Order Info trigger word was sent
	for( var trigWord = socket.FirstTriggerWord(); !socket.FinishedTriggerWords(); trigWord = socket.NextTriggerWord() )
	{
		switch( trigWord )
		{
			case 0x5000: // Bulk Order Info - TW_BODINFO, custom UOX3 triggerword triggered via context menu
			{
				if( CheckBodTimers( pUser, myNPC.GetTag( "bodType" ) ))
				{
					if( EraStringToNum( GetServerSetting( "CoreShardEra" )) >= EraStringToNum( "lbr" ))
					{
						myNPC.SetTimer( Timer.MOVETIME, 1000 ); // Pause NPC in their tracks for a second
						myNPC.TurnToward( pUser );
						SmallBODAcceptGump( pUser, myNPC );
						return 1;
					}
				}
			}
		}
	}

	return false;
}

function SmallBODAcceptGump( pUser, myNPC )
{
	var socket        = pUser.socket;
	var bodType 	  = myNPC.GetTag( "bodType" );
	const bodSubtype  = myNPC.GetTag( "bodSubtype" );
	const pSkill      = pUser.skills[BODTypesToSkillNames[bodType]]; // The player's level of the BOD's relevant skill.
	var bodEntry 	  = SelectBodEntry( bodType, bodSubtype, true, pSkill );
	var itemName	  = bodEntry.name; // name of the create entry
	var graphicID	  = bodEntry.id; // graphical ID of item to craft
	var materialColor = bodEntry.resources[0][1]; // colour of primary resource required to craft item

	// Store bodEntry as custom object property on pUser
	pUser.bodEntry = bodEntry;
	pUser.bodNPC = myNPC;

	var amountMax = 0;
	var reqExceptional = false;

	var init = bodEntry.tempInit;
	if( init != 1 ) // Keep from resetting the amount needed.
	{
		if( pSkill >= 700 )
		{
			if((( pSkill + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
			{
				reqExceptional = true;
			}

			let values = [ 10, 15, 20, 20 ];
			amountMax = values[Math.floor( Math.random() * values.length )];
		}
		else if( pSkill >= 500 )
		{
			let values = [ 10, 15, 15, 20 ];
			amountMax = values[Math.floor( Math.random() * values.length )];
		}
		else
		{
			let values = [ 10, 10, 15, 20 ];
			amountMax = values[Math.floor( Math.random() * values.length )];
		}

		pUser.bodEntry.tempReqExceptional = reqExceptional;
		pUser.bodEntry.tempAmountMax = amountMax;
		pUser.bodEntry.tempBodType = bodType;
		pUser.bodEntry.tempBodSubtype = bodSubtype;
		pUser.bodEntry.tempInit = true;
	}
	else
	{
		amountMax = pUser.bodEntry.tempAmountMax;
		reqExceptional = pUser.bodEntry.tempReqExceptional;
		pUser.bodEntry.tempBodType = bodType;
		pUser.bodEntry.tempBodSubtype = bodSubtype;
	}

	var bodGump = new Gump;
	bodGump.AddPage( 0 );
	bodGump.AddBackground( 25, 10, 430, 264, 5054 );

    bodGump.AddTiledGump( 33, 20, 413, 245, 2624 );
    bodGump.AddCheckerTrans( 33, 20, 413, 245 );

    bodGump.AddGump( 20, 5, 10460 );
    bodGump.AddGump( 430, 5, 10460 );
    bodGump.AddGump( 20, 249, 10460 );
    bodGump.AddGump( 430, 249, 10460 );

	bodGump.AddHTMLGump( 190, 25, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17250, socket.language ) + "</basefont>" ); // A bulk order
	bodGump.AddHTMLGump( 40, 48, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17267, socket.language ) + "</basefont>" ); // Ah!  Thanks for the goods!  Would you help me out?

	bodGump.AddHTMLGump( 40, 72, 210, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>" ); // Amount to make:
	bodGump.AddText( 250, 72, 1152, amountMax );

	bodGump.AddHTMLGump( 40, 96, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" ); // Items requested:
	bodGump.AddPicture( 325, 96, graphicID);// image of the item requested
	bodGump.AddHTMLGump( 40, 120, 210, 20, 0, 0, "<basefont color=#ffffff>" + itemName + "</basefont>" );// itemName of item to make

	if( reqExceptional || materialColor > 0 )
	{
		bodGump.AddHTMLGump( 40, 144, 210, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" ); // Special requirements to meet:
	}

	if( reqExceptional )
	{
		bodGump.AddHTMLGump( 40, 168, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>"); // All items must be exceptional.
	}

	if( materialColor > 0 ) // checks to see if a material is required for the bod.
	{
		if( reqExceptional )
		{
			var y = 192;
		}
		else
		{
			var y = 168;
		}
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
			default:
				materialName = "iron";
				break;
		}
		if( materialName != "" )
		{
			bodGump.AddHTMLGump( 40, y, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" );
		}
	}
	bodGump.AddHTMLGump( 40, 216, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17268, socket.language ) + "</basefont>" ); // Do you want to accept this order?

    bodGump.AddButton( 100, 240, 4005, 4007, 1, 0, 1 );
    bodGump.AddHTMLGump( 135, 240, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17269, socket.language ) + "</basefont>" ); // Ok

    bodGump.AddButton( 275, 240, 4005, 4007, 1, 0, 0 );
    bodGump.AddHTMLGump( 310, 240, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 2709, socket.language ) + "</basefont>" ); // CANCEL

	bodGump.Send( pUser );
	bodGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var bodEntry = pUser.bodEntry;
	if( typeof( bodEntry ) == "undefined" )
		return;

	var bodNPC = pUser.bodNPC;
	if( typeof( bodNPC ) == "undefined" || !ValidateObject( bodNPC ))
		return;

	// Fetch some properties from the bodEntry
	var amountMax = bodEntry.tempAmountMax;
	var reqExceptional = bodEntry.tempReqExceptional;
	var bodType = bodEntry.tempBodType;
	const bodSubtype = bodEntry.tempBodSubtype;
	var graphicID = bodEntry.id;
	var itemName = bodEntry.name;
	var materialColor = bodEntry.resources[0][1];
	var bodSectionID = bodEntry.addItem;

	switch( pButton )
	{
		case 0: // Cancel BOD Offer
			break;
		case 1: // Accept BOD Offer
			var playerPack = pUser.pack;
			if( playerPack.totalItemCount >= playerPack.maxItems )
			{
				socket.SysMessage( GetDictionaryEntry( 1819, socket.language )); // Your backpack cannot hold any more items!

				// Remove temporary reference to bodEntry on pUser
				delete pUser.bodEntry;
				delete pUser.bodNPC;

				return;
			}
			else
			{
				var smallBOD = CreateDFNItem( pUser.socket, pUser, BODTypeToDFNSectionID[bodType], 1, "ITEM", true );
				if( ValidateObject( smallBOD ))
				{
					// Store the BOD properties as permanent tags on the BOD deed
					smallBOD.SetTag( "itemName", itemName );
					smallBOD.SetTag( "graphicID", graphicID );
					smallBOD.SetTag( "amountMax", amountMax );
					smallBOD.SetTag( "reqExceptional", reqExceptional );
					smallBOD.SetTag( "materialColor", materialColor );
					smallBOD.SetTag( "bodSectionID", bodSectionID );
					smallBOD.SetTag( "bodSubtype", bodSubtype );
					smallBOD.SetTag( "init", true );

					pUser.TextMessage( GetDictionaryEntry( 17274, socket.language ), false, 0x3b2, 0, pUser.serial ); // The bulk order deed has been placed in your backpack.
				}
			}
			break;
		default:
			break;
	}

	// Remove temporary reference to bodEntry on pUser
	delete pUser.bodEntry;

	// Regardless of whether player accepts or declines the BOD, apply cooldown until next time
	// they can get an offer for another BOD
	SetBODAcceptanceCooldown( pUser, bodType );
}

function SetBODAcceptanceCooldown( pUser, bodType )
{
	const pSkill = pUser.skills[BODTypesToSkillNames[bodType]];

	// Set BOD cooldown timer for next time player can accept a BOD
	var bodTimer = 0;
	if( pSkill >= 700 )
	{
		bodTimer = 21600000; // 1000 * 60 * 60 * 6 = 6 hours if over or equal to 70.0 skill
	}
	else if( pSkill >= 501 )
	{
		bodTimer = 7200000; // 1000 * 60 * 60 * 2 = 2 hours if over 50.1 and under 70.0 skill
	}
	else
	{
		bodTimer = 3600000; // 1000 * 60 * 60 = 1 hour if under or equal to 50.0 skill
	}

	pUser.StartTimer( bodTimer, bodType, true );
}

function SelectBodEntry( bodType, bodSubtype, considerPlayerSkill, pSkill )
{
	const bodItemEntries = BODTypesToCreateEntries[bodType][bodSubtype];
	const maxMaterialIndex = bodItemEntries[0].length - 1; // Assumes all entries can be made from the same resources.

	let materialIndex = 0;
	if( canCraftColouredWeapons )
	{
		materialIndex = RandomNumber( 0, maxMaterialIndex );
		if( considerPlayerSkill && pSkill >= ( 1000 - ( 50 * ( maxMaterialIndex - materialIndex ) ) ) )
		{
			materialIndex = 0;
		}
	}

	// Find all valid create entries for player's current skill level
	const validItems = [];
	for( let i = 0; i < bodItemEntries.length; i++ )
	{
		const item = bodItemEntries[i];
		if( considerPlayerSkill && pSkill < 700 )
		{
			// Only select items which player can gain skill from if skill is below 70.0
			const createEntry = CreateEntries[item[materialIndex]];
			if( createEntry.avgMinSkill <= pSkill && pSkill <= createEntry.avgMaxSkill )
			{
				validItems.push( item[materialIndex] );
			}
		}
		else
		{
			// All create entries are valid if player's skill is above or equal to 70.0
			validItems.push( item[materialIndex] );
		}
	}

	// Select a random create entry from array of valid entries
	const rndCreateIndex = Math.floor( Math.random() * validItems.length );
	return CreateEntries[validItems[rndCreateIndex]];
}

function onDropItemOnNpc( pDropper, npcDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	if( socket == null )
		return false;

	var amountMax 	  = iDropped.GetTag( "amountMax" ); 	 // amount you have to make of the item
	var amountCur 	  = iDropped.GetTag( "amountCur" ); 	 // amount you have combined
	var iBodType 	  = iDropped.GetTag( "bodType" ); 	     // BOD type of the BOD itself
	var pBodType 	  = npcDroppedOn.GetTag( "bodType" );    // BOD type of the NPC BOD is dropped on, if any

	if( iDropped.sectionID.split("_")[0] == "smallbod" && pBodType > 0 )
	{
		// Check if NPC accepts the type of BOD being dropped on them
		if( iBodType != pBodType )
		{
			// That order is for some other shopkeeper.
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17272, socket.language ), false, 0x3b2, 0, pDropper.serial ); // That order is for some other shopkeeper.
			return false;
		}

		if( amountCur < amountMax )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17270, socket.language ), false, 0x3b2, 0, pDropper.serial ); // You have not completed the order yet.
			return false;
		}

		// Check if enough time has passed since last time player handed in a BOD
		var bodRewardCD = pDropper.GetJSTimer( iBodType * 10, 3214 ); // Fetch timer for BOD reward cooldown
		if( bodRewardCD != 0 )
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17273, socket.language ), false, 0x3b2, 0, pDropper.serial ); // You'll have to wait a few seconds while I inspect the last order.
			return false;
		}

		// Does player's backpack have space for the reward items (gold & special item?
		var playerPack = pDropper.pack;
		if( playerPack.totalItemCount >= ( playerPack.maxItems - 2 ))
		{
			npcDroppedOn.TextMessage( GetDictionaryEntry( 17275, socket.language ), false, 0x3b2, 0, pDropper.serial ); // You do not have enough room in your backpack for the bulk request's reward.
			return false;
		}

		if( DispenseBODRewards( pDropper, npcDroppedOn, iDropped ))
		{
			// Make the player wait 10 seconds before turning in another BOD.
			pDropper.StartTimer( 10000, iBodType * 10, true );
			// On delivery of a completed BOD, kill cooldown timer to get another BOD offer
			var bodOfferCD = pDropper.GetJSTimer( iBodType, 3214 ); // Fetch timer for BOD offer cooldown
			if( bodOfferCD != 0 )
			{
				pDropper.KillJSTimer( iBodType, 3214 );
			}

			iDropped.Delete();
			return 2;
		}
	}

	// Otherwise, reject the item drop and bounce it back where it came from
	return false;
}

// Weighted randomizer that can skew probabilities towards lower values being selected based on weight
// Higher weight, more heavy trend towards min value
function WeightedRandom( min, max, weight )
{
	var n = Math.random();
	return Math.round( min + (( max - min ) * Math.pow( n, weight )));
}

function DispenseBODRewards( pDropper, npcDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	npcDroppedOn.TextMessage( GetDictionaryEntry( 17271, socket.language ), false, 0x3b2, 0, pDropper.serial ); // Thank you so much!  Here is a reward for your effort.

	// FAME REWARD
	// Total fame reward is calculated continuously as additional items are added to the BOD
	// See js/item/smallbod.js for details
	var fameToGive = iDropped.GetTag( "fameValue" ) * fameRewardMultiplier;

	// GOLD REWARD
	// Total gold reward is calculated continuously as additional items are added to the BOD
	// See js/item/smallbod.js for details
	var goldToGive = iDropped.GetTag( "goldValue" ) * goldRewardMultiplier;
	var goldReward = CreateDFNItem( socket, pDropper, "0x0EED", goldToGive, "ITEM", false );
	var goldWeight = 0;
	if( ValidateObject( goldReward ))
	{
		goldWeight = goldReward.weight;
	}

	// Before giving the gold, ensure that player has room for it in their pack (or for check in pack/bank)
	var playerPack = pDropper.pack;
	var giveGoldAsCheck = false;
	var placeCheckInBank = false;

	if( goldToGive >= 5000 || ( playerPack.weight + goldWeight > playerPack.maxWeight ))
	{
		if( playerPack.weight + 100 > playerPack.maxWeight )
		{
			// Can it fit in bankbox?
			var bankBox = pDropper.FindItemLayer( 29 );
			if( ValidateObject( bankBox ) && bankBox.isItem && ( bankBox.weight + 100 <= bankBox.maxWeight ))
			{
				placeCheckInBank = true;
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 1385, socket.language )); // That pack cannot hold any more weight
				goldReward.Delete();
				return false;
			}
		}

		giveGoldAsCheck = true;
	}

	// ITEM REWARD
	// Use average item quality to calculate the "weight" the reward system should put towards trending to LOW quality rewards
	// The lower the average quality of the items in the BOD, the more heavily it trends towards the low rewards
	var avgBodItemQuality = Math.round( iDropped.GetTag( "qualityValue" ) / iDropped.GetTag( "amountCur" ));
	var weightVal = ( 4 - (( avgBodItemQuality / 10 ) * 3.0 ));

	const rewards = BODTypesToRewards[iDropped.GetTag( "bodType" )];

	// Get modifiers to min / max rewards based on properties of the BOD itself
	var minMaxMod = MinMaxRewardModifiers( iDropped, iDropped.GetTag( "bodType" ), rewards.length);

	const minReward = minMaxMod[0];
	const maxReward = rewards.length - 1 + minMaxMod[1];

	const rewardTier = rewards[WeightedRandom( minReward, maxReward, weightVal )];
	let rewardItemIndex = 0;
	if ( rewardTier.items.length > 1 ) {
		switch ( rewardTier.selectType ) {
			case "random":
				rewardItemIndex = RandomNumber(0, rewardTier.items.length - 1);
				break;
			case "weighted":
				rewardItemIndex = WeightedRandom(0, rewardTier.items.length - 1, weightVal);
				break;
			default:
				break;
		}
	}
	const rewardItem = rewardTier.items[rewardItemIndex];
	const rewardDFNItem = CreateDFNItem( socket, pDropper, rewardItem.itemName, 1, "ITEM", false );
	if ( rewardItem.props && rewardDFNItem )
	{
		for( let i = 0; i < rewardItem.props.length; i++ )
		{
			const propModifier = rewardItem.props[i];
			const propToModify = propModifier[0];
			const propValue = propModifier[1];

			rewardDFNItem[propToModify] = propValue;
		}
	}

	var errorFound = false;
	if( !ValidateObject( rewardDFNItem ))
	{
		socket.SysMessage( "An error occurred while attempting to dispense rewards for BOD. Please contact a GM/Admin for assistance!" );
		Console.Error( "Error occured when attempting to create BOD item reward for player with serial " + pDropper.serial + "!" );
		errorFound = true;
	}

	if( !errorFound )
	{
		// Can player's pack hold weight of the special reward item AND the gold/check?
		if( playerPack.weight + rewardDFNItem.weight + goldWeight > playerPack.maxWeight ) // special item + gold
		{
			if( playerPack.weight + rewardDFNItem.weight + 100 > playerPack.maxWeight ) // special item + check
			{
				socket.SysMessage( GetDictionaryEntry( 1385, socket.language )); // That pack cannot hold any more weight
				errorFound = true;
				//return false;
			}
			else
			{
				// Special item reward weight can fit if we give the gold as a check
				giveGoldAsCheck = true;
			}
		}
	}

	if( !errorFound )
	{
		// All good! Hand out the rewards:
		// SPECIAL ITEM
		rewardDFNItem.container = pDropper.pack;
		rewardDFNItem.PlaceInPack();

		// Log creation of gold in server logs
		Console.Log( "[BOD Reward] Special item reward (" + rewardDFNItem.name + " - " + rewardDFNItem.serial + ") given to player (" + pDropper.name + " - " + pDropper.serial + ")." );

		// FAME
		// Ensure player's fame doesn't exceed the default cap of 10k
		pDropper.fame = ( pDropper.fame + fameToGive > 10000 ? 10000 : pDropper.fame + fameToGive );
		if( fameToGive > 50 )
		{
			socket.SysMessage( GetDictionaryEntry( 1377, socket.langauge )); // You have gained a lot of fame.
		}
		else if( fameToGive > 25 )
		{
			socket.SysMessage( GetDictionaryEntry( 1375, socket.langauge )); // You have gained some fame.
		}
		else if( fameToGive > 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 1373, socket.langauge )); // You have gained a little fame.
		}

		// GOLD
		if( !giveGoldAsCheck )
		{
			// Deposit gold directly in player's backpack
			goldReward.container = pDropper.pack;
			goldReward.PlaceInPack();
			pDropper.SoundEffect( 0x3D, true );

		}
		else
		{
			// Too much gold; write them a cheque, and place it in their bank if backpack is too heavy
			var newCheck = CreateDFNItem( socket, pDropper, "0x14F0", 1, "ITEM", !placeCheckInBank ); // Add check
			if( newCheck )
			{
				newCheck.SetTag( "CheckSize", goldToGive );
				newCheck.name = "A bank check";
				newCheck.colour = 0x34;
				newCheck.AddScriptTrigger( bankCheckTrigger );
				newCheck.isNewbie = true;
				newCheck.weight = 100;

				if( placeCheckInBank )
				{
					// Place check in bankbox
					newCheck.container = bankBox;
					newCheck.PlaceInPack();
				}
			}
		}

		// Log creation of gold in server logs
		Console.Log( "[BOD Reward] Gold reward (" + goldToGive + ") given to player (" + pDropper.name + " - " + pDropper.serial + ")." );
	}

	if( errorFound )
	{
		// Cleanup potential gold reward
		if( ValidateObject( goldReward ))
		{
			goldReward.Delete();
		}

		// Cleanup potential item reward
		if( ValidateObject( rewardDFNItem ))
		{
			rewardDFNItem.Delete();
		}
		return false;
	}

	return true;
}

function MinMaxRewardModifiers( iDropped, bodType, numTiers )
{
	let minModPercent = 0;
	let maxModPercent = 0;

	// Apply bonus to min or penalty to max reward based on amount of items in BOD
	var amountMax = iDropped.GetTag( "amountMax" );
	switch( amountMax )
	{
		case 10:
			maxModPercent -= 0.125;
			break;
		case 15: // No change
			break;
		case 20:
			minModPercent += 0.0625;
			break;
	}

	// Apply bonus/penalty if exceptional items required by BOD
	var reqExceptional = iDropped.GetTag( "reqExceptional" );
	if( reqExceptional )
	{
		minModPercent += 0.0625;
	}
	else
	{
		maxModPercent -= 0.125;
	}

	// Apply bonus/penalty if special material required by BOD (replace with bonus/penalty based on specific color rarities?)
	if( bodType == 1 )
	{
		var materialColor = iDropped.GetTag( "materialColor" );
		switch (materialColor) {
			case 0: // Iron
				maxModPercent -= 0.25;
				break;
			case 0x973: // Dull Copper
				maxModPercent -= 0.1875;
				break;
			case 0x966: // Shadow Iron
				maxModPercent -= 0.125;
				break;
			case 0x96E: // Copper
				maxModPercent -= 0.0625;
				break;
			case 0x6D6: // Bronze
						// No change
				break;
			case 0x8A5: // Gold
				minModPercent += 0.0625;
				break;
			case 0x979: // Agapite
				minModPercent += 0.125;
				break;
			case 0x89F: // Verite
				minModPercent += 0.1875;
				break;
			case 0x8AB: // Valorite
				minModPercent += 0.25;
				break;
			default:
				break;
		}
	}

	return [Math.floor( minModPercent * numTiers ), Math.floor( maxModPercent * numTiers )];
}
