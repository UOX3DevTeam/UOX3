//usage: TriggerEvent(18001, "modifyContextMenu", socket, targObj, list, useKR);

function modifyContextMenu( socket, targObj, entries, useKR )
{
	var count = entries.length | 0;
	var packetLen = 12 + ( count * 8 );

	var packet = new Packet();
	packet.ReserveSize( packetLen );

	packet.WriteByte( 0, 0xBF );
	packet.WriteShort( 1, packetLen );
	packet.WriteShort( 3, 0x14);                           // context menu subcmd
	packet.WriteShort( 5, useKR ? 0x0002 : 0x0001 );        // KR/EC vs 2D
	packet.WriteLong( 7, targObj.serial );
	packet.WriteByte( 11, count );

	var off = 12;

	if( useKR )
	{
		for( var i = 0; i < count; i++ )
		{
			var e = entries[i];
			var textID = ( e.text <= 65535 ) ? ( 3000000 + ( e.text | 0 )) : ( e.text | 0 );
			packet.WriteLong( off, textID );
			packet.WriteShort( off + 4, e.id | 0 );
			packet.WriteShort( off + 6, e.flags | 0 );
			off += 8;
		}
	}
	else
	{
		// 2D entry layout EXACTLY like the legacy code (8 bytes):
		// WORD id, WORD cliloc, WORD flags, WORD hue
		for( var j = 0; j < count; j++ )
		{
			var e2 = entries[j];

			var id     = e2.id   | 0;
			var cliloc = e2.text | 0;
			if( cliloc < 0 )
				cliloc = 0;

			if( cliloc > 65535 )
				cliloc = 65535;

			var flags  = e2.flags| 0;
			var hue    = e2.hue  | 0;
			if( hue < 0 )
				hue = 0;

			if( hue > 65535 )
				hue = 65535;

			packet.WriteShort( off,     id );
			packet.WriteShort( off + 2, cliloc );
			packet.WriteShort( off + 4, flags );
			packet.WriteShort( off + 6, hue );
			off += 8;
		}
	}

	socket.Send( packet );
	packet.Free();
}