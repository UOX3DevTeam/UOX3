// =============================================================================
// factions_combat.js
// UOX3 Faction System - phase 1 combat rewards
// Script ID: 8501
//
// Login and logout are called from server/global.js through TriggerEvent.
// This script is also attached to players so onKill can award faction rewards.
// =============================================================================

var CombatScriptId = 8501;
var CombatTownScriptId = 8509;
var CombatRankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
var CombatRankNames = [
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
var CombatMaxSilver = 100000;
var CombatPlayerKillCooldown = 3600000;
var CombatKillPointDecayTime = 86400000;
var CombatBlockSameAccountRewards = false;
var CombatController = null;
var CombatIterateMode = "";

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
	var parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function CombatGetController()
{
	if( ValidateObject( CombatController ) )
		return CombatController;

	CombatController = null;
	CombatIterateMode = "controller";
	IterateOver( "ITEM" );
	CombatIterateMode = "";
	return CombatController;
}

function CombatGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	var factionKey = pChar.GetTag( "faction" );
	if( CombatIsValidFaction( factionKey ) )
		return factionKey;

	return "";
}

function CombatGetKillPoints( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return pChar.GetTag( "faction_kp" );
}

function CombatGetSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return pChar.GetTag( "faction_silver" );
}

function CombatGetRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return pChar.GetTag( "faction_rank" );
}

function CombatGetRankName( pChar )
{
	var rank = CombatGetRank( pChar );
	if( rank < 0 || rank >= CombatRankNames.length )
		rank = 0;

	return CombatRankNames[rank];
}

function CombatUpdateRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return;

	var killPoints = CombatGetKillPoints( pChar );
	var rank = 0;
	for( var rankIndex = CombatRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= CombatRankPoints[rankIndex] )
		{
			rank = rankIndex;
			break;
		}
	}
	pChar.SetTag( "faction_rank", rank );
}

function CombatSetKillPoints( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < 0 )
		amount = 0;

	pChar.SetTag( "faction_kp", amount );
	CombatUpdateRank( pChar );
}

function CombatRecordPlayerKill( pKiller, pKilled, gainedKillPoints, silverReward )
{
	if( !ValidateObject( pKiller ) || !ValidateObject( pKilled ) )
		return false;

	var killerFaction = CombatGetFaction( pKiller );
	var killedFaction = CombatGetFaction( pKilled );
	if( !CombatIsValidFaction( killerFaction ) || !CombatIsValidFaction( killedFaction ) )
		return false;

	var ctrl = CombatGetController();
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

	var townName = TriggerEvent( CombatTownScriptId, "TownNameForObject", pKiller );
	var locationText = "";
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
	if( amount > CombatMaxSilver )
		amount = CombatMaxSilver;

	pChar.SetTag( "faction_silver", amount );
}

function CombatAddSilver( pChar, amount )
{
	CombatSetSilver( pChar, CombatGetSilver( pChar ) + amount );
}

function FactionCombatAttachTrigger( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	if( !pChar.HasScriptTrigger( CombatScriptId ) )
		pChar.AddScriptTrigger( CombatScriptId );
}

function FactionCombatOnLogin( pSock, pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	FactionCombatAttachTrigger( pChar );

	if( CombatGetFaction( pChar ) === "" )
		return;

	var now = GetCurrentClock();
	var lastDecay = pChar.GetTag( "faction_kp_decay_time" );
	if( lastDecay > 0 )
	{
		var daysPassed = Math.floor( ( now - lastDecay ) / CombatKillPointDecayTime );
		if( daysPassed > 0 )
		{
			var killPoints = CombatGetKillPoints( pChar );
			if( killPoints > 0 )
			{
				var decayAmount = daysPassed;
				if( decayAmount > killPoints )
					decayAmount = killPoints;

				CombatSetKillPoints( pChar, killPoints - decayAmount );
				pChar.SysMessage( "Your faction standing decayed by " + decayAmount + " kill point(s)." );
			}
		}
	}
	pChar.SetTag( "faction_kp_decay_time", now );
}

function FactionCombatOnLogout( pSock, pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	if( CombatGetFaction( pChar ) !== "" )
		pChar.SetTag( "faction_kp_decay_time", GetCurrentClock() );
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

	var killerFaction = CombatGetFaction( pKiller );
	var killedFaction = CombatGetFaction( pKilled );
	if( killerFaction === "" || killedFaction === "" )
		return false;
	if( killerFaction === killedFaction )
		return false;

	if( CombatBlockSameAccountRewards && ValidateObject( pKiller.account ) && ValidateObject( pKilled.account ) )
	{
		if( pKiller.account.id == pKilled.account.id )
		{
			pKiller.SysMessage( "You gain no faction reward for killing a character on the same account." );
			return false;
		}
	}

	var now = GetCurrentClock();
	var lastKillTag = "faction_lastkill_" + pKilled.serial;
	var lastKill = pKiller.GetTag( lastKillTag );
	if( lastKill > 0 && ( now - lastKill ) < CombatPlayerKillCooldown )
	{
		pKiller.SysMessage( "You recently defeated this enemy and gain no faction reward." );
		return false;
	}

	pKiller.SetTag( lastKillTag, now );
	return true;
}

function FactionCombatAwardPlayerKill( pKiller, pKilled )
{
	if( !FactionCombatCanRewardPlayerKill( pKiller, pKilled ) )
		return false;

	var victimKillPoints = CombatGetKillPoints( pKilled );
	var gainedKillPoints = 1;
	if( victimKillPoints > 10 )
		gainedKillPoints = Math.floor( victimKillPoints / 10 );
	if( gainedKillPoints < 1 )
		gainedKillPoints = 1;

	var oldRank = CombatGetRank( pKiller );
	CombatAddKillPoints( pKiller, gainedKillPoints );
	CombatSetKillPoints( pKilled, victimKillPoints - gainedKillPoints );

	var silverReward = 20 + ( CombatGetRank( pKilled ) * 40 );
	CombatAddSilver( pKiller, silverReward );
	CombatRecordPlayerKill( pKiller, pKilled, gainedKillPoints, silverReward );

	pKiller.SysMessage( "You earned " + gainedKillPoints + " faction kill point(s) and " + silverReward + " silver." );
	pKilled.SysMessage( "You lost " + gainedKillPoints + " faction kill point(s)." );

	if( CombatGetRank( pKiller ) > oldRank )
		pKiller.SysMessage( "Your faction rank is now " + CombatGetRankName( pKiller ) + "." );

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

	var guardFaction = pKilled.GetTag( "guard_faction" );
	if( !CombatIsValidFaction( guardFaction ) )
		return false;

	var killerFaction = CombatGetFaction( pKiller );
	if( killerFaction === "" || killerFaction === guardFaction )
		return false;

	var silverReward = RandomNumber( 10, 50 );
	var oldRank = CombatGetRank( pKiller );
	CombatAddKillPoints( pKiller, 1 );
	CombatAddSilver( pKiller, silverReward );

	pKiller.SysMessage( "You earned 1 faction kill point and " + silverReward + " silver for slaying a faction guard." );
	if( CombatGetRank( pKiller ) > oldRank )
		pKiller.SysMessage( "Your faction rank is now " + CombatGetRankName( pKiller ) + "." );

	return true;
}

function onKill( pKiller, pKilled )
{
	if( FactionCombatAwardPlayerKill( pKiller, pKilled ) )
		return false;

	FactionCombatAwardGuardKill( pKiller, pKilled );
	return false;
}

function ShowFactionKillStats( pSock )
{
	if( pSock == null )
		return false;

	var ctrl = CombatGetController();
	if( !ValidateObject( ctrl ) )
	{
		pSock.SysMessage( "Faction controller was not found." );
		return false;
	}

	var factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( var i = 0; i < factionKeys.length; i++ )
	{
		var factionKey = factionKeys[i];
		var kills = CombatParseNumber( ctrl.GetTag( "combat_kills_" + factionKey ), 0 );
		var deaths = CombatParseNumber( ctrl.GetTag( "combat_deaths_" + factionKey ), 0 );
		var lastKiller = ctrl.GetTag( "combat_last_killer_" + factionKey );
		var lastVictim = ctrl.GetTag( "combat_last_victim_" + factionKey );
		var lastText = "None";
		if( lastKiller !== "" && lastKiller != 0 )
			lastText = lastKiller + " defeated " + lastVictim;

		pSock.SysMessage( CombatFactionName( factionKey ) + ": kills " + kills + ", deaths " + deaths + ", last " + lastText );
	}

	return true;
}

function FactionKillStatsText( factionKey )
{
	if( !CombatIsValidFaction( factionKey ) )
		return "Kills: 0, Deaths: 0";

	var ctrl = CombatGetController();
	if( !ValidateObject( ctrl ) )
		return "Kills: 0, Deaths: 0";

	return "Kills: " + CombatParseNumber( ctrl.GetTag( "combat_kills_" + factionKey ), 0 ) + ", Deaths: " + CombatParseNumber( ctrl.GetTag( "combat_deaths_" + factionKey ), 0 );
}

function ResetFactionKillStats()
{
	var ctrl = CombatGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	var factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( var i = 0; i < factionKeys.length; i++ )
	{
		var factionKey = factionKeys[i];
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
	if( CombatIterateMode === "controller" )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
		{
			CombatController = toCheck;
			return true;
		}
	}

	return false;
}
