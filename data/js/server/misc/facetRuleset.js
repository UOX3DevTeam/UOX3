const coreShardEra = GetServerSetting( "CoreShardEra" );
const timeBetweenMurdererVisit = 2592000; // 30 days between each time a murderer can visit Ilshenar

// Define which rules apply to which facets
// Example: const facetCorpseCarveRestrict = [ 1, 2, 3, 4 ]; // Corpse carving restricted in Trammel, Ilshenar, Malas, Tokuno
const facetCorpseCarveRestrict = [1, 2, 3, 4, 5];			// Prevents players from carving corpses of other players
const facetMurdererRestrict = [1, 2, 3, 4, 5];				// Restricts access to facet for Murderers
const facetCombatRestrict = [1, 2, 3, 4, 5];				// Prevents players from attacking other players
const facetSpellRestrict = [1, 2, 3, 4, 5];					// Prevents players from casting hostile and/or beneficial spells on other players
const facetPotionRestrict = [1, 2, 3, 4, 5];				// Prevents players from damaging other players with explosion potions
const facetStealRestrict = [1, 2, 3, 4, 5];					// Prevents players from stealing from other players
const facetSnoopRestrict = [1, 2, 3, 4, 5];					// Prevents players from snooping in packs of other players
const facetLootingRestrict = [1, 2, 3, 4, 5];				// Prevents players from looting corpses if that would make them criminal
const facetBlockMovementRestrict = [1, 2, 3, 4, 5];			// Prevents characters from being blocked by other players and/or movable objects
const facetBladeSpiritTargetRestrict = [1, 2, 3, 4, 5]; 	// Restricts BS targets to monsters, wild animals and caster and guild members/enemies
const facetEnergyVortexTargetRestrict = [1, 2, 3, 4, 5];	// Restricts EV targets to monsters, wild animals and caster and guild members/enemies
const facetBardProvokeRestrict = [1, 2, 3, 4, 5];			// Restricts targets for bard provocation skill to monsters, wild animals, caster and guild members/enemies

// Override above facet-specific rules for specific regions
// Example: const regionCorpseCarveOverride = [ 17, 23, 56 ]; // Override corpse carving restrictions in these regions
const regionCorpseCarveOverride = [];
const regionMurdererOverride = [];
const regionCombatOverride = [];
const regionSpellOverride = [];
const regionPotionOverride = [];
const regionStealOverride = [];
const regionSnoopOverride = [];
const regionLootingOverride = [];
const regionBlockMovementOverride = [];
const regionBladeSpiritTargetOverride = [];
const regionEnergyVortexTargetOverride = [];
const regionBardProvokeOverride = [];

function onCombatStart( pAttacker, pDefender )
{
	var socket = pAttacker.socket;
	var worldNum = pAttacker.worldnumber;

	if( regionCombatOverride.indexOf( pAttacker.region.id ) == -1 && facetCombatRestrict.indexOf( pAttacker.worldnumber ) != -1 )
	{
		if( pDefender.npc )
		{
		 	if( ValidateObject( pDefender.owner ))
		 	{
				// Treat Defender as if it IS its owner
				pDefender = pDefender.owner;
		 	}
		 	else
		 	{
			 	if(( pDefender.npcFlag == 1 && pDefender.innocent ) || pDefender.innocent )
			 	{
			 		// Don't allow starting combat against "good" NPCs that are innocent, or against innocent NPCs
			 		socket.SysMessage( GetDictionaryEntry( 9230, socket.language )); // You cannot perform negative acts on your target.
					return false;
			 	}
			 	else
			 	{
			 		// Allow combat against non-good and/or non-innocent NPCs
			 		return true;
			 	}
		 	}
		}

		// Player cannot attack other players in Trammel, Ilshenar, or Malas - unless they are in the same guild or in a guild they're at war with
		var guildRelations = CheckGuildRelationShip( pAttacker, pDefender );
		if( guildRelations != 1 && guildRelations != 4 )
		{
			// Neither in same guild nor at war, disallow combat
			socket.SysMessage( GetDictionaryEntry( 9230, socket.language )); // You cannot perform negative acts on your target.
			return false;
		}
	}

	// By default, allow hard-coded combat start process to continue
	return true;
}

// Players cannot cast aggressive spells at other players in Trammel/Ilshenar/Malas
function onSpellTarget( myTarget, pCaster, spellID )
{
	// We don't care if caster and target is the same
	if( myTarget == pCaster )
		return 0;

	var socket = pCaster.socket;
	if( socket != null )
	{
		var worldNum = pCaster.worldnumber;
		if( regionSpellOverride.indexOf( pCaster.region.id ) == -1 && facetSpellRestrict.indexOf( pCaster.worldnumber ) != -1 )
		{
			var spellCast = Spells[spellID];
			if( spellCast.aggressiveSpell )
			{
				if( myTarget.npc )
				{
				 	if( ValidateObject( myTarget.owner ))
				 	{
						// Treat target as if it IS its owner
						myTarget = myTarget.owner;
				 	}
				 	else
				 	{
					 	if(( myTarget.npcFlag == 1 && myTarget.innocent ) || myTarget.innocent )
					 	{
					 		// Don't allow aggressive spells against blue/innocent/good NPCs
					 		socket.SysMessage( GetDictionaryEntry( 9230, socket.language )); // You cannot perform negative acts on your target.
							return 2;
					 	}
					 	else
					 	{
					 		// Allow aggressive spell against grey/criminal or red/murderer NPCs
					 		return false;
					 	}
				 	}
				}

				// Aggressive spells only allowed between guild members or between guilds at war
				var guildRelations = CheckGuildRelationShip( pCaster, myTarget );
				if( guildRelations != 1 && guildRelations != 4 )
				{
					// Neither in same guild nor at war
					socket.SysMessage( GetDictionaryEntry( 9230, socket.language )); // You cannot perform negative acts on your target.
					return 2;
				}
			}
			else // Non-aggressive spells
			{
				if( myTarget.npc )
				{
					if( ValidateObject( myTarget.owner ))
					{
						// Treat target as if it IS its owner
						myTarget = myTarget.owner;
					}
					else if( !myTarget.isHuman || ( !myTarget.innocent && myTarget.npcFlag != 1 ))
					{
						socket.SysMessage( GetDictionaryEntry( 9231, socket.language )); // You cannot perform beneficial acts on your target.
						return 2;
					}
				}

				// Beneficial spellcasts only allowed between...
				if( guildRelations == 4 || ( pCaster.guild == null && myTarget.guild == null ))
				{
					// ...players in same guild / non-guild players and non-guild players
					return false;
				}
				else if(( pCaster.guild == null && myTarget.guild != null && myTarget.guild.IsAtPeace() ) ||
					( myTarget.guild == null && pCaster.guild != null && pCaster.guild.IsAtPeace() ))
				{
					// ...non-guild players and players in guilds not at war with anyone
					return false;
				}
				else if( pCaster.guild != null && pCaster.guild.IsAtPeace() && myTarget.guild != null && myTarget.guild.IsAtPeace() )
				{
					// ...players in guilds not at war with anyone and another player in another guild not at war with anyone
					return false;
				}

				socket.SysMessage( GetDictionaryEntry( 9231, socket.language )); // You cannot perform beneficial acts on your target.
				return 2;
			}
		}
	}
	return false;
}

// Players cannot steal from other players in Trammel/Ilshenar/Malas
function onSteal( pThief, iStolen, pVictim )
{
	if( !ValidateObject( pThief ) || !ValidateObject( pVictim ))
		return 1;

	var socket = pThief.socket;
	var worldNum = pThief.worldnumber;
	if( regionStealOverride.indexOf( pThief.region.id ) == -1 && facetStealRestrict.indexOf( pThief.worldnumber ) != -1 && !pVictim.npc )
	{
		socket.SysMessage( GetDictionaryEntry( 9230, socket.language )); // You cannot perform negative acts on your target.
		return 1;
	}
	return 0;
}

// Players cannot snoop the backpacks of other players in Trammel/Ilshenar/Malas
function onSnoopAttempt( pSnooped, targPack, pSnooping )
{
	if( !ValidateObject( pSnooped ) || !ValidateObject( pSnooping ) || !ValidateObject( targPack ))
		return true;

	if( pSnooping.isGM ) // Override for GMs
		return true;

	var socket = pSnooping.socket;
	var worldNum = pSnooping.worldnumber;
	if(( regionSnoopOverride.indexOf( pSnooping.region.id ) == -1 && facetSnoopRestrict.indexOf( pSnooping.worldnumber ) != -1 ) && !pSnooped.npc )
	{
		socket.SysMessage( GetDictionaryEntry( 9230, socket.language )); // You cannot perform negative acts on your target.
		return false;
	}

	// Default to allow snooping
	return true;
}

function onFlagChange( pChanging, newStatus, oldStatus )
{
	var worldNum = pChanging.worldnumber;
	if( regionMurdererOverride.indexOf( pChanging.region.id ) == -1 && facetMurdererRestrict.indexOf( pChanging.worldnumber ) != -1 )
	{
		if( newStatus == 0x01 ) // Murderer
		{
			if( worldNum == 2 ) // Ilshenar
			{
				// Murderers can go to Ilshenar once per month. This limitation does not reset if they "go blue" and then turn red again
				var lastIlshenarVisit = parseInt( pChanging.GetTag( "ilshMurderVisit" ));
				if( lastIlshenarVisit == 0 || (( GetCurrentClock() - lastIlshenarVisit ) / 1000 > timeBetweenMurdererVisit ))
				{
					// Allow them to stay in Ilshenar, since it's more than 30 days since their last visit as a murderer (but update timestamp)
					pChanging.SetTag( "ilshMurderVisit", GetCurrentClock().toString() );
					return false;
				}
			}

			// Otherwise, teleport murderers instantly to Chaos shrine in Felucca
			// Murderers are not allowed in Trammel/Malas, and only limited visitation rights to Ilshenar
			pChanging.Teleport( 1456, 852, 0 );
		}
		else if( newStatus == 0x02 ) // Criminal
		{
			if( EraStringToNum( coreShardEra ) >= EraStringToNum( "lbr" ))
			{
				// Criminal flag is not enabled in Trammel ruleset, as per Publish 15 (LBR), so make sure it can't be set
				pChanging.innocent = true;
			}
		}
	}
	return false;
}

// Handle corpse-looting rules
function onPickup( iPickedUp, pGrabber )
{
	var pSock = pGrabber.socket;
	var worldNum = pGrabber.worldnumber;
	if( regionLootingOverride.indexOf( pGrabber.region.id ) == -1 && facetLootingRestrict.indexOf( pGrabber.worldnumber ) != -1 )
	{
		// Check the value of pSock.pickupSpot to determine where the item was picked up from
		switch( pSock.pickupSpot )
		{
			case 3: // otherpack
				if( iPickedUp.container && ( iPickedUp.container ).corpse )
				{
					var corpseOwner = ( iPickedUp.container ).owner;
					if( ValidateObject( corpseOwner ) && !corpseOwner.npc )
					{
						// If corpse belongs to another player, disallow picking up the item, unless
						// the players belong to the same guild, their guilds are at war, or they
						// are in the same party and have set looting rules to allow party members to
						// loot them
						var guildRelations = CheckGuildRelationShip( pGrabber, corpseOwner );
						if( guildRelations != 1 && guildRelations != 4 )
						{
							// OR if corpse belongs to a criminal or murderer
							if( !corpseOwner.criminal && !corpseOwner.murderer )
							{
								// Neither in same guild nor at war, disallow looting corpse
								pSock.SysMessage( GetDictionaryEntry( 9064, pSock.language )); // You cannot pick that up.
								return false;							
							}
						}
					}
				}
				break;
			default:
				break;
		}
	}

	// Default to hard-coded rules for pickup
	return true;
}

// Players cannot collide with non-locked down items or shove other players in Trammel/Ilshenar
// TODO: How is collision with non-locked down items overridden? Client prevents movement through blocking items...
function onCollide( trgSock, srcChar, trgObj )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( trgObj ))
		return true;

	var worldNum = srcChar.worldnumber;
	if( regionBlockMovementOverride.indexOf( trgObj.region.id ) == -1 && facetBlockMovementRestrict.indexOf( trgObj.worldnumber ) != -1 )
	{
		// Don't let characters block other characters...
		if( trgObj.isChar )
		{
			// ...unless they're in the same guild, or at war!
			if( ValidateObject( trgObj.owner ))
			{
				// Treat target as if it IS its owner
				trgObj = trgObj.owner;
			}

			var guildRelations = CheckGuildRelationShip( srcChar, trgObj );
			if( guildRelations != 1 && guildRelations != 4 )
			{
				return false;
			}
		}
		else if( trgObj.isItem && trgObj.movable == 1 )
		{
			// Don't let items dropped on ground block movement
			return false;
		}
		else if( trgObj.isItem && trgObj.dispellable ) // Wall of Stone
		{
			// Don't let Wall of Stone (or Energy Field) block movement unless players are in same guild or at war
			// TODO: Client seems to automatically block movement through these items though...
			var trgChar = CalcCharFromSer( trgObj.morey );
			if( ValidateObject( trgChar ))
			{
				var guildRelations = CheckGuildRelationShip( srcChar, trgChar );
				if( guildRelations != 1 && guildRelations != 4 )
				{
					return false;
				}
			}
		}
	}

	// Default to hard-coded collision handling
	return true;
}

function FacetRuleExplosionDamage( sourceChar, targetChar )
{
	if( regionPotionOverride.indexOf( targetChar.region.id ) == -1 && facetPotionRestrict.indexOf( targetChar.worldnumber ) != -1 )
	{
		if( ValidateObject( targetChar.owner ))
		{
			// Treat target as if it IS its owner
			targetChar = targetChar.owner;
		}

		if( targetChar.npc && targetChar.innocent )
		{
			// Not a valid target for Explosion Damage
			return false;
		}

		if( !targetChar.npc )
		{
			var guildRelations = CheckGuildRelationShip( sourceChar, targetChar );
			if( guildRelations != 1 )
			{
				// Not at war with target player's guild
				if( sourceChar.socket )
				{
					sourceChar.socket.SysMessage( GetDictionaryEntry( 9230, sourceChar.socket.language )); // You cannot perform negative acts on your target.
				}
				return;
			}
		}
	}

	return true;
}

function FacetRuleBladeSpiritTarget( sourceChar, targetChar )
{
	if( regionBladeSpiritTargetOverride.indexOf( targetChar.region.id ) == -1
		&& facetBladeSpiritTargetRestrict.indexOf( targetChar.worldnumber ) != -1 )
	{
		if( ValidateObject( targetChar.owner ))
		{
			// Treat target as if it IS its owner
			targetChar = targetChar.owner;
		}

		if( targetChar.npc && targetChar.innocent )
		{
			// Not a valid target for Blade Spirit
			return false;
		}

		if( !targetChar.npc )
		{
			var guildRelations = CheckGuildRelationShip( sourceChar, targetChar );
			if( guildRelations != 1 && guildRelations != 4 ) // not at war, nor same guild
			{
				// Not a valid target for Blade Spirit
				return false;
			}
		}
	}

	return true;
}

function FacetRuleEnergyVortexTarget( sourceChar, targetChar )
{
	if( regionEnergyVortexTargetOverride.indexOf( targetChar.region.id ) == -1
		&& facetEnergyVortexTargetRestrict.indexOf( targetChar.worldnumber ) != -1 )
	{
		if( ValidateObject( targetChar.owner ))
		{
			// Treat target as if it IS its owner
			targetChar = targetChar.owner;
		}

		if( targetChar.npc && targetChar.innocent )
		{
			// Not a valid target for Energy Vortex
			return false;
		}

		if( !targetChar.npc )
		{
			var guildRelations = CheckGuildRelationShip( sourceChar, targetChar );
			if( guildRelations != 1 && guildRelations != 4 ) // not at war, nor same guild
			{
				// Not a valid target for Energy Vortex
				return false;
			}
		}
	}

	return true;
}

function FacetRuleBardProvoke( sourceChar, targetChar )
{
	if( regionBardProvokeOverride.indexOf( targetChar.region.id ) == -1
		&& facetBardProvokeRestrict.indexOf( targetChar.worldnumber ) != -1 )
	{
		if( ValidateObject( targetChar.owner ))
		{
			// Treat target as if it IS its owner
			targetChar = targetChar.owner;
		}

		if( targetChar.npc && targetChar.innocent )
		{
			// Not a valid target for provocation
			if( sourceChar.socket )
			{
				sourceChar.socket.SysMessage( GetDictionaryEntry( 9230, sourceChar.socket.language )); // You cannot perform negative acts on your target.
			}
			return false;
		}

		if( !targetChar.npc )
		{
			var guildRelations = CheckGuildRelationShip( sourceChar, targetChar );
			if( guildRelations != 1 && guildRelations != 4 ) // not at war, nor same guild
			{
				// Not a valid target for provocation
				if( sourceChar.socket )
				{
					sourceChar.socket.SysMessage( GetDictionaryEntry( 9230, sourceChar.socket.language )); // You cannot perform negative acts on your target.
				}
				return false;
			}
		}
	}

	return true;
}

function onDamage( targetChar, sourceChar, damageValue, damageType )
{
	// Allow all damage without a source character, or with a non-tame/non-hirling NPC as the source
	if( !ValidateObject( sourceChar ) || ( ValidateObject( sourceChar ) && sourceChar.npc && !ValidateObject( targetChar.owner )))
		return true;

	if( regionSpellOverride.indexOf( targetChar.region.id ) == -1 && facetSpellRestrict.indexOf( targetChar.worldnumber ) != -1 )
	{
		if( ValidateObject( targetChar.owner ))
		{
			// Treat target as if it IS its owner
			targetChar = targetChar.owner;
		}

		var guildRelations = CheckGuildRelationShip( sourceChar, targetChar );
		if( guildRelations != 1 )
		{
			// Not at war with target player's guild
			return false;
		}
	}

	return true;
}

function CheckGuildRelationShip( pAttacker, pDefender )
{
	var attackerGuild = pAttacker;
	var defenderGuild = pDefender;
	if( attackerGuild && defenderGuild )
	{
		// Potential relationships:
		// 	0 - Neutral
		// 	1 - At War
		// 	2 - Allied
		// 	3 - Unknown
		// 	4 - Same
		return CompareGuildByGuild( attackerGuild, defenderGuild );
	}
	return -1; // Either player does not belong to a guild
}

function CheckCorpseCarving( pChar, corpseOwner, iCorpse )
{
	var worldNum = pChar.worldnumber;
	if( regionCorpseCarveOverride.indexOf( pChar.region.id ) == -1 && facetCorpseCarveRestrict.indexOf( pChar.worldnumber ) != -1 )
	{
		if( !corpseOwner.npc )
		{
			// Disallow carving up corpses of players in Trammel/Ilshenar/Malas
			return false;
		}
	}
	else // Felucca, Tokuno?
	{
		if( !corpseOwner.npc && !corpseOwner.murderer && !pChar.murderer )
		{
			// Disallow carving up corpses of non-murderers in Felucca, unless pChar is a murderer
			return false;
		}
	}

	return true;
}

// Dummy function to restore script context after other JS events have triggered and executed
// as part of something happening in this script
function _restorecontext_() {}
