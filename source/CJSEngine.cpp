//o-----------------------------------------------------------------------------------------------o
//|	File		-	CJSEngine.cpp
//|	Date		-	2/22/2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	JS Engine Handling
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|
//|					1.0			2/22/2006
//|					Extracted JSObject handling from cScript and encapsulated it in a global class.
//|					Moved global JSEngine loading code into the class.
//|					Created CJSRuntime class to handle multiple runtimes (for threading purposes).
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "CJSEngine.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"
#include <algorithm>

CJSEngine *JSEngine = nullptr;

CJSEngine::CJSEngine( void )
{
	runtimeList.resize( 0 );

	const UI32 maxEngineSize = 0xFFFFFFFF; // 4 gb, hard max

	// 16 MB minimum. Any lower and UOX3 is prone to crashes from frequent JS reloads
	auto maxBytesSize = std::max( static_cast<UI16>(16), cwmWorldState->ServerData()->GetJSEngineSize() ); // from INI

	// Use minimum of INI-provided value and hard-defined maximum
	// maxBytes definition: "Maximum nominal heap before last ditch GC"
	UI32 engineMaxBytes = std::min( static_cast<UI32>( static_cast<UI32>(maxBytesSize) * 1024 * 1024 ), maxEngineSize );

	Console.PrintSectionBegin();
	Console << "Starting JavaScript Engine...." << myendl;

	runtimeList.push_back( new CJSRuntime( engineMaxBytes ) );	// Default Runtime
	runtimeList.push_back( new CJSRuntime( engineMaxBytes ) );	// Console Runtime

	Console << "JavaScript engine startup complete." << myendl;
	Console.PrintSectionBegin();
}
CJSEngine::~CJSEngine( void )
{
	Console << "Destroying JS instances... ";

	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if( (*rIter) != nullptr )
		{
			delete (*rIter);
			(*rIter) = nullptr;
		}
	}

	runtimeList.resize( 0 );

	Console.PrintDone();
}

void CJSEngine::Reload( void )
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if( (*rIter) != nullptr )
			(*rIter)->Reload();
	}
}
void CJSEngine::CollectGarbage( void )
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if( (*rIter) != nullptr )
			(*rIter)->CollectGarbage();
	}
}

JSRuntime *CJSEngine::GetRuntime( UI08 runTime ) const
{
	if( runTime >= runtimeList.size() )
		runTime = 0;

	return runtimeList[runTime]->GetRuntime();
}
JSContext *CJSEngine::GetContext( UI08 runTime ) const
{
	if( runTime >= runtimeList.size() )
		runTime = 0;

	return runtimeList[runTime]->GetContext();
}
JSObject *CJSEngine::GetObject( UI08 runTime ) const
{
	if( runTime >= runtimeList.size() )
		runTime = 0;

	return runtimeList[runTime]->GetObject();
}

UI08 CJSEngine::FindActiveRuntime( JSRuntime *rT ) const
{
	for( RUNTIMELIST_CITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if( (*rIter) != nullptr )
		{
			if( rT == (*rIter)->GetRuntime() )
				return static_cast<UI08>(rIter - runtimeList.begin());
		}
	}
	return 0;
}

JSObject *CJSEngine::GetPrototype( UI08 runTime, JSPrototypes protoNum ) const
{
	JSObject *retVal = nullptr;
	if( runTime < runtimeList.size() )
		retVal = runtimeList[runTime]->GetPrototype( protoNum );

	return retVal;
}

JSObject *CJSEngine::AcquireObject( IUEEntries iType, void *index, UI08 runTime )
{
	JSObject *retVal = nullptr;
	if( index != nullptr && runTime < runtimeList.size() )
		retVal = runtimeList[runTime]->AcquireObject( iType, index );

	return retVal;
}
void CJSEngine::ReleaseObject( IUEEntries iType, void *index )
{
	for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
	{
		if( (*rIter) != nullptr )
			(*rIter)->ReleaseObject( iType, index );
	}
}


CJSRuntime::CJSRuntime( UI32 engineSize )
{
	jsRuntime = JS_NewRuntime( engineSize );
	if( jsRuntime == nullptr )
		Shutdown( FATAL_UOX3_JAVASCRIPT );

	// No need to use a large number here, it's not stack size as documentation indicated,
	// but "chunk size of the stack pool". Recommendations are to leave it at 8192. In
	// debug builds, larger values can even degrade performance drastically
	jsContext = JS_NewContext( jsRuntime, 8192 );

	// Specify JS 1.7 as version to unlock const variables, let, etc
	JS_SetVersion( jsContext, JSVERSION_1_7 );

	if( jsContext == nullptr )
		Shutdown( FATAL_UOX3_JAVASCRIPT );

	jsGlobal = JS_NewObject( jsContext, &global_class, nullptr, nullptr );
	if( jsGlobal == nullptr )
		Shutdown( FATAL_UOX3_JAVASCRIPT );
	JS_LockGCThing( jsContext, jsGlobal );
	//JS_AddRoot( jsContext, &jsGlobal );
	JS_InitStandardClasses( jsContext, jsGlobal );

	objectList.resize( IUE_COUNT );

	InitializePrototypes();
}
CJSRuntime::~CJSRuntime( void )
{
	Cleanup();

	JS_UnlockGCThing( jsContext, spellsObj );
	//JS_RemoveRoot( jsContext, &spellsObj );
	JS_UnlockGCThing( jsContext, accountsObj );
	//JS_RemoveRoot( jsContext, &accountsObj );
	JS_UnlockGCThing( jsContext, consoleObj );
	//JS_RemoveRoot( jsContext, &consoleObj );
	JS_UnlockGCThing( jsContext, createEntriesObj );

	for( size_t i = JSP_ITEM; i < JSP_COUNT; ++i )
	{
		JS_UnlockGCThing( jsContext, protoList[i] );
		//JS_RemoveRoot( jsContext, &protoList[i] );
	}
	JS_UnlockGCThing( jsContext, jsGlobal );
	//JS_RemoveRoot( jsContext, &jsGlobal );
	JS_DestroyContext( jsContext );
	JS_DestroyRuntime( jsRuntime );
}

void CJSRuntime::Cleanup( void )
{
	std::vector< JSOBJECTMAP >::iterator oIter;
	for( oIter = objectList.begin(); oIter != objectList.end(); ++oIter )
	{
		JSOBJECTMAP& ourList = (*oIter);
		for( JSOBJECTMAP_ITERATOR lIter = ourList.begin(); lIter != ourList.end(); ++lIter )
		{
			JS_UnlockGCThing( jsContext, (*lIter).second );
			//JS_RemoveRoot( jsContext, &(*lIter).second );

			JS_SetPrivate( jsContext, (*lIter).second, nullptr );
		}
		ourList.clear();
	}
	objectList.resize( 0 );
}
void CJSRuntime::Reload( void )
{
	Cleanup();

	objectList.resize( IUE_COUNT );
}
void CJSRuntime::CollectGarbage( void )
{
	JS_GC( jsContext );
}

void CJSRuntime::InitializePrototypes( void )
{
	protoList.resize( JSP_COUNT );

	JSContext *cx			= jsContext;
	JSObject *obj			= jsGlobal;

	protoList[JSP_CHAR]		=	JS_InitClass( cx, obj, nullptr, &UOXChar_class.base,	nullptr,		0,		CCharacterProps,		CChar_Methods,		nullptr,	nullptr );
	protoList[JSP_ITEM]		=	JS_InitClass( cx, obj, nullptr, &UOXItem_class.base,	nullptr,		0,		CItemProps,				CItem_Methods,		nullptr,	nullptr );
	protoList[JSP_SPELL]	=	JS_InitClass( cx, obj, nullptr, &UOXSpell_class,		nullptr,		0,		CSpellProperties,		nullptr,			nullptr,	nullptr );
	protoList[JSP_SPELLS]	=	JS_InitClass( cx, obj, nullptr, &UOXSpells_class,		nullptr,		0,		nullptr,				nullptr,			nullptr,	nullptr );
	protoList[JSP_CREATEENTRY]		= JS_InitClass( cx, obj, nullptr, &UOXCreateEntry_class,	nullptr, 0,		CCreateEntryProperties, nullptr,			nullptr,	nullptr );
	protoList[JSP_CREATEENTRIES]	= JS_InitClass( cx, obj, nullptr, &UOXCreateEntries_class,	nullptr, 0,		nullptr,				nullptr,			nullptr,	nullptr );
	protoList[JSP_SOCK]		=	JS_InitClass( cx, obj, nullptr, &UOXSocket_class.base,	nullptr,		0,		CSocketProps,			CSocket_Methods,	nullptr,	nullptr );
	protoList[JSP_ACCOUNTS]	=	JS_InitClass( cx, obj, nullptr, &UOXAccount_class,		nullptr,		0,		CAccountProperties,		CAccount_Methods,	nullptr,	nullptr );
	protoList[JSP_CONSOLE]	=	JS_InitClass( cx, obj, nullptr, &UOXConsole_class,		nullptr,		0,		CConsoleProperties,		CConsole_Methods,	nullptr,	nullptr );
	protoList[JSP_REGION]	=	JS_InitClass( cx, obj, nullptr, &UOXRegion_class,		nullptr,		0,		CRegionProperties,		CRegion_Methods,	nullptr,	nullptr );
	protoList[JSP_SPAWNREGION]=	JS_InitClass( cx, obj, nullptr, &UOXSpawnRegion_class,	nullptr,		0,		CSpawnRegionProperties,	nullptr,			nullptr,	nullptr );
	protoList[JSP_RESOURCE]	=	JS_InitClass( cx, obj, nullptr, &UOXResource_class,		nullptr,		0,		CResourceProperties,	nullptr,			nullptr,	nullptr );
	protoList[JSP_RACE]		=	JS_InitClass( cx, obj, nullptr, &UOXRace_class,			nullptr,		0,		CRaceProperties,		CRace_Methods,		nullptr,	nullptr );
	protoList[JSP_GUILD]	=	JS_InitClass( cx, obj, nullptr, &UOXGuild_class,		nullptr,		0,		CGuildProperties,		CGuild_Methods,		nullptr,	nullptr );
	protoList[JSP_PARTY]	=	JS_InitClass( cx, obj, nullptr, &UOXParty_class.base,	nullptr,		0,		CPartyProperties,		CParty_Methods,		nullptr,	nullptr );
	protoList[JSP_PACKET]	=	JS_InitClass( cx, obj, nullptr, &UOXPacket_class,		Packet,			0,		nullptr,				nullptr,			nullptr,	nullptr );
	protoList[JSP_GUMP]		=	JS_InitClass( cx, obj, nullptr, &UOXGump_class,			Gump,			0,		nullptr,				nullptr,			nullptr,	nullptr );
	protoList[JSP_FILE]		=	JS_InitClass( cx, obj, nullptr, &UOXFile_class,			UOXCFile,		0,		nullptr,				nullptr,			nullptr,	nullptr );
	spellsObj				=	JS_DefineObject( cx, obj, "Spells", &UOXSpells_class, protoList[JSP_SPELLS], 0 );
	accountsObj				=	JS_DefineObject( cx, obj, "Accounts", &UOXAccount_class, protoList[JSP_ACCOUNTS], 0 );
	consoleObj				=	JS_DefineObject( cx, obj, "Console", &UOXConsole_class, protoList[JSP_CONSOLE], 0 );
	createEntriesObj		=	JS_DefineObject( cx, obj, "CreateEntries", &UOXCreateEntries_class, protoList[JSP_CREATEENTRIES], 0 );
	JS_LockGCThing( cx, spellsObj );
	//JS_AddRoot( cx, &spellsObj );
	JS_LockGCThing( cx, accountsObj );
	//JS_AddRoot( cx, &accountsObj );
	JS_LockGCThing( cx, consoleObj );
	//JS_AddRoot( cx, &consoleObj );
	JS_LockGCThing( cx, createEntriesObj );

	for( size_t i = JSP_ITEM; i < JSP_COUNT; ++i )
	{
		JS_LockGCThing( cx, protoList[i] );
		//JS_AddRoot( cx, &protoList[i] );
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
		retVal = protoList[protoNum];

	return retVal;
}

JSObject *CJSRuntime::AcquireObject( IUEEntries iType, void *index )
{
	JSObject *retVal = nullptr;
	if( iType != IUE_COUNT && static_cast<size_t>(iType) < objectList.size() )
	{
		retVal = FindAssociatedObject( iType, index );
		if( retVal == nullptr )
		{
			retVal = MakeNewObject( iType );
			if( retVal != nullptr )
			{
				objectList[iType][index] = retVal;
				JS_SetPrivate( jsContext, retVal, index );
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
		JSObject *toRelease = (*toSearch).second;
		JS_UnlockGCThing( jsContext, toRelease );
		//JS_RemoveRoot( jsContext, &toRelease );
		JS_SetPrivate( jsContext, toRelease, nullptr );
		objectList[iType].erase( toSearch );
	}
}
JSObject *CJSRuntime::FindAssociatedObject( IUEEntries iType, void *index )
{
	JSObject *retVal = nullptr;
	JSOBJECTMAP_CITERATOR toSearch = objectList[iType].find( index );
	if( toSearch != objectList[iType].end() )
		retVal = (*toSearch).second;

	return retVal;
}
JSObject *CJSRuntime::MakeNewObject( IUEEntries iType )
{
	JSObject *toMake = nullptr;
	switch( iType )
	{
		case IUE_RACE:
			toMake = JS_NewObject( jsContext, &UOXRace_class, protoList[JSP_RACE], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CRace_Methods );
			//JS_DefineProperties( jsContext, toMake, CRaceProperties );
			break;
		case IUE_CHAR:
			toMake = JS_NewObject( jsContext, &UOXChar_class.base, protoList[JSP_CHAR], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineProperties( jsContext, toMake, CCharacterProps );
			//JS_DefineFunctions( jsContext, toMake, CChar_Methods );
			break;
		case IUE_ITEM:
			toMake = JS_NewObject( jsContext, &UOXItem_class.base, protoList[JSP_ITEM], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CItem_Methods );
			//JS_DefineProperties( jsContext, toMake, CItemProps );
			break;
		case IUE_SOCK:
			toMake = JS_NewObject( jsContext, &UOXSocket_class.base, protoList[JSP_SOCK], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CSocket_Methods );
			//JS_DefineProperties( jsContext, toMake, CSocketProps );
			break;
		case IUE_GUILD:
			toMake = JS_NewObject( jsContext, &UOXGuild_class, protoList[JSP_GUILD], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CGuild_Methods );
			//JS_DefineProperties( jsContext, toMake, CGuildProperties );
			break;
		case IUE_REGION:
			toMake = JS_NewObject( jsContext, &UOXRegion_class, protoList[JSP_REGION], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CRegion_Methods );
			//JS_DefineProperties( jsContext, toMake, CRegionProperties );
			break;
		case IUE_SPAWNREGION:
			toMake = JS_NewObject( jsContext, &UOXSpawnRegion_class, protoList[JSP_SPAWNREGION], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CSpawnRegion_Methods );
			//JS_DefineProperties( jsContext, toMake, CSpawnRegionProperties );
			break;
		case IUE_PARTY:
			toMake = JS_NewObject( jsContext, &UOXParty_class.base, protoList[JSP_PARTY], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CParty_Methods );
			//JS_DefineProperties( jsContext, toMake, CPartyProperties );
			break;
		case IUE_ACCOUNT:
			toMake = JS_NewObject( jsContext, &UOXAccount_class, protoList[JSP_ACCOUNT], jsGlobal );
			if( toMake == nullptr )
				return nullptr;
			//JS_DefineFunctions( jsContext, toMake, CAccount_Methods );
			//JS_DefineProperties( jsContext, toMake, CAccountProperties );
			break;
		default:
		case IUE_COUNT:
			return nullptr;
	}

	// DAMN! Using the deprecated function it works!
	JS_LockGCThing( jsContext, toMake );
	//JS_AddRoot( jsContext, &toMake );
	return toMake;
}
