// =============================================================================
// factions_items.js
// UOX3 Faction System - phase 3 faction equipment and war horses
// Script ID: 8507
//
// War horse deeds use the same general pattern as static_horse.js:
// - Check faction access
// - Check control slot or follower limits
// - Spawn the NPC
// - Set owner
// - Add follower
// - Follow owner
// - Add bonding when enabled
// - Delete the deed
// =============================================================================

var FactionItemScriptId = 8507;
var FactionItemBondingScriptId = 3107;
var FactionItemMountRestrictionsScriptId = 3106;

var FactionItemMaxControlSlots = GetServerSetting( "MaxControlSlots" );
var FactionItemMaxFollowers = GetServerSetting( "MaxFollowers" );
var FactionItemPetBondingEnabled = GetServerSetting( "PetBondingEnabled" );

var FactionItemHorseSections = {
	TB: "FACTION_WAR_HORSE_TB",
	COM: "FACTION_WAR_HORSE_COM",
	MIN: "FACTION_WAR_HORSE_MIN",
	SL: "FACTION_WAR_HORSE_SL"
};

function ItemIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function ItemGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	var factionKey = pChar.GetTag( "faction" );
	if( ItemIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function ItemCanUseFactionObject( pChar, factionKey )
{
	if( !ValidateObject( pChar ) )
		return false;

	if( !ItemIsFactionValid( factionKey ) )
		return true;

	return ItemGetFaction( pChar ) === factionKey;
}

function ItemGetHorseControlSlots( iUsed )
{
	if( !ValidateObject( iUsed ) )
		return 1;

	var controlSlots = iUsed.morez;
	if( controlSlots <= 0 )
		controlSlots = iUsed.GetTag( "control_slots" );
	if( controlSlots <= 0 )
		controlSlots = 1;

	return controlSlots;
}

function ItemCanControlHorse( pUser, iUsed )
{
	if( !ValidateObject( pUser ) )
		return false;

	var controlSlots = ItemGetHorseControlSlots( iUsed );

	if( FactionItemMaxControlSlots > 0 )
	{
		if( pUser.controlSlotsUsed + controlSlots > FactionItemMaxControlSlots )
		{
			if( pUser.socket != null )
				pUser.socket.SysMessage( GetDictionaryEntry( 2390, pUser.socket.language ) );
			else
				pUser.SysMessage( "That would exceed your maximum pet control slots." );

			return false;
		}
	}
	else if( FactionItemMaxFollowers > 0 && pUser.followerCount + 1 > FactionItemMaxFollowers )
	{
		if( pUser.socket != null )
			pUser.socket.SysMessage( GetDictionaryEntry( 2400, pUser.socket.language ) );
		else
			pUser.SysMessage( "You have too many followers already." );

		return false;
	}

	return true;
}

function ItemSetupFactionHorse( pUser, horse, factionKey, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( horse ) )
		return false;

	horse.owner = pUser;
	horse.SetTag( "faction_horse", 1 );
	horse.SetTag( "faction_mount", 1 );
	horse.SetTag( "faction_item", 1 );
	horse.SetTag( "item_faction", factionKey );
	horse.SetTag( "mount_faction", factionKey );

	if( !horse.HasScriptTrigger( FactionItemScriptId ) )
		horse.AddScriptTrigger( FactionItemScriptId );

	if( !horse.HasScriptTrigger( FactionItemMountRestrictionsScriptId ) )
		horse.AddScriptTrigger( FactionItemMountRestrictionsScriptId );

	if( FactionItemMaxControlSlots > 0 )
	{
		pUser.controlSlotsUsed = pUser.controlSlotsUsed + horse.controlSlots;
	}

	pUser.AddFollower( horse );
	horse.Follow( pUser );

	if( FactionItemPetBondingEnabled > 0 )
	{
		if( !horse.HasScriptTrigger( FactionItemBondingScriptId ) )
			horse.AddScriptTrigger( FactionItemBondingScriptId );

		TriggerEvent( FactionItemBondingScriptId, "StartBonding", pUser, horse );
	}

	pUser.SoundEffect( 0x0215, true );
	return true;
}

function onEquip( pChar, iEquipped )
{
	if( !ValidateObject( pChar ) || !ValidateObject( iEquipped ) )
		return true;

	if( iEquipped.GetTag( "faction_item" ) != 1 )
		return true;

	var requiredFaction = iEquipped.GetTag( "item_faction" );
	if( !ItemCanUseFactionObject( pChar, requiredFaction ) )
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

	if( iUsed.GetTag( "faction_horse_deed" ) != 1 )
		return true;

	var requiredFaction = iUsed.GetTag( "item_faction" );
	if( !ItemIsFactionValid( requiredFaction ) )
	{
		pUser.SysMessage( "This faction war horse deed has no faction assigned." );
		return false;
	}

	if( ItemGetFaction( pUser ) !== requiredFaction )
	{
		pUser.SysMessage( "Only members of that faction may use this war horse deed." );
		return false;
	}

	if( !ItemCanControlHorse( pUser, iUsed ) )
		return false;

	var horseSection = FactionItemHorseSections[requiredFaction];
	var horse = SpawnNPC( horseSection, pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( !ValidateObject( horse ) )
	{
		pUser.SysMessage( "The faction war horse could not be created." );
		return false;
	}

	if( !ItemSetupFactionHorse( pUser, horse, requiredFaction, iUsed ) )
	{
		horse.Delete();
		pUser.SysMessage( "The faction war horse could not be controlled." );
		return false;
	}

	iUsed.Delete();
	pUser.SysMessage( "You summon a faction war horse." );
	return false;
}

function onCharDoubleClick( pUser, targChar, nonMouseClickEvent )
{
	if( !ValidateObject( pUser ) || !ValidateObject( targChar ) )
		return true;

	if( targChar.GetTag( "faction_horse" ) != 1 && targChar.GetTag( "faction_mount" ) != 1 )
		return true;

	var requiredFaction = targChar.GetTag( "item_faction" );
	if( !ItemCanUseFactionObject( pUser, requiredFaction ) )
	{
		pUser.SysMessage( "This faction war horse refuses you." );
		return false;
	}

	return true;
}

function onReleasePet( pUser, pet )
{
	return true;
}

function onClick( pSock, targetObj )
{
	if( !ValidateObject( targetObj ) )
		return false;

	if( targetObj.GetTag( "faction_item" ) != 1 && targetObj.GetTag( "faction_horse" ) != 1 && targetObj.GetTag( "faction_mount" ) != 1 )
		return false;

	var factionKey = targetObj.GetTag( "item_faction" );
	if( factionKey !== "" && factionKey != 0 )
	{
		pSock.SysMessage( targetObj.name + " [" + factionKey + "]" );
		return true;
	}

	return false;
}
