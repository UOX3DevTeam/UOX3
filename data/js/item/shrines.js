function onUseChecked( pUser, iUsed )
{
	var trgSock = pUser.socket;
	var pPoison = pUser.poison;
	if( pUser.dead && pUser.InRange( iUsed, 2 )) 
	{
		pUser.Resurrect();
		pUser.StaticEffect( 0x376A, 10, 16 );
		pUser.SoundEffect( 0x214, true );
	}
	else if( pPoison > 0 && !pUser.atWar && pUser.InRange( iUsed, 2 ))
	{
		pUser.StaticEffect( 0x373A, 0, 15 );
		pUser.SetPoisoned( 0, 0 );
		trgSock.SysMessage( GetDictionaryEntry(1346, socket.language )); //The poison was cured!
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

	if( pUser.dead )
	{
		offset = WriteMenuEntry( toSend, offset, 0x000b, 6195, 0x0020, 0x03E0 ); // Resurrect
	}

	if( coreShardEra >= EraStringToNum("aos") && titheEnabled )
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
				socket.SysMessage( "That is too far away." );
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
				socket.SysMessage( "That is too far away." );
			}
			else
			{
				pUser.Resurrect();
				pUser.StaticEffect( 0x376A, 10, 16 );
				pUser.SoundEffect( 0x214, true );
			}
			break;
		case 12://Tithe Gold
			break;
	}
	return false;
}

function _restorecontext_() {}