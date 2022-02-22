function convoeventgump( pUser, myNPC)
{
	var questGump = new Gump;

	questGump.AddPage( 0 );
	questGump.AddGump( 349, 10, 9392 );
	questGump.AddTiledGump( 349, 130, 100, 120, 9395 );
	questGump.AddTiledGump( 149, 10, 200, 140, 9391 );
	questGump.AddTiledGump( 149, 250, 200, 140, 9397 );
	questGump.AddGump( 349, 250, 9398 );
	questGump.AddGump( 35, 10, 9390 );
	questGump.AddTiledGump( 35, 150, 120, 100, 9393 );
	questGump.AddGump( 35, 250, 9396 );
	questGump.AddTiledGump( 55, 40, 388, 323, 2624 );
	questGump.AddCheckerTrans( 55, 40, 388, 323 );
	questGump.AddXMFHTMLGumpColor( 110, 60, 200, 20, 1049069, false, false, 0x7FFF ); // <STRONG>Conversation Event</STRONG>

	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	for (let i = 0; i < myArray.length; i++)
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var questTrg = myQuestData[6];

		if ( questSlot == myNPC.GetTag("QuestSlot" ) )
		{
			switch (parseInt( questSlot ) )
			{
				case parseInt( questSlot ):
					switch ( pUser.GetTag( "QuestStatus" ) )// status of each part of the quest
					{
						case 1: TriggerEvent(parseInt( questTrg ), "questAccept", questGump ); break;
						case 2: TriggerEvent(parseInt( questTrg ), "questDuringNpcKilling", questGump ); break;
						case 3: TriggerEvent(parseInt( questTrg ), "questGathering", questGump ); break;
						case 4: TriggerEvent(parseInt( questTrg ), "questDuringGathering", questGump ); break;
						case 5: TriggerEvent(parseInt( questTrg ), "questEnd", questGump ); break;
						case 6: TriggerEvent(parseInt( questTrg ), "questBusy", questGump ); break;
					}
					break;
			}
			break;
		}
	}

	questGump.AddGump( 65, 14, 10102 );
	questGump.AddTiledGump( 81, 14, 349, 17, 10101 );
	questGump.AddGump( 426, 14, 10104 );
	questGump.AddTiledGump( 75, 90, 200, 1, 9101 );
	questGump.AddGump( 75, 58, 9781 );
	questGump.AddGump( 380, 45, 223 );
	questGump.AddButton( 220, 335, 0x909, 1, 0, 0 );//okay button
	questGump.AddGump( 0, 0, 10440 );
	questGump.Send( pUser );
	questGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	switch ( pButton )
	{
		case 0:break;
	}
}