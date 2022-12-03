const healSlipScriptID = 4014; // script ID assigned to this script in jse_fileassociations.scp
const slipOnPlayerDamage = 19;
const slipOnNPCDamage = 26;

// For registering "slips" when attempting to heal someone with bandages
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( !damaged.skillsused.healing && !damaged.skillsused.veterinary )
	{
		// Player shouldn't have this script attached if they're not actively healing. Remove!
		damaged.SetTempTag( "slipCount", null );
		damaged.RemoveScriptTrigger( healSlipScriptID );
	}

	// Only have player "slip up" at healing for high damage numbers
	if( !ValidateObject( attacker ) ||
		( damageValue >= slipOnNPCDamage && attacker.npc && !attacker.isHuman ) ||
		( damageValue >= slipOnPlayerDamage && !attacker.npc ))
	{
		damaged.SetTempTag( "slipCount", damaged.GetTempTag( "slipCount" ) + 1 );
		if( damaged.socket )
		{
			damaged.socket.SysMessage( GetDictionaryEntry( 9088, damaged.socket.language )); // Your fingers slip!
		}
	}
	return true;
}