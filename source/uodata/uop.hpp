//

#ifndef uop_hpp
#define uop_hpp

#include <cstdint>
#include <string>
#include <istream>
#include <ostream>
#include <map>
#include <vector>
//=========================================================================================
namespace uo {
    namespace uop{
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
            table_entry(const void *ptr) ;
            table_entry(std::istream &input);
            
            auto read(std::istream &input) ->void ;
            auto read(const void *ptr) ->void ;
            auto save(void *ptr) const ->void ;
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
            table_header(const void *ptr) ;
            table_header(std::istream &input);
            auto load(const void *ptr) ->void ;
            auto load(std::istream &input) ->void ;
            auto save(std::ostream &output,bool reserve) const ->void ;
            
        };
        

        //=========================================================================================
        /// Define if a UOP file is valid (has correct signature and version)
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: true if valid
        auto valid(const void *ptr) ->bool ;
        //=========================================================================================
        /// Define if a UOP file is valid (has correct signature and version)
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: true if valid
        auto valid(std::istream &input) ->bool ;
        
        //=========================================================================================
        /// Retrieve the number of entries in a uop file header
        /// Parameters:
        ///     - ptr: ptr to the uop data
        /// - Returns: number of entries
        auto numberEntries(const void *ptr)->std::uint32_t;
        //=========================================================================================
        /// Retrieve the number of entries in a uop file header
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: number of entries
        auto numberEntries(std::istream &input)->std::uint32_t ;
        //=========================================================================================
        /// Retrieve the starting offset first table entry
        ///     - ptr: ptr to the uop data
        /// - Returns: The offset for the table
        auto firstTableOffset(const void *ptr) ->std::uint64_t;
        //=========================================================================================
        /// Retrieve the starting offset first table entry
        ///     - input: the input stream for the uop data
        /// - Returns: The offset for the table
        auto firstTableOffset(std::istream &input) ->std::uint64_t ;
        //=========================================================================================
        /// Get the offsets for each table in a uop file
        /// Parameters:
        ///     - ptr: ptr to the uop data
       /// - Returns: the offsets for each table in the uop file
        auto offsetsTable(const void *ptr) ->std::vector<std::uint64_t> ;
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
        auto offsetsTableEntries(const void *ptr,std::uint64_t offset) -> std::map<std::uint64_t,table_entry>;
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
        auto allEntries(const void * ptr) -> std::map<std::uint64_t,table_entry>;
        //=========================================================================================
        /// Return the offsets for all entries, and with the actual entry, in a uop file
        /// Parameters:
        ///     - input: the input stream for the uop data
        /// - Returns: The offset, and the actual table entry for the entries in a uop file
        auto allEntries(std::istream &input) -> std::map<std::uint64_t,table_entry>;
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
