#include "Prerequisites.h"

namespace UOX
{
	class JSEncapsulate
	{
	public:
		enum JSObjectType
		{
			JSOT_INT = 0,
			JSOT_DOUBLE,
			JSOT_BOOL,
			JSOT_STRING,
			JSOT_OBJECT,
			JSOT_COUNT
		};

		JSEncapsulate( JSContext *jsCX, jsval *jsVP );
		bool isType( JSObjectType toCheck );
		int			toInt( void );
		bool		toBool( void );
		float		toFloat( void );
		std::string toString( void );
		void *		toObject( void );
//		operator const char *()
	protected:

		bool			beenParsed[JSOT_COUNT];
		JSObjectType	nativeType;

		int				intVal;
		float			floatVal;
		bool			boolVal;
		std::string		stringVal;
		void *			objectVal;

		JSContext *		cx;
		jsval *			vp;

		void Parse( JSObjectType typeConvert );
	};
}
