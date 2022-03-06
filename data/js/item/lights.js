//Notes: in Custom UO Client You do not have to set a direction. direction is only used for OSI Clients.
const debugMsg = false; // Turns on message for player
const dblClck = true; // If true enables lampposts to be double clicked on and off.
const lightEnabled = false; // This enables and disables lamp post to change from on and off with light settings.
const brightLvl = 10; // This Setting is for how bright or dark it has to be for lamps to turn on and off. the Higher the number the Darker the Lower the Number the lighter.
const unequipEnabled = false; // This setting allows the touch and candle to be turned off automatic if unequiped.

function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;

	// Get root container for iUsed
	var iRootContainer = FindRootContainer( iUsed, 0 );
	if( !ValidateObject( iRootContainer ))
	{
		// No root container. iUsed is on the ground/floor!
		// Check if user is within range
		if( !iUsed.InRange( pUser, 2 ))
		{
	  		pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
			return false;
		}
	}
	else
	{
		// Root container found. Who does it belong to?
		if( iRootContainer != pUser.pack )
		{
			var ownerChar = GetPackOwner( iRootContainer, 0 );
			if( ValidateObject( ownerChar ))
			{
				// It's in someone else's backpack
				//no msgs keeps player from turning it on in some one elses backpack when snooping.
				return false;
			}
			else if( !iRootContainer.InRange( pUser, 2 ))
			{
				// Check distance to container holding the iUsed
			  	pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
				return false;
			}
		}
	}

	switch ( iUsed.id )
	{
		case 0x0A27: //small candelabra id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 2;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0B1D;
			break;
		case 0x0B1D: //small candelabra id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A27;
			break;
		case 0x0A29: //large candelabra id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 29;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0B26;
			break;
		case 0x0B26: //large candelabra id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A29;
			break;
		case 0x09FB: //E, wall sconce id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 14;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			};
			iUsed.id = 0x09FD;
			break;
		case 0x09FD: //E, wall sconce id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x09FB;
			break;
		case 0x0A00: //S, wall sconce id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 11;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A02;
			break;
		case 0x0A02: //S, wall sconce id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A00;
			break;
		case 0x0A26: //candlestick candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 2;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0B1A;
			break;
		case 0x0B1A: //candlestick candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A26;
			break;
		case 0x0A28: //player carryable candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 2;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A0F;
			break;
		case 0x0A0F: //player carryable candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A28;
			break;
		case 0x1853: //right skull with candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x1854;
			break;
		case 0x1854: //right skull with candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x1853;
			break;
		case 0x1857: //left skull with candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x1858;
			break;
		case 0x1858: //left skull with candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x1857;
			break;
		case 0x142F: //thick candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 2;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x142C;
			break;
		case 0x142C: //thick candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x142F;
			break;
		case 0x1433: //middle candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 2;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x1430;
			break;
		case 0x1430: //middle candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			pUser.TextMessage("I am turning off");
			iUsed.id = 0x1433;
			break;
		case 0x1437: //slim candle id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 2;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x1434;
			break;
		case 0x1434: //slim candle id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x1437;
			break;
		case 0x1849: //SW heating stand id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x184A;
			break;
		case 0x184A: //SW heating stand id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x1849;
			break;
		case 0x1849: //SW heating stand id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x184A;
			break;
		case 0x184A: //SW heating stand id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x1849;
			break;
		case 0x184D: //NE heating stand id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x184E;
			break;
		case 0x184E: //NE heating stand id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x184D;
			break;
		case 0x0B21: //simple lamp post id unlit
			if ( dblClck == true )
			{
				pUser.SoundEffect( 0x03BE, true );
				iUsed.dir = 29;
				if ( debugMsg == true )
				{
					pUser.TextMessage( "I am turning on" );
				}
				iUsed.id = 0x0B20;
				break;
			}
			break;
		case 0x0B20: //simple lamp post id lit
			if (dblClck == true) 
			{
				pUser.SoundEffect( 0x0047, true );
				iUsed.dir = 99;
				if ( debugMsg == true )
				{
					pUser.TextMessage( "I am turning off" );
				}
				iUsed.id = 0x0B21;
				break;
			}
			break;
		case 0x0B23: //glass lamp post id unlit
			if ( dblClck == true )
			{
				pUser.SoundEffect( 0x03BE, true );
				iUsed.dir = 29;
				if ( debugMsg == true )
				{
					pUser.TextMessage( "I am turning on" );
				}
				iUsed.id = 0x0B22;
				break;
			}
			break;
		case 0x0B22: //glass lamp post id lit
			if (dblClck == true)
			{
				pUser.SoundEffect( 0x0047, true );
				iUsed.dir = 99;
				if (debugMsg == true)
				{
					pUser.TextMessage( "I am turning off" );
				}
				iUsed.id = 0x0B23;
				break;
			}
			break;
		case 0x0B25: //fancy lamp post id unlit
			if (dblClck == true)
			{
				pUser.SoundEffect( 0x03BE, true );
				iUsed.dir = 29;
				if ( debugMsg == true )
				{
					pUser.TextMessage( "I am turning on" );
				}
				iUsed.id = 0x0B24;
				break;
			}
			break;
		case 0x0B24: //fancy lamp post id lit
			if (dblClck == true)
			{
				pUser.SoundEffect( 0x0047, true );
				iUsed.dir = 99;
				if ( debugMsg == true )
				{
					pUser.TextMessage( "I am turning off" );
				}
				iUsed.id = 0x0B25;
				break;
			}
			break;
		case 0x0A18: //E, hanging lantern id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 8;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A15;
			break;
		case 0x0A15: //E, hanging lantern id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A18;
			break;
		case 0x0A1D: //S, hanging lantern id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 5;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A1A;
			break;
		case 0x0A1A: //S, hanging lantern id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A1D;
			break;
		case 0x0A25: //player carryable lantern id unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A22;
			break;
		case 0x0A22: //player carryable lantern id lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A25;
			break;
		case 0x0A05: //E, wall torch unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 8;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A07;
			break;
		case 0x0A05: //E, wall torch lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A25;
			break;
		case 0x0A0A: //S, wall torch unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 5;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A0C;
			break;
		case 0x0A0C: //S, wall torch lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0A0A;
			break;
		case 0x0F64: //player carryable torch unlit
			pUser.SoundEffect( 0x03BE, true );
			iUsed.dir = 1;
			if ( debugMsg == true )
			{
				pUser.TextMessage( "I am turning on" );
			}
			iUsed.id = 0x0A12;
			break;
		case 0x0A12: //player carryable torch lit
			pUser.SoundEffect( 0x0047, true );
			iUsed.dir = 99;
			if ( debugMsg == true ) 
			{
				pUser.TextMessage( "I am turning off" );
			}
			iUsed.id = 0x0F64;
			break;
	}
	return false;
}

function onLightChange(iLight, lightLevel)
{
	if( ValidateObject( iLight ) && iLight.isItem && lightEnabled == true )
	{
		switch (iLight.id) 
		{
			case 0x0B21: //simple lamp post id unlit
				if ( lightLevel > brightLvl ) 
				{
					iLight.dir = 29;
					iLight.id = 0x0B20;
					break;
				}
			case 0x0B20: //simple lamp post id lit
				if ( lightLevel < brightLvl )
				{
					iLight.dir = 99;
					iLight.id = 0x0B21;
					break;
				}
				break;
			case 0x0B23: //glass lamp post id unlit
				if ( lightLevel > brightLvl ) 
				{
					iLight.dir = 29;
					iLight.id = 0x0B22;
					break;
				}
				break;
			case 0x0B22: //glass lamp post id lit
				if ( lightLevel < brightLvl )
				{
					iLight.dir = 99;
					iLight.id = 0x0B23;
					break;
				}
				break;
			case 0x0B25: //fancy lamp post id 
				if ( lightLevel > brightLvl )
				{
					iLight.dir = 29;
					iLight.id = 0x0B24;
					break;
				}
				break;
			case 0x0B24: //fancy lamp post id lit
				if ( lightLevel < brightLvl )
				{
					iLight.dir = 99;
					iLight.id = 0x0B25;
					break;
				}
				break;
		}
	}
	return false;
}

function onUnequip( pEquipper, iEquipped )
{
	if ( ValidateObject( iEquipped ) && iEquipped.isItem && unequipEnabled == true )
	{
		switch (iEquipped.id)
		{
			case 0x0A12: //player carryable torch lit
				pEquipper.SoundEffect ( 0x0047, true );
				iEquipped.dir = 99;
				if ( debugMsg == true ) 
				{
					pEquipper.TextMessage( "I am turning off" );
				}
				iEquipped.id = 0x0F64;
				break;
			case 0x0A0F: //player carryable candle id lit
				pEquipper.SoundEffect( 0x0047, true );
				iEquipped.dir = 99;
				if ( debugMsg == true )
				{
					pEquipper.TextMessage( "I am turning off" );
				}
				iEquipped.id = 0x0A28;
				break;
			case 0x0A22: //player carryable lantern id lit
				pEquipper.SoundEffect( 0x0047, true );
				iEquipped.dir = 99;
				if ( debugMsg == true )
				{
					pEquipper.TextMessage( "I am turning off" );
				}
				iEquipped.id = 0x0A25;
				break;
		}
	}
	return false;
}