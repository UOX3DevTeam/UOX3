function onUseUnChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	var cloakItem = pUser.FindItemLayer( 0x14 );
	var ammoType = null;

	if( cloakItem )
	{
		ammoType = cloakItem.GetTag( "AmmoType" ); // Ensure cloakItem exists before using GetTag
	}

	var lastUsedBy = iUsed.GetTag( "lastUsedBy" );
	var lastUsed = iUsed.GetTag( "lastUsed" );
	var timeNow = GetCurrentClock();

	if( pUser.dead == 1 )
	{
		pUser.SysMessage( GetDictionaryEntry( 330, pSock.language )); // You are dead and cannot do that.
		return false;
	}

	if(( lastUsedBy != null && lastUsedBy != 0 ) && ( lastUsedBy != ( pUser.serial & 0x00FFFFFF )))
	{
		if(( lastUsed + 10000 ) > timeNow )
		{
			pUser.SysMessage( GetDictionaryEntry( 2450, pSock.language )); // Someone else is using this right now.
			return false;
		}
	}

	// Check if user is within range to retrieve arrows/bolts BEFORE shooting
	if( iUsed.InRange( pUser, 4 ))
	{
		if( iUsed.InRange( pUser, 1 ))
		{
			var stuckArrows = iUsed.GetTag( "stuckArrows" );
			var stuckBolts = iUsed.GetTag( "stuckBolts" );

			// Ensure the tags are not null
			if( stuckArrows == null )
				stuckArrows = 0;

			if( stuckBolts == null )
				stuckBolts = 0;

			if( stuckArrows > 0 || stuckBolts > 0 )
			{
				if( stuckArrows > 0 )
				{
					CreateDFNItem( pUser.socket, pUser, "0x0f3f", stuckArrows, "ITEM", true );
					var tempMsg = GetDictionaryEntry(2451, pSock.language ); // "You retrieve %i arrows from the target."
					pUser.SysMessage( tempMsg.replace(/%i/gi, stuckArrows ));
				}
				if( stuckBolts > 0 )
				{
					CreateDFNItem( pUser.socket, pUser, "0x1bfb", stuckBolts, "ITEM", true );
					var tempMsg = GetDictionaryEntry(2452, pSock.language ); // "You retrieve %i crossbow bolts from the target."
					pUser.SysMessage( tempMsg.replace(/%i/gi, stuckBolts ));
				}
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 943, pSock.language )); // "This target is empty."
			}

			// Reset the target's stored shot counts
			pUser.SysMessage( GetDictionaryEntry(2453, pSock.language )); // "You reset the target score."
			var resetTags = [
				"stuckArrows", "stuckBolts", "totalScore", "totalShots",
				"bullseyeShots", "innerShots", "middleShots", "outerShots",
				"lastUsed", "lastUsedBy"
			];

			for( var i = 0; i < resetTags.length; i++ )
			{
				iUsed.SetTag( resetTags[i], 0 );
			}

			return false;
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 1766, pSock.language )); // "You are too close to the target."
			return false;
		}
	}
	else if( !iUsed.InRange( pUser, 12 ))
	{
		pUser.SysMessage( GetDictionaryEntry( 1767, pSock.language )); // "You are too far from the archery butte to get an accurate shot."
		return false;
	}
	else if(( lastUsed + 1500 ) > timeNow )
	{
		pUser.SysMessage( GetDictionaryEntry( 1762, pSock.language )); // "You must wait before using this again."
		return false;
	}

	if( pUser.isonhorse )
	{
		pUser.SysMessage( GetDictionaryEntry( 1757, pSock.language )); // "Please dismount first."
		return false;
	}

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	var weaponType = TriggerEvent(2500, "GetWeaponType", pUser, null );
	if( weaponType != "BOWS" && weaponType != "XBOWS" )
	{
		pUser.SysMessage( GetDictionaryEntry( 947, pSock.language )); // You need to equip a bow to use this.
		return false;
	}

	var ammoID = ( weaponType == "BOWS" ) ? 0x0F3F : 0x1BFB;

	// Check player's inventory and quiver for ammo
	var playerAmmoCount = pUser.ResourceCount( ammoID, 0 );
	var quiverAmmoCount = ( cloakItem ) ? cloakItem.ResourceCount( ammoID, 0 ) : 0;
	var totalAmmoCount = playerAmmoCount + quiverAmmoCount;

	if( totalAmmoCount == 0 )
	{
		pUser.SysMessage( GetDictionaryEntry(( weaponType == "BOWS" ) ? 1770 : 1771, pSock.language ));
		return false;
	}

	// Try to consume from quiver first, then from player inventory
	var ammoConsumed = false;
	if( quiverAmmoCount > 0 && cloakItem.UseResource( 1, ammoID ))
	{
		ammoConsumed = true;
	}
	else if( playerAmmoCount > 0 )
	{
		pUser.UseResource( 1, ammoID );
		ammoConsumed = true;
	}

	if( !ammoConsumed )
	{
		pUser.SysMessage( GetDictionaryEntry(( weaponType == "BOWS" ) ? 1770 : 1771, pSock.language ));
		return false;
	}

	// Handle stuck arrows or bolts
	var stuckTag = ( weaponType == "BOWS" ) ? "stuckArrows" : "stuckBolts";
	var stuckCount = iUsed.GetTag( stuckTag );
	if( stuckCount == null )
	{
		stuckCount = 0;
	}
	iUsed.SetTag( stuckTag, stuckCount + 1 );

	// Animation and movement effect
	var combatAnim = ( weaponType == "BOWS" ) ? 18 : 19;
	pUser.DoAction( combatAnim );
	DoMovingEffect( pUser.x, pUser.y, pUser.z + 10, iUsed.x, iUsed.y, iUsed.z + 10, ammoID, 0x06, 0x00, false );

	// Check if player hits target
	if( !pUser.CheckSkill( 31, 0, 250 ))
	{
		pUser.SoundEffect( 0x0238, true );
		pUser.SysMessage(GetDictionaryEntry( 951, pSock.language )); // You miss the target.
		var tempMsg = GetDictionaryEntry( 2455, pSock.language ); // %s misses!
		iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ));
		return true;
	}

	pUser.SoundEffect( 0x0234, true );

	// Calculate score based on distance and skill
	var distanceToTarget = DistanceBetween( pUser, iUsed );
	var distancePenalty = Math.max( 0.05, 1 - ( distanceToTarget / 15 ));
	distancePenalty -= (( distanceToTarget / pUser.strength ) / 5 );

	var dexBonus = pUser.dexterity * 0.15;
	var skillBonus = pUser.skills.archery * 0.025;

	var bullseyeChance = Math.round(( skillBonus * distancePenalty ) + dexBonus );
	if( bullseyeChance > 65 )
		bullseyeChance = 65;

	var innerChance = Math.round(( pUser.skills.archery * 0.1 * distancePenalty ) + dexBonus );
	if( innerChance > 75 )
		innerChance = 75;

	var middleChance = Math.round(( pUser.skills.archery * 0.15 * distancePenalty ) + dexBonus );
	if( middleChance > 85 )
		middleChance = 85;

	var outerChance = Math.round(( pUser.skills.archery * 0.225 * distancePenalty ) + dexBonus );
	if( outerChance > 95 )
		outerChance = 95;

	var iNum = RandomNumber( 0, 100 );
	var points = 0;
	var hitType = "misses";

	var hitZones = [
		{ chance: bullseyeChance, points: 50, hitType: "hits the bullseye" },
		{ chance: innerChance, points: 10, hitType: "hits the inner ring" },
		{ chance: middleChance, points: 5, hitType: "hits the middle ring" },
		{ chance: outerChance, points: 2, hitType: "hits the outer ring" }
	];

	for( var i = 0; i < hitZones.length; i++ )
	{
		if( hitZones[i].chance >= iNum )
		{
			points = hitZones[i].points;
			hitType = hitZones[i].hitType;
			break;
		}
	}

	if( points == 0 )
	{
		pUser.SoundEffect( 0x0238, true );
	}

	var tempMsg = GetDictionaryEntry( 2457, pSock.language ); // %s hits the target!
	iUsed.TextMessage(tempMsg.replace( /%s/gi, pUser.name + " " + hitType ));

	// Update score
	var totalShots = iUsed.GetTag( "totalShots" );
	if( totalShots == null )
		totalShots = 0;

	iUsed.SetTag( "totalShots", totalShots + 1 );

	var totalScore = iUsed.GetTag( "totalScore" );
	if( totalScore == null )
		totalScore = 0;

	iUsed.SetTag( "totalScore", totalScore + points );

	iUsed.SetTag( "lastUsedBy", ( pUser.serial & 0x00FFFFFF ));
	iUsed.SetTag( "lastUsed", GetCurrentClock() );

	// Report overall score
	tempMsg = GetDictionaryEntry( 2464, pSock.language ); // Score: %i after %u shots.
	tempMsg = tempMsg.replace( /%i/gi, totalScore );
	iUsed.TextMessage( tempMsg.replace( /%u/gi, totalShots ));

	// **Update Quiver Ammo Count**
	if( totalAmmoCount > 0 && cloakItem )
	{
		cloakItem.SetTag( "AmmoCount", totalAmmoCount - 1 );
		cloakItem.Refresh();
	}

	return true;
}