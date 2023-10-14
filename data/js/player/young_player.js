const skillProps = [
	"alchemy", "anatomy", "animallore", "itemid", "armslore", "parrying", "begging",
	"blacksmithing", "bowcraft", "peacemaking", "camping", "carpentry", "cartography", "cooking",
	"detectinghidden", "enticement", "evaluatingintel", "healing", "fishing", "forensics",
	"herding", "hiding", "provocation", "inscription", "lockpicking", "magery", "magicresistance",
	"tactics", "snooping", "musicianship", "poisoning", "archery", "spiritspeak", "stealing",
	"tailoring", "taming", "tasteid", "tinkering", "tracking", "veterinary", "swordsmanship",
	"macefighting", "fencing", "wrestling", "lumberjacking", "mining", "meditation", "stealth",
	"removetrap", "necromancy", "focus", "chivalry", "bushido", "ninjitsu", "spellweaving",
	"imbuing", "mysticism", "throwing"
];

// Healer locations young players are teleported to upon death
const feluccaHealerLocations = [
	[1481, 1612, 20], [2708, 2153, 0], [2249, 1230, 0], [5197, 3994, 37], [1412, 3793, 0],
	[3688, 2232, 20], [2578, 604, 0], [4397, 1089, 0], [5741, 3218, -2], [2996, 3441, 15],
	[624, 2225, 0], [1916, 2814, 0], [2929, 854, 0], [545, 967, 0], [3469, 2559, 36]
];
const trammelHealerLocations = [
	[1481, 1612, 20], [2708, 2153, 0], [2249, 1230, 0], [5197, 3994, 37], [1412, 3793, 0],
	[3688, 2232, 20], [2578, 604, 0], [4397, 1089, 0], [5741, 3218, -2], [2996, 3441, 15],
	[624, 2225, 0], [1916, 2814, 0], [2929, 854, 0], [545, 967, 0], [3469, 2559, 36]
];
const ilshenarHealerLocations = [
	[1216, 468, -13], [723, 1367, -60], [745, 725, -28], [281, 1017, 0], [986, 1011, -32],
	[1175, 1287, -30], [1533, 1341, -3], [529, 217, -44], [1722, 219, 96]
];
const malasHealerLocations = [
	[2079, 1376, -70], [944, 519, -71]
];
const tokunoHealerLocations = [
	[1166, 801, 27], [782, 1228, 25], [268, 624, 15]
];

const youngPlayerItems = [
	// itemSection, amount, newbie?
	["newplayerticket", 1, true]
];

const coreShardEra = GetServerSetting( "CoreShardEra" );
const youngPlayerSystem = GetServerSetting( "YoungPlayerSystem" );
const youngMaxTotalSkills = 3500; // Max skill points in total, per character
const youngMaxTotalStats = 150; // Max 150 stat points in total, per character
const youngMaxSkill = 700; // Max skill points in a single skill, per character
const youngMaxStat = 80; // Max stat points in a single stat, per character
const youngMaxAcctAge = 40; // 40 hours

// Check to make sure player still meets requirements of being considered a "Young" player
function CheckYoungStatus( pSock, pChar, fromLogin )
{
	/* A player is considered “Young” if their account is less than 40 hours and their characters:
	Have less than 350 total skill points
	Have less than 70 in any one skill
	Have less than 150 stat points
	Have less than 80 in any one stat
	Have not had their “Young” status revoked by a GM*/
	// Players can renounce their Young status by saying the words: “I renounce my young player status”
	if( !youngPlayerSystem )
		return;

	var hoursAsYoungLeft = 0;
	var revokeReason = "";
	var pAccount = pChar.account;
	if( pAccount.totalPlayTime > 0 && fromLogin )
	{
		// Does the account have a total playtime of over 40 hours?
		var totalPlayTimeInHrs = ( pAccount.totalPlayTime / 60 );
		if( totalPlayTimeInHrs > youngMaxAcctAge )
		{
			revokeReason = GetDictionaryEntry( 18700, pSock.language ); // You are old enough to be considered an adult, and have outgrown your status as a young player!
		}
		else
		{
			hoursAsYoungLeft = Math.round( youngMaxAcctAge - totalPlayTimeInHrs );
		}
	}

	if( revokeReason == "" )
	{
		// Iterate over the characters on player's account, and check their skills and stats
		for( var i = 1; i < 8; i++ )
		{
			// Reset these for each character
			var totalSkillPoints = 0;
			var highestSkill = 0;
			var totalStatPoints = 0;
			var highestStat = 0;

			var tempChar = pAccount["character"+i];
			if( ValidateObject( tempChar ))
			{
				// Add up character's total base skillpoints
				totalSkillPoints += tempChar.baseskills.allskills;
				if( totalSkillPoints > youngMaxTotalSkills )
				{
					revokeReason = GetDictionaryEntry( 18702, pSock.language ); // You have successfully obtained a respectable skill level, and have outgrown your status as a young player!
					break;
				}

				// Find the character's highest skill
				for( var j = 0; j < 58; j++ )
				{
					if( tempChar.baseskills[skillProps[j]] > highestSkill )
					{
						highestSkill = tempChar.baseskills[skillProps[j]];
					}
				}
				if( highestSkill > youngMaxSkill )
				{
					revokeReason = GetDictionaryEntry( 18702, pSock.language ); // You have successfully obtained a respectable skill level, and have outgrown your status as a young player!
					break;
				}

				// Add up character's total statpoints
				totalStatPoints += tempChar.strength;
				totalStatPoints += tempChar.dexterity;
				totalStatPoints += tempChar.intelligence;
				if( totalStatPoints > youngMaxTotalStats )
				{
					pSock.SysMessage( "totalStatPoints: " + totalStatPoints );
					revokeReason = GetDictionaryEntry( 18703, pSock.language ); // You have grown into a respectably strong person, and have outgrown your status as a young player!
					break;
				}

				// Find the character's highest stat
				highestStat = tempChar.strength;
				if( tempChar.dexterity > highestStat )
				{
					highestStat = tempChar.dexterity;
				}
				if( tempChar.intelligence > highestStat )
				{
					highestStat = tempChar.intelligence;
				}
				if( highestStat > youngMaxStat )
				{
					pSock.SysMessage( "highestStat: " + highestStat );
					revokeReason = GetDictionaryEntry( 18703, pSock.language ); // You have grown into a respectably strong person, and have outgrown your status as a young player!
					break;
				}
			}
		}
	}


	if( revokeReason != "" )
	{
		RevokeYoungStatus( pSock, pChar, revokeReason );
	}
	else if( fromLogin )
	{
		pSock.SysMessage( GetDictionaryEntry( 18701, pSock.language), hoursAsYoungLeft, ( hoursAsYoungLeft > 1 ? "s" : "" )); // You will enjoy the benefits and relatively safe status of a young player for %i more hour%s
	}
}

// Hand out items to player specific for new Young players
function GiveYoungPlayerItems( pSock, pChar )
{
	if( pSock == null )
		return;

	for( var i = 0; i < youngPlayerItems.length; i++ )
	{
		var itemSection = youngPlayerItems[i][0];
		var itemAmount = youngPlayerItems[i][1];
		var isNewbie = youngPlayerItems[i][2];
		var newItem = CreateDFNItem( pSock, pChar, itemSection, itemAmount, "ITEM", true );
		if( ValidateObject( newItem ))
		{
			if( isNewbie )
			{
				newItem.newbie = true;
			}

			if( newItem.sectionID == "newplayerticket" )
			{
				// Store character's serial on ticket, and mark it as owned by player
				newItem.owner = pChar;
				newItem.SetTag( "ticketUsableBy", ( pChar.serial ).toString() );
			}
		}
		else
		{
			Console.Warning( "Unable to create Young player item with section '" + youngPlayerItems[i] + "'!" );
		}
	}
}

// Detect renounce Young message from player
function onTalk( pTalking, strSaid )
{
	if( !youngPlayerSystem )
		return false;

	if( strSaid == "" )
		return false;

	var pSock = pTalking.socket;
	if( pSock == null )
		return false;


	var trigWordHandled = false;
	for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
	{
		if( trigWordHandled )
			break;

		switch( trigWord )
		{
			case 53: // I renounce my young player status
				// Display renounce confirmation gump
				ShowRenounceYoungGump( pSock, pTalking );
				break;
			default:
				break;
		}
	}

	return true;
}

// Revoke the player's young status!
function RevokeYoungStatus( pSock, pChar, revokeReason )
{
	pSock.SysMessage( revokeReason );
	pSock.SysMessage( GetDictionaryEntry( 18704, pSock.language )); // You are no longer considered a young player of Ultima Online, and are no longer subject to the limitations and benefits of being in that caste.
	pChar.account.isYoung = false;
	pChar.Refresh();
}

// Handles situations where the Young player is the target of spells
function onSpellTarget( myTarget, pCaster, spellID )
{
	// We don't care if caster and target is the same - allow it!
	if( myTarget == pCaster )
		return false;

	if( youngPlayerSystem )
	{
		var pSock = pCaster.socket;
		if( pSock != null )
		{
			pSock.SysMessage( GetDictionaryEntry( 18705, pSock.language )); // You may not cast that on the Young.
			return 2;
		}
	}

	// By default, allow Young player to be target of spells from monsters/npcs
	return false;
}

// Handles situations where the Young player is the caster of spells
function onSpellTargetSelect( pCaster, myTarget, spellID )
{
	// We don't care if caster and target is the same - allow it!
	if( myTarget == pCaster )
		return false;

	if( youngPlayerSystem )
	{
		var pSock = pCaster.socket;
		if( pSock != null )
		{
			var spellCast = Spells[spellID];
			if( spellCast.aggressiveSpell && ( !myTarget.npc || ( myTarget.npc && ValidateObject( myTarget.owner ))))
			{
				// Young players cannot cast hostile spells on any other players
				pSock.SysMessage( GetDictionaryEntry( 18706, pSock.language )); // You may not cast that on another player, because you are Young.
				return 2;
			}
			else if(( !myTarget.npc && !myTarget.account.isYoung ) || ( myTarget.npc && ValidateObject( myTarget.owner ) && !myTarget.owner.account.isYoung ))
			{
				// Beneficial spells can be cast on self or other Young players only
				pSock.SysMessage( GetDictionaryEntry( 18707, pSock.language )); // As a young player, you may not cast beneficial spells onto older players.
				return 2;
			}
		}
	}

	// By default, allow casting hostile spells on NPCs, or beneficial spells on other Young players
	return false;
}

// Detect stat change to re-run check of young status
function onStatChange( pChar, statID, statChange )
{
	var pSock = pChar.socket;
	if( pSock == null )
		return;

	if( youngPlayerSystem && statChange > 0 )
	{
		// If the player gained stats, re-check the young status to see if they still meet all of
		// the requirements
		CheckYoungStatus( pSock, pChar, false );
	}
}

// Detect skill change to re-run check of young status
function onSkillChange( pChar, skillID, skillChange )
{
	var pSock = pChar.socket;
	if( pSock == null )
		return;

	if( youngPlayerSystem && skillChange > 0 )
	{
		// If the player gained skill, re-check the young status to see if they still meet all of
		// the requirements
		CheckYoungStatus( pSock, pChar, false );
	}
}

// This will trigger both for Young player when attacking
// and when someone else attacks Young player
function onCombatStart( pAttacker, pDefender )
{
	// Allow by default, let code/other scripts handle it
	if( !youngPlayerSystem )
		return true;

	// Treat pets of attacking player the same as if they're the player
	var pAttackOwner = pAttacker.owner;
	if( ValidateObject( pAttackOwner ) && !pAttackOwner.npc )
	{
		pAttacker = pAttackOwner;
	}

	// Treat pets of defending player the same as if they're the player
	var pDefenderOwner = pDefender.owner;
	if( ValidateObject( pDefenderOwner ) && !pDefenderOwner.npc )
	{
		pDefender = pDefenderOwner;
	}

	// If attacker is Young
	if( !pAttacker.npc && pAttacker.account.isYoung && !pDefender.npc )
	{
		var pAttackSock = pAttacker.socket;
		if( pAttackSock )
		{
			pAttackSock.SysMessage( GetDictionaryEntry( 18708, pAttackSock.language )); // As a Young player, you cannot harm other players, or their followers.
		}
		return false;
	}

	// If defender is Young
	if( !pDefender.npc && pDefender.account.isYoung && !pAttacker.npc )
	{
		var pAttackSock = pAttacker.socket;
		if( pAttackSock )
		{
			pAttackSock.SysMessage( GetDictionaryEntry( 18709, pAttackSock.language )); // You cannot harm Young players, or their followers.
		}
		return false;
	}

	// Allow by default
	return true;
}

// Triggers when Young player deals damage
function onDamageDeal( pAttacker, pTarget, damageValue, damageType )
{
	if( !ValidateObject( pAttacker ) || !ValidateObject( pTarget ))
		return false;

	if( !youngPlayerSystem )
		return true;

	// If player attacks someone's pet, assume the pet owner IS the target
	if( pTarget.npc && ValidateObject( pTarget.owner ) && !pTarget.owner.npc )
	{
		pTarget = pTarget.owner;
	}

	if( !pTarget.npc )
	{
		// Disallow Young players from damaging other players
		var pSock = pAttacker.socket;
		if( pSock )
		{
			pSock.SysMessage( GetDictionaryEntry( 18708, pSock.language )); // As a Young player, you cannot harm other players or their followers.
		}
		return false;
	}

	// Allow applying damage by default
	return true;
}

// Triggers when Young player takes damage
function onDamage( damaged, pAttacker, damageValue, damageType )
{
	if( !ValidateObject( damaged ))
		return false;

	if( !youngPlayerSystem )
		return true;

	if( ValidateObject( pAttacker ))
	{
		// If player is attacked by someone's pet, assume the pet owner IS the pAttacker
		if( pAttacker.npc && ValidateObject( pAttacker.owner ) && !pAttacker.owner.npc )
		{
			pAttacker = pAttacker.owner;
		}

		if( !pAttacker.npc )
		{
			// Disallow, players cannot harm a Young player
			var pAttackerSock = pAttacker.socket;
			if( pAttackerSock )
			{
				pAttackerSock.SysMessage( GetDictionaryEntry( 18709, pAttackerSock.language )); // You cannot harm Young players.
			}
			return false;
		}
	}

	// Allow damage by default
	return true;
}

// Handle death of young player
function onDeath( pDead, iCorpse )
{
	if( !ValidateObject( pDead ))
		return false;

	var pSock = pDead.socket;
	if( pSock == null )
		return false;

	if( !youngPlayerSystem )
		return false;

	// First teleport young player to a healer location
	if( TeleportToSafety( pDead, pSock ))
	{
		// Then show them the Young player death notice after a short delay
		pDead.StartTimer( 2500, 1, true );
	}

	return false;
}

// Teleport Young player to nearest healer
function TeleportToSafety( pDead, pSock )
{
	// If in jail, or in Samurai/Ninja starting locations, cancel teleport
	if( !youngPlayerSystem || pDead.isJailed )
		return false;

	var targX = 0;
	var targY = 0;
	var targZ = 0;
	var teleportList;

	var pRegion = pDead.region;
	if( pRegion.isDungeon )
	{
		// Set teleport location to entrance of dungeon
		return true;
	}
	else
	{
		// Set teleport location to healer location, based on current map
		switch( pDead.worldnumber )
		{
			case 0: // Felucca
				teleportList = feluccaHealerLocations;
				break;
			case 2: // Ilshenar
				teleportList = ilshenarHealerLocations;
				break;
			case 3: // Malas
				teleportList = malasHealerLocations;
				break;
			case 4: // Tokuno
				teleportList = tokunoHealerLocations;
				break;
			case 1: // Trammel
			default:
				teleportList = trammelHealerLocations;
				break;
		}
	}

	var pX = pDead.x;
	var pY = pDead.y;
	var targLoc;
	var squareDist = Infinity;

	// Find the closest healer location to the player's current position
	for( var i = 0; i < teleportList.length; i++ )
	{
		var curTargLoc = teleportList[i];
		var width = pX - curTargLoc[0];
		var height = pY - curTargLoc[1];
		var curSquareDist = width * width + height * height;
		if( curSquareDist < squareDist )
		{
			targLoc = curTargLoc;
			squareDist = curSquareDist;
		}
	}

	pDead.Teleport( targLoc[0], targLoc[1], targLoc[2] );
	return true;
}

// Trigger death notice after a short delay
function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		ShowDeathNotice( timerObj );
	}
}

// Show death notice to player
function ShowDeathNotice( pChar )
{
	if( !ValidateObject( pChar ))
		return;

	var pSock = pChar.socket;
	if( pSock == null )
		return;

	var deathNotice = new Gump();
	deathNotice.NoClose();
	deathNotice.AddBackground( 25, 10, 425, 444, 0x13be );
	deathNotice.AddTiledGump( 33, 20, 407, 425, 0x0a40 );
	deathNotice.AddCheckerTrans( 33, 20, 407, 425 );
	deathNotice.AddHTMLGump( 190, 24, 120, 20, false, false, "<basefont color=white>" + GetDictionaryEntry( 18710, pSock.language ) + "</basefont>" ); // You have died.
	deathNotice.AddHTMLGump( 50, 50, 380, 40, false, false, "<basefont color=white>" + GetDictionaryEntry( 18711, pSock.language ) + "</basefont>" ); // As a ghost you cannot interact with the world. You cannot touch items nor can you use them.
	deathNotice.AddHTMLGump( 50, 100, 380, 45, false, false, "<basefont color=white>" + GetDictionaryEntry( 18712, pSock.language ) + "</basefont>" ); // You can pass through doors as though they do not exist.  However, you cannot pass through walls.
	deathNotice.AddHTMLGump( 50, 140, 380, 60, false, false, "<basefont color=white>" + GetDictionaryEntry( 18713, pSock.language ) + "</basefont>" ); // Since you are a new player, any items you had on your person at the time of your death will be in your backpack upon resurrection.
	deathNotice.AddHTMLGump( 50, 204, 380, 65, false, false, "<basefont color=white>" + GetDictionaryEntry( 18714, pSock.language ) + "</basefont>" ); // To be resurrected you must find a healer in town or wandering in the wilderness.  Some powerful players may also be able to resurrect you.
	deathNotice.AddHTMLGump( 50, 269, 380, 65, false, false, "<basefont color=white>" + GetDictionaryEntry( 18715, pSock.language ) + "</basefont>" ); // While you are still in young status, you will be transported to the nearest healer (along with your items) at the time of your death.
	deathNotice.AddHTMLGump( 50, 334, 380, 70, false, false, "<basefont color=white>" + GetDictionaryEntry( 18716, pSock.language ) + "</basefont>" ); // To rejoin the world of the living simply walk near one of the NPC healers, and they will resurrect you as long as you are not marked as a criminal.
	deathNotice.AddButton( 195, 410, 0x0f8, 0x0f9, 1, 0, 1 );
	deathNotice.Send( pSock );
	deathNotice.Free();
}

// Show dungeon warning to Young player upon entering dungeon with hostile monsters
function onEnterRegion( pEntering, regionEntered )
{
	var townRegion = GetTownRegion( regionEntered );
	if( townRegion.isDungeon && youngPlayerSystem )
	{
		var pSock = pEntering.socket;
		if( pSock == null )
			return;

		// Send young player a warning about hostile monsters when they enter dungeons
		var dungeonWarning = new Gump();
		dungeonWarning.AddBackground( 0, 0, 250, 170, 0x0a28 );
		dungeonWarning.AddHTMLGump( 20, 43, 215, 70, false, false, GetDictionaryEntry( 18717, pSock.language )); // Warning: monsters may attack you on sight down here in the dungeons!
		dungeonWarning.AddButton( 70, 123, 0x0fa5, 0x0fa7, 1, 0, 2 );
		dungeonWarning.AddHTMLGump( 105, 125, 100, 35, false, false, GetDictionaryEntry( 18718, pSock.language )); // OKAY
		dungeonWarning.Send( pSock );
		dungeonWarning.Free();
	}
}

// Show gump where Young player confirms whether to renounce their Young status
function ShowRenounceYoungGump( pSock, pTalking )
{
	var renounceConfirm = new Gump();
	renounceConfirm.AddBackground( 0, 0, 450, 400, 0x0a28 );
	renounceConfirm.AddHTMLGump( 0, 30, 450, 35, false, false, "<center> " + GetDictionaryEntry( 18719, pSock.language ) + "</center>" ); // Renouncing 'Young Player' Status
	renounceConfirm.AddHTMLGump( 30, 70, 390, 210, true, true, GetDictionaryEntry( 18720, pSock.language )); // As a 'Young' player, you are currently under a system of protection that prevents you from being attacked by other players and certain monsters.<br><br>If you choose to renounce your status as a 'Young' player, you will lose this protection. You will become vulnerable to other players, and many monsters that had only glared at you menacingly before will now attack you on sight!<br><br>Select OKAY now if you wish to renounce your status as a 'Young' player, otherwise press CANCEL." )

	renounceConfirm.AddButton( 45, 298, 0x0fa5, 0x0fa7, 1, 0, 3 );
	renounceConfirm.AddHTMLGump( 78, 300, 100, 35, false, false, GetDictionaryEntry( 18718, pSock.language )); // OKAY

	renounceConfirm.AddButton( 178, 298, 0x0fa5, 0x0fa7, 1, 0, 0 );
	renounceConfirm.AddHTMLGump( 211, 300, 100, 35, false, false, GetDictionaryEntry( 18721, pSock.langauge )); // CANCEL
	renounceConfirm.Send( pSock );
	renounceConfirm.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	if( pSock == null )
		return;

	var pChar = pSock.currentChar;
	if( !ValidateObject( pChar ))
		return;

	switch( pButton )
	{
		case 0: // Do nothing
			break;
		case 1: // OKAY.... in death notice gump
			break;
		case 2: // OKAY.... in dungeon notice gump
			break;
		case 3: // OKAY.... in renounce young status gump
			var revokeReason = GetDictionaryEntry( 18722, pSock.language ); // You have chosen to renounce your `Young' player status.
			RevokeYoungStatus( pSock, pChar, revokeReason );
			break;
		case 4: // CANCEL.... in renounce young status gump
			pSock.SysMessage( GetDictionaryEntry( 18723, pSock.language )); // You have chosen not to renounce your `Young' player status.
			break;
		default:
			break;
	}
}

// Display [Young] tag in tooltip
function onTooltip( pChar )
{
	var tooltipText = "";
	if( youngPlayerSystem && pChar.account.isYoung )
	{
		tooltipText = GetDictionaryEntry( 18724 ); // [Young]
	}
	return tooltipText;
}

function _restorecontext_() {}

// [✓] 	As a young player, you may not cast beneficial spells onto older players.
// [] 	As a young player, you may not use beneficial skills on older players.
// [✓] 	Some young warrior felled this, and you cannot bring yourself to take any items from the corpse.
// [✓] 	You are too young to join my guild...
// [] 	Thou art too young to join my guild. Wait a week then return.
// [✓] 	Because of your young status in Britannia you cannot provoke the beast onto another player yet.
// [] 	As a young player, you may not use this spell.
// [✓] 	As a young player, you may not friend pets to older players.
// [✓] 	As an older player, you may not friend pets to young players.
// [✓] 	As a young player, you may not transfer pets to older players.
// [✓] 	As an older player, you may not transfer pets to young players.
// [✓] 	You have chosen to renounce your `Young' player status.
// [✓] 	You have chosen not to renounce your `Young' player status.
// [✓] 	You cannot be a member of the Thieves' Guild while you are Young.
// [] 	Thou art too young to choose this fate.
// [✓] 	You may not cast that on another player, because you are Young.
// [✓] 	You may not cast that on the Young.
// [✓] 	You cannot steal from the Young.
// [] 	Your account currently has young player status.  If you wish to remain In the war torn lands where Minax has taken control, you must give up  this Young player status.  If you do not wish to lose your young player status, choose 'Cancel' and you will be automatically transported to the Lands of Lord British.
// [] 	You cannot join a faction as a young player
// [✓] 	<center> Renouncing 'Young Player' Status</center>
// [✓] 	As a 'Young' player, you are currently under a system of protection that prevents you from being attacked by other players and certain monsters.<br><br>If you choose to renounce your status as a 'Young' player, you will lose this protection. You will become vulnerable to other players, and many monsters that had only glared at you menacingly before will now attack you on sight!<br><br>Select OKAY now if you wish to renounce your status as a 'Young' player, otherwise press CANCEL.
// [] 	While your young player status continues, you can re-take parts or all of the tutorial at any time by selecting the 'Help' button from your paperdoll, then chose the 'Tutorial Options' button.
// [✓] 	You have successfully obtained a respectable skill level, and have outgrown your status as a young player!
// [✓] 	You have grown into a respectably strong person, and have outgrown your status as a young player!
// [✓] 	You are old enough to be considered an adult, and have outgrown your status as a young player!
// [✓] 	You are no longer considered a young player of Ultima Online, and are no longer subject to the limitations and benefits of being in that caste.
// [] 	<U>Young player haven transport</U>: Select this option if you want to be transported to Haven.
// [] 	<u>Go to Haven.</u> This option will take you to the city of Haven. Haven is a place of learning for newcomers to Britannia.
// [] 	You have been kicked out of your guild!  Young players may not remain in a guild which is allied with a faction.
// [] 	I'm a young player!
// [] 	I'm not a young player!
// [] 	Only a young player may use this treasure map.
// [] 	This can only be used by young players.
// [✓] 	You decide against traveling to Felucca while you are still young.
// [] 	The young player has declined your invitation.
// [] 	The young player has accepted your invitation.
// [] 	Select the young player you wish to invite to be your squire.
// [] 	You cannot use this virtue while you are a young player.
// [] 	That is not a young player.
// [] 	Your squire has lost 'young' status.
// [] 	a young player ticket
// [] 	You cannot invite a young player to your faction-aligned guild.
// [] 	You must be young to have this item recharged.
// [] 	You are already asking a young player to be your squire.
// [] 	Your squire has renounced young player status.
// [] 	You cannot invite young players.
// [] 	As a young player, you may not enter this area.
// [] 	You are too young to receive a ~1_TYPE~ gift.
// [] 	Young players may not join Vice vs Virtue. Renounce your young player status by saying, "I renounce my young player status" and try again.
// [] 	Your character is too young to obtain this reward.
// [] 	Young Status
// [] 	Young Requirements
// [] 	Young Benefits
// [] 	Young Restrictions
// [✓]	Warning: monsters may attack you on sight down here in the dungeons!
// [✓]	A monster looks at you menacingly but does not attack.  You would be under attack now if not for your status as a new citizen of Britannia.
// [✓] 	* The poison seems to have no effect. *