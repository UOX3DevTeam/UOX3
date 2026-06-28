// =============================================================================
// factions_commands.js
// UOX3 Faction System - GM testing commands
// COMMAND_SCRIPTS ID suggestion: 1071
// =============================================================================

var CommandMaxFactionSilver = 100000;
var CommandFactionElectionScriptId = 8508;

function CommandRegistration()
{
	RegisterCommand( "factionsilver", 5, true );
	RegisterCommand( "spawnfitem", 5, true );
	RegisterCommand( "spawntrapdeed", 5, true );

	RegisterCommand( "electionstart", 5, true );
	RegisterCommand( "electionvote", 5, true );
	RegisterCommand( "electionend", 5, true );
	RegisterCommand( "electionstatus", 0, true );
	RegisterCommand( "electionreset", 5, true );
}

function CommandIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function CommandClampSilver( amount )
{
	if( amount < 0 )
		amount = 0;
	if( amount > CommandMaxFactionSilver )
		amount = CommandMaxFactionSilver;

	return amount;
}

function command_FACTIONSILVER( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = cmdString.split( " " );
	var action = "";
	var amount = 0;

	if( parts.length >= 1 )
		action = parts[0].toLowerCase();
	if( parts.length >= 2 )
		amount = Number( parts[1] );

	if( action !== "add" && action !== "set" )
	{
		pSock.SysMessage( "Usage: 'factionsilver <add|set> <amount>" );
		return;
	}

	if( isNaN( amount ) )
	{
		pSock.SysMessage( "Amount must be a number." );
		return;
	}

	var currentSilver = pUser.GetTag( "faction_silver" );
	var newSilver = currentSilver;

	if( action === "add" )
		newSilver = currentSilver + amount;
	else
		newSilver = amount;

	newSilver = CommandClampSilver( newSilver );

	pUser.SetTag( "faction_silver", newSilver );
	pSock.SysMessage( "Faction silver set to " + newSilver + "." );
}

function CommandCreateFactionItem( pSock, pUser, itemType, factionKey )
{
	var sectionId = "";
	if( itemType === "ROBE" )
		sectionId = "FACTION_ROBE_" + factionKey;
	else if( itemType === "SHIELD" )
		sectionId = "FACTION_SHIELD_" + factionKey;
	else if( itemType === "HORSE" )
		sectionId = "FACTION_WAR_HORSE_DEED";
	else
		return null;

	var newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", true );
	if( ValidateObject( newItem ) )
	{
		newItem.SetTag( "faction_item", 1 );
		newItem.SetTag( "item_faction", factionKey );
		if( itemType === "HORSE" )
		{
			newItem.SetTag( "faction_horse_deed", 1 );
			newItem.name = factionKey + " Faction War Horse Deed";
		}
	}

	return newItem;
}

function command_SPAWNFITEM( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = cmdString.toUpperCase().split( " " );
	var itemType = parts[0];
	var factionKey = parts[1];

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'spawnfitem <robe|shield|horse> <TB|COM|MIN|SL>" );
		return;
	}

	var newItem = CommandCreateFactionItem( pSock, pUser, itemType, factionKey );
	if( !ValidateObject( newItem ) )
	{
		pSock.SysMessage( "Unable to create faction item." );
		return;
	}

	pSock.SysMessage( "Faction item created." );
}

function command_SPAWNTRAPDEED( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var trapType = cmdString.toUpperCase();
	if( trapType !== "EXPLOSION" && trapType !== "GAS" && trapType !== "SAW" && trapType !== "SPIKE" )
	{
		pSock.SysMessage( "Usage: 'spawntrapdeed <EXPLOSION|GAS|SAW|SPIKE>" );
		return;
	}

	var deed = CreateDFNItem( pSock, pUser, "FACTION_TRAP_DEED", 1, "ITEM", true );
	if( ValidateObject( deed ) )
	{
		deed.SetTag( "trap_deed", 1 );
		deed.SetTag( "trap_deed_type", trapType );
		deed.name = "Faction " + trapType + " Trap Deed";
		pSock.SysMessage( "Trap deed created." );
	}
	else
	{
		pSock.SysMessage( "Unable to create trap deed." );
	}
}

function command_ELECTIONSTART( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionstart <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "StartElection", factionKey ) )
		pSock.SysMessage( "Election started for " + factionKey + "." );
	else
		pSock.SysMessage( "Could not start election." );
}

function command_ELECTIONVOTE( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionvote <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "BeginVoting", factionKey ) )
		pSock.SysMessage( "Voting phase started for " + factionKey + "." );
	else
		pSock.SysMessage( "Could not start voting phase." );
}

function command_ELECTIONEND( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionend <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "ConcludeElection", factionKey ) )
		pSock.SysMessage( "Election concluded for " + factionKey + "." );
	else
		pSock.SysMessage( "Could not conclude election." );
}

function command_ELECTIONSTATUS( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var factionKey = pUser.GetTag( "faction" );
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "You are not in a faction." );
		return;
	}

	TriggerEvent( CommandFactionElectionScriptId, "ShowElectionStatus", pSock, factionKey );
}

function command_ELECTIONRESET( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionreset <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "ResetElection", factionKey ) )
		pSock.SysMessage( "Election state for " + factionKey + " has been reset." );
	else
		pSock.SysMessage( "Could not reset election." );
}
