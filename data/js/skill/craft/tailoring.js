var textHue = 0x480;				// Color of the text.
const scriptID = 4030;				// Use this to tell the gump what script to close.
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

var myPage1 = [11415, 11416, 11417, 11418, 11419, 11420, 11421, 11422, 11423, 11424, 11425];// Hats
var myPage2 = [11426, 114267, 11428, 11429, 11430, 11431, 11432, 11433, 11434, 11435, 11436, 11437, 11438, 11439];// Shirts and Pants
var myPage3 = [11440, 11441, 11442, 11443];// Miscellaneous
var myPage4 = [11444, 11445,  11446, 11447];// Footwear
var myPage5 = [11448, 11449, 11450, 11451, 11452, 11453];// Leather Armor
var myPage6 = [11454, 11455, 11456, 11457, 11458];// Studded Armor
var myPage7 = [11459, 11460, 11461, 11462, 11463, 11464];// Female Armor
var myPage8 = [11465, 11466, 11467, 11468, 11469];// Bone Armor

function page1( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTempTag( "page", 1 );
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

function page5( socket, pUser )
{
	var myGump = new Gump;
	pUser.SetTempTag( "page", 5 );
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
	pUser.SetTempTag( "page", 6 );
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
	pUser.SetTempTag( "page", 7 );
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
	pUser.SetTempTag( "page", 8 );
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
			mChar.SetTempTag( "CANTREPAIR", 1 );
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
						mChar.SetTempTag( "CANTREPAIR", 1 );
						mChar.StartTimer( repairDelay, 1, true );
						return;
					}

					if ( mChar.CheckSkill( 8, minSkill, maxSkill ) )
					{
						socket.CloseGump( gumpID, 0 );
						ourObj.health = ourObj.maxhp;
						mChar.SetTempTag( "REPAIRSUCCESS", 1 );
						socket.SoundEffect( 0x002A, true );
						mChar.StartTimer( repairDelay, 1, true );
					}
					else
					{
						socket.tempObj = bItem;
						socket.CloseGump( gumpID, 0 );
						mChar.SetTempTag( "FAILREPAIR", 1 );
						mChar.StartTimer( repairDelay, 1, true );
					}
				}
				else
				{
					socket.tempObj = bItem;
					socket.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "FULLREPAIR", 1 );
					mChar.StartTimer( repairDelay, 1, true );
				}
			}
			else
			{
				socket.tempObj = bItem;
				socket.CloseGump( gumpID, 0 );
				mChar.SetTempTag( "CHECKPACK", 1 );
				mChar.StartTimer( repairDelay, 1, true );
			}
		}
		else
		{
			socket.tempObj = bItem;
			socket.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "CANTREPAIR", 1 );
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
	}
}

function onGumpPress(pSock, pButton, gumpData) {
	var pUser = pSock.currentChar;
	var bItem = pSock.tempObj;
	var gumpID = scriptID + 0xffff;
	switch (pButton) {
		case 0:
			pUser.SetTempTag("MAKELAST", null);
			pUser.SetTempTag("CRAFT", null)
			pSock.CloseGump(gumpID, 0);
			break;// abort and do nothing
		case 1:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page1", pSock, pUser);
			break;
		case 2:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page2", pSock, pUser);
			break;
		case 3:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page3", pSock, pUser);
			break;
		case 4:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page4", pSock, pUser);
			break;
		case 5:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page5", pSock, pUser);
			break;
		case 6:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page6", pSock, pUser);
			break;
		case 7:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page7", pSock, pUser);
			break;
		case 8:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(scriptID, "page8", pSock, pUser);
			break;
		case 51:
			var targMsg = GetDictionaryEntry(485, pSock.Language);
			pSock.CustomTarget(2, targMsg);
			break;
		case 100:
			MakeItem(pSock, pUser, 130);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 100);
			break;//skullcap
		case 101:
			MakeItem(pSock, pUser, 131);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 101);
			break;//bandana
		case 102:
			MakeItem(pSock, pUser, 132);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 102);
			break;//floppy hat
		case 103:
			MakeItem(pSock, pUser, 134);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 103);
			break;//cap
		case 104:
			MakeItem(pSock, pUser, 133);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 104);
			break;//wide-brim hat
		case 105:
			MakeItem(pSock, pUser, 136);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 105);
			break;//straw hat
		case 106:
			MakeItem(pSock, pUser, 137);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 106);
			break;//wizard's hat
		case 107:
			MakeItem(pSock, pUser, 138);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 107);
			break;//bonnet
		case 108:
			MakeItem(pSock, pUser, 139);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 108);
			break;//feather hat
		case 109:
			MakeItem(pSock, pUser, 140);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 109);
			break;//tricorne hat
		case 110:
			MakeItem(pSock, pUser, 141);
			pUser.StartTimer(gumpDelay, 1, true);
			pUser.SetTempTag("MAKELAST", 110);
			break;//jester hat
		case 200:
			MakeItem(pSock, pUser, 142);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 200);
			break;//doublet
		case 201:
			MakeItem(pSock, pUser, 143);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 201);
			break;//shirt
		case 202:
			MakeItem(pSock, pUser, 144);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 202);
			break;//fancy shirt
		case 203:
			MakeItem(pSock, pUser, 145);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 203);
			break;//tunic
		case 204:
			MakeItem(pSock, pUser, 146);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 204);
			break;//surcoat
		case 205:
			MakeItem(pSock, pUser, 147);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 205);
			break;//plain dress
		case 206:
			MakeItem(pSock, pUser, 148);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 206);
			break;//fancy dress
		case 207:
			MakeItem(pSock, pUser, 149);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 207);
			break;//cloak
		case 208:
			MakeItem(pSock, pUser, 150);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 208);
			break;//robe
		case 209:
			MakeItem(pSock, pUser, 151);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 209);
			break;//jester suit
		case 210:
			MakeItem(pSock, pUser, 180);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 210);
			break;//short pants
		case 211:
			MakeItem(pSock, pUser, 152);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 211);
			break;//long pants
		case 212:
			MakeItem(pSock, pUser, 153);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 212);
			break;//kilt
		case 213:
			MakeItem(pSock, pUser, 154);
			pUser.StartTimer(gumpDelay, 2, true);
			pUser.SetTempTag("MAKELAST", 213);
			break;//skirt
		case 300:
			MakeItem(pSock, pUser, 155);
			pUser.StartTimer(gumpDelay, 3, true);
			pUser.SetTempTag("MAKELAST", 300);
			break;//body sash
		case 301:
			MakeItem(pSock, pUser, 156);
			pUser.StartTimer(gumpDelay, 3, true);
			pUser.SetTempTag("MAKELAST", 301);
			break;//half apron
		case 302:
			MakeItem(pSock, pUser, 157);
			pUser.StartTimer(gumpDelay, 3, true);
			pUser.SetTempTag("MAKELAST", 302);
			break;//full apron
		case 303:
			MakeItem(pSock, pUser, 158);
			pUser.StartTimer(gumpDelay, 3, true);
			pUser.SetTempTag("MAKELAST", 303);
			break;//oil cloth
		case 400:
			MakeItem(pSock, pUser, 159);
			pUser.StartTimer(gumpDelay, 4, true);
			pUser.SetTempTag("MAKELAST", 400);
			break;//sandals
		case 401:
			MakeItem(pSock, pUser, 160);
			pUser.StartTimer(gumpDelay, 4, true);
			pUser.SetTempTag("MAKELAST", 401);
			break;//shoes
		case 402:
			MakeItem(pSock, pUser, 161);
			pUser.StartTimer(gumpDelay, 4, true);
			pUser.SetTempTag("MAKELAST", 402);
			break;//boots
		case 403:
			MakeItem(pSock, pUser, 162);
			pUser.StartTimer(gumpDelay, 4, true);
			pUser.SetTempTag("MAKELAST", 403);
			break;//thigh boots
		case 500:
			MakeItem(pSock, pUser, 163);
			pUser.StartTimer(gumpDelay, 5, true);
			pUser.SetTempTag("MAKELAST", 500);
			break;//leather gorget
		case 501:
			MakeItem(pSock, pUser, 164);
			pUser.StartTimer(gumpDelay, 5, true);
			pUser.SetTempTag("MAKELAST", 501);
			break;//leather cap
		case 502:
			MakeItem(pSock, pUser, 165);
			pUser.StartTimer(gumpDelay, 5, true);
			pUser.SetTempTag("MAKELAST", 502);
			break;//leather gloves
		case 503:
			MakeItem(pSock, pUser, 166);
			pUser.StartTimer(gumpDelay, 5, true);
			pUser.SetTempTag("MAKELAST", 503);
			break;//leather sleeves
		case 504:
			MakeItem(pSock, pUser, 167);
			pUser.StartTimer(gumpDelay, 5, true);
			pUser.SetTempTag("MAKELAST", 504);
			break;//leather legging
		case 505:
			MakeItem(pSock, pUser, 168);
			pUser.StartTimer(gumpDelay, 5, true);
			pUser.SetTempTag("MAKELAST", 505);
			break;//leather tunic
		case 600:
			MakeItem(pSock, pUser, 169);
			pUser.StartTimer(gumpDelay, 6, true);
			pUser.SetTempTag("MAKELAST", 600);
			break;//studded gorget
		case 601:
			MakeItem(pSock, pUser, 170);
			pUser.StartTimer(gumpDelay, 6, true);
			pUser.SetTempTag("MAKELAST", 601);
			break;//studded gloves
		case 602:
			MakeItem(pSock, pUser, 171);
			pUser.StartTimer(gumpDelay, 6, true);
			pUser.SetTempTag("MAKELAST", 602);
			break;//studded sleeves
		case 603:
			MakeItem(pSock, pUser, 172);
			pUser.StartTimer(gumpDelay, 6, true);
			pUser.SetTempTag("MAKELAST", 603);
			break;//studded leggings
		case 604:
			MakeItem(pSock, pUser, 173);
			pUser.StartTimer(gumpDelay, 6, true);
			pUser.SetTempTag("MAKELAST", 604);
			break;//studded tunic
		case 700:
			MakeItem(pSock, pUser, 174);
			pUser.StartTimer(gumpDelay, 7, true);
			pUser.SetTempTag("MAKELAST", 700);
			break;//leather shorts
		case 701:
			MakeItem(pSock, pUser, 175);
			pUser.StartTimer(gumpDelay, 7, true);
			pUser.SetTempTag("MAKELAST", 701);
			break;//leather skirts
		case 802:
			MakeItem(pSock, pUser, 176);
			pUser.StartTimer(gumpDelay, 7, true);
			pUser.SetTempTag("MAKELAST", 702);
			break;//leather bustier
		case 703:
			MakeItem(pSock, pUser, 177);
			pUser.StartTimer(gumpDelay, 7, true);
			pUser.SetTempTag("MAKELAST", 703);
			break;//studded bustier
		case 704:
			MakeItem(pSock, pUser, 178);
			pUser.StartTimer(gumpDelay, 7, true);
			pUser.SetTempTag("MAKELAST", 704);
			break;//feamle leather armor
		case 705:
			MakeItem(pSock, pUser, 179);
			pUser.StartTimer(gumpDelay, 7, true);
			pUser.SetTempTag("MAKELAST", 705);
			break;//studded armor
		case 800:
			MakeItem(pSock, pUser, 181);
			pUser.StartTimer(gumpDelay, 8, true);
			pUser.SetTempTag("MAKELAST", 800);
			break;//bone helmet
		case 901:
			MakeItem(pSock, pUser, 182);
			pUser.StartTimer(gumpDelay, 9, true);
			pUser.SetTempTag("MAKELAST", 901);
			break;//bone gloves
		case 802:
			MakeItem(pSock, pUser, 183);
			pUser.StartTimer(gumpDelay, 8, true);
			pUser.SetTempTag("MAKELAST", 802);
			break;//bone arms
		case 803:
			MakeItem(pSock, pUser, 184);
			pUser.StartTimer(gumpDelay, 8, true);
			pUser.SetTempTag("MAKELAST", 803);
			break;//bone leggings
		case 804:
			MakeItem(pSock, pUser, 185);
			pUser.StartTimer(gumpDelay, 8, true);
			pUser.SetTempTag("MAKELAST", 804);
			break;//bone armor
		case 2100:
			pUser.SetTempTag("ITEMDETAILS", 130);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//skullcap
		case 2101:
			pUser.SetTempTag("ITEMDETAILS", 131);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bandana
		case 2102:
			pUser.SetTempTag("ITEMDETAILS", 132);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//floppy hat
		case 2103:
			pUser.SetTempTag("ITEMDETAILS", 134);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//cap
		case 2104:
			pUser.SetTempTag("ITEMDETAILS", 133);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//wide-brim hat
		case 2105:
			pUser.SetTempTag("ITEMDETAILS", 136);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//straw hat
		case 2106:
			pUser.SetTempTag("ITEMDETAILS", 137);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//wizard's hat
		case 2107:
			pUser.SetTempTag("ITEMDETAILS", 138);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bonnet
		case 2108:
			pUser.SetTempTag("ITEMDETAILS", 139);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//feather hat
		case 2109:
			pUser.SetTempTag("ITEMDETAILS", 140);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//tricorne hat
		case 2110:
			pUser.SetTempTag("ITEMDETAILS", 141);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//jester hat
		case 2200:
			pUser.SetTempTag("ITEMDETAILS", 142);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//doublet
		case 2201:
			pUser.SetTempTag("ITEMDETAILS", 143);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//shirt
		case 2202:
			pUser.SetTempTag("ITEMDETAILS", 144);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//fancy shirt
		case 2203:
			pUser.SetTempTag("ITEMDETAILS", 145);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//tunic
		case 2204:
			pUser.SetTempTag("ITEMDETAILS", 146);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//surcoat
		case 2205:
			pUser.SetTempTag("ITEMDETAILS", 147);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//plain dress
		case 2206:
			pUser.SetTempTag("ITEMDETAILS", 148);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//fancy dress
		case 2207:
			pUser.SetTempTag("ITEMDETAILS", 149);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//cloak
		case 2208:
			pUser.SetTempTag("ITEMDETAILS", 150);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//robe
		case 2209:
			pUser.SetTempTag("ITEMDETAILS", 151);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//jester suit
		case 2210:
			pUser.SetTempTag("ITEMDETAILS", 180);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//short pants
		case 2211:
			pUser.SetTempTag("ITEMDETAILS", 152);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//long pants
		case 2212:
			pUser.SetTempTag("ITEMDETAILS", 153);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//kilt
		case 2213:
			pUser.SetTempTag("ITEMDETAILS", 154);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//skirt
		case 2300:
			pUser.SetTempTag("ITEMDETAILS", 155);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//body sash
		case 2301:
			pUser.SetTempTag("ITEMDETAILS", 156);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//half apron
		case 2302:
			pUser.SetTempTag("ITEMDETAILS", 157);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//full apron
		case 2303:
			pUser.SetTempTag("ITEMDETAILS", 158);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//oil cloth
		case 2400:
			pUser.SetTempTag("ITEMDETAILS", 159);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//sandals
		case 2401:
			pUser.SetTempTag("ITEMDETAILS", 160);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//shoes
		case 2402:
			pUser.SetTempTag("ITEMDETAILS", 161);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//boots
		case 2403:
			pUser.SetTempTag("ITEMDETAILS", 162);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//thigh boots
		case 2500:
			pUser.SetTempTag("ITEMDETAILS", 163);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather gorget
		case 2501:
			pUser.SetTempTag("ITEMDETAILS", 164);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather cap
		case 2502:
			pUser.SetTempTag("ITEMDETAILS", 165);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather gloves
		case 2503:
			pUser.SetTempTag("ITEMDETAILS", 166);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather sleeves
		case 2504:
			pUser.SetTempTag("ITEMDETAILS", 167);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather legging
		case 2505:
			pUser.SetTempTag("ITEMDETAILS", 168);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather tunic
		case 2600:
			pUser.SetTempTag("ITEMDETAILS", 169);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded gorget
		case 2601:
			pUser.SetTempTag("ITEMDETAILS", 170);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded gloves
		case 2602:
			pUser.SetTempTag("ITEMDETAILS", 171);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded sleeves
		case 2603:
			pUser.SetTempTag("ITEMDETAILS", 172);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded leggings
		case 2604:
			pUser.SetTempTag("ITEMDETAILS", 173);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded tunic
		case 2700:
			pUser.SetTempTag("ITEMDETAILS", 174);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather shorts
		case 2701:
			pUser.SetTempTag("ITEMDETAILS", 175);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather skirts
		case 2702:
			pUser.SetTempTag("ITEMDETAILS", 176);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//leather bustier
		case 2703:
			pUser.SetTempTag("ITEMDETAILS", 177);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded bustier
		case 2704:
			pUser.SetTempTag("ITEMDETAILS", 178);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//feamle leather armor
		case 2705:
			pUser.SetTempTag("ITEMDETAILS", 179);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//studded armor
		case 2800:
			pUser.SetTempTag("ITEMDETAILS", 181);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bone helmet
		case 2801:
			pUser.SetTempTag("ITEMDETAILS", 182);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bone gloves
		case 2802:
			pUser.SetTempTag("ITEMDETAILS", 183);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bone arms
		case 2803:
			pUser.SetTempTag("ITEMDETAILS", 184);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bone leggings
		case 2804:
			pUser.SetTempTag("ITEMDETAILS", 185);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//bone armor
		case 5000:
			switch (pUser.GetTempTag("MAKELAST"))
			{
				case 100:
					MakeItem(pSock, pUser, 130);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 100);
					break;//skullcap
				case 101:
					MakeItem(pSock, pUser, 131);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 101);
					break;//bandana
				case 102:
					MakeItem(pSock, pUser, 132);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 102);
					break;//floppy hat
				case 103:
					MakeItem(pSock, pUser, 134);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 103);
					break;//cap
				case 104:
					MakeItem(pSock, pUser, 133);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 104);
					break;//wide-brim hat
				case 105:
					MakeItem(pSock, pUser, 136);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 105);
					break;//straw hat
				case 106:
					MakeItem(pSock, pUser, 137);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 106);
					break;//wizard's hat
				case 107:
					MakeItem(pSock, pUser, 138);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 107);
					break;//bonnet
				case 108:
					MakeItem(pSock, pUser, 139);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 108);
					break;//feather hat
				case 109:
					MakeItem(pSock, pUser, 140);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 109);
					break;//tricorne hat
				case 110:
					MakeItem(pSock, pUser, 141);
					pUser.StartTimer(gumpDelay, 1, true);
					pUser.SetTempTag("MAKELAST", 110);
					break;//jester hat
				case 200:
					MakeItem(pSock, pUser, 142);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 200);
					break;//doublet
				case 201:
					MakeItem(pSock, pUser, 143);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 201);
					break;//shirt
				case 202:
					MakeItem(pSock, pUser, 144);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 202);
					break;//fancy shirt
				case 203:
					MakeItem(pSock, pUser, 145);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 203);
					break;//tunic
				case 204:
					MakeItem(pSock, pUser, 146);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 204);
					break;//surcoat
				case 205:
					MakeItem(pSock, pUser, 147);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 205);
					break;//plain dress
				case 206:
					MakeItem(pSock, pUser, 148);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 206);
					break;//fancy dress
				case 207:
					MakeItem(pSock, pUser, 149);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 207);
					break;//cloak
				case 208:
					MakeItem(pSock, pUser, 150);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 208);
					break;//robe
				case 209:
					MakeItem(pSock, pUser, 151);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 209);
					break;//jester suit
				case 210:
					MakeItem(pSock, pUser, 180);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 210);
					break;//short pants
				case 211:
					MakeItem(pSock, pUser, 152);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 211);
					break;//long pants
				case 212:
					MakeItem(pSock, pUser, 153);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 212);
					break;//kilt
				case 213:
					MakeItem(pSock, pUser, 154);
					pUser.StartTimer(gumpDelay, 2, true);
					pUser.SetTempTag("MAKELAST", 213);
					break;//skirt
				case 300:
					MakeItem(pSock, pUser, 155);
					pUser.StartTimer(gumpDelay, 3, true);
					pUser.SetTempTag("MAKELAST", 300);
					break;//body sash
				case 301:
					MakeItem(pSock, pUser, 156);
					pUser.StartTimer(gumpDelay, 3, true);
					pUser.SetTempTag("MAKELAST", 301);
					break;//half apron
				case 302:
					MakeItem(pSock, pUser, 157);
					pUser.StartTimer(gumpDelay, 3, true);
					pUser.SetTempTag("MAKELAST", 302);
					break;//full apron
				case 303:
					MakeItem(pSock, pUser, 158);
					pUser.StartTimer(gumpDelay, 3, true);
					pUser.SetTempTag("MAKELAST", 303);
					break;//oil cloth
				case 400:
					MakeItem(pSock, pUser, 159);
					pUser.StartTimer(gumpDelay, 4, true);
					pUser.SetTempTag("MAKELAST", 400);
					break;//sandals
				case 401:
					MakeItem(pSock, pUser, 160);
					pUser.StartTimer(gumpDelay, 4, true);
					pUser.SetTempTag("MAKELAST", 401);
					break;//shoes
				case 402:
					MakeItem(pSock, pUser, 161);
					pUser.StartTimer(gumpDelay, 4, true);
					pUser.SetTempTag("MAKELAST", 402);
					break;//boots
				case 403:
					MakeItem(pSock, pUser, 162);
					pUser.StartTimer(gumpDelay, 4, true);
					pUser.SetTempTag("MAKELAST", 403);
					break;//thigh boots
				case 500:
					MakeItem(pSock, pUser, 163);
					pUser.StartTimer(gumpDelay, 5, true);
					pUser.SetTempTag("MAKELAST", 500);
					break;//leather gorget
				case 501:
					MakeItem(pSock, pUser, 164);
					pUser.StartTimer(gumpDelay, 5, true);
					pUser.SetTempTag("MAKELAST", 501);
					break;//leather cap
				case 502:
					MakeItem(pSock, pUser, 165);
					pUser.StartTimer(gumpDelay, 5, true);
					pUser.SetTempTag("MAKELAST", 502);
					break;//leather gloves
				case 503:
					MakeItem(pSock, pUser, 166);
					pUser.StartTimer(gumpDelay, 5, true);
					pUser.SetTempTag("MAKELAST", 503);
					break;//leather sleeves
				case 504:
					MakeItem(pSock, pUser, 167);
					pUser.StartTimer(gumpDelay, 5, true);
					pUser.SetTempTag("MAKELAST", 504);
					break;//leather legging
				case 505:
					MakeItem(pSock, pUser, 168);
					pUser.StartTimer(gumpDelay, 5, true);
					pUser.SetTempTag("MAKELAST", 505);
					break;//leather tunic
				case 600:
					MakeItem(pSock, pUser, 169);
					pUser.StartTimer(gumpDelay, 6, true);
					pUser.SetTempTag("MAKELAST", 600);
					break;//studded gorget
				case 601:
					MakeItem(pSock, pUser, 170);
					pUser.StartTimer(gumpDelay, 6, true);
					pUser.SetTempTag("MAKELAST", 601);
					break;//studded gloves
				case 602:
					MakeItem(pSock, pUser, 171);
					pUser.StartTimer(gumpDelay, 6, true);
					pUser.SetTempTag("MAKELAST", 602);
					break;//studded sleeves
				case 603:
					MakeItem(pSock, pUser, 172);
					pUser.StartTimer(gumpDelay, 6, true);
					pUser.SetTempTag("MAKELAST", 603);
					break;//studded leggings
				case 604:
					MakeItem(pSock, pUser, 173);
					pUser.StartTimer(gumpDelay, 6, true);
					pUser.SetTempTag("MAKELAST", 604);
					break;//studded tunic
				case 700:
					MakeItem(pSock, pUser, 174);
					pUser.StartTimer(gumpDelay, 7, true);
					pUser.SetTempTag("MAKELAST", 700);
					break;//leather shorts
				case 701:
					MakeItem(pSock, pUser, 175);
					pUser.StartTimer(gumpDelay, 7, true);
					pUser.SetTempTag("MAKELAST", 701);
					break;//leather skirts
				case 802:
					MakeItem(pSock, pUser, 176);
					pUser.StartTimer(gumpDelay, 7, true);
					pUser.SetTempTag("MAKELAST", 702);
					break;//leather bustier
				case 703:
					MakeItem(pSock, pUser, 177);
					pUser.StartTimer(gumpDelay, 7, true);
					pUser.SetTempTag("MAKELAST", 703);
					break;//studded bustier
				case 704:
					MakeItem(pSock, pUser, 178);
					pUser.StartTimer(gumpDelay, 7, true);
					pUser.SetTempTag("MAKELAST", 704);
					break;//feamle leather armor
				case 705:
					MakeItem(pSock, pUser, 179);
					pUser.StartTimer(gumpDelay, 7, true);
					pUser.SetTempTag("MAKELAST", 705);
					break;//studded armor
				case 800:
					MakeItem(pSock, pUser, 181);
					pUser.StartTimer(gumpDelay, 8, true);
					pUser.SetTempTag("MAKELAST", 800);
					break;//bone helmet
				case 901:
					MakeItem(pSock, pUser, 182);
					pUser.StartTimer(gumpDelay, 9, true);
					pUser.SetTempTag("MAKELAST", 901);
					break;//bone gloves
				case 802:
					MakeItem(pSock, pUser, 183);
					pUser.StartTimer(gumpDelay, 8, true);
					pUser.SetTempTag("MAKELAST", 802);
					break;//bone arms
				case 803:
					MakeItem(pSock, pUser, 184);
					pUser.StartTimer(gumpDelay, 8, true);
					pUser.SetTempTag("MAKELAST", 803);
					break;//bone leggings
				case 804:
					MakeItem(pSock, pUser, 185);
					pUser.StartTimer(gumpDelay, 8, true);
					pUser.SetTempTag("MAKELAST", 804);
					break;//bone armor
			}break;
	}
}