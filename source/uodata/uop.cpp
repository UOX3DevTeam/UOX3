//Copyright © 2023 Charles Kerr. All rights reserved.

#include "uop.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <zlib.h>

using namespace std::string_literals ;
namespace uo{
    namespace uop{
        //======================================================================
        
        //=========================================================================================
        // Constants used
        //=========================================================================================
        
        // Define the uop signature
        constexpr   auto identifer = std::uint32_t(0x50594D);
        
        // Define where the start table offset is define
        //constexpr   auto table_offset_location = std::uint32_t(12) ;
        
        // Define where the tables start for writing purposes
        constexpr   auto starting_offset = std::uint64_t(512) ;
        
        
        
        
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
        table_entry::table_entry(const std::uint8_t *ptr):table_entry() {
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
        auto table_entry::read(const std::uint8_t *ptr) ->void{
            auto offset = 0 ;
            
            std::copy(ptr,ptr+sizeof(file_offset),reinterpret_cast<std::uint8_t*>(&file_offset));
            offset+=sizeof(file_offset);
            
            std::copy(ptr+offset,ptr+offset+sizeof(header_length),reinterpret_cast<std::uint8_t*>(&header_length));
            offset+=sizeof(header_length);
            
            std::copy(ptr+offset,ptr+offset+sizeof(compressed_length),reinterpret_cast<std::uint8_t*>(&compressed_length));
            offset+=sizeof(compressed_length);
            
            std::copy(ptr+offset,ptr+offset+sizeof(decompressed_length),reinterpret_cast<std::uint8_t*>(&decompressed_length));
            offset+=sizeof(decompressed_length);
            
            std::copy(ptr+offset,ptr+offset+sizeof(hash),reinterpret_cast<std::uint8_t*>(&hash));
            offset+=sizeof(hash);
            
            std::copy(ptr+offset,ptr+offset+sizeof(data_hash),reinterpret_cast<std::uint8_t*>(&data_hash));
            offset+=sizeof(data_hash);
            
            std::copy(ptr+offset,ptr+offset+sizeof(use_compression),reinterpret_cast<std::uint8_t*>(&use_compression));
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
        auto table_entry::save(std::uint8_t *ptr) const ->void{
            auto offset = 0 ;
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&file_offset),reinterpret_cast<const std::uint8_t*>(&file_offset)+sizeof(file_offset),ptr+offset);
            offset+=sizeof(file_offset);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&header_length),reinterpret_cast<const std::uint8_t*>(&header_length)+sizeof(header_length),ptr+offset);
            offset+=sizeof(header_length);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&compressed_length),reinterpret_cast<const std::uint8_t*>(&compressed_length)+sizeof(compressed_length),ptr+offset);
            offset+=sizeof(compressed_length);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&decompressed_length),reinterpret_cast<const std::uint8_t*>(&decompressed_length)+sizeof(decompressed_length),ptr+offset);
            offset+=sizeof(decompressed_length);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&hash),reinterpret_cast<const std::uint8_t*>(&hash)+sizeof(hash),ptr+offset);
            offset+=sizeof(hash);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&data_hash),reinterpret_cast<const std::uint8_t*>(&data_hash)+sizeof(data_hash),ptr+offset);
            offset+=sizeof(data_hash);
            
            std::copy(reinterpret_cast<const std::uint8_t*>(&use_compression),reinterpret_cast<const std::uint8_t*>(&use_compression)+sizeof(use_compression),ptr+offset);
            
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
        table_header::table_header(const std::uint8_t *ptr):table_header(){
            load(ptr);
        }
        //=========================================================================================
        table_header::table_header(std::istream &input):table_header(){
            load(input);
        }
        
        //=========================================================================================
        auto table_header::load(const std::uint8_t *ptr) ->void {
            if (ptr == nullptr){
                throw std::runtime_error("Unable to load table header: nullptr");
            }
            std::copy(ptr,ptr+4,reinterpret_cast<std::uint8_t*>(&number_entries));
            std::copy(ptr+4,ptr+12,reinterpret_cast<std::uint8_t*>(&next_table));
            
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
        auto valid(const std::uint8_t *ptr) ->bool {
            auto sig = std::uint32_t(0);
            auto ver = std::uint32_t(0);
            if (ptr == nullptr){
                return false ;
            }
            std::copy (ptr,ptr+4,reinterpret_cast<std::uint8_t*>(&sig));
            std::copy(ptr+4,ptr+8,reinterpret_cast<std::uint8_t*>(&ver));
            
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
        auto numberEntries(const std::uint8_t *ptr)->std::uint32_t {
            auto count = std::uint32_t(0);
            std::copy(ptr+24,ptr+28,reinterpret_cast<std::uint8_t*>(&count));
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
        auto setNumberEntries( std::uint8_t *ptr,std::uint32_t count)->void {
            std::copy(reinterpret_cast<std::uint8_t*>(&count),reinterpret_cast<std::uint8_t*>(&count)+4,ptr+24);
        }
        //===========================================================================================
        auto setNumberEntries(std::ostream &output,std::uint32_t count)->void {
            auto current = output.tellp();
            output.seekp(24,std::ios::beg);
            output.write(reinterpret_cast<char*>(&count),4);
            output.seekp(current,std::ios::beg);
        }
        
        //===========================================================================================
        auto firstTableOffset(const std::uint8_t *ptr) ->std::uint64_t {
            auto offset = std::uint64_t(0);
            std::copy(ptr+12,ptr+12+sizeof(offset),reinterpret_cast<std::uint8_t*>(&offset));
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
        auto setFirstTableOffset(std::uint8_t *ptr,std::uint64_t offset) ->void {
            std::copy(reinterpret_cast<std::uint8_t*>(&offset),reinterpret_cast<std::uint8_t*>(&offset)+sizeof(offset),ptr+12);
        }
        //===========================================================================================
        auto setFirstTableOffset(std::ostream &output,std::uint64_t offset) ->void {
            auto current = output.tellp();
            output.seekp(12,std::ios::beg);
            output.write(reinterpret_cast<char*>(&offset),sizeof(offset));
            output.seekp(current,std::ios::beg);
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
        auto offsetsTable(const std::uint8_t *ptr) ->std::vector<std::uint64_t> {
            auto offsets = std::vector<std::uint64_t>();
            auto offset = firstTableOffset(ptr);
            while (offset != 0){
                offsets.push_back(offset);
                std::copy(ptr+offset+4,ptr+offset+4+sizeof(offset),reinterpret_cast<std::uint8_t*>(&offset));
            }
            return offsets ;
        }
        
        //===========================================================================================
        auto offsetsTableEntries(const std::uint8_t *ptr,std::uint64_t offset) -> std::map<std::uint64_t,table_entry>{
            auto entries = std::map<std::uint64_t,table_entry>();
            auto header = table_header(ptr+offset);
            for (std::uint32_t j=0 ; j<header.number_entries;j++){
                auto entry_offset = offset+table_header::entry_length+(j*table_entry::entry_size);
                entries.insert_or_assign(entry_offset,table_entry(ptr +entry_offset));
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
        auto allEntries(const std::uint8_t* ptr) -> std::map<std::uint64_t,table_entry> {
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
        
        //==========================================================================================
        auto createTable(std::ostream &output,std::uint64_t offset,std::uint32_t tablesize) ->std::vector<std::uint64_t> {
            auto header = table_header() ;
            if (tablesize > default_table_size){
                throw std::runtime_error("Table size exceeds max table size: "s + std::to_string(default_table_size));
            }
            header.number_entries = tablesize;
            header.next_table=0 ;
            output.seekp(0,std::ios::end);
            header.save(output,false);
            auto offsets = std::vector<std::uint64_t>();
            auto buffer = std::vector<char>(table_entry::entry_size,0);
            for (std::uint32_t j= 0 ; j< tablesize;j++){
                offsets.push_back(static_cast<std::uint64_t>(output.tellp()));
                output.write(buffer.data(),buffer.size());
            }
            return offsets ;
        }
        //===========================================================================================
        auto addEntries(std::fstream &stream, std::uint32_t count) -> std::vector<std::uint64_t> {
            
            auto offsets = std::vector<std::uint64_t>() ;
            auto original_count = numberEntries(stream);
            // first thing to do , is update the number of entries
            setNumberEntries(stream,original_count+count );
            auto lastoffset = std::uint64_t(0) ;
            if (original_count>0){
                auto tables = offsetsTable(stream) ;
                lastoffset = *(tables.rbegin()) ;
            }
            auto numtables = count / default_table_size ;
            stream.seekp(0,std::ios::end);
            auto offset_for_last = static_cast<std::uint64_t>(stream.tellp());
            
            numtables = numtables + ((numtables%default_table_size >0)?1:0) ;
            
            auto tablesize = count ;
            while(count >0) {
                tablesize = std::min(default_table_size,tablesize);
                count -= tablesize ;
                auto header = table_header() ;
                header.number_entries=tablesize ;
                if (count >0){
                    
                    header.next_table=static_cast<std::uint64_t>(stream.tellp())+table_header::entry_length+tablesize*table_entry::entry_size;
                }
                header.save(stream,false);
                auto entry = table_entry();
                for (std::uint32_t j=0 ;j<tablesize;j++){
                    offsets.push_back(static_cast<std::uint64_t>(stream.tellp()));
                    entry.save(stream);
                }
                
            }
            auto current = stream.tellp();
            // Now, do we update a table?
            if (lastoffset>0){
                stream.seekg(lastoffset,std::ios::beg);
                auto lasttable = table_header(stream) ;
                lasttable.next_table =offset_for_last ;;
                stream.seekp(lastoffset,std::ios::beg);
                lasttable.save(stream, false);
            }
            else {
                // there where no tables, so we need to update the file header
                stream.seekp(12,std::ios::beg);
                stream.write(reinterpret_cast<char*>(&lastoffset),sizeof(lastoffset));
                
            }
            stream.seekp(current,std::ios::beg);
            return offsets;
            
        }
        
        
        
        
        //===========================================================================================
        auto compressData(const std::vector<std::uint8_t> &data) ->std::vector<std::uint8_t> {
            auto sourcesize = static_cast<uLong>(data.size());
            auto roughsize = compressBound(sourcesize);
            auto dest = std::vector<std::uint8_t>(roughsize,0) ;
            auto status = compress(dest.data(), &roughsize, data.data(), sourcesize) ;
            if (status != Z_OK) {
                throw std::runtime_error("Error compressing data."s );
            }
            dest.resize(roughsize);
            return dest ;
        }
        //===========================================================================================
        auto compressData(const std::uint8_t *data,std::size_t size) ->std::vector<std::uint8_t> {
            auto sourcesize = static_cast<uLong>(size);
            auto roughsize = compressBound(sourcesize);
            auto dest = std::vector<std::uint8_t>(roughsize,0) ;
            auto status = compress(dest.data(), &roughsize, data, sourcesize) ;
            if (status != Z_OK) {
                throw std::runtime_error("Error compressing data."s );
            }
            dest.resize(roughsize);
            return dest ;
        }
        
        //===========================================================================================
        auto create(std::fstream &output, std::uint32_t numentries,std::uint32_t maxtablesize,std::uint32_t version ) ->std::vector<std::uint64_t>{
            auto rvalue = std::vector<std::uint64_t>() ;
            if (output.good()){
                output.write(reinterpret_cast<const char*>(&identifer),sizeof(identifer));
                output.write(reinterpret_cast<const char*>(&version),sizeof(version));
                auto value = std::uint32_t(0xFD23EC43);// No idea if this needs to be this value, or 0, or whatever
                output.write(reinterpret_cast<char*>(&value),sizeof(value));
                // Were does the first table reside in the file?
                output.write(reinterpret_cast<const char*>(&starting_offset),sizeof(starting_offset));
                // table size
                output.write(reinterpret_cast<const char*>(&maxtablesize),sizeof(maxtablesize));
                // Number of entries
                output.write(reinterpret_cast<const char*>(&numentries),sizeof(numentries));
                // No idea if the next values need to be 1, 0 , or doesnt matter, so will copy what one was
                value = 1 ;
                output.write(reinterpret_cast<char*>(&value),sizeof(value));
                output.write(reinterpret_cast<char*>(&value),sizeof(value));
                value = 0 ;
                output.write(reinterpret_cast<char*>(&value),sizeof(value));
                
                // Ok, now we will fill in the rest of space until the table offset with zeros
                auto byte = std::uint8_t(0) ;
                auto temploc = static_cast<std::uint32_t>(output.tellp());
                for (std::uint32_t j= temploc ; j < starting_offset ; j++){
                    output.write(reinterpret_cast<char*>(&byte),1);
                }
                rvalue = addEntries(output, numentries);
            }
            return rvalue ;
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
        auto readData(const table_entry &entry, const std::uint8_t *ptr) ->std::vector<std::uint8_t> {
            auto buffer = std::vector<std::uint8_t>(entry.compressed_length,0) ;
            auto dataptr = ptr + entry.file_offset+entry.header_length;
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
