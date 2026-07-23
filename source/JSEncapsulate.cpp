#include "JSEncapsulate.h"
#include <string>
#include "StringUtility.hpp"
#include <js/Object.h>
#include <js/Conversions.h>
#include "SEFunctions.h"

namespace
{
void *GetUOXPrivate( JSObject *object )
{
	if( object == nullptr || JSCLASS_RESERVED_SLOTS( JS::GetClass( object ) ) == 0 )
		return nullptr;

	const JS::Value &value = JS::GetReservedSlot( object, 0 );
	return value.isUndefined() || value.isNull() ? nullptr : value.toPrivate();
}
}

void JSEncapsulate::InternalReset( void )
{
	beenParsed[JSOT_INT]	= false;
	beenParsed[JSOT_DOUBLE] = false;
	beenParsed[JSOT_BOOL]	= false;
	beenParsed[JSOT_STRING] = false;
	beenParsed[JSOT_OBJECT] = false;
	nativeType				= JSOT_COUNT;
	className				= "Native";
	classCached				= false;
	intVal					= 0;
	floatVal				= 0.0f;
	boolVal					= false;
	stringVal				= "";
	objectVal				= nullptr;
}
JSEncapsulate::JSEncapsulate() : cx( nullptr ), vp( nullptr ), obj( nullptr )
{
	InternalReset();
}
void JSEncapsulate::SetContext( JSContext *jsCX, const JS::Value *jsVP )
{
	cx = jsCX;
	vp = jsVP;
	Init();
}
void JSEncapsulate::Init( void )
{
	if( vp->isPrimitive() )
	{
		if( vp->isDouble() )
		{
			nativeType	= JSOT_DOUBLE;
		}
		else if( vp->isInt32() )
		{
			nativeType	= JSOT_INT;
		}
		else if( vp->isBoolean() )
		{
			nativeType	= JSOT_BOOL;
		}
		else if( vp->isString() )
		{
			nativeType	= JSOT_STRING;
		}
		else if( vp->isUndefined() )
		{
			nativeType	= JSOT_VOID;
		}
		else if( vp->isNull() )
		{
			nativeType	= JSOT_NULL;
		}
	}
	else if( vp->isObject() )
	{
		nativeType	= JSOT_OBJECT;
	}
}
JSEncapsulate::JSEncapsulate( JSContext *jsCX, const JS::Value *jsVP ) : cx( jsCX ), vp( jsVP ), obj( nullptr )
{
	InternalReset();
	Init();
}
JSEncapsulate::JSEncapsulate( JSContext *jsCX, JSObject *jsVP ) : intVal( 0 ), floatVal( 0 ), boolVal( false ), stringVal( "" ), objectVal( nullptr ), cx( jsCX ), vp( nullptr ), obj( jsVP )
{
	InternalReset();
	// We don't want to call Init() here, because we *know* it's an Object
	nativeType				= JSOT_OBJECT;
	objectVal				= GetUOXPrivate( jsVP );
	beenParsed[JSOT_OBJECT]	= true;
}

bool JSEncapsulate::isType( JSEncapsObjectType toCheck )
{
	return( nativeType == toCheck );
}

SI32 JSEncapsulate::toInt( void )
{
	if( nativeType == JSOT_OBJECT )
	{
		throw new std::runtime_error( "Cannot convert JS Object to an int" );
	}
	if( !beenParsed[JSOT_INT] )
	{
		Parse( JSOT_INT );
	}
	return intVal;
}
bool JSEncapsulate::toBool( void )
{
	if( nativeType == JSOT_OBJECT )
	{
		throw new std::runtime_error( "Cannot convert JS Object to a bool" );
	}
	if( !beenParsed[JSOT_BOOL] )
	{
		Parse( JSOT_BOOL );
	}
	return boolVal;
}
float JSEncapsulate::toFloat( void )
{
	if( nativeType == JSOT_OBJECT )
	{
		throw new std::runtime_error( "Cannot convert JS Object to a float" );
	}
	if( !beenParsed[JSOT_DOUBLE] )
	{
		Parse( JSOT_DOUBLE );
	}
	return floatVal;
}
std::string JSEncapsulate::toString( void )
{
	if( nativeType == JSOT_OBJECT )
	{
		throw new std::runtime_error( "Cannot convert JS Object to a string" );
	}
	if( !beenParsed[JSOT_STRING] )
	{
		Parse( JSOT_STRING );
	}
	return stringVal;
}
void *JSEncapsulate::toObject( void )
{
	if( nativeType != JSOT_OBJECT )
	{
		throw new std::runtime_error( "Cannot convert to JS Object" );
	}
	if( !beenParsed[JSOT_OBJECT] )
	{
		Parse( JSOT_OBJECT );
	}
	return objectVal;
}

std::string JSEncapsulate::ClassName( void )
{
	std::string rVal = className;
	if( !classCached )
	{
		if( nativeType == JSOT_OBJECT )
		{
			JSObject *obj2 = nullptr;
			if( vp != nullptr )
			{
				obj2 = vp->toObjectOrNull();
			}
			else
			{
				obj2 = obj;
			}
			if( obj2 != nullptr )
			{
				const JSClass *mClass = JS::GetClass( obj2 );
				rVal = oldstrutil::trim( mClass->name ); // Remove any whitespace, though I wouldn't have expected any?
				className = rVal;	// Ensure we update the cached value for subsequent calls
			}
		}
		classCached = true;
	}
	return rVal;
}
void JSEncapsulate::Parse( JSEncapsObjectType typeConvert )
{
	double	fvalue;
	SI32		ivalue;
	std::string	svalue;
	bool		bvalue;
	switch( typeConvert )
	{
		case JSOT_INT:
			switch( nativeType )
			{
				case JSOT_INT:		intVal = vp->toInt32();	break;
				case JSOT_DOUBLE:
				{
					JS::RootedValue rootedValue(cx, (*vp) );
					JS::ToNumber( cx, rootedValue, &fvalue );
					intVal = static_cast<SI32>( fvalue );
				}
					break;
				case JSOT_BOOL:		intVal = (vp->toBoolean() ? 1 : 0);	break;
				case JSOT_STRING:
					svalue = JS_GetStringBytes( cx, *vp );
					intVal = std::stoi( svalue, nullptr, 0 );
					break;
				default:
				case JSOT_COUNT:
					break;
			}
			break;
		case JSOT_DOUBLE:
			switch( nativeType )
			{
				case JSOT_INT:
					ivalue		= vp->toInt32();
					floatVal	= static_cast<R32>( ivalue );
					break;
				case JSOT_DOUBLE:
				{
					JS::RootedValue rootedValue( cx, (*vp) );
					JS::ToNumber( cx, rootedValue, &fvalue );
					floatVal	= static_cast<R32>( fvalue );
					}
					break;
				case JSOT_BOOL:		floatVal	= (vp->toBoolean() ? 1.0f : 0.0f);	break;
				case JSOT_STRING:
					svalue		= JS_GetStringBytes( cx, *vp );
					floatVal	= std::stof( svalue );
					break;
				default:
				case JSOT_COUNT:
					break;
			}
			break;
		case JSOT_BOOL:
			switch( nativeType )
			{
				case JSOT_INT:
					ivalue	= vp->toInt32();
					boolVal	= ( ivalue != 0 );
					break;
				case JSOT_DOUBLE:
				{
					JS::RootedValue rootedValue( cx, (*vp) );
					JS::ToNumber( cx, rootedValue, &fvalue );
					boolVal	= ( fvalue != 0.0f );
				}
					break;
				case JSOT_BOOL:		boolVal = vp->toBoolean();	break;
				case JSOT_STRING:
					svalue	= JS_GetStringBytes( cx, *vp );
					boolVal = ( oldstrutil::upper( svalue ) == "TRUE" );
					break;
				default:
				case JSOT_COUNT:
					break;
			}
			break;
		case JSOT_STRING:
			switch( nativeType )
			{
				case JSOT_INT:
					ivalue		= vp->toInt32();
					stringVal	= oldstrutil::number( ivalue );
					break;
				case JSOT_DOUBLE:
				{
					JS::RootedValue rootedValue(cx, (*vp) );
					JS::ToNumber( cx, rootedValue, &fvalue );
					stringVal	= oldstrutil::number( fvalue );
				}
					break;
				case JSOT_BOOL:
					bvalue	= vp->toBoolean();
					if( bvalue )
					{
						stringVal = "TRUE";
					}
					else
					{
						stringVal = "FALSE";
					}
					break;
				case JSOT_STRING:
					stringVal	= JS_GetStringBytes( cx, *vp );
					break;
				default:
				case JSOT_COUNT:
					break;
			}
			break;
		case JSOT_OBJECT:
			objectVal	= GetUOXPrivate( vp->toObjectOrNull() );
			break;
		default:
		case JSOT_COUNT:
			std::cout << '\n' << "JSOT_COUNT enum value passed to JSEncapsulate::Parse(). This should not happen!" << '\n';
			break;
	}
	beenParsed[typeConvert] = true;
}
