#include "JSEncapsulate.h"
#include "ustring.h"

namespace UOX
{
	JSEncapsulate::JSEncapsulate( JSContext *jsCX, jsval *jsVP ) : cx( jsCX ), vp( jsVP ), intVal( 0 ), floatVal( 0 ), boolVal( false ), objectVal( NULL ), stringVal( "" )
	{
		beenParsed[JSOT_INT]	= false;
		beenParsed[JSOT_DOUBLE] = false;
		beenParsed[JSOT_BOOL]	= false;
		beenParsed[JSOT_STRING] = false;
		beenParsed[JSOT_OBJECT] = false;

		nativeType = JSOT_COUNT;
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
			else if( JSVAL_IS_OBJECT( (*vp) ) )
				nativeType	= JSOT_OBJECT;
		}
	}

	bool JSEncapsulate::isType( JSObjectType toCheck )
	{
		return( nativeType == toCheck );
	}

	int JSEncapsulate::toInt( void )
	{
		if( nativeType == JSOT_OBJECT )
			throw new std::runtime_error( "Cannot convert JS Object to int" );
		if( !beenParsed[JSOT_INT] )	
			Parse( JSOT_INT );
		return intVal;
	}
	bool JSEncapsulate::toBool( void )
	{
		if( nativeType == JSOT_OBJECT )
			throw new std::runtime_error( "Cannot convert JS Object to int" );
		if( !beenParsed[JSOT_BOOL] )	
			Parse( JSOT_BOOL );
		return boolVal;
	}
	float JSEncapsulate::toFloat( void )
	{
		if( nativeType == JSOT_OBJECT )
			throw new std::runtime_error( "Cannot convert JS Object to int" );
		if( !beenParsed[JSOT_DOUBLE] )	
			Parse( JSOT_DOUBLE );
		return floatVal;
	}
	std::string JSEncapsulate::toString( void )
	{
		if( nativeType == JSOT_OBJECT )
			throw new std::runtime_error( "Cannot convert JS Object to int" );
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
	void JSEncapsulate::Parse( JSObjectType typeConvert )
	{
		jsdouble	fvalue;
		int			ivalue;
		UString		svalue;
		bool		bvalue;
		switch( typeConvert )
		{
		case JSOT_INT:
			switch( nativeType )
			{
			case JSOT_INT:		intVal = JSVAL_TO_INT( (*vp) );	break;
			case JSOT_DOUBLE:
								JS_ValueToNumber( cx, (*vp), &fvalue );
								intVal = (int)fvalue;
								break;
			case JSOT_BOOL:		intVal = ( (JSVAL_TO_BOOLEAN( (*vp) ) == JS_TRUE) ? 1 : 0 );	break;
			case JSOT_STRING:
								svalue = JS_GetStringBytes( JS_ValueToString( cx, *vp ) );
								intVal = svalue.toInt();
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
								floatVal	= (float)ivalue;
								break;
			case JSOT_DOUBLE:
								JS_ValueToNumber( cx, (*vp), &fvalue );
								floatVal	= (float)fvalue;
								break;
			case JSOT_BOOL:		floatVal	= ( (JSVAL_TO_BOOLEAN( (*vp) ) == JS_TRUE) ? 1.0f : 0.0f );	break;
			case JSOT_STRING:
								svalue		= JS_GetStringBytes( JS_ValueToString( cx, *vp ) );
								floatVal	= svalue.toFloat();
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
								boolVal = (svalue.upper() == "TRUE");
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
								stringVal	= UString::number( ivalue );
								break;
			case JSOT_DOUBLE:
								JS_ValueToNumber( cx, (*vp), &fvalue );
								stringVal	= UString::number( fvalue );
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
			break;
		}
		beenParsed[typeConvert] = true;
	}
}
