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
#include "cVersionClass.h"
#include "Dictionary.h"
#include "speech.h"
#include "CGump.h"
#include "ObjectFactory.h"
#include "network.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"
#include "CJSEngine.h"
#include "PartySystem.h"
#include "cSpawnRegion.h"
#include "CPacketSend.h"
#include <js/Object.h>
#include <js/Array.h>



void		LoadTeleportLocations( void );
void		LoadSpawnRegions( void );
void		LoadRegions( void );
void		UnloadRegions( void );
void		UnloadSpawnRegions( void );
void		LoadSkills( void );
void		ScriptError( JSContext *cx, const char *txt, ... );

#define __EXTREMELY_VERBOSE__

std::map<std::string, ObjectType> stringToObjType;

void InitStringToObjType( void )
{
	stringToObjType["BASEOBJ"]		= OT_CBO;
	stringToObjType["CHARACTER"]	= OT_CHAR;
	stringToObjType["ITEM"]			= OT_ITEM;
	stringToObjType["SPAWNER"]		= OT_SPAWNER;
	stringToObjType["MULTI"]		= OT_MULTI;
	stringToObjType["BOAT"]			= OT_BOAT;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FindObjTypeFromString()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine object type (ITEM, CHARACTER, MULTI, etc) based on provided string
//o------------------------------------------------------------------------------------------------o
ObjectType FindObjTypeFromString( std::string strToFind )
{
	if( stringToObjType.empty() ) // if we haven't built our array yet
	{
		InitStringToObjType();
	}
	std::map<std::string, ObjectType>::const_iterator toFind = stringToObjType.find( oldstrutil::upper( strToFind ));
	if( toFind != stringToObjType.end() )
	{
		return toFind->second;
	}
	return OT_CBO;
}

// Effect related functions
//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoTempEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Does a temporary effect (things like protection, night sight, and what not) frm
//|					src to trg. If iType = 0, then it's a character, otherwise it's an item.
//o------------------------------------------------------------------------------------------------o
bool SE_DoTempEffect( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 7 )
	{
		ScriptError( cx, "DoTempEffect: Invalid number of arguments (takes 7 or 8)" );
		return false;
	}
  auto args				= JS::CallArgsFromVp(argc, vp);
	UI08 iType			= static_cast<UI08>( args.get( 0 ).toInt32() );
	UI32 targNum		= args.get( 3 ).toInt32();
	UI08 more1			= static_cast<UI08>( args.get( 4 ).toInt32() );
	UI08 more2			= static_cast<UI08>( args.get( 5 ).toInt32() );
	UI08 more3			= static_cast<UI08>( args.get( 6 ).toInt32() );

	CItem *myItemPtr	= nullptr;

	if( argc == 8 )
	{
		JSObject *myitemptr = args.get( 7 ).toObjectOrNull();
		myItemPtr = JS::GetMaybePtrFromReservedSlot<CItem>( myitemptr, 0 );
	}

	JSObject *mysrc		= args.get( 1 ).toObjectOrNull();
	CChar *mysrcChar	= nullptr;

	// Check if mysrc is null before continuing - it could be this temp effect as no character-based source!
	if( mysrc != nullptr )
	{
		mysrcChar = JS::GetMaybePtrFromReservedSlot<CChar>( mysrc, 0 );
		if( !ValidateObject( mysrcChar ))
		{
			ScriptError( cx, "DoTempEffect: Invalid src" );
			return false;
		}
	}

	if( iType == 0 )	// character
	{
		JSObject *mydestc = args.get( 2 ).toObjectOrNull();
		CChar *mydestChar = JS::GetMaybePtrFromReservedSlot<CChar>( mydestc, 0 );

		if( !ValidateObject( mydestChar ))
		{
			ScriptError( cx, "DoTempEffect: Invalid target " );
			return false;
		}
		if( argc == 8 )
		{
			Effects->TempEffect( mysrcChar, mydestChar, static_cast<SI08>( targNum ), more1, more2, more3, myItemPtr );
		}
		else
		{
			Effects->TempEffect( mysrcChar, mydestChar, static_cast<SI08>( targNum ), more1, more2, more3 );
		}
	}
	else
	{
		JSObject *mydesti = args.get( 2 ).toObjectOrNull();
		CItem *mydestItem = JS::GetMaybePtrFromReservedSlot<CItem>( mydesti, 0 );

		if( !ValidateObject( mydestItem ))
		{
			ScriptError( cx, "DoTempEffect: Invalid target " );
			return false;
		}
		Effects->TempEffect( mysrcChar, mydestItem, static_cast<SI08>( targNum ), more1, more2, more3 );
	}
	return true;
}

// Speech related functions
void SysBroadcast( const std::string& txt );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_BroadcastMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Broadcasts specified string to all online players
//o------------------------------------------------------------------------------------------------o
bool SE_BroadcastMessage( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "BroadcastMessage: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args				= JS::CallArgsFromVp(argc, vp);
	std::string trgMessage = convertToString( cx, args.get( 0 ).toString() );
	if( trgMessage.empty() || trgMessage.length() == 0 )
	{
		ScriptError( cx, oldstrutil::format( "BroadcastMessage: Invalid string (%s)", trgMessage.c_str() ).c_str() );
		return false;
	}
	SysBroadcast( trgMessage );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcItemFromSer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns item object based on provided serial
//o------------------------------------------------------------------------------------------------o
bool SE_CalcItemFromSer( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 && argc != 4 )
	{
		ScriptError( cx, "CalcItemFromSer: Invalid number of arguments (takes 1 or 4)" );
		return false;
	}
  auto args				= JS::CallArgsFromVp(argc, vp);
	SERIAL targSerial;
	if( argc == 1 )
	{
		std::string str = convertToString( cx, args.get( 0 ).toString() );
		targSerial = oldstrutil::value<SERIAL>( str );
	}
	else
	{
		targSerial = CalcSerial( args.get( 0 ).toInt32(), args.get( 1 ).toInt32(), args.get( 2 ).toInt32(), args.get( 3 ).toInt32() );
	}

	CItem *newItem	= CalcItemObjFromSer( targSerial );
	if( newItem != nullptr )
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
    args.rval().setObjectOrNull(myObj);
	}
	else
	{
    args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcMultiFromSer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns item object based on provided serial
//o------------------------------------------------------------------------------------------------o
bool SE_CalcMultiFromSer( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 && argc != 4 )
	{
		ScriptError( cx, "CalcMultiFromSer: Invalid number of arguments (takes 1 or 4)" );
		return false;
	}
  auto args				= JS::CallArgsFromVp(argc, vp);
	SERIAL targSerial;
	if( argc == 1 )
	{
		targSerial = args.get( 0 ).toInt32();
	}
	else
	{
		targSerial = CalcSerial( args.get( 0 ).toInt32(), args.get( 1 ).toInt32(), args.get( 2 ).toInt32(), args.get( 3 ).toInt32() );
	}

	CItem *newMulti	= CalcMultiFromSer( targSerial );
	if( newMulti != nullptr )
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, newMulti, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
    args.rval().setObjectOrNull(myObj);
	}
	else
	{
    args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcCharFromSer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates and returns character object based on provided serial
//o------------------------------------------------------------------------------------------------o
bool SE_CalcCharFromSer( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 && argc != 4 )
	{
		ScriptError( cx, "CalcCharFromSer: Invalid number of arguments (takes 1 or 4)" );
		return false;
	}
  auto args				= JS::CallArgsFromVp(argc, vp);
	SERIAL targSerial = INVALIDSERIAL;
	if( argc == 1 )
	{
		targSerial = args.get( 0 ).toInt32();
	}
	else
	{
		targSerial = CalcSerial( args.get( 0 ).toInt32(), args.get( 1 ).toInt32(), args.get( 2 ).toInt32(), args.get( 3 ).toInt32() );
	}

	CChar *newChar = CalcCharObjFromSer( targSerial );
	if( newChar != nullptr )
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_CHAR, newChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
    args.rval().setObjectOrNull(myObj);
	}
	else
	{
    args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoMovingEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a moving effect from source object to target object or location
//o------------------------------------------------------------------------------------------------o
bool SE_DoMovingEffect( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 6 )
	{
		ScriptError( cx, "DoMovingEffect: Invalid number of arguments (takes 6->8 or 8->10, or 10->12)" );
		return false;
	}

  auto args				= JS::CallArgsFromVp(argc, vp);
  CBaseObject *src = nullptr;
	CBaseObject *trg	= nullptr;
	bool srcLocation	= false;
	bool targLocation	= false;
	UI16 srcX			= 0;
	UI16 srcY			= 0;
	SI08 srcZ			= 0;
	UI16 targX			= 0;
	UI16 targY			= 0;
	SI08 targZ			= 0;
	UI16 effect			= 0;
	UI08 speed			= 0;
	UI08 loop			= 0;
	bool explode		= 0;
	UI32 hue			= 0;
	UI32 renderMode		= 0;

	if( args.get( 0 ).isInt32() )
	{
		// 10, 11 or 12 arguments
		// srcX, srcY, srcZ, targX, targY, targZ, effect, speed, loop, explode, [hue], [renderMode]
		srcLocation = true;
		targLocation = true;
		targLocation = true;
		srcX	  = args.get( 0 ).toInt32();
		srcY	  = args.get( 1 ).toInt32();
		srcZ	  = args.get( 2 ).toInt32();
		targX	  = args.get( 3 ).toInt32();
		targY	  = args.get( 4 ).toInt32();
		targZ	  = args.get( 5 ).toInt32();
		effect	= args.get( 6 ).toInt32();
		speed	  = args.get( 7 ).toInt32();
		loop	  = args.get( 8 ).toInt32();
    explode = args.get( 9 ).toBoolean();
		if( argc >= 11 )
		{
			hue = args.get( 10 ).toInt32();
		}
		if( argc >= 12 )
		{
			renderMode = args.get( 11 ).toInt32();
		}
	}
	else
	{
		JSObject *srcObj = args.get( 0 ).toObjectOrNull();
		src				 = JS::GetMaybePtrFromReservedSlot<CBaseObject>( srcObj, 0 );
		if( !ValidateObject( src ))
		{
			ScriptError( cx, "DoMovingEffect: Invalid source object" );
			return false;
		}

		if (args.get(1).isInt32() )
		{
			// 8, 9 or 10 arguments
			// srcObj, targX, targY, targZ, effect, speed, loop, explode, [hue], [renderMode]
			targLocation = true;
			targX	  = args.get( 1 ).toInt32();
			targY	  = args.get( 2 ).toInt32();
			targZ	  = args.get( 3 ).toInt32();
			effect	= args.get( 4 ).toInt32();
			speed	  = args.get( 5 ).toInt32();
			loop	  = args.get( 6 ).toInt32();
			explode	= args.get( 7 ).toBoolean();
			if( argc >= 9 )
			{
				hue = args.get( 8 ).toInt32();
			}
			if( argc >= 10 )
			{
        renderMode = args.get(9).toInt32();
			}
		}
		else
		{
			// 6, 7 or 8 arguments
			// srcObj, targObj, effect, speed, loop, explode, [hue], [renderMode]
			if( !args.get( 1 ).isObject())
			{
				ScriptError( cx, "DoMovingEffect: Invalid target object" );
				return false;
			}

			JSObject *trgObj	= args.get( 1 ).toObjectOrNull();
			trg = JS::GetMaybePtrFromReservedSlot<CBaseObject>( trgObj, 0 );
			if( !ValidateObject( trg ))
			{
				ScriptError( cx, "DoMovingEffect: Invalid target object" );
				return false;
			}

			effect	= static_cast<UI16>( args.get( 2 ).toInt32());
			speed	= static_cast<UI08>( args.get( 3 ).toInt32());
			loop	= static_cast<UI08>( args.get( 4 ).toInt32());
			explode	= ( args.get( 5 ).toBoolean() );
			if( argc >= 7 )
			{
				hue = static_cast<UI32>( args.get( 6 ).toInt32());
			}
			if( argc >= 8 )
			{
				renderMode = static_cast<UI32>( args.get( 7 ).toInt32());
			}
		}
	}

	if( srcLocation && targLocation )
	{
		Effects->PlayMovingAnimation( srcX, srcY, srcZ, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode );
	}
	else if( !srcLocation && targLocation )
	{
		Effects->PlayMovingAnimation( src, targX, targY, targZ, effect, speed, loop, explode, hue, renderMode );
	}
	else
	{
		Effects->PlayMovingAnimation( src, trg, effect, speed, loop, explode, hue, renderMode );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoStaticEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a static effect at a target location
//o------------------------------------------------------------------------------------------------o
bool SE_DoStaticEffect( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 7 )
	{
		ScriptError( cx, "StaticEffect: Invalid number of arguments (takes 7 - targX, targY, targZ, effectID, speed, loop, explode)" );
		return false;
	}

  auto args				= JS::CallArgsFromVp(argc, vp);
	SI16 targX		= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 targY		= static_cast<SI16>( args.get( 1 ).toInt32());
	SI16 targZ		= static_cast<SI16>( args.get( 2 ).toInt32());
	UI16 effectId	= static_cast<UI16>( args.get( 3 ).toInt32());
	UI08 speed		= static_cast<UI08>( args.get( 4 ).toInt32());
	UI08 loop		= static_cast<UI08>( args.get( 5 ).toInt32());
	bool explode	= ( args.get( 6 ).toBoolean() );

	Effects->PlayStaticAnimation( targX, targY, targZ, effectId, speed, loop, explode );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RandomNumber()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random number between loNum and hiNum
//o------------------------------------------------------------------------------------------------o
bool SE_RandomNumber( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "RandomNumber: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	auto loVal	= args.get( 0 ).toInt32();
  auto hiVal	= args.get( 1 ).toInt32();
  args.rval().setInt32( RandomNum(loVal, hiVal));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_MakeItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Character creates specified item based on entry in CREATE DFNs
//o------------------------------------------------------------------------------------------------o
bool SE_MakeItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 && argc != 4 )
	{
		ScriptError( cx, "MakeItem: Invalid number of arguments (takes 3, or 4 - socket, character, createID - and optionally - resourceColour)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *mSock = args.get( 0 ).toObjectOrNull();
  CSocket *sock = JS::GetMaybePtrFromReservedSlot<CSocket>( mSock, 0 );
	JSObject *mChar = args.get( 1 ).toObjectOrNull();
  CChar *player = JS::GetMaybePtrFromReservedSlot<CChar>( mChar, 0 );
	if( !ValidateObject( player ))
	{
		ScriptError( cx, "MakeItem: Invalid character" );
		return false;
	}
	UI16 itemMenu		= static_cast<UI16>( args.get( 2 ).toInt32());
	CreateEntry_st *toFind = Skills->FindItem( itemMenu );
	if( toFind == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "MakeItem: Invalid make item (%i)", itemMenu ).c_str() );
		return false;
	}
	UI16 resourceColour = 0;
	if( argc == 4 )
	{
		resourceColour = static_cast<UI16>( args.get( 3 ).toInt32());
	}

	Skills->MakeItem( *toFind, player, sock, itemMenu, resourceColour );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CommandLevelReq()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the command level required to execute the specified command
//o------------------------------------------------------------------------------------------------o
bool SE_CommandLevelReq( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "CommandLevelReq: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string test = convertToString( cx, args.get( 0 ).toString() );
	if( test.empty() || test.length() == 0 )
	{
		ScriptError( cx, "CommandLevelReq: Invalid command name" );
		return false;
	}
	CommandMapEntry_st *details = Commands->CommandDetails( test );
	if( details == nullptr )
	{
		args.rval().setInt32( 255 );
	}
	else
	{
		args.rval().setInt32( details->cmdLevelReq );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CommandExists()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specified command exists in command table or not
//o------------------------------------------------------------------------------------------------o
bool SE_CommandExists( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "CommandExists: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string test = convertToString( cx, args.get( 0 ).toString() );
	if( test.empty() || test.length() == 0 )
	{
		ScriptError( cx, "CommandExists: Invalid command name" );
		return false;
	}
	args.rval().setBoolean(  Commands->CommandExists( test ) );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FirstCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the first command in the table. If nothing, it's a 0 length string.
//o------------------------------------------------------------------------------------------------o
bool SE_FirstCommand( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	const std::string tVal = Commands->FirstCommand();
	JSString *strSpeech = nullptr;
	if( tVal.empty() )
	{
		strSpeech = JS_NewStringCopyZ( cx, "" );
	}
	else
	{
		strSpeech = JS_NewStringCopyZ( cx, tVal.c_str() );
	}
	args.rval().setString( strSpeech );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_NextCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the next command in the table. If nothing, it's a 0 length string.
//o------------------------------------------------------------------------------------------------o
bool SE_NextCommand( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	const std::string tVal = Commands->NextCommand();
	JSString *strSpeech = nullptr;
	if( tVal.empty() )
	{
		strSpeech = JS_NewStringCopyZ( cx, "" );
	}
	else
	{
		strSpeech = JS_NewStringCopyZ( cx, tVal.c_str() );
	}

	args.rval().setString( strSpeech );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FinishedCommandList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if there are no more commands left in the table.
//o------------------------------------------------------------------------------------------------o
bool SE_FinishedCommandList( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setBoolean( Commands->FinishedCommandList() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If called from within CommandRegistration() function in a script registered
//|					under the COMMAND_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP, will register
//|					the specified command in the command table and call the function in the same
//|					script whose name corresponds with the command name, in the shape of
//|						function command_CMDNAME( socket, cmdString )
//o------------------------------------------------------------------------------------------------o
bool SE_RegisterCommand( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 )
	{
		ScriptError( cx, "  RegisterCommand: Invalid number of arguments (takes 4)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string toRegister	= convertToString( cx, args.get( 0 ).toString() );
	UI08 execLevel			= static_cast<UI08>( args.get( 1 ).toInt32());
	bool isEnabled			= ( args.get( 2 ).toBoolean() );
	UI16 scriptId			= JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

	if( scriptId == 0xFFFF )
	{
		ScriptError( cx, " RegisterCommand: JS Script has an Invalid ScriptID" );
		return false;
	}

	Commands->Register( toRegister, scriptId, execLevel, isEnabled );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If called from within SpellRegistration() function in a script registered under
//|					the MAGIC_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP, will register the
//|					onSpellCast() event in the same script as a global listener for use of the
//|					specified magic spell.
//o------------------------------------------------------------------------------------------------o
bool SE_RegisterSpell( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "RegisterSpell: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 spellNumber	= args.get( 0 ).toInt32();
	bool isEnabled		= ( args.get( 1 ).toBoolean() );
	cScript *myScript	= JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
	Magic->Register( myScript, spellNumber, isEnabled );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for use of specified skill, and
//|					triggers onSkill() event in same script when specified skill is used, if
//|					script is added under the SKILLUSE_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a SkillRegistration() function
//o------------------------------------------------------------------------------------------------o
bool SE_RegisterSkill( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "RegisterSkill: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 skillNumber	= args.get( 0 ).toInt32();
	bool isEnabled		= ( args.get( 1 ).toBoolean() );
	UI16 scriptId		= JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));
	if( scriptId != 0xFFFF )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Print( " " );
		Console.MoveTo( 15 );
		Console.Print( "Registering skill number " );
		Console.TurnYellow();
		Console.Print( oldstrutil::format( "%i", skillNumber ));
		if( !isEnabled )
		{
			Console.TurnRed();
			Console.Print( " [DISABLED]" );
		}
		Console.Print( "\n" );
		Console.TurnNormal();
#endif
		// If skill is not enabled, unset scriptId from skill data
		if( !isEnabled )
		{
			cwmWorldState->skill[skillNumber].jsScript = 0;
			return false;
		}

		// Skillnumber above ALLSKILLS refers to STR, INT, DEX, Fame and Karma,
		if( skillNumber < 0 || skillNumber >= ALLSKILLS )
			return true;

		// Both scriptId and skillNumber seem valid; assign scriptId to this skill
		cwmWorldState->skill[skillNumber].jsScript = scriptId;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterPacket()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for a specific network packet, and
//|					triggers onPacketReceive() event in same script when this network packet is sent,
//|					if script is added under the PACKET_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a PacketRegistration() function
//o------------------------------------------------------------------------------------------------o
bool SE_RegisterPacket( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "RegisterPacket: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	UI08 packet			= static_cast<UI08>( args.get( 0 ).toInt32());
	UI08 subCmd			= static_cast<UI08>( args.get( 1 ).toInt32());
	UI16 scriptId		= JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));
	if( scriptId != 0xFFFF )
	{
#if defined( UOX_DEBUG_MODE )
		Console.Print( oldstrutil::format( "Registering packet number 0x%X, subcommand 0x%x\n", packet, subCmd ));
#endif
		Network->RegisterPacket( packet, subCmd, scriptId );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterKey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register JS script as a global listener for a specific keypress in UOX3 console,
//|					and triggers specified function in same script when key is pressed, if script
//|					is added under the CONSOLE_SCRIPTS section of JSE_FILEASSOCIATIONS.SCP
//|					and this function is called from a ConsoleRegistration() function
//o------------------------------------------------------------------------------------------------o
bool SE_RegisterKey( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "RegisterKey: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
  auto encaps = args.get( 0 );
	std::string toRegister	= convertToString( cx, args.get( 1 ).toString() );
	UI16 scriptId			= JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

	if( scriptId == 0xFFFF )
	{
		ScriptError( cx, "RegisterKey: JS Script has an Invalid ScriptID" );
		return false;
	}
	SI32 toPass = 0;
	if( encaps.isString() )
	{
		std::string enStr = convertToString( cx, encaps.toString() );
		if( enStr.length() != 0 )
		{
			toPass  = enStr[0];
		}
		else
		{
			ScriptError( cx, "RegisterKey: JS Script passed an invalid key to register" );
			return false;
		}
	}
	else
	{
		toPass = encaps.toInt32();
	}
	Console.RegisterKey( toPass, toRegister, scriptId );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RegisterConsoleFunc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
bool SE_RegisterConsoleFunc( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "RegisterConsoleFunc: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string funcToRegister	= convertToString( cx, args.get( 0 ).toString() );
	std::string toRegister		= convertToString( cx, args.get( 1 ).toString() );
	UI16 scriptId				= JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx ));

	if( scriptId == 0xFFFF )
	{
		ScriptError( cx, "RegisterConsoleFunc: JS Script has an Invalid ScriptID" );
		return false;
	}

	Console.RegisterFunc( funcToRegister, toRegister, scriptId );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables a specified command on the server
//o------------------------------------------------------------------------------------------------o
bool SE_DisableCommand( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "DisableCommand: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string toDisable = convertToString( cx, args.get( 0 ).toString() );
	Commands->SetCommandStatus( toDisable, false );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableKey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables specified key in console
//o------------------------------------------------------------------------------------------------o
bool SE_DisableKey( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "DisableKey: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 toDisable = args.get( 0 ).toInt32();
	Console.SetKeyStatus( toDisable, false );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableConsoleFunc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
bool SE_DisableConsoleFunc( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "DisableConsoleFunc: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string toDisable = convertToString( cx, args.get( 0 ).toString() );
	Console.SetFuncStatus( toDisable, false );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DisableSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables specified spell on server
//o------------------------------------------------------------------------------------------------o
bool SE_DisableSpell( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "DisableSpell: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 spellNumber = args.get( 0 ).toInt32();
	Magic->SetSpellStatus( spellNumber, false );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified command on server
//o------------------------------------------------------------------------------------------------o
bool SE_EnableCommand( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "EnableCommand: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string toEnable = convertToString( cx, args.get( 0 ).toString() );
	Commands->SetCommandStatus( toEnable, true );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified spell on server
//o------------------------------------------------------------------------------------------------o
bool SE_EnableSpell( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "EnableSpell: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 spellNumber = args.get( 0 ).toInt32();
	Magic->SetSpellStatus( spellNumber, true );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableKey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Enables specified key in console
//o------------------------------------------------------------------------------------------------o
bool SE_EnableKey( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "EnableKey: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 toEnable = args.get( 0 ).toInt32();
	Console.SetKeyStatus( toEnable, true );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EnableConsoleFunc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
bool SE_EnableConsoleFunc( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "EnableConsoleFunc: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string toEnable	= convertToString( cx, args.get( 0 ).toString() );
	Console.SetFuncStatus( toEnable, false );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetHour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the hour of the current UO day
//o------------------------------------------------------------------------------------------------o
bool SE_GetHour( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	bool ampm = cwmWorldState->ServerData()->ServerTimeAMPM();
	UI08 hour = cwmWorldState->ServerData()->ServerTimeHours();
	if( ampm )
	{
		args.rval().setInt32( static_cast<UI64>( hour ) + 12 );
	}
	else
	{
		args.rval().setInt32( hour );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMinute()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the minute of the current UO day
//o------------------------------------------------------------------------------------------------o
bool SE_GetMinute( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	UI08 minute = cwmWorldState->ServerData()->ServerTimeMinutes();
	args.rval().setInt32( minute );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetDay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the day number of the server (UO days since server start)
//o------------------------------------------------------------------------------------------------o
bool SE_GetDay( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 day = cwmWorldState->ServerData()->ServerTimeDay();
	args.rval().setInt32( day );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SecondsPerUOMinute()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets the amonut of real life seconds associated with minute in the game
//o------------------------------------------------------------------------------------------------o
bool SE_SecondsPerUOMinute( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc > 1 )
	{
		ScriptError( cx, "SecondsPerUOMinute: Invalid number of arguments (takes 0 or 1)" );
		return false;
	}
	else if( argc == 1 )
	{
		UI16 secondsPerUOMinute = static_cast<UI16>( args.get( 0 ).toInt32());
		cwmWorldState->ServerData()->ServerSecondsPerUOMinute( secondsPerUOMinute );
	}
	args.rval().setInt32( cwmWorldState->ServerData()->ServerSecondsPerUOMinute() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetCurrentClock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets timestamp for current server clock
//o------------------------------------------------------------------------------------------------o
bool SE_GetCurrentClock( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( cwmWorldState->GetUICurrentTime() );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetStartTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets timestamp for server startup time
//o------------------------------------------------------------------------------------------------o
bool SE_GetStartTime( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( cwmWorldState->GetStartTime() );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRandomSOSArea()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets a random SOS area from list of such areas loaded from [SOSAREAS] section of regions.dfn
//o------------------------------------------------------------------------------------------------o
bool SE_GetRandomSOSArea( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "GetRandomSOSArea: Invalid number of arguments (takes 2)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);

	UI08 worldNum	= 0;
	UI16 instanceId	= 0;
	if( args.get( 0 ).isInt32() && args.get( 1 ).isInt32())
	{
		worldNum	= static_cast<UI08>( args.get( 0 ).toInt32());
		instanceId	= static_cast<UI16>( args.get( 1 ).toInt32());
	}
	else
	{
		ScriptError( cx, "GetRandomSOSArea: Invalid values passed in for worldNum and instanceId - must be integers!" );
		return false;
	}

	// Fetch vector of SOS locations
	auto sosLocs = cwmWorldState->sosLocs;
	if( sosLocs.size() == 0 )
	{
		ScriptError( cx, "GetRandomSOSArea: No valid SOS areas found. Is the [SOSAREAS] section of regions.dfn setup correctly?" );
		return false;
	}

	// Prepare a vector to hold the areas with correct worldNum and instanceId
	std::vector<SOSLocationEntry_st> validSOSLocs;

	// Loop through all SOS areas and cherry-pick the ones with correct worldNum and instanceId
	for( size_t i = 0; i < sosLocs.size(); ++i )
	{
		if( sosLocs[i].worldNum == worldNum && sosLocs[i].instanceId == instanceId )
		{
			validSOSLocs.push_back( sosLocs[i] );
		}
	}

	// Choose a random SOS area from the generated list of such areas
	auto rndSosLoc = validSOSLocs[RandomNum( static_cast<size_t>( 0 ), validSOSLocs.size() - 1 )];

	// Convert properties of chosen SOS area to JS::Values, so we can pass them to a JSObject
	JS::RootedValue jsX1( cx, rndSosLoc.x1 );
	JS::RootedValue jsY1( cx, rndSosLoc.y1 );
	JS::RootedValue jsX2( cx, rndSosLoc.x2 );
	JS::RootedValue jsY2( cx, rndSosLoc.y2 );
	JS::RootedValue jsWorldNum( cx, rndSosLoc.worldNum );
	JS::RootedValue jsInstanceId( cx, rndSosLoc.instanceId );

	// Construct a JS Object with the properties of the chosen SOS area
	JS::RootedObject rndSosLocObj( cx, JS::NewArrayObject( cx, 6 ) );
	JS_SetElement( cx, rndSosLocObj, 0, jsX1 );
	JS_SetElement( cx, rndSosLocObj, 1, jsY1 );
	JS_SetElement( cx, rndSosLocObj, 2, jsX2 );
	JS_SetElement( cx, rndSosLocObj, 3, jsY2 );
	JS_SetElement( cx, rndSosLocObj, 4, jsWorldNum );
	JS_SetElement( cx, rndSosLocObj, 5, jsInstanceId );

	// Pass the JS object to script
	args.rval().setObjectOrNull( rndSosLocObj );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SpawnNPC()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns NPC based on definition in NPC DFNs (or an NPCLIST)
//o------------------------------------------------------------------------------------------------o
bool SE_SpawnNPC( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 5 || argc > 7 )
	{
		ScriptError( cx, "SpawnNPC: Invalid number of arguments (takes 5, 6 or 7)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	CChar *cMade		= nullptr;
	std::string nnpcNum	= convertToString( cx, args.get( 0 ).toString() );
	UI16 x				= static_cast<UI16>( args.get( 1 ).toInt32());
	UI16 y				= static_cast<UI16>( args.get( 2 ).toInt32());
	SI08 z				= static_cast<SI08>( args.get( 3 ).toInt32());
	UI08 world			= static_cast<UI08>( args.get( 4 ).toInt32());
	UI16 instanceId = ( argc == 6 ? static_cast<SI16>( args.get( 5 ).toInt32()) : 0 );
	bool useNpcList = ( argc == 7 ? ( args.get( 6 ).toBoolean() ) : false );

	// Store original script context and object, in case NPC spawned has some event that triggers on spawn and grabs context
	////auto origContext = cx;
	////auto origObject = obj;

	cMade = Npcs->CreateNPCxyz( nnpcNum, x, y, z, world, instanceId, useNpcList );
	if( cMade != nullptr )
	{
		JSObject *myobj		= JSEngine->AcquireObject( IUE_CHAR, cMade, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myobj );
	}
	else
	{
		args.rval().setNull();
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateDFNItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates item based on definition in item DFNs
//o------------------------------------------------------------------------------------------------o
bool SE_CreateDFNItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 3 )
	{
		ScriptError( cx, "CreateDFNItem: Invalid number of arguments (takes at least 3, max 8)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	CSocket *mySock = nullptr;
  if( !args.get( 0 ).isNull() )
	{
		JSObject *mSock			= args.get( 0 ).toObjectOrNull();
		mySock					= JS::GetMaybePtrFromReservedSlot<CSocket>( mSock, 0 );
	}

	CChar *myChar = nullptr;
	if( !args.get( 1 ).isNull() )
	{
		JSObject *mChar			= args.get( 1 ).toObjectOrNull();
		myChar					= JS::GetMaybePtrFromReservedSlot<CChar>( mChar, 0 );
	}

	std::string bpSectNumber	= convertToString( cx, args.get( 2 ).toString() );
	bool bInPack				= true;
	UI16 iAmount				= 1;
	ObjectType itemType			= OT_ITEM;
	UI16 iColor					= 0xFFFF;
	UI08 worldNumber			= 0;
	UI16 instanceId				= 0;

	if( argc > 3 )
	{
		iAmount	= static_cast<UI16> (args.get( 3 ).toInt32());
	}
	if( argc > 4 )
	{
		std::string objType	= convertToString( cx, args.get( 4 ).toString() );
		itemType			= FindObjTypeFromString( objType );
	}
	if( argc > 5 )
	{
		bInPack	= ( args.get( 5 ).toBoolean() );
	}
	if( argc > 6 )
	{
		iColor = static_cast<UI16>( args.get( 6 ).toInt32());
	}
	if( argc > 7 )
	{
		worldNumber	= static_cast<UI08>( args.get( 7 ).toInt32());
	}
	if( argc > 8 )
	{
		instanceId = static_cast<UI16>( args.get( 8 ).toInt32());
	}

	// Store original script context and object, in case Item spawned has some event that triggers on spawn and grabs context
	////auto origContext = cx;
	////auto origObject = obj;

	CItem *newItem = nullptr;
	if( myChar != nullptr )
	{
		newItem = Items->CreateScriptItem( mySock, myChar, bpSectNumber, iAmount, itemType, bInPack, iColor );
	}
	else
	{
		newItem = Items->CreateBaseScriptItem( nullptr, bpSectNumber, worldNumber, iAmount, instanceId, itemType, iColor );
	}

	if( newItem != nullptr )
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateBlankItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a "blank" item with default values from client's tiledata
//|	Notes		-	Default values can be overridden through harditems.dfn
//o------------------------------------------------------------------------------------------------o
bool SE_CreateBlankItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 8 )
	{
		ScriptError( cx, "CreateBlankItem: Invalid number of arguments (takes 7)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	CItem *newItem			= nullptr;
	CSocket *mySock			= nullptr;
	if( !args.get( 0 ).isNull() )
	{
		JSObject *mSock		= args.get( 0 ).toObjectOrNull();
		mySock				= JS::GetMaybePtrFromReservedSlot<CSocket>( mSock, 0 );
	}

	CChar *myChar = nullptr;
	if( !args.get( 1 ).isNull() )
	{
		JSObject *mChar			= args.get( 1 ).toObjectOrNull();
		myChar					= JS::GetMaybePtrFromReservedSlot<CChar>( mChar, 0 );
	}

	SI32 amount				= static_cast<SI32>( args.get( 2 ).toInt32());
	std::string itemName	= convertToString( cx, args.get( 3 ).toString() );
	UI16 itemId				= static_cast<UI16>( args.get( 4 ).toInt32());
	UI16 colour				= static_cast<UI16>( args.get( 5 ).toInt32());
	std::string objType		= convertToString( cx, args.get( 6 ).toString() );
	ObjectType itemType		= FindObjTypeFromString( objType );
	bool inPack				= ( args.get( 7 ).toBoolean() );

	// Store original script context and object, in case NPC spawned has some event that triggers on spawn and grabs context
	//auto origContext = cx;
	//auto origObject = obj;

	newItem = Items->CreateItem( mySock, myChar, itemId, amount, colour, itemType, inPack );
	if( newItem != nullptr )
	{
		if( itemName != "" )
		{
			newItem->SetName( itemName );
		}
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, newItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );

	return true;
}

CMultiObj * BuildHouse( CSocket *s, UI16 houseEntry, bool checkLocation = true, SI16 xLoc = -1, SI16 yLoc = -1, SI08 zLoc = -1, UI08 worldNumber = 0, UI16 instanceId = 0 );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateHouse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates multi based on definition in house DFNs
//o------------------------------------------------------------------------------------------------o
bool SE_CreateHouse( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 4 )
	{
		ScriptError( cx, "CreateHouse: Invalid number of arguments (takes at least 4, max 8)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);

	UI16 houseId				= static_cast<UI16>( args.get( 0 ).toInt32());
	SI16 xLoc					= static_cast<SI16>( args.get( 1 ).toInt32());
	SI16 yLoc					= static_cast<SI16>( args.get( 2 ).toInt32());
	SI08 zLoc					= static_cast<SI08>( args.get( 3 ).toInt32());
	UI16 iColor					= 0xFFFF;
	UI08 worldNumber			= 0;
	UI16 instanceId				= 0;
	bool checkLocation			= true;

	if( argc > 4 )
	{
		worldNumber	= static_cast<UI08>( args.get( 4 ).toInt32());
	}
	if( argc > 5 )
	{
		instanceId	= static_cast<UI16>( args.get( 5 ).toInt32());
	}
	if( argc > 6 )
	{
		iColor		= static_cast<UI16>( args.get( 6 ).toInt32());
	}
	if( argc > 7 )
	{
		checkLocation = ( args.get( 7 ).toBoolean() );
	}

	// Store original script context and object, in case Item spawned has some event that triggers on spawn and grabs context
	//auto origContext = cx;
	//auto origObject = obj;

	CMultiObj *newMulti = BuildHouse( nullptr, houseId, checkLocation, xLoc, yLoc, zLoc, worldNumber, instanceId );
	if( newMulti != nullptr )
	{
		// Apply color to house, if it was provided
		if( iColor != 0xFFFF )
		{
			newMulti->SetColour( iColor );
		}

		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, newMulti, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );
	return true;
}

CMultiObj * BuildBaseMulti( UI16 multiId, SI16 xLoc = -1, SI16 yLoc = -1, SI08 zLoc = 127, UI08 worldNumber = 0, UI16 instanceId = 0 );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateBaseMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates base multi based on ID from multi file
//o------------------------------------------------------------------------------------------------o
bool SE_CreateBaseMulti( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 4 )
	{
		ScriptError( cx, "CreateBaseMulti: Invalid number of arguments (takes at least 4, max 8)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 multiId				= static_cast<UI16>( args.get( 0 ).toInt32());
	SI16 xLoc					= static_cast<SI16>( args.get( 1 ).toInt32());
	SI16 yLoc					= static_cast<SI16>( args.get( 2 ).toInt32());
	SI08 zLoc					= static_cast<SI08>( args.get( 3 ).toInt32());
	UI16 iColor					= 0xFFFF;
	UI08 worldNumber			= 0;
	UI16 instanceId				= 0;
	bool checkLocation			= true;
  (void)checkLocation;

	if( argc > 4 )
	{
		worldNumber				= static_cast<UI08>( args.get( 4 ).toInt32());
	}
	if( argc > 5 )
	{
		instanceId				= static_cast<UI16>( args.get( 5 ).toInt32());
	}
	if( argc > 6 )
	{
		iColor					= static_cast<UI16>( args.get( 6 ).toInt32());
	}
	if( argc > 7 )
	{
		checkLocation = ( args.get( 7 ).toBoolean() );
	}

	// Store original script context and object, in case Item spawned has some event that triggers on spawn and grabs context
	//auto origContext = cx;
	//auto origObject = obj;

	CMultiObj *newMulti = BuildBaseMulti( multiId, xLoc, yLoc, zLoc, worldNumber, instanceId );
	if( newMulti != nullptr )
	{
		// Apply color to house, if it was provided
		if( iColor != 0xFFFF )
		{
			newMulti->SetColour( iColor );
		}

		JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, newMulti, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMurderThreshold()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the max amount of kills allowed before a player turns red
//o------------------------------------------------------------------------------------------------o
bool SE_GetMurderThreshold( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( cwmWorldState->ServerData()->RepMaxKills() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RollDice()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Rolls a die with specified number of sides, and adds a fixed value
//o------------------------------------------------------------------------------------------------o
bool SE_RollDice( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 3 )
	{
		ScriptError( cx, "RollDice: Invalid number of arguments (takes 3)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	UI32 numDice = args.get( 0 ).toInt32();
	UI32 numFace = args.get( 1 ).toInt32();
	UI32 numAdd  = args.get( 2 ).toInt32();

	cDice toRoll( numDice, numFace, numAdd );

	args.rval().setInt32( toRoll.RollDice() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_RaceCompareByRace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compares the relations between two races
//|	Notes		-	0 - neutral
//|					1 to 100 - allies
//|					-1 to -100 - enemies
//o------------------------------------------------------------------------------------------------o
bool SE_RaceCompareByRace( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	RACEID r0 = static_cast<RACEID>( args.get( 0 ).toInt32());
	RACEID r1 = static_cast<RACEID>( args.get( 1 ).toInt32());
	args.rval().setInt32( Races->CompareByRace( r0, r1 ));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns multi at given coordinates, world and instanceId
//o------------------------------------------------------------------------------------------------o
bool SE_FindMulti( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 && argc != 4 && argc != 5 )
	{
		ScriptError( cx, "FindMulti: Invalid number of parameters (1, 4 or 5)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 instanceId = 0;
	if( argc == 1 )
	{
		JSObject *myitemptr = args.get( 0 ).toObjectOrNull();
		CBaseObject *myItemPtr = JS::GetMaybePtrFromReservedSlot<CBaseObject>( myitemptr, 0 );
		if( ValidateObject( myItemPtr ))
		{
			xLoc		= myItemPtr->GetX();
			yLoc		= myItemPtr->GetY();
			zLoc		= myItemPtr->GetZ();
			worldNumber = myItemPtr->WorldNumber();
			instanceId	= myItemPtr->GetInstanceId();
		}
		else
		{
			ScriptError( cx, "FindMulti: Invalid object type" );
			return false;
		}
	}
	else
	{
		xLoc		= static_cast<SI16>( args.get( 0 ).toInt32());
		yLoc		= static_cast<SI16>( args.get( 1 ).toInt32());
		zLoc		= static_cast<SI08>( args.get( 2 ).toInt32());
		worldNumber = static_cast<UI08>( args.get( 3 ).toInt32());
		if( argc == 5 )
		{
			instanceId = static_cast<UI16>( args.get( 4 ).toInt32());
		}
	}
	CMultiObj *multi = FindMulti( xLoc, yLoc, zLoc, worldNumber, instanceId );
	if( ValidateObject( multi ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetItem()
//|	Date		-	12 February, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns item closest to specified coordinates
//o------------------------------------------------------------------------------------------------o
bool SE_GetItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 4 && argc != 5 )
	{
		ScriptError( cx, "GetItem: Invalid number of parameters (4 or 5)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 instanceId = 0;

	xLoc		= static_cast<SI16>( args.get( 0 ).toInt32());
	yLoc		= static_cast<SI16>( args.get( 1 ).toInt32());
	zLoc		= static_cast<SI08>( args.get( 2 ).toInt32());
	worldNumber = static_cast<UI08>( args.get( 3 ).toInt32());
	if( argc == 5 )
	{
		instanceId = static_cast<UI16>( args.get( 4 ).toInt32());
	}

	CItem *item = GetItemAtXYZ( xLoc, yLoc, zLoc, worldNumber, instanceId );
	if( ValidateObject( item ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, item, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindItem()
//|	Date		-	12 February, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns item of given ID that is closest to specified coordinates
//o------------------------------------------------------------------------------------------------o
bool SE_FindItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 5 && argc != 6 )
	{
		ScriptError( cx, "FindItem: Invalid number of parameters (5 or 6)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 xLoc = 0, yLoc = 0;
	SI08 zLoc = 0;
	UI08 worldNumber = 0;
	UI16 id = 0;
	UI16 instanceId = 0;

	xLoc		= static_cast<SI16>( args.get( 0 ).toInt32());
	yLoc		= static_cast<SI16>( args.get( 1 ).toInt32());
	zLoc		= static_cast<SI08>( args.get( 2 ).toInt32());
	worldNumber = static_cast<UI08>( args.get( 3 ).toInt32());
	id			= static_cast<UI16>( args.get( 4 ).toInt32());
	if( argc == 6 )
	{
		instanceId = static_cast<UI16>( args.get( 5 ).toInt32());
	}

	CItem *item = FindItemNearXYZ( xLoc, yLoc, zLoc, worldNumber, id, instanceId );
	if( ValidateObject( item ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, item, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CompareGuildByGuild()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compares the relations between two guilds
//|	Notes		-	0 - Neutral
//|					1 - War
//|					2 - Ally
//|					3 - Unknown
//|					4 - Same
//|					5 - Count
//o------------------------------------------------------------------------------------------------o
bool SE_CompareGuildByGuild( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	GUILDID toCheck		= static_cast<GUILDID>( args.get( 0 ).toInt32());
	GUILDID toCheck2	= static_cast<GUILDID>( args.get( 1 ).toInt32());
	args.rval().setInt32( GuildSys->Compare( toCheck, toCheck2 ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_PossessTown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Source town takes control over target town
//o------------------------------------------------------------------------------------------------o
bool SE_PossessTown( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 town	= static_cast<UI16>( args.get( 0 ).toInt32());
	UI16 sTown	= static_cast<UI16>( args.get( 1 ).toInt32());
	cwmWorldState->townRegions[town]->Possess( cwmWorldState->townRegions[sTown] );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IsRaceWeakToWeather()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character's race is affected by given type of weather
//o------------------------------------------------------------------------------------------------o
bool SE_IsRaceWeakToWeather( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	RACEID race		= static_cast<RACEID>( args.get( 0 ).toInt32());
	WEATHID toCheck = static_cast<WEATHID>( args.get( 1 ).toInt32());
	CRace *tRace	= Races->Race( race );
	if( tRace == nullptr || toCheck >= WEATHNUM )
	{
		return false;
	}
	args.rval().setBoolean(  tRace->AffectedBy( static_cast<WeatherType>( toCheck )) );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRaceSkillAdjustment()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage modifier for specified skill based on race
//o------------------------------------------------------------------------------------------------o
bool SE_GetRaceSkillAdjustment( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	RACEID race = static_cast<RACEID>( args.get( 0 ).toInt32());
	SI32 skill = args.get( 1 ).toInt32();
	args.rval().setInt32( Races->DamageFromSkill( skill, race ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_UseItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
//o------------------------------------------------------------------------------------------------o
bool SE_UseItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "UseItem: Invalid number of arguments (takes 2 - socket/char, and item used)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	CChar *mChar = nullptr;
	CSocket *mySocket = nullptr;

	JSEncapsulate myClass( cx, &( argv[0] ));
	if( myClass.ClassName() == "UOXChar" )
	{
		if( myClass.isType( JSOT_OBJECT ))
		{
			mChar = static_cast<CChar *>( myClass.toObject() );
			if( !ValidateObject( mChar ))
			{
				mChar = nullptr;
			}
		}
	}
	else if( myClass.ClassName() == "UOXSocket" )
	{
		if( myClass.isType( JSOT_OBJECT ))
		{
			mySocket = static_cast<CSocket *>( myClass.toObject() );
			if( mySocket != nullptr )
			{
				mChar = mySocket->CurrcharObj();
			}
		}
	}

	JSObject *mItem	= args.get( 1 ).toObjectOrNull();
	CItem *myItem	= JS::GetMaybePtrFromReservedSlot<CItem>( mItem, 0 );

	if( !ValidateObject( myItem ))
	{
		ScriptError( cx, "UseItem: Invalid item" );
		return false;
	}

	if( !ValidateObject( mChar ))
	{
		ScriptError( cx, "UseItem: Invalid character" );
		return false;
	}

	// Store original script context and object, in case NPC spawned has some event that triggers on spawn and grabs context
	//auto origContext = cx;
	//auto origObject = obj;

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
		UI16 itemId			= myItem->GetId();
		UI16 envTrig		= 0;
		cScript *toExecute	= nullptr;
		if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>( iType )))
		{
			envTrig = JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>( iType ));
			toExecute = JSMapping->GetScript( envTrig );
		}
		else if( JSMapping->GetEnvokeById()->Check( itemId ))
		{
			envTrig = JSMapping->GetEnvokeById()->GetScript( itemId );
			toExecute = JSMapping->GetScript( envTrig );
		}

		// Check for the onUse events in envoke scripts!
		if( toExecute != nullptr )
		{
			if( toExecute->OnUseUnChecked( mChar, myItem ) != 0 )
			{
				// If onUseUnChecked wasn't found, or did not return false, try onUseChecked too
				toExecute->OnUseChecked( mChar, myItem );
			}
		}
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_TriggerTrap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Uses specified item
//o------------------------------------------------------------------------------------------------o
bool SE_TriggerTrap( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "UseItem: Invalid number of arguments (takes 2 - socket/char, and item (trap) triggered)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	CChar *mChar = nullptr;
	CSocket *mySocket = nullptr;

	auto myClass = args.get( 0 );
  std::string className = JS::GetClass( myClass.toObjectOrNull() )->name;

	if (className == "UOXChar")
	{
    mChar = JS::GetMaybePtrFromReservedSlot<CChar>(myClass.toObjectOrNull(), 0);
		if( !ValidateObject( mChar ))
		{
			mChar = nullptr;
		}
	}
	else if( className == "UOXSocket" )
	{
		mySocket = JS::GetMaybePtrFromReservedSlot<CSocket>(myClass.toObjectOrNull(), 0);
		if( mySocket != nullptr )
		{
			mChar = mySocket->CurrcharObj();
		}
	}

	if( !ValidateObject( mChar ))
	{
		ScriptError( cx, "TriggerTrap: Invalid character" );
		return false;
	}

	JSObject *mItem = args.get( 1 ).toObjectOrNull();
	CItem *myItem = JS::GetMaybePtrFromReservedSlot<CItem>( mItem, 0 );

	if( !ValidateObject( myItem ))
	{
		ScriptError( cx, "TriggerTrap: Invalid item" );
		return false;
	}

	// Store original script context and object, in case NPC spawned has some event that triggers on spawn and grabs context
	//auto origContext = cx;
	//auto origObject = obj;

	if( myItem->GetTempVar( CITV_MOREZ, 1 ) == 1 && myItem->GetTempVar( CITV_MOREZ, 2 ) > 0 ) // Is trapped and set to deal more than 0 damage
	{
		Magic->MagicTrap( mChar, myItem );
	}

	// Restore original script context and object
	//JS_SetGlobalObject( origContext, origObject );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_TriggerEvent()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggers a an event/function in a different JS
//|	Notes		-	Takes at least 2 parameters, which is the script number to trigger and the
//|					function name to call. Any extra parameters are extra parameters to the JS event
//o------------------------------------------------------------------------------------------------o
bool SE_TriggerEvent( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 2 )
	{
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 scriptNumberToFire = static_cast<UI16>( args.get( 0 ).toInt32());
	auto eventToFire		= convertToString( cx, args.get( 1 ).toString() );
	cScript *toExecute		= JSMapping->GetScript( scriptNumberToFire );

	if( toExecute == nullptr || eventToFire.empty() )
		return false;

	//auto origContext = cx;
	//auto origObject = obj;

	bool retVal = toExecute->CallParticularEvent( eventToFire.c_str(), &argv[2], argc - 2, rval );

	//JS_SetGlobalObject( origContext, origObject );
  return( retVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesEventExist()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for the existence of a JS event in a different JS file
//|	Notes		-	Takes 2 parameters, which is the script number to check and the
//|					event name to check for
//o------------------------------------------------------------------------------------------------o
bool SE_DoesEventExist( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( 1 );
	UI16 scriptNumberToCheck = static_cast<UI16>( args.get( 0 ).toInt32());
	auto eventToCheck		= convertToString( cx, args.get( 1 ).toString() );
	cScript *toExecute		= JSMapping->GetScript( scriptNumberToCheck );

	if( toExecute == nullptr || eventToCheck.empty() )
		return false;

	bool retVal = toExecute->DoesEventExist( eventToCheck.c_str() );
	if( !retVal )
	{
		args.rval().setInt32( 0 );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetPackOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns owner of container item is contained in (if any)
//o------------------------------------------------------------------------------------------------o
bool SE_GetPackOwner( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "GetPackOwner: Invalid number of arguments (takes 2)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI08 mType		= static_cast<UI08>( args.get( 1 ).toInt32());
	CChar *pOwner	= nullptr;

	if( mType == 0 )	// item
	{
		JSObject *mItem	= args.get( 0 ).toObjectOrNull();
		CItem *myItem	= JS::GetMaybePtrFromReservedSlot<CItem>( mItem, 0 );
		pOwner			= FindItemOwner( myItem );
	}
	else				// serial
	{
		SI32 mSerItem	= args.get( 0 ).toInt32();
		pOwner			= FindItemOwner( CalcItemObjFromSer( mSerItem ));
	}
	if( ValidateObject( pOwner ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_CHAR, pOwner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_FindRootContainer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns root container an item is contained in (if any)
//o------------------------------------------------------------------------------------------------o
bool SE_FindRootContainer( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "FindRootContainer: Invalid number of arguments (takes 2)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI08 mType		= static_cast<UI08>( args.get( 1 ).toInt32());
	CItem *iRoot	= nullptr;

	if( mType == 0 )	// item
	{
		JSObject *mItem	= args.get( 0 ).toObjectOrNull();
		CItem *myItem	= JS::GetMaybePtrFromReservedSlot<CItem>( mItem, 0 );
		iRoot			= FindRootContainer( myItem );
	}
	else				// serial
	{
		SI32 mSerItem	= args.get( 0 ).toInt32();
		iRoot			= FindRootContainer( CalcItemObjFromSer( mSerItem ));
	}
	if( ValidateObject( iRoot ))
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, iRoot, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcTargetedItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted item stored on socket
//o------------------------------------------------------------------------------------------------o
bool SE_CalcTargetedItem( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "CalcTargetedItem: Invalid number of arguments (takes 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *mysockptr = args.get( 0 ).toObjectOrNull();
	CSocket *sChar		= JS::GetMaybePtrFromReservedSlot<CSocket>( mysockptr, 0 );
	if( sChar == nullptr )
	{
		ScriptError( cx, "CalcTargetedItem: Invalid socket" );
		return false;
	}

	CItem *calcedItem = CalcItemObjFromSer( sChar->GetDWord( 7 ));
	if( !ValidateObject( calcedItem ))
	{
		args.rval().setNull();
	}
	else
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, calcedItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CalcTargetedChar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns targeted character stored on socket
//o------------------------------------------------------------------------------------------------o
bool SE_CalcTargetedChar( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "CalcTargetedChar: Invalid number of arguments (takes 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *mysockptr = args.get( 0 ).toObjectOrNull();
	CSocket *sChar		= JS::GetMaybePtrFromReservedSlot<CSocket>( mysockptr, 0 );
	if( sChar == nullptr )
	{
		ScriptError( cx, "CalcTargetedChar: Invalid socket" );
		return false;
	}

	CChar *calcedChar = CalcCharObjFromSer( sChar->GetDWord( 7 ));
	if( !ValidateObject( calcedChar ))
	{
		args.rval().setNull();
	}
	else
	{
		JSObject *myObj	= JSEngine->AcquireObject( IUE_CHAR, calcedChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
		args.rval().setObjectOrNull( myObj );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTileIdAtMapCoord()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the map tile ID at given coordinates
//o------------------------------------------------------------------------------------------------o
bool SE_GetTileIdAtMapCoord( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 )
	{
		ScriptError( cx, "GetTileIDAtMapCoord: Invalid number of arguments (takes 3)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 xLoc		= static_cast<UI16>( args.get( 0 ).toInt32());
	UI16 yLoc		= static_cast<UI16>( args.get( 1 ).toInt32());
	UI08 wrldNumber	= static_cast<UI08>( args.get( 2 ).toInt32());
	auto mMap		= Map->SeekMap( xLoc, yLoc, wrldNumber );
	args.rval().setInt32( mMap.tileId );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StaticInRange()
//|	Date		-	17th August, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static within specified range of given location
//o------------------------------------------------------------------------------------------------o
bool SE_StaticInRange( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 5 )
	{
		ScriptError( cx, "StaticInRange: Invalid number of arguments (takes 5, x, y, world, radius, tile)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 xLoc		= static_cast<UI16>( args.get( 0 ).toInt32());
	UI16 yLoc		= static_cast<UI16>( args.get( 1 ).toInt32());
	UI08 wrldNumber = static_cast<UI08>( args.get( 2 ).toInt32());
	UI16 radius		= static_cast<UI16>( args.get( 3 ).toInt32());
	UI16 tileId		= static_cast<UI16>( args.get( 4 ).toInt32());
	bool tileFound	= false;

	for( SI32 i = xLoc - radius; i <= ( xLoc + radius ); ++i )
	{
		for( SI32 j = yLoc - radius; j <= ( yLoc + radius ); ++j )
		{
			auto artwork = Map->ArtAt( xLoc, yLoc, wrldNumber );
			auto iter = std::find_if( artwork.begin(), artwork.end(), [tileId]( const Tile_st &tile )
			{
				return tile.tileId == tileId;
			});
			if( iter != artwork.end() )
			{
				tileFound = true;
				break;
			}
		}
		if( tileFound == true )
		{
			break;
		}
	}

	args.rval().setBoolean(  tileFound  );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StaticAt()
//|	Date		-	17th August, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks for static at specified location
//|	Notes		-	tile argument is optional; if not specified, will match ANY static found at location
//o------------------------------------------------------------------------------------------------o
bool SE_StaticAt( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 4 && argc != 3 )
	{
		ScriptError( cx, "StaticAt: Invalid number of arguments (takes 4, x, y, world, tile)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 xLoc		= static_cast<UI16>( args.get( 0 ).toInt32());
	UI16 yLoc		= static_cast<UI16>( args.get( 1 ).toInt32());
	UI08 wrldNumber = static_cast<UI08>( args.get( 2 ).toInt32());
	UI16 tileId		= 0xFFFF;
	 bool tileMatch	= false;
	if( argc == 4 )
	{
		tileId = static_cast<UI16>( args.get( 3 ).toInt32());
		tileMatch = true;
	}
	bool tileFound = false;

	auto artwork = Map->ArtAt( xLoc, yLoc, wrldNumber );
	auto iter = std::find_if( artwork.begin(), artwork.end(), [tileId]( const Tile_st &tile )
	{
		return tile.tileId == tileId;
	});
	tileFound = iter != artwork.end();
	args.rval().setBoolean( tileFound );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_StringToNum()
//|	Date		-	27th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts string to number
//o------------------------------------------------------------------------------------------------o
bool SE_StringToNum( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "StringToNum: Invalid number of arguments (takes 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string str = convertToString( cx, args.get( 0 ).toString() );

	args.rval().setInt32( std::stoi( str, nullptr, 0 ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_NumToString()
//|	Date		-	27th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts number to string
//o------------------------------------------------------------------------------------------------o
bool SE_NumToString( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "NumToString: Invalid number of arguments (takes 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 num = args.get( 0 ).toInt32();
	auto str = oldstrutil::number( num );
	args.rval().setString( JS_NewStringCopyZ( cx, str.c_str() ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_NumToHexString()
//|	Date		-	27th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts number to hex string
//o------------------------------------------------------------------------------------------------o
bool SE_NumToHexString( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "NumToHexString: Invalid number of arguments (takes 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 num = args.get( 0 ).toInt32();
	auto str = oldstrutil::number( num, 16 );

	args.rval().setString( JS_NewStringCopyZ( cx, str.c_str() ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetRaceCount()
//|	Date		-	November 9, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the total number of races found in the server
//o------------------------------------------------------------------------------------------------o
bool SE_GetRaceCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 0 )
	{
		ScriptError( cx, "GetRaceCount: Invalid number of arguments (takes 0)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( Races->Count() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_AreaCharacterFunction()
//|	Date		-	January 27, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area of characters
//o------------------------------------------------------------------------------------------------o
bool SE_AreaCharacterFunction( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 && argc != 4 )
	{
		// function name, source character, range
		ScriptError( cx, "AreaCharacterFunction: Invalid number of arguments (takes 3/4, function name, source character, range, optional socket)" );
		return false;
	}

	// Do parameter validation here
  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *srcSocketObj	= nullptr;
	CSocket *srcSocket		= nullptr;
	auto trgFunc			= convertToString( cx, args.get( 0 ).toString() );
	if( trgFunc.empty() )
	{
		ScriptError( cx, "AreaCharacterFunction: Argument 0 not a valid string" );
		return false;
	}

	JSObject *srcBaseObj = args.get( 1 ).toObjectOrNull();
	CBaseObject *srcObject = JS::GetMaybePtrFromReservedSlot<CBaseObject>( srcBaseObj, 0 );

	if( !ValidateObject( srcObject ))
	{
		ScriptError( cx, "AreaCharacterFunction: Argument 1 not a valid object" );
		return false;
	}
	R32 distance = static_cast<R32>( args.get( 2 ).toInt32());
	if( argc == 4 && !args.get( 3 ).isNull() )
	{
		srcSocketObj = args.get( 3 ).toObjectOrNull();
		srcSocket = JS::GetMaybePtrFromReservedSlot<CSocket>( srcSocketObj, 0 );
	}

	std::vector<CChar *> charsFound;
	UI16 retCounter	= 0;
	cScript *myScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
	for( auto &MapArea : MapRegion->PopulateList( srcObject ))
	{
		if( MapArea )
		{
			auto regChars = MapArea->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( ValidateObject( tempChar ))
				{
					if( ObjInRange( srcObject, tempChar, static_cast<UI16>( distance )))
					{
						// Store character reference for later
						charsFound.push_back( tempChar );
					}
				}
			}
		}
	}

	// Now iterate over all the characters that we found previously, and run the area function for each
	try
	{
		std::for_each( charsFound.begin(), charsFound.end(), [myScript, trgFunc, srcObject, srcSocket, &retCounter]( CChar *tempChar )
		{
			if( myScript->AreaObjFunc( trgFunc.c_str(), srcObject, tempChar, srcSocket ))
			{
				++retCounter;
			}
		});
	}
	catch( const std::out_of_range &e )
	{
		ScriptError( cx, oldstrutil::format( "Critical error encountered in AreaObjFunc!", e.what() ).c_str() );
	}

	args.rval().setInt32( retCounter );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_AreaItemFunction()
//|	Date		-	17th August, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Using a passed in function name, executes a JS function on an area of items
//o------------------------------------------------------------------------------------------------o
bool SE_AreaItemFunction( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 && argc != 4 )
	{
		// function name, source character, range
		ScriptError( cx, "AreaItemFunction: Invalid number of arguments (takes 3/4, function name, source object, range, optional socket)" );
		return false;
	}

	// Do parameter validation here
  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *srcSocketObj	= nullptr;
	CSocket *srcSocket		= nullptr;
	auto trgFunc			= convertToString( cx, args.get( 0 ).toString() );
	if( trgFunc.empty() )
	{
		ScriptError( cx, "AreaItemFunction: Argument 0 not a valid string" );
		return false;
	}

	JSObject *srcBaseObj = args.get( 1 ).toObjectOrNull();
	CBaseObject *srcObject = JS::GetMaybePtrFromReservedSlot<CBaseObject>( srcBaseObj, 0 );

	if( !ValidateObject( srcObject ))
	{
		ScriptError( cx, "AreaItemFunction: Argument 1 not a valid object" );
		return false;
	}
	R32 distance = static_cast<R32>( args.get( 2 ).toInt32());
	if( argc == 4 && !args.get( 3 ).isNull() )
	{
		srcSocketObj = args.get( 3 ).toObjectOrNull();
		if( srcSocketObj != nullptr )
		{
			srcSocket	= JS::GetMaybePtrFromReservedSlot<CSocket>( srcSocketObj, 0 );
		}
	}

	std::vector<CItem *> itemsFound;
	UI16 retCounter	= 0;
	cScript *myScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
	for( auto &MapArea : MapRegion->PopulateList( srcObject ))
	{
		if( MapArea )
		{
			auto regItems = MapArea->GetItemList();
			for( const auto &tempItem : regItems->collection() )
			{
				if( ValidateObject( tempItem ))
				{
					if( ObjInRange( srcObject, tempItem, static_cast<UI16>( distance )))
					{
						// Store item reference for later
						itemsFound.push_back( tempItem );
					}
				}
			}
		}
	}

	// Now iterate over all the characters that we found previously, and run the area function for each
	std::for_each( itemsFound.begin(), itemsFound.end(), [myScript, trgFunc, srcObject, srcSocket, &retCounter]( CItem *tempItem )
	{
		if( myScript->AreaObjFunc( trgFunc.c_str(), srcObject, tempItem, srcSocket ))
		{
			++retCounter;
		}
	});

	args.rval().setInt32( retCounter );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetDictionaryEntry()
//|	Date		-	7/17/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Allows the JSScripts to pull entries from the dictionaries and convert them to a string.
//o------------------------------------------------------------------------------------------------o
bool SE_GetDictionaryEntry( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 1 )
	{
		ScriptError( cx, "GetDictionaryEntry: Invalid number of arguments (takes at least 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 dictEntry = static_cast<SI32>( args.get( 0 ).toInt32());
	UnicodeTypes language = ZERO;
	if( argc == 2 )
	{
		language = static_cast<UnicodeTypes>( args.get( 1 ).toInt32());
	}
	std::string txt = Dictionary->GetEntry( dictEntry, language );
	txt = oldstrutil::stringToWstringToString( txt );

	JSString *strTxt = nullptr;
	strTxt = JS_NewStringCopyZ( cx, txt.c_str() );
	args.rval().setString( strTxt );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_Yell()
//|	Date		-	7/17/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Globally yell a message from JS (Based on Commandlevel)
//o------------------------------------------------------------------------------------------------o
bool SE_Yell( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 3 )
	{
		ScriptError( cx, "Yell: Invalid number of arguments (takes 3)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *mSock			= args.get( 0 ).toObjectOrNull();
	CSocket *mySock			= JS::GetMaybePtrFromReservedSlot<CSocket>( mSock, 0 );
	CChar *myChar			= mySock->CurrcharObj();
	std::string textToYell	= convertToString( cx, args.get( 1 ).toString() );
	UI08 commandLevel		= static_cast<UI08>( args.get( 2 ).toInt32());

	std::string yellTo = "";
	switch( static_cast<CommandLevels>( commandLevel ))
	{
		case CL_PLAYER:			yellTo = " (GLOBAL YELL): ";	break;
		case CL_CNS:			yellTo = " (CNS YELL): ";		break;
		case CL_GM:				yellTo = " (GM YELL): ";		break;
		case CL_ADMIN:			yellTo = " (ADMIN YELL): ";		break;
	}

	std::string tmpString = GetNpcDictName( myChar, mySock, NRS_SPEECH ) + yellTo + textToYell;

	if( cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( tmpString );
		unicodeMessage.Font( static_cast<FontType>( myChar->GetFontType() ));
		if( mySock->GetWord( 4 ) == 0x1700 )
		{
			unicodeMessage.Colour( 0x5A );
		}
		else if( mySock->GetWord( 4 ) == 0x0 )
		{
			unicodeMessage.Colour( 0x5A );
		}
		else
		{
			unicodeMessage.Colour( mySock->GetWord( 4 ));
		}
		unicodeMessage.Type( YELL );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( "SYSTEM" );
		unicodeMessage.ID( INVALIDID );
		unicodeMessage.Serial( INVALIDSERIAL );

		mySock->Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Speech( tmpString );
		toAdd.Font( static_cast<FontType>( myChar->GetFontType() ));
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
			toAdd.Colour( mySock->GetWord( 4 ));
		}
		toAdd.Type( SYSTEM );
		toAdd.At( cwmWorldState->GetUICurrentTime() );
		toAdd.TargType( SPTRG_BROADCASTALL );
		toAdd.CmdLevel( commandLevel );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_Reload()
//|	Date		-	29 Dec 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads certain server subsystems
//o------------------------------------------------------------------------------------------------o
bool SE_Reload( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "Reload: needs 1 argument!" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI32 toLoad = args.get( 0 ).toInt32();

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
		case 10: // Reload Dictionaries
			Dictionary->LoadDictionaries();
			break;
		default:
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SendStaticStats()
//|	Date		-	25th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Builds an info gump for specified static or map tile
//o------------------------------------------------------------------------------------------------o
bool SE_SendStaticStats( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "SendStaticStats: needs 1 argument!" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	JSObject *mSock			= args.get( 0 ).toObjectOrNull();
	CSocket *mySock			= JS::GetMaybePtrFromReservedSlot<CSocket>( mSock, 0 );
	if( mySock == nullptr )
	{
		ScriptError( cx, "SendStaticStats: passed an invalid socket!" );
		return false;
	}
	CChar *myChar			= mySock->CurrcharObj();
	if( !ValidateObject( myChar ))
		return true;

	if( mySock->GetDWord( 7 ) == 0 )
	{
		UI08 worldNumber	= myChar->WorldNumber();
		UI16 targetId		= mySock->GetWord( 0x11 );
		SI16 targetX		= mySock->GetWord( 0x0B );		// store our target x y and z locations
		SI16 targetY		= mySock->GetWord( 0x0D );
		SI08 targetZ		= mySock->GetByte( 0x10 );
		if( targetId != 0 )	// we might have a static rock or mountain
		{
			auto artwork = Map->ArtAt( targetX, targetY, worldNumber );
			for( auto &tile : artwork )
			{
				if( targetZ == tile.altitude )
				{
					CGumpDisplay staticStat( mySock, 300, 300 );
					staticStat.SetTitle( "Item [Static]" );
					staticStat.AddData( "ID", targetId, 5 );
					staticStat.AddData( "Height", tile.height() );
					staticStat.AddData( "Name", tile.artInfo->Name() );
					staticStat.Send( 4, false, INVALIDSERIAL );
				}
			}
		}
		else // or it could be a map only
		{
			// manually calculating the ID's if a maptype
			auto map1 = Map->SeekMap( targetX, targetY, worldNumber );
			CGumpDisplay mapStat( mySock, 300, 300 );
			mapStat.SetTitle( "Item [Map]" );
			mapStat.AddData( "ID", targetId, 5 );
			mapStat.AddData( "Name", map1.name() );
			mapStat.Send( 4, false, INVALIDSERIAL );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTileHeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the tile height of a specified tile (item)
//o------------------------------------------------------------------------------------------------o
bool SE_GetTileHeight( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "GetTileHeight: needs 1 argument!" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 tileNum = static_cast<UI16>( args.get( 0 ).toInt32());
	args.rval().setInt32( Map->TileHeight( tileNum ));
	return true;
}

bool SE_IterateFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	cScript *myScript = static_cast<cScript *>( extraData );
	return myScript->OnIterate( a, b );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IterateOver()
//|	Date		-	July 25th, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all world objects
//o------------------------------------------------------------------------------------------------o
bool SE_IterateOver( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "IterateOver: needs 1 argument!" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI32 b				= 0;
	std::string objType = convertToString( cx, args.get( 0 ).toString() );
	ObjectType toCheck	= FindObjTypeFromString( objType );
	cScript *myScript	= JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));
	if( myScript != nullptr )
	{
		ObjectFactory::GetSingleton().IterateOver( toCheck, b, myScript, &SE_IterateFunctor );
	}

	JS_MaybeGC( cx );

	args.rval().setInt32( b );
	return true;
}

bool SE_IterateSpawnRegionsFunctor( CSpawnRegion *a, UI32 &b, void *extraData )
{
	cScript *myScript = static_cast<cScript *>( extraData );
	return myScript->OnIterateSpawnRegions( a, b );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IterateOverSpawnRegions()
//|	Date		-	July, 2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops over all spawn regions in the world
//o------------------------------------------------------------------------------------------------o
bool SE_IterateOverSpawnRegions( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	UI32 b = 0;
	cScript *myScript = JSMapping->GetScript( JS::CurrentGlobalOrNull( cx ));

	if( myScript != nullptr )
	{
		std::for_each( cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(), [&myScript, &b]( std::pair<UI16, CSpawnRegion*> entry )
		{
			if( entry.second )
			{
				SE_IterateSpawnRegionsFunctor( entry.second, b, myScript );
			}
		});
	}

	JS_MaybeGC( cx );

	args.rval().setInt32( b );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldBrightLevel()
//|	Date		-	18th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world bright level - brightest part of the regular day/night cycle
//o------------------------------------------------------------------------------------------------o
bool SE_WorldBrightLevel( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc > 1 )
	{
		ScriptError( cx, oldstrutil::format( "WorldBrightLevel: Unknown Count of Arguments: %d", argc ).c_str() );
		return false;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL brightLevel = static_cast<LIGHTLEVEL>( args.get( 0 ).toInt32());
		cwmWorldState->ServerData()->WorldLightBrightLevel( brightLevel );
	}
	args.rval().setInt32( cwmWorldState->ServerData()->WorldLightBrightLevel() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldDarkLevel()
//|	Date		-	18th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets world dark level - darkest part of the regular day/night cycle
//o------------------------------------------------------------------------------------------------o
bool SE_WorldDarkLevel( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc > 1 )
	{
		ScriptError( cx, oldstrutil::format( "WorldDarkLevel: Unknown Count of Arguments: %d", argc ).c_str() );
		return false;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL darkLevel = static_cast<LIGHTLEVEL>( args.get( 0 ).toInt32());
		cwmWorldState->ServerData()->WorldLightDarkLevel( darkLevel );
	}
	args.rval().setInt32( cwmWorldState->ServerData()->WorldLightDarkLevel() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WorldDungeonLevel()
//|	Date		-	18th July, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets and sets default light level in dungeons
//o------------------------------------------------------------------------------------------------o
bool SE_WorldDungeonLevel( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc > 1 )
	{
		ScriptError( cx, oldstrutil::format( "WorldDungeonLevel: Unknown Count of Arguments: %d", argc ).c_str() );
		return false;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL dungeonLevel = static_cast<LIGHTLEVEL>( args.get( 0 ).toInt32());
		cwmWorldState->ServerData()->DungeonLightLevel( dungeonLevel );
	}
	args.rval().setInt32( cwmWorldState->ServerData()->DungeonLightLevel() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegionFacetStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets enabled state of given spawn region
//o------------------------------------------------------------------------------------------------o
bool SE_GetSpawnRegionFacetStatus( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc > 1 )
	{
		ScriptError( cx, oldstrutil::format( "GetSpawnRegionFacetStatus: Unknown Count of Arguments: %d", argc ).c_str() );
		return false;
	}
	else if( argc == 1 )
	{
		UI32 spawnRegionFacet = static_cast<UI32>( args.get( 0 ).toInt32());
		bool spawnRegionFacetStatus = cwmWorldState->ServerData()->GetSpawnRegionsFacetStatus( spawnRegionFacet );
		if( spawnRegionFacetStatus )
		{
			args.rval().setInt32( 1 );
		}
		else
		{
			args.rval().setInt32( 0 );
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_SetSpawnRegionFacetStatus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets enabled state of spawn regions
//o------------------------------------------------------------------------------------------------o
bool SE_SetSpawnRegionFacetStatus( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc > 2 )
	{
		ScriptError( cx, oldstrutil::format( "SetSpawnRegionFacetStatus: Unknown Count of Arguments: %d", argc ).c_str() );
		return false;
	}
	else if( argc == 1 )
	{
		UI32 spawnRegionFacetVal = static_cast<UI32>( args.get( 0 ).toInt32());
		cwmWorldState->ServerData()->SetSpawnRegionsFacetStatus( spawnRegionFacetVal );
	}
	else if( argc == 2 )
	{
		UI32 spawnRegionFacet = static_cast<UI32>( args.get( 0 ).toInt32());
		bool spawnRegionFacetStatus = ( args.get( 1 ).toBoolean() );
		cwmWorldState->ServerData()->SetSpawnRegionsFacetStatus( spawnRegionFacet, spawnRegionFacetStatus );
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSocketFromIndex()
//|	Date		-	3rd August, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns socket based on provided index, from list of connected clients
//o------------------------------------------------------------------------------------------------o
bool SE_GetSocketFromIndex( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "GetSocketFromIndex: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	UOXSOCKET index = static_cast<UOXSOCKET>( args.get( 0 ).toInt32());

	CSocket *mSock = Network->GetSockPtr( index );
	CChar *mChar = nullptr;
	if( mSock != nullptr )
	{
		mChar = mSock->CurrcharObj();
	}

	if( !ValidateObject( mChar ))
	{
		args.rval().setNull();
		return false;
	}

	JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, mChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
	args.rval().setObjectOrNull( myObj );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ReloadJSFile()
//|	Date		-	5th February, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload specified JS file by scriptId
//o------------------------------------------------------------------------------------------------o
bool SE_ReloadJSFile( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "ReloadJSFile: Invalid number of arguments (takes 1)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 scriptId = static_cast<UI16>( args.get( 0 ).toInt32());
	if( scriptId == JSMapping->GetScriptId( JS::CurrentGlobalOrNull( cx )))
	{
		ScriptError( cx, oldstrutil::format( "ReloadJSFile: JS Script attempted to reload itself, crash avoided (ScriptID %u)", scriptId ).c_str() );
		return false;
	}

	JSMapping->Reload( scriptId );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceArea()
//|	Date		-	18th September, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resource areas to split the world into
//o------------------------------------------------------------------------------------------------o
bool SE_ResourceArea( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 0 )
	{
		ScriptError( cx, oldstrutil::format( "ResourceArea: Invalid Count of Arguments: %d", argc ).c_str() );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	auto resType = std::string( convertToString( cx, args.get( 0 ).toString() ));
	resType = oldstrutil::upper( oldstrutil::trim( oldstrutil::removeTrailing( resType, "//" )));

	args.rval().setInt32( cwmWorldState->ServerData()->ResourceAreaSize() );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceAmount()
//|	Date		-	18th September, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of resources (logs/ore/fish) in each resource area on the server
//o------------------------------------------------------------------------------------------------o
bool SE_ResourceAmount( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc > 2 || argc == 0 )
	{
		ScriptError( cx, oldstrutil::format( "ResourceAmount: Invalid Count of Arguments: %d", argc ).c_str() );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	auto resType = std::string( convertToString( cx, args.get( 0 ).toString() ));
	resType = oldstrutil::upper( oldstrutil::trim( oldstrutil::removeTrailing( resType, "//" )));

	if( argc == 2 )
	{
		SI16 newVal = static_cast<SI16>( args.get( 1 ).toInt32());
		if( resType == "LOGS" )
		{
			cwmWorldState->ServerData()->ResLogs( newVal );
		}
		else if( resType == "ORE" )
		{
			cwmWorldState->ServerData()->ResOre( newVal );
		}
		else if( resType == "FISH" )
		{
			cwmWorldState->ServerData()->ResFish( newVal );
		}
	}

	if( resType == "LOGS" )
	{
		args.rval().setInt32( cwmWorldState->ServerData()->ResLogs() );
	}
	else if( resType == "ORE" )
	{
		args.rval().setInt32( cwmWorldState->ServerData()->ResOre() );
	}
	else if( resType == "FISH" )
	{
		args.rval().setInt32( cwmWorldState->ServerData()->ResFish() );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceTime()
//|	Date		-	18th September, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets respawn timers for ore/log resources on server
//o------------------------------------------------------------------------------------------------o
bool SE_ResourceTime( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc > 2 || argc == 0 )
	{
		ScriptError( cx, oldstrutil::format( "ResourceTime: Invalid Count of Arguments: %d", argc ).c_str() );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	auto resType = std::string( convertToString( cx, args.get( 0 ).toString() ));
	resType = oldstrutil::upper( oldstrutil::trim( oldstrutil::removeTrailing( resType, "//" )));
	if( argc == 2 )
	{
		UI16 newVal = static_cast<UI16>( args.get( 1 ).toInt32());
		if( resType == "LOGS" )
		{
			cwmWorldState->ServerData()->ResLogTime( newVal );
		}
		else if( resType == "ORE" )
		{
			cwmWorldState->ServerData()->ResOreTime( newVal );
		}
		else if( resType == "FISH" )
		{
			cwmWorldState->ServerData()->ResFishTime( newVal );
		}
	}

	if( resType == "LOGS" )
	{
		args.rval().setInt32( cwmWorldState->ServerData()->ResLogTime() );
	}
	else if( resType == "ORE" )
	{
		args.rval().setInt32( cwmWorldState->ServerData()->ResOreTime() );
	}
	else if( resType == "FISH" )
	{
		args.rval().setInt32( cwmWorldState->ServerData()->ResFishTime() );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ResourceRegion()
//|	Date		-	18th September, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a resource object allowing JS to modify resource data.
//o------------------------------------------------------------------------------------------------o
bool SE_ResourceRegion( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 )
	{
		ScriptError( cx, "ResourceRegion: Invalid number of arguments (takes 3)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 2 ).toInt32());
	MapResource_st *mRes = MapRegion->GetResource( x, y, worldNum );
	if( mRes == nullptr )
	{
		ScriptError( cx, "ResourceRegion: Invalid Resource Region" );
		return false;
	}

	JSObject *jsResource = JS_NewObject( cx, &UOXResource_class );
  JS::RootedObject root(cx, jsResource);
	JS::MutableHandleObject rJSResource( &root );
	JS_DefineProperties( cx, rJSResource, CResourceProperties );
  JS::SetReservedSlot( jsResource, 0, JS::PrivateValue(mRes) );

	args.rval().setObject( (*jsResource) );

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ValidateObject()
//|	Date		-	26th January, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if object is a valid and not nullptr or marked for deletion
//o------------------------------------------------------------------------------------------------o
bool SE_ValidateObject( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "ValidateObject: Invalid number of arguments (takes 1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	JSEncapsulate myClass( cx, &( argv[0] ));

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXItem" )
	{
		CBaseObject *myObj = static_cast<CBaseObject *>( myClass.toObject() );
		args.rval().setBoolean( ValidateObject( myObj ));
	}
	else
	{
		args.rval().setBoolean( false );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ApplyDamageBonuses()
//|	Date		-	17th March, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns damage bonuses based on race/weather weakness and character skills
//o------------------------------------------------------------------------------------------------o
bool SE_ApplyDamageBonuses( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 6 )
	{
		ScriptError( cx, "ApplyDamageBonuses: Invalid number of arguments (takes 6)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	CChar *attacker	= nullptr, *defender = nullptr;
	SI16 damage = 0;

	JSEncapsulate damageType( cx, &( argv[0] ));
	JSEncapsulate getFightSkill( cx, &( argv[3] ));
	JSEncapsulate hitLoc( cx, &( argv[4] ));
	JSEncapsulate baseDamage( cx, &(argv [5] ));

	JSEncapsulate attackerClass( cx, &( argv[1] ));
	if( attackerClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		ScriptError( cx, "ApplyDamageBonuses: Passed an invalid Character" );
		return false;
	}

	if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ))
	{
		ScriptError( cx, "ApplyDamageBonuses: Passed an invalid Character" );
		return true;
	}
	else
	{
		attacker = static_cast<CChar *>( attackerClass.toObject() );
		if( !ValidateObject( attacker )  )
		{
			ScriptError( cx, "ApplyDamageBonuses: Passed an invalid Character" );
			return true;
		}
	}

	JSEncapsulate defenderClass( cx, &( argv[2] ));
	if( defenderClass.ClassName() != "UOXChar" ) // It must be a character!
	{
		ScriptError( cx, "ApplyDamageBonuses: Passed an invalid Character" );
		return false;
	}

	if( defenderClass.isType( JSOT_VOID ) || defenderClass.isType( JSOT_NULL ))
	{
		ScriptError( cx, "ApplyDamageBonuses: Passed an invalid Character" );
		return true;
	}
	else
	{
		defender = static_cast<CChar *>( defenderClass.toObject() );
		if( !ValidateObject( defender ))
		{
			ScriptError( cx, "ApplyDamageBonuses: Passed an invalid Character" );
			return true;
		}
	}

	damage = Combat->ApplyDamageBonuses( static_cast<WeatherType>( damageType.toInt() ), attacker, 
				defender, static_cast<UI08>( getFightSkill.toInt() ), static_cast<UI08>( hitLoc.toInt() ), static_cast<SI16>( baseDamage.toInt() ));

	args.rval().setInt32( damage );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_ApplyDefenseModifiers()
//|	Date		-	17th March, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns defense modifiers based on shields/parrying, armor values and elemental damage
//o------------------------------------------------------------------------------------------------o
bool SE_ApplyDefenseModifiers( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 7 )
	{
		ScriptError( cx, "ApplyDefenseModifiers: Invalid number of arguments (takes 7)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	CChar *attacker	= nullptr, *defender = nullptr;
	SI16 damage = 0;

	JSEncapsulate damageType( cx, &( argv[0] ));
	JSEncapsulate getFightSkill( cx, &( argv[3] ));
	JSEncapsulate hitLoc( cx, &( argv[4] ));
	JSEncapsulate baseDamage( cx, &( argv[5] ));
	JSEncapsulate doArmorDamage(cx, &( argv[6] ));

	JSEncapsulate attackerClass( cx, &( argv[1] ));
	if( attackerClass.ClassName() == "UOXChar" )
	{
		if( attackerClass.isType( JSOT_VOID ) || attackerClass.isType( JSOT_NULL ))
		{
			attacker = nullptr;
		}
		else
		{
			attacker = static_cast<CChar *>( attackerClass.toObject() );
			if( !ValidateObject( attacker ))
			{
				attacker = nullptr;
			}
		}
	}

	JSEncapsulate defenderClass( cx, &( argv[2] ));
	if( defenderClass.ClassName() != "UOXChar" )	// It must be a character!
	{
		ScriptError( cx, "ApplyDefenseModifiers: Passed an invalid Character" );
		return false;
	}

	if( defenderClass.isType( JSOT_VOID ) || defenderClass.isType( JSOT_NULL ))
	{
		ScriptError( cx, "ApplyDefenseModifiers: Passed an invalid Character" );
		return true;
	}
	else
	{
		defender = static_cast<CChar *>( defenderClass.toObject() );
		if( !ValidateObject( defender ))
		{
			ScriptError( cx, "ApplyDefenseModifiers: Passed an invalid Character" );
			return true;
		}
	}

	damage = Combat->ApplyDefenseModifiers( static_cast<WeatherType>( damageType.toInt() ), attacker,
			defender, static_cast<UI08>( getFightSkill.toInt() ), static_cast<UI08>( hitLoc.toInt() ),
			static_cast<SI16>( baseDamage.toInt() ), doArmorDamage.toBool() );

	args.rval().setInt32( damage );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_WillResultInCriminal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if hostile action done by one character versus another will result in criminal flag
//o------------------------------------------------------------------------------------------------o
bool SE_WillResultInCriminal( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "WillResultInCriminal: Invalid number of arguments (takes 2: srcChar and trgChar)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	bool result = false;
	CChar *srcChar = nullptr;
	CChar *trgChar = nullptr;

	if( !args.get( 0 ).isNull() && !args.get( 1 ).isNull() )
	{
		JSObject *srcCharObj = args.get( 0 ).toObjectOrNull();
		srcChar = JS::GetMaybePtrFromReservedSlot<CChar>( srcCharObj, 0 );

		JSObject *trgCharObj = args.get( 1 ).toObjectOrNull();
		trgChar = JS::GetMaybePtrFromReservedSlot<CChar>( trgCharObj, 0 );

		if( ValidateObject( srcChar ) && ValidateObject( trgChar ))
		{
			result = WillResultInCriminal( srcChar, trgChar );
		}
	}
	else
	{
		ScriptError( cx, "WillResultInCriminal: Invalid objects passed - characters required)" );
		return false;
	}

	args.rval().setBoolean( result );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CreateParty()
//|	Date		-	21st September, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create a party with specified character as the party leader
//o------------------------------------------------------------------------------------------------o
bool SE_CreateParty( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "CreateParty: Invalid number of arguments (takes 1, the leader)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	JSEncapsulate myClass( cx, &( argv[0] ));

	if( myClass.ClassName() == "UOXChar" || myClass.ClassName() == "UOXSocket" )
	{	// it's a character or socket, fantastic
		CChar *leader		= nullptr;
		CSocket *leaderSock	= nullptr;
		if( myClass.ClassName() == "UOXChar" )
		{
			leader		= static_cast<CChar *>( myClass.toObject() );
			leaderSock	= leader->GetSocket();
		}
		else
		{
			leaderSock	= static_cast<CSocket *>( myClass.toObject() );
			leader		= leaderSock->CurrcharObj();
		}

		if( PartyFactory::GetSingleton().Get( leader ) != nullptr )
		{
			args.rval().setNull();
		}
		else
		{
			Party *tParty	= PartyFactory::GetSingleton().Create( leader );
			JSObject *myObj	= JSEngine->AcquireObject( IUE_PARTY, tParty, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
			args.rval().setObjectOrNull( myObj );
		}
	}
	else	// anything else isn't a valid leader people
	{
		args.rval().setNull();
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_Moon()
//|	Date		-	25th May, 2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets or sets Moon phases on server
//o------------------------------------------------------------------------------------------------o
bool SE_Moon( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc > 2 || argc == 0 )
	{
		ScriptError( cx, oldstrutil::format( "Moon: Invalid Count of Arguments: %d", argc ).c_str() );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 slot = static_cast<SI16>( args.get( 0 ).toInt32());
	if( argc == 2 )
	{
		SI16 newVal = static_cast<SI16>( args.get( 1 ).toInt32());
		cwmWorldState->ServerData()->ServerMoon( slot, newVal );
	}

	args.rval().setInt32( cwmWorldState->ServerData()->ServerMoon( slot ));

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetTownRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified region object
//o------------------------------------------------------------------------------------------------o
bool SE_GetTownRegion( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "GetTownRegion: Invalid number of parameters (1)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 regNum = static_cast<UI16>( args.get( 0 ).toInt32());
	if( cwmWorldState->townRegions.find( regNum ) != cwmWorldState->townRegions.end() )
	{
		CTownRegion *townReg = cwmWorldState->townRegions[regNum];
		if( townReg != nullptr )
		{
			JSObject *myObj = JSEngine->AcquireObject( IUE_REGION, townReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
			args.rval().setObjectOrNull( myObj );
		}
		else
		{
			args.rval().setNull();
		}
	}
	else
	{
		args.rval().setNull();
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegion()
//|	Date		-	June 22, 2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a specified spawn region object
//o------------------------------------------------------------------------------------------------o
bool SE_GetSpawnRegion( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 && argc != 4 )
	{
		ScriptError( cx, "GetSpawnRegion: Invalid number of parameters (1 - spawnRegionID, or 4 - x, y, world and instanceID)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc == 1 )
	{
		// Assume spawn region number was provided
		UI16 spawnRegNum = static_cast<UI16>( args.get( 0 ).toInt32());
		if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
		{
			CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
			if( spawnReg != nullptr )
			{
				JSObject *myObj = JSEngine->AcquireObject( IUE_SPAWNREGION, spawnReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
				args.rval().setObjectOrNull( myObj );
			}
			else
			{
				args.rval().setNull();
			}
		}
		else
		{
			args.rval().setNull();
		}
	}
	else
	{
		// Assume coordinates were provided
		UI16 x = static_cast<UI16>( args.get( 0 ).toInt32());
		UI16 y = static_cast<UI16>( args.get( 1 ).toInt32());
		UI08 worldNum = static_cast<UI08>( args.get( 2 ).toInt32());
		UI16 instanceID = static_cast<UI16>( args.get( 3 ).toInt32());

		// Iterate over each spawn region to find the right one
		auto iter = std::find_if( cwmWorldState->spawnRegions.begin(), cwmWorldState->spawnRegions.end(), [&x, &y, &worldNum, &instanceID, &cx, &args]( std::pair<UI16, CSpawnRegion*> entry )
		{
			if( entry.second && x >= entry.second->GetX1() && x <= entry.second->GetX2() && y >= entry.second->GetY1()
				&& y <= entry.second->GetY2() && entry.second->GetInstanceId() == instanceID && entry.second->WorldNumber() == worldNum )
			{
				JSObject *myObj = JSEngine->AcquireObject( IUE_SPAWNREGION, entry.second, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
				args.rval().setObjectOrNull( myObj );
				return true;
			}
			return false;
		});

		if( iter == cwmWorldState->spawnRegions.end() )
		{
			args.rval().setNull();
		}
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetSpawnRegionCount()
//|	Date		-	June 22, 2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the total number of spawn regions found in the server
//o------------------------------------------------------------------------------------------------o
bool SE_GetSpawnRegionCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 0 )
	{
		ScriptError( cx, "GetSpawnRegionCount: Invalid number of arguments (takes 0)" );
		return false;
	}
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( cwmWorldState->spawnRegions.size() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMapElevation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns map elevation at given coordinates
//o------------------------------------------------------------------------------------------------o
bool SE_GetMapElevation( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 3 )
	{
		ScriptError( cx, "GetMapElevation: Invalid number of arguments (takes 3: X, Y and WorldNumber)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 2 ).toInt32());
	SI08 mapElevation = Map->MapElevation( x, y, worldNum );
	args.rval().setInt32( mapElevation );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_IsInBuilding()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified location is inside a building
//| Notes		-	First checks if player is in a static building. If false, checks if there's a multi
//|					at the same location as the player, and assumes they are in the building if true
//o------------------------------------------------------------------------------------------------o
bool SE_IsInBuilding( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 6 )
	{
		ScriptError( cx, "IsInBuilding: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber and instanceId)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z			= static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 3 ).toInt32());
	UI16 instanceId = static_cast<UI16>( args.get( 4 ).toInt32());
	bool checkHeight = ( args.get( 5 ).toBoolean() );
	bool isInBuilding = Map->InBuilding( x, y, z, worldNum, instanceId );
	if( !isInBuilding )
	{
		// No static building was detected. How about a multi?
		CMultiObj *multi = FindMulti( x, y, z, worldNum, instanceId );
		if( ValidateObject( multi ))
		{
			if( checkHeight )
			{
				// Check if there's multi-items over the player's head
				SI08 multiZ = Map->MultiHeight( multi, x, y, z, 127, checkHeight );
				if( multiZ > z )
				{
					isInBuilding = true;
				}
			}
			else
			{
				isInBuilding = true;
			}
		}
	}
	args.rval().setBoolean( isInBuilding );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckStaticFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any statics at given coordinates has a specific flag
//o------------------------------------------------------------------------------------------------o
bool SE_CheckStaticFlag( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 5 )
	{
		ScriptError( cx, "CheckStaticFlag: Invalid number of arguments (takes 5: X, Y, Z, WorldNumber and TileFlagID)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z			= static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 3 ).toInt32());
	TileFlags toCheck	= static_cast<TileFlags>( args.get( 4 ).toInt32());
	 UI16 ignoreMe = 0;
	bool hasStaticFlag = Map->CheckStaticFlag( x, y, z, worldNum, toCheck, ignoreMe, false );
	args.rval().setBoolean( hasStaticFlag );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckDynamicFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether any dynamics at given coordinates has a specific flag
//o------------------------------------------------------------------------------------------------o
bool SE_CheckDynamicFlag( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 6 )
	{
		ScriptError( cx, "CheckDynamicFlag: Invalid number of arguments (takes 6: X, Y, Z, WorldNumber, instanceId and TileFlagID)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x = static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y = static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z = static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum = static_cast<UI08>( args.get( 3 ).toInt32());
	UI08 instanceId = static_cast<UI08>( args.get( 4 ).toInt32());
	TileFlags toCheck = static_cast<TileFlags>( args.get( 5 ).toInt32());
	 UI16 ignoreMe = 0;
	bool hasDynamicFlag = Map->CheckDynamicFlag( x, y, z, worldNum, instanceId, toCheck, ignoreMe );
	args.rval().setBoolean( hasDynamicFlag );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_CheckTileFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks to see whether tile with given ID has a specific flag
//o------------------------------------------------------------------------------------------------o
bool SE_CheckTileFlag( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 )
	{
		ScriptError( cx, "CheckTileFlag: Invalid number of arguments (takes 2: itemId and tileFlagID)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	UI16 itemId = static_cast<UI16>( args.get( 0 ).toInt32());
	TileFlags flagToCheck	= static_cast<TileFlags>( args.get( 1 ).toInt32());

	bool tileHasFlag = Map->CheckTileFlag( itemId, flagToCheck );
	args.rval().setBoolean( tileHasFlag );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesStaticBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if statics at/above given coordinates blocks movement, etc
//o------------------------------------------------------------------------------------------------o
bool SE_DoesStaticBlock( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 5 )
	{
		ScriptError( cx, "DoesStaticBlock: Invalid number of arguments (takes 5: X, Y, Z, WorldNumber and checkWater)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z			= static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 3 ).toInt32());
	bool checkWater = ( args.get( 4 ).toBoolean() );
	bool staticBlocks = Map->DoesStaticBlock( x, y, z, worldNum, checkWater );
	args.rval().setBoolean( staticBlocks );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesDynamicBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if dynamics at/above given coordinates blocks movement, etc
//o------------------------------------------------------------------------------------------------o
bool SE_DoesDynamicBlock( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 9 )
	{
		ScriptError( cx, "DoesDynamicBlock: Invalid number of arguments (takes 9: X, Y, Z, WorldNumber, instanceId, checkWater, waterWalk, checkOnlyMultis and checkOnlyNonMultis)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z			= static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 3 ).toInt32());
	UI08 instanceId	= static_cast<UI08>( args.get( 4 ).toInt32());
	bool checkWater = ( args.get( 5 ).toBoolean() );
	bool waterWalk = ( args.get( 6 ).toBoolean() );
	bool checkOnlyMultis = ( args.get( 7 ).toBoolean() );
	bool checkOnlyNonMultis = ( args.get( 8 ).toBoolean() );
	bool dynamicBlocks = Map->DoesDynamicBlock( x, y, z, worldNum, instanceId, checkWater, waterWalk, checkOnlyMultis, checkOnlyNonMultis );
	args.rval().setBoolean( dynamicBlocks );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesMapBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if map tile at/above given coordinates blocks movement, etc
//o------------------------------------------------------------------------------------------------o
bool SE_DoesMapBlock( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 8 )
	{
		ScriptError( cx, "DoesMapBlock: Invalid number of arguments (takes 8: X, Y, Z, WorldNumber, checkWater, waterWalk, checkMultiPlacement and checkForRoad)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z			= static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 3 ).toInt32());
	bool checkWater = ( args.get( 4 ).toBoolean() );
	bool waterWalk = ( args.get( 5 ).toBoolean() );
	bool checkMultiPlacement = ( args.get( 6 ).toBoolean() );
	bool checkForRoad = ( args.get( 7 ).toBoolean() );
	bool mapBlocks = Map->DoesMapBlock( x, y, z, worldNum, checkWater, waterWalk, checkMultiPlacement, checkForRoad );
	args.rval().setBoolean( mapBlocks );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DoesCharacterBlock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if characters at/above given coordinates blocks movement, etc
//o------------------------------------------------------------------------------------------------o
bool SE_DoesCharacterBlock( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 5 )
	{
		ScriptError( cx, "DoesCharacterBlock: Invalid number of arguments (takes 5: X, Y, z, WorldNumber, instanceId)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	SI16 x			= static_cast<SI16>( args.get( 0 ).toInt32());
	SI16 y			= static_cast<SI16>( args.get( 1 ).toInt32());
	SI08 z			= static_cast<SI08>( args.get( 2 ).toInt32());
	UI08 worldNum	= static_cast<UI08>( args.get( 3 ).toInt32());
	UI08 instanceId	= static_cast<UI08>( args.get( 4 ).toInt32());
	bool characterBlocks = Map->DoesCharacterBlock( x, y, z, worldNum, instanceId );
	args.rval().setBoolean( characterBlocks );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DeleteFile()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes a file from the server's harddrive...
//o------------------------------------------------------------------------------------------------o
bool SE_DeleteFile( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc < 1 || argc > 3 )
	{
		ScriptError( cx, "DeleteFile: Invalid number of arguments (takes 1 to 3 - fileName and (optionally) subFolderName and useScriptDataDir bool)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	auto fileName = convertToString( cx, args.get( 0 ).toString() );
	std::string subFolderName = nullptr;
	if( argc >= 2 )
	{
		subFolderName = convertToString( cx, args.get( 1 ).toString() );
	}
	bool useScriptDataDir = false;
	if( argc >= 3 )
	{
		useScriptDataDir = ( args.get( 2 ).toBoolean() );
	}

	if( strstr( fileName.c_str(), ".." ) || strstr( fileName.c_str(), "\\" ) || strstr( fileName.c_str(), "/" ))
	{
		ScriptError( cx, "DeleteFile: file names may not contain \".\", \"..\", \"\\\", or \"/\"." );
		return false;
	}

	std::string pathString	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED );

	// if subFolderName argument was supplied, use it
	if( !subFolderName.empty() )
	{
		// However, don't allow special characters in the folder name
		if( strstr( subFolderName.c_str(), ".." ) || strstr( subFolderName.c_str(), "\\" ) || strstr( subFolderName.c_str(), "/" ))
		{
			ScriptError( cx, "DeleteFile: folder names may not contain \".\", \"..\", \"\\\", or \"/\"." );
			return false;
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
      args.rval().setBoolean(false);
			return true;
		}

		pathString += std::filesystem::path::preferred_separator;
	}

	pathString.append( fileName );

	std::filesystem::path filePath = pathString;
	args.rval().setBoolean( std::filesystem::remove( filePath ) );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_EraStringToNum()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Converts an UO era string to an int value for easier comparison in JavaScripts
//o------------------------------------------------------------------------------------------------o
bool SE_EraStringToNum( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
  args.rval().setNull();

	if( argc != 1 )
	{
		ScriptError( cx, "EraStringToNum: Invalid number of arguments (takes 1 - era string)" );
		return false;
	}

	std::string eraString = oldstrutil::upper( convertToString( cx, args.get( 0 ).toString() ));
	if( !eraString.empty() )
	{
		UI08 eraNum = static_cast<UI08>( cwmWorldState->ServerData()->EraStringToEnum( eraString, false, false ));
		if( eraNum != 0 )
		{
			args.rval().setInt32( eraNum );
		}
		else
		{
			ScriptError( cx, "EraStringToNum: Provided argument not valid era string (uo, t2a, uor, td, lbr, aos, se, ml, sa, hs or tol)" );
			return false;
		}
	}
	else
	{
		ScriptError( cx, "EraStringToNum: Provided argument contained no valid string data" );
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerSetting()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets value of specified server setting
//o------------------------------------------------------------------------------------------------o
bool SE_GetServerSetting( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
  args.rval().setNull();

	if( argc != 1 )
	{
		ScriptError( cx, "GetServerSetting: Invalid number of arguments (takes 1 - serverSettingName)" );
		return false;
	}

	JSString *tString;
	std::string settingName = oldstrutil::upper( convertToString( cx, args.get( 0 ).toString() ));
	if( !settingName.empty() )
	{
		auto settingId = cwmWorldState->ServerData()->LookupINIValue( settingName );
		switch( settingId )
		{
			case 1:	 // SERVERNAME
			{
				std::string tempString = { cwmWorldState->ServerData()->ServerName() };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 2:	 // CONSOLELOG
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerConsoleLog() );
				break;
			case 3:	 // COMMANDPREFIX
			{
				std::string tempString = { cwmWorldState->ServerData()->ServerCommandPrefix() };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 4:	 // ANNOUNCEWORLDSAVES
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerAnnounceSavesStatus() );
				break;
			case 26: // JOINPARTMSGS
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerJoinPartAnnouncementsStatus() );
				break;
			case 5:	 // BACKUPSENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerBackupStatus() );
				break;
			case 6:	 // SAVESTIMER
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->ServerSavesTimerStatus() ));
				break;
			case 7:	 // SKILLCAP
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ServerSkillTotalCapStatus() ));
				break;
			case 8:	 // SKILLDELAY
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->ServerSkillDelayStatus() ));
				break;
			case 9:	 // STATCAP
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ServerStatCapStatus() ));
				break;
			case 10:	 // MAXSTEALTHMOVEMENTS
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->MaxStealthMovement() ));
				break;
			case 11:	 // MAXSTAMINAMOVEMENTS
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->MaxStaminaMovement() ));
				break;
			case 12:	 // ARMORAFFECTMANAREGEN
				args.rval().setBoolean( cwmWorldState->ServerData()->ArmorAffectManaRegen() );
				break;
			case 13:	 // CORPSEDECAYTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_CORPSEDECAY )));
				break;
			case 14:	 // WEATHERTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_WEATHER )));
				break;
			case 15:	 // SHOPSPAWNTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_SHOPSPAWN )));
				break;
			case 16:	 // DECAYTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_DECAY )));
				break;
			case 17:	 // INVISIBILITYTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_INVISIBILITY )));
				break;
			case 18:	 // OBJECTUSETIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_OBJECTUSAGE )));
				break;
			case 19:	 // GATETIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_GATE )));
				break;
			case 20:	 // POISONTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_POISON )));
				break;
			case 21:	 // LOGINTIMEOUT
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_LOGINTIMEOUT )));
				break;
			case 22:	 // HITPOINTREGENTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_HITPOINTREGEN )));
				break;
			case 23:	 // STAMINAREGENTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_STAMINAREGEN )));
				break;
			case 37:	 // MANAREGENTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_MANAREGEN )));
				break;
			case 24:	 // BASEFISHINGTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_FISHINGBASE )));
				break;
			case 34:	// MAXPETOWNERS
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MaxPetOwners() ));
				break;
			case 35:	// MAXFOLLOWERS
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MaxFollowers() ));
				break;
			case 36:	// MAXCONTROLSLOTS
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MaxControlSlots() ));
				break;
			case 38:	 // RANDOMFISHINGTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_FISHINGRANDOM )));
				break;
			case 39:	 // SPIRITSPEAKTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_SPIRITSPEAK )));
				break;
			case 40:	 // DIRECTORY
			{
				JSString *tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_ROOT ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 41:	 // DATADIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_DATA ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 42:	 // DEFSDIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_DEFS ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 43:	 // ACTSDIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_ACCOUNTS ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 25:	 // SCRIPTSDIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 44:	 // BACKUPDIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_BACKUP ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 45:	 // MSGBOARDDIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 46:	 // SHAREDDIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_SHARED ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 47:	 // LOOTDECAYSWITHCORPSE
				args.rval().setBoolean( cwmWorldState->ServerData()->CorpseLootDecay() );
				break;
			case 49:	 // GUARDSACTIVE
				args.rval().setBoolean( cwmWorldState->ServerData()->GuardsStatus() );
				break;
			case 27:	 // DEATHANIMATION
				args.rval().setBoolean( cwmWorldState->ServerData()->DeathAnimationStatus() );
				break;
			case 50:	 // AMBIENTSOUNDS
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->WorldAmbientSounds() ));
				break;
			case 51:	 // AMBIENTFOOTSTEPS
				args.rval().setBoolean( cwmWorldState->ServerData()->AmbientFootsteps() );
				break;
			case 52:	 // INTERNALACCOUNTCREATION
				args.rval().setBoolean( cwmWorldState->ServerData()->InternalAccountStatus() );
				break;
			case 53:	 // SHOWOFFLINEPCS
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowOfflinePCs() );
				break;
			case 54:	 // ROGUESENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->RogueStatus() );
				break;
			case 55:	 // PLAYERPERSECUTION
				args.rval().setBoolean( cwmWorldState->ServerData()->PlayerPersecutionStatus() );
				break;
			case 56:	 // ACCOUNTFLUSH
				args.rval().setInt32( static_cast<R64>( cwmWorldState->ServerData()->AccountFlushTimer() ));
				break;
			case 57:	 // HTMLSTATUSENABLED
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->HtmlStatsStatus() ));
				break;
			case 58:	 // SELLBYNAME
				args.rval().setBoolean( cwmWorldState->ServerData()->SellByNameStatus() );
				break;
			case 59:	 // SELLMAXITEMS
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->SellMaxItemsStatus() ));
				break;
			case 60:	 // TRADESYSTEM
				args.rval().setBoolean( cwmWorldState->ServerData()->TradeSystemStatus() );
				break;
			case 61:	 // RANKSYSTEM
				args.rval().setBoolean( cwmWorldState->ServerData()->RankSystemStatus() );
				break;
			case 62:	 // CUTSCROLLREQUIREMENTS
				args.rval().setBoolean( cwmWorldState->ServerData()->CutScrollRequirementStatus() );
				break;
			case 63:	 // CHECKITEMS
				args.rval().setInt32( static_cast<R64>( cwmWorldState->ServerData()->CheckItemsSpeed() ));
				break;
			case 64:	 // CHECKBOATS
				args.rval().setInt32( static_cast<R64>( cwmWorldState->ServerData()->CheckBoatSpeed() ));
				break;
			case 65:	 // CHECKNPCAI
				args.rval().setInt32( static_cast<R64>( cwmWorldState->ServerData()->CheckNpcAISpeed() ));
				break;
			case 66:	 // CHECKSPAWNREGIONS
				args.rval().setInt32( static_cast<R64>( cwmWorldState->ServerData()->CheckSpawnRegionSpeed() ));
				break;
			case 67:	 // POSTINGLEVEL
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MsgBoardPostingLevel() ));
				break;
			case 68:	 // REMOVALLEVEL
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MsgBoardPostRemovalLevel() ));
				break;
			case 69:	 // ESCORTENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->EscortsEnabled() );
				break;
			case 70:	 // ESCORTINITEXPIRE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTWAIT )));
				break;
			case 71:	 // ESCORTACTIVEEXPIRE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTACTIVE )));
				break;
			case 72:	 // MOON1
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ServerMoon( 0 )));
				break;
			case 73:	 // MOON2
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ServerMoon( 1 )));
				break;
			case 74:	 // DUNGEONLEVEL
				args.rval().setInt32( static_cast<LIGHTLEVEL>( cwmWorldState->ServerData()->DungeonLightLevel() ));
				break;
			case 75:	 // CURRENTLEVEL
				args.rval().setInt32( static_cast<LIGHTLEVEL>( cwmWorldState->ServerData()->WorldLightCurrentLevel() ));
				break;
			case 76:	 // BRIGHTLEVEL
				args.rval().setInt32( static_cast<LIGHTLEVEL>( cwmWorldState->ServerData()->WorldLightBrightLevel() ));
				break;
			case 77:	 // BASERANGE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->TrackingBaseRange() ));
				break;
			case 78:	 // BASETIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->TrackingBaseTimer() ));
				break;
			case 79:	 // MAXTARGETS
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->TrackingMaxTargets() ));
				break;
			case 80:	 // MSGREDISPLAYTIME
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->TrackingRedisplayTime() ));
				break;
			case 81:	 // MURDERDECAYTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_MURDERDECAY )));
				break;
			case 82:	 // MAXKILLS
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->RepMaxKills() ));
				break;
			case 83:	 // CRIMINALTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_CRIMINAL )));
				break;
			case 84:	 // MINECHECK
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MineCheck() ));
				break;
			case 85:	 // OREPERAREA
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ResOre() ));
				break;
			case 86:	 // ORERESPAWNTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ResOreTime() ));
				break;
			case 87:	 // RESOURCEAREASIZE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ResourceAreaSize() ));
				break;
			case 88:	 // LOGSPERAREA
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ResLogs() ));
				break;
			case 89:	 // LOGSRESPAWNTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ResLogTime() ));
				break;
			case 90:	 // STATSAFFECTSKILLCHECKS
				args.rval().setBoolean( cwmWorldState->ServerData()->StatsAffectSkillChecks() );
				break;
			case 91:	 // HUNGERRATE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE )));
				break;
			case 92:	 // HUNGERDMGVAL
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->HungerDamage() ));
				break;
			case 93:	 // MAXRANGE
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatMaxRange() ));
				break;
			case 94:	 // SPELLMAXRANGE
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatMaxSpellRange() ));
				break;
			case 95:	 // DISPLAYHITMSG
				args.rval().setBoolean( cwmWorldState->ServerData()->CombatDisplayHitMessage() );
				break;
			case 96:	 // MONSTERSVSANIMALS
				args.rval().setBoolean( cwmWorldState->ServerData()->CombatMonstersVsAnimals() );
				break;
			case 97:	 // ANIMALATTACKCHANCE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->CombatAnimalsAttackChance() ));
				break;
			case 98:	 // ANIMALSGUARDED
				args.rval().setBoolean( cwmWorldState->ServerData()->CombatAnimalsGuarded() );
				break;
			case 99:	 // NPCDAMAGERATE
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatNpcDamageRate() ));
				break;
			case 100:	 // NPCBASEFLEEAT
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatNPCBaseFleeAt() ));
				break;
			case 101:	 // NPCBASEREATTACKAT
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatNPCBaseReattackAt() ));
				break;
			case 102:	 // ATTACKSTAMINA
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatAttackStamina() ));
				break;
			//case 103:	 // LOCATION
				//break;
			case 104:	 // STARTGOLD
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ServerStartGold() ));
				break;
			case 105:	 // STARTPRIVS
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ServerStartPrivs() ));
				break;
			case 106:	 // ESCORTDONEEXPIRE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTDONE )));
				break;
			case 107:	 // DARKLEVEL
				args.rval().setInt32( static_cast<LIGHTLEVEL>( cwmWorldState->ServerData()->WorldLightDarkLevel() ));
				break;
			case 108:	 // TITLECOLOUR
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->TitleColour() ));
				break;
			case 109:	 // LEFTTEXTCOLOUR
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->LeftTextColour() ));
				break;
			case 110:	 // RIGHTTEXTCOLOUR
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->RightTextColour() ));
				break;
			case 111:	 // BUTTONCANCEL
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ButtonCancel() ));
				break;
			case 112:	 // BUTTONLEFT
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ButtonLeft() ));
				break;
			case 113:	 // BUTTONRIGHT
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ButtonRight() ));
				break;
			case 114:	 // BACKGROUNDPIC
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->BackgroundPic() ));
				break;
			case 115:	 // POLLTIME
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->TownNumSecsPollOpen() ));
				break;
			case 116:	 // MAYORTIME
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->TownNumSecsAsMayor() ));
				break;
			case 117:	 // TAXPERIOD
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->TownTaxPeriod() ));
				break;
			case 118:	 // GUARDSPAID
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->TownGuardPayment() ));
				break;
			case 119:	 // DAY
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ServerTimeDay() ));
				break;
			case 120:	 // HOURS
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->ServerTimeHours() ));
				break;
			case 121:	 // MINUTES
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->ServerTimeMinutes() ));
				break;
			case 122:	 // SECONDS
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->ServerTimeSeconds() ));
				break;
			case 123:	 // AMPM
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerTimeAMPM() );
				break;
			case 124:	 // SKILLLEVEL
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->SkillLevel() ));
				break;
			case 125:	 // SNOOPISCRIME
				args.rval().setBoolean( cwmWorldState->ServerData()->SnoopIsCrime() );
				break;
			case 126:	 // BOOKSDIRECTORY
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_BOOKS ).c_str() );
				args.rval().setString( tString );
				break;
			//case 127:	 // SERVERLIST
				//break;
			case 128:	 // PORT
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ServerPort() ));
				break;
			case 129:	 // ACCESSDIRECTORY
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_ACCESS ).c_str() );
				args.rval().setString( tString );
				break;
			case 130:	 // LOGSDIRECTORY
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_LOGS ).c_str() );
				args.rval().setString( tString );
				break;
			case 132:	 // HTMLDIRECTORY
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_HTML ).c_str() );
				args.rval().setString( tString );
				break;
			case 133:	 // SHOOTONANIMALBACK
				args.rval().setBoolean( cwmWorldState->ServerData()->ShootOnAnimalBack() );
				break;
			case 134:	 // NPCTRAININGENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->NPCTrainingStatus() );
				break;
			case 135:	 // DICTIONARYDIRECTORY
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_DICTIONARIES ).c_str() );
				args.rval().setString( tString );
				break;
			case 136:	 // BACKUPSAVERATIO
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->BackupRatio() ));
				break;
			case 137:	 // HIDEWHILEMOUNTED
				args.rval().setBoolean( cwmWorldState->ServerData()->CharHideWhileMounted() );
				break;
			case 138:	 // SECONDSPERUOMINUTE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ServerSecondsPerUOMinute() ));
				break;
			case 139:	 // WEIGHTPERSTR
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->WeightPerStr() ));
				break;
			case 140:	 // POLYDURATION
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_POLYMORPH )));
				break;
			case 141:	 // UOGENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerUOGEnabled() );
				break;
			case 142:	 // NETRCVTIMEOUT
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->ServerNetRcvTimeout() ));
				break;
			case 143:	 // NETSNDTIMEOUT
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->ServerNetSndTimeout() ));
				break;
			case 144:	 // NETRETRYCOUNT
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->ServerNetRetryCount() ));
				break;
			case 145:	 // CLIENTFEATURES
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->GetClientFeatures() ));
				break;
			case 146:	 // PACKETOVERLOADS
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerOverloadPackets() );
				break;
			case 147:	 // NPCMOVEMENTSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCWalkingSpeed() ));
				break;
			case 148:	 // PETHUNGEROFFLINE
				args.rval().setBoolean( cwmWorldState->ServerData()->PetHungerOffline() );
				break;
			case 149:	 // PETOFFLINETIMEOUT
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->PetOfflineTimeout() ));
				break;
			case 150:	 // PETOFFLINECHECKTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_PETOFFLINECHECK ))));
				break;
			case 152:	 // ADVANCEDPATHFINDING
				args.rval().setBoolean( cwmWorldState->ServerData()->AdvancedPathfinding() );
				break;
			case 153:	 // SERVERFEATURES
				args.rval().setInt32( static_cast<size_t>( cwmWorldState->ServerData()->GetServerFeatures() ));
				break;
			case 154:	 // LOOTINGISCRIME
				args.rval().setBoolean( cwmWorldState->ServerData()->LootingIsCrime() );
				break;
			case 155:	 // NPCRUNNINGSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCRunningSpeed() ));
				break;
			case 156:	 // NPCFLEEINGSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCFleeingSpeed() ));
				break;
			case 157:	 // BASICTOOLTIPSONLY
				args.rval().setBoolean( cwmWorldState->ServerData()->BasicTooltipsOnly() );
				break;
			case 158:	 // GLOBALITEMDECAY
				args.rval().setBoolean( cwmWorldState->ServerData()->GlobalItemDecay() );
				break;
			case 159:	 // SCRIPTITEMSDECAYABLE
				args.rval().setBoolean( cwmWorldState->ServerData()->ScriptItemsDecayable() );
				break;
			case 160:	 // BASEITEMSDECAYABLE
				args.rval().setBoolean( cwmWorldState->ServerData()->BaseItemsDecayable() );
				break;
			case 161:	 // ITEMDECAYINHOUSES
				args.rval().setBoolean( cwmWorldState->ServerData()->ItemDecayInHouses() );
				break;
			case 162:	 // SPAWNREGIONSFACETS
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->GetSpawnRegionsFacetStatus() ));
				break;
			case 163:	// PAPERDOLLGUILDBUTTON
				args.rval().setBoolean( cwmWorldState->ServerData()->PaperdollGuildButton() );
				break;
			case 164:	// ATTACKSPEEDFROMSTAMINA
				args.rval().setBoolean( cwmWorldState->ServerData()->CombatAttackSpeedFromStamina() );
				break;
			case 169:	 // DISPLAYDAMAGENUMBERS
				args.rval().setBoolean( cwmWorldState->ServerData()->CombatDisplayDamageNumbers() );
				break;
			case 170:	 // CLIENTSUPPORT4000
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport4000() );
				break;
			case 171:	 // CLIENTSUPPORT5000
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport5000() );
				break;
			case 172:	 // CLIENTSUPPORT6000
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport6000() );
				break;
			case 173:	 // CLIENTSUPPORT6050
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport6050() );
				break;
			case 174:	 // CLIENTSUPPORT7000
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport7000() );
				break;
			case 175:	 // CLIENTSUPPORT7090
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport7090() );
				break;
			case 176:	 // CLIENTSUPPORT70160
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport70160() );
				break;
			case 177:	// CLIENTSUPPORT70240
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport70240() );
				break;
			case 178:	// CLIENTSUPPORT70300
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport70300() );
				break;
			case 179:	// CLIENTSUPPORT70331
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport70331() );
				break;
			case 180:	// CLIENTSUPPORT704565
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport704565() );
				break;
			case 181:	// CLIENTSUPPORT70610
				args.rval().setBoolean( cwmWorldState->ServerData()->ClientSupport70610() );
				break;
			case 182:	// EXTENDEDSTARTINGSTATS
				args.rval().setBoolean( cwmWorldState->ServerData()->ExtendedStartingStats() );
				break;
			case 183:	// EXTENDEDSTARTINGSKILLS
				args.rval().setBoolean( cwmWorldState->ServerData()->ExtendedStartingSkills() );
				break;
			case 184:	// WEAPONDAMAGECHANCE
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatWeaponDamageChance() ));
				break;
			case 185:	// ARMORDAMAGECHANCE
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatArmorDamageChance() ));
				break;
			case 186:	// WEAPONDAMAGEMIN
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatWeaponDamageMin() ));
				break;
			case 187:	// WEAPONDAMAGEMAX
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatWeaponDamageMax() ));
				break;
			case 188:	// ARMORDAMAGEMIN
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatArmorDamageMin() ));
				break;
			case 189:	// ARMORDAMAGEMAX
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatArmorDamageMax() ));
				break;
			case 190:	// GLOBALATTACKSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->GlobalAttackSpeed() ));
				break;
			case 191:	// NPCSPELLCASTSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCSpellCastSpeed() ));
				break;
			case 192:	// FISHINGSTAMINALOSS
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->FishingStaminaLoss() ));
				break;
			case 193:	// RANDOMSTARTINGLOCATION
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerRandomStartingLocation() );
				break;
			case 194:	// ASSISTANTNEGOTIATION
				args.rval().setBoolean( cwmWorldState->ServerData()->GetAssistantNegotiation() );
				break;
			case 195:	// KICKONASSISTANTSILENCE
				args.rval().setBoolean( cwmWorldState->ServerData()->KickOnAssistantSilence() );
				break;
			case 196:	// AF_FILTERWEATHER
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_FILTERWEATHER ));
				break;
			case 197:	// AF_FILTERLIGHT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_FILTERLIGHT ));
				break;
			case 198:	// AF_SMARTTARGET
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_SMARTTARGET ));
				break;
			case 199:	// AF_RANGEDTARGET
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_RANGEDTARGET ));
				break;
			case 200:	// AF_AUTOOPENDOORS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOOPENDOORS ));
				break;
			case 201:	// AF_DEQUIPONCAST
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_DEQUIPONCAST ));
				break;
			case 202:	// AF_AUTOPOTIONEQUIP
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP ));
				break;
			case 203:	// AF_POISONEDCHECKS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_POISONEDCHECKS ));
				break;
			case 204:	// AF_LOOPEDMACROS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_LOOPEDMACROS ));
				break;
			case 205:	// AF_USEONCEAGENT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_USEONCEAGENT ));
				break;
			case 206:	// AF_RESTOCKAGENT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_RESTOCKAGENT ));
				break;
			case 207:	// AF_SELLAGENT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_SELLAGENT ));
				break;
			case 208:	// AF_BUYAGENT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_BUYAGENT ));
				break;
			case 209:	// AF_POTIONHOTKEYS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_POTIONHOTKEYS ));
				break;
			case 210:	// AF_RANDOMTARGETS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_RANDOMTARGETS ));
				break;
			case 211:	// AF_CLOSESTTARGETS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_CLOSESTTARGETS ));
				break;
			case 212:	// AF_OVERHEADHEALTH
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_OVERHEADHEALTH ));
				break;
			case 213:	// AF_AUTOLOOTAGENT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOLOOTAGENT ));
				break;
			case 214:	// AF_BONECUTTERAGENT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_BONECUTTERAGENT ));
				break;
			case 215:	// AF_JSCRIPTMACROS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_JSCRIPTMACROS ));
				break;
			case 216:	// AF_AUTOREMOUNT
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOREMOUNT ));
				break;
			case 217:	// AF_ALL
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_ALL ));
				break;
			case 218:	// CLASSICUOMAPTRACKER
				args.rval().setBoolean( cwmWorldState->ServerData()->GetClassicUOMapTracker() );
				break;
			case 219:	// DECAYTIMERINHOUSE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_DECAYINHOUSE ))));
				break;
			case 220:	// PROTECTPRIVATEHOUSES
				args.rval().setBoolean( cwmWorldState->ServerData()->ProtectPrivateHouses() );
				break;
			case 221:	// TRACKHOUSESPERACCOUNT
				args.rval().setBoolean( cwmWorldState->ServerData()->TrackHousesPerAccount() );
				break;
			case 222:	// MAXHOUSESOWNABLE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->MaxHousesOwnable() ));
				break;
			case 223:	// MAXHOUSESCOOWNABLE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->MaxHousesCoOwnable() ));
				break;
			case 224:	// CANOWNANDCOOWNHOUSES
				args.rval().setBoolean( cwmWorldState->ServerData()->CanOwnAndCoOwnHouses() );
				break;
			case 225:	// COOWNHOUSESONSAMEACCOUNT
				args.rval().setBoolean( cwmWorldState->ServerData()->CoOwnHousesOnSameAccount() );
				break;
			case 226:	// ITEMSDETECTSPEECH
				args.rval().setBoolean( cwmWorldState->ServerData()->ItemsDetectSpeech() );
				break;
			case 227:	// MAXPLAYERPACKITEMS
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->MaxPlayerPackItems() ));
				break;
			case 228:	// MAXPLAYERBANKITEMS
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->MaxPlayerBankItems() ));
				break;
			case 229:	// FORCENEWANIMATIONPACKET
				args.rval().setBoolean( cwmWorldState->ServerData()->ForceNewAnimationPacket() );
				break;
			case 230:	// MAPDIFFSENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->MapDiffsEnabled() );
				break;
			case 231:	// CORESHARDERA
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionCoreShardEra() ), true ) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 232:	// ARMORCALCULATION
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionArmorCalculation() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 233:	// STRENGTHDAMAGEBONUS
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionStrengthDamageBonus() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 234:	// TACTICSDAMAGEBONUS
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionTacticsDamageBonus() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 235:	// ANATOMYDAMAGEBONUS
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionAnatomyDamageBonus() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 236:	// LUMBERJACKDAMAGEBONUS
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionLumberjackDamageBonus() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 237:	// RACIALDAMAGEBONUS
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionRacialDamageBonus() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 238:	// DAMAGEBONUSCAP
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionDamageBonusCap() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 239:	// SHIELDPARRY
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionShieldParry() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 240:	// PARRYDAMAGECHANCE
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatParryDamageChance() ));
				break;
			case 241:	// PARRYDAMAGEMIN
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->CombatParryDamageMin() ));
				break;
			case 242:	// PARRYDAMAGEMAX
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->CombatParryDamageMax() ));
				break;
			case 243:	// ARMORCLASSDAMAGEBONUS
				args.rval().setBoolean( cwmWorldState->ServerData()->CombatArmorClassDamageBonus() );
				break;
			case 244:	// FREESHARDSERVERPOLL
				args.rval().setBoolean( cwmWorldState->ServerData()->FreeshardServerPoll() );
				break;
			case 245:	// ALCHEMYBONUSENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->AlchemyDamageBonusEnabled() );
				break;
			case 246:	// ALCHEMYBONUSMODIFIER
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->AlchemyDamageBonusModifier() ));
				break;
			case 247:	 // NPCFLAGUPDATETIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_NPCFLAGUPDATETIMER ))));
				break;
			case 248:	 // JSENGINESIZE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->GetJSEngineSize() ));
				break;
			case 249:	 // USEUNICODEMESSAGES
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->UseUnicodeMessages() ));
				break;
			case 250:	 // SCRIPTDATADIRECTORY
			{
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTDATA ).c_str() );
				args.rval().setString( tString );
				break;
			}
			case 251:	 // THIRSTRATE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( static_cast<cSD_TID>( tSERVER_THIRSTRATE ))));
				break;
			case 252:	 // THIRSTDRAINVAL
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ThirstDrain() ));
				break;
			case 253:	 // PETTHIRSTOFFLINE
				args.rval().setBoolean( cwmWorldState->ServerData()->PetThirstOffline() );
				break;
			case 255:	 // BLOODDECAYTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAY )));
				break;
			case 256:	 // BLOODDECAYCORPSETIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAYCORPSE )));
				break;
			case 257:	// BLOODEFFECTCHANCE
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->CombatBloodEffectChance() ));
				break;
			case 258:	 // NPCCORPSEDECAYTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_NPCCORPSEDECAY )));
				break;
			case 259:	 // HUNGERENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->HungerSystemEnabled() );
				break;
			case 260:	 // THIRSTENABLED
				args.rval().setBoolean( cwmWorldState->ServerData()->ThirstSystemEnabled() );
				break;
			case 261:	 // TRAVELSPELLSFROMBOATKEYS
				args.rval().setBoolean( cwmWorldState->ServerData()->TravelSpellsFromBoatKeys() );
				break;
			case 262:	 // TRAVELSPELLSWHILEOVERWEIGHT
				args.rval().setBoolean( cwmWorldState->ServerData()->TravelSpellsWhileOverweight() );
				break;
			case 263:	 // MARKRUNESINMULTIS
				args.rval().setBoolean( cwmWorldState->ServerData()->MarkRunesInMultis() );
				break;
			case 264:	 // TRAVELSPELLSBETWEENWORLDS
				args.rval().setBoolean( cwmWorldState->ServerData()->TravelSpellsBetweenWorlds() );
				break;
			case 265:	 // TRAVELSPELLSWHILEAGGRESSOR
				args.rval().setBoolean( cwmWorldState->ServerData()->TravelSpellsWhileAggressor() );
				break;
			case 266:	 // BANKBUYTHRESHOLD
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->BuyThreshold() ));
				break;
			case 267:	 // NETWORKLOG
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerNetworkLog() );
				break;
			case 268:	 // SPEECHLOG
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerSpeechLog() );
				break;
			case 269:	 // NPCMOUNTEDWALKINGSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCMountedWalkingSpeed() ));
				break;
			case 270:	 // NPCMOUNTEDRUNNINGSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCMountedRunningSpeed() ));
				break;
			case 271:	 // NPCMOUNTEDFLEEINGSPEED
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->NPCMountedFleeingSpeed() ));
				break;
			case 272:	 // CONTEXTMENUS
				args.rval().setBoolean( cwmWorldState->ServerData()->ServerContextMenus() );
				break;
			case 273:	// SERVERLANGUAGE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ServerLanguage() ));
				break;
			case 274:	// CHECKPETCONTROLDIFFICULTY
				args.rval().setBoolean( cwmWorldState->ServerData()->CheckPetControlDifficulty() );
				break;
			case 275:	// PETLOYALTYGAINONSUCCESS
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->GetPetLoyaltyGainOnSuccess() ));
				break;
			case 276:	// PETLOYALTYLOSSONFAILURE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->GetPetLoyaltyLossOnFailure() ));
				break;
			case 277:	// PETLOYALTYRATE
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_LOYALTYRATE )));
				break;
			case 278:	// SHOWNPCTITLESINTOOLTIPS
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowNpcTitlesInTooltips() );
				break;
			case 279:	// FISHPERAREA
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->ResFish() ));
				break;
			case 280:	// FISHRESPAWNTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->ResFishTime() ));
				break;
			case 281:	 // ARCHERYHITBONUS
				args.rval().setInt32( static_cast<SI16>( cwmWorldState->ServerData()->CombatArcheryHitBonus() ));
				break;
			case 282:	// ITEMSINTERRUPTCASTING
				args.rval().setBoolean( cwmWorldState->ServerData()->ItemsInterruptCasting() );
				break;
			case 283:	// SYSMESSAGECOLOUR
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SysMsgColour() ));
				break;
			case 284:	// AF_AUTOBANDAGE
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_AUTOBANDAGE ));
				break;
			case 285:	// AF_ENEMYTARGETSHARE
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_ENEMYTARGETSHARE ));
				break;
			case 286:	// AF_FILTERSEASON
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_FILTERSEASON ));
				break;
			case 287:	// AF_SPELLTARGETSHARE
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_SPELLTARGETSHARE ));
				break;
			case 288:	// AF_HUMANOIDHEALTHCHECKS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_HUMANOIDHEALTHCHECKS ));
				break;
			case 289:	// AF_SPEECHJOURNALCHECKS
				args.rval().setBoolean( cwmWorldState->ServerData()->GetDisabledAssistantFeature( AF_SPEECHJOURNALCHECKS ));
				break;
			case 290:	// ARCHERYSHOOTDELAY
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->CombatArcheryShootDelay() ));
				break;
			case 291:	 // MAXCLIENTBYTESIN
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->MaxClientBytesIn() ));
				break;
			case 292:	 // MAXCLIENTBYTESOUT
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->MaxClientBytesOut() ));
				break;
			case 293:	 // NETTRAFFICTIMEBAN
				args.rval().setInt32( static_cast<UI32>( cwmWorldState->ServerData()->NetTrafficTimeban() ));
				break;
			case 294:	 // TOOLUSELIMIT
				args.rval().setBoolean( cwmWorldState->ServerData()->ToolUseLimit() );
				break;
			case 295:	 // TOOLUSEBREAK
				args.rval().setBoolean( cwmWorldState->ServerData()->ToolUseBreak() );
				break;
			case 296:	 // ITEMREPAIRDURABILITYLOSS
				args.rval().setBoolean( cwmWorldState->ServerData()->ItemRepairDurabilityLoss() );
				break;
			case 297:	 // HIDESTATSFORUNKNOWNMAGICITEMS
				args.rval().setBoolean( cwmWorldState->ServerData()->HideStatsForUnknownMagicItems() );
				break;
			case 298:	 // CRAFTCOLOUREDWEAPONS
				args.rval().setBoolean( cwmWorldState->ServerData()->CraftColouredWeapons() );
				break;
			case 299:	// MAXSAFETELEPORTSPERDAY
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->MaxSafeTeleportsPerDay() ));
				break;
			case 300:	 // TELEPORTONEARESTSAFELOCATION
				args.rval().setBoolean( cwmWorldState->ServerData()->TeleportToNearestSafeLocation() );
				break;
			case 301:	 // ALLOWAWAKENPCS
				args.rval().setBoolean( cwmWorldState->ServerData()->AllowAwakeNPCs() );
				break;
			case 302:	 // DISPLAYMAKERSMARK
				args.rval().setBoolean( cwmWorldState->ServerData()->DisplayMakersMark() );
				break;
			case 303:	// SHOWNPCTITLESOVERHEAD
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowNpcTitlesOverhead() );
				break;
			case 304:	// SHOWINVULNERABLETAGOVERHEAD
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowInvulnerableTagOverhead() );
				break;
			case 305:	// PETCOMBATTRAINING
				args.rval().setBoolean( cwmWorldState->ServerData()->PetCombatTraining() );
				break;
			case 306:	// HIRELINGCOMBATTRAINING
				args.rval().setBoolean( cwmWorldState->ServerData()->HirelingCombatTraining() );
				break;
			case 307:	// NPCCOMBATTRAINING
				args.rval().setBoolean( cwmWorldState->ServerData()->NpcCombatTraining() );
				break;
			case 308:	// GLOBALRESTOCKMULTIPLIER
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->GlobalRestockMultiplier() ));
				break;
			case 309:	// SHOWITEMRESISTSTATS
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowItemResistStats() );
				break;
			case 310:	// SHOWWEAPONDAMAGETYPES
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowWeaponDamageTypes() );
				break;
			case 311:	// SHOWRACEWITHNAME
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowRaceWithName() );
				break;
			case 312:	// SHOWRACEINPAPERDOLL
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowRaceInPaperdoll() );
				break;
			case 313:	// WEAPONPARRY
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionWeaponParry() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 314:	// WRESTLINGPARRY
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionWrestlingParry() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 315:	// COMBATHITCHANCE
			{
				std::string tempString = { cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->ExpansionCombatHitChance() )) };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			case 316:	// CASTSPELLSWHILEMOVING
				args.rval().setBoolean( cwmWorldState->ServerData()->CastSpellsWhileMoving() );
				break;
			case 317:	// SHOWREPUTATIONTITLEINTOOLTIP
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowReputationTitleInTooltip() );
				break;
			case 318:	// SHOWGUILDINFOINTOOLTIP
				args.rval().setBoolean( cwmWorldState->ServerData()->ShowGuildInfoInTooltip() );
				break;
			case 319:	// MAXPLAYERPACKWEIGHT
				args.rval().setInt32( static_cast<SI32>( cwmWorldState->ServerData()->MaxPlayerPackWeight() ));
				break;
			case 320:	// MAXPLAYERBANKWEIGHT
				args.rval().setInt32( static_cast<SI32>( cwmWorldState->ServerData()->MaxPlayerBankWeight() ));
				break;
			case 321:	// SAFECOOWNERLOGOUT
				args.rval().setBoolean( cwmWorldState->ServerData()->SafeCoOwnerLogout() );
				break;
			case 322:	// SAFEFRIENDLOGOUT
				args.rval().setBoolean( cwmWorldState->ServerData()->SafeFriendLogout() );
				break;
			case 323:	// SAFEGUESTLOGOUT
				args.rval().setBoolean( cwmWorldState->ServerData()->SafeGuestLogout() );
				break;
			case 324:	// KEYLESSOWNERACCESS
				args.rval().setBoolean( cwmWorldState->ServerData()->KeylessOwnerAccess() );
				break;
			case 325:	// KEYLESSCOOWNERACCESS
				args.rval().setBoolean( cwmWorldState->ServerData()->KeylessCoOwnerAccess() );
				break;
			case 326:	// KEYLESSFRIENDACCESS
				args.rval().setBoolean( cwmWorldState->ServerData()->KeylessFriendAccess() );
				break;
			case 327:	// KEYLESSGUESTACCESS
				args.rval().setBoolean( cwmWorldState->ServerData()->KeylessGuestAccess() );
				break;
			case 328:	// WEAPONDAMAGEBONUSTYPE
				args.rval().setInt32( static_cast<UI08>( cwmWorldState->ServerData()->WeaponDamageBonusType() ));
				break;
			case 329:	// OFFERBODSFROMITEMSALES
				args.rval().setBoolean( cwmWorldState->ServerData()->OfferBODsFromItemSales() );
				break;
			case 330:	// OFFERBODSFROMCONTEXTMENU
				args.rval().setBoolean( cwmWorldState->ServerData()->OfferBODsFromContextMenu() );
				break;
			case 331:	// BODSFROMCRAFTEDITEMSONLY
				args.rval().setBoolean( cwmWorldState->ServerData()->BODsFromCraftedItemsOnly() );
				break;
			case 332:	// BODGOLDREWARDMULTIPLIER
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->BODGoldRewardMultiplier() ));
				break;
			case 333:	// BODFAMEREWARDMULTIPLIER
				args.rval().setInt32( static_cast<R32>( cwmWorldState->ServerData()->BODFameRewardMultiplier() ));
				break;
			case 334:	// ENABLENPCGUILDDISCOUNTS
				args.rval().setBoolean( cwmWorldState->ServerData()->EnableNPCGuildDiscounts() );
				break;
			case 335:	// ENABLENPCGUILDPREMIUMS
				args.rval().setBoolean( cwmWorldState->ServerData()->EnableNPCGuildPremiums() );
				break;
			case 336:	 // AGGRESSORFLAGTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_AGGRESSORFLAG )));
				break;
			case 337:	 // PERMAGREYFLAGTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_PERMAGREYFLAG )));
				break;
			case 338:	 // STEALINGFLAGTIMER
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->SystemTimer( tSERVER_STEALINGFLAG )));
				break;
			case 339:	 // SNOOPAWARENESS
				args.rval().setBoolean( cwmWorldState->ServerData()->SnoopAwareness() );
				break;
			case 340:	 // APSPERFTHRESHOLD
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->APSPerfThreshold() ));
				break;
			case 341:	 // APSINTERVAL
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->APSPerfThreshold() ));
				break;
			case 342:	 // APSDELAYSTEP
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->APSDelayStep() ));
				break;
			case 343:	 // APSDELAYMAXCAP
				args.rval().setInt32( static_cast<UI16>( cwmWorldState->ServerData()->APSDelayMaxCap() ));
				break;
			case 344:	 // YOUNGPLAYERSYSTEM
				args.rval().setBoolean( cwmWorldState->ServerData()->YoungPlayerSystem() );
				break;
			//case 345:	 // YOUNGLOCATION
				//break;
			case 346:	 // SECRETSHARDKEY
			{
				std::string tempString = { cwmWorldState->ServerData()->SecretShardKey() };
				tString = JS_NewStringCopyZ( cx, tempString.c_str() );
				args.rval().setString( tString );
				break;
			}
			default:
				ScriptError( cx, "GetServerSetting: Invalid server setting name provided" );
				return false;
		}
	}
	else
	{
		ScriptError( cx, "GetServerSetting: Provided argument contained no valid string data" );
		return false;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetClientFeature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific client feature is enabled on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetClientFeature( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "GetClientFeature: Invalid number of arguments (takes 1 - feature ID)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	ClientFeatures clientFeature = static_cast<ClientFeatures>( args.get( 0 ).toInt32());
	args.rval().setBoolean( cwmWorldState->ServerData()->GetClientFeature( clientFeature ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerFeature()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a specific Server feature is enabled on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetServerFeature( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 1 )
	{
		ScriptError( cx, "GetServerFeature: Invalid number of arguments (takes 1 - feature ID)" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	ServerFeatures serverFeature = static_cast<ServerFeatures>( args.get( 0 ).toInt32());
	args.rval().setBoolean( cwmWorldState->ServerData()->GetServerFeature( serverFeature ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetAccountCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of accounts on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetAccountCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( Accounts->size() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetPlayerCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of players online on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetPlayerCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( cwmWorldState->GetPlayersOnline() );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetItemCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of items on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetItemCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetMultiCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of multis on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetMultiCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( ObjectFactory::GetSingleton().CountOfObjects( OT_MULTI ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetCharacterCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of characters on server
//o------------------------------------------------------------------------------------------------o
bool SE_GetCharacterCount( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	args.rval().setInt32( ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_GetServerVersionString()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets server version as a string
//o------------------------------------------------------------------------------------------------o
bool SE_GetServerVersionString( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
	std::string versionString = CVersionClass::GetVersion() + "." + CVersionClass::GetBuild() + " [" + OS_STR + "]";
	JSString *tString = JS_NewStringCopyZ( cx, versionString.c_str() );
	args.rval().setString( tString );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_DistanceBetween()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the distance between two locations, or two objects - or a combination of both
//o------------------------------------------------------------------------------------------------o
bool SE_DistanceBetween( JSContext* cx, unsigned argc, JS::Value* vp )
{
	if( argc != 2 && argc != 3 && argc != 4 && argc != 6 )
	{
		ScriptError( cx, "DistanceBetween: needs 2, 3, 4 or 6 arguments - object a, object b - or object a, object b, (bool)checkZ - or x1, y1 and x2, y2 - or x1, y1, z1 and x2, y2, z2!" );
		return false;
	}

  auto args		= JS::CallArgsFromVp(argc, vp);
	if( argc <= 3 )
	{
		// 2 or 3 arguments - find dinstance between two objects in 2D or 3D
		JSObject *srcObj = args.get( 0 ).toObjectOrNull();
		JSObject *trgObj = args.get( 1 ).toObjectOrNull();
		bool checkZ = argc == 3 ? ( args.get( 2 ).toBoolean() ) : false;
		CBaseObject *srcBaseObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>( srcObj, 0 );
		CBaseObject *trgBaseObj = JS::GetMaybePtrFromReservedSlot<CBaseObject>( trgObj, 0 );
		if( !ValidateObject( srcBaseObj ) || !ValidateObject( trgBaseObj ))
		{
			ScriptError( cx, "DistanceBetween: Invalid source or target object" );
			args.rval().setInt32( -1 );
			return false;
		}

		if( checkZ )
		{
			args.rval().setInt32( GetDist3D( srcBaseObj, trgBaseObj ));
		}
		else
		{
			args.rval().setInt32( GetDist( srcBaseObj, trgBaseObj ));
		}
	}
	else
	{
		UI16 x1 = static_cast<UI16>( args.get( 0 ).toInt32());
		UI16 y1 = static_cast<UI16>( args.get( 1 ).toInt32());
		UI16 x2 = 0;
		UI16 y2 = 0;
		if( argc == 4 )
		{
			// 4 arguments - find distance in 2D
			x2		= static_cast<UI16>( args.get( 2 ).toInt32());
			y2		= static_cast<UI16>( args.get( 3 ).toInt32());
			args.rval().setInt32( GetDist( Point3_st( x1, y1, 0 ), Point3_st( x2, y2, 0 )));
		}
		else
		{
			// 6 arguments - find distance in 3D
			SI08 z1 = static_cast<SI08>( args.get( 2 ).toInt32());
			x2		= static_cast<UI16>( args.get( 3 ).toInt32());
			y2		= static_cast<UI16>( args.get( 4 ).toInt32());
			SI08 z2 = static_cast<SI08>( args.get( 5 ).toInt32());
			args.rval().setInt32( GetDist3D( Point3_st( x1, y1, z1 ), Point3_st( x2, y2, z2 )));
		}
	}
	
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_BASEITEMSERIAL()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the BASEITEMSERIAL constant
//o------------------------------------------------------------------------------------------------o
bool SE_BASEITEMSERIAL( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
  args.rval().setInt32( BASEITEMSERIAL );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDSERIAL()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDSERIAL constant
//o------------------------------------------------------------------------------------------------o
bool SE_INVALIDSERIAL( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
  args.rval().setInt32( INVALIDSERIAL );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDID()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDID constant
//o------------------------------------------------------------------------------------------------o
bool SE_INVALIDID( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
  args.rval().setInt32( INVALIDID );
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SE_INVALIDCOLOUR()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the value of the INVALIDCOLOUR constant
//o------------------------------------------------------------------------------------------------o
bool SE_INVALIDCOLOUR( JSContext* cx, unsigned argc, JS::Value* vp )
{
  auto args		= JS::CallArgsFromVp(argc, vp);
  args.rval().setInt32( INVALIDCOLOUR );
	return true;
}
