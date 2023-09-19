//

#include "clientfeature.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string,ClientFeature::feature_t> ClientFeature::FEATURENAMEMAP{
    {"CHAT"s,CHAT},{"UOR"s,UOR},{"TD"s,TD},{"LBR"s,LBR},
    {"AOS"s,AOS},{"SIXCHARS"s,SIXCHARS},{"SE"s,SE},{"ML"s,ML},
    {"EIGHTAGE"s,EIGHTAGE},{"NINTHAGE"s,NINTHAGE},{"TENTHAGE"s,TENTHAGE},{"UNKNOWN1"s,UNKNOWN1},
    {"SEVENCHARS"s,EIGHTAGE},{"EXPANSION"s,NINTHAGE},{"SA"s,TENTHAGE},{"HS"s,UNKNOWN1},
    {"GOTHHOUSE"s,GOTHHOUSE},{"RUSTHOUSE"s,RUSTHOUSE},{"JUNGLEHOUSE"s,JUNGLEHOUSE},{"SHADOWHOUSE"s,SHADOWHOUSE},
    {"TOLHOUSE"s,TOLHOUSE},{"ENDLESSHOUSE"s,ENDLESSHOUSE}
};
//======================================================================
const std::string ClientFeature::name = "CLIENTFEATURES";
//======================================================================
ClientFeature::ClientFeature(std::uint32_t value) {
    featureSet = value ;
    
}
//======================================================================
ClientFeature::ClientFeature() {
    featureSet = 0 ;
    this->set(CHAT,true);
    this->set(UOR,true);
    this->set(TD,true);
    this->set(LBR,true);
    this->set(AOS,true);
    this->set(SIXCHARS,true);
    this->set(SE,true);
    this->set(ML,true);
    this->set(EXPANSION,true);
}
//======================================================================
auto ClientFeature::operator=(std::uint32_t value) -> ClientFeature&{
    featureSet = value ;
    return *this;
}
//======================================================================
auto ClientFeature::test(feature_t feature) const ->bool {
    return featureSet.test(feature) ;
}
//======================================================================
auto ClientFeature::set(feature_t feature, bool state) ->void {
    featureSet.set(feature,state) ;
}
//======================================================================
auto ClientFeature::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    if (key == ClientFeature::name){
        rvalue = true ;
        featureSet = std::stoul(value,nullptr,0) ;
    }
    return  rvalue;
}
//======================================================================
auto ClientFeature::value() const -> std::uint32_t{
    return featureSet.to_ulong();
}
