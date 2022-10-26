function onCharDoubleClick( pUser, targChar )
{
	switch( targChar.id )
	{
		case 0x007a: // Unicorn
			if( pUser.gender != 1 && pUser.gender != 3 && pUser.gender != 5 ) // Only females can ride Unicorns
			{
				pUser.socket.SysMessage( GetDictionaryEntry( 2436, pUser.socket.language )); // The Unicorn refuses to allow you to ride it.
				return false;
			}
			break;
		case 0x0084: // Ki-rin
			if( pUser.gender != 0 && pUser.gender != 2 && pUser.gender != 4 ) // Only males can ride Ki-rins
			{
				pUser.socket.SysMessage( GetDictionaryEntry( 2437, pUser.socket.language )); // The Ki-Rin refuses your attempts to mount it.
				return false;
			}
			break;
		case 0x0115: // Cu Sidhe
			if( pUser.gender != 2 && pUser.gender != 3 ) // Only Elves can ride Cu Sidhes
			{
				pUser.socket.SysMessage( GetDictionaryEntry( 2435, pUser.socket.language )); // Only Elves may use this.
				return false;
			}
			break;
		default:
			break;
	}

	return true;
}