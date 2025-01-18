// how to use
// TriggerEvent( 2204, "AddBuff", mChar, buffIcon, priCliloc, scndCliloc, seconds, " " + ourObj.name ); name sent
// TriggerEvent( 2204, "AddBuff", mChar, buffIcon, priCliloc, scndCliloc, seconds, " " + "blah blah msg" ); any msg sent
// TriggerEvent( 2204, "AddBuff", mChar, buffIcon, priCliloc, scndCliloc, seconds, " " );//No text msg sent
// TriggerEvent( 2204, "AddBuff", mChar, buffIcon, priCliloc, scndCliloc, 0, " " );// No msg no timer sent

const BuffEnabled = true; // Turn buffs on and off here.

function AddBuff( pUser, iconID, priCliloc, scndCliloc, seconds, stringData ) 
{
	if( !BuffEnabled )
	{
		return false;
	}

	var pSocket = pUser.socket;
	if( seconds >= 1 )
	{
		pUser.StartTimer( seconds * 1000, iconID, 2204 );
	}

	var pStream = new Packet;
	var hasArgs = ( stringData.length != 0 );

	var utf16Bytes = [];

	if( hasArgs )
	{
		for( var i = 0; i < stringData.length; ++i )
		{
			var charCode = stringData.charCodeAt( i );
			utf16Bytes.push( charCode & 0xFF ); // lower byte
			utf16Bytes.push( ( charCode >> 8 ) & 0xFF ); // upper byte
		}
	}

	var size = hasArgs ? ( 48 + utf16Bytes.length + 2 ) : 44;
	pStream.ReserveSize( size );
	pStream.WriteByte( 0, 0xDF );
	pStream.WriteShort( 1, size );
	pStream.WriteLong( 3, pUser.serial );
	pStream.WriteShort( 7, iconID );
	pStream.WriteShort( 9, 1 );
	// Skip 4 .., so 11-14
	pStream.WriteShort( 15, iconID );
	pStream.WriteShort( 17, 1 );
	// Skip 4 .., so 19-22
	pStream.WriteShort( 23, seconds );
	// Skip 3 .., so 25-27
	pStream.WriteLong( 28, priCliloc );
	pStream.WriteLong( 32, scndCliloc );

	if( hasArgs )
	{
		// Skip 4 .., so 36-39
		pStream.WriteShort( 40, 1 );
		// Skip 2 .., so 42-43
		for( var k = 0; k < utf16Bytes.length; ++k )
		{
			pStream.WriteByte( 44 + k, utf16Bytes[k] );
		}
		pStream.WriteShort( 44 + utf16Bytes.length, 2 );
	}
	pSocket.Send( pStream );
	pStream.Free();
}

function RemoveBuff( pUser, iconID )
{
	var pSocket = pUser.socket;
	var pStream = new Packet;

	pStream.ReserveSize( 13 );
	pStream.WriteByte( 0, 0xDF );
	pStream.WriteShort( 1, 13 );
	pStream.WriteLong( 3, pUser.serial );
	pStream.WriteShort( 7, iconID );
	pStream.WriteShort( 9, 0 );
	pSocket.Send( pStream );
	pStream.Free();

	pUser.KillJSTimer( iconID, 2204 );
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	RemoveBuff( pUser, timerID );
}
