/// <reference path="../definitions.d.ts" />
// @ts-check
function SkillRegistration()
{
	RegisterSkill( 32, true );	// Spirit Speak
}

/** @type { ( skillUse: BaseObject, skillUsed: number, objType: 0 | 1 ) => boolean } */
function onSkill( pUser, skillUsed, objType )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( !pUser.CheckSkill( 32, 0, pUser.skillCaps.spiritspeak ))
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


