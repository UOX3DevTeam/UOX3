function onSpellTarget( pTarget, pCaster, spellID )
{
	var socket = pCaster.socket;

	if( !ValidateObject( pTarget ) || !ValidateObject( pCaster ) )
		return;

	ReflectSpellToCaster( pCaster, pTarget, spellID );

	return 0;
}

function ReflectSpellToCaster( pCaster, pTarget, spellID )
{
	// Only reflect for male characters not polymorphed into female or wraith form or gm
	if( pCaster.gender == 1 ||  pCaster.gender == 255 )
		return 0;

	// Don't reflect if casting on self (e.g., NPC casts a spell on itself)
	if( pCaster == pTarget )
		return;

	let body = pCaster.id;
	let isPoly = pCaster.isPolymorphed;
	let isFemaleForm = ( body == 0x0191 || body == 0x025E );
	let isWraithForm = ( body == 0x03CA );

	// Only skip reflect if polymorphed *and* in female or wraith body
	if( isPoly && ( isFemaleForm || isWraithForm ))
		return 0;

	// Fetch spell data from global array
	let mSpell = Spells[spellID];
	if( !mSpell )
	{
		return 0;
	}

	let baseDmg = mSpell.baseDmg;
	if( baseDmg <= 0 )
	{
		return 0;
	}

	// Call native damage calculator
	let finalDmg = TriggerEvent( 6001, "CalcFinalSpellDamage", pCaster, pTarget, baseDmg, false );
	if( !finalDmg || finalDmg <= 0 )
		finalDmg = baseDmg;

	// Reflect damage to caster
	pCaster.Damage( finalDmg, 6, pCaster, false ); // 6 = energy
	let hpGain = Math.min( pTarget.health + finalDmg, pTarget.maxhp );
	pTarget.health = hpGain;
	DoMovingEffect( pTarget, pCaster, 0x374A, 0x10, 0x15, false, 0x496, 0 );
    pTarget.SoundEffect( 0x231, true );
	return 0;
}

function _restorecontext_() {}