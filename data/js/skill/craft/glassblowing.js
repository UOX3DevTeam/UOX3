/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue = 0x480;                 // Color hue for all text in the crafting gump
const scriptID = 4036;                 // Script ID used to identify and close this gump
const gumpDelay = 2000;                // Delay (ms) before gump reappears after crafting
const itemDetailsScriptID = 4026;      // Script ID used to show item detail tooltips
const craftGumpID = 4027;              // TriggerEvent ID used to build the crafting gump UI
const itemsPerPage = 10;               // Number of craftable items shown per gump subpage
const displayUnlearnedRecipes = true;  // Show recipes player has not learned
const coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ) );

//////////////////////////////////////////////////////////////////////////////////////////
// Glassblowing CraftingMap
//////////////////////////////////////////////////////////////////////////////////////////

const CraftingMap = {
    // Page 1 - Misc Glassware
    3000: { dictID: 13600, page: 1, timerID: 1 },                // empty bottle
    3001: { dictID: 13601, page: 1, timerID: 1 },                // flask (small)
    3002: { dictID: 13602, page: 1, timerID: 1 },                // flask (medium)
    3003: { dictID: 13603, page: 1, timerID: 1 },                // flask (curved)
    3004: { dictID: 13604, page: 1, timerID: 1 },                // flask (large #1)
    3005: { dictID: 13605, page: 1, timerID: 1 },                // flask (large #2)
    3006: { dictID: 13606, page: 1, timerID: 1 },                // flask (bubbling blue)
    3007: { dictID: 13607, page: 1, timerID: 1 },                // flask (bubbling purple)
    3008: { dictID: 13608, page: 1, timerID: 1 },                // flask (bubbling red)
    3009: { dictID: 13609, page: 1, timerID: 1 },                // empty vials
    3010: { dictID: 13610, page: 1, timerID: 1 },                // full vials
    3011: { dictID: 13611, page: 1, timerID: 1 },                // spinning hourglass
	3012: { customName: "hollow prism", page: 1, timerID: 1, minEra: "ml" },    // hollow prism
	3013: { customName: "gargoyle floor mirror", page: 1, timerID: 1, minEra: "sa" },    // Gargoyle Floor Mirror
	3014: { customName: "gargoyle wall mirror", page: 1, timerID: 1, minEra: "sa" },    // Gargoyle Wall Mirror
	3015: { customName: "empty venom vial", page: 1, timerID: 1, minEra: "sa" },    // Empty Venom Vial
	3016: { customName: "empty oil flask", page: 1, timerID: 1, minEra: "sa" },    // Empty Oil Flask
	3017: { customName: "workable glass", page: 1, timerID: 1, minEra: "sa" },    // Workable Glass
	// Page 2 Glass Weapons
	3018: { customName: "glass sword", page: 2, timerID: 2, minEra: "sa" },    // Glass sword
	3019: { customName: "glass staff", page: 2, timerID: 2, minEra: "sa" }    // Glass staff

};

function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	/** @type {number[]} */
	var pageItems;

	// Special "Last Ten" page uses stored makeIDs directly
	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTag( "LastTenGlassblowing" );
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
		// Build list of makeIDs for this page from CraftingMap
		var makeIDs = [];
		for( var makeIDStr in CraftingMap )
		{
			if( !CraftingMap.hasOwnProperty( makeIDStr ))
				continue;

			var makeID = parseInt( makeIDStr );
			var data = CraftingMap[makeID];
			if( !data || data.page != pageNum )
				continue;

			makeIDs.push( makeID );
		}

		// Sort numeric so order is stable: 3000,3001,...,3012
		makeIDs.sort(function(a, b){ return a - b; });

		// Filter by era / recipe in that order
		pageItems = [];
		for( var k = 0; k < makeIDs.length; k++ )
		{
			var id = makeIDs[k];
			var data2 = CraftingMap[id];
			if( !data2 )
				continue;

			var needsRecipe = data2.recipeID;
			var showAll = displayUnlearnedRecipes;

			if( eraOK( data2 ) && ( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe )))
			{
				pageItems.push( id );
			}
		}

		// Fallback if page empty and not page 1
		if( pageItems.length == 0 && pageNum != 1 )
		{
			pageNum = 1;

			makeIDs = [];
			for( var makeIDStr2 in CraftingMap )
			{
				if( !CraftingMap.hasOwnProperty( makeIDStr2 ))
					continue;

				var makeID2 = parseInt( makeIDStr2 );
				var data3 = CraftingMap[makeID2];
				if( !data3 || data3.page != 1 )
					continue;

				makeIDs.push( makeID2 );
			}
			makeIDs.sort(function(a, b){ return a - b; });

			pageItems = [];
			for( var m = 0; m < makeIDs.length; m++ )
			{
				var id2 = makeIDs[m];
				var data4 = CraftingMap[id2];
				if( !data4 )
					continue;

				var needsRecipe2 = data4.recipeID;
				var showAll2 = displayUnlearnedRecipes;

				if( eraOK( data4 ) && ( !needsRecipe2 || showAll2 || HasLearnedRecipe( pUser, needsRecipe2 )))
				{
					pageItems.push( id2 );
				}
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
	var endIndex = Math.min( startIndex + itemsPerPage, pageItems.length );

	if( startIndex >= pageItems.length )
	{
		subPage = 1;
		startIndex = 0;
		endIndex = Math.min( itemsPerPage, pageItems.length );
		pUser.SetTempTag( "subPage", subPage );
	}

	var glassGump = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", glassGump, socket );
	glassGump.AddPage( 1 );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index = j - startIndex;
		var makeID = pageItems[j];
		var entryText;
		var buttonID = makeID;

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

		// Craft button uses makeID
		glassGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, buttonID );
		glassGump.AddText( 255, 60 + ( index * 20 ), textHue, entryText );

		// Detail button: 20000 + makeID (matches your current onGumpPress)
		glassGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + buttonID );
	}

	// Prev subpage
	if( subPage > 1 )
	{
		glassGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		glassGump.AddHTMLGump( 255, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		glassGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		glassGump.AddHTMLGump( 405, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );
	}

	glassGump.Send( socket );
	glassGump.Free();
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
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

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
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

    var gumpID = scriptID + 0xffff;

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

    // Page tabs
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
        pUser.SetTempTag( "MAKELAST_GLASS", null );
        pUser.SetTempTag( "CRAFT", null );
        socket.CloseGump( gumpID, 0 );
        return;
    }

    var makeID = 0;
    var timerID = 0;

    // Make Last
    if( pButton == 5000 )
    {
        var last = pUser.GetTempTag( "MAKELAST_GLASS" );
        if( last )
            pButton = last;
        else
            return;
    }

    // Craft buttons use makeID directly
    if( CraftingMap[pButton] != undefined )
    {
        makeID = pButton;
        var data = CraftingMap[makeID];
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

        pUser.SetTempTag( "MAKELAST_GLASS", makeID );

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

        pUser.StartTimer( gumpDelay, timerID, scriptID );
        return;
    }

    // Detail buttons: 20000 + makeID
    if( pButton >= 20000 && pButton < 30000 )
    {
        var detailMakeID = pButton - 20000;
        if( CraftingMap[detailMakeID] )
        {
            pUser.SetTempTag( "ITEMDETAILS", detailMakeID );
            TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
        }
        return;
    }
}

// Last Ten
function AddToLastTen( pUser, makeID )
{
    var raw = pUser.GetTag( "LastTenGlassblowing" ) || "";
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

    pUser.SetTag( "LastTenGlassblowing", newList.join( "," ) );
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