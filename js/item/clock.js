function onUse( pUser, iUsed )
{
	var pSock = pUser.socket;
	TriggerEvent( 1052, "TellTime", pSock );
	return false;
}

