//

#include "assistantfeature.hpp"


#include <algorithm>
#include <sstream>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string,AssistantFeature::feature_t> AssistantFeature::ININAMEFEATUREMAP{
    {"AF_FILTERWEATHER"s,FILTERWEATHER},{"AF_FILTERLIGHT"s,FILTERLIGHT},{"AF_SMARTTARGET"s,SMARTTARGET},{"AF_RANGEDTARGET"s,RANGEDTARGET},
    {"AF_AUTOOPENDOORS"s,AUTOOPENDOORS},{"AF_DEQUIPONCAST"s,DEQUIPONCAST},{"AF_AUTOPOTIONEQUIP"s,AUTOPOTIONEQUIP},{"AF_POISONEDCHECKS"s,POISONEDCHECKS},
    {"AF_LOOPEDMACROS"s,LOOPEDMACROS},{"AF_USEONCEAGENT"s,USEONCEAGENT},{"AF_RESTOCKAGENT"s,RESTOCKAGENT},{"AF_SELLAGENT"s,SELLAGENT},
    {"AF_BUYAGENT"s,BUYAGENT},{"AF_POTIONHOTKEYS"s,POTIONHOTKEYS},{"AF_RANDOMTARGETS"s,RANDOMTARGETS},{"AF_CLOSESTTARGETS"s,CLOSESTTARGETS},
    {"AF_OVERHEADHEALTH"s,OVERHEADHEALTH},{"AF_AUTOLOOTAGENT"s,AUTOLOOTAGENT},{"AF_BONECUTTERAGENT"s,BONECUTTERAGENT},{"AF_JSCRIPTMACROS"s,JSCRIPTMACROS},
    {"AF_AUTOREMOUNT"s,AUTOREMOUNT},{"AF_AUTOBANDAGE"s,AUTOBANDAGE},{"AF_ENEMYTARGETSHARE"s,ENEMYTARGETSHARE},{"AF_FILTERSEASON"s,FILTERSEASON},
    {"AF_SPELLTARGETSHARE"s,SPELLTARGETSHARE},{"AF_HUMANOIDHEALTHCHECKS"s,HUMANOIDHEALTHCHECKS},{"AF_SPEECHJOURNALCHECKS"s,SPEECHJOURNALCHECKS}
};

//======================================================================
AssistantFeature::AssistantFeature() {
    this->value = 0 ;
}
//======================================================================
AssistantFeature::AssistantFeature(std::uint64_t value){
    this->value = value ;
}
//======================================================================
auto AssistantFeature::operator=(std::uint64_t value) -> AssistantFeature& {
    this->value = value ;
    return *this ;
}

//======================================================================
auto AssistantFeature::set(feature_t feature , bool state) ->void {
    if (state) {
        value |= feature ;
    }
    else {
        value &= (~feature) ;
    }
}
//======================================================================
auto AssistantFeature::test(feature_t feature) const ->bool {
    return static_cast<bool>(value & feature) ;
}
//======================================================================
auto AssistantFeature::setKeyValue(const std::string &key,const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = ININAMEFEATUREMAP.find(key) ;
    if (iter != ININAMEFEATUREMAP.end()){
        rvalue = true ;
       this->set(iter->second,static_cast<bool>(std::stoi(value,nullptr,0)));
    }
    return rvalue;
}
//======================================================================
auto AssistantFeature::describe() const ->std::string {
    auto output = std::stringstream() ;
    for (const auto &[name,feature]:ININAMEFEATUREMAP){
        output <<"\t"<<name<<" = "<< ((value&feature)!=0 ? 1:0)<<"\n";
    }
    return output.str();
}
//======================================================================
auto AssistantFeature::valueFor(const std::string &keyword) const ->std::optional<bool> {
    auto iter  = ININAMEFEATUREMAP.find(keyword) ;
    if (iter != ININAMEFEATUREMAP.end()){
        return static_cast<bool>(value & iter->second);
    }
    return {} ;
}
