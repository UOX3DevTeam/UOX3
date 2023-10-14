// This script handles interactions with NPC guildmasters, such as joining/leaving and making use
// of special services offered by these NPCs - such as selling disguises (Thieves Guild)
//
// By default, only the Thieves Guild has any special functionality, but custom requirements and/or
// services can be added for other guilds as necessary.

const ageRequirement = 7; // Character needs to be 7 days old to join an NPC guild
const joinGuildCost = 500;
const disguiseKitCost = 700;
const disguiseKitEnabled = true;

// Default guild-ID assignments:
// 1 - The Guild of Arcane Arts
// 2 - The Warrior's Guild
// 3 - The Society of Thieves
// 4 - The League of Rangers
// 5 - The Healer's Guild
// 6 - The Mining Cooperative
// 7 - The Merchant's Association
// 8 - The Order of Engineers
// 9 - The Society of Clothiers
/// 10 - The Maritime Guild
/// 11 - The Bardic Collegium
/// 12 - The Fellowship of Blacksmiths

const npcGuilds = {
	// guildID: [dictionaryEntry, parentGuild, [enemyNPCGuild1, enemyNPCGuild2, etc.]]

	// [1-49] Main NPC Guilds
	1: [17501, -1, []], // The Guild of Arcane Arts
	2: [17502, -1, []], // The Warrior's Guild
	3: [17503, -1, []], // The Society of Thieves
	4: [17504, -1, []], // The League of Rangers
	5: [17505, -1, []], // The Healer's Guild
	6: [17506, -1, []], // The Mining Cooperative
	7: [17507, -1, []], // The Merchant's Association
	8: [17508, -1, []], // The Order of Engineers
	9: [17509, -1, []], // The Society of Clothiers
	10: [17510, -1, []], // The Maritime Guild
	11: [17511, -1, []], // The Bardic Collegium
	12: [17512, -1, []], // The Fellowship of Blacksmiths

	// [50-59] Sub Guilds for The Guild of Arcane Arts
	50: [17550, 1, []], // The Guild of Mages
	51: [17551, 1, []], // The Guild of Sorcery
	52: [17552, 1, []], // The Masters of Illusion

	// [60-69] Sub Guilds for The Warrior's Guild
	60: [17553, 2, []], // The Association of Warriors
	61: [17554, 2, []], // The Fighters and Footmen
	62: [17555, 2, []], // The Guild of Cavalry and Horse

	// [70-79] Sub Guilds for The Society of Thieves
	70: [17556, 3, []], // The Federation of Rogues and Beggars
	71: [17557, 3, []], // The Guild of Assassins
	72: [17558, 3, []], // The Society of Thieves

	// [80-89] Sub Guilds for The League of Rangers
	// None by default

	// [90-99] Sub Guilds for The Healer's Guild
	// None by default

	// [100-109] Sub Guilds for The Mining Cooperative
	// None by default

	// [110-119] Sub Guilds for The Merchant's Association
	110: [17559, 7], // The Merchants' Guild
	111: [17560, 7], // The Guild of Provisioners
	112: [17561, 7], // The Traders' Guild
	113: [17562, 7], // The Barterers' Guild

	// [120-129] Sub Guilds for The Order of Engineers
	// None by default

	// [130-139] Sub Guilds for The Society of Clothiers
	120: [17563, 9], // Tailor's Hall

	// [140-149] Sub Guilds for The Maritime Guild
	130: [17564, 10], // Guild of Fishermen
	131: [17565, 10], // Sailors' Maritime Association
	132: [17566, 10], // Seamen's Chapter
	133: [17567, 10], // Society of Shipwrights

	// [150-159] Sub Guilds for The Bardic Collegium
	// None by default

	// [160-169] Sub Guilds for The Fellowship of Blacksmiths
	160: [17568, 12], // The Armourers' Guild
	161: [17569, 12], // The Blacksmiths' Guild
	162: [17570, 12], // The Guild of Armaments
	163: [17571, 12] // The Society of Weaponsmiths


	// [500-1000] Custom NPC Guilds
	// ...
};

// List of NPC guilds players are able to join:
const enableddGuilds = [ 3 ];

// Can call from other scripts using
// var guildRelation = TriggerEvent( 3217, "CheckGuildRelation", npcGuildA, npcGuildB );
function CheckGuildRelation( npcGuildA, npcGuildB )
{
	// Are the NPC guilds actually the same guild?
	if( npcGuildA == npcGuildB )
	{
		return 1; // Same guild!
	}

	const guildA = npcGuilds[npcGuildA];
	const guildB = npcGuilds[npcGuildB];

	// Do the NPC guilds share a parent/child relationship?
	if( guildA[1] === npcGuildB || guildB[1] === npcGuildA )
	{
		return 2; // Parent/child guilds!
	}

	// Are the NPC guilds considered enemies?
	if( guildA[2] && npcGuilds[guildA[2]][3].indexOf( npcGuildB ) != -1 ||
      guildB[2] && npcGuilds[guildB[2]][3].indexOf( npcGuildA ) != -1 )
	{
    	return 3; // Enemy guilds!
  	}

	return 0; // No relation
}

function onSpeech( strSaid, pChar, npcGuildMaster )
{
	if( strSaid )
	{
		var pSock = pChar.socket;
		if( pSock == null )
			return false;

		var trigWordHandled = false;
		for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
		{
			if( trigWordHandled )
				break;

			switch( trigWord )
			{
				case 4: // join | member
					if( CheckJoinGuildRequirements( pSock, pChar, npcGuildMaster, false ))
					{
						let dictMsg = GetDictionaryEntry( 17500, pSock.language ); // I am the %s of %d
						dictMsg = dictMsg.replace( /%s/gi, GetDictionaryEntry( npcGuildMaster.gender ? 17601 : 17600, pSock.language )); // %s guildmaster/guildmistress
						dictMsg = dictMsg.replace( /%d/gi, GetDictionaryEntry( npcGuilds[npcGuildMaster.npcGuild][0], pSock.language )); // %d [npc guild name]
						dictMsg = GetDictionaryEntry( 17602, pSock.language ); // There is a fee in gold coins for joining the guild : %i
						npcGuildMaster.TextMessage( dictMsg.replace( /%i/gi, joinGuildCost ));
					}
					trigWordHandled = true;
					break;
				case 5: // resign | quit
					if( pChar.npcGuild != npcGuildMaster.npcGuild )
					{
						npcGuildMaster.TextMessage( GetDictionaryEntry( 17603, pSock.language )); // Thou dost not belong to my guild!
					}
					else
					{
						var currTimeMin = Math.floor( Date.now() / ( 60 * 1000 ));
						if( currTimeMin - pChar.npcGuildJoined < 10080 )
						{
							var dictMsg = GetDictionaryEntry( 17621, pSock.language ); // Time since joining guild: %i minutes.
							pChar.TextMessage( dictMsg.replace( /%i/gi, ( currTimeMin - pChar.npcGuildJoined ).toString() ), false, 0x3b2, 0, pChar.serial );
							npcGuildMaster.TextMessage( GetDictionaryEntry( 17604, pSock.language )); // You just joined my guild! You must wait a week to resign.
						}
						else
						{
							npcGuildMaster.TextMessage( GetDictionaryEntry( 17605, pSock.language )); // I accept thy resignation.
							pChar.npcGuild = 0;
						}
					}

					trigWordHandled = true;
					break;
				case 6: // guild | guilds
					var dictMsg = GetDictionaryEntry( 17500, pSock.language ); // I am the %s of %d
					dictMsg = dictMsg.replace( /%s/gi, GetDictionaryEntry( npcGuildMaster.gender ? 17601 : 17600, pSock.language )); // %s guildmaster/guildmistress
					dictMsg = dictMsg.replace( /%d/gi, GetDictionaryEntry( npcGuilds[npcGuildMaster.npcGuild][0], pSock.language )); // %d [npc guild name]
					npcGuildMaster.TextMessage( dictMsg );
					trigWordHandled = true;
					break;
				case 0x1F: // disguise
					// Make sure both NPC and player both belong to Thieves Guild
					if( disguiseKitEnabled && npcGuildMaster.npcGuild == 3 && pChar.npcGuild == 3 )
					{
						let dictMsg = GetDictionaryEntry( 17606, pSock.language );
						npcGuildMaster.TextMessage( dictMsg.replace( /%i/gi, disguiseKitCost )); // That particular item costs %i gold pieces.
						pChar.SetTempTag( "guildServiceRequest", "disguisekit" );
					}
					else
					{
						npcGuildMaster.TextMessage( GetDictionaryEntry( 17607, pSock.language )); // I don't know what you're talking about.
					}
					trigWordHandled = true;
					break;
				default:
					break;
			}
		}

		if( trigWordHandled )
		{
			npcGuildMaster.SetTimer( Timer.MOVETIME, 10000 );
			npcGuildMaster.TurnToward( pChar );
		}
	}
}

function onDropItemOnNpc( pChar, npcGuildMaster, iDropped )
{
	var pSock = pChar.socket;
	if( pSock == null )
		return 0;

	if( iDropped.id == 0x0eed ) // Gold
	{
		if( pChar.npcGuild == npcGuildMaster.npcGuild )
		{
			if( pChar.GetTempTag( "guildServiceRequest" ))
			{
				return HandleGuildServices( pChar, npcGuildMaster, iDropped );
			}
			else
			{
				npcGuildMaster.TextMessage( GetDictionaryEntry( 17608, pSock.language )); // Thou art already a member of our guild.
			}
		}
		else if( pChar.npcGuild != 0 )
		{
			npcGuildMaster.TextMessage( GetDictionaryEntry( 17609, pSock.language )); // Thou must resign from thy other guild first.
		}
		else if( CheckJoinGuildRequirements( pSock, pChar, npcGuildMaster ))
		{
			if( iDropped.amount >= joinGuildCost )
			{
				let dictMsg = GetDictionaryEntry( 17610, pSock.language ); // Welcome to %s!
				dictMsg = dictMsg.replace( /%s/gi, GetDictionaryEntry( 17500 + npcGuildMaster.npcGuild, pSock.language )); // %s [guildname]
				npcGuildMaster.TextMessage( dictMsg, false, 0x3b2, 0, pChar.serial );
				pChar.npcGuild = npcGuildMaster.npcGuild;

				// Get the current timestamp in milliseconds, but round to minutes before storing in character property
				pChar.npcGuildJoined = Math.floor( Date.now() / ( 60 * 1000 ));

				// Subtract the joining fee!
				if( iDropped.amount > joinGuildCost )
				{
					iDropped.amount -= joinGuildCost;
					return 0; // Bounce remainder back to player's backpack
				}
				else
				{
					iDropped.Delete();
					return 2;
				}
			}
			else
			{
				npcGuildMaster.TextMessage( GetDictionaryEntry( 17620, pSock.language )); // That's not enough to cover the guild membership fee, sorry.
			}
		}

		// Bounce dropped item back to player's backpack
		return 0;
	}
}

function CheckJoinGuildRequirements( pSock, pChar, npcGuildMaster )
{
	// General Guild Requirements
	if( pChar.npcGuild == npcGuildMaster.npcGuild )
	{
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17608, pSock.language )); // Thou art already a member of our guild.
		return false;
	}
	else if( enabledGuilds.indexOf( npcGuildMaster.npcGuild ) == -1 )
	{
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17617, pSock.language )); // Sorry, we are not accepting any new members right now.
		return false;
	}
	else if( pChar.npcGuild != 0 )
	{
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17609, pSock.language )); // Thou must resign from thy other guild first.
		return false;
	}
	else if((( new Date() ).getTime() - ( new Date( pChar.createdOn * 60 * 1000 )).getTime() / ( 1000 * 3600 * 24 )).toFixed( 0 ) < ageRequirement )
	{
		// Character too young compared to defined age requirement
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17611, pSock.language )); // You are too young to join my guild...
		return false;
	}

	// Guild-specific requirements
	// Only Thieves Guild have specific requirements to join by default
	switch( npcGuildMaster.npcGuild )
	{
		case 1: // The Guild of Arcane Arts
			break;
		case 2: // The Warrior's Guild
			break;
		case 3: // The Society of Thieves
			return ThievesGuildJoinReq( pChar, npcGuildMaster );
		case 4: // The League of Rangers
			break;
		case 5: // The Healer's Guild
			break;
		case 6: // The Mining Cooperative
			break;
		case 7: // The Merchant's Association
			break;
		case 8: // The Order of Engineers
			break;
		case 9: // The Society of Clothiers
			break;
		case 10: // The Maritime Guild
			break;
		case 11: // The Bardic Collegium
			break;
		case 12: // The Fellowship of Blacksmiths
			break;
		default:
			break;
	}
	return true;
}

function ThievesGuildJoinReq( pChar, npcGuildMaster )
{
	var pSock = pChar.socket;
	if( pChar.account.isYoung )
	{
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17612, pSock.language ), false, 0x3b2, 0, pChar.serial ); // You cannot be a member of the Thieves' Guild while you are Young.
	}
	else if( pChar.murdercount > 0 )
	{
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17613, pSock.language ), false, 0x3b2, 0, pChar.serial ); // This guild is for cunning thieves, not oafish cutthroats.
	}
	else if( pChar.skills.stealing < 600 )
	{
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17614, pSock.language ), false, 0x3b2, 0, pChar.serial ); // You must be at least a journeyman pickpocket to join this elite organization.
	}
	else
	{
		return true;
	}

	return false;
}

function HandleGuildServices( pChar, npcGuildMaster, iDropped )
{
	// Only Thieves Guild offer any additional services by default (disguise kit)
	switch( npcGuildMaster.npcGuild )
	{
		case 3: // The Society of Thieves
			return HandleThievesGuildServices( pChar, npcGuildMaster, iDropped );
		case 1: // The Guild of Arcane Arts
		case 2: // The Warrior's Guild
		case 4: // The League of Rangers
		case 5: // The Healer's Guild
		case 6: // The Mining Cooperative
		case 7: // The Merchant's Association
		case 8: // The Order of Engineers
		case 9: // The Society of Clothiers
		case 10: // The Maritime Guild
		case 11: // The Bardic Collegium
		case 12: // The Fellowship of Blacksmiths
		default:
			// Not sure what player is trying to achieve. Bounce back.
			npcGuildMaster.TextMessage( GetDictionaryEntry( 17568, pChar.socket.language )); // Why are you giving me this? No thanks.
			break;
	}

	// Bounce item back by default
	return 0;
}

function HandleThievesGuildServices( pChar, npcGuildMaster, iDropped )
{
	if( pChar.GetTempTag( "guildServiceRequest" ) == "disguisekit" )
	{
		var retVal = 0; // By default, bounce item back to player's inventory
		if( iDropped.amount >= disguiseKitCost )
		{
			// Create a disguise kit and hand it to player
			var disguiseKit = CreateDFNItem( pChar.socket, pChar, "disguise-kit", 1, "ITEM", true );
			if( ValidateObject( disguiseKit ))
			{
				if( iDropped.amount > disguiseKitCost )
				{
					// Player dropped more gold than needed. Subtract, and bounce rest back to player
					iDropped.amount -= disguiseKitCost;
				}
				else
				{
					// Player dropped exact amount needed
					iDropped.Delete();
					retVal = 2; // Do nothing else with item that was dropped
				}

				pChar.SetTempTag( "guildServiceRequest", null );
			}
		}
		else
		{
			// Not enough gold for a disguise kit, bounce back
			npcGuildMaster.TextMessage( GetDictionaryEntry( 17615, pChar.socket.language ), false, 0x3b2, 0, pChar.serial ); // That's not enough for a disguise kit, sorry.
		}

		return retVal;
	}
	else
	{
		// Not sure what player is trying to achieve. Bounce back.
		npcGuildMaster.TextMessage( GetDictionaryEntry( 17616, pChar.socket.language )); // Why are you giving me this? No thanks.
		return 0;
	}
}