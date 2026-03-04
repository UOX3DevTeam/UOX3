/// <reference path="../../definitions.d.ts" />
// @ts-check

let Skill_Filter = [
	{ code: 0, label: "All" },
	{ code: 1, label: "Blacksmith" },
	{ code: 2, label: "Tailor" },
	{ code: 3, label: "Fletching" },
	{ code: 4, label: "Carpentry" },
	{ code: 5, label: "Inscription" },
	{ code: 6, label: "Cooking" },
	{ code: 7, label: "Alchemy" },
	{ code: 8, label: "Tinkering" },
	{ code: 9, label: "Cartography" }
];

let Expansion_Filter = [
	{ code: 0, label: "All" },
	{ code: 1, label: "UO" },
	{ code: 2, label: "T2A" },
	{ code: 3, label: "UOR" },
	{ code: 4, label: "LBR" },
	{ code: 5, label: "AOS" },
	{ code: 6, label: "SE" },
	{ code: 7, label: "ML" },
	{ code: 8, label: "SA" },
	{ code: 9, label: "TOL" }
];

let Amount_Filter = [
	{ code: 0, label: "All" },
	{ code: 1, label: "Owned" },
	{ code: 2, label: "Unknown" }
];

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return false;

	// Require in pack or locked down
	var inPack     = ( iUsed.container === pUser.pack );
	var lockedDown = ( iUsed.movable === 3 || iUsed.movable === 2 );
	var vendor     = CheckPlayerVendor( iUsed );
	var onVendor   = !!vendor;

	if( !inPack && !lockedDown && !onVendor )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26551, pSocket.language )); // The recipe book must be in your pack, locked down, or on a vendor to use it.
		return false;
	}

	pUser.SetTempTag( "RecipeBookSer", iUsed.serial );
	iUsed.SetTag( "recipeBookLastPage", 0 );

	RecipeBookGump( pUser, iUsed );
	return false;
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( iDropped, pUser, recipeBook )
{
	if( !ValidateObject( pUser ))
		return 0;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return 0;

	if( recipeBook.GetTag( "isRecipeBook" ) != 1)
		return 1;

	if( iDropped === recipeBook )
		return 1;

	var recipeID = iDropped.GetTag( "recipeID" );
	if( recipeID <= 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26552, pSocket.language )); // This book is only designed to hold recipe scrolls.
		return 0;
	}

	var existingIDs = {};
	var uniqueCount = 0;

	for( var item = recipeBook.FirstItem(); !recipeBook.FinishedItems(); item = recipeBook.NextItem() )
	{
		if( !ValidateObject( item ) || !item.isItem )
			continue;

		var recipeid = item.GetTag( "recipeID" );
		if( recipeid > 0 && !existingIDs[recipeid] )
		{
			existingIDs[recipeid] = true;
			uniqueCount++;
		}
	}

	if( !existingIDs[recipeID] && uniqueCount >= 250 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26553, pSocket.language )); // This recipe book cannot hold any more unique recipes.
		return 0;
	}

	iDropped.SetCont( recipeBook );

	var rName = iDropped.GetTag( "recipeName" );
	var recipeAddMsg = GetDictionaryEntry( 26554, pSocket.language ); // You place the %s in the book.
	pSocket.SysMessage( recipeAddMsg.replace( /%s/gi, rName.toString() ));
	RecipeBookGump( pUser, recipeBook );
	recipeBook.Refresh();

	return 2;
}

/** @type { ( pUser: Character, book: Item ) => void } */
function RecipeBookGump( pUser, book )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	var rows = BuildRecipeRows( book );

	var page = book.GetTag( "recipeBookLastPage" );
	if( page < 0 )
		page = 0;

	var perPage = 10;
	var index = page * perPage;

	var count = perPage;
	if( index + count > rows.length )
		count = rows.length - index;

	if( count < 0 )
		count = 0;

	var maxPage = 0;
	if( rows.length > 0 )
		maxPage = Math.floor((rows.length - 1) / perPage);

	if( page > maxPage )
	{
		page = maxPage;
		book.SetTag( "recipeBookLastPage", page );
		index = page * perPage;
		if( index < 0 )
			index = 0;
		if( index + count > rows.length )
			count = rows.length - index;
		if( count < 0 )
			count = 0;
	}

	var canLocked = ( book.movable === 3 );
	var canDrop = ( book.container === pUser.pack );
	var vendor = CheckPlayerVendor( book );
	var canBuy = !!vendor;
	var canPrice = ( canDrop || canBuy || canLocked );
	var width = canPrice ? 600 : 516;
	var xOffset = (( 624 - width ) / 2) | 0;
	var yOffset = 24;

	var recipeBookMenu = new Gump();

	recipeBookMenu.AddPage( 0 );
	recipeBookMenu.AddBackground( xOffset + 10, yOffset + 10, width, 439, 5054 );
	recipeBookMenu.AddTiledGump( xOffset + 18, yOffset + 20, width - 17, 420, 2624 );

	if( canPrice )
	{
		recipeBookMenu.AddTiledGump( xOffset + 573, yOffset + 64, 24, 352, 200 );
		recipeBookMenu.AddTiledGump( xOffset + 493, yOffset + 64, 78, 352, 1416 );
	}

	if( canDrop )
		recipeBookMenu.AddTiledGump( xOffset + 24, yOffset + 64, 32, 352, 1416 );

	recipeBookMenu.AddTiledGump( xOffset + 58,  yOffset + 64, 36,  352, 200 );   // icon
	recipeBookMenu.AddTiledGump( xOffset + 96,  yOffset + 64, 133, 352, 1416 );  // item
	recipeBookMenu.AddTiledGump( xOffset + 231, yOffset + 64, 80,  352, 200 );   // expansion
	recipeBookMenu.AddTiledGump( xOffset + 313, yOffset + 64, 100, 352, 1416 );  // crafting
	recipeBookMenu.AddTiledGump( xOffset + 415, yOffset + 64, 76,  352, 200 );   // amount

	var tableIndex = 0;
	for( var i = index; i < index + count && i < rows.length; i++ )
	{
		recipeBookMenu.AddTiledGump( xOffset + 24, yOffset + 94 + (tableIndex * 32), canPrice ? 573 : 489, 2, 2624 );
		tableIndex++;
	}

	recipeBookMenu.AddCheckerTrans( xOffset + 18, yOffset + 20, width - 17, 420 );
	recipeBookMenu.AddGump( xOffset + 0,          yOffset + 0,   10460 );
	recipeBookMenu.AddGump( xOffset + width - 15, yOffset + 5,   10460 );
	recipeBookMenu.AddGump( xOffset + 0,          yOffset + 429, 10460 );
	recipeBookMenu.AddGump( xOffset + width - 15, yOffset + 429, 10460 );
	recipeBookMenu.AddText( xOffset + 266, yOffset + 32, 0x480, GetDictionaryEntry( 26555, pSocket.language )); // Recipe Book
	recipeBookMenu.AddText( xOffset + 147, yOffset + 64, 0x480, GetDictionaryEntry( 26556, pSocket.language )); // Item
	recipeBookMenu.AddText( xOffset + 246, yOffset + 64, 0x480, GetDictionaryEntry( 26557, pSocket.language )); // Expansion
	recipeBookMenu.AddText( xOffset + 336, yOffset + 64, 0x480, GetDictionaryEntry( 26558, pSocket.language )); // Crafting
	recipeBookMenu.AddText( xOffset + 429, yOffset + 64, 0x480, GetDictionaryEntry( 26559, pSocket.language )); // Amount

	// "Set Filter"
	recipeBookMenu.AddText( xOffset + 70, yOffset + 32, 0x480, GetDictionaryEntry( 26560, pSocket.language )); // Set Filter
	recipeBookMenu.AddButton( xOffset + 35, yOffset + 32, 4005, 1, 0, 1 );

	// Filter status
	var filterText = FilterStatusText( book );
	var filterX = canPrice ? ( xOffset + 380 ) : ( xOffset + 386 );
	recipeBookMenu.AddText( filterX, yOffset + 32, 0x0063, filterText );

	recipeBookMenu.AddButton( xOffset + 375, yOffset + 416, 4017, 4018, 1, 0, 0 );
	recipeBookMenu.AddText( xOffset + 410, yOffset + 416, 0x480, GetDictionaryEntry( 10290, pSocket.language )); //EXIT

	// "Drop" header if canDrop
	if( canDrop )
		recipeBookMenu.AddText( xOffset + 26, yOffset + 64, 0x480, GetDictionaryEntry( 26561, pSocket.language )); // Drop

	// Price / Buy|Set headers if canPrice
	if( canPrice )
	{
		recipeBookMenu.AddText( xOffset + 516, yOffset + 64, 0x480, GetDictionaryEntry( 26562, pSocket.language )); // Price

		if( canBuy )
			recipeBookMenu.AddText( xOffset + 576, yOffset + 64, 0x480, GetDictionaryEntry( 26563, pSocket.language )); // Buy
		else
			recipeBookMenu.AddText( xOffset + 576, yOffset + 64, 0x480, GetDictionaryEntry( 26564, pSocket.language )); // Set
	}

	// Prev / Next page buttons
	if( page > 0 )
	{
		recipeBookMenu.AddButton( xOffset + 75, yOffset + 416, 4014, 4016, 1, 0, 2 );
		recipeBookMenu.AddText( xOffset + 110, yOffset + 416, 0x480, GetDictionaryEntry( 26565, pSocket.language )); // Previous page
	}

	if(( page + 1 ) * perPage < rows.length )
	{
		recipeBookMenu.AddButton( xOffset + 225, yOffset + 416, 4005, 1, 0, 3 );
		recipeBookMenu.AddText( xOffset + 260, yOffset + 416, 0x480, GetDictionaryEntry( 26566, pSocket.language )); // Next page
	}

	// Row contents
	tableIndex = 0;

	for( var i = index; i < index + count && i < rows.length; i++ )
	{
		var r = rows[i];
		var y = yOffset + 96 + (tableIndex * 32);
		tableIndex++;

		// Drop button: 4 + (i * 2)
		if( r.amount > 0 && ( canDrop || canLocked ))
		{
			recipeBookMenu.AddButton( xOffset + 35, y + 2, 5602, 1, 0, 4 + ( i * 2 ));
		}

		// Item ID label (like AddLabel(61,y,..., recipe.ID))
		recipeBookMenu.AddText( xOffset + 61, y, 0x480, "" + r.id );

		// Item name
		recipeBookMenu.AddText( xOffset + 103, y, 0x480, r.name );

		// Expansion
		recipeBookMenu.AddText( xOffset + 235, y, 0x480, r.expansion );

		// Crafting (skill)
		recipeBookMenu.AddText( xOffset + 316, y, 0x480, r.craft );

		// Amount
		recipeBookMenu.AddText( xOffset + 421, y, 0x480, "" + r.amount );

		// Price column / button
		if( canPrice )
		{
			// Price button: 5 + (i * 2)
			if( canDrop || ( canBuy && r.price > 0 ))
			{
				recipeBookMenu.AddButton( xOffset + 579, y + 2, 2117, 1, 0, 5 + ( i * 2 ));
			}

			var priceStr = "" + ( r.price );
			recipeBookMenu.AddText( xOffset + 495, y, 1152, priceStr );
		}
	}

	recipeBookMenu.Send( pSocket );
	recipeBookMenu.Free();
}

/** @type { ( pUser: Character, book: Item ) => void } */
function RecipeFilterGump( pUser, book )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null)
		return;

	var filterValue = BookFilterValues( book );

	var filterGump = new Gump();
	filterGump.AddPage(0);
	filterGump.AddBackground(22, 34, 600, 375, 0x13BE);
	filterGump.AddTiledGump(30, 44, 583, 356, 0xA40);
	filterGump.AddCheckerTrans(30, 44, 583, 356);
	filterGump.AddText( 270, 48, 0x480, GetDictionaryEntry( 26567, pSocket.language )); // Filter Preference

	// --- Skill Filters ---
	filterGump.AddText( 36, 80, 0x480, GetDictionaryEntry( 26558, pSocket.language )); // Crafting
	for( var i = 0; i < Skill_Filter.length; i++ )
	{
		var row = Skill_Filter[i];
		var col = i % 4;
		var rowIdx = ( i / 4 ) | 0;

		var x = 45 + ( col * 125 );
		var y = 110 + ( rowIdx * 32 );
		var selected = ( row.code === filterValue.skill );

		// Button ID: 1000 + i
		filterGump.AddButton( x, y, 4005, 1, 0, 1000 + i );
		filterGump.AddText( x + 35, y, selected ? 0x0063 : 0x480, row.label );
	}

	// --- Expansion Filters ---
	filterGump.AddText( 36, 200, 0x480, GetDictionaryEntry( 26557, pSocket.language )); // Expansion
	for( var j = 0; j < Expansion_Filter.length; j++ )
	{
		var erow = Expansion_Filter[j];

		// 0..4 = first row, 5..9 = second row
		var colE = j % 5;
		var rowE = ( j / 5 ) | 0;

		var ex = 45 + ( colE * 105 );
		var ey = 220 + ( rowE * 32 );
		var selectedE = ( erow.code === filterValue.exp );

		// Button ID: 1100 + j
		filterGump.AddButton( ex, ey, 4005, 1, 0, 1100 + j );
		filterGump.AddText( ex + 35, ey, selectedE ? 0x0063 : 0x480, erow.label );
	}

	// --- Amount Filters ---
	filterGump.AddText( 36, 280, 0x480, GetDictionaryEntry( 26559, pSocket.language )); // Amount
	for( var k = 0; k < Amount_Filter.length; k++ )
	{
		var arow = Amount_Filter[k];
		var colA = k % 3;
		var rowA = (k / 3) | 0;

		var ax = 45 + ( colA * 125 );
		var ay = 300 + ( rowA * 32 );
		var selectedA = ( arow.code === filterValue.amt );

		// Button ID: 1200 + k
		filterGump.AddButton( ax, ay, 4005, 1, 0, 1200 + k );
		filterGump.AddText( ax + 35, ay, selectedA ? 0x0063 : 0x480, arow.label );
	}

	// Clear + Apply at bottom
	filterGump.AddButton( 370, 352, 4005, 1, 0, 1301 );
	filterGump.AddText( 405, 352, 0x480, GetDictionaryEntry( 26568, pSocket.language )); // Clear Filter

	filterGump.AddButton( 505, 352, 4017, 1, 0, 1300 );
	filterGump.AddText( 540, 352, 0x480, GetDictionaryEntry( 18014, pSocket.language )); // APPLY

	filterGump.Send( pSocket );
	filterGump.Free();
}

/** @type { ( pUser: Character, book: Item, recipeID: number, price: number, name: string ) => void } */
function RecipeBuyGump( pUser, book, recipeID, price, name )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// Store pending purchase info on the player
	pUser.SetTempTag( "RecipeBookBuyRID", recipeID );
	pUser.SetTempTag( "RecipeBookBuyPrice", price );
	pUser.SetTempTag( "RecipeBookSer", book.serial );

	var recipeBuyMenu = new Gump();
	recipeBuyMenu.AddPage( 0 );

	// Background
	recipeBuyMenu.AddBackground( 100, 10, 300, 150, 5054 );

	// Text
	recipeBuyMenu.AddText( 125, 25, 0x480, GetDictionaryEntry( 26569, pSocket.language )); // You have agreed to purchase:
	recipeBuyMenu.AddText( 125, 45, 0x480, name );
	recipeBuyMenu.AddText( 125, 70, 0x480, GetDictionaryEntry( 26570, pSocket.language )); // for the amount of:
	recipeBuyMenu.AddText( 125, 90, 0x480, "" + price + " gold" );

	// CANCEL button (501)
	recipeBuyMenu.AddButton( 250, 130, 4005, 4007, 1, 0, 501 );
	recipeBuyMenu.AddText( 282, 130, 0x480, GetDictionaryEntry( 18721, pSocket.language )); // CANCEL

	// OK button (500)
	recipeBuyMenu.AddButton( 120, 130, 4005, 4007, 1, 0, 500 );
	recipeBuyMenu.AddText( 152, 130, 0x480, GetDictionaryEntry( 18718, pSocket.language )); // OKAY

	recipeBuyMenu.Send( pSocket );
	recipeBuyMenu.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSocket, buttonID, gumpData )
{
	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( pSocket == null )
		return;

	if( buttonID === 500 || buttonID === 501 )
	{
		var ser  = pUser.GetTempTag( "RecipeBookSer" );
		if( ser <= 0 )
			return;

		var book = CalcItemFromSer( ser );
		if( !ValidateObject( book ) || book.GetTag( "isRecipeBook" ) != 1)
			return;

		if( buttonID === 501 )
		{
			// Cancel
			pUser.SysMessage( GetDictionaryEntry( 26571, pSocket.language )); // You decide not to buy that recipe.
			RecipeBookGump( pUser, book );
			// Clear pending buy tags
			pUser.SetTempTag( "RecipeBookBuyRID", null );
			pUser.SetTempTag( "RecipeBookBuyPrice", null );
			return;
		}

		// OK (500)
		var rid   = pUser.GetTempTag( "RecipeBookBuyRID" );
		var price = pUser.GetTempTag( "RecipeBookBuyPrice" );

		if( rid <= 0 || price < 0 )
		{
			pUser.SysMessage( GetDictionaryEntry( 26572, pSocket.language )); // Purchase information is invalid.
			RecipeBookGump( pUser, book );
			return;
		}

		// Check still available
		var rows = BuildRecipeRows( book );
		var foundRow = null;
		for( var i = 0; i < rows.length; i++ )
		{
			if( rows[i].id === rid )
			{
				foundRow = rows[i];
				break;
			}
		}

		if( !foundRow || foundRow.amount <= 0 )
		{
			pUser.SysMessage( GetDictionaryEntry( 26573, pSocket.language )); // The recipe selected is not available.
			RecipeBookGump( pUser, book );
			return;
		}

		if( !PayFromBackpackOrBank( pUser, price ))
		{
			var goldMsg = GetDictionaryEntry( 26574, pSocket.language ); // You need %s gold in your backpack or bank.
			pSocket.SysMessage( goldMsg.replace( /%s/gi, price.toString() ));
			RecipeBookGump( pUser, book );
			return;
		}

		if( FromBookToChar( book, rid, pUser ))
		{
			book.Refresh();
			var msg = GetDictionaryEntry( 26575, pSocket.language ); // "You buy the recipe '%s' for %s gold."
			msg = msg.replace( "%s", foundRow.name );
			msg = msg.replace( "%s", price.toString() );
			pSocket.SysMessage( msg );
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 26573, pSocket.language ));  // The recipe selected is not available.
		}

		// Clear pending buy tags
		pUser.SetTempTag( "RecipeBookBuyRID", null );
		pUser.SetTempTag( "RecipeBookBuyPrice", null );

		RecipeBookGump( pUser, book );
		return;
	}

	// Find the book we opened this from
	var ser = pUser.GetTempTag( "RecipeBookSer" );
	if( ser <= 0 )
		return;

	var book = CalcItemFromSer( ser );
	if( !ValidateObject( book ) || book.GetTag( "isRecipeBook" ) != 1)
		return;

	// Apply / Clear
	if( buttonID === 1300 )
	{
		// APPLY: just reopen the main book gump
		RecipeBookGump( pUser, book );
		return;
	}
	if( buttonID === 1301 )
	{
		// CLEAR: reset all filters to 0
		book.SetTag( "recipeBookFilterSkill", 0 );
		book.SetTag( "recipeBookFilterExp", 0 );
		book.SetTag( "recipeBookFilterAmount", 0 );
		RecipeFilterGump( pUser, book );
		return;
	}

	// Skill filter buttons
	if( buttonID >= 1000 && buttonID < 1100 )
	{
		var idx = buttonID - 1000;
		if( idx >= 0 && idx < Skill_Filter.length )
		{
			book.SetTag( "recipeBookFilterSkill", Skill_Filter[idx].code );
		}
		RecipeFilterGump( pUser, book );
		return;
	}

	// Expansion filter buttons
	if( buttonID >= 1100 && buttonID < 1200 )
	{
		var idxE = buttonID - 1100;
		if( idxE >= 0 && idxE < Expansion_Filter.length )
		{
			book.SetTag("recipeBookFilterExp", Expansion_Filter[idxE].code );
		}
		RecipeFilterGump( pUser, book );
		return;
	}

	// Amount filter buttons
	if( buttonID >= 1200 && buttonID < 1300 )
	{
		var idxA = buttonID - 1200;
		if( idxA >= 0 && idxA < Amount_Filter.length )
		{
			book.SetTag( "recipeBookFilterAmount", Amount_Filter[idxA].code );
		}
		RecipeFilterGump( pUser, book );
		return;
	}

	var page = book.GetTag( "recipeBookLastPage" );
	if( page < 0 )
		page = 0;

	var rows = BuildRecipeRows( book );
	var perPage = 10;

	var canLocked = ( book.movable === 3 );             // locked down
	var canDrop = ( book.container === pUser.pack );    // in backpack
	var vendor = CheckPlayerVendor( book );
	var canBuy = !!vendor;
	var canPrice = ( canDrop || canBuy || canLocked );

	switch( buttonID )
	{
		case 0: // EXIT
		{
			return;
		}

		case 1: // Set Filter
		{
			RecipeFilterGump(pUser, book);
			return;
		}

		case 2: // Previous page
		{
			if( page > 0 )
				page--;

			book.SetTag( "recipeBookLastPage", page );
			RecipeBookGump( pUser, book );
			return;
		}

		case 3: // Next page
		{
			if( (page + 1) * perPage < rows.length )
				page++;

			book.SetTag( "recipeBookLastPage", page );
			RecipeBookGump( pUser, book );
			return;
		}
	}

	if( buttonID < 4 )
		return;

	var index = buttonID - 4;
	var type = index % 2;
	index = (index / 2) | 0;

	if( index < 0 || index >= rows.length )
	{
		RecipeBookGump( pUser, book );
		return;
	}

	var r = rows[index];

	if( type === 0 )
	{
		if( !( canDrop || canLocked ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 26576, pSocket.language )); // You are not allowed to access this.
			return;
		}

		if( r.amount <= 0 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 26577, pSocket.language )); // The recipe selected is not available.
			RecipeBookGump( pUser, book );
			return;
		}

		var pack = pUser.pack;
		if( !pack )
		{
			pSocket.SysMessage( GetDictionaryEntry( 8073, pSocket.language )); // You do not have a backpack.
			return;
		}

		if(( pack.totalItemCount >= pack.maxItems ) || ( pack.weight >= pack.weightMax ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 26578, pSocket.language )); // There is not enough room in your backpack for the recipe.
			return;
		}

		if( FromBookToChar( book, r.id, pUser ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 26579, pSocket.language )); // The recipe has been placed in your backpack.
			book.Refresh();
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 26577, pSocket.language )); // The recipe selected is not available.
		}

		RecipeBookGump( pUser, book );
		return;
	}

	if( type === 1 )
	{
		if( canDrop )
		{
			pUser.SetTempTag( "RecipeBookPriceRID", r.id );
			pUser.SetTempTag( "RecipeBookSer", book.serial );
			var priceMsgs = GetDictionaryEntry( 26580, pSocket.language ); // Type the price in gold for recipe: %s
			pSocket.SysMessage( priceMsgs.replace( /%s/gi, r.name ));
			pUser.SpeechInput( 100, book );
		}
		else if( canBuy )
		{
			if( r.amount <= 0 )
			{
				pUser.SysMessage( GetDictionaryEntry( 26577, pSocket.language ));// The recipe selected is not available.
				return;
			}

			if( r.price <= 0 )
			{
				pUser.SysMessage( GetDictionaryEntry( 26581, pSocket.language )); // This recipe is not for sale.
				return;
			}

			RecipeBuyGump( pUser, book, r.id, r.price, r.name );

			return;
		}

	}

	return;
}

/** @type { ( myChar: Character, myItem: Item, mySpeech: string, mySpeechId: number ) => void } */
function onSpeechInput( pUser, pItem, pSpeech, pSpeechID )
{
	if( pSpeechID !== 100 )
		return;

	if( !ValidateObject( pItem ) || pItem.GetTag( "isRecipeBook" ) != 1 )
		return;

	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null)
		return;

	// Find which recipe we are editing (TEMP tag)
	var recipeid = pUser.GetTempTag( "RecipeBookPriceRID" );
	if( recipeid <= 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26581, pSocket.language )); // No recipe selected for pricing.
		return;
	}

	if( pSpeech == null || pSpeech === " " || pSpeech === "" )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26582, pSocket.language )); // You did not type a price.
		return;
	}

	// Try to parse the price
	var price = parseInt( pSpeech, 10 );
	if( isNaN( price ) || price < 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26583, pSocket.language )); //That is not a valid price. Please enter a number.
		return;
	}

	// Set price on all matching scrolls inside this book
	var updated = false;
	for( var item = pItem.FirstItem(); !pItem.FinishedItems(); item = pItem.NextItem() )
	{
		if( !ValidateObject( item ) || !item.isItem )
			continue;

		var recipe = item.GetTag( "recipeID" );
		if( recipe === recipeid )
		{
			item.SetTag( "recipePrice", price );
			updated = true;
		}
	}

	if( !updated )
	{
		pSocket.SysMessage( GetDictionaryEntry( 26584, pSocket.language )); // No matching scrolls were found in this book for that recipe.
		return;
	}

	pItem.Refresh();

	// Clear the temp tag
	pUser.SetTempTag( "RecipeBookPriceRID", null );

	// Name for feedback pull from first matching scroll
	var name = "recipe " + recipeid;
	for( var item2 = pItem.FirstItem(); !pItem.FinishedItems(); item2 = pItem.NextItem() )
	{
		if( ValidateObject( item2 ) && item2.isItem )
		{
			var r2 = item2.GetTag( "recipeID" );
			if( r2 === recipeid )
			{
				var n = item2.GetTag( "recipeName" );
				if( n && n !== "" )
					name = n;
				break;
			}
		}
	}

	var setPriceMsg = GetDictionaryEntry( 26586, pSocket.language ); // "You set the price for %s to %i gold."
	setPriceMsg = setPriceMsg.replace( "%s", name );
	setPriceMsg = setPriceMsg.replace( "%i", price.toString() );
	pSocket.SysMessage( setPriceMsg );
	RecipeBookGump( pUser, pItem );
}

/** @type { ( book: Item, recipeID: number, pUser: Character ) => boolean } */
function FromBookToChar( book, recipeID, pUser )
{
	if( !ValidateObject( book ) || !ValidateObject( pUser ))
		return false;

	var pack = pUser.pack;
	if( !ValidateObject( pack ))
		return false;

	var item = book.FirstItem();
	while( !book.FinishedItems() )
	{
		var cur = item;
		item = book.NextItem();

		if( !ValidateObject( cur ) || !cur.isItem )
			continue;

		var recipeId = cur.GetTag( "recipeID" );
		if( recipeId === ( recipeID ))
		{
			cur.SetTag( "recipePrice", null );
			cur.SetCont( pack );
			return true;
		}
	}
	return false;
}

/** @type { ( book: Item ) => ( Character | null ) } */
function CheckPlayerVendor( book )
{
	if( !ValidateObject( book ) || !book.isItem )
		return null;

	var root = book;
	var guard = 0;
	while( root.container && guard < 10 )
	{
		root = root.container;
		guard++;
	}

	if( root.isChar && ( root.aitype ) === 17 )
		return root;

	return null;
}

/** @type { ( pUser: Character, amount: number ) => boolean } */
function PayFromBackpackOrBank( pUser, amount )
{
	amount |= 0;
	if( amount <= 0 )
		return true;

	if( !ValidateObject( pUser ))
		return false;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return false;

	if( pUser.ResourceCount( 0x0EED, 0 ) >= amount )
	{
		pUser.UseResource( amount, 0x0EED );
		var payGold = GetDictionaryEntry( 26587, pSocket.language ); // %s gold has been paid from your backpack.
		pSocket.SysMessage( payGold.replace( /%s/gi, amount ));
		return true;
	}

	var bankBox = pUser.FindItemLayer( 29 );
	if( bankBox )
	{
		var bankItem;
		var foundGold = false;
		for( bankItem = bankBox.FirstItem(); !bankBox.FinishedItems(); bankItem = bankBox.NextItem() )
		{
			if( ValidateObject( bankItem ) && bankItem.id == 0x0EED && bankItem.amount >= amount )
			{
				bankBox.UseResource( amount, 0x0EED );
				foundGold = true;
				break;
			}
		}
		if( foundGold )
		{
			var payBank = GetDictionaryEntry( 26588, pSocket.language ); // %s gold has been paid from your bank account.
			pSocket.SysMessage( payBank.replace( /%s/gi, amount ));
			return true;
		}
	}
	return false;
}

/** @type { ( skillID: number ) => string } */
function GetSkillName( skillID )
{
	switch( skillID )
	{
		case 0: return "Alchemy";
		case 7: return "Blacksmithing";
		case 8: return "Fletching";
		case 11: return "Carpentry";
		case 12: return "Cartography";
		case 13: return "Cooking";
		case 23: return "Inscription";
		case 34: return "Tailoring";
		case 37: return "Tinkering";
	}
	return "Unknown";
}

/** @type { ( book: Item ) => { skill: number, exp: number, amt: number } } */
function BookFilterValues( book )
{
	return {
		skill: book.GetTag( "recipeBookFilterSkill" ),
		exp:   book.GetTag( "recipeBookFilterExp" ),
		amt:   book.GetTag( "recipeBookFilterAmount" )
	};
}

/** @type { ( row: { skillID: number }, skillFilter: number ) => boolean } */
function MatchesSkillFilter( row, skillFilter )
{
	if( skillFilter === 0 )
		return true;

	switch( skillFilter )
	{
		case 1: return row.skillID === 7;   // Blacksmith
		case 2: return row.skillID === 34;  // Tailor
		case 3: return row.skillID === 8;   // Fletching
		case 4: return row.skillID === 11;  // Carpentry
		case 5: return row.skillID === 23;  // Inscription
		case 6: return row.skillID === 13;  // Cooking
		case 7: return row.skillID === 0;   // Alchemy
		case 8: return row.skillID === 37;  // Tinkering
		case 9: return row.skillID === 12;  // Cartography
	}
	return true;
}

/** @type { ( row: { expansion: string }, expFilter: number ) => boolean } */
function MatchesExpansionFilter( row, expFilter )
{
	if( expFilter === 0 )
		return true;

	var ex = ( row.expansion || "" ).toLowerCase();

	switch( expFilter )
	{
		case 1: // UO
			return ex === "UO";
		case 2: // T2A
			return ex === "T2A";
		case 3: // UOR
			return ex === "UOR";
		case 4: // LBR
			return ex === "LBR";
		case 5: // AOS
			return ex === "AOS";
		case 6: // SE
			return ex === "SE";
		case 7: // ML
			return ex === "ML";
		case 8: // SA
			return ex === "SA";
		case 9: // ToL
			return ex === "TOL";
	}
	return true;
}

/** @type { ( row: { amount: number }, amtFilter: number ) => boolean } */
function MatchesAmountFilter( row, amtFilter )
{
	switch( amtFilter )
	{
		case 0: return true;          // All
		case 1: return row.amount > 0; // Owned
		case 2: return row.amount <= 0; // Unknown (will generally be none in this design)
	}
	return true;
}

/** @type { ( book: Item ) => string } */
function FilterStatusText( book )
{
	var filter = BookFilterValues( book );

	if( filter.skill === 0 && filter.exp === 0 && filter.amt === 0 )
		return " ";

	var parts = [];

	// Skill
	for( var i = 0; i < Skill_Filter.length; i++ )
	{
		if( Skill_Filter[i].code === filter.skill )
		{
			parts.push(Skill_Filter[i].label);
			break;
		}
	}

	// Expansion
	for( var j = 0; j < Expansion_Filter.length; j++ )
	{
		if( Expansion_Filter[j].code === filter.exp )
		{
			parts.push( Expansion_Filter[j].label );
			break;
		}
	}

	// Amount
	for( var k = 0; k < Amount_Filter.length; k++ )
	{
		if( Amount_Filter[k].code === filter.amt )
		{
			parts.push( Amount_Filter[k].label );
			break;
		}
	}

	if( parts.length <= 0 )
		return "Using No Filter";

	return "Filter: " + parts.join( " / " );
}

/** @type { ( book: Item ) => Array<{   id: number,   name: string,   expansion: string,   craft: string,   skillID: number,   amount: number,   price: number }> } */
function BuildRecipeRows( book )
{
	var rows = [];
	if( !ValidateObject( book ) || !book.isItem )
		return rows;

	var filterVals = BookFilterValues( book );
	var skillFilter = filterVals.skill;
	var expFilter   = filterVals.exp;
	var amtFilter   = filterVals.amt;
	var map = {};

	for( var item = book.FirstItem(); !book.FinishedItems(); item = book.NextItem() )
	{
		if( !ValidateObject(item) || !item.isItem )
			continue;

		var recipeId = item.GetTag( "recipeID" );
		if( recipeId <= 0 )
			continue;

		var entry = map[recipeId];
		if (!entry)
		{
			var rName = item.GetTag( "recipeName" );
			var sID   = item.GetTag( "skillID" );
			var exp   = item.GetTag( "expansion" );
			var pVal  = item.GetTag( "recipePrice" );
			var price = 0;
			if( pVal !== null && pVal !== undefined && pVal !== "" )
			{
				var parsed = parseInt( pVal, 10 );
				if( !isNaN( parsed ) && parsed >= 0 )
					price = parsed;
			}

			entry = {
				id: recipeId,
				name: rName,
				expansion: exp,
				craft: GetSkillName(sID),
				skillID: sID,
				amount: 0,
				price: price
			};
			map[recipeId] = entry;
		}

		var amt = item.amount | 0;
		if( amt <= 0 )
			amt = 1;
		entry.amount += amt;
	}

	for( var key in map )
	{
		if( map.hasOwnProperty( key ))
			rows.push( map[key] );
	}

	// Apply filters
	rows = rows.filter( function( r )
	{
		if( !MatchesSkillFilter( r, skillFilter ))
			return false;
		if( !MatchesExpansionFilter( r, expFilter ))
			return false;
		if( !MatchesAmountFilter( r, amtFilter ))
			return false;
		return true;
	});

	rows.sort( function( a, b ) { return a.id - b.id; } );

	return rows;
}
