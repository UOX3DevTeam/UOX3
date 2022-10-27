// Handler for Freeshard Server Poll requests from
// CUO Web (https://play.classicuo.org)
// Last Updated: 5. September, 2022

function PacketRegistration()
{
	RegisterPacket( 0xF1, 0x0 );
}

function onPacketReceive( pSocket, packetNum, subCommand )
{
	var cmd = pSocket.GetByte( 0 );
	if( cmd != packetNum )
		return;

	pSocket.ReadBytes( 4 );
	var len = pSocket.GetWord( 1 );
	var subCmd = pSocket.GetByte( 3 ); // Fetch subCmd

	switch( subCmd )
	{
		case 0xFE: // Freeshard Server Poll Packet request
			if( !GetServerSetting( "FREESHARDSERVERPOLL" ))
			{
				Console.Print( "Freeshard Server Poll Packet detected; response disabled via FREESHARDPOLL ini-setting.\n" );
			}
			else
			{
				Console.Print( "Freeshard Server Poll Packet detected, responding..." );
				SendServerPollInfo( pSocket );
			}
			break;
		default:
			Console.Warning( "Unhandled subcommand (" + subCmd + ") in packet 0xF1." );
	}
	return;
}

function SendServerPollInfo( pSocket )
{
	var uptime = Math.floor( GetCurrentClock() / 1000 ) - Math.floor( GetStartTime() / 1000 );
	var totalAccounts = GetAccountCount();
	var totalOnline = GetPlayerCount();
	var totalItems = 0;
	var totalChars = 0;
	var memoryHigh = 0;
	var memoryLow = 0;
	var toSend = new Packet;
	toSend.ReserveSize( 27 );
	toSend.WriteByte( 0, 0x51 ); // PacketID
	toSend.WriteShort( 1, 27 ); // Length
	toSend.WriteLong( 3, totalOnline ); // Total players online
	toSend.WriteLong( 7, totalItems ); // Total items on shard - no need to send
	toSend.WriteLong( 11, totalChars ); // Total chars on shard - no need to send
	toSend.WriteLong( 15, uptime ); // Server Uptime
	toSend.WriteLong( 19, memoryHigh ); // Max memory usage - no need to send
	toSend.WriteLong( 23, memoryLow ); // Min memory usage - no need to send
	pSocket.Send( toSend );
	toSend.Free();
	Console.Print( "Done!\n" );
	Console.Log( "Response sent to Freeshard Server Poll Packet (totalOnline: " + totalOnline + ", upTimeInSeconds: " + uptime );
}
