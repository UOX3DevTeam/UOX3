function CommandRegistration()
{
	RegisterCommand( "showtime", 1, true );
}

function command_SHOWTIME( socket, cmdString )
{
	var hour 	= GetHour();
	var minute	= GetMinute();
	var ampm	= " AM";
	if( hour >= 12 )
	{
		ampm = " PM";
		hour = hour - 12;
	}
	var hDisp = hour.toString();
	var mDisp = minute.toString();
	if( minute < 10 )
		mDisp = "0" + mDisp;

	var toDisplay	= GetDictionaryEntry( 8002, socket.language ) + " " + hDisp + ":" + mDisp + ampm; // Time: h:m am/pm
	socket.SysMessage( toDisplay );
}