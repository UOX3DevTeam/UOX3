#ifndef __CJSENGINE_H__
#define __CJSENGINE_H__

enum IUEEntries
{
	IUE_RACE = 0,
	IUE_CHAR,
	IUE_ITEM,
	IUE_SOCK,
	IUE_GUILD,
	IUE_REGION,
	IUE_SPAWNREGION,
	IUE_PARTY,
	IUE_ACCOUNT,
	IUE_COUNT
};

enum JSPrototypes
{
	JSP_ITEM	= 0,
	JSP_CHAR,
	JSP_SOCK,
	JSP_GUMP,
	JSP_PACKET,
	JSP_GUILD,
	JSP_RACE,
	JSP_REGION,
	JSP_SPAWNREGION,
	JSP_SPELL,
	JSP_SPELLS,
	JSP_RESOURCE,
	JSP_ACCOUNT,
	JSP_ACCOUNTS,
	JSP_CONSOLE,
	JSP_FILE,
	JSP_PARTY,
	JSP_CREATEENTRY,
	JSP_CREATEENTRIES,
	JSP_COUNT
};

class CJSRuntime
{
private:
	typedef std::map< void *, JSObject * >					JSOBJECTMAP;
	typedef std::map< void *, JSObject * >::iterator		JSOBJECTMAP_ITERATOR;
	typedef std::map< void *, JSObject * >::const_iterator	JSOBJECTMAP_CITERATOR;

	std::vector< JSOBJECTMAP >								objectList;
	std::vector< JSObject * >								protoList;

	JSObject * spellsObj;
	JSObject * accountsObj;
	JSObject * consoleObj;
	JSObject * createEntriesObj;
	JSRuntime * jsRuntime;
	JSContext * jsContext;
	JSObject * jsGlobal;

	JSObject *	FindAssociatedObject( IUEEntries iType, void *index );
	JSObject *	MakeNewObject( IUEEntries iType );

	void		Cleanup( void );
	void		InitializePrototypes( void );
public:
	CJSRuntime();
	CJSRuntime( UI32 engineSize );
	~CJSRuntime();

	void		Reload( void );
	void		CollectGarbage( void );

	JSRuntime *	GetRuntime( void ) const;
	JSContext * GetContext( void ) const;
	JSObject *	GetObject( void ) const;

	JSObject *	GetPrototype( JSPrototypes protoNum ) const;

	JSObject *	AcquireObject( IUEEntries iType, void *index );
	void		ReleaseObject( IUEEntries IType, void *index );
};

class CJSEngine
{
private:
	typedef std::vector< CJSRuntime * >					RUNTIMELIST;
	typedef std::vector< CJSRuntime * >::iterator		RUNTIMELIST_ITERATOR;
	typedef std::vector< CJSRuntime * >::const_iterator	RUNTIMELIST_CITERATOR;

	RUNTIMELIST											runtimeList;

public:

	CJSEngine();
	~CJSEngine();

	JSRuntime *	GetRuntime( UI08 runTime ) const;
	JSContext * GetContext( UI08 runTime ) const;
	JSObject *	GetObject( UI08 runTime ) const;

	UI08		FindActiveRuntime( JSRuntime *rT ) const;

	JSObject *	GetPrototype( UI08 runTime, JSPrototypes protoNum ) const;

	void		Reload( void );
	void		CollectGarbage( void );

	JSObject *	AcquireObject( IUEEntries iType, void *index, UI08 runTime );
	void		ReleaseObject( IUEEntries IType, void *index );
};

extern CJSEngine *JSEngine;

#endif
