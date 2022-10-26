function SkillRegistration()
{
	RegisterSkill( 23, true );	// Inscription
}

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 6019, socket.language )); // This must be in your backpack or equipped before it can be used.
			return false;
		}
		else
		{
			socket.CustomTarget( 0, GetDictionaryEntry( 865, socket.language )); // What do you wish to place a spell on?
		}
	}
	return false;
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 865, pSock.language )); // What do you wish to place a spell on?
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( pUser ) && ValidateObject( ourObj ) && ourObj.isItem )
	{
		if( ourObj.id == 0x0E34 || ourObj.id == 0x0EF3 || ourObj.type == 50 )
		{
			if( ourObj.id == 0x1f14 )
			{
				pUser.SysMessage( "That rune is not blank." );
				return;
			}

			var pPack = pUser.pack;
			if( ValidateObject( pPack ) )
			{
				if( !ValidateObject( pUser.FindItemType( 9 ))) // Do they have a spellbook?
				{
					pSock.SysMessage( GetDictionaryEntry( 921, pSock.language )); // You don't have a spellbook to scribe from!
					return;
				}

				var ownerObj = GetPackOwner( ourObj, 0 );
				if( ownerObj && pUser.serial == ownerObj.serial )
				{
					pSock.MakeMenu( 99, 23 );
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 778, pSock.language )); // You can't use items outside your backpack.
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 773, pSock.language )); // Time to buy a backpack.
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 6001, pSock.language )); // You can't make anything useful from that.
		}
	}
}

