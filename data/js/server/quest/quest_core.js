/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( player: Character, questID: number ) => void } */
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
		return;
	}

	var collectedItems = {};
	var collectedItemGroups = {};
	var harvestKills = {};
	var harvestKillGroups = {};

	var quest = TriggerEvent( 5801, "QuestList", questID );

	if( quest.targetItems )
	{
		for( var targetItemIndex = 0; targetItemIndex < quest.targetItems.length; targetItemIndex++ )
		{
			var targetItem = quest.targetItems[targetItemIndex];
			collectedItems[targetItem.sectionID] = 0;
		}
	}

	if( quest.targetItemGroups )
	{
		for( var targetGroupIndex = 0; targetGroupIndex < quest.targetItemGroups.length; targetGroupIndex++ )
		{
			var targetGroup = quest.targetItemGroups[targetGroupIndex];
			if( targetGroup && targetGroup.groupID )
			{
				collectedItemGroups[String( targetGroup.groupID )] = 0;
			}
		}
	}

	if( quest.targetKills )
	{
		for( var targetKillIndex = 0; targetKillIndex < quest.targetKills.length; targetKillIndex++ )
		{
			var targetKill = quest.targetKills[targetKillIndex];
			harvestKills[targetKill.npcID] = 0;
		}
		player.AddScriptTrigger( 5810 );
	}

	if( quest.targetKillGroups )
	{
		for( var targetKillGroupIndex = 0; targetKillGroupIndex < quest.targetKillGroups.length; targetKillGroupIndex++ )
		{
			var targetKillGroup = quest.targetKillGroups[targetKillGroupIndex];
			if( targetKillGroup && targetKillGroup.groupID )
			{
				harvestKillGroups[String( targetKillGroup.groupID )] = 0;
			}
		}
	}

	if( quest.type == "delivery" && quest.deliveryItem )
	{
		var packageItem = CreateDFNItem( player.socket, player, quest.deliveryItem.sectionID, quest.deliveryItem.amount, "ITEM", true );
		if( ValidateObject( packageItem ))
		{
			packageItem.name = quest.deliveryItem.name || "Unknown Package";
			packageItem.SetTag( "saveColor", packageItem.color );
			packageItem.color = 0x04ea;
			packageItem.isDyeable = false;
			packageItem.isNewbie = true;
			packageItem.SetTag( "QuestItem", true );
			packageItem.SetTag( "delivery", true );
			packageItem.SetTag( "QuestSectionID", packageItem.sectionID );
			packageItem.AddScriptTrigger( 5806 );
			player.SysMessage( "You have received a " + quest.deliveryItem.name + " for delivery." );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 19615, socket.language ));
		}
	}

	var initialSkillLevel = 0;
	if( quest.type == "skillgain" )
	{
		if( !player.GetTag( "AcceleratedSkillGain" ))
		{
			initialSkillLevel = player.baseskills[quest.targetSkill];
			player.SetTag( "AcceleratedSkillGain", quest.targetSkill );
			player.AddScriptTrigger( 5811 );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 19616, socket.language ));
		}
	}

	questProgressArray.push({
		serial: player.serial,
		questID: questID,
		questProgress: 0,
		harvestKills: harvestKills,
		harvestKillGroups: harvestKillGroups,
		collectedItems: collectedItems,
		collectedItemGroups: collectedItemGroups,
		skillProgress: initialSkillLevel,
		targetSkill: quest.targetSkill || -1,
		targetRegion: quest.targetRegion || 0,
		maxSkillPoints: quest.maxSkillPoints || 50.0,
		startTime: quest.timeLimit ? Date.now() : 0,
		timeLimit: quest.timeLimit ? quest.timeLimit * 1000 : 0,
		lastAccepted: Date.now(),
		completed: false,
		questTurnIn: false,
		nextQuestID: quest.nextQuestID || null
	});

	WriteQuestProgress( player, questProgressArray );

	socket.SysMessage( GetDictionaryEntry( 19617, socket.language ));

	if( quest.timeLimit )
	{
		player.StartTimer( quest.timeLimit * 1000, questID, 5800 );
		var minutes = Math.floor( quest.timeLimit / 60 );
		var seconds = quest.timeLimit % 60;
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

/** @type { ( itemSectionID: string, itemQuestSectionID: string, targetGroup: any ) => boolean } */
function DoesItemMatchTargetGroup( itemSectionID, itemQuestSectionID, targetGroup )
{
	if( !targetGroup || !targetGroup.items || !targetGroup.items.length )
	{
		return false;
	}

	var normalizedItemSectionID = String( itemSectionID );
	var normalizedQuestSectionID = String( itemQuestSectionID );

	for( var groupItemIndex = 0; groupItemIndex < targetGroup.items.length; groupItemIndex++ )
	{
		var groupItem = targetGroup.items[groupItemIndex];
		if( !groupItem || !groupItem.sectionID )
		{
			continue;
		}

		var groupSectionID = String( groupItem.sectionID );
		if( groupSectionID == normalizedItemSectionID || groupSectionID == normalizedQuestSectionID )
		{
			return true;
		}
	}

	return false;
}

/** @type { ( creature: Character, targetKillGroup: any ) => boolean } */
function DoesNpcMatchTargetKillGroup( creature, targetKillGroup )
{
	if( !ValidateObject( creature ) || !targetKillGroup )
	{
		return false;
	}

	var creatureSectionID = String( creature.sectionID );
	var creatureRaceID = GetCreatureRaceID( creature );
	var creatureRaceName = GetCreatureRaceName( creature );

	if( targetKillGroup.npcs && targetKillGroup.npcs.length )
	{
		for( var groupNpcIndex = 0; groupNpcIndex < targetKillGroup.npcs.length; groupNpcIndex++ )
		{
			var groupNpc = targetKillGroup.npcs[groupNpcIndex];
			if( !groupNpc || !groupNpc.npcID )
			{
				continue;
			}

			if( String( groupNpc.npcID ) == creatureSectionID )
			{
				return true;
			}
		}
	}

	if( typeof targetKillGroup.raceID != "undefined" )
	{
		var targetRaceID = parseInt( targetKillGroup.raceID, 10 );
		if( !isNaN( targetRaceID ) && targetRaceID > 0 && creatureRaceID == targetRaceID )
		{
			return true;
		}
	}

	if( targetKillGroup.raceName )
	{
		if( creatureRaceName != "" && creatureRaceName == String( targetKillGroup.raceName ).toLowerCase() )
		{
			return true;
		}
	}

	if( targetKillGroup.raceIDs && targetKillGroup.raceIDs.length )
	{
		for( var raceIDIndex = 0; raceIDIndex < targetKillGroup.raceIDs.length; raceIDIndex++ )
		{
			var raceIDEntry = parseInt( targetKillGroup.raceIDs[raceIDIndex], 10 );
			if( !isNaN( raceIDEntry ) && raceIDEntry > 0 && creatureRaceID == raceIDEntry )
			{
				return true;
			}
		}
	}

	if( targetKillGroup.raceNames && targetKillGroup.raceNames.length )
	{
		for( var raceNameIndex = 0; raceNameIndex < targetKillGroup.raceNames.length; raceNameIndex++ )
		{
			var raceNameEntry = String( targetKillGroup.raceNames[raceNameIndex] ).toLowerCase();
			if( creatureRaceName != "" && creatureRaceName == raceNameEntry )
			{
				return true;
			}
		}
	}

	return false;
}

/** @type { ( creature: Character ) => number } */
function GetCreatureRaceID( creature )
{
	if( !ValidateObject( creature ) || !creature.race )
	{
		return 0;
	}

	var raceID = parseInt( creature.race.id, 10 );
	if( isNaN( raceID ))
	{
		return 0;
	}

	return raceID;
}

/** @type { ( creature: Character ) => string } */
function GetCreatureRaceName( creature )
{
	if( !ValidateObject( creature ) || !creature.race || !creature.race.name )
	{
		return "";
	}

	return String( creature.race.name ).toLowerCase();
}

/** @type { ( quest: any, targetEntry: any ) => number } */
function ResolveKillTargetRegion( quest, targetEntry )
{
	if( targetEntry && typeof targetEntry.targetRegion != "undefined" )
	{
		var targetRegion = parseInt( targetEntry.targetRegion, 10 );
		if( !isNaN( targetRegion ) && targetRegion > 0 )
		{
			return targetRegion;
		}
	}

	if( quest && typeof quest.targetRegion != "undefined" )
	{
		var questRegion = parseInt( quest.targetRegion, 10 );
		if( !isNaN( questRegion ) && questRegion > 0 )
		{
			return questRegion;
		}
	}

	return 0;
}

/** @type { ( player: Character, creature: Character, requiredRegion: number ) => boolean } */
function IsKillInRequiredRegion( player, creature, requiredRegion )
{
	requiredRegion = parseInt( requiredRegion, 10 );
	if( isNaN( requiredRegion ) || requiredRegion <= 0 )
	{
		return true;
	}

	var playerRegionId = 0;
	var creatureRegionId = 0;

	if( player && player.region )
	{
		playerRegionId = parseInt( player.region.id, 10 );
		if( isNaN( playerRegionId ))
		{
			playerRegionId = 0;
		}
	}

	if( creature && creature.region )
	{
		creatureRegionId = parseInt( creature.region.id, 10 );
		if( isNaN( creatureRegionId ))
		{
			creatureRegionId = 0;
		}
	}

	return ( playerRegionId == requiredRegion || creatureRegionId == requiredRegion );
}

/** @type { ( quest: any, questEntry: any ) => boolean } */
function AreAllQuestObjectivesComplete( quest, questEntry )
{
	var allObjectivesCompleted = true;

	if( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" )
	{
		if( quest.targetItems )
		{
			for( var targetItemIndex = 0; targetItemIndex < quest.targetItems.length; targetItemIndex++ )
			{
				var exactTarget = quest.targetItems[targetItemIndex];
				if( !questEntry.collectedItems || ( questEntry.collectedItems[exactTarget.sectionID] || 0 ) < exactTarget.amount )
				{
					allObjectivesCompleted = false;
				}
			}
		}

		if( quest.targetItemGroups )
		{
			for( var targetGroupIndex = 0; targetGroupIndex < quest.targetItemGroups.length; targetGroupIndex++ )
			{
				var targetGroup = quest.targetItemGroups[targetGroupIndex];
				if( !targetGroup || !targetGroup.groupID )
				{
					continue;
				}

				if( !questEntry.collectedItemGroups || ( questEntry.collectedItemGroups[String( targetGroup.groupID )] || 0 ) < targetGroup.amount )
				{
					allObjectivesCompleted = false;
				}
			}
		}
	}

	if( quest.type == "kill" || quest.type == "timekills" || quest.type == "multi" )
	{
		if( quest.targetKills )
		{
			for( var targetKillIndex = 0; targetKillIndex < quest.targetKills.length; targetKillIndex++ )
			{
				var targetKill = quest.targetKills[targetKillIndex];
				if( !questEntry.harvestKills || ( questEntry.harvestKills[targetKill.npcID] || 0 ) < targetKill.amount )
				{
					allObjectivesCompleted = false;
				}
			}
		}

		if( quest.targetKillGroups )
		{
			for( var targetKillGroupIndex = 0; targetKillGroupIndex < quest.targetKillGroups.length; targetKillGroupIndex++ )
			{
				var targetKillGroup = quest.targetKillGroups[targetKillGroupIndex];
				if( !targetKillGroup || !targetKillGroup.groupID )
				{
					continue;
				}

				if( !questEntry.harvestKillGroups || ( questEntry.harvestKillGroups[String( targetKillGroup.groupID )] || 0 ) < targetKillGroup.amount )
				{
					allObjectivesCompleted = false;
				}
			}
		}
	}

	return allObjectivesCompleted;
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

	for( var questEntryIndex = 0; questEntryIndex < questProgressArray.length; questEntryIndex++ )
	{
		var questEntry = questProgressArray[questEntryIndex];

		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( !quest )
		{
			continue;
		}

		if( !questEntry.collectedItems )
		{
			questEntry.collectedItems = {};
		}
		if( !questEntry.collectedItemGroups )
		{
			questEntry.collectedItemGroups = {};
		}
		if( !questEntry.harvestKills )
		{
			questEntry.harvestKills = {};
		}

		if( !questEntry.harvestKillGroups )
		{
			questEntry.harvestKillGroups = {};
		}

		if( type == "collect" || type == "collectgroup" )
		{
			if( quest.targetItems )
			{
				for( var targetItemIndex = 0; targetItemIndex < quest.targetItems.length; targetItemIndex++ )
				{
					var targetItem = quest.targetItems[targetItemIndex];

					if( String( targetItem.sectionID ) == String( identifier ))
					{
						questEntry.collectedItems[String( identifier )] = Math.max( 0, ( questEntry.collectedItems[String( identifier )] || 0 ) + progressValue );
						if( questEntry.collectedItems[String( identifier )] > targetItem.amount )
						{
							questEntry.collectedItems[String( identifier )] = targetItem.amount;
						}
					}
				}
			}

			if( type == "collectgroup" && quest.targetItemGroups )
			{
				for( var targetGroupIndex = 0; targetGroupIndex < quest.targetItemGroups.length; targetGroupIndex++ )
				{
					var targetGroup = quest.targetItemGroups[targetGroupIndex];
					if( !targetGroup || !targetGroup.groupID )
					{
						continue;
					}

					var groupID = String( targetGroup.groupID );
					questEntry.collectedItemGroups[groupID] = Math.max( 0, ( questEntry.collectedItemGroups[groupID] || 0 ) + progressValue );

					if( questEntry.collectedItemGroups[groupID] > targetGroup.amount )
					{
						questEntry.collectedItemGroups[groupID] = targetGroup.amount;
					}

					break;
				}
			}
		}

		if( type == "kill" || type == "killgroup" )
		{
			if( type == "kill" && quest.targetKills )
			{
				for( var targetKillIndex = 0; targetKillIndex < quest.targetKills.length; targetKillIndex++ )
				{
					var targetKill = quest.targetKills[targetKillIndex];

					if( String( targetKill.npcID ) == String( identifier ) )
					{
						questEntry.harvestKills[String( identifier )] = ( questEntry.harvestKills[String( identifier )] || 0 ) + progressValue;
						if( questEntry.harvestKills[String( identifier )] > targetKill.amount )
						{
							questEntry.harvestKills[String( identifier )] = targetKill.amount;
						}
					}
				}
			}

			if( type == "killgroup" && quest.targetKillGroups )
			{
				for( var targetKillGroupIndex = 0; targetKillGroupIndex < quest.targetKillGroups.length; targetKillGroupIndex++ )
				{
					var targetKillGroup = quest.targetKillGroups[targetKillGroupIndex];
					if( !targetKillGroup || !targetKillGroup.groupID )
					{
						continue;
					}

					var killGroupID = String( targetKillGroup.groupID );
					questEntry.harvestKillGroups[killGroupID] = ( questEntry.harvestKillGroups[killGroupID] || 0 ) + progressValue;

					if( questEntry.harvestKillGroups[killGroupID] > targetKillGroup.amount )
					{
						questEntry.harvestKillGroups[killGroupID] = targetKillGroup.amount;
					}

					break;
				}
			}
		}

		if( quest.type == "delivery" && quest.targetDelivery.npcID == identifier )
		{
			if( String( identifier ) == String( quest.targetDelivery.npcID ))
			{
				var pack = player.pack;
				var hasItem = false;
				var totalDeliveryAmount = 0;
				var questItemsToProcess = [];

				if( !ValidateObject( pack ))
				{
					socket.SysMessage( GetDictionaryEntry( 19621, socket.language ));
				}
				else
				{
					for( var currentItem = pack.FirstItem(); !pack.FinishedItems(); currentItem = pack.NextItem())
					{
						if( !ValidateObject( currentItem ))
						{
							continue;
						}

						var questSectionID = currentItem.GetTag( "QuestSectionID" ) || currentItem.sectionID;

						if( currentItem.GetTag( "QuestItem" ) && String( questSectionID ) == String( quest.deliveryItem.sectionID ))
						{
							questItemsToProcess.push( currentItem );
							totalDeliveryAmount += currentItem.amount;
						}
					}

					if( totalDeliveryAmount >= quest.deliveryItem.amount )
					{
						hasItem = true;
						var remainingAmountToDeduct = quest.deliveryItem.amount;

						for( var deliveryIndex = 0; deliveryIndex < questItemsToProcess.length && remainingAmountToDeduct > 0; deliveryIndex++ )
						{
							var deliveryItem = questItemsToProcess[deliveryIndex];

							if( !ValidateObject( deliveryItem ))
							{
								continue;
							}

							var amountToDeduct = Math.min( deliveryItem.amount, remainingAmountToDeduct );
							deliveryItem.amount -= amountToDeduct;
							remainingAmountToDeduct -= amountToDeduct;

							if( deliveryItem.amount <= 0 )
							{
								deliveryItem.Delete();
							}
						}
					}
				}

				if( hasItem )
				{
					questEntry.completed = true;
					socket.SysMessage( GetDictionaryEntry( 19620, socket.language ));
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 19621, socket.language ));
				}
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 19622, socket.language ));
			}
		}

		if( quest.type == "skillgain" && quest.targetSkill == identifier )
		{
			questEntry.skillProgress = ( questEntry.skillProgress || 0 ) + progressValue;

			if( questEntry.skillProgress >= quest.maxSkillPoints )
			{
				player.SetTag( "AcceleratedSkillGain", null );
				player.RemoveScriptTrigger( 5811 );
				questEntry.completed = true;
			}
			else
			{
				socket.SysMessage( "Skill progress: " + ( questEntry.skillProgress / 10 ).toFixed( 1 ) + "/" + ( quest.maxSkillPoints / 10 ).toFixed( 1 ) );
			}
		}

		var allObjectivesCompleted = AreAllQuestObjectivesComplete( quest, questEntry );

		if( quest.type == "skillgain" )
		{
			allObjectivesCompleted = questEntry.completed === true;
		}

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
				if( quest.type == "skillgain" && quest.oncomplete )
				{
					socket.SysMessage( quest.oncomplete );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 19623, socket.language ));
				}
				WriteQuestProgress( player, questProgressArray );
				CompleteQuest( player, questID );
			}
			return;
		}
		else
		{
			questEntry.completed = false;
		}

		questUpdated = true;
		break;
	}

	if( questUpdated )
	{
		WriteQuestProgress( player, questProgressArray );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 19624, socket.language ));
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
	if( !ValidateObject( player ) || !ValidateObject( creature ) )
	{
		return false;
	}

	var questProgressArray = ReadQuestProgress( player );

	for( var questEntryIndex = 0; questEntryIndex < questProgressArray.length; questEntryIndex++ )
	{
		var questEntry = questProgressArray[questEntryIndex];

		if( questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );

		if( quest && ( quest.type == "kill" || quest.type == "timekills" || quest.type == "multi" ) && !questEntry.completed )
		{
			if( quest.targetKills )
			{
				for( var targetKillIndex = 0; targetKillIndex < quest.targetKills.length; targetKillIndex++ )
				{
					var targetKill = quest.targetKills[targetKillIndex];

					if( String( targetKill.npcID ) == String( creature.sectionID ))
					{
						var requiredRegion = ResolveKillTargetRegion( quest, targetKill );
						if( !IsKillInRequiredRegion( player, creature, requiredRegion ))
						{
							continue;
						}

						UpdateQuestProgress( player, questEntry.questID, creature.sectionID, 1, "kill" );
					}
				}
			}

			if( quest.targetKillGroups )
			{
				for( var targetKillGroupIndex = 0; targetKillGroupIndex < quest.targetKillGroups.length; targetKillGroupIndex++ )
				{
					var targetKillGroup = quest.targetKillGroups[targetKillGroupIndex];
					if( !targetKillGroup || !targetKillGroup.groupID )
					{
						continue;
					}

					if( !DoesNpcMatchTargetKillGroup( creature, targetKillGroup ))
					{
						continue;
					}

					var requiredGroupRegion = ResolveKillTargetRegion( quest, targetKillGroup );
					if( !IsKillInRequiredRegion( player, creature, requiredGroupRegion ))
					{
						continue;
					}

					UpdateQuestProgress( player, questEntry.questID, String( targetKillGroup.groupID ), 1, "killgroup" );
					break;
				}
			}
		}
	}

	return true;
}

/** @type { ( player: Character, item: Item, isToggledOff?: boolean ) => void } */
function ItemCollected( player, item, isToggledOff )
{
	if( !ValidateObject( player ))
		return;

	var socket = player.socket;
	if( socket == null )
		return;

	if( typeof isToggledOff == "undefined" )
	{
		isToggledOff = false;
	}

	var questProgressArray = ReadQuestProgress( player );

	for( var questEntryIndex = 0; questEntryIndex < questProgressArray.length; questEntryIndex++ )
	{
		var questEntry = questProgressArray[questEntryIndex];

		if( questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		var isValidType = ( quest && ( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" ));

		if( !isValidType || ( !isToggledOff && questEntry.completed ))
		{
			continue;
		}

		if( !questEntry.collectedItems )
		{
			questEntry.collectedItems = {};
		}
		if( !questEntry.collectedItemGroups )
		{
			questEntry.collectedItemGroups = {};
		}

		var itemSectionID = String( item.sectionID );
		var questSectionID = String( item.GetTag( "QuestSectionID" ) || item.sectionID );
		var handledItem = false;

		if( quest.targetItems )
		{
			for( var targetItemIndex = 0; targetItemIndex < quest.targetItems.length; targetItemIndex++ )
			{
				var targetItem = quest.targetItems[targetItemIndex];
				if( String( targetItem.sectionID ) == itemSectionID || String( targetItem.sectionID ) == questSectionID )
				{
					var currentCount = questEntry.collectedItems[itemSectionID] || 0;
					var remaining = targetItem.amount - currentCount;

					if( isToggledOff )
					{
						if( currentCount > 0 )
						{
							var amountToRemove = Math.min( item.amount, currentCount );
							UpdateQuestProgress( player, questEntry.questID, itemSectionID, -amountToRemove, "collect" );

							var savedExactColor = item.GetTag( "saveColor" );
							if( savedExactColor != null && !isNaN( parseInt( savedExactColor, 10 )))
							{
								item.color = parseInt( savedExactColor, 10 );
							}
							else
							{
								item.color = 0;
							}

							item.isNewbie = false;
							item.isDyeable = true;
							item.SetTag( "QuestItem", null );
							item.SetTag( "QuestSectionID", null );
							item.SetTag( "QuestGroupID", null );
							item.SetTag( "saveColor", null );
							item.RemoveScriptTrigger( 5806 );
						}
					}
					else
					{
						if( remaining > 0 )
						{
							var amountToAdd = Math.min( item.amount, remaining );
							UpdateQuestProgress( player, questEntry.questID, itemSectionID, amountToAdd, "collect" );

							item.SetTag( "saveColor", item.color );
							item.color = 0x04ea;
							item.isDyeable = false;
							item.isNewbie = true;
							item.SetTag( "QuestItem", true );
							item.SetTag( "QuestSectionID", questSectionID );
							item.SetTag( "QuestGroupID", null );
							item.AddScriptTrigger( 5806 );
						}
						else
						{
							socket.SysMessage( "Cannot collect more. Target amount reached: " + targetItem.amount );
						}
					}

					handledItem = true;
					break;
				}
			}
		}

		if( handledItem )
		{
			continue;
		}

		if( quest.targetItemGroups )
		{
			for( var targetGroupIndex = 0; targetGroupIndex < quest.targetItemGroups.length; targetGroupIndex++ )
			{
				var targetGroup = quest.targetItemGroups[targetGroupIndex];
				if( !targetGroup || !targetGroup.groupID )
				{
					continue;
				}

				if( !DoesItemMatchTargetGroup( itemSectionID, questSectionID, targetGroup ))
				{
					continue;
				}

				var groupID = String( targetGroup.groupID );
				var currentGroupCount = questEntry.collectedItemGroups[groupID] || 0;
				var remainingGroupAmount = targetGroup.amount - currentGroupCount;

				if( isToggledOff )
				{
					if( currentGroupCount > 0 )
					{
						var amountToRemoveFromGroup = Math.min( item.amount, currentGroupCount );
						UpdateQuestProgress( player, questEntry.questID, groupID, -amountToRemoveFromGroup, "collectgroup" );

						var savedGroupColor = item.GetTag( "saveColor" );
						if( savedGroupColor != null && !isNaN( parseInt( savedGroupColor, 10 )))
						{
							item.color = parseInt( savedGroupColor, 10 );
						}
						else
						{
							item.color = 0;
						}

						item.isNewbie = false;
						item.isDyeable = true;
						item.SetTag( "QuestItem", null );
						item.SetTag( "QuestSectionID", null );
						item.SetTag( "QuestGroupID", null );
						item.SetTag( "saveColor", null );
						item.RemoveScriptTrigger( 5806 );
					}
				}
				else
				{
					if( remainingGroupAmount > 0 )
					{
						var amountToAddToGroup = Math.min( item.amount, remainingGroupAmount );
						UpdateQuestProgress( player, questEntry.questID, groupID, amountToAddToGroup, "collectgroup" );

						item.SetTag( "saveColor", item.color );
						item.color = 0x04ea;
						item.isDyeable = false;
						item.isNewbie = true;
						item.SetTag( "QuestItem", true );
						item.SetTag( "QuestSectionID", questSectionID );
						item.SetTag( "QuestGroupID", groupID );
						item.AddScriptTrigger( 5806 );
					}
					else
					{
						socket.SysMessage( "Cannot collect more. Target amount reached: " + targetGroup.amount );
					}
				}

				break;
			}
		}
	}
}

/** @type { ( pEquipper: Character, iEquipped: Item ) => boolean } */
function EquipAttempt( pEquipper, iEquipped )
{
	if( !ValidateObject( pEquipper ))
		return false;

	var socket = pEquipper.socket;
	if( socket == null )
		return false;

	if( !iEquipped.GetTag( "QuestItem" ))
	{
		return true;
	}

	socket.SysMessage( GetDictionaryEntry( 19630, socket.language )); // This item cannot be equipped as it is a quest item.

	var questProgressArray = ReadQuestProgress( pEquipper );
	var itemSectionID = String( iEquipped.sectionID );
	var questSectionID = String( iEquipped.GetTag( "QuestSectionID" ) || iEquipped.sectionID );

	for( var questEntryIndex = 0; questEntryIndex < questProgressArray.length; questEntryIndex++ )
	{
		var questEntry = questProgressArray[questEntryIndex];

		if( questEntry.serial != pEquipper.serial )
			continue;

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		var isValidType = ( quest && ( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" ) );

		if( !isValidType )
			continue;

		if( !questEntry.collectedItems )
		{
			questEntry.collectedItems = {};
		}
		if( !questEntry.collectedItemGroups )
		{
			questEntry.collectedItemGroups = {};
		}

		// Exact item targets
		if( quest.targetItems && quest.targetItems.length )
		{
			for( var targetItemIndex = 0; targetItemIndex < quest.targetItems.length; targetItemIndex++ )
			{
				var targetItem = quest.targetItems[targetItemIndex];

				if( String( targetItem.sectionID ) == itemSectionID || String( targetItem.sectionID ) == questSectionID )
				{
					var currentCount = questEntry.collectedItems[itemSectionID] || 0;

					if( currentCount > 0 )
					{
						var amountToRemove = Math.min( iEquipped.amount, currentCount );
						UpdateQuestProgress( pEquipper, questEntry.questID, itemSectionID, -amountToRemove, "collect" );

						var savedExactColor = iEquipped.GetTag( "saveColor" );
						if( savedExactColor != null && !isNaN( parseInt( savedExactColor, 10 )))
						{
							iEquipped.color = parseInt( savedExactColor, 10 );
						}
						else
						{
							iEquipped.color = 0;
						}

						iEquipped.isNewbie = false;
						iEquipped.isDyeable = true;
						iEquipped.SetTag( "QuestItem", null );
						iEquipped.SetTag( "QuestSectionID", null );
						iEquipped.SetTag( "QuestGroupID", null );
						iEquipped.SetTag( "saveColor", null );
						iEquipped.RemoveScriptTrigger( 5806 );
						iEquipped.Refresh();

						socket.SysMessage( "You removed Quest Item status from the item." );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 19629, socket.language )); // Cannot decrease further. Current count is 0.
					}

					return false;
				}
			}
		}

		// Grouped item targets
		if( quest.targetItemGroups && quest.targetItemGroups.length )
		{
			for( var targetGroupIndex = 0; targetGroupIndex < quest.targetItemGroups.length; targetGroupIndex++ )
			{
				var targetGroup = quest.targetItemGroups[targetGroupIndex];
				if( !targetGroup || !targetGroup.groupID )
				{
					continue;
				}

				if( !DoesItemMatchTargetGroup( itemSectionID, questSectionID, targetGroup ))
				{
					continue;
				}

				var groupID = String( targetGroup.groupID );
				var currentGroupCount = questEntry.collectedItemGroups[groupID] || 0;

				if( currentGroupCount > 0 )
				{
					var amountToRemoveFromGroup = Math.min( iEquipped.amount, currentGroupCount );
					UpdateQuestProgress( pEquipper, questEntry.questID, groupID, -amountToRemoveFromGroup, "collectgroup" );

					var savedGroupColor = iEquipped.GetTag( "saveColor" );
					if( savedGroupColor != null && !isNaN( parseInt( savedGroupColor, 10 )))
					{
						iEquipped.color = parseInt( savedGroupColor, 10 );
					}
					else
					{
						iEquipped.color = 0;
					}

					iEquipped.isNewbie = false;
					iEquipped.isDyeable = true;
					iEquipped.SetTag( "QuestItem", null );
					iEquipped.SetTag( "QuestSectionID", null );
					iEquipped.SetTag( "QuestGroupID", null );
					iEquipped.SetTag( "saveColor", null );
					iEquipped.RemoveScriptTrigger( 5806 );
					iEquipped.Refresh();

					socket.SysMessage( "You removed Quest Item status from the item." );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 19629, socket.language ));
				}

				return false;
			}
		}
	}

	return false;
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
			"QuestTurnIn=0\n" +
			"Failed=1\n\n";

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

	var quest = TriggerEvent( 5801, "QuestList", completedQuest.questID );
	var nextQuestID = quest ? quest.nextQuestID || "null" : "null";

	mFile.Open( archiveFileName, "a", "Quests" );
	if( mFile )
	{
		var collectedItemsStr = "";
		if( completedQuest.collectedItems )
		{
			for( var collectedItemKey in completedQuest.collectedItems )
			{
				if( completedQuest.collectedItems.hasOwnProperty( collectedItemKey ) )
				{
					if( collectedItemsStr.length > 0 )
					{
						collectedItemsStr += ",";
					}
					collectedItemsStr += collectedItemKey + ":" + completedQuest.collectedItems[collectedItemKey];
				}
			}
		}

		var collectedItemGroupsStr = "";
		if( completedQuest.collectedItemGroups )
		{
			for( var collectedGroupKey in completedQuest.collectedItemGroups )
			{
				if( completedQuest.collectedItemGroups.hasOwnProperty( collectedGroupKey ) )
				{
					if( collectedItemGroupsStr.length > 0 )
					{
						collectedItemGroupsStr += ",";
					}
					collectedItemGroupsStr += collectedGroupKey + ":" + completedQuest.collectedItemGroups[collectedGroupKey];
				}
			}
		}

		var harvestKillsStr = "";
		if( completedQuest.harvestKills )
		{
			for( var killKey in completedQuest.harvestKills )
			{
				if( completedQuest.harvestKills.hasOwnProperty( killKey ) )
				{
					if( harvestKillsStr.length > 0 )
					{
						harvestKillsStr += ",";
					}
					harvestKillsStr += killKey + ":" + completedQuest.harvestKills[killKey];
				}
			}
		}

		var harvestKillGroupsStr = "";
		if( completedQuest.harvestKillGroups )
		{
			for( var harvestKillGroupKey in completedQuest.harvestKillGroups )
			{
				if( completedQuest.harvestKillGroups.hasOwnProperty( harvestKillGroupKey ) )
				{
					if( harvestKillGroupsStr.length > 0 )
					{
						harvestKillGroupsStr += ",";
					}
					harvestKillGroupsStr += harvestKillGroupKey + ":" + completedQuest.harvestKillGroups[harvestKillGroupKey];
				}
			}
		}

		var skillProgressStr = "";
		if( quest && quest.type == "skillgain" )
		{
			skillProgressStr =
				"SkillProgress=" + ( completedQuest.skillProgress || 0 ) + "\n" +
				"MaxSkillPoints=" + ( quest.maxSkillPoints || 0 ) + "\n" +
				"TargetSkill=" + ( quest.targetSkill || "null" ) + "\n";
		}

		var deliveryProgressStr = "";
		if( quest && quest.type == "delivery" )
		{
			deliveryProgressStr =
				"DeliveryProgress=" + ( completedQuest.deliveryProgress || 0 ) + "\n" +
				"DeliveryItem=" + ( completedQuest.deliveryItem || "null" ) + "\n" +
				"TargetDeliveryNPC=" + ( completedQuest.targetDeliveryNPC || "null" ) + "\n";
		}

		var archiveEntry =
			"Serial=" + ( completedQuest.serial || "undefined" ) + "\n" +
			"QuestID=" + ( completedQuest.questID || "undefined" ) + "\n" +
			"NextQuestID=" + nextQuestID + "\n" +
			"LastCompleted=" + Date.now() + "\n" +
			"QuestProgress=" + ( completedQuest.questProgress || 0 ) + "\n" +
			"CollectedItems=" + collectedItemsStr + "\n" +
			"CollectedItemGroups=" + collectedItemGroupsStr + "\n" +
			"HarvestKills=" + harvestKillsStr + "\n" +
			"HarvestKillGroups=" + harvestKillGroupsStr + "\n" +
			skillProgressStr +
			deliveryProgressStr +
			"StartTime=" + ( completedQuest.startTime || 0 ) + "\n" +
			"TimeLimit=" + ( completedQuest.timeLimit || 0 ) + "\n" +
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
		for( var progressIndex = 0; progressIndex < questProgressArray.length; progressIndex++ )
		{
			var progressEntry = questProgressArray[progressIndex];

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

			var killGroupsStr = "";
			if( progressEntry.harvestKillGroups )
			{
				for( var killGroupKey in progressEntry.harvestKillGroups )
				{
					if( progressEntry.harvestKillGroups.hasOwnProperty( killGroupKey ) )
					{
						if( killGroupsStr.length > 0 )
						{
							killGroupsStr += ",";
						}
						killGroupsStr += killGroupKey + ":" + progressEntry.harvestKillGroups[killGroupKey];
					}
				}
			}

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

			var collectedItemGroupsStr = "";
			if( progressEntry.collectedItemGroups )
			{
				for( var groupKey in progressEntry.collectedItemGroups )
				{
					if( progressEntry.collectedItemGroups.hasOwnProperty( groupKey ))
					{
						if( collectedItemGroupsStr.length > 0 )
						{
							collectedItemGroupsStr += ",";
						}
						collectedItemGroupsStr += groupKey + ":" + progressEntry.collectedItemGroups[groupKey];
					}
				}
			}

			var formattedEntry =
				"Serial=" + ( progressEntry.serial || "undefined" ) + "\n" +
				"QuestID=" + ( progressEntry.questID || "undefined" ) + "\n" +
				"QuestProgress=" + ( progressEntry.questProgress || 0 ) + "\n" +
				"HarvestKills=" + killsStr + "\n" +
				"HarvestKillGroups=" + killGroupsStr + "\n" +
				"CollectedItems=" + collectedItemsStr + "\n" +
				"CollectedItemGroups=" + collectedItemGroupsStr + "\n" +
				"SkillProgress=" + ( progressEntry.skillProgress || 0 ) + "\n" +
				"TargetSkill=" + ( progressEntry.targetSkill || -1 ) + "\n" +
				"TargetRegion=" + ( progressEntry.targetRegion || 0 ) + "\n" +
				"MaxSkillPoints=" + ( progressEntry.maxSkillPoints || 50.0 ) + "\n" +
				"StartTime=" + ( progressEntry.startTime || 0 ) + "\n" +
				"TimeLimit=" + ( progressEntry.timeLimit || 0 ) + "\n" +
				"LastAccepted=" + ( progressEntry.lastAccepted || 0 ) + "\n" +
				"Completed=" + ( progressEntry.completed ? "1" : "0" ) + "\n" +
				"QuestTurnIn=" + ( progressEntry.questTurnIn ? "1" : "0" ) + "\n" +
				"NextQuestID=" + ( progressEntry.nextQuestID != null ? progressEntry.nextQuestID : "null" ) + "\n\n";

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
	if( isNaN( entry.serial ))
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
	entry.lastAccepted = parseInt( entry.lastaccepted || "0", 10 );

	processCollectedItems( entry, player );
	processCollectedItemGroups( entry, player );
	processKills( entry, player );
	processKillGroups( entry, player );
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
function processCollectedItemGroups( entry, player )
{
	entry.collectedItemGroups = {};
	var collectedItemGroupsStr = entry.collecteditemgroups || "";

	if( collectedItemGroupsStr == "" )
	{
		return;
	}

	var collectedItemGroups = collectedItemGroupsStr.split( "," );
	for( var groupIndex = 0; groupIndex < collectedItemGroups.length; groupIndex++ )
	{
		var pair = collectedItemGroups[groupIndex].split( ":" );
		if( pair.length == 2 )
		{
			var key = manualTrim( pair[0] );
			var value = parseInt( manualTrim( pair[1] ), 10 );
			entry.collectedItemGroups[key] = value;
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

/** @type { ( entry: any, player: Character ) => void } */
function processKillGroups( entry, player )
{
	entry.harvestKillGroups = {};
	var killGroupsStr = entry.harvestkillgroups || "";

	if( killGroupsStr == "" )
	{
		return;
	}

	var harvestKillGroups = killGroupsStr.split( "," );
	for( var groupIndex = 0; groupIndex < harvestKillGroups.length; groupIndex++ )
	{
		var pair = harvestKillGroups[groupIndex].split( ":" );
		if( pair.length == 2 )
		{
			var key = manualTrim( pair[0] );
			var value = parseInt( manualTrim( pair[1] ), 10 );
			entry.harvestKillGroups[key] = value;
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