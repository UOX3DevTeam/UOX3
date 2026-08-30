/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue                 = 0x480;                 // Color of the text.
const tinkeringID             = 4032;                  // Script ID for this tinkering gump
const gumpDelay               = 2000;                  // Delay (ms) before gump reappears after crafting
const itemDetailsScriptID     = 4026;                  // Generic item details gump
const craftGumpID             = 4027;                  // Shared crafting menu frame
const itemsPerPage            = 10;                    // Items per subpage
const displayUnlearnedRecipes = true;                  // For future recipe use
const coreShardEra            = EraStringToNum( GetServerSetting( "CoreShardEra" ));
const tinkeringSkillID        = 37;                    // Index of "tinkering" in ItemDetailGump.skillNames[]

const craftMapRegistryID = 4038;
var TinkeringMap = {};

/** @type { () => boolean } */
function LoadTinkeringMap()
{
	TinkeringMap = {};

	var tinkeringEntries = TriggerEvent( craftMapRegistryID, "CraftMapRegistry", "tinkering" );

	if( !tinkeringEntries )
	{
		Console.Warning( "Tinkering: CraftMapRegistry returned null." );
		return false;
	}

	if( !IsTinkeringArrayValue( tinkeringEntries ))
	{
		Console.Warning( "Tinkering: CraftMapRegistry did not return an array." );
		return false;
	}

	for( var i = 0; i < tinkeringEntries.length; i++ )
	{
		var entry = tinkeringEntries[i];

		if( !entry || typeof entry.makeID == "undefined" )
			continue;

		if( entry.skill === undefined )
			entry.skill = tinkeringSkillID;

		TinkeringMap[entry.makeID] = entry;
	}

	return true;
}

/** @type { ( value: any ) => boolean } */
function IsTinkeringArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

// o--------------------------------------------------------------------------o
// | PageX() - build a page of tinkering items                                |
// o--------------------------------------------------------------------------o
/** @type { ( socket: Socket, pUser: Character, pageNum: number ) => void } */
function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	if( !TinkeringMap || Object.keys( TinkeringMap ).length == 0 )
	{
		if( !LoadTinkeringMap() )
		{
			socket.SysMessage( "Tinkering craft map failed to load." );
			return;
		}
	}

	var pageItems;

	// Last Ten page (if you wire a tab to 999 later)
	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenTinkering" ) || "";
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
		for( var key in TinkeringMap )
		{
			if( !TinkeringMap.hasOwnProperty( key ))
				continue;

			var makeID = parseInt( key );
			var data = TinkeringMap[makeID];
			if( !data || data.page != pageNum )
				continue;

			makeIDs.push( makeID );
		}

		// Sort by dictID so order matches dictionary sequence
		makeIDs.sort( function( a, b )
		{
			var ea = TinkeringMap[a];
			var eb = TinkeringMap[b];
			if( ea && eb )
				return ( ea.dictID || 0 ) - ( eb.dictID || 0 );
			return a - b;
		});

		// Era / recipe filtering
		pageItems = [];
		for( var k = 0; k < makeIDs.length; k++ )
		{
			var id = makeIDs[k];
			var data2 = TinkeringMap[id];
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
			for( var key2 in TinkeringMap )
			{
				if( !TinkeringMap.hasOwnProperty( key2 ))
					continue;

				var mid2 = parseInt( key2 );
				var d3 = TinkeringMap[mid2];
				if( !d3 || d3.page != 1 )
					continue;

				makeIDs.push( mid2 );
			}

			makeIDs.sort( function( a, b )
			{
				var ea2 = TinkeringMap[a];
				var eb2 = TinkeringMap[b];
				if( ea2 && eb2 )
					return ( ea2.dictID || 0 ) - ( eb2.dictID || 0 );
				return a - b;
			});

			pageItems = [];
			for( var m = 0; m < makeIDs.length; m++ )
			{
				var id2 = makeIDs[m];
				var data4 = TinkeringMap[id2];
				if( !data4 )
					continue;

				var needsRecipe2 = data4.recipeID;
				var showAll2 = displayUnlearnedRecipes;

				if( eraOK( data4 ) && ( !needsRecipe2 || showAll2 || HasLearnedRecipe( pUser, needsRecipe2 )) )
					pageItems.push( id2 );
			}
		}
	}

	// Subpage handling (future-proof; currently you effectively have 1 or 2 subpages per category)
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

	var tinkGump = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", tinkGump, socket );
	tinkGump.AddPage( 1 );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index  = j - startIndex;
		var makeID = pageItems[j];
		var entryText;
		var buttonID = makeID; // use makeID directly as buttonID

		var data5 = TinkeringMap[makeID];

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

		// Craft button
		tinkGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
		tinkGump.AddText(   255, 60 + ( index * 20 ), textHue, entryText );

		// Detail button: 20000 + makeID
		tinkGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + buttonID );
	}

	// Prev subpage
	if( subPage > 1 )
	{
		tinkGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		tinkGump.AddHTMLGump( 255, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		tinkGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		tinkGump.AddHTMLGump( 405, 263, 100, 18, false, false,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
	}

	tinkGump.Send( socket );
	tinkGump.Free();
}

/** @type { ( pUser: Character, timerID: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	if( timerID >= 1 && timerID <= 9 )
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

	// Don't continue if player no longer has access to the crafting tool
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
			socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // This has to be in your backpack before you can use it.
		return;
	}

	var gumpID = tinkeringID + 0xffff;

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

	// Page tabs (1–9)
	if( pButton >= 1 && pButton <= 9 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		PageX( socket, pUser, pButton );
		return;
	}

	// Last Ten (if you add a tab that sends 11000)
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
		pUser.SetTempTag( "MakeLast_Tinkering", null );
		pUser.SetTempTag( "CRAFT", null );
		socket.CloseGump( gumpID, 0 );
		return;
	}

	// Make Last
	if( pButton == 5000 )
	{
		var last = pUser.GetTempTag( "MakeLast_Tinkering" );
		if( last )
			pButton = last;
		else
			return;
	}

	var makeID  = 0;
	var timerID = 0;

	// Craft buttons use makeID directly
	if( TinkeringMap[pButton] != undefined )
	{
		makeID = pButton;
		var data = TinkeringMap[makeID];
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

		// Jewelry that needs gem targeting
		if( data.requiresGemTarget )
		{
			pUser.SetTempTag( "makeID", makeID );
			pUser.SetTempTag( "timerID", timerID );
			pUser.AddScriptTrigger( 4033 ); // crafting_complete.js
			socket.CustomTarget( 2, GetDictionaryEntry( 12008, socket.language )); // Select material to use:
			return;
		}

		// Normal craft
		pUser.SetTempTag( "MakeLast_Tinkering", makeID );

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

		pUser.StartTimer( gumpDelay, timerID, tinkeringID );
		return;
	}

	// Detail buttons: 20000 + makeID
	if( pButton >= 20000 && pButton < 30000 )
	{
		var detailMakeID = pButton - 20000;
		var entry = TinkeringMap[detailMakeID];

		if( entry )
		{
			// Which item details to show
			pUser.SetTempTag( "ITEMDETAILS", detailMakeID );

			// Skill used
			pUser.SetTempTag( "Skill", entry.skill || tinkeringSkillID );

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

			// Optional custom names – plugs into your ItemDetail custom harvest name logic
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

/** @type { ( pSock: Socket, targObj: Character | Item | null ) => void } */
function onCallback2( pSock, targObj )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Fetch makeID and timerID from temp tag
	var makeID  = pUser.GetTempTag( "makeID" );
	var timerID = pUser.GetTempTag( "timerID" );
	pUser.SetTempTag( "makeID", null );
	pUser.SetTempTag( "timerID", null );

	var bItem = pSock.tempObj; // tool
	if( !ValidateObject( bItem ))
		return;

	if( ValidateObject( targObj ) && targObj.isItem )
	{
		// Make sure targeted item is in player's backpack
		var iPackOwner = GetPackOwner( targObj, 0 );
		if( ValidateObject( iPackOwner ))
		{
			if( iPackOwner.serial != pUser.serial )
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

		// Jewelry: verify gem
		if( makeID >= 238 && makeID <= 243 )
		{
			var resourceType = TriggerEvent( 2506, "GetResourceType", targObj.id );
			if( resourceType != "gems" )
			{
				pSock.SysMessage( GetDictionaryEntry( 12007, pSock.language )); // That's not a gem resource!
				return;
			}

			// Tags used by crafting_complete.js
			pUser.SetTempTag( "targetedSubResourceId",   targObj.id );
			pUser.SetTempTag( "targetedSubResourceName", targObj.name );
		}

		MakeItem( pSock, pUser, makeID );
		AddToLastTen( pUser, makeID );

		if( GetServerSetting( "ToolUseLimit" ))
		{
			bItem.usesLeft -= 1;
			if( bItem.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
			{
				bItem.Delete();
				pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language )); // You have worn out your tool!
			}
		}
		pUser.StartTimer( gumpDelay, timerID, tinkeringID );
	}
}

function AddToLastTen( pUser, makeID )
{
	var raw  = pUser.GetTempTag( "LastTenTinkering" ) || "";
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

	pUser.SetTempTag( "LastTenTinkering", newList.join( "," ) );
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
