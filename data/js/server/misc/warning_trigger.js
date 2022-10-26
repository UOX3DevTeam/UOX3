// Warning Script
// Last updated: 10/11/2021
//
// Display a warning as player approaches/collides with an object
// Range at which onMoveDetect triggers is determined by part 1 of object's MORE property
// Warning on movement will only display once per user per server session (until temp tag expires, basically)

function onCollide( pSock, pChar, iObject )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc || pChar.dead || pChar.isGM || pChar.isCounselor || pChar.visible == 3 )
		return false;

	DisplayWarningText( iObject, pChar, false );
	return false;
}

function onMoveDetect( iObj, pChar, rangeToChar, oldCharX, oldCharY )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc || pChar.dead || pChar.isGM || pChar.isCounselor || pChar.visible == 3 )
		return true;

	var maxRange = iObj.GetMoreVar( "more", 1 );
	if( rangeToChar == maxRange )
	{
		DisplayWarningText( iObj, pChar, true );
	}
	return true;
}

function DisplayWarningText( iObject, trgChar, checkHistory )
{
	if( checkHistory && iObject.GetTempTag( "char" + ( trgChar.serial ).toString() ))
		return;

	var warningText = "";
	switch( iObject.morez )
	{
		case 500761: // warning text on brazier in Deceit lvl 1
			warningText = "Heed this warning well, and use this brazier at your own peril.";
			break;
		case 500715: // warning text on painting in Covetous
			warningText = "You notice something odd about the frame of the painting.";
			break;
		case 500717: // warning text on throne in Covetous
			warningText = "A small piece of paper seems to be wedged beneath the throne.";
			break;
		case 1010056: // warning text on statue in Covetous
			warningText = "Beware Ye Who Enter These Halls!";
			break;
		case 1010057: // warning text on statue in Covetous
			warningText = "Go no farther, lest ye face thy death!";
			break;
		default:
			warningText = iObject.desc;
			break;
	}

	if( warningText != "" )
	{
		// Display a personalized warning to player, unseen to other eyes!
		iObject.TextMessage( warningText, false, 0x3b2, 0, trgChar.serial );

		if( checkHistory )
		{
			iObject.SetTempTag( "char" + ( trgChar.serial ).toString(), true );
		}
	}
}
