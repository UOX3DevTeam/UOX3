/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( escortNPC: Character, regionEntered: number ) => void } */
function onEnterRegion( escortNPC, regionEntered )
{
	if( !ValidateObject( escortNPC ) )
	{
		return;
	}

	if( !escortNPC.GetTag( "QuestEscort" ) )
	{
		return;
	}

	var playerSerial = parseInt( escortNPC.GetTag( "QuestPlayerSerial" ), 10 );
	var questID = parseInt( escortNPC.GetTag( "QuestID" ), 10 );
	if( isNaN( playerSerial ) || isNaN( questID ) || playerSerial <= 0 || questID <= 0 )
	{
		return;
	}

	var player = CalcCharFromSer( playerSerial );
	if( !ValidateObject( player ))
	{
		return;
	}

	TriggerEvent( 5800, "EscortReachedRegion", player, questID, regionEntered );
}

/** @type { ( escortNPC: Character, iCorpse: Item ) => boolean } */
function onDeath( escortNPC, iCorpse )
{
	if( !ValidateObject( escortNPC ) )
	{
		return false;
	}

	if( !escortNPC.GetTag( "QuestEscort" ))
	{
		return false;
	}

	var failIfDead = parseInt( escortNPC.GetTag( "EscortFailIfDead" ), 10 );
	if( isNaN( failIfDead ) || failIfDead <= 0 )
	{
		return false;
	}

	var playerSerial = parseInt( escortNPC.GetTag( "QuestPlayerSerial" ), 10 );
	var questID = parseInt( escortNPC.GetTag( "QuestID" ), 10 );
	if( isNaN( playerSerial ) || isNaN( questID ) || playerSerial <= 0 || questID <= 0 )
	{
		return false;
	}

	var player = CalcCharFromSer( playerSerial );
	if( ValidateObject( player ))
	{
		TriggerEvent( 5800, "FailEscortQuest", player, questID, "Your escort has died." );
	}

	return false;
}

/** @type { ( escortNPC: Character, timerID: number ) => void } */
function onTimer( escortNPC, timerID )
{
	if( !ValidateObject( escortNPC ))
	{
		return;
	}

	if( timerID != 1 )
	{
		return;
	}

	if( !escortNPC.GetTag( "QuestEscort" ) )
	{
		return;
	}

	var usesQuestGiver = parseInt( escortNPC.GetTag( "EscortUsesQuestGiver" ), 10 );
	if( isNaN( usesQuestGiver ) )
	{
		usesQuestGiver = 0;
	}

	var playerSerial = parseInt( escortNPC.GetTag( "QuestPlayerSerial" ), 10 );
	if( isNaN( playerSerial ) || playerSerial <= 0 )
	{
		if( !usesQuestGiver )
		{
			escortNPC.Delete();
		}
		return;
	}

	var player = CalcCharFromSer( playerSerial );
	if( !ValidateObject( player ))
	{
		if( !usesQuestGiver )
		{
			escortNPC.Delete();
		}
		return;
	}

	var maxDistance = parseInt( escortNPC.GetTag( "EscortMaxDistance" ), 10 );
	if( isNaN( maxDistance ) || maxDistance <= 0 )
	{
		maxDistance = 24;
	}

	var returnDistance = parseInt( escortNPC.GetTag( "EscortReturnDistance" ), 10 );
	if( isNaN( returnDistance ) || returnDistance <= 0 )
	{
		returnDistance = 48;
	}

	if( !escortNPC.InRange( player, maxDistance ))
	{
		var actualDistance = DistanceBetween( escortNPC, player );
		if( actualDistance <= returnDistance )
		{
			escortNPC.Follow( player );
		}
		else
		{
			var questID = parseInt( escortNPC.GetTag( "QuestID" ), 10 );
			if( !isNaN( questID ) && questID > 0 )
			{
				TriggerEvent( 5800, "FailEscortQuest", player, questID, "You have lost your escort." );
				return;
			}
		}
	}

	var questID = parseInt( escortNPC.GetTag( "QuestID" ), 10 );
	if( !isNaN( questID ) && questID > 0 )
	{
		TriggerEvent( 5800, "CheckEscortTravelAmbush", player, questID, escortNPC );
	}

	escortNPC.StartTimer( 10000, 1, true );
}