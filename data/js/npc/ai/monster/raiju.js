/// <reference path="../../../definitions.d.ts" />
// @ts-check
/** @type { ( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageDealt: number ) => void } */
function onAttack( pAttacker, pDefender ) 
{
	if( RandomNumber( 1, 100 ) <= 10 )
	{
		pDefender.StaticEffect( 0x37B9, 10, 5 );
		if( pDefender.socket != null ) 
		{
			pDefender.SysMessage( "The creature attacks with stunning force!" );
		}

		pDefender.frozen = true;
		pDefender.StartTimer(0, 4000, true);
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pDefender, timerID ) 
{
	if( timerID == 0)
	{
		pDefender.frozen = false;
	}
}
