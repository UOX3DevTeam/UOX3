//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef ip4util_hpp
#define ip4util_hpp

#include <cstdint>
#include <iostream>
#include <filesystem>
#include <string>

#include "netutil.hpp"
//======================================================================
namespace util {
    namespace net {
        
        /**
         A light wrapper around an IP entry (or mask, using wildcards.)
         
         */
        class IP4Entry {
            std::array<std::string,4> entry ;
            
            //===========================================================================
            /**
             Internal routine to load an ip from a string value
             - Parameters:
             - value: The ip value in string format
             - Throws: If the string format is not using wildcard or valid numbers will throw a runtime error
             */
            auto load(const std::string &value) -> void ;
            //===========================================================================
            /**
             Internal routine to load an ip from an int format
             - Parameters:
             - value: The ip value in int format
             */
            auto load(ip4_t value) ->void ;
        public:
            
            //===========================================================================
            /**
             Default constructor, loads a wildcard ip that matches all
             */
            IP4Entry() ;
            //===========================================================================
            /**
             Constructor, loads an ip from a string value
             */
            IP4Entry(const std::string &value);
            //===========================================================================
            /**
             Constructor, loads an ip from an int value
             */
            IP4Entry(ip4_t value) ;
            
            //===========================================================================
            /**
             Static method, that will convert an int ip value to its string format.
             Useful when needing to print out an ip
             - Parameters:
             - ip: Int value of the desired ip
             - Returns: String version of the ip
             */
            static auto describeIP(ip4_t ip)  -> std::string ;
            //===========================================================================
            /**
             Static method, that will convert an string ip value to its int format.
             Useful when needing to print out an ip
             - Parameters:
             - ip: Int value of the desired ip
             - Returns: String version of the ip
             */
            static auto describeIP(const std::string &ip)  -> ip4_t ;
            
            //===========================================================================
            /**
             Provides the string representation of the entry ip
             - Returns: String version of the ip
             */
            auto describe() const -> std::string ;
            //===========================================================================
            /**
             Provides the integer representation of the entry ip.
             - Returns: Int version of the entry
             - Throws: If the entry has wildcards, it isn't a "valid" ip for integer representations,and will throw an error
             */
            [[maybe_unused]] auto  ipValue() const -> ip4_t ;
            
            //===========================================================================
            /**
             Allows access (string representation) of the various components of the ip entry,
             starting from the most signficant. Valid index ranges from 0 to 3.
             - Returns: The string representation of the ip component.
             */
            auto operator[](std::size_t index) const -> const std::string& ;
            //===========================================================================
            /**
             Compares the entry to the provided ip, and determines how many components match (starting from the most signficant).
             Will stop comparing after the first non matching component.
             - Parameters:
             - ip: The integer ip to compare to
             - Returns: The level of match (0 through 4)
             */
            auto matchLevel(ip4_t ip) const -> int ;
            //===========================================================================
            /**
             Compares the entry to the provided ip, and determines how many components match (starting from the most signficant).
             Will stop comparing after the first non matching component.
             - Parameters:
             - ip: The string ip to compare to
             - Returns: The level of match (0 through 4)
             */
            auto matchLevel(const std::string &ip) const -> int ;
            
            //===========================================================================
            /**
             Sets the entry to the ip specified
             - Parameters:
             - value: The int ip to set the entry to
             - Returns: Reference to the entry that has been set
             */
            auto operator=(ip4_t value) -> IP4Entry& ;
            //===========================================================================
            /**
             Sets the entry to the ip specified
             - Parameters:
             - value: The string version of ip to set the entry to
             - Returns: Reference to the entry that has been set
             */
            auto operator=(const std::string &value) -> IP4Entry&;
            
            //===========================================================================
            /**
             Compares to the entry to the value(ip). If all components match (matchlevel of 4, returns true)
             - Parameters:
             - value: The int ip to compare the entry to
             - Returns: True if all components match
             */
            auto operator==(ip4_t value) const ->bool ;
            //===========================================================================
            /**
             Compares to the entry to the value(ip). If all components match (matchlevel of 4, returns true)
             - Parameters:
             - value: The string ip to compare the entry to
             - Returns: True if all components match
             */
            auto operator==(const std::string &value) const ->bool;
            //===========================================================================
            /**
             Compares to the entry to the value(ip). If all components match (matchlevel of 4, returns true)
             - Parameters:
             - value: The entry ip to compare the entry to
             - Returns: True if all components match
             */
            auto operator==(const IP4Entry &value) const ->bool;
            //===========================================================================
            /**
             Does an component by component string compare of each entry
             - Parameters:
             - value: The entry ip to compare the entry to
             - Returns: True if all components match
             */
            auto equal(const IP4Entry &value) const ->bool ;
            
            //===========================================================================
            /**
             Compares an ip to the entry for match.
             - Parameters:
             - value: An int ip to compare the entry to
             - Returns: True if the ip does not match (matchlevel 4)
             */
            auto operator!=(ip4_t value) const ->bool ;
            //===========================================================================
            /**
             Compares an ip to the entry for match.
             - Parameters:
             - value: An string ip to compare the entry to
             - Returns: True if the ip does not match (matchlevel 4)
             */
            auto operator!=(const std::string &value) const ->bool;
            //===========================================================================
            /**
             Compares an ip to the entry for match.
             - Parameters:
             - value: An entry ip to compare the entry to
             - Returns: True if the ip does not match (matchlevel 4)
             */
            auto operator!=(const IP4Entry &value) const ->bool ;
            
        };
        //==========================================================================
        // IP4List
        //==========================================================================
        //===========================================================================
        /**
         ip4list is intended to be used as a allow/deny list of ips (or ip wildcards).
         One can load a list of ips from a file (format: x.x.x.x or wildcard "x.\*.\*.\*"
         Comments in the file are indicated with "//"
         One can then verify if an ip is in this list (or matches a wildcard) with the
         contain() method.
         */
        class IP4List {
        private:
            std::vector<IP4Entry> entries ;
        public:
            //===========================================================================
            /**
             Default constructor, empty list
             */
            IP4List() = default ;
            //===========================================================================
            /**
             Loads an ip list file on construction. If unable to open file, generates
             an empty list (no error is indicated).
             - Parameters:
             - filepath: The file to load
             */
            IP4List(const std::filesystem::path &filepath);
            
            //===========================================================================
            /**
             Loads an ip list file. If unable to open file, generates
             an empty list (no error is indicated).
             - Parameters:
             - filepath: The file to load
             */
            auto load(const std::filesystem::path &filepath) ->void ;
            
            //===========================================================================
            /**
             Clears the list
             */
            auto clear()  -> void;
            //===========================================================================
            /**
             Returns the size of the list
             - Returns: The number of entries in the list
             */
            auto size() const -> std::size_t ;
            //===========================================================================
            /**
             Determines if the list is empty
             - Returns: true if the list is empty
             */
            auto empty() const -> bool ;
            
            //===========================================================================
            /**
             Allows access to an individual list entry
             - Parameters:
             - index: The index into the list
             - Returns: Reference to the list entry
             - Throws: If the range of list is exceeded, throws an out of range.
             */
            auto operator[](std::size_t index) const -> const IP4Entry& ;
            //===========================================================================
            /**
             Allows access to an individual list entry
             - Parameters:
             - index: The index into the list
             - Returns: Reference to the list entry
             - Throws: If the range of list is exceeded, throws an out of range.
             */
            auto operator[](std::size_t index) -> IP4Entry& ;
            
            //===========================================================================
            /**
             Compares the value to the list, if a matchlevel 4 is acheived , returns true
             - Parameters:
             - value: The ip to compare
             - Returns: true if the ip is in the list
             */
            auto contains(ip4_t value) const ->bool ;
            //===========================================================================
            /**
             Compares the value to the list, if a matchlevel 4 is acheived , returns true
             - Parameters:
             - value: The ip to compare
             - Returns: true if the ip is in the list
             */
            auto contains(const std::string &value) const ->bool ;
            
        };
        
        //==========================================================================
        // AllowDeny
        //==========================================================================
        //===========================================================================
        /**
         AllowDeny is a combintion firewall consisting of an allow and deny list
         If a entry is on the allow and only on the allow list it is allowed.
         If an entry is on the deny list, it is denied.
         */
        class AllowDeny {
            IP4List allowList ;
            IP4List denyList ;
        public:
            AllowDeny() =default;
            AllowDeny(const std::filesystem::path &allowpath, const std::filesystem::path &denypath);
            auto reload(const std::filesystem::path &allowpath, const std::filesystem::path &denypath) ->void;
            auto allowIP(ip4_t ipaddress) const  -> bool ;
        };
        //=======================================================================
        // IP4Relay
        //======================================================================
        /**
         Provides the ip to relay to for any given ip.  It looks for the first
         entry that provides a matchlevel 3 (so matches x.x.x) (which is normally on the same local network)
         or provides the "public ip"
         */
        class IP4Relay {
            std::vector<IP4Entry> myips;
            ip4_t publicIP ;
            
        public:
            //===========================================================================
            /**
             Default constructor, defaults public ip to 127.0.0.1
             Also scans all adaptors for valid ips on the system, to use.
             */
            IP4Relay() ;
            //===========================================================================
            /**
             Constructor, sets public ip to the supplied ip
             Also scans all adaptors for valid ips on the system, to use.
             - Parameters:
             - ip: The public ip to use
             */
            IP4Relay(const std::string &ip) ;
            
            //===========================================================================
            /**
             Sets the public ip to be used in relay
             - Parameters:
             - publicIP: the public ip
             */
            auto setPublicIP(const std::string &publicIP) ->void ;
            //===========================================================================
            /**
             Returns the ip to relay to for the given ip
             - Parameters:
             - ipvalue: ip to consider what to relay to.
             - Returns: The ip of an available adaptor, or the public ip.
             */
            auto relayFor(ip4_t ipvalue) const -> ip4_t ;
            
        };
        
        
    }
}

#endif /* ip4util_hpp */
