/// <reference path="../../../definitions.d.ts" />
// @ts-check

var questVersion = 1;
var playerSettingsVersion = 1;

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
	var tamedCreatures = {};
	var tamedCreatureSerials = {};

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

	if( quest.targetTames )
	{
		for( var targetTameIndex = 0; targetTameIndex < quest.targetTames.length; targetTameIndex++ )
		{
			var targetTame = quest.targetTames[targetTameIndex];
			tamedCreatures[targetTame.npcID] = 0;
		}
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
		var escortNPC = ResolveQuestEscortNPC( player, questID, quest, questGiver );
		if( !ValidateObject( escortNPC ) )
		{
			socket.SysMessage( "Unable to start escort quest." );
			return;
		}

		selectedWaypoints = BuildEscortSelectedWaypoints( quest, escortNPC, player );

		if( !selectedWaypoints.length )
		{
			CleanupEscortQuestNPC( player, questID );
			socket.SysMessage( "Unable to determine escort destination." );
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
		tamedCreatures: tamedCreatures,
		tamedCreatureSerials: tamedCreatureSerials,
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
		var archivedQuestEntries = ReadArchivedQuestEntries( player );

		for( var dailyArchiveIndex = 0; dailyArchiveIndex < archivedQuestEntries.length; dailyArchiveIndex++ )
		{
			var archivedQuestEntry = archivedQuestEntries[dailyArchiveIndex];
			if( !archivedQuestEntry )
			{
				continue;
			}

			if( parseInt( archivedQuestEntry.questID, 10 ) == questID )
			{
				var lastCompleted = parseInt( archivedQuestEntry.lastCompleted, 10 );
				if( isNaN( lastCompleted))
				{
					lastCompleted = 0;
				}

				var resetTime = quest.resetDailyTime || 24;
				var currentTime = Date.now();

				if(( currentTime - lastCompleted ) < resetTime * 3600 * 1000 )
				{
					var hoursLeft = Math.ceil(( resetTime * 3600 * 1000 - ( currentTime - lastCompleted )) / ( 3600 * 1000 ));
					if (socket != null)
					{
						socket.SysMessage( "This is a daily quest, and you must wait " + hoursLeft + " more hour(s) to attempt it again." );
					}
					return false;
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
	if( !ValidateObject( timerObj ) || !timerObj.isChar )
	{
		return;
	}

	var player = timerObj;
	var socket = player.socket;
	var questProgressArray = ReadQuestProgress( player );

	if( !questProgressArray || !questProgressArray.length )
	{
		return;
	}

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];

		if( !questEntry || questEntry.questID != timerID || questEntry.completed )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest )
		{
			LogFailedQuest( player, questEntry );
			questProgressArray.splice( i, 1 );
			WriteQuestProgress( player, questProgressArray );
			return;
		}

		TriggerEvent( 5802, "ManageQuestItems", player, questEntry.questID, false );

		if( quest.type == "skillgain" )
		{
			player.SetTag( "AcceleratedSkillGain", null );
			player.RemoveScriptTrigger( 5811 );

			if( socket != null )
			{
				socket.SysMessage( "You have stopped accelerated training for " + GetSkillName( quest.targetSkill ) + "." );
			}
		}

		if( socket != null )
		{
			socket.SysMessage( "You have failed the timed quest: " + quest.title );
		}

		LogFailedQuest( player, questEntry );

		if( quest.type == "escort" )
		{
			CleanupEscortQuestNPC( player, questEntry.questID );
		}

		if( quest.guidedWalk && quest.guidedWalk.enabled )
		{
			CleanupGuidedWalkQuestNPC( player, questEntry.questID );
		}

		if( quest.race && quest.race.enabled )
		{
			CleanupRaceQuestNPC( player, questEntry.questID );
		}

		questProgressArray.splice( i, 1 );
		WriteQuestProgress( player, questProgressArray );
		return;
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
	if( !quest || !questEntry )
	{
		return false;
	}

	var hasObjectives = false;

	if( quest.type == "collect" || quest.type == "timecollect" || quest.type == "multi" )
	{
		if( quest.targetItems )
		{
			for( var targetItemIndex = 0; targetItemIndex < quest.targetItems.length; targetItemIndex++ )
			{
				var exactTarget = quest.targetItems[targetItemIndex];
				if( !exactTarget || !exactTarget.sectionID || exactTarget.amount == null )
				{
					return false;
				}

				hasObjectives = true;

				if( !questEntry.collectedItems || ( questEntry.collectedItems[exactTarget.sectionID] || 0 ) < exactTarget.amount )
				{
					return false;
				}
			}
		}

		if( quest.targetItemGroups )
		{
			for( var targetGroupIndex = 0; targetGroupIndex < quest.targetItemGroups.length; targetGroupIndex++ )
			{
				var targetGroup = quest.targetItemGroups[targetGroupIndex];
				if( !targetGroup || !targetGroup.groupID || targetGroup.amount == null )
				{
					return false;
				}

				hasObjectives = true;

				if( !questEntry.collectedItemGroups || ( questEntry.collectedItemGroups[String( targetGroup.groupID )] || 0 ) < targetGroup.amount )
				{
					return false;
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
				if( !targetKill || !targetKill.npcID || targetKill.amount == null )
				{
					return false;
				}

				hasObjectives = true;

				if( !questEntry.harvestKills || ( questEntry.harvestKills[targetKill.npcID] || 0 ) < targetKill.amount )
				{
					return false;
				}
			}
		}

		if( quest.targetKillGroups )
		{
			for( var targetKillGroupIndex = 0; targetKillGroupIndex < quest.targetKillGroups.length; targetKillGroupIndex++ )
			{
				var targetKillGroup = quest.targetKillGroups[targetKillGroupIndex];
				if( !targetKillGroup || !targetKillGroup.groupID || targetKillGroup.amount == null )
				{
					return false;
				}

				hasObjectives = true;

				if( !questEntry.harvestKillGroups || ( questEntry.harvestKillGroups[String( targetKillGroup.groupID )] || 0 ) < targetKillGroup.amount )
				{
					return false;
				}
			}
		}
	}

	if( quest.type == "tame" || quest.type == "multi" )
	{
		if( quest.targetTames )
		{
			for( var targetTameIndex = 0; targetTameIndex < quest.targetTames.length; targetTameIndex++ )
			{
				var targetTame = quest.targetTames[targetTameIndex];
				if( !targetTame || !targetTame.npcID || targetTame.amount == null )
				{
					return false;
				}

				hasObjectives = true;

				if( !questEntry.tamedCreatures || ( questEntry.tamedCreatures[targetTame.npcID] || 0 ) < targetTame.amount )
				{
					return false;
				}
			}
		}
	}

	if( quest.race && quest.race.enabled )
	{
		hasObjectives = true;

		if( !questEntry.raceCompleted )
		{
			return false;
		}
	}

	if( !hasObjectives )
	{
		return false;
	}

	return true;
}

/** @type { ( player: Character, questID: number, identifier: number | string, progressValue: number, type: string ) => any[] } */
function UpdateQuestProgress( player, questID, identifier, progressValue, type )
{
	if( !ValidateObject( player ))
		return;

	var socket = player.socket;
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

		if( !questEntry.tamedCreatures )
		{
			questEntry.tamedCreatures = {};
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

		if( type == "tame" )
		{
			if( quest.targetTames )
			{
				for( var targetTameIndex = 0; targetTameIndex < quest.targetTames.length; targetTameIndex++ )
				{
					var targetTame = quest.targetTames[targetTameIndex];

					if( String(targetTame.npcID) == String(identifier) )
					{
						questEntry.tamedCreatures[String(identifier)] = ( questEntry.tamedCreatures[String(identifier)] || 0) + progressValue;

						if( questEntry.tamedCreatures[String(identifier)] > targetTame.amount )
						{
							questEntry.tamedCreatures[String(identifier)] = targetTame.amount;
						}
					}
				}
			}
		}

		if( quest.type == "delivery" && quest.targetDelivery && quest.deliveryItem && String( quest.targetDelivery.npcID ) == String( identifier ))
		{
			if( String( identifier ) == String( quest.targetDelivery.npcID ))
			{
				var pack = player.pack;
				var hasItem = false;
				var totalDeliveryAmount = 0;
				var questItemsToProcess = [];

				if( !ValidateObject( pack ))
				{
					if( socket != null )
					{
						socket.SysMessage( GetDictionaryEntry( 19621, socket.language ));
					}
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
					if( socket != null )
					{
						socket.SysMessage( GetDictionaryEntry( 19620, socket.language ));
					}
				}
				else
				{
					if( socket != null )
					{
						socket.SysMessage( GetDictionaryEntry( 19621, socket.language ));
					}
				}
			}
			else
			{
				if( socket != null )
				{
					socket.SysMessage( GetDictionaryEntry( 19622, socket.language ));
				}
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
				if( socket != null )
				{
					socket.SysMessage( "Skill progress: " + ( questEntry.skillProgress / 10 ).toFixed( 1 ) + "/" + ( quest.maxSkillPoints / 10 ).toFixed( 1 ));
				}
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
					if( socket != null )
					{
						socket.SysMessage( quest.oncomplete );
					}
				}
				else
				{
					if( socket != null )
					{
						socket.SysMessage( GetDictionaryEntry( 19623, socket.language ));
					}
				}
				WriteQuestProgress( player, questProgressArray );
			}
			else
			{
				questEntry.completed = true;
				if( quest.type == "skillgain" && quest.oncomplete )
				{
					if( socket != null )
					{
						socket.SysMessage( quest.oncomplete );
					}
				}
				else
				{
					if( socket != null )
					{
						socket.SysMessage( GetDictionaryEntry( 19623, socket.language ));
					}
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
		if( socket != null )
		{
			socket.SysMessage( GetDictionaryEntry( 19624, socket.language ));
		}
	}

	return questProgressArray;
}

/** @type { ( player: Character, questID: number ) => void } */
function CompleteQuest( player, questID )
{
	if( !ValidateObject( player ))
		return;	

	var socket = player.socket;
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
			if( socket != null )
			{
				socket.SysMessage( GetDictionaryEntry( 19625, socket.language ));//You haven't completed the quest yet.
			}
			return;
		}

		var quest = TriggerEvent( 5801, "QuestList", questID );
		if( !quest )
		{
			if( socket != null )
			{
				socket.SysMessage( GetDictionaryEntry( 19626, socket.language )); //Quest data could not be retrieved.
			}
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
		if( socket != null )
		{
				
			socket.SysMessage( "Congratulations! You have completed the quest: " + quest.title );
		}
		DoStaticEffect( player.x, player.y, player.z, 0x376A, 0x40, 0x16, false );

		// Handle rewards
		if( quest.rewards && socket != null )
		{
			QuestRewards( player, quest, socket );
		}

		// Archive the completed quest
		ArchiveCompletedQuest( player, questEntry );

		ApplyQuestModifiers( player, quest );

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
function ApplyQuestModifiers( player, quest )
{
	if( !ValidateObject( player ) || !quest )
	{
		return;
	}

	ApplyModifierSet( player, quest.setTags, "tag" );
	ApplyModifierDelta( player, quest.setTagDeltas, quest.deltaRules, quest.deltaRulesDefault, "tag" );

	ApplyModifierSet( player, quest.setTempTags, "temptag" );
	ApplyModifierDelta( player, quest.setTempTagDeltas, quest.tempDeltaRules, quest.tempDeltaRulesDefault, "temptag" );

	ApplyModifierSet( player, quest.setWorldState, "worldstate" );
	ApplyModifierDelta( player, quest.worldStateDeltas, quest.worldStateDeltaRules, quest.worldStateDeltaRulesDefault, "worldstate" );
}

/** @type { ( player: Character, modifierData: any, modifierType: string ) => void } */
function ApplyModifierSet( player, modifierData, modifierType )
{
	if( !ValidateObject( player ) || !modifierData )
	{
		return;
	}

	for( var key in modifierData )
	{
		if( !modifierData.hasOwnProperty( key ) )
		{
			continue;
		}

		SetModifierValue( player, key, modifierData[key], modifierType );
	}
}

/** @type { ( player: Character, modifierData: any, modifierRules: any, defaultRule: any, modifierType: string ) => void } */
function ApplyModifierDelta( player, modifierData, modifierRules, defaultRule, modifierType )
{
	if( !ValidateObject( player ) || !modifierData )
	{
		return;
	}

	for( var key in modifierData )
	{
		if( !modifierData.hasOwnProperty( key ))
		{
			continue;
		}

		var deltaAmount = modifierData[key];

		if( typeof deltaAmount == "string" )
		{
			deltaAmount = parseInt( deltaAmount, 10 );
			if( isNaN( deltaAmount ))
			{
				deltaAmount = 0;
			}
		}

		var currentValue = GetModifierInt( player, key, modifierType );
		var nextValue = currentValue + deltaAmount;

		var clampRule = ResolveModifierRule( key, modifierRules, defaultRule );

		if( clampRule )
		{
			if( clampRule.hasOwnProperty( "min" ))
			{
				var minValue = clampRule.min;

				if( typeof minValue == "string" )
				{
					minValue = parseInt( minValue, 10 );
				}

				if( !isNaN( minValue ) && nextValue < minValue )
				{
					nextValue = minValue;
				}
			}

			if( clampRule.hasOwnProperty( "max" ))
			{
				var maxValue = clampRule.max;

				if( typeof maxValue == "string" )
				{
					maxValue = parseInt( maxValue, 10 );
				}

				if( !isNaN( maxValue ) && nextValue > maxValue )
				{
					nextValue = maxValue;
				}
			}
		}

		SetModifierValue( player, key, nextValue, modifierType );
	}
}

/** @type { ( player: Character, key: string, modifierType: string ) => number } */
function GetModifierInt( player, key, modifierType )
{
	var currentValue = 0;

	if( modifierType == "tag" )
	{
		currentValue = player.GetTag( key );
	}
	else if( modifierType == "temptag" )
	{
		currentValue = player.GetTempTag( key );
	}
	else if( modifierType == "worldstate" )
	{
		currentValue = GetWorldStateInt( player, key, 0 );
	}

	if( typeof currentValue == "string" )
	{
		currentValue = parseInt( currentValue, 10 );
		if( isNaN( currentValue ))
		{
			currentValue = 0;
		}
	}

	return currentValue || 0;
}

/** @type { ( player: Character, key: string, value: any, modifierType: string ) => void } */
function SetModifierValue( player, key, value, modifierType )
{
	if( modifierType == "tag" )
	{
		player.SetTag( key, value == null ? null : value );
	}
	else if( modifierType == "temptag" )
	{
		player.SetTempTag( key, value == null ? null : value );
	}
	else if( modifierType == "worldstate" )
	{
		SetWorldStateInt( player, key, value == null ? null : ToIntOrZero( value ) );
	}
}

/** @type { ( key: string, modifierRules: any, defaultRule: any ) => any } */
function ResolveModifierRule( key, modifierRules, defaultRule )
{
	if( modifierRules && modifierRules.hasOwnProperty( key ) && modifierRules[key] )
	{
		return modifierRules[key];
	}

	if( defaultRule )
	{
		return defaultRule;
	}

	return null;
}

/** @type { ( value: any ) => number } */
function ToIntOrZero( value )
{
	if( value == null || value === "" )
	{
		return 0;
	}

	if( typeof value == "number" )
	{
		return isNaN( value ) ? 0 : value;
	}

	var parsedValue = parseInt( value, 10 );
	if( isNaN( parsedValue ) )
	{
		return 0;
	}

	return parsedValue;
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
		if( socket != null )
		{
			socket.SysMessage( "Escort progress updated: region " + enteredRegionID );
		}

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
						if( socket != null )
						{
							socket.SysMessage( "Ambush!" );
						}

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
				if( socket != null )
				{
					socket.SysMessage( GetDictionaryEntry( 19623, socket.language ));
				}
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
		if( socket != null )
		{
			socket.SysMessage( failMessage || "You have failed the escort quest." );
		}
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

			if( quest.guidedWalk && quest.guidedWalk.enabled )
			{
				CleanupGuidedWalkQuestNPC( player, questEntry.questID );
			}

			if( quest.race && quest.race.enabled )
			{
				CleanupRaceQuestNPC( player, questEntry.questID );
			}

			questEntry.escortFailed = ( quest.type == "escort" );
			LogFailedQuest( player, questEntry );

			if( socket != null )
			{
				socket.SysMessage( "You have failed the timed quest: " + quest.title );
			}

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

/** @type { ( player: Character ) => void } */
function ValidateGuidedWalkQuestsOnLogin( player )
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

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest || !quest.guidedWalk || !quest.guidedWalk.enabled )
		{
			continue;
		}

		var guidedWalkNPCSerial = parseInt( questEntry.guidedWalkNPCSerial, 10 );
		if( isNaN( guidedWalkNPCSerial ) || guidedWalkNPCSerial <= 0 )
		{
			continue;
		}

		var guideNpc = CalcCharFromSer( guidedWalkNPCSerial );
		if( !ValidateObject( guideNpc ) )
		{
			continue;
		}

		if( !guideNpc.HasScriptTrigger( 5816 ) )
		{
			guideNpc.AddScriptTrigger( 5816 );
		}

		guideNpc.StartTimer( 1000, 1, 5816 );
	}
}

/** @type { ( player: Character ) => void } */
function ValidateRaceQuestsOnLogin( player )
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

	for( var i = 0; i < questProgressArray.length; i++ )
	{
		var questEntry = questProgressArray[i];
		if( !questEntry || questEntry.serial != player.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest || !quest.race || !quest.race.enabled )
		{
			continue;
		}

		var raceNPCSerial = parseInt( questEntry.raceNPCSerial, 10 );
		if( isNaN( raceNPCSerial ) || raceNPCSerial <= 0 )
		{
			continue;
		}

		var raceNpc = CalcCharFromSer( raceNPCSerial );
		if( !ValidateObject( raceNpc ) )
		{
			continue;
		}

		if( !raceNpc.HasScriptTrigger( 5817 ) )
		{
			raceNpc.AddScriptTrigger( 5817 );
		}

		raceNpc.StartTimer( 1000, 1, 5817 );
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

/** @type { ( destinationPool: any[], currentRegionID?: number ) => any | null } */
function PickRandomEscortDestination( destinationPool, currentRegionID )
{
	if( !destinationPool || !destinationPool.length )
	{
		return null;
	}

	currentRegionID = parseInt( currentRegionID, 10 );
	if( isNaN( currentRegionID ) )
	{
		currentRegionID = 0;
	}

	var validDestinationPool = [];

	for( var destinationIndex = 0; destinationIndex < destinationPool.length; destinationIndex++ )
	{
		var destinationEntry = destinationPool[destinationIndex];
		if( !destinationEntry || typeof destinationEntry != "object" )
		{
			continue;
		}

		var destinationRegionID = parseInt( destinationEntry.regionID, 10 );
		if( isNaN( destinationRegionID ) || destinationRegionID <= 0 )
		{
			continue;
		}

		if( currentRegionID > 0 && destinationRegionID == currentRegionID )
		{
			continue;
		}

		validDestinationPool.push( destinationEntry );
	}

	if( !validDestinationPool.length )
	{
		return null;
	}

	var totalWeight = 0;
	for( var validIndex = 0; validIndex < validDestinationPool.length; validIndex++ )
	{
		var validDestinationEntry = validDestinationPool[validIndex];
		var entryWeight = parseInt( validDestinationEntry.weight, 10 );

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

	for( var pickIndex = 0; pickIndex < validDestinationPool.length; pickIndex++ )
	{
		var poolEntry = validDestinationPool[pickIndex];
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

	return validDestinationPool[0] || null;
}

/** @type { ( quest: any, escortNPC?: Character | null, player?: Character | null ) => any[] } */
function BuildEscortSelectedWaypoints( quest, escortNPC, player )
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
		var currentRegionID = 0;

		if( ValidateObject( escortNPC ) && escortNPC.region )
		{
			currentRegionID = parseInt( escortNPC.region.id, 10 );
		}

		if(( isNaN( currentRegionID ) || currentRegionID <= 0 ) && ValidateObject( player ) && player.region )
		{
			currentRegionID = parseInt( player.region.id, 10 );
		}

		if( isNaN( currentRegionID ) )
		{
			currentRegionID = 0;
		}

		var chosenDestination = PickRandomEscortDestination( quest.randomDestinationPool, currentRegionID );
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
	questGiver.SetTag( "QuestRaceOriginalFrozen", questGiver.frozen ? 1 : 0 );
	questGiver.frozen = false;
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
		var originalFrozen = parseInt( raceNpc.GetTag( "QuestRaceOriginalFrozen" ), 10 );

		if( !isNaN( originalCanRun ) )
		{
			raceNpc.canRun = ( originalCanRun == 1 );
		}

		if( !isNaN( originalFrozen ) )
		{
			raceNpc.frozen = ( originalFrozen == 1 );
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
		raceNpc.SetTag( "QuestRaceOriginalFrozen", null );
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
			if( socket )
			{
				socket.SysMessage( quest.race.npcWinText || "You lost the race." );
			}
			CleanupRaceQuestNPC( player, questID );
			continue;
		}

		questEntry.raceCompleted = true;
		questEntry.raceWinner = "npc";

		if( quest.race.npcWinText )
		{
			if( socket )
			{
				socket.SysMessage( String( quest.race.npcWinText ) );
			}
		}
		else
		{
			if( socket )
			{
				socket.SysMessage( "The racer beat you, but the quest continues." );
			}
		}

		var allObjectivesCompleted = AreAllQuestObjectivesComplete( quest, questEntry );
		if( allObjectivesCompleted )
		{
			if( quest.questTurnIn == 1 || npcWinMode == "turnin" )
			{
				questEntry.completed = true;
				if( socket )
				{
					socket.SysMessage( GetDictionaryEntry( 19623, socket.language ));
				}
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
	questGiver.SetTag( "QuestGuidedWalkOriginalFrozen", questGiver.frozen ? 1 : 0 );
	questGiver.frozen = false;

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
		var originalFrozen = parseInt( guideNpc.GetTag( "QuestGuidedWalkOriginalFrozen" ), 10 );

		if( !isNaN( originalFrozen ) )
		{
			guideNpc.frozen = ( originalFrozen == 1 );
		}

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
		guideNpc.SetTag( "QuestGuidedWalkOriginalFrozen", null );

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

/** @type { ( player: Character, creature: Character ) => boolean } */
function CreatureTamed( player, creature )
{
	if( !ValidateObject( player ) || !ValidateObject( creature ) )
	{
		return false;
	}

	var socket = player.socket;
	var questProgressArray = ReadQuestProgress( player );

	for( var questEntryIndex = 0; questEntryIndex < questProgressArray.length; questEntryIndex++ )
	{
		var questEntry = questProgressArray[questEntryIndex];

		if( questEntry.serial != player.serial || questEntry.completed )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest )
		{
			continue;
		}

		if( quest.type != "tame" && quest.type != "multi" )
		{
			continue;
		}

		if( !quest.targetTames )
		{
			continue;
		}

		if( !questEntry.tamedCreatureSerials )
		{
			questEntry.tamedCreatureSerials = {};
		}

		for( var targetTameIndex = 0; targetTameIndex < quest.targetTames.length; targetTameIndex++ )
		{
			var targetTame = quest.targetTames[targetTameIndex];

			if( String( targetTame.npcID ) != String( creature.sectionID ))
			{
				continue;
			}

			var tameSerialKey = String( creature.serial );

			if( questEntry.tamedCreatureSerials[tameSerialKey] )
			{
				if( socket )
				{
					socket.SysMessage( "You have already counted this creature for that quest." );
				}
				continue;
			}

			questEntry.tamedCreatureSerials[tameSerialKey] = 1;
			WriteQuestProgress( player, questProgressArray );

			UpdateQuestProgress( player, questEntry.questID, creature.sectionID, 1, "tame" );
			break;
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
	if( !ValidateObject( player ) || !player.account )
	{
		return {};
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "PlayerSettings_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	mFile.Open( fileName, "r", "Settings" );
	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return {};
	}

	var fileText = "";
	while( !mFile.EOF() )
	{
		var rawLine = mFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !mFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	mFile.Close();
	mFile.Free();

	fileText = fileText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
	if( fileText == "" )
	{
		return {};
	}

	try
	{
		var parsedSettings = JSON.parse( fileText );
		if( parsedSettings && parsedSettings.characters && parsedSettings.characters[playerKey] )
		{
			return parsedSettings.characters[playerKey];
		}
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to parse " + fileName + ": " + error );
	}

	return {};
}

/** @type { ( player: Character, settings: any ) => boolean } */
function SavePlayerSettings( player, settings )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return false;
	}

	if( !settings || typeof settings != "object" )
	{
		settings = {};
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "PlayerSettings_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var saveObject = {
		version: playerSettingsVersion,
		characters: {}
	};

	mFile.Open( fileName, "r", "Settings" );
	if( mFile && mFile.Length() > 0 )
	{
		var existingText = "";
		while( !mFile.EOF() )
		{
			var rawLine = mFile.ReadUntil( "\n" );
			if( rawLine != null && typeof rawLine != "undefined" )
			{
				existingText += rawLine;
				if( !mFile.EOF() )
				{
					existingText += "\n";
				}
			}
		}

		mFile.Close();
		mFile.Free();

		existingText = existingText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
		if( existingText != "" )
		{
			try
			{
				var parsedSettings = JSON.parse( existingText );
				if( parsedSettings && typeof parsedSettings == "object" )
				{
					saveObject = parsedSettings;
				}

				if( !saveObject.characters )
				{
					saveObject.characters = {};
				}
			}
			catch( error )
			{
				Console.Warning( "Quest system: Failed to parse existing " + fileName + ": " + error );
			}
		}
	}
	else if( mFile )
	{
		mFile.Free();
	}

	saveObject.version = playerSettingsVersion;
	saveObject.characters[playerKey] = settings;

	var fileText = "";
	try
	{
		fileText = JSON.stringify( saveObject, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to serialize " + fileName + ": " + error );
		return false;
	}

	mFile = new UOXCFile();
	mFile.Open( fileName, "w", "Settings" );
	if( !mFile )
	{
		return false;
	}

	mFile.Write( fileText + "\n" );
	mFile.Close();
	mFile.Free();

	return true;
}

/** @type { ( player: Character, failedQuest: any ) => void } */
function LogFailedQuest( player, failedQuest )
{
	if( !ValidateObject( player ) || !player.account || !failedQuest )
	{
		return;
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "FailedQuests_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var failedQuestData = {
		version: questVersion,
		characters: {}
	};

	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length() > 0 )
	{
		var existingText = "";
		while( !mFile.EOF() )
		{
			var rawLine = mFile.ReadUntil( "\n" );
			if( rawLine != null && typeof rawLine != "undefined" )
			{
				existingText += rawLine;
				if( !mFile.EOF() )
				{
					existingText += "\n";
				}
			}
		}

		mFile.Close();
		mFile.Free();

		existingText = existingText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
		if( existingText != "" )
		{
			try
			{
				var parsedData = JSON.parse( existingText );
				if( parsedData && typeof parsedData == "object" )
				{
					failedQuestData = parsedData;
				}
			}
			catch( error )
			{
				Console.Warning( "Quest system: Failed to parse existing " + fileName + ": " + error );
			}
		}
	}
	else if( mFile )
	{
		mFile.Free();
	}

	if( !failedQuestData.characters )
	{
		failedQuestData.characters = {};
	}

	if( !failedQuestData.characters[playerKey] )
	{
		failedQuestData.characters[playerKey] = [];
	}

	failedQuestData.version = questVersion;

	var failedQuestList = failedQuestData.characters[playerKey];
	var failedQuestID = parseInt( failedQuest.questID, 10 );
	if( isNaN( failedQuestID ) )
	{
		failedQuestID = 0;
	}

	var existingFailedIndex = -1;
	for( var failedIndex = 0; failedIndex < failedQuestList.length; failedIndex++ )
	{
		if( parseInt( failedQuestList[failedIndex].questID, 10 ) == failedQuestID )
		{
			existingFailedIndex = failedIndex;
			break;
		}
	}

	var failedQuestEntry = {
		serial: player.serial,
		questID: failedQuestID,
		failedAt: Date.now(),
		failed: true,
		questProgress: failedQuest.questProgress || 0,
		harvestKills: failedQuest.harvestKills || {},
		harvestKillGroups: failedQuest.harvestKillGroups || {},
		collectedItems: failedQuest.collectedItems || {},
		collectedItemGroups: failedQuest.collectedItemGroups || {},
		tamedCreatures: failedQuest.tamedCreatures || {},
		tamedCreatureSerials: failedQuest.tamedCreatureSerials || {},
		startTime: failedQuest.startTime || 0,
		timeLimit: failedQuest.timeLimit || 0,
		escortNPCSerial: failedQuest.escortNPCSerial || 0,
		escortUsesQuestGiver: failedQuest.escortUsesQuestGiver ? true : false,
		escortStage: failedQuest.escortStage || 0,
		escortLastRegion: failedQuest.escortLastRegion || "",
		escortFailed: true,
		lastTravelAmbushCheck: failedQuest.lastTravelAmbushCheck || 0,
		selectedWaypoints: failedQuest.selectedWaypoints || [],
		selectedDestinationRegionID: failedQuest.selectedDestinationRegionID || 0,
		selectedDestinationRegionName: failedQuest.selectedDestinationRegionName || "",
		guidedWalkNPCSerial: failedQuest.guidedWalkNPCSerial || 0,
		guidedWalkUsesQuestGiver: failedQuest.guidedWalkUsesQuestGiver ? true : false,
		raceNPCSerial: failedQuest.raceNPCSerial || 0,
		raceUsesQuestGiver: failedQuest.raceUsesQuestGiver ? true : false,
		raceCheckpoint: failedQuest.raceCheckpoint || 0,
		raceCompleted: failedQuest.raceCompleted ? true : false,
		raceWinner: failedQuest.raceWinner || "",
		completed: false,
		questTurnIn: false
	};

	if( existingFailedIndex >= 0 )
	{
		failedQuestList[existingFailedIndex] = failedQuestEntry;
	}
	else
	{
		failedQuestList.push( failedQuestEntry );
	}

	var fileText = "";
	try
	{
		fileText = JSON.stringify( failedQuestData, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to serialize " + fileName + ": " + error );
		return;
	}

	mFile = new UOXCFile();
	mFile.Open( fileName, "w", "Quests" );
	if( !mFile )
	{
		return;
	}

	mFile.Write( fileText + "\n" );
	mFile.Close();
	mFile.Free();
}

/** @type { ( player: Character ) => number[] } */
function ReadFailedQuests( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return [];
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "FailedQuests_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var failedQuests = [];

	mFile.Open( fileName, "r", "Quests" );
	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return failedQuests;
	}

	var fileText = "";
	while( !mFile.EOF() )
	{
		var rawLine = mFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !mFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	mFile.Close();
	mFile.Free();

	fileText = fileText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
	if( fileText == "" )
	{
		return failedQuests;
	}

	try
	{
		var failedQuestData = JSON.parse( fileText );
		if( !failedQuestData || !failedQuestData.characters || !failedQuestData.characters[playerKey] )
		{
			return failedQuests;
		}

		var characterFailedQuests = failedQuestData.characters[playerKey];
		if( !characterFailedQuests || !characterFailedQuests.length )
		{
			return failedQuests;
		}

		for( var i = 0; i < characterFailedQuests.length; i++ )
		{
			var failedEntry = characterFailedQuests[i];
			if( !failedEntry )
			{
				continue;
			}

			var questID = parseInt( failedEntry.questID, 10 );
			if( !isNaN( questID ) && questID > 0 )
			{
				failedQuests.push( questID );
			}
		}
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to parse " + fileName + ": " + error );
		return [];
	}

	return failedQuests;
}

/** @type { ( player: Character, completedQuest: any ) => void } */
function ArchiveCompletedQuest( player, completedQuest )
{
	if( !ValidateObject( player ) || !player.account || !completedQuest )
	{
		return;
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestArchive_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var archiveData = {
		version: questVersion,
		characters: {}
	};

	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length() > 0 )
	{
		var existingText = "";
		while( !mFile.EOF() )
		{
			var rawLine = mFile.ReadUntil( "\n" );
			if( rawLine != null && typeof rawLine != "undefined" )
			{
				existingText += rawLine;
				if( !mFile.EOF() )
				{
					existingText += "\n";
				}
			}
		}

		mFile.Close();
		mFile.Free();

		existingText = existingText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
		if( existingText != "" )
		{
			try
			{
				var parsedData = JSON.parse( existingText );
				if( parsedData && typeof parsedData == "object" )
				{
					archiveData = parsedData;
				}
			}
			catch( error )
			{
				Console.Warning( "Quest system: Failed to parse existing " + fileName + ": " + error );
			}
		}
	}
	else if( mFile )
	{
		mFile.Free();
	}

	if( !archiveData.characters )
	{
		archiveData.characters = {};
	}

	if( !archiveData.characters[playerKey] )
	{
		archiveData.characters[playerKey] = [];
	}

	archiveData.version = questVersion;

	var quest = TriggerEvent( 5801, "QuestList", completedQuest.questID );
	var nextQuestID = quest ? quest.nextQuestID || null : null;
	var completedQuestID = parseInt( completedQuest.questID, 10 );
	if( isNaN( completedQuestID ) )
	{
		completedQuestID = 0;
	}

	var archivedQuestList = archiveData.characters[playerKey];
	var existingArchiveIndex = -1;

	for( var archiveIndex = 0; archiveIndex < archivedQuestList.length; archiveIndex++ )
	{
		if( parseInt( archivedQuestList[archiveIndex].questID, 10 ) == completedQuestID )
		{
			existingArchiveIndex = archiveIndex;
			break;
		}
	}

	var archivedQuestEntry = {
		serial: player.serial,
		questID: completedQuestID,
		nextQuestID: nextQuestID,
		lastCompleted: Date.now(),
		questProgress: completedQuest.questProgress || 0,
		collectedItems: completedQuest.collectedItems || {},
		collectedItemGroups: completedQuest.collectedItemGroups || {},
		harvestKills: completedQuest.harvestKills || {},
		harvestKillGroups: completedQuest.harvestKillGroups || {},
		tamedCreatures: completedQuest.tamedCreatures || {},
		tamedCreatureSerials: completedQuest.tamedCreatureSerials || {},
		escortNPCSerial: completedQuest.escortNPCSerial || 0,
		escortUsesQuestGiver: completedQuest.escortUsesQuestGiver ? true : false,
		escortStage: completedQuest.escortStage || 0,
		escortLastRegion: completedQuest.escortLastRegion || "",
		lastTravelAmbushCheck: completedQuest.lastTravelAmbushCheck || 0,
		selectedWaypoints: completedQuest.selectedWaypoints || [],
		selectedDestinationRegionID: completedQuest.selectedDestinationRegionID || 0,
		selectedDestinationRegionName: completedQuest.selectedDestinationRegionName || "",
		skillProgress: completedQuest.skillProgress || 0,
		targetSkill: completedQuest.targetSkill || -1,
		targetRegion: completedQuest.targetRegion || 0,
		maxSkillPoints: completedQuest.maxSkillPoints || 0,
		startTime: completedQuest.startTime || 0,
		timeLimit: completedQuest.timeLimit || 0,
		guidedWalkNPCSerial: completedQuest.guidedWalkNPCSerial || 0,
		guidedWalkUsesQuestGiver: completedQuest.guidedWalkUsesQuestGiver ? true : false,
		raceNPCSerial: completedQuest.raceNPCSerial || 0,
		raceUsesQuestGiver: completedQuest.raceUsesQuestGiver ? true : false,
		raceCheckpoint: completedQuest.raceCheckpoint || 0,
		raceCompleted: completedQuest.raceCompleted ? true : false,
		raceWinner: completedQuest.raceWinner || "",
		completed: true,
		questTurnIn: true
	};

	if( existingArchiveIndex >= 0 )
	{
		archivedQuestList[existingArchiveIndex] = archivedQuestEntry;
	}
	else
	{
		archivedQuestList.push( archivedQuestEntry );
	}

	var fileText = "";
	try
	{
		fileText = JSON.stringify( archiveData, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to serialize " + fileName + ": " + error );
		return;
	}

	mFile = new UOXCFile();
	mFile.Open( fileName, "w", "Quests" );
	if( !mFile )
	{
		return;
	}

	mFile.Write( fileText + "\n" );
	mFile.Close();
	mFile.Free();
}

/** @type { ( player: Character ) => number[] } */
function ReadArchivedQuests( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return [];
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestArchive_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var archivedQuests = [];

	mFile.Open( fileName, "r", "Quests" );
	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return archivedQuests;
	}

	var fileText = "";
	while( !mFile.EOF() )
	{
		var rawLine = mFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !mFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	mFile.Close();
	mFile.Free();

	fileText = fileText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
	if( fileText == "" )
	{
		return archivedQuests;
	}

	try
	{
		var archiveData = JSON.parse( fileText );
		if( !archiveData || !archiveData.characters || !archiveData.characters[playerKey] )
		{
			return archivedQuests;
		}

		var characterArchivedQuests = archiveData.characters[playerKey];
		if( !characterArchivedQuests || !characterArchivedQuests.length )
		{
			return archivedQuests;
		}

		for( var i = 0; i < characterArchivedQuests.length; i++ )
		{
			var archiveEntry = characterArchivedQuests[i];
			if( !archiveEntry )
			{
				continue;
			}

			var questID = parseInt( archiveEntry.questID, 10 );
			if( !isNaN( questID ) && questID > 0 )
			{
				archivedQuests.push( questID );
			}
		}
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to parse " + fileName + ": " + error );
		return [];
	}

	return archivedQuests;
}

/** @type { ( player: Character ) => any[] } */
function ReadArchivedQuestEntries( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return [];
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestArchive_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var archivedQuestEntries = [];

	mFile.Open( fileName, "r", "Quests" );
	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return archivedQuestEntries;
	}

	var fileText = "";
	while( !mFile.EOF() )
	{
		var rawLine = mFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !mFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	mFile.Close();
	mFile.Free();

	fileText = fileText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
	if( fileText == "" )
	{
		return archivedQuestEntries;
	}

	try
	{
		var archiveData = JSON.parse( fileText );
		if( !archiveData || !archiveData.characters || !archiveData.characters[playerKey] )
		{
			return archivedQuestEntries;
		}

		var characterArchivedQuests = archiveData.characters[playerKey];
		if( !characterArchivedQuests || !characterArchivedQuests.length )
		{
			return archivedQuestEntries;
		}

		for( var i = 0; i < characterArchivedQuests.length; i++ )
		{
			var archiveEntry = characterArchivedQuests[i];
			if( !archiveEntry )
			{
				continue;
			}

			var questID = parseInt( archiveEntry.questID, 10 );
			if( isNaN( questID ) || questID <= 0 )
			{
				continue;
			}

			archivedQuestEntries.push( archiveEntry );
		}
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to parse " + fileName + ": " + error );
		return [];
	}

	return archivedQuestEntries;
}

/** @type { ( player: Character, questProgressArray: any[] ) => boolean } */
function WriteQuestProgress( player, questProgressArray )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return false;
	}

	if( !questProgressArray || !questProgressArray.length )
	{
		questProgressArray = [];
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestProgress_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var progressData = {
		version: questVersion,
		characters: {}
	};

	mFile.Open( fileName, "r", "Quests" );
	if( mFile && mFile.Length() > 0 )
	{
		var existingText = "";
		while( !mFile.EOF() )
		{
			var rawLine = mFile.ReadUntil( "\n" );
			if( rawLine != null && typeof rawLine != "undefined" )
			{
				existingText += rawLine;
				if( !mFile.EOF() )
				{
					existingText += "\n";
				}
			}
		}

		mFile.Close();
		mFile.Free();

		existingText = existingText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
		if( existingText != "" )
		{
			try
			{
				var parsedData = JSON.parse( existingText );
				if( parsedData && typeof parsedData == "object" )
				{
					progressData = parsedData;
				}
			}
			catch( error )
			{
				Console.Warning( "Quest system: Failed to parse existing " + fileName + ": " + error );
			}
		}
	}
	else if( mFile )
	{
		mFile.Free();
	}

	if( !progressData.characters )
	{
		progressData.characters = {};
	}

	progressData.version = questVersion;
	progressData.characters[playerKey] = [];

	for( var progressIndex = 0; progressIndex < questProgressArray.length; progressIndex++ )
	{
		var progressEntry = questProgressArray[progressIndex];
		if( !progressEntry )
		{
			continue;
		}

		if( progressEntry.serial != player.serial )
		{
			continue;
		}

		progressData.characters[playerKey].push({
			serial: player.serial,
			questID: parseInt( progressEntry.questID, 10 ) || 0,
			questProgress: progressEntry.questProgress || 0,

			harvestKills: progressEntry.harvestKills || {},
			harvestKillGroups: progressEntry.harvestKillGroups || {},
			collectedItems: progressEntry.collectedItems || {},
			collectedItemGroups: progressEntry.collectedItemGroups || {},
			tamedCreatures: progressEntry.tamedCreatures || {},
			tamedCreatureSerials: progressEntry.tamedCreatureSerials || {},

			escortNPCSerial: progressEntry.escortNPCSerial || 0,
			escortUsesQuestGiver: progressEntry.escortUsesQuestGiver ? true : false,
			escortStage: progressEntry.escortStage || 0,
			escortLastRegion: progressEntry.escortLastRegion || "",
			escortFailed: progressEntry.escortFailed ? true : false,
			lastTravelAmbushCheck: progressEntry.lastTravelAmbushCheck || 0,
			selectedWaypoints: progressEntry.selectedWaypoints || [],
			selectedDestinationRegionID: progressEntry.selectedDestinationRegionID || 0,
			selectedDestinationRegionName: progressEntry.selectedDestinationRegionName || "",

			skillProgress: progressEntry.skillProgress || 0,
			targetSkill: progressEntry.targetSkill || -1,
			targetRegion: progressEntry.targetRegion || 0,
			maxSkillPoints: progressEntry.maxSkillPoints || 50.0,
			startTime: progressEntry.startTime || 0,
			timeLimit: progressEntry.timeLimit || 0,

			guidedWalkNPCSerial: progressEntry.guidedWalkNPCSerial || 0,
			guidedWalkUsesQuestGiver: progressEntry.guidedWalkUsesQuestGiver ? true : false,

			raceNPCSerial: progressEntry.raceNPCSerial || 0,
			raceUsesQuestGiver: progressEntry.raceUsesQuestGiver ? true : false,
			raceCheckpoint: progressEntry.raceCheckpoint || 0,
			raceCompleted: progressEntry.raceCompleted ? true : false,
			raceWinner: progressEntry.raceWinner || "",

			lastAccepted: progressEntry.lastAccepted || 0,
			completed: progressEntry.completed ? true : false,
			questTurnIn: progressEntry.questTurnIn ? true : false,
			nextQuestID: progressEntry.nextQuestID != null ? progressEntry.nextQuestID : null
		});
	}

	var fileText = "";
	try
	{
		fileText = JSON.stringify( progressData, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to serialize " + fileName + ": " + error );
		return false;
	}

	mFile = new UOXCFile();
	mFile.Open( fileName, "w", "Quests" );
	if( !mFile )
	{
		return false;
	}

	mFile.Write( fileText + "\n" );
	mFile.Close();
	mFile.Free();

	return true;
}

/** @type { ( player: Character ) => any[] } */
function ReadQuestProgress( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return [];
	}

	var mFile = new UOXCFile();
	var userAccount = player.account;
	var fileName = "QuestProgress_" + userAccount.id + ".json";
	var playerKey = String( player.serial );

	var questProgressArray = [];

	mFile.Open( fileName, "r", "Quests" );
	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return questProgressArray;
	}

	var fileText = "";
	while( !mFile.EOF() )
	{
		var rawLine = mFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !mFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	mFile.Close();
	mFile.Free();

	fileText = fileText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
	if( fileText == "" )
	{
		return questProgressArray;
	}

	try
	{
		var progressData = JSON.parse( fileText );
		if( !progressData || !progressData.characters || !progressData.characters[playerKey] )
		{
			return questProgressArray;
		}

		var characterProgress = progressData.characters[playerKey];
		if( !characterProgress || !characterProgress.length )
		{
			return questProgressArray;
		}

		for( var progressIndex = 0; progressIndex < characterProgress.length; progressIndex++ )
		{
			var savedEntry = characterProgress[progressIndex];
			if( !savedEntry )
			{
				continue;
			}

			var questID = parseInt( savedEntry.questID, 10 );
			if( isNaN( questID ) || questID <= 0 )
			{
				continue;
			}

			questProgressArray.push({
				serial: parseInt( savedEntry.serial, 10 ) || player.serial,
				questID: questID,
				questProgress: savedEntry.questProgress || 0,

				harvestKills: savedEntry.harvestKills || {},
				harvestKillGroups: savedEntry.harvestKillGroups || {},
				collectedItems: savedEntry.collectedItems || {},
				collectedItemGroups: savedEntry.collectedItemGroups || {},
				tamedCreatures: savedEntry.tamedCreatures || {},
				tamedCreatureSerials: savedEntry.tamedCreatureSerials || {},

				escortNPCSerial: parseInt( savedEntry.escortNPCSerial, 10 ) || 0,
				escortUsesQuestGiver: savedEntry.escortUsesQuestGiver ? true : false,
				escortStage: parseInt( savedEntry.escortStage, 10 ) || 0,
				escortLastRegion: savedEntry.escortLastRegion || "",
				escortFailed: savedEntry.escortFailed ? true : false,
				lastTravelAmbushCheck: parseInt( savedEntry.lastTravelAmbushCheck, 10 ) || 0,
				selectedWaypoints: savedEntry.selectedWaypoints || [],
				selectedDestinationRegionID: parseInt( savedEntry.selectedDestinationRegionID, 10 ) || 0,
				selectedDestinationRegionName: savedEntry.selectedDestinationRegionName || "",

				skillProgress: parseFloat( savedEntry.skillProgress ) || 0,
				targetSkill: parseInt( savedEntry.targetSkill, 10 ) || -1,
				targetRegion: parseInt( savedEntry.targetRegion, 10 ) || 0,
				maxSkillPoints: parseFloat( savedEntry.maxSkillPoints ) || 50.0,
				startTime: parseInt( savedEntry.startTime, 10 ) || 0,
				timeLimit: parseInt( savedEntry.timeLimit, 10 ) || 0,

				guidedWalkNPCSerial: parseInt( savedEntry.guidedWalkNPCSerial, 10 ) || 0,
				guidedWalkUsesQuestGiver: savedEntry.guidedWalkUsesQuestGiver ? true : false,

				raceNPCSerial: parseInt( savedEntry.raceNPCSerial, 10 ) || 0,
				raceUsesQuestGiver: savedEntry.raceUsesQuestGiver ? true : false,
				raceCheckpoint: parseInt( savedEntry.raceCheckpoint, 10 ) || 0,
				raceCompleted: savedEntry.raceCompleted ? true : false,
				raceWinner: savedEntry.raceWinner || "",

				lastAccepted: parseInt( savedEntry.lastAccepted, 10 ) || 0,
				completed: savedEntry.completed ? true : false,
				questTurnIn: savedEntry.questTurnIn ? true : false,
				nextQuestID: savedEntry.nextQuestID != null ? savedEntry.nextQuestID : null
			});
		}
	}
	catch( error )
	{
		Console.Warning( "Quest system: Failed to parse " + fileName + ": " + error );
		return [];
	}

	return questProgressArray;
}

//////////////////////////////////////////////////////////////////////////////////
//                          World State (JSON-backed)                           //
//////////////////////////////////////////////////////////////////////////////////

/** @type { ( player: Character ) => string } */
function GetWorldStateKey( player )
{
	return "WorldState";
}

/** @type { ( player: Character ) => any } */
function ReadWorldState( player )
{
	if( !ValidateObject( player ) )
	{
		return {};
	}

	var playerSettings = ReadPlayerSettings( player ) || {};
	var worldStateKey = GetWorldStateKey( player );
	var worldState = playerSettings[worldStateKey];

	if( !worldState || typeof worldState != "object" )
	{
		return {};
	}

	return worldState;
}

/** @type { ( player: Character, worldState: any ) => boolean } */
function SaveWorldState( player, worldState )
{
	if( !ValidateObject( player ) )
	{
		return false;
	}

	if( !worldState || typeof worldState != "object" )
	{
		worldState = {};
	}

	var playerSettings = ReadPlayerSettings( player ) || {};
	var worldStateKey = GetWorldStateKey( player );

	playerSettings[worldStateKey] = worldState;
	return SavePlayerSettings( player, playerSettings );
}

/** @type { ( player: Character, stateKey: string, defaultValue?: number ) => number } */
function GetWorldStateInt( player, stateKey, defaultValue )
{
	var worldState = ReadWorldState( player );
	var normalizedKey = NormalizeWSKey( stateKey );

	if( normalizedKey != "" && worldState.hasOwnProperty( normalizedKey ) )
	{
		return ToIntOrZero( worldState[normalizedKey] );
	}

	return ToIntOrZero( defaultValue );
}

/** @type { ( player: Character, stateKey: string, value: number | null | undefined ) => boolean } */
function SetWorldStateInt( player, stateKey, value )
{
	var worldState = ReadWorldState( player );
	var normalizedKey = NormalizeWSKey( stateKey );

	if( normalizedKey == "" )
	{
		return false;
	}

	if( value === null || value === undefined )
	{
		if( worldState.hasOwnProperty( normalizedKey ) )
		{
			delete worldState[normalizedKey];
		}

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

	if( min != null && nextValue < min )
	{
		nextValue = min;
	}

	if( max != null && nextValue > max )
	{
		nextValue = max;
	}

	SetWorldStateInt( player, stateKey, nextValue );
	return nextValue;
}

/** @type { ( stateKey: string ) => string } */
function NormalizeWSKey( stateKey )
{
	return String( stateKey || "" ).trim().toLowerCase();
}