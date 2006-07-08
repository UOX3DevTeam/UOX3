// Cow-Tipping!

function onCharDoubleClick( pUser, targChar )
{
	var rndValue = RandomNumber( 0, 100 );
	if( rndValue < 5 )
	{
		targChar.DoAction( 8 );
		targChar.SoundEffect( 0x78, true );
	}		
}