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

function onSoldToVendor( pSock, npcVendor, iSold )
{
	var pUser = pSock.currentChar;
	if( offerBodsFromItemSales && CheckBodTimers( pUser ))
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

function CheckBodTimers( pUser )
{
	var bodOfferCD = pUser.GetJSTimer( 1, 3214 ); // Fetch timer for BOD offer cooldown
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
	pUser.SetTempTag( "bodShopkeeperSerial", null );
	if( ValidateObject( targetShopkeeper ) && targetShopkeeper != myNPC )
	{
		return false;
	}

	// Check the trigger words to see if the Bulk Order Info trigger word was sent
	for( var trigWord = socket.FirstTriggerWord(); !socket.FinishedTriggerWords(); trigWord = socket.NextTriggerWord() )
	{
		switch( trigWord )
		{
			case 0x5000: // Bulk Order Info - TW_BODINFO, custom UOX3 triggerword triggered via context menu
			{
				if( CheckBodTimers( pUser ))
				{
					if( EraStringToNum( GetServerSetting( "CoreShardEra" )) <= EraStringToNum( "lbr" ))
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
	var socket 		= pUser.socket;
	var bodType 	= myNPC.GetTag( "bodType" );
	var bodEntry 	= SelectBodEntry( pUser, true, bodType );
	var itemName	= bodEntry.name; // name of the create entry
	var graphicID	= bodEntry.id; // graphical ID of item to craft
	var sectionID 	= bodEntry.addItem; // section ID of item to craft
	var materialColor = bodEntry.resources[0][1]; // colour of primary resource required to craft item
	var weaponType  = TriggerEvent( 2500, "GetWeaponType", null, bodEntry.id );

	// Store bodEntry as custom object property on pUser
	pUser.bodEntry = bodEntry;
	pUser.bodNPC = myNPC;

	var amountMax = 0;
	var reqExceptional = false;

	var init = bodEntry.tempInit;
	if( init != 1 ) // Keep from resetting the amount needed.
	{
		if( pUser.skills.blacksmithing >= 700 )
		{
			if((( pUser.skills.blacksmithing + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
			{
				reqExceptional = false;
			}

			let values = [ 10, 15, 20, 20 ];
			amountMax = values[Math.floor( Math.random() * values.length )];
		}
		else if( pUser.skills.blacksmithing >= 500 )
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
		pUser.bodEntry.tempInit = true;
	}
	else
	{
		amountMax = pUser.bodEntry.tempAmountMax;
		reqExceptional = pUser.bodEntry.tempReqExceptional;
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
				var smallBOD = CreateDFNItem( pUser.socket, pUser, "smallbod", 1, "ITEM", true );
				if( ValidateObject( smallBOD ))
				{
					// Store the BOD properties as permanent tags on the BOD deed
					smallBOD.SetTag( "itemName", itemName );
					smallBOD.SetTag( "graphicID", graphicID );
					smallBOD.SetTag( "amountMax", amountMax );
					smallBOD.SetTag( "reqExceptional", reqExceptional );
					smallBOD.SetTag( "materialColor", materialColor );
					smallBOD.SetTag( "bodSectionID", bodSectionID );
					smallBOD.SetTag( "bodType", bodType );
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
	SetBODAcceptanceCooldown( pUser );
}

function SetBODAcceptanceCooldown( pUser )
{
	// Set BOD cooldown timer for next time player can accept a BOD
	var bodTimer = 0;
	if( pUser.skills.blacksmithing >= 700 )
	{
		bodTimer = 21600000; // 1000 * 60 * 60 * 6 = 6 hours if over or equal to 70.0 skill
	}
	else if( pUser.skills.blacksmithing >= 501 )
	{
		bodTimer = 7200000; // 1000 * 60 * 60 * 2 = 2 hours if over 50.1 and under 70.0 skill
	}
	else
	{
		bodTimer = 3600000; // 1000 * 60 * 60 = 1 hour if under or equal to 50.0 skill
	}

	pUser.StartTimer( bodTimer, 1, true );
}

function SelectBodEntry( pUser, considerPlayerSkill, bodType )
{
	var pSkill = considerPlayerSkill ? pUser.skills.blacksmithing : 0;

	// First, determine if the BOD should contain requests for weapons, or armors
	// bodType is generally supplied by a tag on the NPC shopkeeper
	// weaponsmith = bodyType 1, armorsmith = bodType 2, blacksmith = bodType 3
	// These are also used to determine which NPCs accept completed BODs
	var rndItemType = -1;
	switch( bodType )
	{
		case 1: // Weapon BODs only
			rnditemType = 1;
			break;
		case 2: // Armor BODs only
			rndItemType = 2;
			break;
		case 3: // Either weapon or armor BODs
			rndItemType = RandomNumber( 1, 2 );
			break;
		// Add additional BOD types here if/when implemented
		default:
			break;
	}

	if( rndItemType == 1 ) // 1 - Weapon
	{
		var materialIndex = 0;
		if( canCraftColouredWeapons )
		{
			materialIndex = RandomNumber( 0, 8 );
			if( considerPlayerSkill )
			{
				switch( materialIndex )
				{
					case 0: // Iron
						break;
					case 1: // Dull Copper
						materialIndex = ( pUser.skills.blacksmithing >= 650 ? materialIndex : 0 );
						break;
					case 2: // Shadow Iron
						materialIndex = ( pUser.skills.blacksmithing >= 700 ? materialIndex : 0 );
						break;
					case 3: // Copper
						materialIndex = ( pUser.skills.blacksmithing >= 750 ? materialIndex : 0 );
						break;
					case 4: // Bronze
						materialIndex = ( pUser.skills.blacksmithing >= 800 ? materialIndex : 0 );
						break;
					case 5: // Gold
						materialIndex = ( pUser.skills.blacksmithing >= 850 ? materialIndex : 0 );
						break;
					case 6: // Agapite
						materialIndex = ( pUser.skills.blacksmithing >= 900 ? materialIndex : 0 );
						break;
					case 7: // Verite
						materialIndex = ( pUser.skills.blacksmithing >= 950 ? materialIndex : 0 );
						break;
					case 8: // Valorite
						materialIndex = ( pUser.skills.blacksmithing >= 1000 ? materialIndex : 0 );
						break;
					default:
						break;
				}
			}
		}

		// Find all valid create entries for player's current skill level
		var validWeapons = [];
		for( var i = 0; i < weaponCreateEntries.length; i++ )
		{
			var item = weaponCreateEntries[i];
			if( considerPlayerSkill && pSkill < 700 )
			{
				// Only select items which player can gain skill from if skill is below 70.0
				var createEntry = CreateEntries[item[materialIndex]];
				if( createEntry.avgMinSkill <= pSkill && pSkill <= createEntry.avgMaxSkill )
				{
					validWeapons.push( item[materialIndex] );
				}
			}
			else
			{
				// All weapon create entries are valid if player's skill is above or equal to 70.0
				validWeapons.push( item[materialIndex] );
			}
		}

		// Select a random create entry from array of valid entries
		var rndCreateIndex = Math.floor( Math.random() * validWeapons.length );
		return CreateEntries[validWeapons[rndCreateIndex]];
	}
	else // 0 - Armor
	{
		// First, select material type
		var materialIndex = RandomNumber( 0, 8 );
		if( considerPlayerSkill )
		{
			switch( materialIndex )
			{
				case 0: // Iron
					break;
				case 1: // Dull Copper
					materialIndex = ( pUser.skills.blacksmithing >= 650 ? materialIndex : 0 );
					break;
				case 2: // Shadow Iron
					materialIndex = ( pUser.skills.blacksmithing >= 700 ? materialIndex : 0 );
					break;
				case 3: // Copper
					materialIndex = ( pUser.skills.blacksmithing >= 750 ? materialIndex : 0 );
					break;
				case 4: // Bronze
					materialIndex = ( pUser.skills.blacksmithing >= 800 ? materialIndex : 0 );
					break;
				case 5: // Gold
					materialIndex = ( pUser.skills.blacksmithing >= 850 ? materialIndex : 0 );
					break;
				case 6: // Agapite
					materialIndex = ( pUser.skills.blacksmithing >= 900 ? materialIndex : 0 );
					break;
				case 7: // Verite
					materialIndex = ( pUser.skills.blacksmithing >= 950 ? materialIndex : 0 );
					break;
				case 8: // Valorite
					materialIndex = ( pUser.skills.blacksmithing >= 1000 ? materialIndex : 0 );
					break;
				default:
					break;
			}
		}

		// Find all valid armor create entries for player's current skill level, and of chosen material color
		// If player's skill is below 70.0, make sure they get a BOD with items they can gain skill from crafting
		var validArmors = [];
		for( var j = 0; j < armorCreateEntries.length; j++ )
		{
			var item = armorCreateEntries[j];
			if( considerPlayerSkill && pSkill < 700 )
			{
				// Only select items which player can gain skill from if skill is below 70.0
				var createEntry = CreateEntries[item[materialIndex]];
				if( createEntry.avgMinSkill <= pSkill && pSkill <= createEntry.avgMaxSkill )
				{
					validArmors.push( item[materialIndex] );
				}
			}
			else
			{
				// Select from all armors
				validArmors.push( item[materialIndex] );
			}
		}

		// Select a random create entry from array of valid entries
		var rndCreateIndex = Math.floor( Math.random() * validArmors.length );
		return CreateEntries[validArmors[rndCreateIndex]];
	}
}

function onTimer( pUser, timerID )
{
	// When Timer Expires set these flags on player false so he can get another BOD
	switch ( timerID )
	{
		case 1: // BOD cooldown timer elapsed
		{
			//pUser.SetTag( "bodOfferCD", null );
			break;
		}
		case 2: // BOD Hand-in Cooldown timer elapsed
		{
			//pUser.SetTag( "bodHandinCooldown", null );
			break;
		}
	}
}

function onDropItemOnNpc( pDropper, npcDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	if( socket == null )
		return false;

	var amountMax 	= iDropped.GetTag( "amountMax" ); 	// amount you have to make of the item
	var amountCur 	= iDropped.GetTag( "amountCur" ); 	// amount you have combined
	var iBodType 	= iDropped.GetTag( "bodType" ); 	// BOD type of the BOD itself
	var pBodType 	= npcDroppedOn.GetTag( "bodType" ); // BOD type of the NPC BOD is dropped on, if any

	if( iDropped.sectionID == "smallbod" && pBodType > 0 )
	{
		// Check if NPC accepts the type of BOD being dropped on them
		// Weaponsmith only accepts weapon BODs, armorsmith only accepts armor BODs, blacksmith accepts either
		if(( iBodType == 1 || iBodType == 2 ) && ( pBodType != iBodType && pBodType != 3 ))
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
		var bodRewardCD = pUser.GetJSTimer( 2, 3214 ); // Fetch timer for BOD reward cooldown
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
			// On delivery of a completed BOD, kill cooldown timer to get another BOD offer
			var bodOfferCD = pUser.GetJSTimer( 1, 3214 ); // Fetch timer for BOD offer cooldown
			if( bodRewardCD != 0 )
			{
				pDropper.KillJSTimer( 1, 3214 );
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
	if( goldToTive >= 5000 || ( playerPack.weight + goldWeight > playerPack.maxWeight ))
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

	// Get modifiers to min / max rewards based on properties of the BOD itself
	var minMaxMod = MinMaxRewardModifiers( iDropped );
	var bodRewardItem = null;
	if( EraStringToNum( GetServerSetting( "CoreShardEra" )) <= EraStringToNum( "lbr" ))
	{
		// Apply reward modifiers
		minReward = 0 + minMaxMod[0];
		maxReward = 16 + minMaxMod[1];

		switch( WeightedRandom( minReward, maxReward, weightVal ))
		{
			case 0: // Sturdy Pickaxe / Sturdy Shovel (equal chance)
				switch( RandomNumber( 0, 1 ))
				{
					case 0: // Sturdy Pickaxe
						bodRewardItem = CreateBODReward( 0, socket, pDropper, 0 );
						break;
					case 1: // Sturdy Shovel
						break;
						bodRewardItem = CreateBODReward( 1, socket, pDropper, 0 );
					default:
						break;
				}
				break;
			case 1: // Leather Gloves of Mining +1
				bodRewardItem = CreateBODReward( 2, socket, pDropper, 0 );
				break;
			case 2: // Studded Leather Gloves of Mining +3
				bodRewardItem = CreateBODReward( 3, socket, pDropper, 0 );
				break;
			case 3: // Ringmail Gloves of mining +5
				bodRewardItem = CreateBODReward( 4, socket, pDropper, 0 );
				break;
			case 4: // Dull Copper Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x973 );
				break;
			case 5: // Shadow Iron Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x966 );
				break;
			case 6: // Colored Anvil
				bodRewardItem = CreateBODReward( 6, socket, pDropper, WeightedRandom( 0, 7, weightVal ));
				break;
			case 7: // Copper Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x96E );
				break;
			case 8: // Bronze Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x6D6 );
				break;
			case 9: // +10 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 100 );
				break;
			case 10: // +15 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 150 );
				break;
			case 11: // Gold Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x8A5 );
				break;
			case 12: // +30 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 300 );
				break;
			case 13: // Agapite Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x979 );
				break;
			case 14: // +60 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 600 );
				break;
			case 15: // Verite Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x89F );
				break;
			case 16: // Valorite Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x8AB );
				break;
			default:
				break;
		}
	}
	else
	{
		// Apply reward modifiers
		minReward = 0 + minMaxMod[0];
		maxReward = 22 + minMaxMod[1];

		// AoS and beyond has 4 additional item rewards (equal chance for two of them)
		switch( WeightedRandom( minReward, maxReward, weightVal ))
		{
			case 0: // Sturdy Pickaxe / Sturdy Shovel (equal chance)
				switch( RandomNumber( 0, 1 ))
				{
					case 0: // Sturdy Pickaxe
						bodRewardItem = CreateBODReward( 0, socket, pDropper, 0 );
						break;
					case 1: // Sturdy Shovel
						break;
						bodRewardItem = CreateBODReward( 1, socket, pDropper, 0 );
					default:
						break;
				}
				break;
			case 1: // Leather Gloves of Mining +1
				bodRewardItem = CreateBODReward( 2, socket, pDropper, 0 );
				break;
			case 2: // Gargoyle's Pickaxe / Prospector's Tool (equal chance)
				switch( RandomNumber( 0, 1 ))
				{
					case 0: // Gargoyle's Pickaxe
						bodRewardItem = CreateBODReward( 8, socket, pDropper, 0 );
						break;
					case 1: // Prospector's Tool
						bodRewardItem = CreateBODReward( 9, socket, pDropper, 0 );
						break;
					default:
						break;
				}
				break;
			case 3: // Studded Leather Gloves of Mining +3
				bodRewardItem = CreateBODReward( 3, socket, pDropper, 0 );
				break;
			case 4: // Powder of Temperament
				bodRewardItem = CreateBODReward( 10, socket, pDropper, 0 );
				break;
			case 5: // Ringmail Gloves of mining +5
				bodRewardItem = CreateBODReward( 4, socket, pDropper, 0 );
				break;
			case 6: // Dull Copper Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x973 );
				break;
			case 7: // Shadow Iron Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x966 );
				break;
			case 8: // Blacksmith Scroll of Power + 5
				bodRewardItem = CreateBODReward( 11, socket, pDropper, 50 );
				break;
			case 9: // Copper Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x96E );
				break;
			case 10: // Colored Anvil
				bodRewardItem = CreateBODReward( 6, socket, pDropper, WeightedRandom( 0, 7, weightVal ));
				break;
			case 11: // Blacksmith Scroll of Power + 10
				bodRewardItem = CreateBODReward( 11, socket, pDropper, 100 );
				break;
			case 12: // Bronze Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x6D6 );
				break;
			case 13: // +10 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 100 );
				break;
			case 14: // Blacksmith Scroll of Power + 15
				bodRewardItem = CreateBODReward( 11, socket, pDropper, 150 );
				break;
			case 15: // +15 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 150 );
				break;
			case 16: // Blacksmith Scroll of Power + 20
				bodRewardItem = CreateBODReward( 11, socket, pDropper, 200 );
				break;
			case 17: // Gold Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x8A5 );
				break;
			case 18: // +30 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 300 );
				break;
			case 19: // Agapite Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x979 );
				break;
			case 20: // +60 Ancient Smithy Hammer
				bodRewardItem = CreateBODReward( 7, socket, pDropper, 600 );
				break;
			case 21: // Verite Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x89F );
				break;
			case 22: // Valorite Runic Hammer
				bodRewardItem = CreateBODReward( 5, socket, pDropper, 0x8AB );
				break;
			default:
				break;
		}
	}

	var errorFound = false;
	if( !ValidateObject( bodRewardItem ))
	{
		socket.SysMessage( "An error occurred while attempting to dispense rewards for BOD. Please contact a GM/Admin for assistance!" );
		Console.Error( "Error occured when attempting to create BOD item reward for player with serial " + pDropper.serial + "!" );
		errorFound = true;
	}

	if( !errorFound )
	{
		// Can player's pack hold weight of the special reward item AND the gold/check?
		if( playerPack.weight + bodRewardItem.weight + goldWeight > playerPack.maxWeight ) // special item + gold
		{
			if( playerPack.weight + bodRewardItem.weight + 100 > playerPack.maxWeight ) // special item + check
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
		bodRewardItem.container = pDropper.pack;
		bodRewardItem.PlaceInPack();

		// Log creation of gold in server logs
		Console.Log( "[BOD Reward] Special item reward (" + bodRewardItem.name + " - " + bodRewardItem.serial + ") given to player (" + pDropper.name + " - " + pDropper.serial + ")." );

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
		if( ValidateObject( bodRewardItem ))
		{
			bodRewardItem.Delete();
		}
		return false;
	}

	return true;
}

function CreateBODReward( rewardNum, socket, pDropper, extraInfo )
{
	if( !ValidateObject( pDropper ) || socket == null )
		return null;

	var rewardItem = null;
	switch( rewardNum )
	{
		case 0: // Sturdy Pickaxe
			// Allows you to mine either 150 or 200 ore before breaking. Number of uses is determined randomly at the time the reward is given.
			rewardItem = CreateDFNItem( socket, pDropper, "sturdy_pickaxe", 1, "ITEM", false );
			if( ValidateObject( rewardItem ))
			{
				rewardItem.maxUses = RandomNumber( 0, 1 ) ? 150 : 200;
				rewardItem.usesLeft = rewardItem.maxUses;
			}
			break;
		case 1: // Sturdy Shovel
			// Allows you to mine at least 180 ore with one shovel before it breaks.
			rewardItem = CreateDFNItem( socket, pDropper, "sturdy_shovel", 1, "ITEM", false );
			if( ValidateObject( rewardItem ))
			{
				rewardItem.maxUses = WeightedRandom( 0, 1, 3, true ) ? 150 : 200;
				rewardItem.usesLeft = rewardItem.maxUses;
			}
			break;
		case 2: // Leather Gloves of Mining +1
			// Increases mining skill by 1 point. Dyeable using a leather dyetub.
			rewardItem = CreateDFNItem( socket, pDropper, "mining_gloves_1", 1, "ITEM", false );
			rewardItem.SetTag( "skillBonusID", 45 ); // Mining
			rewardItem.SetTag( "skillBonusVal", 10 ); // +1.0
			break;
		case 3: // Studded Leather Gloves of Mining +3
			// Increases mining skill by 3 points. Dyeable using a leather dyetub.
			rewardItem = CreateDFNItem( socket, pDropper, "mining_gloves_3", 1, "ITEM", false );
			rewardItem.SetTag( "skillBonusID", 45 ); // Mining
			rewardItem.SetTag( "skillBonusVal", 30 ); // +3.0
			break;
		case 4: // Ringmail Gloves of mining +5
			// Increases mining skill by 5 points.
			rewardItem = CreateDFNItem( socket, pDropper, "mining_gloves_5", 1, "ITEM", false );
			rewardItem.SetTag( "skillBonusID", 45 ); // Mining
			rewardItem.SetTag( "skillBonusVal", 50 ); // +5.0
			break;
		case 5: // Runic Smithing Hammer
			// Allows you to make weapons of the same color as the hammer. Comes in all different
			// ore colors. Weapons have magic modifiers based on the type of hammer used/
			switch( extraInfo )
			{
				case 0x973: // Dull Copper - Durable/Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "dull_copper_runic_hammer", 1, "ITEM", false );
					break;
				case 0x966: // Shadow Iron - Durable/Ruin
					rewardItem = CreateDFNItem( socket, pDropper, "shadow_iron_runic_hammer", 1, "ITEM", false );
					break;
				case 0x96E: // Copper - Fortified/Ruin/Surpassingly Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "copper_runic_hammer", 1, "ITEM", false );
					break;
				case 0x6D6: // Bronze - Fortified/Might/Surpassingly Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "bronze_runic_hammer", 1, "ITEM", false );
					break;
				case 0x8A5: // Gold - Indestructible/Force/Emminently Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "gold_runic_hammer", 1, "ITEM", false );
					break;
				case 0x979: // Agapite - Indestructible/Power/Emminently Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "agapite_runic_hammer", 1, "ITEM", false );
					break;
				case 0x89F: // Verite - Indestructible/Power/Exceedingly Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "verite_runic_hammer", 1, "ITEM", false );
					break;
				case 0x8AB: // Valorite - Indestructible/Vanquishing/Supremely Accurate
					rewardItem = CreateDFNItem( socket, pDropper, "valorite_runic_hammer", 1, "ITEM", false );
					break;
				default:
					break;
			}
			break;
		case 6: // Colored Anvil
			// Other than looking cool, no special abilities.
			switch( extraInfo )
			{
				case 0: // Dull Copper Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "dc_anvil_deed", 1, "ITEM", false );
					break;
				case 1: // Shadow Iron Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "si_anvil_deed", 1, "ITEM", false );
					break;
				case 2: // Copper Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "c_anvil_deed", 1, "ITEM", false );
					break;
				case 3: // Bronze Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "b_anvil_deed", 1, "ITEM", false );
					break;
				case 4: // Gold Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "g_anvil_deed", 1, "ITEM", false );
					break;
				case 5: // Agapite Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "a_anvil_deed", 1, "ITEM", false );
					break;
				case 6: // Verite Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "v_anvil_deed", 1, "ITEM", false );
					break;
				case 7: // Valorite Anvil Deed
					rewardItem = CreateDFNItem( socket, pDropper, "v_anvil_deed", 1, "ITEM", false );
					break;
				default:
					break;
			}
			break;
		case 7: // Ancient Smithing Hammer
			// Increase blacksmithing skill by 10, 15, 30 or 60 points when equipped
			switch( extraInfo )
			{
				case 100: // +10
					rewardItem = CreateDFNItem( socket, pDropper, "ancient_smithy_hammer_10", 1, "ITEM", false );
					break;
				case 150: // +15
					rewardItem = CreateDFNItem( socket, pDropper, "ancient_smithy_hammer_15", 1, "ITEM", false );
					break;
				case 300: // +30
					rewardItem = CreateDFNItem( socket, pDropper, "ancient_smithy_hammer_30", 1, "ITEM", false );
					break;
				case 600: // +60
					rewardItem = CreateDFNItem( socket, pDropper, "ancient_smithy_hammer_60", 1, "ITEM", false );
					break;
				default:
					break;
			}
			break;
		case 8: // Gargoyle's Pickaxe
			// 100 uses, can summon elemental corresponding to the material being mined
			rewardItem = CreateDFNItem( socket, pDropper, "gargoyles_pickaxe", 1, "ITEM", false );
			if( ValidateObject( rewardItem ))
			{
				rewardItem.maxUses = 100;
				rewardItem.usesLeft = 100;
			}
			break;
		case 9: // Prospector's Tool
			// 50 uses, used to raise one level of a mineral vein
			rewardItem = CreateDFNItem( socket, pDropper, "prospectors_tool", 1, "ITEM", false );
			if( ValidateObject( rewardItem ))
			{
				rewardItem.maxUses = 50;
				rewardItem.usesLeft = 50;
			}
			break;
		case 10: // Powder of Temperament
			// 10 uses, increases durability of metallic armor
			rewardItem = CreateDFNItem( socket, pDropper, "powder_of_temperament", 1, "ITEM", false );
			if( ValidateObject( rewardItem ))
			{
				rewardItem.maxUses = 10;
				rewardItem.usesLeft = 10;
			}
			break;
		case 11: // Blacksmith Scrolls of Power
			// Power scroll to increase blacksmithing skill by +5, +10, +15 or +20
			switch( extraInfo )
			{
				case 50: // Blacksmithing +5
					rewardItem = CreateDFNItem( socket, pDropper, "power_scroll_smith_5", 1, "ITEM", false );
					break;
				case 100: // Blacksmithing +10
					rewardItem = CreateDFNItem( socket, pDropper, "power_scroll_smith_10", 1, "ITEM", false );
					break;
				case 150: // Blacksmithing +15
					rewardItem = CreateDFNItem( socket, pDropper, "power_scroll_smith_15", 1, "ITEM", false );
					break;
				case 200: // Blacksmithing +20
					rewardItem = CreateDFNItem( socket, pDropper, "power_scroll_smith_20", 1, "ITEM", false );
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	return rewardItem;
}

function MinMaxRewardModifiers( iDropped )
{
	var minMod = 0;
	var maxMod = 0;

	// Apply bonus to min if it's large BOD, penalty to max if small BOD
	if( iDropped.GetTag( "largeBOD" ))
	{
		minMod += 2;
	}
	else
	{
		maxMod -= 2;
	}

	// Apply bonus to min or penalty to max reward based on amount of items in BOD
	var amountMax = iDropped.GetTag( "amountMax" );
	switch( amountMax )
	{
		case 10:
			maxMod -= 2;
			break;
		case 15: // No change
			break;
		case 20:
			minMod++;
			break;
	}

	// Apply bonus/penalty if exceptional items required by BOD
	var reqExceptional = iDropped.GetTag( "reqExceptional" );
	if( reqExceptional )
	{
		minMod++;
	}
	else
	{
		maxMod -= 2;
	}

	// Apply bonus/penalty if special material required by BOD (replace with bonus/penalty based on specific color rarities?)
	var materialColor = iDropped.GetTag( "materialColor" );
	switch( materialColor )
	{
		case 0: // Iron
			maxMod -= 4;
			break;
		case 0x973: // Dull Copper
			maxMod -= 3;
			break;
		case 0x966: // Shadow Iron
			maxMod -= 2;
			break;
		case 0x96E: // Copper
			maxMod -= 1;
			break;
		case 0x6D6: // Bronze
			// No change
			break;
		case 0x8A5: // Gold
			minMod += 1;
			break;
		case 0x979: // Agapite
			minMod += 2;
			break;
		case 0x89F: // Verite
			minMod += 3;
			break;
		case 0x8AB: // Valorite
			minMod += 4;
			break;
		default: // Iron
			break;
	}

	return [minMod, maxMod];
}
