// Exodus clockwork monsters are protected by a green shield that renders them
// immune to physical damage, until the shield in question has been destroyed
// using magical damage! Once the shield is destroyed, they can be damaged by normal
// attacks, but will then be immune to magical attacks!
//
// Magical attacks done while shield is down will reflect back in the form of an e-bolt,
// bouncing of the creature's reflective armor (around 50% of the time?)
//
// If combat ends and the NPC is still alive, the magic shield will be re-activated in
// a couple of seconds

function onCreateDFN( exodusNPC, objType )
{
	if( objType != 1 || !exodusNPC.npc )
		return;

	// Initial setup of magical shield - equals maxhp
	exodusNPC.SetTag( "ShieldHP", exodusNPC.maxhp );
}

function onAICombatTarget( exodusNPC, pTarget )
{
	// Whenever exodusNPC enters combat, display shield, if it's active
	var shieldHP = exodusNPC.GetTag( "ShieldHP" );
	if( shieldHP > 0 )
	{
		exodusNPC.KillTimers();
		exodusNPC.StartTimer( 2000, 1, true );
	}
	return true;
}

function onTimer( exodusNPC, timerID )
{
	// As long as timerID is under 10, play the effect again
	if( timerID < 10 )
	{
		var shieldHP = exodusNPC.GetTag( "ShieldHP" );
		if( shieldHP > 0 )
		{
			// Play visual effect associated with magical shield
			DoMovingEffect( exodusNPC, exodusNPC, 0x375a, 0, 0x10, false, 0x089F, 0x0 );

			// Queue up additional timer
			exodusNPC.StartTimer( 2000, timerID + 1, true );
		}
	}
	else if( timerID == 20 )
	{
		// Reactivate and recharge magic shield
		exodusNPC.SetTag( "ShieldHP", exodusNPC.maxhp );

		// Play visual effect associated with magical shield
		DoMovingEffect( exodusNPC, exodusNPC, 0x375a, 0, 0x10, false, 0x089F, 0x0 );

		// Queue up additional timer
		exodusNPC.StartTimer( 2000, 1, true );
	}
}

function onCombatEnd( exodusNPC, targChar )
{
	exodusNPC.StartTimer( 5000, 20, true )
	return true;
}

function onDamage( exodusNPC, attacker, damageValue, damageType )
{
	var immuneToMagic = false;
	var immuneToMelee = true;
	var shieldHP = exodusNPC.GetTag( "ShieldHP" );
	if( shieldHP == 0 )
	{
		// Shield is down. NPC can be damaged by physical attacks, but not by magical ones
		immuneToMelee = false;
		immuneToMagic = true;
	}

	if( damageType == 1 ) // Physical damage
	{
		if( immuneToMelee )
		{
			// Creature is immune to Physical damage, so return false
			DoMovingEffect( exodusNPC, exodusNPC, 0x375a, 0, 0x10, false, 0x089F, 0x0 );
			exodusNPC.SoundEffect( 0x02f4, true );

			if( ValidateObject( attacker ))
			{
				var socket = attacker.socket;
				if( socket != null )
				{
					socket.SysMessage( GetDictionaryEntry( 9130, socket.language )); // Your weapon cannot penetrate the creature's magical barrier.
				}
			}
			return false;
		}
	}
	else if( damageType != 1 ) // Magical/Other damage
	{
		if( immuneToMagic )
		{
			if( ValidateObject( attacker ))
			{
				// Creature is immune to Magical damage, so return false...
				// ... after returning fire in the shape of an e-bolt!
				if( RandomNumber( 0, 1 ))
				{
					// Shoot energy bolt at attacker
					var eBoltSpell = Spells[42]; // Fetch energy bolt data
					exodusNPC.SoundEffect( eBoltSpell.soundEffect, true );
					exodusNPC.SpellMoveEffect( attacker, eBoltSpell );
					attacker.Damage( eBoltSpell.baseDmg, 6, exodusNPC );
				}

				var socket = attacker.socket;
				if( socket != null )
				{
					socket.SysMessage( GetDictionaryEntry( 9056, socket.language )); // That target is immune to magic!
				}
			}
			return false;
		}
		else
		{
			if( shieldHP > 0 )
			{
				// Reduce the health of the magical shield
				shieldHP -= damageValue;
				if( shieldHP < 0 )
				{
					shieldHP = 0;
				}
				exodusNPC.SetTag( "ShieldHP", shieldHP );

				if( shieldHP == 0 )
				{
					// Shield destroyed! Play effect to indicate destruction of magical shield
					DoMovingEffect( exodusNPC, exodusNPC, 0x3728, 0, 0x0d, false, 0x089F, 0x0 );
				}

				// Return false, since magical shield absorbed damage!
				return false;
			}
		}
	}

	// Otherwise, let damage pass through
	return true;
}

// Clockwork Minion's periodic electric burst aoe attack
function onSwing( iSwung, pSwinging, pSwingAt )
{
	if( pSwinging.id == 0x02f5 && RandomNumber( 1, 10 ) > 1 )
	{
		// Play the AoE effect around exodusNPC
		DoMovingEffect( pSwinging, pSwinging, 0x374a, 0, 0x10, false, 0x089F, 0x0 );

		// Find targets within range, and apply damage to them
		AreaCharacterFunction( "ElectricBurstAoE", pSwinging, 4 );

		// Return false to prevent both this special attack and regular attack from happening at same time!
		return false;
	}

	// Let normal onSwing go through
	return true;
}

function ElectricBurstAoE( exodusNPC, trgChar )
{
	if( trgChar == exodusNPC )
		return false;

	if( ValidateObject( trgChar ) && (( !trgChar.npc && trgChar.online && !trgChar.dead ) || trgChar.npc ) && trgChar.vulnerable )
	{
		// Play a moving effect from exodusNPC to target
		DoMovingEffect( exodusNPC, trgChar, 0x36f4, 8, 0x0a, true, 0x089F, 0x0 );

		// Valid target - apply damage
		trgChar.Damage( RandomNumber( 25, 75 ), 6, exodusNPC );
	}
	return true;
}