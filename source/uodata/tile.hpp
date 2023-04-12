//

#ifndef tile_hpp
#define tile_hpp

#include <cstdint>
#include <string>
#include <array>
#include <bitset>
#include <istream>
#include <filesystem>
//=========================================================================================
namespace uo {
    //====================================================================================
    // flag_t
    //====================================================================================
    struct flag_t {
        const static std::array<std::string,64> flagnames ;
        std::bitset<64> flag_value ;
        flag_t(std::uint64_t value=0):flag_value(value){}
        /// Determines if bit has been set
        /// - Parameters:
        ///     - bit: The bit to check, this is 0 based
        /// - Returns: True/false depending on bit state ;
        auto test(std::size_t bit) const ->bool ;
        /// Sets a bit the the specified state
        /// - Parameters:
        ///     - bit: The bit to set, this is 0 based
        ///     - value: True/false for the bit state
        auto set(std::size_t bit,bool value) ->void ;
        /// Sets a bit
        /// - Parameters:
        ///     - bit: The bit to set, this is 0 based
        auto set(std::size_t bit) ->void ;
        /// Clears a bit
        /// - Parameters:
        ///     - bit: The bit to clear, this is 0 based
        auto clear(std::size_t bit) ->void ;
        /// Set the flag to a value (=)
        /// - Parameters:
        ///     - value: The numerical value of all bits
        auto operator=(std::uint64_t value) ->flag_t& ;
        /// Return the numeric value of the flag
        auto value() const -> std::uint64_t ;
        
        ///Load the flag value from input stream
        /// - Parameters:
        ///     - input: The binary input stream
        ///     - is64: indicates if the flag is a 32 bit or 64 bit value
        /// - Returns: Refernce to the flag\_t structure ;
        [[maybe_unused]] auto load(std::istream &input,bool is64) -> flag_t&;
    };
    //====================================================================================
    // tile_t
    //====================================================================================
    struct tile_t {
        std::string name ;
        flag_t flag ;
        tile_t() = default ;
        auto loadName(std::istream &input) ->void ;
    };
    //====================================================================================
    // landtile_t
    //====================================================================================
    struct landtile_t :public tile_t{
        std::uint16_t texture ;
        landtile_t():tile_t(),texture(0){}
        landtile_t(std::istream &input,bool is64) ;
        [[maybe_unused]] auto load(std::istream &input,bool is64) ->landtile_t&;
    };
    //====================================================================================
    // arttile_t
    //====================================================================================
    struct arttile_t :public tile_t{
        std::uint8_t weight;
        std::uint8_t quality ;  // also layer
        std::uint16_t misc_data ;
        std::uint8_t unknown2 ;
        std::uint8_t quantity ;
        std::uint16_t animid ;
        std::uint8_t unknown3;
        std::uint8_t hue ;
        std::uint16_t stacking_offset ;
        std::uint8_t height ;  // Many use this as signed, but it behaves more like unsigned

        arttile_t();
        arttile_t(std::istream &input,bool is64);
        [[maybe_unused]] auto load(std::istream &input,bool is64) ->arttile_t&;
    };

    //====================================================================================
    // General methods
    //====================================================================================
    //====================================================================================
    /// Checks the file size and returns true if the file uses 64 bit flags
    /// - Parameters:
    ///     - path: The path to the tiledata.mul file
    /// - Returns: true if 64 bit flags are used
    /// - Throws: If the file does not exist, throws a std::runtime_error 
    auto uses64BitFlag(const std::filesystem::path &path) ->bool ;
    
}
#endif /* tile_hpp */
