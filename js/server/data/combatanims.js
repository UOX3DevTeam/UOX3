// This is a generic script for determining combat animations based on whether or not a character is mounted
// You can call this script from an external script by using either of the two following lines after
// first defining weaponType through weapontypes.js (scriptID 2500)...
//		TriggerEvent( 2501, "getFootCombatAnim", pUser, weaponType );
//		TriggerEvent( 2501, "getHorseCombatAnim", pUser, weaponType );
// ..Plus these lines:
// 		var combatAnim = StringToNum( pUser.GetTag( "combatAnim" ));
//		pUser.SetTag( "combatAnim", null );

function getFootCombatAnim( pUser, weaponType )
{
	switch( weaponType )
	{
		case "BOWS": pUser.SetTag( "combatAnim", "0x12" ); break;
		case "XBOWS": pUser.SetTag( "combatAnim", "0x13" ); break;
		case "DEF_SWORDS":
		case "SLASH_SWORDS":
		case "ONEHND_LG_SWORDS":
		case "DUAL_SWORD":
		case "DUAL_FENCING_SLASH":
		case "ONEHND_AXES":		
			switch( RandomNumber( 0, 1 ) )
			{
				case 0:
					pUser.SetTag( "combatAnim", "0x09" ); break;
				case 1:
					pUser.SetTag( "combatAnim", "0x0A" ); break;
			}
			break;
		case "DEF_MACES":
			switch( RandomNumber( 0, 1 ) )
			{
				case 0:
					pUser.SetTag( "combatAnim", "0x09" ); break;
				case 1:
					pUser.SetTag( "combatAnim", "0x0B" ); break;
			}
			break;
		case "LG_MACES":
		case "TWOHND_LG_SWORDS":
		case "BARDICHE":
		case "TWOHND_AXES":
			switch( RandomNumber( 0, 1 ) )
			{
				case 0:
					pUser.SetTag( "combatAnim", "0x0C" ); break;
				case 1:
					pUser.SetTag( "combatAnim", "0x0D" ); break;
			}
			break;
		case "DUAL_FENCING_STAB":
		case "DEF_FENCING": pUser.SetTag( "combatAnim", "0x0A" ); break;
		case "TWOHND_FENCING": pUser.SetTag( "combatAnim", "0x0E" ); break;
		case "WRESTLING":
		default:
			switch( RandomNumber( 0, 2 ) )
			{
				case 0:	pUser.SetTag( "combatAnim", "0x0A" ); break;  //fist straight-punch
				case 1:	pUser.SetTag( "combatAnim", "0x09" ); break;  //fist top-down
				default: pUser.SetTag( "combatAnim", "0x1F" ); break;  //fist over-head
			}
			break;
	}
}

function getHorseCombatAnim( pUser, weaponType )
{
	switch( weaponType )
	{
		case "BOWS": pUser.SetTag( "combatAnim", "0x1B" ); break;
		case "XBOWS": pUser.SetTag( "combatAnim", "0x1C" ); break;
		case "TWOHND_FENCING":
		case "TWOHND_LG_SWORDS":
		case "BARDICHE":
		case "TWOHND_AXES":
		case "LG_MACES": pUser.SetTag( "combatAnim", "0x1D" ); break;
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
			pUser.SetTag( "combatAnim", "0x1A" ); break;
	}
}