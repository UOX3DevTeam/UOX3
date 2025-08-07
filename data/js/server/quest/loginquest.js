function LoginQuest( pChar )
{
	var autoQuestID = 9001; // Insert Starting Quest ID here

	// Load settings and progress
	var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pChar ) || {};
	var showLoginQuests = playerSettings["ShowLoginQuestOffers"] !== false;
	var seenQuests = playerSettings["SeenQuests"] || {};
	var progress = TriggerEvent( 5800, "ReadQuestProgress", pChar );
	var archived = TriggerEvent( 5800, "ReadArchivedQuests", pChar );

	var alreadyHas = false;
	for( var i = 0; i < progress.length; i++ )
	{
		if( progress[i].questID == autoQuestID )
		{
			alreadyHas = true;
			break;
		}
	}

	for( var i = 0; i < archived.length; i++ )
	{
		if( parseInt( archived[i], 10 ) == autoQuestID )
		{
			alreadyHas = true;
			break;
		}
	}

	// Show the gump if eligible
	if( showLoginQuests && !alreadyHas && !seenQuests[autoQuestID] )
	{
		seenQuests[autoQuestID] = true;
		playerSettings["SeenQuests"] = seenQuests;
		TriggerEvent( 5800, "SavePlayerSettings", pChar, playerSettings );

		TriggerEvent( 5802, "QuestConversationGump", pChar, null, autoQuestID );
		pChar.SetTempTag( "questConversationID", autoQuestID );
	}
}