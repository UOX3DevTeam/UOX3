/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_core.js
// UOX3 Faction System - shared helper script
// Script ID suggestion: 8500
// =============================================================================

const factionNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

const factionHues = {
	TB: 0x0028,
	COM: 0x010C,
	MIN: 0x0026,
	SL: 0x0455
};

const factionRankNames = [
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

const factionRankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
const factionLeaveDelay = parseInt( GetServerSetting( "FACTIONLEAVEDELAYHOURS" ), 10 ) * 3600000;
const factionMaxSilver = parseInt( GetServerSetting( "FACTIONMAXSILVER" ), 10 );
const factionPlayerDataScriptId = 8513;

function FactionIsValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function FactionName( factionKey )
{
	if( !FactionIsValid( factionKey ) )
		return "Unknown Faction";

	return factionNames[factionKey];
}

function FactionHue( factionKey )
{
	if( !FactionIsValid( factionKey ) )
		return 0;

	return factionHues[factionKey];
}

function GetFactionKey( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	let factionKey = TriggerEvent( factionPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( FactionIsValid( factionKey ) )
		return factionKey;

	return "";
}

function GetFactionKillPoints( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( factionPlayerDataScriptId, "GetFactionValue", pChar, "killPoints", pChar.GetTag( "faction_kp" ) );
}

function SetFactionKillPoints( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < -6 )
		amount = -6;

	let factionData = TriggerEvent( factionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.killPoints = amount;
	factionData.rank = FactionRankForPoints( amount );
	TriggerEvent( factionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function AddFactionKillPoints( pChar, amount )
{
	SetFactionKillPoints( pChar, GetFactionKillPoints( pChar ) + amount );
}

function GetFactionSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( factionPlayerDataScriptId, "GetFactionValue", pChar, "silver", pChar.GetTag( "faction_silver" ) );
}

function SetFactionSilver( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < 0 )
		amount = 0;
	if( amount > factionMaxSilver )
		amount = factionMaxSilver;

	TriggerEvent( factionPlayerDataScriptId, "SetFactionValue", pChar, "silver", amount );
}

function AddFactionSilver( pChar, amount )
{
	SetFactionSilver( pChar, GetFactionSilver( pChar ) + amount );
}

function UpdateFactionRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return;

	let killPoints = GetFactionKillPoints( pChar );
	let rank = FactionRankForPoints( killPoints );
	let factionData = TriggerEvent( factionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.rank = rank;
	TriggerEvent( factionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function FactionRankForPoints( killPoints )
{
	let rank = 0;
	for( let rankIndex = factionRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= factionRankPoints[rankIndex] )
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

	return TriggerEvent( factionPlayerDataScriptId, "GetFactionValue", pChar, "rank", pChar.GetTag( "faction_rank" ) );
}

function GetFactionRankName( pChar )
{
	let rank = GetFactionRank( pChar );
	if( rank < 0 || rank >= factionRankNames.length )
		rank = 0;

	return factionRankNames[rank];
}

function AreFactionEnemies( firstChar, secondChar )
{
	let firstFaction = GetFactionKey( firstChar );
	let secondFaction = GetFactionKey( secondChar );
	if( firstFaction === "" || secondFaction === "" )
		return false;

	return firstFaction !== secondFaction;
}

function AreFactionAllies( firstChar, secondChar )
{
	let firstFaction = GetFactionKey( firstChar );
	let secondFaction = GetFactionKey( secondChar );
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
		pChar.SysMessage( GetDictionaryEntry( 25100, pChar.socket == null ? 0 : pChar.socket.language ) );
		return false;
	}

	const currentFaction = GetFactionKey( pChar );
	if( currentFaction !== "" )
	{
		pChar.SysMessage( GetDictionaryEntry( 25101, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, FactionName( currentFaction ) ) );
		return false;
	}

	let leaveTime = TriggerEvent( factionPlayerDataScriptId, "GetFactionValue", pChar, "leaveTime", pChar.GetTag( "faction_leave_time" ) );
	if( leaveTime > 0 )
	{
		const remaining = factionLeaveDelay - ( GetCurrentClock() - leaveTime );
		if( remaining > 0 )
		{
			const remainingHours = Math.ceil( remaining / 3600000 );
			pChar.SysMessage( GetDictionaryEntry( 25102, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, remainingHours ) );
			return false;
		}
	}

	if( pChar.murdercount >= GetMurderThreshold() && ( factionKey === "TB" || factionKey === "COM" ) )
	{
		pChar.SysMessage( GetDictionaryEntry( 25103, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, FactionName( factionKey ) ) );
		return false;
	}

	let factionData = TriggerEvent( factionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
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
	factionData.roleTown = "";
	factionData.roleSetAt = 0;
	factionData.recentKills = {};
	TriggerEvent( factionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
	TriggerEvent( 8501, "FactionCombatAttachTrigger", pChar );
	pChar.SysMessage( GetDictionaryEntry( 25104, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, FactionName( factionKey ) ) );
	return true;
}

function LeaveFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	let factionKey = GetFactionKey( pChar );
	if( factionKey === "" )
	{
		pChar.SysMessage( GetDictionaryEntry( 25105, pChar.socket == null ? 0 : pChar.socket.language ) );
		return false;
	}

	let factionData = TriggerEvent( factionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	if( factionData.leaveTime > 0 )
	{
		const remaining = factionLeaveDelay - ( GetCurrentClock() - factionData.leaveTime );
		if( remaining > 0 )
		{
			pChar.SysMessage( GetDictionaryEntry( 25106, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, Math.ceil( remaining / 3600000 ) ) );
			return false;
		}
		return FinalizeFactionLeave( pChar, factionData );
	}

	factionData.leaveTime = GetCurrentClock();
	TriggerEvent( factionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
	pChar.SysMessage( GetDictionaryEntry( 25107, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, FactionName( factionKey ) ) );
	return true;
}

function FinalizeFactionLeave( pChar, factionData )
{
	if( !ValidateObject( pChar ) )
		return false;

	if( !factionData || typeof factionData != "object" )
		factionData = TriggerEvent( factionPlayerDataScriptId, "ReadFactionPlayerData", pChar );

	let factionKey = factionData.faction;
	if( !FactionIsValid( factionKey ) )
		return false;

	const cleanedCount = TriggerEvent( 8507, "CleanupFactionOwnedObjects", pChar );
	factionData.faction = "";
	factionData.killPoints = 0;
	factionData.silver = 0;
	factionData.rank = 0;
	factionData.captures = 0;
	factionData.commander = false;
	factionData.role = "";
	factionData.roleFaction = "";
	factionData.roleTown = "";
	factionData.roleSetAt = 0;
	factionData.leaveTime = 0;
	factionData.recentKills = {};
	TriggerEvent( factionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
	if( cleanedCount > 0 )
		pChar.SysMessage( GetDictionaryEntry( 25280, ( pChar.socket == null ? 0 : pChar.socket.language ) ).replace( /%s/, String( cleanedCount ) ) );
	pChar.SysMessage( GetDictionaryEntry( 25108, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, FactionName( factionKey ) ) );
	return true;
}

function FactionCoreOnLogin( pSock, pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return false;

	let factionData = TriggerEvent( factionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	if( FactionIsValid( factionData.faction ) && factionData.leaveTime > 0 )
	{
		const elapsed = GetCurrentClock() - factionData.leaveTime;
		if( elapsed >= factionLeaveDelay )
			return FinalizeFactionLeave( pChar, factionData );

		pChar.SysMessage( GetDictionaryEntry( 25109, pChar.socket == null ? 0 : pChar.socket.language ).replace( /%s/, Math.ceil( ( factionLeaveDelay - elapsed ) / 3600000 ) ) );
	}
	return true;
}

function GiveFactionRobe( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	let factionKey = GetFactionKey( pChar );
	if( factionKey === "" )
		return false;

	const robe = CreateDFNItem( pChar.socket, pChar, "FACTION_ROBE_" + factionKey, 1, "ITEM", true );
	if( !ValidateObject( robe ) )
	{
		pChar.SysMessage( GetDictionaryEntry( 25110, pChar.socket == null ? 0 : pChar.socket.language ) );
		return false;
	}

	robe.SetTag( "faction_item", 1 );
	robe.SetTag( "item_faction", factionKey );
	robe.colour = FactionHue( factionKey );
	return true;
}
