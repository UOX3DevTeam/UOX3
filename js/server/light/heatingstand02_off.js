// simple lightsource switching script
// 17/06/2001 Yeshe; yeshe@manofmystery.org

function onUse( pUser, iUsed )
{
    pUser.SoundEffect( 0x0147, true );
    iUsed.id = 0x184D;
    return false;
}
