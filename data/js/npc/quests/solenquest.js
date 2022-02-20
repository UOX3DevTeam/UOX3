function questoffer( questGump )
{
	questGump.AddXMFHTMLGumpColor( 160, 108, 250, 20, 1054146, false, false, 10000 );//name of quest//Ambitious Solen Queen Quest
	questGump.AddXMFHTMLGumpColor( 98, 140, 312, 200, 1054060, false, true, 90000 );//quest offer
	/* <I>The Solen queen considers you eagerly for a moment then says,</I><BR><BR>
	* 
	* Yes. Yes, I think you could be of use. Normally, of course, I would handle
	* these things on my own, but these are busy times. Much to do, much to do.
	* And besides, if I am to one day become the Matriarch, then it will be good to
	* have experience trusting others to carry out various tasks for me. Yes.<BR><BR>
	* 
	* That is my plan, you see - I will become the next Matriarch. Our current
	* Matriarch is fine and all, but she won't be around forever. And when she steps
	* down, I intend to be the next in line. Ruling others is my destiny, you see.<BR><BR>
	* 
	* What I ask of you is quite simple. First, I need you to remove some of the
	* - well - competition, I suppose. Though I dare say most are hardly competent to
	* live up to such a title. I'm referring to the other queens of this colony,
	* of course. My dear sisters, so to speak. If you could remove 5 of them, I would
	* be most pleased. *sighs* By remove, I mean kill them. Don't make that face
	* at me - this is how things work in a proper society, and ours has been more proper
	* than most since the dawn of time. It's them or me, and whenever I give it
	* any thought, I'm quite sure I'd prefer it to be them.<BR><BR>
	* 
	* I also need you to gather some zoogi fungus for me - 50 should do the trick.<BR><BR>
	* 
	* Will you accept my offer?
	*/
}

function questaccept( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054061, false, true, 90000 );//quest convo
	/* <I>The Solen queen smiles as you decide to help her.</I><BR><BR>
* 
* Excellent. We'll worry about the zoogi fungus later - start by eliminating
* 5 queens from my colony.<BR><BR>That part's important, by the way; they must
* be queens from my colony. Killing queens from the other solen colony does
* little to help me become Matriarch of this colony and will not count
* toward your task.<BR><BR>
* 
* Oh, and none of those nasty infiltrator queens either. They perform a necessary
* duty, I suppose, spying on the other colony. I fail to see why that couldn't be
* left totally to the warriors, though. Nevertheless, they do not count as well.<BR><BR>
* 
* Very well. Carry on. I'll be waiting for your return.
*/
}

function questduringnpckilling( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054066, false, true, 90000 );//quest convo
					 /* <I>The Solen queen looks up as you approach.</I><BR><BR>
                * 
                * You're back, but you have not yet eliminated 5 queens from my colony.
                * Return when you have completed this task.<BR><BR>
                * 
                * Remember, by the way, that queens from the other solen colony and
                * infiltrator queens do not count toward your task.<BR><BR>
                * 
                * Very well. Carry on. I'll be waiting for your return.
                */
}

function questgathering( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054068, false, true, 90000 );//quest convo
					/* <I>The Solen queen looks pleased to see you.</I><BR><BR>
                * 
                * Splendid! You've done quite well in reducing my competition to become
                * the next Matriarch. Now I must ask that you gather some zoogi fungus for me.
                * I must practice processing it into powder of translocation.<BR><BR>
                * 
                * I believe the amount we agreed upon earlier was 50. Please return when
                * you have that amount and then give them to me.<BR><BR>
                * 
                * Farewell for now.
                */
}

function questduringgathering( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054070, false, true, 90000 );//quest convo
	/* <I>The Solen queen looks up as you approach.</I><BR><BR>
* 
* Do you have the zoogi fungus?<BR><BR>
* 
* If so, give them to me. Otherwise, go gather some and then return to me.
*/
}

function questend( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054073, false, true, 90000 );//quest convo
			 /* <I>The Solen queen smiles as she takes the zoogi fungus from you.</I><BR><BR>
                * 
                * Wonderful! I greatly appreciate your help with these tasks. My plans are beginning
                * to take shape ensuring that I will be the next Matriarch. But there is still
                * much to be done until then.<BR><BR>
                * 
                * You've done what I've asked of you and for that I thank you. Please accept this
                * bag of sending and some powder of translocation as a reward. Oh, and I suppose
                * I should give you some gold as well. Yes, yes. Of course.
                */
}

function questbusy( questGump ) 
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054059, false, true, 90000 );
	/* <I>The Solen queen considers you for a moment then says,</I><BR><BR>
                * 
                * Hmmm... I could perhaps benefit from your assistance, but you seem to be
                * busy with another task at the moment. Return to me when you complete whatever
                * it is that you're working on and maybe I can still put you to good use.
                */
}

function questobjective( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 130, 300, 100, 1054062, false, false, 19777215 );//Red Solen Queens
	//questGump.AddXMFHTMLGumpColor(70, 130, 300, 100, 1054063, false, false, 19777215);//Black Solen Queens
}

function questprogress( questGump, myPlayer )
{
	//completed
	if ( myPlayer.GetTag( "SQStatus" ) == 5 )
	{
		questGump.AddXMFHTMLGumpColor(70, 260, 270, 100, 1049077, false, false, 19777215);// completed
		questGump.AddText( 70, 280, 0x64, myPlayer.GetTag( "SQ_NUMTOKILL" ) );
		//questGump.AddXMFHTMLGumpColor(70, 260, 270, 100, 1049078, false, false, 19777215);// has not been completed
	}
	else
	{
		//questGump.AddXMFHTMLGumpColor( 70, 260, 270, 100, 1054065, false, false, 19777215 );//black queens
		questGump.AddXMFHTMLGumpColor( 70, 260, 270, 100, 1054064, false, false, 19777215 );//red queens
		questGump.AddText( 70, 280, 0x64, myPlayer.GetTag( "SQ_NUMTOKILL" ) );
		questGump.AddText( 100, 280, 0x64, "/" );
		questGump.AddText( 130, 280, 0x64, "5" );
	}
}

function onSpeech(myString, myPlayer, myNPC, pSock) 
{
	// Read Quests Log
	var myArray = TriggerEvent( 19806, "ReadQuestLog", myPlayer );
	// Next up, we want to loop through myArray, and make sure our friend is not already there
	var questslotUsed = false;
	var indexOfquestSlot = -1;
	var QuestSlot = 1;
	var questName = "Ambitious Solen Queen Quest";
	var killAmount = 5;
	var collectAmount = 50;

	if ( !myNPC.InRange( myPlayer, 2 ) )
		return;

	var Speech_Array = myString.split(" ");
	var s = 0, currObj = 0;

	for ( s = 1; s <= Speech_Array.length; s++ )
	{
		if ( Speech_Array[currObj].match( /\bQuest\b/i ) || Speech_Array[currObj].match( /\bAdventure\b/i ) || Speech_Array[currObj].match( /\bTask\b/i ) || Speech_Array[currObj].match( /\bquest\b/i ) || Speech_Array[currObj].match( /\badventure\b/i ) || Speech_Array[currObj].match( /\btask\b/i ) ) 
		{
			myNPC.TurnToward(myPlayer);
			for ( let i = 0; i < myArray.length; i++ ) 
			{
				var myQuest = myArray[i].split(",");
				if ( myQuest[4] == questName )
				{
					// Quest is already in the log! Abort
					myPlayer.SetTag("SQStatus", 6)
					TriggerEvent(19802, "convoeventgump", myPlayer, myNPC);
					return false;
				}
				else if ( myQuest[0] == QuestSlot )
				{
					questslotUsed = true;
					indexOfquestSlot = i;
				}
			}

			// Next, remove the existing entry for the selected slot from myArray!
			if ( questslotUsed ) 
			{
				myArray.splice(indexOfquestSlot, 1);
			}

			// Ok, if quest log wasn't found in the array, store quest in the questslot we selected earlier
			myArray.push(QuestSlot.toString() + "," + (myPlayer.serial).toString() + "," + myPlayer.name + "," + questName + "," + killAmount.toString() + "," + collectAmount.toString());
			if (TriggerEvent(19806, "WriteQuestLog", myPlayer, myArray))
			{
				myNPC.SetTag( "QuestSlot", QuestSlot );
				myPlayer.SetTag( "SQStatus", 1 );
				TriggerEvent( 19801, "questgump", myPlayer, myNPC );
				return false;
			}
		}
		else if (Speech_Array[currObj].match( /\bReward\b/i ) || Speech_Array[currObj].match( /\breward\b/i ) )
		{
			myNPC.TurnToward(myPlayer);
			var npcLevel = myPlayer.GetTag( "SQ_NPCLEVEL" );
			var itemLevel = myPlayer.GetTag( "SQ_LEVEL" );
			if ( npcLevel ) 
			{
				var numToKill = myPlayer.GetTag( "SQ_NUMTOKILL" );
				if ( numToKill > 0 ) 
				{
					myPlayer.SetTag( "SQStatus", 2 );
					TriggerEvent( 19802, "convoeventgump", myPlayer, myNPC );
					return false;
				}
				else 
				{
					for ( let i = 0; i < myArray.length; i++ )
					{
						{
							var myQuest = myArray[i].split(",");
							if ( myQuest[4] == questName ) 
							{
								myPlayer.SetTag( "SQStatus", 3 );
								myPlayer.SetTag( "SQ_NPCLEVEL", 0 );
								myPlayer.SetTag( "SQ_NUMTOKILL", 0 );
								TriggerEvent( 19802, "convoeventgump", myPlayer, myNPC );
								return false;
							}
						}
					}
				}
			}
			if (itemLevel) 
			{
				var numToGet = myPlayer.GetTag( "SQ_numToGet" );
				if (numToGet > 0)
				{
					myPlayer.SetTag( "SQStatus", 4 )
					TriggerEvent( 19802, "convoeventgump", myPlayer, myNPC );
					return false;
				}
			}
			else 
			{
				myNPC.TextMessage( "I do not offer rewards." );
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
	var taskLevel = pDropper.GetTag( "SQ_LEVEL" );
	if ( taskLevel )
	{
		var numZoogiFungus = pDropper.ResourceCount(0x26B7);
		if ( numZoogiFungus >= 50 )
		{
			myPlayer.SetTag( "SQStatus", 5 )
			TriggerEvent( 19802, "convoeventgump", myPlayer , pDroppedOn);
			var goldToGive = 0;
			switch ( RandomNumber( 0, 2 ) )
			{
				case 0: goldToGive = 50; break;
				case 1: goldToGive = 75; break;
				case 2: goldToGive = 100; break;
			}
			CreateDFNItem( pDropper.socket, pDropper, "0x0EED", goldToGive, "ITEM", true );
			pDropper.SoundEffect( 0x0037, false );
			decline( pDropper );
			pDropper.UseResource( 50, 0x26B7 );
			return 0;
		}
		else if ( numZoogiFungus <= 50 ) 
		{
			pDropper.SetTag( "SQStatus", 4 )
			TriggerEvent( 19802, "convoeventgump", pDropper, pDroppedOn);
			return 0;
		}
	}
	else
		pDroppedOn.TextMessage( "Our arrangement was for 50 of the zoogi fungus. Please return to me when you have that amount." );
	return 0;
}

function decline( myPlayer )
{
	myPlayer.SetTag( "SQ_numToGet", 0 );
	myPlayer.SetTag( "SQ_IDTOGET", 0 );
	myPlayer.SetTag( "SQ_LEVEL", 0 );
	myPlayer.SetTag( "SQ_NPCLEVEL", 0 );
	myPlayer.SetTag( "SQ_IDTOKILL", 0 );
	myPlayer.SetTag( "SQ_NUMTOKILL", 0 );
	myPlayer.SetTag( "SQStatus", null );
}