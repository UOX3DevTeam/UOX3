/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( guideNpc: Character, timerID: number ) => void } */
function onTimer( guideNpc, timerID )
{
	if( !ValidateObject( guideNpc ) )
	{
		return;
	}

	if( timerID != 1 )
	{
		return;
	}

	if( !guideNpc.GetTag( "QuestGuidedWalk" ) )
	{
		return;
	}

	var questID = parseInt( guideNpc.GetTag( "QuestGuidedWalkQuestID" ), 10 );
	if( isNaN( questID ) || questID <= 0 )
	{
		ClearGuidedWalkState( guideNpc, true );
		return;
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || !quest.guidedWalk || !quest.guidedWalk.steps || !quest.guidedWalk.steps.length )
	{
		ClearGuidedWalkState( guideNpc, true );
		return;
	}

	var playerSerial = parseInt( guideNpc.GetTag( "QuestGuidedWalkPlayerSerial" ), 10 );
	var player = null;
	if( !isNaN( playerSerial ) && playerSerial > 0 )
	{
		player = CalcCharFromSer( playerSerial );
	}

	if( !ValidateObject( player ) )
	{
		BeginGuidedWalkReturnHome( guideNpc );
		HandleGuidedWalkReturnHome( guideNpc );
		guideNpc.StartTimer( 2000, 1, 5816  );
		return;
	}

	var guidedWalkData = quest.guidedWalk;
	var stepIndex = parseInt( guideNpc.GetTag( "QuestGuidedWalkStep" ), 10 );
	if( isNaN( stepIndex ) || stepIndex < 0 )
	{
		stepIndex = 0;
	}

	var guidedWalkComplete = parseInt( guideNpc.GetTag( "QuestGuidedWalkComplete" ), 10 );
	if( isNaN( guidedWalkComplete ) )
	{
		guidedWalkComplete = 0;
	}

	var guidedWalkReturning = parseInt( guideNpc.GetTag( "QuestGuidedWalkReturning" ), 10 );
	if( isNaN( guidedWalkReturning ) )
	{
		guidedWalkReturning = 0;
	}

	var maxDistance = parseInt( guideNpc.GetTag( "QuestGuidedWalkMaxDistance" ), 10 );
	if( isNaN( maxDistance ) || maxDistance <= 0 )
	{
		maxDistance = 24;
	}

	var playerDistance = DistanceBetween( guideNpc, player );

	if( guidedWalkReturning )
	{
		HandleGuidedWalkReturnHome( guideNpc );
		guideNpc.StartTimer( 2000, 1, 5816  );
		return;
	}

	if( guidedWalkComplete )
	{
		if( playerDistance > maxDistance )
		{
			BeginGuidedWalkReturnHome( guideNpc );
			HandleGuidedWalkReturnHome( guideNpc );
		}

		guideNpc.StartTimer( 2000, 1, 5816  );
		return;
	}

	if( playerDistance > maxDistance )
	{
		if( !guideNpc.GetTag( "QuestGuidedWalkTooFarWarned" ) )
		{
			guideNpc.TurnToward( player );
			guideNpc.TextMessage( guidedWalkData.tooFarText || "Stay close. I am not done yet." );
			guideNpc.SetTag( "QuestGuidedWalkTooFarWarned", 1 );
		}

		guideNpc.StartTimer( 2000, 1, 5816  );
		return;
	}

	guideNpc.SetTag( "QuestGuidedWalkTooFarWarned", null );

	if( stepIndex >= guidedWalkData.steps.length )
	{
		guideNpc.SetTag( "QuestGuidedWalkComplete", 1 );

		if( guidedWalkData.finalText )
		{
			guideNpc.TurnToward( player );
			guideNpc.TextMessage( String( guidedWalkData.finalText ) );
		}

		guideNpc.StartTimer( 2000, 1, 5816  );
		return;
	}

	var currentStep = guidedWalkData.steps[stepIndex];
	if( !currentStep )
	{
		guideNpc.SetTag( "QuestGuidedWalkStep", stepIndex + 1 );
		guideNpc.StartTimer( 500, 1, 5816  );
		return;
	}

	var targetX = parseInt( currentStep.x, 10 );
	var targetY = parseInt( currentStep.y, 10 );
	var targetZ = parseInt( currentStep.z, 10 );
	var targetWorld = parseInt( currentStep.world, 10 );
	var arrivalRange = parseInt( currentStep.range, 10 );

	if( isNaN( targetX ) || isNaN( targetY ) )
	{
		guideNpc.SetTag( "QuestGuidedWalkStep", stepIndex + 1 );
		guideNpc.StartTimer( 500, 1, 5816  );
		return;
	}

	if( isNaN( arrivalRange ) || arrivalRange <= 0 )
	{
		arrivalRange = 1;
	}

	var legStartedTag = "QuestGuidedWalkLegStarted_" + stepIndex;
	if( !guideNpc.GetTag( legStartedTag ) )
	{
		if( currentStep.startText )
		{
			guideNpc.TurnToward( player );
			guideNpc.TextMessage( String( currentStep.startText ) );
		}
		guideNpc.SetTag( legStartedTag, 1 );
	}

	if( !isNaN( targetWorld ) && targetWorld >= 0 && guideNpc.worldnumber != targetWorld )
	{
		guideNpc.SetLocation( targetX, targetY, isNaN( targetZ ) ? guideNpc.z : targetZ, targetWorld, guideNpc.instanceID );
	}

	var distanceToStep = DistanceBetween( guideNpc.x, guideNpc.y, targetX, targetY );
	if( distanceToStep <= arrivalRange )
	{
		if( currentStep.arrivalText )
		{
			guideNpc.TurnToward( player );
			guideNpc.TextMessage( String( currentStep.arrivalText ) );
		}

		guideNpc.SetTag( legStartedTag, null );
		guideNpc.SetTag( "QuestGuidedWalkStep", stepIndex + 1 );

		if(( stepIndex + 1 ) >= guidedWalkData.steps.length )
		{
			guideNpc.SetTag( "QuestGuidedWalkComplete", 1 );

			if( guidedWalkData.finalText )
			{
				guideNpc.TurnToward( player );
				guideNpc.TextMessage( String( guidedWalkData.finalText ) );
			}
		}

		guideNpc.StartTimer( 2000, 1, 5816  );
		return;
	}

	if( String( guidedWalkData.movement || "walk" ).toLowerCase() == "run" )
	{
		guideNpc.RunTo( targetX, targetY, isNaN( targetZ ) ? guideNpc.z : targetZ, 40, true, true );
	}
	else
	{
		guideNpc.WalkTo( targetX, targetY, isNaN( targetZ ) ? guideNpc.z : targetZ, 40, true, true );
	}

	guideNpc.StartTimer( 1500, 1, 5816  );
}

function BeginGuidedWalkReturnHome( guideNpc )
{
	if( !ValidateObject( guideNpc ) )
	{
		return;
	}

	if( guideNpc.GetTag( "QuestGuidedWalkReturning" ) )
	{
		return;
	}

	guideNpc.SetTag( "QuestGuidedWalkReturning", 1 );

	var returnText = guideNpc.GetTag( "QuestGuidedWalkReturnText" );
	if( returnText )
	{
		guideNpc.TextMessage( String( returnText ) );
	}
}

function HandleGuidedWalkReturnHome( guideNpc )
{
	if( !ValidateObject( guideNpc ) )
	{
		return;
	}

	var homeX = guideNpc.spawnX;
	var homeY = guideNpc.spawnY;
	var homeZ = guideNpc.spawnZ;
	var homeWorld = guideNpc.worldnumber;
	var homeInstance = guideNpc.instanceID;

	if( isNaN( homeX ) || isNaN( homeY ) )
	{
		ClearGuidedWalkState( guideNpc, true );
		return;
	}

	if( isNaN( homeZ ) )
	{
		homeZ = guideNpc.z;
	}

	var distanceToHome = DistanceBetween( guideNpc.x, guideNpc.y, homeX, homeY );
	if( distanceToHome <= 1 )
	{
		guideNpc.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
		ClearGuidedWalkState( guideNpc, false );
		return;
	}

	var returnMovement = String( guideNpc.GetTag( "QuestGuidedWalkReturnMovement" ) || "walk" ).toLowerCase();

	if( returnMovement == "run" )
	{
		guideNpc.RunTo( homeX, homeY, homeZ, 40, true, true );
	}
	else
	{
		guideNpc.WalkTo( homeX, homeY, homeZ, 40, true, true );
	}
}

function ClearGuidedWalkState( guideNpc, keepAtCurrentLocation )
{
	if( !ValidateObject( guideNpc ) )
	{
		return;
	}

	var originalFrozen = parseInt( guideNpc.GetTag( "QuestGuidedWalkOriginalFrozen" ), 10 );

	guideNpc.Follow( null );
	guideNpc.owner = null;
	if( !isNaN( originalFrozen ) )
	{
		guideNpc.frozen = ( originalFrozen == 1 );
	}

	guideNpc.SetTag( "QuestGuidedWalk", null );
	guideNpc.SetTag( "QuestGuidedWalkQuestID", null );
	guideNpc.SetTag( "QuestGuidedWalkPlayerSerial", null );
	guideNpc.SetTag( "QuestGuidedWalkStep", null );
	guideNpc.SetTag( "QuestGuidedWalkComplete", null );
	guideNpc.SetTag( "QuestGuidedWalkReturning", null );
	guideNpc.SetTag( "QuestGuidedWalkMaxDistance", null );
	guideNpc.SetTag( "QuestGuidedWalkReturnText", null );
	guideNpc.SetTag( "QuestGuidedWalkTooFarWarned", null );
	guideNpc.SetTag( "QuestGuidedWalkOriginalFrozen", null );

	var guideStepIndex = 0;
	for( guideStepIndex = 0; guideStepIndex < 50; guideStepIndex++ )
	{
		guideNpc.SetTag( "QuestGuidedWalkLegStarted_" + guideStepIndex, null );
	}

	if( !keepAtCurrentLocation )
	{
		var homeX = guideNpc.spawnX;
		var homeY = guideNpc.spawnY;
		var homeZ = guideNpc.spawnZ;

		if( !isNaN( homeX ) && !isNaN( homeY ))
		{
			if( isNaN( homeZ ))
			{
				homeZ = guideNpc.z;
			}

			guideNpc.SetLocation( homeX, homeY, homeZ, guideNpc.worldnumber, guideNpc.instanceID);
		}
	}

	if( guideNpc.HasScriptTrigger( 5816 ) )
	{
		guideNpc.RemoveScriptTrigger( 5816 );
	}
}