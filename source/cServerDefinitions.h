#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

#include <stack>

typedef std::vector<Script *> VECSCRIPTLIST;
typedef std::vector<Script *>::iterator VECSCRIPTLIST_ITERATOR;
typedef std::vector<Script *>::const_iterator VECSCRIPTLIST_CITERATOR;
typedef	std::stack<std::string> dirList;

class CDirectoryListing
{
private:
	typedef std::vector<CDirectoryListing> DIRLIST;
	typedef std::vector<CDirectoryListing>::iterator DIRLIST_ITERATOR;

	auto PushDir( DEFINITIONCATEGORIES toMove ) -> bool;
	auto PushDir( std::string toMove ) -> bool;
	auto PopDir() -> void;

	std::vector<std::string> filenameList, shortList;
	std::vector<std::string> flattenedShort, flattenedFull;
	dirList dirs;
	std::string extension;
	std::string currentDir;
	std::string shortCurrentDir;

	DIRLIST subdirectories;
	bool doRecursion;

	void InternalRetrieve();

public:
	CDirectoryListing( bool recurse = true );
	CDirectoryListing( const std::string &dir, const std::string &extent, bool recurse = true );
	CDirectoryListing( DEFINITIONCATEGORIES dir, const std::string &extent, bool recurse = true );
	~CDirectoryListing();

	auto Extension( const std::string &extent ) -> void;
	auto Retrieve( const std::string &dir ) -> void;
	auto Retrieve( DEFINITIONCATEGORIES dir ) -> void;
	auto Flatten( bool isParent) -> void;
	auto ClearFlatten() -> void;

	auto List() -> std::vector<std::string>*;
	auto ShortList() -> std::vector<std::string>*;
	auto FlattenedList() -> std::vector<std::string>*;
	auto FlattenedShortList() -> std::vector<std::string>*;
};

class CServerDefinitions
{
private:
	VECSCRIPTLIST_ITERATOR	 slIter;

	std::map<std::string, SI16>	 priorityMap;
	SI16	 defaultPriority;
	

	auto LoadDFNCategory( DEFINITIONCATEGORIES toLoad ) -> void;
	auto ReloadScriptObjects() -> void;
	auto BuildPriorityMap( DEFINITIONCATEGORIES category, UI08 &wasPrioritized ) -> void;
	auto CleanPriorityMap() -> void;

	auto GetPriority( const char *file ) -> SI16;

	auto Cleanup() -> void;
public:
	std::vector<VECSCRIPTLIST> ScriptListings;

	CServerDefinitions();
	~CServerDefinitions();
	auto Startup() -> void;
	auto Reload() -> bool;
	auto Dispose(DEFINITIONCATEGORIES toDispose ) -> bool;

	auto FindEntry( const std::string &toFind, DEFINITIONCATEGORIES typeToFind ) -> CScriptSection*;
	auto FindEntrySubStr( const std::string &toFind, DEFINITIONCATEGORIES typeToFind ) -> CScriptSection*;
	auto CountOfEntries( DEFINITIONCATEGORIES typeToFind ) -> size_t;
	auto CountOfFiles( DEFINITIONCATEGORIES typeToFind ) -> size_t;
	auto DisplayPriorityMap() -> void;

	auto FirstScript( DEFINITIONCATEGORIES typeToFind ) -> Script*;
	auto NextScript( DEFINITIONCATEGORIES typeToFind ) -> Script*;
	auto FinishedScripts( DEFINITIONCATEGORIES typeToFind ) -> bool;
};

extern CServerDefinitions *FileLookup;

#endif

