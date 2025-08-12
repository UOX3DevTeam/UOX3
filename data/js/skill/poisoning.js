/// <reference path="../definitions.d.ts" />
// @ts-check
function SkillRegistration()
{
	RegisterSkill( 30, true );	// Poisoning
}

/** @type { ( skillUse: BaseObject, skillUsed: number, objType: 0 | 1 ) => void } */
function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 869, pSock.language )); // What poison do you want to apply?
	}

	return true;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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

		var weaponType = TriggerEvent( 2500, "GetWeaponType", pUser, ourObj.id );
		var weaponCombatSkill = TriggerEvent( 2500, "GetCombatSkill", weaponType );

		// Before UOR, all bladed/piercing weapons could be poisoned
		const coreShardEra = GetServerSetting( "CoreShardEra" );

		if( ourLoDmg == 0 && ourHiDmg == 0 && ourObj.type != 14 )
		{
			pSock.SysMessage( GetDictionaryEntry( 1648, pLanguage )); // You can only poison weapons and food
		}
		else if( coreShardEra >= EraStringToNum( "ml" )) // ML (or KR)/Publish46
		{
			// After Pub46, only weapons with "Infectious Strike" move can be poisoned
			pSock.SysMessage( GetDictionaryEntry( 6314, pLanguage )); // You cannot poison that! You can only poison infectious weapons and food.
		}
		else if( coreShardEra >= EraStringToNum( "uor" ) && weaponType != "DEF_SWORDS" && weaponType != "SLASH_SWORDS" && weaponType != "ONEHND_LG_SWORDS" && weaponType != "ONEHND_AXES" && weaponType != "BLOWGUNS" && weaponType != "DEF_FENCING" ) // UO:R era
		{
			// After UOR, only one-handed bladed/piercing weapons can be poisoned
			pSock.SysMessage( GetDictionaryEntry( 6313, pLanguage )); // You cannot poison that! You can only poison one-handed bladed or one-handed piercing weapons, food or drink.
		}
		else if( coreShardEra < EraStringToNum( "uor" ) && weaponCombatSkill != "SWORDSMANSHIP" && weaponCombatSkill != "FENCING" && weaponType != "BLOWGUNS" )
		{
			pSock.SysMessage( GetDictionaryEntry( 6310, pLanguage )); // You cannot poison that! You can only poison bladed or piercing weapons, food or drink.
		}
		else
		{
			var canPoison = false;
			switch( pPotion.morez )
			{
				case 1: canPoison = pUser.CheckSkill( 30, 0, 500 );		break;	// Lesser Poison
				case 2: canPoison = pUser.CheckSkill( 30, 151, 651 );	break;	// Poison
				case 3: canPoison = pUser.CheckSkill( 30, 551, 1051 );	break;	// Greater Poison
				case 4: canPoison = pUser.CheckSkill( 30, 901, pUser.skillCaps.poisoning );	break;	// Deadly Poison
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
				ourObj.poisonedBy = pUser.serial;
				ourObj.poisonCharges = 18 - ( pPotion.morez * 2 );
				ourObj.Refresh();
				pSock.SysMessage( GetDictionaryEntry( 919, pLanguage )); // You successfully poison that item.
				pUser.karma -= 20;
				pSock.SysMessage( GetDictionaryEntry( 1370, pLanguage )); // You have lost some karma.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1649, pLanguage )); // You fail to apply the poison.

				// 5% chance to poison self on failure, with a dosage lower than the poison used
				if( pUser.skills.poisoning < 800 && RandomNumber( 1, 20 ) == 1 )
				{
					pSock.SysMessage( GetDictionaryEntry( 6315, socket.language )); // You make a grave mistake while applying the poison.
					pUser.SetPoisoned( Math.max( 1, ourObj.poison - 1 ), 180 * 1000, pUser );
				}
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
