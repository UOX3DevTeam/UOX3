//o--------------------------------------------------------------------------o
//|	File					-	UOXJSPropertyFuncts.cpp
//|	Date					-	12/14/2001
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	1.0		Abaddon		14th December, 2001 Initial implementation
//|									Includes property getters for CItem and CChar, and property
//|									setters for CChar
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __UOXJSPropertyFuncs__
#define __UOXJSPropertyFuncs__

JSBool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CSpellProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CSpellProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CItemProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CItemProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CItemsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CCharacterProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CCharacterProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CRegionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CRegionProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CGuildProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CGuildProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CGuildsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CGuildsProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CRaceProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CRaceProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CSocketProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CSocketProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CSkillsProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );
JSBool CGumpDataProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp );


#endif
