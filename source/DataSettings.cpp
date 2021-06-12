// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/22/21

#include "DataSettings.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <filesystem>
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for DataSettings
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//=============================================================================
DataSettings::DataSettings(const std::string &basepath) {
	initialize(basepath);
	
}
//=============================================================================
DataSettings& DataSettings::shared(const std::string &basepath) {
	static DataSettings instance(basepath) ;
	return instance ;
}

//=============================================================================
void DataSettings::initialize(const std::string &basepath) {
	_categories.clear();
	auto directory = basepath ;
	if (basepath.empty()){
		directory = (std::filesystem::current_path() / std::filesystem::path("Definitions")).string();
	}
	_directory = directory;
	reload();
}


//=============================================================================
DataCategory DataSettings::category(DataCategory::Category cat){
	auto iter = _categories.find(cat) ;
	if (iter != _categories.end()){
		return iter->second ;
	}
	throw std::runtime_error(std::string("Data category enum not found: ")+std::to_string(cat));
}
//=============================================================================
std::shared_ptr<DataSection> DataSettings::section( DataCategory::Category cat,const std::string &secname){
	auto tempcat  = category(cat);
	return tempcat.section(secname);
}

//=============================================================================
DataCategory DataSettings::category(DataCategory::Category cat) const {
	auto iter = _categories.find(cat) ;
	if (iter != _categories.end()) {
		return iter->second ;
	}
	throw std::out_of_range(std::string("Category not found: ")+std::to_string(cat));
}

//==============================================================================
void DataSettings::reload(DataCategory::Category cat){
	auto iter = _categories.find(cat);
	if (iter != _categories.end()){
		_categories.erase(iter);
	}
	DataCategory category ;
	category.loadData(_directory, cat);
	_categories.insert_or_assign(cat, category);

}

//==============================================================================
void DataSettings::reload() {
	for (auto &entry : DataCategory::directories) {
		reload(entry.first);
	}
}
