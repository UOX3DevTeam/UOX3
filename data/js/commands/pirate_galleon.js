// ServUO-style High Seas pirate encounter prototype.
// Admin test command: 'pirategalleon, then target open water.

const PIRATE_SCRIPT = 5100;
const CANNON_SCRIPT = 5099;
const AI_TIMER = 1;
const CLEANUP_TIMER = 2;
const AI_INTERVAL = 1000;
const FIRE_COOLDOWN = 7000;
const CLEANUP_DELAY = 1800000;
const ENGAGE_RANGE = 28;
// Center-to-center station range. Individual cannon objects still enforce
// their exact 10-tile muzzle range and firing arc.
const BROADSIDE_RANGE = 14;
const BROADSIDE_OFFSET = 11;
const COURSE_TOLERANCE = 3;

function CommandRegistration()
{
	RegisterCommand( "pirategalleon", 2, true );
}

function command_PIRATEGALLEON( socket, cmdString )
{
	socket.CustomTarget( 0, "Target open water for the pirate galleon." );
}

function onCallback0( socket, target )
{
	if( parseInt( socket.GetByte( 11 )) == 255 ) return;
	var user = socket.currentChar;
	var x = socket.GetWord( 11 );
	var y = socket.GetWord( 13 );
	var z = socket.GetSByte( 16 );
	var boat = CreateHouse( 210, x, y, z, user.worldnumber, user.instanceID, 0, true );
	if( !ValidateObject( boat ) || !boat.IsBoat() )
	{
		socket.SysMessage( "The pirate galleon cannot be placed there." );
		return;
	}

	boat.SetTag( "hsPirateGalleon", 1 );
	boat.SetTag( "hsPirateDefeated", 0 );
	boat.AddScriptTrigger( PIRATE_SCRIPT );
	// High Seas galleon fixture bases are below the walkable deck. The Orcish
	// galleon's deck surface is 28 Z above its waterline (the same Z occupied by
	// players standing aboard); the wheel object's base Z is not a character Z.
	var deckZ = boat.z + 28;
	var captain = SpawnPirate( "highseas_pirate_captain", boat, boat.x, boat.y + 6, deckZ );
	if( !ValidateObject( captain ))
	{
		boat.Delete();
		socket.SysMessage( "The pirate captain could not be created." );
		return;
	}
	boat.owner = captain;
	boat.SetTag( "hsPirateCaptain", captain.serial );
	captain.SetTag( "hsPirateCaptain", 1 );

	SpawnPirate( "highseas_pirate_crew", boat, boat.x - 2, boat.y + 2, deckZ );
	SpawnPirate( "highseas_pirate_crew", boat, boat.x + 2, boat.y + 1, deckZ );
	SpawnPirate( "highseas_pirate_crew", boat, boat.x - 2, boat.y - 3, deckZ );
	SpawnPirate( "highseas_pirate_crew", boat, boat.x + 2, boat.y - 4, deckZ );

	boat.SetTempTag( "hsDeployPower", 2 );
	AreaItemFunction( "DeployPirateCannonOnPad", boat, 25 );
	StockPirateHold( boat );
	boat.StartTimer( AI_INTERVAL, AI_TIMER, PIRATE_SCRIPT );
	boat.Refresh();
	socket.SysMessage( "A hostile pirate galleon has entered these waters." );
}

function SpawnPirate( section, boat, x, y, z )
{
	var pirate = SpawnNPC( section, x, y, z, boat.worldnumber, boat.instanceID, false );
	if( ValidateObject( pirate ))
	{
		pirate.multi = boat;
		pirate.SetTag( "hsPirateBoat", boat.serial );
	}
	return pirate;
}

function DeployPirateCannonOnPad( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || item.GetTag( "hsWeaponPad" ) != 1 ) return false;
	TriggerEvent( CANNON_SCRIPT, "DeployNpcCannon", boat, item, parseInt( boat.GetTempTag( "hsDeployPower" )));
	return true;
}

function StockPirateHold( boat )
{
	var hold = boat.GetHold();
	if( !ValidateObject( hold )) return;
	AddHoldLoot( hold, "0x0eed", RandomNumber( 8000, 14000 ));
	AddHoldLoot( hold, "highseas_cannonball", RandomNumber( 20, 35 ));
	AddHoldLoot( hold, "highseas_powder_charge", RandomNumber( 15, 25 ));
	AddHoldLoot( hold, "highseas_fuse_cord", RandomNumber( 15, 25 ));
	AddHoldLoot( hold, "highseas_grapeshot", RandomNumber( 4, 8 ));
	AddHoldLoot( hold, "0x1bd7", RandomNumber( 50, 100 ));
}

function AddHoldLoot( hold, section, amount )
{
	var loot = CreateDFNItem( null, null, section, amount, "ITEM", false, 0, hold.worldnumber, hold.instanceID );
	if( ValidateObject( loot )) loot.container = hold;
}

function onTimer( boat, timerID )
{
	if( !ValidateObject( boat ) || !boat.isItem || boat.GetTag( "hsPirateGalleon" ) != 1 ) return;
	if( timerID == CLEANUP_TIMER )
	{
		RemovePirateCrew( boat );
		boat.Delete();
		return;
	}
	if( timerID != AI_TIMER ) return;

	var captain = CalcCharFromSer( parseInt( boat.GetTag( "hsPirateCaptain" )));
	if( !ValidateObject( captain ) || captain.dead || ( boat.GetHullMaxHits() > 0 && boat.GetHullHits() * 4 < boat.GetHullMaxHits() ))
	{
		DefeatPirateGalleon( boat );
		return;
	}
	// Repair pirate encounters created by older versions which used the wheel
	// fixture's base Z and consequently left their crew beneath the deck.
	boat.SetTempTag( "hsPirateDeckZ", boat.z + 28 );
	AreaCharacterFunction( "CorrectPirateCrewDeckZ", boat, 25 );

	boat.SetTempTag( "hsBestTarget", 0 );
	boat.SetTempTag( "hsBestTargetChar", 0 );
	boat.SetTempTag( "hsBestDistance", ENGAGE_RANGE + 1 );
	AreaCharacterFunction( "FindPirateShipTarget", boat, ENGAGE_RANGE );
	var targetBoat = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBestTarget" )));
	if( ValidateObject( targetBoat ) && targetBoat.IsBoat() )
		NavigatePirateGalleon( boat, targetBoat, captain );
	boat.StartTimer( AI_INTERVAL, AI_TIMER, PIRATE_SCRIPT );
}

function CorrectPirateCrewDeckZ( boat, character )
{
	if( !ValidateObject( character ) || parseInt( character.GetTag( "hsPirateBoat" )) != boat.serial ) return false;
	var deckZ = parseInt( boat.GetTempTag( "hsPirateDeckZ" ));
	if( character.z < deckZ - 2 || character.z > deckZ + 2 )
		character.SetLocation( character.x, character.y, deckZ, boat.worldnumber, boat.instanceID );
	character.multi = boat;
	return true;
}

function FindPirateShipTarget( pirateBoat, candidate )
{
	if( !ValidateObject( candidate ) || candidate.npc || candidate.dead || !candidate.online ) return false;
	var targetBoat = candidate.multi;
	if( !ValidateObject( targetBoat ) || !targetBoat.IsBoat() || targetBoat == pirateBoat || targetBoat.GetTag( "hsPirateGalleon" ) == 1 ) return false;
	var distance = Math.max( Math.abs( targetBoat.x - pirateBoat.x ), Math.abs( targetBoat.y - pirateBoat.y ));
	if( distance < parseInt( pirateBoat.GetTempTag( "hsBestDistance" )))
	{
		pirateBoat.SetTempTag( "hsBestDistance", distance );
		pirateBoat.SetTempTag( "hsBestTarget", targetBoat.serial );
		pirateBoat.SetTempTag( "hsBestTargetChar", candidate.serial );
	}
	return true;
}

function NavigatePirateGalleon( boat, target, captain )
{
	var dx = target.x - boat.x;
	var dy = target.y - boat.y;
	var distance = Math.max( Math.abs( dx ), Math.abs( dy ));
	var targetDir = parseInt( target.dir ) & 0x06;
	var waypointX = target.x;
	var waypointY = target.y;
	// Approach a point off the target's port or starboard side instead of its
	// center. This prevents bow-first collisions and gives the pirate a real
	// parallel broadside course matching the target vessel's heading.
	if( targetDir == 0 || targetDir == 4 )
		waypointX += boat.x < target.x ? -BROADSIDE_OFFSET : BROADSIDE_OFFSET;
	else
		waypointY += boat.y < target.y ? -BROADSIDE_OFFSET : BROADSIDE_OFFSET;
	var approachX = waypointX - boat.x;
	var approachY = waypointY - boat.y;
	var approachDistance = Math.max( Math.abs( approachX ), Math.abs( approachY ));
	var desired = approachDistance > COURSE_TOLERANCE ?
		( Math.abs( approachX ) > Math.abs( approachY ) ? ( approachX > 0 ? 2 : 6 ) : ( approachY > 0 ? 4 : 0 )) : targetDir;

	var current = parseInt( boat.dir ) & 0x06;
	if( current != desired )
	{
		var delta = ( desired - current + 8 ) % 8;
		// Never use TurnBoat(3) here. A flip performs two complete directional
		// fixture rebuilds in one tick, which can leave the intermediate fixtures
		// visible to nearby clients. Turn one quarter at a time, as a helmsman does.
		// The native bridge's historical command numbering is opposite its labels:
		// command 2 advances the cardinal heading (+2), command 1 reduces it (-2).
		boat.TurnBoat( delta == 2 ? 2 : 1 );
	}
	// Once a valid broadside is established, hold station. Resume sailing only
	// when the target pulls beyond cannon range and its side waypoint must be
	// reacquired; otherwise a stationary target causes repeated ramming attempts.
	else if( distance > BROADSIDE_RANGE && approachDistance > COURSE_TOLERANCE )
		boat.SailBoat( current );

	if( distance <= BROADSIDE_RANGE )
	{
		var boardingTarget = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBestTargetChar" )));
		if( ValidateObject( boardingTarget ))
		{
			boat.SetTempTag( "hsBoardingTarget", boardingTarget.serial );
			boat.SetTempTag( "hsBoardingCount", 0 );
			AreaCharacterFunction( "BoardPirateCrew", boat, 25 );
		}
		var now = GetCurrentClock();
		var nextFire = parseInt( boat.GetTempTag( "hsNextBroadside" ));
		if( isNaN( nextFire ) || now >= nextFire )
		{
			boat.SetTempTag( "hsBroadsideTarget", target.serial );
			boat.SetTempTag( "hsBroadsideCaptain", captain.serial );
			AreaItemFunction( "FirePirateBroadside", boat, 25 );
			boat.SetTempTag( "hsNextBroadside", now + FIRE_COOLDOWN );
		}
	}
}

function BoardPirateCrew( pirateBoat, pirate )
{
	if( !ValidateObject( pirate ) || pirate.dead || pirate.GetTag( "hsPirateCaptain" ) == 1 ||
		parseInt( pirate.GetTag( "hsPirateBoat" )) != pirateBoat.serial || pirate.GetTag( "hsPirateBoarded" ) == 1 ) return false;
	var count = parseInt( pirateBoat.GetTempTag( "hsBoardingCount" ));
	if( count >= 2 ) return false;
	var target = CalcCharFromSer( parseInt( pirateBoat.GetTempTag( "hsBoardingTarget" )));
	if( !ValidateObject( target ) || target.dead || !ValidateObject( target.multi ) || !target.multi.IsBoat() ) return false;
	var side = count == 0 ? -1 : 1;
	pirate.SetLocation( target.x + side, target.y, target.z, target.worldnumber, target.instanceID );
	pirate.multi = target.multi;
	pirate.SetTag( "hsPirateBoarded", 1 );
	pirate.target = target;
	pirate.attacker = target;
	pirate.atWar = true;
	pirateBoat.SetTempTag( "hsBoardingCount", count + 1 );
	target.SysMessage( "Pirates swing across the lines and board your vessel!" );
	return true;
}

function FirePirateBroadside( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || item.GetTag( "hsCannonKind" ) != 2 ) return false;
	var target = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBroadsideTarget" )));
	var captain = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBroadsideCaptain" )));
	if( ValidateObject( target ) && ValidateObject( captain ))
		TriggerEvent( CANNON_SCRIPT, "FireNpcCannon", item, target, captain );
	return true;
}

function onDeath( deadChar, corpse )
{
	if( !ValidateObject( deadChar ) || deadChar.GetTag( "hsPirateCaptain" ) != 1 ) return;
	var boat = CalcItemFromSer( parseInt( deadChar.GetTag( "hsPirateBoat" )));
	if( ValidateObject( boat )) DefeatPirateGalleon( boat );
}

function DefeatPirateGalleon( boat )
{
	if( boat.GetTag( "hsPirateDefeated" ) == 1 ) return;
	boat.SetTag( "hsPirateDefeated", 1 );
	boat.owner = null;
	var hold = boat.GetHold();
	if( ValidateObject( hold )) hold.owner = null;
	var tiller = boat.GetTiller();
	if( ValidateObject( tiller )) tiller.TextMessage( null, "The pirate vessel is defeated! Its hold may now be plundered." );
	boat.StartTimer( CLEANUP_DELAY, CLEANUP_TIMER, PIRATE_SCRIPT );
}

function RemovePirateCrew( boat )
{
	AreaCharacterFunction( "DeletePirateCrewMember", boat, 25 );
}

function DeletePirateCrewMember( boat, character )
{
	if( ValidateObject( character ) && parseInt( character.GetTag( "hsPirateBoat" )) == boat.serial ) character.Delete();
	return true;
}
