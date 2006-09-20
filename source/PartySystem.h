#ifndef __PARTYFACTORY_H__
#define __PARTYFACTORY_H__

#include "Prerequisites.h"
#include "Singleton.h"
#include "CDataList.h"

namespace UOX
{
	class PartyEntry
	{
	protected:
		CChar *				member;
		std::bitset< 8 >	settings;
	public:
		CChar *				Member( void ) const;
		bool				IsLeader( void ) const;
		bool				IsLootable( void ) const;

		void				Member( CChar *valid );
		void				IsLeader( bool value );
		void				IsLootable( bool value );

							PartyEntry();
							PartyEntry( CChar *m, bool isLeader = false, bool isLootable = false );
	};

	class Party
	{
	protected:
//		CDataList< PartyEntry * >	members;
		std::vector< PartyEntry * >	members;
		CChar *						leader;
		void						SendPacket( CPUOXBuffer *toSend, CSocket *toSendTo = NULL );
	public:
		void						AddMember( CChar *i );
		void						RemoveMember( CChar *i );
		void						Leader( CChar *member );
		CChar *						Leader( void );
//		CDataList< PartyEntry * > *	MemberList( void )		{ return &members; }
		std::vector< PartyEntry * > *	MemberList( void )		{ return &members; }
		PartyEntry *				Find( CChar *find, int *location = NULL );
		bool						HasMember( CChar *find );
		void						SendList( CSocket *toSendTo = NULL );
									Party();
									Party( CChar *leader );
	};


/** This class is responsible for the creation and destruction of parties
*/
	class PartyFactory : public Singleton< PartyFactory >
	{
	friend class Party;
	protected:
		CDataList< Party * >		parties;
		std::map< SERIAL, Party * >	partyQuickLook;

		void			AddLookup( Party *toQuickLook, CChar *toSave );
		void			RemoveLookup( CChar *toRemove );
	public:
						PartyFactory();
						~PartyFactory();
		Party *			Create( CChar *leader );
		void			Destroy( CChar *member );
		void			Destroy( Party *toRemove );
		Party *			Get( CChar *member );
		void			CreateInvite( CSocket *inviter );
		void			Kick( CSocket *inviter );

        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static PartyFactory& getSingleton( void );
        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static PartyFactory * getSingletonPtr( void );
	};

}

#endif

