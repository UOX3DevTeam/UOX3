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
struct filestream {
    std::filesystem::path path;
    std::vector<std::vector<std::pair<std::string,std::string>>> contents ;
    filestream(const std::filesystem::path &path){this->path = path;}
    
};
auto saveStream(filestream *stream) ->void ;

extern std::vector<std::future<void>> saveFutures;
auto waitOnAllFuture() -> void ;
auto startSave(filestream *stream) ->void ;

#endif /* filestream_hpp */
