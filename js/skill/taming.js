function SkillRegistration()
{
	RegisterSkill( 35, true );	// Animal Taming
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 859, pSock.Language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		var skillToTame = ourObj.skillToTame;
		var pLanguage 	= pSock.Language;
		if( !ourObj.InRange( pUser, 3 ) )
			pSock.SysMessage( "That is too far away." )
		else if( !ourObj.npc || ( ourObj.id == 0x0190 || ourObj.id == 0x0191 ) )
			pSock.SysMessage( "That doesn't look much like an animal." );
		else if( skillToTame == 0 || skillToTame > 1000 )
			pSock.SysMessage( GetDictionaryEntry( 1593, pLanguage ) );
		else if( ourObj.tamed )
		{
			if( ourObj.owner && ourObj.owner.serial == pUser.serial )
				pSock.SysMessage( GetDictionaryEntry( 1594, pLanguage ) );
			else
				pSock.SysMessage( GetDictionaryEntry( 1595, pLanguage ) );
		}
		else if( pUser.skills.taming < skillToTame )
			pSock.SysMessage( "You do not have enough skill to tame that creature." );
		else
		{
			for( var i = 0; i < 3; ++i )
			{
				switch( RandomNumber( 0, 3 ) )
				{
					case 0: pUser.TextMessage( GetDictionaryEntry( 1597, pLanguage ) );	break;
					case 1: pUser.TextMessage( GetDictionaryEntry( 1598, pLanguage ) );	break;
					case 2:	pUser.TextMessage( "Here " + ourObj.name );			break;
					case 3: pUser.TextMessage( "Good " + ourObj.name );			break;
				}
			}
			if( !pUser.CheckSkill( 35, skillToTame, 1000 ) )
				pSock.SysMessage( "You fail to tame that creature" );
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1602, pLanguage ) );
				ourObj.owner 		= pUser;
				ourObj.wandertype 	= 0;
				ourObj.aitype		= 0;
				ourObj.tamed		= true;
				if( ourObj.atWar )
				{
					ourObj.target 	= null;
					ourObj.attack 	= null;
					ourObj.atWar	= false;
				}
				if( pUser.atWar && pUser.target && pUser.target.serial == ourObj.serial )
				{
					pUser.target	= null;
					pUser.attack	= null;
					pUser.atWar	= false;
				}
			}
		}
	}
	else
		pSock.SysMessage( "That doesn't look much like an animal." );
}

