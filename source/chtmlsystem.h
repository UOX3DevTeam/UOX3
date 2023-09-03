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
    void Process(void);
    void Poll(void);
    void LoadTemplate(void);
    void UnloadTemplate(void);
    void Load(CScriptSection *found);

    // Some Getters
    std::string GetName(void) const;
    std::string GetOutput(void) const;
    std::string GetInput(void) const;
    ETemplateType GetTemplateType(void) const;
    std::uint32_t GetScheduledUpdate(void) const;
    std::uint32_t GetUpdateTimer(void) const;
};

class cHTMLTemplates {
  private:
    std::vector<cHTMLTemplate *> Templates;

  public:
    cHTMLTemplates() = default;
    ~cHTMLTemplates();

    void Load(void);
    void Unload(void);
    void Poll(ETemplateType nTemplateId = ETT_ALLTEMPLATES);
    void TemplateInfoGump(CSocket *mySocket);
};

extern cHTMLTemplates *HTMLTemplates;

#endif
