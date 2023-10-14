function SkillRegistration()
{
	RegisterSkill( 30, true );	// Poisoning
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 869, pSock.language )); // What poison do you want to apply?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isItem && pUser )
	{
		if( ourObj.type != 19 || ourObj.morey != 6 )
		{
			pSock.SysMessage( GetDictionaryEntry( 918, pSock.language )); // That is not a valid poison!
			return;
		}
		pSock.tempObj = ourObj;
		pSock.CustomTarget( 1, GetDictionaryEntry( 1613, pSock.language )); // What item do you want to poison?
	}
}

function onCallback1( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	var pPotion = pSock.tempObj;
	pSock.tempObj = null;
	if( ourObj && ourObj.isItem && pUser && pPotion && pPotion.isItem )
	{
		if( ourObj.movable == 2 || ourObj.movable == 3 )
		{
			pSock.SysMessage( GetDictionaryEntry( 774, pLanguage )); // That is locked down and you cannot use it.
			return;
		}
		var pLanguage 	= pSock.language;
		var ourLoDmg 	= ourObj.lodamage;
		var ourHiDmg 	= ourObj.hidamage;
		var ourID		= ourObj.id;

		if( ourLoDmg == 0 && ourHiDmg == 0 && ourObj.type != 14 )
		{
			pSock.SysMessage( GetDictionaryEntry( 1648, pLanguage )); // You can only poison weapons and food
		}
		else if(( ourID >= 0x0F4F && ourID <= 0x0F50 ) || ( ourID >= 0x13B1 && ourID <= 0x13B2 ) || ( ourID >= 0x13FC && ourID <= 0x13FD ))
		{
			pSock.SysMessage( GetDictionaryEntry( 1647, pLanguage )); // Bows cannot be poisoned
		}
		else
		{
			var canPoison = false;
			switch( pPotion.morez )
			{
				case 1: canPoison = pUser.CheckSkill( 30, 0, 500 );		break;	// Lesser Poison
				case 2: canPoison = pUser.CheckSkill( 30, 151, 651 );	break;	// Poison
				case 3: canPoison = pUser.CheckSkill( 30, 551, 1051 );	break;	// Greater Poison
				case 4: canPoison = pUser.CheckSkill( 30, 901, 1401 );	break;	// Deadly Poison
				default:
					pSock.SysMessage( GetDictionaryEntry( 918, pLanguage )); // That is not a valid poison!
					return;
			}

			if( canPoison )
			{
				if( ourObj.poison >= pPotion.morez )
				{
					pSock.SysMessage( GetDictionaryEntry( 6020, pLanguage )); // That item is already poisoned!
					return;
				}
				ourObj.poison = pPotion.morez;
				pSock.SysMessage( GetDictionaryEntry( 919, pLanguage )); // You successfully poison that item.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1649, pLanguage )); // You fail to apply the poison.
			}

			pUser.SoundEffect( 0x0247, true );
			if( pPotion.amount > 1 )
			{
				pPotion.amount = pPotion.amount - 1;
			}
			else
			{
				pPotion.Delete();
			}

			var pBottle = CreateDFNItem( pSock, pUser, "0x0F0E", 1, "ITEM", true )
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 920, pSock.language )); // You can't poison that item.
	}
}
