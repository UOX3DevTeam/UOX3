function onTooltip( equipment, pSocket )
{
	var healingBonus = parseInt( equipment.GetTag( "healingBonus" ));
	var tooltipText = healingBonus + "% Bandage Healing Bonus";
	return tooltipText;
}