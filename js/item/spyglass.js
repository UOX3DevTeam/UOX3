var moonPhase = new Array( "a full moon", "waning gibbous", "in it's last quarter", "waning crescent", "a new moon", "waxing crescent", "in it's first quarter", "waxing gibbous" );

function onUseChecked( pUser, iUsed )
{
	pUser.TextMessage( "You peer into the heavens, seeking the moons...", false, 0x03b2 );
	pUser.TextMessage( "Trammel : " + moonPhase[Moon( 1 )], false, 0x03b2 );
	pUser.TextMessage( "Felucca : " + moonPhase[Moon( 0 )], false, 0x03b2 );
	return false;
}