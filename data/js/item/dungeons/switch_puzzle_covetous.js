// Switch pattern: 3 - 4 - 1 - 5

function onUseChecked( pUser, iSwitch )
{
	// Flip the switch
	if( iSwitch.id == 0x1091 || iSwitch.id == 0x108f )
	{
		iSwitch.id++;
	}
	else
	{
		iSwitch.id--;
	}

	// Play a switch flipping sound
	iSwitch.SoundEffect( 0x004a, true );

	var switchNum = iSwitch.morex;
	var isNextSwitch = iSwitch.morey;
	switch( switchNum )
	{
		case 3:
			// Next switch is 4
			if( isNextSwitch && iSwitch.id == 0x1091 )
			{
				// Reset this switch
				isNextSwitch = 0;
				iSwitch.morez = 4; // next switch

				// Find switch 4 and set morey to 1
				AreaItemFunction( "FindNextSwitch", iSwitch, 5 );
			}
			else
			{
				// Used out of sequence! Reset all switches
				AreaItemFunction( "ResetAllSwitches", iSwitch, 5 );
			}
			break;
		case 4:
			// Next switch is 1
			if( isNextSwitch && iSwitch.id == 0x1091 )
			{
				// Reset this switch
				isNextSwitch = 0;
				iSwitch.morez = 1; // next switch

				// Find switch 1 and set morey to 1
				AreaItemFunction( "FindNextSwitch", iSwitch, 5 );
			}
			else
			{
				// Used out of sequence! Reset all switches
				AreaItemFunction( "ResetAllSwitches", iSwitch, 5 );
			}
			break;
		case 1:
			// Next switch is 5
			if( isNextSwitch && iSwitch.id == 0x1091 )
			{
				// Reset this switch
				isNextSwitch = 0;
				iSwitch.morez = 5; // next switch

				// Find switch 1 and set morey to 1
				AreaItemFunction( "FindNextSwitch", iSwitch, 5 );
			}
			else
			{
				// Used out of sequence! Reset all switches
				AreaItemFunction( "ResetAllSwitches", iSwitch, 5 );
			}
			break;
		case 5:
			// Final switch
			if( isNextSwitch && iSwitch.id == 0x1091 )
			{
				var iTime = GetCurrentClock();
				var lastSpawnTime = iSwitch.GetTempTag( "lastSpawnTime" );

				if( lastSpawnTime && (( iTime - lastSpawnTime < 60000 )))
				{
					// Too short amount of time since last spawn! Reset all switches
					AreaItemFunction( "ResetAllSwitches", iSwitch, 5 );
					return;
				}

				// Store a timestamp for when last spawn took place
				iSwitch.SetTempTag( "lastSpawnTime", iTime );

				// Reset this switch
				isNextSwitch = 0;

				// Spawn a reward lich lord!
				var lichLord = SpawnNPC( "lichlord", 5431, 1876, 0, pUser.worldnumber, pUser.instanceID );
			}
			else
			{
				// Used out of sequence! Reset all switches
				AreaItemFunction( "ResetAllSwitches", iSwitch, 5 );
			}
			break;
		case 10:
			if( iSwitch.id == 0x108e )
			{

			}
			break;
		case 11:
			if( iSwitch.id == 0x1091 )
			{

			}
		default:
			// Switch 2 is always wrong. Reset all switches!
			AreaItemFunction( "ResetAllSwitches", iSwitch, 5 );
			break;
	}
}

function FindNextSwitch( srcItem, trgItem )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return;

	var nextSwitchNum = srcItem.morez;
	var switchNum = trgItem.morex;
	if( switchNum == nextSwitchNum )
	{
		trgItem.morey = 1;
	}
}

function ResetAllSwitches( srcItem, trgItem )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return;

	var switchNum = trgItem.morex;
	if( switchNum == 3 )
	{
		trgItem.morey = 1; // First switch
	}
	else
	{
		trgItem.morey = 0;
	}
}