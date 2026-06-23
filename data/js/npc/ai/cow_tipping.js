/// <reference path="../../definitions.d.ts" />
// @ts-check
// Cow-Tipping!

/** @type { ( currChar: Character, targChar: Character, nonMouseClickEvent: boolean ) => boolean } */
function onCharDoubleClick( pUser, targChar, nonMouseClickEvent )
{
	var rndValue = RandomNumber( 0, 100 );
	if( rndValue < 5 )
	{
		targChar.DoAction( 8 );
		targChar.SoundEffect( 0x78, true );
	}		
}
