// Hits all nearby characters when they attack with their axe

function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	pAttacker.SetTempTag( "pDefenderSerial", ( pDefender.serial ).toString() );
	AreaCharacterFunction( "HitNearbyTargets", pAttacker, 1 );
}

function HitNearbyTargets( pAttacker, targChar )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( targChar ) || pAttacker == targChar )
		return false;

	if( !targChar.vulnerable || targChar.aitype == 17 || targChar.visible > 2 )
		return false;

	var pDefender = CalcCharFromSer( pAttacker.GetTempTag( "pDefenderSerial" ));
	pAttacker.SetTempTag( "pDefenderSerial", null );
	if( ValidateObject( pDefender ) && pDefender == targChar )
		return false;

	if( !targChar.dead && ( targChar.npc || targChar.socket != null ))
	{
		//DoStaticEffect( targChar.x, targChar.y, targChar.z, 0x372a, 0x9, 0x0a, false );
		targChar.SoundEffect( 0x0201, true );

		var damage = pAttacker.attack;
		targChar.Damage( damage, 1, pAttacker, false );
	}
}

function _restorecontext_() {}
