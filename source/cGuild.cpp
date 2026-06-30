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

static void ClearCharacterGuildState( CChar *toClear )
{
	if( !ValidateObject( toClear ))
		return;

	toClear->SetGuildNumber( -1 );
	toClear->SetGuildTitle( "" );
	toClear->SetGuildFealty( 0 );
}

CGuild::CGuild() : name( "" ), gType( GT_STANDARD ), charter( "" ), webpage( "" ), stone( INVALIDSERIAL ), master( INVALIDSERIAL )
{
	abbreviation[0] = 0;
	recruits.resize( 0 );
	members.resize( 0 );
    invites.resize( 0 );
	recruitPtr		= recruits.end();
	memberPtr		= members.end();
    invitePtr		= invites.end();
	warPtr			= relationList.end();
	allyPtr			= relationList.end();
}

CGuild::~CGuild()
{
	JSEngine->ReleaseObject( IUE_GUILD, this );
	recruits.clear();
	members.clear();
    invites.clear();
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
	if( !IsRecruit( newRecruit ))
	{
		recruits.push_back( newRecruit );
	}
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
	if( !IsMember( newMember ))
	{
		members.push_back( newMember );
	}
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
	rankOf.erase( newRecruit );
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
	rankOf.erase( newMember );
	for( SERIAL memberSerial : members )
	{
		CChar *member = CalcCharObjFromSer( memberSerial );
		if( ValidateObject( member ) && member->GetGuildFealty() == newMember )
		{
			member->SetGuildFealty( 0 );
		}
	}
	if( master == newMember )
	{
		CalcMaster();
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
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets guild's relation to another guild
//o------------------------------------------------------------------------------------------------o
void CGuild::SetGuildRelation( GUILDID otherGuild, GUILDRELATION toSet )
{
	if( otherGuild == -1 || toSet < GR_NEUTRAL || toSet >= GR_COUNT )
		return;
	relationList[otherGuild] = toSet;
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
    orderByPrio.clear();
    for( size_t i = 0; i < ranks.size(); ++i )
    {
        if( ranks[i].prio != std::numeric_limits<SI32>::max() )
            orderByPrio.push_back( i );
    }

    std::stable_sort( orderByPrio.begin(), orderByPrio.end(),
        [&]( size_t ida, size_t idb )
        {
            return ranks[ida].prio < ranks[idb].prio;
        } );

    idToOrderIndex.assign( ranks.size(), std::numeric_limits<size_t>::max() );
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
    std::string cleanName = oldstrutil::trim( name );
    if( cleanName.empty() )
        return std::numeric_limits<size_t>::max();

    for( size_t i = 0; i < ranks.size(); ++i )
    {
        if( iequals_ascii( ranks[i].name, cleanName ) )
        {
            ranks[i].prio  = prio;
            ranks[i].flags = flags;
            RebuildRankOrder_();
            return i;
        }
    }
    ranks.push_back( { cleanName, prio, flags } );
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
    std::string cleanName = oldstrutil::trim( name );
    if( cleanName.empty() ) return false;

    SI32 id = FindRankIdByName( cleanName );
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
    std::string cleanOldName = oldstrutil::trim( oldName );
    std::string cleanNewName = oldstrutil::trim( newName );
    if( cleanOldName.empty() || cleanNewName.empty() ) return false;

    SI32 id = FindRankIdByName( cleanOldName );
    if( id < 0 ) return false;
    ranks[ static_cast<size_t>(id) ].name = cleanNewName;
    return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::FindRankIdByName()
//| Prototype   -  SI32 FindRankIdByName( const std::string& name ) const
//| Purpose     -  Lookup rankId by name (case-insensitive); -1 if missing.
//o------------------------------------------------------------------------------------------------o
SI32 CGuild::FindRankIdByName( const std::string& name ) const
{
    std::string cleanName = oldstrutil::trim( name );
    if( cleanName.empty() ) return -1;

    for( size_t i = 0; i < ranks.size(); ++i )
        if( ranks[i].prio != std::numeric_limits<SI32>::max() && iequals_ascii( ranks[i].name, cleanName ) )
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
    if( id >= ranks.size() || ranks[id].prio == std::numeric_limits<SI32>::max() )
        return -1;
    return static_cast<SI32>(id);
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
//| Purpose     -  Assign/check a character's rank.
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
    if( id >= ranks.size() || ranks[id].prio == std::numeric_limits<SI32>::max() )
        return "";
    return ranks[id].name;
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
    if( pos == std::numeric_limits<size_t>::max() ) return false;
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
    if( pos == std::numeric_limits<size_t>::max() ) return false;
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
    if (ranks[id].prio == std::numeric_limits<SI32>::max()) return std::string();
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
    if (ranks[id].prio == std::numeric_limits<SI32>::max()) return false;
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
    {
        if( r.prio != std::numeric_limits<SI32>::max() )
            toSave << "RANKDEF=" << r.name << ',' << r.prio << ',' << r.flags << '\n';
    }

    for( auto s : recruits )
        toSave << "RECRUIT=" << s << '\n';
    for( auto s : members )
        toSave << "MEMBER=" << s << '\n';

    for( const auto& kv : rankOf )
    {
        size_t rid = kv.second;
        if( rid < ranks.size() && ranks[rid].prio != std::numeric_limits<SI32>::max() )
            toSave << "RANKMEM=" << ranks[rid].name << ',' << kv.first << '\n';
    }

    for( auto it = relationList.begin(); it != relationList.end(); ++it )
    {
        if( it->second >= GR_NEUTRAL && it->second < GR_COUNT )
            toSave << GRelationNames[it->second] << ' ' << it->first << '\n';
    }

	// Pending relation requests (RELREQ=guildId,relationInt)
	for( const auto& r : relationRequests )
	{
		toSave << "RELREQ=" << r.fromGuildId << ',' << static_cast<int>( r.relation ) << '\n';
	}

    toSave << "}\n\n";
}

//o------------------------------------------------------------------------------------------------o
//| Function    -  CGuild::Load()
//| Purpose     -  Load guild data (accepts dynamic and legacy formats).
//o------------------------------------------------------------------------------------------------o
void CGuild::Load( CScriptSection* toRead )
{
    invites.clear();
    recruits.clear();
    members.clear();
    rankOf.clear();
    ranks.clear();
    orderByPrio.clear();
    idToOrderIndex.clear();
    relationList.clear();
    warPtr = relationList.end();
    allyPtr = relationList.end();
	relationRequests.clear(); // <-- important for reloads

    bool sawRankDef = false;
    bool sawMembership = false;
    bool sawRankMembership = false;

    auto setRankLegacy = [&]( const char* rankName, const std::string& data )
    {
        SERIAL s = static_cast<UI32>( std::stoul( data, nullptr, 0 ) );
        if( std::string( rankName ) == "Recruit" )
            NewRecruit( s );
        else
            NewMember( s );
        AddRank( rankName, /*prio*/ 0 ); // no-op if exists
        SetRank( s, rankName );
        sawMembership = true;
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
                        if( FindRankIdByName( name ) < 0 )
                            AddRank( name, (SI32)ranks.size() );
                        SetRank( s, name );
                        sawRankMembership = true;
                    }
                }
				else if( UTag == "RELREQ" )
				{
					// data: "fromGuildId,relationInt"
					auto p = data.find( ',' );
					if( p != std::string::npos )
					{
						auto fromStr = data.substr( 0, p );
						auto relStr = data.substr( p + 1 );

						UI32 fromId = static_cast< UI32 >( std::stoul( fromStr, nullptr, 0 ) );
						int  relInt = std::stoi( relStr, nullptr, 0 );

						if( relInt < GR_NEUTRAL ) relInt = GR_NEUTRAL;
						if( relInt > GR_ALLY ) relInt = GR_ALLY;

						AddRelationRequest( static_cast< GUILDID >( fromId ),
							static_cast< GUILDRELATION >( relInt ) );
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

            case 'W':
                if( UTag == "WEBPAGE" ) Webpage( data );
                else if( UTag == "WAR" ) SetGuildRelation( static_cast<SI16>( std::stoi( data, nullptr, 0 ) ), GR_WAR );
                break;
        }
    }

    if( !sawMembership && sawRankMembership )
    {
        for( const auto& kv : rankOf )
        {
            if( !IsMember( kv.first ) && !IsRecruit( kv.first ) )
                NewMember( kv.first );
        }
    }

    // If file provided no rank defs, ensure defaults for legacy mapping.
    if( !sawRankDef )
    {
        AddRank( "Recruit", 0 );
        AddRank( "Member",  20 );
        AddRank( "Veteran", 30 );
        AddRank( "Officer", 40 );
        AddRank( "Guild Master", 50 );
    }

    if( master != INVALIDSERIAL && !IsMember( master ) )
        master = INVALIDSERIAL;

    if( master == INVALIDSERIAL && !members.empty() )
        CalcMaster();

    if( master != INVALIDSERIAL && FindRankIdByName( "Guild Master" ) >= 0 )
    {
        if( GetRankName( master ) != "Guild Master" )
            SetRank( master, "Guild Master" );
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

//o-------------------------------------------------------------------------------------------------o
//| Function	-	CGuildCollection::SetRelation
//o-------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the relation between two guilds (war/ally/neutral/unknown)
//| Notes		-	Updates relationList for *both* guilds so Compare() stays symmetric
//o-------------------------------------------------------------------------------------------------o
bool CGuildCollection::SetRelation( GUILDID guildOne, GUILDID guildTwo, GUILDRELATION relation )
{
	if( guildOne == guildTwo )
		return false;	// "same" is meaningless here

	CGuild* firstGuild  = Guild( guildOne );
	CGuild* secondGuild = Guild( guildTwo );

	if( firstGuild == nullptr || secondGuild == nullptr )
		return false;

	// Apply relation both ways so RelatedToGuild() sees the same result from either side
	firstGuild->SetGuildRelation( guildTwo, relation );
	secondGuild->SetGuildRelation( guildOne, relation );

	return true;
}

//o-------------------------------------------------------------------------------------------------o
//| Function	-	CGuildCollection::SendRelationRequest
//o-------------------------------------------------------------------------------------------------o
//| Purpose		-	Entry point for JS to ask for a relation change between two guilds
//| Notes		-	Currently applies the relation immediately via SetRelation()
//|					Later you can turn this into a "pending request" system that
//|					requires the target guild to accept/deny the request.
//o-------------------------------------------------------------------------------------------------o
bool CGuildCollection::SendRelationRequest( GUILDID fromGuild, GUILDID toGuild, GUILDRELATION relation )
{
    if( fromGuild == toGuild )
        return false;

    CGuild* src = Guild( fromGuild );
    CGuild* trg = Guild( toGuild );
    if( src == nullptr || trg == nullptr )
        return false;

    // Clamp relation into valid enum range
    if( relation < GR_NEUTRAL )
        relation = GR_NEUTRAL;
    if( relation > GR_SAME )
        relation = GR_SAME;

    // Do NOT set relation here - just queue a pending request on target guild.
    trg->AddRelationRequest( fromGuild, relation );

    // Optional: notify online members of trg guild here
    // (iterate members, send sysmsg etc)

    return true;
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

	bool wasGuildMaster = ( nGuild->Master() == mChar->GetSerial() );
	nGuild->RemoveMember( *( s->CurrcharObj() ));
	s->SysMessage( 171 ); // You are no longer in that guild.
	ClearCharacterGuildState( mChar );
	if( wasGuildMaster && nGuild->NumMembers() != 0 )
	{
		auto newGuildMaster = CalcCharObjFromSer( nGuild->Master() );
		if( ValidateObject( newGuildMaster ))
		{
			std::string charName = newGuildMaster->GetName();
			if( newGuildMaster->IsIncognito() || newGuildMaster->IsDisguised() )
			{
				charName = newGuildMaster->GetOrgName();
			}
			nGuild->TellMembers( 1692, charName.c_str(), nGuild->Name().c_str() ); // %s is now the new Guild Master of %s!
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

	for( auto& guildEntry : gList )
	{
		if( guildEntry.first == toErase || guildEntry.second == nullptr )
			continue;

		guildEntry.second->RemoveRelationWithGuild( toErase );
		guildEntry.second->RemoveRelationRequestsFromGuild( toErase );
	}

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
			ClearCharacterGuildState( member );
		}
	}
	for( size_t iC = 0; iC < gErase->NumRecruits(); ++iC )
	{
		SERIAL iRecruit	= gErase->RecruitNumber( iC );
		CChar *recruit	= CalcCharObjFromSer( iRecruit );
		if( ValidateObject( recruit ))
		{
			ClearCharacterGuildState( recruit );
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

// new function
void CGuildCollection::GetAllGuilds( std::vector<CGuild*>& outGuilds ) const
{
	outGuilds.clear();

	for( GUILDLIST::const_iterator itr = gList.begin(); itr != gList.end(); ++itr )
	{
		if( itr->second != nullptr )
			outGuilds.push_back( itr->second );
	}
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
