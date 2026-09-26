/// <reference path="../../definitions.d.ts" />
// @ts-check

// High Seas pirate encounter AI.

// A player-controlled High Seas ship can move every 250ms at full speed. Keep
// pirate pursuit slower so it can pressure a target without matching every
// tile forever and making escape impossible once the hulls meet.
const AI_INTERVAL = Math.max( 1, parseInt( GetServerSetting( "BOATNPCMOVEINTERVAL" ) ) );

/** @type { ( socket: Socket, x: number, y: number, z: number ) => any } */
function AddPirateShip( socket, x, y, z )
{
	let user = socket.currentChar;
	CreatePirateShipEncounter( socket, x, y, z, user.worldnumber, user.instanceID, null );
}

/** @type { ( pirate: Character, spawnRegion: number ) => any } */
function onSpawn( pirate, spawnRegion )
{
	if( !ValidateObject( pirate ) || pirate.sectionID != "highseas_pirate_region_captain" )
	{
		return;
	}
	CreatePirateShipEncounter( null, pirate.x, pirate.y, pirate.z, pirate.worldnumber, pirate.instanceID, pirate );
}

/** @type { ( socket: Socket | null, x: number, y: number, z: number, world: number, instanceID: number, regionCaptain: Character | null ) => any } */
function CreatePirateShipEncounter( socket, x, y, z, world, instanceID, regionCaptain )
{
	let boat = CreateHouse( 210, x, y, z, world, instanceID, 0, true );
	if( !ValidateObject( boat ) || !boat.IsBoat() )
	{
		if( socket )
		{
			socket.SysMessage( "The pirate galleon cannot be placed there." );
		}
		if( ValidateObject( regionCaptain ) )
		{
			regionCaptain.Delete();
		}
		return;
	}

	boat.AddScriptTrigger( 5100 );
	// The Orcish galleon's deck surface is 14 above the hull origin. Using the
	// fixture height puts mobiles a full deck level too high and visually projects
	// them into the masts and sails even when their X/Y is correct.
	let deckZ = boat.z + 14;
	// Keep the captain one tile north of the hull origin and the crew within a
	// one-tile radius of center. The Orcish deck narrows sharply
	// toward both ends, so wider classic-boat offsets can place mobiles outside
	// the walkable multi even though their Z is correct.
	let captain = regionCaptain;
	if( ValidateObject( captain ) )
	{
		captain.id = 0x0190;
		captain.SetLocation( boat.x, boat.y - 1, deckZ, boat.worldnumber, boat.instanceID );
		captain.multi = boat;
		captain.Refresh();
	}
	else
	{
		captain = SpawnPirate( "highseas_pirate_captain", boat, boat.x, boat.y - 1, deckZ );
	}
	if( !ValidateObject( captain ) )
	{
		boat.Delete();
		if( socket )
		{
			socket.SysMessage( "The pirate captain could not be created." );
		}
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
	if( ValidateObject( regionCaptain ) )
	{
		boat.shouldSave = false;
		AreaItemFunction( "MarkRegionPirateItemTransient", boat, 25 );
		AreaCharacterFunction( "MarkRegionPirateCharacterTransient", boat, 25 );
	}
	boat.StartTimer( AI_INTERVAL, 1, 5100 );
	boat.Refresh();
	if( socket )
	{
		socket.SysMessage( "A hostile pirate galleon has entered these waters." );
	}
}

/** @type { ( boat: Multi, item: Item ) => boolean } */
function MarkRegionPirateItemTransient( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat )
	{
		return false;
	}
	item.shouldSave = false;
	return true;
}

/** @type { ( boat: Multi, character: Character ) => boolean } */
function MarkRegionPirateCharacterTransient( boat, character )
{
	if( !ValidateObject( character ) || character.multi != boat )
	{
		return false;
	}
	character.shouldSave = false;
	return true;
}

/** @type { ( section: string, boat: Multi, x: number, y: number, z: number ) => any } */
function SpawnPirate( section, boat, x, y, z )
{
	let pirate = SpawnNPC( section, x, y, z, boat.worldnumber, boat.instanceID, false );
	if( ValidateObject( pirate ) )
	{
		pirate.multi = boat;
	}
	return pirate;
}

/** @type { ( boat: Multi, item: Item ) => any } */
function DeployPirateCannonOnPad( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || !item.isWeaponPad )
	{
		return false;
	}
	TriggerEvent( 5099, "DeployNpcCannon", boat, item, parseInt( boat.GetTempTag( "hsDeployPower" ) ) );
	return true;
}

/** @type { ( boat: Multi ) => any } */
function StockPirateHold( boat )
{
	let hold = boat.GetHold();
	if( !ValidateObject( hold ) )
	{
		return;
	}
	AddHoldLoot( hold, "0x0eed", RandomNumber( 8000, 14000 ) );
	AddHoldLoot( hold, "highseas_cannonball", RandomNumber( 20, 35 ) );
	AddHoldLoot( hold, "highseas_powder_charge", RandomNumber( 15, 25 ) );
	AddHoldLoot( hold, "highseas_fuse_cord", RandomNumber( 15, 25 ) );
	AddHoldLoot( hold, "highseas_grapeshot", RandomNumber( 4, 8 ) );
	AddHoldLoot( hold, "0x1bd7", RandomNumber( 50, 100 ) );
}

/** @type { ( hold: Item, section: string, amount: number ) => any } */
function AddHoldLoot( hold, section, amount )
{
	let loot = CreateDFNItem( null, null, section, amount, "ITEM", false, 0, hold.worldnumber, hold.instanceID );
	if( ValidateObject( loot ) )
	{
		loot.container = hold;
	}
}

/** @type { ( boat: Multi, timerID: number ) => any } */
function onTimer( boat, timerID )
{
	if( !ValidateObject( boat ) || !boat.isItem || !boat.HasScriptTrigger( 5100 ) )
	{
		return;
	}
	if( timerID == 2 )
	{
		if( AreaCharacterFunction( "CountPlayersAboardPirate", boat, 25 ) > 0 )
		{
			boat.decaytime = 30;
			boat.StartTimer( 30000, 2, 5100 );
			return;
		}
		RemovePirateCrew( boat );
		boat.Delete();
		return;
	}
	if( timerID != 1 )
	{
		return;
	}

	let captain = boat.owner;
	RunPirateCaptainAI( boat, captain );
	if( ValidateObject( boat ) && ValidateObject( boat.owner ) )
	{
		boat.StartTimer( AI_INTERVAL, 1, 5100 );
	}
}

// NPC AI slivers are restored with the captain from the world save. This is
// the restart-safe equivalent of BaseShipCaptain.Deserialize scheduling its
// course and crew checks; the boat timer remains only as a live-world fallback.
/** @type { ( pirate: Character ) => any } */
function onAISliver( pirate )
{
	if( !ValidateObject( pirate ) )
	{
		return false;
	}
	let linkedBoat = pirate.multi;
	if( ValidateObject( linkedBoat ) && !ValidateObject( linkedBoat.owner ) && AreaCharacterFunction( "CountPlayersAboardPirate", linkedBoat, 25 ) > 0 )
	{
		linkedBoat.decaytime = 30;
	}
	if( !ValidateObject( linkedBoat ) || linkedBoat.owner != pirate )
	{
		return false;
	}
	let now = GetCurrentClock();
	let nextThink = parseInt( pirate.GetTempTag( "hsNextShipThink" ) );
	if( !isNaN( nextThink ) && now < nextThink )
	{
		return false;
	}
	pirate.SetTempTag( "hsNextShipThink", now + AI_INTERVAL );
	let boat = pirate.multi;
	if( ValidateObject( boat ) )
	{
		RunPirateCaptainAI( boat, pirate );
	}
	return false;
}

/** @type { ( boat: Multi, character: Character ) => any } */
function CountPlayersAboardPirate( boat, character ) { return ValidateObject( character ) && !character.npc && character.multi == boat; }

/** @type { ( boat: Multi, captain: Character ) => any } */
function RunPirateCaptainAI( boat, captain )
{
	let now = GetCurrentClock();
	let nextBoatThink = parseInt( boat.GetTempTag( "hsNextCaptainThink" ) );
	if( !isNaN( nextBoatThink ) && now < nextBoatThink )
	{
		return;
	}
	boat.SetTempTag( "hsNextCaptainThink", now + AI_INTERVAL );
	if( !ValidateObject( captain ) || captain.dead || ( boat.GetHullMaxHits() > 0 && boat.GetHullHits() * 4 < boat.GetHullMaxHits() ) )
	{
		DefeatPirateGalleon( boat );
		return;
	}
	// BaseShipCaptain.CheckCrew returns strays to the galleon's current surface.
	// The captain is already carried at that surface, avoiding a hull-specific
	// hard-coded Z when restoring crew after movement or a world reload.
	boat.SetTempTag( "hsPirateDeckZ", captain.z );
	let nextCrewCheck = parseInt( captain.GetTempTag( "hsNextCrewCheck" ) );
	if( isNaN( nextCrewCheck ) || now >= nextCrewCheck )
	{
		AreaCharacterFunction( "CorrectPirateCrewDeckZ", boat, 25 );
		captain.SetTempTag( "hsNextCrewCheck", now + 1800000 );
	}

	// Retain a live target while it remains in the pursuit envelope. This
	// avoids rebuilding the target every 250ms and lets the captain finish a
	// broadside approach when the player's mobile is briefly outside the scan.
	let targetBoat = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBestTarget" ) ) );
	let targetChar = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBestTargetChar" ) ) );
	if( !IsValidPirateTarget( boat, targetBoat, targetChar, 35 ) )
	{
		boat.SetTempTag( "hsBestTarget", 0 );
		boat.SetTempTag( "hsBestTargetChar", 0 );
		boat.SetTempTag( "hsBestDistance", 35 + 1 );
		AreaCharacterFunction( "FindPirateShipTarget", boat, 35 );
		targetBoat = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBestTarget" ) ) );
		targetChar = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBestTargetChar" ) ) );
	}
	if( ValidateObject( targetBoat ) && targetBoat.IsBoat() )
	{
		NavigatePirateGalleon( boat, targetBoat, captain );
	}
}

/** @type { ( pirateBoat: Multi, targetBoat: Multi, targetChar: Character, maxRange: number ) => any } */
function IsValidPirateTarget( pirateBoat, targetBoat, targetChar, maxRange )
{
	if( !ValidateObject( targetBoat ) || !targetBoat.IsBoat() || targetBoat == pirateBoat || targetBoat.HasScriptTrigger( 5100 )
		|| !ValidateObject( targetChar ) || targetChar.npc || targetChar.dead || !targetChar.online || targetChar.multi != targetBoat )
	{
		return false;
	}
	return Math.max( Math.abs( targetBoat.x - pirateBoat.x ), Math.abs( targetBoat.y - pirateBoat.y ) ) <= maxRange;
}

/** @type { ( boat: Multi, character: Character ) => any } */
function CorrectPirateCrewDeckZ( boat, character )
{
	if( !ValidateObject( character ) || character.multi != boat )
	{
		return false;
	}
	let deckZ = parseInt( boat.GetTempTag( "hsPirateDeckZ" ) );
	if( character.z < deckZ - 2 || character.z > deckZ + 2 )
	{
		character.SetLocation( character.x, character.y, deckZ, boat.worldnumber, boat.instanceID );
	}
	character.multi = boat;
	return true;
}

/** @type { ( pirateBoat: Multi, candidate: Item ) => any } */
function FindPirateShipTarget( pirateBoat, candidate )
{
	if( !ValidateObject( candidate ) || candidate.npc || candidate.dead || !candidate.online )
	{
		return false;
	}
	let targetBoat = candidate.multi;
	if( !ValidateObject( targetBoat ) || !targetBoat.IsBoat() || targetBoat == pirateBoat || targetBoat.HasScriptTrigger( 5100 ) )
	{
		return false;
	}
	let distance = Math.max( Math.abs( targetBoat.x - pirateBoat.x ), Math.abs( targetBoat.y - pirateBoat.y ) );
	if( distance < parseInt( pirateBoat.GetTempTag( "hsBestDistance" ) ) )
	{
		pirateBoat.SetTempTag( "hsBestDistance", distance );
		pirateBoat.SetTempTag( "hsBestTarget", targetBoat.serial );
		pirateBoat.SetTempTag( "hsBestTargetChar", candidate.serial );
	}
	return true;
}

/** @type { ( boat: Multi, target: Character | Item | null, captain: Character ) => any } */
function NavigatePirateGalleon( boat, target, captain )
{
	let dx = target.x - boat.x;
	let dy = target.y - boat.y;
	let distance = Math.max( Math.abs( dx ), Math.abs( dy ) );
	let now = GetCurrentClock();
	let navTarget = parseInt( boat.GetTempTag( "hsNavTarget" ) );
	let pursuing = parseInt( boat.GetTempTag( "hsPursuing" ) ) == 1;
	let resumeAt = parseInt( boat.GetTempTag( "hsResumePursuitAt" ) );
	if( isNaN( resumeAt ) )
	{
		resumeAt = 0;
	}

	// SailBoat does not report a blocked native move back to JavaScript. Detect
	// an unchanged position on the following AI tick and loiter before retrying,
	// instead of hammering the obstruction and spamming the tiller message.
	let attemptPending = parseInt( boat.GetTempTag( "hsMoveAttemptPending" ) ) == 1;
	let attemptX = parseInt( boat.GetTempTag( "hsMoveAttemptX" ) );
	let attemptY = parseInt( boat.GetTempTag( "hsMoveAttemptY" ) );
	if( attemptPending )
	{
		boat.SetTempTag( "hsMoveAttemptPending", 0 );
		if( !isNaN( attemptX ) && !isNaN( attemptY ) && attemptX == boat.x && attemptY == boat.y )
		{
			pursuing = false;
			boat.SetTempTag( "hsPursuing", 0 );
			resumeAt = now + 2000;
			boat.SetTempTag( "hsResumePursuitAt", resumeAt );
		}
	}
	if( navTarget != target.serial )
	{
		boat.SetTempTag( "hsNavTarget", target.serial );
		pursuing = distance > 20;
		boat.SetTempTag( "hsPursuing", pursuing ? 1 : 0 );
		resumeAt = 0;
	}

	// High Seas hulls can span roughly twenty tiles end-to-end when two vessels
	// approach bow-first. Stop before their collision footprints touch.
	if( pursuing && distance <= 20 )
	{
		// Pause before resuming scripted one-tile movement. Do not chase the
		// target's changing relative offset during the pause.
		pursuing = false;
		boat.SetTempTag( "hsPursuing", 0 );
		resumeAt = now + 2000;
		boat.SetTempTag( "hsResumePursuitAt", resumeAt );
	}
	else if( !pursuing && distance >= 24 && now >= resumeAt )
	{
		pursuing = true;
		boat.SetTempTag( "hsPursuing", 1 );
	}

	if( pursuing && distance <= 35 )
	{
		// SailBoat uses collision-checked one-tile movement and does not require
		// the bow to point at the target.
		boat.SetTempTag( "hsMoveAttemptX", boat.x );
		boat.SetTempTag( "hsMoveAttemptY", boat.y );
		boat.SetTempTag( "hsMoveAttemptPending", 1 );
		boat.SailBoat( DirectionToPoint( boat.x, boat.y, target.x, target.y ) );
	}
	else if( distance <= 24 )
	{
		// While loitering, rotate only as needed to put a beam toward the target.
		// The hull remains stationary, so this does not tether the two vessels.
		AlignPirateForTarget( boat, dx, dy );
	}

	let nextFireCheck = parseInt( boat.GetTempTag( "hsNextBroadsideCheck" ) );
	if( distance <= 25 && ( isNaN( nextFireCheck ) || now >= nextFireCheck ) )
	{
		boat.SetTempTag( "hsNextBroadsideCheck", now + 750 );
		boat.SetTempTag( "hsBroadsideTarget", target.serial );
		boat.SetTempTag( "hsBroadsideCaptain", captain.serial );
		AreaItemFunction( "FirePirateBroadside", boat, 25 );
	}
}

/** @type { ( fromX: number, fromY: number, toX: number, toY: number ) => any } */
function DirectionToPoint( fromX, fromY, toX, toY )
{
	let dx = toX - fromX;
	let dy = toY - fromY;
	let adx = Math.abs( dx );
	let ady = Math.abs( dy );
	if( dx == 0 )
	{
		return dy > 0 ? 4 : 0;
	}
	if( dy == 0 )
	{
		return dx > 0 ? 2 : 6;
	}
	if( adx > ady * 2 )
	{
		return dx > 0 ? 2 : 6;
	}
	if( ady > adx * 2 )
	{
		return dy > 0 ? 4 : 0;
	}
	if( dx > 0 )
	{
		return dy > 0 ? 3 : 1;
	}
	return dy > 0 ? 5 : 7;
}

/** @type { ( boat: Multi, dx: number, dy: number ) => any } */
function AlignPirateForTarget( boat, dx, dy )
{
	let current = parseInt( boat.dir ) & 0x06;
	// If the target is east/west, a north/south hull exposes its beam; if the
	// target is north/south, use an east/west hull. Pick the nearer parallel
	// orientation to avoid unnecessary 180-degree turns.
	let first = Math.abs( dx ) >= Math.abs( dy ) ? 0 : 2;
	let second = ( first + 4 ) & 0x07;
	let firstTurns = Math.min( ( first - current + 8 ) % 8, ( current - first + 8 ) % 8 );
	let secondTurns = Math.min( ( second - current + 8 ) % 8, ( current - second + 8 ) % 8 );
	let desired = firstTurns <= secondTurns ? first : second;
	if( current == desired )
	{
		return;
	}
	let delta = ( desired - current + 8 ) % 8;
	boat.TurnBoat( delta == 2 || delta == 4 ? 2 : 1 );
}

/** @type { ( boat: Multi, item: Item ) => any } */
function FirePirateBroadside( boat, item )
{
	if( !ValidateObject( item ) || item.multi != boat || !item.isShipCannon )
	{
		return false;
	}
	let target = CalcItemFromSer( parseInt( boat.GetTempTag( "hsBroadsideTarget" ) ) );
	let captain = CalcCharFromSer( parseInt( boat.GetTempTag( "hsBroadsideCaptain" ) ) );
	let now = GetCurrentClock();
	let nextFire = parseInt( item.GetTempTag( "hsNextFire" ) );
	if( ValidateObject( target ) && ValidateObject( captain ) && ( isNaN( nextFire ) || now >= nextFire )
		&& TriggerEvent( 5099, "FireNpcCannon", item, target, captain ) )
	{
		// Use a ten-second cooldown, plus a random zero-to-three-second delay.
		item.SetTempTag( "hsNextFire", now + 10000 + RandomNumber( 0, 3000 ) );
	}
	return true;
}

/** @type { ( deadChar: Character, corpse: Item ) => any } */
function onDeath( deadChar, corpse )
{
	if( !ValidateObject( deadChar ) )
	{
		return;
	}
	let boat = deadChar.multi;
	if( !ValidateObject( boat ) || boat.owner != deadChar || !boat.HasScriptTrigger( 5100 ) )
	{
		return;
	}
	if( ValidateObject( boat ) )
	{
		DefeatPirateGalleon( boat );
	}
}

/** @type { ( boat: Multi ) => any } */
function DefeatPirateGalleon( boat )
{
	if( !ValidateObject( boat.owner ) )
	{
		return;
	}
	boat.owner = null;
	let hold = boat.GetHold();
	if( ValidateObject( hold ) )
	{
		hold.owner = null;
	}
	let tiller = boat.GetTiller();
	if( ValidateObject( tiller ) )
	{
		tiller.TextMessage( null, "The pirate vessel is defeated! Its hold may now be plundered." );
	}
	// Give a defeated captain's galleon a thirty-minute decay window. Native
	// decay state is serialized, so cleanup survives a shard restart.
	boat.decayable = true;
	boat.decaytime = 1800;
	boat.StartTimer( 1800000, 2, 5100 );
}

/** @type { ( boat: Multi ) => any } */
function RemovePirateCrew( boat ) { AreaCharacterFunction( "DeletePirateCrewMember", boat, 25 ); }

/** @type { ( boat: Multi, character: Character ) => any } */
function DeletePirateCrewMember( boat, character )
{
	if( ValidateObject( character ) && character.multi == boat )
	{
		character.Delete();
	}
	return true;
}
