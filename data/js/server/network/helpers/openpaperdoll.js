//usage: TriggerEvent( 18000, "OpenPaperdoll", socket, targChar )

function OpenPaperdoll( socket, targChar )
{
    // Create packet to open a paperdoll
    var pStream = new Packet;
    pStream.ReserveSize( 66 );
    pStream.WriteByte( 0, 0x88 );
    pStream.WriteLong( 1, targChar.serial );
    pStream.WriteString( 5, targChar.name + ( targChar.title ? " " + targChar.title : "" ), targChar.name.length + ( targChar.title ? targChar.title.length + 1 : 0 ));
    pStream.WriteByte( 65, 0x2 );
    socket.Send( pStream );
    pStream.Free();
}