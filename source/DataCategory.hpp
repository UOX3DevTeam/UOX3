// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  5/19/21

#ifndef DataCategory_hpp
#define DataCategory_hpp

#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <tuple>





#include "DataSection.hpp"



class DataCategory {
public:
	enum Category { invalid = -1, item = 0,npc,create,region,misc,skill,location,menu,spell,newbie,title,advance,house,color,spawn,html,race,weather,command,msgboard,carve,creature,map,config};
private:

	Category _mycategory ;
	std::filesystem::path _basedir ;
	std::map<std::string, std::shared_ptr<DataSection> > _sections;
	
	std::string retrieveSectionName(const std::string& data);
	std::shared_ptr<KeyValue> processLine(const std::string line,const std::string &filepath ="", int linenumber = 0);
	std::map<std::string,int> retreivePostFiles() ;
	std::tuple<std::string,std::string> parseKeyValue(const std::string& data,const std::string &sep = "=") ;

protected:
	
public:
	static const std::map<Category,std::string> directories ;
	static const std::map<std::string,Category> catmapping ;

	DataCategory();
	DataCategory(const std::string &filepath);
	void loadData(const std::string &directory, Category category,const std::string &extension=".dfn") ;
	void loadFile(const std::string& filepath);

	std::shared_ptr<DataSection> section(const std::string& name) const;
	std::shared_ptr<DataSection> containsName(const std::string& name);
	
	std::size_t size() const ;
	std::map<std::string,std::shared_ptr<DataSection> > sections() ;
	
	Category category() const ;
	void setCategory(Category value);
	void clear() ;
	
};

#endif /* DataCategory_hpp */
