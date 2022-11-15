#include "PartySystem.h"
#include "uox3.h"
#include "network.h"
#include "CPacketSend.h"
#include <mutex>
// CPartyEntry code goes here
const size_t BIT_LEADER		= 0;
const size_t BIT_LOOTABLE	= 1;

CChar * CPartyEntry::Member( void ) const	{	return member;							}
bool CPartyEntry::IsLeader( void ) const		{	return settings.test( BIT_LEADER   );	}
bool CPartyEntry::IsLootable( void ) const	{	return settings.test( BIT_LOOTABLE );	}
void CPartyEntry::Member( CChar *valid )		{	member = valid;							}
void CPartyEntry::IsLeader( [[maybe_unused]] bool value )		{	settings.set( BIT_LEADER, true   );		}
void CPartyEntry::IsLootable( [[maybe_unused]] bool value )	{	settings.set( BIT_LOOTABLE, true );		}
CPartyEntry::CPartyEntry() : member( nullptr )	{	settings.reset();						}
CPartyEntry::CPartyEntry( CChar *m, bool isLeader, bool isLootable ) : member( m )
{
	settings.set( BIT_LEADER, isLeader );
	settings.set( BIT_LOOTABLE, isLootable );
}

void UpdateStats( CBaseObject *mObj, UI08 x );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	Party::AddMember()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add new member to party
//o------------------------------------------------------------------------------------------------o
bool Party::AddMember( CChar *i )
{
	bool retVal = false;
	if( ValidateObject( i ) && IsOnline( *i ))
	{
		if( !HasMember( i ))
		{
			CPartyEntry *toAdd = new CPartyEntry( i );
			PartyFactory::GetSingleton().AddLookup( this, i );
			members.push_back( toAdd );
			SendList( nullptr );
			retVal = true;

			CSocket *newSock = i->GetSocket();
			newSock->SysMessage( 9072 ); // You have been added to the party.

			// Send status update to ALL party members
			for( size_t j = 0; j < members.size(); ++j )
			{
				CPartyEntry *toFind = members[j];
				CChar *partyMember = toFind->Member();
				if( partyMember != nullptr )
				{
					if( partyMember->GetSerial() != i->GetSerial() )
					{
						// If party member is online, send them info on the new member
						if( IsOnline( *partyMember ))
						{
							CSocket *s = partyMember->GetSocket();

							// Send stat window update for new member to existing party members
							s->StatWindow( i );

							// Prepare the stat update packet for new member to existing party members
							CPUpdateStat toSendHp(( *i ), 0, true );
							s->Send( &toSendHp );
							CPUpdateStat toSendMana(( *i ), 1, true );
							s->Send( &toSendMana );
							CPUpdateStat toSendStam(( *i ), 2, true );
							s->Send( &toSendStam );

							// Also send info on the existing party member to the new member!
							// Send stat window update packet for existing member to new party member
							newSock->StatWindow( partyMember );

							// Prepare the stat update packet for existing member to new party members
							CPUpdateStat toSendHp2(( *partyMember ), 0, true );
							newSock->Send( &toSendHp2 );
							CPUpdateStat toSendMana2(( *partyMember ), 1, true );
							newSock->Send( &toSendMana2 );
							CPUpdateStat toSendStam2(( *partyMember ), 2, true );
							newSock->Send( &toSendStam2 );

							s->SysMessage( 9076, i->GetNameRequest( partyMember ).c_str() ); // %s joined the party.
						}
					}
				}
			}
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Party::Find()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if character is a member of party
//o------------------------------------------------------------------------------------------------o
CPartyEntry *Party::Find( CChar *i, SI32 *location )
{
	if( ValidateObject( i ))
	{
		for( size_t j = 0; j < members.size(); ++j )
		{
			CPartyEntry *toFind = members[j];
			if( toFind->Member() == i )
			{
				if( location != nullptr )
				{
					( *location ) = static_cast<int>( j );
				}
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Party::RemoveMember()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a character from the party
//o------------------------------------------------------------------------------------------------o
bool Party::RemoveMember( CChar *i )
{
	bool retVal = false;
	if( ValidateObject( i ))
	{
		SI32 removeSpot;
		CPartyEntry *toFind = Find( i, &removeSpot );
		if( toFind != nullptr )
		{
			delete members[removeSpot];
			members.erase( members.begin() + removeSpot );
			PartyFactory::GetSingleton().RemoveLookup( i );

			CPPartyMemberRemove toSend( i );
			for( size_t j = 0; j < members.size(); ++j )
			{
				CPartyEntry *toFind = members[j];
				toSend.AddMember( toFind->Member() );
				if( IsOnline( *toFind->Member() ) && !toFind->IsLeader() )
				{
					toFind->Member()->GetSocket()->SysMessage( 9075 ); // A player has been removed from your party.
				}
			}

			SendPacket( &toSend, nullptr );
			if( i->GetSocket() != nullptr )
			{
				SendPacket( &toSend, i->GetSocket() );
				i->GetSocket()->SysMessage( 9074 ); // You have been removed from the party.
			}
			retVal = true;
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Party::Leader()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set a party member as party leader
//o------------------------------------------------------------------------------------------------o
void Party::Leader( CChar *member )
{
	SI32 newLeaderPos;
	CPartyEntry *newLeader = Find( member, &newLeaderPos );
	if( newLeader != nullptr )
	{
		if( leader != nullptr )
		{
			SI32 oldLeaderPos;
			CPartyEntry *mFind = Find( leader, &oldLeaderPos );
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
CChar *Party::Leader( void )
{
	return leader;
}

Party::Party( bool npc ) : leader( nullptr ), isNPC( npc )
{
}

Party::Party( CChar *ldr, bool npc ) : leader( nullptr ), isNPC( npc )
{
	if( ValidateObject( ldr ))
	{
		AddMember( ldr );
		Leader( ldr );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Party::SendPacket()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send list of party members to client
//o------------------------------------------------------------------------------------------------o
void Party::SendPacket( CPUOXBuffer *toSend, CSocket *toSendTo )
{
	if( toSendTo != nullptr )
	{
		toSendTo->Send( toSend );
	}
	else
	{
		for( size_t k = 0; k < members.size(); ++k )
		{
			CPartyEntry *toFind	= members[k];
			CSocket *tSock		= toFind->Member()->GetSocket();
			if( tSock != nullptr )
			{
				tSock->Send( toSend );
			}
		}
	}
}
void Party::SendList( CSocket *toSendTo )
{
	CPPartyMemberList toSend;
	for( size_t j = 0; j < members.size(); ++j )
	{
		CPartyEntry *toFind = members[j];
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

PartyFactory& PartyFactory::GetSingleton( void )
{
	std::mutex lock;
	std::scoped_lock scope( lock );
	static PartyFactory instance;
	return instance;
}
//-------------------------------------------------------------------------------------------------

void PartyFactory::AddLookup( Party *toQuickLook, CChar *toSave )
{
	if( ValidateObject( toSave ))
	{
		partyQuickLook[toSave->GetSerial()] = toQuickLook;
	}
}
void PartyFactory::RemoveLookup( CChar *toRemove )
{
	if( ValidateObject( toRemove ))
	{
		std::map< SERIAL, Party * >::iterator toFind = partyQuickLook.find( toRemove->GetSerial() );
		if( toFind != partyQuickLook.end() )
		{
			partyQuickLook.erase( toFind );
		}
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
	if( ValidateObject( leader ))
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
		std::vector<CPartyEntry *> *mList = toRemove->MemberList();
		if( mList != nullptr )
		{
			for( size_t j = 0; j < mList->size(); ++j )
			{
				CPartyEntry *mEntry = ( *mList )[j];
				RemoveLookup( mEntry->Member() );
			}
		}
		parties.Remove( toRemove );
		delete toRemove;
	}
}
Party *PartyFactory::Get( CChar *member )
{
	if( ValidateObject( member ))
	{
		std::map<SERIAL, Party *>::iterator toFind = partyQuickLook.find( member->GetSerial() );
		if( toFind != partyQuickLook.end() )
		{
			return toFind->second;
		}
		else
		{
			return nullptr;
		}
	}
	else
		return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PartyFactory::CreateInvite()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Invite a player to the party
//o------------------------------------------------------------------------------------------------o
void PartyFactory::CreateInvite( CSocket *inviter )
{
	SERIAL serial	= inviter->GetDWord( 7 );
	CChar *toInvite	= CalcCharObjFromSer( serial );
	if( !ValidateObject( toInvite ) || toInvite->IsNpc() )
	{
		inviter->SysMessage( 9040 ); // You cannot invite an NPC or unknown player.
		return;
	}
	CChar *inviterChar = inviter->CurrcharObj();
	if( ValidateObject( inviterChar ) && inviterChar == toInvite )
	{
		inviter->SysMessage( 9041 ); // You cannot invite yourself to a party.
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
		targSock->SysMessage( 9002 ); // You have been invited to join a party, type /accept or /decline to deal with the invitation
	}
	else
	{
		inviter->SysMessage( 9042 ); // That player is not online.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PartyFactory::Kick()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Kick a member from the party
//o------------------------------------------------------------------------------------------------o
void PartyFactory::Kick( CSocket *inviter )
{
	SERIAL serial	= inviter->GetDWord( 7 );
	CChar *toRemove	= CalcCharObjFromSer( serial );
	if( !ValidateObject( toRemove ) || toRemove->IsNpc() )
	{
		inviter->SysMessage( 9043 ); // You cannot kick an NPC or unknown player.
		return;
	}
	Party *ourParty = Get( inviter->CurrcharObj() );
	if( ourParty == nullptr )
	{
		inviter->SysMessage( 9044 ); // You are not in a party and cannot kick them out.
		return;
	}
	if(( ourParty->Leader() != inviter->CurrcharObj() ) && ( inviter->CurrcharObj() != toRemove ))
	{
		inviter->SysMessage( 9045 ); // Only the leader can kick someone from a party.
		return;
	}
	if( ourParty->HasMember( toRemove ))
	{
		// even if they're offline, we can kick them out
		ourParty->RemoveMember( toRemove );
		inviter->SysMessage( 9046 ); // The player has been removed from the party.
	}
}

