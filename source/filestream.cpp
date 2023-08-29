//

#include "filestream.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

using namespace std::string_literals ;
auto saveFutures =std::vector<std::future<void>>() ;
//======================================================================
auto waitOnAllFuture() -> void {
    for (auto &future:saveFutures){
        future.wait() ;
    }
    saveFutures = std::vector<std::future<void>>();
}
//======================================================================
auto queueStream(basestream *stream) ->void {
    saveFutures.push_back(std::async(std::launch::async,&saveStream,stream)) ;
}
//======================================================================
auto saveStream(basestream *stream) ->void {
    stream->save();
    delete stream ;
}
//======================================================================
auto ObjectStream::save() const ->void {
    auto output = std::ofstream(path.string());
    std::for_each(contents.begin(),contents.end(),[&output](const ObjectDescription &entry){
        std::for_each(entry.description.begin(), entry.description.end(), [&output](const std::pair<std::string,std::string> &values){
            output<<values.first<<values.second<<"\n" ;
        });
    });
}
