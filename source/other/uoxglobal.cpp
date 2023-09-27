//


#include "books.h"
#include "ceffects.h"
#include "cguild.h"
#include "chtmlsystem.h"
#include "cjsengine.h"
#include "cjsmapping.h"
#include "classes.h" // worlditem, and ccharstuff
#include "cmagic.h"
#include "combat.h"
#include "commands.h"
#include "craces.h"
#include "cserverdefinitions.h"
#include "dictionary.h"
#include "jail.h"
#include "mapstuff.h"
#include "movement.h"
#include "network.h"
#include "pagevector.h"
#include "regions.h"
#include "skills.h"
#include "speech.h"
#include "uodata/uomgr.hpp"
#include "type/weather.hpp"
#include "weight.h"
#include "wholist.h"
#include "worldmain.h"

using namespace std::string_literals ;

//======================================================================
//
//  We will keep our global classes here for now, until we figure out what to do.
//  UOX3 currently makes pointers for everything, and then externs those.  Not sure
//  the advanatage, other then being able to control the shutdown order, and the binary size.
//
//  For now, we will start migrating them to external globals (not pointers), and then
//  sort out, if should be global, singleton, or an actual pointer (and perhaps scoped in a better place?)
//
//======================================================================

WorldWeather worldWeather ;
CWorldMain worldMain ;
CDictionaryContainer worldDictionary;
CHandleCombat worldCombat ;
WorldItem worldItem ;
CCharStuff worldNPC ;
CSkills worldSkill ;
CWeight worldWeight ;
CMagic worldMagic ;
cRaces worldRace ;
CMovement worldMovement ;
CWhoList worldWhoList ;
CWhoList worldOfflist(false) ;
CBooks worldBook ;
PageVector worldGMQueue("GM Queue");
PageVector worldCounselorQueue("Counselor Queue");
CJSMapping worldJSMapping ;
cEffects worldEffect ;
cHTMLTemplates worldHTMLTemplate;
CGuildCollection worldGuildSystem ;
CJailSystem worldJailSystem ;

// startup required
CSpeechQueue worldSpeechSystem ;
CJSEngine worldJSEngine ;
CServerDefinitions worldFileLookup ;
CCommands serverCommands ;
CMulHandler worldMULHandler ;
CNetworkStuff worldNetwork ;
CMapHandler worldMapHandler ;


// New items
uo::UOMgr uoManager ;
