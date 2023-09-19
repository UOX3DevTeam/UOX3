//Copyright © 2023 Charles Kerr. All rights reserved.

#include "directorylocation.hpp"

#include <cstdlib>
#include <stdexcept>

#include "utility/strutil.hpp"

using namespace std::string_literals ;
//======================================================================
const std::map<std::string, dirlocation_t> DirectoryLocation::LOCATIONNAMEMAP{
    {"DIRECTORY"s,dirlocation_t::BASE},{"DATADIRECTORY"s,dirlocation_t::UODIR},
    {"DEFSDIRECTORY"s,dirlocation_t::DEFINITION},{"BOOKSDIRECTORY"s,dirlocation_t::BOOK},
    {"ACTSDIRECTORY"s,dirlocation_t::ACCOUNT},{"SCRIPTSDIRECTORY"s,dirlocation_t::SCRIPT},
    {"SCRIPTDATADIRECTORY"s,dirlocation_t::SCRIPTDATA},{"BACKUPDIRECTORY"s,dirlocation_t::BACKUP},
    {"MSGBOARDDIRECTORY"s,dirlocation_t::MSGBOARD},{"SHAREDDIRECTORY"s,dirlocation_t::SAVE},
    {"ACCESSDIRECTORY"s,dirlocation_t::ACCESS},{"HTMLDIRECTORY"s,dirlocation_t::HTML},
    {"LOGSDIRECTORY"s,dirlocation_t::LOG},{"DICTIONARYDIRECTORY"s,dirlocation_t::LANGUAGE},
};
//======================================================================
auto DirectoryLocation::makePath(const std::filesystem::path &path) const -> std::filesystem::path{
    auto path_to_check = std::filesystem::path("./").make_preferred() ;
    if (!path.is_absolute() && locations.at(static_cast<int>(dirlocation_t::BASE)) != path_to_check && !locations.at(static_cast<int>(dirlocation_t::BASE)).empty()){
        return locations.at(static_cast<int>(dirlocation_t::BASE)) / path ;
    }
    return path ;
}
//======================================================================
auto DirectoryLocation::checkCreatePath(const std::filesystem::path &path, const std::string &name,bool create) {
    const auto formatError = "Directory for %s does not exist: %s"s ;
    const auto formatCreate = "Directory for %s does not exist, creating: %s"s ;
    if (path.empty()) {
        std::cerr << "Path was empty: " << path.string() << std::endl;
        exit(1);
    }
    if (!std::filesystem::exists(path)){
        if (!create){
            throw std::runtime_error(util::format(formatError,path.string().c_str()));
        }
        else {
            //std::cout << "Path is: " << path.string() << std::endl;
            std::clog << util::format(formatCreate,name.c_str(),path.string().c_str()) << std::endl;
            std::filesystem::create_directories(path);
        }
    }

}
//======================================================================
auto DirectoryLocation::normalizePaths() ->void {
    // Normalize all the directories
    
    locations.at(static_cast<int>(dirlocation_t::DEFINITION)) = makePath(locations.at(static_cast<int>(dirlocation_t::DEFINITION)) );
    locations.at(static_cast<int>(dirlocation_t::BOOK)) = makePath(locations.at(static_cast<int>(dirlocation_t::BOOK)))  ;
    locations.at(static_cast<int>(dirlocation_t::ACCOUNT)) = makePath(locations.at(static_cast<int>(dirlocation_t::ACCOUNT)))  ;
    locations.at(static_cast<int>(dirlocation_t::SCRIPT)) = makePath(locations.at(static_cast<int>(dirlocation_t::SCRIPT)))  ;
    locations.at(static_cast<int>(dirlocation_t::SCRIPTDATA)) = makePath(locations.at(static_cast<int>(dirlocation_t::SCRIPTDATA)))  ;
    locations.at(static_cast<int>(dirlocation_t::BACKUP)) = makePath(locations.at(static_cast<int>(dirlocation_t::BACKUP)))  ;
    locations.at(static_cast<int>(dirlocation_t::MSGBOARD)) = makePath(locations.at(static_cast<int>(dirlocation_t::MSGBOARD)))  ;
    locations.at(static_cast<int>(dirlocation_t::SAVE)) = makePath(locations.at(static_cast<int>(dirlocation_t::SAVE)))  ;
    locations.at(static_cast<int>(dirlocation_t::ACCESS)) = makePath(locations.at(static_cast<int>(dirlocation_t::ACCESS)))  ;
    locations.at(static_cast<int>(dirlocation_t::HTML)) = makePath(locations.at(static_cast<int>(dirlocation_t::HTML)))  ;
    locations.at(static_cast<int>(dirlocation_t::LOG)) = makePath(locations.at(static_cast<int>(dirlocation_t::LOG)))  ;
    locations.at(static_cast<int>(dirlocation_t::LANGUAGE)) = makePath(locations.at(static_cast<int>(dirlocation_t::LANGUAGE)))  ;
}

//======================================================================
auto DirectoryLocation::checkPaths() ->void {
    // check the paths that we do not create
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::UODIR)), "UO data", false);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::DEFINITION)), "UOX3 definitions", false);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::SCRIPT)), "UOX3 script", false);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::LANGUAGE)), "UOX3 language/dictionaries", false);
    
    // Now we create everything else
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::BOOK)), "UOX3 book", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::SCRIPTDATA)), "UOX3 script data", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::MSGBOARD)), "UOX3 msgboard", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::SAVE)), "UOX3 save/shared", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::ACCOUNT)), "UOX3 account", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::ACCESS)), "UOX3 access", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::HTML)), "UOX3 html", true);
    checkCreatePath(locations.at(static_cast<int>(dirlocation_t::LOG)), "UOX3 log", true);
}
//======================================================================
DirectoryLocation::DirectoryLocation() {
    reset() ;
}
//======================================================================
auto DirectoryLocation::reset() ->void {
    locations = std::vector<std::filesystem::path>(LOCATIONNAMEMAP.size());
    locations.at(static_cast<int>(dirlocation_t::BASE)) = std::filesystem::current_path() ;
#if defined(_WIN32)
    locations.at(static_cast<int>(dirlocation_t::UODIR))=std::filesystem::path("C:\\Program Files (x86)\\Electronic Arts\\Ultima Online Classic\\");
    auto potential = std::getenv("UODATA") ;
    if (potential != nullptr){
        locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path(potential) ;
    }
#else
    locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path("uodata/");
    auto potential = std::getenv("UODATA") ;
    if (potential != nullptr){
        locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path(potential) ;
    }
    else {
        potential = std::getenv("HOME") ;
        if (potential != nullptr){
            locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path(potential) / std::filesystem::path("uodata/") ;
        }
        
    }
#endif
    locations.at(static_cast<int>(dirlocation_t::DEFINITION))=std::filesystem::path("dfndata/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::BOOK))=std::filesystem::path("books/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::ACCOUNT))=std::filesystem::path("accounts/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::SCRIPT))=std::filesystem::path("js/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::SCRIPTDATA))=locations.at(static_cast<int>(dirlocation_t::SCRIPT))/ std::filesystem::path("jsdata/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::BACKUP))=std::filesystem::path("archives/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::MSGBOARD))=std::filesystem::path("msgboards/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::SAVE))=std::filesystem::path("shared/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::ACCESS))=std::filesystem::path("accounts/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::HTML))=std::filesystem::path("html/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::LOG))=std::filesystem::path("logs/").make_preferred() ;
    locations.at(static_cast<int>(dirlocation_t::LANGUAGE))=std::filesystem::path("dictionaries/").make_preferred() ;
}
//======================================================================
auto DirectoryLocation::setLocation(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = LOCATIONNAMEMAP.find(key) ;
    if (iter != LOCATIONNAMEMAP.end()){
        rvalue = true ;
        locations.at(static_cast<int>(iter->second)) = std::filesystem::path(value).make_preferred() ;
    }
    return rvalue ;
}
//======================================================================
auto DirectoryLocation::postLoad() ->void {
     // First , normalize the paths
    this->normalizePaths();
    // Now check them
    this->checkPaths() ;
}
//======================================================================
auto DirectoryLocation::operator[](dirlocation_t location) const -> const std::filesystem::path& {
    try {
        return locations.at(static_cast<int>(location));
    }
    catch( const std::out_of_range &e){
        throw std::out_of_range("Request for directory index: "s+std::to_string(static_cast<int>(location))+ " error: "s + e.what());
    }
}
//======================================================================
auto DirectoryLocation::operator[](dirlocation_t location) -> std::filesystem::path& {
    try {
        return locations.at(static_cast<int>(location));
    }
    catch( const std::out_of_range &e){
        throw std::out_of_range("Request for directory index: "s+std::to_string(static_cast<int>(location))+ " error: "s + e.what());
    }
}
//======================================================================
auto DirectoryLocation::dumpPaths() const -> std::vector<std::pair<std::string,std::string>> {
    // We are going to dump them in a special order, so not as simple as we wanted
    auto rvalue = std::vector<std::pair<std::string,std::string>>();
    auto specialFind = [this](dirlocation_t dir) -> std::pair<std::string,std::string>{
        auto iter = std::find_if(DirectoryLocation::LOCATIONNAMEMAP.begin(), DirectoryLocation::LOCATIONNAMEMAP.end(), [dir](const std::pair<std::string,dirlocation_t> &entry){
            return dir == entry.second ;
        });
        return std::make_pair(iter->first, this->unwindPath(dir).string());
    };
    rvalue.push_back(specialFind(dirlocation_t::BASE));
    rvalue.push_back(specialFind(dirlocation_t::UODIR));
    rvalue.push_back(specialFind(dirlocation_t::DEFINITION));
    rvalue.push_back(specialFind(dirlocation_t::BOOK));
    rvalue.push_back(specialFind(dirlocation_t::ACCOUNT));
    rvalue.push_back(specialFind(dirlocation_t::SCRIPT));
    rvalue.push_back(specialFind(dirlocation_t::SCRIPTDATA));
    rvalue.push_back(specialFind(dirlocation_t::BACKUP));
    rvalue.push_back(specialFind(dirlocation_t::MSGBOARD));
    rvalue.push_back(specialFind(dirlocation_t::SAVE));
    rvalue.push_back(specialFind(dirlocation_t::ACCESS));
    rvalue.push_back(specialFind(dirlocation_t::HTML));
    rvalue.push_back(specialFind(dirlocation_t::LOG));
    rvalue.push_back(specialFind(dirlocation_t::LANGUAGE));
    return rvalue ;
}

//======================================================================
auto DirectoryLocation::unwindPath(dirlocation_t location) const -> std::filesystem::path {
    if (location == dirlocation_t::BASE || location == dirlocation_t::UODIR){
        return this->locations.at(static_cast<int>(location));
    }
    auto basepath = this->locations.at(static_cast<int>(dirlocation_t::BASE)).string() ;
    auto path = this->locations.at(static_cast<int>(location)).string() ;
    if (path.find(basepath) == 0){
        path = path.substr(basepath.size()) ;
        auto temp = std::filesystem::path(path).make_preferred() ;
        if (temp.is_absolute()){
            path = path.substr(1) ;
        }
    }
    return std::filesystem::path(path) ;
}
//======================================================================
auto DirectoryLocation::valueFor(const std::string &keyword) const ->std::optional<std::string> {
    auto iter = LOCATIONNAMEMAP.find(keyword);
    if (iter != LOCATIONNAMEMAP.end()){
        auto value =  unwindPath(iter->second).string() ;
        if ( !value.empty()) {
            if (value[value.size()-1] != '/' || value[value.size()-1] != '\\'){
                value += "/" ;
            }
        }
        return value ;
    }
    return {};
}
