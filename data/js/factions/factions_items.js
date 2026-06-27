// =============================================================================
// factions_items.js
// UOX3 Faction System - faction equipment and item checks
// Script ID suggestion: 8507
// =============================================================================

function ItemGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	var factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;

	return "";
}

function onEquip( pChar, iEquipped )
{
	if( !ValidateObject( pChar ) || !ValidateObject( iEquipped ) )
		return true;

	if( iEquipped.GetTag( "faction_item" ) != 1 )
		return true;

	var requiredFaction = iEquipped.GetTag( "item_faction" );
	if( requiredFaction === "" || requiredFaction == 0 )
		return true;

	if( ItemGetFaction( pChar ) !== requiredFaction )
	{
		pChar.SysMessage( "Only members of that faction may equip this item." );
		return false;
	}

	return true;
}

function onUnEquip( pChar, iUnequipped )
{
	return true;
}

function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return true;

	if( iUsed.GetTag( "faction_horse" ) != 1 )
		return true;

	var requiredFaction = iUsed.GetTag( "item_faction" );
	if( requiredFaction !== "" && requiredFaction != 0 && ItemGetFaction( pUser ) !== requiredFaction )
	{
		pUser.SysMessage( "This faction war horse refuses you." );
		return false;
	}

	pUser.SysMessage( "This is a placeholder faction war horse deed. Replace this with your shard mount system if needed." );
	return false;
}

function onClick( pSock, iItem )
{
	if( !ValidateObject( iItem ) )
		return false;

	if( iItem.GetTag( "faction_item" ) != 1 )
		return false;

	var itemFaction = iItem.GetTag( "item_faction" );
	if( itemFaction !== "" && itemFaction != 0 )
	{
		pSock.SysMessage( iItem.name + " [" + itemFaction + "]" );
		return true;
	}

	return false;
}

function CommandRegistration()
{
	RegisterCommand( "spawnfitem", 5, true );
}

function command_SPAWNFITEM( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = cmdString.toUpperCase().split( " " );
	var itemType = parts[0];
	var factionKey = parts[1];

	if( factionKey !== "TB" && factionKey !== "COM" && factionKey !== "MIN" && factionKey !== "SL" )
	{
		pSock.SysMessage( "Usage: 'spawnfitem robe TB" );
		return;
	}

	var sectionId = "";
	if( itemType === "ROBE" )
		sectionId = "FACTION_ROBE_" + factionKey;
	else if( itemType === "HORSE" )
		sectionId = "FACTION_WAR_HORSE";
	else
	{
		pSock.SysMessage( "Usage: 'spawnfitem <robe|horse> <TB|COM|MIN|SL>" );
		return;
	}

	var newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", true );
	if( !ValidateObject( newItem ) )
	{
		pSock.SysMessage( "Unable to create faction item." );
		return;
	}

	newItem.SetTag( "faction_item", 1 );
	newItem.SetTag( "item_faction", factionKey );
	if( itemType === "HORSE" )
		newItem.SetTag( "faction_horse", 1 );

	pSock.SysMessage( "Faction item created." );
}
