//

#ifndef serverdata_hpp
#define serverdata_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <variant>

//======================================================================
class ServerData {
  
private:
    static std::map<std::string,std::variant<std::uint8_t,std::int8_t,std::uint16_t,std::int16_t,std::uint32_t,std::int32_t,std::uint64_t,std::int64_t,std::string,bool>> dataValues ;

    ServerData() ;
    template<typename T>
    auto setValue(const std::string &key, T value) ->void {
        dataValues.insert_or_assign(key,value) ;
    }
    template<typename T>
    auto getValue(const std::string &key) const -> T {
        return std::get<T>(dataValues.at(key));
    }
    
public:
    ServerData(const ServerData&) = delete ;
    auto operator=(const ServerData&)->ServerData& = delete ;
    ServerData( ServerData&&) = delete ;
    auto operator=(ServerData&&) = delete ;
    static auto shared() -> ServerData& {
        static ServerData instance ;
        return instance;
    }
    
    auto serverName() const ->std::string ;
    auto serverName(const std::string &value)  ->void ;
};

#endif /* serverdata_hpp */
