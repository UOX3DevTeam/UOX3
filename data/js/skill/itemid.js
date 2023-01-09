function SkillRegistration()
{
	RegisterSkill( 3, true );	// Item Identification
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 857, pSock.language )); // What do you wish to appraise and identify?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( ourObj ) && ourObj.isItem && ValidateObject( pUser ))
	{
		var idFromWand = false;
		if( pSock.tempInt2 == 42 )
		{
			idFromWand = true;
			pSock.tempInt2 = null;
		}

		var pLanguage = pSock.language;
		if( ourObj.corpse )
		{
			pSock.SysMessage( GetDictionaryEntry( 1546, pLanguage )); // You have to use your forensics evaluation skill to know more about this corpse.
			return;
		}
		else if( ourObj.type == 19 ) // Potions
		{
			pSock.SysMessage( GetDictionaryEntry( 6278, pLanguage )); // You have to use your taste identification skill to know more about this potion.
			return;
		}
		else if( ourObj.isItemHeld || ourObj.worldnumber != pUser.worldnumber || ourObj.instanceID != pUser.instanceID )
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pLanguage )); // That is too far away.
			return;
		}
		else if( !pUser.InRange( ourObj, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pLanguage )); // That is too far away.
			return;
		}

		if( !idFromWand && !pUser.CheckSkill( 3, 250, 500 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 1545, pLanguage )); // You can't quite tell what this item is...
			return;
		}

		if( ourObj.name2 != "" && ourObj.name2 != "#" )
		{
			ourObj.name = ourObj.name2;
			ourObj.name2 = "#";
			ourObj.Refresh();
			ourObj.Teleport( ourObj );
		}

		var tempMsg = GetDictionaryEntry( 1547, pLanguage ); // You found that this item appears to be called: %s
		tempMsg = ( tempMsg.replace( /%s/gi, ourObj.name ));
		if( GetServerSetting( "RankSystem" ) && ourObj.rank == 10 )
		{
			tempMsg += " " + GetDictionaryEntry( 9140, pLanguage ); // of exceptional quality
		}
		ourObj.TextMessage( tempMsg, false, 0x3b2, 0, pUser.serial );

		var creatorSerial = ourObj.creator;
		if( creatorSerial )
		{
			var creator = CalcCharFromSer( creatorSerial );
			if( ValidateObject( creator ))
			{
				if( ourObj.madeWith > 0 )
				{
					let madeWord = Skills[ourObj.madeWith].madeWord;
					let tempMsg = GetDictionaryEntry( 1548, pLanguage ); // It is %s by %s.
					tempMsg = ( tempMsg.replace( /%s/gi, madeWord ));
					tempMsg = ( tempMsg.replace( /%w/gi, creator.name ));
				}
				else
				{
					let tempMsg = GetDictionaryEntry( 1549, pLanguage ); // It is made by %s.
					tempMsg = ( tempMsg.replace( /%s/gi, creator.name ));
				}

				ourObj.TextMessage( tempMsg, false, 0x3b2, 0, pUser.serial );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1550, pSock.language )); // You don't know its creator!
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 1550, pSock.language )); // You don't know its creator!
		}

		if( idFromWand || pUser.skills.itemid > 350 )
		{
			if( ourObj.type != 15 ) // IT_MAGICWAND
			{
				if( ourObj.name2 == "" || ourObj.name2 == "#" )
				{
					pSock.SysMessage( GetDictionaryEntry( 1553, pSock.language )); // This item has no hidden magical properties
				}
				return;
			}

			if( idFromWand || pUser.CheckSkill( 3, 500, 750 ))
			{
				var spellNum = ourObj.morey;
				var spellName = GetDictionaryEntry( 592 + spellNum, pSock.language );
				if( spellName != "" )
				{
					if( !idFromWand && !pUser.CheckSkill( 3, 750, 1000 ))
					{
						pSock.SysMessage( GetDictionaryEntry( 1555, pSock.language ), spellName ); // It is enchanted with the spell %s, but you cannot determine how many charges remain.
					}
					else
					{
						let spellCharges = ourObj.morez;
						pSock.SysMessage( GetDictionaryEntry( 1556, pSock.language ), spellName, spellCharges ); // It is enchanted with the spell %s, and has %d charges remaining
					}
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1554, pSock.language )); // This item is enchanted with a spell, but you cannot determine which.
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 1552, pSock.language )); // You can't tell if it is magical or not.
		}
	}
	else
	{
		if( pSock )
		{
			pSock.SysMessage( GetDictionaryEntry( 711, pSock.language )); // That is not a valid item.
		}
	}
}
