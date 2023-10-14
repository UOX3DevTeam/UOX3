// Handles functionality of special consumables thieves can steal from monsters

const manaDraughtTimer = 60000 * 10; // 10 minutes
const seedOfLifeTimer = 60000 * 10; // 10 minutes
const balmLotionTimer = 60000 * 30; // 30 minutes

function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock != null && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		if( pUser.isUsingPotion )
		{
			pSock.SysMessage( GetDictionaryEntry( 430, socket.language )); // You must wait a while before using another potion.
			return false;
		}

		// Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language ) ); //That is locked down and you cannot use it
			return false;
		}

		// Reveal player when they use the item
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}

		HandleConsumableEffect( pUser, iUsed );
	}
	return false;
}

function HandleConsumableEffect( pUser, iUsed )
{
	var consumableID = parseInt( iUsed.GetTag( "consumableID" ));
	switch( consumableID )
	{
		case 1: // Mana Draught
			ConsumeManaDraught( pUser, iUsed );
			break;
		case 2: // Seed of Life
			ConsumeSeedOfLife( pUser, iUsed );
			break;
		case 3: // Balm of Protection
			ConsumeBalmLotion( pUser, iUsed, 1 );
			break;
		case 4: // Balm of Strength
			ConsumeBalmLotion( pUser, iUsed, 2 );
			break;
		case 5: // Balm of Swiftness
			ConsumeBalmLotion( pUser, iUsed, 3 );
			break;
		case 6: // Balm of Wisdom
			ConsumeBalmLotion( pUser, iUsed, 4 );
			break;
		case 7: // Life Shield Lotion
			ConsumeBalmLotion( pUser, iUsed, 5 );
			break;
		case 8: // Stone Skin Lotion
			ConsumeBalmLotion( pUser, iUsed, 6 );
			break;
		//case 9: // Gem of Salvation (NOT a consumable)
		//	break;
		default:
			break;
	}
}

function ConsumeManaDraught( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pUser.GetTag( "hasConsumedMD" ) == 1 )
	{
		var activeDraughtTimer = pUser.GetJSTimer( 1, 5044 );
		if( activeDraughtTimer > 0 )
		{
			var timeLeft = (( activeDraughtTimer - GetCurrentClock() ) / 1000 ).toFixed( 0 );
			pSock.SysMessage( GetDictionaryEntry( 18504, pSock.language ), timeLeft ); // You must wait %i seconds before you can use this item.
			return;
		}
		else
		{
			pUser.SetTag( "hasConsumedMD", null );
		}
	}

	if( pUser.mana == pUser.maxmana )
	{
		pSock.SysMessage( GetDictionaryEntry( 18505, pSock.language )); // You are already at full mana.
		return;
	}

	pUser.mana = Math.min( pUser.maxmana, ( pUser.mana + RandomNumber( 25, 40 )));
	pUser.SysMessage( GetDictionaryEntry( 18506, pSock.language )); // The sour draught instantly restores some of your mana!
	pUser.StaticEffect( 0x376A, 0x09, 0x06 );
	pUser.SoundEffect( 0x01E3, true );
	if( pUser.id > 0x0189 && !pUser.isonhorse )
	{
		pUser.DoAction( 0x22 )
	}

	// Disallow immediately using another potion
	pUser.isUsingPotion = true;
	DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 );

	// Start cooldown timer on pUser
	pUser.SetTag( "hasConsumedMD", true );
	pUser.StartTimer( manaDraughtTimer, 1, true );

	// Consume the item
	if( iUsed.amount > 1 )
	{
		iUsed.amount--;
	}
	else
	{
		iUsed.Delete();
	}
}

function ConsumeSeedOfLife( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pUser.GetTag( "hasConsumedSoL" ) == 1 )
	{
		var activeSeedTimer = pUser.GetJSTimer( 2, 5044 );
		if( activeSeedTimer > 0 )
		{
			var timeLeft = (( activeSeedTimer - GetCurrentClock() ) / 60000 ).toFixed( 0 );
			pSock.SysMessage( GetDictionaryEntry( 18504, pSock.language ), timeLeft ); // You must wait %i seconds before you can use this item.
			return;
		}
		else
		{
			pUser.SetTag( "hasConsumedSoL", null );
		}
	}

	if( pUser.health == pUser.maxhealth )
	{
		pSock.SysMessage( GetDictionaryEntry( 18507, pSock.language )); // You are already at full health.
		return;
	}

	pUser.health = Math.min( pUser.maxhealth, ( pUser.health + RandomNumber( 25, 40 )));
	pUser.SysMessage( GetDictionaryEntry( 18508, pSock.language )); // The bitter seed instantly restores some of your health!

	// Play some effects
	pUser.StaticEffect( 0x376A, 0x09, 0x06 );
	pUser.SoundEffect( 0x01E3, true );
	if( pUser.id > 0x0189 && !pUser.isonhorse )
	{
		pUser.DoAction( 0x22 )
	}

	// Disallow immediately using another potion
	pUser.isUsingPotion = true;
	DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 );

	// Start cooldown timer on pUser
	pUser.SetTag( "hasConsumedSoL", true );
	pUser.StartTimer( seedOfLifeTimer, 2, true );

	// Consume the item
	if( iUsed.amount > 1 )
	{
		iUsed.amount--;
	}
	else
	{
		iUsed.Delete();
	}
}

function ConsumeBalmLotion( pUser, iUsed, balmType )
{
	var pSock = pUser.socket;
	if( pUser.GetTag( "activeBalmLotion" ) > 0 )
	{
		pSock.SysMessage( GetDictionaryEntry( 18509, pSock.language )); // You are already under the effect of a balm or lotion.
		return;
	}

	switch( balmType )
	{
		case 1: // Balm of Protection
			// Reduce damage sustained from acid (grizzle), AoE cold (prism icy water, ice elems),
			// fire (blazing gargoyles) by 50-100%, rolled per tick. Does not protect against spells or weapons
			// Lasts 30 minutes
			pUser.SetTag( "activeBalmLotion", 1 );
			pUser.SysMessage( GetDictionaryEntry( 18510, pSock.language )); // You apply the ointment and suddenly feel less vulnerable!
			pUser.StartTimer( balmLotionTimer, 3, true );
			break;
		case 2: // Balm of Strength
			// Player gains 10 Str
			// Lasts 30 minutes
			pUser.SetTag( "activeBalmLotion", 2 );
			pUser.SysMessage( GetDictionaryEntry( 18511, pSock.language )); // You apply the balm and suddenly feel stronger!
			pUser.strength += 10;
			pUser.StartTimer( balmLotionTimer, 4, true );
			break;
		case 3: // Balm of Swiftness
			// Player gains 10 Dex
			// Lasts 30 minutes
			pUser.SetTag( "activeBalmLotion", 3 );
			pUser.SysMessage( GetDictionaryEntry( 18512, pSock.language )); // You apply the balm and suddenly feel more agile!
			pUser.dexterity += 10;
			pUser.StartTimer( balmLotionTimer, 5, true );
			break;
		case 4: // Balm of Wisdom
			// Player gains 10 Int
			// Lasts 30 minutes
			pUser.SetTag( "activeBalmLotion", 4 );
			pUser.SysMessage( GetDictionaryEntry( 18513, pSock.language )); // You apply the balm and suddenly feel wiser!
			pUser.intelligence += 10;
			pUser.StartTimer( balmLotionTimer, 6, true );
			break;
		case 5: // Life Shield Lotion
			// Reduce damage from Life Drain (Succubus, Tentacles of the Harrower) by 50-100%, rolled each tick
			// Lasts 30 minutes
			pUser.SetTag( "activeBalmLotion", 5 );
			pUser.SysMessage( GetDictionaryEntry( 18514, pSock.language )); // You apply the Life Shield Lotion.
			pUser.StartTimer( balmLotionTimer, 7, true );
			break;
		case 6: // Stone Skin Lotion
			// Player gains 30 physical resist, but loses 5 fire & cold resist
			// Lasts 30 minutes
			pUser.SetTag( "activeBalmLotion", 6 );
			pUser.SysMessage( GetDictionaryEntry( 18515, pSock.language )); // You apply the Stone Skin Lotion.
			pUser.StartTimer( balmLotionTimer, 8, true );
			pUser.Resist( 1, pUser.Resist( 1 ) + 30 );
			pUser.Resist( 4, pUser.Resist( 4 ) - 5 );
			pUser.Resist( 5, pUser.Resist( 5 ) - 5 );
			break;
		default:
			return;
	}

	// Play some effects associated with consuming item
	pUser.StaticEffect( 0x376A, 0x09, 0x06 );
	pUser.SoundEffect( 0x01E3, true );
	if( pUser.id > 0x0189 && !pUser.isonhorse )
	{
		pUser.DoAction( 0x22 )
	}

	// Disallow immediately using another potion
	pUser.isUsingPotion = true;
	DoTempEffect( 0, pUser, pUser, 26, 0, 0, 0 );

	// Consume the item
	if( iUsed.amount > 1 )
	{
		iUsed.amount--;
	}
	else
	{
		iUsed.Delete();
	}
}

function onTimer( timerObj, timerID )
{
	if( timerID >= 3 && timerID <= 8 )
	{
		timerObj.SetTag( "activeBalmLotion", null );
		if( timerObj.socket != null )
		{
			timerObj.socket.SysMessage( GetDictionaryEntry( 18516, pSock.language )); // The effects of the balm or lotion have worn off.
		}
	}

	switch( timerID )
	{
		case 1: // Mana Draught
			timerObj.SetTag( "hasConsumedMD", null );
			break;
		case 2: // Seed of Life
			timerObj.SetTag( "hasConsumedSoL", null );
			break;
		case 3: // Balm of Protection
			// No change, just needs removal of tag to deactivate effect
			break;
		case 4: // Balm of Strength
			timerObj.strength -= 10;
			break;
		case 5: // Balm of Swiftness
			timerObj.dexterity -= 10;
			break;
		case 6: // Balm of Wisdom
			timerObj.intelligence -= 10;
			break;
		case 7: // Life Shield Lotion
			// No change, just needs removal of tag to deactivate effect
			break;
		case 8: // Stone Skin Lotion
			timerObj.Resist( 1, timerObj.Resist( 1 ) - 30 );
			timerObj.Resist( 4, timerObj.Resist( 4 ) + 5 );
			timerObj.Resist( 5, timerObj.Resist( 5 ) + 5 );
			break;
		default:
			break;
	}
}

function _restorecontext_() {}
