function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;

	var CampingAttempt = pUser.skills.camping / 10;
	var Success = RandomNumber( 0, 100 );	
	if( pUser.CheckSkill( 10, 0, 1000 ))
	{
	   	if( iUsed.container != null && iUsed.container.serial == pUser.pack.serial ) 
	   	{
			iUsed.container = null;
			iUsed.Teleport( pUser.x, pUser.y, pUser.z, pUser.worldnumber );
		}
		else
		{
			if( !iUsed.InRange( pUser, 2 ) )
			{
				pUser.SysMessage( GetDictionaryEntry( 482, pSock.Language )); //You need to be closer to use that.
				return false;
			}
		}
		iUsed.movable = 2;
		iUsed.id = 0x0de3;
		iUsed.dir = 29; //Set light radius to 29
		pUser.SysMessage( GetDictionaryEntry( 1765, pSock.Language ));
		iUsed.StartTimer( 60000, 1, true ); //Campfire burns for 60 seconds
	}
	else
		pUser.SysMessage( GetDictionaryEntry( 1764, pSock.Language ));
	return false;
}

function onTimer( campfire, timerID )
{
	if( timerID == 1 )
	{
		campfire.id = 0x0de9; // turn campfire into embers
		campfire.dir = 2; //Set light radius to 2
		campfire.StartTimer( 15000, 2, true ); //Embers last for 15 seconds
	}
	if( timerID == 2 )
	{
		campfire.id = 0x0dea; // turn embers into burnt wood
		campfire.decayable = true;
	}
}

function onCollide( pSock, pUser, iUsed )
{
	if( iUsed.id == 0x0de3 )
	{
		pUser.SysMessage( "Ouch!" );
		pUser.Damage( 2 );
	}
}