/// <reference path="../../definitions.d.ts" />
// @ts-check

var HousePlacementCategories = [
	{
		catID: 1,
		name: "Classic Houses",
		minEra: "t2a",
		entries: [
			{ houseID: 1, multiID: 0x4068, name: "Small brick house", storage: 500, lockdowns: 425, cost: 43800, Era: "t2a" },
			{ houseID: 2, multiID: 0x4064, name: "Stone and plaster house",  storage: 500, lockdowns: 425, cost: 43800, Era: "t2a" },
			{ houseID: 3, multiID: 0x4066, name: "Field stone house",  storage: 500, lockdowns: 425, cost: 43800, Era: "t2a" },
			{ houseID: 4, multiID: 0x406A, name: "Wooden house",  storage: 500, lockdowns: 425, cost: 43800, Era: "t2a" },
			{ houseID: 5, multiID: 0x406C, name: "Wood and plaster house",  storage: 500, lockdowns: 425, cost: 43800, Era: "t2a" },
			{ houseID: 6, multiID: 0x406D, name: "Thatched-roof cottage",  storage: 500, lockdowns: 425, cost: 43800, Era: "t2a" },
			{ houseID: 7, multiID: 0x4070, name: "Blue Tent",  storage: 0, lockdowns: 0, cost: 22800, Era: "t2a" },
			{ houseID: 8, multiID: 0x4072, name: "green tent",  storage: 0, lockdowns: 0, cost: 22800, Era: "t2a" },
			{ houseID: 9, multiID: 0x4074, name: "Brick house",  storage: 500, lockdowns: 1100, cost: 144500, Era: "t2a" },
			{ houseID: 10, multiID: 0x4076, name: "Two-story wood and plaster house",  storage: 500, lockdowns: 1370, cost: 192400, Era: "t2a" },
			{ houseID: 11, multiID: 0x4078, name: "Two-story stone and plaster house",  storage: 500, lockdowns: 1370, cost: 192400, Era: "t2a" },
			{ houseID: 12, multiID: 0x407A, name: "Tower",  storage: 500, lockdowns: 2119, cost: 433200, Era: "t2a" },
			{ houseID: 13, multiID: 0x407C, name: "Keep",  storage: 500, lockdowns: 2625, cost: 665200, Era: "t2a" },
			{ houseID: 14, multiID: 0x407E, name: "Castle",  storage: 500, lockdowns: 4076, cost: 1022800, Era: "t2a" },
			{ houseID: 15, multiID: 0x408C, name: "Large house with patio",  storage: 500, lockdowns: 1100, cost: 152800, Era: "t2a" },
			{ houseID: 16, multiID: 0x408C, name: "Large Smith with patio",  storage: 500, lockdowns: 1100, cost: 152800, Era: "t2a" },
			{ houseID: 17, multiID: 0x4BB8, name: "Farmers cabin",  storage: 500, lockdowns: 500, cost: 55000, Era: "t2a" },
			{ houseID: 26, multiID: 0x4098, name: "Small stone tower",  storage: 500, lockdowns: 580, cost: 88000, Era: "uor" },
			{ houseID: 27, multiID: 0x40A2, name: "Small marble workshop",  storage: 500, lockdowns: 425, cost: 63000, Era: "uor" },
			{ houseID: 28, multiID: 0x40A0, name: "Small stone workshop",  storage: 500, lockdowns: 425, cost: 60000, Era: "uor" },
			{ houseID: 29, multiID: 0x409E, name: "Two-story villa",  storage: 500, lockdowns: 1100, cost: 136000, Era: "uor" },
			{ houseID: 30, multiID: 0x409C, name: "Sandstone house with patio",  storage: 500, lockdowns: 850, cost: 90000, Era: "uor" },
			{ houseID: 31, multiID: 0x409A, name: "Two-story log cabin",  storage: 500, lockdowns: 1100, cost: 97000, Era: "uor" },
			{ houseID: 32, multiID: 0x4096, name: "Marble house with patio",  storage: 500, lockdowns: 1370, cost: 192000, Era: "uor" },
		]
	},
	{
		catID: 2,
		name: "2-Story Customizable Houses",
		minEra: "aos",
		entries: [
			// Replace with your plot houseIDs
			{ houseID: 100, name: "7x7 2-Story Plot" }
		]
	},
	{
		catID: 2,
		name: "2-Story Customizable Houses",
		minEra: "aos",
		entries: [
			// Replace with your plot houseIDs
			{ houseID: 100, name: "7x7 2-Story Plot" }
		]
	},
	{
		catID: 3,
		name: "3-Story Customizable Houses",
		minEra: "aos",
		entries: [
			// Replace with your plot houseIDs
			{ houseID: 200, name: "9x14 3-Story Plot" }
		]
	},
	{
		catID: 4,
		name: "Custom House Contest",
		minEra: "hs",
		entries: [
			// Replace with your contest houseIDs
			{ houseID: 300, name: "Trinsic Keep" }
		]
	}
];

/** @type { ( pUser: Character, iUsed: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	if( !CheckToolinPack( pUser, iUsed ))
	{
		pUser.socket.SysMessage( "That must be in your pack for you to use it." );
		return false;
	}

	if( !pUser.socket )
		return false;

	HousePlacementMenu( pUser.socket );
	return false;
}

/** @type { ( pSocket: Socket ) => void } */
function HousePlacementMenu( pSocket )
{
	pSocket.CloseGump( 5090 + 0xFFFF, 0 );

	var plaementMenu = new Gump;

	var cats = VisibleCategories();
	var catCount = cats.length|0;

	// Layout constants
	var w = 270;
	var firstRowY = 40;
	var rowH = 20;
	var footerH = 30;
	var padBottom = 10;

	var rowsH = catCount * rowH;
	var h = firstRowY + rowsH + footerH + padBottom;
	if( h < 170 )
		h = 170;

	var innerX = 10, innerY = 10, innerW = 250, innerH = h - 20;

	plaementMenu.AddBackground( 0, 0, w, h, 5054 );
	plaementMenu.AddTiledGump( innerX, innerY, innerW, innerH, 2624) ;
	plaementMenu.AddCheckerTrans( innerX, innerY, innerW, innerH );

	plaementMenu.AddText( 55, 12, 0x07FF, "HOUSE PLACEMENT TOOL" );

	// Category buttons: 10 + index (index is in *filtered* list)
	for( var i = 0; i < catCount; i++ )
	{
		var y = firstRowY + ( i * rowH );
		plaementMenu.AddButton( 10, y, 4005, 4007, 1, 0, 10 + i );
		plaementMenu.AddText( 45, y + 2, 0x0481, cats[i].name );
	}

	var closeY = h - 40;
	plaementMenu.AddButton( 10, closeY, 4017, 4019, 1, 0, 0 );
	plaementMenu.AddText( 45, closeY + 2, 0x0481, "Close" );

	// Store visible category mapping for button handling
	// (store catIDs in a temp tag string)
	var map = [];
	for( var m = 0; m < catCount; m++ )
		map.push( "" + cats[m].catID );

	var pUser = pSocket.currentChar;
	if( ValidateObject( pUser ) )
		pUser.SetTempTag( "HPT_CatMap", map.join( "," ));

	plaementMenu.Send( pSocket );
	plaementMenu.Free();
}

/** @type { ( pSocket: Socket, catID: number, page: number ) => void } */
function HouseSelectionMenu( pSocket, catID, page )
{
	pSocket.CloseGump( 5090 + 0xFFFF, 0 );

	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var cat = CategoryByID( catID );
	if( cat == null )
	{
		HousePlacementMenu( pSocket );
		return;
	}

	var list = VisibleEntries( cat );
	var total = list.length;

	if( page < 0 )
		page = 0;

	var totalPages = ( total <= 0 ? 1 : Math.floor(( total + 14 - 1 ) / 14 ));
	if( page >= totalPages )
		page = totalPages - 1;

	pUser.SetTempTag( "HPT_Cat", catID );
	pUser.SetTempTag( "HPT_Page", page );

	var houseSelection = new Gump;

	// Outer frame
	houseSelection.AddBackground( 0, 0, 530, 430, 5054 );

	// Title bar
	houseSelection.AddTiledGump( 10, 10, 500, 20, 2624 );
	houseSelection.AddCheckerTrans( 10, 10, 500, 20 );
	houseSelection.AddText( 190, 12, 0x07FF, "HOUSE PLACEMENT TOOL" );

	// Column header bar
	houseSelection.AddTiledGump( 10, 40, 500, 20, 2624 );
	houseSelection.AddCheckerTrans( 10, 40, 500, 20 );

	houseSelection.AddText( 50, 42, 0x0481, "House Description" );
	houseSelection.AddText( 275, 42, 0x0481, "Storage" );
	houseSelection.AddText( 350, 42, 0x0481, "Lockdowns" );
	houseSelection.AddText( 425, 42, 0x0481, "Cost" );

	// List area
	houseSelection.AddTiledGump( 10, 70, 500, 280, 2624 );
	houseSelection.AddCheckerTrans( 10, 70, 500, 280 );

	// Bank balance bar
	houseSelection.AddTiledGump( 10, 370, 500, 20, 2624 );
	houseSelection.AddCheckerTrans( 10, 370, 500, 20 );

	houseSelection.AddText( 10, 372, 0x0481, "Bank Balance:" );
	var balance = BankBalance( pUser );
	houseSelection.AddText( 250, 372, 0x0481, FormatCost( balance ));

	// Bottom bar
	houseSelection.AddTiledGump( 10, 400, 500, 20, 2624 );
	houseSelection.AddCheckerTrans( 10, 400, 500, 20 );

	// Close ( button 1)
	houseSelection.AddButton( 10, 400, 4017, 4019, 1, 0, 0 );
	houseSelection.AddText( 50, 402, 0x0481, "Close" );

	// Prev/Next
	if( page > 0 )
	{
		houseSelection.AddButton( 200, 400, 4014, 4016, 1, 0, 3 );
		houseSelection.AddText( 250, 402, 0x0481, "Previous" );
	}
	if( page < ( totalPages - 1 ))
	{
		houseSelection.AddText( 400, 402, 0x0481, "Next" );
		houseSelection.AddButton( 480, 400, 4005, 4007, 1, 0, 4 );
	}

	// Render rows
	var start = page * 14;
	var end = Math.min( start + 14, total );

	var row = 0;
	for(  var i = start; i < end; i++ )
	{
		var e = list[i];
		var y = 70 + ( row * 20 );

		// Select button: 1000 + index in this category
		houseSelection.AddButton( 10, y + 2, 4005, 4007, 1, 0, 1000 + i );

		// Columns
		var nm = e.name || ( "House " + e.houseID );
		if( nm.length > 40 )
			nm = nm.substr( 0, 40 );

		var storage = ( e.storage|0 );
		var lockdowns = ( e.lockdowns|0 );
		var cost = ( e.cost|0 );

		houseSelection.AddText( 50,  y, 0x0481, nm );
		houseSelection.AddText( 275, y, 0x0481, "" + storage );
		houseSelection.AddText( 350, y, 0x0481, "" + lockdowns );
		houseSelection.AddText( 425, y, 0x0481, FormatCost( cost ));

		row++;
	}

	houseSelection.Send( pSocket );
	houseSelection.Free();
}

/** @type { ( pSocket: Socket, pButton: number, gumpData: any ) => void } */
function onGumpPress( pSocket, pButton, gumpData )
{
	if( !pSocket )
		return;

	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Close / client closed
	if( pButton == 0 )
	{
		pSocket.CloseGump( 5090 + 0xFFFF, 0 );
		return;
	}

	// Category selection: 10 + index
	if( pButton >= 10 && pButton < 200 ) // safe upper bound
	{
		var idx = pButton- 10;

		var raw = pUser.GetTempTag( "HPT_CatMap" );
		var arr = raw.split( "," );
		if( idx >= 0 && idx < arr.length )
		{
			var catID = parseInt( arr[idx], 10 );
			if( !isNaN( catID ))
			{
				HouseSelectionMenu( pSocket, catID, 0 );
				return;
			}
		}
	}

	// Back to categories
	if( pButton == 2 )
	{
		HousePlacementMenu( pSocket );
		return;
	}

	// Paging
	if( pButton == 3 || pButton == 4 )
	{
		var catNow = pUser.GetTempTag( "HPT_Cat" );
		var pageNow = pUser.GetTempTag( "HPT_Page" );

		if( (pButton|0) == 3 ) pageNow--;
		if( (pButton|0) == 4 ) pageNow++;

		HouseSelectionMenu(pSocket, catNow, pageNow);
		return;
	}

	// Selection (1000 + index within category list)
	if( pButton >= 1000 )
	{
		var catID = pUser.GetTempTag( "HPT_Cat" );
		var cat = CategoryByID(catID);

		if( cat == null )
		{
			HousePlacementMenu(pSocket);
			return;
		}
		var list = VisibleEntries( cat );
		var idxSel = pButton - 1000;
		if( idxSel < 0 || idxSel >= list.length )
		{
			HouseSelectionMenu( pSocket, catID, pUser.GetTempTag( "HPT_Page" ));
			return;
		}

		var entry = list[idxSel];

		// Store selected houseID in socket.tempInt, like your command does
		pSocket.tempInt = entry.houseID;
		pUser.SetTempTag( "HPT_SelectedIndex", idxSel );
		pUser.SetTempTag( "HPT_SelectedCat", catID );

		// Target placement location, using packet coords in callback
		pSocket.CustomTarget( 0, "Select location for house:" );

		// Show preview of house after CustomTarget
        SendHousePreview( pSocket, entry.multiID );
		return;
	}
}

/** @type { ( socket: Socket, ourObj: any ) => void } */
function onCallback0( socket, ourObj )
{
	var cancelCheck = socket.GetByte( 11 );
	if( cancelCheck == 255 )
	{
		return;
	}

	var mChar = socket.currentChar;
	if( !ValidateObject( mChar ))
	{
		return;
	}

	var houseID = socket.tempInt;
	if( houseID <= 0 )
	{
		socket.SysMessage( "House placement failed: no house was selected." );
		return;
	}

	var entry = EntryFromSocket( socket );
	if( entry == null )
	{
		socket.SysMessage( "House placement failed: unable to find selected house entry." );
		return;
	}

	var x = socket.GetWord( 11 );
	var y = socket.GetWord( 13 );
	var z = socket.GetSByte( 16 );

	var strangeByte = socket.GetWord( 1 );
	if(( strangeByte == 0 && ourObj && ourObj.isItem ) || ( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 ))
	{
		z += GetTileHeight( socket.GetWord( 17 ));
	}

	var cost = entry.cost|0;
	if( cost > 0 )
	{
		if( !mChar.isGM && !PayGold( mChar, cost ))
		{
			socket.SysMessage( "But thou hast not the funds in thy bank account!" );
			HouseSelectionMenu( socket, mChar.GetTempTag( "HPT_Cat" ), mChar.GetTempTag( "HPT_Page" ));
			return;
		}
	}

	var newMulti = CreateHouse( houseID, x, y, z, mChar.worldnumber, mChar.instanceID, 0, false );

	if( !ValidateObject( newMulti ))
	{
		socket.SysMessage( "House placement failed: CreateHouse returned no multi." );
		HouseSelectionMenu( socket, mChar.GetTempTag( "HPT_Cat" ), mChar.GetTempTag( "HPT_Page" ));
		return;
	}

	socket.tempInt = 0;
	socket.SysMessage( "House created." );
	socket.CloseGump( 5090 + 0xFFFF, 0 );
}

/** @type { ( pUser: Character, cost: number ) => boolean } */
function PayGold( pUser, cost )
{
	var pSock = pUser.socket;

	// 1) Backpack first
	if( pUser.ResourceCount(0x0EED, 0) >= cost )
	{
		pUser.UseResource( cost, 0x0EED );
		return true;
	}

	// 2) Then bank box
	var bankBox = pUser.FindItemLayer( 29 );
	if( ValidateObject( bankBox ))
	{
		var it;
		for( it = bankBox.FirstItem(); !bankBox.FinishedItems(); it = bankBox.NextItem() )
		{
			if( it != null && it.id == 0x0EED )
			{
				if( it.amount >= cost )
				{
					// This is the important part: remove from bank container
					bankBox.UseResource( cost, 0x0EED, 0 );
					return true;
				}
			}
		}
	}

	return false;
}

/** @type { ( cost: number ) => string } */
function FormatCost( cost )
{
	var s = "" + ( cost|0 );
	var out = "";
	var cnt = 0;

	for( var i = s.length - 1; i >= 0; i-- )
	{
		out = s.charAt( i ) + out;
		cnt++;
		if( cnt == 3 && i > 0 )
		{
			out = "," + out;
			cnt = 0;
		}
	}
	return out;
}

/** @type { ( bankBox: Item ) => number } */
function CountGoldInBankBox( bankBox )
{
	if( !ValidateObject( bankBox ))
		return 0;

	var gold = 0;
	var it;
	for( it = bankBox.FirstItem(); !bankBox.FinishedItems(); it = bankBox.NextItem() )
	{
		if( it != null && it.id == 0x0EED )
			gold += it.amount;
	}
	return gold|0;
}

/** @type { ( pUser: Character ) => number } */
function BankBalance( pUser )
{
	var bankBox = pUser.FindItemLayer( 29 );
	return CountGoldInBankBox( bankBox );
}

/** @type { ( socket: Socket ) => any } */
function EntryFromSocket( socket )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
	{
		return null;
	}

	var catID = pUser.GetTempTag( "HPT_SelectedCat" );
	var idx = pUser.GetTempTag( "HPT_SelectedIndex" );

	var cat = CategoryByID( catID );
	if( cat == null )
	{
		return null;
	}

	var list = VisibleEntries( cat );
	if( idx < 0 || idx >= list.length )
	{
		return null;
	}

	return list[idx];
}

/** @type { () => any[] } */
function VisibleCategories()
{
	var out = [];
	for( var i = 0; i < HousePlacementCategories.length; i++ )
	{
		var cat = HousePlacementCategories[i];
		if( !EraOK( cat ))
			continue;

		// Only include category if it has at least 1 visible entry
		var visEntries = VisibleEntries( cat );
		if( visEntries.length > 0 )
			out.push( cat );
	}
	return out;
}

/** @type { ( cat: any ) => any[] } */
function VisibleEntries( cat )
{
	var out = [];
	if( !cat || !cat.entries )
		return out;

	for( var i = 0; i < cat.entries.length; i++ )
	{
		var era = cat.entries[i];

		// Entry can be filtered by its own era, AND the category era (already checked)
		if( EraOK( era ))
			out.push( era );
	}
	return out;
}

/** @type { ( pUser: Character, iUsed: Item ) => boolean } */
function CheckToolinPack( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;
	if( !ValidateObject( pUser.pack ))
		return false;

	return ( iUsed.container == pUser.pack );
}

/** @type { ( catID: number ) => any } */
function CategoryByID( catID )
{
	for( var i = 0; i < HousePlacementCategories.length; i++ )
	{
		if(( HousePlacementCategories[i].catID ) == catID )
			return HousePlacementCategories[i];
	}
	return null;
}

/** @type { ( obj: any ) => boolean } */
function EraOK( obj )
{
	var coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));

	if( !obj )
		return true;

	// Support "Era": "t2a" shorthand
	if( obj.Era && !obj.minEra )
		obj.minEra = obj.Era;

	if( obj.minEra && coreShardEra < EraStringToNum( obj.minEra ))
		return false;

	if( obj.maxEra && coreShardEra > EraStringToNum( obj.maxEra ))
		return false;

	return true;
}

/** @type { ( pSocket: Socket, multiID: number ) => void } */
function SendHousePreview( pSocket, multiID )
{
	if( !pSocket || !multiID )
	{
		return;
	}

	// Calculate serial to send with multi-placement target cursor
	// [0, 1, targType (0 = hardcoded, 1 = custom target), callbackID]
	var serToSend = [0, 1, 1, 0].reduce(function(acc, val) { 
		return (acc << 8) | val; 
	}) >>> 0;
	var toSend = new Packet;

	var extendedPacket = ( pSocket.clientType >= 10 && pSocket.clientMajorVer >= 7 && pSocket.clientSubVer >= 9 );
	toSend.ReserveSize(( extendedPacket ? 30 : 26 ));

	toSend.ReserveSize( 30 );
	toSend.WriteByte( 0, 0x99 ); // Packet
	toSend.WriteByte( 1, 0x01 ); // 0x01 = from server, 0x00 = from client
	toSend.WriteLong( 2, serToSend ); // Serial of "deed"
	toSend.WriteByte( 6, 0 ); // Cursor flags

	for( var byteIndex = 7; byteIndex < 18; byteIndex++ )
	{
		toSend.WriteByte( byteIndex, 0 );
	}

	toSend.WriteShort( 18, multiID - 0x4000 ); // Multi ID from house.dfn entry minus 0x4000.
    toSend.WriteShort( 20, 0 ); // xOffset
	toSend.WriteShort( 22, 0 ); // YOffset
    toSend.WriteShort( 24, 0 ); // zOffset
	if( extendedPacket )
	{
		toSend.WriteLong( 26, 0 ); // hue
	}

	pSocket.Send( toSend );
	toSend.Free();
}