/// <reference path="../../../definitions.d.ts" />
// @ts-check

var questRegistry = {};
var questRegistryLoaded = false;
var questRegistryLoadError = false;

/** @type { ( questID?: number ) => object|null } */
function QuestList( questID )
{
	if( !questRegistryLoaded )
	{
		LoadQuestRegistry();
	}

	if( !questID )
	{
		return questRegistry;
	}

	return questRegistry[questID] || null;
}

/** @type { () => void } */
function ReloadQuestRegistry()
{
	questRegistry = {};
	questRegistryLoaded = false;
	questRegistryLoadError = false;
	LoadQuestRegistry();
}

/** @type { () => void } */
function LoadQuestRegistry()
{
	questRegistry = {};
	questRegistryLoaded = false;
	questRegistryLoadError = false;

	var questIndexFile = new UOXCFile();
	questIndexFile.Open( "quest_index.json", "r", "quests", true );

	if( questIndexFile == null || questIndexFile.Length() < 0 )
	{
		Console.Error( "Quest system: Unable to open js/jsdata/quests/quest_index.json" );
		questRegistryLoadError = true;
		return;
	}

	var fileText = "";
	while( !questIndexFile.EOF() )
	{
		var rawLine = questIndexFile.ReadUntil( "\n" );
		if( rawLine != null )
		{
			fileText += rawLine;
		}
	}

	questIndexFile.Close();
	questIndexFile.Free();

	fileText = SanitizeJsonText( fileText );

	if( fileText == "" )
	{
		Console.Error( "Quest system: quest_index.json is empty after sanitizing." );
		questRegistryLoadError = true;
		return;
	}

	var questIDs = null;

	try
	{
		questIDs = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Error( "Quest system: Failed to parse quest_index.json: " + error );
		questRegistryLoadError = true;
		return;
	}

	if( !questIDs || typeof questIDs != "object" || IsArrayValue( questIDs ) )
	{
		Console.Error( "Quest system: quest_index.json must be an object map of quest IDs" );
		questRegistryLoadError = true;
		return;
	}

	for( var key in questIDs )
	{
		if( !questIDs.hasOwnProperty( key ) )
		{
			continue;
		}

		if( key.indexOf( "comment" ) == 0 )
		{
			continue;
		}

		var listedQuestID = parseInt( key, 10 );

		if( isNaN( listedQuestID ) || listedQuestID <= 0 )
		{
			Console.Warning( "Quest system: Invalid quest ID in quest_index.json: " + key );
			continue;
		}

		var fileName = listedQuestID + ".json";
		var questData = LoadSingleQuestFile( fileName );

		if( !questData )
		{
			Console.Warning( "Quest system: Failed to load quest file: " + fileName );
			continue;
		}

		if( typeof questData.questID == "undefined" )
		{
			Console.Warning( "Quest system: Missing questID in file: " + fileName );
			continue;
		}

		var actualQuestID = parseInt( questData.questID, 10 );

		if( actualQuestID != listedQuestID )
		{
			Console.Warning( "Quest system: QUESTID mismatch. File " + fileName + " expected " + listedQuestID + " but found " + actualQuestID );
			continue;
		}

		if( questRegistry[actualQuestID] )
		{
			Console.Warning( "Quest system: Duplicate questID detected: " + actualQuestID + ". Existing file: " + questRegistry[actualQuestID].sourceFile + ", duplicate file: " + fileName );
			continue;
		}

		questData.sourceFile = fileName;
		questRegistry[actualQuestID] = questData;
	}

	Console.Print( "Quest system: Loaded " + Object.keys( questRegistry ).length + " quests.\n" );

	questRegistryLoaded = true;
	questRegistryLoadError = false;
}

/** @type { ( fileName: string ) => object|null } */
function LoadSingleQuestFile( fileName )
{
	var questFile = new UOXCFile();
	questFile.Open( fileName, "r", "quests", true );

	if( questFile == null || questFile.Length() < 0 )
	{
		return null;
	}

	var fileText = "";
	while( !questFile.EOF() )
	{
		var rawLine = questFile.ReadUntil( "\n" );
		if( rawLine == null || typeof( rawLine ) == "undefined" )
		{
			continue;
		}

		fileText += rawLine;
	}

	questFile.Close();
	questFile.Free();

	fileText = SanitizeJsonText( fileText );

	if( fileText == "" )
	{
		Console.Warning( "Quest system: Empty quest file: " + fileName );
		return null;
	}

	var parsedQuest = ParseQuestJsonFile( fileText, fileName );
	if( !parsedQuest )
	{
		return null;
	}

	parsedQuest.sourceFile = fileName;
	return parsedQuest;
}

/** @type { ( fileText: string, fileName: string ) => object|null } */
function ParseQuestJsonFile( fileText, fileName )
{
	var quest = null;

	try
	{
		quest = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Warning( "Quest system: JSON parse failed for file " + fileName + ". " + error );
		return null;
	}

	if( !quest || typeof quest != "object" || IsArrayValue( quest ))
	{
		Console.Warning( "Quest system: Quest file must contain one JSON object: " + fileName );
		return null;
	}

	NormalizeQuestDefaults( quest );
	ValidateQuestObject( quest, fileName );

	return quest;
}

/** @type { ( quest: object ) => void } */
function NormalizeQuestDefaults( quest )
{
	if( !quest.targetItems )
	{
		quest.targetItems = [];
	}
	if( !quest.targetItemGroups )
	{
		quest.targetItemGroups = [];
	}
	if( !quest.targetKills )
	{
		quest.targetKills = [];
	}
	if( !quest.targetKillGroups )
	{
		quest.targetKillGroups = [];
	}
	if( !quest.rewards )
	{
		quest.rewards = [];
	}
	if( !quest.npcPhrases )
	{
		quest.npcPhrases = [];
	}
	if( !quest.nextQuest )
	{
		quest.nextQuest = [];
	}
	if( !quest.escortTarget )
	{
		quest.escortTarget = null;
	}
	if( !quest.waypoints )
	{
		quest.waypoints = [];
	}
	if( !quest.randomDestinationPool )
	{
		quest.randomDestinationPool = [];
	}
	if( !quest.race )
	{
		quest.race = null;
	}
	if( !quest.targetTames )
	{
		quest.targetTames = [];
	}

	// Escort alias: lets quest writers use escortTimeLimit, but runtime still uses timeLimit
	if( typeof quest.timeLimit == "undefined" && typeof quest.escortTimeLimit != "undefined" )
	{
		quest.timeLimit = quest.escortTimeLimit;
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateQuestObject( quest, fileName )
{
	WarnUnknownTopLevelKeys( quest, fileName );
	ValidateRequiredQuestFields( quest, fileName );
	ValidateQuestFlags( quest, fileName );
	ValidateQuestObjectives( quest, fileName );
	ValidateKillTargets( quest, fileName );
	ValidateQuestRewards( quest, fileName );
	ValidateQuestTagsAndState( quest, fileName );
	ValidateQuestNextQuest( quest, fileName );
	ValidateEscortQuest( quest, fileName );
	ValidateGuidedWalkQuest( quest, fileName );
	ValidateRaceQuest( quest, fileName );
}

/** @type { ( quest: object, fileName: string ) => void } */
function WarnUnknownTopLevelKeys( quest, fileName )
{
	var validKeys = {
		"questID": true,
		"title": true,
		"description": true,
		"uncomplete": true,
		"complete": true,
		"oncomplete": true,
		"refuse": true,
		"type": true,
		"category": true,
		"oneTimeQuest": true,
		"dailyQuest": true,
		"resetDailyTime": true,
		"questTurnIn": true,
		"requiresQuestID": true,
		"nextQuestID": true,
		"timeLimit": true,
		"targetSkill": true,
		"targetRegion": true,
		"regionName": true,
		"maxSkillPoints": true,
		"minPoint": true,
		"maxPoint": true,
		"trainingarea": true,
		"ontrainingarea": true,
		"npcPhrases": true,
		"deliveryItem": true,
		"targetDelivery": true,
		"targetItems": true,
		"targetItemGroups": true,
		"targetKills": true,
		"targetKillGroups": true,
		"rewards": true,
		"nextQuest": true,
		"setTags": true,
		"setTagDeltas": true,
		"setTempTags": true,
		"setTempTagDeltas": true,
		"setWorldState": true,
		"worldStateDeltas": true,
		"deltaRules": true,
		"tempDeltaRules": true,
		"worldStateDeltaRules": true,
		"escortTarget": true,
		"waypoints": true,
		"travelAmbush": true,
		"escortTimeLimit": true,
		"randomDestinationPool": true,
		"guidedWalk": true,
		"race": true,
		"targetTames": true,
		"sourceFile": true
	};

	for( var key in quest )
	{
		if( !quest.hasOwnProperty( key ) )
		{
			continue;
		}

		if( !validKeys[key] )
		{
			Console.Warning(
				"Quest system: Unknown quest property '" +
				key +
				"' in file " +
				fileName
			);
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateRequiredQuestFields( quest, fileName )
{
	if( typeof quest.questID == "undefined" || isNaN( parseInt( quest.questID, 10 ) ) )
	{
		Console.Warning( "Quest system: Missing or invalid questID in file " + fileName );
	}

	if( !quest.title || typeof quest.title != "string" )
	{
		Console.Warning( "Quest system: Missing or invalid title in file " + fileName );
	}

	if( !quest.type || typeof quest.type != "string" )
	{
		Console.Warning( "Quest system: Missing or invalid type in file " + fileName );
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateQuestFlags( quest, fileName )
{
	if( typeof quest.oneTimeQuest != "undefined" && !IsSimpleValue( quest.oneTimeQuest ) )
	{
		Console.Warning( "Quest system: Invalid oneTimeQuest value in file " + fileName );
	}

	if( typeof quest.dailyQuest != "undefined" && !IsSimpleValue( quest.dailyQuest ) )
	{
		Console.Warning( "Quest system: Invalid dailyQuest value in file " + fileName );
	}

	if( typeof quest.questTurnIn != "undefined" && !IsSimpleValue( quest.questTurnIn ) )
	{
		Console.Warning( "Quest system: Invalid questTurnIn value in file " + fileName );
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateQuestObjectives( quest, fileName )
{
	var questType = String( quest.type || "" ).toLowerCase();

	if( quest.targetItems && !IsArrayValue( quest.targetItems ) )
	{
		Console.Warning( "Quest system: targetItems must be an array in file " + fileName );
	}

	if( quest.targetItemGroups && !IsArrayValue( quest.targetItemGroups ) )
	{
		Console.Warning( "Quest system: targetItemGroups must be an array in file " + fileName );
	}
	if( quest.targetKills && !IsArrayValue( quest.targetKills ) )
	{
		Console.Warning( "Quest system: targetKills must be an array in file " + fileName );
	}
	if( quest.targetKillGroups && !IsArrayValue( quest.targetKillGroups ) )
	{
		Console.Warning( "Quest system: targetKillGroups must be an array in file " + fileName );
	}
	if( quest.rewards && !IsArrayValue( quest.rewards ) )
	{
		Console.Warning( "Quest system: rewards must be an array in file " + fileName );
	}
	if( quest.npcPhrases && !IsArrayValue( quest.npcPhrases ) )
	{
		Console.Warning( "Quest system: npcPhrases must be an array in file " + fileName );
	}
	if( quest.nextQuest && !IsArrayValue( quest.nextQuest ) )
	{
		Console.Warning( "Quest system: nextQuest must be an array in file " + fileName );
	}

	if( questType == "collect" || questType == "timecollect" || questType == "multi" )
	{
		var hasTargetItems = ( quest.targetItems && quest.targetItems.length > 0 );
		var hasTargetItemGroups = ( quest.targetItemGroups && quest.targetItemGroups.length > 0 );

		if( !hasTargetItems && !hasTargetItemGroups )
		{
			Console.Warning( "Quest system: Quest type '" + questType + "' needs targetItems or targetItemGroups in file " + fileName );
		}
	}

	if( questType == "kill" || questType == "timekills" || questType == "multi" )
	{
		var hasTargetKills = ( quest.targetKills && quest.targetKills.length > 0 );
		var hasTargetKillGroups = ( quest.targetKillGroups && quest.targetKillGroups.length > 0 );

		if( !hasTargetKills && !hasTargetKillGroups )
		{
			Console.Warning( "Quest system: Quest type '" + questType + "' needs targetKills or targetKillGroups in file " + fileName );
		}
	}

	if( questType == "delivery" )
	{
		if( !quest.deliveryItem || typeof quest.deliveryItem != "object" )
		{
			Console.Warning( "Quest system: Delivery quest missing deliveryItem in file " + fileName );
		}

		if( !quest.targetDelivery || typeof quest.targetDelivery != "object" )
		{
			Console.Warning( "Quest system: Delivery quest missing targetDelivery in file " + fileName );
		}
	}

	if( questType == "skillgain" )
	{
		if( typeof quest.targetSkill == "undefined" )
		{
			Console.Warning( "Quest system: Skillgain quest missing targetSkill in file " + fileName );
		}
		if( typeof quest.maxSkillPoints == "undefined" )
		{
			Console.Warning( "Quest system: Skillgain quest missing maxSkillPoints in file " + fileName );
		}
	}

	if( quest.targetTames && !IsArrayValue( quest.targetTames ) )
	{
		Console.Warning( "Quest system: targetTames must be an array in file " + fileName );
	}

	if( questType == "tame" )
	{
		if( !quest.targetTames || quest.targetTames.length == 0 )
		{
			Console.Warning( "Quest system: Quest type 'tame' needs targetTames in file " + fileName );
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateKillTargets( quest, fileName )
{
	if( quest.targetKills && IsArrayValue( quest.targetKills ))
	{
		for( var targetKillIndex = 0; targetKillIndex < quest.targetKills.length; targetKillIndex++ )
		{
			var targetKill = quest.targetKills[targetKillIndex];
			if( !targetKill || typeof targetKill != "object" )
			{
				Console.Warning( "Quest system: targetKills entry at index " + targetKillIndex + " is not a valid object in file " + fileName );
				continue;
			}

			if( typeof targetKill.npcID == "undefined" )
			{
				Console.Warning( "Quest system: targetKills entry at index " + targetKillIndex + " is missing npcID in file " + fileName );
			}

			if( typeof targetKill.amount == "undefined" )
			{
				Console.Warning( "Quest system: targetKills entry at index " + targetKillIndex + " is missing amount in file " + fileName );
			}

			if( typeof targetKill.regionName != "undefined" && typeof targetKill.regionName != "string" )
			{
				Console.Warning( "Quest system: targetKills entry at index " + targetKillIndex + " has invalid regionName in file " + fileName );
			}
		}
	}

	if( quest.targetKillGroups && IsArrayValue( quest.targetKillGroups ) )
	{
		for( var targetKillGroupIndex = 0; targetKillGroupIndex < quest.targetKillGroups.length; targetKillGroupIndex++ )
		{
			var targetKillGroup = quest.targetKillGroups[targetKillGroupIndex];
			if( !targetKillGroup || typeof targetKillGroup != "object" )
			{
				Console.Warning( "Quest system: targetKillGroups entry at index " + targetKillGroupIndex + " is not a valid object in file " + fileName );
				continue;
			}

			if( typeof targetKillGroup.groupID == "undefined" )
			{
				Console.Warning( "Quest system: targetKillGroups entry at index " + targetKillGroupIndex + " is missing groupID in file " + fileName );
			}

			if( typeof targetKillGroup.amount == "undefined" )
			{
				Console.Warning( "Quest system: targetKillGroups entry at index " + targetKillGroupIndex + " is missing amount in file " + fileName );
			}

			var hasNpcList = ( targetKillGroup.npcs && IsArrayValue( targetKillGroup.npcs ) && targetKillGroup.npcs.length > 0 );

			var hasRaceID = ( typeof targetKillGroup.raceID != "undefined" );

			var hasRaceName = ( typeof targetKillGroup.raceName != "undefined" && targetKillGroup.raceName != "" );

			var hasRaceIDs = ( targetKillGroup.raceIDs && IsArrayValue( targetKillGroup.raceIDs ) && targetKillGroup.raceIDs.length > 0 );

			var hasRaceNames = ( targetKillGroup.raceNames && IsArrayValue( targetKillGroup.raceNames ) && targetKillGroup.raceNames.length > 0 );

			if( !hasNpcList && !hasRaceID && !hasRaceName && !hasRaceIDs && !hasRaceNames )
			{
				Console.Warning( "Quest system: targetKillGroups entry at index " + targetKillGroupIndex + " must define npcs, raceID, raceName, raceIDs or raceNames in file " + fileName );
			}

			if( typeof targetKillGroup.regionName != "undefined" && typeof targetKillGroup.regionName != "string" )
			{
				Console.Warning( "Quest system: targetKillGroups entry at index " + targetKillGroupIndex + " has invalid regionName in file " + fileName );
			}
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateEscortQuest( quest, fileName )
{
	if( quest.type != "escort" )
	{
		return;
	}

	if( !quest.escortTarget || typeof quest.escortTarget != "object" )
	{
		Console.Warning( "Quest system: Escort quest missing escortTarget in file: " + fileName );
		return;
	}

	if( !quest.escortTarget.npcID && !quest.escortTarget.useQuestGiver )
	{
		Console.Warning( "Quest system: Escort quest needs escortTarget.npcID or escortTarget.useQuestGiver in file: " + fileName );
	}

	var hasWaypoints = ( quest.waypoints && quest.waypoints.length > 0 );
	var hasRandomDestinationPool = ( quest.randomDestinationPool && quest.randomDestinationPool.length > 0 );

	if( !hasWaypoints && !hasRandomDestinationPool )
	{
		Console.Warning( "Quest system: Escort quest needs waypoints or randomDestinationPool in file: " + fileName );
		return;
	}

	if( typeof quest.travelAmbush != "undefined" )
	{
		if( !quest.travelAmbush || typeof quest.travelAmbush != "object" || IsArrayValue( quest.travelAmbush ) )
		{
			Console.Warning( "Quest system: Escort quest travelAmbush must be an object in file: " + fileName );
		}
		else
		{
			if( !quest.travelAmbush.npcIDs || !IsArrayValue( quest.travelAmbush.npcIDs ) || quest.travelAmbush.npcIDs.length == 0 )
			{
				Console.Warning( "Quest system: Escort quest travelAmbush missing npcIDs in file: " + fileName );
			}

			if( typeof quest.travelAmbush.despawnSeconds != "undefined" )
			{
				var travelDespawnSeconds = parseInt( quest.travelAmbush.despawnSeconds, 10 );
				if( isNaN( travelDespawnSeconds ) || travelDespawnSeconds <= 0 )
				{
					Console.Warning( "Quest system: Escort quest travelAmbush.despawnSeconds must be a positive number in file: " + fileName );
				}
			}
		}
	}

	if( hasWaypoints )
	{
		for( var waypointIndex = 0; waypointIndex < quest.waypoints.length; waypointIndex++ )
		{
			var waypoint = quest.waypoints[waypointIndex];
			if( !waypoint || typeof waypoint != "object" )
			{
				Console.Warning( "Quest system: Escort quest has invalid waypoint at index " + waypointIndex + " in file: " + fileName );
				continue;
			}

			if( typeof waypoint.regionID == "undefined" || isNaN( parseInt( waypoint.regionID, 10 ) ) )
			{
				Console.Warning( "Quest system: Escort waypoint missing regionID at index " + waypointIndex + " in file: " + fileName );
			}

			if( typeof waypoint.ambush != "undefined" )
			{
				if( !waypoint.ambush || typeof waypoint.ambush != "object" || IsArrayValue( waypoint.ambush ) )
				{
					Console.Warning( "Quest system: Escort waypoint ambush must be an object at index " + waypointIndex + " in file: " + fileName );
				}
				else
				{
					if( !waypoint.ambush.npcIDs || !IsArrayValue( waypoint.ambush.npcIDs ) || waypoint.ambush.npcIDs.length == 0 )
					{
						Console.Warning( "Quest system: Escort waypoint ambush missing npcIDs at index " + waypointIndex + " in file: " + fileName );
					}

					if( typeof waypoint.ambush.despawnSeconds != "undefined" )
					{
						var waypointDespawnSeconds = parseInt( waypoint.ambush.despawnSeconds, 10 );
						if( isNaN( waypointDespawnSeconds ) || waypointDespawnSeconds <= 0 )
						{
							Console.Warning( "Quest system: Escort waypoint ambush.despawnSeconds must be a positive number at index " + waypointIndex + " in file: " + fileName );
						}
					}
				}
			}

			if( typeof waypoint.order == "undefined" )
			{
				waypoint.order = waypointIndex + 1;
			}
		}
	}

	if( hasRandomDestinationPool )
	{
		for( var destinationIndex = 0; destinationIndex < quest.randomDestinationPool.length; destinationIndex++ )
		{
			var destinationEntry = quest.randomDestinationPool[destinationIndex];
			if( !destinationEntry || typeof destinationEntry != "object" )
			{
				Console.Warning( "Quest system: randomDestinationPool entry is invalid at index " + destinationIndex + " in file: " + fileName );
				continue;
			}

			if( typeof destinationEntry.regionID == "undefined" || isNaN( parseInt( destinationEntry.regionID, 10 ) ) )
			{
				Console.Warning( "Quest system: randomDestinationPool entry missing regionID at index " + destinationIndex + " in file: " + fileName );
			}

			if( typeof destinationEntry.weight == "undefined" )
			{
				destinationEntry.weight = 1;
			}
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateRaceQuest( quest, fileName )
{
	if( typeof quest.race == "undefined" || quest.race == null )
	{
		return;
	}

	if( !quest.race || typeof quest.race != "object" || IsArrayValue( quest.race ) )
	{
		Console.Warning( "Quest system: race must be an object in file: " + fileName );
		return;
	}

	if( typeof quest.race.enabled != "undefined" && !IsSimpleValue( quest.race.enabled ) )
	{
		Console.Warning( "Quest system: race.enabled must be a simple value in file: " + fileName );
	}

	if( typeof quest.race.finishRegion == "undefined" || isNaN( parseInt( quest.race.finishRegion, 10 ) ) )
	{
		Console.Warning( "Quest system: race.finishRegion must be set in file: " + fileName );
	}

	if( !quest.race.checkpoints || !IsArrayValue( quest.race.checkpoints ) || quest.race.checkpoints.length == 0 )
	{
		Console.Warning( "Quest system: race needs a non-empty checkpoints array in file: " + fileName );
		return;
	}

	for( var checkpointIndex = 0; checkpointIndex < quest.race.checkpoints.length; checkpointIndex++ )
	{
		var checkpoint = quest.race.checkpoints[checkpointIndex];
		if( !checkpoint || typeof checkpoint != "object" || IsArrayValue( checkpoint ) )
		{
			Console.Warning( "Quest system: race checkpoint at index " + checkpointIndex + " is invalid in file: " + fileName );
			continue;
		}

		if( typeof checkpoint.x == "undefined" || typeof checkpoint.y == "undefined" )
		{
			Console.Warning( "Quest system: race checkpoint at index " + checkpointIndex + " must define x and y in file: " + fileName );
		}
	}

	if( typeof quest.race.npcWinMode != "undefined" )
	{
		var npcWinMode = String( quest.race.npcWinMode ).toLowerCase();
		if( npcWinMode != "continue" && npcWinMode != "turnin" && npcWinMode != "fail" )
		{
			Console.Warning( "Quest system: race.npcWinMode must be continue, turnin or fail in file: " + fileName );
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateGuidedWalkQuest( quest, fileName )
{
	if( typeof quest.guidedWalk == "undefined" )
	{
		return;
	}

	if( !quest.guidedWalk || typeof quest.guidedWalk != "object" || IsArrayValue( quest.guidedWalk ) )
	{
		Console.Warning( "Quest system: guidedWalk must be an object in file: " + fileName );
		return;
	}

	if( typeof quest.guidedWalk.enabled != "undefined" && !IsSimpleValue( quest.guidedWalk.enabled ) )
	{
		Console.Warning( "Quest system: guidedWalk.enabled must be a simple value in file: " + fileName );
	}

	if( !quest.guidedWalk.steps || !IsArrayValue( quest.guidedWalk.steps ) || quest.guidedWalk.steps.length == 0 )
	{
		Console.Warning( "Quest system: guidedWalk needs a non-empty steps array in file: " + fileName );
		return;
	}

	for( var stepIndex = 0; stepIndex < quest.guidedWalk.steps.length; stepIndex++ )
	{
		var step = quest.guidedWalk.steps[stepIndex];
		if( !step || typeof step != "object" || IsArrayValue( step ) )
		{
			Console.Warning( "Quest system: guidedWalk step at index " + stepIndex + " is invalid in file: " + fileName );
			continue;
		}

		if( typeof step.x == "undefined" || typeof step.y == "undefined" )
		{
			Console.Warning( "Quest system: guidedWalk step at index " + stepIndex + " must define x and y in file: " + fileName );
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateQuestRewards( quest, fileName )
{
	if( !quest.rewards || !IsArrayValue( quest.rewards ) )
	{
		return;
	}

	for( var i = 0; i < quest.rewards.length; i++ )
	{
		var reward = quest.rewards[i];
		if( !reward || typeof reward != "object" )
		{
			Console.Warning( "Quest system: Reward entry at index " + i + " is not a valid object in file " + fileName );
			continue;
		}

		if( !reward.type )
		{
			Console.Warning( "Quest system: Reward entry at index " + i + " is missing type in file " + fileName );
			continue;
		}

		switch( String( reward.type ).toLowerCase() )
		{
			case "gold":
			case "fame":
			case "karma":
			case "skillpoints":
				if( typeof reward.amount == "undefined" )
				{
					Console.Warning( "Quest system: Reward type '" + reward.type + "' missing amount at index " + i + " in file " + fileName );
				}
				break;

			case "item":
				if( !reward.sectionID )
				{
					Console.Warning( "Quest system: Item reward missing sectionID at index " + i + " in file " + fileName );
				}
				if( typeof reward.amount == "undefined" )
				{
					Console.Warning( "Quest system: Item reward missing amount at index " + i + " in file " + fileName );
				}
				break;

			case "skill":
				if( typeof reward.skill == "undefined" || typeof reward.amount == "undefined" )
				{
					Console.Warning( "Quest system: Skill reward missing skill or amount at index " + i + " in file " + fileName );
				}
				break;

			case "virtue":
				if( typeof reward.virtueIndex == "undefined" || typeof reward.amount == "undefined" )
				{
					Console.Warning( "Quest system: Virtue reward missing virtueIndex or amount at index " + i + " in file " + fileName );
				}
				break;

			default:
				Console.Warning( "Quest system: Unknown reward type '" + reward.type + "' at index " + i + " in file " + fileName );
				break;
		}
	}
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateQuestTagsAndState( quest, fileName )
{
	ValidatePlainObjectMap( quest.setTags, "setTags", fileName );
	ValidatePlainObjectMap( quest.setTagDeltas, "setTagDeltas", fileName );
	ValidatePlainObjectMap( quest.setTempTags, "setTempTags", fileName );
	ValidatePlainObjectMap( quest.setTempTagDeltas, "setTempTagDeltas", fileName );
	ValidatePlainObjectMap( quest.setWorldState, "setWorldState", fileName );
	ValidatePlainObjectMap( quest.worldStateDeltas, "worldStateDeltas", fileName );

	ValidateRuleMap( quest.deltaRules, "deltaRules", fileName );
	ValidateRuleMap( quest.tempDeltaRules, "tempDeltaRules", fileName );
	ValidateRuleMap( quest.worldStateDeltaRules, "worldStateDeltaRules", fileName );
}

/** @type { ( quest: object, fileName: string ) => void } */
function ValidateQuestNextQuest( quest, fileName )
{
	if( !quest.nextQuest || !IsArrayValue( quest.nextQuest ))
	{
		return;
	}

	for( var i = 0; i < quest.nextQuest.length; i++ )
	{
		var nextEntry = quest.nextQuest[i];
		if( !nextEntry || typeof nextEntry != "object" )
		{
			Console.Warning( "Quest system: nextQuest entry at index " + i + " is not a valid object in file " + fileName );
			continue;
		}

		if( typeof nextEntry.questID == "undefined" )
		{
			Console.Warning( "Quest system: nextQuest entry at index " + i + " is missing questID in file " + fileName );
		}

		if( typeof nextEntry.cond != "undefined" )
		{
			ValidateNextQuestConditionObject( nextEntry.cond, fileName, i );
		}
	}
}

/** @type { ( cond: object, fileName: string, index: number ) => void } */
function ValidateNextQuestConditionObject( cond, fileName, index )
{
	if( !cond || typeof cond != "object" )
	{
		Console.Warning( "Quest system: nextQuest cond at index " + index + " is not a valid object in file " + fileName );
		return;
	}

	var validCondKeys = {
		"always": true,
		"completedQuest": true,
		"notCompletedQuest": true,
		"hasQuest": true,
		"notHasQuest": true,
		"tagEquals": true,
		"tagMin": true,
		"worldStateEquals": true,
		"worldStateMin": true
	};

	for( var key in cond )
	{
		if( cond.hasOwnProperty( key ) && !validCondKeys[key] )
		{
			Console.Warning( "Quest system: Unknown nextQuest condition property '" + key + "' in file " + fileName + " at nextQuest index " + index );
		}
	}
}

/** @type { ( value: any, objectName: string, fileName: string ) => void } */
function ValidatePlainObjectMap( value, objectName, fileName )
{
	if( typeof value == "undefined" )
	{
		return;
	}

	if( !value || typeof value != "object" || IsArrayValue( value ))
	{
		Console.Warning( "Quest system: " + objectName + " must be a plain object in file " + fileName );
	}
}

/** @type { ( value: any, objectName: string, fileName: string ) => void } */
function ValidateRuleMap( value, objectName, fileName )
{
	if( typeof value == "undefined" )
	{
		return;
	}

	if( !value || typeof value != "object" || IsArrayValue( value ))
	{
		Console.Warning( "Quest system: " + objectName + " must be a plain object in file " + fileName );
		return;
	}

	for( var key in value )
	{
		if( !value.hasOwnProperty( key ) )
		{
			continue;
		}

		var rule = value[key];
		if( !rule || typeof rule != "object" || IsArrayValue( rule ))
		{
			Console.Warning( "Quest system: Rule '" + key + "' in " + objectName + " must be an object in file " + fileName );
		}
	}
}

/** @type { ( value: any ) => boolean } */
function IsArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( value: any ) => boolean } */
function IsSimpleValue( value )
{
	var valueType = typeof value;
	return valueType == "string" || valueType == "number" || valueType == "boolean";
}

/** @type { ( text: string ) => string } */
function TrimString( text )
{
	if( text == null || typeof( text ) == "undefined" )
	{
		return "";
	}

	return text.replace( /^\s+|\s+$/g, "" );
}

/** @type { ( text: string ) => string } */
function SanitizeJsonText( text )
{
	if( text == null || typeof( text ) == "undefined" )
	{
		return "";
	}

	text = String( text );

	if( text.length > 0 && text.charCodeAt( 0 ) == 65279 )
	{
		text = text.substring( 1 );
	}

	text = text.split( "\r\n" ).join( "\n" );
	text = text.split( "\r" ).join( "\n" );

	// Remove/fix hidden characters that break SpiderMonkey JSON.parse
	text = text.split( String.fromCharCode( 160 ) ).join( " " );
	text = text.split( String.fromCharCode( 255 ) ).join( "" );

	// Important: raw tabs inside JSON strings break JSON.parse.
	// Replacing tabs with spaces is safe for indentation too.
	text = text.split( "\t" ).join( " " );

	text = TrimString( text );

	var lastBrace = text.lastIndexOf( "}" );
	if( lastBrace >= 0 )
	{
		text = text.substring( 0, lastBrace + 1 );
	}

	return TrimString( text );
}