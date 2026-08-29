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
var CombatPlayerDataScriptId = 8513;
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
var CombatPlayerKillCooldown = 10800000;
var CombatKillPointDecayTime = 86400000;
var CombatBlockSameAccountRewards = false;
var CombatFactionNpcDefaultKillPoints = 1;
var CombatFactionNpcDefaultSilverMin = 5;
var CombatFactionNpcDefaultSilverMax = 25;
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

	var factionKey = TriggerEvent( CombatPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( CombatIsValidFaction( factionKey ) )
		return factionKey;

	return "";
}

function CombatGetKillPoints( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( CombatPlayerDataScriptId, "GetFactionValue", pChar, "killPoints", pChar.GetTag( "faction_kp" ) );
}

function CombatGetSilver( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( CombatPlayerDataScriptId, "GetFactionValue", pChar, "silver", pChar.GetTag( "faction_silver" ) );
}

function CombatGetRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	return TriggerEvent( CombatPlayerDataScriptId, "GetFactionValue", pChar, "rank", pChar.GetTag( "faction_rank" ) );
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
	TriggerEvent( CombatPlayerDataScriptId, "SetFactionValue", pChar, "rank", rank );
}

function CombatSetKillPoints( pChar, amount )
{
	if( !ValidateObject( pChar ) )
		return;

	if( amount < -6 )
		amount = -6;

	var factionData = TriggerEvent( CombatPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.killPoints = amount;
	factionData.rank = CombatRankForPoints( amount );
	TriggerEvent( CombatPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function CombatRankForPoints( killPoints )
{
	var rank = 0;
	for( var rankIndex = CombatRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= CombatRankPoints[rankIndex] )
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

	TriggerEvent( CombatPlayerDataScriptId, "SetFactionValue", pChar, "silver", amount );
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
	var lastDecay = TriggerEvent( CombatPlayerDataScriptId, "GetFactionValue", pChar, "kpDecayTime", pChar.GetTag( "faction_kp_decay_time" ) );
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
	TriggerEvent( CombatPlayerDataScriptId, "SetFactionValue", pChar, "kpDecayTime", now );
}

function FactionCombatOnLogout( pSock, pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return;

	if( CombatGetFaction( pChar ) !== "" )
		TriggerEvent( CombatPlayerDataScriptId, "SetFactionValue", pChar, "kpDecayTime", GetCurrentClock() );
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
	var lastKill = TriggerEvent( CombatPlayerDataScriptId, "GetRecentKillTime", pKiller, pKilled.serial );
	if( lastKill > 0 && ( now - lastKill ) < CombatPlayerKillCooldown )
	{
		pKiller.SysMessage( "You recently defeated this enemy and gain no faction reward." );
		return false;
	}

	return true;
}

function FactionCombatAwardPlayerKill( pKiller, pKilled )
{
	if( !FactionCombatCanRewardPlayerKill( pKiller, pKilled ) )
		return false;

	var victimKillPoints = CombatGetKillPoints( pKilled );
	if( victimKillPoints <= -6 )
	{
		pKiller.SysMessage( "This victim is not worth enough to receive faction kill points from." );
		return false;
	}
	TriggerEvent( CombatPlayerDataScriptId, "SetRecentKillTime", pKiller, pKilled.serial, GetCurrentClock() );
	var gainedKillPoints = 1;
	if( victimKillPoints > 10 )
		gainedKillPoints = Math.floor( victimKillPoints / 10 );
	if( gainedKillPoints < 1 )
		gainedKillPoints = 1;
	if( gainedKillPoints > 40 )
		gainedKillPoints = 40;

	var oldRank = CombatGetRank( pKiller );
	CombatAddKillPoints( pKiller, gainedKillPoints );
	CombatSetKillPoints( pKilled, victimKillPoints - gainedKillPoints );

	var silverReward = victimKillPoints > 0 ? gainedKillPoints * 40 : 0;
	if( silverReward > 0 )
		CombatAddSilver( pKiller, silverReward );
	CombatRecordPlayerKill( pKiller, pKilled, gainedKillPoints, silverReward );

	if( silverReward > 0 )
		pKiller.SysMessage( "You earned " + gainedKillPoints + " faction kill point(s) and " + silverReward + " silver." );
	else
		pKiller.SysMessage( "You earned " + gainedKillPoints + " faction kill point(s)." );
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

function CombatGetFactionNpcFaction( npcChar )
{
	if( !ValidateObject( npcChar ) || !npcChar.npc )
		return "";
	if( npcChar.GetTag( "faction_npc" ) != 1 )
		return "";

	var factionKey = npcChar.GetTag( "npc_faction" );
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

	var npcFaction = CombatGetFactionNpcFaction( pKilled );
	if( npcFaction === "" )
		return false;

	var killerFaction = CombatGetFaction( pKiller );
	if( killerFaction === "" )
		return false;
	if( killerFaction === npcFaction )
	{
		pKiller.SysMessage( "You gain no faction reward for killing an allied faction NPC." );
		return false;
	}

	var killPoints = CombatParseNumber( pKilled.GetTag( "npc_faction_kp" ), CombatFactionNpcDefaultKillPoints );
	var silverReward = CombatParseNumber( pKilled.GetTag( "npc_faction_silver" ), -1 );
	if( killPoints < 0 )
		killPoints = 0;
	if( silverReward < 0 )
		silverReward = RandomNumber( CombatFactionNpcDefaultSilverMin, CombatFactionNpcDefaultSilverMax );
	if( silverReward < 0 )
		silverReward = 0;

	if( killPoints <= 0 && silverReward <= 0 )
		return false;

	var oldRank = CombatGetRank( pKiller );
	if( killPoints > 0 )
		CombatAddKillPoints( pKiller, killPoints );
	if( silverReward > 0 )
		CombatAddSilver( pKiller, silverReward );

	if( killPoints > 0 && silverReward > 0 )
		pKiller.SysMessage( "You earned " + killPoints + " faction kill point(s) and " + silverReward + " silver for slaying an enemy faction NPC." );
	else if( killPoints > 0 )
		pKiller.SysMessage( "You earned " + killPoints + " faction kill point(s) for slaying an enemy faction NPC." );
	else
		pKiller.SysMessage( "You earned " + silverReward + " silver for slaying an enemy faction NPC." );

	if( CombatGetRank( pKiller ) > oldRank )
		pKiller.SysMessage( "Your faction rank is now " + CombatGetRankName( pKiller ) + "." );

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
