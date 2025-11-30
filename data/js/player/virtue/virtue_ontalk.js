function onTalk( myChar, mySpeech )
{
	var lower = mySpeech.toLowerCase();

	if( lower.indexOf( "lum lum lum" ) >= 0 )
	{
		TriggerEvent( 8005, "Humility_ToggleHunt", myChar );
		return true;
	}
	return true;
}
