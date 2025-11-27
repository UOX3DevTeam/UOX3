/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue                 = 0x480;                 // Color hue for all text in the crafting gump
const tailoringID             = 4030;                  // Script ID used to identify and close this gump
const gumpDelay               = 2000;                  // Delay (ms) before gump reappears after crafting
const repairDelay             = 200;                   // Delay (ms) before gump reappears after selecting a resource
const itemDetailsScriptID     = 4026;                  // Script ID used to show item detail tooltips
const craftGumpID             = 4027;                  // TriggerEvent ID used to build the crafting gump UI
const itemsPerPage            = 10;                    // Number of craftable items shown per gump subpage
const displayUnlearnedRecipes = true;                  // Show recipes player hasn't learned
const coreShardEra            = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const tailoringSkillID        = 34;                    // Tailoring skill index

//////////////////////////////////////////////////////////////////////////////////////////
// Tailoring CraftingMap
//////////////////////////////////////////////////////////////////////////////////////////

// You can later add:
//   skill, minEra, maxEra, recipeID, harvest, harvestNames, customName
// per entry, like in Alchemy / Tinkering.

const CraftingMap = {
	// Hats (Page 1)
	130: { dictID: 11415, page: 1, timerID: 1, harvest: [ 10016 ] },
	131: { dictID: 11416, page: 1, timerID: 1, harvest: [ 10016 ] },
	132: { dictID: 11417, page: 1, timerID: 1, harvest: [ 10016 ] },
	134: { dictID: 11418, page: 1, timerID: 1, harvest: [ 10016 ] },
	133: { dictID: 11419, page: 1, timerID: 1, harvest: [ 10016 ] },
	136: { dictID: 11420, page: 1, timerID: 1, harvest: [ 10016 ] },
	137: { dictID: 11421, page: 1, timerID: 1, harvest: [ 10016 ] },
	138: { dictID: 11422, page: 1, timerID: 1, harvest: [ 10016 ] },
	139: { dictID: 11423, page: 1, timerID: 1, harvest: [ 10016 ] },
	140: { dictID: 11424, page: 1, timerID: 1, harvest: [ 10016 ] },
	141: { dictID: 11425, page: 1, timerID: 1, harvest: [ 10016 ] },
	135: { dictID: 11470, page: 1, timerID: 1, harvest: [ 10016 ] },

	// Shirts & Pants (Page 2)
	142: { dictID: 11426, page: 2, timerID: 2, harvest: [ 10016 ] },
	143: { dictID: 11427, page: 2, timerID: 2, harvest: [ 10016 ] },
	144: { dictID: 11428, page: 2, timerID: 2, harvest: [ 10016 ] },
	145: { dictID: 11429, page: 2, timerID: 2, harvest: [ 10016 ] },
	146: { dictID: 11430, page: 2, timerID: 2, harvest: [ 10016 ] },
	147: { dictID: 11431, page: 2, timerID: 2, harvest: [ 10016 ] },
	148: { dictID: 11432, page: 2, timerID: 2, harvest: [ 10016 ] },
	149: { dictID: 11433, page: 2, timerID: 2, harvest: [ 10016 ] },
	150: { dictID: 11434, page: 2, timerID: 2, harvest: [ 10016 ] },
	151: { dictID: 11435, page: 2, timerID: 2, harvest: [ 10016 ] },
	180: { dictID: 11436, page: 2, timerID: 2, harvest: [ 10016 ] },
	152: { dictID: 11437, page: 2, timerID: 2, harvest: [ 10016 ] },
	153: { dictID: 11438, page: 2, timerID: 2, harvest: [ 10016 ] },
	154: { dictID: 11439, page: 2, timerID: 2, harvest: [ 10016 ] },

	// Misc (Page 3)
	155: { dictID: 11440, page: 3, timerID: 3, harvest: [ 10016 ] },
	156: { dictID: 11441, page: 3, timerID: 3, harvest: [ 10016 ] },
	157: { dictID: 11442, page: 3, timerID: 3, harvest: [ 10016 ] },
	158: { dictID: 11443, page: 3, timerID: 3, harvest: [ 10016 ] },

	// Footwear (Page 4)
	159: { dictID: 11444, page: 4, timerID: 4, harvest: [ 10007 ] },
	160: { dictID: 11445, page: 4, timerID: 4, harvest: [ 10007 ] },
	161: { dictID: 11446, page: 4, timerID: 4, harvest: [ 10007 ] },
	162: { dictID: 11447, page: 4, timerID: 4, harvest: [ 10007 ] },

	// Leather Armor (Page 5)
	163: { dictID: 11448, page: 5, timerID: 5, harvest: [ 10007 ] },
	164: { dictID: 11449, page: 5, timerID: 5, harvest: [ 10007 ] },
	165: { dictID: 11450, page: 5, timerID: 5, harvest: [ 10007 ] },
	166: { dictID: 11451, page: 5, timerID: 5, harvest: [ 10007 ] },
	167: { dictID: 11452, page: 5, timerID: 5, harvest: [ 10007 ] },
	168: { dictID: 11453, page: 5, timerID: 5, harvest: [ 10007 ] },

	// Studded Armor (Page 6)
	169: { dictID: 11454, page: 6, timerID: 6, harvest: [ 10007 ] },
	170: { dictID: 11455, page: 6, timerID: 6, harvest: [ 10007 ] },
	171: { dictID: 11456, page: 6, timerID: 6, harvest: [ 10007 ] },
	172: { dictID: 11457, page: 6, timerID: 6, harvest: [ 10007 ] },
	173: { dictID: 11458, page: 6, timerID: 6, harvest: [ 10007 ] },

	// Female Armor (Page 7)
	174: { dictID: 11459, page: 7, timerID: 7, harvest: [ 10007 ] },
	175: { dictID: 11460, page: 7, timerID: 7, harvest: [ 10007 ] },
	176: { dictID: 11461, page: 7, timerID: 7, harvest: [ 10007 ] },
	177: { dictID: 11462, page: 7, timerID: 7, harvest: [ 10007 ] },
	178: { dictID: 11463, page: 7, timerID: 7, harvest: [ 10007 ] },
	179: { dictID: 11464, page: 7, timerID: 7, harvest: [ 10007 ] },

	// Bone Armor (Page 8)
	181: { dictID: 11465, page: 8, timerID: 8, harvest: [ 10007, 10008 ] },
	182: { dictID: 11466, page: 8, timerID: 8, harvest: [ 10007, 10008 ] },
	183: { dictID: 11467, page: 8, timerID: 8, harvest: [ 10007, 10008 ] },
	184: { dictID: 11468, page: 8, timerID: 8, harvest: [ 10007, 10008 ] },
	185: { dictID: 11469, page: 8, timerID: 8, harvest: [ 10007, 10008 ] } // can add recipeID/minEra here later
};

// Fill defaults (skill, etc.)
(function initTailoringMap()
{
	for( var key in CraftingMap )
	{
		if( !CraftingMap.hasOwnProperty( key ))
			continue;

		var entry = CraftingMap[key];
		if( entry.skill === undefined )
			entry.skill = tailoringSkillID;
	}
})();

/** @type { ( socket: Socket, pUser: Character, pageNum: number ) => void } */
function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	var subPage = pUser.GetTempTag( "subPage" ) || 1;
	var pageItems;

	// Last Ten page
	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenTailoring" ) || "";
		var split = lastTenRaw.split( "," );
		pageItems = [];

		for( var i = 0; i < split.length; i++ )
		{
			var val = parseInt( split[i] );
			if( !isNaN( val ))
				pageItems.push( val ); // makeID
		}
	}
	else
	{
		// Collect all makeIDs for this page
		var makeIDs = [];
		for( var key in CraftingMap )
		{
			if( !CraftingMap.hasOwnProperty( key ))
				continue;

			var makeID = parseInt( key );
			var data = CraftingMap[makeID];
			if( !data || data.page != pageNum )
				continue;

			makeIDs.push( makeID );
		}

		// Sort by dictID so order matches dictionary sequence
		makeIDs.sort( function( a, b )
		{
			var ea = CraftingMap[a];
			var eb = CraftingMap[b];
			if( ea && eb )
				return ( ea.dictID || 0 ) - ( eb.dictID || 0 );
			return a - b;
		});

		// Era / recipe filtering
		pageItems = [];
		for( var k = 0; k < makeIDs.length; k++ )
		{
			var id = makeIDs[k];
			var data2 = CraftingMap[id];
			if( !data2 )
				continue;

			var needsRecipe = data2.recipeID;
			var showAll = displayUnlearnedRecipes;

			if( eraOK( data2 ) && ( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe )) )
				pageItems.push( id );
		}

		// Fallback if page empty (and not page 1)
		if( pageItems.length == 0 && pageNum != 1 )
		{
			pageNum = 1;

			makeIDs = [];
			for( var key2 in CraftingMap )
			{
				if( !CraftingMap.hasOwnProperty( key2 ))
					continue;

				var mid2 = parseInt( key2 );
				var d3 = CraftingMap[mid2];
				if( !d3 || d3.page != 1 )
					continue;

				makeIDs.push( mid2 );
			}

			makeIDs.sort( function( a, b )
			{
				var ea2 = CraftingMap[a];
				var eb2 = CraftingMap[b];
				if( ea2 && eb2 )
					return ( ea2.dictID || 0 ) - ( eb2.dictID || 0 );
				return a - b;
			});

			pageItems = [];
			for( var m = 0; m < makeIDs.length; m++ )
			{
				var id2 = makeIDs[m];
				var data4 = CraftingMap[id2];
				if( !data4 )
					continue;

				var needsRecipe2 = data4.recipeID;
				var showAll2 = displayUnlearnedRecipes;

				if( eraOK( data4 ) && ( !needsRecipe2 || showAll2 || HasLearnedRecipe( pUser, needsRecipe2 )) )
					pageItems.push( id2 );
			}
		}
	}

	// Subpage handling
	var totalSubPages = Math.ceil( pageItems.length / itemsPerPage );
	if( totalSubPages < 1 )
		totalSubPages = 1;
	if( subPage < 1 )
		subPage = 1;
	if( subPage > totalSubPages )
		subPage = totalSubPages;

	pUser.SetTempTag( "page", pageNum );
	pUser.SetTempTag( "subPage", subPage );

	var startIndex = ( subPage - 1 ) * itemsPerPage;
	var endIndex   = Math.min( startIndex + itemsPerPage, pageItems.length );

	if( startIndex >= pageItems.length )
	{
		subPage    = 1;
		startIndex = 0;
		endIndex   = Math.min( itemsPerPage, pageItems.length );
		pUser.SetTempTag( "subPage", subPage );
	}

	var tailoringMenu = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", tailoringMenu, socket );
	tailoringMenu.AddPage( 1 );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index  = j - startIndex;
		var makeID = pageItems[j];
		var entryText;
		var buttonID = makeID; // craft button uses makeID directly
		var data5 = CraftingMap[makeID];

		if( !data5 )
		{
			entryText = "[Missing MakeID: " + makeID + "]";
		}
		else
		{
			if( data5.customName )
			{
				entryText = data5.customName;
			}
			else if( data5.dictID )
			{
				entryText = GetDictionaryEntry( data5.dictID, socket.language );
				if( !entryText || entryText === "" )
					entryText = "[Missing EntryID: " + data5.dictID + "]";
			}
			else
			{
				entryText = "[Unnamed Item: " + makeID + "]";
			}
		}

		tailoringMenu.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
		tailoringMenu.AddText(   255, 60 + ( index * 20 ), textHue, entryText );
		// Detail button: 20000 + makeID (new system pattern)
		tailoringMenu.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + buttonID );
	}

	// Prev subpage
	if( subPage > 1 )
	{
		tailoringMenu.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		tailoringMenu.AddHTMLGump( 255, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		tailoringMenu.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		tailoringMenu.AddHTMLGump( 405, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );
	}

	tailoringMenu.Send( socket );
	tailoringMenu.Free();
}

/** @type { ( pUser: Character, timerID: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	if( timerID >= 1 && timerID <= 8 )
	{
		PageX( socket, pUser, timerID ); // Pages 1–8
	}
	else if( timerID == 999 )
	{
		PageX( socket, pUser, 999 ); // Last Ten
	}
}

/** @type { ( socket: Socket, pButton: number, gumpData: GumpData ) => void } */
function onGumpPress( socket, pButton, gumpData )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
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

	var iPackOwner = GetPackOwner( bItem, 0 );
	if( ValidateObject( iPackOwner ))
	{
		if( iPackOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 6032, socket.language ));
			return;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6022, socket.language ));
		return;
	}

	var gumpID = tailoringID + 0xffff;

	// Subpage back/forward
	if( pButton >= 8001 && pButton < 9000 )
	{
		var subPage = pButton - 8000;
		var pageNum = pUser.GetTempTag( "page" ) || 1;
		pUser.SetTempTag( "subPage", subPage );
		PageX( socket, pUser, pageNum );
		return;
	}

	if( pButton >= 9001 && pButton < 10000 )
	{
		var subPage2 = pButton - 9000;
		var pageNum2 = pUser.GetTempTag( "page" ) || 1;
		pUser.SetTempTag( "subPage", subPage2 );
		PageX( socket, pUser, pageNum2 );
		return;
	}

	// Page tabs (1–8)
	if( pButton >= 1 && pButton <= 8 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		PageX( socket, pUser, pButton );
		return;
	}

	// Last Ten
	if( pButton == 11000 )
	{
		pUser.SetTempTag( "page", 999 );
		pUser.SetTempTag( "subPage", 1 );
		PageX( socket, pUser, 999 );
		return;
	}

	// Close gump
	if( pButton == 0 )
	{
		pUser.SetTempTag( "MakeLast_Tailoring", null );
		pUser.SetTempTag( "CRAFT", null );
		socket.CloseGump( gumpID, 0 );
		return;
	}

	// Unravel button
	if( pButton == 52 )
	{
		UnravelTarget( socket );
		return;
	}

	var usedMakeLast = false;
	var makeID = 0;
	var timerID = 0;

	// Make Last
	if( pButton == 5000 )
	{
		var last = pUser.GetTempTag( "MakeLast_Tailoring" );
		if( last )
		{
			pButton = last;
			usedMakeLast = true;
		}
		else
		{
			return;
		}
	}

	// Craft buttons use makeID directly (if in map)
	if( CraftingMap[pButton] != undefined )
	{
		makeID = pButton;
		var data = CraftingMap[makeID];
		timerID = data.timerID || 1;

		// Era / recipe checks
		if( !eraOK( data ))
		{
			socket.SysMessage( "That item is not available in this era." );
			return;
		}
		if( data.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, data.recipeID ))
		{
			socket.SysMessage( "You must learn that recipe from a scroll." );
			return;
		}

		pUser.SetTempTag( "MakeLast_Tailoring", makeID );

		var materialHue = pUser.GetTempTag( "LastResourceColor" );

		// Cloth items: select cloth (colour) unless Make Last + cached hue
		if(( makeID >= 100 && makeID <= 158 ) || makeID == 180 )
		{
			if( usedMakeLast && materialHue != null )
			{
				MakeItem( socket, pUser, makeID, materialHue );
				AddToLastTen( pUser, makeID );
				pUser.StartTimer( gumpDelay, timerID, tailoringID );
			}
			else
			{
				pUser.SetTempTag( "makeID", makeID );
				pUser.SetTempTag( "timerID", timerID );
				socket.CustomTarget( 1, GetDictionaryEntry( 444, socket.language )); // Select material:
			}
		}
		else
		{
			// Non-colour-selected tailoring (leather/bone etc.)
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
			pUser.StartTimer( gumpDelay, timerID, tailoringID );
		}
		return;
	}

	// Detail buttons: 20000 + makeID (new system)
	if( pButton >= 20000 && pButton < 30000 )
	{
		var detailMakeID = pButton - 20000;
		var entry = CraftingMap[detailMakeID];
		if( entry )
		{
			// Which item to show
			pUser.SetTempTag( "ITEMDETAILS", detailMakeID );

			// Skill used
			pUser.SetTempTag( "Skill", entry.skill || tailoringSkillID );

			// Clear old harvest tags
			pUser.SetTempTag( "Harvest",  null );
			pUser.SetTempTag( "Harvest2", null );
			pUser.SetTempTag( "Harvest3", null );
			pUser.SetTempTag( "Harvest4", null );

			// Clear old custom harvest names
			pUser.SetTempTag( "HarvestName",  null );
			pUser.SetTempTag( "Harvest2Name", null );
			pUser.SetTempTag( "Harvest3Name", null );
			pUser.SetTempTag( "Harvest4Name", null );

			// Optional harvest dictIDs (cloth, leather, bone etc.) – add later per item
			if( entry.harvest && entry.harvest.length > 0 )
			{
				if( entry.harvest.length >= 1 )
					pUser.SetTempTag( "Harvest",  entry.harvest[0] );
				if( entry.harvest.length >= 2 )
					pUser.SetTempTag( "Harvest2", entry.harvest[1] );
				if( entry.harvest.length >= 3 )
					pUser.SetTempTag( "Harvest3", entry.harvest[2] );
				if( entry.harvest.length >= 4 )
					pUser.SetTempTag( "Harvest4", entry.harvest[3] );
			}

			// Optional custom resource names (e.g. "Cloth", "Leather")
			if( entry.harvestNames && entry.harvestNames.length > 0 )
			{
				if( entry.harvestNames.length >= 1 )
					pUser.SetTempTag( "HarvestName",  entry.harvestNames[0] );
				if( entry.harvestNames.length >= 2 )
					pUser.SetTempTag( "Harvest2Name", entry.harvestNames[1] );
				if( entry.harvestNames.length >= 3 )
					pUser.SetTempTag( "Harvest3Name", entry.harvestNames[2] );
				if( entry.harvestNames.length >= 4 )
					pUser.SetTempTag( "Harvest4Name", entry.harvestNames[3] );
			}

			if( entry.recipeID && entry.recipeID > 0 )
				pUser.SetTempTag( "needRecipeID", entry.recipeID );
			else
				pUser.SetTempTag( "needRecipeID", 0 );

			TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
		}
		return;
	}
}

/** @type { ( socket: Socket, ourObj: Character | Item | null ) => void } */
function onCallback1( socket, ourObj )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Fetch makeID and timerID from temp tag
	var makeID  = pUser.GetTempTag( "makeID" );
	var timerID = pUser.GetTempTag( "timerID" );
	pUser.SetTempTag( "makeID", null );
	pUser.SetTempTag( "timerID", null );

	var bItem = socket.tempObj;
	if( !ValidateObject( bItem ))
		return;

	if( ValidateObject( ourObj ) && ourObj.isItem )
	{
		// Make sure targeted item is in player's backpack
		var iPackOwner = GetPackOwner( ourObj, 0 );
		if( ValidateObject( iPackOwner ))
		{
			if( iPackOwner.serial != pUser.serial )
			{
				socket.SysMessage( GetDictionaryEntry( 6032, socket.language ));
				return;
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6022, socket.language ));
			return;
		}

		// Cache cloth color for Make Last
		pUser.SetTempTag( "LastResourceColor", ourObj.colour );

		// Recipe re-check (just in case)
		var data = CraftingMap[makeID];
		if( data && data.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, data.recipeID ))
		{
			socket.SysMessage( "You must learn that recipe from a scroll." );
			return;
		}

		MakeItem( socket, pUser, makeID, ourObj.colour );
		AddToLastTen( pUser, makeID );

		if( GetServerSetting( "ToolUseLimit" ))
		{
			bItem.usesLeft -= 1;
			if( bItem.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
			{
				bItem.Delete();
				socket.SysMessage( GetDictionaryEntry( 10202, socket.language )); // You have worn out your tool!
			}
		}
		pUser.StartTimer( gumpDelay, timerID, tailoringID );
	}
}

function UnravelTarget( socket )
{
	socket.CustomTarget( 2, GetDictionaryEntry( 10295, socket.language )); // What item would you like to unravel?
}

// Clothes and leather armor can be unravelled back into cloth and leather
/** @type { ( socket: Socket, ourObj: Character | Item | null ) => void } */
function onCallback2( socket, ourObj )
{
	var mChar = socket.currentChar;

	if( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
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

	var resourceID = 0;

	if( creatorSerial == -1 || entryMadeFrom == 0 || createEntry == null )
	{
		// Not player-crafted; 1 resource if cloth/leather
		if( materialType == "cloth" || materialType == "leather" )
			resourceAmount = 1;
	}
	else
	{
		if( createEntry.avgMinSkill > mChar.skills.tailoring )
		{
			var gumpID = tailoringID + 0xffff;
			socket.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOUNRAVELSKILL" );
			mChar.StartTimer( gumpDelay, 1, tailoringID );
			return;
		}

		// Loop resources used to craft item
		var resourcesUsed = createEntry.resources;
		for( var i = 0; i < resourcesUsed.length; i++ )
		{
			var resource = resourcesUsed[i];
			var amountNeeded = resource[0];
			var colorNeeded  = resource[1]; // unused here but kept for completeness
			var resourceIDs  = resource[2];

			for( var j = 0; j <= resourceIDs.length; j++ )
			{
				if( !isNaN( parseInt( resourceIDs[j] )))
				{
					var resType = TriggerEvent( 2506, "GetResourceType", parseInt( resourceIDs[j] ));
					if( materialType == resType )
					{
						maxResourceAmount = amountNeeded;
						resourceID = resourceIDs[j];
						break;
					}
				}
			}

			if( maxResourceAmount > 0 )
				break;
		}

		if( maxResourceAmount > 1 )
		{
			var healthPercentage = 0;
			if( ourObj.health >= 1 )
				healthPercentage = Math.floor( ( ourObj.health * 100 ) / ourObj.maxhp );

			var usesPercentage = 0;
			if( ourObj.usesLeft >= 1 )
				usesPercentage = Math.floor( ( ourObj.usesLeft * 100 ) / ourObj.maxUses );

			var itemPercentage = usesPercentage > 0 ? Math.min( healthPercentage, usesPercentage ) : healthPercentage;

			resourceAmount = Math.floor( ( maxResourceAmount * itemPercentage ) / 100 );
			resourceAmount = Math.max( Math.floor( resourceAmount / 2 ), 1 );

			var playerSkill = mChar.skills.tailoring;
			resourceAmount = Math.min( Math.max( Math.floor( resourceAmount * ( playerSkill / 1000 )), 1 ), resourceAmount );
		}
		else
		{
			resourceAmount = 1;
		}
	}

	if( resourceAmount == 0 || resourceID == 0 )
	{
		mChar.SetTempTag( "prevActionResult", "CANTUNRAVEL" );
		mChar.StartTimer( repairDelay, 1, tailoringID );
		return;
	}

	// Delete unravelled item
	ourObj.Delete();

	// Generic skill check
	mChar.CheckSkill( tailoringSkillID, 0, mChar.skillCaps.tailoring );

	// Determine returned resource
	var itemToAdd = "";
	switch( materialType )
	{
		case "cloth":   itemToAdd = "0x1766"; break; // cut cloth
		case "leather": itemToAdd = "0x1068"; break; // cut leather
		default: break;
	}

	var newResource = CreateDFNItem( socket, mChar, itemToAdd, resourceAmount, "ITEM", true, resourceColor );

	mChar.SetTempTag( "resourceFromUnravelling", resourceAmount );
	mChar.SetTempTag( "prevActionResult", "UNRAVELSUCCESS" );
	mChar.StartTimer( gumpDelay, 1, tailoringID );
}

function AddToLastTen( pUser, makeID )
{
	var raw  = pUser.GetTempTag( "LastTenTailoring" ) || "";
	var list = raw.split( "," );

	for( var i = 0; i < list.length; i++ )
	{
		if( parseInt( list[i] ) == makeID )
		{
			list.splice( i, 1 );
			break;
		}
	}

	var newList = [ makeID ];
	for( var j = 0; j < list.length && newList.length < 10; j++ )
	{
		var entry = parseInt( list[j] );
		if( !isNaN( entry ) && entry > 0 )
			newList.push( entry );
	}

	pUser.SetTempTag( "LastTenTailoring", newList.join( "," ));
}

function HasLearnedRecipe( pUser, recipeID )
{
	var myData = TriggerEvent( 4022, "ReadRecipeID", pUser );
	if( !myData || myData.length == 0 )
		return false;

	for( var i = 0; i < myData.length; i++ )
	{
		var data = myData[i].split( "," );
		if( data[0] == recipeID )
			return true;
	}
	return false;
}

function eraOK( entry )
{
	if( entry.minEra && coreShardEra < EraStringToNum( entry.minEra ))
		return false;
	if( entry.maxEra && coreShardEra > EraStringToNum( entry.maxEra ))
		return false;
	return true;
}