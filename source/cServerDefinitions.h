#include <stack>

#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

typedef std::vector< Script * > VECSCRIPTLIST;
typedef	std::stack< std::string > dirList;

class cDirectoryListing
{
protected:
private:
	typedef std::vector< cDirectoryListing > DIRLIST;

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
	void				DisplayPriorityMap( void );

	SI16				GetPriority( const char *file );
public:
						cServerDefinitions();
						cServerDefinitions( const char *indexfilename );
	virtual				~cServerDefinitions();
	bool				Reload( void );

	ScriptSection	*	FindEntry( const char *toFind, DEFINITIONCATEGORIES typeToFind );
	Script			*	FindScript( const char *toFind, DEFINITIONCATEGORIES typeToFind );
	SI32				CountOfEntries( DEFINITIONCATEGORIES typeToFind );
	SI32				CountOfFiles( DEFINITIONCATEGORIES typeToFind );
	VECSCRIPTLIST			*	GetFiles( DEFINITIONCATEGORIES typeToFind );
};

#endif

