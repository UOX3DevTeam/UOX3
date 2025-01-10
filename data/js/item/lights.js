const dblClck = true; // If true enables lampposts to be double clicked on and off.
const dynamicLampPosts = true; // This enables and disables lamp post to change from on and off with light settings.
const brightLvl = 10; // This Setting is for how bright or dark it has to be for lamps to turn on and off. the Higher the number the Darker the Lower the Number the lighter.
const unequipEnabled = false; // This setting allows the touch and candle to be turned off automatic if unequiped.

function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	var soundObject = iUsed;
	var allHearSound = true;

	// Get root container for iUsed
	var iRootContainer = FindRootContainer( iUsed );
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

		soundObject = pUser;
		allHearSound = false;
	}

	// Lights are actually switched on below this switch statement
	var lightDir = 0;
	switch( iUsed.id )
	{
		case 0x0A27: //small candelabra id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			iUsed.id = 0x0B1D;
			lightDir = 2;
			break;
		case 0x0B1D: //small candelabra id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A27;
			break;
		case 0x0A29: //large candelabra id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 29;
			iUsed.id = 0x0B26;
			break;
		case 0x0B26: //large candelabra id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A29;
			break;
		case 0x09FB: //E, wall sconce id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 14;
			iUsed.id = 0x09FD;
			break;
		case 0x09FD: //E, wall sconce id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x09FB;
			break;
		case 0x0A00: //S, wall sconce id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 11;
			iUsed.id = 0x0A02;
			break;
		case 0x0A02: //S, wall sconce id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A00;
			break;
		case 0x0A26: //candlestick candle id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 2;
			iUsed.id = 0x0B1A;
			break;
		case 0x0B1A: //candlestick candle id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A26;
			break;
		case 0x0A28: //player carryable candle id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 2;
			iUsed.id = 0x0A0F;
			break;
		case 0x0A0F: //player carryable candle id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A28;
			break;
		case 0x1853: //right skull with candle id unlit
		case 0x1857: //left skull with candle id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id++;
			break;
		case 0x1854: //right skull with candle id lit
		case 0x1858: //left skull with candle id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id--;
			break;
		case 0x142F: //thick candle id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 2;
			iUsed.id = 0x142C;
			break;
		case 0x142C: //thick candle id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x142F;
			break;
		case 0x1433: //middle candle id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 2;
			iUsed.id = 0x1430;
			break;
		case 0x1430: //middle candle id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x1433;
			break;
		case 0x1437: //slim candle id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 2;
			iUsed.id = 0x1434;
			break;
		case 0x1434: //slim candle id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x1437;
			break;
		case 0x1849: //SW heating stand id unlit
		case 0x184D: //NE heating stand id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id++;
			break;
		case 0x184A: //SW heating stand id lit
		case 0x184E: //NE heating stand id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id--;
			break;
		case 0x0B21: //simple lamp post id unlit
		case 0x0B23: //glass lamp post id unlit
		case 0x0B25: //fancy lamp post id unlit
			if( dblClck == true )
			{
				soundObject.SoundEffect( 0x03BE, allHearSound );
				lightDir = 29;
				iUsed.id--;
				break;
			}
			break;
		case 0x0B20: //simple lamp post id lit
		case 0x0B22: //glass lamp post id lit
		case 0x0B24: //fancy lamp post id lit
			if( dblClck == true )
			{
				soundObject.SoundEffect( 0x0047, allHearSound );
				iUsed.morex = iUsed.dir;
				iUsed.dir = 99;
				iUsed.id++;
				break;
			}
			break;
		case 0x0A18: //E, hanging lantern id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 8;
			iUsed.id = 0x0A15;
			break;
		case 0x0A15: //E, hanging lantern id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A18;
			break;
		case 0x0A1D: //S, hanging lantern id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 5;
			iUsed.id = 0x0A1A;
			break;
		case 0x0A1A: //S, hanging lantern id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A1D;
			break;
		case 0x0A25: //player carryable lantern id unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id = 0x0A22;
			break;
		case 0x0A22: //player carryable lantern id lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A25;
			break;
		case 0x0A05: //E, wall torch unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 8;
			iUsed.id = 0x0A07;
			break;
		case 0x0A05: //E, wall torch lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A25;
			break;
		case 0x0A0A: //S, wall torch unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 5;
			iUsed.id = 0x0A0C;
			break;
		case 0x0A0C: //S, wall torch lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0A0A;
			break;
		case 0x0F64: //player carryable torch unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id = 0x0A12;
			break;
		case 0x0A12: //player carryable torch lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x0F64;
			break;
		case 0x3D97: //S, wall torch unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id = 0x3D94;
			break;
		case 0x3D94: //S, wall torch lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x3D97;
			break;
		case 0x3D9B: //E, wall torch unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id = 0x3D98;
			break;
		case 0x3D98: //E, wall torch lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x3D9B;
			break;
		//SE Lights
		case 0x24BC: //paper lantern brown unlit
		case 0x24BE: //paper lantern yellow unlit
		case 0x24C0: //lantern tall stone unlit
		case 0x24C2: //round hanging lantern red S unlit
		case 0x24C4: //round hanging lantern red E unlit
		case 0x24C6: //round hanging lantern white S unlit
		case 0x24C8: //round hanging lantern white E unlit
		case 0x24CA: //round table lantern unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 1;
			iUsed.id--;
			break;
		case 0x24BB: //paper lantern brown lit
		case 0x24BD: //paper lantern yellow lit
		case 0x24BF: //lantern tall stone lit
		case 0x24C1: //round hanging lantern red S lit
		case 0x24C3: //round hanging lantern red E lit
		case 0x24C5: //round hanging lantern white S lit
		case 0x24C7: //round hanging lantern white E lit
		case 0x24C9: //round table lantern lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id++;
			break;
		// ML Lights
		case 0x2CFD: //Large Glowing Lady Bug Red S unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 11;
			iUsed.id = 0x2CFE;
			break;
		case 0x2D00: //Large Glowing Lady Bug Red E unlit
		case 0x2D02: //Large Glowing Lady Bug Green table beetle E unlit
		case 0x2D04: //Large Glowing Lady Bug Green table beetle S unlit
			soundObject.SoundEffect( 0x03BE, allHearSound );
			lightDir = 11;
			iUsed.id--;
			break;
		case 0x2CFE: //Large Glowing Lady Bug Red S lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x2CFD;
			break;
		case 0x2CFF: //Large Glowing Lady Bug Red E lit
		case 0x2D01: //Large Glowing Lady Bug Green table beetle E lit
		case 0x2D03: //Large Glowing Lady Bug Green table beetle S lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id++;
			break;
		case 0x4C40: //Lamp Dragon S unlit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x4CDE;
			break;
		case 0x4C42: //Lamp Dragon E unlit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x4CE2;
			break;
		case 0x4C44: //Lamp B Bird S unlit
		case 0x4C46: //Lamp B Bird E unlit
		case 0x4C48: //Lamp Koi S unlit
		case 0x4C4A: //Lamp Koi E unlit
		case 0x4C4C: //Dragon Egg S unlit
		case 0x4C4E: //Dragon Egg E unlit
		case 0x4C50: //Lamp Stain Glass S unlit
		case 0x4C52: //Lamp Stand Classic S lit
		case 0x4C54: //Lamp Stand Classic E lit
		case 0x4C56: //Lamp Stand Bell S lit
		case 0x4C58: //Lamp Stand Bell E lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id++;
			break;
		case 0x4C41: //Lamp Dragon S lit non anima
		case 0x4C43: //Lamp Dragon E lit non anima
		case 0x4C45: //Lamp B Bird S lit
		case 0x4C47: //Lamp B Bird E lit
		case 0x4C49: //Lamp Koi S lit
		case 0x4C4B: //Lamp Koi E lit
		case 0x4C4D: //Dragon Egg S lit
		case 0x4C4F: //Dragon Egg E lit
		case 0x4C51: //Lamp Stain Glass S lit
		case 0x4C53: //Lamp Stand Classic S lit
		case 0x4C55: //Lamp Stand Classic E lit
		case 0x4C57: //Lamp Stand Bell S lit
		case 0x4C59: //Lamp Stand Bell E lit
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id--;
			break;
		case 0x4CDE: //Lamp Dragon S lit anima
		case 0x4CDF:
		case 0x4CE0:
		case 0x4CE1:
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x4C40;
			break;
		case 0x4CE2: //Lamp Dragon S lit anima
		case 0x4CE3:
		case 0x4CE4:
		case 0x4CE5:
			soundObject.SoundEffect( 0x0047, allHearSound );
			iUsed.morex = iUsed.dir;
			iUsed.dir = 99;
			iUsed.id = 0x4C42;
			break;
		//Halloween pumpkins
		case 0xA396: //unlit Octopus
		case 0xA39A: //unlit tall tree
		case 0xA39E: //unlit fairy
		case 0xA165: //unlit Skull
		case 0xA169: //unlit Tongue
		case 0xA16D: //unlit Twoface
		case 0xA139: //unlit Green Bat
		case 0xA13D: //unlit Green Cat
		case 0xA141: //unlit Green Devil 
		case 0xA145: //unlit Green Ghost
		case 0xA149: //unlit Green Anhk
		case 0xA14D: //unlit Green Owl
		case 0xA151: //unlit Green Skull
		case 0xA155: //unlit Green Spider
		case 0xA159: //unlit Green Swirl
		case 0xA15D: //unlit Green UO
		case 0xA161: //unlit Green Witch
		case 0xA198: //unlit Huge Dragon
		case 0x9F23: //unlit Rose
		case 0x9F27: //unlit OWl
		case 0x9D23: //unlit Gord Witch
		case 0x9D27: //unlit Gord Zombie
		case 0x9935: //unlit Tall UO
		case 0x9939: //unlit Tall Ankh
		case 0x993D: //unlit Tall Spider
		case 0x9941: //unlit Tall Bat
		case 0x9945: //unlit Tall Skull
		case 0x9949: //unlit Tall Daemon
		case 0x994D: //unlit Tall Spirit
		case 0x9951: //unlit Tall Scalele
		case 0x9BD5: //unlit Tall Rose
		case 0xA5E0: //unlit wolf howling
		case 0xA5E4: //unlit welcome
		case 0xA779: //unlit medusa right
		case 0xA77D: //unlit medusa left
		case 0xA781: //unlit deamon right
		case 0xA785: //unlit deamon left
			soundObject.SoundEffect(0x03BE, allHearSound);
			lightDir = 1;
			iUsed.id++;
			break;
		case 0xA397: //lit Octopus
		case 0xA39B: //lit tall tree
		case 0xA39F: //lit fairy
		case 0xA166: //lit Skull
		case 0xA16A: //lit Tongue
		case 0xA16E: //lit Twoface
		case 0xA13A: //lit Green Bat
		case 0xA13E: //lit Green Cat
		case 0xA142: //lit Green Devil 
		case 0xA146: //lit Green Ghost
		case 0xA14A: //lit Green Anhk
		case 0xA14E: //lit Green Owl
		case 0xA152: //lit Green Skull
		case 0xA156: //lit Green Spider
		case 0xA15A: //lit Green Swirl
		case 0xA15E: //lit Green UO
		case 0xA162: //lit Green Witch
		case 0xA199: //lit Huge Dragon
		case 0x9F24: //lit Rose
		case 0x9F28: //lit OWl
		case 0x9D24: //lit Gord Witch
		case 0x9D28: //lit Gord Zombie
		case 0x9936: //lit Tall UO
		case 0x993A: //lit Tall Ankh
		case 0x993E: //lit Tall Spider
		case 0x9942: //lit Tall Bat
		case 0x9946: //lit Tall Skull
		case 0x994A: //lit Tall Daemon
		case 0x994E: //lit Tall Spirit
		case 0x9952: //lit Tall Scalele
		case 0x9BD6: //lit Tall Rose
		case 0xA5E1: //lit wolf howling
		case 0xA5E5: //lit welcome
		case 0xA77A: //lit medusa right
		case 0xA77E: //lit medusa left
		case 0xA782: //lit deamon right
		case 0xA786: //lit deamon left
			soundObject.SoundEffect(0x0047, allHearSound);
			lightDir = 99;
			iUsed.id--;
			break;
		case 0x9934: //unlit Tall Witch
			soundObject.SoundEffect(0x03BE, allHearSound);
			lightDir = 1;
			iUsed.id = 0x9931;
			break;
		case 0x9931: //lit Tall Witch
			soundObject.SoundEffect(0x0047, allHearSound);
			lightDir = 1;
			iUsed.id = 0x9934;
			break;
	}

	// If lightDir value is set, we're trying to turn ON a light source
	if( lightDir > 0 )
	{
		// If the light source is turned off...
		if( iUsed.dir == 0 || iUsed.dir == 99)
		{
			// ...  AND has a value saved in morex
			if( iUsed.morex != 0 )
			{
				// ... use that value
				iUsed.dir = iUsed.morex;
				iUsed.morex = 0;
			}
			else
			{
				// ... else use default (lightDir) value
				iUsed.dir = lightDir;
			}
		}
	}

	return false;
}

function onLightChange( iLight, lightLevel )
{
	if( ValidateObject( iLight ) && iLight.isItem && dynamicLampPosts == true )
	{
		switch( iLight.id )
		{
			case 0x0B21: //simple lamp post id unlit
			case 0x0B23: //glass lamp post id unlit
			case 0x0B25: //fancy lamp post id unlit
				if( lightLevel > brightLvl )
				{
					if( iLight.dir == 0 || iLight.dir == 99 )
					{
						if( iLight.morex != 0 )
						{
							iLight.dir = iLight.morex;
						}
						else
						{
							iLight.dir = 29;
						}
					}
					iLight.id--;
				}
				break;
			case 0x0B20: //simple lamp post id lit
			case 0x0B22: //glass lamp post id lit
			case 0x0B24: //fancy lamp post id lit
				if( lightLevel < brightLvl )
				{
					iLight.morex = iLight.dir;
					iLight.dir = 99;
					iLight.id++;
				}
				break;
		}
	}
	return false;
}

function onUnequip( pEquipper, iEquipped )
{
	if( ValidateObject( iEquipped ) && iEquipped.isItem && unequipEnabled == true )
	{
		switch( iEquipped.id )
		{
			case 0x0A12: // player carryable torch lit
				pEquipper.SoundEffect( 0x0047, true );
				iEquipped.morex = iEquipped.dir;
				iEquipped.dir = 99;
				iEquipped.id = 0x0F64;
				break;
			case 0x0A0F: // player carryable candle id lit
				pEquipper.SoundEffect( 0x0047, true );
				iEquipped.morex = iEquipped.dir;
				iEquipped.dir = 99;
				iEquipped.id = 0x0A28;
				break;
			case 0x0A22: // player carryable lantern id lit
				pEquipper.SoundEffect( 0x0047, true );
				iEquipped.morex = iEquipped.dir;
				iEquipped.dir = 99;
				iEquipped.id = 0x0A25;
				break;
			default:
				break;
		}
	}
	return false;
}