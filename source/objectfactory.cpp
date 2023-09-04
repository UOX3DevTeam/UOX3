// This class is responsible for the creation and destruction of ingame objects

#include "objectfactory.h"

#include "cchar.h"     // Includes CChar
#include "citem.h"     // Includes CItem, CSpawnItem
#include "cmultiobj.h" // Includes CMultiObj, CBoatObj

#include <algorithm>
#include <climits>
#include <iostream>

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen::SerialGen()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor
// o------------------------------------------------------------------------------------------------o
ObjectFactory::SerialGen::SerialGen(std::uint32_t initial) : serialNumber(initial) {}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen::Next()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return what next serial will be
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen::Next() -> std::uint32_t { return serialNumber++; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen::RegisterSerial()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register next serial number in series
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen::RegisterSerial(std::uint32_t serial) -> void {
    if (serial >= serialNumber) {
        serialNumber = serial + 1;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen::UnregisterSerial()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Free up specified serial number if it was the last one registered
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen::UnregisterSerial(std::uint32_t serial) -> void {
    if (serialNumber == (serial + 1)) {
        serialNumber = serial;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SerialGen::operator = ()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Override operators
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SerialGen::operator=(std::uint32_t value) -> SerialGen & {
    serialNumber = value;
    return *this;
}
auto ObjectFactory::SerialGen::operator=(std::int32_t value) -> SerialGen & {
    serialNumber = static_cast<std::uint32_t>(value);
    return *this;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::ObjectFactory()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor
//|
//| Notes		-	Starting Characters with a serial of 1, rather than 0.
//|					The UO Client doesn't always respond well to a serial of 0
// o------------------------------------------------------------------------------------------------o
ObjectFactory::ObjectFactory() : item_serials(BASEITEMSERIAL), character_serials(1) {}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::shared()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates and returns singleton instance for object factory
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::shared() -> ObjectFactory & {
    static ObjectFactory instance;
    return instance;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::NextSerial()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the next free serial to use when creating an object of a given
// type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::NextSerial(CBaseObject::type_t type) -> std::uint32_t {
    switch (type) {
        case CBaseObject::OT_ITEM:
        case CBaseObject::OT_MULTI:
        case CBaseObject::OT_BOAT:
        case CBaseObject::OT_SPAWNER:
            return item_serials.Next();
        case CBaseObject::OT_CHAR:
            return character_serials.Next();
        default:
            return std::numeric_limits<std::uint32_t>::max();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CollectionForType()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return collection of objects of specific type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CollectionForType(CBaseObject::type_t type) -> factory_collection * {
    factory_collection *collection = nullptr;
    switch (type) {
        case CBaseObject::OT_MULTI:
        case CBaseObject::OT_BOAT:
            collection = &multis;
            break;
        case CBaseObject::OT_SPAWNER:
        case CBaseObject::OT_ITEM:
            collection = &items;
            break;
        case CBaseObject::OT_CHAR:
            collection = &chars;
            break;
            
        default:
            break;
    }
    return collection;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CollectionForType() const
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return const collection of objects of specific type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CollectionForType(CBaseObject::type_t type) const -> const factory_collection * {
    const factory_collection *collection = nullptr;
    switch (type) {
        case CBaseObject::OT_MULTI:
        case CBaseObject::OT_BOAT:
            collection = &multis;
            break;
        case CBaseObject::OT_SPAWNER:
        case CBaseObject::OT_ITEM:
            collection = &items;
            break;
        case CBaseObject::OT_CHAR:
            collection = &chars;
            break;
            
        default:
            break;
    }
    return collection;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::FindCharacter()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find and return item from collection based on specified serial
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::FindCharacter(std::uint32_t serial) -> CBaseObject * {
    auto iter = chars.find(serial);
    if (iter != chars.end()) {
        return iter->second;
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::FindItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find and return item from collection based on specified serial
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::FindItem(std::uint32_t serial) -> CBaseObject * {
    // We look for items first
    CBaseObject *object = nullptr;
    auto iter = items.find(serial);
    if (iter != items.end()) {
        object = iter->second;
    }
    else {
        iter = multis.find(serial);
        if (iter != multis.end()) {
            object = iter->second;
        }
    }
    return object;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::RemoveObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Erase object from collection, if found there
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::RemoveObject(std::uint32_t serial, factory_collection *collection) -> bool {
    auto rValue = false;
    auto iter = collection->find(serial);
    if (iter != collection->end()) {
        rValue = true;
        collection->erase(iter);
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::RegisterObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	We want to register an object with the world, that previously
//|					does not exist in our containers
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::RegisterObject(CBaseObject *object) -> bool {
    auto rValue = false;
    if (object) {
        rValue = this->RegisterObject(object, object->GetSerial());
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::RegisterObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register object using specified serial number
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::RegisterObject(CBaseObject *object, std::uint32_t serial) -> bool {
    auto rValue = false;
    if (object) {
        switch (object->GetObjType()) {
            case CBaseObject::OT_MULTI:
            case CBaseObject::OT_BOAT:
                rValue = true;
                item_serials.RegisterSerial(serial);
                multis.insert_or_assign(serial, object);
                break;
            case CBaseObject::OT_SPAWNER:
            case CBaseObject::OT_ITEM:
                rValue = true;
                item_serials.RegisterSerial(serial);
                items.insert_or_assign(serial, object);
                break;
            case CBaseObject::OT_CHAR:
                rValue = true;
                character_serials.RegisterSerial(serial);
                chars.insert_or_assign(serial, object);
                break;
                
            default:
                break;
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::UnregisterObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove object from collection - but don't destroy it
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::UnregisterObject(CBaseObject *object) -> bool {
    auto rValue = false;
    if (object) {
        auto collection = CollectionForType(object->GetObjType());
        rValue = RemoveObject(object->GetSerial(), collection);
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::IterateOver()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Iterate over all objects of a specified object type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::IterateOver(CBaseObject::type_t type, std::uint32_t &b, void *extra,
                                std::function<bool(CBaseObject *, std::uint32_t &, void *)> function)
-> std::uint32_t {
    auto collection = CollectionForType(type);
    if (collection) {
        // less safe way
        for (auto [serial, object] : *collection) {
            if (object) {
                if (!function(object, b, extra)) {
                    break;
                }
            }
        }
        return 0;
    }
    return 0xFFFFFFFF;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::DestroyObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unregister object and destroy it from memory, either as part of
// server shutdown, |					or because object has been deleted.
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::DestroyObject(CBaseObject *object) -> bool {
    auto rValue = false;
    if (object) {
        UnregisterObject(object);
        delete object;
        object = nullptr;
        rValue = true;
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CreateObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a new object with a serial number, and return it back to the
// creator |					From DFNs, or a PC, by and large.
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CreateObject(CBaseObject::type_t type) -> CBaseObject * {
    auto object = CreateBlankObject(type);
    if (object) {
        object->SetSerial(NextSerial(type));
    }
    return object;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CreateBlankObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a new blank object without a serial number. Serial will be
// provided |					when loading object from worldfiles
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CreateBlankObject(CBaseObject::type_t type) -> CBaseObject * {
    CBaseObject *object = nullptr;
    switch (type) {
        case CBaseObject::OT_ITEM:
            object = new CItem();
            break;
        case CBaseObject::OT_MULTI:
            object = new CMultiObj();
            break;
        case CBaseObject::OT_BOAT:
            object = new CBoatObj();
            break;
        case CBaseObject::OT_SPAWNER:
            object = new CSpawnItem();
            break;
        case CBaseObject::OT_CHAR:
            object = new CChar();
            break;
        default:
            break;
    }
    return object;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::FindObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate and return object from provided serial
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::FindObject(std::uint32_t serial) -> CBaseObject * {
    CBaseObject *object = nullptr;
    if (serial >= BASEITEMSERIAL) {
        object = FindItem(serial);
    }
    else {
        object = FindCharacter(serial);
    }
    return object;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::CountOfObjects()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the amount of objects that exist of a given object type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::CountOfObjects(CBaseObject::type_t type) const -> std::uint32_t {
    auto rValue = std::uint32_t(0);
    auto collection = CollectionForType(type);
    if (collection) {
        rValue = static_cast<std::uint32_t>(collection->size());
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::SizeOfObjects()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the size of memory allocated to given object type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::SizeOfObjects(CBaseObject::type_t type) const -> size_t {
    auto collection = CollectionForType(type);
    auto size = size_t(0);
    switch (type) {
        case CBaseObject::OT_ITEM:
        case CBaseObject::OT_SPAWNER:
            size = sizeof(CItem);
            break;
        case CBaseObject::OT_MULTI:
        case CBaseObject::OT_BOAT:
            size = sizeof(CMultiObj);
            break;
        case CBaseObject::OT_CHAR:
            size = sizeof(CChar);
            break;
        default:
            break;
    }
    return collection->size() * size;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ObjectFactory::ValidObject()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	(Unused) Checks if specified object is a valid object of specified
// type
// o------------------------------------------------------------------------------------------------o
auto ObjectFactory::ValidObject(CBaseObject *object, CBaseObject::type_t type) -> bool {
    auto findObject = [](CBaseObject *object, factory_collection &collect) {
        auto iter = std::find_if(
                                 collect.begin(), collect.end(),
                                 [object](std::pair<std::uint32_t, CBaseObject *> entry) { return entry.second = object; });
        if (iter != collect.end()) {
            return true;
        }
        return false;
    };
    auto rValue = false;
    if (type == CBaseObject::OT_CHAR) {
        rValue = findObject(object, chars);
    }
    else if ((type == CBaseObject::OT_BOAT) || (type == CBaseObject::OT_MULTI)) {
        rValue = findObject(object, multis);
    }
    else if ((type == CBaseObject::OT_ITEM) || (type == CBaseObject::OT_SPAWNER)) {
        rValue = findObject(object, items);
    }
    else {
        rValue = findObject(object, chars);
        if (!rValue) {
            rValue = findObject(object, items);
            if (!rValue) {
                rValue = findObject(object, multis);
            }
        }
    }
    return rValue;
}
