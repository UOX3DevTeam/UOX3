// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/19/21

#include "DataCategory.hpp"
#include "StringUtility.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <fstream>

//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for DataTypeStorage
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++
const  std::map<DataCategory::Category,std::string> DataCategory::directories = {
	{item,"items"},
	{npc,"npc"},
	{create,"create"},
	{region,"regions"},
	{misc,"misc"},
	{skill,"skills"},
	{location,"location"},
	{menu,"menus"},
	{spell,"spells"},
	{newbie,"newbie"},
	{title,"titles"},
	{advance,"advance"},
	{house,"house"},
	{color,"colors"},
	{spawn,"spawn"},
	{html,"html"},
	{race,"race"},
	{weather,"weather"},
	{command,"command"},
	{msgboard,"msgboard"},
	{carve,"carve"},
	{creature,"creatures"},
	{map,"maps"},
	{DataCategory::Category::config,"config"}
};
const std::map<std::string,DataCategory::Category> DataCategory::catmapping ={
	{"item",DataCategory::Category::item},
	{ "npc",DataCategory::Category::npc},
	{"create", DataCategory::Category::create},
	{"region", DataCategory::Category::region},
	{"misc", DataCategory::Category::misc},
	{"skill", DataCategory::Category::skill},
	{"newbie", DataCategory::Category::newbie},
	{"title", DataCategory::Category::title},
	{"advance", DataCategory::Category::advance},
	{"house", DataCategory::Category::house},
	{"color", DataCategory::Category::color},
	{"spawn", DataCategory::Category::spawn},
	{"html", DataCategory::Category::html},
	{"race", DataCategory::Category::race},
	{"weather", DataCategory::Category::weather},
	{"command", DataCategory::Category::command},
	{"msgboard", DataCategory::Category::msgboard},
	{"carve", DataCategory::Category::carve},
	{"creature", DataCategory::Category::creature},
	{"map", DataCategory::Category::map},
	{"config", DataCategory::Category::config}
};



//=============================================================================
std::string DataCategory::retrieveSectionName(const std::string& data){
	auto start = data.find("[");
	auto end = data.find("]");
	if ((start== std::string::npos) || (end == std::string::npos)){
		throw std::runtime_error(std::string("Invalid section header: ")+data);
	}
	return strutil::trim(data.substr(start+1,end-start-1));
}

//=============================================================================
std::shared_ptr<KeyValue> DataCategory::processLine(const std::string line,const std::string &filepath, int linenumber){
	std::shared_ptr<KeyValue> rvalue=nullptr ;
	if (!line.empty()){
		rvalue = std::make_shared<KeyValue>();
		rvalue->original_line = line ;
		rvalue->fileorigin=filepath ;
		rvalue->linenumber = linenumber ;
		auto parsed = parseKeyValue(line);
		rvalue->key = std::get<0>(parsed) ;
		rvalue->upper_key = strutil::toupper(rvalue->key);
		rvalue->enum_key = KeyValue::lookupKeyEnum(rvalue->upper_key);
		rvalue->text = std::get<1>(parsed) ;
		rvalue->upper_text = strutil::toupper(rvalue->text);
		
		// we are going to try to get nummbers since they are common used
		auto pair = strutil::split(rvalue->text," ");
		try {
			rvalue->first_number = std::stoi(std::get<0>(pair),nullptr,0);
			rvalue->second_number = rvalue->first_number ;
			rvalue->second_number = std::stoi(std::get<1>(pair),nullptr,0);
		}
		catch(...){
			// do nothing, just keep going
		}

	}
	
	return rvalue ;
}
//=============================================================================
std::map<std::string,int> DataCategory::retreivePostFiles(){
	auto filepath = _basedir / std::filesystem::path("priority.nfo") ;
	std::map<std::string,int> rvalue ;
	if (!std::filesystem::exists(filepath)) {
		return rvalue ;
	}
	// We have a priority file
	auto input = std::ifstream(filepath.string(),std::ios::in) ;
	if (!input.is_open()){
		return rvalue ;
	}
	char buffer[2049];
	int priority = 0 ;
	while (!input.eof() && !input.fail()){
		input.getline(buffer,2048);
		if (input.gcount() > 0) {
			buffer[input.gcount()] = 0 ;
			auto data = strutil::trim(buffer);
			if (!data.empty()) {
				auto filename = strutil::toupper(data) ;
				rvalue.insert_or_assign(filename, priority);
			}
			
		}
	}
	input.close();
	return rvalue ;
}

//=============================================================================
std::tuple<std::string,std::string> DataCategory::parseKeyValue(const std::string& data,const std::string &sep) {
	std::string key ;
	std::string value ;
	key = data ;
	auto loc = data.find(sep) ;
	if (loc != std::string::npos) {
		key = strutil::trim(data.substr(0,loc));
		if (loc+1 < data.size()) {
			value = strutil::trim(data.substr(loc+1)) ;
		}
		
	}
	return std::make_tuple(key,value);
}


//=============================================================================
void  DataCategory::loadFile(const std::string& filepath){
	
	int linenumber = 0;
	std::string section_name ;
	std::shared_ptr<DataSection> data_section = nullptr ;
	bool sectionheader = false ;
	bool insection = false ;
	auto input = std::ifstream(filepath);
	if (!input.is_open()){
		return  ;
	}
	char buffer[2049];
	while (!input.eof() && !input.fail()){
		input.getline(buffer,2048);
		if (input.gcount()>0) {
			buffer[input.gcount()]=0;
			auto line = std::string(buffer) ;
			strutil::stripTrim(line);
			if (!line.empty()) {
				if (line[0] == '[')  {
					if (insection || sectionheader) {
						throw std::runtime_error(std::string("Section header found while in section or after sectionheader '")+section_name+std::string("' at line ")+std::to_string(linenumber)+std::string(" in file: ")+filepath);
					}
					try {
						section_name = retrieveSectionName(line);
					}
					catch(const std::runtime_error &e) {
						throw std::runtime_error(std::string("Invalid header found at line: ")+std::to_string(linenumber)+ std::string(" in file: ")+filepath+std::string(" header was: ")+line);
					}
					sectionheader = true ;
					
				}
				else if (line[0] == '{') {
					if (!sectionheader) {
						throw std::runtime_error(std::string("Section start found before section header at line: ") +std::to_string(linenumber) + std::string(" in file: ")+filepath);
					}
					insection = true ;
					data_section = std::make_shared<DataSection>(section_name,filepath,linenumber);
				}
				else if (line[0] == '}') {
					if (!sectionheader || (data_section == nullptr)){
						throw std::runtime_error(std::string("Section end found before section header at line: ") +std::to_string(linenumber) + std::string(" in file: ")+filepath);
						
					}
					_sections.insert_or_assign(strutil::toupper(data_section->name()), data_section);
					data_section = nullptr;
					sectionheader = false ;
					insection = false ;
					
				}
				else {
					if (insection && sectionheader && (data_section != nullptr)) {
						// process the data
						auto value = processLine(line,filepath,linenumber);
						if (value != nullptr) {
							value->fileorigin=filepath ;
							value->linenumber = linenumber ;
							data_section->addKeyValue(value);
						}
					}
				}
			}
			
		}
		linenumber++;
	}
	if (insection){
		throw std::runtime_error(std::string("End of file before closing section found: ")+filepath);
	}
	
}

//=============================================================================
std::shared_ptr<DataSection> DataCategory::section(const std::string& name) const{
	auto iter = _sections.find(strutil::toupper(name));
	if (iter != _sections.end()) {
		return iter->second;
	}
	return nullptr ;
}

//=============================================================================
std::shared_ptr<DataSection> DataCategory::containsName(const std::string& name){
	for (auto &iter : _sections) {
		if (iter.first.find(strutil::toupper(name))!= std::string::npos) {
			return iter.second;
		}
	}
	return nullptr;
}


//=============================================================================
void DataCategory::loadData(const std::string &directory, DataCategory::Category category,const std::string &extension){
	_sections.clear();
	auto path = std::filesystem::path(directory) ;
	_basedir = path ;
	auto cat = directories.find(category);
	if (cat == directories.end()) {
		throw std::runtime_error(std::string("No subdirectory defined for category number: ")+std::to_string(category));
	}
	auto subdir = cat->second ;
	if (!subdir.empty()) {
		_basedir = path / std::filesystem::path(subdir) ;
	}
	if (!std::filesystem::exists(_basedir)) {
		throw std::runtime_error(std::string("Data category directory does not exist: ")+_basedir.string());
	}
	auto priorities = retreivePostFiles() ;
	// recursive all subdirectories and files
	auto postfiles = std::vector<std::filesystem::path>() ;
	for (auto &file : std::filesystem::recursive_directory_iterator(_basedir)) {
		auto tempname = file.path().string();
		if (std::filesystem::is_regular_file(file.path())) {
			if (file.path().extension() == extension) {
				// One we may be intersted in
				auto filename = strutil::toupper(file.path().filename().string());
				auto iter = priorities.find(filename) ;
				if (iter!= priorities.end()) {
					postfiles.push_back(file.path());
				}
				else {
					// Process the file
					loadFile(file.path().string());
					
				}
			}
		}
	}
	for (auto &path : postfiles) {
		loadFile(path.string());
	}
	
}

//=============================================================================
std::size_t DataCategory::size() const  {
	return _sections.size();
}
//=============================================================================
std::map<std::string,std::shared_ptr<DataSection> > DataCategory::sections()  {
	return _sections;
}

//=============================================================================
DataCategory::Category DataCategory::category() const {
	return _mycategory;
}
//=============================================================================
void DataCategory::setCategory(DataCategory::Category value) {
	_mycategory = value ;
}

//=============================================================================
DataCategory::DataCategory(){
	_mycategory = invalid ;
}

//=============================================================================
DataCategory::DataCategory(const std::string &filepath): DataCategory(){
	loadFile(filepath);
}
//============================================================================
void DataCategory::clear() {
	_sections.clear();
}
