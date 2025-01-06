function QuestMenu( pUser )
{
	var socket = pUser.socket;
	var playerSerial = pUser.serial;

	// Load active and archived quests
	var activeQuests = TriggerEvent( 5800, "ReadQuestProgress", pUser ) || [];

	// Load player settings for the preferred category
	var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};
	var currentCategory = playerSettings["QuestLogCategory"] || "All Quests";
	var useTransparentBackground = playerSettings["UseTransparentBackground"] || false;

	// Define categories
	var categories = ["All Quests", "Main Story Quest", "Side Quests", "Event Quests", "Daily Quests", "World Quests", "City Quests"];
	var filteredQuests = [];
	for( var i = 0; i < activeQuests.length; i++ )
	{
		var questEntry = activeQuests[i];

		// Check if this quest belongs to the current player
		if (questEntry.serial != playerSerial)
			continue;

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest )
		{
			// Trim and normalize categories for comparison
			var questCategory = manualTrim( quest.category || "" ).toLowerCase();
			var playerCategory = manualTrim( currentCategory || "" ).toLowerCase();

			if( playerCategory == "all quests" || questCategory === playerCategory )
			{
				filteredQuests.push( questEntry );
			}
		}
	}

	var questsPerPage = 10; // Quests displayed per page
	var totalPages = Math.ceil( filteredQuests.length / questsPerPage );

	var gump = new Gump();
	gump.AddPage( 0 );
	gump.AddBackground( 30, 120, 296, 447, 1579 ); // Main background
	var titleHue = 500;
	// Background and transparency toggle
	if( useTransparentBackground )
	{
		gump.AddCheckerTrans( 30, 120, 296, 447 ); // Transparent background
		var titleHue = 1160;
	}
	gump.AddGump( 70, 130, 1577 );
	gump.AddText( 135, 200, titleHue, "Quest Journal" );
	gump.AddButton(50, 100, 1588, 1589, 1, 0, 20 ); // Active Quests Button
	gump.AddText( 65, 102, 500, "Active Quests" );
	gump.AddButton(180, 100, 1588, 1589, 1, 0, 21 ); // Completed Quests Button
	gump.AddText( 193, 102, 0, "Completed Quests" );

	// Adjust the quest list start position below the category buttons
	var questListStartY = 110 + categories.length * 16;
	//var questListStartY = 110 + categories.length * 30;

	// Add pagination for filtered quests
	for( var page = 0; page < totalPages; page++ )
	{
		gump.AddPage( page + 1 );

		var startIndex = page * questsPerPage;
		var endIndex = Math.min(startIndex + questsPerPage, filteredQuests.length );

		for( var i = startIndex; i < endIndex; i++ )
		{
			var questEntry = filteredQuests[i];
			var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
			var buttonY = questListStartY + (( i % questsPerPage ) * 25 ); // Reduced spacing

			if (!quest)
				continue;

			// Add quest name with a navigation button for details
			gump.AddPageButton( 40, buttonY, 1141, 1143, totalPages + i + 1 ); // Navigate to quest details
			gump.AddText( 80, buttonY + 5, 0, quest.title );
		}

		// Navigation buttons for quest list pages
		if( page > 0 )
		{
			gump.AddButton( 50, 450, 1144, 1145, page, 0, 0 ); // Previous Page Button
			gump.AddText( 80, 455, 0, "Previous" );
		}

		if( page < totalPages - 1 )
		{
			gump.AddButton( 200, 450, 1144, 1145, page + 2, 0, 0 ); // Next Page Button
			gump.AddText( 230, 455, 0, "Next" );
		}

		gump.AddPageButton( 40, 550, 1588, 1589, totalPages + filteredQuests.length + 1 ); // Options Menu Button
		gump.AddText( 65, 555, 0, "Options Menu" );
	}

	// Add quest details pages
	for( var i = 0; i < filteredQuests.length; i++ )
	{
		var questEntry = filteredQuests[i];
		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( !quest )
			continue;

		gump.AddPage( totalPages + i + 1 );

		// Add the quest details background
		gump.AddBackground( 350, 120, 306, 350, 1579 ); // Details background
		if( useTransparentBackground )
		{
			gump.AddCheckerTrans( 350, 120, 306, 350 ); // Transparent background
		}
		gump.AddHTMLGump( 47, 220, 266, 300, true, false,
			"Welcome to the Quest Details page!<br><br>" +
			"Here you can view all the information about the selected quest:<br>" +
			"- <b>Description:</b> A detailed summary of what the quest is about.<br>" +
			"- <b>Objectives:</b> A list of tasks or goals you need to complete to finish the quest. These may include collecting items, defeating enemies, or reaching a specific skill level.<br>" +
			"- <b>Rewards:</b> Displays the rewards you will receive upon completing the quest, such as gold, items, or experience.<br><br>" );

		gump.AddText( 440, 130, titleHue, quest.title );

		// Add quest description
		var description = quest.description || "No description available.";
		gump.AddHTMLGump( 370, 160, 266, 92, true, true, description ); // Description

		// Add objectives
		var objectives = TriggerEvent( 5802, "GetQuestObjectives", quest, questEntry ) || "No objectives available.";
		gump.AddHTMLGump( 370, 260, 266, 81, true, true, objectives );

		// Add rewards
		var rewards = TriggerEvent( 5802, "GetQuestRewards", quest ) || "No rewards available.";
		gump.AddHTMLGump( 370, 350, 266, 100, true, true, rewards );

		// Add a back button to return to the main quest list
		var originalPage = Math.floor( i / questsPerPage ) + 1;
		gump.AddPageButton( 225, 530, 247, 249, originalPage ); // Back Button
	}

	// Add Options Menu Page
	gump.AddPage( totalPages + filteredQuests.length + 1 );
	gump.AddBackground( 350, 120, 306, 350, 1579 ); // Options Menu Background
	gump.AddPageButton( 40, 550, 1588, 1589, totalPages + filteredQuests.length + 1 ); // Options Menu Button
	gump.AddText( 65, 555, 500, "Options Menu" );
	var wordHue = 0;
	if( useTransparentBackground )
	{
		gump.AddCheckerTrans( 350, 120, 306, 350 ); // Transparent background
		wordHue = 1152;
	}
	gump.AddHTMLGump( 47, 230, 266, 300, true, false,
		"Welcome to the Quest Journal Options!<br><br>" +
		"Here you can customize how your quests are displayed and how your journal looks:<br>" +
		"- <b>Quest Categories:</b> Filter your active quests by type, such as Main Story, Side Quests, or Event Quests.<br>" +
		"- <b>Background Transparency:</b> Toggle the background of your journal between solid and transparent for better readability.<br><br>" +
		"Make your selections by clicking the buttons beside." );
	gump.AddText(420, 130, titleHue, "Options Menu" );
	gump.AddBackground(365, 150, 280, 22, 9350);
	gump.AddText( 370, 150, 0, "Misc Options" );
	// Transparency toggle
	var transparencyToggleText = useTransparentBackground ? "Disable Transparency" : "Enable Transparency";
	//gump.AddButton( 370, 280, 1141, 1143, 1, 0, 2001 ); // Toggle transparency
	gump.AddCheckbox(370, 180, 210, 0, 2001); //CheckBox
	gump.AddText(400, 180, wordHue, transparencyToggleText);
	gump.AddBackground(365, 205, 280, 22, 9350);
	gump.AddText(370, 205, 0, "Sort by Quest Category");
	// Add category selection buttons
	for( var i = 0; i < categories.length; i++ ) 
	{
		var categoryStartY = 230; // Adjusted start position for categories
		var yPosition = categoryStartY + i * 25; // Spacing between categories
		var isSelected = categories[i] == currentCategory;
		gump.AddCheckbox( 370, yPosition, 210, 0, i + 1000 ); //CheckBox
		//gump.AddButton( 370, yPosition, isSelected ? 1141 : 1143, isSelected ? 1141 : 1143, 1, 0, i + 1000 ); // Category buttons
		gump.AddText( 400, yPosition, isSelected ? 500 : wordHue, categories[i] );
	}
	gump.AddButton( 380, 420, 238, 240, 1, 0, 2000 );//apple button
	gump.AddButton( 550, 420, 247, 249, 1, 0, 1 );//okay button

	// Add a fallback for when no quests are found
	if( filteredQuests.length === 0 )
	{
		gump.AddText( 50, 250, 0, "No quests match the selected category." );
	}

	gump.Send( socket );
	gump.Free();
}

// Utility function to trim whitespace
function manualTrim( str ) 
{
	return str.replace(/^\s+|\s+$/g, "");
}


function CompletedQuestsMenu( pUser ) 
{
	var socket = pUser.socket;

	// Read completed quests
	var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};
	var completedQuests = TriggerEvent( 5800, "ReadArchivedQuests", pUser );
	var useTransparentBackground = playerSettings["UseTransparentBackground"] || false;

	// Ensure unique entries
	var uniqueQuests = [];
	for( var i = 0; i < completedQuests.length; i++ )
	{
		if( uniqueQuests.indexOf( completedQuests[i] ) === -1 )
		{
			uniqueQuests.push( completedQuests[i] );
		}
	}

	var questsPerPage = 10; // Number of quests per page
	var totalPages = Math.ceil( uniqueQuests.length / questsPerPage );

	var gump = new Gump();
	gump.AddBackground( 30, 120, 296, 447, 1579 ); // Main background
	var titleHue = 500;
	// Background and transparency toggle
	if( useTransparentBackground )
	{
		gump.AddCheckerTrans( 30, 120, 296, 447 ); // Transparent background
		var titleHue = 1160;
	}
	gump.AddGump( 70, 130, 1577 );
	gump.AddText( 120, 200, titleHue, "Completed Quests" );
	gump.AddButton( 50, 100, 1588, 1589, 1, 0, 20 ); // Active Quests Button
	gump.AddText( 65, 102, 0, "Active Quests" );
	gump.AddButton( 180, 100, 1588, 1589, 1, 0, 21 ); // Completed Quests Button
	gump.AddText( 193, 102, 500, "Completed Quests" );

	// Loop through pages
	for( var page = 0; page < totalPages; page++ ) 
	{
		gump.AddPage( page + 1 );

		var startIndex = page * questsPerPage;
		var endIndex = Math.min( startIndex + questsPerPage, uniqueQuests.length );

		for( var i = startIndex; i < endIndex; i++ )
		{
			var questID = uniqueQuests[i];
			var quest = TriggerEvent( 5801, "QuestList", questID ); // Fetch quest details

			if( !quest )
				continue;

			var questIndex = i % questsPerPage;
			var buttonY = 220 + questIndex * 30;

			// Add quest name and navigation button to details
			gump.AddPageButton( 40, buttonY, 1141, 1143, totalPages + i + 1 );
			gump.AddText( 60, buttonY + 5, 0, quest.title + " ( Completed )" );
		}

		// Navigation buttons for the quest list pages
		if( page > 0 )
		{
			gump.AddPageButton( 50, 450, 1144, 1145, page );
			gump.AddText( 80, 455, 0, "Previous" );
		}

		if( page < totalPages - 1 )
		{
			gump.AddPageButton( 200, 450, 1144, 1145, page + 2 );
			gump.AddText( 230, 455, 0, "Next" );
		}
	}

	// Add quest details pages
	for( var i = 0; i < uniqueQuests.length; i++ )
	{
		var questID = uniqueQuests[i];
		var quest = TriggerEvent( 5801, "QuestList", questID );

		if( !quest )
			continue;

		gump.AddPage( totalPages + i + 1 );

		// Details background
		gump.AddBackground( 350, 120, 306, 350, 1579 ); // Details background
		gump.AddText( 420, 130, 500, quest.title );

		// Add description
		gump.AddHTMLGump( 370, 160, 266, 92, true, true, "<b>Description:</b><br>" + ( quest.description || "No description available." ) );

		// Add objectives
		var objectives = "Completed";
		gump.AddHTMLGump( 370, 260, 266, 81, true, true, objectives );

		// Add rewards
		var rewards = TriggerEvent( 5802, "GetQuestRewards", quest ); // Use helper function to build rewards
		gump.AddHTMLGump( 370, 350, 266, 100, true, true, rewards );

		// Add a back button to return to the main quest list
		var originalPage = Math.floor( i / questsPerPage ) + 1;
		gump.AddPageButton( 50, 500, 1144, 1145, originalPage );
	}

	gump.Send( socket );
	gump.Free();
}


function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var gumpID = 5823 + 0xffff;

	// Handle button presses for navigating between active and completed quests
	if( pButton === 1 ) 
	{
		pSock.CloseGump(gumpID, 0);
		QuestMenu(pUser);
	}
	else if( pButton === 20 )
	{
		pSock.CloseGump( gumpID, 0 );
		QuestMenu( pUser );
	} 
	else if( pButton === 21 )
	{
		pSock.CloseGump( gumpID, 0 );
		CompletedQuestsMenu( pUser );
	}
	if( pButton == 2000 ) 
	{
		var OtherButton = gumpData.getButton( 0 );
		// Handle category selection
		if ( OtherButton >= 1000 && OtherButton <= 1006 ) 
		{ // Assuming 1000-1003 are category buttons
			var categories = ["All Quests", "Main Story Quest", "Side Quests", "Event Quests", "Daily Quests", "World Quests", "City Quests"];
			var selectedCategory = categories[OtherButton - 1000]; // Map button ID to category

			// Load existing settings
			var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};

			// Update the "QuestLogCategory" setting
			playerSettings["QuestLogCategory"] = selectedCategory;

			// Save the updated settings object
			TriggerEvent( 5800, "SavePlayerSettings", pUser, playerSettings );

			pSock.SysMessage( "Quest log category set to: " + selectedCategory );

			// Refresh the quest menu with the new category
			pSock.CloseGump( gumpID, 0 );
			QuestMenu( pUser );
		}
		else if( OtherButton === 2001 ) 
		{ // Transparency toggle button
			// Load existing settings
			var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};

			// Toggle the transparency setting
			var useTransparentBackground = playerSettings["UseTransparentBackground"] || false;
			playerSettings["UseTransparentBackground"] = !useTransparentBackground;

			// Save the updated settings
			TriggerEvent( 5800, "SavePlayerSettings", pUser, playerSettings );

			pSock.SysMessage( "Transparency " + (playerSettings["UseTransparentBackground"] ? "enabled" : "disabled" ) + "." );

			// Refresh the options menu
			pSock.CloseGump( gumpID, 0 );
			QuestMenu( pUser );
		}
	}
}