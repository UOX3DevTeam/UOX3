// Skill-boosting Equipment

// Original item rewards
// 		Leather gloves of mining
//			Increases mining skill by 1 point. Dyeable using a leather dyetub.
//
// 		Studded leather gloves of mining
//			Increases mining skill by 3point. Dyeable using a leather dyetub.
//
// 		Ringmail gloves of mining
//			Increases mining skill by 5 points.
//
// 		Ancient smithy hammer
//			Increase blacksmithing skill by 10, 15, 30 or 60 points when equipped

function onEquip( pEquipper, iEquipped )
{
	var skillBonusID = iEquipped.GetTag( "skillBonusID" );
	var skillBonusVal = iEquipped.GetTag( "skillBonusVal" );

	switch( skillBonusID )
	{
		case 7: // Blacksmithing
			pEquipper.skills.blacksmithing += skillBonusVal;
			break;
		case 45: // Mining
			pEquipper.skills.mining += skillBonusVal;
			break;
		default:
			break;
	}
}

function onUnequip( pUnequipper, iUnequipped )
{
	var skillBonusID = iUnequipped.GetTag( "skillBonusID" );
	var skillBonusVal = iUnequipped.GetTag( "skillBonusVal" );

	switch( skillBonusID )
	{
		case 7: // Blacksmithing
			pUnequipper.skills.blacksmithing -= skillBonusVal;
			break;
		case 45: // Mining
			pUnequipper.skills.mining -= skillBonusVal;
			break;
		default:
			break;
	}
}

// Display bonus skill in tooltip
function onTooltip( myObj )
{
	var tooltipText = "";
	var skillBonusID = myObj.GetTag( "skillBonusID" );
	var skillBonusVal = myObj.GetTag( "skillBonusVal" );

	switch( skillBonusID )
	{
		case 7: // Blacksmithing
			tooltipText = "Blacksmithing: +" + ( skillBonusVal / 10 );
			break;
		case 45: // Mining
			tooltipText = "Mining: +" + ( skillBonusVal / 10 );
			break;
		default:
			break;
	}

	return tooltipText;
}