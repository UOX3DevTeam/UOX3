var LabelHue = 0x480;				// Color of the text.
var LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4029;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
var itemDetailsID = 4026;
var craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

var myPage1 = [11205, 11206, 11207, 11208, 11209];	// Materials
var myPage2 = [11210, 11211, 11212, 11213, 11214, 11215, 11216, 11217]; // Ammunition
var myPage3 = [11218, 11219, 11220]; // Weapons

function page1( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 1 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
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

		myGump.AddText( 255, 60 + ( index * 20 ), LabelHue, GetDictionaryEntry( myPage1[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2100 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page2( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 2 );
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

		myGump.AddText( 255, 60 + ( index * 20 ), LabelHue, GetDictionaryEntry( myPage2[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2200 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page3( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 3 );
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

		myGump.AddText( 255, 60 + ( index * 20 ), LabelHue, GetDictionaryEntry( myPage3[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2300 + i );
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
			pUser.SetTag( "MAKELAST", null );
			pUser.SetTag( "CRAFT", null )
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
		case 100:
			MakeItem( pSock, pUser, 190 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 100 );
			break;//Kindling
		case 101:
			MakeItem( pSock, pUser, 194 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 101 );
			break;//Shaft
		case 102:
			MakeItem( pSock, pUser, 195 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 102 );
			break;//Five Shafts
		case 103:
			MakeItem( pSock, pUser, 196 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 103 );
			break;//Twenty Shafts
		case 104:
			MakeItem( pSock, pUser, 197 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 104 );
			break;//Fifty Shafts
		case 200:
			MakeItem( pSock, pUser, 198 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 200 );
			break;//Arrow
		case 201:
			MakeItem( pSock, pUser, 199 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 201 );
			break;//Five Arrows
		case 202:
			MakeItem( pSock, pUser, 200 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 202 );
			break;//Twenty Arrows
		case 203:
			MakeItem( pSock, pUser, 201 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 203 );
			break;//Fifty Arrows
		case 204:
			MakeItem( pSock, pUser, 202 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 204 );
			break;//Bolt
		case 205:
			MakeItem( pSock, pUser, 203 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 205 );
			break;//Five Bolts
		case 206:
			MakeItem( pSock, pUser, 204 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 206 );
			break;//Twenty Bolts
		case 207:
			MakeItem( pSock, pUser, 205 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 207 );
			break;//Fifty Bolts
		case 300:
			MakeItem( pSock, pUser, 191 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 300 );
			break;//Bow
		case 301:
			MakeItem( pSock, pUser, 192 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 301 );
			break;//Crossbow
		case 302:
			MakeItem( pSock, pUser, 193 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 302 );
			break;//Heavy Crossbow
		case 2100:
			pUser.SetTag( "ITEMDETAILS", 190 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Kindling
		case 2101:
			pUser.SetTag( "ITEMDETAILS", 194 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Shaft
		case 2102:
			pUser.SetTag( "ITEMDETAILS", 195 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Five Shafts
		case 2103:
			pUser.SetTag( "ITEMDETAILS", 196 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Twenty Shafts
		case 2104:
			pUser.SetTag( "ITEMDETAILS", 197 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Fifty Shafts
		case 2200:
			pUser.SetTag( "ITEMDETAILS", 198 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Arrow
		case 2201:
			pUser.SetTag( "ITEMDETAILS", 199 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Five Arrows
		case 2202:
			pUser.SetTag( "ITEMDETAILS", 200 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Twenty Arrows
		case 2203:
			pUser.SetTag( "ITEMDETAILS", 201 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Fifty Arrows
		case 2204:
			pUser.SetTag( "ITEMDETAILS", 202 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Bolt
		case 2205:
			pUser.SetTag( "ITEMDETAILS", 203 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Five Bolts
		case 2206:
			pUser.SetTag( "ITEMDETAILS", 204 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Twenty Bolts
		case 2207:
			pUser.SetTag( "ITEMDETAILS", 205 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Fifty Bolts
		case 2300:
			pUser.SetTag( "ITEMDETAILS", 191 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Bow
		case 2301:
			pUser.SetTag( "ITEMDETAILS", 192 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Crossbow
		case 2302:
			pUser.SetTag( "ITEMDETAILS", 193 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Heavy Crossbow
		case 5000:
			switch ( pUser.GetTag( "MAKELAST" ) )
			{
				case 100:
					MakeItem( pSock, pUser, 190 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 100 );
					break;//Kindling
				case 101:
					MakeItem( pSock, pUser, 194 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 101 );
					break;//Shaft
				case 102:
					MakeItem( pSock, pUser, 195 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 102 );
					break;//Five Shafts
				case 103:
					MakeItem( pSock, pUser, 196 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 103 );
					break;//Twenty Shafts
				case 104:
					MakeItem( pSock, pUser, 197 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 104 );
					break;//Fifty Shafts
				case 200:
					MakeItem( pSock, pUser, 198 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 200 );
					break;//Arrow
				case 201:
					MakeItem( pSock, pUser, 199 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 201 );
					break;//Five Arrows
				case 202:
					MakeItem( pSock, pUser, 200 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 202 );
					break;//Twenty Arrows
				case 203:
					MakeItem( pSock, pUser, 201 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 203 );
					break;//Fifty Arrows
				case 204:
					MakeItem( pSock, pUser, 202 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 204 );
					break;//Bolt
				case 205:
					MakeItem( pSock, pUser, 203 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 205 );
					break;//Five Bolts
				case 206:
					MakeItem( pSock, pUser, 204 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 206 );
					break;//Twenty Bolts
				case 207:
					MakeItem( pSock, pUser, 205 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 207 );
					break;//Fifty Bolts
				case 300:
					MakeItem( pSock, pUser, 191 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 300 );
					break;//Bow
				case 301:
					MakeItem( pSock, pUser, 192 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 301 );
					break;//Crossbow
				case 302:
					MakeItem( pSock, pUser, 193 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 302 );
					break;//Heavy Crossbow
			}break;
	}
}