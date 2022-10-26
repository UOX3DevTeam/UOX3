function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;

	//Check if user is in range of pickpocket dip
	if( !iUsed.InRange( pUser, 1 ) )
	{
		pUser.SysMessage( GetDictionaryEntry( 482, pSock.language )); //You need to be closer to use that.
		return false;
	}
	if( iUsed.id == 0x1E2C )
	{
		iUsed.id = 0x1EC0; //Convert useless dummy to useful dummy
	}
	else if( iUsed.id == 0x1E2D )
	{
		iUsed.id = 0x1EC3; //convert useless dummy to useful dummy
	}
	else if( iUsed.id == 0x1EC0 || iUsed.id == 0x1EC3 ) //if pickpocket dip is motionless
	{
		//Check if the pickpocket dip is already in use (to delay how often one can use it)
		if( iUsed.GetTag( "inUse" ) > 0 )
		{
			//Safety measure in case timer ever breaks.
			safetyMeasure( iUsed );
			pUser.SysMessage( GetDictionaryEntry( 1762, pSock.language )); //You must wait before you can use that item again.
			return false;
		}

		//Check if character is mounted or not, and then call up an external script to determine combat animations
		if( pUser.isonhorse )
		{
			pUser.SysMessage( GetDictionaryEntry( 1757, pSock.language )); //Please dismount first.
			return false;
		}

		//Check the player's tactics skill to see if he gets chance to gain more skill
		if( pUser.skills.stealing > 250 )
		{
			pUser.SysMessage( GetDictionaryEntry( 1758, pSock.language )); //Your ability to steal cannot improve any further by simply practicing on a dummy.
		}
		else
		{
			if( pUser.CheckSkill( 33, 0, 250 ))
			{
				pUser.SysMessage( GetDictionaryEntry( 1760, pSock.language )); //You successfully avoid disturbing the dip while searching it.
				pUser.SoundEffect( 0x4F, true );
				iUsed.SetTag( "inUse", 1 );
				iUsed.StartTimer( 2500, 2, true );
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 1759, pSock.language )); //You carelessly bump the dip and start it swinging.
				pUser.SoundEffect( 0x390, true );
				iUsed.id++;
				//Star a timer so the pickpocket dip doesn't swing forever
				iUsed.StartTimer( 3000, 1, true );
			}
		}
	}
	else
	{
		//Safety measure in case timer ever breaks.
		SafetyMeasure( iUsed );
		pUser.SysMessage( GetDictionaryEntry( 483, pSock.language )); //You must wait for it to stop swinging!
	}
	return false;
}

function SafetyMeasure( iUsed )
{
	var failedToUse = iUsed.GetTag( "failedToUse" );

	//Check if 4 or more failed attempts have been made
	if( failedToUse > 3 )
	{
		StopDummy( iUsed );
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
	//If timer is 1, stop the swinging pickpocket dip
	if( timerID == 1 )
	{
		// Let's call the stopDummy function!
		stopDummy( iUsed );
	}
	if( timerID == 2 )
	{
		//player stole from dummy successfully, so it didn't move. Reset it.
		iUsed.SetTag( "inUse", null );
		iUsed.SetTag( "failedToUse", 0 );
	}
}

function StopDummy( iUsed )
{
	if( iUsed )
	{
		if( iUsed.id == 0x1ec1 || iUsed.id == 0x1ec2 )
		{
			iUsed.id = 0x1ec0; //stop dummy from moving
		}
		else if( iUsed.id == 0x1ec4 || iUsed.id == 0x1ec5 )
		{
			iUsed.id = 0x1ec3; //stop dummy from moving
		}
		iUsed.SetTag( "failedToUse", 0 ); 	//reset values on dummy
		iUsed.SetTag( "inUse", null ); 		//reset values on dummy
	}
}
