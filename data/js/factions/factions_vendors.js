// =============================================================================
// factions_vendors.js
// UOX3 Faction System - phase 3 faction silver vendors and equipment
// Script ID: 8504
// =============================================================================

var VendorMaxSilver = 100000;

// Stock fields:
// [ displayName, createMode, createValue, silverCost, amount, color ]
// createMode: "blank" uses CreateBlankItem, "section" uses CreateDFNItem
var VendorStock = {
	REAGENT: [
		[ "Black Pearl x10", "blank", 0x0F7A, 5, 10, 0 ],
		[ "Bloodmoss x10", "blank", 0x0F7B, 5, 10, 0 ],
		[ "Mandrake Root x10", "blank", 0x0F86, 5, 10, 0 ],
		[ "Nightshade x10", "blank", 0x0F88, 5, 10, 0 ],
		[ "Garlic x10", "blank", 0x0F84, 3, 10, 0 ],
		[ "Ginseng x10", "blank", 0x0F85, 3, 10, 0 ],
		[ "Spider Silk x10", "blank", 0x0F8D, 5, 10, 0 ],
		[ "Sulfurous Ash x10", "blank", 0x0F8C, 5, 10, 0 ]
	],
	BOARD: [
		[ "Boards x50", "blank", 0x1BD7, 30, 50, 0 ]
	],
	BOTTLE: [
		[ "Empty Bottles x10", "blank", 0x0F0E, 10, 10, 0 ],
		[ "Lesser Cure Potion", "blank", 0x0F07, 25, 1, 0 ],
		[ "Lesser Heal Potion", "blank", 0x0F0C, 20, 1, 0 ]
	],
	ORE: [
		[ "Iron Ingots x10", "blank", 0x1BF2, 10, 10, 0 ]
	],
	EQUIPMENT: [
		[ "Faction Robe", "section", "FACTION_ROBE_{FACTION}", 75, 1, 0 ],
		[ "Faction Shield", "section", "FACTION_SHIELD_{FACTION}", 100, 1, 0 ]
	],
	HORSE: [
		[ "Faction War Horse Deed", "section", "FACTION_WAR_HORSE_DEED", 500, 1, 0 ]
	]
};

function VendorIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function VendorGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	var factionKey = pChar.GetTag( "faction" );
	if( VendorIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function VendorGetSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	var silver = pChar.GetTag( "faction_silver" );
	if( silver < 0 )
		silver = 0;
	if( silver > VendorMaxSilver )
		silver = VendorMaxSilver;

	return silver;
}

function VendorSetSilver( pChar, silver )
{
	if( !ValidateObject( pChar ) )
		return;

	if( silver < 0 )
		silver = 0;
	if( silver > VendorMaxSilver )
		silver = VendorMaxSilver;

	pChar.SetTag( "faction_silver", silver );
}

function VendorSpendSilver( pChar, amount )
{
	var silver = VendorGetSilver( pChar );
	if( silver < amount )
		return false;

	VendorSetSilver( pChar, silver - amount );
	return true;
}

function VendorGetStock( vendorType )
{
	if( vendorType === "" || vendorType == 0 )
		vendorType = "REAGENT";

	vendorType = vendorType.toUpperCase();
	if( !VendorStock[vendorType] )
		vendorType = "REAGENT";

	return VendorStock[vendorType];
}

function VendorCreateItem( pSock, pUser, itemData, factionKey )
{
	var itemName = itemData[0];
	var createMode = itemData[1];
	var createValue = itemData[2];
	var amount = itemData[4];
	var color = itemData[5];
	var newItem = null;

	if( createMode === "section" )
	{
		var sectionId = createValue.replace( "{FACTION}", factionKey );
		newItem = CreateDFNItem( pSock, pUser, sectionId, amount, "ITEM", true );
	}
	else
	{
		newItem = CreateBlankItem( pSock, pUser, amount, itemName, createValue, color, "ITEM", true );
	}

	if( ValidateObject( newItem ) )
	{
		if( createMode === "section" )
		{
			newItem.SetTag( "faction_item", 1 );
			newItem.SetTag( "item_faction", factionKey );
			if( createValue === "FACTION_WAR_HORSE_DEED" )
			{
				newItem.SetTag( "faction_horse_deed", 1 );
				newItem.name = factionKey + " Faction War Horse Deed";
			}
		}
	}

	return newItem;
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

	vendorType = vendorType.toUpperCase();
	var stock = VendorGetStock( vendorType );
	var silver = VendorGetSilver( pUser );
	var height = 120 + ( stock.length * 32 );

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 460, height, 9200 );
	myGump.AddHTMLGump( 15, 15, 430, 25, 0, 0, "<CENTER><b>Faction Silver Vendor</b></CENTER>" );
	myGump.AddHTMLGump( 20, 45, 390, 20, 0, 0, "Vendor Type: " + vendorType );
	myGump.AddHTMLGump( 20, 68, 390, 20, 0, 0, "Your Silver: " + silver + " / " + VendorMaxSilver );

	var y = 98;
	for( var stockIndex = 0; stockIndex < stock.length; stockIndex++ )
	{
		myGump.AddButton( 20, y, 0xFA5, 1, 0, stockIndex + 1 );
		myGump.AddHTMLGump( 60, y, 370, 20, 0, 0, stock[stockIndex][0] + " - " + stock[stockIndex][3] + " silver" );
		y += 32;
	}

	myGump.AddButton( 20, y + 10, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 60, y + 10, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();

	pSock.xText = vendorType;
	pSock.tempInt = npcVendor.serial;
}

function onGumpPress( pSock, pButton, gumpData )
{
	if( pButton == 0 )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var vendorType = pSock.xText;
	var stock = VendorGetStock( vendorType );
	var stockIndex = pButton - 1;
	if( stockIndex < 0 || stockIndex >= stock.length )
		return;

	var playerFaction = VendorGetFaction( pUser );
	if( playerFaction === "" )
	{
		pUser.SysMessage( "Only faction members may spend faction silver." );
		return;
	}

	var itemData = stock[stockIndex];
	var silverCost = itemData[3];

	if( !VendorSpendSilver( pUser, silverCost ) )
	{
		pUser.SysMessage( "You do not have enough faction silver." );
		return;
	}

	var newItem = VendorCreateItem( pSock, pUser, itemData, playerFaction );
	if( !ValidateObject( newItem ) )
	{
		VendorSetSilver( pUser, VendorGetSilver( pUser ) + silverCost );
		pUser.SysMessage( "That item could not be created. Your silver was refunded." );
		return;
	}

	pUser.SysMessage( "You bought " + itemData[0] + " for " + silverCost + " silver." );

	var vendorSerial = pSock.tempInt;
	var npcVendor = CalcCharFromSer( vendorSerial );
	if( ValidateObject( npcVendor ) )
		ShowFactionVendorGump( pSock, pUser, npcVendor );
}
