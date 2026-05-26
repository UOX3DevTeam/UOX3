//o------------------------------------------------------------------------------------------------o
//|	File		-	UOXJSPropertyOpts.cpp
//|	Date		-	12/14/2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	1.0		14th December, 2001 Initial implementation
//|							Includes property getters for CItem and CChar, and property
//|							setters for CChar
//o------------------------------------------------------------------------------------------------o
#ifndef __UOXJSPropertyOps__
#define __UOXJSPropertyOps__

extern std::map< std::string, int >		propLookupAccount;
extern std::map< std::string, int >		propLookupChar;
extern std::map< std::string, int >		propLookupConsole;
extern std::map< std::string, int >		propLookupGuild;
extern std::map< std::string, int >		propLookupItem;
extern std::map< std::string, int >		propLookupParty;
extern std::map< std::string, int >		propLookupRace;
extern std::map< std::string, int >		propLookupRegion;
extern std::map< std::string, int >		propLookupResource;
extern std::map< std::string, int >		propLookupSkills;
extern std::map< std::string, int >		propLookupSocket;
extern std::map< std::string, int >		propLookupSpawnRegion;

typedef bool ( JSPropertyFunc )( JSContext *cx, JSObject *obj, JS::Value id, JS::Value *vp );

JSPropertyFunc CSpellsProps_getProperty;
JSPropertyFunc CSpellProps_getProperty;
JSPropertyFunc CSpellProps_setProperty;

JSPropertyFunc CGlobalSkillsProps_getProperty;
JSPropertyFunc CGlobalSkillProps_getProperty;
JSPropertyFunc CGlobalSkillProps_setProperty;

JSPropertyFunc CCreateEntriesProps_getProperty;
JSPropertyFunc CCreateEntryProps_getProperty;
JSPropertyFunc CCreateEntryProps_setProperty;

JSPropertyFunc CTimerProps_getProperty;

JSPropertyFunc CItemProps_getProperty;
JSPropertyFunc CItemProps_setProperty;
JSPropertyFunc CCharacterProps_getProperty;
JSPropertyFunc CCharacterProps_setProperty;

JSPropertyFunc CRegionProps_getProperty;
JSPropertyFunc CRegionProps_setProperty;

JSPropertyFunc CSpawnRegionProps_getProperty;
JSPropertyFunc CSpawnRegionProps_setProperty;

JSPropertyFunc CGuildProps_getProperty;
JSPropertyFunc CGuildProps_setProperty;
JSPropertyFunc CGuildsProps_getProperty;
JSPropertyFunc CGuildsProps_setProperty;

JSPropertyFunc CRaceProps_getProperty;
JSPropertyFunc CRaceProps_setProperty;

JSPropertyFunc CSocketProps_getProperty;
JSPropertyFunc CSocketProps_setProperty;

JSPropertyFunc CSkillsProps_getProperty;
JSPropertyFunc CSkillsProps_setProperty;

JSPropertyFunc CGumpDataProps_getProperty;

JSPropertyFunc CFileProps_getProperty;
JSPropertyFunc CFileProps_setProperty;

JSPropertyFunc CAccountProps_getProperty;
JSPropertyFunc CAccountProps_setProperty;

JSPropertyFunc CConsoleProps_getProperty;
JSPropertyFunc CConsoleProps_setProperty;

JSPropertyFunc CScriptSectionProps_getProperty;
JSPropertyFunc CScriptSectionProps_setProperty;

JSPropertyFunc CScriptProps_getProperty;

JSPropertyFunc CResourceProps_getProperty;
JSPropertyFunc CResourceProps_setProperty;

JSPropertyFunc CPartyProps_getProperty;
JSPropertyFunc CPartyProps_setProperty;

#endif
