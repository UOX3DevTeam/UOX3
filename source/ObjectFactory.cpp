#include "ObjectFactory.h"
#include "uox3.h"

namespace UOX
{

	#define HASHMAX 2477 // hashmax must be a prime for maximum performce.

	inline SERIAL HashSerial( SERIAL toHash )
	{
		return toHash%HASHMAX;
	}

	/** This class is responsible for the creation and destruction of ingame
		objects and should mean that we can take chars[] and items[] out of scope
	*/

    //---------------------------------------------------------------------------------------------
    template<> ObjectFactory * Singleton< ObjectFactory >::ms_Singleton = 0;
    ObjectFactory* ObjectFactory::getSingletonPtr( void )
    {
        return ms_Singleton;
    }
    ObjectFactory& ObjectFactory::getSingleton( void )
    {  
        assert( ms_Singleton );  return ( *ms_Singleton );  
    }
    //---------------------------------------------------------------------------------------------

	// Starting Characters with a serial of 1, rather than 0. The UO Client doesn't always respond well to a serial of 0 - giwo 6/21/08
	ObjectFactory::ObjectFactory() : nextPC( 1 ), nextNPC( 1 ), nextItem( BASEITEMSERIAL ), nextMulti( BASEITEMSERIAL )
	{
	}
	ObjectFactory::~ObjectFactory()
	{
		chars.clear();
		multis.clear();
		items.clear();
	}

	size_t ObjectFactory::SizeOfObjects( ObjectType toCount )
	{
		size_t toRet = 0;
		switch( toCount )
		{
			default:
				break;
			case OT_ITEM:
			case OT_SPAWNER:
				toRet = items.size() * sizeof( CItem );
				break;
			case OT_MULTI:
			case OT_BOAT:
				toRet = multis.size() * sizeof( CMultiObj );
				break;
			case OT_CHAR:
				toRet = chars.size() * sizeof( CChar );
				break;
		}
		return toRet;
	}

	size_t ObjectFactory::CountOfObjects( ObjectType toCount )
	{
		size_t toRet = 0;
		switch( toCount )
		{
			default:
				break;
			case OT_ITEM:
			case OT_SPAWNER:
				toRet = items.size();
				break;
			case OT_MULTI:
			case OT_BOAT:
				toRet = multis.size();
				break;
			case OT_CHAR:
				toRet = chars.size();
				break;
		}
		return toRet;
	}

	//	Later on we will track in depth, but this allows us
	//	to keep an eye on the serials in use, and make sure
	//	that we assign them correctly
	SERIAL ObjectFactory::NextFreeSerial( ObjectType toFind )
	{
		// For the moment, we'll keep items and chars under a simple system, though
		// long term we probably want to break it out

		SERIAL toRet = INVALIDSERIAL;
		switch( toFind )
		{
			default:
				break;
			case OT_ITEM:
			case OT_MULTI:
			case OT_BOAT:
			case OT_SPAWNER:
				toRet = nextItem++;
				break;
			case OT_CHAR:
				toRet = nextNPC++;
				break;
		}
		return toRet;
	}

	//	We want to create a new object, with a serial number, and 
	//	return this back to the creator.  From DFNs, or a PC, by and
	//	large.
	CBaseObject *ObjectFactory::CreateObject( ObjectType createType )
	{
		CBaseObject *created = NULL;
		switch( createType )
		{
			default:													break;
			case OT_ITEM:			created	= new CItem();				break;
			case OT_MULTI:			created	= new CMultiObj();			break;
			case OT_BOAT:			created	= new CBoatObj();			break;
			case OT_SPAWNER:		created	= new CSpawnItem();			break;
			case OT_CHAR:			created	= new CChar();				break;
		}
		// assign serial here
		if( created != NULL )
			created->SetSerial( NextFreeSerial( createType ) );	// SetSerial() will register our object - giwo

		return created;
	}

	//	We want a blank object, which has NO serial number, and one we will
	//	provide because it's from loading the world!
	CBaseObject *ObjectFactory::CreateBlankObject( ObjectType createType )
	{
		CBaseObject *created = NULL;
		switch( createType )
		{
			default:													break;
			case OT_ITEM:			created	= new CItem();				break;
			case OT_MULTI:			created	= new CMultiObj();			break;
			case OT_BOAT:			created	= new CBoatObj();			break;
			case OT_SPAWNER:		created	= new CSpawnItem();			break;
			case OT_CHAR:			created	= new CChar();				break;
		}
		return created;
	}

	//	CalcObjFromSerial, basically
	CBaseObject *ObjectFactory::FindObject( SERIAL toFind )
	{
		OBJECTMAP_ITERATOR fIter, fUpper, fEnd;
		CBaseObject	*	retVal		= NULL;
		SERIAL			hashValue	= HashSerial( toFind );
		if( toFind != INVALIDSERIAL )
		{
			if( toFind >= BASEITEMSERIAL )	// either an item or multi
			{
				fIter	= items.find( hashValue );
				fUpper	= items.upper_bound( hashValue );
				fEnd	= items.end();
				while( fIter != fUpper && fIter != fEnd )
				{
					if( fIter->second->GetSerial() == toFind )	// is this our object?
					{
						retVal = fIter->second;
						break;
					}
					else
						++fIter;
				}
				if( retVal == NULL )	// Not an item!  Let's check multis
				{
					fIter	= multis.find( hashValue );
					fUpper	= multis.upper_bound( hashValue );
					fEnd	= multis.end();
					while( fIter != fUpper && fIter != fEnd )
					{
						if( fIter->second->GetSerial() == toFind )	// is this our object?
						{
							retVal = fIter->second;
							break;
						}
						else
							++fIter;
					}
				}
			}
			else	// a character of some sort
			{
				fIter	= chars.find( hashValue );
				fUpper	= chars.upper_bound( hashValue );
				fEnd	= chars.end();
				while( fIter != fUpper && fIter != fEnd )
				{
					if( fIter->second->GetSerial() == toFind )	// is this our object?
					{
						retVal = fIter->second;
						break;
					}
					else
						++fIter;
				}
			}
		}
		return retVal;
	}

	//	This function unregisters the object and destroys it from
	//	memory.  Essentially, it's either a server shutdown
	//	or the object has been deleted, and is no longer
	//	part of the world.
	bool ObjectFactory::DestroyObject( CBaseObject *toDestroy )
	{
		assert( toDestroy != NULL );
		UnregisterObject( toDestroy );
		delete toDestroy;
		toDestroy = NULL;
		return true;
	}

	//	We want to register an object with the world, that previously
	//	does not exist in our containers
	bool ObjectFactory::RegisterObject( CBaseObject *toRegister )
	{
		assert( toRegister != NULL );
		return RegisterObject( toRegister, toRegister->GetSerial() );
	}

	//	This allows us to register our object with it's attached serial number
	//	Note that we do not check if this object existed previously, so if you
	//	want to change an object's serial, you would have to Unregister it
	//	and then Register it with the new serial.  HOWEVER, that does not guarantee
	//	valid serial numbers and that it's not a duplicate!  By and large, we don't
	//	want to call this from anywhere but a world loading routine.
	bool ObjectFactory::RegisterObject( CBaseObject *toRegister, SERIAL toAttach )
	{
		assert( toRegister != NULL );
		ObjectType reg = toRegister->GetObjType();
		switch( reg )
		{
		default:
			break;
		case OT_MULTI:
		case OT_BOAT:
			if( toAttach >= nextItem )
				nextItem = toAttach + 1;
			multis.insert( std::make_pair( HashSerial( toAttach ), toRegister ) );
			break;
		case OT_ITEM:
		case OT_SPAWNER:
			if( toAttach >= nextItem )
				nextItem = toAttach + 1;
			items.insert( std::make_pair( HashSerial( toAttach ), toRegister ) );
			break;
		case OT_CHAR:
			if( toAttach >= nextNPC )
				nextNPC = toAttach + 1;
			chars.insert( std::make_pair( HashSerial( toAttach ), toRegister ) );
			break;
		}
		return true;
	}

	//	We're done with an object, or want to detach it from the world
	//	This function allows us to detach it, but not destroy it
	//	in case we want to attach it elsewise
	bool ObjectFactory::UnregisterObject( CBaseObject *toRemove )
	{
		assert( toRemove != NULL );
		OBJECTMAP_ITERATOR rIter, rUpper, rEnd;
		SERIAL hashValue = HashSerial( toRemove->GetSerial() );
		switch( toRemove->GetObjType() )
		{
		default:
			throw new std::runtime_error( "Damn, bad bad work here!" );
			break;
		case OT_MULTI:
		case OT_BOAT:
			rIter	= multis.find( hashValue );
			rUpper	= multis.upper_bound( hashValue );
			rEnd	= multis.end();
			break;
		case OT_ITEM:
		case OT_SPAWNER:
			rIter	= items.find( hashValue );
			rUpper	= items.upper_bound( hashValue );
			rEnd	= items.end();
			break;
		case OT_CHAR:
			rIter	= chars.find( hashValue );
			rUpper	= chars.upper_bound( hashValue );
			rEnd	= chars.end();
			break;
		}
		while( rIter != rUpper && rIter != rEnd )
		{
			if( rIter->second == toRemove )	// is this our object?
			{
				// let's remove it from the hash table
				switch( toRemove->GetObjType() )
				{
				case OT_MULTI:
				case OT_BOAT:		multis.erase( rIter );	break;
				case OT_ITEM:
				case OT_SPAWNER:	items.erase( rIter );	break;
				case OT_CHAR:		chars.erase( rIter );	break;
				}
				break;
			}
			else
				++rIter;
		}
		return true;
	}

	//	This is to deal with dead or weird objects, but I don't think
	//	that we would really need it.  No object should be destroyed
	//	except by this object factory anyway.  We may want to do lazy
	//	deletes and only do invalidations.
	void ObjectFactory::GarbageCollect( void )
	{
		// We may not even need to do anything here, really
	}

	UI32 ObjectFactory::IterateOver( ObjectType toIterOver, UI32 &b, void *extraData, OBJFUNCTOR( text ) )
	{
		OBJECTMAP_ITERATOR mBegin, mEnd;
		switch( toIterOver )
		{
		case OT_MULTI:
		case OT_BOAT:		
			{
				mBegin	= multis.begin();
				mEnd	= multis.end();
			}
			break;
		case OT_ITEM:
		case OT_SPAWNER:	
			{
				mBegin	= items.begin();
				mEnd	= items.end();
			}
			break;
		case OT_CHAR:
			{
				mBegin	= chars.begin();
				mEnd	= chars.end();
			}
			break;
		default:
			return 0xFFFFFFFF;
		}
		bool stillContinue = true;
		while( mBegin != mEnd && stillContinue )
		{
			stillContinue = (*text)(mBegin->second, b, extraData );
			++mBegin;
		}
		return 0;
	}
}
