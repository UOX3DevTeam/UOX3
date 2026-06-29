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
var FactionItemCleanupMode = "";
var FactionItemCleanupTargetSerial = 0;
var FactionItemCleanupCount = 0;
var FactionItemCleanupSocket = null;

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

function ItemOwnerChar( iItem )
{
	if( !ValidateObject( iItem ) )
		return null;

	var packOwner = GetPackOwner( iItem, 0 );
	if( ValidateObject( packOwner ) && packOwner.isChar )
		return packOwner;

	if( ValidateObject( iItem.container ) && iItem.container.isChar )
		return iItem.container;

	return null;
}

function ItemShouldCleanForOwner( ownerChar, itemFaction )
{
	if( !ValidateObject( ownerChar ) || ownerChar.npc )
		return false;
	if( !ItemIsFactionValid( itemFaction ) )
		return false;

	if( FactionItemCleanupTargetSerial != 0 && ownerChar.serial != FactionItemCleanupTargetSerial )
		return false;

	return ownerChar.GetTag( "faction" ) !== itemFaction;
}

function ItemCleanFactionHorse( horseChar )
{
	if( !ValidateObject( horseChar ) || !horseChar.isChar || !horseChar.npc )
		return false;
	if( horseChar.GetTag( "faction_horse" ) != 1 && horseChar.GetTag( "faction_mount" ) != 1 )
		return false;

	var horseFaction = horseChar.GetTag( "item_faction" );
	if( !ItemIsFactionValid( horseFaction ) )
		horseFaction = horseChar.GetTag( "mount_faction" );
	if( !ItemIsFactionValid( horseFaction ) )
		return false;

	var ownerChar = horseChar.owner;
	if( !ValidateObject( ownerChar ) || ownerChar.npc )
		return false;
	if( FactionItemCleanupTargetSerial != 0 && ownerChar.serial != FactionItemCleanupTargetSerial )
		return false;
	if( ownerChar.GetTag( "faction" ) === horseFaction )
		return false;

	ownerChar.RemoveFollower( horseChar );
	if( ownerChar.controlSlotsUsed >= horseChar.controlSlots )
		ownerChar.controlSlotsUsed = ownerChar.controlSlotsUsed - horseChar.controlSlots;
	else
		ownerChar.controlSlotsUsed = 0;

	horseChar.Delete();
	FactionItemCleanupCount++;
	return true;
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

function onEquipAttempt( pChar, iEquipped )
{
	return onEquip( pChar, iEquipped );
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

function CleanupFactionOwnedObjects( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return 0;

	FactionItemCleanupMode = "items";
	FactionItemCleanupTargetSerial = pChar.serial;
	FactionItemCleanupCount = 0;
	IterateOver( "ITEM" );
	FactionItemCleanupMode = "horses";
	IterateOver( "CHARACTER" );
	var cleanCount = FactionItemCleanupCount;
	FactionItemCleanupMode = "";
	FactionItemCleanupTargetSerial = 0;
	FactionItemCleanupCount = 0;
	return cleanCount;
}

function CleanupInvalidFactionItems()
{
	FactionItemCleanupMode = "items";
	FactionItemCleanupTargetSerial = 0;
	FactionItemCleanupCount = 0;
	IterateOver( "ITEM" );
	FactionItemCleanupMode = "horses";
	IterateOver( "CHARACTER" );
	var cleanCount = FactionItemCleanupCount;
	FactionItemCleanupMode = "";
	FactionItemCleanupCount = 0;
	return cleanCount;
}

function ShowFactionItemCheck( pSock )
{
	if( pSock == null )
		return false;

	FactionItemCleanupSocket = pSock;
	FactionItemCleanupMode = "checkitems";
	FactionItemCleanupCount = 0;
	IterateOver( "ITEM" );
	FactionItemCleanupMode = "checkhorses";
	IterateOver( "CHARACTER" );
	if( FactionItemCleanupCount == 0 )
		pSock.SysMessage( "No invalid player-owned faction items or mounts found." );

	FactionItemCleanupMode = "";
	FactionItemCleanupSocket = null;
	FactionItemCleanupCount = 0;
	return true;
}

function onIterate( toCheck )
{
	if( FactionItemCleanupMode === "items" || FactionItemCleanupMode === "checkitems" )
	{
		if( !ValidateObject( toCheck ) || !toCheck.isItem || toCheck.GetTag( "faction_item" ) != 1 )
			return false;

		var itemFaction = toCheck.GetTag( "item_faction" );
		var ownerChar = ItemOwnerChar( toCheck );
		if( !ItemShouldCleanForOwner( ownerChar, itemFaction ) )
			return false;

		if( FactionItemCleanupMode === "checkitems" )
		{
			if( FactionItemCleanupSocket != null )
				FactionItemCleanupSocket.SysMessage( ownerChar.name + " has invalid faction item: " + toCheck.name + " [" + itemFaction + "]" );
		}
		else
		{
			toCheck.Delete();
		}

		FactionItemCleanupCount++;
		return true;
	}

	if( FactionItemCleanupMode === "horses" || FactionItemCleanupMode === "checkhorses" )
	{
		if( !ValidateObject( toCheck ) || !toCheck.isChar || !toCheck.npc )
			return false;
		if( toCheck.GetTag( "faction_horse" ) != 1 && toCheck.GetTag( "faction_mount" ) != 1 )
			return false;

		var horseFaction = toCheck.GetTag( "item_faction" );
		if( !ItemIsFactionValid( horseFaction ) )
			horseFaction = toCheck.GetTag( "mount_faction" );

		var horseOwner = toCheck.owner;
		if( !ItemShouldCleanForOwner( horseOwner, horseFaction ) )
			return false;

		if( FactionItemCleanupMode === "checkhorses" )
		{
			if( FactionItemCleanupSocket != null )
				FactionItemCleanupSocket.SysMessage( horseOwner.name + " has invalid faction mount: " + toCheck.name + " [" + horseFaction + "]" );
		}
		else
		{
			ItemCleanFactionHorse( toCheck );
			return true;
		}

		FactionItemCleanupCount++;
		return true;
	}

	return false;
}
