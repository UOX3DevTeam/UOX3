#include "uox3.h"
#include <map>
#include <vector>
#include <string>
#include <stack>

#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

typedef std::vector< Script * > ScpList;
typedef	std::stack< std::string > dirList;

class cServerDefinitions
{
protected:
private:
	std::vector< ScpList >	ScriptListings;
	std::vector< std::string >	filenameListings;
	std::map< std::string, SI16 >	priorityMap;
	dirList                 dirs;
	SI16				defaultPriority;

	void				ReloadScriptObjects( void );
	void				BuildFileList( DefinitionCategories category );
	void				BuildPriorityMap( DefinitionCategories category, UI08& wasPrioritized );
	void				CleanPriorityMap( void );
	void				CleanFileList( void );
	void				DisplayPriorityMap( void );

	SI16				GetPriority( const char *file );

	bool				PushDir( DefinitionCategories toMove );
	void				PopDir( void );

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

