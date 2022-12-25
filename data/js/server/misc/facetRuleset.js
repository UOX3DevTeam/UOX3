const coreShardEra = GetServerSetting( "CoreShardEra" );
const timeBetweenMurdererVisit = 2592000; // 30 days between each time a murderer can visit Ilshenar

// Define which rules apply to which facets
// Example: const facetCorpseCarveRestrict = [ 1, 2, 3, 4 ]; // Corpse carving restricted in Trammel, Ilshenar, Malas, Tokuno
const facetCorpseCarveRestrict = [1, 2, 3, 4, 5];
const facetMurdererRestrict = [1, 2, 3, 4, 5];
const facetCombatRestrict = [1, 2, 3, 4, 5];
const facetStealRestrict = [1, 2, 3, 4, 5];

// Override above facet-specific rules for specific regions
// Example: const regionCorpseCarveOverride = [ 17, 23, 56 ]; // Override corpse carving restrictions in these regions
const regionCorpseCarveOverride = [];
const regionMurdererOverride = [];
const regionCombatOverride = [];
const regionStealOverride = [];

function onCombatStart( pAttacker, pDefender )
{
	var socket = pAttacker.socket;
	var worldNum = pAttacker.worldnumber;
	// so the check would look like this instead:
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
			 		socket.SysMessage( "You cannot perform negative acts on your target." );
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
			socket.SysMessage( "You cannot perform negative acts on your target." );
			return false;			
		}
	}

	// By default, allow hard-coded combat start process to continue
	return true;
}

// Players cannot cast aggressive spells at other players in Trammel/Ilshenar/Malas
function onSpellTarget( myTarget, myTargetType, pCaster, spellID )
{
	var socket = pCaster.socket;
	if( socket != null )
	{
		var worldNum = pCaster.worldnumber;
		if( regionCombatOverride.indexOf( pCaster.region.id ) == -1 && facetCombatRestrict.indexOf( pCaster.worldnumber ) != -1 )
		{
			var spellCast = Spells[spellID];
			if( spellCast.aggressive )
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
					 		socket.SysMessage( "You cannot perform negative acts on your target." );
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
					socket.SysMessage( "You cannot perform negative acts on your target." );
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
						socket.SysMessage( "You cannot perform beneficial acts on your target." );
						return 2;
					}
				}

				// Only allowed between...
				// ...players in same guild / non-guild players and non-guild players
				if( guildRelations == 4 || ( pCaster.guild == null && myTarget.guild == null ))
				{
					// Allow beneficial spellcast
					return false;
				}

				// ...non-guild players and players in guilds not at war with anyone
				/*if(( pCaster.guild == null && myTarget.guild != null ) || ( pCaster.guild != null && myTarget.guild == null ))
				{
					// Currently unable to check if a guild is NOT at war with _anyone_, need additional guild properties/functions to handle this
					// Allow beneficial spellcast
					return false;
				}*/

				// ...players in guilds not at war with anyone and another player in another guild not at war with anyone
				/*if()
				{
					// Currently unable to check if a guild is NOT at war with anyone
					// Allow beneficial spellcast
					return false;
				}*/

				socket.SysMessage( "You cannot perform beneficial acts on your target." );
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
		return true;

	var socket = pThief.socket;
	var worldNum = pThief.worldnumber;
	if( regionStealOverride.indexOf( pThief.region.id ) == -1 && facetStealRestrict.indexOf( pThief.worldnumber ) != -1 )
	{
		socket.SysMessage( "You cannot perform negative acts on your target." );
		return true;
	}
	return false;
}

// Players cannot snoop the backpacks of other players in Trammel/Ilshenar/Malas
function onSnoopAttempt( pSnooped, pSnooping )
{
	if( !ValidateObject( pSnooped ) || !ValidateObject( pSnooping ))
		return true;

	var socket = pSnooping.socket;
	var worldNum = pSnooping.worldnumber;
	if(( regionStealOverride.indexOf( pSnooping.region.id ) == -1 && facetStealRestrict.indexOf( pSnooping.worldnumber ) != -1 ) && !pSnooped.npc )
	{
		socket.SysMessage( "You cannot perform negative acts on your target." );
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
			if( coreShardEra == "pub15" || coreShardEra == "aos" || coreShardEra == "se" || coreShardEra == "ml" || coreShardEra == "sa" || coreShardEra == "hs" || coreShardEra == "tol" )
			{
				// Criminal flag is not enabled in Trammel ruleset, as per Publish 15, so make sure it can't be set
				pChanging.innocent = true;
			}
		}
	}
	return false;
}

function onPickup( iPickedUp, pGrabber )
{
	var pSock = pGrabber.socket;
	var worldNum = pGrabber.worldnumber;
	if( worldNum == 1 || worldNum == 2 || worldNum == 3) // Trammel, Ilshenar or Malas
	{
		//Check the value of pSock.pickupSpot to determine where the item was picked up from
		switch( pSock.pickupSpot )
		{
			case 3: // otherpack
				if( iPickedUp.container && ( iPickedUp.container ).corpse )
				{
					var corpseOwner = ( iPickedUp.container ).corpse.owner;
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
function onCollide( trgSock, srcChar, trgObj )
{
	if( !ValidateObject( srcChar ) || !ValidateObject( trgObj ))
		return true;

	var worldNum = srcChar.worldnumber;
	if( worldNum == 1 || worldNum == 2 || worldNum == 3) // Trammel, Ilshenar or Malas
	{
		// Don't let characters block other characters...
		if( trgObj.isChar )
		{
			// ...unless they're in the same guild, or at war!
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
			var trgChar = trgObj.caster; // need a way to detect who cast the spell
			var guildRelations = CheckGuildRelations( srcChar, trgChar );
			if( guildRelations != 1 && guildRelations != 4 )
			{
				return false;
			}
		}
	}

	// Default to hard-coded collision handling
	return true;
}

function FacetRuleExplosionDamage( sourceChar, targetChar )
{
	if( regionCombatOverride.indexOf( targetChar.region.id ) == -1 && facetCombatRestrict.indexOf( targetChar.worldnumber ) != -1 )
	{
		var guildRelations = CheckGuildRelationShip( sourceChar, targetChar );
		if( guildRelations != 1 )
		{
			// Not at war with target player's guild
			sourceChar.SysMessage( "You cannot perform negative acts on your target. Expl" );
			return;
		}
	}
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

// This event might need to be in global.js, as it contains rules for both trammel-ruleset and felucca-ruleset
function onCarveCorpse( pChar, iCorpse )
{
	var corpseOwner = iCorpse.owner;
	if( ValidateObject( corpseOwner ))
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
			if( !corpseOwner.murderer && !pChar.murderer )
			{
				// Disallow carving up corpses of non-murderers in Felucca, unless pChar is a murderer
				return false;
			}
		}
	}

	// Default to allow carving up corpses
	return false;
}