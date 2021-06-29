#include "uox3.h"
#include "scriptc.h"
#include "ssection.h"
#include <filesystem>
#include <cstdint>
#include <limits>
#include "IP4Address.hpp"
#if PLATFORM != WINDOWS
#include <netdb.h>

#include <arpa/inet.h>
#endif
#include "StringUtility.hpp"

#define	MAX_TRACKINGTARGETS	128
#define SKILLTOTALCAP		7000
#define SKILLCAP			1000
#define STATCAP				225

const UI32 BIT_ANNOUNCESAVES		= 0;
const UI32 BIT_ANNOUNCEJOINPART		= 1;
const UI32 BIT_SERVERBACKUP			= 2;
const UI32 BIT_SHOOTONANIMALBACK	= 3;
const UI32 BIT_NPCTRAINING			= 4;
const UI32 BIT_LOOTDECAYSONCORPSE	= 5;
const UI32 BIT_GUARDSENABLED		= 6;
const UI32 BIT_PLAYDEATHANIMATION	= 7;
const UI32 BIT_AMBIENTFOOTSTEPS		= 8;
const UI32 BIT_INTERNALACCOUNTS		= 9;
const UI32 BIT_SHOWOFFLINEPCS		= 10;
const UI32 BIT_ROGUESTATUS			= 11;
const UI32 BIT_SNOOPISCRIME			= 12;
const UI32 BIT_PERSECUTIONSTATUS	= 13;
const UI32 BIT_SELLBYNAME			= 14;
const UI32 BIT_TRADESYSSTATUS		= 15;
const UI32 BIT_RANKSYSSTATUS		= 16;
const UI32 BIT_CUTSCROLLREQ			= 17;
const UI32 BIT_SHOWHITMESSAGE		= 18;
const UI32 BIT_ESCORTSTATUS			= 19;
const UI32 BIT_MONSTERSVSANIMALS	= 20;
const UI32 BIT_PETHUNGEROFFLINE		= 21;
const UI32 BIT_HIDEWHILEMOUNTED		= 22;
const UI32 BIT_OVERLOADPACKETS		= 23;
const UI32 BIT_ARMORAFFECTMANAREGEN = 24;
const UI32 BIT_ANIMALSGUARDED		= 25;
const UI32 BIT_ADVANCEDPATHFIND		= 26;
const UI32 BIT_LOOTINGISCRIME		= 27;
const UI32 BIT_BASICTOOLTIPSONLY	= 28;
const UI32 BIT_GLOBALITEMDECAY		= 29;
const UI32 BIT_SCRIPTITEMSDECAYABLE	= 30;
const UI32 BIT_BASEITEMSDECAYABLE	= 31;
const UI32 BIT_ITEMDECAYINHOUSES	= 32;
const UI32 BIT_PAPERDOLLGUILDBUTTON = 33;
const UI32 BIT_ATTSPEEDFROMSTAMINA	= 34;
const UI32 BIT_SHOWDAMAGENUMBERS	= 35;
// 36 free!
// 37 free!
const UI32 BIT_EXTENDEDSTARTINGSTATS	= 38;
const UI32 BIT_EXTENDEDSTARTINGSKILLS	= 39;
const UI32 BIT_ASSISTANTNEGOTIATION		= 40;
const UI32 BIT_KICKONASSISTANTSILENCE	= 41;
const UI32 BIT_CLASSICUOMAPTRACKER		= 42;
const UI32 BIT_PROTECTPRIVATEHOUSES		= 43;
const UI32 BIT_TRACKHOUSESPERACCOUNT	= 44;
const UI32 BIT_CANOWNANDCOOWNHOUSES		= 45;
const UI32 BIT_COOWNHOUSESONSAMEACCOUNT = 46;
const UI32 BIT_ITEMSDETECTSPEECH		= 47;
const UI32 BIT_FORCENEWANIMATIONPACKET	= 48;
const UI32 BIT_MAPDIFFSENABLED			= 49;
const UI32 BIT_ARMORCLASSDAMAGEBONUS	= 50;
const UI32 BIT_CONNECTUOSERVERPOLL		= 51;
const UI32 BIT_ALCHEMYDAMAGEBONUSENABLED = 52;
const UI32 BIT_PETTHIRSTOFFLINE          = 53;
const UI32 BIT_HUNGERSYSTEMENABLED		= 55;
const UI32 BIT_THIRSTSYSTEMENABLED		= 56;
const UI32 BIT_TRAVELSPELLSFROMBOATKEYS	= 57;
const UI32 BIT_TRAVELSPELLSWHILEOVERWEIGHT	= 58;
const UI32 BIT_MARKRUNESINMULTIS			= 59;
const UI32 BIT_TRAVELSPELLSBETWEENWORLDS	= 60;
const UI32 BIT_TRAVELSPELLSWHILEAGGRESSOR	= 61;
const UI32 BIT_CONSOLELOG					= 62;
const UI32 BIT_NETWORKLOG					= 63;
const UI32 BIT_SPEECHLOG					= 64;


// New uox3.ini format lookup
// January 13, 2001	- 	Modified: January 30, 2001 Converted to uppercase
// February 26 2002 	- 	Modified: to support the AccountIsolation, and left out dir3ectory tags
// September 22 2002 	- 	Added the  "HIDEWILEMOUNTED" tag to support hide fix
// September 06 2003 	- 	Removed unused tags (heartbeat, wilderness, uoxbot, lagfix)
// October 16, 2003 	- 	Removed unused tag (SAVEMODE) and added "WEIGHTPERSTR".
// April 3, 2004 		- 	Added new tags, for UOG support, as well as new facet tags etc.
// June 15, 2004 		- 	Added the new tags for the xFTPd, and xFTPc support.
// April 2, 2005 		- 	Removed superfluous tags (BEGGINGTIMER, HUNGERDAMAGERATE, HUNGERTHRESHOLD, BEGGINGRANGE)
// September 13, 2005 	- 	Added MAPCOUNT to the uox.ini to specify the number of maps UOX3 uses.
// November 20, 2005 	- 	Removed USEFACETSAVES, MAP0, MAP1, MAP2, MAP3, USERMAP, FTPDENABLED,
//								FTPDUSER, FTPDUSERLIMIT, FTPDBIND, FTPDROOT, FTPCENABLED, FTPCHOST,
//								FTPCPORT, FTPCUSER, FTPCULFLAGS, and MAPCOUNT as they were unused.
//								Added OVERLOADPACKETS option to uox.ini to toggle JS packet handling.
// December 13, 2005 	- 	Changed uox.ini entry SHOWHIDDENNPCS to SHOWOFFLINEPCS to better reflect its function.
//								Removed CACHEMULS from the uox.ini as it is no longer used.
// February 12, 2006	- 	Added NPCMOVEMENTSPEED to the UOX.ini to allow customization of NPC movement speeds.
// July 7, 2006 		- 	PLAYERPERSECUTION in uox.ini (allows ghosts to drain mana from other players) set to 0 by default
// July 18, 2006 		- 	Added uox.ini flag to disable/enable the A* Pathfinding routine: ADVANCEDPATHFINDING=0/1
// July 24, 2006 		- 	Simplified many entries in uox.ini, making them easier to read.
// January 28, 2007 	- 	Moved dynamic parts of [worldlight] section from uox.ini to their own output file in the /shared/ directory
// March 3, 2007 		- 	Removed CLIENTSUPPORT from the uox.ini
//						  	Allowed customization of supported client features via uox.ini
//									CLIENTFEATURES
//									SERVERFEATURES
// April 9, 2007 		- 	Added a new setting to the uox.ini, LOOTINGISCRIME. If you set it to 0 looting corpses of innocent chars is not taken as a crime.
// June 10, 2007 		- 	Added two new settings to the uox.ini, NPCRUNNINGSPEED and NPCFLEEINGSPEED, they work the same way as NPCMOVEMENTSPEED.
// July 28, 2007 		- 	Added a new setting to the uox.ini: BASICTOOLTIPSONLY. If this is set to 1 the tooltips will only contain basic information, like the name and the weight of an item.
// February 7, 2010 	- 	Added new UOX.INI settings:
//									GLOBALITEMDECAY - Toggles on / off item decay on a global scale.Note that it does not remove the decay - status from items, it just resets the decay - timer when it reaches 0
//									SCRIPTITEMSDECAYABLE - Toggles whether DFN - items will decay by default or not.Can be overriden by DECAY tag in item - DFNs
//									BASEITEMSDECAYABLE - Toggles whether base - items will decay by default or not.Can be overriden by DECAY tag in harditems.dfn
//									ITEMDECAYINHOUSES - Toggles default decay of non - locked down items inside multis( houses and boats )
// December 14, 20011 	- 	Exposed CombatExplodeDelay to UOX.INI, and changed the default delay between targeting and damage for the Explosion spell from 0 to 2 seconds
// March 19, 2012 		- 	Added support for packet 0xD7, SubCommand 0x28 - Guild button on paperdoll, which gives access to guild-functions if character belongs to a guild.
//									Can be enabled / disabled through UOX.INI setting PAPERDOLLGUILDBUTTON = 0 / 1. Defaults to 0.
//							Added new UOX.INI option to determine whether attack - speed bonuses are gained from Stamina( default ) or Dexterity: ATTACKSPEEDFROMSTAMINA = 1 / 0
//							Added new UOX.INI option to control the displaying of damage - numbers in combat( previously used DISPLAYHITMSG for numbers too ) : DISPLAYDAMAGENUMBERS = 1 / 0
//							Added a new section to UOX.INI - [clientsupport] - along with the following settings to determine approved client - versions for the server :
//									CLIENTSUPPORT4000 = 0 / 1
//									CLIENTSUPPORT5000 = 0 / 1
//									CLIENTSUPPORT6000 = 0 / 1
//									CLIENTSUPPORT6050 = 0 / 1
//									CLIENTSUPPORT7000 = 0 / 1
//									CLIENTSUPPORT7090 = 0 / 1
//									CLIENTSUPPORT70160 = 0 / 1
//									CLIENTSUPPORT70240 = 0 / 1
//									NOTE: Each of these settings represent a range of clients, not just the individual versions mentioned.This means that CLIENTSUPPORT4000, for instance,
//									will allow or disallow connections from clients 4.0.0 to 4.0.11f.Also note that while it is possible to enable support for all clients at the same time,
//									it highly recommended to restrict support for client versions that match up to what the server is running.
//							Added new UOX.INI option : EXTENDEDSTARTINGSTATS = 0 / 1
//									If enabled, makes new characters start with 90 statpoints( selectable in clients 7.0.16.0 + only, lower versions only get 90 if using templates ) instead of 80.
//							Added new UOX.INI option : EXTENDEDSTARTINGSKILLS = 0 / 1
//									If enabled, allows for four starting skills( selectable in clients 7.0.16.0 + only, lower versions only get 4th skill if using templates ) instead of three
// November 14, 2012	-	Fixed issue where DISPLAYDAMAGENUMBERS and ATTACKSPEEDFROMSTAMINA settings in UOX.INI were overwriting the DISPLAYHITMSG setting instead of their own respective settings
// November 10, 2013	-	Added new UOX.INI settings for adjusting combat-speed on a global scale:
//									GLOBALATTACKSPEED = 1 	// for adjusting speed of melee and ranged attacks globally for all chars
//									NPCSPELLCASTSPEED = 1 	// for adjusting the overall speed of spellcasts for NPCs (base spellcast speed determined by SPADELAY tag in NPC dfns)
//							Added new UOX.INI settings to make hardcoded damage to armor and weapons in combat more customizable :
//									WEAPONDAMAGECHANCE = 17 	// Chance of weapons taking damage when attacking
//									WEAPONDAMAGEMIN = 0		// Minimum amount of damage a weapon takes if damaged in combat
//									WEAPONDAMAGEMAX = 1		// Maximum amount of damage a weapon takes if damaged in combat
//									ARMORDAMAGECHANCE = 25	// Chance of armor taking damage when defending
//									ARMORDAMAGEMIN = 0		// Minimum amount of damage armor can take if damaged in combat
//									ARMORDAMAGEMAX = 1		// Maximum amount of damage armor can take if damaged in combat
// September 12, 2015 	- 	Added new UOX.INI setting for defining the amount of stamina lost when using the fishing skill
//									FISHINGSTAMINALOSS = 2	// The amount of stamina lost when using the fishing skill
// November 24, 2019 	- 	Added new uox.ini flag for choosing a random starting location for new players (from list in uox.ini)
//									RANDOMSTARTINGLOCATION = 0	// 0 to disable (default), 1 to enable

// NOTE:	Very important the first lookups required duplication or the search fails on them

//+++++++++++++++++++++++++++++++++++++++++++++++
std::int32_t CServerData::lookupINIValue(const std::string& tag) {
	auto iter = uox3inicasevalue.find(tag) ;
	if (iter != uox3inicasevalue.end()){
		return iter->second ;
	}
	return std::numeric_limits<std::int32_t>::max();
}
//+++++++++++++++++++++++++++++++++++++++++++++++
void	CServerData::regAllINIValues() {

	// these numbers dont have to be in order, and can skip aound
	// they just CANT be repeated (now we could add some protection
	// but they dont change that much, and there are a lot, so if you delete one
	// dont worry about reusing it. Just keep adding up from the last one would
	// be easiest.
	// They key is that number HAS to match the case statement number
	// in
	//bool CServerData::HandleLine( const std::string& tag, const std::string& value )
	// in cServerData.cpp (this file).
	regINIValue("SERVERNAME", 1);
	regINIValue("CONSOLELOG", 2);
	regINIValue("COMMANDPREFIX", 3);
	regINIValue("ANNOUNCEWORLDSAVES", 4);
	regINIValue("BACKUPSENABLED", 5);
	regINIValue("SAVESTIMER", 6);
	regINIValue("SKILLCAP", 7);
	regINIValue("SKILLDELAY", 8);
	regINIValue("STATCAP", 9);
	regINIValue("MAXSTEALTHMOVEMENTS", 10);
	regINIValue("MAXSTAMINAMOVEMENTS", 11);
	regINIValue("ARMORAFFECTMANAREGEN", 12);
	regINIValue("CORPSEDECAYTIMER", 13);
	regINIValue("WEATHERTIMER", 14);
	regINIValue("SHOPSPAWNTIMER", 15);
	regINIValue("DECAYTIMER", 16);
	regINIValue("INVISIBILITYTIMER", 17);
	regINIValue("OBJECTUSETIMER", 18);
	regINIValue("GATETIMER", 19);
	regINIValue("POISONTIMER", 20);
	regINIValue("LOGINTIMEOUT", 21);
	regINIValue("HITPOINTREGENTIMER", 22);
	regINIValue("STAMINAREGENTIMER", 23);
	regINIValue("BASEFISHINGTIMER",24);
	regINIValue("SCRIPTSDIRECTORY", 25);
	regINIValue("JOINPARTMSGS", 26);
	//HERE
	regINIValue("MANAREGENTIMER", 37);
	regINIValue("RANDOMFISHINGTIMER", 38);
	regINIValue("SPIRITSPEAKTIMER", 39);
	regINIValue("DIRECTORY", 40);
	regINIValue("DATADIRECTORY", 41);
	regINIValue("DEFSDIRECTORY", 42);
	regINIValue("ACTSDIRECTORY", 43);
	regINIValue("BACKUPDIRECTORY", 44);
	regINIValue("MSGBOARDDIRECTORY", 45);
	regINIValue("SHAREDDIRECTORY", 46);
	regINIValue("LOOTDECAYSWITHCORPSE", 47);
	regINIValue("GUARDSACTIVE", 49);
	regINIValue("DEATHANIMATION", 27);
	regINIValue("AMBIENTSOUNDS", 50);
	regINIValue("AMBIENTFOOTSTEPS", 51);
	regINIValue("INTERNALACCOUNTCREATION", 52);
	regINIValue("SHOWOFFLINEPCS", 53);
	regINIValue("ROGUESENABLED", 54);
	regINIValue("PLAYERPERSECUTION", 55);
	regINIValue("ACCOUNTFLUSH", 56);
	regINIValue("HTMLSTATUSENABLED", 57);
	regINIValue("SELLBYNAME", 58);
	regINIValue("SELLMAXITEMS", 59);
	regINIValue("TRADESYSTEM", 60);
	regINIValue("RANKSYSTEM", 61);
	regINIValue("CUTSCROLLREQUIREMENTS", 62);
	regINIValue("CHECKITEMS", 63);
	regINIValue("CHECKBOATS", 64);
	regINIValue("CHECKNPCAI", 65);
	regINIValue("CHECKSPAWNREGIONS", 66);
	regINIValue("POSTINGLEVEL", 67);
	regINIValue("REMOVALLEVEL", 68);
	regINIValue("ESCORTENABLED", 69);
	regINIValue("ESCORTINITEXPIRE", 70);
	regINIValue("ESCORTACTIVEEXPIRE", 71);
	regINIValue("MOON1", 72);
	regINIValue("MOON2", 73);
	regINIValue("DUNGEONLEVEL", 74);
	regINIValue("CURRENTLEVEL", 75);
	regINIValue("BRIGHTLEVEL", 76);
	regINIValue("BASERANGE", 77);
	regINIValue("BASETIMER", 78);
	regINIValue("MAXTARGETS", 79);
	regINIValue("MSGREDISPLAYTIME", 80);
	regINIValue("MURDERDECAYTIMER", 81);
	regINIValue("MAXKILLS", 82);
	regINIValue("CRIMINALTIMER", 83);
	regINIValue("MINECHECK", 84);
	regINIValue("OREPERAREA", 85);
	regINIValue("ORERESPAWNTIMER", 86);
	regINIValue("ORERESPAWNAREA", 87);
	regINIValue("LOGSPERAREA", 88);
	regINIValue("LOGSRESPAWNTIMER", 89);
	regINIValue("LOGSRESPAWNAREA", 90);
	regINIValue("HUNGERRATE", 91);
	regINIValue("HUNGERDMGVAL", 92);
	regINIValue("MAXRANGE", 93);
	regINIValue("SPELLMAXRANGE",94);
	regINIValue("DISPLAYHITMSG", 95);
	regINIValue("MONSTERSVSANIMALS", 96);
	regINIValue("ANIMALATTACKCHANCE", 97);
	regINIValue("ANIMALSGUARDED", 98);
	regINIValue("NPCDAMAGERATE", 99);
	regINIValue("NPCBASEFLEEAT", 100);
	regINIValue("NPCBASEREATTACKAT", 101);
	regINIValue("ATTACKSTAMINA", 102);
	regINIValue("LOCATION", 103);
	regINIValue("STARTGOLD", 104);
	regINIValue("STARTPRIVS", 105);
	regINIValue("ESCORTDONEEXPIRE", 106);
	regINIValue("DARKLEVEL", 107);
	regINIValue("TITLECOLOUR", 108);
	regINIValue("LEFTTEXTCOLOUR", 109);
	regINIValue("RIGHTTEXTCOLOUR", 110);
	regINIValue("BUTTONCANCEL", 111);
	regINIValue("BUTTONLEFT", 112);
	regINIValue("BUTTONRIGHT", 113);
	regINIValue("BACKGROUNDPIC", 114);
	regINIValue("POLLTIME", 115);
	regINIValue("MAYORTIME", 116);
	regINIValue("TAXPERIOD", 117);
	regINIValue("GUARDSPAID", 118);
	regINIValue("DAY", 119);
	regINIValue("HOURS", 120);
	regINIValue("MINUTES", 121);
	regINIValue("SECONDS", 122);
	regINIValue("AMPM", 123);
	regINIValue("SKILLLEVEL", 124);
	regINIValue("SNOOPISCRIME", 125);
	regINIValue("BOOKSDIRECTORY", 126);
	//regINIValue("SERVERLIST", 127);
	regINIValue("PORT", 128);
	regINIValue("ACCESSDIRECTORY", 129);
	regINIValue("LOGSDIRECTORY", 130);
	regINIValue("ACCOUNTISOLATION", 131);
	regINIValue("HTMLDIRECTORY", 132);
	regINIValue("SHOOTONANIMALBACK", 133);
	regINIValue("NPCTRAININGENABLED", 134);
	regINIValue("DICTIONARYDIRECTORY", 135);
	regINIValue("BACKUPSAVERATIO", 136);
	regINIValue("HIDEWILEMOUNTED", 137);
	regINIValue("SECONDSPERUOMINUTE", 138);
	regINIValue("WEIGHTPERSTR", 139);
	regINIValue("POLYDURATION", 140);
	regINIValue("UOGENABLED", 141);
	regINIValue("NETRCVTIMEOUT", 142);
	regINIValue("NETSNDTIMEOUT", 143);
	regINIValue("NETRETRYCOUNT", 144);
	regINIValue("CLIENTFEATURES", 145);
	regINIValue("OVERLOADPACKETS", 146);
	regINIValue("NPCMOVEMENTSPEED", 147);
	regINIValue("PETHUNGEROFFLINE", 148);
	regINIValue("PETOFFLINETIMEOUT", 149);
	regINIValue("PETOFFLINECHECKTIMER", 150);
	regINIValue("ARCHERRANGE", 151);
	regINIValue("ADVANCEDPATHFINDING", 152);
	regINIValue("SERVERFEATURES", 153);
	regINIValue("LOOTINGISCRIME", 154);
	regINIValue("NPCRUNNINGSPEED", 155);
	regINIValue("NPCFLEEINGSPEED", 156);
	regINIValue("BASICTOOLTIPSONLY", 157);
	regINIValue("GLOBALITEMDECAY", 158);
	regINIValue("SCRIPTITEMSDECAYABLE", 159);
	regINIValue("BASEITEMSDECAYABLE", 160);
	regINIValue("ITEMDECAYINHOUSES", 161);
	regINIValue("COMBATEXPLODEDELAY", 162);
	regINIValue("PAPERDOLLGUILDBUTTON", 163);
	regINIValue("ATTACKSPEEDFROMSTAMINA", 164);
	regINIValue("DISPLAYDAMAGENUMBERS", 169);
	regINIValue("CLIENTSUPPORT4000", 170);
	regINIValue("CLIENTSUPPORT5000", 171);
	regINIValue("CLIENTSUPPORT6000", 172);
	regINIValue("CLIENTSUPPORT6050", 173);
	regINIValue("CLIENTSUPPORT7000", 174);
	regINIValue("CLIENTSUPPORT7090", 175);
	regINIValue("CLIENTSUPPORT70160", 176);
	regINIValue("CLIENTSUPPORT70240", 177);
	regINIValue("CLIENTSUPPORT70300", 178);
	regINIValue("CLIENTSUPPORT70331", 179);
	regINIValue("CLIENTSUPPORT704565", 180);
	regINIValue("CLIENTSUPPORT70610", 181);
	regINIValue("EXTENDEDSTARTINGSTATS", 182);
	regINIValue("EXTENDEDSTARTINGSKILLS", 183);
	regINIValue("WEAPONDAMAGECHANCE", 184);
	regINIValue("ARMORDAMAGECHANCE", 185);
	regINIValue("WEAPONDAMAGEMIN", 186);
	regINIValue("WEAPONDAMAGEMAX", 187);
	regINIValue("ARMORDAMAGEMIN", 188);
	regINIValue("ARMORDAMAGEMAX", 189);
	regINIValue("GLOBALATTACKSPEED", 190);
	regINIValue("NPCSPELLCASTSPEED", 191);
	regINIValue("FISHINGSTAMINALOSS", 192);
	regINIValue("RANDOMSTARTINGLOCATION", 193);
	regINIValue("ASSISTANTNEGOTIATION", 194);
	regINIValue("KICKONASSISTANTSILENCE", 195);
	regINIValue("AF_FILTERWEATHER", 196);
	regINIValue("AF_FILTERLIGHT", 197);
	regINIValue("AF_SMARTTARGET", 198);
	regINIValue("AF_RANGEDTARGET", 199);
	regINIValue("AF_AUTOOPENDOORS", 200);
	regINIValue("AF_DEQUIPONCAST", 201);
	regINIValue("AF_AUTOPOTIONEQUIP", 202);
	regINIValue("AF_POISONEDCHECKS", 203);
	regINIValue("AF_LOOPEDMACROS", 204);
	regINIValue("AF_USEONCEAGENT", 205);
	regINIValue("AF_RESTOCKAGENT", 206);
	regINIValue("AF_SELLAGENT", 207);
	regINIValue("AF_BUYAGENT", 208);
	regINIValue("AF_POTIONHOTKEYS", 209);
	regINIValue("AF_RANDOMTARGETS", 210);
	regINIValue("AF_CLOSESTTARGETS", 211);
	regINIValue("AF_OVERHEADHEALTH", 212);
	regINIValue("AF_AUTOLOOTAGENT", 213);
	regINIValue("AF_BONECUTTERAGENT", 214);
	regINIValue("AF_JSCRIPTMACROS", 215);
	regINIValue("AF_AUTOREMOUNT", 216);
	regINIValue("AF_ALL", 217);
	regINIValue("CLASSICUOMAPTRACKER", 218);
	regINIValue("DECAYTIMERINHOUSE", 219);
	regINIValue("PROTECTPRIVATEHOUSES", 220);
	regINIValue("TRACKHOUSESPERACCOUNT", 221);
	regINIValue("MAXHOUSESOWNABLE", 222);
	regINIValue("MAXHOUSESCOOWNABLE", 223);
	regINIValue("CANOWNANDCOOWNHOUSES", 224);
	regINIValue("COOWNHOUSESONSAMEACCOUNT", 225);
	regINIValue("ITEMSDETECTSPEECH", 226);
	regINIValue("MAXPLAYERPACKITEMS", 227);
	regINIValue("MAXPLAYERBANKITEMS", 228);
	regINIValue("FORCENEWANIMATIONPACKET", 229);
	regINIValue("MAPDIFFSENABLED", 230);
	regINIValue("CUOENABLED", 244);
	regINIValue("ALCHEMYBONUSENABLED", 245);
	regINIValue("ALCHEMYBONUSMODIFIER", 246);
	regINIValue("NPCFLAGUPDATETIMER", 247);
	regINIValue("JSENGINESIZE", 248);
	regINIValue("SCRIPTDATADIRECTORY", 250);
	regINIValue("THIRSTRATE", 251);
	regINIValue("THIRSTDRAINVAL", 252);
	regINIValue("PETTHIRSTOFFLINE", 253);
	regINIValue("EXTERNALIP",254);
	regINIValue("BLOODDECAYTIMER", 255);
	regINIValue("BLOODDECAYCORPSETIMER", 256);
	regINIValue("BLOODEFFECTCHANCE", 257);
	regINIValue("NPCCORPSEDECAYTIMER", 258);
	regINIValue("HUNGERENABLED", 259);
	regINIValue("THIRSTENABLED", 260);
	regINIValue("TRAVELSPELLSFROMBOATKEYS", 261);
	regINIValue("TRAVELSPELLSWHILEOVERWEIGHT", 262);
	regINIValue("MARKRUNESINMULTIS", 263);
	regINIValue("TRAVELSPELLSBETWEENWORLDS", 264);
	regINIValue("TRAVELSPELLSWHILEAGGRESSOR", 265);
	regINIValue("BANKBUYTHRESHOLD", 266);
	regINIValue("NETWORKLOG", 267);
	regINIValue("SPEECHLOG", 268);
	regINIValue("NPCMOUNTEDWALKINGSPEED", 269);
	regINIValue("NPCMOUNTEDRUNNINGSPEED", 270);
	regINIValue("NPCMOUNTEDFLEEINGSPEED", 271);


}
//+++++++++++++++++++++++++++++++++++++++++++++++
void	CServerData::regINIValue(const std::string& tag, std::int32_t value){
	uox3inicasevalue.insert_or_assign(tag,value);
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ResetDefaults( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reset server settings to default
//o-----------------------------------------------------------------------------------------------o
void CServerData::ResetDefaults( void )
{
	resettingDefaults = true;
	serverList.resize( 1 );		// Set the initial count to hold one server.

	ServerIP( "127.0.0.1" );
	ServerPort( 2593 );
	ExternalIP("127.0.0.1");
	serverList[0].setPort( 2593 );
	ServerName( "My UOX3 Shard" );

	// Set default gcMaxBytes limit in MB per JS runtime
	// If set too low, UOX3 might crash when reloading (full) JS engine
	// JS API references describe it as "Maximum nominal heap before last ditch GC"
	SetJSEngineSize( 256 );

	SystemTimer( tSERVER_POTION, 10 );
	ServerMoon( 0, 0 );
	ServerMoon( 1, 0 );
	DungeonLightLevel( 3 );
	WorldLightCurrentLevel( 0 );
	WorldLightBrightLevel( 0 );
	WorldLightDarkLevel( 5 );

	ServerSecondsPerUOMinute( 5 );
	ServerTimeDay( 0 );
	ServerTimeHours( 0 );
	ServerTimeMinutes( 0 );
	ServerTimeSeconds( 0 );
	ServerTimeAMPM( 0 );

	InternalAccountStatus( false );
	CombatMaxRange( 10 );
	CombatMaxSpellRange( 10 );
	CombatExplodeDelay( 2 );

	// load defaults values
	SystemTimer( tSERVER_SHOPSPAWN, 300 );
	SystemTimer( tSERVER_POISON, 180 );
	SystemTimer( tSERVER_DECAY, 300 );
	SystemTimer( tSERVER_DECAYINHOUSE, 3600 );
	ServerSkillTotalCap( 7000 );
	ServerSkillCap( 1000 );
	ServerStatCap( 325 );
	CorpseLootDecay( true );
	ServerSavesTimer( 600 );

	SystemTimer( tSERVER_INVISIBILITY, 60 );
	SystemTimer( tSERVER_HUNGERRATE, 6000 );
	HungerDamage( 2 );
	HungerSystemEnabled( true );

	SystemTimer( tSERVER_THIRSTRATE, 6000 );
	ThirstDrain( 2 );
	ThirstSystemEnabled( false );

	ServerSkillDelay( 5 );
	SystemTimer( tSERVER_OBJECTUSAGE, 1 );
	SystemTimer( tSERVER_HITPOINTREGEN, 8 );
	SystemTimer( tSERVER_STAMINAREGEN, 3 );
	SystemTimer( tSERVER_MANAREGEN, 5 );
	ArmorAffectManaRegen( true );
	SystemTimer( tSERVER_GATE, 30 );
	MineCheck( 1 );
	DeathAnimationStatus( true );
	ShowOfflinePCs( true );
	CombatDisplayHitMessage( true );
	CombatDisplayDamageNumbers( true );
	CombatAttackSpeedFromStamina( true );
	CombatAttackStamina( -2 );
	NPCTrainingStatus( true );
	CharHideWhileMounted( true );
	WeightPerStr( 3.5 );
	SystemTimer( tSERVER_POLYMORPH, 90 );
	ServerOverloadPackets( true );
	AdvancedPathfinding( true );
	LootingIsCrime( true );
	ServerUOGEnabled( true );
	ConnectUOServerPoll( true );

	CombatMonstersVsAnimals( true );
	CombatAnimalsAttackChance( 5 );
	CombatAnimalsGuarded( false );
	CombatNPCBaseFleeAt( 20 );
	CombatNPCBaseReattackAt( 40 );
	ShootOnAnimalBack( false );
	SellByNameStatus( false );
	SkillLevel( 5 );
	SellMaxItemsStatus( 5 );
	CombatNPCDamageRate( 2 );
	RankSystemStatus( true );
	CombatWeaponDamageChance( 17 );
	CombatWeaponDamageMin( 0 );
	CombatWeaponDamageMax( 1 );
	CombatArmorDamageChance( 33 );
	CombatArmorDamageMin( 0 );
	CombatArmorDamageMax( 1 );
	CombatBloodEffectChance( 75 );
	GlobalAttackSpeed( 1.0 );
	NPCSpellCastSpeed( 1.0 );
	FishingStaminaLoss( 2.0 );
	AlchemyDamageBonusEnabled( false );
	AlchemyDamageBonusModifier( 5 );

	auto curWorkingDir = std::filesystem::current_path().string();

	auto wDir = strutil::fixDirectory(curWorkingDir);
	std::string tDir;
	Directory( CSDDP_ROOT, wDir );
	tDir = wDir + std::string("muldata/");
	Directory( CSDDP_DATA, tDir );
	tDir = wDir + std::string("dfndata/");
	Directory( CSDDP_DEFS, tDir );
	tDir = wDir + std::string("accounts/");
	Directory( CSDDP_ACCOUNTS, tDir );
	Directory( CSDDP_ACCESS, tDir );
	tDir = wDir + std::string("js/");
	Directory( CSDDP_SCRIPTS, tDir );
	tDir = wDir + std::string( "js/jsdata" );
	Directory( CSDDP_SCRIPTDATA, tDir );
	tDir = wDir + std::string("archives/");
	Directory( CSDDP_BACKUP, tDir );
	tDir = wDir + std::string("msgboards/");
	Directory( CSDDP_MSGBOARD, tDir );
	tDir = wDir + std::string("shared/");
	Directory( CSDDP_SHARED, tDir );
	tDir = wDir + std::string("html/");
	Directory( CSDDP_HTML, tDir );
	tDir = wDir + std::string("books/");
	Directory( CSDDP_BOOKS, tDir );
	tDir = wDir + std::string("dictionaries/");
	Directory( CSDDP_DICTIONARIES, tDir );
	tDir = wDir + std::string("logs/");
	Directory( CSDDP_LOGS, tDir );

	BuyThreshold( 2000 );
	GuardStatus( true );
	ServerAnnounceSaves( true );
	WorldAmbientSounds( 5 );
	ServerJoinPartAnnouncements( true );
	ServerConsoleLog( true );
	ServerNetworkLog( false );
	ServerSpeechLog( false );
	RogueStatus( true );
	SystemTimer( tSERVER_WEATHER, 60 );
	SystemTimer( tSERVER_LOGINTIMEOUT, 300 );
	BackupRatio( 5 );
	MaxStealthMovement( 10 );
	MaxStaminaMovement( 15 );
	SnoopIsCrime( false );
	SystemTimer( tSERVER_NPCFLAGUPDATETIMER, 5 );
	PetOfflineTimeout( 5 );
	PetHungerOffline( true );
	SystemTimer( tSERVER_PETOFFLINECHECK, 600 );
	ItemsDetectSpeech( false );
	MaxPlayerPackItems( 125 );
	MaxPlayerBankItems( 125 );
	ForceNewAnimationPacket( true );
	MapDiffsEnabled( false );
	TravelSpellsFromBoatKeys( true );
	TravelSpellsWhileOverweight( false );
	MarkRunesInMultis( true );
	TravelSpellsBetweenWorlds( false );
	TravelSpellsWhileAggressor( false );

	CheckBoatSpeed( 0.65 );
	CheckNpcAISpeed( 1 );
	CutScrollRequirementStatus( true );
	PlayerPersecutionStatus( false );
	HtmlStatsStatus( -1 );

	MsgBoardPostingLevel( 0 );
	MsgBoardPostRemovalLevel( 0 );
	// No replacement I can see
	EscortsEnabled( true );
	BasicTooltipsOnly( false );
	GlobalItemDecay( true );
	ScriptItemsDecayable( true );
	BaseItemsDecayable( false );
	SystemTimer( tSERVER_ESCORTWAIT, 900 );
	SystemTimer( tSERVER_ESCORTACTIVE, 600 );
	SystemTimer( tSERVER_ESCORTDONE, 600 );
	AmbientFootsteps( false );
	ServerCommandPrefix( '\'' );

	CheckSpawnRegionSpeed( 30 );
	CheckItemsSpeed( 1.5 );
	NPCWalkingSpeed( 0.38 );
	NPCRunningSpeed( 0.2 );
	NPCFleeingSpeed( 0.3 );
	NPCMountedWalkingSpeed( 0.3 );
	NPCMountedRunningSpeed( 0.12 );
	NPCMountedFleeingSpeed( 0.2 );
	AccountFlushTimer( 0.0 );

	ResLogs( 3 );
	ResLogTime( 600 );
	ResLogArea( 10 );
	ResOre( 10 );
	ResOreTime( 600 );
	ResOreArea( 10 );
	//REPSYS
	SystemTimer( tSERVER_CRIMINAL, 120 );
	RepMaxKills( 4 );
	SystemTimer( tSERVER_MURDERDECAY, 60 );
	//RepSys ---^
	TrackingBaseRange( 10 );
	TrackingMaxTargets( 20 );
	TrackingBaseTimer( 30 );
	TrackingRedisplayTime( 30 );

	SystemTimer( tSERVER_FISHINGBASE, 10 );
	SystemTimer( tSERVER_FISHINGRANDOM, 5 );
	SystemTimer( tSERVER_SPIRITSPEAK, 30 );

	TitleColour( 0 );
	LeftTextColour( 0 );
	RightTextColour( 0 );
	ButtonCancel( 4017 );
	ButtonLeft( 4014 );
	ButtonRight( 4005 );
	BackgroundPic( 2600 );

	// Houses
	ItemDecayInHouses( true );
	ProtectPrivateHouses( true );
	TrackHousesPerAccount( true );
	CanOwnAndCoOwnHouses( true );
	CoOwnHousesOnSameAccount( true );
	MaxHousesOwnable( 1 );
	MaxHousesCoOwnable( 10 );

	// Towns
	TownNumSecsPollOpen( 3600 );	// one hour
	TownNumSecsAsMayor( 36000 );	// 10 hours as mayor
	TownTaxPeriod( 1800 );			// taxed every 30 minutes
	TownGuardPayment( 3600 );		// guards paid every hour

	SetClientFeature( CF_BIT_CHAT, true );
	SetClientFeature( CF_BIT_UOR, true );
	SetClientFeature( CF_BIT_TD, true );
	SetClientFeature( CF_BIT_LBR, true );
	SetClientFeature( CF_BIT_AOS, true );
	SetClientFeature( CF_BIT_SIXCHARS, true );
	SetClientFeature( CF_BIT_SE, true );
	SetClientFeature( CF_BIT_ML, true );
	SetClientFeature( CF_BIT_EXPANSION, true );

	SetServerFeature( SF_BIT_CONTEXTMENUS, true );
	SetServerFeature( SF_BIT_AOS, true );
	SetServerFeature( SF_BIT_SIXCHARS, true );
	SetServerFeature( SF_BIT_SE, true );
	SetServerFeature( SF_BIT_ML, true );

	SetDisabledAssistantFeature( AF_NONE, true );
	SetDisabledAssistantFeature( AF_FILTERWEATHER, false );
	SetDisabledAssistantFeature( AF_FILTERLIGHT, false );
	SetDisabledAssistantFeature( AF_SMARTTARGET, false );
	SetDisabledAssistantFeature( AF_RANGEDTARGET, false );
	SetDisabledAssistantFeature( AF_AUTOOPENDOORS, false );
	SetDisabledAssistantFeature( AF_DEQUIPONCAST, false );
	SetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP, false );
	SetDisabledAssistantFeature( AF_POISONEDCHECKS, false );
	SetDisabledAssistantFeature( AF_LOOPEDMACROS, false );
	SetDisabledAssistantFeature( AF_USEONCEAGENT, false );
	SetDisabledAssistantFeature( AF_RESTOCKAGENT, false );
	SetDisabledAssistantFeature( AF_SELLAGENT, false );
	SetDisabledAssistantFeature( AF_BUYAGENT, false );
	SetDisabledAssistantFeature( AF_POTIONHOTKEYS, false );
	SetDisabledAssistantFeature( AF_RANDOMTARGETS, false );
	SetDisabledAssistantFeature( AF_CLOSESTTARGETS, false );
	SetDisabledAssistantFeature( AF_OVERHEADHEALTH, false );
	SetDisabledAssistantFeature( AF_AUTOLOOTAGENT, false );
	SetDisabledAssistantFeature( AF_BONECUTTERAGENT, false );
	SetDisabledAssistantFeature( AF_JSCRIPTMACROS, false );
	SetDisabledAssistantFeature( AF_AUTOREMOUNT, false );
	SetDisabledAssistantFeature( AF_ALL, false );

	// Enable login-support for any supported client version by default.
	ClientSupport4000( false );
	ClientSupport5000( false );
	ClientSupport6000( false );
	ClientSupport6050( false );
	ClientSupport7000( false );
	ClientSupport7090( false );
	ClientSupport70160( false );
	ClientSupport70240( false );
	ClientSupport70300( false );
	ClientSupport70331( true );
	ClientSupport704565( true );
	ClientSupport70610( true );

	ExtendedStartingStats( true );
	ExtendedStartingSkills( true );

	ServerRandomStartingLocation( false );
	ServerStartGold( 1000 );
	ServerStartPrivs( 0 );
	SystemTimer( tSERVER_CORPSEDECAY, 420 );
	SystemTimer( tSERVER_NPCCORPSEDECAY, 420 );
	SystemTimer( tSERVER_BLOODDECAYCORPSE, 210 ); // Default to half the decay timer of a npc corpse
	SystemTimer( tSERVER_BLOODDECAY, 3 ); // Keep it short and sweet
	resettingDefaults = false;
	PostLoadDefaults();
}
CServerData::CServerData( void )
{
	ResetDefaults();
	regAllINIValues();
}
CServerData::~CServerData()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RefreshIPs( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Refresh IPs for servers in serverlist
//o-----------------------------------------------------------------------------------------------o
/*void CServerData::RefreshIPs( void )
{
	struct hostent *lpHostEntry = nullptr;

	std::vector< physicalServer >::iterator slIter;
	for( slIter = serverList.begin(); slIter != serverList.end(); ++slIter )
	{
		if( !slIter->getDomain().empty() )
		{
			lpHostEntry = gethostbyname( slIter->getDomain().c_str() );
			if( lpHostEntry != nullptr )
			{
				struct in_addr *pinaddr;
				pinaddr = ((struct in_addr*)lpHostEntry->h_addr);
				slIter->setIP( inet_ntoa(*pinaddr) );
			}
		}
	}
}*/

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string ServerName( void ) const
//|					void ServerName( std::string setname )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server name. Sets to specified value, or to default name if no value specified
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ServerName( void ) const
{
	return serverList[0].getName();
}
void CServerData::ServerName( std::string setname )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	serverList[0].setName( setname );
	if( setname.empty() )
	{
		serverList[0].setName( "My UOX3 Shard" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string ServerDomain( void ) const
//|					void ServerDomain( std::string setdomain )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server domain. Sets to specified value, or to no domain if no value specified
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ServerDomain( void ) const
{
	return serverList[0].getDomain();
}
void CServerData::ServerDomain( std::string setdomain )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	if( setdomain.empty() )
		serverList[0].setDomain( "" );
	else
		serverList[0].setDomain( setdomain );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string CServerData::ServerIP( void ) const
//|					void ServerIP( std::string setip )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server IP. Sets to specified value, or to loopback IP if no value specified
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ServerIP( void ) const
{
	return serverList[0].getIP();
}
void CServerData::ServerIP( std::string setip )
{
	if( serverList.empty() )
		serverList.resize( 1 );
	if( setip.empty() )
		serverList[0].setIP("127.0.0.1");
	else
		serverList[0].setIP(setip);
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string CServerData::ExternalIP() const
//|					void CServerData::ExternalIP( const std::string &ip )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets external (WAN) IP for server
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::ExternalIP() const
{
	return externalIP;
}
void CServerData::ExternalIP( const std::string &ip )
{
	externalIP = ip;
	IP4Address::setExternal( externalIP );
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerPort( void ) const
//|					void ServerPort( UI16 setport )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set server port to specified value, or to default port if no value specified
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerPort( void ) const
{
	return port;
}
void CServerData::ServerPort( UI16 setport )
{
	if( setport == 0 )
		port = 2593;
	else
		port = setport;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerConsoleLog( void ) const
//|					void ServerConsoleLog( bool setting )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set logging of console messages, warnings and errors
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerConsoleLog( void ) const
{
	return consolelogenabled;
}
void CServerData::ServerConsoleLog( bool setting )
{
	consolelogenabled = setting;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerNetworkLog( void ) const
//|					void ServerNetworkLog( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets logging of network traffic to logs folder
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerNetworkLog( void ) const
{
	return boolVals.test( BIT_NETWORKLOG );
}
void CServerData::ServerNetworkLog( bool newVal )
{
	boolVals.set( BIT_NETWORKLOG, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerSpeechLog( void ) const
//|					void ServerSpeechLog( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets logging of player/staff speech to logs folder
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerSpeechLog( void ) const
{
	return boolVals.test( BIT_SPEECHLOG );
}
void CServerData::ServerSpeechLog( bool newVal )
{
	boolVals.set( BIT_SPEECHLOG, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	char ServerCommandPrefix( void ) const
//|					void ServerCommandPrefix( char cmdvalue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set command prefix
//o-----------------------------------------------------------------------------------------------o
char CServerData::ServerCommandPrefix( void ) const
{
	return commandprefix;
}
void CServerData::ServerCommandPrefix( char cmdvalue )
{
	commandprefix = cmdvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerAnnounceSavesStatus( void ) const
//|					void ServerAnnounceSaves( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Set status of server accouncements for worldsaves
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerAnnounceSavesStatus( void ) const
{
	return boolVals.test( BIT_ANNOUNCESAVES );
}
void CServerData::ServerAnnounceSaves( bool newVal )
{
	boolVals.set( BIT_ANNOUNCESAVES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerJoinPartAnnouncementsStatus( void ) const
//|					void ServerJoinPartAnnouncements( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of server accouncements for players connecting/disconnecting
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerJoinPartAnnouncementsStatus( void ) const
{
	return boolVals.test( BIT_ANNOUNCEJOINPART );
}
void CServerData::ServerJoinPartAnnouncements( bool newVal )
{
	boolVals.set( BIT_ANNOUNCEJOINPART, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerBackupStatus( void ) const
//|					void ServerBackups( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets enabled status of server worldsave backups
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerBackupStatus( void ) const
{
	return boolVals.test( BIT_SERVERBACKUP );
}
void CServerData::ServerBackups( bool newVal )
{
	boolVals.set( BIT_SERVERBACKUP, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 ServerSavesTimerStatus( void ) const
//|					void ServerSavesTimer( UI32 timer )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server world save timer
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::ServerSavesTimerStatus( void ) const
{
	return serversavestimer;
}
void CServerData::ServerSavesTimer( UI32 timer )
{
	serversavestimer = timer;
	if( timer < 180 )			// 3 minutes is the lowest value you can set saves for
		serversavestimer = 300;	// 10 minutes default
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSkillCapStatus( void ) const
//|					void ServerSkillCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cap for individual skills
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerSkillCapStatus( void ) const
{
	return skillcap;
}
void CServerData::ServerSkillCap( UI16 cap )
{
	skillcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skillcap = SKILLCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSkillTotalCapStatus( void ) const
//|					void ServerSkillTotalCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets total cap for all skills
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerSkillTotalCapStatus( void ) const
{
	return skilltotalcap;
}
void CServerData::ServerSkillTotalCap( UI16 cap )
{
	skilltotalcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		skilltotalcap = SKILLTOTALCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 ServerSkillDelayStatus( void ) const
//|					void ServerSkillDelay( UI08 skdelay )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global delay for skill usage
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::ServerSkillDelayStatus( void ) const
{
	return skilldelay;
}
void CServerData::ServerSkillDelay( UI08 skdelay )
{
	skilldelay = skdelay;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerStatCapStatus( void ) const
//|					void ServerStatCap( UI16 cap )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the total stat cap
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerStatCapStatus( void ) const
{
	return statcap;
}
void CServerData::ServerStatCap( UI16 cap )
{
	statcap = cap;
	if( cap < 1 )		// Default is on second loop sleeping
		statcap = STATCAP;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 MaxStealthMovement( void ) const
//|					void MaxStealthMovement( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed to take while stealthed
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::MaxStealthMovement( void ) const
{
	return maxstealthmovement;
}
void CServerData::MaxStealthMovement( SI16 value )
{
	maxstealthmovement = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 MaxStaminaMovement( void ) const
//|					void MaxStaminaMovement( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max number of steps allowed while running before stamina is reduced
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::MaxStaminaMovement( void ) const
{
	return maxstaminamovement;
}
void CServerData::MaxStaminaMovement( SI16 value )
{
	maxstaminamovement = value;
}

TIMERVAL CServerData::BuildSystemTimeValue( cSD_TID timerID ) const
{
	return BuildTimeValue( static_cast<R32>(SystemTimer( timerID )) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 SystemTimer( cSD_TID timerid ) const
//|					void SystemTimer( cSD_TID timerid, UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server timer
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::SystemTimer( cSD_TID timerid ) const
{
	return serverTimers[timerid];
}
void CServerData::SystemTimer( cSD_TID timerid, UI16 value )
{
	serverTimers[timerid] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string Directory( CSDDirectoryPaths dp )
//|					void Directory( CSDDirectoryPaths dp, std::string value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets directory paths
//o-----------------------------------------------------------------------------------------------o
std::string CServerData::Directory( CSDDirectoryPaths dp )
{
	std::string rvalue;
	if( dp != CSDDP_COUNT )
		rvalue = serverDirectories[dp];
	return rvalue;
}
void CServerData::Directory( CSDDirectoryPaths dp, std::string value )
{
	bool create_dir = false;

	if( dp != CSDDP_COUNT )
	{
		std::string verboseDirectory;
		switch( dp )
		{
			case CSDDP_ROOT:			verboseDirectory = "Root directory";			break;
			case CSDDP_DATA:			verboseDirectory = "Data directory";			break;
			case CSDDP_DEFS:			verboseDirectory = "DFNs directory";			break;
			case CSDDP_ACCESS:			verboseDirectory = "Access directory";			break;
			case CSDDP_ACCOUNTS:		verboseDirectory = "Accounts directory";		break;
			case CSDDP_SCRIPTS:			verboseDirectory = "Scripts directory";			break;
			case CSDDP_SCRIPTDATA:		verboseDirectory = "ScriptData directory";
				create_dir = true;
				break;
			case CSDDP_BACKUP:			verboseDirectory = "Backup directory";			break;
			case CSDDP_MSGBOARD:		verboseDirectory = "Messageboard directory";	break;
			case CSDDP_SHARED:			verboseDirectory = "Shared directory";
				create_dir = true;
				break;
			case CSDDP_HTML:			verboseDirectory = "HTML directory";			break;
			case CSDDP_LOGS:			verboseDirectory = "Logs directory";			break;
			case CSDDP_DICTIONARIES:	verboseDirectory = "Dictionary directory";		break;
			case CSDDP_BOOKS:			verboseDirectory = "Books directory";			break;
			case CSDDP_COUNT:
			default:					verboseDirectory = "Unknown directory";			break;
		};
		// First, let's normalize the path name and fix common errors
		// remove all trailing and leading spaces...
		auto sText = strutil::trim(value) ;

		if( sText.empty() )
		{
			Console.error( strutil::format(" %s directory is blank, set in uox.ini", verboseDirectory.c_str() ));
			Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
			return;
		}

		// Make sure we're terminated with a directory separator
		// Just incase it's not set in the .ini
		// and convert the windows backward slashes to forward slashes

		sText = strutil::fixDirectory(sText);

		bool error = false;
		if( !resettingDefaults )
		{
			error = true;
			//auto curWorkingDir = fixDirectory(std::filesystem::current_path().string());

			auto npath = std::filesystem::path(sText);

			if (std::filesystem::exists(npath)) {
				error = false;
			}
			else if (create_dir) {
				// Create missing directory
				std::filesystem::create_directory(npath);
				error = false;
			}
		}

		if( error )
		{
			Console.error( strutil::format("%s %s does not exist", verboseDirectory.c_str(), sText.c_str()) );
			Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
		}
		else
		{
			// There was a check to see if text was empty, to set to "./".  However, if text was blank, we bailed out in the
			// beginning of the routine
			serverDirectories[dp] = sText;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShootOnAnimalBack( void ) const
//|					void ShootOnAnimalBack( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can shoot arrows while riding mounts
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ShootOnAnimalBack( void ) const
{
	return boolVals.test( BIT_SHOOTONANIMALBACK );
}
void CServerData::ShootOnAnimalBack( bool newVal )
{
	boolVals.set( BIT_SHOOTONANIMALBACK, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool NPCTrainingStatus( void ) const
//|					void NPCTrainingStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs can train players in skills
//o-----------------------------------------------------------------------------------------------o
bool CServerData::NPCTrainingStatus( void ) const
{
	return boolVals.test( BIT_NPCTRAINING );
}
void CServerData::NPCTrainingStatus( bool newVal )
{
	boolVals.set( BIT_NPCTRAINING, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void dumpPaths( void )
//|	Date		-	02/26/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Needed a function that would dump out the paths. If you add any paths to the
//|					server please make sure that you place exports to the console here to please
//|					 so it can be looked up if needed.
//o-----------------------------------------------------------------------------------------------o
void CServerData::dumpPaths( void )
{
	Console.PrintSectionBegin();
	Console << "PathDump: \n";
	Console << "    Root        : " << Directory( CSDDP_ROOT ) << "\n";
	Console << "    Accounts    : " << Directory( CSDDP_ACCOUNTS ) << "\n";
	Console << "    Access      : " << Directory( CSDDP_ACCESS ) << "\n";
	Console << "    Mul(Data)   : " << Directory( CSDDP_DATA ) << "\n";
	Console << "    DFN(Defs)   : " << Directory( CSDDP_DEFS ) << "\n";
	Console << "    JScript     : " << Directory( CSDDP_SCRIPTS ) << "\n";
	Console << "    JScriptData : " << Directory( CSDDP_SCRIPTDATA ) << "\n";
	Console << "    HTML        : " << Directory( CSDDP_HTML ) << "\n";
	Console << "    MSGBoards   : " << Directory( CSDDP_MSGBOARD ) << "\n";
	Console << "    Books       : " << Directory( CSDDP_BOOKS ) << "\n";
	Console << "    Shared      : " << Directory( CSDDP_SHARED ) << "\n";
	Console << "    Backups     : " << Directory( CSDDP_BACKUP ) << "\n";
	Console << "    Logs        : " << Directory( CSDDP_LOGS ) << "\n";
	Console.PrintSectionBegin();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CorpseLootDecay( void ) const
//|					void CorpseLootDecay( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether loot decays along with corpses or is left on ground
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CorpseLootDecay( void ) const
{
	return boolVals.test( BIT_LOOTDECAYSONCORPSE );
}
void CServerData::CorpseLootDecay( bool newVal )
{
	boolVals.set( BIT_LOOTDECAYSONCORPSE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GuardsStatus( void ) const
//|					void GuardStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guards are enabled globally or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GuardsStatus( void ) const
{
	return boolVals.test( BIT_GUARDSENABLED );
}
void CServerData::GuardStatus( bool newVal )
{
	boolVals.set( BIT_GUARDSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DeathAnimationStatus( void ) const
//|					void DeathAnimationStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether death animation plays when a player dies
//o-----------------------------------------------------------------------------------------------o
bool CServerData::DeathAnimationStatus( void ) const
{
	return boolVals.test( BIT_PLAYDEATHANIMATION );
}
void CServerData::DeathAnimationStatus( bool newVal )
{
	boolVals.set( BIT_PLAYDEATHANIMATION, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 WorldAmbientSounds( void ) const
//|					void WorldAmbientSounds( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets intensity of ambient world sounds, bird chirps, animal sounds, etc
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::WorldAmbientSounds( void ) const
{
	return ambientsounds;
}
void CServerData::WorldAmbientSounds( SI16 value )
{
	if( value < 0 )
		ambientsounds = 0;
	else
		ambientsounds = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AmbientFootsteps( void ) const
//|					void AmbientFootsteps( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether context-specific footstep sounds are enabled or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::AmbientFootsteps( void ) const
{
	return boolVals.test( BIT_AMBIENTFOOTSTEPS );
}
void CServerData::AmbientFootsteps( bool newVal )
{
	boolVals.set( BIT_AMBIENTFOOTSTEPS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool InternalAccountStatus( void ) const
//|					void InternalAccountStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether automatic account creation is enabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::InternalAccountStatus( void ) const
{
	return boolVals.test( BIT_INTERNALACCOUNTS );
}
void CServerData::InternalAccountStatus( bool newVal )
{
	boolVals.set( BIT_INTERNALACCOUNTS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShowOfflinePCs( void ) const
//|					void ShowOfflinePCs( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether GMs can see offline players
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ShowOfflinePCs( void ) const
{
	return boolVals.test( BIT_SHOWOFFLINEPCS );
}
void CServerData::ShowOfflinePCs( bool newVal )
{
	boolVals.set( BIT_SHOWOFFLINEPCS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RogueStatus( void ) const
//|					void RogueStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether stealing skill is enabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::RogueStatus( void ) const
{
	return boolVals.test( BIT_ROGUESTATUS );
}
void CServerData::RogueStatus( bool newVal )
{
	boolVals.set( BIT_ROGUESTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SnoopIsCrime( void ) const
//|					void SnoopIsCrime( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether snooping is considered a crime
//o-----------------------------------------------------------------------------------------------o
bool CServerData::SnoopIsCrime( void ) const
{
	return boolVals.test( BIT_SNOOPISCRIME );
}
void CServerData::SnoopIsCrime( bool newVal )
{
	boolVals.set( BIT_SNOOPISCRIME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExtendedStartingStats( void ) const
//|					void ExtendedStartingStats( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting stats are enabled
//|					If enabled players start with 90 total statpoints instead of 80
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ExtendedStartingStats( void ) const
{
	return boolVals.test( BIT_EXTENDEDSTARTINGSTATS );
}
void CServerData::ExtendedStartingStats( bool newVal )
{
	boolVals.set( BIT_EXTENDEDSTARTINGSTATS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ExtendedStartingSkills( void ) const
//|					void ExtendedStartingSkills( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether extended starting skills are enabled
//|					If enabled, players start with 4 skills instead of 3, and have a total of
//|					120 skillpoints at the start instead of 100
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ExtendedStartingSkills( void ) const
{
	return boolVals.test( BIT_EXTENDEDSTARTINGSKILLS );
}
void CServerData::ExtendedStartingSkills( bool newVal )
{
	boolVals.set( BIT_EXTENDEDSTARTINGSKILLS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PlayerPersecutionStatus( void ) const
//|					void PlayerPersecutionStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player ghosts can drain mana from other players by attacking them
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PlayerPersecutionStatus( void ) const
{
	return boolVals.test( BIT_PERSECUTIONSTATUS );
}
void CServerData::PlayerPersecutionStatus( bool newVal )
{
	boolVals.set( BIT_PERSECUTIONSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 HtmlStatsStatus( void ) const
//|					void HtmlStatsStatus( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether html stats are enabled or disabled
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::HtmlStatsStatus( void ) const
{
	return htmlstatusenabled;
}
void CServerData::HtmlStatsStatus( SI16 value )
{
	htmlstatusenabled = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SellByNameStatus( void ) const
//|					void SellByNameStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items are sold by their name, not just ID/Colour
//o-----------------------------------------------------------------------------------------------o
bool CServerData::SellByNameStatus( void ) const
{
	return boolVals.test( BIT_SELLBYNAME );
}
void CServerData::SellByNameStatus( bool newVal )
{
	boolVals.set( BIT_SELLBYNAME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 SellMaxItemsStatus( void ) const
//|					void SellMaxItemsStatus( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum amount of items that can be sold in one go to a vendor
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::SellMaxItemsStatus( void ) const
{
	return sellmaxitems;
}
void CServerData::SellMaxItemsStatus( SI16 value )
{
	sellmaxitems = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TradeSystemStatus( void ) const
//|					void TradeSystemStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the advanced trade system is enabled or not.
//|					If enabled, prices of goods with NPC vendors will fluctuate with demand
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TradeSystemStatus( void ) const
{
	return boolVals.test( BIT_TRADESYSSTATUS );
}
void CServerData::TradeSystemStatus( bool newVal )
{
	boolVals.set( BIT_TRADESYSSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RankSystemStatus( void ) const
//|					void RankSystemStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether rank system is enabled or not. If enabled, it allows for
//|					variable quality of crafted items
//o-----------------------------------------------------------------------------------------------o
bool CServerData::RankSystemStatus( void ) const
{
	return boolVals.test( BIT_RANKSYSSTATUS );
}
void CServerData::RankSystemStatus( bool newVal )
{
	boolVals.set( BIT_RANKSYSSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CutScrollRequirementStatus( void ) const
//|					void CutScrollRequirementStatus( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether skill requirements are cut when casting spells from scrolls
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CutScrollRequirementStatus( void ) const
{
	return boolVals.test( BIT_CUTSCROLLREQ );
}
void CServerData::CutScrollRequirementStatus( bool newVal )
{
	boolVals.set( BIT_CUTSCROLLREQ, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckItemsSpeed( void ) const
//|					void CheckItemsSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds items are checked for decay and other things
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckItemsSpeed( void ) const
{
	return checkitems;
}
void CServerData::CheckItemsSpeed( R64 value )
{
	if( value < 0.0 )
		checkitems = 0.0;
	else
		checkitems = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckBoatSpeed( void ) const
//|					void CheckBoatSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds boats are checked for decay and other things
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckBoatSpeed( void ) const
{
	return checkboats;
}
void CServerData::CheckBoatSpeed( R64 value )
{
	if( value < 0.0 )
		checkboats = 0.0;
	else
		checkboats = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckNpcAISpeed( void ) const
//|					void CheckNpcAISpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds NPCs will execute their AI routines
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckNpcAISpeed( void ) const
{
	return checknpcai;
}
void CServerData::CheckNpcAISpeed( R64 value )
{
	if( value < 0.0 )
		checknpcai = 0.0;
	else
		checknpcai = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 CheckSpawnRegionSpeed( void ) const
//|					void CheckSpawnRegionSpeed( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in seconds) spawn regions are checked for new spawns
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::CheckSpawnRegionSpeed( void ) const
{
	return checkspawnregions;
}
void CServerData::CheckSpawnRegionSpeed( R64 value )
{
	if( value < 0.0 )
		checkspawnregions = 0.0;
	else
		checkspawnregions = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MsgBoardPostingLevel( void ) const
//|					void MsgBoardPostingLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the posting level for bulletin boards
//|					UNUSED?
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::MsgBoardPostingLevel( void ) const
{
	return msgpostinglevel;
}
void CServerData::MsgBoardPostingLevel( UI08 value )
{
	msgpostinglevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 MsgBoardPostRemovalLevel( void ) const
//|					void MsgBoardPostRemovalLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the post removal level for bulleting boards
//|					UNUSED?
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::MsgBoardPostRemovalLevel( void ) const
{
	return msgremovallevel;
}
void CServerData::MsgBoardPostRemovalLevel( UI08 value )
{
	msgremovallevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CServerData::MineCheck( void ) const
//|					void CServerData::MineCheck( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the type of check used when players attempt to use the mining skill
//|						0 = mine anywhere
//|						1 = mine mountainsides/cavefloors only
//|						2 = mine in mining regions only (not working?)
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::MineCheck( void ) const
{
	return minecheck;
}
void CServerData::MineCheck( UI08 value )
{
	minecheck = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ConnectUOServerPoll( void ) const
//|					void ConnectUOServerPoll( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether UOX3 will respond to server poll requests from ConnectUO
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ConnectUOServerPoll( void ) const
{
	return boolVals.test( BIT_CONNECTUOSERVERPOLL );
}
void CServerData::ConnectUOServerPoll( bool newVal )
{
	boolVals.set( BIT_CONNECTUOSERVERPOLL, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatDisplayHitMessage( void ) const
//|					void CombatDisplayHitMessage( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat hit messages are displayed in combat
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatDisplayHitMessage( void ) const
{
	return boolVals.test( BIT_SHOWHITMESSAGE );
}
void CServerData::CombatDisplayHitMessage( bool newVal )
{
	boolVals.set( BIT_SHOWHITMESSAGE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatDisplayDamageNumbers( void ) const
//|					void CombatDisplayDamageNumbers( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat damage numbers are displayed in combat
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatDisplayDamageNumbers( void ) const
{
	return boolVals.test( BIT_SHOWDAMAGENUMBERS );
}
void CServerData::CombatDisplayDamageNumbers( bool newVal )
{
	boolVals.set( BIT_SHOWDAMAGENUMBERS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatAttackSpeedFromStamina( void ) const
//|					void CombatAttackSpeedFromStamina( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat attack speed is derived from stamina instead of dexterity
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatAttackSpeedFromStamina( void ) const
{
	return boolVals.test( BIT_ATTSPEEDFROMSTAMINA );
}
void CServerData::CombatAttackSpeedFromStamina( bool newVal )
{
	boolVals.set( BIT_ATTSPEEDFROMSTAMINA, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 GlobalAttackSpeed( void ) const
//|					void GlobalAttackSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global attack speed in combat
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::GlobalAttackSpeed( void ) const
{
	return globalattackspeed;
}
void CServerData::GlobalAttackSpeed( R32 value )
{
	if( value < 0.0 )
		globalattackspeed = 0.0;
	else
		globalattackspeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCSpellCastSpeed( void ) const
//|					void NPCSpellCastSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global NPC spell casting speed
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCSpellCastSpeed( void ) const
{
	return npcspellcastspeed;
}
void CServerData::NPCSpellCastSpeed( R32 value )
{
	if( value < 0.0 )
		npcspellcastspeed = 0.0;
	else
		npcspellcastspeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 FishingStaminaLoss( void ) const
//|					void FishingStaminaLoss( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the stamina loss for using the fishing skill
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::FishingStaminaLoss( void ) const
{
	return fishingstaminaloss;
}
void CServerData::FishingStaminaLoss( SI16 value )
{
	fishingstaminaloss = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCDamageRate( void ) const
//|					void CombatNPCDamageRate( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the NPC damage divisor. If character is a player, damage from NPCs is
//|					divided by this value.
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatNPCDamageRate( void ) const
{
	return combatnpcdamagerate;
}
void CServerData::CombatNPCDamageRate( SI16 value )
{
	combatnpcdamagerate = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AlchemyDamageBonusEnabled( void ) const
//|					void AlchemyDamageBonusEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether Alchemy Damage Bonus Modifier is enabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::AlchemyDamageBonusEnabled( void ) const
{
	return boolVals.test( BIT_ALCHEMYDAMAGEBONUSENABLED );
}
void CServerData::AlchemyDamageBonusEnabled( bool newVal )
{
	boolVals.set( BIT_ALCHEMYDAMAGEBONUSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 AlchemyDamageBonusModifier( void ) const
//|					void AlchemyDamageBonusModifier( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Alchemy Damage Bonus Modifier, which gives bonus damage to 
//|					explosion potions based on this formula: 
//|						bonusDamage = attackerAlchemySkill / alchemyDamageBonusModifier
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::AlchemyDamageBonusModifier( void ) const
{
	return alchemyDamageBonusModifier;
}
void CServerData::AlchemyDamageBonusModifier( UI08 value )
{
	alchemyDamageBonusModifier = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatAttackStamina( void ) const
//|					void CombatAttackStamina( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of stamina lost by swinging weapon in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatAttackStamina( void ) const
{
	return combatattackstamina;
}
void CServerData::CombatAttackStamina( SI16 value )
{
	combatattackstamina = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 SkillLevel( void ) const
//|					void SkillLevel( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global difficulty level for crafting items
//|					1 = easy, 5 = default, 10 = difficult
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::SkillLevel( void ) const
{
	return skilllevel;
}
void CServerData::SkillLevel( UI08 value )
{
	skilllevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool EscortsEnabled( void ) const
//|					void EscortsEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether escorts are enabled or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::EscortsEnabled( void ) const
{
	return boolVals.test( BIT_ESCORTSTATUS );
}
void CServerData::EscortsEnabled( bool newVal )
{
	boolVals.set( BIT_ESCORTSTATUS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemsDetectSpeech( void ) const
//|					void ItemsDetectSpeech( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items can trigger onSpeech JS event
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ItemsDetectSpeech( void ) const
{
	return boolVals.test( BIT_ITEMSDETECTSPEECH );
}
void CServerData::ItemsDetectSpeech( bool newVal )
{
	boolVals.set( BIT_ITEMSDETECTSPEECH, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ForceNewAnimationPacket( void ) const
//|					void ForceNewAnimationPacket( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should force the use of new animation packet for NPCs
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ForceNewAnimationPacket( void ) const
{
	return boolVals.test( BIT_FORCENEWANIMATIONPACKET );
}
void CServerData::ForceNewAnimationPacket( bool newVal )
{
	boolVals.set( BIT_FORCENEWANIMATIONPACKET, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MapDiffsEnabled( void ) const
//|					void MapDiffsEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should load diff files for maps and statics
//|	Notes		-	Diff files are not used by client versions newer than 7.0.8.2
//o-----------------------------------------------------------------------------------------------o
bool CServerData::MapDiffsEnabled( void ) const
{
	return boolVals.test( BIT_MAPDIFFSENABLED );
}
void CServerData::MapDiffsEnabled( bool newVal )
{
	boolVals.set( BIT_MAPDIFFSENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxPlayerPackItems( void ) const
//|					void MaxPlayerPackItems( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player (and NPC) backpacks
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::MaxPlayerPackItems( void ) const
{
	return maxPlayerPackItems;
}
void CServerData::MaxPlayerPackItems( UI16 newVal )
{
	maxPlayerPackItems = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxPlayerBankItems( void ) const
//|					void MaxPlayerBankItems( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max item capacity for player bankboxes
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::MaxPlayerBankItems( void ) const
{
	return maxPlayerBankItems;
}
void CServerData::MaxPlayerBankItems( UI16 newVal )
{
	maxPlayerBankItems = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool BasicTooltipsOnly( void ) const
//|					void BasicTooltipsOnly( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether basic tooltips are enabled or not (instead of advanced tooltips)
//o-----------------------------------------------------------------------------------------------o
bool CServerData::BasicTooltipsOnly( void ) const
{
	return boolVals.test( BIT_BASICTOOLTIPSONLY );
}
void CServerData::BasicTooltipsOnly( bool newVal )
{
	boolVals.set( BIT_BASICTOOLTIPSONLY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GlobalItemDecay( void ) const
//|					void GlobalItemDecay( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether decay is enabled or disabled, on a global scale
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GlobalItemDecay( void ) const
{
	return boolVals.test( BIT_GLOBALITEMDECAY );
}
void CServerData::GlobalItemDecay( bool newVal )
{
	boolVals.set( BIT_GLOBALITEMDECAY, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ScriptItemsDecayable( void ) const
//|					void ScriptItemsDecayable( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items added through scripts decay or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ScriptItemsDecayable( void ) const
{
	return boolVals.test( BIT_SCRIPTITEMSDECAYABLE );
}
void CServerData::ScriptItemsDecayable( bool newVal )
{
	boolVals.set( BIT_SCRIPTITEMSDECAYABLE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool BaseItemsDecayable( void ) const
//|					void BaseItemsDecayable( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether base items added will decay or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::BaseItemsDecayable( void ) const
{
	return boolVals.test( BIT_BASEITEMSDECAYABLE );
}
void CServerData::BaseItemsDecayable( bool newVal )
{
	boolVals.set( BIT_BASEITEMSDECAYABLE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ItemDecayInHouses( void ) const
//|					void ItemDecayInHouses( bool newVal )
//|	Date		-	2/07/2010
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether items inside houses will decay or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ItemDecayInHouses( void ) const
{
	return boolVals.test( BIT_ITEMDECAYINHOUSES );
}
void CServerData::ItemDecayInHouses( bool newVal )
{
	boolVals.set( BIT_ITEMDECAYINHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ProtectPrivateHouses( void ) const
//|					void ProtectPrivateHouses( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players without explicit access are prevented from entering private houses
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ProtectPrivateHouses( void ) const
{
	return boolVals.test( BIT_PROTECTPRIVATEHOUSES);
}
void CServerData::ProtectPrivateHouses( bool newVal )
{
	boolVals.set( BIT_PROTECTPRIVATEHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TrackHousesPerAccount( void ) const
//|					void TrackHousesPerAccount( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether house ownership is tracked per account (true) or character (false)
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TrackHousesPerAccount( void ) const
{
	return boolVals.test( BIT_TRACKHOUSESPERACCOUNT);
}
void CServerData::TrackHousesPerAccount( bool newVal )
{
	boolVals.set( BIT_TRACKHOUSESPERACCOUNT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanOwnAndCoOwnHouses( void ) const
//|					void CanOwnAndCoOwnHouses( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can both own and co-own houses at the same time
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CanOwnAndCoOwnHouses( void ) const
{
	return boolVals.test( BIT_CANOWNANDCOOWNHOUSES);
}
void CServerData::CanOwnAndCoOwnHouses( bool newVal )
{
	boolVals.set( BIT_CANOWNANDCOOWNHOUSES, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CoOwnHousesOnSameAccount( void ) const
//|					void CoOwnHousesOnSameAccount( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether characters on same account as house owner will be treated as
//|					if they are co-owners of the house
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CoOwnHousesOnSameAccount( void ) const
{
	return boolVals.test( BIT_COOWNHOUSESONSAMEACCOUNT);
}
void CServerData::CoOwnHousesOnSameAccount( bool newVal )
{
	boolVals.set( BIT_COOWNHOUSESONSAMEACCOUNT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PaperdollGuildButton( void ) const
//|					void PaperdollGuildButton( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether guild menu can be accessed from paperdoll button or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PaperdollGuildButton( void ) const
{
	return boolVals.test( BIT_PAPERDOLLGUILDBUTTON );
}
void CServerData::PaperdollGuildButton( bool newVal )
{
	boolVals.set( BIT_PAPERDOLLGUILDBUTTON, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatMonstersVsAnimals( void ) const
//|					void CombatMonstersVsAnimals( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether monsters will attack animals or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatMonstersVsAnimals( void ) const
{
	return boolVals.test( BIT_MONSTERSVSANIMALS );
}
void CServerData::CombatMonstersVsAnimals( bool newVal )
{
	boolVals.set( BIT_MONSTERSVSANIMALS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxHousesOwnable( void ) const
//|					void MaxHousesOwnable( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can own
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::MaxHousesOwnable( void ) const
{
	return maxHousesOwnable;
}
void CServerData::MaxHousesOwnable( UI16 value )
{
	maxHousesOwnable = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 MaxHousesCoOwnable( void ) const
//|					void MaxHousesCoOwnable( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of houses that a player can co-own
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::MaxHousesCoOwnable( void ) const
{
	return maxHousesCoOwnable;
}
void CServerData::MaxHousesCoOwnable( UI16 value )
{
	maxHousesCoOwnable = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CombatAnimalsAttackChance( void ) const
//|					void CombatAnimalsAttackChance( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of monsters attacking animals
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::CombatAnimalsAttackChance( void ) const
{
	return combatanimalattackchance;
}
void CServerData::CombatAnimalsAttackChance( UI16 value )
{
	if( value > 1000 )
		value = 1000;
	combatanimalattackchance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageChance( void ) const
//|					void CombatWeaponDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether combat will damage weapons or not
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatWeaponDamageChance( void ) const
{
	return combatweapondamagechance;
}
void CServerData::CombatWeaponDamageChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatweapondamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageMin( void ) const
//|					void CombatWeaponDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min amount of damage (in hitpoints) weapons will take from combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatWeaponDamageMin( void ) const
{
	return combatweapondamagemin;
}
void CServerData::CombatWeaponDamageMin( UI08 value )
{
	combatweapondamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatWeaponDamageMax( void ) const
//|					void CombatWeaponDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of damage (in hitpoints) weapons will take from combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatWeaponDamageMax( void ) const
{
	return combatweapondamagemax;
}
void CServerData::CombatWeaponDamageMax( UI08 value )
{
	combatweapondamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageChance( void ) const
//|					void CombatArmorDamageChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to damage armor in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatArmorDamageChance( void ) const
{
	return combatarmordamagechance;
}
void CServerData::CombatArmorDamageChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatarmordamagechance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageMin( void ) const
//|					void CombatArmorDamageMin( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the min damage dealt to armor in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatArmorDamageMin( void ) const
{
	return combatarmordamagemin;
}
void CServerData::CombatArmorDamageMin( UI08 value )
{
	combatarmordamagemin = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatArmorDamageMax( void ) const
//|					void CombatArmorDamageMax( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max damage dealt to armor in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatArmorDamageMax( void ) const
{
	return combatarmordamagemax;
}
void CServerData::CombatArmorDamageMax( UI08 value )
{
	combatarmordamagemax = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 CombatBloodEffectChance( void ) const
//|					void CombatBloodEffectChance( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance to spawn blood splatter effects during combat
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::CombatBloodEffectChance( void ) const
{
	return combatbloodeffectchance;
}
void CServerData::CombatBloodEffectChance( UI08 value )
{
	if( value > 100 )
		value = 100;
	combatbloodeffectchance = value;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool HungerSystemEnabled( void ) const
//|                 void HungerSystemEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether hunger system is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::HungerSystemEnabled( void ) const
{
	return boolVals.test( BIT_HUNGERSYSTEMENABLED );
}
void CServerData::HungerSystemEnabled( bool newVal )
{
	boolVals.set( BIT_HUNGERSYSTEMENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool ThirstSystemEnabled( void ) const
//|                 void ThirstSystemEnabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether hunger system is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ThirstSystemEnabled( void ) const
{
	return boolVals.test( BIT_THIRSTSYSTEMENABLED );
}
void CServerData::ThirstSystemEnabled( bool newVal )
{
	boolVals.set( BIT_THIRSTSYSTEMENABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 HungerDamage( void ) const
//|					void HungerDamage( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the damage taken from players being hungry
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::HungerDamage( void ) const
{
	return hungerdamage;
}
void CServerData::HungerDamage( SI16 value )
{
	hungerdamage = value;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   SI16 ThirstDrain( void ) const
//|                 void ThirstDrain( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the stamina drain from players being thirsty
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ThirstDrain( void ) const
{
	return thirstdrain;
}
void CServerData::ThirstDrain( SI16 value )
{
	thirstdrain = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 PetOfflineTimeout( void ) const
//|					void PetOfflineTimeout( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the interval in seconds between checks for the player offline time
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::PetOfflineTimeout( void ) const
{
	return petOfflineTimeout;
}
void CServerData::PetOfflineTimeout( UI16 value )
{
	petOfflineTimeout = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PetHungerOffline( void ) const
//|					void PetHungerOffline( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether pets should hunger while the player (owner) is offline or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PetHungerOffline( void ) const
{
	return boolVals.test( BIT_PETHUNGEROFFLINE );
}
void CServerData::PetHungerOffline( bool newVal )
{
	boolVals.set( BIT_PETHUNGEROFFLINE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool PetThirstOffline( void ) const
//|                 void PetThirstOffline( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether pets should thirst while the player (owner) is offline or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::PetThirstOffline( void ) const
{
	return boolVals.test( BIT_PETTHIRSTOFFLINE );
}
void CServerData::PetThirstOffline( bool newVal )
{
	boolVals.set( BIT_PETTHIRSTOFFLINE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 BuyThreshold( void ) const
//|					void BuyThreshold( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold for when money is taken from a player's bank account
//|					when buying something from a vendor instead of from their backpack
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::BuyThreshold( void ) const
{
	return buyThreshold;
}
void CServerData::BuyThreshold( SI16 value )
{
	buyThreshold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CharHideWhileMounted( void ) const
//|					void CharHideWhileMounted( bool newVal )
//|	Date		-	09/22/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can hide while mounted or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CharHideWhileMounted( void ) const
{
	return boolVals.test( BIT_HIDEWHILEMOUNTED );
}
void CServerData::CharHideWhileMounted( bool newVal )
{
	boolVals.set( BIT_HIDEWHILEMOUNTED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 WeightPerStr( void ) const
//|					void WeightPerStr( R32 newVal )
//|	Date		-	3/12/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of weight one can hold per point of STR
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::WeightPerStr( void ) const
{
	return weightPerSTR;
}
void CServerData::WeightPerStr( R32 newVal )
{
	weightPerSTR = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ServerOverloadPackets( void ) const
//|					void ServerOverloadPackets( bool newVal )
//|	Date		-	11/20/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether packet handling in JS is enabled or disabled
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ServerOverloadPackets( void ) const
{
	return boolVals.test( BIT_OVERLOADPACKETS );
}
void CServerData::ServerOverloadPackets( bool newVal )
{
	boolVals.set( BIT_OVERLOADPACKETS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ArmorAffectManaRegen( void ) const
//|					void ArmorAffectManaRegen( bool newVal )
//|	Date		-	3/20/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether armor affects mana regeneration or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ArmorAffectManaRegen( void ) const
{
	return boolVals.test( BIT_ARMORAFFECTMANAREGEN );
}
void CServerData::ArmorAffectManaRegen( bool newVal )
{
	boolVals.set( BIT_ARMORAFFECTMANAREGEN, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AdvancedPathfinding( void ) const
//|					void AdvancedPathfinding( bool newVal )
//|	Date		-	7/16/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether NPCs use the A* Pathfinding routine or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::AdvancedPathfinding( void ) const
{
	return boolVals.test( BIT_ADVANCEDPATHFIND );
}
void CServerData::AdvancedPathfinding( bool newVal )
{
	boolVals.set( BIT_ADVANCEDPATHFIND, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LootingIsCrime( void ) const
//|					void LootingIsCrime( bool newVal )
//|	Date		-	4/09/2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether looting of corpses can be a crime or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::LootingIsCrime( void ) const
{
	return boolVals.test( BIT_LOOTINGISCRIME );
}
void CServerData::LootingIsCrime( bool newVal )
{
	boolVals.set( BIT_LOOTINGISCRIME, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsFromBoatKeys( void ) const
//|                 void TravelSpellsFromBoatKeys( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are usable with boat keys to teleport
//|					directly to boat's location
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TravelSpellsFromBoatKeys( void ) const
{
	return boolVals.test( BIT_TRAVELSPELLSFROMBOATKEYS );
}
void CServerData::TravelSpellsFromBoatKeys( bool newVal )
{
	boolVals.set( BIT_TRAVELSPELLSFROMBOATKEYS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsWhileOverweight( void ) const
//|                 void TravelSpellsWhileOverweight( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed while overweight
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TravelSpellsWhileOverweight( void ) const
{
	return boolVals.test( BIT_TRAVELSPELLSWHILEOVERWEIGHT );
}
void CServerData::TravelSpellsWhileOverweight( bool newVal )
{
	boolVals.set( BIT_TRAVELSPELLSWHILEOVERWEIGHT, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool MarkRunesInMultis( void ) const
//|                 void MarkRunesInMultis( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether server allows marking recall runes in multis (houses, boats)
//o-----------------------------------------------------------------------------------------------o
bool CServerData::MarkRunesInMultis( void ) const
{
	return boolVals.test( BIT_MARKRUNESINMULTIS );
}
void CServerData::MarkRunesInMultis( bool newVal )
{
	boolVals.set( BIT_MARKRUNESINMULTIS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsBetweenWorlds( void ) const
//|                 void TravelSpellsBetweenWorlds( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed between worlds
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TravelSpellsBetweenWorlds( void ) const
{
	return boolVals.test( BIT_TRAVELSPELLSBETWEENWORLDS );
}
void CServerData::TravelSpellsBetweenWorlds( bool newVal )
{
	boolVals.set( BIT_TRAVELSPELLSBETWEENWORLDS, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool TravelSpellsWhileAggressor( void ) const
//|                 void TravelSpellsWhileAggressor( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets whether travel spells (recall, gate) are allowed while marked as an aggressor
//o-----------------------------------------------------------------------------------------------o
bool CServerData::TravelSpellsWhileAggressor( void ) const
{
	return boolVals.test( BIT_TRAVELSPELLSWHILEAGGRESSOR );
}
void CServerData::TravelSpellsWhileAggressor( bool newVal )
{
	boolVals.set( BIT_TRAVELSPELLSWHILEAGGRESSOR, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 BackupRatio( void ) const
//|					void BackupRatio( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ratio of worldsaves that get backed up
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::BackupRatio( void ) const
{
	return backupRatio;
}
void CServerData::BackupRatio( SI16 value )
{
	backupRatio = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatMaxRange( void ) const
//|					void CombatMaxRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which combat can be engaged
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatMaxRange( void ) const
{
	return combatmaxrange;
}
void CServerData::CombatMaxRange( SI16 value )
{
	combatmaxrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatMaxSpellRange( void ) const
//|					void CombatMaxSpellRange( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum range at which spells can be cast in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatMaxSpellRange( void ) const
{
	return combatmaxspellrange;
}
void CServerData::CombatMaxSpellRange( SI16 value )
{
	combatmaxspellrange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CombatExplodeDelay( void ) const
//|					void CombatExplodeDelay( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the delay in seconds for the explosion spell to go into effect
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::CombatExplodeDelay( void ) const
{
	return combatExplodeDelay;
}
void CServerData::CombatExplodeDelay( UI16 value )
{
	combatExplodeDelay = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CombatAnimalsGuarded( void ) const
//|					void CombatAnimalsGuarded( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether animals are under the protection of town guards or not
//o-----------------------------------------------------------------------------------------------o
bool CServerData::CombatAnimalsGuarded( void ) const
{
	return boolVals.test( BIT_ANIMALSGUARDED );
}
void CServerData::CombatAnimalsGuarded( bool newVal )
{
	boolVals.set( BIT_ANIMALSGUARDED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCBaseFleeAt( void ) const
//|					void CombatNPCBaseFleeAt( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs start fleeing in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatNPCBaseFleeAt( void ) const
{
	return combatnpcbasefleeat;
}
void CServerData::CombatNPCBaseFleeAt( SI16 value )
{
	combatnpcbasefleeat = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 CombatNPCBaseReattackAt( void ) const
//|					void CombatNPCBaseReattackAt( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global health threshold where NPCs reattack after fleeing in combat
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::CombatNPCBaseReattackAt( void ) const
{
	return combatnpcbasereattackat;
}
void CServerData::CombatNPCBaseReattackAt( SI16 value )
{
	combatnpcbasereattackat = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCWalkingSpeed( void ) const
//|					void NPCWalkingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for NPCs
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCWalkingSpeed( void ) const
{
	return npcWalkingSpeed;
}
void CServerData::NPCWalkingSpeed( R32 value )
{
	npcWalkingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCRunningSpeed( void ) const
//|					void NPCRunningSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for NPCs
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCRunningSpeed( void ) const
{
	return npcRunningSpeed;
}
void CServerData::NPCRunningSpeed( R32 value )
{
	npcRunningSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 CServerData::NPCFleeingSpeed( void ) const
//|					void CServerData::NPCFleeingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which NPCs flee in combat
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCFleeingSpeed( void ) const
{
	return npcFleeingSpeed;
}
void CServerData::NPCFleeingSpeed( R32 value )
{
	npcFleeingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCMountedWalkingSpeed( void ) const
//|					void NPCMountedWalkingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default walking speed for mounted NPCs
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCMountedWalkingSpeed( void ) const
{
	return npcMountedWalkingSpeed;
}
void CServerData::NPCMountedWalkingSpeed( R32 value )
{
	npcMountedWalkingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCMountedRunningSpeed( void ) const
//|					void NPCMountedRunningSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default running speed for mounted NPCs
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCMountedRunningSpeed( void ) const
{
	return npcMountedRunningSpeed;
}
void CServerData::NPCMountedRunningSpeed( R32 value )
{
	npcMountedRunningSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 NPCMountedFleeingSpeed( void ) const
//|					void NPCMountedFleeingSpeed( R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the global, default speed at which mounted NPCs flee in combat
//o-----------------------------------------------------------------------------------------------o
R32 CServerData::NPCMountedFleeingSpeed( void ) const
{
	return npcMountedFleeingSpeed;
}
void CServerData::NPCMountedFleeingSpeed( R32 value )
{
	npcMountedFleeingSpeed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TitleColour( void ) const
//|					void TitleColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for titles in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TitleColour( void ) const
{
	return titleColour;
}
void CServerData::TitleColour( UI16 value )
{
	titleColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 LeftTextColour( void ) const
//|					void LeftTextColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for left text in gumps (2 column ones)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::LeftTextColour( void ) const
{
	return leftTextColour;
}
void CServerData::LeftTextColour( UI16 value )
{
	leftTextColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 RightTextColour( void ) const
//|					void RightTextColour( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default text colour for right text in gumps (2 column ones)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::RightTextColour( void ) const
{
	return rightTextColour;
}
void CServerData::RightTextColour( UI16 value )
{
	rightTextColour = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonCancel( void ) const
//|					void ButtonCancel( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for cancel button in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ButtonCancel( void ) const
{
	return buttonCancel;
}
void CServerData::ButtonCancel( UI16 value )
{
	buttonCancel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonLeft( void ) const
//|					void ButtonLeft( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for left button (navigation) in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ButtonLeft( void ) const
{
	return buttonLeft;
}
void CServerData::ButtonLeft( UI16 value )
{
	buttonLeft = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ButtonRight( void ) const
//|					void ButtonRight( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Button ID for right button (navigation) in gumps
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ButtonRight( void ) const
{
	return buttonRight;
}
void CServerData::ButtonRight( UI16 value )
{
	buttonRight = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CServerData::BackgroundPic( void ) const
//|					void CServerData::BackgroundPic( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default Gump ID for background gump
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::BackgroundPic( void ) const
{
	return backgroundPic;
}
void CServerData::BackgroundPic( UI16 value )
{
	backgroundPic = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownNumSecsPollOpen( void ) const
//|					void TownNumSecsPollOpen( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) for which a town voting poll is open
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownNumSecsPollOpen( void ) const
{
	return numSecsPollOpen;
}
void CServerData::TownNumSecsPollOpen( UI32 value )
{
	numSecsPollOpen = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownNumSecsAsMayor( void ) const
//|					void TownNumSecsAsMayor( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) that a PC would be a mayor
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownNumSecsAsMayor( void ) const
{
	return numSecsAsMayor;
}
void CServerData::TownNumSecsAsMayor( UI32 value )
{
	numSecsAsMayor = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownTaxPeriod( void ) const
//|					void TownTaxPeriod( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between periods of taxes for PCs
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownTaxPeriod( void ) const
{
	return taxPeriod;
}
void CServerData::TownTaxPeriod( UI32 value )
{
	taxPeriod = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 TownGuardPayment( void ) const
//|					void TownGuardPayment( UI32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time (in seconds) between payments for guards
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::TownGuardPayment( void ) const
{
	return guardPayment;
}
void CServerData::TownGuardPayment( UI32 value )
{
	guardPayment = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 RepMaxKills( void ) const
//|					void RepMaxKills( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the threshold in player kills before a player turns red (murderer)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::RepMaxKills( void ) const
{
	return maxmurdersallowed;
}
void CServerData::RepMaxKills( UI16 value )
{
	maxmurdersallowed = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResLogs( void ) const
//|					void ResLogs( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of logs in a given resource area
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ResLogs( void ) const
{
	return logsperarea;
}
void CServerData::ResLogs( SI16 value )
{
	logsperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResLogTime( void ) const
//|					void ResLogTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single log to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResLogTime( void ) const
{
	return logsrespawntimer;
}
void CServerData::ResLogTime( UI16 value )
{
	logsrespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CServerData::ResLogArea( void ) const
//|					void CServerData::ResLogArea( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of log-areas to split the world into (min 10)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResLogArea( void ) const
{
	return logsrespawnarea;
}
void CServerData::ResLogArea( UI16 value )
{
	if( value < 10 )
		value = 10;
	logsrespawnarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ResOre( void ) const
//|					void ResOre( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum number of ore in a given resource area
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ResOre( void ) const
{
	return oreperarea;
}
void CServerData::ResOre( SI16 value )
{
	oreperarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResOreTime( void ) const
//|					void ResOreTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the time it takes for 1 single ore to respawn in a resource area
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResOreTime( void ) const
{
	return orerespawntimer;
}
void CServerData::ResOreTime( UI16 value )
{
	orerespawntimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ResOreArea( void ) const
//|					void ResOreArea( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of ore-areas to split the world into (min 10)
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ResOreArea( void ) const
{
	return orerespawnarea;
}
void CServerData::ResOreArea( UI16 value )
{
	if( value < 10 )
		value = 10;
	orerespawnarea = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R64 AccountFlushTimer( void ) const
//|					void AccountFlushTimer( R64 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often (in mins) online accounts are checked to see if they really ARE online
//o-----------------------------------------------------------------------------------------------o
R64 CServerData::AccountFlushTimer( void ) const
{
	return flushTime;
}
void CServerData::AccountFlushTimer( R64 value )
{
	flushTime = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetClientFeature( ClientFeatures bitNum ) const
//|					void SetClientFeature( ClientFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific client-side feature
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetClientFeature( ClientFeatures bitNum ) const
{
	return clientFeatures.test( bitNum );
}
void CServerData::SetClientFeature( ClientFeatures bitNum, bool nVal )
{
	clientFeatures.set( bitNum, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetClientFeatures( void ) const
//|					void SetClientFeatures( UI32 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which client side features to enable for connecting clients
//|	Notes		-	See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
UI32 CServerData::GetClientFeatures( void ) const
{
	return static_cast<UI32>(clientFeatures.to_ulong());
}
void CServerData::SetClientFeatures( UI32 nVal )
{
	clientFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetServerFeature( ServerFeatures bitNum ) const
//|					void SetServerFeature( ServerFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets a specific server-side feature
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetServerFeature( ServerFeatures bitNum ) const
{
	return serverFeatures.test( bitNum );
}
void CServerData::SetServerFeature( ServerFeatures bitNum, bool nVal )
{
	serverFeatures.set( bitNum, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetServerFeatures( void ) const
//|					void SetServerFeatures( size_t nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which server side features to enable
//|	Notes		-	See ServerFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
size_t CServerData::GetServerFeatures( void ) const
{
	return serverFeatures.to_ulong();
}
void CServerData::SetServerFeatures( size_t nVal )
{
	serverFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetAssistantNegotiation( void ) const
//|					void SetAssistantNegotiation( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets status of feature negotiation with assist tools like Razor and AssistUO
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetAssistantNegotiation( void ) const
{
	return boolVals.test( BIT_ASSISTANTNEGOTIATION );
}
void CServerData::SetAssistantNegotiation( bool nVal )
{
	boolVals.set( BIT_ASSISTANTNEGOTIATION, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetClassicUOMapTracker( void ) const
//|					void SetClassicUOMapTracker( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should respond to ClassicUO's WorldMap Tracker packets
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetClassicUOMapTracker( void ) const
{
	return boolVals.test( BIT_CLASSICUOMAPTRACKER );
}
void CServerData::SetClassicUOMapTracker( bool nVal )
{
	boolVals.set( BIT_CLASSICUOMAPTRACKER, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetDisabledAssistantFeature( ClientFeatures bitNum ) const
//|					void SetDisabledAssistantFeature( ClientFeatures bitNum, bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
bool CServerData::GetDisabledAssistantFeature( AssistantFeatures bitNum ) const
{
	return 0 != (CServerData::DisabledAssistantFeatures & bitNum);
}
void CServerData::SetDisabledAssistantFeature( AssistantFeatures bitNum, bool nVal )
{
	if( nVal )
	{
		CServerData::DisabledAssistantFeatures |= bitNum;
	}
	else
		CServerData::DisabledAssistantFeatures &= ~bitNum;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI64 GetDisabledAssistantFeatures( void ) const
//|					void SetDisabledAssistantFeatures( UI64 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which assistant features to enable for connecting clients
//|	Notes		-	Example of assistant: Razor, AssistUO
//|					See ClientFeatures enum in cServerData.h for full list
//o-----------------------------------------------------------------------------------------------o
UI64 CServerData::GetDisabledAssistantFeatures( void ) const
{
	return CServerData::DisabledAssistantFeatures;
}
void CServerData::SetDisabledAssistantFeatures( UI64 nVal )
{
	CServerData::DisabledAssistantFeatures = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool KickOnAssistantSilence( void ) const
//|					void KickOnAssistantSilence( bool nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether player is kicked if there's no response from assistant tool
//|					to verify it complies with the allowed assistant features
//o-----------------------------------------------------------------------------------------------o
bool CServerData::KickOnAssistantSilence( void ) const
{
	return boolVals.test( BIT_KICKONASSISTANTSILENCE );
}
void CServerData::KickOnAssistantSilence( bool nVal )
{
	boolVals.set( BIT_KICKONASSISTANTSILENCE, nVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CServerData::save( void )
//|	Date		-	02/21/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save the uox.ini out. This is the default save
//o-----------------------------------------------------------------------------------------------o
//|	Returns		- [TRUE] If successfull
//o-----------------------------------------------------------------------------------------------o
bool CServerData::save( void )
{
	std::string s = Directory( CSDDP_ROOT );
	s += "uox.ini";
	return save( s );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CServerData::save( std::string filename )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This is the full uox.ini save routing.
//|
//|	Changes		-	02/21/2002	-	Fixed paths not being saved back out
//|
//|	Changes		-	02/21/2002	-	Added ini support for the isloation system
//|									At this point it just defaults to Isolation level 1.
//|
//|	Changes		-	02/27/2002	-	Made sure that ALL directory paths are
//|									written out to the ini file.
//|
//|	Changes		-	04/03/2004	-	Added new tags to the system group
//|
//|										SERVERNAME		: Name of the server.
//|										NETRCVTIMEOUT	: Timeout in seconds for recieving data
//|										NETSNDTIMEOUT	: Timeout in seconds for sending data
//|										UOGENABLED		: Does this server respond to UOGInfo Requests?
//|																		0-No 1-Yes
//|
//|	Changes		-	04/03/2004	-	Added new group [facet] with these tags
//|
//|										USEFACETSAVES	: Does the server seperate facet data?
//|																		0-No : All data will be saved into the shared folder
//|																		1-Yes: Each Facet will save its data into its own shared folder
//|										MAP0-MAP3			:	Format= MAPx=Mapname,MapAccess
//|
//|																		Where Mapname is the name of the map and
//|																		MapAccess the set of bit flags that allow/deny features and map access
//|
//o-----------------------------------------------------------------------------------------------o
bool CServerData::save( std::string filename )
{
	bool rvalue = false;
	std::ofstream ofsOutput;
	ofsOutput.open( filename.c_str(), std::ios::out );
	if( ofsOutput.is_open() )
	{

		ofsOutput << "// UOX Initialization File. V";
		ofsOutput << (static_cast<std::uint16_t>(1)<<8 | static_cast<std::uint16_t>(2)) << '\n' << "//================================" << '\n' << '\n';
		ofsOutput << "[system]" << '\n' << "{" << '\n';
		ofsOutput << "SERVERNAME=" << ServerName() << '\n';
		ofsOutput << "EXTERNALIP=" << ExternalIP() << '\n';
		ofsOutput << "PORT=" << ServerPort() << '\n';
		ofsOutput << "NETRCVTIMEOUT=" << ServerNetRcvTimeout() << '\n';
		ofsOutput << "NETSNDTIMEOUT=" << "3" << '\n';
		ofsOutput << "NETRETRYCOUNT=" << ServerNetRetryCount() << '\n';
		ofsOutput << "CONSOLELOG=" << ( ServerConsoleLog() ? 1 : 0 ) << '\n';
		ofsOutput << "NETWORKLOG=" << ( ServerNetworkLog() ? 1 : 0 ) << '\n';
		ofsOutput << "SPEECHLOG=" << ( ServerSpeechLog() ? 1 : 0 ) << '\n';
		ofsOutput << "COMMANDPREFIX=" << ServerCommandPrefix() << '\n';
		ofsOutput << "ANNOUNCEWORLDSAVES=" << (ServerAnnounceSavesStatus()?1:0) << '\n';
		ofsOutput << "JOINPARTMSGS=" << (ServerJoinPartAnnouncementsStatus()?1:0) << '\n';
		ofsOutput << "BACKUPSENABLED=" << (ServerBackupStatus()?1:0) << '\n';
		ofsOutput << "BACKUPSAVERATIO=" << BackupRatio() << '\n';
		ofsOutput << "SAVESTIMER=" << ServerSavesTimerStatus() << '\n';
		ofsOutput << "ACCOUNTISOLATION=" << "1" << '\n';
		ofsOutput << "UOGENABLED=" << (ServerUOGEnabled()?1:0) << '\n';
		ofsOutput << "CUOENABLED=" << (ConnectUOServerPoll()?1:0) << '\n';
		ofsOutput << "RANDOMSTARTINGLOCATION=" << ( ServerRandomStartingLocation() ? 1 : 0 ) << '\n';
		ofsOutput << "ASSISTANTNEGOTIATION=" << (GetAssistantNegotiation()?1:0) << '\n';
		ofsOutput << "KICKONASSISTANTSILENCE=" << (KickOnAssistantSilence()?1:0) << '\n';
		ofsOutput << "CLASSICUOMAPTRACKER=" << (GetClassicUOMapTracker()?1:0) << '\n';
		ofsOutput << "JSENGINESIZE=" << static_cast<UI16>(GetJSEngineSize()) << '\n';
		ofsOutput << "}" << '\n' << '\n';

		ofsOutput << "[clientsupport]" << '\n' << "{" << '\n';
		ofsOutput << "CLIENTSUPPORT4000=" << (ClientSupport4000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT5000=" << (ClientSupport5000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT6000=" << (ClientSupport6000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT6050=" << (ClientSupport6050()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT7000=" << (ClientSupport7000()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT7090=" << (ClientSupport7090()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70160=" << (ClientSupport70160()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70240=" << (ClientSupport70240()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70300=" << (ClientSupport70300()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70331=" << (ClientSupport70331()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT704565=" << (ClientSupport704565()?1:0) << '\n';
		ofsOutput << "CLIENTSUPPORT70610=" << (ClientSupport70610()?1:0) << '\n';
		ofsOutput << "}" << '\n';

		/*ofsOutput << '\n' << "[play server list]" << '\n' << "{" << '\n';

		std::vector< physicalServer >::iterator slIter;
		for( slIter = serverList.begin(); slIter != serverList.end(); ++slIter )
		{
			ofsOutput << "SERVERLIST=" << slIter->getName() << ",";
			if( !slIter->getDomain().empty() )
				ofsOutput << slIter->getDomain() << ",";
			else
				ofsOutput << slIter->getIP() << ",";
			ofsOutput << slIter->getPort() << '\n';
		}
		ofsOutput << "}" << '\n';*/

		ofsOutput << '\n' << "[skill & stats]" << '\n' << "{" << '\n';
		ofsOutput << "SKILLLEVEL=" << static_cast<UI16>(SkillLevel()) << '\n';
		ofsOutput << "SKILLCAP=" << ServerSkillTotalCapStatus() << '\n';
		ofsOutput << "SKILLDELAY=" << static_cast<UI16>(ServerSkillDelayStatus()) << '\n';
		ofsOutput << "STATCAP=" << ServerStatCapStatus() << '\n';
		ofsOutput << "EXTENDEDSTARTINGSTATS=" << (ExtendedStartingStats()?1:0) << '\n';
		ofsOutput << "EXTENDEDSTARTINGSKILLS=" << (ExtendedStartingSkills()?1:0) << '\n';
		ofsOutput << "MAXSTEALTHMOVEMENTS=" << MaxStealthMovement() << '\n';
		ofsOutput << "MAXSTAMINAMOVEMENTS=" << MaxStaminaMovement() << '\n';
		ofsOutput << "SNOOPISCRIME=" << (SnoopIsCrime()?1:0) << '\n';
		ofsOutput << "ARMORAFFECTMANAREGEN=" << (ArmorAffectManaRegen() ? 1 : 0) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[timers]" << '\n' << "{" << '\n';
		ofsOutput << "CORPSEDECAYTIMER=" << SystemTimer( tSERVER_CORPSEDECAY ) << '\n';
		ofsOutput << "NPCCORPSEDECAYTIMER=" << SystemTimer( tSERVER_NPCCORPSEDECAY ) << '\n';
		ofsOutput << "WEATHERTIMER=" << SystemTimer( tSERVER_WEATHER ) << '\n';
		ofsOutput << "SHOPSPAWNTIMER=" << SystemTimer( tSERVER_SHOPSPAWN ) << '\n';
		ofsOutput << "DECAYTIMER=" << SystemTimer( tSERVER_DECAY ) << '\n';
		ofsOutput << "DECAYTIMERINHOUSE=" << SystemTimer( tSERVER_DECAYINHOUSE ) << '\n';
		ofsOutput << "INVISIBILITYTIMER=" << SystemTimer( tSERVER_INVISIBILITY ) << '\n';
		ofsOutput << "OBJECTUSETIMER=" << SystemTimer( tSERVER_OBJECTUSAGE ) << '\n';
		ofsOutput << "GATETIMER=" << SystemTimer( tSERVER_GATE ) << '\n';
		ofsOutput << "POISONTIMER=" << SystemTimer( tSERVER_POISON ) << '\n';
		ofsOutput << "LOGINTIMEOUT=" << SystemTimer( tSERVER_LOGINTIMEOUT ) << '\n';
		ofsOutput << "HITPOINTREGENTIMER=" << SystemTimer( tSERVER_HITPOINTREGEN ) << '\n';
		ofsOutput << "STAMINAREGENTIMER=" << SystemTimer( tSERVER_STAMINAREGEN ) << '\n';
		ofsOutput << "MANAREGENTIMER=" << SystemTimer( tSERVER_MANAREGEN ) << '\n';
		ofsOutput << "BASEFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGBASE ) << '\n';
		ofsOutput << "RANDOMFISHINGTIMER=" << SystemTimer( tSERVER_FISHINGRANDOM ) << '\n';
		ofsOutput << "SPIRITSPEAKTIMER=" << SystemTimer( tSERVER_SPIRITSPEAK ) << '\n';
		ofsOutput << "PETOFFLINECHECKTIMER=" << SystemTimer( tSERVER_PETOFFLINECHECK ) << '\n';
		ofsOutput << "NPCFLAGUPDATETIMER=" << SystemTimer( tSERVER_NPCFLAGUPDATETIMER ) << '\n';
		ofsOutput << "BLOODDECAYTIMER=" << SystemTimer( tSERVER_BLOODDECAY ) << '\n';
		ofsOutput << "BLOODDECAYCORPSETIMER=" << SystemTimer( tSERVER_BLOODDECAYCORPSE ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[directories]" << '\n' << "{" << '\n';
		ofsOutput << "DIRECTORY=" << Directory( CSDDP_ROOT ) << '\n';
		ofsOutput << "DATADIRECTORY=" << Directory( CSDDP_DATA ) << '\n';
		ofsOutput << "DEFSDIRECTORY=" << Directory( CSDDP_DEFS ) << '\n';
		ofsOutput << "BOOKSDIRECTORY=" << Directory( CSDDP_BOOKS ) << '\n';
		ofsOutput << "ACTSDIRECTORY=" << Directory( CSDDP_ACCOUNTS ) << '\n';
		ofsOutput << "SCRIPTSDIRECTORY=" << Directory( CSDDP_SCRIPTS ) << '\n';
		ofsOutput << "SCRIPTDATADIRECTORY=" << Directory( CSDDP_SCRIPTDATA ) << '\n';
		ofsOutput << "BACKUPDIRECTORY=" << Directory( CSDDP_BACKUP ) << '\n';
		ofsOutput << "MSGBOARDDIRECTORY=" << Directory( CSDDP_MSGBOARD ) << '\n';
		ofsOutput << "SHAREDDIRECTORY=" << Directory( CSDDP_SHARED ) << '\n';
		ofsOutput << "ACCESSDIRECTORY=" << Directory( CSDDP_ACCESS ) << '\n';
		ofsOutput << "HTMLDIRECTORY=" << Directory( CSDDP_HTML ) << '\n';
		ofsOutput << "LOGSDIRECTORY=" << Directory( CSDDP_LOGS ) << '\n';
		ofsOutput << "DICTIONARYDIRECTORY=" << Directory( CSDDP_DICTIONARIES ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[settings]" << '\n' << "{" << '\n';
		ofsOutput << "LOOTDECAYSWITHCORPSE=" << (CorpseLootDecay()?1:0) << '\n';
		ofsOutput << "GUARDSACTIVE=" << (GuardsStatus()?1:0) << '\n';
		ofsOutput << "DEATHANIMATION=" << (DeathAnimationStatus()?1:0) << '\n';
		ofsOutput << "AMBIENTSOUNDS=" << WorldAmbientSounds() << '\n';
		ofsOutput << "AMBIENTFOOTSTEPS=" << (AmbientFootsteps()?1:0) << '\n';
		ofsOutput << "INTERNALACCOUNTCREATION=" << (InternalAccountStatus()?1:0) << '\n';
		ofsOutput << "SHOWOFFLINEPCS=" << (ShowOfflinePCs()?1:0) << '\n';
		ofsOutput << "ROGUESENABLED=" << (RogueStatus()?1:0) << '\n';
		ofsOutput << "PLAYERPERSECUTION=" << (PlayerPersecutionStatus()?1:0) << '\n';
		ofsOutput << "ACCOUNTFLUSH=" << AccountFlushTimer() << '\n';
		ofsOutput << "HTMLSTATUSENABLED=" << HtmlStatsStatus() << '\n';
		ofsOutput << "SELLBYNAME=" << (SellByNameStatus()?1:0) << '\n';
		ofsOutput << "SELLMAXITEMS=" << SellMaxItemsStatus() << '\n';
		ofsOutput << "BANKBUYTHRESHOLD=" << BuyThreshold() << '\n';
		ofsOutput << "TRADESYSTEM=" << (TradeSystemStatus()?1:0) << '\n';
		ofsOutput << "RANKSYSTEM=" << (RankSystemStatus()?1:0) << '\n';
		ofsOutput << "CUTSCROLLREQUIREMENTS=" << (CutScrollRequirementStatus()?1:0) << '\n';
		ofsOutput << "NPCTRAININGENABLED=" << (NPCTrainingStatus()?1:0) << '\n';
		ofsOutput << "HIDEWILEMOUNTED=" << (CharHideWhileMounted()?1:0) << '\n';
		//ofsOutput << "WEIGHTPERSTR=" << static_cast<UI16>(WeightPerStr()) << '\n';
		ofsOutput << "WEIGHTPERSTR=" << static_cast<R32>(WeightPerStr()) << '\n';
		ofsOutput << "POLYDURATION=" << SystemTimer( tSERVER_POLYMORPH ) << '\n';
		ofsOutput << "CLIENTFEATURES=" << GetClientFeatures() << '\n';
		ofsOutput << "SERVERFEATURES=" << GetServerFeatures() << '\n';
		ofsOutput << "OVERLOADPACKETS=" << (ServerOverloadPackets()?1:0) << '\n';
		ofsOutput << "ADVANCEDPATHFINDING=" << (AdvancedPathfinding()?1:0) << '\n';
		ofsOutput << "LOOTINGISCRIME=" << (LootingIsCrime()?1:0) << '\n';
		ofsOutput << "BASICTOOLTIPSONLY=" << (BasicTooltipsOnly()?1:0) << '\n';
		ofsOutput << "GLOBALITEMDECAY=" << (GlobalItemDecay()?1:0) << '\n';
		ofsOutput << "SCRIPTITEMSDECAYABLE=" << (ScriptItemsDecayable()?1:0) << '\n';
		ofsOutput << "BASEITEMSDECAYABLE=" << (BaseItemsDecayable()?1:0) << '\n';
		ofsOutput << "PAPERDOLLGUILDBUTTON=" << (PaperdollGuildButton()?1:0) << '\n';
		ofsOutput << "FISHINGSTAMINALOSS=" << FishingStaminaLoss() << '\n';
		ofsOutput << "ITEMSDETECTSPEECH=" << ItemsDetectSpeech() << '\n';
		ofsOutput << "MAXPLAYERPACKITEMS=" << MaxPlayerPackItems() << '\n';
		ofsOutput << "MAXPLAYERBANKITEMS=" << MaxPlayerBankItems() << '\n';
		ofsOutput << "FORCENEWANIMATIONPACKET=" << ForceNewAnimationPacket() << '\n';
		ofsOutput << "MAPDIFFSENABLED=" << MapDiffsEnabled() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[speedup]" << '\n' << "{" << '\n';
		ofsOutput << "CHECKITEMS=" << CheckItemsSpeed() << '\n';
		ofsOutput << "CHECKBOATS=" << CheckBoatSpeed() << '\n';
		ofsOutput << "CHECKNPCAI=" << CheckNpcAISpeed() << '\n';
		ofsOutput << "CHECKSPAWNREGIONS=" << CheckSpawnRegionSpeed() << '\n';
		ofsOutput << "NPCMOVEMENTSPEED=" << NPCWalkingSpeed() << '\n';
		ofsOutput << "NPCRUNNINGSPEED=" << NPCRunningSpeed() << '\n';
		ofsOutput << "NPCFLEEINGSPEED=" << NPCFleeingSpeed() << '\n';
		ofsOutput << "NPCMOUNTEDWALKINGSPEED=" << NPCMountedWalkingSpeed() << '\n';
		ofsOutput << "NPCMOUNTEDRUNNINGSPEED=" << NPCMountedRunningSpeed() << '\n';
		ofsOutput << "NPCMOUNTEDFLEEINGSPEED=" << NPCMountedFleeingSpeed() << '\n';
		ofsOutput << "NPCSPELLCASTSPEED=" << NPCSpellCastSpeed() << '\n';
		ofsOutput << "GLOBALATTACKSPEED=" << GlobalAttackSpeed() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[message boards]" << '\n' << "{" << '\n';
		ofsOutput << "POSTINGLEVEL=" << static_cast<UI16>(MsgBoardPostingLevel()) << '\n';
		ofsOutput << "REMOVALLEVEL=" << static_cast<UI16>(MsgBoardPostRemovalLevel()) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[escorts]" << '\n' << "{" << '\n';
		ofsOutput << "ESCORTENABLED=" << ( EscortsEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "ESCORTINITEXPIRE=" << SystemTimer( tSERVER_ESCORTWAIT ) << '\n';
		ofsOutput << "ESCORTACTIVEEXPIRE=" << SystemTimer( tSERVER_ESCORTACTIVE ) << '\n';
		ofsOutput << "ESCORTDONEEXPIRE=" << SystemTimer( tSERVER_ESCORTDONE ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[worldlight]" << '\n' << "{" << '\n';
		ofsOutput << "DUNGEONLEVEL=" << static_cast<UI16>(DungeonLightLevel()) << '\n';
		ofsOutput << "BRIGHTLEVEL=" << static_cast<UI16>(WorldLightBrightLevel()) << '\n';
		ofsOutput << "DARKLEVEL=" << static_cast<UI16>(WorldLightDarkLevel()) << '\n';
		ofsOutput << "SECONDSPERUOMINUTE=" << ServerSecondsPerUOMinute() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[tracking]" << '\n' << "{" << '\n';
		ofsOutput << "BASERANGE=" << TrackingBaseRange() << '\n';
		ofsOutput << "BASETIMER=" << TrackingBaseTimer() << '\n';
		ofsOutput << "MAXTARGETS=" << static_cast<UI16>(TrackingMaxTargets()) << '\n';
		ofsOutput << "MSGREDISPLAYTIME=" << TrackingRedisplayTime() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[reputation]" << '\n' << "{" << '\n';
		ofsOutput << "MURDERDECAYTIMER=" << SystemTimer( tSERVER_MURDERDECAY ) << '\n';
		ofsOutput << "MAXKILLS=" << RepMaxKills() << '\n';
		ofsOutput << "CRIMINALTIMER=" << SystemTimer( tSERVER_CRIMINAL ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[resources]" << '\n' << "{" << '\n';
		ofsOutput << "MINECHECK=" << static_cast<UI16>(MineCheck()) << '\n';
		ofsOutput << "OREPERAREA=" << ResOre() << '\n';
		ofsOutput << "ORERESPAWNTIMER=" << ResOreTime() << '\n';
		ofsOutput << "ORERESPAWNAREA=" << ResOreArea() << '\n';
		ofsOutput << "LOGSPERAREA=" << ResLogs() << '\n';
		ofsOutput << "LOGSRESPAWNTIMER=" << ResLogTime() << '\n';
		ofsOutput << "LOGSRESPAWNAREA=" << ResLogArea() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[hunger]" << '\n' << "{" << '\n';
		ofsOutput << "HUNGERENABLED=" << (HungerSystemEnabled()?1:0) << '\n';
		ofsOutput << "HUNGERRATE=" << SystemTimer( tSERVER_HUNGERRATE ) << '\n';
		ofsOutput << "HUNGERDMGVAL=" << HungerDamage() << '\n';
		ofsOutput << "PETHUNGEROFFLINE=" << (PetHungerOffline()?1:0) << '\n';
		ofsOutput << "PETOFFLINETIMEOUT=" << PetOfflineTimeout() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[thirst]" << '\n' << "{" << '\n';
		ofsOutput << "THIRSTENABLED=" << ( ThirstSystemEnabled() ? 1 : 0 ) << '\n';
		ofsOutput << "THIRSTRATE=" << SystemTimer( tSERVER_THIRSTRATE ) << '\n';
		ofsOutput << "THIRSTDRAINVAL=" << ThirstDrain() << '\n';
		ofsOutput << "PETTHIRSTOFFLINE=" << (PetThirstOffline()?1:0) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[combat]" << '\n' << "{" << '\n';
		ofsOutput << "MAXRANGE=" << CombatMaxRange() << '\n';
		ofsOutput << "SPELLMAXRANGE=" << CombatMaxSpellRange() << '\n';
		ofsOutput << "DISPLAYHITMSG=" << (CombatDisplayHitMessage()?1:0) << '\n';
		ofsOutput << "DISPLAYDAMAGENUMBERS=" << (CombatDisplayDamageNumbers()?1:0) << '\n';
		ofsOutput << "MONSTERSVSANIMALS=" << (CombatMonstersVsAnimals()?1:0) << '\n';
		ofsOutput << "ANIMALATTACKCHANCE=" << static_cast<UI16>(CombatAnimalsAttackChance()) << '\n';
		ofsOutput << "ANIMALSGUARDED=" << (CombatAnimalsGuarded()?1:0) << '\n';
		ofsOutput << "NPCDAMAGERATE=" << CombatNPCDamageRate() << '\n';
		ofsOutput << "NPCBASEFLEEAT=" << CombatNPCBaseFleeAt() << '\n';
		ofsOutput << "NPCBASEREATTACKAT=" << CombatNPCBaseReattackAt() << '\n';
		ofsOutput << "ATTACKSTAMINA=" << CombatAttackStamina() << '\n';
		ofsOutput << "ATTACKSPEEDFROMSTAMINA=" << (CombatAttackSpeedFromStamina()?1:0) << '\n';
		ofsOutput << "SHOOTONANIMALBACK=" << (ShootOnAnimalBack()?1:0) << '\n';
		ofsOutput << "COMBATEXPLODEDELAY=" << CombatExplodeDelay() << '\n';
		ofsOutput << "WEAPONDAMAGECHANCE=" << static_cast<UI16>(CombatWeaponDamageChance()) << '\n';
		ofsOutput << "WEAPONDAMAGEMIN=" << static_cast<UI16>(CombatWeaponDamageMin()) << '\n';
		ofsOutput << "WEAPONDAMAGEMAX=" << static_cast<UI16>(CombatWeaponDamageMax()) << '\n';
		ofsOutput << "ARMORDAMAGECHANCE=" << static_cast<UI16>(CombatArmorDamageChance()) << '\n';
		ofsOutput << "ARMORDAMAGEMIN=" << static_cast<UI16>(CombatArmorDamageMin()) << '\n';
		ofsOutput << "ARMORDAMAGEMAX=" << static_cast<UI16>(CombatArmorDamageMax()) << '\n';
		ofsOutput << "ALCHEMYBONUSENABLED=" << (AlchemyDamageBonusEnabled()?1:0) << '\n';
		ofsOutput << "ALCHEMYBONUSMODIFIER=" << static_cast<UI16>(AlchemyDamageBonusModifier()) << '\n';
		ofsOutput << "BLOODEFFECTCHANCE=" << static_cast<UI16>( CombatBloodEffectChance() ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[magic]" << '\n' << "{" << '\n';
		ofsOutput << "TRAVELSPELLSFROMBOATKEYS=" << ( TravelSpellsFromBoatKeys() ? 1 : 0 ) << '\n';
		ofsOutput << "TRAVELSPELLSWHILEOVERWEIGHT=" << ( TravelSpellsWhileOverweight() ? 1 : 0 ) << '\n';
		ofsOutput << "MARKRUNESINMULTIS=" << ( MarkRunesInMultis() ? 1 : 0 ) << '\n';
		ofsOutput << "TRAVELSPELLSBETWEENWORLDS=" << ( TravelSpellsBetweenWorlds() ? 1 : 0 ) << '\n';
		ofsOutput << "TRAVELSPELLSWHILEAGGRESSOR=" << ( TravelSpellsWhileAggressor() ? 1 : 0 ) << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[start locations]" << '\n' << "{" << '\n';
		for( size_t lCtr = 0; lCtr < startlocations.size(); ++lCtr )
			ofsOutput << "LOCATION=" << startlocations[lCtr].newTown << "," << startlocations[lCtr].newDescription << "," << startlocations[lCtr].x << "," << startlocations[lCtr].y << "," << startlocations[lCtr].z << "," << startlocations[lCtr].worldNum << "," << startlocations[lCtr].instanceID << "," << startlocations[lCtr].clilocDesc << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[startup]" << '\n' << "{" << '\n';
		ofsOutput << "STARTGOLD=" << ServerStartGold() << '\n';
		ofsOutput << "STARTPRIVS=" << ServerStartPrivs() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[gumps]" << '\n' << "{" << '\n';
		ofsOutput << "TITLECOLOUR=" << TitleColour() << '\n';
		ofsOutput << "LEFTTEXTCOLOUR=" << LeftTextColour() << '\n';
		ofsOutput << "RIGHTTEXTCOLOUR=" << RightTextColour() << '\n';
		ofsOutput << "BUTTONCANCEL=" << ButtonCancel() << '\n';
		ofsOutput << "BUTTONLEFT=" << ButtonLeft() << '\n';
		ofsOutput << "BUTTONRIGHT=" << ButtonRight() << '\n';
		ofsOutput << "BACKGROUNDPIC=" << BackgroundPic() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[houses]" << '\n' << "{" << '\n';
		ofsOutput << "TRACKHOUSESPERACCOUNT=" << (TrackHousesPerAccount()?1:0) << '\n';
		ofsOutput << "CANOWNANDCOOWNHOUSES=" << (CanOwnAndCoOwnHouses()?1:0) << '\n';
		ofsOutput << "COOWNHOUSESONSAMEACCOUNT=" << (CoOwnHousesOnSameAccount()?1:0) << '\n';
		ofsOutput << "ITEMDECAYINHOUSES=" << (ItemDecayInHouses()?1:0) << '\n';
		ofsOutput << "PROTECTPRIVATEHOUSES=" << (ProtectPrivateHouses()?1:0) << '\n';
		ofsOutput << "MAXHOUSESOWNABLE=" << MaxHousesOwnable() << '\n';
		ofsOutput << "MAXHOUSESCOOWNABLE=" << MaxHousesCoOwnable() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[towns]" << '\n' << "{" << '\n';
		ofsOutput << "POLLTIME=" << TownNumSecsPollOpen() << '\n';
		ofsOutput << "MAYORTIME=" << TownNumSecsAsMayor() << '\n';
		ofsOutput << "TAXPERIOD=" << TownTaxPeriod() << '\n';
		ofsOutput << "GUARDSPAID=" << TownGuardPayment() << '\n';
		ofsOutput << "}" << '\n';

		ofsOutput << '\n' << "[disabled assistant features]" << '\n' << "{" << '\n';
		ofsOutput << "AF_FILTERWEATHER=" << GetDisabledAssistantFeature( AF_FILTERWEATHER ) << '\n';
		ofsOutput << "AF_FILTERLIGHT=" << GetDisabledAssistantFeature( AF_FILTERLIGHT ) << '\n';
		ofsOutput << "AF_SMARTTARGET=" << GetDisabledAssistantFeature( AF_SMARTTARGET ) << '\n';
		ofsOutput << "AF_RANGEDTARGET=" << GetDisabledAssistantFeature( AF_RANGEDTARGET ) << '\n';
		ofsOutput << "AF_AUTOOPENDOORS=" << GetDisabledAssistantFeature( AF_AUTOOPENDOORS ) << '\n';
		ofsOutput << "AF_DEQUIPONCAST=" << GetDisabledAssistantFeature( AF_DEQUIPONCAST ) << '\n';
		ofsOutput << "AF_AUTOPOTIONEQUIP=" << GetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP ) << '\n';
		ofsOutput << "AF_POISONEDCHECKS=" << GetDisabledAssistantFeature( AF_POISONEDCHECKS ) << '\n';
		ofsOutput << "AF_LOOPEDMACROS=" << GetDisabledAssistantFeature( AF_LOOPEDMACROS ) << '\n';
		ofsOutput << "AF_USEONCEAGENT=" << GetDisabledAssistantFeature( AF_USEONCEAGENT ) << '\n';
		ofsOutput << "AF_RESTOCKAGENT=" << GetDisabledAssistantFeature( AF_RESTOCKAGENT ) << '\n';
		ofsOutput << "AF_SELLAGENT=" << GetDisabledAssistantFeature( AF_SELLAGENT ) << '\n';
		ofsOutput << "AF_BUYAGENT=" << GetDisabledAssistantFeature( AF_BUYAGENT ) << '\n';
		ofsOutput << "AF_POTIONHOTKEYS=" << GetDisabledAssistantFeature( AF_POTIONHOTKEYS ) << '\n';
		ofsOutput << "AF_RANDOMTARGETS=" << GetDisabledAssistantFeature( AF_RANDOMTARGETS ) << '\n';
		ofsOutput << "AF_CLOSESTTARGETS=" << GetDisabledAssistantFeature( AF_CLOSESTTARGETS ) << '\n';
		ofsOutput << "AF_OVERHEADHEALTH=" << GetDisabledAssistantFeature( AF_OVERHEADHEALTH ) << '\n';
		ofsOutput << "AF_AUTOLOOTAGENT=" << GetDisabledAssistantFeature( AF_AUTOLOOTAGENT ) << '\n';
		ofsOutput << "AF_BONECUTTERAGENT=" << GetDisabledAssistantFeature( AF_BONECUTTERAGENT ) << '\n';
		ofsOutput << "AF_JSCRIPTMACROS=" << GetDisabledAssistantFeature( AF_JSCRIPTMACROS ) << '\n';
		ofsOutput << "AF_AUTOREMOUNT=" << GetDisabledAssistantFeature( AF_AUTOREMOUNT ) << '\n';
		ofsOutput << "AF_ALL=" << GetDisabledAssistantFeature( AF_ALL ) << '\n';
		ofsOutput << "}" << '\n';


		ofsOutput.close();
		rvalue = true;
	}
	else
		Console.error( strutil::format("Unable to open file %s for writing", filename.c_str()) );
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	Load()
//|	Date		-	January 13, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load up the uox.ini file and parse it into the internals
//|	Returns		-	pointer to the valid inmemory serverdata storage(this)
//|						nullptr is there is an error, or invalid file type
//o-----------------------------------------------------------------------------------------------o
void CServerData::Load( void )
{
	const std::string fileName = Directory( CSDDP_ROOT ) + "uox.ini";
	ParseINI( fileName );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingBaseRange( void ) const
//|					void TrackingBaseRange( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the base (minimum) range even beginner trackers can track at
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TrackingBaseRange( void ) const
{
	return trackingbaserange;
}
void CServerData::TrackingBaseRange( UI16 value )
{
	trackingbaserange = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 TrackingMaxTargets( void ) const
//|					void TrackingMaxTargets( UI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of targets someone can see in the tracking window
//o-----------------------------------------------------------------------------------------------o
UI08 CServerData::TrackingMaxTargets( void ) const
{
	return trackingmaxtargets;
}
void CServerData::TrackingMaxTargets( UI08 value )
{
	if( value >= MAX_TRACKINGTARGETS )
		trackingmaxtargets = MAX_TRACKINGTARGETS;
	else
		trackingmaxtargets = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingBaseTimer( void ) const
//|					void TrackingBaseTimer( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the max amount of time a grandmaster tracker can track someone
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TrackingBaseTimer( void ) const
{
	return trackingbasetimer;
}
void CServerData::TrackingBaseTimer( UI16 value )
{
	trackingbasetimer = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 TrackingRedisplayTime( void ) const
//|					void TrackingRedisplayTime( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how often in seconds the tracking message is updated and displayed
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::TrackingRedisplayTime( void ) const
{
	return trackingmsgredisplaytimer;
}
void CServerData::TrackingRedisplayTime( UI16 value )
{
	trackingmsgredisplaytimer = value;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ParseINI( const std::string filename )
//|	Date		-	02/26/2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Parse the uox.ini file into its required information.
//|
//|	Changes		-	02/26/2002	-	Make sure that we parse out the logs, access
//|									and other directories that we were not parsing/
//o-----------------------------------------------------------------------------------------------o
bool CServerData::ParseINI( const std::string& filename )
{
	bool rvalue = false;
	if( !filename.empty() )
	{

		Console << "Processing INI Settings  ";

		Script toParse( filename, NUM_DEFS, false );
		// Lock this file tight, No access at anytime when open(should only be open and closed anyhow. For Thread blocking)
		if( !toParse.IsErrored() )
		{
			//serverList.clear();
			startlocations.clear();
			for( ScriptSection *sect = toParse.FirstEntry(); sect != nullptr; sect = toParse.NextEntry() )
			{
				if( sect == nullptr )
					continue;
				std::string tag, data;
				for( tag = sect->First(); !sect->AtEnd(); tag = sect->Next() )
				{
					data = strutil::simplify( sect->GrabData() );
					if( !HandleLine( tag, data ) )
					{
						Console.warning( strutil::format("Unhandled tag '%s'", tag.c_str()) );
					}
				}
			}
			Console.PrintDone();
			rvalue = true;
		}
		else
		{
			Console.warning( strutil::format("%s File not found, Using default settings.", filename.c_str() ));
			cwmWorldState->ServerData()->save();
		}
	}
	return rvalue;
}

bool CServerData::HandleLine( const std::string& tag, const std::string& value )
{
	bool rvalue = true;
	auto titer = uox3inicasevalue.find(tag) ;
	if (titer == uox3inicasevalue.end()){
		return false ;
	}

	switch( titer->second )
	{
		case 1:	 // SERVERNAME[0002]
			ServerName( value );
			break;
		case 2:	 // CONSOLELOG[0003]
			ServerConsoleLog( ( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ) );
			break;
		case 3:	 // COMMANDPREFIX[0005]
			ServerCommandPrefix( (value.data()[0]) );	// return the first character of the return string only
			break;
		case 4:	 // ANNOUNCEWORLDSAVES[0006]
			ServerAnnounceSaves( (static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 ? true : false) );
			break;
		case 26:	 // JOINPARTMSGS[0007]
			ServerJoinPartAnnouncements( (static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 ? true : false) );
			break;
		case 5:	 // BACKUPSENABLED[0009]
			ServerBackups( (static_cast<UI16>(std::stoul(value, nullptr, 0)) > 0 ? true : false) );
			break;
		case 6:	 // SAVESTIMER[0010]
			ServerSavesTimer( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 7:	 // SKILLCAP[0011]
			ServerSkillTotalCap( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 8:	 // SKILLDELAY[0012]
			ServerSkillDelay( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 9:	 // STATCAP[0013]
			ServerStatCap( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 10:	 // MAXSTEALTHMOVEMENTS[0014]
			MaxStealthMovement( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 11:	 // MAXSTAMINAMOVEMENTS[0015]
			MaxStaminaMovement( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 12:	 // ARMORAFFECTMANAREGEN[0016]
			ArmorAffectManaRegen( (static_cast<UI08>(std::stoul(value, nullptr, 0)) > 0 ? true : false) );
			break;
		case 13:	 // CORPSEDECAYTIMER[0017]
			SystemTimer( tSERVER_CORPSEDECAY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 14:	 // WEATHERTIMER[0018]
			SystemTimer( tSERVER_WEATHER, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 15:	 // SHOPSPAWNTIMER[0019]
			SystemTimer( tSERVER_SHOPSPAWN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 16:	 // DECAYTIMER[0020]
			SystemTimer( tSERVER_DECAY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 17:	 // INVISIBILITYTIMER[0021]
			SystemTimer( tSERVER_INVISIBILITY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 18:	 // OBJECTUSETIMER[0022]
			SystemTimer( tSERVER_OBJECTUSAGE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 19:	 // GATETIMER[0023]
			SystemTimer( tSERVER_GATE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 20:	 // POISONTIMER[0024]
			SystemTimer( tSERVER_POISON, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 21:	 // LOGINTIMEOUT[0025]
			SystemTimer( tSERVER_LOGINTIMEOUT, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 22:	 // HITPOINTREGENTIMER[0026]
			SystemTimer( tSERVER_HITPOINTREGEN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 23:	 // STAMINAREGENTIMER[0027]
			SystemTimer( tSERVER_STAMINAREGEN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 37:	 // MANAREGENTIMER[0028]
			SystemTimer( tSERVER_MANAREGEN, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 24:	 // BASEFISHINGTIMER[0029]
			SystemTimer( tSERVER_FISHINGBASE,static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 38:	 // RANDOMFISHINGTIMER[0030]
			SystemTimer( tSERVER_FISHINGRANDOM, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 39:	 // SPIRITSPEAKTIMER[0031]
			SystemTimer( tSERVER_SPIRITSPEAK, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 40:	 // DIRECTORY[0032]
		{
			Directory( CSDDP_ROOT, value );
			break;
		}
		case 41:	 // DATADIRECTORY[0033]
		{
			Directory( CSDDP_DATA, value );
			break;
		}
		case 42:	 // DEFSDIRECTORY[0034]
		{
			Directory( CSDDP_DEFS, value );
			break;
		}
		case 43:	 // ACTSDIRECTORY[0035]
		{
			Directory( CSDDP_ACCOUNTS, value );
			break;
		}
		case 25:	 // SCRIPTSDIRECTORY[0036]
		{
			Directory( CSDDP_SCRIPTS, value );
			break;
		}
		case 44:	 // BACKUPDIRECTORY[0037]
		{
			Directory( CSDDP_BACKUP, value );
			break;
		}
		case 45:	 // MSGBOARDDIRECTORY[0038]
		{
			Directory( CSDDP_MSGBOARD, value );
			break;
		}
		case 46:	 // SHAREDDIRECTORY[0039]
		{
			Directory( CSDDP_SHARED, value );
			break;
		}
		case 47:	 // LOOTDECAYSWITHCORPSE[0040]
			CorpseLootDecay( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 49:	 // GUARDSACTIVE[0041]
			GuardStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 27:	 // DEATHANIMATION[0042]
			DeathAnimationStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 50:	 // AMBIENTSOUNDS[0043]
			WorldAmbientSounds( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 51:	 // AMBIENTFOOTSTEPS[0044]
			AmbientFootsteps( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 52:	 // INTERNALACCOUNTCREATION[0045]
			InternalAccountStatus( static_cast<UI16>(std::stoul(value, nullptr, 0))!= 0 );
			break;
		case 53:	 // SHOWOFFLINEPCS[0046]
			ShowOfflinePCs( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 54:	 // ROGUESENABLED[0047]
			RogueStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 55:	 // PLAYERPERSECUTION[0048]
			PlayerPersecutionStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 56:	 // ACCOUNTFLUSH[0049]
			AccountFlushTimer( std::stod(value) );
			break;
		case 57:	 // HTMLSTATUSENABLED[0050]
			HtmlStatsStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 58:	 // SELLBYNAME[0051]
			SellByNameStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 59:	 // SELLMAXITEMS[0052]
			SellMaxItemsStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 60:	 // TRADESYSTEM[0053]
			TradeSystemStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 61:	 // RANKSYSTEM[0054]
			RankSystemStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 62:	 // CUTSCROLLREQUIREMENTS[0055]
			CutScrollRequirementStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0) ;
			break;
		case 63:	 // CHECKITEMS[0056]
			CheckItemsSpeed( std::stod(value) );
			break;
		case 64:	 // CHECKBOATS[0057]
			CheckBoatSpeed( std::stod(value) );
			break;
		case 65:	 // CHECKNPCAI[0058]
			CheckNpcAISpeed( std::stod(value) );
			break;
		case 66:	 // CHECKSPAWNREGIONS[0059]
			CheckSpawnRegionSpeed( std::stod(value) );
			break;
		case 67:	 // POSTINGLEVEL[0060]
			MsgBoardPostingLevel( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 68:	 // REMOVALLEVEL[0061]
			MsgBoardPostRemovalLevel( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 69:	 // ESCORTENABLED[0062]
			EscortsEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 70:	 // ESCORTINITEXPIRE[0063]
			SystemTimer( tSERVER_ESCORTWAIT, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 71:	 // ESCORTACTIVEEXPIRE[0064]
			SystemTimer( tSERVER_ESCORTACTIVE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 72:	 // MOON1[0065]
			ServerMoon( 0, static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 73:	 // MOON2[0066]
			ServerMoon( 1, static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 74:	 // DUNGEONLEVEL[0067]
			DungeonLightLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 75:	 // CURRENTLEVEL[0068]
			WorldLightCurrentLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 76:	 // BRIGHTLEVEL[0069]
			WorldLightBrightLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 77:	 // BASERANGE[0070]
			TrackingBaseRange( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 78:	 // BASETIMER[0071]
			TrackingBaseTimer( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 79:	 // MAXTARGETS[0072]
			TrackingMaxTargets( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 80:	 // MSGREDISPLAYTIME[0073]
			TrackingRedisplayTime( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 81:	 // MURDERDECAYTIMER[0074]
			SystemTimer( tSERVER_MURDERDECAY, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 82:	 // MAXKILLS[0075]
			RepMaxKills( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 83:	 // CRIMINALTIMER[0076]
			SystemTimer( tSERVER_CRIMINAL, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 84:	 // MINECHECK[0077]
			MineCheck( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 85:	 // OREPERAREA[0078]
			ResOre(static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 86:	 // ORERESPAWNTIMER[0079]
			ResOreTime(static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 87:	 // ORERESPAWNAREA[0080]
			ResOreArea( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 88:	 // LOGSPERAREA[0081]
			ResLogs( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 89:	 // LOGSRESPAWNTIMER[0082]
			ResLogTime( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 90:	 // LOGSRESPAWNAREA[0083]
			ResLogArea(static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 91:	 // HUNGERRATE[0084]
			SystemTimer( tSERVER_HUNGERRATE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 92:	 // HUNGERDMGVAL[0085]
			HungerDamage( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 93:	 // MAXRANGE[0086]
			CombatMaxRange( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 94:	 // SPELLMAXRANGE[0087]
			CombatMaxSpellRange(static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 95:	 // DISPLAYHITMSG[0088]
			CombatDisplayHitMessage( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 96:	 // MONSTERSVSANIMALS[0089]
			CombatMonstersVsAnimals( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 97:	 // ANIMALATTACKCHANCE[0090]
			CombatAnimalsAttackChance( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 98:	 // ANIMALSGUARDED[0091]
			CombatAnimalsGuarded( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 99:	 // NPCDAMAGERATE[0092]
			CombatNPCDamageRate( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 100:	 // NPCBASEFLEEAT[0093]
			CombatNPCBaseFleeAt( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 101:	 // NPCBASEREATTACKAT[0094]
			CombatNPCBaseReattackAt( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 102:	 // ATTACKSTAMINA[0095]
			CombatAttackStamina( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 103:	 // LOCATION[0096]
			ServerLocation( value );
			break;
		case 104:	 // STARTGOLD[0097]
			ServerStartGold( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 105:	 // STARTPRIVS[0098]
			ServerStartPrivs( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 106:	 // ESCORTDONEEXPIRE[0099]
			SystemTimer( tSERVER_ESCORTDONE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 107:	 // DARKLEVEL[0100]
			WorldLightDarkLevel( static_cast<LIGHTLEVEL>(std::stoul(value, nullptr, 0)) );
			break;
		case 108:	 // TITLECOLOUR[0101]
			TitleColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 109:	 // LEFTTEXTCOLOUR[0102]
			LeftTextColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 110:	 // RIGHTTEXTCOLOUR[0103]
			RightTextColour( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 111:	 // BUTTONCANCEL[0104]
			ButtonCancel( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 112:	 // BUTTONLEFT[0105]
			ButtonLeft( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 113:	 // BUTTONRIGHT[0106]
			ButtonRight( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 114:	 // BACKGROUNDPIC[0107]
			BackgroundPic( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 115:	 // POLLTIME[0108]
			TownNumSecsPollOpen( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 116:	 // MAYORTIME[0109]
			TownNumSecsAsMayor( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 117:	 // TAXPERIOD[0110]
			TownTaxPeriod( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 118:	 // GUARDSPAID[0111]
			TownGuardPayment( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 119:	 // DAY[0112]
			ServerTimeDay( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 120:	 // HOURS[0113]
			ServerTimeHours( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 121:	 // MINUTES[0114]
			ServerTimeMinutes( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 122:	 // SECONDS[0115]
			ServerTimeSeconds( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 123:	 // AMPM[0116]
			ServerTimeAMPM( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 124:	 // SKILLLEVEL[0117]
			SkillLevel( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 125:	 // SNOOPISCRIME[0118]
			SnoopIsCrime( static_cast<UI16>(std::stoul(value, nullptr, 0)) != 0 );
			break;
		case 126:	 // BOOKSDIRECTORY[0119]
			Directory( CSDDP_BOOKS, value );
			break;
		case 127:	 // SERVERLIST[0120]
		{
			/*std::string sname, sip, sport;
			physicalServer toAdd;
			auto csecs = strutil::sections( value, "," );
			if( csecs.size() == 3 )
			{
				struct hostent *lpHostEntry = nullptr;
				sname	= strutil::stripTrim( csecs[0] );
				sip		= strutil::stripTrim( csecs[1] );
				sport	= strutil::stripTrim( csecs[2] );

				toAdd.setName( sname );
				// Ok look up the data here see if its a number
				bool bDomain = true;
				if( ( lpHostEntry = gethostbyname( sip.c_str() ) ) == nullptr )
				{
					// this was not a domain name so check for IP address
					if( ( lpHostEntry = gethostbyaddr( sip.c_str(), static_cast<UI32>(sip.size()), AF_INET ) ) == nullptr )
					{
						// We get here it wasn't a valid IP either.
						Console.warning( strutil::format("Failed to translate %s", sip.c_str() ));
						Console.warning( "This shard will not show up on the shard listing" );
						break;
					}
					bDomain = false;
				}
				// Going to store a copy of the domain name as well to save to the ini if there is a domain name insteead of an ip.
				if( bDomain ) // Store the domain name for later then seeing as its a valid one
				{
					toAdd.setDomain( sip );
				}
				else // this was a valid ip address so we will use an ip instead so clear the domain string.
				{
					toAdd.setDomain( "" );
				}

				// Ok now the server itself uses the ip so we need to store that :) Means we only need to look thisip once
				struct in_addr *pinaddr;
				pinaddr = ((struct in_addr*)lpHostEntry->h_addr);
				toAdd.setIP( inet_ntoa(*pinaddr) );
				toAdd.setPort( static_cast<UI16>(std::stoul(sport, nullptr, 0)));
				serverList.push_back( toAdd );
			}
			else
			{
				Console.warning(strutil::format("Malformend Serverlist entry: %s", value.c_str() ));
				Console.warning( "This shard will not show up on the shard listing" );
			}*/
			break;
		}
		case 128:	 // PORT[0121]
			ServerPort( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 129:	 // ACCESSDIRECTORY[0122]
			Directory( CSDDP_ACCESS, value );
			break;
		case 130:	 // LOGSDIRECTORY[0123]
			Directory( CSDDP_LOGS, value );
			break;
		case 131:	 // ACCOUNTISOLATION[0124]
			break;
		case 132:	 // HTMLDIRECTORY[0125]
			Directory( CSDDP_HTML, value );
			break;
		case 133:	 // SHOOTONANIMALBACK[0126]
			ShootOnAnimalBack( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 134:	 // NPCTRAININGENABLED[0127]
			NPCTrainingStatus( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 135:	 // DICTIONARYDIRECTORY[0128]
			Directory( CSDDP_DICTIONARIES, value );
			break;
		case 136:	 // BACKUPSAVERATIO[0129]
			BackupRatio( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 137:	 // HIDEWILEMOUNTED[0130]
			CharHideWhileMounted( static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 138:	 // SECONDSPERUOMINUTE[0131]
			ServerSecondsPerUOMinute( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 139:	 // WEIGHTPERSTR[0132]
			//WeightPerStr( value.toUByte() );
			WeightPerStr( std::stof(value) );
			break;
		case 140:	 // POLYDURATION[0133]
			SystemTimer( tSERVER_POLYMORPH, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 141:	 // UOGENABLED[0134]
			ServerUOGEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0))==1 );
			break;
		case 142:	 // NETRCVTIMEOUT[0135]
			ServerNetRcvTimeout( static_cast<UI32>(std::stoul(value, nullptr, 0)));
			break;
		case 143:	 // NETSNDTIMEOUT[0136]
			ServerNetSndTimeout( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 144:	 // NETRETRYCOUNT[0137]
			ServerNetRetryCount( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 145:	 // CLIENTFEATURES[0138]
			SetClientFeatures( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 146:	 // PACKETOVERLOADS[0139]
			ServerOverloadPackets( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 147:	 // NPCMOVEMENTSPEED[0140]
			NPCWalkingSpeed( std::stof(value) );
			break;
		case 148:	 // PETHUNGEROFFLINE[0141]
			PetHungerOffline( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 149:	 // PETOFFLINETIMEOUT[0142]
			PetOfflineTimeout( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 150:	 // PETOFFLINECHECKTIMER[0143]
			SystemTimer( tSERVER_PETOFFLINECHECK, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 152:	 // ADVANCEDPATHFINDING[0145]
			AdvancedPathfinding( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 153:	 // SERVERFEATURES[0146]
			SetServerFeatures( static_cast<UI32>(std::stoul(value, nullptr, 0)) );
			break;
		case 154:	 // LOOTINGISCRIME[0147]
			LootingIsCrime( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 155:	 // NPCRUNNINGSPEED[0148]
			NPCRunningSpeed( std::stof(value) );
			break;
		case 156:	 // NPCFLEEINGSPEED[0149]
			NPCFleeingSpeed( std::stof(value) );
			break;
		case 157:	 // BASICTOOLTIPSONLY[0150]
			BasicTooltipsOnly( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 158:	 // GLOBALITEMDECAY[0151]
			GlobalItemDecay( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 159:	 // SCRIPTITEMSDECAYABLE[0152]
			ScriptItemsDecayable( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 160:	 // BASEITEMSDECAYABLE[0152]
			BaseItemsDecayable( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 161:	 // ITEMDECAYINHOUSES[0153]
			ItemDecayInHouses( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 162:	// COMBATEXPLODEDELAY[0154]
			CombatExplodeDelay( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 163:	// PAPERDOLLGUILDBUTTON[0155]
			PaperdollGuildButton( static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 164:	// ATTACKSPEEDFROMSTAMINA[0156]
			CombatAttackSpeedFromStamina( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 169:	 // DISPLAYDAMAGENUMBERS[0157]
			CombatDisplayDamageNumbers( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 170:	 // CLIENTSUPPORT4000[0158]
			ClientSupport4000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 171:	 // CLIENTSUPPORT5000[0159]
			ClientSupport5000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 172:	 // CLIENTSUPPORT6000[0160]
			ClientSupport6000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 173:	 // CLIENTSUPPORT6050[0161]
			ClientSupport6050( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 174:	 // CLIENTSUPPORT7000[0162]
			ClientSupport7000( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 175:	 // CLIENTSUPPORT7090[0163]
			ClientSupport7090( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 176:	 // CLIENTSUPPORT70160[0164]
			ClientSupport70160( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 177:	// CLIENTSUPPORT70240[0165]
			ClientSupport70240( static_cast<UI16>(std::stoul(value, nullptr, 0))== 1 );
			break;
		case 178:	// CLIENTSUPPORT70300[0166]
			ClientSupport70300( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 179:	// CLIENTSUPPORT70331[0167]
			ClientSupport70331( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 180:	// CLIENTSUPPORT704565[0168]
			ClientSupport704565( static_cast<UI16>(std::stoul(value, nullptr, 0))== 1 );
			break;
		case 181:	// CLIENTSUPPORT70610[0169]
			ClientSupport70610( static_cast<UI16>(std::stoul(value, nullptr, 0))== 1 );
			break;
		case 182:	 // EXTENDEDSTARTINGSTATS[0170]
			ExtendedStartingStats(static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 183:	 // EXTENDEDSTARTINGSKILLS[0171]
			ExtendedStartingSkills( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 184:	// WEAPONDAMAGECHANCE[0172]
			CombatWeaponDamageChance( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 185:	// ARMORDAMAGECHANCE[0173]
			CombatArmorDamageChance( static_cast<UI08>(std::stoul(value, nullptr, 0)));
			break;
		case 186:	// WEAPONDAMAGEMIN[0174]
			CombatWeaponDamageMin( static_cast<UI08>(std::stoul(value, nullptr, 0)));
			break;
		case 187:	// WEAPONDAMAGEMAX[0175]
			CombatWeaponDamageMax( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 188:	// ARMORDAMAGEMIN[0176]
			CombatArmorDamageMin( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 189:	// ARMORDAMAGEMAX[0177]
			CombatArmorDamageMax( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 190:	// GLOBALATTACKSPEED[0178]
			GlobalAttackSpeed( std::stof(value) );
			break;
		case 191:	// NPCSPELLCASTSPEED[0179]
			NPCSpellCastSpeed( std::stof(value) );
			break;
		case 192:	// FISHINGSTAMINALOSS[0180]
			FishingStaminaLoss( std::stof(value) );
			break;
		case 193:	// RANDOMSTARTINGLOCATION[0181]
			ServerRandomStartingLocation( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 194:	// ASSISTANTNEGOTIATION[0183]
			SetAssistantNegotiation( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 195:	// KICKONASSISTANTSILENCE[0184]
			KickOnAssistantSilence( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 196:	// AF_FILTERWEATHER[0185]
			SetDisabledAssistantFeature( AF_FILTERWEATHER, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 197:	// AF_FILTERLIGHT[0186]
			SetDisabledAssistantFeature( AF_FILTERLIGHT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 198:	// AF_SMARTTARGET[0187]
			SetDisabledAssistantFeature( AF_SMARTTARGET, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 199:	// AF_RANGEDTARGET[0188]
			SetDisabledAssistantFeature( AF_RANGEDTARGET, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 200:	// AF_AUTOOPENDOORS[0189]
			SetDisabledAssistantFeature( AF_AUTOOPENDOORS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 201:	// AF_DEQUIPONCAST[0190]
			SetDisabledAssistantFeature( AF_DEQUIPONCAST, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 202:	// AF_AUTOPOTIONEQUIP[0191]
			SetDisabledAssistantFeature( AF_AUTOPOTIONEQUIP, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 203:	// AF_POISONEDCHECKS[0192]
			SetDisabledAssistantFeature( AF_POISONEDCHECKS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 204:	// AF_LOOPEDMACROS[0193]
			SetDisabledAssistantFeature( AF_LOOPEDMACROS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 205:	// AF_USEONCEAGENT[0194]
			SetDisabledAssistantFeature( AF_USEONCEAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 206:	// AF_RESTOCKAGENT[0195]
			SetDisabledAssistantFeature( AF_RESTOCKAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 207:	// AF_SELLAGENT[0196]
			SetDisabledAssistantFeature( AF_SELLAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 208:	// AF_BUYAGENT[0197]
			SetDisabledAssistantFeature( AF_BUYAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 209:	// AF_POTIONHOTKEYS[0198]
			SetDisabledAssistantFeature( AF_POTIONHOTKEYS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 210:	// AF_RANDOMTARGETS[0199]
			SetDisabledAssistantFeature( AF_RANDOMTARGETS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 211:	// AF_CLOSESTTARGETS[0200]
			SetDisabledAssistantFeature( AF_CLOSESTTARGETS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 212:	// AF_OVERHEADHEALTH[0201]
			SetDisabledAssistantFeature( AF_OVERHEADHEALTH, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 213:	// AF_AUTOLOOTAGENT[0202]
			SetDisabledAssistantFeature( AF_AUTOLOOTAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 214:	// AF_BONECUTTERAGENT[0203]
			SetDisabledAssistantFeature( AF_BONECUTTERAGENT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 215:	// AF_JSCRIPTMACROS[0204]
			SetDisabledAssistantFeature( AF_JSCRIPTMACROS, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 216:	// AF_AUTOREMOUNT[0205]
			SetDisabledAssistantFeature( AF_AUTOREMOUNT, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 217:	// AF_ALL[0206]
			SetDisabledAssistantFeature( AF_ALL, static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1 );
			break;
		case 218:	// CLASSICUOMAPTRACKER[0207]
			SetClassicUOMapTracker( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 219:	// DECAYTIMERINHOUSE[0208]
			SystemTimer( tSERVER_DECAYINHOUSE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 220:	// PROTECTPRIVATEHOUSES[0209]
			ProtectPrivateHouses( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 221:	// TRACKHOUSESPERACCOUNT[0210]
			TrackHousesPerAccount( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 222:	// MAXHOUSESOWNABLE[0211]
			MaxHousesOwnable( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 223:	// MAXHOUSESCOOWNABLE[0212]
			MaxHousesCoOwnable( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 224:	// CANOWNANDCOOWNHOUSES[0213]
			CanOwnAndCoOwnHouses( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 225:	// COOWNHOUSESONSAMEACCOUNT[0214]
			CoOwnHousesOnSameAccount( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 226:	// ITEMSDETECTSPEECH[0215]
			ItemsDetectSpeech( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 227:	// MAXPLAYERPACKITEMS[0216]
			MaxPlayerPackItems( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 228:	// MAXPLAYERBANKITEMS[0217]
			MaxPlayerBankItems( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 229:	// FORCENEWANIMATIONPACKET[0218]
			ForceNewAnimationPacket( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 230:	// MAPDIFFSENABLED[0219]
			MapDiffsEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 244:	// CUOENABLED[0233]
			ConnectUOServerPoll( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 245:	// ALCHEMYBONUSENABLED[0234]
			AlchemyDamageBonusEnabled( static_cast<UI16>(std::stoul(value, nullptr, 0)) == 1 );
			break;
		case 246:	// ALCHEMYBONUSMODIFIER[0235]
			AlchemyDamageBonusModifier( static_cast<UI08>(std::stoul(value, nullptr, 0)) );
			break;
		case 247:	 // NPCFLAGUPDATETIMER[0236]
			SystemTimer( tSERVER_NPCFLAGUPDATETIMER, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 248:	 // JSENGINESIZE[0237]
			SetJSEngineSize( static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 250:	 // SCRIPTDATADIRECTORY[0239]
		{
			Directory( CSDDP_SCRIPTDATA, value );
			break;
		}
		case 251:    // THIRSTRATE[0240]
			SystemTimer( tSERVER_THIRSTRATE, static_cast<UI16>(std::stoul(value, nullptr, 0)) );
			break;
		case 252:    // THIRSTDRAINVAL[0241]
			ThirstDrain( static_cast<SI16>(std::stoi(value, nullptr, 0)) );
			break;
		case 253:    // PETTHIRSTOFFLINE[0242]
			PetThirstOffline( (static_cast<SI16>(std::stoi(value, nullptr, 0)) == 1) );
			break;
		case 254:	// ExternalIP
			ExternalIP(value);
			break;
		case 255:	 // BLOODDECAYTIMER[0243]
			SystemTimer( tSERVER_BLOODDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 256:	 // BLOODDECAYCORPSETIMER[0244]
			SystemTimer( tSERVER_BLOODDECAYCORPSE, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 257:	// BLOODEFFECTCHANCE[0245]
			CombatBloodEffectChance( static_cast<UI08>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 258:	 // NPCCORPSEDECAYTIMER[0246]
			SystemTimer( tSERVER_NPCCORPSEDECAY, static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 259:    // HUNGERENABLED[0247]
			HungerSystemEnabled( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 260:    // THIRSTENABLED[0248]
			ThirstSystemEnabled( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 261:    // TRAVELSPELLSFROMBOATKEYS[0249]
			TravelSpellsFromBoatKeys( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 262:    // TRAVELSPELLSWHILEOVERWEIGHT[0250]
			TravelSpellsWhileOverweight( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 263:    // MARKRUNESINMULTIS[0251]
			MarkRunesInMultis( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 264:    // TRAVELSPELLSBETWEENWORLD[0252]
			TravelSpellsBetweenWorlds( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 265:    // TRAVELSPELLSWHILEAGGRESSOR[0253]
			TravelSpellsWhileAggressor( ( static_cast<SI16>( std::stoi( value, nullptr, 0 ) ) == 1 ) );
			break;
		case 266:	// BUYBANKTHRESHOLD[0254]
			BuyThreshold( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) );
			break;
		case 267:	// NETWORKLOG[0255]
			ServerNetworkLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ));
			break;
		case 268:	// SPEECHLOG[0256]
			ServerSpeechLog(( static_cast<UI16>( std::stoul( value, nullptr, 0 ) ) >= 1 ? true : false ));
			break;
		case 269:	 // NPCMOUNTEDWALKINGSPEED[0257]
			NPCMountedWalkingSpeed( std::stof( value ) );
			break;
		case 270:	 // NPCMOUNTEDRUNNINGSPEED[0258]
			NPCMountedRunningSpeed( std::stof( value ) );
			break;
		case 271:	 // NPCMOUNTEDFLEEINGSPEED[0259]
			NPCMountedFleeingSpeed( std::stof( value ) );
			break;
		default:
			rvalue = false;
			break;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ServerStartGold( void ) const
//|					void ServerStartGold( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default amount of starting gold for new characters
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ServerStartGold( void ) const
{
	return startgold;
}
void CServerData::ServerStartGold( SI16 value )
{
	if( value >= 0 )
		startgold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerStartPrivs( void ) const
//|					void ServerStartPrivs( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default starting privs for new characters
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerStartPrivs( void ) const
{
	return startprivs;
}
void CServerData::ServerStartPrivs( UI16 value )
{
	startprivs = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 ServerMoon( SI16 slot ) const
//|					void ServerMoon( SI16 slot, SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the phase of one of the game's two moons
//o-----------------------------------------------------------------------------------------------o
SI16 CServerData::ServerMoon( SI16 slot ) const
{
	SI16 rvalue = -1;
	if( slot >= 0 && slot <= 1 )
		rvalue = moon[slot];
	return rvalue;
}
void CServerData::ServerMoon( SI16 slot, SI16 value )
{
	if( slot >= 0 && slot <= 1 && value >= 0 )
		moon[slot] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL DungeonLightLevel( void ) const
//|					void DungeonLightLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for dungeons
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::DungeonLightLevel( void ) const
{
	return dungeonlightlevel;
}
void CServerData::DungeonLightLevel( LIGHTLEVEL value )
{
	dungeonlightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightCurrentLevel( void ) const
//|					void WorldLightCurrentLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global current light level outside of dungeons
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::WorldLightCurrentLevel( void ) const
{
	return currentlightlevel;
}
void CServerData::WorldLightCurrentLevel( LIGHTLEVEL value )
{
	currentlightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightBrightLevel( void ) const
//|					void WorldLightBrightLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the brightest time of day
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::WorldLightBrightLevel( void ) const
{
	return brightnesslightlevel;
}
void CServerData::WorldLightBrightLevel( LIGHTLEVEL value )
{
	brightnesslightlevel = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LIGHTLEVEL WorldLightDarkLevel( void ) const
//|					void WorldLightDarkLevel( LIGHTLEVEL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the default, global light level for the darkest time of day
//o-----------------------------------------------------------------------------------------------o
LIGHTLEVEL CServerData::WorldLightDarkLevel( void ) const
{
	return darknesslightlevel;
}
void CServerData::WorldLightDarkLevel( LIGHTLEVEL value )
{
	darknesslightlevel=value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PostLoadDefaults( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	If no start locations have been provided in ini, use hardcoded defaults
//o-----------------------------------------------------------------------------------------------o
void CServerData::PostLoadDefaults( void )
{
	if( startlocations.empty() )
	{
		ServerLocation( "Yew,Center,545,982,0,0,0,1075072" );
		ServerLocation( "Minoc,Tavern,2477,411,15,0,0,1075073" );
		ServerLocation( "Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074" );
		ServerLocation( "Moonglow,Docks,4406,1045,0,0,0,1075075" );
		ServerLocation( "Trinsic,West Gate,1832,2779,0,0,0,1075076" );
		ServerLocation( "Magincia,Docks,3675,2259,26,0,0,1075077" );
		ServerLocation( "Jhelom,Docks,1492,3696,0,0,0,1075078" );
		ServerLocation( "Skara Brae,Docks,639,2236,0,0,0,1075079" );
		ServerLocation( "Vesper,Ironwood Inn,2771,977,0,0,0,1075080" );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LPSTARTLOCATION ServerLocation( size_t locNum )
//|					void ServerLocation( std::string toSet )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets server start locations
//o-----------------------------------------------------------------------------------------------o
LPSTARTLOCATION CServerData::ServerLocation( size_t locNum )
{
	LPSTARTLOCATION rvalue = nullptr;
	if( locNum < startlocations.size() )
		rvalue = &startlocations[locNum];
	return rvalue;
}
void CServerData::ServerLocation( std::string toSet )
{
	auto temp = toSet;
	temp = strutil::stripTrim( temp );
	auto csecs = strutil::sections( temp, "," );
	
	if( csecs.size() ==  7 )	// Wellformed server location
	{
		STARTLOCATION toAdd;
		toAdd.x				= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[2] ), nullptr, 0));
		toAdd.y				= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[3] ), nullptr, 0));
		toAdd.z				= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[4] ), nullptr, 0));
		toAdd.worldNum		= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[5] ), nullptr, 0));
		toAdd.instanceID	= 0;
		toAdd.clilocDesc	= static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[6] ), nullptr, 0));
		strcpy( toAdd.oldTown, strutil::stripTrim( csecs[0] ).c_str() );
		strcpy( toAdd.oldDescription, strutil::stripTrim( csecs[1] ).c_str() );
		strcpy( toAdd.newTown, strutil::stripTrim( csecs[0] ).c_str()) ;
		strcpy( toAdd.newDescription, strutil::stripTrim( csecs[1] ).c_str() );
		startlocations.push_back( toAdd );
	}
	else if( csecs.size() ==  8 )	// instanceID included
	{
		STARTLOCATION toAdd;
		toAdd.x				= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[2] ), nullptr, 0));
		toAdd.y				= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[3] ), nullptr, 0));
		toAdd.z				= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[4] ), nullptr, 0));
		toAdd.worldNum		= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[5] ), nullptr, 0));
		toAdd.instanceID	= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[6] ), nullptr, 0));
		toAdd.clilocDesc	= static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[7] ), nullptr, 0));
		strcpy( toAdd.oldTown, strutil::stripTrim( csecs[0] ).c_str() );
		strcpy( toAdd.oldDescription, strutil::stripTrim( csecs[1] ).c_str() );
		strcpy( toAdd.newTown, strutil::stripTrim( csecs[0] ).c_str()) ;
		strcpy( toAdd.newDescription, strutil::stripTrim( csecs[1] ).c_str() );
		startlocations.push_back( toAdd );
	}
	else
	{
		Console.error( "Malformed location entry in ini file" );
	}
}

size_t CServerData::NumServerLocations( void ) const
{
	return startlocations.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ServerSecondsPerUOMinute( void ) const
//|					void ServerSecondsPerUOMinute( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the number of real life seconds per UO minute
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::ServerSecondsPerUOMinute( void ) const
{
	return secondsperuominute;
}
void CServerData::ServerSecondsPerUOMinute( UI16 newVal )
{
	secondsperuominute = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetJSEngineSize( void ) const
//|					void SetJSEngineSize( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets jsEngineSize (in MB), used to define max bytes per JSRuntime
//|					before a last ditch GC effort is made
//o-----------------------------------------------------------------------------------------------o
UI16 CServerData::GetJSEngineSize( void ) const
{
	return jsEngineSize;
}
void CServerData::SetJSEngineSize( UI16 newVal )
{
	jsEngineSize = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SaveTime( void )
//|	Date		-	January 28th, 2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Outputs server time information to time.wsc in the /shared/ directory
//o-----------------------------------------------------------------------------------------------o
void CServerData::SaveTime( void )
{
	std::string		timeFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";
	std::ofstream	timeDestination( timeFile.c_str() );
	if( !timeDestination )
	{
		Console.error( strutil::format("Failed to open %s for writing", timeFile.c_str()) );
		return;
	}

	timeDestination << "[TIME]" << '\n' << "{" << '\n';
	timeDestination << "CURRENTLIGHT=" << static_cast<UI16>(WorldLightCurrentLevel()) << '\n';
	timeDestination << "DAY=" << ServerTimeDay() << '\n';
	timeDestination << "HOUR=" << static_cast<UI16>(ServerTimeHours()) << '\n';
	timeDestination << "MINUTE=" << static_cast<UI16>(ServerTimeMinutes()) << '\n';
	timeDestination << "AMPM=" << ( ServerTimeAMPM() ? 1 : 0 ) << '\n';
	timeDestination << "MOON=" << ServerMoon( 0 ) << "," << ServerMoon( 1 ) << '\n';
	timeDestination << "}" << '\n' << '\n';

	timeDestination.close();
}

void ReadWorldTagData( std::ifstream &inStream, std::string &tag, std::string &data );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CServerData::LoadTime( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads server time information from time.wsc in the /shared/ directory
//o-----------------------------------------------------------------------------------------------o
void CServerData::LoadTime( void )
{
	std::ifstream input;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "time.wsc";

	input.open( filename.c_str(), std::ios_base::in );
	input.seekg( 0, std::ios::beg );


	if( input.is_open() )
	{
		char line[1024];
		while( !input.eof() && !input.fail() )
		{
			input.getline(line, 1023);
			line[input.gcount()] = 0;
			std::string sLine(line);
			sLine = strutil::stripTrim( sLine );
			if( !sLine.empty() )
			{
				if( strutil::toupper( sLine ) == "[TIME]" )
					LoadTimeTags( input );
			}
		}
		input.close();
	}
}

void CServerData::LoadTimeTags( std::ifstream &input )
{
	std::string UTag, tag, data;
	while( tag != "o---o" )
	{
		ReadWorldTagData( input, tag, data );
		if( tag != "o---o" )
		{
			UTag = strutil::toupper(tag);
			
			if( UTag == "AMPM" )
			{
				ServerTimeAMPM( (std::stoi(data, nullptr, 0) == 1) );
			}
			else if( UTag == "CURRENTLIGHT" )
			{
				WorldLightCurrentLevel( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
			else if( UTag == "DAY" )
			{
				ServerTimeDay( static_cast<SI16>(std::stoi(data, nullptr, 0)) );
			}
			else if( UTag == "HOUR" )
			{
				ServerTimeHours( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
			else if( UTag == "MINUTE" )
			{
				ServerTimeMinutes( static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
			else if( UTag == "MOON" )
			{
				auto csecs = strutil::sections( data, "," );
				if( csecs.size() > 1 )
				{
					ServerMoon( 0, static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					ServerMoon( 1, static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
				}
			}
		}
	}
	tag = "";
}

SI16 CServerData::ServerTimeDay( void ) const
{
	return days;
}
void CServerData::ServerTimeDay( SI16 nValue )
{
	days = nValue;
}
UI08 CServerData::ServerTimeHours( void ) const
{
	return hours;
}
void CServerData::ServerTimeHours( UI08 nValue )
{
	hours = nValue;
}
UI08 CServerData::ServerTimeMinutes( void ) const
{
	return minutes;
}

void CServerData::ServerTimeMinutes( UI08 nValue )
{
	minutes = nValue;
}
UI08 CServerData::ServerTimeSeconds( void ) const
{
	return seconds;
}
void CServerData::ServerTimeSeconds( UI08 nValue )
{
	seconds = nValue;
}
bool CServerData::ServerTimeAMPM( void ) const
{
	return ampm;
}
void CServerData::ServerTimeAMPM( bool nValue )
{
	ampm = nValue;
}

bool CServerData::incSecond( void )
{
	bool rvalue = false;
	++seconds;
	if( seconds == 60 )
	{
		seconds = 0;
		rvalue	= incMinute();
	}
	return rvalue;
}

void CServerData::incMoon( SI32 mNumber )
{
	moon[mNumber] = (SI16)((moon[mNumber] + 1)%8);
}

bool CServerData::incMinute( void )
{
	bool rvalue = false;
	++minutes;
	if( minutes%8 == 0 )
		incMoon( 0 );
	if( minutes%3 == 0 )
		incMoon( 1 );

	if( minutes == 60 )
	{
		minutes = 0;
		rvalue	= incHour();
	}
	return rvalue;
}

bool CServerData::incHour( void )
{
	++hours;
	bool retVal = false;
	if( hours == 12 )
	{
		if( ampm )
			retVal = incDay();
		hours	= 0;
		ampm	= !ampm;
	}
	return retVal;
}

bool CServerData::incDay( void )
{
	++days;
	return true;
}

physicalServer *CServerData::ServerEntry( UI16 entryNum )
{
	physicalServer *rvalue = nullptr;
	if( entryNum < serverList.size() )
		rvalue = &serverList[entryNum];
	return rvalue;
}
UI16 CServerData::ServerCount( void ) const
{
	return static_cast<UI16>(serverList.size());
}

void physicalServer::setName(const std::string& newName)
{
	name = newName;
}
void physicalServer::setDomain(const std::string& newDomain)
{
	domain = newDomain;
}
void physicalServer::setIP(const std::string& newIP)
{
	ip = newIP;
}
void physicalServer::setPort(UI16 newPort)
{
	port = newPort;
}
std::string physicalServer::getName( void ) const
{
	return name;
}
std::string physicalServer::getDomain( void ) const
{
	return domain;
}
std::string physicalServer::getIP( void ) const
{
	return ip;
}
UI16 physicalServer::getPort( void ) const
{
	return port;
}
