/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_vendors.js
// UOX3 Faction System - phase 3 faction silver vendors and equipment
// Script ID: 8504
// =============================================================================

const vendorMaxSilver = parseInt( GetServerSetting( "FACTIONMAXSILVER" ), 10 );
const vendorFactionTownScriptId = 8509;
const vendorPlayerDataScriptId = 8513;

// Stock fields:
// [ displayName, createMode, createValue, silverCost, amount, color ]
// createMode: "blank" uses CreateBlankItem, "section" uses CreateDFNItem
const vendorStock = {
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

	let factionKey = TriggerEvent( vendorPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( VendorIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function VendorGetSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	let silver = TriggerEvent( vendorPlayerDataScriptId, "GetFactionValue", pChar, "silver", pChar.GetTag( "faction_silver" ) );
	if( silver < 0 )
		silver = 0;
	if( silver > vendorMaxSilver )
		silver = vendorMaxSilver;

	return silver;
}

function VendorSetSilver( pChar, silver )
{
	if( !ValidateObject( pChar ) )
		return;

	if( silver < 0 )
		silver = 0;
	if( silver > vendorMaxSilver )
		silver = vendorMaxSilver;

	TriggerEvent( vendorPlayerDataScriptId, "SetFactionValue", pChar, "silver", silver );
}

function VendorSpendSilver( pChar, amount )
{
	let silver = VendorGetSilver( pChar );
	if( silver < amount )
		return false;

	VendorSetSilver( pChar, silver - amount );
	return true;
}

function VendorFactionName( factionKey )
{
	if( factionKey === "TB" )
		return "True Britannians";
	if( factionKey === "COM" )
		return "Council of Mages";
	if( factionKey === "MIN" )
		return "Minax";
	if( factionKey === "SL" )
		return "Shadowlords";

	return "Unknown Faction";
}

function VendorTradeAccessError( pSock, npcVendor )
{
	if( !pSock || !ValidateObject( npcVendor ) )
		return "That faction vendor is not available.";

	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return "Only players may trade with faction vendors.";

	const vendorFaction = npcVendor.GetTag( "vendor_faction" );
	if( !VendorIsFactionValid( vendorFaction ) )
		return "This faction vendor is not configured.";

	if( !TriggerEvent( vendorFactionTownScriptId, "TownIsObjectInControlledTownForFaction", npcVendor, vendorFaction ) )
		return "My faction does not control this town.";

	let playerFaction = VendorGetFaction( pUser );
	if( playerFaction === "" )
		return "Only " + VendorFactionName( vendorFaction ) + " members may trade with me.";
	if( playerFaction !== vendorFaction )
		return "I do not trade with enemies of " + VendorFactionName( vendorFaction ) + ".";

	return "";
}

function VendorCanTrade( pSock, npcVendor )
{
	let accessError = VendorTradeAccessError( pSock, npcVendor );
	if( accessError === "" )
		return true;

	if( pSock && ValidateObject( pSock.currentChar ) )
		pSock.currentChar.SysMessage( accessError );
	if( ValidateObject( npcVendor ) )
		npcVendor.TextMessage( accessError );

	return false;
}

function VendorGetStock( vendorType )
{
	if( vendorType === "" || vendorType == 0 )
		vendorType = "REAGENT";

	vendorType = vendorType.toUpperCase();
	if( !vendorStock[vendorType] )
		vendorType = "REAGENT";

	return vendorStock[vendorType];
}

function VendorCreateItem( pSock, pUser, itemData, factionKey )
{
	const itemName = itemData[0];
	let createMode = itemData[1];
	let createValue = itemData[2];
	const amount = itemData[4];
	const color = itemData[5];
	let newItem = null;

	if( createMode === "section" )
	{
		const sectionId = createValue.replace( "{FACTION}", factionKey );
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

	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return false;

	if( !VendorCanTrade( pSock, npcVendor ) )
		return false;

	ShowFactionVendorGump( pSock, pUser, npcVendor );
	return false;
}

function onSell( pSock, npcVendor )
{
	return VendorCanTrade( pSock, npcVendor );
}

function onBuyFromVendor( pSock, npcVendor, itemBought, itemAmount )
{
	return VendorCanTrade( pSock, npcVendor );
}

function onSellToVendor( pSock, npcVendor, itemSold, itemAmount )
{
	return VendorCanTrade( pSock, npcVendor );
}

function ShowFactionVendorGump( pSock, pUser, npcVendor )
{
	let vendorType = npcVendor.GetTag( "vendor_type" );
	if( vendorType === "" || vendorType == 0 )
		vendorType = "REAGENT";

	vendorType = vendorType.toUpperCase();
	const stock = VendorGetStock( vendorType );
	let silver = VendorGetSilver( pUser );
	const height = 120 + ( stock.length * 32 );

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 460, height, 9200 );
	myGump.AddHTMLGump( 15, 15, 430, 25, 0, 0, "<CENTER><b>Faction Silver Vendor</b></CENTER>" );
	myGump.AddHTMLGump( 20, 45, 390, 20, 0, 0, "Vendor Type: " + vendorType );
	myGump.AddHTMLGump( 20, 68, 390, 20, 0, 0, "Your Silver: " + silver + " / " + vendorMaxSilver );

	let y = 98;
	for( let stockIndex = 0; stockIndex < stock.length; stockIndex++ )
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

	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let vendorType = pSock.xText;
	const stock = VendorGetStock( vendorType );
	let stockIndex = pButton - 1;
	if( stockIndex < 0 || stockIndex >= stock.length )
		return;

	let playerFaction = VendorGetFaction( pUser );
	if( playerFaction === "" )
	{
		pUser.SysMessage( "Only faction members may spend faction silver." );
		return;
	}

	const vendorSerial = pSock.tempInt;
	const npcVendor = CalcCharFromSer( vendorSerial );
	if( !ValidateObject( npcVendor ) )
	{
		pUser.SysMessage( "That vendor is no longer available." );
		return;
	}

	if( !VendorCanTrade( pSock, npcVendor ) )
	{
		return;
	}

	const itemData = stock[stockIndex];
	const silverCost = itemData[3];

	if( !VendorSpendSilver( pUser, silverCost ) )
	{
		pUser.SysMessage( "You do not have enough faction silver." );
		return;
	}

	let newItem = VendorCreateItem( pSock, pUser, itemData, playerFaction );
	if( !ValidateObject( newItem ) )
	{
		VendorSetSilver( pUser, VendorGetSilver( pUser ) + silverCost );
		pUser.SysMessage( "That item could not be created. Your silver was refunded." );
		return;
	}

	pUser.SysMessage( "You bought " + itemData[0] + " for " + silverCost + " silver." );

	if( ValidateObject( npcVendor ) )
		ShowFactionVendorGump( pSock, pUser, npcVendor );
}
