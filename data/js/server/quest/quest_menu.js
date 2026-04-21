/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( pUser: Character ) => void } */
function QuestMenu( pUser )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

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
		if( questEntry.serial != playerSerial )
			continue;

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest )
		{
			// Trim and normalize categories for comparison
			var questCategory = ( quest.category || "" ).trim().toLowerCase();
			var playerCategory = ( currentCategory || "" ).trim().toLowerCase();

			if( playerCategory == "all quests" || questCategory == playerCategory )
			{
				filteredQuests.push( questEntry );
			}
		}
	}

	var questsPerPage = 10; // Quests displayed per page
	var totalPages = Math.ceil( filteredQuests.length / questsPerPage );
	if( totalPages == 0 )
	{
		totalPages = 1;
	}

	var mainQuestMenu = new Gump();
	mainQuestMenu.AddPage( 0 );
	mainQuestMenu.AddBackground( 30, 120, 296, 447, 1579 ); // Main background
	var titleHue = 500;
	// Background and transparency toggle
	if( useTransparentBackground )
	{
		mainQuestMenu.AddCheckerTrans( 30, 120, 296, 447 ); // Transparent background
		titleHue = 1160;
	}
	mainQuestMenu.AddGump( 70, 130, 1577 );
	mainQuestMenu.AddText( 135, 200, titleHue, "Quest Journal" );
	mainQuestMenu.AddButton(50, 100, 1588, 1589, 1, 0, 20 ); // Active Quests Button
	mainQuestMenu.AddText( 65, 102, 500, "Active Quests" );
	mainQuestMenu.AddButton(180, 100, 1588, 1589, 1, 0, 21 ); // Completed Quests Button
	mainQuestMenu.AddText( 193, 102, 0, "Completed Quests" );

	// Adjust the quest list start position below the category buttons
	var questListStartY = 110 + categories.length * 16;

	// Add pagination for filtered quests
	for( var page = 0; page < totalPages; page++ )
	{
		mainQuestMenu.AddPage( page + 1 );

		if( filteredQuests.length == 0 && page == 0 )
		{
			mainQuestMenu.AddText( 50, 250, 0, "No quests match the selected category." );
		}

		var startIndex = page * questsPerPage;
		var endIndex = Math.min( startIndex + questsPerPage, filteredQuests.length );

		for( var j = startIndex; j < endIndex; j++ )
		{
			var questEntry = filteredQuests[j];
			var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
			var buttonY = questListStartY + (( j % questsPerPage ) * 25 ); // Reduced spacing

			if( !quest )
				continue;

			// Add quest name with a navigation button for details
			mainQuestMenu.AddPageButton( 40, buttonY, 1141, 1143, totalPages + j + 1 ); // Navigate to quest details
			mainQuestMenu.AddText( 80, buttonY + 5, 0, quest.title );
		}

		// Navigation buttons for quest list pages
		if( page > 0 )
		{
			mainQuestMenu.AddButton( 50, 450, 1144, 1145, page, 0, 0 ); // Previous Page Button
			mainQuestMenu.AddText( 80, 455, 0, "Previous" );
		}

		if( page < totalPages - 1 )
		{
			mainQuestMenu.AddButton( 200, 450, 1144, 1145, page + 2, 0, 0 ); // Next Page Button
			mainQuestMenu.AddText( 230, 455, 0, "Next" );
		}

		mainQuestMenu.AddPageButton( 40, 550, 1588, 1589, totalPages + filteredQuests.length + 1 ); // Options Menu Button
		mainQuestMenu.AddText( 65, 555, 0, "Options Menu" );
	}

	// Add quest details pages
	for( var m = 0; m < filteredQuests.length; m++ )
	{
		var questEntry = filteredQuests[m];
		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( !quest )
			continue;

		mainQuestMenu.AddPage( totalPages + m + 1 );

		// Add the quest details background
		mainQuestMenu.AddBackground( 350, 120, 306, 350, 1579 ); // Details background
		if( useTransparentBackground )
		{
			mainQuestMenu.AddCheckerTrans( 350, 120, 306, 350 ); // Transparent background
		}
		mainQuestMenu.AddHTMLGump( 47, 220, 266, 300, true, false,
			"Welcome to the Quest Details page!<br><br>" +
			"Here you can view all the information about the selected quest:<br>" +
			"- <b>Description:</b> A detailed summary of what the quest is about.<br>" +
			"- <b>Objectives:</b> A list of tasks or goals you need to complete to finish the quest. These may include collecting items, defeating enemies, or reaching a specific skill level.<br>" +
			"- <b>Rewards:</b> Displays the rewards you will receive upon completing the quest, such as gold, items, or experience.<br><br>" );

		mainQuestMenu.AddText( 440, 130, titleHue, quest.title );

		// Add quest description
		var description = quest.description || "No description available.";
		mainQuestMenu.AddHTMLGump( 370, 160, 266, 92, true, true, description ); // Description

		// Add objectives
		var objectives = TriggerEvent( 5802, "GetQuestObjectives", quest, questEntry ) || "No objectives available.";
		mainQuestMenu.AddHTMLGump( 370, 260, 266, 81, true, true, objectives );

		// Add rewards
		var rewards = TriggerEvent( 5802, "GetQuestRewards", quest ) || "No rewards available.";
		mainQuestMenu.AddHTMLGump( 370, 350, 266, 100, true, true, rewards );

		// Add a back button to return to the main quest list
		var originalPage = Math.floor( m / questsPerPage ) + 1;
		mainQuestMenu.AddPageButton( 225, 530, 247, 249, originalPage ); // Back Button
	}

	// Add Options Menu Page
	mainQuestMenu.AddPage( totalPages + filteredQuests.length + 1 );
	mainQuestMenu.AddBackground( 350, 120, 306, 400, 1579 ); // Options Menu Background
	mainQuestMenu.AddPageButton( 40, 550, 1588, 1589, totalPages + filteredQuests.length + 1 ); // Options Menu Button
	mainQuestMenu.AddText( 65, 555, 500, "Options Menu" );
	var wordHue = 0;
	if( useTransparentBackground )
	{
		mainQuestMenu.AddCheckerTrans( 350, 120, 306, 400 ); // Transparent background
		wordHue = 1152;
	}
	mainQuestMenu.AddHTMLGump( 47, 230, 266, 300, true, false,
		"Welcome to the Quest Journal Options!<br><br>" +
		"Here you can customize how your quests are displayed and how your journal looks:<br>" +
		"- <b>Quest Categories:</b> Filter your active quests by type, such as Main Story, Side Quests, or Event Quests.<br>" +
		"- <b>Background Transparency:</b> Toggle the background of your journal between solid and transparent for better readability.<br><br>" +
		"Make your selections by clicking the buttons beside." );
	mainQuestMenu.AddText(420, 130, titleHue, "Options Menu" );
	mainQuestMenu.AddBackground(365, 150, 280, 22, 9350);
	mainQuestMenu.AddText( 370, 150, 0, "Misc Options" );

	// Transparency option
	mainQuestMenu.AddCheckbox( 370, 180, 210, useTransparentBackground ? 1 : 0, 2001 );
	mainQuestMenu.AddText( 400, 180, wordHue, "Use Transparent Background" );

	mainQuestMenu.AddBackground( 365, 205, 280, 22, 9350 );
	mainQuestMenu.AddText( 370, 205, 0, "Sort by Quest Category" );

	// Category selection buttons
	for( var l = 0; l < categories.length; l++ )
	{
		var categoryStartY = 230;
		var yPosition = categoryStartY + l * 25;
		var isSelected = ( categories[l] == currentCategory );

		mainQuestMenu.AddButton( 370, yPosition, isSelected ? 211 : 210, isSelected ? 211 : 210, 1, 0, l + 1000 );
		mainQuestMenu.AddText( 400, yPosition, isSelected ? 500 : wordHue, categories[l] );
	}

	var showLoginQuests = ( playerSettings["ShowLoginQuestOffers"] != false );
	mainQuestMenu.AddCheckbox( 370, 450, 210, showLoginQuests ? 1 : 0, 2002 );
	mainQuestMenu.AddText( 400, 450, wordHue, "Show Login Quest Prompts" );

	mainQuestMenu.AddButton( 380, 480, 238, 240, 1, 0, 2000 );//apple button
	mainQuestMenu.AddButton( 550, 480, 247, 249, 1, 0, 1 );//okay button

	mainQuestMenu.Send( socket );
	mainQuestMenu.Free();
}

/** @type { ( pUser: Character ) => void } */
function CompletedQuestsMenu( pUser ) 
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	// Read completed quests
	var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};
	var completedQuests = TriggerEvent( 5800, "ReadArchivedQuests", pUser ) || [];
	var useTransparentBackground = playerSettings["UseTransparentBackground"] || false;

	var uniqueIds = {};
	var uniqueQuests = [];
	for( var i = 0; i < completedQuests.length; i++ )
	{
		var id = (typeof completedQuests[i] === "object") ? completedQuests[i].questID : completedQuests[i];
		if( !uniqueIds[id] )
		{
			uniqueIds[id] = 1; uniqueQuests.push(id);
		}
	}

	var questsPerPage = 10; // Number of quests per page
	var totalPages = Math.ceil( uniqueQuests.length / questsPerPage );
	if( totalPages == 0 )
	{
		totalPages = 1;
	}

	var completedQuestMenu = new Gump();
	completedQuestMenu.AddBackground( 30, 120, 296, 447, 1579 ); // Main background
	var titleHue = 500;
	// Background and transparency toggle
	if( useTransparentBackground )
	{
		completedQuestMenu.AddCheckerTrans( 30, 120, 296, 447 ); // Transparent background
		var titleHue = 1160;
	}
	completedQuestMenu.AddGump( 70, 130, 1577 );
	completedQuestMenu.AddText( 120, 200, titleHue, "Completed Quests" );
	completedQuestMenu.AddButton( 50, 100, 1588, 1589, 1, 0, 20 ); // Active Quests Button
	completedQuestMenu.AddText( 65, 102, 0, "Active Quests" );
	completedQuestMenu.AddButton( 180, 100, 1588, 1589, 1, 0, 21 ); // Completed Quests Button
	completedQuestMenu.AddText( 193, 102, 500, "Completed Quests" );

	// Loop through pages
	for( var page = 0; page < totalPages; page++ ) 
	{
		completedQuestMenu.AddPage( page + 1 );

		if( uniqueQuests.length == 0 && page == 0 )
		{
			completedQuestMenu.AddText( 50, 250, 0, "No completed quests found." );
		}

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
			completedQuestMenu.AddPageButton( 40, buttonY, 1141, 1143, totalPages + i + 1 );
			completedQuestMenu.AddText( 60, buttonY + 5, 0, quest.title + " ( Completed )" );
		}

		// Navigation buttons for the quest list pages
		if( page > 0 )
		{
			completedQuestMenu.AddPageButton( 50, 450, 1144, 1145, page );
			completedQuestMenu.AddText( 80, 455, 0, "Previous" );
		}

		if( page < totalPages - 1 )
		{
			completedQuestMenu.AddPageButton( 200, 450, 1144, 1145, page + 2 );
			completedQuestMenu.AddText( 230, 455, 0, "Next" );
		}
	}

	// Add quest details pages
	for( var i = 0; i < uniqueQuests.length; i++ )
	{
		var questID = uniqueQuests[i];
		var quest = TriggerEvent( 5801, "QuestList", questID );

		if( !quest )
			continue;

		completedQuestMenu.AddPage( totalPages + i + 1 );

		// Details background
		completedQuestMenu.AddBackground( 350, 120, 306, 350, 1579 ); // Details background
		completedQuestMenu.AddText( 420, 130, 500, quest.title );

		// Add description
		completedQuestMenu.AddHTMLGump( 370, 160, 266, 92, true, true, "<b>Description:</b><br>" + ( quest.description || "No description available." ) );

		// Add objectives
		var objectives = "Completed";
		completedQuestMenu.AddHTMLGump( 370, 260, 266, 81, true, true, objectives );

		// Add rewards
		var rewards = TriggerEvent( 5802, "GetQuestRewards", quest ); // Use helper function to build rewards
		completedQuestMenu.AddHTMLGump( 370, 350, 266, 100, true, true, rewards );

		// Add a back button to return to the main quest list
		var originalPage = Math.floor( i / questsPerPage ) + 1;
		completedQuestMenu.AddPageButton( 50, 500, 1144, 1145, originalPage );
	}

	completedQuestMenu.Send( socket );
	completedQuestMenu.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var gumpID = 5823 + 0xffff;

	// Handle button presses for navigating between active and completed quests
	if( pButton == 1 )
	{
		pSock.CloseGump( gumpID, 0 );
		QuestMenu( pUser );
		return;
	}
	else if( pButton == 20 )
	{
		pSock.CloseGump( gumpID, 0 );
		QuestMenu( pUser );
		return;
	}
	else if( pButton == 21 )
	{
		pSock.CloseGump( gumpID, 0 );
		CompletedQuestsMenu( pUser );
		return;
	}

	if( pButton >= 1000 && pButton <= 1006 )
	{
		var categories = ["All Quests", "Main Story Quest", "Side Quests", "Event Quests", "Daily Quests", "World Quests", "City Quests"];
		var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};

		playerSettings["QuestLogCategory"] = categories[pButton - 1000];
		TriggerEvent( 5800, "SavePlayerSettings", pUser, playerSettings );

		pSock.CloseGump( gumpID, 0 );
		QuestMenu( pUser );
		return;
	}

	// Apply button
	if( pButton == 2000 )
	{
		var categories = ["All Quests", "Main Story Quest", "Side Quests", "Event Quests", "Daily Quests", "World Quests", "City Quests"];
		var playerSettings = TriggerEvent( 5800, "ReadPlayerSettings", pUser ) || {};
		var currentCategory = playerSettings["QuestLogCategory"] || "All Quests";

		var checkedButtons = {};
		var tempButton = 0;

		// Collect all checked checkbox IDs from gumpData
		for( var i = 0; i < 16; i++ )
		{
			tempButton = gumpData.getButton( i );

			if( tempButton === null || typeof tempButton == "undefined" )
			{
				break;
			}

			if( tempButton >= 0 )
			{
				checkedButtons[tempButton] = true;
			}
		}

		// Save transparency as absolute state
		playerSettings["UseTransparentBackground"] = ( checkedButtons[2001] ? true : false );

		// Save login quest prompt as absolute state
		playerSettings["ShowLoginQuestOffers"] = ( checkedButtons[2002] ? true : false );

		TriggerEvent( 5800, "SavePlayerSettings", pUser, playerSettings );

		pSock.SysMessage( "Quest journal options updated." );
		pSock.CloseGump( gumpID, 0 );
		QuestMenu( pUser );
		return;
	}
}
