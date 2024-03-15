const LabelHue = 0x480;				// Color of the text.
const LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4032;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const itemDetailsScriptID = 4026;
const craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

const myPage = [
	// Page 1 - Wooden Items
	[ 11801, 11802, 11803, 11804, 11805 ],

	// Page 2 - Tools
	[ 11820, 11821, 11822, 11823, 11824, 11825, 11826, 11827, 11828, 11829, 11830, 11831, 11832, 11833, 11834, 11835, 11836, 11837, 11838 ],

	// Page 3 - Parts
	[ 11860, 11861, 11862, 11863, 11864, 11865, 11866 ],

	// Page 4 - Utensils
	[ 11880, 11881, 11882, 11883, 11884, 11885, 11886, 11887, 11888, 11889, 11890, 11891 ],

	// Page 5 - Jewelry
	[ 11900, 11901, 11902, 11903, 11904, 11905 ],

	// Page 6 - Miscellaneuos
	[ 11920, 11921, 11922, 11923, 11924, 11925, 11926, 11927 ],

	// Page 7 - Multi-Component Items
	[ 11940, 11941, 11942, 11943, 11946, 11947, 11948 ],

	// Page 8 - Candles
	[ 11960, 11961, 11962, 11963, 11964, 11965, 11966, 11967 ],

	// Page 9 - Traps
	[ 11980, 11981, 11982 ]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 9
	var myGump = new Gump;
	pUser.SetTempTag( "page", pageNum );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	for ( var i = 0; i < myPage[pageNum - 1].length; i++ )
	{
		var index = i % 10;
		if ( index == 0 )
		{
			if ( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage( ( i / 10 ) + 1 );

			if ( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, ( 100 * pageNum ) + i );

		myGump.AddText( 255, 60 + ( index * 20 ), LabelHue, GetDictionaryEntry( myPage[pageNum - 1][i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2000 + ( 100 * pageNum ) + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;

	switch ( timerID )
	{
		case 1: // Page 1 - Wooden Items
		case 2: // Page 2 - Tools
		case 3: // Page 3 - Parts
		case 4: // Page 4 - Utensils
		case 5: // Page 5 - Jewelry
		case 6: // Page 6 - Miscellaneous
		case 7: // Page 7 - Multi-Component Items
		case 8: // Page 8 - Candles
		case 9: // Page 9 - Traps
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
	var resourceHue = 0;
	var recipeID = 0;
	var itemDetailsID = 0;
	var timerID = 0;

	if(( pButton >= 100 && pButton <= 950 ) || pButton == 5000 )
	{
		if( pButton == 5000 )
		{
			// Make Last button
			pButton = pUser.GetTempTag( "MAKELAST" );
		}
		else
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
	}

	switch ( pButton )
	{
		case 0: // Abort and do nothing
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1: // Page 1 - Wooden Items
		case 2: // Page 2 - Tools
		case 3: // Page 3 - Parts
		case 4: // Page 4 - Utensils
		case 5: // Page 5 - Jewelry
		case 6: // Page 6 - Miscellaneous
		case 7: // Page 7 - Multi-Component Items
		case 8: // Page 8 - Candles
		case 9: // Page 9 - Traps
			pSock.CloseGump( gumpID, 0 );
			PageX( pSock, pUser, pButton );
			break;
		// Make Items
		// Page 1 - Wooden Items
		case 100: // Axle
			makeID = 274; timerID = 1; break;
		case 101: // Clock Frame
			makeID = 273; timerID = 1; break;
		case 102: // Jointing Plane
			makeID = 270; timerID = 1; break;
		case 103: // Moulding Plane
			makeID = 271; timerID = 1; break;
		case 104: // Smoothing Plane
			makeID = 272; timerID = 1; break;
		// Page 2 - Tools
		case 200: // Dovetail Saw
			makeID = 218; timerID = 2; break;
		case 201: // Draw Knife
			makeID = 215; timerID = 2; break;
		case 202: // Froe
			makeID = 252; timerID = 2; break;
		case 203: // Hammer
			makeID = 255; timerID = 2; break;
		case 204: // Hatchet
			makeID = 214; timerID = 2; break;
		case 205: // Inshave
			makeID = 258; timerID = 2; break;
		case 206: // Lockpick
			makeID = 260; timerID = 2; break;
		case 207: // Mortar and Pestle
			makeID = 211; timerID = 2; break;
		case 208: // Pick Axe
			makeID = 259; timerID = 2; break;
		case 209: // Saw
			makeID = 217; timerID = 2; break;
		case 210: // Scissors
			makeID = 210; timerID = 2; break;
		case 211: // Scorp
			makeID = 212; timerID = 2; break;
		case 212: // Sewing Kit
			makeID = 216; timerID = 2; break;
		case 213: // Shovel
			makeID = 254; timerID = 2; break;
		case 214: // Sledge Hammer
			makeID = 257; timerID = 2; break;
		case 215: // Smith's Hammer
			makeID = 256; timerID = 2; break;
		case 216: // Tongs
			makeID = 253; timerID = 2; break;
		case 217: // Tool Kit (Tinker's tools)
			makeID = 213; timerID = 2; break;
		case 218: // Fletcher's Tools
			makeID = 284; timerID = 2; break;
		// Page 3 - Parts
		case 300: // Barrel Hoops
			makeID = 224; timerID = 3; break;
		case 301: // Barrel Tap
			makeID = 221; timerID = 3; break;
		case 302: // Clock parts
			makeID = 220; timerID = 3; break;
		case 303: // Gears
			makeID = 219; timerID = 3; break;
		case 304: // Hinge
			makeID = 225; timerID = 3; break;
		case 305: // Sextant parts
			makeID = 223; timerID = 3; break;
		case 306: // Springs
			makeID = 222; timerID = 3; break;
		// Page 4 - Utensils
		case 400: // Butcher Knife
			makeID = 226; timerID = 4; break;
		case 401: // Cleaver
			makeID = 232; timerID = 4; break;
		case 402: // Fork
			makeID = 230; timerID = 4; break;
		case 403: // Fork
			makeID = 231; timerID = 4; break;
		case 404: // Goblet
			makeID = 235; timerID = 4; break;
		case 405: // Knife
			makeID = 233; timerID = 4; break;
		case 406: // Knife
			makeID = 234; timerID = 4; break;
		case 407: // Pewter Mug
			makeID = 236; timerID = 4; break;
		case 408: // Plate
			makeID = 229; timerID = 4; break;
		case 409: // Skinning Knife
			makeID = 237; timerID = 4; break;
		case 410: // Spoon
			makeID = 227; timerID = 4; break;
		case 411: // Spoon
			makeID = 228; timerID = 4; break;
		// Page 5 - Jewelry
		case 500: // Bracelet
			makeID = 243; timerID = 5; break;
		case 501: // Earrings
			makeID = 241; timerID = 5; break;
		case 502: // Necklage (Golden beads)
			makeID = 239; timerID = 5; break;
		case 503: // Necklace (Silver beads)
			makeID = 240; timerID = 5; break;
		case 504: // Necklace (Round)
			makeID = 242; timerID = 5; break;
		case 505: // Weddingband (newbiefied)
			makeID = 238; timerID = 5; break;
		// Page 6 - Miscellaneous
		case 600: // Candelabra
			makeID = 245; timerID = 6; break;
		case 601: // Globe
			makeID = 248; timerID = 6; break;
		case 602: // Heating stand
			makeID = 251; timerID = 6; break;
		case 603: // Iron Key
			makeID = 247; timerID = 6; break;
		case 604: // Keyring
			makeID = 244; timerID = 6; break;
		case 605: // Lantern
			makeID = 250; timerID = 6; break;
		case 606: // Scales
			makeID = 246; timerID = 6; break;
		case 607: // Spy glass
			makeID = 249; timerID = 6; break;
		// Page 7 - Multi-Component Items
		case 700: // Axle and Gears
			makeID = 275; timerID = 7; break;
		case 701: // Clock
			makeID = 276; timerID = 7; break;
		case 702: // Clock
			makeID = 277; timerID = 7; break;
		case 703: // Clock Parts
			makeID = 278; timerID = 7; break;
		case 704: // Potion Keg
			makeID = 281; timerID = 7; break;
		case 705: // Sextant
			makeID = 282; timerID = 7; break;
		case 706: // Sextant Parts
			makeID = 283; timerID = 7; break;
		// Page 8 - Candles
		case 800: // Candelabra
			makeID = 245; timerID = 8; break;
		case 801: // Standing Candelabra
			makeID = 310; timerID = 8; break;
		case 802: // Regular Candle
			makeID = 315; timerID = 8; break;
		case 803: // Round Candle
			makeID = 312; timerID = 8; break;
		case 804: // Skull with Candle
			makeID = 316; timerID = 8; break;
		case 805: // Small Candle
			makeID = 314; timerID = 8; break;
		case 806: // Tall Candle
			makeID = 311; timerID = 8; break;
		case 807: // Thick Candle
			makeID = 313; timerID = 8; break;
		// Page 9 - Traps
		case 900: // Dart Trap
			makeID = 261; timerID = 9; break;
		case 901: // Explosion Trap
			makeID = 263; timerID = 9; break;
		case 902: // Poison Trap
			makeID = 262; timerID = 9; break;
		// Show Item Details
		case 2100: // Axle
			itemDetailsID = 274; break;
		case 2101: // Clock Frame
			itemDetailsID = 273; break;
		case 2102: // Jointing Plane
			itemDetailsID = 270; break;
		case 2103: // Moulding Plane
			itemDetailsID = 271; break;
		case 2104: // Smoothing Plane
			itemDetailsID = 272; break;
		// Page 2 - Tools
		case 2200: // Dovetail Saw
			itemDetailsID = 218; break;
		case 2201: // Draw Knife
			itemDetailsID = 215; break;
		case 2202: // Froe
			itemDetailsID = 252; break;
		case 2203: // Hammer
			itemDetailsID = 255; break;
		case 2204: // Hatchet
			itemDetailsID = 214; break;
		case 2205: // Inshave
			itemDetailsID = 258; break;
		case 2206: // Lockpick
			itemDetailsID = 260; break;
		case 2207: // Mortar and Pestle
			itemDetailsID = 211; break;
		case 2208: // Pick Axe
			itemDetailsID = 259; break;
		case 2209: // Saw
			itemDetailsID = 217; break;
		case 2210: // Scissors
			itemDetailsID = 210; break;
		case 2211: // Scorp
			itemDetailsID = 212; break;
		case 2212: // Sewing Kit
			itemDetailsID = 216; break;
		case 2213: // Shovel
			itemDetailsID = 254; break;
		case 2214: // Sledge Hammer
			itemDetailsID = 257; break;
		case 2215: // Smith's Hammer
			itemDetailsID = 256; break;
		case 2216: // Tongs
			itemDetailsID = 253; break;
		case 2217: // Tool Kit (Tinker's tools)
			itemDetailsID = 213; break;
		case 2218: // Fletcher's Tools
			itemDetailsID = 284; break;
		// Page 3 - Parts
		case 2300: // Barrel Hoops
			itemDetailsID = 224; break;
		case 2301: // Barrel Tap
			itemDetailsID = 221; break;
		case 2302: // Clock parts
			itemDetailsID = 220; break;
		case 2303: // Gears
			itemDetailsID = 219; break;
		case 2304: // Hinge
			itemDetailsID = 225; break;
		case 2305: // Sextant parts
			itemDetailsID = 223; break;
		case 2306: // Springs
			itemDetailsID = 222; break;
		// Page 4 - Utensils
		case 2400: // Butcher Knife
			itemDetailsID = 226; break;
		case 2401: // Cleaver
			itemDetailsID = 232; break;
		case 2402: // Fork
			itemDetailsID = 230; break;
		case 2403: // Fork
			itemDetailsID = 231; break;
		case 2404: // Goblet
			itemDetailsID = 235; break;
		case 2405: // Knife
			itemDetailsID = 233; break;
		case 2406: // Knife
			itemDetailsID = 234; break;
		case 2407: // Pewter Mug
			itemDetailsID = 236; break;
		case 2408: // Plate
			itemDetailsID = 229; break;
		case 2409: // Skinning Knife
			itemDetailsID = 237; break;
		case 2410: // Spoon
			itemDetailsID = 227; break;
		case 2411: // Spoon
			itemDetailsID = 228; break;
		// Page 5 - Jewelry
		case 2500: // Bracelet
			itemDetailsID = 243; break;
		case 2501: // Earrings
			itemDetailsID = 241; break;
		case 2502: // Necklage (Golden beads)
			itemDetailsID = 239; break;
		case 2503: // Necklace (Silver beads)
			itemDetailsID = 240; break;
		case 2504: // Necklace (Round)
			itemDetailsID = 242; break;
		case 2505: // Weddingband (newbiefied)
			itemDetailsID = 238; break;
		// Page 6 - Miscellaneous
		case 2600: // Candelabra
			itemDetailsID = 245; break;
		case 2601: // Globe
			itemDetailsID = 248; break;
		case 2602: // Heating stand
			itemDetailsID = 251; break;
		case 2603: // Iron Key
			itemDetailsID = 247; break;
		case 2604: // Keyring
			itemDetailsID = 244; break;
		case 2605: // Lantern
			itemDetailsID = 250; break;
		case 2606: // Scales
			itemDetailsID = 246; break;
		case 2607: // Spy glass
			itemDetailsID = 249; break;
		// Page 7 - Multi-Component Items
		case 2700: // Axle and Gears
			itemDetailsID = 275; break;
		case 2701: // Clock
			itemDetailsID = 276; break;
		case 2702: // Clock
			itemDetailsID = 277; break;
		case 2703: // Clock Parts
			itemDetailsID = 278; break;
		case 2704: // Potion Keg
			itemDetailsID = 281; break;
		case 2705: // Sextant
			itemDetailsID = 282; break;
		case 2706: // Sextant Parts
			itemDetailsID = 283; break;
		// Page 8 - Candles
		case 2800: // Candelabra
			itemDetailsID = 245; break;
		case 2801: // Standing Candelabra
			itemDetailsID = 310; break;
		case 2802: // Regular Candle
			itemDetailsID = 315; break;
		case 2803: // Round Candle
			itemDetailsID = 312; break;
		case 2804: // Skull with Candle
			itemDetailsID = 316; break;
		case 2805: // Small Candle
			itemDetailsID = 314; break;
		case 2806: // Tall Candle
			itemDetailsID = 311; break;
		case 2807: // Thick Candle
			itemDetailsID = 313; break;
		// Page 9 - Traps
		case 2900: // Dart Trap
			itemDetailsID = 261; break;
		case 2901: // Explosion Trap
			itemDetailsID = 263; break;
		case 2902: // Poison Trap
			itemDetailsID = 262; break;
		default:
			break;
	}

	if( makeID != 0 )
	{
		if(( pButton >= 500 && pButton <= 505 ))
		{
			// Ask crafter which material to use
			pUser.SetTempTag( "makeID", makeID );
			pUser.SetTempTag( "timerID", timerID );
			pUser.AddScriptTrigger( 4033 );
			pSock.CustomTarget( 2, GetDictionaryEntry( 12008, pSock.language )); // Select material to use:
			return;
		}

		TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, resourceHue, recipeID, null, gumpDelay, timerID, 0, 0 );
	}
	else if( itemDetailsID != 0 )
	{
		pUser.SetTempTag( "ITEMDETAILS", itemDetailsID );
		TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
	}
}

function onCallback2( pSock, targObj )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Fetch makeID and timerID from temp tag
	var makeID = pUser.GetTempTag( "makeID" );
	var timerID = pUser.GetTempTag( "timerID" );
	pUser.SetTempTag( "makeID", null );
	pUser.SetTempTag( "timerID", null );

	var bItem = pSock.tempObj; // tool
	if( ValidateObject( bItem ))
	{
		if( ValidateObject( targObj ) && targObj.isItem )
		{
			// Make sure targeted item is in player's backpack
			var iPackOwner = GetPackOwner( targObj, 0 );
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

			if( makeID >= 238 && makeID <= 243 )
			{
				// Jewlery
				var resourceType = TriggerEvent( 2506, "GetResourceType", targObj.id );
				if( resourceType != "gems" )
				{
					pSock.SysMessage( GetDictionaryEntry( 12007, pSock.language )); // That's not a gem resource!
					return;
				}

				// Set a temporary tag on character with ID of selected gem
				// We'll check for this ID in the crafting process, and remove it
				// in the onMakeItem event script (crafting_complete.js)
				pUser.SetTempTag( "targetedSubResourceId", targObj.id );
				pUser.SetTempTag( "targetedSubResourceName", targObj.name );
			}

			TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, 0, 0, null, gumpDelay, timerID, 0, 0 );
		}
	}
}