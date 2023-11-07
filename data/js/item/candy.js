function onUseChecked( pUser, iUsed ) 
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( itemOwner == null || itemOwner != pUser )
	{
		socket.SysMessage(GetDictionaryEntry(1763, pUser.socket.language)); // That item must be in your backpack before it can be used.
	}
	else
	{
		var soundNumbers = [0x03A, 0x03B, 0x03C];
		var randomIndex = Math.floor( Math.random() * soundNumbers.length );
		var randomSoundNumber = soundNumbers[randomIndex];

		socket.SysMessage( "You feel as if you could eat as much as you wanted!" );
		pUser.SoundEffect( randomSoundNumber, false );
	}
}