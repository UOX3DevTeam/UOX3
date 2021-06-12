#include "JSEncapsulate.h"
#include <string>
#include "StringUtility.hpp"
#include "jsobj.h"
#include "jsutil.h"

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
void JSEncapsulate::SetContext( JSContext *jsCX, jsval *jsVP )
{
	cx = jsCX;
	vp = jsVP;
	Init();
}
void JSEncapsulate::Init( void )
{
	if( JSVAL_IS_PRIMITIVE( *vp ) )
	{
		if( JSVAL_IS_DOUBLE( (*vp) ) )
			nativeType	= JSOT_DOUBLE;
		else if( JSVAL_IS_INT( (*vp) ) )
			nativeType	= JSOT_INT;
		else if( JSVAL_IS_BOOLEAN( (*vp) ) )
			nativeType	= JSOT_BOOL;
		else if( JSVAL_IS_STRING( (*vp) ) )
			nativeType	= JSOT_STRING;
		else if( JSVAL_IS_VOID( (*vp) ) )
			nativeType	= JSOT_VOID;
		else if( JSVAL_IS_NULL( (*vp) ) )
			nativeType	= JSOT_NULL;
	}
	else if( JSVAL_IS_OBJECT( (*vp) ) )
		nativeType	= JSOT_OBJECT;
}
JSEncapsulate::JSEncapsulate( JSContext *jsCX, jsval *jsVP ) : cx( jsCX ), vp( jsVP ), obj( nullptr )
{
	InternalReset();
	Init();
}
JSEncapsulate::JSEncapsulate( JSContext *jsCX, JSObject *jsVP ) : intVal( 0 ), floatVal( 0 ), boolVal( false ), stringVal( "" ), objectVal( nullptr ), cx( jsCX ), vp( nullptr ), obj( jsVP )
{
	InternalReset();
	// We don't want to call Init() here, because we *know* it's an Object
	nativeType				= JSOT_OBJECT;
	objectVal				= (void*)JS_GetPrivate( cx, jsVP );
	beenParsed[JSOT_OBJECT]	= true;
}

bool JSEncapsulate::isType( JSEncapsObjectType toCheck )
{
	return( nativeType == toCheck );
}

SI32 JSEncapsulate::toInt( void )
{
	if( nativeType == JSOT_OBJECT )
		throw new std::runtime_error( "Cannot convert JS Object to an int" );
	if( !beenParsed[JSOT_INT] )
		Parse( JSOT_INT );
	return intVal;
}
bool JSEncapsulate::toBool( void )
{
	if( nativeType == JSOT_OBJECT )
		throw new std::runtime_error( "Cannot convert JS Object to a bool" );
	if( !beenParsed[JSOT_BOOL] )
		Parse( JSOT_BOOL );
	return boolVal;
}
float JSEncapsulate::toFloat( void )
{
	if( nativeType == JSOT_OBJECT )
		throw new std::runtime_error( "Cannot convert JS Object to a float" );
	if( !beenParsed[JSOT_DOUBLE] )
		Parse( JSOT_DOUBLE );
	return floatVal;
}
std::string JSEncapsulate::toString( void )
{
	if( nativeType == JSOT_OBJECT )
		throw new std::runtime_error( "Cannot convert JS Object to a string" );
	if( !beenParsed[JSOT_STRING] )
		Parse( JSOT_STRING );
	return stringVal;
}
void *JSEncapsulate::toObject( void )
{
	if( nativeType != JSOT_OBJECT )
		throw new std::runtime_error( "Cannot convert to JS Object" );
	if( !beenParsed[JSOT_OBJECT] )
		Parse( JSOT_OBJECT );
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
				obj2 = JSVAL_TO_OBJECT( *vp );
			else
				obj2 = obj;
			if( obj2 != nullptr )
			{
				JSClass *mClass = OBJ_GET_CLASS( cx, obj2 );
				if( mClass->flags & JSCLASS_IS_EXTENDED )	// extended class
				{
					JSExtendedClass *mClass2 = reinterpret_cast<JSExtendedClass *>( mClass ); // (JSExtendedClass *)mClass;
					className					= mClass2->base.name;
				}
				else
				{
					className = mClass->name;
				}
				rVal = className;
			}
		}
		classCached = true;
	}
	return rVal;
}
void JSEncapsulate::Parse( JSEncapsObjectType typeConvert )
{
	jsdouble	fvalue;
	SI32		ivalue;
	std::string	svalue;
	bool		bvalue;
	switch( typeConvert )
	{
		case JSOT_INT:
			switch( nativeType )
			{
				case JSOT_INT:		intVal = JSVAL_TO_INT( (*vp) );	break;
				case JSOT_DOUBLE:
					JS_ValueToNumber( cx, (*vp), &fvalue );
					intVal = (SI32)fvalue;
					break;
				case JSOT_BOOL:		intVal = ( (JSVAL_TO_BOOLEAN( (*vp) ) == JS_TRUE) ? 1 : 0 );	break;
				case JSOT_STRING:
					svalue = JS_GetStringBytes( JS_ValueToString( cx, *vp ) );
					intVal = std::stoi(svalue, nullptr, 0);
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
					ivalue		= JSVAL_TO_INT( (*vp) );
					floatVal	= (R32)ivalue;
					break;
				case JSOT_DOUBLE:
					JS_ValueToNumber( cx, (*vp), &fvalue );
					floatVal	= (R32)fvalue;
					break;
				case JSOT_BOOL:		floatVal	= ( (JSVAL_TO_BOOLEAN( (*vp) ) == JS_TRUE) ? 1.0f : 0.0f );	break;
				case JSOT_STRING:
					svalue		= JS_GetStringBytes( JS_ValueToString( cx, *vp ) );
					floatVal	= std::stof(svalue);
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
					ivalue	= JSVAL_TO_INT( (*vp) );
					boolVal	= (ivalue != 0);
					break;
				case JSOT_DOUBLE:
					JS_ValueToNumber( cx, (*vp), &fvalue );
					boolVal	= (fvalue != 0.0f);
					break;
				case JSOT_BOOL:		boolVal = (JSVAL_TO_BOOLEAN( (*vp) ) == JS_TRUE);	break;
				case JSOT_STRING:
					svalue	= JS_GetStringBytes( JS_ValueToString( cx, *vp ) );
					boolVal = (strutil::toupper( svalue ) == "TRUE");
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
					ivalue		= JSVAL_TO_INT( (*vp) );
					stringVal	= strutil::number( ivalue );
					break;
				case JSOT_DOUBLE:
					JS_ValueToNumber( cx, (*vp), &fvalue );
					stringVal	= strutil::number( fvalue );
					break;
				case JSOT_BOOL:
					bvalue	= (JSVAL_TO_BOOLEAN( (*vp) ) == JS_TRUE);
					if( bvalue )
						stringVal = "TRUE";
					else
						stringVal = "FALSE";
					break;
				case JSOT_STRING:
					stringVal	= JS_GetStringBytes( JS_ValueToString( cx, *vp ) );
					break;
				default:
				case JSOT_COUNT:
					break;
			}
			break;
		case JSOT_OBJECT:
			objectVal	= (void*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( *vp ) );
			break;
		default:
		case JSOT_COUNT:
			std::cout << '\n' << "JSOT_COUNT enum value passed to JSEncapsulate::Parse(). This should not happen!" << '\n';
			break;
	}
	beenParsed[typeConvert] = true;
}
