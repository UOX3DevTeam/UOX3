#include <cstring>
#include <iostream>

#include "js/CompilationAndEvaluation.h"
#include "js/GlobalObject.h"
#include "js/Initialization.h"
#include "js/Realm.h"
#include "js/RealmOptions.h"
#include "js/RootingAPI.h"
#include "js/SourceText.h"
#include "js/Value.h"
#include "jsapi.h"
#include "mozilla/Utf8.h"

namespace
{
const JSClassOps globalClassOps = {
    nullptr,
    nullptr,
    nullptr,
    JS_NewEnumerateStandardClasses,
    JS_ResolveStandardClass,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    JS_GlobalObjectTraceHook,
};

const JSClass globalClass = {
    "UOX3SmokeGlobal",
    JSCLASS_GLOBAL_FLAGS,
    &globalClassOps,
};
}

extern "C" const char *uox3_mozjs_version();

int main()
{
    if(!JS_Init())
    {
        std::cerr << "JS_Init failed\n";
        return 1;
    }

    JSContext *cx = JS_NewContext(JS::DefaultHeapMaxBytes);
    if(cx == nullptr)
    {
        std::cerr << "JS_NewContext failed\n";
        JS_ShutDown();
        return 1;
    }

    if(!JS::InitSelfHostedCode(cx))
    {
        std::cerr << "JS::InitSelfHostedCode failed\n";
        JS_DestroyContext(cx);
        JS_ShutDown();
        return 1;
    }

    int result = 1;
    {
        JS::RealmOptions realmOptions;
        JS::RootedObject global(
            cx,
            JS_NewGlobalObject(cx, &globalClass, nullptr, JS::FireOnNewGlobalHook, realmOptions));

        if(global != nullptr)
        {
            JSAutoRealm realm(cx, global);
            if(JS::InitRealmStandardClasses(cx))
            {
                constexpr char script[] = "21 * 2";
                JS::SourceText<mozilla::Utf8Unit> source;
                JS::CompileOptions options(cx);
                options.setFileAndLine("mozjs_smoke.js", 1);
                JS::RootedValue value(cx);

                if(source.init(cx, script, std::strlen(script), JS::SourceOwnership::Borrowed) &&
                   JS::Evaluate(cx, options, source, &value) && value.isInt32() &&
                   value.toInt32() == 42)
                {
                    std::cout << "mozjs " << uox3_mozjs_version()
                              << " evaluated 21 * 2 = " << value.toInt32() << '\n';
                    result = 0;
                }
                else
                {
                    std::cerr << "JavaScript evaluation failed\n";
                }
            }
            else
            {
                std::cerr << "JS::InitRealmStandardClasses failed\n";
            }
        }
        else
        {
            std::cerr << "JS_NewGlobalObject failed\n";
        }
    }

    JS_DestroyContext(cx);
    JS_ShutDown();
    return result;
}
