function onTooltip( equipment, pSocket )
{
	var healingBonus = parseInt( equipment.GetTag( "healingbonus" ));
	var tooltipText = healingBonus + "% Bandage Healing Bonus";
	return tooltipText;
}