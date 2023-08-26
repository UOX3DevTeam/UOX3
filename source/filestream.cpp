//

#include "filestream.hpp"
#include <stdexcept>
#include <fstream>

using namespace std::string_literals ;

//======================================================================
BaseStream::BaseStream(){
    type = none ;
}
//======================================================================
PathStream::PathStream():BaseStream() {
    type = text ;
}
//======================================================================
PathStream::PathStream(const std::filesystem::path &path) : PathStream() {
    this->path = path ;
}
//======================================================================
auto PathStream::save(const std::filesystem::path &path) ->void{
    std::string line ;
    auto output = std::ofstream(path) ;
    if (output.is_open()){
        while (std::getline(stream,line)){
            output << line <<"\n" ;
        }
    }
}
//======================================================================
BinaryStream::BinaryStream():BaseStream() {
    type = binary ;
    
}
//======================================================================
BinaryStream::BinaryStream(const std::filesystem::path &path) : BinaryStream() {
    this->path = path ;
}
//======================================================================
auto BinaryStream::save(const std::filesystem::path &path) ->void{
    std::string line ;
    auto output = std::ofstream(path,std::ios::binary) ;
    if (output.is_open()){
        output.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
}

//======================================================================
auto saveStream(BaseStream *entry) ->void {
    entry->save(entry->path) ;
}
