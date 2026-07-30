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
