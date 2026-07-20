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

using JSBool = bool;
using jsval = JS::Value;
using intN = int;
using uintN = unsigned int;
using uint32 = uint32_t;
using jsdouble = double;

#define JS_TRUE true
#define JS_FALSE false

#define JSVAL_NULL JS::NullValue()
#define JSVAL_VOID JS::UndefinedValue()
#define JSVAL_TRUE JS::BooleanValue(true)
#define JSVAL_FALSE JS::BooleanValue(false)

#define BOOLEAN_TO_JSVAL(value) JS::BooleanValue(value)
#define INT_TO_JSVAL(value) JS::Int32Value(value)
#define OBJECT_TO_JSVAL(value) JS::ObjectOrNullValue(value)
#define STRING_TO_JSVAL(value) JS::StringValue(value)

#define JSVAL_IS_BOOLEAN(value) ((value).isBoolean())
#define JSVAL_IS_DOUBLE(value) ((value).isDouble())
#define JSVAL_IS_INT(value) ((value).isInt32())
#define JSVAL_IS_NULL(value) ((value).isNull())
#define JSVAL_IS_OBJECT(value) ((value).isObject())
#define JSVAL_IS_PRIMITIVE(value) ((value).isPrimitive())
#define JSVAL_IS_STRING(value) ((value).isString())
#define JSVAL_IS_VOID(value) ((value).isUndefined())

#define JSVAL_TO_BOOLEAN(value) ((value).toBoolean())
#define JSVAL_TO_INT(value) ((value).toInt32())
#define JSVAL_TO_OBJECT(value) (&(value).toObject())

#define JSID_IS_INT(id) ((id).isInt())
#define JSID_IS_STRING(id) ((id).isString())
#define JSID_TO_INT(id) ((id).toInt())
#define JSID_TO_STRING(id) ((id).toString())

// Legacy native callbacks receive argc/vp and access their call frame through
// these macros. Keep that source shape while using the modern CallArgs API.
#define JS_ARGV(cx, vp) (JS::CallArgsFromVp( argc, vp ).array())
#define JS_RVAL(cx, vp) (JS::CallArgsFromVp( argc, vp ).rval().get())

inline void UOX_JS_SetRval( JS::MutableHandleValue result, const JS::Value &value )
{
	result.set( value );
}

inline void UOX_JS_SetRval( JS::MutableHandleValue result, bool value )
{
	result.setBoolean( value );
}

#define JS_SET_RVAL(cx, vp, value) \
	UOX_JS_SetRval( JS::CallArgsFromVp( argc, vp ).rval(), value )

inline JSObject *UOX_JS_THIS_OBJECT( JSContext *cx, unsigned argc, jsval *vp )
{
	auto args = JS::CallArgsFromVp( argc, vp );
	JS::RootedObject thisObject( cx );
	if( !args.computeThis( cx, &thisObject ))
	{
		return nullptr;
	}
	return thisObject;
}

#define JS_THIS_OBJECT(cx, vp) UOX_JS_THIS_OBJECT( cx, argc, vp )

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

inline bool JS_NewNumberValue( JSContext *, double number, jsval *value )
{
	*value = JS::NumberValue( number );
	return true;
}

inline JSObject *JS_NewArrayObject( JSContext *cx, size_t length, jsval * )
{
	return JS::NewArrayObject( cx, length );
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
	const jsval *value )
{
	JS::RootedObject rootedObj( cx, obj );
	JS::RootedValue rootedValue( cx, *value );
	return JS_SetElement( cx, rootedObj, index, rootedValue );
}

inline bool JS_CallFunctionName( JSContext *cx, JSObject *obj, const char *name,
	unsigned argc, const jsval *argv, jsval *rval )
{
	JS::RootedObject rootedObj( cx, obj );
	auto args = JS::HandleValueArray::fromMarkedLocation( argc, argv );
	auto result = JS::MutableHandleValue::fromMarkedLocation( rval );
	return JS_CallFunctionName( cx, rootedObj, name, args, result );
}

inline bool JS_GetProperty( JSContext *cx, JSObject *obj, const char *name, jsval *value )
{
	JS::RootedObject rootedObj( cx, obj );
	return JS_GetProperty( cx, rootedObj, name,
		JS::MutableHandleValue::fromMarkedLocation( value ) );
}

inline bool JS_GetPendingException( JSContext *cx, jsval *value )
{
	return JS_GetPendingException( cx,
		JS::MutableHandleValue::fromMarkedLocation( value ) );
}

inline bool JS_ValueToECMAUint32( JSContext *cx, const jsval &value, uint32_t *result )
{
	JS::RootedValue rootedValue( cx, value );
	return JS::ToUint32( cx, rootedValue, result );
}

inline bool JS_ValueToNumber( JSContext *cx, const jsval &value, double *result )
{
	JS::RootedValue rootedValue( cx, value );
	return JS::ToNumber( cx, rootedValue, result );
}

#endif
