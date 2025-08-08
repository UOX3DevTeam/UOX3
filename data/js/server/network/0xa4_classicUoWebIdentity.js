// Handler for CUO WebIdentity Packet
//	https://github.com/ClassicUO/packets
//	https://github.com/ClassicUO/packets/blob/main/WebIdentity.ksy
//
// Note that this packet is sent before the regular login packet
// Last Updated: 28. June, 2023

// Shard secret defined by shard admin and shared with CUOWeb proxy
const secretShardKey = GetServerSetting( "SecretShardKey" );

function PacketRegistration()
{
	RegisterPacket( 0xa4, 0x0 ); // Overrides 0xA4 SystemInfo packet
}

function onPacketReceive( pSocket, packetNum, subCommand )
{
	var cmd = pSocket.GetByte( 0 );
	if( cmd != packetNum )
		return;

	var packetLen = 149;
	pSocket.ReadBytes( packetLen ); // Front-load packetLen bytes from socket buffer

	// Client type, for CUO Web this is always the string "CUOWEB".
	// However this is left open for other clients who might implement a similar packet.
	var offset = 1; // skip packet ID
	var clientType = pSocket.GetString( offset );

	// Unix timestamp when the identity packet was created, these packets should be considered short-lived.
	// Example: 0x6492F34E
	offset += clientType.length + 1;
	var timestamp = pSocket.GetDWord( offset );

	// Secret pre-shared key known only by ClassicUO's Game Proxy and the Shard
	offset += 4;
	var secret = pSocket.GetString( offset );

	// ClassicUO Web's unique userId
	// Example: 01H2QS904AM5BHSQRTZSN7JQCY
	offset += secret.length + 1;
	var userID = pSocket.GetString( offset );

	// The user's real connecting IP address as seen by the Game Proxy.
	offset += userID.length + 1;
	var connectionIP = pSocket.GetString( offset );

	// External authentication provider (if any), e.g. Discord.
	offset += connectionIP.length + 1;
	var extAuthProvider = pSocket.GetString( offset );

	// External authentication username (if any), e.g. blank#9244
	offset += extAuthProvider.length + 1;
	var extAuthUsername = pSocket.GetString( offset );

	// External authentication Id (if any), e.g. for discord 91796932124495872
	offset += extAuthUsername.length + 1;
	var extAuthID = pSocket.GetString( offset );

	// User Role as defined by ClassicUO Web, currently one of user, admin, shard-owner
	offset += extAuthID.length + 1;
	var role = pSocket.GetString( offset );

	//  Unused remaining packet section of the SystemInfo packet
	offset += role.length;
	var unused = pSocket.GetString( offset, packetLen - offset );

	// Only proceed to do something with all this if secret key is defined in UOX.INI
	if( secretShardKey != "None" )
	{
		if( secret != secretShardKey )
		{
			// Secret key sent by client does not match the key defined in UOX.INi; Disconnect!
			if( ValidateIpAddress( connectionIP ))
			{
				Console.Warning( "Connection from IP " + connectionIP.toString() + " with incorrect secret key denied access." );
			}
			else
			{
				Console.Warning( "Connection with incorrect secret key denied access." );
			}
			pSocket.Disconnect();
		}
		else
		{
			// Do something with the  retrieved information for connected user,
			// like authenticate that discord username exists on shard discord server
			// ...
		}
	}

	return;
}

function ValidateIpAddress( ipAddress )
{
  var ipRegex = /^(\d{1,3}\.){3}\d{1,3}$/;
  return ipRegex.test( ipAddress );
}
