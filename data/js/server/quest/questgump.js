function QuestGump( pUser )
{
	var socket = pUser.socket;
	var activeQuests = TriggerEvent( 5800, "loadAllQuests", pUser ); // Load all active quests
	var chainQuests = TriggerEvent( 5801, "getQuests" ); // Fetch chain quests dynamically

	var questsPerPage = 10; // Number of quests per page
	var totalPages = Math.ceil( activeQuests.length / questsPerPage );

	var gump = new Gump();
	gump.AddPage( 0 );
	gump.AddBackground( 30, 120, 296, 447, 1579 );
	gump.AddButton( 50, 100, 1588, 1589, 1, 0, 20 );
	gump.AddButton( 180, 100, 1588, 1589, 1, 0, 21 );
	gump.AddGump( 70, 130, 1577 );
	gump.AddText( 65, 102, 500, "Active Quests" );
	gump.AddText( 193, 102, 0, "Completed Quests" );

	for( var page = 0; page < totalPages; page++ )
	{
		gump.AddPage( page + 1 );

		var startIndex = page * questsPerPage;
		var endIndex = Math.min( startIndex + questsPerPage, activeQuests.length );

		// Add quest buttons and descriptions for the current page
		for( var i = startIndex; i < endIndex; i++ )
		{
			var questIndex = i % questsPerPage;
			var quest = null;

			// Find the corresponding quest in chainQuests
			for( var j = 0; j < chainQuests.length; j++ )
			{
				if( chainQuests[j].id === activeQuests[i].questID )
				{
					quest = chainQuests[j];
					break;
				}
			}

			if( !quest ) 
				continue;

			var buttonY = 210 + ( questIndex * 30 );

			// Add quest name and page button for details
			gump.AddPageButton( 40, buttonY, 1141, 1143, totalPages + i + 1 ); // Navigate to quest details
			gump.AddButton( 310, buttonY, 2706, 2708, 1, 101, i + 101 ); // "Track" button
			gump.AddText( 70, buttonY + 5, 500, quest.name ); // Quest name
		}

		// Add navigation buttons for the pages
		if( page > 0 ) 
		{
			gump.AddPageButton( 50, 400, 1145, 1147, page ); // Previous page button
		}

		if( page < totalPages - 1 )
		{
			gump.AddPageButton( 200, 400, 1145, 1147, page + 2 ); // Next page button
		}
	}

	// Add quest details as pages for each quest, keeping the quest list visible
	for( var i = 0; i < activeQuests.length; i++ )
	{
		var questID = activeQuests[i].questID;
		var progress = activeQuests[i];
		var quest = null;

		// Find the quest in chainQuests
		for( var j = 0; j < chainQuests.length; j++ )
		{
			if( chainQuests[j].id === questID )
			{
				quest = chainQuests[j];
				break;
			}
		}

		if( !quest ) 
			continue;

		// Build the objectives string with progress for each target
		var objectives = "";
		for( var k = 0; k < quest.steps[progress.step].objectives.length; k++ )
		{
			var obj = quest.steps[progress.step].objectives[k];

			// Handle skill objectives
			if( obj.type === "skill" )
			{
				var completed = ( progress.progress[k] || 0 ) >= obj.count ? " ( Completed )" : "";
				objectives += "<b>Skill:</b> " +
					( obj.skillName || "Unknown Skill" ) +
					" ( " + ( progress.progress[k] || 0 ) +
					"/" + obj.count + " )" + completed + "<br>";
			}
			// Handle multiple kill targets
			else if( obj.type === "kill" && obj.targets && obj.targets.length )
			{
				if( TriggerEvent( 50502, "couGumpSupport" )) 
				{
					objectives += "<basefont color=#ff8b00>Kill Targets:</basefont><br>";
				}
				else
				{
					objectives += "Kill Targets: <br>";
				}

				for( var t = 0; t < obj.targets.length; t++ )
				{
					var target = obj.targets[t];
					var targetProgress = ( progress.progress[k] && progress.progress[k][t] ) || 0;
					var targetCompleted = targetProgress >= target.count ? " ( Completed )" : "";
					objectives += "- " + ( target.name || "Unknown Target" ) +
						" ( " + targetProgress + "/" + target.count + " )" + targetCompleted + "<br>";
				}
			}
			// General objectives or single target
			else
			{
				var completed = ( progress.progress[k] || 0 ) >= obj.count ? " ( Completed )" : "";
				objectives += obj.type + ": " +
					( obj.displayName || obj.target || "Unknown Objective" ) +
					" ( " + ( progress.progress[k] || 0 ) +
					"/" + obj.count + " )" + completed + "<br>";
			}

			// Additional details for specific objective types
			if( obj.type === "Delivery" )
			{
				if( TriggerEvent( 5800, "couGumpSupport" ))
				{
					objectives += "<basefont color=#ff8b00>Target NPC: </basefont>" + ( obj.npcName || "Unknown NPC" ) + "<br>";
				}
				else
				{
					objectives += "Target NPC: " + ( obj.npcName || "Unknown NPC" ) + "<br>";
				}
			}
			else if( obj.type === "Drop" )
			{
				if( TriggerEvent( 5800, "couGumpSupport" ))
				{
					objectives += "<basefont color=#ff8b00>Target Item: </basefont>" + (obj.itemName || "Unknown Item") + "<br>";
				}
				else
				{
					objectives += "Target Item: " + ( obj.itemName || "Unknown Item" ) + "<br>";
				}
			}
		}


		// Add the quest details page
		gump.AddPage( totalPages + i + 1 );

		// Keep the quest list visible
		var startIndex = Math.floor( i / questsPerPage ) * questsPerPage;
		var endIndex = Math.min( startIndex + questsPerPage, activeQuests.length );
		for( var q = startIndex; q < endIndex; q++ )
		{
			var questIndex = q % questsPerPage;
			var questName = null;

			// Find the quest name in chainQuests
			for( var j = 0; j < chainQuests.length; j++ )
			{
				if( chainQuests[j].id === activeQuests[q].questID )
				{
					questName = chainQuests[j].name;
					break;
				}
			}

			if( !questName )
				questName = "Unknown Quest";

			var buttonY = 210 + ( questIndex * 30 );

			gump.AddPageButton( 40, buttonY, 1141, 1143, totalPages + q + 1 ); // Navigate to quest details
			gump.AddButton( 310, buttonY, 2706, 2708, 1, 1, q + 101 ); // "Track" button
			gump.AddText( 70, buttonY + 5, 500, questName ); // Quest name
		}

		// Display rewards
		var rewards = quest.steps[progress.step].rewards || {};
		var rewardText = "";

		if( rewards.gold )
		{
			rewardText = "Gold: " + ( rewards.gold || 0 ) + "<br>";
		}

		if( rewards.items && rewards.items.length > 0 ) 
		{
			rewardText += "Items:<br>";
			for (var i = 0; i < rewards.items.length; i++)
			{
				var item = rewards.items[i];
				rewardText += "- " + ( item.name || "Unknown Item" ) + " x" + ( item.amount || 1 ) + "<br>";
			}
		}

		if( rewards.fame )
		{
			rewardText += "Fame: " + rewards.fame + "<br>";
		}

		if( rewards.karma )
		{
			rewardText += "Karma: " + rewards.karma + "<br>";
		}

		// Add the quest details
		gump.AddBackground( 350, 120, 306, 309, 1579 ); // Details background
		if( TriggerEvent( 5800, "couGumpSupport" ))
		{
			gump.AddHTMLGump( 370, 130, 264, 92, false, true, "<H1><basefont color=#000000>Description:</H1><br>" + quest.steps[progress.step].description + "</basefont>" );
			gump.AddHTMLGump( 370, 230, 266, 81, false, false, "<H1><basefont color=#000000>Objective:</basefont></H1><br>" + objectives );
		}
		else 
		{

			gump.AddHTMLGump( 370, 130, 264, 92, true, true, "Description:<br>" + quest.steps[progress.step].description );
			gump.AddHTMLGump( 370, 230, 266, 81, false, false, "Objective:<br>" + objectives );
		}

		gump.AddHTMLGump( 370, 320, 266, 150, false, false, "<H1>Rewards:</H1><br>" + rewardText );

		// Add a "Back" button to return to the main quest list
		var originalPage = Math.floor( i / questsPerPage ) + 1;
		gump.AddPageButton( 50, 500, 1144, 1145, originalPage ); // Navigate back to the quest list page
	}

	gump.Send( socket );
	gump.Free();
}


function CompletedQuestsGump( pUser )
{
	var socket = pUser.socket;
	var completedQuests = TriggerEvent( 5800, "ReadQuestLog", pUser ); // Read completed quests
	var chainQuests = TriggerEvent( 5801, "getQuests" ); // Fetch chain quest data
	var trackedQuests = TriggerEvent( 5802, "ReadTrackedQuests", pUser ); // Read tracked quests

	var questsPerPage = 10; // Number of quests per page
	var totalPages = Math.ceil( completedQuests.length / questsPerPage );

	var gump = new Gump();
	gump.AddPage( 0 );
	gump.AddBackground( 30, 120, 296, 447, 1579 ); // Main background
	gump.AddGump( 70, 130, 1577 ); // Decorative gump
	gump.AddButton( 50, 100, 1588, 1589, 1, 0, 20 ); // Active quests button
	gump.AddButton( 180, 100, 1588, 1589, 1, 0, 21 ); // Completed quests button
	gump.AddText( 65, 102, 0, "Active Quests" ); // Header for Active Quests
	gump.AddText( 193, 102, 500, "Completed Quests" ); // Header for Completed Quests

	// Loop through pages
	for( var page = 0; page < totalPages; page++ )
	{
		gump.AddPage( page + 1 );

		var startIndex = page * questsPerPage;
		var endIndex = Math.min( startIndex + questsPerPage, completedQuests.length );

		// Loop through quests on this page
		for( var i = startIndex; i < endIndex; i++ )
		{
			var questIndex = i % questsPerPage;
			var questID = completedQuests[i].split( "," )[1]; // Extract quest ID
			var quest = null;

			// Find the corresponding quest in chainQuests
			for( var j = 0; j < chainQuests.length; j++ )
			{
				if( chainQuests[j].id === questID )
				{
					quest = chainQuests[j];
					break;
				}
			}

			if( !quest )
				continue;

			var buttonY = 210 + ( questIndex * 30 );

			// Add a button to view quest details
			gump.AddPageButton( 40, buttonY, 1141, 1143, totalPages + i + 1 ); // Navigate to quest details
			gump.AddText( 70, buttonY + 5, 500, quest.name + " ( Completed )" ); // Add "( Completed )" to the quest name
		}

		// Add navigation buttons for the pages
		if( page > 0 )
		{
			gump.AddPageButton( 50, 400, 1145, 1147, page ); // Previous page button
		}

		if( page < totalPages - 1 )
		{
			gump.AddPageButton( 200, 400, 1145, 1147, page + 2 ); // Next page button
		}
	}

	// Add details for each quest
	for( var i = 0; i < completedQuests.length; i++ )
	{
		var questID = completedQuests[i].split( "," )[1];
		var quest = null;

		// Find the corresponding quest in chainQuests
		for( var j = 0; j < chainQuests.length; j++ )
		{
			if( chainQuests[j].id === questID )
			{
				quest = chainQuests[j];
				break;
			}
		}

		if( !quest )
			continue;

		// Add quest details page
		gump.AddPage( totalPages + i + 1 );

		gump.AddBackground( 350, 120, 306, 309, 1579 ); // Details background
		if( TriggerEvent( 5800, "couGumpSupport" ))
		{
			gump.AddHTMLGump( 370, 130, 264, 92, false, true, "<H1><basefont color=#000000>Description:</H1><br>" + quest.steps[quest.steps.length - 1].description + "</basefont>" );
			gump.AddHTMLGump( 370, 230, 266, 81, false, false, "<H1><basefont color=#000000>Status:</basefont></H1><br>Completed<br>" );
		}
		else
		{
			gump.AddHTMLGump( 370, 130, 264, 92, true, true, "Description:<br>" + quest.steps[quest.steps.length - 1].description );
			gump.AddHTMLGump( 370, 230, 266, 81, false, false, "Status:<br>Completed<br>" );
		}
		//gump.AddHTMLGump( 370, 320, 266, 81, false, false, "<H1>Rewards:</H1><br>Gold: " + ( quest.steps[quest.steps.length - 1].rewards.gold || 0 ) + "<br>Items: " + ( quest.steps[quest.steps.length - 1].rewards.items || [] ).join( ", " ));
		
		// Display rewards for the last step of the quest
		var rewards = quest.steps[quest.steps.length - 1].rewards || {};
		var rewardText = "";

		if( rewards.gold )
		{
			rewardText = "Gold: " + ( rewards.gold || 0 ) + "<br>";
		}

		if( rewards.items && rewards.items.length > 0 )
		{
			rewardText += "Items:<br>";
			for( var i = 0; i < rewards.items.length; i++ )
			{
				var item = rewards.items[i];
				rewardText += "- " + ( item.name || "Unknown Item" ) + " x" + ( item.amount || 1 ) + "<br>";
			}
		}

		if( rewards.fame )
		{
			rewardText += "Fame: " + rewards.fame + "<br>";
		}

		if( rewards.karma )
		{
			rewardText += "Karma: " + rewards.karma + "<br>";
		}

		gump.AddHTMLGump( 370, 320, 266, 81, true, true, "<H1>Rewards:</H1><br>" + rewardText );


		// Add a "Back" button to return to the main quest list
		var originalPage = Math.floor( i / questsPerPage ) + 1;
		gump.AddPageButton( 50, 500, 1144, 1145, originalPage ); // Navigate back to the quest list page
	}

	gump.Send( socket );
	gump.Free();
}


function onGumpPress( pSock, pButton, gumpData ) 
{
	var maxQuesttracked = 10;
	var pUser = pSock.currentChar;
	var gumpID = 5802 + 0xffff;

	if( pButton === 1 )
	{
		// Clear all tracked quests
		QuestGump( pUser );
	}
	else if( pButton >= 101 && pButton < 111 )
	{
		// Track/Untrack quests for buttons 101 to 110
		var questIndex = pButton - 101;
		var activeQuests = TriggerEvent( 5800, "loadAllQuests", pUser );
		var trackedQuests = ReadTrackedQuests( pUser );

		if( questIndex < activeQuests.length )
		{
			var questID = activeQuests[questIndex].questID;
			var playerSerial = pUser.serial.toString();
			var isTracked = false;

			// Check if the quest is already tracked
			for( var i = 0; i < trackedQuests.length; i++ )
			{
				if( trackedQuests[i].questID === questID && trackedQuests[i].status === 1 )
				{
					isTracked = true;
					break;
				}
			}

			if( isTracked )
			{
				// Untrack the quest
				UpdateTrackedQuestStatus( pUser, questID, 0 ); // Set status to 0 ( untracked )
				TriggerEvent( 5803, "QuestTrackingGump", pUser );
				pSock.CloseGump(gumpID, 0);
				QuestGump( pUser );
				pUser.SysMessage( "Stopped tracking quest: " + questID );
			}
			else
			{
				// Track the quest
				var trackedCount = 0;
				for( var i = 0; i < trackedQuests.length; i++ )
				{
					if( trackedQuests[i].status === 1 )
					{
						trackedCount++;
					}
				}

				if( trackedCount < maxQuesttracked )
				{
					UpdateTrackedQuestStatus( pUser, questID, 1 ); // Set status to 1 ( tracked )
					TriggerEvent( 5803, "QuestTrackingGump", pUser );
					pSock.CloseGump(gumpID, 0);
					QuestGump( pUser );
					pUser.SysMessage( "Started tracking quest: " + questID );
				}
				else
				{
					pUser.SysMessage( "You can only track up to 5 quests." );
				}
			}

			// Update the quest tracking gump
			TriggerEvent( 5803, "QuestTrackingGump", pUser );
			pSock.CloseGump(gumpID, 0);
			QuestGump( pUser );
		}
	}
	else if( pButton === 20 )
	{
		pSock.CloseGump(gumpID, 0);
		QuestGump( pUser );
	}
	else if( pButton === 21 )
	{
		pSock.CloseGump(gumpID, 0);
		CompletedQuestsGump( pUser );
	}
}

function UpdateTrackedQuestStatus( pUser, questID, status )
{
	var trackingQuests = ReadTrackedQuests( pUser );
	var playerSerial = pUser.serial.toString();
	var found = false;

	// Update the status if the quest is already in the list
	for( var i = 0; i < trackingQuests.length; i++ )
	{
		if( trackingQuests[i].questID === questID )
		{
			trackingQuests[i].status = status;
			found = true;
			break;
		}
	}

	// Add new entry if not found
	if( !found )
	{
		trackingQuests.push( { questID: questID, status: status } );
	}

	// Write back to file
	WriteTrackedQuests( pUser, trackingQuests );
}


function WriteTrackedQuests( pUser, trackingQuests )
{
	var mFile = new UOXCFile();
	var userAccount = pUser.account;
	var fileName = "TrackedQuests_" + userAccount.id + ".jsdata";

	// Open the file for writing
	mFile.Open( fileName, "w", "Quests" );
	if( mFile )
	{
		var playerSerial = pUser.serial.toString();
		for( var i = 0; i < trackingQuests.length; i++ )
		{
			var questID = trackingQuests[i].questID;
			var trackStatus = trackingQuests[i].status; // 1 for tracked, 0 for not tracked
			// Format: <player_serial>,<track_status>,<quest_id>
			mFile.Write( playerSerial + "," + trackStatus + "," + questID + "\n" );
		}
		mFile.Close();
		mFile.Free();
		return true; // Indicate success
	}
	return false; // Indicate failure
}


function ReadTrackedQuests( pUser )
{
	var mFile = new UOXCFile();
	var userAccount = pUser.account;
	var fileName = "TrackedQuests_" + userAccount.id + ".jsdata";

	var trackingQuests = [];
	var playerSerial = pUser.serial.toString();

	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length() >= 0 )
	{
		while( !mFile.EOF() )
		{
			var line = manualTrim( mFile.ReadUntil( "\n" ));
			if( line && line !== "" )
			{
				var parts = line.split( "," );
				if( parts[0] === playerSerial )
				{ // Check if this line matches the player's serial
					trackingQuests.push( {
						questID: parts[2],
						status: parseInt( parts[1], 10 ) // Convert "1" or "0" to integer
					} );
				}
			}
		}
		mFile.Close();
		mFile.Free();
	}
	return trackingQuests;
}

function manualTrim( str )
{
	return str.replace( /^\s+|\s+$/g, "" ); // Remove leading and trailing whitespace
}
