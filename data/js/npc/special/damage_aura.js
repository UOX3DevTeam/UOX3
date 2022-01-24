// Damage Aura/AoE ability for NPCs
// To add a damage aura to an NPC, add their ID and details of their aura to the GetAuraInit and
// GetAuraProperties functions at the bottom of the script, and then assign script 3503 to the NPC!

function onCombatStart( pChar, myTarget )
{
	if( ValidateObject( myTarget ) && !pChar.GetTempTag( "AOERunning" ))
	{
		pChar.SetTempTag( "AOERunning", 1 );
		var AuraInit = GetAuraInit( pChar );
		pChar.StartTimer( RandomNumber( AuraInit.minTime, AuraInit.maxTime ), 1, true );
	}
	return true;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		var AuraInit = GetAuraInit( timerObj );
		var damageCount = AreaCharacterFunction( "DealAreaDamage", timerObj, AuraInit.range );
		if( damageCount == 0 )
		{
			// Check if there are any characters in range before deciding to continue with AoE damage
			var nearbyPlayerCount = AreaCharacterFunction( "CheckForNearbyPlayers", timerObj, 18 );
			if( nearbyPlayerCount == 0 )
			{
				// No other players nearby - halt AoE operations!
				timerObj.SetTempTag( "AOERunning", null );
				return;
			}
		}
		timerObj.StartTimer( RandomNumber( AuraInit.minTime, AuraInit.maxTime ), 1, true );
	}
}

function DealAreaDamage( srcObj, trgChar )
{
	if( ValidateObject( trgChar ) && trgChar != srcObj && trgChar.vulnerable && !trgChar.dead
		&& (( trgChar.npc && ( trgChar.tamed || trgChar.hireling )) || trgChar.online ))
	{
		var auraProps = GetAuraProperties( srcObj );
		if( trgChar.socket != null )
			trgChar.SysMessage( auraProps.sysMsgHue, GetDictionaryEntry( auraProps.dictID, trgChar.socket ));

		// Play a "static" moving effect on target, and sound
		DoMovingEffect( trgChar, trgChar, auraProps.fxID, 0, auraProps.fxLength, auraProps.explode, auraProps.fxHue, auraProps.fxRender );
		trgChar.SoundEffect( auraProps.sfx, true );

		// Apply poison, if applicable
		if( auraProps.poisonLvl > 0 )
			trgChar.SetPoisoned( auraProps.poisonLvl, auraProps.poisonTime );

		// Apply damage, if applicable
		if( auraProps.maxDmg > 0 )
			trgChar.Damage( RandomNumber( auraProps.minDmg, auraProps.maxDmg ), auraProps.dmgType );
		return true;
	}
	return false;
}

function CheckForNearbyPlayers( srcObj, trgChar )
{
	if( ValidateObject( trgChar ) && trgChar != srcObj && trgChar.vulnerable && !trgChar.dead
		&& (( trgChar.npc && ( trgChar.tamed || trgChar.hireling )) || trgChar.online ))
		return true;
	else
		return false;
}

function GetAuraInit( trgChar )
{
	// Min time, max time, range
	var AuraInit;

	// If multiple NPCs share the same ID, check for extra stuff like skin color, name, etc.
	switch( trgChar.id )
	{
		case 0x0005: // Phoenix
		case 0x005a: // Lava Serpent
		case 0x006c: // Bronze Elemental
		case 0x00a3: // Snow Elemental
			AuraInit = { minTime:5000, maxTime:10000, range:2 };
			break;
		case 0x02ff: // Betrayer
			AuraInit = { minTime:25000, maxTime:35000, range:2 };
			break;
		default:
			break;
	}
	return AuraInit;
}

function GetAuraProperties( trgChar )
{
	// sysMsgHue, DictID, FX, FXLength, Explode?, FXHue, FXRenderMode, SFX, MinDmg, MaxDmg, DmgType
	var auraProps;

	// If multiple NPCs share the same ID, check for extra stuff like skin color, name, etc.
	switch( trgChar.id )
	{
		case 0x0005: // Phoenix
			auraProps = { sysMsgHue:0x096a, dictID:9061, fxID:0x374a, fxLength:0x0f, explode:false, fxHue:0x2a, fxRender:0, sfx:0x5cf, minDmg:10, maxDmg:15, dmgType:5, poisonLvl:0, poisonTime: 0 };
			break;
		case 0x006c: // Bronze Elemental
			auraProps = { sysMsgHue:0x096a, dictID:9062, fxID:0x36b0, fxLength:0x0d, explode:false, fxHue:0x2a, fxRender:0, sfx:0x5cf, minDmg:10, maxDmg:15, dmgType:5, poisonLvl:0, poisonTime: 0 };
			break;
		case 0x00a3: // Snow Elemental
			auraProps = { sysMsgHue:0x096a, dictID:9063, fxID:0x374a, fxLength:0x0f, explode:false, fxHue:0x47d, fxRender:0, sfx:0x5c6, minDmg:5, maxDmg:10, dmgType:4, poisonLvl:0, poisonTime: 0 };
			break;
		case 0x005a: // Lava Serpent
			auraProps = { sysMsgHue:0x096a, dictID:9061, fxID:0x374a, fxLength:0x0f, explode:false, fxHue:0x2a, fxRender:0, sfx:0x5cf, minDmg:5, maxDmg:10, dmgType:5, poisonLvl:0, poisonTime: 0 };
			break;
		case 0x02ff: // Betrayer
			auraProps = { sysMsgHue:0x096a, dictID:9126, fxID:0x376a, fxLength:0x0f, explode:false, fxHue:0x2539, fxRender:0, sfx:0x1de, minDmg:0, maxDmg:0, dmgType:6, poisonLvl:4, poisonTime: 180000 };
			break;
		default:
			break;
	}
	return auraProps;
}