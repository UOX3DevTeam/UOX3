function onPickup( iPickedUp, pGrabber, containerObj )
{
	if( !ValidateObject( pGrabber ) || !ValidateObject( iPickedUp ))
	{
		return true;
	}

	// Check if the item has already been marked as "picked up"
	if( iPickedUp.GetTag( "questPickedUp" ))
	{
		pGrabber.SysMessage( "This item has already been counted for a quest." );
		return true;
	}

	var activeQuests = TriggerEvent( 5800, "loadAllQuests", pGrabber );
	var chainQuests = TriggerEvent( 5801, "getQuests", pGrabber );

	var itemID = iPickedUp.sectionID;
	var amountPickedUp = iPickedUp.amount || 1; // Default to 1 if no amount property is set

	for( var i = 0; i < activeQuests.length; i++ )
	{
		var progress = activeQuests[i];
		var quest = null;

		// Find the corresponding quest in chainQuests
		for( var j = 0; j < chainQuests.length; j++ )
		{
			if( chainQuests[j].id === progress.questID )
			{
				quest = chainQuests[j];
				break;
			}
		}

		if( !quest || progress.completed )
		{
			continue; // Skip if quest not found or already completed
		}

		var step = quest.steps[progress.step];
		for( var k = 0; k < step.objectives.length; k++ )
		{
			var obj = step.objectives[k];

			if( obj.type === "collect" && obj.target === itemID )
			{
				// Check for location-based requirement
				if( obj.location )
				{
					if( iPickedUp.x !== obj.location.x || iPickedUp.y !== obj.location.y )
					{
						pGrabber.SysMessage( "This item must be picked up from the correct location at ( " + obj.location.x + ", " + obj.location.y + " )." );
						return true;
					}
				}

				// Ensure progress does not exceed the required amount
				var currentProgress = progress.progress[k] || 0;
				var remainingNeeded = obj.count - currentProgress;
				var increment = Math.min( amountPickedUp, remainingNeeded );

				progress.progress[k] = currentProgress + increment;

				// Mark the item as picked up for the quest if fully used
				if( increment > 0 )
				{
					iPickedUp.SetTag( "questPickedUp", true );
				}

				// Notify the player
				pGrabber.SysMessage( "Collected " + progress.progress[k] + "/" + obj.count + " " + ( obj.displayName || "Unknown Item" ) + "." );

				// Save the updated progress
				TriggerEvent( 5800, "saveQuestProgress", pGrabber, progress.questID, progress.step, progress.progress, progress.completed );

				// Check if all objectives for the step are complete
				if( TriggerEvent( 5800, "isStepComplete", step, progress.progress )) 
				{
					TriggerEvent( 5800, "advanceToNextStep", pGrabber, quest );
				}

				return true;
			}
		}
	}

	//pGrabber.SysMessage( "This item is not part of any active quest." ); debug msg
	return true;
}