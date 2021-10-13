var textHue = 0x480;				// Color of the text.
var LabelColor = 0x7FFF;			// Second Color of text.
const scriptID = 4025;				// Use this to tell the gump what script to close.
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const resourceDelay = 200;			// Timer for the gump to reapear after selecting a resource.
var blacksmithID = 4014;
var bronzeID = 4015;
var copperID = 4016;
var agapiteID = 4017;
var dullcopperID = 4018;
var goldID = 4019;
var shadowironID = 4020;
var valoriteID = 4021;
var veriteID = 4022;
var Cooking = 4023;
var Carpentry = 4025;
var Alchemy = 4028;
var Bowcraft = 4029;
var Tailoring = 4030;

function craftinggump( myGump, socket )
{
	var pUser = socket.currentChar;
	var iron = pUser.ResourceCount(0x1BF2);
	var bronze = pUser.ResourceCount(0x1BF2, 0x06d6);
	var copper = pUser.ResourceCount(0x1BF2, 0x07dd);
	var agapite = pUser.ResourceCount(0x1BF2, 0x0979);
	var dullcopper = pUser.ResourceCount(0x1BF2, 0x0973);
	var gold = pUser.ResourceCount(0x1BF2, 0x08a5);
	var shadowiron = pUser.ResourceCount(0x1BF2, 0x0966);
	var valorlite = pUser.ResourceCount(0x1BF2, 0x08ab);
	var verite = pUser.ResourceCount(0x1BF2, 0x089f);
	var logs = pUser.ResourceCount( 0x1BE0 );
	var boards = pUser.ResourceCount( 0x1bd7 );
	var leather = pUser.ResourceCount( 0x1067 );
	var leather1 = pUser.ResourceCount( 0x1068 );
	var leather2 = pUser.ResourceCount( 0x1081 );
	var leather3 = pUser.ResourceCount( 0x1082 );
	var hides = pUser.ResourceCount( 0x1078 );
	var hides1 = pUser.ResourceCount( 0x1079 );

	switch (pUser.GetTag("CRAFT"))
	{
		case 1:
			var grouplist = [10601, 10602, 10603, 10604, 10605, 10606, 10607, 10608, 10609, 10610]; //CATEGORIES
			var resourcename = 10687;
			var resource = (logs + boards);
			var name = 10600;//Carpentry
			var repair = 51;
			break;
		case 2:
			var grouplist = [10902, 10903, 10904, 10905]; //CATEGORIES
			var name = 10901;//Alchemy Menu
			break;
		case 3:
			var grouplist = [11202, 11203, 11204]; //CATEGORIES
			var name = 11201;//Bowcraft Menu
			var resourcename = 10687;
			var resource = (logs + boards);
			var repair = 51;
			break;
		case 4:
			var grouplist = [11403, 11404, 11405, 11406, 11407, 11408, 11410, 11411, 11412]; //CATEGORIES
			var name = 11401;//Tailoring Menu
			var resourcename = 11402;
			var resource = (leather + leather1 + leather2 + leather3 + hides + hides1);
			var repair = 51;
			break;
		case 5:
			var grouplist = [10279, 10280, 10281, 10282, 10283, 10284, 10285] //CATEGORIES
			var name = 10188;//Blacksmithing Menu
			switch (pUser.GetTag("ORE"))
			{
				case 1:
					var resourcename = 10291;
					var resource = iron;
					break;
				case 2:
					var resourcename = 10203;
					var resource = bronze;
					break;
				case 3:
					var resourcename = 10204;
					var resource = copper;
					break;
				case 4:
					var resourcename = 10205;
					var resource = agapite;
					break;
				case 5:
					var resourcename = 10206;
					var resource = dullcopper;
					break;
				case 6:
					var resourcename = 10207;
					var resource = gold;
					break;
				case 7:
					var resourcename = 10208;
					var resource = shadowiron;
					break;
				case 8:
					var resourcename = 10209;
					var resource = valorlite;
					break;
				case 9:
					var resourcename = 10210;
					var resource = verite;
					break;
				default:
					var resourcename = 10291;
					var resource = iron;
					break;
			}
			var repair = 49;
			break;
		case 6:
			var grouplist = [11602, 11603, 11604, 11605]; //CATEGORIES
			var name = 11601;//Cooking Menu
			break;
	}

	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 530, 437, 5054 );
	myGump.AddTiledGump( 10, 10, 510, 22, 2624 );
	myGump.AddTiledGump( 10, 292, 150, 45, 2624 );
	myGump.AddTiledGump( 165, 292, 355, 45, 2624 );
	myGump.AddTiledGump( 10, 342, 510, 85, 2624 );
	myGump.AddTiledGump( 10, 37, 200, 250, 2624 );
	myGump.AddTiledGump( 215, 37, 305, 250, 2624 );
	myGump.AddCheckerTrans( 10, 10, 510, 417 );

	myGump.AddHTMLGump( 210, 12, 510, 20, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( name, socket.language ) + "</basefont> </center>" ); // <CENTER>MENU NAME</CENTER>
	myGump.AddHTMLGump( 50, 37, 200, 22, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10286, socket.language ) + "</basefont> </center>" ); // <CENTER>CATEGORIES</CENTER>
	myGump.AddHTMLGump( 305, 37, 305, 22, 0, 0, "<center><basefont color=#ffffff>" + GetDictionaryEntry( 10287, socket.language ) + "</basefont></center>" ); // <CENTER>CATEGORIES</CENTER>

	myGump.AddHTMLGump( 10, 302, 150, 25, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10288, socket.language ) + "</basefont> </center>" ); // <CENTER>NOTICES</CENTER>

	if (pUser.GetTag("FAILED") >= 1 && pUser.GetTag("FAILED") <= 8)
	{
		myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10292, socket.language) + "</basefont>");
		pUser.SetTag("FAILED", null);
	}

	if (pUser.GetTag("NOANVIL") == 1)
	{
		myGump.AddHTMLGump(170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10213, socket.language) + "</basefont>");
		pUser.SetTag("NOANVIL", null);
	}

	if ( pUser.GetTag( "CANTREPAIR" ) == 1 )
	{
		myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 987, socket.Language ) + "</basefont>" );
		pUser.SetTag( "CANTREPAIR", null );
	}

	if ( pUser.GetTag( "REPAIRSUCCESS" ) == 1 )
	{
		myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 989, socket.Language ) + "</basefont>" );
		pUser.SetTag( "REPAIRSUCCESS", null );
	}

	if ( pUser.GetTag( "FAILREPAIR" ) == 1 )
	{
		myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 990, socket.Language ) + "</basefont>" );
		pUser.SetTag( "CHECKPACK", null );
	}

	if ( pUser.GetTag( "FULLREPAIR" ) == 1 )
	{
		myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 988, socket.Language ) + "</basefont>" );
		pUser.SetTag( "FULLREPAIR", null );
	}

	if ( pUser.GetTag( "CHECKPACK" ) == 1 )
	{
		myGump.AddHTMLGump( 170, 295, 350, 40, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10265, socket.Language ) + "</basefont>" );
		pUser.SetTag( "CHECKPACK", null );
	}

	if (pUser.GetTag( "CRAFT" ) != 2 || pUser.GetTag( "CRAFT" ) != 6 )
	{
		myGump.AddText( 50, 362, textHue, GetDictionaryEntry( resourcename, socket.language ) + " (" + resource.toString() + ")" );

		myGump.AddButton(15, 362, 4005, 4007, 1, 0, 50);

		myGump.AddButton(270, 342, 0xfa5, 1, 0, repair);
		myGump.AddHTMLGump(305, 345, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10212, socket.language ) + "</basefont>" );// REPAIR ITEM
		if (pUser.GetTag( "CRAFT" ) == 5)
		{
			myGump.AddButton( 15, 342, 0xfa5, 1, 0, 52 );
			myGump.AddHTMLGump( 50, 345, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10289, socket.language ) + "</basefont>" );// SMELT ITEM
		}
	}

	myGump.AddButton( 15, 402, 0x0fb1, 1, 0, 0 );
	myGump.AddHTMLGump( 50, 405, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10290, socket.language ) + "</basefont>" );// EXIT

	myGump.AddButton( 270, 402, 0xfab, 1, 0, 5000 );
	myGump.AddHTMLGump( 305, 405, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10277, socket.language ) + "</basefont>" );// MAKE LAST

	for ( var i = 0; i < grouplist.length; i++ )
	{
		//myGump.AddButton( 15, 60, 0xfa5, 0, 4, 0 );
		//myGump.AddXMFHTMLGumpColor( 50, 63, 150, 18, 1044014, false, false, LabelColor ); // LAST TEN
		myGump.AddButton( 15, 80 + ( i * 20 ), 4005, 4007, 1, 0, 1 + i );
		myGump.AddText( 50, 80 + ( i * 20 ), textHue, GetDictionaryEntry( grouplist[i], socket.language ) );
	}
	return;
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
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page1", pSock, pUser );
					break;
				case 2:
					TriggerEvent( Alchemy, "page1", pSock, pUser );
					break;
				case 3:
					TriggerEvent( Bowcraft, "page1", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page1", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page1", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page1", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page1", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page1", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page1", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page1", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page1", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page1", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page1", pSock, pUser );
							break;
					}
					break;
				case 6:
					TriggerEvent( Cooking, "page1", pSock, pUser );
					break;
			}
			break;
		case 2:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page2", pSock, pUser );
					break;
				case 2:
					TriggerEvent( Alchemy, "page2", pSock, pUser );
					break;
				case 3:
					TriggerEvent( Bowcraft, "page2", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page2", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page2", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page2", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page2", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page2", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page2", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page2", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page2", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page2", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page2", pSock, pUser );
							break;
					}
					break;
				case 6:
					TriggerEvent( Cooking, "page2", pSock, pUser );
					break;
			}
			break;
		case 3:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page3", pSock, pUser );
					break;
				case 2:
					TriggerEvent( Alchemy, "page3", pSock, pUser );
					break;
				case 3:
					TriggerEvent( Bowcraft, "page3", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page3", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page3", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page3", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page3", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page3", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page3", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page3", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page3", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page3", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page3", pSock, pUser );
							break;
					}
					break;
				case 6:
					TriggerEvent( Cooking, "page3", pSock, pUser );
					break;
			}
			break;
		case 4:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page4", pSock, pUser );
					break;
				case 2:
					TriggerEvent( Alchemy, "page4", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page4", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page4", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page4", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page4", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page4", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page4", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page4", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page4", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page4", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page4", pSock, pUser );
							break;
					}
					break;
				case 6:
					TriggerEvent( Cooking, "page4", pSock, pUser );
					break;
			}
			break;
		case 5:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page5", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page5", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page5", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page5", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page5", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page5", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page5", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page5", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page5", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page5", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page5", pSock, pUser );
							break;
					}
					break;
			}
			break;
		case 6:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page6", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page6", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page6", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page6", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page6", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page6", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page6", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page6", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page6", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page6", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page6", pSock, pUser );
							break;
					}
					break;
			}
			break;
		case 7:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page7", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page7", pSock, pUser );
					break;
				case 5:
					switch ( pUser.GetTag( "ORE" ) )
					{
						case 1:
							TriggerEvent( blacksmithID, "page7", pSock, pUser );
							break;
						case 2:
							TriggerEvent( bronzeID, "page7", pSock, pUser );
							break;
						case 3:
							TriggerEvent( copperID, "page7", pSock, pUser );
							break;
						case 4:
							TriggerEvent( agapiteID, "page7", pSock, pUser );
							break;
						case 5:
							TriggerEvent( dullcopperID, "page7", pSock, pUser );
							break;
						case 6:
							TriggerEvent( goldID, "page7", pSock, pUser );
							break;
						case 7:
							TriggerEvent( shadowironID, "page7", pSock, pUser );
							break;
						case 8:
							TriggerEvent( valoriteID, "page7", pSock, pUser );
							break;
						case 9:
							TriggerEvent( veriteID, "page7", pSock, pUser );
							break;
					}
					break;
			}
			break;
		case 8:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page8", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page8", pSock, pUser );
					break;
			}
			break;
		case 9:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page9", pSock, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page9", pSock, pUser );
					break;
			}
			break;
		case 10:
			pSock.CloseGump( gumpID, 0 );
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page10", pSock, pUser );
					break;
			}
			break;
		case 49:
			TriggerEvent( blacksmithID, "target", pSock );
			break;
		case 51:
			TriggerEvent( Carpentry, "target", pSock );
			break;
	}
}