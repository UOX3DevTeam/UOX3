function questgump( pUser )
{
	var questGump = new Gump;

	questGump.AddPage( 0 );
	questGump.AddTiledGump( 50, 20, 400, 400, 2624 );
	questGump.AddCheckerTrans( 50, 20, 400, 400 );
	questGump.AddGump( 90, 33, 9005 );
	questGump.AddXMFHTMLGumpColor( 130, 45, 270, 20, 1049010, false, false, 0x7FFF ); // Quest Offer
	questGump.AddTiledGump( 130, 65, 175, 1, 9101 );
	questGump.AddGump( 140, 110, 1209 );

	switch ( pUser.GetTag( "QuestTracker" ) )
	{
		case "1":TriggerEvent( 20000, "questoffer", questGump );break;
		case "2":TriggerEvent( 20001, "questoffer", questGump );break;
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
	switch ( pButton )
	{
		case 0:break; // abort and do nothing if gump is closed with right click
		case 1:
			var OtherButton = gumpData.getButton(0);
			switch ( OtherButton )
			{
				case 0:pUser.SysMessage( "You have declined the Quest." );break; //decline
				case 1:// Accept
					switch (pUser.GetTag( "QuestTracker" ) )
					{
						case "1":
							pUser.SetTag( "QuestStatus", "SQ_1" );					// setting what status the players is in during the Quest.
							TriggerEvent( 19802, "convoeventgump", pUser );	//accept gump
							CreateNpcQuest( pUser, 1, 5 );					// creating the quest setup  Example: Player,level,amount
							CreateItemQuest( pUser, 1, 50 );
							break;
						case "2":
							pUser.SetTag( "QuestStatus", "NPQ_1" );					// setting what status the players is in during the Quest.
							TriggerEvent( 19802, "convoeventgump", pUser );	//accept gump
							CreateNpcQuest( pUser, 2, 5 );					// creating the quest setup  Example: Player,level,amount
							CreateItemQuest( pUser, 2, 5 );
							break;
					}
			}
			break;
	}
}

function CreateItemQuest( pUser, questLevel, numToGet )
{
	var typeToGet = GetItemType( pUser, questLevel );
	switch ( pUser.GetTag( "QuestTracker" ) )
	{
		case "1":
			pUser.SetTag( "SQ_numToGet", numToGet );
			pUser.SetTag( "SQ_LEVEL", questLevel );
			break;
		case "2":
			pUser.SetTag( "NPQ_numToGet", numToGet );
			pUser.SetTag( "NPQ_LEVEL", questLevel );
			break;
	}
	return typeToGet;
}

function CreateNpcQuest( pUser, npcLevel, numToKill )
{
	var typeToKill = GetMonsterType(pUser, npcLevel );
	switch (pUser.GetTag( "QuestTracker" ) )
	{
		case "1":
			pUser.SetTag( "SQ_NUMTOKILL", numToKill );
			pUser.SetTag( "SQ_NPCLEVEL", npcLevel );
			break;
		case "2":
			pUser.SetTag( "NPQ_NUMTOKILL", numToKill );
			pUser.SetTag( "NPQ_NPCLEVEL", npcLevel );
			break;
	}
	return typeToKill;
}

function GetItemType( pUser, questLevel ) 
{
	var retVal = "";
	switch ( questLevel ) 
	{
		case 1:
			switch ( RandomNumber( 0, 1 ) ) 
			{
				case 0:
					pUser.SetTag( "SQ_IDTOGET", 0x26B7 ); //Item ID of the Zoogie Fungus
					retVal = "Zoogi Fungus";
					break;
				case 1:
					pUser.SetTag( "SQ_IDTOGET", 0x26B7 );
					retVal = "Zoogi Fungus";
					break;
			}
			break;
		case 2:
			switch (RandomNumber(0, 1)) 
			{
				case 0:
					pUser.SetTag( "NPQ_IDTOGET", 0x1bef ); //Item ID of the ingots
					retVal = "iron ingot";
					break;
				case 1:
					pUser.SetTag( "NPQ_IDTOGET", 0x1bef );
					retVal = "iron ingot";
					break;
			}
			break;
	}
	return retVal;
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
	}
	return retVal;
}