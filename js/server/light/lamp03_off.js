// simple lightsource switching script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 20/06/2011 Xuri - Added support for saving dir value

function onUseChecked( pUser, iUsed )
{
	pUser.SoundEffect( 0x0147, true );
    iUsed.id = 0x0B25;

	// Turn off light: save current dir to morex, set dir to 99 (some still have light at 0)
	iUsed.morex = iUsed.dir;
	iUsed.dir = 99;

    return false;
}
