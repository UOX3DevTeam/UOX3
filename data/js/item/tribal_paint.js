// Script for Tribal Paint/Savage Kin Paint consumable
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		// Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language ) ); //That is locked down and you cannot use it
			return false;
		}

		if( pUser.isIncognito )
		{
			socket.SysMessage( GetDictionaryEntry( 1677, socket.language )); // You cannot disguise yourself while incognito.
		}
		else if( pUser.isPolymorphed )
		{
			socket.SysMessage( GetDictionaryEntry( 1678, socket.language )); // You cannot disguise yourself while polymorphed.
		}
		else
		{
			var helmLayerItem = pUser.FindItemLayer( 0x06 );
			if( ValidateObject( helmLayerItem ) && helmLayerItem.sectionID == "orcishkinmask" )
			{
				socket.SysMessage( GetDictionaryEntry( 1679, socket.language )); // You are already disguised.
			}
			else
			{
				pUser.id = ( pUser.gender ? 0x00b8 : 0x00b7 );
				pUser.colour = 0;

				// Attach script to player to revert effects of tribal paint in 7 days
				pUser.AddScriptTrigger( 3 );
				TriggerEvent( 3, "StartLongTermTimer", pUser, 5000, 1000 * 60 * 60 * 24 * 7 ); // 7 days

				// You now bear the markings of the savage tribe.  Your body paint will last about a week or you can remove it with an oil cloth
				socket.SysMessage( GetDictionaryEntry( 1680, socket.language ));

				// Delete the tribal paint
				iUsed.Delete();
			}
		}
	}
	return false;
}