/// <reference path="../definitions.d.ts" />
// @ts-check

// Optional, T2A-inspired NPC drives. This controller owns an AI slice only while
// an eligible NPC is pursuing a selected need. Attached scripts remain first in
// the engine's dispatch order, and hard-coded AI remains the fallback.

const AI_ANIMAL = 6;
const AI_ANIMAL_SCARED = 12;
const WT_NONE = 0;
const WT_FOLLOW = 1;
const WT_FREE = 2;
const WT_FROZEN = 5;
const WT_FLEE = 6;
const WT_SCARED = 8;
const MAX_HUNGER = 6;
const DEFAULT_FOOD_THRESHOLD = 3;
const CRITICAL_HUNGER = 1;
const DEFAULT_SEARCH_RADIUS = 12;
const DEFAULT_SHELTER_RADIUS = 24;
const DEFAULT_LEASH_RADIUS = 24;
const DEFAULT_REST_HEALTH = 25;
const DEFAULT_RESUME_HEALTH = 75;
const DEFAULT_SHELTER_START = 22;
const DEFAULT_SHELTER_END = 6;
const DEFAULT_DESIRE_RADIUS = 12;
const DEFAULT_DESIRE_STAY_RANGE = 2;
const DEFAULT_DESIRE_DURATION = 15;
const DEFAULT_TERRITORY_RADIUS = 12;
const DEFAULT_HOARD_GUARD_RADIUS = 4;
const DEFAULT_PACK_RADIUS = 12;
const DEFAULT_PACK_FOLLOW_DISTANCE = 4;
const DEFAULT_BOREDOM_DELAY = 60;
const DEFAULT_IDLE_DURATION = 10;
const DEFAULT_IDLE_RADIUS = 8;
const MAX_SEARCH_RADIUS = 24;
const MAX_SHELTER_RADIUS = 64;
const MAX_LEASH_RADIUS = 64;
const THINK_INTERVAL = 5000;
const RETRY_INTERVAL = 10000;
const AVERSION_REPATH_INTERVAL = 2000;
const SOCIAL_SCAN_INTERVAL = 3000;

/** @type { BaseObject | null } */
var gNpcDriveCandidate = null;
var gNpcDriveCandidateDistance = Number.MAX_VALUE;
var gNpcDriveResource = "";
var gNpcDriveCandidateAction = "";
var gNpcDriveFindingAversion = false;
/** @type { Character | null } */
var gNpcDriveDefender = null;
/** @type { BaseObject | null } */
var gNpcDriveHoard = null;
/** @type { string[] } */
var gNpcDriveResourceList = [];
var gNpcDriveDefenseRadius = 0;
var gNpcDriveMatchedResource = "";
var gNpcDrivePackId = "";
/** @type { string[] } */
var gNpcDriveDiagnosticResources = [];
var gNpcDriveDiagnosticResult = "";
var gNpcDriveDiagnosticDistance = Number.MAX_VALUE;

/** @param {Character} npc @returns {boolean} */
function onAISliver( npc )
{
	if( !ValidateObject( npc ) || !npc.npc )
		return false;
	npc.SetTempTag( "npcDriveLastAISliver", GetCurrentClock().toString() );

	var activeMode = GetDriveMode( npc );
	if( !GetServerSetting( "NpcDriveSystem" ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "system disabled" );
		if( activeMode )
			ClearDriveGoal( npc, true, RETRY_INTERVAL );
		ClearDefenseState( npc );
		ClearSocialState( npc );
		return false;
	}

	if( activeMode )
	{
		npc.SetTempTag( "npcDriveLastDecision", "continuing " + activeMode );
		var activeReason = npc.GetTempTag( "npcDriveReason" ).toString();
		if( activeMode != "aversion" && activeReason != "recovery" && IsEligibleNpc( npc ) && !npc.atWar && !ValidateObject( npc.target ) && npc.wandertype != WT_FLEE && npc.wandertype != WT_SCARED )
		{
			EnsureHomePosition( npc );
			if( !DriveDisabled( npc, "Defense" ) && TryDefense( npc ))
				return false;
			if( !DriveDisabled( npc, "Social" ) && TrySocialAssist( npc ))
				return false;
		}
		return HandleActiveGoal( npc, activeMode );
	}

	if( !IsEligibleNpc( npc ) || npc.atWar || ValidateObject( npc.target ) || npc.wandertype == WT_FLEE || npc.wandertype == WT_SCARED )
	{
		npc.SetTempTag( "npcDriveLastDecision", "native AI state blocked drive scan" );
		return false;
	}

	var now = GetCurrentClock();
	var nextThink = parseInt( npc.GetTempTag( "npcDriveNextThink" ));
	if( !isNaN( nextThink ) && nextThink > now )
		return false;

	npc.SetTempTag( "npcDriveNextThink", ( now + THINK_INTERVAL ).toString() );
	npc.SetTempTag( "npcDriveLastFullScan", now.toString() );
	npc.SetTempTag( "npcDriveLastDecision", "evaluating drives" );
	EnsureHomePosition( npc );

	if( !DriveDisabled( npc, "Desire" ) && TryAversion( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started aversion" );
		return true;
	}

	// Combat survival continues to use UOX3's native fleeAt/reAttackAt behavior.
	// This branch handles out-of-combat recovery using normal regeneration.
	if( !DriveDisabled( npc, "Survival" ) && NeedsRecovery( npc ) && TryShelter( npc, "recovery" ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started recovery" );
		return true;
	}

	// Defense chooses a legal target through the drive layer, then immediately
	// hands combat back to UOX3's native combat and AI systems.
	if( !DriveDisabled( npc, "Defense" ) && TryDefense( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started defense" );
		return false;
	}

	if( !DriveDisabled( npc, "Social" ) && TrySocialAssist( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started social assist" );
		return false;
	}

	var foodThreshold = GetBoundedTag( npc, "NpcDriveFoodThreshold", DEFAULT_FOOD_THRESHOLD, 1, MAX_HUNGER - 1 );
	if( !DriveDisabled( npc, "Food" ) && CanSeekFood( npc ) && npc.hunger <= CRITICAL_HUNGER && TryFood( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started critical food" );
		return true;
	}

	if( !DriveDisabled( npc, "Shelter" ) && IsShelterTime( npc ) && TryShelter( npc, "night" ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started shelter" );
		return true;
	}

	if( !DriveDisabled( npc, "Food" ) && CanSeekFood( npc ) && npc.hunger <= foodThreshold && TryFood( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started food" );
		return true;
	}

	if( !DriveDisabled( npc, "Return" ) && TryPendingReturn( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started resource return" );
		return true;
	}

	if( !DriveDisabled( npc, "Social" ) && TrySocialRegroup( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started social regroup" );
		return true;
	}

	if( !DriveDisabled( npc, "Desire" ) && TryDesire( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started desire" );
		return true;
	}

	if( !DriveDisabled( npc, "Boredom" ) && TryBoredom( npc ))
	{
		npc.SetTempTag( "npcDriveLastDecision", "started boredom" );
		return true;
	}

	npc.SetTempTag( "npcDriveLastDecision", "no drive selected" );
	return false;
}

/** @param {Character} npc @param {number} pathfindResult @returns {boolean} */
function onPathfindEnd( npc, pathfindResult )
{
	if( !ValidateObject( npc ))
		return false;

	var mode = GetDriveMode( npc );
	if( mode && pathfindResult == -2 )
	{
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return true;
	}

	if( mode == "food" )
	{
		var food = GetDriveTarget( npc );
		if( GetServerSetting( "NpcDriveSystem" ) && pathfindResult == 1 && IsUsableFood( npc, food ) && npc.InRange( food, 1 ))
			EatFood( npc, food );

		ClearDriveGoal( npc, false, pathfindResult == 1 ? THINK_INTERVAL : RETRY_INTERVAL );
		return true;
	}

	if( mode == "shelter" )
	{
		var shelter = GetDriveTarget( npc );
		if( GetServerSetting( "NpcDriveSystem" ) && pathfindResult == 1 && IsShelterMarker( shelter ) && npc.InRange( shelter, 1 ) && ShouldContinueShelter( npc ))
			BeginRest( npc, shelter );
		else
			ClearDriveGoal( npc, false, RETRY_INTERVAL );
		return true;
	}

	if( mode == "desire" )
	{
		HandleDesirePathEnd( npc, pathfindResult );
		return true;
	}

	if( mode == "aversion" )
	{
		HandleAversionPathEnd( npc );
		return true;
	}

	if( mode == "social" )
	{
		ClearDriveGoal( npc, false, pathfindResult == 1 ? THINK_INTERVAL : RETRY_INTERVAL );
		return true;
	}

	if( mode == "return" )
	{
		HandleReturnPathEnd( npc, pathfindResult );
		return true;
	}

	if( mode == "idle" )
	{
		FinishIdleActivity( npc, pathfindResult == 1 ? THINK_INTERVAL : RETRY_INTERVAL );
		return true;
	}

	return false;
}

/** @param {Character} npc @returns {boolean} */
function AdminCancelDriveGoal( npc )
{
	if( !ValidateObject( npc ) || !npc.npc )
		return false;

	var mode = GetDriveMode( npc );
	if( mode == "return" )
		ClearPendingReturn( npc );
	if( mode )
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
	else
		npc.SetTempTag( "npcDriveNextThink", ( GetCurrentClock() + RETRY_INTERVAL ).toString() );
	ClearDefenseState( npc );
	ClearSocialState( npc );
	npc.SetTempTag( "npcDriveDefenseNextScan", ( GetCurrentClock() + RETRY_INTERVAL ).toString() );
	npc.SetTempTag( "npcDriveSocialNextScan", ( GetCurrentClock() + RETRY_INTERVAL ).toString() );
	return true;
}

/** @param {Character} npc @returns {boolean} */
function AdminForceDriveReevaluation( npc )
{
	if( !ValidateObject( npc ) || !npc.npc )
		return false;

	if( GetDriveMode( npc ))
		ClearDriveGoal( npc, true, 0 );
	ClearDefenseState( npc );
	ClearSocialState( npc );
	npc.SetTempTag( "npcDriveDefenseNextScan", null );
	npc.SetTempTag( "npcDriveSocialNextScan", null );
	npc.SetTempTag( "npcDriveBoredomNext", null );
	npc.SetTempTag( "npcDriveNextThink", null );
	return true;
}

/** @param {Character} npc @returns {string} */
function AdminDescribePickup( npc )
{
	if( !ValidateObject( npc ) || !npc.npc )
		return "Pickup diagnostic: the selected object is not an NPC.";
	if( !GetServerSetting( "NpcDriveSystem" ))
		return "Pickup diagnostic: NPCDRIVESYSTEM is disabled.";
	if( !IsEligibleNpc( npc ))
		return "Pickup diagnostic: this NPC is not eligible for the drive system.";
	if( DriveDisabled( npc, "Desire" ))
		return "Pickup diagnostic: the Desire drive is disabled.";

	var entries = ParseDesireEntries( npc.GetTag( "NpcDriveDesires" ));
	var pickupResources = [];
	var coolingResources = [];
	for( var i = 0; i < entries.length; ++i )
	{
		if( entries[i].action != "pickup" )
			continue;
		if( !IsDesireReady( npc, entries[i].resource ))
		{
			coolingResources.push( entries[i].resource );
			continue;
		}
		if( pickupResources.indexOf( entries[i].resource ) == -1 )
			pickupResources.push( entries[i].resource );
	}
	if( pickupResources.length == 0 )
	{
		if( coolingResources.length > 0 )
			return "Pickup diagnostic: pickup desires are cooling down: " + coolingResources.join( ", " ) + ".";
		return "Pickup diagnostic: no pickup desire is configured. Example: gold:pickup:15";
	}

	EnsureHomePosition( npc );
	gNpcDriveDiagnosticResources = pickupResources;
	gNpcDriveDiagnosticResult = "";
	gNpcDriveDiagnosticDistance = Number.MAX_VALUE;
	var searchRadius = GetBoundedTag( npc, "NpcDriveDesireSearchRadius", DEFAULT_DESIRE_RADIUS, 1, MAX_SEARCH_RADIUS );
	AreaItemFunction( "NpcDriveDiagnosePickupItem", npc, searchRadius, null );
	var result = gNpcDriveDiagnosticResult;
	gNpcDriveDiagnosticResources = [];
	gNpcDriveDiagnosticResult = "";
	gNpcDriveDiagnosticDistance = Number.MAX_VALUE;

	var activeMode = GetDriveMode( npc );
	var busyReason = "";
	if( npc.atWar || ValidateObject( npc.target ))
		busyReason = " NPC is currently in combat.";
	else if( npc.wandertype == WT_FLEE || npc.wandertype == WT_SCARED )
		busyReason = " NPC is currently fleeing.";
	else if( activeMode && activeMode != "desire" )
		busyReason = " NPC is currently busy with drive '" + activeMode + "' (" + npc.GetTempTag( "npcDriveReason" ).toString() + ").";

	var lastSlice = parseInt( npc.GetTempTag( "npcDriveLastAISliver" ));
	if( isNaN( lastSlice ))
		busyReason += " The drive AI callback has not run for this NPC.";
	else
	{
		var sliceAge = Math.max( 0, GetCurrentClock() - lastSlice );
		busyReason += " Last drive AI slice: " + sliceAge + " ms ago.";
	}
	var lastDecision = npc.GetTempTag( "npcDriveLastDecision" );
	if( lastDecision )
		busyReason += " Last decision: " + lastDecision.toString() + ".";
	var lastFullScan = parseInt( npc.GetTempTag( "npcDriveLastFullScan" ));
	if( !isNaN( lastFullScan ) && lastFullScan > 0 )
		busyReason += " Last full scan: " + Math.max( 0, GetCurrentClock() - lastFullScan ) + " ms ago.";
	var pendingReturn = npc.GetTempTag( "npcDrivePendingReturnItem" );
	if( ParseStoredSerial( pendingReturn ) >= 0 )
		busyReason += " Pending return serial: " + pendingReturn.toString() + ".";

	if( result )
		return "Pickup diagnostic: " + result + busyReason;
	return "Pickup diagnostic: no ground item matching " + pickupResources.join( ", " ) + " was found within " + searchRadius + " tiles of the NPC." + busyReason;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function NpcDriveDiagnosePickupItem( npc, item )
{
	var resource = GetFirstMatchingResource( item, gNpcDriveDiagnosticResources );
	if( !resource )
		return false;
	var distance = DistanceBetween( npc, item, true );
	var rejection = GetPickupRejectionReason( npc, item );
	var isReady = !rejection;
	var currentReady = gNpcDriveDiagnosticResult.indexOf( "READY" ) == 0;
	if(( isReady && !currentReady ) || ( isReady == currentReady && distance < gNpcDriveDiagnosticDistance ))
	{
		gNpcDriveDiagnosticDistance = distance;
		var description = resource + " item 0x" + item.id.toString( 16 ).toUpperCase() + " x" + item.amount + " at distance " + distance;
		gNpcDriveDiagnosticResult = isReady ? "READY - " + description + "." : "REJECTED - " + description + ": " + rejection + ".";
	}
	return true;
}

/** @param {Character} npc @param {Item} item @returns {string} */
function GetPickupRejectionReason( npc, item )
{
	if( !ValidateObject( item ) || !item.isItem )
		return "invalid item";
	if( item.container != null )
		return "item is inside a container";
	if( item.visible > 1 )
		return "item is hidden";
	if( !IsPickupMovable( item ))
		return "item is immovable (movable=" + item.movable + ", weight=" + item.weight + ")";
	if( !IsInsideLeash( npc, item ))
		return "item is outside the NPC's home leash";
	if( !ReservationAvailable( npc, item ))
	{
		var reservation = item.GetTempTag( "npcDriveReservedBy" );
		return "item is reserved by NPC serial " + reservation.toString();
	}
	if( !ValidateObject( npc.pack ))
		return "NPC has no backpack";
	if( !CanCarryDesireItem( npc, item ))
		return "NPC backpack cannot carry the stack (item weight=" + item.weight + ", amount=" + item.amount + ", pack weight=" + npc.pack.weight + ", max=" + npc.pack.weightMax + ")";
	return "";
}

/** @param {Character} npc @param {string} mode @returns {boolean} */
function HandleActiveGoal( npc, mode )
{
	if( !IsEligibleNpc( npc ) || npc.atWar || ValidateObject( npc.target ) || npc.wandertype == WT_FLEE || npc.wandertype == WT_SCARED )
	{
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return false;
	}

	if( mode == "food" )
	{
		var food = GetDriveTarget( npc );
		var foodThreshold = GetBoundedTag( npc, "NpcDriveFoodThreshold", DEFAULT_FOOD_THRESHOLD, 1, MAX_HUNGER - 1 );
		if( DriveDisabled( npc, "Food" ) || npc.hunger > foodThreshold || !IsUsableFood( npc, food ))
		{
			ClearDriveGoal( npc, true, RETRY_INTERVAL );
			return false;
		}

		if( npc.InRange( food, 1 ))
		{
			EatFood( npc, food );
			ClearDriveGoal( npc, true, THINK_INTERVAL );
		}
		return true;
	}

	if( mode == "shelter" )
	{
		var shelter = GetDriveTarget( npc );
		if( !ShouldContinueShelter( npc ) || !IsShelterMarker( shelter ))
		{
			ClearDriveGoal( npc, true, RETRY_INTERVAL );
			return false;
		}

		if( npc.InRange( shelter, 1 ))
			BeginRest( npc, shelter );
		return true;
	}

	if( mode == "rest" )
	{
		if( !ShouldContinueShelter( npc ) || !IsAtRestLocation( npc ))
		{
			ClearDriveGoal( npc, true, THINK_INTERVAL );
			return false;
		}

		npc.wandertype = WT_NONE;
		// Rest controls idle movement, but still lets hard-coded AI scan and react to
		// threats. If core AI acquires a target, the goal is cancelled next slice.
		return false;
	}

	if( mode == "desire" )
		return HandleActiveDesire( npc );

	if( mode == "aversion" )
		return HandleActiveAversion( npc );

	if( mode == "social" )
		return HandleActiveSocial( npc );

	if( mode == "return" )
		return HandleActiveReturn( npc );

	if( mode == "idle" )
		return HandleActiveIdle( npc );

	ClearDriveGoal( npc, true, RETRY_INTERVAL );
	return false;
}

/** @param {Character} npc @returns {boolean} */
function TryFood( npc )
{
	var searchRadius = GetBoundedTag( npc, "NpcDriveSearchRadius", DEFAULT_SEARCH_RADIUS, 1, MAX_SEARCH_RADIUS );
	ResetCandidate();
	AreaItemFunction( "NpcDriveConsiderFood", npc, searchRadius, null );

	if( !ValidateObject( gNpcDriveCandidate ))
		return false;

	var food = gNpcDriveCandidate;
	if( npc.InRange( food, 1 ))
	{
		EatFood( npc, food );
		return true;
	}

	RememberOriginalWander( npc );
	food.SetTempTag( "npcDriveReservedBy", npc.serial.toString() );
	npc.SetTempTag( "npcDriveMode", "food" );
	npc.SetTempTag( "npcDriveTarget", food.serial.toString() );
	npc.SetTempTag( "npcDriveTargetType", "item" );
	npc.WalkTo( food, GetPathMaxSteps() );
	return true;
}

/** @param {Character} npc @param {string} reason @returns {boolean} */
function TryShelter( npc, reason )
{
	npc.SetTempTag( "npcDriveReason", reason );
	if( IsInBuilding( npc.x, npc.y, npc.z, npc.worldnumber, npc.instanceID, true ))
	{
		RememberOriginalWander( npc );
		npc.SetTempTag( "npcDriveRestInPlace", 1 );
		BeginRest( npc, null );
		return true;
	}

	var searchRadius = GetBoundedTag( npc, "NpcDriveShelterRadius", DEFAULT_SHELTER_RADIUS, 1, MAX_SHELTER_RADIUS );
	ResetCandidate();
	AreaItemFunction( "NpcDriveConsiderShelter", npc, searchRadius, null );
	if( !ValidateObject( gNpcDriveCandidate ))
	{
		npc.SetTempTag( "npcDriveReason", null );
		return false;
	}

	var shelter = gNpcDriveCandidate;
	RememberOriginalWander( npc );
	npc.SetTempTag( "npcDriveMode", "shelter" );
	npc.SetTempTag( "npcDriveTarget", shelter.serial.toString() );
	npc.SetTempTag( "npcDriveTargetType", "item" );
	if( npc.InRange( shelter, 1 ))
	{
		BeginRest( npc, shelter );
		return true;
	}

	npc.WalkTo( shelter, GetPathMaxSteps() );
	return true;
}

/** @param {Character} npc @returns {boolean} */
function TryDesire( npc )
{
	var entries = ParseDesireEntries( npc.GetTag( "NpcDriveDesires" ));
	if( entries.length == 0 )
		return false;

	var searchRadius = GetBoundedTag( npc, "NpcDriveDesireSearchRadius", DEFAULT_DESIRE_RADIUS, 1, MAX_SEARCH_RADIUS );
	for( var i = 0; i < entries.length; ++i )
	{
		var entry = entries[i];
		if( !IsDesireReady( npc, entry.resource ))
			continue;
		var target = FindResourceCandidate( npc, entry.resource, entry.action, searchRadius, false );
		if( ValidateObject( target ))
			return StartDesire( npc, entry, target );
	}
	return false;
}

/** @param {Character} npc @returns {boolean} */
function TryAversion( npc )
{
	var entries = ParseAversionEntries( npc.GetTag( "NpcDriveAversions" ));
	if( entries.length == 0 )
		return false;

	var configuredRadius = GetBoundedTag( npc, "NpcDriveAversionSearchRadius", DEFAULT_DESIRE_RADIUS, 1, MAX_SEARCH_RADIUS );
	for( var i = 0; i < entries.length; ++i )
	{
		var entry = entries[i];
		var searchRadius = Math.max( configuredRadius, entry.radius );
		var target = FindResourceCandidate( npc, entry.resource, "nearby", searchRadius, true );
		if( ValidateObject( target ) && DistanceBetween( npc, target, true ) < entry.radius )
			return StartAversion( npc, entry, target );
	}
	return false;
}

/** @param {Character} npc @returns {boolean} */
function TryDefense( npc )
{
	var now = GetCurrentClock();
	var nextScan = parseInt( npc.GetTempTag( "npcDriveDefenseNextScan" ));
	if( !isNaN( nextScan ) && nextScan > now )
		return false;
	npc.SetTempTag( "npcDriveDefenseNextScan", ( now + THINK_INTERVAL ).toString() );
	ClearDefenseState( npc );
	var intruderResources = ParseResourceList( npc.GetTag( "NpcDriveTerritoryTargets" ));
	if( intruderResources.length == 0 )
		return false;

	var territoryRadius = GetBoundedTag( npc, "NpcDriveTerritoryRadius", DEFAULT_TERRITORY_RADIUS, 1, MAX_SEARCH_RADIUS );
	var intruder = FindTerritoryIntruder( npc, intruderResources, territoryRadius );
	var reason = ValidateObject( intruder ) ? "territory:" + gNpcDriveMatchedResource : "";

	if( !ValidateObject( intruder ))
	{
		var hoardResources = ParseResourceList( npc.GetTag( "NpcDriveHoardResources" ));
		if( hoardResources.length > 0 )
		{
			var hoard = FindHoardItem( npc, hoardResources, territoryRadius );
			if( ValidateObject( hoard ))
			{
				var guardRadius = GetBoundedTag( npc, "NpcDriveHoardGuardRadius", DEFAULT_HOARD_GUARD_RADIUS, 1, MAX_SEARCH_RADIUS );
				intruder = FindHoardIntruder( npc, hoard, intruderResources, guardRadius );
				if( ValidateObject( intruder ))
					reason = "hoard:" + gNpcDriveMatchedResource;
			}
		}
	}

	if( !ValidateObject( intruder ) || !npc.InitiateCombat( intruder ))
		return false;

	// A successful defense response supersedes movement owned by a lower-priority
	// drive. Release reservations/follow state without disturbing the combat target.
	if( GetDriveMode( npc ))
		ClearDriveGoal( npc, true, 0 );
	npc.SetTempTag( "npcDriveDefenseReason", reason );
	npc.SetTempTag( "npcDriveDefenseTarget", intruder.serial.toString() );
	npc.SetTempTag( "npcDriveDefenseTime", GetCurrentClock().toString() );
	return true;
}

/** @param {Character} npc @param {string[]} resources @param {number} radius @returns {Character | null} */
function FindTerritoryIntruder( npc, resources, radius )
{
	if( !IsInsideHomeRadius( npc, npc, radius ))
		return null;
	ResetDefenseCandidate( npc, resources, radius, null );
	AreaCharacterFunction( "NpcDriveConsiderTerritoryIntruder", npc, Math.min( MAX_LEASH_RADIUS, radius * 2 ), null );
	return ValidateObject( gNpcDriveCandidate ) && gNpcDriveCandidate.isChar ? gNpcDriveCandidate : null;
}

/** @param {Character} npc @param {string[]} resources @param {number} territoryRadius @returns {Item | null} */
function FindHoardItem( npc, resources, territoryRadius )
{
	if( !IsInsideHomeRadius( npc, npc, territoryRadius ))
		return null;
	ResetDefenseCandidate( npc, resources, territoryRadius, null );
	AreaItemFunction( "NpcDriveConsiderHoardItem", npc, Math.min( MAX_LEASH_RADIUS, territoryRadius * 2 ), null );
	return ValidateObject( gNpcDriveCandidate ) && gNpcDriveCandidate.isItem ? gNpcDriveCandidate : null;
}

/** @param {Character} npc @param {Item} hoard @param {string[]} resources @param {number} radius @returns {Character | null} */
function FindHoardIntruder( npc, hoard, resources, radius )
{
	ResetDefenseCandidate( npc, resources, radius, hoard );
	AreaCharacterFunction( "NpcDriveConsiderHoardIntruder", hoard, radius, null );
	return ValidateObject( gNpcDriveCandidate ) && gNpcDriveCandidate.isChar ? gNpcDriveCandidate : null;
}

/** @param {Character} npc @param {string[]} resources @param {number} radius @param {BaseObject | null} hoard */
function ResetDefenseCandidate( npc, resources, radius, hoard )
{
	ResetCandidate();
	gNpcDriveDefender = npc;
	gNpcDriveResourceList = resources;
	gNpcDriveDefenseRadius = radius;
	gNpcDriveHoard = hoard;
	gNpcDriveMatchedResource = "";
}

/** @param {Character} npc @param {Character} target @returns {boolean} */
function NpcDriveConsiderTerritoryIntruder( npc, target )
{
	var resource = GetDefenseTargetResource( npc, target );
	if( !resource || !IsInsideHomeRadius( npc, target, gNpcDriveDefenseRadius ))
		return false;
	ConsiderDefenseCandidate( npc, target, resource );
	return true;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function NpcDriveConsiderHoardItem( npc, item )
{
	if( !ValidateObject( item ) || !item.isItem || item.container != null || item.visible > 1 || !IsInsideHomeRadius( npc, item, gNpcDriveDefenseRadius ))
		return false;
	var resource = GetFirstMatchingResource( item, gNpcDriveResourceList );
	if( !resource )
		return false;
	ConsiderDefenseCandidate( npc, item, resource );
	return true;
}

/** @param {Item} hoard @param {Character} target @returns {boolean} */
function NpcDriveConsiderHoardIntruder( hoard, target )
{
	if( !ValidateObject( gNpcDriveDefender ) || !ValidateObject( gNpcDriveHoard ) || gNpcDriveHoard.serial != hoard.serial )
		return false;
	var resource = GetDefenseTargetResource( gNpcDriveDefender, target );
	if( !resource || DistanceBetween( hoard, target, true ) > gNpcDriveDefenseRadius )
		return false;
	ConsiderDefenseCandidate( hoard, target, resource );
	return true;
}

/** @param {Character} defender @param {Character} target @returns {string} */
function GetDefenseTargetResource( defender, target )
{
	if( !ValidateObject( target ) || !target.isChar || target.serial == defender.serial || target.dead || target.visible > 2 || !target.vulnerable || target.isGM || target.isCounselor || ( !target.npc && !target.online ) || !defender.CanSee( target ))
		return "";
	return GetFirstMatchingResource( target, gNpcDriveResourceList );
}

/** @param {BaseObject} target @param {string[]} resources @returns {string} */
function GetFirstMatchingResource( target, resources )
{
	for( var i = 0; i < resources.length; ++i )
	{
		if( ObjectHasResource( target, resources[i] ))
			return resources[i];
	}
	return "";
}

/** @param {BaseObject} source @param {BaseObject} target @param {string} resource */
function ConsiderDefenseCandidate( source, target, resource )
{
	var distance = DistanceBetween( source, target, true );
	if( distance < gNpcDriveCandidateDistance )
	{
		gNpcDriveCandidateDistance = distance;
		gNpcDriveCandidate = target;
		gNpcDriveMatchedResource = resource;
	}
}

/** @param {Character} npc @param {BaseObject} target @param {number} radius @returns {boolean} */
function IsInsideHomeRadius( npc, target, radius )
{
	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	var homeWorld = parseInt( npc.GetTempTag( "npcDriveHomeWorld" ));
	var homeInstance = parseInt( npc.GetTempTag( "npcDriveHomeInstance" ));
	return target.worldnumber == homeWorld && target.instanceID == homeInstance && DistanceBetween( homeX, homeY, target.x, target.y ) <= radius;
}

/** @param {Character} npc */
function ClearDefenseState( npc )
{
	npc.SetTempTag( "npcDriveDefenseReason", null );
	npc.SetTempTag( "npcDriveDefenseTarget", null );
	npc.SetTempTag( "npcDriveDefenseTime", null );
}

/** @param {Character} npc @returns {boolean} */
function TrySocialAssist( npc )
{
	var now = GetCurrentClock();
	var nextScan = parseInt( npc.GetTempTag( "npcDriveSocialNextScan" ));
	if( !isNaN( nextScan ) && nextScan > now )
		return false;
	npc.SetTempTag( "npcDriveSocialNextScan", ( now + SOCIAL_SCAN_INTERVAL ).toString() );
	ClearSocialState( npc );

	var radius = GetBoundedTag( npc, "NpcDrivePackRadius", DEFAULT_PACK_RADIUS, 1, MAX_SEARCH_RADIUS );
	var master = GetDriveMaster( npc );
	if( IsValidSocialAnchor( npc, master ) && DistanceBetween( npc, master, true ) <= radius )
	{
		var masterTarget = GetAssistTarget( npc, master );
		if( ValidateObject( masterTarget ) && StartSocialCombat( npc, masterTarget, "master" ))
			return true;
	}

	var packId = GetPackId( npc );
	if( !packId )
		return false;
	ResetCandidate();
	gNpcDrivePackId = packId;
	AreaCharacterFunction( "NpcDriveConsiderPackAssist", npc, radius, null );
	return ValidateObject( gNpcDriveCandidate ) && StartSocialCombat( npc, gNpcDriveCandidate, "pack:" + packId );
}

/** @param {Character} npc @param {Character} packMember @returns {boolean} */
function NpcDriveConsiderPackAssist( npc, packMember )
{
	if( !ValidateObject( packMember ) || !packMember.isChar || !packMember.npc || packMember.serial == npc.serial || GetPackId( packMember ) != gNpcDrivePackId )
		return false;
	var target = GetAssistTarget( npc, packMember );
	if( !ValidateObject( target ))
		return false;
	ConsiderCandidate( npc, target );
	return true;
}

/** @param {Character} npc @param {Character} source @returns {Character | null} */
function GetAssistTarget( npc, source )
{
	if( !ValidateObject( source ) || ( !source.npc && !source.online ) || source.dead || ( !source.atWar && !ValidateObject( source.attacker )))
		return null;
	var target = ValidateObject( source.target ) ? source.target : source.attacker;
	if( !ValidateObject( target ) || !target.isChar || target.serial == npc.serial || target.dead || target.visible > 2 || !target.vulnerable || target.isGM || target.isCounselor || ( !target.npc && !target.online ) || target.worldnumber != npc.worldnumber || target.instanceID != npc.instanceID || !npc.CanSee( target ) || IsSocialAlly( npc, target ))
		return null;
	return target;
}

/** @param {Character} npc @param {Character} target @returns {boolean} */
function IsSocialAlly( npc, target )
{
	var master = GetDriveMaster( npc );
	if( ValidateObject( master ) && master.serial == target.serial )
		return true;
	var packId = GetPackId( npc );
	return !!packId && target.npc && GetPackId( target ) == packId;
}

/** @param {Character} npc @param {Character} target @param {string} reason @returns {boolean} */
function StartSocialCombat( npc, target, reason )
{
	if( !npc.InitiateCombat( target ))
		return false;
	if( GetDriveMode( npc ))
		ClearDriveGoal( npc, true, 0 );
	npc.SetTempTag( "npcDriveSocialReason", reason );
	npc.SetTempTag( "npcDriveSocialTarget", target.serial.toString() );
	npc.SetTempTag( "npcDriveSocialTime", GetCurrentClock().toString() );
	return true;
}

/** @param {Character} npc @returns {boolean} */
function TrySocialRegroup( npc )
{
	var anchor = GetDriveMaster( npc );
	var reason = "master";
	if( !IsValidSocialAnchor( npc, anchor ))
	{
		anchor = FindPackLeader( npc );
		reason = "pack";
	}
	if( !IsValidSocialAnchor( npc, anchor ) || !IsInsideLeash( npc, anchor ))
		return false;

	var followDistance = GetBoundedTag( npc, "NpcDrivePackFollowDistance", DEFAULT_PACK_FOLLOW_DISTANCE, 1, 12 );
	if( DistanceBetween( npc, anchor, true ) <= followDistance )
		return false;

	RememberOriginalWander( npc );
	npc.SetTempTag( "npcDriveMode", "social" );
	npc.SetTempTag( "npcDriveTarget", anchor.serial.toString() );
	npc.SetTempTag( "npcDriveTargetType", "char" );
	npc.SetTempTag( "npcDriveReason", reason + ":regroup" );
	npc.WalkTo( anchor, Math.max( 8, Math.ceil( DistanceBetween( npc, anchor, true ) * 2 )));
	return true;
}

/** @param {Character} npc @returns {Character | null} */
function FindPackLeader( npc )
{
	var packId = GetPackId( npc );
	if( !packId || GetPackRole( npc ) == "leader" )
		return null;
	ResetCandidate();
	gNpcDrivePackId = packId;
	var radius = GetBoundedTag( npc, "NpcDrivePackRadius", DEFAULT_PACK_RADIUS, 1, MAX_SEARCH_RADIUS );
	AreaCharacterFunction( "NpcDriveConsiderPackLeader", npc, radius, null );
	return ValidateObject( gNpcDriveCandidate ) && gNpcDriveCandidate.isChar ? gNpcDriveCandidate : null;
}

/** @param {Character} npc @param {Character} candidate @returns {boolean} */
function NpcDriveConsiderPackLeader( npc, candidate )
{
	if( !IsValidSocialAnchor( npc, candidate ) || !candidate.npc || GetPackId( candidate ) != gNpcDrivePackId || GetPackRole( candidate ) != "leader" || !IsInsideLeash( npc, candidate ))
		return false;
	ConsiderCandidate( npc, candidate );
	return true;
}

/** @param {Character} npc @returns {boolean} */
function HandleActiveSocial( npc )
{
	var anchor = GetDriveTarget( npc );
	if( DriveDisabled( npc, "Social" ) || !IsValidSocialAnchor( npc, anchor ) || !IsInsideLeash( npc, anchor ))
	{
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return false;
	}
	var followDistance = GetBoundedTag( npc, "NpcDrivePackFollowDistance", DEFAULT_PACK_FOLLOW_DISTANCE, 1, 12 );
	if( DistanceBetween( npc, anchor, true ) <= followDistance )
	{
		ClearDriveGoal( npc, true, THINK_INTERVAL );
		return false;
	}
	return true;
}

/** @param {Character} npc @param {Character | null} anchor @returns {boolean} */
function IsValidSocialAnchor( npc, anchor )
{
	return ValidateObject( anchor ) && anchor.isChar && anchor.serial != npc.serial && !anchor.dead && ( anchor.npc || anchor.online ) && anchor.worldnumber == npc.worldnumber && anchor.instanceID == npc.instanceID && npc.CanSee( anchor );
}

/** @param {Character} npc @returns {Character | null} */
function GetDriveMaster( npc )
{
	var serial = ParseStoredSerial( npc.GetTag( "NpcDriveMasterSerial" ));
	if( isNaN( serial ))
		return null;
	var master = CalcCharFromSer( serial );
	return ValidateObject( master ) ? master : null;
}

/** @param {Character} npc @returns {string} */
function GetPackId( npc )
{
	var packId = npc.GetTag( "NpcDrivePackId" );
	if( !packId )
		return "";
	packId = packId.toString().trim().toLowerCase();
	return packId.length <= 32 && /^[a-z0-9_-]+$/.test( packId ) ? packId : "";
}

/** @param {Character} npc @returns {string} */
function GetPackRole( npc )
{
	var role = npc.GetTag( "NpcDrivePackRole" );
	role = role ? role.toString().toLowerCase() : "member";
	return role == "leader" ? "leader" : "member";
}

/** @param {Character} npc */
function ClearSocialState( npc )
{
	npc.SetTempTag( "npcDriveSocialReason", null );
	npc.SetTempTag( "npcDriveSocialTarget", null );
	npc.SetTempTag( "npcDriveSocialTime", null );
}

/** @param {Character} npc @param {{ resource: string, action: string, cooldown: number }} entry @param {BaseObject} target @returns {boolean} */
function StartDesire( npc, entry, target )
{
	var action = entry.action;
	if( action == "pickup" && ( !target.isItem || !CanCarryDesireItem( npc, target )))
		action = "nearby";

	RememberOriginalWander( npc );
	npc.SetTempTag( "npcDriveMode", "desire" );
	npc.SetTempTag( "npcDriveTarget", target.serial.toString() );
	npc.SetTempTag( "npcDriveTargetType", target.isChar ? "char" : "item" );
	npc.SetTempTag( "npcDriveDesireResource", entry.resource );
	npc.SetTempTag( "npcDriveDesireAction", action );
	npc.SetTempTag( "npcDriveDesireCooldown", entry.cooldown );
	npc.SetTempTag( "npcDriveReason", entry.resource + ":" + action );

	if( action == "pickup" )
		target.SetTempTag( "npcDriveReservedBy", npc.serial.toString() );

	if( action == "follow" )
	{
		BeginDesireFollow( npc, target );
		return true;
	}

	var desiredRange = action == "pickup" ? 1 : GetBoundedTag( npc, "NpcDriveDesireStayRange", DEFAULT_DESIRE_STAY_RANGE, 1, 8 );
	if( npc.InRange( target, desiredRange ))
	{
		if( action == "pickup" && TakeDesireItem( npc, target ))
			CompletePickedDesire( npc, target );
		else
			BeginDesireLinger( npc );
		return true;
	}

	npc.WalkTo( target, GetPathMaxSteps() );
	return true;
}

/** @param {Character} npc @param {{ resource: string, radius: number }} entry @param {BaseObject} target @returns {boolean} */
function StartAversion( npc, entry, target )
{
	RememberOriginalWander( npc );
	npc.SetTempTag( "npcDriveMode", "aversion" );
	npc.SetTempTag( "npcDriveTarget", target.serial.toString() );
	npc.SetTempTag( "npcDriveTargetType", target.isChar ? "char" : "item" );
	npc.SetTempTag( "npcDriveDesireResource", entry.resource );
	npc.SetTempTag( "npcDriveAversionRadius", entry.radius );
	npc.SetTempTag( "npcDriveReason", entry.resource + ":avoid" );
	RunFromAversion( npc, target, entry.radius );
	return true;
}

/** @param {Character} npc @returns {boolean} */
function HandleActiveDesire( npc )
{
	if( DriveDisabled( npc, "Desire" ))
	{
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return false;
	}

	var target = GetDriveTarget( npc );
	var resource = npc.GetTempTag( "npcDriveDesireResource" ).toString();
	var action = npc.GetTempTag( "npcDriveDesireAction" ).toString();
	if( !IsResourceTarget( npc, target, resource, action, false ) || !IsInsideLeash( npc, target ))
	{
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return false;
	}

	if( action == "follow" )
	{
		var followEnd = parseInt( npc.GetTempTag( "npcDriveDesireEnd" ));
		var searchRadius = GetBoundedTag( npc, "NpcDriveDesireSearchRadius", DEFAULT_DESIRE_RADIUS, 1, MAX_SEARCH_RADIUS );
		if( !isNaN( followEnd ) && GetCurrentClock() >= followEnd )
		{
			SatisfyDesire( npc );
			return false;
		}
		if( DistanceBetween( npc, target, true ) > searchRadius * 2 )
		{
			ClearDriveGoal( npc, true, RETRY_INTERVAL );
			return false;
		}
		if( npc.wandertype != WT_FOLLOW )
			npc.Follow( target );
		return false;
	}

	if( action == "pickup" )
	{
		if( npc.InRange( target, 1 ))
		{
			if( TakeDesireItem( npc, target ))
				CompletePickedDesire( npc, target );
			else
				ClearDriveGoal( npc, true, RETRY_INTERVAL );
		}
		return true;
	}

	var stayRange = GetBoundedTag( npc, "NpcDriveDesireStayRange", DEFAULT_DESIRE_STAY_RANGE, 1, 8 );
	if( parseInt( npc.GetTempTag( "npcDriveDesireArrived" )) == 1 )
	{
		if( !npc.InRange( target, stayRange ))
		{
			npc.SetTempTag( "npcDriveDesireArrived", null );
			npc.WalkTo( target, Math.max( 8, stayRange * 4 ));
			return true;
		}
		var lingerEnd = parseInt( npc.GetTempTag( "npcDriveDesireEnd" ));
		if( !isNaN( lingerEnd ) && GetCurrentClock() >= lingerEnd )
		{
			SatisfyDesire( npc );
			return false;
		}
		npc.wandertype = WT_NONE;
		return false;
	}
	return true;
}

/** @param {Character} npc @returns {boolean} */
function HandleActiveAversion( npc )
{
	if( DriveDisabled( npc, "Desire" ))
	{
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return false;
	}

	var target = GetDriveTarget( npc );
	var resource = npc.GetTempTag( "npcDriveDesireResource" ).toString();
	var safeRadius = parseInt( npc.GetTempTag( "npcDriveAversionRadius" ));
	if( isNaN( safeRadius ))
		safeRadius = DEFAULT_DESIRE_RADIUS;
	if( !IsResourceTarget( npc, target, resource, "nearby", true ))
	{
		ClearDriveGoal( npc, true, THINK_INTERVAL );
		return false;
	}
	if( DistanceBetween( npc, target, true ) >= safeRadius )
	{
		ClearDriveGoal( npc, true, THINK_INTERVAL );
		return false;
	}

	var nextRepath = parseInt( npc.GetTempTag( "npcDriveAversionRepath" ));
	if( isNaN( nextRepath ) || nextRepath <= GetCurrentClock() )
		RunFromAversion( npc, target, safeRadius );
	return true;
}

/** @param {Character} npc @param {number} pathfindResult */
function HandleDesirePathEnd( npc, pathfindResult )
{
	var target = GetDriveTarget( npc );
	var action = npc.GetTempTag( "npcDriveDesireAction" ).toString();
	var resource = npc.GetTempTag( "npcDriveDesireResource" ).toString();
	if( pathfindResult < 0 || !IsResourceTarget( npc, target, resource, action, false ))
	{
		ClearDriveGoal( npc, false, RETRY_INTERVAL );
		return;
	}

	if( action == "pickup" )
	{
		if( npc.InRange( target, 1 ) && TakeDesireItem( npc, target ))
			CompletePickedDesire( npc, target );
		else
			ClearDriveGoal( npc, false, RETRY_INTERVAL );
		return;
	}

	var stayRange = GetBoundedTag( npc, "NpcDriveDesireStayRange", DEFAULT_DESIRE_STAY_RANGE, 1, 8 );
	if( npc.InRange( target, stayRange ))
		BeginDesireLinger( npc );
	else
		ClearDriveGoal( npc, false, RETRY_INTERVAL );
}

/** @param {Character} npc */
function HandleAversionPathEnd( npc )
{
	var target = GetDriveTarget( npc );
	var resource = npc.GetTempTag( "npcDriveDesireResource" ).toString();
	var safeRadius = parseInt( npc.GetTempTag( "npcDriveAversionRadius" ));
	if( !IsResourceTarget( npc, target, resource, "nearby", true ) || isNaN( safeRadius ) || DistanceBetween( npc, target, true ) >= safeRadius )
	{
		ClearDriveGoal( npc, false, THINK_INTERVAL );
		return;
	}
	RunFromAversion( npc, target, safeRadius );
}

/** @param {Character} npc @param {Character} target */
function BeginDesireFollow( npc, target )
{
	var duration = GetBoundedTag( npc, "NpcDriveDesireDuration", DEFAULT_DESIRE_DURATION, 1, 300 );
	npc.SetTempTag( "npcDriveDesireEnd", ( GetCurrentClock() + duration * 1000 ).toString() );
	npc.Follow( target );
}

/** @param {Character} npc */
function BeginDesireLinger( npc )
{
	var duration = GetBoundedTag( npc, "NpcDriveDesireDuration", DEFAULT_DESIRE_DURATION, 1, 300 );
	npc.SetTempTag( "npcDriveDesireArrived", 1 );
	npc.SetTempTag( "npcDriveDesireEnd", ( GetCurrentClock() + duration * 1000 ).toString() );
	npc.oldWandertype = WT_NONE;
	npc.wandertype = WT_NONE;
}

/** @param {Character} npc */
function SatisfyDesire( npc )
{
	MarkDesireSatisfied( npc );
	ClearDriveGoal( npc, true, THINK_INTERVAL );
}

/** @param {Character} npc */
function MarkDesireSatisfied( npc )
{
	var resource = npc.GetTempTag( "npcDriveDesireResource" ).toString();
	var cooldown = parseInt( npc.GetTempTag( "npcDriveDesireCooldown" ));
	if( !isNaN( cooldown ) && cooldown > 0 )
		npc.SetTempTag( GetDesireCooldownTag( resource ), ( GetCurrentClock() + cooldown * 1000 ).toString() );
}

/** @param {Character} npc @param {Item} item */
function CompletePickedDesire( npc, item )
{
	var resource = npc.GetTempTag( "npcDriveDesireResource" ).toString();
	MarkDesireSatisfied( npc );
	if( !DriveDisabled( npc, "Return" ) && ShouldReturnResource( npc, resource ))
	{
		BeginReturnHome( npc, item, resource );
		return;
	}
	ClearDriveGoal( npc, true, THINK_INTERVAL );
}

/** @param {Character} npc @param {string} resource @returns {boolean} */
function IsDesireReady( npc, resource )
{
	var tagName = GetDesireCooldownTag( resource );
	var until = parseInt( npc.GetTempTag( tagName ));
	if( isNaN( until ) || until <= GetCurrentClock() )
	{
		if( !isNaN( until ))
			npc.SetTempTag( tagName, null );
		return true;
	}
	return false;
}

/** @param {string} resource @returns {string} */
function GetDesireCooldownTag( resource )
{
	return "npcDriveSatisfied_" + resource.replace( /[^a-z0-9_]/g, "_" );
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function CanCarryDesireItem( npc, item )
{
	if( !ValidateObject( item ) || !item.isItem || item.container != null || !IsPickupMovable( item ) || item.visible > 1 )
		return false;
	var pack = npc.pack;
	if( !ValidateObject( pack ))
		return false;
	if( pack.maxItems > 0 && pack.itemsinside >= pack.maxItems )
		return false;
	var stackWeight = item.weight * Math.max( 1, item.amount );
	return pack.weightMax <= 0 || pack.weight + stackWeight <= pack.weightMax;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function TakeDesireItem( npc, item )
{
	if( !npc.InRange( item, 1 ) || !CanCarryDesireItem( npc, item ))
		return false;
	item.container = npc.pack;
	item.Refresh();
	return true;
}

/** @param {Character} npc @param {string} resource @returns {boolean} */
function ShouldReturnResource( npc, resource )
{
	var resources = ParseResourceList( npc.GetTag( "NpcDriveReturnResources" ));
	return resources.indexOf( "all" ) != -1 || resources.indexOf( resource ) != -1;
}

/** @param {Character} npc @returns {boolean} */
function TryPendingReturn( npc )
{
	var item = GetPendingReturnItem( npc );
	if( !ValidateObject( item ) || !ValidateObject( npc.pack ) || !ValidateObject( item.container ) || item.container.serial != npc.pack.serial )
	{
		ClearPendingReturn( npc );
		return false;
	}
	BeginReturnHome( npc, item, npc.GetTempTag( "npcDriveReturnResource" ).toString() );
	return true;
}

/** @param {Character} npc @param {Item} item @param {string} resource */
function BeginReturnHome( npc, item, resource )
{
	RememberOriginalWander( npc );
	var previousPending = ParseStoredSerial( npc.GetTempTag( "npcDrivePendingReturnItem" ));
	if( isNaN( previousPending ) || previousPending != item.serial )
		npc.SetTempTag( "npcDriveReturnFailures", null );
	npc.SetTempTag( "npcDrivePendingReturnItem", item.serial.toString() );
	npc.SetTempTag( "npcDriveReturnResource", resource );
	npc.SetTempTag( "npcDriveMode", "return" );
	npc.SetTempTag( "npcDriveReason", "return:" + resource );
	npc.SetTempTag( "npcDriveTarget", null );
	npc.SetTempTag( "npcDriveTargetType", null );
	var container = GetHoardContainer( npc );
	if( ValidateObject( container ))
	{
		npc.SetTempTag( "npcDriveTarget", container.serial.toString() );
		npc.SetTempTag( "npcDriveTargetType", "item" );
		if( npc.InRange( container, 1 ))
		{
			FinishReturnHome( npc );
			return;
		}
		// Containers usually occupy an impassable tile. Allow a partial path so
		// pathfinding can stop on an adjacent tile, which is close enough to deposit.
		npc.WalkTo( container, Math.min( MAX_LEASH_RADIUS * 2, Math.max( 8, Math.ceil( DistanceBetween( npc, container, true ) * 2 ))), true );
		return;
	}

	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	var homeZ = parseInt( npc.GetTempTag( "npcDriveHomeZ" ));
	if( DistanceBetween( npc.x, npc.y, homeX, homeY ) <= 1 )
	{
		FinishReturnHome( npc );
		return;
	}
	npc.WalkTo( homeX, homeY, isNaN( homeZ ) ? npc.z : homeZ, Math.min( MAX_LEASH_RADIUS * 2, Math.max( 8, Math.ceil( DistanceBetween( npc.x, npc.y, homeX, homeY ) * 2 ))));
}

/** @param {Character} npc @returns {boolean} */
function HandleActiveReturn( npc )
{
	var item = GetPendingReturnItem( npc );
	if( DriveDisabled( npc, "Return" ) || !ValidateObject( item ) || !ValidateObject( npc.pack ) || !ValidateObject( item.container ) || item.container.serial != npc.pack.serial )
	{
		ClearPendingReturn( npc );
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return false;
	}
	var container = GetHoardContainer( npc );
	if( ValidateObject( container ) && npc.InRange( container, 1 ))
	{
		FinishReturnHome( npc );
		return false;
	}
	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	if( !ValidateObject( container ) && DistanceBetween( npc.x, npc.y, homeX, homeY ) <= 1 )
	{
		FinishReturnHome( npc );
		return false;
	}
	return true;
}

/** @param {Character} npc @param {number} pathfindResult */
function HandleReturnPathEnd( npc, pathfindResult )
{
	var container = GetHoardContainer( npc );
	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	if(( ValidateObject( container ) && npc.InRange( container, 1 )) || ( !ValidateObject( container ) && DistanceBetween( npc.x, npc.y, homeX, homeY ) <= 1 ))
	{
		FinishReturnHome( npc );
		return;
	}

	var failures = parseInt( npc.GetTempTag( "npcDriveReturnFailures" ));
	if( isNaN( failures )) failures = 0;
	failures++;
	if( failures >= 3 )
	{
		// Do not let one unreachable chest permanently block every later desire.
		ClearPendingReturn( npc );
		ClearDriveGoal( npc, true, RETRY_INTERVAL );
		return;
	}
	npc.SetTempTag( "npcDriveReturnFailures", failures );
	ClearDriveGoal( npc, false, RETRY_INTERVAL );
}

/** @param {Character} npc */
function FinishReturnHome( npc )
{
	var item = GetPendingReturnItem( npc );
	var container = GetHoardContainer( npc );
	if( ValidateObject( item ) && ValidateObject( container ) && npc.InRange( container, 1 ) && CanDepositReturnItem( container, item ))
	{
		item.container = container;
		item.Refresh();
	}
	ClearPendingReturn( npc );
	ClearDriveGoal( npc, true, THINK_INTERVAL );
}

/** @param {Item} container @param {Item} item @returns {boolean} */
function CanDepositReturnItem( container, item )
{
	if( container.maxItems > 0 && container.itemsinside >= container.maxItems )
		return false;
	var stackWeight = item.weight * Math.max( 1, item.amount );
	return container.weightMax <= 0 || container.weight + stackWeight <= container.weightMax;
}

/** @param {Character} npc @returns {Item | null} */
function GetHoardContainer( npc )
{
	var serial = ParseStoredSerial( npc.GetTag( "NpcDriveHoardContainerSerial" ));
	if( isNaN( serial ))
		return null;
	var container = CalcItemFromSer( serial );
	return ValidateObject( container ) && container.isItem && container.type == 1 && container.container == null && IsInsideLeash( npc, container ) ? container : null;
}

/** @param {Character} npc @returns {Item | null} */
function GetPendingReturnItem( npc )
{
	var serial = ParseStoredSerial( npc.GetTempTag( "npcDrivePendingReturnItem" ));
	if( isNaN( serial ))
		return null;
	var item = CalcItemFromSer( serial );
	return ValidateObject( item ) ? item : null;
}

/** @param {Character} npc */
function ClearPendingReturn( npc )
{
	npc.SetTempTag( "npcDrivePendingReturnItem", null );
	npc.SetTempTag( "npcDriveReturnResource", null );
	npc.SetTempTag( "npcDriveReturnFailures", null );
}

/** @param {Character} npc @returns {boolean} */
function TryBoredom( npc )
{
	var activities = ParseIdleActivities( npc.GetTag( "NpcDriveIdleActivities" ));
	if( activities.length == 0 )
		return false;
	var now = GetCurrentClock();
	var nextActivity = parseInt( npc.GetTempTag( "npcDriveBoredomNext" ));
	if( !isNaN( nextActivity ) && nextActivity > now )
		return false;
	var activity = activities[RandomNumber( 0, activities.length - 1 )];
	return StartIdleActivity( npc, activity );
}

/** @param {Character} npc @param {string} activity @returns {boolean} */
function StartIdleActivity( npc, activity )
{
	RememberOriginalWander( npc );
	var duration = GetBoundedTag( npc, "NpcDriveIdleDuration", DEFAULT_IDLE_DURATION, 1, 300 );
	npc.SetTempTag( "npcDriveMode", "idle" );
	npc.SetTempTag( "npcDriveReason", "boredom:" + activity );
	npc.SetTempTag( "npcDriveIdleEnd", ( GetCurrentClock() + duration * 1000 ).toString() );

	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	var homeZ = parseInt( npc.GetTempTag( "npcDriveHomeZ" ));
	var homeWorld = parseInt( npc.GetTempTag( "npcDriveHomeWorld" ));
	var homeInstance = parseInt( npc.GetTempTag( "npcDriveHomeInstance" ));
	if( activity == "home" && npc.worldnumber == homeWorld && npc.instanceID == homeInstance && DistanceBetween( npc.x, npc.y, homeX, homeY ) > 1 )
	{
		npc.WalkTo( homeX, homeY, isNaN( homeZ ) ? npc.z : homeZ, Math.min( MAX_LEASH_RADIUS * 2, Math.max( 8, Math.ceil( DistanceBetween( npc.x, npc.y, homeX, homeY ) * 2 ))));
		return true;
	}

	if( activity == "roam" && npc.worldnumber == homeWorld && npc.instanceID == homeInstance )
	{
		var leash = GetBoundedTag( npc, "NpcDriveLeashRadius", DEFAULT_LEASH_RADIUS, 1, MAX_LEASH_RADIUS );
		var radius = Math.min( leash, GetBoundedTag( npc, "NpcDriveIdleRadius", DEFAULT_IDLE_RADIUS, 1, MAX_SEARCH_RADIUS ));
		var destX = Math.max( 0, Math.min( 65535, homeX + RandomNumber( -radius, radius )));
		var destY = Math.max( 0, Math.min( 65535, homeY + RandomNumber( -radius, radius )));
		if( destX != npc.x || destY != npc.y )
		{
			npc.WalkTo( destX, destY, npc.z, Math.max( 8, radius * 3 ));
			return true;
		}
	}

	// Pause is also the safe fallback when a home/roam destination cannot be used.
	npc.SetTempTag( "npcDriveReason", "boredom:pause" );
	npc.oldWandertype = WT_NONE;
	npc.wandertype = WT_NONE;
	return true;
}

/** @param {Character} npc @returns {boolean} */
function HandleActiveIdle( npc )
{
	if( DriveDisabled( npc, "Boredom" ))
	{
		FinishIdleActivity( npc, RETRY_INTERVAL );
		return false;
	}
	var endTime = parseInt( npc.GetTempTag( "npcDriveIdleEnd" ));
	if( !isNaN( endTime ) && endTime <= GetCurrentClock() )
	{
		FinishIdleActivity( npc, THINK_INTERVAL );
		return false;
	}
	if( npc.GetTempTag( "npcDriveReason" ).toString() == "boredom:pause" )
	{
		npc.wandertype = WT_NONE;
		return false;
	}
	return true;
}

/** @param {Character} npc @param {number} retryDelay */
function FinishIdleActivity( npc, retryDelay )
{
	ScheduleNextBoredom( npc );
	ClearDriveGoal( npc, true, retryDelay );
}

/** @param {Character} npc */
function ScheduleNextBoredom( npc )
{
	var delay = GetBoundedTag( npc, "NpcDriveBoredomDelay", DEFAULT_BOREDOM_DELAY, 10, 3600 );
	npc.SetTempTag( "npcDriveBoredomNext", ( GetCurrentClock() + delay * 1000 ).toString() );
}

/** @param {string | number | boolean} rawValue @returns {string[]} */
function ParseIdleActivities( rawValue )
{
	var values = ParseResourceList( rawValue );
	var activities = [];
	for( var i = 0; i < values.length; ++i )
	{
		if(( values[i] == "home" || values[i] == "roam" || values[i] == "pause" ) && activities.indexOf( values[i] ) == -1 )
			activities.push( values[i] );
	}
	return activities;
}

/** @param {Character} npc @param {BaseObject} target @param {number} safeRadius */
function RunFromAversion( npc, target, safeRadius )
{
	var deltaX = npc.x - target.x;
	var deltaY = npc.y - target.y;
	if( deltaX == 0 && deltaY == 0 )
		deltaX = RandomNumber( 0, 1 ) == 0 ? -1 : 1;
	var distance = Math.max( 1, Math.max( Math.abs( deltaX ), Math.abs( deltaY )));
	var escapeDistance = Math.max( 4, safeRadius - distance + 4 );
	var destX = Math.round( npc.x + ( deltaX / distance ) * escapeDistance );
	var destY = Math.round( npc.y + ( deltaY / distance ) * escapeDistance );
	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	var leash = GetBoundedTag( npc, "NpcDriveLeashRadius", DEFAULT_LEASH_RADIUS, 1, MAX_LEASH_RADIUS );
	destX = Math.max( homeX - leash, Math.min( homeX + leash, destX ));
	destY = Math.max( homeY - leash, Math.min( homeY + leash, destY ));
	destX = Math.max( 0, Math.min( 65535, destX ));
	destY = Math.max( 0, Math.min( 65535, destY ));

	var currentTargetDistance = DistanceBetween( npc.x, npc.y, target.x, target.y );
	var bestTargetDistance = DistanceBetween( destX, destY, target.x, target.y );
	if( bestTargetDistance <= currentTargetDistance )
	{
		var directions = [[ -1, 0 ], [ 1, 0 ], [ 0, -1 ], [ 0, 1 ], [ -1, -1 ], [ -1, 1 ], [ 1, -1 ], [ 1, 1 ]];
		for( var i = 0; i < directions.length; ++i )
		{
			var candidateX = npc.x + directions[i][0] * escapeDistance;
			var candidateY = npc.y + directions[i][1] * escapeDistance;
			candidateX = Math.max( 0, Math.min( 65535, Math.max( homeX - leash, Math.min( homeX + leash, candidateX ))));
			candidateY = Math.max( 0, Math.min( 65535, Math.max( homeY - leash, Math.min( homeY + leash, candidateY ))));
			var candidateDistance = DistanceBetween( candidateX, candidateY, target.x, target.y );
			if( candidateDistance > bestTargetDistance )
			{
				bestTargetDistance = candidateDistance;
				destX = candidateX;
				destY = candidateY;
			}
		}
	}
	npc.SetTempTag( "npcDriveAversionRepath", ( GetCurrentClock() + AVERSION_REPATH_INTERVAL ).toString() );
	npc.RunTo( destX, destY, npc.z, Math.max( 12, escapeDistance * 3 ), true, true );
}

/** @param {Character} npc @param {string} resource @param {string} action @param {number} radius @param {boolean} findingAversion @returns {BaseObject | null} */
function FindResourceCandidate( npc, resource, action, radius, findingAversion )
{
	ResetCandidate();
	gNpcDriveResource = resource;
	gNpcDriveCandidateAction = action;
	gNpcDriveFindingAversion = findingAversion;
	if( action != "follow" )
		AreaItemFunction( "NpcDriveConsiderResourceItem", npc, radius, null );
	if( action != "pickup" )
		AreaCharacterFunction( "NpcDriveConsiderResourceCharacter", npc, radius, null );
	return gNpcDriveCandidate;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function NpcDriveConsiderResourceItem( npc, item )
{
	if( !IsResourceTarget( npc, item, gNpcDriveResource, gNpcDriveCandidateAction, gNpcDriveFindingAversion ) || ( !gNpcDriveFindingAversion && !IsInsideLeash( npc, item )))
		return false;
	if( gNpcDriveCandidateAction == "pickup" && !ReservationAvailable( npc, item ))
		return false;
	ConsiderCandidate( npc, item );
	return true;
}

/** @param {Character} npc @param {Character} target @returns {boolean} */
function NpcDriveConsiderResourceCharacter( npc, target )
{
	if( !IsResourceTarget( npc, target, gNpcDriveResource, gNpcDriveCandidateAction, gNpcDriveFindingAversion ) || ( !gNpcDriveFindingAversion && !IsInsideLeash( npc, target )))
		return false;
	ConsiderCandidate( npc, target );
	return true;
}

/** @param {Character} npc @param {BaseObject | null} target @param {string} resource @param {string} action @param {boolean} findingAversion @returns {boolean} */
function IsResourceTarget( npc, target, resource, action, findingAversion )
{
	if( !ValidateObject( target ) || target.serial == npc.serial || !ObjectHasResource( target, resource ))
		return false;
	if( target.isItem )
	{
		if( target.container != null || target.visible > 1 )
			return false;
		if( action == "pickup" && !IsPickupMovable( target ))
			return false;
	}
	else if( target.isChar )
	{
		if( target.dead || ( !target.npc && !target.online ))
			return false;
		if( action == "pickup" )
			return false;
	}
	else
		return false;
	return findingAversion || action == "nearby" || action == "follow" || action == "pickup";
}

/** @param {BaseObject} target @param {string} resource @returns {boolean} */
function ObjectHasResource( target, resource )
{
	var resources = GetObjectResources( target );
	return resources.indexOf( resource ) != -1;
}

/** @param {BaseObject} target @returns {string[]} */
function GetObjectResources( target )
{
	var resources = [];
	var rawResources = target.GetTag( "NpcDriveResource" );
	if( rawResources )
	{
		var parts = rawResources.toString().toLowerCase().split( /[,;|]/ );
		for( var i = 0; i < parts.length; ++i )
		{
			var part = parts[i].trim();
			if( part && resources.indexOf( part ) == -1 )
				resources.push( part );
		}
	}
	if( target.isItem )
	{
		resources.push( "item" );
		if( target.id == 0x0eed ) resources.push( "gold" );
	}
	else if( target.isChar )
	{
		resources.push( "character" );
		resources.push( target.npc ? "npc" : "player" );
		if( target.isHuman ) resources.push( "human" );
		if( target.aitype == AI_ANIMAL || target.aitype == AI_ANIMAL_SCARED ) resources.push( "animal" );
	}
	return resources;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function ReservationAvailable( npc, item )
{
	var reservation = item.GetTempTag( "npcDriveReservedBy" );
	if( !reservation || reservation.toString() == npc.serial.toString() )
		return true;
	var reservingNpc = CalcCharFromSer( parseInt( reservation ));
	if( ValidateObject( reservingNpc ) && ( GetDriveMode( reservingNpc ) == "food" || GetDriveMode( reservingNpc ) == "desire" ) && reservingNpc.GetTempTag( "npcDriveTarget" ).toString() == item.serial.toString() )
		return false;
	item.SetTempTag( "npcDriveReservedBy", null );
	return true;
}

/** @param {string | number | boolean} rawValue @returns {{ resource: string, action: string, cooldown: number }[]} */
function ParseDesireEntries( rawValue )
{
	var entries = [];
	if( !rawValue )
		return entries;
	var definitions = rawValue.toString().toLowerCase().split( "," );
	for( var i = 0; i < definitions.length; ++i )
	{
		var parts = definitions[i].split( ":" );
		var resource = parts[0].trim();
		var action = parts.length > 1 ? parts[1].trim() : "nearby";
		var cooldown = parts.length > 2 ? parseInt( parts[2] ) : 0;
		if( resource.length > 32 || !/^[a-z0-9_-]+$/.test( resource ) || ( action != "pickup" && action != "nearby" && action != "follow" ))
			continue;
		if( isNaN( cooldown )) cooldown = 0;
		entries.push({ resource: resource, action: action, cooldown: Math.max( 0, Math.min( 86400, cooldown )) });
	}
	return entries;
}

/** @param {string | number | boolean} rawValue @returns {{ resource: string, radius: number }[]} */
function ParseAversionEntries( rawValue )
{
	var entries = [];
	if( !rawValue )
		return entries;
	var definitions = rawValue.toString().toLowerCase().split( "," );
	for( var i = 0; i < definitions.length; ++i )
	{
		var parts = definitions[i].split( ":" );
		var resource = parts[0].trim();
		var radius = parts.length > 1 ? parseInt( parts[1] ) : 8;
		if( resource.length > 32 || !/^[a-z0-9_-]+$/.test( resource ))
			continue;
		if( isNaN( radius )) radius = 8;
		entries.push({ resource: resource, radius: Math.max( 1, Math.min( MAX_SEARCH_RADIUS, radius )) });
	}
	return entries;
}

/** @param {string | number | boolean} rawValue @returns {string[]} */
function ParseResourceList( rawValue )
{
	var resources = [];
	if( !rawValue )
		return resources;
	var definitions = rawValue.toString().toLowerCase().split( /[,;|]/ );
	for( var i = 0; i < definitions.length; ++i )
	{
		var resource = definitions[i].trim();
		if( resource.length <= 32 && /^[a-z0-9_-]+$/.test( resource ) && resources.indexOf( resource ) == -1 )
			resources.push( resource );
	}
	return resources;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function NpcDriveConsiderFood( npc, item )
{
	if( !IsUsableFood( npc, item ) || !IsInsideLeash( npc, item ))
		return false;

	var reservation = item.GetTempTag( "npcDriveReservedBy" );
	if( reservation && reservation.toString() != npc.serial.toString() )
	{
		var reservingNpc = CalcCharFromSer( parseInt( reservation ));
		if( ValidateObject( reservingNpc ) && GetDriveMode( reservingNpc ) == "food" && reservingNpc.GetTempTag( "npcDriveTarget" ).toString() == item.serial.toString() )
			return false;
		item.SetTempTag( "npcDriveReservedBy", null );
	}

	ConsiderCandidate( npc, item );
	return true;
}

/** @param {Character} npc @param {Item} item @returns {boolean} */
function NpcDriveConsiderShelter( npc, item )
{
	if( !IsShelterMarker( item ) || !IsInsideLeash( npc, item ))
		return false;
	ConsiderCandidate( npc, item );
	return true;
}

/** @param {Character} npc @param {BaseObject} item */
function ConsiderCandidate( npc, item )
{
	var distance = DistanceBetween( npc, item, true );
	if( distance < gNpcDriveCandidateDistance )
	{
		gNpcDriveCandidateDistance = distance;
		gNpcDriveCandidate = item;
	}
}

function ResetCandidate()
{
	gNpcDriveCandidate = null;
	gNpcDriveCandidateDistance = Number.MAX_VALUE;
}

/** @returns {number} */
function GetPathMaxSteps()
{
	return Math.max( 8, Math.min( MAX_LEASH_RADIUS * 2, Math.ceil( gNpcDriveCandidateDistance * 2 )));
}

/** @param {Character} npc @returns {boolean} */
function IsEligibleNpc( npc )
{
	if( npc.dead || npc.tamed || npc.mounted || npc.stabled || npc.frozen || npc.wandertype == WT_FROZEN )
		return false;

	var profile = GetDriveProfile( npc );
	if( profile == "none" || profile == "off" )
		return false;

	return ( profile == "animal" || profile == "humanoid" || profile == "ambient" || npc.aitype == AI_ANIMAL || npc.aitype == AI_ANIMAL_SCARED );
}

/** @param {Character} npc @returns {string} */
function GetDriveProfile( npc )
{
	var profile = npc.GetTag( "NpcDriveProfile" );
	return profile ? profile.toString().toLowerCase() : "";
}

/** @param {Character} npc @param {string} driveName @returns {boolean} */
function DriveDisabled( npc, driveName )
{
	return parseInt( npc.GetTag( "NpcDriveDisable" + driveName )) == 1;
}

/** @param {Character} npc @returns {boolean} */
function CanSeekFood( npc )
{
	return npc.willhunger && !!npc.foodList;
}

/** @param {Character} npc @param {Item | null} item @returns {boolean} */
function IsUsableFood( npc, item )
{
	return ValidateObject( item ) && item.container == null && IsPickupMovable( item ) && item.poison == 0 && item.IsOnFoodList( npc.foodList );
}

/** @param {Item} item @returns {boolean} */
function IsPickupMovable( item )
{
	if( item.movable == 1 )
		return true;
	// movable=0 uses the item's normal tile-data behavior. UOX3 represents the
	// immovable tile-data sentinel as a 255-stone item weight.
	return item.movable == 0 && item.weight < 25500;
}

/** @param {Item | null} item @returns {boolean} */
function IsShelterMarker( item )
{
	return ValidateObject( item ) && item.container == null && parseInt( item.GetTag( "NpcDriveShelter" )) == 1;
}

/** @param {Character} npc @param {BaseObject} item @returns {boolean} */
function IsInsideLeash( npc, item )
{
	var homeX = parseInt( npc.GetTempTag( "npcDriveHomeX" ));
	var homeY = parseInt( npc.GetTempTag( "npcDriveHomeY" ));
	var homeWorld = parseInt( npc.GetTempTag( "npcDriveHomeWorld" ));
	var homeInstance = parseInt( npc.GetTempTag( "npcDriveHomeInstance" ));
	var leashRadius = GetBoundedTag( npc, "NpcDriveLeashRadius", DEFAULT_LEASH_RADIUS, 1, MAX_LEASH_RADIUS );
	return item.worldnumber == homeWorld && item.instanceID == homeInstance && DistanceBetween( homeX, homeY, item.x, item.y ) <= leashRadius;
}

/** @param {Character} npc */
function EnsureHomePosition( npc )
{
	if( npc.GetTempTag( "npcDriveHomeSet" ))
		return;

	var homeX = parseInt( npc.GetTag( "NpcDriveHomeX" ));
	var homeY = parseInt( npc.GetTag( "NpcDriveHomeY" ));
	var homeZ = parseInt( npc.GetTag( "NpcDriveHomeZ" ));
	var homeWorld = parseInt( npc.GetTag( "NpcDriveHomeWorld" ));
	var homeInstance = parseInt( npc.GetTag( "NpcDriveHomeInstance" ));
	if( isNaN( homeX ) || isNaN( homeY ))
	{
		homeX = npc.spawnX;
		homeY = npc.spawnY;
	}
	if( isNaN( homeWorld ))
		homeWorld = npc.worldnumber;
	if( isNaN( homeInstance ))
		homeInstance = npc.instanceID;
	if( isNaN( homeZ ))
		homeZ = ( npc.spawnX != 0 || npc.spawnY != 0 ) && !isNaN( npc.spawnZ ) ? npc.spawnZ : npc.z;
	if( homeX == 0 && homeY == 0 )
	{
		homeX = npc.x;
		homeY = npc.y;
	}

	npc.SetTempTag( "npcDriveHomeX", homeX );
	npc.SetTempTag( "npcDriveHomeY", homeY );
	npc.SetTempTag( "npcDriveHomeZ", homeZ );
	npc.SetTempTag( "npcDriveHomeWorld", homeWorld );
	npc.SetTempTag( "npcDriveHomeInstance", homeInstance );
	npc.SetTempTag( "npcDriveHomeSet", 1 );
}

/** @param {Character} npc @returns {boolean} */
function NeedsRecovery( npc )
{
	return GetHealthPercent( npc ) <= GetBoundedTag( npc, "NpcDriveRestHealth", DEFAULT_REST_HEALTH, 1, 99 );
}

/** @param {Character} npc @returns {boolean} */
function HasRecovered( npc )
{
	var restThreshold = GetBoundedTag( npc, "NpcDriveRestHealth", DEFAULT_REST_HEALTH, 1, 99 );
	var resumeThreshold = GetBoundedTag( npc, "NpcDriveResumeHealth", DEFAULT_RESUME_HEALTH, 1, 100 );
	return GetHealthPercent( npc ) >= Math.max( restThreshold + 1, resumeThreshold );
}

/** @param {Character} npc @returns {number} */
function GetHealthPercent( npc )
{
	return npc.maxhp > 0 ? Math.floor(( npc.health * 100 ) / npc.maxhp ) : 100;
}

/** @param {Character} npc @returns {boolean} */
function IsShelterTime( npc )
{
	var startHour = GetBoundedTag( npc, "NpcDriveShelterStartHour", DEFAULT_SHELTER_START, 0, 23 );
	var endHour = GetBoundedTag( npc, "NpcDriveShelterEndHour", DEFAULT_SHELTER_END, 0, 23 );
	var hour = GetHour();
	return startHour > endHour ? ( hour >= startHour || hour < endHour ) : ( hour >= startHour && hour < endHour );
}

/** @param {Character} npc @returns {boolean} */
function ShouldContinueShelter( npc )
{
	var reason = npc.GetTempTag( "npcDriveReason" ).toString();
	if( reason == "recovery" )
	{
		if( DriveDisabled( npc, "Survival" ))
			return false;
		if( !HasRecovered( npc ))
			return true;
		if( !DriveDisabled( npc, "Shelter" ) && IsShelterTime( npc ))
		{
			npc.SetTempTag( "npcDriveReason", "night" );
			return true;
		}
		return false;
	}

	if( reason == "night" )
	{
		if( !DriveDisabled( npc, "Food" ) && CanSeekFood( npc ) && npc.hunger <= CRITICAL_HUNGER )
			return false;
		if( !DriveDisabled( npc, "Shelter" ) && IsShelterTime( npc ))
			return true;
		if( !DriveDisabled( npc, "Survival" ) && NeedsRecovery( npc ))
		{
			npc.SetTempTag( "npcDriveReason", "recovery" );
			return true;
		}
	}
	return false;
}

/** @param {Character} npc @returns {boolean} */
function IsAtRestLocation( npc )
{
	if( parseInt( npc.GetTempTag( "npcDriveRestInPlace" )) == 1 )
		return IsInBuilding( npc.x, npc.y, npc.z, npc.worldnumber, npc.instanceID, true );

	var shelter = GetDriveTarget( npc );
	return IsShelterMarker( shelter ) && npc.InRange( shelter, 1 );
}

/** @param {Character} npc @param {Item | null} shelter */
function BeginRest( npc, shelter )
{
	npc.SetTempTag( "npcDriveMode", "rest" );
	if( ValidateObject( shelter ))
		npc.SetTempTag( "npcDriveTarget", shelter.serial.toString() );
	npc.oldWandertype = WT_NONE;
	npc.wandertype = WT_NONE;
}

/** @param {Character} npc @returns {string} */
function GetDriveMode( npc )
{
	var mode = npc.GetTempTag( "npcDriveMode" );
	if( !mode )
		return "";
	mode = mode.toString().toLowerCase();
	return mode == "1" ? "food" : mode;
}

/** @param {Character} npc @returns {BaseObject | null} */
function GetDriveTarget( npc )
{
	var serial = ParseStoredSerial( npc.GetTempTag( "npcDriveTarget" ));
	if( isNaN( serial ))
		serial = ParseStoredSerial( npc.GetTempTag( "npcDriveFoodTarget" ));
	if( isNaN( serial ))
		return null;
	return npc.GetTempTag( "npcDriveTargetType" ).toString() == "char" ? CalcCharFromSer( serial ) : CalcItemFromSer( serial );
}

/**
 * Missing UOX3 integer tags read back as numeric zero. Drive serials are always
 * stored as strings so a real serial zero remains distinguishable as "0".
 * @param {string | number | boolean} rawValue
 * @returns {number}
 */
function ParseStoredSerial( rawValue )
{
	if( rawValue === null || rawValue === undefined || rawValue === false || rawValue === "" || ( rawValue === 0 && typeof rawValue != "string" ))
		return NaN;
	return parseInt( rawValue );
}

/** @param {Character} npc @param {Item} food */
function EatFood( npc, food )
{
	if( !IsUsableFood( npc, food ) || !npc.InRange( food, 1 ))
		return;

	if( food.amount > 1 )
		food.amount--;
	else
		food.Delete();

	npc.hunger = Math.min( MAX_HUNGER, npc.hunger + 1 );
	if( !npc.isHuman )
		npc.DoAction( 0x03 ); // ACT_ANIMAL_EAT
}

/** @param {Character} npc */
function RememberOriginalWander( npc )
{
	if( npc.GetTempTag( "npcDriveOriginalWanderSet" ))
		return;
	npc.SetTempTag( "npcDriveOriginalWander", npc.wandertype );
	npc.SetTempTag( "npcDriveOriginalWanderSet", 1 );
}

/** @param {Character} npc @param {boolean} restoreWander @param {number} retryDelay */
function ClearDriveGoal( npc, restoreWander, retryDelay )
{
	var mode = GetDriveMode( npc );
	var target = GetDriveTarget( npc );
	if( mode == "food" && ValidateObject( target ) && target.GetTempTag( "npcDriveReservedBy" ).toString() == npc.serial.toString() )
		target.SetTempTag( "npcDriveReservedBy", null );
	if( mode == "desire" && npc.GetTempTag( "npcDriveDesireAction" ).toString() == "pickup" && ValidateObject( target ) && target.GetTempTag( "npcDriveReservedBy" ).toString() == npc.serial.toString() )
		target.SetTempTag( "npcDriveReservedBy", null );
	if( mode == "desire" && npc.GetTempTag( "npcDriveDesireAction" ).toString() == "follow" )
		npc.Follow( null );

	if( restoreWander && npc.GetTempTag( "npcDriveOriginalWanderSet" ))
	{
		var oldWander = parseInt( npc.GetTempTag( "npcDriveOriginalWander" ));
		npc.wandertype = ( !isNaN( oldWander ) && oldWander >= WT_NONE && oldWander < WT_FROZEN ) ? oldWander : WT_FREE;
	}

	npc.oldWandertype = WT_NONE;
	npc.SetTempTag( "npcDriveMode", null );
	npc.SetTempTag( "npcDriveTarget", null );
	npc.SetTempTag( "npcDriveTargetType", null );
	npc.SetTempTag( "npcDriveFoodTarget", null );
	npc.SetTempTag( "npcDriveReason", null );
	npc.SetTempTag( "npcDriveRestInPlace", null );
	npc.SetTempTag( "npcDriveOriginalWander", null );
	npc.SetTempTag( "npcDriveOriginalWanderSet", null );
	npc.SetTempTag( "npcDriveDesireResource", null );
	npc.SetTempTag( "npcDriveDesireAction", null );
	npc.SetTempTag( "npcDriveDesireCooldown", null );
	npc.SetTempTag( "npcDriveDesireArrived", null );
	npc.SetTempTag( "npcDriveDesireEnd", null );
	npc.SetTempTag( "npcDriveAversionRadius", null );
	npc.SetTempTag( "npcDriveAversionRepath", null );
	npc.SetTempTag( "npcDriveIdleEnd", null );
	npc.SetTempTag( "npcDriveNextThink", ( GetCurrentClock() + retryDelay ).toString() );
}

/** @param {Character} npc @param {string} tagName @param {number} defaultValue @param {number} minimum @param {number} maximum @returns {number} */
function GetBoundedTag( npc, tagName, defaultValue, minimum, maximum )
{
	var rawValue = npc.GetTag( tagName );
	var value = parseInt( rawValue );
	// Integer tags cannot retain zero, but a string tag can explicitly store "0".
	if( isNaN( value ) || ( value == 0 && typeof rawValue != "string" ))
		value = defaultValue;
	return Math.max( minimum, Math.min( maximum, value ));
}
