// Handle item pickups for quests
function onPickup( iPickedUp, pGrabber, containerObj )
{
	if( !ValidateObject(pGrabber ) || !ValidateObject(iPickedUp ))
		return true;

	// Check if the item has already been marked as "picked up"
	if( iPickedUp.GetTag( "questPickedUp" ))
	{
		pGrabber.SysMessage( "This item has already been counted for a quest." );
		return true;
	}

	var activeQuests = TriggerEvent( 5800, "loadAllQuests", pGrabber );
	var chainQuests = TriggerEvent( 5801, "getQuests", pGrabber );

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

			if( obj.type === "collect" && obj.target === iPickedUp.sectionID )
			{
				// Increment progress
				obj.progress = ( progress.progress[k] || 0 ) + 1;

				// Check if a location is required
				if( obj.location )
				{
					if( iPickedUp.x === obj.location.x && iPickedUp.y === obj.location.y )
					{
						// Mark the item as picked up for the quest
						iPickedUp.SetTag( "questPickedUp", true );

						// Increment progress forlocation-based pickup
						pGrabber.SysMessage( "Collected " + obj.progress + "/" + obj.count + " " + obj.target + " from the correct location." );

						//progress.progress[i] = obj.progress;
						TriggerEvent( 5800, "saveQuestProgress", pGrabber, progress.questID, progress.step, progress.progress, progress.completed );

						if( TriggerEvent( 5800, "isStepComplete", step ))
						{
							TriggerEvent( 5800, "advanceToNextStep", pGrabber, quest );
						}
						return true;
					}
					else
					{
						pGrabber.SysMessage( "This item must be picked up from the correct location at (" + obj.location.x + ", " + obj.location.y + ")." );
						return true;
					}
				}
				else
				{
					// Mark the item as picked up for the quest
					iPickedUp.SetTag( "questPickedUp", true );

					pGrabber.SysMessage( "Collected " + obj.progress + "/" + obj.count + " " + obj.target + "." );

					progress.progress[k] = obj.progress;
					TriggerEvent( 5800, "saveQuestProgress", pGrabber, progress.questID, progress.step, progress.progress, progress.completed );

					if( TriggerEvent( 5800, "isStepComplete", step ))
					{
						TriggerEvent( 5800, "advanceToNextStep", pGrabber, quest );
					}
					return true;
				}
			}
		}
	}

	//pGrabber.SysMessage( "This item is not part of any active quest." );debug msg
	return true;
}