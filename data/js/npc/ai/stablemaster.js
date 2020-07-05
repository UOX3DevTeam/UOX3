// Stable-master, by Xuri (xuri@xoduz.org)
// Version: 1.0
// Last Updated: May 11th 2009

//SETTINGS:
var stableCost = 90; //The cost to stable a pet for a week
var maxFollowers = 5; //The maximum amount of followers/pets a character can have at a given time
var maxStableDays = 0; //The maximum amount of days a pet will be kept safe in the stables - 0 (forever) is default
var NPCPause = 30000; //Time in seconds * 1000 the NPC pauses in his tracks for when players interact
//ingame coordinates where stabled pets are moved (should be unreachable for players!!!)
var stableX = 8000;
var stableY = 8000;
var stableZ = 0;

//Ignore these
var maxStabledPets = 0; //The amount of pets a character can have in the stables, determined later on by player skills
var totalStabledPets = 0; //The total amount of pets a character has in the stables, retrieved later on from player

function onSpeech( strSaid, pTalking, StableMaster )
{
	var pSock = pTalking.socket;

	//Verify that a socket (aka connected player) exists, and
	//that the player talking is within 4 tiles of the stablemaster.
	//Else, ignore the speech
	if( pSock != null && pTalking.InRange( StableMaster, 4) )
	{
		// Turn the Stablemaster towards the one talking
		StableMaster.TurnToward( pTalking );

		//Let's pause the NPC in his tracks for X seconds (defined at top of script) so he doesn't wander off
		StableMaster.SetTimer( 11, NPCPause );

		//Iterate through any triggerwords said by the player
		for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
		{
			switch( trigWord )
			{
				case 0x0008: //"stable"
				{
					//Save stablemaster as a tempObj on the player, for retrieval later
					pTalking.tempObj = StableMaster;
					if( maxStableDays == 0 )
						StableMaster.TextMessage( "I charge "+stableCost+" per pet you want to stable. I will withdraw it from thy bank account." );
					else
						StableMaster.TextMessage( "I charge "+stableCost+" per pet for "+maxStableDays+" days of stable time. I will withdraw it from thy bank account." );

					pSock.CustomTarget( 0, "Which animal wouldst thou like to stable here?" );
					break;
				}
				case 0x0009: //"claim"
				{
					totalStabledPets = pTalking.GetTag( "totalStabledPets" );
					if( totalStabledPets > 0 )
					{
						if( !claimPetByName( pTalking, StableMaster, strSaid ) )
						{ //Display the gump with all the pets
							totalStabledPets = pTalking.GetTag( "totalStabledPets" );
							if( totalStabledPets > 0 )
							{
								pTalking.SetTag( "stableMasterSerial", StableMaster.serial );
								StableMaster.TextMessage( "I currently have the following pets of yours stabled right now..." );

								//Ok, player has animals stabled - let's give him the list of pets
								claimGump( pTalking, StableMaster );
							}
						}
					}
					else
						StableMaster.TextMessage( "But I have none of your pets stabled with me at the moment!" );
					break;
				}
				default:
					break;
			}
		}

		return 2;
	}
}

//Function to check if a pet by the name given by the player exist in the stables,
//and if it does, release it directly without displaying the menu
function claimPetByName( pTalking, StableMaster, strSaid )
{
	var splitString = strSaid.toLowerCase().split("vendor ")[1];
	if( splitString )
		var splitString = splitString.split( " " );
	else
		var splitString = strSaid.split( " " );
	if( splitString[1] && splitString[1].toUpperCase() != "LIST" )
	{
		var i = 0;
		var petFound = false;
		var j = maxStabledPets - 1;
		for( i = 0; i <= j; i++ )
		{
			var tempPet = pTalking.GetTag( "stabledPet" + i );
			if( tempPet != null && tempPet != "0" )
			{
				var petObj = CalcCharFromSer( tempPet );
				if( petObj )
				{
					var petName = petObj.name;
					if( petName.toUpperCase() == splitString[1].toUpperCase() )
					{
						//releasePet( petObj, i, StableMaster, pTalking );
						claimPet( pTalking, i, StableMaster );
						petFound = true;
						return true;
					}
				}
			}
		}
		if( petFound != true )
			StableMaster.TextMessage( "I have no pet that answers to that name in my stables." );
	}
	else
		return false;
}

function claimGump( pUser, stableMaster )
{
   	totalStabledPets = pUser.GetTag( "totalStabledPets" );

   	//Let's check the player's skills and set the max amount of slots he
   	//has available in the stables depending on those
   	calcStableSlotBonus( pUser );

   	//extraBGSize is used to resize the claim-pet gump based on how many slots
   	//the user has available - calculated above
	var extraBGSize = 110 + (maxStabledPets * 20);

   	var claimGump = new Gump;
	claimGump.AddPage (0);
	claimGump.AddBackground( 0, 0, 200, extraBGSize, 9250 );
	claimGump.AddButton( 165, 10, 5052, 1, 0, 0 ); // Close Menu Button
	claimGump.AddCheckerTrans( 0, 0, 200, extraBGSize );

	claimGump.AddText( 20, 15, 50, "- Stablemaster Menu -" );

	claimGump.AddHTMLGump( 20, 35, 200, 20, 0, 0, "<BASEFONT COLOR=#FFFFFF>Select a pet to retrieve</BASEFONT>" );
	claimGump.AddHTMLGump( 20, 55, 200, 20, 0, 0, "<BASEFONT COLOR=#FFFFFF>from the stables:</BASEFONT>" );
	claimGump.AddHTMLGump( 20, 75, 200, 20, 0, 0, "<BASEFONT COLOR=#FFFFFF>- - - - - - - - - - - - </BASEFONT>" );

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
				claimGump.AddButton( 20, 99 + (i * 20), 0x845, 1, 0, i+1 );
				claimGump.AddHTMLGump( 42, 95 + (i * 20), 200, 18, 0, 0, "<BASEFONT COLOR=#C0C0EE>" +petObj.name+"</BASEFONT>" );
			}
		}
		else
			claimGump.AddHTMLGump( 25, 95 + (i * 20), 200, 18, 0, 0, "<BASEFONT COLOR=#C0C0EE><SMALL><EM>(Empty storage slot " +(i+1)+")</EM></SMALL></BASEFONT>" );
	}

    claimGump.Send( pUser.socket );
	claimGump.Free();
}

function calcStableSlotBonus( pUser )
{
	//Let's check relevant player-skills and assign him a number of stable-slots depending on those skills
	var stableModifier = pUser.skills.taming + pUser.skills.veterinary + pUser.skills.animallore;
   	if( stableModifier <= 160 )
   		maxStabledPets = 2;
   	else if( stableModifier > 160 && stableModifier <= 199.9 )
   		maxStabledPets = 3;
   	else if( stableModifier >= 200 && stableModifier <= 239.9 )
   		maxStabledPets = 4;
   	else if( stableModifier >= 240 )
   		maxStabledPets = 5;

   	//Increase max stable-slots available by 1 for each of the following skills
   	// he has at 100.0 or above:
   	if( pUser.skills.taming >= 1000 )
   		maxStabledPets++;
   	if( pUser.skills.veterinary >= 1000 )
   		maxStabledPets++;
   	if( pUser.skills.animallore >= 1000 )
   		maxStabledPets++;
}

function onGumpPress(pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var stableMasterSer = pUser.GetTag( "stableMasterSerial" );

	if( stableMasterSer )
	{
		var StableMaster = CalcCharFromSer( stableMasterSer );
		if( pUser.petCount < maxFollowers )
		{
			switch( pButton )
			{
			case 0: //Closes gump
				break;
			default:
				claimPet( pUser, pButton - 1, StableMaster );
				break;
			}
		}
		else
			StableMaster.TextMessage( "Your pet remains in the stables because you have too many followers!" );
	}
}

function claimPet( pUser, petNum, StableMaster )
{
	if( pUser && petNum >= 0 )
	{
		if( pUser.InRange( StableMaster, 4 ) )
		{
			var tempPet = pUser.GetTag( "stabledPet" + petNum );
			if( tempPet != null && tempPet != "0" )
			{
				var petObj = CalcCharFromSer( tempPet );
				if( petObj )
				{
					var totalStabledPets = pUser.GetTag( "totalStabledPets" );
					var stableTimeAt = petObj.GetTag( "stableTimeAt" );
					var maxStableTime = maxStableDays * 86402350; // 86402350 should be approx 24 hours
					var strokeOfLuck = RandomNumber( 1, 10 );

					//If the time passed since pet was stabled is less than the maximum time allowed,
					//or if the player has a stroke of luck even though time has passed max...
					//...then let him claim the pet
					if(( GetCurrentClock() - stableTimeAt < maxStableTime ) || maxStableTime == 0 )
					{
						//Let's release the pet
						releasePet( petObj, petNum, StableMaster, pUser );
					}
					else
					{
						//Let's give the user a small chance of retrieving his pet even after the max time has passed
						if( strokeOfLuck >= 8 )
						{
							StableMaster.TextMessage( "You're in luck today! Even though you're overdue to retrieve your pet, it is still alive!" );
							releasePet( petObj, petNum, StableMaster, pUser );
						}
						else
						{
							//The pet was stabled for too long, and ...died ;P
							totalStabledPets = totalStabledPets - 1;
							pUser.SetTag( "totalStabledPets", totalStabledPets );
							pUser.SetTag( "stabledPet" + petNum, null );
							pUser.SetTag( "stableMasterSerial", null );
							petObj.Delete();
							StableMaster.TextMessage( "I am sorry to inform thee that... well... it died." );
						}
					}
				}
			}
		}
		else
			pUser.SysMessage( "You are no longer in range to retrieve your pet from the stablemaster." );
	}
}

function releasePet( petObj, petNum, StableMaster, pUser )
{
	//Reset some values for the pet, teleport it back to owner's location
	petObj.stabled = 0;
	petObj.visible = 0;
	petObj.frozen = false;
	petObj.hungerstatus = true;
	petObj.vulnerable = true;
	petObj.Teleport( pUser );
	totalStabledPets = totalStabledPets - 1;
	pUser.SetTag( "totalStabledPets", totalStabledPets );
	StableMaster.TextMessage( "I have thy pet; "+petObj.name+". Let me fetch it." );
	pUser.SetTag( "stabledPet" + petNum, null );
	pUser.SetTag( "stableMasterSerial", null );
}

function onCallback0( socket, ourObj )
{
	var pUser = socket.currentChar;
	if( pUser )
		var StableMaster = pUser.tempObj;
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		//Check how many pets the user has stabled already
		totalStabledPets = pUser.GetTag( "totalStabledPets" );

	   	//Let's check the player's skills and set the max amount of slots he
	   	//has available in the stables depending on those:
	   	calcStableSlotBonus( pUser );

	   	//Lots of generic checks:
		if( !pUser.InRange( StableMaster, 4 ) )
			pUser.SysMessage( "You are out of range from the stablemaster." );
		else if( !ourObj.InRange( StableMaster, 7 ) )
			StableMaster.TextMessage( "Huh? I can't see this pet you want stabled anywhere nearby. Are you sure it hasn't wandered off?" );
		else if( ourObj == pUser )
			StableMaster.TextMessage( "HA HA HA! Sorry, I am not an inn." );
		else if( ourObj.owner != pUser )
			StableMaster.TextMessage( "That's not your pet!" )
		else if( ourObj.isDispellable )
			StableMaster.TextMessage( "I cannot stable summoned creatures." );
		else if(( ourObj.id == 0x0123 || ourObj.id == 0x0124 ) && ourObj.pack && ourObj.pack.itemsinside > 0 )
			StableMaster.TextMessage( "You need to unload your pet." );
		else if( ourObj.atWar )
			StableMaster.TextMessage( "I'm sorry. Your pet seems to be busy." );
		else if( totalStabledPets >= maxStabledPets )
			StableMaster.TextMessage( "You have too many pets in the stables!" );
		else if( ourObj.stabled == 1 )
			StableMaster.TextMessage( "Eh? That creature is already stabled!" );
		else
		{
			var i = 0;
			for( i = 0; i <= 4; i++ )
			{
				var tempPet = pUser.GetTag( "stabledPet" + i );
				if( tempPet == null || tempPet == "0" )
				{
					//Time to extract some GOOOOLD!
					var bankItem;
					var foundGold = false;
					var bankBox = pUser.FindItemLayer( 29 );
					if( pUser.ResourceCount( 0x0EED, 0 ) >= stableCost )
					{
						pUser.UseResource( stableCost, 0x0EED );
						pUser.SysMessage( stableCost+" gold has been paid from your back-pack." );
						stablePet( pUser, ourObj, i, StableMaster );
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
										pUser.SysMessage( stableCost+" gold has been withdrawn from your bank-box." );
										stablePet( pUser, ourObj, i, StableMaster );
									}
								}
							}
						}
					}
					if( foundGold != true )
					{
						StableMaster.TextMessage( "But thou hast not the funds in thy bank account!" );
						return;
					}
				}
			}
		}
	}
	else
		StableMaster.TextMessage( "You can't stable that!" );
}

function stablePet( pUser, ourObj, slotNum, StableMaster )
{
	if( pUser && ourObj )
	{
		//Both the player and the targeted pet exists, so.. let's stable it!
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
		totalStabledPets++;
		pUser.SetTag( "totalStabledPets", totalStabledPets );
		StableMaster.TextMessage( "Your pet has been stabled." );
	}
}
