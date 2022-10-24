// Handler for Server Poll requests from
// UOGateway (https://www.uogateway.com)
// ShardPortal (https://www.shardportal.com)
// Last Updated: 21. June, 2021

function PacketRegistration()
{
	RegisterPacket( 0x7F, 0x0 );
}

function onPacketReceive( pSocket, packetNum, subCommand )
{
	var cmd = pSocket.GetByte( 0 );
	if( cmd != packetNum )
		return;

	pSocket.ReadBytes( 8 );
	var len = pSocket.GetWord( 1 );
	var subCmd = pSocket.GetByte( 3 );
	var unknown1 = pSocket.GetByte( 4 );
	var unknown2 = pSocket.GetByte( 5 );
	var unknown3 = pSocket.GetByte( 6 );
	var unknown4 = pSocket.GetByte( 7 );

	if( !GetServerSetting( "UOGENABLED" ))
	{
		Console.Print( "UOG Server Poll Packet detected; response disabled via UOGENABLED ini-setting.\n" );
	}
	else
	{
		Console.Print( "UOG Server Poll Packet detected, responding..." );
		SendUOGServerPollInfo( pSocket );
	}

	return;
}

function SendUOGServerPollInfo( pSocket )
{
	var shardName = GetServerSetting( "SERVERNAME" );
	var uptime = Math.floor( GetCurrentClock() / 1000 ) - Math.floor( GetStartTime() / 1000 );
	var totalOnline = GetPlayerCount();
	var totalItems = 0;
	var totalChars = 0;
	var totalMemory = 0;
	var shardInfo = "UOX3, " + shardName + ", Age=" + Math.round( uptime / 60 / 60 ) + ", Clients=" + totalOnline + ", Items=" + totalItems + ", Chars=" + totalChars + ", Mem=" + totalMemory + "K";
	var toSend = new Packet;
	toSend.ReserveSize( shardInfo.length + 1 );
	toSend.WriteString( 0, shardInfo, shardInfo.length ); // shardInfo string
	toSend.WriteByte( shardInfo.length + 1, 0x00 ); // null terminated
	pSocket.Send( toSend );
	toSend.Free();
	Console.Print( "Done!\n" );
	Console.Log( "Response sent to UOGateway Server Poll Packet: U" + shardInfo );
}