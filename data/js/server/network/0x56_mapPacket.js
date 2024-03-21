function SendMapDetails( socket, map, Width, Height, xtop, ytop, xbottom, ybottom )
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

	var cmd = pSocket.GetByte( 0 );

	if( cmd != packetNum )
		return;

	pSocket.ReadBytes( 11 );
	var subCmd = pSocket.GetByte( 5 ); // Fetch subCmd

	var mapItem = CalcItemFromSer( pSocket.GetDWord( 1 ));
    if( !ValidateObject( mapItem )) // not a valid map item?
        return;

	var maxPins = 50;
	var pins = [];
	if( mapItem.GetTag( "pins" ))
	{
		pins = mapItem.GetTag( "pins" ).split( ";" );
	}

	// Don't allow editing treasure maps!
	if( mapItem.HasScriptTrigger( 5402 ))
		return;

	var editable = mapItem.GetTag( 'editable' );
	switch( subCmd )
	{
		case 1:// Insert new pin into list of pins
			index = pSocket.GetByte( 6 ); // Not sent by client? Always 0
			var pinx = pSocket.GetSWord( 7 );
			var piny = pSocket.GetSWord( 9 );

			if( pins.length >= maxPins )
            {
                pSocket.SysMessage( "Max number of pins reached!" );
                SendMapEditable( pSocket, mapItem, false );
                return;
            }

			var pin = pinx + "," + piny;

			// Insert the new pin into the 'pins' array at the specified index
			pins.push(pin);

			// Update the 'pins' tag in mapItem with the modified array
			mapItem.SetTag( "pins", pins.join( ";" ));
			break;
		case 2:// [UNUSED - not sent by client] Append to list of pins
			/*var pinx = pSocket.GetSWord( 7 );
			var piny = pSocket.GetSWord( 9 );
			mapItem.SetTag("pins", pinx + "," + piny);*/
			break;
		case 3:// [UNUSED - not sent by client] Change pin
			/*var pinx = pSocket.GetSWord(7);
			var piny = pSocket.GetSWord(9);
			index = pSocket.GetByte(6)

			var pin = [index, pinx, piny];
			if (index < pins.length) 
			{
				pins[index] = pin.join(",");
			}
			mapItem.SetTag("pins", pins.join(","));*/
			break;
		case 4:// [UNUSED - not sent by client] Remove pin
			/*var pinx = pSocket.GetSWord(7);
			var piny = pSocket.GetSWord(9);
			index = pSocket.GetByte(6)

			if (index < pins.length) 
			{
				pins.splice(index, 1);
			}
			mapItem.SetTag("pins", pins.join(","));*/
			break;
		case 5:// Clear All Pins
			mapItem.SetTag("pins", null);
			break;
		case 6:// Toggle Plotting Course
			if( !editable )
			{
				mapItem.SetTag( 'editable', !editable );
				SendMapEditable( pSocket, mapItem, !editable );
			}
			break;
		default:
			Console.Warning( "Unhandled subcommand (" + subCmd + ") in packet 0x56." );
			break;
	}
	return;
}

function SendMapCommand( socket, mapItem, command, editable, x, y )
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

function SendMapDisplay( socket, map )
{
	var pUser = socket.currentChar;
	SendMapCommand( socket, map, 0x05, 0, 0, 0 );
}

function SendAddMapPin( socket, map, x, y)
{
	var pUser = socket.currentChar;
	// Add Pins
	SendMapCommand( socket, map, 0x01, 1, x, y );
}

function SendInsertPin(socket, map, x, y)
{
	var pUser = socket.currentChar;
	SendMapCommand( socket, map, 0x02, 1, x, y );
}

function SendChangePin(socket, map, x, y)
{
	var pUser = socket.currentChar;
	SendMapCommand( socket, map, 0x03, 1, x, y );
}

function SendMapEditable( socket, mapItem, editable )
{
	var pUser = socket.currentChar;
	SendMapCommand( socket, mapItem, 0x07, editable ? 1 : 0, 0, 0 );
}