/**
 * Auto-generated for UOX3
 * Version: 1.0
 * Last Modified on: 03/16/2021 @0402 PST
 **/
const cuoProtocolVersion = 0;
const serverType = 2;

function PacketRegistration()
{
	RegisterPacket( 0xF1, 0x0 );
}

function onPacketReceive( pSocket, packetNum, subCommand )
{
	var cmd = pSocket.GetByte( 0 );
	if( cmd != packetNum )
		return;

	pSocket.ReadBytes( 5 );
	var len = pSocket.GetWord( 1 );
	var subCmd = pSocket.GetByte( 3 ); // Fetch subCmd
	var protVer = pSocket.GetByte( 4 ); // Fetch protocol version

	switch( subCmd )
	{
		case 0xC0: // ConnectUO Server Poll Packet request
			if( !GetServerSetting( "CUOENABLED" ))
			{
				Console.Print( "CUO Server Poll Packet detected; response disabled via CUOENABLED ini-setting.\n" );
			}
			else if( protVer == cuoProtocolVersion )
			{
				Console.Print( "CUO Server Poll Packet detected, responding..." );
				SendServerPollInfo( pSocket );
			}
			else
				Console.Warning( "Protocol version mismatch in CUO Server Poll Packet (" + protVer + ") vs script (" + cuoProtocolVersion + ")!" );
			break;
		default:
			Console.Warning( "Unhandled subcommand (" + subCmd + ") in packet 0xF1." );
	}
	return;
}

function SendServerPollInfo( pSocket )
{
	var uptime = Math.floor(GetCurrentClock() / 1000);
	var totalAccounts = GetAccountCount();
	var totalOnline = GetPlayerCount();
	var toSend = new Packet;
	toSend.ReserveSize( 17 );
	toSend.WriteByte( 0, 0xC0 ); // PacketID
	toSend.WriteShort( 1, 17 ); // Length
	toSend.WriteByte( 3, cuoProtocolVersion );
	toSend.WriteByte( 4, serverType );
	toSend.WriteLong( 5, uptime ); // Server uptime
	toSend.WriteLong( 9, totalAccounts ); // Total accounts
	toSend.WriteLong( 13, totalOnline ); // Total players online
	pSocket.Send( toSend );
	toSend.Free();
	Console.Print( "Done!\n" );
	Console.Log( "Response sent to CUO Server Poll Packet (totalAccounts: " + totalAccounts + ", totalOnline: " + totalOnline + ", upTimeInSeconds: " + uptime );
}
