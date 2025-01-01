// Snooping - called when player snoops another PC/NPC's or tamed animal's pack

const maxSnoopDistance = 1; // Determines the max distance at which snooping is possible
const snoopAwarenessDecayTimer = 30000; // (30 seconds) Determine how long it takes for snoopAwareness to decay one level

// Always fetch these settings, in case they've been updated in ini
function FetchServerSettings()
{
	// Core shard era as defined in ini
	coreShardEra = GetServerSetting( "CoreShardEra" );

	// If enabled, a target of snooping becomes more aware of repeat snooping attempts
	// This increases the minimum difficulty for the snooping skillcheck chance:
	// minDifficulty *= 1 + ( awarenessValue / 10 )
	snoopAwareness = GetServerSetting( "SnoopAwareness" );

	// If enabled, young players cannot snoop backpacks of other players, nor can they themselves be "snooped"
	youngPlayerSystem = GetServerSetting( "YoungPlayerSystem" );
}

// Called from global script via onSnoopAttempt() JS event
function SnoopAttempt( snoopTarget, targCont, pUser )
{
	if( !ValidateObject( snoopTarget ) || !ValidateObject( targCont ) || !ValidateObject( pUser ))
		return false;

	var targSock = snoopTarget.socket;
	var pSock = pUser.socket;
	if( pSock == null )
		return false;

	var rootCont = FindRootContainer( targCont );
	if( !ValidateObject( rootCont ) || rootCont == pUser.pack )
		return false;

	// Fetch up-to-date settings from uox.ini
	FetchServerSettings();

	// Always allow those with higher commandlevel to snoop the packs of those with lower commandlevel
	var minDifficulty = 0;
	if( pUser.commandlevel <= snoopTarget.commandlevel )
	{
		if( !GeneralSnoopingChecks( pUser, snoopTarget, pSock, targSock, targCont ))
		{
			return false;
		}

		// Count how many containers deep the container pUser is trying to snoop sits at.
		// The deeper it sits, the more difficult the skill check to snoop it becomes
		minDifficulty = GetSnoopDifficulty( targCont );

		// Add penalty for target's snooping awareness
		if( snoopAwareness )
		{
			var snoopAwareness = parseInt( snoopTarget.GetTempTag( "snoopAware" ));
			if( snoopAwareness > 1 )
			{
				minDifficulty = Math.max( 100, minDifficulty ) * ( snoopAwareness / 4 );
			}
		}

		if( minDifficulty > 1000 )
		{
			// Cap difficulty at 1000 regardless of how deep the containers go
			minDifficulty = 1000;
		}
	}

	var targScriptTriggers = snoopTarget.scriptTriggers;
	if( pUser.commandlevel > snoopTarget.commandlevel || pUser.CheckSkill( 28, minDifficulty, 1000 ))
	{
		// Successful snoop!
		// Open targCont for player
		pSock.OpenContainer( targCont );

		if( pUser.commandlevel > snoopTarget.commandlevel )
		{
			// No need to proceed further, command privileges battle was won!
			return;
		}

		pSock.SysMessage( GetDictionaryEntry( 993, pSock.language )); // You successfully peek into that container.

		for( var i = 0; i < targScriptTriggers.length; i++ )
		{
			if( DoesEventExist( targScriptTriggers[i], "onSnooped" ))
			{
				// If script returns true/1, prevent other scripts with event from running
				if( TriggerEvent( targScriptTriggers[i], "onSnooped", pUser, snoopTarget, true ) == 1 )
				{
					break;
				}
			}
		}

		// Increase target's snooping awareness slightly
		if( snoopAwareness && RandomNumber( 1, 10 ) == 1) // 10% chance to increase
		{
			IncreaseSnoopingAwareness( snoopTarget, 1 );
		}
	}
	else
	{
		// Failed snoop...
		// Increase target's snooping awareness moderately
		if( snoopAwareness && RandomNumber( 1, 10 ) <= 5 ) // 50% chance to increase
		{
			IncreaseSnoopingAwareness( snoopTarget, 2 );
		}

		var doNormal = true;
		for( var i = 0; i < targScriptTriggers.length; i++ )
		{
			if( DoesEventExist( targScriptTriggers[i], "onSnooped" ))
			{
				// If script returns true/1, prevent other scripts with event from running
				if( TriggerEvent( targScriptTriggers[i], "onSnooped", pUser, snoopTarget, false ) == 1 )
				{
					doNormal = false;
				}
			}
		}

		if( doNormal )
		{
			pSock.SysMessage( GetDictionaryEntry( 991, pSock.language )); // You failed to peek into that container.

			// Check if snooping player gets detected
			var snoopingDetected = false;
			if( EraStringToNum( coreShardEra ) >= EraStringToNum( "t2a" ))
			{
				if( EraStringToNum( coreShardEra ) >= EraStringToNum( "uor" ))
				{
					// From UOR onwards (Publish 7), player has a chance to fail at snooping and still remain hidden
					// based on their Hiding skill. The higher their skill, the lower the chance to be revealed:
					// 	At 0 hiding skill, 100% chance to be revealed
					// 	At 100 hiding skill, 50% chance to be revealed
					var detectChance = 1000 - Math.floor( pUser.skills.hiding / 2 ); // Reveal chance starts at 50% of max hiding skill
					if( RandomNumber( 0, 1000 ) > detectChance )
					{
						snoopingDetected = true;
					}
				}
				else
				{
					// Snooping reveals player by default in T2A (Publish from March 28, 1999)
					snoopingDetected = true;
				}
			}

			if( snoopingDetected )
			{
				// Reveal player
				pUser.visible = 0;

				if( snoopTarget.npc )
				{
					if( snoopTarget.isHuman && snoopTarget.aitype != 2 && snoopTarget.aitype != 11 && snoopTarget.aitype != 666 )
					{
						if( GetServerSetting( "SnoopIsCrime" ) && RandomNumber( 0, 1 ) == 1 && pUser.criminal )	//	50% chance of calling guards
						{
							// 9213=Thou'rt scum! Guards!
							// 9214=Guards! A villain!
							// 9215=Tis a villain! Guards!
							// 9216=Guards! help!
							snoopTarget.TextMessage( GetDictionaryEntry( 9213 + RandomNumber( 0, 3), pSock.language ));
						}
						else
						{
							// 994=Art thou attempting to disturb my privacy?
							// 995=Stop that!
							// 996=Be aware I am going to call the guards!
							snoopTarget.TextMessage( GetDictionaryEntry( 994 + RandomNumber( 0, 2 ), pSock.language ));
						}
					}
					else
					{
						// Using optional creatureSoundNum param of SoundEffect(), play the creature's idle sound (if defined) from creatures.dfn
						snoopTarget.SoundEffect( 0, true, 1 );
					}
				}
				else if( targSock )
				{
					var tempSnoopMsg = GetDictionaryEntry( 997, pSock.language ); // You notice %s trying to peek into your pack!
					targSock.SysMessage( tempSnoopMsg.replace( /%s/gi, pUser.name ));
				}

				if( GetServerSetting( "SnoopIsCrime" ))
				{
					pUser.criminal = true;
				}
			}
		}
	}

	// Lower player's karma, but only down to -3 tier
	var karmaChange = 0;
	var karmaLossThreshold = -3000;
	var curKarma = pUser.karma;
	if( curKarma > karmaLossThreshold )
	{
		karmaChange = Math.abs( Math.round(( curKarma - karmaLossThreshold ) / 200 ));
		pUser.karma -= karmaChange;

		if( karmaChange != 0 )
		{
			if( karmaChange <= 25 )
			{
				pSock.SysMessage( GetDictionaryEntry( 1368, pSock.language )); // You have lost a little karma.
			}
			else if( karmaChange <= 50 )
			{
				pSock.SysMessage( GetDictionaryEntry( 1370, pSock.language )); // You have lost some karma.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1372, pSock.language )); // You have lost a lot of karma.
			}
		}
	}
}

function GeneralSnoopingChecks( pUser, snoopTarget, pSock, targSock, targCont )
{
	if( snoopTarget.commandlevel > pUser.commandlevel )
	{
		pSock.SysMessage( GetDictionaryEntry( 991, pSock.language )); // You failed to peek into that container.
		// This message is too annoying... players can pester GMs ;)
		/*if( targSock != null )
		{
			var tempMsg = GetDictionaryEntry( 992, targSock.language ); // % is snooping you!
			targSock.SysMessage( tempMsg.replace( /%s/gi, pUser.name ));
		}*/
		return false;
	}

	if( youngPlayerSystem )
	{
		if( pUser.account.isYoung && !snoopTarget.npc )
		{
			pSock.SysMessage( GetDictionaryEntry( 18729, pSock.language )); // Young players cannot perform hostile actions against other players.
			return false;
		}
		else if( snoopTarget.account.isYoung )
		{
			pSock.SysMessage( GetDictionaryEntry( 18730, pSock.language )); // Young players cannot be targets of hostile actions.
			return false;
		}
	}

	if( pUser.region.isSafeZone || snoopTarget.region.isSafeZone )
	{
		pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
		return false;
	}

	if( pUser.DistanceTo( snoopTarget ) > maxSnoopDistance )
	{
		pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
		return false;
	}

	// Don't allow snooping pack animals from inside a house, if the animals are outside
	if( snoopTarget.sectionID == "packhorse" || snoopTarget.sectionID == "packllama" || snoopTarget.sectionID == "giantbeetle" )
	{
		var pMulti = pUser.multi;
		if( ValidateObject( pMulti ))
		{
			var targMulti = snoopTarget.multi;
			if( !ValidateObject( targMulti ) || targMulti != pMulti )
			{
				// Not allowed, player is inside a multi, and pack animal is either in another multi or not in a multi at all
				return false;
			}
		}
	}

	var pScriptTriggers = pUser.scriptTriggers;
	for( var j = 0; j < pScriptTriggers.length; j++ )
	{
		if( DoesEventExist( pScriptTriggers[j], "onSnoopAttempt" ))
		{
			if( !TriggerEvent( pScriptTriggers[j], "onSnoopAttempt", snoopTarget, targCont, pUser ))
			{
				return false;
			}
		}
	}

	return true;
}

function IncreaseSnoopingAwareness( snoopTarget, incValue )
{
	var oldSnoopAwarenessVal = parseFloat( snoopTarget.GetTempTag( "snoopAware" ));
	if( oldSnoopAwarenessVal < 20 )
	{
		snoopTarget.SetTempTag( "snoopAware", ( oldSnoopAwarenessVal + incValue ).toString() );
		var snoopTimer = snoopTarget.GetJSTimer( 1, 4055 );

		if( snoopTimer > 0 )
		{
			// Update existing snoopTimer - reset it to the default time
			snoopTarget.SetJSTimer( 1, snoopAwarenessDecayTimer, 4055 );
		}
		else
		{
			// Start a new snoopTimer with default duration
			snoopTarget.StartTimer( snoopAwarenessDecayTimer, 1, true );
		}
	}
}

// Gets difficulty of snooping a container - the deeper the container sits
// inside main backpack, the more difficult it becomes
function GetSnoopDifficulty( targCont )
{
	var depth = 0;
	var maxDifficulty = 1000;
	var parentContainer = targCont.container;

	// Iterate through parent containers to determine depth and adjust difficulty
	while( parentContainer != null && parentContainer.isItem )
	{
		depth++;
		parentContainer = parentContainer.container;
	}

	// Calculate the minimum difficulty based on depth and max difficulty
	var minDifficulty = Math.floor(( depth / ( depth + 1 )) * maxDifficulty );

	return minDifficulty;
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	switch( timerID )
	{
		case 1: // Awareness timer has expired
			var snoopAwareness = parseFloat( timerObj.GetTempTag( "snoopAware" ));
			if( snoopAwareness > 1 )
			{
				// Reduce snooping awareness by half, and start the timer again
				timerObj.SetTempTag( "snoopAware", ( snoopAwareness / 2 ).toString() );
				timerObj.StartTimer( snoopAwarenessDecayTimer, 1, true );
			}
			else
			{
				timerObj.SetTempTag( "snoopAware", null );
			}
			break;
		default:
			break;
	}
}

function _restorecontext_() {}
