function onUseChecked( pUser, iUsed )
{
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	var iDice1 = RandomNumber( 1, 6 );
	var iDice2 = RandomNumber( 1, 6 );
	
	var pName = pUser.name;
	
	var diceResults = GetDictionaryEntry( 2722 ) // %s rolls the dice and gets a %i and %d.
	diceResults = diceResults.replace( /%s/gi, pName );
	diceResults = diceResults.replace( /%i/gi, iDice1 );
	diceResults = diceResults.replace( /%d/gi, iDice2 );

	if( iUsed.container != null )
	{
		pUser.EmoteMessage( diceResults );
	}
	else
	{
		iUsed.TextMessage( diceResults );
	}
	return false;
}
