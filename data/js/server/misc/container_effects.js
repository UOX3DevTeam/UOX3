function onUseChecked( pUser, iUsed )
{
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		Open( iUsed );
	}
	return true;
}

function Open( iUsed )
{
	var iDice = RandomNumber( 1, 2 );
	switch( iUsed.id )
	{
		case 0x0A4D:
			iUsed.id = 0x0A4C;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0x0A4F:
			iUsed.id = 0x0A4E;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0x0A51:
			iUsed.id = 0x0A50;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0x0A53:
			iUsed.id = 0x0A52;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0x0A2C:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A2D, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A2E, 1, 18, false );
			}
			break;
		case 0x0A34:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A35, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A36, 1, 18, false );
			}
			break;
		case 0x0A30:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A31, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A32, 1, 18, false );
			}
			break;
		case 0x0A38:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A39, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A3A, 1, 18, false );
			}
			break;
		case 0x0A3C:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A3E, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A3F, 1, 18, false );
			}
			break;
		case 0x0A3D:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A41, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A42, 1, 18, false );
			}
			break;
		case 0x0A44:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A46, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A47, 1, 18, false );
			}
			break;
		case 0x0A45:
			if( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A49, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A4A, 1, 18, false );
			}
			break;
	}
}

function onTimer( iUsed, timerID )
{
	switch( timerID )
	{
		case 1:
			switch( iUsed.id )
			{
				case 0x0A4C:
					iUsed.id = 0x0A4D;
					break;
				case 0x0A4E:
					iUsed.id = 0x0A4F;
					break;
				case 0x0A50:
					iUsed.id = 0x0A51;
					break;
				case 0x0A52:
					iUsed.id = 0x0A53;
					break;
			}
			break;
	}
}