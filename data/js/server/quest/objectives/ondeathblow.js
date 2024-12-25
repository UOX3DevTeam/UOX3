// Handle NPC kills for quests
function onDeathBlow( victim, player )
{
	if( !ValidateObject( player ) || !ValidateObject( victim ))
		return true;

	var activeQuests = TriggerEvent( 5800, "loadAllQuests", player );
	var chainQuests = TriggerEvent( 5801, "getQuests", player );

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
			continue;

		var step = quest.steps[progress.step];

		for( var k = 0; k < step.objectives.length; k++ )
		{
			var obj = step.objectives[k];

			// Check if the objective is to kill a specific NPC type
			if( obj.type === "kill" && obj.target === victim.sectionID )
			{
				// Increment progress
				obj.progress = ( progress.progress[k] || 0 ) + 1;

				// Notify the player
				player.SysMessage( "Killed " + obj.progress + "/" + obj.count + " " + obj.target + "." );

				// Save progress
				progress.progress[k] = obj.progress;
				TriggerEvent( 5800,"saveQuestProgress",player,progress.questID,progress.step,progress.progress,progress.completed );

				// Check if the objective is complete
				if (obj.progress >= obj.count && TriggerEvent( 5800, "isStepComplete", step ))
				{
					TriggerEvent( 5800, "advanceToNextStep", player, quest );
				}

				return true;
			}
		}
	}

	//player.SysMessage( "This kill is not part of any active quest." ); debug msg
	return true;
}

