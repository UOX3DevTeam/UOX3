function questoffer( questGump )
{
	questGump.AddHTMLGump( 160, 108, 250, 20, false, false, "<BASEFONT color=#ffffff>New Player Task</BASEFONT>" );// Name of Quest
	questGump.AddHTMLGump( 98, 140, 312, 200, false, true, "<BASEFONT color=#ffffff>Hey there you look pretty new, Do you want to make a little gold and help me out with a task</BASEFONT>" );// Quest Offer
}

function questaccept( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>So i am looking for some one to kill me 5 mongbats do you think you are up to the task, if not you can say no and ill leave you alone. Oh I also need 5 iron ingots if you dont mind getting me some</BASEFONT>" );//quest Accept
}

function questduringnpckilling( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>what cant handle a couple mongbats get back out there you are a champion in my eyes.</BASEFONT>" );//quest convo
}

function questgathering( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>Very fine in dead now could you go get my iorn ingots we talked about, dont be lazy and i am not lazy i am just busy</BASEFONT>" );//quest convo
}

function questduringgathering( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>Do you have my ingots if not what are you waiting for next year get to stepping young lad.</BASEFONT>" );//quest convo
}

function questend( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>I knew you can do all i ask you are a great person now take you reward and move on.</BASEFONT>" );//quest convo
}

function questbusy( questGump ) 
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>I see you busy come back later</BASEFONT>" );
}

function questobjective( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 130, 300, 100, false, false, "<BASEFONT color=#ffffff>Kill 5 Mongbats</BASEFONT>" );
}

function questprogress( questGump, myPlayer )
{
	//completed
	if (myPlayer.GetTag( "QuestStatus" ) == "NPQ_5" )
	{
		questGump.AddXMFHTMLGumpColor( 70, 260, 270, 100, 1049077, false, false, 19777215);// completed
		questGump.AddText( 70, 280, 0x64, myPlayer.GetTag( "NPQ_NUMTOKILL" ) );
		questGump.AddText( 100, 280, 0x64, "/" );
		questGump.AddText( 130, 280, 0x64, "5" );
	}
	else
	{
		questGump.AddXMFHTMLGumpColor( 70, 260, 270, 100, false, false, "<BASEFONT color=#ffffff>Mongbats left to kill</BASEFONT>" );
		questGump.AddText( 70, 280, 0x64, myPlayer.GetTag( "NPQ_NUMTOKILL" ) );
		questGump.AddText( 100, 280, 0x64, "/" );
		questGump.AddText( 130, 280, 0x64, "5" );
	}
}

function onSpeech( myString, myPlayer, myNPC, pSock ) 
{
	if ( !myNPC.InRange( myPlayer, 2 ) )
		return;

	var questTrackString = "";
	var questNumber = 2; //Is the number of quest
	var questStatus = "2"; //changes the postion of the number
	var tempString = myPlayer.GetTag( "QuestTracker" );
	questTrackString = tempString.substr( questNumber-1, 1 ); // syntax: substr( stringPosition, length )

	var Speech_Array = myString.split(" ");
	var i = 0, currObj = 0;

	for ( i = 1; i <= Speech_Array.length; i++ ) 
	{
		if ( Speech_Array[currObj].match( /\bQuest\b/i ) || Speech_Array[currObj].match( /\bAdventure\b/i ) || Speech_Array[currObj].match( /\bTask\b/i ) || Speech_Array[currObj].match( /\bquest\b/i ) || Speech_Array[currObj].match( /\badventure\b/i ) || Speech_Array[currObj].match( /\btask\b/i ) )
		{
			myPlayer.TextMessage( "QuestTracker string: "+ questTrackString );

			myNPC.TurnToward( myPlayer );
			if ( questTrackString == "2" )
			{
				myPlayer.SetTag( "QuestStatus", "NPQ_6" )
				TriggerEvent( 19802, "convoeventgump", myPlayer );
				return false;
			}
			else
			{
				questTrackString = tempString.substr(0, questNumber - 1) + questStatus + tempString.substr(questNumber); 
				myPlayer.SetTag( "QuestTracker", questTrackString );
				TriggerEvent( 19801, "questgump", myPlayer );
				return false;
			}
		}
		else if ( Speech_Array[currObj].match(/\bReward\b/i ) || Speech_Array[currObj].match(/\breward\b/i ) )
		{
			myNPC.TurnToward( myPlayer );
			var npcLevel = myPlayer.GetTag( "NPQ_NPCLEVEL" );
			var itemLevel = myPlayer.GetTag( "NPQ_LEVEL" );
			if ( npcLevel )
			{
				var numToKill = myPlayer.GetTag( "NPQ_NUMTOKILL" );
				if ( numToKill > 0 )
				{
					myPlayer.SetTag( "QuestStatus", "NPQ_2" )
					TriggerEvent( 19802, "convoeventgump", myPlayer );
					return false;
				}
				else 
				{
					myPlayer.SetTag( "NPQ_NPCLEVEL", 0 );
					myPlayer.SetTag( "NPQ_NUMTOKILL", 0 );
					myPlayer.SetTag( "QuestStatus", "NPQ_3" )
					TriggerEvent( 19802, "convoeventgump", myPlayer );
					return false;
				}
			}
			if ( itemLevel )
			{
				var numToGet = myPlayer.GetTag( "NPQ_numToGet" );
				if ( numToGet > 0 )
				{
					myPlayer.SetTag( "QuestStatus", "NPQ_4" )
					TriggerEvent( 19802, "convoeventgump", myPlayer );
					return false;
				}
			}
			else 
			{
				myNPC.TextMessage( "I don't give rewards" );
			}
			return false;
		}
		else if ( Speech_Array[currObj].match( /\bQuite\b/i ) || Speech_Array[currObj].match( /\bquite\b/i ) )
		{
			myNPC.TurnToward( myPlayer );
			decline( myPlayer );
		}
		currObj++;
	}
}

function onDropItemOnNpc( pDropper, pDroppedOn, iDropped )
{
	pDroppedOn.TurnToward( pDropper );
	var taskLevel = pDropper.GetTag( "NPQ_LEVEL" );
	if ( taskLevel )
	{
		var numIronIngots = pDropper.ResourceCount( 0x1bef );
		if ( numIronIngots >= 5 )
		{
			myPlayer.SetTag( "QuestStatus", "NPQ_5" )
			TriggerEvent( 19802, "convoeventgump", myPlayer );
			var goldToGive = 0;
			switch (RandomNumber(0, 2))
			{
				case 0: goldToGive = 50; break;
				case 1: goldToGive = 75; break;
				case 2: goldToGive = 100; break;
			}
			CreateDFNItem( pDropper.socket, pDropper, "0x0EED", goldToGive, "ITEM", true );
			pDropper.SoundEffect( 0x0037, false );
			decline( pDropper );
			pDropper.UseResource( 5, 0x1bef );
			return 0;
		}
		else if ( numIronIngots <= 5 ) 
		{
			pDropper.SetTag( "QuestStatus", "NPQ_4" )
			TriggerEvent( 19802, "convoeventgump", pDropper );
			return 0;
		}
	}
	else
		pDroppedOn.TextMessage( "Our arrangement was for 5 of theiron ingots. Please return to me when you have that amount." );
	return 0;
}

function decline(myPlayer)
{
	myPlayer.SetTag( "NPQ_numToGet", 0 );
	myPlayer.SetTag( "NPQ_IDTOGET", 0 );
	myPlayer.SetTag( "NPQ_LEVEL", 0 );
	myPlayer.SetTag( "NPQ_NPCLEVEL", 0 );
	myPlayer.SetTag( "NPQ_IDTOKILL", 0 );
	myPlayer.SetTag( "NPQ_NUMTOKILL", 0 );
}