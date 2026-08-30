/// <reference path="../../../definitions.d.ts" />
// @ts-check
/** @type { ( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageDealt: number ) => void } */
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
	if( ValidateObject( target ) && !target.dead && !target.isGM && !target.isCounselor && target.visible != 3 )
	{
		DoMovingEffect( pDefender, target, 0x36BD, 10, false, false, 0xA6 );
		target.Damage( RandomNumber( 20, 25 ), pDefender );

		// Validate target again in case they died from the applied damage
		if( ValidateObject( target ))
		{
			target.poison = 5;
		}

		return true;
	}

	return false;
}

