const healSlipScriptID = 4014; // script ID assigned to this script in jse_fileassociations.scp
const coreShardEra = GetServerSetting( "CoreShardEra" );

// For registering "slips" when attempting to heal someone with bandages
function onDamage( damaged, attacker, damageValue, damageType )
{
	if( !damaged.skillsused.healing && !damaged.skillsused.veterinary )
	{
		// Player shouldn't have this script attached if they're not actively healing. Remove!
		damaged.SetTempTag( "slipCount", null );
		damaged.RemoveScriptTrigger( healSlipScriptID );
	}

	var slipOnPlayerDamage = 0;
	var slipOnNPCDamage = 0;
	if( coreShardEra >= EraStringToNum( "hs" ))
	{
		// With Publish 71 for HS, threshold for slipping now scales by dexterity
		slipOnPlayerDamage = 19 + Math.Round( damaged.dexterity / 12 );
		slipOnNPCDamage = 26 + Math.Round( damaged.dexterity / 12 );
	}
	else if( coreShardEra >= EraStringToNum( "se" ))
	{
		// With Publish 21 for SE, threshold was raised to 26 hp for both PvP/PvE
		// With Publish 25 for SE, threshold reduced from 26 to 19 for PvP only
		slipOnPlayerDamage = 19;
		slipOnNPCDamage = 26;
	}
	else
	{
		// Very low threshold for fingers slipping during bandaging in AoS? era and below
		// Possibly supposed to be 0, no real values to be found
		slipOnPlayerDamage = 2;
		slipOnNPCDamage = 5;
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