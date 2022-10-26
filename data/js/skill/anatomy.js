function SkillRegistration()
{
	RegisterSkill( 1, true );	// Anatomy
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 856, pSock.language )); // Whom shall I examine?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		var pLanguage = pSock.language;
		if( ourObj.dead )
		{
			pSock.SysMessage( GetDictionaryEntry( 1571, pLanguage )); // That does not appear to be a living being.
		}
		else if( ourObj == pUser )
		{
			pSock.SysMessage( GetDictionaryEntry( 9123, pLanguage )); // You know yourself quite well enough already.
		}
		else if( !ourObj.InRange( pUser, 7 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pLanguage )); // That is too far away.
		}
		else if( !pUser.CheckSkill( 1, 0, 1000 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 1504, pLanguage )); // You are not certain...
		}
		else
		{
			var ourStr = ourObj.strength;
			var strString;
			if( ourStr < 100 )
			{
				strString = GetDictionaryEntry(( parseInt( ourStr / 10 ) + 1572 ), pLanguage );
			}
			else
			{
				strString = GetDictionaryEntry( 1581, pLanguage ); // superhumanly strong
			}

			var ourDex = ourObj.dexterity;
			var dexString;
			if( ourDex < 100 )
			{
				dexString = GetDictionaryEntry(( parseInt( ourDex / 10 ) + 1582 ), pLanguage );
			}
			else
			{
				dexString = GetDictionaryEntry( 1591, pLanguage ); // superhumanly agile
			}

			var tempMsg = GetDictionaryEntry( 6000, pLanguage ); // That person looks %s and %t.
			tempMsg = ( tempMsg.replace( /%s/gi, strString ));
			tempMsg = ( tempMsg.replace( /%t/gi, dexString ));
			pSock.SysMessage( tempMsg );
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 1569, pSock.language )); // That is not a player!
}
