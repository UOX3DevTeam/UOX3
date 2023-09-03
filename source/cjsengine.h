#ifndef __CJSENGINE_H__
#define __CJSENGINE_H__

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "spidermonkey.h"
#include "typedefs.h"

struct JSObject;

enum IUEEntries {
    IUE_RACE = 0,
    IUE_CHAR,
    IUE_ITEM,
    IUE_SOCK,
    IUE_GUILD,
    IUE_REGION,
    IUE_SPAWNREGION,
    IUE_PARTY,
    IUE_ACCOUNT,
    IUE_COUNT
};

enum JSPrototypes {
    JSP_ITEM = 0,
    JSP_CHAR,
    JSP_SOCK,
    JSP_GUMP,
    JSP_PACKET,
    JSP_GUILD,
    JSP_RACE,
    JSP_REGION,
    JSP_SPAWNREGION,
    JSP_SPELL,
    JSP_SPELLS,
    JSP_GLOBALSKILL,
    JSP_GLOBALSKILLS,
    JSP_RESOURCE,
    JSP_ACCOUNT,
    JSP_ACCOUNTS,
    JSP_CONSOLE,
    JSP_FILE,
    JSP_PARTY,
    JSP_CREATEENTRY,
    JSP_CREATEENTRIES,
    JSP_TIMER,
    JSP_SCRIPT,
    JSP_COUNT
};

class CJSRuntime {
  private:
    typedef std::map<void *, JSObject *> JSOBJECTMAP;
    typedef std::map<void *, JSObject *>::iterator JSOBJECTMAP_ITERATOR;
    typedef std::map<void *, JSObject *>::const_iterator JSOBJECTMAP_CITERATOR;

    std::vector<JSOBJECTMAP> objectList;
    std::vector<JSObject *> protoList;

    JSObject *spellsObj;
    JSObject *skillsObj;
    JSObject *accountsObj;
    JSObject *consoleObj;
    JSObject *createEntriesObj;
    JSObject *timerObj;
    JSObject *scriptObj;
    JSRuntime *jsRuntime;
    JSContext *jsContext;
    JSObject *jsGlobal;

    JSObject *FindAssociatedObject(IUEEntries iType, void *index);
    JSObject *MakeNewObject(IUEEntries iType);

    void Cleanup(void);
    void InitializePrototypes(void);

  public:
    CJSRuntime();
    CJSRuntime(std::uint32_t engineSize);
    ~CJSRuntime();

    void Reload();
    void CollectGarbage();

    JSRuntime *GetRuntime() const;
    JSContext *GetContext() const;
    JSObject *GetObject() const;

    JSObject *GetPrototype(JSPrototypes protoNum) const;

    JSObject *AcquireObject(IUEEntries iType, void *index);
    void ReleaseObject(IUEEntries IType, void *index);
};

class CJSEngine {
  private:
    typedef std::vector<CJSRuntime *> RUNTIMELIST;
    typedef std::vector<CJSRuntime *>::iterator RUNTIMELIST_ITERATOR;
    typedef std::vector<CJSRuntime *>::const_iterator RUNTIMELIST_CITERATOR;

    RUNTIMELIST runtimeList;

  public:
    CJSEngine() = default;
    ~CJSEngine();

    auto Startup() -> void;

    JSRuntime *GetRuntime(std::uint8_t runTime) const;
    JSContext *GetContext(std::uint8_t runTime) const;
    JSObject *GetObject(std::uint8_t runTime) const;

    std::uint8_t FindActiveRuntime(JSRuntime *rT) const;

    JSObject *GetPrototype(std::uint8_t runTime, JSPrototypes protoNum) const;

    void Reload(void);
    void CollectGarbage(void);

    JSObject *AcquireObject(IUEEntries iType, void *index, std::uint8_t runTime);
    void ReleaseObject(IUEEntries IType, void *index);
};

extern CJSEngine *JSEngine;

#endif
