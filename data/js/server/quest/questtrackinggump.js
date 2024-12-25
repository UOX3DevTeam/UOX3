function QuestTrackingGump( pUser )
{
	var socket = pUser.socket;
	if( !ValidateObject( pUser ))
	{
		return;
	}

	// Load tracked quests and other necessary data
	var trackedQuests = TriggerEvent( 5802, "ReadTrackedQuests", pUser );
	var activeQuests = TriggerEvent( 5800, "loadAllQuests", pUser );
	var chainQuests = TriggerEvent( 5801, "getQuests" );

	var gump = new Gump();
	gump.NoClose();
	gump.AddPage( 0 );
	gump.AddBackground( 30, 120, 250, 425, 1579 );
	gump.AddGump( 50, 100, 1588 );
	if( TriggerEvent( 5800, "couGumpSupport" )) 
	{
		pUser.StartTimer( 1500, 1, 5803 );
		gump.AddHTMLGump( 65, 102, 200, 50, false, false, "<H4><basefont color=#59ff00>Tracked Quests</basefont></H4>" );
		gump.AddCheckerTrans( 30, 120, 250, 425 );
	}
	else
	{
		pUser.StartTimer( 2000, 1, 5803 );
		gump.AddHTMLGump( 65, 102, 200, 50, false, false, "<H4>Tracked Quests</H4>" );
	}

	var yOffset = 150;

	// Iterate over tracked quests, displaying relevant information
	for( var i = 0; i < trackedQuests.length; i++ )
	{
		// Only include quests that are actively tracked ( status === 1 )
		if( trackedQuests[i].status !== 1 )
			continue;

		var questID = trackedQuests[i].questID;
		var quest = null;
		var progress = null;

		// Find the quest in chainQuests
		for( var j = 0; j < chainQuests.length; j++ )
		{
			if( chainQuests[j].id === questID )
			{
				quest = chainQuests[j];
				break;
			}
		}

		// Find progress for the quest
		for( var k = 0; k < activeQuests.length; k++ )
		{
			if( activeQuests[k].questID === questID )
			{
				progress = activeQuests[k];
				break;
			}
		}

		if( !quest || !progress )
			continue;

		// Build objectives string
		var objectives = "";
		for( var o = 0; o < quest.steps[progress.step].objectives.length; o++ )
		{
			var obj = quest.steps[progress.step].objectives[o];

			// Handle kill objectives
			if( obj.type === "kill" && obj.targets && obj.targets.length )
			{
				objectives += "Kill Targets:<br>";
				for( var t = 0; t < obj.targets.length; t++ )
				{
					var target = obj.targets[t];
					var targetProgress = ( progress.progress[o] && progress.progress[o][t] ) || 0;
					objectives += "- " + ( target.name || "Unknown Target" ) + ": " + targetProgress + "/" + target.count + "<br>";
				}
			}
			else
			{
				// General objectives
				objectives += obj.type + ": " + ( progress.progress[o] || 0 ) + "/" + obj.count + "<br>";
			}
		}

		// Add quest details to the gump
		if( TriggerEvent( 5800, "couGumpSupport" ))
		{
			// Add quest details to the gump
			gump.AddHTMLGump( 50, yOffset, 250, 120, false, false,
				"<H2><basefont color=#ff8b00>" + quest.name + "</basefont></H2><br>" +
				"<basefont color=#2eff00>" + objectives + "</basefont>" );
		}
		else 
		{
			gump.AddHTMLGump( 50, yOffset, 250, 120, false, false, quest.name + "<br>" + objectives );
		}

		yOffset += 150; // Adjust for next quest
	}

	// Add a "Back" button to return to the main quest gump
	gump.AddButton( 70, 490, 1144, 1145, 1, 0, 1 ); // Back button
	gump.Send( socket );
	gump.Free();
}

function onGumpPress(  pSock, pButton, gumpData  ) 
{
	var pUser = pSock.currentChar;
	// Kill any timers associated with the tracked quests
	if(  pButton == 1  ) 
	{
		pUser.KillJSTimer( 1, 5803 );
	}
}

function onTimer(  pUser, timerID  )
{
	var socket = pUser.socket;
	if(  timerID == 1  )
	{
		if( ValidateObject( pUser )  && socket != null  )
		{
			var gumpID = 5803 + 0xffff;
			socket.CloseGump( gumpID, 0 );
			QuestTrackingGump(  pUser  );
			return;
		}
		else
		{
			pUser.KillJSTimer( 1, 5803 );
			return;
		}
	}
}
