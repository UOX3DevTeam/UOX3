function SkillRegistration()
{
	RegisterSkill( 19, true );	// Forensics
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 868, pSock.language )); // What corpse do you want to examine?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isItem && pUser )
	{
		var pLanguage = pSock.language;
		if( !ourObj.corpse )
		{
			pSock.SysMessage( GetDictionaryEntry( 909, pLanguage )); // That does not appear to be a corpse.
		}
		else if( !ourObj.InRange( pUser, 7 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pLanguage )); // That is too far away.
		}
		else if( !pUser.CheckSkill( 19, 0, 1000 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 911, pLanguage )); // You are not certain about the corpse.
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 6007, pLanguage )); // You examine the body..."
			var timeSinceDeath = parseInt(( GetCurrentClock() / 1000 ) - ourObj.tempTimer );
			if( timeSinceDeath > 180 )
			{
				pSock.SysMessage( GetDictionaryEntry( 6008, pLanguage )); // It stinks quite a lot by now.
			}
			else if( timeSinceDeath > 60 )
			{
				pSock.SysMessage( GetDictionaryEntry( 6009, pLanguage )); // It is beginning to smell a bit.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6010, pLanguage )); // It looks to have been freshly... planted.
			}

			var kObj = CalcCharFromSer( ourObj.morex );
			if( kObj && kObj.isChar )
			{
				var tempName = GetDictionaryEntry( 6011, pLanguage ); // The killer was %s.
				tempName = ( tempName.replace( /%s/gi, kObj.name ));
				pSock.SysMessage( tempName );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6012, pLanguage )); // The killer left no traces for you to find.
			}
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 909, pSock.language )); // That does not appear to be a corpse.
	}
}
