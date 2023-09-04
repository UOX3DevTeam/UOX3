#ifndef __CJSMAPPING_H__
#define __CJSMAPPING_H__

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "typedefs.h"

struct JSObject;
class CEnvoke;
class Script;
class cScript;



class CJSMappingSection {
public:
    enum type_t {
        SCPT_NORMAL = 0,
        SCPT_COMMAND,
        SCPT_MAGIC,
        SCPT_SKILLUSE,
        SCPT_PACKET,
        SCPT_CONSOLE,
        SCPT_COUNT
    };
    static const std::vector<std::string> ScriptNames;

  private:
    std::map<std::uint16_t, cScript *> scriptIdMap;
    std::map<JSObject *, std::uint16_t> scriptJSMap;

    std::map<std::uint16_t, cScript *>::iterator scriptIdIter;

    type_t scriptType;

  public:
    CJSMappingSection(type_t sT);
    ~CJSMappingSection();

    auto jsCollection() const -> const std::map<JSObject *, std::uint16_t> & { return scriptJSMap; }
    auto jsCollection() -> std::map<JSObject *, std::uint16_t> & { return scriptJSMap; }
    auto collection() const -> const std::map<std::uint16_t, cScript *> & { return scriptIdMap; }
    auto collection() -> std::map<std::uint16_t, cScript *> & { return scriptIdMap; }

    void Reload(std::uint16_t toLoad);
    void Parse(Script *fileAssocData);

    bool IsInMap(std::uint16_t scriptId);

    std::uint16_t GetScriptId(JSObject *toFind);
    cScript *GetScript(std::uint16_t toFind);
    cScript *GetScript(JSObject *toFind);

    cScript *First(void);
    cScript *Next(void);
    bool Finished(void);
};

class CJSMapping {
  private:
    std::vector<CJSMappingSection *> mapSection ;

    CEnvoke *envokeById;
    CEnvoke *envokeByType;

    void Cleanup(void);
    void Parse(CJSMappingSection::type_t toParse = CJSMappingSection::SCPT_COUNT);

  public:
    CJSMapping(){ mapSection = std::vector<CJSMappingSection *>(CJSMappingSection::ScriptNames.size(),nullptr);}
    ~CJSMapping();
    void ResetDefaults(void);

    void Reload(std::uint16_t scriptId = 0xFFFF);
    void Reload(CJSMappingSection::type_t sectionId);

    CJSMappingSection *GetSection(CJSMappingSection::type_t toGet);

    std::uint16_t GetScriptId(JSObject *toFind);

    cScript *GetScript(std::uint16_t toFind);
    cScript *GetScript(JSObject *toFind);

    CEnvoke *GetEnvokeById(void);
    CEnvoke *GetEnvokeByType(void);
};

class CEnvoke {
  private:
    std::map<std::uint16_t, std::uint16_t> envokeList;
    std::string envokeType;

  public:
    CEnvoke(const std::string &envokeType);
    ~CEnvoke();

    void Parse(void);
    bool Check(std::uint16_t envokeId) const;
    std::uint16_t GetScript(std::uint16_t envokeId) const;
};

extern CJSMapping *JSMapping;

#endif // __CJSMAPPING_H__
