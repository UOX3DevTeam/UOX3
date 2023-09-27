//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef uop_hpp
#define uop_hpp

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
namespace uo {
    namespace uop {
        //======================================================================
        //=========================================================================================
        // Constants used
        //=========================================================================================
        // Define the version we support
        constexpr   auto supported_version = std::uint32_t(5) ;
        // Define the default table_size
        constexpr   auto default_table_size  = std::uint32_t(1000) ;
        
        
        //=========================================================================================
        /// Represents a table entry in a uop table
        struct table_entry {
            static constexpr auto entry_size = 34 ;
            std::uint64_t    file_offset ;          // Offset into the file where the data associated with the entry is located at
            std::uint32_t    header_length ;        // If that data has a header, the length of it. To offset to the data, you would do: offset + header_length ;
            std::uint32_t    compressed_length ;    // THe size of the compressed data (you use this to read the data)
            std::uint32_t    decompressed_length ;  // The size of the data after decompressing
            std::uint64_t    hash ;                 // What is the hash value for this entry
            std::uint32_t    data_hash ;            // The hash of the actual data (not sure how this is used)
            std::int16_t     use_compression ;      // If 1, then the data is compressed with zlib
            table_entry();
            table_entry(const std::uint8_t *ptr) ;
            table_entry(std::istream &input);
            
            auto read(std::istream &input) ->void ;
            auto read(const std::uint8_t *ptr) ->void ;
            auto save(std::uint8_t *ptr) const ->void ;
            auto save(std::ostream &input) const ->void ;
            auto data() const ->std::vector<std::uint8_t> ;
            auto valid() const ->bool ;
        };
        
        //=========================================================================================
        /// Represents the header at the beginning of each table in a uop file
        /// The table is then followed by "number_entries" of table_entry
        struct table_header {
            static constexpr auto entry_length = 12 ;
            
            std::uint32_t number_entries ;
            std::uint64_t next_table ;
            table_header():number_entries(0),next_table(0){}
            table_header(const std::uint8_t *ptr) ;
            table_header(std::istream &input);
            auto load(const std::uint8_t *ptr) ->void ;
            auto load(std::istream &input) ->void ;
            auto save(std::ostream &output,bool reserve) const ->void ;
            
        };
        
        //=========================================================================================
        /// Define if a UOP table is valid (has correct signature and version)
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: true if valid
        auto valid(const std::uint8_t *ptr) ->bool ;
        //=========================================================================================
        /// Define if a UOP table is valid (has correct signature and version)
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: true if valid
        auto valid(std::istream &input) ->bool ;
        
        //=========================================================================================
        /// Retrieve the number of entries in a uop file header
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: number of entries
        auto numberEntries(const std::uint8_t *ptr)->std::uint32_t;
        //=========================================================================================
        /// Retrieve the number of entries in a uop file header
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: number of entries
        auto numberEntries(std::istream &input)->std::uint32_t ;
        //=========================================================================================
        /// Set the number of entries specified in the uop header
        /// Parameters:
        ///     - ptr: ptr to the uop data
        ///     - count: the number of entries to set the entry count to in the uop header
        /// - Returns: nothing
        auto setNumberEntries( std::uint8_t *ptr,std::uint32_t count)->void ;
        //=========================================================================================
        /// Set the number of entries specified in the uop header
        /// Parameters:
        ///     - input: the input stream for the uop data
        ///     - count: the number of entries to set the entry count to in the uop header
        /// - Returns: nothing
        auto setNumberEntries(std::ostream &output,std::uint32_t count)->void ;
        
        //=========================================================================================
        /// Retrieve the starting offset first table entry
        ///     - ptr: ptr to the uop data
        /// - Returns: The offset for the table
        auto firstTableOffset(const std::uint8_t *ptr) ->std::uint64_t;
        //=========================================================================================
        /// Retrieve the starting offset first table entry
        ///     - input: the input stream for the uop data
        /// - Returns: The offset for the table
        auto firstTableOffset(std::istream &input) ->std::uint64_t ;
        //=========================================================================================
        /// Set the offset for the first table entry
        /// Parameters:
        ///     - ptr: ptr to the uop data
        ///     - offset: the offset the first table is out, will be set in the header
        /// - Returns: nothing
        auto setFirstTableOffset(std::uint8_t *ptr,std::uint64_t offset) ->void ;
        //=========================================================================================
        /// Set the offset for the first table entry
        /// Parameters:
        ///     - input: the input stream for the uop data
        ///     - offset: the offset the first table is out, will be set in the header
        /// - Returns: nothing
        auto setFirstTableOffset(std::ostream &output,std::uint64_t offset) ->void ;
        
        //=========================================================================================
        /// Get the offsets for each table in a uop file
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: the offsets for each table in the uop file
        auto offsetsTable(const std::uint8_t *ptr) ->std::vector<std::uint64_t> ;
        //=========================================================================================
        /// Get the offsets for each table in a uop file
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: the offsets for each table in the uop file
        auto offsetsTable(std::istream &input) ->std::vector<std::uint64_t> ;
        
        
        //=========================================================================================
        /// Return the offsets for entry, and with the actual entry, in a uop table
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: The offset, and the actual table entry for the entries in the table at the requested offset
        auto offsetsTableEntries(const std::uint8_t *ptr,std::uint64_t offset) -> std::map<std::uint64_t,table_entry>;
        //=========================================================================================
        /// Return the offsets for entry, and with the actual entry, in a uop table
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: The offset, and the actual table entry for the entries in the table at the requested offset
        auto offsetsTableEntries(std::istream &input, std::uint64_t offset) -> std::map<std::uint64_t,table_entry>;
        
        //=========================================================================================
        /// Return the offsets for all entries, and with the actual entry, in a uop file
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: The offset, and the actual table entry for the entries in a uop file
        auto allEntries(const std::uint8_t* ptr) -> std::map<std::uint64_t,table_entry>;
        //=========================================================================================
        /// Return the offsets for all entries, and with the actual entry, in a uop file
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: The offset, and the actual table entry for the entries in a uop file
        auto allEntries(std::istream &input) -> std::map<std::uint64_t,table_entry>;
        
        //=========================================================================================
        /// Create a table and blank entries at the specified offset.
        /// This does not update the previous table or the number of entries in the uop file. Use addEntries for that.
        /// Parameters:
        ///     - output: the output stream for the uop data
        ///     - offset: offset to write the table at
        ///     - tablesize: the number of entries to write for the table
        /// - Returns: The offsets for each entry in the created table.
        auto createTable(std::ostream &output,std::uint64_t offset,std::uint32_t tablesize) ->std::vector<std::uint64_t>;
        
        //=========================================================================================
        /// Add a number of blank entries to a uop file
        /// This will create new table(s) as needed, and update the uop entry count in the header
        /// Parameters:
        ///     - stream: the stream for the uop data  (must be a fstream, not ofstream)
        ///     - count: The number of entries to add
        /// - Returns: The offsets for each entry in the created file.
        auto addEntries(std::fstream &stream, std::uint32_t count) -> std::vector<std::uint64_t> ;
        
        //=========================================================================================
        /// Compress the data specified data
        /// Parameters:
        ///     - data: vector of data to compress
        /// - Returns: a vector of compress data
        auto compressData(const std::vector<std::uint8_t> &data) ->std::vector<std::uint8_t>;
        //=========================================================================================
        /// Compress the data specified data
        /// Parameters:
        ///     - data: a ptr to the data to compress
        ///     - size: the number of bytes in the data to compress
        /// - Returns: a vector of compress data
        auto compressData(const std::uint8_t *data,std::size_t size) ->std::vector<std::uint8_t>;
        
        //=========================================================================================
        /// Create a uop file, with the table entries for the number of table size specified
        /// Parameters:
        ///     - output: the stream to write to (is also internally read, why it must be an fstream, not an ostream)
        ///     - numentries: Initial number of entries to reserve space for
        ///     - maxtablesize: The max size of any table (default is 512)
        ///     - version: The version of uop used (default 5) ;
        /// - Returns: a vector of offsets for each table entry
        auto create(std::fstream &output, std::uint32_t numentries,std::uint32_t maxtablesize=default_table_size,std::uint32_t version = supported_version ) ->std::vector<std::uint64_t> ;
        
        
        
        //=========================================================================================
        /// Request all entries that match the supplied hashes from a map of entries (such as allEntries())
        /// Parameters:
        ///     - requested: a map of <hash,id> of each item desired
        ///     - entries: a map  of <offsets,table_entries> such as returned allEntries
        /// - Returns: a map of <id,<offset,table_entry>> of entries that matched one the requested hashes
        auto entriesFor(const std::map<std::uint64_t,std::uint32_t> &requested, const std::map<std::uint64_t,table_entry> &entries) -> std::map<uint32_t,std::pair<std::uint64_t,table_entry>> ;
        
        
        
        //=========================================================================================
        /// Read the uop data indicated by the table entry.
        /// If the data is compressed, it will be de compressed
        /// Parameters:
        ///     - entry: the table entry one wants the data needed
        ///     - input: the uop data stream
        /// - Returns: a vector containing the deceompressed data
        auto readData(const table_entry &entry, std::istream &input) ->std::vector<std::uint8_t>;
        //=========================================================================================
        /// Read the uop data indicated by the table entry.
        /// If the data is compressed, it will be de compressed
        /// Parameters:
        ///     - entry: the table entry one wants the data needed
        ///     - ptr: ptr to the uop data
        /// - Returns: a vector containing the deceompressed data
        auto readData(const table_entry &entry, const std::uint8_t *ptr) ->std::vector<std::uint8_t>;
    }
}
#endif /* uop_hpp */
