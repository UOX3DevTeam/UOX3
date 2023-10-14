// All players can access this command, which lets them know the individual playtime of their
// current character and the total playtime of their account

function CommandRegistration()
{
	RegisterCommand( "playtime", 0, true );
}

function command_PLAYTIME( socket, cmdString )
{
	var pChar = socket.currentChar;
	if( !ValidateObject( pChar ))
		return;

	// Get playtime on character, and account
	var playTime = pChar.playTime;
	var totalPlayTime = pChar.account.totalPlayTime;

	// Add playtime since last login, which won't get added to char/account until next logout/disconnect
	var minSinceLogin = Math.round( GetCurrentClock() / 1000 / 60 ) - pChar.GetTempTag( "loginTime" );
	playTime += minSinceLogin;
	totalPlayTime += minSinceLogin;

	// Individual character playtime in days, hours and minutes
	var days = Math.floor( playTime / 1440 );
	var hours = Math.floor(( playTime % 1440 ) / 60 );
	var minutes = playTime % 60;
	var playTimeFinal = days + " days " + hours + " hours " + minutes + " minutes";

	// Total account playtime in days, hours and minutes
	days = Math.floor( totalPlayTime / 1440 );
	hours = Math.floor(( totalPlayTime % 1440 ) / 60 );
	minutes = totalPlayTime % 60;
	var totalPlayTimeFinal = days + " days " + hours + " hours " + minutes + " minutes";

	socket.SysMessage( GetDictionaryEntry( 2790, socket.language ), playTimeFinal ); // Time played (Character): %s
	socket.SysMessage( GetDictionaryEntry( 2791, socket.language ), totalPlayTimeFinal ); // Total time played (Account): %s
}
