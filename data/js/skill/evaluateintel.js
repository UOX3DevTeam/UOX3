function SkillRegistration()
{
	RegisterSkill( 16, true );	// Evaluate Intel
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 858, pSock.language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		if( ourObj.dead )
			pSock.SysMessage( GetDictionaryEntry( 1571, pSock.language ) );
		else if( !ourObj.InRange( pUser, 7 ) )
			pSock.SysMessage( "That is too far away." );
		else if( !pUser.CheckSkill( 16, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 1504, pSock.language ) );
		else
		{
			var ourInt = ourObj.intelligence;
			var intString;
			var outMessage;
			if( ourInt < 100 )
				intString = GetDictionaryEntry( (parseInt( ourInt/10 )+1558), pSock.language );
			else
				intString = GetDictionaryEntry( 1567, pSock.language );
			outMessage = GetDictionaryEntry( 1568, pSock.language );
			pSock.SysMessage( outMessage.replace(/%s/gi, intString ));
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 1557, pSock.language ) );
}
