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
#include "cMagic.h"
#include "magic.h"
#include "skills.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"
#include <algorithm>
#include <jsapi.h>
#include <js/Initialization.h>
#include <js/Object.h>
#include <js/Warnings.h>

CJSEngine *JSEngine = nullptr;

void UOX3EngineWarningReporter(JSContext* cx, JSErrorReport* report)
{
	if (report->isWarning())
	{
		Console.Warning(oldstrutil::format("JS script failure: Message (%s)", report->message().c_str()));
	}
	else
	{
		Console.Error(oldstrutil::format("JS script failure: Message (%s)", report->message().c_str()));
	}
}


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
	//runtimeList.push_back( new CJSRuntime( engineMaxBytes ));	// Console Runtime

	Console << "JavaScript engine startup complete." << myendl;
	Console.PrintSectionBegin();
}
//===================================================================
CJSEngine::~CJSEngine()
{
	// Why?  we are shutting down, the process memory will take care of this for us in theory
	/*
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if(( *rIter))
		{
			delete ( *rIter );
		}
	}
	*/
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
	realmGuard = nullptr;
	jsContext = JS_NewContext( engineSize );

	JS::InitSelfHostedCode(jsContext);

	JS::RootedObject rootedGlobal( jsContext, JS_NewGlobalObject( jsContext, &global_class, nullptr, JS::FireOnNewGlobalHook, options ));
	if( rootedGlobal == nullptr )
	{
		Shutdown( FATAL_UOX3_JAVASCRIPT );
	}
	jsGlobal.init( jsContext, rootedGlobal );
	realmGuard = new JSAutoRealm( jsContext, jsGlobal );
	JS::InitRealmStandardClasses( jsContext );

	InitializePrototypes();

	JS::SetWarningReporter( jsContext, UOX3EngineWarningReporter );
}
CJSRuntime::~CJSRuntime( void )
{
	Cleanup();

	delete realmGuard;
	realmGuard = nullptr;
	jsGlobal.reset();
	JS_DestroyContext( jsContext );
}

void CJSRuntime::Cleanup( void )
{
	std::array<JSOBJECTMAP, IUE_COUNT>::iterator oIter;
	for( oIter = objectList.begin(); oIter != objectList.end(); ++oIter )
	{
		JSOBJECTMAP& ourList = ( *oIter );
		for( JSOBJECTMAP_ITERATOR lIter = ourList.begin(); lIter != ourList.end(); ++lIter )
		{
			JS_SetReservedSlot( *( *lIter ).second, 0, JS::UndefinedValue() );
		}
		ourList.clear();
	}
	delete protoList;
}
void CJSRuntime::Reload()
{
	for( auto &ourList : objectList )
	{
		for( auto &entry : ourList )
		{
			JS_SetReservedSlot( *entry.second, 0, JS::UndefinedValue() );
		}
		ourList.clear();
	}
}
void CJSRuntime::CollectGarbage()
{
	JS_GC( jsContext );
}

JSObject *rootClass(JSContext *cx, JS::HandleObject obj, const JSClass *clazz, JSNative constructor,
                           const JSPropertySpec *ps, const JSFunctionSpec *fs) {
  return JS_InitClass( cx, obj, clazz, nullptr, clazz->name, constructor,  0, ps, fs, nullptr, nullptr );
}

JSObject* rootInheritedClass(JSContext* cx, JS::HandleObject obj, const JSClass* clazz, JSNative constructor,
	const JSPropertySpec* ps, const JSFunctionSpec* fs, JS::HandleObject parent ) {
	return JS_InitClass(cx, obj, clazz, parent, clazz->name, constructor, 0, ps, fs, nullptr, nullptr);
}

JSObject *defineSingleton( JSContext *cx, JS::HandleObject obj, const char *name, const JSClass *clazz, JS::HandleObject prototype )
{
	JS::RootedObject singleton( cx, JS_NewObjectWithGivenProto( cx, clazz, prototype ));
	if( singleton == nullptr || !JS_DefineProperty( cx, obj, name, singleton, 0 ))
	{
		return nullptr;
	}
	return singleton;
}

bool ResolveSpellCollection( JSContext *cx, JS::HandleObject obj, JS::HandleId id, bool *resolved )
{
	*resolved = false;
	if( !id.isInt())
		return true;

	const int32_t spellId = id.toInt();
	if( spellId < 0 || static_cast<size_t>( spellId ) >= Magic->spells.size() )
		return true;

	const UI08 runTime = JSEngine->FindActiveRuntime( JS_GetRuntime( cx ));
	JS::RootedObject prototype( cx, JSEngine->GetPrototype( runTime, JSP_SPELL ));
	JS::RootedObject entry( cx, JS_NewObjectWithGivenProto( cx, &UOXSpell_class, prototype ));
	if( entry == nullptr )
		return false;

	JS_SetReservedSlot( entry, 0, JS::PrivateValue( &Magic->spells[spellId] ));
	JS::RootedValue value( cx, JS::ObjectValue( *entry ));
	if( !JS_DefinePropertyById( cx, obj, id, value,
		JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT ))
		return false;
	*resolved = true;
	return true;
}

bool ResolveCreateEntryCollection( JSContext *cx, JS::HandleObject obj, JS::HandleId id, bool *resolved )
{
	*resolved = false;
	if( !id.isInt())
		return true;

	const int32_t entryId = id.toInt();
	if( entryId < 0 || entryId > 0xFFFF )
		return true;
	CreateEntry_st *nativeEntry = Skills->FindItem( static_cast<UI16>( entryId ));
	if( nativeEntry == nullptr )
		return true;

	const UI08 runTime = JSEngine->FindActiveRuntime( JS_GetRuntime( cx ));
	JS::RootedObject prototype( cx, JSEngine->GetPrototype( runTime, JSP_CREATEENTRY ));
	JS::RootedObject entry( cx, JS_NewObjectWithGivenProto( cx, &UOXCreateEntry_class, prototype ));
	if( entry == nullptr )
		return false;

	JS_SetReservedSlot( entry, 0, JS::PrivateValue( nativeEntry ));
	JS::RootedValue value( cx, JS::ObjectValue( *entry ));
	if( !JS_DefinePropertyById( cx, obj, id, value,
		JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT ))
		return false;
	*resolved = true;
	return true;
}


void CJSRuntime::InitializePrototypes()
{
  JSContext *cx = jsContext;
  JS::RootedObject obj(jsContext, jsGlobal);

  protoList = new JS::RootedObjectVector( cx );
  protoList->resize( JSP_COUNT );

  (*protoList)[JSP_BASE]          .set( rootClass(          cx, obj, &UOXBase_class,          nullptr,  CBaseObjectProps,       CBaseObject_Methods ) );
  (*protoList)[JSP_CHAR]          .set( rootInheritedClass( cx, obj, &UOXChar_class,          nullptr,  CCharacterProps,        CChar_Methods, (*protoList)[JSP_BASE] ) );
  (*protoList)[JSP_ITEM]          .set( rootInheritedClass( cx, obj, &UOXItem_class,          nullptr,  CItemProps,             CItem_Methods, (*protoList)[JSP_BASE] ) );
  (*protoList)[JSP_SPELL]         .set( rootClass(          cx, obj, &UOXSpell_class,         nullptr,  CSpellProperties,       nullptr ) );
  (*protoList)[JSP_SPELLS]        .set( rootClass(          cx, obj, &UOXSpells_class,        nullptr,  nullptr,                nullptr ) );
  (*protoList)[JSP_GLOBALSKILL]   .set( rootClass(          cx, obj, &UOXGlobalSkill_class,   nullptr,  CGlobalSkillProperties, nullptr ) );
  (*protoList)[JSP_GLOBALSKILLS]  .set( rootClass(          cx, obj, &UOXGlobalSkills_class,  nullptr,  nullptr,                nullptr ) );
  (*protoList)[JSP_CREATEENTRY]   .set( rootClass(          cx, obj, &UOXCreateEntry_class,   nullptr,  CCreateEntryProperties, nullptr ) );
  (*protoList)[JSP_CREATEENTRIES] .set( rootClass(          cx, obj, &UOXCreateEntries_class, nullptr,  nullptr,                nullptr ) );
  (*protoList)[JSP_TIMER]         .set( rootClass(          cx, obj, &UOXTimer_class,         nullptr,  CTimerProperties,       nullptr ) );
  (*protoList)[JSP_SOCK]          .set( rootClass(          cx, obj, &UOXSocket_class,        nullptr,  CSocketProps,           CSocket_Methods ) );
  (*protoList)[JSP_ACCOUNT]       .set( rootClass(          cx, obj, &UOXAccount_class,       nullptr,  CAccountProperties,     nullptr ) );
  (*protoList)[JSP_ACCOUNTS]      .set( rootClass(          cx, obj, &UOXAccount_class,       nullptr,  nullptr,                CAccount_Methods ) );
  (*protoList)[JSP_CONSOLE]       .set( rootClass(          cx, obj, &UOXConsole_class,       nullptr,  CConsoleProperties,     CConsole_Methods ) );
  (*protoList)[JSP_REGION]        .set( rootClass(          cx, obj, &UOXRegion_class,        nullptr,  CRegionProperties,      CRegion_Methods ) );
  (*protoList)[JSP_SPAWNREGION]   .set( rootClass(          cx, obj, &UOXSpawnRegion_class,   nullptr,  CSpawnRegionProperties, nullptr ) );
  (*protoList)[JSP_RESOURCE]      .set( rootClass(          cx, obj, &UOXResource_class,      nullptr,  CResourceProperties,    nullptr ) );
  (*protoList)[JSP_RACE]          .set( rootClass(          cx, obj, &UOXRace_class,          nullptr,  CRaceProperties,        CRace_Methods ) );
  (*protoList)[JSP_GUILD]         .set( rootClass(          cx, obj, &UOXGuild_class,         nullptr,  CGuildProperties,       CGuild_Methods ) );
  (*protoList)[JSP_PARTY]         .set( rootClass(          cx, obj, &UOXParty_class,         nullptr,  CPartyProperties,       CParty_Methods ) );
  (*protoList)[JSP_PACKET]        .set( rootClass(          cx, obj, &UOXPacket_class,        Packet,   nullptr,                nullptr ) );
  (*protoList)[JSP_GUMP]          .set( rootClass(          cx, obj, &UOXGump_class,          Gump,     nullptr,                nullptr ) );
  (*protoList)[JSP_FILE]          .set( rootClass(          cx, obj, &UOXFile_class,          UOXCFile, nullptr,                nullptr ) );
  (*protoList)[JSP_SCRIPT]        .set( rootClass(          cx, obj, &uox_class,              nullptr,  CScriptProperties,      nullptr ) );
  JS::RootedObject spellsObj(        cx, defineSingleton( cx, obj, "Spells",        &UOXSpells_class,        ( *protoList )[JSP_SPELLS] ) );
  JS::RootedObject skillsObj(        cx, defineSingleton( cx, obj, "Skills",        &UOXGlobalSkills_class,  ( *protoList )[JSP_GLOBALSKILLS] ) );
  JS::RootedObject accountsObj(      cx, defineSingleton( cx, obj, "Accounts",      &UOXAccount_class,       ( *protoList )[JSP_ACCOUNTS] ) );
  JS::RootedObject consoleObj(       cx, defineSingleton( cx, obj, "Console",       &UOXConsole_class,       ( *protoList )[JSP_CONSOLE] ) );
  JS::RootedObject createEntriesObj( cx, defineSingleton( cx, obj, "CreateEntries", &UOXCreateEntries_class, ( *protoList )[JSP_CREATEENTRIES] ) );
  JS::RootedObject timerObj(         cx, defineSingleton( cx, obj, "Timer",         &UOXTimer_class,         ( *protoList )[JSP_TIMER] ) );
  JS::RootedObject scriptObj(        cx, defineSingleton( cx, obj, "SCRIPT",        &uox_class,              ( *protoList )[JSP_SCRIPT] ) );
  // clang-format on

	JS::RootedObject skillsCollection( cx, skillsObj );
	JS::RootedObject skillPrototype( cx, ( *protoList )[JSP_GLOBALSKILL] );
	for( UI08 skillId = 0; skillId < ALLSKILLS; ++skillId )
	{
		JS::RootedObject skillObject( cx,
			JS_NewObjectWithGivenProto( cx, &UOXGlobalSkill_class, skillPrototype ));
		if( skillObject == nullptr )
			continue;

		JS_SetReservedSlot( skillObject, 0, JS::PrivateValue( &cwmWorldState->skill[skillId] ));
		std::string propertyName = std::to_string( skillId );
		JS_DefineProperty( cx, skillsCollection, propertyName.c_str(), skillObject,
			JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT );
	}

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
	JS::RootedObject retVal( jsContext );
	if( iType != IUE_COUNT && static_cast<size_t>( iType ) < objectList.size() )
	{
		retVal = FindAssociatedObject( iType, index );
		if( retVal == nullptr )
		{
			retVal = MakeNewObject( iType );
			if( retVal != nullptr )
			{
				auto rootedObject = std::make_unique<JS::PersistentRootedObject>();
				rootedObject->init( jsContext, retVal );
				objectList[iType][index] = std::move( rootedObject );
				JS_SetReservedSlot( retVal, 0, JS::PrivateValue( index ) );
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
		JSObject *toRelease = *( *toSearch ).second;
		JS_SetReservedSlot( toRelease, 0, JS::UndefinedValue() );
		objectList[iType].erase( toSearch );
	}
}
JSObject *CJSRuntime::FindAssociatedObject( IUEEntries iType, void *index )
{
	JSObject *retVal = nullptr;
	JSOBJECTMAP_CITERATOR toSearch = objectList[iType].find( index );
	if( toSearch != objectList[iType].end() )
	{
		retVal = *( *toSearch ).second;
	}

	return retVal;
}
JSObject *CJSRuntime::MakeNewObject( IUEEntries iType )
{
	JS::RootedObject toMake( jsContext );
	switch( iType )
	{
		case IUE_RACE:				toMake = JS_NewObjectWithGivenProto( jsContext, &UOXRace_class, (*protoList)[JSP_RACE] );								break;
		case IUE_CHAR:				toMake = JS_NewObjectWithGivenProto( jsContext, &UOXChar_class, (*protoList)[JSP_CHAR] );						break;
		case IUE_ITEM:				toMake = JS_NewObjectWithGivenProto( jsContext, &UOXItem_class, (*protoList)[JSP_ITEM] );						break;
		case IUE_SOCK:				toMake = JS_NewObjectWithGivenProto( jsContext, &UOXSocket_class, (*protoList)[JSP_SOCK] );					break;
		case IUE_GUILD:				toMake = JS_NewObjectWithGivenProto( jsContext, &UOXGuild_class, (*protoList)[JSP_GUILD] );							break;
		case IUE_REGION:			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXRegion_class, (*protoList)[JSP_REGION] );						break;
		case IUE_SPAWNREGION:	toMake = JS_NewObjectWithGivenProto( jsContext, &UOXSpawnRegion_class, (*protoList)[JSP_SPAWNREGION] );	break;
		case IUE_PARTY:				toMake = JS_NewObjectWithGivenProto( jsContext, &UOXParty_class, (*protoList)[JSP_PARTY] );					break;
		case IUE_ACCOUNT:			toMake = JS_NewObjectWithGivenProto( jsContext, &UOXAccount_class, (*protoList)[JSP_ACCOUNT] );					break;
		default:
		case IUE_COUNT:				return nullptr;
	}

	if( toMake == nullptr )
		return nullptr;
	return toMake;
}
