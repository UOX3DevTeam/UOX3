/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_towns.js
// UOX3 Faction System - town region ownership bridge
// Script ID: 8509
// =============================================================================

let factionTownController = null;

const factionTownNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

const factionTownOwnerNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

const factionTownDefaults = {
	Britain: { region: 3, owner: "TB" },
	Trinsic: { region: 9, owner: "TB" },
	Moonglow: { region: 13, owner: "COM" },
	SkaraBrae: { region: 8, owner: "COM" },
	Yew: { region: 7, owner: "MIN" },
	Vesper: { region: 11, owner: "MIN" },
	Minoc: { region: 12, owner: "SL" },
	Magincia: { region: 15, owner: "SL" }
};

const factionTownRegions = {
	3: "Britain",
	9: "Trinsic",
	13: "Moonglow",
	8: "SkaraBrae",
	7: "Yew",
	11: "Vesper",
	12: "Minoc",
	15: "Magincia"
};

const factionTownNameAliases = {
	britain: "Britain",
	trinsic: "Trinsic",
	moonglow: "Moonglow",
	skarabrae: "SkaraBrae",
	yew: "Yew",
	vesper: "Vesper",
	minoc: "Minoc",
	magincia: "Magincia",
	cove: "Magincia"
};

let factionTownLastError = "";
let factionTownIterateMode = "";
let factionTownIterateTown = "";
let factionTownIterateFaction = "";
let factionTownIterateSocket = null;
const factionTownTreasuryGrant = 1000;
const factionTownDefaultTaxRate = 0;
const factionTownTaxTimerId = 2;
const factionTownDefaultTaxInterval = 86400000;
const factionTownTaxChangePeriod = parseInt( GetServerSetting( "FACTIONTOWNTAXCHANGEHOURS" ), 10 ) * 3600000;
const factionTownDailyIncome = parseInt( GetServerSetting( "FACTIONTOWNBASEINCOME" ), 10 );
const factionTownTaxOffsets = [ -30, -25, -20, -15, -10, -5, 0, 50, 100, 150, 200, 250, 300 ];
const factionTownGuardLimit = 10;
const factionTownVendorLimit = 10;
let factionTownCountType = "";
const factionTownPlayerDataScriptId = 8513;
let factionTownUpkeepTotal = 0;
let factionTownRemoveDone = false;

function TownIniNumber( settingName, fallbackValue, minValue )
{
	let settingValue = fallbackValue;
	if( typeof GetServerSetting == "function" )
	{
		const rawValue = GetServerSetting( settingName );
		const parsedValue = parseInt( rawValue, 10 );
		if( !isNaN( parsedValue ) )
			settingValue = parsedValue;
	}

	if( typeof minValue != "undefined" && settingValue < minValue )
		settingValue = minValue;

	return settingValue;
}

function TownDefaultTaxIntervalMinutes()
{
	return TownIniNumber( "FACTIONTOWNTAXINTERVAL", Math.ceil( factionTownDefaultTaxInterval / 60000 ), 1 );
}

function TownDefaultTaxIntervalMs()
{
	return TownDefaultTaxIntervalMinutes() * 60000;
}

function TownDefaultTaxRate()
{
	return TownIniNumber( "FACTIONTOWNDEFAULTTAXRATE", factionTownDefaultTaxRate );
}

function TownTreasuryGrantAmount()
{
	return TownIniNumber( "FACTIONTOWNTREASURYGRANT", factionTownTreasuryGrant, 0 );
}

function TownDefaultNpcLimit( npcType )
{
	npcType = String( npcType ).toLowerCase();
	if( npcType === "guard" )
		return TownIniNumber( "FACTIONTOWNGUARDLIMIT", factionTownGuardLimit, 0 );
	if( npcType === "vendor" )
		return TownIniNumber( "FACTIONTOWNVENDORLIMIT", factionTownVendorLimit, 0 );

	return 0;
}

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
	let cleanName = String( townName );
	cleanName = cleanName.replace( /^\s+|\s+$/g, "" );
	cleanName = cleanName.replace( /^the town of /i, "" );

	let aliasName = cleanName.toLowerCase();
	aliasName = aliasName.replace( /[^a-z]/g, "" );
	if( factionTownNameAliases[aliasName] )
		return factionTownNameAliases[aliasName];

	return cleanName;
}

function TownFactionName( factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return "Unknown Faction";

	return factionTownNames[factionKey];
}

function TownOwnerName( factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return "The Town";

	return factionTownOwnerNames[factionKey];
}

function TownGetDefault( townName )
{
	townName = TownNormalizeName( townName );
	if( factionTownDefaults[townName] )
		return factionTownDefaults[townName];

	return null;
}

function TownSetLastError( errorMessage )
{
	factionTownLastError = errorMessage;
	return false;
}

function TownLastError()
{
	return factionTownLastError;
}

function TownRegionForName( townName )
{
	let townInfo = TownGetDefault( townName );
	if( townInfo == null )
		return 0;

	return townInfo.region;
}

function TownDefaultOwner( townName )
{
	let townInfo = TownGetDefault( townName );
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
	const parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function TownNameForRegion( townRegionId )
{
	if( factionTownRegions[townRegionId] )
		return factionTownRegions[townRegionId];

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
	let townName = TownNameForRegion( townRegionId );
	if( townName === "" )
		return "";

	return TownGetOwner( townName );
}

function TownOwnerForObject( townObject )
{
	let townName = TownNameForObject( townObject );
	if( townName === "" )
		return "";

	return TownGetOwner( townName );
}

function TownIsObjectInControlledTownForFaction( townObject, factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return false;

	let townOwner = TownOwnerForObject( townObject );
	if( townOwner === "" )
		return true;

	return townOwner === factionKey;
}

function TownControlledByFactionList( factionKey )
{
	let townList = "";
	for( let townName in factionTownDefaults )
	{
		if( factionTownDefaults.hasOwnProperty( townName ) && TownGetOwner( townName ) === factionKey )
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
	if( ValidateObject( factionTownController ) )
		return factionTownController;

	factionTownController = null;
	factionTownIterateMode = "controller";
	IterateOver( "ITEM" );
	factionTownIterateMode = "";
	return factionTownController;
}

function RegisterController( ctrl )
{
	if( !ValidateObject( ctrl ) || ctrl.GetTag( "faction_controller" ) != 1 )
		return false;

	factionTownController = ctrl;
	if( ctrl.GetTag( "faction_town_tax_enabled" ) == 1 )
	{
		let interval = TownTaxInterval( ctrl );
		ctrl.SetTag( "faction_town_tax_next", GetCurrentClock() + interval );
		ctrl.KillJSTimer( factionTownTaxTimerId, 8509 );
		ctrl.StartTimer( interval, factionTownTaxTimerId, 8509 );
	}

	return true;
}

function TownTaxInterval( ctrl )
{
	if( !ValidateObject( ctrl ) )
		return TownDefaultTaxIntervalMs();

	let interval = TownParseNumber( ctrl.GetTag( "faction_town_tax_interval" ), TownDefaultTaxIntervalMs() );
	if( interval < 60000 )
		interval = 60000;

	return interval;
}

function StartTownTaxTimer( minutes )
{
	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	let interval = TownParseNumber( minutes, 0 );
	if( interval > 0 )
		interval = interval * 60000;
	else
		interval = TownTaxInterval( ctrl );
	if( interval < 60000 )
		interval = 60000;

	ctrl.SetTag( "faction_town_tax_enabled", 1 );
	ctrl.SetTag( "faction_town_tax_interval", interval );
	ctrl.SetTag( "faction_town_tax_next", GetCurrentClock() + interval );
	ctrl.KillJSTimer( factionTownTaxTimerId, 8509 );
	ctrl.StartTimer( interval, factionTownTaxTimerId, 8509 );
	return true;
}

function StopTownTaxTimer()
{
	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.SetTag( "faction_town_tax_enabled", 0 );
	ctrl.SetTag( "faction_town_tax_next", 0 );
	ctrl.KillJSTimer( factionTownTaxTimerId, 8509 );
	return true;
}

function ShowTownTaxStatus( pSock )
{
	if( pSock == null )
		return false;

	const ctrl = TownGetController();
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
	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return "Tax Timer: faction controller missing.";

	const enabled = ctrl.GetTag( "faction_town_tax_enabled" ) == 1;
	let interval = Math.ceil( TownTaxInterval( ctrl ) / 60000 );
	const nextTime = TownParseNumber( ctrl.GetTag( "faction_town_tax_next" ), 0 );
	const lastIncome = TownParseNumber( ctrl.GetTag( "faction_town_last_tax_income" ), 0 );
	const statusText = enabled ? "enabled" : "disabled";
	let taxText = "Tax Timer: " + statusText + ", interval " + interval + " minute(s), last income " + lastIncome + ".";
	if( enabled && nextTime > GetCurrentClock() )
		taxText += " Next cycle in " + Math.ceil( ( nextTime - GetCurrentClock() ) / 60000 ) + " minute(s).";

	return taxText;
}

function onTimer( timerObj, timerID )
{
	if( timerID != factionTownTaxTimerId )
		return;
	if( !ValidateObject( timerObj ) || timerObj.GetTag( "faction_controller" ) != 1 )
		return;

	factionTownController = timerObj;
	if( timerObj.GetTag( "faction_town_tax_enabled" ) != 1 )
		return;

	let totalIncome = RunTownTaxCycle();
	let interval = TownTaxInterval( timerObj );
	timerObj.SetTag( "faction_town_tax_next", GetCurrentClock() + interval );
	timerObj.StartTimer( interval, factionTownTaxTimerId, 8509 );
	if( totalIncome > 0 )
		Console.Print( "Faction town tax cycle generated " + totalIncome + " silver." );
}

function onIterate( toCheck )
{
	if( factionTownIterateMode === "controller" )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
		{
			factionTownController = toCheck;
			return true;
		}

		return false;
	}

	if( factionTownIterateMode === "countnpcs" || factionTownIterateMode === "clearnpcs" || factionTownIterateMode === "listnpcs" )
	{
		if( !TownFactionNpcMatches( toCheck, factionTownIterateTown, factionTownIterateFaction ) )
			return false;

		if( factionTownIterateMode === "countnpcs" && factionTownCountType !== "" && toCheck.GetTag( "faction_npc_type" ) !== factionTownCountType )
			return false;

		if( factionTownIterateMode === "clearnpcs" )
		{
			toCheck.Delete();
			return true;
		}

		if( factionTownIterateMode === "listnpcs" && factionTownIterateSocket != null )
		{
			let npcTown = toCheck.GetTag( "faction_town" );
			let npcType = toCheck.GetTag( "faction_npc_type" );
			const vendorType = toCheck.GetTag( "faction_vendor_type" );
			const npcFaction = TownFactionForNpc( toCheck );
			let typeText = npcType;
			let townText = "No town";
			if( vendorType !== "" )
				typeText += " " + vendorType;
			if( npcTown !== "" )
				townText = TownDisplayName( npcTown );

			factionTownIterateSocket.SysMessage( townText + ": " + TownFactionName( npcFaction ) + " " + typeText + " " + toCheck.name );
		}

		return true;
	}

	if( factionTownIterateMode === "upkeep" || factionTownIterateMode === "removeupkeep" )
	{
		if( factionTownIterateMode === "removeupkeep" && factionTownRemoveDone )
			return false;
		if( !TownFactionNpcMatches( toCheck, factionTownIterateTown, factionTownIterateFaction ) )
			return false;
		if( factionTownIterateMode === "upkeep" )
		{
			let npcUpkeep = TownParseNumber( toCheck.GetTag( "faction_upkeep" ), 0 );
			if( npcUpkeep <= 0 )
			{
				let legacyVendorType = String( toCheck.GetTag( "faction_vendor_type" ) || "" ).toUpperCase();
				npcUpkeep = toCheck.GetTag( "faction_npc_type" ) === "guard" ? 1000 : (( legacyVendorType === "BOARD" || legacyVendorType === "ORE" ) ? 500 : 1000 );
				toCheck.SetTag( "faction_upkeep", npcUpkeep );
			}
			factionTownUpkeepTotal += npcUpkeep;
			return true;
		}
		toCheck.Delete();
		factionTownRemoveDone = true;
		return true;
	}

	if( factionTownIterateMode === "clearoffices" )
	{
		if( !ValidateObject( toCheck ) || !toCheck.isChar || toCheck.npc )
			return false;
		const officeData = TriggerEvent( factionTownPlayerDataScriptId, "ReadFactionPlayerData", toCheck );
		if(( officeData.role === "sheriff" || officeData.role === "finance" ) && officeData.roleTown === factionTownIterateTown )
		{
			officeData.role = "";
			officeData.roleFaction = "";
			officeData.roleTown = "";
			officeData.roleSetAt = 0;
			TriggerEvent( factionTownPlayerDataScriptId, "WriteFactionPlayerData", toCheck, officeData );
			if( toCheck.socket != null )
				toCheck.SysMessage( "Your office in " + TownDisplayName( factionTownIterateTown ) + " ended when control changed." );
			return true;
		}
		return false;
	}

	return false;
}

function TownFactionForNpc( npcChar )
{
	if( !ValidateObject( npcChar ) )
		return "";

	let factionKey = npcChar.GetTag( "faction" );
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
	let npcTown = TownNormalizeName( npcChar.GetTag( "faction_town" ) );
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
	let vendorKey = String( vendorType || "" ).toUpperCase();
	let upkeep = npcType === "guard" ? 1000 : (( vendorKey === "BOARD" || vendorKey === "ORE" ) ? 500 : 1000 );
	npcChar.SetTag( "faction_upkeep", upkeep );

	return true;
}

function TownCountFactionNpcs( townName, factionKey )
{
	townName = TownNormalizeName( townName );
	if( factionKey !== "" && !TownIsFactionValid( factionKey ) )
		return 0;

	factionTownIterateMode = "countnpcs";
	factionTownIterateTown = townName;
	factionTownIterateFaction = factionKey;
	let npcCount = IterateOver( "CHARACTER" );
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	factionTownIterateFaction = "";

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

	factionTownIterateMode = "countnpcs";
	factionTownIterateTown = townName;
	factionTownIterateFaction = factionKey;
	factionTownCountType = npcType;
	let npcCount = IterateOver( "CHARACTER" );
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	factionTownIterateFaction = "";
	factionTownCountType = "";

	return npcCount;
}

function TownNpcLimitForType( npcType )
{
	npcType = String( npcType ).toLowerCase();
	if( npcType !== "guard" && npcType !== "vendor" )
		return 0;

	const ctrl = TownGetController();
	if( ValidateObject( ctrl ) )
	{
		const tagLimit = TownParseNumber( ctrl.GetTag( "faction_town_" + npcType + "_limit" ), -1 );
		if( tagLimit >= 0 )
			return tagLimit;
	}

	return TownDefaultNpcLimit( npcType );
}

function TownSetNpcLimit( npcType, amount )
{
	npcType = String( npcType ).toLowerCase();
	if( npcType !== "guard" && npcType !== "vendor" )
		return false;

	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	amount = TownParseNumber( amount, TownDefaultNpcLimit( npcType ) );
	if( amount < 0 )
		amount = 0;

	ctrl.SetTag( "faction_town_" + npcType + "_limit", amount );
	return true;
}

function TownClearNpcLimit( npcType )
{
	npcType = String( npcType ).toLowerCase();
	if( npcType !== "guard" && npcType !== "vendor" )
		return false;

	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.SetTag( "faction_town_" + npcType + "_limit", -1 );
	return true;
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

	let owner = TownGetOwner( townName );
	if( owner !== factionKey )
		return "Only " + TownFactionName( owner ) + " may place faction NPCs in " + TownDisplayName( townName ) + ".";

	const limit = TownNpcLimitForType( npcType );
	const currentCount = TownCountFactionNpcsByType( townName, factionKey, npcType );
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

	const guards = TownCountFactionNpcsByType( townName, factionKey, "guard" );
	const vendors = TownCountFactionNpcsByType( townName, factionKey, "vendor" );
	return "Faction NPC Limits: guards " + guards + "/" + TownNpcLimitForType( "guard" ) + ", vendors " + vendors + "/" + TownNpcLimitForType( "vendor" ) + ".";
}

function TownClearFactionNpcs( townName, factionKey )
{
	townName = TownNormalizeName( townName );
	if( townName !== "" && TownGetDefault( townName ) == null )
		return 0;
	if( factionKey !== "" && !TownIsFactionValid( factionKey ) )
		return 0;

	factionTownIterateMode = "clearnpcs";
	factionTownIterateTown = townName;
	factionTownIterateFaction = factionKey;
	let removedCount = IterateOver( "CHARACTER" );
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	factionTownIterateFaction = "";

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

	let npcCount = TownCountFactionNpcs( townName, "" );
	if( npcCount == 0 )
	{
		pSock.SysMessage( "No managed faction NPCs found." );
		if( townName !== "" )
			pSock.SysMessage( TownNpcLimitSummary( townName, "" ) );
		return true;
	}

	if( townName !== "" )
		pSock.SysMessage( TownNpcLimitSummary( townName, "" ) );

	factionTownIterateMode = "listnpcs";
	factionTownIterateTown = townName;
	factionTownIterateFaction = "";
	factionTownIterateSocket = pSock;
	IterateOver( "CHARACTER" );
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	factionTownIterateFaction = "";
	factionTownIterateSocket = null;

	return true;
}

function TownGetOwner( townName )
{
	townName = TownNormalizeName( townName );
	const ctrl = TownGetController();
	if( ValidateObject( ctrl ) )
	{
		let owner = ctrl.GetTag( "faction_town_" + townName + "_owner" );
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

	const ctrl = TownGetController();
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

function TownTaxRateSetTag( townName )
{
	return "faction_town_" + TownNormalizeName( townName ) + "_taxrateset";
}

function TownLastTaxChangeTag( townName )
{
	return "faction_town_" + TownNormalizeName( townName ) + "_lasttaxchange";
}

function TownTaxRateIsValid( amount )
{
	amount = TownParseNumber( amount, 9999 );
	for( let taxIndex = 0; taxIndex < factionTownTaxOffsets.length; taxIndex++ )
	{
		if( factionTownTaxOffsets[taxIndex] == amount )
			return true;
	}

	return false;
}

function TownTaxChangeRemaining( townName )
{
	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	const lastChange = TownParseNumber( ctrl.GetTag( TownLastTaxChangeTag( townName ) ), 0 );
	if( lastChange <= 0 )
		return 0;

	const remaining = ( lastChange + factionTownTaxChangePeriod ) - GetCurrentClock();
	return remaining > 0 ? remaining : 0;
}

function TownGetTreasury( townName )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return -1;

	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	return TownParseNumber( ctrl.GetTag( TownTreasuryTag( townName ) ), 0 );
}

function TownSetTreasury( townName, amount )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return false;

	const ctrl = TownGetController();
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
	const currentAmount = TownGetTreasury( townName );
	if( currentAmount < 0 )
		return false;

	return TownSetTreasury( townName, currentAmount + TownParseNumber( amount, 0 ) );
}

function TownSpendTreasury( townName, amount )
{
	const currentAmount = TownGetTreasury( townName );
	amount = TownParseNumber( amount, 0 );
	if( currentAmount < 0 || amount < 0 || currentAmount < amount )
		return false;

	return TownSetTreasury( townName, currentAmount - amount );
}

function TownGrantTreasury( townName )
{
	return TownAddTreasury( townName, TownTreasuryGrantAmount() );
}

function TownGetTaxRate( townName )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return -1;

	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	const taxRate = ctrl.GetTag( TownTaxRateTag( townName ) );
	if( ctrl.GetTag( TownTaxRateSetTag( townName ) ) != 1 )
		return TownDefaultTaxRate();

	return TownParseNumber( taxRate, TownDefaultTaxRate() );
}

function TownSetTaxRate( townName, amount, forceChange )
{
	townName = TownNormalizeName( townName );
	if( TownGetDefault( townName ) == null )
		return false;

	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	amount = TownParseNumber( amount, TownDefaultTaxRate() );
	if( !TownTaxRateIsValid( amount ) )
		return TownSetLastError( "Tax must be one of: -30, -25, -20, -15, -10, -5, 0, 50, 100, 150, 200, 250, or 300." );
	if( !forceChange && TownTaxChangeRemaining( townName ) > 0 )
		return TownSetLastError( "This town's tax rate may only be changed once every 12 hours." );

	ctrl.SetTag( TownTaxRateTag( townName ), amount );
	ctrl.SetTag( TownTaxRateSetTag( townName ), 1 );
	ctrl.SetTag( TownLastTaxChangeTag( townName ), GetCurrentClock() );
	return true;
}

function RunTownTaxCycle()
{
	const ctrl = TownGetController();
	if( !ValidateObject( ctrl ) )
		return -1;

	let totalIncome = 0;
	for( let townName in factionTownDefaults )
	{
		let owner = TownGetOwner( townName );
		if( !TownIsFactionValid( owner ) )
			continue;

		const taxRate = TownGetTaxRate( townName );
		let income = Math.floor( factionTownDailyIncome * ( 100 + taxRate ) / 100 );
		if( income < 0 )
			income = 0;

		const available = TownGetTreasury( townName ) + income;
		let upkeep = TownFactionNpcUpkeep( townName, owner );
		while( upkeep > available && TownRemoveOneFactionNpcForUpkeep( townName, owner ) )
			upkeep = TownFactionNpcUpkeep( townName, owner );
		TownSetTreasury( townName, Math.max( 0, available - upkeep ) );
		totalIncome += income;
	}

	ctrl.SetTag( "faction_town_last_tax_cycle", GetCurrentClock() );
	ctrl.SetTag( "faction_town_last_tax_income", totalIncome );
	return totalIncome;
}

function TownFactionNpcUpkeep( townName, factionKey )
{
	factionTownIterateMode = "upkeep";
	factionTownIterateTown = TownNormalizeName( townName );
	factionTownIterateFaction = factionKey;
	factionTownUpkeepTotal = 0;
	IterateOver( "CHARACTER" );
	let upkeep = factionTownUpkeepTotal;
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	factionTownIterateFaction = "";
	factionTownUpkeepTotal = 0;
	return upkeep;
}

function TownRemoveOneFactionNpcForUpkeep( townName, factionKey )
{
	factionTownIterateMode = "removeupkeep";
	factionTownIterateTown = TownNormalizeName( townName );
	factionTownIterateFaction = factionKey;
	factionTownRemoveDone = false;
	const removed = IterateOver( "CHARACTER" );
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	factionTownIterateFaction = "";
	factionTownRemoveDone = false;
	return removed > 0;
}

function TownClearOffices( townName )
{
	factionTownIterateMode = "clearoffices";
	factionTownIterateTown = TownNormalizeName( townName );
	const cleared = IterateOver( "CHARACTER" );
	factionTownIterateMode = "";
	factionTownIterateTown = "";
	return cleared;
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

	for( let defaultTown in factionTownDefaults )
		pSock.SysMessage( TownDisplayName( defaultTown ) + ": treasury " + TownGetTreasury( defaultTown ) + " silver, tax " + TownGetTaxRate( defaultTown ) + ", owner " + TownFactionName( TownGetOwner( defaultTown ) ) );

	return true;
}

function TownTreasuryByFactionText( factionKey )
{
	if( !TownIsFactionValid( factionKey ) )
		return "Treasury: 0, Tax: 0";

	let totalTreasury = 0;
	let totalTax = 0;
	for( let townName in factionTownDefaults )
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
	factionTownLastError = "";
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

	const townRegion = GetTownRegion( townRegionId );
	if( !TownRegionIsLoaded( townRegion ) )
		return TownSetLastError( "Town region " + townRegionId + " is not loaded." );

	const oldOwner = TownGetOwner( townName );
	if( !TownSetOwner( townName, factionKey ) )
		return TownSetLastError( "Unable to store town owner for " + townName );

	let removedCount = 0;
	if( TownIsFactionValid( oldOwner ) && oldOwner !== factionKey )
	{
		removedCount = TownClearFactionNpcs( townName, oldOwner );
		TownClearOffices( townName );
	}

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

	let factionKey = iSigil.GetTag( "sigil_owner_faction" );
	let townName = iSigil.GetTag( "sigil_town" );
	let townRegionId = iSigil.GetTag( "town_region" );

	return ApplyTownControl( townName, factionKey, townRegionId );
}

function SyncTownControl()
{
	for( let townName in factionTownDefaults )
	{
		if( factionTownDefaults.hasOwnProperty( townName ) )
			ApplyTownControl( townName, TownGetOwner( townName ), factionTownDefaults[townName].region );
	}

	return true;
}

function ShowTownStatus( pSock )
{
	if( !pSock )
		return false;

	for( let townName in factionTownDefaults )
	{
		if( factionTownDefaults.hasOwnProperty( townName ) )
		{
			let townInfo = factionTownDefaults[townName];
			const townRegion = GetTownRegion( townInfo.region );
			const displayName = TownRegionIsLoaded( townRegion ) ? townRegion.name : townName;
			pSock.SysMessage( displayName + ": " + TownFactionName( TownGetOwner( townName ) ) );
		}
	}

	return true;
}
