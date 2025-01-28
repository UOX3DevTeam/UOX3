// Enable and Disable the weapon Special Moves
const paralyzingBlow = true;
const crushingBlow = true;
const concussionBlow = true;

function onEquip( pEquipper, iEquipped ) 
{
	pEquipper.AddScriptTrigger( 5050 );
}

// Remove script trigger on unequip
function onUnequip( pUnequipper, iUnequipped ) 
{
	pUnequipper.RemoveScriptTrigger( 5050 );
}

function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	const coreShardEra = GetServerSetting( "CoreShardEra" );
	var weaponType = TriggerEvent( 2500, "GetWeaponType", pAttacker, null );
	var anatomySkillValue = pAttacker.baseskills.anatomy;
	var scaledSkillValue = anatomySkillValue / 4;
	var randomDouble = Math.random() * 1000

	if(( EraStringToNum( coreShardEra ) >= EraStringToNum("uor") && EraStringToNum( coreShardEra ) <= EraStringToNum( "lbr" )) && scaledSkillValue >= randomDouble )
	{
		if( paralyzingBlow && weaponType == "TWOHND_FENCING" && pDefender.frozen == false ) 
		{
			ParalyzingBlow( pAttacker, pDefender );
		}
		else if( crushingBlow && weaponType == "LG_MACES" ) 
		{
			CrushingBlow( pAttacker, pDefender );
		}
		else if( concussionBlow && ( weaponType == "TWOHND_LG_SWORDS" || weaponType == "TWOHND_AXES" || weaponType == "BARDICHE" ) && pDefender.GetTempTag( "concussion" ) == 0 )
		{
			ConcussionBlow( pAttacker, pDefender );
		}
	}
}

/*
A special attack is available to those with the Fencing skill who wield 2 - handed Fencing weapons in combat.Upon a successful hit,
there is a small chance to inflict a Paralyzing Blow.The victim is paralyzed for five seconds and unable to fight back or cast spells.However,
the victim may still use potions and bandages while paralyzed.
A paralyzed victim may not break this paralysis by any means and may not be paralyzed again until the paralysis wears off.
The base chance to inflict this special damage is your Anatomy skill level divided by 4.
*/

function ParalyzingBlow( pAttacker, pDefender )
{
	var seconds = 3000; // We want this applied to players even if they are "offline" (aka disconnected but not vanished from view yet)
	if( pDefender.npc )
	{
		seconds = 6000;
	}
	else if( pDefender.socket )
	{
		pDefender.TextMessage( GetDictionaryEntry( 17701, pDefender.socket.language ), false, 0x3b2, 0, pDefender.serial );// You receive a paralyzing blow!
	}

	pDefender.StartTimer( seconds, 1, true );
	pDefender.frozen = true;

	if( pAttacker.socket )
	{
		pAttacker.TextMessage( GetDictionaryEntry( 17702, pAttacker.socket.language ), false, 0x3b2, 0, pAttacker.serial );// You deliver a paralyzing blow!
		pAttacker.SoundEffect( 0x11C, true );
	}
}

/*
Mace Fighting has two advantages over other types of combat:
A mace does considerable damage to the armor of your opponent.
Mace fighters have a chance of "stunning" their opponent for a moment.
Upon a successful hit with a two handed Mace type weapon (excluding staves),
there will be a small chance to perform a Crushing Blow, which is a hit for double damage.
The base chance to inflict this special damage is your Anatomy skill level divided by 4. The only weapon that can be used for this special attack is the War Hammer
*/

function CrushingBlow( pAttacker, pDefender ) 
{
	var staminaLoss = Math.floor( Math.random() * ( 5 - 3 + 1 )) + 3;
	if( pDefender.socket )
	{
		pDefender.TextMessage(GetDictionaryEntry( 17703, pDefender.socket.language ), false, 0x3b2, 0, pDefender.serial );// You receive a crushing blow!
	}

	pDefender.stamina -= staminaLoss;

	if( pAttacker.socket ) 
	{
		pAttacker.TextMessage( GetDictionaryEntry( 17704, pAttacker.socket.language ), false, 0x3b2, 0, pAttacker.serial );// You deliver a crushing blow!
		pAttacker.SoundEffect( 0x11C, true );
	}
}

/*
If a swordsman is using a two-handed weapon, he may sometimes deliver a Concussion Blow,
during which the victim’s intelligence is halved for 30 seconds. The effects of a concussion blow are not cumulative,
once a target is the victim of a concussion blow, they cannot be hit in that manner again for 30 seconds. 
The base chance to deliver this special blow is your Anatomy skill level divided by 4.
*/

function ConcussionBlow( pAttacker, pDefender )
{
	if( pDefender.socket )
	{
		pDefender.TextMessage( GetDictionaryEntry( 17705, pDefender.socket.language ), false, 0x3b2, 0, pDefender.serial );// You receive a concussion blow!
	}

	// Drop pDefender's tempint by half their intelligence!
	var cbTempIntDiff = Math.round( pDefender.intelligence / 2 );
	pDefender.tempint -= cbTempIntDiff;

	// Store the amount dropped in a tag, to be retrieved and reversed after timer has ran out
	pDefender.SetTag( "cbTempIntDiff", cbTempIntDiff );

	// Mark pDefender as concussed
	pDefender.SetTag( "concussion", 1 );

	pDefender.StartTimer( 30000, 2, true );

	if( pAttacker.socket ) 
	{
		pAttacker.TextMessage( GetDictionaryEntry( 17706, pAttacker.socket.language ), false, 0x3b2, 0, pAttacker.serial );// You deliver a concussion blow!
		pAttacker.SoundEffect( 0x11C, true );
	}
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		timerObj.frozen = false;
	}

	if( timerID == 2 )
	{
		timerObj.tempint += parseInt( timerObj.GetTag( "cbTempIntDiff" ));
		timerObj.SetTag( "cbTempIntDiff", null );
		timerObj.SetTag( "concussion", null );
	}
}

function _restorecontext_() {}