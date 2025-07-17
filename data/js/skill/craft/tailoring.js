const textHue = 0x480;					// Color hue for all text in the crafting gump
const scriptID = 4030;					// Script ID used to identify and close this gump
const gumpDelay = 2000;					// Delay (ms) before gump reappears after crafting
const repairDelay = 200;				// Delay (ms) before gump reappears after selecting a resource
const itemDetailsScriptID = 4026;		// Script ID used to show item detail tooltips
const craftGumpID = 4027;				// TriggerEvent ID used to build the crafting gump UI
const itemsPerPage = 10;				// Number of craftable items shown per gump subpage
const displayUnlearnedRecipes = true;	// Show recipes player hasn’t learned

//////////////////////////////////////////////////////////////////////////////////////////
// UOX3 Tailoring CraftingMap Configuration
//
// Description:
// This file defines the `CraftingMap` used to populate the tailoring crafting gump in UOX3.
// Each entry links a craftable item (defined in `create/tailoring.dfn`) to its display name,
// gump page, and crafting behavior. This setup powers the dynamic UI for tailoring.
//
// Data Sources:
// - `makeID` refers to the unique item ID defined in `create/tailoring.dfn`.
// - `dictID` refers to a localized string entry defined in `dictionaries/dictionary.eng`.
// - `customName` is an optional hardcoded string that overrides dictionary text if present.
// - `recipeID` is an optional ID that locks the item behind a learned recipe.
//
// Entry Format:
//   makeID: {
//     dictID: <dictionaryID>,       // (Optional) Used for localized names via dictionary.eng
//     customName: "<item name>",    // (Optional) Overrides dictionary name if present
//     page: <pageNumber>,           // Gump page to display item on
//     timerID: <timerID>,           // Timer used to re-show gump after crafting
//     recipeID: <recipeID>          // (Optional) Requires the player to learn the recipe before crafting
//   }
//
// Display Logic:
// - If `customName` is present, it is used directly in the crafting gump.
// - If `dictID` is present (and `customName` is not), `GetDictionaryEntry()` is used.
// - If both are missing, the gump will show a fallback like `[Unnamed Item: ####]`.
// - The `page` field determines which category tab the item appears under.
// - If `recipeID` is present and the player has not learned it, the item will either be:
//     - Hidden entirely (default behavior), or
//     - Displayed but uncraftable, depending on `displayUnlearnedRecipes` setting.
//
// Example Entries:
//
// // Standard item using dictionary ID for localization
// 130: { dictID: 11415, page: 1, timerID: 1 },
//
// // Custom item with hardcoded display name, no dictionary entry required
// 999: { customName: "harrys sword", page: 1, timerID: 1 },
//
// // Recipe-locked item (only craftable if recipe is learned)
// – 185: { dictID: 11469, page: 8, timerID: 8, recipeID: 185 },
//
// Organization:
// - Items are grouped by `page` value (e.g., Hats, Armor, etc.).
// - The crafting gump dynamically builds its layout from this CraftingMap.
// - This system supports localized, custom, hybrid, and recipe-locked crafting menus.
//
//////////////////////////////////////////////////////////////////////////////////////////

const CraftingMap = {
	// Hats (Page 1)
	130: { dictID: 11415, page: 1, timerID: 1 },
	131: { dictID: 11416, page: 1, timerID: 1 },
	132: { dictID: 11417, page: 1, timerID: 1 },
	134: { dictID: 11418, page: 1, timerID: 1 },
	133: { dictID: 11419, page: 1, timerID: 1 },
	136: { dictID: 11420, page: 1, timerID: 1 },
	137: { dictID: 11421, page: 1, timerID: 1 },
	138: { dictID: 11422, page: 1, timerID: 1 },
	139: { dictID: 11423, page: 1, timerID: 1 },
	140: { dictID: 11424, page: 1, timerID: 1 },
	141: { dictID: 11425, page: 1, timerID: 1 },
	135: { dictID: 11470, page: 1, timerID: 1 },

	// Shirts & Pants (Page 2)
	142: { dictID: 11426, page: 2, timerID: 2 },
	143: { dictID: 11427, page: 2, timerID: 2 },
	144: { dictID: 11428, page: 2, timerID: 2 },
	145: { dictID: 11429, page: 2, timerID: 2 },
	146: { dictID: 11430, page: 2, timerID: 2 },
	147: { dictID: 11431, page: 2, timerID: 2 },
	148: { dictID: 11432, page: 2, timerID: 2 },
	149: { dictID: 11433, page: 2, timerID: 2 },
	150: { dictID: 11434, page: 2, timerID: 2 },
	151: { dictID: 11435, page: 2, timerID: 2 },
	180: { dictID: 11436, page: 2, timerID: 2 },
	152: { dictID: 11437, page: 2, timerID: 2 },
	153: { dictID: 11438, page: 2, timerID: 2 },
	154: { dictID: 11439, page: 2, timerID: 2 },

	// Misc (Page 3)
	155: { dictID: 11440, page: 3, timerID: 3 },
	156: { dictID: 11441, page: 3, timerID: 3 },
	157: { dictID: 11442, page: 3, timerID: 3 },
	158: { dictID: 11443, page: 3, timerID: 3 },

	// Footwear (Page 4)
	159: { dictID: 11444, page: 4, timerID: 4 },
	160: { dictID: 11445, page: 4, timerID: 4 },
	161: { dictID: 11446, page: 4, timerID: 4 },
	162: { dictID: 11447, page: 4, timerID: 4 },

	// Leather Armor (Page 5)
	163: { dictID: 11448, page: 5, timerID: 5 },
	164: { dictID: 11449, page: 5, timerID: 5 },
	165: { dictID: 11450, page: 5, timerID: 5 },
	166: { dictID: 11451, page: 5, timerID: 5 },
	167: { dictID: 11452, page: 5, timerID: 5 },
	168: { dictID: 11453, page: 5, timerID: 5 },

	// Studded Armor (Page 6)
	169: { dictID: 11454, page: 6, timerID: 6 },
	170: { dictID: 11455, page: 6, timerID: 6 },
	171: { dictID: 11456, page: 6, timerID: 6 },
	172: { dictID: 11457, page: 6, timerID: 6 },
	173: { dictID: 11458, page: 6, timerID: 6 },

	// Female Armor (Page 7)
	174: { dictID: 11459, page: 7, timerID: 7 },
	175: { dictID: 11460, page: 7, timerID: 7 },
	176: { dictID: 11461, page: 7, timerID: 7 },
	177: { dictID: 11462, page: 7 , timerID: 7},
	178: { dictID: 11463, page: 7, timerID: 7 },
	179: { dictID: 11464, page: 7, timerID: 7 },

	// Bone Armor (Page 8)
	181: { dictID: 11465, page: 8, timerID: 8 },
	182: { dictID: 11466, page: 8, timerID: 8 },
	183: { dictID: 11467, page: 8, timerID: 8 },
	184: { dictID: 11468, page: 8, timerID: 8 },
	185: { dictID: 11469, page: 8, timerID: 8 }
};

function PageX( socket, pUser, pageNum )
{
	let subPage = pUser.GetTempTag( "subPage" ) || 1;

	// Build pages dynamically from CraftingMap
	let myPage = [];
	let dictToMakeID = {}; // local reverse map

	for( let makeID in CraftingMap ) 
	{
		let data = CraftingMap[makeID];
		let page = data.page;
		if( !myPage[page - 1] )
			myPage[page - 1] = [];

		let needsRecipe = data.recipeID;
		let showAll = displayUnlearnedRecipes;

		if( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe ) )
		{
			myPage[page - 1].push( data.dictID );
		}

		dictToMakeID[data.dictID] = parseInt( makeID );
	}

	let pageItems;

	if( pageNum == 999 )
	{
		let lastTenRaw = pUser.GetTag( "LastTenTailoring" ) || "";
		let split = lastTenRaw.split( "," );
		pageItems = [];

		for( var i = 0; i < split.length; i++ )
		{
			let val = parseInt(split[i]);
			if( !isNaN( val ))
				pageItems.push( val );
		}
	}
	else
	{
		if( pageNum < 1 || pageNum > myPage.length )
			pageNum = 1;

		pageItems = myPage[pageNum - 1];
	}

	let totalSubPages = Math.ceil( pageItems.length / itemsPerPage );

	if( subPage < 1 )
		subPage = 1;
	if( subPage > totalSubPages )
		subPage = totalSubPages;

	pUser.SetTempTag( "page", pageNum );
	pUser.SetTempTag( "subPage", subPage );

	let startIndex = ( subPage - 1 ) * itemsPerPage;
	let endIndex = Math.min( startIndex + itemsPerPage, pageItems.length );

	if( startIndex >= pageItems.length ) 
	{
		subPage = 1;
		startIndex = 0;
		endIndex = Math.min( itemsPerPage, pageItems.length );
		pUser.SetTempTag( "subPage", subPage );
	}

	let tailoringMenu = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", tailoringMenu, socket );
	tailoringMenu.AddPage( 1 );

	for( let i = startIndex; i < endIndex; i++)
	{
		let index = i - startIndex;
		let makeID, entryID, entryText, buttonID;

		if( pageNum == 999 )
		{
			makeID = pageItems[i];
		}
		else
		{
			entryID = pageItems[i];
			makeID = dictToMakeID[entryID];
		}

	let data = CraftingMap[makeID];

	if( !data )
	{
		entryText = "[Missing MakeID: " + makeID + "]";
		buttonID = makeID;
	}
	else
	{
		buttonID = makeID;

		if( data.customName )
		{
			entryText = data.customName;
		}
		else if( data.dictID )
		{
			entryText = GetDictionaryEntry( data.dictID, socket.language );
			if( !entryText || entryText === "" )
				entryText = "[Missing EntryID: " + data.dictID + "]";
		}
		else
		{
			entryText = "[Unnamed Item: " + makeID + "]";
		}
	}

	tailoringMenu.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
	tailoringMenu.AddText( 255, 60 + ( index * 20 ), textHue, entryText );
	tailoringMenu.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2000 + buttonID );
}

	if( subPage > 1 )
	{
		tailoringMenu.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		tailoringMenu.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.language ) + "</basefont>" );
	}

	if( subPage < totalSubPages )
	{
		tailoringMenu.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		tailoringMenu.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.language ) + "</basefont>" );
	}

	tailoringMenu.Send( socket );
	tailoringMenu.Free();
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	let socket = pUser.socket;

	if( timerID >= 1 && timerID <= 8 )
	{
		PageX( socket, pUser, timerID ); // Pages 1–8
	}
	else if ( timerID == 999 )
	{
		PageX(socket, pUser, 999); // Last Ten
	}
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var usedMakeLast = false;

	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = socket.tempObj;
	if( !ValidateObject( bItem ) || !pUser.InRange( bItem, 3 ))
	{
		socket.SysMessage( GetDictionaryEntry( 461, socket.language ));
		return;
	}

	if( bItem.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 6031, socket.language ));
		return;
	}

	var iPackOwner = GetPackOwner(bItem, 0);
	if( ValidateObject( iPackOwner ))
	{
		if( iPackOwner.serial != pUser.serial )
		{
			socket.SysMessage(GetDictionaryEntry( 6032, socket.language ));
			return;
		}
	}
	else
	{
		socket.SysMessage(GetDictionaryEntry( 6022, socket.language ));
		return;
	}

	if( pButton >= 8001 && pButton < 9000 )
	{
		let subPage = pButton - 8000;
		let pageNum = pUser.GetTempTag( "page" ) || 1;
		pUser.SetTempTag( "subPage", subPage );
		PageX(socket, pUser, pageNum);
		return;
	}

	if( pButton >= 9001 && pButton < 10000 )
	{
		let subPage = pButton - 9000;
		let pageNum = pUser.GetTempTag( "page" ) || 1;
		pUser.SetTempTag("subPage", subPage);
		PageX( socket, pUser, pageNum );
		return;
	}

	if( pButton >= 1 && pButton <= 8 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		PageX( socket, pUser, pButton );
		return;
	}

	if( pButton == 11000 )
	{
		pUser.SetTempTag( "page", 999 );
		pUser.SetTempTag( "subPage", 1 );
		PageX(socket, pUser, 999);
		return;
	}

	var makeID = 0;
	var timerID = 0;

	// Handle "Make Last"
	if(( pButton >= 100 && pButton <= 804 ) || pButton == 5000 )
	{
		if( pButton == 5000 )
		{
			pButton = pUser.GetTempTag( "MAKELAST" );
			usedMakeLast = true;
		}
		else
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
	}

	// If it's a craft button (found in map)
	if( CraftingMap[pButton] != undefined )
	{
		makeID = pButton;
		timerID = CraftingMap[makeID].timerID || 1;

		let materialHue = pUser.GetTempTag( "LastResourceColor" );
		if(( makeID >= 100 && makeID <= 158 ) || makeID == 180 )
		{
			if( usedMakeLast && materialHue != null )
			{
				// Check if recipe required and not known
				let data = CraftingMap[makeID];
				if( data.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, data.recipeID ))
				{
					socket.SysMessage("You must learn that recipe from a scroll.");
					return;
				}
				MakeItem( socket, pUser, makeID, materialHue );
				AddToLastTen( pUser, makeID );
				pUser.StartTimer( gumpDelay, timerID, 4030 );
			}
			else
			{
				pUser.SetTempTag( "makeID", makeID );
				pUser.SetTempTag( "timerID", timerID );
				socket.CustomTarget(1, GetDictionaryEntry( 444, socket.language ));
			}
		}
		else
		{
			// Check if recipe required and not known
			let data = CraftingMap[makeID];
			if( data.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, data.recipeID ))
			{
				socket.SysMessage("You must learn that recipe from a scroll.");
				return;
			}
			MakeItem( socket, pUser, makeID );
			AddToLastTen( pUser, makeID );
			if( GetServerSetting( "ToolUseLimit" ))
			{
				bItem.usesLeft -= 1;
				if( bItem.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
				{
					bItem.Delete();
					socket.SysMessage( GetDictionaryEntry( 10202, socket.language ));
				}
			}
			pUser.StartTimer( gumpDelay, timerID, 4030 );
		}
		return;
	}

	// If it's a detail button (2000+ button ID pattern)
	if( pButton >= 2000 && pButton <= 3000 )
	{
		let makeID = pButton - 2000;
		if( CraftingMap[makeID] )
		{
			pUser.SetTempTag( "ITEMDETAILS", makeID );
			TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
		}
		return;
	}

	if( pButton == 0 ) // Exit/Close
	{
		let gumpID = scriptID + 0xffff;
		pUser.SetTempTag( "MAKELAST", null );
		pUser.SetTempTag( "CRAFT", null );
		socket.CloseGump( gumpID, 0 );
		return;
	}

	if( pButton == 52 ) // Unravel Item
	{
		UnravelTarget( socket );
		return;
	}
}

function onCallback1( socket, ourObj )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Fetch makeID and timerID from temp tag
	var makeID = pUser.GetTempTag( "makeID" );
	var timerID = pUser.GetTempTag( "timerID" );
	pUser.SetTempTag( "makeID", null );
	pUser.SetTempTag( "timerID", null );

	var bItem = socket.tempObj;
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
					socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That resource is in someone else's backpack!
					return;
				}
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // This has to be in your backpack before you can use it.
				return;
			}

			//Cache cloth color for future "Make Last"
			pUser.SetTempTag( "LastResourceColor", ourObj.colour );

			// Check if recipe required and not known
			let data = CraftingMap[makeID];
			if( data.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, data.recipeID ))
			{
				socket.SysMessage("You must learn that recipe from a scroll.");
				return;
			}

			// Pass in the colour of the desired material to use for crafting
			MakeItem( socket, pUser, makeID, ourObj.colour );
			AddToLastTen( pUser, makeID );
			if( GetServerSetting( "ToolUseLimit" ))
			{
				bItem.usesLeft -= 1;
				if( bItem.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
				{
					bItem.Delete();
					socket.SysMessage( GetDictionaryEntry( 10202, socket.language )); // You have worn out your tool!
					// Play sound effect of tool breaking
				}
			}
			pUser.StartTimer( gumpDelay, timerID, 4030 );
		}
	}
}

function UnravelTarget( socket )
{
	socket.CustomTarget( 2, GetDictionaryEntry( 10295, socket.language )); // What item would you like to unravel?
}

// Clothes and leather armor can be unravelled back into cloth and leather
function onCallback2( socket, ourObj )
{
	// Unravel item, get cloth/leather in return
	var mChar = socket.currentChar;

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
			socket.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOUNRAVELSKILL" );
			mChar.StartTimer( gumpDelay, 1, 4030 );
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
		mChar.StartTimer( repairDelay, 1, 4030 );
		return;
	}

	// Delete the unravelled item
	ourObj.Delete();

	// Run a generic skill check to give player a chance to increase their tailoring skill
	mChar.CheckSkill( 34, 0, mChar.skillCaps.tailoring );

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
	var newResource = CreateDFNItem( socket, mChar, itemToAdd, resourceAmount, "ITEM", true, resourceColor );

	mChar.SetTempTag( "resourceFromUnravelling", resourceAmount );
	mChar.SetTempTag( "prevActionResult", "UNRAVELSUCCESS" );
	mChar.StartTimer( gumpDelay, 1, 4030 );
}

function AddToLastTen( pUser, makeID )
{
	// Append makeID to last ten list
	var raw = pUser.GetTag( "LastTenTailoring" ) || "";
	var list = raw.split( "," );

	// Remove if already in list
	for( var i = 0; i < list.length; i++ )
	{
		if( parseInt( list[i]) == makeID )
		{
			list.splice(i, 1);
			break;
		}
	}

	// Add to front (no unshift in SpiderMonkey 1.8)
	var newList = [makeID];
	for( var i = 0; i < list.length && newList.length < 10; i++ )
	{
		var entry = parseInt( list[i] );
		if( !isNaN( entry ) && entry > 0 )
			newList.push( entry );
	}

	pUser.SetTag( "LastTenTailoring", newList.join( "," ));
}

function HasLearnedRecipe( pUser, recipeID )
{
	var myData = TriggerEvent( 4022, "ReadRecipeID", pUser );
	if( !myData || myData.length == 0)
		return false;

	for( let i = 0; i < myData.length; i++ )
	{
		let data = myData[i].split( "," );
		if( data[0] == recipeID )
			return true;
	}
	return false;
}