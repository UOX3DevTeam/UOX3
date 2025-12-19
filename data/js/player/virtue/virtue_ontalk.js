/// <reference path="../../definitions.d.ts" />
// @ts-check

const humilityVirtueEnabled = GetServerSetting( "HumilityVirtueEnabled" );

/** @type { ( myChar: Character, mySpeech: string ) => boolean } */
function onTalk( myChar, mySpeech )
{
	if( humilityVirtueEnabled )
	{
		var lower = mySpeech.toLowerCase();

		if( lower.indexOf( "lum lum lum" ) >= 0 )
		{
			TriggerEvent( 8005, "Humility_ToggleHunt", myChar );
			return true;
		}
	}
	return true;
}
