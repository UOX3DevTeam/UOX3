const textHue = 0x480;				// Color of the text.
const scriptID = 4025;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reappear after crafting.
const repairDelay = 200;			// Timer for the gump to reappear after repairing an item
const itemDetailsScriptID = 4026;
const craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

const myPage = [

	// Page 1 - Other
	[10611, 10612, 10613, 10614, 10615, 10616, 10617, 10618, 10619, 10620, 10688],

	// Page 2 - Furniture
	[10621, 10622, 10623, 10624, 10625, 10626, 10627, 10628, 10629, 10630, 10631, 10632, 10633],

	// Page 3 - Containers
	[10634, 10635, 10636, 10637, 10638, 10639, 10640, 10641, 10642],

	// Page 4 - Weapons
	[10643, 10644, 10645, 10646, 10647],

	// Page 5 - Armor
	[10648],

	// Page 6 - Instruments
	[10649, 10650, 10651, 10652, 10653, 10654],

	// Page 7 - Misc. Add-Ons
	[10655, 10656, 10657, 10658, 10659, 10660, 10661, 10662, 10663, 10664, 10665],

	// Page 8 - Tailoring and Cooking
	[10666, 10667, 10668, 10669, 10670, 10671, 10672, 10673, 10674, 10675, 10676, 10677],

	// Page 9 - Anvil and Forges
	[10678, 10679, 10680, 10681, 10682],

	// Page 10 - Training
	[10683, 10684, 10685, 10686]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 10
	var myGump = new Gump;
	pUser.SetTempTag( "page", pageNum );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	for( var i = 0; i < myPage[pageNum - 1].length; i++ )
	{
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
	var gumpID = scriptID + 0xffff;
	pSock.tempObj = null;

	if( ValidateObject( mChar ) && mChar.isChar && ValidateObject( bItem ) && bItem.isItem )
	{
		if( !ValidateObject( ourObj ) || !ourObj.isItem
			|| TriggerEvent( 2506, "GetItemMaterialType", ourObj ) != "wood"
			|| !CheckTileFlag( ourObj.id, 22 )) // TF_WEARABLE
		{
			// Targeted object is not an item that can be repaired
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
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
				var repairSkill = mChar.skills.carpentry;
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
						mChar.CheckSkill( 11, minDifficulty, maxDifficulty ); // Skill 11 = carpentry
					}

					// Reduce object's max durability by 1
					if( itemDurabilityLossEnabled )
					{
						ourObj.maxhp -= 1;
					}

					// Repair item here
					ourObj.health = ourObj.maxhp;
					pSock.SoundEffect( 0x023D, true );

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

	switch ( timerID )
	{
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
		case 5: // Page 5
		case 6: // Page 6
		case 7: // Page 7
		case 8: // Page 8
		case 9: // Page 9
		case 10: // Page 10
			PageX( socket, pUser, timerID );
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
	
	var gumpID = scriptID + 0xffff;
	var makeID = 0;
	var recipeID = 0;
	var itemDetailsID = 0;
	var timerID = 0;

	// If button pressed is one of the crafting buttons (or "make last")
	if(( pButton >= 100 && pButton <= 1003 ) || pButton == 5000 )
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

	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;// abort and do nothing
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
		case 5: // Page 5
		case 6: // Page 6
		case 7: // Page 7
		case 8: // Page 8
		case 9: // Page 9
		case 10: // Page 10
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "PageX", pSock, pUser, pButton );
			break;
		case 51:
			RepairTarget( pSock );
			break;
		case 100: // Barrel Staves
			makeID = 73; timerID = 1; break;
		case 101: // Barrel Lid
			makeID = 74; timerID = 1; break;
		case 102: // short music stand ( left )
			makeID = 89; timerID = 1; break;
		case 103: // short music stand ( right )
			makeID = 90; timerID = 1; break;
		case 104: // tall music stand ( left )
			makeID = 91; timerID = 1; break;
		case 105: // tall music stand ( left )
			makeID = 92; timerID = 1; break;
		case 106: // Easel ( S )
			makeID = 76; timerID = 1; break;
		case 107: // Easel ( E )
			makeID = 77; timerID = 1; break;
		case 108: // Easel ( N )
			makeID = 78; timerID = 1; break;
		case 109: // Fishing Pole
			makeID = 79; timerID = 1; break;
		case 110: // Boards
			makeID = 72; timerID = 1; break;
		case 200: // Barstool
			makeID = 50; timerID = 2; break;
		case 201: // Stool
			makeID = 51; timerID = 2; break;
		case 202: // Straw Chair
			makeID = 52; timerID = 2; break;
		case 203: // Wooden Chair
			makeID = 53; timerID = 2; break;
		case 204: // Vesper Style
			makeID = 57; timerID = 2; break;
		case 205: // Trinsic Style
			makeID = 58; timerID = 2; break;
		case 206: // Bench
			makeID = 54; timerID = 2; break;
		case 207: // Wooden Throne
			makeID = 55; timerID = 2; break;
		case 208: // Magincian Throne
			makeID = 56; timerID = 2; break;
		case 209: // Small Table
			makeID = 59; timerID = 2; break;
		case 210: // Writing Table
			makeID = 60; timerID = 2; break;
		case 211: // Large Table
			makeID = 61; timerID = 2; break;
		case 212: // Yew-wood Table
			makeID = 62; timerID = 2; break;
		case 300: // Small Chest
			makeID = 63; timerID = 3; break;
		case 301: // Small Crate
			makeID = 64; timerID = 3; break;
		case 302: // Medium Crate
			makeID = 65; timerID = 3; break;
		case 303: // Large Crate
			makeID = 67; timerID = 3; break;
		case 304: // Chest
			makeID = 68; timerID = 3; break;
		case 305: // Bookshelf
			makeID = 69; timerID = 3; break;
		case 306: // Armoire
			makeID = 70; timerID = 3; break;
		case 307: // Armoire
			makeID = 71; timerID = 3; break;
		case 308: // Open Keg
			makeID = 66; timerID = 3; break;
		case 400: // Shepard's Crook
			makeID = 80; timerID = 4; break;
		case 401: // Quarter Staff
			makeID = 81; timerID = 4; break;
		case 402: // Gnarled Staff
			makeID = 82; timerID = 4; break;
		case 403: // Club
			makeID = 123; timerID = 4; break;
		case 404: // Black Staff
			makeID = 124; timerID = 4; break;
		case 500: // Wooden Shield
			makeID = 75; timerID = 5; break;
		case 600: // Lap Harp
			makeID = 83; timerID = 6; break;
		case 601: // Standing Harp
			makeID = 84; timerID = 6; break;
		case 602: // Drum
			makeID = 85; timerID = 6; break;
		case 603: // Lute
			makeID = 86; timerID = 6; break;
		case 604: // Tambourine
			makeID = 87; timerID = 6; break;
		case 605: // Tambourine ( tassel )
			makeID = 88; timerID = 6; break;
		case 700: // bulletin board ( east )
			makeID = 93; timerID = 7; break;
		case 701: // bulletin board ( south )
			makeID = 93; timerID = 7; break;
		case 702: // Sm Bed ( E )
			makeID = 94; timerID = 7; break;
		case 703: // Sm Bed ( S )
			makeID = 93; timerID = 7; break;
		case 704: // Lg Bed ( E )
			makeID = 96; timerID = 7; break;
		case 705: // Lg Bed ( S )
			makeID = 95; timerID = 7; break;
		case 706: // Dart Board ( S )
			makeID = 97; timerID = 7; break;
		case 707: // Dart Board ( E )
			makeID = 98; timerID = 7; break;
		case 708: // Ballot Box
			makeID = 99; timerID = 7; break;
		case 709: // Pentagram
			makeID = 100; timerID = 7; break;
		case 710: // Abbatoir
			makeID = 101; timerID = 7; break;
		case 800: // Dressform
			makeID = 115; timerID = 8; break;
		case 801: // Dressform
			makeID = 116; timerID = 8; break;
		case 802: // Spin Wheel ( E )
			makeID = 107; timerID = 8; break;
		case 803: // Spin Wheel ( S )
			makeID = 108; timerID = 8; break;
		case 804: // Loom ( E )
			makeID = 109; timerID = 8; break;
		case 805: // Loom ( S )
			makeID = 110; timerID = 8; break;
		case 806: // Stone Oven ( E )
			makeID = 117; timerID = 8; break;
		case 807: // Stone Oven ( S )
			makeID = 118; timerID = 8; break;
		case 808: // Flour Mill ( E )
			makeID = 119; timerID = 8; break;
		case 809: // Flour Mill ( S )
			makeID = 120; timerID = 8; break;
		case 810: // Water Trough( E )
			makeID = 121; timerID = 8; break;
		case 811: // Water Trough( S )
			makeID = 122; timerID = 8; break;
		case 900: // Small Forge
			makeID = 102; timerID = 9; break;
		case 901: // Lg Forge ( E )
			makeID = 103; timerID = 9; break;
		case 902: // Lg Forge ( S )
			makeID = 104; timerID = 9; break;
		case 903: // Anvil ( E )
			makeID = 105; timerID = 9; break;
		case 904: // Anvil ( S )
			makeID = 106; timerID = 9; break;
		case 1000: // Dummy ( E )
			makeID = 111; timerID = 10; break;
		case 1001: // Dummy ( S )
			makeID = 112; timerID = 10; break;
		case 1002: // Pickpocket ( E )
			makeID = 113; timerID = 10; break;
		case 1003: // Pickpocket ( S )
			makeID = 114; timerID = 10; break;
		case 2100: // Barrel Staves
			itemDetailsID = 73; break;
		case 2101: // Barrel Lid
			itemDetailsID = 74; break;
		case 2102: // short music stand ( left )
			itemDetailsID = 89; break;
		case 2103: // short music stand ( right )
			itemDetailsID = 90; break;
		case 2104: // tall music stand ( left )
			itemDetailsID = 91; break;
		case 2105: // tall music stand ( left )
			itemDetailsID = 92; break;
		case 2106: // Easel ( S )
			itemDetailsID = 76; break;
		case 2107: // Easel ( E )
			itemDetailsID = 77; break;
		case 2108: // Easel ( N )
			itemDetailsID = 78; break;
		case 2109: // Fishing Pole
			itemDetailsID = 79; break;
		case 2110: // Boards
			itemDetailsID = 72; break;
		case 2200: // Barstool
			itemDetailsID = 50; break;
		case 2201: // Stool
			itemDetailsID = 51; break;
		case 2202: // Straw Chair
			itemDetailsID = 52; break;
		case 2203: // Wooden Chair
			itemDetailsID = 53; break;
		case 2204: // Vesper Style
			itemDetailsID = 57; break;
		case 2205: // Trinsic Style
			itemDetailsID = 58; break;
		case 2206: // Bench
			itemDetailsID = 54; break;
		case 2207: // Wooden Throne
			itemDetailsID = 55; break;
		case 2208: // Magincian Throne
			itemDetailsID = 56; break;
		case 2209:
			itemDetailsID = 59; break;
		case 2210: // Writing Table
			itemDetailsID = 60; break;
		case 2211: // Large Table
			itemDetailsID = 61; break;
		case 2212: // Yew-wood Table
			itemDetailsID = 62; break;
		case 2300: // Small Chest
			itemDetailsID = 63; break;
		case 2301: // Small Crate
			itemDetailsID = 64; break;
		case 2302: // Medium Crate
			itemDetailsID = 65; break;
		case 2303: // Large Crate
			itemDetailsID = 67; break;
		case 2304: // Chest
			itemDetailsID = 68; break;
		case 2305: // Bookshelf
			itemDetailsID = 69; break;
		case 2306: // Armoire
			itemDetailsID = 70; break;
		case 2307: // Armoire
			itemDetailsID = 71; break;
		case 2308: // Open Keg
			itemDetailsID = 66; break;
		case 2400: // Shepard's Crook
			itemDetailsID = 80; break;
		case 2401: // Quarter Staff
			itemDetailsID = 81; break;
		case 2402: // Gnarled Staff
			itemDetailsID = 82; break;
		case 2403: // Club
			itemDetailsID = 123; break;
		case 2404: // Black Staff
			itemDetailsID = 124; break;
		case 2500: // Wooden Shield
			itemDetailsID = 75; break;
		case 2600: // Lap Harp
			itemDetailsID = 83; break;
		case 2601: // Standing Harp
			itemDetailsID = 84; break;
		case 2602: // Drum
			itemDetailsID = 85; break;
		case 2603: // Lute
			itemDetailsID = 86; break;
		case 2604: // Tambourine
			itemDetailsID = 87; break;
		case 2605: // Tambourine ( tassel )
			itemDetailsID = 88; break;
		case 2700: // bulletin board ( east )
			itemDetailsID = 93; break;
		case 2701: // bulletin board ( south )
			itemDetailsID = 93; break;
		case 2702: // Sm Bed ( E )
			itemDetailsID = 94; break;
		case 2703: // Sm Bed ( S )
			itemDetailsID = 93; break;
		case 2704: // Lg Bed ( E )
			itemDetailsID = 96; break;
		case 2705: // Lg Bed ( S )
			itemDetailsID = 95; break;
		case 2706: // Dart Board ( S )
			itemDetailsID = 97; break;
		case 2707: // Dart Board ( E )
			itemDetailsID = 98; break;
		case 2708: // Ballot Box
			itemDetailsID = 99; break;
		case 2709: // Pentagram
			itemDetailsID = 100; break;
		case 2710: // Abbatoir
			itemDetailsID = 101; break;
		case 2800: // Dressform
			itemDetailsID = 115; break;
		case 2801: // Dressform
			itemDetailsID = 116; break;
		case 2802: // Spin Wheel ( E )
			itemDetailsID = 107; break;
		case 2803: // Spin Wheel ( S )
			itemDetailsID = 108; break;
		case 2804: // Loom ( E )
			itemDetailsID = 109; break;
		case 2805: // Loom ( S )
			itemDetailsID = 110; break;
		case 2806: // Stone Oven ( E )
			itemDetailsID = 117; break;
		case 2807: // Stone Oven ( S )
			itemDetailsID = 118; break;
		case 2808: // Flour Mill ( E )
			itemDetailsID = 119; break;
		case 2809: // Flour Mill ( S )
			itemDetailsID = 120; break;
		case 2810: // Water Trough( E )
			itemDetailsID = 121; break;
		case 2811: // Water Trough( S )
			itemDetailsID = 122; break;
		case 2900: // Small Forge
			itemDetailsID = 102; break;
		case 2901: // Lg Forge ( E )
			itemDetailsID = 103; break;
		case 2902: // Lg Forge ( S )
			itemDetailsID = 104; break;
		case 2903: // Anvil ( E )
			itemDetailsID = 105; break;
		case 2904: // Anvil ( S )
			itemDetailsID = 106; break;
		case 3000: // Dummy ( E )
			itemDetailsID = 111; break;
		case 3001: // Dummy ( S )
			itemDetailsID = 112; break;
		case 3002: // Pickpocket ( E )
			itemDetailsID = 113; break;
		case 3003: // Pickpocket ( S )
			itemDetailsID = 114; break;
		default:
			break;
	}

	TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, resourceHue, recipeID, null, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID );
}