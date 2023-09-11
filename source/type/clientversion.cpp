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
auto ClientType::describe() const -> const std::string& {
    auto iter = std::find_if(TYPENAMEMAP.begin(),TYPENAMEMAP.end(),[this](const std::pair<std::string,type_t> &entry){
        return entry.second == this->type ;
    });
    if (iter != TYPENAMEMAP.end()){
        return iter->first ;
    }
    throw std::out_of_range("Unable to find name for client type value: "s + std::to_string(type));
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
// ClientVersion
//======================================================================
//======================================================================
ClientVersion::ClientVersion():sub(0),major(0),letter(0),minor(0),shortVersion(ClientVersion::DEFAULT){
    
}
//======================================================================
ClientVersion::ClientVersion(std::uint32_t version):ClientVersion(){
    this->major = static_cast<std::uint8_t>(((version>>24)&0xFF)) ;
    this->minor = static_cast<std::uint8_t>(((version>>16)&0xFF)) ;
    this->sub = static_cast<std::uint8_t>(((version>>8)&0xFF)) ;
    this->letter = static_cast<std::uint8_t>(((version)&0xFF)) ;
}
//======================================================================
ClientVersion::ClientVersion(std::uint8_t major, std::uint8_t minor, std::uint8_t sub, std::uint8_t letter):ClientVersion() {
    this->major = major;
    this->minor = minor;
    this->sub = sub;
    this->letter = letter ;
}
//======================================================================
auto ClientVersion::version() const -> std::uint32_t {
    return (static_cast<std::uint32_t>(major)<<24) | (static_cast<std::uint32_t>(minor)<<16) | (static_cast<std::uint32_t>(sub)<<8) | (static_cast<std::uint32_t>(letter));
}
//======================================================================
auto ClientVersion::setVersion(std::uint32_t version) ->void {
    this->major = static_cast<std::uint8_t>(((version>>24)&0xFF)) ;
    this->minor = static_cast<std::uint8_t>(((version>>16)&0xFF)) ;
    this->sub = static_cast<std::uint8_t>(((version>>8)&0xFF)) ;
    this->letter = static_cast<std::uint8_t>(((version)&0xFF)) ;
}
//======================================================================
auto ClientVersion::describe()const ->std::string{
    return std::to_string(static_cast<std::uint16_t>(major))+ "."s + std::to_string(static_cast<std::uint16_t>(minor))+ "."s + std::to_string(static_cast<std::uint16_t>(sub))+ "."s + std::to_string(static_cast<std::uint16_t>(letter));
}
//======================================================================
auto ClientVersion::operator==(ClientVersion::version_t shortVersion) const ->bool {
    return this->shortVersion ==shortVersion ;
}
//======================================================================
auto ClientVersion::operator!=(ClientVersion::version_t shortVersion) const ->bool {
    return !this->operator==(shortVersion);
}
//======================================================================
auto ClientVersion::operator>=(ClientVersion::version_t shortVersion) const ->bool{
    return this->operator>(shortVersion) || this->operator==(shortVersion);
}
//======================================================================
auto ClientVersion::operator>(ClientVersion::version_t shortVersion) const ->bool{
    return this->shortVersion > shortVersion;
}
//======================================================================
auto ClientVersion::operator<=(ClientVersion::version_t shortVersion) const ->bool{
    return this->operator<(shortVersion) || this->operator==(shortVersion);
}
//======================================================================
auto ClientVersion::operator<(ClientVersion::version_t shortVersion) const ->bool{
    return this->shortVersion < shortVersion;
}

