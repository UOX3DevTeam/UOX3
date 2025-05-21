function CommandRegistration()
{
	RegisterCommand( "instabond", 3, true );
}

function command_INSTABOND( socket, cmdString )
{
	socket.CustomTarget( 0, GetDictionaryEntry( 19324, socket.language ));//Select a pet to instantly bond.
}

function onCallback0( socket, ourObj )
{
	const petBondingEnabled = GetServerSetting( "PetBondingEnabled" );
	if( petBondingEnabled > 0 )
	{
		if( !ValidateObject(ourObj) || !ourObj.npc )
		{
			socket.SysMessage( GetDictionaryEntry( 19325, socket.language ));//Invalid target.
			return;
		}
	
		if( !ourObj.tamed )
		{
			socket.SysMessage( GetDictionaryEntry( 19326, socket.language ));// That creature must be tamed before it can be bonded.
			return;
		}

		if( ourObj.GetTag( "isBondedPet" ) == true)
		{
			socket.SysMessage( GetDictionaryEntry( 19327, socket.language ));// That pet is already bonded.
			return;
		}

		ourObj.KillJSTimer( 42, 3107 );
		ourObj.SetTag( "bondingStarted", null );
		ourObj.SetTag( "bondingPlayer", null );
		ourObj.SetTag( "isBondedPet", true );

		socket.SysMessage( GetDictionaryEntry( 19328, socket.language ));//Pet has been instantly bonded!
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 19329, socket.language ));//Pet bonding is disabled on this server.
	}
}
