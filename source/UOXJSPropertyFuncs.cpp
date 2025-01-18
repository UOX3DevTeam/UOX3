//o------------------------------------------------------------------------------------------------o
//|	File		-	UOXJSPropertyFuncts.cpp
//|	Date		-	12/14/2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	1.0		14th December, 2001 Initial implementation
//|							Includes property getters for CItem and CChar, and property
//|							setters for CChar
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"
#include "UOXJSPropertyEnums.h"
#include "UOXJSPropertyFuncs.h"
#include "JSEncapsulate.h"
#include "CJSEngine.h"

#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "regions.h"
#include "magic.h"
#include "cMagic.h"
#include "scriptc.h"
#include "ssection.h"
#include "classes.h"
#include "movement.h"
#include "Dictionary.h"
#include "cSpawnRegion.h"

#include "jsobj.h"
#include "jsutil.h"

#include "PartySystem.h"

void MakeShop( CChar *c );
void ScriptError( JSContext *cx, const char *txt, ... );

JSBool CGuildsProps_getProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] jsval id, jsval *vp )
{
	*vp = INT_TO_JSVAL( 0 );
	return JS_TRUE;
}

JSBool CGuildsProps_setProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] jsval id, [[maybe_unused]] jsval *vp )
{
	return JS_TRUE;
}

JSBool CSpellsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	size_t spellId = JSVAL_TO_INT( id );

	if( spellId >= Magic->spells.size() )
	{
		ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell ID (%i) provided", spellId ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	CSpellInfo *mySpell = &Magic->spells[spellId];
	if( mySpell == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "Spells: Invalid Spell with spellId %i", spellId ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsSpell = JS_NewObject( cx, &UOXSpell_class, nullptr, obj );
	JS_DefineProperties( cx, jsSpell, CSpellProperties );
	JS_SetPrivate( cx, jsSpell, mySpell );

	*vp = OBJECT_TO_JSVAL( jsSpell );
	return JS_TRUE;
}

JSBool CSpellProps_setProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] jsval id, [[maybe_unused]] jsval *vp )
{
	return JS_TRUE;
}

JSBool CSpellProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CSpellInfo *gPriv = static_cast<CSpellInfo*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	std::string spellName = "";

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		bool bDone = false;
		size_t i = 0;
		switch( JSVAL_TO_INT( id ))
		{
			case CSP_ID:
				for( i = 0; i < Magic->spells.size() && !bDone; ++i )
				{
					if( &Magic->spells[i] == gPriv )
					{
						*vp = INT_TO_JSVAL( i );
						bDone = true;
					}
				}
				break;
			case CSP_ACTION:			*vp = INT_TO_JSVAL( gPriv->Action() );					break;
			case CSP_BASEDMG:			*vp = INT_TO_JSVAL( gPriv->BaseDmg() );					break;
			case CSP_DELAY:				JS_NewNumberValue( cx, gPriv->Delay(), vp );			break;
			case CSP_DAMAGEDELAY:		JS_NewNumberValue( cx, gPriv->DamageDelay(), vp );		break;
			case CSP_RECOVERYDELAY:		JS_NewNumberValue( cx, gPriv->RecoveryDelay(), vp );	break;
			case CSP_HEALTH:			*vp = INT_TO_JSVAL( gPriv->Health() );					break;
			case CSP_STAMINA:			*vp = INT_TO_JSVAL( gPriv->Stamina() );					break;
			case CSP_MANA:				*vp = INT_TO_JSVAL( gPriv->Mana() );					break;
			case CSP_MANTRA:			tString = JS_NewStringCopyZ( cx, gPriv->Mantra().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CSP_NAME:
				for( i = 0; i < Magic->spells.size() && !bDone; ++i )
				{
					if( &Magic->spells[i] == gPriv - 1  )
					{
						spellName = Dictionary->GetEntry( magic_table[i].spell_name );
						tString = JS_NewStringCopyZ( cx, spellName.c_str() );
						*vp = STRING_TO_JSVAL( tString );
						bDone = true;
					}
				}
				break;
			case CSP_STRTOSAY:
				tString = JS_NewStringCopyZ( cx, gPriv->StringToSay().c_str() );
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
			case CSP_AGGRESSIVESPELL:	*vp = BOOLEAN_TO_JSVAL( gPriv->AggressiveSpell() );		break;
			case CSP_RESISTABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->Resistable() );			break;
			case CSP_SOUNDEFFECT:		*vp = INT_TO_JSVAL( gPriv->Effect() );					break;
			case CSP_ENABLED:			*vp = BOOLEAN_TO_JSVAL( gPriv->Enabled() );				break;
			default:																			break;
		}
	}
	return JS_TRUE;
}

JSBool CGlobalSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	size_t skillId = JSVAL_TO_INT( id );

	if( skillId > THROWING )
	{
		ScriptError( cx, oldstrutil::format( "Invalid Skill ID, must be between 0 and 57" ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	CWorldMain::Skill_st *mySkill = &cwmWorldState->skill[skillId];
	if( mySkill == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "Invalid Skill" ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsSkill = JS_NewObject( cx, &UOXGlobalSkill_class, nullptr, obj );
	JS_DefineProperties( cx, jsSkill, CGlobalSkillProperties );
	JS_SetPrivate( cx, jsSkill, mySkill );

	*vp = OBJECT_TO_JSVAL( jsSkill );
	return JS_TRUE;
}

JSBool CGlobalSkillProps_setProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] jsval id, [[maybe_unused]] jsval *vp )
{
	return JS_TRUE;
}

JSBool CGlobalSkillProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CWorldMain::Skill_st *gPriv = static_cast<CWorldMain::Skill_st*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	std::string skillName = "";

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CGSKILL_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->name.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGSKILL_MADEWORD:
				tString = JS_NewStringCopyZ( cx, gPriv->madeWord.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGSKILL_STRENGTH:		*vp = INT_TO_JSVAL( gPriv->strength );		break;
			case CGSKILL_DEXTERITY:		*vp = INT_TO_JSVAL( gPriv->dexterity );		break;
			case CGSKILL_INTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->intelligence );	break;
			case CGSKILL_SKILLDELAY:	*vp = INT_TO_JSVAL( gPriv->skillDelay );	break;
			case CGSKILL_SCRIPTID:		*vp = INT_TO_JSVAL( gPriv->jsScript );		break;
			default:																break;
		}
	}
	return JS_TRUE;
}

JSBool CTimerProps_getProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, jsval id, jsval *vp )
{
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			// Character timers (PC and NPC)
			case TIMER_TIMEOUT:			*vp = INT_TO_JSVAL( tCHAR_TIMEOUT );		break;
			case TIMER_INVIS:			*vp = INT_TO_JSVAL( tCHAR_INVIS );			break;
			case TIMER_HUNGER:			*vp = INT_TO_JSVAL( tCHAR_HUNGER );			break;
			case TIMER_THIRST:			*vp = INT_TO_JSVAL( tCHAR_THIRST );			break;
			case TIMER_POISONTIME:		*vp = INT_TO_JSVAL( tCHAR_POISONTIME );		break;
			case TIMER_POISONTEXT:		*vp = INT_TO_JSVAL( tCHAR_POISONTEXT );		break;
			case TIMER_POISONWEAROFF:	*vp = INT_TO_JSVAL( tCHAR_POISONWEAROFF );	break;
			case TIMER_SPELLTIME:		*vp = INT_TO_JSVAL( tCHAR_SPELLTIME );		break;
			case TIMER_SPELLRECOVERYTIME:		*vp = INT_TO_JSVAL( tCHAR_SPELLRECOVERYTIME );		break;
			case TIMER_ANTISPAM:		*vp = INT_TO_JSVAL( tCHAR_ANTISPAM );		break;
			case TIMER_CRIMFLAG:		*vp = INT_TO_JSVAL( tCHAR_CRIMFLAG );		break;
			case TIMER_MURDERRATE:		*vp = INT_TO_JSVAL( tCHAR_MURDERRATE );		break;
			case TIMER_PEACETIMER:		*vp = INT_TO_JSVAL( tCHAR_PEACETIMER );		break;
			case TIMER_FLYINGTOGGLE:	*vp = INT_TO_JSVAL( tCHAR_FLYINGTOGGLE );	break;
			case TIMER_FIREFIELDTICK:	*vp = INT_TO_JSVAL( tCHAR_FIREFIELDTICK );	break;
			case TIMER_POISONFIELDTICK:	*vp = INT_TO_JSVAL( tCHAR_POISONFIELDTICK );break;
			case TIMER_PARAFIELDTICK:	*vp = INT_TO_JSVAL( tCHAR_PARAFIELDTICK );	break;
			case TIMER_YOUNGHEAL:		*vp = INT_TO_JSVAL( tCHAR_YOUNGHEAL );		break;
			case TIMER_YOUNGMESSAGE:	*vp = INT_TO_JSVAL( tCHAR_YOUNGMESSAGE );	break;
			case TIMER_MOVETIME:		*vp = INT_TO_JSVAL( tNPC_MOVETIME );		break;
			case TIMER_SPATIMER:		*vp = INT_TO_JSVAL( tNPC_SPATIMER );		break;
			case TIMER_SUMMONTIME:		*vp = INT_TO_JSVAL( tNPC_SUMMONTIME );		break;
			case TIMER_EVADETIME:		*vp = INT_TO_JSVAL( tNPC_EVADETIME );		break;
			case TIMER_LOYALTYTIME:		*vp = INT_TO_JSVAL( tNPC_LOYALTYTIME );		break;
			case TIMER_IDLEANIMTIME:	*vp = INT_TO_JSVAL( tNPC_IDLEANIMTIME );	break;
			case TIMER_LOGOUT:			*vp = INT_TO_JSVAL( tPC_LOGOUT );			break;

			// Socket Timers (PC only)
			case TIMER_SOCK_SKILLDELAY:		*vp = INT_TO_JSVAL( tPC_SKILLDELAY );		break;
			case TIMER_SOCK_OBJDELAY:		*vp = INT_TO_JSVAL( tPC_OBJDELAY );			break;
			case TIMER_SOCK_SPIRITSPEAK:	*vp = INT_TO_JSVAL( tPC_SPIRITSPEAK );		break;
			case TIMER_SOCK_TRACKING:		*vp = INT_TO_JSVAL( tPC_TRACKING );			break;
			case TIMER_SOCK_FISHING:		*vp = INT_TO_JSVAL( tPC_FISHING );			break;
			case TIMER_SOCK_MUTETIME:		*vp = INT_TO_JSVAL( tPC_MUTETIME );			break;
			case TIMER_SOCK_TRACKINGDISPLAY: *vp = INT_TO_JSVAL( tPC_TRACKINGDISPLAY );	break;
			case TIMER_SOCK_TRAFFICWARDEN: *vp = INT_TO_JSVAL( tPC_TRAFFICWARDEN );		break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CCreateEntriesProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	UI16 createEntryId = static_cast<UI16>( JSVAL_TO_INT( id ));

	CreateEntry_st *myCreateEntry = Skills->FindItem( createEntryId );
	if( myCreateEntry == nullptr )
	{
		ScriptError( cx, oldstrutil::format( "Invalid create entry ID (%i)", createEntryId ).c_str() );
		*vp = JSVAL_NULL;
		return JS_FALSE;
	}

	JSObject *jsCreateEntry = JS_NewObject( cx, &UOXCreateEntry_class, nullptr, obj );
	JS_DefineProperties( cx, jsCreateEntry, CCreateEntryProperties );
	JS_SetPrivate( cx, jsCreateEntry, myCreateEntry );

	*vp = OBJECT_TO_JSVAL( jsCreateEntry );
	return JS_TRUE;
}

JSBool CCreateEntryProps_setProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] jsval id, [[maybe_unused]] jsval *vp )
{
	return JS_TRUE;
}

JSBool CCreateEntryProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CreateEntry_st *gPriv = static_cast<CreateEntry_st*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
		case CMAKE_ID:
			*vp = INT_TO_JSVAL( gPriv->targId );
			break;
		case CMAKE_NAME:
		{
			std::string createName = gPriv->name;
			tString = JS_NewStringCopyZ( cx, createName.c_str() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		}
		case CMAKE_ADDITEM:
		{
			std::string addItem = gPriv->addItem;
			tString = JS_NewStringCopyZ( cx, addItem.c_str() );
			*vp = STRING_TO_JSVAL( tString );
			break;
		}
		case CMAKE_COLOUR:			*vp = INT_TO_JSVAL( gPriv->colour );				break;
		case CMAKE_SOUND:			*vp = INT_TO_JSVAL( gPriv->soundPlayed );			break;
		case CMAKE_MINRANK:			*vp = INT_TO_JSVAL( gPriv->minRank );				break;
		case CMAKE_MAXRANK:			*vp = INT_TO_JSVAL( gPriv->maxRank );				break;
		case CMAKE_DELAY:			*vp = INT_TO_JSVAL( gPriv->delay );					break;
		case CMAKE_SPELL:			*vp = INT_TO_JSVAL( gPriv->spell );					break;
		case CMAKE_RESOURCES:
		{
			ResAmountPair_st resEntry;
			std::vector<ResAmountPair_st> resourcesNeeded = gPriv->resourceNeeded;

			// Loop through each resource required, and add them to a JSObject
			jsval amountNeeded = 0;
			jsval targColour = 0;
			JSObject *resources = JS_NewArrayObject( cx, 0, nullptr );
			for( auto i = 0; i < static_cast<int>( resourcesNeeded.size() ); i++ )
			{
				resEntry = resourcesNeeded[i];
				amountNeeded = INT_TO_JSVAL( resEntry.amountNeeded );
				targColour = INT_TO_JSVAL( resEntry.colour );

				// Add each required resource as its own object
				JSObject *resource = JS_NewArrayObject( cx, 0, nullptr );
				JS_SetElement( cx, resource, 0, &amountNeeded );
				JS_SetElement( cx, resource, 1, &targColour );

				// Make a child object with all the supported resource IDs
				JSObject *idList = JS_NewArrayObject( cx, 0, nullptr );
				for( auto j = 0; j < static_cast<int>( resEntry.idList.size() ); j++ )
				{
					jsval targId = INT_TO_JSVAL( resEntry.idList[j] );
					JS_SetElement( cx, idList, j, &targId );
				}
				jsval idListVal = OBJECT_TO_JSVAL( idList );
				JS_SetElement( cx, resource, 2, &idListVal );

				// Now stuff the resource object into the parent object
				jsval resourceVal = OBJECT_TO_JSVAL( resource );
				JS_SetElement( cx, resources, i, &resourceVal );
			}

			*vp = OBJECT_TO_JSVAL( resources );
			break;
		}
		case CMAKE_SKILLS:
		{
			ResSkillReq_st resEntry;
			std::vector<ResSkillReq_st> skillReqs = gPriv->skillReqs;

			// Loop through each skill required, and add the details to a JSObject
			jsval skillNumber = 0;
			jsval minSkill = 0;
			jsval maxSkill = 0;
			JSObject *skills = JS_NewArrayObject( cx, 0, nullptr );
			for( auto i = 0; i < static_cast<int>( skillReqs.size() ); i++ )
			{
				resEntry = skillReqs[i];
				skillNumber = INT_TO_JSVAL( resEntry.skillNumber );
				minSkill = INT_TO_JSVAL( resEntry.minSkill );
				maxSkill = INT_TO_JSVAL( resEntry.maxSkill );

				// Add each required skill as its own object
				JSObject *skill = JS_NewArrayObject( cx, 0, nullptr );
				JS_SetElement( cx, skill, 0, &skillNumber );
				JS_SetElement( cx, skill, 1, &minSkill );
				JS_SetElement( cx, skill, 2, &maxSkill );

				// Now stuff the skill object into the parent object
				jsval skillVal = OBJECT_TO_JSVAL( skill );
				JS_SetElement( cx, skills, i, &skillVal );
			}

			*vp = OBJECT_TO_JSVAL( skills );
			break;
		}
		case CMAKE_AVGMINSKILL:		*vp = INT_TO_JSVAL( gPriv->AverageMinSkill() );		break;
		case CMAKE_AVGMAXSKILL:		*vp = INT_TO_JSVAL( gPriv->AverageMaxSkill() );		break;
		}
	}
	return JS_TRUE;
}

JSBool CItemProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CItem *gPriv = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	SERIAL TempSerial = INVALIDSERIAL;

	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CIP_SECTIONID:
				tString = JS_NewStringCopyZ( cx, gPriv->GetSectionId().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_TITLE:
				tString = JS_NewStringCopyZ( cx, gPriv->GetTitle().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_X:			*vp = INT_TO_JSVAL( gPriv->GetX() );			break;
			case CIP_Y:			*vp = INT_TO_JSVAL( gPriv->GetY() );			break;
			case CIP_Z:			*vp = INT_TO_JSVAL( gPriv->GetZ() );			break;
			case CIP_OLDX:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.x );
				break;
			}
			case CIP_OLDY:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.y );
				break;
			}
			case CIP_OLDZ:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.z );
				break;
			}
			case CIP_ID:		*vp = INT_TO_JSVAL( gPriv->GetId() );			break;
			case CIP_COLOUR:	*vp = INT_TO_JSVAL( gPriv->GetColour() );		break;
			case CIP_OWNER:
				CChar *pOwner;
				pOwner = gPriv->GetOwnerObj();

				if( !ValidateObject( pOwner ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, pOwner, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			case CIP_VISIBLE:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetVisible() ));	break;
			case CIP_SERIAL:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetSerial() ))
				{
					JS_NewNumberValue( cx, gPriv->GetSerial(), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetSerial() );
				}
				break;
			}
			case CIP_HEALTH:		*vp = INT_TO_JSVAL( gPriv->GetHP() );				break;
			case CIP_SCRIPTTRIGGER:
			{
				// For backwards compatibility, get last scripttrigger from vector
				// For older worldfiles, this will be the only scripttrigger added to the vector after load
				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				UI16 lastScriptTrigger = 0;
				auto numberOfTriggers = scriptTriggers.size();
				if( numberOfTriggers > 0 )
				{
					lastScriptTrigger = scriptTriggers[numberOfTriggers-1];
				}

				*vp = INT_TO_JSVAL( lastScriptTrigger );
				break;
			}
			case CIP_SCRIPTTRIGGERS:
			{
				jsval scriptId;
				JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
				{
					scriptId = INT_TO_JSVAL( scriptTriggers[i] );
					JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
				}

				*vp = OBJECT_TO_JSVAL( scriptTriggersJS );
				break;
			}
			case CIP_WORLDNUMBER:	*vp = INT_TO_JSVAL( gPriv->WorldNumber() );			break;
			case CIP_INSTANCEID:	*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );		break;
			case CIP_AMOUNT:		*vp = INT_TO_JSVAL( gPriv->GetAmount() );			break;

				// We should Accquie an object here and return that, so you can do
				// item.container.name
			case CIP_CONTAINER:
				TempSerial = gPriv->GetContSerial();

				if( TempSerial == INVALIDSERIAL )
				{
					// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					if( TempSerial >= BASEITEMSERIAL )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
					{
						CItem *myCont = CalcItemObjFromSer( TempSerial );
						if( ValidateObject( myCont ))
						{
							JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, myCont, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
							*vp = OBJECT_TO_JSVAL( myItem );
						}
						else
						{
							*vp = JSVAL_NULL;
						}
					}
					else
					{
						CChar *chCont = CalcCharObjFromSer( TempSerial );
						if( ValidateObject( chCont ))
						{
							JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, chCont, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
							*vp = OBJECT_TO_JSVAL( myChar );
						}
						else
						{
							*vp = JSVAL_NULL;
						}
					}
				}
				break;
			case CIP_TYPE:			*vp = INT_TO_JSVAL( static_cast<UI16>( gPriv->GetType() ));		break;
			case CIP_MORE:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MORE )))
				{
					 JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE ));
				}
				break;
			}
			case CIP_MORE0:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MORE0 )))
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE0 ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE0 ));
				}
				break;
			}
			case CIP_MORE1:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MORE1 )))
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE1 ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE1 ));
				}
				break;
			}
			case CIP_MORE2:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MORE2 )))
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MORE2 ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MORE2 ));
				}
				break;
			}
			case CIP_MOREX:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MOREX )))
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MOREX ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MOREX ));
				}
				break;
			}
			case CIP_MOREY:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MOREY )))
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MOREY ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MOREY ));
				}
				break;
			}
			case CIP_MOREZ:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetTempVar( CITV_MOREZ )))
				{
					JS_NewNumberValue( cx, gPriv->GetTempVar( CITV_MOREZ ), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetTempVar( CITV_MOREZ ));
				}
				break;
			}
			case CIP_MOVABLE:		*vp = INT_TO_JSVAL( gPriv->GetMovable() );			break;
			case CIP_ATT:			*vp = INT_TO_JSVAL( RandomNum( gPriv->GetLoDamage(), gPriv->GetHiDamage() ));	break;
			case CIP_LAYER:			*vp = INT_TO_JSVAL( gPriv->GetLayer() );			break;
			case CIP_ITEMSINSIDE:	*vp = INT_TO_JSVAL( gPriv->GetContainsList()->Num() );	break;
			case CIP_TOTALITEMCOUNT: *vp = INT_TO_JSVAL( GetTotalItemCount( gPriv ));	break;
			case CIP_DECAYABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsDecayable() );		break;
			case CIP_DECAYTIME:		JS_NewNumberValue( cx, gPriv->GetDecayTime(), vp );	break;
			case CIP_LODAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetLoDamage() );			break;
			case CIP_HIDAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetHiDamage() );			break;
			case CIP_AC:			*vp = INT_TO_JSVAL( gPriv->GetArmourClass() );		break;
			case CIP_DEF:			*vp = INT_TO_JSVAL( gPriv->GetResist( PHYSICAL ));	break;
			case CIP_RESISTCOLD:	*vp = INT_TO_JSVAL( gPriv->GetResist( COLD ));		break;
			case CIP_RESISTHEAT:	*vp = INT_TO_JSVAL( gPriv->GetResist( HEAT ));		break;
			case CIP_RESISTLIGHT:	*vp = INT_TO_JSVAL( gPriv->GetResist( LIGHT ));	break;
			case CIP_RESISTLIGHTNING:	*vp = INT_TO_JSVAL( gPriv->GetResist( LIGHTNING ));break;
			case CIP_RESISTPOISON:	*vp = INT_TO_JSVAL( gPriv->GetResist( POISON ));	break;
			case CIP_RESISTRAIN:	*vp = INT_TO_JSVAL( gPriv->GetResist( RAIN ));		break;
			case CIP_RESISTSNOW:	*vp = INT_TO_JSVAL( gPriv->GetResist( SNOW ));		break;
			case CIP_DAMAGECOLD:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( COLD ));	break;
			case CIP_DAMAGEHEAT:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( HEAT ));	break;
			case CIP_DAMAGELIGHT:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( LIGHT ));	break;
			case CIP_DAMAGELIGHTNING:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( LIGHTNING ));	break;
			case CIP_DAMAGEPOISON:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( POISON ));	break;
			case CIP_DAMAGERAIN:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( RAIN ));	break;
			case CIP_DAMAGESNOW:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetWeatherDamage( SNOW ));	break;
			case CIP_SPEED:			*vp = INT_TO_JSVAL( gPriv->GetSpeed() );			break;
			case CIP_HEALTHLEECH:	*vp = INT_TO_JSVAL( gPriv->GetHealthLeech() );			break;
			case CIP_STAMINALEECH:	*vp = INT_TO_JSVAL( gPriv->GetStaminaLeech() );			break;
			case CIP_MANALEECH:		*vp = INT_TO_JSVAL( gPriv->GetManaLeech() );			break;
			case CIP_ARTIFACTRARITY:		*vp = INT_TO_JSVAL( gPriv->GetArtifactRarity() );			break;
			case CIP_NAME2:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName2().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_ISCHAR:	*vp = JSVAL_FALSE;								break;
			case CIP_ISITEM:	*vp = JSVAL_TRUE;								break;
			case CIP_ISSPAWNER:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetObjType() == OT_SPAWNER );	break;
			case CIP_RACE:
			{
				CRace *TempRace = nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				if( TempRace == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myRace	= JSEngine->AcquireObject( IUE_RACE, TempRace, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myRace );
				}
				break;
			}
			case CIP_MAXHP:			*vp = INT_TO_JSVAL( gPriv->GetMaxHP() );		break;
			case CIP_MAXUSES:		*vp = INT_TO_JSVAL( gPriv->GetMaxUses() );		break;
			case CIP_USESLEFT:		*vp = INT_TO_JSVAL( gPriv->GetUsesLeft() );		break;
			case CIP_RANK:			*vp = INT_TO_JSVAL( gPriv->GetRank() );			break;
			case CIP_CREATOR:		*vp = INT_TO_JSVAL( gPriv->GetCreator() );		break;
			case CIP_POISON:		*vp = INT_TO_JSVAL( gPriv->GetPoisoned() );		break;
			case CIP_DIR:			*vp = INT_TO_JSVAL( gPriv->GetDir() );			break;
			case CIP_WIPABLE:		*vp = INT_TO_JSVAL( gPriv->IsWipeable() );		break;
			case CIP_BUYVALUE:		*vp = INT_TO_JSVAL( gPriv->GetBuyValue() );		break;
			case CIP_SELLVALUE:		*vp = INT_TO_JSVAL( gPriv->GetSellValue() );	break;
			case CIP_VENDORPRICE:	*vp = INT_TO_JSVAL( gPriv->GetVendorPrice() );	break;
			case CIP_RESTOCK:		*vp = INT_TO_JSVAL( gPriv->GetRestock() );		break;
			case CIP_GOOD:			*vp = INT_TO_JSVAL( gPriv->GetGood() );			break;
			case CIP_DIVINELOCK:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDivineLocked() ); break;
			case CIP_WEIGHT:		*vp = INT_TO_JSVAL( gPriv->GetWeight() );		break;
			case CIP_WEIGHTMAX:		*vp = INT_TO_JSVAL( gPriv->GetWeightMax() );	break;
			case CIP_BASEWEIGHT:	*vp = INT_TO_JSVAL( gPriv->GetBaseWeight() );	break;
			case CIP_MAXITEMS:		*vp = INT_TO_JSVAL( gPriv->GetMaxItems() );		break;
			case CIP_STRENGTH:		*vp = INT_TO_JSVAL( gPriv->GetStrength() );		break;
			case CIP_DEXTERITY:		*vp = INT_TO_JSVAL( gPriv->GetDexterity() );	break;
			case CIP_INTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->GetIntelligence() );	break;
			case CIP_CORPSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCorpse() );	break;
			case CIP_DESC:
				tString = JS_NewStringCopyZ( cx, gPriv->GetDesc().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_EVENT:
				tString = JS_NewStringCopyZ( cx, gPriv->GetEvent().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_TEMPLASTTRADED:	*vp = INT_TO_JSVAL( gPriv->GetTempLastTraded() / 1000 );		break;
			case CIP_TEMPTIMER:		*vp = INT_TO_JSVAL( gPriv->GetTempTimer() / 1000 );		break;
			case CIP_SHOULDSAVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->ShouldSave() );			break;
			case CIP_ISNEWBIE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsNewbie() );			break;
			case CIP_ISDISPELLABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDispellable() );		break;
			case CIP_MADEWITH:		*vp = INT_TO_JSVAL( gPriv->GetMadeWith() );				break;
			case CIP_ENTRYMADEFROM:	*vp = INT_TO_JSVAL( gPriv->EntryMadeFrom() );			break;
			case CIP_ISPILEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPileable() );			break;
			case CIP_ISMARKEDBYMAKER:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsMarkedByMaker() );	break;
			case CIP_ISDYEABLE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsDyeable() );			break;
			case CIP_ISDAMAGEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDamageable() );		break;
			case CIP_ISWIPEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsWipeable() );			break;
			case CIP_ISGUARDED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );			break;
			case CIP_ISDOOROPEN:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDoorOpen() );			break;
			case CIP_ISFIELDSPELL:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsFieldSpell() );		break;
			case CIP_ISLOCKEDDOWN:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsLockedDown() );		break;
			case CIP_ISSHIELDTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsShieldType() );		break;
			case CIP_ISMETALTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsMetalType() );			break;
			case CIP_ISLEATHERTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsLeatherType() );		break;
			case CIP_CANBELOCKEDDOWN:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanBeLockedDown() );	break;
			case CIP_ISCONTTYPE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsContType() );			break;
			case CIP_CARVESECTION:	*vp = INT_TO_JSVAL( gPriv->GetCarve() );				break;
			case CIP_AMMOID:		*vp = INT_TO_JSVAL( gPriv->GetAmmoId() );				break;
			case CIP_AMMOHUE:		*vp = INT_TO_JSVAL( gPriv->GetAmmoHue() );				break;
			case CIP_AMMOFX:		*vp = INT_TO_JSVAL( gPriv->GetAmmoFX() );				break;
			case CIP_AMMOFXHUE:		*vp = INT_TO_JSVAL( gPriv->GetAmmoFXHue() );			break;
			case CIP_AMMOFXRENDER:	*vp = INT_TO_JSVAL( gPriv->GetAmmoFXRender() );			break;
			case CIP_MAXRANGE:		*vp = INT_TO_JSVAL( gPriv->GetMaxRange() );				break;
			case CIP_BASERANGE:		*vp = INT_TO_JSVAL( gPriv->GetBaseRange() );			break;
			case CIP_REGION:
			{
				CTownRegion *myReg = gPriv->GetRegion();
				if( myReg == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, myReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myTown );
				}
				break;
			}
			case CIP_SPAWNSERIAL:	*vp = INT_TO_JSVAL( gPriv->GetSpawn() );				break;
			case CIP_ORIGIN:
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CIP_ISITEMHELD:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsHeldOnCursor() );		break;
			case CIP_STEALABLE:		*vp = INT_TO_JSVAL( gPriv->GetStealable() );				break;

				// The following entries are specifically for CSpawnItem objects
			case CIP_SPAWNSECTION:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CSpawnItem *>( gPriv ))->GetSpawnSection().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				break;
			case CIP_SECTIONALIST:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					*vp = INT_TO_JSVAL(( static_cast<CSpawnItem *>( gPriv ))->IsSectionAList() );
				}
				break;
			case CIP_MININTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					*vp = INT_TO_JSVAL(( static_cast<CSpawnItem *>( gPriv ))->GetInterval( 0 ));
				}
				break;
			case CIP_MAXINTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					*vp = INT_TO_JSVAL(( static_cast<CSpawnItem *>( gPriv ))->GetInterval( 1 ));
				}
				break;
			case CIP_MULTI:
				CMultiObj *multi;
				multi = gPriv->GetMultiObj();

				if( !ValidateObject( multi ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj		= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
				// The following entries are specifically for CMultiObj objects
			case CIP_LOCKDDOWNS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetLockdownCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXLOCKDOWNS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxLockdowns() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_TRASHCONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetTrashContainerCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXTRASHCONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxTrashContainers() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_SECURECONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetSecureContainerCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXSECURECONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxSecureContainers() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_FRIENDS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetFriendCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXFRIENDS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxFriends() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_GUESTS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetGuestCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXGUESTS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxGuests() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_OWNERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetOwnerCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXOWNERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxOwners() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BANS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetBanCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXBANS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxBans() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_VENDORS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetVendorCount() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_MAXVENDORS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetMaxVendors() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_DEED:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetDeed().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_ISPUBLIC:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = BOOLEAN_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetPublicStatus() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BUILDTIMESTAMP:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetBuildTimestamp().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_TRADETIMESTAMP:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					tString = JS_NewStringCopyZ( cx, ( static_cast<CMultiObj *>( gPriv ))->GetTradeTimestamp().c_str() );
					*vp = STRING_TO_JSVAL( tString );
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BANX:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetBanX() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			case CIP_BANY:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					*vp = INT_TO_JSVAL(( static_cast<CMultiObj *>( gPriv )->GetBanY() ));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CItemProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CItem *gPriv = static_cast<CItem *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		TIMERVAL newTime;
		switch( JSVAL_TO_INT( id ))
		{
			case CIP_SECTIONID:		gPriv->SetSectionId( encaps.toString() );					break;
			case CIP_NAME:			gPriv->SetName( encaps.toString() );						break;
			case CIP_X:				gPriv->SetLocation( static_cast<SI16>( encaps.toInt() ), gPriv->GetY(), gPriv->GetZ() );	break;
			case CIP_Y:				gPriv->SetLocation( gPriv->GetX(), static_cast<SI16>( encaps.toInt() ), gPriv->GetZ() );	break;
			case CIP_Z:				gPriv->SetZ( static_cast<SI08>( encaps.toInt() ));			break;
			case CIP_ID:			gPriv->SetId( static_cast<UI16>( encaps.toInt() ));			break;
			case CIP_COLOUR:		gPriv->SetColour( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_OWNER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar*>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetOwner( myChar );
				}
				else
				{
					gPriv->SetOwner( nullptr );
				}
				break;
			case CIP_VISIBLE:		gPriv->SetVisible( static_cast<VisibleTypes>( encaps.toInt() ));	break;
			case CIP_SERIAL:																	break;
			case CIP_HEALTH:		gPriv->SetHP( static_cast<SI16>( encaps.toInt() ));			break;
			case CIP_SCRIPTTRIGGER:
			{
				// For backwards compatibility; clears out other scripts and assigns a specific script
				UI16 scriptId = static_cast<UI16>( encaps.toInt() );
				cScript *toExecute	= JSMapping->GetScript( scriptId );
				if( toExecute == nullptr )
				{
					ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
				}
				else
				{
					gPriv->ClearScriptTriggers();
					gPriv->AddScriptTrigger( scriptId );
				}
				break;
			}
			case CIP_SCRIPTTRIGGERS:
			{
				if( *vp != JSVAL_NULL )
				{
					UI16 scriptId = static_cast<UI16>( encaps.toInt() );
					cScript *toExecute	= JSMapping->GetScript( scriptId );
					if( toExecute == nullptr )
					{
						ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
					}
					else
					{
						gPriv->AddScriptTrigger( scriptId );
					}
				}
				else
				{
					// If null value was provided, clear script triggers on object
					gPriv->ClearScriptTriggers();
				}
				break; 
			}
			case CIP_WORLDNUMBER:
				if( !Map->InsideValidWorld( gPriv->GetX(), gPriv->GetY(), static_cast<UI08>( encaps.toInt() )))
					return JS_FALSE;

				gPriv->RemoveFromSight();
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), static_cast<UI08>( encaps.toInt() ), gPriv->GetInstanceId() );
				break;
			case CIP_INSTANCEID:
			{
				gPriv->RemoveFromSight();
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), gPriv->WorldNumber(), static_cast<UI16>( encaps.toInt() ));
				break;
			}
			case CIP_AMOUNT:	gPriv->SetAmount( static_cast<UI32>( encaps.toInt() )); 	break;
			case CIP_CONTAINER:
				if( *vp != JSVAL_NULL )
				{
					CBaseObject *myObj = static_cast<CBaseObject*>( encaps.toObject() );
					if( !ValidateObject( myObj ))
						break;

					gPriv->SetCont( myObj, true );
				}
				else
				{
					gPriv->SetCont( nullptr, true );
				}
				break;
			case CIP_TYPE:			gPriv->SetType( static_cast<ItemTypes>( encaps.toInt() )); 	break;
			case CIP_MORE:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE, encaps.toInt() );
				}
				break;
			}
			case CIP_MORE0:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE0, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE0, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE0, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE0, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE0, encaps.toInt() );
				}
				break;
			}
			case CIP_MORE1:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE1, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE1, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE1, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE1, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE1, encaps.toInt() );
				}
				break;
			}
			case CIP_MORE2:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MORE2, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE2, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE2, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MORE2, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MORE2, encaps.toInt() );
				}
				break;
			}
			case CIP_MOREX:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//"));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MOREX, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREX, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREX, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREX, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MOREX, encaps.toInt() );
				}
				break;
			}
			case CIP_MOREY:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//"));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MOREY, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREY, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREY, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREY, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MOREY, encaps.toInt() );
				}
				break;
			}
			case CIP_MOREZ:
			{
				auto sEncaps = encaps.toString();
				sEncaps = oldstrutil::trim( oldstrutil::removeTrailing( sEncaps, "//" ));
				auto encapsSections = oldstrutil::sections( sEncaps, " " );
				if( encapsSections.size() >= 4 )
				{
					gPriv->SetTempVar( CITV_MOREZ, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[0], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREZ, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[1], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREZ, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[2], "//" )), nullptr, 0 )));
					gPriv->SetTempVar( CITV_MOREZ, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( encapsSections[3], "//" )), nullptr, 0 )));
				}
				else
				{
					gPriv->SetTempVar( CITV_MOREZ, encaps.toInt() );
				}
				break;
			}
			case CIP_MOVABLE:		gPriv->SetMovable( static_cast<SI08>( encaps.toInt() ));	break;
			case CIP_ATT:			
				gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));
				gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));
				break;
			case CIP_LAYER:			gPriv->SetLayer( static_cast<ItemLayers>( encaps.toInt() ));	break;
			case CIP_ITEMSINSIDE:																break;
			case CIP_DECAYABLE:		gPriv->SetDecayable( encaps.toBool() );				 		break;
			case CIP_DECAYTIME:
				newTime = encaps.toInt();
				if( newTime != 0 )
				{
					newTime = BuildTimeValue( newTime );
				}
				gPriv->SetDecayTime( newTime );
				break;
			case CIP_LODAMAGE:		gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));			break;
			case CIP_HIDAMAGE:		gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));			break;
			case CIP_AC:			gPriv->SetArmourClass( static_cast<UI08>( encaps.toInt() ));		break;
			case CIP_DEF:			gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), PHYSICAL );	break;
			case CIP_RESISTCOLD:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), COLD );		break;
			case CIP_RESISTHEAT:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), HEAT );		break;
			case CIP_RESISTLIGHT:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), LIGHT );		break;
			case CIP_RESISTLIGHTNING:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), LIGHTNING );	break;
			case CIP_RESISTPOISON:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), POISON );		break;
			case CIP_RESISTRAIN:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), RAIN );		break;
			case CIP_RESISTSNOW:	gPriv->SetResist( static_cast<UI16>( encaps.toInt() ), SNOW );		break;
			case CIP_DAMAGECOLD:	gPriv->SetWeatherDamage( COLD, encaps.toBool() );			break;
			case CIP_DAMAGEHEAT:	gPriv->SetWeatherDamage( HEAT, encaps.toBool() );			break;
			case CIP_DAMAGELIGHT:	gPriv->SetWeatherDamage( LIGHT, encaps.toBool() );			break;
			case CIP_DAMAGELIGHTNING:	gPriv->SetWeatherDamage( LIGHTNING, encaps.toBool() );	break;
			case CIP_DAMAGEPOISON:	gPriv->SetWeatherDamage( POISON, encaps.toBool() );			break;
			case CIP_DAMAGERAIN:	gPriv->SetWeatherDamage( RAIN, encaps.toBool() );			break;
			case CIP_DAMAGESNOW:	gPriv->SetWeatherDamage( SNOW, encaps.toBool() );			break;
			case CIP_SPEED:			gPriv->SetSpeed( static_cast<UI08>( encaps.toInt() ));		break;
			case CIP_HEALTHLEECH:	gPriv->SetHealthLeech( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_STAMINALEECH:	gPriv->SetStaminaLeech( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_MANALEECH:		gPriv->SetManaLeech( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_ARTIFACTRARITY:	gPriv->SetArtifactRarity( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_NAME2:			gPriv->SetName2( encaps.toString() );						break;
			case CIP_RACE:			gPriv->SetRace( static_cast<RACEID>( encaps.toInt() ));		break;
			case CIP_MAXHP:			gPriv->SetMaxHP( static_cast<SI16>( encaps.toInt() ));		break;
			case CIP_MAXUSES:		gPriv->SetMaxUses( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_USESLEFT:		gPriv->SetUsesLeft( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_RANK:			gPriv->SetRank( static_cast<SI08>( encaps.toInt() ));		break;
			case CIP_CREATOR:		gPriv->SetCreator( static_cast<SERIAL>( encaps.toInt() ));	break;
			case CIP_POISON:		gPriv->SetPoisoned( static_cast<UI08>( encaps.toInt() ));	break;
			case CIP_DIR:			gPriv->SetDir( static_cast<SI16>( encaps.toInt() ));		break;
			case CIP_WIPABLE:		gPriv->SetWipeable( encaps.toBool() );						break;
			case CIP_BUYVALUE:		gPriv->SetBuyValue( static_cast<UI32>( encaps.toInt() ));	break;
			case CIP_SELLVALUE:		gPriv->SetSellValue( static_cast<UI32>( encaps.toInt() ));	break;
			case CIP_VENDORPRICE:	gPriv->SetVendorPrice( static_cast<UI32>( encaps.toInt() ));break;
			case CIP_RESTOCK:		gPriv->SetRestock( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_GOOD:			gPriv->SetGood( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_DIVINELOCK:	gPriv->SetDivineLock( encaps.toBool() );					break;
			case CIP_WEIGHT:		gPriv->SetWeight( static_cast<SI32>( encaps.toInt() ));		break;
			case CIP_WEIGHTMAX:		gPriv->SetWeightMax( static_cast<SI32>( encaps.toInt() ));	break;
			case CIP_BASEWEIGHT:	gPriv->SetBaseWeight( static_cast<SI32>( encaps.toInt() ));	break;
			case CIP_MAXITEMS:		gPriv->SetMaxItems( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_STRENGTH:		gPriv->SetStrength( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_DEXTERITY:		gPriv->SetDexterity( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_INTELLIGENCE:	gPriv->SetIntelligence( static_cast<SI16>( encaps.toInt() ));	break;
			case CIP_CORPSE:		gPriv->SetCorpse( encaps.toBool() );						break;
			case CIP_DESC:			gPriv->SetDesc( encaps.toString() );						break;
			case CIP_EVENT:			gPriv->SetEvent( encaps.toString() );						break;
			case CIP_TEMPLASTTRADED:
				newTime = encaps.toInt();
				if( newTime != 0 )
					newTime = BuildTimeValue( newTime );
				gPriv->SetTempLastTraded( newTime );
				break;
			case CIP_TEMPTIMER:
				newTime = encaps.toInt();
				if( newTime != 0 )
					newTime = BuildTimeValue( newTime );
				gPriv->SetTempTimer( newTime );
				break;
			case CIP_SHOULDSAVE:	gPriv->ShouldSave( encaps.toBool() );						break;
			case CIP_ISNEWBIE:		gPriv->SetNewbie( encaps.toBool() );						break;
			case CIP_ISDISPELLABLE:	gPriv->SetDispellable( encaps.toBool() );					break;
			case CIP_MADEWITH:		gPriv->SetMadeWith( static_cast<SI08>( encaps.toInt() ));	break;
			case CIP_ENTRYMADEFROM:	gPriv->EntryMadeFrom( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_ISPILEABLE:	gPriv->SetPileable( encaps.toBool() );						break;
			case CIP_ISMARKEDBYMAKER:	gPriv->SetMakersMark( encaps.toBool() );				break;
			case CIP_ISDYEABLE:		gPriv->SetDye( encaps.toBool() );							break;
			case CIP_ISDAMAGEABLE:	gPriv->SetDamageable( encaps.toBool() );					break;
			case CIP_ISWIPEABLE:	gPriv->SetWipeable( encaps.toBool() );						break;
			case CIP_ISGUARDED:		gPriv->SetGuarded( encaps.toBool() );						break;
			case CIP_ISDOOROPEN:	gPriv->SetDoorOpen( encaps.toBool() );						break;
			case CIP_CARVESECTION:	gPriv->SetCarve( encaps.toInt() );							break;
			case CIP_AMMOID:		gPriv->SetAmmoId( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_AMMOHUE:		gPriv->SetAmmoHue( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_AMMOFX:		gPriv->SetAmmoFX( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_AMMOFXHUE:		gPriv->SetAmmoFXHue( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_AMMOFXRENDER:	gPriv->SetAmmoFXRender( static_cast<UI16>( encaps.toInt() ));	break;
			case CIP_MAXRANGE:		gPriv->SetMaxRange( static_cast<UI08>( encaps.toInt() ));	break;
			case CIP_BASERANGE:		gPriv->SetBaseRange( static_cast<UI08>( encaps.toInt() ));	break;
			case CIP_REGION:		gPriv->SetRegion( static_cast<UI16>( encaps.toInt() ));		break;
			case CIP_ORIGIN:		gPriv->SetOrigin( cwmWorldState->ServerData()->EraStringToEnum( encaps.toString() ));						break;
			case CIP_ISITEMHELD:	gPriv->SetHeldOnCursor( encaps.toBool() );					break;
			case CIP_STEALABLE:		gPriv->SetStealable( static_cast<UI08>( encaps.toInt() ));	break;

				// The following entries are specifically for CSpawnItem objects
			case CIP_SPAWNSECTION:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->SetSpawnSection( encaps.toString() );
				}
				break;
			case CIP_SECTIONALIST:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->IsSectionAList( encaps.toBool() );
				}
				break;
			case CIP_MININTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->SetInterval( 0, static_cast<UI08>( encaps.toInt() ));
				}
				break;
			case CIP_MAXINTERVAL:
				if( gPriv->GetObjType() == OT_SPAWNER )
				{
					( static_cast<CSpawnItem *>( gPriv ))->SetInterval( 1, static_cast<UI08>( encaps.toInt() ));
				}
				break;
				// Multis only
			case CIP_MAXLOCKDOWNS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxLockdowns( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXTRASHCONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxTrashContainers( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXSECURECONTAINERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxSecureContainers( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXFRIENDS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxFriends( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXGUESTS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxGuests( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXOWNERS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxOwners( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXBANS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxBans( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_MAXVENDORS:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetMaxVendors( static_cast<UI16>( encaps.toInt() ));
				}
				break;
			case CIP_DEED:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetDeed( encaps.toString() );
				}
				break;
			case CIP_ISPUBLIC:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetPublicStatus( encaps.toBool() );	
				}
				break;
			case CIP_BUILDTIMESTAMP:
			{
				if( gPriv->GetObjType() == OT_MULTI )
				{
					time_t buildTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
					( static_cast<CMultiObj *>( gPriv ))->SetBuildTimestamp( buildTimestamp );
				}
				break;
			}
			case CIP_TRADETIMESTAMP:
			{
				if( gPriv->GetObjType() == OT_MULTI )
				{
					time_t tradeTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
					( static_cast<CMultiObj *>( gPriv ))->SetTradeTimestamp( tradeTimestamp );
				}
				break;
			}
			case CIP_BANX:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetBanX( static_cast<SI16>( encaps.toInt() ));
				}
				break;
			case CIP_BANY:
				if( gPriv->GetObjType() == OT_MULTI )
				{
					( static_cast<CMultiObj *>( gPriv ))->SetBanY( static_cast<SI16>( encaps.toInt() ));
				}
				break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Item property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CCharacterProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CChar *gPriv = static_cast<CChar *>( JS_GetPrivate( cx, obj ));

	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	if( JSVAL_IS_INT( id ))
	{
		CItem *TempItem			= nullptr;
		JSObject *TempObject	= nullptr;
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CCP_ACCOUNTNUM:	*vp = INT_TO_JSVAL( gPriv->GetAccountNum() );	break;
			case CCP_ACCOUNT:
			{
				CAccountBlock_st *accountBlock = &gPriv->GetAccount();
				if( accountBlock == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{	// Otherwise Acquire an object
					JSObject *accountObj = JSEngine->AcquireObject( IUE_ACCOUNT, accountBlock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( accountObj );
				}
				break;
			}
			case CCP_CREATEDON: *vp = INT_TO_JSVAL( gPriv->GetCreatedOn() );		break;
			case CCP_PLAYTIME: *vp = INT_TO_JSVAL( gPriv->GetPlayTime() );		break;
			case CCP_NAME:
				{
					CSocket *tSock = nullptr;

					std::string mCharName = GetNpcDictName( gPriv, tSock, NRS_SCRIPT );
					std::string convertedString = oldstrutil::stringToWstringToString( mCharName );

					tString = JS_NewStringCopyZ( cx, convertedString.c_str() );
					*vp = STRING_TO_JSVAL( tString );
					break;
				}
			case CCP_ORIGNAME:
			{
				[[maybe_unused]] CSocket *tSock = nullptr;

				std::string mCharName = gPriv->GetOrgName();
				std::string convertedString = oldstrutil::stringToWstringToString( mCharName );

				tString = JS_NewStringCopyZ( cx, convertedString.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			}
			case CCP_TITLE:
				tString = JS_NewStringCopyZ( cx, gPriv->GetTitle().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_SECTIONID:
				tString = JS_NewStringCopyZ( cx, gPriv->GetSectionId().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_X:			*vp = INT_TO_JSVAL( gPriv->GetX() );			break;
			case CCP_Y:			*vp = INT_TO_JSVAL( gPriv->GetY() );			break;
			case CCP_Z:			*vp = INT_TO_JSVAL( gPriv->GetZ() );			break;
			case CCP_OLDX:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.x );
				break;
			}
			case CCP_OLDY:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.y );
				break;
			}
			case CCP_OLDZ:		
			{
				auto oldLocation = gPriv->GetOldLocation();
				*vp = INT_TO_JSVAL( oldLocation.z );
				break;
			}
			case CCP_ID:		*vp = INT_TO_JSVAL( gPriv->GetId() );				break;
			case CCP_COLOUR:	*vp = INT_TO_JSVAL( gPriv->GetColour() );			break;
			case CCP_CONTROLSLOTS:	*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetControlSlots() ));			break;
			case CCP_CONTROLSLOTSUSED:	*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetControlSlotsUsed() ));	break;
			case CCP_ORNERINESS:	*vp = INT_TO_JSVAL( gPriv->GetOrneriness() );	break;
			case CCP_OWNER:
				CBaseObject *TempObj;
				TempObj = gPriv->GetOwnerObj();

				if( !ValidateObject( TempObj ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CCP_VISIBLE:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetVisible() ));	break;
			case CCP_SERIAL:
			{
				if( !INT_FITS_IN_JSVAL( gPriv->GetSerial() ))
				{
					JS_NewNumberValue( cx, gPriv->GetSerial(), vp );
				}
				else
				{
					*vp = INT_TO_JSVAL( gPriv->GetSerial() );
				}
				break;
			}
			case CCP_HEALTH:		*vp = INT_TO_JSVAL( gPriv->GetHP() );				break;
			case CCP_SCRIPTTRIGGER:
			{
				// For backwards compatibility, get last scripttrigger from vector
				// For older worldfiles, this will be the only scripttrigger added to the vector after load
				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				UI16 lastScriptTrigger = 0;
				auto numberOfTriggers = scriptTriggers.size();
				if( numberOfTriggers > 0 )
				{
					lastScriptTrigger = scriptTriggers[numberOfTriggers-1];
				}
				*vp = INT_TO_JSVAL( lastScriptTrigger );
				break;
			}
			case CCP_SCRIPTTRIGGERS:
			{
				jsval scriptId;
				JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
				{
					scriptId = INT_TO_JSVAL( scriptTriggers[i] );
					JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
				}

				*vp = OBJECT_TO_JSVAL( scriptTriggersJS );
				break;
			}
			case CCP_WORLDNUMBER:	*vp = INT_TO_JSVAL( gPriv->WorldNumber() );			break;
			case CCP_INSTANCEID:	*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );		break;
			case CCP_TARGET:
				CChar *tempChar;
				tempChar = gPriv->GetTarg();

				if( !ValidateObject( tempChar ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CCP_DEXTERITY:				*vp = INT_TO_JSVAL( gPriv->GetDexterity() );		break;
			case CCP_INTELLIGENCE:			*vp = INT_TO_JSVAL( gPriv->GetIntelligence() );		break;
			case CCP_STRENGTH:				*vp = INT_TO_JSVAL( gPriv->GetStrength() );			break;
			case CCP_ACTUALDEXTERITY:		*vp = INT_TO_JSVAL( gPriv->ActualDexterity() );		break;
			case CCP_ACTUALINTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->ActualIntelligence() );	break;
			case CCP_ACTUALSTRENGTH:		*vp = INT_TO_JSVAL( gPriv->ActualStrength() );		break;
			case CCP_BASESKILLS:
				TempObject = JS_NewObject( cx, &UOXBaseSkills_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_SKILLS:
				TempObject = JS_NewObject( cx, &UOXSkills_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_SKILLUSE:
				TempObject = JS_NewObject( cx, &UOXSkillsUsed_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp = OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_MANA:			*vp = INT_TO_JSVAL( gPriv->GetMana() );			break;
			case CCP_STAMINA:		*vp = INT_TO_JSVAL( gPriv->GetStamina() );		break;
			case CCP_CHARPACK:
				TempItem = gPriv->GetPackItem();

				if( !ValidateObject( TempItem ))
					*vp = JSVAL_NULL;
				else
				{
					// Otherwise Acquire an object
					JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, TempItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myItem );
				}
				break;
			case CCP_FAME:			*vp = INT_TO_JSVAL( gPriv->GetFame() );					break;
			case CCP_KARMA:			*vp = INT_TO_JSVAL( gPriv->GetKarma() );				break;
			case CCP_ATTACK:		*vp = INT_TO_JSVAL( Combat->CalcAttackPower( gPriv, true ));	break;
			case CCP_CANATTACK:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetCanAttack() );		break;
			case CCP_FLEEAT:		*vp = INT_TO_JSVAL( gPriv->GetFleeAt() );				break;
			case CCP_REATTACKAT:	*vp = INT_TO_JSVAL( gPriv->GetReattackAt() );			break;
			case CCP_BRKPEACE:		*vp = INT_TO_JSVAL( gPriv->GetBrkPeaceChance() );		break;
			case CCP_HUNGER:		*vp = INT_TO_JSVAL( gPriv->GetHunger() );				break;
			case CCP_HUNGERRATE:
			{
				CRace *TempRace	= nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				// Try to fetch hungerRate from character's race
				UI16 hungerRate = 0;
				if( TempRace != nullptr )
				{
					hungerRate = TempRace->GetHungerRate();
				}

				// If hungerRate from Race is zero, use the global hunger rate from UOX.INI instead
				if( hungerRate == 0 )
				{
					hungerRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE );
				}

				*vp = INT_TO_JSVAL( hungerRate );
				break;
			}
			case CCP_THIRST:		*vp = INT_TO_JSVAL( gPriv->GetThirst() );				break;
			case CCP_THIRSTRATE:
			{
				CRace *TempRace	= nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				// Try to fetch thirstRate from character's race
				UI16 thirstRate = 0;
				if( TempRace != nullptr )
				{
					thirstRate = TempRace->GetThirstRate();
				}

				// If thirstRate from Race is zero, use the global thirst rate from UOX.INI instead
				if( thirstRate == 0 )
				{
					thirstRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_THIRSTRATE );
				}

				*vp = INT_TO_JSVAL( thirstRate );
				break;
			}
			case CCP_FROZEN:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsFrozen() );			break;
			case CCP_COMMANDLEVEL:	*vp = INT_TO_JSVAL( gPriv->GetCommandLevel() );			break;
			case CCP_RACE:
			{
				CRace *TempRace	= nullptr;
				TempRace = Races->Race( gPriv->GetRace() );

				if( TempRace == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myRace	= JSEngine->AcquireObject( IUE_RACE, TempRace, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myRace );
				}
				break;
			}
			case CCP_HASSTOLEN:		*vp = BOOLEAN_TO_JSVAL( gPriv->HasStolen() );	break;
			case CCP_CRIMINAL:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCriminal() );	break;
			case CCP_MURDERER:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsMurderer() );	break;
			case CCP_INNOCENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsInnocent() );	break;
			case CCP_NEUTRAL:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsNeutral() );	break;
			case CCP_MURDERCOUNT:	*vp = INT_TO_JSVAL( gPriv->GetKills() );		break;
			case CCP_GENDER:
				switch( gPriv->GetId() )
				{
					case 0x0190:	// human male, dead or alive
					case 0x0192:	*vp = INT_TO_JSVAL( 0 );						break;
					case 0x0191:	// human female, dead or alive
					case 0x0193:	*vp = INT_TO_JSVAL( 1 );						break;
					case 0x025D:	// elf male, dead or alive
					case 0x025F:	*vp = INT_TO_JSVAL( 2 );						break;
					case 0x025E:	// elf female, dead or alive
					case 0x0260:	*vp = INT_TO_JSVAL( 3 );						break;
					case 0x029A:	// gargoyle male, dead or alive
					case 0x02B6:	*vp = INT_TO_JSVAL( 4 );						break;
					case 0x029B:	// gargoyle female, dead or alive
					case 0x02B7:	*vp = INT_TO_JSVAL( 5 );						break;
					default:		*vp = INT_TO_JSVAL( 0xFF );						break;
				}
				break;
			case CCP_DEAD:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDead() );		break;
			case CCP_NPC:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsNpc() );		break;
			case CCP_AWAKE:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsAwake() );		break;
			case CCP_ONLINE:		*vp = BOOLEAN_TO_JSVAL( IsOnline(( *gPriv )));	break;
			case CCP_DIRECTION:		*vp = INT_TO_JSVAL( gPriv->GetDir() );			break;
				// 3  objects: regions + towns + guilds
			case CCP_ISRUNNING:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetRunning() );	break;
			case CCP_REGION:
			{
				CTownRegion *myReg = gPriv->GetRegion();
				if( myReg == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, myReg, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myTown );
				}
				break;
			}
			case CCP_ORIGIN:
				tString = JS_NewStringCopyZ( cx, cwmWorldState->ServerData()->EraEnumToString( static_cast<ExpansionRuleset>( gPriv->GetOrigin() )).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_TOWN:
			{
				UI16 tempTownId	= 0xFF;
				tempTownId = gPriv->GetTown();

				// We need to decide here whether 0xFF is a valid town (wilderness) or not
				// i would say no its not
				if( tempTownId == 0xFF )
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Should build the town here
					JSObject *myTown = JSEngine->AcquireObject( IUE_REGION, cwmWorldState->townRegions[tempTownId], JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myTown );
				}
				break;
			}
			case CCP_GUILD:
			{
				GUILDID tempGuildId	= -1;
				tempGuildId = gPriv->GetGuildNumber();

				// Character has no guild
				if( tempGuildId == -1 ) // isn't there a constant or something like?
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myGuild = JSEngine->AcquireObject( IUE_GUILD, GuildSys->Guild( tempGuildId ), JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myGuild );
				}
				break;
			}
			case CCP_SOCKET:
			{ // So we can declare the variables here
				CSocket *tSock = gPriv->GetSocket();
				if( tSock == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{	// Otherwise Acquire an object
					JSObject *mySock	= JSEngine->AcquireObject( IUE_SOCK, tSock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( mySock );
				}
				break;
			}
			case CCP_ISCHAR:		*vp = JSVAL_TRUE;									break;
			case CCP_ISITEM:		*vp = JSVAL_FALSE;									break;
			case CCP_ISSPAWNER:		*vp = JSVAL_FALSE;									break;
			case CCP_SPAWNSERIAL:	*vp = INT_TO_JSVAL( gPriv->GetSpawn() );			break;
			case CCP_MAXHP:			*vp = INT_TO_JSVAL( gPriv->GetMaxHP() );			break;
			case CCP_MAXSTAMINA:	*vp = INT_TO_JSVAL( gPriv->GetMaxStam() );			break;
			case CCP_MAXMANA:		*vp = INT_TO_JSVAL( gPriv->GetMaxMana() );			break;
			case CCP_OLDWANDERTYPE:	*vp = INT_TO_JSVAL( gPriv->GetOldNpcWander() );		break;
			case CCP_WANDERTYPE:	*vp = INT_TO_JSVAL( gPriv->GetNpcWander() );		break;
			case CCP_FX1:			*vp = INT_TO_JSVAL( gPriv->GetFx( 0 ));				break;
			case CCP_FY1:			*vp = INT_TO_JSVAL( gPriv->GetFy( 0 ));				break;
			case CCP_FX2:			*vp = INT_TO_JSVAL( gPriv->GetFx( 1 ));				break;
			case CCP_FY2:			*vp = INT_TO_JSVAL( gPriv->GetFy( 1 ));				break;
			case CCP_FZ:			*vp = INT_TO_JSVAL( gPriv->GetFz() );				break;
			case CCP_ISONHORSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsOnHorse() );		break;
			case CCP_ISFLYING:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsFlying() );		break;
			case CCP_ISGUARDED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );		break;
			case CCP_GUARDING:
			{
				CBaseObject *tempObj = gPriv->GetGuarding();
				if( !ValidateObject( tempObj ))
				{	// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj = nullptr;
					if( tempObj->CanBeObjType( OT_CHAR ))
					{
						myObj = JSEngine->AcquireObject( IUE_CHAR, tempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					else if( tempObj->CanBeObjType( OT_ITEM ))
					{
						myObj = JSEngine->AcquireObject( IUE_ITEM, tempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			}
			case CCP_TDEXTERITY:	*vp = INT_TO_JSVAL( gPriv->GetDexterity2() );		break;
			case CCP_TINTELLIGENCE:	*vp = INT_TO_JSVAL( gPriv->GetIntelligence2() );	break;
			case CCP_TSTRENGTH:		*vp = INT_TO_JSVAL( gPriv->GetStrength2() );		break;
			case CCP_POISON:		*vp = INT_TO_JSVAL( gPriv->GetPoisoned() );			break;
			case CCP_LIGHTLEVEL:	*vp = INT_TO_JSVAL( gPriv->GetFixedLight() );		break;
			case CCP_VULNERABLE:	*vp = BOOLEAN_TO_JSVAL( !gPriv->IsInvulnerable() );	break;
			case CCP_HUNGERSTATUS:	*vp = BOOLEAN_TO_JSVAL( gPriv->WillHunger() );		break;
			case CCP_THIRSTSTATUS:	*vp = BOOLEAN_TO_JSVAL( gPriv->WillThirst() );		break;
			case CCP_LODAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetLoDamage() );			break;
			case CCP_HIDAMAGE:		*vp = INT_TO_JSVAL( gPriv->GetHiDamage() );			break;
			case CCP_FLAG:			*vp = INT_TO_JSVAL( gPriv->GetFlag() );				break;
			case CCP_ATWAR:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsAtWar() );			break;
			case CCP_SPELLCAST:		*vp = INT_TO_JSVAL( gPriv->GetSpellCast() );		break;
			case CCP_ISCASTING:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsCasting() || gPriv->IsJSCasting() );		break;
			case CCP_PRIV:			*vp = INT_TO_JSVAL( gPriv->GetPriv() );				break;
			case CCP_TOWNPRIV:		*vp = INT_TO_JSVAL( gPriv->GetTownPriv() );			break;
			case CCP_GUILDTITLE:
				tString = JS_NewStringCopyZ( cx, gPriv->GetGuildTitle().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_HAIRSTYLE:		*vp = INT_TO_JSVAL( gPriv->GetHairStyle() );		break;
			case CCP_HAIRCOLOUR:	*vp = INT_TO_JSVAL( gPriv->GetHairColour() );		break;
			case CCP_BEARDSTYLE:	*vp = INT_TO_JSVAL( gPriv->GetBeardStyle() );		break;
			case CCP_BEARDCOLOUR:	*vp = INT_TO_JSVAL( gPriv->GetBeardColour() );		break;
			case CCP_FONTTYPE:		*vp = INT_TO_JSVAL( gPriv->GetFontType() );			break;
			case CCP_SAYCOLOUR:		*vp = INT_TO_JSVAL( gPriv->GetSayColour() );		break;
			case CCP_EMOTECOLOUR:	*vp = INT_TO_JSVAL( gPriv->GetEmoteColour() );		break;
			case CCP_ATTACKER:
			{
				// Hm Quite funny, same thing as .owner
				CChar *tempChar	= gPriv->GetAttacker();
				if( !ValidateObject( tempChar ))
				{	// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			}
			case CCP_RACEGATE:		*vp = INT_TO_JSVAL( gPriv->GetRaceGate() );			break;
			case CCP_SKILLLOCK:
				TempObject = JS_NewObject( cx, &UOXSkillsLock_class, nullptr, obj );
				JS_DefineProperties( cx, TempObject, CSkillsProps );
				JS_SetPrivate( cx, TempObject, gPriv );
				*vp	= OBJECT_TO_JSVAL( TempObject );
				break;
			case CCP_DEATHS:		*vp = INT_TO_JSVAL( gPriv->GetDeaths() );					break;
			case CCP_OWNERCOUNT:	*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetOwnerCount() ));		break;
			case CCP_NEXTACT:		*vp = INT_TO_JSVAL( gPriv->GetNextAct() );					break;
			case CCP_PETCOUNT:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetPetList()->Num() ));	break;
			case CCP_FOLLOWERCOUNT:		*vp = INT_TO_JSVAL( static_cast<UI08>( gPriv->GetFollowerList()->Num() ));	break;
			case CCP_OWNEDITEMSCOUNT:	*vp = INT_TO_JSVAL( gPriv->GetOwnedItems()->size() );	break;
			case CCP_CELL:			*vp = INT_TO_JSVAL( gPriv->GetCell() );						break;
			case CCP_ALLMOVE:		*vp = BOOLEAN_TO_JSVAL( gPriv->AllMove() );					break;
			case CCP_HOUSEICONS:	*vp = BOOLEAN_TO_JSVAL( gPriv->ViewHouseAsIcon() );			break;
			case CCP_SPATTACK:		*vp = INT_TO_JSVAL( gPriv->GetSpAttack() );					break;
			case CCP_SPDELAY:		*vp = INT_TO_JSVAL( gPriv->GetSpDelay() );					break;
			case CCP_AITYPE:		*vp = INT_TO_JSVAL( gPriv->GetNpcAiType() );				break;
			case CCP_SPLIT:			*vp = INT_TO_JSVAL( gPriv->GetSplit() );					break;
			case CCP_SPLITCHANCE:	*vp = INT_TO_JSVAL( gPriv->GetSplitChance() );				break;
			case CCP_TRAINER:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanTrain() );				break;
			case CCP_HIRELING:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanBeHired() );				break;
			case CCP_WEIGHT:		*vp = INT_TO_JSVAL( gPriv->GetWeight() );					break;
			case CCP_SQUELCH:		*vp = INT_TO_JSVAL( gPriv->GetSquelched() );				break;
			case CCP_ISJAILED:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsJailed() );				break;
			case CCP_MAGICREFLECT:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsTempReflected() );			break;
			case CCP_PERMMAGICREFLECT:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPermReflected() );		break;
			case CCP_TAMED:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsTamed() );					break;
			case CCP_TAMEDHUNGERRATE: *vp = INT_TO_JSVAL( gPriv->GetTamedHungerRate() );		break;
			case CCP_TAMEDTHIRSTRATE: *vp = INT_TO_JSVAL( gPriv->GetTamedThirstRate() );		break;
			case CCP_HUNGERWILDCHANCE: *vp = INT_TO_JSVAL( gPriv->GetTamedHungerWildChance() );	break;
			case CCP_THIRSTWILDCHANCE: *vp = INT_TO_JSVAL( gPriv->GetTamedThirstWildChance() );	break;
			case CCP_FOODLIST:
				tString = JS_NewStringCopyZ( cx, gPriv->GetFood().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CCP_MOUNTED:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetMounted() );				break;
			case CCP_STABLED:		*vp = BOOLEAN_TO_JSVAL( gPriv->GetStabled() );				break;
			case CCP_USINGPOTION:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsUsingPotion() );			break;
			case CCP_STEALTH:		*vp = INT_TO_JSVAL( gPriv->GetStealth() );					break;
			case CCP_SKILLTOTAME:	*vp = INT_TO_JSVAL( gPriv->GetTaming() );					break;
			case CCP_SKILLTOPROV:	*vp = INT_TO_JSVAL( gPriv->GetProvoing() );					break;
			case CCP_SKILLTOPEACE:	*vp = INT_TO_JSVAL( gPriv->GetPeaceing() );					break;
			case CCP_POISONSTRENGTH:	*vp = INT_TO_JSVAL( gPriv->GetPoisonStrength() );		break;
			case CCP_ISPOLYMORPHED:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsPolymorphed() );			break;
			case CCP_ISINCOGNITO:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsIncognito() );				break;
			case CCP_ISDISGUISED:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDisguised() );				break;
			case CCP_CANRUN:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanRun() );					break;
			case CCP_ISMEDITATING:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsMeditating() );			break;
			case CCP_ISGM:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsGM() );					break;
			case CCP_CANBROADCAST:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanBroadcast() );			break;
			case CCP_SINGCLICKSER:	*vp = BOOLEAN_TO_JSVAL( gPriv->GetSingClickSer() );			break;
			case CCP_NOSKILLTITLES:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoSkillTitles() );			break;
			case CCP_ISGMPAGEABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsGMPageable() );			break;
			case CCP_CANSNOOP:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanSnoop() );				break;
			case CCP_ISCOUNSELOR:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsCounselor() );				break;
			case CCP_NONEEDMANA:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoNeedMana() );				break;
			case CCP_ISDISPELLABLE:	*vp = BOOLEAN_TO_JSVAL( gPriv->IsDispellable() );			break;
			case CCP_NONEEDREAGS:	*vp = BOOLEAN_TO_JSVAL( gPriv->NoNeedReags() );				break;
			case CCP_ISANIMAL:		*vp	= BOOLEAN_TO_JSVAL( cwmWorldState->creatures[gPriv->GetId()].IsAnimal() ); break;
			case CCP_ISHUMAN:		*vp	= BOOLEAN_TO_JSVAL( cwmWorldState->creatures[gPriv->GetId()].IsHuman() ); break;
			case CCP_ORGID:			*vp = INT_TO_JSVAL( gPriv->GetOrgId() );					break;
			case CCP_ORGSKIN:		*vp = INT_TO_JSVAL( gPriv->GetOrgSkin() );					break;
			case CCP_NPCFLAG:		*vp = INT_TO_JSVAL( static_cast<SI32>(gPriv->GetNPCFlag() ));break;
			case CCP_NPCGUILD:		*vp = INT_TO_JSVAL( gPriv->GetNPCGuild() );					break;
			case CCP_ISSHOP:		*vp = BOOLEAN_TO_JSVAL( gPriv->IsShop() );					break;
			case CCP_MAXLOYALTY:	*vp = INT_TO_JSVAL( gPriv->GetMaxLoyalty() );				break;
			case CCP_LOYALTY:		*vp = INT_TO_JSVAL( gPriv->GetLoyalty() );					break;
			case CCP_LOYALTYRATE:
			{
				// Use global loyalty rate from UOX.INI
				UI16 loyaltyRate = 0;
				if( loyaltyRate == 0 )
				{
					loyaltyRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_LOYALTYRATE );
				}

				*vp = INT_TO_JSVAL( loyaltyRate );
				break;
			}
			case CCP_SHOULDSAVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->ShouldSave() );			break;
			case CCP_PARTYLOOTABLE:
			{
				Party *toGet = PartyFactory::GetSingleton().Get( gPriv );
				if( toGet == nullptr )
				{
					*vp = BOOLEAN_TO_JSVAL( false );
				}
				else
				{
					CPartyEntry *toScan = toGet->Find( gPriv );
					if( toScan == nullptr )
					{
						*vp = BOOLEAN_TO_JSVAL( false );
					}
					else
					{
						*vp = BOOLEAN_TO_JSVAL( toScan->IsLootable() );
					}
				}
				break;
			}
			case CCP_PARTY:
			{
				// Hm Quite funny, same thing as .owner
				Party *tempParty = PartyFactory::GetSingleton().Get( gPriv );
				if( tempParty == nullptr )
				{	// Return a JS_NULL
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myParty = JSEngine->AcquireObject( IUE_PARTY, tempParty, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myParty );
				}
				break;
			}
			case CCP_MULTI:
				CMultiObj *multi;
				multi = gPriv->GetMultiObj();

				if( !ValidateObject( multi ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myObj	= JSEngine->AcquireObject( IUE_ITEM, multi, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			case CCP_HOUSESOWNED:		*vp = INT_TO_JSVAL( gPriv->CountHousesOwned( false ));	break;
			case CCP_HOUSESCOOWNED:		*vp = INT_TO_JSVAL( gPriv->CountHousesOwned( true ));	break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		// ... but keep track of the property value we're trying to retrieve, stored in vp!
		jsval origVp = *vp;
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		*vp = origVp;
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Item property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CCharacterProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CChar *gPriv = static_cast<CChar *>( JS_GetPrivate( cx, obj ));
	if( !ValidateObject( gPriv ))
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );

	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CCP_ACCOUNTNUM:	gPriv->SetAccountNum( static_cast<UI16>( encaps.toInt() ));					break;
			case CCP_CREATEDON:		break;
			case CCP_PLAYTIME:		gPriv->SetPlayTime( static_cast<UI32>( encaps.toInt() ));					break;
			case CCP_SECTIONID:		gPriv->SetSectionId( encaps.toString() );									break;
			case CCP_NAME:			gPriv->SetName( encaps.toString() );										break;
			case CCP_ORIGNAME:		gPriv->SetOrgName( encaps.toString() );										break;
			case CCP_TITLE:			gPriv->SetTitle( encaps.toString() );										break;
			case CCP_X:				gPriv->SetLocation( static_cast<SI16>( encaps.toInt() ), gPriv->GetY(), gPriv->GetZ() );	break;
			case CCP_Y:				gPriv->SetLocation( gPriv->GetX(), static_cast<SI16>( encaps.toInt() ), gPriv->GetZ() );	break;
			case CCP_Z:				gPriv->SetZ( static_cast<SI08>( encaps.toInt() ));							break;
			case CCP_ID:			gPriv->SetId( static_cast<UI16>( encaps.toInt() ));							break;
			case CCP_COLOUR:		gPriv->SetColour( static_cast<UI16>( encaps.toInt() ));						break;
			case CCP_CONTROLSLOTS:	gPriv->SetControlSlots( static_cast<UI16>( encaps.toInt() ));				break;
			case CCP_CONTROLSLOTSUSED:	gPriv->SetControlSlotsUsed( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_ORNERINESS:	gPriv->SetOrneriness( static_cast<UI16>( encaps.toInt() ));					break;
			case CCP_OWNER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar*>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetOwner( myChar );
				}
				else
				{
					gPriv->SetOwner( nullptr );
				}
				break;
			case CCP_VISIBLE:		gPriv->SetVisible( static_cast<VisibleTypes>( encaps.toInt() ));	break;
			case CCP_SERIAL:											break;
			case CCP_HEALTH:		gPriv->SetHP( encaps.toInt() );		break;
			case CCP_SCRIPTTRIGGER:
			{
				// For backwards compatibility; clears out other scripts and assigns a specific script
				UI16 scriptId = static_cast<UI16>( encaps.toInt() );
				cScript *toExecute	= JSMapping->GetScript( scriptId );
				if( toExecute == nullptr )
				{
					ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
				}
				else
				{
					gPriv->ClearScriptTriggers();
					gPriv->AddScriptTrigger( scriptId );
				}
				break;
			}
			case CCP_SCRIPTTRIGGERS:
			{
				if( *vp != JSVAL_NULL )
				{
					UI16 scriptId = static_cast<UI16>( encaps.toInt() );
					cScript *toExecute	= JSMapping->GetScript( scriptId );
					if( toExecute == nullptr )
					{
						ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
					}
					else
					{
						gPriv->AddScriptTrigger( scriptId );
					}
				}
				else
				{
					// If null value was provided, clear script triggers on object
					gPriv->ClearScriptTriggers();
				}
				break; 
			}
			case CCP_WORLDNUMBER:
				if( !Map->InsideValidWorld( gPriv->GetX(), gPriv->GetY(), static_cast<UI08>( encaps.toInt() )))
					return JS_FALSE;

				gPriv->RemoveFromSight();
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), static_cast<UI08>( encaps.toInt() ), gPriv->GetInstanceId() );
				if( !gPriv->IsNpc() )
				{
					SendMapChange( static_cast<UI08>( encaps.toInt() ), gPriv->GetSocket() );
				}
				break;
			case CCP_INSTANCEID:
			{
				// Remove character from nearby players
				gPriv->RemoveFromSight();

				// Remove nearby objects from player!
				if( !gPriv->IsNpc() )
				{
					gPriv->RemoveAllObjectsFromSight( gPriv->GetSocket() );
				}
				gPriv->SetLocation( gPriv->GetX(), gPriv->GetY(), gPriv->GetZ(), gPriv->WorldNumber(), static_cast<UI16>( encaps.toInt() ));
				break;
			}
			case CCP_TARGET:
				//TODO: Check if the user(admin per jscript) can set the target
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar *>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetTarg( myChar );
				}
				else
				{
					gPriv->SetTarg( nullptr );
				}
				break;
			case CCP_DEXTERITY:		gPriv->SetDexterity( encaps.toInt() );				break;
			case CCP_INTELLIGENCE:	gPriv->SetIntelligence( encaps.toInt() );			break;
			case CCP_STRENGTH:		gPriv->SetStrength( encaps.toInt() );				break;
			case CCP_SKILLS:															break;
			case CCP_MANA:			gPriv->SetMana( encaps.toInt() );					break;
			case CCP_STAMINA:		gPriv->SetStamina( encaps.toInt() );				break;
			case CCP_CHARPACK:															break;
			case CCP_FAME:			gPriv->SetFame( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_KARMA:			gPriv->SetKarma( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_HUNGER:		gPriv->SetHunger( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_THIRST:		gPriv->SetThirst( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_CANATTACK:		gPriv->SetCanAttack( encaps.toBool() );					break;
			case CCP_FLEEAT:		gPriv->SetFleeAt( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_REATTACKAT:	gPriv->SetReattackAt( static_cast<SI16>( encaps.toInt() ));			break;
			case CCP_BRKPEACE:		gPriv->SetBrkPeaceChance( encaps.toInt() );				break;
			case CCP_SETPEACE:		gPriv->SetPeace( encaps.toInt() );						break;
			case CCP_FROZEN:		gPriv->SetFrozen( encaps.toBool() );					break;
			case CCP_COMMANDLEVEL:	gPriv->SetCommandLevel( static_cast<UI08>( encaps.toInt() ));		break;
			case CCP_RACE:			Races->ApplyRace( gPriv, static_cast<RACEID>( encaps.toInt() ), true );	break;
			case CCP_MAXHP:			gPriv->SetFixedMaxHP( static_cast<SI16>( encaps.toInt() ));			break;
			case CCP_MAXMANA:		gPriv->SetFixedMaxMana( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_MAXSTAMINA:	gPriv->SetFixedMaxStam( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_HASSTOLEN:
			{
				if( encaps.toBool() )
				{
					FlagForStealing( gPriv );
				}
				else
				{
					gPriv->HasStolen( false );
					gPriv->SetTimer( tCHAR_STEALFLAG, 0 );
					UpdateFlag( gPriv );
				}
				break;
			}
			case CCP_CRIMINAL:
				if( encaps.toBool() )
				{
					MakeCriminal( gPriv );
				}
				else
				{
					gPriv->SetTimer( tCHAR_CRIMFLAG, 0 );
					UpdateFlag( gPriv );
				}
				break;
			case CCP_INNOCENT:
				if( !encaps.toBool() )
				{
					MakeCriminal( gPriv );
				}
				else
				{
					gPriv->SetTimer( tCHAR_CRIMFLAG, 0 );
					UpdateFlag( gPriv );
				}
				break;
			case CCP_NEUTRAL:
				if( encaps.toBool() )
				{
					gPriv->SetFlagNeutral();
				}
				else
				{
					gPriv->SetFlagBlue();
				}
				gPriv->SetTimer( tCHAR_CRIMFLAG, 0 );
				UpdateFlag( gPriv );
				break;
			case CCP_MURDERCOUNT:
				gPriv->SetKills( static_cast<SI16>( encaps.toInt() ));
				UpdateFlag( gPriv );
				break;
			case CCP_GENDER:
				switch( static_cast<SI16>( encaps.toInt() ))
				{
					case 0:	// male
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x0192 );
						}
						else
						{
							gPriv->SetId( 0x0190 );
						}
						break;
					case 1:	// female
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x0193 );
						}
						else
						{
							gPriv->SetId( 0x0191 );
						}
						break;
					case 2:	// elf male
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x025F );
						}
						else
						{
							gPriv->SetId( 0x025D );
						}
						break;
					case 3:	// elf female
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x0260 );
						}
						else
						{
							gPriv->SetId( 0x025E );
						}
						break;
					case 4:	// gargoyle male
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x02B6 );
						}
						else
						{
							gPriv->SetId( 0x029A );
						}
						break;
					case 5: // gargoyle female
						if( gPriv->IsDead() )
						{
							gPriv->SetId( 0x02B7 );
						}
						else
						{
							gPriv->SetId( 0x029B );
						}
						break;
					default:
						break;
				}
				break;
			case CCP_NPC:			gPriv->SetNpc( encaps.toBool() );						break;
			case CCP_AWAKE:			gPriv->SetAwake( encaps.toBool() );						break;
			case CCP_DIRECTION:		gPriv->SetDir( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_REGION:		gPriv->SetRegion( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_ORIGIN:		gPriv->SetOrigin( cwmWorldState->ServerData()->EraStringToEnum( encaps.toString() ));	break;
			case CCP_TOWN:
				cwmWorldState->townRegions[gPriv->GetTown()]->RemoveTownMember( *gPriv );
				if( *vp != JSVAL_NULL )
				{
					cwmWorldState->townRegions[encaps.toInt()]->AddAsTownMember( *gPriv );
				}
				break;
			case CCP_GUILD:
				if( !gPriv->IsNpc() )
				{
					GuildSys->Resign( gPriv->GetSocket() );

					if( *vp != JSVAL_NULL )
					{
						CGuild *myGuild = static_cast<CGuild*>( encaps.toObject() );
						myGuild->NewRecruit( gPriv->GetSerial() );
					}
				}
				break;
			case CCP_OLDWANDERTYPE: gPriv->SetOldNpcWander( static_cast<SI08>( encaps.toInt() )); 		break;
			case CCP_WANDERTYPE: 	gPriv->SetNpcWander( static_cast<SI08>( encaps.toInt() ), true );	break;
			case CCP_FX1:			gPriv->SetFx( static_cast<SI16>( encaps.toInt() ), 0 );	break;
			case CCP_FY1:			gPriv->SetFy( static_cast<SI16>( encaps.toInt() ), 0 );	break;
			case CCP_FX2:			gPriv->SetFx( static_cast<SI16>( encaps.toInt() ), 1 );	break;
			case CCP_FY2:			gPriv->SetFy( static_cast<SI16>( encaps.toInt() ), 1 );	break;
			case CCP_FZ:			gPriv->SetFz( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_TDEXTERITY:	gPriv->SetDexterity2( encaps.toInt() );					break;
			case CCP_TINTELLIGENCE:	gPriv->SetIntelligence2( encaps.toInt() );				break;
			case CCP_TSTRENGTH:		gPriv->SetStrength2( encaps.toInt() );					break;
			case CCP_LIGHTLEVEL:
				gPriv->SetFixedLight( static_cast<UI08>( encaps.toInt() ));
				if( gPriv->GetSocket() != nullptr )
				{
					if( static_cast<UI08>( encaps.toInt() ) == 255 )
					{
						DoLight( gPriv->GetSocket(), cwmWorldState->ServerData()->WorldLightCurrentLevel() );
					}
					else
					{
						DoLight( gPriv->GetSocket(), static_cast<UI08>( encaps.toInt() ));
					}
				}
				break;
			case CCP_VULNERABLE:	gPriv->SetInvulnerable( !encaps.toBool() );		break;
			case CCP_HUNGERSTATUS:	gPriv->SetHungerStatus( encaps.toBool() );		break;
			case CCP_THIRSTSTATUS:	gPriv->SetThirstStatus( encaps.toBool() );		break;
			case CCP_LODAMAGE:		gPriv->SetLoDamage( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_HIDAMAGE:		gPriv->SetHiDamage( static_cast<SI16>( encaps.toInt() ));		break;
			case CCP_ATWAR:
				gPriv->SetWar( encaps.toBool() );
				Movement->CombatWalk( gPriv );
				break;
			case CCP_SPELLCAST:		gPriv->SetSpellCast( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_ISCASTING:
			{
				bool isCasting = encaps.toBool();
				gPriv->SetCasting( isCasting );
				gPriv->SetJSCasting( isCasting );
			}
				break;
			case CCP_PRIV:			gPriv->SetPriv( static_cast<UI16>( encaps.toInt() ));			break;
			case CCP_TOWNPRIV:		gPriv->SetTownpriv( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_GUILDTITLE:	gPriv->SetGuildTitle( encaps.toString() );						break;
			case CCP_HAIRSTYLE:		gPriv->SetHairStyle( static_cast<UI16>( encaps.toInt() ) );		break;
			case CCP_HAIRCOLOUR:	gPriv->SetHairColour( static_cast<UI16>( encaps.toInt() ) );	break;
			case CCP_BEARDSTYLE:	gPriv->SetBeardStyle( static_cast<UI16>( encaps.toInt() ) );	break;
			case CCP_BEARDCOLOUR:	gPriv->SetBeardColour( static_cast<UI16>( encaps.toInt() ) );	break;
			case CCP_FONTTYPE:		gPriv->SetFontType( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_SAYCOLOUR:		gPriv->SetSayColour( static_cast<COLOUR>( encaps.toInt() ));	break;
			case CCP_EMOTECOLOUR:	gPriv->SetEmoteColour( static_cast<COLOUR>( encaps.toInt() ));	break;
			case CCP_ATTACKER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar *>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->SetAttacker( myChar );
				}
				else
				{
					gPriv->SetAttacker( nullptr );
				}
				break;
			case CCP_RACEGATE:		gPriv->SetRaceGate( static_cast<RACEID>( encaps.toInt() ));	break;
			case CCP_SKILLLOCK:																	break;
			case CCP_DEATHS:		gPriv->SetDeaths( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_NEXTACT:		gPriv->SetNextAct( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_CELL:			gPriv->SetCell( static_cast<SI08>( encaps.toInt() ));		break;
			case CCP_ALLMOVE:		gPriv->SetAllMove( encaps.toBool() );						break;
			case CCP_HOUSEICONS:	gPriv->SetViewHouseAsIcon( encaps.toBool() );				break;
			case CCP_SPATTACK:		gPriv->SetSpAttack( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_SPDELAY:		gPriv->SetSpDelay( static_cast<SI08>( encaps.toInt() ));	break;
			case CCP_AITYPE:		gPriv->SetNPCAiType( static_cast<SI16>( encaps.toInt() ));	break;
			case CCP_SPLIT:			gPriv->SetSplit( static_cast<UI08>( encaps.toInt() ));		break;
			case CCP_SPLITCHANCE:	gPriv->SetSplitChance( static_cast<UI08>( encaps.toInt() ));break;
			case CCP_HIRELING:		gPriv->SetCanHire( encaps.toBool() );						break;
			case CCP_TRAINER:		gPriv->SetCanTrain( encaps.toBool() );						break;
			case CCP_WEIGHT:		gPriv->SetWeight( static_cast<SI32>( encaps.toInt() ));		break;
			case CCP_SQUELCH:		gPriv->SetSquelched( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_MAGICREFLECT:	gPriv->SetTempReflected( encaps.toBool() );					break;
			case CCP_PERMMAGICREFLECT:	gPriv->SetPermReflected( encaps.toBool() );				break;
			case CCP_TAMED:			gPriv->SetTamed( encaps.toBool() );							break;
			case CCP_TAMEDHUNGERRATE: gPriv->SetTamedHungerRate( static_cast<UI16>( encaps.toInt() )); break;
			case CCP_TAMEDTHIRSTRATE: gPriv->SetTamedThirstRate( static_cast<UI16>( encaps.toInt() )); break;
			case CCP_THIRSTWILDCHANCE: gPriv->SetTamedThirstWildChance( static_cast<UI08>( encaps.toInt() )); break;
			case CCP_HUNGERWILDCHANCE: gPriv->SetTamedHungerWildChance( static_cast<UI08>( encaps.toInt() )); break;
			case CCP_FOODLIST:		gPriv->SetFood( encaps.toString() );			break;
			case CCP_MOUNTED:		gPriv->SetMounted( encaps.toBool() );			break;
			case CCP_STABLED:		gPriv->SetStabled( encaps.toBool() );			break;
			case CCP_USINGPOTION:	gPriv->SetUsingPotion( encaps.toBool() );		break;
			case CCP_STEALTH:		gPriv->SetStealth( encaps.toInt() );			break;
			case CCP_SKILLTOTAME:	gPriv->SetTaming( encaps.toInt() );				break;
			case CCP_SKILLTOPROV:	gPriv->SetProvoing( encaps.toInt() );			break;
			case CCP_SKILLTOPEACE:	gPriv->SetPeaceing( encaps.toInt() );			break;
			case CCP_POISON:		gPriv->SetPoisoned( static_cast<UI08>( encaps.toInt() ));			break;
			case CCP_POISONSTRENGTH:	gPriv->SetPoisonStrength( static_cast<UI08>( encaps.toInt() ));	break;
			case CCP_ISPOLYMORPHED:	gPriv->IsPolymorphed( encaps.toBool() );		break;
			case CCP_ISINCOGNITO:	gPriv->IsIncognito( encaps.toBool() );			break;
			case CCP_ISDISGUISED:	gPriv->IsDisguised( encaps.toBool() );			break;
			case CCP_CANRUN:		gPriv->SetRun( encaps.toBool() );				break;
			case CCP_ISGUARDED:		gPriv->SetGuarded( encaps.toBool() );			break;
			case CCP_GUARDING:
				if( *vp != JSVAL_NULL )
				{
					CBaseObject *myObj = static_cast<CBaseObject *>( encaps.toObject() );
					if( !ValidateObject( myObj ))
						break;

					gPriv->SetGuarding( myObj );
				}
				else
				{
					gPriv->SetGuarding( nullptr );
				}
				break;
			case CCP_ISMEDITATING:	gPriv->SetMeditating( encaps.toBool() );			break;
			case CCP_ISGM:			gPriv->SetGM( encaps.toBool() );					break;
			case CCP_CANBROADCAST:	gPriv->SetBroadcast( encaps.toBool() );				break;
			case CCP_SINGCLICKSER:	gPriv->SetSingClickSer( encaps.toBool() );			break;
			case CCP_NOSKILLTITLES:	gPriv->SetSkillTitles( encaps.toBool() );			break;
			case CCP_ISGMPAGEABLE:	gPriv->SetGMPageable( encaps.toBool() );			break;
			case CCP_CANSNOOP:		gPriv->SetSnoop( encaps.toBool() );					break;
			case CCP_ISCOUNSELOR:	gPriv->SetCounselor( encaps.toBool() );				break;
			case CCP_NONEEDMANA:	gPriv->SetNoNeedMana( encaps.toBool() );			break;
			case CCP_ISDISPELLABLE:	gPriv->SetDispellable( encaps.toBool() );			break;
			case CCP_NONEEDREAGS:	gPriv->SetNoNeedReags( encaps.toBool() );			break;
			case CCP_ORGID:			gPriv->SetOrgId( static_cast<UI16>( encaps.toInt() ));		break;
			case CCP_ORGSKIN:		gPriv->SetOrgSkin( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_NPCFLAG:
				gPriv->SetNPCFlag( static_cast<cNPC_FLAG>( encaps.toInt() ));
				UpdateFlag( gPriv );
				break;
			case CCP_NPCGUILD:		gPriv->SetNPCGuild( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_ISSHOP:
				if( encaps.toBool() )
				{
					MakeShop( gPriv );
				}
				else
				{
					gPriv->SetShop( false );
					CItem *tPack = nullptr;
					for( UI08 i = IL_SELLCONTAINER; i <= IL_BUYCONTAINER; ++i )
					{
						tPack = gPriv->GetItemAtLayer( static_cast<ItemLayers>( i ));
						if( ValidateObject( tPack ))
						{
							tPack->Delete();
						}
					}
					gPriv->Update();
				}
				break;
			case CCP_MAXLOYALTY:	gPriv->SetMaxLoyalty( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_LOYALTY:		gPriv->SetLoyalty( static_cast<UI16>( encaps.toInt() ));	break;
			case CCP_SHOULDSAVE:	gPriv->ShouldSave( encaps.toBool() );			break;
			case CCP_PARTYLOOTABLE:
			{
				Party *toGet = PartyFactory::GetSingleton().Get( gPriv );
				if( toGet != nullptr )
				{
					CPartyEntry *toScan = toGet->Find( gPriv );
					if( toScan != nullptr )
					{
						toScan->IsLootable( encaps.toBool() );
					}
				}
				break;
			}
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Character property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CRegionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CTownRegion *gPriv = static_cast<CTownRegion *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CREGP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CREGP_MAYOR:
				CChar *tempMayor;
				tempMayor = gPriv->GetMayor();

				if( !ValidateObject( tempMayor ))
					*vp = JSVAL_NULL;
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, tempMayor, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CREGP_RACE:				*vp = INT_TO_JSVAL( gPriv->GetRace() );					break;
			case CREGP_TAX:					*vp = INT_TO_JSVAL( gPriv->TaxedAmount() );				break;
			case CREGP_TAXRESOURCE:			*vp = INT_TO_JSVAL( gPriv->GetResourceId() );			break;
			case CREGP_CANMARK:				*vp = BOOLEAN_TO_JSVAL( gPriv->CanMark() );				break;
			case CREGP_CANRECALL:			*vp = BOOLEAN_TO_JSVAL( gPriv->CanRecall() );			break;
			case CREGP_CANGATE:				*vp = BOOLEAN_TO_JSVAL( gPriv->CanGate() );				break;
			case CREGP_CANTELEPORT:			*vp = BOOLEAN_TO_JSVAL( gPriv->CanTeleport() );			break;
			case CREGP_CANPLACEHOUSE:		*vp = BOOLEAN_TO_JSVAL( gPriv->CanPlaceHouse() );		break;
			case CREGP_ISGUARDED:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsGuarded() );			break;
			case CREGP_WORLDNUMBER:			*vp = INT_TO_JSVAL( gPriv->WorldNumber() );				break;
			case CREGP_INSTANCEID:			*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );			break;
			case CREGP_CANCASTAGGRESSIVE:	*vp = BOOLEAN_TO_JSVAL( gPriv->CanCastAggressive() );	break;
			case CREGP_ISSAFEZONE:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsSafeZone() );			break;
			case CREGP_HEALTH:				*vp = INT_TO_JSVAL( gPriv->GetHealth() );				break;
			case CREGP_ISDUNGEON:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsDungeon() );			break;
			case CREGP_CHANCEBIGORE:		*vp = INT_TO_JSVAL( gPriv->GetChanceBigOre() );			break;
			case CREGP_NUMOREPREFERENCES:	*vp = INT_TO_JSVAL( gPriv->GetNumOrePreferences() );	break;
			case CREGP_POPULATION:			*vp = INT_TO_JSVAL( gPriv->GetPopulation() );			break;
			case CREGP_MEMBERS:
				tString = JS_NewStringCopyZ( cx, gPriv->GetTownMemberSerials().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CREGP_ID:					*vp = INT_TO_JSVAL( gPriv->GetRegionNum() );			break;
			case CREGP_NUMGUARDS:			*vp = INT_TO_JSVAL( gPriv->NumGuards() );				break;
			case CREGP_SCRIPTTRIGGER:
			{
				// For backwards compatibility, get last scripttrigger from vector
				// For older worldfiles, this will be the only scripttrigger added to the vector after load
				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				UI16 lastScriptTrigger = 0;
				auto numberOfTriggers = scriptTriggers.size();
				if( numberOfTriggers > 0 )
				{
					lastScriptTrigger = scriptTriggers[numberOfTriggers - 1];
				}

				*vp = INT_TO_JSVAL( lastScriptTrigger );
				break;
			}
			case CREGP_SCRIPTTRIGGERS:
			{
				jsval scriptId;
				JSObject *scriptTriggersJS = JS_NewArrayObject( cx, 0, nullptr );

				std::vector<UI16> scriptTriggers = gPriv->GetScriptTriggers();
				for( auto i = 0; i < static_cast<int>( scriptTriggers.size() ); i++ )
				{
					scriptId = INT_TO_JSVAL( scriptTriggers[i] );
					JS_SetElement( cx, scriptTriggersJS, i, &scriptId );
				}

				*vp = OBJECT_TO_JSVAL( scriptTriggersJS );
				break;
			}
			case CREGP_TAXES:				*vp = INT_TO_JSVAL( gPriv->GetTaxes() );				break;
			case CREGP_RESERVES:			*vp = INT_TO_JSVAL( gPriv->GetReserves() );				break;
			case CREGP_APPEARANCE:			*vp = INT_TO_JSVAL( gPriv->GetAppearance() );			break;
			case CREGP_MUSIC:				*vp = INT_TO_JSVAL( gPriv->GetMusicList() );			break;
			case CREGP_WEATHER:				*vp = INT_TO_JSVAL( gPriv->GetWeather() );				break;
			case CREGP_OWNER:
				tString = JS_NewStringCopyZ( cx, gPriv->GetOwner().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}
JSBool CRegionProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CTownRegion *gPriv = static_cast<CTownRegion *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CREGP_NAME:				gPriv->SetName( encaps.toString() );						break;
			case CREGP_MAYOR:				gPriv->SetMayorSerial( static_cast<UI32>( encaps.toInt() )); break;
			case CREGP_RACE:				gPriv->SetRace( static_cast<RACEID>( encaps.toInt() ));		break;
			case CREGP_TAX:					gPriv->TaxedAmount( static_cast<SI16>( encaps.toInt() ));	break;
			case CREGP_TAXRESOURCE:			gPriv->SetResourceId( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_CANMARK:				gPriv->CanMark( encaps.toBool() );							break;
			case CREGP_CANRECALL:			gPriv->CanRecall( encaps.toBool() );						break;
			case CREGP_CANGATE:				gPriv->CanGate( encaps.toBool() );							break;
			case CREGP_CANTELEPORT:			gPriv->CanTeleport( encaps.toBool() );						break;
			case CREGP_CANPLACEHOUSE:		gPriv->CanPlaceHouse( encaps.toBool() );					break;
			case CREGP_ISGUARDED:			gPriv->IsGuarded( encaps.toBool() );						break;
			case CREGP_CANCASTAGGRESSIVE:	gPriv->CanCastAggressive( encaps.toBool() );				break;
			case CREGP_ISSAFEZONE:			gPriv->IsSafeZone( encaps.toBool() );						break;
			case CREGP_HEALTH:				gPriv->SetHealth( static_cast<SI16>( encaps.toInt() ));		break;
			case CREGP_ISDUNGEON:			gPriv->IsDungeon( encaps.toBool() );						break;
			case CREGP_CHANCEBIGORE:		gPriv->SetChanceBigOre( static_cast<UI08>( encaps.toInt() ));	break;
			case CREGP_NUMGUARDS:			gPriv->SetNumGuards( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_SCRIPTTRIGGER:
			{
				// For backwards compatibility; clears out other scripts and assigns a specific script
				UI16 scriptId = static_cast<UI16>( encaps.toInt() );
				cScript *toExecute	= JSMapping->GetScript( scriptId );
				if( toExecute == nullptr )
				{
					ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
				}
				else
				{
					gPriv->ClearScriptTriggers();
					gPriv->AddScriptTrigger( scriptId );
				}
				break;
			}
			case CREGP_SCRIPTTRIGGERS:
			{
				if( *vp != JSVAL_NULL )
				{
					UI16 scriptId = static_cast<UI16>( encaps.toInt() );
					cScript *toExecute	= JSMapping->GetScript( scriptId );
					if( toExecute == nullptr )
					{
						ScriptError( cx, oldstrutil::format( "Unable to assign script trigger - script ID (%i) not found in jse_fileassociations.scp!", scriptId ).c_str() );
					}
					else
					{
						gPriv->AddScriptTrigger( scriptId );
					}
				}
				else
				{
					// If null value was provided, clear script triggers on object
					gPriv->ClearScriptTriggers();
				}
				break; 
			}
			case CREGP_TAXES:				gPriv->SetTaxesLeft( static_cast<UI32>( encaps.toInt() ));	break;
			case CREGP_RESERVES:			gPriv->SetReserves( static_cast<UI32>( encaps.toInt() ));	break;
			case CREGP_APPEARANCE:			gPriv->SetAppearance( static_cast<WorldType>( encaps.toInt() )); break;
			case CREGP_MUSIC:				gPriv->SetMusicList( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_WEATHER:				gPriv->SetWeather( static_cast<WEATHID>( encaps.toInt() ));	break;
			case CREGP_WORLDNUMBER:			gPriv->WorldNumber( static_cast<UI08>( encaps.toInt() ));	break;
			case CREGP_INSTANCEID:			gPriv->SetInstanceId( static_cast<UI16>( encaps.toInt() ));	break;
			case CREGP_MEMBERS:
				break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Region property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}
JSBool CSpawnRegionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CSpawnRegion *gPriv = static_cast<CSpawnRegion *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CSPAWNREGP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->GetName().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CSPAWNREGP_REGIONNUM:				*vp = INT_TO_JSVAL( gPriv->GetRegionNum() );		break;
			case CSPAWNREGP_ITEM:
			case CSPAWNREGP_ITEMLIST:
			{
				// This could potentially be a list of item ids - let's convert it to a comma-separated string!
				auto itemList = gPriv->GetItem();
				std::string s;
				for( const auto &piece : itemList )
				{
					if( s.empty() )
					{
						s += piece;
					}
					else
					{
						s += ( "," + piece );
					}
				}
				tString = JS_NewStringCopyZ( cx, s.c_str() );
				*vp = STRING_TO_JSVAL( tString );
			}
				break;
			case CSPAWNREGP_NPC:
			case CSPAWNREGP_NPCLIST:
			{
				// This could potentially be a list of NPC ids - let's convert it to a comma-separated string!
				auto  npcList = gPriv->GetNPC();
				std::string s;
				for( const auto &piece : npcList )
				{
					if( s.empty() )
					{
						s += piece;
					}
					else
					{
						s += ( "," + piece );
					}
				}
				tString = JS_NewStringCopyZ( cx, s.c_str() );
				*vp = STRING_TO_JSVAL( tString );
			}
				break;
			case CSPAWNREGP_ITEMCOUNT:				*vp = INT_TO_JSVAL( gPriv->GetCurrentItemAmt() );				break;
			case CSPAWNREGP_NPCCOUNT:				*vp = INT_TO_JSVAL( gPriv->GetCurrentCharAmt() );				break;
			case CSPAWNREGP_MAXITEMS:				*vp = INT_TO_JSVAL( gPriv->GetMaxItemSpawn() );					break;
			case CSPAWNREGP_MAXNPCS:				*vp = INT_TO_JSVAL( gPriv->GetMaxCharSpawn() );					break;
			case CSPAWNREGP_MINTIME:				*vp = INT_TO_JSVAL( gPriv->GetMinTime() );						break;
			case CSPAWNREGP_MAXTIME:				*vp = INT_TO_JSVAL( gPriv->GetMaxTime() );						break;
			case CSPAWNREGP_ONLYOUTSIDE:			*vp = BOOLEAN_TO_JSVAL( gPriv->GetOnlyOutside() );				break;
			case CSPAWNREGP_ISSPAWNER:				*vp = BOOLEAN_TO_JSVAL( gPriv->IsSpawner() );					break;
			case CSPAWNREGP_X1:						*vp = INT_TO_JSVAL( gPriv->GetX1() );							break;
			case CSPAWNREGP_Y1:						*vp = INT_TO_JSVAL( gPriv->GetY1() );							break;
			case CSPAWNREGP_X2:						*vp = INT_TO_JSVAL( gPriv->GetX2() );							break;
			case CSPAWNREGP_Y2:						*vp = INT_TO_JSVAL( gPriv->GetY2() );							break;
			case CSPAWNREGP_DEFZ:					*vp = INT_TO_JSVAL( gPriv->GetDefZ() );							break;
			case CSPAWNREGP_PREFZ:					*vp = INT_TO_JSVAL( gPriv->GetPrefZ() );						break;
			case CSPAWNREGP_WORLD:					*vp = INT_TO_JSVAL( gPriv->WorldNumber() );						break;
			case CSPAWNREGP_INSTANCEID:				*vp = INT_TO_JSVAL( gPriv->GetInstanceId() );					break;
			case CSPAWNREGP_CALL:					*vp = INT_TO_JSVAL( gPriv->GetCall() );							break;
			default:
				break;
		}
	}

	return JS_TRUE;
}
JSBool CSpawnRegionProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CSpawnRegion *gPriv = static_cast<CSpawnRegion *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CSPAWNREGP_NAME:				gPriv->SetName( encaps.toString() );							break;
			case CSPAWNREGP_ITEM:				gPriv->SetItem( encaps.toString() );							break;
			case CSPAWNREGP_ITEMLIST:			gPriv->SetItemList( encaps.toString() );						break;
			case CSPAWNREGP_NPC:				gPriv->SetNPC( encaps.toString() );								break;
			case CSPAWNREGP_NPCLIST:			gPriv->SetNPCList( encaps.toString() );							break;
			case CSPAWNREGP_REGIONNUM:			gPriv->SetRegionNum( static_cast<UI16>( encaps.toInt() ));		break;
			case CSPAWNREGP_MAXITEMS:			gPriv->SetMaxItemSpawn( encaps.toInt() );						break;
			case CSPAWNREGP_MAXNPCS:			gPriv->SetMaxCharSpawn( encaps.toInt() );						break;
			case CSPAWNREGP_MINTIME:			gPriv->SetMinTime( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_MAXTIME:			gPriv->SetMaxTime( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_ONLYOUTSIDE:		gPriv->SetOnlyOutside( encaps.toBool() );						break;
			case CSPAWNREGP_ISSPAWNER:			gPriv->IsSpawner( encaps.toBool() );							break;
			case CSPAWNREGP_X1:					gPriv->SetX1( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_Y1:					gPriv->SetY1( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_X2:					gPriv->SetX2( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_Y2:					gPriv->SetY2( static_cast<SI16>( encaps.toInt() ));				break;
			case CSPAWNREGP_DEFZ:				gPriv->SetDefZ( static_cast<SI08>( encaps.toInt() ));			break;
			case CSPAWNREGP_PREFZ:				gPriv->SetPrefZ( static_cast<SI08>( encaps.toInt() ));			break;
			case CSPAWNREGP_WORLD:				gPriv->WorldNumber( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_INSTANCEID:			gPriv->SetInstanceId( static_cast<UI08>( encaps.toInt() ));		break;
			case CSPAWNREGP_CALL:				gPriv->SetCall( static_cast<UI16>( encaps.toInt() ));			break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting SpawnRegion property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CGuildProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CGuild *gPriv = static_cast<CGuild *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CGP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->Name().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGP_TYPE:			*vp = INT_TO_JSVAL( gPriv->Type() );		break;
			case CGP_MASTER:
				CChar *gMaster;
				gMaster = CalcCharObjFromSer( gPriv->Master() );

				if( !ValidateObject( gMaster ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, gMaster, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myChar );
				}
				break;
			case CGP_STONE:
				CItem *gStone;
				gStone = CalcItemObjFromSer( gPriv->Stone() );

				if( !ValidateObject( gStone ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					// Otherwise Acquire an object
					JSObject *myItem = JSEngine->AcquireObject( IUE_ITEM, gStone, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myItem );
				}
				break;
			case CGP_NUMMEMBERS:	*vp = INT_TO_JSVAL( gPriv->NumMembers() );	break;
			case CGP_NUMRECRUITS:	*vp = INT_TO_JSVAL( gPriv->NumRecruits() );	break;
			case CGP_MEMBERS:
			case CGP_RECRUITS:
				break;
			case CGP_CHARTER:
				tString = JS_NewStringCopyZ( cx, gPriv->Charter().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGP_ABBREVIATION:
				tString = JS_NewStringCopyZ( cx, gPriv->Abbreviation().c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CGP_WEBPAGE:
				tString = JS_NewStringCopyZ( cx, gPriv->Webpage().c_str() );
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
	CGuild *gPriv = static_cast<CGuild *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CGP_NAME:				gPriv->Name( encaps.toString() );						break;
			case CGP_TYPE:				gPriv->Type( static_cast<GuildType>( encaps.toInt() ));	break;
			case CGP_MASTER:
				if( *vp != JSVAL_NULL )
				{
					CChar *myChar = static_cast<CChar*>( encaps.toObject() );
					if( !ValidateObject( myChar ))
						break;

					gPriv->Master(( *myChar ));
				}
				else
				{
					gPriv->Master( INVALIDSERIAL );
				}
				break;
			case CGP_STONE:
				if( *vp != JSVAL_NULL )
				{
					CItem *myItem = static_cast<CItem *>( encaps.toObject() );
					if( !ValidateObject( myItem ))
						break;

					gPriv->Stone(( *myItem ));
				}
				else
				{
					gPriv->Stone( INVALIDSERIAL );
				}
				break;
			case CGP_MEMBERS:
			case CGP_RECRUITS:
				break;
			case CGP_CHARTER:			gPriv->Charter( encaps.toString() );				break;
			case CGP_ABBREVIATION:		gPriv->Abbreviation( encaps.toString().c_str() );	break;
			case CGP_WEBPAGE:			gPriv->Webpage( encaps.toString() );				break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Guild property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}
JSBool CRaceProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CRace *gPriv = static_cast<CRace *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		UI08 TempRace = 0;
		switch( JSVAL_TO_INT( id ))
		{
			case CRP_ID:
				for( TempRace = 0; TempRace < Races->Count(); ++TempRace )
				{
					if( Races->Race( TempRace ) == gPriv )
					{
						*vp = INT_TO_JSVAL( TempRace );
						break;
					}
				}
				break;
			case CRP_NAME:
				tString = JS_NewStringCopyZ( cx, gPriv->Name().c_str() );
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
			case CRP_POISONRESISTANCE:	JS_NewNumberValue( cx, gPriv->PoisonResistance(), vp ); break;
			case CRP_MAGICRESISTANCE:	JS_NewNumberValue( cx, gPriv->MagicResistance(), vp );  break;
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
	CRace *gPriv = static_cast<CRace *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CRP_NAME:				gPriv->Name( encaps.toString() );						break;
			case CRP_WEAKTOWEATHER:
				break;
			case CRP_REQUIRESBEARD:		gPriv->RequiresBeard( encaps.toBool() );				break;
			case CRP_REQUIRESNOBEARD:	gPriv->NoBeard( encaps.toBool() );						break;
			case CRP_ISPLAYERRACE:		gPriv->IsPlayerRace( encaps.toBool() );					break;
			case CRP_GENDERRESTRICT:	gPriv->GenderRestriction( static_cast<GENDER>( encaps.toInt() ));		break;
			case CRP_ARMOURCLASS:		gPriv->ArmourClassRestriction( static_cast<UI08>( encaps.toInt() ));	break;
			case CRP_LANGUAGESKILLMIN:	gPriv->LanguageMin( static_cast<UI16>( encaps.toInt() ));				break;
			case CRP_SKILLADJUSTMENT:
				break;
			case CRP_POISONRESISTANCE:	gPriv->PoisonResistance( encaps.toFloat() );					break;
			case CRP_MAGICRESISTANCE:	gPriv->MagicResistance( encaps.toFloat() );						break;
			case CRP_VISIBLEDISTANCE:	gPriv->VisibilityRange( static_cast<SI08>( encaps.toInt() ));	break;
			case CRP_NIGHTVISION:		gPriv->NightVision( static_cast<UI08>( encaps.toInt() ));		break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it!
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			Console.Warning( oldstrutil::format( "Script context lost after setting Race property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CSocketProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CSocket *gPriv = static_cast<CSocket *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CSOCKP_ACCOUNT:
				break;
			case CSOCKP_CURRENTCHAR:
			{
				CChar *mChar = static_cast<CChar *>( encaps.toObject() );
				if( ValidateObject( mChar ))
				{
					gPriv->CurrcharObj( mChar );
				}
			}
				break;
			case CSOCKP_IDLETIMEOUT:
				break;
			case CSOCKP_WASIDLEWARNED:		gPriv->WasIdleWarned( encaps.toBool() );				break;
			case CSOCKP_TEMPINT:			gPriv->TempInt( encaps.toInt() );						break;
			case CSOCKP_TEMPINT2:			gPriv->TempInt2( encaps.toInt() );						break; // Reserved for JS usage
			case CSOCKP_TEMPOBJ:
				if( *vp == JSVAL_NULL )
				{
					gPriv->TempObj( nullptr );
				}
				else
				{
					gPriv->TempObj( static_cast<CBaseObject *>( encaps.toObject() ));
				}
				break;
			case CSOCKP_TEMPOBJ2: // Reserved for JS usage
				if( *vp == JSVAL_NULL )
				{
					gPriv->TempObj2( nullptr );
				}
				else
				{
					gPriv->TempObj2( static_cast<CBaseObject *>( encaps.toObject() ));
				}
				break;
			case CSOCKP_BUFFER:
				break;
			case CSOCKP_XTEXT:				gPriv->XText( encaps.toString() );						break;
			case CSOCKP_XTEXT2:				gPriv->XText2( encaps.toString() );						break;
			case CSOCKP_CLICKZ:				gPriv->ClickZ( static_cast<SI08>( encaps.toInt() ));	break;
			case CSOCKP_ADDID:
				break;
			case CSOCKP_NEWCLIENT:			gPriv->NewClient( encaps.toBool() );					break;
			case CSOCKP_FIRSTPACKET:		gPriv->FirstPacket( encaps.toBool() );					break;
			case CSOCKP_CRYPTCLIENT:		gPriv->CryptClient( encaps.toBool() );					break;
			case CSOCKP_CLIENTIP:
				break;
			case CSOCKP_WALKSEQUENCE:		gPriv->WalkSequence( static_cast<SI16>( encaps.toInt() ));		break;
			case CSOCKP_CURRENTSPELLTYPE:	gPriv->CurrentSpellType( static_cast<SI08>( encaps.toInt() ));	break;
			case CSOCKP_LOGGING:			gPriv->Logging( encaps.toBool() );								break;
			case CSOCKP_BYTESSENT:
			case CSOCKP_BYTESRECEIVED:
				break;
			case CSOCKP_TARGETOK:			gPriv->TargetOK( encaps.toBool() );									break;
			case CSOCKP_CLICKX:				gPriv->ClickX( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_CLICKY:				gPriv->ClickY( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPX:			gPriv->PickupX( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPY:			gPriv->PickupY( static_cast<SI16>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPZ:			gPriv->PickupZ( static_cast<SI08>( encaps.toInt() ));				break;
			case CSOCKP_PICKUPSPOT:			gPriv->PickupSpot( static_cast<PickupLocations>( encaps.toInt() ));	break;
			case CSOCKP_PICKUPSERIAL:
				break;
			case CSOCKP_LANGUAGE:			gPriv->Language( static_cast<UnicodeTypes>( encaps.toInt() ));		break;
			case CSOCKP_CLIENTMAJORVER:		gPriv->ClientVersionMajor( static_cast<UI08>( encaps.toInt() ));	break;
			case CSOCKP_CLIENTMINORVER:		gPriv->ClientVersionMinor( static_cast<UI08>( encaps.toInt() ));	break;
			case CSOCKP_CLIENTSUBVER:		gPriv->ClientVersionSub( static_cast<UI08>( encaps.toInt() ));		break;
			case CSOCKP_CLIENTLETTERVER:	gPriv->ClientVersionLetter( static_cast<UI08>( encaps.toInt() ));	break;
			case CSOCKP_CLIENTTYPE:			gPriv->ClientType( static_cast<ClientTypes>( encaps.toInt() ));		break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Socket property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CSocketProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CSocket *gPriv = static_cast<CSocket *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		CChar *myChar;
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CSOCKP_ACCOUNT:
			{
				CAccountBlock_st *accountBlock = &gPriv->GetAccount();
				if( accountBlock == nullptr )
				{
					*vp = JSVAL_NULL;
				}
				else
				{	// Otherwise Acquire an object
					JSObject *accountObj	= JSEngine->AcquireObject( IUE_ACCOUNT, accountBlock, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( accountObj );
				}
				break;
			}
			case CSOCKP_CURRENTCHAR:
			{
				myChar = gPriv->CurrcharObj();
				if( !ValidateObject( myChar ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myObj		= JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			}
			case CSOCKP_IDLETIMEOUT:
				break;
			case CSOCKP_WASIDLEWARNED:		*vp = BOOLEAN_TO_JSVAL( gPriv->WasIdleWarned() );		break;
			case CSOCKP_TEMPINT:			*vp = INT_TO_JSVAL( gPriv->TempInt() );					break;
			case CSOCKP_TEMPINT2:			*vp = INT_TO_JSVAL( gPriv->TempInt2() );				break;
			case CSOCKP_TEMPOBJ:
			{
				CBaseObject *mObj	= gPriv->TempObj();
				if( !ValidateObject( mObj ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myObj = nullptr;
					if( mObj->CanBeObjType( OT_ITEM ))
					{
						myObj = JSEngine->AcquireObject( IUE_ITEM, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					else
					{
						myObj = JSEngine->AcquireObject( IUE_CHAR, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				break;
			}
			case CSOCKP_TEMPOBJ2:
			{
				CBaseObject *mObj	= gPriv->TempObj2();
				if( !ValidateObject( mObj ))
				{
					*vp = JSVAL_NULL;
				}
				else
				{
					JSObject *myObj = nullptr;
					if( mObj->CanBeObjType( OT_ITEM ))
					{
						myObj = JSEngine->AcquireObject( IUE_ITEM, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					else
					{
						myObj = JSEngine->AcquireObject( IUE_CHAR, mObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					}
					*vp = OBJECT_TO_JSVAL( myObj );
				}
			}
				break;
			case CSOCKP_BUFFER:
				break;
			case CSOCKP_XTEXT:				
				tString = JS_NewStringCopyZ( cx, gPriv->XText().c_str() );
				*vp = STRING_TO_JSVAL( tString );	break;
			case CSOCKP_XTEXT2:				
				tString = JS_NewStringCopyZ( cx, gPriv->XText2().c_str() );
				*vp = STRING_TO_JSVAL( tString );	break;
			case CSOCKP_CLICKZ:				*vp = INT_TO_JSVAL( gPriv->ClickZ() );					break;
			case CSOCKP_ADDID:
				break;
			case CSOCKP_NEWCLIENT:			*vp = BOOLEAN_TO_JSVAL( gPriv->NewClient() );			break;
			case CSOCKP_FIRSTPACKET:		*vp = BOOLEAN_TO_JSVAL( gPriv->FirstPacket() );			break;
			case CSOCKP_CRYPTCLIENT:		*vp = BOOLEAN_TO_JSVAL( gPriv->CryptClient() );			break;
			case CSOCKP_CLIENTIP:
				break;
			case CSOCKP_WALKSEQUENCE:		*vp = INT_TO_JSVAL( gPriv->WalkSequence() );			break;
			case CSOCKP_CURRENTSPELLTYPE:	*vp = INT_TO_JSVAL( gPriv->CurrentSpellType() );		break;
			case CSOCKP_LOGGING:			*vp = BOOLEAN_TO_JSVAL( gPriv->Logging() );				break;
			case CSOCKP_BYTESSENT:			*vp = INT_TO_JSVAL( gPriv->BytesSent() );				break;
			case CSOCKP_BYTESRECEIVED:		*vp = INT_TO_JSVAL( gPriv->BytesReceived() );			break;
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
				SERIAL mySerial		= gPriv->GetDWord( 7 );
				// Item
				if( mySerial >= BASEITEMSERIAL )
				{
					CItem *myItem = CalcItemObjFromSer( mySerial );

					if( !ValidateObject( myItem ))
					{
						*vp = JSVAL_NULL;
						return JS_TRUE;
					}

					JSObject *myObj = JSEngine->AcquireObject( IUE_ITEM, myItem, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
					*vp = OBJECT_TO_JSVAL( myObj );
				}
				// Char
				else
				{
					CChar *myChar = CalcCharObjFromSer( mySerial );

					if( !ValidateObject( myChar ))
					{
						*vp = JSVAL_NULL;
						return JS_TRUE;
					}

					JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
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

JSBool CSkillsProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	JSEncapsulate myClass( cx, obj );
	CChar *myChar = static_cast<CChar*>( myClass.toObject() );

	if( !ValidateObject( myChar ))
		return JS_FALSE;

	UI08 skillId		= static_cast<UI08>( JSVAL_TO_INT( id ));

	if( myClass.ClassName() == "UOXSkills" )
	{
		*vp = INT_TO_JSVAL( myChar->GetSkill( skillId ));
	}
	else if( myClass.ClassName() == "UOXBaseSkills" )
	{
		*vp = INT_TO_JSVAL( myChar->GetBaseSkill( skillId ));
	}
	else if( myClass.ClassName() == "UOXSkillsUsed" )
	{
		*vp = BOOLEAN_TO_JSVAL( myChar->SkillUsed( skillId ));
	}
	else if( myClass.ClassName() == "UOXSkillsLock" )
	{
		*vp = INT_TO_JSVAL( static_cast<UI08>( myChar->GetSkillLock( skillId )));
	}

	return JS_TRUE;
}

JSBool CSkillsProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	JSEncapsulate myClass( cx, obj );
	CChar *myChar = static_cast<CChar*>( myClass.toObject() );

	if( !ValidateObject( myChar ))
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	UI08 skillId		= static_cast<UI08>( JSVAL_TO_INT( id ));
	SI16 newSkillValue	= static_cast<SI16>( encaps.toInt() );
	UI08 i				= 0;

	if( myClass.ClassName() == "UOXSkills" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetSkill( newSkillValue, i );
			}
		}
		else
		{
			myChar->SetSkill( newSkillValue, skillId );
		}
	}
	else if( myClass.ClassName() == "UOXBaseSkills" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetBaseSkill( newSkillValue, i );
				Skills->UpdateSkillLevel( myChar, i );
			}
		}
		else
		{
			myChar->SetBaseSkill( newSkillValue, skillId );
			Skills->UpdateSkillLevel( myChar, skillId );
		}
	}
	else if( myClass.ClassName() == "UOXSkillsUsed" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SkillUsed( encaps.toBool(), i );
			}
		}
		else
		{
			myChar->SkillUsed( encaps.toBool(), skillId );
		}
	}
	else if( myClass.ClassName() == "UOXSkillsLock" )
	{
		if( skillId == ALLSKILLS )
		{
			for( i = 0; i < ALLSKILLS; ++i )
			{
				myChar->SetSkillLock( static_cast<SkillLock>( newSkillValue ), i );
			}
		}
		else
		{
			myChar->SetSkillLock( static_cast<SkillLock>( newSkillValue ), skillId );
		}
	}

	if( !myChar->IsNpc() )
	{
		CSocket *toFind = myChar->GetSocket();
		if( toFind != nullptr )
		{
			if( skillId == ALLSKILLS )
			{
				for( i = 0; i < ALLSKILLS; ++i )
				{
					toFind->UpdateSkill( i );
				}
			}
			else
			{
				toFind->UpdateSkill( skillId );
			}
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Skill property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CGumpDataProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	SEGumpData_st *gPriv = static_cast<SEGumpData_st *>( JS_GetPrivate( cx, obj ));

	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CGumpData_ID:
				*vp = INT_TO_JSVAL( gPriv->nIDs.size() );
				break;
			case CGumpData_Button:
				*vp = INT_TO_JSVAL( gPriv->nButtons.size() );
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CAccountProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CAccountBlock_st *myAccount = static_cast<CAccountBlock_st *>( JS_GetPrivate( cx, obj ));
	if( myAccount == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		JSString *tString = nullptr;
		switch( JSVAL_TO_INT( id ))
		{
			case CACCOUNT_ID:	*vp = INT_TO_JSVAL( myAccount->wAccountIndex );		break;
			case CACCOUNT_USERNAME:
				tString = JS_NewStringCopyZ( cx, ( myAccount->sUsername ).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CACCOUNT_PASSWORD: // NO.
			case CACCOUNT_PATH:		// Nah.
				break;
			case CACCOUNT_FLAGS: *vp = INT_TO_JSVAL(( myAccount->wFlags ).to_ulong() );	break;
			case CACCOUNT_COMMENT:
				tString = JS_NewStringCopyZ( cx, ( myAccount->sContact ).c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			case CACCOUNT_TIMEBAN: *vp = INT_TO_JSVAL( myAccount->wTimeBan );				break;
			case CACCOUNT_FIRSTLOGIN: *vp = INT_TO_JSVAL( myAccount->wFirstLogin );			break;
			case CACCOUNT_TOTALPLAYTIME: *vp = INT_TO_JSVAL( myAccount->wTotalPlayTime );	break;
			case CACCOUNT_CHARACTER1:
			{
				if( myAccount->dwCharacters[0] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[0];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER2:
			{
				if( myAccount->dwCharacters[1] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[1];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER3:
			{
				if( myAccount->dwCharacters[2] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[2];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER4:
			{
				if( myAccount->dwCharacters[3] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[3];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER5:
			{
				if( myAccount->dwCharacters[4] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[4];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar	= JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER6:
			{
				if( myAccount->dwCharacters[5] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[5];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CHARACTER7:
			{
				if( myAccount->dwCharacters[6] != INVALIDSERIAL )
				{
					CBaseObject *TempObj = myAccount->lpCharacters[6];

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_CURRENTCHAR:
			{
				if( myAccount->dwInGame != INVALIDSERIAL )
				{
					CChar *TempObj = CalcCharObjFromSer( myAccount->dwInGame );

					if( !ValidateObject( TempObj ))
					{
						*vp = JSVAL_NULL;
					}
					else
					{
						// Otherwise Acquire an object
						JSObject *myChar = JSEngine->AcquireObject( IUE_CHAR, TempObj, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
						*vp = OBJECT_TO_JSVAL( myChar );
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;
			}
			case CACCOUNT_LASTIP:
			{
				std::string ipString = oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0xFF000000 ) >> 24 )) 
					+ "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x00FF0000 ) >> 16 ))
					+ "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x0000FF00 ) >> 8 ))
					+ "." + oldstrutil::number( static_cast<SI32>(( myAccount->dwLastIP & 0x000000FF ) % 256 ));
				tString = JS_NewStringCopyZ( cx, ipString.c_str() );
				*vp = STRING_TO_JSVAL( tString );
				break;
			}
			case CACCOUNT_BANNED:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_BANNED ));		break;
			case CACCOUNT_SUSPENDED:		*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_SUSPENDED ));	break;
			case CACCOUNT_PUBLIC:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_PUBLIC ));		break;
			case CACCOUNT_ONLINE:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_ONLINE ));		break;
			case CACCOUNT_CHARSLOT1BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER1 ));	break;
			case CACCOUNT_CHARSLOT2BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER2 ));	break;
			case CACCOUNT_CHARSLOT3BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER3 ));	break;
			case CACCOUNT_CHARSLOT4BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER4 ));	break;
			case CACCOUNT_CHARSLOT5BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER5 ));	break;
			case CACCOUNT_CHARSLOT6BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER6 ));	break;
			case CACCOUNT_CHARSLOT7BLOCKED:	*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_CHARACTER7 ));	break;
			case CACCOUNT_YOUNG:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_YOUNG ));		break;
			case CACCOUNT_UNUSED10:			*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_UNUSED10 ));	break;
			case CACCOUNT_SEER:				*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_SEER ));		break;
			case CACCOUNT_COUNSELOR:		*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_COUNSELOR ));	break;
			case CACCOUNT_GM:				*vp = BOOLEAN_TO_JSVAL( myAccount->wFlags.test( AB_FLAGS_GM ));			break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool CAccountProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CAccountBlock_st *myAccount = static_cast<CAccountBlock_st *>( JS_GetPrivate( cx, obj ));
	if( myAccount == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );

	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CACCOUNT_ID: 
			case CACCOUNT_USERNAME:
			case CACCOUNT_FLAGS:
			case CACCOUNT_PATH:
			case CACCOUNT_CHARACTER1:
			case CACCOUNT_CHARACTER2:
			case CACCOUNT_CHARACTER3:
			case CACCOUNT_CHARACTER4:
			case CACCOUNT_CHARACTER5:
			case CACCOUNT_CHARACTER6:
			case CACCOUNT_CHARACTER7:
			case CACCOUNT_CURRENTCHAR:
			case CACCOUNT_LASTIP:
			case CACCOUNT_FIRSTLOGIN:
				break;
			case CACCOUNT_TOTALPLAYTIME:		myAccount->wTotalPlayTime = static_cast<UI32>( encaps.toInt() );	break;
			case CACCOUNT_PASSWORD:
			{
				std::string newPass = encaps.toString();
				if( newPass.length() > 3 )
				{
					myAccount->sPassword = newPass;
				}
				else
				{
					return JS_FALSE;
				}
				break;
			}
			case CACCOUNT_COMMENT: 
				myAccount->sContact = encaps.toString();
				break;
			case CACCOUNT_TIMEBAN:
			{
				UI32 timeBan = static_cast<UI32>( encaps.toInt() );
				if( timeBan > 0 )
				{
					myAccount->wFlags.set( AB_FLAGS_BANNED, true );
					myAccount->wTimeBan = GetMinutesSinceEpoch() + timeBan;
				}
				else
				{
					myAccount->wTimeBan = 0;
				}
				break;
			}
			case CACCOUNT_BANNED:			myAccount->wFlags.set( AB_FLAGS_BANNED, encaps.toBool() );		break;
			case CACCOUNT_SUSPENDED:		myAccount->wFlags.set( AB_FLAGS_SUSPENDED, encaps.toBool() );	break;
			case CACCOUNT_PUBLIC:			myAccount->wFlags.set( AB_FLAGS_PUBLIC, encaps.toBool() );		break;
			case CACCOUNT_CHARSLOT1BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER1, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT2BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER2, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT3BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER3, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT4BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER4, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT5BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER5, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT6BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER6, encaps.toBool() );	break;
			case CACCOUNT_CHARSLOT7BLOCKED:	myAccount->wFlags.set( AB_FLAGS_CHARACTER7, encaps.toBool() );	break;
			case CACCOUNT_YOUNG:			myAccount->wFlags.set( AB_FLAGS_YOUNG, encaps.toBool() );		break;
			case CACCOUNT_UNUSED10:			myAccount->wFlags.set( AB_FLAGS_UNUSED10, encaps.toBool() );	break;
			case CACCOUNT_SEER:				myAccount->wFlags.set( AB_FLAGS_SEER, encaps.toBool() );		break;
			case CACCOUNT_COUNSELOR:		myAccount->wFlags.set( AB_FLAGS_COUNSELOR, encaps.toBool() );	break;
			case CACCOUNT_GM:				myAccount->wFlags.set( AB_FLAGS_GM, encaps.toBool() );			break;
			default:
				break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Account property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CConsoleProps_getProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, jsval id, jsval *vp )
{
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CCONSOLE_MODE:		*vp = INT_TO_JSVAL( Console.CurrentMode() );	break;
			case CCONSOLE_LOGECHO:	*vp = INT_TO_JSVAL( Console.LogEcho() );		break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CConsoleProps_setProperty( JSContext *cx, [[maybe_unused]] JSObject *obj, jsval id, jsval *vp )
{
	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CCONSOLE_MODE:		Console.CurrentMode( encaps.toInt() );		break;
			case CCONSOLE_LOGECHO:	Console.LogEcho( encaps.toBool() );			break;
			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool CScriptSectionProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	CScriptSection *gPriv = static_cast<CScriptSection *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CSS_NUMTAGS:		*vp = INT_TO_JSVAL( gPriv->NumEntries() );			break;
			case CSS_ATEND:			*vp = BOOLEAN_TO_JSVAL( gPriv->AtEnd() );			break;
			case CSS_ATENDTAGS:		*vp = BOOLEAN_TO_JSVAL( gPriv->AtEndTags() );		break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CScriptSectionProps_setProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, [[maybe_unused]] jsval id, [[maybe_unused]] jsval *vp )
{
	/*		JSEncapsulate encaps( cx, vp );
		if( JSVAL_IS_INT( id ))
		{
			switch( JSVAL_TO_INT( id ))
			{
			default:
				break;
			}
		}*/
		return JS_TRUE;
}

JSBool CResourceProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	MapResource_st *gPriv = static_cast<MapResource_st*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CRESP_LOGAMT:				gPriv->logAmt	= encaps.toInt();			break;
			case CRESP_LOGTIME:
			{
				gPriv->logTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
				break;
			}
			case CRESP_OREAMT:				gPriv->oreAmt	= encaps.toInt();			break;
			case CRESP_ORETIME:				//gPriv->oreTime	= encaps.toInt();			break;
			{
				gPriv->oreTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
				break;
			}
			case CRESP_FISHAMT:				gPriv->fishAmt	= encaps.toInt();			break;
			case CRESP_FISHTIME:			//gPriv->fishTime	= encaps.toInt();			break;
			{
				gPriv->fishTime = static_cast<UI32>( encaps.toInt() * 1000 ); break;
				break;
			}
			default:																	break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Resource property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CResourceProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	MapResource_st *gPriv = static_cast<MapResource_st*>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CRESP_LOGAMT:				*vp = INT_TO_JSVAL( gPriv->logAmt );			break;
			case CRESP_LOGTIME:				JS_NewNumberValue( cx, gPriv->logTime, vp );	break;
			case CRESP_OREAMT:				*vp = INT_TO_JSVAL( gPriv->oreAmt );			break;
			case CRESP_ORETIME:				JS_NewNumberValue( cx, gPriv->oreTime, vp );	break;
			case CRESP_FISHAMT:				*vp = INT_TO_JSVAL( gPriv->fishAmt );			break;
			case CRESP_FISHTIME:			JS_NewNumberValue( cx, gPriv->fishTime, vp );	break;
			default:																		break;
		}
	}
	return JS_TRUE;
}

JSBool CPartyProps_setProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	Party *gPriv = static_cast<Party *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	// Keep track of original script that's executing
	auto origScript = JSMapping->GetScript( JS_GetGlobalObject( cx ));
	auto origScriptID = JSMapping->GetScriptId( JS_GetGlobalObject( cx ));

	JSEncapsulate encaps( cx, vp );
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CPARTYP_LEADER:
			{
				if( encaps.ClassName() == "UOXChar" || encaps.ClassName() == "UOXSocket" )
				{
					CChar *newLeader = nullptr;
					if( encaps.ClassName() == "UOXChar" )
					{
						newLeader = static_cast<CChar *>( encaps.toObject() );
					}
					else
					{
						CSocket *tempSock = static_cast<CSocket *>( encaps.toObject() );
						if( tempSock != nullptr )
						{
							newLeader = tempSock->CurrcharObj();
						}
					}
					if( ValidateObject( newLeader ))
					{
						gPriv->Leader( newLeader );
					}
				}
			}
				break;
			case CPARTYP_ISNPC:			gPriv->IsNPC( encaps.toBool() );				break;
			default:																	break;
		}
	}

	// Active script-context might have been lost, so restore it...
	if( origScript != JSMapping->GetScript( JS_GetGlobalObject( cx )))
	{
		// ... by calling a dummy function in original script!
		JSBool retVal = origScript->CallParticularEvent( "_restorecontext_", &id, 0, vp );
		if( retVal == JS_FALSE )
		{
			// Dummy function not found, let shard admin know!
			Console.Warning( oldstrutil::format( "Script context lost after setting Party property %u. Add 'function _restorecontext_() {}' to original script (%u) as safeguard!", JSVAL_TO_INT( id ), origScriptID ));
		}
	}

	return JS_TRUE;
}

JSBool CPartyProps_getProperty( JSContext *cx, JSObject *obj, jsval id, jsval *vp )
{
	Party *gPriv = static_cast<Party *>( JS_GetPrivate( cx, obj ));
	if( gPriv == nullptr )
		return JS_FALSE;

	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CPARTYP_LEADER:
			{
				CChar *myChar = gPriv->Leader();
				if( !ValidateObject( myChar ))
				{
					*vp = JSVAL_NULL;
					return JS_TRUE;
				}
				JSObject *myObj = JSEngine->AcquireObject( IUE_CHAR, myChar, JSEngine->FindActiveRuntime( JS_GetRuntime( cx )));
				*vp = OBJECT_TO_JSVAL( myObj );
			}
				break;
			case CPARTYP_MEMBERCOUNT:	*vp = INT_TO_JSVAL( gPriv->MemberList()->size() );			break;
			case CPARTYP_ISNPC:			*vp = BOOLEAN_TO_JSVAL( gPriv->IsNPC() );					break;
			default:																				break;
		}
	}
	return JS_TRUE;
}

JSBool CScriptProps_getProperty( [[maybe_unused]] JSContext *cx, [[maybe_unused]] JSObject *obj, jsval id, jsval *vp )
{
	if( JSVAL_IS_INT( id ))
	{
		switch( JSVAL_TO_INT( id ))
		{
			case CSCRIPT_SCRIPTID:		
				*vp = INT_TO_JSVAL( JSMapping->GetScriptId( JS_GetGlobalObject( cx )) );
				break;
			default:
				break;
		}
	}
	return JS_TRUE;
}

JSBool CSocket_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp )
{
	JSEncapsulate srcObj( cx, obj );
	CSocket *srcSock = static_cast<CSocket *>( srcObj.toObject() );
	JSEncapsulate trgObj( cx, &v );
	if( trgObj.isType( JSOT_OBJECT ))
	{
		if( srcObj.ClassName() != trgObj.ClassName() )
		{
			*bp = JS_FALSE;
		}
		else
		{
			CSocket *trgSock = static_cast<CSocket *>( trgObj.toObject() );
			*bp = ( srcSock == trgSock ) ? JS_TRUE : JS_FALSE;
		}
	}
	else
	{
		*bp = ( srcSock == nullptr && trgObj.isType( JSOT_NULL )) ? JS_TRUE : JS_FALSE;
	}
	return JS_TRUE;
}
JSBool CBaseObject_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp )
{
	JSEncapsulate srcObj( cx, obj );
	CBaseObject *src = static_cast<CBaseObject *>( srcObj.toObject() );
	if( !ValidateObject( src ))
	{
		*bp = JS_FALSE;
	}
	else
	{
		JSEncapsulate trgObj( cx, &v );
		if( trgObj.isType( JSOT_OBJECT ))
		{
			if( srcObj.ClassName() != trgObj.ClassName() )
			{
				*bp = JS_FALSE;
			}
			else
			{
				CBaseObject *trg = static_cast<CBaseObject *>( trgObj.toObject() );
				if( !ValidateObject( trg ))
				{
					*bp = JS_FALSE;
				}
				else	// both valid base objects!  Now, we'll declare equality based on SERIAL, not pointer
				{
					*bp = ( src->GetSerial() == trg->GetSerial() ) ? JS_TRUE : JS_FALSE;
				}
			}
		}
		else
		{
			*bp = ( src == nullptr && trgObj.isType( JSOT_NULL )) ? JS_TRUE : JS_FALSE;
		}
	}
	return JS_TRUE;
}
JSBool CParty_equality( JSContext *cx, JSObject *obj, jsval v, JSBool *bp )
{
	JSEncapsulate srcObj( cx, obj );
	Party *srcParty = static_cast<Party *>( srcObj.toObject() );
	JSEncapsulate trgObj( cx, &v );
	if( trgObj.isType( JSOT_OBJECT ))
	{
		if( srcObj.ClassName() != trgObj.ClassName() )
		{
			*bp = JS_FALSE;
		}
		else
		{
			Party *trgParty	= static_cast<Party *>( trgObj.toObject() );
			*bp = ( srcParty == trgParty ) ? JS_TRUE : JS_FALSE;
		}
	}
	else
	{
		*bp = ( srcParty == nullptr && trgObj.isType( JSOT_NULL )) ? JS_TRUE : JS_FALSE;
	}
	return JS_TRUE;
}
