// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/18/21

#ifndef DataSection_hpp
#define DataSection_hpp

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <tuple>
#include <random>
#include "KeyValue.hpp"



class DataSection {
private:
	std::default_random_engine _generator;
	std::vector<std::shared_ptr<KeyValue> > _data ;
	std::string _fileorigin ;
	int _linenumber ;
	std::string _sectionname ;
	
	
protected:
public:
	DataSection(const std::string &secname,const std::string &fileorigin="", int linenumber=0);
	~DataSection() ;
	void addKeyValue(std::shared_ptr<KeyValue> keyvalue);
	const std::string& name() const ;
	std::size_t size() const ;
	std::vector<std::shared_ptr<KeyValue> > keyvalues() const ;
	std::shared_ptr<KeyValue> retrieveKeyValue(std::size_t entry);
	std::shared_ptr<KeyValue> firstKey(KeyValue::KeyType key);
	std::shared_ptr<KeyValue> firstKey(const std::string &key);
	std::shared_ptr<KeyValue> index(std::size_t value);
	std::vector<std::shared_ptr<KeyValue>> retrieveKey(KeyValue::KeyType key);
	std::vector<std::shared_ptr<KeyValue>> retrieveKey(const std::string &key);
	std::shared_ptr<KeyValue> randomKey() ;
	std::string description() const ;
	std::string origin() const  { return _fileorigin;} ;
};

#endif /* DataSection_hpp */
