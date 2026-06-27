// =============================================================================
// factions_combat.js
// UOX3 Faction System - phase 1 combat rewards
// Script ID: 8501
//
// Login and logout are called from server/global.js through TriggerEvent.
// This script is also attached to players so onKill can award faction rewards.
// =============================================================================

var CombatScriptId = 8501;
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

function CombatIsValidFaction( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
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
