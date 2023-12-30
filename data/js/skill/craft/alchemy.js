const textHue = 0x480;				// Color of the text.
const scriptID = 4028;				// Use this to tell the gump what script to close.
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
	// Page 1 - Healing and Curative
	[10908, 10909, 10910, 10911, 10912, 10913, 10914, 10915],

	// Page 2 - Enhancement
	[10916, 10917, 10918, 10919, 10920],

	// Page 3 - Toxic
	[10921, 10922, 10923, 10924],

	// Page 4 - Explosives
	[10925, 10926, 10927]
];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 4
	var myGump = new Gump;
	pUser.SetTempTag( "page", pageNum );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	for( var i = 0; i < myPage[pageNum - 1].length; i++ )
	{
		var index = i % 10;
		if( index == 0 )
		{
			if( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage(( i / 10 ) + 1 );

			if( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );// PREV PAGE
			}
		}
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, ( 100 * pageNum ) + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage[pageNum - 1][i], socket.language ));

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, ( 2000 + ( 100 * pageNum )) + i );
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
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
			TriggerEvent( scriptID, "PageX", socket, pUser, timerID );
			break;
		default:
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

	var gumpID = scriptID + 0xffff;
	var makeID = 0;
	var resourceHue = 0;
	var recipeID = 0;
	var itemDetailsID = 0;
	var timerID = 0;

	if(( pButton >= 100 && pButton <= 402 ) || pButton == 5000 )
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

	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;// abort and do nothing
		case 1: // Page 1
		case 2: // Page 2
		case 3: // Page 3
		case 4: // Page 4
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "PageX", pSock, pUser, pButton );
			break;
		// Make Items
		case 100: // Refresh
			makeID = 305; timerID = 1; break;
		case 101: // Greater Refresh
			makeID = 306; timerID = 1; break;
		case 102: // Lesser Heal
			makeID = 298; timerID = 1; break;
		case 103: // Heal
			makeID = 299; timerID = 1; break;
		case 104: // Greater Heal
			makeID = 300; timerID = 1; break;
		case 105: // Lesser Cure
			makeID = 292; timerID = 1; break;
		case 106: // Cure
			makeID = 293; timerID = 1; break;
		case 107: // Greater Cure
			makeID = 294; timerID = 1; break;
		case 200: // Agility
			makeID = 290; timerID = 2; break;
		case 201: // Greater Agility
			makeID = 291; timerID = 2; break;
		case 202: // Night Sight
			makeID = 309; timerID = 2; break;
		case 203: // Strength
			makeID = 307; timerID = 2; break;
		case 204: // Greater Strength
			makeID = 308; timerID = 2; break;
		case 300: // Lesser Poison
			makeID = 301; timerID = 3; break;
		case 301: // Poison
			makeID = 302; timerID = 3; break;
		case 302: // Greater Poison
			makeID = 303; timerID = 3; break;
		case 303: // Deadly Poison
			makeID = 304; timerID = 3; break;
		case 400: // Lesser Explosion
			makeID = 295; timerID = 4; break;
		case 401: // Explosion
			makeID = 296; timerID = 4; break;
		case 402: // Greater Explosion
			makeID = 297; timerID = 4; break;
		// Show Item Details
		case 2100: // Item Details - Refresh
			itemDetailsID = 305; break;
		case 2101: // Item Details - Greater Refreshment
			itemDetailsID = 306; break;
		case 2102: // Item Details - Lesser Heal
			itemDetailsID = 298; break;
		case 2103: // Item Details - Heal
			itemDetailsID = 299; break;
		case 2104: // Item Details - Greater Heal
			itemDetailsID = 300; break;
		case 2105: // Item Details - Lesser Cure
			itemDetailsID = 292; break;
		case 2106: // Item Details - Cure
			itemDetailsID = 293; break;
		case 2107: // Item Details - Greater Cure
			itemDetailsID = 294; break;
		case 2200: // Item Details - Agility
			itemDetailsID = 290; break;
		case 2201: // Item Details - Greater Agility
			itemDetailsID = 291; break;
		case 2202: // Item Details - Night Sight
			itemDetailsID = 309; break;
		case 2203: // Item Details - Strength
			itemDetailsID = 307; break;
		case 2204: // Item Details - Greater Strength
			itemDetailsID = 308; break;
		case 2300: // Item Details - Lesser Poison
			itemDetailsID = 301; break;
		case 2301: // Item Details - Poison
			itemDetailsID = 302; break;
		case 2302: // Item Details - Greater Poison
			itemDetailsID = 303; break;
		case 2303: // Item Details - Deadly Poison
			itemDetailsID = 304; break;
		case 2400: // Item Details - Lesser Explosion
			itemDetailsID = 295; break;
		case 2401: // Item Details - Explosion
			itemDetailsID = 296; break;
		case 2402: // Item Details - Greater Explosion
			itemDetailsID = 297; break;
		default:
			break;
	}

	TriggerEvent( 4039, "makeitem", pSock, bItem, makeID, resourceHue, recipeID, null, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID );
}