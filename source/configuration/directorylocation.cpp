//Copyright © 2023 Charles Kerr. All rights reserved.

#include "directorylocation.hpp"

#include <cstdlib>
#include <stdexcept>

#include "subsystem/console.hpp"
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
    if (!path.is_absolute() && locations.at(static_cast<int>(dirlocation_t::BASE)) != path_to_check){
        return locations.at(static_cast<int>(dirlocation_t::BASE)) / path ;
    }
    return path ;
}
//======================================================================
auto DirectoryLocation::checkCreatePath(const std::filesystem::path &path, const std::string &name,bool create) {
    const auto formatError = "Directory for %s does not exist: %s"s ;
    const auto formatCreate = "Directory for %s does not exist, creating: %s"s ;

    if (!std::filesystem::exists(path)){
        if (!create){
            throw std::runtime_error(util::format(formatError,path.string().c_str()));
        }
        else {
            std::clog << util::format(formatCreate,path.string().c_str()) << std::endl;
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
    locations.at(static_cast<int>(dirlocation_t::UODIR))=std::filesystem::path("C:\\Program Files (x86)\\Electronic Arts\\Ultima Online Classic");
    auto potential = std::getenv("UODATA") ;
    if (potential != nullptr){
        locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path(potential) ;
    }
#else
    locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path("uodata");
    auto potential = std::getenv("UODATA") ;
    if (potential != nullptr){
        locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path(potential) ;
    }
    else {
        potential = std::getenv("HOME") ;
        if (potential != nullptr){
            locations.at(static_cast<int>(dirlocation_t::UODIR)) = std::filesystem::path(potential) / std::filesystem::path("uodata") ;
        }
        
    }
#endif
    locations.at(static_cast<int>(dirlocation_t::DEFINITION))=std::filesystem::path("dfndata") ;
    locations.at(static_cast<int>(dirlocation_t::BOOK))=std::filesystem::path("books") ;
    locations.at(static_cast<int>(dirlocation_t::ACCOUNT))=std::filesystem::path("accounts") ;
    locations.at(static_cast<int>(dirlocation_t::SCRIPT))=std::filesystem::path("js") ;
    locations.at(static_cast<int>(dirlocation_t::SCRIPTDATA))=locations.at(static_cast<int>(dirlocation_t::SCRIPT))/ std::filesystem::path("jsdata") ;
    locations.at(static_cast<int>(dirlocation_t::BACKUP))=std::filesystem::path("archives") ;
    locations.at(static_cast<int>(dirlocation_t::MSGBOARD))=std::filesystem::path("msgboards") ;
    locations.at(static_cast<int>(dirlocation_t::SAVE))=std::filesystem::path("shared") ;
    locations.at(static_cast<int>(dirlocation_t::ACCESS))=std::filesystem::path("accounts") ;
    locations.at(static_cast<int>(dirlocation_t::HTML))=std::filesystem::path("html") ;
    locations.at(static_cast<int>(dirlocation_t::LOG))=std::filesystem::path("logs") ;
    locations.at(static_cast<int>(dirlocation_t::LANGUAGE))=std::filesystem::path("dictionaries") ;
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
auto DirectoryLocation::dumpPaths() const ->void {
    Console::shared().printSectionBegin();
    Console::shared() << "PathDump: \n";
    Console::shared() << "    Root        : " << locations.at(static_cast<int>(dirlocation_t::BASE)).string() << "\n";
    Console::shared() << "    Accounts    : " << locations.at(static_cast<int>(dirlocation_t::ACCOUNT)).string() << "\n";
    Console::shared() << "    Access      : " << locations.at(static_cast<int>(dirlocation_t::ACCESS)).string() << "\n";
    Console::shared() << "    Mul(Data)   : " << locations.at(static_cast<int>(dirlocation_t::UODIR)).string() << "\n";
    Console::shared() << "    DFN(Defs)   : " << locations.at(static_cast<int>(dirlocation_t::DEFINITION)).string() << "\n";
    Console::shared() << "    JScript     : " << locations.at(static_cast<int>(dirlocation_t::SCRIPT)).string() << "\n";
    Console::shared() << "    JScriptData : " << locations.at(static_cast<int>(dirlocation_t::SCRIPTDATA)).string() << "\n";
    Console::shared() << "    HTML        : " << locations.at(static_cast<int>(dirlocation_t::HTML)).string() << "\n";
    Console::shared() << "    MSGBoards   : " << locations.at(static_cast<int>(dirlocation_t::MSGBOARD)).string() << "\n";
    Console::shared() << "    Books       : " << locations.at(static_cast<int>(dirlocation_t::BOOK)).string() << "\n";
    Console::shared() << "    Shared      : " << locations.at(static_cast<int>(dirlocation_t::SAVE)).string() << "\n";
    Console::shared() << "    Backups     : " << locations.at(static_cast<int>(dirlocation_t::BACKUP)).string() << "\n";
    Console::shared() << "    Logs        : " << locations.at(static_cast<int>(dirlocation_t::LOG)).string() << "\n";
    Console::shared().printSectionBegin();
}
