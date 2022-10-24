#ifndef ObjectFactory_hpp
#define ObjectFactory_hpp
#include "uox3.h"

#include <unordered_map>
#include <functional>

class CBaseObject;
using factory_collection = std::unordered_map<UI32, CBaseObject*>;

//o------------------------------------------------------------------------------------------------o
// ObjectFactory
//o------------------------------------------------------------------------------------------------o
class ObjectFactory
{
	factory_collection chars;
	factory_collection multis;
	factory_collection items;
	
	//o--------------------------------------------------------------------------------------------o
	// SerialGen_st
	//o--------------------------------------------------------------------------------------------o
	// Is is nested as we dont want to pollute the global namespace
	//o--------------------------------------------------------------------------------------------o
	struct SerialGen_st
	{
		UI32 serialNumber;
		SerialGen_st( UI32 initial = 0 );
		auto Next() -> UI32;
		auto RegisterSerial( UI32 serial ) -> void;
		// Note, this only frees up the serial number if
		// deregisterd one was the last one created
		auto UnregisterSerial( UI32 serial ) -> void;
		auto operator = ( UI32 value ) ->SerialGen_st&;
		auto operator = ( SI32 value ) ->SerialGen_st&;
	};
	SerialGen_st item_serials;
	SerialGen_st character_serials;
	
	auto NextSerial( ObjectType type ) -> UI32;
	auto RemoveObject( UI32 serial, factory_collection *collection ) -> bool;
	ObjectFactory();
	
	auto FindCharacter( UI32 serial ) -> CBaseObject*;
	auto FindItem( UI32 serial ) -> CBaseObject*;
	auto CollectionForType( ObjectType type ) -> factory_collection*;
	auto CollectionForType( ObjectType type ) const -> const factory_collection*;
public:
	ObjectFactory( const ObjectFactory& ) = delete; // remove copy constructor
	ObjectFactory( const ObjectFactory&& ) = delete; // remove move constructor
	auto operator = ( const ObjectFactory& ) -> ObjectFactory& = delete; // remove assignment
	auto operator = ( const ObjectFactory&& ) -> ObjectFactory& = delete;// remove move
	static auto GetSingleton() -> ObjectFactory&;
	
	//o--------------------------------------------------------------------------------------------o
	// The registeration is only setting the serial generation for the object type to the correct
	// value.  To ensure an object doesnt have a serial number that is greater then the next we 
	// would generate. This is used in world loads, where the serial number is supplied
	//o--------------------------------------------------------------------------------------------o
	auto RegisterObject( CBaseObject *object ) -> bool;
	auto RegisterObject( CBaseObject *object, UI32 serial ) -> bool;
	
	//o--------------------------------------------------------------------------------------------o
	// This removes an object from the collection. Note, however that the serial number is not
	// decremented
	//o--------------------------------------------------------------------------------------------o
	auto UnregisterObject( CBaseObject *object ) -> bool;
	
	//o--------------------------------------------------------------------------------------------o
	// Iterate over objects
	//o--------------------------------------------------------------------------------------------o
	auto IterateOver( ObjectType type, UI32 &b, void *extra, std::function<bool( CBaseObject*, UI32 &, void * )> function ) -> UI32;
	
	//o--------------------------------------------------------------------------------------------o
	// This removes the object from the collection, and deletes the object
	//o--------------------------------------------------------------------------------------------o
	auto DestroyObject( CBaseObject *object ) -> bool;
	
	//o--------------------------------------------------------------------------------------------o
	// Create objects, Blank object doesn't get a serial number
	//o--------------------------------------------------------------------------------------------o
	auto CreateObject( ObjectType type ) -> CBaseObject *;
	auto CreateBlankObject( ObjectType type ) -> CBaseObject *;
	
	auto FindObject( UI32 toFind ) -> CBaseObject *;
	auto ValidObject( CBaseObject *object, ObjectType type = ObjectType::OT_CBO ) -> bool;

	//o--------------------------------------------------------------------------------------------o
	// Probably should be a size_t return, but uo can only handle a uint32
	//o--------------------------------------------------------------------------------------------o
	auto CountOfObjects( ObjectType type ) const -> UI32;
	
	auto SizeOfObjects( ObjectType type ) const -> size_t;
};


#endif /* ObjectFactory_hpp */
