function onUseChecked( pUser, iUsed )
{ 
	var iDice1 = RandomNumber( 1, 6 );
	var iDice2 = RandomNumber( 1, 6 );
	
	var pName = pUser.name;
	
	if( iUsed.container != null )
		pUser.EmoteMessage( pName+" rolls the dice and gets a "+iDice1+" and a "+iDice2+"." );
	else
		iUsed.TextMessage( pName+" rolls the dice and gets a "+iDice1+" and a "+iDice2+"." );
	return false;
}
