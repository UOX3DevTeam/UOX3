function onUse( pUser, iUsed ) 
{ 
    pUser.SoundEffect( 0x0048, true );
    iUsed.id = 0x0B1A;
    return false;
}
