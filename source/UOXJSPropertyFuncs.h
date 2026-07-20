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

typedef bool ( JSPropertyFunc )( JSContext *cx, JSObject *obj, jsid id, JS::Value *vp );
typedef bool ( JSPropertySetter )( JSContext *cx, JSObject *obj, jsid id, bool strict, JS::Value *vp );

JSPropertyFunc CSpellsProps_getProperty;
JSPropertyFunc CSpellProps_getProperty;
JSPropertySetter CSpellProps_setProperty;

JSPropertyFunc CGlobalSkillsProps_getProperty;
JSPropertyFunc CGlobalSkillProps_getProperty;
JSPropertySetter CGlobalSkillProps_setProperty;

JSPropertyFunc CCreateEntriesProps_getProperty;
JSPropertyFunc CCreateEntryProps_getProperty;
JSPropertySetter CCreateEntryProps_setProperty;

JSPropertyFunc CTimerProps_getProperty;

JSPropertyFunc CItemProps_getProperty;
JSPropertySetter CItemProps_setProperty;
JSPropertyFunc CCharacterProps_getProperty;
JSPropertySetter CCharacterProps_setProperty;

JSPropertyFunc CRegionProps_getProperty;
JSPropertySetter CRegionProps_setProperty;

JSPropertyFunc CSpawnRegionProps_getProperty;
JSPropertySetter CSpawnRegionProps_setProperty;

JSPropertyFunc CGuildProps_getProperty;
JSPropertySetter CGuildProps_setProperty;
JSPropertyFunc CGuildsProps_getProperty;
JSPropertySetter CGuildsProps_setProperty;

JSPropertyFunc CRaceProps_getProperty;
JSPropertySetter CRaceProps_setProperty;

JSPropertyFunc CSocketProps_getProperty;
JSPropertySetter CSocketProps_setProperty;

JSPropertyFunc CSkillsProps_getProperty;
JSPropertySetter CSkillsProps_setProperty;

JSPropertyFunc CGumpDataProps_getProperty;

JSPropertyFunc CFileProps_getProperty;
JSPropertySetter CFileProps_setProperty;

JSPropertyFunc CAccountProps_getProperty;
JSPropertySetter CAccountProps_setProperty;

JSPropertyFunc CConsoleProps_getProperty;
JSPropertySetter CConsoleProps_setProperty;

JSPropertyFunc CScriptSectionProps_getProperty;
JSPropertySetter CScriptSectionProps_setProperty;

JSPropertyFunc CScriptProps_getProperty;

JSPropertyFunc CResourceProps_getProperty;
JSPropertySetter CResourceProps_setProperty;

JSPropertyFunc CPartyProps_getProperty;
JSPropertySetter CPartyProps_setProperty;

#endif
