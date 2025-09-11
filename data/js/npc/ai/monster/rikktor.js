function onAttack( pAttacker, pDefender )
{
	if( !ValidateObject(pAttacker))
		return;

	// 20% chance per attack round
	if( RandomNumber(1, 100) <= 20 )
	{
		pAttacker.SoundEffect( 0x2F4, true );
		AreaCharacterFunction( "EarthQuake", pAttacker, 8, null );
	}
}

function EarthQuake( rikktor, target, sock )
{
	if( !ValidateObject( target ) || target == rikktor || !target.vulnerable )
		return false;

	// 60% of current HP, clamped between 10 and 75
	let dmg = Math.round( target.health * 0.6 );
	dmg = Math.max( 10, Math.min( 75, dmg ));

	target.Damage( dmg, 1, rikktor, true ); // 1 = physical

	// Play recoil animation if humanoid and unmounted
	if(( target.BodyID == 0x0190 || target.BodyID == 0x0191) && !target.isonhorse )
		target.DoAction(20); // Recoil anim

	return true;
}

function _restorecontext_() {}
