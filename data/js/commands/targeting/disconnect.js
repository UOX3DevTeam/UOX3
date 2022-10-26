// Commands to handle kicking, banning and unbanning players from the server

function CommandRegistration()
{
	RegisterCommand( "kick", 2, true );
	RegisterCommand( "disconnect", 2, true );
	RegisterCommand( "ban", 2 , true );
	RegisterCommand( "unban", 2 , true );
}

function command_KICK( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 196, socket.language ); // Select character to kick.
	socket.CustomTarget( 0, targMsg );
}

function command_DISCONNECT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 196, socket.language ); // Select character to kick.
	socket.CustomTarget( 0, targMsg );
}

function command_BAN( socket, cmdString )
{
	var banDuration = 60 * 24;
	if( cmdString !== "" )
	{
		banDuration = parseInt( cmdString );

		if( isNaN( banDuration ))
		{
			// Invalid command syntax! Correct syntax: ban / ban <durationInMinutes> (if no duration is specified, ban defaults to 24 hours)
			socket.SysMessage( GetDictionaryEntry( 2018, socket.language ));
			return;
		}
	}

	socket.tempint = banDuration;
	var targMsg = GetDictionaryEntry( 2015, socket.language ); // Select character to ban for %d minutes.
	targMsg = targMsg.replace( '%d', banDuration );
	socket.CustomTarget( 1, targMsg, 1 );
}

function command_UNBAN( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 2019, socket.language); // Select character to unban.
	socket.CustomTarget( 2, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar && ourObj.online )
	{
		var targSock = ourObj.socket;
		if( targSock && targSock != socket )
		{
			socket.SysMessage( GetDictionaryEntry( 1029, socket.language ) ); // Kicking player!
			targSock.SysMessage( GetDictionaryEntry( 1030, targSock.language ) ); // You have been kicked!
			targSock.Disconnect();
		}
	}
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		if( !ourObj.npc )
		{
			var ourAccount = ourObj.account;
			if( ourAccount == null )
				return;

			// Mark account as banned
			ourAccount.isBanned = 1;

			// Ban for 24 hours by default, if nothing else is specified
			var timebanDuration = 60 * 24;
			if( socket.tempint != null )
			{
				timebanDuration = socket.tempint;
			}
			ourAccount.timeban = timebanDuration;

			// Disconnect player, if online
			if( ourAccount.isOnline && ValidateObject( ourAccount.currentChar ) && ourAccount.currentChar.socket != null )
			{
				var banMsg = GetDictionaryEntry( 2017, ourAccount.currentChar.socket.language ); // You have been banned for %d minutes!
				ourAccount.currentChar.SysMessage( banMsg, timebanDuration );
				ourAccount.currentChar.socket.Disconnect();
			}

			socket.SysMessage( GetDictionaryEntry( 2016, socket.language ), ourAccount.id, timebanDuration ); // Account with ID #%d has been banned for %d minutes.
		}
	}
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		if( !ourObj.npc )
		{
			var ourAccount = ourObj.account;
			if( ourAccount == null )
				return;

			ourAccount.isBanned = 0;
			ourAccount.timeban = 0;

			socket.SysMessage( GetDictionaryEntry( 2020, socket.language ), ourAccount.id ); // Account with ID #%d has been unbanned.
		}
	}
}