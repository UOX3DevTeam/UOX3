function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock && iUsed && iUsed.isItem )
	{
		var pOwner = GetPackOwner( iUsed, 0 );
		if( pOwner && pOwner.serial == pUser.serial )
		{
			if( !pUser.CheckSkill( 37, 0, 1000 ) )
				pSock.SysMessage( GetDictionaryEntry( 481, pSock.language ) );
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 480, pSock.language ) );
				CreateBlankItem( pSock, pUser, 1, "#", 0x1057, 0, "ITEM", true );
				iUsed.Delete();
			}
		}
		else
			pSock.SysMessage( GetDictionaryEntry( 775, pSock.language ) );
	}
	return false;
}


