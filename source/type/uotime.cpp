//

#include "uotime.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "utility/strutil.hpp"

using namespace std::string_literals ;

//======================================================================
/*
 timeDestination << "[TIME]" << '\n' << "{" << '\n';
 timeDestination << "CURRENTLIGHT=" << static_cast<std::uint16_t>(worldLightCurrentLevel()) << '\n';
 timeDestination << "DAY=" << ServerTimeDay() << '\n';
 timeDestination << "HOUR=" << static_cast<std::uint16_t>(ServerTimeHours()) << '\n';
 timeDestination << "MINUTE=" << static_cast<std::uint16_t>(ServerTimeMinutes()) << '\n';
 timeDestination << "AMPM=" << (ServerTimeAMPM() ? 1 : 0) << '\n';
 timeDestination << "MOON=" << serverMoon(0) << "," << serverMoon(1) << '\n';
 timeDestination << "}" << '\n' << '\n';
 
 
 */
//======================================================================
UOTime::UOTime(){
    std::fill(moon.begin(), moon.end(), 0);
    seconds =0 ;
    minutes = 0;
    hours = 0 ;
    days = 0;
    ampm = false ;
}
//======================================================================
auto UOTime::load(const std::filesystem::path &savepath) -> bool {
    auto input = std::ifstream(savepath.string() );
    if (!input.is_open()){
        return false ;
    }
    load(input) ;
    return true;
}
//======================================================================
auto UOTime::load(std::istream &input) -> void {
    auto buffer = std::vector<char>(4096,0) ;
    while (input.good() && !input.eof()){
        input.getline(buffer.data(),buffer.size()-1);
        if (input.gcount()>0){
            buffer[input.gcount()] = 0 ;
            std::string line = buffer.data() ;
            line = util::trim(util::strip(line,"//"));
            if (!line.empty()){
                if (line[0] != '}' && line[0] != '[' && line[0] != '}'){
                    auto [akey,value] = util::split(line,"=");
                    auto key = util::upper(akey) ;
                    if (key == "CURRENTLIGHT") {
                        this->worldLightLevel = static_cast<std::uint16_t>(std::stoul(value)) ;
                    }
                    else if (key == "DAY") {
                        this->days = static_cast<std::int16_t>(std::stoi(value)) ;
                    }
                    else if (key == "HOUR") {
                        this->hours = static_cast<std::uint8_t>(std::stoul(value)) ;
                    }
                    else if (key == "MINUTE") {
                        this->minutes = static_cast<std::uint8_t>(std::stoul(value)) ;
                    }
                    else if (key == "AMPM") {
                        this->ampm = static_cast<bool>(std::stoi(value)) ;
                    }
                    else if (key == "MOON") {
                        auto [one,two] = util::split(value,",") ;
                        moon[0] = static_cast<std::uint16_t>(std::stoul(one)) ;
                        moon[1] = static_cast<std::uint16_t>(std::stoul(two)) ;
                    }
                }
            }
        }
    }
}
//======================================================================
auto UOTime::save(const std::filesystem::path &writepath) -> bool {
    auto output = std::ofstream(writepath.string()) ;
    if (!output.is_open()){
        return false ;
    }
    save(output) ;
    return true ;
}
//======================================================================
auto UOTime::save(std::ostream &output) ->void {
    output <<"[TIME]\n{\n";
    output <<"\tCURRENTLIGHT = "<<worldLightLevel<<"\n" ;
    output <<"\tDAY = "<<days<<"\n" ;
    output <<"\tHOUR = "<<hours<<"\n" ;
    output <<"\tMINUTE = "<<minutes<<"\n" ;
    output <<"\tAMPM = "<<ampm<<"\n" ;
    output <<"\tMOON = "<< moon[0]<<","<<moon[1]<<"\n}\n";
}
//======================================================================
auto UOTime::incrementDay() ->bool {
    this->days += 1 ;
    return true ;
}
//======================================================================
auto UOTime::incrementHour() ->bool {
    auto rvalue = false ;
    hours += 1 ;
    if (hours == 12){
        if (ampm) {
            rvalue = incrementDay();
        }
        hours = 0 ;
        ampm = !ampm ;
    }
    return rvalue ;
}
//======================================================================
auto UOTime::incrementMinute() ->bool {
    auto rvalue = false ;
    minutes += 1 ;
    if (minutes%8 == 0){
        incrementMoon(0);
    }
    if (minutes%3 == 0) {
        incrementMoon(1);
    }
    if (minutes == 60){
        minutes = 0 ;
        rvalue = incrementHour();
    }
    return rvalue ;
}
//======================================================================
auto UOTime::incrementSecond() ->bool {
    auto rvalue = false ;
    seconds += 1 ;
    if (seconds == 60) {
        seconds = 0 ;
        rvalue = incrementMinute() ;
    }
    return rvalue ;
}
//======================================================================
auto UOTime::incrementMoon(int moon)->void{
    this->moon[moon] = (this->moon[moon] + 1) % 8 ;
}
