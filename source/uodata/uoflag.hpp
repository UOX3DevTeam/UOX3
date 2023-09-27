//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef uoflag_hpp
#define uoflag_hpp

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
namespace uo {
    //====================================================================
    // UOX3 compatability
    //====================================================================
    enum class flag_t {
        FLOORLEVEL=0,HOLDABLE,TRANSPARENT,TRANSLUCENT,
        WALL,DAMAGING,BLOCKING,WET,
        UNKNOWN1,SURFACE,CLIMBABLE,STACKABLE,
        WINDOW,NOSHOOT,DISPLAYA,DISPLAYAN,
        DESCRIPTION,FOLIAGE,PARTIALHUE,UNKNOWN2,
        MAP,CONTAINER,WEARABLE,LIGHT,
        ANIMATED,NODIAGONAL,UNKNOWN3,ARMOR,
        ROOF,DOOR,STAIRBACK,STAIRRIGHT
    };
    auto uobitForName(const std::string &name) -> flag_t ;
    
    
    //======================================================================
    struct Flag{
        auto test(flag_t bit) const ->bool ;
        auto set(flag_t bit, bool state) ->void ;
        //====================================================================
        constexpr static auto   BACKGROUND=0x1ull;
        constexpr static auto   WEAPON=0x2ull;
        constexpr static auto   TRANSPARENT=0x4ull;
        constexpr static auto   TRANSLUCENT=0x8ull;
        
        constexpr static auto   WALL=0x10ull;
        constexpr static auto   DAMAGING=0x20ull;
        constexpr static auto   IMPASSABLE=0x40ull;
        constexpr static auto   WET=0x80ull;
        
        constexpr static auto   BIT9=0x100ull;
        constexpr static auto   SURFACE=0x200ull;
        constexpr static auto   CLIMBABLE=0x400ull;
        constexpr static auto   STACKABLE=0x800ull;
        
        constexpr static auto   WINDOW=0x1000ull;
        constexpr static auto   NOSHOOT=0x2000ull;
        constexpr static auto   ARTICLEA=0x4000ull;
        constexpr static auto   ARTICLEAN=0x8000ull;
        
        constexpr static auto   ARTICLETHE=0x10000ull;
        constexpr static auto   FOLIAGE=0x20000ull;
        constexpr static auto   PARTIALHUE=0x40000ull;
        constexpr static auto   NOHOUSE=0x80000ull;
        
        constexpr static auto   MAP=0x100000ull;
        constexpr static auto   CONTAINER=0x200000ull;
        constexpr static auto   WEARABLE=0x400000ull;
        constexpr static auto   LIGHTSOURCE=0x800000ull;
        
        constexpr static auto   ANIMATION=0x1000000ull;
        constexpr static auto   HOVEROVER=0x2000000ull;
        constexpr static auto   NODIAGNOL=0x4000000ull;
        constexpr static auto   ARMOR=0x8000000ull;
        
        constexpr static auto   ROOF=0x10000000ull;
        constexpr static auto   DOOR=0x20000000ull;
        constexpr static auto   STAIRBACK=0x40000000ull;
        constexpr static auto   STAIRRIGHT=0x80000000ull;
        
        constexpr static auto   ALPHABLEND=0x100000000ull;
        constexpr static auto   USENEWART=0x200000000ull;
        constexpr static auto   ARTUSED=0x400000000ull;
        constexpr static auto   BIT36=0x800000000ull;
        
        constexpr static auto   NOSHADOW=0x1000000000ull;
        constexpr static auto   PIXELBLEED=0x2000000000ull;
        constexpr static auto   PLAYANIMONCE=0x4000000000ull;
        constexpr static auto   BIT40=0x8000000000ull;
        
        constexpr static auto   MULTIMOVABLE=0x10000000000ull;
        constexpr static auto   BIT42=0x20000000000ull;
        constexpr static auto   BIT43=0x40000000000ull;
        constexpr static auto   BIT44=0x80000000000ull;
        
        constexpr static auto   BIT45=0x100000000000ull;
        constexpr static auto   BIT46=0x200000000000ull;
        constexpr static auto   BIT47=0x400000000000ull;
        constexpr static auto   BIT48=0x800000000000ull;
        
        constexpr static auto   BIT49=0x1000000000000ull;
        constexpr static auto   BIT50=0x2000000000000ull;
        constexpr static auto   BIT51=0x4000000000000ull;
        constexpr static auto   BIT52=0x8000000000000ull;
        
        constexpr static auto   BIT53=0x10000000000000ull;
        constexpr static auto   BIT54=0x20000000000000ull;
        constexpr static auto   BIT55=0x40000000000000ull;
        constexpr static auto   BIT56=0x80000000000000ull;
        
        constexpr static auto   BIT57=0x100000000000000ull;
        constexpr static auto   BIT58=0x200000000000000ull;
        constexpr static auto   BIT59=0x400000000000000ull;
        constexpr static auto   BIT60=0x800000000000000ull;
        
        constexpr static auto   BIT61=0x1000000000000000ull;
        constexpr static auto   BIT62=0x2000000000000000ull;
        constexpr static auto   BIT63=0x4000000000000000ull;
        constexpr static auto   BIT64=0x8000000000000000ull;
        
        /// The names of the flag masks. Allows one to get a string for a flag mask
        static const std::unordered_map<std::string,unsigned long long> flagname_masks ;
        /// obtains the mask for a flag name
        /// Parameters:
        /// - flag name: the name of the flag
        /// - Returns: the flag mask
        static auto maskForName(const std::string &flag_name) ->unsigned long long ;
        /// obtains flag for a specifig flag bit (1-64). NOT 0 based!
        /// Parameters:
        /// - bit: the flag bit (1-64) NOT 0 based!
        /// - Returns: the flag name
        static auto flagNameForBit(int bit) ->const std::string& ;
        /// generates a header of all the flag names, useful for a title row in a csv file
        /// Parameters:
        /// - output: the stream to the text file
        /// - sep: The separator to use between the flag names
        /// - Returns: nothing
        static auto header(std::ostream &output , const std::string &sep=",")  ->void ;
        
        /// The actual value of all flags
        unsigned long long value ;
        
        /// Constructor for the flag object
        /// Parameters:
        /// - flag value: The inital value for all flags
        Flag(unsigned long long flag_value=0) ;
        /// Constructor for the flag object
        /// Parameters:
        /// - value: a text string of all flag values (1 or blank/0)
        /// - sep: the separtor used between the values ;
        Flag(const std::string &value,const std::string &sep=":") ;
        /// Constructor for the flag object
        /// Parameters:
        /// - values: vector of string values (0/1 or empty) for each flag
        Flag(const std::vector<std::string> &values);
        
        /// Determines if any of the flag bits are set in the mask
        /// Parameters:
        /// - mask: the mask to compare to
        /// - Returns: true if any flag bit is set
        auto hasFlag(unsigned long long mask) const -> bool ;
        /// Sets flag bits are set in the mask
        /// Parameters:
        /// - mask: the mask to "or" with
        /// - Returns: a reference to the flag object
        [[maybe_unused]] auto setFlag(unsigned long long mask) ->Flag& ;
        /// Sets flag bits are set in the mask
        /// Parameters:
        /// - mask: the mask to "inverted and and" with
        /// - Returns: a reference to the flag object
        [[maybe_unused]] auto clearFlag(unsigned long long mask) -> Flag&;
        /// Generates a string of flags that are set
        /// Parameters:
        /// - sep: separator to use between names, defaults to ":"
        /// - Returns: a string with the flag names that are set
        auto description(const std::string &sep=":") const ->std::string ;
        
        /// Generates on one line, a 1/empty value for each flag
        /// Individual entries are separated by the specified sep.
        /// Useful when wanting to generating csv files
        /// Parameters:
        /// - output: output stream to the text file
        /// - sep: separator to use between names, defaults to ":"
        /// - Returns: a string with the flag names that are set
        auto column(std::ostream &output , const std::string &sep=",") const ->void ;
        
        /// Allows one to set the flag object with a unsigned 64 bit value
        [[maybe_unused]] auto operator=(unsigned long long value) ->Flag& ;
        /// Allows one to set the flag object with a signed 64 bit value
        [[maybe_unused]] auto operator=(long long value) ->Flag& ;
        /// Allows one to set the flag object with a unsigned 32 bit value
        [[maybe_unused]] auto operator=(unsigned int value) ->Flag& ;
        /// Allows one to set the flag object with a signed 32 bit value
        [[maybe_unused]] auto operator=(int value) ->Flag& ;
    };
    
}
#endif /* uoflag_hpp */
