//

#ifndef uoxversion_hpp
#define uoxversion_hpp

#include <cstdint>
#include <iostream>
#include <string>

//======================================================================
struct UOXVersion {
    static const std::string version;
    static const std::string build;
    static const std::string realBuild;
    static const std::string scriptVersion;
    static const std::string characterVersion;
    static const std::string itemVersion;
    static const std::string productName;
    static const std::string name;
    static const std::string product;
    static const std::string email;
    static const std::string programmers;
};

#endif /* uoxversion_hpp */
