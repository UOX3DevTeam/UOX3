function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.InRange( iUsed, 3 ) )
		{
			if( pUser.isonhorse )
				pUser.DoAction( 0x1D );
			else
				pUser.DoAction( 0x0D );

			pUser.SoundEffect( 0x013E, true );
			var newItem = CreateBlankItem( socket, pUser, 1, "#", 0x0DF9, 0, "ITEM", true );
			if( newItem && newItem.isItem )
			{
				newItem.decay = true;
				socket.SysMessage( GetDictionaryEntry( 476, socket.Language ) );
			}
		}
		else
			socket.SysMessage( "That is too far away." );
	}
	return false;
}
