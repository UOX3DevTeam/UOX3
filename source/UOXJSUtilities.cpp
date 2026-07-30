#include "UOXJSUtilities.h"

#include <js/CharacterEncoding.h>
#include <js/RootingAPI.h>

std::string JSStringToString( JSContext *cx, JSString *string )
{
	JS::RootedString rootedString( cx, string );
	JS::UniqueChars encodedString = JS_EncodeStringToASCII( cx, rootedString );
	return encodedString ? std::string( encodedString.get() ) : std::string();
}
