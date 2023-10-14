function SkillRegistration()
{
	RegisterSkill( 33, true );	// Stealing
}

const coreShardEra = GetServerSetting( "CoreShardEra" );
const youngPlayerSystem = GetServerSetting( "YoungPlayerSystem" );

// If enabled, a thief's dexterity will directly affect the chance of success at stealing
// 	Up to -10% penalty for dex from 50 down to 0
// 	Up to +10% bonus for dex from 50 up to 100
const dexAffectsStealChance = false;

// If enabled, the current light level will directly affect the success chance of stealing
//	Up to -10% penalty for bright light levels (0 to 10)
//	Up to +10% bonus for dark light levels (11 to 20)
const lightLevelAffectsStealChance = false;

const allowStealingContainers = ( EraStringToNum( coreShardEra ) <= EraStringToNum( "lbr" ));
const allowStealingContainersOverride = -1; // -1 = no override, 0 = always false, 1 = always true

// If enabled, freshly traded items between players are safe from being stolen for X milliseconds
const protectTradedItems = true;
const protectTradedItemsDuration = 30; // 30 seconds

// If enabled, stolen items are returned to owner(s) if thief dies within X milliseconds after stealing
const returnStolenItemsOnThiefDeath = false;
const returnStolenItemsTimer = 30 * 1000; // 30 seconds
const returnStolenItemScriptID = 8000;

// If enabled, stolen items cannot be moved from thief's backpack for X milliseconds after stealing
// By default, only enabled for AoS and beyond
const stolenItemsImmovable = ( EraStringToNum( coreShardEra ) >= EraStringToNum( "aos" ));
const stolenItemsImmovableTimer = 30 * 1000; // 30 seconds
const stolenItemsImmovableOverride = -1; // -1 = no override, 0 = always false, 1 = always true

// If enabled, players can steal special consumable loot from monsters
const specialMonsterLoot = ( EraStringToNum( coreShardEra ) >= EraStringToNum( "ml" ));
const specialMonsterLootOverride = -1; // -1 = no override, 0 = always false, 1 = always true

// If enabled, players can steal from town guards just as they can from any other NPC
const stealFromGuardsOverride = false;

function onSkill( pThief, objType, skillUsed )
{
	var pSock = pThief.socket;
	if( pSock )
	{
		if( pThief.frozen )
		{
			pSock.SysMessage( GetDictionaryEntry( 9211, pSock.language )); // You cannot use that while paralyzed.
			return true;
		}
		// At UO launch, stealing was not possible in combat. Did that ever change?
		if( pThief.atWar ) // && EraStringToNum( coreShardEra ) == EraStringToNum( "uo" ))
		{
			pSock.SysMessage( GetDictionaryEntry( 9196, pSock.language )); // You cannot attempt to steal in the heat of combat!
			return true;
		}

		// Since T2A, and New Player Experience, players marked as "young" cannot steal from other players OR NPCs
		if( youngPlayerSystem && pThief.account.isYoung )
		{
			pSock.SysMessage( GetDictionaryEntry( 1997, pSock.language )); // As a Young player, you cannot use the stealing skill
			return true;
		}

		var pRegion = pThief.region;
		if( pRegion )
		{
			if( pRegion.isSafeZone )
			{
				// Player is in a safe zone where no aggressive actions can be taken, disallow
				pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
			}
			else if( GetServerSetting( "RoguesEnabled" ))
			{
				// Reveal the thief as soon as they use the skill
				if( pThief.visible == 1 || pThief.visible == 2 )
				{
					pThief.visible = 0;
				}
				pSock.CustomTarget( 0, GetDictionaryEntry( 863, pSock.language ), 1 ); // Update to "Which item will you attempt to steal?"
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 864, pSock.language )); // Contact your shard operator if you want stealing available.
			}
		}
	}
	return true;
}

function onCallback0( pSock, myTarget )
{
	if( pSock == null )
		return;

	var pThief = pSock.currentChar;
	if( !ValidateObject( pThief ))
		return;

	// Abort if player cancelled the targeting cursor
	var cancelCheck = parseInt( pSock.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( pSock.GetWord( 1 ) || !ValidateObject( myTarget ))
	{
		// No valid dynamic object was targeted
		pSock.SysMessage( GetDictionaryEntry( 1103, pSock.language )); // Not a valid target
		return;
	}

	if( myTarget.isChar || ( myTarget.isItem && myTarget.type == 1 ))
	{
		// Character was targeted, steal random object from their backpack
		// OR - a container was targeted, steal a random object from the container
		StealRandomTarget( pSock, myTarget );
	}
	else
	{
		// Specific item was targeted, attempt to steal it
		StealSpecificTarget( pSock, myTarget );
	}
}

function StealRandomTarget( pSock, myTarget )
{
	var pThief = pSock.currentChar;
	if( !ValidateObject( pThief ))
		return;

	var itemToSteal = null;
	var rootCont = null;
	var itemOwner = null;
	if( myTarget.isItem )
	{
		rootCont = FindRootContainer( myTarget, 0 );
		if( !ValidateObject( rootCont ))
		{
			if( myTarget.type == 1 )
			{
				// Container is its own root container
				rootCont = myTarget;
			}
			else
			{
				return;
			}
		}

		itemOwner = GetPackOwner( rootCont, 0 );
		if( ValidateObject( itemOwner ) && itemOwner.isChar && myTarget.container == itemOwner ) // Equipped on character's paperdoll
		{
			pSock.SysMessage( GetDictionaryEntry( 9198, pSock.language )); // You cannot steal items which are equipped.
			return;
		}
		else if( rootCont.movable == 3 )
		{
			var objMulti = rootCont.multi;
			if( ValidateObject( objMulti ))
			{
				if( objMulti.IsSecureContainer( rootCont ))
				{
					pSock.SysMessage( GetDictionaryEntry( 9285, pSock.language )); // You cannot steal from a secure container.
					return;
				}
			}
		}
		else if( allowStealingContainersOverride == 1 ||
			( allowStealingContainersOverride == -1 && allowStealingContainers ))
		{
			// Prior to AoS, entire containers could be stolen at once, depending on weight
			//		Players used to train stealing on packhorses by filling small pouches with items to get the "right" weight to attempt to steal
			// After AoS, containers cannot be stolen
			//		Players no longer train stealing via pouches, but have to steal items of specific weights
			if( ValidateObject( itemOwner ) && rootCont != myTarget && rootCont != itemOwner.pack )
			{
				// Player is attempting to steal an entire container from within target's backpack
				itemToSteal = myTarget;
			}
		}
	}
	else
	{
		// Make sure target char has a backpack to steal from
		if( !ValidateObject( myTarget.pack ))
		{
			pSock.SysMessage( GetDictionaryEntry( 875, pSock.language )); // Bad luck, your victim doesn't have a backpack.
			return;
		}

		if( myTarget.tamed && ValidateObject( myTarget.owner ) && myTarget.owner == pThief )
		{
			// Trying to steal from own pet!
			if( myTarget.sectionID != "packhorse" && myTarget.sectionID != "packllama" && myTarget.sectionID != "giantbeetle" )
			{
				pSock.SysMessage( GetDictionaryEntry( 9193, pSock.language )); // You can't steal from this.
				return;
			}
		}
		else if( !myTarget.isAnimal && !myTarget.isHuman && !myTarget.tamed )
		{
			// Steal special loot from monster
			if( specialMonsterLootOverride == 1 || ( specialMonsterLootOverride == -1 && !myTarget.GetTag( "alreadyStolenFrom" ) && specialMonsterLoot ))
			{
				var monsterLoot = StealFromMonster( pSock, pThief, myTarget );
				if( ValidateObject( monsterLoot ))
				{
					itemToSteal = monsterLoot;
				}
			}
		}
		else if( myTarget.isAnimal && !myTarget.tamed )
		{
			// Don't allow stealing from wild animals
			pSock.SysMessage( GetDictionaryEntry( 9193, pSock.language )); // You can't steal from this.
			return;
		}

		itemOwner = myTarget;
	}

	// TODO: As long as item weighs below 10 stones, player can attempt to steal with any skill, but if noticed by nearby NPCs,
	// player will go grey and guards will be called by those NPCs
	if( itemOwner == null && rootCont == myTarget )
	{
		// Stealing from containers in town not possible before Publish 5, UOR
		if( EraStringToNum( coreShardEra ) < EraStringToNum( "uor" ))
		{
			pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that.
			return;
		}
		else if( EraStringToNum( coreShardEra ) >= EraStringToNum( "aos" ))
		{
			// After ... pub16?, players can only steal from containers in guarded regions in Felucca - Trammel is off limits
			if( rootCont.worldnumber != 0 )
			{
				pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that.
				return;
			}
		}
		else
		{
			itemOwner = myTarget;
		}
	}

	// Perform checks for basic rules for stealing skill
	if( !CheckGeneralStealingRules( pSock, pThief, itemOwner ))
	{
		if( ValidateObject( itemToSteal ) && itemToSteal.GetTempTag( "specialLoot" ))
		{
			CleanupSpecialItem( itemToSteal );
		}
		return;
	}

	// Select an item at random to steal from target's pack/targeted container
	if( itemToSteal == null )
	{
		var targetCont = myTarget.isChar ? myTarget.pack : myTarget;
		var validItemList = [];

		if( targetCont.totalItemCount > 0 )
		{
			for( var potentialItem = targetCont.FirstItem(); !targetCont.FinishedItems(); potentialItem = targetCont.NextItem() )
			{
				if( !ValidateObject( potentialItem ))
					continue;

				// Only items in root of target container can be attempted stolen
				if( potentialItem.container != targetCont )
					continue;

				// Add item to list of valid items to steal from
				validItemList.push( potentialItem );
			}
		}

		if( validItemList.length > 0 )
		{
			// Select a random item from the valid list of items to attempt to steal
			itemToSteal = validItemList[RandomNumber( 0, validItemList.length - 1 )];
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 9188, pSock.language )); // You reach into the backpack... but find it's empty.
			return;
		}
	}

	// All good, let the stealing attempt commence!
	DoStealing( pSock, myTarget, itemToSteal, true );
}

function StealSpecificTarget( pSock, myTarget )
{
	var pThief = pSock.currentChar;
	if( !ValidateObject( pThief ))
		return;

	if( myTarget.movable != 2 && myTarget.stealable != 2 )
	{
		// If item is not in a container, no need to steal it. Just pick it up normally?
		if( myTarget.container == null )
			return;

		// Don't allow stealing items in secure trade window, or from a player's bank box
		if( myTarget.container.type == 20 || myTarget.container.layer == 0x1D ) // type 20 = IT_TRADEWINDOW item type, layer 0x1D == Bank box
		{
			pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that
			return;
		}

		// Don't allow stealing from NPC shopkeeper bags
		if( myTarget.container.layer == 0x1A || myTarget.container.layer == 0x1B || myTarget.container.layer == 0x1C )
		{
			pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that
			return;
		}

		var rootCont = FindRootContainer( myTarget, 0 );
		if( !ValidateObject( rootCont ))
		{
			return;
		}

		// Don't allow stealing from secure containers
		if( rootCont.movable == 3 )
		{
			var objMulti = rootCont.multi;
			if( ValidateObject( objMulti ))
			{
				if( objMulti.IsSecureContainer( rootCont ))
				{
					pSock.SysMessage( GetDictionaryEntry( 9285, pSock.language )); // You cannot steal from a secure container.
					return;
				}
			}
		}

		var itemOwner = GetPackOwner( rootCont, 0 );
		if( myTarget.container == itemOwner )
		{
			// Don't allow stealing items off of someone's paperdoll
			pSock.SysMessage( GetDictionaryEntry( 9198, pSock.language )); // You cannot steal items which are equipped.
			return;
		}

		if( !ValidateObject( itemOwner ))
		{
			itemOwner = rootCont; // itemOwner is a container
		}
	}
	else
	{
		// Item is it's own owner
		itemOwner = myTarget;
	}

	// Perform checks for basic rules for stealing skill
	if( !CheckGeneralStealingRules( pSock, pThief, itemOwner ))
	{
		return;
	}

	// All good, let the stealing attempt commence!
	DoStealing( pSock, itemOwner, myTarget, false );
}

function StealFromMonster( pSock, pThief, myTarget )
{
	// Since Publish 57, players can steal a wider variety of items from monsters

	// Drop rate of rare items depends on player's stealing skill
	var stealSkill = pThief.skills.stealing;
	var rareDropRate = 0;
	if( stealSkill == 1000 )
	{
		rareDropRate = 2;
	}
	else if( stealSkill >= 1001 && stealSkill <= 1100 )
	{
		rareDropRate = 5;
	}
	else if( stealSkill >= 1101 && stealSkill <= 1199 )
	{
		rareDropRate = 8;
	}
	else if( stealSkill == 1200 )
	{
		rareDropRate = 10;
	}

	// A bonus to the rare drop rate is provided based on the monster's fame level,
	// scaled from 1 to 40 based on total fame points:
	// 0% below fame level 15, 1% below 25, 2% below 35, 3% below 40, and 5% at 40
	var monsterFameLevel = myTarget.fame / 750;
	var stealBonus = ( monsterFameLvl <= 15 ) ? 0 : ( monsterFameLvl <= 25 ) ? 1 : ( monsterFameLvl <= 35 ) ? 2 : ( monsterFameLvl < 40 ) ? 3 : 5;
	rareDropRate += stealBonus;

	var monsterLoot = null;
	var stealRare = rareDropRate > RandomNumber( 0, 100 );
	if( stealRare )
	{
		// Thieves with GM or higher skill have a chance to steal some rare items:
		// Seed Of Life, Mana Draught, Gem Of Salvation, Balm Of Strength, Balm Of Wisdom, Balm Of Swiftness, Balm Of Protection, Stone Skin Lotion and Life Shield Lotion.
		var rareItem = null;
		var rndRare = RandomNumber( 1, 9 );
		switch( rndRare )
		{
			case 1: // Seed of Life
				monsterLoot = CreateDFNItem( null, pThief, "seedoflife", 1, "ITEM", false );
				break;
			case 2: // Mana Draught
				monsterLoot = CreateDFNItem( null, pThief, "manadraught", 1, "ITEM", false );
				break;
			case 3: // Gem of Salvation
				monsterLoot = CreateDFNItem( null, pThief, "gemofsalvation", 1, "ITEM", false );
				break;
			case 4: // Balm of Strength
				monsterLoot = CreateDFNItem( null, pThief, "balmofstrength", 1, "ITEM", false );
				break;
			case 5: // Balm of Wisdom
				monsterLoot = CreateDFNItem( null, pThief, "balmofwisdom", 1, "ITEM", false );
				break;
			case 6: // Balm of Swiftness
				monsterLoot = CreateDFNItem( null, pThief, "balmofswiftness", 1, "ITEM", false );
				break;
			case 7: // Balm of Protection
				monsterLoot = CreateDFNItem( null, pThief, "balmofprotection", 1, "ITEM", false );
				break;
			case 8: // Stone Skin Lotion
				monsterLoot = CreateDFNItem( null, pThief, "stoneskinlotion", 1, "ITEM", false );
				break;
			case 9: // Life Shield Lotion
				monsterLoot = CreateDFNItem( null, pThief, "lifeshieldlotion", 1, "ITEM", false );
				break;
			default:
				break;
		}

		if( ValidateObject( monsterLoot ))
		{
			monsterLoot.SetTempTag( "specialLoot", true );
		}
	}

	return monsterLoot;
}

function CheckGeneralStealingRules( pSock, pThief, thiefTarget )
{
	if( !ValidateObject( pThief ) || !ValidateObject( thiefTarget ))
		return false;

	if( pThief == thiefTarget )
	{
		pSock.SysMessage( GetDictionaryEntry( 873, pSock.language )); // You catch yourself red handed.
		return;
	}

	// Both hands must be free to steal
	if( ValidateObject( pThief.FindItemLayer( 0x01 )) || ValidateObject( pThief.FindItemLayer( 0x02 )))
	{
		pSock.SysMessage( GetDictionaryEntry( 9189, pSock.language )); // Both hands must be free to steal.
		return;
	}

	// Check for Line of Sight to target
	if( !pThief.CanSee( thiefTarget ))
	{
		pSock.SysMessage( GetDictionaryEntry( 1646, pSock.language )); // You cannot see that
		return;
	}

	// Check if stealing is disallowed in either thief or potential target's regions
	if( pThief.region.isSafeZone || thiefTarget.region.isSafeZone )
	{
		// Target is in a safe zone where all aggressive actions are forbidden, disallow
		pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
		return false;
	}

	if( thiefTarget.isChar )
	{
		if( !thiefTarget.npc )
		{
			// Victim is another player
			if( pThief.npcGuild != 3 ) // Not a member of thieves guild?
			{
				// In T2A expansion, players can steal from guild enemies without being in thieves guild
				if( EraStringToNum( coreShardEra ) == EraStringToNum( "t2a") && TriggerEvent( 2508, "CheckGuildRelationShip", pThief, thiefTarget ) != 1 )
				{
					pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that
					return false;
				}
				else
				{
					// This came into effect in Publish 2, UOR, which superseded an earlier change from Thief Revamp in Publish released on Feb 24, 1999,
					// which said players could only steal from opponents in a guild war if not in NPC thieves guild
					pSock.SysMessage( GetDictionaryEntry( 9190, pSock.language )); // You must be in the thieves guild to steal from other players.
					return false;
				}
			}
			else if( pThief.murderer )
			{
				pSock.SysMessage( GetDictionaryEntry( 9191, pSock.language )); // You are currently suspended from the thieves guild.
				return false;
			}
		}
		else
		{
			// Victim is an NPC
			if( thiefTarget.isShop && thiefTarget.vulnerable ) // Shopkeeper
			{
				pSock.SysMessage( GetDictionaryEntry( 9192, pSock.language )); // You cannot steal from shopkeepers.
				return false;
			}
			else if( thiefTarget.aitype == 17 ) // Player Vendor
			{
				pSock.SysMessage( GetDictionaryEntry( 9199, pSock.language )); // You cannot steal from vendors.
				return false;
			}
			else if( thiefTarget.isDispellable )
			{
				pSock.SysMessage( GetDictionaryEntry( 9193, pSock.language )); // You can't steal from this.
				return false;
			}
			else if( !stealFromGuardsOverride && ( thiefTarget.npcai == 4 && EraStringToNum( coreShardEra ) >= EraStringToNum( "aos" )))
			{
				pSock.SysMessage( GetDictionaryEntry( 9200, pSock.language )); // You cannot steal from guards.
				return false;
			}
			else if( !thiefTarget.tamed && thiefTarget.GetTag( "alreadyStolenFrom" ))
			{
				pSock.SysMessage( GetDictionaryEntry( 9194, pSock.language )); // That creature has already been stolen from.  There is nothing left to steal.
				return false;
			}
		}

		// Don't allow stealing from someone with higher command level access than you
		if( pThief.commandlevel < thiefTarget.commandlevel )
		{
			pSock.SysMessage( GetDictionaryEntry( 878, pSock.language )); // You can't steal from gods.
			return;
		}

		// In AoS and beyond, players can only steal gold and gems from innocent players in Dungeon/LL areas of Felucca
		if( thiefTarget.worldnumber == 0 && thiefTarget.innocent && !thiefTarget.npc )
		{
			var cRegion = thiefTarget.region;
			if( cRegion.isDungeon || ( thiefTarget.x >= 5119 && thiefTarget.x <= 6143 && thiefTarget.y >= 2304 && thiefTarget.y <= 4096 )) // In Dungeons/Lost Lands area
			{
				if( EraStringToNum( coreShardEra ) >= EraStringToNum( "aos" ))
				{
					// Only gold and gems can be stolen in Dungeon/LL areas of Felucca...
					if( itemToSteal.id != 0x0eed && ( itemToSteal.id < 0x0f0f || itemToSteal.id > 0x0f30 ))
					{
						// ...if thief and victim NOT in guilds at war with each other
						if( TriggerEvent( 2508, "CheckGuildRelationShip", pThief, thiefTarget ) != 1 )
						{
							pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that
							return;
						}
					}
				}
			}
		}
	}

	// Don't allow stealing from a character in a different instance!
	if( thiefTarget.instanceID != pThief.instanceID )
	{
		pSock.SysMessage( GetDictionaryEntry( 9201, pSock.language )); // You cannot steal what cannot be seen...
		return false;
	}

	// Don't allow stealing from non-visible targets
	if( thiefTarget.visible != 0 )
	{
		pSock.SysMessage( GetDictionaryEntry( 9201, pSock.language )); // You cannot steal what cannot be seen...
		return false;
	}

	// Don't allow stealing if too far away from target/owner
	if( !pThief.InRange( thiefTarget, 1 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 886, pSock.language )); // You are too far away to steal that item.
		return;
	}

	if( thiefTarget.isItem )
	{
		// Don't allow stealing non-stealable items, or items held on cursor by another player
		if( !thiefTarget.stealable || thiefTarget.isItemHeld )
		{
			pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that.
			return;
		}

		// Preparation for Faction system
		/*if( thiefTarget.GetTag( "townSigil" ))
		{
			// Publish 8, UO:R
			// Thieves with 80+ stealing skill may steal town sigils
			// Thief must also be in a faction

			// Publish 86, UO:HS
			// Vice vs Virtue - Players must use detecting hidden and have at least 100.0% stealing to reveal and steal the sigil.

			if( pThief.factionID == 0 )
			{
				pSock.SysMessage( GetDictionaryEntry( 9202, pSock.language )); // You must join a faction to do this
				return;
			}
			else if( pThief.factionID == thiefTarget.factionID )
			{
				pSock.SysMessage( GetDictionaryEntry( 9203, pSock.language )); // You cannot steal your own sigil
				return;
			}
			else if( pThief.skills.stealing < 800 )
			{
				pSock.SysMessage( GetDictionaryEntry( 9204, pSock.language )); // You do not have enough skill to steal the sigil
				return;
			}
			else if( pThief.isIncognito )
			{
				pSock.SysMessage( GetDictionaryEntry( 9205, pSock.language )); // You cannot steal the sigil when you are incognito
				return;
			}
			else if( pThief.isPolymorphed )
			{
				pSock.SysMessage( GetDictionaryEntry( 9206, pSock.language )); // You cannot steal the sigil while polymorphed
				return;
			}
			else if( pThief.isDisguised )
			{
				pSock.SysMessage( GetDictionaryEntry( 9207, pSock.language )); // You cannot steal the sigil while disguised
				return;
			}
			else if( thiefTarget.GetTag( "stealRestrict" ))
			{
				pSock.SysMessage( GetDictionaryEntry( 9208, pSock.language )); // ??? Cannot steal town sigil within one hour after server startup
				return;
			}
		}*/
	}

	// All checks passed, good to go
	return true;
}

function DoStealing( pSock, itemOwner, itemToSteal, randomItem )
{
	var pThief = pSock.currentChar;
	if( !ValidateObject( pThief ))
		return;

	var itemCont = itemToSteal.container;
	if( !ValidateObject( itemCont ) && itemToSteal.stealable != 2 )
	{
		if( itemToSteal.GetTempTag( "specialLoot" ))
		{
			CleanupSpecialItem( itemToSteal );
		}
		return;
	}

	// Trigger onSteal JS event for stolen item
	var retVal = 0;
	var iScriptTriggers = itemToSteal.scriptTriggers;
	for( var i = 0; i < iScriptTriggers.length; i++ )
	{
		if( DoesEventExist( iScriptTriggers[i], "onSteal" ))
		{
			retVal = TriggerEvent( iScriptTriggers[i], "onSteal", pThief, itemToSteal, itemOwner );
			switch( retVal )
			{
				case 1: // Item was not stolen, and we don't want to continue running this script
					return;
				case 2: // Item was stolen, and we don't want to continue running this script
					{
						if( ValidateObject( itemOwner ) && itemOwner.isChar )
						{
							var ioScriptTriggers = itemOwner.scriptTriggers;
							for( var j = 0; j < ioScriptTriggers.length; j++ )
							{
								if( DoesEventExist( ioScriptTriggers[j], "onStolenFrom" ))
								{
									if( TriggerEvent( ioScriptTriggers[j], "onStolenFrom", pThief, itemOwner, itemToSteal ) == 1 )
									{
										break;
									}
								}
							}
						}
					}
					return;
				default:
					break;
			}
		}
	}

	// Trigger onSteal event for player doing the stealing
	var pScriptTriggers = pThief.scriptTriggers;
	for( var l = 0; l < pScriptTriggers.length; l++ )
	{
		if( DoesEventExist( pScriptTriggers[l], "onSteal" ))
		{
			retVal = TriggerEvent( pScriptTriggers[l], "onSteal", pThief, itemToSteal, itemOwner );
			switch( parseInt( retVal ))
			{
				case 1: // Item was not stolen, and we don't want to continue running this script
					return;
				case 2: // Item was stolen, and we don't want to continue running this script
					{
						if( ValidateObject( itemOwner ) && itemOwner.isChar )
						{
							var io2ScriptTriggers = itemOwner.scriptTriggers;
							for( var m = 0; m < io2ScriptTriggers.length; m++ )
							{
								if( DoesEventExist( io2ScriptTriggers[m], "onStolenFrom" ))
								{
									if( TriggerEvent( io2ScriptTriggers[m], "onStolenFrom", pThief, itemOwner, itemToSteal ) == 1 )
									{
										break;
									}
								}
							}
						}
					}
					return;
				default:
					break;
			}
		}
	}

	// Disallow stealing items that are freshly protected after being in secure trade window
	if( protectTradedItems )
	{
		// Check timestamp for when item was last traded
		if( itemToSteal.tempLastTraded > 0 && Math.floor( GetCurrentClock() / 1000 ) - itemToSteal.tempLastTraded < protectTradedItemsDuration )
		{
			pSock.SysMessage( GetDictionaryEntry( 9212, pSock.language )); // That item is temporarily protected from theft
			return;
		}
	}


	// Disallow stealing if player is wearing too much armor
	if( pThief.Defense( 0, 1, false ) > 40 )
	{
		if( itemToSteal.GetTempTag( "specialLoot" ))
		{
			CleanupSpecialItem( itemToSteal );
		}
		pSock.SysMessage( GetDictionaryEntry( 1643, pSock.language )); // Your armour is too heavy to do that
		return;
	}

	// Don't allow stealing locked down/immovable items
	// Don't allow stealing spellbooks
	// Don't allow stealing deeds
	// Don't allow stealing BOD books (Publish 19)
	if( itemToSteal.movable == 3 || itemToSteal.type == 9 || itemToSteal.id == 0x14f0 ) // TODO: || itemToSteal.type == bodBookType )
	{
		pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that.
		return;
	}

	var itemOwnerOwner = null;
	if( ValidateObject( itemOwner ))
	{
		if( itemOwner.isChar )
		{
			if( youngPlayerSystem && !itemOwner.npc && itemOwner.account.isYoung )
			{
				pSock.SysMessage( GetDictionaryEntry( 9209, pSock.language )); // You cannot steal from the Young.
				return;
			}

			// Explode orcish masks if stealing from an orc while wearing one (Since Publish 11, UO:TD)
			if( itemOwner.race == 22 && pThief.FindItemLayer( 0x06 ))
			{
				if( EraStringToNum( coreShardEra ) >= EraStringToNum( "td" ))
				{
					TriggerEvent( 3207, "ExplodeOrcishMask", pThief );
				}
			}
			else if( ValidateObject( itemOwner.owner ))
			{
				// itemOwner is owned by someone! Interesting
				itemOwnerOwner = itemOwner.owner;
			}
		}
		else // itemOwner is an item
		{
			// Don't allow stealing from town chests if item is not marked as a rare item (stealable = 2)
			if( itemOwner.type == 1 && itemOwner.movable == 2 && itemToSteal.stealable != 2 )
			{
				pSock.SysMessage( GetDictionaryEntry( 874, pSock.language )); // You cannot steal that.
				return;
			}
		}
	}

	// Don't allow stealing newbie items
	if( itemToSteal.newbie )
	{
		pSock.SysMessage( GetDictionaryEntry( 879, pSock.language )); // That item has no value to you.
		return;
	}

	if( ValidateObject( itemOwner ) && itemOwner.isChar )
	{
		var ownerName = itemOwner.name;
		var dictMsg = GetDictionaryEntry( 877, pSock.language ); // You reach into %'s pack and try to take something...
		pSock.SysMessage( dictMsg.replace( /%s/gi, ownerName ));
	}

	var amountToSteal = 1;
	if( !itemToSteal.isPileable )
	{
		// If item weighs more than thief is capable of stealing, disallow attempt entirely
		if( itemToSteal.weight > pThief.skills.stealing )
		{
			pSock.SysMessage( GetDictionaryEntry( 1551, pSock.language )); // That is too heavy
			return;
		}
	}
	else
	{
		// What about stacks of items, like piles of gold? How many do we steal?
		if( itemToSteal.amount > 1 )
		{
			// Player can max steal ( Stealing skill / 10 ) / individual weight of item in pile
			var maxAmount = Math.round(( pThief.skills.stealing / 100 ) / ( itemToSteal.weight / 100 )); // TODO: Double check this...
			if( maxAmount < 1 )
			{
				maxAmount = 1;
			}
			else if( maxAmount > itemToSteal.amount )
			{
				maxAmount = itemToSteal.amount;
			}

			amountToSteal = RandomNumber( Math.round( maxAmount / 2 ), maxAmount );
		}
	}

	// Regardless of whether thief was caught or not, flag them for stealing
	// The stealing flag will last however long the UOX.INI setting STEALINGFLAGTIMER says - 2 minutes by default
	// However, do it from a timer, since setting the flag will trigger another event (onFlagChange) which if present might
	// mess with the rest of the execution of this script
	// UNLESS they are stealing from their own pet/packhorse/packllama!
	var theftWitnessed = 0;
	if( itemOwnerOwner != pThief )
	{
		// Prepare messages to show if player gets noticed while stealing
		var itemName = itemToSteal.name;
		if( itemToSteal.name == "#" )
		{
			// Get name from tiledata instead
			itemName = itemToSteal.GetTileName();
		}

		// Message for target of theft, if they notice
		var targetMsg = GetDictionaryEntry( 884, pSock.language ); // You notice %s trying to steal %s from you!
		targetMsg = targetMsg.replace( /%s/gi, pThief.name );
		targetMsg = targetMsg.replace( /%w/gi, itemName );

		// Message for player witnesses, if they notice
		var publicMsg = GetDictionaryEntry( 885, pSock.language ); // You notice %s trying to steal %w from %d!
		publicMsg = publicMsg.replace( /%s/gi, pThief.name );
		publicMsg = publicMsg.replace( /%w/gi, itemName );
		publicMsg = publicMsg.replace( /%d/gi, itemOwner.name );

		// Store temporary messages on socket
		pSock.xText = targetMsg;
		pSock.xText2 = publicMsg;
		pSock.tempInt2 = itemOwner.serial;

		pThief.hasStolen = true;

		// See how many nearby characters detect the theft
		theftWitnessed = AreaCharacterFunction( "FindNearbyPlayers", pThief, 7, pSock );

		pSock.xText = null;
		pSock.tempInt2 = null;
	}

	// Check if item can be stolen based on item type, weight vs stealing skill, then return min skill needed to steal the item
	var stealChance = CalcStealChance( pThief, itemToSteal, randomItem, amountToSteal, theftWitnessed );
	if( stealChance == -1 )
	{
		pSock.SysMessage( GetDictionaryEntry( 1551, pSock.language )); // That is too heavy
		return;
	}

	// Lower player's karma, but only down to -4 tier
	var karmaChange = 0;
	var karmaLossThreshold = -4000;
	var curKarma = pThief.karma;
	if( curKarma > karmaLossThreshold )
	{
		karmaChange = Math.abs( Math.round(( curKarma - karmaLossThreshold ) / 50 ));
		if( EraStringToNum( coreShardEra ) >= EraStringToNum( "lbr" ))
		{
			// Double karma loss in consensual PvP rule set areas since Publish 15
			karmaChange *= 2;
		}
		pThief.karma -= karmaChange;

		if( karmaChange != 0 )
		{
			if( karmaChange <= 25 )
			{
				pSock.SysMessage( GetDictionaryEntry( 1368, pSock.language )); // You have lost a little karma.
			}
			else if( karmaChange <= 50 )
			{
				pSock.SysMessage( GetDictionaryEntry( 1370, pSock.language )); // You have lost some karma.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1372, pSock.language )); // You have lost a lot of karma.
			}
		}
	}

	// Prior to AoS, thieves are marked as aggressors upon stealing from other players:
	if( EraStringToNum( coreShardEra ) < EraStringToNum( "aos" ))
	{
		if( ValidateObject( itemOwner ) && itemOwner != pThief )
		{
			// Don't mark player as aggressor if he's the owner of the itemOwner, though!
			if( !ValidateObject( itemOwnerOwner ) || itemOwnerOwner != pThief )
			{
				pThief.AddAggressorFlag( itemOwner );
			}
		}
	}

	// After a stealing attempt, a thief must wait 10 seconds before they can use another skill
	pThief.SetTimer( Timer.SOCK_SKILLDELAY, 10000 );

	// Make it harder to steal the more armor the thief is wearing - since T2A
	// Reduce chance of stealing item by up to 97.5% depending on how much armor the thief wears
	if( EraStringToNum( coreShardEra ) >= EraStringToNum( "t2a" ))
	{
		var defenseImpact = Math.min( 0.975, Math.max( 0, ( pThief.Defense( 0, 1, false ) - 1 ) / 40 ));
		stealChance = Math.floor( stealChance * ( 1 - defenseImpact ));
	}

	// Max weight that can be stolen, per era:
	// LBR to 2010: skill / 10 determines absolute max amount of stones that can be stolen
	// Nov 2011 to CURRENT DAY: skill / 10, but now they are recommended weights for training, rather than absolute fail/pass, up to 120/10 = 12 stones
	// 	Quote Stratics: "It is possible to steal weights higher than the recommended trainning weight by skill, chances of success are very low. If Random Stealing
	//		(rather than Direct Stealing) is used, then the  thief’s ability to steal high-stoned items successfully is greatly increased."
	//

	// Gaining stealing skill is dependent on skill vs weight of item being stolen;
	// The more the item weighs, the higher the player's skill needs to be to pass
	// the skill-check. But gaining skill also requires upgrading to more heavy items
	// as the player's skill increases!
	// Example: An item that weighs 1 stones (100) will let player gain up to 20.0 skill, but requires at least 10.0 skill to gain anything
	// Example: An item that weighs 2 stones (200) will let player gain up to 30.0 skill, but requires at least 20.0 skill to gain anything
	// Example: An item that weighs 4 stones (400) will let player gain up to 50.0 skill, but requires at least 30.0 skill to gain anything
	// Example: An item that weighs 8 stones (800) will let player gain up to 90.0 skill, but requires at least 80.0 skill to gain anything
	// Example: An item that weighs 10 stones (1000) will let player gain up to 110.0 skill, but requires at least 1000.0 skill to gain anything
	// HOWEVER, only allow skill checks when not stealing same item twice in a row
	if( parseInt( pThief.GetTempTag( "lastStealSerial" )) != itemToSteal.serial )
	{
		var totalWeight = itemToSteal.weight * amountToSteal;
		pThief.CheckSkill( 33, totalWeight, totalWeight + 100 );
	}

	// Based on the stealChance we calculated earlier, do a simple check to see if player succeeds at stealing the item
	if( stealChance >= RandomNumber( 0, 100 ))
	{
		// Success!
		if( theftWitnessed )
		{
			// Criminal flag + permagrey flag
			pSock.SysMessage( GetDictionaryEntry( 882, pSock.language )); // You have been caught!

			// If item has no owner, but it's inside a guarded zone, criminal flag
			if( !ValidateObject( itemOwner ))
			{
				if( itemToSteal.region.isGuarded )
				{
					pThief.criminal = true;
				}
			}
			else if( itemOwner.region.isGuarded )
			{
				// If itemOwner exists, is an item, and is inside a guarded zone, criminal flag
				if( itemOwner.isItem )
				{
					pThief.criminal = true;
				}
				else
				{
					// If this action would lead to a criminal flag... criminal flag!
					if( WillResultInCriminal( pThief, itemOwner ))
					{
						pThief.criminal = true;
					}

					// If era is before Pub16, also add a permagrey flag,
					// that turns thief grey to target until thief dies, and remains a
					// criminal "behind the scenes" for everyone else (though they still see the
					// thief as blue)
					if( EraStringToNum( coreShardEra ) < EraStringToNum( "aos" ))
					{
						pThief.AddPermaGreyFlag( itemOwner );
					}
				}
			}
		}
		else
		{
			// Permagrey flag only
			// If target was innocent, add permagrey flag to thief (before Pub16 only)
			// If there's no valid itemOwner, no permagrey flag added
			if( EraStringToNum( coreShardEra ) < EraStringToNum( "aos" ))
			{
				if( ValidateObject( itemOwner ) && itemOwner.isChar && itemOwner.innocent )
				{
					if( !ValidateObject( itemOwnerOwner ) || itemOwnerOwner != pThief )
					{
						pThief.AddPermaGreyFlag( itemOwner );
					}
				}
			}
		}

		// Can player fit the item they're trying to steal in their own backpack?
		if( pThief.pack.totalItemCount >= pThief.pack.maxItems )
		{
			// Backpack has too many items already!
			pSock.SysMessage( GetDictionaryEntry( 1819, pSock.language )); // Your backpack cannot hold any more items!
			CleanupSpecialItem( itemToSteal );
		}
	   	else if( pThief.pack.weight >= pThief.pack.weightMax || pThief.pack.weight + itemToSteal.weight > pThief.pack.weightMax )
		{
			// Backpack cannot hold any more weight!
	   		pSock.SysMessage( GetDictionaryEntry( 1936, pSock.language )); // That container cannot hold any more weight!
			CleanupSpecialItem( itemToSteal );
		}
		else
		{
			var itemStolen = null;
			if( amountToSteal == itemToSteal.amount )
			{
				// Player steals entire item (or entire pile of items, if a pile)
				itemToSteal.container = pThief.pack;
				itemToSteal.PlaceInPack();
				itemStolen = itemToSteal;
				itemStolen.movable = 1;
			}
			else
			{
				// Stealing a portion of a stack of items
				itemStolen = CreateDFNItem( null, null, itemToSteal.sectionID, amountToSteal, "ITEM", false );
				if( !ValidateObject( itemStolen ))
				{
					// Something went wrong!
					CleanupSpecialItem( itemToSteal );
					return;
				}

				// Temporarily store serial of last item that was stolen
				pThief.SetTempTag( "lastStealSerial", ( itemStolen.serial ).toString() );
				pThief.SetTempTag( "lastStealAmount", ( itemStolen.amount ).toString() );

				itemStolen.container = pThief.pack;
				itemStolen.PlaceInPack();

				// Store a temporary timestamp for when item was stolen
				itemStolen.SetTempTag( "stolenAtTime", Math.floor( GetCurrentClock() / 1000 ));

				// Store a reference to the original owner, and timestamp for when item was stolen
				if( ValidateObject( itemOwner ))
				{
					itemStolen.SetTempTag( "stolenItemOwner", ( itemOwner.serial ).toString() );
					if( !itemOwner.isHuman && !itemOwner.tamed )
					{
						// Mark monster as already stolen from
						itemOwner.SetTag( "alreadyStolenFrom", true );
					}
				}

				// After Publish 16, items recently stolen are temporary "blessed" for two minutes
				// They will stay with the thief if killed, but CAN still be stolen from the thief
				if( stolenItemsImmovableOverride == 1 ||
					( stolenItemsImmovableOverride == -1 && ( stolenItemsImmovable && ( !ValidateObject( itemOwnerOwner ) || itemOwnerOwner != pThief ))))
				{
					// Mark item as newbie/blessed, and make it immovable
					itemStolen.newbie = true;
					itemStolen.movable = 3;

					// Start timer to return item status to non-blessed in 2 minutes
					itemStolen.StartTimer( stolenItemsImmovableTimer, 1, true );
				}
				else
				{
					itemStolen.movable = 1;
				}

				// TODO: Based on era, stolen items become immovable in thief's inventory for X amount of time,
				// and if thief dies while carrying the stolen item within that time, item is returned to owner
				// This would not apply to stacks of items (what to do if player removes items from stack), nor
				// to containers (if entire container was stolen, since you can't know if it still contains same items as before)
				// Mark stolen item to be returned if thief dies within 30 seconds?
				if( returnStolenItemsOnThiefDeath && !itemStolen.isPileable && itemStolen.type != 1 && ValidateObject( itemOwner ) && itemOwner.isChar )
				{
					if( !ValidateObject( itemOwnerOwner ) || itemOwnerOwner != pThief )
					{
						// Add script to thief that returns stolen item to original owner if thief dies before timer is up
						pThief.AddScriptTrigger( returnStolenItemScriptID );

						// Start timer to remove script after timer is up
						pThief.StartTimer( returnStolenItemsTimer, 2, true );
					}
				}

				// Remember to reduce the original pile's amount as well
				itemToSteal.amount -= amountToSteal;
				//itemToSteal.Refresh(); // Needed?
			}

			// If item was a rare stealable, remember to turn it into a "normal" stealable after it's been stolen
			if( itemStolen.stealable == 2 )
			{
				itemStolen.stealable = 1;
			}

			if( itemToSteal.GetTempTag( "specialLoot" ))
			{
				pSock.SysMessage( GetDictionaryEntry( 9210, pSock.language )); // You successfully steal a special item from the creature!
				// TODO: Add magical chime noise to go with this message
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 880, pSock.language )); // You successfully steal the item!
			}

			// Trigger onStolenFrom JS event on target
			if( ValidateObject( itemOwner ) && itemOwner.isChar )
			{
				if( !ValidateObject( itemOwnerOwner ) || itemOwnerOwner != pThief )
				{
					var scriptTriggers = itemOwner.scriptTriggers;
					for( var k = 0; k < scriptTriggers.length; k++ )
					{
						if( DoesEventExist( scriptTriggers[k], "onStolenFrom" ))
						{
							if( TriggerEvent( scriptTriggers[k], "onStolenFrom", pThief, itemOwner, itemStolen ) == 1 )
							{
								// Script on target handles result of being stolen from. Stop here.
								return;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if( theftWitnessed && pThief.region.isGuarded )
		{
			// Criminal flag
			pSock.SysMessage( GetDictionaryEntry( 882, pSock.language )); // You have been caught!
			if( ValidateObject( itemOwner ) && itemOwner.isChar && WillResultInCriminal( pThief, itemOwner ))
			{
				pThief.criminal = true;
			}
		}

		pSock.SysMessage( GetDictionaryEntry( 881, pSock.language )); // You failed to steal that item.
		CleanupSpecialItem( itemToSteal );
	}
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	switch( timerID )
	{
		case 1: // Make recently stolen blessed items non-blessed again
			if( timerObj.isItem )
			{
				timerObj.newbie = 0;
				timerObj.movable = 1;
			}
			break;
		case 2: // Remove script that returns item to owner if thief dies
			if( timerObj.isChar )
			{
				timerObj.RemoveScriptTrigger( returnStolenItemScriptID );
			}
			break;
		/*case 10: // Flag player as having stolen since their last death
			timerObj.hasStolen = true;
			break;
		case 11: // Flag player as criminal
			timerObj.criminal = true;
			break;*/
		default:
			break;
	}
}

function FindNearbyPlayers( srcChar, trgChar, pSock )
{
	if( ValidateObject( trgChar ) && trgChar != srcChar )
	{
		if( !trgChar.npc && !trgChar.online )
			return false;

		// Check Line of Sight between potential witness and thief
		if( trgChar.CanSee( srcChar ))
		{
			// The further away they are, the less likely they are to detect the theft
			// It also matters if NPCs are looking in the direction of thief or not!
			var facingThief = ( trgChar.direction == trgChar.DirectionTo( srcChar ));
			if( RandomNumber( 1, 5 + trgChar.DistanceTo( srcChar ) - ( facingThief ? 2 : 0 )) == 3 )
			{
				// Detected...
				var trgSock = trgChar.socket;
				if( trgSock != null )
				{
					// ...by a player!
					if( parseInt( pSock.tempInt2 ) == trgChar.serial )
					{
						// Inform the target player about the theft
						let targetNoticeMsg = pSock.xText.toString();
						trgChar.TextMessage( targetNoticeMsg, false, 0x3b2, 0, trgChar.serial ); // You notice %s trying to steal %w from you!
					}
					else
					{
						// Inform player witness about the theft
						let publicNoticeMsg = pSock.xText2;
						trgChar.TextMessage( publicNoticeMsg, false, 0x3b2, 0, trgChar.serial ); // You notice %s trying to steal %w from %d!
					}
					return true;
				}
				else
				{
					// ...by an NPC!
					if( trgChar.isHuman ) // We don't want monsters or animals to call for guards
					{
						if( parseInt( pSock.tempInt2 ) == trgChar.serial )
						{
							// Victim will always call guards if they catch the thief in the act
							trgChar.TextMessage( GetDictionaryEntry( 883 )); // Guards!! A thief is among us!
						}
						else if( RandomNumber( 1, 10 ) < ( 7 + ( facingThief ? 2 : 0 )))
						{
							// Random chance that NPC witnesses will call the guards
							trgChar.TextMessage( GetDictionaryEntry( 883 )); // Guards!! A thief is among us!
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}

// Check if item is able to be stolen based on item type and weight vs stealing
// skill, then return the min skill needed to steal the item.
//function CalcStealChance( pThief, itemToSteal, randomItem, theftWitnessed )
function CalcStealChance( pThief, itemToSteal, randomItem, itemAmount, theftWitnessed )
{
	var stealChance = -1;

	switch( itemToSteal.type )
	{
		case 63: 	// Item spawn container
		case 64:  	// Locked spawn container
		case 65: 	// Unlockable item spawn container
		case 87: 	// Trash Container
			break;
		case 1: 	// Container/Backpack
		default:
			var skillModifier = 100 + pThief.skills.stealing;
			var targetingFactor = 1;

			// Stealing a specifically targeted item should be more difficult than stealing randomly
			// (Since Publish 5, UOR)
			if( EraStringToNum( coreShardEra ) >= EraStringToNum( "uor" ))
			{
				// targetingFactor 1 is for random target, 3 is for specific target
				targetingFactor = randomItem ? 1 : 3;
			}

			// Use item weight and skill modifier to calculate a difficulty rating
			var itemWeight = ( itemToSteal.weight / 100 ) * itemAmount;
			var calcDiff = ( itemWeight * targetingFactor * 2000 ) / skillModifier;

			if( !theftWitnessed )
			{
				stealChance = 100 - calcDiff;
			}
			else
			{
				// If the theft attempt was witnessed, success becomes harder - use alternate calculation
				calcDiff = ( itemWeight * targetingFactor * 30 ) + 100;

				// Finally, use the above to calculate final chance of success
				stealChance = Math.max( 0, ( 500 + 2 * ( pThief.skills.stealing - calcDiff )) / 10 );
			}

			// Does dexterity affect player's steal chance? Custom addition, disabled by default!
			if( dexAffectsStealChance )
			{
				// Apply a 0 to -10% penalty for stealChance the lower pThief's dex is below 50
				// Apply a 0 to +10% bonus for stealChance the higher pThief's dex is above 50
				stealChance = stealChance * ( 1 + (( pThief.dexterity - 50 ) / 500 ))
			}

			break;
	}

	return stealChance;
}

// Clean up any potential special items created on monsters when players attempt to steal
// (since they failed to steal it, apparently)
function CleanupSpecialItem( itemToSteal )
{
	if( ValidateObject( itemToSteal ))
	{
		if( itemToSteal.GetTag( "specialLoot" ))
		{
			itemToSteal.Delete();
		}
	}
}

// Dummy function to restore script context after other JS events have triggered and executed
// as part of something happening in this script
function _restorecontext_() {}
