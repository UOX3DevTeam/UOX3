function onUseChecked( pUser, iUsed )
{
	var srcSock = pUser.socket;
	var campfire = 0;

	// Store gumpID for gumps opened by this script in a global variable for later use
	if( iUsed )
	{
		var scriptID = 5021;
		gumpID = scriptID+0xffff;
	}
	else
	{
		return false;
	}

	// Find owner of root container iUsed is contained in, if any
	var packOwner = GetPackOwner( iUsed, 0 );
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	// If bedroll is rolled up
	if( iUsed.id == 0x0a57 || iUsed.id == 0x0a58 || iUsed.id == 0x0a59 )
	{
		// If iUsed is in a container, and owner of root container equals pUser
		if( packOwner != null && packOwner.serial == pUser.serial )
		{
			iUsed.container = null;
			iUsed.Teleport( pUser.x, pUser.y, pUser.z, pUser.worldnumber );
			iUsed.decaytime = 0;
		}
		else // Perform distance check
		{
			if( !iUsed.InRange( pUser, 2 ))
			{
				pUser.SysMessage( GetDictionaryEntry( 482, srcSock.language )); //You need to be closer to use that.
				return false;
			}
		}

		// Unroll bedroll
		if( iUsed.id == 0x0a57 || iUsed.id == 0x0a58 )
		{
			iUsed.id = 0x0a55;
		}
		else if( iUsed.id == 0x0a59 )
		{
			iUsed.id = 0x0a56;
		}
	}
	else if( iUsed.id == 0x0a55 || iUsed.id == 0x0a56 ) // If bedroll is unrolled
	{
		if( pUser.online && !pUser.criminal && !pUser.atWar && pUser.attacker == null )
		{
			// Check that bedroll is not in a container
			packOwner = GetPackOwner( iUsed, 0 );
			if( packOwner == null )
			{
				// Distance-check to bedroll -> Open logout gump
				if( iUsed.InRange( pUser, 2 ))
				{
					// Check for nearby campfire within a radius of 12 tiles, and if any of those are marked as secure for the player
					var foundCampfire = AreaItemFunction( "findCampfire", pUser, 12 );
					if( foundCampfire )
					{
						// Store iUsed in tempObj on player socket, and current user as custom tag on iUsed
						pUser.socket.tempObj = iUsed;
						iUsed.SetTag( "inuseby", pUser.serial & 0x00FFFFFF );
						// Start timer on item instead of character, so it will be safe to do KillTimers on object later on
						iUsed.StartTimer( 10000, 1, false );
						displayGump( srcSock, pUser );
					}
					else
					{
						pUser.SysMessage( GetDictionaryEntry( 2704, srcSock.language )); // There are no secure campfires nearby.
					}
				}
				else
				{
					pUser.SysMessage( GetDictionaryEntry( 482, srcSock.language )); //You need to be closer to use that.
				}
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 2705, srcSock.language )); // This needs to be lying on the ground.
			}
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 2706, srcSock.language )); // You cannot do this while in combat, or while having a criminal flag.
		}

		// Roll up bedroll
		if( iUsed.id == 0x0a55 )
		{
			iUsed.id = 0x0a58;
		}
		else if( iUsed.id == 0x0a56 )
		{
			iUsed.id = 0x0a59;
		}
		iUsed.container = pUser.pack;
		iUsed.decaytime = 0;
	}
	return false;
}

function findCampfire( pUser, trgItem )
{
	if( trgItem && trgItem.id == 0x0de3 && trgItem.GetTag( "securecamp" ) == true )
	{
		// Check if campfire has a tag named after the player's serial
		// If it does, return true, as this campfire is secure for the player
		var checkSecureStatus = trgItem.GetTag( pUser.serial & 0x00FFFFFF );
		if( checkSecureStatus )
		{
			return true;
		}
	}
	return false;
}

function displayGump(srcSock, pUser)
{
	var myGump = new Gump;

	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 400, 350, 0xA28 );
	myGump.AddText( 65, 10, 0, GetDictionaryEntry( 2707, srcSock.language )); // Logging out via camping
	myGump.AddButton( 26, 300, 0xfa5, 1, 0, 1 );
	myGump.AddButton( 280, 300, 0xfa5, 1, 0, 0 );
	myGump.AddText( 60, 300, 0, GetDictionaryEntry( 2708, srcSock.language )); // CONTINUE
	myGump.AddText( 315, 300, 0, GetDictionaryEntry( 2709, srcSock.language )); // CANCEL
	myGump.AddXMFHTMLGump( 60, 60, 300, 200, 1011016, true, true ); //Secure camp logout text
	myGump.Send( srcSock );
	myGump.Free();
}

function onGumpPress(srcSock, myButtonID )
{
	var srcChar = srcSock.currentChar;
	var iUsed = srcSock.tempObj;
	switch( myButtonID )
	{
		case 0: // User cancelled manually, so kill timer to auto-close gump, and reset tempObj
			srcSock.SysMessage( GetDictionaryEntry( 2710, srcSock.language )); // You cancel the logout.
			srcSock.tempObj = null;
			if( iUsed )
			{
				iUsed.KillTimers();
				iUsed.SetTag( "inuseby", null );
			}
			break;
		case 1: // Log out button
			srcSock.SysMessage( GetDictionaryEntry( 2711, srcSock.language )); // Logging out.
			if( iUsed )
			{
				iUsed.KillTimers();
				iUsed.SetTag( "inuseby", null );
			}
			srcSock.Disconnect();
			break;
		default:
			break;
    }
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 && timerObj )
	{
		//Close secure logout-gump after 10 seconds
		var pUser = CalcCharFromSer( timerObj.GetTag( "inuseby" ) & 0x00FFFFFF );
		var socket = pUser.socket;
		var pStream = new Packet;
		pStream.ReserveSize( 13 );
		pStream.WriteByte( 0, 0xBF ); //Command: Packet 0xBF - General Information Packet
		pStream.WriteShort( 1, 13 ); // Packet length
		pStream.WriteShort( 3, 0x04 ); //Subcommand 0x04 - Close Generic Gump
		pStream.WriteLong( 5, gumpID ); //dialogID - which gump to destroy
		pStream.WriteLong( 9, 0x0 ); //buttonID // response buttonID for packet 0xB1
		socket.Send( pStream );
		pStream.Free();
		onGumpPress( socket, 0 ); //Fake-push button 0, response buttonID in packet seems broken
	}
}