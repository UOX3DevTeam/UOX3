function SkillRegistration()
{
	RegisterSkill( 16, true );	// Evaluate Intel
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 858, pSock.Language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		var ourInt = ourObj.intelligence;
		if( !pUser.CheckSkill( 16, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 1504, pSock.Language ) );
		else
		{
			var intString;
			if( ourInt < 100 )
				intString = GetDictionaryEntry( (parseInt( ourInt/10 )+1558), pSock.Language );
			else
				intString = GetDictionaryEntry( 1567, pSock.Language );
			pSock.SysMessage( GetDictionaryEntry( 1568, pSock.Language ) + " " + intString );
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 1557, pSock.Language ) );
}
