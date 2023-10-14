function onUseChecked( pUser, iUsed )
{
	var wellPlayed = pUser.CheckSkill( 29, 0, 1000 );
	var soundID;
	switch( iUsed.id )
	{
		case 0x0E9C:	// Drum
			if( wellPlayed )
			{
				soundID = 0x0038;
			}
			else
			{
				soundID = 0x0039;
			}
			break;
		case 0x0E9D:	// Tambourine
		case 0x0E9E:
			if( wellPlayed )
			{
				soundID = 0x0052;
			}
			else
			{
				soundID = 0x0053;
			}
			break;
		case 0x0EB1:	// Standing Harp
			if( wellPlayed )
			{
				soundID = 0x0043;
			}
			else
			{
				soundID = 0x0044;
			}
			break;
		case 0x0EB2:	// Harp
			if( wellPlayed )
			{
				soundID = 0x0045;
			}
			else
			{
				soundID = 0x0046;
			}
			break;
		case 0x0EB3:	// Lute
		case 0x0EB4:
			if( wellPlayed )
			{
				soundID = 0x004C;
			}
			else
			{
				soundID = 0x004D;
			}
			break;
		case 0x2805:	//Bamboo Flute
		case 0x2807:
			if( wellPlayed )
			{
				soundID = 0x504;
			}
			else
			{
				soundID = 0x503;
			}
			break;
		default:
			return false;
	}
	pUser.SoundEffect( soundID, true );
	return false;
}

