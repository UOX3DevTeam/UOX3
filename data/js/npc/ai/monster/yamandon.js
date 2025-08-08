function onAttack( pAttacker, pDefender ) 
{
	if( RandomNumber( 1, 100 ) <= 20 )
	{
		pDefender.DoAction( 0x0B );

		AreaCharacterFunction( "ApplyEffects", pDefender, 8, null );
	}
}

function ApplyEffects(pDefender, target, pSock)
{
	if( !target.dead )
	{
		target.Damage( RandomNumber( 20, 25 ), pDefender );

		DoMovingEffect( pDefender, target, 0x36BD, 10, false, false, 0xA6 );

		target.poison = 5;

		return true;
	}

	return false;
}

