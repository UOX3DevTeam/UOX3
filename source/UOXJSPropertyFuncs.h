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


JSPropertyOp CSpellsProps_getProperty;
JSPropertyOp CSpellProps_getProperty;
JSStrictPropertyOp CSpellProps_setProperty;

JSPropertyOp CGlobalSkillsProps_getProperty;
JSPropertyOp CGlobalSkillProps_getProperty;
JSStrictPropertyOp CGlobalSkillProps_setProperty;

JSPropertyOp CCreateEntriesProps_getProperty;
JSPropertyOp CCreateEntryProps_getProperty;
JSStrictPropertyOp CCreateEntryProps_setProperty;

JSPropertyOp CTimerProps_getProperty;

JSPropertyOp CItemProps_getProperty;
JSStrictPropertyOp CItemProps_setProperty;
JSPropertyOp CCharacterProps_getProperty;
JSStrictPropertyOp CCharacterProps_setProperty;
JSBool CBaseObject_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

JSPropertyOp CRegionProps_getProperty;
JSStrictPropertyOp CRegionProps_setProperty;

JSPropertyOp CSpawnRegionProps_getProperty;
JSStrictPropertyOp CSpawnRegionProps_setProperty;

JSPropertyOp CGuildProps_getProperty;
JSStrictPropertyOp CGuildProps_setProperty;
JSPropertyOp CGuildsProps_getProperty;
JSStrictPropertyOp CGuildsProps_setProperty;

JSPropertyOp CRaceProps_getProperty;
JSStrictPropertyOp CRaceProps_setProperty;

JSPropertyOp CSocketProps_getProperty;
JSStrictPropertyOp CSocketProps_setProperty;
JSBool CSocket_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

JSPropertyOp CSkillsProps_getProperty;
JSStrictPropertyOp CSkillsProps_setProperty;

JSPropertyOp CGumpDataProps_getProperty;

JSPropertyOp CFileProps_getProperty;
JSStrictPropertyOp CFileProps_setProperty;

JSPropertyOp CAccountProps_getProperty;
JSStrictPropertyOp CAccountProps_setProperty;

JSPropertyOp CConsoleProps_getProperty;
JSStrictPropertyOp CConsoleProps_setProperty;

JSPropertyOp CScriptSectionProps_getProperty;
JSStrictPropertyOp CScriptSectionProps_setProperty;

JSPropertyOp CScriptProps_getProperty;

JSPropertyOp CResourceProps_getProperty;
JSStrictPropertyOp CResourceProps_setProperty;

JSPropertyOp CPartyProps_getProperty;
JSStrictPropertyOp CPartyProps_setProperty;
JSBool CParty_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

#endif
