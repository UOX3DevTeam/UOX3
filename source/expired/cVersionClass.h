// o------------------------------------------------------------------------------------------------o
//|	File		-	cVersionClass.h
//|	Date		-	10/17/2002
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-
// o------------------------------------------------------------------------------------------------o
#ifndef __CVERSIONCLASS_H__
#define __CVERSIONCLASS_H__

class CVersionClass {
  public:
    CVersionClass();
    ~CVersionClass();
    static std::string GetVersion();
    static std::string GetBuild();
    static std::string GetRealBuild();
    static std::string GetScriptVersion();
    static std::string GetCharacterVersion();
    static std::string GetItemVersion();
    static std::string GetProductName();
    static std::string GetName();
    static std::string GetEmail();
    static std::string GetProgrammers();
};

#endif // __CVERSIONCLASS_H__
