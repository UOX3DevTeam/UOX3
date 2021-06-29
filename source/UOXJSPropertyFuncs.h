//o-----------------------------------------------------------------------------------------------o
//|	File		-	UOXJSPropertyFuncts.cpp
//|	Date		-	12/14/2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	1.0		14th December, 2001 Initial implementation
//|							Includes property getters for CItem and CChar, and property
//|							setters for CChar
//o-----------------------------------------------------------------------------------------------o
#ifndef __UOXJSPropertyFuncs__
#define __UOXJSPropertyFuncs__

typedef JSBool (JSPropertyFunc)( JSContext *cx, JSObject *obj, jsval id, jsval *vp );

JSPropertyFunc CSpellsProps_getProperty;
JSPropertyFunc CSpellProps_getProperty;
JSPropertyFunc CSpellProps_setProperty;

JSPropertyFunc CCreateEntriesProps_getProperty;
JSPropertyFunc CCreateEntryProps_getProperty;
JSPropertyFunc CCreateEntryProps_setProperty;

JSPropertyFunc CItemProps_getProperty;
JSPropertyFunc CItemProps_setProperty;
JSPropertyFunc CCharacterProps_getProperty;
JSPropertyFunc CCharacterProps_setProperty;
JSBool CBaseObject_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

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
JSBool CSocket_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

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

JSPropertyFunc CResourceProps_getProperty;
JSPropertyFunc CResourceProps_setProperty;

JSPropertyFunc CPartyProps_getProperty;
JSPropertyFunc CPartyProps_setProperty;
JSBool CParty_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

#endif
