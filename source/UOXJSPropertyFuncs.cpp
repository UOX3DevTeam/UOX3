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
#include "uox3.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"
#include "UOXJSPropertyEnums.h"
#include "UOXJSPropertyFuncs.h"

#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "trigger.h"
#include "cScript.h"

JSBool CGuildsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	*vp = INT_TO_JSVAL(0);
	return JS_TRUE;
}

JSBool CGuildsProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	return JS_TRUE;
}

JSBool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	UI32 SpellID = JSVAL_TO_INT(id);

	if( SpellID >= spells.size() )
	{
		Console.Error( 2, "Invalid Spell ID" ); // Revise please...
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsSpell = JS_NewObject( cx, &UOXSpell_class, NULL, obj );
	JS_DefineProperties( cx, jsSpell, CSpellProperties );
	JS_SetPrivate( cx, jsSpell, &spells[SpellID ] );

	*vp = OBJECT_TO_JSVAL( jsSpell );
	return JS_TRUE;
}

JSBool CSpellProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	return JS_TRUE;
}

JSBool CSpellProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	SpellInfo *gPriv = (SpellInfo*)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	JSString *tString = NULL;
	UI32 i;
	bool bDone = false;

	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CSP_ID:
			for( i = 0; i < spells.size() && !bDone; i++ )
			{
				if( &spells[i] == gPriv ) 
				{
					*vp = INT_TO_JSVAL( i );
					bDone = true;
				}
			}
			break;
		case CSP_ACTION:			*vp = INT_TO_JSVAL( gPriv->Action() );					break;
		case CSP_DELAY:				*vp = INT_TO_JSVAL( gPriv->Delay() );					break;
		case CSP_HEALTH:			*vp = INT_TO_JSVAL( gPriv->Health() );					break;
		case CSP_STAMINA:			*vp = INT_TO_JSVAL( gPriv->Stamina() );					break;
		case CSP_MANA:				*vp = INT_TO_JSVAL( gPriv->Mana() );					break;
		case CSP_MANTRA:			tString = JS_NewStringCopyZ( cx, gPriv->Mantra() );
									*vp = STRING_TO_JSVAL( tString );
									break;
		case CSP_STRTOSAY:			tString = JS_NewStringCopyZ( cx, gPriv->StringToSay() );
									*vp = STRING_TO_JSVAL( tString );
									break;
		case CSP_SCROLLLOW:			*vp = INT_TO_JSVAL( gPriv->ScrollLow() );				break;
		case CSP_SCROLLHIGH:		*vp = INT_TO_JSVAL( gPriv->ScrollHigh() );				break;
		case CSP_CIRCLE:			*vp = INT_TO_JSVAL( gPriv->Circle() );					break;
		case CSP_LOWSKILL:			*vp = INT_TO_JSVAL( gPriv->LowSkill() );				break;
		case CSP_HIGHSKILL:			*vp = INT_TO_JSVAL( gPriv->HighSkill() );				break;
		case CSP_GINSENG:			*vp = INT_TO_JSVAL( gPriv->Reagants().ginseng );		break;
		case CSP_MOSS:				*vp = INT_TO_JSVAL( gPriv->Reagants().moss );			break;
		case CSP_DRAKE:				*vp = INT_TO_JSVAL( gPriv->Reagants().drake );			break;
		case CSP_PEARL:				*vp = INT_TO_JSVAL( gPriv->Reagants().pearl );			break;
		case CSP_SILK:				*vp = INT_TO_JSVAL( gPriv->Reagants().silk );			break;
		case CSP_ASH:				*vp = INT_TO_JSVAL( gPriv->Reagants().ash );			break;
		case CSP_SHADE:				*vp = INT_TO_JSVAL( gPriv->Reagants().shade );			break;
		case CSP_GARLIC:			*vp = INT_TO_JSVAL( gPriv->Reagants().garlic );			break;
		case CSP_REQUIRETARGET:		*vp = BOOLEAN_TO_JSVAL( gPriv->RequireTarget() );		break;
		case CSP_REQUIREITEM:		*vp = BOOLEAN_TO_JSVAL( gPriv->RequireItemTarget() );	break;
		case CSP_REQUIRECHAR:		*vp = BOOLEAN_TO_JSVAL( gPriv->RequireCharTarget() );	break;
		case CSP_REQUIRELOCATION:	*vp = BOOLEAN_TO_JSVAL( gPriv->RequireLocTarget() );	break;
		case CSP_TRAVELSPELL:		*vp = BOOLEAN_TO_JSVAL( gPriv->TravelSpell() );			break;
		case CSP_FIELDSPELL:		*vp = BOOLEAN_TO_JSVAL( gPriv->FieldSpell() );			break;
		case CSP_REFLECTABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->SpellReflectable() );	break;
		case CSP_AGRESSIVESPELL:	*vp = BOOLEAN_TO_JSVAL( gPriv->AggressiveSpell() );		break;
		case CSP_RESISTABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->Resistable() );			break;
		default:																			break;
		}
	}
	return JS_TRUE;
}

JSBool CItemProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CItem *gPriv = (CItem *)JS_GetPrivate( cx, obj );
	SERIAL TempSerial = INVALIDSERIAL;

	if( gPriv == NULL )
		return JS_FALSE;
	JSString *tString = NULL;
	CRace *TempRace = NULL;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CIP_NAME:
			tString = JS_NewStringCopyZ( cx, gPriv->GetName() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CIP_TITLE:
			tString = JS_NewStringCopyZ( cx, gPriv->GetTitle() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CIP_X:			*vp = INT_TO_JSVAL( gPriv->GetX() );			break;
		case CIP_Y:			*vp = INT_TO_JSVAL( gPriv->GetY() );			break;
		case CIP_Z:			*vp = INT_TO_JSVAL( gPriv->GetZ() );			break;
		case CIP_ID:		*vp = INT_TO_JSVAL( gPriv->GetID() );			break;
		case CIP_COLOUR:	*vp = INT_TO_JSVAL( gPriv->GetColour() );		break;
		case CIP_OWNER:		
			// The owner property is not the pack's owner, but the item's owner.
			// If you want the container's owner, look that up instead
			if(*vp!=JSVAL_NULL)
			{
				CChar *pOwner = (CChar *)gPriv->GetOwnerObj();

				if( pOwner == NULL )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
					cScript *myScript = Trigger->GetScript( myScpTrig );
	
					JSObject *myObj = myScript->AcquireObject( IUE_CHAR );
					JS_SetPrivate( cx, myObj, pOwner );
					*vp = OBJECT_TO_JSVAL( myObj );
				}
			}
			else
			{
					gPriv->SetOwner((cBaseObject *)NULL);
			}
			break;
		case CIP_VISIBLE:		*vp = INT_TO_JSVAL( gPriv->GetVisible() );			break;
		case CIP_SERIAL:		
			if( gPriv->GetSerial() != INVALIDSERIAL )
			{
				*vp = INT_TO_JSVAL( gPriv->GetSerial() - BASEITEMSERIAL );
			}
			else
			{
				*vp = INT_TO_JSVAL( INVALIDSERIAL );
			}
			break;
		case CIP_HEALTH:		*vp = INT_TO_JSVAL( gPriv->GetHP() );				break;
		case CIP_SCRIPTTRIGGER:	*vp = INT_TO_JSVAL( gPriv->GetScriptTrigger() );	break;
		case CIP_PRIVATEWORD:
			
			break;
		case CIP_WORLDNUMBER:	*vp = INT_TO_JSVAL( gPriv->WorldNumber() );			break;
		case CIP_AMOUNT:		*vp = INT_TO_JSVAL( gPriv->GetAmount() );			break;
		
		// We should Accquie an object here and return that, so you can do
		// item.container.name
		case CIP_CONTAINER:
			TempSerial = gPriv->GetContSerial();

			if( TempSerial == INVALIDSERIAL )
				// Return a JS_NULL
				*vp = JSVAL_NULL;
			else
			{
				// Otherwise Acquire an object
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				if( TempSerial >= BASEITEMSERIAL )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
				{
					JSObject *myItem = myScript->AcquireObject( IUE_ITEM );
					JS_SetPrivate( cx, myItem, calcItemObjFromSer( TempSerial ) );
					*vp = OBJECT_TO_JSVAL( myItem );
				}
				else
				{
					JSObject *myChar = myScript->AcquireObject( IUE_CHAR );
					JS_SetPrivate( cx, myChar, calcCharObjFromSer( TempSerial ) );
					*vp = OBJECT_TO_JSVAL( myChar );
				}
			}

			break;
		case CIP_TYPE:			*vp = INT_TO_JSVAL( gPriv->GetType() );				break;
		case CIP_MORE:			*vp = INT_TO_JSVAL( gPriv->GetMore() );				break;
		case CIP_MOREX:			*vp = INT_TO_JSVAL( gPriv->GetMoreX() );			break;
		case CIP_MOREY:			*vp = INT_TO_JSVAL( gPriv->GetMoreY() );			break;
		case CIP_MOREZ:			*vp = INT_TO_JSVAL( gPriv->GetMoreZ() );			break;
		case CIP_MOVABLE:		*vp = INT_TO_JSVAL( gPriv->GetMovable() );			break;
		case CIP_ATT:			*vp = INT_TO_JSVAL( RandomNum( gPriv->GetLoDamage(), gPriv->GetHiDamage() ) );	break;
		case CIP_DEF:			*vp = INT_TO_JSVAL( gPriv->GetDef() );				break;
		case CIP_LAYER:			*vp = INT_TO_JSVAL( gPriv->GetLayer() );			break;
		case CIP_ITEMSINSIDE:	*vp = INT_TO_JSVAL( gPriv->NumItems() );			break;
		case CIP_DECAYABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->isDecayable() );		break;
		case CIP_DECAYTIME:		*vp = INT_TO_JSVAL( gPriv->GetDecayTime() );		break;
		case CIP_LODAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetLoDamage() );			break;
		case CIP_HIDAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetHiDamage() );			break;
		case CIP_NAME2:
			tString = JS_NewStringCopyZ( cx, gPriv->GetName2() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CIP_ISCHAR:	*vp = JSVAL_FALSE;								break;
		case CIP_ISITEM:	*vp = JSVAL_TRUE;								break;
		case CIP_RACE:
			TempRace = Races->Race( gPriv->GetRace() );

			if( TempRace == NULL )
			{
				*vp = JSVAL_NULL;
			}
			else
			{
				// Otherwise Acquire an object
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				JSObject *myRace = myScript->AcquireObject( IUE_RACE );
				JS_SetPrivate( cx, myRace, TempRace );
				*vp = OBJECT_TO_JSVAL( myRace );
			}
			break;
		case CIP_RACEID:		*vp = INT_TO_JSVAL( gPriv->GetRace() );			break;
		case CIP_MAXHP:			*vp = INT_TO_JSVAL( gPriv->GetMaxHP() );		break;
		case CIP_RANK:			*vp = INT_TO_JSVAL( gPriv->GetRank() );			break;
		case CIP_POISON:		*vp = INT_TO_JSVAL( gPriv->GetPoisoned() );		break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CCharacterProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	SERIAL TempSerial = INVALIDSERIAL;
	CItem *TempItem = NULL;
	CRace *TempRace = NULL;
	JSObject *TempObject = NULL;
	UI08 TempTownID = 0xFF;
	GuildID TempGuildID = INVALIDID; // should work, shouldn't it ?
	CChar *gPriv = (CChar *)JS_GetPrivate( cx, obj );

	if( gPriv == NULL )
		return JS_FALSE;
	JSString *tString = NULL;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CCP_NAME:
			tString = JS_NewStringCopyZ( cx, gPriv->GetName() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CCP_TITLE:
			tString = JS_NewStringCopyZ( cx, gPriv->GetTitle() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CCP_X:			*vp = INT_TO_JSVAL( gPriv->GetX() );			break;
		case CCP_Y:			*vp = INT_TO_JSVAL( gPriv->GetY() );			break;
		case CCP_Z:			*vp = INT_TO_JSVAL( gPriv->GetZ() );			break;
		case CCP_ID:		*vp = INT_TO_JSVAL( gPriv->GetID() );			break;
		case CCP_COLOUR:	*vp = INT_TO_JSVAL( gPriv->GetColour() );		break;
		case CCP_OWNER:
			// The Item-thingy for characters as well (damn what possibiliteis we've got :)
			cBaseObject *TempObj;
			TempObj = gPriv->GetOwnerObj();

			if( TempObj == NULL )
			{
				// Return a JS_NULL
				*vp = JSVAL_NULL;
			}
			else
			{
				// Otherwise Acquire an object
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				JSObject *myChar = myScript->AcquireObject( IUE_CHAR );
				JS_SetPrivate( cx, myChar, TempObj );
				*vp = OBJECT_TO_JSVAL( myChar );
			}

			break;
		case CCP_VISIBLE:		*vp = INT_TO_JSVAL( gPriv->GetVisible() );			break;
		case CCP_SERIAL:		*vp = INT_TO_JSVAL( gPriv->GetSerial() );			break;
		case CCP_HEALTH:		*vp = INT_TO_JSVAL( gPriv->GetHP() );				break;
		case CCP_SCRIPTTRIGGER:	*vp = INT_TO_JSVAL( gPriv->GetScriptTrigger() );	break;
		case CCP_PRIVATEWORD:
			
			break;
		case CCP_WORLDNUMBER:	*vp = INT_TO_JSVAL( gPriv->WorldNumber() );			break;
		case CCP_TARGET:
			// Hm Quite funny, same thing as .owner
			TempSerial = gPriv->GetTarg();

			if( TempSerial == INVALIDSERIAL )
			{
				// Return a JS_NULL
				*vp = JSVAL_NULL;
			}
			else
			{
				// Otherwise Acquire an object
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				JSObject *myChar = myScript->AcquireObject( IUE_CHAR );
				JS_SetPrivate( cx, myChar, &chars[TempSerial] );
				*vp = OBJECT_TO_JSVAL( myChar );
			}

			break;
		case CCP_DEXTERITY:		*vp = INT_TO_JSVAL( gPriv->GetDexterity() );	break;
		case CCP_INTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->GetIntelligence() );	break;
		case CCP_STRENGTH:		*vp = INT_TO_JSVAL( gPriv->GetStrength() );		break;
		case CCP_BASESKILLS:
			TempObject = JS_NewObject( cx, &UOXBaseSkills_class, NULL, obj );
			JS_DefineProperties( cx, TempObject, CSkillsProps );
			JS_SetPrivate( cx, TempObject, gPriv );
			*vp = OBJECT_TO_JSVAL( TempObject );
			
			break;
		case CCP_SKILLS:
			TempObject = JS_NewObject( cx, &UOXSkills_class, NULL, obj );
			JS_DefineProperties( cx, TempObject, CSkillsProps );
			JS_SetPrivate( cx, TempObject, gPriv );
			*vp = OBJECT_TO_JSVAL( TempObject );
			
			break;
		case CCP_MANA:			*vp = INT_TO_JSVAL( gPriv->GetMana() );			break;
		case CCP_STAMINA:		*vp = INT_TO_JSVAL( gPriv->GetStamina() );		break;
		
		// Used that to return either JSVAL_NULL if there is no backpack
		// or to return an item object (which is the backpack)
		// sorry, didn't know what the identifier was meant for
		case CCP_CHARPACK:
			TempItem = getPack( gPriv );

			if( TempItem == NULL )
			{
				// Return a JS_NULL
				*vp = JSVAL_NULL;
			}
			else
			{
				// Otherwise Acquire an object
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				JSObject *myItem = myScript->AcquireObject( IUE_ITEM );
				JS_SetPrivate( cx, myItem, TempItem );
				*vp = OBJECT_TO_JSVAL( myItem );
			}
	
			break;
		case CCP_FAME:			*vp = INT_TO_JSVAL( gPriv->GetFame() );						break;
		case CCP_KARMA:			*vp = INT_TO_JSVAL( gPriv->GetKarma() );					break;
		case CCP_DEFENSE:		*vp = INT_TO_JSVAL( Combat->calcDef( gPriv, 0, true ) );	break;
		case CCP_ATTACK:		*vp = INT_TO_JSVAL( Combat->calcAtt( gPriv ) );				break;
		case CCP_HUNGER:		*vp = INT_TO_JSVAL( gPriv->GetHunger() );					break;
		case CCP_FROZEN:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsFrozen() );				break;
		case CCP_COMMANDLEVEL:	*vp = INT_TO_JSVAL( gPriv->GetCommandLevel() );				break;
		// Allocate a race object here and return that... (a lot nicer, isn't it?)
		case CCP_RACE:
			TempRace = Races->Race( gPriv->GetRace() );

			if( TempRace == NULL )
			{
				*vp = JSVAL_NULL;
			}
			else
			{
				// Otherwise Acquire an object
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				JSObject *myRace = myScript->AcquireObject( IUE_RACE );
				JS_SetPrivate( cx, myRace, TempRace );
				*vp = OBJECT_TO_JSVAL( myRace );
			}
			
			break;
		case CCP_CRIMINAL:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCriminal() );	break;
		case CCP_MURDERER:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsMurderer() );	break;
		case CCP_INNOCENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsInnocent() );	break;
		case CCP_MURDERCOUNT:	*vp = INT_TO_JSVAL( gPriv->GetKills() );		break;
		case CCP_GENDER:
			if( gPriv->GetID() == 0x0190 || gPriv->GetID() == 0x0192 )
				*vp = INT_TO_JSVAL( 0 );
			else if( gPriv->GetID() == 0x0191 || gPriv->GetID() == 0x0193 )
				*vp = INT_TO_JSVAL( 1 );
			else
				*vp = INT_TO_JSVAL( 2 );
			break;
		case CCP_DEAD:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDead() );		break;
		case CCP_NPC:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsNpc() );		break;
		case CCP_ONLINE:		*vp = BOOLEAN_TO_JSVAL( isOnline( gPriv ) );	break;
		case CCP_DIRECTION:		*vp = INT_TO_JSVAL( gPriv->GetDir() );			break;
		
		// 3  objects: regions + towns + guilds
		// Goal: myChar.target.textmessage( "Your target is a member of " + myChar.guild.name );
		case CCP_REGION:		*vp = INT_TO_JSVAL( gPriv->GetRegion() );		break;
		case CCP_TOWN:
			TempTownID = gPriv->GetTown();

			// We need to decide here whether 0xFF is a valid town (wilderness) or not
			// i would say no its not
			if( TempTownID == 0xFF )
			{
				*vp = JSVAL_NULL;
			}
			else
			{
				// Should build the town here
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );
				
				JSObject *myTown = myScript->AcquireObject( IUE_REGION );
	
				JS_SetPrivate( cx, myTown, region[TempTownID] );
				*vp = OBJECT_TO_JSVAL( myTown );
			}
			break;
		case CCP_GUILD:
			TempGuildID = gPriv->GetGuildNumber();

			// Character has no guild
			if( TempGuildID == -1 ) // isn't there a constant or something like?
			{
				*vp = JSVAL_NULL;
			}
			else
			{
				// if he has one, lets build our guild !
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );
				JSObject *myGuild = myScript->AcquireObject( IUE_GUILD );
	
				JS_SetPrivate( cx, myGuild, GuildSys->Guild( TempGuildID ) );
				*vp = OBJECT_TO_JSVAL( myGuild );
			}
			break;
		case CCP_SOCKET:
			{ // So we can declar the variables here
			UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
			cScript *myScript = Trigger->GetScript( myScpTrig );
			JSObject *mySock = myScript->AcquireObject( IUE_SOCK );
			JS_SetPrivate( cx, mySock, calcSocketObjFromChar( gPriv ) );
			*vp = OBJECT_TO_JSVAL( mySock );
			}
			break;
		case CCP_XID:			*vp = INT_TO_JSVAL( gPriv->GetxID() );			break;
		case CCP_XSKIN:			*vp = INT_TO_JSVAL( gPriv->GetxSkin() );		break;
		case CCP_ISCHAR:		*vp = JSVAL_TRUE;								break;
		case CCP_ISITEM:		*vp = JSVAL_FALSE;								break;
		case CCP_RACEID:		*vp = INT_TO_JSVAL( gPriv->GetRace() );			break;
		case CCP_MAXHP:			*vp = INT_TO_JSVAL( gPriv->GetMaxHP() );		break;
		case CCP_MAXSTAMINA:	*vp = INT_TO_JSVAL( gPriv->GetMaxStam() );		break;
		case CCP_MAXMANA:		*vp = INT_TO_JSVAL( gPriv->GetMaxMana() );		break;
		case CCP_WANDERTYPE:	*vp = INT_TO_JSVAL( gPriv->GetNpcWander() );	break;
		case CCP_ISONHORSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsOnHorse() );	break;
		case CCP_TDEXTERITY:	*vp = INT_TO_JSVAL( gPriv->Dexterity2() );		break;
		case CCP_TINTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->Intelligence2() );	break;
		case CCP_TSTRENGTH:		*vp = INT_TO_JSVAL( gPriv->Strength2() );		break;
		case CCP_POISON:		*vp = INT_TO_JSVAL( gPriv->GetPoisoned() );		break;
		case CCP_LIGHTLEVEL:	*vp = INT_TO_JSVAL( gPriv->GetFixedLight() );	break;
		case CCP_ARMOUR:		*vp = INT_TO_JSVAL( gPriv->GetDef() );			break;
		case CCP_VULNERABLE:	*vp = BOOLEAN_TO_JSVAL( !gPriv->IsInvulnerable() );	break;
		case CCP_HUNGERSTATUS:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetHungerStatus() ); break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CCharacterProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CChar *gPriv = (CChar *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	
	UI16 uivalue = (UI16)JSVAL_TO_INT( *vp );
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CCP_NAME:	gPriv->SetName( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );		break;
		case CCP_TITLE:	gPriv->SetTitle( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );	break;
		case CCP_X:			
			MapRegion->RemoveChar( gPriv );
			gPriv->SetX( (SI16)JSVAL_TO_INT( *vp ) );
			MapRegion->AddChar( gPriv );
			gPriv->Teleport();
			break;
		case CCP_Y:			
			MapRegion->RemoveChar( gPriv );
			gPriv->SetY( (SI16)JSVAL_TO_INT( *vp ) );
			MapRegion->AddChar( gPriv );
			gPriv->Teleport();
			break;
		case CCP_Z:			
			gPriv->SetZ( (SI08)JSVAL_TO_INT( *vp ) );
			gPriv->Teleport();
			break;
		case CCP_ID:		
			gPriv->SetID( uivalue );
			gPriv->SetxID( uivalue );
			gPriv->SetOrgID( uivalue );
			gPriv->Teleport();	
			break;
		case CCP_COLOUR:	
			gPriv->SetColour( uivalue );
			gPriv->SetxSkin( uivalue );
			gPriv->SetOrgSkin( uivalue );
			gPriv->Teleport();	
			break;
		case CCP_OWNER:		
			if( *vp != JSVAL_NULL )
			{
				CChar *myChar = (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( *vp ) );
				if( myChar == NULL )
					break;

				gPriv->SetOwner( (cBaseObject *)myChar );
			}
			else
			{
				gPriv->SetOwner( NULL );
			}
			break;
		case CCP_VISIBLE:	gPriv->SetVisible( (SI08)JSVAL_TO_INT( *vp ) );	gPriv->Update();	break;
		case CCP_SERIAL:	
			
			break;
			// We want other players to see the change dont we
		case CCP_HEALTH:	gPriv->SetHP( (SI16)JSVAL_TO_INT( *vp ) ); gPriv->Update();			break;
		case CCP_SCRIPTTRIGGER:	gPriv->SetScriptTrigger( (UI16)JSVAL_TO_INT( *vp ) );			break;
		case CCP_PRIVATEWORD:
			
			break;
		case CCP_WORLDNUMBER:	
			gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), (UI08)JSVAL_TO_INT( *vp ) );
			gPriv->Teleport();
			break;
		case CCP_TARGET:
			//TODO: Check if the user(admin per jscript) can set the target

			break;
		case CCP_DEXTERITY:		gPriv->SetDexterity( (SI16)JSVAL_TO_INT( *vp ) );				break;
		case CCP_INTELLIGENCE:	gPriv->SetIntelligence( (SI16)JSVAL_TO_INT( *vp ) );			break;
		case CCP_STRENGTH:		gPriv->SetStrength( (SI16)JSVAL_TO_INT( *vp ) );				break;
		case CCP_SKILLS:

			break;
		case CCP_MANA:			gPriv->SetMana( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CCP_STAMINA:		gPriv->SetStamina( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CCP_CHARPACK:

			break;
		case CCP_FAME:			gPriv->SetFame( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CCP_KARMA:			gPriv->SetKarma( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CCP_HUNGER:		gPriv->SetHunger( (SI08)JSVAL_TO_INT( *vp ) );					break;
		case CCP_FROZEN:		gPriv->SetFrozen( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );			break;
		case CCP_COMMANDLEVEL:	gPriv->SetCommandLevel( (UI08)JSVAL_TO_INT( *vp ) );			break;
		case CCP_RACEID:
		case CCP_RACE:			Races->gate( gPriv, (RACEID)JSVAL_TO_INT( *vp ), true );		break;
		case CCP_CRIMINAL:		
			if( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE )
				criminal( gPriv );
			else
			{
				gPriv->SetCrimFlag( -1 );
				setcharflag( gPriv );
			}
			break;
		case CCP_INNOCENT:
			if( JSVAL_TO_BOOLEAN( *vp ) != JS_TRUE )
				criminal( gPriv );
			else
			{
				gPriv->SetCrimFlag( -1 );
				setcharflag( gPriv );
			}
			break;
		case CCP_MURDERCOUNT:	gPriv->SetKills( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CCP_GENDER:
			switch( (SI16)JSVAL_TO_INT( *vp ) )
			{
			case 0:	// male
				if( gPriv->IsDead() )
					gPriv->SetID( 0x0192 );
				else
					gPriv->SetID( 0x0190 );
				gPriv->Teleport();
				break;
			case 1:	// female
				if( gPriv->IsDead() )
					gPriv->SetID( 0x0193 );
				else
					gPriv->SetID( 0x0191 );
				gPriv->Teleport();
				break;
			default:
				break;
			}
			break;
		case CCP_NPC:			gPriv->SetNpc( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );			break;
		case CCP_DIRECTION:		gPriv->SetDir( (UI08)JSVAL_TO_INT( *vp ) );						break;
		case CCP_REGION:		gPriv->SetRegion( (UI08)JSVAL_TO_INT( *vp ) );					break;
		case CCP_TOWN:
			region[gPriv->GetTown()]->RemoveTownMember( *gPriv );
			region[JSVAL_TO_INT( *vp )]->AddAsTownMember( *gPriv );
			break;
		case CCP_XID:			gPriv->SetxID( (SI16)JSVAL_TO_INT( *vp ) );						break;
		case CCP_XSKIN:			gPriv->SetxSkin( (UI16)JSVAL_TO_INT( *vp ) );					break;
		case CCP_GUILD:
			GuildSys->Resign( calcSocketObjFromChar( gPriv ) );

			if( *vp != JSVAL_NULL )
			{
				CGuild *myGuild =(CGuild*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( *vp ) );
				myGuild->NewRecruit( gPriv->GetSerial() );
			}
			break;
		case CCP_WANDERTYPE:	gPriv->SetNpcWander( (SI08)JSVAL_TO_INT( *vp ) );				break;
		case CCP_TDEXTERITY:		gPriv->Dexterity2( (SI16)JSVAL_TO_INT( *vp ) );				break;
		case CCP_TINTELLIGENCE:	gPriv->Intelligence2( (SI16)JSVAL_TO_INT( *vp ) );			break;
		case CCP_TSTRENGTH:		gPriv->Strength2( (SI16)JSVAL_TO_INT( *vp ) );				break;
		case CCP_LIGHTLEVEL:	
			gPriv->SetFixedLight( (UI08)JSVAL_TO_INT( *vp ) );
			if( calcSocketObjFromChar( gPriv ) != NULL )
				if( (UI08)JSVAL_TO_INT( *vp ) == 255 )
					doLight( calcSocketObjFromChar( gPriv ), static_cast<char>(cwmWorldState->ServerData()->GetWorldLightCurrentLevel() ));
				else
					doLight( calcSocketObjFromChar( gPriv ), (UI08)JSVAL_TO_INT( *vp ) );
			break;
		case CCP_ARMOUR:	gPriv->SetDef( (UI16)JSVAL_TO_INT( *vp ) );						break;
		case CCP_VULNERABLE:	gPriv->SetInvulnerable( !( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE ) );	break;
		case CCP_HUNGERSTATUS:	gPriv->SetHungerStatus( ( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE ) ); break;
		}
	}
	return JS_TRUE;
}

JSBool CRegionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	cTownRegion *gPriv = (cTownRegion *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	JSString *tString = NULL;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CREGP_NAME:
			tString = JS_NewStringCopyZ( cx, gPriv->GetName() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CREGP_MAYOR:
			break;
		case CREGP_RACE:				*vp = INT_TO_JSVAL( gPriv->GetRace() );					break;
		case CREGP_TAX:					*vp = INT_TO_JSVAL( gPriv->TaxedAmount() );				break;
		case CREGP_TAXRESOURCE:			*vp = INT_TO_JSVAL( gPriv->GetResourceID() );			break;
		case CREGP_CANMARK:				*vp = BOOLEAN_TO_JSVAL( gPriv->CanMark() );				break;
		case CREGP_CANRECALL:			*vp = BOOLEAN_TO_JSVAL( gPriv->CanRecall() );			break;
		case CREGP_CANGATE:				*vp = BOOLEAN_TO_JSVAL( gPriv->CanGate() );				break;
		case CREGP_ISGUARDED:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );			break;
		case CREGP_CANCASTAGGRESSIVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanCastAggressive() );	break;
		case CREGP_HEALTH:				*vp = INT_TO_JSVAL( gPriv->GetHealth() );				break;
		case CREGP_ISDUNGEON:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDungeon() );			break;
		case CREGP_CHANCEBIGORE:		*vp = INT_TO_JSVAL( gPriv->GetChanceBigOre() );			break;
		case CREGP_CHANCECOLOURORE:		*vp = INT_TO_JSVAL( gPriv->GetChanceColourOre() );		break;
		case CREGP_NUMOREPREFERENCES:	*vp = INT_TO_JSVAL( gPriv->GetNumOrePreferences() );	break;
		case CREGP_OREPREFERENCES:
			break;
		case CREGP_POPULATION:			*vp = INT_TO_JSVAL( gPriv->GetPopulation() );			break;
		case CREGP_MEMBERS:
		default:
			break;
		}
	}
	return JS_TRUE;
}
JSBool CRegionProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	cTownRegion *gPriv = (cTownRegion *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CREGP_NAME:				gPriv->SetName( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );	break;
		case CREGP_MAYOR:
			break;
		case CREGP_RACE:				gPriv->SetRace( (RACEID)JSVAL_TO_INT( *vp ) );						break;
		case CREGP_TAX:					gPriv->TaxedAmount( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CREGP_TAXRESOURCE:			gPriv->SetResourceID( (UI16)JSVAL_TO_INT( *vp ) );					break;
		case CREGP_CANMARK:				gPriv->CanMark( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CREGP_CANRECALL:			gPriv->CanRecall( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CREGP_CANGATE:				gPriv->CanGate( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CREGP_ISGUARDED:			gPriv->IsGuarded( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CREGP_CANCASTAGGRESSIVE:	gPriv->CanCastAggressive( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );		break;
		case CREGP_HEALTH:				gPriv->SetHealth( (SI16)JSVAL_TO_INT( *vp ) );						break;
		case CREGP_ISDUNGEON:			gPriv->IsDungeon( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CREGP_CHANCEBIGORE:		gPriv->SetChanceBigOre( (UI08)JSVAL_TO_INT( *vp ) );				break;
		case CREGP_CHANCECOLOURORE:		gPriv->SetChanceColourOre( (UI08)JSVAL_TO_INT( *vp ) );				break;
		case CREGP_OREPREFERENCES:
		case CREGP_MEMBERS:
		default:
			break;
		}
	}
	return JS_TRUE;
}
JSBool CGuildProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CGuild *gPriv = (CGuild *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	JSString *tString = NULL;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CGP_NAME:
			tString = JS_NewStringCopyZ( cx, gPriv->Name() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CGP_TYPE:			*vp = INT_TO_JSVAL( gPriv->Type() );		break;
		case CGP_MASTER:
		case CGP_STONE:
			break;
		case CGP_NUMMEMBERS:	*vp = INT_TO_JSVAL( gPriv->NumMembers() );	break;
		case CGP_NUMRECRUITS:	*vp = INT_TO_JSVAL( gPriv->NumRecruits() );	break;
		case CGP_MEMBERS:
		case CGP_RECRUITS:
			break;
		case CGP_CHARTER:
			tString = JS_NewStringCopyZ( cx, gPriv->Charter() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CGP_ABBREVIATION:
			tString = JS_NewStringCopyZ( cx, gPriv->Abbreviation() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CGP_WEBPAGE:
			tString = JS_NewStringCopyZ( cx, gPriv->Webpage() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		default:
			break;
		}
	}
	return JS_TRUE;
}
JSBool CGuildProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CGuild *gPriv = (CGuild *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CGP_NAME:				gPriv->Name( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );			break;
		case CGP_TYPE:				gPriv->Type( (GuildType)JSVAL_TO_INT( *vp ) );								break;
		case CGP_MASTER:
		case CGP_STONE:
		case CGP_MEMBERS:
		case CGP_RECRUITS:
			break;
		case CGP_CHARTER:			gPriv->Charter( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );			break;
		case CGP_ABBREVIATION:		gPriv->Abbreviation( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );	break;
		case CGP_WEBPAGE:			gPriv->Webpage( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );			break;
		default:
			break;
		}
	}
	return JS_TRUE;
}
JSBool CRaceProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CRace *gPriv = (CRace *)JS_GetPrivate( cx, obj );
	UI08 TempRace;

	if( gPriv == NULL )
		return JS_FALSE;
	JSString *tString = NULL;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CRP_ID:
			for( TempRace = 0; TempRace < Races->Count(); TempRace++ )
			{
				if( Races->Race( TempRace ) == gPriv )
				{
					*vp = INT_TO_JSVAL( TempRace );
					break;
				}
			}
			break;
		case CRP_NAME:
			tString = JS_NewStringCopyZ( cx, gPriv->Name() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		case CRP_WEAKTOWEATHER:
			break;
		case CRP_REQUIRESBEARD:		*vp = BOOLEAN_TO_JSVAL( gPriv->RequiresBeard() );		break;
		case CRP_REQUIRESNOBEARD:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoBeard() );				break;
		case CRP_ISPLAYERRACE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsPlayerRace() );		break;
		case CRP_GENDERRESTRICT:	*vp = INT_TO_JSVAL( gPriv->GenderRestriction() );		break;
		case CRP_ARMOURCLASS:		*vp = INT_TO_JSVAL( gPriv->ArmourClassRestriction() );	break;
		case CRP_LANGUAGESKILLMIN:	*vp = INT_TO_JSVAL( gPriv->LanguageMin() );				break;
		case CRP_SKILLADJUSTMENT:
			break;
		case CRP_POISONRESISTANCE:	*vp = DOUBLE_TO_JSVAL( gPriv->PoisonResistance() );		break;
		case CRP_MAGICRESISTANCE:	*vp = DOUBLE_TO_JSVAL( gPriv->MagicResistance() );		break;
		case CRP_VISIBLEDISTANCE:	*vp = INT_TO_JSVAL( gPriv->VisibilityRange() );			break;
		case CRP_NIGHTVISION:		*vp = INT_TO_JSVAL( gPriv->NightVision() );				break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CRaceProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CRace *gPriv = (CRace *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CRP_NAME:	gPriv->Name( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );		break;
		case CRP_WEAKTOWEATHER:
			break;
		case CRP_REQUIRESBEARD:		gPriv->RequiresBeard( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );	break;
		case CRP_REQUIRESNOBEARD:	gPriv->NoBeard( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );		break;
		case CRP_ISPLAYERRACE:		gPriv->IsPlayerRace( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );	break;
		case CRP_GENDERRESTRICT:	gPriv->GenderRestriction( JSVAL_TO_INT( *vp ) );			break;
		case CRP_ARMOURCLASS:		gPriv->ArmourClassRestriction( (UI08)JSVAL_TO_INT( *vp ) );	break;
		case CRP_LANGUAGESKILLMIN:	gPriv->LanguageMin( (UI16)JSVAL_TO_INT( *vp ) );			break;
		case CRP_SKILLADJUSTMENT:
			break;
		case CRP_POISONRESISTANCE:	
			//	gPriv->PoisonResistance( JSVAL_TO_DOUBLE( *vp ) );			break;
			break;
		case CRP_MAGICRESISTANCE:	
			// gPriv->MagicResistance( JSVAL_TO_DOUBLE( *vp ) );			break;
			break;
		case CRP_VISIBLEDISTANCE:	gPriv->VisibilityRange( (SI08)JSVAL_TO_INT( *vp ) );		break;
		case CRP_NIGHTVISION:		gPriv->NightVision( (UI08)JSVAL_TO_INT( *vp ) );			break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CItemProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CItem *gPriv = (CItem *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CIP_NAME:	gPriv->SetName( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );		break;
		case CIP_X:			
			MapRegion->RemoveItem( gPriv );
			gPriv->SetX( (SI16)JSVAL_TO_INT( *vp ) );
			MapRegion->AddItem( gPriv );
			RefreshItem( gPriv );
			break;
		case CIP_Y:			
			MapRegion->RemoveItem( gPriv );
			gPriv->SetY( (SI16)JSVAL_TO_INT( *vp ) );
			MapRegion->AddItem( gPriv );
			RefreshItem( gPriv );
			break;
		case CIP_Z:			
			gPriv->SetZ( (SI08)JSVAL_TO_INT( *vp ) );
			RefreshItem( gPriv );
			break;
		case CIP_ID:		gPriv->SetID( (UI16)JSVAL_TO_INT( *vp ) );		RefreshItem( gPriv );	break;
		case CIP_COLOUR:	gPriv->SetColour( (UI16)JSVAL_TO_INT( *vp ) );	RefreshItem( gPriv );	break;
		case CIP_OWNER:		
			if( *vp != JSVAL_NULL ) 
			{	 
				CChar *myChar = (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( *vp ) ); 
				if( myChar == NULL ) 
					break; 
				gPriv->SetOwner( (cBaseObject *)myChar ); 
			}
			else
			{
				gPriv->SetOwner( (cBaseObject *)NULL );
			}
			break;
		case CIP_VISIBLE:	gPriv->SetVisible( (SI08)JSVAL_TO_INT( *vp ) );	RefreshItem( gPriv );	break;
		case CIP_SERIAL:	
			
			break;
		case CIP_HEALTH:	gPriv->SetHP( (SI16)JSVAL_TO_INT( *vp ) ); 								break;
		case CIP_SCRIPTTRIGGER:	gPriv->SetScriptTrigger( (UI16)JSVAL_TO_INT( *vp ) );				break;
		case CIP_PRIVATEWORD:
			
			break;
		case CIP_WORLDNUMBER:	
			gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), (UI08)JSVAL_TO_INT( *vp ) );
			RefreshItem( gPriv );
			break;
		case CIP_AMOUNT:	gPriv->SetAmount( (UI32)JSVAL_TO_INT( *vp ) ); 	RefreshItem( gPriv );		break;
		case CIP_CONTAINER:
			break;
		case CIP_TYPE:		gPriv->SetType( (UI08)JSVAL_TO_INT( *vp ) ); 							break;
		case CIP_MORE:		gPriv->SetMore( JSVAL_TO_INT( *vp ) ); 									break;
		case CIP_MOREX:		gPriv->SetMoreX( JSVAL_TO_INT( *vp ) );									break;
		case CIP_MOREY:		gPriv->SetMoreY( JSVAL_TO_INT( *vp ) );									break;
		case CIP_MOREZ:		gPriv->SetMoreZ( JSVAL_TO_INT( *vp ) );									break;
		case CIP_MOVABLE:	gPriv->SetMovable( (SI08)JSVAL_TO_INT( *vp ) ); 						break;
		case CIP_ATT:		gPriv->SetLoDamage( (SI16)JSVAL_TO_INT( *vp ) ); 	
							gPriv->SetHiDamage( (SI16)JSVAL_TO_INT( *vp ) ); 	
							break;
		case CIP_DEF:		gPriv->SetDef( (UI16)JSVAL_TO_INT( *vp ) ); 							break;
		case CIP_LAYER:		gPriv->SetLayer( (SI08)JSVAL_TO_INT( *vp ) ); 							break;
		case CIP_ITEMSINSIDE:
			break;
		case CIP_DECAYABLE:	gPriv->SetDecayable( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE ); 				break;
		case CIP_DECAYTIME:	gPriv->SetDecayTime( JSVAL_TO_INT( *vp ) ); 							break;
		case CIP_LODAMAGE:	gPriv->SetLoDamage( (SI16)JSVAL_TO_INT( *vp ) );						break;
		case CIP_HIDAMAGE:	gPriv->SetHiDamage( (SI16)JSVAL_TO_INT( *vp ) );						break;
		case CIP_NAME2:		gPriv->SetName2( JS_GetStringBytes( JS_ValueToString( cx, *vp ) ) );	break;
		case CIP_RACEID:
		case CIP_RACE:		gPriv->SetRace( (RACEID)JSVAL_TO_INT( *vp ) );							break;
		case CIP_MAXHP:		gPriv->SetMaxHP( (SI16)JSVAL_TO_INT( *vp ) );							break;
		case CIP_RANK:		gPriv->SetRank( (SI08)JSVAL_TO_INT( *vp ) );							break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CSocketProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	cSocket *gPriv = (cSocket *)JS_GetPrivate( cx, obj );
	if( gPriv == NULL )
		return JS_FALSE;
	UI16 myScpTrig = 0xFFFF;
	cScript *myScript = NULL;
	JSObject *myObj = NULL;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CSOCKP_ACCOUNT:
			break;
		case CSOCKP_CURRENTCHAR:
			if( gPriv->CurrcharObj() == NULL )
			{
				*vp = JSVAL_NULL;
			}
			else
			{
				myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				myScript = Trigger->GetScript( myScpTrig );

				myObj = myScript->AcquireObject( IUE_CHAR );
				JS_SetPrivate( cx, myObj, gPriv->CurrcharObj() );
				*vp = OBJECT_TO_JSVAL( myObj );
			}
			break;
		case CSOCKP_IDLETIMEOUT:
			break;
		case CSOCKP_WASIDLEWARNED:		gPriv->WasIdleWarned( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );			break;
		case CSOCKP_TEMPINT:
		case CSOCKP_BUFFER:
		case CSOCKP_XTEXT:
		case CSOCKP_ADDX:
		case CSOCKP_ADDY:
			break;
		case CSOCKP_ADDZ:				gPriv->AddZ( (SI08)JSVAL_TO_INT( *vp ) );							break;
		case CSOCKP_ADDID:
		case CSOCKP_ADDMITEM:
			break;
		case CSOCKP_NEWCLIENT:			gPriv->NewClient( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CSOCKP_FIRSTPACKET:		gPriv->FirstPacket( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );			break;
		case CSOCKP_CRYPTCLIENT:		gPriv->CryptClient( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );			break;
		case CSOCKP_CLIENTIP:
			break;
		case CSOCKP_WALKSEQUENCE:		gPriv->WalkSequence( (SI16)JSVAL_TO_INT( *vp ) );					break;
		case CSOCKP_CURRENTSPELLTYPE:	gPriv->CurrentSpellType( (SI08)JSVAL_TO_INT( *vp ) );				break;
			break;
		case CSOCKP_LOGGING:			gPriv->Logging( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CSOCKP_BYTESSENT:
		case CSOCKP_BYTESRECEIVED:
			break;
		case CSOCKP_TARGETOK:			gPriv->TargetOK( JSVAL_TO_BOOLEAN( *vp ) == JS_TRUE );				break;
		case CSOCKP_CLICKX:				gPriv->ClickX( (SI16)JSVAL_TO_INT( *vp ) );							break;
		case CSOCKP_CLICKY:				gPriv->ClickY( (SI16)JSVAL_TO_INT( *vp ) );							break;
		case CSOCKP_PICKUPX:			gPriv->PickupX( (SI16)JSVAL_TO_INT( *vp ) );						break;
		case CSOCKP_PICKUPY:			gPriv->PickupY( (SI16)JSVAL_TO_INT( *vp ) );						break;
		case CSOCKP_PICKUPZ:			gPriv->PickupZ( (SI08)JSVAL_TO_INT( *vp ) );						break;
		case CSOCKP_PICKUPSPOT:			gPriv->PickupSpot( (PickupLocations)JSVAL_TO_INT( *vp ) );			break;
		case CSOCKP_PICKUPSERIAL:
			break;
		case CSOCKP_LANGUAGE:			gPriv->Language( (UnicodeTypes)JSVAL_TO_INT( *vp ) );				break;
		case CSOCKP_CLIENTMAJORVER:		gPriv->ClientVersionMajor( (UI08)JSVAL_TO_INT( *vp ) );				break;
		case CSOCKP_CLIENTMINORVER:		gPriv->ClientVersionMinor( (UI08)JSVAL_TO_INT( *vp ) );				break;
		case CSOCKP_CLIENTSUBVER:		gPriv->ClientVersionSub( (UI08)JSVAL_TO_INT( *vp ) );				break;
		case CSOCKP_CLIENTLETTERVER:	gPriv->ClientVersionLetter( (UI08)JSVAL_TO_INT( *vp ) );			break;
		case CSOCKP_CLIENTTYPE:			gPriv->ClientType( (ClientTypes)JSVAL_TO_INT( *vp ) );				break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CSocketProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	cSocket *gPriv = (cSocket *)JS_GetPrivate( cx, obj );
	CChar *myChar;

	if( gPriv == NULL )
		return JS_FALSE;
	if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
		case CSOCKP_ACCOUNT:
		case CSOCKP_CURRENTCHAR:
			myChar = gPriv->CurrcharObj();
			if( myChar == NULL )
			{
				*vp = JSVAL_NULL;
			}
			else
			{
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				JSObject *myObj = myScript->AcquireObject( IUE_CHAR );
				JS_SetPrivate( cx, myObj, gPriv->CurrcharObj() );
				*vp = OBJECT_TO_JSVAL( myObj );
			}

		case CSOCKP_IDLETIMEOUT:
			break;
		case CSOCKP_WASIDLEWARNED:		*vp = BOOLEAN_TO_JSVAL( gPriv->WasIdleWarned() );		break;
		case CSOCKP_TEMPINT:
		case CSOCKP_BUFFER:
		case CSOCKP_XTEXT:
		case CSOCKP_ADDX:
		case CSOCKP_ADDY:
			break;
		case CSOCKP_ADDZ:				*vp = INT_TO_JSVAL( gPriv->AddZ() );					break;
		case CSOCKP_ADDID:
		case CSOCKP_ADDMITEM:
			break;
		case CSOCKP_NEWCLIENT:			*vp = BOOLEAN_TO_JSVAL( gPriv->NewClient() );			break;
		case CSOCKP_FIRSTPACKET:		*vp = BOOLEAN_TO_JSVAL( gPriv->FirstPacket() );			break;
		case CSOCKP_CRYPTCLIENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->CryptClient() );			break;
		case CSOCKP_CLIENTIP:
			break;
		case CSOCKP_WALKSEQUENCE:		*vp = INT_TO_JSVAL( gPriv->WalkSequence() );			break;
		case CSOCKP_CURRENTSPELLTYPE:	*vp = INT_TO_JSVAL( gPriv->CurrentSpellType() );		break;
		case CSOCKP_LOGGING:			*vp = BOOLEAN_TO_JSVAL( gPriv->Logging() );				break;
		case CSOCKP_BYTESSENT:
		case CSOCKP_BYTESRECEIVED:
			break;
		case CSOCKP_TARGETOK:			*vp = BOOLEAN_TO_JSVAL( gPriv->TargetOK() );			break;
		case CSOCKP_CLICKX:				*vp = INT_TO_JSVAL( gPriv->ClickX() );					break;
		case CSOCKP_CLICKY:				*vp = INT_TO_JSVAL( gPriv->ClickY() );					break;
		case CSOCKP_PICKUPX:			*vp = INT_TO_JSVAL( gPriv->PickupX() );					break;
		case CSOCKP_PICKUPY:			*vp = INT_TO_JSVAL( gPriv->PickupY() );					break;
		case CSOCKP_PICKUPZ:			*vp = INT_TO_JSVAL( gPriv->PickupZ() );					break;
		case CSOCKP_PICKUPSPOT:			*vp = INT_TO_JSVAL( gPriv->PickupSpot() );				break;
		case CSOCKP_PICKUPSERIAL:
			break;
		case CSOCKP_LANGUAGE:			*vp = INT_TO_JSVAL( gPriv->Language() );				break;
		case CSOCKP_CLIENTMAJORVER:		*vp = INT_TO_JSVAL( gPriv->ClientVersionMajor() );		break;
		case CSOCKP_CLIENTMINORVER:		*vp = INT_TO_JSVAL( gPriv->ClientVersionMinor() );		break;
		case CSOCKP_CLIENTSUBVER:		*vp = INT_TO_JSVAL( gPriv->ClientVersionSub() );		break;
		case CSOCKP_CLIENTLETTERVER:	*vp = INT_TO_JSVAL( gPriv->ClientVersionLetter() );		break;
		case CSOCKP_CLIENTTYPE:			*vp = INT_TO_JSVAL( gPriv->ClientType() );				break;
		case CSOCKP_TARGET:
			{
				SERIAL mySerial = gPriv->GetDWord( 7 );
				UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
				cScript *myScript = Trigger->GetScript( myScpTrig );

				// Item
				if( mySerial >= BASEITEMSERIAL )
				{
					CItem *myItem = calcItemObjFromSer( mySerial );

					if( myItem == NULL )
					{
						*vp = JSVAL_NULL;
						return JS_TRUE;
					}

					JSObject *myObj = myScript->AcquireObject( IUE_ITEM );
					JS_SetPrivate( cx, myObj,  myItem);
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				// Char
				else
				{
					CChar *myChar = calcCharObjFromSer( mySerial );

					if( myChar == NULL )
					{
						*vp = JSVAL_NULL;
						return JS_TRUE;
					}

					JSObject *myObj = myScript->AcquireObject( IUE_CHAR );
					JS_SetPrivate( cx, myObj,  myChar);
					*vp = OBJECT_TO_JSVAL( myObj );
				}

				return JS_TRUE;
			}
			break;
		default:
			break;
		}
	}
	return JS_TRUE;
}

JSBool CItemsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	// Create a new JS Object representing the accquired object...
	// IF SOMEONE IS ENUMERATING THOSE OBJECTS THIS COULD CAUSE OUT_OF_MEMORY !
	CItem *myItem = (CItem*)JS_GetPrivate( cx, JS_GetParent( cx, obj ) );

	if( myItem == NULL )
		return JS_FALSE;
	
	UI32 Index = JSVAL_TO_INT( id );
	CItem *mySubItem = myItem->GetItemObj( Index );
	if( mySubItem == NULL )
	{
		*vp = JSVAL_NULL;
		return JS_TRUE;
	}
	
	// Otherwise Acquire an object
	UI16 myScpTrig = Trigger->FindObject( JS_GetGlobalObject( cx ) );
	cScript *myScript = Trigger->GetScript( myScpTrig );

	JSObject *myJSItem = myScript->AcquireObject( IUE_ITEM );
	JS_SetPrivate( cx, myJSItem, mySubItem );
	*vp = OBJECT_TO_JSVAL( myJSItem );
	
	return JS_TRUE;
}

JSBool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( myChar == NULL )
		return JS_FALSE;
	
	UI08 SkillID = (UI08)JSVAL_TO_INT( id );

	JSClass *myClass = JS_GetClass( obj );

	SI16 SkillValue = JSVAL_NULL;

	if( !strcmp( myClass->name, "UOXSkills" ) )
		SkillValue = myChar->GetSkill( SkillID );
	else if( !strcmp( myClass->name, "UOXBaseSkills" ) )
		SkillValue = myChar->GetBaseSkill( SkillID );

	*vp = INT_TO_JSVAL( SkillValue );

	return JS_TRUE;
}

JSBool CSkillsProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( myChar == NULL )
		return JS_FALSE;
	
	UI08 SkillID = (UI08)JSVAL_TO_INT( id );

	JSClass *myClass = JS_GetClass( obj );	
	SI16 NewSkillValue = (SI16)JSVAL_TO_INT( *vp );

	if( !strcmp( myClass->name, "UOXSkills" ) )
		myChar->SetSkill( NewSkillValue, SkillID );
	else if( !strcmp( myClass->name, "UOXBaseSkills" ) )
	{
		myChar->SetBaseSkill( NewSkillValue, SkillID );
		Skills->updateSkillLevel( myChar, SkillID );
	}

	if( !myChar->IsNpc() )
	{
		cSocket *toFind = calcSocketObjFromChar( myChar );
		if( toFind != NULL )
			updateskill( toFind, SkillID );
	}
	return JS_TRUE;
}

JSBool CGumpDataProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	SEGumpData *gPriv = (SEGumpData *)JS_GetPrivate( cx, obj );
	//SEGump *toDelete = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gPriv == NULL )
		return JS_FALSE;
	
	//if( JSVAL_IS_INT( id ) ) 
	{
		switch( JSVAL_TO_INT( id ) )
		{
			case CGumpData_ID:
				*vp = INT_TO_JSVAL( gPriv->nIDs.size()) ;
				break ;
			case CGumpData_Button:
				*vp = INT_TO_JSVAL( gPriv->nButtons.size()) ;
				break ;
			default:
				break;
		}
	}
	return JS_TRUE;
}
