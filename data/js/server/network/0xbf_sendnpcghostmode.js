// Sends a packet to toggle ghost mode appearance for an NPC on the client.
// Typically used to make dead pets or NPCs appear as transparent ghosts without affecting game logic.
function SendNpcGhostMode( pSock, unknownFlag, serial, ghostFlag )
{
    var pkt = new Packet();				// Create a new packet
    pkt.ReserveSize( 11 );				// Reserve 11 bytes of space

    pkt.WriteByte( 0, 0xBF );			// Packet ID: Extended command
    pkt.WriteShort( 1, 11 );			// Total packet length
    pkt.WriteShort( 3, 0x19 );			// Subcommand: 0x19 (SendNpcGhostMode)
    pkt.WriteByte( 5, unknownFlag );	// Unknown/extra flag (often 0)
    pkt.WriteLong( 6, serial );			// Target serial
    pkt.WriteByte( 10, ghostFlag );		// Ghost flag (1 = Ghost, 0 = not Ghost)

    pSock.Send( pkt );					// Send packet to client
    pkt.Free();							// Free memory
}