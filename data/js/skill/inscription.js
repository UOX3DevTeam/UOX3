function SkillRegistration()
{
	RegisterSkill( 23, true )	// Inscription
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 865, pSock.Langauge ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( pUser ) && ValidateObject( ourObj ) && ourObj.isItem )
	{
		if( ourObj.id == 0x0E34 || ourObj.id == 0x0EF3 )
		{
			var pPack = pUser.pack;
			if( ValidateObject( pPack ) )
			{
				if( !ValidateObject( pUser.FindItemType( 9 ) ) )	// Do they have a spellbook?
				{
					pSock.SysMessage( GetDictionaryEntry( 921, pSock.Language ) );
					return;
				}
	
				var ownerObj = GetPackOwner( ourObj, 0 );
				if( ownerObj && pUser.serial == ownerObj.serial )
					pSock.MakeMenu( 99, 23 );
				else
					pSock.SysMessage( GetDictionaryEntry( 778, pSock.Language ) );
			}
			else
				pSock.SysMessage( GetDictionaryEntry( 773, pSock.Language ) );
		}
		else
			pSock.SysMessage( "You can make nothing useful from that." );
	}
}

