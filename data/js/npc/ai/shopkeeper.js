// This script handles special interactions between players and shopkeepers
// that take place when players attempt (and/or succeed) at trading with these

// If enabled, guild members get a -10% discount for items they buy from shopkeepers in the guild
const enableNPCGuildDiscounts = GetServerSetting( "EnableNPCGuildDiscounts" );

// If enabled, guild members get a +10% premium price offered for items they sell to shopkeepers in the guild
const enableNPCGuildPremiums = GetServerSetting( "EnableNPCGuildPremiums" );

const coreShardEra = GetServerSetting( "CoreShardEra" );

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

	// Add a custom tag to items bought from NPC shopkeepers
	if( !iBought.isPileable )
	{
		iBought.SetTag( "boughtFromNPC", true );
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

	if( youngPlayerSystem && EraStringToNum( coreShardEra ) >= EraStringToNum( "lbr" ) && pChar.account.isYoung && !iSold.isPileable && iSold.GetTag( "boughtFromNPC" ))
	{
		// Young players can sell vendor-bought non-pileable items back to vendors for the full price
		var goldDiff = iSold.buyvalue - iSold.sellvalue;
		var moreGold = CreateDFNItem( pSock, pChar, "0x0eed", goldDiff, "ITEM", true );
		if( ValidateObject( moreGold ))
		{
			npcShopkeep.TextMessage( GetDictionaryEntry( 18739, pSock.language ), false, 0x3b2, 0, pChar.serial ); // As a Young player, you are refunded the full value of store-bought non-stackable items
			moreGold.PlaceInPack( true );
		}
	}
	else
	{
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
				goldBonus.PlaceInPack( true );
			}
		}
	}

	return false;
}