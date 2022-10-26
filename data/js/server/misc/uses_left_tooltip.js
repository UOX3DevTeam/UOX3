// Display remaining uses left in a tooltip, based on item's usesLeft property
function onTooltip( myObj )
{
	var tooltipText = "";
	var usesLeft = myObj.usesLeft;
	if( usesLeft > 0 )
	{
		tooltipText = GetDictionaryEntry( 9403 ); // uses remaining: %i
		tooltipText = ( tooltipText.replace( /%i/gi, ( usesLeft ).toString() ));
		myObj.SetTempTag( "clilocTooltip", 1042971 ); // ~1_NOTHING~
	}
	return tooltipText;
}