function convoeventgump( pUser )
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

	switch ( pUser.GetTag( "QuestTracker" ) )// tracker for the quest
	{
		case "1":
			switch ( pUser.GetTag( "QuestStatus" ) )// status of each part of the quest
			{
				case "SQ_1": TriggerEvent( 20000, "questaccept", questGump ); break;
				case "SQ_2": TriggerEvent( 20000, "questduringnpckilling", questGump ); break;
				case "SQ_3": TriggerEvent( 20000, "questgathering", questGump ); break;
				case "SQ_4": TriggerEvent( 20000, "questduringgathering", questGump ); break;
				case "SQ_5": TriggerEvent( 20000, "questend", questGump ); break;
				case "SQ_6": TriggerEvent( 20000, "questbusy", questGump ); break;
			}
			break;
		case "2":
			switch ( pUser.GetTag( "QuestStatus" ) )// status of each part of the quest
			{
				case "NPQ_1": TriggerEvent( 20001, "questaccept", questGump ); break;
				case "NPQ_2": TriggerEvent( 20001, "questduringnpckilling", questGump ); break;
				case "NPQ_3": TriggerEvent( 20001, "questgathering", questGump ); break;
				case "NPQ_4": TriggerEvent( 20001, "questduringgathering", questGump ); break;
				case "NPQ_5": TriggerEvent( 20001, "questend", questGump ); break;
				case "NPQ_6": TriggerEvent( 20001, "questbusy", questGump ); break;
			}
			break;
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