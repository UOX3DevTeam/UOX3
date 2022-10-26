function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		socket.tempObj = iUsed;
		var targMsg = GetDictionaryEntry( 485, socket.language ); // What item would you like to repair?
		socket.CustomTarget( 1, targMsg );
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var mChar = socket.currentChar;
	var bItem = socket.tempObj;
	socket.tempObj = null;

	if( mChar && mChar.isChar && bItem && bItem.isItem )
	{
		if( !ourObj || !ourObj.isItem )
		{
			socket.SysMessage( GetDictionaryEntry( 986, socket.language )); // You cannot repair that.
			return;
		}

		var minSkill = 999;
		var maxSkill = 1000;
		// This needs to use GetWeaponType eventually
		var weaponID = ourObj.id;
		if( weaponID == 0x13B1 || weaponID == 0x13B2 || weaponID == 0x26C2 || weaponID == 0x26CC ||
		    weaponID == 0x2571 || weaponID == 0x0F4F || weaponID == 0x0F50 || weaponID == 0x13FC ||
		    weaponID == 0x13FD || weaponID == 0x26C3 || weaponID == 0x26CD || weaponID == 0x27A5 ||
		    weaponID == 0x27F0 )	// Is it a bow?
		{
			var ownerObj = GetPackOwner( ourObj, 0 );
			if( ownerObj && mChar.serial == ownerObj.serial )
			{
				if( ourObj.health < ourObj.maxhp )
				{
					if( ourObj.hidamage > 0 )
					{
						var offset = (( ourObj.lodamage + ourObj.hidamage ) / 2 );
						if( offset <= 25 )
						{
							minSkill = ( 510 + (( offset - 1 ) / 5 ) * 100 );
							maxSkill = ( 799 + (( offset - 1 ) / 5 ) * 50 );
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 987, socket.language )); // You cannot repair this item.
						return;
					}

					if( mChar.CheckSkill( 8, minSkill, maxSkill, true ) )
					{
						ourObj.health = ourObj.maxhp;
						socket.SysMessage( GetDictionaryEntry( 989, socket.language )); // You repair the item successfully.
						socket.SoundEffect( 0x002A, true );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 990, socket.language )); // You fail to repair the item.
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 988, socket.language )); // That item is already fully repaired.
				}
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 6021, socket.language )); // You must have that item in your pack to repair it!
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 986, socket.language )); // You cannot repair that.
		}
	}
}

