// =============================================================================
// factions_vendors.js
// UOX3 Faction System - faction silver vendors
// Script ID suggestion: 8504
// =============================================================================

var VendorStock = {
	REAGENT: [
		[ "Black Pearl x10", "0x0F7A", 5, 10 ],
		[ "Bloodmoss x10", "0x0F7B", 5, 10 ],
		[ "Mandrake Root x10", "0x0F86", 5, 10 ],
		[ "Nightshade x10", "0x0F88", 5, 10 ],
		[ "Garlic x10", "0x0F84", 3, 10 ],
		[ "Ginseng x10", "0x0F85", 3, 10 ],
		[ "Spider Silk x10", "0x0F8D", 5, 10 ],
		[ "Sulfurous Ash x10", "0x0F8C", 5, 10 ]
	],
	BOARD: [
		[ "Boards x50", "0x1BD7", 30, 50 ]
	],
	BOTTLE: [
		[ "Empty Bottles x10", "0x0F0E", 10, 10 ],
		[ "Lesser Cure Potion", "0x0F07", 25, 1 ],
		[ "Lesser Heal Potion", "0x0F0C", 20, 1 ]
	],
	ORE: [
		[ "Iron Ingots x10", "0x1BF2", 10, 10 ]
	]
};

function VendorGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	var factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
}

function onBuy( pSock, npcVendor )
{
	if( !pSock || !ValidateObject( npcVendor ) )
		return false;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return false;

	var vendorFaction = npcVendor.GetTag( "vendor_faction" );
	var playerFaction = VendorGetFaction( pUser );
	if( playerFaction === "" || playerFaction !== vendorFaction )
	{
		npcVendor.TextMessage( "I only trade with my own faction." );
		return false;
	}

	ShowFactionVendorGump( pSock, pUser, npcVendor );
	return false;
}

function ShowFactionVendorGump( pSock, pUser, npcVendor )
{
	var vendorType = npcVendor.GetTag( "vendor_type" );
	if( vendorType === "" || vendorType == 0 )
		vendorType = "REAGENT";

	var stock = VendorStock[vendorType];
	if( !stock )
		stock = VendorStock.REAGENT;

	var silver = pUser.GetTag( "faction_silver" );
	var height = 105 + ( stock.length * 32 );
	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, height, 9200 );
	myGump.AddHTMLGump( 15, 15, 400, 25, 0, 0, "<CENTER><b>Faction Silver Vendor</b></CENTER>" );
	myGump.AddHTMLGump( 20, 45, 390, 20, 0, 0, "Your Silver: " + silver );

	var y = 75;
	for( var stockIndex = 0; stockIndex < stock.length; stockIndex++ )
	{
		myGump.AddButton( 20, y, 0xFA5, 1, 0, stockIndex + 1 );
		myGump.AddHTMLGump( 60, y, 340, 20, 0, 0, stock[stockIndex][0] + " - " + stock[stockIndex][2] + " silver" );
		y += 32;
	}

	myGump.AddButton( 20, y + 10, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 60, y + 10, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();

	pSock.xText = vendorType;
}

function onGumpPress( pSock, pButton, gumpData )
{
	if( pButton == 0 )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var vendorType = pSock.xText;
	var stock = VendorStock[vendorType];
	if( !stock )
		stock = VendorStock.REAGENT;

	var stockIndex = pButton - 1;
	if( stockIndex < 0 || stockIndex >= stock.length )
		return;

	var itemData = stock[stockIndex];
	var silver = pUser.GetTag( "faction_silver" );
	if( silver < itemData[2] )
	{
		pUser.SysMessage( "You do not have enough faction silver." );
		return;
	}

	var newItem = CreateDFNItem( pSock, pUser, itemData[1], itemData[3], "ITEM", true );
	if( !ValidateObject( newItem ) )
	{
		pUser.SysMessage( "That item could not be created." );
		return;
	}

	pUser.SetTag( "faction_silver", silver - itemData[2] );
	pUser.SysMessage( "You bought " + itemData[0] + " for " + itemData[2] + " silver." );
}
