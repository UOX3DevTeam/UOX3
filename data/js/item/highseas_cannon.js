// ServUO pub57 High Seas ship cannon workflow.
const CANNON_DEED = 1;
const CANNON = 2;
const POWDER_ID = 0xA2BE;
const BALL_ID = 0x4224;
const GRAPE_ID = 0xA2BF;
const FLAME_ID = 0x44C1;
const FROST_ID = 0x422B;
const AMMO_STANDARD = 1;
const AMMO_GRAPESHOT = 2;
const AMMO_FLAME = 3;
const AMMO_FROST = 4;
const FUSE_ID = 0x1420;
const RAMROD_ID = 0x4246;
const LIGHT_CANNON_RANGE = 10;
const HEAVY_CANNON_RANGE = 10;
const PUMPKIN_CANNON_RANGE = 10;
const PUMPKIN_CANNON_POWER = 4;
const PREP_TIME = 4000;
const ACTION_TIME = 1500;
const TIMER_PREP = 10;
const TIMER_CHARGE = 11;
const TIMER_LOAD = 12;
const TIMER_PRIME = 13;
const TIMER_FIRE = 14;
const IRON_ID = 0x1BF2;
const BOARD_ID = 0x1BD7;
const CLOTH_ID = 0x1766;
const CANNON_UNLOAD_ENTRY = 40;
const CANNON_DISMANTLE_ENTRY = 41;
const CANNON_REPAIR_ENTRY = 42;

function onUseChecked( user, used )
{
	if( used.cannonRole == CANNON_DEED )
	{
		if( !IsInPack( used, user.pack ))
		{
			user.socket.SysMessage( "The cannon deed must be in your backpack." );
			return false;
		}
		if( !ValidateObject( user.multi ) || !user.multi.IsBoat() )
		{
			user.socket.SysMessage( "You must be on the ship to deploy a weapon." );
			return false;
		}
		if( !user.multi.IsOwner( user ))
		{
			user.socket.SysMessage( "You must be the owner of the ship to do this." );
			return false;
		}
		user.socket.tempObj = used;
		user.socket.CustomTarget( 0, "Target a weapon pad on your ship." );
		return false;
	}
	if( used.cannonRole != CANNON )
		return false;
	if( !used.InRange( user, 3 ))
	{
		user.socket.SysMessage( "You are too far away." );
		return false;
	}
	if( !CanOperateCannon( used.multi, user ))
	{
		user.socket.SysMessage( "You do not have permission to use this ship cannon." );
		return false;
	}
	NormalizeCannonMagazine( used );
	AddCannonAction( used, user.multi == used.multi && used.InRange( user, 2 ) ?
		"You are now operating the cannon." : "You are too far away." );
	ShowCannonGump( user, used );
	return true; // Also open the cannon's three-stack ServUO magazine.
}

function onDropItemOnItem( dropped, user, cannon )
{
	if( !ValidateObject( dropped ) || !ValidateObject( user ) || !ValidateObject( cannon ) ||
		cannon.cannonRole != CANNON ) return 0;
	if( !CanOperateCannon( cannon.multi, user ) || user.multi != cannon.multi || !cannon.InRange( user, 2 ))
	{
		user.socket.SysMessage( "You may not use this cannon magazine." );
		return 0;
	}
	if( dropped.id != BALL_ID && dropped.id != GRAPE_ID && dropped.id != FLAME_ID &&
		dropped.id != FROST_ID && dropped.id != POWDER_ID && dropped.id != FUSE_ID )
	{
		user.socket.SysMessage( "The cannon magazine cannot hold that type of object." );
		return 0;
	}
	if( cannon.totalItemCount >= 3 )
	{
		user.socket.SysMessage( "The cannon magazine cannot hold more items." );
		return 0;
	}
	return 1;
}

function OperateCannon( user, used )
{
	var boat = used.multi;
	if( !ValidateObject( boat ) || user.multi != boat )
	{
		user.socket.SysMessage( "You must be aboard the ship to operate this cannon." );
		return false;
	}
	if( parseInt( used.GetTempTag( "hsCannonBusy" )) == 1 )
	{
		user.socket.SysMessage( "The cannon operation is already in progress." );
		return false;
	}

	var stage = parseInt( used.cannonStage );
	if( stage < 1 )
	{
		if( user.ResourceCount( RAMROD_ID, 0 ) < 1 ) { user.socket.SysMessage( "You need a ramrod." ); return false; }
		BeginCannonAction( used, user, TIMER_PREP, PREP_TIME, "Preparing to fire..." );
	}
	else if( stage == 1 )
	{
		if( CountCannonResource( used, POWDER_ID ) < 1 ) { user.socket.SysMessage( "The magazine does not have a powder charge." ); return false; }
		BeginCannonAction( used, user, TIMER_CHARGE, CannonActionTime( used ), "Charging started." );
	}
	else if( stage == 2 )
	{
		var ammo = SelectCannonAmmo( user, boat, used );
		if( CountCannonResource( used, ammo ) < 1 ) { user.socket.SysMessage( "The magazine does not have ammunition." ); return false; }
		used.SetTempTag( "hsPendingAmmo", ammo );
		BeginCannonAction( used, user, TIMER_LOAD, CannonActionTime( used ), "Loading started." );
	}
	else if( stage == 3 )
	{
		if( CountCannonResource( used, FUSE_ID ) < 1 ) { user.socket.SysMessage( "The magazine does not have fuse cord." ); return false; }
		BeginCannonAction( used, user, TIMER_PRIME, CannonActionTime( used ), "Priming started." );
	}
	else
	{
		LightCannonFuse( user, used );
	}
	return false;
}

function CanOperateCannon( boat, user )
{
	return ValidateObject( boat ) && boat.CanCommandShip( user );
}

function NormalizeCannonMagazine( cannon )
{
	if( !ValidateObject( cannon ) || !cannon.isShipCannon ) return;
	cannon.type = 1;
	cannon.maxItems = 3;
	cannon.weightMax = 30000;
}

function IsShipOfficer( boat, user )
{
	return ValidateObject( boat ) && boat.GetSecurityLevel( user ) >= 4;
}

function onContextMenuRequest( socket, cannon )
{
	var user = socket.currentChar;
	var boat = ValidateObject( cannon ) ? cannon.multi : null;
	if( !ValidateObject( user ) || !cannon.isShipCannon ||
		!IsShipOfficer( boat, user ) || !cannon.InRange( user, 2 )) return true;
	var stage = parseInt( cannon.cannonStage );
	var hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	var entries = [
		{ id: CANNON_UNLOAD_ENTRY, text: 1116072, flags: stage == 4 ? 0x0000 : 0x0001, hue: 0x03E0 },
		{ id: CANNON_DISMANTLE_ENTRY, text: 1116069, flags: stage <= 1 && cannon.totalItemCount == 0 && hits >= 100 ? 0x0000 : 0x0001, hue: 0x03E0 },
		{ id: CANNON_REPAIR_ENTRY, text: 1116602, flags: hits < 100 ? 0x0000 : 0x0001, hue: 0x03E0 }
	];
	TriggerEvent( 18001, "modifyContextMenu", socket, cannon, entries, true );
	return false;
}

function onContextMenuSelect( socket, cannon, popupEntry )
{
	var user = socket.currentChar;
	var boat = ValidateObject( cannon ) ? cannon.multi : null;
	if( !ValidateObject( user ) || !IsShipOfficer( boat, user ) || !cannon.InRange( user, 2 )) return false;
	if( popupEntry == CANNON_UNLOAD_ENTRY ) UnloadCannon( user, cannon );
	else if( popupEntry == CANNON_DISMANTLE_ENTRY ) DismantleCannon( user, cannon );
	else if( popupEntry == CANNON_REPAIR_ENTRY ) RepairCannon( user, cannon );
	return false;
}

function ShowCannonGump( user, cannon )
{
	var boat = cannon.multi;
	if( !CanOperateCannon( boat, user ))
	{
		user.socket.SysMessage( "Only the ship's captain and authorized crew may operate this cannon." );
		return;
	}
	var linkedPad = CalcItemFromSer( parseInt( cannon.cannonLinkSerial ));
	if( !ValidateObject( linkedPad )) AreaItemFunction( "RestoreHighSeasCannonPad", cannon, 2 );
	user.SetTempTag( "hsCannonMenu", cannon.serial );
	var stage = parseInt( cannon.cannonStage );
	var g = new Gump();
	g.AddPage( 0 );
	// Exact ServUO ShipCannonGump footprint and control placement.
	g.AddBackground( 0, 0, 250, 175, 0x06DB );
	g.AddHTMLGump( 10, 10, 230, 18, false, false,
		"<BASEFONT COLOR=#7FCFFF>" + CannonPositionName( cannon ) + "</BASEFONT>" );
	g.AddHTMLGump( 115, 35, 70, 18, false, false, "<BASEFONT COLOR=#80FF80>STATUS</BASEFONT>" );
	if( stage == 4 )
	{
		g.AddButton( 10, 35, 0x0FA5, 0x0FA7, 1, 0, 8 );
		g.AddHTMLGump( 45, 35, 70, 18, false, false, "<BASEFONT COLOR=#FFFFFF>UNLOAD</BASEFONT>" );
		g.AddButton( 10, 89, 0x0FA5, 0x0FA7, 1, 0, 6 );
		g.AddHTMLGump( 45, 89, 70, 18, false, false, "<BASEFONT COLOR=#FFFFFF>FIRE</BASEFONT>" );
	}
	else
	{
		g.AddButton( 10, 35, 0x0FA5, 0x0FA7, 1, 0, 1 );
		g.AddHTMLGump( 45, 35, 70, 18, false, false, "<BASEFONT COLOR=#FFFFFF>PREP</BASEFONT>" );
	}
	AddCannonStatusLine( g, 53, stage >= 2 ? "Charged" : "Not Charged", stage >= 2 );
	AddCannonStatusLine( g, 71, stage >= 3 ? AmmoName( parseInt( cannon.morex )) : "Not Loaded", stage >= 3 );
	AddCannonStatusLine( g, 89, stage >= 4 ? "Primed" : "No Fuse", stage >= 4 );
	for( var i = 0; i < 3; ++i )
	{
		var action = cannon.GetTempTag( "hsCannonAction" + i );
		if( action ) g.AddHTMLGump( 10, 112 + i * 18, 230, 18, false, false,
			"<BASEFONT COLOR=" + ( i == 0 ? "#FFFFFF" : "#B0B0B0" ) + ">" + action + "</BASEFONT>" );
	}
	g.Send( user );
	g.Free();
}

function IsViewingCannonGump( user, cannon )
{
	return ValidateObject( user ) && ValidateObject( cannon ) &&
		parseInt( user.GetTempTag( "hsCannonMenu" )) == cannon.serial;
}

function AddCannonStatusLine( g, y, text, good )
{
	g.AddHTMLGump( 115, y, 125, 18, false, false,
		"<BASEFONT COLOR=" + ( good ? "#80FF80" : "#FF8080" ) + ">" + text + "</BASEFONT>" );
}

function CannonPositionName( cannon )
{
	var boat = cannon.multi;
	if( !ValidateObject( boat )) return CannonName( cannon );
	var dx = cannon.x - boat.x, dy = cannon.y - boat.y;
	var facing = parseInt( boat.dir ) & 0x06;
	var localX = dx, localY = dy;
	if( facing == 2 ) { localX = dy; localY = -dx; }
	else if( facing == 4 ) { localX = -dx; localY = -dy; }
	else if( facing == 6 ) { localX = -dy; localY = dx; }
	var side = localX < 0 ? "Port" : ( localX > 0 ? "Starboard" : "" );
	var section = localY < -2 ? "Bow" : ( localY > 2 ? "Aft" : "Amidship" );
	return section + ( side ? " " + side : "" ) + " Cannon";
}

function AddCannonAction( cannon, message )
{
	if( !ValidateObject( cannon ) || !message ) return;
	cannon.SetTempTag( "hsCannonAction2", cannon.GetTempTag( "hsCannonAction1" ));
	cannon.SetTempTag( "hsCannonAction1", cannon.GetTempTag( "hsCannonAction0" ));
	cannon.SetTempTag( "hsCannonAction0", message );
}

function RestoreHighSeasCannonPad( cannon, candidate )
{
	if( !ValidateObject( candidate ) || !candidate.isWeaponPad || candidate.multi != cannon.multi ||
		candidate.x != cannon.x || candidate.y != cannon.y ) return false;
	cannon.cannonLinkSerial = candidate.serial;
	candidate.cannonLinkSerial = cannon.serial;
	return true;
}

function CannonStageName( stage )
{
	if( stage == 1 ) return "prepared";
	if( stage == 2 ) return "charged";
	if( stage == 3 ) return "loaded";
	if( stage == 4 ) return "ready";
	return "empty";
}

function CannonName( cannon )
{
	var power = parseInt( cannon.cannonPower );
	if( power == PUMPKIN_CANNON_POWER ) return "Pumpkin Cannon";
	return power == 2 ? "Heavy Ship Cannon" : "Light Ship Cannon";
}

function CannonRange( cannon )
{
	var configuredRange = parseInt( cannon.cannonRange );
	if( configuredRange > 0 ) return configuredRange;
	var power = parseInt( cannon.cannonPower );
	if( power == PUMPKIN_CANNON_POWER ) return PUMPKIN_CANNON_RANGE;
	return power == 2 ? HEAVY_CANNON_RANGE : LIGHT_CANNON_RANGE;
}

function onGumpPress( socket, button, gumpData )
{
	var user = socket.currentChar;
	var cannon = CalcItemFromSer( parseInt( user.GetTempTag( "hsCannonMenu" )));
	// Right-click/close sends button zero. ServUO removes the mobile from the
	// cannon's Viewing list here; clearing our equivalent tag prevents both this
	// response and later action timers from reopening the window.
	if( button == 0 )
	{
		user.SetTempTag( "hsCannonMenu", null );
		return;
	}
	if( !ValidateObject( cannon ) || !cannon.isShipCannon ) return;
	var boat = cannon.multi;
	if( !CanOperateCannon( boat, user )) return;
	if( user.multi != boat || !cannon.InRange( user, 2 ) || user.dead )
	{
		AddCannonAction( cannon, "You are too far away." );
		ShowCannonGump( user, cannon );
		return;
	}
	if( button == 1 ) OperateCannon( user, cannon );
	else if( button == 6 && parseInt( cannon.cannonStage ) == 4 ) LightCannonFuse( user, cannon );
	else if( button == 8 && parseInt( cannon.cannonStage ) == 4 ) UnloadCannon( user, cannon );
	ShowCannonGump( user, cannon );
}

function SelectCannonAmmo( user, boat, cannon )
{
	// ServUO loads a cannonball first when one is present in the magazine, then
	// falls back to grapeshot. Special cannonballs remain valid cannonballs.
	var choices = [BALL_ID, FROST_ID, FLAME_ID, GRAPE_ID];
	for( var i = 0; i < choices.length; ++i )
		if( CountCannonResource( cannon, choices[i] ) > 0 ) return choices[i];
	return BALL_ID;
}

function CountCannonResource( cannon, itemID )
{
	return ValidateObject( cannon ) ? cannon.ResourceCount( itemID, 0 ) : 0;
}

function ConsumeCannonResource( cannon, amount, itemID )
{
	if( ValidateObject( cannon )) cannon.UseResource( amount, itemID, 0 );
}

function FindCannonResource( cannon, itemID )
{
	if( !ValidateObject( cannon )) return null;
	for( var item = cannon.FirstItem(); !cannon.FinishedItems(); item = cannon.NextItem() )
		if( ValidateObject( item ) && item.id == itemID ) return item;
	return null;
}

function AmmoName( ammo )
{
	if( ammo == AMMO_GRAPESHOT ) return "grapeshot";
	if( ammo == AMMO_FLAME ) return "flame cannonball";
	if( ammo == AMMO_FROST ) return "frost cannonball";
	return "standard cannonball";
}

function ClearLoadedAmmoProfile( cannon )
{
	cannon.morex = 0;
	cannon.morey = 0;
	cannon.morez = 0;
	cannon.lodamage = 0;
	cannon.hidamage = 0;
}

function UnloadCannon( user, cannon )
{
	var boat = cannon.multi;
	if( !IsShipOfficer( boat, user )) { user.socket.SysMessage( "Only the captain or a ship officer may unload this cannon." ); return; }
	if( parseInt( cannon.GetTempTag( "hsCannonBusy" )) == 1 ) { user.socket.SysMessage( "The cannon is currently being operated." ); return; }
	var stage = parseInt( cannon.cannonStage );
	if( stage != 4 ) { user.socket.SysMessage( "The ship cannon is not fully loaded." ); return; }
	if( stage >= 4 ) CreateDFNItem( user.socket, user, "highseas_fuse_cord", 1, "ITEM", true );
	if( stage >= 3 )
	{
		var loadedAmmo = parseInt( cannon.morex );
		var ammoSection = loadedAmmo == AMMO_GRAPESHOT ? "highseas_grapeshot" :
			( loadedAmmo == AMMO_FLAME ? "highseas_flame_cannonball" : ( loadedAmmo == AMMO_FROST ? "highseas_frost_cannonball" : "highseas_cannonball" ));
		CreateDFNItem( user.socket, user, ammoSection, 1, "ITEM", true );
	}
	if( stage >= 2 ) CreateDFNItem( user.socket, user, "highseas_powder_charge", 1, "ITEM", true );
	cannon.cannonStage = stage == 0 ? 0 : 1;
	ClearLoadedAmmoProfile( cannon );
	AddCannonAction( cannon, "Cannon unloaded." );
	user.socket.SysMessage( "The cannon has been fully unloaded and its supplies returned." );
}

function RepairCannon( user, cannon )
{
	var boat = cannon.multi;
	if( !IsShipOfficer( boat, user )) { user.socket.SysMessage( "Only the captain or a ship officer may repair this cannon." ); return; }
	var hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	if( hits >= 100 ) { user.socket.SysMessage( "The cannon is already in pristine condition." ); return; }
	var level = hits >= 75 ? 1 : ( hits >= 50 ? 2 : ( hits >= 25 ? 3 : 4 ));
	var skill = parseInt( user.skills.blacksmithing ) / 10;
	var needed = 36 * level;
	needed -= ( skill / 200 ) * needed;
	needed = Math.max( 1, needed );
	var available = CountShipResource( user, boat, IRON_ID );
	var minimum = Math.max( 1, Math.floor( needed / 10 ));
	if( available < minimum )
	{
		user.socket.SysMessage( "You need at least " + minimum + " iron ingots to repair this cannon." );
		return;
	}
	var used = Math.max( 1, Math.floor( Math.min( available, needed )));
	var percent = Math.min( 1, available / needed );
	ConsumeShipResource( user, boat, used, IRON_ID );
	hits += Math.floor(( 100 - hits ) * percent );
	if( hits > 100 ) hits = 100;
	cannon.health = hits;
	cannon.Refresh();
	user.socket.SysMessage( "You use " + used + " iron ingots. The cannon is now " + hits + "% repaired." );
}

function RepairShipHull( user, boat )
{
	if( !CanOperateCannon( boat, user )) { user.socket.SysMessage( "Only authorized ship crew may repair this vessel." ); return; }
	if( user.multi != boat ) { user.socket.SysMessage( "You must be aboard the vessel to repair it." ); return; }
	if( !boat.IsNearLandOrDocks() ) { user.socket.SysMessage( "Permanent ship repairs may only be made near land or docks." ); return; }
	var hits = boat.GetHullHits();
	var maxHits = boat.GetHullMaxHits();
	if( maxHits <= 0 || hits >= maxHits ) { user.socket.SysMessage( "The ship is already fully repaired." ); return; }
	var durability = hits * 100 / maxHits;
	var woodNeeded = 17 * ( 100 - durability );
	var clothNeeded = 17 * ( 100 - durability );
	woodNeeded -= (( parseInt( user.skills.carpentry ) / 10 ) / 200 ) * woodNeeded;
	clothNeeded -= (( parseInt( user.skills.tailoring ) / 10 ) / 200 ) * clothNeeded;
	woodNeeded = Math.max( 1, woodNeeded );
	clothNeeded = Math.max( 1, clothNeeded );
	var wood = CountShipResource( user, boat, BOARD_ID );
	var cloth = CountShipResource( user, boat, CLOTH_ID );
	var minWood = Math.max( 1, Math.floor( woodNeeded / 10 ));
	var minCloth = Math.max( 1, Math.floor( clothNeeded / 10 ));
	if( wood < minWood || cloth < minCloth )
	{
		user.socket.SysMessage( "You need at least " + minCloth + " cloth and " + minWood + " boards to repair this ship." );
		return;
	}
	var woodUsed = Math.min( wood, woodNeeded );
	var clothUsed = Math.min( cloth, clothNeeded );
	var percWood = Math.min( 100, wood / woodNeeded * 100 );
	var percCloth = Math.min( 100, cloth / clothNeeded * 100 );
	if( clothUsed > woodUsed ) { clothUsed = woodUsed; percCloth = percWood; }
	else if( woodUsed > clothUsed ) { woodUsed = clothUsed; percWood = percCloth; }
	var totalPercent = ( percWood + percCloth ) / 2;
	var consumeWood = Math.max( 1, Math.floor( woodUsed ));
	var consumeCloth = Math.max( 1, Math.floor( clothUsed ));
	ConsumeShipResource( user, boat, consumeWood, BOARD_ID );
	ConsumeShipResource( user, boat, consumeCloth, CLOTH_ID );
	var repaired = boat.RepairHull( Math.floor(( maxHits - hits ) * totalPercent / 100 ));
	var newPercent = Math.floor( boat.GetHullHits() * 100 / maxHits );
	user.socket.SysMessage( "You use " + consumeCloth + " cloth and " + consumeWood + " boards. The ship is now " + newPercent + "% repaired." );
}

function DismantleCannon( user, cannon )
{
	var boat = cannon.multi;
	if( !ValidateObject( boat ) || boat.GetSecurityLevel( user ) < 4 )
	{
		user.socket.SysMessage( "Only the ship's captain or an officer may dismantle this cannon." ); return;
	}
	var hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	if( parseInt( cannon.cannonStage ) > 1 || cannon.totalItemCount > 0 ) { user.socket.SysMessage( "The ship cannon and magazine must be fully unloaded before it can be dismantled." ); return; }
	if( hits < 100 ) { user.socket.SysMessage( "The cannon must be fully repaired before it can be dismantled." ); return; }
	var pad = CalcItemFromSer( parseInt( cannon.cannonLinkSerial ));
	if( ValidateObject( pad )) pad.cannonLinkSerial = 0;
	var power = parseInt( cannon.cannonPower );
	CreateDFNItem( user.socket, user, power == PUMPKIN_CANNON_POWER ? "pumpkin_cannon_deed" :
		( power == 2 ? "highseas_heavy_cannon_deed" : "highseas_light_cannon_deed" ), 1, "ITEM", true );
	cannon.Delete();
	user.socket.SysMessage( "You dismantle the ship cannon and recover its deed." );
}

function BeginEmergencyRepairs( user, boat )
{
	if( !CanOperateCannon( boat, user )) { user.socket.SysMessage( "Only authorized ship crew may order emergency repairs." ); return; }
	if( user.multi != boat ) { user.socket.SysMessage( "You must be aboard the vessel." ); return; }
	var hits = boat.GetHullHits();
	var maxHits = boat.GetHullMaxHits();
	if( maxHits <= 0 || hits * 4 >= maxHits ) { user.socket.SysMessage( "Emergency repairs are only needed on a scuttled vessel." ); return; }
	if( boat.IsUnderEmergencyRepairs() ) { user.socket.SysMessage( "Emergency repairs are already holding this vessel together." ); return; }
	if( CountShipResource( user, boat, CLOTH_ID ) < 55 || CountShipResource( user, boat, BOARD_ID ) < 25 )
	{
		user.socket.SysMessage( "Emergency repairs require 55 cloth and 25 boards." ); return;
	}
	ConsumeShipResource( user, boat, 55, CLOTH_ID );
	ConsumeShipResource( user, boat, 25, BOARD_ID );
	boat.StartEmergencyRepairs( 360 );
	user.socket.SysMessage( "Emergency repairs will hold the scuttled vessel together for six minutes." );
}

function BeginCannonAction( cannon, user, timerID, delay, message )
{
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", user.serial );
	AddCannonAction( cannon, message );
	user.socket.SysMessage( message );
	cannon.StartTimer( delay, timerID, true );
}

function CannonActionTime( cannon )
{
	return cannon.cannonActionTime > 0 ? cannon.cannonActionTime : ACTION_TIME;
}

function CountShipResource( user, boat, itemID )
{
	var amount = user.ResourceCount( itemID, 0 );
	var hold = ValidateObject( boat ) ? boat.GetHold() : null;
	if( ValidateObject( hold )) amount += hold.ResourceCount( itemID, 0 );
	return amount;
}

function ConsumeShipResource( user, boat, amount, itemID )
{
	var fromPack = Math.min( amount, user.ResourceCount( itemID, 0 ));
	if( fromPack > 0 ) user.UseResource( fromPack, itemID, 0 );
	var remaining = amount - fromPack;
	var hold = ValidateObject( boat ) ? boat.GetHold() : null;
	if( remaining > 0 && ValidateObject( hold )) hold.UseResource( remaining, itemID, 0 );
}

function IsInPack( item, pack )
{
	var parent = item.container;
	while( ValidateObject( parent ))
	{
		if( parent == pack ) return true;
		parent = parent.container;
	}
	return false;
}

function onCallback0( socket, target )
{
	var deed = socket.tempObj;
	socket.tempObj = null;
	var deedPower = ValidateObject( deed ) ? parseInt( deed.cannonPower ) : 1;
	var cannonPower = deedPower == PUMPKIN_CANNON_POWER ? PUMPKIN_CANNON_POWER : ( deedPower == 2 ? 2 : 1 );
	if( !ValidateObject( deed ) || !ValidateObject( target ) || !target.isItem || !target.isWeaponPad )
	{
		socket.SysMessage( "That is not a weapon pad." ); return;
	}
	var boat = target.multi;
	if( !ValidateObject( boat ) || !boat.IsBoat() || socket.currentChar.multi != boat ||
		!boat.IsOwner( socket.currentChar ) || !target.InRange( socket.currentChar, 2 ))
	{
		socket.SysMessage( "You may only place this on your own ship." ); return;
	}
	var existingSerial = parseInt( target.cannonLinkSerial );
	if( existingSerial )
	{
		var existing = CalcItemFromSer( existingSerial );
		if( ValidateObject( existing ) && existing.isShipCannon )
		{
			// ServUO IsValidCannonSpot places the cannon at the weapon pad's
			// calculated tile top, not at the pad's base Z.
			existing.SetLocation( target.x, target.y, target.z + GetTileHeight( target.id ), target.worldnumber, target.instanceID );
			existing.multi = boat;
			existing.cannonLinkSerial = target.serial;
			if( parseInt( existing.health ) <= 0 ) existing.health = 100;
			NormalizeCannonMagazine( existing );
			SetCannonArt( existing, boat );
			existing.Refresh();
			socket.SysMessage( "The existing cannon has been restored to the weapon pad." );
			return;
		}
		target.cannonLinkSerial = 0;
	}
	var cannon = CreateDFNItem( socket, socket.currentChar, cannonPower == PUMPKIN_CANNON_POWER ? "pumpkin_cannon" :
		( cannonPower == 2 ? "highseas_heavy_cannon" : "highseas_light_cannon" ), 1, "ITEM", false );
	if( !ValidateObject( cannon )) return;
	cannon.SetLocation( target.x, target.y, target.z + GetTileHeight( target.id ), target.worldnumber, target.instanceID );
	cannon.multi = boat;
	SetCannonArt( cannon, boat );
	cannon.cannonRole = CANNON;
	cannon.cannonPower = cannonPower;
	cannon.cannonStage = 0;
	cannon.health = 100;
	cannon.maxhp = 100;
	cannon.cannonLinkSerial = target.serial;
	target.cannonLinkSerial = cannon.serial;
	NormalizeCannonMagazine( cannon );
	deed.Delete();
}

function onCallback1( socket, target )
{
	var cannon = socket.tempObj;
	socket.tempObj = null;
	if( !ValidateObject( cannon ) || parseInt( cannon.cannonStage ) != 4 ) return;
	if( !CanOperateCannon( cannon.multi, socket.currentChar ) || socket.currentChar.multi != cannon.multi )
	{
		socket.SysMessage( "You are no longer authorized to operate this cannon." ); return;
	}
	if( socket.currentChar.region && socket.currentChar.region.isGuarded )
	{
		socket.SysMessage( "You are forbidden from discharging cannons within guarded town limits." ); return;
	}
	var targetBoat = ResolveTargetBoat( target );
	if( !ValidateObject( targetBoat ) || !targetBoat.IsBoat() )
	{
		socket.SysMessage( "That is not an enemy ship." ); return;
	}
	if( cannon.multi == targetBoat ) { socket.SysMessage( "You cannot fire on your own ship." ); return; }
	if( AreFriendlyShips( cannon.multi, targetBoat ))
	{
		socket.SysMessage( "You cannot fire on a friendly, allied, guild, or party vessel." ); return;
	}
	if( !IsTargetInCannonArc( cannon, targetBoat ))
	{
		socket.SysMessage( "That ship is outside this cannon's firing arc or range." ); return;
	}
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", socket.currentChar.serial );
	cannon.SetTempTag( "hsCannonTarget", targetBoat.serial );
	socket.SysMessage( "The fuse is lit!" );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( ACTION_TIME, TIMER_FIRE, true );
}

function AreFriendlyShips( sourceBoat, targetBoat )
{
	if( !ValidateObject( sourceBoat ) || !ValidateObject( targetBoat )) return false;
	var sourceOwner = sourceBoat.owner;
	var targetOwner = targetBoat.owner;
	if( !ValidateObject( sourceOwner ) || !ValidateObject( targetOwner )) return false;
	if( sourceOwner == targetOwner ) return true;
	if( sourceOwner.party != null && targetOwner.party != null && sourceOwner.party == targetOwner.party ) return true;
	if( sourceOwner.guild != null && targetOwner.guild != null )
	{
		var relation = CompareGuildByGuild( sourceOwner.guild, targetOwner.guild );
		if( relation == 2 || relation == 4 ) return true;
	}
	return false;
}

function ResolveTargetBoat( target )
{
	if( !ValidateObject( target )) return null;
	if( target.isItem && target.IsBoat() ) return target;
	if( ValidateObject( target.multi ) && target.multi.IsBoat() ) return target.multi;
	return null;
}

function LightCannonFuse( user, cannon )
{
	if( user.region && user.region.isGuarded )
	{
		user.socket.SysMessage( "You are forbidden from discharging cannons within guarded town limits." );
		return false;
	}
	if( !HasLitTorch( user ))
	{
		AddCannonAction( cannon, "You need a lighted fire source." );
		user.socket.SysMessage( "You need a lighted fire source." );
		return false;
	}
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", user.serial );
	cannon.SetTempTag( "hsCannonTarget", 0 );
	ResolveAutomaticCannonTarget( cannon, user );
	AddCannonAction( cannon, "The fuse is lit!" );
	user.socket.SysMessage( "The fuse is lit!" );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( ACTION_TIME, TIMER_FIRE, true );
	return true;
}

function HasLitTorch( user )
{
	for( var layer = 1; layer <= 2; ++layer )
	{
		var held = user.FindItemLayer( layer );
		if( ValidateObject( held ) && held.id >= 0x0A12 && held.id <= 0x0A15 ) return true;
	}
	return ContainerHasLitTorch( user.pack );
}

function ContainerHasLitTorch( container )
{
	if( !ValidateObject( container )) return false;
	for( var item = container.FirstItem(); !container.FinishedItems(); item = container.NextItem() )
	{
		if( !ValidateObject( item )) continue;
		if( item.id >= 0x0A12 && item.id <= 0x0A15 ) return true;
		if( item.itemsinside > 0 && ContainerHasLitTorch( item )) return true;
	}
	return false;
}

function ResolveAutomaticCannonTarget( cannon, user )
{
	var range = CannonRange( cannon );
	cannon.SetTempTag( "hsTrajectoryOperator", ValidateObject( user ) ? user.serial : 0 );
	cannon.SetTempTag( "hsTrajectoryBest", range + 1 );
	cannon.SetTempTag( "hsCannonTarget", 0 );
	AreaItemFunction( "FindCannonTrajectoryTarget", cannon, range );
	return CalcItemFromSer( parseInt( cannon.GetTempTag( "hsCannonTarget" )));
}

function FindCannonTrajectoryTarget( cannon, candidate )
{
	if( !ValidateObject( candidate ) || !candidate.IsBoat() || candidate == cannon.multi || AreFriendlyShips( cannon.multi, candidate )) return false;
	var dx = candidate.x - cannon.x;
	var dy = candidate.y - cannon.y;
	var vx = 0, vy = 0;
	switch( GetCannonFacing( cannon ))
	{
		case 0: vy = -1; break;
		case 2: vx = 1; break;
		case 4: vy = 1; break;
		case 6: vx = -1; break;
	}
	var forward = dx * vx + dy * vy;
	var lateral = Math.abs( dx * vy - dy * vx );
	var range = CannonRange( cannon );
	if( forward <= 0 || forward > range || lateral > 1 + Math.floor( forward / 3 )) return false;
	if( forward < parseInt( cannon.GetTempTag( "hsTrajectoryBest" )))
	{
		cannon.SetTempTag( "hsTrajectoryBest", forward );
		cannon.SetTempTag( "hsCannonTarget", candidate.serial );
	}
	return true;
}

function IsTargetInCannonArc( cannon, target )
{
	var dx = target.x - cannon.x;
	var dy = target.y - cannon.y;
	var vx = 0, vy = 0;
	switch( GetCannonFacing( cannon ))
	{
		case 0: vy = -1; break;
		case 2: vx = 1; break;
		case 4: vy = 1; break;
		case 6: vx = -1; break;
	}
	var forward = dx * vx + dy * vy;
	var lateral = Math.abs( dx * vy - dy * vx );
	var range = CannonRange( cannon );
	return forward > 0 && forward <= range && lateral <= 1 + Math.floor( forward / 3 );
}

function GetCannonFacing( cannon )
{
	if( cannon.id == cannon.cannonArtSouth || cannon.id == 16918 || cannon.id == 16922 || cannon.id == 41979 ) return 4;
	if( cannon.id == cannon.cannonArtWest || cannon.id == 16919 || cannon.id == 16923 || cannon.id == 41980 ) return 6;
	if( cannon.id == cannon.cannonArtNorth || cannon.id == 16920 || cannon.id == 16924 || cannon.id == 41981 ) return 0;
	return 2;
}

function onTimer( cannon, timerID )
{
	if( !ValidateObject( cannon )) return;
	var user = CalcCharFromSer( parseInt( cannon.GetTempTag( "hsCannonOperator" )));
	if( timerID != TIMER_FIRE && ( !ValidateObject( user ) || !cannon.InRange( user, 3 ) || user.multi != cannon.multi ))
	{
		cannon.SetTempTag( "hsCannonBusy", 0 );
		if( ValidateObject( user ) && user.socket ) user.socket.SysMessage( "The cannon operation was canceled." );
		return;
	}
	if( timerID == TIMER_PREP )
	{
		if( user.ResourceCount( RAMROD_ID, 0 ) < 1 ) { AddCannonAction( cannon, "You need a ramrod." ); user.socket.SysMessage( "You need a ramrod." ); }
		else
		{
			cannon.cannonStage = 1;
			AddCannonAction( cannon, "Preparation finished." );
			user.socket.SysMessage( "Preparation finished." );
			if( CountCannonResource( cannon, POWDER_ID ) >= 1 )
			{
				BeginCannonAction( cannon, user, TIMER_CHARGE, CannonActionTime( cannon ), "Charging started." );
				return;
			}
			user.socket.SysMessage( "Charging stopped. You need a powder charge." );
		}
	}
	else if( timerID == TIMER_CHARGE )
	{
		if( CountCannonResource( cannon, POWDER_ID ) < 1 ) user.socket.SysMessage( "Charging stopped. The magazine needs a powder charge." );
		else
		{
			ConsumeCannonResource( cannon, 1, POWDER_ID );
			cannon.cannonStage = 2;
			AddCannonAction( cannon, "Charging finished." );
			user.socket.SysMessage( "Charging finished." );
			var nextAmmo = SelectCannonAmmo( user, cannon.multi, cannon );
			if( CountCannonResource( cannon, nextAmmo ) >= 1 )
			{
				cannon.SetTempTag( "hsPendingAmmo", nextAmmo );
				BeginCannonAction( cannon, user, TIMER_LOAD, CannonActionTime( cannon ), "Loading started." );
				return;
			}
			user.socket.SysMessage( "Loading stopped. You need a cannonball or grapeshot." );
		}
	}
	else if( timerID == TIMER_LOAD )
	{
		var ammo = parseInt( cannon.GetTempTag( "hsPendingAmmo" ));
		var ammoItem = FindCannonResource( cannon, ammo );
		if( !ValidateObject( ammoItem )) user.socket.SysMessage( "Loading stopped. The magazine ammunition is missing." );
		else
		{
			cannon.morex = Math.max( AMMO_STANDARD, parseInt( ammoItem.morex ));
			cannon.morey = Math.max( 1, parseInt( ammoItem.morey ));
			cannon.morez = Math.max( 0, parseInt( ammoItem.morez ));
			cannon.lodamage = Math.max( 0, parseInt( ammoItem.lodamage ));
			cannon.hidamage = Math.max( cannon.lodamage, parseInt( ammoItem.hidamage ));
			ConsumeCannonResource( cannon, 1, ammo );
			cannon.cannonStage = 3;
			AddCannonAction( cannon, "Loading finished." );
			user.socket.SysMessage( "Loading finished." );
			if( CountCannonResource( cannon, FUSE_ID ) >= 1 )
			{
				BeginCannonAction( cannon, user, TIMER_PRIME, CannonActionTime( cannon ), "Priming started." );
				return;
			}
			user.socket.SysMessage( "Priming stopped. You need fuse cord." );
		}
	}
	else if( timerID == TIMER_PRIME )
	{
		if( CountCannonResource( cannon, FUSE_ID ) < 1 ) user.socket.SysMessage( "Priming stopped. The magazine needs fuse cord." );
		else { ConsumeCannonResource( cannon, 1, FUSE_ID ); cannon.cannonStage = 4; AddCannonAction( cannon, "Ready to fire." ); user.socket.SysMessage( "Priming finished. The cannon is ready to fire." ); }
	}
	else if( timerID == TIMER_FIRE )
	{
		FireCannon( cannon, user );
		return;
	}
	cannon.SetTempTag( "hsCannonBusy", 0 );
	if( ValidateObject( user ) && user.socket && cannon.InRange( user, 3 ) && IsViewingCannonGump( user, cannon ))
		ShowCannonGump( user, cannon );
}

function FireCannon( cannon, user )
{
	var target = ResolveAutomaticCannonTarget( cannon, user );
	if( ValidateObject( user ) && user.region && user.region.isGuarded )
	{
		cannon.SetTempTag( "hsCannonBusy", 0 );
		if( user.socket ) user.socket.SysMessage( "Cannon fire is forbidden within guarded town limits." );
		return;
	}
	var ammo = parseInt( cannon.morex );
	var minDamage = Math.max( 0, parseInt( cannon.lodamage ));
	var maxDamage = Math.max( minDamage, parseInt( cannon.hidamage ));
	var sourceBoat = cannon.multi;
	var cannonDamageMod = ValidateObject( sourceBoat ) &&
		( parseInt( sourceBoat.id ) - 0x4000 ) >= 0x18 && ( parseInt( sourceBoat.id ) - 0x4000 ) <= 0x1B ? 1.5 : 1.0;
	var range = CannonRange( cannon );
	var missX = cannon.x;
	var missY = cannon.y;
	switch( GetCannonFacing( cannon ))
	{
		case 0: missY -= range; break;
		case 2: missX += range; break;
		case 4: missY += range; break;
		case 6: missX -= range; break;
	}
	DoMovingEffect( cannon.x, cannon.y, cannon.z + 2, ValidateObject( target ) ? target.x : missX,
		ValidateObject( target ) ? target.y : missY, ValidateObject( target ) ? target.z + 8 : cannon.z, 0x36E4, 8, 0, false );
	cannon.SoundEffect( 0x011C, true );
	var damage = 0;
	if( ValidateObject( target ) && ammo == AMMO_GRAPESHOT )
	{
		var lastHitDamage = 0;
		var lastHitX = target.x;
		var lastHitY = target.y;
		var pellets = Math.max( 1, parseInt( cannon.morey ));
		var spread = Math.max( 0, parseInt( cannon.morez ));
		for( var i = 0; i < pellets; ++i )
		{
			var hitDamage = Math.floor( RandomNumber( minDamage, maxDamage ) * cannonDamageMod );
			var hitX = target.x + RandomNumber( -spread, spread );
			var hitY = target.y + RandomNumber( -spread, spread );
			damage += hitDamage;
			target.DamageHull( hitDamage );
			DoStaticEffect( hitX, hitY, target.z + 8, 0x36CB, 15, 15, true );
			DamageCrewAtImpact( target, user, hitX, hitY );
			lastHitDamage = hitDamage;
			lastHitX = hitX;
			lastHitY = hitY;
		}
		DamageCannonNearImpact( target, user, lastHitDamage, lastHitX, lastHitY );
	}
	else if( ValidateObject( target ))
	{
		damage = Math.floor( RandomNumber( minDamage, maxDamage ) * cannonDamageMod );
		var impactX = target.x;
		var impactY = target.y;
		var shotDX = target.x - cannon.x;
		var shotDY = target.y - cannon.y;
		if( Math.abs( shotDX ) >= Math.abs( shotDY ))
		{
			impactX -= shotDX > 0 ? 2 : -2;
			impactY += RandomNumber( -1, 1 );
		}
		else
		{
			impactY -= shotDY > 0 ? 2 : -2;
			impactX += RandomNumber( -1, 1 );
		}
		target.DamageHull( damage );
		DoStaticEffect( impactX, impactY, target.z + 8, 0x36CB, 15, 15, true );
		DamageCannonNearImpact( target, user, damage, impactX, impactY );
	}
	if( ValidateObject( target ))
	{
		target.SoundEffect( 0x0207, true );
		target.Refresh();
	}
	// ServUO ClearCannon resets every preparation stage after each shot.
	cannon.cannonStage = 0;
	ClearLoadedAmmoProfile( cannon );
	cannon.SetTempTag( "hsCannonBusy", 0 );
	if( ValidateObject( user ) && user.socket )
	{
		var resultMessage = ValidateObject( target ) ? "The cannon strikes the enemy hull for " + damage + " damage." : "The cannon fires, but the shot finds no target.";
		AddCannonAction( cannon, resultMessage );
		user.socket.SysMessage( resultMessage );
		if( cannon.InRange( user, 3 ) && IsViewingCannonGump( user, cannon )) ShowCannonGump( user, cannon );
	}
}

function DamageCrewAtImpact( targetBoat, shooter, hitX, hitY )
{
	targetBoat.SetTempTag( "hsImpactX", hitX );
	targetBoat.SetTempTag( "hsImpactY", hitY );
	targetBoat.SetTempTag( "hsImpactShooter", ValidateObject( shooter ) ? shooter.serial : 0 );
	AreaCharacterFunction( "HighSeasGrapeshotCharacter", targetBoat, 20 );
}

function HighSeasGrapeshotCharacter( targetBoat, targetChar )
{
	if( !ValidateObject( targetChar ) || targetChar.dead || !targetChar.vulnerable || targetChar.multi != targetBoat ) return false;
	if( targetChar.x != parseInt( targetBoat.GetTempTag( "hsImpactX" )) || targetChar.y != parseInt( targetBoat.GetTempTag( "hsImpactY" ))) return false;
	var shooter = CalcCharFromSer( parseInt( targetBoat.GetTempTag( "hsImpactShooter" )));
	if( ValidateObject( shooter )) targetChar.Damage( 35, 1, shooter );
	else targetChar.Damage( 35, 1 );
	return true;
}

function DamageCannonNearImpact( targetBoat, shooter, damage, hitX, hitY )
{
	targetBoat.SetTempTag( "hsImpactX", hitX );
	targetBoat.SetTempTag( "hsImpactY", hitY );
	targetBoat.SetTempTag( "hsImpactDamage", damage );
	targetBoat.SetTempTag( "hsImpactShooter", ValidateObject( shooter ) ? shooter.serial : 0 );
	AreaItemFunction( "HighSeasCannonImpact", targetBoat, 20 );
}

function HighSeasCannonImpact( targetBoat, targetItem )
{
	if( !ValidateObject( targetItem ) || !targetItem.isShipCannon || targetItem.multi != targetBoat ) return false;
	var hitX = parseInt( targetBoat.GetTempTag( "hsImpactX" ));
	var hitY = parseInt( targetBoat.GetTempTag( "hsImpactY" ));
	if( Math.abs( targetItem.x - hitX ) > 1 || Math.abs( targetItem.y - hitY ) > 1 ) return false;
	var hits = parseInt( targetItem.health );
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	hits -= parseInt( targetBoat.GetTempTag( "hsImpactDamage" ));
	if( hits <= 0 )
	{
		DoStaticEffect( targetItem.x, targetItem.y, targetItem.z + 2, 0x36CB, 15, 15, true );
		targetItem.SoundEffect( 0x0207, true );
		targetItem.Delete();
		var shooter = CalcCharFromSer( parseInt( targetBoat.GetTempTag( "hsImpactShooter" )));
		if( ValidateObject( shooter ) && shooter.socket ) shooter.socket.SysMessage( "The enemy ship cannon has been destroyed!" );
	}
	else
	{
		targetItem.health = hits;
		targetItem.Refresh();
	}
	return true;
}

function onTooltip( cannon, socket )
{
	if( !cannon.isShipCannon ) return "";
	var hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	var stage = parseInt( cannon.cannonStage );
	var ammo = parseInt( cannon.morex );
	cannon.SetTempTag( "clilocTooltip", 1042971 );
	var condition = hits >= 100 ? "Pristine" : ( hits >= 75 ? "Slightly Damaged" :
		( hits >= 50 ? "Moderately Damaged" : ( hits >= 25 ? "Heavily Damaged" : "Severely Damaged" )));
	return "Charged: " + ( stage >= 2 ? "Yes" : "No" ) + "\nAmmo: " +
		( stage >= 3 ? AmmoName( ammo ) : "Empty" ) + "\nPrimed: " + ( stage >= 4 ? "Yes" : "No" ) +
		"\nCondition: " + condition;
}

function SetCannonArt( cannon, boat )
{
	var facing = parseInt( boat.dir ) & 0x07;
	var power = parseInt( cannon.cannonPower );
	var artOffset = power == 2 ? 4 : 0;
	var south = parseInt( cannon.cannonArtSouth ) || ( power == PUMPKIN_CANNON_POWER ? 41979 : 16918 + artOffset );
	var west = parseInt( cannon.cannonArtWest ) || ( power == PUMPKIN_CANNON_POWER ? 41980 : 16919 + artOffset );
	var north = parseInt( cannon.cannonArtNorth ) || ( power == PUMPKIN_CANNON_POWER ? 41981 : 16920 + artOffset );
	var east = parseInt( cannon.cannonArtEast ) || ( power == PUMPKIN_CANNON_POWER ? 41982 : 16921 + artOffset );
	if( facing == 0 || facing == 4 )
	{
		if( cannon.x < boat.x ) cannon.id = west;
		else if( cannon.x > boat.x ) cannon.id = east;
		else cannon.id = facing == 0 ? north : south;
	}
	else
	{
		if( cannon.y < boat.y ) cannon.id = north;
		else if( cannon.y > boat.y ) cannon.id = south;
		else cannon.id = facing == 2 ? east : west;
	}
	cannon.dir = facing;
}

// Scripted naval encounters use the same cannon objects, arcs, effects and hull
// damage as player-operated weapons. These entry points only replace the manual
// deed/loading clicks performed by a player crew.
function DeployNpcCannon( boat, pad, cannonPower )
{
	if( !ValidateObject( boat ) || !ValidateObject( pad ) || pad.multi != boat || !pad.isWeaponPad ) return null;
	var existing = CalcItemFromSer( parseInt( pad.cannonLinkSerial ));
	if( ValidateObject( existing ) && existing.isShipCannon ) return existing;
	var normalizedPower = cannonPower == PUMPKIN_CANNON_POWER ? PUMPKIN_CANNON_POWER : ( cannonPower == 2 ? 2 : 1 );
	var cannon = CreateDFNItem( null, null, normalizedPower == PUMPKIN_CANNON_POWER ? "pumpkin_cannon" :
		( normalizedPower == 2 ? "highseas_heavy_cannon" : "highseas_light_cannon" ), 1,
		"ITEM", false, 0, boat.worldnumber, boat.instanceID );
	if( !ValidateObject( cannon )) return null;
	cannon.SetLocation( pad.x, pad.y, pad.z + GetTileHeight( pad.id ), boat.worldnumber, boat.instanceID );
	cannon.multi = boat;
	cannon.cannonRole = CANNON;
	cannon.cannonPower = normalizedPower;
	cannon.cannonStage = 1;
	cannon.health = 100;
	cannon.maxhp = 100;
	cannon.cannonLinkSerial = pad.serial;
	pad.cannonLinkSerial = cannon.serial;
	NormalizeCannonMagazine( cannon );
	SetCannonArt( cannon, boat );
	cannon.Refresh();
	return cannon;
}

function FireNpcCannon( cannon, targetBoat, captain )
{
	if( !ValidateObject( cannon ) || !ValidateObject( targetBoat ) || !ValidateObject( captain ) ||
		!cannon.isShipCannon || cannon.multi == targetBoat ||
		parseInt( cannon.GetTempTag( "hsCannonBusy" )) == 1 || !IsTargetInCannonArc( cannon, targetBoat )) return false;
	cannon.cannonStage = 4;
	cannon.morex = AMMO_STANDARD;
	cannon.morey = 1;
	cannon.morez = 0;
	cannon.lodamage = 5000;
	cannon.hidamage = 5000;
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", captain.serial );
	cannon.SetTempTag( "hsCannonTarget", targetBoat.serial );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( ACTION_TIME, TIMER_FIRE, 5099 );
	return true;
}
