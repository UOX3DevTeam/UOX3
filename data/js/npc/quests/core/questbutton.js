function questbutton( pUser )
{
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	
	var myGump = new Gump;
	var pSock = pUser.socket;

	myGump.AddPage( 0 );
	myGump.AddHTMLGump( 130, 45, 270, 16, false, false, "<BASEFONT color=#FFFFFF>Quest Log</BASEFONT>" );
	myGump.AddTiledGump( 50, 20, 400, 400, 0x1404 );
	myGump.AddTiledGump( 50, 29, 30, 390, 0x28DC );
	myGump.AddTiledGump( 34, 140, 17, 279, 0x242F );
	myGump.AddGump( 48, 135, 0x28AB );
	myGump.AddGump( -16, 285, 0x28A2 );
	myGump.AddGump( 0, 10, 0x28B5 );
	myGump.AddGump( 25, 0, 0x28B4 );
	myGump.AddTiledGump( 83, 15, 350, 15, 0x280A );
	myGump.AddGump( 34, 419, 0x2842 );
	myGump.AddGump( 442, 419, 0x2840 );
	myGump.AddTiledGump( 51, 419, 392, 17, 0x2775 );
	myGump.AddTiledGump( 415, 29, 44, 390, 0xA2D );
	myGump.AddTiledGump( 415, 29, 30, 390, 0x28DC );
	myGump.AddGump( 370, 50, 0x589 );
	myGump.AddGump( 379, 60, 0x15A9 );
	myGump.AddGump( 425, 0, 0x28C9 );
	myGump.AddGump( 90, 33, 0x232D );
	myGump.AddTiledGump( 130, 65, 175, 1, 0x238D );
	for ( let i = 0; i < myArray.length; i++ ) 
	{
		var myQuestData = myArray[i].split(",");
		var myQuestSlot = myQuestData[0];
		var myPlayerName = myQuestData[2];
		var myQuestName = myQuestData[3];
		var questStatus = myQuestData[15];

		var index = i % 10;
		if ( pUser.GetTag( questStatus.toString() ) >= 1 && pUser.GetTag( questStatus.toString() ) < 5)
		{
			myGump.AddHTMLGump( 98, 140 + ( index * 20 ), 270, 21, false, false, "<BASEFONT color=#FFFFFF>" + myQuestName + "</BASEFONT>" );
			myGump.AddButton( 368, 140 + ( index * 20 ), 0x26B0, 0x26B1, 1, 0, 1 * myQuestSlot );
		}
	}

	myGump.AddButton( 313, 395, 0x2EEC, 0x2EEE, 1, 0, 0 );//close button
	myGump.Send( pSock );
	myGump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pUser );
	for ( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var myQuestSlot = myQuestData[0];
		switch ( pButton )
		{
			case 0: break;// abort and do nothing
			case parseInt( myQuestSlot ):
				pUser.SetTempTag( "QuestSlotTemp", myQuestSlot );
				TriggerEvent( 19800, "questProgress", pUser );
				break;//accept
		}
	}
}
