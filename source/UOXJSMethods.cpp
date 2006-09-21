//o--------------------------------------------------------------------------o
//|	File					-	UOXJSMethods.cpp
//|	Date					-	
//|	Developers		-	DarkStorm
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									1.0		Dark-Storm	20th December, 2001 Initial implementation
//|									Methods for the javascript objects
//|									
//|									List of supported Classes:
//|										- Gumps
//|										- Char
//|										- Item
//|										- Socket
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "UOXJSPropertySpecs.h"
#include "SEFunctions.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "JSEncapsulate.h"
#include "CJSEngine.h"

#include "cMagic.h"
#include "cGuild.h"
#include "skills.h"
#include "speech.h"
#include "gump.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "network.h"
#include "classes.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "commands.h"
#include "movement.h"
#include "wholist.h"
#include "townregion.h"
#include "Dictionary.h"
#include "jail.h"
#include "magic.h"
#include "CPacketSend.h"
#include "mapstuff.h"
#include "cThreadQueue.h"
#include "combat.h"
#include "PartySystem.h"

namespace UOX
{

void BuildAddMenuGump( CSocket *s, UI16 m );	// Menus for item creation
void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, UI08 srcWorld, SI16 trgX, SI16 trgY, SI08 trgZ, UI08 trgWorld );
bool BuyShop( CSocket *s, CChar *c );
void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway );

void MethodError( const char *txt, ... )
{
	va_list argptr;
	char msg[512];
	va_start( argptr, txt );
	vsprintf( msg, txt, argptr );
	va_end( argptr );
	Console.Error( msg );
}

//
//	Copied that here from SEFunctions.cpp
//	Default paramters weren't working !?
//
void MethodSpeech( CBaseObject &speaker, char *message, SpeechType sType, COLOUR sColour = 0x005A, FontType fType = FNT_NORMAL, SpeechTarget spTrg = SPTRG_PCNPC )
{
	CSpeechEntry& toAdd = SpeechSys->Add();
	toAdd.Font( fType );
	toAdd.Speech( message );
	toAdd.Speaker( speaker.GetSerial() );
	toAdd.Type( sType );
	toAdd.TargType( spTrg );

	// Added that because transparent text could cause trouble
	if( sColour != 0x1700 && sColour != 0x0)
		toAdd.Colour( sColour );
	else
		toAdd.Colour( 0x0058 );

	toAdd.At( cwmWorldState->GetUICurrentTime() );
}

JSBool Packet( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CPUOXBuffer *toAdd = new CPUOXBuffer;

	JS_DefineFunctions( cx, obj, CPacket_Methods );
	JS_SetPrivate( cx, obj, toAdd);
	JS_LockGCThing( cx, obj );
	//JS_AddRoot( cx, &obj );
	return JS_TRUE;
}

JSBool CPacket_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CPUOXBuffer *toDelete = static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, obj ));

	if( toDelete == NULL )  
		return JS_FALSE;

	delete toDelete;
	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, obj );
	JS_SetPrivate( cx, obj, NULL );

	return JS_TRUE;
}

JSBool CPacket_WriteByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CPacket_WriteByte) Invalid Number of Arguments %d, needs: 2 ", argc );
		return JS_TRUE;
	}
	
	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, obj ));
	if( myPacket == NULL )
	{
		MethodError( "(CPacket_WriteByte) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>(JSVAL_TO_INT( argv[0] ));
	UI08	toWrite		= static_cast<UI08>(JSVAL_TO_INT( argv[1] ));

	myPacket->GetPacketStream().WriteByte( position, toWrite );

	return JS_TRUE;
}

JSBool CPacket_WriteShort( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CPacket_WriteShort) Invalid Number of Arguments %d, needs: 2 ", argc );
		return JS_TRUE;
	}
	
	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, obj ));
	if( myPacket == NULL )
	{
		MethodError( "(CPacket_WriteShort) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>(JSVAL_TO_INT( argv[0] ));
	UI16	toWrite		= static_cast<UI16>(JSVAL_TO_INT( argv[1] ));

	myPacket->GetPacketStream().WriteShort( position, toWrite );

	return JS_TRUE;
}

JSBool CPacket_WriteLong( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CPacket_WriteLong) Invalid Number of Arguments %d, needs: 2 ", argc );
		return JS_TRUE;
	}
	
	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, obj ));
	if( myPacket == NULL )
	{
		MethodError( "(CPacket_WriteLong) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>(JSVAL_TO_INT( argv[0] ));
	UI32	toWrite		= static_cast<UI32>(JSVAL_TO_INT( argv[1] ));

	myPacket->GetPacketStream().WriteLong( position, toWrite );

	return JS_TRUE;
}

JSBool CPacket_WriteString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "(CPacket_WriteString) Invalid Number of Arguments %d, needs: 3 ", argc );
		return JS_TRUE;
	}
	
	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, obj ));
	if( myPacket == NULL )
	{
		MethodError( "(CPacket_WriteString) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t	position	= static_cast<size_t>(JSVAL_TO_INT( argv[0] ));
 	char *	toWrite		= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	size_t	len			= static_cast<size_t>(JSVAL_TO_INT( argv[2] ));


	myPacket->GetPacketStream().WriteString( position, toWrite, len );

	return JS_TRUE;
}

JSBool CPacket_ReserveSize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(CPacket_ReserveSize) Invalid Number of Arguments %d, needs: 1 ", argc );
		return JS_TRUE;
	}
	
	CPUOXBuffer *myPacket = static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, obj ));
	if( myPacket == NULL )
	{
		MethodError( "(CPacket_ReserveSize) Invalid Object Passed" );
		return JS_TRUE;
	}

	size_t len = static_cast<size_t>(JSVAL_TO_INT( argv[0] ));


	myPacket->GetPacketStream().ReserveSize( len );

	return JS_TRUE;
}

JSBool CSocket_Send( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(CSocket_Send) Invalid Number of Arguments %d, needs: 1 ", argc );
		return JS_TRUE;
	}

	CSocket *mSock			= static_cast<CSocket *>(JS_GetPrivate( cx, obj ));
	JSObject *jsObj			= JSVAL_TO_OBJECT( argv[0] );
	CPUOXBuffer *myPacket	= static_cast<CPUOXBuffer *>(JS_GetPrivate( cx, jsObj ));

	if( mSock == NULL || myPacket == NULL )
	{
		MethodError( "(CPacket_WriteString) Invalid Object Passed" );
		return JS_TRUE;
	}

	mSock->Send( myPacket );

	return JS_TRUE;
}

// JSGump constructor !
JSBool Gump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Allocate the GumpList here and "SetPrivate" it to the Object
	SEGump *toAdd = new SEGump;
	toAdd->one = new STRINGLIST;
	toAdd->two = new STRINGLIST;
	toAdd->TextID = 0;

	JS_DefineFunctions( cx, obj, CGump_Methods );
	JS_SetPrivate( cx, obj, toAdd);
	JS_LockGCThing( cx, obj );
	//JS_AddRoot( cx, &obj );
	return JS_TRUE;
}

// Gump related functions
JSBool CGump_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SEGump *toDelete = (SEGump*)JS_GetPrivate( cx, obj );

	if( toDelete == NULL )  
		return JS_FALSE;
	
	delete toDelete->one;
	delete toDelete->two;
	delete toDelete;

	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, obj );
	JS_SetPrivate( cx, obj, NULL );

	return JS_TRUE;
}
//
JSBool CGumpData_Free(JSContext *cx, JSObject *obj,uintN argc, jsval *argv, jsval *rval)
{
	SEGumpData *toDelete = (SEGumpData*)JS_GetPrivate( cx, obj );

	if( toDelete == NULL )  
		return JS_FALSE;
	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, &obj );
	JS_SetPrivate( cx, obj, NULL ) ;

	delete toDelete;
	return JS_TRUE;
}
//
JSBool CGumpData_getEdit( JSContext *cx, JSObject *obj, uintN argc,jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getID) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = STRING_TO_JSVAL("");
		return JS_TRUE;
	}
	
	SEGumpData *myItem = (SEGumpData *)JS_GetPrivate( cx, obj );
	
	if( myItem == NULL  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned");
		*rval = STRING_TO_JSVAL("") ;
		return JS_TRUE;
	}

	size_t index = JSVAL_TO_INT(argv[0]); 
	
	if( index < myItem->sEdits.size() )
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ( cx,myItem->sEdits[index].c_str()));
	else
		*rval = STRING_TO_JSVAL("") ;
	return JS_TRUE;
}
//
JSBool CGumpData_getID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getID) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	
	SEGumpData *myItem = (SEGumpData *)JS_GetPrivate( cx, obj );
	
	if( myItem == NULL  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned");
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	size_t index = JSVAL_TO_INT(argv[0]); 
	
	if( index < myItem->nIDs.size() )
		*rval = INT_TO_JSVAL(myItem->nIDs[index]);
	else
		*rval = INT_TO_JSVAL(-1) ;
	
	return JS_TRUE;
}
//
JSBool CGumpData_getButton( JSContext *cx, JSObject *obj, uintN argc,jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getbutton) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	
	SEGumpData *myItem = (SEGumpData *)JS_GetPrivate( cx, obj );
	
	if( myItem == NULL  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned");
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	size_t index = JSVAL_TO_INT(argv[0]); 
	if( index < myItem->nButtons.size())
		*rval = INT_TO_JSVAL(myItem->nButtons[index]);
	else
		*rval = INT_TO_JSVAL(-1) ;
	
	return JS_TRUE;
}
//

JSBool CGump_AddCheckbox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError("AddCheckbox: Wrong count of Parameters, needs 5");
		return JS_FALSE;
	}

	SI16 tL			= (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR			= (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage		= (UI16)JSVAL_TO_INT( argv[2] );
	SI16 initState	= (SI16)JSVAL_TO_INT( argv[3] );
	SI16 relay		= (SI16)JSVAL_TO_INT( argv[4] );


	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "Gump_AddCheckbox: Couldn't find gump associated with object");
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "checkbox %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, initState, relay );
	gList->one->push_back( temp );
	
	return JS_TRUE;
}

JSBool CGump_NoClose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError("NoClose: Wrong count of Parameters, needs 0");
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoClose: Couldn't find gump associated with object");
		return JS_FALSE;
	}

	gList->one->push_back( "noclose" );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_NoMove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	9th February, 2003
//|	Developer(s)	-	Xuri
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify nomove on gumps created
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_NoMove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoMove: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoMove: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "nomove" );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_NoDispose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify nodispose on create
//| Comments		-	What nodispose is, I have no idea what so ever
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_NoDispose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoDispose: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoDispose: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "nodispose" );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_NoResize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify noresize on create
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_NoResize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoResize: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump *)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoResize: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "noresize" );
	
	return JS_TRUE;
}

JSBool CGump_AddBackground( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "AddBackground: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 bL = (SI16)JSVAL_TO_INT( argv[2] );
	SI16 bR = (SI16)JSVAL_TO_INT( argv[3] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[4] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddBackground: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "resizepic %i %i %i %i %i", tL, tR, gImage, bL, bR );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_AddButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		MethodError( "AddButton: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );
	SI16 x1 = (SI16)JSVAL_TO_INT( argv[3] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[4] );
	SI16 x3 = (SI16)JSVAL_TO_INT( argv[5] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddButton: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "button %i %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, x1, x2, x3 );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_AddPageButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddPageButton: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );
	SI16 pageNum = (SI16)JSVAL_TO_INT( argv[3] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddPageButton: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "button %i %i %i %i 0 %i", tL, tR, gImage, gImage + 1, pageNum );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddCheckerTrans( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to tile a particular gump image
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddCheckerTrans( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddCheckerTrans: Invalid number of arguments (takes 5 x y width height)" );
		return JS_FALSE;
	}

	SI16 x		= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y		= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width	= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height	= (SI16)JSVAL_TO_INT( argv[3] ); // height

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddCheckerTrans: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "checkertrans %i %i %i %i", x, y, width, height );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddCroppedText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify a text field cropped to certain
//|						dimensions
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddCroppedText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		MethodError( "AddCroppedText: Invalid number of arguments (takes 6 x y hue width height text)" );
		return JS_FALSE;
	}

	SI16 TextX		= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 TextY		= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 TextHue	= (SI16)JSVAL_TO_INT( argv[2] ); // Hue
	SI16 TextWidth	= (SI16)JSVAL_TO_INT( argv[3] ); // width
	SI16 TextHeight	= (SI16)JSVAL_TO_INT( argv[4] ); // height

 	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[5] ) );

	if( TextString == NULL )
	{
		MethodError( "AddCroppedText: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddCroppedText: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	UI32 TextID = gList->TextID;
	++gList->TextID;
	
	char temp[256];
	sprintf( temp, "croppedtext %i %i %i %i %i %lu", TextX, TextY, TextWidth, TextHeight, TextHue, TextID );
	gList->one->push_back( temp );
	gList->two->push_back( TextString );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddGroup( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify a new group to work with
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddGroup( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddGroup: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddGroup: Couldn't find gump associated with object" );
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "group %li", JSVAL_TO_INT( argv[0] ) );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_AddGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "AddGump: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	
	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );
	
	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "gumppic %i %i %i", tL, tR, gImage );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	16th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a new HTML gump to the stream of the gump
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddHTMLGump: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x				= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y				= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width			= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height			= (SI16)JSVAL_TO_INT( argv[3] ); // height
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );

 	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[6] ) );

	if( TextString == NULL )
	{
		MethodError( "AddHTMLGump: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddHTMLGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	UI32 TextID = gList->TextID;
	++gList->TextID;
	
	SI32 iBrd	= (hasBorder?1:0);
	SI32 iScrl	= (hasScrollbar?1:0);
	char temp[256];
	sprintf( temp, "htmlgump %i %i %i %i %lu %li %li", x, y, width, height, TextID, iBrd, iScrl );
	gList->one->push_back( temp );
	gList->two->push_back( TextString );

	return JS_TRUE;
}

JSBool CGump_AddPage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddPage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddPage: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "page %li", JSVAL_TO_INT( argv[0] ) );
	gList->one->push_back( temp );

	return JS_TRUE;
}
JSBool CGump_AddPicture( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "AddPicture: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddPicture: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "tilepic %i %i %i", tL, tR, gImage );
	gList->one->push_back( temp );

	return JS_TRUE;
}
JSBool CGump_AddRadio( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "Gump_AddRadio: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	SI16 tL				= (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR				= (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage			= (UI16)JSVAL_TO_INT( argv[2] );
	SI16 initialState	= (SI16)JSVAL_TO_INT( argv[3] );
	SI16 relay			= (SI16)JSVAL_TO_INT( argv[4] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddRadio: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "radio %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, initialState, relay );
	gList->one->push_back( temp );

	return JS_TRUE;
}
JSBool CGump_AddText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddText: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	UI32 TextID;
	
	SI16 TextX = (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 TextY = (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 TextHue = (SI16)JSVAL_TO_INT( argv[2] ); // Hue

 	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[3] ) );

	if( TextString == NULL )
	{
		MethodError( "AddText: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddText: Couldn't find gump associated with object");
		return JS_FALSE;
	}

	TextID = gList->TextID;
	++gList->TextID;
	
	char temp[256];
	sprintf( temp, "text %i %i %i %lu", TextX, TextY, TextHue, TextID );
	gList->one->push_back( temp );
	gList->two->push_back( TextString );

	return JS_TRUE;
}

JSBool CGump_AddTextEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		MethodError( "AddTextEntry: Invalid number of arguments (takes 8)" );
		return JS_FALSE;
	}
	
	SI16 tL					= (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR					= (SI16)JSVAL_TO_INT( argv[1] );
	SI16 width				= (SI16)JSVAL_TO_INT( argv[2] );
	SI16 height				= (SI16)JSVAL_TO_INT( argv[3] );
	SI16 hue				= (SI16)JSVAL_TO_INT( argv[4] );
	SI16 relay				= (SI16)JSVAL_TO_INT( argv[5] );
	SI16 initialTextIndex	= (SI16)JSVAL_TO_INT( argv[6] );
 	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[7] ) );

	if( test == NULL )
	{
		MethodError( "AddTextEntry: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddTextEntry: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "textentry %i %i %i %i %i %i %i", tL, tR, width, height, hue, relay, initialTextIndex );
	gList->one->push_back( temp );
	gList->two->push_back( test );

	return JS_TRUE;
}
//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddTiledGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to tile a particular gump image
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddTiledGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "AddTiledGump: Invalid number of arguments (takes 5 x y width height gump)" );
		return JS_FALSE;
	}

	SI16 x		= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y		= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width	= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height	= (SI16)JSVAL_TO_INT( argv[3] ); // height
	SI16 gumpID = (SI16)JSVAL_TO_INT( argv[4] ); // gump to tile

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddTiledGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "gumppictiled %i %i %i %i %i", x, y, width, height, gumpID );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	16th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a new HTML gump to the stream of the gump
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddXMFHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddXMFHTMLGump: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x				= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y				= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width			= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height			= (SI16)JSVAL_TO_INT( argv[3] ); // height
	SI32 number			= (SI32)JSVAL_TO_INT( argv[4] ); // number
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddXMFHTMLGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= (hasBorder?1:0);
	SI32 iScrl	= (hasScrollbar?1:0);
	char temp[256];
	sprintf( temp, "xmfhtmlgump %i %i %i %i %li %li %li", x, y, width, height, number, iBrd, iScrl );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddXMFHTMLGumpColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	16th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a new HTML gump to the stream of the gump
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddXMFHTMLGumpColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		MethodError( "AddXMFHTMLGumpColor: Invalid number of arguments (takes 8)" );
		return JS_FALSE;
	}

	SI16 x				= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y				= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width			= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height			= (SI16)JSVAL_TO_INT( argv[3] ); // height
	SI32 number			= (SI32)JSVAL_TO_INT( argv[4] ); // number
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );
	SI32 rgbColour		= (SI32)JSVAL_TO_INT( argv[7] );	// colour

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddXMFHTMLGumpColor: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= (hasBorder?1:0);
	SI32 iScrl	= (hasScrollbar?1:0);
	char temp[256];
	sprintf( temp, "xmfhtmlgumpcolor %i %i %i %i %li %li %li %li", x, y, width, height, number, iBrd, iScrl, rgbColour );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_Send( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Send: Invalid number of arguments (takes 1, socket or char)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ) )
	{
		MethodError( "You have to pass a valid Socket or Character" );
	}

	JSEncapsulate myClass( cx, &(argv[0]) );

	SEGump *myGump = (SEGump*)JS_GetPrivate( cx, obj );

	if( myClass.ClassName() == "UOXSocket" ) 
	{
		CSocket *mySock = (CSocket *)myClass.toObject();
		if( mySock == NULL ) 
		{
			MethodError( "Send: Passed an invalid Socket" );
			return JS_FALSE;
		}
		UI32 gumpID = (0xFFFF + JSMapping->GetScriptID( JS_GetGlobalObject( cx ) ));
		SendVecsAsGump( mySock, *(myGump->one), *(myGump->two), gumpID, INVALIDSERIAL );
	}
	else if( myClass.ClassName() == "UOXChar" ) 
	{
		CChar *myChar = (CChar*)myClass.toObject();
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "Send: Passed an invalid Character" );
			return JS_FALSE;
		}

		CSocket *mySock = myChar->GetSocket();
		UI32 gumpID = (0xFFFF + JSMapping->GetScriptID( JS_GetGlobalObject( cx ) ));
		SendVecsAsGump( mySock, *(myGump->one), *(myGump->two), gumpID, INVALIDSERIAL );
	}
	else
	{
		MethodError( "Send: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	return JS_TRUE;
}

// Character related methods!
JSBool CBase_TextMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 3 )
	{
		MethodError( "TextMessage: Invalid number of arguments (takes 1 - 3)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CBaseObject *myObj		= (CBaseObject*)myClass.toObject();

	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );
	if( trgMessage == NULL )
	{
		MethodError( "You have to supply a messagetext" );
		return JS_FALSE;
	}

	UI16 txtHue = 0x0000;
	if( argc == 3 )
		txtHue = static_cast<UI16>(JSVAL_TO_INT( argv[2] ));
	
	if( myClass.ClassName() == "UOXItem" ) 
	{
		CItem *myItem = (CItem *)myObj;
		if( !ValidateObject( myItem ) )  
		{
			MethodError( "TextMessage: Invalid Item" );
			return JS_FALSE;
		}
		if( !txtHue )
			txtHue = 0x047F;
		MethodSpeech( *myItem, trgMessage, OBJ, txtHue, FNT_NORMAL );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = (CChar *)myObj;
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "TextMessage: Invalid Character" );
			return JS_FALSE;
		}

		SpeechTarget spTrg = SPTRG_PCNPC;
		if( argc >= 2 && JSVAL_TO_BOOLEAN( argv[1] ) != JS_TRUE )
			spTrg = SPTRG_INDIVIDUAL;

		if( myChar->GetNPCAiType() == AI_EVIL )
		{
			if( !txtHue )
				txtHue = 0x0026;
			MethodSpeech( *myChar, trgMessage, TALK, txtHue, (FontType)myChar->GetFontType(), spTrg );
		}
		else
		{
			if( !txtHue )
				txtHue = myChar->GetSayColour();
			MethodSpeech( *myChar, trgMessage, TALK, txtHue, (FontType)myChar->GetFontType(), spTrg );
		}
	}	

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_KillTimers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	04/20/2002
//|	Developer(s)	-	Rukus
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Kill all related timers that have been association with
//|									an item or character.
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CBase_KillTimers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc != 0 )
	{
		MethodError( "KillTimers: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>(JS_GetPrivate(cx, obj));
	if( myObj==NULL )
	{
		MethodError("KillTimers: Invalid object assigned.");
		return JS_FALSE;
	}
	SERIAL mySer = myObj->GetSerial();
	cwmWorldState->tempEffects.Push();
	for( CTEffect *Effect = cwmWorldState->tempEffects.First(); !cwmWorldState->tempEffects.Finished(); Effect = cwmWorldState->tempEffects.Next() )
	{
		if( mySer == Effect->Destination() )
			cwmWorldState->tempEffects.Remove( Effect, true );
	}
	cwmWorldState->tempEffects.Pop();
	return JS_TRUE;
}


JSBool CBase_Delete( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Delete: Invalid object");
		return JS_FALSE;
	}

	myObj->Delete();

	return JS_TRUE;
}

JSBool CChar_Wander( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		MethodError( "Wander: Invalid number of arguments (takes 3-4, coordinates)" );
		return JS_FALSE;
	}

	SI16 x1 = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 y1 = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[2] );
	SI16 y2 = 0;
	if( argc > 3 )
		y2 = (SI16)JSVAL_TO_INT( argv[3] );

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) || !myChar->IsNpc() )
	{
		MethodError( "WanderBox: Invalid character" );
		return JS_FALSE;
	}
	myChar->SetFx( x1, 0 );
	myChar->SetFy( y1, 0 );
	myChar->SetFz( -1 );
	myChar->SetFx( x2, 1 );
	if( argc > 3 )
	{
		myChar->SetFy( y2, 1 );
		myChar->SetNpcWander( WT_BOX );
	}
	else
		myChar->SetNpcWander( WT_CIRCLE );

	return JS_TRUE;
}

JSBool CChar_Follow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Follow: Invalid number of arguments (takes 1, char object)" );
		return JS_FALSE;
	}

	JSObject *jsObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *myObj = (CBaseObject *)JS_GetPrivate( cx, jsObj );

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) || !myChar->IsNpc() || !ValidateObject( myObj ) || myObj->GetSerial() >= BASEITEMSERIAL )
	{
		MethodError( "Follow: Invalid character" );
		return JS_FALSE;
	}
	
	myChar->SetFTarg( (CChar*)myObj );
	myChar->SetNpcWander( WT_FOLLOW );

	return JS_TRUE;
}

JSBool CChar_DoAction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "DoAction: Invalid number of arguments (takes 1, action)" );
		return JS_FALSE;
	}

	UI16 targAction = static_cast<UI16>(JSVAL_TO_INT( argv[0] ));
	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ));

	if( !ValidateObject( myChar ) )
	{
		MethodError( "Action: Invalid character" );
		return JS_FALSE;
	}
	Effects->PlayCharacterAnimation( myChar, targAction );
	return JS_TRUE;
}

JSBool CChar_EmoteMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "EmoteMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	
	CChar *myChar = static_cast<CChar*>(JS_GetPrivate(cx, obj));
	JSString *targMessage = JS_ValueToString( cx, argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );
	
	if( !ValidateObject( myChar ) || trgMessage == NULL )
	{
		MethodError( "EmoteMessage: Invalid character or speech");
		return JS_FALSE;
	}
	MethodSpeech( *myChar, trgMessage, EMOTE, myChar->GetEmoteColour(), (FontType)myChar->GetFontType() );
	return JS_TRUE;
}

JSBool CChar_Dismount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Dismount: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	
	CChar *myChar = static_cast<CChar*>(JS_GetPrivate(cx, obj));
	
	if( !ValidateObject( myChar ) )
	{
		MethodError( "Dismount: Invalid character");
		return JS_FALSE;
	}
	if( myChar->IsOnHorse() )
		DismountCreature( myChar );
	return JS_TRUE;
}

JSBool CMisc_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SysMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CSocket *mySock		= NULL;
	JSEncapsulate myClass( cx, obj );

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar	= (CChar*)myClass.toObject();
		mySock			= myChar->GetSocket();
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		mySock			= (CSocket*)myClass.toObject();
	}

	if( mySock == NULL )
	{
		MethodError( "SysMessage: invalid socket" );
		return JS_FALSE;
	}

	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );

	if( trgMessage == NULL )
	{
		MethodError( "SysMessage: Invalid speech (%s)", targMessage );
		return JS_FALSE;
	}
	mySock->sysmessage( trgMessage );
	return JS_TRUE;
}

JSBool CSocket_Disconnect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Disconnect: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	
	CSocket *targSock = (CSocket*)JS_GetPrivate( cx, obj );

	if( targSock == NULL )
	{
		MethodError( "SysMessage: Invalid socket" );
		return JS_FALSE;
	}
	
	Network->Disconnect( targSock );
	JS_SetPrivate( cx, obj, NULL ); // yes we should do that...

	return JS_TRUE;
}

//
//	The following assumptions are made:
//	1 argument: Split the list by , (1,1,1,1) i.e.
//	2 arguments: x + y
//	3 arguments: x + y + z
//	4 arguments: x + y + z + world
//
JSBool CBase_Teleport( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSEncapsulate myClass( cx, obj );

	CBaseObject *myObj	= (CBaseObject*)myClass.toObject();
	SI16 x				= -1;
	SI16 y				= -1;
	SI08 z				= myObj->GetZ();
	UI08 world			= myObj->WorldNumber();

	switch( argc )
	{
		// Just Teleport...
		case 0:
			if( myClass.ClassName() == "UOXChar" )
			{
				((CChar*)myObj)->Teleport();
				return JS_TRUE;
			}
			MethodError( "For Items you need at least one parameter for Teleport" );
			break;

		// Parameters as a string
		case 1:
			if( JSVAL_IS_OBJECT( argv[0] ) )
			{	// we can work with this, it should be either a character or item, hopefully
				JSEncapsulate jsToGoTo( cx, &(argv[0]) );
				if( jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar" )
				{
					CBaseObject *toGoTo = (CBaseObject *)jsToGoTo.toObject();
					if( !ValidateObject( toGoTo ) )
					{
						MethodError( "No object associated with this object" );
						break;
					}
					x		= toGoTo->GetX();
					y		= toGoTo->GetY();
					z		= toGoTo->GetZ();
					world	= toGoTo->WorldNumber();
				}
				else if( jsToGoTo.ClassName() == "UOXSocket" )
				{
					CSocket *mySock		= (CSocket *)jsToGoTo.toObject();
					CChar *mySockChar	= mySock->CurrcharObj();
					x					= mySockChar->GetX();
					y					= mySockChar->GetY();
					z					= mySockChar->GetZ();
					world				= mySockChar->WorldNumber();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
			}
			else if( JSVAL_IS_INT( argv[0] ) )
			{
				size_t placeNum = JSVAL_TO_INT( argv[0] );
				if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
				{
					GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
					x		= toGoTo.x;
					y		= toGoTo.y;
					z		= toGoTo.z;
					world	= toGoTo.worldNum;
				}
			}
			else	// Needs to be implemented
				MethodError( "Text-styled Parameters may be added later" );
			break;

		// 2 Parameters, x + y
		case 2: 
			x = (SI16)JSVAL_TO_INT( argv[0] );
			y = (SI16)JSVAL_TO_INT( argv[1] );
			break;

		// x,y,z
		case 3:
			x = (SI16)JSVAL_TO_INT( argv[0] );
			y = (SI16)JSVAL_TO_INT( argv[1] );
			z = (SI08)JSVAL_TO_INT( argv[2] );
			break;

		// x,y,z,world
		case 4:
			x		= (SI16)JSVAL_TO_INT( argv[0] );
			y		= (SI16)JSVAL_TO_INT( argv[1] );
			z		= (SI08)JSVAL_TO_INT( argv[2] );
			world	= (UI08)JSVAL_TO_INT( argv[3] );
			break;
		default:
			MethodError( "Invalid number of arguments passed to Teleport, needs either 1, 2, 3 or 4" );
			break;
	}

	if( myClass.ClassName() == "UOXItem" ) 
	{
		CItem *myItem = (CItem*)myObj;
		if( !ValidateObject( myItem ) )  
		{
			MethodError( "Teleport: Invalid Item" );
			return JS_FALSE;
		}

		myItem->SetLocation( x, y, z, world );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = (CChar*)myObj;
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "Teleport: Invalid Character" );
			return JS_FALSE;
		}

		if( world != myChar->WorldNumber() && !myChar->IsNpc() )
		{
			CSocket *mySock = myChar->GetSocket();
			if( mySock == NULL ) 
				return JS_TRUE;
			if( !Map->InsideValidWorld( x, y, world) )
			{
				MethodError( "Teleport: Not a valid World" );
				return JS_FALSE;
			}
			myChar->SetLocation( x, y, z, world );
			SendMapChange( world, mySock );
		} 
		else 
			myChar->SetLocation( x, y, z, world );
	}	

	return JS_TRUE;
}

JSBool CBase_StaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI16 effectID		= (UI16)JSVAL_TO_INT( argv[0] );
	UI08 speed			= (UI08)JSVAL_TO_INT( argv[1] );
	UI08 loop			= (UI08)JSVAL_TO_INT( argv[2] );

	JSEncapsulate		myClass( cx, obj );
	CBaseObject *myObj	= (CBaseObject*)myClass.toObject();
	
	if( !ValidateObject( myObj ) )
	{
		MethodError( "StaticEffect: Invalid Object" );
		return JS_FALSE;
	}

	if( myClass.ClassName() == "UOXItem" )
	{
		bool explode = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );
		Effects->PlayStaticAnimation( myObj, effectID, speed, loop, explode );		
	}
	else
	{
		Effects->PlayStaticAnimation( myObj, effectID, speed, loop );
	}

	return JS_TRUE;
}

JSBool CMisc_MakeMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "MakeMenu: Invalid number of arguments (takes 2, number of menu, skill used)" );
		return JS_FALSE;
	}

	CSocket *mySock		= NULL;
	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar	= (CChar*)myClass.toObject();
		mySock			= myChar->GetSocket();
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		mySock			= (CSocket*)myClass.toObject();
	}

	if( mySock == NULL )
	{
		MethodError( "MakeMenu: invalid socket" );
		return JS_FALSE;
	}
	int menu		= JSVAL_TO_INT( argv[0] );
	UI08 skillNum	= static_cast<UI08>(JSVAL_TO_INT( argv[1] ));
	Skills->NewMakeMenu( mySock, menu, skillNum );
	return JS_TRUE;
}

JSBool CMisc_SoundEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SoundEffect: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );

	UI16 sound = (UI16)JSVAL_TO_INT( argv[0] );
	bool allHear = ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" )
	{
		CBaseObject *myObj = (CBaseObject*)myClass.toObject();

		if( ValidateObject( myObj ) )
		{
			Effects->PlaySound( myObj, sound, allHear );
		}
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = (CSocket*)myClass.toObject();

		if( mySock != NULL )
			Effects->PlaySound( mySock, sound, allHear );
	}

	return JS_TRUE;
}

JSBool CMisc_SellTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = JSVAL_FALSE;
	if( argc != 1 )
	{
		MethodError( "SellTo: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *myNPC		= (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( myNPC ) )
	{
		MethodError( "SellTo: Invalid NPC" );
		return JS_FALSE;
	}

	CPSellList toSend;
	if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = (CSocket*)myClass.toObject();
		if( mySock == NULL )
		{
			MethodError( "Passed an invalid socket to SellTo" );
			return JS_FALSE;
		}

		CChar *mChar = mySock->CurrcharObj();
		if( ValidateObject( mChar ) )
		{
			myNPC->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60.0f ) );
			if( toSend.CanSellItems( (*mChar), (*myNPC) ) )
			{
				mySock->Send( &toSend );
				*rval = JSVAL_TRUE;
			}
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = (CChar*)myClass.toObject();
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Passed an invalid char to SellTo" );
			return JS_FALSE;
		}		

		myNPC->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60.0f ) );
		CSocket *mSock = myChar->GetSocket();
		if( toSend.CanSellItems( (*myChar), (*myNPC) ) )
		{
				mSock->Send( &toSend );
				*rval = JSVAL_TRUE;
		}
	}

	return JS_TRUE;
}

JSBool CMisc_BuyFrom( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SellTo: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *myNPC		= (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( myNPC ) )
	{
		MethodError( "BuyFrom: Invalid NPC" );
		return JS_FALSE;
	}

	if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mySock = (CSocket *)myClass.toObject();
		if( mySock == NULL )
		{
			MethodError( "Invalid source socket in BuyFrom" );
			return JS_FALSE;
		}

		if( myNPC->GetNPCAiType() == AI_PLAYERVENDOR )
		{
			mySock->TempObj( myNPC );
			myNPC->TextMessage( mySock, 772, TALK, false );
			mySock->target( 0, TARGET_PLVBUY, " ");
		} 
		else
			BuyShop( mySock, myNPC );
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = (CChar*)myClass.toObject();
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Passed an invalid char to BuyFrom" );
			return JS_FALSE;
		}		

		CSocket *mySock = myChar->GetSocket();	

		if( myNPC->GetNPCAiType() == AI_PLAYERVENDOR )
		{
			mySock->TempObj( myNPC );
			myNPC->TextMessage( mySock, 772, TALK, false );
			mySock->target( 0, TARGET_PLVBUY, " ");
		} 
		else
			BuyShop( mySock, myNPC );
	}

	return JS_TRUE;
}

JSBool CMisc_HasSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "HasSpell: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	UI08 SpellID = (UI08)JSVAL_TO_INT( argv[0] );

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = (CChar*)myClass.toObject();
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Invalid char for HasSpell" );
			return JS_FALSE;
		}

		CItem *myItem = FindItemOfType( myChar, IT_SPELLBOOK );

		if( !ValidateObject( myItem ) )
		{
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
			return JS_TRUE;
		}

		if( Magic->HasSpell( myItem, SpellID ) )
			*rval = BOOLEAN_TO_JSVAL( JS_TRUE );
		else
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
	}
	else if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = (CItem*)myClass.toObject();
		if( !ValidateObject( myItem ) )
		{
			MethodError( "Invalid item for HasSpell" );
			return JS_FALSE;
		}		

		if( Magic->HasSpell( myItem, SpellID ) )
			*rval = BOOLEAN_TO_JSVAL( JS_TRUE );
		else
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
	}

	return JS_TRUE;
}

JSBool CMisc_RemoveSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveSpell: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	UI08 SpellID = (UI08)JSVAL_TO_INT( argv[0] );

	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar = (CChar*)myClass.toObject();
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Invalid char for HasSpell" );
			return JS_FALSE;
		}

		CItem *myItem = FindItemOfType( myChar, IT_SPELLBOOK );

		if( ValidateObject( myItem ) )
		{
			Magic->RemoveSpell( myItem, SpellID );			
		}
	}
	else if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = (CItem*)myClass.toObject();
		if( !ValidateObject( myItem ) )
		{
			MethodError( "Invalid item for RemoveSpell" );
			return JS_FALSE;
		}
		
		Magic->RemoveSpell( myItem, SpellID );
	}

	return JS_TRUE;
}

JSBool CBase_GetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Invalid Count of Parameters: %d, need: 1", argc );
	}
	
	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Invalid Object assigned (GetTag)" );
		return JS_FALSE;
	}
	
	std::string localString		= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	TAGMAPOBJECT localObject	= myObj->GetTag( localString );
	if( localObject.m_ObjectType == TAGMAP_TYPE_STRING )
	{
		JSString *localJSString = JS_NewString( cx, (char*)localObject.m_StringValue.c_str(),localObject.m_StringValue.length() );
		*rval = (jsval)STRING_TO_JSVAL(localJSString);
	}
	else if( localObject.m_ObjectType == TAGMAP_TYPE_BOOL )
		*rval = (jsval)BOOLEAN_TO_JSVAL((localObject.m_IntValue == 1));
	else
		*rval = (jsval)INT_TO_JSVAL(localObject.m_IntValue);
	return JS_TRUE;
}

JSBool CBase_SetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc != 2 ) && ( argc != 1 ))
	{
		MethodError( "SetTag: Invalid Count of Parameters: %d, need: 2", argc );
	}
	
	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Invalid Object assigned (SetTag)" );
		return JS_FALSE;
	}

	std::string localString = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	TAGMAPOBJECT localObject;
	if( argc == 2 )
	{
		JSEncapsulate encaps( cx, &(argv[1]) );
		if( encaps.isType( JSOT_STRING ) )
		{			// String value handling
			const std::string stringVal = encaps.toString();
			if( stringVal == "" )
			{
				localObject.m_Destroy		= TRUE;
				localObject.m_IntValue		= 0;
				localObject.m_ObjectType	= TAGMAP_TYPE_INT;
				localObject.m_StringValue	= "";
			}
			else
			{
				localObject.m_Destroy		= FALSE;
				localObject.m_StringValue	= stringVal;
				localObject.m_IntValue		= localObject.m_StringValue.length();
				localObject.m_ObjectType	= TAGMAP_TYPE_STRING;
			}			
		}
		else if( encaps.isType( JSOT_BOOL ) )
		{
			const bool boolVal = encaps.toBool();
			if( !boolVal )
			{
				localObject.m_Destroy		= TRUE;
				localObject.m_IntValue		= 0;
			}
			else
			{
				localObject.m_Destroy		= FALSE;
				localObject.m_IntValue		= 1;
			}
			localObject.m_ObjectType	= TAGMAP_TYPE_BOOL;
			localObject.m_StringValue	= "";
		}
		else if( encaps.isType( JSOT_INT ) )
		{
			const int intVal = encaps.toInt();
			if( !intVal )
			{
				localObject.m_Destroy		= TRUE;
				localObject.m_IntValue		= 0;
			}
			else
			{
				localObject.m_Destroy		= FALSE;
				localObject.m_IntValue		= intVal;
			}
			localObject.m_ObjectType	= TAGMAP_TYPE_INT;
			localObject.m_StringValue	= "";
		}
		else if( encaps.isType( JSOT_NULL ) )
		{
			localObject.m_Destroy		= TRUE;
			localObject.m_IntValue		= 0;
			localObject.m_ObjectType	= TAGMAP_TYPE_INT;
			localObject.m_StringValue	= "";
		}
		else
			return JS_TRUE;
		myObj->SetTag( localString, localObject );
	}
	else
	{
		localObject.m_Destroy		= TRUE;
		localObject.m_ObjectType	= TAGMAP_TYPE_INT;
		localObject.m_IntValue		= 0;
		localObject.m_StringValue	= "";
		myObj->SetTag( localString, localObject );
	}
	return JS_TRUE;
}

JSBool CBase_GetNumTags( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Invalid Count of Parameters: %d, need: 0", argc );
	}
	
	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Invalid Object assigned (GetTag)" );
		return JS_FALSE;
	}
	
	*rval = INT_TO_JSVAL( myObj->GetNumTags() );
	return JS_TRUE;
}

JSBool CChar_OpenBank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "OpenBank: Invalid Character object assigned" );
		return JS_FALSE;
	}

	CSocket *mySock = NULL;
	// Open the bank of myChar to myChar
	if( argc == 0 )
	{
		mySock = myChar->GetSocket();
		if( mySock != NULL )
			mySock->openBank( myChar );
	}
	// Open it to the passed socket
	else if( argc == 1 )
	{
		mySock = (CSocket*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
		if( mySock != NULL )
			mySock->openBank( myChar );
	}
	else
	{
		MethodError( "OpenBank, Invalid count of Paramters: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool CChar_OpenLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "OpenLayer: Invalid Character object assigned" );
		return JS_FALSE;
	}

	if( argc != 2 )
	{
		MethodError( "OpenLayer, Invalid count of Paramters: %d", argc );
		return JS_FALSE;
	}
	CSocket *mySock = (CSocket*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( mySock != NULL )
	{
		CItem *iLayer = myChar->GetItemAtLayer( (ItemLayers)JSVAL_TO_INT( argv[1] ) );
		if( ValidateObject( iLayer ) )
			mySock->openPack( iLayer );
	}

	return JS_TRUE;
}

JSBool CChar_TurnToward( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	//	
	//	2 Parameters: x + y
	//	1 Parameter: either item or character
	//	

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(TurnToward) Wrong object assigned" );
		return JS_FALSE;
	}

	SI16 x, y;

	if( argc == 1 )
	{
		if( !JSVAL_IS_OBJECT( argv[0] ) )
		{
			MethodError( "(TurnToward) Invalid Object passed" );
			return JS_FALSE;
		}

		CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if( argc == 2 )
	{
		// x + y
		x = (SI16)JSVAL_TO_INT( argv[0] );
		y = (SI16)JSVAL_TO_INT( argv[1] );
	}
	else
	{
		MethodError( "(TurnToward) Wrong paramter count: %d, needs either one char/item or x+y", argc );
		return JS_FALSE;
	}

	// Just don't do anything if NewDir eq OldDir
	
	UI08 NewDir = Movement->Direction( myChar, x, y );

	if( NewDir != myChar->GetDir() )
		myChar->SetDir( NewDir );

	return JS_TRUE;
}

JSBool CChar_DirectionTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	//	
	//	2 Parameters: x + y
	//	1 Parameter: either item or character
	//	

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(DirectionTo) Wrong object assigned" );
		return JS_FALSE;
	}

	SI16 x, y;

	if( argc == 1 )
	{
		if( !JSVAL_IS_OBJECT( argv[0] ) )
		{
			MethodError( "(DirectionTo) Invalid Object passed" );
			return JS_FALSE;
		}

		CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if( argc == 2 )
	{
		// x + y
		x = (SI16)JSVAL_TO_INT( argv[0] );
		y = (SI16)JSVAL_TO_INT( argv[1] );
	}
	else
	{
		MethodError( "(DirectionTo) Wrong paramter count: %d, needs either one char/item or x+y", argc );
		return JS_FALSE;
	}

	// Just don't do anything if NewDir eq OldDir
	
	UI08 NewDir = Movement->Direction( myChar, x, y );

	*rval = INT_TO_JSVAL( NewDir );

	return JS_TRUE;
}

JSBool CChar_ExecuteCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
 		MethodError( "ExecuteCommand: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
 	}
 	
 	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
 	CChar *myChar			= (CChar*)JS_GetPrivate( cx, obj );
 	char *trgMessage		= JS_GetStringBytes( targMessage );
 	CSocket *targSock		= myChar->GetSocket();
 	if( targSock == NULL || trgMessage == NULL )
 	{
 		MethodError( "ExecuteCommand: Invalid socket or speech (%s)", targMessage );
		return JS_FALSE;
 	}
 	Commands->Command( targSock, myChar, trgMessage );
	return JS_TRUE;
}

JSBool CGuild_AcceptRecruit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CGuild *myGuild = (CGuild*)JS_GetPrivate( cx, obj );

	if( myGuild == NULL )
	{
		MethodError( "(AcceptRecruit) Invalid Object assigned" );
		return JS_FALSE;
	}
	
	// Two choices here... 0 paramteres = accept the JS_GetParent object
	// 1 parameter = get the cchar from there
	if( argc == 0 )
	{
		JSObject *Parent = JS_GetParent( cx, obj );
		CChar *myChar = (CChar*)JS_GetPrivate( cx, Parent );
		myGuild->RecruitToMember( *myChar );
	}
	else if( argc == 1 )
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
		myGuild->RecruitToMember( *myChar );
	}
	else
	{
		MethodError( "(AcceptRecruit) Invalid Parameter Count: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool CChar_ResourceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(ResourceCount) Invalid Object assigned" );
		return JS_FALSE;
	}

	UI16 realID = (UI16)JSVAL_TO_INT( argv[0] );
	UI16 itemColour = 0;
		
	if(( argc < 1 ) || ( argc > 2 ))
	{
		MethodError( "(ResourceCount) Invalid count of parameters: %d, either needs 1 or 2" , argc );
		return JS_FALSE;
	}

	if( argc == 2 )
	{
		itemColour = (UI16)JSVAL_TO_INT( argv[1] );
	}
	
	*rval = INT_TO_JSVAL( GetItemAmount( myChar, realID, itemColour ) );
	return JS_TRUE;
}

JSBool CBase_UseResource( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSEncapsulate myClass( cx, obj );
	CBaseObject *myObj		= (CBaseObject*)myClass.toObject();

	if( !ValidateObject( myObj ) )
	{
		MethodError( "(UseResource) Invalid Object assigned" );
		return JS_FALSE;
	}

	UI16 realID		= (UI16)JSVAL_TO_INT( argv[1] );
	UI16 itemColour = 0;
	UI32 amount		= (UI32)JSVAL_TO_INT( argv[0] );	

	// Min. 2 Arguments (amount + id) or 3
	if(( argc < 2 ) || ( argc > 3 ))
	{
		MethodError( "(UseResource) Invalid count of parameters: %d, either needs 2 or 3" , argc );
		return JS_FALSE;
	}

	if( argc == 3 )
	{
		itemColour = (UI16)JSVAL_TO_INT( argv[2] );
	}

	UI32 retVal = 0;

	Console.Print("\n*****\nCharname: %s\nRealID: %i\nitemColor: %i\nAmount: %i\n*****\n", myObj->GetName().c_str(), realID, itemColour, amount );
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *myChar	= (CChar *)myObj;
		retVal			= DeleteItemAmount( myChar, amount, realID, itemColour );
	}
	else
	{
		CItem *myItem	= (CItem *)myObj;
		retVal			= DeleteSubItemAmount( myItem, amount, realID, itemColour );
	}
	*rval = INT_TO_JSVAL( retVal );
	return JS_TRUE;
}

JSBool CChar_BoltEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( ValidateObject( myChar ) )
		Effects->bolteffect( myChar );

	return JS_TRUE;
}

JSBool CMisc_CustomTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Either useable with sockets OR characters
	JSEncapsulate myClass( cx, obj );

	if(( argc > 2 ) || ( argc < 1 ))
	{
		MethodError( "(CustomTarget) Invalid count of parameters: %d, either needs 1 or 2", argc );
		return JS_FALSE;
	}

	CSocket *mySock = NULL;

	if( myClass.ClassName() == "UOXChar" )
	{
		// Character
		CChar *myChar = (CChar*)myClass.toObject();

		if( !ValidateObject( myChar ) )
		{
			MethodError( "(CustomTarget) Invalid Character assigned" );
			return JS_FALSE;
		}

		mySock = myChar->GetSocket();
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		// We have a socket here
		mySock = (CSocket*)myClass.toObject();
	}

	if( mySock == NULL )
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return JS_TRUE;
	}

	mySock->TempInt( (SI32)JSMapping->GetScript( JS_GetGlobalObject( cx ) ) );
	UI08 tNum = (UI08)JSVAL_TO_INT( argv[0] );
	char toSay[512] = { 0, }; // Could become long (make sure it's NULL )
 	
	if( argc == 2 )
	{
		sprintf( toSay, "%s", JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) ) );
	}

	mySock->target( 1, tNum, toSay );
	return JS_TRUE;
}

JSBool CMisc_PopUpTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc > 2 ) || ( argc < 1 ))
	{
		MethodError( "(PopUpTarget) Invalid count of parameters: %d, either needs 1 or 2", argc );
		return JS_FALSE;
	}

	// Either useable with sockets OR characters
	JSEncapsulate myClass( cx, obj );
	CSocket *mySock = NULL;

	if( myClass.ClassName() == "UOXChar" )
	{
		// Character
		CChar *myChar = (CChar*)myClass.toObject();

		if( !ValidateObject( myChar ) )
		{
			MethodError( "(PopUpTarget) Invalid Character assigned" );
			return JS_FALSE;
		}

		mySock = myChar->GetSocket();
	}

	else if( myClass.ClassName() == "UOXSocket" )
	{
		// We have a socket here
		mySock = (CSocket*)myClass.toObject();
	}

	if( mySock == NULL )
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return JS_TRUE;
	}

	UI08 tNum = (UI08)JSVAL_TO_INT( argv[0] );
	char toSay[512] = { 0, }; // Could become long (make sure it's NULL )

	if( argc == 2 )
	{
		sprintf( toSay, "%s", JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) ) );
	}

	mySock->target( 0, tNum, toSay );
	return JS_TRUE;
}

JSBool CBase_InRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(InRange): Invalid count of parameters: %d needs 2 (Item/Char and distance)", argc );
		return JS_FALSE;
	}

	UI16 distance = (UI16)JSVAL_TO_INT( argv[1] );
	
	CBaseObject *me = (CBaseObject*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( me ) )
	{
			MethodError( "(InRange) Invalid Object assigned to self" );
			return JS_FALSE;
	}

	CBaseObject *them = (CBaseObject*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( them ) )
	{
			MethodError( "(InRange) Invalid Object assigned to target" );
			return JS_FALSE;
	}

	if( ( them->GetObjType() == OT_ITEM ) && ( me->GetObjType() == OT_CHAR ) )
	{
		CItem *myItem = static_cast< CItem * >(them);
		if( myItem->GetCont() != NULL )
		{
			*rval = BOOLEAN_TO_JSVAL( FindItemOwner( myItem ) == me );
			return JS_TRUE;
		}
	}

	*rval = BOOLEAN_TO_JSVAL( objInRange( me, them, distance ) );
	return JS_TRUE;
}

JSBool CBase_StartTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CBaseObject *myObj = static_cast< CBaseObject* >( JS_GetPrivate( cx, obj ) );
	
	if( !ValidateObject( myObj ) )
	{
		MethodError( "(StartTimer) Invalid Object assigned" );
		return JS_FALSE;
	}

	if( argc != 2 && argc != 3 )
	{
		MethodError( "(StartTimer) Invalid count of parameter: %d (needs 2 or 3)", argc );
		return JS_FALSE;
	}

	// 1. Parameter Delay, 2. Parameter Callback
	UI32 ExpireTime = BuildTimeValue( JSVAL_TO_INT( argv[0] ) / 1000.0f );
	UI08 TriggerNum = (UI08)JSVAL_TO_INT( argv[1] );
	
	CTEffect *Effect = new CTEffect;

	if( argc == 3 )
	{
		if( JSVAL_IS_BOOLEAN( argv[2] ) )	// Is it a boolean?  If so, might be calling back into here
		{
			if( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE )
				Effect->AssocScript( JSMapping->GetScriptID( JS_GetGlobalObject( cx ) ) );
			else
				Effect->More2( 0xFFFF );
		}
		else
			Effect->More2( (UI16)JSVAL_TO_INT( argv[2] ) );
	}
	else
		Effect->More2( 0xFFFF );

	Effect->Destination( myObj->GetSerial() );
	Effect->ExpireTime( ExpireTime );
	Effect->Source( myObj->GetSerial() );
	Effect->Number( 40 );
	Effect->More1( TriggerNum );

	cwmWorldState->tempEffects.Add( Effect );
	
	return JS_TRUE;
}

JSBool CChar_CheckSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "CheckSkill: Invalid number of arguments (takes 3, skillNum, minSkill and maxSkill)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "CheckSkill: Invalid character" );
		return JS_FALSE;
	}

	UI08 skillNum = (UI08)JSVAL_TO_INT( argv[0] );
	UI16 minSkill = (UI16)JSVAL_TO_INT( argv[1] );
	UI16 maxSkill = (UI16)JSVAL_TO_INT( argv[2] );
	*rval = BOOLEAN_TO_JSVAL( Skills->CheckSkill( myChar, skillNum, minSkill, maxSkill ) );
	return JS_TRUE;
}

JSBool CChar_FindItemLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myItem = NULL;
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(FindItemLayer) Invalid Charobject assigned" );
		return JS_FALSE;
	}

	if( argc == 1 ) 
	{
		myItem = myChar->GetItemAtLayer( (ItemLayers)JSVAL_TO_INT( argv[0] ) );
	}
	else
	{
		MethodError( "(FindItemLayer) Unknown Count of Arguments: %d, needs: 1", argc );
		return JS_FALSE;
	}

	if( !ValidateObject( myItem ) )
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	JSObject *myJSItem	= JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );

	*rval = OBJECT_TO_JSVAL( myJSItem );

	return JS_TRUE;
}

JSBool CChar_FindItemType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(FindItemType) Invalid Count of Arguments, takes 1" );
		return JS_FALSE;
	}

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "(FindItemType) Invalid Charobject assigned" );
		return JS_FALSE;
	}

	UI08 iType = static_cast<UI08>(JSVAL_TO_INT( argv[0] ));

	CItem *myItem = FindItemOfType( myChar, static_cast<ItemTypes>(iType) );
	if( !ValidateObject( myItem ) )
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	JSObject *myJSItem	= JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );

	*rval = OBJECT_TO_JSVAL( myJSItem );

	return JS_TRUE;
}

// Open the Plank
void OpenPlank( CItem *p );
JSBool CItem_OpenPlank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(OpenPlank) Invalid Count of Arguments: %d, needs: 0", argc );
		return JS_FALSE;
	}

	CItem *myItem = (CItem*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myItem ) )
	{
		MethodError( "(OpenPlank) Invalid Object assigned" );
		return JS_FALSE;
	}

	OpenPlank( myItem );

	return JS_TRUE;
}

JSBool CChar_SpeechInput( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Get our own Script ID
	UI08 speechID		= 0;
	CItem *speechItem	= NULL;

	if( argc == 1 ) // Just the ID has been passed
	{
		speechID = (UI08)JSVAL_TO_INT( argv[0] );
	}
	else if( argc == 2 ) // We got an "affected" item as well
	{
		speechID = (UI08)JSVAL_TO_INT( argv[0] );
		
		if( argv[1] != JSVAL_NULL )
		{
			JSObject *myObj = JSVAL_TO_OBJECT( argv[1] );
			speechItem = (CItem*)JS_GetPrivate( cx, myObj );
		}		
	}
	else
	{
		MethodError( "(SpeechInput) Invalid Count of parameters: %d, needs: either 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(SpeechInput) Invalid object assigned" );
		return JS_FALSE;
	}

	myChar->SetSpeechMode( 9 );
	if( ValidateObject( speechItem ) )
		myChar->SetSpeechItem( speechItem );
	else
		myChar->SetSpeechItem( NULL );

	myChar->SetSpeechID( speechID );
	myChar->SetSpeechCallback( JSMapping->GetScript( JS_GetGlobalObject( cx ) ) );

	return JS_TRUE;
}

JSBool CChar_CastSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( ( argc != 1 ) && ( argc != 2 ) )
	{
		MethodError( "(CastSpell) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myChar ) )
	{
		MethodError( "(CastSpell) Invalid object assigned" );
		return JS_FALSE;
	}
	
	SI08 spellCast = (SI08)JSVAL_TO_INT( argv[0] );
	bool spellSuccess;

	if( myChar->IsNpc() )
	{
		myChar->SetSpellCast( spellCast );
		Magic->CastSpell( NULL, myChar );
	}
	else
	{
		CSocket *sock = myChar->GetSocket();
		
		if(( argc == 2 ) && ( argv[1] == JSVAL_TRUE ) )
		{
			// Next...
			myChar->SetSpellCast( spellCast );
			Magic->CastSpell( sock, myChar );
		}
		else
			spellSuccess = Magic->SelectSpell( sock, spellCast );
	}
	return JS_TRUE;
}

JSBool CChar_MagicEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SI08 spellnum = (SI08)JSVAL_TO_INT( argv[0] );

	CChar *myObj = (CChar*)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myObj ) )
	{
		MethodError( "StaticEffect: Invalid Object" );
		return JS_FALSE;
	}

	Magic->doStaticEffect( myObj, spellnum );

	return JS_TRUE;
}

JSBool CChar_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myObj = static_cast<CChar*>(JS_GetPrivate(cx, obj));
	UI08 part = (UI08)JSVAL_TO_INT( argv[0] );
	
	if( !ValidateObject( myObj ) || ( part <= 0 ) || ( part > 4 ) )
	{
		MethodError( "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)" );
		*rval = INT_TO_JSVAL( 0 );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetSerial( part ) );

	return JS_TRUE;
}

JSBool CChar_SetPoisoned( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 && argc > 3 )
	{
		MethodError( "(SetPoisoned) Invalid Number of Arguments %d, needs: 2 or 3", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myChar ) || myChar->GetObjType() != OT_CHAR )
	{
		MethodError( "(SetPoisoned) Invalid object assigned" );
		return JS_FALSE;
	}

	SI08 newVal = (SI08)JSVAL_TO_INT( argv[0] );

	if( argc > 1 )
	{
		SI32 wearOff = (SI32)JSVAL_TO_INT( argv[1] );
		
		if( argc == 2 || ( argc == 3 && JSVAL_TO_BOOLEAN( argv[2] ) ) )
		{
			if( myChar->GetPoisoned() > newVal )
				newVal = myChar->GetPoisoned();
		}
		myChar->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( (R32)wearOff / 1000.0f ) );
	}

	myChar->SetPoisonStrength( newVal );
	myChar->SetPoisoned( newVal );
	return JS_TRUE;
}

void explodeItem( CSocket *mSock, CItem *nItem );
JSBool CChar_ExplodeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(ExplodeItem) Invalid Number of Arguments %d, needs: 1", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );
	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *trgObj = (CBaseObject *)JS_GetPrivate( cx, tObj );
	
	if( !ValidateObject( trgObj ) || trgObj->GetObjType() != OT_ITEM || myChar->GetSocket() == NULL )
	{
		MethodError( "(ExplodeItem) Invalid object" );
		return JS_FALSE;
	}
	
	explodeItem( myChar->GetSocket(), (CItem *)trgObj );
	return JS_TRUE;
}

JSBool CChar_SetInvisible( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 && argc > 2 )
	{
		MethodError( "(SetInvisible) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );
	UI08 newVal = (UI08)JSVAL_TO_INT( argv[0] );

	myChar->SetVisible( (VisibleTypes)newVal );
	if( argc == 2 )
	{
		UI32 TimeOut = (UI32)JSVAL_TO_INT( argv[1] );
		myChar->SetTimer( tCHAR_INVIS, BuildTimeValue( (R32)TimeOut / 1000.0f ) );
	}
	return JS_TRUE;
}

JSBool CBase_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );
	UI08 part = (UI08)JSVAL_TO_INT( argv[0] );
	
	if( !ValidateObject( myObj ) || ( part <= 0 ) || ( part > 4 ) )
	{
		MethodError( "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)" );
		*rval = INT_TO_JSVAL( 0 );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetSerial( part ) );

	return JS_TRUE;
}

JSBool CItem_SetCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myItem = (CItem*)JS_GetPrivate( cx, obj );
	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *trgObj = (CBaseObject *)JS_GetPrivate( cx, tObj );

	if( !ValidateObject( myItem ) || !ValidateObject( trgObj ) || ( trgObj->GetSerial() == INVALIDSERIAL ) )
	{
		MethodError( "SetCont: Invalid Object/Argument, takes 1 arg: containerobject" );
		return JS_FALSE;
	}
	
	// return true if the change was successful, false otherwise
	*rval = BOOLEAN_TO_JSVAL( myItem->SetCont( trgObj ) );
	
	return JS_TRUE;
}

JSBool CItem_IsMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(IsMulti) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) )
	{
		MethodError( "(IsMulti) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	
	*rval = BOOLEAN_TO_JSVAL( myItem->CanBeObjType( OT_MULTI ) );
	return JS_TRUE;
}

JSBool CItem_IsInMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsInMulti) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(IsInMulti) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CBaseObject *toFind = (CBaseObject *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(IsInMulti) Invalid object in house" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( ( toFind->GetMultiObj() == myItem ) );
	return JS_TRUE;
}

JSBool CItem_IsOnBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnBanList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(IsOnBanList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(IsOnBanList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	
	*rval = BOOLEAN_TO_JSVAL( myItem->IsOnBanList( toFind ) );
	return JS_TRUE;
}
JSBool CItem_IsOnOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnOwnerList) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(IsOnOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(IsOnOwnerList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	
	*rval = BOOLEAN_TO_JSVAL( myItem->IsOwner( toFind ) );
	return JS_TRUE;
}

JSBool CItem_AddToBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToBanList) Invalid Number of Arguments %d, needs: 1", argc );
		return JS_FALSE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(AddToBanList) Invalid object assigned" );
		return JS_FALSE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(AddToBanList) Invalid character" );
		return JS_FALSE;
	}
	
	*rval = JSVAL_TRUE;
	myItem->AddToBanList( toFind );
	return JS_TRUE;
}

JSBool CItem_AddToOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToOwnerList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(AddToOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(AddToOwnerList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	*rval = JSVAL_TRUE;
	myItem->AddAsOwner( toFind );
	return JS_TRUE;
}
JSBool CItem_RemoveFromBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromBanList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(RemoveFromBanList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(RemoveFromBanList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	
	*rval = JSVAL_TRUE;
	myItem->RemoveFromBanList( toFind );
	return JS_TRUE;
}
JSBool CItem_RemoveFromOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromOwnerList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(RemoveFromOwnerList) Invalid object assigned" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(RemoveFromOwnerList) Invalid character" );
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	
	*rval = JSVAL_TRUE;
	myItem->RemoveAsOwner( toFind );
	return JS_TRUE;
}
JSBool CItem_PlaceInPack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(PlaceInPack) Invalid Number of Arguments %d, needs: 0", argc );
		return JS_FALSE;
	}
	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_ITEM ) )
	{
		MethodError( "(PlaceInPack) Invalid object assigned" );
		return JS_FALSE;
	}
	myItem->PlaceInPack();
	return JS_TRUE;
}

JSBool CSocket_OpenURL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 3 parameters
    {
        MethodError( "OpenURL: Invalid Number of Arguments %d, needs: 1" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "OpenURL: Invalid socket!" );
        return JS_FALSE;
    }
	std::string url = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
    mySock->OpenURL( url );
    return JS_TRUE;
}

JSBool CSocket_GetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetByte: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetByte: Invalid socket!");
        return JS_FALSE;
    }
	int offset	= JSVAL_TO_INT( argv[0] );
	*rval		= INT_TO_JSVAL( mySock->GetByte( offset ) );
    return JS_TRUE;
}
JSBool CSocket_GetSByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetSByte: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetSByte: Invalid socket!");
        return JS_FALSE;
    }
	int offset	= JSVAL_TO_INT( argv[0] );
	*rval		= INT_TO_JSVAL( static_cast<SI08>(mySock->GetByte( offset )) );
    return JS_TRUE;
}
JSBool CSocket_GetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetWord: Invalid socket!");
        return JS_FALSE;
    }
	int offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( mySock->GetWord( offset ) );
    return JS_TRUE;
}
JSBool CSocket_GetSWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetSWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetSWord: Invalid socket!");
        return JS_FALSE;
    }
	int offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( static_cast<SI16>(mySock->GetWord( offset )) );
    return JS_TRUE;
}
JSBool CSocket_GetDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetDWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetDWord: Invalid socket!");
        return JS_FALSE;
    }
	int offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( mySock->GetDWord( offset ) );
    return JS_TRUE;
}
JSBool CSocket_GetSDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetSDWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetSDWord: Invalid socket!");
        return JS_FALSE;
    }
	int offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( static_cast<SI32>(mySock->GetDWord( offset )) );
    return JS_TRUE;
}
JSBool CSocket_GetString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "GetString: Invalid number of arguments (takes 1 or 2)" );
		return JS_FALSE;
	}

    CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mSock == NULL )
    {
        MethodError( "GetString: Invalid socket!");
        return JS_FALSE;
    }

	int length		= -1;
	int offset		= JSVAL_TO_INT( argv[0] );
	if( argc == 2 )
		length = JSVAL_TO_INT( argv[1] );

	char toReturn[128];
	if( length != -1 )
	{
		strncpy( toReturn, (char *)&(mSock->Buffer())[offset], length );
		toReturn[length] = 0;
	}
	else
		strcpy( toReturn, (char *)&(mSock->Buffer())[offset] );

	JSString *strSpeech = NULL;
	strSpeech = JS_NewStringCopyZ( cx, toReturn );
	*rval = STRING_TO_JSVAL( strSpeech );

	return JS_TRUE;
}
JSBool CSocket_SetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetByte: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
    CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mSock == NULL )
    {
        MethodError( "SetByte: Invalid socket!");
        return JS_FALSE;
    }
	int offset = JSVAL_TO_INT( argv[0] );
	UI08 byteToSet = (UI08)JSVAL_TO_INT( argv[1] );

	mSock->SetByte( offset, byteToSet );
	return JS_TRUE;
}

JSBool CSocket_SetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

    CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mSock == NULL )
    {
        MethodError( "SetWord: Invalid socket!");
        return JS_FALSE;
    }

	int offset		= JSVAL_TO_INT( argv[0] );
	UI16 byteToSet	= (UI16)JSVAL_TO_INT( argv[1] );

	mSock->SetWord( offset, byteToSet );
	return JS_TRUE;
}

JSBool CSocket_SetDWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetDWord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

    CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mSock == NULL )
    {
        MethodError( "SetDWord: Invalid socket!");
        return JS_FALSE;
    }

	int offset		= JSVAL_TO_INT( argv[0] );
	long byteToSet	= JSVAL_TO_INT( argv[1] );

	mSock->SetDWord( offset, byteToSet );
	return JS_TRUE;
}

JSBool CSocket_SetString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetString: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

    CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mSock == NULL )
    {
        MethodError( "SetString: Invalid socket!");
        return JS_FALSE;
    }

	int offset = JSVAL_TO_INT( argv[0] );
 	char *trgMessage = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( trgMessage == NULL )
	{
		MethodError( "SetString: No string to set" );
		return JS_FALSE;
	}

	strcpy( (char *)&(mSock->Buffer())[offset], trgMessage );
	return JS_TRUE;
}
JSBool CSocket_ReadBytes( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ReadBytes: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

    CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mSock == NULL )
    {
        MethodError( "ReadBytes: Invalid socket!");
        return JS_FALSE;
    }

	int bCount		= JSVAL_TO_INT( argv[0] );
	mSock->Receive( bCount );
	return JS_TRUE;
}
JSBool CSocket_WhoList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );
    if( mySock == NULL )
    {
        MethodError( "WhoList: Invalid socket!");
        return JS_FALSE;
    }

	bool sendOnList = true;
	if( argc == 1 )
		sendOnList = ( JSVAL_TO_BOOLEAN( argv[0] ) == JS_TRUE );

	if( sendOnList )
		WhoList->SendSocket( mySock );
	else
		OffList->SendSocket( mySock );

	return JS_TRUE;
}

JSBool CSocket_Midi( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Midi: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	UI16 midi = (UI16)JSVAL_TO_INT( argv[0] );

	CSocket *mySock = (CSocket*)JS_GetPrivate( cx, obj );

	if( mySock != NULL )
		Effects->playMidi( mySock, midi );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_YellMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	January 30, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Yells a text message to those in range
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CChar_YellMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "YellMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );

	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );

	if( trgMessage == NULL )
	{
		MethodError( "YellMessage: You have to supply a messagetext" );
	}
	
	CChar *myChar = (CChar*)myObj;
	if( !ValidateObject( myChar ) )  
	{
		MethodError( "YellMessage: Invalid Character" );
		return JS_FALSE;
	}

	if( myChar->GetNPCAiType() == AI_EVIL )
		MethodSpeech( *myChar, trgMessage, YELL, 0x0026, (FontType)myChar->GetFontType() );
	else
		MethodSpeech( *myChar, trgMessage, YELL, myChar->GetSayColour(), (FontType)myChar->GetFontType() );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_WhisperMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	January 30, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Whispers a text message to those in range
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CChar_WhisperMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "WhisperMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CBaseObject *myObj = (CBaseObject*)JS_GetPrivate( cx, obj );

	JSString *targMessage = JS_ValueToString( cx, argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );

	if( trgMessage == NULL )
	{
		MethodError( "WhisperMessage: You have to supply a messagetext" );
	}
	
	CChar *myChar = (CChar*)myObj;
	if( !ValidateObject( myChar ) )  
	{
		MethodError( "WhisperMessage: Invalid Character" );
		return JS_FALSE;
	}

	if( myChar->GetNPCAiType() == AI_EVIL )
		MethodSpeech( *myChar, trgMessage, WHISPER, 0x0026, (FontType)myChar->GetFontType() );
	else
		MethodSpeech( *myChar, trgMessage, WHISPER, myChar->GetSayColour(), (FontType)myChar->GetFontType() );

	return JS_TRUE;
}

void BuildGumpFromScripts( CSocket *s, UI16 m );

JSBool CSocket_OpenGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "OpenGump: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ) )
	{
		MethodError( "You have to pass a valid menu number" );
	}

	UI16 menuNumber = (UI16)JSVAL_TO_INT( argv[0] );
	CSocket *mySock = (CSocket *)JS_GetPrivate( cx, obj );

	if( mySock == NULL )
	{
		MethodError( "OpenGump: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	BuildGumpFromScripts( mySock, menuNumber );

	return JS_TRUE;
}

// Race methods


JSBool CRace_CanWearArmour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "CanWearArmour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "CanWearArmour: Unknown Object has been passed" );
		return JS_FALSE;
	}

	CItem *toFind = (CItem *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "CanWearArmour: Invalid item passed" );
		return JS_TRUE;
	}
	
	ARMORCLASS srcClass = myRace->ArmourClassRestriction();
	ARMORCLASS trgClass = toFind->GetArmourClass();
	*rval = BOOLEAN_TO_JSVAL( ( trgClass == 0 ) || ( ( srcClass & trgClass ) != 0 ) );	// they have a matching class
	return JS_TRUE;
}
JSBool CRace_IsValidHairColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidHairColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "IsValidHairColour: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	COLOUR cVal = (COLOUR)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( ( myRace->IsValidHair( cVal ) ) );
	return JS_TRUE;
}
JSBool CRace_IsValidSkinColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidSkinColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "IsValidSkinColour: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	COLOUR cVal = (COLOUR)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( ( myRace->IsValidSkin( cVal ) ) );
	return JS_TRUE;
}
JSBool CRace_IsValidBeardColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidBeardColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "IsValidBeardColour: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	COLOUR cVal = (COLOUR)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( ( myRace->IsValidBeard( cVal ) ) );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_ApplySection( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Apply's a DFN section to an Item/Character
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	

bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply );

JSBool CBase_ApplySection( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ApplySection: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CBaseObject *myObj		= (CBaseObject*)myClass.toObject();
	std::string trgSection	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );

	if( trgSection.empty() || trgSection.length() == 0 )
	{
		MethodError( "You have to supply a section to apply" );
		return JS_FALSE;
	}
	
	if( myClass.ClassName() == "UOXItem" )
	{
		CItem *myItem = (CItem*)myObj;
		if( !ValidateObject( myItem ) )  
		{
			MethodError( "ApplySection: Invalid Item" );
			return JS_FALSE;
		}
		ScriptSection *toFind = FileLookup->FindEntry( trgSection, items_def );
		ApplyItemSection( myItem, toFind );
	}
	else if( myClass.ClassName() == "UOXChar" ) 
	{
		CChar *myChar = (CChar*)myObj;
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "ApplySection: Invalid Character" );
			return JS_FALSE;
		}

		ScriptSection *toFind = FileLookup->FindEntry( trgSection, npc_def );
		Npcs->ApplyNpcSection( myChar, toFind );
	}	

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a spell to the character's spell book
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CChar_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddSpell: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CChar *myChar	= (CChar *)JS_GetPrivate( cx, obj );
	UI08 spellNum	= (UI08)JSVAL_TO_INT( argv[0] );
	CItem *sBook	= FindItemOfType( myChar, IT_SPELLBOOK );
	if( ValidateObject( sBook ) )
		Magic->AddSpell( sBook, spellNum );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Does the actions associated with spell failure
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CChar_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "SpellFail: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );

	Effects->PlayStaticAnimation( myChar, 0x3735, 0, 30 );
	Effects->PlaySound( myChar, 0x005C );
	if( !myChar->IsNpc() )
	{
		CSocket *mSock = myChar->GetSocket();
		if( mSock != NULL )
			myChar->TextMessage( mSock, 771, EMOTE, false );
	}
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CItem_Refresh( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Causes the item to be refreshed to sockets
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CItem_Refresh( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Refresh: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	myItem->Dirty( UT_UPDATE );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CItem_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Applies the specified rank to the item
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CItem_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ApplyRank: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CItem *myItem	= (CItem *)JS_GetPrivate( cx, obj );
	int rank		= JSVAL_TO_INT( argv[0] );

	Skills->ApplyRank( NULL, myItem, rank );
	return JS_TRUE;
}

JSBool CAccount_GetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

JSBool CAccount_SetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool CAccount_AddAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	8/15/2003 10:43:18 PM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / MyndTrip Studios / EAWare
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Add an account through the JS. The calling script will
//|							be required to specifiy the username, password, flags, 
//|							and comment/contact information.
//o--------------------------------------------------------------------------o
//| Modifications	-	Removed UOXAccountWrapper and exposed global var Accounts
//o--------------------------------------------------------------------------o
JSBool CAccount_AddAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	if( argc != 4 )
	{
		MethodError( "Account.AddAccount(user,pass,email,flags): Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	// Ok get our object from the global context
	if( !JSVAL_IS_STRING(argv[0]) || !JSVAL_IS_STRING(argv[1]) || !JSVAL_IS_STRING(argv[2]) || !( JSVAL_IS_INT( argv[3] ) || JSVAL_IS_STRING( argv[3] ) ) )
	{
		MethodError( "Account.AddAccount(user,pass,email,flags): Invalid parameter specifled, please check param types." );
		return JS_FALSE;
	}
	std::string lpszUsername	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	std::string lpszPassword	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	std::string lpszComment		= JS_GetStringBytes( JS_ValueToString( cx, argv[2] ) );
	UI16 u16Flags		= 0;
	
	if( JSVAL_IS_INT( argv[3] ) )
		u16Flags = (UI16)JSVAL_TO_INT( argv[3] );
	else
		u16Flags = UString( JS_GetStringBytes( JS_ValueToString( cx, argv[3] ) ) ).toUShort();

	if( lpszUsername.empty() || lpszPassword.empty() || lpszComment.empty() || lpszUsername.length() == 0 || lpszPassword.length() == 0 || lpszComment.length() == 0 )
		return JS_FALSE;

	
	if( !Accounts->AddAccount( lpszUsername, lpszPassword, lpszComment, u16Flags ) )
	{
		MethodError( "Account.AddAccount( user, pass, email, flags ): Account was not added. Duplicate expected." );
		return JS_FALSE;
	}
	return JS_TRUE;
}
//o--------------------------------------------------------------------------o
//|	Function		-	JSBool CAccount_DelAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	8/21/2003 2:39:27 PM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / Stiffed-Attic Software / EAWare
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Remove an account from the memory. This is a perminent 
//|							action that CAN NOT be undone. After a call to this 
//|							function the account will be immidiatly removed from
//|							memory, and will not be written out to the accounts.adm
//|							file, and will have the associated directory tagged and
//|							renamed. 
//o--------------------------------------------------------------------------o
//| Modifications	-	Removed UOXAccountWrapper and exposed global var Accounts
//o--------------------------------------------------------------------------o
JSBool CAccount_DelAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	if( argc != 1 )
	{
		MethodError( "Account.DelAccount([username/id]): Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	char	*lpszUsername = NULL;
	UI16	ui16AccountID;
	// Ok get out object from the global context
	if( JSVAL_IS_STRING(argv[0]) )
	{
		lpszUsername = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
		if( !Accounts->DelAccount( lpszUsername ) )
		{
			MethodError(" Account.DelAccount(username): Unable to remove account specified.");
			return JS_FALSE;
		}

	}	
	else if( JSVAL_IS_INT( argv[0] ) )
	{
		ui16AccountID = (UI16)JSVAL_TO_INT(argv[0]);
		if( !Accounts->DelAccount( ui16AccountID ) )
		{
			MethodError(" Account.DelAccount(accountID): Unable to remove account specified.");
			return JS_FALSE;
		}
	}
	else
	{
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool CAccount_ModAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

JSBool CAccount_SaveAccounts( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

// Basic file wrapping structure for abstracting away file IO for the JS file funcs
// UOXFile constructor !
JSBool UOXCFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UOXFileWrapper *toAdd = new UOXFileWrapper;
	toAdd->mWrap = NULL;

	JSBool myVal = JS_DefineFunctions( cx, obj, CFile_Methods );
	JS_SetPrivate( cx, obj, toAdd );
	JS_LockGCThing( cx, obj );
	//JS_AddRoot( cx, &obj );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Frees memory allocated by file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Free: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );
	delete mFile;
	JS_UnlockGCThing( cx, obj );
	//JS_RemoveRoot( cx, &obj );
	JS_SetPrivate( cx, obj, NULL );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Open( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Opens a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Open( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "Open: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	char *filename	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	UString mode	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	if( mode.lower().find_first_of("rwa",0,1) == std::string::npos )
	{
		MethodError( "Open: Invalid mode must be \"read\", \"write\", or \"append\"!" );
		return JS_FALSE;
	}
	if( strstr( filename, ".." ) || strstr( filename, "\\" ) || strstr( filename, "/" ) )
	{
		MethodError( "Open: file names may not contain \"..\", \"\\\", or \"/\"." );
		return JS_FALSE;
	}
	
	mFile->mWrap = fopen( filename, mode.lower().substr(0,1).c_str() );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Closes a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Open: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	fclose( mFile->mWrap );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Reads from a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Read: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ) )
	{
		MethodError( "Read: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	int bytes = JSVAL_TO_INT( argv[0] );
	char data[512];

	if( bytes > 512 || bytes < 1 )
	{
		MethodError( "Read: Invalid byte count, must be from 1 to 512!" );
		return JS_FALSE;
	}

	fread( data, 1, bytes, mFile->mWrap );

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, data ) );
	return JS_TRUE;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Reads from a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ReadUntil: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ) )
	{
		MethodError( "ReadUntil: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	char *until = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	char line[512];
	int c;

	if( until[0] == '\\' && strlen( until ) > 1 )
	{
		switch( until[1] )
		{
			case '\\': until[0] = '\\'; break;
			case '\'': until[0] = '\''; break;
			case '\"': until[0] = '\"'; break;
			case 'n':  until[0] = '\n'; break;
			default: MethodError( "ReadUntil: Unsupported character escape sequence %s", until );	break;
		}
	}

	for( c = 0; c < 512 && !feof( mFile->mWrap ); ++c )
	{
		line[c] = fgetc( mFile->mWrap );
		if( line[c] == until[0] || line[c] == '\n' )
			break;
	}
	line[c<512 ? c : 511] = 0;

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, line ) );
	return JS_TRUE;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Writes to a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Write: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ) )
	{
		MethodError( "Write: Error writing to file, file was not opened sucessfully!" );
		return JS_FALSE;
	} 
	else if( ftell( mFile->mWrap ) > 1049600 ) 
	{
		MethodError( "Write: Error writing to file.  File my not exceed 1mb." );
		return JS_FALSE;
	}

	char *str = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( str != NULL )
		fprintf( mFile->mWrap, str );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_EOF( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	3/05/2006
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns if we have read to the end of a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_EOF( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "EOF: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap )
		return JS_FALSE;

	*rval = BOOLEAN_TO_JSVAL( ( feof( mFile->mWrap ) != 0 ) );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Length( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	3/05/2006
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the length of the file
//o--------------------------------------------------------------------------o
JSBool CFile_Length( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Length: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap )
		return JS_FALSE;

	long fpos = ftell( mFile->mWrap );
	fseek( mFile->mWrap, 0, SEEK_END );
	*rval = INT_TO_JSVAL( ftell( mFile->mWrap ) );
	fseek( mFile->mWrap, fpos, SEEK_SET );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Pos( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	3/05/2006
//|	Developer(s)	-	giwo
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns or sets the position we are at in the file
//o--------------------------------------------------------------------------o
JSBool CFile_Pos( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 || argc != 1 )
	{
		MethodError( "Pos: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap )
		return JS_FALSE;

	if( argc == 1 )
		fseek( mFile->mWrap, JSVAL_TO_INT( argv[0] ), SEEK_SET );

	*rval = INT_TO_JSVAL( ftell( mFile->mWrap ) );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_FirstItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	02 Aug 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns first object in the object's list
//o--------------------------------------------------------------------------o
//|	Returns			-	True if script can execute, false otherwise
//o--------------------------------------------------------------------------o	
JSBool CBase_FirstItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "FirstItem: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myObj ) )
	{
		MethodError( "FirstItem: Invalid object assigned." );
		return JS_FALSE;
	}
	CItem *firstItem = NULL;
	if( myObj->GetObjType() == OT_CHAR )
		firstItem = ( static_cast< CChar * >(myObj) )->FirstItem();
	else
		firstItem = ( static_cast< CItem * >(myObj) )->GetContainsList()->First();
	if( ValidateObject( firstItem ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, firstItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_NextItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	02 Aug 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns next object in the object's list
//o--------------------------------------------------------------------------o
//|	Returns			-	True if script can execute, false otherwise
//o--------------------------------------------------------------------------o	
JSBool CBase_NextItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NextItem: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myObj ) )
	{
		MethodError( "NextItem: Invalid object assigned." );
		return JS_FALSE;
	}
	CItem *firstItem = NULL;
	if( myObj->GetObjType() == OT_CHAR )
		firstItem = ( static_cast< CChar * >(myObj) )->NextItem();
	else
		firstItem = ( static_cast< CItem * >(myObj) )->GetContainsList()->Next();
	if( ValidateObject( firstItem ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, firstItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_FinishedItems( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	02 Aug 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns true if finished all items in object's list
//o--------------------------------------------------------------------------o
//|	Returns			-	True if script can execute, false otherwise
//o--------------------------------------------------------------------------o	
JSBool CBase_FinishedItems( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "FinishedItems: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	CBaseObject *myObj = static_cast<CBaseObject*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myObj ) )
	{
		MethodError( "NextItem: Invalid object assigned." );
		return JS_FALSE;
	}
	if( myObj->GetObjType() == OT_CHAR )
		*rval = BOOLEAN_TO_JSVAL( ( static_cast< CChar * >(myObj) )->FinishedItems() );
	else
		*rval = BOOLEAN_TO_JSVAL( ( static_cast< CItem * >(myObj) )->GetContainsList()->Finished() );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_WalkTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	06 Sep 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Begins pathfinding for a character
//o--------------------------------------------------------------------------o
//|	Returns			-	Nothing
//o--------------------------------------------------------------------------o	
JSBool CChar_WalkTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		MethodError( "WalkTo: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( cMove ) )
	{
		MethodError( "WalkTo: Invalid source character" );
		return JS_FALSE;
	}
	SI16 gx			= 0;
	SI16 gy			= 0;
	UI08 maxSteps	= 0;
	switch( argc )
	{
		case 2:
			if( JSVAL_IS_OBJECT( argv[0] ) )
			{	// we can work with this, it should be either a character or item, hopefully
				JSEncapsulate jsToGoTo( cx, &(argv[0]) );
				if( jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar" )
				{
					CBaseObject *toGoTo = (CBaseObject *)jsToGoTo.toObject();
					if( !ValidateObject( toGoTo ) )
					{
						MethodError( "No object associated with this object" );
						break;
					}
					gx		= toGoTo->GetX();
					gy		= toGoTo->GetY();
				}
				else if( jsToGoTo.ClassName() == "UOXSocket" )
				{
					CSocket *mySock		= (CSocket *)jsToGoTo.toObject();
					CChar *mySockChar	= mySock->CurrcharObj();
					gx					= mySockChar->GetX();
					gy					= mySockChar->GetY();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
				maxSteps = (UI08)JSVAL_TO_INT( argv[1] );
				break;
			}
			MethodError( "Text-styled Parameters may be added later" );
			break;

		// 2 Parameters, x + y
		case 3: 
			gx			= (SI16)JSVAL_TO_INT( argv[0] );
			gy			= (SI16)JSVAL_TO_INT( argv[1] );
			maxSteps	= (UI08)JSVAL_TO_INT( argv[2] );
			break;
		default:
			MethodError( "Invalid number of arguments passed to WalkTo, needs either 2 or 3" );
			break;
	}

	cMove->FlushPath();
#if defined( UOX_DEBUG_MODE )
	Console.Print( "WalkTo: Moving character 0x%X to (%i,%i) with a maximum of %i steps\n", cMove->GetSerial(), gx, gy, maxSteps );
#endif
	cMove->SetOldNpcWander( cMove->GetNpcWander() );
	cMove->SetNpcWander( WT_PATHFIND );
	if( cwmWorldState->ServerData()->AdvancedPathfinding() )
		Movement->AdvancedPathfinding( cMove, gx, gy, false );
	else
		Movement->PathFind( cMove, gx, gy, false, maxSteps );
	return JS_TRUE;
}

JSBool CChar_RunTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		MethodError( "RunTo: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( cMove ) )
	{
		MethodError( "RunTo: Invalid source character" );
		return JS_FALSE;
	}
	UI16 gx			= 0;
	UI16 gy			= 0;
	UI08 maxSteps	= 0;
	switch( argc )
	{
		case 2:
			if( JSVAL_IS_OBJECT( argv[0] ) )
			{	// we can work with this, it should be either a character or item, hopefully
				JSEncapsulate jsToGoTo( cx, &(argv[0]) );
				if( jsToGoTo.ClassName() == "UOXItem" || jsToGoTo.ClassName() == "UOXChar" )
				{
					CBaseObject *toGoTo = (CBaseObject *)jsToGoTo.toObject();
					if( !ValidateObject( toGoTo ) )
					{
						MethodError( "No object associated with this object" );
						break;
					}
					gx		= toGoTo->GetX();
					gy		= toGoTo->GetY();
				}
				else if( jsToGoTo.ClassName() == "UOXSocket" )
				{
					CSocket *mySock		= (CSocket *)jsToGoTo.toObject();
					CChar *mySockChar	= mySock->CurrcharObj();
					gx					= mySockChar->GetX();
					gy					= mySockChar->GetY();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
				maxSteps = (UI08)JSVAL_TO_INT( argv[1] );
				break;
			}
			MethodError( "Text-styled Parameters may be added later" );
			break;

		// 2 Parameters, x + y
		case 3: 
			gx			= (SI16)JSVAL_TO_INT( argv[0] );
			gy			= (SI16)JSVAL_TO_INT( argv[1] );
			maxSteps	= (UI08)JSVAL_TO_INT( argv[2] );
			break;
		default:
			MethodError( "Invalid number of arguments passed to RunTo, needs either 2 or 3" );
			break;
	}

	cMove->FlushPath();
#if defined( UOX_DEBUG_MODE )
	Console.Print( "RunTo: Moving character %i to (%i,%i) with a maximum of %i steps", cMove->GetSerial(), gx, gy, maxSteps );
#endif
	cMove->SetOldNpcWander( cMove->GetNpcWander() );
	cMove->SetNpcWander( WT_PATHFIND );

	if( cwmWorldState->ServerData()->AdvancedPathfinding() )
		Movement->AdvancedPathfinding( cMove, gx, gy, true );
	else
		Movement->PathFind( cMove, gx, gy, true, maxSteps );
	return JS_TRUE;
}

JSBool CMisc_GetTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "GetTimer: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &(argv[0]) );
	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *cMove = static_cast<CChar*>(myClass.toObject());
		if( !ValidateObject( cMove ) )
		{
			MethodError( "GetTimer: Invalid source character" );
			return JS_FALSE;
		}

		*rval = INT_TO_JSVAL( cMove->GetTimer( (cC_TID)encaps.toInt() ) );
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mSock = static_cast<CSocket *>(myClass.toObject());
		if( mSock == NULL )
		{
			MethodError( "GetTimer: Invalid source socket" );
			return JS_FALSE;
		}

		*rval = INT_TO_JSVAL( mSock->GetTimer( (cS_TID)encaps.toInt() ) );
	}

	return JS_TRUE;
}

JSBool CMisc_SetTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetTimer: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &(argv[0]) );
	JSEncapsulate encaps2( cx, &(argv[1]) );
	JSEncapsulate myClass( cx, obj );

	R32 timerVal = encaps2.toFloat();
	if( timerVal != 0 )
		timerVal = BuildTimeValue( timerVal / 1000.0f );
	if( myClass.ClassName() == "UOXChar" )
	{
		CChar *cMove = static_cast<CChar*>(myClass.toObject());
		if( !ValidateObject( cMove ) )
		{
			MethodError( "SetTimer: Invalid source character" );
			return JS_FALSE;
		}

		cMove->SetTimer( (cC_TID)encaps.toInt(), static_cast<TIMERVAL>(timerVal) );
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		CSocket *mSock = static_cast<CSocket *>(myClass.toObject());
		if( mSock == NULL )
		{
			MethodError( "SetTimer: Invalid source socket" );
			return JS_FALSE;
		}

		mSock->SetTimer( (cS_TID)encaps.toInt(), static_cast<TIMERVAL>(timerVal) );
	}

	return JS_TRUE;
}

JSBool CBase_DistanceTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "DistanceTo: Invalid number of arguments (takes 1, game object)" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *myObj	= (CBaseObject *)JS_GetPrivate( cx, jsObj );

	CBaseObject *thisObj	= static_cast<CBaseObject *>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( thisObj ) || !ValidateObject( myObj ) )
	{
		MethodError( "DistanceTo: Invalid character" );
		return JS_FALSE;
	}
	
	*rval = INT_TO_JSVAL( getDist( thisObj, myObj ) );
	return JS_TRUE;
}

JSBool CItem_Glow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSObject *mSock		= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock	= (CSocket *)JS_GetPrivate( cx, mSock );

	CItem *mItem	= static_cast<CItem *>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( mItem ) )
	{
		MethodError( "Glow: Invalid item" );
		mySock->sysmessage( 1095 ); 
		return JS_FALSE;
	}

	CChar *mChar = mySock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return JS_FALSE;

	if( mItem->GetGlow() != INVALIDSERIAL ) 
	{
		mySock->sysmessage( 1097 );
		return JS_FALSE;
	}
	if( mItem->GetCont() == NULL && FindItemOwner( mItem ) != mChar )
	{
		mySock->sysmessage( 1096 );
		return JS_FALSE;
	}

	mItem->SetGlowColour( mItem->GetColour() );

	CItem *glowItem = Items->CreateItem( mySock, mChar, 0x1647, 1, 0, OT_ITEM ); // spawn light emitting object
	if( glowItem == NULL )
		return JS_FALSE;
	glowItem->SetDecayable( mItem->isDecayable() );
	glowItem->SetDecayTime( mItem->GetDecayTime() );
	glowItem->SetName( "glower" );
	glowItem->SetMovable( 2 );

	mItem->SetGlow( glowItem->GetSerial() );
	Items->GlowItem( mItem );

	mChar->Update( mySock );
	mySock->sysmessage( 1098 );

	return JS_TRUE;
}

JSBool CItem_UnGlow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSObject *mSock		= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock	= (CSocket *)JS_GetPrivate( cx, mSock );

	CItem *mItem	= static_cast<CItem *>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( mItem ) )
	{
		MethodError( "UnGlow: Invalid item" );
		mySock->sysmessage( 1099 ); 
		return JS_FALSE;
	}

	CChar *mChar = mySock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return JS_FALSE;

	if( mItem->GetCont() == NULL && FindItemOwner( mItem ) != mChar )
	{
		mySock->sysmessage( 1100 );
		return JS_FALSE;
	}

	CItem *glowItem = calcItemObjFromSer( mItem->GetGlow() );
	if( mItem->GetGlow() == INVALIDSERIAL || !ValidateObject( glowItem ) ) 
	{
		mySock->sysmessage( 1101 );
		return JS_FALSE;
	}

	mItem->SetColour( mItem->GetGlowColour() );

	glowItem->Delete();
	mItem->SetGlow( INVALIDSERIAL );

	mChar->Update( mySock );
	mySock->sysmessage( 1102 );

	return JS_TRUE;
}

JSBool CChar_Gate( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		MethodError( "Gate: Invalid number of arguments (takes 1; item/place, or 4; x y z world)" );
		return JS_FALSE;
	}

	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Gate: Invalid source character" );
		return JS_FALSE;
	}

	SI16 destX = -1, destY = -1;
	SI08 destZ = -1;
	UI08 destWorld = 0;

	if( argc == 1 )
	{
		if( JSVAL_IS_OBJECT( argv[0] ) )
		{
			JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
			CItem *mItem		= (CItem *)JS_GetPrivate( cx, jsObj );
			if( !ValidateObject( mItem ) )
			{
				MethodError( "Gate: Invalid item passed" );
				return JS_FALSE;
			}

			destX		= mItem->GetTempVar( CITV_MOREX );
			destY		= mItem->GetTempVar( CITV_MOREY );
			destZ		= mItem->GetTempVar( CITV_MOREZ );
			destWorld	= mItem->GetTempVar( CITV_MORE );
		}
		else
		{
			size_t placeNum = JSVAL_TO_INT( argv[0] );
			if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
			{
				GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
				destX		= toGoTo.x;
				destY		= toGoTo.y;
				destZ		= toGoTo.z;
				destWorld	= toGoTo.worldNum;
			}
		}
	}
	else
	{
		destX		= JSVAL_TO_INT( argv[0] );
		destY		= JSVAL_TO_INT( argv[1] );
		destZ		= JSVAL_TO_INT( argv[2] );
		destWorld	= JSVAL_TO_INT( argv[3] );
	}

	if( !Map->MapExists( destWorld ) )
		destWorld = mChar->WorldNumber();

	SpawnGate( mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber(), destX, destY, destZ, destWorld );

	return JS_TRUE;
}

JSBool CChar_Recall( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Recall: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}

	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Recall: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= (CItem *)JS_GetPrivate( cx, jsObj );
	if( !ValidateObject( mItem ) )
	{
		MethodError( "Recall: Invalid item passed" );
		return JS_FALSE;
	}

	SI16 destX = mItem->GetTempVar( CITV_MOREX ), destY = mItem->GetTempVar( CITV_MOREY );
	SI08 destZ = mItem->GetTempVar( CITV_MOREZ );
	UI08 destWorld = mItem->GetTempVar( CITV_MORE );

	if( !Map->MapExists( destWorld ) )
		destWorld = mChar->WorldNumber();

	mChar->SetLocation( destX, destY, destZ, destWorld );

	return JS_TRUE;
}

JSBool CChar_Mark( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Mark: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}

	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Mark: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= (CItem *)JS_GetPrivate( cx, jsObj );
	if( !ValidateObject( mItem ) )
	{
		MethodError( "Mark: Invalid item passed" );
		return JS_FALSE;
	}

	mItem->SetTempVar( CITV_MOREX, mChar->GetX() );
	mItem->SetTempVar( CITV_MOREY, mChar->GetY() );
	mItem->SetTempVar( CITV_MOREZ, mChar->GetZ() );
	mItem->SetTempVar( CITV_MORE, mChar->WorldNumber() );

	char tempname[512];
	if( mChar->GetRegion()->GetName()[0] != 0 )
		sprintf( tempname, Dictionary->GetEntry( 684 ).c_str(), mChar->GetRegion()->GetName().c_str() );
	else 
		strcpy( tempname, Dictionary->GetEntry( 685 ).c_str() );
	mItem->SetName( tempname );

	return JS_TRUE;
}

JSBool CChar_SetSkillByName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetSkillByName: Invalid number of arguments (takes 2, string, value)" );
		return JS_FALSE;
	}

	CChar *mChar			= (CChar *)JS_GetPrivate( cx, obj );
	std::string skillName	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	UI16 value				= JSVAL_TO_INT( argv[1] );
	CSocket *mSock			= NULL;
	if( !mChar->IsNpc() )
		mSock = mChar->GetSocket();
	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		if( skillName == cwmWorldState->skill[i].name )
		{
			mChar->SetBaseSkill( value, i );
			Skills->updateSkillLevel( mChar, i );

			if( mSock != NULL ) 
				mSock->updateskill( i );
			*rval = JSVAL_TRUE;
			return JS_TRUE;
		}
	}
	*rval = JSVAL_FALSE;
	return JS_TRUE;
}

JSBool CChar_Kill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Kill: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	CChar *mChar = (CChar *)JS_GetPrivate( cx, obj );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Kill: Invalid character passed" );
		return JS_FALSE;
	}
	HandleDeath( mChar );
	return JS_TRUE;
}

JSBool CChar_Resurrect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Resurrect: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	CChar *mChar = (CChar *)JS_GetPrivate( cx, obj );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Resurrect: Invalid character passed" );
		return JS_FALSE;
	}
	NpcResurrectTarget( mChar );
	return JS_TRUE;
}

JSBool CItem_Dupe( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Dupe: Invalid number of arguments (takes 1, string, value)" );
		return JS_FALSE;
	}

	CItem *mItem			= (CItem *)JS_GetPrivate( cx, obj );
	JSObject *jsObj			= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mSock			= (CSocket *)JS_GetPrivate( cx, jsObj );
	if( !ValidateObject( mItem ) || mSock == NULL )
	{
		MethodError( "Dupe: Bad parameters passed" );
		return JS_FALSE;
	}
	Items->DupeItem( mSock, mItem, mItem->GetAmount() );
	return JS_TRUE;
}

JSBool CChar_Jail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Jail: Invalid number of arguments (takes 0 or 1, seconds to Jail)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "Jail: Invalid character" );
		return JS_FALSE;
	}

	SI32 numSecsToJail = 100000;
	if( argc == 1 )
		numSecsToJail = (SI32)JSVAL_TO_INT( argv[0] );

	JailSys->JailPlayer( myChar, numSecsToJail );
	return JS_TRUE;
}

JSBool CChar_Release( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Release: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "Jail: Invalid character" );
		return JS_FALSE;
	}

	JailSys->ReleasePlayer( myChar );
	return JS_TRUE;
}

JSBool CConsole_Print( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Print: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	Console.Print( arg0.toString().c_str() );
	return JS_TRUE;
}

JSBool CConsole_Log( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "Log: Invalid number of arguments (takes 1 or 2)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	JSEncapsulate arg1;
	if( argc == 1 )
	{
		Console.Log( arg0.toString().c_str() );
	}
	else
	{
		arg1.SetContext( cx, &(argv[1]) );
		Console.Log( arg0.toString().c_str(), arg1.toString().c_str() );
	}
	return JS_TRUE;
}

JSBool CConsole_Error( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Error: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	Console.Error( arg0.toString().c_str() );
	return JS_TRUE;
}

JSBool CConsole_Warning( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Warning: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	Console.Warning( arg0.toString().c_str() );
	return JS_TRUE;
}

JSBool CConsole_PrintSectionBegin( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "PrintSectionBegin: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.PrintSectionBegin();
	return JS_TRUE;
}

JSBool CConsole_TurnYellow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnYellow: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnYellow();
	return JS_TRUE;
}

JSBool CConsole_TurnRed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnRed: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnRed();
	return JS_TRUE;
}

JSBool CConsole_TurnGreen( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnGreen: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnGreen();
	return JS_TRUE;
}

JSBool CConsole_TurnBlue( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnBlue: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnBlue();
	return JS_TRUE;
}

JSBool CConsole_TurnNormal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnNormal: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnNormal();
	return JS_TRUE;
}

JSBool CConsole_TurnBrightWhite( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "TurnBrightWhite: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.TurnBrightWhite();
	return JS_TRUE;
}

JSBool CConsole_PrintDone( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 && argc != 1 )
	{
		MethodError( "PrintDone: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	bool normalDone = true;
	if( argc != 0 )
	{
		JSEncapsulate encaps( cx, &(argv[0]) );
		normalDone	= encaps.toBool();
	}
	if( normalDone )
		Console.PrintDone();
	else
		messageLoop << MSG_PRINTDONE;
	return JS_TRUE;
}

JSBool CConsole_PrintFailed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 && argc != 1 )
	{
		MethodError( "PrintFailed: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	bool normalFailed = true;
	if( argc != 0 )
	{
		JSEncapsulate encaps( cx, &(argv[0]) );
		normalFailed	= encaps.toBool();
	}
	if( normalFailed )
		Console.PrintFailed();
	else
		messageLoop << MSG_PRINTFAILED;
	return JS_TRUE;
}

JSBool CConsole_PrintPassed( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "PrintPassed: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.PrintPassed();
	return JS_TRUE;
}

JSBool CConsole_ClearScreen( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "ClearScreen: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	Console.ClearScreen();
	return JS_TRUE;
}

JSBool CConsole_PrintBasedOnVal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "PrintBasedOnVal: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	Console.PrintBasedOnVal( arg0.toBool() );
	return JS_TRUE;
}

JSBool CConsole_MoveTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "MoveTo: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	JSEncapsulate arg1( cx, &(argv[1]) );
	Console.MoveTo( arg0.toInt(), arg1.toInt() );
	return JS_TRUE;
}

JSBool CConsole_PrintSpecial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "PrintSpecial: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	JSEncapsulate arg1( cx, &(argv[1]) );
	Console.PrintSpecial( arg0.toInt(), arg1.toString().c_str() );
	return JS_TRUE;
}

JSBool CConsole_BeginShutdown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "BeginShutdown: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	messageLoop << MSG_SHUTDOWN;
	return JS_TRUE;
}
JSBool CConsole_Reload( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Reload: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate arg0( cx, &(argv[0]) );
	int mArg = arg0.toInt();
	if( mArg < 0 || mArg > 8 )
	{
		MethodError( "Reload: Section to reload must be between 0 and 8" );
		return JS_FALSE;
	}
	messageLoop.NewMessage( MSG_RELOAD, UString::number( mArg ).c_str() );
	return JS_TRUE;
}

JSBool CChar_SpellMoveEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SpellMoveEffect: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	SpellInfo *mySpell	= (SpellInfo *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[1] ) );
	if( mySpell == NULL )
	{
		MethodError( "SpellMoveEffect: Invalid spell" );
		return JS_FALSE;
	}

	CChar *source = static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	CBaseObject *target = (CBaseObject *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( source ) || !ValidateObject( target ) )
	{
		MethodError( "SpellMoveEffect: Invalid object passed" );
		return JS_FALSE;
	}

	CMagicMove temp = mySpell->MoveEffect();
	
	if( temp.Effect() != INVALIDID )
		Effects->PlayMovingAnimation( source, target, temp.Effect(), temp.Speed(), temp.Loop(), ( temp.Explode() == 1 ) );

	return JS_TRUE;
}

JSBool CChar_SpellStaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SpellStaticEffect: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CChar *source = (CChar *)JS_GetPrivate( cx, obj );
	if( !ValidateObject( source ) )
	{
		MethodError( "SpellStaticEffect: Invalid character passed" );
		return JS_FALSE;
	}

	SpellInfo *mySpell = static_cast<SpellInfo *>(JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) ) );
	if( mySpell == NULL )
	{
		MethodError( "SpellStaticEffect: Invalid spell" );
		return JS_FALSE;
	}

	CMagicStat temp = mySpell->StaticEffect();
	
	if( temp.Effect() != INVALIDID )
		Effects->PlayStaticAnimation( source, temp.Effect(), temp.Speed(), temp.Loop() );

	return JS_TRUE;
}

JSBool CChar_BreakConcentration( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "BreakConcentration: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}

	CChar *mChar = static_cast<CChar *>(JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ) )
	{
		MethodError( "BreakConcentration: Invalid character" );
		return JS_FALSE;
	}

	CSocket *mSock = NULL;
	if( argc == 1 )
	{
		mSock = static_cast<CSocket *>(JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) ) );
		if( mSock == NULL )
		{
			MethodError( "BreakConcentration: Invalid socket" );
			return JS_FALSE;
		}
	}

	mChar->BreakConcentration( mSock );

	return JS_TRUE;
}

JSBool CSocket_SendAddMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SendAddMenu: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CSocket *mSock = (CSocket*)JS_GetPrivate( cx, obj );
	if( mSock == NULL )
	{
		MethodError( "SendAddMenu: Invalid socket" );
		return JS_FALSE;
	}

	UI16 menuNum = (UI16)JSVAL_TO_INT( argv[0] );

	BuildAddMenuGump( mSock, menuNum );

	return JS_TRUE;
}

JSBool CItem_LockDown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "LockDown: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CItem *mItem = (CItem *)JS_GetPrivate( cx, obj );
	if( mItem == NULL )
	{
		MethodError( "LockDown: Invalid item" );
		return JS_FALSE;
	}

	mItem->LockDown();
	return JS_TRUE;
}

JSBool CChar_InitWanderArea( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *mChar = static_cast<CChar *>(JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar ) || !mChar->IsNpc() )
	{
		MethodError( "InitWanderArea: Invalid character" );
		return JS_FALSE;
	}
	InitializeWanderArea( mChar, 10, 10 );
	return JS_TRUE;
}

void newCarveTarget( CSocket *s, CItem *i );
JSBool CItem_Carve( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Carve: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	CItem *toCarve = static_cast<CItem *>(JS_GetPrivate( cx, obj ));
	if( !ValidateObject( toCarve ) )
	{
		MethodError( "Carve: Invalid item" );
		return JS_FALSE;
	}

	CSocket *mSock = static_cast<CSocket *>(JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) ) );
	if( mSock == NULL )
	{
		MethodError( "Carve: Invalid socket" );
		return JS_FALSE;
	}

	newCarveTarget( mSock, toCarve );

	return JS_TRUE;
}

JSBool CBase_CanSee( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 3 )
	{
		MethodError( "CanSee: Invalid number of arguments (takes 1, a char/item or 3, an x/y/z)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CSocket *mSock		= NULL;
	CChar *mChar		= NULL;

	// Let's validate the person seeing (socket/char)

	if( myClass.ClassName() == "UOXSocket" )
	{
		mSock	= (CSocket *)myClass.toObject();
		if( mSock == NULL ) 
		{
			MethodError( "CanSee: Passed an invalid Socket" );
			return JS_FALSE;
		}
		mChar	= mSock->CurrcharObj();
		if( !ValidateObject( mChar ) )  
		{
			MethodError( "CanSee: Socket to look from has an invalid Character attached" );
			return JS_FALSE;
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		mChar	= (CChar *)myClass.toObject();
		if( !ValidateObject( mChar ) )  
		{
			MethodError( "CanSee: Passed an invalid Character" );
			return JS_FALSE;
		}
		mSock	= mChar->GetSocket();
	}

	// If we've reached this far, the socket/char is valid
	// Now let's validate the input!

	SI16 x = -1, y = -1;
	SI08 z = 0;
	if( argc == 1 )	// we've been passed an item, character, or socket
	{
		JSEncapsulate myClass( cx, &(argv[0]) );

		if( myClass.ClassName() == "UOXSocket" )
		{
			CSocket *tSock = (CSocket *)myClass.toObject();
			if( tSock == NULL )
			{
				MethodError( "CanSee: Passed an invalid Socket to look at" );
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			CChar *tChar = tSock->CurrcharObj();
			if( !ValidateObject( tChar ) )  
			{
				MethodError( "CanSee: Socket to look at has invalid Character attached" );
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			if( tChar->WorldNumber() != mChar->WorldNumber() )
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			x = tChar->GetX();
			y = tChar->GetY();
			z = tChar->GetZ();
		}
		else if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" )
		{
			CBaseObject *tObj	= (CBaseObject *)myClass.toObject();
			if( !ValidateObject( tObj ) )  
			{
				MethodError( "CanSee: Object to look at is invalid" );
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			if( tObj->WorldNumber() != mChar->WorldNumber() )
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			if( tObj->CanBeObjType( OT_ITEM ) )
			{
				if( ((CItem *)tObj)->GetCont() != NULL )
				{
					MethodError( "CanSee: Object to look at cannot be in a container" );
					*rval = JSVAL_FALSE;
					return JS_TRUE;
				}
			}
			x = tObj->GetX();
			y = tObj->GetY();
			z = tObj->GetZ();
		}
	}
	else	// we've been passed an x, y and z
	{
		x	= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
		y	= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
		z	= static_cast<SI08>(JSVAL_TO_INT( argv[2] ));
	}
	*rval = BOOLEAN_TO_JSVAL( LineOfSight( mSock, mChar, x, y, z, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) ) ;
	return JS_TRUE;
}


JSBool CSocket_DisplayDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(CSocket_DisplayDamage) Invalid Number of Arguments %d, needs: 2", argc );
		return JS_TRUE;
	}

	CSocket *mSock			= static_cast<CSocket *>(JS_GetPrivate( cx, obj ));
	JSEncapsulate myClass( cx, &(argv[0]) );

	if( myClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		MethodError( "CSocket_DisplayDamage: Passed an invalid Character" );
		return JS_FALSE;
	}

	CChar *mChar			= static_cast<CChar *>(myClass.toObject());
	if( !ValidateObject( mChar )  )
	{
		MethodError( "(CSocket_DisplayDamage): Passed an invalid Character" );
		return JS_TRUE;
	}

	JSEncapsulate damage( cx, &(argv[1]) );

	CPDisplayDamage dispDamage( (*mChar), (UI16)damage.toInt() );
	mSock->Send( &dispDamage );

	return JS_TRUE;
}

JSBool CChar_ReactOnDamage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "(CChar_ReactOnDamage) Invalid Number of Arguments %d, needs: 1 (damageType) or 2 (damageType and attacker)", argc );
		return JS_TRUE;
	}

	CChar *attacker	= NULL;
	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar )  )
	{
		MethodError( "(CChar_ReactOnDamage): Operating on an invalid Character" );
		return JS_TRUE;
	}
	JSEncapsulate damage( cx, &(argv[0]) );

	if( argc >= 2 )
	{
		JSEncapsulate attackerClass( cx, &(argv[1]) );
		if( attackerClass.ClassName() != "UOXChar" )	// It must be a character!
		{
			MethodError( "CChar_ReactOnDamage: Passed an invalid Character" );
			return JS_FALSE;
		}

		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ) )
			attacker = NULL;
		else
		{
			attacker	= static_cast<CChar *>(attackerClass.toObject());
			if( !ValidateObject( attacker )  )
			{
				MethodError( "(CChar_ReactOnDamage): Passed an invalid Character" );
				return JS_TRUE;
			}
		}
	}
	mChar->ReactOnDamage( static_cast<WeatherType>(damage.toInt()), attacker );
	return JS_TRUE;
}

JSBool CChar_Damage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	bool doRepsys;

	if( argc != 1 && argc != 2 && argc != 3)
	{
		MethodError( "(CChar_Damage) Invalid Number of Arguments %d, needs: 1 (amount), 2 (amount and attacker) or 3 (amount, attacker and doRepsys)", argc );
		return JS_TRUE;
	}

	CChar *attacker	= NULL;
	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar )  )
	{
		MethodError( "(CChar_Damage): Operating on an invalid Character" );
		return JS_TRUE;
	}
	JSEncapsulate damage( cx, &(argv[0]) );

	if( argc >= 2 )
	{
		JSEncapsulate attackerClass( cx, &(argv[1]) );
		if( attackerClass.ClassName() != "UOXChar" )	// It must be a character!
		{
			MethodError( "CChar_Damage: Passed an invalid Character" );
			return JS_FALSE;
		}

		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ) )
			attacker = NULL;
		else
		{
			attacker	= static_cast<CChar *>(attackerClass.toObject());
			if( !ValidateObject( attacker )  )
			{
				MethodError( "(CChar_Damage): Passed an invalid Character" );
				return JS_TRUE;
			}
		}
	}
	if( argc >= 3 )
	{
		doRepsys = ( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE );
	}
	mChar->Damage( damage.toInt(), attacker, doRepsys );
	return JS_TRUE;
}

JSBool CChar_Heal( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "(CChar_Heal) Invalid Number of Arguments %d, needs: 1 (amount) or 2 (amount and healer)", argc );
		return JS_TRUE;
	}

	CChar *healer	= NULL;
	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ));
	if( !ValidateObject( mChar )  )
	{
		MethodError( "(CChar_Heal): Operating on an invalid Character" );
		return JS_TRUE;
	}
	JSEncapsulate Heal( cx, &(argv[0]) );

	if( argc == 2 )
	{
		JSEncapsulate healerClass( cx, &(argv[1]) );
		if( healerClass.ClassName() != "UOXChar" )	// It must be a character!
		{
			MethodError( "CChar_Heal: Passed an invalid Character" );
			return JS_FALSE;
		}

		if( healerClass.isType( JSOT_VOID ) || healerClass.isType( JSOT_NULL ) )
			healer = NULL;
		else
		{
			healer	= static_cast<CChar *>(healerClass.toObject());
			if( !ValidateObject( healer )  )
			{
				MethodError( "(CChar_Heal): Passed an invalid Character" );
				return JS_TRUE;
			}
		}
	}

	mChar->Heal( static_cast<SI16>(Heal.toInt()), healer );
	return JS_TRUE;
}

JSBool CBase_Resist( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 2 )
	{
		MethodError( "Resist: Invalid number of arguments (takes 1, the resist type or 2, the resist type and value to set)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar		= NULL;
	CItem *mItem		= NULL;

	// Let's validate the char/item

	if( myClass.ClassName() == "UOXItem" )
	{
		mItem	= (CItem *)myClass.toObject();
		if( !ValidateObject( mItem ) )  
		{
			MethodError( "Resist: Passed an invalid Item" );
			return JS_FALSE;
		}
	}
	else if( myClass.ClassName() == "UOXChar" )
	{
		mChar	= (CChar *)myClass.toObject();
		if( !ValidateObject( mChar ) )  
		{
			MethodError( "Resist: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	
	JSEncapsulate resistType( cx, &(argv[0]) );

	if( argc == 1 )
	{
		if( ValidateObject( mChar ) )
			*rval = INT_TO_JSVAL( mChar->GetResist( (WeatherType)resistType.toInt() ) );
		else if( ValidateObject( mItem ) )
			*rval = INT_TO_JSVAL( mItem->GetResist( (WeatherType)resistType.toInt() ) );
		else
			*rval = JS_FALSE;
	}
	if( argc == 2 )
	{
		*rval = JS_TRUE;
		JSEncapsulate value( cx, &(argv[1]) );
		if( ValidateObject( mChar ) )
			mChar->SetResist( (UI16)value.toInt(), (WeatherType)resistType.toInt() );
		else if( ValidateObject( mItem ) )
			mItem->SetResist( (UI16)value.toInt(), (WeatherType)resistType.toInt() );
		else
			*rval = JS_FALSE;
	}	
	return JS_TRUE;
}

JSBool CChar_Defense( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3)
	{
		MethodError( "Defense: Invalid number of arguments (takes 3, the hit location, the resist type and if the armor should get damaged)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	CChar *mChar		= NULL;
	
	// Let's validate the char

	if( myClass.ClassName() == "UOXChar" )
	{
		mChar	= (CChar *)myClass.toObject();
		if( !ValidateObject( mChar ) )  
		{
			MethodError( "Defense: Passed an invalid Character" );
			return JS_FALSE;
		}
	}
	else
	{
		MethodError( "Defense: Passed an invalid Character" );
		return JS_FALSE;
	}

	JSEncapsulate hitLoc( cx, &(argv[0]) );
	JSEncapsulate resistType( cx, &(argv[1]) );
	JSEncapsulate doArmorDamage( cx, &(argv[2]) );

	*rval = INT_TO_JSVAL( Combat->calcDef( mChar, (UI08)hitLoc.toInt(), doArmorDamage.toBool(), (WeatherType)resistType.toInt() ) );
	return JS_TRUE;
}


// Party Methods

JSBool CParty_Remove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Remove: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXParty" )
	{
		Party *ourParty = static_cast<Party *>(myClass.toObject());
		if( ourParty == NULL )
		{
			MethodError( "Remove: Invalid party" );
			return JS_FALSE;
		}

		JSEncapsulate toRemove( cx, &(argv[0]) );
		CChar *charToRemove = static_cast<CChar *>(toRemove.toObject());
		if( !ValidateObject( charToRemove ) )  
		{
			MethodError( "Remove: Invalid character to remove" );
			return JS_FALSE;
		}
		*rval = BOOLEAN_TO_JSVAL( ourParty->RemoveMember( charToRemove ) );
	}
	return JS_TRUE;
}

JSBool CParty_Add( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Add: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );

	// let's setup our default return value here
	*rval = BOOLEAN_TO_JSVAL( false );

	if( myClass.ClassName() == "UOXParty" )
	{
		Party *ourParty = static_cast<Party *>(myClass.toObject());
		if( ourParty == NULL )
		{
			MethodError( "Add: Invalid party" );
			return JS_FALSE;
		}

		JSEncapsulate toAdd( cx, &(argv[0]) );
		CChar *charToAdd = static_cast<CChar *>(toAdd.toObject());
		if( !ValidateObject( charToAdd ) )  
		{
			MethodError( "Add: Invalid character to add" );
			return JS_FALSE;
		}
		if( charToAdd->IsNpc() )
		{
			if( ourParty->IsNPC() )
				*rval = BOOLEAN_TO_JSVAL( ourParty->AddMember( charToAdd ) );
			else
			{
				*rval = BOOLEAN_TO_JSVAL( false );
				MethodError( "Add: Adding NPC to a PC party attempted" );
			}
		}
		else
		{
			if( ourParty->IsNPC() )
			{
				*rval = BOOLEAN_TO_JSVAL( false );
				MethodError( "Add: Adding PC to a NPC party attempted" );
			}
			else
			{	// Send PC the invite
				// OK, what this means is that it's a PC party, and because it exists
				// we know that the leader needs to be active!  And if he's active,
				// he should ALSO be online

				CSocket *targSock	= charToAdd->GetSocket();
				if( targSock != NULL )
				{
					CChar *leader		= ourParty->Leader();
					CSocket *leaderSock = leader->GetSocket();

					if( leaderSock != NULL )
					{
						CPPartyInvitation toSend;
						toSend.Leader( leader );
						targSock->Send( &toSend );
						targSock->sysmessage( "You have been invited to join a party, type /accept or /decline to deal with the invitation" );
						*rval = BOOLEAN_TO_JSVAL( true );
					}
					else
					{
						*rval = BOOLEAN_TO_JSVAL( false );
						MethodError( "Add: PC selected is not online" );
					}
				}
				else
				{
					*rval = BOOLEAN_TO_JSVAL( false );
					MethodError( "Add: PC selected is not online" );
				}
			}
		}
	}
	return JS_TRUE;
}

JSBool CParty_GetMember( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "GetMember: Invalid number of arguments (1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, obj );
	if( myClass.ClassName() == "UOXParty" )
	{
		Party *ourParty = static_cast<Party *>(myClass.toObject());
		if( ourParty == NULL )
		{
			MethodError( "GetMember: Invalid party" );
			return JS_FALSE;
		}

		JSEncapsulate toGetMember( cx, &(argv[0]) );
		size_t memberOffset = toGetMember.toInt();
		if( memberOffset < 0 || memberOffset >= ourParty->MemberList()->size() )  
		{
			MethodError( "GetMember: Invalid character to get, index out of bounds" );
			*rval = JSVAL_NULL;
			return JS_TRUE;
		}
		CChar *mChar = (*(ourParty->MemberList()))[memberOffset]->Member();
		if( mChar == NULL )
			*rval = JSVAL_NULL;
		else
		{
			JSObject *myJSChar	= JSEngine->AcquireObject( IUE_CHAR, mChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
			*rval				= OBJECT_TO_JSVAL( myJSChar );
		}
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

}

