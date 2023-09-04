#ifndef ObjectFactory_hpp
#define ObjectFactory_hpp

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#include "typedefs.h"
#include "cbaseobject.h"
using factory_collection = std::unordered_map<std::uint32_t, CBaseObject *>;

// o------------------------------------------------------------------------------------------------o
//  ObjectFactory
// o------------------------------------------------------------------------------------------------o
class ObjectFactory {
    factory_collection chars;
    factory_collection multis;
    factory_collection items;
    
    // o--------------------------------------------------------------------------------------------o
    //  SerialGen_st
    // o--------------------------------------------------------------------------------------------o
    //  Is is nested as we dont want to pollute the global namespace
    // o--------------------------------------------------------------------------------------------o
    struct SerialGen_st {
        std::uint32_t serialNumber;
        SerialGen_st(std::uint32_t initial = 0);
        auto Next() -> std::uint32_t;
        auto RegisterSerial(std::uint32_t serial) -> void;
        // Note, this only frees up the serial number if
        // deregisterd one was the last one created
        auto UnregisterSerial(std::uint32_t serial) -> void;
        auto operator=(std::uint32_t value) -> SerialGen_st &;
        auto operator=(std::int32_t value) -> SerialGen_st &;
    };
    SerialGen_st item_serials;
    SerialGen_st character_serials;
    
    auto NextSerial(CBaseObject::type_t type) -> std::uint32_t;
    auto RemoveObject(std::uint32_t serial, factory_collection *collection) -> bool;
    ObjectFactory();
    
    auto FindCharacter(std::uint32_t serial) -> CBaseObject *;
    auto FindItem(std::uint32_t serial) -> CBaseObject *;
    auto CollectionForType(CBaseObject::type_t type) -> factory_collection *;
    auto CollectionForType(CBaseObject::type_t type) const -> const factory_collection *;
    
public:
    ObjectFactory(const ObjectFactory &) = delete;                      // remove copy constructor
    ObjectFactory(const ObjectFactory &&) = delete;                     // remove move constructor
    auto operator=(const ObjectFactory &) -> ObjectFactory & = delete;  // remove assignment
    auto operator=(const ObjectFactory &&) -> ObjectFactory & = delete; // remove move
    static auto shared() -> ObjectFactory &;
    
    // o--------------------------------------------------------------------------------------------o
    //  The registeration is only setting the serial generation for the object type to the correct
    //  value.  To ensure an object doesnt have a serial number that is greater then the next we
    //  would generate. This is used in world loads, where the serial number is supplied
    // o--------------------------------------------------------------------------------------------o
    auto RegisterObject(CBaseObject *object) -> bool;
    auto RegisterObject(CBaseObject *object, std::uint32_t serial) -> bool;
    
    // o--------------------------------------------------------------------------------------------o
    //  This removes an object from the collection. Note, however that the serial number is not
    //  decremented
    // o--------------------------------------------------------------------------------------------o
    auto UnregisterObject(CBaseObject *object) -> bool;
    
    // o--------------------------------------------------------------------------------------------o
    //  Iterate over objects
    // o--------------------------------------------------------------------------------------------o
    auto IterateOver(CBaseObject::type_t type, std::uint32_t &b, void *extra,
                     std::function<bool(CBaseObject *, std::uint32_t &, void *)> function) -> std::uint32_t;
    
    // o--------------------------------------------------------------------------------------------o
    //  This removes the object from the collection, and deletes the object
    // o--------------------------------------------------------------------------------------------o
    auto DestroyObject(CBaseObject *object) -> bool;
    
    // o--------------------------------------------------------------------------------------------o
    //  Create objects, Blank object doesn't get a serial number
    // o--------------------------------------------------------------------------------------------o
    auto CreateObject(CBaseObject::type_t type) -> CBaseObject *;
    auto CreateBlankObject(CBaseObject::type_t type) -> CBaseObject *;
    
    auto FindObject(std::uint32_t toFind) -> CBaseObject *;
    auto ValidObject(CBaseObject *object, CBaseObject::type_t type = CBaseObject::OT_CBO) -> bool;
    
    // o--------------------------------------------------------------------------------------------o
    //  Probably should be a size_t return, but uo can only handle a uint32
    // o--------------------------------------------------------------------------------------------o
    auto CountOfObjects(CBaseObject::type_t type) const -> std::uint32_t;
    
    auto SizeOfObjects(CBaseObject::type_t type) const -> size_t;
};

#endif /* ObjectFactory_hpp */
