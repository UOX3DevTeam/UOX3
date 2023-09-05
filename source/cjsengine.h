#ifndef __CJSENGINE_H__
#define __CJSENGINE_H__

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "spidermonkey.h"
#include "typedefs.h"

struct JSObject;

enum iueentries_t {
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

enum jsprototypes_t {
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

    JSObject *FindAssociatedObject(iueentries_t iType, void *index);
    JSObject *MakeNewObject(iueentries_t iType);

    void Cleanup();
    void InitializePrototypes();

  public:
    CJSRuntime();
    CJSRuntime(std::uint32_t engineSize);
    ~CJSRuntime();

    void Reload();
    void CollectGarbage();

    JSRuntime *GetRuntime() const;
    JSContext *GetContext() const;
    JSObject *GetObject() const;

    JSObject *GetPrototype(jsprototypes_t protoNum) const;

    JSObject *AcquireObject(iueentries_t iType, void *index);
    void ReleaseObject(iueentries_t IType, void *index);
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

    auto startup() -> void;

    JSRuntime *GetRuntime(std::uint8_t runTime) const;
    JSContext *GetContext(std::uint8_t runTime) const;
    JSObject *GetObject(std::uint8_t runTime) const;

    std::uint8_t FindActiveRuntime(JSRuntime *rT) const;

    JSObject *GetPrototype(std::uint8_t runTime, jsprototypes_t protoNum) const;

    void Reload();
    void CollectGarbage();

    JSObject *AcquireObject(iueentries_t iType, void *index, std::uint8_t runTime);
    void ReleaseObject(iueentries_t IType, void *index);
};

extern CJSEngine *JSEngine;

#endif
