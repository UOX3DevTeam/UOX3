#include "uox3.h"
#ifndef va_start
	#include <cstdarg>
//	using namespace std;
#endif
#include "ssection.h"

// Implementation of town regions

const SI08 MAYOR = 0x01;
const SI08 ENEMY = 0x02;
const SI08 JOINER = 0x03;

cTownRegion::cTownRegion( UI08 region ) : race( 0 ), weather( 255 ), priv( 0 ), regionNum( region ), midilist( 0 ),
mayorSerial( INVALIDSERIAL ), taxedResource( 0x0EED ), taxedAmount( 0 ), goldReserved( 0 ), guardsPurchased( 0 ),
resourceCollected( 0 ), visualAppearance( WRLD_SPRING ), health( 30000 ), timeToElectionClose( 0 ), timeToNextPoll( 0 ), 
timeSinceGuardsPaid( 0 ), timeSinceTaxedMembers( 0 ), worldNumber( 0 )
{
	guards.resize( 0 );
	townMember.resize( 0 );
	alliedTowns.resize( 0 );
	memset( goodsell, 0, sizeof( goodsell[0] ) * 256 );
	memset( goodbuy,  0, sizeof(  goodbuy[0] ) * 256 );
	memset( goodrnd1, 0, sizeof( goodrnd1[0] ) * 256 );
	memset( goodrnd2, 0, sizeof( goodrnd2[0] ) * 256 );
	strcpy( name, Dictionary->GetEntry( 1117 ) );
	strcpy( guardowner, Dictionary->GetEntry( 1118 ) );
}

cTownRegion::~cTownRegion()
{
	guards.resize( 0 );
	townMember.resize( 0 );
	alliedTowns.resize( 0 );
}

bool cTownRegion::Load( Script *ss )
// ss is a script section containing all the data!
{
	int location = -1;
	int guardUpper;
	char temp[512];
	const char *tag = NULL;
	const char *data = NULL;
	sprintf( temp, "TOWNREGION %i", regionNum );
	if( !ss->isin( temp ) )	// doesn't exist
		return false;

	ScriptSection *target = ss->FindEntry( temp );
	for( tag = target->First(); !target->AtEnd(); tag = target->Next() )
	{
		data = target->GrabData();
		switch( tag[0] )
		{
		case 'A':
			if( !strcmp( tag, "ALLYTOWN" ) )
			{
				guardUpper = alliedTowns.size();
				alliedTowns.resize( guardUpper + 1 );
				alliedTowns[guardUpper] = (UI08)makeNum( data );
			}
			break;
		case 'E':
			if( !strcmp( tag, "ELECTIONTIME" ) )
				timeToElectionClose = makeNum( data );
			break;
		case 'G':
			if( !strcmp( tag, "GUARDOWNER" ) )
				strncpy( guardowner, data, 49 );
			else if( !strcmp( tag, "GUARD" ) )	// load our purchased guard
			{
				guardUpper = guards.size();
				guards.resize( guardUpper + 1 );
				guards[guardUpper] = makeNum( data );
			}
			else if( !strcmp( tag, "GUARDSBOUGHT" ) ) // num guards bought
				guardsPurchased = (SI16)makeNum( data );
			break;
		case 'H':
			if( !strcmp( tag, "HEALTH" ) )
				health = (SI16)makeNum( data );
			break;
		case 'M':
			if( !strcmp( tag, "MEMBER" ) )
			{
				location = townMember.size();
				townMember.resize( location + 1 );
				townMember[location].targVote = INVALIDSERIAL;
				townMember[location].townMember = makeNum( data );
			}
			else if( !strcmp( tag, "MAYOR" ) )
				mayorSerial = makeNum( data );
			break;
		case 'N':
			if( !strcmp( tag, "NAME" ) )
				strncpy( name, data, 49 );
			break;
		case 'P':
			if( !strcmp( tag, "PRIV" ) )
				priv = (UI08)makeNum( data );
			else if( !strcmp( tag, "POLLTIME" ) )
				timeToNextPoll = makeNum( data );
			break;
		case 'R':
			if( !strcmp( tag, "RACE" ) )
				race = static_cast<RACEID>(makeNum( data ));
			else if( !strcmp( tag, "RESOURCEAMOUNT" ) )
				goldReserved = makeNum( data );
			else if( !strcmp( tag, "RESOURCECOLLECTED" ) )
				resourceCollected = makeNum( data );
			break;
		case 'T':
			if( !strcmp( tag, "TAXEDID" ) )
				taxedResource = (UI16)makeNum( data );
			else if( !strcmp( tag, "TAXEDAMOUNT" ) )
				taxedAmount = (UI16)makeNum( data );
			else if( !strcmp( tag, "TIMET" ) )
				timeSinceTaxedMembers = makeNum( data );
			else if( !strcmp( tag, "TIMEG" ) )
				timeSinceGuardsPaid = makeNum( data );
			break;
		case 'V':
			if( !strcmp( tag, "VOTE" ) && location != -1 )
				townMember[location].targVote = makeNum( data );
			break;
		case 'W':
			if( !strcmp( tag, "WEATHER" ) )
				weather = (UI08)makeNum( data );
			else if( !strcmp( tag, "WORLD" ) )
				worldNumber = (UI08)makeNum( data );
			break;
		}
	}
	return true;
}
bool cTownRegion::Save( FILE *fp )
// entry is the region #, fp is the file to save in
{
	fprintf( fp, "[TOWNREGION %i]\n{\n", regionNum );
	fprintf( fp, "WEATHER=%i\n", weather );
	fprintf( fp, "RACE=%i\n", race );
	fprintf( fp, "NAME=%s\n", name );
	fprintf( fp, "GUARDOWNER=%s\n", guardowner );
	fprintf( fp, "MAYOR=%i\n", mayorSerial );
	fprintf( fp, "PRIV=%i\n", priv );
	fprintf( fp, "RESOURCEAMOUNT=%i\n", goldReserved );
	fprintf( fp, "TAXEDID=%x\n", taxedResource );
	fprintf( fp, "TAXEDAMOUNT=%i\n", taxedAmount );
	fprintf( fp, "GUARDSPURCHASED=%i\n", guardsPurchased );
	fprintf( fp, "TIMEG=%li\n", timeSinceGuardsPaid );		// time since guards paid
	fprintf( fp, "TIMET=%li\n", timeSinceTaxedMembers );	// time since taxed
	fprintf( fp, "RESOURCECOLLECTED=%li\n", resourceCollected );	// amount of taxes we've collected
	fprintf( fp, "HEALTH=%i\n", health );
	fprintf( fp, "ELECTIONTIME=%li\n", timeToElectionClose );	// time to election close
	fprintf( fp, "POLLTIME=%li\n", timeToNextPoll );	// time to next poll
	fprintf( fp, "WORLD=%i\n", worldNumber );	// time to next poll
	for( UI32 counter = 0; counter < townMember.size(); counter++ )
	{
		fprintf( fp, "MEMBER=%i\n", townMember[counter].townMember );
		fprintf( fp, "VOTE=%i\n", townMember[counter].targVote );
	}
	for( UI32 guardCount = 0; guardCount < guards.size(); guardCount++ )
	{
		fprintf( fp, "GUARD=%i\n", guards[guardCount] );
	}
	for( UI32 allyCount = 0; allyCount < alliedTowns.size(); allyCount++ )
	{
		fprintf( fp, "ALLYTOWN=%i\n", alliedTowns[allyCount] );
	}

	fprintf( fp, "}\n\n" );
	return true;
}
void cTownRegion::CalcNewMayor( void )
// There has got to be a better way than this hideous O(n^2) algy
{
	if( townMember.size() == 0 )
		return;
	// if there are no members, there are no new mayors
	std::vector< int > votes;
	votes.resize( townMember.size() );
	for( UI32 counter = 0; counter < votes.size(); counter++ )
	{
		votes[counter] = 0;	// init the count before adding
		for( UI32 counter2 = 0; counter2 < votes.size(); counter2++ )
		{
			if( townMember[counter].townMember == townMember[counter2].targVote )
				votes[counter]++;
		}
	}
	int maxIndex = 0;
	for( UI32 indexCounter = 1; indexCounter < votes.size(); indexCounter++ )
	{
		if( votes[indexCounter] > votes[maxIndex] )
			maxIndex = indexCounter;
	}
	// maxIndex is now our new mayor!
	CChar *oldMayor = GetMayor();
	if( mayorSerial == townMember[maxIndex].townMember )
	{
		cSocket *targSock = calcSocketObjFromChar( calcCharObjFromSer( mayorSerial ) );
		if( targSock != NULL )
			sysmessage( targSock, 1119 );
		// welcome the mayor back for another term
	}
	if( votes[maxIndex] > 0 )
		mayorSerial = townMember[maxIndex].townMember;
	else
		mayorSerial = 0xFFFFFFF;

	CChar *newMayor = GetMayor();
	if( oldMayor != NULL )
		oldMayor->SetTownpriv( 1 );
	if( newMayor != NULL )
		newMayor->SetTownpriv( 2 );	// set mayor priv last
	if( newMayor != oldMayor )
	{
		if( oldMayor == NULL && newMayor != NULL )
			TellMembers( 1120, newMayor->GetName() );
		else if( oldMayor != NULL && newMayor == NULL )
			TellMembers( 1121 );
		else if( oldMayor != NULL && newMayor != NULL )
			TellMembers( 1122, oldMayor->GetName(), newMayor->GetName() );
		else
			TellMembers( 1123 );
	}
	else if( newMayor != NULL )
		TellMembers( 1124, oldMayor->GetName() );
	else
		TellMembers( 1123 );
}
CChar * cTownRegion::GetMayor( void )
// returns the index into chars[] of the mayor
{
	return calcCharObjFromSer( mayorSerial );
}

SERIAL cTownRegion::GetMayorSerial( void ) const
// returns the mayor's serial
{
	return mayorSerial;
}

bool cTownRegion::AddAsTownMember( CChar& toAdd )
// toAdd is the character to add
{
	if( Races->CompareByRace( toAdd.GetRace(), race ) == 1 )	// if we're racial enemies
		return false;	// we can't have a racial enemy in the town!

	for( UI32 counter = 0; counter < townMember.size(); counter++ )		// exhaustive error checking
	{
		if( townMember[counter].townMember == toAdd.GetSerial() )
			return false;	// already exists in our town!
	}
	townMember.resize( townMember.size() + 1 );
	townMember[townMember.size()-1].townMember = toAdd.GetSerial();
	townMember[townMember.size()-1].targVote = INVALIDSERIAL;
	townMember[townMember.size()-1].PlayerBank = NULL;
	toAdd.SetTown( regionNum );
	toAdd.SetTownpriv( 1 );	// set as resident
	return true;
}
bool cTownRegion::RemoveTownMember( CChar& toAdd )
{
	if( toAdd.GetTown() != regionNum )
		return false;	// not in our town

	for( UI32 counter = 0; counter < townMember.size(); counter++ )
	{
		if( toAdd.GetSerial() == townMember[counter].townMember )
		{
			RemoveCharacter( counter );
			toAdd.SetTown( 255 );	// don't know what default town is, now it's 255 :>
			toAdd.SetTownpriv( 0 );
			return true;
		}
	}
	return false;	// we're not in our town
}

bool cTownRegion::InitFromScript( int& l )
{
	ScriptSection *Regions;
	const char *tag = NULL;
	const char *data = NULL;
	char sect[512];
	int actgood = -1;
	bool orePrefLoaded = false;
	guards.resize( 10 );
	UI32 a;
	for( a = 0; a < 10; a++ )
	{
		guards[a] = RandomNum( 1000, 1001 );
	}
	a = 0;
	minColourSkill = 600;
	chanceFindBigOre = 80;
	chanceColourOre = 10;
	sprintf( sect, "REGION %i", regionNum );
	Regions = FileLookup->FindEntry( sect, regions_def );
	if( Regions == NULL ) 
		return false;
	else
	{
		for( tag = Regions->First(); !Regions->AtEnd(); tag = Regions->Next() )
		{
			data = Regions->GrabData();
			switch( tag[0] )
			{

			case 'a':
			case 'A':
				if( !strcmp( "ABWEATH", tag ) )
				{
					weather = (UI08)makeNum( data );
				}
				else if( !strcmp( "APPEARANCE", tag ) )
				{
					visualAppearance = WRLD_COUNT;
					for( WorldType wt = WRLD_SPRING; wt < WRLD_COUNT; wt = (WorldType)(wt + (WorldType)1) )
					{
						if( !strcmp( data, WorldTypeNames[wt].c_str() ) )
						{
							visualAppearance = wt;
							break;
						}
					}
					if( visualAppearance == WRLD_COUNT )
						visualAppearance = WRLD_UNKNOWN;
				}
				break;
			case 'b':
			case 'B':
				if( !strcmp( "BUYABLE", tag ) )
				{
					if( actgood >-1 )
						goodbuy[actgood] = makeNum( data );
					else
						Console.Error( 2, "regions dfn -> You must write BUYABLE after GOOD <num>!" );
				}
				break;

			case 'c':
			case 'C':
				if( !strcmp( tag, "COLOURMINSKILL" ) )
					minColourSkill = (SI16)makeNum( data );
				else if( !strcmp( tag, "CHANCEFORBIGORE" ) )
					chanceFindBigOre = (UI08)makeNum( data );
				else if( !strcmp( tag, "CHANCEFORCOLOUR" ) )
					chanceColourOre = (UI08)makeNum( data );
				break;

			case 'd':
			case 'D':
				if( !strcmp( tag, "DUNGEON" ) )
				{
					if( makeNum( data ) != 0 )
						priv |= 0x80;
				}
				break;
			case 'e':
			case 'E':
				if( !strcmp( "ESCORTS", tag ) )
				{
					// Load the region number in the global array of valid escortable regions
					if( makeNum( data ) == 1 )
					{
						// Store the region index into the valid escort region array
						validEscortRegion[escortRegions] = regionNum;
						escortRegions++;
					}
				} // End - Dupois
				break;

			case 'g':
			case 'G':
				if( !strcmp( "GUARDNUM", tag ) )
				{
					if( a < 10 )
					{
						if( a >= guards.size() )
							guards.resize( a + 1 );

						guards[a++] = makeNum( data );
					}
					else
						Console.Error( 2, "Region %i has more than 10 'GUARDNUM'.  The ones after 10 will be discarded", regionNum );
				}
				else if( !strcmp( "GUARDOWNER", tag ) )
					strcpy( guardowner, data );
				else if( !strcmp( "GUARDED", tag ) )
				{
					if( makeNum( data ) ) 
						priv |= 0x01;   
				}
				else if( !strcmp( "GATE", tag ) )
				{
					if( makeNum( data ) ) 
						priv |= 0x04;
				}
				else if( !strcmp( "GOOD", tag ) )
					actgood = makeNum( data );
				break;
			case 'm':
			case 'M':
				if( !strcmp("MIDILIST", tag ) )
					midilist = makeNum( data );
				else if( !strcmp("MAGICDAMAGE", tag ) )
				{
					if( !makeNum( data ) )
						priv |= 0x40;
				}
				else if( !strcmp("MARK", tag ) )
				{
					if( makeNum( data ) )
						priv |= 0x02;
				}
				break;
			case 'n':
			case 'N':
				if( !strcmp("NAME", tag ) )
				{
					strcpy( name, data );
					actgood = -1; // Magius(CHE)
				}
				break;
			case 'o':
			case 'O':
				if( !strcmp( "OREPREF", tag ) )
				{
					const char *numPart;
					char oreName[512];
					orePref toPush;
					numPart = data;
					while( (*numPart) != ' ' && (*numPart) != 0x00 && (*numPart) != 0x13 && (*numPart) != 0x10 )
						numPart++;
					strncpy( oreName, data, numPart - data );
					oreName[numPart-data] = 0x00;
					toPush.oreIndex = Skills->GetOreIndex( oreName );
					if( toPush.oreIndex != -1 )
					{
						if( *numPart != ' ' )
							toPush.percentChance = 100 / Skills->GetNumberOfOres();
						else
							toPush.percentChance = static_cast<UI08>(makeNum( numPart ));
						orePreferences.push_back( toPush );
						orePrefLoaded = true;
					}
					else
						Console.Error( 2, "Invalid ore preference in region %i as %s", regionNum, oreName );
				}
				break;
			case 'r':
			case 'R':

				if( !strcmp( "RECALL", tag ) )
				{
					if( makeNum( data ) ) 
						priv |= 0x08;
				}
				else if( !strcmp( "RANDOMVALUE", tag ) )
				{
					if( actgood > -1 )
					{
						char temp[256];
						gettokennum( data, 0, temp );
						goodrnd1[actgood] = makeNum( temp );
						gettokennum( data, 1, temp );
						goodrnd2[actgood] = makeNum( temp );
						if( goodrnd2[actgood] < goodrnd1[actgood] )
						{
							Console.Error( 2, " regions dfn -> You must write RANDOMVALUE NUM2[%i] greater than NUM1[%i].", goodrnd2[actgood], goodrnd1[actgood] );
							goodrnd2[actgood] = goodrnd1[actgood] = 0;
						}
					}
					else
						Console.Error( 2, " regions dfn -> You must write RANDOMVALUE after GOOD <num>!" );
				}
				else if( !strcmp( "RACE", tag ) )
					race = static_cast<RACEID>(makeNum( data ));
				break;

			case 's':
			case 'S':
				if( !strcmp( "SELLABLE", tag ) )
				{
					if( actgood > -1 )
						goodsell[actgood] = makeNum( data );
					else
						Console.Error( 2, " regions dfn -> You must write SELLABLE after GOOD <num>!" );
				}
				else if( !strcmp( "SPAWN", tag ) )
				{
					cSpawnRegion *spawnReg = spawnregion[totalspawnregions];
					if( spawnReg != NULL )
					{
						spawnReg->SetX1( location[l-1].x1 );
						spawnReg->SetY1( location[l-1].y1 );
						spawnReg->SetX2( location[l-1].x2 );
						spawnReg->SetY2( location[l-1].y2 );
						loadPreDefSpawnRegion( totalspawnregions, data );
					}
					totalspawnregions++;
				}
				break;

			case 'w':
			case 'W':
				if( !strcmp( tag, "WORLD" ) )
					worldNumber = (UI08)makeNum( data );
				break;
			case 'x':
			case 'X':
				if( !strcmp( "X1", tag ) )
					location[l].x1 = (SI16)makeNum( data );
				else if( !strcmp( "X2", tag ) )
					location[l].x2 = (SI16)makeNum( data );
				break;
			case 'y':
			case 'Y':

				if( !strcmp( "Y1", tag ) )
					location[l].y1 = (SI16)makeNum( data );
				else if( !strcmp( "Y2", tag ) )
				{
					location[l].y2 = (SI16)makeNum( data );
					location[l].region = regionNum;
					l++;
				}
				break;
			}
		}
	}
	if( !orePrefLoaded )
	{
		orePref toLoad;
		int numOres = Skills->GetNumberOfOres();
		for( int myCounter = 0; myCounter < Skills->GetNumberOfOres(); myCounter++ )
		{
			toLoad.oreIndex = myCounter;
			toLoad.percentChance = (UI08)(100 / numOres);
			orePreferences.push_back( toLoad );
		}
	}
	return true;
}

bool cTownRegion::IsGuarded( void ) const
{
	return ( ( priv&0x01 ) == 0x01 );
}

bool cTownRegion::CanMark( void ) const
{
	return ( ( priv&0x02 ) == 0x02 );
}

bool cTownRegion::CanGate( void ) const
{
	return ( ( priv&0x04 ) == 0x04 );
}

bool cTownRegion::CanRecall( void ) const
{
	return ( ( priv&0x08 ) == 0x08 );
}

bool cTownRegion::CanCastAggressive( void ) const
{
	return ( ( priv&0x40 ) == 0x40 );
}

const char *cTownRegion::GetName( void ) const
{
	return name;
}
const char *cTownRegion::GetOwner( void ) const
{
	return guardowner;
}

weathID cTownRegion::GetWeather( void ) const
{
	return weather;
}

SI32 cTownRegion::GetGoodSell( UI08 index ) const
{
	return goodsell[index];
}
SI32 cTownRegion::GetGoodBuy( UI08 index ) const
{
	return goodbuy[index];
}
SI32 cTownRegion::GetGoodRnd1( UI08 index ) const
{
	return goodrnd1[index];
}
SI32 cTownRegion::GetGoodRnd2( UI08 index ) const
{
	return goodrnd2[index];
}

SI32 cTownRegion::GetMidiList( void ) const
{
	return midilist;
}

long cTownRegion::GetRandomGuard( void )
{
	int guardCount = guards.size();
	if( guardCount == 0 )
		return 1000;
	else
		return ( guards[rand()%guardCount] );
}

bool cTownRegion::DisplayTownMenu( CItem *used, cSocket *sock, SI08 flag )
{
	if( flag == MAYOR )
	{
		SendMayorGump( sock );
		return true;
	}
	CChar *tChar = sock->CurrcharObj();
	if( !IsMemberOfTown( tChar ) )
	{
		if( Races->CompareByRace( tChar->GetRace(), race ) > 0 )	// if we're racial enemies
		{
			if( tChar->GetTown() != 255 )
				SendEnemyGump( sock );
			else
				sysmessage( sock, 1125 );
			return true;
		}
		else if( tChar->GetTown() != 255 ) // another town person
		{
			if( tChar->GetTown() == regionNum )	// they think we're in this region!!!
			{
				if( !AddAsTownMember( (*tChar) ) )
				{
					sysmessage( sock, 1126 );
					return false;
				}
				DisplayTownMenu( used, sock, flag );
				return true;
			}
			SendBasicInfo( sock );
			return true;
		}
		else
		{
			SendPotentialMember( sock );
			return true;
		}
	}	

	SendDefaultGump( sock );
	return true;
}

bool cTownRegion::IsMemberOfTown( CChar *player ) const
{
	if( player == NULL )
		return false;
	for( UI32 counter = 0; counter < townMember.size(); counter++ )
	{
		if( townMember[counter].townMember == player->GetSerial() )
			return true;
	}
	return false;
}

void cTownRegion::SendEnemyGump( cSocket *sock )
{
	char temp[512];

	STRINGLIST one, two;
	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( temp, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 280 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( temp );
	sprintf( temp, "text 70 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( temp );
	one.push_back( "page 1" );

	two.push_back( "Enemy" );
	sprintf( temp, "gumppic 25 50 1141" );	// town name
	one.push_back( temp );
	sprintf( temp, "text 35 51 %i 1", cwmWorldState->ServerData()->GetRightTextColour() );	// town name
	one.push_back( temp );
	sprintf( temp, "text 25 71 %i 2", cwmWorldState->ServerData()->GetRightTextColour() );	// population
	one.push_back( temp );
	sprintf( temp, "text 55 111 %i 3", cwmWorldState->ServerData()->GetRightTextColour() );	// Seize townstone
	one.push_back( temp );
	sprintf( temp, "text 55 131 %i 4", cwmWorldState->ServerData()->GetRightTextColour() );	// Destroy townstone
	one.push_back( temp );

	sprintf( temp, "button 25 111 %i %i 1 0 61", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// seize townstone
	one.push_back( temp );
	sprintf( temp, "button 25 131 %i %i 1 0 62", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// destroy townstone
	one.push_back( temp );
	sprintf( temp, "%s (%s)", name, Races->Name( race ) );
	two.push_back( temp );
	sprintf( temp, "Population %i", GetPopulation() );
	two.push_back( temp );
	two.push_back( "Seize Townstone" );
	two.push_back( "Attack Townstone" );

	SendVecsAsGump( sock, one, two, 3, INVALIDSERIAL );
}
void cTownRegion::SendBasicInfo( cSocket *sock )
{
	GumpDisplay BasicGump( sock );
	BasicGump.SetTitle( "Basic Townstone gump" );
	BasicGump.Send( 4, false, INVALIDSERIAL );
}
void cTownRegion::SendPotentialMember( cSocket *sock )
{
	STRINGLIST one, two;
	char temp[512];
	UnicodeTypes sLang	= sock->Language();

	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( temp, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 280 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( temp );
	sprintf( temp, "text 70 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( temp );
	one.push_back( "page 1" );

	sprintf( temp, "Outsider" );	// our title
	two.push_back( temp );
	sprintf( temp, "gumppic 25 50 1141" );	// town name
	one.push_back( temp );
	sprintf( temp, "text 35 51 %i 1", cwmWorldState->ServerData()->GetRightTextColour() );	// town name
	one.push_back( temp );
	sprintf( temp, "text 25 71 %i 2", cwmWorldState->ServerData()->GetRightTextColour() );	// population
	one.push_back( temp );
	sprintf( temp, "text 55 91 %i 3", cwmWorldState->ServerData()->GetRightTextColour() );	// join town
	one.push_back( temp );
	sprintf( temp, "text 55 111 %i 4", cwmWorldState->ServerData()->GetRightTextColour() );	// view taxes (to help make decisions about joining?)
	one.push_back( temp );

	sprintf( temp, "button 25 91 %i %i 1 0 41", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// leave town
	one.push_back( temp );
	sprintf( temp, "button 25 111 %i %i 1 0 3", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// view taxes
	one.push_back( temp );

	sprintf( temp, "%s (%s)", name, Races->Name( race ) );
	two.push_back( temp );
	sprintf( temp, Dictionary->GetEntry( 1127, sLang ), GetPopulation() );
	two.push_back( temp );
	two.push_back( Dictionary->GetEntry( 1128, sLang ) );
	two.push_back( Dictionary->GetEntry( 1129, sLang ) );

	SendVecsAsGump( sock, one, two, 3, INVALIDSERIAL );
}

void cTownRegion::SendMayorGump( cSocket *sock )
{
	STRINGLIST one, two;
	char temp[512];
	UnicodeTypes sLang	= sock->Language();

	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( temp, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 280 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( temp );
	sprintf( temp, "text 70 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( temp );
	one.push_back( "page 1" );

	two.push_back( "Mayor Controls" );	// our title

	one.push_back( "gumppic 25 50 1141" );	// town name
	one.push_back( "gumppic 25 260 1141" );
	sprintf( temp, "text 35 51 %i 1", cwmWorldState->ServerData()->GetRightTextColour() );	// town name
	one.push_back( temp );
	sprintf( temp, "text 25 71 %i 2", cwmWorldState->ServerData()->GetRightTextColour() );	// population
	one.push_back( temp );
	sprintf( temp, "text 55 91 %i 3", cwmWorldState->ServerData()->GetRightTextColour() ); // set taxes
	one.push_back( temp );
	sprintf( temp, "text 55 280 %i 4", cwmWorldState->ServerData()->GetRightTextColour() ); // return to main menu
	one.push_back( temp );
	sprintf( temp, "text 55 111 %i 5", cwmWorldState->ServerData()->GetRightTextColour() ); // list town members
	one.push_back( temp );
	sprintf( temp, "text 55 131 %i 6", cwmWorldState->ServerData()->GetRightTextColour() ); // force early election
	one.push_back( temp );
	sprintf( temp, "text 55 151 %i 7", cwmWorldState->ServerData()->GetRightTextColour() ); // purchase more guards
	one.push_back( temp );
	sprintf( temp, "text 55 171 %i 8", cwmWorldState->ServerData()->GetRightTextColour() ); // fire a guard
	one.push_back( temp );
	sprintf( temp, "text 55 261 %i 9", cwmWorldState->ServerData()->GetRightTextColour() ); // treasury amount
	one.push_back( temp );
	sprintf( temp, "text 55 191 %i 10", cwmWorldState->ServerData()->GetRightTextColour() );	// make ally
	one.push_back( temp );

	sprintf( temp, "button 25 91 %i %i 1 0 21", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // set taxes 
	one.push_back( temp );
	sprintf( temp, "button 25 111 %i %i 1 0 22", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // list town members 
	one.push_back( temp );
	sprintf( temp, "button 25 131 %i %i 1 0 23", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // force early election
	one.push_back( temp );
	sprintf( temp, "button 25 151 %i %i 1 0 24", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // purchase more guards
	one.push_back( temp );
	sprintf( temp, "button 25 171 %i %i 1 0 25", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // fire a guard
	one.push_back( temp );
	sprintf( temp, "button 25 280 %i %i 1 0 40", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // return to main menu
	one.push_back( temp );
	sprintf( temp, "button 25 191 %i %i 1 0 26", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 ); // make ally of other town
	one.push_back( temp );

	sprintf( temp, "%s (%s)", name, Races->Name( race ) );
	two.push_back( temp );
	sprintf( temp, Dictionary->GetEntry( 1130, sLang ), GetPopulation() );
	two.push_back( temp );
	two.push_back( Dictionary->GetEntry( 1131, sLang ) );
	two.push_back( Dictionary->GetEntry( 1132, sLang ) );
	two.push_back( Dictionary->GetEntry( 1133, sLang ) );
	two.push_back( Dictionary->GetEntry( 1134, sLang ) );
	two.push_back( Dictionary->GetEntry( 1135, sLang ) );
	two.push_back( Dictionary->GetEntry( 1136, sLang ) );
	sprintf( temp, Dictionary->GetEntry( 1137, sLang ), goldReserved );
	two.push_back( temp );
	two.push_back( Dictionary->GetEntry( 1138, sLang ) );

	SendVecsAsGump( sock, one, two, 3, INVALIDSERIAL );
}

void cTownRegion::SendDefaultGump( cSocket *sock )
{
	STRINGLIST one, two;
	char temp[512];

	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( temp, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 280 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( temp );
	sprintf( temp, "text 70 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( temp );
	one.push_back( "page 1" );

	two.push_back( "Generic View" );	// our title
	one.push_back( "gumppic 25 50 1141" );	// town name
	sprintf( temp, "text 35 51 %i 1", cwmWorldState->ServerData()->GetRightTextColour() );	// town name
	one.push_back( temp );
	sprintf( temp, "text 25 71 %i 2", cwmWorldState->ServerData()->GetRightTextColour() );	// population
	one.push_back( temp );
	sprintf( temp, "text 55 91 %i 3", cwmWorldState->ServerData()->GetRightTextColour() );	// leave town
	one.push_back( temp );
	sprintf( temp, "text 55 111 %i 4", cwmWorldState->ServerData()->GetRightTextColour() );	// view taxes
	one.push_back( temp );
	sprintf( temp, "text 55 131 %i 5", cwmWorldState->ServerData()->GetRightTextColour() );	// toggle town title on/off
	one.push_back( temp );
	sprintf( temp, "text 55 151 %i 6", cwmWorldState->ServerData()->GetRightTextColour() );	// vote for mayor
	one.push_back( temp );
	sprintf( temp, "text 55 171 %i 7", cwmWorldState->ServerData()->GetRightTextColour() );	// donate resource
	one.push_back( temp );
	sprintf( temp, "tilepic 205 171 %i", GetResourceID() );				// picture of the resource
	one.push_back( temp );
	sprintf( temp, "text 55 191 %i 8", cwmWorldState->ServerData()->GetRightTextColour() );	// view budget
	one.push_back( temp );
	sprintf( temp, "text 55 211 %i 9", cwmWorldState->ServerData()->GetRightTextColour() );	// view allied towns
	one.push_back( temp );
	sprintf( temp, "text 55 231 %i 10", cwmWorldState->ServerData()->GetRightTextColour() );	// view enemy towns
	one.push_back( temp );

	sprintf( temp, "button 25 91 %i %i 1 0 2", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// leave town
	one.push_back( temp );
	sprintf( temp, "button 25 111 %i %i 1 0 3", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// view taxes
	one.push_back( temp );
	sprintf( temp, "button 25 131 %i %i 1 0 4", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// toggle title
	one.push_back( temp );
	sprintf( temp, "button 25 151 %i %i 1 0 5", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// vote for mayor
	one.push_back( temp );
	sprintf( temp, "button 25 171 %i %i 1 0 6", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// donate gold
	one.push_back( temp );
	sprintf( temp, "button 25 191 %i %i 1 0 7", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// view budget
	one.push_back( temp );
	sprintf( temp, "button 25 211 %i %i 1 0 8", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// view allied towns
	one.push_back( temp );
	sprintf( temp, "button 25 231 %i %i 1 0 9", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );	// view enemy towns
	one.push_back( temp );

	CChar *mChar		= sock->CurrcharObj();
	UnicodeTypes sLang	= sock->Language();
	sprintf( temp, "%s (%s)", name, Races->Name( race ) );
	two.push_back( temp );
	sprintf( temp, Dictionary->GetEntry( 1139, sLang ), GetPopulation() );
	two.push_back( temp );
	two.push_back( Dictionary->GetEntry( 1140, sLang ) );
	two.push_back( Dictionary->GetEntry( 1141, sLang ) );
	sprintf( temp, Dictionary->GetEntry( 1142, sLang ), mChar->GetTownTitle()?"Off":"On" );
	two.push_back( temp );
	two.push_back( Dictionary->GetEntry( 1143, sLang ) );
	two.push_back( Dictionary->GetEntry( 1144, sLang ) );
	two.push_back( Dictionary->GetEntry( 1145, sLang ) );
	two.push_back( Dictionary->GetEntry( 1146, sLang ) );
	two.push_back( Dictionary->GetEntry( 1147, sLang ) );

	if( mChar->GetTownPriv() == 2 || mChar->IsGM() ) // if we've got a mayor (remove isGM check!)
	{
		sprintf( temp, "button 25 281 %i %i 1 0 20", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1 );
		one.push_back( temp );
		sprintf( temp, "text 55 281 %i 11", cwmWorldState->ServerData()->GetLeftTextColour() );
		one.push_back( temp );
		sprintf( temp, Dictionary->GetEntry( 1148, sLang ) );
		two.push_back( temp );
	}
	SendVecsAsGump( sock, one, two, 3, INVALIDSERIAL );

}
SI32 cTownRegion::GetPopulation( void ) const
{
	return townMember.size();
}

void cTownRegion::DisplayTownMembers( cSocket *sock )
{
	GumpDisplay townListing( sock, 300, 300 );
	townListing.SetTitle( Dictionary->GetEntry( 1149, sock->Language() ) );
	CChar *sChar = sock->CurrcharObj();
	for( UI32 counter = 0; counter < townMember.size(); counter++ )
	{
		CChar *townChar = calcCharObjFromSer( townMember[counter].townMember );
		if( townChar != NULL )
		{
			if( sChar->IsGM() )
				townListing.AddData( townChar->GetName(), townChar->GetSerial(), 3 );
			else
				townListing.AddData( townChar->GetName(), " " );	// don't want them to know the serial
		}
		else
		{
			RemoveCharacter( counter );
			counter--;
		}
	}
	townListing.Send( 4, false, INVALIDSERIAL );
}

bool cTownRegion::VoteForMayor( cSocket *sock )
{
	SERIAL serial = sock->GetDWord( 7 );
	CChar *srcChar = sock->CurrcharObj();
	CChar *target = calcCharObjFromSer( serial );
	if( target != NULL )
	{
		if( !IsMemberOfTown( target ) )	// he's not in our town!
		{
			sysmessage( sock, 1150 );
			return false;
		}
		SERIAL serialPos = FindPositionOf( (*srcChar) );
		if( serialPos == INVALIDSERIAL )
		{
			sysmessage( sock, 1151 );
			return false;
		}
		if( townMember[serialPos].targVote != INVALIDSERIAL )
		{
			sysmessage( sock, 1152 );
			return false;
		}
		townMember[serialPos].targVote = target->GetSerial();
		sysmessage( sock, 1153 );
		return true;
	}
	else
	{
		sysmessage( sock, 1154 );
		return false;
	}
}

SERIAL cTownRegion::FindPositionOf( CChar& toAdd )
{
	for( SERIAL counter = 0; counter < townMember.size(); counter++ )
	{
		if( townMember[counter].townMember == toAdd.GetSerial() )
			return counter;
	}
	return INVALIDSERIAL;
}

UI16 cTownRegion::GetResourceID( void ) const
{
	return taxedResource;
}

bool cTownRegion::DonateResource( cSocket *s, SI32 amount )
{
	goldReserved += amount;
	CChar *tChar = s->CurrcharObj();
	if( amount > 1000 )
		sysmessage( s, 1155, tChar->GetName() );
	else if( amount > 750 )
		sysmessage( s, 1156, tChar->GetName() );
	else if( amount > 500 )
		sysmessage( s, 1157, tChar->GetName() );
	else if( amount > 250 )
		sysmessage( s, 1158, tChar->GetName() );
	else
		sysmessage( s, 1159, tChar->GetName() );
	return true;
}

bool cTownRegion::PurchaseGuard( cSocket *sock, UI08 number )
{
	if( number * 10000 < goldReserved )	// if we don't have the cash
	{
		sysmessage( sock, 1160 );
		return false;
	}

	goldReserved -= (number * 10000);
	int maxGuardIndex = guards.size();
	guards.resize( maxGuardIndex + number );

	for( UI32 counter = maxGuardIndex; counter < guards.size(); counter++ )
		guards[counter] = RandomNum( 1000, 1001 );
	return true;
}

bool cTownRegion::ViewBudget( cSocket *sock )
{
	UnicodeTypes sLang = sock->Language();
	GumpDisplay Budget( sock, 300, 300 );
	Budget.SetTitle( Dictionary->GetEntry( 1161, sLang ) );
	Budget.AddData( Dictionary->GetEntry( 1162, sLang ), guardsPurchased );
	Budget.AddData( Dictionary->GetEntry( 1163, sLang ), guards.size() );
	Budget.AddData( Dictionary->GetEntry( 1164, sLang ), guards.size() * 20 );
	Budget.Send( 4, false, INVALIDSERIAL );

	return true;
}

bool cTownRegion::PeriodicCheck( void )
{
	time_t now;
	time( &now );
	if( difftime( now, timeSinceTaxedMembers ) >= cwmWorldState->ServerData()->GetTownTaxPeriod() )
	{
		for( UI32 memberCounter = 0; memberCounter < townMember.size(); memberCounter++ )
		{
			CChar *townMem = calcCharObjFromSer( townMember[memberCounter].townMember );
			if( townMem != NULL )
			{
				UI16 resourceID = GetResourceID();
				UI32 numResources = GetAmount( townMem, resourceID );

				if( taxedAmount > numResources )
				{
					UI32 bankAmount = GetBankCount( townMem, resourceID, 0 );
					if( taxedAmount > ( numResources + bankAmount ) )
						JailSys->JailPlayer( townMem, 900 );
					else
					{
						DeleteQuantity( townMem, resourceID, numResources );
						DeleteBankItem( townMem, resourceID, 0, bankAmount - numResources );
					}
				}
				else
				{
					DeleteQuantity( townMem, resourceID, taxedAmount );
					resourceCollected += taxedAmount;
				}
			}
		}
		timeSinceTaxedMembers = now;
	}
	if( difftime( now, timeSinceGuardsPaid ) >= cwmWorldState->ServerData()->GetTownGuardPayment() )	// init to 1000 seconds
	{
		if( goldReserved < static_cast<SI32>(( 20 * guards.size() )) )
		{
			// insufficient resources
			bool enoughGuards = false;
			while( !enoughGuards )
			{
				guards.resize( guards.size() - 1 );
				enoughGuards = ( goldReserved >= static_cast<SI32>(( 20 * guards.size() ) ));
			}
		}
		goldReserved -= 20 * guards.size();
		timeSinceGuardsPaid = now;
	}

	if( now > timeToNextPoll && townMember.size() != 0 )
	{
		TellMembers( 1165 );
		for( UI32 counter = 0; counter < townMember.size(); counter++ )
			townMember[counter].targVote = INVALIDSERIAL;
		timeToElectionClose = now + cwmWorldState->ServerData()->GetTownNumSecsPollOpen();	// 2 days polls are open
		timeToNextPoll = timeToElectionClose + cwmWorldState->ServerData()->GetTownNumSecsAsMayor();	// secs as mayor over the top of the end of the poll
		CChar *mayor = GetMayor();
		if( mayor != NULL )
			mayor->SetTownpriv( 1 );	// mayor becomes a basic member again, so he can't do anything while the poll is occuring :>
	}

	if( now > timeToElectionClose && townMember.size() != 0 )	// election finished
	{
		TellMembers( 1166 );
		CalcNewMayor();
		timeToElectionClose = timeToNextPoll + 1000;	// just so long as it's after the next election
	}
	return true;
}	

WorldType cTownRegion::GetAppearance( void ) const
{
	return visualAppearance;
}

void cTownRegion::ViewTaxes( cSocket *sock )
{
	STRINGLIST one, two;
	char temp[512];

	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( temp, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 280 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( temp );
	sprintf( temp, "text 70 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( temp );
	one.push_back( "page 1" );


	two.push_back( "Taxes" );	// our title
	sprintf( temp, "gumppic 25 50 1141" );	// town name
	one.push_back( temp );
	sprintf( temp, "text 35 51 %i 1", cwmWorldState->ServerData()->GetRightTextColour() );	// town name
	one.push_back( temp );
	sprintf( temp, "text 35 71 %i 2", cwmWorldState->ServerData()->GetRightTextColour() );	// population
	one.push_back( temp );
	sprintf( temp, "text 35 111 %i 3", cwmWorldState->ServerData()->GetRightTextColour() ); // # of resources
	one.push_back( temp );
	sprintf( temp, "tilepic 5 111 %i", GetResourceID() );				// picture of the resource
	one.push_back( temp );

	sprintf( temp, "%s (%s)", name, Races->Name( race ) );
	two.push_back( temp );
	sprintf( temp, "Population %i", GetPopulation() );
	two.push_back( temp );
	CTile tile;
	Map->SeekTile( GetResourceID(), &tile );
	sprintf(  temp, "%i %ss", taxedAmount, tile.Name() );
	two.push_back( temp );

	SendVecsAsGump( sock, one, two, 3, INVALIDSERIAL );
}

SI16 cTownRegion::GetHealth( void ) const
{
	return health;
}

void cTownRegion::DoDamage( SI16 reduction )
{
	health -= reduction;
	if( health < 0 )
		health = 0;
	if( health == 0 )
		TellMembers( 1167 );
	else if( health <= 1000 )
		TellMembers( 1168 );
	else if( health <= 5000 )
		TellMembers( 1169 );
	else
		TellMembers( 1170 );
	
}

bool cTownRegion::IsAlliedTown( UI08 townToCheck ) const
{
	for( UI32 counter = 0; counter < alliedTowns.size(); counter++ )
	{
		if( alliedTowns[counter] == townToCheck )
			return true;
	}
	return false;
}

bool cTownRegion::MakeAlliedTown( UI08 townToMake )
{
	if( regionNum == townToMake )
		return false;

	if( IsAlliedTown( townToMake ) )	// already allied
	{
		TellMembers( 1171 );
		return false;
	}

	if( Races->CompareByRace( region[townToMake]->GetRace(), race ) == 1 )	// if we're racial enemies
		return false;

	int location = alliedTowns.size();
	alliedTowns.resize( location + 1 );
	alliedTowns[location] = townToMake;	// let's ally ourselves
	TellMembers( 1172, name, region[townToMake]->GetName() );
	return true;

}

void cTownRegion::TellMembers( SI32 dictEntry, ...) // System message (In lower left corner)
{
	char msg[512];
	char tmpMsg[512];

	for( UI32 memberCounter = 0; memberCounter < townMember.size(); memberCounter++ )
	{
		CChar *targetChar = calcCharObjFromSer( townMember[memberCounter].townMember );
		cSocket *targetSock = calcSocketObjFromChar( targetChar );
		if( targetSock != NULL )
		{
			const char *txt = Dictionary->GetEntry( dictEntry, targetSock->Language() );

			strcpy( msg, "TOWN: " );
			va_list argptr;
#ifdef __NONANSI_VASTART__
			va_start( argptr );
#else
			va_start( argptr, dictEntry );
#endif
			vsprintf( tmpMsg, txt, argptr );
			va_end( argptr );
			strcat( msg, tmpMsg );

			CSpeechEntry *toAdd = SpeechSys->Add();
			toAdd->Speech( msg );
			toAdd->Font( FNT_NORMAL );
			toAdd->Speaker( INVALIDSERIAL );
			toAdd->SpokenTo( townMember[memberCounter].townMember );
			toAdd->Colour( 0x000B );
			toAdd->Type( SYSTEM );
			toAdd->At( uiCurrentTime );
			toAdd->TargType( SPTRG_INDIVIDUAL );
		}
	}
}

RACEID cTownRegion::GetRace( void ) const
{
	return race;
}

void cTownRegion::SendAlliedTowns( cSocket *sock )
{
	GumpDisplay Ally( sock, 300, 300 );
	char temp[100];
	sprintf( temp, Dictionary->GetEntry( 1173, sock->Language() ), alliedTowns.size() );
	Ally.SetTitle( temp );
	for( UI32 counter = 0; counter < alliedTowns.size(); counter++ )
		Ally.AddData( region[alliedTowns[counter]]->GetName(), " " );

	Ally.Send( 4, false, INVALIDSERIAL );
}

void cTownRegion::ForceEarlyElection( void )
{
	time_t now;
	time(&now);
	CChar *mayor = GetMayor();
	timeToNextPoll = now;	// time to open poll
	TellMembers( 1174 );
	for( UI32 counter = 0; counter < townMember.size(); counter++ )
		townMember[counter].targVote = INVALIDSERIAL;
	if( mayor != NULL )
		mayor->SetTownpriv( 1 );
}

void cTownRegion::SendEnemyTowns( cSocket *sock )
{
	GumpDisplay Enemy( sock, 300, 300 );
	char temp[100];
	UI08 enemyCount = 0;
	for( UI16 counter = 0; counter < 256; counter++ )
	{
		if( counter != regionNum && Races->CompareByRace( race, region[counter]->GetRace() ) == 1 )	// if we're racial enemies, and not the same as ourselves
		{
			enemyCount++;
			Enemy.AddData( region[counter]->GetName(), Races->Name( region[counter]->GetRace() ) );
		}
	}
	sprintf( temp, "Enemy Towns (%i)", enemyCount );
	Enemy.SetTitle( temp );
	Enemy.Send( 4, false, INVALIDSERIAL );
}

void cTownRegion::Possess( UI08 possessorTown )
{
	region[possessorTown]->SetRace( race );
	region[possessorTown]->TellMembers( 1175 );
	region[possessorTown]->SetReserves( region[possessorTown]->GetReserves() + GetReserves() / 3 * 2 );
	region[possessorTown]->SetTaxesLeft( region[possessorTown]->GetTaxes() + GetTaxes() / 3 * 2 );
	TellMembers( 1176 );
	MakeAlliedTown( possessorTown );
	region[possessorTown]->MakeAlliedTown( regionNum );

	// remove the old members, preparing the way for the new ones
	CChar *targChar;
	for( int counter = townMember.size() - 1; counter >= 0; counter++ )
	{
		targChar = calcCharObjFromSer( townMember[counter].townMember );
		RemoveCharacter( counter );
		if( targChar != NULL )
		{
			targChar->SetTown( 255 );
			targChar->SetTownpriv( 0 );
		}
	}
	townMember.resize( 0 );
}

long cTownRegion::GetReserves( void ) const
{
	return resourceCollected;
}
long cTownRegion::GetTaxes( void ) const
{
	return goldReserved;
}
void cTownRegion::SetTaxesLeft( long newValue )
{
	goldReserved = newValue;
}
void cTownRegion::SetReserves( long newValue )
{
	resourceCollected = newValue;
}
void cTownRegion::SetRace( RACEID newRace )
{
	race = newRace;
}
SI16 cTownRegion::GetMinColourSkill( void ) const
{
	return minColourSkill;
}
UI08 cTownRegion::GetChanceBigOre( void ) const
{
	return chanceFindBigOre;
}
UI08 cTownRegion::GetChanceColourOre( void ) const
{
	return chanceColourOre;
}
bool cTownRegion::RemoveCharacter( int position )
{
#pragma note( "Town banks don't exist yet, so don't delete them!" )
//	Items->DeleItem( calcItemFromSer( townMember[position].PlayerBank->GetSerial() ) );
	for( unsigned int counter2 = position; counter2 < ( townMember.size() - 1 ); counter2++ )
	{
		townMember[counter2].townMember = townMember[counter2+1].townMember;
		townMember[counter2].targVote   = townMember[counter2+1].targVote;
		townMember[counter2].PlayerBank = townMember[counter2+1].PlayerBank;
	}
	townMember.resize( townMember.size() - 1 );
	return true;
}
SI32 cTownRegion::GetNumOrePreferences( void ) const
{
	return orePreferences.size();
}
const orePref *cTownRegion::GetOrePreference( SI32 targValue ) const
{
	if( targValue < 0 || targValue >= static_cast<SI32>(orePreferences.size()) )
		return NULL;
	return &orePreferences[targValue];
}

SI32 cTownRegion::GetOreChance( void ) const
{
	int sumReturn = 0;
	for( UI32 i = 0; i < orePreferences.size(); i++ )
		sumReturn += orePreferences[i].percentChance;
	return sumReturn;
}
bool cTownRegion::IsDungeon( void ) const
{
	return( (priv&0x80) == 0x80 );
}

UI32 cTownRegion::NumGuards( void ) const
{
	return guards.size();
}

UI08 cTownRegion::WorldNumber( void ) const
{
	return worldNumber;
}

void cTownRegion::IsGuarded( bool value )
{
	if( value )
		priv |= 0x01;
	else
		priv &= 0xFE;
}
void cTownRegion::CanMark( bool value )
{
	if( value )
		priv |= 0x02;
	else
		priv &= 0xFD;
}
void cTownRegion::CanGate( bool value )
{
	if( value )
		priv |= 0x04;
	else
		priv &= 0xFB;
}
void cTownRegion::CanRecall( bool value )
{
	if( value )
		priv |= 0x08;
	else
		priv &= 0xF7;
}
void cTownRegion::CanCastAggressive( bool value )
{
	if( value )
		priv |= 0x40;
	else
		priv &= 0xBF;
}
void cTownRegion::IsDungeon( bool value )
{
	if( value )
		priv |= 0x80;
	else
		priv &= 0x7F;
}
void cTownRegion::SetName( const char *toSet )
{
	strncpy( name, toSet, 49 );
	name[49] = 0;
}

void cTownRegion::TaxedAmount( UI16 amount )
{
	taxedAmount = amount;
}

UI16 cTownRegion::TaxedAmount( void ) const
{
	return taxedAmount;
}

void cTownRegion::SetResourceID( UI16 resID )
{
	taxedResource = resID;
}

void cTownRegion::SetHealth( SI16 newValue )
{
	health = newValue;
}

void cTownRegion::SetChanceBigOre( UI08 newValue )
{
	chanceFindBigOre = newValue;
}
void cTownRegion::SetChanceColourOre( UI08 newValue )
{
	chanceColourOre = newValue;
}
