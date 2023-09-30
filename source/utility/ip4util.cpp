//Copyright © 2023 Charles Kerr. All rights reserved.

#include "ip4util.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <optional>
#include <array>
#include <cstdint>
#include <fstream>

#include "strutil.hpp"
using namespace std::string_literals ;

//======================================================================

namespace util {
    namespace net {
        //=============================================================================
        // IP4Entry
        //=============================================================================
        auto IP4Entry::load(const std::string &value) -> void {
            std::fill(entry.begin(), entry.end(), "*"s) ;
            auto parts = parse(value, "."s);
            for(std::size_t i = 0 ; i < parts.size();i++){
                entry[i] = parts[i];
                if (entry[i].empty()){
                    entry[i] = "*";
                }
                if (entry[i]!="*"){
                    try {
#if defined(_WIN32)
#pragma warning( push )
#pragma warning( disable : 4834 )
#endif
                        // this is just trying to see if this is a number, or not
                        std::stoul(entry[i]);
#if defined(_WIN32)
#pragma warning( pop )
#endif
                        
                    }
                    catch(...) {
                        throw std::runtime_error("Invalid ip entry: "s + value);
                    }
                }
            }
        }
        //=============================================================================
        auto IP4Entry::load( ip4_t value) -> void {
            std::fill(entry.begin(), entry.end(), "*"s) ;
            for (std::size_t i=0;i<4;i++){
                entry[i] = std::to_string((value>>((3-i)*8)) & 0xFF);
            }
        }
        
        //=============================================================================
        IP4Entry::IP4Entry(){
            std::fill(entry.begin(), entry.end(), "*"s) ;
        }
        //=============================================================================
        IP4Entry::IP4Entry(const std::string &value) {
            load(value);
        }
        //=============================================================================
        IP4Entry::IP4Entry(ip4_t value) {
            load(value);
        }
        //=============================================================================
        auto IP4Entry::describeIP(ip4_t ip)  -> std::string {
            return util::format("%i.%i.%i.%i",((ip>>24)&0xFF),((ip>>16)&0xFF),((ip>>8)&0xFF),(ip&0xFF));
        }
        //=============================================================================
        auto IP4Entry::describeIP(const std::string &ip)  -> ip4_t {
            auto values = util::parse(ip, ".") ;
            ip4_t ipvalue = 0  ;
            switch(values.size()){
                default:
                case 4:
                    ipvalue |= std::stoul(values[3]) ;
                    [[ fallthrough]];
                case 3:
                    ipvalue |= (std::stoul(values[2])<<8)  ;
                    [[ fallthrough]];
                case 2:
                    ipvalue |= (std::stoul(values[1])<<16)  ;
                    [[ fallthrough]];
                    
                case 1:
                    ipvalue |= (std::stoul(values[0])<<24)  ;
                    [[ fallthrough]];
                case 0:
                    break;
            }
            return ipvalue ;
            
        }
        //=============================================================================
        auto IP4Entry::describe() const -> std::string {
            return entry[0]+"."s+entry[1]+"."s+entry[2]+"."+entry[3];
        }
        //=============================================================================
        auto IP4Entry::ipValue() const -> std::uint32_t {
            return (std::stoi(entry[0])<<24) | (std::stoi(entry[1])<<16) | (std::stoi(entry[2])<<8) | std::stoi(entry[3]);
        }
        
        //=============================================================================
        auto IP4Entry::operator[](std::size_t index) const -> const std::string&{
            return entry[index];
        }
        //=============================================================================
        auto IP4Entry::matchLevel(ip4_t ip) const -> int {
            int count = 0 ;
            for (std::size_t i=0;i<4;i++){
                if ((entry[i] != "*") && (((ip>>((3-i)*8)) & 0xFF) != std::stoi(entry[i])) ) {
                    return count;
                }
                count++;
            }
            return count ;
        }
        //=============================================================================
        auto IP4Entry::matchLevel(const std::string &ip) const -> int {
            int count = 0 ;
            auto temp = IP4Entry(ip) ;
            for (std::size_t i=0;i<4;i++){
                if ( ((entry[i]!="*") && (temp[i]!="*")) && (entry[i] != temp[i]) ){
                    return count;
                }
                count++;
            }
            return count ;
        }
        
        //=============================================================================
        auto IP4Entry::operator=(ip4_t value) -> IP4Entry& {
            load(value);
            return *this;
        }
        //=============================================================================
        auto IP4Entry::operator=(const std::string &value) -> IP4Entry&{
            load(value);
            return *this;
        }
        
        //=============================================================================
        auto IP4Entry::operator==(ip4_t value) const -> bool{
            return this->matchLevel(value)==4 ;
        }
        //=============================================================================
        auto IP4Entry::operator==(const std::string &value) const -> bool{
            return this->matchLevel(value)==4 ;
        }
        //=============================================================================
        auto IP4Entry::operator==(const IP4Entry &value) const -> bool{
            for (std::size_t i=0;i<4;i++){
                if ( (entry[i] != value[i]) && (entry[i]!="*")&& (value[i]!="*")){
                    return false ;
                }
            }
            return true ;
        }
        
        //=============================================================================
        auto IP4Entry::equal(const IP4Entry &value) const -> bool{
            for (std::size_t i=0;i<4;i++){
                if (entry[i] != value[i]){
                    return false ;
                }
            }
            return true ;
        }
        //=============================================================================
        auto IP4Entry::operator!=(ip4_t value) const -> bool{
            return !this->operator==(value) ;
        }
        //=============================================================================
        auto IP4Entry::operator!=(const std::string &value) const -> bool{
            return !this->operator==(value) ;
        }
        //=============================================================================
        auto IP4Entry::operator!=(const IP4Entry &value) const -> bool{
            return !this->operator==(value);
        }
        
        //==========================================================================
        // IP4List
        //==========================================================================
        //=======================================================================
        IP4List::IP4List(const std::filesystem::path &filepath){
            load(filepath);
        }
        
        //=======================================================================
        auto IP4List::load(const std::filesystem::path &filepath) ->void {
            entries.clear();
            auto input = std::ifstream(filepath.string()) ;
            if (input.is_open()){
                // We now read and look for entries
                auto buffer = std::vector<char>(4096,0) ;
                auto linecount = 0 ;
                while (!input.eof() && input.good()) {
                    linecount++ ;
                    input.getline(buffer.data(),buffer.size()-1) ;
                    if (input.gcount()!= 0){
                        buffer[input.gcount()]=0 ;
                        std::string line = buffer.data() ;
                        line = trim(line) ;
                        auto tline = trim(strip(line,"//")) ;
                        if (!tline.empty()){
                            try{
                                auto ipentry = IP4Entry(tline) ;
                                entries.push_back(ipentry);
                            }
                            catch(...) {
                                std::cerr <<"Ignoring line: "<< std::to_string(linecount)<<" entry: \""<<line<<"\" file: " << filepath << std::endl;
                            }
                        }
                    }
                }
            }
        }
        //=======================================================================
        auto IP4List::clear() -> void {
            entries.clear();
        }
        
        //=======================================================================
        auto IP4List::size() const -> std::size_t {
            return entries.size();
        }
        //=======================================================================
        auto IP4List::empty() const -> bool {
            return entries.empty();
        }
        
        //=======================================================================
        auto IP4List::operator[](std::size_t index) const -> const IP4Entry& {
            return entries[index];
        }
        //=======================================================================
        auto IP4List::operator[](std::size_t index) -> IP4Entry& {
            return entries[index];
        }
        
        //=======================================================================
        auto IP4List::contains(ip4_t value) const ->bool {
            for (const auto &entry:entries){
                if (entry == value) {
                    return true ;
                }
            }
            return false ;
        }
        //=======================================================================
        auto IP4List::contains(const std::string &value) const ->bool {
            for (const auto &entry:entries){
                if (entry == value) {
                    return true ;
                }
            }
            return false ;
        }
        
        //=======================================================================
        // AllowDeny
        //======================================================================
        
        //=========================================================
        AllowDeny::AllowDeny(const std::filesystem::path &allowpath, const std::filesystem::path &denypath):AllowDeny() {
            reload(allowpath,denypath);
        }
        //=========================================================
        auto AllowDeny::reload(const std::filesystem::path &allowpath, const std::filesystem::path &denypath) ->void{
            allowList.load(allowpath);
            denyList.load(denypath);
        }
        //=========================================================
        auto AllowDeny::allowIP(ip4_t ipaddress) const  -> bool {
            if ((allowList.empty() && !denyList.contains(ipaddress)) || (!allowList.empty() && allowList.contains(ipaddress)) ) {
                return true ;
            }
            return false ;
        }
        
        //=======================================================================
        // IP4Relay
        //======================================================================
        
        //=========================================================
        IP4Relay::IP4Relay():publicIP(0x7f000001) {
            for (const auto &ip : availIP4()){
                myips.push_back(IP4Entry(ip));
            }
        }
        //=========================================================
        IP4Relay::IP4Relay(const std::string &ip) : IP4Relay() {
            publicIP = IP4Entry(ip).ipValue() ;
        }
        
        //=========================================================
        auto IP4Relay::setPublicIP(const std::string &publicIP) ->void {
            this->publicIP = IP4Entry::describeIP(publicIP) ;
        }
        
        //=========================================================
        auto IP4Relay::relayFor(ip4_t ipvalue) const -> ip4_t {
            
            // Get a match that is at least 3.  Othewise send the public ip
            for (const auto &entry: myips) {
                if (entry.matchLevel(ipvalue)>=3) {
                    return entry.ipValue();
                }
            }
            return publicIP ;
        }
        
        
    }
}
