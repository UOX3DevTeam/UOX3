// =============================================================================
// factions_towns.js
// UOX3 Faction System - town region ownership bridge
// Script ID: 8509
// =============================================================================

var FactionTownController = null;

var FactionTownNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

var FactionTownOwnerNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

var FactionTownDefaults = {
	Britain: { region: 3, owner: "TB" },
	Trinsic: { region: 9, owner: "TB" },
	Moonglow: { region: 13, owner: "COM" },
	SkaraBrae: { region: 8, owner: "COM" },
	Yew: { region: 7, owner: "MIN" },
	Vesper: { region: 11, owner: "MIN" },
	Minoc: { region: 12, owner: "SL" },
	Cove: { region: 10, owner: "SL" }
};

var FactionTownRegions = {
	3: "Britain",
	9: "Trinsic",
	13: "Moonglow",
	8: "SkaraBrae",
	7: "Yew",
	11: "Vesper",
	12: "Minoc",
	10: "Cove"
};

var FactionTownNameAliases = {
	britain: "Britain",
	trinsic: "Trinsic",
	moonglow: "Moonglow",
	skarabrae: "SkaraBrae",
	yew: "Yew",
	vesper: "Vesper",
	minoc: "Minoc",
	cove: "Cove"
};

var FactionTownLastError = "";
var FactionTownIterateMode = "";
var FactionTownIterateTown = "";
var FactionTownIterateFaction = "";
var FactionTownIterateSocket = null;
var FactionTownTreasuryGrant = 1000;
var FactionTownDefaultTaxRate = 100;
var FactionTownTaxTimerId = 2;
var FactionTownDefaultTaxInterval = 3600000;
var FactionTownGuardLimit = 10;
var FactionTownVendorLimit = 10;
var FactionTownCountType = "";

function TownIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function TownRegionIsLoaded( townRegion )
{
	return ( townRegion != null && typeof townRegion.id != "undefined" );
}

function TownNormalizeName( townName )
{
	var cleanName = String( townName );
	cleanName = cleanName.replace( /^\s+|\s+$/g, "" );
	cleanName = cleanName.replace( /^the town of /i, "" );

	var aliasName = cleanName.toLowerCase();
	aliasName = aliasName.replace( /[^a-z]/g, "" );
	if( FactionTownNameAliases[aliasName] )
		return FactionTownNameAliases[aliasName];

	return cleanName;
}

function TownFactionName( factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return "Unknown Faction";

	return FactionTownNames[factionKey];
}

function TownOwnerName( factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return "The Town";

	return FactionTownOwnerNames[factionKey];
}

function TownGetDefault( townName )
{
	townName = TownNormalizeName( townName );
	if( FactionTownDefaults[townName] )
		return FactionTownDefaults[townName];

	return null;
}

function TownSetLastError( errorMessage )
{
	FactionTownLastError = errorMessage;
	return false;
}

function TownLastError()
{
	return FactionTownLastError;
}

function TownRegionForName( townName )
{
	var townInfo = TownGetDefault( townName );
	if( townInfo == null )
		return 0;

	return townInfo.region;
}

function TownDefaultOwner( townName )
{
	var townInfo = TownGetDefault( townName );
	if( townInfo == null )
		return "";

	return townInfo.owner;
}

function TownDisplayName( townName )
{
	townName = TownNormalizeName( townName );
	if( townName === "SkaraBrae" )
		return "Skara Brae";

	return townName;
}

function TownParseNumber( value, fallback )
{
	var parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function TownNameForRegion( townRegionId )
{
	if( FactionTownRegions[townRegionId] )
		return FactionTownRegions[townRegionId];

	return "";
}

function TownNameForObject( townObject )
{
	if( !ValidateObject( townObject ) || !TownRegionIsLoaded( townObject.region ) )
		return "";

	return TownNameForRegion( townObject.region.id );
}

function TownOwnerForRegion( townRegionId )
{
	var townName = TownNameForRegion( townRegionId );
	if( townName === "" )
		return "";

	return TownGetOwner( townName );
}

function TownOwnerForObject( townObject )
{
	var townName = TownNameForObject( townObject );
	if( townName === "" )
		return "";

	return TownGetOwner( townName );
}

function TownIsObjectInControlledTownForFaction( townObject, factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return false;

	var townOwner = TownOwnerForObject( townObject );
	if( townOwner === "" )
		return true;

	return townOwner === factionKey;
}

function TownControlledByFactionList( factionKey )
{
	var townList = "";
	for( var townName in FactionTownDefaults )
	{
		if( FactionTownDefaults.hasOwnProperty( townName ) && TownGetOwner( townName ) === factionKey )
		{
			if( townList !== "" )
				townList += ", ";
			townList += TownDisplayName( townName );
		}
	}

	if( townList === "" )
		return "None";

	return townList;
}

function TownGetController()
{
	if( ValidateObject( FactionTownController ) )
		return FactionTownController;

	FactionTownController = null;
	FactionTownIterateMode = "controller";
	IterateOver( "ITEM" );
	FactionTownIterateMode = "";
	return FactionTownController;
}

function RegisterController( ctrl )
{
	if( !ValidateObject( ctrl ) || ctrl.GetTag( "faction_controller" ) != 1 )
		return false;

	FactionTownController = ctrl;
	if( ctrl.GetTag( "faction_town_tax_enabled" ) == 1 )
	{
		var interval = TownTaxInterval( ctrl );
		ctrl.SetTag( "faction_town_tax_next", GetCurrentClock() + interval );
		ctrl.KillJSTimer( FactionTownTaxTimerId, 8509 );
		ctrl.StartTimer( interval, FactionTownTaxTimerId, 8509 );
	}

	return true;
}

function TownTaxInterval( ctrl )
{
	if( !ValidateObject( ctrl ) )
		return FactionTownDefaultTaxInterval;

	var interval = TownParseNumber( ctrl.GetTag( "faction_town_tax_interval" ), FactionTownDefaultTaxInterval );
	if( interval < 60000 )
		interval = 60000;

	return interval;
}

function StartTownTaxTimer( minutes )
{
	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	var interval = TownParseNumber( minutes, 0 );
	if( interval > 0 )
		interval = interval * 60000;
	else
		interval = TownTaxInterval( ctrl );
	if( interval < 60000 )
		interval = 60000;

	ctrl.SetTag( "faction_town_tax_enabled", 1 );
	ctrl.SetTag( "faction_town_tax_interval", interval );
	ctrl.SetTag( "faction_town_tax_next", GetCurrentClock() + interval );
	ctrl.KillJSTimer( FactionTownTaxTimerId, 8509 );
	ctrl.StartTimer( interval, FactionTownTaxTimerId, 8509 );
	return true;
}

function StopTownTaxTimer()
{
	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.SetTag( "faction_town_tax_enabled", 0 );
	ctrl.SetTag( "faction_town_tax_next", 0 );
	ctrl.KillJSTimer( FactionTownTaxTimerId, 8509 );
	return true;
}

function ShowTownTaxStatus( pSock )
{
	if( pSock == null )
		return false;

	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
	{
		pSock.SysMessage( "Faction controller was not found." );
		return false;
	}

	pSock.SysMessage( TownTaxStatusText() );
	return true;
}

function TownTaxStatusText()
{
	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return "Tax Timer: faction controller missing.";

	var enabled = ctrl.GetTag( "faction_town_tax_enabled" ) == 1;
	var interval = Math.ceil( TownTaxInterval( ctrl ) / 60000 );
	var nextTime = TownParseNumber( ctrl.GetTag( "faction_town_tax_next" ), 0 );
	var lastIncome = TownParseNumber( ctrl.GetTag( "faction_town_last_tax_income" ), 0 );
	var statusText = enabled ? "enabled" : "disabled";
	var taxText = "Tax Timer: " + statusText + ", interval " + interval + " minute(s), last income " + lastIncome + ".";
	if( enabled && nextTime > GetCurrentClock() )
		taxText += " Next cycle in " + Math.ceil( ( nextTime - GetCurrentClock() ) / 60000 ) + " minute(s).";

	return taxText;
}

function onTimer( timerObj, timerID )
{
	if( timerID != FactionTownTaxTimerId )
		return;
	if( !ValidateObject( timerObj ) || timerObj.GetTag( "faction_controller" ) != 1 )
		return;

	FactionTownController = timerObj;
	if( timerObj.GetTag( "faction_town_tax_enabled" ) != 1 )
		return;

	var totalIncome = RunTownTaxCycle();
	var interval = TownTaxInterval( timerObj );
	timerObj.SetTag( "faction_town_tax_next", GetCurrentClock() + interval );
	timerObj.StartTimer( interval, FactionTownTaxTimerId, 8509 );
	if( totalIncome > 0 )
		Console.Print( "Faction town tax cycle generated " + totalIncome + " silver." );
}

function onIterate( toCheck )
{
	if( FactionTownIterateMode === "controller" )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
		{
			FactionTownController = toCheck;
			return true;
		}

		return false;
	}

	if( FactionTownIterateMode === "countnpcs" || FactionTownIterateMode === "clearnpcs" || FactionTownIterateMode === "listnpcs" )
	{
		if( !TownFactionNpcMatches( toCheck, FactionTownIterateTown, FactionTownIterateFaction ) )
			return false;

		if( FactionTownIterateMode === "countnpcs" && FactionTownCountType !== "" && toCheck.GetTag( "faction_npc_type" ) !== FactionTownCountType )
			return false;

		if( FactionTownIterateMode === "clearnpcs" )
		{
			toCheck.Delete();
			return true;
		}

		if( FactionTownIterateMode === "listnpcs" && FactionTownIterateSocket != null )
		{
			var npcTown = toCheck.GetTag( "faction_town" );
			var npcType = toCheck.GetTag( "faction_npc_type" );
			var vendorType = toCheck.GetTag( "faction_vendor_type" );
			var npcFaction = TownFactionForNpc( toCheck );
			var typeText = npcType;
			var townText = "No town";
			if( vendorType !== "" )
				typeText += " " + vendorType;
			if( npcTown !== "" )
				townText = TownDisplayName( npcTown );

			FactionTownIterateSocket.SysMessage( townText + ": " + TownFactionName( npcFaction ) + " " + typeText + " " + toCheck.name );
		}

		return true;
	}

	return false;
}

function TownFactionForNpc( npcChar )
{
	if( !ValidateObject( npcChar ) )
		return "";

	var factionKey = npcChar.GetTag( "faction" );
	if( TownIsFactionValid( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "guard_faction" );
	if( TownIsFactionValid( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "vendor_faction" );
	if( TownIsFactionValid( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "npc_faction" );
	if( TownIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function TownIsFactionNpc( npcChar )
{
	if( !ValidateObject( npcChar ) || !npcChar.isChar || !npcChar.npc )
		return false;

	return ( npcChar.GetTag( "faction_spawned" ) == 1 );
}

function TownFactionNpcMatches( npcChar, townName, factionKey )
{
	if( !TownIsFactionNpc( npcChar ) )
		return false;

	townName = TownNormalizeName( townName );
	var npcTown = TownNormalizeName( npcChar.GetTag( "faction_town" ) );
	if( npcTown === "" )
		npcTown = TownNameForObject( npcChar );

	if( townName !== "" && npcTown !== townName )
		return false;

	if( factionKey !== "" && TownFactionForNpc( npcChar ) !== factionKey )
		return false;

	return true;
}

function TownTagFactionNpc( npcChar, factionKey, townName, npcType, vendorType )
{
	if( !ValidateObject( npcChar ) || !TownIsFactionValid( factionKey ) )
		return false;

	townName = TownNormalizeName( townName );
	if( townName !== "" && TownGetDefault( townName ) == null )
		return false;

	npcChar.SetTag( "faction_spawned", 1 );
	npcChar.SetTag( "faction", factionKey );
	npcChar.SetTag( "npc_faction", factionKey );
	npcChar.SetTag( "faction_town", townName );
	npcChar.SetTag( "faction_npc_type", npcType );
	npcChar.SetTag( "faction_vendor_type", vendorType );
	npcChar.SetTag( "faction_spawned_at", GetCurrentClock() );

	return true;
}

function TownCountFactionNpcs( townName, factionKey )
{
	townName = TownNormalizeName( townName );
	if( factionKey !== "" && !TownIsFactionValid( factionKey ) )
		return 0;

	FactionTownIterateMode = "countnpcs";
	FactionTownIterateTown = townName;
	FactionTownIterateFaction = factionKey;
	var npcCount = IterateOver( "CHARACTER" );
	FactionTownIterateMode = "";
	FactionTownIterateTown = "";
	FactionTownIterateFaction = "";

	return npcCount;
}

function TownCountFactionNpcsByType( townName, factionKey, npcType )
{
	townName = TownNormalizeName( townName );
	if( factionKey !== "" && !TownIsFactionValid( factionKey ) )
		return 0;

	npcType = String( npcType ).toLowerCase();
	if( npcType !== "guard" && npcType !== "vendor" )
		return 0;

	FactionTownIterateMode = "countnpcs";
	FactionTownIterateTown = townName;
	FactionTownIterateFaction = factionKey;
	FactionTownCountType = npcType;
	var npcCount = IterateOver( "CHARACTER" );
	FactionTownIterateMode = "";
	FactionTownIterateTown = "";
	FactionTownIterateFaction = "";
	FactionTownCountType = "";

	return npcCount;
}

function TownNpcLimitForType( npcType )
{
	npcType = String( npcType ).toLowerCase();
	if( npcType === "guard" )
		return FactionTownGuardLimit;
	if( npcType === "vendor" )
		return FactionTownVendorLimit;

	return 0;
}

function TownCanPlaceFactionNpc( townName, factionKey, npcType )
{
	townName = TownNormalizeName( townName );
	npcType = String( npcType ).toLowerCase();
	if( townName === "" || TownGetDefault( townName ) == null )
		return "Unknown faction town.";
	if( !TownIsFactionValid( factionKey ) )
		return "Invalid faction.";
	if( npcType !== "guard" && npcType !== "vendor" )
		return "Invalid faction NPC type.";

	var owner = TownGetOwner( townName );
	if( owner !== factionKey )
		return "Only " + TownFactionName( owner ) + " may place faction NPCs in " + TownDisplayName( townName ) + ".";

	var limit = TownNpcLimitForType( npcType );
	var currentCount = TownCountFactionNpcsByType( townName, factionKey, npcType );
	if( limit > 0 && currentCount >= limit )
		return TownDisplayName( townName ) + " already has " + currentCount + "/" + limit + " faction " + npcType + "(s).";

	return "";
}

function TownNpcLimitSummary( townName, factionKey )
{
	townName = TownNormalizeName( townName );
	if( factionKey === "" )
		factionKey = TownGetOwner( townName );
	if( townName === "" || TownGetDefault( townName ) == null || !TownIsFactionValid( factionKey ) )
		return "Faction NPC Limits: unknown town.";

	var guards = TownCountFactionNpcsByType( townName, factionKey, "guard" );
	var vendors = TownCountFactionNpcsByType( townName, factionKey, "vendor" );
	return "Faction NPC Limits: guards " + guards + "/" + FactionTownGuardLimit + ", vendors " + vendors + "/" + FactionTownVendorLimit + ".";
}

function TownClearFactionNpcs( townName, factionKey )
{
	townName = TownNormalizeName( townName );
	if( townName !== "" && TownGetDefault( townName ) == null )
		return 0;
	if( factionKey !== "" && !TownIsFactionValid( factionKey ) )
		return 0;

	FactionTownIterateMode = "clearnpcs";
	FactionTownIterateTown = townName;
	FactionTownIterateFaction = factionKey;
	var removedCount = IterateOver( "CHARACTER" );
	FactionTownIterateMode = "";
	FactionTownIterateTown = "";
	FactionTownIterateFaction = "";

	return removedCount;
}

function ShowTownNpcStatus( pSock, townName )
{
	if( !pSock )
		return false;

	townName = TownNormalizeName( townName );
	if( townName !== "" && TownGetDefault( townName ) == null )
	{
		pSock.SysMessage( "Unknown faction town: " + townName );
		return false;
	}

	var npcCount = TownCountFactionNpcs( townName, "" );
	if( npcCount == 0 )
	{
		pSock.SysMessage( "No managed faction NPCs found." );
		if( townName !== "" )
			pSock.SysMessage( TownNpcLimitSummary( townName, "" ) );
		return true;
	}

	if( townName !== "" )
		pSock.SysMessage( TownNpcLimitSummary( townName, "" ) );

	FactionTownIterateMode = "listnpcs";
	FactionTownIterateTown = townName;
	FactionTownIterateFaction = "";
	FactionTownIterateSocket = pSock;
	IterateOver( "CHARACTER" );
	FactionTownIterateMode = "";
	FactionTownIterateTown = "";
	FactionTownIterateFaction = "";
	FactionTownIterateSocket = null;

	return true;
}

function TownGetOwner( townName )
{
	townName = TownNormalizeName( townName );
	var ctrl = TownGetController();
	if( ValidateObject( ctrl ) )
	{
		var owner = ctrl.GetTag( "faction_town_" + townName + "_owner" );
		if( TownIsFactionValid( owner ) )
			return owner;
	}

	return TownDefaultOwner( townName );
}

function TownSetOwner( townName, factionKey )
{
	townName = TownNormalizeName( townName );
	if( !TownIsFactionValid( factionKey ) || TownGetDefault( townName ) == null )
		return false;

	var ctrl = TownGetController();
	if( ValidateObject( ctrl ) )
	{
		ctrl.SetTag( "faction_town_" + townName + "_owner", factionKey );
		ctrl.SetTag( "faction_town_" + townName + "_claimed", GetCurrentClock() );
	}

	return true;
}

function TownTreasuryTag( townName )
{
	return "faction_town_" + TownNormalizeName( townName ) + "_treasury";
}

function TownTaxRateTag( townName )
{
	return "faction_town_" + TownNormalizeName( townName ) + "_taxrate";
}

function TownGetTreasury( townName )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return -1;

	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	return TownParseNumber( ctrl.GetTag( TownTreasuryTag( townName ) ), 0 );
}

function TownSetTreasury( townName, amount )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return false;

	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	amount = TownParseNumber( amount, 0 );
	if( amount < 0 )
		amount = 0;

	ctrl.SetTag( TownTreasuryTag( townName ), amount );
	return true;
}

function TownAddTreasury( townName, amount )
{
	var currentAmount = TownGetTreasury( townName );
	if( currentAmount < 0 )
		return false;

	return TownSetTreasury( townName, currentAmount + TownParseNumber( amount, 0 ) );
}

function TownSpendTreasury( townName, amount )
{
	var currentAmount = TownGetTreasury( townName );
	amount = TownParseNumber( amount, 0 );
	if( currentAmount < 0 || amount < 0 || currentAmount < amount )
		return false;

	return TownSetTreasury( townName, currentAmount - amount );
}

function TownGrantTreasury( townName )
{
	return TownAddTreasury( townName, FactionTownTreasuryGrant );
}

function TownGetTaxRate( townName )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return -1;

	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	var taxRate = ctrl.GetTag( TownTaxRateTag( townName ) );
	if( taxRate === "" || taxRate == 0 )
		return FactionTownDefaultTaxRate;

	return TownParseNumber( taxRate, FactionTownDefaultTaxRate );
}

function TownSetTaxRate( townName, amount )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return false;

	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	amount = TownParseNumber( amount, FactionTownDefaultTaxRate );
	if( amount < 0 )
		amount = 0;

	ctrl.SetTag( TownTaxRateTag( townName ), amount );
	return true;
}

function RunTownTaxCycle()
{
	var ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	var totalIncome = 0;
	for( var townName in FactionTownDefaults )
	{
		var owner = TownGetOwner( townName );
		if( !TownIsFactionValid( owner ) )
			continue;

		var taxRate = TownGetTaxRate( townName );
		if( taxRate <= 0 )
			continue;

		if( TownAddTreasury( townName, taxRate ) )
			totalIncome += taxRate;
	}

	ctrl.SetTag( "faction_town_last_tax_cycle", GetCurrentClock() );
	ctrl.SetTag( "faction_town_last_tax_income", totalIncome );
	return totalIncome;
}

function ShowTownTreasury( pSock, townName )
{
	if( pSock == null )
		return false;

	townName = TownNormalizeName( townName );
	if( townName !== "" )
	{
		if( TownGetDefault( townName ) == null )
		{
			pSock.SysMessage( "Unknown faction town." );
			return false;
		}

		pSock.SysMessage( TownDisplayName( townName ) + ": treasury " + TownGetTreasury( townName ) + " silver, tax " + TownGetTaxRate( townName ) + ", owner " + TownFactionName( TownGetOwner( townName ) ) );
		return true;
	}

	for( var defaultTown in FactionTownDefaults )
		pSock.SysMessage( TownDisplayName( defaultTown ) + ": treasury " + TownGetTreasury( defaultTown ) + " silver, tax " + TownGetTaxRate( defaultTown ) + ", owner " + TownFactionName( TownGetOwner( defaultTown ) ) );

	return true;
}

function TownTreasuryByFactionText( factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return "Treasury: 0, Tax: 0";

	var totalTreasury = 0;
	var totalTax = 0;
	for( var townName in FactionTownDefaults )
	{
		if( TownGetOwner( townName ) === factionKey )
		{
			totalTreasury += TownGetTreasury( townName );
			totalTax += TownGetTaxRate( townName );
		}
	}

	return "Treasury: " + totalTreasury + ", Tax/Cycle: " + totalTax;
}

function ApplyTownControl( townName, factionKey, townRegionId )
{
	FactionTownLastError = "";
	townName = TownNormalizeName( townName );
	if( !TownIsFactionValid( factionKey ) )
		return TownSetLastError( "Invalid faction key: " + factionKey );

	if( TownGetDefault( townName ) == null )
		return TownSetLastError( "Unknown faction town: " + townName );

	townRegionId = Number( townRegionId );
	if( isNaN( townRegionId ) || townRegionId == 0 )
		townRegionId = TownRegionForName( townName );
	if( townRegionId == 0 )
		return TownSetLastError( "No town region configured for " + townName );

	var townRegion = GetTownRegion( townRegionId );
	if( !TownRegionIsLoaded( townRegion ) )
		return TownSetLastError( "Town region " + townRegionId + " is not loaded." );

	var oldOwner = TownGetOwner( townName );
	if( !TownSetOwner( townName, factionKey ) )
		return TownSetLastError( "Unable to store town owner for " + townName );

	var removedCount = 0;
	if( TownIsFactionValid( oldOwner ) && oldOwner !== factionKey )
		removedCount = TownClearFactionNpcs( townName, oldOwner );

	if( oldOwner !== factionKey )
		TownGrantTreasury( townName );

	townRegion.owner = TownOwnerName( factionKey );
	townRegion.isGuarded = true;

	if( townRegion.numGuards < 5 )
		townRegion.numGuards = 5;

	BroadcastMessage( TownFactionName( factionKey ) + " now controls " + townRegion.name + "." );
	if( removedCount > 0 )
		BroadcastMessage( removedCount + " opposing faction NPC(s) have withdrawn from " + townRegion.name + "." );

	return true;
}

function ApplySigilTownControl( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return false;

	var factionKey = iSigil.GetTag( "sigil_owner_faction" );
	var townName = iSigil.GetTag( "sigil_town" );
	var townRegionId = iSigil.GetTag( "town_region" );

	return ApplyTownControl( townName, factionKey, townRegionId );
}

function SyncTownControl()
{
	for( var townName in FactionTownDefaults )
	{
		if( FactionTownDefaults.hasOwnProperty( townName ) )
			ApplyTownControl( townName, TownGetOwner( townName ), FactionTownDefaults[townName].region );
	}

	return true;
}

function ShowTownStatus( pSock )
{
	if( !pSock )
		return false;

	for( var townName in FactionTownDefaults )
	{
		if( FactionTownDefaults.hasOwnProperty( townName ) )
		{
			var townInfo = FactionTownDefaults[townName];
			var townRegion = GetTownRegion( townInfo.region );
			var displayName = TownRegionIsLoaded( townRegion ) ? townRegion.name : townName;
			pSock.SysMessage( displayName + ": " + TownFactionName( TownGetOwner( townName ) ) );
		}
	}

	return true;
}
