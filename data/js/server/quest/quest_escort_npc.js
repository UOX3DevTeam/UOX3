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

	var playerSerial = escortNPC.GetTag( "QuestPlayerSerial" );
	var questID = escortNPC.GetTag( "QuestID" );

	if( playerSerial == 0 || questID == 0 )
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

	if( !escortNPC.GetTag( "QuestEscort" ) )
	{
		return false;
	}

	var playerSerial = escortNPC.GetTag( "QuestPlayerSerial" );
	var questID = escortNPC.GetTag( "QuestID" );

	if( playerSerial == 0 || questID == 0 )
	{
		return false;
	}

	var player = CalcCharFromSer( playerSerial );
	if( ValidateObject( player ) )
	{
		TriggerEvent( 5800, "FailEscortQuest", player, questID, "Your escort has died." );
	}

	return false;
}

/** @type { ( escortNPC: Character, timerID: number ) => void } */
function onTimer( escortNPC, timerID )
{
	if( !ValidateObject( escortNPC ) || timerID != 1 )
	{
		return;
	}

	if( !escortNPC.GetTag( "QuestEscort" ) )
	{
		return;
	}

	var usesQuestGiver = escortNPC.GetTag( "EscortUsesQuestGiver" );
	var playerSerial = escortNPC.GetTag( "QuestPlayerSerial" );
	var questID = escortNPC.GetTag( "QuestID" );

	if( playerSerial == 0 )
	{
		if( !usesQuestGiver )
		{
			escortNPC.Delete();
		}
		return;
	}

	var player = CalcCharFromSer( playerSerial );
	if( !ValidateObject( player ) )
	{
		if( !usesQuestGiver )
		{
			escortNPC.Delete();
		}
		return;
	}

	var maxDistance = escortNPC.GetTag( "EscortMaxDistance" );
	if( maxDistance == 0 )
	{
		maxDistance = 24;
	}

	var returnDistance = escortNPC.GetTag( "EscortReturnDistance" );
	if( returnDistance == 0 )
	{
		returnDistance = 48;
	}

	if( !escortNPC.InRange( player, maxDistance ) )
	{
		var actualDistance = DistanceBetween( escortNPC, player );
		if( actualDistance <= returnDistance )
		{
			escortNPC.Follow( player );
		}
		else if( questID != 0 )
		{
			TriggerEvent( 5800, "FailEscortQuest", player, questID, "You have lost your escort." );
			return;
		}
	}

	if( questID != 0 )
	{
		TriggerEvent( 5800, "CheckEscortTravelAmbush", player, questID, escortNPC );
	}

	escortNPC.StartTimer( 10000, 1, 5814 );
}