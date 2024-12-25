function QuestGathering( objMade, creator )
{
	// Validate the created object
	if( !ValidateObject( objMade )) 
	{
		return;
	}

	// Get the item's section ID
	var itemID = objMade.sectionID;
	var amountCreated = objMade.amount || 1; // Amount of ore created ( default is 1 )

	// Load active quests for the creator
	var activeQuests = TriggerEvent( 5800, "loadAllQuests", creator );
	var chainQuests = TriggerEvent( 5801, "getQuests", creator );

	for( var q = 0; q < activeQuests.length; q++ )
	{
		var progress = activeQuests[q];
		var quest = null;

		// Match the active quest with its chain quest data
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
		for( var o = 0; o < step.objectives.length; o++ )
		{
			var obj = step.objectives[o];

			// Check if the objective is to collect a specific item
			if( obj.type === "gather" && obj.target === itemID )
			{
				var currentProgress = progress.progress[o] || 0;
				var remainingNeeded = obj.count - currentProgress;

				if( remainingNeeded > 0 )
				{
					// Only count the amount needed to complete the objective
					var amountToAdd = Math.min( amountCreated, remainingNeeded );
					progress.progress[o] = currentProgress + amountToAdd;

					// Notify the player
					creator.SysMessage(  "Collected " + progress.progress[o] + "/" + obj.count + " " + ( obj.displayName || "Unknown Item" ) + "!" );

					// Save the updated progress
					TriggerEvent( 5800, "saveQuestProgress", creator, progress.questID, progress.step, progress.progress, progress.completed );

					// Check if the objective is complete
					if( progress.progress[o] >= obj.count )
					{
						// Check if all objectives in the step are complete
						if (TriggerEvent( 5800, "isStepComplete", step, progress.progress ) )
						{
							// Advance to the next step or mark quest as completed
							TriggerEvent( 5800, "advanceToNextStep", creator, quest );
							return; // No need to check further objectives
						}
					}
				}
				else
				{
					// Notify the player if they've already collected enough
					creator.SysMessage( "You have already collected enough " + ( obj.displayName || "Unknown Item" ) + "!" );
				}
				return; // Exit after processing this objective
			}
		}
	}
}