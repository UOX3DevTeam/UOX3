// simple lightsource switching script
// 17/06/2001 Yeshe; yeshe@manofmystery.org

function onUse( pUser, iUsed ) 
{ 
    DoSoundEffect( iUsed, 1, 0x0047, true );
    iUsed.id = 0x0A0F;
    srcSock = CalcSockFromChar( pUser );
    SysMessage( srcSock, "You light the candle." );
}
