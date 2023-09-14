//

#include "era.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
/*
 CORE, UO, T2A, UOR,
 TD, LBR, AOS, SE,
 ML, SA, HS, TOL
 */
const std::map<std::string, Era::era_t> Era::ERANAMEMAP{
    {"core"s,CORE},{"uo"s,UO},{"t2a"s,T2A},{"uor",UOR},
    {"td"s,TD},{"lbr"s,LBR},{"aos"s,AOS},{"se",SE},
    {"ml"s,ML},{"sa"s,SA},{"hs"s,HS},{"tol",TOL}

};
//======================================================================
auto Era::eraFromName(const std::string &name) -> era_t {
    auto iter = ERANAMEMAP.find(name) ;
    if (iter != ERANAMEMAP.end()){
        return iter->second ;
    }
    // We would normally throw, but we return the latest instead
    //throw std::runtime_error("Era does not exist: "s + name);
    return TOL ;
}
//======================================================================
auto Era::nameFromEra(era_t era) -> const std::string& {
    auto iter = std::find_if(ERANAMEMAP.begin(),ERANAMEMAP.end(),[era](const std::pair<std::string,era_t> &entry){
        return entry.second == era ;
    });
    if (iter != ERANAMEMAP.end()){
        return iter->first ;
    }
    //throw std::runtime_error("Era value does not exist: "s + std::to_string(static_cast<int>(era)));
    iter = std::find_if(ERANAMEMAP.begin(),ERANAMEMAP.end(),[](const std::pair<std::string,era_t> &entry){
        return entry.second == TOL ;
    });
    return iter->first ;
}
//======================================================================
auto Era::size()-> size_t {
    return ERANAMEMAP.size();
}
//======================================================================
auto Era::latest() -> era_t {
    return TOL ;
}
//======================================================================
Era::Era(){
    this->value = CORE;
}
//======================================================================
Era::Era(era_t era){
    this->value = era ;
}
//======================================================================
Era::Era(const std::string &eraname) {
    this->value = eraFromName(eraname);
}
//======================================================================
auto Era::describe() const -> const std::string& {
    return nameFromEra(this->value);
}
//======================================================================
auto Era::operator==(const Era &value) const ->bool {
    return value.value == this->value;
}
//======================================================================
auto Era::operator!=(const Era &value) const ->bool {
    return !this->operator==(value);
}
//======================================================================
auto Era::operator<(const Era &value) const ->bool {
    return this->value < value.value;
}
//======================================================================
auto Era::operator>(const Era &value) const ->bool {
    return this->value > value.value;
}
//======================================================================
auto Era::operator<=(const Era &value) const ->bool {
    return operator==(value) || operator<(value) ;
}
//======================================================================
auto Era::operator>=(const Era &value) const ->bool {
    return operator==(value) || operator>(value) ;
}
