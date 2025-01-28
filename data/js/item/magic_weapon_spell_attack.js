// This script is attached to characters when a magic weapon with a spell effect is equipped
const scriptID = 3305;

// If enabled, damage dealt and resist difficulty will be based on wielder's Magery skill
// If disabled, these are based on the average of minimum skill required to cast a given spell
// and the max skill cap for gaining additional skill from casting said spell, as defined in
// spells.dfn
const useAttackerMagerySkill = false;

function onAttack( mAttacker, mDefender, hitStatus, hitLoc, damageDealt )
{
	// Fetch weapon in main hand
	var iWeapon = mAttacker.FindItemLayer( 0x01 );
	if( !ValidateObject( iWeapon ))
	{
		iWeapon = mAttacker.FindItemLayer( 0x02 );
	}
	if( !ValidateObject( iWeapon ))
	{
		// There's no weapon equipped! Why is this script running?
		mAttacker.RemoveScriptTrigger( scriptID );
		return;
	}

	// Fetch which spell is enabled for this item
	var spellCircle = iWeapon.morex;
	var spellNumber = iWeapon.morey;
	var spellCharges = iWeapon.morez;

	// Check if the item has any charges left
	if( spellCharges == 0 )
	{
		if( mAttacker.socket != null )
		{
			mAttacker.socket.SysMessage( GetDictionaryEntry( 9401, mAttacker.socket.language )); // This item is out of charges.
		}
		return;
	}

	var mSpell = Spells[spellNumber];
	var spellDamage = mSpell.baseDmg;

	// If target has magic reflection enabled, reflect all effects and damage back to caster!
	if( mDefender.magicReflect )
	{
		mDefender.magicReflect = false;
		mDefender = mAttacker;
	}

	// Play effects associated with spell
	if( spellNumber != 5 && spellNumber != 12 && spellNumber != 18 && spellNumber != 30 )
	{
		mDefender.SoundEffect( mSpell.soundEffect, true );
		mDefender.SpellStaticEffect( mSpell );
	}

	iWeapon.morez--; // Reduce spell charges on item by 1
	iWeapon.Refresh();

	// Apply effect to character based on the type of spell
	switch( spellNumber )
	{
		case 1: // Clumsiness
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 25; // Duration of effect - if cast by someone with Magery skill of 100.0 the duration would be 50 seconds
			var resistDifficulty = 206; // Half-ways point between minimum and "max" skill required to cast spell
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			DoTempEffect( 0, mAttacker, mDefender, 3, effectStrength, effectDuration, resistDifficulty );
			break;
		case 3: // Feeblemindedness
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 25; // Duration of effect - if cast by someone with Magery skill of 100.0 the duration would be 50 seconds
			var resistDifficulty = 206; // Half-ways point between minimum and "max" skill required to cast spell
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			DoTempEffect( 0, mAttacker, mDefender, 4, effectStrength, effectDuration, resistDifficulty );
			break;
		case 5: // Burning (Magic Arrow)
			var resistDifficulty = 206; // Half-ways point between minimum and "max" skill required to cast spell

			// Should we use the magery skill of weapon wielder when calculating resist?
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			var spellResisted = CheckTargetResist( resistDifficulty, mDefender, spellCircle );

			// Should we use magery skill of weapon wielder when calculating final spell damage?
			if( useAttackerMagerySkill )
			{
				spellDamage = CalcSpellDamageMod( mAttacker, mDefender, spellDamage, spellResisted );
			}
			else
			{
				spellDamage = RandomNumber( spellDamage/2, spellDamage );
				if( spellResisted )
				{
					spellDamage /= 2;
				}
			}

			DoTempEffect( 0, mAttacker, mDefender, 28, spellDamage, 0, resistDifficulty );
			break;
		case 8: // Weakness
			var effectStrength = 5;
			var effectDuration = 50;
			var resistDifficulty = 206; // Half-ways point between minimum and "max" skill required to cast spell
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			DoTempEffect( 0, mAttacker, mDefender, 5, effectStrength, effectDuration, resistDifficulty );
			break;0
		case 12: // Wounding (Harm)
			var resistDifficulty = 281; // Half-ways point between minimum and "max" skill required to cast spell

			// Should we use the magery skill of weapon wielder when calculating resist?
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			var spellResisted = CheckTargetResist( resistDifficulty, mDefender, spellCircle );

			// Should we use magery skill of weapon wielder when calculating final spell damage?
			if( useAttackerMagerySkill )
			{
				spellDamage = CalcSpellDamageMod( mAttacker, mDefender, spellDamage, spellResisted );
			}
			else
			{
				spellDamage = RandomNumber( spellDamage/2, spellDamage );
				if( spellResisted )
				{
					spellDamage /= 2;
				}
			}

			DoTempEffect( 0, mAttacker, mDefender, 29, spellDamage, 0, resistDifficulty );
			break;
		case 18: // Demon's Breath (Fireball)
			var resistDifficulty = 381; // Half-ways point between minimum and "max" skill required to cast spell

			// Should we use the magery skill of weapon wielder when calculating resist?
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			var spellResisted = CheckTargetResist( resistDifficulty, mDefender, spellCircle );

			// Should we use magery skill of weapon wielder when calculating final spell damage?
			if( useAttackerMagerySkill )
			{
				spellDamage = CalcSpellDamageMod( mAttacker, mDefender, spellDamage, spellResisted );
			}
			else
			{
				spellDamage = RandomNumber( spellDamage/2, spellDamage );
				if( spellResisted )
				{
					spellDamage /= 2;
				}
			}

			DoTempEffect( 0, mAttacker, mDefender, 30, spellDamage, 0, resistDifficulty );
			break;
		case 27: // Evil (Curse)
			var effectStrength = 5;
			var effectDuration = 50;
			var resistDifficulty = 481; // Half-ways point between minimum and "max" skill required to cast spell
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			DoTempEffect( 0, mAttacker, mDefender, 12, effectStrength, effectDuration, resistDifficulty );
			break;
		case 30: // Thunder (Lightning)
			var resistDifficulty = 481; // Half-ways point between minimum and "max" skill required to cast spell

			// Should we use the magery skill of weapon wielder when calculating resist?
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			var spellResisted = CheckTargetResist( resistDifficulty, mDefender, spellCircle );

			// Should we use magery skill of weapon wielder when calculating final spell damage?
			if( useAttackerMagerySkill )
			{
				spellDamage = CalcSpellDamageMod( mAttacker, mDefender, spellDamage, spellResisted );
			}
			else
			{
				spellDamage = RandomNumber( spellDamage/2, spellDamage );
				if( spellResisted )
				{
					spellDamage /= 2;
				}
			}

			DoTempEffect( 0, mAttacker, mDefender, 31, spellDamage, 0, 0 );
			break;
		case 31: // Mage's Bane (Mana Drain)
			var resistDifficulty = 481; // Half-ways point between minimum and "max" skill required to cast spell
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			var spellResisted = CheckTargetResist( resistDifficulty, mDefender, spellCircle );
			if( !spellResisted )
			{
				var manaToDrain = Math.round( 600 / 35 );
				if( mDefender.mana < manaToDrain )
				{
					manaToDrain = mDefender.mana;
				}

				mDefender.mana -= manaToDrain;
			}
			break;
		case 38: // Ghoul's Touch (Paralyze)
			var effectDuration = 10;
			var resistDifficulty = 581; // Half-ways point between minimum and "max" skill required to cast spell
			if( useAttackerMagerySkill )
			{
				resistDifficulty = mAttacker.skills.magery;
			}
			var spellResisted = CheckTargetResist( resistDifficulty, mDefender, spellCircle );
			if( !spellResisted )
			{
				DoTempEffect( 0, mAttacker, mDefender, 1, 0, effectDuration, 0 );
			}
			break;
	}
}

function CheckTargetResist( resistDifficulty, mDefender, spellCircle )
{
	if( ValidateObject( mDefender ) )
	{
		var resistResult = mDefender.CheckSkill( 26, 80 * spellCircle, 800 + ( 80 * spellCircle ));
		var defaultChance = mDefender.skills.magicresistance / 5;
		var resistChance = mDefender.skills.magicresistance - ((( resistDifficulty - 20 ) / 5 ) + ( spellCircle * 5 ));
		if( defaultChance > resistChance )
		{
			resistChance = defaultChance;
		}
		if( RandomNumber( 1, 100 ) < resistChance / 10 )
		{
			var tSock = mDefender.socket;
			if( tSock != null )
			{
				tSock.SysMessage( GetDictionaryEntry( 699, tSock.language ) );
			}
			resistResult = true;
		}
		else
		{
			resistResult = false;
		}
	}
	return resistResult;
}

function CalcSpellDamageMod( mAttacker, mDefender, spellDamage, spellResisted )
{
	// Randomize in upper-half of damage range for some variety
	spellDamage = RandomNumber( spellDamage/2, spellDamage );

	// If spell was resisted, halve damage
	if( spellResisted )
	{
		spellDamage = spellDamage / 2;
	}

	// Add damage bonus/penalty based on attacker's EVALINT vs defender's MAGICRESISTANCE
	var casterEval = mAttacker.skills.evaluatingintel / 10;
	var targetResist = mDefender.skills.magicresistance / 10;
	if( targetResist > casterEval )
	{
		spellDamage *= ((( casterEval - targetResist ) / 200.0 ) + 1 );
	}
	else
	{
		spellDamage *= ((( casterEval - targetResist ) / 500.0 ) + 1 );
	}

	return Math.round( spellDamage );
}