#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

#include <stack>

typedef std::vector< Script * > VECSCRIPTLIST;
typedef std::vector< Script * >::iterator VECSCRIPTLIST_ITERATOR;
typedef std::vector< Script * >::const_iterator VECSCRIPTLIST_CITERATOR;
typedef	std::stack< std::string > dirList;

class cDirectoryListing
{
private:
	typedef std::vector< cDirectoryListing > DIRLIST;
	typedef std::vector< cDirectoryListing >::iterator DIRLIST_ITERATOR;

	bool			PushDir( DEFINITIONCATEGORIES toMove );
	bool			PushDir( std::string toMove );
	void			PopDir( void );

	STRINGLIST		filenameList, shortList;
	STRINGLIST		flattenedShort, flattenedFull;
	dirList			dirs;
	std::string		extension;
	std::string		currentDir;
	std::string		shortCurrentDir;

	DIRLIST			subdirectories;
	bool			doRecursion;

	void			InternalRetrieve( void );

public:
	cDirectoryListing( bool recurse = true );
	cDirectoryListing( const std::string &dir, const std::string &extent, bool recurse = true );
	cDirectoryListing( DEFINITIONCATEGORIES dir, const std::string &extent, bool recurse = true );
	~cDirectoryListing();

	void			Extension( const std::string &extent );
	void			Retrieve( const std::string &dir );
	void			Retrieve( DEFINITIONCATEGORIES dir );
	void			Flatten( bool isParent );
	void			ClearFlatten( void );

	STRINGLIST *	List( void );
	STRINGLIST *	ShortList( void );
	STRINGLIST *	FlattenedList( void );
	STRINGLIST *	FlattenedShortList( void );
};

class CServerDefinitions
{
private:
	std::vector< VECSCRIPTLIST >			ScriptListings;
	VECSCRIPTLIST_ITERATOR					slIter;

	std::map< std::string, SI16 >			priorityMap;
	SI16									defaultPriority;

	void				LoadDFNCategory( DEFINITIONCATEGORIES toLoad );
	void				ReloadScriptObjects( void );
	void				BuildPriorityMap( DEFINITIONCATEGORIES category, UI08& wasPrioritized );
	void				CleanPriorityMap( void );

	SI16				GetPriority( const char *file );

	void				Cleanup( void );
public:
	CServerDefinitions();
	CServerDefinitions( const char *indexfilename );
	~CServerDefinitions();
	bool				Reload( void );
	bool				Dispose( DEFINITIONCATEGORIES toDispose );

	ScriptSection	*	FindEntry( std::string toFind, DEFINITIONCATEGORIES typeToFind );
	ScriptSection	*	FindEntrySubStr( std::string toFind, DEFINITIONCATEGORIES typeToFind );
	size_t				CountOfEntries( DEFINITIONCATEGORIES typeToFind );
	size_t				CountOfFiles( DEFINITIONCATEGORIES typeToFind );
	void				DisplayPriorityMap( void );

	Script			*	FirstScript( DEFINITIONCATEGORIES typeToFind );
	Script			*	NextScript( DEFINITIONCATEGORIES typeToFind );
	bool				FinishedScripts( DEFINITIONCATEGORIES typeToFind );
};

extern CServerDefinitions *FileLookup;

#endif

