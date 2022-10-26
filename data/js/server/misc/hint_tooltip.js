// Helpful clilocs
//  1042971 - ~1_NOTHING~	// Any custom string
// 	1062613 - "~1_NAME~" 	// String in quotes, first letter of first word NOT capitalized automatically
// 	1158847 - Embroidered: ~1_MESSAGE~
// 	1072305 - Engraved: ~1_INSCRIPTION~
// 	1157315 - <BASEFONT COLOR=#668cff>Branded: ~1_VAL~<BASEFONT COLOR=#FFFFFF>

// Hint Script
// Last Updated: 10/11/2021
//
// Displays a hint in a tooltip, or as a private text message when the item is double-clicked
// Message is either "hard-scripted" based on an ID stored in item's MOREY property, or
// uses the item's DESC property if no valid MOREY value is provided

function onTooltip( myObj )
{
	var tooltipText = "";
	var hintText = "";
	var tooltipClilocID = 1062613; // "~1_NAME~"
	hintText = GetHintMessage( myObj, true );

	if( hintText && hintText.length > 0 )
	{
		tooltipText = hintText;

		// Set a temp tag with the desired cliloc ID to use, which will be read in code!
		myObj.SetTempTag( "tooltipCliloc", tooltipClilocID );
	}
	return tooltipText;
}

function onUseChecked( pUser, iUsed )
{
	iUsed.TextMessage( GetHintMessage( iUsed, false ), false, 0x3b2, 0, pUser.serial );
	return false;
}

function GetHintMessage( iUsed, useHtml )
{
	var hintText = "";
	switch( iUsed.morey )
	{
		case 500716: // hint text from Covetous
			if( useHtml )
			{
				hintText = "<BASEFONT COLOR=#668cff>";
			}
			hintText += "A small plaque on the bottom of the frame reads: 3-4-1-5.";
			if( useHtml )
			{
				hintText += "<BASEFONT COLOR=#FFFFFF>";
			}
			break;
		case 500718: // hint text from Covetous
			hintText = "You seem not to be able to free the note, but upon it you see the writing: 3-4-1-5.";
			break;
		default:
			hintText = iUsed.desc;
			break;
	}
	return hintText;
}