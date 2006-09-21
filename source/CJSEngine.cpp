//o--------------------------------------------------------------------------o
//|	File			-	CJSEngine.cpp
//|	Date			-	2/22/2006
//|	Developers		-	Based on parts of cScript.cpp by Abaddon, rewritten by giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	JS Engine Handling
//o--------------------------------------------------------------------------o
//| Modifications	-	Version History
//|
//|							1.0			giwo		2/22/2006
//|							Extracted JSObject handling from cScript and encapsulated it in a global class.
//|							Moved global JSEngine loading code into the class.
//							Created CJSRuntime class to handle multiple runtimes (for threading purposes).
//|
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "CJSEngine.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "UOXJSPropertySpecs.h"

namespace UOX
{
	CJSEngine *JSEngine = NULL;

	CJSEngine::CJSEngine( void )
	{
		runtimeList.resize( 0 );

		const SI32 DefEngineSize = 0x1000000;

		std::ifstream engineData( "engine.dat" );
		SI32 engineSize = DefEngineSize;
		if( engineData.is_open() )
		{
			char line[1024];
			engineData.getline( line, 1024 );
			UString sLine( line );
			sLine = sLine.removeComment().stripWhiteSpace();
			if( !sLine.empty() )
				engineSize = UOX_MAX( sLine.toLong(), DefEngineSize );

			engineData.close();
		}
		
		Console.PrintSectionBegin();
		Console << "Starting JavaScript Engine...." << myendl;
		
		runtimeList.push_back( new CJSRuntime( engineSize ) );	// Default Runtime
		runtimeList.push_back( new CJSRuntime( engineSize ) );	// Console Runtime

		Console << "JavaScript engine startup complete." << myendl;
		Console.PrintSectionBegin();
	}
	CJSEngine::~CJSEngine( void )
	{
		Console << "Destroying JS instances... ";

		for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
		{
			if( (*rIter) != NULL )
			{
				delete (*rIter);
				(*rIter) = NULL;
			}
		}

		runtimeList.resize( 0 );

		Console.PrintDone();
	}

	void CJSEngine::Reload( void )
	{
		for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
		{
			if( (*rIter) != NULL )
				(*rIter)->Reload();
		}
	}
	void CJSEngine::CollectGarbage( void )
	{
		for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
		{
			if( (*rIter) != NULL )
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
			if( (*rIter) != NULL )
			{
				if( rT == (*rIter)->GetRuntime() )
					return static_cast<UI08>(rIter - runtimeList.begin());
			}
		}
		return 0;
	}

	JSObject *CJSEngine::GetPrototype( UI08 runTime, JSPrototypes protoNum ) const
	{
		JSObject *retVal = NULL;
		if( runTime < runtimeList.size() )
			retVal = runtimeList[runTime]->GetPrototype( protoNum );

		return retVal;
	}

	JSObject *CJSEngine::AcquireObject( IUEEntries iType, void *index, UI08 runTime )
	{
		JSObject *retVal = NULL;
		if( runTime < runtimeList.size() )
			retVal = runtimeList[runTime]->AcquireObject( iType, index );

		return retVal;
	}
	void CJSEngine::ReleaseObject( IUEEntries iType, void *index )
	{
		for( RUNTIMELIST_ITERATOR rIter = runtimeList.begin(); rIter != runtimeList.end(); ++rIter )
		{
			if( (*rIter) != NULL )
				(*rIter)->ReleaseObject( iType, index );
		}
	}


	CJSRuntime::CJSRuntime( UI32 engineSize )
	{
		jsRuntime = JS_NewRuntime( engineSize );
		if( jsRuntime == NULL )
			Shutdown( FATAL_UOX3_JAVASCRIPT );

		jsContext = JS_NewContext( jsRuntime, 0x500000 );
		if( jsContext == NULL )
			Shutdown( FATAL_UOX3_JAVASCRIPT );

		jsGlobal = JS_NewObject( jsContext, &global_class, NULL, NULL ); 
		if( jsGlobal == NULL )
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

				JS_SetPrivate( jsContext, (*lIter).second, NULL );
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

		protoList[JSP_CHAR]		=	JS_InitClass( cx, obj, NULL, &UOXChar_class.base,	NULL,		0,		CCharacterProps,		CChar_Methods,		NULL,	NULL );
		protoList[JSP_ITEM]		=	JS_InitClass( cx, obj, NULL, &UOXItem_class.base,	NULL,		0,		CItemProps,				CItem_Methods,		NULL,	NULL );
		protoList[JSP_SPELL]	=	JS_InitClass( cx, obj, NULL, &UOXSpell_class,		NULL,		0,		CSpellProperties,		NULL,				NULL,	NULL );
		protoList[JSP_SPELLS]	=	JS_InitClass( cx, obj, NULL, &UOXSpells_class,		NULL,		0,		NULL,					NULL,				NULL,	NULL );
		protoList[JSP_SOCK]		=	JS_InitClass( cx, obj, NULL, &UOXSocket_class.base,	NULL,		0,		CSocketProps,			CSocket_Methods,	NULL,	NULL );
		protoList[JSP_ACCOUNTS]	=	JS_InitClass( cx, obj, NULL, &UOXAccount_class,		NULL,		0,		CAccountProperties,		CAccount_Methods,	NULL,	NULL );
		protoList[JSP_CONSOLE]	=	JS_InitClass( cx, obj, NULL, &UOXConsole_class,		NULL,		0,		CConsoleProperties,		CConsole_Methods,	NULL,	NULL );
		protoList[JSP_REGION]	=	JS_InitClass( cx, obj, NULL, &UOXRegion_class,		NULL,		0,		CRegionProperties,		NULL,				NULL,	NULL );
		protoList[JSP_RESOURCE]	=	JS_InitClass( cx, obj, NULL, &UOXResource_class,	NULL,		0,		CResourceProperties,	NULL,				NULL,	NULL );
		protoList[JSP_RACE]		=	JS_InitClass( cx, obj, NULL, &UOXRace_class,		NULL,		0,		CRaceProperties,		CRace_Methods,		NULL,	NULL );
		protoList[JSP_GUILD]	=	JS_InitClass( cx, obj, NULL, &UOXGuild_class,		NULL,		0,		CGuildProperties,		CGuild_Methods,		NULL,	NULL );
		protoList[JSP_PARTY]	=	JS_InitClass( cx, obj, NULL, &UOXParty_class.base,	NULL,		0,		CPartyProperties,		CParty_Methods,		NULL,	NULL );
		protoList[JSP_PACKET]	=	JS_InitClass( cx, obj, NULL, &UOXPacket_class,		Packet,		0,		NULL,					NULL,				NULL,	NULL );
		protoList[JSP_GUMP]		=	JS_InitClass( cx, obj, NULL, &UOXGump_class,		Gump,		0,		NULL,					NULL,				NULL,	NULL );
		protoList[JSP_FILE]		=	JS_InitClass( cx, obj, NULL, &UOXFile_class,		UOXCFile,	0,		NULL,					NULL,				NULL,	NULL );
		spellsObj				=	JS_DefineObject( cx, obj, "Spells", &UOXSpells_class, protoList[JSP_SPELLS], 0 );
		accountsObj				=	JS_DefineObject( cx, obj, "Accounts", &UOXAccount_class, protoList[JSP_ACCOUNTS], 0 );
		consoleObj				=	JS_DefineObject( cx, obj, "Console", &UOXConsole_class, protoList[JSP_CONSOLE], 0 );

		JS_LockGCThing( cx, spellsObj );
		//JS_AddRoot( cx, &spellsObj );
		JS_LockGCThing( cx, accountsObj );
		//JS_AddRoot( cx, &accountsObj );
		JS_LockGCThing( cx, consoleObj );
		//JS_AddRoot( cx, &consoleObj );
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
		JSObject *retVal = NULL;
		if( protoNum != JSP_COUNT )
			retVal = protoList[protoNum];

		return retVal;
	}

	JSObject *CJSRuntime::AcquireObject( IUEEntries iType, void *index )
	{
		JSObject *retVal = NULL;
		if( iType != IUE_COUNT && static_cast<size_t>(iType) < objectList.size() )
		{
			retVal = FindAssociatedObject( iType, index );
			if( retVal == NULL )
			{
				retVal = MakeNewObject( iType );
				if( retVal != NULL )
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
			JS_SetPrivate( jsContext, toRelease, NULL );
			objectList[iType].erase( toSearch );
		}
	}
	JSObject *CJSRuntime::FindAssociatedObject( IUEEntries iType, void *index )
	{
		JSObject *retVal = NULL;
		JSOBJECTMAP_CITERATOR toSearch = objectList[iType].find( index );
		if( toSearch != objectList[iType].end() )
			retVal = (*toSearch).second;

		return retVal;
	}
	JSObject *CJSRuntime::MakeNewObject( IUEEntries iType )
	{
		JSObject *toMake = NULL;
		switch( iType )
		{
		case IUE_RACE:
			toMake = JS_NewObject( jsContext, &UOXRace_class, protoList[JSP_RACE], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineFunctions( jsContext, toMake, CRace_Methods );
			//JS_DefineProperties( jsContext, toMake, CRaceProperties );
			break;
		case IUE_CHAR:
			toMake = JS_NewObject( jsContext, &UOXChar_class.base, protoList[JSP_CHAR], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineProperties( jsContext, toMake, CCharacterProps );
			//JS_DefineFunctions( jsContext, toMake, CChar_Methods );
			break;
		case IUE_ITEM:
			toMake = JS_NewObject( jsContext, &UOXItem_class.base, protoList[JSP_ITEM], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineFunctions( jsContext, toMake, CItem_Methods );
			//JS_DefineProperties( jsContext, toMake, CItemProps );
			break;
		case IUE_SOCK:
			toMake = JS_NewObject( jsContext, &UOXSocket_class.base, protoList[JSP_SOCK], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineFunctions( jsContext, toMake, CSocket_Methods );
			//JS_DefineProperties( jsContext, toMake, CSocketProps );
			break;
		case IUE_GUILD:
			toMake = JS_NewObject( jsContext, &UOXGuild_class, protoList[JSP_GUILD], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineFunctions( jsContext, toMake, CGuild_Methods );
			//JS_DefineProperties( jsContext, toMake, CGuildProperties );
			break;
		case IUE_REGION:
			toMake = JS_NewObject( jsContext, &UOXRegion_class, protoList[JSP_REGION], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineFunctions( jsContext, toMake, CRegion_Methods );
			//JS_DefineProperties( jsContext, toMake, CRegionProperties );
			break;
		case IUE_PARTY:
			toMake = JS_NewObject( jsContext, &UOXParty_class.base, protoList[JSP_PARTY], jsGlobal ); 
			if( toMake == NULL )
				return NULL;
			//JS_DefineFunctions( jsContext, toMake, CRegion_Methods );
			//JS_DefineProperties( jsContext, toMake, CRegionProperties );
			break;
		default:
		case IUE_COUNT:
			return NULL;
		}

		// DAMN! Using the deprecated function it works!
		JS_LockGCThing( jsContext, toMake );
		//JS_AddRoot( jsContext, &toMake );
		return toMake;
	}

}
