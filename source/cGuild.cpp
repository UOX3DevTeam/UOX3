#include "uox3.h"
#include "cGuild.h"
#include "speech.h"
#include "ssection.h"
#include "scriptc.h"
#include "classes.h"
#include "CPacketSend.h"
#include "Dictionary.h"
#include "CJSEngine.h"
#include "StringUtility.hpp"
#include "osunique.hpp"
#include <algorithm>
#ifndef va_start
#include <cstdarg>
#endif
#include <numeric>

using namespace std::string_literals;

CGuildCollection *GuildSys;

#define DEFAULTCHARTER "UOX3 Guildstone"
#define DEFAULTWEBPAGE "http://www.uox3.org/"

CGuild::CGuild() : name( "" ), gType( GT_STANDARD ), charter( "" ), webpage( "" ), stone( INVALIDSERIAL ), master( INVALIDSERIAL )
{
	abbreviation[0] = 0;
	recruits.resize( 0 );
	members.resize( 0 );
    invites.resize( 0 );
    veterans.resize( 0 );
    officers.resize( 0 );
	recruitPtr		= recruits.end();
	memberPtr		= members.end();
    invitePtr		= invites.end();
    veteranPtr		= veterans.end();
    officerPtr		= officers.end();
	warPtr			= relationList.end();
	allyPtr			= relationList.end();
}

CGuild::~CGuild()
{
	JSEngine->ReleaseObject( IUE_GUILD, this );
	recruits.clear();
	members.clear();
    invites.clear();
    veterans.clear();
    officers.clear();
	relationList.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::FirstWar()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Moves to start of the relation list and returns the next
//|					relation which is at war
//o------------------------------------------------------------------------------------------------o
GUILDID CGuild::FirstWar( void )
{
	warPtr = relationList.begin();
	return NextWar();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::NextWar()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Moves through relation list, returning guild ID of next
//|					guild at war, or -1 if no war
//o------------------------------------------------------------------------------------------------o
GUILDID CGuild::NextWar( void )
{
	while( warPtr != relationList.end() )
	{
		if( warPtr->second == GR_WAR )
		{
			return warPtr->first;
		}

		++warPtr;
	}
	return -1;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FinishedWar()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if at end of war list, or false if not
//|					Must iterate through war list if not at end of relation list
//|					Moves back one if it does find something it's at war with
//|					to ensure pointer integrity
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedWar( void )
{
	if( warPtr == relationList.end() )
		return true;
	if( NextWar() == -1 )
		return true;
	else if( warPtr != relationList.begin() )	// we move back if we found it, so NextWar will return a valid warring guild
	{
		--warPtr;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FirstAlly()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Moves to start of the relation list and returns the next
//|					relation which is at ally
//o------------------------------------------------------------------------------------------------o
GUILDID CGuild::FirstAlly( void )
{
	allyPtr = relationList.begin();
	return NextAlly();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::NextAlly()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Moves through relation list, returning guild ID of next
//|					guild as ally, or -1 if no ally
//o------------------------------------------------------------------------------------------------o
GUILDID CGuild::NextAlly( void )
{
	while( allyPtr != relationList.end() )
	{
		if( allyPtr->second == GR_ALLY )
		{
			return allyPtr->first;
		}

		++allyPtr;
	}
	return -1;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FinishedAlly()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if at end of ally list, or false if not
//|					Must iterate through ally list if not at end of relation list
//|					Moves back one if it does find something it's an ally with
//|					to ensure pointer integrity
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedAlly( void )
{
	if( allyPtr == relationList.end() )
		return true;
	if( NextAlly() == -1 )
		return true;
	else if( allyPtr != relationList.begin() )	// we move back if we found it, so NextWar will return a valid warring guild
	{
		--allyPtr;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Name()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the name of the guild.
//|					Setting also updates name of guildstone, if it exists
//o------------------------------------------------------------------------------------------------o
const std::string CGuild::Name( void ) const
{
	return name;
}
void CGuild::Name( std::string txt )
{
	name = txt;
	if( Stone() != INVALIDSERIAL )	// we have a guildstone
	{
		CItem *gStone = CalcItemObjFromSer( Stone() );
		if( ValidateObject( gStone ))
		{
			gStone->SetName( txt );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Abbreviation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the abbreviation of the guild
//o------------------------------------------------------------------------------------------------o
auto CGuild::Abbreviation() const -> const std::string&
{
	return abbreviation;
}
auto CGuild::Abbreviation( const std::string &value ) -> void
{
	abbreviation = value;
	if( value.size() > 4 )
	{
		abbreviation = value.substr( 0, 4 );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Type()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the guild's type
//|					 GT_STANDARD = 0,
//|					 GT_ORDER,
//|					 GT_CHAOS,
//|					 GT_UNKNOWN,
//|					 GT_COUNT
//o------------------------------------------------------------------------------------------------o
GuildType CGuild::Type( void ) const
{
	return gType;
}
void CGuild::Type( GuildType nType )
{
	gType = nType;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Charter()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the charter of the guild
//o------------------------------------------------------------------------------------------------o
const std::string CGuild::Charter( void ) const
{
	return charter;
}
void CGuild::Charter( const std::string &txt )
{
	charter = txt;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Webpage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the webpage of the guild
//o------------------------------------------------------------------------------------------------o
const std::string CGuild::Webpage( void ) const
{
	return webpage;
}
void CGuild::Webpage( const std::string &txt )
{
	webpage = txt;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Stone()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the serial of the guild stone
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::Stone( void ) const
{
	return stone;
}
void CGuild::Stone( SERIAL newStone )
{
	stone = newStone;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Master()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the serial of the guild master (if any)
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::Master( void ) const
{
	return master;
}
void CGuild::Master( SERIAL newMaster )
{
	master = newMaster;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FirstRecruit()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the serial of the first recruit in the recruit list
//|					If no recruits, returns INVALIDSERIAL
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::FirstRecruit( void )
{
	SERIAL retVal	= INVALIDSERIAL;
	recruitPtr		= recruits.begin();
	if( !FinishedRecruits() )
	{
		retVal = ( *recruitPtr );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::NextRecruit()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the serial of the next recruit in the recruit list
//|					If there are no more, it returns INVALIDSERIAL
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::NextRecruit()
{
	SERIAL retVal = INVALIDSERIAL;
	if( !FinishedRecruits() )
	{
		++recruitPtr;
		if( !FinishedRecruits() )
		{
			retVal = (*recruitPtr);
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FinishedRecruits()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if there are no more recruits left
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedRecruits()
{
	return ( recruitPtr == recruits.end() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::RecruitNumber()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the serial of the recruit in slot rNum
//|					If rNum is invalid, it returns INVALIDSERIAL
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::RecruitNumber( size_t rNum ) const
{
	if( rNum >= recruits.size() )
	{
		return INVALIDSERIAL;
	}
	else
	{
		return recruits[rNum];
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::MemberNumber()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the serial of the member in slot rNum
//|					If rNum is invalid, it returns INVALIDSERIAL
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::MemberNumber( size_t rNum ) const
{
	if( rNum >= members.size() )
	{
		return INVALIDSERIAL;
	}
	else
	{
		return members[rNum];
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FirstMember()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the serial of the first member of the guild
//|					If no members, returns INVALIDSERIAL
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::FirstMember( void )
{
	SERIAL retVal	= INVALIDSERIAL;
	memberPtr		= members.begin();
	if( !FinishedMember() )
	{
		retVal = ( *memberPtr );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::NextMember()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the serial of the next member of the guild, if any
//|					If none, it returns INVALIDSERIAL
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::NextMember( void )
{
	SERIAL retVal = INVALIDSERIAL;
	if( !FinishedMember() )
	{
		++memberPtr;	// post ++ forces a copy constructor
		if( !FinishedMember() )
		{
			retVal = ( *memberPtr );
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::FinishedMember()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if there are no more members left
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedMember( void )
{
	return ( memberPtr == members.end() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Stone()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the guild's stone serial to the CItem newStone
//o------------------------------------------------------------------------------------------------o
void CGuild::Stone( CItem &newStone )
{
	stone = newStone.GetSerial();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CGuild::Master()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the guild's master serial to CChar newMaster's serial
//o------------------------------------------------------------------------------------------------o
void CGuild::Master( CChar &newMaster )
{
	master = newMaster.GetSerial();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::NewRecruit()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add new recruit to guild
//o------------------------------------------------------------------------------------------------o
void CGuild::NewRecruit( CChar &newRecruit )
{
	NewRecruit( newRecruit.GetSerial() );
}
void CGuild::NewRecruit( SERIAL newRecruit )
{
	if( IsMember( newRecruit ))
	{
		RemoveMember( newRecruit );
	}
	recruits.push_back( newRecruit );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::NewMember()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add new member to guild
//o------------------------------------------------------------------------------------------------o
void CGuild::NewMember( CChar &newMember )
{
	NewMember( newMember.GetSerial() );
}
void CGuild::NewMember( SERIAL newMember )
{
	if( IsRecruit( newMember ))
	{
		RemoveRecruit( newMember );
	}
	members.push_back( newMember );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::RemoveRecruit()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove recruit from guild
//o------------------------------------------------------------------------------------------------o
void CGuild::RemoveRecruit( CChar &newRecruit )
{
	RemoveRecruit( newRecruit.GetSerial() );
}
void CGuild::RemoveRecruit( SERIAL newRecruit )
{
	auto iter = std::find_if( recruits.begin(), recruits.end(), [newRecruit]( SERIAL &entry )
	{
		return entry == newRecruit;
	});
	if( iter != recruits.end() )
	{
		recruits.erase( iter );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::RemoveMember()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove member from guild
//o------------------------------------------------------------------------------------------------o
void CGuild::RemoveMember( CChar &newMember )
{
	RemoveMember( newMember.GetSerial() );
}
void CGuild::RemoveMember( SERIAL newMember )
{
	auto iter = std::find_if( members.begin(), members.end(), [newMember]( SERIAL &entry )
	{
		return entry == newMember;
	});
	if( iter != members.end() )
	{
		members.erase( iter );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::RecruitToMember()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Change guild recruit to member
//o------------------------------------------------------------------------------------------------o
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::IsRecruit()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character can be found in list of guild recruits
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsRecruit( CChar &toCheck ) const
{
	return IsRecruit( toCheck.GetSerial() );
}

//============================================================================================
auto CGuild::IsRecruit( SERIAL toCheck ) const -> bool
{
	auto rValue = false;
	auto iter = std::find_if( recruits.begin(), recruits.end(), [toCheck]( const SERIAL &entry )
	{
		return toCheck == entry;
	});
	if( iter != recruits.end() )
	{
		rValue = true;
	}
	return rValue;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::IsMember()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character can be found in list of guild members
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsMember( CChar &toCheck ) const
{
	return IsMember( toCheck.GetSerial() );
}
//============================================================================================
auto CGuild::IsMember( SERIAL toCheck ) const -> bool
{
	auto rValue = false;
	auto iter = std::find_if( members.begin(), members.end(), [toCheck]( const SERIAL &entry )
	{
		return toCheck == entry;
	});
	if( iter != members.end() )
	{
		rValue = true;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	 CGuild::RelatedToGuild()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine other guild's relation (Alliance/War) to this guild
//o------------------------------------------------------------------------------------------------o
GUILDRELATION CGuild::RelatedToGuild( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
	{
		return GR_UNKNOWN;
	}
	else
	{
		return toFind->second;
	}
}
bool CGuild::IsAtWar( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
	{
		return false;
	}
	else
	{
		return ( toFind->second == GR_WAR );
	}
}
bool CGuild::IsNeutral( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
	{
		return false;
	}
	else
	{
		return ( toFind->second == GR_NEUTRAL );
	}
}
bool CGuild::IsAlly( GUILDID otherGuild ) const
{
	GUILDREL::const_iterator toFind = relationList.find( otherGuild );
	if( toFind == relationList.end() )
	{
		return false;
	}
	else
	{
		return ( toFind->second == GR_ALLY );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	 CGuild::IsAtPeace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine if this guild is in a state of peace, i.e. NOT at war with anyone
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsAtPeace() const
{
	for( auto &relation : relationList )
	{
		if( relation.second == GR_WAR )
			return false;
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::SetGuildRelation()
//|					CGuild::GuildRelationList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets guild's relation to another guild
//|	Notes		-	NOTE: This is aimed ONLY at menu stuff
//o------------------------------------------------------------------------------------------------o
void CGuild::SetGuildRelation( GUILDID otherGuild, GUILDRELATION toSet )
{
	relationList[otherGuild] = toSet;
}
GUILDREL *CGuild::GuildRelationList( void )
{
	return &relationList;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::AddInvite( CChar& )
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Add a pending invite for the given character (by serial)
//|                Skips if already member/recruit/invited
//o------------------------------------------------------------------------------------------------o
void CGuild::AddInvite( CChar& c )
{
	AddInvite( c.GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::AddInvite( SERIAL )
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Add a pending invite for the given serial
//|                Skips if already member/recruit/invited
//o------------------------------------------------------------------------------------------------o
void CGuild::AddInvite( SERIAL s )
{
	if( IsMember( s ) || IsRecruit( s ) || IsInvited( s ) )
		return;
	invites.push_back( s );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::RemoveInvite( CChar& )
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Remove a pending invite for the given character (by serial)
//o------------------------------------------------------------------------------------------------o
void CGuild::RemoveInvite( CChar& c )
{
	RemoveInvite( c.GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::RemoveInvite( SERIAL )
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Remove a pending invite for the given serial, if present
//o------------------------------------------------------------------------------------------------o
void CGuild::RemoveInvite( SERIAL s )
{
	auto it = std::find( invites.begin(), invites.end(), s );
	if( it != invites.end() )
		invites.erase( it );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::IsInvited( CChar& ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Check if the character has a pending invite (by serial)
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsInvited( CChar& c ) const
{
	return IsInvited( c.GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::IsInvited( SERIAL ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Check if the serial has a pending invite
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsInvited( SERIAL s ) const
{
	return std::find( invites.begin(), invites.end(), s ) != invites.end();
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::NumInvites() const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Return number of pending invites
//o------------------------------------------------------------------------------------------------o
size_t CGuild::NumInvites() const
{
	return invites.size();
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::InviteNumber( size_t ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Return invite serial at index, or INVALIDSERIAL if out of range
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::InviteNumber( size_t i ) const
{
	return ( i < invites.size() ) ? invites[i] : INVALIDSERIAL;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::FirstInvite()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Position iterator at first invite and return its serial,
//|                or INVALIDSERIAL if list is empty
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::FirstInvite()
{
	invitePtr = invites.begin();
	return FinishedInvites() ? INVALIDSERIAL : *invitePtr;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::NextInvite()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Advance to next invite and return its serial,
//|                or INVALIDSERIAL if at end
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::NextInvite()
{
	if( FinishedInvites() )
		return INVALIDSERIAL;
	++invitePtr;
	return FinishedInvites() ? INVALIDSERIAL : *invitePtr;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::FinishedInvites()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Return true if invite iterator is at end
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedInvites()
{
	return invitePtr == invites.end();
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::NewVeteran()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Adds a new Veteran to the guild
//| Notes       - Removes character from Recruit, Member, or Officer lists if present
//o------------------------------------------------------------------------------------------------o
void CGuild::NewVeteran( CChar& c )
{
	 NewVeteran( c.GetSerial() );
}
void CGuild::NewVeteran( SERIAL s )
{
	RemoveRecruit( s);
	RemoveMember( s );
	RemoveOfficer( s );
	veterans.push_back( s );
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::RemoveVeteran()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Removes a Veteran from the guild
//o------------------------------------------------------------------------------------------------o
void CGuild::RemoveVeteran( CChar& c )
{
	RemoveVeteran( c.GetSerial() );
}
void CGuild::RemoveVeteran( SERIAL s )
{
	auto it = std::find( veterans.begin(), veterans.end(), s );
	if( it != veterans.end() )
		veterans.erase( it );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::VeteranNumber( size_t ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Return serial of Veteran at index, or INVALIDSERIAL if out of range
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::VeteranNumber( size_t i ) const
{
	return ( i < veterans.size() ) ? veterans[i] : INVALIDSERIAL;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::IsVeteran( CChar& ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Check if character is a Veteran of this guild
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsVeteran( CChar& c ) const
{
	return IsVeteran( c.GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::IsVeteran()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Checks if specified character is a Veteran of the guild
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsVeteran( SERIAL s ) const
{
	return std::find( veterans.begin(), veterans.end(), s ) != veterans.end();
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::FirstVeteran()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns serial of first Veteran in guild (or INVALIDSERIAL if none)
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::FirstVeteran()
{
	veteranPtr = veterans.begin();
	return FinishedVeterans() ? INVALIDSERIAL : *veteranPtr;
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::NextVeteran()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns serial of next Veteran in list (or INVALIDSERIAL if finished)
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::NextVeteran()
{
	if( FinishedVeterans() )
		return INVALIDSERIAL;
	++veteranPtr;
	return FinishedVeterans() ? INVALIDSERIAL : *veteranPtr;
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::FinishedVeterans()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns true if there are no more Veterans left to iterate
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedVeterans()
{
	return veteranPtr == veterans.end(); 
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::NumVeterans()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns number of Veterans in the guild
//o------------------------------------------------------------------------------------------------o
size_t CGuild::NumVeterans() const 
{
	return veterans.size(); 
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::NewOfficer()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Adds a new Officer to the guild
//| Notes       - Removes character from Recruit, Member, or Veteran lists if present
//o------------------------------------------------------------------------------------------------o
void CGuild::NewOfficer(CChar& c)
{
	NewOfficer( c.GetSerial() );
}
void CGuild::NewOfficer( SERIAL s )
{
	RemoveRecruit( s );
	RemoveMember( s );
	RemoveVeteran( s );
	officers.push_back( s );
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::RemoveOfficer()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Removes an Officer from the guild
//o------------------------------------------------------------------------------------------------o
void CGuild::RemoveOfficer( CChar& c ) 
{
	RemoveOfficer( c.GetSerial() );
}
void CGuild::RemoveOfficer (SERIAL s )
{
	auto it = std::find( officers.begin(), officers.end(), s );
	if( it != officers.end())
		officers.erase( it );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::OfficerNumber( size_t ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Return serial of Officer at index, or INVALIDSERIAL if out of range
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::OfficerNumber( size_t i ) const
{
	return ( i < officers.size() ) ? officers[i] : INVALIDSERIAL;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::IsOfficer( CChar& ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Check if character is an Officer of this guild
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsOfficer( CChar& c ) const
{
	return IsOfficer( c.GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::IsOfficer()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Checks if specified character is an Officer of the guild
//o------------------------------------------------------------------------------------------------o
bool CGuild::IsOfficer(SERIAL s) const
{
	return std::find( officers.begin(), officers.end(), s ) != officers.end();
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::FirstOfficer()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns serial of first Officer in guild (or INVALIDSERIAL if none)
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::FirstOfficer()
{
	officerPtr = officers.begin();
	return FinishedOfficers() ? INVALIDSERIAL : *officerPtr;
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::NextOfficer()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns serial of next Officer in list (or INVALIDSERIAL if finished)
//o------------------------------------------------------------------------------------------------o
SERIAL CGuild::NextOfficer()
{
	if( FinishedOfficers() )
		return INVALIDSERIAL;
	++officerPtr;
	return FinishedOfficers() ? INVALIDSERIAL : *officerPtr;
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::FinishedOfficers()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns true if there are no more Officers left to iterate
//o------------------------------------------------------------------------------------------------o
bool CGuild::FinishedOfficers()
{
	return officerPtr == officers.end(); 
}

//o------------------------------------------------------------------------------------------------o
//| Function    - CGuild::NumOfficers()
//o------------------------------------------------------------------------------------------------o
//| Purpose     - Returns number of Officers in the guild
//o------------------------------------------------------------------------------------------------o
size_t CGuild::NumOfficers() const 
{
	return officers.size(); 
}

// local ASCII case-insensitive equality (avoid locale pitfalls)
static bool iequals_ascii( const std::string& a, const std::string& b )
{
    if( a.size() != b.size() ) return false;
    for( size_t i = 0; i < a.size(); ++i )
    {
        unsigned char ca = static_cast<unsigned char>(a[i]);
        unsigned char cb = static_cast<unsigned char>(b[i]);
        if( std::tolower(ca) != std::tolower(cb) ) return false;
    }
    return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::RebuildRankOrder_()
//| Prototype   -  void RebuildRankOrder_()
//| Purpose     -  Rebuild derived promotion order after ranks change; keeps IDs stable.
//o------------------------------------------------------------------------------------------------o
void CGuild::RebuildRankOrder_()
{
    orderByPrio.resize( ranks.size() );
    std::iota( orderByPrio.begin(), orderByPrio.end(), size_t(0) );

    std::stable_sort( orderByPrio.begin(), orderByPrio.end(),
        [&]( size_t ida, size_t idb )
        {
            return ranks[ida].prio < ranks[idb].prio;
        } );

    idToOrderIndex.assign( ranks.size(), 0 );
    for( size_t i = 0; i < orderByPrio.size(); ++i )
        idToOrderIndex[ orderByPrio[i] ] = i;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::AddRank()
//| Prototype   -  size_t AddRank( const std::string& name, SI32 prio, UI32 flags = 0 )
//| Purpose     -  Add a new rank definition to this guild, or update an existing one.
//| Notes       -  Rank IDs are stable (index in ranks vector). Promotion order is separate.
//o------------------------------------------------------------------------------------------------o
size_t CGuild::AddRank( const std::string& name, SI32 prio, UI32 flags )
{
    for( size_t i = 0; i < ranks.size(); ++i )
    {
        if( iequals_ascii( ranks[i].name, name ) )
        {
            ranks[i].prio  = prio;
            ranks[i].flags = flags;
            RebuildRankOrder_();
            return i;
        }
    }
    ranks.push_back( { name, prio, flags } );
    RebuildRankOrder_();
    return ranks.size() - 1;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::RemoveRankByName()
//| Prototype   -  bool RemoveRankByName( const std::string& name )
//| Purpose     -  Remove a rank; fails if any member currently holds it.
//| Notes       -  Keeps rankId space stable via tombstoning (name -> "(deleted)", prio=INT_MAX).
//o------------------------------------------------------------------------------------------------o
bool CGuild::RemoveRankByName( const std::string& name )
{
    SI32 id = FindRankIdByName( name );
    if( id < 0 ) return false;
    size_t rid = static_cast<size_t>( id );

    // deny remove if in use
    for( const auto& kv : rankOf )
        if( kv.second == rid )
            return false;

    ranks[rid].name  = "(deleted)";
    ranks[rid].prio  = std::numeric_limits<SI32>::max();
    ranks[rid].flags = 0;
    RebuildRankOrder_();
    return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::RenameRank()
//| Prototype   -  bool RenameRank( const std::string& oldName, const std::string& newName )
//| Purpose     -  Change name only; stable ID, prio and flags unaffected.
//o------------------------------------------------------------------------------------------------o
bool CGuild::RenameRank( const std::string& oldName, const std::string& newName )
{
    SI32 id = FindRankIdByName( oldName );
    if( id < 0 ) return false;
    ranks[ static_cast<size_t>(id) ].name = newName;
    return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::FindRankIdByName()
//| Prototype   -  SI32 FindRankIdByName( const std::string& name ) const
//| Purpose     -  Lookup rankId by name (case-insensitive); -1 if missing.
//o------------------------------------------------------------------------------------------------o
SI32 CGuild::FindRankIdByName( const std::string& name ) const
{
    for( size_t i = 0; i < ranks.size(); ++i )
        if( iequals_ascii( ranks[i].name, name ) )
            return static_cast<SI32>( i );
    return -1;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::GetRankId()
//| Prototype   -  SI32 GetRankId( SERIAL s ) const
//o------------------------------------------------------------------------------------------------o
//| Purpose     -  Returns the current rank id for a member; -1 if none.
//o------------------------------------------------------------------------------------------------o
SI32 CGuild::GetRankId(SERIAL s) const
{
    auto it = rankOf.find(s);
    if (it == rankOf.end())
        return -1;
    size_t id = it->second;
    return (id < ranks.size()) ? static_cast<SI32>(id) : -1;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::NumRanks() / GetRankDef()
//| Prototype   -  size_t NumRanks() const
//|               const RankDef* GetRankDef( size_t rankId ) const
//| Purpose     -  Introspection utilities for rank registry.
//o------------------------------------------------------------------------------------------------o
size_t CGuild::NumRanks() const
{
    return ranks.size();
}
const CGuild::RankDef* CGuild::GetRankDef( size_t rankId ) const
{
    if( rankId >= ranks.size() ) return nullptr;
    return &ranks[rankId];
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::SetRank() / HasRank()
//| Prototype   -  bool SetRank( CChar& c, const std::string& rankName )
//|               bool SetRank( SERIAL s, const std::string& rankName )
//|               bool HasRank( CChar& c, const std::string& rankName ) const
//|               bool HasRank( SERIAL s, const std::string& rankName ) const
//| Purpose     -  Assign/check a character�s rank.
//o------------------------------------------------------------------------------------------------o
bool CGuild::SetRank( CChar& c, const std::string& rankName )
{
    return SetRank( c.GetSerial(), rankName );
}
bool CGuild::SetRank( SERIAL s, const std::string& rankName )
{
    SI32 id = FindRankIdByName( rankName );
    if( id < 0 ) return false;
    rankOf[s] = static_cast<size_t>( id );
    return true;
}
bool CGuild::HasRank( CChar& c, const std::string& rankName ) const
{
    return HasRank( c.GetSerial(), rankName );
}
bool CGuild::HasRank( SERIAL s, const std::string& rankName ) const
{
    SI32 id = FindRankIdByName( rankName );
    if( id < 0 ) return false;
    auto it = rankOf.find( s );
    return ( it != rankOf.end() ) && ( it->second == static_cast<size_t>( id ) );
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::GetRankName()
//| Prototype   -  const std::string GetRankName( CChar& c ) const
//|               const std::string GetRankName( SERIAL s ) const
//| Purpose     -  Get current rank name; empty if none.
//o------------------------------------------------------------------------------------------------o
const std::string CGuild::GetRankName( CChar& c ) const
{
    return GetRankName( c.GetSerial() );
}
const std::string CGuild::GetRankName( SERIAL s ) const
{
    auto it = rankOf.find( s );
    if( it == rankOf.end() ) return "";
    size_t id = it->second;
    return ( id < ranks.size() ) ? ranks[id].name : "";
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::Promote() / Demote()
//| Prototype   -  bool Promote( CChar& c )
//|               bool Demote( CChar& c )
//| Purpose     -  Move to next higher/lower rank by priority.
//o------------------------------------------------------------------------------------------------o
bool CGuild::Promote( CChar& c )
{
    if( ranks.empty() ) return false;
    auto it = rankOf.find( c.GetSerial() );
    if( it == rankOf.end() ) return false;

    size_t curId = it->second;
    if( curId >= idToOrderIndex.size() ) return false;

    size_t pos = idToOrderIndex[curId];
    if( pos + 1 >= orderByPrio.size() ) return false; // already top

    size_t nextId = orderByPrio[pos + 1];
    it->second = nextId;
    return true;
}
bool CGuild::Demote( CChar& c )
{
    if( ranks.empty() ) return false;
    auto it = rankOf.find( c.GetSerial() );
    if( it == rankOf.end() ) return false;

    size_t curId = it->second;
    if( curId >= idToOrderIndex.size() ) return false;

    size_t pos = idToOrderIndex[curId];
    if( pos == 0 ) return false; // already bottom

    size_t prevId = orderByPrio[pos - 1];
    it->second = prevId;
    return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::CountInRank() / ListRank()
//| Prototype   -  size_t CountInRank( const std::string& rankName ) const
//|               void   ListRank( const std::string& rankName, std::vector<SERIAL>& out ) const
//| Purpose     -  Query members in a specific rank.
//o------------------------------------------------------------------------------------------------o
size_t CGuild::CountInRank( const std::string& rankName ) const
{
    SI32 id = FindRankIdByName( rankName );
    if( id < 0 ) return 0;
    size_t rid = static_cast<size_t>( id );
    size_t count = 0;
    for( const auto& kv : rankOf )
        if( kv.second == rid ) ++count;
    return count;
}
void CGuild::ListRank( const std::string& rankName, std::vector<SERIAL>& out ) const
{
    out.clear();
    SI32 id = FindRankIdByName( rankName );
    if( id < 0 ) return;
    size_t rid = static_cast<size_t>( id );
    for( const auto& kv : rankOf )
        if( kv.second == rid ) out.push_back( kv.first );
}

SI32 CGuild::GetRankPrioById(size_t id) const
{
    if (id >= ranks.size()) return std::numeric_limits<SI32>::max();
    return ranks[id].prio;
}

std::string CGuild::GetRankNameById(size_t id) const
{
    if (id >= ranks.size()) return std::string();
    return ranks[id].name;
}

bool CGuild::RemoveRankById(size_t id)
{
    if (id >= ranks.size()) return false;

    // deny remove if in use
    for (const auto& kv : rankOf)
        if (kv.second == id)
            return false;

    ranks[id].name  = "(deleted)";
    ranks[id].prio  = std::numeric_limits<SI32>::max();
    ranks[id].flags = 0;
    RebuildRankOrder_();
    return true;
}

bool CGuild::SetRankById(SERIAL s, size_t id)
{
    if (id >= ranks.size()) return false;
    rankOf[s] = id;
    return true;
}


//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::Save()
//| Purpose     -  Save guild data (supports dynamic ranks; writes legacy lines for compat).
//o------------------------------------------------------------------------------------------------o
void CGuild::Save( std::ostream& toSave, GUILDID gNum )
{
    toSave << "[GUILD " << gNum << "]\n{\n";
    toSave << "NAME=" << name << '\n';
    toSave << "ABBREVIATION=" << abbreviation << '\n';
    toSave << "TYPE=" << GTypeNames[gType] << '\n';
    toSave << "CHARTER=" << charter << '\n';
    for( auto s : invites ) toSave << "INVITE=" << s << '\n';
    toSave << "WEBPAGE=" << webpage << '\n';
    toSave << "STONE=" << stone << '\n';
    toSave << "MASTER=" << master << '\n';

    // Dynamic ranks
    for( const auto& r : ranks )
        toSave << "RANKDEF=" << r.name << ',' << r.prio << ',' << r.flags << '\n';

    for( const auto& kv : rankOf )
    {
        size_t rid = kv.second;
        if( rid < ranks.size() )
            toSave << "RANKMEM=" << ranks[rid].name << ',' << kv.first << '\n';
    }

    // Optional: legacy lines for backward compatibility
    auto writeLegacy = [&]( const char* legacyName, const char* key )
    {
        SI32 id = FindRankIdByName( legacyName );
        if( id < 0 ) return;
        size_t rid = static_cast<size_t>( id );
        for( const auto& kv : rankOf )
            if( kv.second == rid )
                toSave << key << '=' << kv.first << '\n';
    };
    writeLegacy( "Recruit", "RECRUIT" );
    writeLegacy( "Member",  "MEMBER" );
    writeLegacy( "Veteran", "VETERAN" );
    writeLegacy( "Officer", "OFFICER" );

    for( auto it = relationList.begin(); it != relationList.end(); ++it )
        toSave << GRelationNames[it->second] << ' ' << it->first << '\n';

    toSave << "}\n\n";
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::Load()
//| Purpose     -  Load guild data (accepts dynamic and legacy formats).
//o------------------------------------------------------------------------------------------------o
void CGuild::Load( CScriptSection* toRead )
{
    // clear transient maps; keep ranks unless you want file to dictate them
    // ranks.clear(); // uncomment if you want file to fully define ranks
    invites.clear();
    rankOf.clear();

    bool sawRankDef = false;

    auto setRankLegacy = [&]( const char* rankName, const std::string& data )
    {
        SERIAL s = static_cast<UI32>( std::stoul( data, nullptr, 0 ) );
        AddRank( rankName, /*prio*/ 0 ); // no-op if exists
        SetRank( s, rankName );
    };

    for( const auto& sec : toRead->collection() )
    {
        std::string tag  = sec->tag;
        std::string data = sec->data;
        if( tag.empty() ) continue;

        std::string UTag = oldstrutil::upper( tag );
        switch( UTag[0] )
        {
            case '{': case '/': break;

            case 'A':
                if( UTag == "ABBREVIATION" ) Abbreviation( data.c_str() );
                else if( UTag == "ALLY" )   SetGuildRelation( static_cast<SI16>( std::stoi( data, nullptr, 0 )), GR_ALLY );
                break;

            case 'C':
                if( UTag == "CHARTER" ) Charter( data );
                break;

            case 'I':
                if( UTag == "INVITE" ) AddInvite( static_cast<UI32>( std::stoul( data, nullptr, 0 ) ) );
                break;

            case 'M':
                if( UTag == "MASTER" ) Master( static_cast<UI32>( std::stoul( data, nullptr, 0 ) ) );
                else if( UTag == "MEMBER" ) setRankLegacy( "Member", data );
                break;
            case 'N':
                if( UTag == "NAME" ) Name( data );
                else if( UTag == "NEUTRAL" ) SetGuildRelation( static_cast<SI16>( std::stoi( data, nullptr, 0 ) ), GR_NEUTRAL );
                break;

            case 'O':
                if( UTag == "OFFICER" ) setRankLegacy( "Officer", data );
                break;

            case 'R':
                if( UTag == "RECRUIT" ) setRankLegacy( "Recruit", data );
                else if( UTag == "RANKDEF" )
                {
                    // name,prio[,flags]
                    auto p1 = data.find(',');
                    if( p1 == std::string::npos ) { AddRank( data, (SI32)ranks.size() ); sawRankDef = true; break; }
                    auto p2 = data.find(',', p1 + 1);
                    auto name  = data.substr( 0, p1 );
                    auto prio  = std::stoi( data.substr( p1 + 1, (p2 == std::string::npos ? data.size() : p2) - (p1 + 1) ) );
                    auto flags = (p2 == std::string::npos) ? 0u : (UI32)std::stoul( data.substr( p2 + 1 ) );
                    AddRank( name, prio, flags );
                    sawRankDef = true;
                }
                else if( UTag == "RANKMEM" )
                {
                    // name,serial
                    auto p = data.find(',');
                    if( p != std::string::npos )
                    {
                        auto name = data.substr( 0, p );
                        SERIAL s  = static_cast<UI32>( std::stoul( data.substr( p + 1 ) ) );
                        AddRank( name, (SI32)ranks.size() );
                        SetRank( s, name );
                    }
                }
                break;

            case 'S':
                if( UTag == "STONE" ) Stone( static_cast<UI32>( std::stoul( data, nullptr, 0 ) ) );
                break;

            case 'T':
                if( UTag == "TYPE" )
                {
                    for( GuildType gt = GT_STANDARD; gt < GT_COUNT; gt = static_cast<GuildType>( gt + static_cast<GuildType>( 1 ) ) )
                        if( data == GTypeNames[gt] ) { Type( gt ); break; }
                }
                break;

            case 'U':
                if( UTag == "UNKNOWN" ) SetGuildRelation( static_cast<SI16>( std::stoi( data, nullptr, 0 ) ), GR_UNKNOWN );
                break;

            case 'V':
                if( UTag == "VETERAN" ) setRankLegacy( "Veteran", data );
                break;

            case 'W':
                if( UTag == "WEBPAGE" ) Webpage( data );
                else if( UTag == "WAR" ) SetGuildRelation( static_cast<SI16>( std::stoi( data, nullptr, 0 ) ), GR_WAR );
                break;
        }
    }

    // If file provided no rank defs, ensure defaults for legacy mapping.
    if( !sawRankDef )
    {
        AddRank( "Recruit", 0 );
        AddRank( "Member",  1 );
        AddRank( "Veteran", 2 );
        AddRank( "Officer", 3 );
    }
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::NumMembers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of guild members
//o------------------------------------------------------------------------------------------------o
size_t CGuild::NumMembers( void ) const
{
	return members.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::NumRecruits()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of guild recruits
//o------------------------------------------------------------------------------------------------o
size_t CGuild::NumRecruits( void ) const
{
	return recruits.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::CalcMaster()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate next guild master based on vote count
//o------------------------------------------------------------------------------------------------o
void CGuild::CalcMaster( void )
{
	if( members.empty() )
	{
		Master( INVALIDSERIAL );
		return;
	}
	std::vector<SI32> votes;
	votes.resize( members.size() );
	UI32 maxIndex = 0;

	for( size_t counter = 0; counter < votes.size(); ++counter )
	{
		votes[counter] = 0;	// init the count before adding
		for( size_t counter2 = 0; counter2 < votes.size(); ++counter2 )
		{
			CChar *myChar = CalcCharObjFromSer( members[counter2] );
			if( ValidateObject( myChar ) && myChar->GetGuildFealty() == members[counter] )
			{
				++votes[counter];
			}
		}
		if( votes[counter] > votes[maxIndex] )
		{
			maxIndex = static_cast<UI32>( counter );
		}
	}

	Master( members[maxIndex] );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::TellMembers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Tell all guild members a message
//o------------------------------------------------------------------------------------------------o
void CGuild::TellMembers( SI32 dictEntry, ... )
{
	for( auto &member: members )
	{
		CChar *targetChar	= CalcCharObjFromSer( member );
		CSocket *targetSock	= targetChar->GetSocket();
		if( targetSock != nullptr )
		{
			std::string txt = "GUILD: ";
			txt += Dictionary->GetEntry( dictEntry, targetSock->Language() );

			va_list argptr;
			va_start( argptr, dictEntry );

			if( cwmWorldState->ServerData()->UseUnicodeMessages() )
			{
				std::string tempStr = oldstrutil::format( 512, txt, argptr );

				CPUnicodeMessage unicodeMessage;
				unicodeMessage.Message( tempStr );
				unicodeMessage.Font( FNT_NORMAL );
				unicodeMessage.Colour( 0x000B );
				unicodeMessage.Type( SYSTEM );
				unicodeMessage.Language( "ENG" );
				unicodeMessage.Name( "System" );
				unicodeMessage.ID( INVALIDID );
				unicodeMessage.Serial( INVALIDSERIAL );

				targetSock->Send( &unicodeMessage );
			}
			else
			{
				CSpeechEntry& toAdd = SpeechSys->Add();
				toAdd.Speech( oldstrutil::format( 512, txt, argptr ));
				toAdd.Font( FNT_NORMAL );
				toAdd.Speaker( INVALIDSERIAL );
				toAdd.SpokenTo( member );
				toAdd.Colour( 0x000B );
				toAdd.Type( SYSTEM );
				toAdd.At( cwmWorldState->GetUICurrentTime() );
				toAdd.TargType( SPTRG_INDIVIDUAL );
			}
			va_end( argptr );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::SetGuildFaction()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the guild's faction - neutral, order or chaos
//o------------------------------------------------------------------------------------------------o
void CGuild::SetGuildFaction( GuildType newFaction )
{
	Type( newFaction );

	if( newFaction != GT_STANDARD )
	{
		for( auto &member : members )
		{
			CChar *memberChar	= CalcCharObjFromSer( member );
			if( !memberChar->GetGuildToggle() )
			{
				memberChar->SetGuildToggle( true );
				CSocket *memberSock	= memberChar->GetSocket();
				if( memberSock != nullptr )
				{
					memberSock->SysMessage( 154 ); // Let him know about the change
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuild::TypeName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets guild type as a string
//o------------------------------------------------------------------------------------------------o
const std::string CGuild::TypeName( void )
{
	return GTypeNames[Type()];
}


//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::NumGuilds()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of guilds on the server
//o------------------------------------------------------------------------------------------------o
size_t CGuildCollection::NumGuilds( void ) const
{
	return gList.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::MaximumGuild()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the next available guild ID
//o------------------------------------------------------------------------------------------------o
GUILDID CGuildCollection::MaximumGuild( void )
{
	GUILDID maxVal = -1;
	GUILDLIST::const_iterator pFind = gList.begin();
	while( pFind != gList.end() )
	{
		if( pFind->first > maxVal )
		{
			maxVal = pFind->first;
		}
		++pFind;
	}
	return static_cast<GUILDID>( maxVal + 1 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::NewGuild()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a new guild
//o------------------------------------------------------------------------------------------------o
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
		return nullptr;

	return pFind->second;
}
CGuild *CGuildCollection::operator[]( GUILDID num )
{
	return Guild( num );
}

GUILDID CGuildCollection::FindGuildId( const CGuild* targetGuild )
{
    if( targetGuild == nullptr )
    {
        return -1;
    }

    // Use std::find_if to search the map
    auto it = std::find_if( gList.begin(), gList.end(),
                           [targetGuild]( const auto& pair ) {
                               return pair.second == targetGuild;
                           });

    // Check if find_if found an element
    if( it != gList.end() )
    {
        return it->first; // Return the key (GUILDID)
    }
    else
    {
        return -1;
    }
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Save()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves guild to worldfile
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::Save( void )
{
	Console << "Saving guild data.... ";
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "guilds.wsc";
	std::ofstream toSave( filename.c_str() );
	GUILDLIST::const_iterator pMove = gList.begin();
	while( pMove != gList.end() )
	{
		( pMove->second )->Save( toSave, pMove->first );
		++pMove;
	}
	Console.PrintDone();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load guilds from worldfile
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::Load( void )
{
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "guilds.wsc";
	if( FileExists( filename ))
	{
		Script newScript( filename, NUM_DEFS, false );
		CScriptSection *testSect	= nullptr;
		GUILDID guildNum		= 0;
		for( testSect = newScript.FirstEntry(); testSect != nullptr; testSect = newScript.NextEntry() )
		{
			std::string text = newScript.EntryName();
			text = text.substr( 6 );
			guildNum = static_cast<SI16>( std::stoi( text, nullptr, 0 ));
			if( gList[guildNum] != nullptr )
			{
				delete gList[guildNum];
			}
			gList[guildNum] = new CGuild();
			gList[guildNum]->Load( testSect );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Compare()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compare the guild relation between two guilds
//o------------------------------------------------------------------------------------------------o
GUILDRELATION CGuildCollection::Compare( GUILDID srcGuild, GUILDID trgGuild ) const
{
	if( srcGuild == -1 || trgGuild == -1 )
		return GR_UNKNOWN;

	if( srcGuild == trgGuild )
		return GR_SAME;

	CGuild *mGuild = Guild( srcGuild );
	if( mGuild == nullptr )
		return GR_UNKNOWN;

	return mGuild->RelatedToGuild( trgGuild );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Compare()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compare the guild relation between two characters
//o------------------------------------------------------------------------------------------------o
GUILDRELATION CGuildCollection::Compare( CChar *src, CChar *trg ) const
{
	if( src == nullptr || trg == nullptr )
		return GR_UNKNOWN;

	auto srcGuild = src->GetGuildNumber();
	auto trgGuild = trg->GetGuildNumber();
	if( srcGuild == -1 || trgGuild == -1 )
		return GR_UNKNOWN;

	if( srcGuild == trgGuild )
		return GR_SAME;

	return Compare( srcGuild, trgGuild );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Menu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send guild menu to player
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::Menu( CSocket *s, SI16 menu, GUILDID trgGuild, SERIAL plId )
{
	if( s == nullptr )
		return;
	if( trgGuild >= static_cast<SI32>( NumGuilds() ))
		return;

	CPSendGumpMenu toSend;
	toSend.GumpId( menu );
	toSend.UserId( INVALIDSERIAL );

	toSend.addCommand( "page 0" );
	toSend.addCommand( oldstrutil::format( "resizepic 0 0 %u 600 400", cwmWorldState->ServerData()->BackgroundPic() ));
	toSend.addCommand( oldstrutil::format( "button 560 10 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 )); //OKAY
	toSend.addCommand( oldstrutil::format( "text 120 10 %u 0", cwmWorldState->ServerData()->TitleColour() ));
	toSend.addCommand( "page 1" );

	SERIAL gMaster	= gList[trgGuild]->Master();
	CChar *mChar	= s->CurrcharObj();
	CChar *gMstr	= CalcCharObjFromSer( gMaster );
	UI16 numButtons = 0, numText = 0, numColumns = 1;
	
	auto guildFealty = "yourself"s;
	if( mChar->GetGuildFealty() != mChar->GetSerial() && mChar->GetGuildFealty() != INVALIDSERIAL )
	{
		CChar *fChar = CalcCharObjFromSer( mChar->GetGuildFealty() );
		if( ValidateObject( fChar ))
		{
			guildFealty = fChar->GetNameRequest( mChar, NRS_GUILD );
		}
	}
	else
	{
		mChar->SetGuildFealty( mChar->GetSerial() );
	}
	auto guildt = "INVALID"s;
	switch( gList[trgGuild]->Type() )
	{
		case 0:
			guildt = " Standard"s;
			break;
		case 1:
			guildt = "n Order"s;
			break;
		case 2:
			guildt = " Chaos"s;
			break;
		default:
			break;
	}
	auto toggle = "Off"s;
	
	if( mChar->GetGuildToggle() )
	{
		toggle = "On"s;
	}

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
		case BasePage + 1:	numButtons = 10;			// Main menu
			toSend.addText( Dictionary->GetEntry( 102, sLang ));
			toSend.addText( Dictionary->GetEntry( 103, sLang ));
			toSend.addText( Dictionary->GetEntry( 104, sLang ));
			toSend.addText( Dictionary->GetEntry( 105, sLang ));
			toSend.addText( oldstrutil::format( Dictionary->GetEntry( 106, sLang ), guildFealty.c_str() ));
			toSend.addText( oldstrutil::format( Dictionary->GetEntry( 107, sLang ), toggle.c_str() ));
			toSend.addText( Dictionary->GetEntry( 108, sLang ));
			toSend.addText( Dictionary->GetEntry( 109, sLang ));
			toSend.addText( oldstrutil::format( Dictionary->GetEntry( 110, sLang ), gName.c_str() ));
			toSend.addText( oldstrutil::format( Dictionary->GetEntry( 111, sLang ), gName.c_str() ));
			toSend.addText( Dictionary->GetEntry( 112, sLang ));

			if( mChar->GetSerial() == gMaster || mChar->IsGM() )	// Guildmaster Access?
			{
				++numButtons;
				if( ValidateObject( gMstr))
				{
					toSend.addText( oldstrutil::format( Dictionary->GetEntry( 113, sLang ), gMstr->GetGuildTitle().c_str() ));
				}
				else
				{
					toSend.addText( oldstrutil::format( Dictionary->GetEntry( 17622, sLang ), gMstr->GetGuildTitle().c_str() )); // This guild has no Guild Master.
				}
			}
			break;
		case BasePage + 2:	numButtons = 16;		// Guildmaster menu
			toSend.addText( Dictionary->GetEntry( 114, sLang ));
			toSend.addText( Dictionary->GetEntry( 115, sLang ));
			toSend.addText( Dictionary->GetEntry( 116, sLang ));
			toSend.addText(oldstrutil::format( Dictionary->GetEntry( 117, sLang ), guildt.c_str() ));
			for( tCounter = 118; tCounter <= 130; ++tCounter )
			{
				toSend.addText( Dictionary->GetEntry( tCounter, sLang ));
			}
			break;
		case BasePage + 3:	numButtons = 4;			// Guild type
			toSend.addText( Dictionary->GetEntry( 131, sLang ));
			toSend.addText( Dictionary->GetEntry( 133, sLang ));
			toSend.addText( Dictionary->GetEntry( 134, sLang ));
			toSend.addText( Dictionary->GetEntry( 135, sLang ));
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			break;
		case BasePage + 4:	numButtons = 3;			// Set charter
			toSend.addText( Dictionary->GetEntry( 136, sLang ));
			toSend.addText( Dictionary->GetEntry( 138, sLang ));
			toSend.addText( Dictionary->GetEntry( 139, sLang ));
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			break;
		case BasePage + 5:	numButtons = 3;			// View charter
			toSend.addText( Dictionary->GetEntry( 140, sLang ));
			toSend.addText( gList[trgGuild]->Charter() );
			toSend.addText( oldstrutil::format( Dictionary->GetEntry( 142, sLang ), gList[trgGuild]->Webpage().c_str() ));
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			break;
		case BasePage + 6:								// List of recruits
			toSend.addText( Dictionary->GetEntry( 143, sLang ));
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
			{
				toSend.addText( CalcCharObjFromSer( tChar )->GetNameRequest( mChar, NRS_GUILD ));
				++tCtr;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;
			break;
		case BasePage + 7:								// List of members
			toSend.addText( Dictionary->GetEntry( 144, sLang ));
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstMember(); !gList[trgGuild]->FinishedMember(); tChar = gList[trgGuild]->NextMember() )
			{
				toSend.addText( CalcCharObjFromSer( tChar )->GetNameRequest( mChar, NRS_GUILD ));
				++tCtr;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;
			break;
		case BasePage + 8:								// Member dismiss
			toSend.addText( Dictionary->GetEntry( 145, sLang ));
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstMember(); !gList[trgGuild]->FinishedMember(); tChar = gList[trgGuild]->NextMember() )
			{
				toSend.addText( CalcCharObjFromSer( tChar )->GetNameRequest( mChar, NRS_GUILD ));
				++tCtr;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 9:								// Dismiss recruit
			toSend.addText( Dictionary->GetEntry( 146, sLang ));
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
			{
				toSend.addText( CalcCharObjFromSer( tChar )->GetNameRequest( mChar, NRS_GUILD ));
				++tCtr;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 10:								// Accept recruit
			toSend.addText( Dictionary->GetEntry( 147, sLang ));
			tCtr = 0;
			for( tChar = gList[trgGuild]->FirstRecruit(); !gList[trgGuild]->FinishedRecruits(); tChar = gList[trgGuild]->NextRecruit() )
			{
				toSend.addText( CalcCharObjFromSer( tChar )->GetNameRequest( mChar, NRS_GUILD ));
				++tCtr;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 11:								// War list
			toSend.addText( Dictionary->GetEntry( 148, sLang ));
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_WAR )
				{
					toSend.addText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 13:								// Declare war list
			toSend.addText( Dictionary->GetEntry( 149, sLang ));
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_WAR )
				{
					toSend.addText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 14:								// Declare peace list
			toSend.addText( Dictionary->GetEntry( 150, sLang ));
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_WAR )
				{
					toSend.addText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 15:								// List of guilds warring on us
		case BasePage + 16:								// Display recruit information
			numButtons = 1;
			toSend.addText( "Unfilled functionality" );
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			break;
		case BasePage + 17:								// Display member information
			CChar *kChar;
			kChar = CalcCharObjFromSer( plId );
			if( ValidateObject( kChar ))
			{
				auto temp = kChar->GetNameRequest( mChar, NRS_GUILD );
				if( kChar->IsInnocent() )
				{
					temp += std::string(" Innocent" );
				}
				else if( kChar->IsMurderer() )
				{
					temp += std::string(" Murderer" );
				}
				else
				{
					temp += std::string(" Criminal" );
				}
				toSend.addText( temp );
				// To display: Name, str, dex, int, # kills, # deaths, status (criminal, murderer, innocent), top x skills
				toSend.addText( "Strength" );
				toSend.addText( oldstrutil::format( "%i", kChar->GetStrength() ));
				toSend.addText( "Dexterity" );
				toSend.addText( oldstrutil::format( "%i", kChar->GetDexterity() ));
				toSend.addText( "Intelligence" );
				toSend.addText( oldstrutil::format( "%i", kChar->GetIntelligence() ));
				toSend.addText( "Kills" );
				toSend.addText( oldstrutil::format( "%i", kChar->GetKills() ));
				toSend.addText( "Deaths" );
				toSend.addText( oldstrutil::format( "%u", kChar->GetDeaths() ));
				numText = 10;
				numColumns = 2;
				numButtons = 1;
			}
			else
			{
				numButtons = 1;
				toSend.addText( "Unknown player" );
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			break;
		case BasePage + 18:								// Ally list
			toSend.addText( Dictionary->GetEntry( 151, sLang ));
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_ALLY )
				{
					toSend.addText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
		case BasePage + 19:								// Declare Ally list
			toSend.addText( Dictionary->GetEntry( 152, sLang ));
			tCtr	= 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_ALLY )
				{
					toSend.addText( gList[toCheck->first]->Name() );
					++tCtr;
				}
				++toCheck;
			}
			toSend.addText( Dictionary->GetEntry( 130, sLang ));
			numButtons = ++tCtr;	break;
	}

	for( UI16 iCtr = 0; iCtr < numButtons; ++iCtr )
	{
		toSend.addCommand( oldstrutil::format( "button 20 %i %i %i 1 0 %i", 30 + 20 * iCtr, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, iCtr + 2 ));
		toSend.addCommand( oldstrutil::format( "text 50 %i %i %i", 30 + 20 * iCtr, cwmWorldState->ServerData()->LeftTextColour(), iCtr + 1 ));
	}
	if( numText != 0 )
	{
		for( UI16 iCtr = 0; iCtr < numText; ++iCtr )
		{
			if( numColumns == 1 )
			{
				toSend.addCommand( oldstrutil::format( "text 50 %i %i %i", 30 + 20 * ( iCtr + numButtons ), cwmWorldState->ServerData()->LeftTextColour(), iCtr + numButtons + 1 ));
			}
			else
			{
				toSend.addCommand( oldstrutil::format( "text %i %i %i %i", 50 + ( iCtr % numColumns ) * 100, 30 + 20 * (( iCtr / 2 ) + numButtons ), cwmWorldState->ServerData()->LeftTextColour(), iCtr + numButtons + 1 ));
			}
		}
	}
	toSend.Finalize();
	s->Send( &toSend );
}
void CGuildCollection::GumpInput( CPIGumpInput *gi )
{
	UI08 type		= gi->Type();
	UI08 index		= gi->Index();
	std::string text = gi->Reply();
	CSocket *s		= gi->GetSocket();

	if( text.empty() )
		return;

	if( type != 100 )
		return;

	GUILDID trgGuild	= static_cast<GUILDID>( s->TempInt() );
	CChar *gMaster		= CalcCharObjFromSer( gList[trgGuild]->Master() );
	switch( index )
	{
		case 1:	gList[trgGuild]->Name( text );			break; // set guild name
		case 2:	gList[trgGuild]->Abbreviation( text.c_str() );	break; // set guild abbreviation
		case 3:	gMaster->SetGuildTitle( text );			break; // set guildmaster title
		case 4:	gList[trgGuild]->Charter( text );		break;	// new charter
		case 5:	gList[trgGuild]->Webpage( text );		break;	// new webpage
		case 6: // set guild member title
			auto gMember = CalcCharObjFromSer( s->TempInt2() );
			if( ValidateObject( gMember ))
			{
				gMember->SetGuildTitle( text );
				auto gSock = gMember->GetSocket();
				if( gSock != nullptr )
				{
					gSock->SysMessage( 1686, text.c_str() ); // You have been granted a guild title of '%s'!
				}
				gMember->Dirty( UT_UPDATE );
			}
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::ToggleAbbreviation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggles guild abbreviation on/off - except for order/chaos, which must show
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::ToggleAbbreviation( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	GUILDID guildNumber = mChar->GetGuildNumber();
	if( guildNumber == -1 )
		return;

	if( gList[guildNumber]->Type() != GT_STANDARD )						// Check for Order/Chaos
	{
		s->SysMessage( 153 );	// They may not toggle it off!
	}
	else
	{
		mChar->SetGuildToggle( !mChar->GetGuildToggle() );
		if( mChar->GetGuildToggle() )						// If set to On then
		{
			s->SysMessage( 154 );	// Tell player about the change
		}
		else													// Otherwise
		{
			s->SysMessage( 155 );	// And tell him also
		}
		mChar->Dirty( UT_UPDATE );
	}
}

void CGuildCollection::TransportGuildStone( CSocket *s, GUILDID guildId )
{
	CChar *mChar = s->CurrcharObj();
	if( guildId == -1 )
		return;

	if( gList[guildId]->Stone() == INVALIDSERIAL )
		return;
	
	// Make sure this is the guild master
	if( gList[guildId]->Master() == mChar->GetSerial() )
	{
		CItem *gTransportStone = Items->CreateItem( s, mChar, 0x1869, 1, 0, OT_ITEM, true );
		if( ValidateObject( gTransportStone ))
		{
			gTransportStone->SetTempVar( CITV_MORE, guildId );
			gTransportStone->SetNewbie( true );
			gTransportStone->SetWeight( 1, true );
			gTransportStone->SetType( IT_GUILDSTONE );
			gTransportStone->SetName( oldstrutil::format( Dictionary->GetEntry( 101 ), gList[guildId]->Name().c_str() ));
			CItem *gStone = CalcItemObjFromSer( gList[guildId]->Stone() );
			gStone->Delete();

			// A guildstone transporter object has been placed in your backpack. Use it to move the guildstone to a new location.
			s->SysMessage( 1972 );
		}
	}
}

void TextEntryGump( CSocket *s, SERIAL ser, UI08 type, UI08 index, SI16 maxlength, SI32 dictEntry );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::GumpChoice()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button presses in guild menus
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::GumpChoice( CSocket *s )
{
	UI32 realType		= s->GetDWord( 7 );
	UI32 button			= s->GetDWord( 11 );
	GUILDID trgGuild	= static_cast<GUILDID>( s->TempInt() );
	if( button == 1 )	// hit cancel
		return;

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	SERIAL ser = mChar->GetSerial();
	UI16 tCtr = 0;
	GUILDREL::iterator toCheck;
	GUILDREL *ourList;
	size_t offCounter;

	CChar *tChar = nullptr;

	switch( realType )
	{
		case INVALIDSERIAL:
		case BasePage:
		case BasePage + 1:								// Main menu
			switch( button )
			{
				case 2:		s->SendTargetCursor( 2, 0, 0, 156 );				break;	// recruit into guild
				case 3:		Menu( s, BasePage + 7, trgGuild );	break;
				case 4:		Menu( s, BasePage + 5, trgGuild );	break;
				case 5:		s->SendTargetCursor( 2, 1, 0, 157 );				break;	// declare fealty
				case 6:		ToggleAbbreviation( s );			break;	// toggle abbreviation
				case 7:		Resign( s );						break;	//	Resign from guild
				case 8:		Menu( s, BasePage + 6, trgGuild );	break;	// View candidates
				case 9:		Menu( s, BasePage + 11, trgGuild );	break;	// View warring guilds
				case 10:	Menu( s, BasePage + 15, trgGuild );	break;	// View guilds that declared war on us
				case 11:	Menu( s, BasePage + 18, trgGuild );	break;	// View list of allied guilds
				case 12:	Menu( s, BasePage + 2, trgGuild );	break;	// Access guildmaster functions
			}
			break;
		case BasePage + 2:								// Guildmaster menu
			switch( button )
			{
				case 2:		TextEntryGump( s, ser, 100, 1, 30, 158 );	break;	// set guild name
				case 3:		TextEntryGump( s, ser, 100, 2, 4,  159 );	break;	// set guild abbreviation
				case 4:		Menu( s, BasePage + 3, trgGuild );		break;	// set guild type
				case 5:		Menu( s, BasePage + 4, trgGuild );		break;	// set guild charter
				case 6:		Menu( s, BasePage + 8, trgGuild );		break;	// dismiss a member
				case 7:		Menu( s, BasePage + 13, trgGuild );		break;	// declare war from menu
				case 8:		s->SendTargetCursor( 2, 2, 0, 160 );	break;	// declare war from target
				case 9:		Menu( s, BasePage + 14, trgGuild );		break;	// declare peace
				case 10:	Menu( s, BasePage + 19, trgGuild );		break;	// declare ally from menu
				case 11:	s->SendTargetCursor( 2, 3, 0, 161 );	break;	// declare ally from target
				case 12:	Menu( s, BasePage + 10, trgGuild );		break;	// accept candidate seeking membership
				case 13:	Menu( s, BasePage + 9, trgGuild );		break;	// refuse candidate seeking membership
				case 14:	TextEntryGump( s, ser, 100, 3, 15,  162 );	break;	// set guild master's title
				case 15:	s->SendTargetCursor( 2, 4, 0, 1685 );	break; // Select guild member to grant title to
#if defined( _MSC_VER )
#pragma note( "Move guildstone functionality goes here" )
#endif
				case 16:	TransportGuildStone( s, trgGuild );		break;	// move guildstone
				case 17:	Menu( s, BasePage + 1, trgGuild );		break;	// return to main menu
			}
			break;
		case BasePage + 3:								// Guild type
			switch( button )
			{
				case 2:		gList[trgGuild]->SetGuildFaction( GT_STANDARD );	break;
				case 3:		gList[trgGuild]->SetGuildFaction( GT_ORDER );		break;
				case 4:		gList[trgGuild]->SetGuildFaction( GT_CHAOS );		break;
				case 5:		Menu( s, BasePage + 2, trgGuild );		break;
			}
			break;
		case BasePage + 4:								// Set charter
			switch( button )
			{
				case 2:		TextEntryGump( s, ser, 100, 4, 50, 163 );	break;
				case 3:		TextEntryGump( s, ser, 100, 5, 50, 164 );	break;
				case 4:		Menu( s, BasePage + 2, trgGuild );		break;
			}
			break;
		case BasePage + 5:								// View charter
			switch( button )
			{
				case 3:		s->OpenURL( gList[trgGuild]->Webpage() );	break;
				case 2:
				case 4:		Menu( s, BasePage + 1, trgGuild );			break;
			}
			break;
		case BasePage + 6:								// List of recruits
			if( gList[trgGuild]->NumRecruits() >= ( button - 2 ))
			{
				Menu( s, BasePage + 1, trgGuild );
			}
			else
			{
				Menu( s, BasePage + 16, trgGuild, gList[trgGuild]->RecruitNumber( button - 2 ));	// display recruit number
			}
			break;
		case BasePage + 7:								// List of members
			if( gList[trgGuild]->NumMembers() >= ( button - 2 ))
			{
				Menu( s, BasePage + 1, trgGuild );
			}
			else
			{
				Menu( s, BasePage + 17, trgGuild, gList[trgGuild]->MemberNumber( button - 2 ));	// display member number
			}
			break;
		case BasePage + 8:								// Member dismiss
			if( gList[trgGuild]->NumMembers() <= ( button - 2 ))
			{
				Menu( s, BasePage + 2, trgGuild );
			}
			else
			{
				tChar = CalcCharObjFromSer( gList[trgGuild]->MemberNumber( button - 2 ));
				if( ValidateObject( tChar ))
				{
					if( tChar == mChar )
					{
						s->SysMessage( 1691 ); // You cannot dismiss yourself from the guild!
						break;
					}

					gList[trgGuild]->RemoveMember(( *tChar ));
					tChar->SetGuildNumber( -1 );
					s->SysMessage( 1689, tChar->GetName().c_str() ); // You have dismissed %s from your guild!
					auto tSock = tChar->GetSocket();
					if( tSock != nullptr )
					{
						tSock->SysMessage( 1690 ); // You have been dismissed from your guild!
					}
				}
			}
			break;
		case BasePage + 9:								// Dismiss recruit
			if( gList[trgGuild]->NumRecruits() <= ( button - 2 ))
			{
				Menu( s, BasePage + 2, trgGuild );
			}
			else
			{
				gList[trgGuild]->RemoveRecruit( gList[trgGuild]->RecruitNumber( button - 2 ));
			}
			break;
		case BasePage + 10:								// Accept recruit
			if( gList[trgGuild]->NumRecruits() <= ( button - 2 ))
			{
				Menu( s, BasePage + 2, trgGuild );
			}
			else
			{
				tChar = CalcCharObjFromSer( gList[trgGuild]->RecruitNumber( button - 2 ));
				if( ValidateObject( tChar ))
				{
					gList[trgGuild]->RecruitToMember(( *tChar ));
					tChar->SetGuildNumber( trgGuild );
					if( gList[trgGuild]->Type() != GT_STANDARD )
					{
						tChar->SetGuildToggle( true );
					}
				}
			}
			break;
		case BasePage + 11:								// War list
		case BasePage + 12:								// Grant title
			Menu( s, BasePage + 2, trgGuild );
			break;
		case BasePage + 13:								// Declare war list
			offCounter = tCtr = 0;
			ourList = gList[trgGuild]->GuildRelationList();
			if( ourList->size() <= ( button - 2 ))
			{
				Menu( s, BasePage + 2, trgGuild );
			}

			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_WAR )
				{
					if( offCounter == ( button - 2 ))
					{
						gList[trgGuild]->SetGuildRelation( tCtr, GR_ALLY );
						gList[trgGuild]->TellMembers( 165, gList[tCtr]->Name().c_str() );
						gList[tCtr]->TellMembers( 166, gList[trgGuild]->Name().c_str() );
						break;
					}
					else
					{
						++offCounter;
					}
				}
				++tCtr;
				++toCheck;
			}
			break;
		case BasePage + 14:								// Declare peace list
			offCounter = tCtr = 0;
			ourList = gList[trgGuild]->GuildRelationList();
			if( ourList->size() <= ( button - 2 ))
			{
				Menu( s, BasePage + 2, trgGuild );
			}

			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second == GR_WAR )
				{
					if( offCounter == ( button - 2 ))
					{
						gList[trgGuild]->SetGuildRelation( tCtr, GR_NEUTRAL );
						gList[trgGuild]->TellMembers( 167, gList[tCtr]->Name().c_str() );
						gList[tCtr]->TellMembers( 168, gList[trgGuild]->Name().c_str() );
						break;
					}
					else
					{
						++offCounter;
					}
				}
				++tCtr;
				++toCheck;
			}
			break;
		case BasePage + 15:								// List of guilds warring on us
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage + 16:								// Display recruit information
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage + 17:								// Display member information
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage + 18:								// Ally list
			Menu( s, BasePage + 1, trgGuild );
			break;
		case BasePage + 19:								// Declare Ally list
			offCounter = tCtr = 0;
			ourList = gList[trgGuild]->GuildRelationList();
			toCheck = ourList->begin();
			while( toCheck != ourList->end() )
			{
				if( toCheck->second != GR_ALLY )
				{
					if( offCounter == ( button - 2 ))
					{
						gList[trgGuild]->SetGuildRelation( tCtr, GR_ALLY );
						gList[trgGuild]->TellMembers( 169, gList[tCtr]->Name().c_str() );
						gList[tCtr]->TellMembers( 170, gList[trgGuild]->Name().c_str() );
						break;
					}
					else
					{
						++offCounter;
					}
				}
				++tCtr;
				++toCheck;
			}
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Resign()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles player resigning from guild
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::Resign( CSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	SI16 guildNumber = mChar->GetGuildNumber();
	if( guildNumber == -1 )
		return;

	CGuild *nGuild = gList[guildNumber];
	if( nGuild == nullptr )
		return;

	nGuild->RemoveMember( *( s->CurrcharObj() ));
	s->SysMessage( 171 ); // You are no longer in that guild.
	mChar->SetGuildNumber( -1 );
	mChar->SetGuildTitle( "" );
	if( nGuild->Master() == mChar->GetSerial() && nGuild->NumMembers() != 0 )
	{
		nGuild->CalcMaster();
		auto newGuildMaster = CalcCharObjFromSer( nGuild->Master() );
		if( ValidateObject( newGuildMaster ))
		{
			nGuild->TellMembers( 1692, newGuildMaster->GetName().c_str(), nGuild->Name().c_str() ); // %s is now the new Guild Master of %s!
		}
	}

	if( nGuild->NumMembers() == 0 )
	{
		SERIAL stone = nGuild->Stone();
		if( stone != INVALIDSERIAL )
		{
			CItem *gStone = CalcItemObjFromSer( stone );
			if( ValidateObject( gStone ))
			{
				gStone->Delete();
			}
		}
		Erase( guildNumber );
		s->SysMessage( 172 ); // You have been the last member of that guild so the stone vanishes.
	}
	mChar->Dirty( UT_UPDATE );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::Erase()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Erase guild - there are no guild members left
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::Erase( GUILDID toErase )
{
	GUILDLIST::iterator pFind = gList.find( toErase );
	if( pFind == gList.end() )	// doesn't exist
		return;

	CGuild *gErase = pFind->second;
	if( gErase == nullptr )
	{
		gList.erase( pFind );
		return;
	}
	for( size_t iCounter = 0; iCounter < gErase->NumMembers(); ++iCounter )
	{
		SERIAL iMember	= gErase->MemberNumber( iCounter );
		CChar *member	= CalcCharObjFromSer( iMember );
		if( ValidateObject( member ))
		{
			member->SetGuildNumber( -1 );
		}
	}
	for( size_t iC = 0; iC < gErase->NumRecruits(); ++iC )
	{
		SERIAL iRecruit	= gErase->RecruitNumber( iC );
		CChar *recruit	= CalcCharObjFromSer( iRecruit );
		if( ValidateObject( recruit ))
		{
			recruit->SetGuildNumber( -1 );
		}
	}
	delete pFind->second;
	gList.erase( pFind );
}

CGuildCollection::~CGuildCollection()
{
	GUILDLIST::const_iterator i = gList.begin();
	while( i != gList.end() )
	{
		if( i->second != nullptr )
		{
			delete i->second;
		}
		++i;
	}

	gList.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::PlaceStone()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle placement of guildstone from deed, by player
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::PlaceStone( CSocket *s, CItem *deed )
{
	if( s == nullptr || !ValidateObject( deed ))
		return;

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	if( deed->GetId() == 0x14F0 )
	{
		if( mChar->GetGuildNumber() != -1 )	// in a guild
		{
			s->ObjMessage( 173, deed ); // You are already in a guild.
			return;
		}
		GUILDID gNum = NewGuild();
		CGuild *nGuild = Guild( gNum );
		if( nGuild == nullptr )
		{
			s->ObjMessage( 174, deed ); // Critical error adding guildstone, please contact a GM!
			Console.Error( oldstrutil::format( "Critical error adding guildstone, memory allocation failed.  Attempted by player 0x%X", mChar->GetSerial() ));
			return;
		}
		mChar->SetGuildNumber( gNum );
		s->TempInt( gNum );
		nGuild->NewMember(( *mChar ));
		CItem *stone = Items->CreateItem( nullptr, mChar, 0x0ED5, 1, 0, OT_ITEM );
		if( !ValidateObject( stone ))
		{
			s->ObjMessage( 176, deed ); // Critical error, unable to spawn guildstone, please contact a GM!
			Console.Error( oldstrutil::format( "Critical error spawning guildstone, no stone made.  Attempted by player 0x%X", mChar->GetSerial() ));
			return;
		}
		stone->SetName( Dictionary->GetEntry( 175 )); // Guildstone for an unnamed guild
		if( mChar->GetId() == 0x0191 || mChar->GetId() == 0x0193 || mChar->GetId() == 0x025E || mChar->GetId() == 0x0260 )
		{
			mChar->SetGuildTitle( "Guildmistress" );
		}
		else
		{
			mChar->SetGuildTitle( "Guildmaster" );
		}
		stone->SetLocation( mChar );
		nGuild->Webpage( DEFAULTWEBPAGE );
		nGuild->Charter( DEFAULTCHARTER );
		stone->SetType( IT_GUILDSTONE );
		stone->SetTempVar( CITV_MORE, gNum );
		stone->SetWipeable( false );
		stone->SetDecayable( false );
		deed->Delete();
		nGuild->Master(( *mChar ));
		nGuild->Stone(( *stone ));
		TextEntryGump( s, mChar->GetSerial(), 100, 1, 30, 158 );
	}
	else if( deed->GetId() == 0x1869 )
	{
		// Transporter stone for guildstone
		if( mChar->GetGuildNumber() == -1 )	// not in a guild
		{
			s->ObjMessage( "You don't appear to be in a guild", deed ); // You don't appear to be in a guild
			return;
		}
		GUILDID gNum = deed->GetTempVar( CITV_MORE );
		CGuild *nGuild = Guild( gNum );
		if( nGuild == nullptr )
		{
			s->ObjMessage( 174, deed ); // Critical error adding guildstone, please contact a GM!
			Console.Error( oldstrutil::format( "Critical error adding guildstone, memory allocation failed.  Attempted by player 0x%X", mChar->GetSerial() ));
			return;
		}
		CItem *stone = Items->CreateItem( nullptr, mChar, 0x0ED5, 1, 0, OT_ITEM );
		if( !ValidateObject( stone ))
		{
			s->ObjMessage( 176, deed ); // Critical error, unable to spawn guildstone, please contact a GM!
			Console.Error( oldstrutil::format( "Critical error spawning guildstone, no stone made.  Attempted by player 0x%X", mChar->GetSerial() ));
			return;
		}
		stone->SetName( oldstrutil::format( Dictionary->GetEntry( 101 ), nGuild->Name().c_str() ));
		stone->SetLocation( mChar );
		stone->SetType( IT_GUILDSTONE );
		stone->SetTempVar( CITV_MORE, gNum );
		stone->SetWipeable( false );
		stone->SetDecayable( false );
		deed->Delete();
		nGuild->Stone(( *stone ));
	}
	else
	{
		s->SysMessage( 177 ); // That is not a valid guildstone deed
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::ResultInCriminal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets/Gets whether action will result in criminal flag based on guild relation
//o------------------------------------------------------------------------------------------------o
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
	if( !ValidateObject( src ) || !ValidateObject( trg ))
		return false;

	return ResultInCriminal( src->GetGuildNumber(), trg->GetGuildNumber() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGuildCollection::DisplayTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display guild title for player character
//o------------------------------------------------------------------------------------------------o
void CGuildCollection::DisplayTitle( CSocket *s, CChar *src ) const
{
	if( !ValidateObject( src ) || s == nullptr )
		return;

	GUILDID sGuild = src->GetGuildNumber();
	if( sGuild != -1 && src->GetGuildToggle() )
	{
		std::string title;

		CGuild *mGuild = Guild( sGuild );
		if( mGuild == nullptr )
			return;

		auto abbreviation = std::string( mGuild->Abbreviation() );
		if( abbreviation.empty() ) // 0 length string
		{
			abbreviation = "none";
		}
		if( mGuild->Type() != GT_STANDARD )
		{
			auto guildtype= GTypeNames[mGuild->Type()];
			if( !src->GetGuildTitle().empty() )
			{
				title = oldstrutil::format( "[%s, %s] [%s]", src->GetGuildTitle().c_str(), abbreviation.c_str(), guildtype.c_str() );
			}
			else
			{
				title = oldstrutil::format(  "[%s] [%s]", abbreviation.c_str(), guildtype.c_str() );
			}
		}
		else
		{
			if( !src->GetGuildTitle().empty() )
			{
				title = oldstrutil::format( "[%s, %s]", src->GetGuildTitle().c_str(), abbreviation.c_str() );
			}
			else
			{
				title = oldstrutil::format( "[%s]", abbreviation.c_str() );
			}
		}
		s->ObjMessage( title, src );
	}
}
