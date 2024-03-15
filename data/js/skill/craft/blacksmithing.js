const textHue = 0x480;				// Color of the text.
const blacksmithID = 4023;			// Use this to tell the gump what script to close.
const bronzeID = 4015;
const copperID = 4016;
const agapiteID = 4017;
const dullcopperID = 4018;
const goldID = 4019;
const shadowironID = 4020;
const valoriteID = 4021;
const veriteID = 4022;
const gumpDelay = 2000;				// Timer for the gump to reappear after crafting.
const ingotDelay = 200;				// Timer for the gump to reappear after selecting a ingot.
const repairDelay = 200;			// Timer for the gump to reappear after repairing an item
const craftGumpID = 4027;
const itemDetailsScriptID = 4026;

 // If enabled, players can craft coloured variants of weapons, though unless the craftItems array
 // is updated with specific create entries for the coloured weapon variants, they'll just be
 // regular weapons with ore colour applied
const allowColouredWeapons = GetServerSetting( "CraftColouredWeapons" );

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the armor or weapon to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

const myPage = [
	// Page 1 - Metal Armors
	[10217, 10218, 10219, 10220, 10221, 10222, 10223, 10224, 10225, 10226, 10227, 10228, 10229],
	// Page 2 - Helmets
	[10230, 10231, 10232, 10233, 10234],
	// Page 3 - Shields
	[10235, 10236, 10237, 10238, 10239, 10293],
	 // Page 4 - Bladed
	[10240, 10241, 10242, 10243, 10244, 10245, 10246, 10247],
	// Page 5 - Axes
	[10248, 10249, 10250, 10251, 10252, 10253, 10254],
	// Page 6 - PoleArms
	[10255, 10256, 10257, 10258, 10259],
	// Page 7 - Bashing
	[10260, 10261, 10262, 10263, 10264]
];

const craftItems = [
	// Iron
	[
		// Metal Armors
		[ 7, 9, 8, 10, 11, 12, 13, 16, 15, 14, 17, 18, 19 ],
		// Helmets
		[ 46, 48, 45, 47, 49 ],
		// Shields
		[ 1, 2, 6, 3, 5, 4 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Dull Copper
	[
		// Metal Armors
		[ 506, 508, 507, 509, 510, 511, 512, 515, 514, 513, 516, 517, 518 ],
		// Helmets
		[ 520, 522, 519, 521, 523 ],
		// Shields
		[ 500, 501, 505, 502, 504, 503 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Shadow Iron
	[
		// Metal Armors
		[ 606, 608, 607, 609, 610, 611, 612, 615, 614, 613, 616, 617, 618 ],
		// Helmets
		[ 620, 622, 619, 621, 623 ],
		// Shields
		[ 600, 601, 605, 602, 604, 603 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Copper
	[
		// Metal Armors
		[ 706, 708, 707, 709, 710, 711, 7012, 715, 714, 713, 716, 717, 718 ],
		// Helmets
		[ 720, 722, 719, 721, 723 ],
		// Shields
		[ 700, 701, 705, 702, 704, 703 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Bronze
	[
		// Metal Armors
		[ 806, 808, 807, 809, 810, 811, 812, 815, 814, 813, 816, 817, 818 ],
		// Helmets
		[ 820, 822, 819, 821, 823 ],
		// Shields
		[ 800, 801, 805, 802, 804, 803 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Gold
	[
		// Metal Armors
		[ 906, 908, 907, 909, 910, 911, 912, 915, 914, 913, 916, 917, 918 ],
		// Helmets
		[ 920, 922, 919, 921, 923 ],
		// Shields
		[ 900, 901, 905, 902, 904, 903 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Agapite
	[
		// Metal Armors
		[ 1206, 1208, 1207, 1209, 1210, 1211, 1212, 1215, 1214, 1213, 1216, 1217, 1218 ],
		// Helmets
		[ 1220, 1222, 1219, 1221, 1223 ],
		// Shields
		[ 1200, 1201, 1205, 1202, 1204, 1203 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Verite
	[
		// Metal Armors
		[ 1006, 1008, 1007, 1009, 1010, 1011, 1012, 1015, 1014, 1013, 1016, 1017, 1018 ],
		// Helmets
		[ 1020, 1022, 1019, 1021, 1023 ],
		// Shields
		[ 1000, 1001, 1005, 1002, 1004, 1003 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Valorite
	[
		// Metal Armors
		[ 1106, 1108, 1107, 1109, 1110, 1111, 1112, 1115, 1114, 1113, 1116, 1117, 1118 ],
		// Helmets
		[ 1120, 1122, 1119, 1121, 1123 ],
		// Shields
		[ 1100, 1101, 1105, 1102, 1104, 1103 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 7
	var myGump = new Gump;
	pUser.SetTempTag( "page", pageNum );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	var resourceHue = pUser.GetTempTag( "resourceHue" );

	for( var i = 0; i < myPage[pageNum - 1].length; i++ )
	{
		// Don't show weapon entries if player has coloured ingots selected
		if( !allowColouredWeapons && resourceHue > 0 && pageNum > 3 )
			continue;

		var index = i % 10;
		if( index == 0 )
		{
			if( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage(( i / 10) + 1 );

			if( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, ( 100 * pageNum ) + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage[pageNum - 1][i], socket.language ));

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, ( 2000 + ( 100 * pageNum )) + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function Page8( socket, pUser )
{
	//Ingot Choices
	var myGump = new Gump;
	pUser.SetTempTag( "page", 8 );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	var iron 		= pUser.ResourceCount( 0x1BF2 );
	var bronze 		= pUser.ResourceCount( 0x1BF2, 0x06d6 );
	var copper 		= pUser.ResourceCount( 0x1BF2, 0x07dd );
	var agapite 	= pUser.ResourceCount( 0x1BF2, 0x0979 );
	var dullcopper 	= pUser.ResourceCount( 0x1BF2, 0x0973 );
	var gold 		= pUser.ResourceCount( 0x1BF2, 0x08a5 );
	var shadowiron 	= pUser.ResourceCount( 0x1BF2, 0x0966 );
	var valorite 	= pUser.ResourceCount( 0x1BF2, 0x08ab );
	var verite 		= pUser.ResourceCount( 0x1BF2, 0x089f );
	var myPage8 = [
		GetDictionaryEntry( 10291, socket.language ) + " (" + iron.toString() + ")",
		GetDictionaryEntry( 10203, socket.language ) + " (" + dullcopper.toString() + ")",
		GetDictionaryEntry( 10204, socket.language ) + " (" + shadowiron.toString() + ")",
		GetDictionaryEntry( 10205, socket.language ) + " (" + copper.toString() + ")",
		GetDictionaryEntry( 10206, socket.language ) + " (" + bronze.toString() + ")",
		GetDictionaryEntry( 10207, socket.language ) + " (" + gold.toString() + ")",
		GetDictionaryEntry( 10208, socket.language ) + " (" + agapite.toString() + ")",
		GetDictionaryEntry( 10209, socket.language ) + " (" + verite.toString() + ")",
		GetDictionaryEntry( 10210, socket.language ) + " (" + valorite.toString() + ")"
	];

	for( var i = 0; i < myPage8.length; i++ )
	{
		var index = i % 10;
		if( index == 0 )
		{
			if( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage(( i / 10 ) + 1 );

			if( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );// PREV PAGE
			}
		}

		myGump.AddButton( 220, 60 + ( index * 20), 4005, 4007, 1, 0, 1000 + i );
		myGump.AddText( 255, 60 + ( index * 20), textHue, myPage8[i] );
	}
	myGump.Send( socket );
	myGump.Free();
}

function FindNearbyAnvils( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( trgItem.id == 0x0faf || trgItem.id == 0x0fb0 || trgItem.id == 0x2dd5 || trgItem.id == 0x2dd6 );
}

function FindNearbyForges( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return (( trgItem.id >= 0x197a && trgItem.id <= 0x19a9 ) || trgItem.id == 0x0Fb1 || trgItem.id == 0x2db0 || trgItem.id == 0x2dd8 );
}

function SmeltTarget( pSock )
{
	pSock.CustomTarget( 1, GetDictionaryEntry( 440, pSock.language )); // What item would you like to smelt?
}

// Armor and weapons can be smelted back into ingots.
function onCallback1( pSock, ourObj )
{
	// Smelt item, get ingots in return
	var mChar = pSock.currentChar;

	if( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTSMELT" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	var nearbyAnvil = AreaItemFunction( "FindNearbyAnvils", mChar, 3, pSock );
	var nearbyForge = AreaItemFunction( "FindNearbyForges", mChar, 3, pSock );
	if( nearbyForge == 0 || nearbyAnvil == 0)
	{
		// No forge nearby
		mChar.SetTempTag( "prevActionResult", "NOFORGEORANVIL" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	var creatorSerial = ourObj.creator;
	var entryMadeFrom = ourObj.entryMadeFrom;
	var createEntry;
	if( entryMadeFrom != null && entryMadeFrom != 0 )
	{
		createEntry = CreateEntries[entryMadeFrom];
	}
	if( createEntry != null && createEntry.id != ourObj.id )
	{
		createEntry = null;
	}

	var resourceName = "iron ingot";
	var resourceAmount = 0;
	var maxResourceAmount = 1;
	var resourceHue = ourObj.colour;

	if( creatorSerial == -1 || entryMadeFrom == 0 || createEntry == null )
	{
		// Not a player-crafted item, return 1 ingot if item is made of metal
		var materialType = TriggerEvent( 2506, "GetItemMaterialType", ourObj );
		if( materialType == "metal" )
		{
			resourceAmount = 1;
		}
	}
	else
	{
		if( createEntry.avgMinSkill > mChar.skills.mining )
		{
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOSMELTSKILL" );
			mChar.StartTimer( gumpDelay, 1, true );
			return;
		}

		// Loop through resources used to craft item, see how many ingots were used
		var resourcesUsed = createEntry.resources;
		for( var i = 0; i < resourcesUsed.length; i++ )
		{
			var resource = resourcesUsed[i];
			var amountNeeded = resource[0];
			var colorNeeded = resource[1];
			var resourceIDs = resource[2];

			// Loop through list of resourceIDs that were valid for crafting this item, see if ANY
			// were a match for the resource we're trying to return
			for( var j = 0; j <= resourceIDs.length; j++ )
			{
				// If both resource needed matches up, and resource color matches up, go for it
				if( resourceIDs[j] == 0x1bf2 && colorNeeded == resourceHue )
				{
					maxResourceAmount = amountNeeded;
					break;
				}
			}
		}

		if( maxResourceAmount > 1 )
		{
			// Calculate amount of resources returned based on player's mining skill, item's wear and tear,
			// and amount of resources that went into making the item in the first place
			if ( ourObj.health >= 1 || ourObj.usesLeft >= 1 ) 
			{
				var healthPercentage = 0;
				if( ourObj.health >= 1 )
				{
					healthPercentage = Math.floor( ( ourObj.health * 100) / ourObj.maxhp );
				}

				var usesPercentage = 0;
				if( ourObj.usesLeft >= 1 ) 
				{
					usesPercentage = Math.floor( ( ourObj.usesLeft * 100 ) / ourObj.maxUses );
				}

				var itemPercentage = usesPercentage > 0 ? Math.min( healthPercentage, usesPercentage ) : healthPercentage;

				resourceAmount = Math.floor( ( maxResourceAmount * itemPercentage ) / 100 );
			}

			// Halve the amount of resources returned
			resourceAmount = Math.max( Math.floor( resourceAmount / 2 ), 1 );

			// Fetch player's Mining skill
			var playerSkill = mChar.skills.mining;

			// Based on player's Mining skill, return between 1 to maxResourceAmount
			resourceAmount = Math.min( Math.max( Math.floor( resourceAmount * ( playerSkill / 1000 )), 1 ), resourceAmount );
		}
		else
		{
			resourceAmount = 1;
		}
	}

	if( resourceAmount == 0 )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTSMELT" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	if( ourObj.isDyeable )
	{
		// Dyeable items should return regular iron ingots
		resourceHue = 0;
	}

	switch( resourceHue )
	{
		case 0: // Iron Ingot
		default:
			break;
		case 0x0973: // Dull Copper
			resourceName = "dull copper ingot";
			break;
		case 0x0966: // Shadow Iron
			resourceName = "shadow iron ingot";
			break;
		case 0x07dd: // Copper
			resourceName = "copper ingot";
			break;
		case 0x06d6: // Bronze
			resourceName = "bronze ingot";
			break;
		case 0x08a5: // Gold
			resourceName = "gold ingot";
			break;
		case 0x0979: // Agapite
			resourceName = "agapite ingot";
			break;
		case 0x089f: // Verite
			resourceName = "verite ingot";
			break;
		case 0x08ab: // Valorite
			resourceName = "valorite ingot";
			break;
	}

	// Delete the melted item
	ourObj.Delete();

	// Run a generic skill check to give player a chance to increase their mining skill
	mChar.CheckSkill( 45, 0, 1000 );

	var newResource = CreateDFNItem( pSock, mChar, "0x1bf2", resourceAmount, "ITEM", true, resourceHue );
	newResource.name = resourceName;

	mChar.SetTempTag( "ingotsFromSmelting", resourceAmount );
	mChar.SetTempTag( "prevActionResult", "SMELTITEMSUCCESS" );
	mChar.StartTimer( gumpDelay, 1, true );
}

function RepairTarget( pSock )
{
	pSock.CustomTarget( 2, GetDictionaryEntry( 485, pSock.language )); // What item would you like to repair?
}

function onCallback2( pSock, ourObj )
{
	// Repair Item
	var mChar = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( mChar ) || mChar.dead )
		return;

	var bItem = pSock.tempObj;
	var anvil = AreaItemFunction( "FindNearbyAnvils", mChar, 3, pSock );
	var gumpID = blacksmithID + 0xffff;
	pSock.tempObj = null;

	if( ValidateObject( mChar ) && mChar.isChar && ValidateObject( bItem ) && bItem.isItem )
	{
		if( !ValidateObject( ourObj ) || !ourObj.isItem
			|| TriggerEvent( 2506, "GetItemMaterialType", ourObj ) != "metal"
			|| !CheckTileFlag( ourObj.id, 22 )) // TF_WEARABLE
		{
			// Targeted object is not an item that can be repaired
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		if( anvil == 0 )
		{
			// No anvil nearby
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOANVIL" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		var itemDurabilityLossEnabled = GetServerSetting( "ItemRepairDurabilityLoss" );
		var repairID = ourObj.id;
		var ownerObj = GetPackOwner( ourObj, 0 );
		if( ownerObj && mChar.serial == ownerObj.serial )
		{
			var maxHitpoints = ourObj.maxhp;
			var currentHitpoints = ourObj.health;
			if( currentHitpoints < maxHitpoints )
			{
				// Get base repair difficulty based on amount of HP missing and max hitpoints
				var deltaHP = maxHitpoints - currentHitpoints;
				var repairDifficulty = (( deltaHP / maxHitpoints ) * 1000 );
				var minDifficulty = repairDifficulty - 250;
				var skillBonus = 0;
				var repairSkill = mChar.skills.blacksmithing;
				if( minDifficulty < 0 )
				{
					// If minDifficulty is negative, add the negative value as a bonus to player's skill
					skillBonus = minDifficulty * -1;
					minDifficulty = 0;
				}
				else if( minDifficulty > repairSkill )
				{
					// Player skill below minimum repair difficulty, Too difficult to make the attempt!
					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
					return;
				}
				var maxDifficulty = Math.min( repairDifficulty + 250, 1000 );

				// Allow repair if random number between min and base difficulty is under player's skill
				if( RandomNumber( minDifficulty, 1000 ) < ( Math.max( repairSkill + skillBonus, 999 )))
				{
					// Give player a chance every now and then to gain skill from repairing
					if( RandomNumber( 1, 5 ) == 1 )
					{
						// Run a skill-check, which might trigger a skill-gain if player passes
						mChar.CheckSkill( 8, minDifficulty, maxDifficulty ); // Skill 8 = blacksmithing
					}

					// Reduce object's max durability by 1
					if( itemDurabilityLossEnabled )
					{
						ourObj.maxhp -= 1;
					}

					// Repair item here
					ourObj.health = ourObj.maxhp;
					pSock.SoundEffect( 0x002A, true );

					// Reopen gump after a short delay
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "REPAIRSUCCESS" );
					mChar.StartTimer( repairDelay, 1, true );

					// GM skill (100.0 skillpoints)
					// 	Item with 51 HP max
					// 		item with 2 hp left - 99.65% chance to repair
					// 		item with 25 hp left - 99.86% chance to repair
					// 		item with 40 hp left - 99.9% chance to repair

					// Expert Smith (71.5 skill points)
					//	Item with 51 HP max
					// 		item with 2 hp left - 1.45% chance to repair
					// 		item with 25 hp left - 61.49% chance to repair
					// 		item with 40 hp left - 74.9% chance to repair
					// 		item with 48 hp left - 90.6% chance to repair

					// Apprentice Smith (51.5 skill points)
					// 	Item with 51 HP max
					// 		item with 2 hp left - 0% chance to repair
					// 		item with 25 hp left - 34.5% chance to repair
					// 		item with 40 hp left - 54.9% chance to repair
					// 		item with 48 hp left - 70.6% chance to repair
				}
				else
				{
					// Failed to repair item - decrease item health!
					if( repairSkill >= 1000 ) // GM Smith
					{
						ourObj.health -= 1;
					}
					else if( repairSkill >= 715 ) // Expert Smith
					{
						ourObj.health -= 2;
					}
					else // Below Expert Smith
					{
						ourObj.health -= 3;
					}

					if( ourObj.health <= 0 )
					{
						// Item has been destroyed!
						pSock.SysMessage( GetDictionaryEntry( 311, pSock.language ).replace(/%s/gi, ourObj.name )); // Your %s has been destroyed.
						ourObj.Delete();
					}

					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "FAILREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
				}
			}
			else
			{
				pSock.tempObj = bItem;
				pSock.CloseGump( gumpID, 0 );
				mChar.SetTempTag( "prevActionResult", "FULLREPAIR" );
				mChar.StartTimer( repairDelay, 1, true );
			}
		}
		else
		{
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CHECKPACK" );
			mChar.StartTimer( repairDelay, 1, true );
		}
	}
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;

	switch( timerID )
	{
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
		case 5: // Page 5
		case 6: // Page 6
		case 7: // Page 7
			PageX( socket, pUser, timerID );
			break;
		case 8: // Page 8
			Page8( socket, pUser );
			break;
		default:
			break;
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	// Don't continue if player no longer has access to the crafting tool
	var bItem = pSock.tempObj;
	if( !ValidateObject( bItem ) || !pUser.InRange( bItem, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
		return;
	}

	if( bItem.movable == 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language )); // Locked down resources cannot be used!
		return;
	}

	var iPackOwner = GetPackOwner( bItem, 0 );
	if( ValidateObject( iPackOwner )) // Is the item in a backpack?
	{
		if( iPackOwner.serial != pUser.serial ) // And if so does the pack belong to the user?
		{
			pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language )); // This has to be in your backpack before you can use it.
		return;
	}

	var gumpID = blacksmithID + 0xffff;
	var makeID = 0;
	var recipeID = 0;
	var itemDetailsID = 0;
	var oreID = pUser.GetTempTag( "ORE" );
	var resourceHue = pUser.GetTempTag( "resourceHue" );
	var newOreID = -1;
	var newResourceHue = -1;
	var timerID = 0;

	// Check for nearby anvil
	var anvil = 0;

	// If button pressed is one of the crafting buttons (or "make last"), check that anvil was found
	if(( pButton >= 100 && pButton <= 704 ) || pButton == 5000 )
	{
		anvil = AreaItemFunction( "FindNearbyAnvils", pUser, 2, pSock );
		if( anvil > 0 )
		{
			if( pButton == 5000 )
			{
				// Make Last button
				pButton = pUser.GetTempTag( "MAKELAST" );
			}
			else
			{
				// Update Make Last entry
				pUser.SetTempTag( "MAKELAST", pButton );
			}
		}
		else
		{
			// No anvil found nearby!
			pUser.SetTempTag( "prevActionResult", "NOANVIL" );
		}
	}

	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "prevActionResult", null );
			pUser.SetTempTag( "MAKELAST", null );
			pSock.CloseGump( gumpID, 0 );
			break;// abort and do nothing
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
		case 5: // Page 5
		case 6: // Page 6
		case 7: // Page 7
			pSock.CloseGump( gumpID, 0 );
			PageX( pSock, pUser, pButton );
			break;
		case 49: // Repair Item
			RepairTarget( pSock );
			break;
		case 50: // Select Materials
			pSock.CloseGump( gumpID, 0 );
			Page8( pSock, pUser );
			break;
		case 52: // Smelt Item
			SmeltTarget( pSock );
			break;
		// [100-199]
		case 100: // Ringmail Gloves
			makeID = craftItems[oreID][0][0]; timerID = 1; break;
		case 101: // Ringmail Legs
			makeID = craftItems[oreID][0][1]; timerID = 1; break;
		case 102: // Ringmail Arms
			makeID = craftItems[oreID][0][2]; timerID = 1; break;
		case 103: // Ringmail Chest
			makeID = craftItems[oreID][0][3]; timerID = 1; break;
		case 104: // Chain Coif
			makeID = craftItems[oreID][0][4]; timerID = 1; break;
		case 105: // Chain Legs
			makeID = craftItems[oreID][0][5]; timerID = 1; break;
		case 106: // Chain Chest
			makeID = craftItems[oreID][0][6]; timerID = 1; break;
		case 107: // Plate Arms
			makeID = craftItems[oreID][0][7]; timerID = 1; break;
		case 108: // Plate Gloves
			makeID = craftItems[oreID][0][8]; timerID = 1; break;
		case 109: // Plate Gorget
			makeID = craftItems[oreID][0][9]; timerID = 1; break;
		case 110: // Plate Legs
			makeID = craftItems[oreID][0][10]; timerID = 1; break;
		case 111: // Plate Chest
			makeID = craftItems[oreID][0][11]; timerID = 1; recipeID = 18; break;// Example Recipe
		case 112: // Female Plate Chest
			makeID = craftItems[oreID][0][12]; timerID = 1; break;
		// [200-299]
		case 200: // Bascinet
			makeID = craftItems[oreID][1][0]; timerID = 2; break;
		case 201: // Close Helm
			makeID = craftItems[oreID][1][1]; timerID = 2; break;
		case 202: // Helmet
			makeID = craftItems[oreID][1][2]; timerID = 2; break;
		case 203: // Norse Helm
			makeID = craftItems[oreID][1][3]; timerID = 2; break;
		case 204: // Plate Helm
			makeID = craftItems[oreID][1][4]; timerID = 2; break;
		// [300-399]
		case 300: // Buckler
			makeID = craftItems[oreID][2][0]; timerID = 3; break;
		case 301: // Bronze Shield
			makeID = craftItems[oreID][2][1]; timerID = 3; break;
		case 302: // Heater Shield
			makeID = craftItems[oreID][2][2]; timerID = 3; break;
		case 303: // Metal Shield
			makeID = craftItems[oreID][2][3]; timerID = 3; break;
		case 304: // Metal Kite Shield
			makeID = craftItems[oreID][2][4]; timerID = 3; break;
		case 305: // Tear Kite Shield
			makeID = craftItems[oreID][2][5]; timerID = 3; break;
		// [400-499]
		case 400: // Broadsword
			makeID = craftItems[oreID][3][0]; timerID = 4; break;
		case 401: // Cutlass
			makeID = craftItems[oreID][3][1]; timerID = 4; break;
		case 402: // Dagger
			makeID = craftItems[oreID][3][2]; timerID = 4; break;
		case 403: // Katana
			makeID = craftItems[oreID][3][3]; timerID = 4; break;
		case 404: // Kryss
			makeID = craftItems[oreID][3][4]; timerID = 4; break;
		case 405: // Longsword
			makeID = craftItems[oreID][3][5]; timerID = 4; break;
		case 406: // Scimitar
			makeID = craftItems[oreID][3][6]; timerID = 4; break;
		case 407: // Viking Sword
			makeID = craftItems[oreID][3][7]; timerID = 4; break;
		// [500-599]
		case 500: // Axe
			makeID = craftItems[oreID][4][0]; timerID = 5; break;
		case 501: // Battle Axe
			makeID = craftItems[oreID][4][1]; timerID = 5; break;
		case 502: // Double Axe
			makeID = craftItems[oreID][4][2]; timerID = 5; break;
		case 503: // Executioners Axe
			makeID = craftItems[oreID][4][3]; timerID = 5; break;
		case 504: // Large Battle Axe
			makeID = craftItems[oreID][4][4]; timerID = 5; break;
		case 505: // Two Handed Axe
			makeID = craftItems[oreID][4][5]; timerID = 5; break;
		case 506: // War Axe
			makeID = craftItems[oreID][4][6]; timerID = 5; break;
		// [600-699]
		case 600: // Bardiche
			makeID = craftItems[oreID][5][0]; timerID = 6; break;
		case 601: // Halberd
			makeID = craftItems[oreID][5][1]; timerID = 6; break;
		case 602: // Short Spear
			makeID = craftItems[oreID][5][2]; timerID = 6; break;
		case 603: // Spear
			makeID = craftItems[oreID][5][3]; timerID = 6; break;
		case 604: // War Fork
			makeID = craftItems[oreID][5][4]; timerID = 6; break;
		// [700-799]
		case 700: // Hammer Pick
			makeID = craftItems[oreID][6][0]; timerID = 7; break;
		case 701: // Mace
			makeID = craftItems[oreID][6][1]; timerID = 7; break;
		case 702: // Maul
			makeID = craftItems[oreID][6][2]; timerID = 7; break;
		case 703: // War Mace
			makeID = craftItems[oreID][6][3]; timerID = 7; break;
		case 704: // War Hammer
			makeID = craftItems[oreID][6][4]; timerID = 7; break;
		// Select Ore Type
		case 1000: // Iron
			newOreID = (( newOreID == -1 ) ? 0 : newOreID );
			newResourceHue = 0; // Update manually if color changes in skills.dfn!
		case 1001: // Dull Copper
			if( pButton == 1001 && pUser.skills[7] < 650 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x0973 : newResourceHue ); // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 1 : newOreID );
		case 1002: // Shadow Iron
			if( pButton == 1002 && pUser.skills[7] < 700 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x0966 : newResourceHue ); // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 2 : newOreID );
		case 1003: // Copper
			if( pButton == 1003 && pUser.skills[7] < 750 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x07dd : newResourceHue ); // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 3 : newOreID );
		case 1004: // Bronze
			if( pButton == 1004 && pUser.skills[7] < 800 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x06d6 : newResourceHue ); // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 4 : newOreID );
		case 1005: // Gold
			if( pButton == 1005 && pUser.skills[7] < 850 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x08a5 : newResourceHue ); // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 5 : newOreID );
		case 1006: // Agapite
			if( pButton == 1006 && pUser.skills[7] < 900 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x0979 : newResourceHue );; // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 6 : newOreID );
		case 1007: // Verite
			if( pButton == 1007 && pUser.skills[7] < 950 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x089f : newResourceHue );; // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 7 : newOreID );
		case 1008: // Valorite
			if( pButton == 1008 && pUser.skills[7] < 990 )
			{
				pSock.CloseGump( gumpID, 0 );
				pUser.StartTimer( ingotDelay, 8, true);
				pUser.SetTempTag( "prevActionResult", "FAILED" );
				break;
			}
			newResourceHue = (( newResourceHue == - 1 ) ? 0x08ab : newResourceHue );; // Update manually if color changes in skills.dfn!
			newOreID = (( newOreID == -1 ) ? 8 : newOreID );

			// Run common code for this group of buttons
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "prevActionResult", null );
			pUser.SetTempTag( "ORE", newOreID );
			pUser.SetTempTag( "resourceHue", newResourceHue );
			Page8( pSock, pUser );
			break;
		// Show Item Details
		case 2100: // Ringmail Gloves
			itemDetailsID = 7; break;
		case 2101: // Ringmail Legs
			itemDetailsID = 9; break;
		case 2102: // Ringmail Arms
			itemDetailsID = 8; break;
		case 2103: // Ringmail Chest
			itemDetailsID = 10; break;
		case 2104: // Chain Coif
			itemDetailsID = 11; break;
		case 2105: // Chain Legs
			itemDetailsID = 12; break;
		case 2106: // Chain Chest
			itemDetailsID = 13; break;
		case 2107: // Plate Arms
			itemDetailsID = 16; break;
		case 2108: // Plate Gloves
			itemDetailsID = 15; break;
		case 2109: // Plate Gorget
			itemDetailsID = 14; break;
		case 2110: // Plate Legs
			itemDetailsID = 17; break;
		case 2111: // Plate Chest
			itemDetailsID = 18; break;
		case 2112: // Female Plate Chest
			itemDetailsID = 19; break;
		// [200-299]
		case 2200: // Bascinet
			itemDetailsID = 46; break;
		case 2201: // Close Helm
			itemDetailsID = 48; break;
		 case 2202: //Helmet
			itemDetailsID = 45; break;
		case 2203: // Norse Helm
			itemDetailsID = 47; break;
		case 2204: // Plate Helm
			itemDetailsID = 49; break;
		// [300-399]
		case 2300: // Buckler
			itemDetailsID = 1; break;
		case 2301: // Bronze Shield
			itemDetailsID = 2; break;
		case 2302: // Heater Shield
			itemDetailsID = 6; break;
		case 2303: // Metal Shield
			itemDetailsID = 3; break;
		case 2304: // MetalKiteShield
			itemDetailsID = 5; break;
		case 2305: // tear kite shield
			itemDetailsID = 4; break;
		// [400-499]
		case 2400: // Broadsword
			itemDetailsID = 25; break;
		case 2401: // Cutlass
			itemDetailsID = 21; break;
		case 2402: // Dagger
			itemDetailsID = 20; break;
		case 2403: // Katana
			itemDetailsID = 22; break;
		case 2404: // Kryss
			itemDetailsID = 23; break;
		case 2405: // Longsword
			itemDetailsID = 26; break;
		case 2406: // Scimitar
			itemDetailsID = 24; break;
		case 2407: // Viking Sword
			itemDetailsID = 27; break;
		// [500-599]
		case 2500: // Axe
			itemDetailsID = 29; break;
		case 2501: // Battle Axe
			itemDetailsID = 28; break;
		case 2502: // Double Axe
			itemDetailsID = 32; break;
		case 2503: // Executioners Axe
			itemDetailsID = 30; break;
		case 2504: // Large Battle Axe
			itemDetailsID = 33; break;
		case 2505: // Two Handed Axe
			itemDetailsID = 31; break;
		case 2506: // War Axe
			itemDetailsID = 34; break;
		// [600-699]
		case 2600: // Bardiche
			itemDetailsID = 38; break;
		case 2601: // Halberd
			itemDetailsID = 39; break;
		case 2602: // Short Spear
			itemDetailsID = 35; break;
		case 2603: // Spear
			itemDetailsID = 36; break;
		case 2604: // WarFork
			itemDetailsID = 37; break;
		// [700-799]
		case 2700: // Hammer Pick
			itemDetailsID = 44; break;
		case 2701: // Mace
			itemDetailsID = 40; break;
		case 2702: // Maul
			itemDetailsID = 41; break;
		case 2703: // War Mace
			itemDetailsID = 42; break;
		case 2704: // War Hammer
			itemDetailsID = 43; break;
		default:
			break;
	}

	TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, resourceHue, recipeID, anvil, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID );
}

function _restorecontext_() {}
