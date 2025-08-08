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
				var followerList = pUser.GetFollowerList();
				for( var i = 0; i < followerList.length; i++ )
				{
					var tempFollower = followerList[i];
					// Only teleport pets if set to follow and within range
					if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( pChar, 24 ))
					{
						tempFollower.Teleport( otherGate );
						tempFollower.Follow( pUser );
					}
				}

				// Teleport Player
				pUser.Teleport( otherGate );
			}
		}
	}
	return false;
}
