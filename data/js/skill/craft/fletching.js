const LabelHue = 0x480;				// Color of the text.
const LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4029;				// Use this to tell the gump what script to close.
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
	// Page 1 - Materials
	[ 11205, 11206, 11207, 11208, 11209 ],

	// Page 2 - Ammunition
	[ 11210, 11211, 11212, 11213, 11214, 11215, 11216, 11217 ],

	// Page 3 - Weapons
	[ 11218, 11219, 11220 ]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 3
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

	switch ( timerID )
	{
		case 1: // Page 1 - Materials
		case 2: // Page 2 - Ammunition
		case 3: // Page 3 - Weapons
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

	if(( pButton >= 100 && pButton <= 302 ) || pButton == 5000 )
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
		case 1: // Page 1 - Materials
		case 2: // Page 2 - Ammunition
		case 3: // Page 3 - Weapons
			pSock.CloseGump( gumpID, 0 );
			PageX( pSock, pUser, pButton );
			break;
		// Make Items
		case 100: // Kindling
			makeID = 190; timerID = 1; break;
		case 101: // Shaft
			makeID = 194; timerID = 1; break;
		case 102: // Five Shafts
			makeID = 195; timerID = 1; break;
		case 103: // Twenty Shafts
			makeID = 196; timerID = 1; break;
		case 104: // Fifty Shafts
			makeID = 197; timerID = 1; break;
		case 200: // Arrow
			makeID = 198; timerID = 2; break;
		case 201: // Five Arrows
			makeID = 199; timerID = 2; break;
		case 202: // Twenty Arrows
			makeID = 200; timerID = 2; break;
		case 203: // Fifty Arrows
			makeID = 201; timerID = 2; break;
		case 204: // Bolt
			makeID = 202; timerID = 2; break;
		case 205: // Five Bolts
			makeID = 203; timerID = 2; break;
		case 206: // Twenty Bolts
			makeID = 204; timerID = 2; break;
		case 207: // Fifty Bolts
			makeID = 205; timerID = 2; break;
		case 300: // Bow
			makeID = 191; timerID = 3; break;
		case 301: // Crossbow
			makeID = 192; timerID = 3; break;
		case 302: // Heavy Crossbow
			makeID = 193; timerID = 3; break;
		// Show Item Details
		case 2100: // Kindling
			itemDetailsID = 190; break;
		case 2101: // Shaft
			itemDetailsID = 194; break;
		case 2102: // Five Shafts
			itemDetailsID = 195; break;
		case 2103: // Twenty Shafts
			itemDetailsID = 196; break;
		case 2104: // Fifty Shafts
			itemDetailsID = 197; break;
		case 2200: // Arrow
			itemDetailsID = 198; break;
		case 2201: // Five Arrows
			itemDetailsID = 199; break;
		case 2202: // Twenty Arrows
			itemDetailsID = 200; break;
		case 2203: // Fifty Arrows
			itemDetailsID = 201; break;
		case 2204: // Bolt
			itemDetailsID = 202; break;
		case 2205: // Five Bolts
			itemDetailsID = 203; break;
		case 2206: // Twenty Bolts
			itemDetailsID = 204; break;
		case 2207: // Fifty Bolts
			itemDetailsID = 205; break;
		case 2300: // Bow
			itemDetailsID = 191; break;
		case 2301: // Crossbow
			itemDetailsID = 192; break;
		case 2302: // Heavy Crossbow
			itemDetailsID = 193; break;
		default:
			break;
	}

	TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, resourceHue, recipeID, null, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID );
}