function ClearSpecialMove( pUser, abilityID )
{
	var socket = pUser.socket;
	var toSend = new Packet;

	toSend.ReserveSize( 7 )
	toSend.WriteByte( 0, 0xbf ); // Packet
	toSend.WriteShort( 1, 0x07 ); // Length
	toSend.WriteShort( 3, 0x21 ); // SubCmd
	toSend.WriteByte( 5, abilityID ); // Ability ID
	toSend.WriteByte( 6, 0 ); // On/off
	socket.Send( toSend );
	toSend.Free();

	pUser.SetTempTag( "abilityID", null );
	pUser.SetTempTag( "doubleMana", null );
}

function DeactivateSpecialMove( pUser, abilityID ) 
{
	var socket = pUser.socket;
	var toSend = new Packet;

	toSend.ReserveSize( 7 )
	toSend.WriteByte( 0, 0xbf ); // Packet
	toSend.WriteShort( 1, 0x07 ); // Length
	toSend.WriteShort( 3, 0x21 ); // SubCmd
	toSend.WriteByte( 5, abilityID ); // Ability ID
	toSend.WriteByte( 6, 0); // On/off
	socket.Send( toSend );
	toSend.Free();

	pUser.SetTempTag( "abilityID", null );
}
