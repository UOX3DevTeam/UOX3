/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( raceNpc: Character, timerID: number ) => void } */
function onTimer( raceNpc, timerID )
{
	if( !ValidateObject( raceNpc ) )
	{
		return;
	}

	if( timerID != 1 )
	{
		return;
	}

	if( !raceNpc.GetTag( "QuestRace" ) )
	{
		return;
	}

	var questID = parseInt( raceNpc.GetTag( "QuestRaceQuestID" ), 10 );
	if( isNaN( questID ) || questID <= 0 )
	{
		ClearRaceState( raceNpc, true );
		return;
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || !quest.race || !quest.race.checkpoints || !quest.race.checkpoints.length )
	{
		ClearRaceState( raceNpc, true );
		return;
	}

	var returning = parseInt( raceNpc.GetTag( "QuestRaceReturning" ), 10 );
	if( isNaN( returning ) )
	{
		returning = 0;
	}

	if( returning )
	{
		HandleRaceReturnHome( raceNpc );
		raceNpc.StartTimer( 1500, 1, 5817 );
		return;
	}

	var playerSerial = parseInt( raceNpc.GetTag( "QuestRacePlayerSerial" ), 10 );
	var player = null;
	if( !isNaN( playerSerial ) && playerSerial > 0 )
	{
		player = CalcCharFromSer( playerSerial );
	}

	if( !ValidateObject( player ) )
	{
		BeginRaceReturnHome( raceNpc );
		HandleRaceReturnHome( raceNpc );
		raceNpc.StartTimer( 1500, 1, 5817 );
		return;
	}

	TriggerEvent( 5800, "RacePlayerReachedFinishCheckpoint", player, questID );

	var checkpointIndex = parseInt( raceNpc.GetTag( "QuestRaceCheckpoint" ), 10 );
	if( isNaN( checkpointIndex ) || checkpointIndex < 0 )
	{
		checkpointIndex = 0;
	}

	if( checkpointIndex >= quest.race.checkpoints.length )
	{
		TriggerEvent( 5800, "RaceNPCReachedFinish", player, questID, raceNpc );
		BeginRaceReturnHome( raceNpc );
		HandleRaceReturnHome( raceNpc );
		raceNpc.StartTimer( 1500, 1, 5817 );
		return;
	}

	var checkpoint = quest.race.checkpoints[checkpointIndex];
	if( !checkpoint )
	{
		raceNpc.SetTag( "QuestRaceCheckpoint", checkpointIndex + 1 );
		raceNpc.StartTimer( 500, 1, 5817 );
		return;
	}

	var targetX = parseInt( checkpoint.x, 10 );
	var targetY = parseInt( checkpoint.y, 10 );
	var targetZ = parseInt( checkpoint.z, 10 );
	var targetWorld = parseInt( checkpoint.world, 10 );
	var range = parseInt( checkpoint.range, 10 );

	if( isNaN( targetX ) || isNaN( targetY ) )
	{
		raceNpc.SetTag( "QuestRaceCheckpoint", checkpointIndex + 1 );
		raceNpc.StartTimer( 500, 1, 5817 );
		return;
	}

	if( isNaN( range ) || range <= 0 )
	{
		range = 1;
	}

	if( !isNaN( targetWorld ) && targetWorld >= 0 && raceNpc.worldnumber != targetWorld )
	{
		raceNpc.SetLocation( targetX, targetY, isNaN( targetZ ) ? raceNpc.z : targetZ, targetWorld, raceNpc.instanceID );
	}

	var distanceToCheckpoint = DistanceBetween( raceNpc.x, raceNpc.y, targetX, targetY );
	if( distanceToCheckpoint <= range )
	{
		if( checkpoint.text )
		{
			raceNpc.TextMessage( String( checkpoint.text ) );
		}

		raceNpc.SetTag( "QuestRaceCheckpoint", checkpointIndex + 1 );
		raceNpc.StartTimer( 500, 1, 5817 );
		return;
	}

	var movement = String( quest.race.movement || "run" ).toLowerCase();

	if( movement == "walk" )
	{
		raceNpc.WalkTo( targetX, targetY, 40, true, true );
	}
	else
	{
		raceNpc.canRun = true;
		raceNpc.RunTo( targetX, targetY, 40, true, true );
	}

	raceNpc.StartTimer( 1000, 1, 5817 );
}

function BeginRaceReturnHome( raceNpc )
{
	if( !ValidateObject( raceNpc ) )
	{
		return;
	}

	if( raceNpc.GetTag( "QuestRaceReturning" ) )
	{
		return;
	}

	raceNpc.SetTag( "QuestRaceReturning", 1 );

	var returnText = raceNpc.GetTag( "QuestRaceReturnText" );
	if( returnText )
	{
		raceNpc.TextMessage( String( returnText ) );
	}
}

function HandleRaceReturnHome( raceNpc )
{
	if( !ValidateObject( raceNpc ) )
	{
		return;
	}

	var homeX = parseInt( raceNpc.GetTag( "QuestRaceHomeX" ), 10 );
	var homeY = parseInt( raceNpc.GetTag( "QuestRaceHomeY" ), 10 );
	var homeZ = parseInt( raceNpc.GetTag( "QuestRaceHomeZ" ), 10 );
	var homeWorld = parseInt( raceNpc.GetTag( "QuestRaceHomeWorld" ), 10 );
	var homeInstance = parseInt( raceNpc.GetTag( "QuestRaceHomeInstance" ), 10 );

	if( isNaN( homeX ) || isNaN( homeY ) )
	{
		ClearRaceState( raceNpc, true );
		return;
	}

	if( isNaN( homeZ ) )
	{
		homeZ = raceNpc.z;
	}

	if( isNaN( homeWorld ) || homeWorld < 0 )
	{
		homeWorld = raceNpc.worldnumber;
	}

	if( isNaN( homeInstance ) || homeInstance < 0 )
	{
		homeInstance = raceNpc.instanceID;
	}

	if( raceNpc.worldnumber != homeWorld || raceNpc.instanceID != homeInstance )
	{
		raceNpc.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
		ClearRaceState( raceNpc, false );
		return;
	}

	var distanceToHome = DistanceBetween( raceNpc.x, raceNpc.y, homeX, homeY );
	if( distanceToHome <= 1 )
	{
		raceNpc.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
		ClearRaceState( raceNpc, false );
		return;
	}

	var returnMovement = String( raceNpc.GetTag( "QuestRaceReturnMovement" ) || "walk" ).toLowerCase();

	if( returnMovement == "run" )
	{
		raceNpc.canRun = true;
		raceNpc.RunTo( homeX, homeY, 40, true, true );
	}
	else
	{
		raceNpc.WalkTo( homeX, homeY, 40, true, true );
	}
}

function ClearRaceState( raceNpc, keepAtCurrentLocation )
{
	if( !ValidateObject( raceNpc ) )
	{
		return;
	}

	var homeX = parseInt( raceNpc.GetTag( "QuestRaceHomeX" ), 10 );
	var homeY = parseInt( raceNpc.GetTag( "QuestRaceHomeY" ), 10 );
	var homeZ = parseInt( raceNpc.GetTag( "QuestRaceHomeZ" ), 10 );
	var homeWorld = parseInt( raceNpc.GetTag( "QuestRaceHomeWorld" ), 10 );
	var homeInstance = parseInt( raceNpc.GetTag( "QuestRaceHomeInstance" ), 10 );
	var originalCanRun = parseInt( raceNpc.GetTag( "QuestRaceOriginalCanRun" ), 10 );
	var originalFrozen = parseInt( raceNpc.GetTag( "QuestRaceOriginalFrozen" ), 10 );

	raceNpc.Follow( null );
	raceNpc.owner = null;

	if( !isNaN( originalCanRun ) )
	{
		raceNpc.canRun = ( originalCanRun == 1 );
	}

	if( !isNaN( originalFrozen ) )
	{
		raceNpc.frozen = ( originalFrozen == 1 );
	}

	raceNpc.SetTag( "QuestRace", null );
	raceNpc.SetTag( "QuestRaceQuestID", null );
	raceNpc.SetTag( "QuestRacePlayerSerial", null );
	raceNpc.SetTag( "QuestRaceCheckpoint", null );
	raceNpc.SetTag( "QuestRaceReturning", null );
	raceNpc.SetTag( "QuestRaceHomeX", null );
	raceNpc.SetTag( "QuestRaceHomeY", null );
	raceNpc.SetTag( "QuestRaceHomeZ", null );
	raceNpc.SetTag( "QuestRaceHomeWorld", null );
	raceNpc.SetTag( "QuestRaceHomeInstance", null );
	raceNpc.SetTag( "QuestRaceOriginalCanRun", null );
	raceNpc.SetTag( "QuestRaceOriginalFrozen", null );
	raceNpc.SetTag( "QuestRaceReturnText", null );
	raceNpc.SetTag( "QuestRaceReturnMovement", null );

	if( raceNpc.HasScriptTrigger( 5817 ) )
	{
		raceNpc.RemoveScriptTrigger( 5817 );
	}

	if( !keepAtCurrentLocation && !isNaN( homeX ) && !isNaN( homeY ) )
	{
		if( isNaN( homeZ ) )
		{
			homeZ = raceNpc.z;
		}
		if( isNaN( homeWorld ) || homeWorld < 0 )
		{
			homeWorld = raceNpc.worldnumber;
		}
		if( isNaN( homeInstance ) || homeInstance < 0 )
		{
			homeInstance = raceNpc.instanceID;
		}

		raceNpc.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
	}
}
