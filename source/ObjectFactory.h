
#ifndef ObjectFactory_hpp
#define ObjectFactory_hpp
#include "uox3.h"

#include <unordered_map>
#include <functional>

class CBaseObject;
using factory_collection = std::unordered_map<std::uint32_t, CBaseObject*>;

//=========================================================
// ObjectFactory
//=========================================================
//=========================================================
//=========================================================
class ObjectFactory {
	factory_collection chars;
	factory_collection multis;
	factory_collection items;
	
	//=========================================================
	// serialgen_t
	//=========================================================
	// Is is nested as we dont want to pollute the global namespace
	//=========================================================
	struct serialgen_t {
		std::uint32_t serial_number ;
		serialgen_t(std::uint32_t initial=0) ;
		auto next() -> std::uint32_t ;
		auto registerSerial(std::uint32_t serial) ->void ;
		// Note, this only frees up the serial number if
		// deregisterd one was the last one created
		auto unregisterSerial(std::uint32_t serial) ->void ;
		auto operator=(std::uint32_t value) ->serialgen_t& ;
		auto operator=(std::int32_t value) ->serialgen_t& ;
	};
	serialgen_t item_serials ;
	serialgen_t character_serials ;
	
	auto nextSerial(ObjectType type) -> std::uint32_t ;
	auto removeObject(std::uint32_t serial,factory_collection *collection)->bool;
	ObjectFactory() ;
	
	auto findCharacter(std::uint32_t serial) ->CBaseObject*;
	auto findItem(std::uint32_t serial) ->CBaseObject*;
	auto collectionForType(ObjectType type) -> factory_collection* ;
	auto collectionForType(ObjectType type) const -> const factory_collection* ;
public:
	ObjectFactory(const ObjectFactory&) = delete ; // remove copy constructor
	ObjectFactory(const ObjectFactory&& ) = delete ; // remove move constructor
	auto operator=(const ObjectFactory&) ->ObjectFactory& = delete ; // remove assignment
	auto operator=(const ObjectFactory&&) ->ObjectFactory& = delete ;// remove move
	static auto getSingleton() -> ObjectFactory& ;
	
	// ********************************************************
	// The registeration is only setting the serial generation
	// for the object type to the correct value.  To enusre
	// an object doesnt have a serial number that is greater
	// then the next we would generate.  This is used
	// in world loads, where the serial number is supplied
	// ********************************************************
	auto RegisterObject(CBaseObject *object) ->bool ;
	auto RegisterObject(CBaseObject *object, std::uint32_t serial) ->bool ;
	
	// *******************************************************
	// This removes an object from the collection.  Note, however
	// that the serial number is not decremented
	auto UnregisterObject(CBaseObject *object) ->bool ;
	
	
	// *******************************************************
	// This DOES NOTHING,so please remove the reference call
	// in uox.cpp, around line 2138
	auto GarbageCollect() ->void {};
	
	// *******************************************************
	// Iterate over objects
	auto IterateOver(ObjectType type, std::uint32_t &b, void *extra, std::function<bool(CBaseObject*,std::uint32_t &,void *)> function) ->std::uint32_t;
	
	// ******************************************************
	// This removes the object from the collection, and deletes the object
	auto DestroyObject(CBaseObject *object) ->bool ;
	
	// *******************************************************
	// Create objects, Blank object doesn't get a serial number
	auto CreateObject(ObjectType type) -> CBaseObject * ;
	auto CreateBlankObject(ObjectType type) ->CBaseObject * ;
	
	auto FindObject( std::uint32_t toFind ) -> CBaseObject * ;
	
	// ******************************************************
	// Probably should be a size_t return, but uo can only handle a uint32
	auto CountOfObjects(ObjectType type) const -> std::uint32_t ;
	
	auto SizeOfObjects(ObjectType type) const ->size_t;
};


#endif /* ObjectFactory_hpp */
