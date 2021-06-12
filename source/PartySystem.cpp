#include "PartySystem.h"
#include "uox3.h"
#include "network.h"
#include "CPacketSend.h"
#include <mutex>
// PartyEntry code goes here
const size_t BIT_LEADER		= 0;
const size_t BIT_LOOTABLE		= 1;

CChar * PartyEntry::Member( void ) const	{	return member;							}
bool PartyEntry::IsLeader( void ) const		{	return settings.test( BIT_LEADER   );	}
bool PartyEntry::IsLootable( void ) const	{	return settings.test( BIT_LOOTABLE );	}
void PartyEntry::Member( CChar *valid )		{	member = valid;							}
void PartyEntry::IsLeader( bool value )		{	settings.set( BIT_LEADER, true   );		}
void PartyEntry::IsLootable( bool value )	{	settings.set( BIT_LOOTABLE, true );		}
PartyEntry::PartyEntry() : member( nullptr )	{	settings.reset();						}
PartyEntry::PartyEntry( CChar *m, bool isLeader, bool isLootable ) : member( m )
{
	settings.set( BIT_LEADER, isLeader );
	settings.set( BIT_LOOTABLE, isLootable );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AddMember( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Add new member to party
//o-----------------------------------------------------------------------------------------------o
bool Party::AddMember( CChar *i )
{
	bool retVal = false;
	if( ValidateObject( i ) )
	{
		if( !HasMember( i ) )
		{
			PartyEntry *toAdd	= new PartyEntry( i );
			PartyFactory::getSingleton().AddLookup( this, i );
			members.push_back( toAdd );
			SendList( nullptr );
			retVal = true;
		}
	}
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	PartyEntry *Find( CChar *i, SI32 *location )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character is a member of party
//o-----------------------------------------------------------------------------------------------o
PartyEntry *Party::Find( CChar *i, SI32 *location )
{
	if( ValidateObject( i ) )
	{
		for( size_t j = 0; j < members.size(); ++j )
		{
			PartyEntry *toFind = members[j];
			if( toFind->Member() == i )
			{
				if( location != nullptr )
					(*location) = static_cast<int>(j);
				return toFind;
			}
		}
	}
	return nullptr;
}
bool Party::HasMember( CChar *find )
{
	return ( Find( find ) != nullptr );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RemoveMember( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a character from the party
//o-----------------------------------------------------------------------------------------------o
bool Party::RemoveMember( CChar *i )
{
	bool retVal = false;
	if( ValidateObject( i ) )
	{
		SI32 removeSpot;
		PartyEntry *toFind = Find( i, &removeSpot );
		if( toFind != nullptr )
		{
			delete members[removeSpot];
			members.erase( members.begin() + removeSpot );
			PartyFactory::getSingleton().RemoveLookup( i );

			CPPartyMemberRemove toSend( i );
			for( size_t j = 0; j < members.size(); ++j )
			{
				PartyEntry *toFind = members[j];
				toSend.AddMember( toFind->Member() );
			}

			SendPacket( &toSend, nullptr );
			if( i->GetSocket() != nullptr )
				SendPacket( &toSend, i->GetSocket() );
			retVal = true;
		}
	}
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Leader( CChar *member )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Set a party member as party leader
//o-----------------------------------------------------------------------------------------------o
void Party::Leader( CChar *member )
{
	SI32 newLeaderPos;
	PartyEntry *newLeader = Find( member, &newLeaderPos );
	if( newLeader != nullptr )
	{
		if( leader != nullptr )
		{
			SI32 oldLeaderPos;
			PartyEntry *mFind = Find( leader, &oldLeaderPos );
			if( mFind != nullptr )
			{
				mFind->IsLeader( false );
				// We need to swap their position in the array, because the first cab
				// off the rank is the leader, and the client makes assumptions about
				// this
				members[oldLeaderPos] = newLeader;
				members[newLeaderPos] = mFind;
			}
		}
		leader = newLeader->Member();
		newLeader->IsLeader( true );
	}
}
CChar *Party::Leader( void )	{	return leader;	}
Party::Party( bool npc ) : leader( nullptr ), isNPC( npc )	{					}
Party::Party( CChar *ldr, bool npc ) : leader( nullptr ), isNPC( npc )
{
	if( ValidateObject( ldr ) )
	{
		AddMember( ldr );
		Leader( ldr );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendPacket( CPUOXBuffer *toSend, CSocket *toSendTo )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send list of party members to client
//o-----------------------------------------------------------------------------------------------o
void Party::SendPacket( CPUOXBuffer *toSend, CSocket *toSendTo )
{
	if( toSendTo != nullptr )
		toSendTo->Send( toSend );
	else
	{
		for( size_t k = 0; k < members.size(); ++k )
		{
			PartyEntry *toFind	= members[k];
			CSocket *tSock		= toFind->Member()->GetSocket();
			if( tSock != nullptr )
				tSock->Send( toSend );
		}
	}
}
void Party::SendList( CSocket *toSendTo )
{
	CPPartyMemberList toSend;
	for( size_t j = 0; j < members.size(); ++j )
	{
		PartyEntry *toFind = members[j];
		toSend.AddMember( toFind->Member() );
	}
	SendPacket( &toSend, toSendTo );
}
bool Party::IsNPC( void ) const
{
	return isNPC;
}
void Party::IsNPC( bool value )
{
	isNPC = value;
}

/** This class is responsible for the creation and destruction of parties
 */
//-------------------------------------------------------------------------------------------------

PartyFactory& PartyFactory::getSingleton( void )
{
	std::mutex lock;
	std::scoped_lock scope(lock) ;
	static PartyFactory instance ;
	return instance ;
}
//-------------------------------------------------------------------------------------------------

void PartyFactory::AddLookup( Party *toQuickLook, CChar *toSave )
{
	if( ValidateObject( toSave ) )
		partyQuickLook[toSave->GetSerial()] = toQuickLook;
}
void PartyFactory::RemoveLookup( CChar *toRemove )
{
	if( ValidateObject( toRemove ) )
	{
		std::map< SERIAL, Party * >::iterator toFind = partyQuickLook.find( toRemove->GetSerial() );
		if( toFind != partyQuickLook.end() )
			partyQuickLook.erase( toFind );
	}
}
PartyFactory::PartyFactory()
{
	partyQuickLook.clear();
}
PartyFactory::~PartyFactory()
{
	for( Party *obj = parties.First(); !parties.Finished(); obj = parties.Next() )
	{
		delete obj;
		obj = nullptr;
	}
}
Party *PartyFactory::Create( CChar *leader )
{
	Party *toAdd	= nullptr;
	if( ValidateObject( leader ) )
	{
		toAdd		= new Party( leader );
		parties.Add( toAdd );
	}
	return toAdd;
}
void PartyFactory::Destroy( CChar *member )
{
	Party *toRemove = Get( member );
	Destroy( toRemove );
}
void PartyFactory::Destroy( Party *toRemove )
{
	if( toRemove != nullptr )
	{
		std::vector< PartyEntry * > *mList = toRemove->MemberList();
		if( mList != nullptr )
		{
			for( size_t j = 0; j < mList->size(); ++j )
			{
				PartyEntry *mEntry = (*mList)[j];
				RemoveLookup( mEntry->Member() );
			}
		}
		parties.Remove( toRemove );
		delete toRemove;
	}
}
Party *PartyFactory::Get( CChar *member )
{
	if( ValidateObject( member ) )
	{
		std::map< SERIAL, Party * >::iterator toFind = partyQuickLook.find( member->GetSerial() );
		if( toFind != partyQuickLook.end() )
			return toFind->second;
		else
			return nullptr;
	}
	else
		return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CreateInvite( CSocket *inviter )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Invite a player to the party
//o-----------------------------------------------------------------------------------------------o
void PartyFactory::CreateInvite( CSocket *inviter )
{
	SERIAL serial	= inviter->GetDWord( 7 );
	CChar *toInvite	= calcCharObjFromSer( serial );
	if( !ValidateObject( toInvite ) || toInvite->IsNpc() )
	{
		inviter->sysmessage( "You cannot invite an npc or unknown player" );
		return;
	}
	CChar *inviterChar = inviter->CurrcharObj();
	if( ValidateObject( inviterChar ) && inviterChar == toInvite )
	{
		inviter->sysmessage( "You cannot invite yourself to a party" );
		return;
	}
	Party *ourParty = Get( inviterChar );
	if( ourParty == nullptr )
	{
		//Party *tParty = Create( inviterChar );
		Create( inviterChar);
	}
	CSocket *targSock = toInvite->GetSocket();
	if( targSock != nullptr )
	{
		CPPartyInvitation toSend;
		toSend.Leader( inviterChar );
		targSock->Send( &toSend );
		targSock->sysmessage( "You have been invited to join a party, type /accept or /decline to deal with the invitation" );
	}
	else
	{
		inviter->sysmessage( "That player is not online" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Kick( CSocket *inviter )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Kick a member from the party
//o-----------------------------------------------------------------------------------------------o
void PartyFactory::Kick( CSocket *inviter )
{
	SERIAL serial	= inviter->GetDWord( 7 );
	CChar *toRemove	= calcCharObjFromSer( serial );
	if( !ValidateObject( toRemove ) || toRemove->IsNpc() )
	{
		inviter->sysmessage( "You cannot kick an npc or unknown player" );
		return;
	}
	Party *ourParty = Get( inviter->CurrcharObj() );
	if( ourParty == nullptr )
	{
		inviter->sysmessage( "You are not in a party and cannot kick them out" );
		return;
	}
	if( ( ourParty->Leader() != inviter->CurrcharObj() ) && ( inviter->CurrcharObj() != toRemove ) )
	{
		inviter->sysmessage( "Only the leader can kick someone out" );
		return;
	}
	if( ourParty->HasMember( toRemove ) )
	{	// even if they're offline, we can kick them out
		ourParty->RemoveMember( toRemove );
		inviter->sysmessage( "The player has been removed" );
	}
}

