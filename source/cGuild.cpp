#include "uox3.h"
#include "cGuild.h"
#include "ssection.h"
#ifndef va_start
	#include <cstdarg>
	//using namespace std;
#endif

CGuild::CGuild() : name( "" ), charter( "" ), webpage( "" ), stone( INVALIDSERIAL ), master( INVALIDSERIAL ), gType( GT_STANDARD )
{
	abbreviation[0] = 0;
	recruitPtr = recruits.end();
	memberPtr = members.end();
	warPtr = relationList.end();
	allyPtr = relationList.end();
}

CGuild::~CGuild()
{
	recruits.erase( recruits.begin(), recruits.end() );
	members.erase( members.begin(), members.end() );
	relationList.erase( relationList.begin(), relationList.end() );
}

//o---------------------------------------------------------------------------o
//|   Function    -  GuildID FirstWar( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Moves to start of the relation list and returns the next
//|					 relation which is at war
//o---------------------------------------------------------------------------o
GuildID CGuild::FirstWar( void )
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
GuildID CGuild::NextWar( void )
{
	while( warPtr != relationList.end() )
	{
		if( warPtr->second == GR_WAR )
			return warPtr->first;

		warPtr++;
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
		warPtr--;
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
GuildID CGuild::FirstAlly( void )
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
GuildID CGuild::NextAlly( void )
{
	while( allyPtr != relationList.end() )
	{
		if( allyPtr->second == GR_WAR )
			return allyPtr->first;

		allyPtr++;
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
		allyPtr--;
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    -  const char *Name( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the name of the guild
//o---------------------------------------------------------------------------o
const char *CGuild::Name( void ) const
{
	return name.c_str();
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
//|   Function    -  const char *Charter( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the charter of the guild
//o---------------------------------------------------------------------------o
const char *CGuild::Charter( void ) const
{
	return charter.c_str();
}

//o---------------------------------------------------------------------------o
//|   Function    -  const char *Webpage( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the webpage of the guild
//o---------------------------------------------------------------------------o
const char *CGuild::Webpage( void ) const
{
	return webpage.c_str();
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
	recruitPtr = recruits.begin();
	if( FinishedRecruits() )
		return INVALIDSERIAL;
	else
		return (*recruitPtr);
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
	recruitPtr++;
	if( FinishedRecruits() )
		return INVALIDSERIAL;
	else
		return (*recruitPtr);
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
SERIAL CGuild::RecruitNumber( UI32 rNum ) const
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
SERIAL CGuild::MemberNumber( UI32 rNum ) const
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
	memberPtr = members.begin();
	if( FinishedMember() )
		return INVALIDSERIAL;
	else
		return (*memberPtr);
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
	++memberPtr;	// post ++ forces a copy constructor
	if( FinishedMember() )
		return INVALIDSERIAL;
	else
		return (*memberPtr);
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
//|   Function    -  void Name( const char *txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the name of the guild to txt.  If a stone exists, it
//|					 updates the stone's name
//o---------------------------------------------------------------------------o
void CGuild::Name( const char *txt )
{
	name = txt;
	if( Stone() != INVALIDSERIAL )	// we have a guildstone
	{
		CItem *gStone = calcItemObjFromSer( Stone() );
		if( gStone != NULL )
		{
			gStone->SetName( txt );
		}
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
//|   Function    -  void Charter( const char *txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's charter to txt
//o---------------------------------------------------------------------------o
void CGuild::Charter( const char *txt )
{
	charter = txt;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Webpage( const char *txt )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the guild's webpage to txt
//o---------------------------------------------------------------------------o
void CGuild::Webpage( const char *txt )
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
	SERLIST::iterator toFind = recruits.begin();
	while( toFind != recruits.end() )
	{
		if( (*toFind) == newRecruit )
		{
			recruits.erase( toFind );
			return;
		}
		toFind++;
	}
}
void CGuild::RemoveMember( CChar &newMember )
{
	RemoveMember( newMember.GetSerial() );
}
void CGuild::RemoveMember( SERIAL newMember )
{
	SERLIST::iterator toFind = members.begin();
	while( toFind != members.end() )
	{
		if( (*toFind) == newMember )
		{
			members.erase( toFind );
			return;
		}
		toFind++;
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
	SERLIST::const_iterator toFind = recruits.begin();
	while( toFind != recruits.end() )
	{
		if( (*toFind) == toCheck )
			return true;
		toFind++;
	}
	return false;
}
bool CGuild::IsMember( SERIAL toCheck ) const
{
	SERLIST::const_iterator toFind = members.begin();
	while( toFind != members.end() )
	{
		if( (*toFind) == toCheck )
			return true;
		toFind++;
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


GuildRelation CGuild::RelatedToGuild( GuildID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return GR_UNKNOWN;
	else
		return toFind->second;
}
bool CGuild::IsAtWar( GuildID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return false;
	else
		return ( toFind->second == GR_WAR );
}
bool CGuild::IsNeutral( GuildID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return false;
	else
		return ( toFind->second == GR_NEUTRAL );
}
bool CGuild::IsAlly( GuildID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
		return false;
	else
		return ( toFind->second == GR_ALLY );
}

void CGuild::SetGuildRelation( GuildID otherGuild, GuildRelation toSet )
{
	relationList[otherGuild] = toSet;
}

GUILDREL *CGuild::GuildRelationList( void )	// NOTE: This is aimed ONLY at menu stuff
{
	return &relationList;
}

void CGuild::Save( std::ofstream &toSave, GuildID gNum )
{
	toSave << "[GUILD " << gNum << ']' << std::endl << "{" << std::endl;
	toSave << "NAME=" << name.c_str() << std::endl;
	toSave << "ABBREVIATION=" << abbreviation << std::endl;
	toSave << "TYPE=" << GTypeNames[gType].c_str() << std::endl;
	toSave << "CHARTER=" << charter.c_str() << std::endl;
	toSave << "WEBPAGE=" << webpage.c_str() << std::endl;
	toSave << "STONE=" << stone << std::endl;
	toSave << "MASTER=" << master << std::endl;
	SERLIST::iterator counter = recruits.begin();
	while( counter != recruits.end() )
	{
		toSave << "RECRUIT=" << (*counter) << std::endl;
		counter++;
	}
	counter = members.begin();
	while( counter != members.end() )
	{
		toSave << "MEMBER=" << (*counter) << std::endl;
		counter++;
	}
	GUILDREL::iterator relly = relationList.begin();
	while( relly != relationList.end() )
	{
		toSave << GRelationNames[relly->second].c_str() << " " << relly->first <<std::endl;
		relly++;
	}
	toSave << "}" << std::endl << std::endl;
}
void CGuild::Load( ScriptSection *toRead )
{
	const char *tag = NULL;
	const char *data = NULL;
	for( tag = toRead->First(); !toRead->AtEnd(); tag = toRead->Next() )
	{
		data = toRead->GrabData();
		if( tag == NULL )
			continue;
		switch( tag[0] )
		{
		case '{':
		case '/':	break;	// open section, comment, we don't really care ;)
		case 'A':
			if( !strncmp( tag, "ABBREVIATION", 12 ) )
				Abbreviation( data );
			else if( !strncmp( tag, "Ally", 4 ) )
				SetGuildRelation( static_cast<GuildID>(makeNum( data )), GR_ALLY );
			break;
		case 'C':
			if( !strncmp( tag, "CHARTER", 7 ) )
				Charter( data );
			break;
		case 'M':
			if( !strncmp( tag, "MASTER", 6 ) )
				Master( makeNum( data ) );
			else if( !strncmp( tag, "MEMBER", 6 ) )
				NewMember( makeNum( data ) );
			break;
		case 'N':
			if( !strncmp( tag, "NAME", 4 ) )
				Name( data );
			else if( !strncmp( tag, "Neutral", 7 ) )
				SetGuildRelation( static_cast<GuildID>(makeNum( data )), GR_NEUTRAL );
			break;
		case 'R':
			if( !strncmp( tag, "RECRUIT", 7 ) )
				NewRecruit( makeNum( data ) );
		case 'S':
			if( !strncmp( tag, "STONE", 5 ) )
				Stone( makeNum( data ) );
			break;
		case 'T':
			if( !strncmp( tag, "TYPE", 4 ) )
			{
				for( GuildType gt = GT_STANDARD; gt < GT_COUNT; gt = (GuildType)(gt + (GuildType)1) )
				{
					if( !strcmp( data, GTypeNames[gt].c_str() ) )
					{
						Type( gt );
						break;
					}
				}
			}
			break;
		case 'U':
			if( !strncmp( tag, "Unknown", 7 ) )
				SetGuildRelation( static_cast<GuildID>(makeNum( data )), GR_UNKNOWN );
			break;
		case 'W':
			if( !strncmp( tag, "WEBPAGE", 7 ) )
				Webpage( data );
			else if( !strncmp( tag, "War", 3 ) )
				SetGuildRelation( static_cast<GuildID>(makeNum( data )), GR_WAR );
			break;
		}
	}
}

int CGuild::NumMembers( void ) const
{
	return members.size();
}
int CGuild::NumRecruits( void ) const
{
	return recruits.size();
}

void CGuild::CalcMaster( void )
{
	if( members.size() == 0 )
	{
		Master( INVALIDSERIAL );
		return;
	}
	std::vector< int > votes;
	CHARLIST mVoting;
	votes.resize( members.size() );
	mVoting.resize( members.size() );
	for( UI32 ct = 0; ct < votes.size(); ct++ )		// Find pointer list so that we don't keep recalculating it
		mVoting[ct] = calcCharObjFromSer( members[ct] );

	for( UI32 counter = 0; counter < votes.size(); counter++ )
	{
		votes[counter] = 0;	// init the count before adding
		for( UI32 counter2 = 0; counter2 < votes.size(); counter2++ )
		{
			if( mVoting[counter2]->GetGuildFealty() == members[counter] )
				votes[counter]++;
		}
	}
	UI32 maxIndex = 0;
	for( UI32 indexCounter = 1; indexCounter < votes.size(); indexCounter++ )
	{
		if( votes[indexCounter] > votes[maxIndex] )
			maxIndex = indexCounter;
	}
	Master( *(mVoting[maxIndex]) );
}

void CGuild::TellMembers( char *txt, ... )
{
	va_list argptr;
	char msg[512];
	char tmpMsg[512];
	strcpy( msg, "GUILD: " );
	va_start( argptr, txt );
	vsprintf( tmpMsg, txt, argptr );
	va_end( argptr );
	strcat( msg, tmpMsg );
	for( UI32 memberCounter = 0; memberCounter < members.size(); memberCounter++ )
	{
		CSpeechEntry *toAdd = SpeechSys->Add();
		toAdd->Speech( msg );
		toAdd->Font( FNT_NORMAL );
		toAdd->SpokenTo( members[memberCounter] );
		toAdd->Colour( 0x000B );
		toAdd->Type( SYSTEM );
		toAdd->At( cwmWorldState->GetUICurrentTime() );
		toAdd->TargType( SPTRG_INDIVIDUAL );
	}
}

void CGuild::TellMembers( SI32 dictEntry, ... )
{
	char msg[512];
	char tmpMsg[512];

	for( UI32 memberCounter = 0; memberCounter < members.size(); memberCounter++ )
	{
		CChar *targetChar = calcCharObjFromSer( members[memberCounter] );
		cSocket *targetSock = calcSocketObjFromChar( targetChar );
		if( targetSock != NULL )
		{
			const char *txt = Dictionary->GetEntry( dictEntry, targetSock->Language() );

			strcpy( msg, "TOWN: " );
			va_list argptr;
			va_start( argptr, dictEntry );
			vsprintf( tmpMsg, txt, argptr );
			va_end( argptr );
			strcat( msg, tmpMsg );

			CSpeechEntry *toAdd = SpeechSys->Add();
			toAdd->Speech( msg );
			toAdd->Font( FNT_NORMAL );
			toAdd->Speaker( INVALIDSERIAL );
			toAdd->SpokenTo( members[memberCounter] );
			toAdd->Colour( 0x000B );
			toAdd->Type( SYSTEM );
			toAdd->At( cwmWorldState->GetUICurrentTime() );
			toAdd->TargType( SPTRG_INDIVIDUAL );
		}
	}
}

const char *CGuild::TypeName( void )
{
	return GTypeNames[Type()].c_str();
}

CGuildCollection::CGuildCollection()
{
}
UI32 CGuildCollection::NumGuilds( void ) const
{
	return gList.size();
}

GuildID CGuildCollection::MaximumGuild( void )
{
	GuildID maxVal = -1;
	GUILDLIST::iterator pFind = gList.begin();
	while( pFind != gList.end() )
	{
		if( pFind->first > maxVal )
			maxVal = pFind->first;
		pFind++;
	}
	return (GuildID)maxVal + 1;
}
GuildID CGuildCollection::NewGuild( void )
{
	CGuild *toAdd = new CGuild();
	GuildID gAdd = MaximumGuild();
	gList[gAdd] = toAdd;
	return gAdd;
}
CGuild *CGuildCollection::Guild( GuildID num ) const
{
	GUILDLIST::const_iterator pFind = gList.find( num );
	if( pFind == gList.end() )
		return NULL;
	return pFind->second;
}
CGuild *CGuildCollection::operator[]( GuildID num )
{
	return Guild( num );
}
void CGuildCollection::Save( void )
{
	char disp[5] = "|/-\\";
	char filename[MAX_PATH];
	int iCounter = 0;

	Console << "Saving guild data.... ";
	sprintf( filename, "%sguilds.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	std::ofstream toSave( filename );
	GUILDLIST::iterator pMove = gList.begin();
	Console.TurnYellow();
	while( pMove != gList.end() )
	{
		Console << "\b" << (UI08)disp[iCounter];
		(pMove->second)->Save( toSave, pMove->first );
		iCounter = (iCounter + 1)%4;
		pMove++;
	}
	Console.PrintDone();

}
void CGuildCollection::Load( void )
{
	char filename[MAX_PATH];
	sprintf( filename, "%sguilds.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	FILE *npcExists = fopen( filename, "r" );

	if( npcExists == NULL )
	{
		Console.Error( 1, "Failed to open guild file %s for reading", filename );
		return;
	}

	fclose( npcExists );
	Script newScript( filename, NUM_DEFS, false );
	ScriptSection *testSect = NULL;
	GuildID guildNum = 0;
	for( testSect = newScript.FirstEntry(); testSect != NULL; testSect = newScript.NextEntry() )
	{
		const char *text = newScript.EntryName();
		guildNum = static_cast<GuildID>(makeNum( &(text[6])) );
		if( gList[guildNum] != NULL )
			delete gList[guildNum];
		gList[guildNum] = new CGuild();
		gList[guildNum]->Load( testSect );
	}

}

GuildRelation CGuildCollection::Compare( GuildID srcGuild, GuildID trgGuild ) const
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

GuildRelation CGuildCollection::Compare( CChar *src, CChar *trg ) const
{
	if( src == NULL || trg == NULL )
		return GR_UNKNOWN;
	if( src->GetGuildNumber() == -1 || trg->GetGuildNumber() == -1 )
		return GR_UNKNOWN;
	if( src->GetGuildNumber() == trg->GetGuildNumber() )
		return GR_SAME;
	return Compare( src->GetGuildNumber(), trg->GetGuildNumber() );
}

void CGuildCollection::Menu( cSocket *s, SI16 menu, GuildID trgGuild, SERIAL plID )
{
	if( s == NULL )
		return;
	if( trgGuild >= static_cast<SI32>(NumGuilds()) )
		return;
	STRINGLIST one, two;
	char tempString[128];
	one.push_back( "nomove" );
	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( tempString, "resizepic 0 0 %i 400 400", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( tempString );
	sprintf( tempString, "button 360 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( tempString );
	sprintf( tempString, "text 120 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );
	one.push_back( tempString );
	one.push_back( "page 1" );

	SERIAL gMaster = gList[trgGuild]->Master();
	CChar *mChar = s->CurrcharObj();
	CChar *gMstr = calcCharObjFromSer( gMaster );
	UI16 numButtons = 0, numText = 0, numColumns = 1;

	char guildfealty[30], guildt[10], toggle[4];
	strcpy( guildfealty, "yourself" );
	if( mChar->GetGuildFealty() != mChar->GetSerial() && mChar->GetGuildFealty() != INVALIDSERIAL )
	{
		CChar *fChar = calcCharObjFromSer( mChar->GetGuildFealty() );
		if( fChar != NULL )
			strcpy( guildfealty, fChar->GetName() );
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
			strcpy(guildt, "INVALID");
			break;
	}
	strcpy( toggle, "Off" );
	if( mChar->GetGuildToggle() ) 
		strcpy( toggle, "On" );

	std::string gName = gList[trgGuild]->Name();
	UI16 tCtr			= 0;
	SERIAL tChar		= 0;
	GUILDREL::iterator toCheck;
	GUILDREL *ourList;
	s->TempInt( trgGuild );
	UnicodeTypes sLang	= s->Language();
	switch( menu )
	{
	case -1:			break;
	case BasePage:		break;
	case BasePage+1:	numButtons = 10;			// Main menu
		two.push_back( Dictionary->GetEntry( 102, sLang ) );
		two.push_back( Dictionary->GetEntry( 103, sLang ) );
		two.push_back( Dictionary->GetEntry( 104, sLang ) );
		two.push_back( Dictionary->GetEntry( 105, sLang ) );
		sprintf( tempString, Dictionary->GetEntry( 106, sLang ), guildfealty );
		two.push_back( tempString );
		sprintf( tempString, Dictionary->GetEntry( 107, sLang ), toggle );
		two.push_back( tempString );
		two.push_back( Dictionary->GetEntry( 108, sLang ) );
		two.push_back( Dictionary->GetEntry( 109, sLang ) );
		sprintf( tempString, Dictionary->GetEntry( 110, sLang ), gName.c_str() );
		two.push_back( tempString );
		sprintf( tempString, Dictionary->GetEntry( 111, sLang ), gName.c_str() );
		two.push_back( tempString );
		two.push_back( Dictionary->GetEntry( 112, sLang ) );

	    if( mChar->GetSerial() == gMaster || mChar->IsGM() )	// Guildmaster Access?
		{															
			numButtons++;
			sprintf( tempString, Dictionary->GetEntry( 113, sLang ), gMstr->GetGuildTitle() );
			two.push_back( tempString );
		} 
		break;
	case BasePage+2:	numButtons = 16;		// Guildmaster menu
		two.push_back( Dictionary->GetEntry( 114, sLang ) );
		two.push_back( Dictionary->GetEntry( 115, sLang ) );
		two.push_back( Dictionary->GetEntry( 116, sLang ) );
		sprintf( tempString, Dictionary->GetEntry( 117, sLang ), guildt );
		two.push_back( tempString );
		two.push_back( Dictionary->GetEntry( 118, sLang ) );
		two.push_back( Dictionary->GetEntry( 119, sLang ) );
		two.push_back( Dictionary->GetEntry( 120, sLang ) );
		two.push_back( Dictionary->GetEntry( 121, sLang ) );
		two.push_back( Dictionary->GetEntry( 122, sLang ) );
		two.push_back( Dictionary->GetEntry( 123, sLang ) );
		two.push_back( Dictionary->GetEntry( 124, sLang ) );
		two.push_back( Dictionary->GetEntry( 125, sLang ) );
		two.push_back( Dictionary->GetEntry( 126, sLang ) );
		two.push_back( Dictionary->GetEntry( 127, sLang ) );
		two.push_back( Dictionary->GetEntry( 128, sLang ) );
		two.push_back( Dictionary->GetEntry( 129, sLang ) );
		two.push_back( Dictionary->GetEntry( 130, sLang ) );
		break;
	case BasePage+3:	numButtons = 4;			// Guild type
		two.push_back( Dictionary->GetEntry( 131, sLang ) );
		two.push_back( Dictionary->GetEntry( 132, sLang ) );
		two.push_back( Dictionary->GetEntry( 133, sLang ) );
		two.push_back( Dictionary->GetEntry( 134, sLang ) );
		two.push_back( Dictionary->GetEntry( 135, sLang ) );
		break;
	case BasePage+4:	numButtons = 3;			// Set charter
		two.push_back( Dictionary->GetEntry( 136, sLang ) );
		two.push_back( Dictionary->GetEntry( 137, sLang ) );
		two.push_back( Dictionary->GetEntry( 138, sLang ) );
		two.push_back( Dictionary->GetEntry( 139, sLang ) );
		break;
	case BasePage+5:	numButtons = 2;			// View charter
		two.push_back( Dictionary->GetEntry( 140, sLang ) );
		sprintf( tempString, Dictionary->GetEntry( 141, sLang ), gList[trgGuild]->Charter() );
		two.push_back( tempString );
		sprintf( tempString, Dictionary->GetEntry( 142, sLang ), gList[trgGuild]->Webpage() );
		two.push_back( tempString );
		break;
	case BasePage+6:								// List of recruits
		two.push_back( Dictionary->GetEntry( 143, sLang ) );
		tCtr = 0;
		for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
		{
			two.push_back( chars[calcCharFromSer( tChar )].GetName() );
			tCtr++;
		}
		numButtons = tCtr;
		break;
	case BasePage+7:								// List of members
		two.push_back( Dictionary->GetEntry( 144, sLang ) );
		tCtr = 0;
		for( tChar = gList[trgGuild]->FirstMember(); !gList[trgGuild]->FinishedMember(); tChar = gList[trgGuild]->NextMember() )
		{
			two.push_back( chars[calcCharFromSer( tChar )].GetName() );
			tCtr++;
		}
		numButtons = tCtr;	
		break;
	case BasePage+8:								// Member dismiss
		two.push_back( Dictionary->GetEntry( 145, sLang ) );
		tCtr = 0;
		for( tChar = gList[trgGuild]->FirstMember(); !gList[trgGuild]->FinishedMember(); tChar = gList[trgGuild]->NextMember() )
		{
			two.push_back( chars[calcCharFromSer( tChar )].GetName() );
			tCtr++;
		}
		numButtons = tCtr;	break;
	case BasePage+9:								// Dismiss recruit
		two.push_back( Dictionary->GetEntry( 146, sLang ) );
		tCtr = 0;
		for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
		{
			two.push_back( chars[calcCharFromSer( tChar )].GetName() );
			tCtr++;
		}
		numButtons = tCtr;	break;
	case BasePage+10:								// Accept recruit
		two.push_back( Dictionary->GetEntry( 147, sLang ) );
		tCtr = 0;
		for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
		{
			two.push_back( chars[calcCharFromSer( tChar )].GetName() );
			tCtr++;
		}
		numButtons = tCtr;	break;
	case BasePage+11:								// War list
		two.push_back( Dictionary->GetEntry( 148, sLang ) );
		tCtr = 0;
		ourList = gList[trgGuild]->GuildRelationList();
		toCheck = ourList->begin();
		while( toCheck != ourList->end() )
		{
			if( toCheck->second == GR_WAR )
			{
				two.push_back( gList[toCheck->first]->Name() );
				tCtr++;
			}
			toCheck++;
		}
		numButtons = tCtr;	break;
	case BasePage+12:								// Grant title
		numButtons = 0;
		two.push_back( "Unfilled functionality" );
		break;
	case BasePage+13:								// Declare war list
		two.push_back( Dictionary->GetEntry( 149, sLang ) );
		tCtr = 0;
		ourList = gList[trgGuild]->GuildRelationList();
		toCheck = ourList->begin();
		while( toCheck != ourList->end() )
		{
			if( toCheck->second != GR_WAR )
			{
				two.push_back( gList[toCheck->first]->Name() );
				tCtr++;
			}
			toCheck++;
		}
		numButtons = tCtr;	break;
	case BasePage+14:								// Declare peace list
		two.push_back( Dictionary->GetEntry( 150, sLang ) );
		tCtr = 0;
		ourList = gList[trgGuild]->GuildRelationList();
		toCheck = ourList->begin();
		while( toCheck != ourList->end() )
		{
			if( toCheck->second == GR_WAR )
			{
				two.push_back( gList[toCheck->first]->Name() );
				tCtr++;
			}
			toCheck++;
		}
		numButtons = tCtr;	break;
	case BasePage+15:								// List of guilds warring on us
	case BasePage+16:								// Display recruit information
		numButtons = 0;
		two.push_back( "Unfilled functionality" );
		break;
	case BasePage+17:								// Display member information
		tChar = calcCharFromSer( plID );
		if( tChar != INVALIDSERIAL )
		{
			char temp[128];
			sprintf( temp, "%s (", chars[tChar].GetName() );
			if( chars[tChar].IsInnocent() )
				strcat( temp, "Innocent) " );
			else if( chars[tChar].IsMurderer() )
				strcat( temp, "Murderer) " );
			else
				strcat( temp, "Criminal) " );
			two.push_back( temp );
			// To display: Name, str, dex, int, # kills, # deaths, status (criminal, murderer, innocent), top x skills
			two.push_back( "Strength" );
			sprintf( temp, "%i", chars[tChar].GetStrength() );
			two.push_back( temp );

			two.push_back( "Dexterity" );
			sprintf( temp, "%i", chars[tChar].GetDexterity() );
			two.push_back( temp );

			two.push_back( "Intelligence" );
			sprintf( temp, "%i", chars[tChar].GetIntelligence() );
			two.push_back( temp );

			two.push_back( "Kills" );
			sprintf( temp, "%i", chars[tChar].GetKills() );
			two.push_back( temp );

			two.push_back( "Deaths" );
			sprintf( temp, "%i", chars[tChar].GetDeaths() );
			two.push_back( temp );

			numText = 10;
			numColumns = 2;
		}
		else
		{
			numButtons = 0;
			two.push_back( "Unknown player" );
		}
		break;
	case BasePage+18:								// Ally list
		two.push_back( Dictionary->GetEntry( 151, sLang ) );
		tCtr = 0;
		ourList = gList[trgGuild]->GuildRelationList();
		toCheck = ourList->begin();
		while( toCheck != ourList->end() )
		{
			if( toCheck->second == GR_ALLY )
			{
				two.push_back( gList[toCheck->first]->Name() );
				tCtr++;
			}
			toCheck++;
		}
		numButtons = tCtr;	break;
	case BasePage+19:								// Declare Ally list
		two.push_back( Dictionary->GetEntry( 152, sLang ) );
		tCtr = 0;
		ourList = gList[trgGuild]->GuildRelationList();
		toCheck = ourList->begin();
		while( toCheck != ourList->end() )
		{
			if( toCheck->second != GR_ALLY )
			{
				two.push_back( gList[toCheck->first]->Name() );
				tCtr++;
			}
			toCheck++;
		}
		numButtons = tCtr;	break;
	}

	for( int iCtr = 0; iCtr < numButtons; iCtr++ )
	{
		sprintf( tempString, "button 20 %i %i %i 1 0 %i", 30 + 20 * iCtr, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, iCtr + 2 );
		one.push_back( tempString );
		sprintf( tempString, "text 50 %i %i %i", 30 + 20 * iCtr, cwmWorldState->ServerData()->GetLeftTextColour(), iCtr + 1 );
		one.push_back( tempString );
	}
	if( numText != 0 )
	{
		for( int iCtr = 0; iCtr < numText; iCtr++ )
		{
			if( numColumns == 1 )
				sprintf( tempString, "text 50 %i %i %i", 30 + 20 * ( iCtr + numButtons ), cwmWorldState->ServerData()->GetLeftTextColour(), iCtr + numButtons + 1 );
			else
				sprintf( tempString, "text %i %i %i %i", 50 + (iCtr % numColumns) * 100, 30 + 20 * ( ( iCtr / 2 ) + numButtons ), cwmWorldState->ServerData()->GetLeftTextColour(), iCtr + numButtons + 1 );
			one.push_back( tempString );
		}
	}

	SendVecsAsGump( s, one, two, menu, INVALIDSERIAL );
}
void CGuildCollection::GumpInput( cSocket *s )
{
	UI08 type = s->GetByte( 7 );
	UI08 index = s->GetByte( 8 );
	char *text = (char *)&(s->Buffer()[12]);

	if( type != 100 )
		return;
	GuildID trgGuild = (GuildID)s->TempInt();
	CChar *gMaster = calcCharObjFromSer( gList[trgGuild]->Master() );
	switch( index )
	{
	case 1:	gList[trgGuild]->Name( text );			break; // set guild name
	case 2:	gList[trgGuild]->Abbreviation( text );	break; // set guild abbreviation
	case 3:	gMaster->SetGuildTitle( text );			break; // set guildmaster title
	case 4:	gList[trgGuild]->Charter( text );		break;	// new charter
	case 5:	gList[trgGuild]->Webpage( text );		break;	// new webpage
	}
}

void CGuildCollection::ToggleAbbreviation( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	GuildID guildnumber = mChar->GetGuildNumber();
	if( guildnumber == -1 )
		return;

	if( gList[guildnumber]->Type() != GT_STANDARD )						// Check for Order/Chaos
		sysmessage( s, 153 );	// They may not toggle it off!
	else
	{
		mChar->SetGuildToggle( !mChar->GetGuildToggle() );
		if( mChar->GetGuildToggle() )						// If set to On then
			sysmessage( s, 154 );	// Tell player about the change
		else													// Otherwise
			sysmessage( s, 155 );	// And tell him also
	}
}
void CGuildCollection::GumpChoice( cSocket *s )
{
	long realType = s->GetDWord( 7 );
	long button = s->GetDWord( 11 );
	GuildID trgGuild = (GuildID)s->TempInt();
	if( button == 1 )	// hit cancel
		return;
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;
	SERIAL ser = mChar->GetSerial();
	UI16 tCtr = 0;
	GUILDREL::iterator toCheck;
	GUILDREL *ourList;
	int offCounter;

	CHARACTER tChar = INVALIDSERIAL;

	switch( realType )
	{
	case -1:			
	case BasePage:		
	case BasePage+1:								// Main menu
		switch( button )
		{
		case 2:		target( s, 2, 0, 156 );		break;	// recruit into guild
		case 3:		Menu( s, BasePage + 7, trgGuild );	break;
		case 4:		Menu( s, BasePage + 5, trgGuild );	break;
		case 5:		target( s, 2, 1, 157 );		break;	// declare fealty
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
		case 2:		entrygump( s, ser, 100, 1, 30, 158 );	break;	// set guild name
		case 3:		entrygump( s, ser, 100, 2, 3,  159 );	break;	// set guild abbreviation
		case 4:		Menu( s, BasePage + 3, trgGuild );		break;	// set guild type
		case 5:		Menu( s, BasePage + 4, trgGuild );		break;	// set guild charter
		case 6:		Menu( s, BasePage + 8, trgGuild );		break;	// dismiss a member
		case 7:		Menu( s, BasePage + 13, trgGuild );		break;	// declare war from menu
		case 8:		target( s, 2, 2, 160 );			break;	// declare war from target
		case 9:		Menu( s, BasePage + 14, trgGuild );		break;	// declare peace
		case 10:	Menu( s, BasePage + 19, trgGuild );		break;	// declare ally from menu
		case 11:	target( s, 2, 3, 161 );			break;	// declare ally from target
		case 12:	Menu( s, BasePage + 10, trgGuild );		break;	// accept candidate seeking membership
		case 13:	Menu( s, BasePage + 9, trgGuild );		break;	// refuse candidate seeking membership
		case 14:	entrygump( s, ser, 100, 3, 15,  162 );	break;	// set guild master's title
		case 15:	Menu( s, BasePage + 12, trgGuild );		break;	// grant title to another member
		case 16:											break;	// move guildstone
		case 17:	Menu( s, BasePage + 1, trgGuild );		break;	// return to main menu
		}
		break;
	case BasePage+3:								// Guild type
		switch( button )
		{
		case 2:		Menu( s, BasePage + 1, trgGuild );		break;
		case 3:		gList[trgGuild]->Type( GT_STANDARD );	break;
		case 4:		gList[trgGuild]->Type( GT_ORDER );		break;
		case 5:		gList[trgGuild]->Type( GT_CHAOS );		break;
		}
		break;
	case BasePage+4:								// Set charter
		switch( button )
		{
		case 2:		Menu( s, BasePage + 1, trgGuild );		break;
		case 3:		entrygump( s, ser, 100, 4, 50, 163 );	break;
		case 4:		entrygump( s, ser, 100, 5, 50, 164 );	break;
		}
		break;
	case BasePage+5:								// View charter
		switch( button )
		{
		case 2:		Menu( s, BasePage + 1, trgGuild );			break;
		case 3:		weblaunch( s, gList[trgGuild]->Webpage() );	break;
		}
		break;
	case BasePage+6:								// List of recruits
		Menu( s, BasePage + 16, trgGuild, gList[trgGuild]->RecruitNumber( button - 2 ) );	// display recruit number
		break;
	case BasePage+7:								// List of members
		Menu( s, BasePage + 17, trgGuild, gList[trgGuild]->MemberNumber( button - 2 ) );	// display member number
		break;
	case BasePage+8:								// Member dismiss
		tChar = calcCharFromSer( gList[trgGuild]->MemberNumber( button - 2 ) );
		if( tChar != INVALIDSERIAL )
		{
			gList[trgGuild]->RemoveMember( chars[tChar] );
			chars[tChar].SetGuildNumber( -1 );
		}
		break;
	case BasePage+9:								// Dismiss recruit
		gList[trgGuild]->RemoveRecruit( gList[trgGuild]->RecruitNumber( button - 2 ) );
		break;
	case BasePage+10:								// Accept recruit
		tChar = calcCharFromSer( gList[trgGuild]->RecruitNumber( button - 2 ) );
		if( tChar != INVALIDSERIAL )
		{
			gList[trgGuild]->RecruitToMember( chars[tChar] );
			chars[tChar].SetGuildNumber( trgGuild );
		}
		break;
	case BasePage+11:								// War list
	case BasePage+12:								// Grant title
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
					gList[trgGuild]->TellMembers( 165, gList[tCtr]->Name() );
					gList[tCtr]->TellMembers( 166, gList[trgGuild]->Name() );
					break;
				}
				else
					offCounter++;
			}
			tCtr++;
			toCheck++;
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
					gList[trgGuild]->TellMembers( 167, gList[tCtr]->Name() );
					gList[tCtr]->TellMembers( 168, gList[trgGuild]->Name() );
					break;
				}
				else
					offCounter++;
			}
			tCtr++;
			toCheck++;
		}
		break;
	case BasePage+15:								// List of guilds warring on us
		break;
	case BasePage+16:								// Display recruit information
		Menu( s, BasePage + 1, trgGuild );
		break;
	case BasePage+17:								// Display member information
		Menu( s, BasePage + 1, trgGuild );
		break;
	case BasePage+18:								// Ally list
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
					gList[trgGuild]->TellMembers( 169, gList[tCtr]->Name() );
					gList[tCtr]->TellMembers( 170, gList[trgGuild]->Name() );
					break;
				}
				else
					offCounter++;
			}
			tCtr++;
			toCheck++;
		}
		break;
	}
}
void CGuildCollection::Resign( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 guildnumber = mChar->GetGuildNumber();
	if( guildnumber == -1 )
		return;

	CGuild *nGuild = gList[guildnumber];
	if( nGuild == NULL )
		return;

	nGuild->RemoveMember( *(s->CurrcharObj()) );
	sysmessage( s, 171 );
	mChar->SetGuildNumber( -1 );
	if( nGuild->Master() == mChar->GetSerial() && nGuild->NumMembers() != 0 )
		nGuild->CalcMaster();

	if( nGuild->NumMembers() == 0 )
	{
		SERIAL stone = nGuild->Stone();
		if( stone != INVALIDSERIAL )
		{
			CItem *gStone = calcItemObjFromSer( stone );
			if( gStone != NULL )
				Items->DeleItem( gStone );
		}
		Erase( guildnumber );
		sysmessage( s, 172 );
	}
}

void CGuildCollection::Erase( GuildID toErase )
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
	for( int iCounter = 0; iCounter < gErase->NumMembers(); iCounter++ )
	{
		SERIAL iMember = gErase->MemberNumber( iCounter );
		CChar *member = calcCharObjFromSer( iMember );
		if( member != NULL )
			member->SetGuildNumber( -1 );
	}
	for( int iC = 0; iC < gErase->NumRecruits(); iC++ )
	{
		SERIAL iRecruit = gErase->RecruitNumber( iC );
		CChar *recruit = calcCharObjFromSer( iRecruit );
		if( recruit != NULL )
			recruit->SetGuildNumber( -1 );
	}
	delete pFind->second;
	gList.erase( pFind );
}

CGuildCollection::~CGuildCollection()
{
	GUILDLIST::iterator i = gList.begin();
	while( i != gList.end() )
	{
		if( i->second != NULL )
			delete i->second;
		i++;
	}

	gList.erase( gList.begin(), gList.end() );
}


void CGuildCollection::PlaceStone( cSocket *s, CItem *deed )
{
	if( s == NULL || deed == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	if( mChar == NULL )
		return;

	if( deed->GetID() == 0x14F0 )
	{
		if( mChar->GetGuildNumber() != -1 )	// in a guild
		{
			objMessage( s, 173, deed );
			return;
		}
		GuildID gNum = NewGuild();
		CGuild *nGuild = Guild( gNum );
		if( nGuild == NULL )
		{
			objMessage( s, 174, deed );
			Console.Error( 3, "Critical error adding guildstone, memory allocation failed.  Attempted by player %i", mChar->GetSerial() );
			return;
		}
		mChar->SetGuildNumber( gNum );
		s->TempInt( gNum );
		nGuild->NewMember( (*mChar) );
		CItem *stone = Items->SpawnItem( NULL, mChar, 1, Dictionary->GetEntry( 175 ), false, 0x0ED5, 0, false, false );
		if( stone == NULL )
		{
			objMessage( s, 176, deed );
			Console.Error( 3, "Critical error spawning guildstone, no stone made.  Attempted by player %i", mChar->GetSerial() );
			return;
		}
		if( mChar->GetID() == 0x0191 || mChar->GetID() == 0x0193 )
			mChar->SetGuildTitle( "Guildmistress" );
		else
			mChar->SetGuildTitle( "Guildmaster" );
		stone->SetLocation( mChar );
		nGuild->Webpage( DEFAULTWEBPAGE );
		nGuild->Charter( DEFAULTCHARTER );
		stone->SetType( 202 );
		stone->SetPriv( 0 );
		stone->SetMore( gNum );
		RefreshItem( stone );
		Items->DeleItem( deed );
		nGuild->Master( (*mChar) );
		nGuild->Stone( (*stone) );
		entrygump( s, mChar->GetSerial(), 100, 1, 30, 158 );
	}
	else
		sysmessage( s, 177 );
}

bool CGuildCollection::ResultInCriminal( GuildID srcGuild, GuildID trgGuild ) const
{
	GuildRelation gRel = Compare( srcGuild, trgGuild );
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
	if( src == NULL || trg == NULL )
		return false;
	return ResultInCriminal( src->GetGuildNumber(), trg->GetGuildNumber() );
}

void CGuildCollection::DisplayTitle( cSocket *s, CChar *src ) const
{
	if( src == NULL || s == NULL )
		return;
	char title[150];
	char abbreviation[5];
	char guildtype[10] = { 0, };

	GuildID sGuild = src->GetGuildNumber();
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
			if( src->GetGuildTitle()[0] != 0  ) 
				sprintf( title, "[%s, %s] [%s]", src->GetGuildTitle(), abbreviation, guildtype );
			else 
				sprintf( title, "[%s] [%s]", abbreviation, guildtype );
		}
		else
		{
			if( src->GetGuildTitle()[0] != 0 )
				sprintf( title,"[%s, %s]", src->GetGuildTitle(), abbreviation );
			else 
				sprintf( title, "[%s]", abbreviation );
		}
		objMessage( s, title, src );
	}
}
