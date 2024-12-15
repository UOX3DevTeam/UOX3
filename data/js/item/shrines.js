function onUseUnChecked(pUser, iUsed)
{
	var userSocket = pUser.socket;

	if( !pUser.InRange( iUsed, 2 ))
	{
		socket.SysMessage( GetDictionaryEntry( 400, socket.language )); // That is too far away.
		return false;
	}

	// Resurrection logic for non-murderers and murderers at Chaos Shrine
	if( pUser.dead )
	{
		if( !pUser.murderer || ( pUser.murderer && iUsed.morex == 1 ))
		{
			ResurrectGump( pUser, iUsed );
		}
	}
	// Poison curing logic for non-murderers
	else if( !pUser.murderer && pUser.poison > 0 && !pUser.atWar ) 
	{
		pUser.StaticEffect( 0x373A, 0, 15 );
		pUser.SetPoisoned( 0, 0 );
		userSocket.SysMessage(GetDictionaryEntry( 1346, userSocket.language )); // The poison was cured!
	}

	// Prevent hard-code from running
	return false;
}

function onContextMenuRequest( socket, shrine )
{
	var coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));
	var pUser = socket.currentChar;

	// Determine the number of entries based on era and user state
	var numEntries = 0; // Start with zero and increment as needed
	var addKarmaOption = !pUser.account.isYoung; // Check if the player is not young
	var titheEnabled = false;

	if( addKarmaOption )
	{
		numEntries++;
	}
	if( pUser.dead )
	{
		numEntries++;
	}
	if( coreShardEra >= EraStringToNum( "aos" ) && titheEnabled )
	{
		numEntries++;
	}

	// Prepare the packet
	var packetLen = 12 + ( numEntries * 8 );
	var toSend = new Packet();
	toSend.ReserveSize( packetLen );

	// Write initial packet structure
	toSend.WriteByte( 0, 0xBF );
	toSend.WriteShort( 1, packetLen );
	toSend.WriteShort( 3, 0x14 ); // subCmd
	toSend.WriteShort( 5, 0x0001); // 0x0001 for 2D client
	toSend.WriteLong( 7, shrine.serial );
	toSend.WriteByte( 11, numEntries ); // Number of entries

	var offset = 12;

	// Add context menu entries
	if( addKarmaOption )
	{
		offset = WriteMenuEntry( toSend, offset, 0x000a, pUser.karmaLocked ? 6197 : 6196, 0x0020, 0x03E0 ); // Lock/Unlock Karma
	}

	if( pUser.dead && !pUser.murderer || ( pUser.murderer && iUsed.morex == 1 ))
	{
		offset = WriteMenuEntry( toSend, offset, 0x000b, 6195, 0x0020, 0x03E0 ); // Resurrect
	}

	if( coreShardEra >= EraStringToNum( "aos" ) && titheEnabled && !pUser.murderer )
	{
		offset = WriteMenuEntry(toSend, offset, 0x000c, 6198, 0x0020, 0x03E0); // Tithe Gold
	}

	// Send the packet
	socket.Send( toSend );
	toSend.Free();
}

function WriteMenuEntry( packet, offset, uniqueID, clilocID, flag, hue )
{
	packet.WriteShort( offset, uniqueID );
	offset += 2;
	packet.WriteShort( offset, clilocID );
	offset += 2;
	packet.WriteShort( offset, flag );
	offset += 2;
	packet.WriteShort( offset, hue );
	offset += 2; // Move to the next entry
	return offset;
}

function onContextMenuSelect( socket, shrine, popupEntry )
{
	var pUser = socket.currentChar;
	switch( popupEntry )
	{
		case 10://Karma Locking
			if( !pUser.InRange( shrine, 2 )) 
			{
				socket.SysMessage( GetDictionaryEntry( 400, socket.language )); // That is too far away.
				return;
			}

			if( pUser.karmaLocked == true ) 
			{
				pUser.karmaLocked = false;
				socket.SysMessage( GetDictionaryEntry( 5753, socket.language )); // Your karma has been unlocked. Your karma can be raised again.
			}
			else
			{
				pUser.karmaLocked = true;
				socket.SysMessage( GetDictionaryEntry( 5752, socket.language )); // Your karma has been locked. Your karma can no longer be raised.
			}
			break;
		case 11:// Resurrect
			if( !pUser.dead ) 
			{
				return;
			}

			if( !pUser.InRange( shrine, 2 ))
			{
				socket.SysMessage( GetDictionaryEntry( 400, socket.language )); // That is too far away.
				return;
			}
			else
			{
				ResurrectGump( pUser, shrine );
				pUser.frozen = true;
			}
			break;
		case 12://Tithe Gold
			break;
	}
	return false;
}

function ResurrectGump( pUser, iUsed )
{
	var message = 0;
	if( iUsed.morex == 1 )
		message = 5761; // It is possible for you to be resurrected here at this Shrine. Do you wish to try?<br>CONTINUE - You chose to try to come back to life now.<br>CANCEL - You prefer to remain a ghost for now.
	else
		message = 5762; // It is possible for you to be resurrected here at this shrine of the Virtues. Do you wish to try?<br>CONTINUE - You chose to try to come back to life now.<br>CANCEL - You prefer to remain a ghost for now.

	var ResurrectGump = new Gump;
	var pSocket = pUser.socket;
	ResurrectGump.AddBackground( 0, 0, 400, 350, 2600 );

	ResurrectGump.AddHTMLGump( 0, 20, 400, 35, false, false, GetDictionaryEntry( 5763, pSocket.language ));// Ressurrect

	ResurrectGump.AddHTMLGump( 50, 55, 300, 140, true, true, GetDictionaryEntry( message, pSocket.language ));

	ResurrectGump.AddButton( 200, 227, 4005, 4007, 1, 0, 0 );
	ResurrectGump.AddHTMLGump( 235, 230, 110, 35, false, false, GetDictionaryEntry( 18762, pSocket.language )); // CANCEL

	ResurrectGump.AddButton( 65, 227, 4005, 4007, 1, 0, 1 );
	ResurrectGump.AddHTMLGump( 100, 230, 110, 35, false, false, GetDictionaryEntry( 2708, pSocket.language )); // CONTINUE

	ResurrectGump.Send(pSocket );
	ResurrectGump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	switch( pButton )
	{
		case 0:
			// abort and do nothing
			pUser.frozen = false;
			break;
		case 1:
			pUser.Resurrect();
			TriggerEvent( 8002, "ResurrectFamePenalty", pUser );
			TriggerEvent( 8002, "ResurrectMurderPenalty", pUser );
			pUser.StaticEffect( 0x376A, 10, 16 );
			pUser.SoundEffect( 0x214, true );
			pSock.SysMessage( GetDictionaryEntry( 390, pSock.language )); // You have been resurrected.
			pUser.frozen = false;
			break;
	}
}

function _restorecontext_() {}