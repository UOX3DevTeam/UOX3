// Bank Checks (by Xuri)
// v1.02
// Last Updated: 5. July 2021

function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	var bankBox = pUser.FindItemLayer( 29 );
	if( ValidateObject( bankBox ) && iUsed.container && iUsed.container.serial == bankBox.serial )
	{
		var checkSize = iUsed.GetTag( "CheckSize" );
		if( checkSize > 65535 )
		{
			var numOfGoldPiles = ( checkSize / 65535 );
			var i = 0; var newGoldPile;
			var remainingGold = checkSize;

			// Check that player's bankbox can hold all the piles of gold
			if(( bankBox.totalItemCount + numOfGoldPiles ) >= bankBox.maxItems )
			{
				pSock.SysMessage( GetDictionaryEntry( 1818 ), pSock.language ); // That container is already at max capacity
				return false;
			}

			for( i = 1; i < numOfGoldPiles + 1; i++ )
			{
				if( remainingGold >= 65535 )
				{
					newGoldPile = CreateDFNItem( pUser.socket, pUser, "0x0EED", 65535, "ITEM", false );
					remainingGold = remainingGold - 65535;
					newGoldPile.container = bankBox;
				}
				else
				{
					newGoldPile = CreateDFNItem( pUser.socket, pUser, "0x0EED", remainingGold, "ITEM", false );
					newGoldPile.container = bankBox;
					continue;
				}
			}
		}
		else
		{
			// Check that player's bankbox can hold all the piles of gold
			if( bankBox.totalItemCount >= bankBox.maxItems )
			{
				pSock.SysMessage( GetDictionaryEntry( 1818 ), pSock.language ); // That container is already at max capacity
				return false;
			}

			var newGoldPile = CreateDFNItem( pUser.socket, pUser, "0x0EED", checkSize, "ITEM", false );
			newGoldPile.container = bankBox;
		}
		pUser.TextMessage( GetDictionaryEntry( 2703, pSock.language ) + " " + checkSize, false, 0x096a ); // Gold was deposited in your account:
		iUsed.Delete();
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 2702, pSock.language )); // That must be in your bank box to use it.
	}
	return false;
}

// Display value of bank check
function onTooltip( bankCheck )
{
	var tooltipText = "";
	var checkSize = bankCheck.GetTag( "CheckSize" );
	// 2793=Value: %i
	tooltipText = GetDictionaryEntry( 2793 ).replace( /%i/gi, checkSize ); // Value: %i
	bankCheck.SetTempTag( "clilocTooltip", 1042971 ); // ~1_NOTHING~
	return tooltipText;
}

// Show value as part of name if tooltips are disabled
const aosTooltipsEnabledClient = GetClientFeature( 4 );
const aosTooltipsEnabledServer = GetServerFeature( 5 );
function onNameRequest( bankCheck, pUser )
{
	// Default name
	var nameString = bankCheck.name;

	if( !aosTooltipsEnabledClient && !aosTooltipsEnabledServer )
	{
		nameString = bankCheck.name + " " + GetDictionaryEntry( 2794, pUser.socket.language ).replace( /%i/gi, bankCheck.GetTag( "CheckSize" )); // for %i gold
	}

	return nameString;
}

function _restorecontext_() {}