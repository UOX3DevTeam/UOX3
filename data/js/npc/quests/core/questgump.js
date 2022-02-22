function questgump( pUser, myNPC)
{
	var socket = pUser.socket;
	socket.tempObj = myNPC;
	var questGump = new Gump;
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	questGump.AddPage( 0 );
	questGump.AddTiledGump( 50, 20, 400, 400, 2624 );
	questGump.AddCheckerTrans( 50, 20, 400, 400 );
	questGump.AddGump( 90, 33, 9005 );
	questGump.AddXMFHTMLGumpColor( 130, 45, 270, 20, 1049010, false, false, 0x7FFF ); // Quest Offer
	questGump.AddTiledGump( 130, 65, 175, 1, 9101 );
	questGump.AddGump( 140, 110, 1209 );

	for ( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var questTrg = myQuestData[6];

		if ( questSlot == myNPC.GetTag( "QuestSlot" ) ) 
		{
			switch ( parseInt(questSlot ) )
			{
				case parseInt(questSlot ):TriggerEvent( parseInt( questTrg ), "questOffer", questGump ); break;
			}
			break;
		}
	}

	questGump.AddRadio( 85, 350, 0x25f8, 0, 1 );
	questGump.AddXMFHTMLGumpColor( 120, 356, 280, 20, 1049011, false, false, 0x7FFF ); // I accept!
	questGump.AddRadio( 85, 385, 0x25f8, 0, 0 );
	questGump.AddXMFHTMLGumpColor( 120, 391, 280, 20, 1049012, false, false, 0x7FFF ); // No thanks, I decline.
	questGump.AddButton(340, 390, 0xf7, 1, 0, 1);//Okay button
	questGump.AddTiledGump( 50, 29, 30, 390, 10460 );
	questGump.AddTiledGump( 34, 140, 17, 279, 9263 );
	questGump.AddGump( 48, 135, 10411 );
	questGump.AddGump( -16, 285, 10402 );
	questGump.AddGump( 0, 10, 10421 );
	questGump.AddGump( 25, 0, 10420 );
	questGump.AddTiledGump( 83, 15, 350, 15, 10250 );
	questGump.AddGump( 34, 419, 10306 );
	questGump.AddGump( 442, 419, 10304 );
	questGump.AddTiledGump( 51, 419, 392, 17, 10101 );
	questGump.AddTiledGump( 415, 29, 44, 390, 2605 );
	questGump.AddTiledGump( 415, 29, 30, 390, 10460 );
	questGump.AddGump( 425, 0, 10441 );
	questGump.AddGump( 370, 50, 1417 );
	questGump.AddGump( 379, 60, 0x15c9 );//quest item picture
	questGump.Send( pUser );
	questGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var myNPC = socket.tempObj;
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	switch ( pButton )
	{
		case 0:break; // abort and do nothing if gump is closed with right click
		case 1:
			var OtherButton = gumpData.getButton(0);
			switch ( OtherButton )
			{
				case 0:pUser.SysMessage( "Such a pity, if you change your mind, I'll be here." );break; //decline
				case 1:// Accept
					for ( let i = 0; i < myArray.length; i++ )
					{
						var myQuestData = myArray[i].split(",");
						var questSlot = myQuestData[0];
						var killAmount = myQuestData[4];
						var collectAmount = myQuestData[5];

						if ( questSlot == myNPC.GetTag( "QuestSlot" ) )
						{
							switch (parseInt(questSlot))
							{
								case parseInt(questSlot):
									pUser.SetTempTag("QuestSlotTemp", questSlot);
									TriggerEvent( 19802, "convoeventgump", pUser, myNPC );			//accept gump
									if(killAmount >= 1)
										CreateNpcQuest( pUser, parseInt( questSlot ), parseInt ( killAmount ) );			// creating the quest setup  Example: Player,level,amount
									if(collectAmount >= 1)
										CreateItemQuest( pUser, parseInt( questSlot ), parseInt( collectAmount ) );
									break;
							}
							break;
						}
					}
			}
			break;
	}
}

function CreateItemQuest( pUser, questLevel, numToGet )
{
	var typeToGet = GetItemType( pUser, questLevel );
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	for (let i = 0; i < myArray.length; i++)
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var QiNumToGet = myQuestData[7];
		var QiLevel = myQuestData[8];

		if ( questSlot == pUser.GetTempTag("QuestSlotTemp" ) )
		{
			switch ( parseInt( questSlot ) )
			{
				case parseInt( questSlot ):
					pUser.SetTag( QiNumToGet.toString(), numToGet );
					pUser.SetTag( QiLevel.toString(), questLevel );
					break;
			}
			break;
		}
	}
	return typeToGet;
}

function CreateNpcQuest( pUser, npcLevel, numToKill )
{
	var typeToKill = GetMonsterType(pUser, npcLevel );
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	for ( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var QnNumToKill = myQuestData[9];
		var QnLevel = myQuestData[10];

		if ( questSlot == pUser.GetTempTag( "QuestSlotTemp" ) )
		{
			switch (parseInt( questSlot ) )
			{
				case parseInt( questSlot ):
					pUser.SetTag( QnNumToKill.toString(), numToKill );
					pUser.SetTag( QnLevel.toString(), npcLevel );
					break;
			}
			break;
		}
	}
	return typeToKill;
}

function GetItemType( pUser, questLevel ) 
{
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	for (let i = 0; i < myArray.length; i++) 
	{
		var myQuestData = myArray[i].split(",");
		var ItemId = myQuestData[11];
		var iIdToGet = myQuestData[12];
		switch (questLevel)
		{
			case questLevel:
				pUser.SetTag(iIdToGet, ItemId);
		}
	}
}

function GetMonsterType( pUser, npcLevel ) 
{
	var retVal = "";
	switch ( npcLevel ) 
	{
		case 1:
			switch ( RandomNumber( 0, 1 ) )
			{
				case 0:
					pUser.SetTag( "SQ_IDTOKILL", 0x0327 ); //red or black ant queen npc id
					retVal = "a red solen queen";
					break;
				case 1:
					pUser.SetTag( "SQ_IDTOKILL", 0x0327 );
					retVal = "a red solen queen";
					break;
			}
			break;
		case 2:
			switch ( RandomNumber( 0, 1 ) )
			{
				case 0:
					pUser.SetTag( "NPQ_IDTOKILL", 0x0027 ); // mongbat npc id
					retVal = "a mongbat";
					break;
				case 1:
					pUser.SetTag( "NPQ_IDTOKILL", 0x0027 );
					retVal = "a mongbat";
					break;
			}
			break;
		case 3:
			pUser.SetTag( "TTTQ_IDTOKILL", 0x00ce ); // alligator npc id
			retVal = "Vice Jaw";
			break;
	}
	return retVal;
}