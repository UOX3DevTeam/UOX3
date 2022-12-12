// Teleporter into the solen hive
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( socket && iUsed && iUsed.isItem )
	{
		if( iUsed.InRange( pUser, 3 ))
		{
			if( iUsed.sectionID == "solenentrancehole" || iUsed.sectionID == "solenentrancehole2" )
			{
				var x = 0;
				var y = 0;

				if( iUsed.x == 2608 && iUsed.y == 763 ) // Entrance Next to Minoc
				{
					x = 5916;
					y = 1793;
				}
				else if( iUsed.x == 2609 && iUsed.y == 763 ) // Entrance Next to Minoc
				{
					x = 5921;
					y = 1793;
				}
				else if( iUsed.x == 2608 && iUsed.y == 764 ) // Entrance Next to Minoc
				{
					x = 5917;
					y = 1797;
				}
				else if( iUsed.x == 2609 && iUsed.y == 764 ) // Entrance Next to Minoc
				{
					x = 5921;
					y = 1797;
				}
				else if( iUsed.x == 1690 && iUsed.y == 2789 ) // Entrance Next to Trinisc
				{
					x = 5653;
					y = 2018;
				}
				else if( iUsed.x == 1691 && iUsed.y == 2789 ) // Entrance Next to Trinisc
				{
					x = 5658;
					y = 2017;
				}
				else if( iUsed.x == 1690 && iUsed.y == 2790 ) // Entrance Next to Trinisc
				{
					x = 5655;
					y = 2021;
				}
				else if( iUsed.x == 1691 && iUsed.y == 2790 ) // Entrance Next to Trinisc
				{
					x = 5658;
					y = 2021;
				}
				else if( iUsed.x == 1724 && iUsed.y == 814 ) // Entrance Next to Compassion Shrine
				{
					x = 5919;
					y = 2019;
				}
				else if( iUsed.x == 1725 && iUsed.y == 814 ) // Entrance Next to Compassion Shrine
				{
					x = 5922;
					y = 2020;
				}
				else if( iUsed.x == 1724 && iUsed.y == 815 ) // Entrance Next to Compassion Shrine
				{
					x = 5917;
					y = 2023;
				}
				else if( iUsed.x == 1725 && iUsed.y == 815 ) // Entrance Next to Compassion Shrine
				{
					x = 5922;
					y = 2024;
				}
				else if( iUsed.x == 730 && iUsed.y == 1451 ) // Entrance Next to Yew Orc Fort
				{
					x = 5654;
					y = 1789;
				}
				else if( iUsed.x == 731 && iUsed.y == 1451 ) // Entrance Next to Yew Orc Fort
				{
					x = 5658;
					y = 1791;
				}
				else if( iUsed.x == 730 && iUsed.y == 1452 ) // Entrance Next to Yew Orc Fort
				{
					x = 5652;
					y = 1793;
				}
				else if( iUsed.x == 731 && iUsed.y == 1452 ) // Entrance Next to Yew Orc Fort
				{
					x = 5656;
					y = 1795;
				}
				TeleportMainEntrance( pUser, x, y );
			}
			else if( iUsed.sectionID == "shexitteleporter" )
			{
				var x = 0;
				var y = 0;

				if( iUsed.x == 5918 && iUsed.y == 1794 || iUsed.x == 5919 && iUsed.y == 1794  ||  iUsed.x == 5918 && iUsed.y == 1795  || iUsed.x == 5919 && iUsed.y == 1795 ) // Exit Next to Minoc
				{
					x = 2607;
					y = 763;
				}
				else if( iUsed.x == 5655 && iUsed.y == 2018 || iUsed.x == 5656 && iUsed.y == 2018  ||  iUsed.x == 5655 && iUsed.y == 2019  || iUsed.x == 5656 && iUsed.y == 2019 ) // Exit Next to Trinisc
				{
					x = 1689;
					y = 2789;
				}
				else if( iUsed.x == 5919 && iUsed.y == 2021 || iUsed.x == 5920 && iUsed.y == 2021  ||  iUsed.x == 5919 && iUsed.y == 2022  || iUsed.x == 5920 && iUsed.y == 2022 ) // Exit Next to Compassion Shrine
				{
					x = 1723;
					y = 814;
				}
				else if( iUsed.x == 5654 && iUsed.y == 1791 || iUsed.x == 5655 && iUsed.y == 1791  ||  iUsed.x == 5654 && iUsed.y == 1792  || iUsed.x == 5655 && iUsed.y == 1792 ) // Exit Next to Yew Orc Fort
				{
					x = 729;
					y = 1451;
				}
				TeleportMainEntrance( pUser, x, y );
			}
			else
			{
				var examine = iUsed.GetTag("Examined");
				var rand = RandomNumber( 1, 100 );
				if( rand < 50 && examine == 0 )
				{
					pUser.TextMessage( "* You examine the solen hole, but discover it does not lead anywhere *" );
					iUsed.SetTag( "Examined", 1 );
					return;
				}
				else
				{
					pUser.TextMessage( "* " + pUser.name + " dives into the hole and disappears! *" );
					Teleport( pUser ); // maybe random locations in the hive because they appear random.
				}
			}
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
	if( !ValidateObject( pUser ) || !pUser.isChar || pUser.npc || iObject.sectionID == "solenentrancehole" || iObject.sectionID == "solenentrancehole2" || iObject.GetTag("Examined") == 1 || iObject.sectionID == "shexitteleporter" )
		return false;

	if( iObject.GetTag( "Spawned" ) == true )
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
				iObject.SetTag( "Spawned", true );
				iObject.StartTimer( 300000, 1, true ); // 5 min cool down timer keep from spamming it.
				spawnedAnt.SetTag( "parentSerial", ( iObject.serial ).toString() );
			}
		}
	}

	// need to chance to spawn a beetle.?
	return true;
}

function onTimer( iObject, timerID )
{
	switch( timerID )
	{
		case 1:
			iObject.SetTag( "Spawned", false );
			break;
	}
}

function SpawnAnt( iObject )
{
	var random = RandomNumber( 1, 3 );
	var worldNum = iObject.worldnumber;
	var spawnAnt = null;

	if( random < 2 )
	{
		if( worldNum == 0 )
		{
			var spawnAnt = SpawnNPC( "blacksolenworker", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3214 );
		}
		else
		{
			var spawnAnt = SpawnNPC( "redsolenworker", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3214 );
		}
		return spawnAnt;
	}
    else
	{
		if( worldNum == 0 )
		{
			var spawnAnt = SpawnNPC( "blacksolenwarrior", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3214 );
		}
		else
		{
			var spawnAnt = SpawnNPC( "redsolenwarrior", iObject.x, iObject.y, iObject.z, iObject.worldnumber, iObject.instanceID );
			spawnAnt.AddScriptTrigger( 3214 );
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

function TeleportMainEntrance( pUser, x, y )
{
	// Teleport player's pets to Solen Hive
	var petList = pUser.GetPetList();
	for( var i = 0; i < petList.length; i++ )
	{
		var tempPet = petList[i];
		if( ValidateObject( tempPet ) && tempPet.InRange( pUser, 24 ))
		{
			tempPet.Teleport( x, y, 0 );
			tempPet.Follow( pUser );
		}
	}

	// Teleport player to Solen Hive
	pUser.Teleport( x, y, 0 );
}