// =============================================================================
// factions_guards.js
// UOX3 Faction System - simple faction guard AI
// Script ID suggestion: 8503
// =============================================================================

var GuardScanRange = 10;
var GuardWarnDelay = 30000;
var GuardFactionTownScriptId = 8509;

function GuardGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	var factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
}

function GuardFactionName( factionKey )
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

function GuardGetGuardFaction( npcChar )
{
	if( !ValidateObject( npcChar ) )
		return "";

	var guardFaction = npcChar.GetTag( "guard_faction" );
	if( guardFaction === "TB" || guardFaction === "COM" || guardFaction === "MIN" || guardFaction === "SL" )
		return guardFaction;

	guardFaction = npcChar.GetTag( "faction" );
	if( guardFaction === "TB" || guardFaction === "COM" || guardFaction === "MIN" || guardFaction === "SL" )
		return guardFaction;

	return "";
}

function GuardIsActive( npcChar, guardFaction )
{
	if( !ValidateObject( npcChar ) || guardFaction === "" )
		return false;

	return TriggerEvent( GuardFactionTownScriptId, "TownIsObjectInControlledTownForFaction", npcChar, guardFaction );
}

function GuardIsEnemyTarget( npcChar, targetChar, guardFaction )
{
	if( !ValidateObject( npcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.serial == npcChar.serial || targetChar.dead || targetChar.npc )
		return false;

	var targetFaction = GuardGetFaction( targetChar );
	if( targetFaction === "" || targetFaction === guardFaction )
		return false;

	return true;
}

function GuardClearTarget( npcChar )
{
	if( !ValidateObject( npcChar ) )
		return;

	npcChar.target = null;
	npcChar.attacker = null;
	npcChar.atWar = false;
}

function GuardEngageTarget( npcChar, targetChar, guardFaction )
{
	if( !GuardIsEnemyTarget( npcChar, targetChar, guardFaction ) )
		return false;

	var now = GetCurrentClock();
	var warnedKey = "warned_" + targetChar.serial;
	var lastWarn = npcChar.GetTag( warnedKey );
	if( lastWarn == 0 || now - lastWarn > GuardWarnDelay )
	{
		npcChar.SetTag( warnedKey, now );
		npcChar.TextMessage( targetChar.name + " is an enemy of " + GuardFactionName( guardFaction ) + "!" );
	}

	npcChar.target = targetChar;
	npcChar.attacker = targetChar;
	npcChar.atWar = true;
	return true;
}

function onAISliver( npcChar )
{
	if( !ValidateObject( npcChar ) || npcChar.dead )
		return false;

	var guardFaction = GuardGetGuardFaction( npcChar );
	if( guardFaction === "" )
		return false;

	if( !GuardIsActive( npcChar, guardFaction ) )
	{
		GuardClearTarget( npcChar );
		return false;
	}

	if( ValidateObject( npcChar.target ) )
	{
		if( GuardIsEnemyTarget( npcChar, npcChar.target, guardFaction ) )
			return false;

		GuardClearTarget( npcChar );
	}

	npcChar.SetTempTag( "scan_guard_faction", guardFaction );
	AreaCharacterFunction( "GuardScanCharacter", npcChar, GuardScanRange, null );
	return false;
}

function GuardScanCharacter( npcChar, targetChar, pSock )
{
	if( !ValidateObject( npcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.serial == npcChar.serial || targetChar.dead || targetChar.npc )
		return false;

	var guardFaction = npcChar.GetTempTag( "scan_guard_faction" );
	if( guardFaction === "" )
		return false;

	return GuardEngageTarget( npcChar, targetChar, guardFaction );
}

function onAICombatTarget( pAttacker, pTarget )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( pTarget ) )
		return true;

	var guardFaction = GuardGetGuardFaction( pAttacker );
	if( guardFaction === "" )
		return true;

	if( !GuardIsActive( pAttacker, guardFaction ) )
		return false;

	if( !GuardIsEnemyTarget( pAttacker, pTarget, guardFaction ) )
		return false;

	return true;
}

function onDeath( npcChar, iCorpse )
{
	// Guard kill rewards are handled by factions_combat.js through onKill on the player.
	return false;
}

function onClick( pSock, npcChar )
{
	if( !ValidateObject( npcChar ) )
		return false;

	var guardFaction = GuardGetGuardFaction( npcChar );
	if( guardFaction !== "" && guardFaction != 0 )
	{
		var townOwner = TriggerEvent( GuardFactionTownScriptId, "TownOwnerForObject", npcChar );
		if( townOwner !== "" && townOwner !== guardFaction )
			pSock.SysMessage( npcChar.name + " [" + guardFaction + " Guard - inactive in enemy town]" );
		else
			pSock.SysMessage( npcChar.name + " [" + guardFaction + " Guard]" );
		return true;
	}
	return false;
}
