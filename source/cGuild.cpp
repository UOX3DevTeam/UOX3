#include "uox3.h"
#include "cGuild.h"
#include "speech.h"
#include "ssection.h"
#include "scriptc.h"
#include "classes.h"
#include "CPacketSend.h"
#include "Dictionary.h"
#ifndef va_start
	#include <cstdarg>
//using namespace std;
#endif

namespace UOX
{

CGuildCollection *GuildSys;

#define DEFAULTCHARTER "UOX3 DevTeam Guildstone"
#define DEFAULTWEBPAGE "http://www.uox3dev.net/"

CGuild::CGuild() : name( "" ), charter( "" ), webpage( "" ), stone( INVALIDSERIAL ), master( INVALIDSERIAL ), gType( GT_STANDARD )
{
	abbreviation[0] = 0;
	recruits.resize( 0 );
	members.resize( 0 );
	recruitPtr		= recruits.end();
	memberPtr		= members.end();
	warPtr			= relationList.end();
	allyPtr			= relationList.end();
}

CGuild::~CGuild()
{
	recruits.clear();
	members.clear();
	relationList.clear();
}

//o---------------------------------------------------------------------------o
//|   Function    -  GuildID FirstWar( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Moves to start of the relation list and returns the next
//|					 relation which is at war
//o---------------------------------------------------------------------------o
GUILDID CGuild::FirstWar( void )
{
	warPtr = relationList.end();
	return NextWar();
}

//o---------------------------------------------------------------------------o
//|   Function    -  GuildID NextWar( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Moves through relation list, returning guild ID of next 
//|					 guild at war, or -1 if no war
//o---------------------------------------------------------------------------o
GUILDID CGuild::NextWar( void )
{
	while( warPtr != relationList.end() )
	{
		if( warPtr->second == GR_WAR )
			return warPtr->first;

		++warPtr;
	}
	return -1;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool FinishedWar( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if at end of war list, or false if not
//|					 Must iterate through war list if not at end of relation list
//|					 Moves back one if it does find something it's at war with
//|					 to ensure pointer integrity
//o---------------------------------------------------------------------------o
bool CGuild::FinishedWar( void )
{
	if( warPtr == relationList.end() )
		return true;
	if( NextWar() == -1 )
		return true;
	else if( warPtr != relationList.begin() )	// we move back if we found it, so NextWar will return a valid warring guild
		--warPtr;
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    -  GuildID FirstAlly( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Moves to start of the relation list and returns the next
//|					 relation which is at ally
//o---------------------------------------------------------------------------o
GUILDID CGuild::FirstAlly( void )
{
	allyPtr = relationList.begin();
	return NextAlly();
}

//o---------------------------------------------------------------------------o
//|   Function    -  GuildID NextAlly( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Moves through relation list, returning guild ID of next 
//|					 guild as ally, or -1 if no ally
//o---------------------------------------------------------------------------o
GUILDID CGuild::NextAlly( void )
{
	while( allyPtr != relationList.end() )
	{
		if( allyPtr->second == GR_ALLY )
			return allyPtr->first;

		++allyPtr;
	}
	return -1;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool FinishedAlly( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if at end of ally list, or false if not
//|					 Must iterate through ally list if not at end of relation list
//|					 Moves back one if it does find something it's an ally with
//|					 to ensure pointer integrity
//o---------------------------------------------------------------------------o
bool CGuild::FinishedAlly( void )
{
	if( allyPtr == relationList.end() )
		return true;
	if( NextAlly() == -1 )
		return true;
	else if( allyPtr != relationList.begin() )	// we move back if we found it, so NextWar will return a valid warring guild
		--allyPtr;
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    -  const std::string Name( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the name of the guild
//o---------------------------------------------------------------------------o
const std::string CGuild::Name( void ) const
{
	return name;
}

//o---------------------------------------------------------------------------o
//|   Function    -  const char *Abbreviation( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the abbreviation of the guild
//o---------------------------------------------------------------------------o
const char *CGuild::Abbreviation( void ) const
{
	return abbreviation;
}

//o---------------------------------------------------------------------------o
//|   Function    -  GuildType Type( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the guild's type
//|					 GT_STANDARD = 0,
//|					 GT_ORDER,
//|					 GT_CHAOS,
//|					 GT_UNKNOWN,
//|					 GT_COUNT
//o---------------------------------------------------------------------------o
GuildType CGuild::Type( void ) const
{
	return gType;
}

//o---------------------------------------------------------------------------o
//|   Function    -  const std::string Charter( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the charter of the guild
//o---------------------------------------------------------------------------o
const std::string CGuild::Charter( void ) const
{
	return charter;
}

//o---------------------------------------------------------------------------o
//|   Function    -  const std::string Webpage( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the webpage of the guild
//o---------------------------------------------------------------------------o
const std::string CGuild::Webpage( void ) const
{
	return webpage;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL Stone( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the guild stone (if any)
//o---------------------------------------------------------------------------o
SERIAL CGuild::Stone( void ) const
{
	return stone;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL Master( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the guild master (if any)
//o---------------------------------------------------------------------------o
SERIAL CGuild::Master( void ) const
{
	return master;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL FirstRecruit( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the first recruit in the recruit list
//|					 If no recruits, returns INVALIDSERIAL
//o---------------------------------------------------------------------------o
SERIAL CGuild::FirstRecruit( void )
{
	SERIAL retVal	= INVALIDSERIAL;
	recruitPtr		= recruits.begin();
	if( !FinishedRecruits() )
		retVal = (*recruitPtr);
	return retVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL NextRecruit( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the next recruit in the recruit list
//|					 If there are no more, it returns INVALIDSERIAL
//o---------------------------------------------------------------------------o
SERIAL CGuild::NextRecruit( void )
{
	SERIAL retVal = INVALIDSERIAL;
	if( !FinishedRecruits() )
	{
		++recruitPtr;
		if( !FinishedRecruits() )
			retVal = (*recruitPtr);
	}
	return retVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool FinishedRecruits( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if there are no more recruits left
//o---------------------------------------------------------------------------o
bool CGuild::FinishedRecruits( void )
{
	return ( recruitPtr == recruits.end() );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL RecruitNumber( int rNum )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the recruit in slot rNum
//|					 If rNum is invalid, it returns INVALIDSERIAL
//o---------------------------------------------------------------------------o
SERIAL CGuild::RecruitNumber( size_t rNum ) const
{
	if( rNum >= recruits.size() )
		return INVALIDSERIAL;
	else
		return recruits[rNum];
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL MemberNumber( int rNum )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the member in slot rNum
//|					 If rNum is invalid, it returns INVALIDSERIAL
//o---------------------------------------------------------------------------o
SERIAL CGuild::MemberNumber( size_t rNum ) const
{
	if( rNum >= members.size() )
		return INVALIDSERIAL;
	else
		return members[rNum];
}


//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL FirstMember( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the first member of the guild
//|					 If no members, returns INVALIDSERIAL
//o---------------------------------------------------------------------------o
SERIAL CGuild::FirstMember( void )
{
	SERIAL retVal	= INVALIDSERIAL;
	memberPtr		= members.begin();
	if( !FinishedMember() )
		retVal = (*memberPtr);
	return retVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL NextMember( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the serial of the next member of the guild, if any
//|					 If none, it returns INVALIDSERIAL
//o---------------------------------------------------------------------------o
SERIAL CGuild::NextMember( void )
{
	SERIAL retVal = INVALIDSERIAL;
	if( !FinishedMember() )
	{
		++memberPtr;	// post ++ forces a copy constructor
		if( !FinishedMember() )
			retVal = (*memberPtr);
	}
	return retVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool FinishedMember( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if there are no more members left
//o---------------------------------------------------------------------------o
bool CGuild::FinishedMember( void )
{
	return ( memberPtr == members.end() );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Name( std::string txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the name of the guild to txt.  If a stone exists, it
//|					 updates the stone's name
//o---------------------------------------------------------------------------o
void CGuild::Name( std::string txt )
{
	name = txt;
	if( Stone() != INVALIDSERIAL )	// we have a guildstone
	{
		CItem *gStone = calcItemObjFromSer( Stone() );
		if( ValidateObject( gStone ) )
			gStone->SetName( txt );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Abbreviation( const char *txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's abbreviation to txt
//o---------------------------------------------------------------------------o
void CGuild::Abbreviation( const char *txt )
{
	if( strlen( txt ) > 3 )
	{
		strncpy( abbreviation, txt, 3 );
		abbreviation[3] = 0;
	}
	else
		strcpy( abbreviation, txt );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Type( GuildType nType )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's type to nType
//o---------------------------------------------------------------------------o
void CGuild::Type( GuildType nType )
{
	gType = nType;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Charter( std::string txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's charter to txt
//o---------------------------------------------------------------------------o
void CGuild::Charter( std::string txt )
{
	charter = txt;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Webpage( std::string txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's webpage to txt
//o---------------------------------------------------------------------------o
void CGuild::Webpage( std::string txt )
{
	webpage = txt;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Stone( SERIAL newStone )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's stone serial to newStone
//o---------------------------------------------------------------------------o
void CGuild::Stone( SERIAL newStone )
{
	stone = newStone;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Stone( CItem &newStone )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's stone serial to the CItem newStone
//o---------------------------------------------------------------------------o
void CGuild::Stone( CItem &newStone )
{
	stone = newStone.GetSerial();
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Master( SERIAL newMaster )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's master serial to newMaster
//o---------------------------------------------------------------------------o
void CGuild::Master( SERIAL newMaster )
{
	master = newMaster;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Master( CChar &newMaster )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's master serial to CChar newMaster's serial
//o---------------------------------------------------------------------------o
void CGuild::Master( CChar &newMaster )
{
	master = newMaster.GetSerial();
}
void CGuild::NewRecruit( CChar &newRecruit )
{
	NewRecruit( newRecruit.GetSerial() );
}
void CGuild::NewRecruit( SERIAL newRecruit )
{
	if( IsMember( newRecruit ) )
		RemoveMember( newRecruit );
	recruits.push_back( newRecruit );
}
void CGuild::NewMember( CChar &newMember )
{
	NewMember( newMember.GetSerial() );
}
void CGuild::NewMember( SERIAL newMember )
{
	if( IsRecruit( newMember ) )
		RemoveRecruit( newMember );
	members.push_back( newMember );
}
void CGuild::RemoveRecruit( CChar &newRecruit )
{
	RemoveRecruit( newRecruit.GetSerial() );
}
void CGuild::RemoveRecruit( SERIAL newRecruit )
{
	SERLIST_ITERATOR toFind = recruits.begin();
	while( toFind != recruits.end() )
	{
		if( (*toFind) == newRecruit )
		{
			if( recruitPtr != recruits.begin() && recruitPtr >= toFind )
				--recruitPtr;
			recruits.erase( toFind );
			return;
		}
		++toFind;
	}
}
void CGuild::RemoveMember( CChar &newMember )
{
	RemoveMember( newMember.GetSerial() );
}
void CGuild::RemoveMember( SERIAL newMember )
{
	SERLIST_ITERATOR toFind = members.begin();
	while( toFind != members.end() )
	{
		if( (*toFind) == newMember )
		{
			if( memberPtr != members.begin() && memberPtr >= toFind )
				--memberPtr;
			members.erase( toFind );
			return;
		}
		++toFind;
	}
}
void CGuild::RecruitToMember( CChar &newMember )
{
	RemoveRecruit( newMember );
	NewMember( newMember );
}
void CGuild::RecruitToMember( SERIAL newMember )
{
	RemoveRecruit( newMember );
	NewMember( newMember );
}

bool CGuild::IsRecruit( SERIAL toCheck ) const
{
	SERLIST_CITERATOR toFind = recruits.begin();
	while( toFind != recruits.end() )
	{
		if( (*toFind) == toCheck )
			return true;
		++toFind;
	}
	return false;
}
bool CGuild::IsMember( SERIAL toCheck ) const
{
	SERLIST_CITERATOR toFind = members.begin();
	while( toFind != members.end() )
	{
		if( (*toFind) == toCheck )
			return true;
		++toFind;
	}
	return false;
}
bool CGuild::IsRecruit( CChar &toCheck ) const
{
	return IsRecruit( toCheck.GetSerial() );
}
bool CGuild::IsMember( CChar &toCheck ) const
{
	return IsMember( toCheck.GetSerial() );
}


GUILDRELATION CGuild::RelatedToGuild( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return GR_UNKNOWN;
	else
		return toFind->second;
}
bool CGuild::IsAtWar( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return false;
	else
		return ( toFind->second == GR_WAR );
}
bool CGuild::IsNeutral( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return false;
	else
		return ( toFind->second == GR_NEUTRAL );
}
bool CGuild::IsAlly( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return false;
	else
		return ( toFind->second == GR_ALLY );
}

void CGuild::SetGuildRelation( GUILDID otherGuild, GUILDRELATION toSet )
{
	relationList[otherGuild] = toSet;
}

GUILDREL *CGuild::GuildRelationList( void )	// NOTE: This is aimed ONLY at menu stuff
{
	return &relationList;
}

void CGuild::Save( std::ofstream &toSave, GUILDID gNum )
{
	toSave << "[GUILD " << gNum << ']' << std::endl << "{" << std::endl;
	toSave << "NAME=" << name << std::endl;
	toSave << "ABBREVIATION=" << abbreviation << std::endl;
	toSave << "TYPE=" << GTypeNames[gType] << std::endl;
	toSave << "CHARTER=" << charter << std::endl;
	toSave << "WEBPAGE=" << webpage << std::endl;
	toSave << "STONE=" << stone << std::endl;
	toSave << "MASTER=" << master << std::endl;
	SERLIST_ITERATOR counter;
	counter = recruits.begin();
	while( counter != recruits.end() )
	{
		toSave << "RECRUIT=" << (*counter) << std::endl;
		++counter;
	}
	counter = members.begin();
	while( counter != members.end() )
	{
		toSave << "MEMBER=" << (*counter) << std::endl;
		++counter;
	}
	GUILDREL::const_iterator relly = relationList.begin();
	while( relly != relationList.end() )
	{
		toSave << GRelationNames[relly->second] << " " << relly->first <<std::endl;
		++relly;
	}
	toSave << "}" << std::endl << std::endl;
}
void CGuild::Load( ScriptSection *toRead )
{
	UString tag;
	UString data;
	UString UTag;
	for( tag = toRead->First(); !toRead->AtEnd(); tag = toRead->Next() )
	{
		data = toRead->GrabData();
		if( tag.empty() )
			continue;
		UTag = tag.upper();
		switch( (tag.data()[0]) )
		{
			case '{':
			case '/':	break;	// open section, comment, we don't really care ;)
			case 'A':
				if( UTag == "ABBREVIATION" )
					Abbreviation( data.c_str() );
				else if( UTag == "ALLY" )
					SetGuildRelation( data.toShort(), GR_ALLY );
				break;
			case 'C':
				if( UTag == "CHARTER" )
					Charter( data );
				break;
			case 'M':
				if( UTag == "MASTER" )
					Master( data.toULong() );
				else if( UTag == "MEMBER" )
					NewMember( data.toULong() );
				break;
			case 'N':
				if( UTag == "NAME" )
					Name( data );
				else if( UTag == "NEUTRAL" )
					SetGuildRelation( data.toShort(), GR_NEUTRAL );
				break;
			case 'R':
				if( UTag == "RECRUIT" )
					NewRecruit( data.toULong() );
			case 'S':
				if( UTag == "STONE" )
					Stone( data.toULong() );
				break;
			case 'T':
				if( UTag == "TYPE" )
				{
					for( GuildType gt = GT_STANDARD; gt < GT_COUNT; gt = (GuildType)(gt + (GuildType)1) )
					{
						if( data == GTypeNames[gt] )
						{
							Type( gt );
							break;
						}
					}
				}
				break;
			case 'U':
				if( UTag == "UNKNOWN" )
					SetGuildRelation( data.toShort(), GR_UNKNOWN );
				break;
			case 'W':
				if( UTag == "WEBPAGE" )
					Webpage( data );
				else if( UTag == "WAR" )
					SetGuildRelation( data.toShort(), GR_WAR );
				break;
		}
	}
}

size_t CGuild::NumMembers( void ) const
{
	return members.size();
}
size_t CGuild::NumRecruits( void ) const
{
	return recruits.size();
}

void CGuild::CalcMaster( void )
{
	if( members.empty() )
	{
		Master( INVALIDSERIAL );
		return;
	}
	std::vector< int > votes;
	votes.resize( members.size() );
	UI32 maxIndex = 0;

	for( size_t counter = 0; counter < votes.size(); ++counter )
	{
		votes[counter] = 0;	// init the count before adding
		for( size_t counter2 = 0; counter2 < votes.size(); ++counter2 )
		{
			CChar *myChar = calcCharObjFromSer( members[counter2] );
			if( ValidateObject( myChar ) && myChar->GetGuildFealty() == members[counter] )
				++votes[counter];
		}
		if( votes[counter] > votes[maxIndex] )
			maxIndex = counter;
	}

	Master( members[maxIndex] );
}

void CGuild::TellMembers( SI32 dictEntry, ... )
{
	char tmpMsg[512];

	SERLIST_CITERATOR cIter;
	for( cIter = members.begin(); cIter != members.end(); ++cIter )
	{
		CChar *targetChar	= calcCharObjFromSer( (*cIter) );
		CSocket *targetSock	= calcSocketObjFromChar( targetChar );
		if( targetSock != NULL )
		{
			std::string txt = "GUILD: ";
			txt += Dictionary->GetEntry( dictEntry, targetSock->Language() );

			va_list argptr;
			va_start( argptr, dictEntry );
			vsprintf( tmpMsg, txt.c_str(), argptr );
			va_end( argptr );

			CSpeechEntry *toAdd = SpeechSys->Add();
			toAdd->Speech( tmpMsg );
			toAdd->Font( FNT_NORMAL );
			toAdd->Speaker( INVALIDSERIAL );
			toAdd->SpokenTo( (*cIter) );
			toAdd->Colour( 0x000B );
			toAdd->Type( SYSTEM );
			toAdd->At( cwmWorldState->GetUICurrentTime() );
			toAdd->TargType( SPTRG_INDIVIDUAL );
		}
	}
}

const std::string CGuild::TypeName( void )
{
	return GTypeNames[Type()];
}

CGuildCollection::CGuildCollection()
{
}
size_t CGuildCollection::NumGuilds( void ) const
{
	return gList.size();
}

GUILDID CGuildCollection::MaximumGuild( void )
{
	GUILDID maxVal = -1;
	GUILDLIST::const_iterator pFind = gList.begin();
	while( pFind != gList.end() )
	{
		if( pFind->first > maxVal )
			maxVal = pFind->first;
		++pFind;
	}
	return static_cast<GUILDID>(maxVal + 1);
}
GUILDID CGuildCollection::NewGuild( void )
{
	CGuild *toAdd = new CGuild();
	GUILDID gAdd = MaximumGuild();
	gList[gAdd] = toAdd;
	return gAdd;
}
CGuild *CGuildCollection::Guild( GUILDID num ) const
{
	GUILDLIST::const_iterator pFind = gList.find( num );
	if( pFind == gList.end() )
		return NULL;
	return pFind->second;
}
CGuild *CGuildCollection::operator[]( GUILDID num )
{
	return Guild( num );
}
void CGuildCollection::Save( void )
{
	Console << "Saving guild data.... ";
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "guilds.wsc";
	std::ofstream toSave( filename.c_str() );
	GUILDLIST::const_iterator pMove = gList.begin();
	while( pMove != gList.end() )
	{
		(pMove->second)->Save( toSave, pMove->first );
		++pMove;
	}
	Console.PrintDone();

}
void CGuildCollection::Load( void )
{
	std::string filename	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "guilds.wsc";
	if( FileExists( filename ) )
	{
		Script newScript( filename, NUM_DEFS, false );
		ScriptSection *testSect	= NULL;
		GUILDID guildNum		= 0;
		for( testSect = newScript.FirstEntry(); testSect != NULL; testSect = newScript.NextEntry() )
		{
			UString text	= newScript.EntryName();
			text = text.substr( 6 );
			guildNum		= text.toShort();
			if( gList[guildNum] != NULL )
				delete gList[guildNum];
			gList[guildNum] = new CGuild();
			gList[guildNum]->Load( testSect );
		}
	}
}

GUILDRELATION CGuildCollection::Compare( GUILDID srcGuild, GUILDID trgGuild ) const
{
	if( srcGuild == -1 || trgGuild == -1 )
		return GR_UNKNOWN;
	if( srcGuild == trgGuild )
		return GR_SAME;
	CGuild *mGuild = Guild( srcGuild );
	if( mGuild == NULL )
		return GR_UNKNOWN;
	return mGuild->RelatedToGuild( trgGuild );
}

GUILDRELATION CGuildCollection::Compare( CChar *src, CChar *trg ) const
{
	if( src == NULL || trg == NULL )
		return GR_UNKNOWN;
	if( src->GetGuildNumber() == -1 || trg->GetGuildNumber() == -1 )
		return GR_UNKNOWN;
	if( src->GetGuildNumber() == trg->GetGuildNumber() )
		return GR_SAME;
	return Compare( src->GetGuildNumber(), trg->GetGuildNumber() );
}

void CGuildCollection::Menu( CSocket *s, SI16 menu, GUILDID trgGuild, SERIAL plID )
{
	if( s == NULL )
		return;
	if( trgGuild >= static_cast<SI32>(NumGuilds()) )
		return;

	CPSendGumpMenu toSend;
	toSend.GumpID( menu );
	toSend.UserID( INVALIDSERIAL );

	toSend.AddCommand( "nomove" );
	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 600 400", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 560 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 120 10 %u 0", cwmWorldState->ServerData()->TitleColour() );
	toSend.AddCommand( "page 1" );

	SERIAL gMaster	= gList[trgGuild]->Master();
	CChar *mChar	= s->CurrcharObj();
	CChar *gMstr	= calcCharObjFromSer( gMaster );
	UI16 numButtons = 0, numText = 0, numColumns = 1;

	char guildfealty[30], guildt[10], toggle[4];
	strcpy( guildfealty, "yourself" );
	if( mChar->GetGuildFealty() != mChar->GetSerial() && mChar->GetGuildFealty() != INVALIDSERIAL )
	{
		CChar *fChar = calcCharObjFromSer( mChar->GetGuildFealty() );
		if( ValidateObject( fChar ) )
			strcpy( guildfealty, fChar->GetName().c_str() );
	}
	else
		mChar->SetGuildFealty( mChar->GetSerial() );
	switch( gList[trgGuild]->Type() )
	{
		case 0:	
			strcpy( guildt, " Standard" );
			break;
		case 1:	
			strcpy( guildt, "n Order" );
			break;
		case 2:	
			strcpy( guildt, " Chaos" );
			break;
		default:
			strcpy( guildt, "INVALID" );
			break;
	}
	strcpy( toggle, "Off" );
	if( mChar->GetGuildToggle() )
		strcpy( toggle, "On" );

	std::string gName	= gList[trgGuild]->Name();
	UI16 tCtr			= 0;
	SERIAL tChar		= 0;
	GUILDREL::iterator toCheck;
	GUILDREL *ourList;
	s->TempInt( trgGuild );
	UnicodeTypes sLang	= s->Language();

	UI32 tCounter = 0;

	switch( menu )
	{
		case -1:			break;
		case BasePage:		break;
		case BasePage+1:	numButtons = 10;			// Main menu
			toSend.AddText( Dictionary->GetEntry( 102, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 103, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 104, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 105, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 106, sLang ), guildfealty );
			toSend.AddText( Dictionary->GetEntry( 107, sLang ), toggle );
			toSend.AddText( Dictionary->GetEntry( 108, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 109, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 110, sLang ), gName.c_str() );
			toSend.AddText( Dictionary->GetEntry( 111, sLang ), gName.c_str() );
			toSend.AddText( Dictionary->GetEntry( 112, sLang ) );

			if( mChar->GetSerial() == gMaster || mChar->IsGM() )	// Guildmaster Access?
			{
				++numButtons;
				toSend.AddText( Dictionary->GetEntry( 113, sLang ), gMstr->GetGuildTitle().c_str() );
			}
			break;
		case BasePage+2:	numButtons = 16;		// Guildmaster menu
			toSend.AddText( Dictionary->GetEntry( 114, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 115, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 116, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 117, sLang ), guildt );
			for( tCounter = 118; tCounter <= 130; ++tCounter )
				toSend.AddText( Dictionary->GetEntry( tCounter, sLang ) );
			break;
		case BasePage+3:	numButtons = 4;			// Guild type
			toSend.AddText( Dictionary->GetEntry( 131, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 133, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 134, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 135, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			break;
		case BasePage+4:	numButtons = 3;			// Set charter
			toSend.AddText( Dictionary->GetEntry( 136, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 138, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 139, sLang ) );
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			break;
		case BasePage+5:	numButtons = 3;			// View charter
			toSend.AddText( Dictionary->GetEntry( 140, sLang ) );
			toSend.AddText( gList[trgGuild]->Charter() );
			toSend.AddText( Dictionary->GetEntry( 142, sLang ), gList[trgGuild]->Webpage().c_str() );
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			break;
		case BasePage+6:								// List of recruits
			toSend.AddText( Dictionary->GetEntry( 143, sLang ) );
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
			{
				toSend.AddText( calcCharObjFromSer( tChar )->GetName() );
				++tCtr;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;
			break;
		case BasePage+7:								// List of members
			toSend.AddText( Dictionary->GetEntry( 144, sLang ) );
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstMember(); !gList[trgGuild]->FinishedMember(); tChar = gList[trgGuild]->NextMember() )
			{
				toSend.AddText( calcCharObjFromSer( tChar )->GetName() );
				++tCtr;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;
			break;
		case BasePage+8:								// Member dismiss
			toSend.AddText( Dictionary->GetEntry( 145, sLang ) );
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstMember(); !gList[trgGuild]->FinishedMember(); tChar = gList[trgGuild]->NextMember() )
			{
				toSend.AddText( calcCharObjFromSer( tChar )->GetName() );
				++tCtr;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+9:								// Dismiss recruit
			toSend.AddText( Dictionary->GetEntry( 146, sLang ) );
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
			{
				toSend.AddText( calcCharObjFromSer( tChar )->GetName() );
				++tCtr;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+10:								// Accept recruit
			toSend.AddText( Dictionary->GetEntry( 147, sLang ) );
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
			{
				toSend.AddText( calcCharObjFromSer( tChar )->GetName() );
				++tCtr;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+11:								// War list
			toSend.AddText( Dictionary->GetEntry( 148, sLang ) );
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_WAR )
				{
					toSend.AddText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+12:								// Grant title
			numButtons = 1;
			toSend.AddText( "Unfilled functionality" );
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			break;
		case BasePage+13:								// Declare war list
			toSend.AddText( Dictionary->GetEntry( 149, sLang ) );
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_WAR )
				{
					toSend.AddText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+14:								// Declare peace list
			toSend.AddText( Dictionary->GetEntry( 150, sLang ) );
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_WAR )
				{
					toSend.AddText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+15:								// List of guilds warring on us
		case BasePage+16:								// Display recruit information
			numButtons = 1;
			toSend.AddText( "Unfilled functionality" );
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			break;
		case BasePage+17:								// Display member information
			CChar *kChar;
			kChar = calcCharObjFromSer( plID );
			if( ValidateObject( kChar ) )
			{
				char temp[128];
				sprintf( temp, "%s (", kChar->GetName().c_str() );
				if( kChar->IsInnocent() )
					strcat( temp, "Innocent) " );
				else if( kChar->IsMurderer() )
					strcat( temp, "Murderer) " );
				else
					strcat( temp, "Criminal) " );
				toSend.AddText( temp );
				// To display: Name, str, dex, int, # kills, # deaths, status (criminal, murderer, innocent), top x skills
				toSend.AddText( "Strength" );
				toSend.AddText( "%i", kChar->GetStrength() );
				toSend.AddText( "Dexterity" );
				toSend.AddText( "%i", kChar->GetDexterity() );
				toSend.AddText( "Intelligence" );
				toSend.AddText( "%i", kChar->GetIntelligence() );
				toSend.AddText( "Kills" );
				toSend.AddText( "%i", kChar->GetKills() );
				toSend.AddText( "Deaths" );
				toSend.AddText( "%u", kChar->GetDeaths() );
				numText = 10;
				numColumns = 2;
				numButtons = 1;
			}
			else
			{
				numButtons = 1;
				toSend.AddText( "Unknown player" );
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			break;
		case BasePage+18:								// Ally list
			toSend.AddText( Dictionary->GetEntry( 151, sLang ) );
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_ALLY )
				{
					toSend.AddText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
		case BasePage+19:								// Declare Ally list
			toSend.AddText( Dictionary->GetEntry( 152, sLang ) );
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_ALLY )
				{
					toSend.AddText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.AddText( Dictionary->GetEntry( 130, sLang ) );
			numButtons = ++tCtr;	break;
	}

	for( UI16 iCtr = 0; iCtr < numButtons; ++iCtr )
	{
		toSend.AddCommand( "button 20 %i %i %i 1 0 %i", 30 + 20 * iCtr, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, iCtr + 2 );
		toSend.AddCommand( "text 50 %i %i %i", 30 + 20 * iCtr, cwmWorldState->ServerData()->LeftTextColour(), iCtr + 1 );
	}
	if( numText != 0 )
	{
		for( UI16 iCtr = 0; iCtr < numText; ++iCtr )
		{
			if( numColumns == 1 )
				toSend.AddCommand( "text 50 %i %i %i", 30 + 20 * ( iCtr + numButtons ), cwmWorldState->ServerData()->LeftTextColour(), iCtr + numButtons + 1 );
			else
				toSend.AddCommand( "text %i %i %i %i", 50 + (iCtr % numColumns) * 100, 30 + 20 * ( ( iCtr / 2 ) + numButtons ), cwmWorldState->ServerData()->LeftTextColour(), iCtr + numButtons + 1 );
		}
	}
	toSend.Finalize();
	s->Send( &toSend );
}
void CGuildCollection::GumpInput( CPIGumpInput *gi )
{
	UI08 type		= gi->Type();
	UI08 index		= gi->Index();
	UString text	= gi->Reply();
	CSocket *s		= gi->GetSocket();

	if( type != 100 )
		return;
	GUILDID trgGuild	= static_cast<GUILDID>(s->TempInt());
	CChar *gMaster		= calcCharObjFromSer( gList[trgGuild]->Master() );
	switch( index )
	{
		case 1:	gList[trgGuild]->Name( text );			break; // set guild name
		case 2:	gList[trgGuild]->Abbreviation( text.c_str() );	break; // set guild abbreviation
		case 3:	gMaster->SetGuildTitle( text );			break; // set guildmaster title
		case 4:	gList[trgGuild]->Charter( text );		break;	// new charter
		case 5:	gList[trgGuild]->Webpage( text );		break;	// new webpage
	}
}

void CGuildCollection::ToggleAbbreviation( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	GUILDID guildnumber = mChar->GetGuildNumber();
	if( guildnumber == -1 )
		return;

	if( gList[guildnumber]->Type() != GT_STANDARD )						// Check for Order/Chaos
		s->sysmessage( 153 );	// They may not toggle it off!
	else
	{
		mChar->SetGuildToggle( !mChar->GetGuildToggle() );
		if( mChar->GetGuildToggle() )						// If set to On then
			s->sysmessage( 154 );	// Tell player about the change
		else													// Otherwise
			s->sysmessage( 155 );	// And tell him also
	}
}

void TextEntryGump( CSocket *s, SERIAL ser, char type, char index, SI16 maxlength, SI32 dictEntry );

void CGuildCollection::GumpChoice( CSocket *s )
{
	UI32 realType		= s->GetDWord( 7 );
	UI32 button			= s->GetDWord( 11 );
	GUILDID trgGuild	= (GUILDID)s->TempInt();
	if( button == 1 )	// hit cancel
		return;
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	SERIAL ser = mChar->GetSerial();
	UI16 tCtr = 0;
	GUILDREL::iterator toCheck;
	GUILDREL *ourList;
	int offCounter;

	CChar *tChar = NULL;

	switch( realType )
	{
		case INVALIDSERIAL:
		case BasePage:
		case BasePage+1:								// Main menu
			switch( button )
			{
				case 2:		s->target( 2, 0, 156 );				break;	// recruit into guild
				case 3:		Menu( s, BasePage + 7, trgGuild );	break;
				case 4:		Menu( s, BasePage + 5, trgGuild );	break;
				case 5:		s->target( 2, 1, 157 );				break;	// declare fealty
				case 6:		ToggleAbbreviation( s );			break;	// toggle abbreviation
				case 7:		Resign( s );						break;	//	Resign from guild
				case 8:		Menu( s, BasePage + 6, trgGuild );	break;	// View candidates
				case 9:		Menu( s, BasePage + 11, trgGuild );	break;	// View warring guilds
				case 10:	Menu( s, BasePage + 15, trgGuild );	break;	// View guilds that declared war on us
				case 11:	Menu( s, BasePage + 18, trgGuild );	break;	// View list of allied guilds
				case 12:	Menu( s, BasePage + 2, trgGuild );	break;	// Access guildmaster functions
			}
			break;
		case BasePage+2:								// Guildmaster menu
			switch( button )
			{
				case 2:		TextEntryGump( s, ser, 100, 1, 30, 158 );	break;	// set guild name
				case 3:		TextEntryGump( s, ser, 100, 2, 3,  159 );	break;	// set guild abbreviation
				case 4:		Menu( s, BasePage + 3, trgGuild );		break;	// set guild type
				case 5:		Menu( s, BasePage + 4, trgGuild );		break;	// set guild charter
				case 6:		Menu( s, BasePage + 8, trgGuild );		break;	// dismiss a member
				case 7:		Menu( s, BasePage + 13, trgGuild );		break;	// declare war from menu
				case 8:		s->target( 2, 2, 160 );					break;	// declare war from target
				case 9:		Menu( s, BasePage + 14, trgGuild );		break;	// declare peace
				case 10:	Menu( s, BasePage + 19, trgGuild );		break;	// declare ally from menu
				case 11:	s->target( 2, 3, 161 );			break;	// declare ally from target
				case 12:	Menu( s, BasePage + 10, trgGuild );		break;	// accept candidate seeking membership
				case 13:	Menu( s, BasePage + 9, trgGuild );		break;	// refuse candidate seeking membership
				case 14:	TextEntryGump( s, ser, 100, 3, 15,  162 );	break;	// set guild master's title
				case 15:	Menu( s, BasePage + 12, trgGuild );		break;	// grant title to another member
#if defined( _MSC_VER )
#pragma note( "Move guildstone functionality goes here" )
#endif
				case 16:											break;	// move guildstone
				case 17:	Menu( s, BasePage + 1, trgGuild );		break;	// return to main menu
			}
			break;
		case BasePage+3:								// Guild type
			switch( button )
			{
				case 2:		gList[trgGuild]->Type( GT_STANDARD );	break;
				case 3:		gList[trgGuild]->Type( GT_ORDER );		break;
				case 4:		gList[trgGuild]->Type( GT_CHAOS );		break;
				case 5:		Menu( s, BasePage + 2, trgGuild );		break;
			}
			break;
		case BasePage+4:								// Set charter
			switch( button )
			{
				case 2:		TextEntryGump( s, ser, 100, 4, 50, 163 );	break;
				case 3:		TextEntryGump( s, ser, 100, 5, 50, 164 );	break;
				case 4:		Menu( s, BasePage + 2, trgGuild );		break;
			}
			break;
		case BasePage+5:								// View charter
			switch( button )
			{
				case 3:		s->OpenURL( gList[trgGuild]->Webpage() );	break;
				case 2:
				case 4:		Menu( s, BasePage + 1, trgGuild );			break;
			}
			break;
		case BasePage+6:								// List of recruits
			if( gList[trgGuild]->NumRecruits() >= (button - 2) )
				Menu( s, BasePage + 1, trgGuild );
			else
				Menu( s, BasePage + 16, trgGuild, gList[trgGuild]->RecruitNumber( button - 2 ) );	// display recruit number
			break;
		case BasePage+7:								// List of members
			if( gList[trgGuild]->NumMembers() >= (button - 2) )
				Menu( s, BasePage + 1, trgGuild );
			else
				Menu( s, BasePage + 17, trgGuild, gList[trgGuild]->MemberNumber( button - 2 ) );	// display member number
			break;
		case BasePage+8:								// Member dismiss
			if( gList[trgGuild]->NumMembers() >= (button - 2) )
				Menu( s, BasePage + 2, trgGuild );
			else
			{
				tChar = calcCharObjFromSer( gList[trgGuild]->MemberNumber( button - 2 ) );
				if( ValidateObject( tChar ) )
				{
					gList[trgGuild]->RemoveMember( (*tChar) );
					tChar->SetGuildNumber( -1 );
				}
			}
			break;
		case BasePage+9:								// Dismiss recruit
			if( gList[trgGuild]->NumRecruits() >= (button - 2) )
				Menu( s, BasePage + 2, trgGuild );
			else
				gList[trgGuild]->RemoveRecruit( gList[trgGuild]->RecruitNumber( button - 2 ) );
			break;
		case BasePage+10:								// Accept recruit
			if( gList[trgGuild]->NumRecruits() >= (button - 2) )
				Menu( s, BasePage + 2, trgGuild );
			else
			{
				tChar = calcCharObjFromSer( gList[trgGuild]->RecruitNumber( button - 2 ) );
				if( ValidateObject( tChar ) )
				{
					gList[trgGuild]->RecruitToMember( (*tChar) );
					tChar->SetGuildNumber( trgGuild );
				}
			}
			break;
		case BasePage+11:								// War list
		case BasePage+12:								// Grant title
			Menu( s, BasePage + 2, trgGuild );
			break;
		case BasePage+13:								// Declare war list
			offCounter = tCtr = 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_WAR )
				{
					if( offCounter == button - 2 )
					{
						gList[trgGuild]->SetGuildRelation( tCtr, GR_ALLY );
						gList[trgGuild]->TellMembers( 165, gList[tCtr]->Name().c_str() );
						gList[tCtr]->TellMembers( 166, gList[trgGuild]->Name().c_str() );
						break;
					}
					else
						++offCounter;
				}
				++tCtr;
				++toCheck;
			}
			break;
		case BasePage+14:								// Declare peace list
			offCounter = tCtr = 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_WAR )
				{
					if( offCounter == button - 2 )
					{
						gList[trgGuild]->SetGuildRelation( tCtr, GR_NEUTRAL );
						gList[trgGuild]->TellMembers( 167, gList[tCtr]->Name().c_str() );
						gList[tCtr]->TellMembers( 168, gList[trgGuild]->Name().c_str() );
						break;
					}
					else
						++offCounter;
				}
				++tCtr;
				++toCheck;
			}
			break;
		case BasePage+15:								// List of guilds warring on us
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage+16:								// Display recruit information
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage+17:								// Display member information
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage+18:								// Ally list
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage+19:								// Declare Ally list
			offCounter = tCtr = 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_ALLY )
				{
					if( offCounter == button - 2 )
					{
						gList[trgGuild]->SetGuildRelation( tCtr, GR_ALLY );
						gList[trgGuild]->TellMembers( 169, gList[tCtr]->Name().c_str() );
						gList[tCtr]->TellMembers( 170, gList[trgGuild]->Name().c_str() );
						break;
					}
					else
						++offCounter;
				}
				++tCtr;
				++toCheck;
			}
			break;
	}
}
void CGuildCollection::Resign( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 guildnumber = mChar->GetGuildNumber();
	if( guildnumber == -1 )
		return;

	CGuild *nGuild = gList[guildnumber];
	if( nGuild == NULL )
		return;

	nGuild->RemoveMember( *(s->CurrcharObj()) );
	s->sysmessage( 171 );
	mChar->SetGuildNumber( -1 );
	if( nGuild->Master() == mChar->GetSerial() && nGuild->NumMembers() != 0 )
		nGuild->CalcMaster();

	if( nGuild->NumMembers() == 0 )
	{
		SERIAL stone = nGuild->Stone();
		if( stone != INVALIDSERIAL )
		{
			CItem *gStone = calcItemObjFromSer( stone );
			if( ValidateObject( gStone ) )
				gStone->Delete();
		}
		Erase( guildnumber );
		s->sysmessage( 172 );
	}
}

void CGuildCollection::Erase( GUILDID toErase )
{
	GUILDLIST::iterator pFind = gList.find( toErase );
	if( pFind == gList.end() )	// doesn't exist
		return;
	CGuild *gErase = pFind->second;
	if( gErase == NULL )
	{
		gList.erase( pFind );
		return;
	}
	for( size_t iCounter = 0; iCounter < gErase->NumMembers(); ++iCounter )
	{
		SERIAL iMember	= gErase->MemberNumber( iCounter );
		CChar *member	= calcCharObjFromSer( iMember );
		if( ValidateObject( member ) )
			member->SetGuildNumber( -1 );
	}
	for( size_t iC = 0; iC < gErase->NumRecruits(); ++iC )
	{
		SERIAL iRecruit	= gErase->RecruitNumber( iC );
		CChar *recruit	= calcCharObjFromSer( iRecruit );
		if( ValidateObject( recruit ) )
			recruit->SetGuildNumber( -1 );
	}
	delete pFind->second;
	gList.erase( pFind );
}

CGuildCollection::~CGuildCollection()
{
	GUILDLIST::const_iterator i = gList.begin();
	while( i != gList.end() )
	{
		if( i->second != NULL )
			delete i->second;
		++i;
	}

	gList.clear();
}


void CGuildCollection::PlaceStone( CSocket *s, CItem *deed )
{
	if( s == NULL || !ValidateObject( deed ) )
		return;
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	if( deed->GetID() == 0x14F0 )
	{
		if( mChar->GetGuildNumber() != -1 )	// in a guild
		{
			s->objMessage( 173, deed );
			return;
		}
		GUILDID gNum = NewGuild();
		CGuild *nGuild = Guild( gNum );
		if( nGuild == NULL )
		{
			s->objMessage( 174, deed );
			Console.Error( 3, "Critical error adding guildstone, memory allocation failed.  Attempted by player 0x%X", mChar->GetSerial() );
			return;
		}
		mChar->SetGuildNumber( gNum );
		s->TempInt( gNum );
		nGuild->NewMember( (*mChar) );
		CItem *stone = Items->CreateItem( NULL, mChar, 0x0ED5, 1, 0, OT_ITEM );
		if( stone == NULL )
		{
			s->objMessage( 176, deed );
			Console.Error( 3, "Critical error spawning guildstone, no stone made.  Attempted by player 0x%X", mChar->GetSerial() );
			return;
		}
		stone->SetName( Dictionary->GetEntry( 175 ) );
		if( mChar->GetID() == 0x0191 || mChar->GetID() == 0x0193 )
			mChar->SetGuildTitle( "Guildmistress" );
		else
			mChar->SetGuildTitle( "Guildmaster" );
		stone->SetLocation( mChar );
		nGuild->Webpage( DEFAULTWEBPAGE );
		nGuild->Charter( DEFAULTCHARTER );
		stone->SetType( IT_GUILDSTONE );
		stone->SetTempVar( CITV_MORE, gNum );
		stone->SetWipeable( false );
		stone->SetDecayable( false );
		deed->Delete();
		nGuild->Master( (*mChar) );
		nGuild->Stone( (*stone) );
		TextEntryGump( s, mChar->GetSerial(), 100, 1, 30, 158 );
	}
	else
		s->sysmessage( 177 );
}

bool CGuildCollection::ResultInCriminal( GUILDID srcGuild, GUILDID trgGuild ) const
{
	GUILDRELATION gRel = Compare( srcGuild, trgGuild );
	switch( gRel )
	{
		case GR_WAR:
		case GR_SAME:
			return false;
		case GR_ALLY:
		case GR_NEUTRAL:
		case GR_UNKNOWN:
		case GR_COUNT:
		default:
			return true;
	}
}
bool CGuildCollection::ResultInCriminal( CChar *src, CChar *trg ) const
{
	if( !ValidateObject( src ) || !ValidateObject( trg ) )
		return false;
	return ResultInCriminal( src->GetGuildNumber(), trg->GetGuildNumber() );
}

void CGuildCollection::DisplayTitle( CSocket *s, CChar *src ) const
{
	if( !ValidateObject( src ) || s == NULL )
		return;
	char title[150];
	char abbreviation[5];
	char guildtype[10] = { 0, };

	GUILDID sGuild = src->GetGuildNumber();
	if( sGuild != -1 && src->GetGuildToggle() )
	{
		CGuild *mGuild = Guild( sGuild );
		if( mGuild == NULL )
			return;
		strcpy( abbreviation, mGuild->Abbreviation() );
		if( abbreviation[0] == 0 ) // 0 length string
			strcpy( abbreviation, "none" );
		if( mGuild->Type() != GT_STANDARD )
		{
			strcpy( guildtype, GTypeNames[mGuild->Type()].c_str() );
			if( src->GetGuildTitle()[0] != 0 )
				sprintf( title, "[%s, %s] [%s]", src->GetGuildTitle().c_str(), abbreviation, guildtype );
			else
				sprintf( title, "[%s] [%s]", abbreviation, guildtype );
		}
		else
		{
			if( src->GetGuildTitle()[0] != 0 )
				sprintf( title,"[%s, %s]", src->GetGuildTitle().c_str(), abbreviation );
			else
				sprintf( title, "[%s]", abbreviation );
		}
		s->objMessage( title, src );
	}
}

}
