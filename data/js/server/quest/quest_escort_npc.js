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

	if( isNaN( playerSerial ) || playerSerial <= 0 || isNaN( questID ) || questID <= 0 )
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

	var playerSerial = parseInt( escortNPC.GetTag( "QuestPlayerSerial" ), 10 );
	var questID = parseInt( escortNPC.GetTag( "QuestID" ), 10 );

	if( isNaN( playerSerial ) || playerSerial <= 0 || isNaN( questID ) || questID <= 0 )
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

	var usesQuestGiver = parseInt( escortNPC.GetTag( "EscortUsesQuestGiver" ), 10 );
	var playerSerial = parseInt( escortNPC.GetTag( "QuestPlayerSerial" ), 10 );
	var questID = parseInt( escortNPC.GetTag( "QuestID" ), 10 );

	if( isNaN( usesQuestGiver ))
	{
		usesQuestGiver = 0;
	}

	if( isNaN( playerSerial ) || playerSerial <= 0 )
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

	if( !escortNPC.InRange( player, maxDistance ) )
	{
		var actualDistance = DistanceBetween( escortNPC, player );

		if( actualDistance <= returnDistance )
		{
			escortNPC.SetTag( "EscortLostGraceCount", null );
			escortNPC.Follow( player );
		}
		else
		{
			if( ValidateObject( escortNPC.attacker ) || ValidateObject( player.attacker ) )
			{
				escortNPC.SetTag( "EscortLostGraceCount", null );
				escortNPC.StartTimer( 10000, 1, 5814 );
				return;
			}

			var lostGraceCount = parseInt( escortNPC.GetTag( "EscortLostGraceCount" ), 10 );
			if( isNaN( lostGraceCount ))
			{
				lostGraceCount = 0;
			}

			lostGraceCount++;
			escortNPC.SetTag( "EscortLostGraceCount", lostGraceCount );

			if( lostGraceCount >= 3 && !isNaN( questID ) && questID > 0 )
			{
				TriggerEvent( 5800, "FailEscortQuest", player, questID, "You have lost your escort." );
				return;
			}
		}
	}
	else
	{
		escortNPC.SetTag( "EscortLostGraceCount", null );
	}

	if( !isNaN( questID ) && questID > 0 )
	{
		TriggerEvent( 5800, "CheckEscortTravelAmbush", player, questID, escortNPC );
	}

	escortNPC.StartTimer( 10000, 1, 5814 );
}