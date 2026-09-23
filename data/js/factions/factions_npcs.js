/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_npcs.js
// UOX3 Faction System - generic faction NPC allegiance
// Script ID suggestion: 8512
// =============================================================================

const factionNpcScanRange = parseInt( GetServerSetting( "FACTIONNPCSCANRANGE" ), 10 );
const factionNpcWarnDelay = 30000;
const factionNpcTownScriptId = 8509;

function FactionNpcIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function FactionNpcName( factionKey )
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

function FactionNpcFactionForPlayer( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return "";

	let factionKey = pChar.GetTag( "faction" );
	if( FactionNpcIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function FactionNpcFactionForNpc( npcChar )
{
	if( !ValidateObject( npcChar ) || !npcChar.npc )
		return "";

	let factionKey = npcChar.GetTag( "npc_faction" );
	if( FactionNpcIsFactionValid( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "guard_faction" );
	if( FactionNpcIsFactionValid( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "vendor_faction" );
	if( FactionNpcIsFactionValid( factionKey ) )
		return factionKey;

	factionKey = npcChar.GetTag( "faction" );
	if( FactionNpcIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function FactionNpcIsActive( npcChar, npcFaction )
{
	if( !ValidateObject( npcChar ) || npcFaction === "" )
		return false;

	if( npcChar.GetTag( "npc_faction_require_town_control" ) == 1 )
		return TriggerEvent( factionNpcTownScriptId, "TownIsObjectInControlledTownForFaction", npcChar, npcFaction );

	return true;
}

function FactionNpcIsAggressive( npcChar )
{
	if( !ValidateObject( npcChar ) )
		return false;

	return ( npcChar.GetTag( "npc_faction_passive" ) != 1 );
}

function FactionNpcIsEnemyTarget( npcChar, targetChar, npcFaction )
{
	if( !ValidateObject( npcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.serial == npcChar.serial || targetChar.dead || targetChar.npc )
		return false;

	let targetFaction = FactionNpcFactionForPlayer( targetChar );
	if( targetFaction === "" || targetFaction === npcFaction )
		return false;

	return true;
}

function FactionNpcClearTarget( npcChar )
{
	if( !ValidateObject( npcChar ) )
		return;

	npcChar.target = null;
	npcChar.attacker = null;
	npcChar.atWar = false;
}

function FactionNpcEngageTarget( npcChar, targetChar, npcFaction )
{
	if( !FactionNpcIsEnemyTarget( npcChar, targetChar, npcFaction ) )
		return false;

	const now = GetCurrentClock();
	const warnedKey = "warned_" + targetChar.serial;
	let lastWarn = npcChar.GetTag( warnedKey );
	if( lastWarn == 0 || now - lastWarn > factionNpcWarnDelay )
	{
		npcChar.SetTag( warnedKey, now );
		npcChar.TextMessage( GetDictionaryEntry( 25362, targetChar.socket == null ? 0 : targetChar.socket.language ).replace( /%s/, targetChar.name ).replace( /%s/, FactionNpcName( npcFaction ) ) );
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

	let npcFaction = FactionNpcFactionForNpc( npcChar );
	if( npcFaction === "" )
		return false;

	if( !FactionNpcIsActive( npcChar, npcFaction ) )
	{
		FactionNpcClearTarget( npcChar );
		return false;
	}

	if( ValidateObject( npcChar.target ) )
	{
		if( FactionNpcIsEnemyTarget( npcChar, npcChar.target, npcFaction ) )
			return false;

		FactionNpcClearTarget( npcChar );
	}

	if( !FactionNpcIsAggressive( npcChar ) )
		return false;

	npcChar.SetTempTag( "scan_faction_npc_faction", npcFaction );
	AreaCharacterFunction( "FactionNpcScanCharacter", npcChar, factionNpcScanRange, null );
	return false;
}

function FactionNpcScanCharacter( npcChar, targetChar, pSock )
{
	if( !ValidateObject( npcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.serial == npcChar.serial || targetChar.dead || targetChar.npc )
		return false;

	let npcFaction = npcChar.GetTempTag( "scan_faction_npc_faction" );
	if( npcFaction === "" )
		return false;

	return FactionNpcEngageTarget( npcChar, targetChar, npcFaction );
}

function onAICombatTarget( pAttacker, pTarget )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( pTarget ) )
		return true;

	let npcFaction = FactionNpcFactionForNpc( pAttacker );
	if( npcFaction === "" )
		return true;

	if( !FactionNpcIsActive( pAttacker, npcFaction ) )
		return false;

	if( !FactionNpcIsEnemyTarget( pAttacker, pTarget, npcFaction ) )
		return false;

	return true;
}

function onClick( pSock, npcChar )
{
	if( !ValidateObject( npcChar ) )
		return false;

	let npcFaction = FactionNpcFactionForNpc( npcChar );
	if( npcFaction === "" )
		return false;

	let suffix = "Faction NPC";
	if( npcChar.GetTag( "npc_faction_passive" ) == 1 )
		suffix = "Faction NPC - passive";
	if( !FactionNpcIsActive( npcChar, npcFaction ) )
		suffix = "Faction NPC - inactive";

	pSock.SysMessage( GetDictionaryEntry( 25306, pSock.language ).replace( /%s/, String( npcChar.name ) ).replace( /%s/, String( FactionNpcName( npcFaction ) ) ).replace( /%s/, String( suffix ) ) );
	return true;
}
