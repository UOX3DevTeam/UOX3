#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

#include <stack>

namespace UOX
{

typedef std::vector< Script * > VECSCRIPTLIST;
typedef std::vector< Script * >::iterator VECSCRIPTLIST_ITERATOR;
typedef std::vector< Script * >::const_iterator VECSCRIPTLIST_CITERATOR;
typedef	std::stack< std::string > dirList;

class cDirectoryListing
{
protected:
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
					cDirectoryListing( std::string dir, std::string extent, bool recurse = true );
					cDirectoryListing( DEFINITIONCATEGORIES dir, std::string extent, bool recurse = true );
					~cDirectoryListing();

	void			Extension( std::string extent );
	void			Retrieve( std::string dir );
	void			Retrieve( DEFINITIONCATEGORIES dir );
	void			Flatten( bool isParent );
	void			ClearFlatten( void );

	STRINGLIST *	List( void );
	STRINGLIST *	ShortList( void );
	STRINGLIST *	FlattenedList( void );
	STRINGLIST *	FlattenedShortList( void );
};

class cServerDefinitions
{
protected:
private:
	std::vector< VECSCRIPTLIST >			ScriptListings;
	std::map< std::string, SI16 >	priorityMap;
	SI16							defaultPriority;

	void				ReloadScriptObjects( void );
	void				BuildPriorityMap( DEFINITIONCATEGORIES category, UI08& wasPrioritized );
	void				CleanPriorityMap( void );

	SI16				GetPriority( const char *file );
public:
						cServerDefinitions();
						cServerDefinitions( const char *indexfilename );
						~cServerDefinitions();
	bool				Reload( void );

	ScriptSection	*	FindEntry( std::string toFind, DEFINITIONCATEGORIES typeToFind );
	ScriptSection	*	FindEntrySubStr( std::string toFind, DEFINITIONCATEGORIES typeToFind );
	size_t				CountOfEntries( DEFINITIONCATEGORIES typeToFind );
	size_t				CountOfFiles( DEFINITIONCATEGORIES typeToFind );
	VECSCRIPTLIST	*	GetFiles( DEFINITIONCATEGORIES typeToFind );
	void				DisplayPriorityMap( void );
};

extern cServerDefinitions *FileLookup;

}

#endif

