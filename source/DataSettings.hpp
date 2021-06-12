// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/22/21

#ifndef DataSettings_hpp
#define DataSettings_hpp

#include <cstdint>
#include <string>
#include <map>

#include "DataCategory.hpp"

class DataSettings {
private:
	std::map<DataCategory::Category,DataCategory> _categories ;
	DataSettings(const std::string &basepath = "");
	void initialize(const std::string &basepath = "") ;

	std::string _directory ;
protected:
public:
	static DataSettings& shared(const std::string &basepath = "") ;
	DataSettings( const DataSettings &) = delete;
	DataSettings& operator=(const DataSettings&) = delete ;
	
	
	DataCategory category(DataCategory::Category cat);
	std::shared_ptr<DataSection> section( DataCategory::Category cat,const std::string &secname);
	DataCategory category(DataCategory::Category cat) const ;
	void reload(DataCategory::Category cat) ;
	void reload();

};

#endif /* DataSettings_hpp */
