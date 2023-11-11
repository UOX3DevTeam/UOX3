function onTooltip( equipment, pSocket )
{
	var healingBonus = parseInt( equipment.GetTag( "healingBonus" ));
	var tooltipText = GetDictionaryEntry(18800, pSocket.language ); // %i% Bandage Healing Bonus
	return tooltipText.replace( /%i/gi, healingBonus );
}