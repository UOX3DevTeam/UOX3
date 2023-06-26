// This script handles special interactions between players and shopkeepers
// that take place when players attempt (and/or succeed) at trading with these

// If enabled, guild members get a -10% discount for items they buy from shopkeepers in the guild
const enableNPCGuildDiscounts = GetServerSetting( "EnableNPCGuildDiscounts" );

// If enabled, guild members get a +10% premium price offered for items they sell to shopkeepers in the guild
const enableNPCGuildPremiums = GetServerSetting( "EnableNPCGuildPremiums" );

function onBoughtFromVendor( pSock, npcShopkeep, iBought, iAmount )
{
	if( pSock == null || !ValidateObject( npcShopkeep ) || !ValidateObject( iBought ))
		return false;

	var pChar = pSock.currentChar;
	if( !ValidateObject( pChar ))
		return false;

	// Give player a discount for purchased items if member of same guild as shopkeeper
	if( enableNPCGuildDiscount && pChar.npcGuild == npcShopkeep.npcGuild )
	{
		npcShopkeep.TextMessage( GetDictionaryEntry( 17618, pSock.language )); // As a fellow guild member, you qualify for a discount!
		var itemCost = iBought.buyvalue * iAmount;
		var discountAmount = Math.round( itemCost * 0.1 );
		var goldReturned = CreateDFNItem( pSock, pChar, "0x0eed", discountAmount, "ITEM", true );
		if( ValidateObject( goldReturned ))
		{
			// Play gold coins SFX
			if( discountAmount == 1 )
			{
				pChar.SoundEffect( 0x0035, false );
			}
			else if( discountAmount < 6 )
			{
				pChar.SoundEffect( 0x0036, false );
			}
			else
			{
				pChar.SoundEffect( 0x0037, false );
			}
			goldBonus.PlaceInPack();
		}
	}

	return false;
}

function onSoldToVendor( pSock, npcShopkeep, iSold, iAmount )
{
	if( pSock == null || !ValidateObject( npcShopkeep ) || !ValidateObject( iSold ))
		return false;

	var pChar = pSock.currentChar;
	if( !ValidateObject( pChar ))
		return false;

	// Give player a 10% bonus for items sold to shopkeeper if member of the same guild
	if( enableNPCGuildBonus && pChar.npcGuild == npcShopkeep.npcGuild )
	{
		npcShopkeep.TextMessage( GetDictionaryEntry( 17619, pSock.language )); // As a fellow guild member, I can give you a premium price for this!
		var itemValue = iSold.sellvalue * iAmount;
		var bonusAmount = Math.round( itemValue * 1.1 );
		var goldBonus = CreateDFNItem( pSock, pChar, "0x0eed", bonusAmount, "ITEM", true );
		if( ValidateObject( goldBonus ))
		{
			// Play gold coins SFX
			if( bonusAmount == 1 )
			{
				pChar.SoundEffect( 0x0035, false );
			}
			else if( bonusAmount < 6 )
			{
				pChar.SoundEffect( 0x0036, false );
			}
			else
			{
				pChar.SoundEffect( 0x0037, false );
			}
			goldBonus.PlaceInPack();
		}
	}

	return false;
}