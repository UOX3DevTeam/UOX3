// simple lightsource switching script
// 17/06/2001 Yeshe; yeshe@manofmystery.org

function onUse( pUser, iUsed ) 
{ 
    pUser.SoundEffect( 0x0048, true );
    iUsed.id = 0x0B20;
    return false;
}
