function onUseChecked( pUser, iUsed )
{
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	var pSock = pUser.socket;
	TriggerEvent( 1052, "TellTime", pSock );
	return false;
}

