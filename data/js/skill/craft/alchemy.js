var textHue = 0x480;				// Color of the text.
var scriptID = 4028;				// Use this to tell the gump what script to close.
var gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
var itemDetailsID = 4026;
var craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

var myPage1 = [10908, 10909, 10910, 10911, 10912, 10913, 10914, 10915];	// Healing and Curative
var myPage2 = [10916, 10917, 10918, 10919, 10920]; // Enhancement
var myPage3 = [10921, 10922, 10923, 10924]; // Toxic
var myPage4 = [10925]; // Explosive // 10926, 10927 needs scripted

function page1( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTempTag( "page", 1 );
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for ( var i = 0; i < myPage1.length; i++ )
	{
		var index = i % 10;
		if ( index == 0 )
		{
			if ( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.Language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage( ( i / 10 ) + 1 );

			if ( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.Language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 100 + i );

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry( myPage1[i], socket.language ) );

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2100 + i);
	}
	myGump.Send( socket );
	myGump.Free();
}

function page2( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTempTag( "page", 2 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage2.length; i++ )
	{
		var index = i % 10;
		if ( index == 0 )
		{
			if ( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.Language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage( ( i / 10 ) + 1 );

			if ( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.Language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 200 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage2[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2200 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page3( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTempTag( "page", 3 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage3.length; i++ )
	{
		var index = i % 10;
		if ( index == 0 )
		{
			if ( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.Language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage( ( i / 10 ) + 1 );

			if ( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.Language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 300 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage3[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2300 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page4( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTempTag( "page", 4 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage4.length; i++ )
	{
		var index = i % 10;
		if ( index == 0 )
		{
			if ( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.Language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage( ( i / 10 ) + 1 );

			if ( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.Language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 400 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage4[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2400 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function onTimer( pUser, timerID )
{
	var socket = pUser.socket;

	switch ( timerID )
	{
		case 1:
			TriggerEvent( scriptID, "page1", socket, pUser );
			break;
		case 2:
			TriggerEvent( scriptID, "page2", socket, pUser );
			break;
		case 3:
			TriggerEvent( scriptID, "page3", socket, pUser );
			break;
		case 4:
			TriggerEvent( scriptID, "page4", socket, pUser );
			break;
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var bItem = pSock.tempObj;
	var gumpID = scriptID + 0xffff;
	switch ( pButton )
	{
		case 0:
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;// abort and do nothing
		case 1:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page1", pSock, pUser );
			break;
		case 2:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page2", pSock, pUser );
			break;
		case 3:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page3", pSock, pUser );
			break;
		case 4:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page4", pSock, pUser );
			break;
		case 100:
			MakeItem( pSock, pUser, 305 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 100 );
			break;//refresh
		case 101:
			MakeItem( pSock, pUser, 306 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 101 );
			break;//greater refrehment
		case 102:
			MakeItem( pSock, pUser, 298 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 102 );
			break;//lesser heal
		case 103:
			MakeItem( pSock, pUser, 299 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 103 );
			break;//heal
		case 104:
			MakeItem( pSock, pUser, 300 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 104 );
			break;//greater heal
		case 105:
			MakeItem( pSock, pUser, 292 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 105 );
			break;//lesser cure
		case 106:
			MakeItem( pSock, pUser, 293 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 106 );
			break;//cure
		case 107:
			MakeItem( pSock, pUser, 293 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTempTag( "MAKELAST", 107 );
			break;//greater cure
		case 200:
			MakeItem( pSock, pUser, 290 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTempTag( "MAKELAST", 200 );
			break;//agility
		case 201:
			MakeItem( pSock, pUser, 291 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTempTag( "MAKELAST", 201 );
			break;//greater agility
		case 202:
			MakeItem( pSock, pUser, 309 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTempTag( "MAKELAST", 202 );
			break;//night sight
		case 203:
			MakeItem( pSock, pUser, 307 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTempTag( "MAKELAST", 203 );
			break;//strength
		case 204:
			MakeItem( pSock, pUser, 308 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTempTag( "MAKELAST", 204 );
			break;//greater strength
		case 300:
			MakeItem( pSock, pUser, 301 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTempTag( "MAKELAST", 300 );
			break;//lesser poison
		case 301:
			MakeItem( pSock, pUser, 302 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTempTag( "MAKELAST", 301 );
			break;//poison
		case 302:
			MakeItem( pSock, pUser, 303 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTempTag( "MAKELAST", 302 );
			break;//greater poison
		case 303:
			MakeItem( pSock, pUser, 304 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTempTag( "MAKELAST", 303 );
			break;//deadly poison
		case 400:
			MakeItem( pSock, pUser, 295 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTempTag( "MAKELAST", 400 );
			break;//lesser explosion
		case 401:
			MakeItem( pSock, pUser, 296 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTempTag( "MAKELAST", 401 );
			break;//explosion
		case 402:
			MakeItem( pSock, pUser, 297 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTempTag( "MAKELAST", 402 );
			break;//greater explosion
		case 2100:
			pUser.SetTempTag( "ITEMDETAILS", 305 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//refresh
		case 2101:
			pUser.SetTempTag( "ITEMDETAILS", 306 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater refrehment
		case 2102:
			pUser.SetTempTag( "ITEMDETAILS", 298 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//lesser heal
		case 2103:
			pUser.SetTempTag( "ITEMDETAILS", 299 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//heal
		case 2104:
			pUser.SetTempTag( "ITEMDETAILS", 300 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater heal
		case 2105:
			pUser.SetTempTag( "ITEMDETAILS", 292 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//lesser cure
		case 2106:
			pUser.SetTempTag( "ITEMDETAILS", 293 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//cure
		case 2107:
			pUser.SetTempTag( "ITEMDETAILS", 293 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater cure
		case 2200:
			pUser.SetTempTag( "ITEMDETAILS", 290 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//agility
		case 2201:
			pUser.SetTempTag( "ITEMDETAILS", 291 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater agility
		case 2202:
			pUser.SetTempTag( "ITEMDETAILS", 309 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//night sight
		case 2203:
			pUser.SetTempTag( "ITEMDETAILS", 307 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//strength
		case 2204:
			pUser.SetTempTag( "ITEMDETAILS", 308 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater strength
		case 2300:
			pUser.SetTempTag( "ITEMDETAILS", 301 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//lesser poison
		case 2301:
			pUser.SetTempTag( "ITEMDETAILS", 302 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//poison
		case 2302:
			pUser.SetTempTag( "ITEMDETAILS", 303 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater poison
		case 2303:
			pUser.SetTempTag( "ITEMDETAILS", 304 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//deadly poison
		case 2400:
			pUser.SetTempTag( "ITEMDETAILS", 295 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//lesser explosion
		case 2401:
			pUser.SetTempTag( "ITEMDETAILS", 296 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//explosion
		case 2402:
			pUser.SetTempTag( "ITEMDETAILS", 297 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//greater explosion
		case 5000:
			switch ( pUser.GetTempTag( "MAKELAST" ) )
			{
				case 100:
					MakeItem( pSock, pUser, 305 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 100 );
					break;//refresh
				case 101:
					MakeItem( pSock, pUser, 306 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 101 );
					break;//greater refrehment
				case 102:
					MakeItem( pSock, pUser, 298 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 102 );
					break;//lesser heal
				case 103:
					MakeItem( pSock, pUser, 299 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 103 );
					break;//heal
				case 104:
					MakeItem( pSock, pUser, 300 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 104 );
					break;//greater heal
				case 105:
					MakeItem( pSock, pUser, 292 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 105 );
					break;//lesser cure
				case 106:
					MakeItem( pSock, pUser, 293 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 106 );
					break;//cure
				case 107:
					MakeItem( pSock, pUser, 293 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTempTag( "MAKELAST", 107 );
					break;//greater cure
				case 200:
					MakeItem( pSock, pUser, 290 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTempTag( "MAKELAST", 200 );
					break;//agility
				case 201:
					MakeItem( pSock, pUser, 291 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTempTag( "MAKELAST", 201 );
					break;//greater agility
				case 202:
					MakeItem( pSock, pUser, 309 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTempTag( "MAKELAST", 202 );
					break;//night sight
				case 203:
					MakeItem( pSock, pUser, 307 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTempTag( "MAKELAST", 203 );
					break;//strength
				case 204:
					MakeItem( pSock, pUser, 308 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTempTag( "MAKELAST", 204 );
					break;//greater strength
				case 300:
					MakeItem( pSock, pUser, 301 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTempTag( "MAKELAST", 300 );
					break;//lesser poison
				case 301:
					MakeItem( pSock, pUser, 302 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTempTag( "MAKELAST", 301 );
					break;//poison
				case 302:
					MakeItem( pSock, pUser, 303 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTempTag( "MAKELAST", 302 );
					break;//greater poison
				case 303:
					MakeItem( pSock, pUser, 304 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTempTag( "MAKELAST", 303 );
					break;//deadly poison
				case 400:
					MakeItem( pSock, pUser, 295 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTempTag( "MAKELAST", 400 );
					break;//lesser explosion
				case 401:
					MakeItem( pSock, pUser, 296 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTempTag( "MAKELAST", 401 );
					break;//explosion
				case 402:
					MakeItem( pSock, pUser, 297 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTempTag( "MAKELAST", 402 );
					break;//greater explosion
			}break;
	}
}