var textHue = 0x480;				// Color of the text.
const scriptID = 4025;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const repairDelay = 200;			// Timer for the gump to reapear after selecting a resource.
var itemDetailsID = 4026;
var craftGumpID = 4027;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the item to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

var myPage1 = [10611, 10612, 10613, 10614, 10615, 10616, 10617, 10618, 10619, 10620];	// Other
var myPage2 = [10621, 10622, 10623, 10624, 10625, 10626, 10627, 10628, 10629, 10630, 10631, 10632, 10633]; // Furniture
var myPage3 = [10634, 10635, 10636, 10637, 10638, 10639, 10640, 10641, 10642]; // Containers
var myPage4 = [10643, 10644, 10645, 10646, 10647]; // Weapons
var myPage5 = [10648]; // Armor
var myPage6 = [10649, 10650, 10651, 10652, 10653, 10654]; // Instruments
var myPage7 = [10655, 10656, 10657, 10658, 10659, 10660, 10661, 10662, 10663, 10664, 10665]; // Misc. Add-Ons
var myPage8 = [10666, 10667, 10668, 10669, 10670, 10671, 10672, 10673, 10674, 10675, 10676, 10677]; // Tailoring and Cooking
var myPage9 = [10678, 10679, 10680, 10681, 10682]; // Anvil and Forges
var myPage10 = [10683, 10684, 10685, 10686]; // Training

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

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage1[i], socket.language ) );

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

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage2[i], socket.language ) );

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

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage3[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2300 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page4( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 4 );
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

function page5( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 5 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage5.length; i++ )
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
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 500 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage5[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2500 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page6( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 6 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage6.length; i++ )
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
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 600 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage6[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2600 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page7( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 7 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage7.length; i++ )
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
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 700 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage7[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2700 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page8( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 8 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage8.length; i++ )
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
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 800 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage8[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2800 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page9( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 9 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage9.length; i++ )
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
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 900 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage9[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2900 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function page10( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTag( "page", 10 );
	TriggerEvent( craftGumpID, "craftinggump", myGump, socket );
	for ( var i = 0; i < myPage10.length; i++ )
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
		myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 1000 + i );

		myGump.AddText( 255, 60 + ( index * 20 ), textHue, GetDictionaryEntry( myPage10[i], socket.language ) );

		myGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 2950 + i );
	}
	myGump.Send( socket );
	myGump.Free();
}

function target( pSock )
{
	var targMsg = GetDictionaryEntry( 485, pSock.Language );
	pSock.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{//Repair Item
	var mChar = socket.currentChar;
	var bItem = socket.tempObj;
	var gumpID = scriptID + 0xffff;
	//socket.tempObj = null;

	if ( mChar && mChar.isChar && bItem && bItem.isItem )
	{
		if ( !ourObj || !ourObj.isItem )
		{
			socket.tempObj = bItem;
			socket.CloseGump( gumpID, 0 );
			mChar.SetTag( "CANTREPAIR", 1 );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}
		var minSkill = 999;
		var maxSkill = 1000;
		var repairID = ourObj.id;
		if ( repairids( repairID ) )
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if ( ownerObj && mChar.serial == ownerObj.serial )
			{
				if ( ourObj.health < ourObj.maxhp )
				{
					if ( ourObj.def > 0 )
					{
						// Items with > 12 def would have impossible skill req's, with this equation
						if ( ourObj.def <= 12 ) {
							// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 ( 0-3 = 61.0, 4-6 = 71.0, ect )
							minSkill = ( 610 + ( ( ourObj.def - 1 ) / 3 ) * 100 );
							// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 ( 0-3 = 84.9, 4-6 = 89.9, ect )
							maxSkill = ( 849 + ( ( ourObj.def - 1 ) / 3 ) * 50 );
						}
					}
					else if ( ourObj.hidamage > 0 )
					{
						var offset = ( ( ourObj.lodamage + ourObj.hidamage ) / 2 );
						// Items with > 25 Avg Damage would have impossible skill req's, with this equation
						if ( offset <= 25 ) {
							minSkill = ( 510 + ( ( offset - 1 ) / 5 ) * 100 );
							maxSkill = ( 799 + ( ( offset - 1 ) / 5 ) * 50 );
						}
					}
					else
					{
						socket.tempObj = bItem;
						socket.CloseGump( gumpID, 0 );
						mChar.SetTag( "CANTREPAIR", 1 );
						mChar.StartTimer( repairDelay, 1, true );
						return;
					}

					if ( mChar.CheckSkill( 8, minSkill, maxSkill ) )
					{
						socket.CloseGump( gumpID, 0 );
						ourObj.health = ourObj.maxhp;
						mChar.SetTag( "REPAIRSUCCESS", 1 );
						socket.SoundEffect( 0x002A, true );
						mChar.StartTimer( repairDelay, 1, true );
					}
					else
					{
						socket.tempObj = bItem;
						socket.CloseGump( gumpID, 0 );
						mChar.SetTag( "FAILREPAIR", 1 );
						mChar.StartTimer( repairDelay, 1, true );
					}
				}
				else
				{
					socket.tempObj = bItem;
					socket.CloseGump( gumpID, 0 );
					mChar.SetTag( "FULLREPAIR", 1 );
					mChar.StartTimer( repairDelay, 1, true );
				}
			}
			else
			{
				socket.tempObj = bItem;
				socket.CloseGump( gumpID, 0 );
				mChar.SetTag( "CHECKPACK", 1 );
				mChar.StartTimer( repairDelay, 1, true );
			}
		}
		else
		{
			socket.tempObj = bItem;
			socket.CloseGump( gumpID, 0 );
			mChar.SetTag( "CANTREPAIR", 1 );
			mChar.StartTimer( repairDelay, 1, true );
		}
	}
}

function repairids( repairID )
{
	return ( repairID == 0x0F43 || repairID == 0x0F44 || repairID == 0x0F45 || repairID == 0x0F46 ||
		repairID == 0x0F47 || repairID == 0x0F48 || repairID == 0x0F49 || repairID == 0x0F4A ||
		repairID == 0x0F4B || repairID == 0x0F4C || repairID == 0x0F4E || repairID == 0x0F4E ||
		repairID == 0x0F51 || repairID == 0x0F52 || repairID == 0x0F5C || repairID == 0x0F5D ||
		repairID == 0x0F5E || repairID == 0x0F5F || repairID == 0x0F60 || repairID == 0x0F61 ||
		repairID == 0x0F62 || repairID == 0x0F63 || repairID == 0x13AF || repairID == 0x13B0 ||
		repairID == 0x13B5 || repairID == 0x13B6 || repairID == 0x13B7 || repairID == 0x13B8 ||
		repairID == 0x13B9 || repairID == 0x13BA || repairID == 0x13F6 || repairID == 0x13F7 ||
		repairID == 0x13FA || repairID == 0x13FB || repairID == 0x13FC || repairID == 0x13FD ||
		repairID == 0x13FE || repairID == 0x13FF || repairID == 0x1400 || repairID == 0x1401 ||
		repairID == 0x1402 || repairID == 0x1403 || repairID == 0x1404 || repairID == 0x1405 ||
		repairID == 0x1406 || repairID == 0x1407 || repairID == 0x1438 || repairID == 0x1439 ||
		repairID == 0x143A || repairID == 0x143B || repairID == 0x143C || repairID == 0x143D ||
		repairID == 0x143E || repairID == 0x143F || repairID == 0x1440 || repairID == 0x1441 ||
		repairID == 0x1442 || repairID == 0x1443 || repairID == 0x13BB || repairID == 0x13BC ||
		repairID == 0x13BD || repairID == 0x13BE || repairID == 0x13BF || repairID == 0x13C0 ||
		repairID == 0x13C1 || repairID == 0x13C2 || repairID == 0x13C3 || repairID == 0x13C4 ||
		repairID == 0x13E5 || repairID == 0x13E6 || repairID == 0x13E7 || repairID == 0x13E8 ||
		repairID == 0x13E9 || repairID == 0x13EA || repairID == 0x13EB || repairID == 0x13EC ||
		repairID == 0x13ED || repairID == 0x13EE || repairID == 0x13EF || repairID == 0x13F0 ||
		repairID == 0x13F1 || repairID == 0x13F2 || repairID == 0x1408 || repairID == 0x1409 ||
		repairID == 0x140A || repairID == 0x140B || repairID == 0x140C || repairID == 0x140D ||
		repairID == 0x140E || repairID == 0x140F || repairID == 0x1410 || repairID == 0x1411 ||
		repairID == 0x1412 || repairID == 0x1413 || repairID == 0x1414 || repairID == 0x1415 ||
		repairID == 0x1416 || repairID == 0x1417 || repairID == 0x1418 || repairID == 0x1419 ||
		repairID == 0x141A );
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
		case 5:
			TriggerEvent( scriptID, "page5", socket, pUser );
			break;
		case 6:
			TriggerEvent( scriptID, "page6", socket, pUser );
			break;
		case 7:
			TriggerEvent( scriptID, "page7", socket, pUser );
			break;
		case 8:
			TriggerEvent( scriptID, "page8", socket, pUser );
			break;
		case 9:
			TriggerEvent( scriptID, "page9", socket, pUser );
			break;
		case 10:
			TriggerEvent( scriptID, "page10", socket, pUser );
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
		case 4:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page4", pSock, pUser );
			break;
		case 5:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page5", pSock, pUser );
			break;
		case 6:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page6", pSock, pUser );
			break;
		case 7:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page7", pSock, pUser );
			break;
		case 8:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page8", pSock, pUser );
			break;
		case 9:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page9", pSock, pUser );
			break;
		case 10:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "page10", pSock, pUser );
			break;
		case 51:
			var targMsg = GetDictionaryEntry( 485, pSock.Language );
			pSock.CustomTarget( 2, targMsg );
			break;
		case 100:
			MakeItem( pSock, pUser, 73 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 100 );
			break;//Barrel Staves
		case 101:
			MakeItem( pSock, pUser, 74 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 101 );
			break;//Barrel Lid
		case 102:
			MakeItem( pSock, pUser, 89 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 102 );
			break;//short music stand ( left )
		case 103:
			MakeItem( pSock, pUser, 90 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 103 );
			break;//short music stand ( right )
		case 104:
			MakeItem( pSock, pUser, 91 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 104 );
			break;//tall music stand ( left )
		case 105:
			MakeItem( pSock, pUser, 92 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 105 );
			break;//tall music stand ( left )
		case 106:
			MakeItem( pSock, pUser, 76 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 106 );
			break;//Easel ( S )
		case 107:
			MakeItem( pSock, pUser, 77 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 107 );
			break;//Easel ( E )
		case 108:
			MakeItem( pSock, pUser, 78 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 108 );
			break;//Easel ( N )
		case 109:
			MakeItem( pSock, pUser, 79 );
			pUser.StartTimer( gumpDelay, 1, true );
			pUser.SetTag( "MAKELAST", 109 );
			break;//Fishing Pole
		case 200:
			MakeItem( pSock, pUser, 50 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 200 );
			break;//Barstool
		case 201:
			MakeItem( pSock, pUser, 51 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 201 );
			break;//Stool
		case 202:
			MakeItem( pSock, pUser, 52 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 202 );
			break;//Straw Chair
		case 203:
			MakeItem( pSock, pUser, 53 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 203 );
			break;//Wooden Chair
		case 204:
			MakeItem( pSock, pUser, 57 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 204 );
			break;//Vesper Style
		case 205:
			MakeItem( pSock, pUser, 58 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 205 );
			break;//Trinsic Style
		case 206:
			MakeItem( pSock, pUser, 54 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 206 );
			break;//Bench
		case 207:
			MakeItem( pSock, pUser, 55 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 207 );
			break;//Wooden Throne
		case 208:
			MakeItem( pSock, pUser, 56 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 208 );
			break;//Magincian Throne
		case 209:
			MakeItem( pSock, pUser, 59 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 209 );
			break;//Small Table
		case 210:
			MakeItem( pSock, pUser, 60 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 210 );
			break;//Writing Table
		case 211:
			MakeItem( pSock, pUser, 61 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 211 );
			break;//Large Table
		case 212:
			MakeItem( pSock, pUser, 62 );
			pUser.StartTimer( gumpDelay, 2, true );
			pUser.SetTag( "MAKELAST", 212 );
			break;//Yew-wood Table
		case 300:
			MakeItem( pSock, pUser, 63 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 300 );
			break;//Small Chest
		case 301:
			MakeItem( pSock, pUser, 64 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 301 );
			break;//Small Crate
		case 302:
			MakeItem( pSock, pUser, 65 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 302 );
			break;//Medium Crate
		case 303:
			MakeItem( pSock, pUser, 67 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 303 );
			break;//Large Crate
		case 304:
			MakeItem( pSock, pUser, 68 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 304 );
			break;//Chest
		case 305:
			MakeItem( pSock, pUser, 69 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 305 );
			break;//Bookshelf
		case 306:
			MakeItem( pSock, pUser, 70 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 306 );
			break;//Armoire
		case 307:
			MakeItem( pSock, pUser, 71 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 307 );
			break;//Armoire
		case 308:
			MakeItem( pSock, pUser, 66 );
			pUser.StartTimer( gumpDelay, 3, true );
			pUser.SetTag( "MAKELAST", 308 );
			break;//Open Keg
		case 400:
			MakeItem( pSock, pUser, 80 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTag( "MAKELAST", 400 );
			break;//Shepard's Crook
		case 401:
			MakeItem( pSock, pUser, 81 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTag( "MAKELAST", 401 );
			break;//Quarter Staff
		case 402:
			MakeItem( pSock, pUser, 82 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTag( "MAKELAST", 402 );
			break;//Gnarled Staff
		case 403:
			MakeItem( pSock, pUser, 123 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTag( "MAKELAST", 403 );
			break;//Club
		case 404:
			MakeItem( pSock, pUser, 124 );
			pUser.StartTimer( gumpDelay, 4, true );
			pUser.SetTag( "MAKELAST", 404 );
			break;//Black Staff
		case 500:
			MakeItem( pSock, pUser, 75 );
			pUser.StartTimer( gumpDelay, 5, true );
			pUser.SetTag( "MAKELAST", 500 );
			break;//Wooden Shield
		case 600:
			MakeItem( pSock, pUser, 83 );
			pUser.StartTimer( gumpDelay, 6, true );
			pUser.SetTag( "MAKELAST", 600 );
			break;//Lap Harp
		case 601:
			MakeItem( pSock, pUser, 84 );
			pUser.StartTimer( gumpDelay, 6, true );
			pUser.SetTag( "MAKELAST", 601 );
			break;//Standing Harp
		case 602:
			MakeItem( pSock, pUser, 85 );
			pUser.StartTimer( gumpDelay, 6, true );
			pUser.SetTag( "MAKELAST", 602 );
			break;//Drum
		case 603:
			MakeItem( pSock, pUser, 86 );
			pUser.StartTimer( gumpDelay, 6, true );
			pUser.SetTag( "MAKELAST", 603 );
			break;//Lute
		case 604:
			MakeItem( pSock, pUser, 87 );
			pUser.StartTimer( gumpDelay, 6, true );
			pUser.SetTag( "MAKELAST", 604 );
			break;//Tambourine
		case 605:
			MakeItem( pSock, pUser, 88 );
			pUser.StartTimer( gumpDelay, 6, true );
			pUser.SetTag( "MAKELAST", 605 );
			break;//Tambourine ( tassel )
		case 700:
			MakeItem( pSock, pUser, 93 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 700 );
			break;//bulletin board ( east )
		case 701:
			MakeItem( pSock, pUser, 93 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 701 );
			break;//bulletin board ( south )
		case 702:
			MakeItem( pSock, pUser, 94 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 702 );
			break;//Sm Bed ( E )
		case 703:
			MakeItem( pSock, pUser, 93 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 703 );
			break;//Sm Bed ( S )
		case 704:
			MakeItem( pSock, pUser, 96 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 704 );
			break;//Lg Bed ( E )
		case 705:
			MakeItem( pSock, pUser, 95 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 705 );
			break;//Lg Bed ( S )
		case 706:
			MakeItem( pSock, pUser, 97 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 706 );
			break;//Dart Board ( S )
		case 707:
			MakeItem( pSock, pUser, 98 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 707 );
			break;//Dart Board ( E )
		case 708:
			MakeItem( pSock, pUser, 99 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 708 );
			break;//Ballot Box
		case 709:
			MakeItem( pSock, pUser, 100 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 709 );
			break;//Pentagram
		case 710:
			MakeItem( pSock, pUser, 101 );
			pUser.StartTimer( gumpDelay, 7, true );
			pUser.SetTag( "MAKELAST", 710 );
			break;//Abbatoir
		case 800:
			MakeItem( pSock, pUser, 115 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 800 );
			break;//Dressform
		case 801:
			MakeItem( pSock, pUser, 116 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 801 );
			break;//Dressform
		case 802:
			MakeItem( pSock, pUser, 107 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 802 );
			break;//Spin Wheel ( E )
		case 803:
			MakeItem( pSock, pUser, 108 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 803 );
			break;//Spin Wheel ( S )
		case 804:
			MakeItem( pSock, pUser, 109 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 804 );
			break;//Loom ( E )
		case 805:
			MakeItem( pSock, pUser, 110 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 805 );
			break;//Loom ( S )
		case 806:
			MakeItem( pSock, pUser, 117 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 806 );
			break;//Stone Oven ( E )
		case 807:
			MakeItem( pSock, pUser, 118 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 807 );
			break;//Stone Oven ( S )
		case 808:
			MakeItem( pSock, pUser, 119 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 808 );
			break;//Flour Mill ( E )
		case 809:
			MakeItem( pSock, pUser, 120 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 809 );
			break;//Flour Mill ( S )
		case 810:
			MakeItem( pSock, pUser, 121 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 810 );
			break;//Water Trough( E )
		case 811:
			MakeItem( pSock, pUser, 122 );
			pUser.StartTimer( gumpDelay, 8, true );
			pUser.SetTag( "MAKELAST", 811 );
			break;//Water Trough( S )
		case 900:
			MakeItem( pSock, pUser, 102 );
			pUser.StartTimer( gumpDelay, 9, true );
			pUser.SetTag( "MAKELAST", 900 );
			break;//Small Forge
		case 901:
			MakeItem( pSock, pUser, 103 );
			pUser.StartTimer( gumpDelay, 9, true );
			pUser.SetTag( "MAKELAST", 901 );
			break;//Lg Forge ( E )
		case 902:
			MakeItem( pSock, pUser, 104 );
			pUser.StartTimer( gumpDelay, 9, true );
			pUser.SetTag( "MAKELAST", 902 );
			break;//Lg Forge ( S )
		case 903:
			MakeItem( pSock, pUser, 105 );
			pUser.StartTimer( gumpDelay, 9, true );
			pUser.SetTag( "MAKELAST", 903 );
			break;//Anvil ( E )
		case 904:
			MakeItem( pSock, pUser, 106 );
			pUser.StartTimer( gumpDelay, 9, true );
			pUser.SetTag( "MAKELAST", 904 );
			break;//Anvil ( S )
		case 1000:
			MakeItem( pSock, pUser, 111 );
			pUser.StartTimer( gumpDelay, 10, true );
			pUser.SetTag( "MAKELAST", 1000 );
			break;//Dummy ( E )
		case 1001:
			MakeItem( pSock, pUser, 112 );
			pUser.StartTimer( gumpDelay, 10, true );
			pUser.SetTag( "MAKELAST", 1001 );
			break;//Dummy ( S )
		case 1002:
			MakeItem( pSock, pUser, 113 );
			pUser.StartTimer( gumpDelay, 10, true );
			pUser.SetTag( "MAKELAST", 1002 );
			break;//Pickpocket ( E )
		case 1003:
			MakeItem( pSock, pUser, 114 );
			pUser.StartTimer( gumpDelay, 10, true );
			pUser.SetTag( "MAKELAST", 1003 );
			break;//Pickpocket ( S )
		case 2100:
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTag( "ITEMDETAILS", 73 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Barrel Staves
		case 2101:
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTag( "ITEMDETAILS", 74 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Barrel Lid
		case 2102:
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTag( "ITEMDETAILS", 89 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//short music stand ( left )
		case 2103:
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTag( "ITEMDETAILS", 90 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//short music stand ( right )
		case 2104:
			pUser.SetTag( "ITEMDETAILS", 91 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//tall music stand ( left )
		case 2105:
			pUser.SetTag( "ITEMDETAILS", 92 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//tall music stand ( left )
		case 2106:
			pUser.SetTag( "ITEMDETAILS", 76 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Easel ( S )
		case 2107:
			pUser.SetTag( "ITEMDETAILS", 77 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Easel ( E )
		case 2108:
			pUser.SetTag( "ITEMDETAILS", 78 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Easel ( N )
		case 2109:
			pUser.SetTag( "ITEMDETAILS", 79 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Fishing Pole
		case 2200:
			pUser.SetTag( "ITEMDETAILS", 50 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Barstool
		case 2201:
			pUser.SetTag( "ITEMDETAILS", 51 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Stool
		case 2202:
			pUser.SetTag( "ITEMDETAILS", 52 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Straw Chair
		case 2203:
			pUser.SetTag( "ITEMDETAILS", 53 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Wooden Chair
		case 2204:
			pUser.SetTag( "ITEMDETAILS", 57 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Vesper Style
		case 2205:
			pUser.SetTag( "ITEMDETAILS", 58 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Trinsic Style
		case 2206:
			pUser.SetTag( "ITEMDETAILS", 54 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Bench
		case 2207:
			pUser.SetTag( "ITEMDETAILS", 55 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Wooden Throne
		case 2208:
			pUser.SetTag( "ITEMDETAILS", 56 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Magincian Throne
		case 2209:
			pUser.SetTag( "ITEMDETAILS", 59 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			pUser.SetTag( "MAKELAST", 209 );
			break;//Small Table
		case 2210:
			pUser.SetTag( "ITEMDETAILS", 60 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Writing Table
		case 2211:
			pUser.SetTag( "ITEMDETAILS", 61 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Large Table
		case 2212:
			pUser.SetTag( "ITEMDETAILS", 62 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Yew-wood Table
		case 2300:
			pUser.SetTag( "ITEMDETAILS", 63 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Small Chest
		case 2301:
			pUser.SetTag( "ITEMDETAILS", 64 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Small Crate
		case 2302:
			pUser.SetTag( "ITEMDETAILS", 65 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Medium Crate
		case 2303:
			pUser.SetTag( "ITEMDETAILS", 67 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Large Crate
		case 2304:
			pUser.SetTag( "ITEMDETAILS", 68 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Chest
		case 2305:
			pUser.SetTag( "ITEMDETAILS", 69 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Bookshelf
		case 2306:
			pUser.SetTag( "ITEMDETAILS", 70 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Armoire
		case 2307:
			pUser.SetTag( "ITEMDETAILS", 71 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Armoire
		case 2308:
			pUser.SetTag( "ITEMDETAILS", 66 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Open Keg
		case 2400:
			pUser.SetTag( "ITEMDETAILS", 80 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Shepard's Crook
		case 2401:
			pUser.SetTag( "ITEMDETAILS", 81 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Quarter Staff
		case 2402:
			pUser.SetTag( "ITEMDETAILS", 82 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Gnarled Staff
		case 2403:
			pUser.SetTag( "ITEMDETAILS", 123 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Club
		case 2404:
			pUser.SetTag( "ITEMDETAILS", 124 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Black Staff
		case 2500:
			pUser.SetTag( "ITEMDETAILS", 75 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Wooden Shield
		case 2600:
			pUser.SetTag( "ITEMDETAILS", 83 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Lap Harp
		case 2601:
			pUser.SetTag( "ITEMDETAILS", 84 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Standing Harp
		case 2602:
			pUser.SetTag( "ITEMDETAILS", 85 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Drum
		case 2603:
			pUser.SetTag( "ITEMDETAILS", 86 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Lute
		case 2604:
			pUser.SetTag( "ITEMDETAILS", 87 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Tambourine
		case 2605:
			pUser.SetTag( "ITEMDETAILS", 88 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Tambourine ( tassel )
		case 2700:
			pUser.SetTag( "ITEMDETAILS", 93 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//bulletin board ( east )
		case 2701:
			pUser.SetTag( "ITEMDETAILS", 93 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//bulletin board ( south )
		case 2702:
			pUser.SetTag( "ITEMDETAILS", 94 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Sm Bed ( E )
		case 2703:
			pUser.SetTag( "ITEMDETAILS", 93 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Sm Bed ( S )
		case 2704:
			pUser.SetTag( "ITEMDETAILS", 96 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Lg Bed ( E )
		case 2705:
			pUser.SetTag( "ITEMDETAILS", 95 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Lg Bed ( S )
		case 2706:
			pUser.SetTag( "ITEMDETAILS", 97 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Dart Board ( S )
		case 2707:
			pUser.SetTag( "ITEMDETAILS", 98 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Dart Board ( E )
		case 2708:
			pUser.SetTag( "ITEMDETAILS", 99 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Ballot Box
		case 2709:
			pUser.SetTag( "ITEMDETAILS", 100 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Pentagram
		case 2710:
			pUser.SetTag( "ITEMDETAILS", 101 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Abbatoir
		case 2800:
			pUser.SetTag( "ITEMDETAILS", 115 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Dressform
		case 2801:
			pUser.SetTag( "ITEMDETAILS", 116 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Dressform
		case 2802:
			pUser.SetTag( "ITEMDETAILS", 107 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Spin Wheel ( E )
		case 2803:
			pUser.SetTag( "ITEMDETAILS", 108 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Spin Wheel ( S )
		case 2804:
			pUser.SetTag( "ITEMDETAILS", 109 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Loom ( E )
		case 2805:
			pUser.SetTag( "ITEMDETAILS", 110 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Loom ( S )
		case 2806:
			pUser.SetTag( "ITEMDETAILS", 117 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Stone Oven ( E )
		case 2807:
			pUser.SetTag( "ITEMDETAILS", 118 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Stone Oven ( S )
		case 2808:
			pUser.SetTag( "ITEMDETAILS", 119 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Flour Mill ( E )
		case 2809:
			pUser.SetTag( "ITEMDETAILS", 120 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Flour Mill ( S )
		case 2810:
			pUser.SetTag( "ITEMDETAILS", 121 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Water Trough( E )
		case 2811:
			pUser.SetTag( "ITEMDETAILS", 122 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Water Trough( S )
		case 2900:
			pUser.SetTag( "ITEMDETAILS", 102 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Small Forge
		case 2901:
			pUser.SetTag( "ITEMDETAILS", 103 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Lg Forge ( E )
		case 2902:
			pUser.SetTag( "ITEMDETAILS", 104 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Lg Forge ( S )
		case 2903:
			pUser.SetTag( "ITEMDETAILS", 105 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Anvil ( E )
		case 2904:
			pUser.SetTag( "ITEMDETAILS", 106 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Anvil ( S )
		case 2950:
			pUser.SetTag( "ITEMDETAILS", 111 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Dummy ( E )
		case 2951:
			pUser.SetTag( "ITEMDETAILS", 112 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Dummy ( S )
		case 2952:
			pUser.SetTag( "ITEMDETAILS", 113 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Pickpocket ( E )
		case 2953:
			pUser.SetTag( "ITEMDETAILS", 114 );
			TriggerEvent( itemDetailsID, "ItemDetailGump", pUser );
			break;//Pickpocket ( S )
		case 5000:
			switch ( pUser.GetTag( "MAKELAST" ) )
			{
				case 100:
					MakeItem( pSock, pUser, 73 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 100 );
					break;//Barrel Staves
				case 101:
					MakeItem( pSock, pUser, 74 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 101 );
					break;//Barrel Lid
				case 102:
					MakeItem( pSock, pUser, 89 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 102 );
					break;//short music stand ( left )
				case 103:
					MakeItem( pSock, pUser, 90 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 103 );
					break;//short music stand ( right )
				case 104:
					MakeItem( pSock, pUser, 91 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 104 );
					break;//tall music stand ( left )
				case 105:
					MakeItem( pSock, pUser, 92 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 105 );
					break;//tall music stand ( left )
				case 106:
					MakeItem( pSock, pUser, 76 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 106 );
					break;//Easel ( S )
				case 107:
					MakeItem( pSock, pUser, 77 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 107 );
					break;//Easel ( E )
				case 108:
					MakeItem( pSock, pUser, 78 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 108 );
					break;//Easel ( N )
				case 109:
					MakeItem( pSock, pUser, 79 );
					pUser.StartTimer( gumpDelay, 1, true );
					pUser.SetTag( "MAKELAST", 109 );
					break;//Fishing Pole
				case 200:
					MakeItem( pSock, pUser, 50 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 200 );
					break;//Barstool
				case 201:
					MakeItem( pSock, pUser, 51 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 201 );
					break;//Stool
				case 202:
					MakeItem( pSock, pUser, 52 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 202 );
					break;//Straw Chair
				case 203:
					MakeItem( pSock, pUser, 53 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 203 );
					break;//Wooden Chair
				case 204:
					MakeItem( pSock, pUser, 57 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 204 );
					break;//Vesper Style
				case 205:
					MakeItem( pSock, pUser, 58 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 205 );
					break;//Trinsic Style
				case 206:
					MakeItem( pSock, pUser, 54 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 206 );
					break;//Bench
				case 207:
					MakeItem( pSock, pUser, 55 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 207 );
					break;//Wooden Throne
				case 208:
					MakeItem( pSock, pUser, 56 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 208 );
					break;//Magincian Throne
				case 209:
					MakeItem( pSock, pUser, 59 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 209 );
					break;//Small Table
				case 210:
					MakeItem( pSock, pUser, 60 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 210 );
					break;//Writing Table
				case 211:
					MakeItem( pSock, pUser, 61 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 211 );
					break;//Large Table
				case 212:
					MakeItem( pSock, pUser, 62 );
					pUser.StartTimer( gumpDelay, 2, true );
					pUser.SetTag( "MAKELAST", 212 );
					break;//Yew-wood Table
				case 300:
					MakeItem( pSock, pUser, 63 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 300 );
					break;//Small Chest
				case 301:
					MakeItem( pSock, pUser, 64 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 301 );
					break;//Small Crate
				case 302:
					MakeItem( pSock, pUser, 65 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 302 );
					break;//Medium Crate
				case 303:
					MakeItem( pSock, pUser, 67 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 303 );
					break;//Large Crate
				case 304:
					MakeItem( pSock, pUser, 68 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 304 );
					break;//Chest
				case 305:
					MakeItem( pSock, pUser, 69 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 305 );
					break;//Bookshelf
				case 306:
					MakeItem( pSock, pUser, 70 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 306 );
					break;//Armoire
				case 307:
					MakeItem( pSock, pUser, 71 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 307 );
					break;//Armoire
				case 308:
					MakeItem( pSock, pUser, 66 );
					pUser.StartTimer( gumpDelay, 3, true );
					pUser.SetTag( "MAKELAST", 308 );
					break;//Open Keg
				case 400:
					MakeItem( pSock, pUser, 80 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTag( "MAKELAST", 400 );
					break;//Shepard's Crook
				case 401:
					MakeItem( pSock, pUser, 81 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTag( "MAKELAST", 401 );
					break;//Quarter Staff
				case 402:
					MakeItem( pSock, pUser, 82 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTag( "MAKELAST", 402 );
					break;//Gnarled Staff
				case 403:
					MakeItem( pSock, pUser, 123 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTag( "MAKELAST", 403 );
					break;//Club
				case 404:
					MakeItem( pSock, pUser, 124 );
					pUser.StartTimer( gumpDelay, 4, true );
					pUser.SetTag( "MAKELAST", 404 );
					break;//Black Staff
				case 500:
					MakeItem( pSock, pUser, 75 );
					pUser.StartTimer( gumpDelay, 5, true );
					pUser.SetTag( "MAKELAST", 500 );
					break;//Wooden Shield
				case 600:
					MakeItem( pSock, pUser, 83 );
					pUser.StartTimer( gumpDelay, 6, true );
					pUser.SetTag( "MAKELAST", 600 );
					break;//Lap Harp
				case 601:
					MakeItem( pSock, pUser, 84 );
					pUser.StartTimer( gumpDelay, 6, true );
					pUser.SetTag( "MAKELAST", 601 );
					break;//Standing Harp
				case 602:
					MakeItem( pSock, pUser, 85 );
					pUser.StartTimer( gumpDelay, 6, true );
					pUser.SetTag( "MAKELAST", 602 );
					break;//Drum
				case 603:
					MakeItem( pSock, pUser, 86 );
					pUser.StartTimer( gumpDelay, 6, true );
					pUser.SetTag( "MAKELAST", 603 );
					break;//Lute
				case 604:
					MakeItem( pSock, pUser, 87 );
					pUser.StartTimer( gumpDelay, 6, true );
					pUser.SetTag( "MAKELAST", 604 );
					break;//Tambourine
				case 605:
					MakeItem( pSock, pUser, 88 );
					pUser.StartTimer( gumpDelay, 6, true );
					pUser.SetTag( "MAKELAST", 605 );
					break;//Tambourine ( tassel )
				case 700:
					MakeItem( pSock, pUser, 93 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 700 );
					break;//bulletin board ( east )
				case 701:
					MakeItem( pSock, pUser, 93 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 701 );
					break;//bulletin board ( south )
				case 702:
					MakeItem( pSock, pUser, 94 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 702 );
					break;//Sm Bed ( E )
				case 703:
					MakeItem( pSock, pUser, 93 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 703 );
					break;//Sm Bed ( S )
				case 704:
					MakeItem( pSock, pUser, 96 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 704 );
					break;//Lg Bed ( E )
				case 705:
					MakeItem( pSock, pUser, 95 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 705 );
					break;//Lg Bed ( S )
				case 706:
					MakeItem( pSock, pUser, 97 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 706 );
					break;//Dart Board ( S )
				case 707:
					MakeItem( pSock, pUser, 98 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 707 );
					break;//Dart Board ( E )
				case 708:
					MakeItem( pSock, pUser, 99 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 708 );
					break;//Ballot Box
				case 709:
					MakeItem( pSock, pUser, 100 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 709 );
					break;//Pentagram
				case 710:
					MakeItem( pSock, pUser, 101 );
					pUser.StartTimer( gumpDelay, 7, true );
					pUser.SetTag( "MAKELAST", 710 );
					break;//Abbatoir
				case 800:
					MakeItem( pSock, pUser, 115 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 800 );
					break;//Dressform
				case 801:
					MakeItem( pSock, pUser, 116 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 801 );
					break;//Dressform
				case 802:
					MakeItem( pSock, pUser, 107 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 802 );
					break;//Spin Wheel ( E )
				case 803:
					MakeItem( pSock, pUser, 108 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 803 );
					break;//Spin Wheel ( S )
				case 804:
					MakeItem( pSock, pUser, 109 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 804 );
					break;//Loom ( E )
				case 805:
					MakeItem( pSock, pUser, 110 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 805 );
					break;//Loom ( S )
				case 806:
					MakeItem( pSock, pUser, 117 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 806 );
					break;//Stone Oven ( E )
				case 807:
					MakeItem( pSock, pUser, 118 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 807 );
					break;//Stone Oven ( S )
				case 808:
					MakeItem( pSock, pUser, 119 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 808 );
					break;//Flour Mill ( E )
				case 809:
					MakeItem( pSock, pUser, 120 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 809 );
					break;//Flour Mill ( S )
				case 810:
					MakeItem( pSock, pUser, 121 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 810 );
					break;//Water Trough( E )
				case 811:
					MakeItem( pSock, pUser, 122 );
					pUser.StartTimer( gumpDelay, 8, true );
					pUser.SetTag( "MAKELAST", 811 );
					break;//Water Trough( S )
				case 900:
					MakeItem( pSock, pUser, 102 );
					pUser.StartTimer( gumpDelay, 9, true );
					pUser.SetTag( "MAKELAST", 900 );
					break;//Small Forge
				case 901:
					MakeItem( pSock, pUser, 103 );
					pUser.StartTimer( gumpDelay, 9, true );
					pUser.SetTag( "MAKELAST", 901 );
					break;//Lg Forge ( E )
				case 902:
					MakeItem( pSock, pUser, 104 );
					pUser.StartTimer( gumpDelay, 9, true );
					pUser.SetTag( "MAKELAST", 902 );
					break;//Lg Forge ( S )
				case 903:
					MakeItem( pSock, pUser, 105 );
					pUser.StartTimer( gumpDelay, 9, true );
					pUser.SetTag( "MAKELAST", 903 );
					break;//Anvil ( E )
				case 904:
					MakeItem( pSock, pUser, 106 );
					pUser.StartTimer( gumpDelay, 9, true );
					pUser.SetTag( "MAKELAST", 904 );
					break;//Anvil ( S )
				case 1000:
					MakeItem( pSock, pUser, 111 );
					pUser.StartTimer( gumpDelay, 10, true );
					pUser.SetTag( "MAKELAST", 1000 );
					break;//Dummy ( E )
				case 1001:
					MakeItem( pSock, pUser, 112 );
					pUser.StartTimer( gumpDelay, 10, true );
					pUser.SetTag( "MAKELAST", 1001 );
					break;//Dummy ( S )
				case 1002:
					MakeItem( pSock, pUser, 113 );
					pUser.StartTimer( gumpDelay, 10, true );
					pUser.SetTag( "MAKELAST", 1002 );
					break;//Pickpocket ( E )
				case 1003:
					MakeItem( pSock, pUser, 114 );
					pUser.StartTimer( gumpDelay, 10, true );
					pUser.SetTag( "MAKELAST", 1003 );
					break;//Pickpocket ( S )
			}break;
	}
}