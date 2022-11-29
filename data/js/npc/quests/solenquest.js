function questOffer( questGump )
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

function questAccept( questGump )
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

function questDuringNpcKilling( questGump )
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

function questGathering( questGump )
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

function questDuringGathering( questGump )
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054070, false, true, 90000 );//quest convo
	/* <I>The Solen queen looks up as you approach.</I><BR><BR>
* 
* Do you have the zoogi fungus?<BR><BR>
* 
* If so, give them to me. Otherwise, go gather some and then return to me.
*/
}

function questEnd( questGump )
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

function questBusy( questGump ) 
{
	questGump.AddXMFHTMLGumpColor( 70, 110, 365, 220, 1054059, false, true, 90000 );
	/* <I>The Solen queen considers you for a moment then says,</I><BR><BR>
                * 
                * Hmmm... I could perhaps benefit from your assistance, but you seem to be
                * busy with another task at the moment. Return to me when you complete whatever
                * it is that you're working on and maybe I can still put you to good use.
                */
}

function questObjective( questGump )
{
	questGump.AddText( 70, 130, 0x64, "Kill " + killAmount + " Red Solen Queens" );
	questGump.AddText( 70, 150, 0x64, "Collect " + collectAmount + " Zoogi Fungus" );
}

function questProgress( questGump, myPlayer )
{
	var numToKill = myPlayer.GetTag( "SQ_numToKill" );
	var numToGet = myPlayer.GetTag( "SQ_numToGet" )
	var questStatus = myPlayer.GetTag( "SQ_Status" )

	//completed
	if( questStatus == 6 )
	{
		questGump.AddXMFHTMLGumpColor( 70, 260, 270, 100, 1049077, false, false, 19777215 );// completed
	}
	else
	{
			questGump.AddText( 70, 260,  0x64, "Npcs left to Kill: "  + numToKill );
			questGump.AddText( 70, 280, 0x64, "Items left to Collect: " + numToGet );
	}
}

const questName = "Ambitious Solen Queen Quest";
const itemId = 0x26B7;
const npcId = 0x0327;
const collectAmount = 50;
const killAmount = 5;

function onCharDoubleClick( myPlayer, myNPC )
{
	// Read Quests Log
	var myArray = TriggerEvent( 19806, "ReadQuestLog", myPlayer );

	// Next up, we want to loop through myArray, and make sure our friend is not already there
	var questslotUsed = false;
	var indexOfquestSlot = -1;
	var questSlot = 1;
	var questTrg = 20000;
	var iNumToGet = "SQ_numToGet";
	var iLevel = "SQ_level";
	var nNumToKill = "SQ_numToKill";
	var nLevel = "SQ_npcLevel";
	var iIdToGet = "SQ_idToGet";
	var iIdToKill = "SQ_idToKill";
	var questStatus = "SQ_Status";

	myNPC.TurnToward( myPlayer );
	for( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var playerSerial = myQuestData[1];
		var questStatusData = myQuestData[15];
		if ( myNPC.GetTag( "Completed_" + playerSerial ) == 1 )
		{
			myNPC.TextMessage( "You have already completed this quest." );
			return false;
		}

		if( myNPC.GetTag( "Declined_" + playerSerial ) == 0 || myPlayer.GetTag( questStatusData ) >= 1 )
		{
			// Quest is already in the log! Abort
			myPlayer.SetTag(questStatus, 8 )
			TriggerEvent(19802, "convoeventgump", myPlayer, myNPC );
			return false;
		}
		else if( myQuestData[0] == questSlot )
		{
			questslotUsed = true;
			indexOfquestSlot = i;
		}
	}
	// Next, remove the existing entry for the selected slot from myArray!
	if( questslotUsed == true )
	{
		myArray.splice( indexOfquestSlot, 2 );
	}

	// Ok, if quest log wasn't found in the array, store quest in the questslot we selected earlier
	//                 0                                 1                             2                      3                  4                                5                                 6                   7                8                9                10               11                         12                  13                   14              15
	myArray.push( questSlot.toString() + "," + ( myPlayer.serial ).toString() + "," + myPlayer.name + "," + questName + "," + killAmount.toString() + "," + collectAmount.toString() + "," + questTrg.toString() + "," + iNumToGet + "," + iLevel + "," + nNumToKill + "," + nLevel + "," + itemId.toString() + "," + iIdToGet + "," + npcId.toString() + "," + iIdToKill + "," + questStatus );
	if( TriggerEvent( 19806, "WriteQuestLog", myPlayer, myArray ))
	{
		myNPC.SetTag( "QuestSlot", questSlot );
		myPlayer.SetTag( questStatus, 1 );
		TriggerEvent( 19801, "questGump", myPlayer, myNPC );
		return false;
	}
}

function onSpeech( myString, myPlayer, myNPC, pSock ) 
{
	// Read Quests Log
	var myArray = TriggerEvent( 19806, "ReadQuestLog", myPlayer );

	// Next up, we want to loop through myArray, and make sure our quest is not already there
	var questslotUsed = false;
	var indexOfquestSlot = -1;
	var questSlot = 1;
	var questTrg = 20000;
	var iNumToGet = "SQ_numToGet";
	var iLevel = "SQ_level";
	var nNumToKill = "SQ_numToKill";
	var nLevel = "SQ_npcLevel";
	var iIdToGet = "SQ_idToGet";
	var iIdToKill = "SQ_idToKill";
	var questStatus = "SQ_Status";

	if( !myNPC.InRange( myPlayer, 2 ))
		return;

	var Speech_Array = myString.split(" ");
	var s = 0, currObj = 0;

	for( s = 1; s <= Speech_Array.length; s++ )
	{
		if( Speech_Array[currObj].match( /\bQuest\b/i ) || Speech_Array[currObj].match( /\bAdventure\b/i ) || Speech_Array[currObj].match( /\bTask\b/i ) || Speech_Array[currObj].match( /\bquest\b/i ) || Speech_Array[currObj].match( /\badventure\b/i ) || Speech_Array[currObj].match( /\btask\b/i ) ) 
		{
			myNPC.TurnToward( myPlayer );
			for ( let i = 0; i < myArray.length; i++ ) 
			{
				var myQuestData = myArray[i].split(",");
				var playerSerial = myQuestData[1];
				var questStatusData = myQuestData[15];
				if ( myNPC.GetTag( "Completed_" + playerSerial ) == 1 )
				{
					myNPC.TextMessage( "You have already completed this quest." );
					return false;
				}

				if ( myNPC.GetTag("Declined_" + playerSerial ) == 0 || myPlayer.GetTag( questStatusData ) >= 1 )
				{
					// Quest is already in the log! Abort
					myPlayer.SetTag(questStatus, 8)
					TriggerEvent(19802, "convoeventgump", myPlayer, myNPC );
					return false;
				}
				else if( myQuestData[0] == questSlot )
				{
					questslotUsed = true;
					indexOfquestSlot = i;
				}
			}

			// Next, remove the existing entry for the selected slot from myArray!
			if( questslotUsed == true ) 
			{
				myArray.splice( indexOfquestSlot, 1 );
			}

			// Ok, if quest log wasn't found in the array, store quest in the questslot we selected earlier
			myArray.push( questSlot.toString() + "," + ( myPlayer.serial ).toString() + "," + myPlayer.name + "," + questName + "," + killAmount.toString() + "," + collectAmount.toString()+ "," + questTrg.toString() + "," + iNumToGet + "," + iLevel + "," + nNumToKill + "," + nLevel + "," + itemId.toString() + "," + iIdToGet + "," + npcId.toString() + "," + iIdToKill + "," + questStatus );
			if( TriggerEvent( 19806, "WriteQuestLog", myPlayer, myArray ))
			{
				myNPC.SetTag( "QuestSlot", questSlot );
				myPlayer.SetTag( questStatus, 1 );
				TriggerEvent( 19801, "questGump", myPlayer, myNPC );
				return false;
			}
		}
		else if( Speech_Array[currObj].match( /\bReward\b/i ) || Speech_Array[currObj].match( /\breward\b/i ) || Speech_Array[currObj].match( /\bprogress\b/i ))
		{
			myNPC.TurnToward(myPlayer);
			var npcLevel = myPlayer.GetTag( "SQ_npcLevel" );

			if( npcLevel )
			{
				var numToKill = myPlayer.GetTag( "SQ_numToKill" );

				if( numToKill > 0 )
				{
					myPlayer.SetTag( questStatus, 2 );
					TriggerEvent(19802, "convoeventgump", myPlayer, myNPC );
					return false;
				}
				else
				{
					myPlayer.SetTag( questStatus, 3 );
					myPlayer.SetTag( "SQ_npcLevel", 0 );
					myPlayer.SetTag( "SQ_numToKill", 0 );
					TriggerEvent(19802, "convoeventgump", myPlayer, myNPC );
					return false;
				}
			}
			else
			{
				myNPC.TextMessage( "I do not offer rewards." );
			}
			return false;
		}
		else if( Speech_Array[currObj].match( /\bQuite\b/i ) || Speech_Array[currObj].match( /\bquite\b/i ) || Speech_Array[currObj].match( /\bresign\b/i ))
		{
			myNPC.TurnToward( myPlayer );
			decline( myPlayer, myNPC );
		}
		currObj++;
	}
}

function onDropItemOnNpc( pDropper, pDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	// Read Quests Log
	var myArray = TriggerEvent( 19806, "ReadQuestLog", pDropper );
	for ( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var playerSerial = myQuestData[1];
		var killAmount = myQuestData[4];
		var collectAmount = myQuestData[5];
		var iLevel = myQuestData[8];
		var nNumToKill = myQuestData[9];
		var questStatus = myQuestData[15];

		pDroppedOn.TurnToward( pDropper );
		var itemLevel = pDropper.GetTag( iLevel );

		if( itemLevel )
		{
			var numZoogiFungus = pDropper.ResourceCount( itemId ); // This will look for the Item ID you put in at the top.

			if ( numZoogiFungus >= collectAmount ) // Checks to make sure you collected the amount
			{
				var pPack = pDropper.pack;
				if( pPack.totalItemCount >= pPack.maxItems || pPack.weight >= pPack.weightMax ) 
				{
					pDropper.SetTag( questStatus, 9 ) // Full Backpack
					TriggerEvent( 19802, "convoeventgump", pDropper, pDroppedOn );
					return 0;
				}

				pDropper.UseResource( collectAmount, itemId );
				if ( nNumToKill >= killAmount )  // Checks to make sure you have killed the amount
				{
					pDropper.SetTag( questStatus, 6 ) // Quest Completed
					pDroppedOn.SetTag( "Completed_" + playerSerial, 1)
					TriggerEvent( 19802, "convoeventgump", pDropper, pDroppedOn );
					decline( pDropper, pDroppedOn ); // Sets all tags to 0 or null
					rewardPlayer( pDropper, pDroppedOn ); // Reward Player
					return 0;
				}
				else
				{
					pDropper.SetTag( questStatus, 2 ); // Still need to kill npcs
					TriggerEvent( 19802, "convoeventgump", pDropper, pDroppedOn );
					return false;
				}
				return 0;
			}
			else if ( numZoogiFungus < collectAmount ) 
			{
				pDropper.SetTag( questStatus, 4 )
				TriggerEvent( 19802, "convoeventgump", pDropper, pDroppedOn );
				return 0;
			}
		}
		else
		{
			pDroppedOn.TextMessage( "Our arrangement was for 50 of the zoogi fungus. Please return to me when you have that amount." );
		}
	}
	return 0;
}

function rewardPlayer( myPlayer, myNPC )
{
	var goldToGive = 0;
	switch ( RandomNumber( 0, 2 ))
	{
		case 0: goldToGive = 50; break;
		case 1: goldToGive = 75; break;
		case 2: goldToGive = 100; break;
	}
	CreateDFNItem( myPlayer.socket, myPlayer, "0x0EED", goldToGive, "ITEM", true );
	myPlayer.SoundEffect( 0x0037, false );
}

function decline( myPlayer, myNPC )
{
	var myArray = TriggerEvent( 19806, "ReadQuestLog", myPlayer );
	for( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var playerSerial = myQuestData[1];
		var questName = myQuestData[3];
		var killAmount = myQuestData[4];
		var collectAmount = myQuestData[5];
		var questTrg = myQuestData[6];
		var iNumToGet = myQuestData[7];
		var iLevel = myQuestData[8];
		var nNumToKill = myQuestData[9];
		var nLevel = myQuestData[10];
		var itemId = myQuestData[11];
		var iIdToGet = myQuestData[12];
		var npcId = myQuestData[13];
		var iIdToKill = myQuestData[14];
		var questStatus = myQuestData[15];
		myNPC.SetTag( "Declined_" + playerSerial, 1 )
		myPlayer.SetTag( parseInt( iNumToGet ), 0 );
		myPlayer.SetTag( parseInt( iIdToGet ), 0 );
		myPlayer.SetTag( parseInt( iLevel ), 0 );
		myPlayer.SetTag( parseInt( nNumToKill ), 0 );
		myPlayer.SetTag( parseInt( nLevel ), 0 );
		myPlayer.SetTag( parseInt( iIdToKill ), 0 );
		myPlayer.SetTag( questStatus, 0 );
	}
}