const LabelHue = 0x480;				// Color of the text.
const LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4036;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const itemDetailsScriptID = 4026;
const craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

const myPage = [
	// Page 1 - Misc
	[13600, 13601, 13602, 13603, 13604, 13605, 13606, 13607, 13608, 13609, 13610, 13611 ]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1
	var myGump = new Gump;
	pUser.SetTempTag( "page", pageNum );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	for ( var i = 0; i < myPage[pageNum - 1].length; i++ )
	{
		var index = i % 10;
		if ( index == 0 )
		{
			if ( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage( ( i / 10 ) + 1 );

			if ( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, ( 100 * pageNum ) + i );

		myGump.AddText( 255, 60 + ( index * 20 ), LabelHue, GetDictionaryEntry( myPage[pageNum - 1][i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2000 + ( 100 * pageNum ) + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;

	switch( timerID )
	{
		case 1: // Page 1 - Maps
			PageX( socket, pUser, timerID );
			break;
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	// Don't continue if player no longer has access to the crafting tool
	var bItem = pSock.tempObj;
	if( !ValidateObject( bItem ) || !pUser.InRange( bItem, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
		return;
	}

	if( bItem.movable == 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language )); // Locked down resources cannot be used!
		return;
	}

	var iPackOwner = GetPackOwner( bItem, 0 );
	if( ValidateObject( iPackOwner )) // Is the item in a backpack?
	{
		if( iPackOwner.serial != pUser.serial ) // And if so does the pack belong to the user?
		{
			pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language )); // This has to be in your backpack before you can use it.
		return;
	}

	var gumpID = scriptID + 0xffff;
	var makeID = 0;
	var resourceHue = 0;
	var recipeID = 0;
	var itemDetailsID = 0;
	var timerID = 0;

	if(( pButton >= 100 && pButton <= 305 ) || pButton == 5000 )
	{
		if( pButton == 5000 )
		{
			// Make Last button
			pButton = pUser.GetTempTag( "MAKELAST" );
		}
		else
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
	}

	switch ( pButton )
	{
		case 0: // Abort and do nothing
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1: // Page 1 - Maps
			pSock.CloseGump( gumpID, 0 );
			PageX( pSock, pUser, pButton );
			break;
		// Make Items
		case 100: // empty bottle
			makeID = 3000; timerID = 1; break;
		case 101: // flask (small)
			makeID = 3001; timerID = 1; break;
		case 102: // flask (medium)
			makeID = 3002; timerID = 1; break;
		case 103: // flask (curved)
			makeID = 3003; timerID = 1; break;
		case 103: // flask (large #1)
			makeID = 3004; timerID = 1; break;
		case 103: // flask (large #2)
			makeID = 3005; timerID = 1; break;
		case 103: // flask (bubbling blue)
			makeID = 3006; timerID = 1; break;
		case 103: // flask (bubbling purple)
			makeID = 3007; timerID = 1; break;
		case 103: // flask (bubbling red)
			makeID = 3008; timerID = 1; break;
		case 103: // empty vials
			makeID = 3009; timerID = 1; break;
		case 103: // full vials
			makeID = 3010; timerID = 1; break;
		case 103: // spinning hourglass
			makeID = 3011; timerID = 1; break;
		// Show Item Details
		case 2100: // empty bottle
			itemDetailsID = 3000; break;
		case 2101: // flask (small)
			itemDetailsID = 3001; break;
		case 2102: // flask (medium)
			itemDetailsID = 3002; break;
		case 2103: // flask (curved)
			itemDetailsID = 3003; break;
		case 2104: // flask (large #1)
			itemDetailsID = 3004; break;
		case 2105: // flask (large #2)
			itemDetailsID = 3005; break;
		case 2106: // flask (bubbling blue)
			itemDetailsID = 3006; break;
		case 2107: // flask (bubbling purple)
			itemDetailsID = 3007; break;
		case 2108: // flask (bubbling red)
			itemDetailsID = 3008; break;
		case 2109: // empty vials
			itemDetailsID = 3009; break;
		case 2110: // full vials
			itemDetailsID = 3010; break;
		case 2111: // spinning hourglass
			itemDetailsID = 3011; break;
		default:
			break;
	}

	TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, resourceHue, recipeID, null, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID );
}