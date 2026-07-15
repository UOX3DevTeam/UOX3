// ServUO-style High Seas pirate encounter prototype.
// Admin test command: 'pirategalleon, then target open water.

const PIRATE_SCRIPT = 5100;
const CANNON_SCRIPT = 5099;
const AI_TIMER = 1;
const CLEANUP_TIMER = 2;
const AI_INTERVAL = 1000;
// Four crew produce ServUO's 10 second ShootFrequency (20 - 4 * 2.5), with
// each cannon adding its own random zero-to-three-second delay.
const FIRE_COOLDOWN = 10000;
const CLEANUP_DELAY = 1800000;
const ENGAGE_RANGE = 25;
const MIN_PURSUIT_RANGE = 10;
const MAX_PURSUIT_RANGE = 35;

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
		if( AreaCharacterFunction( "CountPlayersAboardPirate", boat, 25 ) > 0 )
		{
			boat.decaytime = 30;
			boat.StartTimer( 30000, CLEANUP_TIMER, PIRATE_SCRIPT );
			return;
		}
		RemovePirateCrew( boat );
		boat.Delete();
		return;
	}
	if( timerID != AI_TIMER ) return;

	var captain = CalcCharFromSer( parseInt( boat.GetTag( "hsPirateCaptain" )));
	RunPirateCaptainAI( boat, captain );
	if( ValidateObject( boat ) && boat.GetTag( "hsPirateDefeated" ) != 1 )
		boat.StartTimer( AI_INTERVAL, AI_TIMER, PIRATE_SCRIPT );
}

// NPC AI slivers are restored with the captain from the world save. This is
// the restart-safe equivalent of BaseShipCaptain.Deserialize scheduling its
// course and crew checks; the boat timer remains only as a live-world fallback.
function onAISliver( pirate )
{
	if( !ValidateObject( pirate )) return false;
	var linkedBoat = CalcItemFromSer( parseInt( pirate.GetTag( "hsPirateBoat" )));
	if( ValidateObject( linkedBoat ) && linkedBoat.GetTag( "hsPirateDefeated" ) == 1 &&
		AreaCharacterFunction( "CountPlayersAboardPirate", linkedBoat, 25 ) > 0 )
		linkedBoat.decaytime = 30;
	if( pirate.GetTag( "hsPirateCaptain" ) != 1 ) return false;
	var now = GetCurrentClock();
	var nextThink = parseInt( pirate.GetTempTag( "hsNextShipThink" ));
	if( !isNaN( nextThink ) && now < nextThink ) return false;
	pirate.SetTempTag( "hsNextShipThink", now + AI_INTERVAL );
	var boat = CalcItemFromSer( parseInt( pirate.GetTag( "hsPirateBoat" )));
	if( ValidateObject( boat )) RunPirateCaptainAI( boat, pirate );
	return false;
}

function CountPlayersAboardPirate( boat, character )
{
	return ValidateObject( character ) && !character.npc && character.multi == boat;
}

function RunPirateCaptainAI( boat, captain )
{
	var now = GetCurrentClock();
	var nextBoatThink = parseInt( boat.GetTempTag( "hsNextCaptainThink" ));
	if( !isNaN( nextBoatThink ) && now < nextBoatThink ) return;
	boat.SetTempTag( "hsNextCaptainThink", now + AI_INTERVAL );
	if( !ValidateObject( captain ) || captain.dead || ( boat.GetHullMaxHits() > 0 && boat.GetHullHits() * 4 < boat.GetHullMaxHits() ))
	{
		DefeatPirateGalleon( boat );
		return;
	}
	// BaseShipCaptain.CheckCrew returns strays to the galleon's current surface.
	// The captain is already carried at that surface, avoiding a hull-specific
	// hard-coded Z when restoring crew after movement or a world reload.
	boat.SetTempTag( "hsPirateDeckZ", captain.z );
	var nextCrewCheck = parseInt( captain.GetTempTag( "hsNextCrewCheck" ));
	if( isNaN( nextCrewCheck ) || now >= nextCrewCheck )
	{
		AreaCharacterFunction( "CorrectPirateCrewDeckZ", boat, 25 );
		captain.SetTempTag( "hsNextCrewCheck", now + 1800000 );
	}

	boat.SetTempTag( "hsBestTarget", 0 );
	boat.SetTempTag( "hsBestTargetChar", 0 );
	boat.SetTempTag( "hsBestDistance", ENGAGE_RANGE + 1 );
	AreaCharacterFunction( "FindPirateShipTarget", boat, ENGAGE_RANGE );
	var targetBoat = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBestTarget" )));
	if( ValidateObject( targetBoat ) && targetBoat.IsBoat() )
		NavigatePirateGalleon( boat, targetBoat, captain );
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
	// BaseShipCaptain pursues the target vessel only while it is between ten
	// and thirty-five tiles away. It does not use the prototype side waypoint or
	// station-keeping behavior that was previously added here.
	if( distance >= MIN_PURSUIT_RANGE && distance <= MAX_PURSUIT_RANGE )
	{
		var desired = Math.abs( dx ) > Math.abs( dy ) ? ( dx > 0 ? 2 : 6 ) : ( dy > 0 ? 4 : 0 );
		var current = parseInt( boat.dir ) & 0x06;
		if( current != desired )
		{
			var delta = ( desired - current + 8 ) % 8;
			boat.TurnBoat( delta == 2 ? 2 : 1 );
		}
		else
			boat.SailBoat( current );
	}

	if( distance <= ENGAGE_RANGE )
	{
		boat.SetTempTag( "hsBroadsideTarget", target.serial );
		boat.SetTempTag( "hsBroadsideCaptain", captain.serial );
		AreaItemFunction( "FirePirateBroadside", boat, 25 );
	}
}

function FirePirateBroadside( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || item.GetTag( "hsCannonKind" ) != 2 ) return false;
	var target = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBroadsideTarget" )));
	var captain = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBroadsideCaptain" )));
	var now = GetCurrentClock();
	var nextFire = parseInt( item.GetTempTag( "hsNextFire" ));
	if( ValidateObject( target ) && ValidateObject( captain ) && ( isNaN( nextFire ) || now >= nextFire ) &&
		TriggerEvent( CANNON_SCRIPT, "FireNpcCannon", item, target, captain ))
		item.SetTempTag( "hsNextFire", now + FIRE_COOLDOWN + RandomNumber( 0, 3000 ));
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
	// ServUO gives a defeated captain's galleon a thirty-minute decay window.
	// Native decay state is serialized, so cleanup survives a shard restart.
	boat.decayable = true;
	boat.decaytime = 1800;
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
