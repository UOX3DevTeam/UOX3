#include "uox3.h"
#include <map>
#include <vector>
#include <string>

#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

typedef vector< Script * > ScpList;

class cServerDefinitions
{
protected:
private:
	vector< ScpList >	ScriptListings;
	vector< string >	filenameListings;
	map< string, SI16 >	priorityMap;
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

	ScriptSection	*	FindEntry( const char *toFind, DefinitionCategories typeToFind );
	Script			*	FindScript( const char *toFind, DefinitionCategories typeToFind );
	SI32				CountOfEntries( DefinitionCategories typeToFind );
	SI32				CountOfFiles( DefinitionCategories typeToFind );
	ScpList			*	GetFiles( DefinitionCategories typeToFind );
};

#endif

