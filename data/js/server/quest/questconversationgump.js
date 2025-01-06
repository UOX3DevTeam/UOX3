const DebugMessages = false;// Enable debug messages

function QuestConversationGump( pUser, npcTarget, questID )
{
	var socket = pUser.socket;
	var playerSerial = pUser.serial; // Use player serial to filter quests

	// Fetch quest details using the provided quest ID
	var quest = TriggerEvent( 5801, "QuestList", questID );

	if( !quest ) 
	{
		socket.SysMessage( "This quest does not exist." );
		return;
	}

	// Fetch player's progress for the current quest and filter by player serial
	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", pUser );
	var currentQuestProgress = null;

	for( var i = 0; i < questProgressArray.length; i++ ) 
	{
		var questEntry = questProgressArray[i];
		if( questEntry.questID == questID && questEntry.serial == playerSerial) 
		{
			currentQuestProgress = questEntry;
			break;
		}
	}

	// Determine the description to display
	var description = quest.description; // Default description
	if( currentQuestProgress )
	{
		if( currentQuestProgress.completed )
		{
			description = quest.complete || "You have completed this quest!";
		}
		else
		{
			description = quest.uncomplete || "You have not completed this quest yet.";
		}
	}

	// Handle chain quests: If this is a chain quest and there is a nextQuestID
	if( quest.nextQuestID && currentQuestProgress && currentQuestProgress.completed ) 
	{
		var nextQuest = TriggerEvent( 5801, "QuestList", quest.nextQuestID );
		if( nextQuest )
		{
			description += "<br><br><b>Next Quest Available:</b> " + nextQuest.title + "<br>" + nextQuest.description;
		}
	}

	// Create the gump
	var gump = new Gump();
	gump.AddPage( 0 );
	gump.NoClose();
	gump.AddBackground( 30, 120, 296, 520, 1579 ); // Background
	gump.AddGump( 70, 130, 1577 ); // Decorative gump

	// Quest title
	var title = "<center>" + quest.title + "</center>";
	gump.AddHTMLGump( 75, 205, 200, 30, true, false, title ); // Title

	// Description
	gump.AddHTMLGump( 50, 230, 264, 100, true, true, description ); // Description

	// Get objectives with progress and add to the gump
	if( !currentQuestProgress || !currentQuestProgress.completed )
	{
		var objectives = GetQuestObjectives( quest, currentQuestProgress );
		gump.AddHTMLGump( 50, 350, 264, 100, true, true, objectives ); // Objectives
	}

	// Get rewards and add to the gump
	var rewards = GetQuestRewards( quest );
	gump.AddHTMLGump( 50, 470, 264, 100, true, true, rewards ); // Rewards

	// Add buttons based on quest status
	if( !currentQuestProgress )
	{
		gump.AddButton( 60, 600, 0x2EE0, 0x2EE2, 1, 0, 1 ); // Start quest button
		gump.AddButton( 220, 600, 0x2EF2, 0x2EF4, 1, 0, 2 ); // refuse button
	}
	else if( currentQuestProgress.completed )
	{
		gump.AddButton( 60, 600, 0x2EE0, 0x2EE2, 1, 0, 3 ); // Turn in quest button
	}
	else 
	{
		gump.AddButton( 50, 600, 0x2EF5, 0x2EF7, 1, 0, 4 ); // resign quest button
		gump.AddButton( 220, 600, 0x2EEC, 0x2EEE, 1, 0, 0 ); // close button
	}

	// Send the gump to the player
	gump.Send( socket );
	gump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var questNpc = CalcCharFromSer( parseInt( pUser.GetTag( "questNpcSerial" )));
	var initialQuestID = parseInt( questNpc.GetTag( "QuestID" ), 10 );
	var playerQuestID = resolvePlayerQuestID( pUser, initialQuestID );
	var quest = TriggerEvent( 5801, "QuestList", playerQuestID );

	switch ( pButton ) 
	{
		case 0: // Close gump
			break;
		case 1: // Accept quest
			TriggerEvent( 5800, "StartQuest", pUser, playerQuestID );
			pUser.SoundEffect( 0x5B4, true );
			break;

		case 2: // Refuse quest
			questNpc.TextMessage( quest.refuse );
			break;
		case 3: // Turn in quest
			if( playerQuestID )
			{
				var playerPack = pUser.pack;
				if( playerPack.totalItemCount >= playerPack.maxItems )
				{
					pSock.SysMessage( GetDictionaryEntry( 1819, pSock.language )); // Your backpack cannot hold any more items!
					return;
				}

				var turnInSuccess = processQuestTurnIn( pUser, playerQuestID ); // Handle item turn-in
				if( turnInSuccess )
				{
					TriggerEvent( 5800, "CompleteQuest", pUser, playerQuestID ); // Complete the quest
					pUser.SoundEffect(0x5B5, true);
				}
				else
				{
					pSock.SysMessage( "You need to have all required quest items to turn in this quest." );
				}
			}
			break;
		case 4: // Resign Quest
			ResignQuest( pUser, playerQuestID );
			manageQuestItems( pUser, playerQuestID, false );
			pUser.SoundEffect( 0x5B3, true );
			break;
			default:break
	}
}

function ResignQuest( player, questID )
{
	var socket = player.socket;
	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", player );
	var newQuestProgressArray = [];
	var questFound = false;

	var quest = TriggerEvent( 5801, "QuestList", questID );

	for( var i = 0; i < questProgressArray.length; i++ ) 
	{
		var questEntry = questProgressArray[i];

		// Skip the quest that needs to be resigned, effectively removing it
		if (questEntry.questID == questID && questEntry.serial == player.serial)
		{
			questFound = true;

			// Handle skill training quest resignation
			if( quest.type == "skillgain" )
			{
				player.SetTag( "AcceleratedSkillGain", null ); // Remove the tag
				player.RemoveScriptTrigger( 5811 ); // Remove the quest skill gain script trigger
				socket.SysMessage( "You have stopped accelerated training for " + GetSkillName( quest.targetSkill ) + "." );
			}

			socket.SysMessage( "You have resigned from the quest: " + quest.title );
		} 
		else
		{
			newQuestProgressArray.push( questEntry );
		}
	}

	if( !questFound )
	{
		socket.SysMessage( "You are not currently on this quest." );
		return false;
	}

	// Write back the updated quest progress, excluding the resigned quest
	TriggerEvent( 5800, "WriteQuestProgress", player, newQuestProgressArray );

	socket.SysMessage( "The quest has been completely removed from your progress." );
	return true;
}

function manageQuestItems( player, questID, mark )
{
	var socket = player.socket;
	var pack = player.pack; // Get the player's backpack

	if( !ValidateObject( pack ))
	{
		player.SysMessage( "You do not have a backpack." );
		return;
	}

	var currentItem;
	var quest = TriggerEvent( 5801, "QuestList", questID ); // Fetch quest details

	if( !quest || ( !quest.targetItems && !quest.deliveryItem )) 
	{
		socket.SysMessage( "This quest does not have item requirements." );
		return;
	}

	// Iterate through all items in the player's backpack
	for( currentItem = pack.FirstItem(); !pack.FinishedItems(); currentItem = pack.NextItem())
	{
		if( !ValidateObject( currentItem ))
		{
			continue;
		}

		// Handle target items for collection quests
		if( quest.targetItems )
		{
			for( var i = 0; i < quest.targetItems.length; i++ )
			{
				var targetItem = quest.targetItems[i];

				// Check if the item matches the quest requirement
				if( String( currentItem.sectionID ) === String( targetItem.itemID ))
				{
					if( mark )
					{
						// Mark item as a quest item
						if( !currentItem.GetTag( "QuestItem" ))
						{
							currentItem.SetTag( "saveColor", currentItem.color );
							currentItem.color = 0x04ea; // Orange hue
							currentItem.isNewbie = true;
							currentItem.isDyeable = false;
							currentItem.SetTag( "QuestItem", true );
							currentItem.AddScriptTrigger( 5806 ); // Quest Item script trigger
							socket.SysMessage( "Marked item as a quest item: " + targetItem.name );
						}
					}
					else
					{
						// Unmark item as a quest item
						if( currentItem.GetTag( "QuestItem" ))
						{
							var questItemColor = currentItem.GetTag( "saveColor" );
							currentItem.color = questItemColor;
							currentItem.isNewbie = false;
							currentItem.isDyeable = true;
							currentItem.SetTag( "QuestItem", null );
							currentItem.RemoveScriptTrigger( 5806 ); // Quest Item script trigger
							socket.SysMessage( "Unmarked item as a quest item: " + targetItem.name );
						}
					}
				}
			}
		}

		// Handle delivery items
		if( quest.type === "delivery" && quest.deliveryItem )
		{
			if( String( currentItem.sectionID ) === String( quest.deliveryItem.itemID ))
			{
				if( mark )
				{
					// Mark delivery item as a quest item
					if( !currentItem.GetTag( "QuestItem" ))
					{
						currentItem.SetTag( "saveColor", currentItem.color );
						currentItem.color = 0x04ea; // Orange hue
						currentItem.isNewbie = true;
						currentItem.isDyeable = false;
						currentItem.SetTag( "QuestItem", true );
						currentItem.AddScriptTrigger( 5806 ); // Quest Item script trigger
						socket.SysMessage( "Marked delivery item as a quest item: " + quest.deliveryItem.name );
					}
				}
				else
				{
					// If resigning, delete the delivery item
					currentItem.Delete(  );
					socket.SysMessage( "Deleted delivery item: " + quest.deliveryItem.name );
				}
			}
		}
	}
}

function GetQuestObjectives( quest, questProgress ) 
{
	var objectives = "";

	// Items to collect
	if( quest.targetItems && quest.targetItems.length > 0 ) 
	{
		objectives += "<b>Items to Collect:</b><br>";
		for( var i = 0; i < quest.targetItems.length; i++ ) 
		{
			var targetItem = quest.targetItems[i];
			var itemName = targetItem.name || "Unknown Item"; // Use `name` if available, fallback to "Unknown Item"
			var collected = ( questProgress && questProgress.collectedItems[targetItem.itemID] ) || 0;

			objectives += "- " + itemName + ": " + collected + "/" + targetItem.amount + "<br>";
		}
	}

	// Creatures to kill
	if( quest.targetKills && quest.targetKills.length > 0 )
	{
		objectives += "<b>Creatures to Kill:</b><br>";
		for( var j = 0; j < quest.targetKills.length; j++ )
		{
			var targetKill = quest.targetKills[j];
			var killed = ( questProgress && questProgress.harvestKills[targetKill.npcID] ) || 0;

			objectives += "- " + targetKill.npcID + ": " + killed + "/" + targetKill.amount + "<br>";
		}
	}

	// Time limit
	if( quest.timeLimit )
	{
		var minutes = Math.floor( quest.timeLimit / 60 ); // Convert seconds to minutes
		var seconds = quest.timeLimit % 60; // Get remaining seconds
		objectives += "<b>Time Limit:</b><br>";
		objectives += "- " + minutes + " minute( s ) and " + seconds + " second( s )<br>";
	}

	if( quest.type === "delivery" ) 
	{
		objectives += "<b>Delivery Quest:</b><br>";

		// Check for delivery item details
		if( quest.deliveryItem )
		{
			var itemName = quest.deliveryItem.name || "Unknown Item";
			objectives += "- Item: " + itemName + " ( " + quest.deliveryItem.amount + " )<br>";
		}

		// Check for targetDelivery details
		if( quest.targetDelivery && quest.targetDelivery.name )
		{
			objectives += "- NPC: " + quest.targetDelivery.name + "<br>";
		}
		else if( quest.targetDelivery && quest.targetDelivery.sectionID ) 
		{
			objectives += "- NPC: " + quest.targetDelivery.sectionID + "<br>"; // Fallback to sectionID if name is missing
		}

		// Include location if provided
		if( quest.targetDelivery.location )
		{
			var loc = quest.targetDelivery.location;
			objectives += "- Location: X=" + loc.x + ", Y=" + loc.y + ", Z=" + loc.z + "<br>";
		}
	}

	// Skill objectives
	if( quest.type === "skillgain" && quest.targetSkill !== undefined && quest.maxSkillPoints !== undefined )
	{
		objectives += "<b>Skill Training:</b><br>";
		var skillName = GetSkillName( quest.targetSkill ) || "Unknown Skill"; // Use a helper to get the skill name
		var currentProgress = ( questProgress && questProgress.skillProgress ) || 0; // Default to 0 if not started
		var maxProgress = quest.maxSkillPoints / 10; // Convert max skill points to in-game scale
		var regionName = quest.regionName || "Unknown Region"; // Use `regionName` if available

		objectives += "- Train " + skillName + " in " + regionName + ": " + ( currentProgress / 10 ).toFixed( 1 ) + "/" + maxProgress.toFixed( 1 ) + "<br>";
	}

	// Daily Quest
	if( quest.dailyQuest == 1 )
	{
		objectives += "<b>Daily Quest:</b><br>";
		var lastAccepted = questProgress && questProgress.lastAccepted ? questProgress.lastAccepted : Date.now();
		var resetTime = quest.resetDailyTime || 24; // Reset interval in hours
		var currentTime = Date.now();

		var timeSinceAccepted = ( currentTime - lastAccepted ) / ( 3600 * 1000 ); // Time in hours
		var hoursLeft = Math.max(0, Math.ceil(resetTime - timeSinceAccepted ));

		if( hoursLeft > 0 )
		{
			objectives += "- Hours until reset: " + hoursLeft + " hour(s)<br>";
		}
		else
		{
			objectives += "- Quest is ready to reset!<br>";
		}
	}

	if( objectives === "" )
	{
		objectives = "No specific objectives.";
	}

	return objectives;
}

function GetSkillName( skillID )
{
	var skillNames = [
		"alchemy", "anatomy", "animallore", "itemid", "armslore", "parrying", "begging",
		"blacksmith", "bowcraft", "peacemaking", "camping", "carpentry", "cartography",
		"cooking", "detectinghidden", "enticement", "evaluatingintelligence", "healing",
		"fishing", "forensics", "herding", "hiding", "provocation", "inscription",
		"lockpicking", "magery", "magicresistance", "tactics", "snooping", "musicianship", "poisoning",
		"archery", "spiritSpeak", "stealing", "tailoring", "animaltaming", "tasteID",
		"tinkering", "tracking", "veterinary", "swordsmanship", "macefighting",
		"fencing", "wrestling", "lumberjacking", "mining", "meditation", "stealth",
		"removetrap", "necromancy", "focus", "chivalry", "bushido", "ninjitsu", "spellweaving"
	];

	return skillNames[skillID] || "unknown skill";
}

function GetQuestRewards( quest )
{
	// List rewards
	var rewards = "";

	if( quest.rewards && quest.rewards.length > 0 )
	{
		rewards += "<b>Rewards:</b><br>";
		for( var k = 0; k < quest.rewards.length; k++ )
		{
			var reward = quest.rewards[k];
			var rewardName = reward.name || "Unknown Reward"; // Use `name` if available, fallback to "Unknown Reward"
			rewards += "- " + reward.amount + " " + rewardName + "<br>";
		}
	}
	else
	{
		rewards = "No rewards specified.";
	}

	return rewards;
}

function processQuestTurnIn( player, questID )
{
	// Fetch the quest details
	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest )
	{
		player.SysMessage( "This quest does not exist." );
		return false;
	}

	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", player );
	var questProgress = null;

	// Find the quest progress entry
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		if( questProgressArray[i].questID === questID )
		{
			questProgress = questProgressArray[i];
			break;
		}
	}

	// Handle skill gain quests
	if( quest.type === "skillgain" ) 
	{
		if( questProgress.skillProgress >= quest.maxSkillPoints )
		{
			player.SysMessage( "You have completed the skill training for this quest!" );
			return true;
		} 
		else
		{
			player.SysMessage( "You still need to improve your skill. Current progress: " + ( questProgress.skillProgress / 10 ).toFixed( 1 ) + "/" + ( quest.maxSkillPoints / 10 ).toFixed( 1 ) );
			return false;
		}
	}

	// Check if the quest is a "kill" type and validate completion
	if( quest.type === "kill" || quest.type === "timekills" || quest.type === "multi" )
	{
		if( !questProgress || !questProgress.harvestKills )
		{
			player.SysMessage( "You have not completed the kill objectives for this quest." );
			return false;
		}

		// Validate kill objectives
		for( var i = 0; i < quest.targetKills.length; i++ )
		{
			var targetKill = quest.targetKills[i];
			if( ( questProgress.harvestKills[targetKill.npcID] || 0 ) < targetKill.amount )
			{
				player.SysMessage( "You have not killed enough " + targetKill.npcID + "s." );
				return false;
			}
		}

		// If kill objectives are complete
		player.SysMessage( "You have completed the kill objectives for this quest." );
		return true;
	}

	// If the quest is not a kill quest, check for item turn-in
	if( quest.type === "collect" || quest.type === "timecollect" || quest.type === "multi" )
	{
		if (!quest.targetItems || quest.targetItems.length === 0)
		{
			player.SysMessage( "This quest does not require item turn-in." );
			return false;
		}

		// Fetch quest items from the player's backpack
		var questItems = findQuestItems(player, questID);
		var requiredItems = []; // Clone target items manually

		for( var i = 0; i < quest.targetItems.length; i++ )
		{
			var targetItem = quest.targetItems[i];
			requiredItems.push({ itemID: targetItem.itemID, amount: targetItem.amount });
		}

		// Validate collected items against required objectives
		for( var i = 0; i < questItems.length; i++ )
		{
			var item = questItems[i];
			for( var j = 0; j < requiredItems.length; j++ )
			{
				if( String( item.sectionID) === String( requiredItems[j].itemID ))
				{
					var toDeduct = Math.min( item.amount, requiredItems[j].amount );
					requiredItems[j].amount -= toDeduct; // Deduct the required amount
					item.amount -= toDeduct; // Deduct from the item's stack

					if( item.amount === 0 )
					{
						item.Delete(); // Remove the item if the stack is empty
					}

					if( requiredItems[j].amount === 0 )
					{
						break; // Move to the next required item
					}
				}
			}
		}

		// Check if all required items are collected
		for( var k = 0; k < requiredItems.length; k++ )
		{
			if( requiredItems[k].amount > 0 )
			{
				player.SysMessage("You are still missing some items for this quest.");
				return false;
			}
		}

		player.SysMessage( "All quest items have been turned in successfully." );
		return true;
	}

	// Handle delivery quests
	if( quest.type === "delivery" )
	{
		// Validate NPC
		if( quest.targetDelivery.npcID !== questNpc.serial )
		{
			player.SysMessage( "This is not the correct NPC for delivery." );
			return false;
		}

		// Find the delivery item in the backpack
		var deliveryItems = findQuestItems( player, questID );
		var foundItem = false;

		for( var i = 0; i < deliveryItems.length; i++ )
		{
			var item = deliveryItems[i];
			if( String( item.sectionID ) === String( quest.deliveryItem.itemID ) && item.amount >= quest.deliveryItem.amount )
			{
				foundItem = true;

				// Remove the item from the player's backpack
				if( item.amount > quest.deliveryItem.amount ) 
				{
					item.amount -= quest.deliveryItem.amount;
				}
				else 
				{
					item.Delete();
				}
				break;
			}
		}

		if( !foundItem )
		{
			player.SysMessage( "You don't have the required item to deliver." );
			return false;
		}

		player.SysMessage( "You have delivered the required item!" );
		return true;
	}

	// If quest type doesn't match known types
	player.SysMessage( "This quest type is not supported for turn-in." );
	return false;
}

function findQuestItems( player, questID )
{
	var questItems = [];
	var pack = player.pack; // Get the player's backpack

	if( !ValidateObject( pack ))
	{
		player.SysMessage( "You do not have a backpack." );
		return questItems; // Return an empty array if no backpack is found
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || ( !quest.targetItems && !quest.deliveryItem ))
	{
		player.SysMessage( "No target items or delivery items found for this quest." );
		return questItems;
	}

	// Build the list of required item IDs
	var requiredItemIDs = [];
	if( quest.targetItems ) 
	{
		for( var i = 0; i < quest.targetItems.length; i++ )
		{
			requiredItemIDs.push( String( quest.targetItems[i].itemID ));
		}
	}

	// Add delivery item ID if it's a delivery quest
	if( quest.type === "delivery" && quest.deliveryItem )
	{
		requiredItemIDs.push( String( quest.deliveryItem.itemID ));
	}

	var currentItem;

	// Iterate through all items in the player's backpack
	for( currentItem = pack.FirstItem(); !pack.FinishedItems(); currentItem = pack.NextItem() )
	{
		if( !ValidateObject( currentItem ))
		{
			continue;
		}

		// Check if the item matches the quest and has the "QuestItem" tag
		if( currentItem.GetTag( "QuestItem" ) && requiredItemIDs.indexOf( String( currentItem.sectionID )) !== -1 ) 
		{
			questItems.push( currentItem );
		}
	}

	return questItems;
}

function onCharDoubleClick( pUser, questNpc ) 
{
	QuestNpcInterAction( pUser, questNpc );
	return false;
}

function QuestNpcInterAction( pUser, questNpc )
{
	var gumpID = 5822 + 0xffff;
	var socket = pUser.socket;
	pUser.SetTag( "questNpcSerial", questNpc.serial );

	// Validate the targeted object and player
	if( !ValidateObject( pUser ) || !ValidateObject( questNpc ))
	{
		socket.SysMessage( "Invalid target or player." );
		return false;
	}

	// Check if the player is within range
	if( !questNpc.InRange( pUser, 2 ))
	{
		pUser.SysMessage( "You are too far away." );
		return false;
	}

	// Check if the NPC is a delivery recipient
	var deliveryQuestID = parseInt( questNpc.GetTag( "DeliveryQuestID" ), 10 );
	if( deliveryQuestID )
	{
		if( TriggerEvent( 5800, "CheckQuest", pUser, deliveryQuestID ))
		{ // Validate quest eligibility
			processDeliveryQuest(pUser, questNpc, deliveryQuestID);
		}
		else
		{
			return false;
		}
		// Handle delivery quest
		//if( processDeliveryQuest( pUser, questNpc, deliveryQuestID )) 
		//{
		///	return; // Delivery quest handled successfully
		//}
	}

	// Fetch the initial quest ID from the NPC's tag
	var initialQuestID = parseInt( questNpc.GetTag( "QuestID" ), 10 );
	if( !initialQuestID ) 
	{
		pUser.SysMessage( "This NPC has no quest assigned." );
		return false;
	}

	// Resolve the player's current quest ID in the chain
	var playerQuestID = resolvePlayerQuestID( pUser, initialQuestID );

	if( !playerQuestID )
	{
		questNpc.TurnToward( pUser );
		questNpc.TextMessage( "You have completed all quests" );
		return false;
	}

	// Check if the quest is already completed
	var archivedQuests = TriggerEvent( 5800, "ReadArchivedQuests", pUser );

	if( DebugMessages ) 
	{
		pUser.SysMessage( "Archived quests loaded: " + archivedQuests.length );
	}

	for( var i = 0; i < archivedQuests.length; i++ )
	{
		var archivedQuestID = parseInt( archivedQuests[i], 10 ); // Ensure it's treated as an integer
		if( DebugMessages )
		{
			pUser.SysMessage( "Checking archived quest: " + archivedQuestID );
		}
		if( archivedQuestID === playerQuestID ) // Compare consistently
		{
			targObj.TurnToward( pUser );
			targObj.TextMessage( "I'm sorry, I have nothing for you at this time." );
			return false;
		}
	}

	// Fetch player's progress for the current quest
	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", pUser );
	var currentQuestProgress = null;

	for( var i = 0; i < questProgressArray.length; i++ ) 
	{
		if( questProgressArray[i].questID === playerQuestID ) 
		{
			currentQuestProgress = questProgressArray[i];
			break;
		}
	}

	// Show the quest conversation gump
	questNpc.TurnToward( pUser );
	socket.CloseGump( gumpID, 0 );
	QuestConversationGump( pUser, questNpc, playerQuestID );
	return;
}

function processDeliveryQuest( player, questNpc, deliveryQuestID )
{
	// Fetch the quest details
	var quest = TriggerEvent( 5801, "QuestList", deliveryQuestID );
	if( !quest || quest.type !== "delivery" )
	{
		questNpc.TextMessage( "This is not a delivery quest." );
		return false;
	}

	// Ensure the NPC matches the quest's recipient
	if( questNpc.sectionID !== quest.targetDelivery.sectionID )
	{
		questNpc.TextMessage( "I am not the intended recipient of this delivery." );
		return false;
	}

	// Check if the player has the required item
	var requiredItem = quest.deliveryItem;
	var questItems = findQuestItems( player, deliveryQuestID );
	var itemDelivered = false;

	for( var i = 0; i < questItems.length; i++ )
	{
		if( String( questItems[i].sectionID ) === String( requiredItem.itemID ))
		{
			// Check if enough items are delivered
			if( questItems[i].amount >= requiredItem.amount )
			{
				itemDelivered = true;
				break;
			}
		}
	}

	if( !itemDelivered )
	{
		player.SysMessage( "You do not have the required item to deliver." );
		return false;
	}

	// Remove the item from the player's inventory
	for( var j = 0; j < questItems.length; j++ )
	{
		if( String( questItems[j].sectionID ) === String( requiredItem.itemID )) 
		{
			if( questItems[j].amount > requiredItem.amount )
			{
				questItems[j].amount -= requiredItem.amount;
			}
			else
			{
				questItems[j].Delete();
			}
			break;
		}
	}

	// Update the player's quest progress and notify
	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		if( questProgressArray[i].questID === deliveryQuestID )
		{
			questProgressArray[i].completed = true;
			break;
		}
	}

	TriggerEvent( 5800, "WriteQuestProgress", player, questProgressArray );

	// Notify the player and complete the quest
	player.SysMessage( "You have delivered the required item!" );
	TriggerEvent( 5800, "CompleteQuest", player, deliveryQuestID );
	return true;
}

function resolvePlayerQuestID( player, initialQuestID )
{
	var archivedQuests = TriggerEvent( 5800, "ReadArchivedQuests", player );

	if( !archivedQuests || !isArray( archivedQuests ))
	{
		//player.SysMessage( "Archived Quests: Invalid data format." );
		return initialQuestID;
	}

	var currentQuestID = parseInt( initialQuestID, 10 );

	for( var questID = currentQuestID; questID; )
	{
		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( !quest ) 
		{
			break; // No further quests in the chain
		}

		// Skip daily quests that haven't reset
		if (quest.dailyQuest == 1) 
		{
			for (var i = 0; i < archivedQuests.length; i++) 
			{
				if (archivedQuests[i].questID == questID)
				{
					var lastCompleted = archivedQuests[i].lastCompleted || 0;
					var resetTime = quest.resetDailyTime || 24; // Default reset time is 24 hours
					if ((Date.now() - lastCompleted) < resetTime * 3600 * 1000)
					{
						questID = parseInt(quest.nextQuestID, 10); // Move to the next quest
						continue;
					}
				}
			}
		}


		if( !isQuestArchived( archivedQuests, questID ))
		{
			return questID; // Return the first uncompleted quest
		}

		questID = parseInt( quest.nextQuestID, 10 );
	}

	return null; // All quests completed
}

// Helper function to check if a value exists in an array
function isQuestArchived( array, value )
{
	value = parseInt( value, 10 ); // Ensure value is a number
	for( var i = 0; i < array.length; i++ ) 
	{
		if( parseInt( array[i], 10 ) === value )
		{
			return true;
		}
	}
	return false;
}

// Helper function to check if a value is an array
function isArray( value ) 
{
	return Object.prototype.toString.call( value ) === "[object Array]";
}

function onContextMenuRequest( socket, targObj )
{
	// handle your own packet with context menu here
	var pUser = socket.currentChar;
	var offset = 12;
	var numEntries = 2;

	// Prepare packet
	var toSend = new Packet();
	var packetLen = ( 12 + ( numEntries * 8 ));
	toSend.ReserveSize(  packetLen );
	toSend.WriteByte(  0, 0xBF );
	toSend.WriteShort(  1, packetLen );
	toSend.WriteShort(  3, 0x14 ); // subCmd
	toSend.WriteShort(  5, 0x0001 ); // 0x0001 for 2D client, 0x0002 for KR (  maybe this needs to be 0x0002?  )
	toSend.WriteLong(  7, targObj.serial  );
	toSend.WriteByte(  11, numEntries ); // Number of entries

	toSend.WriteShort(  offset, 0x000A );    // Unique ID
	toSend.WriteShort(  offset += 2, 6156 ); // Quest Conversation
	toSend.WriteShort(  offset += 2, 0x0020 ); // Flag, color enabled
	toSend.WriteShort(  offset += 2, 0x03E0 ); // Hue of text

	offset += 2; // for each additional entry

	toSend.WriteShort(  offset, 0x000b );    // Unique ID
	toSend.WriteShort(  offset += 2, 6155 ); // Cancel Quest
	toSend.WriteShort(  offset += 2, 0x0020 ); // Flag, color enabled
	toSend.WriteShort(  offset += 2, 0x03E0 ); // Hue of text

	//Send packet
	socket.Send( toSend );
	toSend.Free();

	return false;
}

function onContextMenuSelect( socket, targObj, popupEntry )
{
	var pUser = socket.currentChar;

	// Validate the targeted object and player
	if( !ValidateObject( pUser ) || !ValidateObject( targObj ))
	{
		pUser.SysMessage( "Invalid target or player." );
		return false;
	}

	switch ( popupEntry )
	{
		case 0x000A: // Quest Conversation
			{
				// Validate the targeted object and player
				if( !ValidateObject( pUser ) || !ValidateObject( targObj ))
				{
					socket.SysMessage( "Invalid target or player." );
					return false;
				}

				// Check if the player is within range
				if( !targObj.InRange( pUser, 2 ))
				{
					pUser.SysMessage( "You are too far away." );
					return false;
				}

				QuestNpcInterAction( pUser, targObj );
			}
			break;

		case 0x000B: // Cancel Quest (Optional)
			{
				var initialQuestID = parseInt( targObj.GetTag( "QuestID" ), 10 );
				var playerQuestID = resolvePlayerQuestID( pUser, initialQuestID );
				var quest = TriggerEvent( 5801, "QuestList", playerQuestID );
				targObj.TextMessage( quest.refuse );
				pUser.SoundEffect(  0x5B4, true  );
			}
			break;

		default:
			return true; // Let the default context menu handling proceed
	}

	return false; // Prevent default context menu handling for handled entries
}