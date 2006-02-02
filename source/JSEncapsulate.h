#include "Prerequisites.h"

namespace UOX
{
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
		int			toInt( void );
		bool		toBool( void );
		float		toFloat( void );
		std::string toString( void );
		void *		toObject( void );

		std::string	ClassName( void );
//		operator const char *()
	private:

		void				InternalReset( void );
		void				Init( void );
		bool				beenParsed[JSOT_COUNT];
		JSEncapsObjectType	nativeType;

		int					intVal;
		float				floatVal;
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
}
