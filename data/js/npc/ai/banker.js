//	Banker AI
//	Responds to bank-commands:
// 	BANK, BALANCE/STATEMENT, CHECK, WITHDRAW # and DEPOSIT # or DEPOSIT without arguments to get target cursor

// The maximum amount of items depositable in a bank box
var maxBankItemAmt = 125;

// Script ID for banker AI script
var bankCheckTrigger = 5015;

function onSpeech( strSaid, pTalking, pTalkingTo )
{
	if( strSaid )
	{
		var pSock = pTalking.socket;
		if( pSock == null )
			return false;

		if( pTalking.criminal || pTalking.murderer )
		{
			pTalkingTo.TextMessage( GetDictionaryEntry( 7000, pSock.language ), true, 0x03b2 ); // Thou art a criminal and cannot access thy bank box.
			return false;
		}

		var bankBox = pTalking.FindItemLayer( 29 );
		if( !ValidateObject( bankBox ))
		{
			pSock.SysMessage( "Error: No valid bankbox found! Please contact a GM for assistance." );
			Console.Error( "No valid bankbox found for character " + pTalking.name + " with serial " + ( pTalking.serial ).toString() + "!" );
			return false;
		}

		var trigWordHandled = false;
		for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
		{
			if( trigWordHandled )
				break;

			switch( trigWord )
			{
				case 0: // Withdraw
					WithdrawFromBank( pSock, pTalking, pTalkingTo, bankBox, strSaid );
					trigWordHandled = true;
					break;
				case 1: // Balance, Statement
					CheckBalance( pSock, pTalking, pTalkingTo, bankBox );
					trigWordHandled = true;
					break;
				case 2: // Bank
					OpenBank( pSock, pTalking, pTalkingTo, bankBox );
					trigWordHandled = true;
					break;
				case 3: // Check
					CreateCheck( pSock, pTalking, pTalkingTo, bankBox, strSaid );
					trigWordHandled = true;
					break;
				default:
					break;
			}
		}

		// Handle non-triggerword based commands here
		var splitString = strSaid.split( " " );
		switch( splitString[0].toUpperCase() )
		{
			case "DEPOSIT": // uox3 specific command, add other languages as needed
				if( bankBox.totalItemCount >= bankBox.maxItems )
				{
					pSock.SysMessage( GetDictionaryEntry( 1818, pSock.language )); // The container is already at capacity.
					return false;
				}

				if( bankBox.weight > bankBox.weightMax )
				{
					pSock.SysMessage( GetDictionaryEntry( 9183, pSock.language )); // Your bank box is overloaded and cannot hold any more weight.
					return false;
				}

				if( !splitString[1])
				{
					pTalking.tempObj = pTalkingTo;
					pTalkingTo.TextMessage( GetDictionaryEntry( 7001, pSock.language ), true, 0x03b2 ); // Which item do you wish to deposit in your bank box?
					pTalking.CustomTarget( 0, "" );
				}
				else if( splitString[1] && splitString[1] > 0 )
				{
					var depositAmt = parseInt( splitString[1], 10 );
					var goldInPack = pTalking.ResourceCount( 0x0EED, 0 );
					if( !isNaN( depositAmt ) && depositAmt > 0)
					{
						if( goldInPack < depositAmt )
						{
							pTalkingTo.TextMessage( GetDictionaryEntry( 7002, pSock.language ), true, 0x03b2 ); // Ah, art thou trying to fool me? Thou hast not so much gold!
						}
						else
						{
							var bankContents = CountBankContents( pTalking, bankBox );
							if( parseInt( bankContents[0] ) < maxBankItemAmt )
							{
								pTalking.UseResource( depositAmt, 0x0EED, 0 );
								DivideDepositedGold( pTalking, bankBox, depositAmt );
								pTalkingTo.TextMessage( GetDictionaryEntry( 7003, pSock.language ), true, 0x03b2 ); // Thou hast deposited gold to thy account.
							}
							else
							{
								pSock.SysMessage( GetDictionaryEntry( 1935, pSock.language )); // That container cannot hold any more items!
							}
						}
					}
				}
				else
				{
					pTalkingTo.TextMessage( GetDictionaryEntry( 7004, pSock.language ), true, 0x03b2 ); // Thou must tell me how much thou wishest to deposit.
				}
				break;
			default:
				break;
		}
	}

	return false;
}

function WithdrawFromBank( pSock, pTalking, pTalkingTo, bankBox, strSaid )
{
	var playerPack = pTalking.pack;
	if( playerPack.totalItemCount >= playerPack.maxItems )
	{
		pSock.SysMessage( GetDictionaryEntry( 1819, pSock.language )); // Your backpack cannot hold any more items!
		return;
	}

	var splitString = strSaid.split( " " );
	if( splitString[1] && splitString[1] != 0 )
	{
		var withdrawAmt = parseInt( splitString[1], 10 );
		if( !isNaN( withdrawAmt ) && withdrawAmt > 0 )
		{
			pTalking.TextMessage( withdrawAmt );
			if( bankBox )
			{
				var goldInBank = CountGoldInBank( pTalking, bankBox );
				if( withdrawAmt > goldInBank )
				{
					pTalkingTo.TextMessage( GetDictionaryEntry( 7002, pSock.language ), true, 0x03b2 ); // Ah, art thou trying to fool me? Thou hast not so much gold!
				}
				else
				{
					if( withdrawAmt > 65535 )
					{
						DivideWithdrawnGold( pTalking, bankBox, withdrawAmt );
					}
					else
					{
						var newWithdrawGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", withdrawAmt, "ITEM", true );
					}
					bankBox.UseResource( withdrawAmt, 0x0EED, 0 );
					pTalkingTo.TextMessage( GetDictionaryEntry( 7005, pSock.language ), true, 0x03b2 ); // Thou hast withdrawn gold from thy account.
				}
			}
		}
	}
	else
	{
		pTalkingTo.TextMessage( GetDictionaryEntry( 7006, pSock.language ), true, 0x03b2 ); // Thou must tell me how much thou wishest to withdraw.
	}
}

function CheckBalance( pSock, pTalking, pTalkingTo, bankBox )
{
	if( bankBox )
	{
		var goldInBank = CountGoldInBank( pTalking, bankBox );
		pTalkingTo.TextMessage( GetDictionaryEntry( 7007, pSock.language ) + " " + goldInBank, true, 0x03b2 ); // Thy current bank balance is:
	}
}

function OpenBank( pSock, pTalking, pTalkingTo, bankBox )
{
	if( !pTalking.InRange( pTalkingTo, 8 ))
	{
		return;
	}

	if( !ValidateObject( bankBox ))
	{
		// This should rarely be necessary, all players should have had a bank-box added
		// upon character creation.
		bankBox = CreateNewBankBox( pTalking );
		if( !ValidateObject( bankBox ))
		{
			return;
		}
	}

	pTalking.OpenBank( pSock );
	var bankContents = CountBankContents( pTalking, bankBox );
	if( bankContents )
	{
		var itemAmount = parseInt( bankContents[0], 10 );
		var stones = parseInt( bankContents[1], 10 );
		stones = stones.toFixed( 2 );

		var bankMsg = GetDictionaryEntry( 7008, pSock.language ); // Bank container has %i items, %u stones
		bankMsg = bankMsg.replace(/%i/gi, itemAmount);
		pTalking.TextMessage( bankMsg.replace( /%u/gi, stones ), false, 0x096a );
	}
}

function CreateCheck( pSock, pTalking, pTalkingTo, bankBox, strSaid )
{
	if( bankBox.totalItemCount >= bankBox.maxItems )
	{
		pSock.SysMessage( GetDictionaryEntry( 1818, pSock.language )); // The container is already at capacity.
		return;
	}

	var splitString = strSaid.split( " " );
	if( splitString[1] && ( parseInt( splitString[1], 10 ) >= 5000 ))
	{
		var checkSize = parseInt( splitString[1], 10 );
		if( bankBox )
		{
			var goldInBank = CountGoldInBank( pTalking, bankBox );
			if( checkSize > goldInBank )
			{
				pTalkingTo.TextMessage( GetDictionaryEntry( 7002, pSock.language ), true, 0x03b2 ); // Ah, art thou trying to fool me? Thou hast not so much gold!
			}
			else
			{
				var newCheck = CreateDFNItem( pSock, pTalking, "0x14F0", 1, "ITEM", false ); //Add check
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

					var checkMsg = GetDictionaryEntry( 7009, pSock.language ); // Into your bank box I have placed a check in the amount of %i
					pTalkingTo.TextMessage( checkMsg.replace(/%i/gi, checkSize ), true, 0x03b2 );
				}
			}
		}
	}
	else
	{
		pTalkingTo.TextMessage( GetDictionaryEntry( 7010, pSock.language ), true, 0x03b2 ); // We cannot create checks for such a paltry amount of gold!
	}
}

// Creates a new bank box for players that, for some reason, do not already have one
function CreateNewBankBox( pTalking )
{
	// Create a new bankbox for the player
	var newBankBox = CreateDFNItem( pTalking.socket, pTalking, "0x09ab", 1, "ITEM", false );
	newBankBox.name = pTalking.name + "'s bank box";
	newBankBox.layer = 29;
	newBankBox.owner = pTalking;
	newBankBox.container = pTalking;
	newBankBox.maxItems = parseInt( GetServerSetting( "MAXPLAYERBANKITEMS" ));
	newBankBox.weightMax = parseInt( GetServerSetting( "MAXPLAYERBANKWEIGHT" ));
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
function CountBankContents( pTalking, bankBox )
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
			var mItemWeight = mItem.weight;
			if( mItem.amount > 1 )
			{
				mItemWeight *= mItem.amount;
			}
			bankContents[1] = bankContents[1] + ( mItemWeight / 100 );
		}
	}
	return bankContents;
}

// Count the gold in player's bankbox!
// If ResourceCount JS Method worked with items, this could be removed
function CountGoldInBank( pTalking, bankBox )
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
	var isInRange = pTalking.InRange( pTalkingTo, 8 );
	if( isInRange )
	{
		if( !pSock.GetWord( 1 ) && ourObj.isItem )
		{
			if( bankBox && ourObj.container && (( ourObj.container == pTalking ) || ( ourObj.container == pTalking.pack )))
			{
				var bankContents = CountBankContents( pTalking, bankBox );
				if( parseInt( bankContents[0] ) < maxBankItemAmt )
				{
					ourObj.container = bankBox;
					pTalkingTo.TextMessage( GetDictionaryEntry( 7011, pSock.language ), true, 0x03b2 ); // You have deposited the selected item in your bank box.
				}
				else
				{
					pTalking.SysMessage( GetDictionaryEntry( 1935, pSock.language )); // That container cannot hold any more items.
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
		pTalking.SysMessage( GetDictionaryEntry( 7012, pSock.language )); // You are out of range from the banker.
	}
}

// Divide gold to be withdrawn into several piles of max 65535 each
function DivideWithdrawnGold( pTalking, bankBox, withdrawAmt )
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

// Divide gold to be deposited into several piles of max 65535 each
function DivideDepositedGold( pTalking, bankBox, depositAmt )
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