// Moongate.js (v1.5)
// Gump driven gates for location and facet travel 
// Originally written by Rais 7th Feb 02
// Contact: uox_rais@hotmail.com

// Example usage: 
// In game, do the following 
// 'add 0xf6c    (adds a gate)
// 'setscptrig 5011  (Set the script to run on the gate)
// (if you changed the ID in jse_fileassociations.scp, then use the ID you assigned) 

// Issue's: 
// Pets dont follow, but that's the same of item type 89 (world gates)

// Tag's Used: 
// cchar.ItemUsed hold the serial of the item used (the gate) 

// Updates: 
// 5/3/02: Made the actual gump routine a function, save's having the exact same code twice 
// 15/2/06: Included script with UOX3 by default (Xuri)
// 20/5/07: v1.3 - Added locations in Malas & Tokuno (Xuri)
// 22/11/21: v1.4 - Updated coordinates for Ilshenar gates, fixed a spelling error (Xuri)
// 25/11/21: v1.5 - Pets within range now follow a player through public moongates (Xuri)
//					Criminals, aggressors in PvP combat can no longer use public moongates (Xuri)

// Set to 0 to disable teleport locations for a certain map from appearing in the moongate menu
var enableFelucca = 1;
var enableTrammel = 0;
var enableIlshenar = 0;
var enableMalas = 0;
var enableTokuno = 0;

function onUseChecked( pUser, iUsed ) 
{ 
	// get users socket
	var srcSock = pUser.socket;

	if( srcSock == null || !CanUseGate( srcSock, pUser ))
		return false;

	//Are we using the gate allready?
	//Get the serial of the item we have just used
	var Serial = new Array;
	Serial[0] = iUsed.GetSerial( 1 );
	Serial[1] = iUsed.GetSerial( 2 );
	Serial[2] = iUsed.GetSerial( 3 );
	Serial[3] = iUsed.GetSerial( 4 );

	//Get the stored serial from tag
	var oldSerial = new Array;
	var iCheck = pUser.GetTempTag( "ItemUsed" );
	if( iCheck )
	{
		oldSerial = iCheck.split( ',' );
	}

	//Compare the 2, if the same, then we are allready using this gate
	if( oldSerial.length == 4 && Serial[0] == oldSerial[0] && Serial[1] == oldSerial[1] && Serial[2] == oldSerial[2] && Serial[3] == oldSerial[3] )
	{
		return false;
	}
	else
	{
		pUser.SetTempTag( "ItemUsed", Serial[0] + "," + Serial[1] + "," + Serial[2] + "," + Serial[3] );
	}

	// Create and display travel gump
	DisplayTravelGump( srcSock, pUser );
	return false;
}

function onCollide( srcSock, pUser, iUsed )
{
	if( srcSock == null || !CanUseGate( srcSock, pUser ))
		return false;

	var Serial = new Array;
	Serial[0] = iUsed.GetSerial( 1 );
	Serial[1] = iUsed.GetSerial( 2 );
	Serial[2] = iUsed.GetSerial( 3 );
	Serial[3] = iUsed.GetSerial( 4 );
	var oldSerial = new Array;
	var iCheck = pUser.GetTempTag( "ItemUsed" );
	if( iCheck )
	{
		oldSerial = iCheck.split( ',' );
	}
	if( oldSerial.length == 4 && Serial[0] == oldSerial[0] && Serial[1] == oldSerial[1] && Serial[2] == oldSerial[2] && Serial[3] == oldSerial[3] )
	{
		return;
	}
	else
	{
		pUser.SetTempTag( "ItemUsed", Serial[0] + "," + Serial[1] + "," + Serial[2] + "," + Serial[3] );
	}
	DisplayTravelGump( srcSock, pUser );
}

// Verify that user is allowed to use the gate
function CanUseGate( srcSock, pUser )
{
	if( pUser.criminal )
	{
		// Disallow moongate travel for players flagged as criminals
		if( srcSock )
		{
			srcSock.SysMessage( GetDictionaryEntry( 9112, srcSock.language )); // Thou'rt a criminal and cannot escape so easily.
		}
		return false;
	}
	else if( pUser.atWar && pUser.attackFirst )
	{
		// Disallow moongate travel for players flagged as aggressors
		var pTarget = pUser.target;
		if( ValidateObject( pTarget ))
		{
			if( !pTarget.npc && !pTarget.dead && pTarget.online && !pTarget.criminal && !pTarget.murderer )
			{
				if( srcSock )
				{
					srcSock.SysMessage( GetDictionaryEntry( 9113, srcSock.language )); // Wouldst thou flee during the heat of battle??
				}
				return false;
			}
		}
	}
	else
	{
		// Disallow moongate travel for players busy casting a spell
		if( pUser.GetTimer( Timer.SPELLTIME ) != 0 )
		{
			if( pUser.isCasting )
			{
				if( srcSock )
				{
					srcSock.SysMessage( GetDictionaryEntry( 9114, srcSock.language )); // You are too busy to do that at the moment.
				}
				return false;
			}
		}
	}

	return true;
}

function onGumpPress( srcSock, myButton )
{
	// Get character from socket
	var srcChar = srcSock.currentChar;

	// Range check. Its possible to call up the gate and run away
	// get the items's serial
	var oldSerial = new Array;
	var iCheck = srcChar.GetTempTag( "ItemUsed" );
	if( iCheck )
	{
		oldSerial = iCheck.split( ',' );

		// Convert serial into actual numbers not strings
		oldSerial[0] = parseInt( oldSerial[0] );
		oldSerial[1] = parseInt( oldSerial[1] );
		oldSerial[2] = parseInt( oldSerial[2] );
		oldSerial[3] = parseInt( oldSerial[3] );
	}

	// Find out what the item is from the serial
	var iUsed = null;
	if( oldSerial.length == 4 )
	{
		iUsed = CalcItemFromSer( oldSerial[0], oldSerial[1], oldSerial[2], oldSerial[3] );
	}

	if( !ValidateObject( iUsed ))
	{
		// Clear the tag so we can use gate again?
		srcChar.SetTempTag( "ItemUsed", null );
		return;
	}

	var inRange = iUsed.InRange( srcChar, 5 );

	// Clear the tag so we can use gate again?
	srcChar.SetTempTag( "ItemUsed", null );

	// Check var inRange and button (dont say anything if canceled)
	if( inRange == false && myButton >= 0)
	{
		srcSock.SysMessage( GetDictionaryEntry( 2749, srcSock.language )); // You have moved too far away to use this.
		return;
	}

	// Where we going... Fel or tram
	if( myButton >=5 && myButton <= 12 )
	{
		tWorld = 0;  //Going to Fel
	}
	if( myButton >= 13 && myButton <= 20 )
	{
		tWorld = 1;  //Going to Tram
	}

	var targetLocation = new Array();
	switch( myButton )
	{
		// Fel Tram (world 0 and 1)
		case 5:
		case 13:
		{
			// Britain
			targetLocation = [ 1336, 1997, 5, tWorld ];
			break;
		}
		case 6:
		case 14:
		{
			// Magincia
			targetLocation = [ 3563, 2139, 31, tWorld ];
			break;
		}
		case 7:
		case 15:
		{
			// Moonglow
			targetLocation = [ 4467, 1283, 5, tWorld ];
			break;
		}
		case 8:
		case 16:
		{
			// Skara Brae
			targetLocation = [ 643, 2067, 5, tWorld ];
			break;
		}

		case 9:
		case 17:
		{
			// Trinsic
			targetLocation = [ 1828, 2948, -20, tWorld ];
			break;
		}
		case 10:
		case 18:
		{
			// Vesper
			targetLocation = [ 2701, 692, 5, tWorld ];
			break;
		}
		case 11:
		case 19:
		{
			// Yew
			targetLocation = [ 771, 752, 5, tWorld ];
			break;
		}
		case 12:
		case 20:
		{
			// Jhelom
			targetLocation = [ 1499, 3771, 5, tWorld ];
			break;
		}

		//Iishenar
		case 21:
		{
			// Compassion
			targetLocation = [ 1215, 467, -13, 2 ];
			break;
		}
		case 22:
		{
			// Honesty
			targetLocation = [ 722, 1366, -60, 2 ];
			break;
		}
		case 23:
		{
			// Honor
			targetLocation = [ 744, 724, -28, 2 ];
			break;
		}
		case 24:
		{
			// Humility
			targetLocation = [ 281, 1016, 0, 2 ];
			break;
		}
		case 25:
		{
			// Justice
			targetLocation = [ 987, 1011, -32, 2 ];
			break;
		}
		case 26:
		{
			// Sacrifice
			targetLocation = [ 1174, 1286, -30, 2 ];
			break;
		}
		case 27:
		{
			// Sprirituality
			targetLocation = [ 1532, 1340, -3, 2 ];
			break;
		}
		case 28:
		{
			// Valor
			targetLocation = [ 528, 216, -45, 2 ];
			break;
		}
		case 29:
		{
			// Chaos
			targetLocation = [ 1721, 218, 96, 2 ];
			break;
		}
		//Malas
		case 30:
		{
			// New Luna
			targetLocation = [ 1015, 527, -65, 3 ];
			break;
		}
		case 31:
		{
			//Old Luna
			targetLocation = [ 1054, 358, -86, 3 ];
			break;
		}
		case 32:
		{
			// Umbra
			targetLocation = [ 1997, 1386, -85, 3 ];
			break;
		}
		//Tokuno Islands
		case 33:
		{
			// Isamu~Jima
			targetLocation = [ 1169, 998, 42, 4 ];
			break;
		}
		case 34:
		{
			// Makoto~Jima
			targetLocation = [ 801, 1204, 25, 4 ];
			break;
		}
		case 35:
		{
			// Homare~Jima
			targetLocation = [ 270, 628, 15, 4 ];
			break;
		}
		default:
			return;
	}

	// Teleport player's pets
	var petList = srcChar.GetPetList();
	for( var i = 0; i < petList.length; i++ )
	{
		var tempPet = petList[i];
		if( ValidateObject( tempPet ) && tempPet.InRange( srcChar, 24 ))
		{
			tempPet.Teleport( targetLocation[0], targetLocation[1], targetLocation[2], targetLocation[3] );
			tempPet.Follow( srcChar );
		}
	}

	// Teleport player
	srcChar.Teleport( targetLocation[0], targetLocation[1], targetLocation[2], targetLocation[3] );
}

function DisplayTravelGump( srcSock, pUser )
{
	// Make a noise as the gump opens
	pUser.SoundEffect( 0x58, false );

	var myGump = new Gump;

	// add a background
	myGump.AddPage( 1 );  //Page 1 Felucca
	myGump.AddBackground( 20, 20, 260, 245, 0x23f0 );
	myGump.AddText( 40, 40, 0, GetDictionaryEntry( 2750, srcSock.language )); // Pick your destination:
	//Cancel Button
	myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 );
	myGump.AddText( 80, 210, 0, GetDictionaryEntry( 2709, srcSock.language )); // CANCEL
	//Facet Buttons
	if( enableFelucca == 1 )
	{
		myGump.AddText( 70, 70, 12, "Felucca" );
		myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 );
		//Location Buttons
		myGump.AddText( 170, 70, 0, "Britian" );
		myGump.AddButton( 150, 75, 0x837, 1, 0, 5 );
		myGump.AddText( 170, 90, 0, "Magincia" );
		myGump.AddButton( 150, 95, 0x837, 1, 0, 6 );
		myGump.AddText( 170, 110, 0, "Moonglow" );
		myGump.AddButton( 150, 115, 0x837, 1, 0, 7 );
		myGump.AddText( 170, 130, 0, "Skara Brea" );
		myGump.AddButton( 150, 135, 0x837, 1, 0, 8 );
		myGump.AddText( 170, 150, 0, "Trinsic" );
		myGump.AddButton( 150, 155, 0x837, 1, 0, 9 );
		myGump.AddText( 170, 170, 0, "Vesper" );
		myGump.AddButton( 150, 175, 0x837, 1, 0, 10 );
		myGump.AddText( 170, 190, 0, "Yew" );
		myGump.AddButton( 150, 195, 0x837, 1, 0, 11 );
		myGump.AddText( 170, 210, 0, "Jhelom" );
		myGump.AddButton( 150, 215, 0x837, 1, 0, 12 );
	}
	if( enableTrammel == 1 )
	{
		myGump.AddText( 70, 90, 0, "Trammel" );
		myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
	}
	if( enableIlshenar == 1 )
	{
		myGump.AddText( 70, 110, 0, "Iishenar" );
		myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
	}
	if( enableMalas == 1 )
	{
		myGump.AddText( 70, 130, 0, "Malas" );
		myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );
	}
	if( enableTokuno == 1 )
	{
		myGump.AddText( 70, 150, 0, "Tokuno Islands" );
		myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
	}

	//PAGE 2
	myGump.AddPage( 2 );  //Page 2 Trammel
	myGump.AddBackground( 20, 20, 260, 245, 0x23f0 );
	myGump.AddText( 40, 40, 0, GetDictionaryEntry( 2750, srcSock.language )); // Pick your destination:

	myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 );
	myGump.AddText( 80, 210, 0, GetDictionaryEntry( 2709, srcSock.language )); // CANCEL

	if( enableFelucca == 1 )
	{
		myGump.AddText( 70, 70, 0, "Felucca" );
		myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 );
	}
	if( enableTrammel == 1 )
	{
		myGump.AddText( 70, 90, 12, "Trammel" );
		myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
		myGump.AddText( 170, 70, 0, "Britian" );
		myGump.AddButton( 150, 75, 0x837, 1, 0, 13 );
		myGump.AddText( 170, 90, 0, "Magincia" );
		myGump.AddButton( 150, 95, 0x837, 1, 0, 14 );
		myGump.AddText( 170, 110, 0, "Moonglow" );
		myGump.AddButton( 150, 115, 0x837, 1, 0, 15 );
		myGump.AddText( 170, 130, 0, "Skara Brea" );
		myGump.AddButton( 150, 135, 0x837, 1, 0, 16 );
		myGump.AddText( 170, 150, 0, "Trinsic" );
		myGump.AddButton( 150, 155, 0x837, 1, 0, 17 );
		myGump.AddText( 170, 170, 0, "Vesper" );
		myGump.AddButton( 150, 175, 0x837, 1, 0, 18 );
		myGump.AddText( 170, 190, 0, "Yew" );
		myGump.AddButton( 150, 195, 0x837, 1, 0, 19 );
		myGump.AddText( 170, 210, 0, "Jhelom" );
		myGump.AddButton( 150, 215, 0x837, 1, 0, 20 );
	}
	if( enableIlshenar == 1 )
	{
		myGump.AddText( 70, 110, 0, "Iishenar" );
		myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
	}
	if( enableMalas == 1 )
	{
		myGump.AddText( 70, 130, 0, "Malas" );
		myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );
	}
	if( enableTokuno == 1 )
	{
		myGump.AddText( 70, 150, 0, "Tokuno Islands" );
		myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
	}

	//PAGE 3
	myGump.AddPage( 3 );  //Page 3 IIsh
	myGump.AddBackground( 20, 20, 260, 245, 0x23f0 );
	myGump.AddText( 40, 40, 0, GetDictionaryEntry( 2750, srcSock.language )); // Pick your destination:

	myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 );
	myGump.AddText( 80, 210, 0, GetDictionaryEntry( 2709, srcSock.language )); // CANCEL

	if( enableFelucca == 1 )
	{
		myGump.AddText( 70, 70, 0, "Felucca" );
		myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 );
	}
	if( enableTrammel == 1 )
	{
		myGump.AddText( 70, 90, 0, "Trammel" );
		myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
	}
	if( enableIlshenar == 1 )
	{
		myGump.AddText( 70, 110, 12, "Iishenar" );
		myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );

		myGump.AddText( 170, 70, 0, "Compassion" );
		myGump.AddButton( 150, 75, 0x837, 1, 0, 21 );
		myGump.AddText( 170, 90, 0, "Honesty" );
		myGump.AddButton( 150, 95, 0x837, 1, 0, 22 );
		myGump.AddText( 170, 110, 0, "Honor" );
		myGump.AddButton( 150, 115, 0x837, 1, 0, 23 );
		myGump.AddText( 170, 130, 0, "Humility" );
		myGump.AddButton( 150, 135, 0x837, 1, 0, 24 );
		myGump.AddText( 170, 150, 0, "Justice" );
		myGump.AddButton( 150, 155, 0x837, 1, 0, 25 );
		myGump.AddText( 170, 170, 0, "Sacrifice" );
		myGump.AddButton( 150, 175, 0x837, 1, 0, 26 );
		myGump.AddText( 170, 190, 0, "Sprirituality" );
		myGump.AddButton( 150, 195, 0x837, 1, 0, 27 );
		myGump.AddText( 170, 210, 0, "Valor" );
		myGump.AddButton( 150, 215, 0x837, 1, 0, 28 );
		myGump.AddText( 170, 230, 0, "Chaos" );
		myGump.AddButton( 150, 235, 0x837, 1, 0, 29 );
	}
	if( enableMalas == 1 )
	{
		myGump.AddText( 70, 130, 0, "Malas" );
		myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );
	}
	if( enableTokuno == 1 )
	{
		myGump.AddText( 70, 150, 0, "Tokuno Islands" );
		myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
	}

	//PAGE 4
	myGump.AddPage( 4 );  //Page 4 Malas
	myGump.AddBackground( 20, 20, 260, 245, 0x23f0 );
	myGump.AddText( 40, 40, 0, GetDictionaryEntry( 2750, srcSock.language )); // Pick your destination:

	myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 );
	myGump.AddText( 80, 210, 0, GetDictionaryEntry( 2709, srcSock.language )); // CANCEL

	if( enableFelucca == 1 )
	{
		myGump.AddText( 70, 70, 0, "Felucca" );
		myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 );
	}
	if( enableTrammel == 1 )
	{
		myGump.AddText( 70, 90, 0, "Trammel" );
		myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
	}
	if( enableIlshenar == 1 )
	{
		myGump.AddText( 70, 110, 0, "Iishenar" );
		myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
	}
	if( enableMalas == 1 )
	{
		myGump.AddText( 70, 130, 12, "Malas" );
		myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );

		myGump.AddText( 170, 70, 0, "New Luna" );
		myGump.AddButton( 150, 75, 0x837, 1, 0, 30 );
		myGump.AddText( 170, 90, 0, "Old Luna" );
		myGump.AddButton( 150, 95, 0x837, 1, 0, 31 );
		myGump.AddText( 170, 110, 0, "Umbra" );
		myGump.AddButton( 150, 115, 0x837, 1, 0, 32 );
	}
	if( enableTokuno == 1 )
	{
		myGump.AddText( 70, 150, 0, "Tokuno Islands" );
		myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
	}

	//PAGE 5
	myGump.AddPage( 5 );  //Page 4 Tokuno Islands
	myGump.AddBackground( 20, 20, 260, 245, 0x23f0 );
	myGump.AddText( 40, 40, 0, GetDictionaryEntry( 2750, srcSock.language )); // Pick your destination:

	myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 );
	myGump.AddText( 80, 210, 0, GetDictionaryEntry( 2709, srcSock.language )); // CANCEL

	if( enableFelucca == 1 )
	{
		myGump.AddText( 70, 70, 0, "Felucca" );
		myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 );
	}
	if( enableTrammel == 1 )
	{
		myGump.AddText( 70, 90, 0, "Trammel" );
		myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
	}
	if( enableIlshenar == 1 )
	{
		myGump.AddText( 70, 110, 0, "Iishenar" );
		myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
	}
	if( enableMalas == 1 )
	{
		myGump.AddText( 70, 130, 0, "Malas" );
		myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );
	}
	if( enableTokuno == 1 )
	{
		myGump.AddText( 70, 150, 12, "Tokuno Islands" );
		myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
		myGump.AddText( 170, 70, 0, "Isamu~Jima" );
		myGump.AddButton( 150, 75, 0x837, 1, 0, 33 );
		myGump.AddText( 170, 90, 0, "Makoto~Jima" );
		myGump.AddButton( 150, 95, 0x837, 1, 0, 34 );
		myGump.AddText( 170, 110, 0, "Homare~Jima" );
		myGump.AddButton( 150, 115, 0x837, 1, 0, 35 );
	}
	myGump.Send( srcSock );
}