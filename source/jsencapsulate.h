/* Include all the standard header *after* all the configuration
 settings have been made.
 */
#include <cstdint>
#include <string>

#include "spidermonkey.h"
#include "typedefs.h"

struct JSContext;
struct JSObject;

/** In order to avoid finger-aches :)
 */
#include "typedefs.h"

class JSEncapsulate {
  public:
    enum type_t {
        JSOT_INT = 0,
        JSOT_DOUBLE,
        JSOT_BOOL,
        JSOT_STRING,
        JSOT_OBJECT,
        JSOT_NULL,
        JSOT_VOID,
        JSOT_COUNT
    };

    JSEncapsulate(JSContext *jsCX, jsval *jsVP);
    JSEncapsulate(JSContext *jsCX, JSObject *jsVP);
    JSEncapsulate();
    void SetContext(JSContext *jsCX, jsval *jsVP);
    bool isType(JSEncapsulate::type_t toCheck);
    std::int32_t toInt();
    bool toBool();
    R32 toFloat();
    std::string toString();
    void *toObject();

    std::string ClassName();

  private:
    void InternalReset();
    void Init();
    bool beenParsed[JSOT_COUNT + 1];
    JSEncapsulate::type_t nativeType;

    std::int32_t intVal;
    R32 floatVal;
    bool boolVal;
    std::string stringVal;
    void *objectVal;

    JSContext *cx;
    jsval *vp;
    JSObject *obj;

    std::string className;
    bool classCached;

    void Parse(JSEncapsulate::type_t typeConvert);
};
