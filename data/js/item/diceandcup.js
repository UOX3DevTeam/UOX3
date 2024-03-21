function onUseChecked( pUser, iUsed )
{
	Roll( pUser, iUsed );
	return false;
}

function Roll( pUser, iUsed )
{
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	var iDice1 = RandomNumber( 1, 6 );
	var iDice2 = RandomNumber( 1, 6 );
	
	var pName = pUser.name;
	
	var diceResults = GetDictionaryEntry( 2722 ) // %s rolls the dice and gets a %i and %d.
	diceResults = diceResults.replace( /%s/gi, pName );
	diceResults = diceResults.replace( /%i/gi, iDice1 );
	diceResults = diceResults.replace( /%d/gi, iDice2 );

	if( iUsed.container != null )
	{
		pUser.EmoteMessage( diceResults );
	}
	else
	{
		iUsed.TextMessage( diceResults );
	}
}

function onContextMenuRequest( socket, targObj )
{
	// handle your own packet with context menu here
	var pUser = socket.currentChar;
	var offset = 12;
	var numEntries = 1;

	// Prepare packet
	var toSend = new Packet();
	var packetLen = ( 12 + ( numEntries * 8 ));
	toSend.ReserveSize( packetLen );
	toSend.WriteByte( 0, 0xBF );
	toSend.WriteShort( 1, packetLen );
	toSend.WriteShort( 3, 0x14 ); // subCmd
	toSend.WriteShort( 5, 0x0001 ); // 0x0001 for 2D client, 0x0002 for KR (maybe this needs to be 0x0002?)
	toSend.WriteLong( 7, targObj.serial );
	toSend.WriteByte( 11, numEntries ); // Number of entries

	toSend.WriteShort( offset, 0x000A );    // Unique ID
	toSend.WriteShort( offset += 2, 6244 ); // Roll Dice
	toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
	toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text

	//Send packet
	socket.Send( toSend );
	toSend.Free();

	return false;
}

function onContextMenuSelect( socket, targObj, popupEntry )
{
	var pUser = socket.currentChar;
	// Handle checking popupEntry here, to make sure it matches one of the context menu options sent in custom packet in onContextMenuRequest
	switch( popupEntry )
	{
		case 10:
			Roll( pUser, targObj );
			break;
	}
	return false;
}