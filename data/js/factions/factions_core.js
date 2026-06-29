// =============================================================================
// factions_core.js
// UOX3 Faction System - shared helper script
// Script ID suggestion: 8500
// =============================================================================

var FactionNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

var FactionHues = {
	TB: 0x0028,
	COM: 0x010C,
	MIN: 0x0026,
	SL: 0x0455
};

var FactionRankNames = [
	"Soldier",
	"Scout",
	"Corporal",
	"Sergeant",
	"Lieutenant",
	"Captain",
	"Major",
	"Colonel",
	"General",
	"Commander"
];

var FactionRankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
var FactionLeaveDelay = 259200000;
var FactionMaxSilver = 100000;
var FactionPlayerDataScriptId = 8513;

function FactionIsValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function FactionName( factionKey )
{
	if( !FactionIsValid( factionKey ) )
		return "Unknown Faction";

	return FactionNames[factionKey];
}

function FactionHue( factionKey )
{
	if( !FactionIsValid( factionKey ) )
		return 0;

	return FactionHues[factionKey];
}

function GetFactionKey( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	var factionKey = TriggerEvent( FactionPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( FactionIsValid( factionKey ) )
		return factionKey;

	return "";
}

function GetFactionKillPoints( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( FactionPlayerDataScriptId, "GetFactionValue", pChar, "killPoints", pChar.GetTag( "faction_kp" ) );
}

function SetFactionKillPoints( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < 0 )
		amount = 0;

	var factionData = TriggerEvent( FactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.killPoints = amount;
	factionData.rank = FactionRankForPoints( amount );
	TriggerEvent( FactionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function AddFactionKillPoints( pChar, amount )
{
	SetFactionKillPoints( pChar, GetFactionKillPoints( pChar ) + amount );
}

function GetFactionSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( FactionPlayerDataScriptId, "GetFactionValue", pChar, "silver", pChar.GetTag( "faction_silver" ) );
}

function SetFactionSilver( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < 0 )
		amount = 0;
	if( amount > FactionMaxSilver )
		amount = FactionMaxSilver;

	TriggerEvent( FactionPlayerDataScriptId, "SetFactionValue", pChar, "silver", amount );
}

function AddFactionSilver( pChar, amount )
{
	SetFactionSilver( pChar, GetFactionSilver( pChar ) + amount );
}

function UpdateFactionRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return;

	var killPoints = GetFactionKillPoints( pChar );
	var rank = FactionRankForPoints( killPoints );
	var factionData = TriggerEvent( FactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.rank = rank;
	TriggerEvent( FactionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function FactionRankForPoints( killPoints )
{
	var rank = 0;
	for( var rankIndex = FactionRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= FactionRankPoints[rankIndex] )
		{
			rank = rankIndex;
			break;
		}
	}

	return rank;
}

function GetFactionRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( FactionPlayerDataScriptId, "GetFactionValue", pChar, "rank", pChar.GetTag( "faction_rank" ) );
}

function GetFactionRankName( pChar )
{
	var rank = GetFactionRank( pChar );
	if( rank < 0 || rank >= FactionRankNames.length )
		rank = 0;

	return FactionRankNames[rank];
}

function AreFactionEnemies( firstChar, secondChar )
{
	var firstFaction = GetFactionKey( firstChar );
	var secondFaction = GetFactionKey( secondChar );
	if( firstFaction === "" || secondFaction === "" )
		return false;

	return firstFaction !== secondFaction;
}

function AreFactionAllies( firstChar, secondChar )
{
	var firstFaction = GetFactionKey( firstChar );
	var secondFaction = GetFactionKey( secondChar );
	if( firstFaction === "" || secondFaction === "" )
		return false;

	return firstFaction === secondFaction;
}

function JoinFaction( pChar, factionKey )
{
	if( !ValidateObject( pChar ) )
		return false;

	if( !FactionIsValid( factionKey ) )
	{
		pChar.SysMessage( "Invalid faction." );
		return false;
	}

	var currentFaction = GetFactionKey( pChar );
	if( currentFaction !== "" )
	{
		pChar.SysMessage( "You are already in the " + FactionName( currentFaction ) + "." );
		return false;
	}

	var leaveTime = TriggerEvent( FactionPlayerDataScriptId, "GetFactionValue", pChar, "leaveTime", pChar.GetTag( "faction_leave_time" ) );
	if( leaveTime > 0 )
	{
		var remaining = FactionLeaveDelay - ( GetCurrentClock() - leaveTime );
		if( remaining > 0 )
		{
			var remainingHours = Math.ceil( remaining / 3600000 );
			pChar.SysMessage( "You must wait " + remainingHours + " more hour(s) before joining a faction." );
			return false;
		}
	}

	if( pChar.murdercount >= GetMurderThreshold() && ( factionKey === "TB" || factionKey === "COM" ) )
	{
		pChar.SysMessage( "Murderers may not join the " + FactionName( factionKey ) + "." );
		return false;
	}

	var factionData = TriggerEvent( FactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.faction = factionKey;
	factionData.joinTime = GetCurrentClock();
	factionData.killPoints = 0;
	factionData.silver = 0;
	factionData.rank = 0;
	factionData.leaveTime = 0;
	factionData.captures = 0;
	factionData.commander = false;
	factionData.role = "";
	factionData.roleFaction = "";
	factionData.roleSetAt = 0;
	factionData.recentKills = {};
	TriggerEvent( FactionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
	TriggerEvent( 8501, "FactionCombatAttachTrigger", pChar );
	pChar.SysMessage( "You have joined the " + FactionName( factionKey ) + "." );
	return true;
}

function LeaveFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	var factionKey = GetFactionKey( pChar );
	if( factionKey === "" )
	{
		pChar.SysMessage( "You are not in a faction." );
		return false;
	}

	var cleanedCount = TriggerEvent( 8507, "CleanupFactionOwnedObjects", pChar );
	var factionData = TriggerEvent( FactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.faction = "";
	factionData.killPoints = 0;
	factionData.silver = 0;
	factionData.rank = 0;
	factionData.captures = 0;
	factionData.commander = false;
	factionData.role = "";
	factionData.roleFaction = "";
	factionData.roleSetAt = 0;
	factionData.leaveTime = GetCurrentClock();
	factionData.recentKills = {};
	TriggerEvent( FactionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
	if( cleanedCount > 0 )
		pChar.SysMessage( cleanedCount + " faction item(s) or mount(s) were removed." );
	pChar.SysMessage( "You have left the " + FactionName( factionKey ) + "." );
	return true;
}

function GiveFactionRobe( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	var factionKey = GetFactionKey( pChar );
	if( factionKey === "" )
		return false;

	var robe = CreateDFNItem( pChar.socket, pChar, "FACTION_ROBE_" + factionKey, 1, "ITEM", true );
	if( !ValidateObject( robe ) )
	{
		pChar.SysMessage( "The faction robe could not be created." );
		return false;
	}

	robe.SetTag( "faction_item", 1 );
	robe.SetTag( "item_faction", factionKey );
	robe.colour = FactionHue( factionKey );
	return true;
}
