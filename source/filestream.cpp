//

#include "filestream.hpp"
#include <stdexcept>
#include <fstream>

using namespace std::string_literals ;
auto saveFutures = std::vector<std::future<void>>() ;
//======================================================================
auto saveStream(filestream *stream) ->void {
    auto output = std::ofstream(stream->path.string());
    if (output.is_open()) {
        for (const auto &entry:stream->contents){
            for (const auto &[key,value]:entry){
                output<<key<<value<<"\n";
            }
        }
    }
    delete stream ;
}
//======================================================================
auto waitOnAllFuture() -> void {
    for (auto &future:saveFutures){
        future.wait() ;
    }
    saveFutures = std::vector<std::future<void>>();
}
//======================================================================
auto startSave(filestream *stream) ->void {
    saveFutures.push_back(std::async(std::launch::async,&saveStream,stream)) ;
}
