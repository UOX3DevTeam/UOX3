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


JSBool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CSpellProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CSpellProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CGlobalSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CGlobalSkillProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CGlobalSkillProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CCreateEntriesProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CCreateEntryProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CCreateEntryProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CTimerProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );

JSBool CItemProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CItemProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );
JSBool CCharacterProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CCharacterProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );
JSBool CBaseObject_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

JSBool CRegionProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CRegionProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CSpawnRegionProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CSpawnRegionProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CGuildProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CGuildProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );
JSBool CGuildsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CGuildsProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CRaceProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CRaceProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CSocketProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CSocketProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );
JSBool CSocket_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

JSBool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CSkillsProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CGumpDataProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );

JSBool CFileProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CFileProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CAccountProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CAccountProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CConsoleProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CConsoleProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CScriptSectionProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CScriptSectionProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CScriptProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );

JSBool CResourceProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CResourceProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );

JSBool CPartyProps_getProperty( JSContext *cx, JSObject *obj, jsid id, jsval *vp );
JSBool CPartyProps_setProperty( JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp );
JSBool CParty_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp );

#endif
