const LabelHue = 0x480;				// Color of the text.
const LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4034;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const itemDetailsScriptID = 4026;
const craftGumpID = 4027;
const manualMillTarget = true;		// If true, player must manually target mills when grinding wheat

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

const myPage = [
	// Page 1 - Ingredients
	[ 11606, 11607, 11608, 11609, 11610 ],

	// Page 2 - Preparations
	[ 11611, 11612, 11613, 11614, 11615, 11616, 11617, 11618 ],

	// Page 3 - Baking
	[11619, 11620, 11621, 11657, 11622, 11623, 11624, 11625, 11626, 11627, 11628, 11629 ],

	// Page 4 - Barbecue
	[ 11630, 11631, 11632, 11633, 11634, 11635 ]

];

function PageX( socket, pUser, pageNum )
{
	// Pages 1 - 4
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

		myGump.AddText( 255, 60 + ( index * 20 ), LabelHue, GetDictionaryEntry( myPage[pageNum - 1][i], socket.language ));

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2000 + ( 100 * pageNum ) + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

const mills = [
		0x188b, 0x1893, 0x1920, 0x1922, 0x192c, 0x192e
];
function FindNearbyMills( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( mills.indexOf( trgItem.id ) != -1 );
}

const ovens = [
		0x461, 0x46F, 0x92B, 0x93F
];
function FindNearbyOvens( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( ovens.indexOf( trgItem.id ) != -1 );
}

const heatSources = [
		0x0461, 0x0480, 0x092B, 0x0933, 0x0937, 0x0942, 0x0945, 0x0950, 0x0953,
		0x095e, 0x0961, 0x096c, 0x0de3, 0x0de8, 0x0fac
];
function FindNearbyHeatSources( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( heatSources.indexOf( trgItem.id ) != -1 );
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;

	switch ( timerID )
	{
		case 1: // Page 1 - Ingredients
		case 2: // Page 2 - Preparation
		case 3: // Page 3 - Baking
		case 4: // Page 4 - Barbecue
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
	var itemDetailsID = 0;
	var timerID = 0;

	// Check for nearby heatsource
	var nearbyHeatSource = 0;
	var nearbyMill = 0;
	var nearbyOven = 0;

	// Make Last Button
	if( pButton == 5000 )
	{
		pButton = pUser.GetTempTag( "MAKELAST" );
	}

	if( pButton >= 300 && pButton <= 311 )
	{
		// Baking - Requires Oven
		nearbyOven = AreaItemFunction( "FindNearbyOvens", pUser, 2, pSock );
		if( nearbyOven > 0 )
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
		else
		{
			// No dynamic oven found nearby! Look for a static one?
			var staticFound = false;
			for( var i = 0; i < ovens.length; i++ )
			{
				if( StaticInRange( pUser.x, pUser.y, pUser.worldnumber, 2, ovens[i] ))
				{
					staticFound = true;
					break;
				}
			}
			if( !staticFound )
			{
				pUser.SetTempTag( "prevActionResult", "NOOVEN" );
			}
		}
	}
	else if( pButton >= 400 && pButton <= 405 )
	{
		// Barbecue - Requires Fire/Generic Heat Source
		nearbyHeatSource = AreaItemFunction( "FindNearbyHeatSources", pUser, 2, pSock );
		if( nearbyHeatSource > 0 )
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
		else
		{
			// No dynamic heat source found nearby! Look for a static one?
			var staticFound = false;
			for( var i = 0; i < heatSources.length; i++ )
			{
				if( StaticInRange( pUser.x, pUser.y, pUser.worldnumber, 2, heatSources[i] ))
				{
					staticFound = true;
					break;
				}
			}
			if( !staticFound )
			{
				pUser.SetTempTag( "prevActionResult", "NOHEATSOURCE" );
			}
		}
	}
	else if( pButton >= 101 && pButton <= 207 )
	{
		// Ingredients and Preparation - no heatsource required
		pUser.SetTempTag( "MAKELAST", pButton );
	}

	switch ( pButton )
	{
		case 0: // Abort and do nothing
			pUser.SetTempTag( "MAKELAST", null );
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1: // Page 1 - Ingredients
		case 2: // Page 2 - Preparation
		case 3: // Page 3 - Baking
		case 4: // Page 4 - Barbecue
			pSock.CloseGump( gumpID, 0 );
			PageX( pSock, pUser, pButton );
			break;
		// Make Items
		// Page 1 - Ingredients
		case 100: // Sack of Flour
			makeID = 1500; timerID = 1; break;
		case 101: // Dough
			makeID = 1501; timerID = 1; break;
		case 102: // Sweet Dough
			makeID = 1502; timerID = 1; break;
		case 103: // Cake Mix
			makeID = 1503; timerID = 1; break;
		case 104: // Cookie Mix
			makeID = 1504; timerID = 1; break;
		// Page 2 - Preparation
		case 200: // Unbaked Quiche
			makeID = 1550; timerID = 2; break;
		case 201: // Unbaked Meat Pie
			makeID = 1551; timerID = 2; break;
		case 202: // Uncooked Sausage Pizza
			makeID = 1552; timerID = 2; break;
		case 203: // Uncooked Cheese Pizza
			makeID = 1553; timerID = 2; break;
		case 204: // Unbaked Fruit Pie
			makeID = 1554; timerID = 2; break;
		case 205: // Unbaked Peach Cobbler
			makeID = 1555; timerID = 2; break;
		case 206: // Unbaked Applie Pie
			makeID = 1556; timerID = 2; break;
		case 207: // Unbaked Pumpkin Pie
			makeID = 1557; timerID = 2; break;
		// Page 3 - Baking
		case 300: // Bread Loaf
			makeID = 1600; timerID = 3; break;
		case 301: // Pan of Cookies
			makeID = 1601; timerID = 3; break;
		case 302: // Cake
			makeID = 1602; timerID = 3; break;
		case 303: // Muffins
			makeID = 1603; timerID = 3; break;
		case 304: // Baked Quiche
			makeID = 1604; timerID = 3; break;
		case 305: // Baked Meat Pie
			makeID = 1605; timerID = 3; break;
		case 306: // Sausage Pizza
			makeID = 1606; timerID = 3; break;
		case 307: // Cheese Pizza
			makeID = 1607; timerID = 3; break;
		case 308: // Baked Fruit Pie
			makeID = 1608; timerID = 3; break;
		case 309: // Baked Peach Cobbler
			makeID = 1609; timerID = 3; break;
		case 310: // Baked Applie Pie
			makeID = 1610; timerID = 3; break;
		case 311: // Baked Pumpkin Pie
			makeID = 1611; timerID = 3; break;
		// Page 4 - Barbecue
		case 400: // Cooked Bird
			makeID = 1650; timerID = 4; break;
		case 401: // Chicken Leg
			makeID = 1651; timerID = 4; break;
		case 402: // Fish Steak
			makeID = 1652; timerID = 4; break;
		case 403: // Fried Eggs
			makeID = 1653; timerID = 4; break;
		case 404: // Leg of Lamb
			makeID = 1654; timerID = 4; break;
		case 405: // Cut of Ribs
			makeID = 1655; timerID = 4; break;
		// Show Item Details
		case 2100: // Sack of Flour
			itemDetailsID = 1500; break;
		case 2101: // Dough
			itemDetailsID = 1501; break;
		case 2102: // Sweet Dough
			itemDetailsID = 1502; break;
		case 2103: // Cake Mix
			itemDetailsID = 1503; break;
		case 2104: // Cookie Mix
			itemDetailsID = 1504; break;
		case 2200: // Unbaked Quiche
			itemDetailsID = 1550; break;
		case 2201: // Unbaked Meat Pie
			itemDetailsID = 1551; break;
		case 2202: // Uncooked Sausage Pizza
			itemDetailsID = 1552; break;
		case 2203: // Uncooked Cheese Pizza
			itemDetailsID = 1553; break;
		case 2204: // Unbaked Fruit Pie
			itemDetailsID = 1554; break;
		case 2205: // Unbaked Peach Cobbler
			itemDetailsID = 1555; break;
		case 2206: // Unbaked Apple Pie
			itemDetailsID = 1556; break;
		case 2207: // Unbaked Pumpkin Pie
			itemDetailsID = 1557; break;
		case 2300: // Bread Loaf
			itemDetailsID = 1600; break;
		case 2301: // Pan of Cookies
			itemDetailsID = 1601; break;
		case 2302: // Cake
			itemDetailsID = 1602; break;
		case 2303: // Muffins
			itemDetailsID = 1603; break;
		case 2304: // Baked Quiche
			itemDetailsID = 1604; break;
		case 2305: // Baked Meat Pie
			itemDetailsID = 1605; break;
		case 2306: // Sausage Pizza
			itemDetailsID = 1606; break;
		case 2307: // Cheese Pizza
			itemDetailsID = 1607; break;
		case 2308: // Baked Fruit Pie
			itemDetailsID = 1608; break;
		case 2309: // Baked Peach Cobbler
			itemDetailsID = 1609; break;
		case 2310: // Baked Apple Pie
			itemDetailsID = 1610; break;
		case 2311: // Baked Pumpkin Pie
			itemDetailsID = 1611; break;
		case 2400: // Cooked Bird
			itemDetailsID = 1650; break;
		case 2401: // Chicken Leg
			itemDetailsID = 1651; break;
		case 2402: // Fish Steak
			itemDetailsID = 1652; break;
		case 2403: // Fried Eggs
			itemDetailsID = 1653; break;
		case 2404: // Leg of Lamb
			itemDetailsID = 1654; break;
		case 2405: // Cut of Ribs
			itemDetailsID = 1655; break;
		default:
			break;
	}

	if( makeID != 0 )
	{
		var makeItem = false;
		if( makeID == 1500 )
		{
			// Grind wheat to flour
			if( manualMillTarget )
			{
				// Require player to manually target mill
				var pack = pUser.pack;
				var packItem;
				for( packItem = pack.FirstItem(); !pack.FinishedItems(); packItem = pack.NextItem() )
				{
					if( ValidateObject( packItem ) && packItem.id == 0x1ebd )
					{
						// Found wheat item in inventory, ask user where to grind it
						TriggerEvent( 101, "onUseChecked", pUser, packItem ); // 101 = wheat.js
						return;
					}
				}
			}
			else
			{
				// Look for nearby dynamic mill
				var nearbyMill = AreaItemFunction( "FindNearbyMills", pUser, 2, pSock );
				if( nearbyMill == 0 )
				{
					// No dynamic oven found nearby! Look for a static one?
					var staticFound = false;
					for( var i = 0; i < ovens.length; i++ )
					{
						if( StaticInRange( pUser.x, pUser.y, pUser.worldnumber, 2, ovens[i] ))
						{
							nearbyMill = 1;
							break;
						}
					}
				}

				if( nearbyMill > 0 )
				{
					makeItem = true;
					pUser.SetTempTag( "MAKELAST", pButton );
				}
				else
				{
					pUser.SetTempTag( "prevActionResult", "NOMILL" );
				}
			}
		}
		else if( makeID >= 1650 && makeID <= 1655 && nearbyHeatSource > 0 )
		{
			// Barbecue
			makeItem = true;
		}
		else if( makeID >= 1600 && makeID <= 1611 && nearbyOven > 0 )
		{
			// Baking
			makeItem = true;
		}
		else if( makeID >= 1501 && makeID <= 1557 )
		{
			// Ingredients/Baking - no heatsource or mill required
			makeItem = true;
		}

		if( makeItem )
		{
			MakeItem( pSock, pUser, makeID );
			if( GetServerSetting( "ToolUseLimit" ))
			{
				bItem.usesLeft -= 1;
				if( bItem.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
				{
					bItem.Delete();
					pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language )); // You have worn out your tool!
					// Play sound effect of tool breaking
				}
			}
		}
		pUser.StartTimer( gumpDelay, timerID, true );
	}
	else if( itemDetailsID != 0 )
	{
		pUser.SetTempTag( "ITEMDETAILS", itemDetailsID );
		TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
	}
}

function _restorecontext_() {}
