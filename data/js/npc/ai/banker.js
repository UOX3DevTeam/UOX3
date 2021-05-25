//	Banker AI
//	Responds to bank-commands:
// 	BANK, BALANCE/STATEMENT, CHECK, WITHDRAW # and DEPOSIT # or DEPOSIT without arguments to get target cursor

//The maximum amount of items depositable in a bank box
// - unfortunately only used when depositing items using the DEPOSIT command
var maxBankItemAmt = 125;
var bankCheckTrigger = 5015;

function onSpeech( strSaid, pTalking, pTalkingTo )
{
	if( strSaid )
	{
		if( pTalking.criminal || pTalking.murderer )
		{
			pTalkingTo.TextMessage( "Thou art a criminal and cannot access thy bank box.", true, 0x03b2 );
			return false;
		}
		var bankBox = pTalking.FindItemLayer( 29 );
		var goldInBank = 0;
		var splitString = strSaid.split( " " );
		switch( splitString[0].toUpperCase() )
		{
		case "BANK":
			if( !bankBox )
			{
				//This should rarely be necessary, all players should have had a bank-box added
				//upon character creation.
				bankBox = createNewBankBox( pTalking );
				if( !bankBox )
				{
					return false;
				}
			}
			pTalking.OpenBank( pTalking.socket );
			var bankContents = countBankContents( pTalking, bankBox );
			if( bankContents )
			{
				var itemAmount = parseInt( bankContents[0], 10 );
				var stones = parseInt( bankContents[1], 10 );
				stones = stones.toFixed( 2 );
				pTalking.TextMessage( "Bank container has " + bankContents[0] + " items, " + stones + " stones", false, 0x096a );
			}
			break;
		case "BALANCE":
		case "STATEMENT":
			if( bankBox )
			{
				goldInBank = countGoldInBank( pTalking, bankBox );
				pTalkingTo.TextMessage( "Thy current bank balance is: " + goldInBank, true, 0x03b2 );
			}
			break;
		case "CHECK":
			if( bankBox.totalItemCount >= bankBox.maxItems )
			{
				pTalking.socket.SysMessage( GetDictionaryEntry( 1818, pTalking.socket.language )); // The container is already at capacity.
				return false;
			}
			else if( splitString[1] && ( parseInt( splitString[1], 10 ) >= 5000 ))
			{
				var checkSize = parseInt( splitString[1], 10 );
				if( bankBox )
				{
					goldInBank = countGoldInBank( pTalking, bankBox );
					if( checkSize > goldInBank )
					{
						pTalkingTo.TextMessage( "Ah, art thou trying to fool me? Thou hast not so much gold!", true, 0x03b2 );
					}
					else
					{
						var newCheck = CreateDFNItem( pTalking.socket, pTalking, "0x14F0", 1, "ITEM", false ); //Add check
						if( newCheck )
						{
							bankBox.UseResource( checkSize, 0x0EED, 0 );
							newCheck.SetTag( "CheckSize", checkSize );
							newCheck.name = "A bank check";
							newCheck.colour = 0x34;
							newCheck.AddScriptTrigger( bankCheckTrigger );
							newCheck.isNewbie = true;
							newCheck.weight = 100;
							newCheck.container = bankBox;
							pTalkingTo.TextMessage( "Into your bank box I have placed a check in the amount of " + checkSize, true, 0x03b2 );
						}
					}
				}
			}
			else
			{
				pTalkingTo.TextMessage( "We cannot create checks for such a paltry amount of gold!", true, 0x03b2 );
			}
			break;
		case "WITHDRAW":
			var playerPack = pTalking.charpack;
			if( playerPack.totalItemCount >= playerPack.maxItems )
			{
				pTalking.socket.SysMessage( 1819 ); // Your backpack cannot hold any more items!
				return false;
			}
			if( splitString[1] && splitString[1] != 0 )
			{
				var withdrawAmt = parseInt( splitString[1], 10 );
				if( !isNaN( withdrawAmt ) && withdrawAmt > 0 )
				{
					pTalking.TextMessage( withdrawAmt );
					if( bankBox )
					{
						goldInBank = countGoldInBank( pTalking, bankBox );
						if( withdrawAmt > goldInBank )
						{
							pTalkingTo.TextMessage( "Ah, art thou trying to fool me? Thou hast not so much gold!", true, 0x03b2 );
						}
						else
						{
							if( withdrawAmt > 65535 )
							{
								divideWithdrawnGold( pTalking, bankBox, withdrawAmt );
							}
							else
							{
								var newWithdrawGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", withdrawAmt, "ITEM", true );
							}
							bankBox.UseResource( withdrawAmt, 0x0EED, 0 );
							pTalkingTo.TextMessage( "Thou hast withdrawn gold from thy account.", true, 0x03b2 );
						}
					}
				}
			}
			else
			{
				pTalkingTo.TextMessage( "Thou must tell me how much thou wishest to withdraw.", true, 0x03b2 );
			}
			break;
		case "DEPOSIT": //uox3 specific command
			if( bankBox.totalItemCount >= bankBox.maxItems )
			{
				pTalking.socket.SysMessage( GetDictionaryEntry( 1818, pTalking.socket.language )); // The container is already at capacity.
				return false;
			}
			if( !splitString[1])
			{
				pTalking.tempObj = pTalkingTo;
				pTalkingTo.TextMessage( "Which item do you wish to deposit in your bank box?", true, 0x03b2 );
				pTalking.CustomTarget( 0, "" );
			}
			else if( splitString[1] && splitString[1] > 0 )
			{
				var depositAmt = parseInt( splitString[1], 10 );
				var goldInPack = pTalking.ResourceCount( 0x0EED, 0 );
				if( !isNaN( depositAmt ) && depositAmt > 0)
				{
					if( bankBox )
					{
						if( goldInPack < depositAmt )
						{
							pTalkingTo.TextMessage( "Ah, art thou trying to fool me? Thou hast not so much gold!", true, 0x03b2 );
						}
						else
						{
							var bankContents = countBankContents( pTalking, bankBox );
							if( parseInt( bankContents[0] ) < maxBankItemAmt )
							{
								pTalking.UseResource( depositAmt, 0x0EED, 0 );
								divideDepositedGold( pTalking, bankBox, depositAmt );
								pTalkingTo.TextMessage( "Thou hast deposited gold to thy account.", true, 0x03b2 );
							}
							else
							{
								pTalking.SysMessage( "That container cannot hold more items." );
							}
						}
					}
				}
			}
			else
			{
				pTalkingTo.TextMessage( "Thou must tell me how much thou wishest to deposit.", true, 0x03b2 );
			}
			break;
		default:
			break;
		}
	}
	return false;
}

/* Checks to see if the player already has a bank box.
function checkForBankBox( pTalking )
{
	var bankBox = pTalking.FindItemLayer( 29 );
    if( bankBox == null || !bankBox.isItem )
    {
    	return false;
	}
}*/

// Creates a new bank box for players that, for some reason, do not already have one
function createNewBankBox( pTalking )
{
	// Create a new bankbox for the player
	var newBankBox = CreateDFNItem( pTalking.socket, pTalking, "0x09ab", 1, "ITEM", false );
	newBankBox.name = pTalking.name + "'s bank box";
	newBankBox.layer = 29;
	newBankBox.owner = pTalking;
	newBankBox.container = pTalking;
	newBankBox.maxItems = parseInt(GetServerSetting( "MAXPLAYERBANKITEMS" ));
	newBankBox.type = 1;
	newBankBox.morex = 1;
	if( newBankBox )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Counts the amount of items in the bank box and registers their total weight
function countBankContents( pTalking, bankBox )
{
	var mItem;
	var bankContents = new Array();
	bankContents[0] = 0;
	bankContents[1] = 1;
	for( mItem = bankBox.FirstItem(); !bankBox.FinishedItems(); mItem = bankBox.NextItem() )
	{
		if( mItem != null )
		{
			bankContents[0] = bankContents[0] + 1;
			bankContents[1] = bankContents[1] + ( mItem.weight / 100 );
		}
	}
	return bankContents;
}

// Count the gold in player's bankbox!
// If ResourceCount JS Method worked with items, this could be removed
function countGoldInBank( pTalking, bankBox )
{
	var mItem;
	var goldInBank = 0;
	for( mItem = bankBox.FirstItem(); !bankBox.FinishedItems(); mItem = bankBox.NextItem() )
	{
		if( mItem != null )
		{
			if( mItem.id == 0x0EED )
			{
				goldInBank = goldInBank + mItem.amount;
			}
		}
	}
	return goldInBank;
}

// Select item to deposit into bankbox
function onCallback0( pSock, ourObj )
{
	var pTalking = pSock.currentChar;
	var pTalkingTo = pTalking.tempObj;
	var bankBox = pTalking.FindItemLayer( 29 );
	var isInRange = pTalking.InRange( pTalkingTo, 7 );
	if( isInRange )
	{
		if( !pSock.GetWord( 1 ) && ourObj.isItem )
		{
			if( bankBox && ourObj.container && (( ourObj.container == pTalking ) || ( ourObj.container == pTalking.pack )))
			{
				var bankContents = countBankContents( pTalking, bankBox );
				if( parseInt( bankContents[0] ) < maxBankItemAmt )
				{
					ourObj.container = bankBox;
					pTalkingTo.TextMessage( "You have deposited the selected item in your bank box.", true, 0x03b2 );
				}
				else
				{
					pTalking.SysMessage( "That container cannot hold more items." );
				}
			}
			else
			{
				pTalking.SysMessage( GetDictionaryEntry( 778, pSock.language )); //You can't use items outside your backpack
			}
		}
	}
	else
	{
		pTalking.SysMessage( "You are out of range from the banker." );
	}
}

//Divide gold to be withdrawn into several piles of max 65535 each
function divideWithdrawnGold( pTalking, bankBox, withdrawAmt )
{
	var numOfGoldPiles = ( withdrawAmt / 65535 );
	var i = 0; var newGoldPile;
	var remainingGold = withdrawAmt;
	for( i = 1; i < numOfGoldPiles + 1; i++ )
	{
		if( remainingGold >= 65535 )
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", 65535, "ITEM", true );
			remainingGold = remainingGold - 65535;
		}
		else
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", remainingGold, "ITEM", true );
			continue;
		}
	}
}

//Divide gold to be deposited into several piles of max 65535 each
function divideDepositedGold( pTalking, bankBox, depositAmt )
{
	var numOfGoldPiles = ( depositAmt / 65535 );
	var i = 0; var newGoldPile;
	var remainingGold = depositAmt;
	for( i = 1; i < numOfGoldPiles + 1; i++ )
	{
		if( remainingGold >= 65535 )
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", 65535, "ITEM", false );
			remainingGold = remainingGold - 65535;
			newGoldPile.container = bankBox;
		}
		else
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", remainingGold, "ITEM", false );
			newGoldPile.container = bankBox;
			continue;
		}
	}
}