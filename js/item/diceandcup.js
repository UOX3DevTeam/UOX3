function onUseChecked( pUser, iUsed )
{ 
	var Dice1 = RandomNumber( 1, 6 );
	var Dice2 = RandomNumber( 1, 6 );
	
	var pName = pUser.name;
	
	if( iUsed.container != null )
		pUser.EmoteMessage( pName+" rolls the dice and gets a "+iDice1+" and a "+iDice2+"." );
	else
		iUsed.TextMessage( pName+" rolls the dice and gets a "+iDice1+" and a "+iDice2+"." );
	return false;
}
