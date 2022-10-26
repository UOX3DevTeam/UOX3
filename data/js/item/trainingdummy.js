// Settings
// If true, UOX3 will attempt to automatically make training dummies damageable if they're not already
const damageableDummies = true;

function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	//Check if user is in range of combat dummy
	if( !iUsed.InRange( pUser, 1 ))
	{
		pUser.SysMessage( GetDictionaryEntry( 482, pSock.language )); //You need to be closer to use that.
		return false;
	}
	else if( iUsed.id == 0x1070 || iUsed.id == 0x1074 ) //if training dummy is motionless
	{
		//Determine weapon-type by calling external script and loading a value set there afterwards
		var weaponType = TriggerEvent( 2500, "GetWeaponType", pUser, null );
		if( weaponType == "BOWS" || weaponType == "XBOWS" )
		{
			pUser.SysMessage( GetDictionaryEntry( 938, pSock.language )); //Practice archery on archery buttes!
			return false;
		}

		//Check if character is mounted or not, and then call up an external script to determine combat animations
		var combatAnim = 0;
		if( pUser.isonhorse )
		{
			combatAnim = TriggerEvent( 2501, "GetHorseCombatAnim", pUser, weaponType );
		}
		else
		{
			combatAnim = TriggerEvent( 2501, "GetFootCombatAnim", pUser, weaponType );
		}

		if( !isNaN( combatAnim ) && combatAnim != 0 )
		{
			pUser.DoAction( combatAnim );
		}

		//Play some random sound effects when the training dummy is hit
		switch( RandomNumber( 0, 2 ) )
		{
			case 0: pUser.SoundEffect( 0x013B, true );	break;
			case 1: pUser.SoundEffect( 0x013C, true );	break;
			case 2: pUser.SoundEffect( 0x013D, true );	break;
			default:
				return false;
		}

		//Change the motionless dummy to a swinging dummy!
		if( iUsed.id == 0x1070 )
		{
			iUsed.id++;
		}
		else if( iUsed.id == 0x1074 )
		{
			iUsed.id++;
		}

		//Check the player's tactics skill to see if he gets chance to gain more skill
		if( pUser.skills.tactics > 250 )
		{
			pUser.SysMessage( GetDictionaryEntry( 939, pSock.language )); //You feel you would gain no more from using that.
		}
		else
		{
			pUser.CheckSkill( 27, 0, 250 );
		}

		// Automatically set training dummies as damageable if they're not already
		if( damageableDummies && !iUsed.isDamageable )
		{
			iUsed.isDamageable = 1;

			if( iUsed.maxhp == 0 )
			{
				iUsed.maxhp = 100;
				iUsed.health = 100;
			}
		}

		// If the training dummy is marked as damageable,
		// handle damage numbers and health bar updates
		if( iUsed.isDamageable )
		{
			// Get random attack value for character
			var dummyRandomDamage = pUser.attack;

			// Create and send a custom packet showing damage over training dummy
		    var myPacket = new Packet; // Create new packet stream
		    myPacket.ReserveSize( 7 ); // Reserve packet size of 7, which is optimal for packet 0x0B
		    myPacket.WriteByte( 0, 0x0B ); // Write packetID (0x0B) at position 0
		    myPacket.WriteLong( 1, iUsed.serial ); // Write character serial at position 1 (0+WriteByte, or 0+1)
		    myPacket.WriteShort( 5, dummyRandomDamage ); // Write damage number at position 5 (1+WriteLong, or 1+4)
		    pSock.Send( myPacket ); // Send stream to socket
		    myPacket.Free(); // Free up stream

		    // Update the training dummy's health
		    if( iUsed.health > dummyRandomDamage )
		    {
		    	iUsed.health = iUsed.health - dummyRandomDamage;
		    }
		    else
		    {
		    	iUsed.health = 1;
		    }

		    // Refresh the dummy's health bar nearby players
		    iUsed.UpdateStats( 0 );
		}

		// Kill existing timers on training dummy, just in case
	    iUsed.KillTimers();

		//Start a timer so the dummy doesn't swing forever
		iUsed.StartTimer( 3000, 1, true );
	}
	else
	{
		//Safety measure in case timer ever breaks and dummy never stops swinging
		safetyMeasure( iUsed );

		pUser.SysMessage( GetDictionaryEntry( 483, pSock.language )); //You must wait for it to stop swinging!
	}
	return false;
}

function safetyMeasure( iUsed )
{
	var failedToUse = iUsed.GetTag( "failedToUse" );

	//Check if 4 or more failed attempts have been made
	if( failedToUse > 3 )
	{
		stopDummy( iUsed );
	}
	else
	{
		// Else, add to failed attempts
		failedToUse++;
		iUsed.SetTag( "failedToUse", failedToUse );
	}

}

function onTimer( iUsed, timerID )
{
	if( timerID == 1 )
	{
		//If timer is 1, stop the swinging dummy
		stopDummy( iUsed );

		if( iUsed.isDamageable )
		{
			iUsed.StartTimer( 4000, 2, true );
		}
	}
	if( timerID == 2 && iUsed.isDamageable )
	{
		// Restore training dummy health
		iUsed.health = iUsed.maxhp;

		 // Refresh the dummy's health bar nearby players
		iUsed.UpdateStats( 0 );
	}
}

function stopDummy( iUsed )
{
	if( iUsed )
	{
		if( iUsed.id == 0x1071 || iUsed.id == 0x1072 || iUsed.id == 0x1073 )
			iUsed.id = 0x1070;
		else if( iUsed.id == 0x1075 || iUsed.id == 0x1076 || iUsed.id == 0x1077 )
			iUsed.id = 0x1074;
		iUsed.SetTag( "failedToUse", 0 ); 	//reset values on dummy
	}
}