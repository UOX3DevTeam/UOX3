#ifndef __OBJECTFACTORY_H__
#define __OBJECTFACTORY_H__

#include "Prerequisites.h"
#include "typedefs.h"
/** This class is responsible for the creation and destruction of ingame
	objects and should mean that we can take chars[] and items[] out of scope
 */
#define OBJFUNCTOR( text )		bool (*text)( CBaseObject *, UI32 &, void * )

class ObjectFactory
{
private:
	typedef std::multimap< SERIAL, CBaseObject *>	OBJECTMAP;
	typedef OBJECTMAP::iterator						OBJECTMAP_ITERATOR;
	typedef OBJECTMAP::const_iterator				OBJECTMAP_CITERATOR;

	OBJECTMAP		chars;				// This will split into PCs and NPCs when the object tree expands
	OBJECTMAP		multis;
	OBJECTMAP		items;
	SERIAL			nextPC, nextNPC, nextItem, nextMulti;

	SERIAL			NextFreeSerial( ObjectType toFind );
	// To be a singleton, we make the constructor
	// private, and delete the copy/move constructors
	ObjectFactory();
	~ObjectFactory();
	ObjectFactory(const ObjectFactory&) = delete ;
	ObjectFactory& operator=(const ObjectFactory&) = delete ;
public:

	UI32			CountOfObjects( ObjectType toCount );
	size_t			SizeOfObjects( ObjectType toCount );
	CBaseObject *	CreateObject( ObjectType createType );
	CBaseObject *	CreateBlankObject( ObjectType createType );
	CBaseObject *	FindObject( SERIAL toFind );
	bool			DestroyObject( CBaseObject *toDestroy );
	bool			RegisterObject( CBaseObject *toRegister );
	bool			RegisterObject( CBaseObject *toRegister, SERIAL toAttach );
	bool			UnregisterObject( CBaseObject *toRemove );
	void			GarbageCollect( void );

	UI32			IterateOver( ObjectType toIterOver, UI32 &b, void *extraData, OBJFUNCTOR( text ) );

	// the way you get the instance
	static ObjectFactory& getSingleton( void );

};

#endif

