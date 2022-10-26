// Equip-effects for magical armors
// See magicarmor.js for details on full setup

function onEquip( pEquipper, iEquipped )
{
	if( iEquipped.type != 15 )
		return;

	// Fetch which spell is enabled for this item
	var spellCircle = iEquipped.morex;
	var spellNumber = iEquipped.morey;
	var spellCharges = iEquipped.morez;

	// Check if the item has any charges left
	if( spellCharges == 0 )
	{
		pEquipper.socket.SysMessage( GetDictionaryEntry( 0, pEquipper.socket.language )); // This item is out of charges.
		return;
	}

	// Play effects associated with spell
	var mSpell = Spells[spellNumber];
	pEquipper.SoundEffect( spellNumber, true );
	pEquipper.SpellStaticEffect( mSpell );
	iEquipped.morez--; // Reduce spell charges on item by 1

	// Apply effect to character based on the type of spell
	switch( spellNumber )
	{
		case 1: // Clumsy
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 25; // Duration of effect - if cast by someone with Magery skill of 100.0 the duration would be 50 seconds
			var resistDifficulty = 500; // Difficulty of resisting the effect. 500 is equal difficulty to resisting the spell if cast by someone with 50.0 Magery
			DoTempEffect( 0, null, pEquipper, 3, effectStrength, effectDuration, resistDifficulty );
			break;
		case 3: // Feeblemindedness
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 25; // Duration of effect - if cast by someone with Magery skill of 100.0 the duration would be 50 seconds
			var resistDifficulty = 500; // Difficulty of resisting the effect. 500 is equal difficulty to resisting the spell if cast by someone with 50.0 Magery
			DoTempEffect( 0, null, pEquipper, 4, effectStrength, effectDuration, resistDifficulty );
			break;
		case 4: // Blessings
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 50; // Duration of effect - if cast by someone with Magery skill of 100.0 the duration would be 100 seconds
			DoTempEffect( 0, null, pEquipper, 11, effectStrength, effectDuration, 0 );
			break;
		case 5: // Protection
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 120; // Duration of effect - if cast as spell, has fixed duration of 120 seconds
			DoTempEffect( 0, null, pEquipper, 21, effectStrength, effectDuration, 0 );
			break;
		case 6: // Night Eyes (Night Sight)
			var effectDuration = 100;
			DoTempEffect( 0, null, pEquipper, 2, 0, effectDuration, 0 );
			break;
		case 8: // Weakness
			var effectStrength = 5;
			var effectDuration = 50;
			var resistDifficulty = 500; // Difficulty of resisting the effect. 500 is equal difficulty to resisting the spell if cast by someone with 50.0 Magery
			DoTempEffect( 0, null, pEquipper, 5, effectStrength, effectDuration, resistDifficulty );
			break;
		case 10: // Cunning
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 100; // Duration of effect - if cast as a spell, has fixed duration of 100 seconds
			DoTempEffect( 0, null, pEquipper, 7, effectStrength, effectDuration, 0 );
			break;
		case 16: // Strength
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 100; // Duration of effect - if cast as a spell, has fixed duration of 100 seconds
			DoTempEffect( 0, null, pEquipper, 8, effectStrength, effectDuration, 0 );
			break;
		case 19: // Agility
			var effectStrength = 5; // Effect strength from 1-10, relative to caster of this spell with Magery skill from 10-100
			var effectDuration = 100; // Duration of effect - if cast as a spell, has fixed duration of 100 seconds
			DoTempEffect( 0, null, pEquipper, 6, effectStrength, effectDuration, 0 );
			break;
		case 27: // Curse
			var effectStrength = 5;
			var effectDuration = 50;
			var resistDifficulty = 500; // Difficulty of resisting the effect. 500 is equal difficulty to resisting the spell if cast by someone with 50.0 Magery
			DoTempEffect( 0, null, pEquipper, 12, effectStrength, effectDuration, resistDifficulty );
			break;
		case 36: // Spell Reflection
			pEquipper.magicReflect = true;
			break;
		case 44: // Invisibility
			pEquipper.visible = 2;

			var invisTimer = GetServerSetting( "InvisibilityTimer" );
			pEquipper.SetTimer( Timer.INVIS, invisTimer );

			// Invalidate the attacker to prevent character from auto-attacking/defending and breaking invisibility
			if( !pEquipper.npc )
				pEquipper.InvalidateAttacker();
			break;
		default:
			break;
	}
}

// Display amount of charges left on magic armor in item tooltips
function onTooltip( myObj )
{
	var tooltipText = "";
	if( myObj.type != 15 && ( myObj.name2 == "#" || myObj.name2 == "" ))
	{
		tooltipText = GetDictionaryEntry( 9403 ); // uses remaining: %i
		tooltipText = ( tooltipText.replace( /%i/gi, ( myObj.morez ).toString() ));
	}
	return tooltipText;
}