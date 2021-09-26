// Display remaining uses left in a tooltip, based on item's UsesLeft tag
function onTooltip( myObj )
{
	var tooltipText = "";
	var usesLeft = myObj.GetTag( "UsesLeft" );
	if( usesLeft > 0 )
	{
		tooltipText = GetDictionaryEntry( 9403 ); // uses remaining: %i
		tooltipText = ( tooltipText.replace(/%i/gi, (usesLeft).toString()) );
	}
	return tooltipText;
}