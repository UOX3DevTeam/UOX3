#include "Prerequisites.h"
#include "typedefs.h"
enum JSEncapsObjectType
{
	JSOT_INT = 0,
	JSOT_DOUBLE,
	JSOT_BOOL,
	JSOT_STRING,
	JSOT_OBJECT,
	JSOT_NULL,
	JSOT_VOID,
	JSOT_COUNT
};
class JSEncapsulate
{
public:

	JSEncapsulate( JSContext *jsCX, jsval *jsVP );
	JSEncapsulate( JSContext *jsCX, JSObject *jsVP );
	JSEncapsulate();
	void		SetContext( JSContext *jsCX, jsval *jsVP );
	bool		isType( JSEncapsObjectType toCheck );
	SI32		toInt( void );
	bool		toBool( void );
	R32		toFloat( void );
	std::string toString( void );
	void *		toObject( void );

	std::string	ClassName( void );
private:

	void				InternalReset( void );
	void				Init( void );
	bool				beenParsed[JSOT_COUNT+1];
	JSEncapsObjectType	nativeType;

	SI32				intVal;
	R32					floatVal;
	bool				boolVal;
	std::string			stringVal;
	void *				objectVal;

	JSContext *			cx;
	jsval *				vp;
	JSObject *			obj;

	std::string			className;
	bool				classCached;

	void Parse( JSEncapsObjectType typeConvert );
};
