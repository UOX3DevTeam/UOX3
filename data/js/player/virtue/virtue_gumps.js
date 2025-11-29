function virtueGump( pUser, tChar )
{
	if( !ValidateObject( pUser ) || !ValidateObject( tChar ))
		return;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	var g = new Gump();

	g.AddPage( 0 );
	g.AddGump( 30, 40, 104 );
	g.AddPage( 1 );

	// Humility
	g.AddButtonTileArt( 61, 71, 108, 108, 1, 0, 11, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 5 ), 61, 71);
	g.AddGumpColor( 61, 71, 108, TriggerEvent( 8003, "Virtue_GetHue", tChar, 0 ));
	// Valor
	g.AddButtonTileArt( 123, 46, 112, 112, 1, 0, 12, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 5 ), 123, 46);
	g.AddGumpColor(123, 46, 112, TriggerEvent( 8003, "Virtue_GetHue", tChar, 4 ));
	// Honor
	g.AddButtonTileArt( 187, 70, 107, 107, 1, 0, 13, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 5 ), 187, 70);
	g.AddGumpColor(187, 70, 107, TriggerEvent( 8003, "Virtue_GetHue", tChar, 5 ));
	// Sacrifice
	g.AddButtonTileArt( 35, 135, 105, 105, 1, 0, 14, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 1 ), 35, 135);
	g.AddGumpColor(35, 135, 110, TriggerEvent( 8003, "Virtue_GetHue", tChar, 1 ));
	// Compassion
	g.AddButtonTileArt( 211, 133, 105, 105, 1, 0, 15, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 2 ), 211, 133);
	g.AddGumpColor(211, 133, 105, TriggerEvent( 8003, "Virtue_GetHue", tChar, 2 ));
	// Spirituality
	g.AddButtonTileArt( 61, 195, 111, 111, 1, 0, 16, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 3 ), 61, 195);
	g.AddGumpColor(61, 195, 111, TriggerEvent( 8003, "Virtue_GetHue", tChar, 3 ));
	// Justice
	g.AddButtonTileArt( 186, 195, 109, 109, 1, 0, 17, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 6 ), 186, 195);
	g.AddGumpColor(186, 195, 109, TriggerEvent( 8003, "Virtue_GetHue", tChar, 6 ));
	// Honesty
	g.AddButtonTileArt( 121, 221, 106, 106, 1, 0, 18, 0x1018, TriggerEvent( 8003, "Virtue_GetHue", tChar, 7 ), 121, 221);
	g.AddGumpColor(121, 221, 106, TriggerEvent( 8003, "Virtue_GetHue", tChar, 7 ));

	// Only show status / close buttons when viewing self,
	if( pUser.serial === tChar.serial )
	{
		// Button 1: open status gump
		g.AddButton( 57, 269, 2027, 1, 0, 100 );

		// Button 2: close (we just treat as close)
		g.AddButton( 186, 269, 2071, 1, 0, 104 );
	}

	g.Send( pSocket );
	g.Free();
}

function virtueGumpStatus( pUser, tChar )
{
	if( !ValidateObject( pUser ) || !ValidateObject( tChar ))
		return;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	var g = new Gump();
	g.AddPage( 0 );

	g.AddGump(30, 40, 2080);
	g.AddGump(47, 77, 2081);
	g.AddGump(47, 147, 2081);
	g.AddGump(47, 217, 2081);
	g.AddGump(47, 267, 2083);
	g.AddGump(70, 213, 2091);

	g.AddPage(1);

	g.AddXMFHTMLGump(140, 73, 200, 20, 1077972, false, false); // The Virtues

	g.AddXMFHTMLGump(80, 100, 100, 40, 1051000, false, false); // Humility
	g.AddXMFHTMLGump(80, 129, 100, 40, 1051001, false, false); // Sacrifice
	g.AddXMFHTMLGump(80, 159, 100, 40, 1051002, false, false); // Compassion
	g.AddXMFHTMLGump(80, 189, 100, 40, 1051003, false, false); // Spirituality
	g.AddXMFHTMLGump(200, 100, 200, 40, 1051004, false, false); // Valor
	g.AddXMFHTMLGump(200, 129, 200, 40, 1051005, false, false); // Honor
	g.AddXMFHTMLGump(200, 159, 200, 40, 1051006, false, false); // Justice
	g.AddXMFHTMLGump(200, 189, 200, 40, 1051007, false, false); // Honesty
	g.AddXMFHTMLGump(75, 224, 220, 60, 1052062, false, false); // Click on a blue gem to view your status in that virtue.

	g.AddButton(60, 100, 1210, 1210, 1, 0, 1);
	g.AddButton(60, 129, 1210, 1210, 1, 0, 2);
	g.AddButton(60, 159, 1210, 1210, 1, 0, 3);
	g.AddButton(60, 189, 1210, 1210, 1, 0, 4);
	g.AddButton(180, 100, 1210, 1210, 1, 0, 5);
	g.AddButton(180, 129, 1210, 1210, 1, 0, 6);
	g.AddButton(180, 159, 1210, 1210, 1, 0, 7);
	g.AddButton(180, 189, 1210, 1210, 1, 0, 8);
	g.AddButton(280, 43, 4014, 4014, 1, 0, 101);

	g.Send( pSocket );
	g.Free();
}

function showVirtueInfoGump( pUser, virtueIndex, descriptionID, webPage )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	if( virtueIndex < 0 )
		virtueIndex = 0;
	if( virtueIndex > 7 )
		virtueIndex = 7;

	var value    = TriggerEvent( 8003, "Virtue_GetValue", pUser, virtueIndex );
	var maxValue = TriggerEvent( 8003, "Virtue_GetMaxAmount", virtueIndex );

	// Compute number of filled dots (0..10), matching RunUO logic
	var dots = 0;

	if( value < 4000 )
	{
		dots = Math.floor( value / 400 );
	}
	else if( value < 10000 )
	{
		dots = Math.floor(( value - 4000 ) / 600 );
	}
	else if( value < maxValue )
	{
		var denom = ( maxValue - 10000 ) / 10;
		if( denom <= 0 )
			dots = 10;
		else
			dots = Math.floor(( value - 10000 ) / denom );
	}
	else
	{
		dots = 10;
	}

	if( dots < 0 )
		dots = 0;
	if( dots > 10 )
		dots = 10;

	// Value description ID (cliloc), same logic as RunUO
	var valueDescID;

	if( value < 1 )
		valueDescID = 1052044; // You have not started on the path of this Virtue.
	else if( value < 400 )
		valueDescID = 1052045; // You have barely begun your journey...
	else if( value < 2000 )
		valueDescID = 1052046; // You have progressed in this Virtue...
	else if( value < 3600 )
		valueDescID = 1052047; // Your journey through the path is going well.
	else if( value < 4000 )
		valueDescID = 1052048; // You feel very close to achieving your next path...
	else if( dots < 1 )
		valueDescID = 1052049; // You have achieved a path in this Virtue.
	else if( dots < 9 )
		valueDescID = 1052047; // Your journey through the path is going well.
	else if( dots < 10 )
		valueDescID = 1052048; // You feel very close to achieving your next path...
	else
		valueDescID = 1052050; // You have achieved the highest path in this Virtue.

	// Store state for onGumpPress (we only really need the URL, but keeping virtue/desc
	// can be handy if you want to re-open or debug later)
	pUser.SetTempTag( "VirtueInfo_VirtueIndex", virtueIndex );
	pUser.SetTempTag( "VirtueInfo_DescID", descriptionID );
	pUser.SetTempTag( "VirtueInfo_WebPage", webPage ? webPage : "" );

	var g = new Gump();

	g.AddPage( 0 );

	// Frame art (same coords and gumps as RunUO)
	g.AddGump( 30, 40, 2080 );
	g.AddGump( 47, 77, 2081 );
	g.AddGump( 47, 147, 2081 );
	g.AddGump( 47, 217, 2081 );
	g.AddGump( 47, 267, 2083 );
	g.AddGump( 70, 213, 2091 );

	g.AddPage( 1 );

	// Progress dots at top
	for( var i = 0; i < 10; i++ )
	{
		var x = 95 + ( i * 17 );
		var gumpID = ( i < dots ) ? 2362 : 2360; // Filled vs empty dot art
		g.AddGump( x, 50, gumpID );
	}

	// Virtue name line: 1051000 + virtueIndex (same as RunUO)
	// AddXMFHTMLGump( x, y, width, height, clilocID, background, scrollbar )
	g.AddXMFHTMLGump( 157, 73, 200, 40, 1051000 + virtueIndex, false, false );

	// Main description text (passed in as descriptionID)
	g.AddXMFHTMLGump( 75, 95, 220, 140, descriptionID, false, false );

	// Value description ("You have not started", "Your journey...", etc.)
	g.AddXMFHTMLGump( 70, 224, 229, 60, valueDescID, false, false );

	// Button 2: "learn more" / web link (top right)
	g.AddButton( 65, 277, 1209, 1, 0, 102 );

	// Button 1: back (bottom left)
	g.AddButton( 280, 43, 4014, 1, 0, 103 );

	// Bottom line: if no webPage, "This virtue is not yet defined."
	// else "-click to learn more (opens webpage)"
	var hasWeb = webPage && webPage.length > 0;
	var bottomID = hasWeb ? 1052052 : 1052055;

	g.AddXMFHTMLGump( 83, 275, 400, 40, bottomID, false, false );

	g.Send( pSocket );
	g.Free();
}

function onGumpPress( socket, buttonID, gumpID )
{
	if( !socket || !socket.currentChar )
		return false;

	var pUser = socket.currentChar;
	var tChar = pUser;

	switch( buttonID )
	{
		case 1: showVirtueInfoGump( pUser, 0, 1052051, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#humility" );break;
		case 2: showVirtueInfoGump( pUser, 1, 1052053, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#sacrifice"); break;
		case 3: showVirtueInfoGump( pUser, 2, 1052054, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#compassion"); break;
		case 4: showVirtueInfoGump( pUser, 3, 1052056, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#spirituality"); break;
		case 5: showVirtueInfoGump( pUser, 4, 1052057, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#valor"); break;
		case 6: showVirtueInfoGump( pUser, 5, 1052058, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#honor"); break;
		case 7: showVirtueInfoGump( pUser, 6, 1052059, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#justice"); break;
		case 8: showVirtueInfoGump( pUser, 7, 1052060, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#honesty"); break;
		case 11:
			TriggerEvent( 8005, "Humility_StartPetTarget", pUser );
			virtueGump( pUser, tChar );
			break;
		case 12: pUser.SysMessage("This virtue is not activated through the virtue menu."); virtueGump( pUser, tChar ); break;
		case 13: pUser.SysMessage("This virtue is not activated through the virtue menu."); virtueGump( pUser, tChar ); break;
		case 14: pUser.SysMessage("You must be a Seeker of Spirituality to invoke this Virtue."); virtueGump( pUser, tChar ); break;
		case 15: pUser.SysMessage("Target the Champion Idol of the Champion you wish to challenge!."); virtueGump( pUser, tChar ); break;
		case 16: pUser.SysMessage("Target what you wish to honor.."); virtueGump( pUser, tChar ); break;
		case 17: pUser.SysMessage("You must reach the first path in this virtue to invoke it."); virtueGump( pUser, tChar ); break;
		case 18: pUser.SysMessage("This virtue is not activated through the virtue menu."); virtueGump( pUser, tChar ); break;
	}

	// Main Virtue gump buttons
	if( buttonID === 100 )
	{
		if( pUser.serial === tChar.serial )
		{
			virtueGumpStatus(pUser, tChar)
		}

		return true;
	}

	if( buttonID === 101 )
	{
		virtueGump( pUser, tChar );
		return true;
	}

	if (buttonID === 102)
	{
		var virtueIndex = pUser.GetTempTag( "VirtueInfo_VirtueIndex" );
		var virtueDescID = pUser.GetTempTag( "VirtueInfo_DescID" );
		var webURL = pUser.GetTempTag("VirtueInfo_WebPage")
		socket.OpenURL( webURL.toString() );
		showVirtueInfoGump( pUser, virtueIndex, virtueDescID, webURL.toString() );
	}

	if( buttonID === 103 )
	{
		if( pUser.serial === tChar.serial )
		{
			virtueGumpStatus( pUser, tChar )
		}
		return true;
	}

	// Status gump: any button closes it, nothing else
	if( buttonID === 104 )
	{
		// Status gump: any button closes it, nothing else
		return false;
	}

	// Not our gump
	return false;
}