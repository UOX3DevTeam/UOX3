/// <reference path="../../definitions.d.ts" />
// @ts-check

// Shared context-menu packet builder.
// Usage: TriggerEvent( 18001, "modifyContextMenu", socket, targObj, entries, useKR );
/**
 * Builds and sends the enhanced-client or legacy context-menu packet for an object.
 * @type { ( socket: Socket, targObj: Character | Item, entries: Array<{ id: number, text: number, flags: number, hue: number }>, useKR: boolean ) => boolean }
 */
function modifyContextMenu( socket, targObj, entries, useKR )
{
	if( socket == null || !ValidateObject( targObj ) || entries == null )
	{
		return false;
	}

	let count = entries.length | 0;
	if( count < 1 )
	{
		return false;
	}
	if( count > 255 )
	{
		count = 255;
	}

	const packetLen = 12 + ( count * 8 );
	const packet = new Packet();
	packet.ReserveSize( packetLen );
	packet.WriteByte( 0, 0xBF );
	packet.WriteShort( 1, packetLen );
	packet.WriteShort( 3, 0x14 );
	packet.WriteShort( 5, useKR ? 0x0002 : 0x0001 );
	packet.WriteLong( 7, targObj.serial );
	packet.WriteByte( 11, count );

	let off = 12;
	if( useKR )
	{
		for( let i = 0; i < count; ++i )
		{
			const entry = entries[i];
			let textID = entry.text | 0;
			if( textID >= 0 && textID <= 65535 )
			{
				textID += 3000000;
			}

			packet.WriteLong( off, textID );
			packet.WriteShort( off + 4, entry.id | 0 );
			packet.WriteShort( off + 6, entry.flags | 0 );
			off += 8;
		}
	}
	else
	{
		for( let j = 0; j < count; ++j )
		{
			const legacyEntry = entries[j];
			let cliloc = legacyEntry.text | 0;
			let hue = legacyEntry.hue | 0;
			if( cliloc < 0 )
			{
				cliloc = 0;
			}
			if( cliloc > 65535 )
			{
				cliloc = 65535;
			}
			if( hue < 0 )
			{
				hue = 0;
			}
			if( hue > 65535 )
			{
				hue = 65535;
			}

			packet.WriteShort( off, legacyEntry.id | 0 );
			packet.WriteShort( off + 2, cliloc );
			packet.WriteShort( off + 4, legacyEntry.flags | 0 );
			packet.WriteShort( off + 6, hue );
			off += 8;
		}
	}

	socket.Send( packet );
	packet.Free();
	return true;
}
