function onUse( pUser, iUsed ) 
{ 
    DoSoundEffect( iUsed, 1, 0x0146, true );
    iUsed.id = 0x0A26;
    srcSock = CalcSockFromChar( pUser );
    SysMessage( srcSock, "You extinguish the candle." );
}
