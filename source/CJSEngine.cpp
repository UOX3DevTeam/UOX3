//o------------------------------------------------------------------------------------------------o
//|	File		-	CJSEngine.cpp
//|	Date		-	2/22/2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	JS Engine Handling
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|
//|					1.0			2/22/2006
//|					Extracted JSObject handling from cScript and encapsulated it in a global class.
//|					Moved global JSEngine loading code into the class.
//|					Created CJSRuntime class to handle multiple runtimes (for threading purposes).
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "CJSEngine.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"
#include <algorithm>
#include <js/Initialization.h>
#include <js/Object.h>

CJSEngine *JSEngine = nullptr;

//==================================================================================================
auto CJSEngine::Startup() -> void
{
	runtimeList.resize( 0 );
  const UI32 maxEngineSize = JS::DefaultHeapMaxBytes;

	// 16 MB minimum. Any lower and UOX3 is prone to crashes from frequent JS reloads
	auto maxBytesSize = std::max( static_cast<UI16>( 16 ), cwmWorldState->ServerData()->GetJSEngineSize() ); // from INI

	// Use minimum of INI-provided value and hard-defined maximum
	// maxBytes definition: "Maximum nominal heap before last ditch GC"
	UI32 engineMaxBytes = std::min( static_cast<UI32>( static_cast<UI32>( maxBytesSize ) * 1024 * 1024 ), maxEngineSize );

	Console.PrintSectionBegin();
	Console << "Starting JavaScript Engine...." << myendl;

	if (!JS_Init())
	{
		throw new std::runtime_error("Unable to initialise JavaScript engine");
  }

	runtimeList.push_back( new CJSRuntime( engineMaxBytes ));	// Default Runtime
	runtimeList.push_back( new CJSRuntime( engineMaxBytes ));	// Console Runtime

	Console << "JavaScript engine startup complete." << myendl;
	Console.PrintSectionBegin();
}
//===================================================================
CJSEngine::~CJSEngine()
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if(( *rIter))
		{
			delete ( *rIter );
		}
	}
  JS_ShutDown();
}

void CJSEngine::Reload( void )
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if(( *rIter ) != nullptr )
		{
			( *rIter )->Reload();
		}
	}
}
void CJSEngine::CollectGarbage( void )
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if(( *rIter ) != nullptr )
		{
			( *rIter )->CollectGarbage();
		}
	}
}

JSRuntime *CJSEngine::GetRuntime( UI08 runTime ) const
{
	if( runTime >= runtimeList.size() )
	{
		runTime = 0;
	}

	return runtimeList[runTime]->GetRuntime();
}
JSContext *CJSEngine::GetContext( UI08 runTime ) const
{
	if( runTime >= runtimeList.size() )
	{
		runTime = 0;
	}

	return runtimeList[runTime]->GetContext();
}
JSObject *CJSEngine::GetObject( UI08 runTime ) const
{
	if( runTime >= runtimeList.size() )
	{
		runTime = 0;
	}

	return runtimeList[runTime]->GetObject();
}

UI08 CJSEngine::FindActiveRuntime( JSRuntime *rT ) const
{
	for( RUNTIMELIST_CITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if(( *rIter ) != nullptr )
		{
			if( rT == ( *rIter )->GetRuntime() )
				return static_cast<UI08>( rIter - runtimeList.begin() );
		}
	}
	return 0;
}

JSObject *CJSEngine::GetPrototype( UI08 runTime, JSPrototypes protoNum ) const
{
	JSObject *retVal = nullptr;
	if( runTime < runtimeList.size() )
	{
		retVal = runtimeList[runTime]->GetPrototype( protoNum );
	}

	return retVal;
}

JSObject *CJSEngine::AcquireObject( IUEEntries iType, void *index, UI08 runTime )
{
	JSObject *retVal = nullptr;
	if( index != nullptr && runTime < runtimeList.size() )
	{
		retVal = runtimeList[runTime]->AcquireObject( iType, index );
	}

	return retVal;
}
void CJSEngine::ReleaseObject( IUEEntries iType, void *index )
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if(( *rIter ) != nullptr )
		{
			( *rIter )->ReleaseObject( iType, index );
		}
	}
}

//========================================================================================================
// CJSRuntime
//======================================================================================================
CJSRuntime::CJSRuntime( UI32 engineSize )
{
  JS::RealmOptions options;
	jsContext = JS_NewContext( engineSize );

	JS::InitSelfHostedCode(jsContext);

	jsGlobal = JS_NewGlobalObject(jsContext, &global_class, nullptr, JS::FireOnNewGlobalHook, options);
	if( jsGlobal == nullptr )
	{
		Shutdown( FATAL_UOX3_JAVASCRIPT );
	}
	JS::InitRealmStandardClasses( jsContext );

	objectList.resize( IUE_COUNT );

	InitializePrototypes();
}
CJSRuntime::~CJSRuntime( void )
{
	Cleanup();

	// TODO: Unroot them

	JS_DestroyContext( jsContext );
}

void CJSRuntime::Cleanup( void )
{
	std::vector<JSOBJECTMAP>::iterator oIter;
	for( oIter = objectList.begin(); oIter != objectList.end(); ++oIter )
	{
		JSOBJECTMAP& ourList = ( *oIter );
		for( JSOBJECTMAP_ITERATOR lIter = ourList.begin(); lIter != ourList.end(); ++lIter )
		{
			JS::SetReservedSlot( ( *lIter ).second, 0, JS::UndefinedValue() );
		}
		ourList.clear();
	}
	objectList.resize( 0 );
	delete protoList;
}
void CJSRuntime::Reload()
{
	Cleanup();

	objectList.resize( IUE_COUNT );
}
void CJSRuntime::CollectGarbage()
{
	JS_GC( jsContext );
}

JSObject *rootClass(JSContext *cx, JS::HandleObject obj, const JSClass *clazz, JSNative constructor,
                           const JSPropertySpec *ps, const JSFunctionSpec *fs) {
  return JS_InitClass( cx, obj, clazz, nullptr, clazz->name, constructor,  0, ps, fs, nullptr, nullptr );
}

void CJSRuntime::InitializePrototypes()
{
  JSContext *cx = jsContext;
  JS::RootedObject obj(jsContext, jsGlobal);

  protoList = new JS::RootedObjectVector( cx );
  protoList->resize( JSP_COUNT );

  (*protoList)[JSP_CHAR]          .set( rootClass( cx, obj, &UOXChar_class,          nullptr,  CCharacterProps,        CChar_Methods ) );
  (*protoList)[JSP_ITEM]          .set( rootClass( cx, obj, &UOXItem_class,          nullptr,  CItemProps,             CItem_Methods ) );
  (*protoList)[JSP_SPELL]         .set( rootClass( cx, obj, &UOXSpell_class,         nullptr,  CSpellProperties,       nullptr ) );
  (*protoList)[JSP_SPELLS]        .set( rootClass( cx, obj, &UOXSpells_class,        nullptr,  nullptr,                nullptr ) );
  (*protoList)[JSP_GLOBALSKILL]   .set( rootClass( cx, obj, &UOXGlobalSkill_class,   nullptr,  CGlobalSkillProperties, nullptr ) );
  (*protoList)[JSP_GLOBALSKILLS]  .set( rootClass( cx, obj, &UOXGlobalSkills_class,  nullptr,  nullptr,                nullptr ) );
  (*protoList)[JSP_CREATEENTRY]   .set( rootClass( cx, obj, &UOXCreateEntry_class,   nullptr,  CCreateEntryProperties, nullptr ) );
  (*protoList)[JSP_CREATEENTRIES] .set( rootClass( cx, obj, &UOXCreateEntries_class, nullptr,  nullptr,                nullptr ) );
  (*protoList)[JSP_TIMER]         .set( rootClass( cx, obj, &UOXTimer_class,         nullptr,  CTimerProperties,       nullptr ) );
  (*protoList)[JSP_SOCK]          .set( rootClass( cx, obj, &UOXSocket_class,        nullptr,  CSocketProps,           CSocket_Methods ) );
  (*protoList)[JSP_ACCOUNTS]      .set( rootClass( cx, obj, &UOXAccount_class,       nullptr,  CAccountProperties,     CAccount_Methods ) );
  (*protoList)[JSP_CONSOLE]       .set( rootClass( cx, obj, &UOXConsole_class,       nullptr,  CConsoleProperties,     CConsole_Methods ) );
  (*protoList)[JSP_REGION]        .set( rootClass( cx, obj, &UOXRegion_class,        nullptr,  CRegionProperties,      CRegion_Methods ) );
  (*protoList)[JSP_SPAWNREGION]   .set( rootClass( cx, obj, &UOXSpawnRegion_class,   nullptr,  CSpawnRegionProperties, nullptr ) );
  (*protoList)[JSP_RESOURCE]      .set( rootClass( cx, obj, &UOXResource_class,      nullptr,  CResourceProperties,    nullptr ) );
  (*protoList)[JSP_RACE]          .set( rootClass( cx, obj, &UOXRace_class,          nullptr,  CRaceProperties,        CRace_Methods ) );
  (*protoList)[JSP_GUILD]         .set( rootClass( cx, obj, &UOXGuild_class,         nullptr,  CGuildProperties,       CGuild_Methods ) );
  (*protoList)[JSP_PARTY]         .set( rootClass( cx, obj, &UOXParty_class,         nullptr,  CPartyProperties,       CParty_Methods ) );
  (*protoList)[JSP_PACKET]        .set( rootClass( cx, obj, &UOXPacket_class,        Packet,   nullptr,                nullptr ) );
  (*protoList)[JSP_GUMP]          .set( rootClass( cx, obj, &UOXGump_class,          Gump,     nullptr,                nullptr ) );
  (*protoList)[JSP_FILE]          .set( rootClass( cx, obj, &UOXFile_class,          UOXCFile, nullptr,                nullptr ) );
  (*protoList)[JSP_SCRIPT]        .set( rootClass( cx, obj, &uox_class,              nullptr,  CScriptProperties,      nullptr ) );
  spellsObj        = JS_DefineObject( cx, obj, "Spells", &UOXSpells_class );
  skillsObj        = JS_DefineObject( cx, obj, "Skills", &UOXGlobalSkills_class );
  accountsObj      = JS_DefineObject( cx, obj, "Accounts", &UOXAccount_class );
  consoleObj       = JS_DefineObject( cx, obj, "Console", &UOXConsole_class );
  createEntriesObj = JS_DefineObject( cx, obj, "CreateEntries", &UOXCreateEntries_class );
  timerObj         = JS_DefineObject( cx, obj, "Timer", &UOXTimer_class );
  scriptObj        = JS_DefineObject( cx, obj, "SCRIPT", &uox_class );
  // clang-format on

	// TODO: Root them
}

JSRuntime *CJSRuntime::GetRuntime( void ) const
{
	return jsRuntime;
}
JSContext *CJSRuntime::GetContext( void ) const
{
	return jsContext;
}
JSObject *CJSRuntime::GetObject( void ) const
{
	return jsGlobal;
}

JSObject *CJSRuntime::GetPrototype( JSPrototypes protoNum ) const
{
	JSObject *retVal = nullptr;
	if( protoNum != JSP_COUNT )
	{
		retVal = (*protoList)[protoNum];
	}

	return retVal;
}

JSObject *CJSRuntime::AcquireObject( IUEEntries iType, void *index )
{
	JSObject *retVal = nullptr;
	if( iType != IUE_COUNT && static_cast<size_t>( iType ) < objectList.size() )
	{
		retVal = FindAssociatedObject( iType, index );
		if( retVal == nullptr )
		{
			retVal = MakeNewObject( iType );
			if( retVal != nullptr )
			{
				objectList[iType][index] = retVal;
				JS::SetReservedSlot( retVal, 0, JS::PrivateValue( index ) );
			}
		}
	}
	return retVal;
}
void CJSRuntime::ReleaseObject( IUEEntries iType, void *index )
{
	JSOBJECTMAP_ITERATOR toSearch = objectList[iType].find( index );
	if( toSearch != objectList[iType].end() )
	{
		JSObject *toRelease = ( *toSearch ).second;
		// TODO: Unroot it
		JS::SetReservedSlot( toRelease, 0, JS::UndefinedValue() );
		objectList[iType].erase( toSearch );
	}
}
JSObject *CJSRuntime::FindAssociatedObject( IUEEntries iType, void *index )
{
	JSObject *retVal = nullptr;
	JSOBJECTMAP_CITERATOR toSearch = objectList[iType].find( index );
	if( toSearch != objectList[iType].end() )
	{
		retVal = ( *toSearch ).second;
	}

	return retVal;
}
JSObject *CJSRuntime::MakeNewObject( IUEEntries iType )
{
	JSObject *toMake = nullptr;
	switch( iType )
	{
		case IUE_RACE:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXRace_class, (*protoList)[JSP_RACE] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_CHAR:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXChar_class, (*protoList)[JSP_CHAR] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_ITEM:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXItem_class, (*protoList)[JSP_ITEM] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_SOCK:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXSocket_class, (*protoList)[JSP_SOCK] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_GUILD:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXGuild_class, (*protoList)[JSP_GUILD] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_REGION:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXRegion_class, (*protoList)[JSP_REGION] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_SPAWNREGION:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXSpawnRegion_class, (*protoList)[JSP_SPAWNREGION] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_PARTY:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXParty_class, (*protoList)[JSP_PARTY] );
			if( toMake == nullptr )
				return nullptr;
			break;
		case IUE_ACCOUNT:
			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXAccount_class, (*protoList)[JSP_ACCOUNT] );
			if( toMake == nullptr )
				return nullptr;
			break;
		default:
		case IUE_COUNT:
			return nullptr;
	}

	// DAMN! Using the deprecated function it works!
	// TODO: Root it
	return toMake;
}
