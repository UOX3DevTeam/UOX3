//

#include "uop.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <zlib.h>

using namespace std::string_literals ;

//=========================================================================================
namespace uo {
    namespace uop{
        //=========================================================================================
        // Constants used
        //=========================================================================================
        // Define the version we support
        constexpr   auto supported_version = std::uint32_t(5) ;
        // Define the uop signature
        constexpr   auto identifer = std::uint32_t(0x50594D);

        //=========================================================================================
        // internal structures
        //=========================================================================================
        
        //=========================================================================================
        // table_entry
        //=========================================================================================
        
        //=========================================================================================
        table_entry::table_entry():file_offset(0),header_length(0),compressed_length(0),decompressed_length(0),hash(0),data_hash(0),use_compression(0){
        }
        //=========================================================================================
        table_entry::table_entry(const void *ptr):table_entry() {
            if (ptr == nullptr){
                throw std::runtime_error("uop_table_entry initialization failed: ptr was null");
            }
            this->read(ptr) ;
        }
        //=========================================================================================
        table_entry::table_entry(std::istream &input):table_entry() {
            this->read(input) ;
        }
        //=========================================================================================
        auto table_entry::read(std::istream &input) ->void {
            input.read(reinterpret_cast<char*>(&file_offset),sizeof(file_offset));
            input.read(reinterpret_cast<char*>(&header_length),sizeof(header_length));
            input.read(reinterpret_cast<char*>(&compressed_length),sizeof(compressed_length));
            input.read(reinterpret_cast<char*>(&decompressed_length),sizeof(decompressed_length));
            input.read(reinterpret_cast<char*>(&hash),sizeof(hash));
            input.read(reinterpret_cast<char*>(&data_hash),sizeof(data_hash));
            input.read(reinterpret_cast<char*>(&use_compression),sizeof(use_compression));
        }
        //=========================================================================================
        auto table_entry::read(const void *ptr) ->void{
            auto offset = 0 ;
            
            std::copy(static_cast<const std::uint8_t*>(ptr),static_cast<const std::uint8_t*>(ptr)+sizeof(file_offset),reinterpret_cast<std::uint8_t*>(&file_offset));
            offset+=sizeof(file_offset);
            
            std::copy(static_cast<const std::uint8_t*>(ptr)+offset,static_cast<const std::uint8_t*>(ptr)+offset+sizeof(header_length),reinterpret_cast<std::uint8_t*>(&header_length));
            offset+=sizeof(header_length);
            
            std::copy(static_cast<const std::uint8_t*>(ptr)+offset,static_cast<const std::uint8_t*>(ptr)+offset+sizeof(compressed_length),reinterpret_cast<std::uint8_t*>(&compressed_length));
            offset+=sizeof(compressed_length);
            
            std::copy(static_cast<const std::uint8_t*>(ptr)+offset,static_cast<const std::uint8_t*>(ptr)+offset+sizeof(decompressed_length),reinterpret_cast<std::uint8_t*>(&decompressed_length));
            offset+=sizeof(decompressed_length);
            
            std::copy(static_cast<const std::uint8_t*>(ptr)+offset,static_cast<const std::uint8_t*>(ptr)+offset+sizeof(hash),reinterpret_cast<std::uint8_t*>(&hash));
            offset+=sizeof(hash);
            
            std::copy(static_cast<const std::uint8_t*>(ptr)+offset,static_cast<const std::uint8_t*>(ptr)+offset+sizeof(data_hash),reinterpret_cast<std::uint8_t*>(&data_hash));
            offset+=sizeof(data_hash);
            
            std::copy(static_cast<const std::uint8_t*>(ptr)+offset,static_cast<const std::uint8_t*>(ptr)+offset+sizeof(use_compression),reinterpret_cast<std::uint8_t*>(&use_compression));
        }
        //=========================================================================================
        auto table_entry::save(std::ostream &output) const ->void{
            output.write(reinterpret_cast<const char*>(&file_offset),sizeof(file_offset));
            output.write(reinterpret_cast<const char*>(&header_length),sizeof(header_length));
            output.write(reinterpret_cast<const char*>(&compressed_length),sizeof(compressed_length));
            output.write(reinterpret_cast<const char*>(&decompressed_length),sizeof(decompressed_length));
            output.write(reinterpret_cast<const char*>(&hash),sizeof(hash));
            output.write(reinterpret_cast<const char*>(&data_hash),sizeof(data_hash));
            output.write(reinterpret_cast<const char*>(&use_compression),sizeof(use_compression));
        }
        //=========================================================================================
        auto table_entry::save(void *ptr) const ->void{
            auto offset = 0 ;
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&file_offset),reinterpret_cast<const std::uint8_t*>(&file_offset)+sizeof(file_offset),static_cast<const std::uint8_t*>(ptr)+offset);
            offset+=sizeof(file_offset);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&header_length),reinterpret_cast<const std::uint8_t*>(&header_length)+sizeof(header_length),static_cast<const std::uint8_t*>(ptr)+offset);
            offset+=sizeof(header_length);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&compressed_length),reinterpret_cast<const std::uint8_t*>(&compressed_length)+sizeof(compressed_length),static_cast<const std::uint8_t*>(ptr)+offset);
            offset+=sizeof(compressed_length);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&decompressed_length),reinterpret_cast<const std::uint8_t*>(&decompressed_length)+sizeof(decompressed_length),static_cast<const std::uint8_t*>(ptr)+offset);
            offset+=sizeof(decompressed_length);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&hash),reinterpret_cast<const std::uint8_t*>(&hash)+sizeof(hash),static_cast<const std::uint8_t*>(ptr)+offset);
            offset+=sizeof(hash);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&data_hash),reinterpret_cast<const std::uint8_t*>(&data_hash)+sizeof(data_hash),static_cast<const std::uint8_t*>(ptr)+offset);
            offset+=sizeof(data_hash);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&use_compression),reinterpret_cast<const std::uint8_t*>(&use_compression)+sizeof(use_compression),static_cast<const std::uint8_t*>(ptr)+offset);
            
        }
        
        //=========================================================================================
        auto table_entry::data() const ->std::vector<std::uint8_t> {
            auto rvalue = std::vector<std::uint8_t>(entry_size,0);
            this->save(rvalue.data());
            return rvalue ;
        }
        //=========================================================================================
        auto table_entry::valid() const ->bool{
            return (hash!=0) && (decompressed_length!=0) ;
        }
        
        //=========================================================================================
        //table_header
        //=========================================================================================
        
        //=========================================================================================
        table_header::table_header(const void *ptr):table_header(){
            load(ptr);
        }
        //=========================================================================================
        table_header::table_header(std::istream &input):table_header(){
            load(input);
        }
        
        //=========================================================================================
        auto table_header::load(const void *ptr) ->void {
            if (ptr == nullptr){
                throw std::runtime_error("Unable to load table header: nullptr");
            }
            std::copy(static_cast<const std::uint8_t*>(ptr),static_cast<const std::uint8_t*>(ptr)+4,reinterpret_cast<std::uint8_t*>(&number_entries));
            std::copy(static_cast<const std::uint8_t*>(ptr)+4,static_cast<const std::uint8_t*>(ptr)+12,reinterpret_cast<std::uint8_t*>(&next_table));
            
        }
        //=========================================================================================
        auto table_header::load(std::istream &input) ->void {
            if (!input.good()){
                throw std::runtime_error("Unable to load table header: stream not good state");
            }
            input.read(reinterpret_cast<char*>(&number_entries),4);
            input.read(reinterpret_cast<char*>(&next_table),8);
        }
        //=========================================================================================
        auto table_header::save(std::ostream &output,bool reserve) const ->void {
            if (!output.good()){
                throw std::runtime_error("Unable to save table header: stream not good state");
            }
            output.write(reinterpret_cast<const char*>(&number_entries),4);
            output.write(reinterpret_cast<const char*>(&next_table),8);
            if (reserve){
                auto buffer = std::vector<char>(table_entry::entry_size * number_entries,0) ;
                output.write(buffer.data(), buffer.size()  );
            }
        }
        
        //=========================================================================================
        // Generic uop methods
        //=========================================================================================
        
        //=========================================================================================
        auto valid(const void *ptr) ->bool {
            auto sig = std::uint32_t(0);
            auto ver = std::uint32_t(0);
            if (ptr == nullptr){
                return false ;
            }
            std::copy (static_cast<const std::uint8_t*>(ptr),static_cast<const std::uint8_t*>(ptr)+4,reinterpret_cast<std::uint8_t*>(&sig));
            std::copy(static_cast<const std::uint8_t*>(ptr)+4,static_cast<const std::uint8_t*>(ptr)+8,reinterpret_cast<std::uint8_t*>(&ver));
            
            return (sig == identifer) && (ver <= supported_version) ;
        }
        //=========================================================================================
        auto valid(std::istream &input) ->bool {
            auto sig = std::uint32_t(0);
            auto ver = std::uint32_t(0);
            input.read(reinterpret_cast<char*>(&sig),4);
            input.read(reinterpret_cast<char*>(&ver),4);
            if (!input.good() || (input.gcount()!=4)){
                return false ;
            }
            return (sig == identifer) && (ver <= supported_version) ;
        }
        
        
        //===========================================================================================
        auto numberEntries(const void *ptr)->std::uint32_t {
            auto count = std::uint32_t(0);
            std::copy(static_cast<const std::uint8_t*>(ptr)+24,static_cast<const std::uint8_t*>(ptr)+28,reinterpret_cast<std::uint8_t*>(&count));
            return count ;
        }
        //===========================================================================================
        auto numberEntries(std::istream &input)->std::uint32_t {
            auto current = input.tellg();
            input.seekg(24,std::ios::beg);
            auto count = std::uint32_t(0);
            input.read(reinterpret_cast<char*>(&count),4);
            input.seekg(current,std::ios::beg);
            return count ;
        }
        //===========================================================================================
        auto firstTableOffset(const void *ptr) ->std::uint64_t {
            auto offset = std::uint64_t(0);
            std::copy(static_cast<const std::uint8_t*>(ptr)+12,static_cast<const std::uint8_t*>(ptr)+12+sizeof(offset),reinterpret_cast<std::uint8_t*>(&offset));
            return offset ;
        }
        //===========================================================================================
        auto firstTableOffset(std::istream &input) ->std::uint64_t {
            auto current = input.tellg();
            input.seekg(12,std::ios::beg);
            auto offset = std::uint64_t(0);
            input.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            input.seekg(current,std::ios::beg);
            return offset ;
        }
        //===========================================================================================
        auto offsetsTable(std::istream &input) ->std::vector<std::uint64_t> {
            auto offsets = std::vector<std::uint64_t>();
            auto current = input.tellg();
            auto offset = firstTableOffset(input);
            while (offset!=0){
                offsets.push_back(offset);
                input.seekg(offset+4,std::ios::beg);
                input.read(reinterpret_cast<char*>(&offset),sizeof(offset));
             }
            input.seekg(current,std::ios::beg);
            return offsets ;
        }
        
        //===========================================================================================
        auto offsetsTable(const void *ptr) ->std::vector<std::uint64_t> {
            auto offsets = std::vector<std::uint64_t>();
            auto offset = firstTableOffset(ptr);
            while (offset != 0){
                offsets.push_back(offset);
                std::copy(static_cast<const std::uint8_t*>(ptr)+offset+4,static_cast<const std::uint8_t*>(ptr)+offset+4+sizeof(offset),reinterpret_cast<std::uint8_t*>(&offset));
            }
            return offsets ;
        }

        //===========================================================================================
        auto offsetsTableEntries(const void *ptr,std::uint64_t offset) -> std::map<std::uint64_t,table_entry>{
            auto entries = std::map<std::uint64_t,table_entry>();
            auto header = table_header(static_cast<const std::uint8_t*>(ptr)+offset);
            for (std::uint32_t j=0 ; j<header.number_entries;j++){
                auto entry_offset = offset+table_header::entry_length+(j*table_entry::entry_size);
                entries.insert_or_assign(entry_offset,table_entry(static_cast<const std::uint8_t*>(ptr) +entry_offset));
            }
            return entries;
        }
        //===========================================================================================
        auto offsetsTableEntries(std::istream &input, std::uint64_t offset) -> std::map<std::uint64_t,table_entry>{
            auto entries = std::map<std::uint64_t,table_entry>();
            auto current = input.tellg();
            input.seekg(offset,std::ios::beg);
            
            auto header = table_header(input);
            for (std::uint32_t j=0 ; j<header.number_entries;j++){
                auto entry_offset = static_cast<std::uint64_t>(input.tellg());
                entries.insert_or_assign(entry_offset,table_entry(input));
            }
            input.seekg(current,std::ios::beg);
            return entries;
        }

        //==========================================================================================
        auto allEntries(const void *ptr) -> std::map<std::uint64_t,table_entry> {
            auto tables = offsetsTable(ptr);
            auto rvalue = std::map<std::uint64_t,table_entry>();
            for (const auto &offset:tables){
                auto temp = offsetsTableEntries(ptr,offset) ;
                for (auto const &entry :temp){
                    rvalue.insert(entry);
                }
            }
            return  rvalue ;
        }
        
        //==========================================================================================
        auto allEntries(std::istream &input) -> std::map<std::uint64_t,table_entry> {
            auto tables = offsetsTable(input);
            auto rvalue = std::map<std::uint64_t,table_entry>();
            for (const auto &offset:tables){
                auto temp = offsetsTableEntries(input,offset) ;
                for (auto const &entry :temp){
                    rvalue.insert(entry);
                }
            }
            return  rvalue ;
        }
        //===========================================================================================
        auto entriesFor(const std::map<std::uint64_t,std::uint32_t> &requested, const std::map<std::uint64_t,table_entry> &entries) -> std::map<uint32_t,std::pair<std::uint64_t,table_entry>> {
            
            auto rvalue = std::map<std::uint32_t,std::pair<std::uint64_t,table_entry>>();
            for (const auto &[hash,id]:requested){
                auto temphash = hash ;
                auto iter = std::find_if(entries.begin(),entries.end(),[temphash](const std::pair<std::uint64_t,table_entry> &value){
                    return value.second.hash == temphash ;
                });
                if (iter != entries.end()){
                    rvalue[id] = *iter ;
                }
            }
            return rvalue ;
        }

        
        //===========================================================================================
        auto readData(const table_entry &entry, std::istream &input) ->std::vector<std::uint8_t> {
            auto buffer = std::vector<std::uint8_t>(entry.compressed_length,0) ;
            input.seekg(entry.file_offset+entry.header_length,std::ios::beg);
            input.read(reinterpret_cast<char*>(buffer.data()),buffer.size());
            if (entry.use_compression){
                auto temp = std::vector<std::uint8_t>(entry.decompressed_length,0);
                uLong destlen = static_cast<uLongf>(temp.size()) ;
                uLong sourcelen = static_cast<uLongf>(buffer.size()) ;
                auto status = uncompress2(reinterpret_cast<Bytef *>(temp.data()), &destlen, reinterpret_cast<const Bytef *>(buffer.data()), &sourcelen);
                if (status != Z_OK){
                    throw std::runtime_error("Error decompressing data.");
                }
                temp.resize(destlen);
                std::swap(buffer,temp);
            }
            return buffer ;
        }
        //===========================================================================================
        auto readData(const table_entry &entry, const void *ptr) ->std::vector<std::uint8_t> {
            auto buffer = std::vector<std::uint8_t>(entry.compressed_length,0) ;
            auto dataptr = static_cast<const std::uint8_t*>(ptr)+ entry.file_offset+entry.header_length;
            std::copy(dataptr,dataptr+buffer.size(),buffer.data());
            if (entry.use_compression){
                auto temp = std::vector<std::uint8_t>(entry.decompressed_length,0);
                uLong destlen = static_cast<uLongf>(temp.size()) ;
                uLong sourcelen = static_cast<uLongf>(buffer.size()) ;
                auto status = uncompress2(reinterpret_cast<Bytef *>(temp.data()), &destlen, reinterpret_cast<const Bytef *>(buffer.data()), &sourcelen);
                if (status != Z_OK){
                    throw std::runtime_error("Error decompressing data.");
                }
                temp.resize(destlen);
                std::swap(buffer,temp);
            }
            return buffer ;
        }

    }
}
