/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_combat.js
// UOX3 Faction System - phase 1 combat rewards
// Script ID: 8501
//
// Login and logout are called from server/global.js through TriggerEvent.
// This script is also attached to players so onKill can award faction rewards.
// =============================================================================

const combatScriptId = 8501;
const combatTownScriptId = 8509;
const combatPlayerDataScriptId = 8513;
const combatRankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
const combatRankNames = [
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
const combatMaxSilver = parseInt( GetServerSetting( "FACTIONMAXSILVER" ), 10 );
const combatPlayerKillCooldown = parseInt( GetServerSetting( "FACTIONKILLCOOLDOWNMINUTES" ), 10 ) * 60000;
const combatKillPointDecayTime = parseInt( GetServerSetting( "FACTIONKILLPOINTDECAYHOURS" ), 10 ) * 3600000;
const combatBlockSameAccountRewards = ( GetServerSetting( "FACTIONBLOCKSAMEACCOUNTREWARDS" ) != 0 );
const combatFactionNpcDefaultKillPoints = parseInt( GetServerSetting( "FACTIONNPCKILLPOINTS" ), 10 );
const combatFactionNpcDefaultSilverMin = parseInt( GetServerSetting( "FACTIONNPCSILVERMIN" ), 10 );
const combatFactionNpcDefaultSilverMax = parseInt( GetServerSetting( "FACTIONNPCSILVERMAX" ), 10 );
let combatController = null;
let combatIterateMode = "";

function CombatIsValidFaction( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function CombatFactionName( factionKey )
{
	if( factionKey === "TB" )
		return "True Britannians";
	if( factionKey === "COM" )
		return "Council of Mages";
	if( factionKey === "MIN" )
		return "Minax";
	if( factionKey === "SL" )
		return "Shadowlords";

	return factionKey;
}

function CombatParseNumber( value, fallback )
{
	const parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function CombatGetController()
{
	if( ValidateObject( combatController ) )
		return combatController;

	combatController = null;
	combatIterateMode = "controller";
	IterateOver( "ITEM" );
	combatIterateMode = "";
	return combatController;
}

function CombatGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	let factionKey = TriggerEvent( combatPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( CombatIsValidFaction( factionKey ) )
		return factionKey;

	return "";
}

function CombatGetKillPoints( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( combatPlayerDataScriptId, "GetFactionValue", pChar, "killPoints", pChar.GetTag( "faction_kp" ) );
}

function CombatGetSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( combatPlayerDataScriptId, "GetFactionValue", pChar, "silver", pChar.GetTag( "faction_silver" ) );
}

function CombatGetRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( combatPlayerDataScriptId, "GetFactionValue", pChar, "rank", pChar.GetTag( "faction_rank" ) );
}

function CombatGetRankName( pChar )
{
	let rank = CombatGetRank( pChar );
	if( rank < 0 || rank >= combatRankNames.length )
		rank = 0;

	return combatRankNames[rank];
}

function CombatUpdateRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return;

	let killPoints = CombatGetKillPoints( pChar );
	let rank = 0;
	for( let rankIndex = combatRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= combatRankPoints[rankIndex] )
		{
			rank = rankIndex;
			break;
		}
	}
	TriggerEvent( combatPlayerDataScriptId, "SetFactionValue", pChar, "rank", rank );
}

function CombatSetKillPoints( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < -6 )
		amount = -6;

	const factionData = TriggerEvent( combatPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.killPoints = amount;
	factionData.rank = CombatRankForPoints( amount );
	TriggerEvent( combatPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function CombatRankForPoints( killPoints )
{
	let rank = 0;
	for( let rankIndex = combatRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= combatRankPoints[rankIndex] )
		{
			rank = rankIndex;
			break;
		}
	}

	return rank;
}

function CombatRecordPlayerKill( pKiller, pKilled, gainedKillPoints, silverReward )
{
	if( !ValidateObject( pKiller ) || !ValidateObject( pKilled ) )
		return false;

	let killerFaction = CombatGetFaction( pKiller );
	let killedFaction = CombatGetFaction( pKilled );
	if( !CombatIsValidFaction( killerFaction ) || !CombatIsValidFaction( killedFaction ) )
		return false;

	const ctrl = CombatGetController();
	if( ValidateObject( ctrl ) )
	{
		ctrl.SetTag( "combat_kills_" + killerFaction, CombatParseNumber( ctrl.GetTag( "combat_kills_" + killerFaction ), 0 ) + 1 );
		ctrl.SetTag( "combat_deaths_" + killedFaction, CombatParseNumber( ctrl.GetTag( "combat_deaths_" + killedFaction ), 0 ) + 1 );
		ctrl.SetTag( "combat_last_killer_" + killerFaction, pKiller.name );
		ctrl.SetTag( "combat_last_victim_" + killerFaction, pKilled.name );
		ctrl.SetTag( "combat_last_reward_kp_" + killerFaction, gainedKillPoints );
		ctrl.SetTag( "combat_last_reward_silver_" + killerFaction, silverReward );
		ctrl.SetTag( "combat_last_time_" + killerFaction, GetCurrentClock() );
	}

	const townName = TriggerEvent( combatTownScriptId, "TownNameForObject", pKiller );
	let locationText = "";
	if( townName !== "" )
		locationText = " near " + townName;

	BroadcastMessage( CombatFactionName( killerFaction ) + " defeated " + CombatFactionName( killedFaction ) + locationText + "." );
	return true;
}

function CombatAddKillPoints( pChar, amount )
{
	CombatSetKillPoints( pChar, CombatGetKillPoints( pChar ) + amount );
}

function CombatSetSilver( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < 0 )
		amount = 0;
	if( amount > combatMaxSilver )
		amount = combatMaxSilver;

	TriggerEvent( combatPlayerDataScriptId, "SetFactionValue", pChar, "silver", amount );
}

function CombatAddSilver( pChar, amount )
{
	CombatSetSilver( pChar, CombatGetSilver( pChar ) + amount );
}

function FactionCombatAttachTrigger( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	if( !pChar.HasScriptTrigger( combatScriptId ) )
		pChar.AddScriptTrigger( combatScriptId );
}

function FactionCombatOnLogin( pSock, pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	FactionCombatAttachTrigger( pChar );

	if( CombatGetFaction( pChar ) === "" )
		return;

	const now = GetCurrentClock();
	const lastDecay = TriggerEvent( combatPlayerDataScriptId, "GetFactionValue", pChar, "kpDecayTime", pChar.GetTag( "faction_kp_decay_time" ) );
	if( lastDecay > 0 )
	{
		const daysPassed = Math.floor( ( now - lastDecay ) / combatKillPointDecayTime );
		if( daysPassed > 0 )
		{
			let killPoints = CombatGetKillPoints( pChar );
			if( killPoints > 0 )
			{
				let decayAmount = daysPassed;
				if( decayAmount > killPoints )
					decayAmount = killPoints;

				CombatSetKillPoints( pChar, killPoints - decayAmount );
				pChar.SysMessage( GetDictionaryEntry( 25111, ( pChar.socket == null ? 0 : pChar.socket.language ) ).replace( /%s/, String( decayAmount ) ) );
			}
		}
	}
	TriggerEvent( combatPlayerDataScriptId, "SetFactionValue", pChar, "kpDecayTime", now );
}

function FactionCombatOnLogout( pSock, pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	if( CombatGetFaction( pChar ) !== "" )
		TriggerEvent( combatPlayerDataScriptId, "SetFactionValue", pChar, "kpDecayTime", GetCurrentClock() );
}

function FactionCombatCanRewardPlayerKill( pKiller, pKilled )
{
	if( !ValidateObject( pKiller ) || !ValidateObject( pKilled ) )
		return false;
	if( !pKiller.isChar || !pKilled.isChar )
		return false;
	if( pKiller.npc || pKilled.npc )
		return false;
	if( pKiller.serial == pKilled.serial )
		return false;

	let killerFaction = CombatGetFaction( pKiller );
	let killedFaction = CombatGetFaction( pKilled );
	if( killerFaction === "" || killedFaction === "" )
		return false;
	if( killerFaction === killedFaction )
		return false;

	if( combatBlockSameAccountRewards && ValidateObject( pKiller.account ) && ValidateObject( pKilled.account ) )
	{
		if( pKiller.account.id == pKilled.account.id )
		{
			pKiller.SysMessage( GetDictionaryEntry( 25112, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ) );
			return false;
		}
	}

	const now = GetCurrentClock();
	const lastKill = TriggerEvent( combatPlayerDataScriptId, "GetRecentKillTime", pKiller, pKilled.serial );
	if( lastKill > 0 && ( now - lastKill ) < combatPlayerKillCooldown )
	{
		pKiller.SysMessage( GetDictionaryEntry( 25113, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ) );
		return false;
	}

	return true;
}

function FactionCombatAwardPlayerKill( pKiller, pKilled )
{
	if( !FactionCombatCanRewardPlayerKill( pKiller, pKilled ) )
		return false;

	const victimKillPoints = CombatGetKillPoints( pKilled );
	if( victimKillPoints <= -6 )
	{
		pKiller.SysMessage( GetDictionaryEntry( 25114, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ) );
		return false;
	}
	TriggerEvent( combatPlayerDataScriptId, "SetRecentKillTime", pKiller, pKilled.serial, GetCurrentClock() );
	let gainedKillPoints = 1;
	if( victimKillPoints > 10 )
		gainedKillPoints = Math.floor( victimKillPoints / 10 );
	if( gainedKillPoints < 1 )
		gainedKillPoints = 1;
	if( gainedKillPoints > 40 )
		gainedKillPoints = 40;

	const oldRank = CombatGetRank( pKiller );
	CombatAddKillPoints( pKiller, gainedKillPoints );
	CombatSetKillPoints( pKilled, victimKillPoints - gainedKillPoints );

	let silverReward = victimKillPoints > 0 ? gainedKillPoints * 40 : 0;
	if( silverReward > 0 )
		CombatAddSilver( pKiller, silverReward );
	CombatRecordPlayerKill( pKiller, pKilled, gainedKillPoints, silverReward );

	if( silverReward > 0 )
		pKiller.SysMessage( GetDictionaryEntry( 25115, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( gainedKillPoints ) ).replace( /%s/, String( silverReward ) ) );
	else
		pKiller.SysMessage( GetDictionaryEntry( 25116, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( gainedKillPoints ) ) );
	pKilled.SysMessage( GetDictionaryEntry( 25117, ( pKilled.socket == null ? 0 : pKilled.socket.language ) ).replace( /%s/, String( gainedKillPoints ) ) );

	if( CombatGetRank( pKiller ) > oldRank )
		pKiller.SysMessage( GetDictionaryEntry( 25118, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( CombatGetRankName( pKiller ) ) ) );

	pKiller.StaticEffect( 0x373A, 10, 16 );
	return true;
}

function FactionCombatAwardGuardKill( pKiller, pKilled )
{
	if( !ValidateObject( pKiller ) || !ValidateObject( pKilled ) )
		return false;
	if( !pKiller.isChar || pKiller.npc )
		return false;
	if( !pKilled.npc )
		return false;

	const guardFaction = pKilled.GetTag( "guard_faction" );
	if( !CombatIsValidFaction( guardFaction ) )
		return false;

	let killerFaction = CombatGetFaction( pKiller );
	if( killerFaction === "" || killerFaction === guardFaction )
		return false;

	let silverReward = RandomNumber( 10, 50 );
	const oldRank = CombatGetRank( pKiller );
	CombatAddKillPoints( pKiller, 1 );
	CombatAddSilver( pKiller, silverReward );

	pKiller.SysMessage( GetDictionaryEntry( 25119, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( silverReward ) ) );
	if( CombatGetRank( pKiller ) > oldRank )
		pKiller.SysMessage( GetDictionaryEntry( 25118, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( CombatGetRankName( pKiller ) ) ) );

	return true;
}

function CombatGetFactionNpcFaction( npcChar )
{
	if( !ValidateObject( npcChar ) || !npcChar.npc )
		return "";
	if( npcChar.GetTag( "faction_npc" ) != 1 )
		return "";

	let factionKey = npcChar.GetTag( "npc_faction" );
	if( CombatIsValidFaction( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "faction" );
	if( CombatIsValidFaction( factionKey ) )
		return factionKey;

	return "";
}

function FactionCombatAwardFactionNpcKill( pKiller, pKilled )
{
	if( !ValidateObject( pKiller ) || !ValidateObject( pKilled ) )
		return false;
	if( !pKiller.isChar || pKiller.npc || !pKilled.npc )
		return false;
	if( pKilled.GetTag( "npc_faction_no_reward" ) == 1 )
		return false;

	let npcFaction = CombatGetFactionNpcFaction( pKilled );
	if( npcFaction === "" )
		return false;

	let killerFaction = CombatGetFaction( pKiller );
	if( killerFaction === "" )
		return false;
	if( killerFaction === npcFaction )
	{
		pKiller.SysMessage( GetDictionaryEntry( 25120, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ) );
		return false;
	}

	let killPoints = CombatParseNumber( pKilled.GetTag( "npc_faction_kp" ), combatFactionNpcDefaultKillPoints );
	let silverReward = CombatParseNumber( pKilled.GetTag( "npc_faction_silver" ), -1 );
	if( killPoints < 0 )
		killPoints = 0;
	if( silverReward < 0 )
		silverReward = RandomNumber( combatFactionNpcDefaultSilverMin, combatFactionNpcDefaultSilverMax );
	if( silverReward < 0 )
		silverReward = 0;

	if( killPoints <= 0 && silverReward <= 0 )
		return false;

	const oldRank = CombatGetRank( pKiller );
	if( killPoints > 0 )
		CombatAddKillPoints( pKiller, killPoints );
	if( silverReward > 0 )
		CombatAddSilver( pKiller, silverReward );

	if( killPoints > 0 && silverReward > 0 )
		pKiller.SysMessage( GetDictionaryEntry( 25121, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( killPoints ) ).replace( /%s/, String( silverReward ) ) );
	else if( killPoints > 0 )
		pKiller.SysMessage( GetDictionaryEntry( 25122, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( killPoints ) ) );
	else
		pKiller.SysMessage( GetDictionaryEntry( 25123, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( silverReward ) ) );

	if( CombatGetRank( pKiller ) > oldRank )
		pKiller.SysMessage( GetDictionaryEntry( 25118, ( pKiller.socket == null ? 0 : pKiller.socket.language ) ).replace( /%s/, String( CombatGetRankName( pKiller ) ) ) );

	return true;
}

function onKill( pKiller, pKilled )
{
	if( FactionCombatAwardPlayerKill( pKiller, pKilled ) )
		return false;

	if( FactionCombatAwardGuardKill( pKiller, pKilled ) )
		return false;

	FactionCombatAwardFactionNpcKill( pKiller, pKilled );
	return false;
}

function ShowFactionKillStats( pSock )
{
	if( pSock == null )
		return false;

	const ctrl = CombatGetController();
	if( !ValidateObject( ctrl ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25124, pSock.language ) );
		return false;
	}

	const factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( let i = 0; i < factionKeys.length; i++ )
	{
		let factionKey = factionKeys[i];
		const kills = CombatParseNumber( ctrl.GetTag( "combat_kills_" + factionKey ), 0 );
		const deaths = CombatParseNumber( ctrl.GetTag( "combat_deaths_" + factionKey ), 0 );
		const lastKiller = ctrl.GetTag( "combat_last_killer_" + factionKey );
		const lastVictim = ctrl.GetTag( "combat_last_victim_" + factionKey );
		let lastText = "None";
		if( lastKiller !== "" && lastKiller != 0 )
			lastText = lastKiller + " defeated " + lastVictim;

		pSock.SysMessage( GetDictionaryEntry( 25125, pSock.language ).replace( /%s/, String( CombatFactionName( factionKey ) ) ).replace( /%s/, String( kills ) ).replace( /%s/, String( deaths ) ).replace( /%s/, String( lastText ) ) );
	}

	return true;
}

function FactionKillStatsText( factionKey )
{
	if( !CombatIsValidFaction( factionKey ) )
		return "Kills: 0, Deaths: 0";

	const ctrl = CombatGetController();
	if( !ValidateObject( ctrl ) )
		return "Kills: 0, Deaths: 0";

	return "Kills: " + CombatParseNumber( ctrl.GetTag( "combat_kills_" + factionKey ), 0 ) + ", Deaths: " + CombatParseNumber( ctrl.GetTag( "combat_deaths_" + factionKey ), 0 );
}

function ResetFactionKillStats()
{
	const ctrl = CombatGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	const factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( let i = 0; i < factionKeys.length; i++ )
	{
		let factionKey = factionKeys[i];
		ctrl.SetTag( "combat_kills_" + factionKey, 0 );
		ctrl.SetTag( "combat_deaths_" + factionKey, 0 );
		ctrl.SetTag( "combat_last_killer_" + factionKey, "" );
		ctrl.SetTag( "combat_last_victim_" + factionKey, "" );
		ctrl.SetTag( "combat_last_reward_kp_" + factionKey, 0 );
		ctrl.SetTag( "combat_last_reward_silver_" + factionKey, 0 );
		ctrl.SetTag( "combat_last_time_" + factionKey, 0 );
	}

	return true;
}

function onIterate( toCheck )
{
	if( combatIterateMode === "controller" )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
		{
			combatController = toCheck;
			return true;
		}
	}

	return false;
}
