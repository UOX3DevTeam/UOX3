function onCombatStart( pAttacker, pDefender )
{
	var socket = pAttacker.socket;
	if( pAttacker.worldnumber == 1 || pAttacker.worldnumber == 2 || pAttacker.worldnumber == 3 && pDefender.online )
	{
		socket.SysMessage( "You cannot perform beneficial acts on your target." );
		return false;
	}
	return true;
}

function onSpellTarget( myTarget, myTargetType, pCaster, spellID )
{
	var socket = pCaster.socket;
	if( socket != null )
	{
		if( pCaster.worldnumber == 1 || pCaster.worldnumber == 2 || pCaster.worldnumber == 3 && myTarget.online )
		{
			socket.SysMessage( "You cannot perform beneficial acts on your target." );
			return 2;
		}
	}
	return false;
}

function onSteal( pThief, iStolen, pVictim )
{
	if( pThief.worldnumber == 1 || pThief.worldnumber == 2 || pThief.worldnumber == 3 && pVictim.online )
	{
		socket.SysMessage( "You cannot perform beneficial acts on your target." );
		return true;
	}
	return false;
}