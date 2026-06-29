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

function onAISliver( npcChar )
{
	if( !ValidateObject( npcChar ) || npcChar.dead )
		return false;

	var guardFaction = npcChar.GetTag( "guard_faction" );
	if( guardFaction === "" || guardFaction == 0 )
		return false;

	if( !TriggerEvent( GuardFactionTownScriptId, "TownIsObjectInControlledTownForFaction", npcChar, guardFaction ) )
		return false;

	if( ValidateObject( npcChar.attacker ) )
		return false;

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
	var targetFaction = GuardGetFaction( targetChar );
	if( targetFaction === "" || targetFaction === guardFaction )
		return false;

	var now = GetCurrentClock();
	var warnedKey = "warned_" + targetChar.serial;
	var lastWarn = npcChar.GetTag( warnedKey );
	if( lastWarn == 0 || now - lastWarn > GuardWarnDelay )
	{
		npcChar.SetTag( warnedKey, now );
		npcChar.TextMessage( "Leave this area, " + targetChar.name + "." );
		return true;
	}

	npcChar.attacker = targetChar;
	npcChar.atWar = true;
	return true;
}

function onAICombatTarget( pAttacker, pTarget )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( pTarget ) )
		return true;

	var guardFaction = pAttacker.GetTag( "guard_faction" );
	if( guardFaction === "" || guardFaction == 0 )
		return true;

	if( !TriggerEvent( GuardFactionTownScriptId, "TownIsObjectInControlledTownForFaction", pAttacker, guardFaction ) )
		return false;

	var targetFaction = GuardGetFaction( pTarget );
	if( targetFaction === "" || targetFaction === guardFaction )
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

	var guardFaction = npcChar.GetTag( "guard_faction" );
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
