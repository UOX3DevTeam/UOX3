function SkillRegistration()
{
	RegisterSkill( 32, true );	// Spirit Speak
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( !pUser.CheckSkill( 32, 0, 1000 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 1501, pSock.language )); // You fail your attempt at contacting the netherworld.
		}
		else
		{
			pUser.SoundEffect( 0x024A, true );
			pSock.SysMessage( GetDictionaryEntry( 1502, pSock.language )); // You establish a connection to the netherworld.
			pSock.SetTimer( Timer.SOCK_SPIRITSPEAK, ( 1000 * ( 30 + ( pUser.skills.spiritspeak / 10 ) + pUser.intelligence )));
		}
	}
	return true;
}


