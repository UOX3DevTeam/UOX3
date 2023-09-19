//

#include "startlocation.hpp"
#include <stdexcept>

#include "utility/strutil.hpp"

using namespace std::string_literals ;

//======================================================================
//======================================================================
StartLocation::StartLocation():xloc(0),yloc(0),zloc(0),worldNumber(0),instanceID(0),clilocID(0) {
}
//======================================================================
StartLocation::StartLocation(const std::string &line):StartLocation() {
    auto values = util::parse(line,",") ;
    switch(values.size()){
        default:
        case 8:
            clilocID = static_cast<std::uint32_t>(std::stoul(values[7],nullptr,0));
            [[fallthrough]];
        case 7:
            instanceID = std::stoi(values[6],nullptr,0);
            [[fallthrough]];
        case 6:
            worldNumber = std::stoi(values[5],nullptr,0);
            [[fallthrough]];
        case 5:
            zloc = std::stoi(values[4],nullptr,0);
            [[fallthrough]];
        case 4:
            yloc = std::stoi(values[3],nullptr,0);
            [[fallthrough]];
        case 3:
            xloc = std::stoi(values[2],nullptr,0);
            [[fallthrough]];
        case 2:
            description = values[1];
            [[fallthrough]];
        case 1:
            town = values[0];
            [[fallthrough]];
        case 0:
            break;
    }
}
//======================================================================
auto StartLocation::describe() const -> std::string {
    return town + ","s + description+","s+ std::to_string(xloc) +","s + std::to_string(yloc) + ","s+std::to_string(zloc) + ","s + std::to_string(worldNumber) + ","s + std::to_string(instanceID) + ","s + std::to_string(clilocID);
}
