//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef secgroup_hpp
#define secgroup_hpp
#include "framework.h"

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <vector>
#include <string>


#include "section.hpp"
//=========================================================
class secgroup_t {
private:
	std::unordered_map<std::string,std::unordered_map<std::string,section_t> > _section_types ;
	
	auto loadFile(const std::filesystem::path &path)->bool ;
public:
	secgroup_t()=default ;
	secgroup_t(const std::string &filepath,bool resursive=true,const std::string &extension=".cfg") ;
	
	auto load(const std::string &filepath,bool resursive=true,const std::string &extension=".cfg")->bool;
	auto size() const ->size_t ;
	auto size(const std::string &type) const ->size_t ;
	
	auto contains(const std::string &type)const ->bool ;
	auto contains(const std::string &type, const std::string &identifier) const ->bool ;
	
	auto type(const std::string &type) ->std::unordered_map<std::string, section_t>* ;
	auto type(const std::string &type) const -> const std::unordered_map<std::string, section_t>* ;
	auto section(const std::string &type,const std::string &identifier) ->section_t* ;
	auto section(const std::string &type,const std::string &identifier)const ->const section_t* ;
};
#endif /* secgroup_hpp */
