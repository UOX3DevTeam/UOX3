/// <reference path="../definitions.d.ts" />
// @ts-check

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

const factionItemScriptId = 8507;
const factionItemBondingScriptId = 3107;
const factionItemMountRestrictionsScriptId = 3106;
let factionItemCleanupMode = "";
let factionItemCleanupTargetSerial = 0;
let factionItemCleanupCount = 0;
let factionItemCleanupSocket = null;

const factionItemMaxControlSlots = GetServerSetting( "MaxControlSlots" );
const factionItemMaxFollowers = GetServerSetting( "MaxFollowers" );
const factionItemPetBondingEnabled = GetServerSetting( "PetBondingEnabled" );

const factionItemHorseSections = {
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

	let factionKey = pChar.GetTag( "faction" );
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

	const packOwner = GetPackOwner( iItem, 0 );
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

	if( factionItemCleanupTargetSerial != 0 && ownerChar.serial != factionItemCleanupTargetSerial )
		return false;

	return ownerChar.GetTag( "faction" ) !== itemFaction;
}

function ItemCleanFactionHorse( horseChar )
{
	if( !ValidateObject( horseChar ) || !horseChar.isChar || !horseChar.npc )
		return false;
	if( horseChar.GetTag( "faction_horse" ) != 1 && horseChar.GetTag( "faction_mount" ) != 1 )
		return false;

	let horseFaction = horseChar.GetTag( "item_faction" );
	if( !ItemIsFactionValid( horseFaction ) )
		horseFaction = horseChar.GetTag( "mount_faction" );
	if( !ItemIsFactionValid( horseFaction ) )
		return false;

	const ownerChar = horseChar.owner;
	if( !ValidateObject( ownerChar ) || ownerChar.npc )
		return false;
	if( factionItemCleanupTargetSerial != 0 && ownerChar.serial != factionItemCleanupTargetSerial )
		return false;
	if( ownerChar.GetTag( "faction" ) === horseFaction )
		return false;

	ownerChar.RemoveFollower( horseChar );
	if( ownerChar.controlSlotsUsed >= horseChar.controlSlots )
		ownerChar.controlSlotsUsed = ownerChar.controlSlotsUsed - horseChar.controlSlots;
	else
		ownerChar.controlSlotsUsed = 0;

	horseChar.Delete();
	factionItemCleanupCount++;
	return true;
}

function ItemGetHorseControlSlots( iUsed )
{
	if( !ValidateObject( iUsed ) )
		return 1;

	let controlSlots = iUsed.morez;
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

	let controlSlots = ItemGetHorseControlSlots( iUsed );

	if( factionItemMaxControlSlots > 0 )
	{
		if( pUser.controlSlotsUsed + controlSlots > factionItemMaxControlSlots )
		{
			if( pUser.socket != null )
				pUser.socket.SysMessage( GetDictionaryEntry( 2390, pUser.socket.language ) );
			else
				pUser.SysMessage( "That would exceed your maximum pet control slots." );

			return false;
		}
	}
	else if( factionItemMaxFollowers > 0 && pUser.followerCount + 1 > factionItemMaxFollowers )
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

	if( !horse.HasScriptTrigger( factionItemScriptId ) )
		horse.AddScriptTrigger( factionItemScriptId );

	if( !horse.HasScriptTrigger( factionItemMountRestrictionsScriptId ) )
		horse.AddScriptTrigger( factionItemMountRestrictionsScriptId );

	if( factionItemMaxControlSlots > 0 )
	{
		pUser.controlSlotsUsed = pUser.controlSlotsUsed + horse.controlSlots;
	}

	pUser.AddFollower( horse );
	horse.Follow( pUser );

	if( factionItemPetBondingEnabled > 0 )
	{
		if( !horse.HasScriptTrigger( factionItemBondingScriptId ) )
			horse.AddScriptTrigger( factionItemBondingScriptId );

		TriggerEvent( factionItemBondingScriptId, "StartBonding", pUser, horse );
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

	const requiredFaction = iEquipped.GetTag( "item_faction" );
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

	const requiredFaction = iUsed.GetTag( "item_faction" );
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

	const horseSection = factionItemHorseSections[requiredFaction];
	const horse = SpawnNPC( horseSection, pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
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

	const requiredFaction = targChar.GetTag( "item_faction" );
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

	let factionKey = targetObj.GetTag( "item_faction" );
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

	factionItemCleanupMode = "items";
	factionItemCleanupTargetSerial = pChar.serial;
	factionItemCleanupCount = 0;
	IterateOver( "ITEM" );
	factionItemCleanupMode = "horses";
	IterateOver( "CHARACTER" );
	const cleanCount = factionItemCleanupCount;
	factionItemCleanupMode = "";
	factionItemCleanupTargetSerial = 0;
	factionItemCleanupCount = 0;
	return cleanCount;
}

function CleanupInvalidFactionItems()
{
	factionItemCleanupMode = "items";
	factionItemCleanupTargetSerial = 0;
	factionItemCleanupCount = 0;
	IterateOver( "ITEM" );
	factionItemCleanupMode = "horses";
	IterateOver( "CHARACTER" );
	const cleanCount = factionItemCleanupCount;
	factionItemCleanupMode = "";
	factionItemCleanupCount = 0;
	return cleanCount;
}

function ShowFactionItemCheck( pSock )
{
	if( pSock == null )
		return false;

	factionItemCleanupSocket = pSock;
	factionItemCleanupMode = "checkitems";
	factionItemCleanupCount = 0;
	IterateOver( "ITEM" );
	factionItemCleanupMode = "checkhorses";
	IterateOver( "CHARACTER" );
	if( factionItemCleanupCount == 0 )
		pSock.SysMessage( "No invalid player-owned faction items or mounts found." );

	factionItemCleanupMode = "";
	factionItemCleanupSocket = null;
	factionItemCleanupCount = 0;
	return true;
}

function onIterate( toCheck )
{
	if( factionItemCleanupMode === "items" || factionItemCleanupMode === "checkitems" )
	{
		if( !ValidateObject( toCheck ) || !toCheck.isItem || toCheck.GetTag( "faction_item" ) != 1 )
			return false;

		const itemFaction = toCheck.GetTag( "item_faction" );
		const ownerChar = ItemOwnerChar( toCheck );
		if( !ItemShouldCleanForOwner( ownerChar, itemFaction ) )
			return false;

		if( factionItemCleanupMode === "checkitems" )
		{
			if( factionItemCleanupSocket != null )
				factionItemCleanupSocket.SysMessage( ownerChar.name + " has invalid faction item: " + toCheck.name + " [" + itemFaction + "]" );
		}
		else
		{
			toCheck.Delete();
		}

		factionItemCleanupCount++;
		return true;
	}

	if( factionItemCleanupMode === "horses" || factionItemCleanupMode === "checkhorses" )
	{
		if( !ValidateObject( toCheck ) || !toCheck.isChar || !toCheck.npc )
			return false;
		if( toCheck.GetTag( "faction_horse" ) != 1 && toCheck.GetTag( "faction_mount" ) != 1 )
			return false;

		let horseFaction = toCheck.GetTag( "item_faction" );
		if( !ItemIsFactionValid( horseFaction ) )
			horseFaction = toCheck.GetTag( "mount_faction" );

		const horseOwner = toCheck.owner;
		if( !ItemShouldCleanForOwner( horseOwner, horseFaction ) )
			return false;

		if( factionItemCleanupMode === "checkhorses" )
		{
			if( factionItemCleanupSocket != null )
				factionItemCleanupSocket.SysMessage( horseOwner.name + " has invalid faction mount: " + toCheck.name + " [" + horseFaction + "]" );
		}
		else
		{
			ItemCleanFactionHorse( toCheck );
			return true;
		}

		factionItemCleanupCount++;
		return true;
	}

	return false;
}
