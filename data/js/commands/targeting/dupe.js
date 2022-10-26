// This command allows GMs to duplicate the selected object and create one or more identical ones

function CommandRegistration()
{
	RegisterCommand( "dupe", 2, true );
}

function command_DUPE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 38, socket.language );
	if( cmdString )
	{
		socket.tempint = parseInt( cmdString );
	}
	else
	{
		socket.tempint = 1;
	}
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	if( !socket.GetWord( 1 ) && ValidateObject( ourObj ))
	{
		if( ourObj.isItem || ourObj.isChar )
		{
			var pUser = socket.currentChar;
			var numToDupe = socket.tempint;
			for( var i = 0; i < numToDupe; ++i )
			{
				if( ourObj.isSpawner )
				{
					var newSpawner = CreateBlankItem( socket, pUser, 1, ourObj.name, ourObj.id, ourObj.colour, "SPAWNER", true );
					if( ValidateObject( newSpawner ))
					{
						newSpawner.spawnsection = ourObj.spawnsection;
						if( ourObj.sectionalist )
						{
							newSpawner.sectionalist = true;
						}
						newSpawner.mininterval = ourObj.mininterval;
						newSpawner.maxinterval = ourObj.maxinterval;
						newSpawner.amount = ourObj.amount;
						newSpawner.visible = ourObj.visible;
						newSpawner.weight = ourObj.weight;
						newSpawner.movable = ourObj.movable;
						newSpawner.more = ourObj.more;
						newSpawner.morex = ourObj.morex;
						newSpawner.morey = ourObj.morey;
						newSpawner.morez = ourObj.morez;
						newSpawner.decayable = ourObj.decayable;
						newSpawner.type = ourObj.type;
						newSpawner.x = ourObj.x + 1;
						newSpawner.y = ourObj.y + 1;
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 8077, socket.language )); // Failed to dupe item.
						return;
					}
				}
				else
				{
					ourObj.Dupe( socket );
				}
			}
		}
	}
	var tempMsg = GetDictionaryEntry( 8078, socket.language ); // %i duped items have been placed in your backpack.
	socket.SysMessage( tempMsg.replace( /%i/gi, numToDupe.toString() ));
	socket.tempint = 0;
}
