#ifndef __OBJECTFACTORY_H__
#define __OBJECTFACTORY_H__

#include "Prerequisites.h"
#include "Singleton.h"

namespace UOX
{

/** This class is responsible for the creation and destruction of ingame
	objects and should mean that we can take chars[] and items[] out of scope
*/
	#define OBJFUNCTOR( text )		bool (*text)( cBaseObject *, UI32 &, void * )

	class ObjectFactory : public Singleton< ObjectFactory >
	{
	protected:
		typedef std::multimap< SERIAL, cBaseObject *>	OBJECTMAP;
		typedef OBJECTMAP::iterator						OBJECTMAP_ITERATOR;
		typedef OBJECTMAP::const_iterator				OBJECTMAP_CITERATOR;

		OBJECTMAP		chars;				// This will split into PCs and NPCs when the object tree expands
		OBJECTMAP		multis;
		OBJECTMAP		items;
		SERIAL			nextPC, nextNPC, nextItem, nextMulti;

		SERIAL			NextFreeSerial( ObjectType toFind );
	public:
						ObjectFactory();
						~ObjectFactory();

		size_t			CountOfObjects( ObjectType toCount );
		size_t			SizeOfObjects( ObjectType toCount );
		cBaseObject *	CreateObject( ObjectType createType );
		cBaseObject *	CreateBlankObject( ObjectType createType );
		cBaseObject *	FindObject( SERIAL toFind );
		bool			DestroyObject( cBaseObject *toDestroy );
		bool			RegisterObject( cBaseObject *toRegister );
		bool			RegisterObject( cBaseObject *toRegister, SERIAL toAttach );
		bool			UnregisterObject( cBaseObject *toRemove );
		void			GarbageCollect( void );

		UI32			IterateOver( ObjectType toIterOver, UI32 &b, void *extraData, OBJFUNCTOR( text ) );

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
        static ObjectFactory& getSingleton( void );
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
        static ObjectFactory * getSingletonPtr( void );
	};

}

#endif