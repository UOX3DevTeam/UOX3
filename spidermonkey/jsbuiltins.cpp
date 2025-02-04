/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sw=4 et tw=99:
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla SpiderMonkey JavaScript 1.9 code, released
 * May 28, 2008.
 *
 * The Initial Developer of the Original Code is
 *   Andreas Gal <gal@mozilla.com>
 *
 * Contributor(s):
 *   Brendan Eich <brendan@mozilla.org>
 *   Mike Shaver <shaver@mozilla.org>
 *   David Anderson <danderson@mozilla.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "jsstddef.h"
#include <math.h>

#include "jsapi.h"
#include "jsarray.h"
#include "jsbool.h"
#include "jscntxt.h"
#include "jsgc.h"
#include "jsiter.h"
#include "jslibmath.h"
#include "jsmath.h"
#include "jsnum.h"
#include "prmjtime.h"
#include "jsscope.h"
#include "jsstr.h"
#include "jstracer.h"

#include "nanojit/avmplus.h"
#include "nanojit/nanojit.h"

using namespace avmplus;
using namespace nanojit;

/*
 * NB: bool FASTCALL is not compatible with Nanojit's calling convention usage.
 * Do not use bool FASTCALL, use JSBool only!
 */

jsdouble FASTCALL
js_dmod(jsdouble a, jsdouble b)
{
    if (b == 0.0) {
        jsdpun u;
        u.s.hi = JSDOUBLE_HI32_EXPMASK | JSDOUBLE_HI32_MANTMASK;
        u.s.lo = 0xffffffff;
        return u.d;
    }
    jsdouble r;
#ifdef XP_WIN
    /* Workaround MS fmod bug where 42 % (1/0) => NaN, not 42. */
    if (JSDOUBLE_IS_FINITE(a) && JSDOUBLE_IS_INFINITE(b))
        r = a;
    else
#endif
        r = fmod(a, b);
    return r;
}

jsint FASTCALL
js_imod(jsint a, jsint b)
{
    if (a < 0 || b <= 0)
        return -1;
    int r = a % b;
    return r;
    #define JSVAL_ERROR_COOKIE ((jsval)0xBAD)
}

/* The following boxing/unboxing primitives we can't emit inline because
   they either interact with the GC and depend on Spidermonkey's 32-bit
   integer representation. */

jsval FASTCALL
js_BoxDouble(JSContext* cx, jsdouble d)
{
    jsint i;
    if (JSDOUBLE_IS_INT(d, i))
        return INT_TO_JSVAL(i);
    JS_ASSERT(JS_ON_TRACE(cx));
    jsval v; /* not rooted but ok here because we know GC won't run */
    if (!js_NewDoubleInRootedValue(cx, d, &v))
        return JSVAL_ERROR_COOKIE;
    return v;
}

jsval FASTCALL
js_BoxInt32(JSContext* cx, jsint i)
{
    if (JS_LIKELY(INT_FITS_IN_JSVAL(i)))
        return INT_TO_JSVAL(i);
    JS_ASSERT(JS_ON_TRACE(cx));
    jsval v; /* not rooted but ok here because we know GC won't run */
    jsdouble d = (jsdouble)i;
    if (!js_NewDoubleInRootedValue(cx, d, &v))
        return JSVAL_ERROR_COOKIE;
    return v;
} 

jsdouble FASTCALL
js_UnboxDouble(jsval v)
{
    if (JS_LIKELY(JSVAL_IS_INT(v)))
        return (jsdouble)JSVAL_TO_INT(v);
    return *JSVAL_TO_DOUBLE(v);
}

jsint FASTCALL
js_UnboxInt32(jsval v)
{
    if (JS_LIKELY(JSVAL_IS_INT(v)))
        return JSVAL_TO_INT(v);
    return js_DoubleToECMAInt32(*JSVAL_TO_DOUBLE(v));
}

int32 FASTCALL
js_DoubleToInt32(jsdouble d)
{
    return js_DoubleToECMAInt32(d);
}

int32 FASTCALL
js_DoubleToUint32(jsdouble d)
{
    return js_DoubleToECMAUint32(d);
}

jsdouble FASTCALL
js_Math_sin(jsdouble d)
{
    return sin(d);
}

jsdouble FASTCALL
js_Math_cos(jsdouble d)
{
    return cos(d);
}

jsdouble FASTCALL
js_Math_floor(jsdouble d)
{
    return floor(d);
}

jsdouble FASTCALL
js_Math_ceil(jsdouble d)
{
    return ceil(d);
}

extern jsdouble js_NaN;

jsdouble FASTCALL
js_Math_pow(jsdouble d, jsdouble p)
{
    if (!JSDOUBLE_IS_FINITE(p) && (d == 1.0 || d == -1.0))
        return js_NaN;
    if (p == 0)
        return 1.0;
    return pow(d, p);
}

jsdouble FASTCALL
js_Math_sqrt(jsdouble d)
{
    return sqrt(d);
}

jsdouble FASTCALL
js_Math_log(jsdouble d)
{
#if !JS_USE_FDLIBM_MATH && defined(SOLARIS) && defined(__GNUC__)
    if (d < 0)
        return js_NaN;
#endif
    return log(d);
}

jsdouble FASTCALL
js_Math_max(jsdouble d, jsdouble p)
{
    if (JSDOUBLE_IS_NaN(d) || JSDOUBLE_IS_NaN(p))
        return js_NaN;

    if (p == 0 && p == d && fd_copysign(1.0, d) == -1)
        return p;
    return (d > p) ? d : p;
}

JSBool FASTCALL
js_Array_dense_setelem(JSContext* cx, JSObject* obj, jsint i, jsval v)
{
    JS_ASSERT(OBJ_IS_DENSE_ARRAY(cx, obj));

    jsuint length = ARRAY_DENSE_LENGTH(obj);
    if ((jsuint)i < length) {
        if (obj->dslots[i] == JSVAL_HOLE) {
            if (i >= obj->fslots[JSSLOT_ARRAY_LENGTH])
                obj->fslots[JSSLOT_ARRAY_LENGTH] = i + 1;
            obj->fslots[JSSLOT_ARRAY_COUNT]++;
        }
        obj->dslots[i] = v;
        return JS_TRUE;
    }
    return OBJ_SET_PROPERTY(cx, obj, INT_TO_JSID(i), &v);
}

JSString* FASTCALL
js_Array_p_join(JSContext* cx, JSObject* obj, JSString *str)
{
    jsval v;
    if (!js_array_join_sub(cx, obj, TO_STRING, str, &v))
        return NULL;
    JS_ASSERT(JSVAL_IS_STRING(v));
    return JSVAL_TO_STRING(v);
}

jsval FASTCALL
js_Array_p_push1(JSContext* cx, JSObject* obj, jsval v)
{
    if (!(OBJ_IS_DENSE_ARRAY(cx, obj) 
          ? js_array_push1_dense(cx, obj, v, &v)
          : js_array_push_slowly(cx, obj, 1, &v, &v))) {
        return JSVAL_ERROR_COOKIE;
    }
    return v;
}

jsval FASTCALL
js_Array_p_pop(JSContext* cx, JSObject* obj)
{
    jsval v;
    if (!(OBJ_IS_DENSE_ARRAY(cx, obj) 
          ? js_array_pop_dense(cx, obj, &v)
          : js_array_pop_slowly(cx, obj, &v))) {
        return JSVAL_ERROR_COOKIE;
    }
    return v;
}

JSString* FASTCALL
js_String_p_substring(JSContext* cx, JSString* str, jsint begin, jsint end)
{
    JS_ASSERT(end >= begin);
    JS_ASSERT(JS_ON_TRACE(cx));
    return js_NewDependentString(cx, str, (size_t)begin, (size_t)(end - begin));
}

JSString* FASTCALL
js_String_p_substring_1(JSContext* cx, JSString* str, jsint begin)
{
    jsint end = JSSTRING_LENGTH(str);
    JS_ASSERT(end >= begin);
    JS_ASSERT(JS_ON_TRACE(cx));
    return js_NewDependentString(cx, str, (size_t)begin, (size_t)(end - begin));
}

JSString* FASTCALL
js_String_getelem(JSContext* cx, JSString* str, jsint i)
{
    if ((size_t)i >= JSSTRING_LENGTH(str))
        return NULL;
    return js_GetUnitString(cx, str, (size_t)i);
}

JSString* FASTCALL
js_String_fromCharCode(JSContext* cx, jsint i)
{
    JS_ASSERT(JS_ON_TRACE(cx));
    jschar c = (jschar)i;
    if (c < UNIT_STRING_LIMIT)
        return js_GetUnitStringForChar(cx, c);
    return js_NewStringCopyN(cx, &c, 1);
}

jsint FASTCALL
js_String_p_charCodeAt(JSString* str, jsint i)
{
    if (i < 0 || (jsint)JSSTRING_LENGTH(str) <= i)
        return -1;
    return JSSTRING_CHARS(str)[i];
}

jsdouble FASTCALL
js_Math_random(JSRuntime* rt)
{
    JS_LOCK_RUNTIME(rt);
    js_random_init(rt);
    jsdouble z = js_random_nextDouble(rt);
    JS_UNLOCK_RUNTIME(rt);
    return z;
}

JSString* FASTCALL
js_String_p_concat_1int(JSContext* cx, JSString* str, jsint i)
{
    // FIXME: should be able to use stack buffer and avoid istr...
    JSString* istr = js_NumberToString(cx, i);
    if (!istr)
        return NULL;
    return js_ConcatStrings(cx, str, istr);
}

JSString* FASTCALL
js_String_p_concat_2str(JSContext* cx, JSString* str, JSString* a, JSString* b)
{
    str = js_ConcatStrings(cx, str, a);
    if (str)
        return js_ConcatStrings(cx, str, b);
    return NULL;
}

JSString* FASTCALL
js_String_p_concat_3str(JSContext* cx, JSString* str, JSString* a, JSString* b, JSString* c)
{
    str = js_ConcatStrings(cx, str, a);
    if (str) {
        str = js_ConcatStrings(cx, str, b);
        if (str)
            return js_ConcatStrings(cx, str, c);
    }
    return NULL;
}

JSObject* FASTCALL
js_String_p_match(JSContext* cx, JSString* str, jsbytecode *pc, JSObject* regexp)
{
    jsval vp[3] = { JSVAL_NULL, STRING_TO_JSVAL(str), OBJECT_TO_JSVAL(regexp) };
    if (!js_StringMatchHelper(cx, 1, vp, pc))
        return (JSObject*) JSVAL_TO_BOOLEAN(JSVAL_VOID);
    JS_ASSERT(JSVAL_IS_NULL(vp[0]) ||
              (!JSVAL_IS_PRIMITIVE(vp[0]) && OBJ_IS_ARRAY(cx, JSVAL_TO_OBJECT(vp[0]))));
    return JSVAL_TO_OBJECT(vp[0]);
}

JSObject* FASTCALL
js_String_p_match_obj(JSContext* cx, JSObject* str, jsbytecode *pc, JSObject* regexp)
{
    jsval vp[3] = { JSVAL_NULL, OBJECT_TO_JSVAL(str), OBJECT_TO_JSVAL(regexp) };
    if (!js_StringMatchHelper(cx, 1, vp, pc))
        return (JSObject*) JSVAL_TO_BOOLEAN(JSVAL_VOID);
    JS_ASSERT(JSVAL_IS_NULL(vp[0]) ||
              (!JSVAL_IS_PRIMITIVE(vp[0]) && OBJ_IS_ARRAY(cx, JSVAL_TO_OBJECT(vp[0]))));
    return JSVAL_TO_OBJECT(vp[0]);
}

JSString* FASTCALL
js_String_p_replace_str(JSContext* cx, JSString* str, JSObject* regexp, JSString* repstr)
{
    jsval vp[4] = {
        JSVAL_NULL, STRING_TO_JSVAL(str), OBJECT_TO_JSVAL(regexp), STRING_TO_JSVAL(repstr)
    };
    if (!js_StringReplaceHelper(cx, 2, NULL, repstr, vp))
        return NULL;
    JS_ASSERT(JSVAL_IS_STRING(vp[0]));
    return JSVAL_TO_STRING(vp[0]);
}

JSString* FASTCALL
js_String_p_replace_str2(JSContext* cx, JSString* str, JSString* patstr, JSString* repstr)
{
    jsval vp[4] = {
        JSVAL_NULL, STRING_TO_JSVAL(str), STRING_TO_JSVAL(patstr), STRING_TO_JSVAL(repstr)
    };
    if (!js_StringReplaceHelper(cx, 2, NULL, repstr, vp))
        return NULL;
    JS_ASSERT(JSVAL_IS_STRING(vp[0]));
    return JSVAL_TO_STRING(vp[0]);
}

JSString* FASTCALL
js_String_p_replace_str3(JSContext* cx, JSString* str, JSString* patstr, JSString* repstr,
                         JSString* flagstr)
{
    jsval vp[5] = {
        JSVAL_NULL, STRING_TO_JSVAL(str), STRING_TO_JSVAL(patstr), STRING_TO_JSVAL(repstr),
        STRING_TO_JSVAL(flagstr)
    };
    if (!js_StringReplaceHelper(cx, 3, NULL, repstr, vp))
        return NULL;
    JS_ASSERT(JSVAL_IS_STRING(vp[0]));
    return JSVAL_TO_STRING(vp[0]);
}

JSObject* FASTCALL
js_String_p_split(JSContext* cx, JSString* str, JSString* sepstr)
{
    // FIXME: optimize by calling into a lower level exported from jsstr.cpp.
    jsval vp[4] = { JSVAL_NULL, STRING_TO_JSVAL(str), STRING_TO_JSVAL(sepstr), JSVAL_VOID };
    if (!js_str_split(cx, 2, vp))
        return NULL;
    JS_ASSERT(JSVAL_IS_OBJECT(vp[0]));
    return JSVAL_TO_OBJECT(vp[0]);
}

jsdouble FASTCALL
js_StringToNumber(JSContext* cx, JSString* str)
{
    const jschar* bp;
    const jschar* end;
    const jschar* ep;
    jsdouble d;

    JSSTRING_CHARS_AND_END(str, bp, end);
    if ((!js_strtod(cx, bp, end, &ep, &d) ||
         js_SkipWhiteSpace(ep, end) != end) &&
        (!js_strtointeger(cx, bp, end, &ep, 0, &d) ||
         js_SkipWhiteSpace(ep, end) != end)) {
        return js_NaN;
    }
    return d;
}

jsint FASTCALL
js_StringToInt32(JSContext* cx, JSString* str)
{
    const jschar* bp;
    const jschar* end;
    const jschar* ep;
    jsdouble d;

    JSSTRING_CHARS_AND_END(str, bp, end);
    if (!js_strtod(cx, bp, end, &ep, &d) || js_SkipWhiteSpace(ep, end) != end)
        return 0;
    return (jsint)d;
}

jsdouble FASTCALL
js_ParseFloat(JSContext* cx, JSString* str)
{
    const jschar* bp;
    const jschar* end;
    const jschar* ep;
    jsdouble d;

    JSSTRING_CHARS_AND_END(str, bp, end);
    if (!js_strtod(cx, bp, end, &ep, &d) || ep == bp)
        return js_NaN;
    return d;
}

jsdouble FASTCALL
js_ParseInt(JSContext* cx, JSString* str)
{
    const jschar* bp;
    const jschar* end;
    const jschar* ep;
    jsdouble d;

    JSSTRING_CHARS_AND_END(str, bp, end);
    if (!js_strtointeger(cx, bp, end, &ep, 0, &d) || ep == bp)
        return js_NaN;
    return d;
}

jsdouble FASTCALL
js_ParseIntDouble(jsdouble d)
{
    if (!JSDOUBLE_IS_FINITE(d))
        return js_NaN;
    return floor(d);
}

jsval FASTCALL
js_Any_getprop(JSContext* cx, JSObject* obj, JSString* idstr)
{
    jsval v;
    jsid id;

    if (!js_ValueToStringId(cx, STRING_TO_JSVAL(idstr), &id))
        return JSVAL_ERROR_COOKIE;
    if (!OBJ_GET_PROPERTY(cx, obj, id, &v))
        return JSVAL_ERROR_COOKIE;
    return v;
}

JSBool FASTCALL
js_Any_setprop(JSContext* cx, JSObject* obj, JSString* idstr, jsval v)
{
    jsid id;
    if (!js_ValueToStringId(cx, STRING_TO_JSVAL(idstr), &id))
        return JS_FALSE;
    return OBJ_SET_PROPERTY(cx, obj, id, &v);
}

jsval FASTCALL
js_Any_getelem(JSContext* cx, JSObject* obj, jsint index)
{
    jsval v;
    jsid id;
    if (index < 0)
        return JSVAL_ERROR_COOKIE;
    if (!js_IndexToId(cx, index, &id))
        return JSVAL_ERROR_COOKIE;
    if (!OBJ_GET_PROPERTY(cx, obj, id, &v))
        return JSVAL_ERROR_COOKIE;
    return v;
}

JSBool FASTCALL
js_Any_setelem(JSContext* cx, JSObject* obj, jsint index, jsval v)
{
    jsid id;
    if (index < 0)
        return JSVAL_ERROR_COOKIE;
    if (!js_IndexToId(cx, index, &id))
        return JS_FALSE;
    return OBJ_SET_PROPERTY(cx, obj, id, &v);
}

JSObject* FASTCALL
js_FastValueToIterator(JSContext* cx, jsuint flags, jsval v)
{
    if (!js_ValueToIterator(cx, flags, &v))
        return NULL;
    return JSVAL_TO_OBJECT(v);
}

jsval FASTCALL
js_FastCallIteratorNext(JSContext* cx, JSObject* iterobj)
{
    jsval v;
    if (!js_CallIteratorNext(cx, iterobj, &v))
        return JSVAL_ERROR_COOKIE;
    return v;
}

GuardRecord* FASTCALL
js_CallTree(InterpState* state, Fragment* f)
{
    GuardRecord* lr;
    union { NIns *code; GuardRecord* (FASTCALL *func)(InterpState*, Fragment*); } u;

    u.code = f->code();
    JS_ASSERT(u.code);

#if defined(JS_NO_FASTCALL) && defined(NANOJIT_IA32)
    SIMULATE_FASTCALL(lr, state, NULL, u.func);
#else
    lr = u.func(state, NULL);
#endif

    if (lr->exit->exitType == NESTED_EXIT) {
        /* This only occurs once a tree call guard mismatches and we unwind the tree call stack.
           We store the first (innermost) tree call guard in state and we will try to grow
           the outer tree the failing call was in starting at that guard. */
        if (!state->lastTreeCallGuard)
            state->lastTreeCallGuard = lr;
    } else {
        /* If the tree exits on a regular (non-nested) guard, keep updating lastTreeExitGuard
           with that guard. If we mismatch on a tree call guard, this will contain the last
           non-nested guard we encountered, which is the innermost loop or branch guard. */
        state->lastTreeExitGuard = lr;
    }

    return lr;
}

JS_STATIC_ASSERT(JSSLOT_PRIVATE == JSSLOT_ARRAY_LENGTH);
JS_STATIC_ASSERT(JSSLOT_ARRAY_LENGTH + 1 == JSSLOT_ARRAY_COUNT);

JSObject* FASTCALL
js_FastNewArray(JSContext* cx, JSObject* proto)
{
    JS_ASSERT(OBJ_IS_ARRAY(cx, proto));

    JS_ASSERT(JS_ON_TRACE(cx));
    JSObject* obj = (JSObject*) js_NewGCThing(cx, GCX_OBJECT, sizeof(JSObject));
    if (!obj)
        return NULL;

    JSClass* clasp = &js_ArrayClass;
    obj->classword = jsuword(clasp);

    obj->fslots[JSSLOT_PROTO] = OBJECT_TO_JSVAL(proto);
    obj->fslots[JSSLOT_PARENT] = proto->fslots[JSSLOT_PARENT];

    obj->fslots[JSSLOT_ARRAY_LENGTH] = 0;
    obj->fslots[JSSLOT_ARRAY_COUNT] = 0;
    for (unsigned i = JSSLOT_ARRAY_COUNT + 1; i != JS_INITIAL_NSLOTS; ++i)
        obj->fslots[i] = JSVAL_VOID;

    JSObjectOps* ops = clasp->getObjectOps(cx, clasp);
    obj->map = ops->newObjectMap(cx, 1, ops, clasp, obj);
    if (!obj->map)
        return NULL;
    obj->dslots = NULL;
    return obj;
}

JSObject* FASTCALL
js_FastNewObject(JSContext* cx, JSObject* ctor)
{
    JS_ASSERT(HAS_FUNCTION_CLASS(ctor));
    JSFunction* fun = GET_FUNCTION_PRIVATE(cx, ctor);
    JSClass* clasp = FUN_INTERPRETED(fun) ? &js_ObjectClass : fun->u.n.clasp;
    JS_ASSERT(clasp != &js_ArrayClass);

    JS_LOCK_OBJ(cx, ctor);
    JSScope *scope = OBJ_SCOPE(ctor);
    JS_ASSERT(scope->object == ctor);
    JSAtom* atom = cx->runtime->atomState.classPrototypeAtom;

    JSScopeProperty *sprop = SCOPE_GET_PROPERTY(scope, ATOM_TO_JSID(atom));
    JS_ASSERT(SPROP_HAS_VALID_SLOT(sprop, scope));
    jsval v = LOCKED_OBJ_GET_SLOT(ctor, sprop->slot);
    JS_UNLOCK_SCOPE(cx, scope);

    JSObject* proto;
    if (JSVAL_IS_PRIMITIVE(v)) {
        if (!js_GetClassPrototype(cx, JSVAL_TO_OBJECT(ctor->fslots[JSSLOT_PARENT]), 
                                  INT_TO_JSID(JSProto_Object), &proto)) {
            return NULL;
        }
    } else {
        proto = JSVAL_TO_OBJECT(v);
    }

    JS_ASSERT(JS_ON_TRACE(cx));
    JSObject* obj = (JSObject*) js_NewGCThing(cx, GCX_OBJECT, sizeof(JSObject));
    if (!obj)
        return NULL;

    obj->classword = jsuword(clasp);
    obj->fslots[JSSLOT_PROTO] = OBJECT_TO_JSVAL(proto);
    obj->fslots[JSSLOT_PARENT] = ctor->fslots[JSSLOT_PARENT];
    for (unsigned i = JSSLOT_PRIVATE; i != JS_INITIAL_NSLOTS; ++i)
        obj->fslots[i] = JSVAL_VOID;

    obj->map = js_HoldObjectMap(cx, proto->map);
    obj->dslots = NULL;
    return obj;
}

JSBool FASTCALL
js_AddProperty(JSContext* cx, JSObject* obj, JSScopeProperty* sprop)
{
    JSScopeProperty* sprop2 = NULL; // initialize early to make MSVC happy

    JS_ASSERT(OBJ_IS_NATIVE(obj));
    JS_ASSERT(SPROP_HAS_STUB_SETTER(sprop));

    JS_LOCK_OBJ(cx, obj);
    JSScope* scope = OBJ_SCOPE(obj);
    if (scope->object == obj) {
        JS_ASSERT(!SCOPE_HAS_PROPERTY(scope, sprop));
    } else {
        scope = js_GetMutableScope(cx, obj);
        if (!scope) {
            JS_UNLOCK_OBJ(cx, obj);
            return JS_FALSE;
        }
    }

    uint32 slot = sprop->slot;
    if (!scope->table && sprop->parent == scope->lastProp && slot == scope->map.freeslot) {
        if (slot < STOBJ_NSLOTS(obj) && !OBJ_GET_CLASS(cx, obj)->reserveSlots) {
            JS_ASSERT(JSVAL_IS_VOID(STOBJ_GET_SLOT(obj, scope->map.freeslot)));
            ++scope->map.freeslot;
        } else {
            if (!js_AllocSlot(cx, obj, &slot)) {
                JS_UNLOCK_SCOPE(cx, scope);
                return JS_FALSE;
            }

            if (slot != sprop->slot)
                goto slot_changed;
        }

        SCOPE_EXTEND_SHAPE(cx, scope, sprop);
        ++scope->entryCount;
        scope->lastProp = sprop;
        JS_UNLOCK_SCOPE(cx, scope);
        return JS_TRUE;
    }

    sprop2 = js_AddScopeProperty(cx, scope, sprop->id,
                                 sprop->getter, sprop->setter, SPROP_INVALID_SLOT,
                                 sprop->attrs, sprop->flags, sprop->shortid);
    if (sprop2 == sprop) {
        JS_UNLOCK_SCOPE(cx, scope);
        return JS_TRUE;
    }
    slot = sprop2->slot;

  slot_changed:
    js_FreeSlot(cx, obj, slot);
    JS_UNLOCK_SCOPE(cx, scope);
    return JS_FALSE;
}

JSBool FASTCALL
js_HasNamedProperty(JSContext* cx, JSObject* obj, JSString* idstr)
{
    jsid id;
    if (!js_ValueToStringId(cx, STRING_TO_JSVAL(idstr), &id))
        return JS_FALSE;

    JSObject* obj2;
    JSProperty* prop;
    if (!OBJ_LOOKUP_PROPERTY(cx, obj, id, &obj2, &prop))
        return JSVAL_TO_BOOLEAN(JSVAL_VOID);
    if (prop)
        OBJ_DROP_PROPERTY(cx, obj2, prop);
    return prop != NULL;
}

jsval FASTCALL
js_CallGetter(JSContext* cx, JSObject* obj, JSScopeProperty* sprop)
{
    JS_ASSERT(!SPROP_HAS_STUB_GETTER(sprop));
    jsval v;
    if (!SPROP_GET(cx, sprop, obj, obj, &v))
        return JSVAL_ERROR_COOKIE;
    return v;
}

JSString* FASTCALL
js_TypeOfObject(JSContext* cx, JSObject* obj)
{
    JSType type = JS_TypeOfValue(cx, OBJECT_TO_JSVAL(obj));
    return ATOM_TO_STRING(cx->runtime->atomState.typeAtoms[type]);
}

JSString* FASTCALL
js_TypeOfBoolean(JSContext* cx, jsint unboxed)
{
    jsval boxed = BOOLEAN_TO_JSVAL(unboxed);
    JS_ASSERT(JSVAL_IS_VOID(boxed) || JSVAL_IS_BOOLEAN(boxed));
    JSType type = JS_TypeOfValue(cx, boxed);
    return ATOM_TO_STRING(cx->runtime->atomState.typeAtoms[type]);
}

jsint FASTCALL
js_Object_p_hasOwnProperty(JSContext* cx, JSObject* obj, JSString *str)
{
    jsid id = ATOM_TO_JSID(STRING_TO_JSVAL(str));
    jsval v;
    if (!js_HasOwnProperty(cx, obj->map->ops->lookupProperty, obj, id, &v))
        return JSVAL_TO_BOOLEAN(JSVAL_VOID);
    JS_ASSERT(JSVAL_IS_BOOLEAN(v));
    return JSVAL_TO_BOOLEAN(v);
}

jsint FASTCALL
js_Object_p_propertyIsEnumerable(JSContext* cx, JSObject* obj, JSString *str)
{
    jsid id = ATOM_TO_JSID(STRING_TO_JSVAL(str));
    jsval v;
    if (!js_PropertyIsEnumerable(cx, obj, id, &v))
        return JSVAL_TO_BOOLEAN(JSVAL_VOID);
    JS_ASSERT(JSVAL_IS_BOOLEAN(v));
    return JSVAL_TO_BOOLEAN(v);
}

jsdouble FASTCALL
js_BooleanToNumber(JSContext* cx, jsint unboxed)
{
    if (unboxed == JSVAL_TO_BOOLEAN(JSVAL_VOID))
        return js_NaN;
    return unboxed;
}

JSString* FASTCALL
js_ObjectToString(JSContext* cx, JSObject* obj)
{
    if (!obj)
        return ATOM_TO_STRING(cx->runtime->atomState.nullAtom);
    jsval v;
    if (!OBJ_DEFAULT_VALUE(cx, obj, JSTYPE_STRING, &v))
        return NULL;
    JS_ASSERT(JSVAL_IS_STRING(v));
    return JSVAL_TO_STRING(v);
}

JSObject* FASTCALL
js_Array_1int(JSContext* cx, JSObject* proto, jsint i)
{
    JS_ASSERT(JS_ON_TRACE(cx));
    JSObject* obj = js_FastNewArray(cx, proto);
    if (obj)
        obj->fslots[JSSLOT_ARRAY_LENGTH] = i;
    return obj;
}

#define ARRAY_CTOR_GUTS(exact_len, newslots_code)                             \
    JS_ASSERT(JS_ON_TRACE(cx));                                               \
    JSObject* obj = js_FastNewArray(cx, proto);                               \
    if (obj) {                                                                \
        const uint32 len = ARRAY_GROWBY;                                      \
        jsval* newslots = (jsval*) JS_malloc(cx, sizeof (jsval) * (len + 1)); \
        if (newslots) {                                                       \
            obj->dslots = newslots + 1;                                       \
            ARRAY_SET_DENSE_LENGTH(obj, len);                                 \
            {newslots_code}                                                   \
            while (++newslots < obj->dslots + len)                            \
                *newslots = JSVAL_HOLE;                                       \
            obj->fslots[JSSLOT_ARRAY_LENGTH] = (exact_len);                   \
            return obj;                                                       \
        }                                                                     \
    }                                                                         \
    return NULL;

JSObject* FASTCALL
js_Array_1str(JSContext* cx, JSObject* proto, JSString *str)
{
    ARRAY_CTOR_GUTS(1, *++newslots = STRING_TO_JSVAL(str);)
}

JSObject* FASTCALL
js_Array_2obj(JSContext* cx, JSObject* proto, JSObject *obj1, JSObject* obj2)
{
    ARRAY_CTOR_GUTS(2,
        *++newslots = OBJECT_TO_JSVAL(obj1);
        *++newslots = OBJECT_TO_JSVAL(obj2);)
}

JSObject* FASTCALL
js_Array_3num(JSContext* cx, JSObject* proto, jsdouble n1, jsdouble n2, jsdouble n3)
{
    ARRAY_CTOR_GUTS(3,
        if (!js_NewDoubleInRootedValue(cx, n1, ++newslots))
            return NULL;
        if (!js_NewDoubleInRootedValue(cx, n2, ++newslots))
            return NULL;
        if (!js_NewDoubleInRootedValue(cx, n3, ++newslots))
            return NULL;)
}

JSObject* FASTCALL
js_Arguments(JSContext* cx)
{
    return NULL;
}

jsdouble FASTCALL
js_Date_now(JSContext*)
{
    return PRMJ_Now() / PRMJ_USEC_PER_MSEC;
}

/* soft float */

jsdouble FASTCALL
js_fneg(jsdouble x)
{
    return -x;
}

jsdouble FASTCALL
js_i2f(jsint i)
{
    return i;
}

jsdouble FASTCALL
js_u2f(jsuint u)
{
    return u;
}

jsint FASTCALL
js_fcmpeq(jsdouble x, jsdouble y)
{
    return x==y;
}

jsint FASTCALL
js_fcmplt(jsdouble x, jsdouble y)
{
    return x < y;
}

jsint FASTCALL
js_fcmple(jsdouble x, jsdouble y)
{
    return x <= y;
}

jsint FASTCALL
js_fcmpgt(jsdouble x, jsdouble y)
{
    return x > y;
}

jsint FASTCALL
js_fcmpge(jsdouble x, jsdouble y)
{
    return x >= y;
}

jsdouble FASTCALL
js_fmul(jsdouble x, jsdouble y)
{
    return x * y;
}
jsdouble FASTCALL
js_fadd(jsdouble x, jsdouble y)
{
    return x + y;
}

jsdouble FASTCALL
js_fdiv(jsdouble x, jsdouble y)
{
    return x / y;
}

jsdouble FASTCALL
js_fsub(jsdouble x, jsdouble y)
{
    return x - y;
}

#define LO ARGSIZE_LO
#define F  ARGSIZE_F
#define Q  ARGSIZE_Q

#if defined AVMPLUS_64BIT
#define P	ARGSIZE_Q
#else
#define P	ARGSIZE_LO
#endif

#ifdef DEBUG
#define NAME(op) ,#op
#else
#define NAME(op)
#endif

#define BUILTIN1(op, at0, atr, tr, t0, cse, fold) \
    { (intptr_t)&js_##op, (at0 << 2) | atr, cse, fold NAME(op) },
#define BUILTIN2(op, at0, at1, atr, tr, t0, t1, cse, fold) \
    { (intptr_t)&js_##op, (at0 << 4) | (at1 << 2) | atr, cse, fold NAME(op) },
#define BUILTIN3(op, at0, at1, at2, atr, tr, t0, t1, t2, cse, fold) \
    { (intptr_t)&js_##op, (at0 << 6) | (at1 << 4) | (at2 << 2) | atr, cse, fold NAME(op) },
#define BUILTIN4(op, at0, at1, at2, at3, atr, tr, t0, t1, t2, t3, cse, fold) \
    { (intptr_t)&js_##op, (at0 << 8) | (at1 << 6) | (at2 << 4) | (at3 << 2) | atr, cse, fold NAME(op) },
#define BUILTIN5(op, at0, at1, at2, at3, at4, atr, tr, t0, t1, t2, t3, t4, cse, fold) \
    { (intptr_t)&js_##op, (at0 << 10) | (at1 << 8) | (at2 << 6) | (at3 << 4) | (at4 << 2) | atr, cse, fold NAME(op) },

struct CallInfo builtins[] = {
#include "builtins.tbl"
};
