/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( player: Character, questID: number, questGiver?: Character | null ) => void } */
function StartQuest( player, questID, questGiver )
{
	if( !ValidateObject( player ) )
		return;

	var socket = player.socket;
	if( socket == null )
		return;

	var questProgressArray = ReadQuestProgress( player );

	if( !CheckQuest( player, questID ) )
	{
		return;
	}

	var collectedItems = {};
	var collectedItemGroups = {};
	var harvestKills = {};
	var harvestKillGroups = {};
	var escortNPCSerial = 0;
	var escortUsesQuestGiver = false;
	var guidedWalkNPCSerial = 0;
	var guidedWalkUsesQuestGiver = false;
	var raceNPCSerial = 0;
	var raceUsesQuestGiver = false;

	var quest = TriggerEvent( 5801, "QuestList", questID );
	var selectedWaypoints = [];

	if( quest && quest.guidedWalk && quest.guidedWalk.enabled && ValidateObject( questGiver ) )
	{
		guidedWalkNPCSerial = questGiver.serial;
		guidedWalkUsesQuestGiver = true;
	}

	if( quest && quest.race && quest.race.enabled )
	{
		if( !ValidateObject( questGiver ) )
		{
			socket.SysMessage( "Unable to start race quest." );
			return;
		}

		if( questGiver.GetTag( "QuestRace" ) )
		{
			socket.SysMessage( "This racer is already racing someone else." );
			return;
		}

		raceNPCSerial = questGiver.serial;
		raceUsesQuestGiver = true;
	}

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
		if( ValidateObject( packageItem ) )
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
			socket.SysMessage( GetDictionaryEntry( 19615, socket.language ) );
		}
	}

	var initialSkillLevel = 0;
	if( quest.type == "skillgain" )
	{
		if( !player.GetTag( "AcceleratedSkillGain" ) )
		{
			initialSkillLevel = player.baseskills[quest.targetSkill];
			player.SetTag( "AcceleratedSkillGain", quest.targetSkill );
			player.AddScriptTrigger( 5811 );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 19616, socket.language ) );
		}
	}

	if( quest.type == "escort" && quest.escortTarget )
	{
		selectedWaypoints = BuildEscortSelectedWaypoints( quest );

		if( !selectedWaypoints.length )
		{
			socket.SysMessage( "Unable to determine escort destination." );
			return;
		}

		var escortNPC = ResolveQuestEscortNPC( player, questID, quest, questGiver );
		if( !ValidateObject( escortNPC ) )
		{
			socket.SysMessage( "Unable to start escort quest." );
			return;
		}

		escortNPCSerial = escortNPC.serial;
		escortUsesQuestGiver = ( quest.escortTarget.useQuestGiver ? true : false );
	}

	questProgressArray.push({
		serial: player.serial,
		questID: questID,
		questProgress: 0,
		harvestKills: harvestKills,
		harvestKillGroups: harvestKillGroups,
		escortNPCSerial: escortNPCSerial,
		escortUsesQuestGiver: escortUsesQuestGiver,
		escortStage: 0,
		escortLastRegion: "",
		escortFailed: false,
		lastTravelAmbushCheck: 0,
		selectedWaypoints: selectedWaypoints,
		selectedDestinationRegionID: ( selectedWaypoints.length > 0 ? parseInt( selectedWaypoints[selectedWaypoints.length - 1].regionID, 10 ) : 0 ),
		selectedDestinationRegionName: ( selectedWaypoints.length > 0 ? ( selectedWaypoints[selectedWaypoints.length - 1].regionName || "" ) : "" ),
		collectedItems: collectedItems,
		collectedItemGroups: collectedItemGroups,
		skillProgress: initialSkillLevel,
		targetSkill: quest.targetSkill || -1,
		targetRegion: quest.targetRegion || 0,
		maxSkillPoints: quest.maxSkillPoints || 50.0,
		startTime: quest.timeLimit ? Date.now() : 0,
		timeLimit: quest.timeLimit ? quest.timeLimit * 1000 : 0,
		guidedWalkNPCSerial: guidedWalkNPCSerial,
		guidedWalkUsesQuestGiver: guidedWalkUsesQuestGiver,
		raceNPCSerial: raceNPCSerial,
		raceUsesQuestGiver: raceUsesQuestGiver,
		raceCheckpoint: 0,
		raceCompleted: false,
		raceWinner: "",
		lastAccepted: Date.now(),
		completed: false,
		questTurnIn: false,
		nextQuestID: quest.nextQuestID || null
	});

	WriteQuestProgress( player, questProgressArray );
	StartGuidedWalkQuestNPC( player, questID, quest, questGiver );
	StartRaceQuestNPC( player, questID, quest, questGiver );

	socket.SysMessage( GetDictionaryEntry( 19617, socket.language ) );

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

			if( quest.type == "escort" )
			{
				CleanupEscortQuestNPC( player, questEntry.questID );
			}

			if( quest.race && quest.race.enabled )
			{
				CleanupRaceQuestNPC( player, questEntry.questID );
			}

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

	if( quest.race && quest.race.enabled )
	{
		if( !questEntry.raceCompleted )
		{
			allObjectivesCompleted = false;
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

		if( quest.type == "escort" )
		{
			CleanupEscortQuestNPC( player, questID );
		}

		if( quest.guidedWalk && quest.guidedWalk.enabled )
		{
			CleanupGuidedWalkQuestNPC( player, questID );
		}

		if( quest.race && quest.race.enabled )
		{
			CleanupRaceQuestNPC( player, questID );
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

/** @type { ( escortNPC: Character, player: Character, questID: number, quest: any ) => Character | null } */
function SetupQuestEscortNPC( escortNPC, player, questID, quest )
{
	if( !ValidateObject( escortNPC ) || !ValidateObject( player ) || !quest || !quest.escortTarget )
	{
		return null;
	}

	escortNPC.owner = player;
	escortNPC.Follow( player );
	escortNPC.SetTag( "QuestEscort", 1 );
	escortNPC.SetTag( "QuestID", questID );
	escortNPC.SetTag( "QuestPlayerSerial", player.serial );
	escortNPC.SetTag( "EscortStage", 0 );
	escortNPC.SetTag( "EscortFailIfDead", quest.escortTarget.failIfDead ? 1 : 0 );
	escortNPC.SetTag( "EscortMaxDistance", quest.escortTarget.maxDistance || 24 );
	escortNPC.SetTag( "EscortReturnDistance", quest.escortTarget.returnDistance || 48 );
	escortNPC.SetTag( "EscortWaypointCount", quest.waypoints ? quest.waypoints.length : 0 );
	escortNPC.SetTag( "EscortUsesQuestGiver", quest.escortTarget.useQuestGiver ? 1 : 0 );

	if( quest.escortTarget.name )
	{
		escortNPC.name = String( quest.escortTarget.name );
	}

	if( !escortNPC.HasScriptTrigger( 5814 ))
	{
		escortNPC.AddScriptTrigger( 5814 );
	}

	escortNPC.StartTimer( 10000, 1, 5814 );
	return escortNPC;
}

/** @type { ( player: Character, questID: number, quest: any, questGiver?: Character | null ) => Character | null } */
function ResolveQuestEscortNPC( player, questID, quest, questGiver )
{
	if( !quest || !quest.escortTarget )
	{
		return null;
	}

	if( quest.escortTarget.useQuestGiver )
	{
		if( !ValidateObject( questGiver ))
		{
			return null;
		}

		return SetupQuestEscortNPC( questGiver, player, questID, quest );
	}

	if( !quest.escortTarget.npcID )
	{
		return null;
	}

	var escortNPC = SpawnNPC( String( quest.escortTarget.npcID ), player.x + 1, player.y, player.z, player.worldnumber, player.instanceID, false );
	if( !ValidateObject( escortNPC ) )
	{
		return null;
	}

	return SetupQuestEscortNPC( escortNPC, player, questID, quest );
}

/** @type { ( player: Character, questID: number ) => void } */
function CleanupEscortQuestNPC( player, questID )
{
	if( !ValidateObject( player ))
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			continue;
		}

		var escortNPCSerial = parseInt( questEntry.escortNPCSerial, 10 );
		if( isNaN( escortNPCSerial ) || escortNPCSerial <= 0 )
		{
			return;
		}

		var escortNPC = CalcCharFromSer( escortNPCSerial );
		if( !ValidateObject( escortNPC ))
		{
			return;
		}

		if( questEntry && questEntry.escortStage >= 0 )
		{
			for( var ambushStageIndex = 0; ambushStageIndex <= questEntry.escortStage; ambushStageIndex++ )
			{
				escortNPC.SetTag( "EscortAmbush_" + questID + "_" + ambushStageIndex, null );
			}
		}

		escortNPC.Follow( null );
		escortNPC.SetTag( "QuestEscort", null );
		escortNPC.SetTag( "QuestID", null );
		escortNPC.SetTag( "QuestPlayerSerial", null );
		escortNPC.SetTag( "EscortStage", null );
		escortNPC.SetTag( "EscortFailIfDead", null );
		escortNPC.SetTag( "EscortMaxDistance", null );
		escortNPC.SetTag( "EscortReturnDistance", null );
		escortNPC.SetTag( "EscortWaypointCount", null );
		escortNPC.SetTag( "EscortUsesQuestGiver", null );
		escortNPC.owner = null;

		if( questEntry.escortUsesQuestGiver )
		{
			if( escortNPC.HasScriptTrigger( 5814 ))
			{
				escortNPC.RemoveScriptTrigger( 5814 );
			}
		}
		else
		{
			escortNPC.Delete();
		}
		return;
	}
}

/** @type { ( player: Character, questID: number, regionID: number ) => void } */
function EscortReachedRegion( player, questID, regionID )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	var enteredRegionID = regionID;

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( !quest || quest.type != "escort" )
		{
			return;
		}

		var activeWaypoints = GetEscortActiveWaypoints( questEntry, quest );
		if( !activeWaypoints || !activeWaypoints.length )
		{
			return;
		}

		var escortStage = parseInt( questEntry.escortStage, 10 );
		if( isNaN( escortStage ) || escortStage < 0 )
		{
			escortStage = 0;
		}

		var nextWaypoint = activeWaypoints[escortStage];
		if( !nextWaypoint )
		{
			return;
		}

		var requiredRegionID = parseInt( nextWaypoint.regionID, 10 );
		if( isNaN( requiredRegionID ) || requiredRegionID <= 0 )
		{
			return;
		}

		if( requiredRegionID != enteredRegionID )
		{
			return;
		}

		questEntry.escortStage = escortStage + 1;
		questEntry.escortLastRegion = String( enteredRegionID );
		socket.SysMessage( "Escort progress updated: region " + enteredRegionID );

		var escortNPC = null;
		var escortNPCSerial = parseInt( questEntry.escortNPCSerial, 10 );
		if( !isNaN( escortNPCSerial ) && escortNPCSerial > 0 )
		{
			escortNPC = CalcCharFromSer( escortNPCSerial );
		}

		if( nextWaypoint.ambush && typeof nextWaypoint.ambush == "object" )
		{
			var ambushAlreadyTriggered = false;
			var ambushTagName = "EscortAmbush_" + questID + "_" + escortStage;

			if( ValidateObject( escortNPC ) )
			{
				ambushAlreadyTriggered = ( parseInt( escortNPC.GetTag( ambushTagName ), 10 ) > 0 );
			}

			if( !ambushAlreadyTriggered )
			{
				var allowInGuardedRegion = parseInt( nextWaypoint.ambush.allowInGuardedRegion, 10 );
				if( isNaN( allowInGuardedRegion ) )
				{
					allowInGuardedRegion = 0;
				}

				var currentRegionObj = null;
				if( ValidateObject( escortNPC ) && escortNPC.region )
				{
					currentRegionObj = escortNPC.region;
				}
				else if( player.region )
				{
					currentRegionObj = player.region;
				}

				var regionIsGuarded = IsRegionGuarded( currentRegionObj );

				if( !regionIsGuarded || allowInGuardedRegion )
				{
					if( SpawnEscortAmbush( player, escortNPC, nextWaypoint.ambush ) )
					{
						socket.SysMessage( "Ambush!" );

						if( ValidateObject( escortNPC ) )
						{
							escortNPC.SetTag( ambushTagName, 1 );
						}
					}
				}
			}
		}

		WriteQuestProgress( player, questProgressArray );

		if( questEntry.escortStage >= activeWaypoints.length )
		{
			questEntry.completed = true;
			WriteQuestProgress( player, questProgressArray );

			if( quest.questTurnIn == 1 )
			{
				socket.SysMessage( GetDictionaryEntry( 19623, socket.language ) );
			}
			else
			{
				CompleteQuest( player, questID );
			}
			return;
		}

		return;
	}
}

/** @type { ( player: Character, questID: number, failMessage: string ) => void } */
function FailEscortQuest( player, questID, failMessage )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	var newQuestProgressArray = [];
	var removedQuest = false;

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.serial == player.serial && questEntry.questID == questID )
		{
			questEntry.escortFailed = true;
			LogFailedQuest( player, questEntry );
			removedQuest = true;
			continue;
		}

		newQuestProgressArray.push( questEntry );
	}

	if( removedQuest )
	{
		CleanupEscortQuestNPC( player, questID );
		WriteQuestProgress( player, newQuestProgressArray );
		socket.SysMessage( failMessage || "You have failed the escort quest." );
	}
}

/** @type { ( player: Character, escortNPC: Character | null, ambushData: any ) => boolean } */
function SpawnEscortAmbush( player, escortNPC, ambushData )
{
	if( !ValidateObject( player ) || !ambushData || !ambushData.npcIDs || !ambushData.npcIDs.length )
	{
		return false;
	}

	var spawnX = player.x;
	var spawnY = player.y;
	var spawnZ = player.z;
	var spawnWorld = player.worldnumber;
	var spawnInstance = player.instanceID;

	if( ValidateObject( escortNPC ) )
	{
		spawnX = escortNPC.x;
		spawnY = escortNPC.y;
		spawnZ = escortNPC.z;
		spawnWorld = escortNPC.worldnumber;
		spawnInstance = escortNPC.instanceID;
	}

	var despawnSeconds = parseInt( ambushData.despawnSeconds, 10 );
	if( isNaN( despawnSeconds ) || despawnSeconds <= 0 )
	{
		despawnSeconds = 120;
	}

	var spawnedAny = false;

	for( var ambushNpcIndex = 0; ambushNpcIndex < ambushData.npcIDs.length; ambushNpcIndex++ )
	{
		var ambushNpcID = String( ambushData.npcIDs[ambushNpcIndex] );
		if( ambushNpcID == "" )
		{
			continue;
		}

		var offsetX = RandomNumber( -4, 4 );
		var offsetY = RandomNumber( -4, 4 );

		var ambushNpc = SpawnNPC( ambushNpcID, spawnX + offsetX, spawnY + offsetY, spawnZ, spawnWorld, spawnInstance, false );

		if( ValidateObject( ambushNpc ) )
		{
			spawnedAny = true;

			ambushNpc.SetTag( "EscortAmbushSpawned", 1 );
			ambushNpc.SetTag( "EscortAmbushDespawnSeconds", despawnSeconds );
			ambushNpc.shouldSave = false;

			if( !ambushNpc.HasScriptTrigger( 5815 ) )
			{
				ambushNpc.AddScriptTrigger( 5815 );
			}

			ambushNpc.StartTimer( despawnSeconds * 1000, 1, 5815  );
		}
	}

	return spawnedAny;
}

/** @type { ( escortNPC: Character, player: Character, questID: number, quest: any, questEntry: any ) => boolean } */
function RepairEscortNPCFromProgress( escortNPC, player, questID, quest, questEntry )
{
	if( !ValidateObject( escortNPC ) || !ValidateObject( player ) || !quest || quest.type != "escort" )
	{
		return false;
	}

	var activeWaypoints = GetEscortActiveWaypoints( questEntry, quest );
	var waypointCount = ( activeWaypoints && activeWaypoints.length ) ? activeWaypoints.length : 0;

	escortNPC.owner = player;
	escortNPC.Follow( player );
	escortNPC.SetTag( "QuestEscort", 1 );
	escortNPC.SetTag( "QuestID", questID );
	escortNPC.SetTag( "QuestPlayerSerial", player.serial );
	escortNPC.SetTag( "EscortStage", questEntry.escortStage || 0 );
	escortNPC.SetTag( "EscortFailIfDead", quest.escortTarget && quest.escortTarget.failIfDead ? 1 : 0 );
	escortNPC.SetTag( "EscortMaxDistance", ( quest.escortTarget && quest.escortTarget.maxDistance ) ? quest.escortTarget.maxDistance : 24 );
	escortNPC.SetTag( "EscortReturnDistance", ( quest.escortTarget && quest.escortTarget.returnDistance ) ? quest.escortTarget.returnDistance : 48 );
	escortNPC.SetTag( "EscortWaypointCount", waypointCount );
	escortNPC.SetTag( "EscortUsesQuestGiver", questEntry.escortUsesQuestGiver ? 1 : 0 );

	if( !escortNPC.HasScriptTrigger( 5814 ) )
	{
		escortNPC.AddScriptTrigger( 5814 );
	}

	escortNPC.StartTimer( 10000, 1, 5814 );
	return true;
}

/** @type { ( player: Character ) => void } */
function ValidateTimedQuestsOnLogin( player )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	if( !questProgressArray || !questProgressArray.length )
	{
		return;
	}

	var updatedQuestProgressArray = [];
	var progressChanged = false;
	var currentTime = Date.now();

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial )
		{
			updatedQuestProgressArray.push( questEntry );
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest )
		{
			updatedQuestProgressArray.push( questEntry );
			continue;
		}

		var timeLimitMilliseconds = parseInt( questEntry.timeLimit, 10 );
		var startTimeMilliseconds = parseInt( questEntry.startTime, 10 );

		if( isNaN( timeLimitMilliseconds ) || timeLimitMilliseconds <= 0 || isNaN( startTimeMilliseconds ) || startTimeMilliseconds <= 0 )
		{
			updatedQuestProgressArray.push( questEntry );
			continue;
		}

		var expirationTime = startTimeMilliseconds + timeLimitMilliseconds;
		var remainingMilliseconds = expirationTime - currentTime;

		if( remainingMilliseconds <= 0 )
		{
			TriggerEvent( 5802, "ManageQuestItems", player, questEntry.questID, false );

			if( quest.type == "skillgain" )
			{
				player.SetTag( "AcceleratedSkillGain", null );
				player.RemoveScriptTrigger( 5811 );
			}

			if( quest.type == "escort" )
			{
				CleanupEscortQuestNPC( player, questEntry.questID );
			}

			if( quest.race && quest.race.enabled )
			{
				CleanupRaceQuestNPC( player, questEntry.questID );
			}

			questEntry.escortFailed = ( quest.type == "escort" );
			LogFailedQuest( player, questEntry );

			socket.SysMessage( "You have failed the timed quest: " + quest.title );
			progressChanged = true;
			continue;
		}

		player.KillJSTimer( questEntry.questID, 5800 );
		player.StartTimer( remainingMilliseconds, questEntry.questID, 5800 );
		updatedQuestProgressArray.push( questEntry );
	}

	if( progressChanged )
	{
		WriteQuestProgress( player, updatedQuestProgressArray );
	}
}

/** @type { ( player: Character ) => void } */
function RestoreQuestPlayerTriggersOnLogin( player )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	if( !questProgressArray || !questProgressArray.length )
	{
		return;
	}

	var needsKillTrigger = false;
	var needsSkillGainTrigger = false;
	var restoredSkillID = -1;

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial || questEntry.completed )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest )
		{
			continue;
		}

		var hasKillTargets =
			( quest.targetKills && quest.targetKills.length > 0 ) ||
			( quest.targetKillGroups && quest.targetKillGroups.length > 0 );

		if(( quest.type == "kill" || quest.type == "timekills" || quest.type == "multi" ) && hasKillTargets )
		{
			needsKillTrigger = true;
		}

		if( quest.type == "skillgain" )
		{
			needsSkillGainTrigger = true;
			restoredSkillID = quest.targetSkill;
		}
	}

	if( needsKillTrigger && !player.HasScriptTrigger( 5810 ) )
	{
		player.AddScriptTrigger( 5810 );
	}

	if( needsSkillGainTrigger )
	{
		if( !player.HasScriptTrigger( 5811 ) )
		{
			player.AddScriptTrigger( 5811 );
		}

		if( restoredSkillID >= 0 )
		{
			player.SetTag( "AcceleratedSkillGain", restoredSkillID );
		}
	}
}

/** @type { ( player: Character ) => void } */
function ValidateEscortQuestsOnLogin( player )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	if( !questProgressArray || !questProgressArray.length )
	{
		return;
	}

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest || quest.type != "escort" )
		{
			continue;
		}

		var escortNPCSerial = parseInt( questEntry.escortNPCSerial, 10 );
		if( isNaN( escortNPCSerial ) || escortNPCSerial <= 0 )
		{
			FailEscortQuest( player, questEntry.questID, "Your escort quest could not be restored." );
			return;
		}

		var escortNPC = CalcCharFromSer( escortNPCSerial );
		if( !ValidateObject( escortNPC ) )
		{
			FailEscortQuest( player, questEntry.questID, "Your escort is no longer present. The escort quest has failed." );
			return;
		}

		RepairEscortNPCFromProgress( escortNPC, player, questEntry.questID, quest, questEntry );
	}
}

/** @type { ( regionObj: any ) => boolean } */
function IsRegionGuarded( regionObj )
{
	if( !regionObj )
	{
		return false;
	}

	if( typeof regionObj.guarded != "undefined" )
	{
		var guardedValue = parseInt( regionObj.guarded, 10 );
		if( !isNaN( guardedValue ) && guardedValue > 0 )
		{
			return true;
		}
	}

	if( typeof regionObj.isGuarded != "undefined" )
	{
		return !!regionObj.isGuarded;
	}

	return false;
}

/** @type { ( player: Character, questID: number, escortNPC: Character ) => void } */
function CheckEscortTravelAmbush( player, questID, escortNPC )
{
	if( !ValidateObject( player ) || !ValidateObject( escortNPC ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || quest.type != "escort" || !quest.travelAmbush || !quest.travelAmbush.enabled )
	{
		return;
	}

	var travelAmbush = quest.travelAmbush;

	var allowInGuardedRegion = parseInt( travelAmbush.allowInGuardedRegion, 10 );
	if( isNaN( allowInGuardedRegion ) )
	{
		allowInGuardedRegion = 0;
	}

	var currentRegionObj = escortNPC.region || player.region;
	if( IsRegionGuarded( currentRegionObj ) && !allowInGuardedRegion )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			continue;
		}

		var checkInterval = parseInt( travelAmbush.checkInterval, 10 );
		if( isNaN( checkInterval ) || checkInterval <= 0 )
		{
			checkInterval = 30;
		}

		var now = Date.now();
		var lastCheck = parseInt( questEntry.lastTravelAmbushCheck, 10 );
		if( isNaN( lastCheck ) )
		{
			lastCheck = 0;
		}

		if( lastCheck > 0 && ( now - lastCheck ) < ( checkInterval * 1000 ) )
		{
			return;
		}

		questEntry.lastTravelAmbushCheck = now;

		var escortStage = parseInt( questEntry.escortStage, 10 );
		if( isNaN( escortStage ) )
		{
			escortStage = 0;
		}

		var minStage = parseInt( travelAmbush.minStage, 10 );
		if( isNaN( minStage ) )
		{
			minStage = 0;
		}

		var maxStage = parseInt( travelAmbush.maxStage, 10 );
		if( isNaN( maxStage ) )
		{
			maxStage = -1;
		}

		if( escortStage < minStage )
		{
			WriteQuestProgress( player, questProgressArray );
			return;
		}

		if( maxStage >= 0 && escortStage > maxStage )
		{
			WriteQuestProgress( player, questProgressArray );
			return;
		}

		var chance = parseInt( travelAmbush.chance, 10 );
		if( isNaN( chance ) || chance <= 0 )
		{
			chance = 25;
		}

		if( RandomNumber( 1, 100 ) <= chance )
		{
			if( SpawnEscortAmbush( player, escortNPC, travelAmbush ) )
			{
				socket.SysMessage( "You are ambushed on the road!" );
			}
		}

		WriteQuestProgress( player, questProgressArray );
		return;
	}
}

/** @type { ( waypoints: any[] ) => any[] } */
function CloneEscortWaypoints( waypoints )
{
	var clonedWaypoints = [];

	if( !waypoints || !waypoints.length )
	{
		return clonedWaypoints;
	}

	for( var waypointIndex = 0; waypointIndex < waypoints.length; waypointIndex++ )
	{
		var waypoint = waypoints[waypointIndex];
		if( !waypoint || typeof waypoint != "object" )
		{
			continue;
		}

		var clonedWaypoint = {};
		for( var key in waypoint )
		{
			if( waypoint.hasOwnProperty( key ) )
			{
				clonedWaypoint[key] = waypoint[key];
			}
		}

		if( typeof clonedWaypoint.order == "undefined" )
		{
			clonedWaypoint.order = waypointIndex + 1;
		}

		clonedWaypoints.push( clonedWaypoint );
	}

	return clonedWaypoints;
}

/** @type { ( destinationPool: any[] ) => any | null } */
function PickRandomEscortDestination( destinationPool )
{
	if( !destinationPool || !destinationPool.length )
	{
		return null;
	}

	var totalWeight = 0;
	for( var destinationIndex = 0; destinationIndex < destinationPool.length; destinationIndex++ )
	{
		var destinationEntry = destinationPool[destinationIndex];
		if( !destinationEntry || typeof destinationEntry != "object" )
		{
			continue;
		}

		var entryWeight = parseInt( destinationEntry.weight, 10 );
		if( isNaN( entryWeight ) || entryWeight <= 0 )
		{
			entryWeight = 1;
		}

		totalWeight += entryWeight;
	}

	if( totalWeight <= 0 )
	{
		return null;
	}

	var roll = RandomNumber( 1, totalWeight );
	var runningTotal = 0;

	for( var pickIndex = 0; pickIndex < destinationPool.length; pickIndex++ )
	{
		var poolEntry = destinationPool[pickIndex];
		if( !poolEntry || typeof poolEntry != "object" )
		{
			continue;
		}

		var poolWeight = parseInt( poolEntry.weight, 10 );
		if( isNaN( poolWeight ) || poolWeight <= 0 )
		{
			poolWeight = 1;
		}

		runningTotal += poolWeight;
		if( roll <= runningTotal )
		{
			return poolEntry;
		}
	}

	return destinationPool[0] || null;
}

/** @type { ( quest: any ) => any[] } */
function BuildEscortSelectedWaypoints( quest )
{
	if( !quest || quest.type != "escort" )
	{
		return [];
	}

	if( quest.waypoints && quest.waypoints.length > 0 )
	{
		return CloneEscortWaypoints( quest.waypoints );
	}

	if( quest.randomDestinationPool && quest.randomDestinationPool.length > 0 )
	{
		var chosenDestination = PickRandomEscortDestination( quest.randomDestinationPool );
		if( chosenDestination )
		{
			return [{
				regionID: parseInt( chosenDestination.regionID, 10 ),
				regionName: chosenDestination.regionName || "",
				order: 1
			}];
		}
	}

	return [];
}

/** @type { ( questEntry: any, quest: any ) => any[] } */
function GetEscortActiveWaypoints( questEntry, quest )
{
	if( questEntry && questEntry.selectedWaypoints && questEntry.selectedWaypoints.length > 0 )
	{
		return questEntry.selectedWaypoints;
	}

	if( quest && quest.waypoints && quest.waypoints.length > 0 )
	{
		return quest.waypoints;
	}

	return [];
}

/** @type { ( selectedWaypoints: any[] ) => string } */
function SerializeSelectedWaypoints( selectedWaypoints )
{
	if( !selectedWaypoints || !selectedWaypoints.length )
	{
		return "";
	}

	var serializedParts = [];

	for( var waypointIndex = 0; waypointIndex < selectedWaypoints.length; waypointIndex++ )
	{
		var waypoint = selectedWaypoints[waypointIndex];
		if( !waypoint )
		{
			continue;
		}

		var regionID = parseInt( waypoint.regionID, 10 );
		if( isNaN( regionID ) || regionID <= 0 )
		{
			continue;
		}

		var regionName = "";
		if( waypoint.regionName )
		{
			regionName = String( waypoint.regionName ).replace( /\|/g, "" ).replace( /:/g, "" );
		}

		serializedParts.push( regionID + ":" + regionName );
	}

	return serializedParts.join( "|" );
}

/** @type { ( rawText: string ) => any[] } */
function ParseSelectedWaypoints( rawText )
{
	var parsedWaypoints = [];

	if( !rawText || rawText == "" )
	{
		return parsedWaypoints;
	}

	var waypointParts = String( rawText ).split( "|" );
	for( var waypointIndex = 0; waypointIndex < waypointParts.length; waypointIndex++ )
	{
		var waypointText = manualTrim( waypointParts[waypointIndex] );
		if( waypointText == "" )
		{
			continue;
		}

		var pieces = waypointText.split( ":" );
		if( pieces.length < 1 )
		{
			continue;
		}

		var regionID = parseInt( manualTrim( pieces[0] ), 10 );
		if( isNaN( regionID ) || regionID <= 0 )
		{
			continue;
		}

		var regionName = "";
		if( pieces.length > 1 )
		{
			regionName = manualTrim( pieces.slice( 1 ).join( ":" ) );
		}

		parsedWaypoints.push({
			regionID: regionID,
			regionName: regionName,
			order: parsedWaypoints.length + 1
		});
	}

	return parsedWaypoints;
}

/** @type { ( player: Character, questID: number, quest: any, questGiver?: Character | null ) => boolean } */
function StartRaceQuestNPC( player, questID, quest, questGiver )
{
	if( !ValidateObject( player ) || !quest || !quest.race || !quest.race.enabled )
	{
		return false;
	}

	if( !ValidateObject( questGiver ) )
	{
		return false;
	}

	if( !quest.race.checkpoints || !quest.race.checkpoints.length )
	{
		return false;
	}

	if( questGiver.GetTag( "QuestRace" ) )
	{
		return false;
	}

	questGiver.owner = player;
	questGiver.Follow( null );

	questGiver.SetTag( "QuestRace", 1 );
	questGiver.SetTag( "QuestRaceQuestID", questID );
	questGiver.SetTag( "QuestRacePlayerSerial", player.serial );
	questGiver.SetTag( "QuestRaceCheckpoint", 0 );
	questGiver.SetTag( "QuestRaceReturning", 0 );

	questGiver.SetTag( "QuestRaceHomeX", questGiver.x );
	questGiver.SetTag( "QuestRaceHomeY", questGiver.y );
	questGiver.SetTag( "QuestRaceHomeZ", questGiver.z );
	questGiver.SetTag( "QuestRaceHomeWorld", questGiver.worldnumber );
	questGiver.SetTag( "QuestRaceHomeInstance", questGiver.instanceID );
	questGiver.SetTag( "QuestRaceOriginalCanRun", questGiver.canRun ? 1 : 0 );
	questGiver.SetTag( "QuestRaceReturnText", quest.race.returnText || "" );
	questGiver.SetTag( "QuestRaceReturnMovement", quest.race.returnMovement || "walk" );
	questGiver.isAwake =true;

	if( quest.race.npcCanRun != 0 )
	{
		questGiver.canRun = true;
	}

	if( quest.race.startText )
	{
		questGiver.TurnToward( player );
		questGiver.TextMessage( String( quest.race.startText ) );
	}

	if( !questGiver.HasScriptTrigger( 5817 ) )
	{
		questGiver.AddScriptTrigger( 5817 );
	}

	questGiver.StartTimer( 1000, 1, 5817 );
	return true;
}

/** @type { ( player: Character, questID: number ) => void } */
function CleanupRaceQuestNPC( player, questID )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			continue;
		}

		var raceNPCSerial = parseInt( questEntry.raceNPCSerial, 10 );
		if( isNaN( raceNPCSerial ) || raceNPCSerial <= 0 )
		{
			return;
		}

		var raceNpc = CalcCharFromSer( raceNPCSerial );
		if( !ValidateObject( raceNpc ) )
		{
			return;
		}

		raceNpc.Follow( null );
		raceNpc.owner = null;

		var homeX = parseInt( raceNpc.GetTag( "QuestRaceHomeX" ), 10 );
		var homeY = parseInt( raceNpc.GetTag( "QuestRaceHomeY" ), 10 );
		var homeZ = parseInt( raceNpc.GetTag( "QuestRaceHomeZ" ), 10 );
		var homeWorld = parseInt( raceNpc.GetTag( "QuestRaceHomeWorld" ), 10 );
		var homeInstance = parseInt( raceNpc.GetTag( "QuestRaceHomeInstance" ), 10 );
		var originalCanRun = parseInt( raceNpc.GetTag( "QuestRaceOriginalCanRun" ), 10 );

		if( !isNaN( originalCanRun ) )
		{
			raceNpc.canRun = ( originalCanRun == 1 );
		}

		raceNpc.SetTag( "QuestRace", null );
		raceNpc.SetTag( "QuestRaceQuestID", null );
		raceNpc.SetTag( "QuestRacePlayerSerial", null );
		raceNpc.SetTag( "QuestRaceCheckpoint", null );
		raceNpc.SetTag( "QuestRaceReturning", null );
		raceNpc.SetTag( "QuestRaceHomeX", null );
		raceNpc.SetTag( "QuestRaceHomeY", null );
		raceNpc.SetTag( "QuestRaceHomeZ", null );
		raceNpc.SetTag( "QuestRaceHomeWorld", null );
		raceNpc.SetTag( "QuestRaceHomeInstance", null );
		raceNpc.SetTag( "QuestRaceOriginalCanRun", null );
		raceNpc.SetTag( "QuestRaceReturnText", null );
		raceNpc.SetTag( "QuestRaceReturnMovement", null );

		if( raceNpc.HasScriptTrigger( 5817 ) )
		{
			raceNpc.RemoveScriptTrigger( 5817 );
		}

		if( !isNaN( homeX ) && !isNaN( homeY ) )
		{
			if( isNaN( homeZ ) )
			{
				homeZ = raceNpc.z;
			}
			if( isNaN( homeWorld ) || homeWorld < 0 )
			{
				homeWorld = raceNpc.worldnumber;
			}
			if( isNaN( homeInstance ) || homeInstance < 0 )
			{
				homeInstance = raceNpc.instanceID;
			}

			raceNpc.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
		}

		return;
	}
}

/** @type { ( player: Character, regionID: number ) => void } */
function RacePlayerReachedFinish( player, questID )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	questID = parseInt( questID, 10 );
	if( isNaN( questID ) || questID <= 0 )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial || questEntry.completed || questEntry.questID != questID )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( !quest || !quest.race || !quest.race.enabled )
		{
			continue;
		}

		if( questEntry.raceCompleted )
		{
			continue;
		}

		questEntry.raceCompleted = true;
		questEntry.raceWinner = "player";

		if( quest.race.playerWinText )
		{
			socket.SysMessage( String( quest.race.playerWinText ) );
		}
		else
		{
			socket.SysMessage( "You won the race." );
		}

		var raceNPCSerial = parseInt( questEntry.raceNPCSerial, 10 );
		if( !isNaN( raceNPCSerial ) && raceNPCSerial > 0 )
		{
			var raceNpc = CalcCharFromSer( raceNPCSerial );
			if( ValidateObject( raceNpc ) )
			{
				raceNpc.SetTag( "QuestRaceReturning", 1 );
			}
		}

		if( AreAllQuestObjectivesComplete( quest, questEntry ) )
		{
			questEntry.completed = true;

			if( quest.questTurnIn == 1 )
			{
				socket.SysMessage( GetDictionaryEntry( 19623, socket.language ) );
				WriteQuestProgress( player, questProgressArray );
			}
			else
			{
				WriteQuestProgress( player, questProgressArray );
				CompleteQuest( player, questID );
			}
		}
		else
		{
			WriteQuestProgress( player, questProgressArray );
		}

		return;
	}
}

/** @type { ( player: Character, regionID: number ) => void } */
function RacePlayerReachedRegion( player, regionID )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial || questEntry.completed )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest || !quest.race || !quest.race.enabled )
		{
			continue;
		}

		var finishRegion = parseInt( quest.race.finishRegion, 10 );
		if( !isNaN( finishRegion ) && finishRegion > 0 && finishRegion == regionID )
		{
			RacePlayerReachedFinish( player, questEntry.questID );
			return;
		}
	}
}

/** @type { ( player: Character, questID: number, raceNpc: Character ) => void } */
function RaceNPCReachedFinish( player, questID, raceNpc )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var socket = player.socket;
	if( socket == null )
	{
		return;
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || !quest.race || !quest.race.enabled )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	var newQuestProgressArray = [];

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];

		if( !questEntry || questEntry.serial != player.serial || questEntry.questID != questID )
		{
			newQuestProgressArray.push( questEntry );
			continue;
		}

		if( questEntry.raceCompleted )
		{
			newQuestProgressArray.push( questEntry );
			continue;
		}

		var npcWinMode = String( quest.race.npcWinMode || "continue" ).toLowerCase();

		if( npcWinMode == "fail" )
		{
			questEntry.raceWinner = "npc";
			LogFailedQuest( player, questEntry );
			socket.SysMessage( quest.race.npcWinText || "You lost the race." );
			CleanupRaceQuestNPC( player, questID );
			continue;
		}

		questEntry.raceCompleted = true;
		questEntry.raceWinner = "npc";

		if( quest.race.npcWinText )
		{
			socket.SysMessage( String( quest.race.npcWinText ) );
		}
		else
		{
			socket.SysMessage( "The racer beat you, but the quest continues." );
		}

		var allObjectivesCompleted = AreAllQuestObjectivesComplete( quest, questEntry );
		if( allObjectivesCompleted )
		{
			if( quest.questTurnIn == 1 || npcWinMode == "turnin" )
			{
				questEntry.completed = true;
				socket.SysMessage( GetDictionaryEntry( 19623, socket.language ) );
				newQuestProgressArray.push( questEntry );
			}
			else
			{
				questEntry.completed = true;
				newQuestProgressArray.push( questEntry );
				WriteQuestProgress( player, newQuestProgressArray );
				CompleteQuest( player, questID );
				return;
			}
		}
		else
		{
			newQuestProgressArray.push( questEntry );
		}
	}

	WriteQuestProgress( player, newQuestProgressArray );
}

/** @type { ( player: Character, questID: number ) => void } */
function RacePlayerReachedFinishCheckpoint( player, questID )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest || !quest.race || !quest.race.enabled || !quest.race.checkpoints || !quest.race.checkpoints.length )
	{
		return;
	}

	var finalCheckpoint = quest.race.checkpoints[quest.race.checkpoints.length - 1];
	if( !finalCheckpoint )
	{
		return;
	}

	var targetX = parseInt( finalCheckpoint.x, 10 );
	var targetY = parseInt( finalCheckpoint.y, 10 );
	var targetWorld = parseInt( finalCheckpoint.world, 10 );
	var finishRange = parseInt( finalCheckpoint.playerRange, 10 );

	if( isNaN( targetX ) || isNaN( targetY ) )
	{
		return;
	}

	if( isNaN( finishRange ) || finishRange <= 0 )
	{
		finishRange = parseInt( finalCheckpoint.range, 10 );
		if( isNaN( finishRange ) || finishRange <= 0 )
		{
			finishRange = 3;
		}
	}

	if( !isNaN( targetWorld ) && targetWorld >= 0 && player.worldnumber != targetWorld )
	{
		return;
	}

	if( DistanceBetween( player.x, player.y, targetX, targetY ) <= finishRange )
	{
		RacePlayerReachedFinish( player, questID );
	}
}

/** @type { ( player: Character, questID: number, quest: any, questGiver?: Character | null ) => boolean } */
function StartGuidedWalkQuestNPC( player, questID, quest, questGiver )
{
	if( !ValidateObject( player ) || !quest || !quest.guidedWalk || !quest.guidedWalk.enabled )
	{
		return false;
	}

	if( !ValidateObject( questGiver ) )
	{
		return false;
	}

	if( !quest.guidedWalk.steps || !quest.guidedWalk.steps.length )
	{
		return false;
	}

	if( questGiver.GetTag( "QuestGuidedWalk" ) )
	{
		return false;
	}

	questGiver.owner = player;
	questGiver.Follow( null );

	questGiver.SetTag( "QuestGuidedWalk", 1 );
	questGiver.SetTag( "QuestGuidedWalkQuestID", questID );
	questGiver.SetTag( "QuestGuidedWalkPlayerSerial", player.serial );
	questGiver.SetTag( "QuestGuidedWalkStep", 0 );
	questGiver.SetTag( "QuestGuidedWalkComplete", 0 );
	questGiver.SetTag( "QuestGuidedWalkReturning", 0 );
	questGiver.SetTag( "QuestGuidedWalkMaxDistance", quest.guidedWalk.maxDistance || 24 );
	questGiver.SetTag( "QuestGuidedWalkReturnText", quest.guidedWalk.returnText || "" );

	questGiver.SetTag( "QuestGuidedWalkHomeX", questGiver.x );
	questGiver.SetTag( "QuestGuidedWalkHomeY", questGiver.y );
	questGiver.SetTag( "QuestGuidedWalkHomeZ", questGiver.z );
	questGiver.SetTag( "QuestGuidedWalkHomeWorld", questGiver.worldnumber );
	questGiver.SetTag( "QuestGuidedWalkHomeInstance", questGiver.instanceID );
	questGiver.SetTag( "QuestGuidedWalkReturnMovement", quest.guidedWalk.returnMovement || "walk" );

	if( quest.guidedWalk.startText )
	{
		questGiver.TurnToward( player );
		questGiver.TextMessage( String( quest.guidedWalk.startText ) );
	}

	if( !questGiver.HasScriptTrigger( 5816 ) )
	{
		questGiver.AddScriptTrigger( 5816 );
	}

	questGiver.StartTimer( 1000, 1, 5816 );
	return true;
}

/** @type { ( player: Character, questID: number ) => void } */
function CleanupGuidedWalkQuestNPC( player, questID )
{
	if( !ValidateObject( player ) )
	{
		return;
	}

	var questProgressArray = ReadQuestProgress( player );
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( questEntry.serial != player.serial || questEntry.questID != questID )
		{
			continue;
		}

		var guidedWalkNPCSerial = parseInt( questEntry.guidedWalkNPCSerial, 10 );
		if( isNaN( guidedWalkNPCSerial ) || guidedWalkNPCSerial <= 0 )
		{
			return;
		}

		var guideNpc = CalcCharFromSer( guidedWalkNPCSerial );
		if( !ValidateObject( guideNpc ) )
		{
			return;
		}

		guideNpc.Follow( null );
		guideNpc.owner = null;

		var homeX = parseInt( guideNpc.GetTag( "QuestGuidedWalkHomeX" ), 10 );
		var homeY = parseInt( guideNpc.GetTag( "QuestGuidedWalkHomeY" ), 10 );
		var homeZ = parseInt( guideNpc.GetTag( "QuestGuidedWalkHomeZ" ), 10 );
		var homeWorld = parseInt( guideNpc.GetTag( "QuestGuidedWalkHomeWorld" ), 10 );
		var homeInstance = parseInt( guideNpc.GetTag( "QuestGuidedWalkHomeInstance" ), 10 );

		guideNpc.SetTag( "QuestGuidedWalk", null );
		guideNpc.SetTag( "QuestGuidedWalkQuestID", null );
		guideNpc.SetTag( "QuestGuidedWalkPlayerSerial", null );
		guideNpc.SetTag( "QuestGuidedWalkStep", null );
		guideNpc.SetTag( "QuestGuidedWalkComplete", null );
		guideNpc.SetTag( "QuestGuidedWalkReturning", null );
		guideNpc.SetTag( "QuestGuidedWalkMaxDistance", null );
		guideNpc.SetTag( "QuestGuidedWalkReturnText", null );
		guideNpc.SetTag( "QuestGuidedWalkTooFarWarned", null );
		guideNpc.SetTag( "QuestGuidedWalkHomeX", null );
		guideNpc.SetTag( "QuestGuidedWalkHomeY", null );
		guideNpc.SetTag( "QuestGuidedWalkHomeZ", null );
		guideNpc.SetTag( "QuestGuidedWalkHomeWorld", null );
		guideNpc.SetTag( "QuestGuidedWalkHomeInstance", null );
		guideNpc.SetTag( "QuestGuidedWalkReturnMovement", null );

		var guideStepIndex = 0;
		for( guideStepIndex = 0; guideStepIndex < 50; guideStepIndex++ )
		{
			guideNpc.SetTag( "QuestGuidedWalkLegStarted_" + guideStepIndex, null );
		}

		if( guideNpc.HasScriptTrigger( 5816 ) )
		{
			guideNpc.RemoveScriptTrigger( 5816 );
		}

		if( !isNaN( homeX ) && !isNaN( homeY ) )
		{
			if( isNaN( homeZ ) )
			{
				homeZ = guideNpc.z;
			}
			if( isNaN( homeWorld ) || homeWorld < 0 )
			{
				homeWorld = guideNpc.worldnumber;
			}
			if( isNaN( homeInstance ) || homeInstance < 0 )
			{
				homeInstance = guideNpc.instanceID;
			}

			guideNpc.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
		}

		return;
	}
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
				var acceleratedGain = RandomNumber( quest.minPoint, quest.maxPoint );

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

		var collectedItemGroupsStr = "";
		if( failedQuest.collectedItemGroups )
		{
			for( var key in failedQuest.collectedItemGroups )
			{
				if( failedQuest.collectedItemGroups.hasOwnProperty( key ) )
				{
					if( collectedItemGroupsStr.length > 0 )
					{
						collectedItemGroupsStr += ",";
					}
					collectedItemGroupsStr += key + ":" + failedQuest.collectedItemGroups[key];
				}
			}
		}

		var harvestKillGroupsStr = "";
		if( failedQuest.harvestKillGroups )
		{
			for( var key in failedQuest.harvestKillGroups )
			{
				if( failedQuest.harvestKillGroups.hasOwnProperty( key ) )
				{
					if( harvestKillGroupsStr.length > 0 )
					{
						harvestKillGroupsStr += ",";
					}
					harvestKillGroupsStr += key + ":" + failedQuest.harvestKillGroups[key];
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
			"HarvestKillGroups=" + harvestKillGroupsStr + "\n" +
			"CollectedItemGroups=" + collectedItemGroupsStr + "\n" +
			"EscortNPCSerial=" + ( failedQuest.escortNPCSerial || 0 ) + "\n" +
			"EscortUsesQuestGiver=" + ( failedQuest.escortUsesQuestGiver ? "1" : "0" ) + "\n" +
			"EscortStage=" + ( failedQuest.escortStage || 0 ) + "\n" +
			"EscortLastRegion=" + ( failedQuest.escortLastRegion || "" ) + "\n" +
			"EscortFailed=1\n" +
			"LastTravelAmbushCheck=" + ( failedQuest.lastTravelAmbushCheck || 0 ) + "\n" +
			"SelectedWaypoints=" + SerializeSelectedWaypoints( failedQuest.selectedWaypoints ) + "\n" +
			"SelectedDestinationRegionID=" + ( failedQuest.selectedDestinationRegionID || 0 ) + "\n" +
			"SelectedDestinationRegionName=" + ( failedQuest.selectedDestinationRegionName || "" ) + "\n" +
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
			"EscortNPCSerial=" + ( completedQuest.escortNPCSerial || 0 ) + "\n" +
			"EscortUsesQuestGiver=" + ( completedQuest.escortUsesQuestGiver ? "1" : "0" ) + "\n" +
			"EscortStage=" + ( completedQuest.escortStage || 0 ) + "\n" +
			"EscortLastRegion=" + ( completedQuest.escortLastRegion || "" ) + "\n" +
			"LastTravelAmbushCheck=" + ( completedQuest.lastTravelAmbushCheck || 0 ) + "\n" +
			"SelectedWaypoints=" + SerializeSelectedWaypoints( completedQuest.selectedWaypoints ) + "\n" +
			"SelectedDestinationRegionID=" + ( completedQuest.selectedDestinationRegionID || 0 ) + "\n" +
			"SelectedDestinationRegionName=" + ( completedQuest.selectedDestinationRegionName || "" ) + "\n" +
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
				"EscortNPCSerial=" + ( progressEntry.escortNPCSerial || 0 ) + "\n" +
				"EscortUsesQuestGiver=" + ( progressEntry.escortUsesQuestGiver ? "1" : "0" ) + "\n" +
				"EscortStage=" + ( progressEntry.escortStage || 0 ) + "\n" +
				"EscortLastRegion=" + ( progressEntry.escortLastRegion || "" ) + "\n" +
				"EscortFailed=" + ( progressEntry.escortFailed ? "1" : "0" ) + "\n" +
				"LastTravelAmbushCheck=" + ( progressEntry.lastTravelAmbushCheck || 0 ) + "\n" +
				"SelectedWaypoints=" + SerializeSelectedWaypoints( progressEntry.selectedWaypoints ) + "\n" +
				"SelectedDestinationRegionID=" + ( progressEntry.selectedDestinationRegionID || 0 ) + "\n" +
				"SelectedDestinationRegionName=" + ( progressEntry.selectedDestinationRegionName || "" ) + "\n" +
				"SkillProgress=" + ( progressEntry.skillProgress || 0 ) + "\n" +
				"TargetSkill=" + ( progressEntry.targetSkill || -1 ) + "\n" +
				"TargetRegion=" + ( progressEntry.targetRegion || 0 ) + "\n" +
				"MaxSkillPoints=" + ( progressEntry.maxSkillPoints || 50.0 ) + "\n" +
				"StartTime=" + ( progressEntry.startTime || 0 ) + "\n" +
				"TimeLimit=" + ( progressEntry.timeLimit || 0 ) + "\n" +
				"RaceNPCSerial=" + ( progressEntry.raceNPCSerial || 0 ) + "\n" +
				"RaceUsesQuestGiver=" + ( progressEntry.raceUsesQuestGiver ? "1" : "0" ) + "\n" +
				"RaceCheckpoint=" + ( progressEntry.raceCheckpoint || 0 ) + "\n" +
				"RaceCompleted=" + ( progressEntry.raceCompleted ? "1" : "0" ) + "\n" +
				"RaceWinner=" + ( progressEntry.raceWinner || "" ) + "\n" +
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
	entry.raceNPCSerial = parseInt( entry.racenpcserial || "0", 10 );
	if( isNaN( entry.raceNPCSerial ) )
	{
		entry.raceNPCSerial = 0;
	}

	entry.raceUsesQuestGiver = ( entry.raceusesquestgiver == "1" );

	entry.raceCheckpoint = parseInt( entry.racecheckpoint || "0", 10 );
	if( isNaN( entry.raceCheckpoint ) )
	{
		entry.raceCheckpoint = 0;
	}

	entry.raceCompleted = ( entry.racecompleted == "1" );
	entry.raceWinner = entry.racewinner || "";

	entry.escortNPCSerial = parseInt( entry.escortnpcserial || "0", 10 );
	if( isNaN( entry.escortNPCSerial ) )
		entry.escortNPCSerial = 0;

	entry.escortUsesQuestGiver = ( entry.escortusesquestgiver == "1" );
	entry.escortStage = parseInt( entry.escortstage || "0", 10 );
	if( isNaN( entry.escortStage ) )
		entry.escortStage = 0;

	entry.escortLastRegion = entry.escortlastregion || "";
	entry.escortFailed = ( entry.escortfailed == "1" );

	entry.lastTravelAmbushCheck = parseInt( entry.lasttravelambushcheck || "0", 10 );
	if( isNaN( entry.lastTravelAmbushCheck ) )
		entry.lastTravelAmbushCheck = 0;

	entry.selectedWaypoints = ParseSelectedWaypoints( entry.selectedwaypoints || "" );

	entry.selectedDestinationRegionID = parseInt( entry.selecteddestinationregionid || "0", 10 );
	if( isNaN( entry.selectedDestinationRegionID ) )
		entry.selectedDestinationRegionID = 0;

	entry.selectedDestinationRegionName = entry.selecteddestinationregionname || "";

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