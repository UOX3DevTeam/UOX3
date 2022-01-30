// Equip-effects for masks of orcish kin
function onEquipAttempt( pEquipper, iEquipped )
{
	var pSocket = pEquipper.socket;
	if( pEquipper.id == 0x00b7 || pEquipper.id == 0x00b8 )
	{
		if( pSocket != null )
		{
			pSocket.SysMessage( GetDictionaryEntry( 9127, pSocket.language )); // You can't do that while wearing savage kin paint
		}
		return false;
	}

	var headWear = pEquipper.FindItemLayer( 0x06 );
	if( ValidateObject( headWear ))
	{
		if( pSocket != null )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1744, pSocket.language )); // You can't equip two items in the same slot.
		}
		return false;
	}

	return true;
}

function onEquip( pEquipper, iEquipped )
{
	// Reduce equipper's karma
	pEquipper.karma -= 20;
	if( pEquipper.socket != null )
	{
		pEquipper.socket.SysMessage( GetDictionaryEntry( 1370, pEquipper.socket.language )); // You have lost some karma.
	}
}
