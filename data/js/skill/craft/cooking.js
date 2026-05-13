/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue               = 0x480;                 // Color of the text.
const cookingID             = 4034;                  // Script ID for this cooking gump
const gumpDelay             = 2000;                  // Delay (ms) before gump reappears after crafting
const itemDetailsScriptID   = 4026;                  // Generic item details gump
const craftGumpID           = 4027;                  // Shared crafting menu frame
const itemsPerPage          = 10;                    // Items per subpage
const displayUnlearnedRecipes = true;                // For future recipe use
const coreShardEra          = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const cookingSkillID        = 13;                    // Index for "cooking" in ItemDetailGump skillNames[]
const manualMillTarget      = true;                  // If true, player must target mills when grinding wheat

// Mills / ovens / heat sources for AreaItemFunction + StaticInRange checks
const mills = [
	0x188b, 0x1893, 0x1920, 0x1922, 0x192c, 0x192e
];

const ovens = [
	0x0461, 0x046f, 0x092b, 0x093f
];

const heatSources = [
	0x0461, 0x0480, 0x092B, 0x0933, 0x0937, 0x0942, 0x0945, 0x0950, 0x0953,
	0x095e, 0x0961, 0x096c, 0x0de3, 0x0de8, 0x0fac
];

function FindNearbyMills( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( mills.indexOf( trgItem.id ) != -1 );
}

function FindNearbyOvens( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( ovens.indexOf( trgItem.id ) != -1 );
}

function FindNearbyHeatSources( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( heatSources.indexOf( trgItem.id ) != -1 );
}

const craftMapRegistryID = 4038;
var CookingMap = {};

/** @type { () => boolean } */
function LoadCookingMap()
{
	CookingMap = {};

	var cookingEntries = TriggerEvent( craftMapRegistryID, "CraftMapRegistry", "cooking" );

	if( !cookingEntries || !IsCookingArrayValue( cookingEntries ) )
	{
		Console.Warning( "Cooking: Unable to load cooking craft map data." );
		return false;
	}

	for( var i = 0; i < cookingEntries.length; i++ )
	{
		var entry = cookingEntries[i];

		if( !entry || typeof entry.makeID == "undefined" )
			continue;

		if( entry.skill === undefined )
			entry.skill = cookingSkillID;

		CookingMap[entry.makeID] = entry;
	}

	Console.Print( "Cooking: Loaded " + cookingEntries.length + " craft map entries.\n" );
	return true;
}

/** @type { ( value: any ) => boolean } */
function IsCookingArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( socket: Socket, pUser: Character, pageNum: number ) => void } */
function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	if( !CookingMap || Object.keys( CookingMap ).length == 0 )
	{
		if( !LoadCookingMap() )
		{
			socket.SysMessage( "Cooking craft map failed to load." );
			return;
		}
	}

	var pageItems;

	// No "Last Ten" page here (but we keep the infrastructure if you want it later)
	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenCooking" ) || "";
		var split = lastTenRaw.split( "," );
		pageItems = [];

		for( var i = 0; i < split.length; i++ )
		{
			var val = parseInt( split[i] );
			if( !isNaN( val ))
				pageItems.push( val );
		}
	}
	else
	{
		// Collect all makeIDs for this page
		var makeIDs = [];
		for( var key in CookingMap )
		{
			if( !CookingMap.hasOwnProperty( key ))
				continue;

			var makeID = parseInt( key );
			var data = CookingMap[makeID];
			if( !data || data.page != pageNum )
				continue;

			makeIDs.push( makeID );
		}

		// Sort by dictID so order matches dictionary sequence
		makeIDs.sort( function( a, b )
		{
			var ea = CookingMap[a];
			var eb = CookingMap[b];
			if( ea && eb )
				return ( ea.dictID || 0 ) - ( eb.dictID || 0 );
			return a - b;
		});

		// Era / recipe filtering (hooks for future use)
		pageItems = [];
		for( var k = 0; k < makeIDs.length; k++ )
		{
			var id = makeIDs[k];
			var data2 = CookingMap[id];
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
			for( var key2 in CookingMap )
			{
				if( !CookingMap.hasOwnProperty( key2 ))
					continue;

				var mid2 = parseInt( key2 );
				var d3 = CookingMap[mid2];
				if( !d3 || d3.page != 1 )
					continue;

				makeIDs.push( mid2 );
			}

			makeIDs.sort( function( a, b )
			{
				var ea2 = CookingMap[a];
				var eb2 = CookingMap[b];
				if( ea2 && eb2 )
					return ( ea2.dictID || 0 ) - ( eb2.dictID || 0 );
				return a - b;
			});

			pageItems = [];
			for( var m = 0; m < makeIDs.length; m++ )
			{
				var id2 = makeIDs[m];
				var data4 = CookingMap[id2];
				if( !data4 )
					continue;

				var needsRecipe2 = data4.recipeID;
				var showAll2 = displayUnlearnedRecipes;

				if( eraOK( data4 ) && ( !needsRecipe2 || showAll2 || HasLearnedRecipe( pUser, needsRecipe2 )) )
					pageItems.push( id2 );
			}
		}
	}

	// Subpage handling (future-proof; currently only 1 subpage per page)
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

	var cookGump = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", cookGump, socket );
	cookGump.AddPage( 1 );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index  = j - startIndex;
		var makeID = pageItems[j];
		var entryText;
		var buttonID = makeID; // use makeID directly as buttonID

		var data5 = CookingMap[makeID];

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
		cookGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
		cookGump.AddText(   255, 60 + ( index * 20 ), textHue, entryText );

		// Detail button: 20000 + makeID
		cookGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + buttonID );
	}

	// Prev subpage
	if( subPage > 1 )
	{
		cookGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		cookGump.AddHTMLGump( 255, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		cookGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		cookGump.AddHTMLGump( 405, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
	}

	cookGump.Send( socket );
	cookGump.Free();
}

/** @type { ( pUser: Character, timerID: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	if( timerID >= 1 && timerID <= 4 )
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

	// Don't continue if player no longer has access to the cooking tool
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
			socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That is in someone else's backpack!
			return;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // This must be in your pack to use.
		return;
	}

	var gumpID = cookingID + 0xffff;

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

	// Page tabs (1–4: Ingredients, Preparation, Baking, Barbecue)
	if( pButton >= 1 && pButton <= 4 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		PageX( socket, pUser, pButton );
		return;
	}

	// Last Ten (if you ever wire it into the main craft gump)
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
		pUser.SetTempTag( "MakeLast_Cooking", null );
		pUser.SetTempTag( "CRAFT", null );
		socket.CloseGump( gumpID, 0 );
		return;
	}

	// Make Last
	if( pButton == 5000 )
	{
		var last = pUser.GetTempTag( "MakeLast_Cooking" );
		if( last )
			pButton = last;
		else
			return;
	}

	var makeID  = 0;
	var timerID = 0;

	// Craft buttons use makeID directly
	if( CookingMap[pButton] != undefined )
	{
		makeID = pButton;
		var data = CookingMap[makeID];
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

		pUser.SetTempTag( "MakeLast_Cooking", makeID );

		// Environment checks
		var makeItem = false;

		if( makeID == 1500 )
		{
			// Sack of Flour – wheat grinding
			if( manualMillTarget )
			{
				// Require player to manually target mill
				var pack = pUser.pack;
				var packItem;
				for( packItem = pack.FirstItem(); !pack.FinishedItems(); packItem = pack.NextItem() )
				{
					if( ValidateObject( packItem ) && packItem.id == 0x1ebd )
					{
						// Found wheat; call its onUseChecked
						TriggerEvent( 101, "onUseChecked", pUser, packItem ); // 101 = wheat.js
						return;
					}
				}

				// No wheat found – just fail silently and let DFN/msg handle it
				return;
			}
			else
			{
				// Auto mill lookup (dynamic first, then static)
				var nearbyMill = AreaItemFunction( "FindNearbyMills", pUser, 2, socket );
				if( nearbyMill == 0 )
				{
					var staticFound = false;
					for( var i = 0; i < mills.length; i++ )
					{
						if( StaticInRange( pUser.x, pUser.y, pUser.worldnumber, 2, mills[i] ))
						{
							staticFound = true;
							break;
						}
					}

					if( staticFound )
						nearbyMill = 1;
				}

				if( nearbyMill > 0 )
				{
					makeItem = true;
				}
				else
				{
					pUser.SetTempTag( "prevActionResult", "NOMILL" );
				}
			}
		}
		else if( makeID >= 1600 && makeID <= 1611 )
		{
			// Baking – requires oven
			var nearbyOven = AreaItemFunction( "FindNearbyOvens", pUser, 2, socket );
			if( nearbyOven > 0 )
			{
				makeItem = true;
			}
			else
			{
				var staticFoundOven = false;
				for( var o = 0; o < ovens.length; o++ )
				{
					if( StaticInRange( pUser.x, pUser.y, pUser.worldnumber, 2, ovens[o] ))
					{
						staticFoundOven = true;
						break;
					}
				}
				if( !staticFoundOven )
					pUser.SetTempTag( "prevActionResult", "NOOVEN" );
				else
					makeItem = true;
			}
		}
		else if( makeID >= 1650 && makeID <= 1655 )
		{
			// Barbecue – requires generic heat source
			var nearbyHeat = AreaItemFunction( "FindNearbyHeatSources", pUser, 2, socket );
			if( nearbyHeat > 0 )
			{
				makeItem = true;
			}
			else
			{
				var staticFoundHeat = false;
				for( var h = 0; h < heatSources.length; h++ )
				{
					if( StaticInRange( pUser.x, pUser.y, pUser.worldnumber, 2, heatSources[h] ))
					{
						staticFoundHeat = true;
						break;
					}
				}
				if( !staticFoundHeat )
					pUser.SetTempTag( "prevActionResult", "NOHEATSOURCE" );
				else
					makeItem = true;
			}
		}
		else if( ( makeID >= 1501 && makeID <= 1557 ) )
		{
			// Ingredients & preparations – no heat source or mill required
			makeItem = true;
		}

		if( makeItem )
		{
			MakeItem( socket, pUser, makeID );
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
		}

		pUser.StartTimer( gumpDelay, timerID, cookingID );
		return;
	}

	// Detail buttons: 20000 + makeID
	if( pButton >= 20000 && pButton < 30000 )
	{
		var detailMakeID = pButton - 20000;
		var entry = CookingMap[detailMakeID];

		if( entry )
		{
			// Which item details to show
			pUser.SetTempTag( "ITEMDETAILS", detailMakeID );

			// Skill used
			pUser.SetTempTag( "Skill", entry.skill || cookingSkillID );

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

			// Optional harvest dictIDs
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

			// Optional custom names – plug into your new ItemDetail custom text logic
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
	var raw  = pUser.GetTempTag( "LastTenCooking" ) || "";
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

	pUser.SetTempTag( "LastTenCooking", newList.join( "," ) );
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