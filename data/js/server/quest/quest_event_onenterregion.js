/// <reference path="../../../definitions.d.ts" />
// @ts-check

function onEnterRegion( pEntering, regionEntered )
{
	var pSock = pEntering.socket;
	if( pSock == null )
	{
		return;
	}

	TriggerEvent( 5800, "RacePlayerReachedRegion", pEntering, regionEntered );

	var activeQuests = TriggerEvent( 5800, "ReadQuestProgress", pEntering );

	if( !activeQuests || activeQuests.length == 0 )
	{
		return;
	}

	for( var i = 0; i < activeQuests.length; i++ )
	{
		var questEntry = activeQuests[i];

		if( questEntry.serial != pEntering.serial )
		{
			continue;
		}

		var quest = TriggerEvent( 5801, "QuestList", questEntry.questID );
		if( !quest )
		{
			continue;
		}

		if( quest.type == "skillgain" )
		{
			if( quest.trainingarea ) 
			{
				pSock.SysMessage( quest.ontrainingarea );
			}
			return;
		}
	}
}
