const petBondingEnabled = GetServerSetting( "PetBondingEnabled" );
const vetGumpCooldown = 4000; // milliseconds
const scriptID = 3109;

function inRange( npcVet, player )
{
	if( !petBondingEnabled )
		return; // Skip if pet bonding is disabled

	if( !ValidateObject( player ) || player.isItem || ( player.isChar && ( player.npc || !player.online || player.dead )))
		return;

	var nearbyPlayersList = [];
	var nearbyPlayersListRaw = npcVet.GetTempTag( "vetNearbyPlayersList" );
	if( nearbyPlayersListRaw != 0 && nearbyPlayersListRaw.length > 0 )
	{
		nearbyPlayersList = nearbyPlayersListRaw.split( "," );
	}

	var serial = player.serial.toString();
	var found = false;
	for( var i = 0; i < nearbyPlayersList.length; ++i )
	{
		if( nearbyPlayersList[i] == serial )
		{
			found = true;
			break;
		}
	}

	if( !found )
	{
		nearbyPlayersList.push( serial );
	}

	npcVet.SetTempTag( "vetNearbyPlayersList", nearbyPlayersList.join( "," ));
}

function onAISliver( npcVet )
{
	if( !petBondingEnabled )
	{
		return; // Skip if pet bonding is disabled
	}

	var now = GetCurrentClock();
	var lastTick = parseInt( npcVet.GetTempTag( "vetLastTick" ));
	var cooldown = vetGumpCooldown; // milliseconds

	if(( now - lastTick ) < cooldown )
		return; // Skip if cooldown hasn't passed

	npcVet.SetTempTag( "vetLastTick", now.toString() ); // Update cooldown tracker

	var nearbyPlayersListRaw = npcVet.GetTempTag( "vetNearbyPlayersList" );
	if( nearbyPlayersListRaw == 0 || nearbyPlayersListRaw.length == 0 )
		return;

	// Iterate over nearby players, see if they have any dead pets
	var nearbyPlayersList = nearbyPlayersListRaw.split( "," );
	var cleanedList = [];
	for( var i = 0; i < nearbyPlayersList.length; ++i )
	{
		var serial = parseInt( nearbyPlayersList[i], 10 );
		if( isNaN( serial ))
			continue;

		var player = CalcCharFromSer( serial );
		if( !ValidateObject( player ))
			continue;

		// Only keep nearby players in memory
		if( npcVet.InRange( player, 24 ))
		{
			cleanedList.push( serial.toString() );
		}

		// Only show gump if player is within range
		if( npcVet.InRange( player, 6 ) && player.GetTempTag( "vetGumpOpen" ) != true )
		{
			var lastShown = parseInt( player.GetTempTag( "vetGumpLastShown" ));
			var cooldown = 10000;

			if(( now - lastShown ) < cooldown )
				continue;

			// Check if player has any dead bonded pets that require resurrection
			var deadPetList = GetDeadPetList( player );

			// Only open gump if there's a dead pet nearby to resurrect
			if( deadPetList.length > 0 )
			{
				player.SetTempTag( "vetGumpOpen", true );
				player.SetTempTag( "vetGumpLastShown", now.toString() );
				VetResurrectGump( player.socket, deadPetList );
			}
		}
		else if( !npcVet.InRange( player, 6 ))
		{
			// Force-close resurrection gump if they're out of range
			player.SetTempTag( "vetGumpOpen", null );

			if( player.socket != null )
			{
				let gumpID = scriptID + 0xffff;
				player.socket.CloseGump( gumpID, 0 );
			}
		}
	}

	// Only retain valid, recent, nearby serials
	npcVet.SetTempTag( "vetNearbyPlayersList", cleanedList.join( "," ));
}

// Let players re-trigger the resurrection gump by talking to the vet
function onSpeech( strSaid, pTalking, pTalkingTo )
{
	if( !pTalking.npc )
	{
		// Reset vet/player's gump cooldown if player has dead pets to resurrect
		var deadPetList = GetDeadPetList( pTalking );
		if( deadPetList.length > 0 )
		{
			pTalkingTo.SetTempTag( "vetLastTick", null );
			pTalking.SetTempTag( "vetGumpLastShown", null );
		}
	}

	return 0;
}

function GetDeadPetList( player )
{
	var deadPetList = [];
	var allPets = player.GetPetList();

	// Iterate over list of pets
	for( var i = 0; i < allPets.length; ++i )
	{
		var tempPet = allPets[i];
		if( !ValidateObject( tempPet ))
			continue;

		// Only include dead bonded pets that are within short range of the player
		if( tempPet.GetTag( "isPetDead" ) && player.InRange( tempPet, 6 ))
		{
			deadPetList.push( tempPet );
		}
	}

	return deadPetList;
}

function VetResurrectGump( socket, deadPetList )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var gump = new Gump;
	gump.AddPage( 0 );

	// Background
	gump.AddGump( 0, 0, 0xE10 );
	gump.AddTiledGump(0, 14, 15, 380, 0xE13);
	gump.AddTiledGump( 380, 14, 14, 380, 0xE15 );
	gump.AddGump( 0, 381, 0xE16 );
	gump.AddTiledGump(15, 381, 370, 16, 0xE17 );
	gump.AddTiledGump( 15, 0, 370, 16, 0xE11 );
	gump.AddGump( 380, 0, 0xE12 );
	gump.AddGump( 380, 381, 0xE18 );
	gump.AddTiledGump( 15, 15, 365, 370, 0xA40 );

	// Text
	gump.AddXMFHTMLGumpColor( 30, 20, 355, 35, 1113193, false, false, 0xFFFFFF );//Ah, thine pet seems to be in dire condition! I can help thee, but must charge a small fee...
	gump.AddXMFHTMLGumpColor( 30, 72, 345, 40, 1113284, false, false, 0x1DB2D );//Please select the pet you wish to resurrect:
	gump.AddXMFHTMLGumpColor( 20, 280, 345, 40, 1113286, false, false, 0x1DB2D );//<CENTER>Your pet will suffer 0.2 points of skill-loss if resurrected in this manner.</CENTER>

	// Horizontal lines
	gump.AddTiledGump( 95, 62, 200, 1, 0x23C5 );
	gump.AddTiledGump( 15, 325, 365, 1, 0x2393 );

	// Buttons
	gump.AddButton( 110, 343, 0xF7, 0xF8, 1, 0, 1 ); // OK
	gump.AddButton( 230, 343, 0xF2, 0xF1, 1, 0, 0 ); // Cancel

	// Frame
	gump.AddTiledGump( 15, 14, 365, 1, 0x2393 );
	gump.AddTiledGump( 380, 14, 1, 370, 0x2391 );
	gump.AddTiledGump( 15, 385, 365, 1, 0x2393 );
	gump.AddTiledGump( 15, 14, 1, 370, 0x2391 );
	gump.AddTiledGump( 0, 0, 395, 1, 0x23C5 );
	gump.AddTiledGump( 394, 0, 1, 397, 0x23C3 );
	gump.AddTiledGump( 0, 396, 395, 1, 0x23C5 );
	gump.AddTiledGump( 0, 0, 1, 397, 0x23C3 );

	// Radio buttons for each pet
	for( var i = 0; i < deadPetList.length; ++i )
	{
		var deadPet = deadPetList[i];
		var y = 102 + ( i * 35 );
		var resFee = GetResFee( deadPet );

		gump.AddRadio( 30, y, 0x2600, 0, deadPet.serial );
		gump.AddText( 70, y + 5, 0x47E, deadPet.name + "  (" + resFee + " gold)" );
	}

	gump.Send( pUser.socket );
	gump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( pButton == 0 || gumpData == 0 )
	{
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	var selectedSerial =  gumpData.getButton( 0 );
	if( selectedSerial == -1 )
	{
		// No pet was selected in menu
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	var petNpc = CalcCharFromSer( selectedSerial );
	var resFee = GetResFee( petNpc );
	var goldInPack = pUser.ResourceCount( 0x0EED, 0 );

	if( !ValidateObject( petNpc ))
	{
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	if( goldInPack < resFee )
	{
		pSock.SysMessage( GetDictionaryEntry( 19330, pSock.language )); // You do not have enough gold on you to pay the fee.
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	var now = GetCurrentClock();
	var deathTime = parseInt( petNpc.GetTempTag( "bondedPetDeathTime" )) || 0;
	var waitTime = 10 * 60 * 1000; // 10 minutes in ms

	if(( now - deathTime ) < waitTime)
	{
		pSock.SysMessage( GetDictionaryEntry( 19340, pSock.language )); // That creature’s spirit lacks cohesion. Try again in a few minutes.
		return;
	}

	pUser.UseResource( resFee, 0x0EED, 0 );
	PetResurrect( pSock, petNpc );
	var tempMsg = GetDictionaryEntry( 19331, pSock.language ); // %i gold has been withdrawn from your backpack.
	tempMsg = ( tempMsg.replace( /%i/gi, resFee.toString() ));
	pSock.SysMessage( tempMsg );
	pSock.SysMessage( GetDictionaryEntry( 19332, pSock.language )); // Your pet has been resurrected.
	pUser.SetTempTag( "vetGumpOpen", null );

	// Check if player has any additional dead bonded pets that require resurrection
	var deadPetList = GetDeadPetList( pUser );
	if( deadPetList.length > 0 )
	{
		pUser.SetTempTag( "vetGumpLastShown", null );
	}
}

function PetResurrect( socket, deadPet )
{
	var skillList = [
	"alchemy", "anatomy", "animallore", "itemid", "armslore", "parrying",
	"begging", "blacksmithing", "bowcraft", "peacemaking", "camping",
	"carpentry", "cartography", "cooking", "detectinghidden", "enticement",
	"evaluatingintel", "healing", "fishing", "forensics", "herding",
	"hiding", "provocation", "inscription", "lockpicking", "magery",
	"magicresistance", "tactics", "snooping", "musicianship", "poisoning",
	"archery", "spiritspeak", "stealing", "tailoring", "taming", "tasteid",
	"tinkering", "tracking", "veterinary", "swordsmanship", "macefighting",
	"fencing", "wrestling", "lumberjacking", "mining", "meditation",
	"stealth", "removetrap", "necromancy", "focus", "chivalry", "bushido",
	"ninjitsu", "spellweaving", "imbuing", "mysticism", "throwing"
	];

	var petsAI = deadPet.GetTag( "PetsAI" );
	var petsHue = deadPet.GetTag( "PetsHue" );

	// Apply skill loss: lose 0.2 (2 points on UOX3 scale) per skill
	for( var i = 0; i < skillList.length; ++i )
	{
		var skillName = skillList[i];
		var curSkill = deadPet.baseskills[skillName];
		if( typeof curSkill == "number" && curSkill > 0 )
		{
			var newSkill = curSkill - 2;
			if( newSkill < 0 )
			{
				newSkill = 0;
			}
			deadPet.baseskills[skillName] = newSkill;
		}
	}

	TriggerEvent( 3108, "SendNpcGhostMode", socket, 0, deadPet.serial, 0 );
	deadPet.aitype = petsAI;
	deadPet.colour = petsHue;
	deadPet.target = null;
	deadPet.atWar = false;
	deadPet.attacker = null;
	deadPet.SetTag( "isPetDead", false );
	deadPet.SetTempTag( "deathTime", null ); // Optional cleanup
}

function GetResFee( deadPet )
{
	var skillToTame = deadPet.skillToTame;
	if( skillToTame == null || skillToTame <= 0 )
	{
		skillToTame = 0;
	}
	else
	{
		skillToTame = skillToTame / 10;
	}

	var resFee = 100 + Math.pow( 1.1041, skillToTame );
	resFee = Math.floor( resFee );
	if( resFee > 30000 )
	{
		resFee = 30000;
	}
	else if( resFee < 100 )
	{
		resFee = 100;
	}

	return resFee;
}