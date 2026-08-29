/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_traps.js
// UOX3 Faction System - faction trap deeds and placed traps
// Script ID suggestion: 8506
// =============================================================================

const trapDamage = {
	EXPLOSION: 30,
	GAS: 15,
	SAW: 20,
	SPIKE: 10
};

const trapRange = {
	EXPLOSION: 2,
	GAS: 3,
	SAW: 1,
	SPIKE: 1
};

const trapEffect = {
	EXPLOSION: 0x36B0,
	GAS: 0x3709,
	SAW: 0x1D8,
	SPIKE: 0x1CF
};
const trapTownScriptId = 8509;
const trapPlacementRange = 12;

function TrapGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	let factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
}

function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return true;
	if( iUsed.GetTag( "trap_placed" ) == 1 )
	{
		if( iUsed.GetTag( "trap_faction" ) !== TrapGetFaction( pUser ) )
		{
			pUser.SysMessage( "Only a member of the owning faction may remove this trap." );
			return false;
		}
		if( iUsed.GetTag( "trap_placer_serial" ) != pUser.serial && !pUser.isGM )
		{
			pUser.SysMessage( "Only the trap's placer may remove it." );
			return false;
		}
		iUsed.Delete();
		pUser.SysMessage( "You remove the faction trap." );
		return false;
	}

	if( iUsed.GetTag( "trap_deed" ) != 1 )
		return true;

	let trapType = iUsed.GetTag( "trap_deed_type" );
	if( !trapDamage[trapType] )
	{
		pUser.SysMessage( "This trap deed is invalid." );
		return false;
	}

	let factionKey = TrapGetFaction( pUser );
	if( factionKey === "" )
	{
		pUser.SysMessage( "Only faction members may place faction traps." );
		return false;
	}

	pUser.SetTempTag( "placing_trap_type", trapType );
	pUser.SetTempTag( "placing_trap_faction", factionKey );
	pUser.SetTempTag( "placing_trap_deed", iUsed.serial );
	pUser.socket.CustomTarget( 30, "Where do you wish to place the trap?" );
	return false;
}

function onCallback30( pSock, target )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let trapType = pUser.GetTempTag( "placing_trap_type" );
	const trapFaction = pUser.GetTempTag( "placing_trap_faction" );
	const deedSerial = pUser.GetTempTag( "placing_trap_deed" );
	if( !trapDamage[trapType] )
		return;

	const deed = CalcItemFromSer( deedSerial );
	if( !ValidateObject( deed ) || deed.GetTag( "trap_deed" ) != 1 )
	{
		pUser.SysMessage( "That trap deed is no longer available." );
		return;
	}

	const targetX = ValidateObject( target ) ? target.x : pUser.x;
	const targetY = ValidateObject( target ) ? target.y : pUser.y;
	if( Math.abs( targetX - pUser.x ) > trapPlacementRange || Math.abs( targetY - pUser.y ) > trapPlacementRange )
	{
		pUser.SysMessage( "That location is too far away." );
		return;
	}
	let townName = TriggerEvent( trapTownScriptId, "TownNameForObject", pUser );
	if( townName === "" || TriggerEvent( trapTownScriptId, "TownGetOwner", townName ) !== trapFaction )
	{
		pUser.SysMessage( "Faction traps may only be placed in a town controlled by your faction." );
		return;
	}

	let trap = CreateDFNItem( pSock, pUser, "FACTION_TRAP_" + trapType, 1, "ITEM", false );
	if( !ValidateObject( trap ) )
		trap = CreateDFNItem( pSock, pUser, "FACTION_TRAP_BASE", 1, "ITEM", false );

	if( !ValidateObject( trap ) )
	{
		pUser.SysMessage( "The trap could not be created." );
		return;
	}
	deed.Delete();

	trap.SetTag( "trap_placed", 1 );
	trap.SetTag( "trap_type", trapType );
	trap.SetTag( "trap_faction", trapFaction );
	trap.SetTag( "trap_last_trigger", 0 );
	trap.SetTag( "trap_placer_serial", pUser.serial );
	trap.SetTag( "faction_town", townName );
	trap.movable = 3;

	if( ValidateObject( target ) )
		trap.Teleport( target.x, target.y, target.z, pUser.worldnumber, pUser.instanceID );
	else
		trap.Teleport( pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );

	pUser.SysMessage( "You have placed a faction trap." );
}

function onCollide( targSock, pColliding, iTrap )
{
	if( !ValidateObject( pColliding ) || !ValidateObject( iTrap ) )
		return false;
	if( iTrap.GetTag( "trap_placed" ) != 1 )
		return false;
	if( !pColliding.isChar || pColliding.dead )
		return false;

	const trapFaction = iTrap.GetTag( "trap_faction" );
	let victimFaction = TrapGetFaction( pColliding );
	if( victimFaction === trapFaction )
		return false;

	const now = GetCurrentClock();
	const lastTrigger = iTrap.GetTag( "trap_last_trigger" );
	if( lastTrigger > 0 && now - lastTrigger < 10000 )
		return false;
	iTrap.SetTag( "trap_last_trigger", now );

	let trapType = iTrap.GetTag( "trap_type" );
	let damage = trapDamage[trapType];
	if( !damage )
		damage = 10;

	iTrap.StaticEffect( trapEffect[trapType], 10, 0 );
	iTrap.SoundEffect( 0x22F, true );
	pColliding.SetTempTag( "trap_scan_type", trapType );
	pColliding.SetTempTag( "trap_scan_faction", trapFaction );
	AreaCharacterFunction( "TrapDamageCharacter", pColliding, trapRange[trapType], targSock );

	if( trapType === "EXPLOSION" || trapType === "SAW" )
		iTrap.Delete();

	return false;
}

function TrapDamageCharacter( srcChar, targetChar, pSock )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( targetChar ) )
		return false;
	if( targetChar.dead )
		return false;

	let trapType = srcChar.GetTempTag( "trap_scan_type" );
	const trapFaction = srcChar.GetTempTag( "trap_scan_faction" );
	if( TrapGetFaction( targetChar ) === trapFaction )
		return false;

	if( trapType === "GAS" )
		targetChar.poison = 2;

	targetChar.Damage( trapDamage[trapType], 0, srcChar, false );
	return true;
}

function onClick( pSock, iTrap )
{
	if( !ValidateObject( iTrap ) )
		return false;
	if( iTrap.GetTag( "trap_placed" ) != 1 )
		return false;

	pSock.SysMessage( "Faction Trap [" + iTrap.GetTag( "trap_faction" ) + "]" );
	return true;
}

