// ServUO pub57 High Seas ship cannon workflow.
const CANNON_DEED = 1;
const CANNON = 2;
const POWDER_ID = 0xA2BE;
const BALL_ID = 0x4224;
const GRAPE_ID = 0xA2BF;
const FLAME_ID = 0x44C1;
const FROST_ID = 0x422B;
const FUSE_ID = 0x1420;
const RAMROD_ID = 0x4246;
const LIGHT_CANNON_RANGE = 8;
const HEAVY_CANNON_RANGE = 10;
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

function onUseChecked( user, used )
{
	if( used.GetTag( "hsCannonKind" ) == CANNON_DEED )
	{
		if( !IsInPack( used, user.pack ))
		{
			user.socket.SysMessage( "The cannon deed must be in your backpack." );
			return false;
		}
		user.socket.tempObj = used;
		user.socket.CustomTarget( 0, "Target a weapon pad on your ship." );
		return false;
	}
	if( used.GetTag( "hsCannonKind" ) != CANNON || !used.InRange( user, 3 ))
		return false;
	ShowCannonGump( user, used );
	return false;
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

	var stage = parseInt( used.GetTag( "hsCannonStage" ));
	if( stage < 1 )
	{
		if( user.ResourceCount( RAMROD_ID, 0 ) < 1 ) { user.socket.SysMessage( "You need a ramrod." ); return false; }
		BeginCannonAction( used, user, TIMER_PREP, PREP_TIME, "Preparing to fire..." );
	}
	else if( stage == 1 )
	{
		if( CountShipResource( user, boat, POWDER_ID ) < 1 ) { user.socket.SysMessage( "You need a powder charge." ); return false; }
		BeginCannonAction( used, user, TIMER_CHARGE, CannonActionTime( used ), "Charging started." );
	}
	else if( stage == 2 )
	{
		var ammo = SelectCannonAmmo( user, boat, used );
		if( CountShipResource( user, boat, ammo ) < 1 ) { user.socket.SysMessage( "You need a cannonball or grapeshot." ); return false; }
		used.SetTempTag( "hsPendingAmmo", ammo );
		BeginCannonAction( used, user, TIMER_LOAD, CannonActionTime( used ), "Loading started." );
	}
	else if( stage == 3 )
	{
		if( CountShipResource( user, boat, FUSE_ID ) < 1 ) { user.socket.SysMessage( "You need fuse cord." ); return false; }
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

function IsShipOfficer( boat, user )
{
	return ValidateObject( boat ) && boat.GetSecurityLevel( user ) >= 4;
}

function ShowCannonGump( user, cannon )
{
	var boat = cannon.multi;
	if( !CanOperateCannon( boat, user ))
	{
		user.socket.SysMessage( "Only the ship's captain and authorized crew may operate this cannon." );
		return;
	}
	var linkedPad = CalcItemFromSer( parseInt( cannon.GetTag( "hsWeaponPadSerial" )));
	if( !ValidateObject( linkedPad )) AreaItemFunction( "RestoreHighSeasCannonPad", cannon, 2 );
	user.SetTempTag( "hsCannonMenu", cannon.serial );
	var stage = parseInt( cannon.GetTag( "hsCannonStage" ));
	var hits = parseInt( cannon.GetTag( "hsCannonHits" ));
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	var hullPercent = ValidateObject( boat ) && boat.GetHullMaxHits() > 0 ? Math.floor( boat.GetHullHits() * 100 / boat.GetHullMaxHits() ) : 0;
	var g = new Gump();
	g.AddPage( 0 );
	g.AddBackground( 0, 0, 420, 420, 0x0A28 );
	g.AddHTMLGump( 25, 20, 310, 30, false, false,
		"<CENTER><BIG>" + ( parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ? "Heavy" : "Light" ) + " Ship Cannon</BIG></CENTER>" );
	g.AddHTMLGump( 35, 55, 350, 35, false, false, "Cannon: " + hits + "% &nbsp; Hull: " + hullPercent + "% &nbsp; State: " + CannonStageName( stage ));
	AddCannonMenuButton( g, 40, 95, 1, stage == 4 ? "Light fuse" : "Prepare and load cannon" );
	AddCannonMenuButton( g, 40, 125, 2, "Unload cannon" );
	AddCannonMenuButton( g, 40, 155, 3, "Repair cannon" );
	AddCannonMenuButton( g, 40, 185, 4, "Dismantle cannon" );
	AddCannonMenuButton( g, 40, 215, 5, "Repair ship hull" );
	AddCannonMenuButton( g, 40, 245, 6, "Begin emergency repairs" );
	g.AddHTMLGump( 35, 280, 340, 25, false, false, "Magazine ammunition preference:" );
	AddCannonMenuButton( g, 40, 305, 7, "Standard cannonball" );
	AddCannonMenuButton( g, 40, 335, 8, "Flame cannonball" );
	AddCannonMenuButton( g, 220, 305, 9, "Frost cannonball" );
	AddCannonMenuButton( g, 220, 335, 10, "Grapeshot" );
	g.Send( user );
	g.Free();
}

function RestoreHighSeasCannonPad( cannon, candidate )
{
	if( !ValidateObject( candidate ) || candidate.GetTag( "hsWeaponPad" ) != 1 || candidate.multi != cannon.multi ||
		candidate.x != cannon.x || candidate.y != cannon.y ) return false;
	cannon.SetTag( "hsWeaponPadSerial", candidate.serial );
	candidate.SetTag( "hsCannonSerial", cannon.serial );
	return true;
}

function AddCannonMenuButton( g, x, y, button, text )
{
	g.AddButton( x, y, 0x0FA5, 0x0FA7, 1, 0, button );
	g.AddHTMLGump( x + 35, y + 2, 250, 25, false, false, text );
}

function CannonStageName( stage )
{
	if( stage == 1 ) return "prepared";
	if( stage == 2 ) return "charged";
	if( stage == 3 ) return "loaded";
	if( stage == 4 ) return "ready";
	return "empty";
}

function onGumpPress( socket, button, gumpData )
{
	var user = socket.currentChar;
	var cannon = CalcItemFromSer( parseInt( user.GetTempTag( "hsCannonMenu" )));
	if( !ValidateObject( cannon ) || cannon.GetTag( "hsCannonKind" ) != CANNON || !cannon.InRange( user, 3 )) return;
	var boat = cannon.multi;
	if( !CanOperateCannon( boat, user )) return;
	if( button == 1 ) OperateCannon( user, cannon );
	else if( button == 2 ) UnloadCannon( user, cannon );
	else if( button == 3 ) RepairCannon( user, cannon );
	else if( button == 4 ) DismantleCannon( user, cannon );
	else if( button == 5 ) RepairShipHull( user, boat );
	else if( button == 6 ) BeginEmergencyRepairs( user, boat );
	else if( button >= 7 && button <= 10 )
	{
		var preferred = button == 7 ? BALL_ID : ( button == 8 ? FLAME_ID : ( button == 9 ? FROST_ID : GRAPE_ID ));
		cannon.SetTag( "hsPreferredAmmo", preferred );
		user.socket.SysMessage( "Cannon magazine preference set to " + AmmoName( preferred ) + "." );
		ShowCannonGump( user, cannon );
	}
}

function SelectCannonAmmo( user, boat, cannon )
{
	var preferred = parseInt( cannon.GetTag( "hsPreferredAmmo" ));
	if(( preferred == BALL_ID || preferred == GRAPE_ID || preferred == FLAME_ID || preferred == FROST_ID ) &&
		CountShipResource( user, boat, preferred ) > 0 ) return preferred;
	var choices = [BALL_ID, FLAME_ID, FROST_ID, GRAPE_ID];
	for( var i = 0; i < choices.length; ++i )
		if( CountShipResource( user, boat, choices[i] ) > 0 ) return choices[i];
	return BALL_ID;
}

function AmmoName( ammo )
{
	if( ammo == GRAPE_ID ) return "grapeshot";
	if( ammo == FLAME_ID ) return "flame cannonball";
	if( ammo == FROST_ID ) return "frost cannonball";
	return "standard cannonball";
}

function UnloadCannon( user, cannon )
{
	var boat = cannon.multi;
	if( !IsShipOfficer( boat, user )) { user.socket.SysMessage( "Only the captain or a ship officer may unload this cannon." ); return; }
	if( parseInt( cannon.GetTempTag( "hsCannonBusy" )) == 1 ) { user.socket.SysMessage( "The cannon is currently being operated." ); return; }
	var stage = parseInt( cannon.GetTag( "hsCannonStage" ));
	if( stage >= 4 ) CreateDFNItem( user.socket, user, "highseas_fuse_cord", 1, "ITEM", true );
	if( stage >= 3 )
	{
		var loadedAmmo = parseInt( cannon.GetTag( "hsCannonAmmo" ));
		var ammoSection = loadedAmmo == GRAPE_ID ? "highseas_grapeshot" :
			( loadedAmmo == FLAME_ID ? "highseas_flame_cannonball" : ( loadedAmmo == FROST_ID ? "highseas_frost_cannonball" : "highseas_cannonball" ));
		CreateDFNItem( user.socket, user, ammoSection, 1, "ITEM", true );
	}
	if( stage >= 2 ) CreateDFNItem( user.socket, user, "highseas_powder_charge", 1, "ITEM", true );
	cannon.SetTag( "hsCannonStage", stage == 0 ? 0 : 1 );
	cannon.SetTag( "hsCannonAmmo", 0 );
	user.socket.SysMessage( "The cannon has been fully unloaded and its supplies returned." );
}

function RepairCannon( user, cannon )
{
	var boat = cannon.multi;
	if( !IsShipOfficer( boat, user )) { user.socket.SysMessage( "Only the captain or a ship officer may repair this cannon." ); return; }
	var hits = parseInt( cannon.GetTag( "hsCannonHits" ));
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
	cannon.SetTag( "hsCannonHits", hits );
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
	var hits = parseInt( cannon.GetTag( "hsCannonHits" ));
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	if( parseInt( cannon.GetTag( "hsCannonStage" )) > 1 ) { user.socket.SysMessage( "The cannon must be fully unloaded before it can be dismantled." ); return; }
	if( hits < 100 ) { user.socket.SysMessage( "The cannon must be fully repaired before it can be dismantled." ); return; }
	var pad = CalcItemFromSer( parseInt( cannon.GetTag( "hsWeaponPadSerial" )));
	if( ValidateObject( pad )) pad.SetTag( "hsCannonSerial", 0 );
	CreateDFNItem( user.socket, user, parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ?
		"highseas_heavy_cannon_deed" : "highseas_light_cannon_deed", 1, "ITEM", true );
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
	user.socket.SysMessage( message );
	cannon.StartTimer( delay, timerID, true );
}

function CannonActionTime( cannon )
{
	return parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ? 2000 : ACTION_TIME;
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
	var cannonPower = ValidateObject( deed ) && parseInt( deed.GetTag( "hsCannonPower" )) == 2 ? 2 : 1;
	if( !ValidateObject( deed ) || !ValidateObject( target ) || !target.isItem || target.GetTag( "hsWeaponPad" ) != 1 )
	{
		socket.SysMessage( "That is not a weapon pad." ); return;
	}
	var boat = target.multi;
	if( !ValidateObject( boat ) || !boat.IsBoat() || !boat.IsOwner( socket.currentChar ) || !target.InRange( socket.currentChar, 2 ))
	{
		socket.SysMessage( "You may only place this on your own ship." ); return;
	}
	var existingSerial = parseInt( target.GetTag( "hsCannonSerial" ));
	if( existingSerial )
	{
		var existing = CalcItemFromSer( existingSerial );
		if( ValidateObject( existing ) && existing.GetTag( "hsCannonKind" ) == CANNON )
		{
			// ServUO IsValidCannonSpot places the cannon at the weapon pad's
			// calculated tile top, not at the pad's base Z.
			existing.SetLocation( target.x, target.y, target.z + GetTileHeight( target.id ), target.worldnumber, target.instanceID );
			existing.multi = boat;
			existing.SetTag( "hsWeaponPadSerial", target.serial );
			if( parseInt( existing.GetTag( "hsCannonHits" )) <= 0 ) existing.SetTag( "hsCannonHits", 100 );
			SetCannonArt( existing, boat );
			existing.Refresh();
			socket.SysMessage( "The existing cannon has been restored to the weapon pad." );
			return;
		}
		target.SetTag( "hsCannonSerial", 0 );
	}
	var cannon = CreateDFNItem( socket, socket.currentChar, cannonPower == 2 ? "highseas_heavy_cannon" : "highseas_light_cannon", 1, "ITEM", false );
	if( !ValidateObject( cannon )) return;
	cannon.SetLocation( target.x, target.y, target.z + GetTileHeight( target.id ), target.worldnumber, target.instanceID );
	cannon.multi = boat;
	SetCannonArt( cannon, boat );
	cannon.SetTag( "hsCannonKind", CANNON );
	cannon.SetTag( "hsCannonPower", cannonPower );
	cannon.SetTag( "hsCannonStage", 1 );
	cannon.SetTag( "hsCannonShots", 0 );
	cannon.SetTag( "hsCannonHits", 100 );
	cannon.SetTag( "hsWeaponPadSerial", target.serial );
	target.SetTag( "hsCannonSerial", cannon.serial );
	deed.Delete();
}

function onCallback1( socket, target )
{
	var cannon = socket.tempObj;
	socket.tempObj = null;
	if( !ValidateObject( cannon ) || parseInt( cannon.GetTag( "hsCannonStage" )) != 4 ) return;
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
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", user.serial );
	cannon.SetTempTag( "hsCannonTarget", 0 );
	ResolveAutomaticCannonTarget( cannon, user );
	user.socket.SysMessage( "The fuse is lit!" );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( 2000, TIMER_FIRE, true );
	return true;
}

function ResolveAutomaticCannonTarget( cannon, user )
{
	var range = parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ? HEAVY_CANNON_RANGE : LIGHT_CANNON_RANGE;
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
	var range = parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ? HEAVY_CANNON_RANGE : LIGHT_CANNON_RANGE;
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
	var range = parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ? HEAVY_CANNON_RANGE : LIGHT_CANNON_RANGE;
	return forward > 0 && forward <= range && lateral <= 1 + Math.floor( forward / 3 );
}

function GetCannonFacing( cannon )
{
	if( cannon.id == 16918 || cannon.id == 16922 ) return 4;
	if( cannon.id == 16919 || cannon.id == 16923 ) return 6;
	if( cannon.id == 16920 || cannon.id == 16924 ) return 0;
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
		if( user.ResourceCount( RAMROD_ID, 0 ) < 1 ) user.socket.SysMessage( "You need a ramrod." );
		else
		{
			cannon.SetTag( "hsCannonStage", 1 );
			user.socket.SysMessage( "Preparation finished." );
			if( CountShipResource( user, cannon.multi, POWDER_ID ) >= 1 )
			{
				BeginCannonAction( cannon, user, TIMER_CHARGE, CannonActionTime( cannon ), "Charging started." );
				return;
			}
			user.socket.SysMessage( "Charging stopped. You need a powder charge." );
		}
	}
	else if( timerID == TIMER_CHARGE )
	{
		if( CountShipResource( user, cannon.multi, POWDER_ID ) < 1 ) user.socket.SysMessage( "Charging stopped. You need a powder charge." );
		else
		{
			ConsumeShipResource( user, cannon.multi, 1, POWDER_ID );
			cannon.SetTag( "hsCannonStage", 2 );
			user.socket.SysMessage( "Charging finished." );
			var nextAmmo = SelectCannonAmmo( user, cannon.multi, cannon );
			if( CountShipResource( user, cannon.multi, nextAmmo ) >= 1 )
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
		if( CountShipResource( user, cannon.multi, ammo ) < 1 ) user.socket.SysMessage( "Loading stopped. The ammunition is missing." );
		else
		{
			ConsumeShipResource( user, cannon.multi, 1, ammo );
			cannon.SetTag( "hsCannonAmmo", ammo );
			cannon.SetTag( "hsCannonStage", 3 );
			user.socket.SysMessage( "Loading finished." );
			if( CountShipResource( user, cannon.multi, FUSE_ID ) >= 1 )
			{
				BeginCannonAction( cannon, user, TIMER_PRIME, CannonActionTime( cannon ), "Priming started." );
				return;
			}
			user.socket.SysMessage( "Priming stopped. You need fuse cord." );
		}
	}
	else if( timerID == TIMER_PRIME )
	{
		if( CountShipResource( user, cannon.multi, FUSE_ID ) < 1 ) user.socket.SysMessage( "Priming stopped. You need fuse cord." );
		else { ConsumeShipResource( user, cannon.multi, 1, FUSE_ID ); cannon.SetTag( "hsCannonStage", 4 ); user.socket.SysMessage( "Priming finished. The cannon is ready to fire." ); }
	}
	else if( timerID == TIMER_FIRE )
	{
		FireCannon( cannon, user );
		return;
	}
	cannon.SetTempTag( "hsCannonBusy", 0 );
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
	var ammo = parseInt( cannon.GetTag( "hsCannonAmmo" ));
	var heavy = parseInt( cannon.GetTag( "hsCannonPower" )) == 2;
	var range = heavy ? HEAVY_CANNON_RANGE : LIGHT_CANNON_RANGE;
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
	if( ValidateObject( target ) && ammo == GRAPE_ID )
	{
		var lastHitDamage = 0;
		var lastHitX = target.x;
		var lastHitY = target.y;
		for( var i = 0; i < 15; ++i )
		{
			var hitDamage = heavy ? RandomNumber( 50, 75 ) : RandomNumber( 40, 50 );
			var hitX = target.x + RandomNumber( -2, 2 );
			var hitY = target.y + RandomNumber( -2, 2 );
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
		damage = heavy && ammo == BALL_ID ? 6500 : 5000;
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
	// ServUO clears charge, ammunition and primer after firing. Cleaning is only
	// required once the cannon's cleanliness counter passes ten shots.
	var shots = parseInt( cannon.GetTag( "hsCannonShots" ));
	if( isNaN( shots ) || shots < 0 ) shots = 0;
	cannon.SetTag( "hsCannonStage", shots >= 10 ? 0 : 1 );
	cannon.SetTag( "hsCannonShots", shots >= 10 ? 0 : shots + 1 );
	cannon.SetTag( "hsCannonAmmo", 0 );
	cannon.SetTempTag( "hsCannonBusy", 0 );
	if( ValidateObject( user ) && user.socket )
		user.socket.SysMessage( ValidateObject( target ) ? "The cannon strikes the enemy hull for " + damage + " damage." : "The cannon fires, but the shot finds no target." );
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
	if( !ValidateObject( targetItem ) || targetItem.GetTag( "hsCannonKind" ) != CANNON || targetItem.multi != targetBoat ) return false;
	var hitX = parseInt( targetBoat.GetTempTag( "hsImpactX" ));
	var hitY = parseInt( targetBoat.GetTempTag( "hsImpactY" ));
	if( Math.abs( targetItem.x - hitX ) > 1 || Math.abs( targetItem.y - hitY ) > 1 ) return false;
	var hits = parseInt( targetItem.GetTag( "hsCannonHits" ));
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
		targetItem.SetTag( "hsCannonHits", hits );
		targetItem.Refresh();
	}
	return true;
}

function onTooltip( cannon, socket )
{
	if( cannon.GetTag( "hsCannonKind" ) != CANNON ) return "";
	var hits = parseInt( cannon.GetTag( "hsCannonHits" ));
	if( isNaN( hits ) || hits <= 0 ) hits = 100;
	var stage = parseInt( cannon.GetTag( "hsCannonStage" ));
	var ammo = parseInt( cannon.GetTag( "hsCannonAmmo" ));
	var boat = cannon.multi;
	var hull = ValidateObject( boat ) && boat.GetHullMaxHits() > 0 ? Math.floor( boat.GetHullHits() * 100 / boat.GetHullMaxHits() ) : 0;
	cannon.SetTempTag( "clilocTooltip", 1042971 );
	return "Condition: " + hits + "%\nState: " + CannonStageName( stage ) +
		( stage >= 3 ? "\nAmmo: " + AmmoName( ammo ) : "" ) + "\nShip hull: " + hull + "%";
}

function SetCannonArt( cannon, boat )
{
	var facing = parseInt( boat.dir ) & 0x07;
	var artOffset = parseInt( cannon.GetTag( "hsCannonPower" )) == 2 ? 4 : 0;
	if( facing == 0 || facing == 4 )
	{
		if( cannon.x < boat.x ) cannon.id = 16919 + artOffset;      // West
		else if( cannon.x > boat.x ) cannon.id = 16921 + artOffset; // East
		else cannon.id = ( facing == 0 ? 16920 : 16918 ) + artOffset;
	}
	else
	{
		if( cannon.y < boat.y ) cannon.id = 16920 + artOffset;
		else if( cannon.y > boat.y ) cannon.id = 16918 + artOffset;
		else cannon.id = ( facing == 2 ? 16921 : 16919 ) + artOffset;
	}
	cannon.dir = facing;
}

// Scripted naval encounters use the same cannon objects, arcs, effects and hull
// damage as player-operated weapons. These entry points only replace the manual
// deed/loading clicks performed by a player crew.
function DeployNpcCannon( boat, pad, cannonPower )
{
	if( !ValidateObject( boat ) || !ValidateObject( pad ) || pad.multi != boat || pad.GetTag( "hsWeaponPad" ) != 1 ) return null;
	var existing = CalcItemFromSer( parseInt( pad.GetTag( "hsCannonSerial" )));
	if( ValidateObject( existing ) && existing.GetTag( "hsCannonKind" ) == CANNON ) return existing;
	var cannon = CreateDFNItem( null, null, cannonPower == 2 ? "highseas_heavy_cannon" : "highseas_light_cannon", 1,
		"ITEM", false, 0, boat.worldnumber, boat.instanceID );
	if( !ValidateObject( cannon )) return null;
	cannon.SetLocation( pad.x, pad.y, pad.z + GetTileHeight( pad.id ), boat.worldnumber, boat.instanceID );
	cannon.multi = boat;
	cannon.SetTag( "hsCannonKind", CANNON );
	cannon.SetTag( "hsCannonPower", cannonPower == 2 ? 2 : 1 );
	cannon.SetTag( "hsCannonStage", 1 );
	cannon.SetTag( "hsCannonShots", 0 );
	cannon.SetTag( "hsCannonHits", 100 );
	cannon.SetTag( "hsWeaponPadSerial", pad.serial );
	pad.SetTag( "hsCannonSerial", cannon.serial );
	SetCannonArt( cannon, boat );
	cannon.Refresh();
	return cannon;
}

function FireNpcCannon( cannon, targetBoat, captain )
{
	if( !ValidateObject( cannon ) || !ValidateObject( targetBoat ) || !ValidateObject( captain ) ||
		cannon.GetTag( "hsCannonKind" ) != CANNON || cannon.multi == targetBoat ||
		parseInt( cannon.GetTempTag( "hsCannonBusy" )) == 1 || !IsTargetInCannonArc( cannon, targetBoat )) return false;
	cannon.SetTag( "hsCannonStage", 4 );
	cannon.SetTag( "hsCannonAmmo", BALL_ID );
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", captain.serial );
	cannon.SetTempTag( "hsCannonTarget", targetBoat.serial );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( ACTION_TIME, TIMER_FIRE, 5099 );
	return true;
}
