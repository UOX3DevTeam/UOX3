/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( player: Character, questID: number ) => void } */
function StartQuest( player, questID )
{
	if( !ValidateObject( player ))
		return;	

	var socket = player.socket;
	if( socket == null )
		return;

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
			collectedItems[targetItem.sectionID] = 0; // Start with 0 collected
		}
	}

	if( quest.targetKills )
	{
		for( var i = 0; i < quest.targetKills.length; i++ )
		{
			var targetKill = quest.targetKills[i];
			harvestKills[targetKill.npcID] = 0; // Start with 0 kills
		}
		player.AddScriptTrigger( 5810 );// onKill quest trigger
	}

	// Add the delivery item to the player's backpack if it's a delivery quest
	if( quest.type == "delivery" && quest.deliveryItem )
	{
			var package = CreateDFNItem( player.socket, player, quest.deliveryItem.sectionID, quest.deliveryItem.amount, "ITEM", true )
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
				package.SetTag( "QuestSectionID", package.sectionID );
				package.AddScriptTrigger( 5806 );// Quest Item script trigger
				player.SysMessage( "You have received a " + quest.deliveryItem.name + " for delivery." );
			} 
			else
			{
				socket.SysMessage( GetDictionaryEntry( 19615, socket.language )); // Failed to create the delivery item.
			}
	}

	var initialSkillLevel = 0;
	if( quest.type == "skillgain" )
	{
		if( !player.GetTag( "AcceleratedSkillGain" ))
		{
			initialSkillLevel = player.baseskills[quest.targetSkill]; // Get current skill level
			player.SetTag( "AcceleratedSkillGain", quest.targetSkill );
			player.AddScriptTrigger( 5811 ); // Quest skill gain script trigger
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 19616, socket.language )); // You are already under the effect of an accelerated skillgain scroll.
		}
	}

	// Add new quest to progress
	questProgressArray.push({
		serial: player.serial,
		questID: questID,
		questProgress: 0, // General progress
		harvestKills: harvestKills, // Kill objectives
		collectedItems: collectedItems, // Item objectives
		skillProgress: initialSkillLevel, // Initialize skill progress
		targetSkill: quest.targetSkill || -1, // Target skill for "skillgain"
		targetRegion: quest.targetRegion || 0, // Target region for "skillgain"
		maxSkillPoints: quest.maxSkillPoints || 50.0, // Max skill points for "skillgain"
		startTime: quest.timeLimit ? Date.now() : 0, // Timed quests
		timeLimit: quest.timeLimit ? quest.timeLimit * 1000 : 0,
		lastAccepted: Date.now(), // Record the time the quest was accepted
		completed: false,
		questTurnIn: false,
		nextQuestID: quest.nextQuestID || null
	});

	WriteQuestProgress( player, questProgressArray );

	socket.SysMessage( GetDictionaryEntry( 19617, socket.language )); // You have accepted the Quest.

	// Start the timer if the quest is timed
	if( quest.timeLimit )
	{
		player.StartTimer( quest.timeLimit * 1000, questID, 5800 ); // Timer in milliseconds
		var minutes = Math.floor( quest.timeLimit / 60 ); // Convert total seconds to minutes
		var seconds = quest.timeLimit % 60; // Get remaining seconds
		socket.SysMessage( "You have " + minutes + " minute( s ) and " + seconds + " second( s ) to complete this quest." );
	}
}

/** @type { ( player: Character, questID: number, mode?: string ) => boolean } */
function CheckQuest( player, questID, mode)
{
	mode = mode || "start"; // default
	if( !ValidateObject( player ))
		return false;	

	var socket = player.socket;
	if( socket == null )
		return false;

	var playerSerial = player.serial; // Use player serial to identify the quest owner
	// Fetch quest details using the QuestList trigger
	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest )
	{
		socket.SysMessage( "Quest not found." );
		return false; // Indicate that the quest cannot proceed
	}

	// Read the player's active and archived quest data
	var questProgressArray = ReadQuestProgress( player );
	var archivedQuests = ReadArchivedQuests( player );

	// Prerequisite quest gate
	if( quest.requiresQuestID != null && quest.requiresQuestID != 0 )
	{
		var reqID = parseInt( quest.requiresQuestID, 10 );
		if( reqID > 0 )
		{
			// archivedQuests is an array of questIDs
			if( archivedQuests.indexOf( reqID ) == -1 )
			{
				var reqQuest = TriggerEvent( 5801, "QuestList", reqID );
				var reqName = ( reqQuest && reqQuest.title ) ? reqQuest.title : ( "Quest #" + reqID );

				socket.SysMessage( "You must complete \"" + reqName + "\" before you can start this quest." );
				return false;
			}
		}
	}

	// Check if already working on this quest
	if( mode == "start" )
	{
		// Check if the quest is already in progress
		// Ensure no duplicate quests for the same player
		for (var i = 0; i < questProgressArray.length; i++) 
		{
			if( questProgressArray[i].questID == questID && questProgressArray[i].serial == playerSerial ) 
			{
				socket.SysMessage( GetDictionaryEntry( 19618, socket.language )); // You are already working on this quest.
				return false;
			}
		}
	}

	// Check if the quest is marked as oneTimeQuest and already completed
	if( quest.oneTimeQuest == 1 ) 
	{
		for( var i = 0; i < archivedQuests.length; i++ )
		{
			if( parseInt( archivedQuests[i], 10 ) == questID )
			{
				socket.SysMessage( GetDictionaryEntry( 19619, socket.language )); // This quest can only be completed once, and you have already completed it.
				return false; // Indicate that the quest cannot proceed
			}
		}
	}

	// Check if the quest is a daily quest and ensure it has reset
	if( quest.dailyQuest == 1 )
	{
		for( var i = 0; i < archivedQuests.length; i++ )
		{
			if( parseInt( archivedQuests[i], 10 ) == questID )
			{
				var lastCompleted = archivedQuests[i].lastCompleted || 0;
				var resetTime = quest.resetDailyTime || 24; // Default reset time is 24 hours
				var currentTime = Date.now();

				if(( currentTime - lastCompleted ) < resetTime * 3600 * 1000)
				{
					var hoursLeft = Math.ceil(( resetTime * 3600 * 1000 - ( currentTime - lastCompleted )) / ( 3600 * 1000 ));
					socket.SysMessage( "This is a daily quest, and you must wait " + hoursLeft + " more hour(s) to attempt it again." );
					return false; // Indicate that the quest cannot proceed
				}
			}
		}
	}
	socket.SoundEffect( 0x5B4, true );
	return true;
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( timerObj, timerID )
{
	var player = timerObj; // Assuming timerObj is the player object
	var socket = player.socket;
	var questProgressArray = ReadQuestProgress( player );

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.questID == timerID && !questEntry.completed )
		{
			var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

			TriggerEvent( 5802, "ManageQuestItems", player, questEntry.questID, false );

			if( quest.type == "skillgain" ) 
			{
				player.SetTag( "AcceleratedSkillGain", null ); // Remove the tag
				player.RemoveScriptTrigger( 5811 ); // Remove the quest skill gain script trigger
				socket.SysMessage( "You have stopped accelerated training for " + GetSkillName( quest.targetSkill ) + "." );
			}

			// Time has expired
			socket.SysMessage( "You have failed the timed quest: " + quest.title );

			// Log the failed quest
			LogFailedQuest( player, questEntry );

			questProgressArray.splice( i, 1 ); // Remove the failed quest
			WriteQuestProgress( player, questProgressArray );
			return;
		}
	}
}

/** @type { ( player: Character, questID: number, identifier: number | string, progressValue: number, type: string ) => any[] } */
function UpdateQuestProgress( player, questID, identifier, progressValue, type )
{
	if( !ValidateObject( player ))
		return;	

	var socket = player.socket;
	if( socket == null )
		return;

	var questProgressArray = ReadQuestProgress( player );
	var questUpdated = false;

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];

		// Ensure the quest belongs to the current player
		if( questEntry.serial != player.serial || questEntry.questID != questID ) 
		{
			continue;
		}

		if( questEntry.questID == questID ) 
		{
			var quest = TriggerEvent( 5801, "QuestList", questID ); // Fetch quest data
			var allObjectivesCompleted = true;

			// Check item collection objectives
			if( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" )
			{
				if( quest.targetItems )
				{
					for( var j = 0; j < quest.targetItems.length; j++ )
					{
						var target = quest.targetItems[j];

						if( String( target.sectionID ) == String( identifier ))
						{
							questEntry.collectedItems[identifier] = Math.max( 0, questEntry.collectedItems[identifier] + progressValue );

							// Cap the collected amount to the target amount
							if( questEntry.collectedItems[identifier] > target.amount )
							{
								questEntry.collectedItems[identifier] = target.amount;
							}
						}

						// Check if all items are collected
						if( !questEntry.collectedItems || ( questEntry.collectedItems[target.sectionID] ) < target.amount )
						{
							allObjectivesCompleted = false;
						}
					}
				}
			}

			// Check kill objectives
			if( quest.type == "kill" || quest.type == "timekills" || quest.type == "multi" )
			{
				if( quest.targetKills )
				{
					for( var k = 0; k < quest.targetKills.length; k++ )
					{
						var target = quest.targetKills[k];

						if( String( target.npcID ) == String( identifier ))
						{
							questEntry.harvestKills[identifier] = ( questEntry.harvestKills[identifier] || 0 ) + progressValue;

							// Cap the kill count to the target amount
							if( questEntry.harvestKills[identifier] > target.amount )
							{
								questEntry.harvestKills[identifier] = target.amount;
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

			if( quest.type == "delivery" )
			{
				// Check if the player delivered the item to the correct NPC
				if( identifier == quest.targetDelivery.npcID ) 
				{
					// Fetch the delivery item from the player's backpack
					var pack = player.pack; // Get the player's backpack
					var hasItem = false;

					// Iterate through all items in the player's backpack
					for( currentItem = pack.FirstItem(); !pack.FinishedItems(); currentItem = pack.NextItem())
					{
						if( !ValidateObject( currentItem )) 
						{
							continue;
						}

						// Check if the item matches the delivery item and has the correct tag
						/*var questSectionID = deliveryItem.GetTag( "QuestSectionID" ) || deliveryItem.sectionID;
						if( currentItem.GetTag( "QuestItem" ) && String( currentItem.sectionID ) == String( quest.deliveryItem.sectionID || currentItem.sectionID == questSectionID  ) && currentItem.amount >= quest.deliveryItem.amount )
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
						}*/
						var questSectionID = currentItem.GetTag("QuestSectionID") || currentItem.sectionID;
						if ( currentItem.GetTag("QuestItem") && String(questSectionID) == String(quest.deliveryItem.sectionID) && currentItem.amount >= quest.deliveryItem.amount )
						{
							hasItem = true;

							if (currentItem.amount > quest.deliveryItem.amount)
								currentItem.amount -= quest.deliveryItem.amount;
							else
								currentItem.Delete();

							break;
						}
					}

					if( hasItem )
					{
						questEntry.completed = true;
						socket.SysMessage( GetDictionaryEntry( 19620, socket.language )); // You have successfully delivered the item!
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 19621, socket.language )); // You don't have the required item to deliver.
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 19622, socket.language )); // This is not the correct NPC to deliver the item.
				}
			}

			// Check skill gain objectives
			if( quest.type == "skillgain" && quest.targetSkill == identifier )
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
					socket.SysMessage( "Skill progress: " + ( questEntry.skillProgress / 10 ).toFixed(1) + "/" + ( quest.maxSkillPoints / 10 ).toFixed(1) );
					allObjectivesCompleted = false;
				}
			}

			// Mark quest as completed if all objectives are met
			if( allObjectivesCompleted )
			{
				if( quest.questTurnIn == 1 )
				{
					questEntry.completed = true;
					if( quest.type == "skillgain" && quest.oncomplete )
					{
						socket.SysMessage( quest.oncomplete );
					}
					else 
					{
						socket.SysMessage( GetDictionaryEntry( 19623, socket.language ));
					}
					WriteQuestProgress( player, questProgressArray );
				}
				else
				{
					questEntry.completed = true;
					if (quest.type == "skillgain" && quest.oncomplete )
					{
						socket.SysMessage( quest.oncomplete );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 19623, socket.language )); // You've completed the quest! Don't forget to collect your reward.
					}
					WriteQuestProgress( player, questProgressArray );
					CompleteQuest( player, questID );
				}
				return;
			}
			else
			{
				// If any objectives are now missing, make sure it's not marked as completed
				questEntry.completed = false;
			}

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
		socket.SysMessage( GetDictionaryEntry( 19624, socket.language )); // No progress updated for the quest.
	}

	return questProgressArray;
}

/** @type { ( player: Character, questID: number ) => void } */
function CompleteQuest( player, questID )
{
	if( !ValidateObject( player ))
		return;	

	var socket = player.socket;
	if( socket == null )
		return;

	var questProgressArray = ReadQuestProgress( player );
	var newQuestProgressArray = [];
	var questCompleted = false;

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		// Ensure the quest is tied to the current player and matches the questID
		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			newQuestProgressArray.push( questEntry );
			continue;
		}

		// Ensure the quest is completed
		if( !questEntry.completed )
		{
			socket.SysMessage( GetDictionaryEntry( 19625, socket.language ));//You haven't completed the quest yet.
			return;
		}

		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( !quest )
		{
			socket.SysMessage( GetDictionaryEntry( 19626, socket.language )); //Quest data could not be retrieved.
			return;
		}

		// Notify the player and play a visual effect
		socket.SysMessage( "Congratulations! You have completed the quest: " + quest.title );
		DoStaticEffect( player.x, player.y, player.z, 0x376A, 0x40, 0x16, false );

		// Handle rewards
		if( quest.rewards )
		{
			QuestRewards( player, quest, socket );
		}

		// Archive the completed quest
		ArchiveCompletedQuest( player, questEntry );

		ApplyQuestSetTags( player, quest );
		ApplyQuestSetTagDeltas( player, quest );

		ApplyQuestSetTempTags( player, quest );
		ApplyQuestSetTempTagDeltas( player, quest );

		ApplyQuestSetWorldState( player, quest );
		ApplyQuestWorldStateDeltas( player, quest );

		// Cancel any active quest timer for this questID
		player.KillJSTimer( questID, 5800 );

		// Mark the quest as completed
		questCompleted = true;
	}

	// Save updated progress, excluding the completed quest
	WriteQuestProgress( player, newQuestProgressArray );

	// Handle chained quests
	if( questCompleted )
	{
		StartNextQuestInChain( player, questID, socket );
	}
}

/** @type { ( player: Character, quest: any, socket: Socket ) => void } */
function QuestRewards( player, quest, socket )
{
	if( !ValidateObject( player ))
		return;

	var bankBox = player.FindItemLayer( 29 );

	quest.rewards.forEach( function( reward )
	{
		switch( reward.type )
		{
			case "gold":
				GoldReward( player, reward, bankBox, socket );
				break;
			case "item":
				{
					var color = 0;
					if( reward.color != null )
					{
						color = parseInt( reward.color, 10 );
					}
					else if( reward.hue != null )
					{
						color = parseInt( reward.hue, 10 );
					}

					if( isNaN( color ))
						color = 0;

					CreateDFNItem(player.socket, player, reward.sectionID, reward.amount, "ITEM", true, color );
					socket.SysMessage("You receive a reward: " + reward.amount + " of item " + reward.name + "!");
					break;
				}
			case "karma":
				player.karma += reward.amount;
				socket.SysMessage( "You gained " + reward.amount + " karma!" );
				break;
			case "fame":
				player.fame += reward.amount;
				socket.SysMessage( "You gained " + reward.amount + " fame!" );
				break;
			case "skill":
				SkillReward( player, reward, socket );
				break;
			case "skillpoints":
				SkillPointsPoolReward( player, reward, socket );
				break;
			case "virtue":
				{
					// Prefer numeric index 0..7 in reward.virtueIndex
					var vIdx = parseInt( reward.virtueIndex, 10);
					if( isNaN( vIdx ))
						vIdx = 0;

					var amt = parseInt( reward.amount, 10 ) || 0;
					if( amt <= 0 )
					{
						socket.SysMessage( "Virtue reward amount invalid." );
						break;
					}

					var result = TriggerEvent(8003, "Virtue_Award", player, vIdx, amt);

					if( result && result.success )
					{
						if( result.gainedPath )
							socket.SysMessage("You have gained a path in virtue!");
						else
							socket.SysMessage("You have gained in virtue.");
					}
					break;
				}
			default:
				socket.SysMessage( "Unknown reward type: " + reward.type );
				break;
		}
	});
}

/** @type { ( player: Character, quest: any ) => void } */
function ApplyQuestSetTags( player, quest )
{
	if( !quest || !quest.setTags )
		return;

	for( var key in quest.setTags )
	{
		if( !quest.setTags.hasOwnProperty( key ))
			continue;

		var val = quest.setTags[key];

		// null/undefined => remove tag
		if( val == null )
			player.SetTag( key, null );
		else
			player.SetTag( key, val );
	}
}

/** @type { ( player: Character, quest: any ) => void } */
function ApplyQuestSetTempTags( player, quest )
{
	if( !quest || !quest.setTempTags )
		return;

	for( var key in quest.setTempTags )
	{
		if( !quest.setTempTags.hasOwnProperty( key ))
			continue;

		var val = quest.setTempTags[key];

		// null/undefined => remove
		if( val == null )
			player.SetTempTag( key, null );
		else
			player.SetTempTag( key, val );
	}
}

/** @type { ( player: Character, quest: any ) => void } */
function ApplyQuestSetWorldState( player, quest )
{
	if( !quest || !quest.setWorldState )
		return;

	for( var worldStateKey in quest.setWorldState )
	{
		if( !quest.setWorldState.hasOwnProperty( worldStateKey ) )
			continue;

		var worldStateValue = quest.setWorldState[worldStateKey];

		// null/undefined => remove the world-state key
		if( worldStateValue == null )
		{
			SetWorldStateInt( player, worldStateKey, null );
		}
		else
		{
			SetWorldStateInt( player, worldStateKey, ToIntOrZero( worldStateValue ) );
		}
	}
}

/** @type { ( player: Character, quest: any ) => void } */
function ApplyQuestWorldStateDeltas( player, quest )
{
	if( !quest || !quest.worldStateDeltas )
		return;

	for( var worldStateKey in quest.worldStateDeltas )
	{
		if( !quest.worldStateDeltas.hasOwnProperty( worldStateKey ) )
			continue;

		var deltaAmount = ToIntOrZero( quest.worldStateDeltas[worldStateKey] );
		var currentWorldStateValue = GetWorldStateInt( player, worldStateKey, 0 );
		var nextWorldStateValue = currentWorldStateValue + deltaAmount;

		// Optional clamp rules:
		// - quest.worldStateDeltaRules[worldStateKey] overrides
		// - else quest.worldStateDeltaRulesDefault
		var clampRule = ResolveDeltaRule( quest, worldStateKey, "worldStateDeltaRules", "worldStateDeltaRulesDefault" );

		if( clampRule )
		{
			var hasMin = clampRule.hasOwnProperty( "min" );
			var hasMax = clampRule.hasOwnProperty( "max" );

			var minAllowedValue = ( hasMin ? ToIntOrZero( clampRule.min ) : null );
			var maxAllowedValue = ( hasMax ? ToIntOrZero( clampRule.max ) : null );

			if( hasMin || hasMax )
			{
				nextWorldStateValue = ClampInt(
					nextWorldStateValue,
					hasMin ? minAllowedValue : null,
					hasMax ? maxAllowedValue : null
				);
			}
		}

		SetWorldStateInt( player, worldStateKey, nextWorldStateValue );
	}
}

/** @type { ( player: Character, quest: any ) => void } */
function ApplyQuestSetTagDeltas( player, quest )
{
	if( !quest || !quest.setTagDeltas )
		return;

	for( var key in quest.setTagDeltas )
	{
		if( !quest.setTagDeltas.hasOwnProperty( key ))
			continue;

		var deltaVal = quest.setTagDeltas[key];
		var delta = ToIntOrZero( deltaVal );

		var cur = GetIntTagOrZero( player, key );
		var next = cur + delta;

		// Optional clamp
		var rule = ResolveDeltaRule( quest, key, "deltaRules", "deltaRulesDefault" );
		if( rule )
		{
			var minVal = ( rule.hasOwnProperty( "min" ) ? ToIntOrZero( rule.min ) : null );
			var maxVal = ( rule.hasOwnProperty( "max" ) ? ToIntOrZero( rule.max ) : null );

			// If they specify only min or only max, we still clamp correctly
			if( rule.hasOwnProperty( "min" ) || rule.hasOwnProperty( "max" ))
				next = ClampInt( next, rule.hasOwnProperty( "min" ) ? minVal : null, rule.hasOwnProperty( "max" ) ? maxVal : null );
		}

		player.SetTag( key, next );
	}
}

/** @type { ( player: Character, quest: any ) => void } */
function ApplyQuestSetTempTagDeltas( player, quest )
{
	if( !quest || !quest.setTempTagDeltas )
		return;

	for( var key in quest.setTempTagDeltas )
	{
		if( !quest.setTempTagDeltas.hasOwnProperty( key ))
			continue;

		var deltaVal = quest.setTempTagDeltas[key];
		var delta = ToIntOrZero( deltaVal );

		var cur = GetIntTempTagOrZero( player, key );
		var next = cur + delta;

		// Optional clamp
		var rule = ResolveDeltaRule( quest, key, "tempDeltaRules", "tempDeltaRulesDefault" );
		if( rule )
		{
			var minVal = ( rule.hasOwnProperty( "min" ) ? ToIntOrZero( rule.min ) : null );
			var maxVal = ( rule.hasOwnProperty( "max" ) ? ToIntOrZero( rule.max ) : null );

			if( rule.hasOwnProperty( "min" ) || rule.hasOwnProperty( "max" ))
				next = ClampInt( next, rule.hasOwnProperty( "min" ) ? minVal : null, rule.hasOwnProperty( "max" ) ? maxVal : null );
		}

		player.SetTempTag( key, next );
	}
}

/** @type { ( v: any ) => number } */
function ToIntOrZero( v )
{
	if( v == null || v === "" )
		return 0;

	var n = parseInt( v, 10 );
	if( isNaN( n ))
		return 0;

	return n;
}

/** @type { ( v: number, minVal: number | null, maxVal: number | null ) => number } */
function ClampInt( v, minVal, maxVal )
{
	if( minVal != null && v < minVal ) v = minVal;
	if( maxVal != null && v > maxVal ) v = maxVal;
	return v;
}

/** @type { ( obj: BaseObject, tagName: string ) => number } */
function GetIntTagOrZero( obj, tagName )
{
	return ToIntOrZero( obj.GetTag( tagName ));
}

/** @type { ( obj: BaseObject, tagName: string ) => number } */
function GetIntTempTagOrZero( obj, tagName )
{
	if( !obj.GetTempTag )
		return 0;
	return ToIntOrZero( obj.GetTempTag( tagName ));
}

// Resolve the clamp rule for a given key:
// - if quest.deltaRules has a rule for this key, use it
// - else if quest.deltaRulesDefault exists, use that
// - else return null (no clamping)
/** @type { ( quest: any, key: string, rulesObjName: string, defaultRuleName: string ) => any } */
function ResolveDeltaRule( quest, key, rulesObjName, defaultRuleName )
{
	if( !quest )
		return null;

	var rules = quest[rulesObjName];
	if( rules && rules.hasOwnProperty( key ) && rules[key] )
		return rules[key];

	var defRule = quest[defaultRuleName];
	if( defRule )
		return defRule;

	return null;
}

/** @type { ( player: Character, reward: any, bankBox: Item, socket: Socket ) => void } */
function GoldReward( player, reward, bankBox, socket )
{
	if( reward.bankgold == 1 )
	{
		if( ValidateObject( bankBox ) && bankBox.totalItemCount < bankBox.maxItems )
		{
			var gold = CreateDFNItem( player.socket, player, "0x0eed", reward.amount, "ITEM", false );
			gold.container = bankBox
			socket.SysMessage( GetDictionaryEntry( 19627, socket.language )); // Gold has been deposited into your bank.
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 19628, socket.language )); // Bank is full. Gold added to your backpack.
			CreateDFNItem( player.socket, player, "0x0eed", reward.amount, "ITEM", true );
		}
	}
	else
	{
		CreateDFNItem( player.socket, player, "0x0eed", reward.amount, "ITEM", true );
	}
	socket.SysMessage( "You receive a reward: " + reward.amount + " gold!" );
}

/** @type { ( player: Character, questID: number, socket: Socket ) => void } */
function StartNextQuestInChain( player, questID, socket )
{
	if( !ValidateObject( player ))
		return;

	var completedQuest = TriggerEvent( 5801, "QuestList", questID );
	if( !completedQuest )
		return;

	// Dynamic (branching) resolution
	var nextQuestID = ResolveNextQuestID( player, completedQuest );
	if( nextQuestID <= 0 )
		return;

	// Safety: do not auto-start if already active or archived
	if( HasActiveQuestID( player, nextQuestID ))
		return;

	if( HasArchivedQuestID( player, nextQuestID ))
		return;

	var nextQuest = TriggerEvent( 5801, "QuestList", nextQuestID );
	if( nextQuest )
	{
		if( socket )
			socket.SysMessage( "A new quest has been unlocked: " + nextQuest.title );

		StartQuest( player, nextQuestID );
	}
}

/** @type { ( player: Character, questID: number ) => boolean } */
function HasArchivedQuestID( player, questID )
{
	var archivedQuestIDs = ReadArchivedQuests( player ) || [];
	var questIDInt = parseInt( questID, 10 );

	for( var i = 0; i < archivedQuestIDs.length; i++ )
	{
		if( parseInt( archivedQuestIDs[i], 10 ) == questIDInt )
			return true;
	}
	return false;
}

/** @type { ( player: Character, questID: number ) => boolean } */
function HasActiveQuestID( player, questID )
{
	var activeQuestEntries = ReadQuestProgress( player ) || [];
	var questIDInt = parseInt( questID, 10 );

	for( var i = 0; i < activeQuestEntries.length; i++ )
	{
		var entry = activeQuestEntries[i];
		if( entry.serial == player.serial && parseInt( entry.questID, 10 ) == questIDInt )
			return true;
	}
	return false;
}

/** @type { ( player: Character, condition: any ) => boolean } */
function QuestCondPasses( player, condition )
{
	if( !condition )
		return true;

	// tagEquals: { tag:"X", value: 1 }
	if( condition.tagEquals )
	{
		var tagName = condition.tagEquals.tag;
		var expectedValue = condition.tagEquals.value;

		var currentValue = player.GetTag( tagName ); // UOX3 returns 0 if missing
		return ( String( currentValue ) == String( expectedValue ));
	}

	// tagMin: { tag:"X", value: 10 }
	if( condition.tagMin )
	{
		var minTagName = condition.tagMin.tag;

		var minRequiredValue = parseInt( condition.tagMin.value, 10 );
		if( isNaN( minRequiredValue )) minRequiredValue = 0;

		var currentTagValue = parseInt( player.GetTag( minTagName ), 10 );
		if( isNaN( currentTagValue )) currentTagValue = 0;

		return ( currentTagValue >= minRequiredValue );
	}

	// completedQuest: 123
	if( condition.completedQuest != null )
		return HasArchivedQuestID( player, condition.completedQuest );

	// notCompletedQuest: 123
	if( condition.notCompletedQuest != null )
		return !HasArchivedQuestID( player, condition.notCompletedQuest );

	// hasQuest: 123
	if( condition.hasQuest != null )
		return HasActiveQuestID( player, condition.hasQuest );

	// notHasQuest: 123
	if( condition.notHasQuest != null )
		return !HasActiveQuestID( player, condition.notHasQuest );

	// worldStateEquals: { key:"dl_ws_joinedthieves", value: 1 }
	if( condition.worldStateEquals )
	{
		var worldStateKey = condition.worldStateEquals.key;
		var expectedWSValue = condition.worldStateEquals.value;

		return ( GetWorldStateInt( player, worldStateKey, 0 ) == ToIntOrZero( expectedWSValue ));
	}

	// worldStateMin: { key:"dl_ws_ironreachrep", value: 10 }
	if( condition.worldStateMin )
	{
		var worldStateKeyMin = condition.worldStateMin.key;
		var minWorldStateValue = ToIntOrZero( condition.worldStateMin.value );

		return ( GetWorldStateInt( player, worldStateKeyMin, 0 ) >= minWorldStateValue );
	}

	// always: true (optional convenience)
	if( condition.always )
		return true;

	return true;
}

/** @type { ( player: Character, quest: any ) => number } */
function ResolveNextQuestID( player, quest )
{
	if( !quest )
		return 0;

	// Branching chain: nextQuest: [ { questID, cond }, ... ]
	if( quest.nextQuest && quest.nextQuest.length )
	{
		for( var i = 0; i < quest.nextQuest.length; i++ )
		{
			var nextQuestRule = quest.nextQuest[i];
			if( !nextQuestRule )
				continue;

			if( QuestCondPasses( player, nextQuestRule.cond ))
			{
				var resolvedQuestID = parseInt( nextQuestRule.questID, 10 );
				if( !isNaN( resolvedQuestID ) && resolvedQuestID > 0 )
					return resolvedQuestID;
			}
		}
		// No rule matched
		return 0;
	}

	// Old single-field fallback
	if( quest.nextQuestID != null && quest.nextQuestID != 0 )
	{
		var fallbackQuestID = parseInt( quest.nextQuestID, 10 );
		if( !isNaN( fallbackQuestID ) && fallbackQuestID > 0 )
			return fallbackQuestID;
	}

	return 0;
}

/** @type { ( creature: Character, player: Character ) => boolean } */
function CreatureKilled( creature, player )
{
	var questProgressArray = ReadQuestProgress( player );

	for( var i = 0; i < questProgressArray.length; i++ ) 
	{
		var questEntry = questProgressArray[i];

		// Ensure the quest belongs to the current player
		if( questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest && ( quest.type == "kill" || quest.type == "timekills" || quest.type == "multi" ) && !questEntry.completed )
		{
			for( var j = 0; j < quest.targetKills.length; j++ )
			{
				var target = quest.targetKills[j];

				if( target.npcID == creature.sectionID ) 
				{
					UpdateQuestProgress( player, questEntry.questID, creature.sectionID, 1, "kill" );
					return true;
				}
			}

			return true;
		}
	}
}

/** @type { ( player: Character, item: Item, isToggledOff?: boolean ) => void } */
function ItemCollected( player, item, isToggledOff )
{
	if( !ValidateObject( player ))
		return;

	var socket = player.socket;
	if( socket == null)
		return;

	// Default the isToggledOff value
	if( typeof isToggledOff == "undefined" ) 
	{
		isToggledOff = false;
	}

	var questProgressArray = ReadQuestProgress( player );

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];

		// Ensure the quest belongs to the current player
		if( questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		var isValidType = ( quest && ( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" ));

		if( isValidType && ( isToggledOff || !questEntry.completed ))
		{
			for( var j = 0; j < quest.targetItems.length; j++ )
			{
				var target = quest.targetItems[j];


				var questSectionID = item.GetTag( "QuestSectionID" ) || item.sectionID;
				if( String( target.sectionID ) == String( item.sectionID ) || ( target.sectionID == questSectionID ))
				{
					var currentCount = questEntry.collectedItems[item.sectionID] || 0;
					var remaining = target.amount - currentCount;

					if( isToggledOff )
					{
						// Decrease the count when untoggled, ensuring it doesn't go below 0
						if( currentCount > 0 )
						{
							var amountToRemove = Math.min( item.amount, currentCount );
							UpdateQuestProgress( player, questEntry.questID, item.sectionID, -amountToRemove, "collect" );

							var questItemColor = item.GetTag( "saveColor" );
							if( questItemColor != null && !isNaN(parseInt(questItemColor)) )
							{
								item.color = parseInt( questItemColor );
							}
							else
							{
								item.color = 0; // fallback to default color (non-dyed)
							}
							item.isNewbie = false;
							item.isDyeable = true;
							item.SetTag( "QuestItem", null );
							item.SetTag( "QuestSectionID", null );
							item.RemoveScriptTrigger( 5806 ); // Quest Item script trigger

							socket.SysMessage( "You removed " + amountToRemove + " Quest Item(s)." );
						}
						else
						{
							socket.SysMessage( GetDictionaryEntry( 19629, socket.language ));// Cannot decrease further. Current count is 0.
						}
					}
					else
					{
						// Increase the count when toggled on
						if( remaining > 0 )
						{
							var amountToAdd = Math.min( item.amount, remaining );
							UpdateQuestProgress( player, questEntry.questID, item.sectionID, amountToAdd, "collect" );

							item.SetTag( "saveColor", item.color );
							item.SetTag( "saveType", item.type );
							item.color = 0x04ea; // orange hue
							item.isDyeable = false;
							item.isNewbie = true;
							item.SetTag( "QuestItem", true );
							item.SetTag( "QuestSectionID", target.sectionID );
							item.AddScriptTrigger( 5806 ); // Quest Item script trigger

							socket.SysMessage( "You set " + amountToAdd + " item(s) to Quest Item status." );
						}
						else
						{
							socket.SysMessage( "Cannot collect more. Target amount reached: " + target.amount );
						}
					}
					return; // Exit after updating progress
				}
			}
		}
	}

	socket.SysMessage( "Item does not match any quest requirements." );
}

/** @type { ( pEquipper: Character, iEquipped: Item ) => boolean } */
function EquipAttempt( pEquipper, iEquipped )
{
	if( !ValidateObject( pEquipper ))
		return false;

	var socket = pEquipper.socket;
	if( socket == null)
		return false;

	if( iEquipped.GetTag( "QuestItem" ))
	{
		socket.SysMessage( GetDictionaryEntry( 19630, socket.language )); // This item cannot be equipped as it is a quest item.

		var questProgressArray = ReadQuestProgress( pEquipper );
		var itemSectionID = iEquipped.sectionID;
		var questSectionID = iEquipped.GetTag("QuestSectionID") || itemSectionID;

		for( var i = 0; i < questProgressArray.length; i++ )
		{
			var questEntry = questProgressArray[i];

			if( questEntry.serial != pEquipper.serial )
				continue;

			var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
			var isValidType = ( quest && ( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" ));

			if( !isValidType )
				continue;

			for( var j = 0; j < quest.targetItems.length; j++ )
			{
				var target = quest.targetItems[j];

				// Match by tag or item.sectionID
				if( target.sectionID == questSectionID || target.sectionID == itemSectionID )
				{
					var currentCount = questEntry.collectedItems[itemSectionID] || 0;

					if( currentCount > 0 )
					{
						var amountToRemove = Math.min( iEquipped.amount, currentCount );
						UpdateQuestProgress( pEquipper, questEntry.questID, itemSectionID, -amountToRemove, "collect" );

						var questItemColor = iEquipped.GetTag( "saveColor" );
						if( questItemColor != null && !isNaN(parseInt(questItemColor)) )
						{
							iEquipped.color = parseInt( questItemColor );
						}
						else
						{
							iEquipped.color = 0; // fallback to default color (non-dyed)
						}
	
						iEquipped.isNewbie = false;
						iEquipped.isDyeable = true;
						iEquipped.SetTag( "QuestItem", null );
						iEquipped.SetTag( "QuestSectionID", null );
						iEquipped.RemoveScriptTrigger( 5806 );
						iEquipped.Refresh();

						socket.SysMessage( "You removed Quest Item status from the item." );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 19629, socket.language )); // Cannot decrease further. Current count is 0.
					}
				}
			}
		}
		return false;
	}
	return true;
}

/** @type { ( pPlayer: Character, skill: number, skillGainAmount: number ) => boolean } */
function AccelerateSkillGain( pPlayer, skill, skillGainAmount )
{
	var activeQuests = ReadQuestProgress( pPlayer );

	for( var i = 0; i < activeQuests.length; i++ ) 
	{
		var questEntry = activeQuests[i];

		// Ensure the quest entry matches the player's serial
		if( questEntry.serial != pPlayer.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest && quest.type == "skillgain" && !questEntry.completed ) 
		{
			if( pPlayer.region.id == quest.targetRegion && skill == quest.targetSkill ) 
			{
				var currentSkillPoints = pPlayer.baseskills[skill];
				var acceleratedGain = RandomNumber( quest.minPoint, quest.MaxPoint );

				if(( currentSkillPoints + acceleratedGain ) >= quest.maxSkillPoints )
				{
					UpdateQuestProgress( pPlayer, questEntry.questID, skill, quest.maxSkillPoints - currentSkillPoints, "skillgain" );
				}
				else
				{
					// Apply accelerated skill gain and update progress
					pPlayer.AddSkill( skill, acceleratedGain, false );
					UpdateQuestProgress( pPlayer, questEntry.questID, skill, acceleratedGain, "skillgain" );
				}

				return true;
			}
		}
	}

	return true;
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

// Map common aliases -> canonical keys used by baseskills/skillCaps
var skillAliasMap = {
	// combat
	"swords": "swordsmanship", "swordsmanship": "swordsmanship",
	"mace": "macefighting", "macing": "macefighting", "macefighting": "macefighting",
	"fencing": "fencing", "wrestling": "wrestling", "archery": "archery",
	// magic
	"magery": "magery", "evalint": "evaluatingintelligence", "evaluating intelligence": "evaluatingintelligence",
	"evaluatingintelligence": "evaluatingintelligence", "meditation": "meditation",
	"resist": "magicresistance", "resistingspells": "magicresistance", "magicresistance": "magicresistance",
	// crafts & misc (just a few common ones; names already match)
	"tactics": "tactics", "anatomy": "anatomy", "healing": "healing",
	"blacksmith": "blacksmith", "carpentry": "carpentry", "tinkering": "tinkering",
	"tailoring": "tailoring", "inscription": "inscription", "musicianship": "musicianship",
	"stealth": "stealth", "hiding": "hiding", "provocation": "provocation", "peacemaking": "peacemaking",
	"cartography": "cartography", "mining": "mining", "lumberjacking": "lumberjacking",
	// newer
	"chivalry": "chivalry", "bushido": "bushido", "ninjitsu": "ninjitsu", "spellweaving": "spellweaving"
};

/** @type { ( skill: string | number ) => string } */
function NormalizeKey( skill )
{
	return String( skill || "" ).replace( /\s+/g, "" ).toLowerCase();
}

/** @type { ( skillNameOrId: string | number ) => string | null } */
function resolveSkillKey( skillNameOrId )
{
	var skillNumber = parseInt( skillNameOrId, 10 );
	if( !isNaN( skillNumber ))
	{
		var getSkill = GetSkillName(skillNumber);
		return ( getSkill && getSkill !== "unknown skill" ) ? getSkill : null;
	}

	var key = NormalizeKey( skillNameOrId );
	// exact match first
	if( skillAliasMap.hasOwnProperty( key ))
		return skillAliasMap[key];
	// if caller already used canonical (e.g. "magery"), allow it
	return key || null;
}

/** @type { ( player: Character, key: string ) => number } */
function GetBaseSkillTenths( player, key )
{
	// baseskills.* is 0..1000
	var value = player.baseskills[key];
	return ( typeof value === "number" ) ? ( value | 0 ) : 0;
}

/** @type { ( player: Character, key: string, valTenths: number ) => void } */
function SetBaseSkillTenths( player, key, valTenths )
{
	player.baseskills[key] = ( valTenths | 0 );
}

/** @type { ( player: Character, key: string ) => number } */
function GetSkillCapTenths( player, key )
{
	var cap = player.skillCaps[key];
	// fallback to 100.0 if undefined/not numeric
	return ( typeof cap === "number" && cap > 0 ) ? ( cap | 0 ) : 1000;
}

/** @type { ( player: Character, reward: any, socket: Socket ) => void } */
function SkillReward( player, reward, socket )
{
	var key = resolveSkillKey( reward.skill );
	if( !key || player.baseskills[key] === undefined )
	{
		socket.SysMessage( "Quest reward error: Unknown skill '" + reward.skill + "'." );
		return;
	}

	var addTenths = Math.round(( +reward.amount || 0 ) * 10 );
	if( addTenths === 0 )
	{
		socket.SysMessage( "Quest reward notice: 0 skill points specified for " + reward.skill + "." );
		return;
	}

	var cur = GetBaseSkillTenths( player, key );
	var cap = GetSkillCapTenths( player, key );
	var nxt = Math.max( 0, Math.min( cur + addTenths, cap ));

	SetBaseSkillTenths( player, key, nxt );

	var gained = ( nxt - cur ) / 10.0;
	if( gained > 0 )
		socket.SysMessage( "You gained " + gained.toFixed(1) + " in " + key + "!" );
	else
		socket.SysMessage( "Your " + key + " is already at its cap." );
}

// Optional pooled points (stored in tenths under 'UnspentSkillPoints' tag)
/** @type { ( player: Character, reward: any, socket: Socket ) => void } */
function SkillPointsPoolReward( player, reward, socket )
{
	var addTenths = Math.round(( +reward.amount || 0 ) * 10 );
	if( addTenths <= 0 )
	{ 
		socket.SysMessage( "Quest reward notice: 0 pooled skill points specified." );
		return;
	}

	var cur = parseInt( player.GetTag( "UnspentSkillPoints" ), 10 ) || 0;
	player.SetTag( "UnspentSkillPoints", String( cur + addTenths ));
	socket.SysMessage( "You received " + ( addTenths / 10 ).toFixed( 1 ) + " unspent skill points!" );
}

// Spend pooled points into a specific skill
/** @type { ( player: Character, socket: Socket, skillNameOrId: string | number, amount: number ) => void } */
function SpendSkillPoints( player, socket, skillNameOrId, amount ) 
{
	var key = resolveSkillKey( skillNameOrId );
	if( !key || player.baseskills[key] === undefined )
	{
		socket.SysMessage( "Unknown skill: " + skillNameOrId );
		return;
	}

	var pool = parseInt(player.GetTag( "UnspentSkillPoints" ), 10 ) || 0;
	var spendTenths = Math.min( pool, Math.round(( +amount || 0 ) * 10 ));
	if( spendTenths <= 0 ) 
	{ 
		socket.SysMessage( "No unspent points to spend." );
		return;
	}

	var cur = GetBaseSkillTenths( player, key );
	var cap = GetSkillCapTenths( player, key );
	var room = Math.max( 0, cap - cur );
	var applied = Math.min( room, spendTenths );
	if( applied <= 0 )
	{ 
		socket.SysMessage( "That skill is already at its cap." );
		return;
	}

	SetBaseSkillTenths( player, key, cur + applied );
	player.SetTag( "UnspentSkillPoints", String(pool - applied ));
	socket.SysMessage( "Allocated " + ( applied / 10 ).toFixed( 1 ) + " points to " + key + "." );
}

//////////////////////////////////////////////////////////////////////////////////
//							Save/Read Functions									//
//////////////////////////////////////////////////////////////////////////////////

/** @type { ( player: Character ) => any } */
function ReadPlayerSettings( player )
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "PlayerSettings_" + userAccount.id + ".jsdata";

	var settings = {};

	mFile.Open( fileName, "r", "Settings" );
	if( mFile && mFile.Length() > 0 )
	{
		while( !mFile.EOF() )
		{
			var line = manualTrim( mFile.ReadUntil( "\n" )); // Use manualTrim instead of trim
			if( line == "" )
				continue; // Skip empty lines

			var parts = line.split("=");
			if( parts.length == 2 )
			{
				var key = manualTrim( parts[0] ); // Use manualTrim on the key
				var value = manualTrim( parts[1] ); // Use manualTrim on the value

				// Check if the value is a quoted string
				if( value.charAt( 0 ) == '"' && value.charAt( value.length - 1 ) == '"' )
				{
					// Remove quotes and unescape internal quotes
					settings[key] = value.substring( 1, value.length - 1 ).replace( /\\"/g, '"' );
				} 
				else if( value == "1" || value == "0" )
				{
					// Convert "1"/"0" to boolean true/false
					settings[key] = value == "1";
				}
				else if( !isNaN( value ))
				{
					// Convert numeric strings to numbers
					settings[key] = parseFloat( value );
				}
				else
				{
					// Keep as string for other cases
					settings[key] = value;
				}
			}
		}
		mFile.Close();
		mFile.Free();
	}
	else
	{
		if( mFile )
		{
			mFile.Free();
		}
	}

	return settings; // Return parsed settings
}

/** @type { ( player: Character, settings: any ) => boolean } */
function SavePlayerSettings( player, settings ) 
{
	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "PlayerSettings_" + userAccount.id + ".jsdata";

	mFile.Open( fileName, "w", "Settings" );
	if( mFile )
	{
		for( var key in settings )
		{
			if( settings.hasOwnProperty( key ))
			{
				var value = settings[key];

				// Serialize based on data type
				if( typeof value == "string" ) 
				{
					mFile.Write( key + "=" + '"' + value.replace( /"/g, '\\"' ) + '"' + "\n" ); // Escape quotes for strings
				} 
				else if( typeof value == "boolean" )
				{
					mFile.Write( key + "=" + ( value ? "1" : "0" ) + "\n" ); // Boolean as 1/0
				} 
				else if( !isNaN(value ))
				{
					mFile.Write( key + "=" + value + "\n" ); // Numbers directly
				}
				else
				{
					// Fallback for unsupported data types
					mFile.Write( key + "=" + ( value || "undefined" ) + "\n" );
				}
			}
		}
		mFile.Close();
		mFile.Free();
		return true; // Save succeeded
	}

	return false; // Failed to save settings
}

/** @type { ( player: Character, failedQuest: any ) => void } */
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
			"Failed=1\n" +

		mFile.Write( failedEntry );
		mFile.Close();
		mFile.Free();
	}
}

/** @type { ( player: Character ) => number[] } */
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

		while( !mFile.EOF() )
		{
			var line = mFile.ReadUntil( "\n" );
			line = manualTrim( line ); // Remove leading/trailing spaces

			// Skip empty lines
			if( line == "" )
			{
				continue;
			}

			// Split the line into key and value
			var parts = line.split( "=" );
			if( parts.length == 2 )
			{
				var key = manualTrim( parts[0] ).toLowerCase();
				var value = manualTrim( parts[1] );

				if( key == "questid" )
				{
					currentQuestID = parseInt( value, 10 ); // Parse the QuestID
				} 
				else if( key == "serial" )
				{
					currentSerial = parseInt( value, 10 ); // Parse the Serial
				}

				// If we reach the end of an entry and Serial matches the player, save the QuestID
				if( key == "failed" && value == "1" && currentQuestID != null && currentSerial == player.serial )
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

/** @type { ( player: Character, completedQuest: any ) => void } */
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
		if( quest && quest.type == "skillgain" )
		{
			skillProgressStr = "SkillProgress=" + ( completedQuest.skillProgress || 0 ) + "\n" +
				"MaxSkillPoints=" + ( quest.maxSkillPoints || 0 ) + "\n" +
				"TargetSkill=" + ( quest.targetSkill || "null" ) + "\n";
		}

		// Add delivery progress
		var deliveryProgressStr = "";
		if( quest && quest.type == "delivery" )
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
			"LastCompleted=" + Date.now() + "\n" + // Record completion time
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

/** @type { ( player: Character ) => number[] } */
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

		while( !mFile.EOF() )
		{
			var line = mFile.ReadUntil( "\n" );
			line = manualTrim( line ); // Remove leading/trailing spaces

			// Skip empty lines
			if( line == "" ) 
			{
				// Check if we reached the end of an entry
				if( currentQuestID != null && currentSerial == player.serial ) 
				{
					archivedQuests.push( currentQuestID ); // Add the quest ID
				}
				currentQuestID = null; // Reset for the next entry
				currentSerial = null; // Reset for the next entry
				continue;
			}

			// Split the line into key and value
			var parts = line.split( "=" );
			if( parts.length == 2 ) 
			{
				var key = manualTrim( parts[0] ).toLowerCase();
				var value = manualTrim( parts[1] );

				if( key == "questid" ) 
				{
					currentQuestID = parseInt( value, 10 ); // Parse quest ID
				}
				else if( key == "serial" )
				{
					currentSerial = parseInt( value, 10 ); // Parse player serial
				}

				// Add quest ID if marked as completed
				if( key == "completed" && value == "1" && currentQuestID != null && currentSerial == player.serial ) 
				{
					archivedQuests.push( currentQuestID );
				}
			}
		}

		mFile.Close();
		mFile.Free();
	} 

	return archivedQuests; // Return array of quest IDs
}

/** @type { ( player: Character, questProgressArray: any[] ) => boolean } */
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
				"LastAccepted=" + ( progressEntry.lastAccepted || 0 ) + "\n" + // Save the lastAccepted timestamp
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

/** @type { ( player: Character ) => any[] } */
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
		while( !mFile.EOF() )
		{
			var line = mFile.ReadUntil( "\n" );

			// Normalize line: Remove unexpected characters
			line = manualTrim( line.replace( /[^\x20-\x7E]/g, "" ));

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

				var key = manualTrim( parts[0] ).toLowerCase(); // Convert key to lowercase
				var value = manualTrim( parts[1] );

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
	return questProgressArray;
}

/** @type { ( entry: any, player: Character ) => void } */
function finalizeQuestEntry( entry, player )
{
	entry.serial = parseInt( entry.serial || "0", 10 );
	if( isNaN( entry.serial ) )
		entry.serial = 0;
	entry.questID = parseInt( entry.questid || "0", 10 );
	entry.completed = entry.completed == "1";
	entry.questTurnIn = entry.questturnin == "1";
	entry.startTime = parseInt( entry.starttime || "0", 10 );
	entry.timeLimit = parseInt( entry.timelimit || "0", 10 );
	entry.skillProgress = parseFloat( entry.skillprogress || "0.0" );
	entry.targetSkill = parseInt( entry.targetskill || "-1", 10 );
	entry.targetRegion = parseInt( entry.targetregion || "0", 10 );
	entry.maxSkillPoints = parseFloat( entry.maxskillpoints || "50.0" );
	entry.lastAccepted = parseInt( entry.lastaccepted || "0", 10 ); // Add lastAccepted timestamp

	processCollectedItems(entry, player);
	processKills(entry, player);
}

/** @type { ( entry: any, player: Character ) => void } */
function processCollectedItems( entry, player )
{
	entry.collectedItems = {};
	var collectedItemsStr = entry.collecteditems || ""; // Use consistent key

	if( collectedItemsStr == "" ) 
	{
		return;
	}

	var collectedItems = collectedItemsStr.split( "," );
	for( var i = 0; i < collectedItems.length; i++ )
	{
		var pair = collectedItems[i].split( ":" );
		if( pair.length == 2 )
		{
			var key = manualTrim( pair[0] );
			var value = parseInt( manualTrim( pair[1] ), 10 );
			entry.collectedItems[key] = value;
		}
	}
}

/** @type { ( entry: any, player: Character ) => void } */
function processKills( entry, player )
{
	// Initialize harvestKills as an object if not already
	entry.harvestKills = {};

	// Use the correct key to extract the raw kills string
	var killsStr = entry.harvestkills || ""; // Note: lowercase to match file format

	// If killsStr is empty, there�s nothing to process
	if( killsStr == "" )
	{
		return;
	}

	var harvestKills = killsStr.split( "," );
	for( var i = 0; i < harvestKills.length; i++ ) 
	{
		var pair = harvestKills[i].split( ":" );
		if( pair.length == 2 )
		{
			var key = manualTrim( pair[0] );
			var value = parseInt( manualTrim( pair[1] ), 10 );
			entry.harvestKills[key] = value;
		}
	}
}

/** @type { ( str: string ) => string } */
function manualTrim( str )
{
	return str.replace( /^\s+|\s+$/g, "" );
}

//////////////////////////////////////////////////////////////////////////////////
//                          World State (file-backed)                           //
//////////////////////////////////////////////////////////////////////////////////

// PlayerSettings key used for world state blob (PER-CHAR by serial)
/** @type { ( player: Character ) => string } */
function GetWorldStateKey( player )
{
	// Stored inside the account-based PlayerSettings file, but keyed per character.
	return "WorldState_" + player.serial;
}

/** @type { ( player: Character ) => Record<string, number> } */
function ReadWorldState( player )
{
	var playerSettings = ReadPlayerSettings( player ) || {};
	var worldStateKey = GetWorldStateKey( player );
	var rawBlob = playerSettings[worldStateKey];

	// PlayerSettings parses quoted strings, so rawBlob is either string or undefined
	if( typeof rawBlob != "string" || rawBlob == "" )
		return {};

	return ParseWorldStateBlob( rawBlob );
}

/** @type { ( player: Character, worldState: Record<string, number> ) => boolean } */
function SaveWorldState( player, worldState )
{
	var playerSettings = ReadPlayerSettings( player ) || {};
	var worldStateKey = GetWorldStateKey( player );

	playerSettings[worldStateKey] = SerializeWorldStateBlob( worldState );
	return SavePlayerSettings( player, playerSettings );
}

/** @type { ( player: Character, stateKey: string, defaultValue?: number ) => number } */
function GetWorldStateInt( player, stateKey, defaultValue )
{
	var worldState = ReadWorldState( player );
	var normalizedKey = NormalizeWSKey( stateKey );

	if( worldState.hasOwnProperty( normalizedKey ) )
		return ToIntOrZero( worldState[normalizedKey] );

	return ToIntOrZero( defaultValue );
}

/** @type { ( player: Character, stateKey: string, value: number | null | undefined ) => boolean } */
function SetWorldStateInt( player, stateKey, value )
{
	var worldState = ReadWorldState( player );
	var normalizedKey = NormalizeWSKey( stateKey );

	// null/undefined => remove key
	if( value === null || value === undefined )
	{
		if( worldState.hasOwnProperty( normalizedKey ) )
			delete worldState[normalizedKey];

		return SaveWorldState( player, worldState );
	}

	worldState[normalizedKey] = ToIntOrZero( value );
	return SaveWorldState( player, worldState );
}

/** @type { ( player: Character, stateKey: string, delta: number, min?: number | null, max?: number | null ) => number } */
function AddWorldStateDelta( player, stateKey, delta, min, max )
{
	var currentValue = GetWorldStateInt( player, stateKey, 0 );
	var nextValue = currentValue + ToIntOrZero( delta );

	if( min != null && nextValue < min ) nextValue = min;
	if( max != null && nextValue > max ) nextValue = max;

	SetWorldStateInt( player, stateKey, nextValue );
	return nextValue;
}

/** @type { ( stateKey: string ) => string } */
function NormalizeWSKey( stateKey )
{
	// Keep it simple + stable. Lowercase prevents duplicates.
	return String( stateKey || "" ).replace( /^\s+|\s+$/g, "" ).toLowerCase();
}

/** @type { ( rawBlob: string ) => Record<string, number> } */
function ParseWorldStateBlob( rawBlob )
{
	var worldState = {};
	rawBlob = String( rawBlob || "" );

	if( rawBlob == "" )
		return worldState;

	var pairs = rawBlob.split( "," );
	for( var i = 0; i < pairs.length; i++ )
	{
		var pairText = manualTrim( pairs[i] );
		if( pairText == "" )
			continue;

		var keyValuePair = pairText.split( ":" );
		if( keyValuePair.length != 2 )
			continue;

		var normalizedKey = NormalizeWSKey( keyValuePair[0] );
		if( normalizedKey == "" )
			continue;

		var valueInt = parseInt( manualTrim( keyValuePair[1] ), 10 );
		if( isNaN( valueInt ) )
			valueInt = 0;

		worldState[normalizedKey] = valueInt;
	}

	return worldState;
}

/** @type { ( worldState: Record<string, number> ) => string } */
function SerializeWorldStateBlob( worldState )
{
	if( !worldState )
		return "";

	// stable order (so diffs are clean)
	var normalizedKeys = [];
	for( var key in worldState )
	{
		if( worldState.hasOwnProperty( key ) )
		{
			var normalizedKey = NormalizeWSKey( key );
			if( normalizedKey != "" )
				normalizedKeys.push( normalizedKey );
		}
	}
	normalizedKeys.sort();

	var outBlob = "";
	for( var i = 0; i < normalizedKeys.length; i++ )
	{
		var normalizedKey = normalizedKeys[i];
		var valueInt = ToIntOrZero( worldState[normalizedKey] );

		if( outBlob.length > 0 ) outBlob += ",";
		outBlob += normalizedKey + ":" + valueInt;
	}

	return outBlob;
}