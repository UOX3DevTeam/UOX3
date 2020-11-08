function SkillRegistration()
{
	RegisterSkill( 6, true );	// Begging
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 866, pSock.language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		var pLanguage 	= pSock.language;
		if( !ourObj.InRange( pUser, 3 ) )
			pSock.SysMessage( GetDictionaryEntry( 901, pLanguage ) );
		else if( !ourObj.npc )
			pSock.SysMessage( GetDictionaryEntry( 899, pLanguage ) );
		else if( ourObj.aiType == 17 ) // Player Vendor
			pSock.SysMessage( GetDictionaryEntry( 900, pLanguage ) );
		else if( !ourObj.isHuman )
			pSock.SysMessage( GetDictionaryEntry( 905, pLanguage ) );
		else if( !pUser.CheckSkill( 6, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 902, pLanguage ) );
		else
		{
			ourObj.TextMessage( GetDictionaryEntry( 903, pLanguage ) );
			CreateDFNItem( pUser.socket, pUser, "0x0eed", parseInt(10 + (RandomNumber( 0, (pUser.skills.begging + 1) ) / 25)), "ITEM", true );
			pSock.SysMessage( GetDictionaryEntry( 904, pLanguage ) );
		}
	}
	else
		pSock.SysMessage( "You cannot beg from that." );
}