const petBondingEnabled = GetServerSetting( "PetBondingEnabled" );

function inRange(npc, player)
{
	if (!petBondingEnabled)
		return; // Skip if pet bonding is disabled

	if (!ValidateObject(player) || player.isItem || (player.isChar && (player.npc || !player.online || player.dead)))
		return;

	var serial = player.serial.toString();
	var spokenListRaw = npc.GetTag("vetSpokenList") || "";
	var spokenList = [];

	if (spokenListRaw.length > 0 && spokenListRaw != "0")
		spokenList = spokenListRaw.split(",");

	var found = false;
	for (var i = 0; i < spokenList.length; ++i)
	{
		if (spokenList[i] == serial)
		{
			found = true;
			break;
		}
	}

	if (!found)
		spokenList.push(serial);

	npc.SetTag("vetSpokenList", spokenList.join(","));
}

function onAISliver( npc )
{
	if( !petBondingEnabled )
	{
		return; // Skip if pet bonding is disabled
	}

	var now = GetCurrentClock();
	var lastTick = parseInt(npc.GetTag( "vetLastTick" )) || 0;
	var cooldown = 4000; // milliseconds

	if(( now - lastTick ) < cooldown )
		return; // Skip if cooldown hasn't passed

	var spokenListRaw = npc.GetTag( "vetSpokenList" );
	var spokenList = spokenListRaw.split( "," );

	npc.SetTag( "vetLastTick", now.toString() ); // Update cooldown tracker

	var cleanedList = [];

	for( var i = 0; i < spokenList.length; ++i )
	{
		var serial = parseInt( spokenList[i], 10 );
		if( isNaN( serial ))
			continue;

		var player = CalcCharFromSer( serial );
		if( !ValidateObject( player ))
			continue;

		// Only keep nearby players in memory
		if( npc.InRange( player, 10 ))
			cleanedList.push( serial.toString() );

		if( npc.InRange( player, 6 ) && player.GetTempTag("vetGumpOpen") != true )
		{
			var lastShown = parseInt( player.GetTempTag( "vetGumpLastShown_" + npc.serial )) || 0;
			var cooldown = 10000;

			if(( now - lastShown ) < cooldown )
				continue;

			player.SetTempTag( "vetGumpOpen", true );
			player.SetTempTag( "vetGumpLastShown_" + npc.serial, now.toString() );
			VetResurrectGump( player.socket, npc );
		}
	}

	// Only retain valid, recent, nearby serials
	npc.SetTag( "vetSpokenList", cleanedList.join( "," ));
}

function VetResurrectGump( socket )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var allPets = pUser.GetPetList(); // Get all pets and followers
	var petList = [];

	for( var i = 0; i < allPets.length; ++i )
	{
		var pet = allPets[i];
		if( !ValidateObject( pet ))
			continue;

		// Only include dead bonded pets
		if( pet.GetTag( "isPetDead" ) == true )
			petList.push( pet );
	}

	if( petList.length == 0 )
	{
		pUser.socket.SysMessage( "You have no dead bonded pets in need of resurrection." );
		return;
	}

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
	for( var i = 0; i < petList.length; ++i )
	{
		var pet = petList[i];
		var y = 102 + ( i * 35 );
		var fee = GetResFee( pet );

		gump.AddRadio( 30, y, 0x2600, 0, pet.serial );
		gump.AddText( 70, y + 5, 0x47E, pet.name + "  (" + fee + " gold)" );
	}

	gump.Send( pUser.socket );
	gump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( pButton == 0 || gumpData == 0)
	{
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	var selectedSerial =  gumpData.getButton( 0 );
	var petNpc = CalcCharFromSer( selectedSerial );
	var cost = GetResFee( petNpc );
	var goldInPack = pUser.ResourceCount( 0x0EED, 0 );

	if( !ValidateObject( petNpc ))
	{
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	if( goldInPack < cost )
	{
		pSock.SysMessage( "You do not have enough gold on you to pay the fee." );
		pUser.SetTempTag( "vetGumpOpen", null );
		return;
	}

	pUser.UseResource( cost, 0x0EED, 0 );
	PetResurrect( pSock, petNpc );
	pSock.SysMessage( cost + " gold has been withdrawn from your backpack" );
	pSock.SysMessage( "Your pet has been resurrected." );
	pUser.SetTempTag( "vetGumpOpen", null );
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
				newSkill = 0;
			deadPet.baseskills[skillName] = newSkill;
		}
	}

	TriggerEvent( 3108, "SendNpcGhostMode", socket, 0, deadPet.serial, 0  );
	deadPet.aitype = petsAI;
	deadPet.colour = petsHue;
	deadPet.target = null;
	deadPet.atWar = false;
	deadPet.attacker = null;
	deadPet.SetTag( "isPetDead", false );
}


function GetResFee( pet )
{
	var skill = pet.skillToTame;

	if( skill == null || skill <= 0 )
	{
		skill = 0;
	}
	else
	{
		skill = skill / 10;
	}

	var fee = 100 + Math.pow( 1.1041, skill );
	fee = Math.floor( fee );

	if( fee > 30000 )
	{
		fee = 30000;
	}
	else if( fee < 100 )
	{
		fee = 100;
	}

	return fee;
}