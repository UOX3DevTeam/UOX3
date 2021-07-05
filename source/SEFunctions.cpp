// Here are the functions that are exposed to the Script Engine
#include <filesystem>
#include "uox3.h"
#include "cdice.h"
#include "SEFunctions.h"
#include "cGuild.h"
#include "combat.h"
#include "movement.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cRaces.h"
#include "skills.h"
#include "commands.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "classes.h"
#include "regions.h"
#include "magic.h"
#include "ssection.h"
#include "cThreadQueue.h"
#include "cHTMLSystem.h"
#include "cServerDefinitions.h"
#include "Dictionary.h"
#include "speech.h"
#include "gump.h"
#include "ObjectFactory.h"
#include "network.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"
#include "JSEncapsulate.h"
#include "CJSEngine.h"
#include "PartySystem.h"
#include "cSpawnRegion.h"
#include "CPacketSend.h"



void		LoadTeleportLocations( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		UnloadRegions( void );
void		UnloadSpawnRegions( void );
void		LoadSkills( void );

#define __EXTREMELY_VERBOSE__

#ifdef __EXTREMELY_VERBOSE__
void DoSEErrorMessage( const std::string& txt )
{
	if (!txt.empty()){
		auto msg = txt ;
		if (msg.size()>512){
			msg = msg.substr(0,512);
		}
		Console.error( msg );
	}

}
#else
void DoSEErrorMessage( const std::string& txt )
{
	return;
}
#endif

std::map< std::string, ObjectType > stringToObjType;

void InitStringToObjType( void )
{
	stringToObjType["BASEOBJ"]		= OT_CBO;
	stringToObjType["CHARACTER"]	= OT_CHAR;
	stringToObjType["ITEM"]			= OT_ITEM;
	stringToObjType["SPAWNER"]		= OT_SPAWNER;
	stringToObjType["MULTI"]		= OT_MULTI;
	stringToObjType["BOAT"]			= OT_BOAT;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ObjectType FindObjTypeFromString( std::string strToFind )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine object type (ITEM, CHARACTER, MULTI, etc) based on provided string
//o-----------------------------------------------------------------------------------------------o
ObjectType FindObjTypeFromString( std::string strToFind )
{
	if( stringToObjType.empty() ) // if we haven't built our array yet
	{
		InitStringToObjType();
	}
	std::map< std::string, ObjectType >::const_iterator toFind = stringToObjType.find( strutil::toupper( strToFind ));
	if( toFind != stringToObjType.end() )
	{
		return toFind->second;
	}
	return OT_CBO;
}

// Effect related functions
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DoTempEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Does a temporary effect (things like protection, night sight, and what not) frm
//|					src to trg. If iType = 0, then it's a character, otherwise it's an item.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DoTempEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 7 )
	{
		DoSEErrorMessage( "DoTempEffect: Invalid number of arguments (takes 7 or 8)" );
		return JS_FALSE;
	}
	UI08 iType			= static_cast<UI08>(JSVAL_TO_INT( argv[0] ));
	UI32 targNum		= JSVAL_TO_INT( argv[3] );
	UI08 more1			= (UI08)JSVAL_TO_INT( argv[4] );
	UI08 more2			= (UI08)JSVAL_TO_INT( argv[5] );
	UI08 more3			= (UI08)JSVAL_TO_INT( argv[6] );

	CItem *myItemPtr	= nullptr;

	if( argc == 8 )
	{
		JSObject *myitemptr = nullptr;
		myitemptr = JSVAL_TO_OBJECT( argv[7] );
		myItemPtr = static_cast<CItem *>(JS_GetPrivate( cx, myitemptr ));
	}

	JSObject *mysrc		= JSVAL_TO_OBJECT( argv[1] );
	CChar *mysrcChar	= static_cast<CChar*>(JS_GetPrivate( cx, mysrc ));

	if( !ValidateObject( mysrcChar ) )
	{
		DoSEErrorMessage( "DoTempEffect: Invalid src" );
		return JS_FALSE;
	}

	if( iType == 0 )	// character
	{
		JSObject *mydestc = JSVAL_TO_OBJECT( argv[2] );
		CChar *mydestChar = static_cast<CChar*>(JS_GetPrivate( cx, mydestc ));

		if( !ValidateObject( mydestChar ) )
		{
			DoSEErrorMessage( "DoTempEffect: Invalid target " );
			return JS_FALSE;
		}
		if( argc == 8 )
			Effects->tempeffect( mysrcChar, mydestChar, static_cast<SI08>(targNum), more1, more2, more3, myItemPtr );
		else
			Effects->tempeffect( mysrcChar, mydestChar, static_cast<SI08>(targNum), more1, more2, more3 );
	}
	else
	{
		JSObject *mydesti = JSVAL_TO_OBJECT( argv[2] );
		CItem *mydestItem = static_cast<CItem *>(JS_GetPrivate( cx, mydesti ));

		if( !ValidateObject( mydestItem ) )
		{
			DoSEErrorMessage( "DoTempEffect: Invalid target " );
			return JS_FALSE;
		}
		Effects->tempeffect( mysrcChar, mydestItem, static_cast<SI08>(targNum), more1, more2, more3 );
	}
	return JS_TRUE;
}

// Speech related functions
void sysBroadcast( const std::string& txt );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_BroadcastMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Broadcasts specified string to all online players
//o-----------------------------------------------------------------------------------------------o
JSBool SE_BroadcastMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "BroadcastMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string trgMessage = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgMessage.empty() || trgMessage.length() == 0 )
	{
		DoSEErrorMessage( strutil::format("BroadcastMessage: Invalid string (%s)", trgMessage.c_str()) );
		return JS_FALSE;
	}
	sysBroadcast( trgMessage );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CalcItemFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns item object based on provided serial
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CalcItemFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		DoSEErrorMessage( "CalcItemFromSer: Invalid number of arguments (takes 1 or 4)" );
		return JS_FALSE;
	}
	SERIAL targSerial;
	if( argc == 1 )
	{
		std::string str = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
		targSerial = strutil::value<SERIAL>(str);
	}
	else
		targSerial = calcserial( (UI08)JSVAL_TO_INT( argv[0] ), (UI08)JSVAL_TO_INT( argv[1] ), (UI08)JSVAL_TO_INT( argv[2] ), (UI08)JSVAL_TO_INT( argv[3] ) );

	CItem *newItem		= calcItemObjFromSer( targSerial );
	if( newItem != nullptr )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CalcMultiFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns item object based on provided serial
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CalcMultiFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		DoSEErrorMessage( "CalcMultiFromSer: Invalid number of arguments (takes 1 or 4)" );
		return JS_FALSE;
	}
	SERIAL targSerial;
	if( argc == 1 )
		targSerial = (SERIAL)JSVAL_TO_INT( argv[0] );
	else
		targSerial = calcserial( (UI08)JSVAL_TO_INT( argv[0] ), (UI08)JSVAL_TO_INT( argv[1] ), (UI08)JSVAL_TO_INT( argv[2] ), (UI08)JSVAL_TO_INT( argv[3] ) );

	CItem *newMulti		= calcMultiFromSer( targSerial );
	if( newMulti != nullptr )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newMulti, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CalcCharFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns character object based on provided serial
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CalcCharFromSer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 )
	{
		DoSEErrorMessage( "CalcCharFromSer: Invalid number of arguments (takes 1 or 4)" );
		return JS_FALSE;
	}
	SERIAL targSerial = INVALIDSERIAL;
	if( argc == 1 )
		targSerial = (SERIAL)JSVAL_TO_INT( argv[0] );
	else
		targSerial = calcserial( (UI08)JSVAL_TO_INT( argv[0] ), (UI08)JSVAL_TO_INT( argv[1] ), (UI08)JSVAL_TO_INT( argv[2] ), (UI08)JSVAL_TO_INT( argv[3] ) );

	CChar *newChar		= calcCharObjFromSer( targSerial );
	if( newChar != nullptr )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, newChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DoMovingEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a moving effect from source object to target object or location
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DoMovingEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 6 )
	{
		DoSEErrorMessage( "DoMovingEffect: Invalid number of arguments (takes 6->8 or 8->10)" );
		return JS_FALSE;
	}
	JSObject *srcObj	= JSVAL_TO_OBJECT( argv[0] );
	CBaseObject *src	= static_cast<CBaseObject *>(JS_GetPrivate( cx, srcObj ));
	if( !ValidateObject( src ) )
	{
		DoSEErrorMessage( "DoMovingEffect: Invalid source object" );
		return JS_FALSE;
	}
	bool targLocation	= false;
	UI08 offset			= 0;
	UI16 targX			= 0;
	UI16 targY			= 0;
	SI08 targZ			= 0;
	CBaseObject *trg	= nullptr;
	if( JSVAL_IS_INT( argv[1] ) )
	{	// Location moving effect
		targLocation	= true;
		offset			= true;
		targX			= (UI16)JSVAL_TO_INT( argv[1] );
		targY			= (UI16)JSVAL_TO_INT( argv[2] );
		targZ			= (SI08)JSVAL_TO_INT( argv[3] );
	}
	else
	{
		JSObject *trgObj	= JSVAL_TO_OBJECT( argv[1] );
		trg					= static_cast<CBaseObject *>(JS_GetPrivate( cx, trgObj ));
		if( !ValidateObject( trg ) )
		{
			DoSEErrorMessage( "DoMovingEffect: Invalid target object" );
			return JS_FALSE;
		}
	}
	UI16 effect		= (UI16)JSVAL_TO_INT( argv[2+offset] );
	UI08 speed		= (UI08)JSVAL_TO_INT( argv[3+offset] );
	UI08 loop		= (UI08)JSVAL_TO_INT( argv[4+offset] );
	bool explode	= ( JSVAL_TO_BOOLEAN( argv[5+offset] ) == JS_TRUE );
	UI32 hue = 0, renderMode = 0;
	if( argc - offset >= 7 ) // there's at least 7/9 parameters
		hue = (UI32)JSVAL_TO_INT( argv[6+offset] );
	if( argc - offset >= 8 ) // there's at least 8/10 parameters
		renderMode = (UI32)JSVAL_TO_INT( argv[7+offset] );

	if( targLocation )
		Effects->PlayMovingAnimation( src, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode );
	else
		Effects->PlayMovingAnimation( src, trg, effect, speed, loop, explode, hue, renderMode );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DoStaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a static effect at a target location
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DoStaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		DoSEErrorMessage( "DoStaticEffect: Invalid number of arguments (takes 7 - targX, targY, targZ, effectID, speed, loop, explode)" );
		return JS_FALSE;
	}

	SI16 targX		= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 targY		= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	SI16 targZ		= static_cast<SI16>(JSVAL_TO_INT( argv[2] ));
	UI16 effectID	= (UI16)JSVAL_TO_INT( argv[3] );
	UI08 speed		= (UI08)JSVAL_TO_INT( argv[4] );
	UI08 loop		= (UI08)JSVAL_TO_INT( argv[5] );
	bool explode	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );

	Effects->PlayStaticAnimation( targX, targY, targZ, effectID, speed, loop, explode );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RandomNumber( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random number between loNum and hiNum
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RandomNumber( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RandomNumber: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate loVal( cx, &(argv[0]) );
	JSEncapsulate hiVal( cx, &(argv[1]) );
	*rval = INT_TO_JSVAL( RandomNum( loVal.toInt(), hiVal.toInt() ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_MakeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Character creates specified item based on entry in CREATE DFNs
//o-----------------------------------------------------------------------------------------------o
JSBool SE_MakeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "MakeItem: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	JSObject *mSock = JSVAL_TO_OBJECT( argv[0] );
	CSocket *sock	= static_cast<CSocket *>(JS_GetPrivate( cx, mSock ));
	JSObject *mChar = JSVAL_TO_OBJECT( argv[1] );
	CChar *player	= static_cast<CChar *>(JS_GetPrivate( cx, mChar ));
	if( !ValidateObject( player ) )
	{
		DoSEErrorMessage( "MakeItem: Invalid character" );
		return JS_FALSE;
	}
	UI16 itemMenu		= (UI16)JSVAL_TO_INT( argv[2] );
	createEntry *toFind = Skills->FindItem( itemMenu );
	if( toFind == nullptr )
	{
		DoSEErrorMessage( strutil::format("MakeItem: Invalid make item (%i)", itemMenu) );
		return JS_FALSE;
	}
	Skills->MakeItem( *toFind, player, sock, itemMenu );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CommandLevelReq( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the command level required to execute the specified command
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CommandLevelReq( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CommandLevelReq: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( test.empty() || test.length() == 0 )
	{
		DoSEErrorMessage( "CommandLevelReq: Invalid command name" );
		return JS_FALSE;
	}
	CommandMapEntry *details = Commands->CommandDetails( test );
	if( details == nullptr )
		*rval = INT_TO_JSVAL( 255 );
	else
		*rval = INT_TO_JSVAL( details->cmdLevelReq );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CommandExists( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specified command exists in command table or not
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CommandExists( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CommandExists: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string test = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( test.empty() || test.length() == 0 )
	{
		DoSEErrorMessage( "CommandExists: Invalid command name" );
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( Commands->CommandExists( test ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_FirstCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the first command in the table. If nothing, it's a 0 length string.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_FirstCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	const std::string tVal = Commands->FirstCommand();
	JSString *strSpeech = nullptr;
	if( tVal.empty() )
		strSpeech = JS_NewStringCopyZ( cx, "" );
	else
		strSpeech = JS_NewStringCopyZ( cx, tVal.c_str() );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_NextCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the next command in the table. If nothing, it's a 0 length string.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_NextCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	const std::string tVal = Commands->NextCommand();
	JSString *strSpeech = nullptr;
	if( tVal.empty() )
		strSpeech = JS_NewStringCopyZ( cx, "" );
	else
		strSpeech = JS_NewStringCopyZ( cx, tVal.c_str() );

	*rval = STRING_TO_JSVAL( strSpeech );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_FinishedCommandList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if there are no more commands left in the table.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_FinishedCommandList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = BOOLEAN_TO_JSVAL( Commands->FinishedCommandList() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RegisterCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	If called from within CommandRegistration() function in a script registered
//|					under the COMMAND_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP, will register
//|					the specified command in the command table and call the function in the same
//|					script whose name corresponds with the command name, in the shape of
//|						function command_CMDNAME( socket, cmdString )
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RegisterCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "  RegisterCommand: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	std::string toRegister	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	UI08 execLevel			= static_cast<UI08>(JSVAL_TO_INT( argv[1] ));
	bool isEnabled			= ( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE );
	UI16 scriptID			= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );

	if( scriptID == 0xFFFF )
	{
		DoSEErrorMessage( " RegisterCommand: JS Script has an Invalid ScriptID" );
		return JS_FALSE;
	}

	Commands->Register( toRegister, scriptID, execLevel, isEnabled );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RegisterSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	If called from within SpellRegistration() function in a script registered under
//|					the MAGIC_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP, will register the
//|					onSpellCast() event in the same script as a global listener for use of the
//|					specified magic spell.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RegisterSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterSpell: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	SI32 spellNumber			= JSVAL_TO_INT( argv[0] );
	bool isEnabled			= ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );
	cScript *myScript		= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	Magic->Register( myScript, spellNumber, isEnabled );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RegisterSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for use of specified skill, and
//|					triggers onSkill() event in same script when specified skill is used, if
//|					script is added under the SKILLUSE_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a SkillRegistration() function
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RegisterSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterSkill: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	SI32 skillNumber			= JSVAL_TO_INT( argv[0] );
	bool isEnabled			= ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );
	UI16 scriptID			= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	if( scriptID != 0xFFFF )
	{
#if defined( UOX_DEBUG_MODE )
		Console.print( " " );
		Console.MoveTo( 15 );
		Console.print( "Registering skill number " );
		Console.TurnYellow();
		Console.print( strutil::format("%i", skillNumber ));
		if( !isEnabled )
		{
			Console.TurnRed();
			Console.print( " [DISABLED]" );
		}
		Console.print( "\n" );
		Console.TurnNormal();
#endif
		// If skill is not enabled, unset scriptID from skill data
		if( !isEnabled )
		{
			cwmWorldState->skill[skillNumber].jsScript = 0;
			return JS_FALSE;
		}

		// Skillnumber above ALLSKILLS refers to STR, INT, DEX, Fame and Karma,
		if( skillNumber < 0 || skillNumber >= ALLSKILLS )
			return JS_TRUE;

		// Both scriptID and skillNumber seem valid; assign scriptID to this skill
		cwmWorldState->skill[skillNumber].jsScript = scriptID;
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RegisterPacket( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for a specific network packet, and
//|					triggers onPacketReceive() event in same script when this network packet is sent,
//|					if script is added under the PACKET_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a PacketRegistration() function
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RegisterPacket( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterPacket: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UI08 packet			= static_cast<UI08>(JSVAL_TO_INT( argv[0] ));
	UI08 subCmd			= static_cast<UI08>(JSVAL_TO_INT( argv[1] ));
	UI16 scriptID		= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );
	if( scriptID != 0xFFFF )
	{
#if defined( UOX_DEBUG_MODE )
		Console.print( strutil::format("Registering packet number 0x%X, subcommand 0x%x\n", packet, subCmd) );
#endif
		Network->RegisterPacket( packet, subCmd, scriptID );
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RegisterKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for a specific keypress in UOX3 console,
//|					and triggers specified function in same script when key is pressed, if script
//|					is added under the CONSOLE_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a ConsoleRegistration() function
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RegisterKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterKey: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &(argv[0]) );
	std::string toRegister	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	UI16 scriptID			= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );

	if( scriptID == 0xFFFF )
	{
		DoSEErrorMessage( "RegisterKey: JS Script has an Invalid ScriptID" );
		return JS_FALSE;
	}
	SI32 toPass = 0;
	if( encaps.isType( JSOT_STRING ) )
	{
		std::string enStr = encaps.toString();
		if( enStr.length() != 0 )
			toPass  = enStr[0];
		else
		{
			DoSEErrorMessage( "RegisterKey: JS Script passed an invalid key to register" );
			return JS_FALSE;
		}
	}
	else
		toPass = encaps.toInt();
	Console.RegisterKey( toPass, toRegister, scriptID );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RegisterConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RegisterConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "RegisterConsoleFunc: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	std::string funcToRegister	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	std::string toRegister		= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	UI16 scriptID				= JSMapping->GetScriptID( JS_GetGlobalObject( cx ) );

	if( scriptID == 0xFFFF )
	{
		DoSEErrorMessage( "RegisterConsoleFunc: JS Script has an Invalid ScriptID" );
		return JS_FALSE;
	}

	Console.RegisterFunc( funcToRegister, toRegister, scriptID );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DisableCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables a specified command on the server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DisableCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableCommand: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string toDisable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Commands->SetCommandStatus( toDisable, false );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DisableKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables specified key in console
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DisableKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableKey: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	SI32 toDisable = JSVAL_TO_INT( argv[0] );
	Console.SetKeyStatus( toDisable, false );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DisableConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DisableConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableConsoleFunc: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string toDisable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Console.SetFuncStatus( toDisable, false );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DisableSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables specified spell on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DisableSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "DisableSpell: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	SI32 spellNumber = JSVAL_TO_INT( argv[0] );
	Magic->SetSpellStatus( spellNumber, false );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_EnableCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified command on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_EnableCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableCommand: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string toEnable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Commands->SetCommandStatus( toEnable, true );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_EnableSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified spell on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_EnableSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableSpell: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	SI32 spellNumber = JSVAL_TO_INT( argv[0] );
	Magic->SetSpellStatus( spellNumber, true );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_EnableKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified key in console
//o-----------------------------------------------------------------------------------------------o
JSBool SE_EnableKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableKey: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	SI32 toEnable = JSVAL_TO_INT( argv[0] );
	Console.SetKeyStatus( toEnable, true );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_EnableConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o-----------------------------------------------------------------------------------------------o
JSBool SE_EnableConsoleFunc( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "EnableConsoleFunc: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	std::string toEnable	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	Console.SetFuncStatus( toEnable, false );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetHour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the hour of the current UO day
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetHour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	bool ampm = cwmWorldState->ServerData()->ServerTimeAMPM();
	UI08 hour = cwmWorldState->ServerData()->ServerTimeHours();
	if( ampm )
		*rval = INT_TO_JSVAL( static_cast<UI64>(hour) + 12 );
	else
		*rval = INT_TO_JSVAL( hour );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the minute of the current UO day
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI08 minute = cwmWorldState->ServerData()->ServerTimeMinutes();
	*rval = INT_TO_JSVAL( minute );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetDay( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the day number of the server (UO days since server start)
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetDay( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SI16 day = cwmWorldState->ServerData()->ServerTimeDay();
	*rval = INT_TO_JSVAL( day );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_SecondsPerUOMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets the amonut of real life seconds associated with minute in the game
//o-----------------------------------------------------------------------------------------------o
JSBool SE_SecondsPerUOMinute( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( "SecondsPerUOMinute: Invalid number of arguments (takes 0 or 1)" );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		UI16 secondsPerUOMinute = (UI16)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->ServerSecondsPerUOMinute( secondsPerUOMinute );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ServerSecondsPerUOMinute() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetCurrentClock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets current server clock, aka number of clock ticks since server startup
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetCurrentClock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->GetUICurrentTime() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_SpawnNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns NPC based on definition in NPC DFNs
//o-----------------------------------------------------------------------------------------------o
JSBool SE_SpawnNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 5 || argc > 6 )
	{
		DoSEErrorMessage( "SpawnNPC: Invalid number of arguments (takes 5 or 6)" );
		return JS_FALSE;
	}

	CChar *cMade		= nullptr;
	std::string nnpcNum	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	UI16 x				= (UI16)JSVAL_TO_INT( argv[1] );
	UI16 y				= (UI16)JSVAL_TO_INT( argv[2] );
	SI08 z				= (SI08)JSVAL_TO_INT( argv[3] );
	UI08 world			= (UI08)JSVAL_TO_INT( argv[4] );
	UI16 instanceID = ( argc == 6 ? (SI16)JSVAL_TO_INT( argv[5] ) : 0 );

	cMade				= Npcs->CreateNPCxyz( nnpcNum, x, y, z, world, instanceID );
	if( cMade != nullptr )
	{
		JSObject *myobj		= JSEngine->AcquireObject( IUE_CHAR, cMade, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval				= OBJECT_TO_JSVAL( myobj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CreateDFNItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates item based on definition in item DFNs
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CreateDFNItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 3 )
	{
		DoSEErrorMessage( "CreateDFNItem: Invalid number of arguments (takes at least 3)" );
		return JS_FALSE;
	}

	CSocket *mySock = nullptr;
	if( argv[0] != JSVAL_NULL )
	{
		JSObject *mSock			= JSVAL_TO_OBJECT( argv[0] );
		mySock					= static_cast<CSocket *>(JS_GetPrivate( cx, mSock ));
	}

	CChar *myChar = nullptr;
	if( argv[1] != JSVAL_NULL )
	{
		JSObject *mChar			= JSVAL_TO_OBJECT( argv[1] );
		myChar					= static_cast<CChar *>(JS_GetPrivate( cx, mChar ));
	}

	std::string bpSectNumber	= JS_GetStringBytes( JS_ValueToString( cx, argv[2] ) );
	bool bInPack				= true;
	UI16 iAmount				= 1;
	ObjectType itemType			= OT_ITEM;
	UI08 worldNumber = 0;
	UI16 instanceID = 0;

	if( argc > 3 )
		iAmount					= static_cast< UI16 >(JSVAL_TO_INT( argv[3] ));
	if( argc > 4 )
	{
		std::string objType		= JS_GetStringBytes( JS_ValueToString( cx, argv[4] ) );
		itemType				= FindObjTypeFromString( objType );
	}
	if( argc > 5 )
		bInPack					= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	if( argc > 6 )
		worldNumber				= static_cast<UI08>(JSVAL_TO_INT( argv[6] ));
	if( argc > 7 )
		instanceID				= static_cast<UI16>(JSVAL_TO_INT( argv[7] ));

	CItem *newItem = nullptr;
	if( myChar != nullptr )
		newItem = Items->CreateScriptItem( mySock, myChar, bpSectNumber, iAmount, itemType, bInPack );
	else
		newItem = Items->CreateBaseScriptItem( bpSectNumber, worldNumber, iAmount, instanceID, itemType );

	if( newItem != nullptr )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CreateBlankItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a "blank" item with default values from client's tiledata
//|	Notes		-	Default values can be overridden through harditems.dfn
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CreateBlankItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		DoSEErrorMessage( "CreateBlankItem: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}
	CItem *newItem			= nullptr;
	CSocket *mySock			= nullptr;
	if( argv[0] != JSVAL_NULL )
	{
		JSObject *mSock		= JSVAL_TO_OBJECT( argv[0] );
		mySock				= static_cast<CSocket *>(JS_GetPrivate( cx, mSock ));
	}

	JSObject *mChar			= JSVAL_TO_OBJECT( argv[1] );
	CChar *myChar			= static_cast<CChar *>(JS_GetPrivate( cx, mChar ));
	SI32 amount				= (SI32)JSVAL_TO_INT( argv[2] );
	std::string itemName	= JS_GetStringBytes( JS_ValueToString( cx, argv[3] ) );
	bool isString			= false; //Never used!!
	std::string szItemName;			 //Never used!!
	UI16 itemID				= INVALIDID;
	if( JSVAL_IS_STRING( argv[4] ) )
	{
		szItemName = JS_GetStringBytes( JS_ValueToString( cx, argv[4] ) ); //Never used!!
		isString = true; //Never used!!
	}
	else
		itemID				= (UI16)JSVAL_TO_INT( argv[4] );
	UI16 colour				= (UI16)JSVAL_TO_INT( argv[5] );
	std::string objType		= JS_GetStringBytes( JS_ValueToString( cx, argv[6] ) );
	ObjectType itemType		= FindObjTypeFromString( objType );
	bool inPack				= ( JSVAL_TO_BOOLEAN( argv[7] ) == JS_TRUE );

	newItem = Items->CreateItem( mySock, myChar, itemID, amount, colour, itemType, inPack );
	if( newItem != nullptr )
	{
		if( itemName != "" )
			newItem->SetName( itemName );
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetMurderThreshold( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the max amount of kills allowed before a player turns red
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetMurderThreshold( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->RepMaxKills() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RollDice( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Rolls a die with specified number of sides, and adds a fixed value
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RollDice( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 3 )
	{
		DoSEErrorMessage( "RollDice: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	UI32 numDice = JSVAL_TO_INT( argv[0] );
	UI32 numFace = JSVAL_TO_INT( argv[1] );
	UI32 numAdd  = JSVAL_TO_INT( argv[2] );

	cDice toRoll( numDice, numFace, numAdd );

	*rval = INT_TO_JSVAL( toRoll.roll() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_RaceCompareByRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Compares the relations between two races
//|	Notes		-	0 - neutral
//|					1 to 100 - allies
//|					-1 to -100 - enemies
//o-----------------------------------------------------------------------------------------------o
JSBool SE_RaceCompareByRace( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID r0 = (RACEID)JSVAL_TO_INT( argv[0] );
	RACEID r1 = (RACEID)JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( Races->CompareByRace( r0, r1 ) );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_FindMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns multi at given coordinates, world and instanceID
//o-----------------------------------------------------------------------------------------------o
JSBool SE_FindMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 && argc != 4 && argc != 5 )
	{
		DoSEErrorMessage( "FindMulti: Invalid number of parameters (1, 4 or 5)" );
		return JS_FALSE;
	}
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 instanceID = 0;
	if( argc == 1 )
	{
		JSObject *myitemptr = JSVAL_TO_OBJECT( argv[0] );
		CBaseObject *myItemPtr = static_cast<CBaseObject *>(JS_GetPrivate( cx, myitemptr ));
		if( ValidateObject( myItemPtr ) )
		{
			xLoc		= myItemPtr->GetX();
			yLoc		= myItemPtr->GetY();
			zLoc		= myItemPtr->GetZ();
			worldNumber = myItemPtr->WorldNumber();
			instanceID	= myItemPtr->GetInstanceID();
		}
		else
		{
			DoSEErrorMessage( "FindMulti: Invalid object type" );
			return JS_FALSE;
		}
	}
	else
	{
		xLoc		= (SI16)JSVAL_TO_INT( argv[0] );
		yLoc		= (SI16)JSVAL_TO_INT( argv[1] );
		zLoc		= (SI08)JSVAL_TO_INT( argv[2] );
		worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
		if( argc == 5 )
		{
			instanceID = (UI16)JSVAL_TO_INT( argv[4] );
		}
	}
	CMultiObj *multi = findMulti( xLoc, yLoc, zLoc, worldNumber, instanceID );
	if( ValidateObject( multi ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	12 February, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns item closest to specified coordinates
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 && argc != 5 )
	{
		DoSEErrorMessage( "GetItem: Invalid number of parameters (4 or 5)" );
		return JS_FALSE;
	}
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 instanceID = 0;

	xLoc		= (SI16)JSVAL_TO_INT( argv[0] );
	yLoc		= (SI16)JSVAL_TO_INT( argv[1] );
	zLoc		= (SI08)JSVAL_TO_INT( argv[2] );
	worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
	if( argc == 5 )
	{
		instanceID = (UI16)JSVAL_TO_INT( argv[4] );
	}

	CItem *item = GetItemAtXYZ( xLoc, yLoc, zLoc, worldNumber, instanceID );
	if( ValidateObject( item ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, item, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_FindItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	12 February, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns item of given ID that is closest to specified coordinates
//o-----------------------------------------------------------------------------------------------o
JSBool SE_FindItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 && argc != 6 )
	{
		DoSEErrorMessage( "FindItem: Invalid number of parameters (5 or 6)" );
		return JS_FALSE;
	}
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 id = 0;
	UI16 instanceID = 0;

	xLoc		= (SI16)JSVAL_TO_INT( argv[0] );
	yLoc		= (SI16)JSVAL_TO_INT( argv[1] );
	zLoc		= (SI08)JSVAL_TO_INT( argv[2] );
	worldNumber = (UI08)JSVAL_TO_INT( argv[3] );
	id			= (UI16)JSVAL_TO_INT( argv[4] );
	if( argc == 6 )
	{
		instanceID = (UI16)JSVAL_TO_INT( argv[5] );
	}

	CItem *item = FindItemNearXYZ( xLoc, yLoc, zLoc, worldNumber, id, instanceID );
	if( ValidateObject( item ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, item, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CompareGuildByGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Compares the relations between two guilds
//|	Notes		-	0 - Neutral
//|					1 - War
//|					2 - Ally
//|					3 - Unknown
//|					4 - Same
//|					5 - Count
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CompareGuildByGuild( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	GUILDID toCheck		= (GUILDID)JSVAL_TO_INT( argv[0] );
	GUILDID toCheck2	= (GUILDID)JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( GuildSys->Compare( toCheck, toCheck2 ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_PossessTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Source town takes control over target town
//o-----------------------------------------------------------------------------------------------o
JSBool SE_PossessTown( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	UI16 town	= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 sTown	= (UI16)JSVAL_TO_INT( argv[1] );
	cwmWorldState->townRegions[town]->Possess( cwmWorldState->townRegions[sTown] );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_IsRaceWeakToWeather( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character's race is affected by given type of weather
//o-----------------------------------------------------------------------------------------------o
JSBool SE_IsRaceWeakToWeather( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race		= (RACEID)JSVAL_TO_INT( argv[0] );
	weathID toCheck = (weathID)JSVAL_TO_INT( argv[1] );
	CRace *tRace	= Races->Race( race );
	if( tRace == nullptr || toCheck >= WEATHNUM )
	{
		return JS_FALSE;
	}
	*rval = BOOLEAN_TO_JSVAL( tRace->AffectedBy( (WeatherType)toCheck ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetRaceSkillAdjustment( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage modifier for specified skill based on race
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetRaceSkillAdjustment( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		return JS_FALSE;
	}
	RACEID race = (RACEID)JSVAL_TO_INT( argv[0] );
	SI32 skill = JSVAL_TO_INT( argv[1] );
	*rval = INT_TO_JSVAL( Races->DamageFromSkill( skill, race ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_UseItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
//o-----------------------------------------------------------------------------------------------o
JSBool SE_UseItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "UseItem: Invalid number of arguments (takes 2 - socket/char, and item used)" );
		return JS_FALSE;
	}

	CChar *mChar = nullptr;
	CSocket *mySocket = nullptr;

	JSEncapsulate myClass( cx, &(argv[0]) );
	if( myClass.ClassName() == "UOXChar" )
	{
		if( myClass.isType( JSOT_OBJECT ))
		{
			mChar	= static_cast<CChar *>(myClass.toObject());
			if( !ValidateObject( mChar )  )
			{
				mChar = nullptr;
			}
		}
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		if( myClass.isType( JSOT_OBJECT ))
		{
			mySocket	= static_cast<CSocket *>(myClass.toObject());
			if( mySocket != nullptr )
				mChar = mySocket->CurrcharObj();
		}
	}

	JSObject *mItem	= JSVAL_TO_OBJECT( argv[1] );
	CItem *myItem	= static_cast<CItem *>(JS_GetPrivate( cx, mItem ));

	if( !ValidateObject( myItem ) )
	{
		DoSEErrorMessage( "UseItem: Invalid item" );
		return JS_FALSE;
	}

	if( !ValidateObject( mChar ) )
	{
		DoSEErrorMessage( "UseItem: Invalid character" );
		return JS_FALSE;
	}

	bool scriptExecuted = false;
	std::vector<UI16> scriptTriggers = myItem->GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		// Loop through all scriptIDs registered for item, check for scripts
		cScript *toExecute = JSMapping->GetScript( i );
		if( toExecute != nullptr )
		{
			// Script was found, let's check for OnUseUnChecked event
			SI08 retVal = toExecute->OnUseUnChecked( mChar, myItem );
			if( retVal != -1 )
			{
				scriptExecuted = true;
				if( retVal == 0 )
				{
					// Script returned 0 - don't continue with other scripts
					break;
				}
			}

			// No OnUseUnChecked event returned 0, check OnUseChecked event as well!
			// We don't actually do any checking in this case, as this should be
			// handled in script that calls UseItem
			retVal = toExecute->OnUseChecked( mChar, myItem );
			if( retVal != -1 )
			{
				scriptExecuted = true;
				if( retVal == 0 )
				{
					// Script returned 0 - don't continue with other scripts
					break;
				}
			}
		}
	}

	// Handle envoke stuff outside for loop, as we only want this to execute once
	if( scriptTriggers.size() == 0 || !scriptExecuted )
	{
		ItemTypes iType		= myItem->GetType();
		UI16 itemID			= myItem->GetID();
		UI16 envTrig		= 0;
		cScript *toExecute = nullptr;
		if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>(iType) ) )
		{
			envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>(iType) );
			toExecute = JSMapping->GetScript( envTrig );
		}
		else if( JSMapping->GetEnvokeByID()->Check( itemID ) )
		{
			envTrig = JSMapping->GetEnvokeByID()->GetScript( itemID );
			toExecute = JSMapping->GetScript( envTrig );
		}

		// Check for the onUse events in envoke scripts!
		if( toExecute != nullptr )
		{
			if( toExecute->OnUseUnChecked( mChar, myItem ) == 0 )
			{
				return JS_TRUE;
			}

			if( toExecute->OnUseChecked( mChar, myItem ) == 0 )
			{
				return JS_TRUE;
			}
		}
	}

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_TriggerTrap( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
//o-----------------------------------------------------------------------------------------------o
JSBool SE_TriggerTrap( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "UseItem: Invalid number of arguments (takes 2 - socket/char, and item (trap) triggered)" );
		return JS_FALSE;
	}

	CChar *mChar = nullptr;
	CSocket *mySocket = nullptr;

	JSEncapsulate myClass( cx, &( argv[0] ) );
	if( myClass.ClassName() == "UOXChar" )
	{
		if( myClass.isType( JSOT_OBJECT ) )
		{
			mChar = static_cast<CChar *>( myClass.toObject() );
			if( !ValidateObject( mChar ) )
			{
				mChar = nullptr;
			}
		}
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		if( myClass.isType( JSOT_OBJECT ) )
		{
			mySocket = static_cast<CSocket *>( myClass.toObject() );
			if( mySocket != nullptr )
				mChar = mySocket->CurrcharObj();
		}
	}

	if( !ValidateObject( mChar ) )
	{
		DoSEErrorMessage( "TriggerTrap: Invalid character" );
		return JS_FALSE;
	}

	JSObject *mItem = JSVAL_TO_OBJECT( argv[1] );
	CItem *myItem = static_cast<CItem *>( JS_GetPrivate( cx, mItem ) );

	if( !ValidateObject( myItem ) )
	{
		DoSEErrorMessage( "TriggerTrap: Invalid item" );
		return JS_FALSE;
	}

	if( myItem->GetTempVar( CITV_MOREZ, 1 ) == 1 && myItem->GetTempVar( CITV_MOREZ, 2 ) > 0 ) // Is trapped and set to deal more than 0 damage
	{
		Magic->MagicTrap( mChar, myItem );
	}

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_TriggerEvent( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers a an event/function in a different JS
//|	Notes		-	Takes at least 2 parameters, which is the script number to trigger and the
//|					function name to call. Any extra parameters are extra parameters to the JS event
//o-----------------------------------------------------------------------------------------------o
JSBool SE_TriggerEvent( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 2 )
	{
		return JS_FALSE;
	}

	UI16 scriptNumberToFire = (UI16)JSVAL_TO_INT( argv[0] );
	char *eventToFire		= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	cScript *toExecute		= JSMapping->GetScript( scriptNumberToFire );

	if( toExecute == nullptr || eventToFire == nullptr )
		return JS_FALSE;

	auto origContext = cx;
	auto origObject = obj;
	if( toExecute->CallParticularEvent( eventToFire, &argv[2], argc - 2 ) )
	{
		*rval = JS_TRUE;
		JS_SetGlobalObject( origContext, origObject );
		return JS_TRUE;
	}
	else
	{
		*rval = JS_FALSE;
		JS_SetGlobalObject( origContext, origObject );
		return JS_FALSE;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetPackOwner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns owner of container item is contained in (if any)
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetPackOwner( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "GetPackOwner: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UI08 mType		= (UI08)JSVAL_TO_INT( argv[1] );
	CChar *pOwner	= nullptr;

	if( mType == 0 )	// item
	{
		JSObject *mItem	= JSVAL_TO_OBJECT( argv[0] );
		CItem *myItem	= static_cast<CItem *>(JS_GetPrivate( cx, mItem ));
		pOwner			= FindItemOwner( myItem );
	}
	else				// serial
	{
		SI32 mSerItem	= JSVAL_TO_INT( argv[0] );
		pOwner			= FindItemOwner( calcItemObjFromSer( mSerItem ) );
	}
	if( ValidateObject( pOwner ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, pOwner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_FindRootContainer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns root container an item is contained in (if any)
//o-----------------------------------------------------------------------------------------------o
JSBool SE_FindRootContainer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "FindRootContainer: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	UI08 mType		= (UI08)JSVAL_TO_INT( argv[1] );
	CItem *iRoot	= nullptr;

	if( mType == 0 )	// item
	{
		JSObject *mItem	= JSVAL_TO_OBJECT( argv[0] );
		CItem *myItem	= static_cast<CItem *>(JS_GetPrivate( cx, mItem ));
		iRoot			= FindRootContainer( myItem );
	}
	else				// serial
	{
		SI32 mSerItem	= JSVAL_TO_INT( argv[0] );
		iRoot			= FindRootContainer( calcItemObjFromSer( mSerItem ) );
	}
	if( ValidateObject( iRoot ) )
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, iRoot, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CalcTargetedItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted item stored on socket
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CalcTargetedItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcTargetedItem: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSObject *mysockptr = JSVAL_TO_OBJECT( argv[0] );
	CSocket *sChar = static_cast<CSocket *>(JS_GetPrivate( cx, mysockptr ));
	if( sChar == nullptr )
	{
		DoSEErrorMessage( "CalcTargetedItem: Invalid socket" );
		return JS_FALSE;
	}

	CItem *calcedItem = calcItemObjFromSer( sChar->GetDWord( 7 ) );
	if( !ValidateObject( calcedItem ) )
		*rval = JSVAL_NULL;
	else
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, calcedItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CalcTargetedChar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted character stored on socket
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CalcTargetedChar( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CalcTargetedChar: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSObject *mysockptr = JSVAL_TO_OBJECT( argv[0] );
	CSocket *sChar		= static_cast<CSocket *>(JS_GetPrivate( cx, mysockptr ));
	if( sChar == nullptr )
	{
		DoSEErrorMessage( "CalcTargetedChar: Invalid socket" );
		return JS_FALSE;
	}

	CChar *calcedChar = calcCharObjFromSer( sChar->GetDWord( 7 ) );
	if( !ValidateObject( calcedChar ) )
		*rval = JSVAL_NULL;
	else
	{
		JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, calcedChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetTileIDAtMapCoord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the map tile ID at given coordinates
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetTileIDAtMapCoord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "GetTileIDAtMapCoord: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	UI16 xLoc			= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc			= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber		= (UI08)JSVAL_TO_INT( argv[2] );
	const map_st mMap	= Map->SeekMap( xLoc, yLoc, wrldNumber );
	*rval				= INT_TO_JSVAL( mMap.id );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_StaticInRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	17th August, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static within specified range of given location
//o-----------------------------------------------------------------------------------------------o
JSBool SE_StaticInRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "StaticInRange: Invalid number of arguments (takes 5, x, y, world, radius, tile)" );
		return JS_FALSE;
	}

	UI16 xLoc		= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc		= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber = (UI08)JSVAL_TO_INT( argv[2] );
	UI16 radius		= (UI16)JSVAL_TO_INT( argv[3] );
	UI16 tileID		= (UI16)JSVAL_TO_INT( argv[4] );
	bool tileFound	= false;

	for( SI32 i = xLoc - radius; i <= (xLoc + radius); ++i )
	{
		for( SI32 j = yLoc - radius; j <= (yLoc + radius); ++j )
		{
			CStaticIterator msi( xLoc, yLoc, wrldNumber );
			for( Static_st *mRec = msi.First(); mRec != nullptr; mRec = msi.Next() )
			{
				if( mRec != nullptr && mRec->itemid == tileID )
				{
					tileFound = true;
					break;
				}
			}
		}
	}

	*rval			= BOOLEAN_TO_JSVAL( tileFound );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_StaticAt( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	17th August, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static at specified location
//|	Notes		-	tile argument is optional; if not specified, will match ANY static found at location
//o-----------------------------------------------------------------------------------------------o
JSBool SE_StaticAt( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 && argc != 3 )
	{
		DoSEErrorMessage( "StaticAt: Invalid number of arguments (takes 4, x, y, world, tile)" );
		return JS_FALSE;
	}

	UI16 xLoc		= (UI16)JSVAL_TO_INT( argv[0] );
	UI16 yLoc		= (UI16)JSVAL_TO_INT( argv[1] );
	UI08 wrldNumber = (UI08)JSVAL_TO_INT( argv[2] );
	UI16 tileID		= 0xFFFF;
	bool tileMatch	= false;
	if( argc == 4 )
	{
		tileID		= (UI16)JSVAL_TO_INT( argv[3] );
		tileMatch	= true;
	}
	bool tileFound	= false;

	CStaticIterator msi( xLoc, yLoc, wrldNumber );
	for( Static_st *mRec = msi.First(); mRec != nullptr; mRec = msi.Next() )
	{
		if( mRec != nullptr && (!tileMatch || mRec->itemid==tileID ) )
		{
			tileFound = true;
			break;
		}
	}
	*rval			= BOOLEAN_TO_JSVAL( tileFound );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_StringToNum( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	27th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts string to number
//o-----------------------------------------------------------------------------------------------o
JSBool SE_StringToNum( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "StringToNum: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	std::string str = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );

	*rval = INT_TO_JSVAL( std::stoi(str, nullptr, 0) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_NumToString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	27th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts number to string
//o-----------------------------------------------------------------------------------------------o
JSBool SE_NumToString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "NumToString: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SI32 num = JSVAL_TO_INT( argv[0] );
	auto str = strutil::number( num );
	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, str.c_str() ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_NumToHexString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	27th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts number to hex string
//o-----------------------------------------------------------------------------------------------o
JSBool SE_NumToHexString( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "NumToHexString: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SI32 num = JSVAL_TO_INT( argv[0] );
	auto str = strutil::number( num, 16 );

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, str.c_str() ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetRaceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	November 9, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the total number of races found in the server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetRaceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		DoSEErrorMessage( "GetRaceCount: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( Races->Count() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_AreaCharacterFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	January 27, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area of characters
//o-----------------------------------------------------------------------------------------------o
JSBool SE_AreaCharacterFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		// function name, source character, range
		DoSEErrorMessage( "AreaCharacterFunction: Invalid number of arguments (takes 3/4, function name, source character, range, optional socket)" );
		return JS_FALSE;
	}

	// Do parameter validation here
	JSObject *srcSocketObj		= nullptr;
	CSocket *srcSocket			= nullptr;
	char *trgFunc				= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgFunc == nullptr )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 0 not a valid string" );
		return JS_FALSE;
	}

	JSObject *srcBaseObj	= JSVAL_TO_OBJECT( argv[1] );
	CBaseObject *srcObject		= static_cast<CBaseObject *>(JS_GetPrivate( cx, srcBaseObj ));

	if( !ValidateObject( srcObject ) )
	{
		DoSEErrorMessage( "AreaCharacterFunction: Argument 1 not a valid object" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		srcSocket		= static_cast<CSocket *>(JS_GetPrivate( cx, srcSocketObj ));
	}

	UI16 retCounter				= 0;
	cScript *myScript			= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	REGIONLIST nearbyRegions	= MapRegion->PopulateList( srcObject );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == nullptr )	// no valid region
			continue;
		GenericList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) )
				continue;
			if( objInRange( srcObject, tempChar, (UI16)distance ) )
			{
				if( myScript->AreaObjFunc( trgFunc, srcObject, tempChar, srcSocket ) )
					++retCounter;
			}
		}
		regChars->Pop();
	}
	*rval = INT_TO_JSVAL( retCounter );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_AreaItemFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	17th August, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area of items
//o-----------------------------------------------------------------------------------------------o
JSBool SE_AreaItemFunction( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 && argc != 4 )
	{
		// function name, source character, range
		DoSEErrorMessage( "AreaItemFunction: Invalid number of arguments (takes 3/4, function name, source character, range, optional socket)" );
		return JS_FALSE;
	}

	// Do parameter validation here
	JSObject *srcSocketObj		= nullptr;
	CSocket *srcSocket			= nullptr;
	char *trgFunc				= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( trgFunc == nullptr )
	{
		DoSEErrorMessage( "AreaItemFunction: Argument 0 not a valid string" );
		return JS_FALSE;
	}

	JSObject *srcBaseObj	= JSVAL_TO_OBJECT( argv[1] );
	CBaseObject *srcObject		= static_cast<CBaseObject *>(JS_GetPrivate( cx, srcBaseObj ));

	if( !ValidateObject( srcObject ) )
	{
		DoSEErrorMessage( "AreaItemFunction: Argument 1 not a valid object" );
		return JS_FALSE;
	}
	R32 distance = static_cast<R32>(JSVAL_TO_INT( argv[2] ));
	if( argc == 4 )
	{
		srcSocketObj	= JSVAL_TO_OBJECT( argv[3] );
		if( srcSocketObj != nullptr )
		{
			srcSocket		= static_cast<CSocket *>(JS_GetPrivate( cx, srcSocketObj ));
		}
		else
		{
			DoSEErrorMessage( "AreaItemFunction: Argument 3 not a valid socket" );
		}
	}

	UI16 retCounter					= 0;
	cScript *myScript				= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	REGIONLIST nearbyRegions		= MapRegion->PopulateList( srcObject );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == nullptr )	// no valid region
			continue;
		GenericList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
		{
			if( !ValidateObject( tempItem ) )
				continue;
			if( objInRange( srcObject, tempItem, (UI16)distance ) )
			{
				if( myScript->AreaObjFunc( trgFunc, srcObject, tempItem, srcSocket ) )
					++retCounter;
			}
		}
		regItems->Pop();
	}
	*rval = INT_TO_JSVAL( retCounter );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetDictionaryEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	7/17/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows the JSScripts to pull entries from the dictionaries and convert them to a string.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetDictionaryEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 )
	{
		DoSEErrorMessage( "GetDictionaryEntry: Invalid number of arguments (takes at least 1)" );
		return JS_FALSE;
	}

	SI32 dictEntry = (SI32)JSVAL_TO_INT( argv[0] );
	UnicodeTypes language = ZERO;
	if( argc == 2 )
		language = (UnicodeTypes)JSVAL_TO_INT( argv[1] );
	std::string txt = Dictionary->GetEntry( dictEntry, language );

	JSString *strTxt = nullptr;
	strTxt = JS_NewStringCopyZ( cx, txt.c_str() );
	*rval = STRING_TO_JSVAL( strTxt );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_Yell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	7/17/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Globally yell a message from JS (Based on Commandlevel)
//o-----------------------------------------------------------------------------------------------o
JSBool SE_Yell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 3 )
	{
		DoSEErrorMessage( "Yell: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	JSObject *mSock			= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock			= static_cast<CSocket *>(JS_GetPrivate( cx, mSock ));
	CChar *myChar			= mySock->CurrcharObj();
	std::string textToYell	= JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	UI08 commandLevel		= (UI08)JSVAL_TO_INT( argv[2] );

	std::string yellTo = "";
	switch( (CommandLevels)commandLevel )
	{
		case CL_PLAYER:			yellTo = " (GLOBAL YELL): ";	break;
		case CL_CNS:			yellTo = " (CNS YELL): ";		break;
		case CL_GM:				yellTo = " (GM YELL): ";		break;
		case CL_ADMIN:			yellTo = " (ADMIN YELL): ";		break;
	}

	std::string tmpString = myChar->GetName() + yellTo + textToYell;

	CSpeechEntry& toAdd = SpeechSys->Add();
	toAdd.Speech( tmpString );
	toAdd.Font( (FontType)myChar->GetFontType() );
	toAdd.Speaker( INVALIDSERIAL );
	if( mySock->GetWord( 4 ) == 0x1700 )
	{
		toAdd.Colour( 0x5A );
	}
	else if( mySock->GetWord( 4 ) == 0x0 )
	{
		toAdd.Colour( 0x5A );
	}
	else
	{
		toAdd.Colour( mySock->GetWord( 4 ) );
	}
	toAdd.Type( SYSTEM );
	toAdd.At( cwmWorldState->GetUICurrentTime() );
	toAdd.TargType( SPTRG_BROADCASTALL );
	toAdd.CmdLevel( commandLevel );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_Reload( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	29 Dec 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads certain server subsystems
//o-----------------------------------------------------------------------------------------------o
JSBool SE_Reload( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "Reload: needs 1 argument!" );
		return JS_FALSE;
	}

	SI32 toLoad = JSVAL_TO_INT( argv[0] );

	switch( toLoad )
	{
		case 0:	// Reload regions
			UnloadRegions();
			LoadRegions();
			LoadTeleportLocations();
			break;
		case 1:	// Reload spawn regions
			UnloadSpawnRegions();
			LoadSpawnRegions();
			break;
		case 2:	// Reload Spells
			Magic->LoadScript();
			break;
		case 3: // Reload Commands
			Commands->Load();
			break;
		case 4:	// Reload DFNs
			FileLookup->Reload();
			LoadSkills();
			Skills->Load();
			break;
		case 5: // Reload JScripts
			messageLoop << MSG_RELOADJS;
			break;
		case 6: // Reload HTMLTemplates
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			break;
		case 7:	// Reload INI
			cwmWorldState->ServerData()->Load();
			break;
		case 8: // Reload Everything
			FileLookup->Reload();
			UnloadRegions();
			LoadRegions();
			UnloadSpawnRegions();
			LoadSpawnRegions();
			Magic->LoadScript();
			Commands->Load();
			LoadSkills();
			Skills->Load();
			messageLoop << MSG_RELOADJS;
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			cwmWorldState->ServerData()->Load();
			break;
		case 9: // Reload Accounts
			Accounts->Load();
			break;
		default:
			break;
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_SendStaticStats( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	25th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Builds an info gump for specified static or map tile
//o-----------------------------------------------------------------------------------------------o
JSBool SE_SendStaticStats( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "SendStaticStats: needs 1 argument!" );
		return JS_FALSE;
	}

	JSObject *mSock			= JSVAL_TO_OBJECT( argv[0] );
	CSocket *mySock			= static_cast<CSocket *>(JS_GetPrivate( cx, mSock ));
	if( mySock == nullptr )
	{
		DoSEErrorMessage( "SendStaticStats: passed an invalid socket!" );
		return JS_FALSE;
	}
	CChar *myChar			= mySock->CurrcharObj();
	if( !ValidateObject( myChar ) )
		return JS_TRUE;

	if( mySock->GetDWord( 7 ) == 0 )
	{
		UI08 worldNumber	= myChar->WorldNumber();
		UI16 targetID		= mySock->GetWord( 0x11 );
		SI16 targetX		= mySock->GetWord( 0x0B );		// store our target x y and z locations
		SI16 targetY		= mySock->GetWord( 0x0D );
		SI08 targetZ		= mySock->GetByte( 0x10 );
		if( targetID != 0 )	// we might have a static rock or mountain
		{
			CStaticIterator msi( targetX, targetY, worldNumber );
			CMulHandler tileXTemp;
			for( Static_st *stat = msi.First(); stat != nullptr; stat = msi.Next() )
			{
				CTile& tile = Map->SeekTile( stat->itemid );
				if( targetZ == stat->zoff )
				{
					GumpDisplay staticStat( mySock, 300, 300 );
					staticStat.SetTitle( "Item [Static]" );
					staticStat.AddData( "ID", targetID, 5 );
					staticStat.AddData( "Height", tile.Height() );
					staticStat.AddData( "Name", tile.Name() );
					staticStat.Send( 4, false, INVALIDSERIAL );
				}
			}
		}
		else		// or it could be a map only
		{
			// manually calculating the ID's if a maptype
			const map_st map1 = Map->SeekMap( targetX, targetY, worldNumber );
			GumpDisplay mapStat( mySock, 300, 300 );
			mapStat.SetTitle( "Item [Map]" );
			mapStat.AddData( "ID", targetID, 5 );
			CLand& land = Map->SeekLand( map1.id );
			mapStat.AddData( "Name", land.Name() );
			mapStat.Send( 4, false, INVALIDSERIAL );
		}
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetTileHeight( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the tile height of a specified tile (item)
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetTileHeight( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetTileHeight: needs 1 argument!" );
		return JS_FALSE;
	}

	UI16 tileNum = (UI16)JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( Map->TileHeight( tileNum ) );
	return JS_TRUE;
}

bool SE_IterateFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	cScript *myScript = static_cast<cScript *>(extraData);
	return myScript->OnIterate( a, b );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_IterateOver( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	July 25th, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all world objects
//o-----------------------------------------------------------------------------------------------o
JSBool SE_IterateOver( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "IterateOver: needs 1 argument!" );
		return JS_FALSE;
	}

	UI32 b				= 0;
	std::string objType = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	ObjectType toCheck	= FindObjTypeFromString( objType );
	cScript *myScript	= JSMapping->GetScript( JS_GetGlobalObject( cx ) );
	if( myScript != nullptr )
		ObjectFactory::getSingleton().IterateOver( toCheck, b, myScript, &SE_IterateFunctor );

	JS_MaybeGC( cx );

	*rval = INT_TO_JSVAL( b );
	return JS_TRUE;
}

bool SE_IterateSpawnRegionsFunctor( CSpawnRegion *a, UI32 &b, void *extraData )
{
	cScript *myScript = static_cast<cScript *>( extraData );
	return myScript->OnIterateSpawnRegions( a, b );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_IterateOverSpawnRegions( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	July, 2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops over all spawn regions in the world
//o-----------------------------------------------------------------------------------------------o
JSBool SE_IterateOverSpawnRegions( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI32 b = 0;
	cScript *myScript = JSMapping->GetScript( JS_GetGlobalObject( cx ) );

	if( myScript != nullptr )
	{
		SPAWNMAP_CITERATOR spIter = cwmWorldState->spawnRegions.begin();
		SPAWNMAP_CITERATOR spEnd = cwmWorldState->spawnRegions.end();
		while( spIter != spEnd )
		{
			CSpawnRegion *spawnReg = spIter->second;
			if( spawnReg != nullptr )
			{
				SE_IterateSpawnRegionsFunctor( spawnReg, b, myScript );
			}
			++spIter;
		}
	}

	JS_MaybeGC( cx );

	*rval = INT_TO_JSVAL( b );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_WorldBrightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world bright level - brightest part of the regular day/night cycle
//o-----------------------------------------------------------------------------------------------o
JSBool SE_WorldBrightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage(strutil::format( "WorldBrightLevel: Unknown Count of Arguments: %d", argc) );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL brightLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->WorldLightBrightLevel( brightLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->WorldLightBrightLevel() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_WorldDarkLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world dark level - darkest part of the regular day/night cycle
//o-----------------------------------------------------------------------------------------------o
JSBool SE_WorldDarkLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( strutil::format("WorldDarkLevel: Unknown Count of Arguments: %d", argc) );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL darkLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->WorldLightDarkLevel( darkLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->WorldLightDarkLevel() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_WorldDungeonLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th July, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets default light level in dungeons
//o-----------------------------------------------------------------------------------------------o
JSBool SE_WorldDungeonLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		DoSEErrorMessage( strutil::format("WorldDungeonLevel: Unknown Count of Arguments: %d", argc) );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL dungeonLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->DungeonLightLevel( dungeonLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->DungeonLightLevel() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetSocketFromIndex( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	3rd August, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns socket based on provided index, from list of connected clients
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetSocketFromIndex( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetSocketFromIndex: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXSOCKET index = (UOXSOCKET)JSVAL_TO_INT( argv[0] );

	CSocket *mSock	= Network->GetSockPtr( index );
	CChar *mChar	= nullptr;
	if( mSock != nullptr )
		mChar = mSock->CurrcharObj();

	if( !ValidateObject( mChar ) )
	{
		*rval = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, mChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
	*rval = OBJECT_TO_JSVAL( myObj );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ReloadJSFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	5th February, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload specified JS file by scriptID
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ReloadJSFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ReloadJSFile: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UI16 scriptID			= static_cast<UI16>(JSVAL_TO_INT( argv[0] ));
	if( scriptID == JSMapping->GetScriptID( JS_GetGlobalObject( cx ) ) )
	{
		DoSEErrorMessage( strutil::format("ReloadJSFile: JS Script attempted to reload itself, crash avoided (ScriptID %u)", scriptID) );
		return JS_FALSE;
	}

	JSMapping->Reload( scriptID );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ResourceArea( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th September, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resource areas to split the world into
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ResourceArea( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( strutil::format("ResourceArea: Invalid Count of Arguments: %d", argc) );
		return JS_FALSE;
	}

	auto resType = std::string( JS_GetStringBytes( JS_ValueToString( cx, argv[0] )) );
	resType = strutil::toupper( strutil::stripTrim( resType ));
	if( argc == 2 )
	{
		UI16 newVal = static_cast<UI16>(JSVAL_TO_INT( argv[1] ));
		if( resType == "LOGS" ) // Logs
		{
			cwmWorldState->ServerData()->ResLogArea( newVal );
		}
		else if( resType == "ORE" ) // Ore
		{
			cwmWorldState->ServerData()->ResOreArea( newVal );
		}
	}

	if( resType == "LOGS" )
	{
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResLogArea() );
	}
	else if( resType == "ORE" )
	{
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResOreArea() );
	}

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ResourceAmount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th September, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resources (logs/ore) in each resource area on the server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ResourceAmount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( strutil::format("ResourceAmount: Invalid Count of Arguments: %d", argc) );
		return JS_FALSE;
	}
	auto resType = std::string( JS_GetStringBytes( JS_ValueToString( cx, argv[0] )) );
	resType = strutil::toupper( strutil::stripTrim( resType ));

	if( argc == 2 )
	{
		SI16 newVal = static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
		if( resType == "LOGS" )
		{
			cwmWorldState->ServerData()->ResLogs( newVal );
		}
		else if( resType == "ORE" )
		{
			cwmWorldState->ServerData()->ResOre( newVal );
		}
	}

	if( resType == "LOGS" )
	{
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResLogs() );
	}
	else if( resType == "ORE" )
	{
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResOre() );
	}

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ResourceTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th September, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets respawn timers for ore/log resources on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ResourceTime( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( strutil::format("ResourceTime: Invalid Count of Arguments: %d", argc) );
		return JS_FALSE;
	}

	auto resType = std::string( JS_GetStringBytes( JS_ValueToString( cx, argv[0] )) );
	resType = strutil::toupper( strutil::stripTrim( resType ));
	if( argc == 2 )
	{
		UI16 newVal = static_cast<UI16>(JSVAL_TO_INT( argv[1] ));
		if( resType == "LOGS" )
		{
			cwmWorldState->ServerData()->ResLogTime( newVal );
		}
		else if( resType == "ORE" )
		{
			cwmWorldState->ServerData()->ResOreTime( newVal );
		}
	}

	if( resType == "LOGS" )
	{
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResLogTime() );
	}
	else if( resType == "ORE" )
	{
		*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ResOreTime() );
	}

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ResourceRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	18th September, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a resource object allowing JS to modify resource data.
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ResourceRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "ResourceRegion: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[2] ));
	MapResource_st *mRes = MapRegion->GetResource( x, y, worldNum );
	if( mRes == nullptr )
	{
		DoSEErrorMessage( "ResourceRegion: Invalid Resource Region" );
		return JS_FALSE;
	}

	JSObject *jsResource = JS_NewObject( cx, &UOXResource_class, nullptr, obj );
	JS_DefineProperties( cx, jsResource, CResourceProperties );
	JS_SetPrivate( cx, jsResource, mRes );

	*rval = OBJECT_TO_JSVAL( jsResource );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ValidateObject( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	26th January, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if object is a valid and not nullptr or marked for deletion
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ValidateObject( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "ValidateObject: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, &(argv[0]) );

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" )
	{
		CBaseObject *myObj	= static_cast<CBaseObject *>(myClass.toObject());
		*rval				= BOOLEAN_TO_JSVAL( ValidateObject( myObj ) );
	}
	else
		*rval = JSVAL_FALSE;

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ApplyDamageBonuses( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	17th March, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage bonuses based on race/weather weakness and character skills
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ApplyDamageBonuses( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}

	CChar *attacker	= nullptr, *defender = nullptr;
	SI16 damage = 0;

	JSEncapsulate damageType( cx, &( argv[0] ) );
	JSEncapsulate getFightSkill( cx, &( argv[3] ) );
	JSEncapsulate hitLoc( cx, &( argv[4] ) );
	JSEncapsulate baseDamage( cx, &(argv[5]) );

	JSEncapsulate attackerClass( cx, &(argv[1]) );
	if( attackerClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_FALSE;
	}

	if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ) )
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_TRUE;
	}
	else
	{
		attacker	= static_cast<CChar *>(attackerClass.toObject());
		if( !ValidateObject( attacker )  )
		{
			DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
			return JS_TRUE;
		}
	}

	JSEncapsulate defenderClass( cx, &(argv[2]) );
	if( defenderClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_FALSE;
	}

	if( defenderClass.isType( JSOT_VOID ) || defenderClass.isType( JSOT_NULL ) )
	{
		DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
		return JS_TRUE;
	}
	else
	{
		defender	= static_cast<CChar *>(defenderClass.toObject());
		if( !ValidateObject( defender )  )
		{
			DoSEErrorMessage( "ApplyDamageBonuses: Passed an invalid Character" );
			return JS_TRUE;
		}
	}

	damage	= Combat->ApplyDamageBonuses( (WeatherType)damageType.toInt(), attacker, defender, (UI08)getFightSkill.toInt(), (UI08)hitLoc.toInt(), (SI16)baseDamage.toInt() );
	*rval	= INT_TO_JSVAL( damage );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_ApplyDefenseModifiers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	17th March, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns defense modifiers based on shields/parrying, armor values and elemental damage
//o-----------------------------------------------------------------------------------------------o
JSBool SE_ApplyDefenseModifiers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		DoSEErrorMessage( "ApplyDefenseModifiers: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	CChar *attacker	= nullptr, *defender = nullptr;
	SI16 damage = 0;

	JSEncapsulate damageType( cx, &( argv[0] ) );
	JSEncapsulate getFightSkill( cx, &( argv[3] ) );
	JSEncapsulate hitLoc( cx, &( argv[4] ) );
	JSEncapsulate baseDamage( cx, &(argv[5]) );
	JSEncapsulate doArmorDamage(cx, &( argv[6] ) );

	JSEncapsulate attackerClass( cx, &(argv[1]) );
	if( attackerClass.ClassName() == "UOXChar" )
	{
		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ) )
		{
			attacker = nullptr;
		}
		else
		{
			attacker	= static_cast<CChar *>(attackerClass.toObject());
			if( !ValidateObject( attacker )  )
			{
				attacker = nullptr;
			}
		}
	}

	JSEncapsulate defenderClass( cx, &(argv[2]) );
	if( defenderClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		DoSEErrorMessage( "ApplyDefenseModifiers: Passed an invalid Character" );
		return JS_FALSE;
	}

	if( defenderClass.isType( JSOT_VOID ) || defenderClass.isType( JSOT_NULL ) )
	{
		DoSEErrorMessage( "ApplyDefenseModifiers: Passed an invalid Character" );
		return JS_TRUE;
	}
	else
	{
		defender	= static_cast<CChar *>(defenderClass.toObject());
		if( !ValidateObject( defender )  )
		{
			DoSEErrorMessage( "ApplyDefenseModifiers: Passed an invalid Character" );
			return JS_TRUE;
		}
	}

	damage	= Combat->ApplyDefenseModifiers( (WeatherType)damageType.toInt(), attacker, defender, (UI08)getFightSkill.toInt(), (UI08)hitLoc.toInt(), (SI16)baseDamage.toInt(), doArmorDamage.toBool() );
	*rval	= INT_TO_JSVAL( damage );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CreateParty( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	21st September, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a party with specified character as the party leader
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CreateParty( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "CreateParty: Invalid number of arguments (takes 1, the leader)" );
		return JS_FALSE;
	}

	JSEncapsulate myClass( cx, &(argv[0]) );

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXSocket" )
	{	// it's a character or socket, fantastic
		CChar *leader		= nullptr;
		CSocket *leaderSock	= nullptr;
		if( myClass.ClassName() == "UOXChar" )
		{
			leader		= static_cast<CChar *>(myClass.toObject());
			leaderSock	= leader->GetSocket();
		}
		else
		{
			leaderSock	= static_cast<CSocket *>(myClass.toObject());
			leader		= leaderSock->CurrcharObj();
		}

		if( PartyFactory::getSingleton().Get( leader ) != nullptr )
		{
			*rval = JSVAL_NULL;
		}
		else
		{
			Party *tParty	= PartyFactory::getSingleton().Create( leader );
			JSObject *myObj	= JSEngine->AcquireObject( IUE_PARTY, tParty, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
			*rval			= OBJECT_TO_JSVAL( myObj );
		}
	}
	else	// anything else isn't a valid leader people
		*rval = JSVAL_NULL;

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_Moon( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	25th May, 2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets or sets Moon phases on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_Moon( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 2 || argc == 0 )
	{
		DoSEErrorMessage( strutil::format("Moon: Invalid Count of Arguments: %d", argc) );
		return JS_FALSE;
	}

	SI16 slot = static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	if( argc == 2 )
	{
		SI16 newVal = static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
		cwmWorldState->ServerData()->ServerMoon( slot, newVal );
	}

	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->ServerMoon( slot ) );

	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetTownRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified region object
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetTownRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetRegion: Invalid number of parameters (1)" );
		return JS_FALSE;
	}

	UI16 regNum = (UI16)JSVAL_TO_INT( argv[0] );
	if( cwmWorldState->townRegions.find( regNum ) != cwmWorldState->townRegions.end() )
	{
		CTownRegion *townReg = cwmWorldState->townRegions[regNum];
		if( townReg != nullptr )
		{
			JSObject *myObj = JSEngine->AcquireObject( IUE_REGION, townReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
			*rval = OBJECT_TO_JSVAL( myObj );
		}
		else
		{
			*rval = JSVAL_NULL;
		}
	}
	else
	{
		*rval = JSVAL_NULL;
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetSpawnRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	June 22, 2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified spawn region object
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetSpawnRegion( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetSpawnRegion: Invalid number of parameters (1)" );
		return JS_FALSE;
	}

	UI16 spawnRegNum = (UI16)JSVAL_TO_INT( argv[0] );
	if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
	{
		CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
		if( spawnReg != nullptr )
		{
			JSObject *myObj = JSEngine->AcquireObject( IUE_SPAWNREGION, spawnReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx ) ) );
			*rval = OBJECT_TO_JSVAL( myObj );
		}
		else
		{
			*rval = JSVAL_NULL;
		}
	}
	else
	{
		*rval = JSVAL_NULL;
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetSpawnRegionCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date		-	June 22, 2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the total number of spawn regions found in the server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetSpawnRegionCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		DoSEErrorMessage( "GetSpawnRegionCount: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	*rval = INT_TO_JSVAL( cwmWorldState->spawnRegions.size() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetMapElevation( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map elevation at given coordinates
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetMapElevation( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		DoSEErrorMessage( "GetMapElevation: Invalid number of arguments (takes 3: X, Y and WorldNumber)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[2] ));
	SI08 mapElevation = Map->MapElevation( x, y, worldNum );
	*rval = INT_TO_JSVAL( mapElevation );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_IsInBuilding( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified location is inside a building
//| Notes		-	First checks if player is in a static building. If false, checks if there's a multi
//|					at the same location as the player, and assumes they are in the building if true
//o-----------------------------------------------------------------------------------------------o
JSBool SE_IsInBuilding( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "IsInBuilding: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber and instanceID)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	SI08 z			= static_cast<SI08>(JSVAL_TO_INT( argv[2] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[3] ));
	UI08 instanceID = static_cast<UI08>(JSVAL_TO_INT( argv[4] ));
	bool checkHeight = ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool isInBuilding = Map->inBuilding( x, y, z, worldNum, instanceID );
	if( !isInBuilding )
	{
		// No static building was detected. How about a multi?
		CMultiObj *multi = findMulti( x, y, z, worldNum, instanceID );
		if( ValidateObject( multi ) )
		{
			if( checkHeight )
			{
				// Check if there's multi-items over the player's head
				SI08 multiZ = Map->MultiHeight( multi, x, y, z, 127, checkHeight );
				if( multiZ > z )
					isInBuilding = true;
			}
			else
				isInBuilding = true;
		}
	}
	*rval = BOOLEAN_TO_JSVAL( isInBuilding );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CheckStaticFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CheckStaticFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "CheckStaticFlag: Invalid number of arguments (takes 5: X, Y, Z, WorldNumber and TileFlagID)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 z			= static_cast<SI16>(JSVAL_TO_INT( argv[2] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[3] ));
	TileFlags toCheck	= static_cast<TileFlags>(JSVAL_TO_INT( argv[4] ));
	bool hasStaticFlag = Map->CheckStaticFlag( x, y, z, worldNum, toCheck, false );
	*rval = BOOLEAN_TO_JSVAL( hasStaticFlag );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CheckDynamicFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a specific flag
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CheckDynamicFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "CheckDynamicFlag: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber, instanceID and TileFlagID)" );
		return JS_FALSE;
	}

	SI16 x = static_cast<SI16>( JSVAL_TO_INT( argv[0] ) );
	SI16 y = static_cast<SI16>( JSVAL_TO_INT( argv[1] ) );
	UI08 z = static_cast<SI16>( JSVAL_TO_INT( argv[2] ) );
	UI08 worldNum = static_cast<UI08>( JSVAL_TO_INT( argv[3] ) );
	UI08 instanceID = static_cast<UI08>( JSVAL_TO_INT( argv[4] ) );
	TileFlags toCheck = static_cast<TileFlags>( JSVAL_TO_INT( argv[5] ) );
	bool hasDynamicFlag = Map->CheckDynamicFlag( x, y, z, worldNum, instanceID, toCheck );
	*rval = BOOLEAN_TO_JSVAL( hasDynamicFlag );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_CheckTileFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether tile with given ID has a specific flag
//o-----------------------------------------------------------------------------------------------o
JSBool SE_CheckTileFlag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		DoSEErrorMessage( "CheckTileFlag: Invalid number of arguments (takes 2: itemID and tileFlagID)" );
		return JS_FALSE;
	}

	UI16 itemID = (UI16)JSVAL_TO_INT( argv[0] );
	TileFlags flagToCheck	= static_cast<TileFlags>(JSVAL_TO_INT( argv[1] ));

	bool tileHasFlag = Map->CheckTileFlag( itemID, flagToCheck );
	*rval = BOOLEAN_TO_JSVAL( tileHasFlag );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DoesStaticBlock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DoesStaticBlock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		DoSEErrorMessage( "DoesStaticBlock: Invalid number of arguments (takes 5: X, Y, Z, WorldNumber and checkWater)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 z			= static_cast<SI16>(JSVAL_TO_INT( argv[2] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[3] ));
	bool checkWater = ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );
	bool staticBlocks = Map->DoesStaticBlock( x, y, z, worldNum, checkWater );
	*rval = BOOLEAN_TO_JSVAL( staticBlocks );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DoesDynamicBlock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if dynamics at/above given coordinates blocks movement, etc
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DoesDynamicBlock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		DoSEErrorMessage( "DoesDynamicBlock: Invalid number of arguments (takes 8: X, Y, Z, WorldNumber, instanceID, checkWater, waterWalk, checkOnlyMultis and checkOnlyNonMultis)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 z			= static_cast<SI16>(JSVAL_TO_INT( argv[2] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[3] ));
	UI08 instanceID	= static_cast<UI08>(JSVAL_TO_INT( argv[4] ));
	bool checkWater = ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool waterWalk = ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );
	bool checkOnlyMultis = ( JSVAL_TO_BOOLEAN( argv[7] ) == JS_TRUE );
	bool checkOnlyNonMultis = ( JSVAL_TO_BOOLEAN( argv[8] ) == JS_TRUE );
	bool dynamicBlocks = Map->DoesDynamicBlock( x, y, z, worldNum, instanceID, checkWater, waterWalk, checkOnlyMultis, checkOnlyNonMultis );
	*rval = BOOLEAN_TO_JSVAL( dynamicBlocks );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DoesMapBlock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if map tile at/above given coordinates blocks movement, etc
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DoesMapBlock( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		DoSEErrorMessage( "DoesMapBlock: Invalid number of arguments (takes 8: X, Y, Z, WorldNumber, checkWater, waterWalk, checkMultiPlacement and checkForRoad)" );
		return JS_FALSE;
	}

	SI16 x			= static_cast<SI16>(JSVAL_TO_INT( argv[0] ));
	SI16 y			= static_cast<SI16>(JSVAL_TO_INT( argv[1] ));
	UI08 z			= static_cast<SI16>(JSVAL_TO_INT( argv[2] ));
	UI08 worldNum	= static_cast<UI08>(JSVAL_TO_INT( argv[3] ));
	bool checkWater = ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );
	bool waterWalk = ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool checkMultiPlacement = ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );
	bool checkForRoad = ( JSVAL_TO_BOOLEAN( argv[7] ) == JS_TRUE );
	bool mapBlocks = Map->DoesMapBlock( x, y, z, worldNum, checkWater, waterWalk, checkMultiPlacement, checkForRoad );
	*rval = BOOLEAN_TO_JSVAL( mapBlocks );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DeleteFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes a file from the server's harddrive...
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DeleteFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 || argc > 3 )
	{
		DoSEErrorMessage( "DeleteFile: Invalid number of arguments (takes 1 to 3 - fileName and (optionally) subFolderName and useScriptDataDir bool)" );
		return JS_FALSE;
	}

	char *fileName = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	char *subFolderName = nullptr;
	if( argc >= 2 )
		subFolderName = JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) );
	bool useScriptDataDir = false;
	if( argc >= 3 )
		useScriptDataDir = ( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE );

	if( strstr( fileName, ".." ) || strstr( fileName, "\\" ) || strstr( fileName, "/" ) )
	{
		DoSEErrorMessage( "DeleteFile: file names may not contain \".\", \"..\", \"\\\", or \"/\"." );
		return JS_FALSE;
	}

	std::string pathString	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED );

	// if subFolderName argument was supplied, use it
	if( subFolderName != nullptr )
	{
		// However, don't allow special characters in the folder name
		if( strstr( subFolderName, ".." ) || strstr( subFolderName, "\\" ) || strstr( subFolderName, "/" ) )
		{
			DoSEErrorMessage( "DeleteFile: folder names may not contain \".\", \"..\", \"\\\", or \"/\"." );
			return JS_FALSE;
		}

		// If script wants to look in script-data folder instead of shared folder, let it
		if( useScriptDataDir )
		{
			pathString = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTDATA );
		}

		// Append subfolder name to path
		pathString.append( subFolderName );

		if( !std::filesystem::exists( pathString ))
		{
			// Return JS_TRUE to allow script to continue running even if file was not found for deletion, but set return value to false
			*rval = false;
			return JS_TRUE;
		}

		pathString += std::filesystem::path::preferred_separator;
	}

	pathString.append( fileName );

	std::filesystem::path filePath = pathString;
	*rval = std::filesystem::remove( filePath );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetServerSetting( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets value of specified server setting
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetServerSetting( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = NULL;

	if( argc != 1 )
	{
		DoSEErrorMessage( "GetServerSetting: Invalid number of arguments (takes 1 - serverSettingName)" );
		return JS_FALSE;
	}

	JSString *tString;
	std::string settingName = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	if( !settingName.empty() )
	{
		auto settingID = cwmWorldState->ServerData()->lookupINIValue( settingName );
		switch( settingID )
		{
			case 1:	 // SERVERNAME[0002]
			{
				std::string tempString = { cwmWorldState->ServerData()->ServerName() };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 2:	 // CONSOLELOG[0003]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerConsoleLog() );
				break;
			case 3:	 // COMMANDPREFIX[0005]
			{
				std::string tempString = { cwmWorldState->ServerData()->ServerCommandPrefix() };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 4:	 // ANNOUNCEWORLDSAVES[0006]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerAnnounceSavesStatus() );
				break;
			case 26: // JOINPARTMSGS[0007]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() );
				break;
			case 5:	 // BACKUPSENABLED[0009]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerBackupStatus() );
				break;
			case 6:	 // SAVESTIMER[0010]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->ServerSavesTimerStatus()));
				break;
			case 7:	 // SKILLCAP[0011]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ServerSkillTotalCapStatus()));
				break;
			case 8:	 // SKILLDELAY[0012]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->ServerSkillDelayStatus()));
				break;
			case 9:	 // STATCAP[0013]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ServerStatCapStatus()));
				break;
			case 10:	 // MAXSTEALTHMOVEMENTS[0014]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->MaxStealthMovement()));
				break;
			case 11:	 // MAXSTAMINAMOVEMENTS[0015]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->MaxStaminaMovement()));
				break;
			case 12:	 // ARMORAFFECTMANAREGEN[0016]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ArmorAffectManaRegen() );
				break;
			case 13:	 // CORPSEDECAYTIMER[0017]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_CORPSEDECAY )));
				break;
			case 14:	 // WEATHERTIMER[0018]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_WEATHER )));
				break;
			case 15:	 // SHOPSPAWNTIMER[0019]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_SHOPSPAWN )));
				break;
			case 16:	 // DECAYTIMER[0020]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_DECAY )));
				break;
			case 17:	 // INVISIBILITYTIMER[0021]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_INVISIBILITY )));
				break;
			case 18:	 // OBJECTUSETIMER[0022]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_OBJECTUSAGE )));
				break;
			case 19:	 // GATETIMER[0023]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_GATE )));
				break;
			case 20:	 // POISONTIMER[0024]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_POISON )));
				break;
			case 21:	 // LOGINTIMEOUT[0025]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_LOGINTIMEOUT )));
				break;
			case 22:	 // HITPOINTREGENTIMER[0026]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_HITPOINTREGEN )));
				break;
			case 23:	 // STAMINAREGENTIMER[0027]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_STAMINAREGEN )));
				break;
			case 37:	 // MANAREGENTIMER[0028]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN )));
				break;
			case 24:	 // BASEFISHINGTIMER[0029]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_FISHINGBASE )));
				break;
			case 38:	 // RANDOMFISHINGTIMER[0030]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_FISHINGRANDOM )));
				break;
			case 39:	 // SPIRITSPEAKTIMER[0031]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_SPIRITSPEAK )));
				break;
			case 40:	 // DIRECTORY[0032]
			{
				JSString *tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_ROOT ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 41:	 // DATADIRECTORY[0033]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_DATA ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 42:	 // DEFSDIRECTORY[0034]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_DEFS ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 43:	 // ACTSDIRECTORY[0035]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 25:	 // SCRIPTSDIRECTORY[0036]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 44:	 // BACKUPDIRECTORY[0037]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 45:	 // MSGBOARDDIRECTORY[0038]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 46:	 // SHAREDDIRECTORY[0039]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_SHARED ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 47:	 // LOOTDECAYSWITHCORPSE[0040]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CorpseLootDecay() );
				break;
			case 49:	 // GUARDSACTIVE[0041]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GuardsStatus() );
				break;
			case 27:	 // DEATHANIMATION[0042]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->DeathAnimationStatus() );
				break;
			case 50:	 // AMBIENTSOUNDS[0043]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->WorldAmbientSounds()));
				break;
			case 51:	 // AMBIENTFOOTSTEPS[0044]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->AmbientFootsteps() );
				break;
			case 52:	 // INTERNALACCOUNTCREATION[0045]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->InternalAccountStatus() );
				break;
			case 53:	 // SHOWOFFLINEPCS[0046]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ShowOfflinePCs() );
				break;
			case 54:	 // ROGUESENABLED[0047]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->RogueStatus() );
				break;
			case 55:	 // PLAYERPERSECUTION[0048]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->PlayerPersecutionStatus() );
				break;
			case 56:	 // ACCOUNTFLUSH[0049]
				*rval = INT_TO_JSVAL( static_cast<R64>(cwmWorldState->ServerData()->AccountFlushTimer()));
				break;
			case 57:	 // HTMLSTATUSENABLED[0050]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->HtmlStatsStatus()));
				break;
			case 58:	 // SELLBYNAME[0051]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->SellByNameStatus() );
				break;
			case 59:	 // SELLMAXITEMS[0052]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->SellMaxItemsStatus()));
				break;
			case 60:	 // TRADESYSTEM[0053]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->TradeSystemStatus() );
				break;
			case 61:	 // RANKSYSTEM[0054]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->RankSystemStatus() );
				break;
			case 62:	 // CUTSCROLLREQUIREMENTS[0055]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CutScrollRequirementStatus() );
				break;
			case 63:	 // CHECKITEMS[0056]
				*rval = INT_TO_JSVAL( static_cast<R64>(cwmWorldState->ServerData()->CheckItemsSpeed()));
				break;
			case 64:	 // CHECKBOATS[0057]
				*rval = INT_TO_JSVAL( static_cast<R64>(cwmWorldState->ServerData()->CheckBoatSpeed()));
				break;
			case 65:	 // CHECKNPCAI[0058]
				*rval = INT_TO_JSVAL( static_cast<R64>(cwmWorldState->ServerData()->CheckNpcAISpeed()));
				break;
			case 66:	 // CHECKSPAWNREGIONS[0059]
				*rval = INT_TO_JSVAL( static_cast<R64>(cwmWorldState->ServerData()->CheckSpawnRegionSpeed()));
				break;
			case 67:	 // POSTINGLEVEL[0060]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->MsgBoardPostingLevel()));
				break;
			case 68:	 // REMOVALLEVEL[0061]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->MsgBoardPostRemovalLevel()));
				break;
			case 69:	 // ESCORTENABLED[0062]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->EscortsEnabled() );
				break;
			case 70:	 // ESCORTINITEXPIRE[0063]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTWAIT )));
				break;
			case 71:	 // ESCORTACTIVEEXPIRE[0064]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTACTIVE )));
				break;
			case 72:	 // MOON1[0065]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->ServerMoon( 0 )));
				break;
			case 73:	 // MOON2[0066]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->ServerMoon( 1 )));
				break;
			case 74:	 // DUNGEONLEVEL[0067]
				*rval = INT_TO_JSVAL( static_cast<LIGHTLEVEL>(cwmWorldState->ServerData()->DungeonLightLevel()));
				break;
			case 75:	 // CURRENTLEVEL[0068]
				*rval = INT_TO_JSVAL( static_cast<LIGHTLEVEL>(cwmWorldState->ServerData()->WorldLightCurrentLevel()));
				break;
			case 76:	 // BRIGHTLEVEL[0069]
				*rval = INT_TO_JSVAL( static_cast<LIGHTLEVEL>(cwmWorldState->ServerData()->WorldLightBrightLevel()));
				break;
			case 77:	 // BASERANGE[0070]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->TrackingBaseRange()));
				break;
			case 78:	 // BASETIMER[0071]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->TrackingBaseTimer()));
				break;
			case 79:	 // MAXTARGETS[0072]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->TrackingMaxTargets()));
				break;
			case 80:	 // MSGREDISPLAYTIME[0073]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->TrackingRedisplayTime()));
				break;
			case 81:	 // MURDERDECAYTIMER[0074]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_MURDERDECAY )));
				break;
			case 82:	 // MAXKILLS[0075]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->RepMaxKills()));
				break;
			case 83:	 // CRIMINALTIMER[0076]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_CRIMINAL )));
				break;
			case 84:	 // MINECHECK[0077]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->MineCheck()));
				break;
			case 85:	 // OREPERAREA[0078]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->ResOre()));
				break;
			case 86:	 // ORERESPAWNTIMER[0079]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ResOreTime()));
				break;
			case 87:	 // ORERESPAWNAREA[0080]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ResOreArea()));
				break;
			case 88:	 // LOGSPERAREA[0081]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->ResLogs()));
				break;
			case 89:	 // LOGSRESPAWNTIMER[0082]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ResLogTime()));
				break;
			case 90:	 // LOGSRESPAWNAREA[0083]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ResLogArea()));
				break;
			case 91:	 // HUNGERRATE[0084]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE )));
				break;
			case 92:	 // HUNGERDMGVAL[0085]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->HungerDamage()));
				break;
			case 93:	 // MAXRANGE[0086]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->CombatMaxRange()));
				break;
			case 94:	 // SPELLMAXRANGE[0087]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->CombatMaxSpellRange()));
				break;
			case 95:	 // DISPLAYHITMSG[0088]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CombatDisplayHitMessage() );
				break;
			case 96:	 // MONSTERSVSANIMALS[0089]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CombatMonstersVsAnimals() );
				break;
			case 97:	 // ANIMALATTACKCHANCE[0090]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->CombatAnimalsAttackChance()));
				break;
			case 98:	 // ANIMALSGUARDED[0091]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CombatAnimalsGuarded() );
				break;
			case 99:	 // NPCDAMAGERATE[0092]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->CombatNPCDamageRate()));
				break;
			case 100:	 // NPCBASEFLEEAT[0093]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->CombatNPCBaseFleeAt()));
				break;
			case 101:	 // NPCBASEREATTACKAT[0094]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->CombatNPCBaseReattackAt()));
				break;
			case 102:	 // ATTACKSTAMINA[0095]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->CombatAttackStamina()));
				break;
			//case 103:	 // LOCATION[0096]
				//break;
			case 104:	 // STARTGOLD[0097]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->ServerStartGold()));
				break;
			case 105:	 // STARTPRIVS[0098]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ServerStartPrivs()));
				break;
			case 106:	 // ESCORTDONEEXPIRE[0099]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTDONE )));
				break;
			case 107:	 // DARKLEVEL[0100]
				*rval = INT_TO_JSVAL( static_cast<LIGHTLEVEL>(cwmWorldState->ServerData()->WorldLightDarkLevel()));
				break;
			case 108:	 // TITLECOLOUR[0101]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->TitleColour()));
				break;
			case 109:	 // LEFTTEXTCOLOUR[0102]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->LeftTextColour()));
				break;
			case 110:	 // RIGHTTEXTCOLOUR[0103]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->RightTextColour()));
				break;
			case 111:	 // BUTTONCANCEL[0104]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ButtonCancel()));
				break;
			case 112:	 // BUTTONLEFT[0105]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ButtonLeft()));
				break;
			case 113:	 // BUTTONRIGHT[0106]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ButtonRight()));
				break;
			case 114:	 // BACKGROUNDPIC[0107]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->BackgroundPic()));
				break;
			case 115:	 // POLLTIME[0108]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->TownNumSecsPollOpen()));
				break;
			case 116:	 // MAYORTIME[0109]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->TownNumSecsAsMayor()));
				break;
			case 117:	 // TAXPERIOD[0110]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->TownTaxPeriod()));
				break;
			case 118:	 // GUARDSPAID[0111]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->TownGuardPayment()));
				break;
			case 119:	 // DAY[0112]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->ServerTimeDay()));
				break;
			case 120:	 // HOURS[0113]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->ServerTimeHours()));
				break;
			case 121:	 // MINUTES[0114]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->ServerTimeMinutes()));
				break;
			case 122:	 // SECONDS[0115]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->ServerTimeSeconds()));
				break;
			case 123:	 // AMPM[0116]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerTimeAMPM() );
				break;
			case 124:	 // SKILLLEVEL[0117]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->SkillLevel()));
				break;
			case 125:	 // SNOOPISCRIME[0118]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->SnoopIsCrime() );
				break;
			case 126:	 // BOOKSDIRECTORY[0119]
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			//case 127:	 // SERVERLIST[0120]
				//break;
			case 128:	 // PORT[0121]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ServerPort()));
				break;
			case 129:	 // ACCESSDIRECTORY[0122]
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_ACCESS ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			case 130:	 // LOGSDIRECTORY[0123]
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_LOGS ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			case 132:	 // HTMLDIRECTORY[0125]
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_HTML ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			case 133:	 // SHOOTONANIMALBACK[0126]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ShootOnAnimalBack() );
				break;
			case 134:	 // NPCTRAININGENABLED[0127]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->NPCTrainingStatus() );
				break;
			case 135:	 // DICTIONARYDIRECTORY[0128]
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_DICTIONARIES ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			case 136:	 // BACKUPSAVERATIO[0129]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->BackupRatio()));
				break;
			case 137:	 // HIDEWILEMOUNTED[0130]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CharHideWhileMounted() );
				break;
			case 138:	 // SECONDSPERUOMINUTE[0131]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->ServerSecondsPerUOMinute()));
				break;
			case 139:	 // WEIGHTPERSTR[0132]
				*rval = INT_TO_JSVAL( static_cast<R32>(cwmWorldState->ServerData()->WeightPerStr()));
				break;
			case 140:	 // POLYDURATION[0133]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( tSERVER_POLYMORPH )));
				break;
			case 141:	 // UOGENABLED[0134]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerUOGEnabled() );
				break;
			case 142:	 // NETRCVTIMEOUT[0135]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->ServerNetRcvTimeout()));
				break;
			case 143:	 // NETSNDTIMEOUT[0136]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->ServerNetSndTimeout()));
				break;
			case 144:	 // NETRETRYCOUNT[0137]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->ServerNetRetryCount()));
				break;
			case 145:	 // CLIENTFEATURES[0138]
				*rval = INT_TO_JSVAL( static_cast<UI32>(cwmWorldState->ServerData()->GetClientFeatures()));
				break;
			case 146:	 // PACKETOVERLOADS[0139]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerOverloadPackets() );
				break;
			case 147:	 // NPCMOVEMENTSPEED[0140]
				*rval = INT_TO_JSVAL( static_cast<R32>(cwmWorldState->ServerData()->NPCWalkingSpeed()));
				break;
			case 148:	 // PETHUNGEROFFLINE[0141]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->PetHungerOffline() );
				break;
			case 149:	 // PETOFFLINETIMEOUT[0142]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->PetOfflineTimeout()));
				break;
			case 150:	 // PETOFFLINECHECKTIMER[0143]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_PETOFFLINECHECK ))));
				break;
			case 152:	 // ADVANCEDPATHFINDING[0145]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->AdvancedPathfinding() );
				break;
			case 153:	 // SERVERFEATURES[0146]
				*rval = INT_TO_JSVAL( static_cast<size_t>(cwmWorldState->ServerData()->GetServerFeatures()));
				break;
			case 154:	 // LOOTINGISCRIME[0147]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->LootingIsCrime() );
				break;
			case 155:	 // NPCRUNNINGSPEED[0148]
				*rval = INT_TO_JSVAL( static_cast<R32>(cwmWorldState->ServerData()->NPCRunningSpeed()));
				break;
			case 156:	 // NPCFLEEINGSPEED[0149]
				*rval = INT_TO_JSVAL( static_cast<R32>(cwmWorldState->ServerData()->NPCFleeingSpeed()));
				break;
			case 157:	 // BASICTOOLTIPSONLY[0150]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->BasicTooltipsOnly() );
				break;
			case 158:	 // GLOBALITEMDECAY[0151]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GlobalItemDecay() );
				break;
			case 159:	 // SCRIPTITEMSDECAYABLE[0152]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ScriptItemsDecayable() );
				break;
			case 160:	 // BASEITEMSDECAYABLE[0152]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->BaseItemsDecayable() );
				break;
			case 161:	 // ITEMDECAYINHOUSES[0153]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ItemDecayInHouses() );
				break;
			case 162:	// COMBATEXPLODEDELAY[0154]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CombatExplodeDelay() );
				break;
			case 163:	// PAPERDOLLGUILDBUTTON[0155]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->PaperdollGuildButton() );
				break;
			case 164:	// ATTACKSPEEDFROMSTAMINA[0156]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CombatAttackSpeedFromStamina() );
				break;
			case 169:	 // DISPLAYDAMAGENUMBERS[0157]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CombatDisplayDamageNumbers() );
				break;
			case 170:	 // CLIENTSUPPORT4000[0158]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport4000() );
				break;
			case 171:	 // CLIENTSUPPORT5000[0159]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport5000() );
				break;
			case 172:	 // CLIENTSUPPORT6000[0160]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport6000() );
				break;
			case 173:	 // CLIENTSUPPORT6050[0161]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport6050() );
				break;
			case 174:	 // CLIENTSUPPORT7000[0162]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport7000() );
				break;
			case 175:	 // CLIENTSUPPORT7090[0163]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport7090() );
				break;
			case 176:	 // CLIENTSUPPORT70160[0164]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport70160() );
				break;
			case 177:	// CLIENTSUPPORT70240[0165]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport70240() );
				break;
			case 178:	// CLIENTSUPPORT70300[0166]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport70300() );
				break;
			case 179:	// CLIENTSUPPORT70331[0167]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport70331() );
				break;
			case 180:	// CLIENTSUPPORT704565[0168]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport704565() );
				break;
			case 181:	// CLIENTSUPPORT70610[0169]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ClientSupport70610() );
				break;
			case 182:	// EXTENDEDSTARTINGSTATS[0170]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ExtendedStartingStats() );
				break;
			case 183:	// EXTENDEDSTARTINGSKILLS[0171]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ExtendedStartingSkills() );
				break;
			case 184:	// WEAPONDAMAGECHANCE[0172]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->CombatWeaponDamageChance()));
				break;
			case 185:	// ARMORDAMAGECHANCE[0173]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->CombatArmorDamageChance()));
				break;
			case 186:	// WEAPONDAMAGEMIN[0174]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->CombatWeaponDamageMin()));
				break;
			case 187:	// WEAPONDAMAGEMAX[0175]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->CombatWeaponDamageMax()));
				break;
			case 188:	// ARMORDAMAGEMIN[0176]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->CombatArmorDamageMin()));
				break;
			case 189:	// ARMORDAMAGEMAX[0177]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->CombatArmorDamageMax()));
				break;
			case 190:	// GLOBALATTACKSPEED[0178]
				*rval = INT_TO_JSVAL( static_cast<R32>(cwmWorldState->ServerData()->GlobalAttackSpeed()));
				break;
			case 191:	// NPCSPELLCASTSPEED[0179]
				*rval = INT_TO_JSVAL( static_cast<R32>(cwmWorldState->ServerData()->NPCSpellCastSpeed()));
				break;
			case 192:	// FISHINGSTAMINALOSS[0180]
				*rval = INT_TO_JSVAL( static_cast<SI16>(cwmWorldState->ServerData()->FishingStaminaLoss()));
				break;
			case 193:	// RANDOMSTARTINGLOCATION[0181]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerRandomStartingLocation() );
				break;
			case 194:	// ASSISTANTNEGOTIATION[0183]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetAssistantNegotiation() );
				break;
			case 195:	// KICKONASSISTANTSILENCE[0184]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->KickOnAssistantSilence() );
				break;
			case 196:	// AF_FILTERWEATHER[0185]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_FILTERWEATHER ) );
				break;
			case 197:	// AF_FILTERLIGHT[0186]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_FILTERLIGHT ) );
				break;
			case 198:	// AF_SMARTTARGET[0187]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_SMARTTARGET ) );
				break;
			case 199:	// AF_RANGEDTARGET[0188]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_RANGEDTARGET ) );
				break;
			case 200:	// AF_AUTOOPENDOORS[0189]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOOPENDOORS ) );
				break;
			case 201:	// AF_DEQUIPONCAST[0190]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_DEQUIPONCAST ) );
				break;
			case 202:	// AF_AUTOPOTIONEQUIP[0191]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP ) );
				break;
			case 203:	// AF_POISONEDCHECKS[0192]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_POISONEDCHECKS ) );
				break;
			case 204:	// AF_LOOPEDMACROS[0193]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_LOOPEDMACROS ) );
				break;
			case 205:	// AF_USEONCEAGENT[0194]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_USEONCEAGENT ) );
				break;
			case 206:	// AF_RESTOCKAGENT[0195]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_RESTOCKAGENT ) );
				break;
			case 207:	// AF_SELLAGENT[0196]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_SELLAGENT ) );
				break;
			case 208:	// AF_BUYAGENT[0197]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_BUYAGENT ) );
				break;
			case 209:	// AF_POTIONHOTKEYS[0198]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_POTIONHOTKEYS ) );
				break;
			case 210:	// AF_RANDOMTARGETS[0199]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_RANDOMTARGETS ) );
				break;
			case 211:	// AF_CLOSESTTARGETS[0200]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_CLOSESTTARGETS ) );
				break;
			case 212:	// AF_OVERHEADHEALTH[0201]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_OVERHEADHEALTH ) );
				break;
			case 213:	// AF_AUTOLOOTAGENT[0202]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOLOOTAGENT ) );
				break;
			case 214:	// AF_BONECUTTERAGENT[0203]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_BONECUTTERAGENT ) );
				break;
			case 215:	// AF_JSCRIPTMACROS[0204]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_JSCRIPTMACROS ) );
				break;
			case 216:	// AF_AUTOREMOUNT[0205]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOREMOUNT ) );
				break;
			case 217:	// AF_ALL[0206]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_ALL ) );
				break;
			case 218:	// CLASSICUOMAPTRACKER[0207]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetClassicUOMapTracker() );
				break;
			case 219:	// DECAYTIMERINHOUSE[0208]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_DECAYINHOUSE ))));
				break;
			case 220:	// PROTECTPRIVATEHOUSES[0209]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ProtectPrivateHouses() );
				break;
			case 221:	// TRACKHOUSESPERACCOUNT[0210]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->TrackHousesPerAccount() );
				break;
			case 222:	// MAXHOUSESOWNABLE[0211]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->MaxHousesOwnable()));
				break;
			case 223:	// MAXHOUSESCOOWNABLE[0212]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->MaxHousesCoOwnable()));
				break;
			case 224:	// CANOWNANDCOOWNHOUSES[0213]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CanOwnAndCoOwnHouses() );
				break;
			case 225:	// COOWNHOUSESONSAMEACCOUNT[0214]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->CoOwnHousesOnSameAccount() );
				break;
			case 226:	// ITEMSDETECTSPEECH[0215]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ItemsDetectSpeech() );
				break;
			case 227:	// MAXPLAYERPACKITEMS[0216]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->MaxPlayerPackItems()));
				break;
			case 228:	// MAXPLAYERBANKITEMS[0217]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->MaxPlayerBankItems()));
				break;
			case 229:	// FORCENEWANIMATIONPACKET[0218]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ForceNewAnimationPacket() );
				break;
			case 230:	// MAPDIFFSENABLED[0219]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->MapDiffsEnabled() );
				break;
			case 244:	// CUOENABLED[0233]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ConnectUOServerPoll() );
				break;
			case 245:	// ALCHEMYBONUSENABLED[0234]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->AlchemyDamageBonusEnabled() );
				break;
			case 246:	// ALCHEMYBONUSMODIFIER[0235]
				*rval = INT_TO_JSVAL( static_cast<UI08>(cwmWorldState->ServerData()->AlchemyDamageBonusModifier()));
				break;
			case 247:	 // NPCFLAGUPDATETIMER[0236]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_NPCFLAGUPDATETIMER ))));
				break;
			case 248:	 // JSENGINESIZE[0237]
				*rval = INT_TO_JSVAL( static_cast<UI16>(cwmWorldState->ServerData()->GetJSEngineSize() ));
				break;
			case 250:	 // SCRIPTDATADIRECTORY[0239]
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTDATA ).c_str() );
				*rval = STRING_TO_JSVAL( tString );
				break;
			}
			case 251:	 // THIRSTRATE[0240]
				*rval = INT_TO_JSVAL( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_THIRSTRATE ))));
				break;
			case 252:	 // THIRSTDRAINVAL[0241]
				*rval = INT_TO_JSVAL( static_cast<SI16>( cwmWorldState->ServerData()->ThirstDrain() ));
				break;
			case 253:	 // PETTHIRSTOFFLINE[0242]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->PetThirstOffline() );
				break;
			case 255:	 // BLOODDECAYTIMER[0243]
				*rval = INT_TO_JSVAL( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAY ) ) );
				break;
			case 256:	 // BLOODDECAYCORPSETIMER[0244]
				*rval = INT_TO_JSVAL( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAYCORPSE ) ) );
				break;
			case 257:	// BLOODEFFECTCHANCE[0245]
				*rval = INT_TO_JSVAL( static_cast<UI08>( cwmWorldState->ServerData()->CombatBloodEffectChance() ) );
				break;
			case 258:	 // NPCCORPSEDECAYTIMER[0246]
				*rval = INT_TO_JSVAL( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_NPCCORPSEDECAY ) ) );
				break;
			case 259:	 // HUNGERENABLED[0247]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->HungerSystemEnabled() );
				break;
			case 260:	 // THIRSTENABLED[0248]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ThirstSystemEnabled() );
				break;
			case 261:	 // TRAVELSPELLSFROMBOATKEYS[0249]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->TravelSpellsFromBoatKeys() );
				break;
			case 262:	 // TRAVELSPELLSWHILEOVERWEIGHT[0250]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->TravelSpellsWhileOverweight() );
				break;
			case 263:	 // MARKRUNESINMULTIS[0251]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->MarkRunesInMultis() );
				break;
			case 264:	 // TRAVELSPELLSBETWEENWORLDS[0252]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() );
				break;
			case 265:	 // TRAVELSPELLSWHILEAGGRESSOR[0253]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->TravelSpellsWhileAggressor() );
				break;
			case 266:	 // BANKBUYTHRESHOLD[0254]
				*rval = INT_TO_JSVAL( static_cast<UI16>( cwmWorldState->ServerData()->BuyThreshold() ) );
				break;
			case 267:	 // NETWORKLOG[0255]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerNetworkLog() );
				break;
			case 268:	 // SPEECHLOG[0256]
				*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->ServerSpeechLog() );
				break;
			case 269:	 // NPCMOUNTEDWALKINGSPEED[0257]
				*rval = INT_TO_JSVAL( static_cast<R32>( cwmWorldState->ServerData()->NPCMountedWalkingSpeed() ) );
				break;
			case 270:	 // NPCMOUNTEDRUNNINGSPEED[0258]
				*rval = INT_TO_JSVAL( static_cast<R32>( cwmWorldState->ServerData()->NPCMountedRunningSpeed() ) );
				break;
			case 271:	 // NPCMOUNTEDFLEEINGSPEED[0259]
				*rval = INT_TO_JSVAL( static_cast<R32>( cwmWorldState->ServerData()->NPCMountedFleeingSpeed() ) );
				break;
			default:
				DoSEErrorMessage( "GetServerSetting: Invalid server setting name provided" );
				return false;
		}
	}
	else
	{
		DoSEErrorMessage( "GetServerSetting: Provided argument contained no valid string data" );
		return JS_FALSE;
	}
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetClientFeature( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific client feature is enabled on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetClientFeature( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetClientFeature: Invalid number of arguments (takes 1 - feature ID)" );
		return JS_FALSE;
	}

	ClientFeatures clientFeature = static_cast<ClientFeatures>(JSVAL_TO_INT( argv[0] ));
	*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetClientFeature( clientFeature ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetServerFeature( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific Server feature is enabled on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetServerFeature( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		DoSEErrorMessage( "GetServerFeature: Invalid number of arguments (takes 1 - feature ID)" );
		return JS_FALSE;
	}

	ServerFeatures serverFeature = static_cast<ServerFeatures>(JSVAL_TO_INT( argv[0] ));
	*rval = BOOLEAN_TO_JSVAL( cwmWorldState->ServerData()->GetServerFeature( serverFeature ) );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetAccountCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of accounts on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetAccountCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( Accounts->size() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_GetPlayerCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of players online on server
//o-----------------------------------------------------------------------------------------------o
JSBool SE_GetPlayerCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	*rval = INT_TO_JSVAL( cwmWorldState->GetPlayersOnline() );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_DistanceBetween( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the distance between two locations, or two objects - or a combination of both
//o-----------------------------------------------------------------------------------------------o
JSBool SE_DistanceBetween( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 && argc != 4 && argc != 6 )
	{
		DoSEErrorMessage( "DistanceBetween: needs 2, 3, 4 or 6 arguments - object a, object b - or object a, object b, (bool)checkZ - or x1, y1 and x2, y2 - or x1, y1, z1 and x2, y2, z2!" );
		return JS_FALSE;
	}

	if( argc <= 3 )
	{
		// 2 or 3 arguments - find dinstance between two objects in 2D or 3D
		JSObject *srcObj = JSVAL_TO_OBJECT( argv[0] );
		JSObject *trgObj = JSVAL_TO_OBJECT( argv[1] );
		bool checkZ = argc == 3 ? ( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE ) : false;
		CBaseObject *srcBaseObj = static_cast<CBaseObject *>( JS_GetPrivate( cx, srcObj ) );
		CBaseObject *trgBaseObj = static_cast<CBaseObject *>( JS_GetPrivate( cx, trgObj ) );
		if( !ValidateObject( srcBaseObj ) || !ValidateObject( trgBaseObj ) )
		{
			DoSEErrorMessage( "DistanceBetween: Invalid source or target object" );
			*rval = INT_TO_JSVAL(-1);
			return JS_FALSE;
		}

		if( checkZ )
			*rval = INT_TO_JSVAL( getDist3D( srcBaseObj, trgBaseObj ) );
		else
			*rval = INT_TO_JSVAL( getDist( srcBaseObj, trgBaseObj ) );
	}
	else
	{
		UI16 x1 = static_cast<UI16>( JSVAL_TO_INT( argv[0] ) );
		UI16 y1 = static_cast<UI16>( JSVAL_TO_INT( argv[1] ) );
		UI16 x2 = 0;
		UI16 y2 = 0;
		if( argc == 4 )
		{
			// 4 arguments - find distance in 2D
			x2 = static_cast<UI16>( JSVAL_TO_INT( argv[2] ) );
			y2 = static_cast<UI16>( JSVAL_TO_INT( argv[3] ) );
			*rval = INT_TO_JSVAL( getDist( point3( x1, y1, 0 ), point3( x2, y2, 0 )));
		}
		else
		{
			// 6 arguments - find distance in 3D
			SI08 z1 = static_cast<SI08>( JSVAL_TO_INT( argv[2] ) );
			x2 = static_cast<UI16>( JSVAL_TO_INT( argv[3] ) );
			y2 = static_cast<UI16>( JSVAL_TO_INT( argv[4] ) );
			SI08 z2 = static_cast<SI08>( JSVAL_TO_INT( argv[5] ) );
			*rval = INT_TO_JSVAL( getDist3D( point3( x1, y1, z1 ), point3( x2, y2, z2 ) ) );
		}
	}
	
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_BASEITEMSERIAL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the BASEITEMSERIAL constant
//o-----------------------------------------------------------------------------------------------o
JSBool SE_BASEITEMSERIAL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JS_NewNumberValue( cx, BASEITEMSERIAL, rval );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_INVALIDSERIAL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDSERIAL constant
//o-----------------------------------------------------------------------------------------------o
JSBool SE_INVALIDSERIAL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JS_NewNumberValue( cx, INVALIDSERIAL, rval );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_INVALIDID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDID constant
//o-----------------------------------------------------------------------------------------------o
JSBool SE_INVALIDID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JS_NewNumberValue( cx, INVALIDID, rval );
	return JS_TRUE;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	JSBool SE_INVALIDCOLOUR( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDCOLOUR constant
//o-----------------------------------------------------------------------------------------------o
JSBool SE_INVALIDCOLOUR( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JS_NewNumberValue( cx, INVALIDCOLOUR, rval );
	return JS_TRUE;
}
