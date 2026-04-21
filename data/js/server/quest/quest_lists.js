/// <reference path="../../../definitions.d.ts" />
// @ts-check

var questRegistry = {};
var questRegistryLoaded = false;
var questRegistryLoadError = false;

function QuestList( questID )
{
	if( !questRegistryLoaded && !questRegistryLoadError )
	{
		LoadQuestRegistry();
	}

	if( !questID )
	{
		return questRegistry;
	}

	return questRegistry[questID] || null;
}

function ReloadQuestRegistry()
{
	questRegistry = {};
	questRegistryLoaded = false;
	questRegistryLoadError = false;
	LoadQuestRegistry();
	return questRegistryLoaded;
}

function LoadQuestRegistry()
{
	questRegistry = {};

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
			if( !questIndexFile.EOF() )
			{
				fileText += "\n";
			}
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
			Console.Warning(
				"Quest system: QUESTID mismatch. File " +
				fileName +
				" expected " +
				listedQuestID +
				" but found " +
				actualQuestID
			);
			continue;
		}

		if( questRegistry[actualQuestID] )
		{
			Console.Warning(
				"Quest system: Duplicate questID detected: " +
				actualQuestID +
				". Existing file: " + questRegistry[actualQuestID].sourceFile +
				", duplicate file: " + fileName
			);
			continue;
		}

		questData.sourceFile = fileName;
		questRegistry[actualQuestID] = questData;
	}

	Console.Print(
		"Quest system: Loaded " +
		Object.keys( questRegistry ).length +
		" quests."
	);

	questRegistryLoaded = true;
}

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
		if( !questFile.EOF() )
		{
			fileText += "\n";
		}
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

function ParseQuestJsonFile( fileText, fileName )
{
	var quest = null;

	try
	{
		quest = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Warning(
			"Quest system: JSON parse failed for file " +
			fileName +
			". " +
			error
		);
		return null;
	}

	if( !quest || typeof quest != "object" || IsArrayValue( quest ) )
	{
		Console.Warning(
			"Quest system: Quest file must contain one JSON object: " + fileName
		);
		return null;
	}

	NormalizeQuestDefaults( quest );
	ValidateQuestObject( quest, fileName );

	return quest;
}

function NormalizeQuestDefaults( quest )
{
	if( !quest.targetItems )
	{
		quest.targetItems = [];
	}
	if( !quest.targetKills )
	{
		quest.targetKills = [];
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
}

function ValidateQuestObject( quest, fileName )
{
	WarnUnknownTopLevelKeys( quest, fileName );
	ValidateRequiredQuestFields( quest, fileName );
	ValidateQuestFlags( quest, fileName );
	ValidateQuestObjectives( quest, fileName );
	ValidateQuestRewards( quest, fileName );
	ValidateQuestTagsAndState( quest, fileName );
	ValidateQuestNextQuest( quest, fileName );
}

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
		"targetKills": true,
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

function ValidateQuestObjectives( quest, fileName )
{
	var questType = String( quest.type || "" ).toLowerCase();

	if( quest.targetItems && !IsArrayValue( quest.targetItems ) )
	{
		Console.Warning( "Quest system: targetItems must be an array in file " + fileName );
	}
	if( quest.targetKills && !IsArrayValue( quest.targetKills ) )
	{
		Console.Warning( "Quest system: targetKills must be an array in file " + fileName );
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

	if(( questType == "collect" || questType == "timecollect" || questType == "multi" ) && ( !quest.targetItems || quest.targetItems.length == 0 ))
	{
		Console.Warning( "Quest system: Quest type '" + questType + "' needs targetItems in file " + fileName );
	}

	if(( questType == "kill" || questType == "timekills" || questType == "multi" ) && ( !quest.targetKills || quest.targetKills.length == 0 ))
	{
		Console.Warning( "Quest system: Quest type '" + questType + "' needs targetKills in file " + fileName );
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
}

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
			Console.Warning(
				"Quest system: Reward entry at index " +
				i +
				" is not a valid object in file " +
				fileName
			);
			continue;
		}

		if( !reward.type )
		{
			Console.Warning(
				"Quest system: Reward entry at index " +
				i +
				" is missing type in file " +
				fileName
			);
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
					Console.Warning(
						"Quest system: Reward type '" +
						reward.type +
						"' missing amount at index " +
						i +
						" in file " +
						fileName
					);
				}
				break;

			case "item":
				if( !reward.sectionID )
				{
					Console.Warning(
						"Quest system: Item reward missing sectionID at index " +
						i +
						" in file " +
						fileName
					);
				}
				if( typeof reward.amount == "undefined" )
				{
					Console.Warning(
						"Quest system: Item reward missing amount at index " +
						i +
						" in file " +
						fileName
					);
				}
				break;

			case "skill":
				if( typeof reward.skill == "undefined" || typeof reward.amount == "undefined" )
				{
					Console.Warning(
						"Quest system: Skill reward missing skill or amount at index " +
						i +
						" in file " +
						fileName
					);
				}
				break;

			case "virtue":
				if( typeof reward.virtueIndex == "undefined" || typeof reward.amount == "undefined" )
				{
					Console.Warning(
						"Quest system: Virtue reward missing virtueIndex or amount at index " +
						i +
						" in file " +
						fileName
					);
				}
				break;

			default:
				Console.Warning(
					"Quest system: Unknown reward type '" +
					reward.type +
					"' at index " +
					i +
					" in file " +
					fileName
				);
				break;
		}
	}
}

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

function ValidateQuestNextQuest( quest, fileName )
{
	if( !quest.nextQuest || !IsArrayValue( quest.nextQuest ) )
	{
		return;
	}

	for( var i = 0; i < quest.nextQuest.length; i++ )
	{
		var nextEntry = quest.nextQuest[i];
		if( !nextEntry || typeof nextEntry != "object" )
		{
			Console.Warning(
				"Quest system: nextQuest entry at index " +
				i +
				" is not a valid object in file " +
				fileName
			);
			continue;
		}

		if( typeof nextEntry.questID == "undefined" )
		{
			Console.Warning(
				"Quest system: nextQuest entry at index " +
				i +
				" is missing questID in file " +
				fileName
			);
		}

		if( typeof nextEntry.cond != "undefined" )
		{
			ValidateNextQuestConditionObject( nextEntry.cond, fileName, i );
		}
	}
}

function ValidateNextQuestConditionObject( cond, fileName, index )
{
	if( !cond || typeof cond != "object" )
	{
		Console.Warning(
			"Quest system: nextQuest cond at index " +
			index +
			" is not a valid object in file " +
			fileName
		);
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
			Console.Warning(
				"Quest system: Unknown nextQuest condition property '" +
				key +
				"' in file " +
				fileName +
				" at nextQuest index " +
				index
			);
		}
	}
}

function ValidatePlainObjectMap( value, objectName, fileName )
{
	if( typeof value == "undefined" )
	{
		return;
	}

	if( !value || typeof value != "object" || IsArrayValue( value ) )
	{
		Console.Warning(
			"Quest system: " +
			objectName +
			" must be a plain object in file " +
			fileName
		);
	}
}

function ValidateRuleMap( value, objectName, fileName )
{
	if( typeof value == "undefined" )
	{
		return;
	}

	if( !value || typeof value != "object" || IsArrayValue( value ) )
	{
		Console.Warning(
			"Quest system: " +
			objectName +
			" must be a plain object in file " +
			fileName
		);
		return;
	}

	for( var key in value )
	{
		if( !value.hasOwnProperty( key ) )
		{
			continue;
		}

		var rule = value[key];
		if( !rule || typeof rule != "object" || IsArrayValue( rule ) )
		{
			Console.Warning(
				"Quest system: Rule '" +
				key +
				"' in " +
				objectName +
				" must be an object in file " +
				fileName
			);
		}
	}
}

function IsArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

function IsSimpleValue( value )
{
	var valueType = typeof value;
	return valueType == "string" || valueType == "number" || valueType == "boolean";
}

function TrimString( text )
{
	if( text == null || typeof( text ) == "undefined" )
	{
		return "";
	}

	return text.replace( /^\s+|\s+$/g, "" );
}

function SanitizeJsonText( text )
{
	if( text == null || typeof( text ) == "undefined" )
	{
		return "";
	}

	text = String( text );

	// Strip UTF-8 BOM if present
	if( text.length > 0 && text.charCodeAt( 0 ) == 0xFEFF )
	{
		text = text.substring( 1 );
	}

	// Strip other non-printable control characters except tab/newline/carriage return
	text = text.replace( /[^\x09\x0A\x0D\x20-\x7E]/g, "" );

	return TrimString( text );
}