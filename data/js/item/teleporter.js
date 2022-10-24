function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem && pUser.CanSee( iUsed ))
	{
		if( pUser.InRange( iUsed, 3 ))
		{
			var otherGate = CalcItemFromSer( iUsed.morex );
			if( ValidateObject( otherGate ))
			{
				// Teleport player's pets
				var petList = pUser.GetPetList();
				for( var i = 0; i < petList.length; i++ )
				{
					var tempPet = petList[i];
					if( ValidateObject( tempPet ) && tempPet.InRange( pUser, 24 ))
					{
						tempPet.Teleport( otherGate );
						tempPet.Follow( pUser );
					}
				}

				// Teleport Player
				pUser.Teleport( otherGate );
			}
		}
	}
	return false;
}
