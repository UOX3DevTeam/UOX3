/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_townstone.js
// UOX3 Faction System - townstone faction control panel
// Script ID: 8510
// =============================================================================

const townstoneTownScriptId = 8509;
const townstonePlayerDataScriptId = 8513;
const townstoneGuardCost = parseInt( GetServerSetting( "FACTIONGUARDCOST" ), 10 );
const townstoneVendorCost = parseInt( GetServerSetting( "FACTIONVENDORCOST" ), 10 );
const townstoneTaxOffsets = [ -30, -25, -20, -15, -10, -5, 0, 50, 100, 150, 200, 250, 300 ];

const townstoneVendorTypes = [
	[ 201, "REAGENT", "Reagent Vendor" ],
	[ 202, "BOARD", "Board Vendor" ],
	[ 203, "BOTTLE", "Bottle Vendor" ],
	[ 204, "ORE", "Iron Ore Vendor" ],
	[ 205, "EQUIPMENT", "Equipment Vendor" ],
	[ 206, "HORSE", "Horse Vendor" ]
];

function TownstoneVendorPurchaseCost( vendorType )
{
	vendorType = String( vendorType ).toUpperCase();
	if( vendorType === "BOARD" || vendorType === "ORE" )
		return 3000;
	return townstoneVendorCost;
}

function TownstoneFactionName( factionKey )
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

function TownstoneIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function TownstoneIsStaff( pUser )
{
	if( !ValidateObject( pUser ) )
		return false;

	return ( pUser.isGM || pUser.commandlevel >= 5 );
}

function TownstoneHasRole( pUser, roleName, factionKey, townName )
{
	if( !ValidateObject( pUser ) || !TownstoneIsFactionValid( factionKey ) )
		return false;

	const factionData = TriggerEvent( townstonePlayerDataScriptId, "ReadFactionPlayerData", pUser );
	if( factionData.faction !== factionKey )
		return false;
	if( factionData.role !== roleName )
		return false;

	if( factionData.roleFaction !== factionKey )
		return false;

	return ( roleName === "commander" || factionData.roleTown === townName );
}

function TownstoneCanUseRole( pUser, roleName, factionKey, townName )
{
	return ( TownstoneIsStaff( pUser ) || TownstoneHasRole( pUser, roleName, factionKey, townName ) );
}

function TownstoneSetContext( pUser, townName, townRegionId )
{
	pUser.SetTempTag( "faction_townstone_town", townName );
	pUser.SetTempTag( "faction_townstone_region", townRegionId );
}

function TownstoneCurrentTown( pUser )
{
	let townName = pUser.GetTempTag( "faction_townstone_town" );
	if( townName === "" )
		townName = TriggerEvent( townstoneTownScriptId, "TownNameForObject", pUser );

	return townName;
}

function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return false;

	let townRegionId = parseInt( iUsed.GetTag( "faction_town_region" ), 10 );
	if( isNaN( townRegionId ) || townRegionId <= 0 )
		townRegionId = parseInt( iUsed.GetTag( "town_region" ), 10 );
	if( isNaN( townRegionId ) || townRegionId <= 0 )
	{
		if( iUsed.region != null && typeof iUsed.region.id != "undefined" )
			townRegionId = iUsed.region.id;
	}

	if( isNaN( townRegionId ) || townRegionId <= 0 )
	{
		pUser.SysMessage( "This faction townstone is not linked to a town region." );
		return false;
	}

	if( pUser.socket == null )
		return false;

	OpenFactionTownstone( pUser.socket, pUser, townRegionId );
	return false;
}

function TownstoneSpawnNpc( pSock, pUser, factionKey, npcType, vendorType )
{
	let townName = TownstoneCurrentTown( pUser );
	const townOwner = TriggerEvent( townstoneTownScriptId, "TownGetOwner", townName );
	const playerTownOwner = TriggerEvent( townstoneTownScriptId, "TownOwnerForObject", pUser );
	if( townOwner !== factionKey || playerTownOwner !== factionKey )
	{
		pSock.SysMessage( "You must be in a town controlled by " + TownstoneFactionName( factionKey ) + "." );
		return false;
	}

	const limitError = TriggerEvent( townstoneTownScriptId, "TownCanPlaceFactionNpc", townName, factionKey, npcType );
	if( limitError !== "" )
	{
		pSock.SysMessage( limitError );
		return false;
	}

	const cost = npcType === "guard" ? townstoneGuardCost : TownstoneVendorPurchaseCost( vendorType );
	if( !TownstoneIsStaff( pUser ) )
	{
		const treasury = TriggerEvent( townstoneTownScriptId, "TownGetTreasury", townName );
		if( treasury < cost )
		{
			pSock.SysMessage( "The " + townName + " treasury needs " + cost + " silver. Current treasury: " + treasury + "." );
			return false;
		}

		if( !TriggerEvent( townstoneTownScriptId, "TownSpendTreasury", townName, cost ) )
		{
			pSock.SysMessage( "Unable to spend from the town treasury." );
			return false;
		}
	}

	let sectionId = "";
	if( npcType === "guard" )
		sectionId = "FACTION_GUARD_" + factionKey;
	else
		sectionId = "FACTION_" + vendorType + "_VENDOR_" + factionKey;

	const newNpc = SpawnNPC( sectionId, pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( !ValidateObject( newNpc ) )
	{
		if( !TownstoneIsStaff( pUser ) )
			TriggerEvent( townstoneTownScriptId, "TownAddTreasury", townName, cost );
		pSock.SysMessage( "Unable to create faction NPC." );
		return false;
	}

	TriggerEvent( townstoneTownScriptId, "TownTagFactionNpc", newNpc, factionKey, townName, npcType, vendorType );
	return true;
}

function OpenFactionTownstone( pSock, pUser, townRegionId )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	let townName = TriggerEvent( townstoneTownScriptId, "TownNameForRegion", townRegionId );
	if( townName === "" )
	{
		pSock.SysMessage( "This town is not configured for factions." );
		return false;
	}

	const townRegion = GetTownRegion( townRegionId );
	let displayName = townName;
	if( townRegion != null && typeof townRegion.id != "undefined" )
		displayName = townRegion.name;

	const ownerFaction = TriggerEvent( townstoneTownScriptId, "TownGetOwner", townName );
	const managedNpcCount = TriggerEvent( townstoneTownScriptId, "TownCountFactionNpcs", townName, "" );
	const npcLimitSummary = TriggerEvent( townstoneTownScriptId, "TownNpcLimitSummary", townName, ownerFaction );
	const treasury = TriggerEvent( townstoneTownScriptId, "TownGetTreasury", townName );
	const taxRate = TriggerEvent( townstoneTownScriptId, "TownGetTaxRate", townName );
	const canPlaceGuard = TownstoneCanUseRole( pUser, "sheriff", ownerFaction, townName );
	const canPlaceVendor = TownstoneCanUseRole( pUser, "finance", ownerFaction, townName );
	const isStaff = TownstoneIsStaff( pUser );
	let y = 0;

	TownstoneSetContext( pUser, townName, townRegionId );

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 460, 480, 9200 );
	myGump.AddHTMLGump( 15, 15, 410, 25, 0, 0, "<CENTER><b>Faction Town Control</b></CENTER>" );
	myGump.AddHTMLGump( 25, 50, 390, 20, 0, 0, "Town: " + displayName );
	myGump.AddHTMLGump( 25, 75, 390, 20, 0, 0, "Faction Owner: " + TownstoneFactionName( ownerFaction ) );
	myGump.AddHTMLGump( 25, 100, 390, 20, 0, 0, "Managed NPCs: " + managedNpcCount );
	myGump.AddHTMLGump( 25, 125, 390, 20, 0, 0, "Town Treasury: " + treasury + " silver, tax " + taxRate );
	myGump.AddHTMLGump( 25, 150, 410, 20, 0, 0, npcLimitSummary );

	myGump.AddButton( 25, 180, 0xFA5, 1, 0, 900 );
	myGump.AddHTMLGump( 65, 180, 320, 20, 0, 0, "List Managed Faction NPCs" );

	y = 210;
	if( canPlaceGuard )
	{
		myGump.AddButton( 25, y, 0xFA5, 1, 0, 101 );
		myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Place Faction Guard (" + townstoneGuardCost + ")" );
		y += 25;
	}

	if( canPlaceVendor )
	{
		myGump.AddButton( 25, y, 0xFA5, 1, 0, 102 );
		myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Set Town Tax Rate" );
		y += 25;
		for( let vendorIndex = 0; vendorIndex < townstoneVendorTypes.length; vendorIndex++ )
		{
			myGump.AddButton( 25, y, 0xFA5, 1, 0, townstoneVendorTypes[vendorIndex][0] );
			myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Place " + townstoneVendorTypes[vendorIndex][2] + " (" + TownstoneVendorPurchaseCost( townstoneVendorTypes[vendorIndex][1] ) + ")" );
			y += 25;
		}
	}

	if( isStaff )
	{
		myGump.AddButton( 25, y, 0xFA5, 1, 0, 901 );
		myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Clear Managed Faction NPCs" );
	}

	myGump.AddButton( 25, 445, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 445, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function OpenTownTaxGump( pSock, pUser, townName, townRegionId )
{
	const currentTax = TriggerEvent( townstoneTownScriptId, "TownGetTaxRate", townName );
	const remaining = TriggerEvent( townstoneTownScriptId, "TownTaxChangeRemaining", townName );
	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 390, 430, 9200 );
	myGump.AddHTMLGump( 15, 15, 360, 25, 0, 0, "<CENTER><b>" + townName + " Tax Rate</b></CENTER>" );
	myGump.AddHTMLGump( 25, 50, 340, 20, 0, 0, "Current tax offset: " + currentTax + "%" );
	if( remaining > 0 )
		myGump.AddHTMLGump( 25, 75, 340, 40, 0, 0, "Another change is available in " + Math.ceil( remaining / 3600000 ) + " hour(s)." );
	else
		myGump.AddHTMLGump( 25, 75, 340, 40, 0, 0, "Choose the town's daily income tax offset." );

	let y = 120;
	for( let taxIndex = 0; taxIndex < townstoneTaxOffsets.length; taxIndex++ )
	{
		const taxOffset = townstoneTaxOffsets[taxIndex];
		myGump.AddButton( 25, y, 0xFA5, 1, 0, 300 + taxIndex );
		myGump.AddHTMLGump( 65, y, 250, 20, 0, 0, ( taxOffset > 0 ? "+" : "" ) + taxOffset + "%" );
		y += 22;
	}
	myGump.AddButton( 25, 405, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 405, 100, 20, 0, 0, "Cancel" );
	myGump.Send( pSock );
	myGump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let townName = TownstoneCurrentTown( pUser );
	let townRegionId = Number( pUser.GetTempTag( "faction_townstone_region" ) );
	const ownerFaction = TriggerEvent( townstoneTownScriptId, "TownGetOwner", townName );
	if( !TownstoneIsFactionValid( ownerFaction ) )
	{
		pSock.SysMessage( "This town is not currently faction-controlled." );
		return;
	}

	if( pButton == 0 )
		return;

	if( pButton == 900 )
	{
		TriggerEvent( townstoneTownScriptId, "ShowTownNpcStatus", pSock, townName );
		OpenFactionTownstone( pSock, pUser, townRegionId );
		return;
	}

	if( pButton == 901 )
	{
		if( !TownstoneIsStaff( pUser ) )
		{
			pSock.SysMessage( "Only staff may clear faction NPCs from a townstone." );
			return;
		}

		const removedCount = TriggerEvent( townstoneTownScriptId, "TownClearFactionNpcs", townName, "" );
		pSock.SysMessage( "Removed " + removedCount + " managed faction NPC(s)." );
		OpenFactionTownstone( pSock, pUser, townRegionId );
		return;
	}

	if( pButton == 101 )
	{
		if( !TownstoneCanUseRole( pUser, "sheriff", ownerFaction, townName ) )
		{
			pSock.SysMessage( "Only this faction's Sheriff may place guards." );
			return;
		}

		if( TownstoneSpawnNpc( pSock, pUser, ownerFaction, "guard", "" ) )
			pSock.SysMessage( "Faction guard created." );
		OpenFactionTownstone( pSock, pUser, townRegionId );
		return;
	}

	if( pButton == 102 )
	{
		if( !TownstoneCanUseRole( pUser, "finance", ownerFaction, townName ) )
		{
			pSock.SysMessage( "Only this town's Finance Minister may set its tax rate." );
			return;
		}
		OpenTownTaxGump( pSock, pUser, townName, townRegionId );
		return;
	}

	if( pButton >= 300 && pButton < 300 + townstoneTaxOffsets.length )
	{
		if( !TownstoneCanUseRole( pUser, "finance", ownerFaction, townName ) )
		{
			pSock.SysMessage( "Only this town's Finance Minister may set its tax rate." );
			return;
		}
		const taxOffset = townstoneTaxOffsets[pButton - 300];
		if( TriggerEvent( townstoneTownScriptId, "TownSetTaxRate", townName, taxOffset, TownstoneIsStaff( pUser ) ) )
			pSock.SysMessage( townName + " tax rate changed to " + ( taxOffset > 0 ? "+" : "" ) + taxOffset + "%." );
		else
			pSock.SysMessage( TriggerEvent( townstoneTownScriptId, "TownLastError" ) );
		OpenFactionTownstone( pSock, pUser, townRegionId );
		return;
	}

	for( let vendorIndex = 0; vendorIndex < townstoneVendorTypes.length; vendorIndex++ )
	{
		if( pButton == townstoneVendorTypes[vendorIndex][0] )
		{
			if( !TownstoneCanUseRole( pUser, "finance", ownerFaction, townName ) )
			{
				pSock.SysMessage( "Only this faction's Finance Minister may place vendors." );
				return;
			}

			if( TownstoneSpawnNpc( pSock, pUser, ownerFaction, "vendor", townstoneVendorTypes[vendorIndex][1] ) )
				pSock.SysMessage( townstoneVendorTypes[vendorIndex][2] + " created." );
			OpenFactionTownstone( pSock, pUser, townRegionId );
			return;
		}
	}
}
