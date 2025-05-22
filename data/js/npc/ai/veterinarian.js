const searchAmount = 4;
const searchInterval = 5000;
const searchRange = 8;
const searchTimer = 15000;

function inRange(myNPC, objInRange)
{
	//if (!ValidateObject(objInRange) || objInRange.npc || !objInRange.online || objInRange.dead || objInRange.isGM || objInRange.isCounselor)
	//	return;

	//var sock = objInRange.socket;
	//if (!ValidateObject(sock))
	//	return;

	// Skip if player already has the gump open
	if (objInRange.GetTempTag("vetGumpOpen") == true)
		return;

	// Show gump right away and set tag
	showVetResurrectGump(objInRange.socket);
	objInRange.SetTempTag("vetGumpOpen", true);

	// Get the current server clock, and if it exists, the time for when the last search was started
	// Compare the two, and the script will see if enough time has passed to initiate a new search
	// This also ensures that the script stays working even if the server saves in the middle of a
	// search, but crashes before the next save.
	var iTime = GetCurrentClock();
	var initSearchTime = parseInt( myNPC.GetTempTag( "initSearchTime" ));

	// If search has already been initiated, don't start a new search, unless an abnormal amount of time has passed
	if(( initSearchTime != null && initSearchTime != 0 ) && (((( iTime - initSearchTime ) / 1000 ) < searchTimer ) && !( initSearchTime > iTime )))
	{
		return;
	}

	if(((( iTime - initSearchTime ) / 1000 ) > searchTimer ) || initSearchTime > iTime )
	{
		myNPC.SetTempTag( "initSearchTime", iTime.toString() );
		myNPC.StartTimer( searchInterval, 1, true );
	}
}


function onTimer( srcChar, timerID )
{
	if( timerID < searchAmount )
	{
		//Search for nearby characters the specified amount of times
		AreaCharacterFunction( "searchForPlayers", srcChar, searchRange );
		srcChar.StartTimer( searchInterval, timerID + 1, true );
	}
}

// This function iterates through all characters within the specified radius in AreaCharacterFunction
// It then checks to make sure they are valid for msgs
function searchForPlayers( srcChar, trgChar, pSock )
{
	if( srcChar == trgChar )
		return false;

	//if( !ValidateObject( trgChar ) || trgChar.npc || !trgChar.online || trgChar.dead || trgChar.isGM || trgChar.isCounselor )
	//	return false;

	if (trgChar.GetTempTag("vetGumpOpen") == true)
		return false;

	var sock = trgChar.socket;
	if (ValidateObject(sock))
	{
		showVetResurrectGump(sock);
		trgChar.SetTempTag("vetGumpOpen", true);
	}
}

function showVetResurrectGump(socket)
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
		if( pet.GetTag("isPetDead") == true )
			petList.push(pet);
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
	gump.AddXMFHTMLGumpColor( 30, 20, 355, 35, 1113193, false, false, 0xFFFFFF );
	gump.AddXMFHTMLGumpColor( 30, 72, 345, 40, 1113284, false, false, 0x1DB2D );
	gump.AddXMFHTMLGumpColor( 20, 280, 345, 40, 1113286, false, false, 0x1DB2D );

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
		var fee = GetResFee( pet.serial );
		pUser.SetTempTag("serialPet", pet.serial);
		gump.AddRadio( 30, y, 0x25FF, ( i == 0 ? 1 : 0 ), pet.serial );
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

	var serialPet = parseInt( pUser.GetTempTag( "serialPet" ));
	if( pButton == 0 )
	{
		pUser.SetTempTag( "serialPet", null )
		pUser.SetTempTag("vetGumpOpen", null);
		return;
	}
	var selectedSerial = gumpData.getButton( 0 );
	var serial = CalcCharFromSer( serialPet );
	var cost = GetResFee( selectedSerial );
	var goldInPack = pUser.ResourceCount( 0x0EED, 0 );
	if( goldInPack < cost )
	{
		pSock.SysMessage( "You do not have enough gold on you to pay the fee." );
		return;
	}

	pUser.UseResource( cost, 0x0EED, 0 );
	onPetResurrect( pSock, serial );
	pSock.SysMessage( cost + " gold has been withdrawn from your backpack" );
	pSock.SysMessage( "Your pet has been resurrected." );
	pUser.SetTempTag("vetGumpOpen", null );
	pUser.SetTempTag( "serialPet", null )
}

function onPetResurrect( socket, deadPet )
{
	var petsAI = deadPet.GetTag( "PetsAI" );
	var petsHue = deadPet.GetTag( "PetsHue" );

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
		skill = 0;
	else
		skill = skill / 10;

	var fee = 100 + Math.pow( 1.1041, skill );
	fee = Math.floor( fee );

	if( fee > 30000 )
		fee = 30000;
	else if( fee < 100 )
		fee = 100;

	return fee;
}
