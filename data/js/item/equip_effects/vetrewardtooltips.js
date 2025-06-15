const coreShardEra = EraStringToNum(GetServerSetting("CoreShardEra"));
const VetRewardEnabled = false;

function onTooltip( vetRewardItem )
{
	var tooltipText = "";
	var vetReward = vetRewardItem.GetTag( "vetRewardYear" );
	var retouchColor = vetRewardItem.GetTag( "retouching" );

	// Handle vetRewardYear
	if( vetReward && VetRewardEnabled )
	{
		var indicator = "th";
		switch( vetReward )
		{
			case 1: indicator = "st"; break;
			case 2: indicator = "nd"; break;
			case 3: indicator = "rd"; break;
		}
		tooltipText = vetReward + indicator + " Year Veteran Reward";
	}

	// Handle retouching color
	if( retouchColor != null && coreShardEra >= EraStringToNum( "aos" ))
	{
		if( tooltipText != "" )
			tooltipText += "\n";

		tooltipText += "Mount Hue: " + retouchColor;
	}

	if( tooltipText != "" )
		vetRewardItem.SetTempTag( "tooltipSortOrder", 21 );

	return tooltipText;
}