// Bank Checks (by Xuri)
// v1.00
// Last Updated: 16. August 2006

function onUseChecked( pUser, iUsed )
{
	var bankBox = pUser.FindItemLayer( 29 );
	if( bankBox && iUsed.container && iUsed.container.serial == bankBox.serial )			
	{
		var checkSize = iUsed.GetTag( "CheckSize" );
		if( checkSize > 65535 )
		{
			var numOfGoldPiles = ( checkSize / 65535 );
			var i = 0; var newGoldPile;
			var remainingGold = checkSize;
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
			var newGoldPile = CreateDFNItem( pUser.socket, pUser, "0x0EED", checkSize, "ITEM", false );
			newGoldPile.container = bankBox;
		}
		pUser.TextMessage( "Gold was deposited in your account: " + checkSize, false, 0x096a );		
		iUsed.Delete();
	}
	else
		pUser.SysMessage( "That must be in your bank box to use it." );
	return false;
}