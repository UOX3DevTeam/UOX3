//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef filemap_hpp
#define filemap_hpp

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

namespace util {
    //=================================================================================
    class filemap_t {
        std::filesystem::path path ;
    public:
        filemap_t(const filemap_t&) = delete ;
        auto operator=(const filemap_t&)->filemap_t& = delete ;
        //=================================================================================
        filemap_t(filemap_t&&)  ;
        //=================================================================================
        auto operator=(filemap_t&& ) -> filemap_t& ;
        //=================================================================================
        /// Constructor for filemap. Makes an empty filemap
        filemap_t():ptr(nullptr),length(0){}
        
        //=================================================================================
        /// Destructor for filemap
        ~filemap_t() ;
        //=================================================================================
        /// Constructor for filemap. Memory map the specified file, and the variables
        /// ptr and length will be set
        /// - Parameters:
        ///     - filepath: The filepath to the file
        /// - Returns: Nothing
        filemap_t(const std::filesystem::path &filepath);
        
        //=================================================================================
        /// Memory map the specified file, and the variables
        /// ptr and length will be set
        /// - Parameters:
        ///     - filepath: The filepath to the file
        /// - Returns: The pointer to the memory mapped file
        /// - Throws: If unable to open the file or map the file, throws a runtime error
        [[maybe_unused]] auto map(const std::filesystem::path &filepath) ->void* ;
        
        //=================================================================================
        /// Unmaps a file from memory
        /// - Parameters:
        ///     - nothrow: specifies if the method should throw on error
        /// - Returns: True if able to unmap the file
        /// - Throws: If unable to unmap the file, throws a runtime error based on nothrow value.
        [[maybe_unused]] auto unmap(bool nothrow=false)->bool ;
        
        /// The pointer to the memory mapped file. Will be nullptr if not mapped
        void *ptr ;
        
        /// The length of the file in memory. Will be zero if not mapped
        std::size_t length ;
    };
}
#endif /* filemap_hpp */
