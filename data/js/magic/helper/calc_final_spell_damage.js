// Helper function to calculate final spell damage based on a specified base damage, whether
// spell was resisted, and whether caster's eval int is higher than target's magic resistance
function CalcFinalSpellDamage( caster, target, baseDmg, spellResisted )
{
	if( spellResisted )
	{
		// Spell resisted - halve the damage
		baseDmg = Math.round( baseDmg / 2 );
	}

	var casterEval = caster.skills.evaluatingintel;
	var targetResist = target.skills.magicresistance;

	// Add damage bonus/penalty based on caster's eval int vs target's resist
	if( targetResist > casterEval )
	{
		// Target's resistance is higher - apply penalty to spell damage
		baseDmg *= (((( casterEval / 10 ) - ( targetResist / 10 )) / 200 ) + 1 );
	}
	else
	{
		// Caster's eval int is higher - apply bonus to spell damage
		baseDmg *= ((( casterEval - targetResist ) / 500 ) + 1 );
	}

	return Math.round( baseDmg );
}