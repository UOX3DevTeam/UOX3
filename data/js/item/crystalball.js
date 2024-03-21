function onUseChecked( pUser, iUsed )
{
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	var fortunetelling = RandomNumber( 19005, 19034 );//Dictonary msgs

	if( iUsed.container != null )
	{
		pUser.EmoteMessage(  GetDictionaryEntry( fortunetelling ), true, 0x3B2 );
	}
	else
	{
		iUsed.TextMessage(  GetDictionaryEntry( fortunetelling ), true, 0x3B2 );
	}
	return false;
}
