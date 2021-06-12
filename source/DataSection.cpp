// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/18/21

#include "DataSection.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "StringUtility.hpp"

//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for DataSection
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++


//===============================================================================
DataSection::DataSection(const std::string &secname,const std::string &fileorigin, int linenumber){
	
	_sectionname = secname;
	_fileorigin = fileorigin;
	_linenumber = linenumber;
	_generator = std::default_random_engine() ;
}
//===============================================================================
DataSection::~DataSection(){
	_data.clear();
}
//===============================================================================
void DataSection::addKeyValue(std::shared_ptr<KeyValue> keyvalue) {
	_data.push_back(keyvalue);

}

//===============================================================================
const std::string& DataSection::name() const {
	return _sectionname ;
}

//===============================================================================
std::size_t DataSection::size() const {
	return _data.size();
}
//===============================================================================
std::vector<std::shared_ptr<KeyValue> > DataSection::keyvalues() const {
	return _data;
}
//===============================================================================
std::shared_ptr<KeyValue> DataSection::retrieveKeyValue(std::size_t entry){
	if (entry < _data.size()){
		return _data[entry];
	}
	throw std::out_of_range(std::string("KeyValue request: ")+std::to_string(entry)+ std::string(" from section: ") + _sectionname + std::string(" exceeds total number of key/values: ")+std::to_string(_data.size()));
}

//===============================================================================
std::vector<std::shared_ptr<KeyValue>> DataSection::retrieveKey(KeyValue::KeyType key) {
	std::vector<std::shared_ptr<KeyValue> > rvalue ;
	for (auto &keyvalue : _data) {
		if (keyvalue->enum_key == key){
			rvalue.push_back(keyvalue);
		}
	}
	return rvalue ;
}
//===============================================================================
std::vector<std::shared_ptr<KeyValue>> DataSection::retrieveKey(const std::string &key){
	auto ukey = strutil::toupper(key);
	std::vector<std::shared_ptr<KeyValue> > rvalue ;
	for (auto &keyvalue : _data) {
		if (keyvalue->upper_key == key){
			rvalue.push_back(keyvalue);
		}
	}
	return rvalue ;

}

//===============================================================================
std::shared_ptr<KeyValue> DataSection::firstKey(KeyValue::KeyType key){
	auto temp = retrieveKey(key);
	if (temp.size()>0) {
		return temp[0] ;
	}
	return nullptr ;
}
//===============================================================================
std::shared_ptr<KeyValue> DataSection::firstKey(const std::string &key){
	auto temp = retrieveKey(key);
	if (temp.size()>0) {
		return temp[0] ;
	}
	return nullptr ;

}
//===============================================================================
std::shared_ptr<KeyValue> DataSection::randomKey() {
	if (_data.size() > 0) {
		std::uniform_int_distribution<std::size_t> distribution(0,_data.size()-1);
		return _data[distribution(_generator)];
	}
	return nullptr;

}

//===============================================================================
std::shared_ptr<KeyValue> DataSection::index(std::size_t value){
	if (value < _data.size()){
		return _data[value];
	}
	return nullptr;
}

//==============================================================================
std::string DataSection::description() const {
	std::stringstream output ;
	output << "[" << _sectionname << "]\n{\n";
	for (auto &keyvalue : _data) {
		output << "\t"<< keyvalue->description()<<"\n";
	}
	output << "}\n\n" ;
	return output.str();
}
