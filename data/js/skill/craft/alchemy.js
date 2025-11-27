/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue               = 0x480;                 // Color of the text.
const alchemyID             = 4028;                  // Script ID for this alchemy gump
const gumpDelay             = 2000;                  // Delay (ms) before gump reappears after crafting
const itemDetailsScriptID   = 4026;                  // Generic item details gump
const craftGumpID           = 4027;                  // Shared crafting menu frame
const itemsPerPage          = 10;                    // Items per subpage
const displayUnlearnedRecipes = true;                // For future recipe use
const coreShardEra          = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const alchemySkillID        = 0;                     // Skill ID: Alchemy

// o--------------------------------------------------------------------------o
// | AlchemyMap                                                               |
// o--------------------------------------------------------------------------o
// | Keyed by makeID (create entry ID).                                      |
// | Each entry:                                                             |
// |   dictID  - dictionary entry for row text                               |
// |   page    - main category page (1..4)                                   |
// |   timerID - which page timer should reopen                              |
// |   skill   - skill used (default: alchemySkillID)                        |
// |   recipeID?, minEra?, maxEra?, harvest?[] harvestNames? ["cloth" ]      |
// |   can be added later										             |
// o--------------------------------------------------------------------------o
// If you want to add multi-reagent potions later 
// (e.g. harvest: [ garlic_dictNumber, ginseng_dictNumber ] or harvestNames: [ "garlic, "ginseng" ]), 
// the same pattern will just work with no script changes.

const AlchemyMap = {
	// Page 1 - Curative / Refresh
	298: { dictID: 10910, page: 1, timerID: 1, harvest: [ 10022 ] }, // Lesser Heal
	299: { dictID: 10911, page: 1, timerID: 1, harvest: [ 10022 ] }, // Heal
	300: { dictID: 10912, page: 1, timerID: 1, harvest: [ 10022 ] }, // Greater Heal
	305: { dictID: 10908, page: 1, timerID: 1, harvest: [ 10025 ] }, // Refresh
	306: { dictID: 10909, page: 1, timerID: 1, harvest: [ 10025 ] }, // Total Refresh
	292: { dictID: 10913, page: 1, timerID: 1, harvest: [ 10020 ] }, // Lesser Cure
	293: { dictID: 10914, page: 1, timerID: 1, harvest: [ 10020 ] }, // Cure
	294: { dictID: 10915, page: 1, timerID: 1, harvest: [ 10020 ] }, // Greater Cure

	// Page 2 - Enhancement
	290: { dictID: 10916, page: 2, timerID: 2, harvest: [ 10019 ] }, // Agility
	291: { dictID: 10917, page: 2, timerID: 2, harvest: [ 10019 ] }, // Greater Agility
	295: { dictID: 10918, page: 2, timerID: 2, harvest: [ 10021 ] }, // Strength
	296: { dictID: 10919, page: 2, timerID: 2, harvest: [ 10021 ] }, // Greater Strength
	297: { dictID: 10920, page: 2, timerID: 2, harvest: [ 10021 ] }, // Night Sight

	// Page 3 - Poison
	301: { dictID: 10921, page: 3, timerID: 3, harvest: [ 10024 ] }, // Lesser Poison
	302: { dictID: 10922, page: 3, timerID: 3, harvest: [ 10024 ] }, // Poison
	303: { dictID: 10923, page: 3, timerID: 3, harvest: [ 10024 ] }, // Greater Poison
	304: { dictID: 10924, page: 3, timerID: 3, harvest: [ 10024 ] }, // Deadly Poison

	// Page 4 - Explosive
	307: { dictID: 10925, page: 4, timerID: 4, harvest: [ 10026 ] }, // Explosion
	308: { dictID: 10926, page: 4, timerID: 4, harvest: [ 10026 ] }, // Greater Explosion
	309: { dictID: 10927, page: 4, timerID: 4, harvest: [ 10023 ] }  // Conflagration (or w/e 10927 is)
};

// Fill in defaults (skill, etc)
(function initAlchemyMap()
{
	for( var key in AlchemyMap )
	{
		if( !AlchemyMap.hasOwnProperty( key ))
			continue;

		var entry = AlchemyMap[key];

		if( entry.skill === undefined )
			entry.skill = alchemySkillID;

		// If you ever want to drive item detail resources here:
		//   entry.harvest = [ <dictID for bottles>, <dictID for reagents>, ... ];
		// For now, the detail gump will infer resources directly from DFN create entries.
	}
})();

// o--------------------------------------------------------------------------o
// | PageX() - build a page of alchemy items                                  |
// o--------------------------------------------------------------------------o
/** @type { ( socket: Socket, pUser: Character, pageNum: number ) => void } */
function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	var pageItems;

	// Special Last Ten page
	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenAlchemy" ) || "";
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
		for( var key in AlchemyMap )
		{
			if( !AlchemyMap.hasOwnProperty( key ))
				continue;

			var makeID = parseInt( key );
			var data = AlchemyMap[makeID];
			if( !data || data.page != pageNum )
				continue;

			makeIDs.push( makeID );
		}

		// Sort by dictID so order matches dictionary sequence
		makeIDs.sort( function( a, b )
		{
			var ea = AlchemyMap[a];
			var eb = AlchemyMap[b];
			if( ea && eb )
				return ( ea.dictID || 0 ) - ( eb.dictID || 0 );
			return a - b;
		});

		// Era / recipe filtering (no recipes yet, but keep hook)
		pageItems = [];
		for( var k = 0; k < makeIDs.length; k++ )
		{
			var id = makeIDs[k];
			var data2 = AlchemyMap[id];
			if( !data2 )
				continue;

			var needsRecipe = data2.recipeID;
			var showAll = displayUnlearnedRecipes;

			if( eraOK( data2 ) && ( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe )))
				pageItems.push( id );
		}

		// Fallback: if no items on this page and it's not page 1, go to page 1
		if( pageItems.length == 0 && pageNum != 1 )
		{
			pageNum = 1;

			makeIDs = [];
			for( var key2 in AlchemyMap )
			{
				if( !AlchemyMap.hasOwnProperty( key2 ))
					continue;

				var mid2 = parseInt( key2 );
				var d3 = AlchemyMap[mid2];
				if( !d3 || d3.page != 1 )
					continue;

				makeIDs.push( mid2 );
			}

			makeIDs.sort( function( a, b )
			{
				var ea2 = AlchemyMap[a];
				var eb2 = AlchemyMap[b];
				if( ea2 && eb2 )
					return ( ea2.dictID || 0 ) - ( eb2.dictID || 0 );
				return a - b;
			});

			pageItems = [];
			for( var m = 0; m < makeIDs.length; m++ )
			{
				var id2 = makeIDs[m];
				var data4 = AlchemyMap[id2];
				if( !data4 )
					continue;

				var needsRecipe2 = data4.recipeID;
				var showAll2 = displayUnlearnedRecipes;

				if( eraOK( data4 ) && ( !needsRecipe2 || showAll2 || HasLearnedRecipe( pUser, needsRecipe2 )))
					pageItems.push( id2 );
			}
		}
	}

	// Subpage handling
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

	var alchGump = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", alchGump, socket );
	alchGump.AddPage( 1 );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index  = j - startIndex;
		var makeID = pageItems[j];
		var entryText;
		var buttonID = makeID; // use makeID directly as buttonID

		var data5 = AlchemyMap[makeID];

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
		alchGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
		alchGump.AddText(   255, 60 + ( index * 20 ), textHue, entryText );

		// Detail button: 20000 + makeID (same pattern as glassblowing)
		alchGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + buttonID );
	}

	// Prev subpage
	if( subPage > 1 )
	{
		alchGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		alchGump.AddHTMLGump( 255, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		alchGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		alchGump.AddHTMLGump( 405, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
	}

	alchGump.Send( socket );
	alchGump.Free();
}

// o--------------------------------------------------------------------------o
// | onTimer - reopen last page after crafting                                |
// o--------------------------------------------------------------------------o
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

// o--------------------------------------------------------------------------o
// | onGumpPress - navigation, Make Last, craft & details                     |
// o--------------------------------------------------------------------------o
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
		socket.SysMessage( GetDictionaryEntry( 6031, socket.language )); // That is locked down and cannot be used.
		return;
	}

	var packOwner = GetPackOwner( tool, 0 );
	if( ValidateObject( packOwner ))
	{
		if( packOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That is not in your pack.
			return;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // You must have that item in your pack to use it.
		return;
	}

	var gumpID = alchemyID + 0xffff;

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

	// Page tabs (Alchemy has 4 categories, but using <=8 is harmless)
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
		pUser.SetTempTag( "MakeLast_Alchemy", null );
		pUser.SetTempTag( "CRAFT", null );
		socket.CloseGump( gumpID, 0 );
		return;
	}

	var makeID  = 0;
	var timerID = 0;

	// Make Last
	if( pButton == 5000 )
	{
		var last = pUser.GetTempTag( "MakeLast_Alchemy" );
		if( last )
			pButton = last;
		else
			return;
	}

	// Craft buttons use makeID directly
	if( AlchemyMap[pButton] != undefined )
	{
		makeID = pButton;
		var data = AlchemyMap[makeID];
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

		pUser.SetTempTag( "MakeLast_Alchemy", makeID );

		MakeItem( socket, pUser, makeID );
		AddToLastTen( pUser, makeID );

		if( GetServerSetting( "ToolUseLimit" ))
		{
			tool.usesLeft -= 1;
			if( tool.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
			{
				tool.Delete();
				socket.SysMessage( GetDictionaryEntry( 10202, socket.language )); // Your tool wears out.
			}
		}

		pUser.StartTimer( gumpDelay, timerID, alchemyID );
		return;
	}

	// Detail buttons: 20000 + makeID
	if( pButton >= 20000 && pButton < 30000 )
	{
		var detailMakeID = pButton - 20000;
		var entry = AlchemyMap[detailMakeID];

		if( entry )
		{
			// Which item details to show
			pUser.SetTempTag( "ITEMDETAILS", detailMakeID );

			// Skill used
			pUser.SetTempTag( "Skill", entry.skill || alchemySkillID );

			// Clear old harvest tags to avoid cross-contamination
			pUser.SetTempTag( "Harvest", null );
			pUser.SetTempTag( "Harvest2", null );
			pUser.SetTempTag( "Harvest3", null );
			pUser.SetTempTag( "Harvest4", null );

			// Clear old harvest names
			pUser.SetTempTag( "HarvestName",  null );
			pUser.SetTempTag( "Harvest2Name", null );
			pUser.SetTempTag( "Harvest3Name", null );
			pUser.SetTempTag( "Harvest4Name", null );

			// If you later add entry.harvest = [dictID1, dictID2,...], you can push them here
			if( entry.harvest && entry.harvest.length > 0 )
			{
				if( entry.harvest.length >= 1 )
					pUser.SetTempTag( "Harvest", entry.harvest[0] );
				if( entry.harvest.length >= 2 )
					pUser.SetTempTag( "Harvest2", entry.harvest[1] );
				if( entry.harvest.length >= 3 )
					pUser.SetTempTag( "Harvest3", entry.harvest[2] );
				if( entry.harvest.length >= 4 )
					pUser.SetTempTag( "Harvest4", entry.harvest[3] );
			}

			// OPTIONAL custom names – these override the dictionary string
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

// o--------------------------------------------------------------------------o
// | Last Ten handling                                                        |
// o--------------------------------------------------------------------------o
function AddToLastTen( pUser, makeID )
{
	var raw  = pUser.GetTempTag( "LastTenAlchemy" ) || "";
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

	pUser.SetTempTag( "LastTenAlchemy", newList.join( "," ) );
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