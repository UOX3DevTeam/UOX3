//

#include "clientversion.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
// ClientType
//======================================================================
//================================================================================
const std::map<std::string,ClientType::type_t> ClientType::TYPENAMEMAP {
    {"DEFAULT"s,DEFAULT},{"UNKNOWN"s,UNKNOWN},{"KRRIOS"s,KRRIOS},{"T2A"s,T2A},
    {"UO3D"s,UO3D},{"ML"s,ML},{"KR2D"s,KR2D},{"KR3D"s,KR3D},
    {"SA2D"s,SA2D},{"SA3D",SA3D},{"HS2D"s,HS2D},{"HS3D"s,HS3D},
    {"TOL2D"s,TOL2D},{"TOL3D",TOL3D}
};
//================================================================================
auto ClientType::typeForName(const std::string &name)  ->type_t {
    try {
        return TYPENAMEMAP.at(name) ;
    }
    catch(...){
        throw std::runtime_error("Client type not found: "s + name);
    }
}
//================================================================================
auto ClientType::nameForType(type_t type)->const std::string& {
    auto iter = std::find_if(TYPENAMEMAP.begin(),TYPENAMEMAP.end(),[type](const std::pair<std::string,type_t> &entry){
        return type == entry.second;
    });
    if (iter != TYPENAMEMAP.end()){
        return iter->first ;
    }
    throw std::out_of_range("Client type not found: " + std::to_string(type)) ;
}



//================================================================================
auto ClientType::operator=(const type_t type) -> ClientType& {
    this->type = type ;
    return *this ;
}

//================================================================================
auto ClientType::operator==(type_t type) const -> bool {
    return this->type == type ;
}
//================================================================================
auto ClientType::operator!=(type_t type) const ->bool {
    return !operator==(type);
}

//================================================================================
auto ClientType::operator>(type_t type) const ->bool {
    return this->type > type ;
}
//================================================================================
auto ClientType::operator>=(type_t type) const -> bool {
    return this->operator==(type) || this->operator>(type);
}
//================================================================================
auto ClientType::operator<(type_t type) const ->bool {
    return this->type < type ;
}
//================================================================================
auto ClientType::operator<=(type_t type) const -> bool {
    return this->operator==(type) || this->operator<(type);
}


//================================================================================
ClientType::ClientType() {
    type = DEFAULT ;
}
//================================================================================
ClientType::ClientType(type_t type){
    this->type = type ;
}
//======================================================================
// NClientVersion
//======================================================================
//======================================================================
NClientVersion::NClientVersion():sub(0),major(0),letter(0),minor(0){
    
}
//======================================================================
NClientVersion::NClientVersion(std::uint32_t version){
    this->major = static_cast<std::uint8_t>(((version>>24)&0xFF)) ;
    this->minor = static_cast<std::uint8_t>(((version>>16)&0xFF)) ;
    this->sub = static_cast<std::uint8_t>(((version>>8)&0xFF)) ;
    this->letter = static_cast<std::uint8_t>(((version)&0xFF)) ;
}
//======================================================================
NClientVersion::NClientVersion(std::uint8_t major, std::uint8_t minor, std::uint8_t sub, std::uint8_t letter) {
    this->major = major;
    this->minor = minor;
    this->sub = sub;
    this->letter = letter ;
}
//======================================================================
auto NClientVersion::version() const -> std::uint32_t {
    return (static_cast<std::uint32_t>(major)<<24) | (static_cast<std::uint32_t>(minor)<<16) | (static_cast<std::uint32_t>(sub)<<8) | (static_cast<std::uint32_t>(letter));
}

