// =============================================================================
// factions_traps.js
// UOX3 Faction System - faction trap deeds and placed traps
// Script ID suggestion: 8506
// =============================================================================

var TrapDamage = {
	EXPLOSION: 30,
	GAS: 15,
	SAW: 20,
	SPIKE: 10
};

var TrapRange = {
	EXPLOSION: 2,
	GAS: 3,
	SAW: 1,
	SPIKE: 1
};

var TrapEffect = {
	EXPLOSION: 0x36B0,
	GAS: 0x3709,
	SAW: 0x1D8,
	SPIKE: 0x1CF
};

function TrapGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	var factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
}

function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return true;

	if( iUsed.GetTag( "trap_deed" ) != 1 )
		return true;

	var trapType = iUsed.GetTag( "trap_deed_type" );
	if( !TrapDamage[trapType] )
	{
		pUser.SysMessage( "This trap deed is invalid." );
		return false;
	}

	var factionKey = TrapGetFaction( pUser );
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
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var trapType = pUser.GetTempTag( "placing_trap_type" );
	var trapFaction = pUser.GetTempTag( "placing_trap_faction" );
	var deedSerial = pUser.GetTempTag( "placing_trap_deed" );
	if( !TrapDamage[trapType] )
		return;

	var deed = CalcItemFromSer( deedSerial );
	if( ValidateObject( deed ) )
		deed.Delete();

	var trap = CreateDFNItem( pSock, pUser, "FACTION_TRAP_" + trapType, 1, "ITEM", false );
	if( !ValidateObject( trap ) )
		trap = CreateDFNItem( pSock, pUser, "FACTION_TRAP_BASE", 1, "ITEM", false );

	if( !ValidateObject( trap ) )
	{
		pUser.SysMessage( "The trap could not be created." );
		return;
	}

	trap.SetTag( "trap_placed", 1 );
	trap.SetTag( "trap_type", trapType );
	trap.SetTag( "trap_faction", trapFaction );
	trap.SetTag( "trap_last_trigger", 0 );
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

	var trapFaction = iTrap.GetTag( "trap_faction" );
	var victimFaction = TrapGetFaction( pColliding );
	if( victimFaction === trapFaction )
		return false;

	var now = GetCurrentClock();
	var lastTrigger = iTrap.GetTag( "trap_last_trigger" );
	if( lastTrigger > 0 && now - lastTrigger < 10000 )
		return false;
	iTrap.SetTag( "trap_last_trigger", now );

	var trapType = iTrap.GetTag( "trap_type" );
	var damage = TrapDamage[trapType];
	if( !damage )
		damage = 10;

	iTrap.StaticEffect( TrapEffect[trapType], 10, 0 );
	iTrap.SoundEffect( 0x22F, true );
	pColliding.SetTempTag( "trap_scan_type", trapType );
	pColliding.SetTempTag( "trap_scan_faction", trapFaction );
	AreaCharacterFunction( "TrapDamageCharacter", pColliding, TrapRange[trapType], targSock );

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

	var trapType = srcChar.GetTempTag( "trap_scan_type" );
	var trapFaction = srcChar.GetTempTag( "trap_scan_faction" );
	if( TrapGetFaction( targetChar ) === trapFaction )
		return false;

	if( trapType === "GAS" )
		targetChar.poison = 2;

	targetChar.Damage( TrapDamage[trapType], 0, srcChar, false );
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

