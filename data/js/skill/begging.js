function SkillRegistration()
{
	RegisterSkill( 6, true );	// Begging
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 866, pSock.language )); // Whom do you wish to annoy?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		var pLanguage = pSock.language;
		if( !ourObj.InRange( pUser, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 901, pLanguage )); // You are not close enough to beg.
		}
		else if( !ourObj.npc )
		{
			pSock.SysMessage( GetDictionaryEntry( 899, pLanguage )); // Maybe you should just ask?
		}
		else if( ourObj.aiType == 17 ) // Player Vendor
		{
			pSock.SysMessage( GetDictionaryEntry( 900, pLanguage )); // This person is not susceptible to your pleading for gold.
		}
		else if( !ourObj.isHuman )
		{
			pSock.SysMessage( GetDictionaryEntry( 905, pLanguage )); // That would be foolish.
		}
		else if( !pUser.CheckSkill( 6, 0, 1000 ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 902, pLanguage )); // They seem to ignore your begging pleas.
		}
		else
		{
			ourObj.TextMessage( GetDictionaryEntry( 903, pLanguage ) ); // Ohh thou lookest so poor, here is some gold I hope this will assist thee.
			CreateDFNItem( pSock, pUser, "0x0eed", parseInt( 10 + ( RandomNumber( 0, ( pUser.skills.begging + 1 )) / 25 )), "ITEM", true );
			pSock.SysMessage( GetDictionaryEntry( 904, pLanguage )); // Some gold is placed in your pack.
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6005, pLanguage )); // You cannot beg from that.
	}
}