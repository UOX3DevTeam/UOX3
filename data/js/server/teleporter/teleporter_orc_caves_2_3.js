// Teleporter between Orc Caves levels 2 and 3
function onCollide( pSock, pChar, iObject )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc )
		return false;

	// Freeze player to avoid them dropping to Z 0 if they take another step too quickly after teleporting
	pChar.frozen = true;
	pChar.Refresh();

	if( pChar.x == 5363 || pChar.x == 5364 )
	{
		pChar.StartTimer( 100, 0, true );
	}
	else // pChar.x == 5272 || pChar.x == 5273
	{
		pChar.StartTimer( 100, 1, true );
	}

	return true;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 0 )
	{
		// Teleport player's pets to Orc Caves Level 3
		var petList = timerObj.GetPetList();
		for( var i = 0; i < petList.length; i++ )
		{
			var tempPet = petList[i];
			if( ValidateObject( tempPet ) && tempPet.InRange( timerObj, 24 ))
			{
				tempPet.Teleport(( timerObj.x == 5363 ? 5272 : 5273 ), 2041, 3 );
				tempPet.Follow( timerObj );
			}
		}

		// Teleport player to Orc Caves Level 3
		timerObj.Teleport(( timerObj.x == 5363 ? 5272 : 5273 ), 2041, 3 );
		timerObj.StartTimer( 50, 2, true );
	}
	else if( timerID == 1 )
	{
		// Teleport player's pets to Orc Caves Level 2
		var petList = timerObj.GetPetList();
		for( var i = 0; i < petList.length; i++ )
		{
			var tempPet = petList[i];
			if( ValidateObject( tempPet ) && tempPet.InRange( timerObj, 24 ))
			{
				tempPet.Teleport(( timerObj.x == 5272 ? 5363 : 5364 ), 1290, 3 );
				tempPet.Follow( timerObj );
			}
		}

		// Teleport player to Orc Caves Level 2
		timerObj.Teleport(( timerObj.x == 5272 ? 5363 : 5364 ), 1290, 3 );
		timerObj.StartTimer( 50, 2, true );
	}
	else if( timerID == 2 )
	{
		// Unfreeze player
		timerObj.frozen = false;
		timerObj.Refresh();
	}
}