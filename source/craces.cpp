//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  craces.cpp
//
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

// Abaddon:  I have a VERY set idea of features and plans for races
// Please DO NOT TOUCH THEM, because I will be working on them quite
// solidly, along with EviLDeD, over the next few months.  

#include "uox3.h"

/*char *cRaces::getName( PLAYER player1 )
// PRE:	player1 is a valid player
// POST:	returns pointer to a string
{
	return races[chars[player1].race].raceName;
}*/
char *cRaces::getName( RACEID race )
// PRE:	race is a valid race
// POST:	returns pointer to a string
{
	return races[race].raceName;
}
cRaces::~cRaces( void )
// PRE:	cRaces has been initialized
// POST:	Dynamic memory deleted
{
  //  EviLDeD - February 24, 2000
  //  Abaddon, when you allocate memory for an array, remember to 
  //  remove all the items, as well as any items that are dynakically
  //  created within them as well. Memory Cleanup
  for(int i=0;i<iCount;i++)
  {
    if(races[i].racialEnemies != NULL)
      delete[] races[i].racialEnemies;
    if(races[i].raceName != NULL)
      delete[] races[i].raceName;
  }
  delete[] races;
  //  EviLDeD - End

	//if( races != NULL )
	//	delete[] races;
}

void cRaces::load( void )
// PRE:		races.scp exists
// POST:	class loaded and populated, dynamically
{
	int i=0;
	int iCountA=0;
	int raceDiff=0;
	unsigned int raceCount = 0;
	bool done = false;
	openscript("races.scp");
	printf("Loading races now!\n");
	char sect[512];
	char skillthing[512];
	while ( !done )
	{
		sprintf(sect, "RACE %d", raceCount);
		if (!i_scripts[race_script]->find(sect))
		{
			done = true;
		}
		else
			raceCount++;
	}
	printf("Found right number of races! (%i)\n", raceCount );
	iCount = raceCount;
	races = new internalStruct[iCount];
	modifierCount = 0;
	strcpy( sect, "COMBAT MODS" );
	if( !i_scripts[race_script]->find( sect ) )
	{
		modifierCount = 4;
		combat = new combatModifiers[modifierCount];
		combat[0].value = 0;
		combat[1].value = 50;
		combat[2].value = 100;
		combat[3].value = 200;
	}
	else
	{
		read2();
		if( strcmp( script1, "MODCOUNT" ) )
		{
			printf("MODCOUNT must come before any entries!\n" );
			modifierCount = 4;
			combat = new combatModifiers[modifierCount];
			combat[0].value = 0;
			combat[1].value = 50;
			combat[2].value = 100;
			combat[3].value = 200;
		}
		else
		{
			modifierCount = str2num( script2 );
			if( modifierCount < 4 )
			{
				printf("MODCOUNT must be more >= 4, or it uses the defaults!\n" );
				modifierCount = 4;
				combat = new combatModifiers[modifierCount];
				combat[0].value = 0;
				combat[1].value = 50;
				combat[2].value = 100;
				combat[3].value = 200;

			}
			else
			{
				combat = new combatModifiers[modifierCount];
				for( i = 0; i < modifierCount; i++ )
				{
					read2();
					combat[i].value = str2num( script2 );
				}
			}
		}
	}

//	printf("Dynamically created right number\n");
	for( i=0; i < raceCount; i++ )
	{
		sprintf(sect, "RACE %d", i);
//		printf("Finding race %i", i );
		races[i].racialEnemies = new RACEID[raceCount];
		memset(races[i].racialEnemies, 0, raceCount * sizeof(RACEID));
		setSkill( STRENGTH, 100, i );
		setSkill( DEXTERITY, 100, i );
		setSkill( INTELLECT, 100, i );
		for( iCountA=0; iCountA < TRUESKILLS; iCountA++ )
		{
			setSkill( iCountA, 0, i );
		}
		for( iCountA=0; iCountA < raceCount; iCountA++ )
			setRacialNeutral( i, iCountA );

		setSkinMin( i, 0 );		setSkinMax( i, 0 );
		setBeardMin( i, 0 );	setBeardMax( i, 0 );
		setHairMin( i, 0 );		setHairMax( i, 0 );

		setGenderRestrict( 0, i );
		setLightAffect( false, i );		setLightDamage( i, 0 );
		setLightningAffect( false, i ); setLightningDamage( i, 0 );
		setRainAffect( false, i );		setRainDamage( i, 0 );
		setHeatAffect( false, i );		setHeatDamage( i, 0 );
		setColdAffect( false, i );		setColdDamage( i, 0 );
		setSnowAffect( false, i );		setSnowDamage( i, 0 );

		setLightLevel( i, 0x01 );
		setLightSecs( i, 0 );
		setArmorRestrict( i, 0 );
		setVisRange( i, 18 );
		setVisLevel( i, 0 );
		setLanguageMin( i, 0);

		setRequireBeard( false, i );
		setNoBeard( false, i );
		setIsPlayerRace( i, true );

		i_scripts[race_script]->find(sect);

		do {
			read2();
			if(!(strcmp(script1, "NAME")))
			{
				races[i].raceName = new char[strlen( script2 ) + 1];
				strcpy( races[i].raceName, script2 );
			}
			else if(!(strcmp("STRCAP", script1)))
				setSkill( STRENGTH, str2num( script2 ), i );
			else if(!(strcmp("DEXCAP", script1)))
				setSkill( DEXTERITY, str2num( script2 ), i );
			else if(!(strcmp("INTCAP", script1)))
				setSkill( INTELLECT, str2num( script2 ), i );
			else if(!(strcmp("HAIRMIN",script1)))
				setHairMin( i, hstr2num( script2 ) );
			else if(!(strcmp("HAIRMAX",script1)))
				setHairMax( i, hstr2num( script2 ) );
			else if(!(strcmp("SKINMIN",script1)))
				setSkinMin( i, hstr2num( script2 ) );
			else if(!(strcmp("SKINMAX",script1)))
				setSkinMax( i, hstr2num( script2 ) );
			else if(!(strcmp("BEARDMIN",script1)))
				setBeardMin( i, hstr2num( script2 ) );
			else if(!(strcmp("BEARDMAX",script1)))
				setBeardMax( i, hstr2num( script2 ) );
			else if(!(strcmp("REQUIREBEARD",script1)))
				setRequireBeard( true, i );
			else if(!(strcmp("NOBEARD",script1)))
				setNoBeard( true, i );

			else if(!(strcmp("LIGHTAFFECT",script1)))	// are we affected by light?
				setLightAffect( true, i );
			else if(!(strcmp("LIGHTDAMAGE",script1)))	// how much damage to take from light
				setLightDamage( i, str2num( script2 ) );
			else if(!(strcmp("LIGHTLEVEL",script1)))	// light level at which to take damage
				setLightLevel( i, str2num( script2 ) );
			else if(!(strcmp("LIGHTSECS",script1)))		// how often light affects in secs
				setLightSecs( i, str2num( script2 ) );

			else if(!(strcmp("LIGHTNINGAFFECT",script1)))	// are we affected by light?
				setLightningAffect( true, i );
			else if(!(strcmp("LIGHTNINGDAMAGE",script1)))	// how much damage to take from light
				setLightningDamage( i, str2num( script2 ) );
			else if(!(strcmp("LIGHTNINGSECS",script1)))		// how often light affects in secs
				setLightningSecs( i, str2num( script2 ) );

			else if(!(strcmp("RAINAFFECT",script1)))	// are we affected by light?
				setRainAffect( true, i );
			else if(!(strcmp("RAINDAMAGE",script1)))	// how much damage to take from light
				setRainDamage( i, str2num( script2 ) );
			else if(!(strcmp("RAINSECS",script1)))		// how often light affects in secs
				setRainSecs( i, str2num( script2 ) );

			else if(!(strcmp("SNOWAFFECT",script1)))	// are we affected by light?
				setSnowAffect( true, i );
			else if(!(strcmp("SNOWDAMAGE",script1)))	// how much damage to take from light
				setSnowDamage( i, str2num( script2 ) );
			else if(!(strcmp("SNOWSECS",script1)))		// how often light affects in secs
				setSnowSecs( i, str2num( script2 ) );

			else if(!(strcmp("HEATAFFECT",script1)))	// are we affected by light?
				setHeatAffect( true, i );
			else if(!(strcmp("HEATDAMAGE",script1)))	// how much damage to take from light
				setHeatDamage( i, str2num( script2 ) );
			else if(!(strcmp("HEATSECS",script1)))		// how often light affects in secs
				setHeatSecs( i, str2num( script2 ) );

			else if(!(strcmp("COLDAFFECT",script1)))	// are we affected by light?
				setColdAffect( true, i );
			else if(!(strcmp("COLDDAMAGE",script1)))	// how much damage to take from light
				setColdDamage( i, str2num( script2 ) );
			else if(!(strcmp("COLDSECS",script1)))		// how often light affects in secs
				setColdSecs( i, str2num( script2 ) );

			else if(!(strcmp("ARMORREST",script1)))
				setArmorRestrict( i, str2num( script2 ) );		// this is going to be a power of 2 thingy :)

			else if(!(strcmp("NIGHTVIS", script1 )))					// night vision level... light bonus
				setVisLevel( i, str2num( script2 ) );
			else if(!(strcmp("VISRANGE", script1 )))					// set visibility range ... defaults to 18
				setVisRange( i, str2num( script2 ) );


			for( iCountA=0; iCountA < TRUESKILLS; iCountA++ )
			{
				sprintf(skillthing, "%sG", skillname[iCountA] );
				if(!(strcmp(skillthing, script1)))
					setSkill( iCountA, str2num( script2 ), i );
				sprintf(skillthing, "%sL", skillname[iCountA] );
				if(!(strcmp(skillthing, script1)))
					setSkill( iCountA, modifierCount + str2num( script2 ), i );
			}
			if(!(strcmp("RACIALENEMY",script1)))
			{
				raceDiff = str2num(script2);
				if( raceDiff > raceCount )
					printf("Error in race %i, invalid enemy race %i", i, raceDiff );
				else
					setRacialEnemy( i, raceDiff );
			}
			else if(!(strcmp("RACIALAID",script1)))
			{
				raceDiff = str2num(script2);
				if( raceDiff > raceCount )
					printf("Error in race %i, invalid ally race %i", i, raceDiff );
				else
					setRacialAlly( i, raceDiff );
			}
			else if(!( strcmp( "GENDER", script1 )))
			{
				if(!( strcmp( "MALE", script2 )))
					setGenderRestrict( MALE, i );
				else if(!( strcmp( "FEMALE", script2 )))
					setGenderRestrict( FEMALE, i );
				else
					setGenderRestrict( MALE, i );
			}
            else if(!(strcmp("LANGUAGEMIN", script1 ))) // set language min 
	            setLanguageMin( i, str2num( script2 ) ); 
			else if(!(strcmp("PLAYERRACE", script1 ))) // is it a player race?
			{
				raceDiff = str2num( script2 );
				setIsPlayerRace( i, (raceDiff != 0) );
			}
		} while(script1[0]!='}');


	}
	closescript();
}

int cRaces::Compare(PLAYER player1, PLAYER player2)
// PRE: player1.race and player2.race are below the maximum number of races
// POST: Returns 0 if no enemy or ally, 1 if enemy, or 2 if ally
{
	if( chars[player1].race >= iCount ) // invalid race?
	{
		printf("ERROR: Player has bad race attribute! Player %i Race %i", player1, chars[player1].race );
		return 0;
	}
	else if( chars[player2].race >= iCount ) // invalid race?
	{
		printf("ERROR: Player has bad race attribute! Player %i Race %i", player2, chars[player2].race );
		return 0;
	}
	else
		return races[chars[player1].race].racialEnemies[chars[player2].race]; // enemy race
}

void cRaces::gate(PLAYER s, RACEID x, int always)
// PRE:	PLAYER s is valid, x is a race index and always = 0 or 1
// POST: PLAYER s belongs to new race x or doesn't change based on restrictions
{
//	char sect[512];
	
	int hairobject=-1, beardobject=-1, i = -1;
	int j;
	int hairColor=0;
	SERIAL serial,serhash,ci;
	int n;
	if( !isPlayerRace( x ) )
	{
		sysmessage( calcSocketFromChar( s ), "This race is not for players!" );
		return;
	}
	if( chars[s].raceGate == 65535 || always == 1 )
	{
		if( getGenderRestrict( x ) != 0 )
		{
			if( getGenderRestrict( x ) != FEMALE && ((chars[s].id1==0x01)&&(chars[s].id2==0x91)) )
			{
				sysmessage(calcSocketFromChar(s), "You are not of the right gender!");
				return;
			}
			if( getGenderRestrict( x ) != MALE && ((chars[s].id1==0x01)&&(chars[s].id2==0x90)) )
			{
				sysmessage(calcSocketFromChar(s), "You are not of the right gender!");
				return;
			}
		}
		staticeffect(s, 0x37, 0x3A, 0, 15);
		soundeffect2(s, 0x01, 0xE9);
		chars[s].raceGate=x;
		chars[s].race=x;
		if( chars[s].st > getSkill( STRENGTH, x ) )
		{
			sysmessage(calcSocketFromChar(s),"You feel yourself losing strength.");
			chars[s].st = getSkill( STRENGTH, x );
		}
		if( chars[s].dx > getSkill( DEXTERITY, x ) )
		{
			sysmessage(calcSocketFromChar(s),"You feel yourself losing speed.");
			chars[s].dx = getSkill( DEXTERITY, x );
		}
		if( chars[s].in > getSkill( INTELLECT, x ) )
		{
			sysmessage(calcSocketFromChar(s),"You feel yourself losing wisdom.");
			chars[s].in = getSkill( INTELLECT, x );
		}
		if ((j=calcSocketFromChar(s))!=-1)
		{
			statwindow(j, s);
		}

		if( hairRestricted( x ) )
		{
			serial=chars[s].serial;
			serhash=serial%HASHMAX;
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				i=contsp[serhash].pointer[ci];
				if (i!=-1)
					if ((items[i].layer==0x0B) && (items[i].contserial==serial))
					{
						hairobject=i;
						break;
					}  
			}
			if (hairobject>-1)
			{
				hairColor = items[hairobject].color1*256 + items[hairobject].color2;
				if( !hairInRange( hairColor, x ) )
				{
					hairColor = getRandomHair( x );
					items[hairobject].color1 = (unsigned char)(hairColor>>8);
					items[hairobject].color2 = (unsigned char)(hairColor%256);
//					for (j=0;j<now;j++) if (perm[j]) senditem(j,hairobject);  
					RefreshItem( hairobject );
				}
			}
		}
		if( getNoBeard( x ) )
		{
			serial=chars[s].serial;
			serhash=serial%HASHMAX;
			beardobject = -1;
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				i=contsp[serhash].pointer[ci];
				if (i!=-1)
					if ((items[i].layer==0x10) && (items[i].contserial==serial))
					{
						beardobject=i;
						break;
					}  
			}
			if( beardobject != -1 )
			{
				Items->DeleItem( beardobject );
			}
		}
		if( ( getRequireBeard( x ) ) && ((chars[s].id1==0x01) && (chars[s].id2==0x90)) )
		{
			serial=chars[s].serial;
			serhash=serial%HASHMAX;
			beardobject = -1;
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				i=contsp[serhash].pointer[ci];
				if (i!=-1)
					if ((items[i].layer==0x10) && (items[i].contserial==serial))
					{
						beardobject=i;
						break;
					}  
			}
			if (beardobject>-1)
			{
				// beard check down below will make it work right
			}
			else
			{
				if( beardRestricted( x ) )
					hairColor = getRandomBeard( x );
				else
					hairColor = 0x0480;
				n = Items->SpawnItem(calcSocketFromChar(s), s, 1, "#", 0, 0x20, 0x4C, (unsigned char)(hairColor>>8), (unsigned char)(hairColor%256),0,0);
				if( n != -1 )
				{
					setserial(n, s, 4);
					items[n].layer=0x10;
					RefreshItem( n );
				}
			}
		}
		if( beardRestricted( x ) )
		{
			serial = chars[s].serial;
			serhash = serial%HASHMAX;
			beardobject = -1;
			for( ci = 0; ci < contsp[serhash].max; ci++ )
			{
				i = contsp[serhash].pointer[ci];
				if( i != -1 )
				{
					if( items[i].layer == 0x10 && items[i].contserial == serial )
					{
						beardobject = i;
						break;
					}
				}
			}
			if( beardobject > -1 )
			{
				hairColor = items[beardobject].color1*256 + items[beardobject].color2;
				if( !beardInRange( hairColor, x ) )
				{
					hairColor = getRandomBeard( x );
					items[beardobject].color1 = (unsigned char)(hairColor>>8);
					items[beardobject].color2 = (unsigned char)(hairColor%256);
					RefreshItem( beardobject );
				}
			}
		}
		if( skinRestricted( x ) )	// do we have a limited skin colour range?
		{
			hairColor = chars[s].skin1*256 + chars[s].skin2;
			if( !skinInRange( hairColor, x ) )	// if not in range
			{
				hairColor = getRandomSkin( x );	// get random skin in range
				chars[s].skin1 = chars[s].xskin1 = chars[s].orgskin1 = (unsigned char)(hairColor>>8);
				chars[s].skin2 = chars[s].xskin2 = chars[s].orgskin2 = (unsigned char)(hairColor%256);
			}

		}
		updatechar( s );
	}
	else sysmessage(calcSocketFromChar(s),"You have already used a race gate with this character.");
}

int cRaces::armor( int id1, int id2 )
// PRE: id1 and id2 point to valid item IDs
// POST: returns level of armour restriction
//       6 - plate and below
//       5 - bone and below
//       4 - chain and below
//       3 - ring and below
//       2 - studded leather and below
//       1 - leather and female and below
//       0 - no restriction
{
	switch( id1*256 + id2 )
	{
	case 0x1416:
	case 0x1413:
	case 0x1414:
	case 0x1412:
	case 0x141A:
	case 0x1410:	return 6;

	case 0x144F:
	case 0x144E:
	case 0x1450:
	case 0x1451:
	case 0x1452:	return 5;
		
	case 0x13C4:
	case 0x13C3:
	case 0x13BB:	return 4;

	case 0x13ED:
	case 0x13EF:
	case 0x13F1:
	case 0x13F2:	return 3;

	case 0x13D6:
	case 0x13DD:
	case 0x13D4:
	case 0x13E1:
	case 0x13E2:	return 2;

	case 0x13CD:
	case 0x13D2:
	case 0x13D3:
	case 0x1DBA:
	case 0x13CE:
	case 0x13C7:
	case 0x1C00:
	case 0x1C02:
	case 0x1C04:
	case 0x1C06:
	case 0x1C08:
	case 0x1C0A:
	case 0x1C0C:	return 1;

	case 0x1408:
	case 0x140A:
	case 0x140C:
	case 0x140E:
	case 0x141C:
	default:	return 0;
	}
	return 0;
}

COLOR cRaces::getColorValue( unsigned char colorType, unsigned char maxmin, RACEID race )
// PRE:	race is valid race, colorType either BEARD, HAIR or SKIN, maxmin 0 or 1
// POST:	returns COLOR value depending on input
{
	if( maxmin != 0 && maxmin != 1 ) maxmin = 1;
	switch( colorType )
	{
	case BEARD:	return races[race].beardColors[maxmin];
	case HAIR:	return races[race].hairColors[maxmin];
	case SKIN:	return races[race].skinColors[maxmin];
	default:	return races[race].skinColors[maxmin];
	}
	return races[race].skinColors[maxmin];	// compilers that don't realize it never gets here
}

void cRaces::setColorValue( unsigned char colorType, unsigned char maxmin, COLOR value, RACEID race )
// PRE:	race is valid race, colorType either BEARD, HAIR or SKIN, maxmin 0 or 1
// POST:	sets colour value
{
	if( maxmin != 0 && maxmin != 1 ) maxmin = 1;
	switch( colorType )
	{
	case BEARD:	races[race].beardColors[maxmin] = value; break;
	case HAIR:	races[race].hairColors[maxmin] = value; break;
	case SKIN:	races[race].skinColors[maxmin] = value; break;
	default:	races[race].skinColors[maxmin] = value; break;
	}
}

bool cRaces::beardInRange( COLOR color, RACEID x )
// PRE:	Race is valid
// POST:	Returns whether colour is valid for beard
{
	return ( color >= getBeardMin( x ) && color <= getBeardMax( x ) );
}
bool cRaces::skinInRange( COLOR color, RACEID x )
// PRE:	Race is valid
// POST:	Returns whether colour is valid for skin
{
	return ( color >= getSkinMin( x ) && color <= getSkinMax( x ) );
}
bool cRaces::hairInRange( COLOR color, RACEID x )
// PRE:	Race is valid
// POST:	Returns whether colour is valid for hair
{
	return ( color >= getHairMin( x ) && color <= getHairMax( x ) );
}

int cRaces::getSkill( int skill, RACEID race )
// PRE:	skill is valid, race is valid
// POST:	returns skill bonus associated with race
{ 
	return races[race].iSkills[skill];
}

void cRaces::setSkill( int skill, int value, RACEID race )
// PRE:	skill is valid, value is valid, race is valid
// POST:	sets race's skill bonus to value
{ 
	races[race].iSkills[skill] = value;
}

GENDER cRaces::getGenderRestrict( RACEID race ) 
// PRE:	Race is valid
// POST:	returns whether race's gender is restricted, and if so, which gender
//		0 - none 1 - male 2 - female
{ 
	return races[race].restrictGender; 
}

void cRaces::setGenderRestrict( GENDER gender, RACEID race )
// PRE:	Race is valid, gender is valid
// POST:	Sets race's gender restriction
{ 
	races[race].restrictGender = gender; 
}

bool cRaces::getLightAffect( RACEID race )
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by light
{ 
	return getAffect( race, LIGHT );
}

void cRaces::setLightAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	setAffect( race, LIGHT, value );
}

bool cRaces::getRequireBeard( RACEID race )
// PRE:	race is valid
// POST:	returns whether race must have beard
{ 
	return races[race].reqBeard; 
}

void cRaces::setRequireBeard( bool value, RACEID race )
// PRE:	Race is valid, and value is true or false
// POST:	sets whether race requires a beard
{ 
	races[race].reqBeard = value; 
}

COLOR cRaces::getHairMin( RACEID race )
// PRE:	Race is valid
// POST:	returns COLOR of hair for race at the minimum
{ 
	return getColorValue( HAIR, 0, race ); 
}

COLOR cRaces::getHairMax( RACEID race )
// PRE:	Race is valid
// POST:	returns COLOR of hair for race at the maximum
{ 
	return getColorValue( HAIR, 1, race ); 
}

COLOR cRaces::getBeardMin( RACEID race )
// PRE:	race is valid
// POST:	returns COLOR of beard for race at the minimum
{ 
	return getColorValue( BEARD, 0, race ); 
}

COLOR cRaces::getBeardMax( RACEID race )
// PRE:	Race is valid
// POST:	Returns COLOR of beard for race at the maximum
{ 
	return getColorValue( BEARD, 1, race ); 
}

COLOR cRaces::getSkinMin( RACEID race )
// PRE:	Race is valid
// POST:	Returns COLOR of skin for race at the minimum
{ 
	return getColorValue( SKIN, 0, race ); 
}

COLOR cRaces::getSkinMax( RACEID race )
// PRE:	Race is valid
// POST:	Returns COLOR of skin for race at the maximum
{ 
	return getColorValue( SKIN, 1, race ); 
}

void cRaces::setHairMin( RACEID race, COLOR value )
// PRE:	Race is valid, COLOR is valid color value or 0
// POST:	sets race's min hair colour to value
{ 
	setColorValue( HAIR, 0, value, race ); 
}

void cRaces::setHairMax( RACEID race, COLOR value )
// PRE:	Race is valid, COLOR is valid color value or 0
// POST:	sets race's max hair colour to value
{ 
	setColorValue( HAIR, 1, value, race ); 
}

void cRaces::setBeardMin( RACEID race, COLOR value )
// PRE:	Race is valid, COLOR is valid color value or 0
// POST:	sets race's min beard colour to value
{ 
	setColorValue( BEARD, 0, value, race ); 
}

void cRaces::setBeardMax( RACEID race, COLOR value )
// PRE:	Race is valid, COLOR is valid color value or 0
// POST:	sets race's max beard colour to value
{ 
	setColorValue( BEARD, 1, value, race ); 
}

void cRaces::setSkinMin( RACEID race, COLOR value )
// PRE:	Race is valid, COLOR is valid color value or 0
// POST:	sets race's min skin colour to value
{ 
	setColorValue( SKIN, 0, value, race ); 
}

void cRaces::setSkinMax( RACEID race, COLOR value )
// PRE:	Race is valid, COLOR is valid color value or 0
// POST:	sets race's max skin colour to value
{ 
	setColorValue( SKIN, 1, value, race ); 
}


void cRaces::setArmorRestrict( RACEID race, ARMORCLASS value )
// PRE:	Race is valid, value is a valid armorclass
// POST:	sets the armor class of race
{ 
	races[race].armorRestrict = value; 
}

ARMORCLASS cRaces::getArmorRestrict( RACEID race )
// PRE:	Race is valid
// POST:	Returns armor class of race
{ 
	return races[race].armorRestrict; 
}

COLOR cRaces::getRandomSkin( RACEID x )
// PRE:	Race is valid
// POST:	returns a valid skin colour for the race
{ 
	return RandomNum( getSkinMin( x ), getSkinMax( x ) );
}

COLOR cRaces::getRandomHair( RACEID x )
// PRE:	Race is valid
// POST:	returns a valid hair colour for the race
{ 
	return RandomNum( getHairMin( x ), getHairMax( x ) ); 
}

COLOR cRaces::getRandomBeard( RACEID x )
// PRE:	Race is valid
// POST:	returns a valid beard colour for the race
{ 
	return RandomNum( getBeardMin( x ), getBeardMax( x ) ); 
}

bool cRaces::beardRestricted( RACEID x )
// PRE:	race is valid
// POST:	returns true if race's beard colour is restricted
{ 
	return( getBeardMin( x ) != 0 && getBeardMax( x ) != 0 ); 
}

bool cRaces::hairRestricted( RACEID x )
// PRE:	race is valid
// POST:	returns true if race's hair colour is restricted
{ 
	return( getHairMin( x ) != 0 && getHairMax( x ) != 0 ); 
}

bool cRaces::skinRestricted( RACEID x )
// PRE:	race is valid
// POST:	returns true if race's skin colour is restricted
{ 
	return( getSkinMin( x ) != 0 && getSkinMax( x ) != 0 ); 
}

SI16 cRaces::getDamageFromSkill( int skill, RACEID x )
// PRE:	x is valid, skill is valid
// POST:	returns chance difference to race x in skill skill
{
	assert( skill <= SKILLS );
	int modifier = races[x].iSkills[skill];
	if( modifier >= modifierCount )
		return -(combat[modifier].value);
	else
		return (combat[modifier].value);
	return 0;
}

SI16 cRaces::getFightPercent( int skill, RACEID x )
// PRE:	x is valid, skill is valid
// POST:	returns positive/negative fight damage bonus for race x with skill skill
{
	int modifier = races[x].iSkills[skill];
	int divValue = combat[modifier].value / 10;
	divValue = divValue / 10;
	if( divValue == 0 )
		return 100;
	if( modifier >= modifierCount )
		return -(int)(100/(float)divValue);
	else
		return (int)(100/(float)divValue);
	return 100;
}

void cRaces::setRacialInfo( RACEID race, RACEID toSet, RACEREL value )
// PRE:		race and toSet are valid races, value is a valid relation
// POST:	the relation between race and toset is set to value
{
	races[race].racialEnemies[toSet] = value;
}
void cRaces::setRacialEnemy( RACEID race, RACEID enemy )
// PRE:		race and enemy are valid
// POST:	enemy is race's enemy
{
	setRacialInfo( race, enemy, 1 );
}
void cRaces::setRacialAlly( RACEID race, RACEID ally )
// PRE:		race and ally are valid
// POST:	ally is race's ally
{
	setRacialInfo( race, ally, 2 );
}
void cRaces::setRacialNeutral( RACEID race, RACEID neutral )
// PRE:		race and neutral are valid
// POST:	neutral is neutral to race
{
	setRacialInfo( race, neutral, 0 );
}


SKILLVAL cRaces::getLanguageMin( RACEID x )
//  PRE:		x is a valid race
// POST:		returns race's minimum skill for language
{
		return races[x].languageMin;
}

void cRaces::setLanguageMin( SKILLVAL toSetTo, RACEID race )
//  PRE:		race and toSetTo is valid
// POST:		race's min language requirement is set to toSetTo
{
	races[race].languageMin = toSetTo;
}


// SNOW SECTION

void cRaces::setSnowDamage( RACEID race, unsigned char value )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	setDamage( race, SNOW, value );
}

bool cRaces::getSnowAffect( RACEID race )
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by light
{ 
	return getAffect( race, SNOW );
}

void cRaces::setSnowAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	setAffect( race, SNOW, value ); 
}


// RAIN SECTION

void cRaces::setRainDamage( RACEID race, unsigned char value )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	setDamage( race, RAIN, value );
}

bool cRaces::getRainAffect( RACEID race )
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by light
{ 
	return getAffect( race, RAIN );
}

void cRaces::setRainAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by rain
{
	setAffect( race, RAIN, value ); 
}

// COLD SECTION

void cRaces::setColdDamage( RACEID race, unsigned char value )
// PRE:	Race is valid, value is true or false
// POST:	sets how much race is affected by cold
{
	setDamage( race, COLD, value ); 
}

bool cRaces::getColdAffect( RACEID race )
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by cold
{
	return getAffect( race, COLD ); 
}

void cRaces::setColdAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by cold
{ 
	setAffect( race, COLD, value );
}


// HEAT SECTION

void cRaces::setHeatDamage( RACEID race, unsigned char value )
// PRE:	Race is valid, value is true or false
// POST:	sets how much race is affected by heat
{ 
	setDamage( race, HEAT, value );
}

bool cRaces::getHeatAffect( RACEID race )
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by heat
{
	return getAffect( race, HEAT ); 
}

void cRaces::setHeatAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by heat
{ 
	setAffect( race, HEAT, value );
}


// LIGHT SECTION

void cRaces::setLightDamage( RACEID race, unsigned char value )
// PRE:	Race is valid, value is valid unsigned char
// POST:	sets how much race is affected by light
{
	setDamage( race, LIGHT, value ); 
}

void cRaces::setLightLevel( RACEID race, LIGHTLEVEL value )
// PRE:	Race is valid, value is a valid light level
// POST:	the light level that race burns at is set to value
{ 
	races[race].lightLevel = value; 
}

LIGHTLEVEL cRaces::getLightLevel( RACEID race )
// PRE:	Race is valid
// POST:	Returns the light level that race burns at
{ 
	return races[race].lightLevel; 
}

void cRaces::setLightSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from light
{ 
	setSecs( race, LIGHT, value );
}

SECONDS cRaces::getLightSecs( RACEID race )
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from light
{ 
	return getSecs( race, LIGHT ); 
}

// LIGHTNING SECTION

void cRaces::setLightningDamage( RACEID race, unsigned char value )
// PRE:	Race is valid, value is valid unsigned char
// POST:	sets how much race is affected by lightning
{ 
	setDamage( race, LIGHTNING, value );
}

bool cRaces::getLightningAffect( RACEID race )
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by lightning
{ 
	return getAffect( race, LIGHTNING );
}

void cRaces::setLightningAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by lightning
{ 
	setAffect( race, LIGHTNING, value );
}

void cRaces::setRainSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from rain
{ 
	setSecs( race, RAIN, value ); 
}

SECONDS cRaces::getRainSecs( RACEID race )
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from rain
{ 
	return getSecs( race, RAIN ); 
}

void cRaces::setSnowSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from snow
{ 
	setSecs( race, SNOW, value ); 
}

SECONDS cRaces::getSnowSecs( RACEID race )
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from snow
{ 
	return getSecs( race, SNOW ); 
}

void cRaces::setHeatSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from heat
{ 
	setSecs( race, HEAT, value ); 
}

SECONDS cRaces::getHeatSecs( RACEID race )
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from heat
{ 
	return getSecs( race, HEAT ); 
}

void cRaces::setColdSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from cold
{ 
	setSecs( race, COLD, value ); 
}

SECONDS cRaces::getColdSecs( RACEID race )
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from cold
{ 
	return getSecs( race, COLD ); 
}

void cRaces::setLightningSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from lightning
{ 
	setSecs( race, LIGHTNING, value );
}

SECONDS cRaces::getLightningSecs( RACEID race )
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from lightning
{ 
	return getSecs( race, LIGHTNING ); 
}

SECONDS cRaces::getSecs( RACEID race, int element )
// PRE:	Race is valid, element is an element of weather
// POST:	Returns number of seconds between burns for race from element
{
	return races[race].weathSecs[element];
}

void cRaces::setSecs( RACEID race, int element, SECONDS value )
// PRE:	Race is valid, element is element of weather, value is seconds
// POST:	Sets number of seconds between burns for race from element
{
	races[race].weathSecs[element] = value;
}

bool cRaces::getAffect( RACEID race, int element )
// PRE:	Race is valid, elemt is element of weather
// POST:	returns whether race is affected by that element
{
	return races[race].weathAffect[element];
}

void cRaces::setAffect( RACEID race, int element, bool value)
// PRE:	Race is valid, element is element of weather, value is true or false
// POST:	Sets whether race affected by element
{
	races[race].weathAffect[element] = value;
}

unsigned char cRaces::getDamage( RACEID race, int element )
// PRE:	Race is valid, element is an element of weather
// POST:	Returns damage incurred by race from element
{
	return races[race].weathDamage[element];
}

void cRaces::setDamage( RACEID race, int element, unsigned char damage )
// PRE:	race is valid, element is element of weather, damage is >0 && <256
// POST:	sets damage incurred by race from element
{
	races[race].weathDamage[element] = damage;
}

LIGHTLEVEL cRaces::getVisLevel( RACEID x )
// PRE:	x is valid
// POST:	returns light level bonus of race x
{
	return races[x].nightVision;
}
void cRaces::setVisLevel( RACEID x, LIGHTLEVEL bonus )
// PRE:	x is valid
// POST:	sets race's light level bonus to bonus
{
	races[x].nightVision = bonus;
}

RANGE cRaces::getVisRange( RACEID x )
// PRE:	x is valid
// POST:	Returns distance that race can see
{
	return races[x].visDistance;
}

void cRaces::setVisRange( RACEID x, RANGE range )
// PRE:	x is valid and range is valid
// POST:	sets distance that race can see to range
{
	races[x].visDistance = range;
}

bool cRaces::getNoBeard( RACEID x )
{
	return races[x].noBeard;
}

void cRaces::setNoBeard( bool value, RACEID race )
{
	races[race].noBeard = value;
}


void cRaces::debugPrint( RACEID x )
{
	printf("Race ID: %d\n", (int) x); fflush(stdout);
	printf("Race: %s\n", races[x].raceName);
	if (races[x].reqBeard) printf("Req Beard: Yes\n"); else printf("Req Beard: No\n");
	if (races[x].noBeard) printf("No Beard: Yes\n"); else printf("No Beard: No\n");
	if (races[x].playerRace) printf("Player Race: Yes\n"); else printf("Player Race: No\n");
	printf("Restrict Gender: %d\n", (int) races[x].restrictGender);
	printf("LightLevel: %d\n", (int) races[x].lightLevel);
	printf("NightVistion: %d\n", (int) races[x].nightVision);
	printf("ArmorRestrict: %d\n", (int) races[x].armorRestrict);
	printf("LangMin: %d\n", (int) races[x].languageMin);
	printf("Vis Distance: %d\n\n", (int) races[x].visDistance);
}

void cRaces::debugPrintAll( void )
{
	for (RACEID x = 0; x < iCount; ++x)
		debugPrint(x);
}

void cRaces::setIsPlayerRace( RACEID x, bool value )
// PRE:		x is a valid race, value is either true or false
// POST:	sets if x is a player race or not
{
	races[x].playerRace = value;
}


void cRaces::InitRace( RACEID race )
// PRE:		Newly created race, do memsets and clear
// POST:	clears out, sets all defaults
{
	int iCountA = 0;
	memset( races[race].racialEnemies, 0, iCount * sizeof(RACEID));
	setSkill( STRENGTH, 100, race );
	setSkill( DEXTERITY, 100, race );
	setSkill( INTELLECT, 100, race );
	for( iCountA = 0; iCountA < TRUESKILLS; iCountA++ )
	{
		setSkill( iCountA, 0, race );
	}
	for( iCountA = 0; iCountA < iCount; iCountA++ )
		setRacialNeutral( race, iCountA );

	setSkinMin( race, 0 );		setSkinMax( race, 0 );
	setBeardMin( race, 0 );	setBeardMax( race, 0 );
	setHairMin( race, 0 );		setHairMax( race, 0 );

	setGenderRestrict( 0, race );
	setLightAffect( false, race );		setLightDamage( race, 0 );
	setLightningAffect( false, race );	setLightningDamage( race, 0 );
	setRainAffect( false, race );		setRainDamage( race, 0 );
	setHeatAffect( false, race );		setHeatDamage( race, 0 );
	setColdAffect( false, race );		setColdDamage( race, 0 );
	setSnowAffect( false, race );		setSnowDamage( race, 0 );

	setLightLevel( race, 0x01 );
	setLightSecs( race, 0 );
	setArmorRestrict( race, 0 );
	setVisRange( race, 18 );
	setVisLevel( race, 0 );
	setLanguageMin( race, 0);

	setRequireBeard( false, race );
	setNoBeard( false, race );
	setIsPlayerRace( race, true );

}