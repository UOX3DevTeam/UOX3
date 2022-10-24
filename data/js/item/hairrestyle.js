const enableTooltips = true;
const tooltipClilocID = 1042971; // Cliloc ID to use for tooltips. 1042971 should work with clients from ~v3.0.x to modern day

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	if( iUsed.container != null )
	{
		if( pUser.id == 0x191 )
		{
			var hsGump = new Gump;
			hsGump.AddPage( 0 );
			hsGump.AddBackground( 100, 10, 400, 385, 0xA28 );
			hsGump.AddButton( 175, 340, 0xfa5, 1, 0, 0 );
			hsGump.AddHTMLGump( 100, 25, 400, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 2075, socket.language ) + "</CENTER>" ); // HAIRSTYLE SELECTION MENU
			hsGump.AddHTMLGump( 210, 342, 90, 35, 0, 0, GetDictionaryEntry( 2090, socket.language )); // CANCEL
			hsGump.AddBackground( 175, 50, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2084, socket.language )); // Short
			hsGump.AddGump( 110, 10, 0xed1c ); // Short
			hsGump.AddButton( 140, 60, 0xfa5, 1, 0, 1 );
			hsGump.AddBackground( 175, 120, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2085, socket.language )); // Long
			hsGump.AddGump( 110, 80, 0xed1d ); // Long
			hsGump.AddButton( 140, 130, 0xfa5, 1, 0, 2 );
			hsGump.AddBackground( 175, 190, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2086, socket.language )); // Ponytail
			hsGump.AddGump( 110, 150, 0xed1e ); // Ponytail
			hsGump.AddButton( 140, 200, 0xfa5, 1, 0, 3 );
			hsGump.AddBackground( 175, 260, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2087, socket.language )); // Mohawk
			hsGump.AddGump( 110, 220, 0xed27 ); // Mohawk
			hsGump.AddButton( 140, 270, 0xfa5, 1, 0, 4 );
			hsGump.AddBackground( 275, 50, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2080, socket.language )); // Pageboy
			hsGump.AddGump( 210, 10, 0xED26 ); // Pageboy
			hsGump.AddButton( 240, 60, 0xfa5, 1, 0, 5 );
			hsGump.AddBackground( 275, 120, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2089, socket.language )); // Buns
			hsGump.AddGump( 210, 80, 0xed28 ); // Buns
			hsGump.AddButton( 240, 130, 0xfa5, 1, 0, 6 );
			hsGump.AddBackground( 275, 190, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2082, socket.language )); // Pigtails
			hsGump.AddGump( 210, 150, 0xede6 ); // Pigtails
			hsGump.AddButton( 240, 200, 0xfa5, 1, 0, 7 );
			hsGump.AddBackground( 275, 260, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2083, socket.language )); // Topknot
			hsGump.AddGump( 210, 220, 0xED29 ); // Topknot
			hsGump.AddButton( 240, 270, 0xfa5, 1, 0, 8 );
			hsGump.AddBackground( 375, 50, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2088, socket.language )); // Curly
			hsGump.AddGump( 310, 10, 0xed25 ); // Curly
			hsGump.AddButton( 340, 60, 0xfa5, 1, 0, 9 );
			hsGump.AddButton( 275, 340, 0xfa5, 1, 0, 11 );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2079, socket.language )); // Bald (If you choose to go bald, you will lose your hair color)
			hsGump.AddHTMLGump( 310, 342, 90, 35, 0, 0, "Bald" );
			hsGump.Send( socket ); // send this gump to client now
			hsGump.Free(); // clear this gump from uox-memory
			return false;
		}
		else if( pUser.id == 0x190 )
		{
			var hsGump = new Gump;
			hsGump.AddPage( 0 );
			hsGump.AddBackground( 100, 10, 400, 385, 0xA28 );
			hsGump.AddButton( 175, 340, 0xfa5, 1, 0, 0 );
			hsGump.AddHTMLGump( 100, 25, 400, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 2075, socket.language ) + "</CENTER>" ); // HAIRSTYLE SELECTION MENU
			hsGump.AddHTMLGump( 210, 342, 90, 35, 0, 0,  GetDictionaryEntry( 2090, socket.language )); // CANCEL
			hsGump.AddBackground( 175, 50, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2084, socket.language )); // Short
			hsGump.AddGump( 110, 10, 0xC60C ); // Short
			hsGump.AddButton( 140, 60, 0xfa5, 1, 0, 1 );
			hsGump.AddBackground( 175, 120, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2085, socket.language )); // Long
			hsGump.AddGump( 110, 80, 0xc60d ); // Long
			hsGump.AddButton( 140, 130, 0xfa5, 1, 0, 2 );
			hsGump.AddBackground( 175, 190, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2086, socket.language )); // Ponytail
			hsGump.AddGump( 110, 150, 0xc60e ); // Ponytail
			hsGump.AddButton( 140, 200, 0xfa5, 1, 0, 3 );
			hsGump.AddBackground( 175, 260, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2087, socket.language )); // Mohawk
			hsGump.AddGump( 110, 220, 0xC60F ); // Mohawk
			hsGump.AddButton( 140, 270, 0xfa5, 1, 0, 4 );
			hsGump.AddBackground( 275, 50, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2080, socket.language )); // Pageboy
			hsGump.AddGump( 210, 10, 0xED26 ); // Pageboy
			hsGump.AddButton( 240, 60, 0xfa5, 1, 0, 5 );
			hsGump.AddBackground( 275, 120, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2081, socket.language )); // Receding
			hsGump.AddGump( 210, 80, 0xEDE5 ); // Receding
			hsGump.AddButton( 240, 130, 0xfa5, 1, 0, 10 );
			hsGump.AddBackground( 275, 190, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2082, socket.language )); // Pigtails
			hsGump.AddGump( 210, 150, 0xede6 ); // Pigtails
			hsGump.AddButton( 240, 200, 0xfa5, 1, 0, 7 );
			hsGump.AddBackground( 275, 260, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2083, socket.language )); // Topknot
			hsGump.AddGump( 210, 220, 0xED29 ); // Topknot
			hsGump.AddButton( 240, 270, 0xfa5, 1, 0, 8 );
			hsGump.AddBackground( 375, 50, 50, 50, 0xA3C );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2088, socket.language )); // Curly
			hsGump.AddGump( 310, 10, 0xed25 ); // Curly
			hsGump.AddButton( 340, 60, 0xfa5, 1, 0, 9 );
			hsGump.AddButton( 275, 340, 0xfa5, 1, 0, 11 );
			hsGump.AddHTMLGump( 310, 342, 90, 35, 0, 0, "Bald" );
			hsGump.AddToolTip( tooltipClilocID, socket, GetDictionaryEntry( 2079, socket.language )); // Bald (If you choose to go bald, you will lose your hair color)
			hsGump.Send( socket ); // send this gump to client now
			hsGump.Free(); // clear this gump from uox-memory
			return false;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
	}
	return false;
}

function onGumpPress( pSock, myButtonID )
{
	var myPlayer = pSock.currentChar;
	var iUsed = pSock.tempObj;
	switch( myButtonID )
	{
		case 0: // Refuse
			break;
		case 1: // short
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x203b", 1, "ITEM", true );
				if( itemMade )
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 2: // Long
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x203c", 1, "ITEM", true );
				if( itemMade )
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 3: // Ponytail
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x203d", 1, "ITEM", true );
				if( itemMade )
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 4: // Mohawk
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x2044", 1, "ITEM", true );
				if( itemMade)
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;

		case 5: // Pageboy
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x2045", 1, "ITEM", true );
				if( itemMade)
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 6: // Buns
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x2046", 1, "ITEM", true );
				if( itemMade)
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 7: // 2-tails
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x2049", 1, "ITEM", true );
				if( itemMade)
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 8: // Topknot
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x204A", 1, "ITEM", true );
				if( itemMade)
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 9: // Curly
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x2047", 1, "ITEM", true );
				if( itemMade )
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 10: // Receding
			// Check if player currently has item equipped in layer 2
			var tempObj = myPlayer.FindItemLayer( 0x0B );

			// If he has, unequip it
			if( tempObj != null )
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2076, myPlayer.socket.language )); // You need to be bald to change your hair.
			}
			else
			{
				// If there is no item equipped in layer B, create one and equip it on character
				var itemMade = CreateDFNItem( myPlayer.socket, myPlayer, "0x2048", 1, "ITEM", true );
				if( itemMade )
				{
					itemMade.container = myPlayer;
					itemMade.layer = 0x0B;
					iUsed.Delete();
				}
				else
				{
					myPlayer.SysMessage( GetDictionaryEntry( 2077, myPlayer.socket.language )); // Failed to change hairstyle.
				}
			}
			break;
		case 11: // bald
			var tempObj = myPlayer.FindItemLayer( 0x0B );
			if( tempObj != null )
			{
				tempObj.Delete();
			}
			else
			{
				myPlayer.SysMessage( GetDictionaryEntry( 2078, myPlayer.socket.language )); // You are already bald - hairstyle change aborted.
			}
			break;
	}
}
