function onEnterRegion( pEntering, regionEntered )
{
	var pSock = pEntering.socket;
	if( !pSock )
	{
		return;
	}

	// Fetch the active quests for the player
	var activeQuests = TriggerEvent( 5800, "ReadQuestProgress", pEntering );

	if( !activeQuests || activeQuests.length == 0 )
	{
		return;
	}
	// Loop through active quests to check for skill training in the entered region
	for (var i = 0; i < activeQuests.length; i++)
	{
		var questEntry = activeQuests[i];

		// Ensure the quest entry matches the player's serial
		if (questEntry.serial != pEntering.serial)
		{
			continue; // Skip quests not associated with the current player
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest.type == "skillgain" )
		{
			if( quest.trainingarea ) 
			{
				// Display the skill training message
				pSock.SysMessage(quest.ontrainingarea);
			}
			return;
		}
	}
}