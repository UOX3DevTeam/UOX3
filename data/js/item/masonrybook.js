/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( socket && iUsed && iUsed.isItem )
	{
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		}
		else if( pUser.skills[0] < 1000 )
		{
			socket.SysMessage( GetDictionaryEntry( 6298, socket.language )); // Only a Grandmaster Carpenter can learn from this book.
		}
		else if( pUser.GetTag( "StoneCrafting" ) == 1 )
		{
			socket.SysMessage( GetDictionaryEntry( 6302, socket.language )); // You have already learned this information.
		}
		else if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); //That is locked down and you cannot use it
		}
		else 
		{
			pUser.SetTag( "StoneCrafting", 1 );
			socket.SysMessage( GetDictionaryEntry( 6299, socket.language )); // You have learned to make items from stone. You will need miners to gather stones for you to make these items.
			iUsed.Delete();
		}
	}
	return false;
}
