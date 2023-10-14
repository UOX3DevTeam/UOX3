function onWarModeToggle( pChar )
{
	if( pChar.GetTimer( Timer.PEACETIMER ) > GetCurrentClock() )
	{
		var pSock = pChar.socket;
		if( pSock )
		{
			pChar.SysMessage( GetDictionaryEntry( 2788, pSock.language )); // You are currently under the effect of peace and can not attack!
		}
		return false;
	}

	// If peacetimer has run out, remove script from player
	pChar.RemoveScriptTrigger( 7000 );
	return true;
}