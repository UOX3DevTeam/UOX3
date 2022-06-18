#include "uox3.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include "StringUtility.hpp"

#include <filesystem>
#include <memory>
#include <array>

using namespace std::string_literals ;

CServerDefinitions *FileLookup;
//=======================================================
auto CurrentWorkingDir() ->std::string {
	return std::filesystem::current_path().string();
}

//=======================================================
auto BuildPath(const std::string &extra) ->std::string {
	auto temp = std::filesystem::current_path();
	temp /= std::filesystem::path(extra);
	return temp.string();
}

//=======================================================
auto ShortDirectory(std::string sPath) ->std::string {
	return std::filesystem::path(sPath).filename().string();
}

//=======================================================
const std::array<std::string,NUM_DEFS> dirnames{

	"items"s,
	"npc"s,
	"create"s,
	"regions"s,
	"misc"s,
	"skills"s,
	"location"s,
	"menus"s,
	"spells"s,
	"newbie"s,
	"titles"s,
	"advance"s,
	"house"s,
	"colors"s,
	"spawn"s,
	"html"s,
	"race"s,
	"weather"s,
	"harditems"s,
	"command"s,
	"msgboard"s,
	"carve"s,
	"creatures"s,
	"maps"s
};

std::multimap<UI32,ADDMENUITEM> g_mmapAddMenuMap;

//=======================================================
CServerDefinitions::CServerDefinitions() : defaultPriority( 0 ) {
	// Load our device ips
	
}
//=======================================================
auto CServerDefinitions::startup() ->void {
	Console.PrintSectionBegin();
	Console << "Loading server scripts..." << myendl;
	Console << "   o Clearing AddMenuMap entries(" << static_cast<UI64>(g_mmapAddMenuMap.size()) << ")" << myendl;
	g_mmapAddMenuMap.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	Console.PrintSectionBegin();

}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Reload( void )
//|	Date		-	04/17/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload the dfn files.
//|	Changes		-	04042004 - Added the code to clear out the
//|									Auto-AddMenu items so there isn't any duplication in the
//|									multimap
//o-----------------------------------------------------------------------------------------------o
auto CServerDefinitions::Reload() ->bool {

	// We need to clear out the AddMenuItem Map
	g_mmapAddMenuMap.clear();
	//
	Cleanup();
	ScriptListings.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	return true;
}

//=======================================================
auto CServerDefinitions::Cleanup() ->void {
	std::vector< VECSCRIPTLIST >::iterator slIter;
	for( slIter = ScriptListings.begin(); slIter != ScriptListings.end(); ++slIter ) {
		VECSCRIPTLIST& toDel = (*slIter);
		for( size_t j = 0; j < toDel.size(); ++j ) {
			if( toDel[j]){
				delete toDel[j];
				toDel[j] = nullptr;
			}
		}
	}
}

//=======================================================
CServerDefinitions::~CServerDefinitions() {
	Cleanup();
}

//=======================================================
auto CServerDefinitions::Dispose(DEFINITIONCATEGORIES toDispose) ->bool {
	bool retVal = false;
	if( toDispose != NUM_DEFS ) {
		VECSCRIPTLIST& toDel = ScriptListings[toDispose];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter ) {
			Script *toDelete = (*dIter);
			if(toDelete) {
				retVal = true;
				delete toDelete;
			}
		}
		toDel.clear();
	}
	return retVal;
}

//=======================================================
auto CServerDefinitions::FindEntry( const std::string &toFind, DEFINITIONCATEGORIES typeToFind ) ->ScriptSection *{
	ScriptSection *rvalue = nullptr;

	if( !toFind.empty() && typeToFind != NUM_DEFS ){
		auto tUFind = oldstrutil::upper( toFind );

		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter ){
			Script *toCheck = (*dIter);
			if(toCheck ) {
				rvalue = toCheck->FindEntry( tUFind );
				if(rvalue){
					break;
				}
			}
		}
	}
	return rvalue;
}

//=======================================================
auto CServerDefinitions::FindEntrySubStr( const std::string &toFind, DEFINITIONCATEGORIES typeToFind ) ->ScriptSection *{
	ScriptSection *rvalue = nullptr;
	if( !toFind.empty() && typeToFind != NUM_DEFS ) {
		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter ) {
			Script *toCheck = (*dIter);
			if(toCheck) {
				rvalue = toCheck->FindEntrySubStr( toFind );
				if( rvalue ){
					break;
				}
			}
		}
	}
	return rvalue;
}

const std::string defExt = ".dfn";

//=======================================================
struct PrioScan {
	std::string	filename;
	SI16	priority;
	PrioScan() : filename( "" ), priority( 0 ) {
	}
	PrioScan( const char *toUse, SI16 mPrio ) : filename( toUse ), priority( mPrio ) {
	}
};

//=======================================================
inline auto operator==(const PrioScan& x, const PrioScan& y) -> bool {
	return ( x.priority == y.priority );
}

//=======================================================
inline auto operator<(const PrioScan& x, const PrioScan& y) ->bool {
	return ( x.priority < y.priority );
}

//=======================================================
inline auto operator>(const PrioScan& x, const PrioScan& y) ->bool {
	return ( x.priority > y.priority );
}

//=======================================================
auto CServerDefinitions::LoadDFNCategory( DEFINITIONCATEGORIES toLoad) ->void {
	CleanPriorityMap();
	defaultPriority = 0;
	UI08 wasPriod = 2;
	BuildPriorityMap( toLoad, wasPriod );

	cDirectoryListing fileList( toLoad, defExt );
	fileList.Flatten( true );
	std::vector< std::string > *shortListing	= fileList.FlattenedShortList();
	std::vector< std::string > *longListing		= fileList.FlattenedList();

	std::vector< PrioScan >	mSort;
	for( size_t i = 0; i < shortListing->size(); ++i ) {
		mSort.push_back( PrioScan( (*longListing)[i].c_str(), GetPriority((*shortListing)[i].c_str()) ) );
	}
	if( !mSort.empty() ) {
		std::sort( mSort.begin(), mSort.end() );
		Console.print( oldstrutil::format("Section %20s : %6i", dirnames[toLoad].c_str(), 0 ));
		size_t iTotal = 0;
		Console.TurnYellow();

		std::vector< PrioScan >::const_iterator mIter;
		for( mIter = mSort.begin(); mIter != mSort.end(); ++mIter ) {
			Console.print( "\b\b\b\b\b\b" );
			ScriptListings[toLoad].push_back( new Script( (*mIter).filename, toLoad, false ) );
			iTotal += ScriptListings[toLoad].back()->NumEntries();
			Console.print( oldstrutil::format("%6i", iTotal) );
		}

		Console.print( oldstrutil::format("\b\b\b\b\b\b%6i", CountOfEntries( toLoad )) );
		Console.TurnNormal();
		Console.print( " entries" );
		switch( wasPriod ) {
			case 0:
				Console.PrintSpecial( CGREEN,	"prioritized" );
				break;	// prioritized
			case 1:
				Console.PrintSpecial( CGREEN,		"done" );
				break;	// file exist, no section
			default:
			case 2:
				Console.PrintSpecial( CBLUE,	"done" );
				break;	// no file
		};
	}
}

//=======================================================
auto CServerDefinitions::ReloadScriptObjects() ->void {
	Console << myendl;

	for( SI32 sCtr = 0; sCtr < NUM_DEFS; ++sCtr ) {
		LoadDFNCategory( (DEFINITIONCATEGORIES)sCtr );
	}
	CleanPriorityMap();
}

//=======================================================
auto CServerDefinitions::CountOfEntries(DEFINITIONCATEGORIES typeToFind) ->size_t {
	size_t sumEntries = 0;
	VECSCRIPTLIST *toScan = &(ScriptListings[typeToFind]);
	if( toScan){
		for( auto cIter = toScan->begin(); cIter != toScan->end(); ++cIter ){
			sumEntries += (*cIter)->NumEntries();
		}
	}
	return sumEntries;
}

//=======================================================
auto CServerDefinitions::CountOfFiles(DEFINITIONCATEGORIES typeToFind) ->size_t {
	return ScriptListings[typeToFind].size();
}

//=======================================================
auto CServerDefinitions::FirstScript(DEFINITIONCATEGORIES typeToFind) ->Script *{
	Script *retScript = nullptr;
	slIter = ScriptListings[typeToFind].begin();
	if( !FinishedScripts(typeToFind) ){
		retScript = (*slIter);
	}
	return retScript;
}
//=======================================================
auto CServerDefinitions::NextScript(DEFINITIONCATEGORIES typeToFind) ->Script * {
	Script *retScript = nullptr;
	if( !FinishedScripts(typeToFind) ) {
		++slIter;
		if(!FinishedScripts( typeToFind)){
			retScript = (*slIter);
		}
	}
	return retScript;
}
//=======================================================
auto CServerDefinitions::FinishedScripts(DEFINITIONCATEGORIES typeToFind) ->bool {
	return ( slIter == ScriptListings[typeToFind].end() );
}

//=======================================================
auto CServerDefinitions::CleanPriorityMap() ->void {
	priorityMap.clear();
}
//===============================================================================================
auto CServerDefinitions::BuildPriorityMap( DEFINITIONCATEGORIES category, UI08& wasPrioritized )->void {
	cDirectoryListing priorityFile( category, "priority.nfo", false );
	std::vector< std::string > *longList = priorityFile.List();
	if( !longList->empty() ) {
		std::string filename = (*longList)[0];
		//	Do we have any priority informat?
		if( FileExists( filename ) ){	// the file exists, so perhaps we do
			auto prio = std::make_unique<Script>( filename, category, false );	// generate a script for it
			if(prio){	// successfully made a script
				auto prioInfo = prio->FindEntry( "PRIORITY" );	// find the priority entry
				if( prioInfo){
					for (const auto &sec : prioInfo->collection()){
						auto tag = sec->tag ;
						auto data = sec->data ;
						if( oldstrutil::upper( tag ) == "DEFAULTPRIORITY" ) {
							defaultPriority = static_cast<UI16>(std::stoul(data, nullptr, 0));
						}
						else {
							std::string filenametemp = oldstrutil::lower( tag );
							priorityMap[filenametemp] =static_cast<SI16>(std::stoi(data, nullptr, 0));
						}
					}
					wasPrioritized = 0;
				}
				else{
					wasPrioritized = 1;
				}
				prio = nullptr;
			}
			else{
				wasPrioritized = 2;
			}
			return;
		}
	}
#if defined( UOX_DEBUG_MODE )
	//	Console.warning( oldstrutil::format( "Failed to open priority.nfo for reading in %s DFN", dirnames[category].c_str() ));
#endif
	wasPrioritized = 2;
}

//=======================================================
void CServerDefinitions::DisplayPriorityMap() {
	Console << "Dumping map... " << myendl;
	for (auto &[name,priority] : priorityMap){
		Console << name << " : " << priority << myendl;
	}
	Console << "Dumped" << myendl;
}

//=======================================================
auto CServerDefinitions::GetPriority(const char *file) ->std::int16_t {
	auto retVal = defaultPriority;
	auto lowername = oldstrutil::lower(file);
	auto p = priorityMap.find( lowername );
	if( p != priorityMap.end() ){
		retVal = p->second;
	}
	return retVal;
}

//=======================================================
auto cDirectoryListing::PushDir(DEFINITIONCATEGORIES toMove) ->bool {
	auto filePath = cwmWorldState->ServerData()->Directory( CSDDP_DEFS );
	filePath += dirnames[toMove];
	return PushDir(filePath);
}
//=======================================================
auto cDirectoryListing::PushDir(std::string toMove) ->bool{
	std::string cwd = CurrentWorkingDir();
	dirs.push( cwd );
	auto path = std::filesystem::path( toMove );
	auto rvalue = true ;
	if( !std::filesystem::exists(path)) {
		Console.error(oldstrutil::format( "DFN directory %s does not exist", toMove.c_str()) );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	std::filesystem::current_path( path );
	currentDir = toMove;
	toMove = oldstrutil::replaceSlash(toMove);
	shortCurrentDir = ShortDirectory(toMove);
	return rvalue;
}
//=======================================================
auto cDirectoryListing::PopDir() ->void {
	if(dirs.empty()) {
		Console.error( "cServerDefinition::PopDir called, but dirs is empty" );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	else {
		auto path = std::filesystem::path(dirs.top());

		if( std::filesystem::exists(path )) {
			std::filesystem::current_path(path);
		}
		dirs.pop();
	}
}
//=======================================================
cDirectoryListing::cDirectoryListing( bool recurse ) : extension( ".dfn" ), doRecursion( recurse ) {
}
//=======================================================
cDirectoryListing::cDirectoryListing( const std::string &dir, const std::string &extent, bool recurse ) : doRecursion( recurse ) {
	Extension( extent );
	Retrieve( dir );
}
//=======================================================
cDirectoryListing::cDirectoryListing( DEFINITIONCATEGORIES dir, const std::string &extent, bool recurse ) : doRecursion( recurse ) {
	Extension(extent);
	Retrieve(dir);
}
//=======================================================
cDirectoryListing::~cDirectoryListing(){
	while( !dirs.empty() ){
		auto path = std::filesystem::path( dirs.top() );

		if( std::filesystem::exists(path)) {
			std::filesystem::current_path( path );
		}
		dirs.pop();
	}
}

//=======================================================
auto cDirectoryListing::Retrieve(const std::string &dir) ->void {
	bool dirSet = PushDir( dir );
	InternalRetrieve();
	if(dirSet) {
		PopDir();
	}
}
//=======================================================
auto cDirectoryListing::Retrieve(DEFINITIONCATEGORIES dir) ->void {
	bool dirSet = PushDir(dir);
	InternalRetrieve();
	if(dirSet) {
		PopDir();
	}
}

//=======================================================
auto cDirectoryListing::List() -> std::vector< std::string >*  {
	return &filenameList;
}

//=======================================================
auto cDirectoryListing::ShortList() ->std::vector< std::string >*{
	return &shortList;
}

//=======================================================
auto cDirectoryListing::FlattenedList() ->std::vector< std::string >*{
	return &flattenedFull;
}

//=======================================================
auto cDirectoryListing::FlattenedShortList() ->std::vector< std::string >* {
	return &flattenedShort;
}

//=======================================================
auto cDirectoryListing::InternalRetrieve() ->void {
	std::string filePath;
	auto path = std::filesystem::current_path();
	for( const auto& entry : std::filesystem::directory_iterator(path)) {
		auto name =  entry.path().filename().string();
		filePath = entry.path().string();
		auto ext = entry.path().extension();
		if( std::filesystem::is_regular_file( entry )) {
			if( !extension.empty() ) {
				if( entry.path().extension().string() != extension ) {
					name = "";
				}
			}
			if( !name.empty() ) {
				shortList.push_back( name );
				filenameList.push_back( filePath );
			}
		}
		else if( std::filesystem::is_directory(entry) && doRecursion ) {
			subdirectories.push_back( cDirectoryListing( name, extension, doRecursion ));
		}
	}
}

//=======================================================
auto cDirectoryListing::Extension(const std::string &extent) ->void {
	extension = extent;
}

//=======================================================
auto cDirectoryListing::Flatten(bool isParent) ->void {
	ClearFlatten();
	std::for_each(filenameList.begin(), filenameList.end(), [isParent,this](const std::string & entry){
		flattenedFull.push_back(entry);
		auto temp = std::string() ;
		if (!isParent) {
			temp = shortCurrentDir + "/"s ;
		}
		temp += entry ;
		flattenedShort.push_back(temp) ;
	});
	std::string temp ;
	DIRLIST_ITERATOR dIter;
	for( dIter = subdirectories.begin(); dIter != subdirectories.end(); ++dIter )
	{
		(*dIter).Flatten( false );
		auto shortFlat	= (*dIter).FlattenedShortList();
		auto longFlat	= (*dIter).FlattenedList();
		for( size_t k = 0; k < longFlat->size(); ++k )
		{
			flattenedFull.push_back( (*longFlat)[k] );
			if( isParent )
				temp = "";
			else
			{
				temp = shortCurrentDir;
				temp += "/";
			}
			temp += (*shortFlat)[k];
			flattenedShort.push_back( temp );
		}
		(*dIter).ClearFlatten();
	}
}
//=======================================================
auto cDirectoryListing::ClearFlatten() ->void {
	flattenedFull.clear();
	flattenedShort.clear();
}
