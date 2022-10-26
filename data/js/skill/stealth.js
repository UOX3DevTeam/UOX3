function SkillRegistration()
{
	RegisterSkill( 47, true );	// Stealth
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( pUser.isonhorse )
		{
			pSock.SysMessage( GetDictionaryEntry( 837, pSock.language )); // You cannot stealth while mounted.
		}
		else if( pUser.visible != 1 )
		{
			pSock.SysMessage( GetDictionaryEntry( 836, pSock.language )); // You must hide first.
		}
		else if( pUser.skills.hiding < 700 )
		{
			pSock.SysMessage( GetDictionaryEntry( 838, pSock.language )); // You are not hidden well enough. Become better at hiding.
			pUser.visible = 0;
			pUser.stealth = -1;
		}
		else if( !pUser.CheckSkill( 47, 0, 1000 ) )
		{
			pUser.visible = 0;
			pUser.stealth = -1;
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 839, pSock.language )); // You begin to move quietly.
			pUser.visible = 1;
			pUser.stealth = 0;
		}
	}
	return true;
}
