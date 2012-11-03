function onUseChecked( pUser, iUsed )
{
    pUser.SoundEffect( 0x0147, true );
    iUsed.id = 0x0A26;

	// Turn off light: save current dir to morex, set dir to 99 (some still have light at 0)
	iUsed.morex = iUsed.dir;
	iUsed.dir = 99;

    return false;
}
