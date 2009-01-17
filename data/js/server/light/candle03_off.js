function onUseChecked( pUser, iUsed )
{
    pUser.SoundEffect( 0x0147, true );
    iUsed.id = 0x0A26;
    return false;
}
