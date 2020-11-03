function onUseUnChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	var lastUsedBy = iUsed.GetTag( "lastUsedBy" );
	var lastUsed = iUsed.GetTag( "lastUsed" );
	var timeNow = GetCurrentClock();

	if( pUser.dead == 1 )
	{
		pUser.SysMessage( GetDictionaryEntry( 330, pSock.language )); //You are dead and cannot do that.
		return false;
	}

	if(( lastUsedBy != null || lastUsedBy != 0 ) && ( lastUsedBy != ( pUser.serial & 0x00FFFFFF )))
	{
		if(( lastUsed + 10000 ) > timeNow )
		{
			pUser.SysMessage( "Someone else is using this right now." );
			return false;
		}
	}
	//Check if user is standing right next to dummy, and if there are arrows/bolts to collect
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
					pUser.SysMessage( "You retrieve "+stuckArrows+" arrows from the target." );
				}
				if( stuckBolts > 0 )
				{
					var newBolts = CreateDFNItem( pUser.socket, pUser, "0x1bfb", stuckBolts, "ITEM", true );
					pUser.SysMessage( "You retrieve "+stuckBolts+" crossbow bolts from the target." );
				}
			}
			else
				pUser.SysMessage( GetDictionaryEntry( 943, pSock.language )); //This target is empty
			pUser.SysMessage( "You reset the target score." );
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
			pUser.SysMessage( GetDictionaryEntry( 1766, pSock.language )); //You are too close to the target.
	}
	else if( !iUsed.InRange( pUser, 7 ))
		pUser.SysMessage( GetDictionaryEntry( 1767, pSock.language )); //You are too far away from the archery butte to get an accurate shot.
	else if(( lastUsed + 1500 ) > timeNow )
	{
		pUser.SysMessage( "You must wait a moment before using this again." );
		return false;
	}
	else
	{ //Close enough, and not too far away =)
		if( pUser.isonhorse )
		{
			pUser.SysMessage( GetDictionaryEntry( 1757, pSock.language )); //Please dismount first.
			return false;
		}
		if( iUsed.id == 0x100a)
		{
			if( pUser.direction != 6 )
			{
				pUser.TurnToward( iUsed );
				pUser.SysMessage( "You weren't even facing the target! Try again." );
				return false;
			}
			if( pUser.x < iUsed.x || pUser.y != iUsed.y )
			{
				if( pUser.x < iUsed.x )
					pUser.SysMessage( GetDictionaryEntry( 1768, pSock.language )); //You would do better to stand in front of the archery butte.
				if( pUser.y != iUsed.y )
					pUser.SysMessage( GetDictionaryEntry( 1769, pSock.language )); //You aren't properly lined up with the archery butte to get an accurate shot.
				return false;
			}
		}
		else if( iUsed.id == 0x100b )
		{
			if( pUser.direction != 0 )
			{
				pUser.TurnToward( iUsed );
				pUser.SysMessage( "You weren't even facing the target! Try again." );
				return false;
			}
			if( pUser.y < iUsed.y || pUser.x != iUsed.x )
			{
				if( pUser.y < iUsed.y )
					pUser.SysMessage( GetDictionaryEntry( 1768, pSock.language )); //You would do better to stand in front of the archery butte.
				if( pUser.x != iUsed.x )
					pUser.SysMessage( GetDictionaryEntry( 1769, pSock.language )); //You aren't properly lined up with the archery butte to get an accurate shot.
				return false;
			}
		}
		TriggerEvent( 2500, "getWeaponType", pUser );
		var weaponType = pUser.GetTag( "weaponType" );
		pUser.SetTag( "weaponType", null );
		if( weaponType != "BOWS" && weaponType != "XBOWS" )
		{
			pUser.SysMessage( GetDictionaryEntry( 947, pSock.language )); //You need to equip a bow to use this.
			return false;
		}
		else
		{
			switch( weaponType )
			{
				case "BOWS":
					var combatAnim = 18;
					var Arrows = pUser.ResourceCount( 0x0F3F, 0 );
					if( Arrows == 0 )
					{
						pUser.SysMessage( GetDictionaryEntry( 1770, pSock.language )); //You do not have any arrows with which to practice.
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
					var Bolts = pUser.ResourceCount( 0x1BFB, 0 );
					if( Bolts == 0 )
					{
						pUser.SysMessage( GetDictionaryEntry( 1771, pSock.language )); //You do not have any bolts with which to practice.
						return false;
					}
					else
					{
						var ammoType = "0x1bfe";
						pUser.UseResource( 1, 0x1BFB );
						var stuckBolts = iUsed.GetTag( "stuckBolts" ) + 1;
						iUsed.SetTag( "stuckBolts", stuckBolts );
					}
			}
			pUser.DoAction( combatAnim );
			ammoType = parseInt( ammoType );
			DoMovingEffect( pUser, iUsed, ammoType, 0x06, 0x00, false );
			var points = 0;
			if(!pUser.CheckSkill( 31, 0, 250 ))
			{
				pUser.SoundEffect( 0x0238, true );
				pUser.SysMessage( GetDictionaryEntry( 951, pSock.language )); //You miss the target.
				iUsed.TextMessage( pUser.name +" misses!" );
			}
			else
			{
				pUser.SoundEffect( 0x0234, true );
				var BullseyeChance = pUser.skills.archery * 0.10;
				var InnerChance = pUser.skills.archery * 0.105;
				var MiddleChance = pUser.skills.archery * 0.115;
				var OuterChance = pUser.skills.archery * 0.12 ;
				var iNum = RandomNumber( 0, 150 );

				if( BullseyeChance > iNum )
				{
					//BULLSEYE!
					var bullseyeShots = iUsed.GetTag( "bullseyeShots" );
					iUsed.SetTag( "bullseyeShots", bullseyeShots + 1 );
					if( bullseyeShots > 0 )
					{
						var BullseyeSplit = RandomNumber( 0, 100 );
						if(( 30 + (( pUser.skills.archery / 100 ) * 2 )) > BullseyeSplit )
						{
							iUsed.TextMessage( pUser.name +" splits an arrow in the bullseye!" );
							points = 100;
							var Bullseye = 1;
						}
					}
					if( !Bullseye )
					{
						iUsed.TextMessage( pUser.name +" hits a BULLSEYE!" );
                        points = 50;
					}
				}
				else if( InnerChance > iNum )
				{
					//Inner Ring
					var innerShots = iUsed.GetTag( "innerShots" );
					iUsed.SetTag( "innerShots", innerShots + 1 );
					if( innerShots > 0 )
					{
						var innerShotSplit = RandomNumber( 0, 100 );
						if(( 30 + (( pUser.skills.archery / 100 ) * 2 )) > innerShotSplit )
						{
							iUsed.TextMessage( pUser.name +" splits an arrow in the innter ring!" );
							points = 20;
							var Innershot = 1;
						}
					}
					if( !Innershot == 1 )
					{
						iUsed.TextMessage( pUser.name +" hits the inner ring." );
						points = 10;
					}
				}
				else if( MiddleChance > iNum )
				{
					//Middle Ring
					var middleShots = iUsed.GetTag( "middleShots" );
					iUsed.SetTag( "middleShots", middleShots + 1 );
					if( middleShots > 0 )
					{
						var middleShotSplit = RandomNumber( 0, 100 );
						if(( 30 + (( pUser.skills.archery / 100 ) * 2 )) > middleShotSplit )
						{
							iUsed.TextMessage( pUser.name + " splits an arrow in the middle ring!" );
							points = 15;
							var Middleshot = 1;
						}
					}
					if( !Middleshot == 1 )
					{
						iUsed.TextMessage( pUser.name + " hits the middle ring." );
						points = 5;
					}
				}
				else
				{
					//Outer Ring
					var outerShots = iUsed.GetTag( "outerShots" );
					iUsed.SetTag( "outerShots", outerShots + 1 );
					if( outerShots > 0 )
					{
						var outerShotSplit = RandomNumber( 0, 100 );
						if(( 30 + (( pUser.skills.archery / 100 ) * 2 )) > outerShotSplit )
						{
							iUsed.TextMessage( pUser.name + " splits an arrow in the outer ring!" );
							points = 5;
							var Outershot = 1;
						}
					}
					if( !Outershot == 1 )
					{
						iUsed.TextMessage( pUser.name + " hits the outer ring." );
						var points = 2;
					}
				}
			}
			var totalShots = iUsed.GetTag( "totalShots" ) + 1;
			iUsed.SetTag( "totalShots", totalShots );
			var totalScore = iUsed.GetTag( "totalScore" ) + points;
			iUsed.SetTag( "totalScore", totalScore );
			iUsed.TextMessage( "Score: "+totalScore+" after "+totalShots+" shots." );
			iUsed.SetTag( "lastUsedBy", ( pUser.serial & 0x00FFFFFF ) );
			var lastUsed = GetCurrentClock();
			iUsed.SetTag( "lastUsed", lastUsed );
		}
	}
	return false;
}