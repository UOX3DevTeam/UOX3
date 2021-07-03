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
#include "CJSEngine.h"
#include "CJSMapping.h"
#include "StringUtility.hpp"
// Implementation of town regions

const SI08 MAYOR = 0x01;
//const SI08 ENEMY = 0x02;
//const SI08 JOINER = 0x03;

const UI32 BIT_GUARDED		=	0;
const UI32 BIT_MARK			=	1;
const UI32 BIT_GATE			=	2;
const UI32 BIT_RECALL		=	3;
const UI32 BIT_AGGRESSIVE	=	6;
const UI32 BIT_DUNGEON		=	7;
const UI32 BIT_SAFEZONE		=	8;
const UI32 BIT_TELEPORT		=	9;
const UI32 BIT_HOUSING		=	10;

const RACEID	DEFTOWN_RACE				= 0;
const weathID	DEFTOWN_WEATHER				= 255;
const SI32		DEFTOWN_MUSICLIST			= 0;
const SERIAL	DEFTOWN_MAYOR				= INVALIDSERIAL;
const UI16		DEFTOWN_TAXEDRESOURCE		= 0x0EED;
const UI16		DEFTOWN_TAXEDAMOUNT			= 0;
const SI32		DEFTOWN_GOLDRESERVED		= 0;
const SI16		DEFTOWN_GUARDSPURCHASED		= 0;
const UI32		DEFTOWN_RESOURCECOLLECTED	= 0;
const WorldType	DEFTOWN_VISUALAPPEARANCE	= WRLD_SPRING;
const SI16		DEFTOWN_HEALTH				= 30000;
const UI32		DEFTOWN_ELECTIONCLOSE		= 0;
const UI32		DEFTOWN_NEXTPOLL			= 0;
const UI32		DEFTOWN_GUARDSPAID			= 0;
const UI32		DEFTOWN_TAXEDMEMBERS		= 0;
const UI08		DEFTOWN_WORLDNUMBER			= 0;
const UI16		DEFTOWN_INSTANCEID			= 0;
const UI16		DEFTOWN_JSSCRIPT			= 0xFFFF;
const UI08		DEFTOWN_FINDBIGORE			= 0;
const UI16		DEFTOWN_NUMGUARDS			= 10;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion( UI16 region )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for CTownRegion class
//o-----------------------------------------------------------------------------------------------o
CTownRegion::CTownRegion( UI16 region ) : race( DEFTOWN_RACE ), weather( DEFTOWN_WEATHER ),
regionNum( region ), musicList( DEFTOWN_MUSICLIST ), mayorSerial( DEFTOWN_MAYOR ), taxedResource( DEFTOWN_TAXEDRESOURCE ),
taxedAmount( DEFTOWN_TAXEDAMOUNT ), goldReserved( DEFTOWN_GOLDRESERVED ), guardsPurchased( DEFTOWN_GUARDSPURCHASED ),
resourceCollected( DEFTOWN_RESOURCECOLLECTED ), visualAppearance( DEFTOWN_VISUALAPPEARANCE ), health( DEFTOWN_HEALTH ),
timeToElectionClose( DEFTOWN_ELECTIONCLOSE ), timeToNextPoll( DEFTOWN_NEXTPOLL ), timeSinceGuardsPaid( DEFTOWN_GUARDSPAID ),
timeSinceTaxedMembers( DEFTOWN_TAXEDMEMBERS ), worldNumber( DEFTOWN_WORLDNUMBER ), instanceID( DEFTOWN_INSTANCEID ), jsScript( DEFTOWN_JSSCRIPT ),
chanceFindBigOre( DEFTOWN_FINDBIGORE ), numGuards( DEFTOWN_NUMGUARDS )
{
	priv.reset();
	townMember.resize( 0 );
	alliedTowns.resize( 0 );
	orePreferences.resize( 0 );
	locations.resize( 0 );
	name		= Dictionary->GetEntry( 1117 );
	guardowner	= Dictionary->GetEntry( 1118 );
	guardList	= "guard";
	goodList.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~CTownRegion()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for CTownRegion class
//o-----------------------------------------------------------------------------------------------o
CTownRegion::~CTownRegion()
{
	JSEngine->ReleaseObject( IUE_REGION, this );

	townMember.resize( 0 );
	alliedTowns.resize( 0 );
	orePreferences.resize( 0 );
	locations.resize( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Load( Script *ss )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads town regions from region world files
//|	Notes		-	ss is a script section containing all the data!
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::Load( Script *ss )
{
	size_t location = 0xFFFFFFFF;
	std::string tag;
	std::string data;
	std::string UTag;
	std::string sect = std::string("TOWNREGION ") + strutil::number( regionNum );
	if( !ss->isin( sect )) // doesn't exist
	{
		return false;
	}

	ScriptSection *target = ss->FindEntry( sect );
	if( target == nullptr )
		return false;

	for( tag = target->First(); !target->AtEnd(); tag = target->Next() )
	{
		UTag = strutil::toupper( tag );
		data = target->GrabData();
		UI32 duint ;
		try {
			duint = static_cast<UI32>(std::stoul(data, nullptr, 0));
		}
		catch (...) {
			duint = 0;
		}
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "ALLYTOWN" )
					alliedTowns.push_back( static_cast<UI08>(duint) );
				break;
			case 'E':
				if( UTag == "ELECTIONTIME" )
					timeToElectionClose = static_cast<SI32>(duint);
				break;
			case 'G':
				if( UTag == "GUARDOWNER" )
				{
					guardowner = data;
				}
				else if( UTag == "GUARD" ) // load our purchased guard
				{
					++numGuards;
				}
				else if( UTag == "GUARDSBOUGHT" ) // num guards bought
				{
					guardsPurchased = static_cast<SI16>(duint);
				}
				break;
			case 'H':
				if( UTag == "HEALTH" )
				{
					health = static_cast<SI16>(duint);
				}
				break;
			case 'M':
				if( UTag == "MEMBER" )
				{
					location = townMember.size();
					townMember.resize( location + 1 );
					townMember[location].targVote = INVALIDSERIAL;
					townMember[location].townMember = duint;
				}
				else if( UTag == "MAYOR" )
				{
					mayorSerial = duint;
				}
				break;
			case 'N':
				if( UTag == "NAME" )
				{
					name = data.substr( 0, 49 );
				}
				else if( UTag == "NUMGUARDS" )
				{
					numGuards = static_cast<UI16>(duint);
				}
				break;
			case 'P':
				if( UTag == "PRIV" )
				{
					// Overwrites privs loaded from regions.dfn, making it impossible to change privs after initial server startup
					// Currently not used for anything, so we disable until we find a better solution.
					//priv = std::bitset<10>( data.toUShort() );
				}
				else if( UTag == "POLLTIME" )
				{
					timeToNextPoll = static_cast<SI32>(duint);
				}
				break;
			case 'R':
				if( UTag == "RACE" )
				{
					race = static_cast<UI16>(duint);
				}
				else if( UTag == "RESOURCEAMOUNT" )
				{
					goldReserved = static_cast<SI32>(duint);
				}
				else if( UTag == "RESOURCECOLLECTED" )
				{
					resourceCollected = static_cast<SI32>(duint);
				}
				break;
			case 'T':
				if( UTag == "TAXEDID" )
				{
					taxedResource = static_cast<UI16>(duint);
				}
				else if( UTag == "TAXEDAMOUNT" )
				{
					taxedAmount = static_cast<UI16>(duint);
				}
				else if( UTag == "TIMET" )
				{
					timeSinceTaxedMembers = static_cast<SI32>(duint);
				}
				else if( UTag == "TIMEG" )
				{
					timeSinceGuardsPaid = static_cast<SI32>(duint);
				}
				break;
			case 'V':
				if( UTag == "VOTE" && location != 0xFFFFFFFF )
				{
					townMember[location].targVote = duint;
				}
				break;
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Save( std::ofstream &outStream )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves town region data to worldfiles
//|	Notes		-	entry is the region #, fp is the file to save in
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::Save( std::ofstream &outStream )
{
	outStream << "[TOWNREGION " << static_cast<UI16>(regionNum) << "]" << '\n' << "{" << '\n';
	outStream << "RACE=" << race << '\n';
	outStream << "GUARDOWNER=" << guardowner << '\n';
	outStream << "MAYOR=" << std::hex << "0x" << mayorSerial << std::dec << '\n';
	outStream << "PRIV=" << priv.to_ulong() << '\n';
	outStream << "RESOURCEAMOUNT=" << goldReserved << '\n';
	outStream << "TAXEDID=" << std::hex << "0x" << taxedResource << std::dec << '\n';
	outStream << "TAXEDAMOUNT=" << taxedAmount << '\n';
	outStream << "GUARDSPURCHASED=" << guardsPurchased << '\n';
	outStream << "TIMEG=" << timeSinceGuardsPaid << '\n';
	outStream << "TIMET=" << timeSinceTaxedMembers << '\n';
	outStream << "RESOURCECOLLECTED=" << resourceCollected << '\n';
	outStream << "HEALTH=" << health << '\n';
	outStream << "ELECTIONTIME=" << timeToElectionClose << '\n';
	outStream << "POLLTIME=" << timeToNextPoll << '\n';
	outStream << "WORLD=" << static_cast<UI16>(worldNumber) << '\n';
	outStream << "INSTANCEID=" << static_cast<UI16>(instanceID) << '\n';
	outStream << "NUMGUARDS=" << numGuards << '\n';

	std::vector< townPers >::const_iterator mIter;
	for( mIter = townMember.begin(); mIter != townMember.end(); ++mIter )
	{
		outStream << "MEMBER=" << std::hex << "0x" << (*mIter).townMember << '\n';
		outStream << "VOTE=" << "0x" << (*mIter).targVote << std::dec << '\n';
	}
	std::vector< UI16 >::const_iterator aIter;
	for( aIter = alliedTowns.begin(); aIter != alliedTowns.end(); ++aIter )
	{
		outStream << "ALLYTOWN=" << static_cast<UI16>((*aIter)) << '\n';
	}
	outStream << "}" << '\n' << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CalcNewMayor( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates new town mayor based on votes
//|	Notes		-	There has got to be a better way than this hideous O(n^2) algy
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::CalcNewMayor( void )
{
	if( townMember.empty() )
		return;
	// if there are no members, there are no new mayors
	std::vector< SI32 > votes;
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
	if( ValidateObject( oldMayor ) && mayorSerial == townMember[maxIndex].townMember )
	{
		CSocket *targSock = oldMayor->GetSocket();
		if( targSock != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar * GetMayor( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the character who is town mayor
//o-----------------------------------------------------------------------------------------------o
CChar * CTownRegion::GetMayor( void )
{
	return calcCharObjFromSer( mayorSerial );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetMayorSerial( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the serial of the character who is town mayor
//o-----------------------------------------------------------------------------------------------o
SERIAL CTownRegion::GetMayorSerial( void ) const
{
	return mayorSerial;
}
void CTownRegion::SetMayorSerial( SERIAL newValue )
{
	mayorSerial = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AddAsTownMember( CChar& toAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds specified character to the town's member list
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::AddAsTownMember( CChar& toAdd )
{
	if( Races->CompareByRace( toAdd.GetRace(), race ) <= RACE_ENEMY )	// if we're racial enemies
		return false;	// we can't have a racial enemy in the town!

	for( UI32 counter = 0; counter < townMember.size(); ++counter )		// exhaustive error checking
	{
		if( townMember[counter].townMember == toAdd.GetSerial() )
			return false;	// already exists in our town!
	}
	townMember.resize( townMember.size() + 1 );
	townMember[townMember.size()-1].townMember = toAdd.GetSerial();
	townMember[townMember.size()-1].targVote = INVALIDSERIAL;
	townMember[townMember.size()-1].PlayerBank = nullptr;
	toAdd.SetTown( regionNum );
	toAdd.SetTownpriv( 1 );	// set as resident
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RemoveTownMember( CChar& toAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specified character from the town's member list
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::RemoveTownMember( CChar& toAdd )
{
	if( toAdd.GetTown() == 0 || toAdd.GetTown() != regionNum )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool oreSkillComparator (orePref o1, orePref o2)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sorts list of ore preferences for the town region in descending order based on
//|					minimum skill required to mine said ore
//o-----------------------------------------------------------------------------------------------o
bool oreSkillComparator (orePref o1, orePref o2)
{
	if (o1.oreIndex == nullptr)
		return false;
	if (o2.oreIndex == nullptr)
		return true;
	return o1.oreIndex->minSkill > o2.oreIndex->minSkill;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InitFromScript( ScriptSection *toScan )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes town region from associated script section
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::InitFromScript( ScriptSection *toScan )
{
	std::string tag;
	std::string data;
	std::string UTag;
	SI32 actgood = -1;
	bool orePrefLoaded = false;

	// Some default values
	numGuards			= 10;
	chanceFindBigOre	= 80;
	CanTeleport( true );
	CanPlaceHouse( false );

	regLocs ourLoc;
	for( tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
	{
		UTag = strutil::toupper( tag );
		data = toScan->GrabData();
		UI32 duint ;
		try {
			duint = static_cast<UI32>(std::stoul(data, nullptr, 0));
		}
		catch (...) {
			duint = 0;
		}
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "ABWEATH" )
				{
					weather = static_cast<UI08>(duint);
				}
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
			case 'B':
				if( UTag == "BUYABLE" )
				{
					if( actgood > - 1 )
					{
						goodList[actgood].buyVal = static_cast<SI32>(duint);
					}
					else
					{
						Console.error( "regions dfn -> You must write BUYABLE after GOOD <num>!" );
					}
				}
				break;
			case 'C':
				if( UTag == "CHANCEFORBIGORE" )
				{
					chanceFindBigOre =static_cast<UI08>(duint);
				}
				break;
			case 'D':
				if( UTag == "DUNGEON" )
				{
					IsDungeon(( duint == 1 ));
				}
				break;
			case 'E':
				if( UTag == "ESCORTS" )
				{
					// Load the region number in the global array of valid escortable regions
					if( duint == 1 )
					{
						cwmWorldState->escortRegions.push_back( regionNum );
					}
				}
				break;
			case 'G':
				if( UTag == "GUARDNUM" )
				{
					break;
				}
				else if( UTag == "GUARDLIST" )
				{
					guardList = data;
				}
				else if( UTag == "GUARDOWNER" )
				{
					guardowner = data;
				}
				else if( UTag == "GUARDED" )
				{
					IsGuarded(( duint == 1 ));
				}
				else if( UTag == "GATE" )
				{
					CanGate(( duint == 1 ));
				}
				else if( UTag == "GOOD" )
				{
					actgood = static_cast<int>(duint);
				}
				break;
			case 'H':
				if( UTag == "HOUSING" )
				{
					CanPlaceHouse(( duint == 1 ));
				}
				break;
			case 'I':
				if( UTag == "INSTANCEID" )
				{
					instanceID = static_cast<UI16>(duint);
				}
				break;
			case 'M':
				if( UTag == "MUSICLIST" || UTag == "MIDILIST" )
				{
					musicList = static_cast<UI16>(duint);
				}
				else if( UTag == "MAGICDAMAGE" )
				{
					CanCastAggressive(( duint == 1 ));
				}
				else if( UTag == "MARK" )
				{
					CanMark(( duint == 1 ));
				}
				break;
			case 'N':
				if( UTag == "NAME" )
				{
					name = data;
					actgood = -1;
				}
				break;
			case 'O':
				if( UTag == "OREPREF" )
				{
					std::string numPart;
					std::string oreName;
					orePref toPush;
					data			= strutil::simplify( data );
					oreName			= strutil::extractSection( data, ",", 0, 0 );
					toPush.oreIndex = Skills->FindOre( oreName );
					if( toPush.oreIndex != nullptr )
					{
						auto csecs = strutil::sections( data, "," );
						if( csecs.size() > 1 )
						{
							// Use chance specified in orepref
							toPush.percentChance = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0));
						}
						else if( toPush.oreIndex->oreChance > 0 )
						{
							// No chance specified in orepref, use default chance of ore type
							toPush.percentChance = toPush.oreIndex->oreChance;
						}
						else
						{
							// Fallback for older skills.dfn setups
							toPush.percentChance = 1000 / static_cast<UI16>(Skills->GetNumberOfOres());
						}

						orePreferences.push_back( toPush );
						orePrefLoaded = true;
					}
					else
					{
						Console.error( strutil::format("Invalid ore preference in region %i as %s", regionNum, oreName.c_str() ));
					}
				}
				break;
			case 'R':
				if( UTag == "RECALL" )
				{
					CanRecall(( duint == 1 ));
				}
				else if( UTag == "RANDOMVALUE" )
				{
					if( actgood > -1 )
					{
						auto ssecs = strutil::sections( data, " " );
						if( ssecs.size() > 1 )
						{
							goodList[actgood].rand1 = std::stoi(strutil::stripTrim( ssecs[0] ), nullptr, 0);
							goodList[actgood].rand2 = std::stoi(strutil::stripTrim( ssecs[1] ), nullptr, 0);
						}
						else
						{
							goodList[actgood].rand1 = static_cast<SI32>(duint);
							goodList[actgood].rand2 = goodList[actgood].rand1;
						}
						if( goodList[actgood].rand2 < goodList[actgood].rand1 )
						{
							Console.error( strutil::format(" regions dfn -> You must write RANDOMVALUE NUM2[%i] greater than NUM1[%i].", goodList[actgood].rand2, goodList[actgood].rand1 ));
							goodList[actgood].rand2 = goodList[actgood].rand1 = 0;
						}
					}
					else
					{
						Console.error( " regions dfn -> You must write RANDOMVALUE after GOOD <num>!" );
					}
				}
				else if( UTag == "RACE" )
				{
					race = static_cast<UI16>(duint);
				}
				break;
			case 'S':
				if( UTag == "SAFEZONE" )
				{
					IsSafeZone(( duint == 1 ));
				}
				else if( UTag == "SELLABLE" )
				{
					if( actgood > - 1 )
					{
						goodList[actgood].sellVal = static_cast<SI32>(duint);
					}
					else
					{
						Console.error( " regions dfn -> You must write SELLABLE after GOOD <num>!" );
					}
				}
				else if( UTag == "SPAWN" )
				{
					std::string sect = "PREDEFINED_SPAWN " + data;
					ScriptSection *predefSpawn = FileLookup->FindEntry( sect, spawn_def );
					if( predefSpawn == nullptr )
					{
						Console.warning( strutil::format("Undefined region spawn %s, check your regions.dfn and spawn.dfn files", data.c_str()) );
					}
					else
					{
						for( UI16 i = 0xFFFF; i > 0; --i )
						{
							if( cwmWorldState->spawnRegions.find( i ) != cwmWorldState->spawnRegions.end() )
							{
								CSpawnRegion *spawnReg			= new CSpawnRegion( i );
								cwmWorldState->spawnRegions[i]	= spawnReg;
								if( spawnReg != nullptr )
								{
									spawnReg->SetX1( locations[locations.size() - 1].x1 );
									spawnReg->SetY1( locations[locations.size() - 1].y1 );
									spawnReg->SetX2( locations[locations.size() - 1].x2 );
									spawnReg->SetY2( locations[locations.size() - 1].y2 );
									spawnReg->Load( predefSpawn );
								}
								break;
							}
						}
					}
				}
				else if( UTag == "SCRIPT" )
				{
					std::uint16_t scriptID = static_cast<std::uint16_t>(duint);
					if( scriptID != 0 )
					{
						cScript *toExecute	= JSMapping->GetScript( scriptID );
						if( toExecute == nullptr )
						{
							Console.warning( strutil::format("SCRIPT tag found with invalid script ID (%s) when loading region data!", strutil::number(scriptID).c_str()) );
						}
						else
						{
							this->AddScriptTrigger( scriptID );
						}
					}
				}
				break;
			case 'T':
				if( UTag == "TELEPORT" )
				{
					CanTeleport(( duint == 1 ));
				}
				break;
			case 'W':
				if( UTag == "WORLD" )
				{
					worldNumber = static_cast<UI08>(duint);
				}
				break;
			case 'X':
				if( UTag == "X1" )
				{
					ourLoc.x1 = static_cast<UI16>(duint);
				}
				else if( UTag == "X2" )
				{
					ourLoc.x2 = static_cast<UI16>(duint);
				}
				break;
			case 'Y':
				if( UTag == "Y1" )
				{
					ourLoc.y1 = static_cast<UI16>(duint);
				}
				else if( UTag == "Y2" )
				{
					ourLoc.y2 = static_cast<UI16>(duint);
					locations.push_back( ourLoc );
				}
				break;
		}
	}

	// No ore preference tags found for this region, apply default chance to find ores
	if( !orePrefLoaded )
	{
		orePref toLoad;
		size_t numOres = Skills->GetNumberOfOres();
		for( size_t myCounter = 0; myCounter < numOres; ++myCounter )
		{
			toLoad.oreIndex			= Skills->GetOre( myCounter );
			toLoad.percentChance	= toLoad.oreIndex->oreChance;
			orePreferences.push_back( toLoad );
		}
	}
	// sort orePreferences in order of descending minSkill
	std::sort( orePreferences.begin(), orePreferences.end(), oreSkillComparator );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsGuarded( void ) const
//|					void IsGuarded( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion is protected by guards
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::IsGuarded( void ) const
{
	return priv.test( BIT_GUARDED );
}
void CTownRegion::IsGuarded( bool value )
{
	priv.set( BIT_GUARDED, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanPlaceHouse( void ) const
//|					void CanPlaceHouse( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion allows player houses
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::CanPlaceHouse( void ) const
{
	return priv.test( BIT_HOUSING );
}
void CTownRegion::CanPlaceHouse( bool value )
{
	priv.set( BIT_HOUSING, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanMark( void ) const
//|					void CanMark( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast the Mark spell in the townregion
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::CanMark( void ) const
{
	return priv.test( BIT_MARK );
}
void CTownRegion::CanMark( bool value )
{
	priv.set( BIT_MARK, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanGate( void ) const
//|					void CanGate( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast the Gate spell in the townregion
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::CanGate( void ) const
{
	return priv.test( BIT_GATE );
}
void CTownRegion::CanGate( bool value )
{
	priv.set( BIT_GATE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanRecall( void ) const
//|					void CanRecall( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast the Recall spell in the townregion
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::CanRecall( void ) const
{
	return priv.test( BIT_RECALL );
}
void CTownRegion::CanRecall( bool value )
{
	priv.set( BIT_RECALL, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanCastAggressive( void ) const
//|					void CanCastAggressive( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast Aggressive spells in the townregion
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::CanCastAggressive( void ) const
{
	return priv.test( BIT_AGGRESSIVE );
}
void CTownRegion::CanCastAggressive( bool value )
{
	priv.set( BIT_AGGRESSIVE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsSafeZone( void ) const
//|					void IsSafeZone( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion is considered a safe zone or not
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::IsSafeZone( void ) const
{
	return priv.test( BIT_SAFEZONE );
}
void CTownRegion::IsSafeZone( bool value )
{
	priv.set( BIT_SAFEZONE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanTeleport( void ) const
//|					void CanTeleport( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion allows use of the Teleport spell or not
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::CanTeleport( void ) const
{
	return priv.test( BIT_TELEPORT );
}
void CTownRegion::CanTeleport( bool value )
{
	priv.set( BIT_TELEPORT, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetName( void ) const
//|					void SetName( std::string toSet )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the name of the townregion
//o-----------------------------------------------------------------------------------------------o
std::string CTownRegion::GetName( void ) const
{
	return name;
}
void CTownRegion::SetName( std::string toSet )
{
	name = toSet.substr( 0, 49 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetOwner( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the name of the guard owner for the townregion
//o-----------------------------------------------------------------------------------------------o
std::string CTownRegion::GetOwner( void ) const
{
	return guardowner;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	weathID GetWeather( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the weather in the townregion
//o-----------------------------------------------------------------------------------------------o
weathID CTownRegion::GetWeather( void ) const
{
	return weather;
}
void CTownRegion::SetWeather( weathID newValue )
{
	weather = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetGoodSell( UI08 index ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the trade system good sell-value at specified index for the townregion
//o-----------------------------------------------------------------------------------------------o
SI32 CTownRegion::GetGoodSell( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.sellVal;
	return rVal;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetGoodBuy( UI08 index ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the trade system good buy-value at specified index for the townregion
//o-----------------------------------------------------------------------------------------------o
SI32 CTownRegion::GetGoodBuy( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.buyVal;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetGoodRnd1( UI08 index ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets minimum random value for trade system good at specified index for the townregion
//o-----------------------------------------------------------------------------------------------o
SI32 CTownRegion::GetGoodRnd1( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.rand1;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetGoodRnd2( UI08 index ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets maximum random value for trade system good at specified index for the townregion
//o-----------------------------------------------------------------------------------------------o
SI32 CTownRegion::GetGoodRnd2( UI08 index ) const
{
	SI32 rVal = 0;
	std::map< SI32, GoodData_st >::const_iterator gIter = goodList.find( index );
	if( gIter != goodList.end() )
		rVal = gIter->second.rand2;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMusicList( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets musicList for the townregion, as originally specified in region DFNs
//o-----------------------------------------------------------------------------------------------o
UI16 CTownRegion::GetMusicList( void ) const
{
	return musicList;
}
void CTownRegion::SetMusicList( UI16 newValue )
{
	musicList = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar * GetRandomGuard( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns and returns a random NPC guard for the townregion
//o-----------------------------------------------------------------------------------------------o
CChar * CTownRegion::GetRandomGuard( void )
{
	CChar *ourGuard = Npcs->CreateRandomNPC( guardList );
	ourGuard->SetNPCAiType( AI_GUARD );
	return ourGuard;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DisplayTownMenu( CItem *used, CSocket *sock, SI08 flag )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays town menu for townstone used by the player
//o-----------------------------------------------------------------------------------------------o
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
		if( Races->CompareByRace( tChar->GetRace(), race ) <= RACE_ENEMY )	// if we're racial enemies
		{
			if( tChar->GetTown() != 255 )
				SendEnemyGump( sock );
			else
				sock->sysmessage( 1125 );
			return true;
		}
		else if( tChar->GetTown() != 255 ) // another town person
		{
			if( tChar->GetTown() == regionNum || tChar->GetTown() == 0 )	// they think we're in this region!!!
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsMemberOfTown( CChar *player ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether a specific player is member of the townregion
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendEnemyGump( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client with a list of the townregion's enemies
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendEnemyGump( CSocket *sock )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.addCommand( "page 0" );
	toSend.addCommand(strutil::format( "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() ));
	toSend.addCommand( strutil::format("button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1)); //OKAY
	toSend.addCommand( strutil::format("text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour()) );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.addCommand( "page 1" );

	toSend.addText( "Enemy" );
	toSend.addCommand( "gumppic 25 50 1141" );	// town name
	toSend.addCommand( strutil::format("text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() ));	// town name
	toSend.addCommand( strutil::format("text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour()) );	// population
	toSend.addCommand( strutil::format("text 55 111 %u 3", cwmWorldState->ServerData()->RightTextColour() ));	// Seize townstone
	toSend.addCommand( strutil::format("text 55 131 %u 4", cwmWorldState->ServerData()->RightTextColour()) );	// Destroy townstone

	toSend.addCommand( strutil::format("button 25 111 %u %i 1 0 61", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// seize townstone
	toSend.addCommand( strutil::format("button 25 131 %u %i 1 0 62", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// destroy townstone
	toSend.addText(strutil::format( "%s (%s)", name.c_str(), Races->Name( race ).c_str()) );
	toSend.addText( strutil::format("Population %i", GetPopulation() ));
	toSend.addText( "Seize Townstone" );
	toSend.addText( "Attack Townstone" );

	toSend.Finalize();
	sock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendBasicInfo( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client with basic townstone gump menu
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendBasicInfo( CSocket *sock )
{
	GumpDisplay BasicGump( sock );
	BasicGump.SetTitle( "Basic Townstone gump" );
	BasicGump.Send( 4, false, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendPotentialMember( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client of a potential townregion member
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendPotentialMember( CSocket *sock )
{
	UnicodeTypes sLang	= sock->Language();
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.addCommand( "page 0" );
	toSend.addCommand( strutil::format("resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic()) );
	toSend.addCommand( strutil::format("button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1)); //OKAY
	toSend.addCommand( strutil::format("text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() ));           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.addCommand( "page 1" );

	toSend.addText( "Outsider" );	// our title
	toSend.addCommand( "gumppic 25 50 1141" );	// town name
	toSend.addCommand( strutil::format("text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() ));	// town name
	toSend.addCommand( strutil::format("text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour() ));	// population
	toSend.addCommand( strutil::format("text 55 91 %u 3", cwmWorldState->ServerData()->RightTextColour() ));	// join town
	toSend.addCommand( strutil::format("text 55 111 %u 4", cwmWorldState->ServerData()->RightTextColour() ));	// view taxes (to help make decisions about joining?)

	toSend.addCommand( strutil::format("button 25 91 %u %i 1 0 41", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// leave town
	toSend.addCommand( strutil::format("button 25 111 %u %i 1 0 3", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// view taxes

	toSend.addText(strutil::format( "%s (%s)", name.c_str(), Races->Name( race ).c_str() ));

	toSend.addText( strutil::format(Dictionary->GetEntry( 1127, sLang ), GetPopulation()) );
	toSend.addText( Dictionary->GetEntry( 1128, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1129, sLang ) );

	toSend.Finalize();
	sock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendMayorGump( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client with townregion mayor info and functions
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendMayorGump( CSocket *sock )
{
	UnicodeTypes sLang	= sock->Language();
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.addCommand( "page 0" );
	toSend.addCommand( strutil::format("resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() ));
	toSend.addCommand( strutil::format("button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1)); //OKAY
	toSend.addCommand( strutil::format("text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour()) );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.addCommand( "page 1" );

	toSend.addText( "Mayor Controls" );	// our title

	toSend.addCommand( "gumppic 25 50 1141" );	// town name
	toSend.addCommand( "gumppic 25 260 1141" );
	toSend.addCommand( strutil::format("text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() ));	// town name
	toSend.addCommand( strutil::format("text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour() ));	// population
	toSend.addCommand( strutil::format("text 55 91 %u 3", cwmWorldState->ServerData()->RightTextColour() )); // set taxes
	toSend.addCommand( strutil::format("text 55 280 %u 4", cwmWorldState->ServerData()->RightTextColour()) ); // return to main menu
	toSend.addCommand( strutil::format("text 55 111 %u 5", cwmWorldState->ServerData()->RightTextColour()) ); // list town members
	toSend.addCommand( strutil::format("text 55 131 %u 6", cwmWorldState->ServerData()->RightTextColour()) ); // force early election
	toSend.addCommand( strutil::format("text 55 151 %u 7", cwmWorldState->ServerData()->RightTextColour() )); // purchase more guards
	toSend.addCommand( strutil::format("text 55 171 %u 8", cwmWorldState->ServerData()->RightTextColour() )); // fire a guard
	toSend.addCommand( strutil::format("text 55 261 %u 9", cwmWorldState->ServerData()->RightTextColour() )); // treasury amount
	toSend.addCommand( strutil::format("text 55 191 %u 10", cwmWorldState->ServerData()->RightTextColour()) );	// make ally

	toSend.addCommand( strutil::format("button 25 91 %u %i 1 0 21", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 )); // set taxes
	toSend.addCommand( strutil::format("button 25 111 %u %i 1 0 22", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 )); // list town members
	toSend.addCommand( strutil::format("button 25 131 %u %i 1 0 23", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 )); // force early election
	toSend.addCommand( strutil::format("button 25 151 %u %i 1 0 24", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 )); // purchase more guards
	toSend.addCommand( strutil::format("button 25 171 %u %i 1 0 25", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 )); // fire a guard
	toSend.addCommand( strutil::format("button 25 280 %u %i 1 0 40", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 )); // return to main menu
	toSend.addCommand( strutil::format("button 25 191 %u %i 1 0 26", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1) ); // make ally of other town

	toSend.addText( strutil::format("%s (%s)", name.c_str(), Races->Name( race ).c_str()) );
	toSend.addText( strutil::format(Dictionary->GetEntry( 1130, sLang ), GetPopulation()) );
	toSend.addText( Dictionary->GetEntry( 1131, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1132, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1133, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1134, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1135, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1136, sLang ) );
	toSend.addText( strutil::format(Dictionary->GetEntry( 1137, sLang ), goldReserved ));
	toSend.addText( Dictionary->GetEntry( 1138, sLang ) );

	toSend.Finalize();
	sock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendDefaultGump( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends default townregion gump menu to client
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendDefaultGump( CSocket *sock )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.addCommand( "page 0" );
	toSend.addCommand( strutil::format("resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic()) );
	toSend.addCommand( strutil::format("button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1)); //OKAY
	toSend.addCommand( strutil::format("text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() ));           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.addCommand( "page 1" );

	toSend.addText( "Generic View" );	// our title
	toSend.addCommand( "gumppic 25 50 1141" );	// town name
	toSend.addCommand( strutil::format("text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour() ));	// town name
	toSend.addCommand( strutil::format("text 25 71 %u 2", cwmWorldState->ServerData()->RightTextColour()) );	// population
	toSend.addCommand( strutil::format("text 55 91 %u 3", cwmWorldState->ServerData()->RightTextColour() ));	// leave town
	toSend.addCommand( strutil::format("text 55 111 %u 4", cwmWorldState->ServerData()->RightTextColour()) );	// view taxes
	toSend.addCommand(strutil::format( "text 55 131 %u 5", cwmWorldState->ServerData()->RightTextColour()) );	// toggle town title on/off
	toSend.addCommand(strutil::format( "text 55 151 %u 6", cwmWorldState->ServerData()->RightTextColour() ));	// vote for mayor
	toSend.addCommand(strutil::format( "text 55 171 %u 7", cwmWorldState->ServerData()->RightTextColour()) );	// donate resource
	toSend.addCommand( strutil::format("tilepic 205 171 %u", GetResourceID() ));				// picture of the resource
	toSend.addCommand( strutil::format("text 55 191 %u 8", cwmWorldState->ServerData()->RightTextColour()) );	// view budget
	toSend.addCommand( strutil::format("text 55 211 %u 9", cwmWorldState->ServerData()->RightTextColour()) );	// view allied towns
	toSend.addCommand( strutil::format("text 55 231 %u 10", cwmWorldState->ServerData()->RightTextColour()) );	// view enemy towns

	toSend.addCommand( strutil::format("button 25 91 %u %i 1 0 2", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// leave town
	toSend.addCommand( strutil::format("button 25 111 %u %i 1 0 3", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// view taxes
	toSend.addCommand( strutil::format("button 25 131 %u %i 1 0 4", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// toggle title
	toSend.addCommand( strutil::format("button 25 151 %u %i 1 0 5", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// vote for mayor
	toSend.addCommand( strutil::format("button 25 171 %u %i 1 0 6", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// donate gold
	toSend.addCommand( strutil::format("button 25 191 %u %i 1 0 7", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// view budget
	toSend.addCommand( strutil::format("button 25 211 %u %i 1 0 8", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// view allied towns
	toSend.addCommand( strutil::format("button 25 231 %u %i 1 0 9", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));	// view enemy towns

	CChar *mChar		= sock->CurrcharObj();
	UnicodeTypes sLang	= sock->Language();
	toSend.addText( strutil::format("%s (%s)", name.c_str(), Races->Name( race ).c_str() ));
	toSend.addText(strutil::format( Dictionary->GetEntry( 1139, sLang ), GetPopulation()) );
	toSend.addText( Dictionary->GetEntry( 1140, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1141, sLang ) );
	toSend.addText( strutil::format(Dictionary->GetEntry( 1142, sLang ), mChar->GetTownTitle()?"Off":"On" ));
	toSend.addText( Dictionary->GetEntry( 1143, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1144, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1145, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1146, sLang ) );
	toSend.addText( Dictionary->GetEntry( 1147, sLang ) );

	if( mChar->GetTownPriv() == 2 || mChar->IsGM() ) // if we've got a mayor (remove isGM check!)
	{
		toSend.addCommand( strutil::format("button 25 281 %u %i 1 0 20", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1 ));
		toSend.addCommand(strutil::format( "text 55 281 %u 11", cwmWorldState->ServerData()->LeftTextColour() ));
		toSend.addText( Dictionary->GetEntry( 1148, sLang ) );
	}
	toSend.Finalize();
	sock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetPopulation( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets population of townregion
//o-----------------------------------------------------------------------------------------------o
size_t CTownRegion::GetPopulation( void ) const
{
	return townMember.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DisplayTownMembers( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets population of townregion
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetTownMemberSerials( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a comma-separated string of town member serials
//o-----------------------------------------------------------------------------------------------o
std::string CTownRegion::GetTownMemberSerials( void ) const
{
	std::string townMemberSerials;
	for( size_t counter = 0; counter < townMember.size(); ++counter )
	{
		CChar const * townMemberChar = calcCharObjFromSer( townMember[counter].townMember );
		if( ValidateObject( townMemberChar ) )
		{
			if( townMemberSerials.empty() )
				townMemberSerials += std::to_string(townMember[counter].townMember);
			else
				townMemberSerials += std::string(",") + std::to_string(townMember[counter].townMember);
		}
	}
	return townMemberSerials;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::vector GetTownMembers( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a comma-separated string of town member serials
//o-----------------------------------------------------------------------------------------------o
std::vector< CTownRegion::townPers > CTownRegion::GetTownMembers( void ) const
{
	return townMember;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool VoteForMayor( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Submits player's vote for mayor
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL FindPositionOf( CChar& toAdd )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Find position of specified player in townregion's member list
//o-----------------------------------------------------------------------------------------------o
SERIAL CTownRegion::FindPositionOf( CChar& toAdd )
{
	for( SERIAL counter = 0; counter < townMember.size(); ++counter )
	{
		if( townMember[counter].townMember == toAdd.GetSerial() )
			return counter;
	}
	return INVALIDSERIAL;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetResourceID( void ) const
//|					void SetResourceID( UI16 resID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ID of resource taxed in townregion
//o-----------------------------------------------------------------------------------------------o
UI16 CTownRegion::GetResourceID( void ) const
{
	return taxedResource;
}
void CTownRegion::SetResourceID( UI16 resID )
{
	taxedResource = resID;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DonateResource( CSocket *s, SI32 amount )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Register player's resource donation for townregion
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PurchaseGuard( CSocket *sock, UI08 number )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Purchase a new town guard for the townregion
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ViewBudget( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	View townregion's budget for guards
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PeriodicCheck( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Perform periodic check of townregion related functions
//o-----------------------------------------------------------------------------------------------o
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
		timeSinceTaxedMembers = static_cast<SI32>(now);
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
		timeSinceGuardsPaid = static_cast<SI32>(now);
	}

	if( now > timeToNextPoll && !townMember.empty() )
	{
		TellMembers( 1165 );
		for( size_t counter = 0; counter < townMember.size(); ++counter )
			townMember[counter].targVote = INVALIDSERIAL;
		timeToElectionClose = static_cast<SI32>(now) + cwmWorldState->ServerData()->TownNumSecsPollOpen();	// 2 days polls are open
		timeToNextPoll		= timeToElectionClose + cwmWorldState->ServerData()->TownNumSecsAsMayor();	// secs as mayor over the top of the end of the poll
		CChar *mayor		= GetMayor();
		if( ValidateObject( mayor ) )
			mayor->SetTownpriv( 1 );	// mayor becomes a basic member again, so he can't do anything while the poll is occuring :>
	}

	if( now > timeToElectionClose && !townMember.empty() )// election finished
	{
		TellMembers( 1166 );
		CalcNewMayor();
		timeToElectionClose = timeToNextPoll + 1000;	// just so long as it's after the next election
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	WorldType GetAppearance( void ) const
//|					void SetAppearance( WorldType worldType )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets visual appearance (season) of townregion
//o-----------------------------------------------------------------------------------------------o
WorldType CTownRegion::GetAppearance( void ) const
{
	return visualAppearance;
}
void CTownRegion::SetAppearance( WorldType worldType )
{
	visualAppearance = worldType;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ViewTaxes( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send Gump menu with information about townregion's taxes
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::ViewTaxes( CSocket *sock )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 3 );

	toSend.addCommand( "page 0" );
	toSend.addCommand( strutil::format("resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() ));
	toSend.addCommand( strutil::format("button 280 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1)); //OKAY
	toSend.addCommand( strutil::format("text 70 10 %u 0", cwmWorldState->ServerData()->TitleColour() ));           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.addCommand( "page 1" );

	toSend.addText( "Taxes" );	// our title
	toSend.addCommand( "gumppic 25 50 1141" );	// town name
	toSend.addCommand( strutil::format("text 35 51 %u 1", cwmWorldState->ServerData()->RightTextColour()) );	// town name
	toSend.addCommand( strutil::format("text 35 71 %u 2", cwmWorldState->ServerData()->RightTextColour()) );	// population
	toSend.addCommand( strutil::format("text 35 111 %u 3", cwmWorldState->ServerData()->RightTextColour() )); // # of resources
	toSend.addCommand( strutil::format("tilepic 5 111 %u", GetResourceID() ));				// picture of the resource

	toSend.addText(strutil::format( "%s (%s)", name.c_str(), Races->Name( race ).c_str()) );
	toSend.addText( strutil::format("Population %i", GetPopulation() ));
	CTile& tile = Map->SeekTile( GetResourceID() );
	toSend.addText( strutil::format("%i %ss", taxedAmount, tile.Name()) );
	toSend.Finalize();
	sock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetHealth( void ) const
//|					void SetHealth( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the health of townstone in this townregion
//o-----------------------------------------------------------------------------------------------o
SI16 CTownRegion::GetHealth( void ) const
{
	return health;
}
void CTownRegion::SetHealth( SI16 newValue )
{
	health = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DoDamage( SI16 reduction )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deal damage to townregion's townstone and notify online townregion members
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsAlliedTown( UI16 townToCheck ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether another townregion is in the list of allies for the townregion
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::IsAlliedTown( UI16 townToCheck ) const
{
	for( size_t counter = 0; counter < alliedTowns.size(); ++counter )
	{
		if( alliedTowns[counter] == townToCheck )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MakeAlliedTown( UI16 townToMake )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds specified townregion to townregion's list of allied townregions
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::MakeAlliedTown( UI16 townToMake )
{
	if( regionNum == townToMake )
		return false;

	if( IsAlliedTown( townToMake ) )	// already allied
	{
		TellMembers( 1171 );
		return false;
	}

	if( Races->CompareByRace( cwmWorldState->townRegions[townToMake]->GetRace(), race ) <= RACE_ENEMY )	// if we're racial enemies
		return false;

	// let's ally ourselves
	alliedTowns.push_back( townToMake );
	TellMembers( 1172, name.c_str(), cwmWorldState->townRegions[townToMake]->GetName().c_str() );
	return true;

}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TellMembers( SI32 dictEntry, ...)
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a client system message to all online members of townregion
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::TellMembers( SI32 dictEntry, ...)
{

	for( size_t memberCounter = 0; memberCounter < townMember.size(); ++memberCounter )
	{
		CChar *targetChar = calcCharObjFromSer( townMember[memberCounter].townMember );
		if( !ValidateObject( targetChar ) )
			continue;

		CSocket *targetSock = targetChar->GetSocket();
		if( targetSock != nullptr )
		{
			std::string txt = Dictionary->GetEntry( dictEntry, targetSock->Language() );
			std::string msg = "TOWN: " ;
			va_list argptr;
			va_start( argptr, dictEntry );
			msg += strutil::format(txt,argptr);

			CSpeechEntry& toAdd = SpeechSys->Add();
			toAdd.Speech( msg );
			toAdd.Font( FNT_NORMAL );
			toAdd.Speaker( INVALIDSERIAL );
			toAdd.SpokenTo( townMember[memberCounter].townMember );
			toAdd.Colour( 0x000B );
			toAdd.Type( SYSTEM );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_INDIVIDUAL );
			va_end(argptr);
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	RACEID GetRace( void ) const
//|					void SetRace( RACEID newRace )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race ID associated with townregion
//o-----------------------------------------------------------------------------------------------o
RACEID CTownRegion::GetRace( void ) const
{
	return race;
}
void CTownRegion::SetRace( RACEID newRace )
{
	race = newRace;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendAlliedTowns( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends list of townregion's allied townregions to client
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendAlliedTowns( CSocket *sock )
{
	GumpDisplay Ally( sock, 300, 300 );

	auto temp = strutil::format( Dictionary->GetEntry( 1173, sock->Language() ).c_str(), alliedTowns.size() );
	Ally.SetTitle( temp );
	for( size_t counter = 0; counter < alliedTowns.size(); ++counter )
		Ally.AddData( cwmWorldState->townRegions[alliedTowns[counter]]->GetName(), " " );

	Ally.Send( 4, false, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ForceEarlyElection( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces early election for townregion
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::ForceEarlyElection( void )
{
	time_t now;
	time(&now);
	CChar *mayor	= GetMayor();
	timeToNextPoll	= static_cast<SI32>(now);	// time to open poll
	TellMembers( 1174 );
	for( size_t counter = 0; counter < townMember.size(); ++counter )
		townMember[counter].targVote = INVALIDSERIAL;
	if( ValidateObject( mayor ) )
		mayor->SetTownpriv( 1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendEnemyTowns( CSocket *sock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends list of townregion's enemy townregions to client
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::SendEnemyTowns( CSocket *sock )
{
	GumpDisplay Enemy( sock, 300, 300 );

	UI08 enemyCount = 0;
	TOWNMAP_CITERATOR tIter	= cwmWorldState->townRegions.begin();
	TOWNMAP_CITERATOR tEnd	= cwmWorldState->townRegions.end();
	TOWNMAP_CITERATOR ourTown = cwmWorldState->townRegions.find( regionNum );
	while( tIter != tEnd )
	{
		CTownRegion *myReg = tIter->second;
		if( myReg != nullptr )
		{
			if( tIter != ourTown && Races->CompareByRace( race, myReg->GetRace() ) <= RACE_ENEMY )	// if we're racial enemies, and not the same as ourselves
			{
				++enemyCount;
				Enemy.AddData( myReg->GetName(), Races->Name( myReg->GetRace() ) );
			}
		}
		++tIter;
	}

	Enemy.SetTitle( strutil::format("Enemy Towns (%u)", enemyCount ) );
	Enemy.Send( 4, false, INVALIDSERIAL );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Possess( CTownRegion *possessorTown )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Another townregion takes possession of the townregion, and clears out old
//|					memberlist for the possessed townregion
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetReserves( void ) const
//|					void SetReserves( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets collected resource reserves for townregion
//o-----------------------------------------------------------------------------------------------o
UI32 CTownRegion::GetReserves( void ) const
{
	return resourceCollected;
}
void CTownRegion::SetReserves( UI32 newValue )
{
	resourceCollected = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetTaxes( void ) const
//|					void SetTaxesLeft( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets collected gold reserves for townregion
//o-----------------------------------------------------------------------------------------------o
UI32 CTownRegion::GetTaxes( void ) const
{
	return goldReserved;
}
void CTownRegion::SetTaxesLeft( UI32 newValue )
{
	goldReserved = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetChanceBigOre( void ) const
//|					void SetChanceBigOre( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets chance to find big ore in the townregion
//o-----------------------------------------------------------------------------------------------o
UI08 CTownRegion::GetChanceBigOre( void ) const
{
	return chanceFindBigOre;
}
void CTownRegion::SetChanceBigOre( UI08 newValue )
{
	chanceFindBigOre = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RemoveCharacter( size_t position )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove character from townregion's member list
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::RemoveCharacter( size_t position )
{
	townMember.erase( townMember.begin() + position );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetNumOrePreferences( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the number of ore preferences present for the townregion
//o-----------------------------------------------------------------------------------------------o
size_t CTownRegion::GetNumOrePreferences( void ) const
{
	return orePreferences.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const orePref *GetOrePreference( size_t targValue ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the townregion's preference for a specified ore
//o-----------------------------------------------------------------------------------------------o
const orePref *CTownRegion::GetOrePreference( size_t targValue ) const
{
	if( targValue >= orePreferences.size() )
		return nullptr;
	return &orePreferences[targValue];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetOreChance( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the chance to find ore when mining in the townregion
//o-----------------------------------------------------------------------------------------------o
SI32 CTownRegion::GetOreChance( void ) const
{
	SI32 sumReturn = 0;
	std::vector< orePref >::const_iterator oIter;
	for( oIter = orePreferences.begin(); oIter != orePreferences.end(); ++oIter )
		sumReturn += (*oIter).percentChance;
	return sumReturn;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsDungeon( void ) const
//|					void IsDungeon( bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether townregion is a dungeon
//o-----------------------------------------------------------------------------------------------o
bool CTownRegion::IsDungeon( void ) const
{
	return priv.test( BIT_DUNGEON );
}
void CTownRegion::IsDungeon( bool value )
{
	priv.set( BIT_DUNGEON, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 NumGuards( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the number of guards in the townregion
//o-----------------------------------------------------------------------------------------------o
UI16 CTownRegion::NumGuards( void ) const
{
	return numGuards;
}
void CTownRegion::SetNumGuards( UI16 newValue )
{
	numGuards = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 WorldNumber( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the worldnumber of the townregion
//o-----------------------------------------------------------------------------------------------o
UI08 CTownRegion::WorldNumber( void ) const
{
	return worldNumber;
}
void CTownRegion::WorldNumber( UI08 newValue )
{
	worldNumber = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetInstanceID( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the worldnumber of the townregion
//o-----------------------------------------------------------------------------------------------o
UI16 CTownRegion::GetInstanceID( void ) const
{
	return instanceID;
}
void CTownRegion::SetInstanceID( UI16 newValue )
{
	instanceID = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TaxedAmount( UI16 amount )
//|					UI16 TaxedAmount( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of gold taxed from townregion members
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::TaxedAmount( UI16 amount )
{
	taxedAmount = amount;
}
UI16 CTownRegion::TaxedAmount( void ) const
{
	return taxedAmount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetScriptTriggers( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of script triggers on object
//o-----------------------------------------------------------------------------------------------o
std::vector<UI16> CTownRegion::GetScriptTriggers( void )
{
	return scriptTriggers;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddScriptTrigger( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a script trigger to object's list of script triggers
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::AddScriptTrigger( UI16 newValue )
{
	if( std::find(scriptTriggers.begin(), scriptTriggers.end(), newValue) == scriptTriggers.end() )
	{
		// Add scriptID to scriptTriggers if not already present
		scriptTriggers.push_back(newValue);
	}

	// Sort vector in ascending order, so order in which scripts are evaluated is predictable
	std::sort(scriptTriggers.begin(), scriptTriggers.end());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveScriptTrigger( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a specific script trigger to object's list of script triggers
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::RemoveScriptTrigger( UI16 newValue )
{
	// Remove all elements containing specified script trigger from vector
	scriptTriggers.erase(std::remove(scriptTriggers.begin(), scriptTriggers.end(), newValue), scriptTriggers.end());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearScriptTriggers( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears out all script triggers from object
//o-----------------------------------------------------------------------------------------------o
void CTownRegion::ClearScriptTriggers( void )
{
	scriptTriggers.clear();
	scriptTriggers.shrink_to_fit();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetRegionNum( void ) const
//|					void SetRegionNum( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets region number (from region DFNs) for the townregion
//o-----------------------------------------------------------------------------------------------o
UI16 CTownRegion::GetRegionNum( void ) const
{
	return regionNum;
}
void CTownRegion::SetRegionNum( UI16 newVal )
{
	regionNum = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetNumLocations( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of locations in townregion as defined in region DFNs via X1/Y1, X2/Y2 pairs
//o-----------------------------------------------------------------------------------------------o
size_t CTownRegion::GetNumLocations( void ) const
{
	return locations.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const regLocs *GetLocation( size_t locNum ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets specified location in townregion
//o-----------------------------------------------------------------------------------------------o
const regLocs *CTownRegion::GetLocation( size_t locNum ) const
{
	if( locNum >= locations.size() )
		return nullptr;
	return &locations[locNum];
}
