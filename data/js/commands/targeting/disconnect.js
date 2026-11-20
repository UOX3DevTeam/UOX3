/// <reference path="../../definitions.d.ts" />
// @ts-check
// Commands to handle kicking, shard-local banning and shard-local unbanning

function CommandRegistration()
{
	RegisterCommand( "kick", 8, true );
	RegisterCommand( "disconnect", 8, true );
	RegisterCommand( "localban", 8, true );
	RegisterCommand( "localunban", 8, true );
	// Backwards-compatible aliases. Account changes made by these commands are
	// always local to the game shard on which the command is used.
	RegisterCommand( "ban", 8, true );
	RegisterCommand( "unban", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_KICK( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 196, socket.language ); // Select character to kick.
	socket.CustomTarget( 0, targMsg );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_DISCONNECT( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 196, socket.language ); // Select character to kick.
	socket.CustomTarget( 0, targMsg );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_BAN( socket, cmdString )
{
	command_LOCALBAN( socket, cmdString );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_LOCALBAN( socket, cmdString )
{
	var banDuration = 60 * 24;
	if( cmdString !== "" )
	{
		var trimmedDuration = cmdString.trim();
		banDuration = Number( trimmedDuration );

		if( trimmedDuration === "" || !/^\d+$/.test( trimmedDuration ) ||
			!isFinite( banDuration ) || banDuration > 5256000 )
		{
			// Invalid command syntax! Correct syntax: localban / localban <durationInMinutes>
			// If no duration is specified, the ban defaults to 24 hours. A duration of 0 is permanent.
			// Timed bans are capped at 10 years to keep the stored absolute expiry within range.
			socket.SysMessage( GetDictionaryEntry( 2018, socket.language ));
			return;
		}
	}

	socket.tempint = banDuration;
	var targMsg = GetDictionaryEntry( 2015, socket.language ); // Select character to ban for %d minutes.
	targMsg = targMsg.replace( '%d', banDuration );
	socket.CustomTarget( 1, targMsg, 1 );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_UNBAN( socket, cmdString )
{
	command_LOCALUNBAN( socket, cmdString );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_LOCALUNBAN( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 2019, socket.language); // Select character to unban.
	socket.CustomTarget( 2, targMsg );
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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
			Console.Log( "LOCAL BAN: Account #" + ourAccount.id + " was banned for " +
				timebanDuration + " minute(s) on " + GetServerSetting( "SERVERNAME" ) + ".", "command.log" );

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

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
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
			Console.Log( "LOCAL UNBAN: Account #" + ourAccount.id + " was unbanned on " +
				GetServerSetting( "SERVERNAME" ) + ".", "command.log" );

			socket.SysMessage( GetDictionaryEntry( 2020, socket.language ), ourAccount.id ); // Account with ID #%d has been unbanned.
		}
	}
}
