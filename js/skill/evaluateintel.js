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
			if( ourInt < 10 )
				intString = GetDictionaryEntry( 1558, pSock.Language );
			else if( ourInt < 20 )
				intString = GetDictionaryEntry( 1559, pSock.Language );
			else if( ourInt < 30 )
				intString = GetDictionaryEntry( 1560, pSock.Language );
			else if( ourInt < 40 )
				intString = GetDictionaryEntry( 1561, pSock.Language );
			else if( ourInt < 50 )
				intString = GetDictionaryEntry( 1562, pSock.Language );
			else if( ourInt < 60 )
				intString = GetDictionaryEntry( 1563, pSock.Language );
			else if( ourInt < 70 )
				intString = GetDictionaryEntry( 1564, pSock.Language );
			else if( ourInt < 80 )
				intString = GetDictionaryEntry( 1565, pSock.Language );
			else if( ourInt < 90 )
				intString = GetDictionaryEntry( 1566, pSock.Language );
			else
				intString = GetDictionaryEntry( 1567, pSock.Language );
			pSock.SysMessage( GetDictionaryEntry( 1568, pSock.Language ) + " " + intString );
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 1557, pSock.Language ) );
}
