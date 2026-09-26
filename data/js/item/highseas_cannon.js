/// <reference path="../definitions.d.ts" />
// @ts-check

// Implements High Seas cannon deployment, loading, targeting, firing, damage,
// repairs and persistence for player-controlled and scripted ships.

/** @type { ( user: Character, used: Item ) => any } */
function onUseChecked( user, used )
{
	if( used.cannonRole == 1 )
	{
		if( !IsInPack( used, user.pack ) )
		{
			user.socket.SysMessage( "The cannon deed must be in your backpack." );
			return false;
		}
		if( !ValidateObject( user.multi ) || !user.multi.IsBoat() )
		{
			user.socket.SysMessage( "You must be on the ship to deploy a weapon." );
			return false;
		}
		if( !user.multi.IsOwner( user ) )
		{
			user.socket.SysMessage( "You must be the owner of the ship to do this." );
			return false;
		}
		user.socket.tempObj = used;
		user.socket.CustomTarget( 0, "Target a weapon pad on your ship." );
		return false;
	}
	if( used.cannonRole != 2 )
	{
		return false;
	}
	if( !used.InRange( user, 3 ) )
	{
		user.socket.SysMessage( "You are too far away." );
		return false;
	}
	if( !CanOperateCannon( used.multi, user ) )
	{
		user.socket.SysMessage( "You do not have permission to use this ship cannon." );
		return false;
	}
	NormalizeCannonMagazine( used );
	AddCannonAction( used, user.multi == used.multi && used.InRange( user, 2 ) ? "You are now operating the cannon." : "You are too far away." );
	// Open the three-item magazine first, then display the control menu after
	// the client finishes its container-opening sequence.
	used.SetTempTag( "hsCannonOperator", user.serial );
	used.StartTimer( 500, 9, true );
	return true;
}

/** @type { ( dropped: Item, user: Character, cannon: Item ) => any } */
function onDropItemOnItem( dropped, user, cannon )
{
	if( !ValidateObject( dropped ) || !ValidateObject( user ) || !ValidateObject( cannon ) || cannon.cannonRole != 2 )
	{
		return 0;
	}
	if( !CanOperateCannon( cannon.multi, user ) || user.multi != cannon.multi || !cannon.InRange( user, 2 ) )
	{
		user.socket.SysMessage( "You may not use this cannon magazine." );
		return 0;
	}
	if( dropped.id != 0x4224 && dropped.id != 0xA2BF && dropped.id != 0x44C1 && dropped.id != 0x422B && dropped.id != 0xA2BE && dropped.id != 0x1420 )
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

/** @type { ( user: Character, used: Item ) => any } */
function OperateCannon( user, used )
{
	let boat = used.multi;
	if( !ValidateObject( boat ) || user.multi != boat )
	{
		user.socket.SysMessage( "You must be aboard the ship to operate this cannon." );
		return false;
	}
	if( parseInt( used.GetTempTag( "hsCannonBusy" ) ) == 1 )
	{
		user.socket.SysMessage( "The cannon operation is already in progress." );
		return false;
	}

	let stage = parseInt( used.cannonStage );
	if( stage < 1 )
	{
		if( user.ResourceCount( 0x4246, 0 ) < 1 )
		{
			user.socket.SysMessage( "You need a ramrod." );
			return false;
		}
		BeginCannonAction( used, user, 10, 4000, "Preparing to fire..." );
	}
	else if( stage == 1 )
	{
		if( CountCannonResource( used, 0xA2BE ) < 1 )
		{
			user.socket.SysMessage( "The magazine does not have a powder charge." );
			return false;
		}
		BeginCannonAction( used, user, 11, CannonActionTime( used ), "Charging started." );
	}
	else if( stage == 2 )
	{
		let ammo = SelectCannonAmmo( user, boat, used );
		if( CountCannonResource( used, ammo ) < 1 )
		{
			user.socket.SysMessage( "The magazine does not have ammunition." );
			return false;
		}
		used.SetTempTag( "hsPendingAmmo", ammo );
		BeginCannonAction( used, user, 12, CannonActionTime( used ), "Loading started." );
	}
	else if( stage == 3 )
	{
		if( CountCannonResource( used, 0x1420 ) < 1 )
		{
			user.socket.SysMessage( "The magazine does not have fuse cord." );
			return false;
		}
		BeginCannonAction( used, user, 13, CannonActionTime( used ), "Priming started." );
	}
	else
	{
		LightCannonFuse( user, used );
	}
	return false;
}

/** @type { ( boat: Multi, user: Character ) => any } */
function CanOperateCannon( boat, user ) { return ValidateObject( boat ) && boat.CanCommandShip( user ); }

/** @type { ( cannon: Item ) => any } */
function NormalizeCannonMagazine( cannon )
{
	if( !ValidateObject( cannon ) || !cannon.isShipCannon )
	{
		return;
	}
	cannon.type = 1;
	cannon.maxItems = 3;
	cannon.weightMax = 30000;
}

/** @type { ( boat: Multi, user: Character ) => any } */
function IsShipOfficer( boat, user ) { return ValidateObject( boat ) && boat.GetSecurityLevel( user ) >= 4; }

/** @type { ( socket: Socket, cannon: Item ) => any } */
function onContextMenuRequest( socket, cannon )
{
	let user = socket.currentChar;
	let boat = ValidateObject( cannon ) ? cannon.multi : null;
	if( !ValidateObject( user ) || !cannon.isShipCannon || !IsShipOfficer( boat, user ) || !cannon.InRange( user, 2 ) )
	{
		return true;
	}
	let stage = parseInt( cannon.cannonStage );
	let hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 )
	{
		hits = 100;
	}
	let entries = [
		{ id : 40, text : 1116072, flags : stage == 4 ? 0x0000 : 0x0001, hue : 0x03E0 },
		{ id : 41, text : 1116069, flags : stage <= 1 && cannon.totalItemCount == 0 && hits >= 100 ? 0x0000 : 0x0001, hue : 0x03E0 },
		{ id : 42, text : 1116602, flags : hits < 100 ? 0x0000 : 0x0001, hue : 0x03E0 }
	];
	TriggerEvent( 18001, "modifyContextMenu", socket, cannon, entries, true );
	return false;
}

/** @type { ( socket: Socket, cannon: Item, popupEntry: number ) => any } */
function onContextMenuSelect( socket, cannon, popupEntry )
{
	let user = socket.currentChar;
	let boat = ValidateObject( cannon ) ? cannon.multi : null;
	if( !ValidateObject( user ) || !IsShipOfficer( boat, user ) || !cannon.InRange( user, 2 ) )
	{
		return false;
	}
	if( popupEntry == 40 )
	{
		UnloadCannon( user, cannon );
	}
	else if( popupEntry == 41 )
	{
		DismantleCannon( user, cannon );
	}
	else if( popupEntry == 42 )
	{
		RepairCannon( user, cannon );
	}
	return false;
}

/** @type { ( user: Character, cannon: Item ) => any } */
function ShowCannonGump( user, cannon )
{
	let boat = cannon.multi;
	if( !CanOperateCannon( boat, user ) )
	{
		user.socket.SysMessage( "Only the ship's captain and authorized crew may operate this cannon." );
		return;
	}
	let linkedPad = CalcItemFromSer( parseInt( cannon.cannonLinkSerial ) );
	if( !ValidateObject( linkedPad ) )
	{
		AreaItemFunction( "RestoreHighSeasCannonPad", cannon, 2 );
	}
	user.SetTempTag( "hsCannonMenu", cannon.serial );
	let stage = parseInt( cannon.cannonStage );
	let g = new Gump;
	g.AddPage( 0 );
	// Compact ship-cannon gump footprint and control placement.
	g.AddBackground( 0, 0, 250, 175, 0x06DB );
	g.AddHTMLGump( 10, 10, 230, 18, false, false, "<BASEFONT COLOR=#7FCFFF>" + CannonPositionName( cannon ) + "</BASEFONT>" );
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
	AddCannonStatusLine( g, 71, stage >= 3 ? AmmoName( parseInt( cannon.morex ) ) : "Not Loaded", stage >= 3 );
	AddCannonStatusLine( g, 89, stage >= 4 ? "Primed" : "No Fuse", stage >= 4 );
	for( let i = 0; i < 3; ++i )
	{
		let action = cannon.GetTempTag( "hsCannonAction" + i );
		if( action )
		{
			g.AddHTMLGump( 10, 112 + i * 18, 230, 18, false, false, "<BASEFONT COLOR=" + ( i == 0 ? "#FFFFFF" : "#B0B0B0" ) + ">" + action + "</BASEFONT>" );
		}
	}
	g.Send( user );
	g.Free();
}

/** @type { ( user: Character, cannon: Item ) => any } */
function IsViewingCannonGump( user, cannon )
{
	return ValidateObject( user ) && ValidateObject( cannon ) && parseInt( user.GetTempTag( "hsCannonMenu" ) ) == cannon.serial;
}

/** @type { ( g: Gump, y: number, text: string, good: boolean ) => any } */
function AddCannonStatusLine( g, y, text, good )
{
	g.AddHTMLGump( 115, y, 125, 18, false, false, "<BASEFONT COLOR=" + ( good ? "#80FF80" : "#FF8080" ) + ">" + text + "</BASEFONT>" );
}

/** @type { ( cannon: Item ) => any } */
function CannonPositionName( cannon )
{
	let boat = cannon.multi;
	if( !ValidateObject( boat ) )
	{
		return CannonName( cannon );
	}
	let dx = cannon.x - boat.x, dy = cannon.y - boat.y;
	let facing = parseInt( boat.dir ) & 0x06;
	let localX = dx, localY = dy;
	if( facing == 2 )
	{
		localX = dy;
		localY = -dx;
	}
	else if( facing == 4 )
	{
		localX = -dx;
		localY = -dy;
	}
	else if( facing == 6 )
	{
		localX = -dy;
		localY = dx;
	}
	let side = localX < 0 ? "Port" : ( localX > 0 ? "Starboard" : "" );
	let section = localY < -2 ? "Bow" : ( localY > 2 ? "Aft" : "Amidship" );
	return section + ( side ? " " + side : "" ) + " Cannon";
}

/** @type { ( user: Character, cannon: Item, socket: Socket ) => boolean } */
function FireHighSeasPortBroadsideCannon( user, cannon, socket ) { return FireHighSeasBroadsideCannon( user, cannon, socket, -1 ); }

/** @type { ( user: Character, cannon: Item, socket: Socket ) => boolean } */
function FireHighSeasStarboardBroadsideCannon( user, cannon, socket ) { return FireHighSeasBroadsideCannon( user, cannon, socket, 1 ); }

/** @type { ( user: Character, cannon: Item, socket: Socket, requestedSide: number ) => boolean } */
function FireHighSeasBroadsideCannon( user, cannon, socket, requestedSide )
{
	if( !ValidateObject( user ) || !ValidateObject( cannon ) || !socket || cannon.cannonRole != 2 || parseInt( cannon.cannonStage ) != 4
		|| parseInt( cannon.GetTempTag( "hsCannonBusy" ) ) == 1 )
	{
		return false;
	}
	let boat = cannon.multi;
	if( !ValidateObject( boat ) || user.multi != boat || !CanOperateCannon( boat, user ) )
	{
		return false;
	}
	let dx = cannon.x - boat.x;
	let dy = cannon.y - boat.y;
	let facing = parseInt( boat.dir ) & 0x06;
	let localX = dx;
	if( facing == 2 )
	{
		localX = dy;
	}
	else if( facing == 4 )
	{
		localX = -dx;
	}
	else if( facing == 6 )
	{
		localX = -dy;
	}
	if( ( requestedSide < 0 && localX >= 0 ) || ( requestedSide > 0 && localX <= 0 ) )
	{
		return false;
	}
	if( user.region && user.region.isGuarded )
	{
		return false;
	}
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", user.serial );
	cannon.SetTempTag( "hsCannonTarget", 0 );
	ResolveAutomaticCannonTarget( cannon, user );
	AddCannonAction( cannon, "The fuse is lit by broadside command!" );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( 1500, 14, true );
	return true;
}

/** @type { ( cannon: Item, message: string ) => any } */
function AddCannonAction( cannon, message )
{
	if( !ValidateObject( cannon ) || !message )
	{
		return;
	}
	cannon.SetTempTag( "hsCannonAction2", cannon.GetTempTag( "hsCannonAction1" ) );
	cannon.SetTempTag( "hsCannonAction1", cannon.GetTempTag( "hsCannonAction0" ) );
	cannon.SetTempTag( "hsCannonAction0", message );
}

/** @type { ( cannon: Item, candidate: Item ) => any } */
function RestoreHighSeasCannonPad( cannon, candidate )
{
	if( !ValidateObject( candidate ) || !candidate.isWeaponPad || candidate.multi != cannon.multi || candidate.x != cannon.x || candidate.y != cannon.y )
	{
		return false;
	}
	cannon.cannonLinkSerial = candidate.serial;
	candidate.cannonLinkSerial = cannon.serial;
	return true;
}

/** @type { ( stage: number ) => any } */
function CannonStageName( stage )
{
	if( stage == 1 )
	{
		return "prepared";
	}
	if( stage == 2 )
	{
		return "charged";
	}
	if( stage == 3 )
	{
		return "loaded";
	}
	if( stage == 4 )
	{
		return "ready";
	}
	return "empty";
}

/** @type { ( cannon: Item ) => any } */
function CannonName( cannon )
{
	let power = parseInt( cannon.cannonPower );
	if( power == 3 )
	{
		return "Blundercannon";
	}
	if( power == 4 )
	{
		return "Pumpkin Cannon";
	}
	return power == 2 ? "Heavy Ship Cannon" : "Light Ship Cannon";
}

/** @type { ( cannon: Item ) => any } */
function CannonRange( cannon )
{
	let configuredRange = parseInt( cannon.cannonRange );
	if( configuredRange > 0 )
	{
		return configuredRange;
	}
	let power = parseInt( cannon.cannonPower );
	if( power == 3 )
	{
		return 12;
	}
	if( power == 4 )
	{
		return 10;
	}
	return power == 2 ? 10 : 10;
}

/** @type { ( socket: Socket, button: number, gumpData: GumpData ) => any } */
function onGumpPress( socket, button, gumpData )
{
	let user = socket.currentChar;
	let cannon = CalcItemFromSer( parseInt( user.GetTempTag( "hsCannonMenu" ) ) );
	// Right-click/close sends button zero. Clearing the viewing tag prevents both this
	// response and later action timers from reopening the window.
	if( button == 0 )
	{
		user.SetTempTag( "hsCannonMenu", null );
		return;
	}
	if( !ValidateObject( cannon ) || !cannon.isShipCannon )
	{
		return;
	}
	let boat = cannon.multi;
	if( !CanOperateCannon( boat, user ) )
	{
		return;
	}
	if( user.multi != boat || !cannon.InRange( user, 2 ) || user.dead )
	{
		AddCannonAction( cannon, "You are too far away." );
		ShowCannonGump( user, cannon );
		return;
	}
	if( button == 1 )
	{
		OperateCannon( user, cannon );
	}
	else if( button == 6 && parseInt( cannon.cannonStage ) == 4 )
	{
		LightCannonFuse( user, cannon );
	}
	else if( button == 8 && parseInt( cannon.cannonStage ) == 4 )
	{
		UnloadCannon( user, cannon );
	}
	ShowCannonGump( user, cannon );
}

/** @type { ( user: Character, boat: Multi, cannon: Item ) => any } */
function SelectCannonAmmo( user, boat, cannon )
{
	// Load a standard cannonball first when one is present in the magazine, then
	// fall back to grapeshot. Special cannonballs remain valid cannonballs.
	let choices = [ 0x4224, 0x422B, 0x44C1, 0xA2BF ];
	for( let i = 0; i < choices.length; ++i )
	{
		if( CountCannonResource( cannon, choices[i] ) > 0 )
		{
			return choices[i];
		}
	}
	return 0x4224;
}

/** @type { ( cannon: Item, itemID: number ) => any } */
function CountCannonResource( cannon, itemID ) { return ValidateObject( cannon ) ? cannon.ResourceCount( itemID, -1 ) : 0; }

/** @type { ( cannon: Item, amount: number, itemID: number ) => any } */
function ConsumeCannonResource( cannon, amount, itemID )
{
	if( ValidateObject( cannon ) )
	{
		cannon.UseResource( amount, itemID, -1 );
	}
}

/** @type { ( cannon: Item, itemID: number ) => any } */
function FindCannonResource( cannon, itemID )
{
	if( !ValidateObject( cannon ) )
	{
		return null;
	}
	for( let item = cannon.FirstItem(); !cannon.FinishedItems(); item = cannon.NextItem() )
	{
		if( ValidateObject( item ) && item.id == itemID )
		{
			return item;
		}
	}
	return null;
}

/** @type { ( ammo: number ) => any } */
function AmmoName( ammo )
{
	if( ammo == 2 )
	{
		return "grapeshot";
	}
	if( ammo == 3 )
	{
		return "flame cannonball";
	}
	if( ammo == 4 )
	{
		return "frost cannonball";
	}
	return "standard cannonball";
}

/** @type { ( cannon: Item ) => any } */
function ClearLoadedAmmoProfile( cannon )
{
	cannon.morex = 0;
	cannon.morey = 0;
	cannon.morez = 0;
	cannon.lodamage = 0;
	cannon.hidamage = 0;
}

/** @type { ( user: Character, cannon: Item ) => any } */
function UnloadCannon( user, cannon )
{
	let boat = cannon.multi;
	if( !IsShipOfficer( boat, user ) )
	{
		user.socket.SysMessage( "Only the captain or a ship officer may unload this cannon." );
		return;
	}
	if( parseInt( cannon.GetTempTag( "hsCannonBusy" ) ) == 1 )
	{
		user.socket.SysMessage( "The cannon is currently being operated." );
		return;
	}
	let stage = parseInt( cannon.cannonStage );
	if( stage != 4 )
	{
		user.socket.SysMessage( "The ship cannon is not fully loaded." );
		return;
	}
	if( stage >= 4 )
	{
		CreateDFNItem( user.socket, user, "highseas_fuse_cord", 1, "ITEM", true );
	}
	if( stage >= 3 )
	{
		let loadedAmmo = parseInt( cannon.morex );
		let ammoSection = loadedAmmo == 2
			? "highseas_grapeshot"
			: ( loadedAmmo == 3 ? "highseas_flame_cannonball" : ( loadedAmmo == 4 ? "highseas_frost_cannonball" : "highseas_cannonball" ) );
		CreateDFNItem( user.socket, user, ammoSection, 1, "ITEM", true );
	}
	if( stage >= 2 )
	{
		CreateDFNItem( user.socket, user, "highseas_powder_charge", 1, "ITEM", true );
	}
	cannon.cannonStage = stage == 0 ? 0 : 1;
	ClearLoadedAmmoProfile( cannon );
	AddCannonAction( cannon, "Cannon unloaded." );
	user.socket.SysMessage( "The cannon has been fully unloaded and its supplies returned." );
}

/** @type { ( user: Character, cannon: Item ) => any } */
function RepairCannon( user, cannon )
{
	let boat = cannon.multi;
	if( !IsShipOfficer( boat, user ) )
	{
		user.socket.SysMessage( "Only the captain or a ship officer may repair this cannon." );
		return;
	}
	let hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 )
	{
		hits = 100;
	}
	if( hits >= 100 )
	{
		user.socket.SysMessage( "The cannon is already in pristine condition." );
		return;
	}
	let level = hits >= 75 ? 1 : ( hits >= 50 ? 2 : ( hits >= 25 ? 3 : 4 ) );
	let skill = parseInt( user.skills.blacksmithing ) / 10;
	let needed = 36 * level;
	needed -= ( skill / 200 ) * needed;
	needed = Math.max( 1, needed );
	let available = CountShipResource( user, boat, 0x1BF2 );
	let minimum = Math.max( 1, Math.floor( needed / 10 ) );
	if( available < minimum )
	{
		user.socket.SysMessage( "You need at least " + minimum + " iron ingots to repair this cannon." );
		return;
	}
	let used = Math.max( 1, Math.floor( Math.min( available, needed ) ) );
	let percent = Math.min( 1, available / needed );
	ConsumeShipResource( user, boat, used, 0x1BF2 );
	hits += Math.floor( ( 100 - hits ) * percent );
	if( hits > 100 )
	{
		hits = 100;
	}
	cannon.health = hits;
	cannon.Refresh();
	user.socket.SysMessage( "You use " + used + " iron ingots. The cannon is now " + hits + "% repaired." );
}

/** @type { ( user: Character, boat: Multi ) => any } */
function RepairShipHull( user, boat )
{
	if( !CanOperateCannon( boat, user ) )
	{
		user.socket.SysMessage( "Only authorized ship crew may repair this vessel." );
		return;
	}
	if( user.multi != boat )
	{
		user.socket.SysMessage( "You must be aboard the vessel to repair it." );
		return;
	}
	if( !boat.IsNearLandOrDocks() )
	{
		user.socket.SysMessage( "Permanent ship repairs may only be made near land or docks." );
		return;
	}
	let hits = boat.GetHullHits();
	let maxHits = boat.GetHullMaxHits();
	if( maxHits <= 0 || hits >= maxHits )
	{
		user.socket.SysMessage( "The ship is already fully repaired." );
		return;
	}
	let durability = hits * 100 / maxHits;
	let woodNeeded = 17 * ( 100 - durability );
	let clothNeeded = 17 * ( 100 - durability );
	woodNeeded -= ( ( parseInt( user.skills.carpentry ) / 10 ) / 200 ) * woodNeeded;
	clothNeeded -= ( ( parseInt( user.skills.tailoring ) / 10 ) / 200 ) * clothNeeded;
	woodNeeded = Math.max( 1, woodNeeded );
	clothNeeded = Math.max( 1, clothNeeded );
	let wood = CountShipResource( user, boat, 0x1BD7 );
	let cloth = CountShipResource( user, boat, 0x1766 );
	let minWood = Math.max( 1, Math.floor( woodNeeded / 10 ) );
	let minCloth = Math.max( 1, Math.floor( clothNeeded / 10 ) );
	if( wood < minWood || cloth < minCloth )
	{
		user.socket.SysMessage( "You need at least " + minCloth + " cloth and " + minWood + " boards to repair this ship." );
		return;
	}
	let woodUsed = Math.min( wood, woodNeeded );
	let clothUsed = Math.min( cloth, clothNeeded );
	let percWood = Math.min( 100, wood / woodNeeded * 100 );
	let percCloth = Math.min( 100, cloth / clothNeeded * 100 );
	if( clothUsed > woodUsed )
	{
		clothUsed = woodUsed;
		percCloth = percWood;
	}
	else if( woodUsed > clothUsed )
	{
		woodUsed = clothUsed;
		percWood = percCloth;
	}
	let totalPercent = ( percWood + percCloth ) / 2;
	let consumeWood = Math.max( 1, Math.floor( woodUsed ) );
	let consumeCloth = Math.max( 1, Math.floor( clothUsed ) );
	ConsumeShipResource( user, boat, consumeWood, 0x1BD7 );
	ConsumeShipResource( user, boat, consumeCloth, 0x1766 );
	let repaired = boat.RepairHull( Math.floor( ( maxHits - hits ) * totalPercent / 100 ) );
	let newPercent = Math.floor( boat.GetHullHits() * 100 / maxHits );
	user.socket.SysMessage( "You use " + consumeCloth + " cloth and " + consumeWood + " boards. The ship is now " + newPercent + "% repaired." );
}

/** @type { ( user: Character, cannon: Item ) => any } */
function DismantleCannon( user, cannon )
{
	let boat = cannon.multi;
	if( !ValidateObject( boat ) || boat.GetSecurityLevel( user ) < 4 )
	{
		user.socket.SysMessage( "Only the ship's captain or an officer may dismantle this cannon." );
		return;
	}
	let hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 )
	{
		hits = 100;
	}
	if( parseInt( cannon.cannonStage ) > 1 || cannon.totalItemCount > 0 )
	{
		user.socket.SysMessage( "The ship cannon and magazine must be fully unloaded before it can be dismantled." );
		return;
	}
	if( hits < 100 )
	{
		user.socket.SysMessage( "The cannon must be fully repaired before it can be dismantled." );
		return;
	}
	let pad = CalcItemFromSer( parseInt( cannon.cannonLinkSerial ) );
	if( ValidateObject( pad ) )
	{
		pad.cannonLinkSerial = 0;
	}
	let power = parseInt( cannon.cannonPower );
	CreateDFNItem( user.socket, user,
		power == 4 ? "pumpkin_cannon_deed"
				   : ( power == 3 ? "blundercannon_deed" : ( power == 2 ? "highseas_heavy_cannon_deed" : "highseas_light_cannon_deed" ) ),
		1, "ITEM", true );
	cannon.Delete();
	user.socket.SysMessage( "You dismantle the ship cannon and recover its deed." );
}

/** @type { ( user: Character, boat: Multi ) => any } */
function BeginEmergencyRepairs( user, boat )
{
	if( !CanOperateCannon( boat, user ) )
	{
		user.socket.SysMessage( "Only authorized ship crew may order emergency repairs." );
		return;
	}
	if( user.multi != boat )
	{
		user.socket.SysMessage( "You must be aboard the vessel." );
		return;
	}
	let hits = boat.GetHullHits();
	let maxHits = boat.GetHullMaxHits();
	if( maxHits <= 0 || hits * 4 >= maxHits )
	{
		user.socket.SysMessage( "Emergency repairs are only needed on a scuttled vessel." );
		return;
	}
	if( boat.IsUnderEmergencyRepairs() )
	{
		user.socket.SysMessage( "Emergency repairs are already holding this vessel together." );
		return;
	}
	if( CountShipResource( user, boat, 0x1766 ) < 55 || CountShipResource( user, boat, 0x1BD7 ) < 25 )
	{
		user.socket.SysMessage( "Emergency repairs require 55 cloth and 25 boards." );
		return;
	}
	ConsumeShipResource( user, boat, 55, 0x1766 );
	ConsumeShipResource( user, boat, 25, 0x1BD7 );
	boat.StartEmergencyRepairs( 360 );
	user.socket.SysMessage( "Emergency repairs will hold the scuttled vessel together for six minutes." );
}

/** @type { ( cannon: Item, user: Character, timerID: number, delay: number, message: string ) => any } */
function BeginCannonAction( cannon, user, timerID, delay, message )
{
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", user.serial );
	AddCannonAction( cannon, message );
	user.socket.SysMessage( message );
	cannon.StartTimer( delay, timerID, true );
}

/** @type { ( cannon: Item ) => any } */
function CannonActionTime( cannon ) { return cannon.cannonActionTime > 0 ? cannon.cannonActionTime : 1500; }

/** @type { ( user: Character, boat: Multi, itemID: number ) => any } */
function CountShipResource( user, boat, itemID )
{
	let amount = user.ResourceCount( itemID, 0 );
	let hold = ValidateObject( boat ) ? boat.GetHold() : null;
	if( ValidateObject( hold ) )
	{
		amount += hold.ResourceCount( itemID, 0 );
	}
	return amount;
}

/** @type { ( user: Character, boat: Multi, amount: number, itemID: number ) => any } */
function ConsumeShipResource( user, boat, amount, itemID )
{
	let fromPack = Math.min( amount, user.ResourceCount( itemID, 0 ) );
	if( fromPack > 0 )
	{
		user.UseResource( fromPack, itemID, 0 );
	}
	let remaining = amount - fromPack;
	let hold = ValidateObject( boat ) ? boat.GetHold() : null;
	if( remaining > 0 && ValidateObject( hold ) )
	{
		hold.UseResource( remaining, itemID, 0 );
	}
}

/** @type { ( item: Item, pack: BaseObject ) => any } */
function IsInPack( item, pack )
{
	let parent = item.container;
	while( ValidateObject( parent ) )
	{
		if( parent == pack )
		{
			return true;
		}
		parent = parent.container;
	}
	return false;
}

/** @type { ( socket: Socket, target: Character | Item | null ) => any } */
function onCallback0( socket, target )
{
	let deed = socket.tempObj;
	socket.tempObj = null;
	let deedPower = ValidateObject( deed ) ? parseInt( deed.cannonPower ) : 1;
	let cannonPower = deedPower == 4 ? 4 : ( deedPower == 3 ? 3 : ( deedPower == 2 ? 2 : 1 ) );
	if( !ValidateObject( deed ) || parseInt( socket.GetWord( 11 ) ) == 0xFFFF )
	{
		return;
	}
	let targetX = parseInt( socket.GetWord( 11 ) );
	let targetY = parseInt( socket.GetWord( 13 ) );
	let user = socket.currentChar;
	if( !ValidateObject( target ) || !target.isItem || !target.isWeaponPad )
	{
		user.SetTempTag( "hsCannonTargetX", targetX );
		user.SetTempTag( "hsCannonTargetY", targetY );
		user.SetTempTag( "hsCannonTargetPad", 0 );
		AreaItemFunction( "FindWeaponPadAtTarget", user, 40 );
		target = CalcItemFromSer( parseInt( user.GetTempTag( "hsCannonTargetPad" ) ) );
		user.SetTempTag( "hsCannonTargetX", null );
		user.SetTempTag( "hsCannonTargetY", null );
		user.SetTempTag( "hsCannonTargetPad", null );
	}
	if( !ValidateObject( target ) || !target.isItem || !target.isWeaponPad )
	{
		socket.SysMessage( "There is no weapon pad at that location." );
		return;
	}
	let boat = target.multi;
	if( !ValidateObject( boat ) || !boat.IsBoat() )
	{
		socket.SysMessage( "That weapon pad is not attached to a ship." );
		return;
	}
	if( !boat.IsOwner( user ) )
	{
		socket.SysMessage( "You may only place this on your own ship." );
		return;
	}
	if( !target.InRange( user, 3 ) )
	{
		socket.SysMessage( "You must move closer to the weapon pad." );
		return;
	}
	let existingSerial = parseInt( target.cannonLinkSerial );
	if( existingSerial )
	{
		let existing = CalcItemFromSer( existingSerial );
		if( ValidateObject( existing ) && existing.isShipCannon )
		{
			// Place the cannon at the weapon pad's calculated tile top rather than
			// at the pad's base Z.
			existing.SetLocation( target.x, target.y, target.z + GetTileHeight( target.id ), target.worldnumber, target.instanceID );
			existing.multi = boat;
			existing.cannonLinkSerial = target.serial;
			if( parseInt( existing.health ) <= 0 )
			{
				existing.health = 100;
			}
			NormalizeCannonMagazine( existing );
			SetCannonArt( existing, boat );
			existing.Refresh();
			socket.SysMessage( "The existing cannon has been restored to the weapon pad." );
			return;
		}
		target.cannonLinkSerial = 0;
	}
	let cannon = CreateDFNItem( socket, socket.currentChar,
		cannonPower == 4 ? "pumpkin_cannon" : ( cannonPower == 3 ? "blundercannon" : ( cannonPower == 2 ? "highseas_heavy_cannon" : "highseas_light_cannon" ) ),
		1, "ITEM", false );
	if( !ValidateObject( cannon ) )
	{
		return;
	}
	cannon.SetLocation( target.x, target.y, target.z + GetTileHeight( target.id ), target.worldnumber, target.instanceID );
	cannon.multi = boat;
	SetCannonArt( cannon, boat );
	cannon.cannonRole = 2;
	cannon.cannonPower = cannonPower;
	cannon.cannonStage = 0;
	cannon.health = 100;
	cannon.maxhp = 100;
	cannon.cannonLinkSerial = target.serial;
	target.cannonLinkSerial = cannon.serial;
	NormalizeCannonMagazine( cannon );
	deed.Delete();
}

/** @type { ( user: Character, candidate: Item ) => any } */
function FindWeaponPadAtTarget( user, candidate )
{
	if( !ValidateObject( candidate ) || !candidate.isWeaponPad || candidate.x != parseInt( user.GetTempTag( "hsCannonTargetX" ) )
		|| candidate.y != parseInt( user.GetTempTag( "hsCannonTargetY" ) ) )
	{
		return false;
	}
	let boat = candidate.multi;
	if( !ValidateObject( boat ) || !boat.IsBoat() )
	{
		return false;
	}
	user.SetTempTag( "hsCannonTargetPad", candidate.serial );
	return true;
}

/** @type { ( socket: Socket, target: Character | Item | null ) => any } */
function onCallback1( socket, target )
{
	let cannon = socket.tempObj;
	socket.tempObj = null;
	if( !ValidateObject( cannon ) || parseInt( cannon.cannonStage ) != 4 )
	{
		return;
	}
	if( !CanOperateCannon( cannon.multi, socket.currentChar ) || socket.currentChar.multi != cannon.multi )
	{
		socket.SysMessage( "You are no longer authorized to operate this cannon." );
		return;
	}
	if( socket.currentChar.region && socket.currentChar.region.isGuarded )
	{
		socket.SysMessage( "You are forbidden from discharging cannons within guarded town limits." );
		return;
	}
	let targetBoat = ResolveTargetBoat( target );
	let cannonTarget = targetBoat;
	if( !ValidateObject( cannonTarget ) && CannonCharacterTargetingEnabled() && IsValidCannonCharacterTarget( cannon, socket.currentChar, target ) )
	{
		cannonTarget = target;
	}
	if( !ValidateObject( cannonTarget ) )
	{
		socket.SysMessage( CannonCharacterTargetingEnabled() ? "That is not a valid ship or character target." : "That is not an enemy ship." );
		return;
	}
	if( ValidateObject( targetBoat ) && cannon.multi == targetBoat )
	{
		socket.SysMessage( "You cannot fire on your own ship." );
		return;
	}
	if( ValidateObject( targetBoat ) && AreFriendlyShips( cannon.multi, targetBoat ) )
	{
		socket.SysMessage( "You cannot fire on a friendly, allied, guild, or party vessel." );
		return;
	}
	if( !IsTargetInCannonArc( cannon, cannonTarget ) )
	{
		socket.SysMessage( "That target is outside this cannon's firing arc or range." );
		return;
	}
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", socket.currentChar.serial );
	cannon.SetTempTag( "hsCannonTarget", cannonTarget.serial );
	socket.SysMessage( "The fuse is lit!" );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( 1500, 14, true );
}

/** @type { ( sourceBoat: Multi, targetBoat: Multi ) => any } */
function AreFriendlyShips( sourceBoat, targetBoat )
{
	if( !ValidateObject( sourceBoat ) || !ValidateObject( targetBoat ) )
	{
		return false;
	}
	let sourceOwner = sourceBoat.owner;
	let targetOwner = targetBoat.owner;
	if( !ValidateObject( sourceOwner ) || !ValidateObject( targetOwner ) )
	{
		return false;
	}
	if( sourceOwner == targetOwner )
	{
		return true;
	}
	if( sourceOwner.party != null && targetOwner.party != null && sourceOwner.party == targetOwner.party )
	{
		return true;
	}
	if( sourceOwner.guild != null && targetOwner.guild != null )
	{
		let relation = CompareGuildByGuild( sourceOwner.guild, targetOwner.guild );
		if( relation == 2 || relation == 4 )
		{
			return true;
		}
	}
	return false;
}

/** @type { ( target: Character | Item | null ) => any } */
function ResolveTargetBoat( target )
{
	if( !ValidateObject( target ) )
	{
		return null;
	}
	if( target.isItem && target.IsBoat() )
	{
		return target;
	}
	if( ValidateObject( target.multi ) && target.multi.IsBoat() )
	{
		return target.multi;
	}
	return null;
}

/** @type { () => boolean } */
function CannonCharacterTargetingEnabled() { return GetServerSetting( "CANNONCHARACTERTARGETING" ) == true; }

/** @type { ( cannon: Item, shooter: Character, target: Character | Item | null ) => boolean } */
function IsValidCannonCharacterTarget( cannon, shooter, target )
{
	if( !ValidateObject( target ) || !target.isChar || target == shooter || target.dead || !target.vulnerable )
	{
		return false;
	}
	if( target.region && target.region.isGuarded )
	{
		return false;
	}
	if( ValidateObject( target.multi ) && target.multi.IsBoat() )
	{
		return false;
	}
	if( ValidateObject( shooter ) && shooter.party != null && target.party != null && shooter.party == target.party )
	{
		return false;
	}
	if( ValidateObject( shooter ) && shooter.guild != null && target.guild != null )
	{
		let relation = CompareGuildByGuild( shooter.guild, target.guild );
		if( relation == 2 || relation == 4 )
		{
			return false;
		}
	}
	return IsTargetInCannonArc( cannon, target );
}

/** @type { ( user: Character, cannon: Item ) => any } */
function LightCannonFuse( user, cannon )
{
	if( user.region && user.region.isGuarded )
	{
		user.socket.SysMessage( "You are forbidden from discharging cannons within guarded town limits." );
		return false;
	}
	if( !HasLitTorch( user ) )
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
	cannon.StartTimer( 1500, 14, true );
	return true;
}

/** @type { ( user: Character ) => any } */
function HasLitTorch( user )
{
	for( let layer = 1; layer <= 2; ++layer )
	{
		let held = user.FindItemLayer( layer );
		if( ValidateObject( held ) && held.id >= 0x0A12 && held.id <= 0x0A15 )
		{
			return true;
		}
	}
	return ContainerHasLitTorch( user.pack );
}

/** @type { ( container: Item ) => any } */
function ContainerHasLitTorch( container )
{
	if( !ValidateObject( container ) )
	{
		return false;
	}
	for( let item = container.FirstItem(); !container.FinishedItems(); item = container.NextItem() )
	{
		if( !ValidateObject( item ) )
		{
			continue;
		}
		if( item.id >= 0x0A12 && item.id <= 0x0A15 )
		{
			return true;
		}
		if( item.itemsinside > 0 && ContainerHasLitTorch( item ) )
		{
			return true;
		}
	}
	return false;
}

/** @type { ( cannon: Item, user: Character ) => any } */
function ResolveAutomaticCannonTarget( cannon, user )
{
	let range = CannonRange( cannon );
	cannon.SetTempTag( "hsTrajectoryOperator", ValidateObject( user ) ? user.serial : 0 );
	cannon.SetTempTag( "hsTrajectoryBest", range + 1 );
	cannon.SetTempTag( "hsCannonTarget", 0 );
	AreaItemFunction( "FindCannonTrajectoryTarget", cannon, range );
	if( CannonCharacterTargetingEnabled() )
	{
		AreaCharacterFunction( "FindCannonTrajectoryCharacterTarget", cannon, range );
	}
	return ResolveStoredCannonTarget( cannon );
}

/** @type { ( cannon: Item ) => any } */
function ResolveStoredCannonTarget( cannon )
{
	let targetSerial = parseInt( cannon.GetTempTag( "hsCannonTarget" ) );
	let targetItem = CalcItemFromSer( targetSerial );
	if( ValidateObject( targetItem ) )
	{
		return targetItem;
	}
	return CalcCharFromSer( targetSerial );
}

/** @type { ( cannon: Item, candidate: Item ) => any } */
function FindCannonTrajectoryTarget( cannon, candidate )
{
	if( !ValidateObject( candidate ) || !candidate.IsBoat() || candidate == cannon.multi || AreFriendlyShips( cannon.multi, candidate ) )
	{
		return false;
	}
	let dx = candidate.x - cannon.x;
	let dy = candidate.y - cannon.y;
	let vx = 0, vy = 0;
	switch( GetCannonFacing( cannon ) )
	{
	case 0:
		vy = -1;
		break;
	case 2:
		vx = 1;
		break;
	case 4:
		vy = 1;
		break;
	case 6:
		vx = -1;
		break;
	}
	let forward = dx * vx + dy * vy;
	let lateral = Math.abs( dx * vy - dy * vx );
	let range = CannonRange( cannon );
	if( forward <= 0 || forward > range || lateral > 1 + Math.floor( forward / 3 ) )
	{
		return false;
	}
	if( forward < parseInt( cannon.GetTempTag( "hsTrajectoryBest" ) ) )
	{
		cannon.SetTempTag( "hsTrajectoryBest", forward );
		cannon.SetTempTag( "hsCannonTarget", candidate.serial );
	}
	return true;
}

/** @type { ( cannon: Item, target: Character | Item | null ) => any } */
function IsTargetInCannonArc( cannon, target )
{
	let dx = target.x - cannon.x;
	let dy = target.y - cannon.y;
	let vx = 0, vy = 0;
	switch( GetCannonFacing( cannon ) )
	{
	case 0:
		vy = -1;
		break;
	case 2:
		vx = 1;
		break;
	case 4:
		vy = 1;
		break;
	case 6:
		vx = -1;
		break;
	}
	let forward = dx * vx + dy * vy;
	let lateral = Math.abs( dx * vy - dy * vx );
	let range = CannonRange( cannon );
	return forward > 0 && forward <= range && lateral <= 1 + Math.floor( forward / 3 );
}

/** @type { ( cannon: Item ) => any } */
function GetCannonFacing( cannon )
{
	if( cannon.id == cannon.cannonArtSouth || cannon.id == 16918 || cannon.id == 16922 || cannon.id == 41979 )
	{
		return 4;
	}
	if( cannon.id == cannon.cannonArtWest || cannon.id == 16919 || cannon.id == 16923 || cannon.id == 41980 )
	{
		return 6;
	}
	if( cannon.id == cannon.cannonArtNorth || cannon.id == 16920 || cannon.id == 16924 || cannon.id == 41981 )
	{
		return 0;
	}
	return 2;
}

/** @type { ( cannon: Item, timerID: number ) => any } */
function onTimer( cannon, timerID )
{
	if( !ValidateObject( cannon ) )
	{
		return;
	}
	let user = CalcCharFromSer( parseInt( cannon.GetTempTag( "hsCannonOperator" ) ) );
	if( timerID == 9 )
	{
		if( ValidateObject( user ) && user.socket && cannon.InRange( user, 3 ) )
		{
			ShowCannonGump( user, cannon );
		}
		return;
	}
	if( timerID != 14 && ( !ValidateObject( user ) || !cannon.InRange( user, 3 ) || user.multi != cannon.multi ) )
	{
		cannon.SetTempTag( "hsCannonBusy", 0 );
		if( ValidateObject( user ) && user.socket )
		{
			user.socket.SysMessage( "The cannon operation was canceled." );
		}
		return;
	}
	if( timerID == 10 )
	{
		if( user.ResourceCount( 0x4246, 0 ) < 1 )
		{
			AddCannonAction( cannon, "You need a ramrod." );
			user.socket.SysMessage( "You need a ramrod." );
		}
		else
		{
			cannon.cannonStage = 1;
			AddCannonAction( cannon, "Preparation finished." );
			user.socket.SysMessage( "Preparation finished." );
			if( CountCannonResource( cannon, 0xA2BE ) >= 1 )
			{
				BeginCannonAction( cannon, user, 11, CannonActionTime( cannon ), "Charging started." );
				return;
			}
			user.socket.SysMessage( "Charging stopped. You need a powder charge." );
		}
	}
	else if( timerID == 11 )
	{
		if( CountCannonResource( cannon, 0xA2BE ) < 1 )
		{
			user.socket.SysMessage( "Charging stopped. The magazine needs a powder charge." );
		}
		else
		{
			ConsumeCannonResource( cannon, 1, 0xA2BE );
			cannon.cannonStage = 2;
			AddCannonAction( cannon, "Charging finished." );
			user.socket.SysMessage( "Charging finished." );
			let nextAmmo = SelectCannonAmmo( user, cannon.multi, cannon );
			if( CountCannonResource( cannon, nextAmmo ) >= 1 )
			{
				cannon.SetTempTag( "hsPendingAmmo", nextAmmo );
				BeginCannonAction( cannon, user, 12, CannonActionTime( cannon ), "Loading started." );
				return;
			}
			user.socket.SysMessage( "Loading stopped. You need a cannonball or grapeshot." );
		}
	}
	else if( timerID == 12 )
	{
		let ammo = parseInt( cannon.GetTempTag( "hsPendingAmmo" ) );
		let ammoItem = FindCannonResource( cannon, ammo );
		if( !ValidateObject( ammoItem ) )
		{
			user.socket.SysMessage( "Loading stopped. The magazine ammunition is missing." );
		}
		else
		{
			cannon.morex = Math.max( 1, parseInt( ammoItem.morex ) );
			cannon.morey = Math.max( 1, parseInt( ammoItem.morey ) );
			cannon.morez = Math.max( 0, parseInt( ammoItem.morez ) );
			cannon.lodamage = Math.max( 0, parseInt( ammoItem.lodamage ) );
			cannon.hidamage = Math.max( cannon.lodamage, parseInt( ammoItem.hidamage ) );
			ConsumeCannonResource( cannon, 1, ammo );
			cannon.cannonStage = 3;
			AddCannonAction( cannon, "Loading finished." );
			user.socket.SysMessage( "Loading finished." );
			if( CountCannonResource( cannon, 0x1420 ) >= 1 )
			{
				BeginCannonAction( cannon, user, 13, CannonActionTime( cannon ), "Priming started." );
				return;
			}
			user.socket.SysMessage( "Priming stopped. You need fuse cord." );
		}
	}
	else if( timerID == 13 )
	{
		if( CountCannonResource( cannon, 0x1420 ) < 1 )
		{
			user.socket.SysMessage( "Priming stopped. The magazine needs fuse cord." );
		}
		else
		{
			ConsumeCannonResource( cannon, 1, 0x1420 );
			cannon.cannonStage = 4;
			AddCannonAction( cannon, "Ready to fire." );
			user.socket.SysMessage( "Priming finished. The cannon is ready to fire." );
		}
	}
	else if( timerID == 14 )
	{
		FireCannon( cannon, user );
		return;
	}
	cannon.SetTempTag( "hsCannonBusy", 0 );
	if( ValidateObject( user ) && user.socket && cannon.InRange( user, 3 ) && IsViewingCannonGump( user, cannon ) )
	{
		ShowCannonGump( user, cannon );
	}
}

/** @type { ( cannon: Item, user: Character ) => any } */
function FireCannon( cannon, user )
{
	let target = ResolveStoredCannonTarget( cannon );
	if( !ValidateObject( target ) || !IsTargetInCannonArc( cannon, target ) || ( target.isChar && !IsValidCannonCharacterTarget( cannon, user, target ) ) )
	{
		target = ResolveAutomaticCannonTarget( cannon, user );
	}
	if( ValidateObject( user ) && user.region && user.region.isGuarded )
	{
		cannon.SetTempTag( "hsCannonBusy", 0 );
		if( user.socket )
		{
			user.socket.SysMessage( "Cannon fire is forbidden within guarded town limits." );
		}
		return;
	}
	let ammo = parseInt( cannon.morex );
	let minDamage = Math.max( 0, parseInt( cannon.lodamage ) );
	let maxDamage = Math.max( minDamage, parseInt( cannon.hidamage ) );
	let sourceBoat = cannon.multi;
	let cannonDamageMod
		= ValidateObject( sourceBoat ) && ( parseInt( sourceBoat.id ) - 0x4000 ) >= 0x18 && ( parseInt( sourceBoat.id ) - 0x4000 ) <= 0x1B ? 1.5 : 1.0;
	let range = CannonRange( cannon );
	let missX = cannon.x;
	let missY = cannon.y;
	switch( GetCannonFacing( cannon ) )
	{
	case 0:
		missY -= range;
		break;
	case 2:
		missX += range;
		break;
	case 4:
		missY += range;
		break;
	case 6:
		missX -= range;
		break;
	}
	DoMovingEffect( cannon.x, cannon.y, cannon.z + 2, ValidateObject( target ) ? target.x : missX, ValidateObject( target ) ? target.y : missY,
		ValidateObject( target ) ? target.z + 8 : cannon.z, 0x36E4, 8, 0, false );
	cannon.SoundEffect( 0x011C, true );
	let damage = 0;
	if( ValidateObject( target ) && target.isChar )
	{
		damage = Math.min( 35, Math.max( 1, RandomNumber( minDamage, maxDamage ) ) );
		target.Damage( damage, 1, user );
		DoStaticEffect( target.x, target.y, target.z + 8, 0x36CB, 15, 15, true );
	}
	else if( ValidateObject( target ) && ammo == 2 )
	{
		let lastHitDamage = 0;
		let lastHitX = target.x;
		let lastHitY = target.y;
		let pellets = Math.max( 1, parseInt( cannon.morey ) );
		let spread = Math.max( 0, parseInt( cannon.morez ) );
		for( let i = 0; i < pellets; ++i )
		{
			let hitDamage = Math.floor( RandomNumber( minDamage, maxDamage ) * cannonDamageMod );
			let hitX = target.x + RandomNumber( -spread, spread );
			let hitY = target.y + RandomNumber( -spread, spread );
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
	else if( ValidateObject( target ) )
	{
		damage = Math.floor( RandomNumber( minDamage, maxDamage ) * cannonDamageMod );
		let impactX = target.x;
		let impactY = target.y;
		let shotDX = target.x - cannon.x;
		let shotDY = target.y - cannon.y;
		if( Math.abs( shotDX ) >= Math.abs( shotDY ) )
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
	if( ValidateObject( target ) )
	{
		target.SoundEffect( 0x0207, true );
		target.Refresh();
	}
	if( ValidateObject( user ) )
	{
		let cannonHits = Math.max( 0, parseInt( cannon.health ) );
		cannon.health = Math.max( 0, cannonHits - RandomNumber( 0, 4 ) );
		cannon.Refresh();
	}
	// Reset every cannon preparation stage after each shot.
	cannon.cannonStage = 0;
	ClearLoadedAmmoProfile( cannon );
	cannon.SetTempTag( "hsCannonBusy", 0 );
	if( ValidateObject( user ) && user.socket )
	{
		let resultMessage = ValidateObject( target )
			? ( target.isChar ? "The cannon strikes its target for " + damage + " damage." : "The cannon strikes the enemy hull for " + damage + " damage." )
			: "The cannon fires, but the shot finds no target.";
		AddCannonAction( cannon, resultMessage );
		user.socket.SysMessage( resultMessage );
		if( cannon.InRange( user, 3 ) && IsViewingCannonGump( user, cannon ) )
		{
			ShowCannonGump( user, cannon );
		}
	}
}

/** @type { ( targetBoat: Multi, shooter: BaseObject, hitX: number, hitY: number ) => any } */
function DamageCrewAtImpact( targetBoat, shooter, hitX, hitY )
{
	targetBoat.SetTempTag( "hsImpactX", hitX );
	targetBoat.SetTempTag( "hsImpactY", hitY );
	targetBoat.SetTempTag( "hsImpactShooter", ValidateObject( shooter ) ? shooter.serial : 0 );
	AreaCharacterFunction( "HighSeasGrapeshotCharacter", targetBoat, 20 );
}

/** @type { ( targetBoat: Multi, targetChar: Character ) => any } */
function HighSeasGrapeshotCharacter( targetBoat, targetChar )
{
	if( !ValidateObject( targetChar ) || targetChar.dead || !targetChar.vulnerable || targetChar.multi != targetBoat )
	{
		return false;
	}
	if( targetChar.x != parseInt( targetBoat.GetTempTag( "hsImpactX" ) ) || targetChar.y != parseInt( targetBoat.GetTempTag( "hsImpactY" ) ) )
	{
		return false;
	}
	let shooter = CalcCharFromSer( parseInt( targetBoat.GetTempTag( "hsImpactShooter" ) ) );
	if( ValidateObject( shooter ) )
	{
		targetChar.Damage( 35, 1, shooter );
	}
	else
	{
		targetChar.Damage( 35, 1 );
	}
	return true;
}

/** @type { ( targetBoat: Multi, shooter: BaseObject, damage: number, hitX: number, hitY: number ) => any } */
function DamageCannonNearImpact( targetBoat, shooter, damage, hitX, hitY )
{
	targetBoat.SetTempTag( "hsImpactX", hitX );
	targetBoat.SetTempTag( "hsImpactY", hitY );
	targetBoat.SetTempTag( "hsImpactDamage", damage );
	targetBoat.SetTempTag( "hsImpactShooter", ValidateObject( shooter ) ? shooter.serial : 0 );
	AreaItemFunction( "HighSeasCannonImpact", targetBoat, 20 );
}

/** @type { ( targetBoat: Multi, targetItem: Item ) => any } */
function HighSeasCannonImpact( targetBoat, targetItem )
{
	if( !ValidateObject( targetItem ) || !targetItem.isShipCannon || targetItem.multi != targetBoat )
	{
		return false;
	}
	let hitX = parseInt( targetBoat.GetTempTag( "hsImpactX" ) );
	let hitY = parseInt( targetBoat.GetTempTag( "hsImpactY" ) );
	if( Math.abs( targetItem.x - hitX ) > 1 || Math.abs( targetItem.y - hitY ) > 1 )
	{
		return false;
	}
	let hits = parseInt( targetItem.health );
	if( isNaN( hits ) || hits <= 0 )
	{
		hits = 100;
	}
	hits -= parseInt( targetBoat.GetTempTag( "hsImpactDamage" ) );
	if( hits <= 0 )
	{
		DoStaticEffect( targetItem.x, targetItem.y, targetItem.z + 2, 0x36CB, 15, 15, true );
		targetItem.SoundEffect( 0x0207, true );
		targetItem.Delete();
		let shooter = CalcCharFromSer( parseInt( targetBoat.GetTempTag( "hsImpactShooter" ) ) );
		if( ValidateObject( shooter ) && shooter.socket )
		{
			shooter.socket.SysMessage( "The enemy ship cannon has been destroyed!" );
		}
	}
	else
	{
		targetItem.health = hits;
		targetItem.Refresh();
	}
	return true;
}

/** @type { ( cannon: Item, socket: Socket ) => any } */
function onTooltip( cannon, socket )
{
	if( !cannon.isShipCannon )
	{
		return "";
	}
	let hits = parseInt( cannon.health );
	if( isNaN( hits ) || hits <= 0 )
	{
		hits = 100;
	}
	let stage = parseInt( cannon.cannonStage );
	let ammo = parseInt( cannon.morex );
	cannon.SetTempTag( "clilocTooltip", 1042971 );
	let condition = hits >= 100
		? "Pristine"
		: ( hits >= 75 ? "Slightly Damaged" : ( hits >= 50 ? "Moderately Damaged" : ( hits >= 25 ? "Heavily Damaged" : "Severely Damaged" ) ) );
	return "Charged: " + ( stage >= 2 ? "Yes" : "No" ) + "\nAmmo: " + ( stage >= 3 ? AmmoName( ammo ) : "Empty" ) + "\nPrimed: " + ( stage >= 4 ? "Yes" : "No" )
		+ "\nCondition: " + condition;
}

/** @type { ( cannon: Item, boat: Multi ) => any } */
function SetCannonArt( cannon, boat )
{
	let facing = parseInt( boat.dir ) & 0x07;
	let power = parseInt( cannon.cannonPower );
	let artOffset = power == 2 ? 4 : 0;
	let south = parseInt( cannon.cannonArtSouth ) || ( power == 4 ? 41979 : ( power == 3 ? 41664 : 16918 + artOffset ) );
	let west = parseInt( cannon.cannonArtWest ) || ( power == 4 ? 41980 : ( power == 3 ? 41665 : 16919 + artOffset ) );
	let north = parseInt( cannon.cannonArtNorth ) || ( power == 4 ? 41981 : ( power == 3 ? 41666 : 16920 + artOffset ) );
	let east = parseInt( cannon.cannonArtEast ) || ( power == 4 ? 41982 : ( power == 3 ? 41667 : 16921 + artOffset ) );
	if( facing == 0 || facing == 4 )
	{
		if( cannon.x < boat.x )
		{
			cannon.id = west;
		}
		else if( cannon.x > boat.x )
		{
			cannon.id = east;
		}
		else
		{
			cannon.id = facing == 0 ? north : south;
		}
	}
	else
	{
		if( cannon.y < boat.y )
		{
			cannon.id = north;
		}
		else if( cannon.y > boat.y )
		{
			cannon.id = south;
		}
		else
		{
			cannon.id = facing == 2 ? east : west;
		}
	}
	cannon.dir = facing;
}

// Scripted naval encounters use the same cannon objects, arcs, effects and hull
// damage as player-operated weapons. These entry points only replace the manual
// deed/loading clicks performed by a player crew.
/** @type { ( boat: Multi, pad: Item, cannonPower: number ) => any } */
function DeployNpcCannon( boat, pad, cannonPower )
{
	if( !ValidateObject( boat ) || !ValidateObject( pad ) || pad.multi != boat || !pad.isWeaponPad )
	{
		return null;
	}
	let existing = CalcItemFromSer( parseInt( pad.cannonLinkSerial ) );
	if( ValidateObject( existing ) && existing.isShipCannon )
	{
		return existing;
	}
	let normalizedPower = cannonPower == 4 ? 4 : ( cannonPower == 3 ? 3 : ( cannonPower == 2 ? 2 : 1 ) );
	let cannon = CreateDFNItem( null, null,
		normalizedPower == 4 ? "pumpkin_cannon"
							 : ( normalizedPower == 3 ? "blundercannon" : ( normalizedPower == 2 ? "highseas_heavy_cannon" : "highseas_light_cannon" ) ),
		1, "ITEM", false, 0, boat.worldnumber, boat.instanceID );
	if( !ValidateObject( cannon ) )
	{
		return null;
	}
	cannon.SetLocation( pad.x, pad.y, pad.z + GetTileHeight( pad.id ), boat.worldnumber, boat.instanceID );
	cannon.multi = boat;
	cannon.cannonRole = 2;
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

/** @type { ( cannon: Item, targetBoat: Multi, captain: Character ) => any } */
function FireNpcCannon( cannon, targetBoat, captain )
{
	if( !ValidateObject( cannon ) || !ValidateObject( targetBoat ) || !ValidateObject( captain ) || !cannon.isShipCannon || cannon.multi == targetBoat
		|| parseInt( cannon.GetTempTag( "hsCannonBusy" ) ) == 1 || !IsTargetInCannonArc( cannon, targetBoat ) )
	{
		return false;
	}
	cannon.cannonStage = 4;
	cannon.morex = 1;
	cannon.morey = 1;
	cannon.morez = 0;
	cannon.lodamage = 5000;
	cannon.hidamage = 5000;
	cannon.SetTempTag( "hsCannonBusy", 1 );
	cannon.SetTempTag( "hsCannonOperator", captain.serial );
	cannon.SetTempTag( "hsCannonTarget", targetBoat.serial );
	cannon.SoundEffect( 0x0666, true );
	cannon.StartTimer( 1500, 14, 5099 );
	return true;
}

/** @type { ( cannon: Item, candidate: Character ) => boolean } */
function FindCannonTrajectoryCharacterTarget( cannon, candidate )
{
	let shooter = CalcCharFromSer( parseInt( cannon.GetTempTag( "hsTrajectoryOperator" ) ) );
	if( !IsValidCannonCharacterTarget( cannon, shooter, candidate ) )
	{
		return false;
	}
	let dx = candidate.x - cannon.x;
	let dy = candidate.y - cannon.y;
	let vx = 0;
	let vy = 0;
	switch( GetCannonFacing( cannon ) )
	{
	case 0:
		vy = -1;
		break;
	case 2:
		vx = 1;
		break;
	case 4:
		vy = 1;
		break;
	case 6:
		vx = -1;
		break;
	}
	let forward = dx * vx + dy * vy;
	if( forward < parseInt( cannon.GetTempTag( "hsTrajectoryBest" ) ) )
	{
		cannon.SetTempTag( "hsTrajectoryBest", forward );
		cannon.SetTempTag( "hsCannonTarget", candidate.serial );
	}
	return true;
}
