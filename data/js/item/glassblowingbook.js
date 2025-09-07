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
			socket.SysMessage(GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		}
		else if( pUser.skills[0] < 1000 )
		{
			socket.SysMessage( GetDictionaryEntry( 6301, socket.Language ) ); // Only a Grandmaster Alchemist can learn from this book.
		}
		else if( pUser.GetTag( "GlassBlowing" ) == 1 )
		{
			socket.SysMessage( GetDictionaryEntry( 6302, socket.Language ) ); // You have already learned this information.
		}
		else if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.Language )); //That is locked down and you cannot use it
		}
		else 
		{
			pUser.SetTag( "GlassBlowing", 1 );
			socket.SysMessage( GetDictionaryEntry( 6303, socket.Language )); // You have learned to mine for stones.  Target mountains when mining to find stones.
			iUsed.Delete();
		}
	}
	return false;
}
