function sendMapDetails( socket, map, Width, Height, xtop, ytop, xbottom, ybottom )
{
	var pUser = socket.currentChar;
	var toSend = new Packet;

	toSend.ReserveSize( 21 );
	toSend.WriteByte( 0, 0xF5 );
	toSend.WriteLong( 1, map.serial );	//map.serial
	toSend.WriteShort( 5, 0x139D);
	toSend.WriteShort( 7, xtop );	    // Left of Top Left
	toSend.WriteShort( 9, ytop );       // Top of Top Left
	toSend.WriteShort( 11, xbottom );   // Right of Bottom Right
	toSend.WriteShort( 13, ybottom );   // Bottom of Bottom Right
	toSend.WriteShort( 15, Width );     // Assuming End.X - Start.X 
	toSend.WriteShort( 17, Height );    // Assuming End.Y - Start.Y (or vice versa?)

	var mapType = 0x00
	switch( pUser.worldnumber )
	{
		case 0:
			mapType = 0x00; // Felucca
			break;
		case 1:
			mapType = 0x01; // Trammel
			break;
		case 2:
			mapType = 0x02; // Ilshenar
			break;
		case 3:
			mapType = 0x03; // Malas
			break;
		case 4:
			mapType = 0x04; // Tokuno
			break;
		case 5:
			mapType = 0x05; // TerMur
			break;
		default:
			mapType = 0x00; // Felucca
			break;
	}
	toSend.WriteShort( 19, mapType ); // World facet
	pUser.SysMessage(worldNum);
	socket.Send( toSend );
	toSend.Free();
}

function PacketRegistration()
{
	RegisterPacket( 0x56, 0x0 );
}

/*
Action Flag command
1: Add Pin
2: Insert New Pin
3: Change Pin
4: Remove Pin
5: Clear Pins
6: Toggle Edit Map (Client)
7: Reply From Server to Action 6*/

function onPacketReceive(pSocket, packetNum, subcommand )
{
	var pUser = pSocket.currentChar;
	var mapItem = CalcItemFromSer( parseInt( pUser.GetTempTag( "parentMapSerial" )));

	var cmd = pSocket.GetByte( 0 );

	if( cmd != packetNum )
		return;

	var pins = [];
	if (mapItem.GetTag("pins")) 
	{
		pins = mapItem.GetTag("pins").split(";");
	}

	pSocket.ReadBytes( 11 );
	var subCmd = pSocket.GetByte( 5 ); // Fetch subCmd

	var editable = mapItem.GetTag( 'editable' );
	switch( subCmd )
	{
		case 1://Append to list of pins
			var pinx = pSocket.GetSWord( 7 );
			var piny = pSocket.GetSWord( 9 );
			mapItem.SetTag("pins", pinx + "," + piny);
			break;
		case 2://Insert into list of pins
			var pinx = pSocket.GetSWord(7);
			var piny = pSocket.GetSWord(9);
			index = pSocket.GetByte(6);

			var pin = [index, pinx, piny];

			// Insert the new pin into the 'pins' array at the specified index
			pins.splice(index, 0, pin.join(","));

			// Update the 'pins' tag in mapItem with the modified array
			mapItem.SetTag("pins", pins.join(","));
			break;
		case 3:// Change pin
			var pinx = pSocket.GetSWord(7);
			var piny = pSocket.GetSWord(9);
			index = pSocket.GetByte(6)

			var pin = [index, pinx, piny];
			if (index < pins.length) 
			{
				pins[index] = pin.join(",");
			}
			mapItem.SetTag("pins", pins.join(","));
			pUser.SysMessage(pins);
			pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		case 4://Remove pin
			var pinx = pSocket.GetSWord(7);
			var piny = pSocket.GetSWord(9);
			index = pSocket.GetByte(6)

			if (index < pins.length) 
			{
				pins.splice(index, 1);
			}
			mapItem.SetTag("pins", pins.join(","));
			pUser.SysMessage( "Sending Sub Command, " + subCmd ); // Debug msg
			break;
		case 5://Clear Pins
			mapItem.SetTag("pins", null);
			break;
		case 6://Toggle Plotting Course
			if ( editable == 0 )
				sendMapEditable( pSocket, mapItem, false );
			break;
		default:
			Console.Warning( "Unhandled subcommand (" + subCmd + ") in packet 0x56." );
			break;
	}
	return;
}

function sendMapCommand( socket, mapItem, command, editable, x, y )
{
	var pUser = socket.currentChar;
	var toSend = new Packet;
	toSend.ReserveSize( 11 );
	toSend.WriteByte( 0, 0x56 );
	toSend.WriteLong( 1, mapItem.serial );    //map.serial
	toSend.WriteByte( 5, command );           // command 1-7
	toSend.WriteByte( 6, editable );          // 1 or 0 
	toSend.WriteShort( 7, x );                // x location of pin on the map
	toSend.WriteShort( 9, y );                // y location of pin on the map
	socket.Send( toSend );
	toSend.Free();
}

function sendMapDisplay( socket, map )
{
	var pUser = socket.currentChar;
	sendMapCommand( socket, map, 0x05, 0, 0, 0 );
}

function sendAddMapPin( socket, map, x, y)
{
	var pUser = socket.currentChar;
	//Remove all Pins
	//sendMapCommand( socket, map, 0x05, 0, 0, 0 );
	// Add Pins
	sendMapCommand( socket, map, 0x01, 1, x, y );
}

function sendInsertPin(socket, map, x, y)
{
	var pUser = socket.currentChar;
	sendMapCommand( socket, map, 0x02, 1, x, y );
}

function sendMapEditable( socket, mapItem, editable )
{
	var pUser = socket.currentChar;
	mapItem.SetTag( 'editable', editable );
	sendMapCommand( socket, mapItem, 0x07, editable ? 1 : 0, 0, 0 );
}