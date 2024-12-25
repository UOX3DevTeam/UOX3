// Utility function to trim whitespace
function manualTrim(  str  )
{
	return str.replace(  /^\s+|\s+$/g, ""  );
}

// Utility function to Array check
function isArray(  obj  ) 
{
	return Object.prototype.toString.call(  obj  ) === "[object Array]";
}

function loadAllQuests(  pUser  )
{
	return ReadQuestProgress(  pUser  );
}

// Save all quests for the player
function serializeQuests( quests )
{
	var serialized = [];
	for( var i = 0; i < quests.length; i++ )
	{
		serialized.push( quests[i].questID + "|" + quests[i].step + "|" + quests[i].progress.join( "," ) + "|" + ( quests[i].completed ? 1 : 0 ) );
	}
	return serialized.join( ";" ); // Separate quests with semicolons
}

// Save progress for a specific quest
function saveQuestProgress( pUser, questID, step, progress, completed )
{
	var questProgressArray = ReadQuestProgress( pUser );
	var updatedQuestProgress = [];
	var playerSerial = pUser.serial.toString(  );

	if( completed )
	{
		// Add the completed quest to the quest log
		AddToQuestLog( pUser, questID );
	}
	else
	{
		var questFound = false;
		for( var i = 0; i < questProgressArray.length; i++ )
		{
			if( questProgressArray[i].questID === questID && questProgressArray[i].serial === playerSerial )
			{
				// Update the quest progress
				questProgressArray[i].step = step;
				questProgressArray[i].progress = progress;
				questProgressArray[i].completed = completed;
				questFound = true;
			}
			// Always add non-completed quests back to the updated list
			updatedQuestProgress.push( questProgressArray[i] );
		}

		// If the quest was not found in progress, add it as a new entry
		if( !questFound ) 
		{
			updatedQuestProgress.push( {serial: playerSerial,questID: questID,step: step,progress: progress,completed: completed,} );
		}
	}

	// Save only the updated progress
	WriteQuestProgress( pUser, updatedQuestProgress );
	pUser.SysMessage( "Quest progress updated." );
}

function AddToQuestLog( player, questID )
{
	// Read the current quest log
	var completedQuests = ReadQuestLog( player );

	// Format: "<player_serial>,<questID>"
	var playerSerial = player.serial.toString(  );
	var questEntry = playerSerial + "," + questID;

	// Check if the quest is already logged
	var questAlreadyLogged = false;
	for( var i = 0; i < completedQuests.length; i++ )
	{
		if( completedQuests[i] === questEntry )
		{
			questAlreadyLogged = true;
			break;
		}
	}

	// If not already logged, add it to the log
	if( !questAlreadyLogged )
	{
		completedQuests.push( questEntry );

		// Write the updated log back to the file
		if( WriteQuestLog( player, completedQuests ) )
		{
			player.SysMessage( "Your completion of quest ID " + questID + " has been logged with serial " + playerSerial + "." );
		}
		else
		{
			player.SysMessage( "Failed to log the completion of quest ID " + questID + "." );
		}
	}
}

// Load progress for a specific quest
function loadQuestProgress( player, questID ) 
{
	var quests = loadAllQuests( player );

	for( var i = 0; i < quests.length; i++ )
	{
		if( quests[i].questID === questID )
		{
			return quests[i];
		}
	}

	// Fetch the chain quest details
	var chainQuests = TriggerEvent( 5801, "getQuests" );
	var quest = null;

	for( var j = 0; j < chainQuests.length; j++ )
	{
		if( chainQuests[j].id === questID )
		{
			quest = chainQuests[j];
			break;
		}
	}

	// Default structure with properly initialized progress
	var progress = {questID: questID,step: 0,progress: [],completed: false};

	// If the quest is found, initialize the `progress` structure
	if( quest )
	{
		var step = quest.steps[progress.step];

		for( var o = 0; o < step.objectives.length; o++ )
		{
			var obj = step.objectives[o];

			if( obj.type === "kill" && obj.targets )
			{
				progress.progress[o] = [];
				for( var t = 0; t < obj.targets.length; t++ )
				{
					progress.progress[o][t] = 0; // Initialize progress for each target
				}
			}
			else
			{
				progress.progress[o] = 0; // Initialize general progress
			}
		}
	}

	return progress;
}

// Check if a step is complete
/*function isStepComplete( step )
{
	for( var i = 0; i < step.objectives.length; i++ )
	{
		var obj = step.objectives[i];

		// Check for multi-target objectives
		if( obj.targets && typeof obj.targets === "object" && obj.targets.length !== undefined )
		{
			for( var t = 0; t < obj.targets.length; t++ )
			{
				if( obj.targets[t].progress < obj.targets[t].count )
				{
					return false; // Not complete if any target is incomplete
				}
			}
		}
		else if( obj.type === "location" && !obj.reached )
		{
			return false; // Location not reached
		}
		else if( obj.progress < obj.count )
		{
			return false; // General objective not complete
		}
	}
	return true; // All objectives complete
}*/

function isStepComplete(step, progressData)
{
	for (var i = 0; i < step.objectives.length; i++)
	{
		var obj = step.objectives[i];

		// Handle multi-target objectives
		if (obj.targets && typeof obj.targets === "object" && obj.targets.length !== undefined)
		{
			for (var t = 0; t < obj.targets.length; t++) {
				var targetProgress = (progressData[i] && progressData[i][t]) || 0;
				if (targetProgress < obj.targets[t].count)
				{
					return false; // Not complete if any target is incomplete
				}
			}
		}
		else if (obj.type === "location" && !obj.reached)
		{
			return false; // Location not reached
		}
		else
		{
			var currentProgress = progressData[i] || 0;
			if (currentProgress < obj.count)
			{
				return false; // General objective not complete
			}
		}
	}

	return true; // All objectives complete
}


function advanceToNextStep( player, quest )
{
	var progress = loadQuestProgress( player, quest.id );

	if( !progress )
	{
		player.SysMessage( "Error: Unable to load progress for quest ID " + quest.id );
		return;
	}

	// Move to the next step
	progress.step++;
	if( progress.step >= quest.steps.length )
	{
		// Quest complete
		progress.completed = true;
		player.SysMessage( "Quest complete: " + quest.name );

		// Log the completed quest
		AddToQuestLog( player, quest.id );

		// Remove from active progress
		removeFromQuestProgress( player, quest.id );

		// Grant rewards need rewroked commited out for now
		var lastStep = quest.steps[quest.steps.length - 1];
		grantRewards( player, lastStep.rewards );
	}
	else
	{
		// Advance to the next step
		var nextStep = quest.steps[progress.step];
		progress.progress = new Array( nextStep.objectives.length ).fill( 0 );

		// Save updated progress
		saveQuestProgress( player, progress.questID, progress.step, progress.progress, progress.completed );

		player.SysMessage( "Step complete! Next step: " + nextStep.description );
	}
}

// Grant rewards to the player
function grantRewards( player, rewards )
{
	// Handle gold rewards
	if( rewards.gold && rewards.gold > 0 )
	{
		CreateDFNItem( player.socket, player, "0x0eed", rewards.gold, "ITEM", true );
		player.SysMessage( "You received " + rewards.gold + " gold!" );
	}

	// Handle item rewards
	if( rewards.items )
	{
		for (var i = 0; i < rewards.items.length; i++)
		{
			var rewardItem = rewards.items[i];
			var amount = rewardItem.amount || 1; // Default to 1 if amount is not specified
			var color = rewardItem.color || 0; // Default to 0 (no color)

			// Create the item(s)
			var item = CreateDFNItem(
				player.socket,
				player,
				rewardItem.id,
				amount,
				"ITEM",
				true, // Spawn in player's pack
				color // Pass the color to CreateDFNItem
			);

			if( item )
			{
				player.SysMessage( "You received " + amount + " " + (item.name || rewardItem.id) + "!" );
			}
			else
			{
				player.SysMessage( "Failed to create reward item: " + rewardItem.id );
			}
		}
	}
}

// Remove a completed quest from active progress
function removeFromQuestProgress( player, questID )
{
	var allQuests = loadAllQuests( player );
	var updatedQuests = [];

	for( var i = 0; i < allQuests.length; i++ )
	{
		if( allQuests[i].questID !== questID )
		{
			updatedQuests.push( allQuests[i] );
		}
	}

	// Save the updated progress
	WriteQuestProgress( player, updatedQuests );
}

// Write Quest Log back to file
function WriteQuestLog( pUser, myArray )
{
	// Create a new file object
	var mFile = new UOXCFile(  );
	var userAccount = pUser.account;
	var fileName = "QuestLog_" + userAccount.id + ".jsdata";

	mFile.Open( fileName, "w", "Quests" ); // Open file for Writing
	if( mFile != null )
	{
		// Loop through each entry in myArray and save it to a new line
		for( let i = 0; i < myArray.length; i++ )
		{
			mFile.Write( myArray[i] + "\n" );
		}

		// Close and free the file
		mFile.Close(  )
		mFile.Free(  );
		return true;
	}

	return false;
}

function ReadQuestLog( pUser )
{
	// Create a new file object
	var mFile = new UOXCFile(  );
	var userAccount = pUser.account;
	var fileName = "QuestLog_" + userAccount.id + ".jsdata";

	// Create an array variable to store contents of Quest System
	var myArray = [];

	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length(  ) >= 0 )
	{
		// Read until we reach the end of the file, or until we find a match for the quester
		while ( !mFile.EOF(  ) )
		{
			// Read a line of text from the file
			var line = mFile.ReadUntil( "\n" );
			if( line.length <= 1 || line == "" )
			{
				continue;
			}

			// Store each line from file in myArray
			myArray.push( line );
		}
		mFile.Close(  );

		// Frees memory allocated by file object
		mFile.Free(  );
	}
	//pUser.SysMessage( myArray.length )
	return myArray;
}

function WriteQuestProgress( player, questProgressArray )
{
	var mFile = new UOXCFile(  );
	var userAccount = player.account;
	var fileName = "QuestProgress_" + userAccount.id + ".jsdata";

	mFile.Open( fileName, "w", "Quests" ); // Open file for writing
	if( mFile )
	{
		for( var i = 0; i < questProgressArray.length; i++ )
		{
			var progressEntry = questProgressArray[i];
			// Format: <serial>|<questID>|<step>|<progress>|<completed>
			var serializedProgress = player.serial + "|" + progressEntry.questID + "|" + progressEntry.step + "|" + progressEntry.progress.join( "," ) + "|" + ( progressEntry.completed ? "1" : "0" );
				mFile.Write( serializedProgress + "\n" );
		}
		mFile.Close(  );
		mFile.Free(  );
		return true;
	}
	return false;
}

function ReadQuestProgress( player )
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestProgress_" + userAccount.id + ".jsdata";

	var questProgressArray = [];
	var playerSerial = player.serial.toString(); // Get the current character's serial

	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length() >= 0 )
	{
		while( !mFile.EOF() )
		{
			var line = mFile.ReadUntil( "\n" );
			if( line.length <= 1 || line === "" )
			{
				continue;
			}

			// Parse each line into a quest progress object
			var parts = line.split( "|" );
			if( parts.length >= 5 )
			{
				// Only include quests for the current character's serial
				if( parts[0] === playerSerial )
				{
					questProgressArray.push( {serial: parts[0],questID: parts[1],step: parseInt(parts[2], 10),progress: parts[3].split(",").map(function(value) {return parseFloat(value) || 0;}),completed: parts[4] === "1",} );
				}
			}
		}
		mFile.Close();
		mFile.Free();
	}

	return questProgressArray;
}