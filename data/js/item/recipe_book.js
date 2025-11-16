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

function onUseChecked( pUser, iUsed )
{
	if( !pUser || !iUsed )
		return false;

	var pSocket = pUser.socket;
	if( !pSocket )
		return false;

	// Require in pack or locked down
	var inPack     = ( iUsed.container === pUser.pack );
	var lockedDown = ( iUsed.movable === 3 || iUsed.movable === 2 );
	var vendor     = checkPlayerVendor( iUsed );
	var onVendor   = !!vendor;

	if( !inPack && !lockedDown && !onVendor )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6551, pSocket.language )); // The recipe book must be in your pack, locked down, or on a vendor to use it.
		return false;
	}

	pUser.SetTempTag( "RecipeBookSer", iUsed.serial );
	iUsed.SetTag( "rb_last_page", 0 );

	recipeBookGump( pUser, iUsed );
	return false;
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( iDropped, pUser, recipeBook )
{
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
		pSocket.SysMessage( GetDictionaryEntry( 6552, pSocket.language )); // This book is only designed to hold recipe scrolls.
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
		pSocket.SysMessage( GetDictionaryEntry( 6553, pSocket.language )); // This recipe book cannot hold any more unique recipes.
		return 0;
	}

	iDropped.SetCont( recipeBook );

	var rName = iDropped.GetTag( "recipeName" );
	var recipeAddMsg = GetDictionaryEntry( 6554, pSocket.language ); // You place the %s in the book.
	pSocket.SysMessage( recipeAddMsg.replace( /%s/gi, rName.toString() ));
	recipeBookGump( pUser, recipeBook );
	recipeBook.Refresh();

	return 2;
}

function recipeBookGump( pUser, book )
{
	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	var rows = buildRecipeRows( book );

	var page = book.GetTag( "rb_last_page" );
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
		book.SetTag( "rb_last_page", page );
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
	var vendor = checkPlayerVendor( book );
	var canBuy = !!vendor;
	var canPrice = ( canDrop || canBuy || canLocked );
	var width = canPrice ? 600 : 516;
	var xOffset = (( 624 - width ) / 2) | 0;
	var yOffset = 24;

	var recipeBookGump = new Gump();

	recipeBookGump.AddPage( 0 );
	recipeBookGump.AddBackground( xOffset + 10, yOffset + 10, width, 439, 5054 );
	recipeBookGump.AddTiledGump( xOffset + 18, yOffset + 20, width - 17, 420, 2624 );

	if( canPrice )
	{
		recipeBookGump.AddTiledGump( xOffset + 573, yOffset + 64, 24, 352, 200 );
		recipeBookGump.AddTiledGump( xOffset + 493, yOffset + 64, 78, 352, 1416 );
	}

	if( canDrop )
		recipeBookGump.AddTiledGump( xOffset + 24, yOffset + 64, 32, 352, 1416 );

	recipeBookGump.AddTiledGump( xOffset + 58,  yOffset + 64, 36,  352, 200 );   // icon
	recipeBookGump.AddTiledGump( xOffset + 96,  yOffset + 64, 133, 352, 1416 );  // item
	recipeBookGump.AddTiledGump( xOffset + 231, yOffset + 64, 80,  352, 200 );   // expansion
	recipeBookGump.AddTiledGump( xOffset + 313, yOffset + 64, 100, 352, 1416 );  // crafting
	recipeBookGump.AddTiledGump( xOffset + 415, yOffset + 64, 76,  352, 200 );   // amount

	var tableIndex = 0;
	for( var i = index; i < index + count && i < rows.length; i++ )
	{
		recipeBookGump.AddTiledGump( xOffset + 24, yOffset + 94 + (tableIndex * 32), canPrice ? 573 : 489, 2, 2624 );
		tableIndex++;
	}

	recipeBookGump.AddCheckerTrans( xOffset + 18, yOffset + 20, width - 17, 420 );
	recipeBookGump.AddGump( xOffset + 0,          yOffset + 0,   10460 );
	recipeBookGump.AddGump( xOffset + width - 15, yOffset + 5,   10460 );
	recipeBookGump.AddGump( xOffset + 0,          yOffset + 429, 10460 );
	recipeBookGump.AddGump( xOffset + width - 15, yOffset + 429, 10460 );
	recipeBookGump.AddText( xOffset + 266, yOffset + 32, 0x480, GetDictionaryEntry( 6555, pSocket.language )); // Recipe Book
	recipeBookGump.AddText( xOffset + 147, yOffset + 64, 0x480, GetDictionaryEntry( 6556, pSocket.language )); // Item
	recipeBookGump.AddText( xOffset + 246, yOffset + 64, 0x480, GetDictionaryEntry( 6557, pSocket.language )); // Expansion
	recipeBookGump.AddText( xOffset + 336, yOffset + 64, 0x480, GetDictionaryEntry( 6558, pSocket.language )); // Crafting
	recipeBookGump.AddText( xOffset + 429, yOffset + 64, 0x480, GetDictionaryEntry( 6559, pSocket.language )); // Amount

	// "Set Filter"
	recipeBookGump.AddText( xOffset + 70, yOffset + 32, 0x480, GetDictionaryEntry( 6560, pSocket.language )); // Set Filter
	recipeBookGump.AddButton( xOffset + 35, yOffset + 32, 4005, 1, 0, 1 );

	// Filter status
	var filterText = getFilterStatusText( book );
	var filterX = canPrice ? ( xOffset + 380 ) : ( xOffset + 386 );
	recipeBookGump.AddText( filterX, yOffset + 32, 0x0063, filterText );

	recipeBookGump.AddButton( xOffset + 375, yOffset + 416, 4017, 4018, 1, 0, 0 );
	recipeBookGump.AddText( xOffset + 410, yOffset + 416, 0x480, GetDictionaryEntry( 10290, pSocket.language )); //EXIT

	// "Drop" header if canDrop
	if( canDrop )
		recipeBookGump.AddText( xOffset + 26, yOffset + 64, 0x480, GetDictionaryEntry( 6561, pSocket.language )); // Drop

	// Price / Buy|Set headers if canPrice
	if( canPrice )
	{
		recipeBookGump.AddText( xOffset + 516, yOffset + 64, 0x480, GetDictionaryEntry( 6562, pSocket.language )); // Price

		if( canBuy )
			recipeBookGump.AddText( xOffset + 576, yOffset + 64, 0x480, GetDictionaryEntry( 6563, pSocket.language )); // Buy
		else
			recipeBookGump.AddText( xOffset + 576, yOffset + 64, 0x480, GetDictionaryEntry( 6564, pSocket.language )); // Set
	}

	// Prev / Next page buttons
	if( page > 0 )
	{
		recipeBookGump.AddButton( xOffset + 75, yOffset + 416, 4014, 4016, 1, 0, 2 );
		recipeBookGump.AddText( xOffset + 110, yOffset + 416, 0x480, GetDictionaryEntry( 6565, pSocket.language )); // Previous page
	}

	if(( page + 1 ) * perPage < rows.length )
	{
		recipeBookGump.AddButton( xOffset + 225, yOffset + 416, 4005, 1, 0, 3 );
		recipeBookGump.AddText( xOffset + 260, yOffset + 416, 0x480, GetDictionaryEntry( 6566, pSocket.language )); // Next page
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
			recipeBookGump.AddButton( xOffset + 35, y + 2, 5602, 1, 0, 4 + ( i * 2 ));
		}

		// Item ID label (like AddLabel(61,y,..., recipe.ID))
		recipeBookGump.AddText( xOffset + 61, y, 0x480, "" + r.id );

		// Item name
		recipeBookGump.AddText( xOffset + 103, y, 0x480, r.name );

		// Expansion
		recipeBookGump.AddText( xOffset + 235, y, 0x480, r.expansion );

		// Crafting (skill)
		recipeBookGump.AddText( xOffset + 316, y, 0x480, r.craft );

		// Amount
		recipeBookGump.AddText( xOffset + 421, y, 0x480, "" + r.amount );

		// Price column / button
		if( canPrice )
		{
			// Price button: 5 + (i * 2)
			if( canDrop || ( canBuy && r.price > 0 ))
			{
				recipeBookGump.AddButton( xOffset + 579, y + 2, 2117, 1, 0, 5 + ( i * 2 ));
			}

			var priceStr = "" + ( r.price );
			recipeBookGump.AddText( xOffset + 495, y, 1152, priceStr );
		}
	}

	recipeBookGump.Send( pSocket );
	recipeBookGump.Free();
}

function recipeFilterGump( pUser, book )
{
	var pSocket = pUser.socket;
	if( pSocket == null)
		return;

	var filterValue = getBookFilterValues( book );

	var filterGump = new Gump();
	filterGump.AddPage(0);
	filterGump.AddBackground(22, 34, 600, 375, 0x13BE);
	filterGump.AddTiledGump(30, 44, 583, 356, 0xA40);
	filterGump.AddCheckerTrans(30, 44, 583, 356);
	filterGump.AddText( 270, 48, 0x480, GetDictionaryEntry( 6567, pSocket.language )); // Filter Preference

	// --- Skill Filters ---
	filterGump.AddText( 36, 80, 0x480, GetDictionaryEntry( 6558, pSocket.language )); // Crafting
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
	filterGump.AddText( 36, 200, 0x480, GetDictionaryEntry( 6557, pSocket.language )); // Expansion
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
	filterGump.AddText( 36, 280, 0x480, GetDictionaryEntry( 6559, pSocket.language )); // Amount
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
	filterGump.AddText( 405, 352, 0x480, GetDictionaryEntry( 6568, pSocket.language )); // Clear Filter

	filterGump.AddButton( 505, 352, 4017, 1, 0, 1300 );
	filterGump.AddText( 540, 352, 0x480, GetDictionaryEntry( 18014, pSocket.language )); // APPLY

	filterGump.Send( pSocket );
	filterGump.Free();
}

function recipeBuyGump( pUser, book, recipeID, price, name )
{
	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	// Store pending purchase info on the player
	pUser.SetTempTag( "RecipeBookBuyRID", recipeID );
	pUser.SetTempTag( "RecipeBookBuyPrice", price );
	pUser.SetTempTag( "RecipeBookSer", book.serial );

	var g = new Gump();
	g.AddPage( 0 );

	// Background
	g.AddBackground( 100, 10, 300, 150, 5054 );

	// Text
	g.AddText( 125, 25, 0x480, GetDictionaryEntry( 6569, pSocket.language )); // You have agreed to purchase:
	g.AddText( 125, 45, 0x480, name );
	g.AddText( 125, 70, 0x480, GetDictionaryEntry( 6570, pSocket.language )); // for the amount of:
	g.AddText( 125, 90, 0x480, "" + price + " gold" );

	// CANCEL button (501)
	g.AddButton( 250, 130, 4005, 4007, 1, 0, 501 );
	g.AddText( 282, 130, 0x480, GetDictionaryEntry( 18721, pSocket.language )); // CANCEL

	// OK button (500)
	g.AddButton( 120, 130, 4005, 4007, 1, 0, 500 );
	g.AddText( 152, 130, 0x480, GetDictionaryEntry( 18718, pSocket.language )); // OKAY

	g.Send( pSocket );
	g.Free();
}

function onGumpPress( pSocket, buttonID, gumpData )
{
	var pUser = pSocket.currentChar;
	if( !pUser )
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
			pUser.SysMessage( GetDictionaryEntry( 6571, pSocket.language )); // You decide not to buy that recipe.
			recipeBookGump( pUser, book );
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
			pUser.SysMessage( GetDictionaryEntry( 6572, pSocket.language )); // Purchase information is invalid.
			recipeBookGump( pUser, book );
			return;
		}

		// Check still available
		var rows = buildRecipeRows( book );
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
			pUser.SysMessage( GetDictionaryEntry( 6573, pSocket.language )); // The recipe selected is not available.
			recipeBookGump( pUser, book );
			return;
		}

		if( !payFromBackpackOrBank( pUser, price ))
		{
			var goldMsg = GetDictionaryEntry( 6574, pSocket.language ); // You need %s gold in your backpack or bank.
			pSocket.SysMessage( goldMsg.replace( /%s/gi, price.toString() ));
			recipeBookGump( pUser, book );
			return;
		}

		if( fromBookToChar( book, rid, pUser ))
		{
			book.Refresh();
			var msg = GetDictionaryEntry( 6575, pSocket.language ); // "You buy the recipe '%s' for %s gold."
			msg = msg.replace( "%s", foundRow.name );
			msg = msg.replace( "%s", price.toString() );
			pSocket.SysMessage( msg );
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 6573, pSocket.language ));  // The recipe selected is not available.
		}

		// Clear pending buy tags
		pUser.SetTempTag( "RecipeBookBuyRID", null );
		pUser.SetTempTag( "RecipeBookBuyPrice", null );

		recipeBookGump( pUser, book );
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
		recipeBookGump( pUser, book );
		return;
	}
	if( buttonID === 1301 )
	{
		// CLEAR: reset all filters to 0
		book.SetTag( "rb_f_skill", 0 );
		book.SetTag( "rb_f_exp", 0 );
		book.SetTag( "rb_f_amt", 0 );
		recipeFilterGump( pUser, book );
		return;
	}

	// Skill filter buttons
	if( buttonID >= 1000 && buttonID < 1100 )
	{
		var idx = buttonID - 1000;
		if( idx >= 0 && idx < Skill_Filter.length )
		{
			book.SetTag( "rb_f_skill", Skill_Filter[idx].code );
		}
		recipeFilterGump( pUser, book );
		return;
	}

	// Expansion filter buttons
	if( buttonID >= 1100 && buttonID < 1200 )
	{
		var idxE = buttonID - 1100;
		if( idxE >= 0 && idxE < Expansion_Filter.length )
		{
			book.SetTag("rb_f_exp", Expansion_Filter[idxE].code );
		}
		recipeFilterGump( pUser, book );
		return;
	}

	// Amount filter buttons
	if( buttonID >= 1200 && buttonID < 1300 )
	{
		var idxA = buttonID - 1200;
		if( idxA >= 0 && idxA < Amount_Filter.length )
		{
			book.SetTag( "rb_f_amt", Amount_Filter[idxA].code );
		}
		recipeFilterGump( pUser, book );
		return;
	}

	var page = book.GetTag( "rb_last_page" );
	if( page < 0 )
		page = 0;

	var rows = buildRecipeRows( book );
	var perPage = 10;

	var canLocked = ( book.movable === 3 );             // locked down
	var canDrop = ( book.container === pUser.pack );    // in backpack
	var vendor = checkPlayerVendor( book );
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
			recipeFilterGump(pUser, book);
			return;
		}

		case 2: // Previous page
		{
			if( page > 0 )
				page--;

			book.SetTag( "rb_last_page", page );
			recipeBookGump( pUser, book );
			return;
		}

		case 3: // Next page
		{
			if( (page + 1) * perPage < rows.length )
				page++;

			book.SetTag( "rb_last_page", page );
			recipeBookGump( pUser, book );
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
		recipeBookGump( pUser, book );
		return;
	}

	var r = rows[index];

	if( type === 0 )
	{
		if( !( canDrop || canLocked ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 6576, pSocket.language )); // You are not allowed to access this.
			return;
		}

		if( r.amount <= 0 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 6577, pSocket.language )); // The recipe selected is not available.
			recipeBookGump( pUser, book );
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
			pSocket.SysMessage( GetDictionaryEntry( 6578, pSocket.language )); // There is not enough room in your backpack for the recipe.
			return;
		}

		if( fromBookToChar( book, r.id, pUser ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 6579, pSocket.language )); // The recipe has been placed in your backpack.
			book.Refresh();
		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 6577, pSocket.language )); // The recipe selected is not available.
		}

		recipeBookGump( pUser, book );
		return;
	}

	if( type === 1 )
	{
		if( canDrop )
		{
			pUser.SetTempTag( "RecipeBookPriceRID", r.id );
			pUser.SetTempTag( "RecipeBookSer", book.serial );
			var priceMsgs = GetDictionaryEntry( 6580, pSocket.language ); // Type the price in gold for recipe: %s
			pSocket.SysMessage( priceMsgs.replace( /%s/gi, r.name ));
			pUser.SpeechInput( 100, book );
		}
		else if( canBuy )
		{
			if( r.amount <= 0 )
			{
				pUser.SysMessage( GetDictionaryEntry( 6577, pSocket.language ));// The recipe selected is not available.
				return;
			}

			if( r.price <= 0 )
			{
				pUser.SysMessage( GetDictionaryEntry( 6581, pSocket.language )); // This recipe is not for sale.
				return;
			}

			recipeBuyGump( pUser, book, r.id, r.price, r.name );

			return;
		}

	}

	return;
}

function onSpeechInput( pUser, pItem, pSpeech, pSpeechID )
{
	if( pSpeechID !== 100 )
		return;

	if( !ValidateObject( pItem ) || pItem.GetTag( "isRecipeBook" ) != 1 )
		return;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	// Find which recipe we are editing (TEMP tag)
	var recipeid = pUser.GetTempTag( "RecipeBookPriceRID" );
	if( recipeid <= 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6581, pSocket.language )); // No recipe selected for pricing.
		return;
	}

	if( pSpeech == null || pSpeech === " " || pSpeech === "" )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6582, pSocket.language )); // You did not type a price.
		return;
	}

	// Try to parse the price
	var price = parseInt( pSpeech, 10 );
	if( isNaN( price ) || price < 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6583, pSocket.language )); //That is not a valid price. Please enter a number.
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
		pSocket.SysMessage( GetDictionaryEntry( 6584, pSocket.language )); // No matching scrolls were found in this book for that recipe.
		return;
	}

	pItem.Refresh();

	// Clear the temp tag
	pUser.SetTempTag( "RecipeBookPriceRID", null );

	// Name for feedback – pull from first matching scroll
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

	var setPriceMsg = GetDictionaryEntry( 6586, pSocket.language ); // "You set the price for '%s' to %s gold."
	setPriceMsg = setPriceMsg.replace( "%s", name );
	setPriceMsg = setPriceMsg.replace( "%s", price.toString() );
	pSocket.SysMessage( setPriceMsg );
	recipeBookGump( pUser, pItem );
}

function fromBookToChar( book, recipeID, pUser )
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

function checkPlayerVendor( book )
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

function payFromBackpackOrBank( pUser, amount )
{
	amount |= 0;
	if( amount <= 0 )
		return true;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	if( pUser.ResourceCount( 0x0EED, 0 ) >= amount )
	{
		pUser.UseResource( amount, 0x0EED );
		var payGold = GetDictionaryEntry( 6587, pSocket.language ); // %s gold has been paid from your backpack.
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
			var payBank = GetDictionaryEntry( 6588, pSocket.language ); // %s gold has been paid from your bank account.
			pSocket.SysMessage( payBank.replace( /%s/gi, amount ));
			return true;
		}
	}
	return false;
}

function getSkillName( skillID )
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

function getBookFilterValues( book )
{
	return {
		skill: book.GetTag( "rb_f_skill" ),
		exp:   book.GetTag( "rb_f_exp" ),
		amt:   book.GetTag( "rb_f_amt" )
	};
}

function matchesSkillFilter( row, skillFilter )
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

function matchesExpansionFilter( row, expFilter )
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

function matchesAmountFilter( row, amtFilter )
{
	switch( amtFilter )
	{
		case 0: return true;          // All
		case 1: return row.amount > 0; // Owned
		case 2: return row.amount <= 0; // Unknown (will generally be none in this design)
	}
	return true;
}

function getFilterStatusText( book )
{
	var filter = getBookFilterValues( book );

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

function buildRecipeRows( book )
{
	var rows = [];
	if( !ValidateObject( book ) || !book.isItem )
		return rows;

	var filterVals = getBookFilterValues( book );
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
				craft: getSkillName(sID),
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
		if( !matchesSkillFilter( r, skillFilter ))
			return false;
		if( !matchesExpansionFilter( r, expFilter ))
			return false;
		if( !matchesAmountFilter( r, amtFilter ))
			return false;
		return true;
	});

	rows.sort( function( a, b ) { return a.id - b.id; } );

	return rows;
}