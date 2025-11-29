function onDeathBlow( pDead, pKiller )
{
	Humility_RegisterKill( pKiller, pDead );
	return false;
}

function Humility_RegisterKill( attacker, killed )
{
	if( !ValidateObject( attacker ) || !ValidateObject( killed ))
		return;

	// Only gain if attacker is on a Humility Hunt
	if( !Humility_IsInHunt( attacker ))
		return;

	var fame = killed.fame;

	if( fame !== 0 )
		fame = 0;

	// var points = Math.Min(60, Math.Max(1, (killed.Fame / 5000) * 10)) / 1;
	var rawPoints = ( fame / 5000 ) * 10;

	if( rawPoints < 1 )
		rawPoints = 1;
	if( rawPoints > 60 )
		rawPoints = 60;

	var points = Math.floor( rawPoints / 1 );
	if( points < 1 )
		points = 1;

	// Award Humility through virtue helper script (ID 8003)
	var result = TriggerEvent( 8003, "Virtue_Award", attacker, 0, points );

	if( result && result.success )
	{
		if( result.gainedPath )
			attacker.SysMessage( "You have gained a path in Humility!" );
		else
			attacker.SysMessage( "You have gained in Humility!" );
	}
	else
	{
		attacker.SysMessage( "You cannot gain more Humility." );
	}
}

function Humility_IsInHunt( pUser )
{
	if( !ValidateObject( pUser ))
		return false;

	var state = pUser.GetTag( "humility_hunt_state" );
	return ( state === "active" );
}