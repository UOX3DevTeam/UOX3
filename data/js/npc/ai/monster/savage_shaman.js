const savageShamanScriptID = 3213; // scriptID of this script from jse_fileassociations.scp

// Chance of triggering Tribal Dance for Savage Shamans when hit in combat
function onDefense( pAttacker, savageShaman )
{
	if( RandomNumber( 1, 5 ) == 5 ) // 20% chance on hit to start a savage dance, if they're not already casting
	{
		// Begin Savage Dance
		if( !savageShaman.isCasting )
		{
			BeginSavageDance( savageShaman, pAttacker );
		}
	}

	if( pAttacker.InRange( savageShaman, 1 ) && RandomNumber( 1, 100 ) >= 80 && !savageShaman.isCasting && !savageShaman.tempRetreating )
	{
		savageShaman.tempRetreating = true;
		savageShaman.origFleeAt = savageShaman.fleeAt;
		savageShaman.origReAttackAt = savageShaman.reAttackAt;
		savageShaman.fleeAt = 100;
		savageShaman.reAttackAt = 100;

		// Start timer to stop shaman from fleeing
		savageShaman.StartTimer( 3000, 2, true );
	}
}

function BeginSavageDance( savageShaman, pAttacker )
{
	// Let's pause the NPC in his tracks for X seconds so he doesn't wander off
	savageShaman.SetTimer( Timer.MOVETIME, 1000 );

	// Start dancing
	savageShaman.DoAction( 0x6F );

	// Look for other Savage Shamans to dance with
	var dancePartnerCount = AreaCharacterFunction( "SeekDancePartners", savageShaman, 8 );

	// Start a timer to end the savage dance and cast a spell if there are 3 or more other shamans in range
	// (including this shaman)
	if( dancePartnerCount >= 2 )
	{
		savageShaman.StartTimer( 1000, 1, true );
	}
}

function SeekDancePartners( savageShaman, dancePartner )
{
	var myTarget = savageShaman.target;
	if( !ValidateObject( myTarget ) || ( !myTarget.npc && ( myTarget.dead || !myTarget.online )))
		return false;

	// Only NPCs can be dance partners
	if( ValidateObject( dancePartner ) && dancePartner.isChar && dancePartner.npc )
	{
		// Only other shamans can be dance partners - other savages don't count
		if( dancePartner.race && dancePartner.race.id == savageShaman.race.id && dancePartner.HasScriptTrigger( savageShamanScriptID ))
		{
			if( dancePartner.CanSee( savageShaman ))
			{
				// Let's pause the NPC in his tracks for X seconds so he doesn't wander off
				dancePartner.SetTimer( Timer.MOVETIME, 1000 );

				// Start dancing
				dancePartner.DoAction( 0x6F );

				return true;
			}
		}
	}
	return false;
}

function onTimer( savageShaman, timerID )
{
	if( !ValidateObject( savageShaman ))
		return;

	if( timerID == 1 )
	{
		// Look for other nearby characters to affect by the Savage Dance
		savageShaman.nearbyChars = [];
		AreaCharacterFunction( "SavageDanceEffect", savageShaman, 8 );
		if( typeof(savageShaman.nearbyChars) != "undefined" )
		{
			// If we found some characters nearby, cast a random powerful group spell
			if( savageShaman.nearbyChars.length > 0 )
			{
				switch( RandomNumber( 1, 3 ))
				{
					case 1: // Greater Heal
						CastMassGreaterHeal( savageShaman );
						break;
					case 2: // Lightning
						CastChainLightning( savageShaman );
						break;
					case 3: // Poison
						CastMassPoison( savageShaman );
						break;
					default:
						break;
				}
			}
		}
	}
	else if( timerID == 2 )
	{
		// Reset fleeAt/reAttackAt properties back to original
		savageShaman.tempRetreating = false;
		savageShaman.fleeAt = savageShaman.origFleeAt;
		savageShaman.reAttackAt = savageShaman.origReAttackAt;
	}
	else
	{
		// Release shaman from spellcasting action
		savageShaman.frozen = false;
		savageShaman.isCasting = false;
		savageShaman.spellCast = -1;

		if( timerID == 29 ) // (Mass) Greater Heal Spell
		{
			MassGreaterHealEffect( savageShaman );
		}
		else if( timerID == 49 ) // Chain Lightning Spell
		{
			ChainLightningEffect( savageShaman );
		}
		else if( timerID == 20 ) // (Mass) Poison Spell
		{
			MassPoisonEffect( savageShaman );
		}
	}
}

function SavageDanceEffect( savageShaman, nearbyChar )
{
	if( !ValidateObject( nearbyChar ) || ( !nearbyChar.npc && !nearbyChar.online ))
		return;

	// Add the character we found to a list of nearby characters stored on the shaman JS object
	savageShaman.nearbyChars.push( nearbyChar );
}

// Cast a Mass Greater Heal spell that affect all other nearby Savages
function CastMassGreaterHeal( savageShaman )
{
	savageShaman.frozen = true;
	savageShaman.isCasting = true;

	var mSpell = Spells[29]; // Greater Heal spell
	savageShaman.spellCast = 29;

	// Pause shaman as they cast their Greater Heal spell
	savageShaman.SetTimer( Timer.MOVETIME, mSpell.delay * 1000 );

	savageShaman.DoAction( 0x0b, 0x01 );
	savageShaman.TextMessage( "Vas " + mSpell.mantra ); // Vas In Vas Mani

	savageShaman.StartTimer( mSpell.delay * 1000, 29, true );
}

// Handle application of effect for Mass Greater Heal
function MassGreaterHealEffect( savageShaman )
{
	// Finish the mass greater heal spell
	var mSpell = Spells[29];

	// Heal Shaman itself
	savageShaman.SpellStaticEffect( mSpell );
	savageShaman.health += 50;

	// Play SFX associated with spell
	savageShaman.SoundEffect( mSpell.soundEffect, true );

	var validTargets = [];
	for( var i = 0; i < savageShaman.nearbyChars.length; i++ )
	{
		var nearbyChar = savageShaman.nearbyChars[i];

		// Make other nearby Shamans assist in casting the spell
		if(( nearbyChar.race && nearbyChar.race.id == savageShaman.race.id ) && ( nearbyChar.HasScriptTrigger( savageShamanScriptID )))
		{
			nearbyChar.SetTimer( Timer.MOVETIME, mSpell.delay * 1000 );
			nearbyChar.DoAction( 0x0b, 0x01 );
		}

		// Only heal other Savages or Savage Ridgebacks
		if(( nearbyChar.race && nearbyChar.race.id == savageShaman.race.id ) || ( nearbyChar.id == 0x00BC ))
		{
			// Won't heal a poisoned ally
			if( nearbyChar.poison > 0 )
				continue;

			// Play spell FX associated with Greater Heal Spell
			nearbyChar.SpellStaticEffect( mSpell );

			// Healing amount = 40% of shaman's magery skill + 1 to 10 (random)
			var healAmount = (( savageShaman.skills.magery / 10 ) * 0.4 ) + RandomNumber( 1, 10 );
			nearbyChar.health += healAmount;
		}
	}
}

// Cast Chain Lightning at all enemies in range
function CastChainLightning( savageShaman )
{
	savageShaman.frozen = true;
	savageShaman.isCasting = true;

	var mSpell = Spells[49]; // Chain Lightning
	savageShaman.spellCast = 49;

	// Pause shaman as they cast their Greater Heal spell
	savageShaman.SetTimer( Timer.MOVETIME, mSpell.delay * 1000 );

	savageShaman.DoAction( 0x0b, 0x01 );
	savageShaman.TextMessage( mSpell.mantra ); // Vas Ort Grav

	savageShaman.StartTimer( mSpell.delay * 1000, 49, true );
}

// Handle application of effect for Chain Lightning
function ChainLightningEffect( savageShaman )
{
	// Finish the chain lightning spell
	var mSpell = Spells[49];

	// Play SFX associated with spell
	savageShaman.SoundEffect( mSpell.soundEffect, true );

	var validTargets = [];
	for( var i = 0; i < savageShaman.nearbyChars.length; i++ )
	{
		var nearbyChar = savageShaman.nearbyChars[i];

		// Make other nearby Shamans assist in casting the spell
		if(( nearbyChar.race && nearbyChar.race.id == savageShaman.race.id ) && ( nearbyChar.HasScriptTrigger( savageShamanScriptID )))
		{
			nearbyChar.SetTimer( Timer.MOVETIME, mSpell.delay * 1000 );
			nearbyChar.DoAction( 0x0b, 0x01 );
		}

		// Only cast lightning at enemies of the Savage Shaman
		if(( nearbyChar.race && nearbyChar.race.id != savageShaman.race.id ) && ( nearbyChar.id != 0x00BC ))
		{
			// Don't affect players wearing tribal paint
			if( nearbyChar.id == 0x00b7 || nearbyChar.id == 0x00b8 )
				continue;

			// Play spell FX associated Chain Lightning Spell, on target
			nearbyChar.BoltEffect();

			var baseDmg = mSpell.baseDmg;

			if( nearbyChar.magicReflect )
			{
				// Disable magic reflect
				nearbyChar.magicReflect = false;

				// Reflect spell back at Savage Shaman
				savageShaman.SpellStaticEffect( mSpell );
				savageShaman.BoltEffect();
				savageShaman.health -= baseDmg;
			}
			else
			{
				// Randomize in upper-half of damage range for some variety
				baseDmg = RandomNumber( Math.round( baseDmg / 2 ), baseDmg );

				// Take target's magic resistance into account
				var spellResisted = TriggerEvent( 6000, "CheckResist", savageShaman, nearbyChar, mSpell.circle );

				// Calculate final damage based on caster's eval int vs target's resistance, and whether spell was resisted
				baseDmg = TriggerEvent( 6001, "CalcFinalSpellDamage", savageShaman, nearbyChar, baseDmg, spellResisted )

				// Apply the damage
				nearbyChar.Damage( baseDmg, 6, savageShaman );
			}
		}
	}
}

// Cast Mass Poison at all enemies in range
function CastMassPoison( savageShaman )
{
	savageShaman.frozen = true;
	savageShaman.isCasting = true;

	var mSpell = Spells[20]; // Poison
	savageShaman.spellCast = 20;

	// Pause shaman as they cast their Greater Heal spell
	savageShaman.SetTimer( Timer.MOVETIME, mSpell.delay * 1000 );

	savageShaman.DoAction( 0x0b, 0x01 );
	savageShaman.TextMessage( "Vas " + mSpell.mantra ); // Vas In Nox

	savageShaman.StartTimer( mSpell.delay * 1000, 20, true );
}

// Handle application of effect for Mass Poison
function MassPoisonEffect( savageShaman )
{
	// Finish the mass greater heal spell
	var mSpell = Spells[20];

	// Play SFX associated with spell
	savageShaman.SoundEffect( mSpell.soundEffect, true );

	var validTargets = [];
	for( var i = 0; i < savageShaman.nearbyChars.length; i++ )
	{
		var nearbyChar = savageShaman.nearbyChars[i];

		// Make other nearby Shamans assist in casting the spell
		if(( nearbyChar.race && nearbyChar.race.id == savageShaman.race.id ) && ( nearbyChar.HasScriptTrigger( savageShamanScriptID )))
		{
			nearbyChar.SetTimer( Timer.MOVETIME, mSpell.delay * 1000 );
			nearbyChar.DoAction( 0x0b, 0x01 );
		}

		// Only cast lightning at enemies of the Savage Shaman
		if(( nearbyChar.race && nearbyChar.race.id != savageShaman.race.id ) && ( nearbyChar.id != 0x00BC ))
		{
			// Don't affect players wearing tribal paint
			if( nearbyChar.id == 0x00b7 || nearbyChar.id == 0x00b8 )
				continue;

			// Play spell FX associated with (Mass) Poison Spell, on target
			nearbyChar.SpellStaticEffect( mSpell );

			// Calculate poison strength
			var poisonStrength = CalcMagicPosionStrength( savageShaman, nearbyChar );

			// Apply the poison, if it's stronger or equal to what what is already on character
			if( nearbyChar.poison <= poisonStrength )
			{
				var poisonDuration = CalcPoisonDuration( poisonStrength );
				nearbyChar.SetPoisoned( poisonStrength, poisonDuration * 1000 );
			}
		}
	}
}

function CalcMagicPosionStrength( savageShaman, nearbyChar )
{
	// Default to lesser poison if target is further than 2 tiles away from caster
	var poisonStrength = 1;
	var skillVal = 0;

	// Check if caster is within 2 tiles from target
	if( savageShaman.DistanceTo( nearbyChar ) > 2 )
	{
		// Calculate poison strength based on average of caster's Magery and Poisoning skills
		skillVal = (( savageShaman.skills.magery + savageShaman.skills.poisoning ) / 2 );
		if( skillVal >= 1000 ) // GM in both skills
		{
			// 5% chance of Deadly Poison
			if( RandomNum( 1, 100 ) <= 5 )
			{
				poisonStrength = 4; // Deadly Poison
			}
			else
			{
				poisonStrength = 3; // Greater Poison
			}
		}
		else if( skillVal > 701 )
		{
			poisonStrength = 3; // Greater Poison
		}
		else if( skillVal > 301 )
		{
			poisonStrength = 2; // Regular Poison
		}
	}

	// Take target's magic resistance into account
	var spellResisted = TriggerEvent( 6000, "CheckResist", savageShaman, nearbyChar, 3 );
	if( spellResisted && poisonStrength > 1 )
	{
		// Reduce poison strength, since it was resisted
		poisonStrength--;
	}

	return poisonStrength;
}

function CalcPoisonDuration( poisonStrength )
{
	// Calculate duration of poison, based on the strength of the poison
	var poisonDuration = 0;
	switch( poisonStrength )
	{
		case 1: // Lesser poison - 9 to 13 pulses, 2 second frequency
			poisonDuration = RandomNumber( 9, 13 ) * 2;
			break;
		case 2: // Normal poison - 10 to 14 pulses, 3 second frequency
			poisonDuration = RandomNumber( 10, 14 ) * 3;
			break;
		case 3: // Greater poison - 11 to 15 pulses, 4 second frequency
			poisonDuration = RandomNumber( 11, 15 ) * 4;
			break;
		case 4: // Deadly poison - 12 to 16 pulses, 5 second frequency
			poisonDuration = RandomNumber( 12, 16 ) * 5;
			break;
		case 5: // Lethal poison - 13 to 17 pulses, 5 second frequency
			poisonDuration = RandomNumber( 13, 17 ) * 5;
			break;
	}
	return poisonDuration;
}