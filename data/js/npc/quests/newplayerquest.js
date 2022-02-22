function questOffer( questGump )
{
	questGump.AddHTMLGump( 160, 108, 250, 20, false, false, "<BASEFONT color=#ffffff>New Player Quest</BASEFONT>" );// Name of Quest
	questGump.AddHTMLGump( 98, 140, 312, 200, false, true, "<BASEFONT color=#ffffff>Hey there you look pretty new, Do you want to make a little gold and help me out with a task</BASEFONT>" );// Quest Offer
}

function questAccept( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>So i am looking for some one to kill me 5 mongbats do you think you are up to the task, if not you can say no and ill leave you alone. Oh I also need 5 iron ingots if you dont mind getting me some</BASEFONT>" );//quest Accept
}

function questDuringNpcKilling( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>what cant handle a couple mongbats get back out there you are a champion in my eyes.</BASEFONT>" );//quest convo
}

function questGathering( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>Very fine in dead now could you go get my iorn ingots we talked about, dont be lazy and i am not lazy i am just busy</BASEFONT>" );//quest convo
}

function questDuringGathering( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>Do you have my ingots if not what are you waiting for next year get to stepping young lad.</BASEFONT>" );//quest convo
}

function questEnd( questGump )
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>I knew you can do all i ask you are a great person now take you reward and move on.</BASEFONT>" );//quest convo
}

function questBusy( questGump ) 
{
	questGump.AddHTMLGump( 70, 110, 365, 220, false, true, "<BASEFONT color=#ffffff>I see you busy come back later</BASEFONT>" );
}

function questObjective( questGump )
{
	questGump.AddHTMLGump( 70, 130, 300, 100, false, false, "<BASEFONT color=#ffffff>Kill 5 Mongbats</BASEFONT>" );
}

function questProgress( questGump, myPlayer )
{
	var numToKill = myPlayer.GetTag( "NPQ_numToKill" );
	if ( numToKill >= 1 )
	{
		myPlayer.SetTag( "QuestStatus", 5 )// end
	}
	//completed
	if (myPlayer.GetTag( "QuestStatus" ) == 5 )
	{
		questGump.AddXMFHTMLGumpColor( 70, 260, 270, 100, 1049077, false, false, 19777215);// completed
		questGump.AddText( 70, 280, 0x64, myPlayer.GetTag( "NPQ_numToKill" ) );
		questGump.AddText( 100, 280, 0x64, "/" );
		questGump.AddText( 130, 280, 0x64, "5" );
	}
	else
	{
		if (numToKill < killAmount)
		{
			questGump.AddHTMLGump(70, 260, 270, 100, false, false, "<BASEFONT color=#ffffff>Mongbats left to kill</BASEFONT>");
			questGump.AddText(70, 280, 0x64, myPlayer.GetTag("NPQ_numToKill"));
			questGump.AddText(100, 280, 0x64, "/");
			questGump.AddText(130, 280, 0x64, "5");
		}
		else
		{
			questGump.AddXMFHTMLGumpColor(70, 260, 270, 100, 1054064, false, false, 19777215);//red queens
			questGump.AddText(70, 280, 0x64, myPlayer.GetTag("NPQ_numToGet"));
			questGump.AddText(100, 280, 0x64, "/");
			questGump.AddText(130, 280, 0x64, collectAmount.toString());
		}
	}
}

const questName = "New Player Quest";
const itemId = 0x1bef;
const killAmount = 5;
const collectAmount = 5;

function onSpeech( myString, myPlayer, myNPC, pSock ) 
{
	// Read Quests Log
	var myArray = TriggerEvent( 19806, "ReadQuestLog", myPlayer );
	// Next up, we want to loop through myArray, and make sure our friend is not already there
	var questslotUsed = false;
	var indexOfquestSlot = -1;
	var QuestSlot = 2;
	var Questtrg = 20001;
	var iNumToGet = "NPQ_numToGet";
	var iLevel = "NPQ_Level";
	var nNumToKill = "NPQ_numToKill";
	var nLevel = "NPQ_npcLevel";
	var iIdToGet = "NPQ_idToGet";

	if ( !myNPC.InRange( myPlayer, 2 ) )
		return;

	var Speech_Array = myString.split(" ");
	var s = 0, currObj = 0;

	for (s = 1; s <= Speech_Array.length; s++)
	{
		if ( Speech_Array[currObj].match( /\bQuest\b/i ) || Speech_Array[currObj].match( /\bAdventure\b/i ) || Speech_Array[currObj].match( /\bTask\b/i ) || Speech_Array[currObj].match( /\bquest\b/i ) || Speech_Array[currObj].match( /\badventure\b/i ) || Speech_Array[currObj].match( /\btask\b/i ) )
		{
			myNPC.TurnToward(myPlayer);
			for ( let i = 0; i < myArray.length; i++ ) 
			{
				var myQuest = myArray[i].split(",");
				if (myQuest[1] == myPlayer.serial)
				{
					// Quest is already in the log! Abort
					myPlayer.SetTag( "QuestStatus", 6 )
					TriggerEvent(19802, "convoeventgump", myPlayer);
					return false;
				}
				else if (myQuest[0] == QuestSlot)
				{
					questslotUsed = true;
					indexOfquestSlot = i;
				}
			}

			// Next, remove the existing entry for the selected slot from myArray!
			if (questslotUsed) 
			{
				myArray.splice( indexOfquestSlot, 1 );
			}

			// Ok, if quest log wasn't found in the array, store quest in the questslot we selected earlier
			myArray.push(QuestSlot.toString() + "," + (myPlayer.serial).toString() + "," + myPlayer.name + "," + questName + "," + killAmount.toString() + "," + collectAmount.toString()+ "," + Questtrg.toString() + "," + iNumToGet + "," + iLevel + "," + nNumToKill + "," + nLevel + "," + itemId.toString() + "," + iIdToGet);
			if (TriggerEvent( 19806, "WriteQuestLog", myPlayer, myArray ) )
			{
				myNPC.SetTag( "QuestSlot", QuestSlot );
				myPlayer.SetTag("QuestStatus", 1);
				TriggerEvent(19801, "questgump", myPlayer, myNPC );
				return false;
			}
		}
		else if ( Speech_Array[currObj].match( /\bReward\b/i ) || Speech_Array[currObj].match( /\breward\b/i ) )
		{
			myNPC.TurnToward( myPlayer );
			var npcLevel = myPlayer.GetTag( "NPQ_npcLevel" );
			var itemLevel = myPlayer.GetTag( "NPQ_Level" );
			if ( npcLevel )
			{
				var numToKill = myPlayer.GetTag( "NPQ_numToKill" );
				if ( numToKill > 0 )
				{
					myPlayer.SetTag( "QuestStatus", 2 )
					TriggerEvent( 19802, "convoeventgump", myPlayer, myNPC);
					return false;
				}
				else 
				{
					myPlayer.SetTag( "NPQ_npcLevel", 0 );
					myPlayer.SetTag( "NPQ_numToKill", 0 );
					myPlayer.SetTag( "QuestStatus", 3 )
					TriggerEvent( 19802, "convoeventgump", myPlayer, myNPC );
					return false;
				}
			}
			if ( itemLevel )
			{
				var numToGet = myPlayer.GetTag( "NPQ_numToGet" );
				if ( numToGet > 0 )
				{
					myPlayer.SetTag( "QuestStatus", 4 )
					TriggerEvent( 19802, "convoeventgump", myPlayer, myNPC );
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
	var taskLevel = pDropper.GetTag( "NPQ_Level" );
	if ( taskLevel )
	{
		var numIronIngots = pDropper.ResourceCount( itemId );
		if ( numIronIngots >= 5 )
		{
			myPlayer.SetTag( "QuestStatus", 5 )
			TriggerEvent( 19802, "convoeventgump", myPlayer, pDroppedOn );
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
			pDropper.UseResource( 5, itemId );
			return 0;
		}
		else if ( numIronIngots <= 5 ) 
		{
			pDropper.SetTag( "QuestStatus", 4 )
			TriggerEvent( 19802, "convoeventgump", pDropper, pDroppedOn );
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
	myPlayer.SetTag( "NPQ_Level", 0 );
	myPlayer.SetTag( "NPQ_npcLevel", 0 );
	myPlayer.SetTag( "NPQ_IDTOKILL", 0 );
	myPlayer.SetTag( "NPQ_numToKill", 0 );
	myPlayer.SetTag( "QuestStatus", null );
}