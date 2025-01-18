#include "uox3.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "cSpawnRegion.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "scriptc.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "ObjectFactory.h"
#include "StringUtility.hpp"

using namespace std::string_literals;

cItem *Items = nullptr;

ItemTypes FindItemTypeFromTag( const std::string& strToFind );
void SetRandomName( CBaseObject *s, const std::string& namelist );

//o------------------------------------------------------------------------------------------------o
//|	Function	-	bool ApplySpawnItemSection( CSpawnItem *applyTo, const DFNTAGS tag, const SI32 ndata, const SI32 odata, const std::string &cdata )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load item data from script sections and apply to spawner objects
//o------------------------------------------------------------------------------------------------o
bool ApplySpawnItemSection( CSpawnItem *applyTo, const DFNTAGS tag, const SI32 ndata, const SI32 odata, const std::string& cdata )
{
	if( !ValidateObject( applyTo ))
		return false;

	switch( tag )
	{
		case DFNTAG_SPAWNOBJLIST:				
			applyTo->IsSectionAList( true );
			[[fallthrough]];
		case DFNTAG_SPAWNOBJ:
			applyTo->SetSpawnSection( cdata );
			return true;
		case DFNTAG_INTERVAL:
			applyTo->SetInterval( 0, static_cast<UI08>( ndata ));
			applyTo->SetInterval( 1, static_cast<UI08>( odata ));
			return true;
		default:
			break;
	}
	return false;
}

UI16 AddRandomColor( const std::string& colorlist );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	ApplyItemSection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load items from the script sections to the array
//o------------------------------------------------------------------------------------------------o
auto ApplyItemSection( CItem *applyTo, CScriptSection *toApply, std::string sectionId ) -> bool
{
	if( toApply == nullptr || !ValidateObject( applyTo ))
		return false;

	std::string cdata;
	SI32 ndata = -1, odata = -1;
	bool isSpawner = ( applyTo->GetObjType() == OT_SPAWNER );

	TAGMAPOBJECT customTag;
	std::string customTagName;
	std::string customTagStringValue;
	for( const auto &sec : toApply->collection2() )
	{
		auto tag = sec->tag;
		cdata = sec->cdata;
		odata = sec->odata;
		ndata = sec->ndata;
		
		if( isSpawner && ApplySpawnItemSection( static_cast<CSpawnItem *>( applyTo ), tag, ndata, odata, cdata ))
		{
			continue;
		}

		auto ssecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( cdata, "//" )), " " );
		switch( tag )
		{
			case DFNTAG_ARTIFACTRARITY:	applyTo->SetArtifactRarity( static_cast<SI16>( ndata ));	break;
			case DFNTAG_AMMO:
				applyTo->SetAmmoId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				if( ssecs.size() > 1 )
				{
					applyTo->SetAmmoHue( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_AMMOFX:
				applyTo->SetAmmoFX( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				if( ssecs.size() > 1 )
				{
					applyTo->SetAmmoFXHue( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					if( ssecs.size() > 2 )
					{
						applyTo->SetAmmoFXRender( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					}
				}
				break;
			case DFNTAG_AMOUNT:
				if( ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						UI16 rndAmount = static_cast<UI16>( RandomNum( ndata, odata ));
						applyTo->SetAmount( rndAmount );
					}
					else
					{
						applyTo->SetAmount( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in AMOUNT tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_DAMAGE:
			case DFNTAG_ATT:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetLoDamage( static_cast<SI16>( ndata ));
						applyTo->SetHiDamage( static_cast<SI16>( odata ));
					}
					else
					{
						applyTo->SetLoDamage( static_cast<SI16>( ndata ));
						applyTo->SetHiDamage( static_cast<SI16>( ndata ));
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in ATT/DAMAGE tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_AC:				applyTo->SetArmourClass( static_cast<UI08>( ndata ));	break;
			case DFNTAG_BASERANGE:		applyTo->SetBaseRange( static_cast<UI08>( ndata ));		break;
			case DFNTAG_CREATOR:		applyTo->SetCreator( ndata );							break;
			case DFNTAG_COLOUR:			applyTo->SetColour( static_cast<UI16>( ndata ));		break;
			case DFNTAG_COLOURLIST:		applyTo->SetColour( AddRandomColor( cdata ));			break;
			case DFNTAG_CORPSE:			applyTo->SetCorpse( ndata != 0 )		;				break;
			case DFNTAG_COLD:			applyTo->SetWeatherDamage( COLD, ndata != 0 );			break;
			case DFNTAG_ELEMENTRESIST:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetResist( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )), HEAT );
					applyTo->SetResist( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )), COLD );
					applyTo->SetResist( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )), LIGHTNING );
					applyTo->SetResist( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )), POISON );
				}
				break;
			case DFNTAG_ERBONUS:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetResist( applyTo->GetResist( HEAT ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )), HEAT );
					applyTo->SetResist( applyTo->GetResist( COLD ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )), COLD );
					applyTo->SetResist( applyTo->GetResist( LIGHTNING ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )), LIGHTNING );
					applyTo->SetResist( applyTo->GetResist( POISON ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )), POISON );
				}
				break;
			case DFNTAG_EVENT:			applyTo->SetEvent( cdata );				break;
			case DFNTAG_DAMAGEABLE:		applyTo->SetDamageable(ndata != 0 );	break;
			case DFNTAG_DEF:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), PHYSICAL );
					}
					else
					{
						applyTo->SetResist( ndata, PHYSICAL );
					}
				}
				else
					Console.Warning( oldstrutil::format( "Invalid data found in DEF tag inside item script [%s]", sectionId.c_str() ));
				break;
			case DFNTAG_DEFBONUS:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( applyTo->GetResist( PHYSICAL ) + static_cast<UI16>( RandomNum( ndata, odata )), PHYSICAL );
					}
					else
					{
						applyTo->SetResist( applyTo->GetResist( PHYSICAL ) + static_cast<UI16>( ndata ), PHYSICAL );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in DEFBONUS tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_DEX:
				if( ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetDexterity( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetDexterity( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in DEX tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_DEXADD:			applyTo->SetDexterity2( static_cast<SI16>( ndata ));					break;
			case DFNTAG_DIR:			applyTo->SetDir( static_cast<UI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( cdata, "//" )), nullptr, 0 )));			break;
			case DFNTAG_DYE:			applyTo->SetDye( ndata != 0 );				break;
			case DFNTAG_DECAY:
				if( ndata == 1 )
				{
					applyTo->SetDecayable( true );
				}
				else
				{
					applyTo->SetDecayable( false );
				}
				break;
			case DFNTAG_DISPELLABLE:	applyTo->SetDispellable( true );			break;
			case DFNTAG_DISABLED:		applyTo->SetDisabled( ndata != 0 );			break;
			case DFNTAG_DOORFLAG:		break;
			case DFNTAG_GOOD:			applyTo->SetGood( static_cast<SI16>( ndata ));			break;
			case DFNTAG_GLOW:			applyTo->SetGlow( ndata );								break;
			case DFNTAG_GLOWBC:			applyTo->SetGlowColour( static_cast<UI16>( ndata ));	break;
			case DFNTAG_GLOWTYPE:		applyTo->SetGlowEffect( static_cast<UI08>( ndata ));	break;
			case DFNTAG_GET:
			{
				std::string scriptEntry = "";
				if( ssecs.size() == 1 )
				{
					scriptEntry = cdata;
				}
				else
				{
					UI32 rndEntry = RandomNum( 0, static_cast<SI32>( ssecs.size() - 1 ));
					scriptEntry = oldstrutil::trim( oldstrutil::removeTrailing( ssecs[rndEntry], "//" ));
				}

				auto toFind = FileLookup->FindEntry( scriptEntry, items_def );
				if( toFind == nullptr )
				{
					Console.Warning( oldstrutil::format( "Invalid script entry (%s) called with GET tag, item serial 0x%X", scriptEntry.c_str(), applyTo->GetSerial() ));
				}
				else if( toFind == toApply )
				{
					Console.Warning( oldstrutil::format( "Infinite loop avoided with GET tag inside item script [%s]", scriptEntry.c_str() ));
				}
				else
				{
					ApplyItemSection( applyTo, toFind, scriptEntry );
				}
				break;
			}
			case DFNTAG_GETT2A:
			case DFNTAG_GETUOR:
			case DFNTAG_GETTD:
			case DFNTAG_GETLBR:
			case DFNTAG_GETAOS:
			case DFNTAG_GETSE:
			case DFNTAG_GETML:
			case DFNTAG_GETSA:
			case DFNTAG_GETHS:
			case DFNTAG_GETTOL:
			{
				// Inherit stats for object based on active ruleset
				bool getParent = false;
				std::string tagName = "";
				switch( cwmWorldState->ServerData()->ExpansionCoreShardEra() )
				{
					case ER_T2A:	if( tag == DFNTAG_GETT2A ) { getParent = true; tagName = "GETT2A"; }		break;
					case ER_UOR:	if( tag == DFNTAG_GETUOR ) { getParent = true; tagName = "GETUOR"; }		break;
					case ER_TD:		if( tag == DFNTAG_GETTD ) { getParent = true; tagName = "GETTD"; }			break;
					case ER_LBR:	if( tag == DFNTAG_GETLBR ) { getParent = true; tagName = "GETLBR"; }		break;
					case ER_AOS:	if( tag == DFNTAG_GETAOS ) { getParent = true; tagName = "GETAOS"; }		break;
					case ER_SE:		if( tag == DFNTAG_GETSE ) { getParent = true; tagName = "GETSE"; }			break;
					case ER_ML:		if( tag == DFNTAG_GETML ) { getParent = true; tagName = "GETML"; }			break;
					case ER_SA:		if( tag == DFNTAG_GETSA ) { getParent = true; tagName = "GETSA"; }			break;
					case ER_HS:		if( tag == DFNTAG_GETHS ) { getParent = true; tagName = "GETHS"; }			break;
					case ER_TOL:	if( tag == DFNTAG_GETTOL ) { getParent = true; tagName = "GETTOL"; }		break;
					default:
						break;
				}

				if( getParent )
				{
					std::string scriptEntry = "";
					if( ssecs.size() == 1 )
					{
						scriptEntry = cdata;
					}
					else
					{
						UI32 rndEntry = RandomNum( 0, static_cast<SI32>( ssecs.size() - 1 ));
						scriptEntry = oldstrutil::trim( oldstrutil::removeTrailing( ssecs[rndEntry], "//" ));
					}

					CScriptSection *toFind = FileLookup->FindEntry( scriptEntry, items_def );
					if( toFind == NULL )
					{
						Console.Warning( oldstrutil::format( "Invalid script entry (%s) called with %s tag, item serial 0x%X", scriptEntry.c_str(), tagName.c_str(), applyTo->GetSerial() ));
					}
					else if( toFind == toApply )
					{
						Console.Warning( oldstrutil::format( "Infinite loop avoided with %s tag inside item script [%s]", tagName.c_str(), scriptEntry.c_str() ));
					}
					else
					{
						ApplyItemSection( applyTo, toFind, scriptEntry );
					}
				}
				break;
			}
			case DFNTAG_HP:
				if( ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetHP( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetHP( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in HP tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_HIDAMAGE:		applyTo->SetHiDamage( static_cast<SI16>( ndata ));		break;
			case DFNTAG_HEAT:			applyTo->SetWeatherDamage( HEAT, ndata != 0 );			break;
			case DFNTAG_ID:				// applyTo->SetId( static_cast<UI16>( ndata ));				break;
				if( ssecs.size() == 1 )
				{
					applyTo->SetId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				else
				{
					SI32 rndEntry = RandomNum( 0, static_cast<SI32>( ssecs.size() - 1 ));
					applyTo->SetId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[rndEntry], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_INTELLIGENCE:	applyTo->SetIntelligence( static_cast<SI16>( ndata ));	break;
			case DFNTAG_INTADD:			applyTo->SetIntelligence2( static_cast<SI16>( ndata ));	break;
			case DFNTAG_LODAMAGE:		applyTo->SetLoDamage( static_cast<SI16>( ndata ));		break;
			case DFNTAG_LAYER:			applyTo->SetLayer( static_cast<ItemLayers>( ndata ));	break;
			case DFNTAG_LIGHT:			applyTo->SetWeatherDamage( LIGHT, ndata != 0 );			break;
			case DFNTAG_LIGHTNING:		applyTo->SetWeatherDamage( LIGHTNING, ndata != 0 );		break;
			case DFNTAG_MAXHP:			applyTo->SetMaxHP( static_cast<UI16>( ndata ));			break;
			case DFNTAG_MAXITEMS:		applyTo->SetMaxItems( static_cast<UI16>( ndata ));		break;
			case DFNTAG_MAXRANGE:		applyTo->SetMaxRange( static_cast<UI08>( ndata ));		break;
			case DFNTAG_MAXUSES:		applyTo->SetMaxUses( static_cast<UI16>( ndata ));		break;
			case DFNTAG_MOVABLE:		applyTo->SetMovable( static_cast<SI08>( ndata ));		break;
			case DFNTAG_MORE:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MORE, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MORE, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_MORE0:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MORE0, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE0, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE0, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE0, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MORE0, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_MORE1:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MORE1, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE1, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE1, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE1, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MORE1, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_MORE2:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MORE2, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE2, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE2, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MORE2, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MORE2, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_MOREX:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MOREX, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREX, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREX, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREX, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MOREX, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_MOREY:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MOREY, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREY, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREY, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREY, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MOREY, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_MOREZ:
				if( ssecs.size() >= 4 )
				{
					applyTo->SetTempVar( CITV_MOREZ, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREZ, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREZ, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
					applyTo->SetTempVar( CITV_MOREZ, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 )));
				}
				else
				{
					applyTo->SetTempVar( CITV_MOREZ, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_NAME:			applyTo->SetName( cdata );								break;
			case DFNTAG_NAME2:			applyTo->SetName2( cdata );								break;
			case DFNTAG_NAMELIST:		SetRandomName( applyTo, cdata );						break;
			case DFNTAG_NEWBIE:			applyTo->SetNewbie( true );								break;
			case DFNTAG_OFFSPELL:		applyTo->SetOffSpell( static_cast<SI08>( ndata ));		break;
			case DFNTAG_ORIGIN:			applyTo->SetOrigin( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->EraStringToEnum( cdata )));		break;
			case DFNTAG_POISONDAMAGE:	applyTo->SetWeatherDamage( POISON, ndata != 0 );		break;
			case DFNTAG_POISONED:		applyTo->SetPoisoned( static_cast<UI08>( ndata ));		break;
			case DFNTAG_PILEABLE:		applyTo->SetPileable( ndata != 0 );						break;
			case DFNTAG_PRIV:			applyTo->SetPriv( static_cast<UI08>( ndata ));			break;
			case DFNTAG_RANK:
				applyTo->SetRank( static_cast<SI08>( ndata ));
				if( applyTo->GetRank() <= 0 )
				{
					applyTo->SetRank( 10 );
				}
				break;
			case DFNTAG_RACE:			applyTo->SetRace( static_cast<UI16>( ndata ));			break;
			case DFNTAG_RESISTFIRE:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), HEAT );
					}
					else
					{
						applyTo->SetResist( ndata, HEAT );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTFIRE tag inside Item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESISTCOLD:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), COLD );
					}
					else
					{
						applyTo->SetResist( ndata, COLD );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTCOLD tag inside Item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESISTLIGHTNING:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), LIGHTNING );
					}
					else
					{
						applyTo->SetResist( ndata, LIGHTNING );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTLIGHTNING tag inside Item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESISTPOISON:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), POISON );
					}
					else
					{
						applyTo->SetResist( ndata, POISON );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTPOISON tag inside Item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESTOCK:	
			{
				// Apply global restock multiplier from INI to item's restock property
				auto restockMultiplier = cwmWorldState->ServerData()->GlobalRestockMultiplier();
				applyTo->SetRestock( static_cast<UI16>( floor( static_cast<UI16>( ndata ) * restockMultiplier )));
				break;
			}
			case DFNTAG_RAIN:			applyTo->SetWeatherDamage( RAIN, ndata != 0 );			break;
			case DFNTAG_SECTIONID:		applyTo->SetSectionId( cdata );							break;
			case DFNTAG_SK_MADE:		applyTo->SetMadeWith( static_cast<SI08>( ndata ));		break;
			case DFNTAG_SWINGSPEEDINCREASE:	applyTo->SetSwingSpeedIncrease( static_cast<SI16>( ndata ));		break;
			case DFNTAG_SPD:			applyTo->SetSpeed( static_cast<UI08>( ndata ));			break;
			case DFNTAG_STRENGTH:		applyTo->SetStrength( static_cast<SI16>( ndata ));		break;
			case DFNTAG_STRADD:			applyTo->SetStrength2( static_cast<SI16>( ndata ));		break;
			case DFNTAG_STEALABLE:		applyTo->SetStealable( static_cast<UI08>( ndata ));		break;
			case DFNTAG_SNOW:			applyTo->SetWeatherDamage( SNOW, ndata != 0 );			break;
			case DFNTAG_SCRIPT:			applyTo->AddScriptTrigger( static_cast<UI16>( ndata ));	break;
			case DFNTAG_SPELLS:
				if( ssecs.size() == 3 )
				{
					applyTo->SetSpell( 0, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					applyTo->SetSpell( 1, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
					applyTo->SetSpell( 2, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 )));
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in SPELLS tag inside Item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_TYPE:
				ItemTypes iType;
				iType = FindItemTypeFromTag( cdata );
				if( iType == IT_COUNT )
				{
					iType = static_cast<ItemTypes>( ndata );
				}
				if( iType < IT_COUNT )
				{
					applyTo->SetType( iType );
				}
				break;
			case DFNTAG_USESLEFT:		applyTo->SetUsesLeft( static_cast<UI16>( ndata ));			break;
			case DFNTAG_VISIBLE:		applyTo->SetVisible( static_cast<VisibleTypes>( ndata ));	break;
			case DFNTAG_VALUE:
				if( ndata >= 0 )
				{
					if( odata && odata >= 0 )
					{
						applyTo->SetBuyValue( ndata );
						applyTo->SetSellValue( odata );
					}
					else
					{
						// Only one value was specified in DFN tag - use it for both buy and sell value
						applyTo->SetBuyValue( ndata );
						applyTo->SetSellValue( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in VALUE tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_WEIGHT:
				applyTo->SetWeight( ndata );
				applyTo->SetBaseWeight( ndata ); // Let's set the base-weight as well. Primarily used for containers.
				break;
			case DFNTAG_WEIGHTMAX:		applyTo->SetWeightMax( ndata );				break;
			case DFNTAG_WIPE:			applyTo->SetWipeable( ndata != 0 );			break;
			case DFNTAG_ADDMENUITEM:
				Console.Print( cdata );
				break;
			case DFNTAG_CUSTOMSTRINGTAG:
			{
				auto count = 0;
				std::string result;
				for( auto &sec : ssecs )
				{
					if( count > 0 )
					{
						if( count == 1 )
						{
							result = oldstrutil::trim( oldstrutil::removeTrailing( sec, "//" ));
						}
						else
						{
							result = result + " " + oldstrutil::trim( oldstrutil::removeTrailing( sec, "//" ));
						}
					}
					count++;
				}
				customTagName			= oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" ));
				customTagStringValue	= result;

				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= false;
					customTag.m_StringValue	= customTagStringValue;
					customTag.m_IntValue	= static_cast<SI32>( customTag.m_StringValue.size() );
					customTag.m_ObjectType	= TAGMAP_TYPE_STRING;
					applyTo->SetTag( customTagName, customTag );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in CUSTOMSTRINGTAG tag inside Item script [%s] - Supported data format: <tagName> <text>", sectionId.c_str() ));
				}
				break;
			}
			case DFNTAG_CUSTOMINTTAG:
			{
				auto count = 0;
				std::string result;
				for( auto &sec : ssecs )
				{
					if( count > 0 )
					{
						if( count == 1 )
						{
							result = oldstrutil::trim( oldstrutil::removeTrailing( sec, "//" ));
						}
					}
					count++;
				}
				customTagName = oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" ));
				customTagStringValue = result;
				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= static_cast<SI32>( std::stoi( customTagStringValue, nullptr, 0 ));
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					applyTo->SetTag( customTagName, customTag );
					if( count > 2 )
					{
						Console.Warning( oldstrutil::format( "Multiple values detected for CUSTOMINTTAG in Item script [%s] - only first value will be used! Supported data format: <tagName> <value>", sectionId.c_str() ));
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in CUSTOMINTTAG tag in Item script [%s] - Supported data format: <tagName> <value>", sectionId.c_str() ));
				}
				break;
			}
			case DFNTAG_SPAWNOBJ:
			case DFNTAG_SPAWNOBJLIST:
				break;
			case DFNTAG_LOOT:
				[[fallthrough]];
			case DFNTAG_PACKITEM:
			{
				if( !cdata.empty() )
				{
					auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( cdata, "//" )), "," );
					if( csecs.size() > 1 )
					{
						UI16 iAmount = 0;
						std::string amountData = oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ));
						auto tsects = oldstrutil::sections( amountData, " " );
						if( tsects.size() > 1 ) // check if the second part of the tag-data contains two sections separated by a space
						{
							auto first = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[0], "//" )), nullptr, 0 ));
							auto second = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[1], "//" )), nullptr, 0 ));

							// Tag contained a minimum and maximum value for amount! Let's randomize!
							iAmount = static_cast<UI16>( RandomNum( first, second ));
						}
						else
						{
							iAmount = static_cast<UI16>( std::stoul( amountData, nullptr, 0 ));
						}
						auto tdata = oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" ));

						if( tag == DFNTAG_LOOT )
						{
							Items->AddRespawnItem( applyTo, tdata, true, true, iAmount, true );
						}
						else
						{
							Items->AddRespawnItem( applyTo, tdata, true, false, iAmount, false );
						}
					}
					else
					{
						if( tag == DFNTAG_LOOT )
						{
							Items->AddRespawnItem( applyTo, cdata, true, true, 1, true );
						}
						else
						{
							Items->AddRespawnItem( applyTo, cdata, true, false, 1, false );
						}
					}
				}
				break;
			}
			default:
			{
				Console.Warning( oldstrutil::format( "Unknown items dfn tag %i %s %i %i ", tag, cdata.c_str(), ndata, odata ));
				break;
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::CreateItem()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic item and gives it an ID, Colour, and amount, also will
//|					automatically look for an entry in harditems.dfn and set its location (be it in
//|					a pack or on the ground).
//o------------------------------------------------------------------------------------------------o
CItem * cItem::CreateItem( CSocket *mSock, CChar *mChar, const UI16 itemId, const UI16 iAmount, const UI16 iColour,
	const ObjectType itemType, bool inPack, bool shouldSave, UI08 worldNumber, UI16 instanceId, SI16 xLoc, SI16 yLoc, SI08 zLoc )
{
	if( ValidateObject( mChar ))
	{
		worldNumber = mChar->WorldNumber();
		instanceId = mChar->GetInstanceId();
		if( inPack && !ValidateObject( mChar->GetPackItem() ))
		{
			std::string charName = GetNpcDictName( mChar, nullptr, NRS_SYSTEM );
			Console.Warning( oldstrutil::format( "CreateItem(): Character %s(0x%X) has no pack, item creation aborted.", charName.c_str(), mChar->GetSerial() ));
			return nullptr;
		}

		if( inPack )
		{
			// Check if character's backpack can hold more items before creating any item
			CItem *playerPack = mChar->GetPackItem();
			if( ValidateObject( playerPack ))
			{
				if( playerPack->GetContainsList()->Num() >= playerPack->GetMaxItems() )
				{
					if( mSock != nullptr )
					{
						mSock->SysMessage( 1819 ); // Your backpack cannot hold any more items!
					}
					inPack = false; // Spawn item at character's feet instead
				}
			}
		}
	}


	CItem *iCreated = CreateBaseItem( worldNumber, itemType, instanceId, shouldSave );
	if( iCreated == nullptr )
		return nullptr;

	if( itemId != 0x0000 )
	{
		if( Map->IsValidTile( itemId ))
		{
			iCreated->SetId( itemId );
		}
	}
	if( iColour != 0x0000 )
	{
		iCreated->SetColour( iColour );
	}

	// Only set item to decayable by default if ini setting is enabled
	if( cwmWorldState->ServerData()->BaseItemsDecayable() )
	{
		iCreated->SetDecayable( true );
	}

	GetScriptItemSettings( iCreated );

	if( iAmount > 1 )
	{
		iCreated->SetPileable( true );
		iCreated->SetAmount( iAmount );
	}

	std::vector<UI16> scriptTriggers = iCreated->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			toExecute->OnCreate( iCreated, false, false );
		}
	}

	return PlaceItem( mSock, mChar, iCreated, inPack, worldNumber, instanceId, xLoc, yLoc, zLoc );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::CreateScriptItem()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a script item, gives it an amount, and sets
//|					its location (be it in a pack or on the ground).
//o------------------------------------------------------------------------------------------------o
CItem * cItem::CreateScriptItem( CSocket *mSock, CChar *mChar, const std::string &item, const UI16 iAmount, 
	const ObjectType itemType, bool inPack, const UI16 iColor, bool shouldSave )
{
	if( inPack && !ValidateObject( mChar->GetPackItem() ))
	{
		std::string charName = GetNpcDictName( mChar, nullptr, NRS_SYSTEM );
		Console.Warning( oldstrutil::format( "CreateScriptItem(): Character %s(0x%X) has no pack, item creation aborted.", charName.c_str(), mChar->GetSerial() ));
		return nullptr;
	}

	if( inPack )
	{
		// Check if character's backpack can hold more items before creating any item
		CItem *playerPack = mChar->GetPackItem();
		if( ValidateObject( playerPack ))
		{
			if( playerPack->GetContainsList()->Num() >= playerPack->GetMaxItems() )
			{
				if( mSock != nullptr )
				{
					mSock->SysMessage( 1819 ); // Your backpack cannot hold any more items!
				}
				inPack = false; // Spawn item at character's feet instead
			}
		}
	}

	CItem *iCreated = CreateBaseScriptItem( nullptr, item, mChar->WorldNumber(), iAmount, mChar->GetInstanceId(), itemType, 0xFFFF, shouldSave );
	if( iCreated == nullptr )
		return nullptr;

	if( iColor != 0xFFFF )
	{
		iCreated->SetColour( iColor );
	}
	if( iAmount > 1 && !iCreated->IsPileable() )
	{
		// Turns out we need to spawn more than one item, let's do that here:
		CItem *iCreated2 = nullptr;
		for( UI16 i = 1; i < iAmount; i++ )
		{
			if( inPack )
			{
				CItem *mPack = mChar->GetPackItem();
				iCreated2 = CreateBaseScriptItem( mPack, item, mChar->WorldNumber(), 1, mChar->GetInstanceId(), itemType );
			}
			else
			{
				iCreated2 = CreateBaseScriptItem( nullptr, item, mChar->WorldNumber(), 1, mChar->GetInstanceId(), itemType );
			}

			if( iCreated2 )
			{
				if( iColor != 0xFFFF )
				{
					iCreated2->SetColour( iColor );
				}
				PlaceItem( mSock, mChar, iCreated2, inPack );
			}
		}
	}

	// Return the original item created in the function.
	return PlaceItem( mSock, mChar, iCreated, inPack );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::*CreateRandomItem()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a random item from an itemlist in specified dfn file,
//|						gives it a random buy/sell value, and places it
//o------------------------------------------------------------------------------------------------o
CItem *cItem::CreateRandomItem( CSocket *mSock, const std::string& itemList )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ))
		return nullptr;

	CItem *iCreated = CreateRandomItem( nullptr, itemList, mChar->WorldNumber(), mChar->GetInstanceId() );
	if( iCreated == nullptr )
		return nullptr;

	if( iCreated->GetBuyValue() != 0 )
	{
		iCreated->SetBuyValue( RandomNum( static_cast<UI32>( 1 ), iCreated->GetBuyValue() ));
		iCreated->SetSellValue( static_cast<UI32>( iCreated->GetBuyValue() / 2 ));
	}
	if( iCreated->GetHP() != 0 )
	{
		iCreated->SetHP( static_cast<SI16>( RandomNum( static_cast<SI16>( 1 ), iCreated->GetHP() )));
	}

	return PlaceItem( mSock, mChar, iCreated, true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::CreateRandomItem()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a random item from an itemlist in specified dfn file
//o------------------------------------------------------------------------------------------------o
auto cItem::CreateRandomItem( CItem *mCont, const std::string& sItemList, const UI08 worldNum, const UI16 instanceId, bool shouldSave, bool useLootlist ) -> CItem *
{
	CItem *iCreated = nullptr;
	std::string sect;

	// Are we looking for a LOOTLIST, or an ITEMLIST?
	if( useLootlist )
	{
		sect = "LOOTLIST "s + sItemList;
	}
	else
	{
		sect = "ITEMLIST "s + sItemList;
	}
	sect	 = oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));

	// This is never "blank", since we assign "LOOTLIST" or "ITEMLIST" in the previous step!!
	if( sect == "blank" ) // The itemlist-entry is just a blank filler item
		return nullptr;

	// Look up the relevant LOOTLIST or ITEMLIST from DFNs
	auto ItemList = FileLookup->FindEntry( sect, items_def );
	if( ItemList )
	{
		// Count the number of entries in the list
		const size_t itemListSize = ItemList->NumEntries();
		if( itemListSize > 0 )
		{
			int itemEntryToSpawn = -1;
			int sum_of_weight = 0;

			// Let's see if the entries in the list are setup with randomization weights,
			// and sum them all up if that's the case. Format of string:
			//	weight|sectionName,amount
			//	weight|sectionName,amountMin amountMax
			//	weight|LOOTLIST=sectionName,amount
			//	weight|LOOTLIST=sectionName,amountMin amountMax
			//	weight|ITEMLIST=sectionName,amount
			//	weight|ITEMLIST=sectionName,amountMin amountMax
			for( size_t j = 0; j < itemListSize; j++ )
			{
				// Split string for entry into a stringlist, based on | as a separator
				auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( ItemList->MoveTo( j ), "//" )), "|" );
				if( csecs.size() == 2 )
				{
					// The entry has a weight assigned to it - let's add the weight to the total
					sum_of_weight += static_cast<int>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
				}
				else
				{
					// The entry has no weight assigned to it - add a default weight of 1
					sum_of_weight += 1;
				}
			}

			// Choose a random number between 0 and the total sum of all weights
			int rndChoice = RandomNum( 0, sum_of_weight - 1 );
			int itemWeight = 0;

			// Prepare a vector to hold multiple entries, if more than one qualifies based on weighting
			std::vector<int> matchingEntries;

			// Loop through the items in the itemlist/lootlist
			int weightOfChosenItem = 0;
			for( auto j = 0; j < static_cast<int>( itemListSize ); j++ )
			{
				auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( ItemList->MoveTo( j ), "//" )), "|" );
				if( csecs.size() == 2 )
				{
					// Ok, item entry has a weight, let's compare that weight to our chosen random number
					itemWeight = static_cast<int>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
					if( rndChoice < itemWeight )
					{
						// If we find another entry with same weight as the first one found, or if none have been found yet, add to list
						if( weightOfChosenItem == 0 || weightOfChosenItem == itemWeight )
						{
							itemEntryToSpawn = j;
							weightOfChosenItem = itemWeight;

							// Add the entry index to a temporary vector of all entries with same weight, then continue looking for more!
							matchingEntries.push_back( j );
							continue;
						}
					}
					rndChoice -= itemWeight;
				}
			}

			// Did we find more than one entry that matched our random weight criteria?
			if( matchingEntries.size() > 0 )
			{
				// Choose a random one of these!
				itemEntryToSpawn = matchingEntries[static_cast<int>( RandomNum( static_cast<size_t>( 0 ), matchingEntries.size() - 1 ))];
			}
			matchingEntries.clear();

			std::string k = "";
			std::vector<std::string> csecs;
			if( itemEntryToSpawn != -1 )
			{
				// If an entry has been selected based on weights, use that
				csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( ItemList->MoveTo( itemEntryToSpawn ), "//" )), "," );
			}
			else
			{
				// Otherwise choose a random entry
				csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( ItemList->MoveTo( RandomNum( static_cast<size_t>( 0 ), itemListSize - 1 )), "//" )), "," );
			}
			auto csecs2 = oldstrutil::sections( oldstrutil::trim( csecs[0] ), "|" );
			k = csecs2.size() > 1 ? csecs2[1] : csecs2[0];

			// Also fetch amount to spawn, if specified
			int amountToSpawn = 1;
			if( csecs.size() > 1 )
			{
				// UI16 iAmount = 0;
				std::string amountData = oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ));
				auto tsects = oldstrutil::sections( amountData, " " );
				if( tsects.size() > 1 ) // check if the second part of the tag-data contains two sections separated by a space
				{
					auto first = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[0], "//" )), nullptr, 0 ));
					auto second = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[1], "//" )), nullptr, 0 ));

					// Tag contained a minimum and maximum value for amount! Let's randomize!
					amountToSpawn = static_cast<UI16>( RandomNum( first, second ));
				}
				else
				{
					amountToSpawn = static_cast<UI16>( std::stoul(amountData, nullptr, 0 ));
				}
			}

			if( !k.empty() )
			{
				if( oldstrutil::upper( k ) == "ITEMLIST" || oldstrutil::upper( k ) == "LOOTLIST" )
				{
					// The chosen entry contained another ITEMLIST or LOOTLIST reference! Let's dive back into it...
					iCreated = CreateRandomItem( mCont, ItemList->GrabData(), worldNum, instanceId, shouldSave );
				}
				else
				{
					// Finally, we have an item! Spawn it!
					iCreated = CreateBaseScriptItem( nullptr, k, worldNum, amountToSpawn, instanceId, OT_ITEM, 0xFFFF, shouldSave );

					// However, we have a valid container, and the amount is larger than 1, and the item is not stackable, let's spawn some more items!
					if( ValidateObject( mCont ) && amountToSpawn > 1 && !iCreated->IsPileable() )
					{
						for( int i = 1; i < amountToSpawn; i++ )
						{
							CItem *iCreated2 = CreateBaseScriptItem( mCont, k, worldNum, 1, instanceId, OT_ITEM, 0xFFFF, shouldSave );
							if( ValidateObject( iCreated2 ))
							{
								// Place item in container and randomize location
								iCreated2->SetCont( mCont );
								iCreated2->PlaceInPack();
							}
						}
					}
				}
			}
		}
	}
	return iCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::CreateMulti()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a multi, and looks for an entry in harditems.dfn
//o------------------------------------------------------------------------------------------------o
CMultiObj * cItem::CreateMulti( const std::string& cName, const UI16 itemId, const bool isBoat, const UI16 worldNum, const UI16 instanceId, const bool isBaseMulti )
{
	CMultiObj *mCreated = static_cast<CMultiObj *>( ObjectFactory::GetSingleton().CreateObject(( isBoat ) ? OT_BOAT : OT_MULTI ));
	if( mCreated == nullptr )
		return nullptr;

	mCreated->SetId( itemId );
	if( !isBaseMulti )
	{
		GetScriptItemSettings( mCreated );
	}
	mCreated->WorldNumber( worldNum );
	mCreated->SetInstanceId( instanceId );
	mCreated->SetDecayable( false );
	if( !cName.empty() )
	{
		mCreated->SetName( cName );
	}

	return mCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateBaseItem( const UI08 worldNum, const ObjectType itemType, const UI16 instanceId )
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic item
//o------------------------------------------------------------------------------------------------o
CItem * cItem::CreateBaseItem( const UI08 worldNum, const ObjectType itemType, const UI16 instanceId, bool shouldSave )
{
	if( itemType != OT_ITEM && itemType != OT_SPAWNER )
		return nullptr;

	CItem *iCreated = static_cast<CItem *>( ObjectFactory::GetSingleton().CreateObject( itemType ));
	if( iCreated == nullptr )
		return nullptr;

	iCreated->ShouldSave( shouldSave );
	iCreated->SetWipeable( true );
	iCreated->WorldNumber( worldNum );
	iCreated->SetInstanceId( instanceId );

	return iCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::CreateBaseScriptItem()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic item from the scripts
//o------------------------------------------------------------------------------------------------o
CItem * cItem::CreateBaseScriptItem( CItem *mCont, std::string ourItem, const UI08 worldNum, const UI16 iAmount, const UI16 instanceId, const ObjectType itemType, const UI16 iColor, bool shouldSave )
{
	ourItem	= oldstrutil::trim( oldstrutil::removeTrailing( ourItem, "//" ));

	if( ourItem == "blank" ) // The lootlist-entry is just a blank filler item
		return nullptr;

	CScriptSection *itemCreate = FileLookup->FindEntry( ourItem, items_def );
	if( itemCreate == nullptr )
	{
		Console.Error( oldstrutil::format( "CreateBaseScriptItem(): Bad script item %s (Item Not Found).", ourItem.c_str() ));
		return nullptr;
	}

	CItem *iCreated = nullptr;
	if( itemCreate->ItemListExist() )
	{
		iCreated = CreateRandomItem( mCont, itemCreate->ItemListData(), worldNum, instanceId, shouldSave );
	}
	else
	{
		iCreated = CreateBaseItem( worldNum, itemType, instanceId, shouldSave );
		if( iCreated == nullptr )
			return nullptr;

		// Only set item to decayable by default if ini setting is enabled
		if( cwmWorldState->ServerData()->ScriptItemsDecayable() )
		{
			iCreated->SetDecayable( true );
		}

		if( !ApplyItemSection( iCreated, itemCreate, ourItem ))
		{
			Console.Error( "Trying to apply an item section failed" );
		}

		// If maxHP has not been defined for a new item, set it to the same value as HP
		if( !iCreated->GetMaxHP() && iCreated->GetHP() )
		{
			iCreated->SetMaxHP( iCreated->GetHP() );
		}

		// If maxUses is higher than usesLeft for a new item, randomize the amount of usesLeft the item should have!
		if( iCreated->GetMaxUses() > iCreated->GetUsesLeft() )
		{
			iCreated->SetUsesLeft( static_cast<UI16>( RandomNum( iCreated->GetUsesLeft(), iCreated->GetMaxUses() )));
		}
		else if( !iCreated->GetMaxUses() && iCreated->GetUsesLeft() )
		{
			// Also, if maxUses has not been defined, but usesLeft HAS, set maxUses to equal usesLeft
			iCreated->SetMaxUses( iCreated->GetUsesLeft() );
		}

		// Check for (and run) onCreateDFN() event for newly created item
		std::vector<UI16> scriptTriggers = iCreated->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				toExecute->OnCreate( iCreated, true, false );
			}
		}
	}
	if( ValidateObject( iCreated ))
	{
		if( iColor != 0xFFFF )
		{
			iCreated->SetColour( iColor );
		}
		if( iAmount > 1 && iCreated->IsPileable() )
		{
			iCreated->SetAmount( iAmount );
		}

		// Keep reference to DFN sectionId item was created from (if it has not been set already via special DFN tag SECTIONID)
		if( iCreated->GetSectionId() == "UNKNOWN" )
		{
			iCreated->SetSectionId( ourItem );
		}
	}

	return iCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::GetScriptItemSettings()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Grabs item entries from harditems.dfn
//o------------------------------------------------------------------------------------------------o
void cItem::GetScriptItemSettings( CItem *iCreated )
{	
	std::string hexId = oldstrutil::number( iCreated->GetId(), 16 );
	while( hexId.size() < 4 )
	{
		hexId = "0" + hexId;
	}

	const std::string item = "x" + hexId;
	CScriptSection *toFind = FileLookup->FindEntrySubStr( item, hard_items_def );
	if( toFind != nullptr )
	{
		ApplyItemSection( iCreated, toFind, item );
	}
}

CItem *AutoStack( CSocket *mSock, CItem *iToStack, CItem *iPack );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::PlaceItem()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Places an item that was just created
//o------------------------------------------------------------------------------------------------o
CItem * cItem::PlaceItem( CSocket *mSock, CChar *mChar, CItem *iCreated, const bool inPack, UI08 worldNumber, UI16 instanceId, SI16 xLoc, SI16 yLoc, SI08 zLoc )
{
	if( ValidateObject( mChar ) && inPack )
	{
		if( iCreated->IsPileable() )
		{
			iCreated = AutoStack( mSock, iCreated, mChar->GetPackItem() );	// if it didn't stack, it's iCreated... if it did, then it's the stack!
		}
		else
		{
			iCreated->SetCont( mChar->GetPackItem() );
			iCreated->PlaceInPack();
		}

		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			if( mSock != nullptr )
			{
				// Refresh container tooltip
				CPToolTip pSend( iCreated->GetContSerial(), mSock );
				mSock->Send(&pSend);
			}
		}
	}
	else if( ValidateObject( mChar ))
	{
		iCreated->SetLocation( mChar );
	}
	else
	{
		iCreated->SetLocation( xLoc, yLoc, zLoc, worldNumber, instanceId );
	}
	return iCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DecayItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Cause items to decay when left on the ground
//o------------------------------------------------------------------------------------------------o
auto DecayItem( CItem& toDecay, const UI32 nextDecayItems, UI32 nextDecayItemsInHouses ) -> bool
{
	if( toDecay.GetDecayTime() == 0 || !cwmWorldState->ServerData()->GlobalItemDecay() )
	{
		if( toDecay.GetMulti() == INVALIDSERIAL )
		{
			toDecay.SetDecayTime( nextDecayItems );
		}
		else
		{
			toDecay.SetDecayTime( nextDecayItemsInHouses );
		}
		return false;
	}
	const auto isCorpse = toDecay.IsCorpse();

	// Multis
	if( !toDecay.IsFieldSpell() && !isCorpse ) // Gives fieldspells a chance to decay in multis
	{
		if( toDecay.IsLockedDown() || toDecay.IsDoorOpen() ||
		   ( ValidateObject( toDecay.GetMultiObj() ) &&
			( toDecay.GetMovable() >= 2 || !cwmWorldState->ServerData()->ItemDecayInHouses() )))
			{
			toDecay.SetDecayTime( nextDecayItemsInHouses );
			return false;
		}
	}

	if( toDecay.IsContType() )
	{
		if( isCorpse && (( ValidateObject( toDecay.GetOwnerObj() ) && !cwmWorldState->ServerData()->PlayerCorpseLootDecay() )  // Player corpse
			|| ( !ValidateObject( toDecay.GetOwnerObj() ) && !cwmWorldState->ServerData()->NpcCorpseLootDecay() ))) // NPC corpse
		{
			std::vector<CItem *> corpseItems;
			auto iCont = toDecay.GetContainsList();
			for( const auto &io : iCont->collection() )
			{
				if( ValidateObject( io ))
				{
					if( io->GetLayer() != IL_HAIR && io->GetLayer() != IL_FACIALHAIR )
					{
						// Store a reference to the item we want to move out of corpse...
						corpseItems.push_back( io );
					}
				}
			}

			// Loop through the items we want to move out of corpse
			std::for_each( corpseItems.begin(), corpseItems.end(), [toDecay, nextDecayItems]( CItem *corpseItem )
			{
				corpseItem->SetCont( nullptr );
				corpseItem->SetLocation(( &toDecay ));
				corpseItem->SetDecayTime( nextDecayItems );
			});
		}
	}

	if( toDecay.CanBeObjType( OT_MULTI ))
	{
		toDecay.SetDecayTime( nextDecayItems );
		Console.Warning( oldstrutil::format( "Warning: Prevented multi (serial: 0x%X) from decaying!", toDecay.GetSerial() ));
	}
	toDecay.Delete();  // This is a problem, if done in a ierator loop
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::GetPackType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the pack type based on ID
//o------------------------------------------------------------------------------------------------o
PackTypes cItem::GetPackType( CItem *i )
{
	PackTypes packType = PT_UNKNOWN;
	switch( i->GetId() )
	{
		case 0x2006:	// coffin
		case 0x0ECA:	// bones
		case 0x0ECB:	// bones
		case 0x0ECC:	// bones
		case 0x0ECD:	// bones
		case 0x0ECE:	// bones
		case 0x0ECF:	// bones
		case 0x0ED0:	// bones
		case 0x0ED1:	// bones
		case 0x0ED2:	// bones
			packType = PT_COFFIN;
			break;
		case 0x0E75:	// backpack
		case 0x0E79:	// pouch
		case 0x09B0:	// pouch
		case 0xA1F6:	// First Aid Belt
			packType = PT_PACK;
			break;
		case 0x0E76:	// leather bag
		case 0x2256:	// bagball
		case 0x2257:	// bagball
			packType = PT_BAG;
			break;
		case 0x0E77:	// barrel
		case 0x0E7F:	// keg
		case 0x0E83:	// empty tub
		case 0x0FAE:	// barrel with lids
		case 0x1AD7:	// potion kegs
		case 0x1940:	// barrel with lids
			packType = PT_BARREL;
			break;
		case 0x0E7A:	// square basket
		case 0x24D5:	// SE basket
		case 0x24D6:	// SE basket
		case 0x24D9:	// SE basket
		case 0x24DA:	// SE basket
			packType = PT_SQBASKET;
			break;
		case 0x0990:	// round basket
		case 0x09AC:	// round bushel
		case 0x09B1:	// round basket
		case 0x24D7:	// SE basket
		case 0x24D8:	// SE basket
		case 0x24DD:	// SE basket
		case 0x1882:	// winnoning basket
			packType = PT_RBASKET;
			break;
		case 0x0E40:	// gold chest
		case 0x0E41:	// gold chest
		case 0x4025:	// gargoyle chest
		case 0x4026:	// gargoyle chest
		case 0xA304:	// metal chest
		case 0xA305:	// metal chest
		case 0xA306:	// rusty metal chest
		case 0xA307:	// rusty metal chest
		case 0xA308:	// gold chest
		case 0xA309:	// gold chest
		case 0xA30A:	// barnacle metal chest
		case 0xA30B:	// barnacle metal chest
			packType = PT_GCHEST;
			break;
		case 0x0E7D:	// wooden box
		case 0x09AA:	// wooden box
			packType = PT_WBOX;
			break;
		case 0x0E3C:	// large wooden crate
		case 0x0E3D:	// large wooden crate
		case 0x0E3E:	// small wooden create
		case 0x0E3F:	// small wooden crate
		case 0x0E7E:	// wooden crate
		case 0x09A9:	// small wooden crate
			packType = PT_CRATE;
			break;
		case 0x2AF8:	// Shopkeeper buy, sell and sold layers
			packType = PT_SHOPPACK;
			break;
		case 0x0A30:   // chest of drawers (fancy)
		case 0x0A38:   // chest of drawers (fancy)
			packType = PT_DRAWER;
			break;
		case 0x0E42:	// wooden chest
		case 0x0E43:	// wooden chest
			packType = PT_WCHEST;
			break;
		case 0x0E80:	// brass box
		case 0x09A8:	// metal box
			packType = PT_MBOX;
			break;
		case 0x0E7C:	// silver chest
		case 0x09AB:	// silver chest
			packType = PT_SCHEST;
			break;
		case 0x0A97:	// bookcase
		case 0x0A98:	// bookcase
		case 0x0A99:	// bookcase
		case 0x0A9A:	// bookcase
		case 0x0A9B:	// bookcase
		case 0x0A9C:	// bookcase
		case 0x0A9D:	// bookcase (empty)
		case 0x0A9E:	// bookcase (empty)
			packType = PT_BOOKCASE;
			break;
		case 0x0A4C:	// fancy armoire (open)
		case 0x0A4D:	// fancy armoire
		case 0x0A50:	// fancy armoire (open)
		case 0x0A51:	// fancy armoire
			packType = PT_FARMOIRE;
			break;
		case 0x0A4E:	// wooden armoire (open)
		case 0x0A4F:	// wooden armoire
		case 0x0A52:	// wooden armoire (open)
		case 0x0A53:	// wooden armoire
			packType = PT_WARMOIRE;
			break;
		case 0x0A2C:	// chest of drawers (wood)
		case 0x0A34:	// chest of drawers (wood)
		case 0x0A35:	// dresser
		case 0x0A3C:	// dresser
		case 0x0A3D:	// dresser
		case 0x0A44:	// dresser
			packType = PT_DRESSER;
			break;
		case 0x09B2:	// bank box ..OR.. backpack 2
			if( i->GetTempVar( CITV_MOREX ) == 1 )
			{
				packType = PT_BANK;
			}
			else
			{
				packType = PT_PACK2;
			}
			break;
		case 0x2bd9:
		case 0x2BDA:
		case 0x2BDB:
		case 0x2BDC:
			packType = PT_STOCKING;
			break;
		case 0x232A:	// giftbox
		case 0x232B:	// giftbox
			packType = PT_GIFTBOX1;
			break;
		case 0x2857:	// SE armoire
		case 0x2858:	// SE armoire
			packType = PT_SEARMOIRE1;
			break;
		case 0x285B:	// SE armoire
		case 0x285C:	// SE armoire
			packType = PT_SEARMOIRE2;
			break;
		case 0x285D:	// SE armoire
		case 0x285E:	// SE armoire
		case 0x2859:	// SE armoire
		case 0x285A:	// SE armoire
			packType = PT_SEARMOIRE3;
			break;
		case 0x24DB:	// SE basket
		case 0x24DC:	// SE basket
			packType = PT_SEBASKET;
			break;
		case 0x280B:	// SE chest
		case 0x280C:	// SE chest
			packType = PT_SECHEST1;
			break;
		case 0x280D:	// SE chest
		case 0x280E:	// SE chest
			packType = PT_SECHEST2;
			break;
		case 0x280F:	// SE chest
		case 0x2810:	// SE chest
			packType = PT_SECHEST3;
			break;
		case 0x2811:	// SE chest
		case 0x2812:	// SE chest
		case 0x2815:	// SE cabinet
		case 0x2816:	// SE cabinet
		case 0x2817:	// SE chest
		case 0x2818:	// SE chest
			packType = PT_SECHEST4;
			break;
		case 0x2813:	// SE chest
		case 0x2814:	// SE chest
			packType = PT_SECHEST5;
			break;
		case 0x46A5:	// SA giftbox
		case 0x46A6:	// SA giftbox
			packType = PT_GIFTBOX2;
			break;
		case 0x46A2:	// SA giftbox
			packType = PT_GIFTBOX3;
			break;
		case 0x46A3:	// SA giftbox
			packType = PT_GIFTBOX4;
			break;
		case 0x46A4:	// SA giftbox
			packType = PT_GIFTBOX5;
			break;
		case 0x46A7:	// SA giftbox
			packType = PT_GIFTBOX6;
			break;
		case 0x0E1C:	// Backgammon board
		case 0x0FAD:	// Backgammon board
			packType = PT_GAME_BACKGAMMON;
			break;
		case 0x0FA6:	// Chess board
			packType = PT_GAME_CHESS;
			break;
		case 0xA202:	// Dolphin mailbox
		case 0xA203:	// Dolphin mailbox
		case 0xA204:	// Dolphin mailbox
		case 0xA205:	// Dolphin mailbox
			packType = PT_MAILBOX1;
			break;
		case 0xA206:	// Squirrel mailbox
		case 0xA207:	// Squirrel mailbox
		case 0xA208:	// Squirrel mailbox
		case 0xA209:	// Squirrel mailbox
			packType = PT_MAILBOX2;
			break;
		case 0xA1F5:	// Wooden barrel mailbox
		case 0xA1F7:	// Wooden barrel mailbox
		case 0xA1F8:	// Wooden barrel mailbox
		case 0xA1F9:	// Wooden barrel mailbox
			packType = PT_MAILBOX3;
			break;
		case 0xA268:	// Light mailbox
		case 0xA269:	// Light mailbox
		case 0xA26A:	// Light mailbox
		case 0xA26B:	// Light mailbox
		case 0xA26C:	// Light mailbox
		case 0xA26D:	// Light mailbox
		case 0xA26E:	// Light mailbox
		case 0xA26F:	// Light mailbox
			packType = PT_MAILBOX4;
			break;
		case 0xA3EB:	// Sitting kitten mailbox
		case 0xA3EC:	// Sitting kitten mailbox
		case 0xA3ED:	// Sitting kitten mailbox
		case 0xA3EE:	// Sitting kitten mailbox
			packType = PT_MAILBOX5;
			break;
		case 0xA3EF:	// Standing kitten mailbox
		case 0xA3F0:	// Standing kitten mailbox
		case 0xA3F1:	// Standing kitten mailbox
		case 0xA3F2:	// Standing kitten mailbox
			packType = PT_MAILBOX6;
			break;
		case 0xA3F3:	// Scarecrow mailbox
		case 0xA3F4:	// Scarecrow mailbox
		case 0xA3F5:	// Scarecrow mailbox
		case 0xA3F6:	// Scarecrow mailbox
			packType = PT_MAILBOX7;
			break;
		case 0xA3F7:	// Lion mailbox
		case 0xA3F8:	// Lion mailbox
		case 0xA3F9:	// Lion mailbox
		case 0xA3FA:	// Lion mailbox
			packType = PT_MAILBOX8;
			break;
		case 0x4141:	// Square gray mailbox
		case 0x4142:	// Square gray mailbox
		case 0x4143:	// Square gray mailbox
		case 0x4144:	// Square gray mailbox
			packType = PT_MAILBOX9;
			break;
		default:
			packType = PT_UNKNOWN;
			break;
	}
	return packType;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::AddRespawnItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles spawning of items from spawn objects/containers
//o------------------------------------------------------------------------------------------------o
void cItem::AddRespawnItem( CItem *mCont, const std::string& iString, const bool inCont, const bool randomItem, const UI16 itemAmount, bool useLootlist )
{
	if( !ValidateObject( mCont ) || iString.empty() )
		return;

	CItem *iCreated = nullptr;
	if( randomItem )
	{
		// LOOT tag was used
		iCreated = CreateRandomItem( mCont, iString, mCont->WorldNumber(), mCont->GetInstanceId(), true, useLootlist );
	}
	else
	{
		// PACKITEM tag was used
		iCreated = CreateBaseScriptItem( mCont, iString, mCont->WorldNumber(), itemAmount, mCont->GetInstanceId() );
	}

	if( iCreated == nullptr )
		return;

	// If item is stackable and amount is > 1, amount should have been set already
	// However, if item is not stackable, spawn each item individually
	if(( itemAmount > 1 ) && (( !randomItem && !iCreated->IsPileable() ) || randomItem ))
	{
		CItem *iCreated2 = nullptr;
		for( UI08 i = 1; i < itemAmount; ++i )
		{
			if( randomItem )
			{
				// If amount was specified for a LOOT tag, spawn a random item each time
				iCreated2 = CreateRandomItem( mCont, iString, mCont->WorldNumber(), mCont->GetInstanceId(), true, useLootlist );
			}
			else
			{
				// If amount was specified for a PACKITEM tag, spawn more copies of the same item
				iCreated2 = CreateBaseScriptItem( mCont, iString, mCont->WorldNumber(), 1, mCont->GetInstanceId() );
			}
			if( iCreated2 )
			{
				if( inCont )
				{
					// Place item randomly in container
					iCreated2->SetCont( mCont );
					iCreated2->PlaceInPack();
				}
				else
				{
					iCreated2->SetLocation( mCont ); // Set item'mCont location in the world
				}
				iCreated2->SetSpawn( mCont->GetSerial() ); // Set source item was spawned from

				// Try to stack the new item with existing items in the container, if any
				if( iCreated2->IsPileable() )
				{
					AutoStack( nullptr, iCreated2, mCont );
				}
			}
		}
	}

	// Ok, time to finalize setup of original item that was spawned
	if( inCont )
	{
		iCreated->SetCont( mCont );
	}
	else
	{
		iCreated->SetLocation( mCont );
	}
	iCreated->SetSpawn( mCont->GetSerial() );

	if( inCont )
	{
		if( ValidateObject( mCont ))
		{
			// Determine random location within container to place item
			iCreated->PlaceInPack();

			// Try to stack the new item with existing items in the container, if any
			AutoStack( nullptr, iCreated, mCont );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::GlowItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle glowing items
//o------------------------------------------------------------------------------------------------o
void cItem::GlowItem( CItem *i )
{
	if( i->GetGlow() != INVALIDSERIAL )
	{
		CItem *j = CalcItemObjFromSer( i->GetGlow() );
		if( !ValidateObject( j ))
			return;

		CBaseObject *getCont = i->GetCont();
		if( getCont == nullptr ) // On the ground
		{
			j->SetCont( nullptr );
			j->SetDir( 29 );
			j->SetLocation( i );
		}
		else if( getCont->GetObjType() == OT_ITEM ) // In a pack
		{
			j->SetCont( getCont );
			j->SetDir( 99 );  // gives no light in backpacks
			j->SetX( i->GetX() );
			j->SetY( i->GetY() );
			j->SetZ( i->GetZ() );
		}
		else // Equipped
		{
			CChar *s = static_cast<CChar *>( getCont );
			if( ValidateObject( s ))
			{
				j->SetCont( getCont );
				j->SetX( s->GetX() );
				j->SetY( s->GetY() );
				j->SetZ( s->GetZ()+4 );
				if( IsOnline(( *s )))
				{
					j->SetDir( 29 );
				}
				else
				{
					j->SetDir( 99 );
				}
				j->SetLayer( IL_TALISMAN );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::CheckEquipment()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks equipment of character and validates that they have enough strength
//|					to have each item equipped
//o------------------------------------------------------------------------------------------------o
void cItem::CheckEquipment( CChar *p )
{
	if( ValidateObject( p ))
	{
		CSocket *pSock = p->GetSocket();
		if( pSock == nullptr )
			return;

		std::vector<CItem *> itemsToUnequip;
		const SI16 StrengthToCompare = p->GetStrength();
		for( CItem *i = p->FirstItem(); !p->FinishedItems(); i = p->NextItem() )
		{
			if( ValidateObject( i ))
			{
				if( i->GetStrength() > StrengthToCompare )//if strength required > character's strength
				{
					itemsToUnequip.push_back( i );
				}
			}
		}

		// Loop through the items we want to move out of corpse
		std::for_each( itemsToUnequip.begin(), itemsToUnequip.end(), [p, pSock]( CItem *iUnequip )
		{
			auto pPack = p->GetPackItem();
			if( ValidateObject( pPack ) )
			{
				// Pack detected, drop to pack
				iUnequip->SetCont( pPack );
				iUnequip->PlaceInPack();
			}
			else
			{
				// No pack detected - drop on ground
				iUnequip->SetCont( nullptr );
				iUnequip->SetLocation( p );
			}

			for( auto &item : FindNearbyPlayers( p ))
			{
				p->SendWornItems( item );
			}

			pSock->SysMessage( 2782 ); // You are not strong enough to keep some of your items equipped!
			Effects->ItemSound( pSock, iUnequip );
		});
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::StoreItemRandomValue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Stores an item's random "good" value (used by trade system)
//o------------------------------------------------------------------------------------------------o
void cItem::StoreItemRandomValue( CItem *i, CTownRegion *tReg )
{
	if( i->GetGood() < 0 )
		return;

	if( tReg == nullptr )
	{
		CBaseObject *getLastCont = i->GetCont();
		if( getLastCont != nullptr )
		{
			tReg = CalcRegionFromXY( getLastCont->GetX(), getLastCont->GetY(), getLastCont->WorldNumber(), getLastCont->GetInstanceId() );
		}
		if( tReg == nullptr )
		{
			return;
		}
	}

	const SI32 min = tReg->GetGoodRnd1( static_cast<UI08>( i->GetGood() ));
	const SI32 max = tReg->GetGoodRnd2( static_cast<UI08>( i->GetGood() ));

	if( max != 0 || min != 0 )
	{
		i->SetRndValueRate( RandomNum( min, max ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cItem::DupeItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Duplicates selected item
//o------------------------------------------------------------------------------------------------o
CItem *cItem::DupeItem( CSocket *s, CItem *i, UI32 amount )
{
	CChar *mChar		= s->CurrcharObj();
	CBaseObject *iCont	= i->GetCont();
	CItem *charPack		= mChar->GetPackItem();

	if( !ValidateObject( mChar ) || i->IsCorpse() || ( !ValidateObject( iCont ) && !ValidateObject( charPack )))
		return nullptr;

	CItem *c = i->Dupe();
	if( c == nullptr )
		return nullptr;

	c->IncLocation( 2, 2 );
	if(( !ValidateObject( iCont ) || iCont->GetObjType() != OT_ITEM ) && ValidateObject( charPack ))
	{
		c->SetCont( charPack );
	}

	if( amount > MAX_STACK )
	{
		amount -= MAX_STACK;
		DupeItem( s, i, MAX_STACK );
	}
	c->SetAmount( amount );

	std::vector<UI16> scriptTriggers = c->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			toExecute->OnCreate( c, false, false );
		}
	}

	return c;
}
