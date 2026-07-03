// Custom house commit confirmation gump.

function CustomHouseFormatGold( amount )
{
	return amount.toString().replace( /\B(?=(\d{3})+(?!\d))/g, "," );
}

function CustomHouseCostLine( commitCost, isGM )
{
	if( isGM )
	{
		if( commitCost > 0 )
			return "Cost: " + CustomHouseFormatGold( commitCost ) + " gold (GM free)<BR>";

		if( commitCost < 0 )
			return "Refund: " + CustomHouseFormatGold( -commitCost ) + " gold (GM free)<BR>";

		return "Cost: 0 gold (GM free)<BR>";
	}

	if( commitCost > 0 )
		return "Cost: " + CustomHouseFormatGold( commitCost ) + " gold<BR>";

	if( commitCost < 0 )
		return "Refund: " + CustomHouseFormatGold( -commitCost ) + " gold<BR>";

	return "Cost: 0 gold<BR>";
}

function onConfirm( pSocket, pUser, iMulti, oldPrice, newPrice, commitCost, bankBalance, isGM )
{
	if( pSocket == null || !ValidateObject( pUser ) || !ValidateObject( iMulti ))
		return false;

	var canCommit = ( isGM || commitCost <= 0 || bankBalance >= commitCost );
	var confirmGump = new Gump;
	var detailText = "Current house value: " + CustomHouseFormatGold( oldPrice ) + " gold<BR>";
	detailText += "New house value: " + CustomHouseFormatGold( newPrice ) + " gold<BR>";
	detailText += CustomHouseCostLine( commitCost, isGM );
	detailText += "Bank balance: " + CustomHouseFormatGold( bankBalance ) + " gold<BR>";

	if( !canCommit )
		detailText += "<BASEFONT COLOR=#FF0000>You do not have enough gold in your bank box.</BASEFONT>";
	else
		detailText += "Do you wish to commit this house design?";

	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 0, 0, 360, 190, 2620 );
	confirmGump.AddBackground( 10, 10, 340, 170, 3000 );
	confirmGump.AddHTMLGump( 20, 20, 320, 25, 0, 0, "<CENTER><BIG>Confirm House Design</BIG></CENTER>" );
	confirmGump.AddHTMLGump( 30, 55, 300, 85, 0, 0, detailText );

	if( canCommit )
	{
		confirmGump.AddButton( 90, 145, 4023, 4024, 1, 0, 1 );
		confirmGump.AddText( 125, 145, 0, "COMMIT" );
	}

	confirmGump.AddButton( 215, 145, 4020, 4021, 1, 0, 0 );
	confirmGump.AddText( 250, 145, 0, "CANCEL" );
	confirmGump.Send( pSocket );
	confirmGump.Free();
	return true;
}

function onGumpPress( pSocket, pButton, gumpData )
{
	if( pButton != 1 || pSocket == null )
		return;

	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	HouseCommitCustomize( pUser );
}
