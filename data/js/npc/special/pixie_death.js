// Random spells cast by pixies upon death
function onDeathBlow( pKilled, pKiller )
{
	if( CastRandomSpell( pKilled ))
		return true;
	else
		return false;
}

function CastRandomSpell( pKilled )
{
	// Bless, Curse, Explosion or Greater Poison on the attacker, or it can be Greater Heal cast on the Pixie itself.
	switch( RandomNumber( 4, 4 ))
	{
		case 1: // Bless - casts on current target
			pKilled.CastSpell( 17, true );
			break;
		case 2: // Curse - casts on current target
			pKilled.CastSpell( 27, true );
			break;
		case 3: // Explosion - casts on current target
			pKilled.CastSpell( 43, true );
			break;
		case 4: // Greater Poison - casts on current target
			pKilled.CastSpell( 20, true );
			var currTarget = pKilled.target;
			if( ValidateObject( currTarget ))
				currTarget.poison = 3; // Set poison level to equal that of greater poison
			break;
		case 5: // Greater Heal - casts on self, preventing death
			// Store reference to current target
			var currTarget = pKilled.target;

			// Target self, cast heal
			pKilled.target = pKilled;
			pKilled.CastSpell( 29, true );

			// Restore previous target
			if( ValidateObject( currTarget ))
				pKilled.target = currTarget;
			return false;
		default:
			break;
	}
	return true;
}