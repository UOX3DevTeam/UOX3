#ifndef UOX_JS_COMPAT_H
#define UOX_JS_COMPAT_H

#include <jsapi.h>
#include <js/Array.h>
#include <js/CallAndConstruct.h>
#include <js/CharacterEncoding.h>
#include <js/Exception.h>
#include <js/Object.h>
#include <js/PropertyAndElement.h>
#include <js/Conversions.h>
#include <js/Warnings.h>

inline bool JS_SetElement( JSContext *cx, JSObject *obj, uint32_t index,
	const JS::Value *value )
{
	JS::RootedObject rootedObj( cx, obj );
	JS::RootedValue rootedValue( cx, *value );
	return JS_SetElement( cx, rootedObj, index, rootedValue );
}

inline bool JS_GetProperty( JSContext *cx, JSObject *obj, const char *name, JS::Value *value )
{
	JS::RootedObject rootedObj( cx, obj );
	return JS_GetProperty( cx, rootedObj, name,
		JS::MutableHandleValue::fromMarkedLocation( value ) );
}

#endif
