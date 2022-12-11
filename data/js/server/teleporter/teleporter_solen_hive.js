// Teleporter into the solen hive
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		if( iUsed.InRange( pUser, 2 ))
		{
			pUser.TextMessage( "*" + pUser.name + " dives into the hole and disappears!*" );
			Teleport( pUser );
		}
		else
		{
			pUser.SysMessage( "I can't reach that." );
		}
	}
	return false;
}

// Spawns Solen Ants when you walk on it.
function onCollide( pSock, pUser, iObject )
{
	if( !ValidateObject( pUser ) || !pUser.isChar || pUser.npc )
		return false;

	var spawnCount = iObject.GetTag( "spawnCount" );
	if( spawnCount < 5 )
	{
		var count = 1 + RandomNumber(1, 4);

		for(var i = 0; i < count; ++i)
		{
			if( spawnCount >= 5 )
              break;

			var spawnedAnt = SpawnAnt( iObject );
			if( ValidateObject( spawnedAnt ))
			{
				spawnCount++;
				iObject.SetTag( "spawnCount", spawnCount );
				spawnedAnt.SetTag( "parentSerial", ( iObject.serial ).toString() );
			}
		}
	}

	// need to chance to spawn a beetle.?

	return true;
}

function SpawnAnt( iObject )
{
	var random = RandomNumber( 1, 3 );
	var worldNum = iObject.worldnumber
	var spawnAnt = null;

	if( random < 2 )
	{
		if( worldNum == 0 )
		{
			var spawnAnt = SpawnNPC( "blacksolenworker", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3514 );
		}
		else
		{
			var spawnAnt = SpawnNPC( "redsolenworker", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3514 );
		}
		return spawnAnt;
	}
    else
	{
		if( worldNum == 0 )
		{
			var spawnAnt = SpawnNPC( "blacksolenwarrior", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3514 );
		}
		else
		{
			var spawnAnt = SpawnNPC( "redsolenwarrior", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3514 );
		}
		return spawnAnt;
    }

	return spawnAnt;
}

function Teleport( pUser )
{
	// Teleport player's pets to Solen Hive
	var petList = pUser.GetPetList();
	for( var i = 0; i < petList.length; i++ )
	{
		var tempPet = petList[i];
		if( ValidateObject( tempPet ) && tempPet.InRange( pUser, 24 ))
		{
			tempPet.Teleport( 5922, 2024, 0 );
			tempPet.Follow( pUser );
		}
	}

	// Teleport player to Solen Hive
	pUser.Teleport( 5922, 2024, 0 );
}