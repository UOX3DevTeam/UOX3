//

#ifndef filestream_hpp
#define filestream_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//======================================================================
struct BaseStream {
    enum Type {none,text,binary};
    Type type ;
    std::filesystem::path path ;
    virtual auto save(const std::filesystem::path &path) ->void {} ;
    virtual ~BaseStream() = default ;
    BaseStream() ;
};
struct PathStream:BaseStream{
    PathStream() ;
    PathStream(const std::filesystem::path &path) ;
    ~PathStream() = default ;
    auto save(const std::filesystem::path &path) ->void final ;
    std::stringstream stream;
};
struct BinaryStream:BaseStream{
    BinaryStream() ;
    BinaryStream(const std::filesystem::path &path) ;
    ~BinaryStream() = default ;
    auto save(const std::filesystem::path &path) ->void final ;
    std::vector<std::uint8_t> data ;
};
auto saveStream(BaseStream *entry) ->void;
#endif /* filestream_hpp */
