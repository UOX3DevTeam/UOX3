// o-------------------------------------------------------------------------------------------------o
//|	File		-	chtmlsystem.h
// o-------------------------------------------------------------------------------------------------o
//|	Purpose		-	Interface for the cHTMLSystem class
// o-------------------------------------------------------------------------------------------------o

#ifndef __CHTMLSYSTEM_H__
#define __CHTMLSYSTEM_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CScriptSection;

enum ETemplateType {
    ETT_ALLTEMPLATES = -1,
    ETT_GMSTATUS = 0,
    ETT_GUILD,
    ETT_OFFLINE,
    ETT_PLAYER,
    ETT_ONLINE,
    ETT_INVALIDTEMPLATE = 0xFF
};

class cHTMLTemplate {
private:
    std::uint32_t updateTimer;
    std::string inputFile;
    bool loaded;
    ETemplateType type;
    std::string content;
    std::string outputFile;
    std::string name;
    std::uint32_t scheduledUpdate;
    
public:
    cHTMLTemplate();
    ~cHTMLTemplate();
    void Process();
    void Poll();
    void LoadTemplate();
    void UnloadTemplate();
    void Load(CScriptSection *found);
    
    // Some Getters
    std::string GetName() const;
    std::string GetOutput() const;
    std::string GetInput() const;
    ETemplateType GetTemplateType() const;
    std::uint32_t GetScheduledUpdate() const;
    std::uint32_t GetUpdateTimer() const;
};

class cHTMLTemplates {
private:
    std::vector<cHTMLTemplate *> Templates;
    
public:
    cHTMLTemplates() = default;
    ~cHTMLTemplates();
    
    void Load();
    void Unload();
    void Poll(ETemplateType nTemplateId = ETT_ALLTEMPLATES);
    void TemplateInfoGump(CSocket *mySocket);
};

extern cHTMLTemplates *HTMLTemplates;

#endif
