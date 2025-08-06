/// <reference path="../../../definitions.d.ts" />
// @ts-check
function onDeathBlow( zealot, pKiller )
{
	// Prevent death when zealot is in their human form, morph into undead
	if( zealot.id == 0x0190 || zealot.id == 0x0191 )
	{
		MorphIntoUndead( zealot, pKiller );
		return false;
	}

	// Don't prevent death when zealot is in their undead form
	return true;
}

function MorphIntoUndead( zealot, pKiller )
{
	// Play FX/SFX associated with Summon Creature spell
	var spellNum = 40; // Summon Creature

	// Get Spell object based on ID
	var summonSpell = Spells[spellNum];

	// Play Summon Creature STATFX on the zealot
	zealot.SpellStaticEffect( summonSpell );

	// Play SFX associated with Summon Creature spell
	zealot.SoundEffect( summonSpell.soundEffect, true );

	if( zealot.id == 0x0190 )
	{
		// Male zealots morph into Bone Knight/Skeletal Knight
		let boneKnight = SpawnNPC( "skeletalknight", zealot.x, zealot.y, zealot.z, zealot.worldnumber, zealot.instanceID );
		if( ValidateObject( boneKnight ))
		{
			if( ValidateObject( pKiller ))
			{
				boneKnight.InitiateCombat( pKiller );
			}
			zealot.Delete();
		}
	}
	else
	{
		// Female zealots morph into Bone Magi/Skeletal Mage
		// bonemage
		let skeletalmage = SpawnNPC( "skeletalmage", zealot.x, zealot.y, zealot.z, zealot.worldnumber, zealot.instanceID );
		if( ValidateObject( skeletalmage ))
		{
			if( ValidateObject( pKiller ))
			{
				skeletalmage.InitiateCombat( pKiller );
			}
			zealot.Delete();
		}
	}
}