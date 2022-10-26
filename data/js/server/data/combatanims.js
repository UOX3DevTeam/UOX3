// This is a generic script for determining combat animations based on whether or not a character is mounted
// You can call this script from an external script by using either of the two following lines after
// first defining weaponType through weapontypes.js (scriptID 2500)...
//		var combatAnim = TriggerEvent( 2501, "GetFootCombatAnim", pUser, weaponType );
//		var combatAnim = TriggerEvent( 2501, "GetHorseCombatAnim", pUser, weaponType );

function GetFootCombatAnim( pUser, weaponType )
{
	switch( weaponType )
	{
		case "BOWS":
			combatAnim = 0x12;
			break;
		case "XBOWS":
			combatAnim = 0x13;
			break;
		case "DEF_SWORDS":
		case "SLASH_SWORDS":
		case "ONEHND_LG_SWORDS":
		case "DUAL_SWORD":
		case "DUAL_FENCING_SLASH":
		case "ONEHND_AXES":		
		{
			switch( RandomNumber( 0, 1 ) )
			{
				case 0:
					combatAnim = 0x09;
					break;
				case 1:
					combatAnim = 0x0A;
					break;
			}
			break;
		}
		case "DEF_MACES":
		{
			switch( RandomNumber( 0, 1 ) )
			{
				case 0:
					combatAnim = 0x09;
					break;
				case 1:
					combatAnim = 0x0B;
					break;
			}
			break;
		}
		case "LG_MACES":
		case "TWOHND_LG_SWORDS":
		case "BARDICHE":
		case "TWOHND_AXES":
		{
			switch( RandomNumber( 0, 1 ) )
			{
				case 0:
					combatAnim = 0x0C;
					break;
				case 1:
					combatAnim = 0x0D;
					break;
			}
			break;
		}
		case "DUAL_FENCING_STAB":
		case "DEF_FENCING":
			combatAnim = 0x0A;
			break;
		case "TWOHND_FENCING":
			combatAnim = 0x0E;
			break;
		case "WRESTLING":
		default:
		{
			switch( RandomNumber( 0, 2 ) )
			{
				case 0:	combatAnim = 0x0A; //fist straight-punch
					break;
				case 1:	combatAnim = 0x09; //fist top-down
					break;
				default: combatAnim = 0x1F; //fist over-head
					break;
			}
			break;
		}
	}
	return combatAnim;
}

function GetHorseCombatAnim( pUser, weaponType )
{
	var combatAnim = 0x1A;
	switch( weaponType )
	{
		case "BOWS":
			combatAnim = 0x1B;
			break;
		case "XBOWS":
			combatAnim = 0x1C;
			break;
		case "TWOHND_FENCING":
		case "TWOHND_LG_SWORDS":
		case "BARDICHE":
		case "TWOHND_AXES":
		case "LG_MACES":
			combatAnim = 0x1D;
			break;
		case "DEF_SWORDS":
		case "SLASH_SWORDS":
		case "DUAL_SWORD":
		case "ONEHND_LG_SWORDS":
		case "ONEHND_AXES":		
		case "DEF_MACES":
		case "DUAL_FENCING_SLASH":
		case "DUAL_FENCING_STAB":
		case "DEF_FENCING":
		case "WRESTLING":
		default:
			combatAnim = 0x1A;
			break;
	}
	return combatAnim;
}