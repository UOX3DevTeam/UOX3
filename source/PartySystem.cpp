#include "PartySystem.h"
#include "uox3.h"
#include "network.h"
#include "CPacketSend.h"

namespace UOX
{
	// PartyEntry code goes here
	const BIT_LEADER		= 0;
	const BIT_LOOTABLE		= 1;

	CChar * PartyEntry::Member( void ) const	{	return member;							}
	bool PartyEntry::IsLeader( void ) const		{	return settings.test( BIT_LEADER   );	}
	bool PartyEntry::IsLootable( void ) const	{	return settings.test( BIT_LOOTABLE );	}
	void PartyEntry::Member( CChar *valid )		{	member = valid;							}
	void PartyEntry::IsLeader( bool value )		{	settings.set( BIT_LEADER, true   );		}
	void PartyEntry::IsLootable( bool value )	{	settings.set( BIT_LOOTABLE, true );		}
	PartyEntry::PartyEntry() : member( NULL )	{	settings.reset();						}
	PartyEntry::PartyEntry( CChar *m, bool isLeader, bool isLootable ) : member( m )
	{
		settings.set( BIT_LEADER, isLeader );
		settings.set( BIT_LOOTABLE, isLootable );
	}

	// Party class!
	void Party::AddMember( CChar *i )
	{
		if( ValidateObject( i ) )
		{
			bool isFound = (Find( i ) != NULL);
			if( !isFound )
			{
				PartyEntry *toAdd	= new PartyEntry( i );
				PartyFactory::getSingleton().AddLookup( this, i );
				members.push_back( toAdd );
				SendList( NULL );
			}
		}
	}
	PartyEntry *Party::Find( CChar *i, int *location )
	{
		if( ValidateObject( i ) )
		{
			for( size_t j = 0; j < members.size(); ++j )
			{
				PartyEntry *toFind = members[j];
				if( toFind->Member() == i )
				{
					if( location != NULL )
						(*location) = j;
					return toFind;
				}
			}
		}
		return NULL;
	}
	bool Party::HasMember( CChar *find )
	{
		return ( Find( find ) != NULL );
	}
	void Party::RemoveMember( CChar *i )
	{
		if( ValidateObject( i ) )
		{
			int removeSpot;
			PartyEntry *toFind = Find( i, &removeSpot );
			if( toFind != NULL )
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

				SendPacket( &toSend, NULL );
				if( i->GetSocket() != NULL )
					SendPacket( &toSend, i->GetSocket() );
			}
		}
	}
	void Party::Leader( CChar *member )
	{
		PartyEntry *newLeader = Find( member );
		if( newLeader != NULL )
		{
			if( leader != NULL )
			{
				PartyEntry *mFind = Find( leader );
				if( mFind != NULL )
					mFind->IsLeader( false );
			}
			leader = newLeader->Member();
			newLeader->IsLeader( true );
		}
	}
	CChar *Party::Leader( void )	{	return leader;	}
	Party::Party() : leader( NULL )	{					}
	Party::Party( CChar *ldr ) : leader( NULL )
	{
		if( ValidateObject( ldr ) )
		{
			AddMember( ldr );
			Leader( ldr );
		}
	}
	void Party::SendPacket( CPUOXBuffer *toSend, CSocket *toSendTo )
	{
		if( toSendTo != NULL )
			toSendTo->Send( toSend );
		else
		{
			for( size_t k = 0; k < members.size(); ++k )
			{
				PartyEntry *toFind	= members[k];
				CSocket *tSock		= toFind->Member()->GetSocket();
				if( tSock != NULL )
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

/** This class is responsible for the creation and destruction of parties
*/
    //---------------------------------------------------------------------------------------------
    template<> PartyFactory * Singleton< PartyFactory >::ms_Singleton = 0;
    PartyFactory* PartyFactory::getSingletonPtr( void )
    {
        return ms_Singleton;
    }
    PartyFactory& PartyFactory::getSingleton( void )
    {  
        assert( ms_Singleton );  return ( *ms_Singleton );  
    }
    //---------------------------------------------------------------------------------------------

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
			obj = NULL;
		}
	}
	Party *PartyFactory::Create( CChar *leader )
	{
		Party *toAdd	= NULL;
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
		if( toRemove != NULL )
		{
			std::vector< PartyEntry * > *mList = toRemove->MemberList();
			if( mList != NULL )
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
				return NULL;
		}
		else
			return NULL;
	}
	void PartyFactory::CreateInvite( CSocket *inviter )
	{
		SERIAL serial	= inviter->GetDWord( 7 );
		CChar *toInvite	= calcCharObjFromSer( serial );
		if( !ValidateObject( toInvite ) || toInvite->IsNpc() )
		{
			inviter->sysmessage( "You cannot invite an npc or unknown player" );
		}
		Party *ourParty = Get( inviter->CurrcharObj() );
		if( ourParty == NULL )
		{
			Party *tParty = Create( inviter->CurrcharObj() );
		}
		CSocket *targSock = toInvite->GetSocket();
		if( targSock != NULL )
		{
			CPPartyInvitation toSend;
			toSend.Leader( inviter->CurrcharObj() );
			targSock->Send( &toSend );
			targSock->sysmessage( "You have been invited to join a party, type /accept or /decline to deal with the invitation" );
		}
		else
		{
			inviter->sysmessage( "That player is not online" );
		}
	}
	void PartyFactory::Kick( CSocket *inviter )
	{
		SERIAL serial	= inviter->GetDWord( 7 );
		CChar *toRemove	= calcCharObjFromSer( serial );
		if( !ValidateObject( toRemove ) || toRemove->IsNpc() )
		{
			inviter->sysmessage( "You cannot kick an npc or unknown player" );
		}
		Party *ourParty = Get( inviter->CurrcharObj() );
		if( ourParty == NULL )
		{
			inviter->sysmessage( "You are not in a party and cannot kick them out" );
			return;
		}
		if( ourParty->Leader() != inviter->CurrcharObj() )
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
}

