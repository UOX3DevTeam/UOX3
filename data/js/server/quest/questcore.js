const DebugMessages = false;
function startQuest( player, questID )
{
	var questProgressArray = ReadQuestProgress( player );

	if( !CheckQuest( player, questID ))
	{
		return; // Stop execution if the quest cannot proceed
	}

	// Initialize objectives
	var collectedItems = {};
	var harvestKills = {};

	var quest = TriggerEvent( 5801, "QuestList", questID );

	if( quest.targetItems )
	{
		for( var i = 0; i < quest.targetItems.length; i++ )
		{
			var targetItem = quest.targetItems[i];
			collectedItems[targetItem.itemID] = 0; // Start with 0 collected
		}
	}

	if( quest.targetKills )
	{
		for( var i = 0; i < quest.targetKills.length; i++ )
		{
			var targetKill = quest.targetKills[i];
			harvestKills[targetKill.npcID] = 0; // Start with 0 kills
		}
	}

	// Add the delivery item to the player's backpack if it's a delivery quest
	if( quest.type === "delivery" && quest.deliveryItem )
	{
			var package = CreateDFNItem( player.socket, player, quest.deliveryItem.itemID, quest.deliveryItem.amount, "ITEM", true )
			if( ValidateObject( package )) 
			{
				// Set the item's name to the delivery item name from the quest
				package.name = quest.deliveryItem.name || "Unknown Package";

				// Mark it as a quest item
				package.SetTag( "saveColor", package.color );
				package.color = 0x04ea; // orange hue
				package.isDyeable = false;
				package.isNewbie = true;
				package.SetTag( "QuestItem", true );
				package.SetTag( "delivery", true );
				package.AddScriptTrigger( 5806 );// Quest Item script trigger
				player.SysMessage( "You have received a " + quest.deliveryItem.name + " for delivery." );
			} 
			else
			{
				player.SysMessage( "Failed to create the delivery item." );
			}
	}

	if( quest && quest.type == "skillgain" && player.GetTag( "AcceleratedSkillGain" ) == false )
	{
		player.SetTag( "AcceleratedSkillGain", quest.targetSkill );
		player.AddScriptTrigger(5811);// Quest skill gain script trigger
	}
	else
	{	
		player.SysMessage( "You are already under the effect of an accelerated skillgain scroll." )
	}

	// Add new quest to progress
	questProgressArray.push({
		serial: player.serial,
		questID: questID,
		questProgress: 0, // General progress
		harvestKills: harvestKills, // Kill objectives
		collectedItems: collectedItems, // Item objectives
		skillProgress: 0, // Initialize skill progress
		targetSkill: quest.targetSkill || -1, // Target skill for "skillgain"
		targetRegion: quest.targetRegion || 0, // Target region for "skillgain"
		maxSkillPoints: quest.maxSkillPoints || 50.0, // Max skill points for "skillgain"
		startTime: quest.timeLimit ? Date.now() : 0, // Timed quests
		timeLimit: quest.timeLimit ? quest.timeLimit * 1000 : 0,
		completed: false,
		questTurnIn: false,
		nextQuestID: quest.nextQuestID || null
	});

	WriteQuestProgress( player, questProgressArray );

	if( DebugMessages )
	{
		player.SysMessage( "You have started the quest: " + quest.title );
	}

	player.SysMessage( "You have accepted the Quest." );

	// Start the timer if the quest is timed
	if( quest.timeLimit )
	{
		player.StartTimer( quest.timeLimit * 1000, questID, true ); // Timer in milliseconds
		var minutes = Math.floor( quest.timeLimit / 60 ); // Convert total seconds to minutes
		var seconds = quest.timeLimit % 60; // Get remaining seconds
		player.SysMessage( "You have " + minutes + " minute( s ) and " + seconds + " second( s ) to complete this quest." );
	}
}

function CheckQuest( player, questID )
{
	// Fetch quest details using the QuestList trigger
	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest )
	{
		player.SysMessage( "Quest not found." );
		return false; // Indicate that the quest cannot proceed
	}

	// Read the player's active and archived quest data
	var questProgressArray = ReadQuestProgress( player );
	var archivedQuests = ReadArchivedQuests( player );

	// Check if the quest is already in progress
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		if( questProgressArray[i].questID === questID )
		{
			player.SysMessage( "You are already working on this quest." );
			return false; // Indicate that the quest cannot proceed
		}
	}

	// Check if the quest has already been completed
	for( var i = 0; i < archivedQuests.length; i++ ) 
	{
		if( archivedQuests[i].questID === questID )
		{
			player.SysMessage( "You have already completed this quest." );
			return false; // Indicate that the quest cannot proceed
		}
	}

	if( DebugMessages ) 
	{
		// All checks passed, the quest can proceed
		player.SysMessage( "You are eligible for this quest." );
	}
	return true;
}

function onTimer( timerObj, timerID )
{
	var player = timerObj; // Assuming timerObj is the player object
	var questProgressArray = LoadQuestProgress( player );

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.questID === timerID && !questEntry.completed )
		{
			var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

			TriggerEvent( 5802, "manageQuestItems", player, questEntry.questID, false );

			// Time has expired
			player.SysMessage( "You have failed the timed quest: " + quest.title );

			// Log the failed quest
			LogFailedQuest( player, questEntry );

			questProgressArray.splice( i, 1 ); // Remove the failed quest
			SaveQuestProgress( player, questProgressArray );
			return;
		}
	}
}

function updateQuestProgress( player, questID, identifier, progressValue, type )
{
	var questProgressArray = ReadQuestProgress( player );
	var questUpdated = false;

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.questID === questID ) 
		{
			var quest = TriggerEvent( 5801, "QuestList", questID ); // Fetch quest data
			var allObjectivesCompleted = true;

			// Check item collection objectives
			if( quest.type === "collect" || quest.type === "timecollect" || quest.type === "multi" )
			{
				if( quest.targetItems )
				{
					for( var j = 0; j < quest.targetItems.length; j++ )
					{
						var target = quest.targetItems[j];

						if( DebugMessages )
						{
							player.SysMessage( "Checking target itemID: " + target.itemID );
						}

						if( String( target.itemID ) === String( identifier ))
						{
							questEntry.collectedItems[identifier] = ( questEntry.collectedItems[identifier] || 0 ) + progressValue;

							// Cap the collected amount to the target amount
							if( questEntry.collectedItems[identifier] > target.amount )
							{
								questEntry.collectedItems[identifier] = target.amount;
							}

							if( DebugMessages )
							{
								player.SysMessage( "Collected " + questEntry.collectedItems[identifier] + "/" + target.amount + " " + identifier );
							}
						}

						// Check if all items are collected
						if( !questEntry.collectedItems || ( questEntry.collectedItems[target.itemID] || 0 ) < target.amount )
						{
							allObjectivesCompleted = false;
						}
					}
				}
			}

			// Check kill objectives
			if( quest.type === "kill" || quest.type === "timekills" || quest.type === "multi" )
			{
				if( quest.targetKills )
				{
					for( var k = 0; k < quest.targetKills.length; k++ )
					{
						var target = quest.targetKills[k];

						if( DebugMessages )
						{
							player.SysMessage( "Checking target npcID: " + target.npcID );
						}

						if( String( target.npcID ) === String( identifier ))
						{
							questEntry.harvestKills[identifier] = ( questEntry.harvestKills[identifier] || 0 ) + progressValue;

							// Cap the kill count to the target amount
							if( questEntry.harvestKills[identifier] > target.amount )
							{
								questEntry.harvestKills[identifier] = target.amount;
							}

							if( DebugMessages )
							{
								player.SysMessage( "Killed " + questEntry.harvestKills[identifier] + "/" + target.amount + " " + identifier );
							}
						}

						// Check if all kills are completed
						if( !questEntry.harvestKills || ( questEntry.harvestKills[target.npcID] || 0 ) < target.amount )
						{
							allObjectivesCompleted = false;
						}
					}
				}
			}

			if( quest.type === "delivery" )
			{
				// Check if the player delivered the item to the correct NPC
				if( identifier === quest.targetDelivery.npcID ) 
				{
					// Fetch the delivery item from the player's backpack
					var pack = player.pack; // Get the player's backpack
					var hasItem = false;

					// Iterate through all items in the player's backpack
					for( currentItem = pack.FirstItem(  ); !pack.FinishedItems(  ); currentItem = pack.NextItem(  ))
					{
						if( !ValidateObject( currentItem )) 
						{
							continue;
						}

						// Check if the item matches the delivery item and has the correct tag
						if( currentItem.GetTag( "QuestItem" ) && String( currentItem.sectionID ) === String( quest.deliveryItem.itemID ) && currentItem.amount >= quest.deliveryItem.amount )
						{
							hasItem = true;

							// Remove the required quantity from the player's backpack
							if( currentItem.amount > quest.deliveryItem.amount )
							{
								currentItem.amount -= quest.deliveryItem.amount; // Reduce item stack
							}
							else
							{
								currentItem.Delete(); // Remove the item completely
							}
							break;
						}
					}

					if( hasItem )
					{
						questEntry.completed = true;
						player.SysMessage( "You have successfully delivered the item!" );
					}
					else
					{
						player.SysMessage( "You don't have the required item to deliver." );
					}
				}
				else
				{
					player.SysMessage( "This is not the correct NPC to deliver the item." );
				}
			}

			// Check skill gain objectives
			if( quest.type === "skillgain" && quest.targetSkill === identifier )
			{
				// Update skill progress, initializing if undefined
				questEntry.skillProgress = ( questEntry.skillProgress || 0 ) + progressValue;

				// Cap the skill progress to max skill points
				if( questEntry.skillProgress >= quest.maxSkillPoints ) 
				{
					// Complete the skill gain objective
					player.SetTag( "AcceleratedSkillGain", null ); // Remove the tag
					player.RemoveScriptTrigger( 5811 ); // Remove quest skill gain script trigger
					questEntry.completed = true; // Mark the quest as completed
				}
				else
				{
					// Notify player of ongoing progress
					player.SysMessage( "Skill progress: " + ( questEntry.skillProgress / 10 ).toFixed(1) + "/" + ( quest.maxSkillPoints / 10 ).toFixed(1) );
					allObjectivesCompleted = false;
				}
			}



			// Mark quest as completed if all objectives are met
			if( allObjectivesCompleted )
			{
				if( quest.questTurnIn == 1 )
				{
					questEntry.completed = true;
					if( quest.type === "skillgain" && quest.oncomplete )
					{
						player.SysMessage( quest.oncomplete );
					}
					else 
					{
						player.SysMessage( "You've completed the quest! Don't forget to collect your reward." );
					}
					WriteQuestProgress( player, questProgressArray );
				}
				else
				{
					questEntry.completed = true;
					if (quest.type === "skillgain" && quest.oncomplete )
					{
						player.SysMessage( quest.oncomplete );
					}
					else
					{
						player.SysMessage( "You've completed the quest! Don't forget to collect your reward." );
					}
					WriteQuestProgress( player, questProgressArray );
					completeQuest( player, questID );
				}
				return;
			}

			// Ensure progress is saved after each update
			questUpdated = true;
			break;
		}
	}

	if( questUpdated )
	{
		WriteQuestProgress( player, questProgressArray );
	}
	else
	{
		player.SysMessage( "No progress updated for the quest." );
	}

	return questProgressArray; // Return updated progress
}


function completeQuest( player, questID )
{
	var questProgressArray = ReadQuestProgress( player );
	var newQuestProgressArray = [];

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.questID === questID )
		{
			// Ensure the quest is completed
			if( questEntry.completed )
			{
				// Handle rewards
				var quest = TriggerEvent( 5801, "QuestList", questID );

				// Ensure the quest is completed
				player.SysMessage( "Congratulations! You have completed the quest: " + quest.title );
				DoStaticEffect( player.x, player.y, player.z, 0x376A, 0x40, 0x16, false );

				if( quest.rewards )
				{
					for( var j = 0; j < quest.rewards.length; j++ )
					{
						var reward = quest.rewards[j];

						if( reward.type === "gold" )
						{
							// Reward gold
							CreateDFNItem( player.socket, player, "0x0eed", reward.amount, "ITEM", true ); // 0x0eed is gold
							player.SysMessage( "You received " + reward.amount + " gold!" );
						}
						else if( reward.type === "item" )
						{
							// Reward item
							CreateDFNItem( player.socket, player, reward.itemID, reward.amount, "ITEM", true );
							player.SysMessage( "You received " + reward.amount + " of item " + reward.name + "!" );
						}
						else if( reward.type === "karma" )
						{
							// Reward karma
							player.karma += reward.amount; // Assuming `karma` is a property of the player
							player.SysMessage( "You gained " + reward.amount + " karma!" );
						}
						else if( reward.type === "fame" )
						{
							// Reward fame
							player.fame += reward.amount; // Assuming `fame` is a property of the player
							player.SysMessage( "You gained " + reward.amount + " fame!" );
						}
						else
						{
							player.SysMessage( "Unknown reward type: " + reward.type );
						}
					}
				}

				// Archive the completed quest
				ArchiveCompletedQuest( player, questEntry );

				// Cancel the quest timer for the correct questID
				player.KillJSTimer( questID, 5820 );

				// Mark that the quest was completed and turned in
				questCompleted = true;
			}
			else
			{
				player.SysMessage( "You haven't completed the quest yet." );
				return;
			}
		}
		else
		{
			newQuestProgressArray.push( questEntry ); // Add incomplete quests to the new array
		}
	}

	// Save updated progress ( excluding the completed quest if archived )
	WriteQuestProgress( player, newQuestProgressArray );

	// Start the next quest in the chain, if available, after turn-in
	if( questCompleted )
	{
		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( quest && quest.nextQuestID )
		{
			var nextQuest = TriggerEvent( 5801, "QuestList", quest.nextQuestID );
			if( nextQuest )
			{
				player.SysMessage( "A new quest has been unlocked: " + nextQuest.title );
				startQuest( player, parseInt( quest.nextQuestID ));
			}
		}
	}
}

function onCreatureKilled( creature, player )
{
	var questProgressArray = ReadQuestProgress( player );

	if( DebugMessages ) 
	{
		// Debug: Log the collected item's sectionID
		player.SysMessage( "npc sectionID: " + creature.sectionID );
	}

	for( var i = 0; i < questProgressArray.length; i++ ) 
	{
		var questEntry = questProgressArray[i];
		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest && ( quest.type === "kill" || quest.type === "timekills" || quest.type === "multi" ) && !questEntry.completed )
		{
			for( var j = 0; j < quest.targetKills.length; j++ )
			{
				var target = quest.targetKills[j];

				if( DebugMessages )
				{
					// Debug: Check each target item
					player.SysMessage( "Checking target npcID: " + target.npcID );
				}

				if( target.npcID == creature.sectionID ) 
				{
					if( DebugMessages )
					{
						player.SysMessage( "killing npc: " + creature.sectionID );
					}

					updateQuestProgress( player, questEntry.questID, creature.sectionID, 1, "kill" );
					return true; // Exit after updating progress
				}
			}

			return true;
		}
	}
}

function onItemCollected(player, item, isToggledOff)
{
	// Default the isToggledOff value
	if( typeof isToggledOff === "undefined" ) 
	{
		isToggledOff = false;
	}

	var questProgressArray = ReadQuestProgress( player );

	if( DebugMessages )
	{
		player.SysMessage( "Item sectionID: " + item.sectionID );
	}

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest && ( quest.type === "collect" || quest.type === "timecollect" || quest.type === "multi" ) && !questEntry.completed )
		{
			for( var j = 0; j < quest.targetItems.length; j++ )
			{
				var target = quest.targetItems[j];

				if( DebugMessages )
				{
					player.SysMessage( "Checking target itemID: " + target.itemID );
				}

				if( String( target.itemID ) === String( item.sectionID ))
				{
					var currentCount = questEntry.collectedItems[item.sectionID] || 0;
					var remaining = target.amount - currentCount;

					if( isToggledOff )
					{
						// Decrease the count when untoggled, ensuring it doesn't go below 0
						if( currentCount > 0 ) 
						{
							var amountToRemove = Math.min( item.amount, currentCount );
							updateQuestProgress( player, questEntry.questID, item.sectionID, -amountToRemove, "collect" );

							var questItemColor = item.GetTag( "saveColor" );
							item.color = questItemColor;
							item.isNewbie = false;
							item.isDyeable = true;
							item.SetTag( "QuestItem", null );
							item.RemoveScriptTrigger( 5806 ); // Quest Item script trigger

							player.SysMessage( "You removed " + amountToRemove + " Quest Item(s)." );
						}
						else
						{
							player.SysMessage( "Cannot decrease further. Current count is 0." );
						}
					}
					else
					{
						// Increase the count when toggled on
						if( remaining > 0 )
						{
							var amountToAdd = Math.min( item.amount, remaining );
							updateQuestProgress( player, questEntry.questID, item.sectionID, amountToAdd, "collect" );

							item.SetTag( "saveColor", item.color );
							item.color = 0x04ea; // orange hue
							item.isDyeable = false;
							item.isNewbie = true;
							item.SetTag( "QuestItem", true );
							item.AddScriptTrigger( 5806 ); // Quest Item script trigger

							player.SysMessage( "You set " + amountToAdd + " item(s) to Quest Item status." );
						}
						else
						{
							player.SysMessage( "Cannot collect more. Target amount reached: " + target.amount );
						}
					}
					return; // Exit after updating progress
				}
			}
		}
	}

	player.SysMessage( "Item does not match any quest requirements." );
}

function AccelerateSkillGain( pPlayer, skill, skillGainAmount )
{
	var activeQuests = ReadQuestProgress( pPlayer );

	for( var i = 0; i < activeQuests.length; i++ ) 
	{
		var questEntry = activeQuests[i];
		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest && quest.type === "skillgain" && !questEntry.completed ) 
		{
			if( pPlayer.region.id === quest.targetRegion && skill === quest.targetSkill ) 
			{
				var currentSkillPoints = pPlayer.baseskills[skill];
				var acceleratedGain = RandomNumber( quest.minPoint, quest.MaxPoint );

				if(( currentSkillPoints + acceleratedGain ) >= quest.maxSkillPoints )
				{
					updateQuestProgress( pPlayer, questEntry.questID, skill, quest.maxSkillPoints - currentSkillPoints, "skillgain" );
				}
				else
				{
					// Apply accelerated skill gain and update progress
					pPlayer.AddSkill( skill, acceleratedGain, false );
					updateQuestProgress( pPlayer, questEntry.questID, skill, acceleratedGain, "skillgain" );
				}

				return true;
			}
		}
	}

	return true;
}

//Helper Function for skillnames
function GetSkillName(skillID) {
	var skillNames = [
		"alchemy", "anatomy", "animallore", "itemid", "armslore", "parrying", "begging",
		"blacksmithy", "bowcraft", "peacemaking", "camping", "carpentry", "cartography",
		"cooking", "detectinghidden", "enticing", "evaluatingintelligence", "healing",
		"fishing", "forensics", "herding", "hiding", "provocation", "inscription",
		"lockpicking", "magery", "magicresistance", "tactics", "musicianship", "poisoning",
		"archery", "spiritSpeak", "stealing", "tailoring", "animaltaming", "tasteID",
		"tinkering", "tracking", "veterinary", "swordsmanship", "macefighting",
		"fencing", "wrestling", "lumberjacking", "mining", "meditation", "stealth",
		"removetrap", "necromancy", "focus", "chivalry", "bushido", "ninjitsu", "spellweaving"
	];

	return skillNames[skillID] || "unknown skill";
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//							Save/Read Functions									//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//Not used this are wrappers just incase there is some sort logging need
// Save progress to file
function SaveQuestProgress( player, questProgressArray )
{
	return WriteQuestProgress( player, questProgressArray );
}

// Load progress from file
function LoadQuestProgress( player )
{
	return ReadQuestProgress( player );
}

function LogFailedQuest( player, failedQuest )
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var failedLogFileName = "FailedQuests_" + userAccount.id + ".jsdata";

	mFile.Open( failedLogFileName, "a", "Quests" ); // Append mode
	if( mFile )
	{
		// Serialize collectedItems
		var collectedItemsStr = "";
		if( failedQuest.collectedItems )
		{
			for( var key in failedQuest.collectedItems )
			{
				if( failedQuest.collectedItems.hasOwnProperty( key ))
				{
					if( collectedItemsStr.length > 0 )
					{
						collectedItemsStr += ",";
					}
					collectedItemsStr += key + ":" + failedQuest.collectedItems[key];
				}
			}
		}

		// Serialize harvestKills
		var harvestKillsStr = "";
		if( failedQuest.harvestKills )
		{
			for( var key in failedQuest.harvestKills )
			{
				if( failedQuest.harvestKills.hasOwnProperty( key ))
				{
					if( harvestKillsStr.length > 0 )
					{
						harvestKillsStr += ",";
					}
					harvestKillsStr += key + ":" + failedQuest.harvestKills[key];
				}
			}
		}

		// Write the failed quest details
		var failedEntry =
			"Serial=" + ( failedQuest.serial || "undefined" ) + "\n" +
			"QuestID=" + ( failedQuest.questID || "undefined" ) + "\n" +
			"QuestProgress=" + ( failedQuest.questProgress || 0 ) + "\n" +
			"HarvestKills=" + harvestKillsStr + "\n" +
			"CollectedItems=" + collectedItemsStr + "\n" +
			"StartTime=" + ( failedQuest.startTime || 0 ) + "\n" +
			"TimeLimit=" + ( failedQuest.timeLimit || 0 ) + "\n" +
			"Completed=0\n" +
			"QuestTurnIn=0\n\n";

		mFile.Write( failedEntry );
		mFile.Close();
		mFile.Free();
	}
}

function ReadFailedQuests( player )
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var failedFileName = "FailedQuests_" + userAccount.id + ".jsdata";

	var failedQuests = [];
	mFile.Open( failedFileName, "r", "Quests" );
	if( mFile && mFile.Length() >= 0 )
	{
		var currentQuestID = null;
		var currentSerial = null;

		while ( !mFile.EOF())
		{
			var line = mFile.ReadUntil( "\n" );
			line = manualTrim( line ); // Remove leading/trailing spaces

			// Skip empty lines
			if( line === "" )
			{
				continue;
			}

			// Split the line into key and value
			var parts = line.split( "=" );
			if( parts.length === 2 )
			{
				var key = manualTrim( parts[0] ).toLowerCase(  );
				var value = manualTrim( parts[1] );

				if( key === "questid" )
				{
					currentQuestID = parseInt( value, 10 ); // Parse the QuestID
				} 
				else if( key === "serial" )
				{
					currentSerial = parseInt( value, 10 ); // Parse the Serial
				}

				// If we reach the end of an entry and Serial matches the player, save the QuestID
				if( key === "failed" && value === "1" && currentQuestID !== null && currentSerial === player.serial )
				{
					failedQuests.push( currentQuestID );
					currentQuestID = null; // Reset for next entry
					currentSerial = null; // Reset for next entry
				}
			}
		}
		mFile.Close();
		mFile.Free();
	} 
	else
	{
		player.SysMessage( "No failed quests found." );
	}

	return failedQuests;
}


function ArchiveCompletedQuest( player, completedQuest )
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var archiveFileName = "QuestArchive_" + userAccount.id + ".jsdata";

	var quest = TriggerEvent( 5801, "QuestList", completedQuest.questID ); // Fetch the quest details
	var nextQuestID = quest ? quest.nextQuestID || "null" : "null"; // Safely resolve nextQuestID

	mFile.Open( archiveFileName, "a", "Quests" ); // Append mode
	if( mFile )
	{
		// Serialize collectedItems
		var collectedItemsStr = "";
		if( completedQuest.collectedItems )
		{
			for( var key in completedQuest.collectedItems )
			{
				if( completedQuest.collectedItems.hasOwnProperty( key ))
				{
					if( collectedItemsStr.length > 0 )
					{
						collectedItemsStr += ",";
					}
					collectedItemsStr += key + ":" + completedQuest.collectedItems[key];
				}
			}
		}

		// Serialize harvestKills
		var harvestKillsStr = "";
		if( completedQuest.harvestKills )
		{
			for( var key in completedQuest.harvestKills )
			{
				if( completedQuest.harvestKills.hasOwnProperty( key ))
				{
					if( harvestKillsStr.length > 0 )
					{
						harvestKillsStr += ",";
					}
					harvestKillsStr += key + ":" + completedQuest.harvestKills[key];
				}
			}
		}

		// Add skill progress
		var skillProgressStr = "";
		if( quest && quest.type === "skillgain" )
		{
			skillProgressStr = "SkillProgress=" + ( completedQuest.skillProgress || 0 ) + "\n" +
				"MaxSkillPoints=" + ( quest.maxSkillPoints || 0 ) + "\n" +
				"TargetSkill=" + ( quest.targetSkill || "null" ) + "\n";
		}

		// Add delivery progress
		var deliveryProgressStr = "";
		if( quest && quest.type === "delivery" )
		{
			deliveryProgressStr = "DeliveryProgress=" + (completedQuest.deliveryProgress || 0 ) + "\n" +
				"DeliveryItem=" + ( completedQuest.deliveryItem || "null" ) + "\n" +
				"TargetDeliveryNPC=" + ( completedQuest.targetDeliveryNPC || "null" ) + "\n";
		}

		// Write the serialized data to the archive file
		var archiveEntry =
			"Serial=" + ( completedQuest.serial || "undefined" ) + "\n" +
			"QuestID=" + ( completedQuest.questID || "undefined" ) + "\n" +
			"NextQuestID=" + nextQuestID + "\n" + // Save the next quest in the chain
			"QuestProgress=" + ( completedQuest.questProgress || 0 ) + "\n" +
			"CollectedItems=" + collectedItemsStr + "\n" +
			"HarvestKills=" + harvestKillsStr + "\n" +
			skillProgressStr + // Add skill progress
			deliveryProgressStr + // Add delivery progress
			"StartTime=" + ( completedQuest.startTime || 0 ) + "\n" + // Save startTime
			"TimeLimit=" + ( completedQuest.timeLimit || 0 ) + "\n" + // Save timeLimit
			"Completed=1\n" +
			"QuestTurnIn=1\n\n";

		mFile.Write( archiveEntry );
		mFile.Close();
		mFile.Free();
	}
}

function ReadArchivedQuests( player ) 
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var archiveFileName = "QuestArchive_" + userAccount.id + ".jsdata";

	var archivedQuests = [];
	mFile.Open( archiveFileName, "r", "Quests" );
	if( mFile && mFile.Length() >= 0 )
	{
		var currentQuestID = null;
		var currentSerial = null;

		while ( !mFile.EOF())
		{
			var line = mFile.ReadUntil( "\n" );
			line = manualTrim( line ); // Remove leading/trailing spaces

			// Skip empty lines
			if( line === "" ) 
			{
				// Check if we reached the end of an entry
				if( currentQuestID !== null && currentSerial === player.serial ) 
				{
					archivedQuests.push( currentQuestID ); // Add the quest ID
				}
				currentQuestID = null; // Reset for the next entry
				currentSerial = null; // Reset for the next entry
				continue;
			}

			// Split the line into key and value
			var parts = line.split( "=" );
			if( parts.length === 2 ) 
			{
				var key = manualTrim( parts[0] ).toLowerCase(  );
				var value = manualTrim( parts[1] );

				if( key === "questid" ) 
				{
					currentQuestID = parseInt( value, 10 ); // Parse quest ID
				} else if( key === "serial" )
				{
					currentSerial = parseInt( value, 10 ); // Parse player serial
				}

				// Add quest ID if marked as completed
				if( key === "completed" && value === "1" && currentQuestID !== null && currentSerial === player.serial ) 
				{
					archivedQuests.push( currentQuestID );
				}
			}
		}

		mFile.Close();
		mFile.Free();
	} 
	else
	{
		if( DebugMessages )
		{
			player.SysMessage( "No archived quest file found or unable to read." );
		}
	}

	return archivedQuests; // Return array of quest IDs
}


function WriteQuestProgress( player, questProgressArray )
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestProgress_" + userAccount.id + ".jsdata";

	mFile.Open( fileName, "w", "Quests" );
	if( mFile )
	{
		for( var i = 0; i < questProgressArray.length; i++ )
		{
			var progressEntry = questProgressArray[i];

			if( DebugMessages ) 
			{
				var debugStr = "Serial=" + progressEntry.serial +
					", QuestID=" + progressEntry.questID +
					", QuestProgress=" + progressEntry.questProgress +
					", Completed=" + progressEntry.completed +
					", harvestKills=";
				if( progressEntry.harvestKills )
				{
					for( var key in progressEntry.harvestKills )
					{
						if( progressEntry.harvestKills.hasOwnProperty( key )) 
						{
							debugStr += key + ":" + progressEntry.harvestKills[key] + " ";
						}
					}
				}

				player.SysMessage( "Saving progressEntry: " + debugStr );
			}

			// Serialize kills
			var killsStr = "";
			if( progressEntry.harvestKills )
			{
				for( var killKey in progressEntry.harvestKills )
				{
					if( progressEntry.harvestKills.hasOwnProperty( killKey ))
					{
						if( killsStr.length > 0 ) 
						{
							killsStr += ",";
						}
						killsStr += killKey + ":" + progressEntry.harvestKills[killKey];
					}
				}
			}

			// Serialize items
			var collectedItemsStr = "";
			if( progressEntry.collectedItems )
			{
				for( var itemKey in progressEntry.collectedItems )
				{
					if( progressEntry.collectedItems.hasOwnProperty( itemKey )) 
					{
						if( collectedItemsStr.length > 0 )
						{
							collectedItemsStr += ",";
						}
						collectedItemsStr += itemKey + ":" + progressEntry.collectedItems[itemKey];
					}
				}
			}

			if( DebugMessages )
			{
				// Debug: Print serialized kills
				player.SysMessage( "Writing harvestKills: " + killsStr );
			}

			// Write all required fields
			var formattedEntry =
				"Serial=" + ( progressEntry.serial || "undefined" ) + "\n" +
				"QuestID=" + ( progressEntry.questID || "undefined" ) + "\n" +
				"QuestProgress=" + ( progressEntry.questProgress || 0 ) + "\n" +
				"HarvestKills=" + killsStr + "\n" +
				"CollectedItems=" + collectedItemsStr + "\n" +
				"SkillProgress=" + ( progressEntry.skillProgress || 0 ) + "\n" + // Skill progress
				"TargetSkill=" + ( progressEntry.targetSkill || -1 ) + "\n" +   // Target skill ID
				"TargetRegion=" + ( progressEntry.targetRegion || 0 ) + "\n" + // Target region ID
				"MaxSkillPoints=" + ( progressEntry.maxSkillPoints || 50.0 ) + "\n" + // Max skill points
				"StartTime=" + ( progressEntry.startTime || 0 ) + "\n" + // Start time
				"TimeLimit=" + ( progressEntry.timeLimit || 0 ) + "\n" + // Time limit
				"Completed=" + ( progressEntry.completed ? "1" : "0" ) + "\n" +
				"QuestTurnIn=" + ( progressEntry.questTurnIn ? "1" : "0" ) + "\n" +
				"NextQuestID=" + ( progressEntry.nextQuestID || "undefined" ) + "\n\n";
			mFile.Write( formattedEntry );
		}
		mFile.Close();
		mFile.Free();
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
	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length() >= 0 )
	{
		var currentEntry = null;
		while ( !mFile.EOF())
		{
			var line = mFile.ReadUntil( "\n" );

			// Normalize line: Remove unexpected characters
			line = manualTrim( line.replace( /[^\x20-\x7E]/g, "" ));

			if( DebugMessages ) 
			{
				// Debug: Print normalized line
				player.SysMessage( "Normalized line: " + line );
			}

			if( line == "" )
			{
				if( currentEntry )
				{
					finalizeQuestEntry( currentEntry, player );
					questProgressArray.push( currentEntry );
					currentEntry = null;
				}
				continue;
			}

			var parts = line.split( "=" );
			if( parts.length == 2 )
			{
				if( !currentEntry )
					currentEntry = {};

				var key = manualTrim( parts[0] ).toLowerCase(  ); // Convert key to lowercase
				var value = manualTrim( parts[1] );

				if( DebugMessages )
				{
					// Debug: Log parsed key-value pairs
					player.SysMessage( "Parsed key: " + key + ", value: " + value );
				}

				currentEntry[key] = value;
			}
		}

		// Handle the last entry
		if( currentEntry )
		{
			finalizeQuestEntry( currentEntry, player );
			questProgressArray.push( currentEntry );
		}

		mFile.Close();
		mFile.Free();
	}
	else
	{
		if( DebugMessages )
		{
			player.SysMessage( "Failed to open or read quest progress file." );
		}
	}
	return questProgressArray;
}

// Helper function to finalize and normalize quest entry
function finalizeQuestEntry( entry, player )
{
	entry.questID = parseInt( entry.questid || "0", 10 );
	entry.completed = entry.completed === "1";
	entry.questTurnIn = entry.questturnin === "1";
	entry.startTime = parseInt( entry.starttime || "0", 10 );
	entry.timeLimit = parseInt( entry.timelimit || "0", 10 );
	entry.skillProgress = parseFloat( entry.skillprogress || "0.0" );
	entry.targetSkill = parseInt( entry.targetskill || "-1", 10 );
	entry.targetRegion = parseInt( entry.targetregion || "0", 10 );
	entry.maxSkillPoints = parseFloat( entry.maxskillpoints || "50.0" );

	processCollectedItems(entry, player);
	processKills(entry, player);
}

// Helper function to process collectedItems
function processCollectedItems( entry, player )
{
	entry.collectedItems = {};
	var collectedItemsStr = entry.collecteditems || ""; // Use consistent key

	if( DebugMessages )
	{
		// Debug: Log the raw collected items string
		player.SysMessage( "Processing collected items: " + collectedItemsStr );
	}

	if( collectedItemsStr === "" ) 
	{
		if( DebugMessages ) 
		{
			player.SysMessage( "No collected items to process." );
		}
		return;
	}

	var collectedItems = collectedItemsStr.split( "," );
	for( var i = 0; i < collectedItems.length; i++ )
	{
		var pair = collectedItems[i].split( ":" );
		if( pair.length === 2 )
		{
			var key = manualTrim( pair[0] );
			var value = parseInt( manualTrim( pair[1] ), 10 );
			entry.collectedItems[key] = value;

			if( DebugMessages )
			{
				// Debug: Log each parsed collected item entry
				player.SysMessage( "Parsed collected item - ID: " + key + ", Count: " + value );
			}
		}
	}

	if( DebugMessages )
	{
		// Debug: Log the final processed collected items object
		player.SysMessage( "Final collectedItems object: " + objectToString( entry.collectedItems ));
	}
}

function processKills( entry, player )
{
	// Initialize harvestKills as an object if not already
	entry.harvestKills = {};

	// Use the correct key to extract the raw kills string
	var killsStr = entry.harvestkills || ""; // Note: lowercase to match file format

	if( DebugMessages )
	{
		// Debug: Log the raw kills string
		player.SysMessage( "Processing kills: " + killsStr );
	}

	// If killsStr is empty, there’s nothing to process
	if( killsStr === "" )
	{
		if( DebugMessages )
		{
			player.SysMessage( "No kills to process." );
		}
		return;
	}

	var harvestKills = killsStr.split( "," );
	for( var i = 0; i < harvestKills.length; i++ ) 
	{
		var pair = harvestKills[i].split( ":" );
		if( pair.length === 2 )
		{
			var key = manualTrim( pair[0] );
			var value = parseInt( manualTrim( pair[1] ), 10 );
			entry.harvestKills[key] = value;

			if( DebugMessages )
			{
				// Debug: Log each parsed kill entry
				player.SysMessage( "Parsed kill - NPC: " + key + ", Count: " + value );
			}
		}
	}

	if( DebugMessages ) 
	{
		// Debug: Log the reconstructed harvestKills object
		player.SysMessage( "Final harvestKills object: " + objectToString( entry.harvestKills ));
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//					Utility Functions											//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Utility function to trim whitespace
function manualTrim( str )
{
	return str.replace( /^\s+|\s+$/g, "" );
}

//Custom Object-to-String Formatter
function objectToString( obj )
{
	var result = "";
	for( var key in obj )
	{
		if( obj.hasOwnProperty( key ))
		{
			if( result.length > 0 )
			{
				result += ", ";
			}
			result += key + ": " + obj[key];
		}
	}
	return "{" + result + "}";
}