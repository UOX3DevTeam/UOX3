//

#ifndef startlocation_hpp
#define startlocation_hpp

#include <cstdint>
#include <iostream>
#include <string>

//======================================================================
struct StartLocation {
    std::int16_t xloc ;
    std::int16_t yloc ;
    std::int16_t zloc ;
    std::int16_t worldNumber ;
    std::int16_t instanceID ;
    std::uint32_t clilocID ;
    std::string town ;
    std::string description ;
  
    StartLocation() ;
    StartLocation(const std::string &line) ;
    auto describe() const -> std::string ;
};

#endif /* startlocation_hpp */
