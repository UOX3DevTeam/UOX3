//

#ifndef filestream_hpp
#define filestream_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <future>




//======================================================================
struct basestream {
    std::filesystem::path path;
    basestream(const std::filesystem::path &path){this->path = path;}
    basestream() = default ;
    virtual ~basestream() = default ;
    virtual auto save() const ->void {}
};
//======================================================================
struct ObjectDescription {
    std::vector<std::pair<std::string,std::string>> description ;
    ObjectDescription() { description.reserve(500);}
};
//======================================================================
struct ObjectStream : public basestream {
    std::vector<ObjectDescription> contents ;
    ObjectStream():basestream(){contents.reserve(2500);}
    ObjectStream(const std::filesystem::path &path):basestream(path){contents.reserve(2500);}
    auto save() const -> void final ;
};


auto waitOnAllFuture() -> void ;
auto queueStream(basestream *stream) ->void ;
auto saveStream(basestream *stream) ->void;
#endif /* filestream_hpp */
