function onUseChecked( pUser, iUsed )
{
	var itemOwner = GetPackOwner( iUsed, 0 );
	if ( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		Open( iUsed );
		return true;
	}
	else
		return true;
}

function Open( iUsed )
{
	var iDice = RandomNumber( 1, 2 );
	switch ( iUsed.id )
	{
		case 0xA4D:
			iUsed.id = 0xA4C;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0x0A4F:
			iUsed.id = 0xA4E;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0xA51:
			iUsed.id = 0xA50;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0xA53:
			iUsed.id = 0xA52;
			iUsed.StartTimer( 10000, 1, true );
			break;
		case 0x0A2C:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A2D, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A2E, 1, 18, false );
			}
			break;
		case 0x0A34:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A35, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A36, 1, 18, false );
			}
			break;
		case 0x0A30:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A31, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A32, 1, 18, false );
			}
			break;
		case 0x0A38:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A39, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 2, 0x0A3A, 1, 18, false );
			}
			break;
		case 0x0A3C:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A3E, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A3F, 1, 18, false );
			}
			break;
		case 0x0A3D:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A41, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A42, 1, 18, false );
			}
			break;
		case 0x0A44:
			if ( iDice == 1 )
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A46, 1, 18, false );
			}
			else
			{
				DoStaticEffect( iUsed.x, iUsed.y, iUsed.z + 1, 0x0A47, 1, 18, false );
			}
			break;
		case 0x0A45:
			if ( iDice == 1 )
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
	switch ( timerID )
	{
		case 1:
			switch ( iUsed.id )
			{
				case 0xA4C:
					iUsed.id = 0xA4D;
					break;
				case 0xA4E:
					iUsed.id = 0xA4F;
					break;
				case 0xA50:
					iUsed.id = 0xA51;
					break;
				case 0xA52:
					iUsed.id = 0xA53;
					break;
			}
			break;
	}
}