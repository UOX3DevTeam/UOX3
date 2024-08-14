function onUseUnChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	var cloakItem = pUser.FindItemLayer( 0x14 );
	// Check for ammo type in the quiver
	var ammoType = cloakItem.GetTag( "AmmoType" );
	var lastUsedBy = iUsed.GetTag( "lastUsedBy" );
	var lastUsed = iUsed.GetTag( "lastUsed" );
	var timeNow = GetCurrentClock();

	if( pUser.dead == 1 )
	{
		pUser.SysMessage( GetDictionaryEntry( 330, pSock.language )); // You are dead and cannot do that.
		return false;
	}

	if(( lastUsedBy != null || lastUsedBy != 0 ) && ( lastUsedBy != ( pUser.serial & 0x00FFFFFF )))
	{
		if(( lastUsed + 10000 ) > timeNow )
		{
			pUser.SysMessage( GetDictionaryEntry( 2450, pSock.language )); // Someone else is using this right now.
			return false;
		}
	}
	// Check if user is standing right next to dummy, and if there are arrows/bolts to collect
	if( iUsed.InRange( pUser, 4 ))
	{
		if( iUsed.InRange( pUser, 1 ) )
		{
			var stuckArrows = iUsed.GetTag( "stuckArrows" );
			var stuckBolts = iUsed.GetTag( "stuckBolts" );
			if( stuckArrows > 0 || stuckBolts > 0 )
			{
				if( stuckArrows > 0 )
				{
					var newArrows = CreateDFNItem( pUser.socket, pUser, "0x0f3f", stuckArrows, "ITEM", true );
					var tempMsg = GetDictionaryEntry( 2451, pSock.language ) // You retrieve %i arrows from the target.
					pUser.SysMessage( tempMsg.replace( /%i/gi, stuckArrows ) );
				}
				if( stuckBolts > 0 )
				{
					var newBolts = CreateDFNItem( pUser.socket, pUser, "0x1bfb", stuckBolts, "ITEM", true );
					var tempMsg = GetDictionaryEntry( 2452, pSock.language ) // You retrieve %i crossbow bolts from the target.
					pUser.SysMessage( tempMsg.replace( /%i/gi, stuckBolts ) );
				}
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 943, pSock.language )); //This target is empty
			}

			pUser.SysMessage( GetDictionaryEntry( 2453, pSock.language )); // You reset the target score.
			iUsed.SetTag( "stuckArrows", 0 );
			iUsed.SetTag( "stuckBolts", 0 );
			iUsed.SetTag( "totalScore", 0 );
			iUsed.SetTag( "totalShots", 0 );
			iUsed.SetTag( "bullseyeShots", 0 );
			iUsed.SetTag( "innerShots", 0 );
			iUsed.SetTag( "middleShots", 0 );
			iUsed.SetTag( "outerShots", 0 );
			iUsed.SetTag( "lastUsed", 0 );
			iUsed.SetTag( "lastUsedBy", 0 );
			return false;
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 1766, pSock.language )); // You are too close to the target.
		}
	}
	else if( !iUsed.InRange( pUser, 12 ))
	{
		pUser.SysMessage( GetDictionaryEntry( 1767, pSock.language )); // You are too far away from the archery butte to get an accurate shot.
	}
	else if(( lastUsed + 1500 ) > timeNow )
	{
		pUser.SysMessage( GetDictionaryEntry( 1762, pSock.language )); // You must wait before you can use that item again.
		return false;
	}
	else
	{
		// Close enough, and not too far away =)
		if( pUser.isonhorse )
		{
			pUser.SysMessage( GetDictionaryEntry( 1757, pSock.language )); // Please dismount first.
			return false;
		}
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}
		if( iUsed.id == 0x100a)
		{
			if( pUser.direction != 6 )
			{
				pUser.TurnToward( iUsed );
				pUser.SysMessage( GetDictionaryEntry( 2454, pSock.language )); // You turn to face the target.
				return false;
			}
			if( pUser.x < iUsed.x || pUser.y != iUsed.y )
			{
				if( pUser.x < iUsed.x )
				{
					pUser.SysMessage( GetDictionaryEntry( 1768, pSock.language )); // You would do better to stand in front of the archery butte.
				}
				if( pUser.y != iUsed.y )
				{
					pUser.SysMessage( GetDictionaryEntry( 1769, pSock.language )); // You aren't properly lined up with the archery butte to get an accurate shot.
				}
				return false;
			}
		}
		else if( iUsed.id == 0x100b )
		{
			if( pUser.direction != 0 )
			{
				pUser.TurnToward( iUsed );
				pUser.SysMessage( GetDictionaryEntry( 2454, pSock.language )); // You turn to face the target.
				return false;
			}
			if( pUser.y < iUsed.y || pUser.x != iUsed.x )
			{
				if( pUser.y < iUsed.y )
				{
					pUser.SysMessage( GetDictionaryEntry( 1768, pSock.language )); // You would do better to stand in front of the archery butte.
				}
				if( pUser.x != iUsed.x )
				{
					pUser.SysMessage( GetDictionaryEntry( 1769, pSock.language )); // You aren't properly lined up with the archery butte to get an accurate shot.
				}
				return false;
			}
		}
		var weaponType = TriggerEvent( 2500, "GetWeaponType", pUser, null );
		if( weaponType != "BOWS" && weaponType != "XBOWS" )
		{
			pUser.SysMessage( GetDictionaryEntry( 947, pSock.language )); // You need to equip a bow to use this.
			return false;
		}
		else
		{
			switch( weaponType )
			{
				case "BOWS":
					var combatAnim = 18;
					var arrowCount = pUser.ResourceCount( 0x0F3F, 0 );
					var totalAmmoCount = 0;
					for( var mItem = cloakItem.FirstItem(); !cloakItem.FinishedItems(); mItem = cloakItem.NextItem() )
					{
						if( ammoType == "Arrow" && mItem.id == 0x0f3f )
						{
							totalAmmoCount += mItem.amount;
							if( mItem.amount > 0 )
								mItem.amount -= 1;
							else
								mItem.Delete();
						}
					}
					if( arrowCount == 0 && totalAmmoCount == 0)
					{
						pUser.SysMessage( GetDictionaryEntry( 1770, pSock.language )); // You do not have any arrows with which to practice.
						return false;
					}
					else
					{
						var ammoType = "0x0f42";
						pUser.UseResource( 1, 0x0F3F );
						var stuckArrows = iUsed.GetTag( "stuckArrows" ) + 1;
						iUsed.SetTag( "stuckArrows", stuckArrows );
					}
					break;
				case "XBOWS":
					var combatAnim = 19;
					var boltCount = pUser.ResourceCount( 0x1BFB, 0 );
					var totalAmmoCount = 0;
					for( var mItem = cloakItem.FirstItem(); !cloakItem.FinishedItems(); mItem = cloakItem.NextItem() )
					{
						if( ammoType == "Bolt" && mItem.id == 0x1bfb )
						{
							totalAmmoCount += mItem.amount;
							if( mItem.amount > 0 )
								mItem.amount -= 1;
							else
								mItem.Delete();
						}
					}
					if( boltCount == 0 && totalAmmoCount == 0)
					{
						pUser.SysMessage( GetDictionaryEntry( 1771, pSock.language )); // You do not have any bolts with which to practice.
						return false;
					}
					else
					{
						var ammoType = "0x1bfe";
						pUser.UseResource( 1, 0x1BFB );
						var stuckBolts = iUsed.GetTag( "stuckBolts" ) + 1;
						iUsed.SetTag( "stuckBolts", stuckBolts );
					}
					break;
			}
			pUser.DoAction( combatAnim );
			ammoType = parseInt( ammoType );
			DoMovingEffect( pUser.x, pUser.y, pUser.z + 10, iUsed.x, iUsed.y, iUsed.z + 10, ammoType, 0x06, 0x00, false );
			var points = 0;
			if( !pUser.CheckSkill( 31, 0, 250 ))
			{
				pUser.SoundEffect( 0x0238, true );
				pUser.SysMessage( GetDictionaryEntry( 951, pSock.language )); // You miss the target.
				var tempMsg = GetDictionaryEntry( 2455, pSock.language ) // %s misses!
				iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
			}
			else
			{
				pUser.SoundEffect( 0x0234, true );

				// Calculate distance penalty
				var distanceToTarget = DistanceBetween( pUser, iUsed );
				var distancePenalty = Math.max( 0.05, 1 - (distanceToTarget / 15 ));

				// Modify distance penalty based on strength
				distancePenalty -= (( distanceToTarget / pUser.strength ) / 5 );

				// Calculate dexterity bonus
				var dexBonus = pUser.dexterity * 0.15;

				// Calculate chance of hitting bullseye
				var bullseyeChance = Math.round((( pUser.skills.archery * 0.025 ) * distancePenalty ) + dexBonus );
				if( bullseyeChance > 65 )
				{
					bullseyeChance = 65;
				}

				// Calculate chance of hitting inner ring
				var innerChance = Math.round((( pUser.skills.archery * 0.1 ) * distancePenalty ) + dexBonus );
				if( innerChance > 75 )
				{
					innerChance = 75;
				}

				// Calculate chance of hitting middle ring
				var middleChance = Math.round((( pUser.skills.archery * 0.15 ) * distancePenalty ) + dexBonus );
				if( middleChance > 85 )
				{
					middleChance = 85;
				}

				// Calculate chance of hitting outer ring
				var outerChance = Math.round((( pUser.skills.archery * 0.225 ) * distancePenalty ) + dexBonus );
				if( outerChance > 95 )
				{
					outerChance = 95;
				}

				var iNum = RandomNumber( 0, 100 );

				if( bullseyeChance >= iNum )
				{
					//BULLSEYE!
					var bullseyeShots = iUsed.GetTag( "bullseyeShots" );
					iUsed.SetTag( "bullseyeShots", bullseyeShots + 1 );
					var bullseyeSplit = false;
					if( bullseyeShots > 0 )
					{
						var bullseyeSplitChance = RandomNumber( 0, 100 );
						if(( 15 + (( pUser.skills.archery / 100 ) * 2 )) > bullseyeSplitChance )
						{
							var tempMsg = GetDictionaryEntry( 2456, pSock.language ) // %s splits an arrow in the bullseye!
							iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
							points = 100;
							bullseyeSplit = true;
							pUser.CheckSkill( 31, 0, 250 ); // Extra bonus skill check for hitting bullseye
						}
					}
					if( !bullseyeSplit )
					{
						var tempMsg = GetDictionaryEntry( 2457, pSock.language ) // %s hits the bullseye!
						iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
                        points = 50;
					}
				}
				else if( innerChance >= iNum )
				{
					//Inner Ring
					var innerShots = iUsed.GetTag( "innerShots" );
					iUsed.SetTag( "innerShots", innerShots + 1 );
					var innerShotSplit = false;
					if( innerShots > 0 )
					{
						var innerShotSplitChance = RandomNumber( 0, 100 );
						if(( 15 + (( pUser.skills.archery / 100 ) * 2 )) > innerShotSplitChance )
						{
							var tempMsg = GetDictionaryEntry( 2458, pSock.language ) // %s splits an arrow in the inner ring!
							iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
							points = 20;
							innerShotSplit = true;
						}
					}
					if( !innerShotSplit )
					{
						var tempMsg = GetDictionaryEntry( 2459, pSock.language ) // %s hits the inner ring.
						iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
						points = 10;
					}
				}
				else if( middleChance >= iNum )
				{
					//Middle Ring
					var middleShots = iUsed.GetTag( "middleShots" );
					iUsed.SetTag( "middleShots", middleShots + 1 );
					var middleShotSplit = false;
					if( middleShots > 0 )
					{
						var middleShotSplitChance = RandomNumber( 0, 100 );
						if(( 15 + (( pUser.skills.archery / 100 ) * 2 )) > middleShotSplitChance )
						{
							var tempMsg = GetDictionaryEntry( 2460, pSock.language ) // %s splits an arrow in the middle ring!
							iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
							points = 15;
							middleShotSplit = true;
						}
					}
					if( !middleShotSplit )
					{
						var tempMsg = GetDictionaryEntry( 2461, pSock.language ) // %s hits the middle ring.
						iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
						points = 5;
					}
				}
				else if( outerChance >= iNum )
				{
					//Outer Ring
					var outerShots = iUsed.GetTag( "outerShots" );
					iUsed.SetTag( "outerShots", outerShots + 1 );
					var outerShotSplit = false;
					if( outerShots > 0 )
					{
						var outerShotSplitChance = RandomNumber( 0, 100 );
						if(( 15 + (( pUser.skills.archery / 100 ) * 2 )) > outerShotSplitChance )
						{
							var tempMsg = GetDictionaryEntry( 2462, pSock.language ) // %s splits an arrow in the outer ring!
							iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
							points = 5;
							outerShotSplit = true;
						}
					}
					if( !outerShotSplit )
					{
						var tempMsg = GetDictionaryEntry( 2463, pSock.language ) // %s hits the outer ring.
						iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
						points = 2;
					}
				}
				else
				{
					pUser.SoundEffect( 0x0238, true );
					pUser.SysMessage( GetDictionaryEntry( 951, pSock.language )); //You miss the target.
					var tempMsg = GetDictionaryEntry( 2455, pSock.language ) // %s misses
					iUsed.TextMessage( tempMsg.replace( /%s/gi, pUser.name ) );
				}
			}

			// Update tags on archery butte with current status
			var totalShots = iUsed.GetTag( "totalShots" ) + 1;
			iUsed.SetTag( "totalShots", totalShots );
			var totalScore = iUsed.GetTag( "totalScore" ) + points;
			iUsed.SetTag( "totalScore", totalScore );
			iUsed.SetTag( "lastUsedBy", ( pUser.serial & 0x00FFFFFF ) );
			var lastUsed = GetCurrentClock();
			iUsed.SetTag( "lastUsed", lastUsed );
			if( totalAmmoCount != 0 )
			{
				cloakItem.SetTag( "AmmoCount", totalAmmoCount - 1 );
				cloakItem.Refresh();
			}

			// Report overall score
			var tempMsg = GetDictionaryEntry( 2464, pSock.language ) // Score: %i after %u shots.
			tempMsg = tempMsg.replace( /%i/gi, totalScore );
			iUsed.TextMessage( tempMsg.replace( /%u/gi, totalShots ) );
		}
	}
	return false;
}