// This script is assigned to all corpses by default, in code, but can be overridden on individual items and/or via global script
const youngPlayerSystem = GetServerSetting( "YoungPlayerSystem" );

function onCarveCorpse( pChar, iCorpse )
{
	var corpseOwner = iCorpse.owner;
	if( ValidateObject( corpseOwner ))
	{
		if( !corpseOwner.npc )
		{
			// Disallow young players from carving player corpses
			if( pChar.account.isYoung )
			{
				pChar.socket.SysMessage( GetDictionaryEntry( 18746, pChar.socket.language )); // As a Young player, you cannot carve up player corpses.
				return false;
			}

			// Trigger check for corpse carving in facet ruleset script
			return TriggerEvent( 2507, "CheckCorpseCarving", pChar, corpseOwner, iCorpse );
		}
	}

	// Default to allow carving up corpses (monsters, animals, etc)
	return true;
}

// Triggers when someone tries to pick up an item from the corpse
function onPickup( iPickedUp, pGrabber, containerObj )
{
	if( !ValidateObject( iPickedUp) || !ValidateObject( pGrabber ) || !ValidateObject( containerObj ))
		return false;

	if( pGrabber.npc )
		return true;

	var pSock = pGrabber.socket;
	var corpseOwner = containerObj.owner;
	// Is corpse owned by a character who is not pGrabber?
	if( ValidateObject( corpseOwner ) && corpseOwner != pGrabber )
	{
		// Corpse owner is an NPC who somehow still exists (pet ghost?)... but does the NPC have an owner that is NOT an NPC?
		if( corpseOwner.npc && ValidateObject( corpseOwner.owner ) && !corpseOwner.owner.npc )
		{
			// If so, assume the player in question is the owner of the NPC's corpse
			corpseOwner = corpseOwner.owner;
		}

		// Is corpse owned by a player?
		if( !corpseOwner.npc )
		{
			if( youngPlayerSystem )
			{
				// If so, is the player trying to loot it a Young player?
				if( pGrabber.account.isYoung )
				{
					// Disallow - Young players cannot loot corpses of other players
					if( pSock )
					{
						pSock.SysMessage( GetDictionaryEntry( 18740, pSock.language )); // You may not take items from the corpses of the more established players.
					}
					return false;
				}
				else if( corpseOwner.account.isYoung ) // Or is the corpse-owner Young?
				{
					// Disallow - "elder" players cannot loot corpses of Young players
					if( pSock )
					{
						pSock.SysMessage( GetDictionaryEntry( 18741, pSock.language )); // You cannot steal from the Young.
					}
					return false;
				}
			}
		}
	}

	// Don't allow elder players to loot monsters killed by Young players
	var corpseKiller = CalcCharFromSer( containerObj.morex );
	if( ValidateObject( corpseKiller ))
	{
		if( corpseKiller != pGrabber )
		{
			if( !corpseKiller.npc && youngPlayerSystem && corpseKiller.account.isYoung )
			{
				if( pSock )
				{
					pSock.SysMessage( GetDictionaryEntry( 18742, pSock.language )); // Some young warrior felled this, and you cannot bring yourself to take any items from the corpse.
				}
				return false;
			}
			else if( !pGrabber.npc && youngPlayerSystem && pGrabber.account.isYoung )
			{
				// Young players cannot loot a monster they did not kill themselves for 2 minutes after the creature died
				var timeSinceDeath = parseInt( GetCurrentClock() / 1000 ) - parseInt( ourObj.tempTimer / 1000 );
				if( timeSinceDeath < 120 )
				{
					pSock.SysMessage( GetDictionaryEntry( 18743, pSock.language )); // You cannot loot this corpse yet. Try again in a few minutes!
					return false;
				}
			}
		}
	}

	// Allow pickup by default
	return true;
}