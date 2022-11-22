function onSpeech( myString, myPlayer, myNPC )
{
	var socket = myPlayer.socket;
	var onehour = myPlayer.GetTag( "oneHour" );
	var twohour = myPlayer.GetTag( "twoHour" );
	var sixhour = myPlayer.GetTag( "sixHour" );

	var Speech_Array = myString.split(" ");
	var s = 0, currObj = 0;

	for( s = 1; s <= Speech_Array.length; s++ )
	{
		if( Speech_Array[currObj].match( /\bbod\b/i ) || Speech_Array[currObj].match( /\bBOD\b/i ))
		{
			myNPC.TurnToward( myPlayer );
			var expiryTimestamp = 0;
			if( onehour == 1 )
			{
				expiryTimestamp = myPlayer.GetJSTimer( 1, 2507 );
			}
			else if(twohour == 1)
			{
				expiryTimestamp = myPlayer.GetJSTimer( 2, 2507 );
			}
			else if(sixhour == 1)
			{
				expiryTimestamp = myPlayer.GetJSTimer( 3, 2507 );
			}
			else if( onehour == 0 || twohour == 0 || sixhour == 0 )
			{
				SmallBODAcceptGump( myPlayer, myNPC );
				return false;
			}
			var currentTimestamp = GetCurrentClock();
			myNPC.TextMessage( GetDictionaryEntry( 17265, socket.language ) + " " + ( ( expiryTimestamp - currentTimestamp ) / 60000 ).toFixed(0) + " " + GetDictionaryEntry( 17266, socket.language ));
			return false;
		}
	}
	currObj++;
}

function SmallBODAcceptGump( pUser, myNPC )
{
	//If you create the Bod with adminmenu or add command.
	var socket = pUser.socket;
	var bodGump = new Gump;
	var rand		= RandomNumber( 1, 49 );
	var bodEntry	= CreateEntries[rand];
	var itemName	= bodEntry.name; // name of the create entry
	var graphicID	= bodEntry.id; // section id of item to craft
	var weaponType  = TriggerEvent( 2500, "GetWeaponType", null, bodEntry.id );
	pUser.SetTempTag( "itemName", itemName );
	pUser.SetTempTag( "graphicID", graphicID );

	if( weaponType == "WRESTLING" ) // check make sure its not a weapon before asking for ore type
	{
			var materialColor = 0;
			var reqMaterial = false;
			switch( RandomNumber( 0, 8 ))
			{
				case 0:
					if( pUser.skills.blacksmithing >= 800 )
					{
						materialColor = 1750; // bronze
						reqMaterial = true;
						break;
					}
					break;
				case 1:
					if( pUser.skills.blacksmithing >= 750 )
					{
						materialColor = 2414; // copper
						reqMaterial = true;
						break;
					}
					break;
				case 2:
					if( pUser.skills.blacksmithing >= 900 )
					{
						materialColor = 2425; // agapite
						reqMaterial = true;
						break;
					}
					break;
				case 3:
					if( pUser.skills.blacksmithing >= 650 )
					{
						materialColor = 2419;// dull copper
						reqMaterial = true;
						break;
					}
					break;
				case 4:
					if( pUser.skills.blacksmithing >= 850 )
					{
						materialColor = 2213; // gold
						reqMaterial = true;
						break;
					}
					break;
				case 5:
					if( pUser.skills.blacksmithing >= 700 )
					{
						materialColor = 2406; // shadow iron
						reqMaterial = true;
						break;
					}
					break;
				case 6:
					if( pUser.skills.blacksmithing >= 1000 )
					{
						materialColor = 2219; // valorite
						reqMaterial = true;
						break;
					}
					break;
				case 7:
					if( pUser.skills.blacksmithing >= 950 )
					{
						materialColor = 2207; // verite
						reqMaterial = true;
						break;
					}
					break;
				case 8:
					materialColor = 0; // iron
					break;
				default:
					materialColor = 0;
					break;
			}
			pUser.SetTempTag( "materialColor", materialColor );
			pUser.SetTempTag( "reqMaterial", reqMaterial );
	}

	var init = pUser.GetTempTag( "init" ) // just to make sure we dont set the bod over
	if( init == false ) // Keep from resetting the amount needed.
	{
		//var amountMax = 0;
		if( pUser.skills.blacksmithing >= 701 )
		{
			if((( pUser.skills.blacksmithing + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
			{
				pUser.SetTempTag( "reqExceptional", 1 );
			}

			var values = [ 10, 15, 20, 20 ];
			var amountMax = values[Math.floor( Math.random() * values.length )];
			pUser.SetTempTag( "amountMax", amountMax );
		}
		else if( pUser.skills.blacksmithing >= 501 )
		{					
			var values = [ 10, 15, 15, 20 ];
			var amountMax = values[Math.floor( Math.random() * values.length )];
			pUser.SetTempTag( "amountMax", amountMax );
		}
		else
		{
			var values = [ 10, 10, 15, 20 ];
			var amountMax = values[Math.floor( Math.random() * values.length )];	
			pUser.SetTempTag( "amountMax", amountMax );
		}
		pUser.SetTempTag( "init", true );
	}

	var amountMax = parseInt( pUser.GetTempTag( "amountMax" )); // amount you have to make of the item
	var reqExceptional = parseInt( pUser.GetTempTag( "reqExceptional" ));
	var reqMaterial = pUser.GetTempTag( "reqMaterial" );

	bodGump.AddPage(0);
	bodGump.AddBackground( 25, 10, 430, 264, 5054 );

    bodGump.AddTiledGump( 33, 20, 413, 245, 2624 );
    bodGump.AddCheckerTrans( 33, 20, 413, 245 );

    bodGump.AddGump( 20, 5, 10460 );
    bodGump.AddGump( 430, 5, 10460 );
    bodGump.AddGump(20, 249, 10460 );
    bodGump.AddGump( 430, 249, 10460 );

	bodGump.AddHTMLGump( 190, 25, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17250, socket.language ) + "</basefont>" ); // A bulk order
	bodGump.AddHTMLGump(40, 48, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17267, socket.language ) + "</basefont>" ); // Ah!  Thanks for the goods!  Would you help me out?

	bodGump.AddHTMLGump( 40, 72, 210, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>" ); // Amount to make:
	bodGump.AddText( 250, 72, 1152, amountMax );

	bodGump.AddHTMLGump( 40, 96, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" ); // Items requested:
	bodGump.AddPicture(385, 96, graphicID);// image of the item requested
	bodGump.AddHTMLGump( 40, 120, 210, 20, 0, 0, "<basefont color=#ffffff>" + itemName + "</basefont>");// itemName of item to make

	if( reqExceptional == 1 || reqMaterial == true )
	{
		bodGump.AddHTMLGump( 40, 144, 210, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" ); // Special requirements to meet:
	}

	if( reqExceptional == 1 )
	{
		bodGump.AddHTMLGump( 40, 168, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>"); // All items must be exceptional.
	}

	if( reqMaterial == true ) // checks to see if a material is required for the bod.
	{
		if( reqExceptional == 1 )
		{
			var y = 192;
		}
		else
		{
			var y = 168;
		}
		var materialName = "";
		switch( pUser.GetTempTag( "materialColor" ))
		{
			case 0:
				materialName = "normal";
				break;
			case 2419:
				materialName = "dull copper";
				break;
			case 2406:
				materialName = "shadow iron";
				break;
			case 2414:
				materialName = "copper";
				break;
			case 1750:
				materialName = "bronze";
				break;
			case 2213:
				materialName = "gold";
				break;
			case 2425:
				materialName = "agapite";
				break;
			case 2207:
				materialName = "verite";
				break;
			case 2219:
				materialName = "valorite";
				break;
			default:
				materialName = "normal";
				break;
		}
		if( materialName != "" )
		{
			bodGump.AddHTMLGump( 40, y, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" );
		}
	}
	bodGump.AddHTMLGump( 40, 216, 350, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17268, socket.language ) + "</basefont>" ); // Do you want to accept this order?

    bodGump.AddButton( 100, 240, 4005, 4007, 1, 0, 1 );
    bodGump.AddHTMLGump(135, 240, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17269, socket.language ) + "</basefont>" ); // Ok

    bodGump.AddButton( 275, 240, 4005, 4007, 1, 0, 0 );
    bodGump.AddHTMLGump(310, 240, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 2709, socket.language ) + "</basefont>" ); // CANCEL

	bodGump.Send( pUser );
	bodGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var amountMax = parseInt( pUser.GetTempTag("amountMax" ));
	var reqExceptional = parseInt( pUser.GetTempTag("reqExceptional" ));
	var reqMaterial = pUser.GetTempTag( "reqMaterial" );
	var graphicID = pUser.GetTempTag( "graphicID" );
	var itemName = pUser.GetTempTag( "itemName" );
	var materialColor = pUser.GetTempTag( "materialColor" );

	switch ( pButton )
	{
		case 0:
			pUser.SetTempTag( "itemName", null );
			pUser.SetTempTag( "graphicID", null );
			pUser.SetTempTag( "amountMax", null );
			pUser.SetTempTag( "reqExceptional", null );
			pUser.SetTempTag( "reqMaterial", null );
			pUser.SetTempTag( "materialColor", null );
			pUser.SetTempTag( "init", false );// Exit
			break;
		case 1:
			var smallbod = CreateDFNItem(pUser.socket, pUser, ( "smallbod" ), 1, "ITEM", true );
			smallbod.SetTag( "itemName", itemName );
			smallbod.SetTag( "graphicID", graphicID );
			smallbod.SetTag( "amountMax", amountMax );
			smallbod.SetTag( "reqExceptional", reqExceptional );
			smallbod.SetTag( "reqMaterial", reqMaterial );
			smallbod.SetTag( "materialColor", materialColor );
			smallbod.SetTag( "init", true );
			pUser.SetTempTag( "itemName", null );
			pUser.SetTempTag( "graphicID", null );
			pUser.SetTempTag( "amountMax", null );
			pUser.SetTempTag( "reqExceptional", null );
			pUser.SetTempTag( "reqMaterial", null );
			pUser.SetTempTag( "materialColor", null );
			pUser.SetTempTag( "init", false );
			if( pUser.skills.blacksmithing >= 701 )
			{
				pUser.StartTimer( 21600000, 3, true );
				pUser.SetTag( "sixHour", 1 );
				break;
			}
			else if( pUser.skills.blacksmithing >= 501 )
			{
				pUser.StartTimer( 7200000, 2, true );
				pUser.SetTag( "twoHour", 1 );
				break;
			}
			else
			{
				pUser.StartTimer( 3600000, 1, true );
				pUser.SetTag( "oneHour", 1 );
				break;
			}
			break;
	}
}

function onTimer( pUser, timerID ) 
{
	// When Timer Expires set these flags on player false so he can get another BOD
	switch ( timerID )
	{
		case 1:pUser.SetTag( "oneHour", 0 );break;
		case 2:pUser.SetTag( "twoHour", 0 );break;
		case 3:pUser.SetTag( "sixHour", 0 );break;
	}
}

function onDropItemOnNpc( pDropper, pDroppedOn, iDropped )
{
	var socket = pDropper.socket;
	var amountMax = parseInt( iDropped.GetTag( "amountMax" )); // amount you have to make of the item
	var amountCur = parseInt( iDropped.GetTag( "amountCur" )); // amount you have combined
	if ( iDropped.sectionID == "smallbod" )
	{
		if( amountCur < amountMax )
		{
			pDroppedOn.TextMessage( GetDictionaryEntry( 17270, socket.language )); // You have not completed the order yet.
			return false;
		}

		var onehour = pDropper.GetTag( "oneHour" );
		var twohour = pDropper.GetTag( "twoHour" );
		var sixhour = pDropper.GetTag( "sixHour" );
		if( onehour == 1 )
		{
			pDropper.KillJSTimer( 1, 2507 );
			pDropper.SetTag( "oneHour", 0 );
		}
		else if( twohour == 1 )
		{
			pDropper.KillJSTimer( 2, 2507 );
			pDropper.SetTag( "twoHour", 0 );
		}
		else if( sixhour == 1 )
		{
			pDropper.KillJSTimer( 3, 2507 );
			pDropper.SetTag( "sixHour", 0 );
		}
		iDropped.Delete();
		pDropper.SoundEffect( 0x3D, true );
		pDroppedOn.TextMessage( GetDictionaryEntry( 17271, socket.language )); // Thank you so much!  Here is a reward for your effort.
		var goldToGive = 0;
		switch( RandomNumber( 0, 2 ))
		{
			case 0: goldToGive = 100; break;
			case 1: goldToGive = 200; break;
			case 2: goldToGive = 300; break;
		}
		CreateDFNItem( pDropper.socket, pDropper, "0x0EED", goldToGive, "ITEM", true );
		return 2;
	}

	// Otherwise, reject the item drop and bounce it back where it came from
	return false;
}