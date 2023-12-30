const textHue = 0x480;				// Color of the text.
const scriptID = 4030;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const repairDelay = 200;			// Timer for the gump to reapear after selecting a resource.
const itemDetailsScriptID = 4026;
const craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

const myPage = [
	// Page 1 - Hats
	[11415, 11416, 11417, 11418, 11419, 11420, 11421, 11422, 11423, 11424, 11425],

	// Page 2 - Shirts and Pants
	[11426, 11427, 11428, 11429, 11430, 11431, 11432, 11433, 11434, 11435, 11436, 11437, 11438, 11439],

	// Page 3 - Miscellaneous
	[11440, 11441, 11442, 11443],

	// Page 4 - Footwear
	[11444, 11445,  11446, 11447],

	// Page 5 - Leather Armor
	[11448, 11449, 11450, 11451, 11452, 11453],

	// Page 6 - Studded Armor
	[11454, 11455, 11456, 11457, 11458],

	// Page 7 - Female Armor
	[11459, 11460, 11461, 11462, 11463, 11464],

	// Page 8 - Bone Armor
	[11465, 11466, 11467, 11468, 11469]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 8
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
		case 8: // Page 8
			PageX( socket, pUser, timerID );
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

	var gumpID = scriptID + 0xffff;
	var makeID = 0;
	var recipeID = 0;
	var itemDetailsID = 0;
	var timerID = 0;

	// If button pressed is one of the crafting buttons (or "make last"), check that anvil was found
	if(( pButton >= 100 && pButton <= 804 ) || pButton == 5000 )
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
		case 0: // Abort and do nothing
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
		case 5: // Page 5
		case 6: // Page 6
		case 7: // Page 7
		case 8: // Page 8
			pSock.CloseGump( gumpID, 0 );
			PageX( pSock, pUser, pButton );
			break;
/*		case 51: // Repair Item
			var targMsg = GetDictionaryEntry( 485, pSock.language ); //
			pSock.CustomTarget( 2, targMsg );
			break;*/
		case 52: // Unravel Item
			UnravelTarget( pSock );
			break;
		case 100: // skullcap
			makeID = 130; timerID = 1; break;
		case 101: // bandana
			makeID = 131; timerID = 1; break;
		case 102: // floppy hat
			makeID = 132; timerID = 1; break;
		case 103: // cap
			makeID = 134; timerID = 1; break;
		case 104: // wide-brim hat
			makeID = 133; timerID = 1; break;
		case 105: // straw hat
			makeID = 136; timerID = 1; break;
		case 106: // wizard's hat
			makeID = 137; timerID = 1; break;
		case 107: // bonnet
			makeID = 138; timerID = 1; break;
		case 108: // feather hat
			makeID = 139; timerID = 1; break;
		case 109: // tricorne hat
			makeID = 140; timerID = 1; break;
		case 110: // jester hat
			makeID = 141; timerID = 1; break;
		case 200: // doublet
			makeID = 142; timerID = 2; break;
		case 201: // shirt
			makeID = 143; timerID = 2; break;
		case 202: // fancy shirt
			makeID = 144; timerID = 2; break;
		case 203: // tunic
			makeID = 145; timerID = 2; break;
		case 204: // surcoat
			makeID = 146; timerID = 2; break;
		case 205: // plain dress
			makeID = 147; timerID = 2; break;
		case 206: // fancy dress
			makeID = 148; timerID = 2; break;
		case 207: // cloak
			makeID = 149; timerID = 2; break;
		case 208: // robe
			makeID = 150; timerID = 2; break;
		case 209: // jester suit
			makeID = 151; timerID = 2; break;
		case 210: // short pants
			makeID = 180; timerID = 2; break;
		case 211: // long pants
			makeID = 152; timerID = 2; break;
		case 212: // kilt
			makeID = 153; timerID = 2; break;
		case 213: // skirt
			makeID = 154; timerID = 2; break;
		case 300: // body sash
			makeID = 155; timerID = 3; break;
		case 301: // half apron
			makeID = 156; timerID = 3; break;
		case 302: // full apron
			makeID = 157; timerID = 3; break;
		case 303: // oil cloth
			makeID = 158; timerID = 3; break;
		case 400: // sandals
			makeID = 159; timerID = 4; break;
		case 401: // shoes
			makeID = 160; timerID = 4; break;
		case 402: // boots
			makeID = 161; timerID = 4; break;
		case 403: // thigh boots
			makeID = 162; timerID = 4; break;
		case 500: // leather gorget
			makeID = 163; timerID = 5; break;
		case 501: // leather cap
			makeID = 164; timerID = 5; break;
		case 502: // leather gloves
			makeID = 165; timerID = 5; break;
		case 503: // leather sleeves
			makeID = 166; timerID = 5; break;
		case 504: // leather legging
			makeID = 167; timerID = 5; break;
		case 505: // leather tunic
			makeID = 168; timerID = 5; break;
		case 600: // studded gorget
			makeID = 169; timerID = 6; break;
		case 601: // studded gloves
			makeID = 170; timerID = 6; break;
		case 602: // studded sleeves
			makeID = 171; timerID = 6; break;
		case 603: // studded leggings
			makeID = 172; timerID = 6; break;
		case 604: // studded tunic
			makeID = 173; timerID = 6; break;
		case 700: // leather shorts
			makeID = 174; timerID = 7; break;
		case 701: // leather skirts
			makeID = 175; timerID = 7; break;
		case 802: // leather bustier
			makeID = 176; timerID = 7; break;
		case 703: // studded bustier
			makeID = 177; timerID = 7; break;
		case 704: // female leather armor
			makeID = 178; timerID = 7; break;
		case 705: // studded armor
			makeID = 179; timerID = 7; break;
		case 800: // bone helmet
			makeID = 181; timerID = 8; break;
		case 901: // bone gloves
			makeID = 182; timerID = 9; break;
		case 802: // bone arms
			makeID = 183; timerID = 8; break;
		case 803: // bone leggings
			makeID = 184; timerID = 8; break;
		case 804: // bone armor
			makeID = 185; timerID = 8; break;
		case 2100: // skullcap
			itemDetailsID = 130; break;
		case 2101: // bandana
			itemDetailsID = 131; break;
		case 2102: // floppy hat
			itemDetailsID = 132; break;
		case 2103: // cap
			itemDetailsID = 134; break;
		case 2104: // wide-brim hat
			itemDetailsID = 133; break;
		case 2105: // straw hat
			itemDetailsID = 136; break;
		case 2106: // wizard's hat
			itemDetailsID = 137; break;
		case 2107: // bonnet
			itemDetailsID = 138; break;
		case 2108: // feather hat
			itemDetailsID = 139; break;
		case 2109: // tricorne hat
			itemDetailsID = 140; break;
		case 2110: // jester hat
			itemDetailsID = 141; break;
		case 2200: // doublet
			itemDetailsID = 142; break;
		case 2201: // shirt
			itemDetailsID = 143; break;
		case 2202: // fancy shirt
			itemDetailsID = 144; break;
		case 2203: // tunic
			itemDetailsID = 145; break;
		case 2204: // surcoat
			itemDetailsID = 146; break;
		case 2205: // plain dress
			itemDetailsID = 147; break;
		case 2206: // fancy dress
			itemDetailsID = 148; break;
		case 2207: // cloak
			itemDetailsID = 149; break;
		case 2208: // robe
			itemDetailsID = 150; break;
		case 2209: // jester suit
			itemDetailsID = 151; break;
		case 2210: // short pants
			itemDetailsID = 180; break;
		case 2211: // long pants
			itemDetailsID = 152; break;
		case 2212: // kilt
			itemDetailsID = 153; break;
		case 2213: // skirt
			itemDetailsID = 154; break;
		case 2300: // body sash
			itemDetailsID = 155; break;
		case 2301: // half apron
			itemDetailsID = 156; break;
		case 2302: // full apron
			itemDetailsID = 157; break;
		case 2303: // oil cloth
			itemDetailsID = 158; break;
		case 2400: // sandals
			itemDetailsID = 159; break;
		case 2401: // shoes
			itemDetailsID = 160; break;
		case 2402: // boots
			itemDetailsID = 161; break;
		case 2403: // thigh boots
			itemDetailsID = 162; break;
		case 2500: // leather gorget
			itemDetailsID = 163; break;
		case 2501: // leather cap
			itemDetailsID = 164; break;
		case 2502: // leather gloves
			itemDetailsID = 165; break;
		case 2503: // leather sleeves
			itemDetailsID = 166; break;
		case 2504: // leather legging
			itemDetailsID = 167; break;
		case 2505: // leather tunic
			itemDetailsID = 168; break;
		case 2600: // studded gorget
			itemDetailsID = 169; break;
		case 2601: // studded gloves
			itemDetailsID = 170; break;
		case 2602: // studded sleeves
			itemDetailsID = 171; break;
		case 2603: // studded leggings
			itemDetailsID = 172; break;
		case 2604: // studded tunic
			itemDetailsID = 173; break;
		case 2700: // leather shorts
			itemDetailsID = 174; break;
		case 2701: // leather skirts
			itemDetailsID = 175; break;
		case 2702: // leather bustier
			itemDetailsID = 176; break;
		case 2703: // studded bustier
			itemDetailsID = 177; break;
		case 2704: // female leather armor
			itemDetailsID = 178; break;
		case 2705: // studded armor
			itemDetailsID = 179; break;
		case 2800: // bone helmet
			itemDetailsID = 181; break;
		case 2801: // bone gloves
			itemDetailsID = 182; break;
		case 2802: // bone arms
			itemDetailsID = 183; break;
		case 2803: // bone leggings
			itemDetailsID = 184; break;
		case 2804: // bone armor
			itemDetailsID = 185; break;
		default:
			break;
	}

	if( makeID >= 100 && makeID <= 158 )
	{
		// These items require cloth resources. Ask player which material to use!
		pUser.SetTempTag( "makeID", makeID );
		pUser.SetTempTag( "timerID", timerID );
		pSock.CustomTarget( 1, GetDictionaryEntry( 444, pSock.language )); // Select material to use.
		//var undyedRes = pUser.ResourceCount( 0x1767, 0 );
		//var totalRes = pUser.ResourceCount( 0x1767, -1 );
	}
	else
	{
		TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, 0, recipeID, null, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID );
	}
}

function onCallback1( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Fetch makeID and timerID from temp tag
	var makeID = pUser.GetTempTag( "makeID" );
	var timerID = pUser.GetTempTag( "timerID" );
	pUser.SetTempTag( "makeID", null );
	pUser.SetTempTag( "timerID", null );

	var bItem = pSock.tempObj;
	if( ValidateObject( bItem ))
	{
		if( ValidateObject( ourObj ) && ourObj.isItem )
		{
			// Make sure targeted item is in player's backpack
			var iPackOwner = GetPackOwner( ourObj, 0 );
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

			// Pass in the colour of the desired material to use for crafting
			TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, ourObj.colour, recipeID, null, gumpDelay, timerID, 0, 0 );
		}
	}
}

function UnravelTarget( pSock )
{
	pSock.CustomTarget( 2, GetDictionaryEntry( 10295, pSock.language )); // What item would you like to unravel?
}

// Clothes and leather armor can be unravelled back into cloth and leather
function onCallback2( pSock, ourObj )
{
	// Unravel item, get cloth/leather in return
	var mChar = pSock.currentChar;

	if( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTUNRAVEL" );
		mChar.StartTimer( repairDelay, 1, true );
		return;
	}

	var creatorSerial = ourObj.creator;
	var entryMadeFrom = ourObj.entryMadeFrom;
	var createEntry;
	if( entryMadeFrom != null && entryMadeFrom != 0 )
		createEntry = CreateEntries[entryMadeFrom];
	if( createEntry != null && createEntry.id != ourObj.id )
		createEntry = null;

	var resourceAmount = 0;
	var maxResourceAmount = 1;
	var resourceColor = ourObj.colour;
	var materialType = TriggerEvent( 2506, "GetItemMaterialType", ourObj, 0 );

	if( creatorSerial == -1 || entryMadeFrom == 0 || createEntry == null )
	{
		// Not a player-crafted item, return 1 resource if item is made of cloth/leather
		if( materialType == "cloth" || materialType == "leather" )
		{
			resourceAmount = 1;
		}
	}
	else
	{
		if( createEntry.avgMinSkill > mChar.skills.tailoring )
		{
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOUNRAVELSKILL" );
			mChar.StartTimer( gumpDelay, 1, true );
			return;
		}

		// Loop through resources used to craft item, see how many resources were used
		var resourceID = 0;
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
				if( !isNaN( parseInt( resourceIDs[j] )))
				{
					// If we found some resource that match up to cloth, or leather, go for it
					var resourceType = TriggerEvent( 2506, "GetResourceType", parseInt( resourceIDs[j] ));
					/*mChar.TextMessage( "MaterialType: " + materialType );
					mChar.TextMessage( "ResourceType: " + resourceType );
					mChar.TextMessage( "resourceID: " + resourceIDs[j] );*/
					if( materialType == resourceType )
					{
						maxResourceAmount = amountNeeded;
						resourceID = resourceIDs[j];
						break;
					}
				}
			}

			// We only really care about the first and primary resource....
			if( maxResourceAmount > 0 )
				break;
		}

		if( maxResourceAmount > 1 )
		{
			// Calculate amount of resources returned based on player's mining skill, item's wear and tear,
			// and amount of resources that went into making the item in the first place
			if( ourObj.health >= 1 || ourObj.usesLeft >= 1 )
			{
				var healthPercentage = 0;
				if( ourObj.health >= 1 )
				{
					healthPercentage = Math.floor( ( ourObj.health * 100 ) / ourObj.maxhp );
				}

				var usesPercentage = 0;
				if( ourObj.usesLeft >= 1 )
				{
					usesPercentage = Math.floor( ( ourObj.usesLeft * 100 ) / ourObj.maxUses );
				}

				var itemPercentage = usesPercentage > 0 ? Math.min( healthPercentage, usesPercentage ) : healthPercentage;

				// Reduce amount of resources returned based on state of object's wear and tear
				resourceAmount = Math.floor( ( maxResourceAmount * itemPercentage ) / 100 );
			}

			// Halve the amount of resources returned
			resourceAmount = Math.max( Math.floor( resourceAmount / 2 ), 1 );

			// Fetch player's Tailoring skill
			var playerSkill = mChar.skills.tailoring;

			// Based on player's Tailoring skill, return between 1 to maxResourceAmount
			resourceAmount = Math.min( Math.max( Math.floor( resourceAmount * ( playerSkill / 1000 )), 1 ), resourceAmount );
		}
		else
		{
			resourceAmount = 1;
		}
	}

	if( resourceAmount == 0 || resourceID == 0 )
	{
		// Targeted object is not an item that can be unravelled
		mChar.SetTempTag( "prevActionResult", "CANTUNRAVEL" );
		mChar.StartTimer( repairDelay, 1, true );
		return;
	}

	// Delete the unravelled item
	ourObj.Delete();

	// Run a generic skill check to give player a chance to increase their tailoring skill
	mChar.CheckSkill( 34, 0, 1000 );

	// Determine the actual resource item to add to player's backpack
	// We'll default to one specific resource per material type
	var itemToAdd = "";
	switch( materialType )
	{
		case "cloth":
			itemToAdd = "0x1766"; // cut cloth
			break;
		case "leather":
			itemToAdd = "0x1068"; // cut leather
			break;
		default:
			break;
	}
	var newResource = CreateDFNItem( pSock, mChar, itemToAdd, resourceAmount, "ITEM", true );

	mChar.SetTempTag( "resourceFromUnravelling", resourceAmount );
	mChar.SetTempTag( "prevActionResult", "UNRAVELSUCCESS" );
	mChar.StartTimer( gumpDelay, 1, true );
}