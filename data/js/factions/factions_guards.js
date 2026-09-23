/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_guards.js
// UOX3 Faction System - simple faction guard AI
// Script ID suggestion: 8503
// =============================================================================

const guardScanRange = 10;
const guardWarnDelay = 30000;
const guardFactionTownScriptId = 8509;

function GuardGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	let factionKey = pChar.GetTag( "faction" );
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

	let guardFaction = npcChar.GetTag( "guard_faction" );
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

	return TriggerEvent( guardFactionTownScriptId, "TownIsObjectInControlledTownForFaction", npcChar, guardFaction );
}

function GuardIsEnemyTarget( npcChar, targetChar, guardFaction )
{
	if( !ValidateObject( npcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.serial == npcChar.serial || targetChar.dead || targetChar.npc )
		return false;

	let targetFaction = GuardGetFaction( targetChar );
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

	const now = GetCurrentClock();
	const warnedKey = "warned_" + targetChar.serial;
	let lastWarn = npcChar.GetTag( warnedKey );
	if( lastWarn == 0 || now - lastWarn > guardWarnDelay )
	{
		npcChar.SetTag( warnedKey, now );
		npcChar.TextMessage( GetDictionaryEntry( 25362, targetChar.socket == null ? 0 : targetChar.socket.language ).replace( /%s/, targetChar.name ).replace( /%s/, GuardFactionName( guardFaction ) ) );
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

	let guardFaction = GuardGetGuardFaction( npcChar );
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
	AreaCharacterFunction( "GuardScanCharacter", npcChar, guardScanRange, null );
	return false;
}

function GuardScanCharacter( npcChar, targetChar, pSock )
{
	if( !ValidateObject( npcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.serial == npcChar.serial || targetChar.dead || targetChar.npc )
		return false;

	let guardFaction = npcChar.GetTempTag( "scan_guard_faction" );
	if( guardFaction === "" )
		return false;

	return GuardEngageTarget( npcChar, targetChar, guardFaction );
}

function onAICombatTarget( pAttacker, pTarget )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( pTarget ) )
		return true;

	let guardFaction = GuardGetGuardFaction( pAttacker );
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

	let guardFaction = GuardGetGuardFaction( npcChar );
	if( guardFaction !== "" && guardFaction != 0 )
	{
		const townOwner = TriggerEvent( guardFactionTownScriptId, "TownOwnerForObject", npcChar );
		if( townOwner !== "" && townOwner !== guardFaction )
			pSock.SysMessage( GetDictionaryEntry( 25293, pSock.language ).replace( /%s/, String( npcChar.name ) ).replace( /%s/, String( guardFaction ) ) );
		else
			pSock.SysMessage( GetDictionaryEntry( 25294, pSock.language ).replace( /%s/, String( npcChar.name ) ).replace( /%s/, String( guardFaction ) ) );
		return true;
	}
	return false;
}
