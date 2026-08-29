// =============================================================================
// factions_townstone.js
// UOX3 Faction System - townstone faction control panel
// Script ID: 8510
// =============================================================================

var TownstoneTownScriptId = 8509;
var TownstonePlayerDataScriptId = 8513;
var TownstoneGuardCost = 250;
var TownstoneVendorCost = 500;

var TownstoneVendorTypes = [
	[ 201, "REAGENT", "Reagent Vendor" ],
	[ 202, "BOARD", "Board Vendor" ],
	[ 203, "BOTTLE", "Bottle Vendor" ],
	[ 204, "EQUIPMENT", "Equipment Vendor" ],
	[ 205, "HORSE", "Horse Vendor" ]
];

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

function TownstoneHasRole( pUser, roleName, factionKey )
{
	if( !ValidateObject( pUser ) || !TownstoneIsFactionValid( factionKey ) )
		return false;

	var factionData = TriggerEvent( TownstonePlayerDataScriptId, "ReadFactionPlayerData", pUser );
	if( factionData.faction !== factionKey )
		return false;
	if( factionData.role !== roleName )
		return false;

	return factionData.roleFaction === factionKey;
}

function TownstoneCanUseRole( pUser, roleName, factionKey )
{
	return ( TownstoneIsStaff( pUser ) || TownstoneHasRole( pUser, roleName, factionKey ) );
}

function TownstoneSetContext( pUser, townName, townRegionId )
{
	pUser.SetTempTag( "faction_townstone_town", townName );
	pUser.SetTempTag( "faction_townstone_region", townRegionId );
}

function TownstoneCurrentTown( pUser )
{
	var townName = pUser.GetTempTag( "faction_townstone_town" );
	if( townName === "" )
		townName = TriggerEvent( TownstoneTownScriptId, "TownNameForObject", pUser );

	return townName;
}

function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return false;

	var townRegionId = parseInt( iUsed.GetTag( "faction_town_region" ), 10 );
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
	var townName = TownstoneCurrentTown( pUser );
	var townOwner = TriggerEvent( TownstoneTownScriptId, "TownGetOwner", townName );
	var playerTownOwner = TriggerEvent( TownstoneTownScriptId, "TownOwnerForObject", pUser );
	if( townOwner !== factionKey || playerTownOwner !== factionKey )
	{
		pSock.SysMessage( "You must be in a town controlled by " + TownstoneFactionName( factionKey ) + "." );
		return false;
	}

	var limitError = TriggerEvent( TownstoneTownScriptId, "TownCanPlaceFactionNpc", townName, factionKey, npcType );
	if( limitError !== "" )
	{
		pSock.SysMessage( limitError );
		return false;
	}

	var cost = npcType === "guard" ? TownstoneGuardCost : TownstoneVendorCost;
	if( !TownstoneIsStaff( pUser ) )
	{
		var treasury = TriggerEvent( TownstoneTownScriptId, "TownGetTreasury", townName );
		if( treasury < cost )
		{
			pSock.SysMessage( "The " + townName + " treasury needs " + cost + " silver. Current treasury: " + treasury + "." );
			return false;
		}

		if( !TriggerEvent( TownstoneTownScriptId, "TownSpendTreasury", townName, cost ) )
		{
			pSock.SysMessage( "Unable to spend from the town treasury." );
			return false;
		}
	}

	var sectionId = "";
	if( npcType === "guard" )
		sectionId = "FACTION_GUARD_" + factionKey;
	else
		sectionId = "FACTION_" + vendorType + "_VENDOR_" + factionKey;

	var newNpc = SpawnNPC( sectionId, pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( !ValidateObject( newNpc ) )
	{
		if( !TownstoneIsStaff( pUser ) )
			TriggerEvent( TownstoneTownScriptId, "TownAddTreasury", townName, cost );
		pSock.SysMessage( "Unable to create faction NPC." );
		return false;
	}

	TriggerEvent( TownstoneTownScriptId, "TownTagFactionNpc", newNpc, factionKey, townName, npcType, vendorType );
	return true;
}

function OpenFactionTownstone( pSock, pUser, townRegionId )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	var townName = TriggerEvent( TownstoneTownScriptId, "TownNameForRegion", townRegionId );
	if( townName === "" )
	{
		pSock.SysMessage( "This town is not configured for factions." );
		return false;
	}

	var townRegion = GetTownRegion( townRegionId );
	var displayName = townName;
	if( townRegion != null && typeof townRegion.id != "undefined" )
		displayName = townRegion.name;

	var ownerFaction = TriggerEvent( TownstoneTownScriptId, "TownGetOwner", townName );
	var managedNpcCount = TriggerEvent( TownstoneTownScriptId, "TownCountFactionNpcs", townName, "" );
	var npcLimitSummary = TriggerEvent( TownstoneTownScriptId, "TownNpcLimitSummary", townName, ownerFaction );
	var treasury = TriggerEvent( TownstoneTownScriptId, "TownGetTreasury", townName );
	var taxRate = TriggerEvent( TownstoneTownScriptId, "TownGetTaxRate", townName );
	var canPlaceGuard = TownstoneCanUseRole( pUser, "sheriff", ownerFaction );
	var canPlaceVendor = TownstoneCanUseRole( pUser, "finance", ownerFaction );
	var isStaff = TownstoneIsStaff( pUser );
	var y = 0;

	TownstoneSetContext( pUser, townName, townRegionId );

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 460, 420, 9200 );
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
		myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Place Faction Guard (" + TownstoneGuardCost + ")" );
		y += 25;
	}

	if( canPlaceVendor )
	{
		for( var vendorIndex = 0; vendorIndex < TownstoneVendorTypes.length; vendorIndex++ )
		{
			myGump.AddButton( 25, y, 0xFA5, 1, 0, TownstoneVendorTypes[vendorIndex][0] );
			myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Place " + TownstoneVendorTypes[vendorIndex][2] + " (" + TownstoneVendorCost + ")" );
			y += 25;
		}
	}

	if( isStaff )
	{
		myGump.AddButton( 25, y, 0xFA5, 1, 0, 901 );
		myGump.AddHTMLGump( 65, y, 320, 20, 0, 0, "Clear Managed Faction NPCs" );
	}

	myGump.AddButton( 25, 380, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 380, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var townName = TownstoneCurrentTown( pUser );
	var townRegionId = Number( pUser.GetTempTag( "faction_townstone_region" ) );
	var ownerFaction = TriggerEvent( TownstoneTownScriptId, "TownGetOwner", townName );
	if( !TownstoneIsFactionValid( ownerFaction ) )
	{
		pSock.SysMessage( "This town is not currently faction-controlled." );
		return;
	}

	if( pButton == 0 )
		return;

	if( pButton == 900 )
	{
		TriggerEvent( TownstoneTownScriptId, "ShowTownNpcStatus", pSock, townName );
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

		var removedCount = TriggerEvent( TownstoneTownScriptId, "TownClearFactionNpcs", townName, "" );
		pSock.SysMessage( "Removed " + removedCount + " managed faction NPC(s)." );
		OpenFactionTownstone( pSock, pUser, townRegionId );
		return;
	}

	if( pButton == 101 )
	{
		if( !TownstoneCanUseRole( pUser, "sheriff", ownerFaction ) )
		{
			pSock.SysMessage( "Only this faction's Sheriff may place guards." );
			return;
		}

		if( TownstoneSpawnNpc( pSock, pUser, ownerFaction, "guard", "" ) )
			pSock.SysMessage( "Faction guard created." );
		OpenFactionTownstone( pSock, pUser, townRegionId );
		return;
	}

	for( var vendorIndex = 0; vendorIndex < TownstoneVendorTypes.length; vendorIndex++ )
	{
		if( pButton == TownstoneVendorTypes[vendorIndex][0] )
		{
			if( !TownstoneCanUseRole( pUser, "finance", ownerFaction ) )
			{
				pSock.SysMessage( "Only this faction's Finance Minister may place vendors." );
				return;
			}

			if( TownstoneSpawnNpc( pSock, pUser, ownerFaction, "vendor", TownstoneVendorTypes[vendorIndex][1] ) )
				pSock.SysMessage( TownstoneVendorTypes[vendorIndex][2] + " created." );
			OpenFactionTownstone( pSock, pUser, townRegionId );
			return;
		}
	}
}
