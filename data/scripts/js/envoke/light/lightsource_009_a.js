function onUse( pUser, iUsed ) 
{ 
    DoSoundEffect( iUsed, 1, 0x0047, true );
    iUsed.id = 0x0B1A;
    srcSock = CalcSockFromChar( pUser );
    SysMessage( srcSock, "You light the candle." );
}
