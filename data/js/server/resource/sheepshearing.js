function shearSheep( pUser, sheep )
{
	if( !sheep || !pUser )
		return;
	var pSock = pUser.socket;

	// If sheep is already shorn, do nothing
	if( sheep.id == 0x00df )
		pSock.SysMessage( GetDictionaryEntry( 1774, pSock.language ) ); // This sheep is not yet ready to be shorn.
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 1773, pSock.language ) ); // You shear some wool from the sheep
		sheep.SoundEffect( 0x0248, true );

		// remove sheep's wool and add to player's backpack
		sheep.id = 0x00DF;
		CreateDFNItem( pUser.socket, pUser, "0x0df8", 2, "ITEM", true ); //give the player some wool

		// Determine the respawn time (in seconds) for the sheep's wool
		var delay = RandomNumber( 60, 180 );

		// Trigger a hardcoded timer in UOX3 which changes ID of the sheep back to normal
		DoTempEffect( 0, sheep, pUser, 43, delay, 0, 0 );
	}
}