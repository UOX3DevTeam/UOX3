function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	
	//Check if user is in range of pickpocket dip
	if( !iUsed.InRange( pUser, 1 ) )
	{
		pUser.SysMessage( GetDictionaryEntry( 482, pSock.Language )); //You need to be closer to use that.
		return false;
	}
	else if( iUsed.id == 0x1EC3 || iUsed.id == 0x1EC0 ) //if pickpocket dip is motionless
	{
		//Check if the pickpocket dip is already in use (to delay how often one can use it)
		if( iUsed.GetTag( "inUse" ) > 0 )
		{
			pUser.SysMessage( GetDictionaryEntry( 1762, pSock.Language )); //You must wait before you can use that item again.
			return false;
		}
		
		//Check if character is mounted or not, and then call up an external script to determine combat animations
		if( pUser.isonhorse )
		{
			pUser.SysMessage( GetDictionaryEntry( 1757, pSock.Language )); //Please dismount first.
			return false;
		}

		//Check the player's tactics skill to see if he gets chance to gain more skill
		if( pUser.skills.stealing > 250 )
			pUser.SysMessage( GetDictionaryEntry( 1758, pSock.Language )); //Your ability to steal cannot improve any further by simply practicing on a dummy.
		else
		{
			if( pUser.CheckSkill( 33, 0, 250 ))
			{
				pUser.SysMessage( GetDictionaryEntry( 1760, pSock.Language )); //You successfully avoid disturbing the dip while searching it.
				pUser.SoundEffect( 0x4F, true );
				iUsed.SetTag( "inUse", 1 );
				iUsed.StartTimer( 2500, 2, true );
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 1759, pSock.Language )); //You carelessly bump the dip and start it swinging.
				pUser.SoundEffect( 0x390, true );
				iUsed.id++;
				//Star a timer so the pickpocket dip doesn't swing forever
				iUsed.StartTimer( 3000, 1, true );
			}
		}
	}
	else
		pUser.SysMessage( GetDictionaryEntry( 483, pSock.Language )); //You must wait for it to stop swinging!
	return false;
}

function onTimer( iUsed, timerID )
{
	//If timer is 1, stop the swinging pickpocket dip
	if( timerID == 1 )
		iUsed.id--;
	if( timerID == 2 )
		iUsed.SetTag( "inUse", null );	
}
