// ServUO-style High Seas pirate encounter prototype.
// Admin test command: 'pirategalleon, then target open water.

const PIRATE_SCRIPT = 5100;
const CANNON_SCRIPT = 5099;
const AI_TIMER = 1;
const CLEANUP_TIMER = 2;
// A player-controlled High Seas ship can move every 250ms at full speed. Keep
// pirate pursuit slower so it can pressure a target without matching every
// tile forever and making escape impossible once the hulls meet.
const AI_INTERVAL = Math.max( 1, parseInt( GetServerSetting( "BOATNPCMOVEINTERVAL" )));
// Four crew produce ServUO's 10 second ShootFrequency (20 - 4 * 2.5), with
// each cannon adding its own random zero-to-three-second delay.
const FIRE_COOLDOWN = 10000;
const CLEANUP_DELAY = 1800000;
const ENGAGE_RANGE = 25;
const MIN_PURSUIT_RANGE = 10;
const MAX_PURSUIT_RANGE = 35;
// These UOX High Seas hulls can span roughly twenty tiles end-to-end when two
// vessels approach bow-first. Stop before their collision footprints touch and
// use a separate resume range to prevent one-tile chase/stop oscillation.
const UOX_STOP_RANGE = 20;
const UOX_RESUME_RANGE = 24;
const LOITER_DELAY = 2000;
const FIRE_CHECK_INTERVAL = 750;

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

	boat.AddScriptTrigger( PIRATE_SCRIPT );
	// ServUO OrcishGalleon.ZSurface is 14 above the hull origin.  Using the
	// fixture height (28) puts mobiles a full deck level too high and visually
	// projects them into the masts and sails even when their X/Y is correct.
	var deckZ = boat.z + 14;
	// ServUO spawns the captain one tile north of the hull origin and keeps
	// crew within a one-tile radius of center.  The Orcish deck narrows sharply
	// toward both ends, so wider classic-boat offsets can place mobiles outside
	// the walkable multi even though their Z is correct.
	var captain = SpawnPirate( "highseas_pirate_captain", boat, boat.x, boat.y - 1, deckZ );
	if( !ValidateObject( captain ))
	{
		boat.Delete();
		socket.SysMessage( "The pirate captain could not be created." );
		return;
	}
	boat.owner = captain;

	SpawnPirate( "highseas_pirate_crew", boat, boat.x - 1, boat.y - 1, deckZ );
	SpawnPirate( "highseas_pirate_crew", boat, boat.x + 1, boat.y - 1, deckZ );
	SpawnPirate( "highseas_pirate_crew", boat, boat.x - 1, boat.y + 1, deckZ );
	SpawnPirate( "highseas_pirate_crew", boat, boat.x + 1, boat.y + 1, deckZ );

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
		pirate.multi = boat;
	return pirate;
}

function DeployPirateCannonOnPad( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || !item.isWeaponPad ) return false;
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
	if( !ValidateObject( boat ) || !boat.isItem || !boat.HasScriptTrigger( PIRATE_SCRIPT )) return;
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

	var captain = boat.owner;
	RunPirateCaptainAI( boat, captain );
	if( ValidateObject( boat ) && ValidateObject( boat.owner ))
		boat.StartTimer( AI_INTERVAL, AI_TIMER, PIRATE_SCRIPT );
}

// NPC AI slivers are restored with the captain from the world save. This is
// the restart-safe equivalent of BaseShipCaptain.Deserialize scheduling its
// course and crew checks; the boat timer remains only as a live-world fallback.
function onAISliver( pirate )
{
	if( !ValidateObject( pirate )) return false;
	var linkedBoat = pirate.multi;
	if( ValidateObject( linkedBoat ) && !ValidateObject( linkedBoat.owner ) &&
		AreaCharacterFunction( "CountPlayersAboardPirate", linkedBoat, 25 ) > 0 )
		linkedBoat.decaytime = 30;
	if( !ValidateObject( linkedBoat ) || linkedBoat.owner != pirate ) return false;
	var now = GetCurrentClock();
	var nextThink = parseInt( pirate.GetTempTag( "hsNextShipThink" ));
	if( !isNaN( nextThink ) && now < nextThink ) return false;
	pirate.SetTempTag( "hsNextShipThink", now + AI_INTERVAL );
	var boat = pirate.multi;
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

	// Retain a live target while it remains in ServUO's pursuit envelope. This
	// avoids rebuilding the target every 250ms and lets the captain finish a
	// broadside approach when the player's mobile is briefly outside the scan.
	var targetBoat = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBestTarget" )));
	var targetChar = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBestTargetChar" )));
	if( !IsValidPirateTarget( boat, targetBoat, targetChar, MAX_PURSUIT_RANGE ))
	{
		boat.SetTempTag( "hsBestTarget", 0 );
		boat.SetTempTag( "hsBestTargetChar", 0 );
		boat.SetTempTag( "hsBestDistance", MAX_PURSUIT_RANGE + 1 );
		AreaCharacterFunction( "FindPirateShipTarget", boat, MAX_PURSUIT_RANGE );
		targetBoat = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBestTarget" )));
		targetChar = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBestTargetChar" )));
	}
	if( ValidateObject( targetBoat ) && targetBoat.IsBoat() )
		NavigatePirateGalleon( boat, targetBoat, captain );
}

function IsValidPirateTarget( pirateBoat, targetBoat, targetChar, maxRange )
{
	if( !ValidateObject( targetBoat ) || !targetBoat.IsBoat() || targetBoat == pirateBoat ||
		targetBoat.HasScriptTrigger( PIRATE_SCRIPT ) || !ValidateObject( targetChar ) ||
		targetChar.npc || targetChar.dead || !targetChar.online || targetChar.multi != targetBoat ) return false;
	return Math.max( Math.abs( targetBoat.x - pirateBoat.x ), Math.abs( targetBoat.y - pirateBoat.y )) <= maxRange;
}

function CorrectPirateCrewDeckZ( boat, character )
{
	if( !ValidateObject( character ) || character.multi != boat ) return false;
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
	if( !ValidateObject( targetBoat ) || !targetBoat.IsBoat() || targetBoat == pirateBoat || targetBoat.HasScriptTrigger( PIRATE_SCRIPT )) return false;
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
	var now = GetCurrentClock();
	var navTarget = parseInt( boat.GetTempTag( "hsNavTarget" ));
	var pursuing = parseInt( boat.GetTempTag( "hsPursuing" )) == 1;
	var resumeAt = parseInt( boat.GetTempTag( "hsResumePursuitAt" ));
	if( isNaN( resumeAt )) resumeAt = 0;

	// SailBoat does not report a blocked native move back to JavaScript. Detect
	// an unchanged position on the following AI tick and loiter before retrying,
	// instead of hammering the obstruction and spamming the tiller message.
	var attemptPending = parseInt( boat.GetTempTag( "hsMoveAttemptPending" )) == 1;
	var attemptX = parseInt( boat.GetTempTag( "hsMoveAttemptX" ));
	var attemptY = parseInt( boat.GetTempTag( "hsMoveAttemptY" ));
	if( attemptPending )
	{
		boat.SetTempTag( "hsMoveAttemptPending", 0 );
		if( !isNaN( attemptX ) && !isNaN( attemptY ) && attemptX == boat.x && attemptY == boat.y )
		{
			pursuing = false;
			boat.SetTempTag( "hsPursuing", 0 );
			resumeAt = now + LOITER_DELAY;
			boat.SetTempTag( "hsResumePursuitAt", resumeAt );
		}
	}
	if( navTarget != target.serial )
	{
		boat.SetTempTag( "hsNavTarget", target.serial );
		pursuing = distance > UOX_STOP_RANGE;
		boat.SetTempTag( "hsPursuing", pursuing ? 1 : 0 );
		resumeAt = 0;
	}

	if( pursuing && distance <= UOX_STOP_RANGE )
	{
		// ServUO StopMove + ResumeCourseTimed(loiter), adapted to scripted
		// one-tile movement. Do not chase the target's changing relative offset.
		pursuing = false;
		boat.SetTempTag( "hsPursuing", 0 );
		resumeAt = now + LOITER_DELAY;
		boat.SetTempTag( "hsResumePursuitAt", resumeAt );
	}
	else if( !pursuing && distance >= UOX_RESUME_RANGE && now >= resumeAt )
	{
		pursuing = true;
		boat.SetTempTag( "hsPursuing", 1 );
	}

	if( pursuing && distance <= MAX_PURSUIT_RANGE )
	{
		// BaseBoat.StartMove receives a world direction in ServUO. SailBoat is
		// UOX's collision-checked one-tile equivalent and does not require the bow
		// to point at the target.
		boat.SetTempTag( "hsMoveAttemptX", boat.x );
		boat.SetTempTag( "hsMoveAttemptY", boat.y );
		boat.SetTempTag( "hsMoveAttemptPending", 1 );
		boat.SailBoat( DirectionToPoint( boat.x, boat.y, target.x, target.y ));
	}
	else if( distance <= UOX_RESUME_RANGE )
	{
		// While loitering, rotate only as needed to put a beam toward the target.
		// The hull remains stationary, so this does not tether the two vessels.
		AlignPirateForTarget( boat, dx, dy );
	}

	var nextFireCheck = parseInt( boat.GetTempTag( "hsNextBroadsideCheck" ));
	if( distance <= ENGAGE_RANGE && ( isNaN( nextFireCheck ) || now >= nextFireCheck ))
	{
		boat.SetTempTag( "hsNextBroadsideCheck", now + FIRE_CHECK_INTERVAL );
		boat.SetTempTag( "hsBroadsideTarget", target.serial );
		boat.SetTempTag( "hsBroadsideCaptain", captain.serial );
		AreaItemFunction( "FirePirateBroadside", boat, 25 );
	}
}

function DirectionToPoint( fromX, fromY, toX, toY )
{
	var dx = toX - fromX;
	var dy = toY - fromY;
	var adx = Math.abs( dx );
	var ady = Math.abs( dy );
	if( dx == 0 ) return dy > 0 ? 4 : 0;
	if( dy == 0 ) return dx > 0 ? 2 : 6;
	if( adx > ady * 2 ) return dx > 0 ? 2 : 6;
	if( ady > adx * 2 ) return dy > 0 ? 4 : 0;
	if( dx > 0 ) return dy > 0 ? 3 : 1;
	return dy > 0 ? 5 : 7;
}

function AlignPirateForTarget( boat, dx, dy )
{
	var current = parseInt( boat.dir ) & 0x06;
	// If the target is east/west, a north/south hull exposes its beam; if the
	// target is north/south, use an east/west hull. Pick the nearer parallel
	// orientation to avoid unnecessary 180-degree turns.
	var first = Math.abs( dx ) >= Math.abs( dy ) ? 0 : 2;
	var second = ( first + 4 ) & 0x07;
	var firstTurns = Math.min(( first - current + 8 ) % 8, ( current - first + 8 ) % 8 );
	var secondTurns = Math.min(( second - current + 8 ) % 8, ( current - second + 8 ) % 8 );
	var desired = firstTurns <= secondTurns ? first : second;
	if( current == desired ) return;
	var delta = ( desired - current + 8 ) % 8;
	boat.TurnBoat( delta == 2 || delta == 4 ? 2 : 1 );
}

function FirePirateBroadside( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || !item.isShipCannon ) return false;
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
	if( !ValidateObject( deadChar )) return;
	var boat = deadChar.multi;
	if( !ValidateObject( boat ) || boat.owner != deadChar || !boat.HasScriptTrigger( PIRATE_SCRIPT )) return;
	if( ValidateObject( boat )) DefeatPirateGalleon( boat );
}

function DefeatPirateGalleon( boat )
{
	if( !ValidateObject( boat.owner )) return;
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
	if( ValidateObject( character ) && character.multi == boat ) character.Delete();
	return true;
}
