function SkillRegistration()
{
	RegisterSkill( 1, true );	// Anatomy
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 856, pSock.language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		var pLanguage = pSock.language;
		if( ourObj.dead )
			pSock.SysMessage( GetDictionaryEntry( 1571, pLanguage ) );
		else if( !ourObj.InRange( pUser, 7 ) )
			pSock.SysMessage( "That is too far away." );
		else if( !pUser.CheckSkill( 1, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 1504, pLanguage ) );
		else
		{
			var ourStr = ourObj.strength;
			var strString;
			if( ourStr < 100 )
				strString = GetDictionaryEntry( (parseInt( ourStr/10 )+1572), pLanguage );
			else
				strString = GetDictionaryEntry( 1581, pLanguage );

			var ourDex = ourObj.dexterity;
			var dexString;
			if( ourDex < 100 )
				dexString = GetDictionaryEntry( (parseInt( ourDex/10 )+1582), pLanguage );
			else
				dexString = GetDictionaryEntry( 1591, pLanguage );

			pSock.SysMessage( "That person looks " + strString + " and " + dexString + "." );
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 1569, pSock.language ) );
}
