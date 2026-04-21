/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( pUser: Character, npcTarget: Character, questID: number ) => void } */
function QuestConversationGump( pUser, npcTarget, questID )
{
	if( !ValidateObject( pUser ) || !ValidateObject( npcTarget ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	var playerSerial = pUser.serial; // Use player serial to filter quests

	// Fetch quest details using the provided quest ID
	var quest = TriggerEvent( 5801, "QuestList", questID );

	if( !quest ) 
	{
		socket.SysMessage( GetDictionaryEntry( 19602, socket.language ));//This quest does not exist.
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
			description = quest.complete;
		}
		else
		{
			description = quest.uncomplete;
		}
	}

	// Handle chain quests: If this is a chain quest and there is a nextQuestID
	// Chain preview: show what would unlock next (branching-aware)
	if (currentQuestProgress && currentQuestProgress.completed)
	{
		var nextQuestID = ResolveNextQuestID_Any(pUser, quest);
		if (nextQuestID > 0)
		{
			var nextQuest = TriggerEvent(5801, "QuestList", nextQuestID);
			if (nextQuest)
			{
				description += "<br><br><b>Next Quest Available:</b> " +
					nextQuest.title + "<br>" + nextQuest.description;
			}
		}
	}

	var questConvoMenu = new Gump();
	questConvoMenu.AddPage( 0 );
	questConvoMenu.NoClose();
	questConvoMenu.AddBackground( 30, 120, 296, 520, 1579 ); // Background
	questConvoMenu.AddGump( 70, 130, 1577 ); // Decorative gump

	// Quest title
	var title = "<center>" + quest.title + "</center>";
	questConvoMenu.AddHTMLGump( 75, 205, 200, 30, true, false, title ); // Title
	questConvoMenu.AddHTMLGump( 50, 230, 264, 100, true, true, description ); // Description

	if( !currentQuestProgress || !currentQuestProgress.completed )
	{
		var objectives = GetQuestObjectives( quest, currentQuestProgress );
		questConvoMenu.AddHTMLGump( 50, 350, 264, 100, true, true, objectives ); // Objectives
	}

	var rewards = GetQuestRewards( quest );
	questConvoMenu.AddHTMLGump( 50, 470, 264, 100, true, true, rewards ); // Rewards

	if( !currentQuestProgress )
	{
		questConvoMenu.AddButton( 60, 600, 0x2EE0, 0x2EE2, 1, 0, 1 ); // Start quest button
		questConvoMenu.AddButton( 220, 600, 0x2EF2, 0x2EF4, 1, 0, 2 ); // refuse button
	}
	else if( currentQuestProgress.completed )
	{
		questConvoMenu.AddButton( 60, 600, 0x2EE0, 0x2EE2, 1, 0, 3 ); // Turn in quest button
	}
	else 
	{
		questConvoMenu.AddButton( 50, 600, 0x2EF5, 0x2EF7, 1, 0, 4 ); // resign quest button
		questConvoMenu.AddButton( 220, 600, 0x2EEC, 0x2EEE, 1, 0, 0 ); // close button
	}

	// Lock the questID this gump represents (so onGumpPress doesn't re-resolve)
	pUser.SetTempTag( "QuestConversationQuestID", questID );

	questConvoMenu.Send( socket );
	questConvoMenu.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	if( !ValidateObject( pUser ))
		return;

	// If this gump was opened from login, we will NOT have an NPC.
	var loginQuestID = parseInt( pUser.GetTempTag( "questConversationID" ), 10 );
	var hasLoginQuest = ( loginQuestID !== null && loginQuestID !== undefined && loginQuestID > 0 );

	var questNpc = null;
	var playerQuestID = null;
	var quest = null;

	if( hasLoginQuest )
	{
		playerQuestID = loginQuestID;
		quest = TriggerEvent( 5801, "QuestList", playerQuestID );
	}
	else
	{
		var npcSer = parseInt( pUser.GetTag( "questNpcSerial" ), 10 );
		questNpc = CalcCharFromSer( npcSer );

		if( !ValidateObject( questNpc ))
		{
			return;
		}

		// Use the questID the gump was opened for (prevents re-resolving chain on click)
		playerQuestID = parseInt(pUser.GetTempTag("QuestConversationQuestID"), 10);
		if (isNaN(playerQuestID) || playerQuestID <= 0)
		{
			// Fallback if temp tag missing (shouldn't happen)
			var initialQuestID = parseInt(questNpc.GetTag("QuestID"), 10);
			playerQuestID = ResolvePlayerQuestID(pUser, initialQuestID);
		}
		quest = TriggerEvent(5801, "QuestList", playerQuestID);
	}

	switch( pButton )
	{
		case 0: // Close gump
			// Important: clear the login temp tag so later NPC gumps don't mis-detect
			if( hasLoginQuest )
			{
				pUser.SetTempTag( "questConversationID", null );
			}
			pUser.SetTempTag( "QuestConversationQuestID", null );
			break;
		case 1: // Accept quest
			TriggerEvent( 5800, "StartQuest", pUser, playerQuestID );
			if( hasLoginQuest )
			{
				pUser.SetTempTag( "questConversationID", null );
			}
			pUser.SetTempTag( "QuestConversationQuestID", null );
			break;
		case 2: // Refuse quest
			if( quest && quest.refuse )
			{
				if( ValidateObject( questNpc ))
				{
					questNpc.TextMessage( quest.refuse );
				}
				else
				{
					pSock.SysMessage( quest.refuse ); // login flow has no NPC
				}
			}
			if( hasLoginQuest )
			{
				pUser.SetTempTag( "questConversationID", null );
			}
			pUser.SetTempTag( "QuestConversationQuestID", null );
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

				var turnInSuccess = ProcessQuestTurnIn( pUser, playerQuestID ); // Handle item turn-in
				if( turnInSuccess )
				{
					TriggerEvent( 5800, "CompleteQuest", pUser, playerQuestID ); // Complete the quest
					pUser.SoundEffect(0x5B5, true);
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 19603, pSock.language ));// You need to have all required quest items to turn in this quest.
				}
			}
			pUser.SetTempTag( "QuestConversationQuestID", null );
			break;
		case 4: // Resign Quest
			ResignQuest( pUser, playerQuestID );
			ManageQuestItems( pUser, playerQuestID, false );
			pUser.SoundEffect( 0x5B3, true );
			pUser.SetTempTag( "QuestConversationQuestID", null );
			break;
			default:break
	}
}

/** @type { ( player: Character, questID: number ) => boolean } */
function ResignQuest( player, questID )
{
	if( !ValidateObject( player ))
		return false;

	var socket = player.socket;
	if( socket == null )
		return false;

	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", player );
	var newQuestProgressArray = [];
	var questFound = false;

	var quest = TriggerEvent( 5801, "QuestList", questID );

	for( var i = 0; i < questProgressArray.length; i++ ) 
	{
		var questEntry = questProgressArray[i];

		// Skip the quest that needs to be resigned, effectively removing it
		if( questEntry.questID == questID && questEntry.serial == player.serial )
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
		socket.SysMessage( GetDictionaryEntry( 19604, socket.language ));//You are not currently on this quest.
		return false;
	}

	// Write back the updated quest progress, excluding the resigned quest
	TriggerEvent( 5800, "WriteQuestProgress", player, newQuestProgressArray );

	socket.SysMessage( GetDictionaryEntry(19605, socket.language ));//The quest has been completely removed from your progress.
	return true;
}

/** @type { ( player: Character, questID: number, mark: boolean ) => void } */
function ManageQuestItems( player, questID, mark )
{
	if( !ValidateObject( player ))
		return;

	var socket = player.socket;
	if( socket == null )
		return;

	var pack = player.pack;

	if( !ValidateObject( pack ))
	{
		socket.SysMessage( GetDictionaryEntry( 19606, socket.language )); // You do not have a backpack.
		return;
	}

	// Try to fetch quest info, but DO NOT early-return on failure when unmarking
	var quest = TriggerEvent( 5801, "QuestList", questID ) || {};

	// Build a quick lookup for sectionIDs (for mark path and as a fallback on unmark)
	var sectionLookup = {};
	if( quest.targetItems && quest.targetItems.length )
	{
		for( var i = 0; i < quest.targetItems.length; i++ )
		{
			// targetItems are data objects, not items; don't call GetTag on them
			sectionLookup[String( quest.targetItems[i].sectionID )] = true;
		}
	}

	/** @type { ( item: Item ) => void } */
	function UnMarkItem( item )
	{
		var saved = item.GetTag( "saveColor" );
		if( saved != null && !isNaN(parseInt( saved )))
		{
			item.color = parseInt( saved );
		}
		else
		{
			item.color = 0;
		}

		item.isNewbie  = false;
		item.isDyeable = true;

		item.SetTag( "QuestItem", null );
		item.SetTag( "QuestSectionID", null );
		item.SetTag( "QuestID", null );
		item.SetTag( "saveColor", null );

		item.RemoveScriptTrigger( 5806 );
	}

	// Optional: handle items inside nested containers
	/** @type { ( container: Item, fn: ( item: Item ) => void ) => void } */
	function ForEachItemIn( container, fn )
	{
		for (var containerItem = container.FirstItem(); !container.FinishedItems(); containerItem = container.NextItem())
		{
			if( !ValidateObject( containerItem ))
				continue;

			fn( containerItem );
			// If this item is itself a container, walk it too (API mirrors backpacks)
			if( typeof containerItem.FirstItem === "function" && typeof containerItem.FinishedItems === "function" )
			{
				ForEachItemIn( containerItem, fn );
			}
		}
	}

	ForEachItemIn( pack, function( currentItem )
	{
		if( mark )
		{
			// Collection targets
			var matchesTarget = sectionLookup[String(currentItem.sectionID)] === true;

			// Delivery target
			if( !matchesTarget && quest.type == "delivery" && quest.deliveryItem )
			{
				matchesTarget = ( String( currentItem.sectionID ) == String( quest.deliveryItem.sectionID ));
			}

			if( matchesTarget && !currentItem.GetTag( "QuestItem" ))
			{
				currentItem.SetTag( "saveColor", currentItem.color );
				currentItem.color     = 0x04ea; // Orange hue
				currentItem.isNewbie  = true;
				currentItem.isDyeable = false;

				currentItem.SetTag( "QuestItem", true );
				currentItem.SetTag( "QuestSectionID", currentItem.sectionID);
				currentItem.SetTag( "QuestID", questID );

				currentItem.AddScriptTrigger( 5806 );
				// socket.SysMessage("Marked item as a quest item.");
			}
			return;
		}

		// UNMARK path (resign). Prefer exact QuestID match; fall back to section match if quest data exists.
		if( currentItem.GetTag( "QuestItem" ))
		{
			var itemQuestID   = currentItem.GetTag( "QuestID" );
			var itemSectionID = String( currentItem.GetTag( "QuestSectionID" ) || currentItem.sectionID );

			var belongsToThisQuest =
				( itemQuestID != null && String( itemQuestID ) == String( questID )) ||
				( sectionLookup[itemSectionID] === true ) ||           // fallback if QuestID wasn�t set earlier
				( itemQuestID == null && !quest.targetItems );         // last resort if we have no quest data at all

			if( belongsToThisQuest )
			{
				UnMarkItem( currentItem );
				// socket.SysMessage("Unmarked quest item.");
			}
		}

		// Delivery items on resign: delete only if they belong to this quest
		if( quest.type == "delivery" && quest.deliveryItem )
		{
			if( String( currentItem.sectionID ) == String( quest.deliveryItem.sectionID ))
			{
				// avoid nuking unrelated items: require QuestID match or at least QuestItem tag
				if( String( currentItem.GetTag( "QuestID" )) == String( questID ) || currentItem.GetTag( "QuestItem" ))
				{
					currentItem.Delete();
					socket.SysMessage( "Deleted delivery item: " + quest.deliveryItem.name );
				}
			}
		}
	});
}

/** @type { ( quest: any, questProgress: any ) => string } */
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
			var collected = ( questProgress && questProgress.collectedItems[targetItem.sectionID] ) || 0;

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
			var npcName = targetKill.name || "Unknown Npc"; // Use `name` if available, fallback to "Unknown Npc"
			var killed = ( questProgress && questProgress.harvestKills[targetKill.npcID] ) || 0;

			objectives += "- " + npcName + ": " + killed + "/" + targetKill.amount + "<br>";
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

	// Delivery
	if( quest.type == "delivery" ) 
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
	if( quest.type == "skillgain" && quest.targetSkill != undefined && quest.maxSkillPoints != undefined )
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

	if( objectives == "" )
	{
		objectives = "No specific objectives.";
	}

	return objectives;
}

/** @type { ( skillID: number ) => string } */
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

/** @type { ( quest: any ) => string } */
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

/** @type { ( player: Character, questID: number ) => boolean } */
function ProcessQuestTurnIn( player, questID )
{
	if( !ValidateObject( player ))
		return false;

	var socket = player.socket;
	if( socket == null )
		return false;

	// Fetch the quest details
	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest )
	{
		player.SysMessage( GetDictionaryEntry( 19602, socket.language )); // This quest does not exist.
		return false;
	}

	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", player );
	var questProgress = null;

	// Find the quest progress entry
	for( var progressIndex = 0; progressIndex < questProgressArray.length; progressIndex++ )
	{
		if( questProgressArray[progressIndex].questID == questID && questProgressArray[progressIndex].serial == player.serial )
		{
			questProgress = questProgressArray[progressIndex];
			break;
		}
	}

	// Handle skill gain quests
	if( quest.type == "skillgain" )
	{
		if( !questProgress )
			return false;

		if( questProgress.skillProgress >= quest.maxSkillPoints )
		{
			socket.SysMessage( GetDictionaryEntry( 19607, socket.language )); // You have completed the skill training for this quest!
			return true;
		}

		socket.SysMessage(
			"You still need to improve your skill. Current progress: " +
			( questProgress.skillProgress / 10 ).toFixed( 1 ) + "/" +
			( quest.maxSkillPoints / 10 ).toFixed( 1 )
		);
		return false;
	}

	// Validate kill objectives for quests that include kill requirements
	if( quest.type == "kill" || quest.type == "timekills" || quest.type == "multi" )
	{
		if( !questProgress || !questProgress.harvestKills )
		{
			return false;
		}

		for( var killIndex = 0; killIndex < quest.targetKills.length; killIndex++ )
		{
			var targetKill = quest.targetKills[killIndex];
			if(( questProgress.harvestKills[targetKill.npcID] || 0 ) < targetKill.amount )
			{
				socket.SysMessage( "You have not killed enough " + targetKill.npcID + "s." );
				return false;
			}
		}
	}

	// Validate and remove item objectives for quests that include item requirements
	if( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" )
	{
		if( !quest.targetItems || quest.targetItems.length == 0 )
		{
			return false;
		}

		var questItems = FindQuestItems( player, questID );
		var requiredItems = [];

		for( var targetIndex = 0; targetIndex < quest.targetItems.length; targetIndex++ )
		{
			requiredItems.push({
				sectionID: String( quest.targetItems[targetIndex].sectionID ),
				amount: quest.targetItems[targetIndex].amount
			});
		}

		// Pass 1: verify total amounts across all stacks before touching anything
		for( var requiredIndex = 0; requiredIndex < requiredItems.length; requiredIndex++ )
		{
			var totalFound = 0;

			for( var itemIndex = 0; itemIndex < questItems.length; itemIndex++ )
			{
				if( String( questItems[itemIndex].sectionID ) == requiredItems[requiredIndex].sectionID )
				{
					totalFound += questItems[itemIndex].amount;
				}
			}

			if( totalFound < requiredItems[requiredIndex].amount )
			{
				socket.SysMessage( GetDictionaryEntry( 19608, socket.language )); // You are still missing some items for this quest.
				return false;
			}
		}

		// Pass 2: remove only after validation succeeded
		for( var deductIndex = 0; deductIndex < requiredItems.length; deductIndex++ )
		{
			var remainingAmountToDeduct = requiredItems[deductIndex].amount;

			for( var questItemIndex = 0; questItemIndex < questItems.length && remainingAmountToDeduct > 0; questItemIndex++ )
			{
				var questItem = questItems[questItemIndex];

				if( !ValidateObject( questItem ))
				{
					continue;
				}

				if( String( questItem.sectionID ) != requiredItems[deductIndex].sectionID )
				{
					continue;
				}

				var amountToDeduct = Math.min( questItem.amount, remainingAmountToDeduct );
				questItem.amount -= amountToDeduct;
				remainingAmountToDeduct -= amountToDeduct;

				if( questItem.amount <= 0 )
				{
					questItem.Delete();
				}
			}
		}

		socket.SysMessage( GetDictionaryEntry( 19609, socket.language )); // All quest items have been turned in successfully.
		return true;
	}

	// Delivery quests are handled through ProcessDeliveryQuest when talking to the target NPC
	if( quest.type == "delivery" )
	{
		socket.SysMessage( GetDictionaryEntry( 19610, socket.language )); // You don't have the required item to deliver.
		return false;
	}

	// Kill-only quests that made it this far are valid turn-ins
	if( quest.type == "kill" || quest.type == "timekills" )
	{
		return true;
	}

	return false;
}

/** @type { ( player: Character, questID: number ) => Item[] } */
function FindQuestItems( player, questID )
{
	if( !ValidateObject( player ))
		return [];

	var socket = player.socket;
	if( socket == null )
		return [];

	var questItems = [];
	var pack = player.pack; // Get the player's backpack

	if( !ValidateObject( pack ))
	{
		socket.SysMessage( GetDictionaryEntry( 19606, socket.language ));
		return questItems; // Return an empty array if no backpack is found
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || ( !quest.targetItems && !quest.deliveryItem ))
	{
		return questItems;
	}

	// Build the list of required item IDs
	var requiredsectionIDs = [];
	if( quest.targetItems ) 
	{
		for( var i = 0; i < quest.targetItems.length; i++ )
		{
			requiredsectionIDs.push( String( quest.targetItems[i].sectionID ));
		}
	}

	// Add delivery item ID if it's a delivery quest
	if( quest.type == "delivery" && quest.deliveryItem )
	{
		requiredsectionIDs.push( String( quest.deliveryItem.sectionID ));
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
		if( currentItem.GetTag( "QuestItem" ) && requiredsectionIDs.indexOf( String( currentItem.sectionID )) != -1 ) 
		{
			questItems.push( currentItem );
		}
	}

	return questItems;
}

/** @type { ( currChar: Character, targChar: Character ) => boolean } */
function onCharDoubleClick( pUser, questNpc ) 
{
	if( !ValidateObject( pUser ))
		return false;

	QuestNpcInterAction( pUser, questNpc );
	return true;
}

/** @type { ( pUser: Character, questNpc: Character ) => boolean } */
function QuestNpcInterAction( pUser, questNpc )
{
	var gumpID = 5822 + 0xffff;

	if( !ValidateObject( pUser ) || !ValidateObject( questNpc ) )
		return false;

	var socket = pUser.socket;
	if( socket == null )
		return false;

	pUser.SetTag( "questNpcSerial", questNpc.serial );

	if( !questNpc.InRange( pUser, 2 ) )
	{
		socket.SysMessage( "You are too far away." );
		return false;
	}

	var deliveryQuestID = parseInt( questNpc.GetTag( "DeliveryQuestID" ), 10 );
	if( !isNaN( deliveryQuestID ) && deliveryQuestID > 0 )
	{
		if( !IsQuestArchivedForPlayer( pUser, deliveryQuestID ) )
		{
			if( TriggerEvent( 5800, "CheckQuest", pUser, deliveryQuestID, "check" ) )
			{
				ProcessDeliveryQuest( pUser, questNpc, deliveryQuestID );
				return true;
			}
		}
	}

	var npcRootQuestID = GetNpcQuestRootID( questNpc );
	if( npcRootQuestID <= 0 )
		return false;

	var playerQuestID = ResolvePlayerQuestID( pUser, npcRootQuestID );
	if( !playerQuestID )
	{
		questNpc.TurnToward( pUser );

		var rootQuest = TriggerEvent( 5801, "QuestList", npcRootQuestID );
		if( !rootQuest )
		{
			socket.SysMessage( "Quest data could not be loaded." );
			return false;
		}

		questNpc.TextMessage( GetDictionaryEntry( 19612, socket.language ) ); // You have completed all quests
		return false;
	}

	questNpc.TurnToward( pUser );
	socket.CloseGump( gumpID, 0 );
	QuestConversationGump( pUser, questNpc, playerQuestID );
	return true;
}

/** @type { ( player: Character, questNpc: Character, deliveryQuestID: number ) => boolean } */
function ProcessDeliveryQuest( player, questNpc, deliveryQuestID )
{
	if( !ValidateObject( player ))
		return false;

	var socket = player.socket;
	if( socket == null )
		return false;

	// Fetch the quest details
	var quest = TriggerEvent( 5801, "QuestList", deliveryQuestID );
	if( !quest || quest.type != "delivery" )
	{
		questNpc.TextMessage( "This is not a delivery quest." );
		return false;
	}

	// Ensure the NPC matches the quest's recipient
	if( !quest.targetDelivery || questNpc.sectionID != quest.targetDelivery.sectionID )
	{
		questNpc.TextMessage( "I am not the intended recipient of this delivery." );
		return false;
	}

	var requiredItem = quest.deliveryItem;
	if( !requiredItem )
	{
		return false;
	}

	var questItems = FindQuestItems( player, deliveryQuestID );
	var totalDeliveryAmount = 0;

	// Pass 1: verify total amount across all matching stacks
	for( var itemIndex = 0; itemIndex < questItems.length; itemIndex++ )
	{
		if( String( questItems[itemIndex].sectionID ) == String( requiredItem.sectionID ) )
		{
			totalDeliveryAmount += questItems[itemIndex].amount;
		}
	}

	if( totalDeliveryAmount < requiredItem.amount )
	{
		socket.SysMessage( GetDictionaryEntry( 19613, socket.language )); // You do not have the required item to deliver.
		return false;
	}

	// Pass 2: deduct across as many stacks as needed
	var remainingAmountToDeduct = requiredItem.amount;

	for( var deductIndex = 0; deductIndex < questItems.length && remainingAmountToDeduct > 0; deductIndex++ )
	{
		var questItem = questItems[deductIndex];

		if( !ValidateObject( questItem ))
		{
			continue;
		}

		if( String( questItem.sectionID ) != String( requiredItem.sectionID ) )
		{
			continue;
		}

		var amountToDeduct = Math.min( questItem.amount, remainingAmountToDeduct );
		questItem.amount -= amountToDeduct;
		remainingAmountToDeduct -= amountToDeduct;

		if( questItem.amount <= 0 )
		{
			questItem.Delete();
		}
	}

	// Update the player's quest progress
	var questProgressArray = TriggerEvent( 5800, "ReadQuestProgress", player );
	for( var progressIndex = 0; progressIndex < questProgressArray.length; progressIndex++ )
	{
		if( questProgressArray[progressIndex].questID == deliveryQuestID && questProgressArray[progressIndex].serial == player.serial )
		{
			questProgressArray[progressIndex].completed = true;
			break;
		}
	}

	TriggerEvent( 5800, "WriteQuestProgress", player, questProgressArray );

	// Notify the player and complete the quest
	socket.SysMessage( GetDictionaryEntry( 19614, socket.language )); // You have delivered the required item!
	TriggerEvent( 5800, "CompleteQuest", player, deliveryQuestID );
	return true;
}

/** @type { ( questNpc: Character ) => number } */
function GetNpcQuestRootID( questNpc )
{
	// Preferred going forward
	var rootQuestID = parseInt( questNpc.GetTag( "QuestRootID" ), 10 );
	if( !isNaN( rootQuestID ) && rootQuestID > 0 )
		return rootQuestID;

	// Backward compatible
	var legacyQuestID = parseInt( questNpc.GetTag( "QuestID" ), 10 );
	if( !isNaN( legacyQuestID ) && legacyQuestID > 0 )
		return legacyQuestID;

	return 0;
}

/** @type { ( player: Character, questID: number ) => boolean } */
function IsQuestArchivedForPlayer( player, questID )
{
	var archivedQuestIDs = TriggerEvent( 5800, "ReadArchivedQuests", player ) || [];
	var questIDInt = parseInt( questID, 10 );

	for( var index = 0; index < archivedQuestIDs.length; index++ )
	{
		if( parseInt( archivedQuestIDs[index], 10 ) == questIDInt )
			return true;
	}
	return false;
}

/** @type { ( player: Character, questID: number ) => boolean } */
function IsQuestActiveForPlayer( player, questID )
{
	var activeQuestEntries = TriggerEvent( 5800, "ReadQuestProgress", player ) || [];
	var questIDInt = parseInt( questID, 10 );

	for( var index = 0; index < activeQuestEntries.length; index++ )
	{
		var entry = activeQuestEntries[index];
		if( entry && entry.serial == player.serial && parseInt( entry.questID, 10 ) == questIDInt )
			return true;
	}
	return false;
}

/** @type { ( player: Character, questData: any ) => number } */
function ResolveNextQuestID_Any( player, questData )
{
	// Prefer the real branching resolver in script 5800
	var resolved = TriggerEvent( 5800, "ResolveNextQuestID", player, questData );
	resolved = parseInt( resolved, 10 );
	if( !isNaN( resolved ) && resolved > 0 )
		return resolved;

	// Fallback: old linear field
	var fallback = parseInt( questData && questData.nextQuestID, 10 );
	if( !isNaN( fallback ) && fallback > 0 )
		return fallback;

	return 0;
}

/** @type { ( player: Character, npcRootQuestID: number ) => ( number | null ) } */
function ResolvePlayerQuestID( player, npcRootQuestID )
{
	if( !ValidateObject( player ) )
		return null;

	var currentQuestID = parseInt( npcRootQuestID, 10 );
	if( isNaN( currentQuestID ) || currentQuestID <= 0 )
		return null;

	// Loop guard in case of bad data / circular chains
	for( var hopCount = 0; hopCount < 50; hopCount++ )
	{
		var questData = TriggerEvent( 5801, "QuestList", currentQuestID );
		if( !questData )
			return null;

		// If active, keep showing it
		if( IsQuestActiveForPlayer( player, currentQuestID ) )
			return currentQuestID;

		// If not archived, this is the next offerable quest
		if( !IsQuestArchivedForPlayer( player, currentQuestID ) )
			return currentQuestID;

		// Archived: advance using branching/linear
		var nextQuestID = ResolveNextQuestID_Any( player, questData );
		if( nextQuestID <= 0 )
			return null;

		currentQuestID = nextQuestID;
	}

	return null;
}

/** @type { ( array: any[], value: number ) => boolean } */
function isQuestArchived( array, value )
{
	value = parseInt( value, 10 ); // Ensure value is a number
	for( var i = 0; i < array.length; i++ ) 
	{
		if( parseInt( array[i], 10 ) == value )
		{
			return true;
		}
	}
	return false;
}

/** @type { ( value: any ) => boolean } */
function isArray( value ) 
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
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

	toSend.WriteShort(  offset, 0x000C );    // Unique ID
	toSend.WriteShort(  offset += 2, 6155 ); // Cancel Quest
	toSend.WriteShort(  offset += 2, 0x0020 ); // Flag, color enabled
	toSend.WriteShort(  offset += 2, 0x03E0 ); // Hue of text

	//Send packet
	socket.Send( toSend );
	toSend.Free();

	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject, popupEntry: number ) => boolean } */
function onContextMenuSelect( socket, questNpc, popupEntry )
{
	var pUser = socket.currentChar;

	if( !ValidateObject( pUser ) || !ValidateObject( questNpc ))
	{
		return false;
	}

	switch( popupEntry )
	{
		case 0x000A: // Quest Conversation
			{
				if( !ValidateObject( pUser ) || !ValidateObject( questNpc ))
				{
					return false;
				}

				if( !questNpc.InRange( pUser, 2 ))
				{
					pUser.SysMessage( "You are too far away." );
					return false;
				}

				QuestNpcInterAction( pUser, questNpc );
			}
			break;
		case 0x000C: // Cancel Quest (Optional)
			{
				if( !ValidateObject( pUser ) || !ValidateObject( questNpc ))
				{
					return false;
				}

				if( !questNpc.InRange( pUser, 2 ))
				{
					pUser.SysMessage( "You are too far away." );
					return false;
				}

				var initialQuestID = parseInt( questNpc.GetTag( "QuestID" ), 10 );
				var playerQuestID = ResolvePlayerQuestID( pUser, initialQuestID );
				ResignQuest( pUser, playerQuestID );
				ManageQuestItems( pUser, playerQuestID, false );
				pUser.SoundEffect( 0x5B3, true );
			}
			break;
		default:
			return true;
	}

	return false;
}
