function onCreateDFN( objMade, objType )
{
	const maxCharges = 30;
	if( objType == 0 ) 
	{
		var Charges = 0;
		switch( RandomNumber( 0, 2 ))
		{
			case 0: Charges = 3; break;
			case 1: Charges = 6; break;
			case 2: Charges = 9; break;
		}
		objMade.SetTag( "charges", Charges )
		objMade.SetTag( "maxCharges", maxCharges )
	}
}

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var Charges = iUsed.GetTag( "charges" )
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
			return false;
		}

		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial ) 
		{
			socket.SysMessage( GetDictionaryEntry( 19070, socket.language )); // The bag of sending must be in your backpack.
			return false;
		}
		else if( Charges == 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 0, socket.language)); // This item is out of charges.
			return false;
		}
		else
		{
			socket.tempObj = iUsed;
			socket.CustomTarget( 0 );
			iUsed.Refresh();
		}
	}
	return false;
}

function onCallback0( socket, myTarget)
{
	var pUser = socket.currentChar;
	var requiredCharges = 1;
	var iUsed = socket.tempObj;
	var Charges = parseInt( iUsed.GetTag( "charges" ));
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 19070, socket.language )); // The bag of sending must be in your backpack
		return false;
	}
	else if( Charges == 0 )
	{
		pUser.SysMessage( GetDictionaryEntry( 0, socket.language )); // This item is out of charges.
		return false;
	}

	if( !socket.GetWord( 1 ) && myTarget.isItem )
	{
		var bankBox = pUser.FindItemLayer( 29 );
		if( !ValidateObject( bankBox ))
		{
			socket.SysMessage( GetDictionaryEntry( 19071, socket.language ));// Error: No valid bankbox found! Please contact a GM for assistance.
			return false;
		}
		else if( !ValidateObject( myTarget.container ) || myTarget.container != pUser.pack )
		{
			socket.SysMessage( GetDictionaryEntry( 19072, socket.language )); // You may only send items from your backpack to your bank box.
			return false;
		}
		else if( myTarget.type == 1 || myTarget == iUsed )
		{
			socket.SysMessage( GetDictionaryEntry( 19073, socket.language )); // You may only send items from your backpack to your bank box.
			return false;
		}
		else if( bankBox.totalItemCount >= bankBox.maxItems )
		{
			socket.SysMessage( GetDictionaryEntry( 19074, socket.language )); // Your bank box is full.
			return false;
		}
		else if( bankBox.weight > bankBox.weightMax )
		{
			socket.SysMessage( GetDictionaryEntry( 9183, socket.language )); // Your bank box is overloaded and cannot hold any more weight.
			return false;
		}
		else if( requiredCharges > Charges )
		{
			socket.SysMessage( GetDictionaryEntry( 19075, socket.language )); // You don't have enough charges to send that much weight
			return false;
		}
		else
		{
			iUsed.SetTag( "charges", Charges - requiredCharges );
			socket.SysMessage( GetDictionaryEntry( 19076, socket.language )); // The item was placed in your bank box.
			myTarget.container = bankBox;
			iUsed.Refresh();
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 19077, socket.language )); // The bag of sending rejects that item.
		return false;
	}
}

function onTooltip( iUsed, socket )
{
	var tooltipText = "";
	var Charges = parseInt( iUsed.GetTag( "charges" ));
	tooltipText = GetDictionaryEntry( 9252, socket.language ) + " " + Charges;// Charges:
	return tooltipText;
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
	toSend.WriteShort( 3, 0x14); // subCmd
	toSend.WriteShort( 5, 0x0001); // 0x0001 for 2D client, 0x0002 for KR (maybe this needs to be 0x0002?)
	toSend.WriteLong( 7, targObj.serial );
	toSend.WriteByte( 11, numEntries ); // Number of entries

	toSend.WriteShort( offset, 0x000a );    // Unique ID
	toSend.WriteShort( offset += 2, 6189 ); // Use the Bag of Sending
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
			onUseChecked( pUser, targObj );
			break;
	}
	return false;
}