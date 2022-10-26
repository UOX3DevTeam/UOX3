function SkillRegistration()
{
	RegisterSkill( 14, true );	// Detecting Hidden
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 860, pSock.language )); // Where do you wish to search for hidden characters?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( pUser && pUser.isChar )
	{
		// This needs to eventually use MAX_VISRANGE and the Race Range
		var detectRange   = parseInt(((( 15 - 1 ) / 1000 ) * pUser.skills.detectinghidden ) + 1 );
		var detectCounter = AreaCharacterFunction( "DetectHiddenLoop", pUser, detectRange, pSock );
		if( detectCounter != 0 )
		{
			pSock.SysMessage( GetDictionaryEntry( 1437, pSock.language )); // You fail to find anyone.
		}
	}
}

function DetectHiddenLoop( srcChar, trgChar, pSock )
{
	if( srcChar && srcChar.isChar && trgChar && trgChar.isChar )
	{
		if( trgChar.visible == 0 || trgChar.visible == 3 )
			return false;

		var trgMulti = trgChar.multi;
		var srcMulti = srcChar.multi;

		if( ValidateObject( trgMulti ) && ( trgMulti.IsOnOwnerList( srcChar ) || trgMulti.IsOnFriendList( srcChar )))
		{
			// Guaranteed success when detecting stealthed players house you own/are friend of
			trgChar.visible = 0;
			trgChar.stealth = -1;
			var tSock = trgChar.socket;
			if( tSock )
			{
				tSock.SysMessage( GetDictionaryEntry( 1436, tSock.language ));
			}
			return true;
		}
		else
		{
			var distance       = srcChar.DistanceTo( trgChar );
			var checkSkill     = 0;
			var hidingSkill    = trgChar.skills.hiding;
			var chanceToDetect = parseInt(( distance * 25 ) + ( hidingSkill / 2 ));

			if( chanceToDetect >= hidingSkill )
			{
				checkSkill = hidingSkill;
			}
			else
			{
				checkSkill = RandomNumber( chanceToDetect, hidingSkill );
			}

			if( srcChar.CheckSkill( 14, checkSkill, 1000 ))
			{
				trgChar.visible = 0;
				trgChar.stealth = -1;
				var tSock = trgChar.socket;
				if( tSock )
				{
					tSock.SysMessage( GetDictionaryEntry( 1436, tSock.language ));
				}
				return true;
			}
		}
	}
	return false;
}
