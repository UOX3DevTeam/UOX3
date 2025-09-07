/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pUser.socket.language )); // That item must be in your backpack before it can be used.
	}
	else
	{
		pUser.socket.tempObj = iUsed;
		pUser.CustomTarget( 0, GetDictionaryEntry( 2751, pUser.socket.language )); // Select the weapon or armour you wish to use the cloth on.
	}
	return false;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( pSock, myTarget )
{
	var iUsed = pSock.tempObj;
	var pUser = pSock.currentChar;
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		// That must be in your pack for you to use it.
		var itemOwner = GetPackOwner( myTarget, 0 );
		if( itemOwner == null || itemOwner != pUser )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
		}
		else
		{
			let itemPoisonLvl = myTarget.poison;
			if( itemPoisonLvl > 0 )
			{
				if( itemPoisonLvl > 2 )
				{
					itemPoisonLvl -= 2;
				}
				else
				{
					itemPoisonLvl = 0;
				}
				if( itemPoisonLvl == 0 )
				{
					pSock.SysMessage( GetDictionaryEntry( 2752, pSock.language )); // You carefully wipe the item clean of poison.
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 2753, pSock.language )); // You carefully wipe the item, but some poison still lingers.
				}
				pUser.UseResource( 1, 0x175d, 0x07d1 );
			}
			else if( myTarget.GetTag( "corrosionLevel" ) > 0 )
			{
				myTarget.SetTag( "corrosionLevel", null );
				pSock.SysMessage( GetDictionaryEntry( 6317, pSock.language )); // You have cleaned the item.
				pUser.UseResource( 1, 0x175d, 0x07d1 );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 2754, pSock.language )); // That item is not poisoned.
			}
		}
	}
	else
	{
		if( myTarget.isChar && myTarget == pUser )
		{
			if( myTarget.id == 0x00b7 || myTarget.id == 0x00b8 )
			{
				pSock.SysMessage( GetDictionaryEntry( 1681, pSock.language )); // You wipe away all of your body paint.
				pUser.id = pUser.orgID;
				pUser.colour = pUser.orgSkin;
				return;
			}
			else if( myTarget.isDisguised )
			{
				pSock.SysMessage( GetDictionaryEntry( 9234, pSock.language )); // You have removed your disguise.
				pUser.KillJSTimer( 1, 5023 ); // Kill disguise timer in disguise script
				pUser.name = pUser.origName;
				pUser.origName = "";
				pUser.isDisguised = false;
				return;
			}
		}

		pSock.SysMessage( GetDictionaryEntry( 2755, pSock.language )); // That's not an item!
	}
}
