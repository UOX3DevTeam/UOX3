//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef hash_hpp
#define hash_hpp

#include <cstdint>
#include <string>
#include <vector>
#include <istream>

//=========================================================================================
namespace uo {
    namespace uop{

        //=========================================================================================
        /// Apply the hash little2 algorithim on the supplied string
        /// Parameters:
        ///     -  hashstring: the string to be hashed
        ///  - Returns: The hash value
        auto hashLittle2(const std::string &hashstring) ->std::uint64_t;
        
        //=========================================================================================
        /// Apply the hash adler32 algorithim on the data
        /// Parameters:
        ///     -  ptr: ptr to the data to be hashed
        ///     -  amount: the length of the data
        ///  - Returns: The hash value
        auto hashAdler32(const std::uint8_t *ptr,std::size_t amount) ->std::uint32_t ;
        //=========================================================================================
        /// Apply the hash adler32 algorithim on the data
        /// Parameters:
        ///     -  data: vector of data to be hashed
        ///  - Returns: The hash value
        auto hashAdler32(const std::vector<std::uint8_t> &data) ->std::uint32_t;
        //=========================================================================================
        /// Apply the hash adler32 algorithim on the data
        /// Parameters:
        ///     -  input: stream containing the data to be hashed
        ///     -  amount: the length of the data
        ///  - Returns: The hash value
        auto hashAdler32(std::istream &input,std::size_t amount) ->std::uint32_t;
    }
}
#endif /* hash_hpp */
