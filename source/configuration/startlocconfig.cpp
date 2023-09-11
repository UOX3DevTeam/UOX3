//

#include "startlocconfig.hpp"

#include <sstream>
#include <stdexcept>

using namespace std::string_literals ;

//======================================================================
auto StartLocConfig::describe() const -> std::string {
    auto output = std::stringstream() ;
    for (const auto &entry:startLocation){
        output <<"\t"<<keyword<<" = "<<entry.describe() << "\n";
    }
    return output.str();
}
//======================================================================
StartLocConfig::StartLocConfig(const std::string &keyword){
    this->keyword = keyword ;
}

//======================================================================
auto StartLocConfig::reset() ->void {
    startLocation.clear() ;
}
//======================================================================
auto StartLocConfig::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    if (key == keyword){
        rvalue = true ;
        this->startLocation.push_back(StartLocation(value));
    }
    return rvalue ;
}
//======================================================================
auto StartLocConfig::operator[](size_t index) const -> const StartLocation& {
    return startLocation.at(index) ;
}
//======================================================================
auto StartLocConfig::operator[](size_t index)  ->  StartLocation& {
    return startLocation.at(index) ;
}
//======================================================================
auto StartLocConfig::size() const -> size_t {
    return startLocation.size();
}
