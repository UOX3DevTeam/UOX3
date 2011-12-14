// simple lightsource switching script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 20/06/2011 Xuri - Added support for loading dir value from morex

function onUseChecked( pUser, iUsed )
{
    pUser.SoundEffect( 0x0047, true );
    iUsed.id = 0x184A;
    
	// If lightsource is turned off AND has a value saved in morex, apply value to dir
	// else if no dir value can be found, use specified value
	// else, do nothing, as the item already has a dir value
	if(( iUsed.dir == 0 || iUsed.dir == 99 ) && iUsed.morex != 0 )
	{
		iUsed.dir = iUsed.morex;
		iUsed.morex = 0;
	}
	else if( iUsed.dir == 0 && iUsed.morex == 0 )
		iUsed.dir = 1;

    return false;
}
