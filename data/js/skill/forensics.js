function SkillRegistration()
{
	RegisterSkill( 19, true );	// Forensics
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 868, pSock.language ) ); // What corpse do you want to examine?

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isItem && pUser )
	{
		var pLanguage = pSock.language;
		if( !ourObj.corpse )
			pSock.SysMessage( GetDictionaryEntry( 909, pLanguage ) ); // That does not appear to be a corpse.
		else if( !ourObj.InRange( pUser, 7 ) )
			pSock.SysMessage( "That is too far away." );
		else if( !pUser.CheckSkill( 19, 0, 1000 ) )
			pSock.SysMessage( GetDictionaryEntry( 911, pLanguage ) ); // You are not certain about the corpse.
		else
		{
			pSock.SysMessage( "You examine the body..." );
			var timeSinceDeath = parseInt( (GetCurrentClock() - ourObj.tempTimer) / 1000 );
			if( timeSinceDeath > 180 )
				pSock.SysMessage( "It stinks quite a lot by now." );
			else if( timeSinceDeath > 60 )
				pSock.SysMessage( "It is beginning to smell a bit." );
			else
				pSock.SysMessage( "It looks to have been freshly... planted." );

			var kObj = CalcCharFromSer( ourObj.morex );
			if( kObj && kObj.isChar )
				pSock.SysMessage( "The killer was " + kObj.name + "." );
			else
				pSock.SysMessage( "The killer left no traces for you to find." );
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 909, pSock.language ) ); // That does not appear to be a corpse.
}
