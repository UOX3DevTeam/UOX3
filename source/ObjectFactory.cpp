// This class is responsible for the creation and destruction of ingame objects

#include "ObjectFactory.h"

#include "cChar.h" // Includes CChar
#include "cItem.h" // Includes CItem, CSpawnItem
#include "cMultiObj.h" // Includes CMultiObj, CBoatObj

#include <iostream>
#include <algorithm>
#include <climits>

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen_st::SerialGen_st()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor
//o------------------------------------------------------------------------------------------------o
ObjectFactory::SerialGen_st::SerialGen_st( UI32 initial ) :serialNumber( initial )
{	
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen_st::Next()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return what next serial will be
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen_st::Next() -> UI32
{
	return serialNumber++;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen_st::RegisterSerial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register next serial number in series
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen_st::RegisterSerial(UI32 serial) -> void
{
	if( serial >= serialNumber )
	{
		serialNumber = serial + 1;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen_st::UnregisterSerial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Free up specified serial number if it was the last one registered
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen_st::UnregisterSerial( UI32 serial ) -> void
{
	if( serialNumber == ( serial + 1 ))
	{
		serialNumber = serial;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen_st::operator = ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Override operators
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen_st::operator = ( UI32 value ) ->SerialGen_st&
{
	serialNumber = value;
	return *this;
}
auto ObjectFactory::SerialGen_st::operator = ( SI32 value ) ->SerialGen_st&
{
	serialNumber = static_cast<UI32>( value );
	return *this;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::ObjectFactory()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor
//|
//| Notes		-	Starting Characters with a serial of 1, rather than 0. 
//|					The UO Client doesn't always respond well to a serial of 0
//o------------------------------------------------------------------------------------------------o
ObjectFactory::ObjectFactory():item_serials( BASEITEMSERIAL ), character_serials( 1 )
{	
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::GetSingleton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates and returns singleton instance for object factory
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::GetSingleton() -> ObjectFactory&
{
	static ObjectFactory instance;
	return instance;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::NextSerial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the next free serial to use when creating an object of a given type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::NextSerial( ObjectType type ) -> UI32
{
	switch( type )
	{
		case OT_ITEM:
		case OT_MULTI:
		case OT_BOAT:
		case OT_SPAWNER:
			return item_serials.Next();
		case OT_CHAR:
			return character_serials.Next();
		default:
			return std::numeric_limits<UI32>::max();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CollectionForType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return collection of objects of specific type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CollectionForType( ObjectType type ) -> factory_collection*
{
	factory_collection *collection = nullptr;
	switch( type )
	{
		case OT_MULTI:
		case OT_BOAT:
			collection = &multis;
			break;
		case OT_SPAWNER:
		case OT_ITEM:
			collection = &items;
			break;
		case OT_CHAR:
			collection = &chars;
			break;
			
		default:
			break;
	}
	return collection;
	
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CollectionForType() const
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return const collection of objects of specific type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CollectionForType( ObjectType type ) const -> const factory_collection*
{
	const factory_collection *collection = nullptr;
	switch( type )
	{
		case OT_MULTI:
		case OT_BOAT:
			collection = &multis;
			break;
		case OT_SPAWNER:
		case OT_ITEM:
			collection = &items;
			break;
		case OT_CHAR:
			collection = &chars;
			break;
			
		default:
			break;
	}
	return collection;
	
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::FindCharacter()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find and return item from collection based on specified serial
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::FindCharacter( UI32 serial ) ->CBaseObject*
{
	auto iter = chars.find( serial );
	if( iter != chars.end() )
	{
		return iter->second;
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::FindItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find and return item from collection based on specified serial
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::FindItem( UI32 serial ) ->CBaseObject*
{
	// We look for items first
	CBaseObject *object = nullptr;
	auto iter = items.find( serial );
	if( iter != items.end() )
	{
		object = iter->second;
	}
	else
	{
		iter = multis.find( serial );
		if( iter!= multis.end() )
		{
			object = iter->second;
		}
	}
	return object;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::RemoveObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Erase object from collection, if found there
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::RemoveObject( UI32 serial, factory_collection *collection ) -> bool
{
	auto rValue = false;
	auto iter = collection->find( serial );
	if( iter != collection->end() )
	{
		rValue = true;
		collection->erase( iter );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::RegisterObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	We want to register an object with the world, that previously
//|					does not exist in our containers
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::RegisterObject( CBaseObject *object ) -> bool
{
	auto rValue = false;
	if( object )
	{
		rValue = this->RegisterObject( object, object->GetSerial() );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::RegisterObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register object using specified serial number
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::RegisterObject( CBaseObject *object, UI32 serial ) -> bool
{
	auto rValue = false;
	if( object )
	{
		switch( object->GetObjType() )
		{
			case OT_MULTI:
			case OT_BOAT:
				rValue = true;
				item_serials.RegisterSerial( serial );
				multis.insert_or_assign( serial, object );
				break;
			case OT_SPAWNER:
			case OT_ITEM:
				rValue = true;
				item_serials.RegisterSerial( serial );
				items.insert_or_assign( serial, object );
				break;
			case OT_CHAR:
				rValue = true;
				character_serials.RegisterSerial( serial );
				chars.insert_or_assign( serial, object );
				break;
				
			default:
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::UnregisterObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove object from collection - but don't destroy it
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::UnregisterObject( CBaseObject *object ) -> bool
{
	auto rValue = false;
	if( object )
	{
		auto collection = CollectionForType( object->GetObjType() );
		rValue = RemoveObject( object->GetSerial(), collection );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::IterateOver()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Iterate over all objects of a specified object type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::IterateOver( ObjectType type, UI32 &b, void *extra, std::function<bool( CBaseObject*, UI32 &, void * )> function ) -> UI32
{
	auto collection = CollectionForType( type );
	if( collection )
	{
		// less safe way
		for( auto [serial, object] : *collection )
		{
			if( object )
			{
				if( !function( object, b, extra ))
				{
					break;
				}
			}
		}
		return 0;
	}
	return 0xFFFFFFFF;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::DestroyObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unregister object and destroy it from memory, either as part of server shutdown,
//|					or because object has been deleted.
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::DestroyObject( CBaseObject *object ) -> bool
{
	auto rValue = false;
	if( object )
	{
		UnregisterObject( object );
		delete object;
		object = nullptr;
		rValue = true;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CreateObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a new object with a serial number, and return it back to the creator
//|					From DFNs, or a PC, by and large.
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CreateObject( ObjectType type ) -> CBaseObject *
{
	auto object = CreateBlankObject( type );
	if( object )
	{
		object->SetSerial(NextSerial( type ));
	}
	return object;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CreateBlankObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a new blank object without a serial number. Serial will be provided
//|					when loading object from worldfiles
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CreateBlankObject( ObjectType type ) ->CBaseObject *
{
	CBaseObject *object = nullptr;
	switch( type )
	{
		case OT_ITEM:
			object = new CItem();
			break;
		case OT_MULTI:
			object = new CMultiObj();
			break;
		case OT_BOAT:
			object = new CBoatObj();
			break;
		case OT_SPAWNER:
			object = new CSpawnItem();
			break;
		case OT_CHAR:
			object = new CChar();
			break;
		default:
			break;
	}
	return object;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::FindObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate and return object from provided serial
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::FindObject( UI32 serial ) -> CBaseObject *
{
	CBaseObject *object = nullptr;
	if( serial >= BASEITEMSERIAL )
	{
		object = FindItem( serial );
	}
	else
	{
		object = FindCharacter( serial );
	}
	return object;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CountOfObjects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the amount of objects that exist of a given object type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CountOfObjects( ObjectType type ) const -> UI32
{
	auto rValue = UI32( 0 );
	auto collection = CollectionForType( type );
	if( collection )
	{
		rValue = static_cast<UI32>( collection->size() );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SizeOfObjects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the size of memory allocated to given object type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SizeOfObjects( ObjectType type ) const -> size_t
{
	auto collection = CollectionForType( type );
	auto size = size_t( 0 );
	switch( type )
	{
		case OT_ITEM:
		case OT_SPAWNER:
			size = sizeof( CItem );
			break;
		case OT_MULTI:
		case OT_BOAT:
			size =  sizeof( CMultiObj );
			break;
		case OT_CHAR:
			size =   sizeof( CChar );
			break;
		default:
			break;
	}
	return collection->size() * size;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::ValidObject()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(Unused) Checks if specified object is a valid object of specified type
//o------------------------------------------------------------------------------------------------o
auto ObjectFactory::ValidObject( CBaseObject *object, ObjectType type ) -> bool
{
	auto findObject = []( CBaseObject* object, factory_collection &collect )
	{
		auto iter = std::find_if( collect.begin(), collect.end(), [object] ( std::pair<UI32, CBaseObject *> entry )
		{
			return entry.second = object;
		});
		if( iter != collect.end() )
		{
			return true;
		}
		return false;
		
	};
	auto rValue = false;
	if( type == ObjectType::OT_CHAR )
	{
		rValue = findObject( object, chars );
	}
	else if(( type == ObjectType::OT_BOAT ) || ( type == ObjectType::OT_MULTI ))
	{
		rValue = findObject( object, multis );
	}
	else if(( type == ObjectType::OT_ITEM ) || ( type == ObjectType::OT_SPAWNER ))
	{
		rValue = findObject( object, items );
	}
	else
	{
		rValue = findObject( object, chars );
		if( !rValue )
		{
			rValue = findObject( object, items );
			if( !rValue )
			{
				rValue = findObject( object, multis );
			}
		}
	}
	return rValue;
}
