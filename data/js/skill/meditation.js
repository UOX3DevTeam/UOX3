/// <reference path="../definitions.d.ts" />
// @ts-check
function SkillRegistration()
{
	RegisterSkill( 46, true ); // Meditation
}

const coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));

/** @type { ( skillUse: BaseObject, skillUsed: number, objType: 0 | 1 ) => boolean } */
function onSkill( pUser, skillUsed, objType )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		// Fetch skill-specific skill delay to use on failure
		var pSkillDelay = Skills[46].skillDelay;

		if(( coreShardEra <= EraStringToNum( "lbr" ) && pUser.Defense( 0, 1, false, true ) > 10 ) || ( coreShardEra > EraStringToNum( "aos" ) && pUser.Defense( 0, 1, true, true ) > 0 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 967, pSock.language )); // The energies cannot be absorbed through your armour.
			pUser.isMeditating = false;
		}
		else if( CheckHands( pUser ))
		{
			pSock.SysMessage( GetDictionaryEntry( 968, pSock.language )); // You cannot meditate with a weapon or shield equipped!
			pUser.isMeditating = false;
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, ( pSkillDelay * 1000 ) / 4 );
		}
		else if( pUser.mana == pUser.maxmana )
		{
			pSock.SysMessage( GetDictionaryEntry( 969, pSock.language )); // You are at peace.
			pUser.isMeditating = false;
			pSock.SetTimer( Timer.SOCK_SKILLDELAY, ( pSkillDelay * 1000 ) / 2 );
		}
		else if( !pUser.CheckSkill( 46, 0, pUser.skillCaps.meditation ))
		{
			pSock.SysMessage( GetDictionaryEntry( 970, pSock.language )); // You cannot focus your concentration.
			pUser.isMeditating = false;
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 971, pSock.language )); // You enter a meditative trance.
			pUser.isMeditating = true;
			pUser.SoundEffect( 0x00F9, true );
		}
	}
	return true;
}

function CheckHands( pUser )
{
	var retVal = null;
	var rHand = pUser.FindItemLayer( 0x01 ); // Right Hand
	if( ValidateObject( rHand ))
	{
		if( rHand.type != 9 ) // Spellbook
		{
			retVal = rHand;
		}
	}
	if( !retVal )
	{
		var lHand = pUser.FindItemLayer( 0x02 ); // Left Hand;
		if( ValidateObject( lHand ))
		{
			retVal = lHand;
		}
	}
	return retVal;
}

