#include "uox3.h"
#include <map>
#include <vector>
#include <string>
#include <stack>

#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

typedef std::vector< Script * > ScpList;
typedef	std::stack< std::string > dirList;

class cDirectoryListing
{
protected:
private:
	typedef std::vector< cDirectoryListing > DIRLIST;

	bool			PushDir( DefinitionCategories toMove );
	bool			PushDir( std::string toMove );
	void			PopDir( void );

	stringList		filenameList, shortList;
	stringList		flattenedShort, flattenedFull;
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
					cDirectoryListing( DefinitionCategories dir, std::string extent, bool recurse = true );
					~cDirectoryListing();

	void			Extension( std::string extent );
	void			Retrieve( std::string dir );
	void			Retrieve( DefinitionCategories dir );
	void			Flatten( bool isParent );
	void			ClearFlatten( void );

	stringList *	List( void );
	stringList *	ShortList( void );
	stringList *	FlattenedList( void );
	stringList *	FlattenedShortList( void );
};

class cServerDefinitions
{
protected:
private:
	std::vector< ScpList >			ScriptListings;
	std::map< std::string, SI16 >	priorityMap;
	SI16							defaultPriority;

	void				ReloadScriptObjects( void );
	void				BuildPriorityMap( DefinitionCategories category, UI08& wasPrioritized );
	void				CleanPriorityMap( void );
	void				DisplayPriorityMap( void );

	SI16				GetPriority( const char *file );
public:
						cServerDefinitions();
						cServerDefinitions( const char *indexfilename );
	virtual				~cServerDefinitions();
	bool				Reload( void );

	ScriptSection	*	FindEntry( const char *toFind, DefinitionCategories typeToFind );
	Script			*	FindScript( const char *toFind, DefinitionCategories typeToFind );
	SI32				CountOfEntries( DefinitionCategories typeToFind );
	SI32				CountOfFiles( DefinitionCategories typeToFind );
	ScpList			*	GetFiles( DefinitionCategories typeToFind );
};

#endif

