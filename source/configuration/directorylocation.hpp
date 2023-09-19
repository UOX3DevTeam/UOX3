//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef directorylocation_hpp
#define directorylocation_hpp

#include <cstdint>
#include <iostream>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

//======================================================================
// We make this outside the struct, as we are going to share, and hopefully makes more sense?
enum class dirlocation_t {
    BASE=0,UODIR,DEFINITION,BOOK,
    ACCOUNT,SCRIPT,SCRIPTDATA,BACKUP,
    MSGBOARD,SAVE,ACCESS,HTML,
    LOG,LANGUAGE
};
//======================================================================
class DirectoryLocation {
    static const std::map<std::string, dirlocation_t> LOCATIONNAMEMAP ;
    std::vector<std::filesystem::path>  locations ;
    auto makePath(const std::filesystem::path &path) const -> std::filesystem::path ;
    auto normalizePaths() ->void ;
    auto checkPaths() ->void ;
    auto checkCreatePath(const std::filesystem::path &path, const std::string &name,bool create) ;
public:
    DirectoryLocation() ;
    auto reset() ->void ;
    auto setLocation(const std::string &key, const std::string &value) ->bool ;
    auto postLoad() ->void ;
    auto operator[](dirlocation_t location) const -> const std::filesystem::path& ;
    auto operator[](dirlocation_t location) -> std::filesystem::path& ;
    auto dumpPaths() const -> std::vector<std::pair<std::string,std::string>> ;
    //auto dumpPaths() const ->void ;
    auto unwindPath(dirlocation_t location) const -> std::filesystem::path;
    auto valueFor(const std::string &keyword) const ->std::optional<std::string> ;
    
};

#endif /* directorylocation_hpp */
