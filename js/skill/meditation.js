function SkillRegistration()
{
	RegisterSkill( 46, true );	// Meditation
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( pUser.defense > 10 )
		{
			pSock.SysMessage( GetDictionaryEntry( 967, pSock.Language ) );
			pUser.isMeditating = false;
		}
		else if( CheckHands( pUser ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 968, pSock.Language ) );
			pUser.isMeditating = false;
		}
		else if( pUser.mana == pUser.maxMana )
		{
			pSock.SysMessage( GetDictionaryEntry( 969, pSock.Language ) );
			pUser.isMeditating = false;
		}
		else if( !pUser.CheckSkill( 46, 0, 1000 ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 970, pSock.Language ) );
			pUser.isMeditating = false;
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 971, pSock.Language ) );
			pUser.isMeditating = true;
			pUser.SoundEffect( 0x00F9, true );
		}
	}
	return true;
}

function CheckHands( pUser )
{
	var retVal = null;
	var rHand = pUser.FindItemLayer( 0x01 );		// Right Hand
	if( ValidateObject( rHand ) )
	{
		if( rHand.type != 9 )				// Spellbook
			retVal = rHand;
	}
	if( !retVal )
	{
		var lHand = pUser.FindItemLayer( 0x02 );	// Left Hand;
		if( ValidateObject( lHand ) )
			retVal = lHand;
	}
	return retVal;
}

