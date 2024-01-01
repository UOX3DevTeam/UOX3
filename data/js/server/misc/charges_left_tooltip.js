// Display remaining charges left in a tooltip, based on item's usesLeft property
function onTooltip(iUsed, pSocket)
{
	var tooltipText = "";
	var myCharges = iUsed.GetTag("charges").split("|");
	var charges = parseInt(myCharges[0]);
	var reCharges = parseInt(myCharges[1]);
	var maxCharges = parseInt(myCharges[2]);
	var Pet = CalcCharFromSer(parseInt(iUsed.GetTag("petSerial")));

	tooltipText = GetDictionaryEntry(9252, pSocket.language) + " " + charges + "/" + maxCharges;// Charges:

	if( ValidateObject( Pet ) && iUsed.sectionID == "crystalballofpetsummoning" )
	{
		tooltipText += "\n" + GetDictionaryEntry(19061, pSocket.language) + " " + Pet.name;
	}
	return tooltipText;
}

function _restorecontext_() {}