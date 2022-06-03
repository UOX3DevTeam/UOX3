
#include "ObjectFactory.h"

#include <iostream>
#include <algorithm>
#include <climits>

#include "cBaseObject.h"
#include "cItem.h" // Includes CItem, CSpawnItem
#include "cMultiObj.h" // Includes CMultiObj, CBoatObj
#include "cChar.h" // Includes CChar


//=========================================================
// ObjectFactory::serialgen_t
//=========================================================
//=========================================================
ObjectFactory::serialgen_t::serialgen_t(std::uint32_t initial) :serial_number(initial){
	
}
//=========================================================
auto ObjectFactory::serialgen_t::next() -> std::uint32_t {
	return serial_number++;
}
//=========================================================
auto ObjectFactory::serialgen_t::registerSerial(std::uint32_t serial) ->void {
	if (serial >= serial_number) {
		serial_number = serial + 1 ;
	}
}
//=========================================================
auto ObjectFactory::serialgen_t::unregisterSerial(std::uint32_t serial) ->void {
	if (serial_number == (serial+1)){
		serial_number = serial ;
	}
}

//=========================================================
auto ObjectFactory::serialgen_t::operator=(std::uint32_t value) ->serialgen_t& {
	serial_number = value ;
	return *this;
}
//=========================================================
auto ObjectFactory::serialgen_t::operator=(std::int32_t value) ->serialgen_t& {
	serial_number = static_cast<std::uint32_t>(value) ;
	return *this;
}

//=========================================================
// ObjectFactory
//=========================================================
//=========================================================
ObjectFactory::ObjectFactory():character_serials(1),item_serials(BASEITEMSERIAL){
	
}
//=========================================================
auto ObjectFactory::getSingleton() -> ObjectFactory& {
	static ObjectFactory instance ;
	return instance ;
}
//=========================================================
auto ObjectFactory::nextSerial(ObjectType type) -> std::uint32_t {
	switch (type){
		case OT_ITEM:
		case OT_MULTI:
		case OT_BOAT:
		case OT_SPAWNER:
			return item_serials.next();
		case OT_CHAR:
			return character_serials.next();
		default:
			return std::numeric_limits<std::uint32_t>::max();
	}
}
//=========================================================
auto ObjectFactory::collectionForType(ObjectType type) -> factory_collection* {
	factory_collection *collection = nullptr ;
	switch (type) {
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
	return collection ;
	
}
//=========================================================
auto ObjectFactory::collectionForType(ObjectType type) const -> const factory_collection* {
	const factory_collection *collection = nullptr ;
	switch (type) {
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
	return collection ;
	
}
//=========================================================
auto ObjectFactory::findCharacter(std::uint32_t serial) ->CBaseObject*{
	auto iter = chars.find(serial) ;
	if (iter != chars.end()){
		return iter->second;
	}
	return nullptr ;
}

//=========================================================
auto ObjectFactory::findItem(std::uint32_t serial) ->CBaseObject*{
	// We look for items first
	CBaseObject *object = nullptr ;
	auto iter = items.find(serial) ;
	if (iter != items.end()){
		object = iter->second;
	}
	else {
		iter = multis.find(serial);
		if (iter!= multis.end()){
			object = iter->second ;
		}
	}
	return object ;
}

//=========================================================
auto ObjectFactory::removeObject(std::uint32_t serial,factory_collection *collection)->bool{
	auto rvalue = false ;
	auto iter = collection->find(serial) ;
	if (iter != collection->end()){
		rvalue = true ;
		collection->erase(iter);
	}
	return rvalue ;
}

//=========================================================
auto ObjectFactory::RegisterObject( CBaseObject *object) ->bool {
	auto rvalue = false ;
	if (object) {
		rvalue = this->RegisterObject(object, object->GetSerial());
	}
	return rvalue ;
}
//=========================================================
auto ObjectFactory::RegisterObject( CBaseObject *object, std::uint32_t serial) ->bool {
	auto rvalue = false ;
	if (object){
		switch (object->GetObjType()) {
			case OT_MULTI:
			case OT_BOAT:
				rvalue = true ;
				item_serials.registerSerial(serial);
				multis.insert_or_assign(serial, object);
				break;
			case OT_SPAWNER:
			case OT_ITEM:
				rvalue = true ;
				item_serials.registerSerial(serial);
				items.insert_or_assign(serial, object);
				break;
			case OT_CHAR:
				rvalue = true ;
				character_serials.registerSerial(serial);
				chars.insert_or_assign(serial, object);
				break;
				
			default:
				break;
		}
	}
	return rvalue ;
}
//=========================================================
auto ObjectFactory::UnregisterObject(CBaseObject *object) ->bool {
	auto rvalue = false ;
	if (object) {
		auto collection = collectionForType(object->GetObjType());
		rvalue = removeObject(object->GetSerial(), collection);
	}
	return rvalue ;
}
//=========================================================
auto ObjectFactory::IterateOver(ObjectType type, std::uint32_t &b, void *extra, std::function<bool(CBaseObject*,std::uint32_t &,void *)> function) ->std::uint32_t{
	auto collection = collectionForType(type) ;
	if (collection) {
		// less save way
		for (auto [serial,object] : *collection){
			if (object){
				if (!function(object,b,extra)){
					break;
				}
			}
		}
		// safer way, it copys the collection so changes can be made under it
		/*
		auto copy = *collection ;
		for (auto [serial,object] : copy){
			auto realobject = collection->find(serial) ;
			if (realobject != collection->end()){
				if (realobject->second){
					if (!function(realobject->second,b,extra)){
						break;
					}
				}
			}
		}
		 */
		return 0 ;
	}
	return 0xFFFFFFFF ;
}
//=========================================================
auto ObjectFactory::DestroyObject(CBaseObject *object) ->bool {
	auto rvalue = false ;
	if (object){
		UnregisterObject(object);
		delete object ;
		object = nullptr ;
		rvalue = true ;
	}
	return rvalue ;
}

//=========================================================
auto ObjectFactory::CreateObject(ObjectType type) -> CBaseObject * {
	auto object = CreateBlankObject(type);
	if (object) {
		object->SetSerial(nextSerial(type));
	}
	return object;
}
//=========================================================
auto ObjectFactory::CreateBlankObject(ObjectType type) ->CBaseObject * {
	CBaseObject *object = nullptr ;
	switch(type) {
		case OT_ITEM:
			object = new CItem() ;
			break;
		case OT_MULTI:
			object = new CMultiObj() ;
			break;
		case OT_BOAT:
			object = new CBoatObj() ;
			break;
		case OT_SPAWNER:
			object = new CSpawnItem() ;
			break;
		case OT_CHAR:
			object = new CChar() ;
			break;
		default:
			break;
	}
	return object ;
}
//=========================================================
auto ObjectFactory::FindObject( std::uint32_t serial ) -> CBaseObject * {
	CBaseObject *object = nullptr ;
	if (serial >= BASEITEMSERIAL) {
		object = findItem(serial);
	}
	else {
		object = findCharacter(serial);
	}
	return object ;
}
//=========================================================
auto ObjectFactory::CountOfObjects(ObjectType type) const -> std::uint32_t {
	auto rvalue = std::uint32_t(0) ;
	auto collection = collectionForType(type);
	if (collection){
		rvalue = static_cast<std::uint32_t>(collection->size());
	}
	return rvalue ;
}
//=========================================================
auto ObjectFactory::SizeOfObjects(ObjectType type) const ->size_t {
	auto collection = collectionForType(type);
	auto size = size_t(0) ;
	switch (type) {
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
	return collection->size() * size ;
	
}
