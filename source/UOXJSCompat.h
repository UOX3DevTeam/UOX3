#ifndef UOX_JS_COMPAT_H
#define UOX_JS_COMPAT_H

#include <algorithm>
#include <memory>
#include <vector>

#include <jsapi.h>
#include <js/Array.h>
#include <js/CallAndConstruct.h>
#include <js/CharacterEncoding.h>
#include <js/Exception.h>
#include <js/Object.h>
#include <js/PropertyAndElement.h>
#include <js/Conversions.h>
#include <js/Warnings.h>

// Legacy native callbacks receive argc/vp and access their call frame through
// these macros. Keep that source shape while using the modern CallArgs API.
#define JS_RVAL(cx, vp) (JS::CallArgsFromVp( argc, vp ).rval().get())

inline void *JS_GetPrivate( JSContext *, JSObject *obj )
{
	if( obj == nullptr )
	{
		return nullptr;
	}
	const JS::Value &value = JS::GetReservedSlot( obj, 0 );
	return value.isUndefined() || value.isNull() ? nullptr : value.toPrivate();
}

inline void JS_SetPrivate( JSContext *, JSObject *obj, void *value )
{
	JS::SetReservedSlot( obj, 0,
		value == nullptr ? JS::UndefinedValue() : JS::PrivateValue( value ));
}

inline JSObject *JS_NewObject( JSContext *cx, const JSClass *jsClass,
	JSObject *, JSObject *parent )
{
	JSObject *obj = JS_NewObject( cx, jsClass );
	if( obj != nullptr && parent != nullptr )
	{
		JS::SetReservedSlot( obj, 1, JS::ObjectValue( *parent ));
	}
	return obj;
}

inline bool UOX_JS_DefineProperties( JSContext *cx, JSObject *obj,
	const JSPropertySpec *properties )
{
	JS::RootedObject rootedObj( cx, obj );
	return JS_DefineProperties( cx, rootedObj, properties );
}

#define JS_DefineProperties(cx, obj, properties) \
	UOX_JS_DefineProperties( cx, obj, properties )

inline bool UOX_JS_DefineFunctions( JSContext *cx, JSObject *obj,
	const JSFunctionSpec *functions )
{
	JS::RootedObject rootedObj( cx, obj );
	return JS_DefineFunctions( cx, rootedObj, functions );
}

#define JS_DefineFunctions(cx, obj, functions) \
	UOX_JS_DefineFunctions( cx, obj, functions )

inline std::vector<std::unique_ptr<JS::PersistentRootedObject>> &UOX_GCLockedObjects()
{
	static std::vector<std::unique_ptr<JS::PersistentRootedObject>> roots;
	return roots;
}

inline void JS_LockGCThing( JSContext *cx, JSObject *obj )
{
	UOX_GCLockedObjects().push_back(
		std::make_unique<JS::PersistentRootedObject>( cx, obj ));
}

inline void JS_UnlockGCThing( JSContext *, JSObject *obj )
{
	auto &roots = UOX_GCLockedObjects();
	roots.erase( std::remove_if( roots.begin(), roots.end(),
		[obj]( const auto &root ) { return root->get() == obj; } ), roots.end() );
}

inline JSObject *JS_GetParent( JSContext *, JSObject *obj )
{
	const JS::Value &parent = JS::GetReservedSlot( obj, 1 );
	return parent.isObject() ? &parent.toObject() : nullptr;
}

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
