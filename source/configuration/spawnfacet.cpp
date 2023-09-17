//

#include "spawnfacet.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::string SpawnFacet::name = "SPAWNREGIONSFACETS";
//======================================================================
SpawnFacet::SpawnFacet(){
    this->reset();
}
//======================================================================
auto SpawnFacet::set(size_t index, bool value) ->void {
    facets.set(index,value) ;
}
//======================================================================
auto SpawnFacet::test(size_t index) const ->bool {
    return facets.test(index) ;
}
//======================================================================
auto SpawnFacet::reset() ->void {
    facets.reset();
}
//======================================================================
auto SpawnFacet::value() const -> std::uint64_t {
    return facets.to_ullong();
}
//======================================================================
auto SpawnFacet::operator=(std::uint64_t value) -> SpawnFacet& {
    facets = value ;
    return *this ;
}
//======================================================================
auto SpawnFacet::operator[](size_t index) const -> bool {
    if (index >= facets.size()){
        throw std::out_of_range("Facet index exceeded limits: "s+std::to_string(index));
    }
    return facets.test(index);
}
//======================================================================
auto SpawnFacet::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    if (key == SpawnFacet::name){
        rvalue = true ;
        facets = std::stoull(value) ;
    }
    return rvalue ;
}
