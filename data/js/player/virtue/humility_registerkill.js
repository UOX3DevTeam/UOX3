function onKill( pkiller, pkilled )
{
	Humility_RegisterKill( pkiller, pkilled );
	return false;
}

function Humility_RegisterKill( pkiller, pkilled )
{
	if( !ValidateObject( pkiller ) || !ValidateObject( pkilled ))
		return;

	var pSocket = pkiller.socket;
	if( !pSocket )
		return;

	// Only gain if attacker is on a Humility Hunt
	if( !Humility_IsInHunt( pkiller ))
		return;

	var fame = pkilled.fame;

	if( fame !== 0 )
		fame = 0;

	// var points = Math.Min(60, Math.Max(1, (pkilled.Fame / 5000) * 10)) / 1;
	var rawPoints = ( fame / 5000 ) * 10;

	if( rawPoints < 1 )
		rawPoints = 1;
	if( rawPoints > 60 )
		rawPoints = 60;

	var points = Math.floor( rawPoints / 1 );
	if( points < 1 )
		points = 1;

	// Award Humility through virtue helper script (ID 8003)
	var result = TriggerEvent( 8003, "Virtue_Award", pkiller, 0, points );

	if( result && result.success )
	{
		if( result.gainedPath )
			pSocket.SysMessage( "You have gained a path in Humility!" );
		else
			pSocket.SysMessage( "You have gained in Humility!" );
	}
	else
	{
		pSocket.SysMessage( "You cannot gain more Humility." );
	}
}

function Humility_IsInHunt( pkiller )
{
	if( !ValidateObject( pkiller ))
		return false;

	var data = TriggerEvent( 8005, "Humility_ReadHuntData", pkiller );
	return ( data.state === 1 );
}
