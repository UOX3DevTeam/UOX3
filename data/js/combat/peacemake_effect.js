function onWarModeToggle( pChar )
{
	if( pChar.GetTimer( Timer.PEACETIMER ) > GetCurrentClock() )
	{
		pChar.SysMessage( "You are currently under the effect of peace and can not attack!" );
		return false;
	}

	// If peacetimer has run out, remove script from player
	pChar.RemoveScriptTrigger( 7000 );
	return true;
}