#include "uox3.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "cSpawnRegion.h"
#include "skills.h"
#include "speech.h"
#include "ssection.h"
#include "gump.h"
#include "mapstuff.h"
#include "scriptc.h"
#include "CPacketSend.h"
#include "jail.h"
#include "Dictionary.h"
#include "classes.h"

namespace UOX
{

// Implementation of town regions

const SI08 MAYOR = 0x01;
const SI08 ENEMY = 0x02;
const SI08 JOINER = 0x03;

const RACEID	DEFTOWN_RACE				= 0;
const weathID	DEFTOWN_WEATHER				= 255;
const UI08		DEFTOWN_PRIV				= 0;
const SI32		DEFTOWN_MIDILIST			= 0;
const SERIAL	DEFTOWN_MAYOR				= INVALIDSERIAL;
const UI16		DEFTOWN_TAXEDRESOURCE		= 0x0EED;
const UI16		DEFTOWN_TAXEDAMOUNT			= 0;
const SI32		DEFTOWN_GOLDRESERVED		= 0;
const SI16		DEFTOWN_GUARDSPURCHASED		= 0;
const long		DEFTOWN_RESOURCECOLLECTED	= 0;
const WorldType	DEFTOWN_VISUALAPPEARANCE	= WRLD_SPRING;
const SI16		DEFTOWN_HEALTH				= 30000;
const long		DEFTOWN_ELECTIONCLOSE		= 0;
const long		DEFTOWN_NEXTPOLL			= 0;
const long		DEFTOWN_GUARDSPAID			= 0;
const long		DEFTOWN_TAXEDMEMBERS		= 0;
const UI08		DEFTOWN_WORLDNUMBER			= 0;
const UI16		DEFTOWN_JSSCRIPT			= 0xFFFF;
const SI16		DEFTOWN_MINCOLOURSKILL		= 0;
const UI08		DEFTOWN_FINDBIGORE			= 0;
const UI08		DEFTOWN_FINDCOLOURORE		= 0;
const UI16		DEFTOWN_NUMGUARDS			= 10;

CTownRegion::CTownRegion( UI08 region ) : race( DEFTOWN_RACE ), weather( DEFTOWN_WEATHER ), priv( DEFTOWN_PRIV ), 
regionNum( region ), midilist( DEFTOWN_MIDILIST ), mayorSerial( DEFTOWN_MAYOR ), taxedResource( DEFTOWN_TAXEDRESOURCE ), 
taxedAmount( DEFTOWN_TAXEDAMOUNT ), goldReserved( DEFTOWN_GOLDRESERVED ), guardsPurchased( DEFTOWN_GUARDSPURCHASED ),
resourceCollected( DEFTOWN_RESOURCECOLLECTED ), visualAppearance( DEFTOWN_VISUALAPPEARANCE ), health( DEFTOWN_HEALTH ), 
timeToElectionClose( DEFTOWN_ELECTIONCLOSE ), timeToNextPoll( DEFTOWN_NEXTPOLL ), timeSinceGuardsPaid( DEFTOWN_GUARDSPAID ), 
timeSinceTaxedMembers( DEFTOWN_TAXEDMEMBERS ), worldNumber( DEFTOWN_WORLDNUMBER ), jsScript( DEFTOWN_JSSCRIPT ), 
minColourSkill( DEFTOWN_MINCOLOURSKILL ), chanceFindBigOre( DEFTOWN_FINDBIGORE ), chanceColourOre( DEFTOWN_FINDCOLOURORE ),
numGuards( DEFTOWN_NUMGUARDS )
{
	townMember.resize( 0 );
	alliedTowns.resize( 0 );
	orePreferences.resize( 0 );
	locations.resize( 0 );
	name		= Dictionary->GetEntry( 1117 );
	guardowner	= Dictionary->GetEntry( 1118 );
	guardList	= "guard";
	goodList.clear();
}

CTownRegion::~CTownRegion()
{
	townMember.resize( 0 );
	alliedTowns.resize( 0 );
	orePreferences.resize( 0 );
	locations.resize( 0 );
}

bool CTownRegion::Load( Script *ss )
// ss is a script section containing all the data!
{
	size_t location = 0xFFFFFFFF;
	UString tag;
	UString data;
	UString UTag;
	UString sect = "TOWNREGION " + UString::number( regionNum );
	if( !ss->isin( sect ) )	// doesn't exist
		return false;

	ScriptSection *target = ss->FindEntry( sect );
	for( tag = target->First(); !target->AtEnd(); tag = target->Next() )
	{
		UTag = tag.upper();
		data = target->GrabData();
		switch( (tag.data()[0]) )
		{
			case 'A':
				if( UTag == "ALLYTOWN" )
					alliedTowns.push_back( data.toUByte() );
				break;
			case 'E':
				if( UTag == "ELECTIONTIME" )
					timeToElectionClose = data.toLong();
				break;
			case 'G':
				if( UTag == "GUARDOWNER" )
					guardowner = data;
				else if( UTag == "GUARD" )	// load our purchased guard
					++numGuards;
				else if( UTag == "GUARDSBOUGHT" ) // num guards bought
					guardsPurchased = data.toShort();
				break;
			case 'H':
				if( UTag == "HEALTH" )
					health = data.toShort();
				break;
			case 'M':
				if( UTag == "MEMBER" )
				{
					location = townMember.size();
					townMember.resize( location + 1 );
					townMember[location].targVote = INVALIDSERIAL;
					townMember[location].townMember = data.toULong();
				}
				else if( UTag == "MAYOR" )
					mayorSerial = data.toULong();
				break;
			case 'N':
				if( UTag == "NAME" )
					name = data.substr( 0, 49 );
				else if( UTag == "NUMGUARDS" )
					numGuards = data.toUShort();
				break;
			case 'P':
				if( UTag == "PRIV" )
					priv = data.toUByte();
				else if( UTag == "POLLTIME" )
					timeToNextPoll = data.toLong();
				break;
			case 'R':
				if( UTag == "RACE" )
					race = data.toUShort();
				else if( UTag == "RESOURCEAMOUNT" )
					goldReserved = data.toLong();
				else if( UTag == "RESOURCECOLLECTED" )
					resourceCollected = data.toLong();
				break;
			case 'T':
				if( UTag == "TAXEDID" )
					taxedResource = data.toUShort();
				else if( UTag == "TAXEDAMOUNT" )
					taxedAmount = data.toUShort();
				else if( UTag == "TIMET" )
					timeSinceTaxedMembers = data.toLong();
				else if( UTag == "TIMEG" )
					timeSinceGuardsPaid = data.toLong();
				break;
			case 'V':
				if( UTag == "VOTE" && location != 0xFFFFFFFF )
					townMember[location].targVote = data.toULong();
				break;
			case 'W':
				if( UTag == "WEATHER" )
					weather = data.toUByte();
				else if( UTag == "WORLD" )
					worldNumber = data.toUByte();
				break;
		}
	}
	return true;
}
bool CTownRegion::Save( std::ofstream &outStream )
// entry is the region #, fp is the file to save in
{
	outStream << "[TOWNREGION " << static_cast<UI16>(regionNum) << "]" << std::endl << "{" << std::endl;
	outStream << "WEATHER=" << static_cast<UI16>(weather) << std::endl;
	outStream << "RACE=" << race << std::endl;
	outStream << "GUARDOWNER=" << guardowner << std::endl;
	outStream << "MAYOR=" << std::hex << "0x" << mayorSerial << std::dec << std::endl;
	outStream << "PRIV=" << static_cast<UI16>(priv) << std::endl;
	outStream << "RESOURCEAMOUNT=" << goldReserved << std::endl;
	outStream << "TAXEDID=" << std::hex << "0x" << taxedResource << std::dec << std::endl;
	outStream << "TAXEDAMOUNT=" << taxedAmount << std::endl;
	outStream << "GUARDSPURCHASED=" << guardsPurchased << std::endl;
	outStream << "TIMEG=" << timeSinceGuardsPaid << std::endl;
	outStream << "TIMET=" << timeSinceTaxedMembers << std::endl;
	outStream << "RESOURCECOLLECTED=" << resourceCollected << std::endl;
	outStream << "HEALTH=" << health << std::endl;
	outStream << "ELECTIONTIME=" << timeToElectionClose << std::endl;
	outStream << "POLLTIME=" << timeToNextPoll << std::endl;
	outStream << "WORLD=" << static_cast<UI16>(worldNumber) << std::endl;
	outStream << "NUMGUARDS=" << numGuards << std::endl;

	std::vector< townPers >::const_iterator mIter;
	for( mIter = townMember.begin(); mIter != townMember.end(); ++mIter )
	{
		outStream << "MEMBER=" << std::hex << "0x" << (*mIter).townMember << std::endl;
		outStream << "VOTE=" << "0x" << (*mIter).targVote << std::dec << std::endl;
	}
	std::vector< UI08 >::const_iterator aIter;
	for( aIter = alliedTowns.begin(); aIter != alliedTowns.end(); ++aIter )
	{
		outStream << "ALLYTOWN=" << static_cast<UI16>((*aIter)) << std::endl;
	}
	outStream << "}" << std::endl << std::endl;
	return true;
}
void CTownRegion::CalcNewMayor( void )
// There has got to be a better way than this hideous O(n^2) algy
{
	if( townMember.empty() )
		return;
	// if there are no members, there are no new mayors
	std::vector< int > votes;
	votes.resize( townMember.size() );
	for( size_t counter = 0; counter < votes.size(); ++counter )
	{
		votes[counter] = 0;	// init the count before adding
		for( size_t counter2 = 0; counter2 < votes.size(); ++counter2 )
		{
			if( townMember[counter].townMember == townMember[counter2].targVote )
				++votes[counter];
		}
	}
	size_t maxIndex = 0;
	for( size_t indexCounter = 1; indexCounter < votes.size(); ++indexCounter )
	{
		if( votes[indexCounter] > votes[maxIndex] )
			maxIndex = indexCounter;
	}
	// maxIndex is now our new mayor!
	CChar *oldMayor = GetMayor();
	if( mayorSerial == townMember[maxIndex].townMember )
	{
		CSocket *targSock = calcSocketObjFromChar( calcCharObjFromSer( mayorSerial ) );
		if( targSock != NULL )
			targSock->sysmessage( 1119 );
		// welcome the mayor back for another term
	}
	if( votes[maxIndex] > 0 )
		mayorSerial = townMember[maxIndex].townMember;
	else
		mayorSerial = 0xFFFFFFF;

	CChar *newMayor = GetMayor();
	if( ValidateObject( oldMayor ) )
		oldMayor->SetTownpriv( 1 );
	if( ValidateObject( newMayor ) )
		newMayor->SetTownpriv( 2 );	// set mayor priv last
	if( newMayor != oldMayor )
	{
		const bool oldMayorExists = ValidateObject( oldMayor );
		const bool newMayorExists = ValidateObject( newMayor );
		if( !oldMayorExists && newMayorExists )
			TellMembers( 1120, newMayor->GetName().c_str() );
		else if( oldMayorExists && !newMayorExists )
			TellMembers( 1121 );
		else if( oldMayorExists && newMayorExists )
			TellMembers( 1122, oldMayor->GetName().c_str(), newMayor->GetName().c_str() );
		else
			TellMembers( 1123 );
	}
	else if( ValidateObject( newMayor ) )
		TellMembers( 1124, oldMayor->GetName().c_str() );
	else
		TellMembers( 1123 );
}
CChar * CTownRegion::GetMayor( void )
// returns the mayor character
{
	return calcCharObjFromSer( mayorSerial );
}

SERIAL CTownRegion::GetMayorSerial( void ) const
// returns the mayor's serial
{
	return mayorSerial;
}

bool CTownRegion::AddAsTownMember( CChar& toAdd )
// toAdd is the character to add
{
	if( Races->CompareByRace( toAdd.GetRace(), race ) == 1 )	// if we're racial enemies
		return false;	// we can't have a racial enemy in the town!

	for( UI32 counter = 0; counter < townMember.size(); ++counter )		// exhaustive error checking
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
bool CTownRegion::RemoveTownMember( CChar& toAdd )
{
	if( toAdd.GetTown() != regionNum )
		return false;	// not in our town

	for( size_t counter = 0; counter < townMember.size(); ++counter )
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

bool CTownRegion::InitFromScript( ScriptSection *toScan )
{
	UString tag;
	UString data;
	UString UTag;
	int actgood				= -1;
	bool orePrefLoaded		= false;
	numGuards			= 10;
	minColourSkill		= 600;
	chanceFindBigOre	= 80;
	chanceColourOre		= 10;
	regLocs ourLoc;
	for( tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
	{
		UTag = tag.upper();
		data = toScan->GrabData();
		switch( (tag.data()[0]) )
		{
			case 'a':
			case 'A':
				if( UTag == "ABWEATH" )
					weather = data.toUByte();
				else if( UTag == "APPEARANCE" )
				{
					visualAppearance = WRLD_COUNT;
					for( WorldType wt = WRLD_SPRING; wt < WRLD_COUNT; wt = (WorldType)(wt + (WorldType)1) )
					{
						if( data == WorldTypeNames[wt] )
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
				if( UTag == "BUYABLE" )
				{
					if( actgood > -1 )
						goodList[actgood].buyVal = data.toLong();
					else
						Console.Error( 2, "regions dfn -> You must write BUYABLE after GOOD <num>!" );
				}
				break;
			case 'c':
			case 'C':
				if( UTag == "COLOURMINSKILL" )
					minColourSkill = data.toShort();
				else if( UTag == "CHANCEFORBIGORE" )
					chanceFindBigOre = data.toUByte();
				else if( UTag == "CHANCEFORCOLOUR" )
					chanceColourOre = data.toUByte();
				break;
			case 'd':
			case 'D':
				if( UTag == "DUNGEON" )
					IsDungeon( (data.toUByte() == 1) );
				break;
			case 'e':
			case 'E':
				if( UTag == "ESCORTS" ) 
				{
					// Load the region number in the global array of valid escortable regions
					if( data.toUByte() == 1 )
						cwmWorldState->escortRegions.push_back( regionNum );
				} // End - Dupois
				break;
			case 'g':
			case 'G':
				if( UTag == "GUARDNUM" )
					break;
				else if( UTag == "GUARDLIST" )
					guardList = data;
				else if( UTag == "GUARDOWNER" )
					guardowner = data;
				else if( UTag == "GUARDED" )
					IsGuarded( (data.toUByte() == 1) );
				else if( UTag == "GATE" )
					CanGate( (data.toUByte() == 1) );
				else if( UTag == "GOOD" )
					actgood = data.toLong();
				break;
			case 'm':
			case 'M':
				if( UTag == "MIDILIST" )
					midilist = data.toUShort();
				else if( UTag == "MAGICDAMAGE" )
					CanCastAggressive( (data.toUByte() == 1) );
				else if( UTag == "MARK" )
					CanMark( (data.toUByte() == 1) );
				break;
			case 'n':
			case 'N':
				if( UTag == "NAME" )
				{
					name = data;
					actgood = -1; // Magius(CHE)
				}
				break;
			case 'o':
			case 'O':
				if( UTag == "OREPREF" )
				{
					UString numPart;
					std::string oreName;
					orePref toPush;
					data			= data.simplifyWhiteSpace();
					oreName			= data.section( " ", 0, 0 );
					toPush.oreIndex = Skills->FindOre( oreName );
					if( toPush.oreIndex != NULL )
					{
						if( data.sectionCount( " " ) == 0 )
							toPush.percentChance = 100 / Skills->GetNumberOfOres();
						else
							toPush.percentChance = data.section( " ", 1, 1 ).toByte();
						orePreferences.push_back( toPush );
						orePrefLoaded = true;
					}
					else
						Console.Error( 2, "Invalid ore preference in region %i as %s", regionNum, oreName.c_str() );
				}
				break;
			case 'r':
			case 'R':
				if( UTag == "RECALL" )
					CanRecall( (data.toUByte() == 1) );
				else if( UTag == "RANDOMVALUE" )
				{
					if( actgood > -1 )
					{
						if( data.sectionCount( " " ) != 0 )
						{
							goodList[actgood].rand1 = data.section( " ", 0, 0 ).toLong();
							goodList[actgood].rand2 = data.section( " ", 1, 1 ).toLong();
						}
						else
						{
							goodList[actgood].rand1 = data.toLong();
							goodList[actgood].rand2 = goodList[actgood].rand1;
						}
						if( goodList[actgood].rand2 < goodList[actgood].rand1 )
						{
							Console.Error( 2, " regions dfn -> You must write RANDOMVALUE NUM2[%i] greater than NUM1[%i].", goodList[actgood].rand2, goodList[actgood].rand1 );
							goodList[actgood].rand2 = goodList[actgood].rand1 = 0;
						}
					}
					else
						Console.Error( 2, " regions dfn -> You must write RANDOMVALUE after GOOD <num>!" );
				}
				else if( UTag == "RACE" )
					race = data.toUShort();
				break;
			case 's':
			case 'S':
				if( UTag == "SELLABLE" )
				{
					if( actgood > -1 )
						goodList[actgood].sellVal = data.toLong();
					else
						Console.Error( 2, " regions dfn -> You must write SELLABLE after GOOD <num>!" );
				}
				else if( UTag == "SPAWN" )
				{
					UString sect = "PREDEFINED_SPAWN " + data;
					ScriptSection *predefSpawn = FileLookup->FindEntry( sect, spawn_def );
					if( predefSpawn == NULL )
						Console.Warning( 2, "Undefined region spawn %s, check your regions.scp and spawn.scp files", data.c_str() );
					else
					{
						UI16 regNum				= static_cast<UI16>(spawnregions.size());
						spawnregions.push_back( new CSpawnRegion( regNum ) );
						CSpawnRegion *spawnReg	= spawnregions[regNum];
						if( spawnReg != NULL )
						{
							spawnReg->SetX1( locations[locations.size() - 1].x1 );
							spawnReg->SetY1( locations[locations.size() - 1].y1 );
							spawnReg->SetX2( locations[locations.size() - 1].x2 );
							spawnReg->SetY2( locations[locations.size() - 1].y2 );
							spawnReg->Load( predefSpawn );
						}
					}
				}
				else if( UTag == "SCRIPT" )
					jsScript = data.toUShort();
				break;
			case 'w':
			case 'W':
				if( UTag == "WORLD" )
					worldNumber = data.toUByte();
				break;
			case 'x':
			case 'X':
				if( UTag == "X1" )
					ourLoc.x1 = data.toShort();
				else if( UTag == "X2" )
					ourLoc.x2 = data.toShort();
				break;
			case 'y':
			case 'Y':
				if( UTag == "Y1" )
					ourLoc.y1 = data.toShort();
				else if( UTag == "Y2" )
				{
					ourLoc.y2 = data.toShort();
					locations.push_back( ourLoc );
				}
				break;
		}
	}
	if( !orePrefLoaded )
	{
		orePref toLoad;
		size_t numOres = Skills->GetNumberOfOres();
		for( size_t myCounter = 0; myCounter < numOres; ++myCounter )
		{
			toLoad.oreIndex			= Skills->GetOre( myCounter );
			toLoad.percentChance	= (UI08)(100 / numOres);
			orePreferences.push_back( toLoad );
		}
	}
	return true;
}

bool CTownRegion::IsGuarded( void ) const
{
	return ( ( priv&0x01 ) == 0x01 );
}

bool CTownRegion::CanMark( void ) const
{
	return ( ( priv&0x02 ) == 0x02 );
}

bool CTownRegion::CanGate( void ) const
{
	return ( ( priv&0x04 ) == 0x04 );
}

bool CTownRegion::CanRecall( void ) const
{
	return ( ( priv&0x08 ) == 0x08 );
}

bool CTownRegion::CanCastAggressive( void ) const
{
	return ( ( priv&0x40 ) == 0x40 );
}

std::string CTownRegion::GetName( void ) const
{
	return name;
}
std::string CTownRegion::GetOwner( void ) const
{
	return guardowner;
}

weathID CTownRegion::GetWeather( void ) const
{
	return weather;
}

SI32 CTownRegion::GetGoodSell( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.sellVal;
	return rVal;
}
SI32 CTownRegion::GetGoodBuy( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.buyVal;
	return rVal;
}
SI32 CTownRegion::GetGoodRnd1( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.rand1;
	return rVal;
}
SI32 CTownRegion::GetGoodRnd2( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.rand2;
	return rVal;
}

UI16 CTownRegion::GetMidiList( void ) const
{
	return midilist;
}

CChar * CTownRegion::GetRandomGuard( void )
{
	CChar *ourGuard = Npcs->CreateRandomNPC( guardList );
	ourGuard->SetNPCAiType( aiGUARD );
	return ourGuard;
}

bool CTownRegion::DisplayTownMenu( CItem *used, CSocket *sock, SI08 flag )
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
				sock->sysmessage( 1125 );
			return true;
		}
		else if( tChar->GetTown() != 255 ) // another town person
		{
			if( tChar->GetTown() == regionNum )	// they think we're in this region!!!
			{
				if( !AddAsTownMember( (*tChar) ) )
				{
					sock->sysmessage( 1126 );
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

bool CTownRegion::IsMemberOfTown( CChar *player ) const
{
	if( !ValidateObject( player ) )
		return false;
	for( size_t counter = 0; counter < townMember.size(); ++counter )
	{
		if( townMember[counter].townMember == player->GetSerial() )
			return true;
	}
	return false;
}

void CTownRegion::SendEnemyGump( CSocket *sock )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.AddCommand( "page 1" );

	toSend.AddText( "Enemy" );
	toSend.AddCommand( "gumppic 25 50 1141" );	// town name
	toSend.AddCommand( "text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() );	// town name
	toSend.AddCommand( "text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour() );	// population
	toSend.AddCommand( "text 55 111 %u 3", cwmWorldState->ServerData()->RightTextColour() );	// Seize townstone
	toSend.AddCommand( "text 55 131 %u 4", cwmWorldState->ServerData()->RightTextColour() );	// Destroy townstone

	toSend.AddCommand( "button 25 111 %u %i 1 0 61", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// seize townstone
	toSend.AddCommand( "button 25 131 %u %i 1 0 62", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// destroy townstone
	toSend.AddText( "%s (%s)", name.c_str(), Races->Name( race ).c_str() );
	toSend.AddText( "Population %i", GetPopulation() );
	toSend.AddText( "Seize Townstone" );
	toSend.AddText( "Attack Townstone" );

	toSend.Finalize();
	sock->Send( &toSend );
}
void CTownRegion::SendBasicInfo( CSocket *sock )
{
	GumpDisplay BasicGump( sock );
	BasicGump.SetTitle( "Basic Townstone gump" );
	BasicGump.Send( 4, false, INVALIDSERIAL );
}
void CTownRegion::SendPotentialMember( CSocket *sock )
{
	UnicodeTypes sLang	= sock->Language();
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.AddCommand( "page 1" );

	toSend.AddText( "Outsider" );	// our title
	toSend.AddCommand( "gumppic 25 50 1141" );	// town name
	toSend.AddCommand( "text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() );	// town name
	toSend.AddCommand( "text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour() );	// population
	toSend.AddCommand( "text 55 91 %u 3", cwmWorldState->ServerData()->RightTextColour() );	// join town
	toSend.AddCommand( "text 55 111 %u 4", cwmWorldState->ServerData()->RightTextColour() );	// view taxes (to help make decisions about joining?)

	toSend.AddCommand( "button 25 91 %u %i 1 0 41", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// leave town
	toSend.AddCommand( "button 25 111 %u %i 1 0 3", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// view taxes

	toSend.AddText( "%s (%s)", name.c_str(), Races->Name( race ).c_str() );

	toSend.AddText( Dictionary->GetEntry( 1127, sLang ), GetPopulation() );
	toSend.AddText( Dictionary->GetEntry( 1128, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1129, sLang ) );

	toSend.Finalize();
	sock->Send( &toSend );
}

void CTownRegion::SendMayorGump( CSocket *sock )
{
	UnicodeTypes sLang	= sock->Language();
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.AddCommand( "page 1" );

	toSend.AddText( "Mayor Controls" );	// our title

	toSend.AddCommand( "gumppic 25 50 1141" );	// town name
	toSend.AddCommand( "gumppic 25 260 1141" );
	toSend.AddCommand( "text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() );	// town name
	toSend.AddCommand( "text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour() );	// population
	toSend.AddCommand( "text 55 91 %u 3", cwmWorldState->ServerData()->RightTextColour() ); // set taxes
	toSend.AddCommand( "text 55 280 %u 4", cwmWorldState->ServerData()->RightTextColour() ); // return to main menu
	toSend.AddCommand( "text 55 111 %u 5", cwmWorldState->ServerData()->RightTextColour() ); // list town members
	toSend.AddCommand( "text 55 131 %u 6", cwmWorldState->ServerData()->RightTextColour() ); // force early election
	toSend.AddCommand( "text 55 151 %u 7", cwmWorldState->ServerData()->RightTextColour() ); // purchase more guards
	toSend.AddCommand( "text 55 171 %u 8", cwmWorldState->ServerData()->RightTextColour() ); // fire a guard
	toSend.AddCommand( "text 55 261 %u 9", cwmWorldState->ServerData()->RightTextColour() ); // treasury amount
	toSend.AddCommand( "text 55 191 %u 10", cwmWorldState->ServerData()->RightTextColour() );	// make ally

	toSend.AddCommand( "button 25 91 %u %i 1 0 21", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // set taxes 
	toSend.AddCommand( "button 25 111 %u %i 1 0 22", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // list town members 
	toSend.AddCommand( "button 25 131 %u %i 1 0 23", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // force early election
	toSend.AddCommand( "button 25 151 %u %i 1 0 24", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // purchase more guards
	toSend.AddCommand( "button 25 171 %u %i 1 0 25", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // fire a guard
	toSend.AddCommand( "button 25 280 %u %i 1 0 40", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // return to main menu
	toSend.AddCommand( "button 25 191 %u %i 1 0 26", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ); // make ally of other town

	toSend.AddText( "%s (%s)", name.c_str(), Races->Name( race ).c_str() );
	toSend.AddText( Dictionary->GetEntry( 1130, sLang ), GetPopulation() );
	toSend.AddText( Dictionary->GetEntry( 1131, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1132, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1133, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1134, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1135, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1136, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1137, sLang ), goldReserved );
	toSend.AddText( Dictionary->GetEntry( 1138, sLang ) );

	toSend.Finalize();
	sock->Send( &toSend );
}

void CTownRegion::SendDefaultGump( CSocket *sock )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.AddCommand( "page 1" );

	toSend.AddText( "Generic View" );	// our title
	toSend.AddCommand( "gumppic 25 50 1141" );	// town name
	toSend.AddCommand( "text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() );	// town name
	toSend.AddCommand( "text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour() );	// population
	toSend.AddCommand( "text 55 91 %u 3", cwmWorldState->ServerData()->RightTextColour() );	// leave town
	toSend.AddCommand( "text 55 111 %u 4", cwmWorldState->ServerData()->RightTextColour() );	// view taxes
	toSend.AddCommand( "text 55 131 %u 5", cwmWorldState->ServerData()->RightTextColour() );	// toggle town title on/off
	toSend.AddCommand( "text 55 151 %u 6", cwmWorldState->ServerData()->RightTextColour() );	// vote for mayor
	toSend.AddCommand( "text 55 171 %u 7", cwmWorldState->ServerData()->RightTextColour() );	// donate resource
	toSend.AddCommand( "tilepic 205 171 %u", GetResourceID() );				// picture of the resource
	toSend.AddCommand( "text 55 191 %u 8", cwmWorldState->ServerData()->RightTextColour() );	// view budget
	toSend.AddCommand( "text 55 211 %u 9", cwmWorldState->ServerData()->RightTextColour() );	// view allied towns
	toSend.AddCommand( "text 55 231 %u 10", cwmWorldState->ServerData()->RightTextColour() );	// view enemy towns

	toSend.AddCommand( "button 25 91 %u %i 1 0 2", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// leave town
	toSend.AddCommand( "button 25 111 %u %i 1 0 3", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// view taxes
	toSend.AddCommand( "button 25 131 %u %i 1 0 4", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// toggle title
	toSend.AddCommand( "button 25 151 %u %i 1 0 5", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// vote for mayor
	toSend.AddCommand( "button 25 171 %u %i 1 0 6", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// donate gold
	toSend.AddCommand( "button 25 191 %u %i 1 0 7", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// view budget
	toSend.AddCommand( "button 25 211 %u %i 1 0 8", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// view allied towns
	toSend.AddCommand( "button 25 231 %u %i 1 0 9", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );	// view enemy towns

	CChar *mChar		= sock->CurrcharObj();
	UnicodeTypes sLang	= sock->Language();
	toSend.AddText( "%s (%s)", name.c_str(), Races->Name( race ).c_str() );
	toSend.AddText( Dictionary->GetEntry( 1139, sLang ), GetPopulation() );
	toSend.AddText( Dictionary->GetEntry( 1140, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1141, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1142, sLang ), mChar->GetTownTitle()?"Off":"On" );
	toSend.AddText( Dictionary->GetEntry( 1143, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1144, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1145, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1146, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1147, sLang ) );

	if( mChar->GetTownPriv() == 2 || mChar->IsGM() ) // if we've got a mayor (remove isGM check!)
	{
		toSend.AddCommand( "button 25 281 %u %i 1 0 20", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 );
		toSend.AddCommand( "text 55 281 %u 11", cwmWorldState->ServerData()->LeftTextColour() );
		toSend.AddText( Dictionary->GetEntry( 1148, sLang ) );
	}
	toSend.Finalize();
	sock->Send( &toSend );
}
size_t CTownRegion::GetPopulation( void ) const
{
	return townMember.size();
}

void CTownRegion::DisplayTownMembers( CSocket *sock )
{
	GumpDisplay townListing( sock, 300, 300 );
	townListing.SetTitle( Dictionary->GetEntry( 1149, sock->Language() ) );
	CChar *sChar = sock->CurrcharObj();
	for( size_t counter = 0; counter < townMember.size(); ++counter )
	{
		CChar *townChar = calcCharObjFromSer( townMember[counter].townMember );
		if( ValidateObject( townChar ) )
		{
			if( sChar->IsGM() )
				townListing.AddData( townChar->GetName(), townChar->GetSerial(), 3 );
			else
				townListing.AddData( townChar->GetName(), " " );	// don't want them to know the serial
		}
		else
		{
			RemoveCharacter( counter );
			--counter;
		}
	}
	townListing.Send( 4, false, INVALIDSERIAL );
}

bool CTownRegion::VoteForMayor( CSocket *sock )
{
	SERIAL serial	= sock->GetDWord( 7 );
	CChar *srcChar	= sock->CurrcharObj();
	CChar *target	= calcCharObjFromSer( serial );
	if( ValidateObject( target ) )
	{
		if( !IsMemberOfTown( target ) )	// he's not in our town!
		{
			sock->sysmessage( 1150 );
			return false;
		}
		SERIAL serialPos = FindPositionOf( (*srcChar) );
		if( serialPos == INVALIDSERIAL )
		{
			sock->sysmessage( 1151 );
			return false;
		}
		if( townMember[serialPos].targVote != INVALIDSERIAL )
		{
			sock->sysmessage( 1152 );
			return false;
		}
		townMember[serialPos].targVote = target->GetSerial();
		sock->sysmessage( 1153 );
		return true;
	}
	else
	{
		sock->sysmessage( 1154 );
		return false;
	}
}

SERIAL CTownRegion::FindPositionOf( CChar& toAdd )
{
	for( SERIAL counter = 0; counter < townMember.size(); ++counter )
	{
		if( townMember[counter].townMember == toAdd.GetSerial() )
			return counter;
	}
	return INVALIDSERIAL;
}

UI16 CTownRegion::GetResourceID( void ) const
{
	return taxedResource;
}

bool CTownRegion::DonateResource( CSocket *s, SI32 amount )
{
	goldReserved += amount;
	CChar *tChar = s->CurrcharObj();
	if( amount > 1000 )
		s->sysmessage( 1155, tChar->GetName().c_str() );
	else if( amount > 750 )
		s->sysmessage( 1156, tChar->GetName().c_str() );
	else if( amount > 500 )
		s->sysmessage( 1157, tChar->GetName().c_str() );
	else if( amount > 250 )
		s->sysmessage( 1158, tChar->GetName().c_str() );
	else
		s->sysmessage( 1159, tChar->GetName().c_str() );
	return true;
}

bool CTownRegion::PurchaseGuard( CSocket *sock, UI08 number )
{
	if( number * 10000 < goldReserved )	// if we don't have the cash
	{
		sock->sysmessage( 1160 );
		return false;
	}

	goldReserved -= (number * 10000);
	for( UI08 counter = 0; counter < number; ++counter )
		++numGuards;
	return true;
}

bool CTownRegion::ViewBudget( CSocket *sock )
{
	UnicodeTypes sLang = sock->Language();
	GumpDisplay Budget( sock, 300, 300 );
	Budget.SetTitle( Dictionary->GetEntry( 1161, sLang ) );
	Budget.AddData( Dictionary->GetEntry( 1162, sLang ), guardsPurchased );
	Budget.AddData( Dictionary->GetEntry( 1163, sLang ), numGuards );
	Budget.AddData( Dictionary->GetEntry( 1164, sLang ), numGuards * 20 );
	Budget.Send( 4, false, INVALIDSERIAL );

	return true;
}

bool CTownRegion::PeriodicCheck( void )
{
	time_t now;
	time( &now );
	if( difftime( now, timeSinceTaxedMembers ) >= cwmWorldState->ServerData()->TownTaxPeriod() )
	{
		for( size_t memberCounter = 0; memberCounter < townMember.size(); ++memberCounter )
		{
			CChar *townMem = calcCharObjFromSer( townMember[memberCounter].townMember );
			if( ValidateObject( townMem ) )
			{
				UI16 resourceID = GetResourceID();
				UI32 numResources = GetItemAmount( townMem, resourceID );

				if( taxedAmount > numResources )
				{
					UI32 bankAmount = GetBankCount( townMem, resourceID, 0 );
					if( taxedAmount > ( numResources + bankAmount ) )
						JailSys->JailPlayer( townMem, 900 );
					else
					{
						DeleteItemAmount( townMem, numResources, resourceID );
						DeleteBankItem( townMem, bankAmount - numResources, resourceID );
					}
				}
				else
				{
					DeleteItemAmount( townMem, taxedAmount, resourceID );
					resourceCollected += taxedAmount;
				}
			}
		}
		timeSinceTaxedMembers = now;
	}
	if( difftime( now, timeSinceGuardsPaid ) >= cwmWorldState->ServerData()->TownGuardPayment() )	// init to 1000 seconds
	{
		if( goldReserved < static_cast<SI32>(( 20 * numGuards )) )
		{
			// insufficient resources
			bool enoughGuards = false;
			while( !enoughGuards )
			{
				--numGuards;
				enoughGuards = ( goldReserved >= static_cast<SI32>(( 20 * numGuards ) ));
			}
		}
		goldReserved -= 20 * numGuards;
		timeSinceGuardsPaid = now;
	}

	if( now > timeToNextPoll && !townMember.empty() )
	{
		TellMembers( 1165 );
		for( size_t counter = 0; counter < townMember.size(); ++counter )
			townMember[counter].targVote = INVALIDSERIAL;
		timeToElectionClose = now + cwmWorldState->ServerData()->TownNumSecsPollOpen();	// 2 days polls are open
		timeToNextPoll		= timeToElectionClose + cwmWorldState->ServerData()->TownNumSecsAsMayor();	// secs as mayor over the top of the end of the poll
		CChar *mayor		= GetMayor();
		if( ValidateObject( mayor ) )
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

WorldType CTownRegion::GetAppearance( void ) const
{
	return visualAppearance;
}

void CTownRegion::ViewTaxes( CSocket *sock )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.AddCommand( "page 1" );

	toSend.AddText( "Taxes" );	// our title
	toSend.AddCommand( "gumppic 25 50 1141" );	// town name
	toSend.AddCommand( "text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() );	// town name
	toSend.AddCommand( "text 35 71 %u 2", cwmWorldState->ServerData()->RightTextColour() );	// population
	toSend.AddCommand( "text 35 111 %u 3", cwmWorldState->ServerData()->RightTextColour() ); // # of resources
	toSend.AddCommand( "tilepic 5 111 %u", GetResourceID() );				// picture of the resource

	toSend.AddText( "%s (%s)", name.c_str(), Races->Name( race ).c_str() );
	toSend.AddText( "Population %i", GetPopulation() );
	CTile tile;
	Map->SeekTile( GetResourceID(), &tile );
	toSend.AddText( "%i %ss", taxedAmount, tile.Name() );
	toSend.Finalize();
	sock->Send( &toSend );
}

SI16 CTownRegion::GetHealth( void ) const
{
	return health;
}

void CTownRegion::DoDamage( SI16 reduction )
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

bool CTownRegion::IsAlliedTown( UI08 townToCheck ) const
{
	for( size_t counter = 0; counter < alliedTowns.size(); ++counter )
	{
		if( alliedTowns[counter] == townToCheck )
			return true;
	}
	return false;
}

bool CTownRegion::MakeAlliedTown( UI08 townToMake )
{
	if( regionNum == townToMake )
		return false;

	if( IsAlliedTown( townToMake ) )	// already allied
	{
		TellMembers( 1171 );
		return false;
	}

	if( Races->CompareByRace( regions[townToMake]->GetRace(), race ) == 1 )	// if we're racial enemies
		return false;

	// let's ally ourselves
	alliedTowns.push_back( townToMake );
	TellMembers( 1172, name.c_str(), regions[townToMake]->GetName().c_str() );
	return true;

}

void CTownRegion::TellMembers( SI32 dictEntry, ...) // System message (In lower left corner)
{
	char msg[512];
	char tmpMsg[512];

	for( size_t memberCounter = 0; memberCounter < townMember.size(); ++memberCounter )
	{
		CChar *targetChar = calcCharObjFromSer( townMember[memberCounter].townMember );
		CSocket *targetSock = calcSocketObjFromChar( targetChar );
		if( targetSock != NULL )
		{
			std::string txt = Dictionary->GetEntry( dictEntry, targetSock->Language() );

			strcpy( msg, "TOWN: " );
			va_list argptr;
			va_start( argptr, dictEntry );
			vsprintf( tmpMsg, txt.c_str(), argptr );
			va_end( argptr );
			strcat( msg, tmpMsg );

			CSpeechEntry& toAdd = SpeechSys->Add();
			toAdd.Speech( msg );
			toAdd.Font( FNT_NORMAL );
			toAdd.Speaker( INVALIDSERIAL );
			toAdd.SpokenTo( townMember[memberCounter].townMember );
			toAdd.Colour( 0x000B );
			toAdd.Type( SYSTEM );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_INDIVIDUAL );
		}
	}
}

RACEID CTownRegion::GetRace( void ) const
{
	return race;
}

void CTownRegion::SendAlliedTowns( CSocket *sock )
{
	GumpDisplay Ally( sock, 300, 300 );
	char temp[100];
	sprintf( temp, Dictionary->GetEntry( 1173, sock->Language() ).c_str(), alliedTowns.size() );
	Ally.SetTitle( temp );
	for( size_t counter = 0; counter < alliedTowns.size(); ++counter )
		Ally.AddData( regions[alliedTowns[counter]]->GetName(), " " );

	Ally.Send( 4, false, INVALIDSERIAL );
}

void CTownRegion::ForceEarlyElection( void )
{
	time_t now;
	time(&now);
	CChar *mayor	= GetMayor();
	timeToNextPoll	= now;	// time to open poll
	TellMembers( 1174 );
	for( size_t counter = 0; counter < townMember.size(); ++counter )
		townMember[counter].targVote = INVALIDSERIAL;
	if( ValidateObject( mayor ) )
		mayor->SetTownpriv( 1 );
}

void CTownRegion::SendEnemyTowns( CSocket *sock )
{
	GumpDisplay Enemy( sock, 300, 300 );
	char temp[100];
	UI08 enemyCount = 0;
	for( UI16 counter = 0; counter < 256; ++counter )
	{
		if( counter != regionNum && Races->CompareByRace( race, regions[counter]->GetRace() ) == 1 )	// if we're racial enemies, and not the same as ourselves
		{
			++enemyCount;
			Enemy.AddData( regions[counter]->GetName(), Races->Name( regions[counter]->GetRace() ) );
		}
	}
	sprintf( temp, "Enemy Towns (%u)", enemyCount );
	Enemy.SetTitle( temp );
	Enemy.Send( 4, false, INVALIDSERIAL );
}

void CTownRegion::Possess( CTownRegion *possessorTown )
{
	possessorTown->SetRace( race );
	possessorTown->TellMembers( 1175 );
	possessorTown->SetReserves( possessorTown->GetReserves() + GetReserves() / 3 * 2 );
	possessorTown->SetTaxesLeft( possessorTown->GetTaxes() + GetTaxes() / 3 * 2 );
	TellMembers( 1176 );
	MakeAlliedTown( possessorTown->GetRegionNum() );
	possessorTown->MakeAlliedTown( regionNum );

	// remove the old members, preparing the way for the new ones
	CChar *targChar;
	for( size_t counter = townMember.size() - 1; counter >= 0 && counter < townMember.size(); ++counter )
	{
		targChar = calcCharObjFromSer( townMember[counter].townMember );
		RemoveCharacter( counter );
		if( ValidateObject( targChar ) )
		{
			targChar->SetTown( 255 );
			targChar->SetTownpriv( 0 );
		}
	}
	townMember.resize( 0 );
}

long CTownRegion::GetReserves( void ) const
{
	return resourceCollected;
}
long CTownRegion::GetTaxes( void ) const
{
	return goldReserved;
}
void CTownRegion::SetTaxesLeft( long newValue )
{
	goldReserved = newValue;
}
void CTownRegion::SetReserves( long newValue )
{
	resourceCollected = newValue;
}
void CTownRegion::SetRace( RACEID newRace )
{
	race = newRace;
}
SI16 CTownRegion::GetMinColourSkill( void ) const
{
	return minColourSkill;
}
UI08 CTownRegion::GetChanceBigOre( void ) const
{
	return chanceFindBigOre;
}
UI08 CTownRegion::GetChanceColourOre( void ) const
{
	return chanceColourOre;
}
bool CTownRegion::RemoveCharacter( size_t position )
{
	townMember.erase( townMember.begin() + position );
	return true;
}
size_t CTownRegion::GetNumOrePreferences( void ) const
{
	return orePreferences.size();
}
const orePref *CTownRegion::GetOrePreference( size_t targValue ) const
{
	if( targValue >= orePreferences.size() )
		return NULL;
	return &orePreferences[targValue];
}

SI32 CTownRegion::GetOreChance( void ) const
{
	int sumReturn = 0;
	std::vector< orePref >::const_iterator oIter;
	for( oIter = orePreferences.begin(); oIter != orePreferences.end(); ++oIter )
		sumReturn += (*oIter).percentChance;
	return sumReturn;
}
bool CTownRegion::IsDungeon( void ) const
{
	return ( (priv&0x80) == 0x80 );
}

UI16 CTownRegion::NumGuards( void ) const
{
	return numGuards;
}

UI08 CTownRegion::WorldNumber( void ) const
{
	return worldNumber;
}

void CTownRegion::IsGuarded( bool value )
{
	if( value )
		priv |= 0x01;
	else
		priv &= 0xFE;
}
void CTownRegion::CanMark( bool value )
{
	if( value )
		priv |= 0x02;
	else
		priv &= 0xFD;
}
void CTownRegion::CanGate( bool value )
{
	if( value )
		priv |= 0x04;
	else
		priv &= 0xFB;
}
void CTownRegion::CanRecall( bool value )
{
	if( value )
		priv |= 0x08;
	else
		priv &= 0xF7;
}
void CTownRegion::CanCastAggressive( bool value )
{
	if( value )
		priv |= 0x40;
	else
		priv &= 0xBF;
}
void CTownRegion::IsDungeon( bool value )
{
	if( value )
		priv |= 0x80;
	else
		priv &= 0x7F;
}
void CTownRegion::SetName( std::string toSet )
{
	name = toSet.substr( 0, 49 );
}

void CTownRegion::TaxedAmount( UI16 amount )
{
	taxedAmount = amount;
}

UI16 CTownRegion::TaxedAmount( void ) const
{
	return taxedAmount;
}

void CTownRegion::SetResourceID( UI16 resID )
{
	taxedResource = resID;
}

void CTownRegion::SetHealth( SI16 newValue )
{
	health = newValue;
}

void CTownRegion::SetChanceBigOre( UI08 newValue )
{
	chanceFindBigOre = newValue;
}
void CTownRegion::SetChanceColourOre( UI08 newValue )
{
	chanceColourOre = newValue;
}

UI16 CTownRegion::GetScriptTrigger( void ) const
{
	return jsScript;
}
void CTownRegion::SetScriptTrigger( UI16 newValue )
{
	jsScript = newValue;
}

UI08 CTownRegion::GetRegionNum( void ) const
{
	return regionNum;
}
void CTownRegion::SetRegionNum( UI08 newVal )
{
	regionNum = newVal;
}

size_t CTownRegion::GetNumLocations( void ) const
{
	return locations.size();
}
const regLocs *CTownRegion::GetLocation( size_t locNum ) const
{
	if( locNum >= locations.size() )
		return NULL;
	return &locations[locNum];
}

}
