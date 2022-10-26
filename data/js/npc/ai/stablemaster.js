// Stable-master, by Xuri (xuri@xoduz.org)
// Version: 1.2
// Last Updated: September 10th 2022
//
// For context menu support, stable master needs to be assigned NPCAI 9 (AI_STABLEMASTER)
// TODO: Expand script with option for having localized stables - current script is global,
// and it doesn't matter which stable-master you speak to, they will all have your pets

// SETTINGS:
var stableCost = 90; // The cost to stable a pet for a week
var maxFollowers = 5; // The maximum amount of followers/pets a character can have at a given time
var maxStableDays = 0; // The maximum amount of days a pet will be kept safe in the stables - 0 (forever) is default
var NPCPause = 30000; // Time in seconds * 1000 the NPC pauses in his tracks for when players interact

// ingame coordinates where stabled pets are moved (should be unreachable for players!!!)
var stableX = 8000;
var stableY = 8000;
var stableZ = 0;

// Ignore these
var maxStabledPets = 0; // The amount of pets a character can have in the stables, determined later on by player skills
var totalStabledPets = 0; // The total amount of pets a character has in the stables, retrieved later on from player

// Script ID assigned to this script in jse_fileassociations.scp. Used to prevent multiple instances of same gump being opened
var scriptID = 3105;

const maxControlSlots = GetServerSetting( "MaxControlSlots" );

function onSpeech( strSaid, pTalking, stableMaster )
{
	var pSock = pTalking.socket;

	// Verify that a socket (aka connected player) exists, and
	// that the player talking is within 8 tiles of the stableMaster.
	// Else, ignore the speech
	if( pSock != null && pTalking.InRange( stableMaster, 8 ))
	{
		// Turn the stableMaster towards the one talking
		stableMaster.TurnToward( pTalking );

		// Let's pause the NPC in his tracks for X seconds (defined at top of script) so he doesn't wander off
		stableMaster.SetTimer( Timer.MOVETIME, NPCPause );

		// Let's check the player's skills and set the max amount of slots he
	   	// has available in the stables depending on those
	   	CalcStableSlotBonus( pTalking );

		// Iterate through any triggerwords said by the player
		for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
		{
			switch( trigWord )
			{
				case 0x0008: // "stable"
				{
					// Save stableMaster as a tempObj on the player, for retrieval later
					pTalking.tempObj = stableMaster;
					if( maxStableDays == 0 )
					{
						stableMaster.TextMessage( "I charge " + stableCost + " per pet you want to stable. I will withdraw it from thy bank account." );
					}
					else
					{
						stableMaster.TextMessage( "I charge " + stableCost + " per pet for " + maxStableDays + " days of stable time. I will withdraw it from thy bank account." );
					}


					pSock.CustomTarget( 0, GetDictionaryEntry( 2099, pSock.language )); // Which animal wouldst thou like to stable here?
					return 1;
				}
				case 0x0009: // "claim"
				{
					totalStabledPets = pTalking.GetTag( "totalStabledPets" );
					if( totalStabledPets > 0 )
					{
						if( strSaid.split( " " ).length == 1 ) // only claim, nothing else
						{
							// claim all pets
							ClaimAllPets( pTalking, stableMaster );
						}
						else if( !ClaimPetByName( pTalking, stableMaster, strSaid ))
						{
							// Display the gump with all the pets
							totalStabledPets = pTalking.GetTag( "totalStabledPets" );
							if( totalStabledPets > 0 )
							{
								pTalking.SetTag( "stableMasterSerial", stableMaster.serial );
								stableMaster.TextMessage( GetDictionaryEntry( 2100, pSock.language )); // I currently have the following pets of yours stabled right now...

								// Ok, player has animals stabled - let's give him the list of pets
								ClaimGump( pTalking, stableMaster );
							}
						}
					}
					else
					{
						stableMaster.TextMessage( GetDictionaryEntry( 2101, pSock.language )); // But I have none of your pets stabled with me at the moment!
					}
					return 1;
					break;
				}
				default:
					return 0;
			}
		}

		return 0;
	}
}

function ClaimAllPets( pTalking, stableMaster, strSaid )
{
	var i = 0;
	var petCount = 0;
	var controlSlotsUsed = pTalking.controlSlotsUsed;
	for( i = 0; i < maxStabledPets; i++ )
	{
		var tempPetSerial = pTalking.GetTag( "stabledPet" + i );
		if( tempPetSerial != null && tempPetSerial != "0" )
		{
			var tempPet = CalcCharFromSer( tempPetSerial );
			if( ValidateObject( tempPet ))
			{
				if( maxControlSlots > 0 && ( controlSlotsUsed + tempPet.controlSlots <= maxControlSlots ))
				{
					petCount++;
					ReleasePet( tempPet, i, stableMaster, pTalking, false );
				}
			}
		}
	}

	if( petCount > 0 )
	{
		var hour = GetHour();
		if( hour < 6 || hour > 18 )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2102, pTalking.socket.language )); // Here you go... and have a nice night!
		}
		else
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2103, pTalking.socket.language )); // Here you go... and a good day to you!
		}
	}
}

// Function to check if a pet by the name given by the player exist in the stables,
// and if it does, release it directly without displaying the menu
function ClaimPetByName( pTalking, stableMaster, strSaid )
{
	var splitString = strSaid.toUpperCase().split( "CLAIM " )[1];
	if( !splitString || ( splitString.toUpperCase() == "LIST" || splitString[0].toUpperCase() == "LIST" ))
		return false;

	var i = 0;
	var petFound = false;
	var j = maxStabledPets - 1;
	var controlSlotsUsed = pTalking.controlSlotsUsed;
	for( i = 0; i <= j; i++ )
	{
		var tempPet = pTalking.GetTag( "stabledPet" + i );
		if( tempPet != null && tempPet != "0" )
		{
			var petObj = CalcCharFromSer( tempPet );
			if( petObj )
			{
				if( petObj.name.toUpperCase() == splitString.toUpperCase() )
				{
					if( maxControlSlots > 0 && ( controlSlotsUsed + tempPet.controlSlots <= maxControlSlots ))
					{
						ClaimPet( pTalking, i, stableMaster );
						petFound = true;
						return true;
					}
					else
					{
						pTalking.socket.SysMessage( GetDictionaryEntry( 2390, pTalking.socket.language )); // That would exceed your maximum pet control slots.
						return false;
					}
				}
			}
		}
	}

	stableMaster.TextMessage( GetDictionaryEntry( 2104, pTalking.socket.language )); // I have no pet that answers to that name in my stables.
	return false;
}

function ClaimGump( pUser, stableMaster )
{
	var pSock = pUser.socket;
	var gumpID = scriptID + 0xffff;
	pSock.CloseGump( gumpID, 0 );

   	totalStabledPets = pUser.GetTag( "totalStabledPets" );

   	// extraBGSize is used to resize the claim-pet gump based on how many slots
   	// the user has available - calculated above
	var extraBGSize = 110 + ( maxStabledPets * 20 );

   	var ClaimGump = new Gump;
	ClaimGump.AddPage( 0 );
	ClaimGump.AddBackground( 0, 0, 200, extraBGSize, 9250 );
	ClaimGump.AddButton( 165, 10, 5052, 1, 0, 0 ); // Close Menu Button
	ClaimGump.AddCheckerTrans( 0, 0, 200, extraBGSize );

	ClaimGump.AddText( 20, 15, 50, "- " + GetDictionaryEntry( 2105, pSock.language ) + " -" ); // StableMaster Menu
	ClaimGump.AddHTMLGump( 20, 35, 200, 20, 0, 0, "<BASEFONT COLOR=#FFFFFF>" + GetDictionaryEntry( 2106, pSock.language ) + "</BASEFONT>" ); // Select a pet to retrieve
	ClaimGump.AddHTMLGump( 20, 55, 200, 20, 0, 0, "<BASEFONT COLOR=#FFFFFF>" + GetDictionaryEntry( 2107, pSock.language ) + ":</BASEFONT>" ); // from the stables
	ClaimGump.AddHTMLGump( 20, 75, 200, 20, 0, 0, "<BASEFONT COLOR=#FFFFFF>- - - - - - - - - - - - </BASEFONT>" );

	var i = 0;
	var j = maxStabledPets - 1;
	for( i = 0; i <= j; i++ )
	{
		var tempPet = pUser.GetTag( "stabledPet" + i );
		if( tempPet != null && tempPet != "0" )
		{
			var petObj = CalcCharFromSer( tempPet );
			if( petObj )
			{
				ClaimGump.AddButton( 20, 99 + ( i * 20 ), 0x845, 1, 0, i + 1 );
				ClaimGump.AddHTMLGump( 42, 95 + ( i * 20 ), 200, 18, 0, 0, "<BASEFONT COLOR=#C0C0EE>" +petObj.name+"</BASEFONT>" );
			}
		}
		else
		{
			ClaimGump.AddHTMLGump( 25, 95 + ( i * 20 ), 200, 18, 0, 0, "<BASEFONT COLOR=#C0C0EE><SMALL><EM>(" + GetDictionaryEntry( 2108, pSock.language ) + " " +(i+1)+")</EM></SMALL></BASEFONT>" ); // Empty storage slot
		}
	}

    ClaimGump.Send( pUser.socket );
	ClaimGump.Free();
}

function CalcStableSlotBonus( pUser )
{
	// Let's check relevant player-skills and assign him a number of stable-slots depending on those skills
	var stableModifier = pUser.skills.taming + pUser.skills.veterinary + pUser.skills.animallore;
   	if( stableModifier <= 160 )
   	{
   		maxStabledPets = 2;
   	}
   	else if( stableModifier > 160 && stableModifier <= 199.9 )
   	{
   		maxStabledPets = 3;
   	}
   	else if( stableModifier >= 200 && stableModifier <= 239.9 )
   	{
   		maxStabledPets = 4;
   	}
   	else if( stableModifier >= 240 )
   	{
   		maxStabledPets = 5;
   	}

   	// Increase max stable-slots available by 1 for each of the following skills
   	// he has at 100.0 or above:
   	if( pUser.skills.taming >= 1000 )
   	{
   		maxStabledPets++;
   	}
   	if( pUser.skills.veterinary >= 1000 )
   	{
   		maxStabledPets++;
   	}
   	if( pUser.skills.animallore >= 1000 )
   	{
   		maxStabledPets++;
   	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var stableMasterSer = pUser.GetTag( "stableMasterSerial" );

	if( stableMasterSer )
	{
		var stableMaster = CalcCharFromSer( stableMasterSer );
		if( pUser.petCount < maxFollowers )
		{
			switch( pButton )
			{
				case 0: // Closes gump
					break;
				default:
				{
					ClaimPet( pUser, pButton - 1, stableMaster );
					break;
				}
			}
		}
		else
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2109, pSock.language )); // Your pet remains in the stables because you have too many followers!
		}
	}
}

function ClaimPet( pUser, petNum, stableMaster )
{
	if( pUser && petNum >= 0 )
	{
		if( pUser.InRange( stableMaster, 8 ))
		{
			var tempPet = pUser.GetTag( "stabledPet" + petNum );
			if( tempPet != null && tempPet != "0" )
			{
				var petObj = CalcCharFromSer( tempPet );
				if( ValidateObject( petObj ))
				{
					if( maxControlSlots > 0 && ( pUser.controlSlotsUsed + petObj.controlSlots > maxControlSlots ))
					{
						pUser.socket.SysMessage( GetDictionaryEntry( 2390, pUser.socket.language )); // That would exceed your maximum pet control slots.
						return;
					}
					var totalStabledPets = pUser.GetTag( "totalStabledPets" );
					var stableTimeAt = petObj.GetTag( "stableTimeAt" );
					var maxStableTime = maxStableDays * 86402350; // 86402350 should be approx 24 hours
					var strokeOfLuck = RandomNumber( 1, 10 );

					// If the time passed since pet was stabled is less than the maximum time allowed,
					// or if the player has a stroke of luck even though time has passed max...
					// ...then let him claim the pet
					if(( GetCurrentClock() - stableTimeAt < maxStableTime ) || maxStableTime == 0 )
					{
						//Let's release the pet
						ReleasePet( petObj, petNum, stableMaster, pUser, true );
					}
					else
					{
						// Let's give the user a small chance of retrieving his pet even after the max time has passed
						if( strokeOfLuck >= 8 )
						{
							stableMaster.TextMessage( GetDictionaryEntry( 2110, pUser.socket.language )); // You're in luck today! Even though you're overdue to retrieve your pet, it is still alive!
							ReleasePet( petObj, petNum, stableMaster, pUser, true );
						}
						else
						{
							// The pet was stabled for too long, and ...died ;P
							totalStabledPets = totalStabledPets - 1;
							pUser.SetTag( "totalStabledPets", totalStabledPets );
							pUser.SetTag( "stabledPet" + petNum, null );
							pUser.SetTag( "stableMasterSerial", null );
							petObj.Delete();
							stableMaster.TextMessage( GetDictionaryEntry( 2110, pUser.socket.language )); // I am sorry to inform thee that... well... it died.
						}
					}
				}
			}
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 2110, pSock.language )); // You are no longer in range to retrieve your pet from the stableMaster.
		}
	}
}

function ReleasePet( petObj, petNum, stableMaster, pUser, sayReleaseMsg )
{
	// Reset some values for the pet, teleport it back to owner's location
	petObj.stabled = 0;
	petObj.visible = 0;
	petObj.frozen = false;
	petObj.hungerstatus = true;
	petObj.vulnerable = true;
	petObj.Teleport( pUser );
	totalStabledPets = totalStabledPets - 1;
	pUser.SetTag( "totalStabledPets", totalStabledPets );
	pUser.SetTag( "stabledPet" + petNum, null );
	pUser.SetTag( "stableMasterSerial", null );
	pUser.controlSlotsUsed = pUser.controlSlotsUsed + petObj.controlSlots;
	if( sayReleaseMsg )
	{
		var npcMsg = GetDictionaryEntry( 2113, pUser.socket.language ); // I have thy pet; %s. Let me fetch it.
		stableMaster.TextMessage( npcMsg.replace( /%s/gi, petObj.name ));
	}
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	var stableMaster;
	if( pUser )
	{
		stableMaster = pUser.tempObj;
	}

	if( !ValidateObject( stableMaster ))
		return;

	if( !pSock.GetWord( 1 ) && ValidateObject( ourObj ) && ourObj.isChar )
	{
		// Check how many pets the user has stabled already
		totalStabledPets = pUser.GetTag( "totalStabledPets" );

	   	// Lots of generic checks:
		if( !pUser.InRange( stableMaster, 8 ))
		{
			pUser.SysMessage( GetDictionaryEntry( 2114, pSock.language )); // You are out of range from the stableMaster.
		}
		else if( !ourObj.tamed || ourObj.isHuman )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2389, pSock.language )); // You can't stable that!
		}
		else if( !ourObj.InRange( stableMaster, 8 ))
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2115, pSock.language )); // Huh? I can't see this pet you want stabled anywhere nearby. Are you sure it hasn't wandered off?
		}
		else if( ourObj == pUser )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2116, pSock.language )); // HA HA HA! Sorry, I am not an inn.
		}
		else if( ourObj.owner != pUser )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2117, pSock.language )); // That's not your pet!
		}
		else if( ourObj.isDispellable )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2118, pSock.language )); // I cannot stable summoned creatures.
		}
		else if(( ourObj.id == 0x0123 || ourObj.id == 0x0124 ) && ourObj.pack && ourObj.pack.itemsinside > 0 )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2119, pSock.language )); // You need to unload your pet.
		}
		else if( ourObj.atWar )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2120, pSock.language )); // I'm sorry. Your pet seems to be busy.
		}
		else if( totalStabledPets >= maxStabledPets )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2121, pSock.language )); // You have too many pets in the stables!
		}
		else if( ourObj.stabled == 1 )
		{
			stableMaster.TextMessage( GetDictionaryEntry( 2122, pSock.language )); // Eh? That creature is already stabled!
		}
		else
		{
			var i = 0;
			for( i = 0; i <= 4; i++ )
			{
				var tempPet = pUser.GetTag( "stabledPet" + i );
				if( tempPet == null || tempPet == "0" )
				{
					// Time to extract some GOOOOLD!
					var bankItem;
					var foundGold = false;
					var bankBox = pUser.FindItemLayer( 29 );
					if( pUser.ResourceCount( 0x0EED, 0 ) >= stableCost )
					{
						pUser.UseResource( stableCost, 0x0EED );
						pUser.SysMessage( GetDictionaryEntry( 2123, pSock.language ), stableCost ); // %i gold has been paid from your back-pack.
						StablePet( pUser, ourObj, i, stableMaster );
						return;
					}
					else if( bankBox )
					{
						for( bankItem = bankBox.FirstItem(); !bankBox.FinishedItems(); bankItem = bankBox.NextItem() )
						{
							if( bankItem != null && bankItem != 0 )
							{
								if( bankItem.id == 0x0EED )
								{
									if( bankItem.amount >= stableCost )
									{
										bankBox.UseResource( stableCost, 0x0EED );
										foundGold = true;
										pUser.SysMessage( GetDictionaryEntry( 2124, pSock.language ), stableCost );
										StablePet( pUser, ourObj, i, stableMaster );
										return;
									}
								}
							}
						}
					}
					if( foundGold != true )
					{
						stableMaster.TextMessage( GetDictionaryEntry( 2125, pSock.language )); // But thou hast not the funds in thy bank account!
						return;
					}
				}
			}
		}
	}
	else
	{
		stableMaster.TextMessage( GetDictionaryEntry( 2125, pSock.language )); // You can't stable that!
	}
}

function StablePet( pUser, ourObj, slotNum, stableMaster )
{
	if( pUser && ourObj )
	{
		// Both the player and the targeted pet exists, so.. let's stable it!
		pUser.SetTag( "stabledPet" + slotNum, ourObj.serial );
		ourObj.SetTag( "stableTimeAt", GetCurrentClock() );
		ourObj.stabled = 1;
		ourObj.visible = 3;
		ourObj.wandertype = 0;
		ourObj.frozen = true;
		ourObj.hunger = 6;
		ourObj.hungerstatus = false;
		ourObj.vulnerable = false;
		ourObj.Teleport( stableX, stableY, stableZ );

		// Reduce control slots in use for player by amount occupied by pet that was stabled
		pUser.controlSlotsUsed = Math.max( 0, pUser.controlSlotsUsed - ourObj.controlSlots );

		// Increase the count of pets stabled, store as tag on player so it doesn't get lost if stablemaster is lost
		totalStabledPets++;
		pUser.SetTag( "totalStabledPets", totalStabledPets );
		stableMaster.TextMessage( GetDictionaryEntry( 2127, pUser.socket.language )); // Your pet has been stabled
	}
}
