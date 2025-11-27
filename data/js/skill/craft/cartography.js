/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue               = 0x480;                 // Color of the text.
const cartographyID         = 4035;                  // Script ID for this cartography gump
const gumpDelay             = 2000;                  // Delay (ms) before gump reappears after crafting
const itemDetailsScriptID   = 4026;                  // Generic item details gump
const craftGumpID           = 4027;                  // Shared crafting menu frame
const itemsPerPage          = 10;                    // Items per subpage
const displayUnlearnedRecipes = true;                // For future recipe use
const coreShardEra          = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const cartographySkillID    = 12;                    // Skill index for "cartography" in ItemDetailGump

//////////////////////////////////////////////////////////////////////////////////////////
// CartographyMap
// Keyed by makeID (create entry ID).
// Each entry:
//   dictID      - dictionary entry for row text (13100..13103)
//   page        - main category page (1..N)
//   timerID     - which page timer should reopen
//   skill       - skill used (default: cartographySkillID)
//   recipeID?   - optional recipe ID
//   minEra/maxEra? - optional era gating
//   harvest?[]  - optional material dictionary IDs
//   harvestNames?[] - optional custom material names
//
// NOTE: For the World Map row, we use base makeID 2003.
//       At craft time, we still adjust to 2003..2007 depending on worldnumber,
//       just like the original script did.
//
//////////////////////////////////////////////////////////////////////////////////////////

const CartographyMap = {
	// Page 1 - Maps
	2000: { dictID: 13100, page: 1, timerID: 1 }, // Local Map
	2001: { dictID: 13101, page: 1, timerID: 1 }, // City Map
	2002: { dictID: 13102, page: 1, timerID: 1 }, // Sea Chart
	2003: { dictID: 13103, page: 1, timerID: 1 }  // World Map (base; world-specific in onGumpPress)
};

// Fill in defaults (skill, etc)
(function initCartographyMap()
{
	for( var key in CartographyMap )
	{
		if( !CartographyMap.hasOwnProperty( key ))
			continue;

		var entry = CartographyMap[key];

		if( entry.skill === undefined )
			entry.skill = cartographySkillID;

		// If you later want harvest info for detail gump, you can do:
		// CartographyMap[2000].harvest = [ <dict-for-blank-scroll> ];
		// CartographyMap[2000].harvestNames = [ "Blank scroll" ];
	}
})();

/** @type { ( socket: Socket, pUser: Character, pageNum: number ) => void } */
function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	var pageItems;

	// Special Last Ten page (if you ever want it for Cartography)
	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenCartography" ) || "";
		var split = lastTenRaw.split( "," );
		pageItems = [];

		for( var i = 0; i < split.length; i++ )
		{
			var val = parseInt( split[i] );
			if( !isNaN( val ))
				pageItems.push( val ); // makeID itself
		}
	}
	else
	{
		// Collect all makeIDs for this page
		var makeIDs = [];
		for( var key in CartographyMap )
		{
			if( !CartographyMap.hasOwnProperty( key ))
				continue;

			var makeID = parseInt( key );
			var data = CartographyMap[makeID];
			if( !data || data.page != pageNum )
				continue;

			makeIDs.push( makeID );
		}

		// Sort by dictID so order matches dictionary sequence
		makeIDs.sort( function( a, b )
		{
			var ea = CartographyMap[a];
			var eb = CartographyMap[b];
			if( ea && eb )
				return ( ea.dictID || 0 ) - ( eb.dictID || 0 );
			return a - b;
		});

		// Era / recipe filtering (hooks for future use)
		pageItems = [];
		for( var k = 0; k < makeIDs.length; k++ )
		{
			var id = makeIDs[k];
			var data2 = CartographyMap[id];
			if( !data2 )
				continue;

			var needsRecipe = data2.recipeID;
			var showAll = displayUnlearnedRecipes;

			if( eraOK( data2 ) && ( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe )) )
				pageItems.push( id );
		}

		// Fallback: if no items on this page and it's not page 1, go to page 1
		if( pageItems.length == 0 && pageNum != 1 )
		{
			pageNum = 1;

			makeIDs = [];
			for( var key2 in CartographyMap )
			{
				if( !CartographyMap.hasOwnProperty( key2 ))
					continue;

				var mid2 = parseInt( key2 );
				var d3 = CartographyMap[mid2];
				if( !d3 || d3.page != 1 )
					continue;

				makeIDs.push( mid2 );
			}

			makeIDs.sort( function( a, b )
			{
				var ea2 = CartographyMap[a];
				var eb2 = CartographyMap[b];
				if( ea2 && eb2 )
					return ( ea2.dictID || 0 ) - ( eb2.dictID || 0 );
				return a - b;
			});

			pageItems = [];
			for( var m = 0; m < makeIDs.length; m++ )
			{
				var id2 = makeIDs[m];
				var data4 = CartographyMap[id2];
				if( !data4 )
					continue;

				var needsRecipe2 = data4.recipeID;
				var showAll2 = displayUnlearnedRecipes;

				if( eraOK( data4 ) && ( !needsRecipe2 || showAll2 || HasLearnedRecipe( pUser, needsRecipe2 )) )
					pageItems.push( id2 );
			}
		}
	}

	// Subpage handling (future-proof; only 1 subpage needed right now)
	var subPage = pUser.GetTempTag( "subPage" );
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

	var cartGump = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", cartGump, socket );
	cartGump.AddPage( 1 );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index  = j - startIndex;
		var makeID = pageItems[j];
		var entryText;
		var buttonID = makeID; // use makeID directly as buttonID

		var data5 = CartographyMap[makeID];

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

		// Craft button uses makeID
		cartGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
		cartGump.AddText(   255, 60 + ( index * 20 ), textHue, entryText );

		// Detail button: 20000 + makeID
		cartGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + buttonID );
	}

	// Prev subpage
	if( subPage > 1 )
	{
		cartGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		cartGump.AddHTMLGump( 255, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		cartGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		cartGump.AddHTMLGump( 405, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
	}

	cartGump.Send( socket );
	cartGump.Free();
}

/** @type { ( pUser: Character, timerID: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	if( timerID >= 1 && timerID <= 8 )
	{
		PageX( pSocket, pUser, timerID );
	}
	else if( timerID == 999 )
	{
		PageX( pSocket, pUser, 999 );
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

	var tool = socket.tempObj;
	if( !ValidateObject( tool ) || !pUser.InRange( tool, 3 ))
	{
		socket.SysMessage( GetDictionaryEntry( 461, socket.language )); // You are too far away.
		return;
	}

	if( tool.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 6031, socket.language )); // Locked down resources cannot be used!
		return;
	}

	var packOwner = GetPackOwner( tool, 0 );
	if( ValidateObject( packOwner ))
	{
		if( packOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That is in someone else's pack.
			return;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // Must be in your pack to use it.
		return;
	}

	var gumpID = cartographyID + 0xffff;

	// Subpage back / forward
	if( pButton >= 8001 && pButton < 9000 )
	{
		var subPage = pButton - 8000;
		var pageNum = pUser.GetTempTag( "page" );
		pUser.SetTempTag( "subPage", subPage );
		PageX( socket, pUser, pageNum );
		return;
	}

	if( pButton >= 9001 && pButton < 10000 )
	{
		var subPage2 = pButton - 9000;
		var pageNum2 = pUser.GetTempTag( "page" );
		pUser.SetTempTag( "subPage", subPage2 );
		PageX( socket, pUser, pageNum2 );
		return;
	}

	// Page tabs (Cartography only uses page 1 for now, but up to 8 is harmless)
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
		pUser.SetTempTag( "MakeLast_Cartography", null );
		pUser.SetTempTag( "CRAFT", null );
		socket.CloseGump( gumpID, 0 );
		return;
	}

	var makeID  = 0;
	var timerID = 0;

	// Make Last
	if( pButton == 5000 )
	{
		var last = pUser.GetTempTag( "MakeLast_Cartography" );
		if( last )
			pButton = last;
		else
			return;
	}

	// Craft buttons use makeID directly
	if( CartographyMap[pButton] != undefined )
	{
		makeID = pButton;
		var data = CartographyMap[makeID];
		timerID = data.timerID || 1;

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

		// Save Make Last
		pUser.SetTempTag( "MakeLast_Cartography", makeID );

		// Special case: World Map uses different create entries per world
		var makeToCraft = makeID;
		if( makeID == 2003 ) // world map row
		{
			switch( pUser.worldnumber )
			{
				case 0: // Felucca
				case 1: // Trammel
					makeToCraft = 2003; break;
				case 2: // Ilshenar
					makeToCraft = 2004; break;
				case 3: // Malas
					makeToCraft = 2005; break;
				case 4: // Tokuno
					makeToCraft = 2006; break;
				case 5: // TerMur
					makeToCraft = 2007; break;
			}
		}

		// Let crafting_complete.js handle map-specific setup
		pUser.AddScriptTrigger( 4033 );

		MakeItem( socket, pUser, makeToCraft );
		AddToLastTen( pUser, makeID );

		if( GetServerSetting( "ToolUseLimit" ))
		{
			tool.usesLeft -= 1;
			if( tool.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
			{
				tool.Delete();
				socket.SysMessage( GetDictionaryEntry( 10202, socket.language )); // You have worn out your tool!
			}
		}

		pUser.StartTimer( gumpDelay, timerID, cartographyID );
		return;
	}

	// Detail buttons: 20000 + makeID
	if( pButton >= 20000 && pButton < 30000 )
	{
		var detailMakeID = pButton - 20000;
		var entry = CartographyMap[detailMakeID];

		if( entry )
		{
			// Which item details to show
			pUser.SetTempTag( "ITEMDETAILS", detailMakeID );

			// Skill used
			pUser.SetTempTag( "Skill", entry.skill || cartographySkillID );

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

			// If you later add entry.harvest = [ ... ], push them into tags
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

			// OPTIONAL custom names – override / supplement dictionary labels
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

// Last Ten handling
function AddToLastTen( pUser, makeID )
{
	var raw  = pUser.GetTempTag( "LastTenCartography" ) || "";
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

	pUser.SetTempTag( "LastTenCartography", newList.join( "," ) );
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