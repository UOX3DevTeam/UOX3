//

#include "serverdata.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
std::map<std::string,std::variant<std::uint8_t,std::int8_t,std::uint16_t,std::int16_t,std::uint32_t,std::int32_t,std::uint64_t,std::int64_t,std::string,bool>> ServerData::dataValues ;

//======================================================================
auto ServerData::serverName() const -> std::string  {
    return this->getValue<std::string>("SERVERNAME"s);
}
//======================================================================
auto ServerData::serverName(const std::string &value)  ->void {
    this->setValue( "SERVERNAME"s,value);
}
