//o--------------------------------------------------------------------------o
//|	File			-	cChar.cpp
//|	Date			-	29th March, 2000
//|	Developers		-	Abaddon/EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	New class written based upon old UOX char_st.  Number of old members removed
//|						and a number of members types modified as well
//|
//|	Version History -
//|									
//|						1.0		Abaddon		29th March, 2000
//|						Initial implementation, all get/set mechanisms written up to
//|						the end of SetSayColour().  Bools reduced down to a single char
//|						with get/set mechanisms for setting/clearings bits
//|						No documentation currently done, all functions obvious so far
//|						Note:  individual byte setting within longs/chars need speeding up
//|						consider memcpy into correct word, note that this will be endian specific!
//|
//| 					1.1		Abaddon		2nd April, 2000
//|						Finished up the initial implementation on all classes. Fixed a minor bit shift error
//|						on some of the char variables, all working fine now
//|
//| 					1.2		Abaddon		25 July, 2000
//|						All Accessor funcs plus a few others are now flagged as const functions, meaning that
//|						those functions GUARENTEE never to alter data, at compile time
//|						Thoughts about CBaseObject and prelim plans made
//|
//| 					1.3		Abaddon		28 July, 2000
//|						Initial CBaseObject implementation put in.  CChar reworked to deal with only things it has to
//|						Proper constructor written
//|						Plans for CChar derived objects thought upon (notably CPC and CNPC)
//|						
//|						1.4		giwo		27 September, 2005
//|						Added PlayerValues_st and NPCValues_st to allow saving wasted memory on unnecesarry variables
//|						Organized many functions to their respective areas and added documentation for them.
//|						Changed itemLayers to a map
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "power.h"
#include "weight.h"
#include "cGuild.h"
#include "msgboard.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "speech.h"
#include "ObjectFactory.h"
#include "teffect.h"
#include "cSpawnRegion.h"
#include "Dictionary.h"
#include "movement.h"
#include "CJSEngine.h"

namespace UOX
{

// Bitmask bits

const UI32 BIT_GM				=	0x0001;
const UI32 BIT_BROADCAST		=	0x0002;
const UI32 BIT_INVULNERABLE		=	0x0004;
const UI32 BIT_SINGCLICKSER		=	0x0008;
const UI32 BIT_SKILLTITLES		=	0x0010;
const UI32 BIT_GMPAGEABLE		=	0x0020;
const UI32 BIT_SNOOP			=	0x0040;
const UI32 BIT_COUNSELOR		=	0x0080;
const UI32 BIT_ALLMOVE			=	0x0100;
const UI32 BIT_FROZEN			=	0x0200;
const UI32 BIT_VIEWHOUSEASICON	=	0x0400;
const UI32 BIT_NONEEDMANA		=	0x1000;
const UI32 BIT_DISPELLABLE		=	0x2000;
const UI32 BIT_PERMREFLECTED	=	0x4000;
const UI32 BIT_NONEEDREAGS		=	0x8000;


const UI32 BIT_UNICODE			=	0x02;
const UI32 BIT_NPC				=	0x04;
const UI32 BIT_SHOP				=	0x08;
const UI32 BIT_DEAD				=	0x10;
const UI32 BIT_ATWAR			=	0x20;
const UI32 BIT_ATTACKFIRST		=	0x40;
const UI32 BIT_ONHORSE			=	0x80;
const UI32 BIT_TOWNTITLE		=	0x100;
const UI32 BIT_REACTIVEARMOUR	=	0x200;
const UI32 BIT_TRAIN			=	0x400;
const UI32 BIT_GUILDTOGGLE		=	0x800;
const UI32 BIT_TAMED			=	0x1000;
const UI32 BIT_GUARDED			=	0x2000;
const UI32 BIT_RUN				=	0x4000;
const UI32 BIT_POLYMORPHED		=	0x8000;
const UI32 BIT_INCOGNITO		=	0x10000;
const UI32 BIT_USINGPOTION		=	0x20000;
const UI32 BIT_MAYLEVITATE		=	0x40000;
const UI32 BIT_WILLHUNGER		=	0x80000;
const UI32 BIT_MEDITATING		=	0x100000;
const UI32 BIT_CASTING			=	0x200000;
const UI32 BIT_JSCASTING		=	0x400000;

const UI32 BIT_MAXHPFIXED		=	0x01;
const UI32 BIT_MAXMANAFIXED		=	0x02;
const UI32 BIT_MAXSTAMFIXED		=	0x04;
const UI32 BIT_CANATTACK		=	0x08;

const UI32 BIT_MOUNTED			=	0x01;
const UI32 BIT_STABLED			=	0x02;

// Player defaults

const SI16			DEFPLAYER_CALLNUM 			= -1;
const SI16			DEFPLAYER_PLAYERCALLNUM		= -1;
const SERIAL		DEFPLAYER_TRACKINGTARGET 	= INVALIDSERIAL;
const UI08			DEFPLAYER_SQUELCHED			= 0;
const UI08			DEFPLAYER_COMMANDLEVEL 		= PLAYER_CMDLEVEL;
const UI08			DEFPLAYER_POSTTYPE 			= PT_LOCAL;
const UI16			DEFPLAYER_HAIRSTYLE			= INVALIDID;
const UI16			DEFPLAYER_BEARDSTYLE 		= INVALIDID;
const COLOUR		DEFPLAYER_HAIRCOLOUR 		= INVALIDCOLOUR;
const COLOUR		DEFPLAYER_BEARDCOLOUR 		= INVALIDCOLOUR;
const UI08			DEFPLAYER_SPEECHMODE 		= 0;
const UI08			DEFPLAYER_SPEECHID 			= 0;
const SERIAL		DEFPLAYER_ROBE 				= INVALIDSERIAL;
const UI16			DEFPLAYER_ACCOUNTNUM		= AB_INVALID_ID;
const UI16			DEFPLAYER_ORIGSKIN			= 0;
const UI16			DEFPLAYER_ORIGID			= 0x0190;
const UI08			DEFPLAYER_FIXEDLIGHT 		= 255;
const UI16			DEFPLAYER_DEATHS			= 0;

CChar::PlayerValues_st::PlayerValues_st() : callNum( DEFPLAYER_CALLNUM ), playerCallNum( DEFPLAYER_PLAYERCALLNUM ), trackingTarget( DEFPLAYER_TRACKINGTARGET ),
squelched( DEFPLAYER_SQUELCHED ), commandLevel( DEFPLAYER_COMMANDLEVEL ), postType( DEFPLAYER_POSTTYPE ), hairStyle( DEFPLAYER_HAIRSTYLE ), beardStyle( DEFPLAYER_BEARDSTYLE ),
hairColour( DEFPLAYER_HAIRCOLOUR ), beardColour( DEFPLAYER_BEARDCOLOUR ), speechItem( NULL ), speechMode( DEFPLAYER_SPEECHMODE ), speechID( DEFPLAYER_SPEECHID ),
speechCallback( NULL ), robe( DEFPLAYER_ROBE ), accountNum( DEFPLAYER_ACCOUNTNUM ), origSkin( DEFPLAYER_ORIGSKIN ), origID( DEFPLAYER_ORIGID ), 
fixedLight( DEFPLAYER_FIXEDLIGHT ), deaths( DEFPLAYER_DEATHS ), socket( NULL )
{
	if( cwmWorldState != NULL )
		trackingTargets.resize( cwmWorldState->ServerData()->TrackingMaxTargets() );
}

const SI08			DEFNPC_WANDER 				= 0;
const SI08			DEFNPC_OLDWANDER 			= 0;
const SERIAL		DEFNPC_FTARG				= INVALIDSERIAL;
const SI08			DEFNPC_FZ1 					= -1;
const SI16			DEFNPC_AITYPE 				= 0;
const SI16			DEFNPC_SPATTACK				= 0;
const SI08			DEFNPC_SPADELAY				= 0;
const SI16			DEFNPC_TAMING 				= 0x7FFF;
const SI16			DEFNPC_PEACEING 			= 0x7FFF;
const SI16			DEFNPC_PROVOING				= 0x7FFF;
const SI16			DEFNPC_FLEEAT 				= 0;
const SI16			DEFNPC_REATTACKAT 			= 0;
const UI08			DEFNPC_SPLIT 				= 0;
const UI08			DEFNPC_SPLITCHANCE 			= 0;
const UI08			DEFNPC_TRAININGPLAYERIN		= 0;
const UI32			DEFNPC_HOLDG 				= 0;
const UI08			DEFNPC_QUESTTYPE			= 0;
const UI08			DEFNPC_QUESTDESTREGION 		= 0;
const UI08			DEFNPC_QUESTORIGREGION		= 0;
const SI16			DEFNPC_WANDERAREA			= -1;
const cNPC_FLAG		DEFNPC_NPCFLAG				= fNPC_NEUTRAL;
const UI16			DEFNPC_BOOLFLAG				= 0;

CChar::NPCValues_st::NPCValues_st() : wanderMode( DEFNPC_WANDER ), oldWanderMode( DEFNPC_OLDWANDER ), fTarg( DEFNPC_FTARG ), fz( DEFNPC_FZ1 ),
aiType( DEFNPC_AITYPE ), spellAttack( DEFNPC_SPATTACK ), spellDelay( DEFNPC_SPADELAY ), taming( DEFNPC_TAMING ), fleeAt( DEFNPC_FLEEAT ),
reAttackAt( DEFNPC_REATTACKAT ), splitNum( DEFNPC_SPLIT ), splitChance( DEFNPC_SPLITCHANCE ), trainingPlayerIn( DEFNPC_TRAININGPLAYERIN ),
goldOnHand( DEFNPC_HOLDG ), questType( DEFNPC_QUESTTYPE ), questDestRegion( DEFNPC_QUESTDESTREGION ), questOrigRegion( DEFNPC_QUESTORIGREGION ),
petGuarding( NULL ), npcFlag( DEFNPC_NPCFLAG ), boolFlags( DEFNPC_BOOLFLAG ), peaceing( DEFNPC_PEACEING ), provoing( DEFNPC_PROVOING )
{
	fx[0] = fx[1] = fy[0] = fy[1] = DEFNPC_WANDERAREA;
	petFriends.resize( 0 );
}

const SERIAL		DEFCHAR_TOWNVOTE 			= INVALIDSERIAL;
const UI32			DEFCHAR_BOOLS 				= 0;
const UI16			DEFCHAR_BOOLS2 				= 0;
const SI08			DEFCHAR_DISPZ 				= 0;
const SI08			DEFCHAR_FONTTYPE 			= 3;
const RACEID		DEFCHAR_OLDRACE 			= 0;
const UI16			DEFCHAR_MAXHP 				= 0;
const UI16			DEFCHAR_MAXHP_OLDSTR 		= 0;
const UI16			DEFCHAR_MAXMANA				= 0;
const UI16			DEFCHAR_MAXMANA_OLDINT 		= 0;
const UI16			DEFCHAR_MAXSTAM				= 0;
const UI16			DEFCHAR_MAXSTAM_OLDDEX 		= 0;
const COLOUR		DEFCHAR_SAYCOLOUR 			= 0x0058;
const COLOUR		DEFCHAR_EMOTECOLOUR			= 0x0023;
const SI08			DEFCHAR_CELL 				= -1;
const SERIAL		DEFCHAR_TARG 				= INVALIDSERIAL;
const SERIAL		DEFCHAR_ATTACKER 			= INVALIDSERIAL;
const SI08			DEFCHAR_HUNGER 				= 6;
const UI08			DEFCHAR_REGIONNUM 			= 255;
const UI08			DEFCHAR_TOWN 				= 0;
const SI08			DEFCHAR_TOWNPRIV 			= 0;
const UI16			DEFCHAR_ADVOBJ 				= 0;
const SERIAL		DEFCHAR_GUILDFEALTY			= INVALIDSERIAL;
const SI16			DEFCHAR_GUILDNUMBER			= -1;
const UI08			DEFCHAR_FLAG 				= 0x04;
const SI08			DEFCHAR_SPELLCAST 			= -1;
const UI08			DEFCHAR_NEXTACTION 			= 0;
const SI08			DEFCHAR_STEALTH				= -1;
const UI08			DEFCHAR_RUNNING				= 0;
const RACEID		DEFCHAR_RACEGATE 			= INVALIDID;
const UI08			DEFCHAR_STEP				= 1;
const UI16			DEFCHAR_PRIV				= 0;
const UI16			DEFCHAR_NOMOVE 				= 0;
const UI16			DEFCHAR_POISONCHANCE 		= 0;
const UI08			DEFCHAR_POISONSTRENGTH 		= 0;
const SI32			DEFCHAR_TEMPWEIGHT 			= 0;
const UI16			DEFCHAR_FIRERESIST 			= 0;
const UI16			DEFCHAR_COLDRESIST 			= 0;
const UI16			DEFCHAR_ENERGYRESIST 		= 0;
const UI16			DEFCHAR_POISONRESIST		= 0;

CChar::CChar() : CBaseObject(),
townvote( DEFCHAR_TOWNVOTE ), bools( DEFCHAR_BOOLS ), bools2( DEFCHAR_BOOLS2 ), tempWeight( DEFCHAR_TEMPWEIGHT), 
fonttype( DEFCHAR_FONTTYPE ), maxHP( DEFCHAR_MAXHP ), maxHP_oldstr( DEFCHAR_MAXHP_OLDSTR ), 
oldRace( DEFCHAR_OLDRACE ), maxMana( DEFCHAR_MAXMANA ), maxMana_oldint( DEFCHAR_MAXMANA_OLDINT ),
maxStam( DEFCHAR_MAXSTAM ), maxStam_olddex( DEFCHAR_MAXSTAM_OLDDEX ), saycolor( DEFCHAR_SAYCOLOUR ), 
emotecolor( DEFCHAR_EMOTECOLOUR ), cell( DEFCHAR_CELL ), packitem( NULL ), 
targ( DEFCHAR_TARG ), attacker( DEFCHAR_ATTACKER ), hunger( DEFCHAR_HUNGER ), regionNum( DEFCHAR_REGIONNUM ), town( DEFCHAR_TOWN ), 
townpriv( DEFCHAR_TOWNPRIV ), advobj( DEFCHAR_ADVOBJ ), guildfealty( DEFCHAR_GUILDFEALTY ), guildnumber( DEFCHAR_GUILDNUMBER ), flag( DEFCHAR_FLAG ), 
spellCast( DEFCHAR_SPELLCAST ), nextact( DEFCHAR_NEXTACTION ), stealth( DEFCHAR_STEALTH ), running( DEFCHAR_RUNNING ), 
raceGate( DEFCHAR_RACEGATE ), step( DEFCHAR_STEP ), priv( DEFCHAR_PRIV ), PoisonStrength( DEFCHAR_POISONSTRENGTH ),
fireResist( DEFCHAR_FIRERESIST ), coldResist( DEFCHAR_COLDRESIST ), energyResist( DEFCHAR_ENERGYRESIST ), poisonResist( DEFCHAR_POISONRESIST )
{
	id		= 0x0190;
	objType = OT_CHAR;
	name	= "Mr. noname";
	memset( charTimers, 0, sizeof( charTimers[0] ) * tCHAR_COUNT );
	memset( baseskill, 0, sizeof( baseskill[0] ) * (ALLSKILLS+1) );
	memset( skill, 0, sizeof( skill[0] ) * (INTELLECT+1) );
	memset( lockState, 0, sizeof( lockState[0] ) * (INTELLECT+1) );
	for( UI08 j = 0; j <= ALLSKILLS; ++j )
		atrophy[j] = j;
	SetCanTrain( true );

	SetHungerStatus( true );
	SetTamedHungerRate( 0 );
	SetTamedHungerWildChance( 0 );
	
	foodList.reserve( MAX_NAME );
	
	memset( weathDamage, 0, sizeof( weathDamage[0] ) * WEATHNUM );
	skillUsed[0] = skillUsed[1] = 0;
	memset( regen, 0, sizeof( UI32 ) * 3 );

	strength = dexterity = intelligence = 1;

	itemLayers.clear();
	layerCtr = itemLayers.end();

	mPlayer	= NULL;
	mNPC	= NULL;

	SetMaxHPFixed( false );
	SetMaxManaFixed( false );
	SetMaxStamFixed( false );
	SetCanAttack( true );
	SetBrkPeaceChanceGain( 0 );
	SetBrkPeaceChance( 0 );
}

CChar::~CChar()	// Destructor to clean things up when deleted
{
	if( IsValidNPC() )
	{
		delete mNPC;
		mNPC = NULL;
	}

	if( IsValidPlayer() )
	{
		delete mPlayer;
		mPlayer = NULL;
	}
}

void CChar::CreateNPC( void )
{
	if( !IsValidNPC() )
		mNPC = new NPCValues_st();
}

void CChar::CreatePlayer( void )
{
	if( !IsValidPlayer() )
		mPlayer = new PlayerValues_st();
}

bool CChar::IsValidNPC( void ) const
{
	return ( mNPC != NULL );
}

bool CChar::IsValidPlayer( void ) const
{
	return ( mPlayer != NULL );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI32 TempWeight()
//|   Date        -  11. Mar, 20006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Temporary weight modificator
//o---------------------------------------------------------------------------o
SI32 CChar::GetTempWeight( void ) const
{
	return tempWeight;
}
void CChar::SetTempWeight( SI32 newValue )
{
	tempWeight = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 DamageResist()
//|   Date        -  11. Mar, 20006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Set and Get the damage resist values
//o---------------------------------------------------------------------------o
void CChar::SetDamageResist( UI16 newValue, DamageTypes damage )
{
	switch( damage )
	{
		case DAMAGE_FIRE:
			fireResist = newValue;
			break;
		case DAMAGE_COLD:
			coldResist = newValue;
			break;
		case DAMAGE_ENERGY:
			energyResist = newValue;
			break;
		case DAMAGE_POISON:
			poisonResist = newValue;
			break;
		default:
			break;
	}
}
UI16 CChar::GetDamageResist( DamageTypes damage ) const
{
	switch( damage )
	{
		case DAMAGE_FIRE:
			return fireResist;
			break;
		case DAMAGE_COLD:
			return coldResist;
			break;
		case DAMAGE_ENERGY:
			return energyResist;
			break;
		case DAMAGE_POISON:
			return poisonResist;
			break;
		default:
			return 0;
			break;
	}
}

void CChar::IncreaseDamageResist( DamageTypes damage )
{
	if( damage == DAMAGE_NORM )
		return;

	// Increase damage resistance, very basic
	UI16 damageResist = GetDamageResist( damage );
	if( damageResist <= 1000 )
		SetDamageResist( damageResist + 100, damage );
	else if( damageResist <= 5000 )
		SetDamageResist( damageResist + 50, damage );
	else if( damageResist <= 9000 )
		SetDamageResist( damageResist + 10, damage );
	else
		SetDamageResist( damageResist + 1, damage );
	return;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Hunger()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Hunger level of the character
//o---------------------------------------------------------------------------o
SI08 CChar::GetHunger( void ) const
{
	return hunger;
}
void CChar::SetHunger( SI08 newValue )
{
	hunger = newValue;
}
UI16 CChar::GetTamedHungerRate( void ) const
{
	return hungerRate;
}
void CChar::SetTamedHungerRate( UI16 newValue )
{
	hungerRate = newValue;
}
UI08 CChar::GetTamedHungerWildChance( void ) const
{
	return hungerWildChance;
}
void CChar::SetTamedHungerWildChance( UI08 newValue )
{
	hungerWildChance = newValue;
}

std::string CChar::GetFood( void ) const
{
	return foodList;
}

void CChar::SetFood( std::string food )
{
	foodList = food.substr( 0, MAX_NAME - 1 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void DoHunger()
//|   Date        -  21. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Calculate Hunger level of the character and do all
//|					 related effects.
//o---------------------------------------------------------------------------o
void CChar::DoHunger( CSocket *mSock )
{
	UI16 hungerRate;
	SI16 hungerDamage;

	if ( !IsDead() && !IsInvulnerable() )	// No need to do anything on dead or invulnerable chars
	{
		if( !IsNpc() && mSock != NULL )	// Do Hunger for player chars
		{
			if( WillHunger() && GetCommandLevel() == PLAYER_CMDLEVEL  )
			{
				if( GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					if( Races->DoesHunger( GetRace() ) ) // prefer the hunger settings frome the race
					{
						hungerRate	 = Races->GetHungerRate( GetRace() );
						hungerDamage = Races->GetHungerDamage( GetRace() );
					}
					else // use the global values if there is no race setting
					{
						hungerRate   = cwmWorldState->ServerData()->SystemTimer( tSERVER_HUNGERRATE );
						hungerDamage = cwmWorldState->ServerData()->HungerDamage();
					}

					if( GetHunger() > 0 )
					{
						DecHunger();
						const UI16 HungerTrig = GetScriptTrigger();
						cScript *toExecute = JSMapping->GetScript( HungerTrig );
						cScript *globalExecute = JSMapping->GetScript( (UI16)0 );
						bool doHunger = true;
						if( toExecute != NULL )
						{
							doHunger = !toExecute->OnHungerChange( (this), GetHunger() );
						}
						else if( globalExecute != NULL )
						{
							doHunger = !globalExecute->OnHungerChange( (this), GetHunger() );
						}
						if( doHunger )
						{
							switch( GetHunger() )
							{
								default:
								case 6:								break;
								case 5:	mSock->sysmessage( 1222 );	break;
								case 4:	mSock->sysmessage( 1223 );	break;
								case 3:	mSock->sysmessage( 1224 );	break;
								case 2:	mSock->sysmessage( 1225 );	break;
								case 1:	mSock->sysmessage( 1226 );	break;
								case 0:	mSock->sysmessage( 1227 );	break;	
							}
						}
					}
					else if( GetHP() > 0 && hungerDamage > 0)
					{
						mSock->sysmessage( 1228 );
						Damage( hungerDamage );
						if( GetHP() <= 0 )
							mSock->sysmessage( 1229 );
					}
					SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>(hungerRate) ) );
				}
			}
		}
		else if( IsNpc() && !IsTamed() && Races->DoesHunger( GetRace() ) )
		{
			if( WillHunger() && !GetMounted() && !GetStabled() )
			{
				if( GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					hungerRate	 = Races->GetHungerRate( GetRace() );
					hungerDamage = Races->GetHungerDamage( GetRace() );

					if( GetHunger() > 0 )
					{
						DecHunger();
						const UI16 HungerTrig = GetScriptTrigger();
						cScript *toExecute = JSMapping->GetScript( HungerTrig );
						cScript *globalExecute = JSMapping->GetScript( (UI16)0 );
						if( toExecute != NULL )
						{
							toExecute->OnHungerChange( (this), GetHunger() );
						}
						else if( globalExecute != NULL )
						{
							globalExecute->OnHungerChange( (this), GetHunger() );
						}
					}
					else if( GetHP() > 0 && hungerDamage > 0)
					{
						Damage( hungerDamage );
					}
					SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>(hungerRate) ) );
				}
			}
		}
		else if( IsTamed() && GetTamedHungerRate() > 0 )
		{
			if( WillHunger() && !GetMounted() && !GetStabled() )
			{
				if( cwmWorldState->ServerData()->PetHungerOffline() == false )
				{
					CChar *owner = GetOwnerObj();
					if( !ValidateObject( owner ) )
						return;

					if( !isOnline( (*owner) ) )
						return;
				}

				if( GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					hungerRate = GetTamedHungerRate();

					if( GetHunger() > 0 )
					{
						DecHunger();
						const UI16 HungerTrig = GetScriptTrigger();
						cScript *toExecute = JSMapping->GetScript( HungerTrig );
						cScript *globalExecute = JSMapping->GetScript( (UI16)0 );
						if( toExecute != NULL )
						{
							toExecute->OnHungerChange( (this), GetHunger() );
						}
						else if( globalExecute != NULL )
						{
							globalExecute->OnHungerChange( (this), GetHunger() );
						}
					}
					else if( (UI08)RandomNum( 0, 100 ) <= GetTamedHungerWildChance() )
					{
						SetOwner( NULL );
						SetHunger( 6 );
					}
					SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>(hungerRate) ) );
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void checkPetOfflineTimeout()
//|   Date        -  21. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Calculate Hunger level of the character and do all
//|					 related effects.
//o---------------------------------------------------------------------------o
void CChar::checkPetOfflineTimeout( void )
{
	if( IsTamed() )
	{
		if( GetMounted() || GetStabled() )
			return;

		CChar *owner = GetOwnerObj();
		if( !ValidateObject( owner ) )
		{
			SetTamed( false ); // The owner is gone, so reset the tamed status
		}
		else
		{
			if( isOnline( (*owner) ) )
				return; // The owner is still online, so leave it alone

			time_t currTime, lastOnTime;
			const UI32 offlineTimeout = static_cast<UI32>(cwmWorldState->ServerData()->PetOfflineTimeout() * 3600 * 24);

			time( &currTime );
			lastOnTime = static_cast<time_t>(GetLastOnSecs());
			
			if( currTime > 0 && lastOnTime > 0)
				if( difftime( currTime, lastOnTime) >= offlineTimeout )
				{
					SetOwner( NULL );
					SetHunger( 6 );
				}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 Town()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  The town the character belongs to
//o---------------------------------------------------------------------------o
UI08 CChar::GetTown( void ) const
{
	return town;
}
void CChar::SetTown( UI08 newValue )
{
	town = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 BrkPeaceChanceGain()
//|   Date        -  25. Feb, Grimson
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  The chance gain to break peace
//o---------------------------------------------------------------------------o
UI08 CChar::GetBrkPeaceChanceGain( void ) const
{
	if( IsNpc() )
		return brkPeaceChanceGain;
	else
		return static_cast<UI08>( GetBaseSkill( 9 ) / 10);
}
void CChar::SetBrkPeaceChanceGain( UI08 newValue )
{
	brkPeaceChanceGain = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 CurrBrkPeaceChanceGain()
//|   Date        -  25. Feb, Grimson
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  The current chance to break peace
//o---------------------------------------------------------------------------o
UI08 CChar::GetBrkPeaceChance( void ) const
{
	return brkPeaceChance;
}
void CChar::SetBrkPeaceChance( UI08 newValue )
{
	brkPeaceChance = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool isUnicode( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is speaking in unicode
//o---------------------------------------------------------------------------o
bool CChar::isUnicode( void ) const
{
	return MFLAGGET( bools, BIT_UNICODE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool isNpc( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is an npc
//o---------------------------------------------------------------------------o
bool CChar::IsNpc( void ) const
{
	return MFLAGGET( bools, BIT_NPC );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsShop( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is a shopkeeper
//o---------------------------------------------------------------------------o
bool CChar::IsShop( void ) const
{
	return MFLAGGET( bools, BIT_SHOP );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsDead( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is dead
//o---------------------------------------------------------------------------o
bool CChar::IsDead( void ) const
{
	return MFLAGGET( bools, BIT_DEAD );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool GetCanAttack( void ) const
//|   Date        -  25. Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns whether the char can attack targets
//o---------------------------------------------------------------------------o
bool CChar::GetCanAttack( void ) const
{
	return MFLAGGET( bools2, BIT_CANATTACK );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsAtWar( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is at war
//o---------------------------------------------------------------------------o
bool CChar::IsAtWar( void ) const
{
	return MFLAGGET( bools, BIT_ATWAR );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool DidAttackFirst( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character attacked first
//o---------------------------------------------------------------------------o
bool CChar::DidAttackFirst( void ) const
{
	return MFLAGGET( bools, BIT_ATTACKFIRST );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsOnHorse( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is on a horse
//o---------------------------------------------------------------------------o
bool CChar::IsOnHorse( void ) const
{
	return MFLAGGET( bools, BIT_ONHORSE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool GetTownTitle( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character's town info is displayed
//o---------------------------------------------------------------------------o
bool CChar::GetTownTitle( void ) const
{
	return MFLAGGET( bools, BIT_TOWNTITLE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool GetReactiveArmour( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character has reactive armour
//o---------------------------------------------------------------------------o
bool CChar::GetReactiveArmour( void ) const
{
	return MFLAGGET( bools, BIT_REACTIVEARMOUR );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool CanTrain( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is can train
//o---------------------------------------------------------------------------o
bool CChar::CanTrain( void ) const
{
	return MFLAGGET( bools, BIT_TRAIN );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool GetGuildToggle( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character displays guild information
//o---------------------------------------------------------------------------o
bool CChar::GetGuildToggle( void ) const
{
	return MFLAGGET( bools, BIT_GUILDTOGGLE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsTamed( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is tamed
//o---------------------------------------------------------------------------o
bool CChar::IsTamed( void ) const
{
	return MFLAGGET( bools, BIT_TAMED );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsGuarded( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is guarded
//o---------------------------------------------------------------------------o
bool CChar::IsGuarded( void ) const
{
	return MFLAGGET( bools, BIT_GUARDED );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool CanRun( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character can run
//o---------------------------------------------------------------------------o
bool CChar::CanRun( void ) const
{
	return MFLAGGET( bools, BIT_RUN );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsPolymorphed( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is polymorphed
//o---------------------------------------------------------------------------o
bool CChar::IsPolymorphed( void ) const
{
	return MFLAGGET( bools, BIT_POLYMORPHED );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsIncognito( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is incognito
//o---------------------------------------------------------------------------o
bool CChar::IsIncognito( void ) const
{
	return MFLAGGET( bools, BIT_INCOGNITO );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool IsUsingPotion( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is using a potion
//o---------------------------------------------------------------------------o
bool CChar::IsUsingPotion( void ) const
{
	return MFLAGGET( bools, BIT_USINGPOTION );
}

bool CChar::MayLevitate( void ) const
{
	return MFLAGGET( bools, BIT_MAYLEVITATE );
}

bool CChar::WillHunger( void ) const
{
	return MFLAGGET( bools, BIT_WILLHUNGER );
}

bool CChar::IsMeditating( void ) const
{
	return MFLAGGET( bools, BIT_MEDITATING );
}

bool CChar::IsCasting( void ) const
{
	return MFLAGGET( bools, BIT_CASTING );
}

bool CChar::IsJSCasting( void ) const
{
	return MFLAGGET( bools, BIT_JSCASTING );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void setUnicode( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's unicode speech status
//o---------------------------------------------------------------------------o
void CChar::setUnicode( bool newVal )
{
	MFLAGSET( bools, newVal, BIT_UNICODE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetNpc( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's npc status
//o---------------------------------------------------------------------------o
void CChar::SetNpc( bool newVal )
{
	MFLAGSET( bools, newVal, BIT_NPC );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetShop( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's shopkeeper status
//o---------------------------------------------------------------------------o
void CChar::SetShop( bool newVal )
{
	MFLAGSET( bools, newVal, BIT_SHOP );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetDead( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's dead status
//o---------------------------------------------------------------------------o
void CChar::SetDead( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_DEAD );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetCanAttack( bool newVal ) 
//|   Date        -  25.Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's ability to attack
//o---------------------------------------------------------------------------o
void CChar::SetCanAttack( bool newValue )
{
	MFLAGSET( bools2, newValue, BIT_CANATTACK );
	SetBrkPeaceChance( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetPeace( bool newVal ) 
//|   Date        -  25.Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character to peace
//o---------------------------------------------------------------------------o
void CChar::SetPeace( UI32 newValue )
{
	if( newValue > 0 )
	{
		SetCanAttack( false );
		if( IsAtWar() )
			SetWar( false );
		SetTarg( NULL );
		SetAttacker( NULL );
		SetAttackFirst( NULL );
		SetTimer( tCHAR_PEACETIMER, BuildTimeValue( newValue ) );
	}
	else
	{
		SetCanAttack( true );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetWar( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's at war status
//o---------------------------------------------------------------------------o
void CChar::SetWar( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_ATWAR );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetAttackFirst( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character attacked first
//o---------------------------------------------------------------------------o
void CChar::SetAttackFirst( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_ATTACKFIRST );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetOnHorse( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character is on a horse or not
//o---------------------------------------------------------------------------o
void CChar::SetOnHorse( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_ONHORSE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetTownTitle( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's town title display status
//o---------------------------------------------------------------------------o
void CChar::SetTownTitle( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_TOWNTITLE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetReactiveArmour( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character has reactive armour
//o---------------------------------------------------------------------------o
void CChar::SetReactiveArmour( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_REACTIVEARMOUR );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetCanTrain( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's trainer status
//o---------------------------------------------------------------------------o
void CChar::SetCanTrain( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_TRAIN );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetGuildToggle( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's guild display status
//o---------------------------------------------------------------------------o
void CChar::SetGuildToggle( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_GUILDTOGGLE );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetTamed( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's tamed status
//o---------------------------------------------------------------------------o
void CChar::SetTamed( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_TAMED );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetGuarded( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the character's guarded status
//o---------------------------------------------------------------------------o
void CChar::SetGuarded( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_GUARDED );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetRun( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character can run
//o---------------------------------------------------------------------------o
void CChar::SetRun( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_RUN );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void IsPolymorphed( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character is polymorphed
//o---------------------------------------------------------------------------o
void CChar::IsPolymorphed( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_POLYMORPHED );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void IsIncognito( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character is incognito
//o---------------------------------------------------------------------------o
void CChar::IsIncognito( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_INCOGNITO );
}
//o---------------------------------------------------------------------------o
//|   Function    -  void SetUsingPotion( bool newVal )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the character is using a potion or not
//o---------------------------------------------------------------------------o
void CChar::SetUsingPotion( bool newVal )
{
	MFLAGSET( bools, newVal, BIT_USINGPOTION );
}

void CChar::SetLevitate( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_MAYLEVITATE );
}

void CChar::SetHungerStatus( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_WILLHUNGER );
}

void CChar::SetMeditating( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_MEDITATING );
}

void CChar::SetCasting( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_CASTING );
}

// This is only a temporary measure until ALL code is switched over to JS code
// As it stands, it'll try and auto-direct cast if you set casting and spell timeouts
void CChar::SetJSCasting( bool newValue )
{
	MFLAGSET( bools, newValue, BIT_JSCASTING );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetInBuilding( bool newValue ) 
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Set's whether a char is inside a building or not
//o---------------------------------------------------------------------------o
void CChar::SetInBuilding( bool newValue )
{
	isInBuilding = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemoveSelfFromOwner( void ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes character from it's owner's pet list
//o---------------------------------------------------------------------------o
void CChar::RemoveSelfFromOwner( void )
{
	CChar *oldOwner = GetOwnerObj();
	if( ValidateObject( oldOwner ) )
		oldOwner->GetPetList()->Remove( this );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddSelfToOwner( void ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds character to it's new owner's pet list
//o---------------------------------------------------------------------------o
void CChar::AddSelfToOwner( void )
{
	CChar *newOwner = GetOwnerObj();
	if( !ValidateObject( newOwner ) )
	{
		SetTamed( false );
		SetNpcWander( 2 );
	}
	else
	{
		newOwner->GetPetList()->Add( this );
		SetTamed( true );
	}
	UpdateFlag( this );
	Dirty( UT_UPDATE );
}

UI32 CChar::GetTownVote( void ) const
{
	return townvote;
}
void CChar::SetTownVote( UI32 newValue )
{
	townvote = newValue;
}

UI32 CChar::GetGuildFealty( void ) const
{
	return guildfealty;
}
void CChar::SetGuildFealty( UI32 newValue )
{
	guildfealty = newValue;
}

std::string CChar::GetGuildTitle( void ) const
{
	return guildtitle;
}
void CChar::SetGuildTitle( std::string newValue )
{
	guildtitle = newValue;
}

TIMERVAL CChar::GetRegen( UI08 part ) const
{
	return regen[part];
}
void CChar::SetRegen( TIMERVAL newValue, UI08 part )
{
	regen[part] = newValue;
}

TIMERVAL CChar::GetWeathDamage( UI08 part ) const
{
	return weathDamage[part];
}
void CChar::SetWeathDamage( TIMERVAL newValue, UI08 part )
{
	weathDamage[part] = newValue;
}

UI08 CChar::GetNextAct( void ) const
{
	return nextact;
}

void CChar::SetNextAct( UI08 newVal )
{
	nextact = newVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	TIMERVAL Timer()
//|	Date		-	Unknown
//|	Programmer	-	giwo
//o---------------------------------------------------------------------------o
//|	Purpose		-	Timer values for the character
//o---------------------------------------------------------------------------o
TIMERVAL CChar::GetTimer( cC_TID timerID ) const
{
	TIMERVAL rvalue = 0;
	if( timerID != tCHAR_COUNT )
		rvalue = charTimers[timerID];
	return rvalue;
}
void CChar::SetTimer( cC_TID timerID, TIMERVAL value )
{
	if( timerID != tCHAR_COUNT )
		charTimers[timerID] = value;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	UI08 PoisonStrength()
//|	Date			-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
UI08 CChar::GetPoisonStrength( void ) const
{
	return PoisonStrength;
}
void CChar::SetPoisonStrength( UI08 value ) 
{
	PoisonStrength = value;
}

COLOUR CChar::GetEmoteColour( void ) const
{
	return emotecolor;
}
void CChar::SetEmoteColour( COLOUR newValue )
{
	emotecolor = newValue;
}

COLOUR CChar::GetSayColour( void ) const
{
	return saycolor;
}
void CChar::SetSayColour( COLOUR newValue )
{
	saycolor = newValue;
}

UI16 CChar::GetSkin( void ) const
{
	return GetColour();
}
void CChar::SetSkin( UI16 value )
{
	SetColour( value );
}

SI08 CChar::GetStealth( void ) const
{
	return stealth;
}
void CChar::SetStealth( SI08 newValue )
{
	stealth = newValue;
}

SI08 CChar::GetCell( void ) const
{
	return cell;
}
void CChar::SetCell( SI08 newVal )
{
	cell = newVal;
}

UI08 CChar::GetRunning( void ) const
{
	return running;
}
void CChar::SetRunning( UI08 newValue )
{
	running = newValue;
}

UI08 CChar::GetStep( void ) const
{
	return step;
}
void CChar::SetStep( UI08 newValue )
{
	step = newValue;
}

CTownRegion *CChar::GetRegion( void ) const
{
	if( cwmWorldState->townRegions.find( regionNum ) == cwmWorldState->townRegions.end() )
		return cwmWorldState->townRegions[0xFF];
	return cwmWorldState->townRegions[regionNum];
}
void CChar::SetRegion( UI08 newValue )
{
	regionNum = newValue;
}
UI08 CChar::GetRegionNum( void ) const
{
	return regionNum;
}

void CChar::SetLocation( const CBaseObject *toSet )
{
	if( toSet != NULL )
		SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber() );
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world )
{
	Dirty( UT_LOCATION );
	MapRegion->ChangeRegion( this, newX, newY, world );
	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
	x		= newX;
	y		= newY;
	z		= newZ;
	WorldNumber( world );
	CMultiObj *newMulti = findMulti( newX, newY, newZ, world );
	if( GetMultiObj() != newMulti )
		SetMulti( newMulti );
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	SetLocation( newX, newY, newZ, WorldNumber() );
}

CItem * CChar::GetPackItem( void )
{
	if( packitem == NULL )
	{
		CItem *tempItem = GetItemAtLayer( IL_PACKITEM );
		if( ValidateObject( tempItem ) && tempItem->GetType() == IT_CONTAINER )
			packitem = tempItem;
	}
	return packitem;
}
CChar *CChar::GetTarg( void ) const
{
	return calcCharObjFromSer( targ );
}
CChar *CChar::GetAttacker( void ) const
{
	return calcCharObjFromSer( attacker );
}

UI16 CChar::GetAdvObj( void ) const
{
	return advobj;
}

RACEID CChar::GetRaceGate( void ) const
{
	return raceGate;
}
void CChar::SetPackItem( CItem *newVal )
{
	packitem = newVal;
}

SERIAL calcSerFromObj( CBaseObject *mObj )
{
	SERIAL toReturn = INVALIDSERIAL;
	if( ValidateObject( mObj ) )
		toReturn = mObj->GetSerial();
	return toReturn;
}

void CChar::SetTarg( CChar *newTarg )
{
	targ = calcSerFromObj( newTarg );
}
void CChar::SetAttacker( CChar *newValue )
{
	attacker = calcSerFromObj( newValue );
}

void CChar::SetAdvObj( UI16 newValue )
{
	advobj = newValue;
}

void CChar::SetRaceGate( RACEID newValue )
{
	raceGate = newValue;
}

SI08 CChar::GetSpellCast( void ) const
{
	return spellCast;
}

void CChar::SetSpellCast( SI08 newValue )
{
	spellCast = newValue;
}

UI16 CChar::GetPriv( void ) const
{
	return priv;
}
SI08 CChar::GetTownPriv( void ) const
{
	return townpriv;
}
void CChar::SetPriv( UI16 newValue )
{
	priv = newValue;
}

void CChar::SetTownpriv( SI08 newValue )
{
	townpriv = newValue;
}

SKILLVAL CChar::GetBaseSkill( UI08 skillToGet ) const
{
	return baseskill[skillToGet];
}
UI16 CChar::GetAtrophy( UI08 skillToGet ) const
{
	return atrophy[skillToGet];
}
UI08 CChar::GetSkillLock( UI08 skillToGet ) const
{
	return lockState[skillToGet];
}
SKILLVAL CChar::GetSkill( UI08 skillToGet ) const
{
	SKILLVAL rvalue		= skill[skillToGet];
	SI32 modifier	= Races->DamageFromSkill( skillToGet, race );
	if( modifier != 0 )
	{
		SKILLVAL toAdd	= (SKILLVAL)( (R32)skill[skillToGet] * ( (R32)modifier / 1000 ) );		// percentage to add
		rvalue		+= toAdd; // return the bonus
	}
	return rvalue;
}

void CChar::SetBaseSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	baseskill[skillToSet] = newSkillValue;
}
void CChar::SetSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	skill[skillToSet] = newSkillValue;
}
void CChar::SetAtrophy( UI16 newValue, UI08 skillToSet )
{
	atrophy[skillToSet] = newValue;
}
void CChar::SetSkillLock( UI08 newSkillValue, UI08 skillToSet )
{
	lockState[skillToSet] = newSkillValue;
}

SI16 CChar::GetGuildNumber( void ) const
{
	return guildnumber;
}
UI08 CChar::GetFlag( void ) const
{
	return flag;
}

void CChar::SetFlag( UI08 newValue )
{
	flag = newValue;
}
void CChar::SetGuildNumber( SI16 newValue )
{
	guildnumber = newValue;
}

SI08 CChar::GetFontType( void ) const
{
	return fonttype;
}

void CChar::SetFontType( SI08 newType )
{
	fonttype = newType;
}

bool CChar::IsGM( void ) const
{
	return MFLAGGET( priv, BIT_GM );
}
bool CChar::CanBroadcast( void ) const
{
	return MFLAGGET( priv, BIT_BROADCAST );
}
bool CChar::IsInvulnerable( void ) const
{
	return MFLAGGET( priv, BIT_INVULNERABLE );
}
bool CChar::GetSingClickSer( void ) const
{
	return MFLAGGET( priv, BIT_SINGCLICKSER );
}
bool CChar::NoSkillTitles( void ) const
{
	return MFLAGGET( priv, BIT_SKILLTITLES );
}
bool CChar::IsGMPageable( void ) const
{
	return MFLAGGET( priv, BIT_GMPAGEABLE );
}
bool CChar::CanSnoop( void ) const
{
	return MFLAGGET( priv, BIT_SNOOP );
}
bool CChar::IsCounselor( void ) const
{
	return MFLAGGET( priv, BIT_COUNSELOR );
}

bool CChar::AllMove( void ) const
{
	return MFLAGGET( priv, BIT_ALLMOVE );
}
bool CChar::IsFrozen( void ) const
{
	return MFLAGGET( priv, BIT_FROZEN );
}
bool CChar::ViewHouseAsIcon( void ) const
{
	return MFLAGGET( priv, BIT_VIEWHOUSEASICON );
}
// 0x0800 is free
bool CChar::NoNeedMana( void ) const
{
	return MFLAGGET( priv, BIT_NONEEDMANA );
}
bool CChar::IsDispellable( void ) const
{
	return MFLAGGET( priv, BIT_DISPELLABLE );
}
bool CChar::IsPermReflected( void ) const
{
	return MFLAGGET( priv, BIT_PERMREFLECTED );
}
bool CChar::NoNeedReags( void ) const
{
	return MFLAGGET( priv, BIT_NONEEDREAGS );
}

void CChar::SetGM( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_GM );
}
void CChar::SetBroadcast( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_BROADCAST );
}
void CChar::SetInvulnerable( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_INVULNERABLE );
}
void CChar::SetSingClickSer( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_SINGCLICKSER );
}
void CChar::SetSkillTitles( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_SKILLTITLES );
}
void CChar::SetGMPageable( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_GMPAGEABLE );
}
void CChar::SetSnoop( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_SNOOP );
}
void CChar::SetCounselor( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_COUNSELOR );
}

void CChar::SetAllMove( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_ALLMOVE );
}
void CChar::SetFrozen( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_FROZEN );
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_VIEWHOUSEASICON );
}
// 0x0800 is free
void CChar::SetNoNeedMana( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_NONEEDMANA );
}
void CChar::SetDispellable( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_DISPELLABLE );
}
void CChar::SetPermReflected( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_PERMREFLECTED );
}
void CChar::SetNoNeedReags( bool newValue )
{
	MFLAGSET( priv, newValue, BIT_NONEEDREAGS );
}

CChar *CChar::Dupe( void )
{
	CChar *target = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
	if( target == NULL )
		return NULL;

	CBaseObject::CopyData( target );
	CopyData( target );

	return target;
}

void CChar::CopyData( CChar *target )
{
	target->SetMulti( GetMultiObj() );
	target->SetOwner( GetOwnerObj() );
	target->SetSpawn( GetSpawn() );
	target->SetTownVote( townvote );
	target->SetPriv( GetPriv() );
	target->SetName( name );
	target->SetTitle( title );
	target->SetLocation( this );
	target->SetDir( dir );
	target->SetID( id );
	target->SetSkin( GetColour() );
	target->SetFontType( fonttype );
	target->SetSayColour( saycolor );
	target->SetEmoteColour( emotecolor );
	target->SetStrength( strength );
	target->SetDexterity( dexterity );
	target->SetIntelligence( intelligence );
	if ( GetMaxHPFixed() )
		target->SetFixedMaxHP(  maxHP );
	if ( GetMaxManaFixed() )
		target->SetFixedMaxMana(  maxMana );
	if ( GetMaxStamFixed() )
		target->SetFixedMaxStam(  maxStam );
	target->SetHP(  hitpoints );
	target->SetStamina( stamina );
	target->SetMana( mana );

	target->SetStrength2( st2 );
	target->SetDexterity2( dx2 );
	target->SetIntelligence2( in2 );
	target->SetTempWeight( tempWeight);
	
	target->SetHiDamage( hidamage );
	target->SetLoDamage( lodamage );

	for( UI08 i = 0; i <= ALLSKILLS; ++i )
	{
		target->SetBaseSkill( baseskill[i], i );
		target->SetSkill( skill[i], i );
		target->SetAtrophy( atrophy[i], i );
		target->SetSkillLock( lockState[i], i );
	}

	for( UI08 j = STRENGTH; j <= INTELLECT+1; ++j )
	{
		target->SetAtrophy( atrophy[j], j );
		target->SetSkillLock( lockState[j], j );
	}

	target->SetCell( cell );
	target->SetPackItem( packitem );
	target->SetWeight( weight );
	target->SetDef( def );
	target->SetTarg( GetTarg() );
	target->SetRegen( regen[0], 0 );
	target->SetRegen( regen[1], 1 );
	target->SetRegen( regen[2], 2 );
	target->SetAttacker( GetAttacker() );
	target->SetVisible( GetVisible() );

	for( int mTID = (int)tCHAR_TIMEOUT; mTID < (int)tCHAR_COUNT; ++mTID )
		target->SetTimer( (cC_TID)mTID, GetTimer( (cC_TID)mTID ) );
	target->SetHunger( hunger );
	target->SetTamedHungerRate( hungerRate );
	target->SetTamedHungerWildChance( hungerWildChance );
	target->SetBrkPeaceChance( GetBrkPeaceChance() );
	target->SetBrkPeaceChanceGain( GetBrkPeaceChanceGain() );
	target->SetFood( foodList );
	target->SetRegion( regionNum );
	target->SetTown( town );
	target->SetTownpriv( townpriv );
	target->SetAdvObj( advobj );
	target->SetDisabled( isDisabled() );
	target->SetCanTrain( CanTrain() );
	target->SetLastOn( GetLastOn() );
	target->SetLastOnSecs( GetLastOnSecs() );
	target->SetGuildTitle( guildtitle );
	target->SetGuildFealty( guildfealty );
	target->SetGuildNumber( guildnumber );
	target->SetFlag( flag );
	target->SetCasting( IsCasting() );
	target->SetJSCasting( IsJSCasting() );
	target->SetSpellCast( spellCast );
	target->SetNextAct( nextact );
	target->SetSquelched( GetSquelched() );
	target->SetMeditating( IsMeditating() );
	target->SetStealth( stealth );
	target->SetRunning( running );
	target->SetRace( GetRace() );
	target->SetRaceGate( raceGate );
	target->SetCarve( carve );
	for( UI08 counter2 = 0; counter2 < WEATHNUM; ++counter2 )
	{
		target->SetWeathDamage( weathDamage[counter2], counter2 );
	}
	if( IsValidNPC() )
	{
		target->SetFleeAt( GetFleeAt() );
		target->SetReattackAt( GetReattackAt() );
		target->SetNpcWander( GetNpcWander() );
		target->SetOldNpcWander( GetOldNpcWander() );
		target->SetFTarg( GetFTarg() );
		target->SetFx( GetFx( 0 ), 0 );
		target->SetFx( GetFx( 1 ), 1 );
		target->SetFy( GetFy( 0 ), 0 );
		target->SetFy( GetFy( 1 ), 1 );
		target->SetFz( GetFz() );
		target->SetTaming( GetTaming() );
		target->SetPeaceing( GetPeaceing() );
		target->SetProvoing( GetProvoing() );
		target->SetNPCAiType( GetNPCAiType() );
		target->SetSpAttack( GetSpAttack() );
		target->SetSpDelay( GetSpDelay() );
		target->SetSplit( GetSplit() );
		target->SetSplitChance( GetSplitChance() );
		target->SetTrainingPlayerIn( GetTrainingPlayerIn() );
		target->SetHoldG( GetHoldG() );
		target->SetQuestType( GetQuestType() );
		target->SetQuestDestRegion( GetQuestDestRegion() );
		target->SetQuestOrigRegion( GetQuestOrigRegion() );
	}
	if( IsValidPlayer() )
	{
		target->SetDeaths( GetDeaths() );
		target->SetFixedLight( GetFixedLight() );
		target->SetGuarding( GetGuarding() );
		target->SetOrgName( GetOrgName() );
		target->SetRobe( GetRobe() );
		target->SetAccount( GetAccount() );
		target->SetOrgID( GetOrgID() );
		target->SetCallNum( GetCallNum() );
		target->SetPlayerCallNum( GetPlayerCallNum() );
		target->SetCommandLevel( GetCommandLevel() );
		target->SetPostType( GetPostType() );
		target->SetTrackingTarget( GetTrackingTarget() );
		for( UI08 counter = 0; counter < mPlayer->trackingTargets.size(); ++counter )
		{
			target->SetTrackingTargets( mPlayer->trackingTargets[counter], counter );
		}
	}
}

FlagColors CChar::FlagColour( CChar *toCompare )
{
	FlagColors retVal = FC_INNOCENT;

	GUILDRELATION gComp	= GR_UNKNOWN;
	SI08 rComp			= 0;

	if( ValidateObject( toCompare ) )
	{
		if( !IsIncognito() )
		{
			gComp	= GuildSys->Compare( this, toCompare );
			rComp	= Races->Compare( this, toCompare );
		}
	}

	if( IsInvulnerable() )
		retVal = FC_INVULNERABLE;
	else if( IsMurderer() )
		retVal = FC_MURDERER;
	else if( IsCriminal() )
		retVal = FC_CRIMINAL;
	else if( IsNeutral() )
		retVal = FC_NEUTRAL;
	else if( rComp != 0 || gComp != GR_UNKNOWN )
	{
		if( gComp == GR_ALLY || gComp == GR_SAME || rComp > 0 )
			retVal = FC_FRIEND;
		else if( gComp == GR_WAR || race < 0 )
			retVal = FC_ENEMY;
	}
	return retVal;
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::RemoveFromSight
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Loops through all online chars and removes the character
//|                  from their sight
//o---------------------------------------------------------------------------o
void CChar::RemoveFromSight( CSocket *mSock )
{
	CPRemoveItem toSend = (*this);
	if( mSock != NULL )
		mSock->Send( &toSend );
	else
	{
		SOCKLIST nearbyChars = FindPlayersInOldVisrange( this );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			if( !(*cIter)->LoginComplete() )
				continue;
			if( (*cIter)->CurrcharObj() != this )
				(*cIter)->Send( &toSend );
		}
	}
}
//o---------------------------------------------------------------------------o
//|	Function	-	void SendToSocket( CSocket *s, bool sendDispZ )
//|	Date		-	April 7th, 2000
//|	Programmer	-	Abaddon
//|	Modified	-	Maarc (June 16, 2003)
//|						Got rid of array based packet sending, replaced with
//|						CPDrawObject, simplifying logic
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends the information about this person to socket S
//|                  IF in range.  Essentially a replacement for impowncreate
//o---------------------------------------------------------------------------o

void CChar::SendToSocket( CSocket *s )
{
	if( s != NULL && s->LoginComplete() )
	{
		CChar *mCharObj = s->CurrcharObj();
		if( mCharObj == this )
		{
			CPDrawGamePlayer gpToSend( (*this) );
			s->Send( &gpToSend );

			SendWornItems( s );
		}
		else if( ( GetVisible() != VT_VISIBLE || !isOnline( (*this) ) ) && GetCommandLevel() > mCharObj->GetCommandLevel() )
			return;

		CPDrawObject toSend( (*this) );

		toSend.SetRepFlag( static_cast<UI08>(FlagColour( mCharObj )) );

		for( LAYERLIST_ITERATOR lIter = itemLayers.begin(); lIter != itemLayers.end(); ++lIter )
		{
			if( ValidateObject( lIter->second ) )
				toSend.AddItem( lIter->second );
		}

		toSend.Finalize();
		s->Send( &toSend );

		CPToolTip pSend( GetSerial() );
		s->Send( &pSend );
	}
}

void checkRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight );
void CChar::Teleport( void )
{
	CSocket *mSock = GetSocket();
	RemoveFromSight();
	Update();
	if( mSock != NULL )
	{
		UI16 visrange = mSock->Range() + Races->VisRange( GetRace() );
		mSock->statwindow( this );
		mSock->WalkSequence( -1 );

		REGIONLIST nearbyRegions = MapRegion->PopulateList( this );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( ValidateObject( tempChar ) )
				{
					if( this != tempChar && objInRange( this, tempChar, visrange ) && 
						( isOnline( (*tempChar) ) || tempChar->IsNpc() || 
						( IsGM() && cwmWorldState->ServerData()->ShowOfflinePCs() ) ) )
						tempChar->SendToSocket( mSock );
				}
			}
			regChars->Pop();
			CDataList< CItem * > *regItems = MapArea->GetItemList();
			regItems->Push();
			for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
			{
				if( ValidateObject( tempItem ) )
				{
					if( tempItem->GetID( 1 ) >= 0x40 && objInRange( this, tempItem, DIST_BUILDRANGE ) )
						tempItem->SendToSocket( mSock );
					else if( objInRange( this, tempItem, visrange ) )
						tempItem->SendToSocket( mSock );
				}
			}
			regItems->Pop();
		}
	}
	CheckCharInsideBuilding( this, mSock, false );
	
	bool forceWeatherupdate = true;
	
	if( ValidateObject( GetMultiObj() ) )
		if( GetMultiObj()->GetType() == IT_PLANK ) //Don't force a weather update while on boat to prevent spam.
			forceWeatherupdate = false;

	checkRegion( mSock, (*this), forceWeatherupdate );
}

void CChar::ExposeToView( void )
{
	SetVisible( VT_VISIBLE );
	SetStealth( -1 );

	// hide it from ourselves, we want to show ourselves to everyone in range
	// now we tell everyone we exist
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Update( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends update to all those in range
//o---------------------------------------------------------------------------o
void CChar::Update( CSocket *mSock )
{
	if( mSock != NULL )
		SendToSocket( mSock );
	else
	{
		SOCKLIST nearbyChars = FindNearbyPlayers( this );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			if( !(*cIter)->LoginComplete() )
				continue;
			SendToSocket( (*cIter) );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *GetItemAtLayer( ItemLayers Layer ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item at layer Layer on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::GetItemAtLayer( ItemLayers Layer )
{
	CItem *rVal = NULL;
	LAYERLIST_ITERATOR lIter = itemLayers.find( Layer );
	if( lIter != itemLayers.end() )
		rVal = lIter->second;
	return rVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool WearItem( CItem *toWear ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Wears the item toWear and adjusts the stats if any are
//|					 required to change.  Returns true if successfully equipped
//o---------------------------------------------------------------------------o
bool CChar::WearItem( CItem *toWear )
{
	bool rvalue = true;
	ItemLayers tLayer = toWear->GetLayer();
	if( tLayer != IL_NONE )	// Layer == 0 is a special case, for things like trade windows and such
	{
		if( ValidateObject( GetItemAtLayer( tLayer ) ) )
		{
#if defined( UOX_DEBUG_MODE )
			Console.Warning( 2, "Failed to equip item %s(0x%X) to layer 0x%X on character %s(0x%X)", toWear->GetName().c_str(), toWear->GetSerial(), tLayer, GetName().c_str(), serial );
#endif
			rvalue = false;
		}
		else
		{
			itemLayers[tLayer] = toWear;

			IncStrength2( itemLayers[tLayer]->GetStrength2() );
			IncDexterity2( itemLayers[tLayer]->GetDexterity2() );
			IncIntelligence2( itemLayers[tLayer]->GetIntelligence2() );
			
			if( toWear->isPostLoaded() ) {
				if( itemLayers[tLayer]->GetPoisoned() )
					SetPoisoned( GetPoisoned() + itemLayers[tLayer]->GetPoisoned() );	// should be +, not -

				UI16 scpNum			= toWear->GetScriptTrigger();
				cScript *tScript	= JSMapping->GetScript( scpNum );
				if( tScript != NULL )
					tScript->OnEquip( this, toWear );
			}
		}
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool TakeOffItem( ItemLayers Layer )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Takes the item off the character's paperdoll,
//|					 ensuring that any stat adjustments are made
//|					 Returns true if successfully unequipped
//o---------------------------------------------------------------------------o
bool CChar::TakeOffItem( ItemLayers Layer )
{
	bool rvalue = false;
	if( ValidateObject( GetItemAtLayer( Layer ) ) )
	{
		if( Layer == IL_PACKITEM )	// It's our pack!
			SetPackItem( NULL );
		IncStrength2( -itemLayers[Layer]->GetStrength2() );
		IncDexterity2( -itemLayers[Layer]->GetDexterity2() );
		IncIntelligence2( -itemLayers[Layer]->GetIntelligence2() );
		if( itemLayers[Layer]->GetPoisoned() )
		{
			if( itemLayers[Layer]->GetPoisoned() > GetPoisoned() )
				SetPoisoned( 0 );
			else
				SetPoisoned( GetPoisoned() - itemLayers[Layer]->GetPoisoned() );
		}

		cScript *tScript = NULL;
		UI16 scpNum = itemLayers[Layer]->GetScriptTrigger();
		tScript = JSMapping->GetScript( scpNum );
		if( tScript != NULL )
			tScript->OnUnequip( this, itemLayers[Layer] );

		itemLayers[Layer] = NULL;
		rvalue = true;
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *FirstItem( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item reference for the first item on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::FirstItem( void )
{
	CItem *rVal = NULL;

	layerCtr = itemLayers.begin();
	if( !FinishedItems() )
		rVal = layerCtr->second;
	return rVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *NextItem( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item reference for the next item on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::NextItem( void )
{
	CItem *rVal = NULL;
	++layerCtr;
	if( !FinishedItems() )
		rVal = layerCtr->second;
	return rVal;
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool FinishedItems( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if there are no more items on the paperdoll
//o---------------------------------------------------------------------------o
bool CChar::FinishedItems( void )
{
	return ( layerCtr == itemLayers.end() );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsValidMount( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is a valid mount
//o---------------------------------------------------------------------------o
bool CChar::IsValidMount( void ) const
{
	bool retVal = false;
	switch( id )
	{
		// Basic Ridables
		case 0xC8:		// horse (Light brown)
		case 0xCC:		// horse (Dark Brown)
		case 0xE2:		// horse (Light Gray)
		case 0xE4:		// horse (Gray Brown)
			// T2A Ridables
		case 0xD2:		// Desert Ostard 
		case 0xDA:		// Frenzied Ostard 
		case 0xDB:		// Forest Ostard 
		case 0xDC:		// llama 
			// LBR Ridables
		case 0x7A:		// Unicorn
		case 0x72:		// Dark Steed
		case 0x73:		// Ethereal Horse
		case 0x74:		// Nightmare - normal
		case 0x75:		// Silver Steed
		case 0x76:		// Britannian War Horse
		case 0x77:		// Council of Mages War Horse
		case 0x78:		// Minax War Horse
		case 0x79:		// Shadowlord War Horse
		case 0x84:		// Kirin
		case 0x90:		// Sea horse
		case 0xAA:		// Ethereal Llama
		case 0xAB:		// Ethereal Ostard
		case 0xB1:		// Nightmare
		case 0xB2:		// Another Nightmare
		case 0xB3:		// A third Nightmare
		case 0xBB:		// Ridgeback
		case 0xBC:		// Savage Ridgeback
		case 0xBE:		// Fire Steed
		case 0xBF:		// Etheral Kirin
		case 0xC0:		// Ethereal Unicorn
		case 0xC1:		// Ethereal Ridgeback
		case 0xC2:		// Ethereal Swampdragon
		case 0xC3:		// Ethereal Beetle
		case 0x317:		// Giant Beetle
		case 0x319:		// Skeletal Mount
		case 0x31A:		// Swamp Dragon
		case 0x31F:		// Armored Swamp Dragon
		case 0x3E6:		// Kirin
			//Samurai Empire Ridables
		case 0xA9:		// Giant Fire Beetle
		case 0xF3:		// Hai Riyo
			//Mondain's Legacy Ridables
		case 0x114:		// Chimera
		case 0x115:		// Cu Sidhe
		case 0x11C:		// Mondain's Steed
			retVal = true;
			break;
		default:
			break;
	}

	return retVal;
}

bool CChar::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[CHARACTER]" << std::endl;
	return true;
}

bool CChar::DumpBody( std::ofstream &outStream ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 

	CBaseObject::DumpBody( outStream );	// Make the default save of BaseObject members now
	dumping << "GuildTitle=" << GetGuildTitle() << std::endl;  
	dumping << "Hunger=" << (SI16)GetHunger() << std::endl;
	dumping << "TempWeight=" << GetTempWeight() << std::endl;
	dumping << "FireResist=" << (SI16)GetDamageResist( DAMAGE_FIRE ) << std::endl;
	dumping << "ColdResist=" << (SI16)GetDamageResist( DAMAGE_COLD ) << std::endl;
	dumping << "EnergyResist=" << (SI16)GetDamageResist( DAMAGE_ENERGY ) << std::endl;
	dumping << "PoisonResist=" << (SI16)GetDamageResist( DAMAGE_POISON ) << std::endl;
	dumping << "TamedHungerRate=" << (SI16)GetTamedHungerRate() << std::endl;
	dumping << "TamedHungerWildChance=" << (SI16)GetTamedHungerWildChance() << std::endl;
	dumping << "BrkPeaceChanceGain=" << (SI16)GetBrkPeaceChanceGain() << std::endl;
	dumping << "BrkPeaceChance=" << (SI16)GetBrkPeaceChance() << std::endl;
	dumping << "Foodlist=" << foodList << std::endl;
	if ( GetMaxHPFixed() )
		dumping << "MAXHP=" << (SI16)maxHP << std::endl;
	if ( GetMaxManaFixed() )
		dumping << "MAXMANA=" << (SI16)maxMana << std::endl;
	if ( GetMaxStamFixed() )
		dumping << "MAXSTAM=" << (SI16)maxStam << std::endl;
	dumping << "Town=" << (SI16)GetTown() << std::endl;
	dumping << "TownVote=" << std::hex << "0x" << GetTownVote() << std::endl;
	dumping << "GuildFealty=" << "0x" << GetGuildFealty() << std::endl;  
	dumping << "Speech=" << "0x" << GetSayColour() << ",0x" << GetEmoteColour() << std::endl;
	dumping << "SummonTimer=" << std::dec << GetTimer( tNPC_SUMMONTIME ) << std::endl;
	dumping << "MayLevitate=" << (MayLevitate()?1:0) << std::endl;
	dumping << "Stealth=" << (SI16)GetStealth() << std::endl;
	dumping << "Reserved=" << (SI16)GetCell() << std::endl;
	dumping << "Region=" << (SI16)GetRegionNum() << std::endl;
	if( ValidateObject( packitem ) )
		dumping << "PackItem=" << std::hex << "0x" << packitem->GetSerial() << std::endl;	// store something meaningful
	else
		dumping << "PackItem=" << std::hex << "0x" << INVALIDSERIAL << std::endl;
	dumping << "AdvanceObject=" << std::dec << GetAdvObj() << std::endl;
	dumping << "AdvRaceObject=" << GetRaceGate() << std::endl;
	dumping << "Privileges=" << std::hex << "0x" << GetPriv() << std::endl;
	dumping << "TownPrivileges=" << std::dec << (SI16)GetTownPriv() << std::endl;

	// Write out the BaseSkills and the SkillLocks here
	// Format: BaseSkills=[0,34]-[1,255]-[END]
	dumping << "BaseSkills=";
	for( UI08 bsc = 0; bsc < ALLSKILLS; ++bsc )
		dumping << "[" << (SI32)bsc << "," << GetBaseSkill( bsc ) << "]-";
	dumping << "[END]" << std::endl;

	if( !IsNpc() )
	{
		dumping << "Atrophy=";
		if( GetAtrophy( 0 ) >= 10 )
			dumping << GetAtrophy( 0 );
		else
			dumping << "0" << GetAtrophy( 0 );
	
		for( UI08 atc = 1; atc < ALLSKILLS; ++atc )
		{
			if( GetAtrophy( atc ) >= 10 )
				dumping << "," << GetAtrophy( atc );
			else
				dumping << ",0" << GetAtrophy( atc );
		}
		dumping << "[END]" << std::endl;

		// Format: SkillLocks=[0,34]-[1,255]-[END]
		dumping << "SkillLocks=";
		for( UI08 slc = 0; slc < ALLSKILLS; ++slc )
		{
			if( GetSkillLock( slc ) <= 2 )
				dumping << "[" << (SI16)slc << "," << (SI16)GetSkillLock( slc ) << "]-";
			else
				dumping << "[" << (SI16)slc << ",0]-";
		}
		dumping << "[END]" << std::endl;
	}
	dumping << "GuildNumber=" << GetGuildNumber() << std::endl;  
	dumping << "FontType=" << (SI16)GetFontType() << std::endl;
	dumping << "TownTitle=" << (GetTownTitle()?1:0) << std::endl;
	//-------------------------------------------------------------------------------------------
	dumping << "CanRun=" << (SI16)((CanRun() && IsNpc())?1:0) << std::endl;
	dumping << "CanAttack=" << (SI16)(GetCanAttack()?1:0) << std::endl;
	dumping << "AllMove=" << (SI16)(AllMove()?1:0) << std::endl;
	dumping << "IsNpc=" << (SI16)(IsNpc()?1:0) << std::endl;
	dumping << "IsShop=" << (SI16)(IsShop()?1:0) << std::endl;
	dumping << "Dead=" << (SI16)(IsDead()?1:0) << std::endl;
	dumping << "CanTrain=" << (SI16)(CanTrain()?1:0) << std::endl;
	dumping << "IsWarring=" << (SI16)(IsAtWar()?1:0) << std::endl;
	dumping << "GuildToggle=" << (SI16)(GetGuildToggle()?1:0) << std::endl;
	dumping << "PoisonStrength=" << (UI16)(GetPoisonStrength()) << std::endl;
	dumping << "WillHunger=" << (SI16)(WillHunger()?1:0) << std::endl;

	TIMERVAL mTime = GetTimer( tCHAR_MURDERRATE );
	dumping << "MurderTimer=";
	if( mTime == 0 || mTime < cwmWorldState->GetUICurrentTime() )
		dumping << (SI16)0 << std::endl;
	else
		dumping << (UI32)(mTime - cwmWorldState->GetUICurrentTime()) << std::endl;

	TIMERVAL pTime = GetTimer( tCHAR_PEACETIMER );
	dumping << "PeaceTimer=";
	if( pTime == 0 || pTime < cwmWorldState->GetUICurrentTime() )
		dumping << (SI16)0 << std::endl;
	else
		dumping << (UI32)(pTime - cwmWorldState->GetUICurrentTime()) << std::endl;

	outStream << dumping.str();

	if( IsValidPlayer() )
		mPlayer->DumpBody( outStream );
	if( IsValidNPC() )
		mNPC->DumpBody( outStream );
	return true;
}

void CChar::NPCValues_st::DumpBody( std::ofstream& outStream )
{
	std::string destination; 
	std::ostringstream dumping( destination ); 

	dumping << "NpcAIType=" << aiType << std::endl;
	dumping << "Taming=" << taming << std::endl;
	dumping << "Peaceing=" << peaceing << std::endl;
	dumping << "Provoing=" << provoing << std::endl;
	dumping << "HoldG=" << goldOnHand << std::endl;
	dumping << "Split=" << (SI16)splitNum << "," << (SI16)splitChance << std::endl;
	dumping << "WanderArea=" << fx[0] << "," << fy[0] << "," << fx[1] << "," << fy[1] << "," << (SI16)fz << std::endl;
	dumping << "NpcWander=" << (SI16)wanderMode << "," << (SI16)oldWanderMode << std::endl;
	dumping << "SPAttack=" << spellAttack << "," << (SI16)spellDelay << std::endl;
	dumping << "QuestType=" << (SI16)questType << std::endl;
	dumping << "QuestRegions=" << (SI16)questOrigRegion << "," << (SI16)questDestRegion << std::endl;
	dumping << "FleeAt=" << fleeAt << std::endl;
	dumping << "ReAttackAt=" << reAttackAt << std::endl;
	dumping << "NPCFlag=" << (SI16)npcFlag << std::endl;
	dumping << "Mounted=" << (SI16)(MFLAGGET( boolFlags, BIT_MOUNTED )?1:0) << std::endl;
	dumping << "Stabled=" << (SI16)(MFLAGGET( boolFlags, BIT_STABLED )?1:0) << std::endl;

	outStream << dumping.str();
}

void CChar::PlayerValues_st::DumpBody( std::ofstream& outStream )
{
	std::string destination;
	std::ostringstream dumping( destination );

	dumping << "Account=" << accountNum << std::endl;
	dumping << "LastOn=" << lastOn << std::endl;
	dumping << "LastOnSecs=" << lastOnSecs << std::endl;
	dumping << "OrgName=" << origName << std::endl;
	dumping << "RobeSerial=" << std::hex << "0x" << robe << std::endl;
	dumping << "OriginalID=" << "0x" << origID << ",0x" << origSkin << std::endl;
	dumping << "Hair=" << "0x" << hairStyle << ",0x" << hairColour << std::endl;
	dumping << "Beard=" << "0x" << beardStyle << ",0x" << beardColour << std::endl;
	dumping << "CommandLevel=" << std::dec << (SI16)commandLevel << std::endl;	// command level
	dumping << "Squelched=" << (SI16)squelched << std::endl;
	dumping << "Deaths=" << deaths << std::endl;
	dumping << "FixedLight=" << (SI16)fixedLight << std::endl;

	outStream << dumping.str();
}

//o-----------------------------------------------------------------------o
//|	Function	-	Save( ofstream &outstream )
//|	Date		-	July 21, 2000
//|	Programmer	-	EviLDeD
//o-----------------------------------------------------------------------o
//|	Returns		-	true/false indicating the success of the write operation
//o-----------------------------------------------------------------------o
bool CChar::Save( std::ofstream &outStream )
{
	bool rvalue = false;
	if( !isFree() )
	{
		SI16 mX = GetX();
		SI16 mY = GetY();
		MapData_st& mMap = Map->GetMapData( worldNumber );
		if( mX >= 0 && ( mX < mMap.xBlock || mX >= 7000 ) )
		{
			if( mY >= 0 && ( mY < mMap.yBlock || mY >= 7000 ) )
			{
				DumpHeader( outStream );
				DumpBody( outStream );
				DumpFooter( outStream );

				for( LAYERLIST_ITERATOR lIter = itemLayers.begin(); lIter != itemLayers.end(); ++lIter )
				{
					if( ValidateObject( lIter->second ) )
					{
						if( lIter->second->ShouldSave() )
							lIter->second->Save( outStream );
					}
				}
				rvalue = true;
			}
		}
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void BreakConcentration( CSocket *sock = NULL )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Breaks the concentration of the character
//|					 sending a message is a socket exists
//o---------------------------------------------------------------------------o
void CChar::BreakConcentration( CSocket *sock )
{
	if( IsMeditating() )
	{
		SetMeditating( false );
		if( sock != NULL )
			sock->sysmessage( 100 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  CHARLIST *GetPetList( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the list of pets the character owns
//o---------------------------------------------------------------------------o
CDataList< CChar * > *CChar::GetPetList( void )
{
	return &petsControlled;
}

//o---------------------------------------------------------------------------o
//|   Function    -  ITEMLIST *GetOwnedItems( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the list of items the char owns
//o---------------------------------------------------------------------------o
ITEMLIST *CChar::GetOwnedItems( void )
{
	return &ownedItems;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddOwnedItem( CItem *toAdd ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds the item toAdd to the player's owned list
//|					 ensuring that it is not ALREADY in the list
//o---------------------------------------------------------------------------o
void CChar::AddOwnedItem( CItem *toAdd )
{
	ITEMLIST_CITERATOR i;
	for( i = ownedItems.begin(); i != ownedItems.end(); ++i )
	{
		if( (*i) == toAdd )		// We already have it
			break;
	}
	if( i == ownedItems.end() )
		ownedItems.push_back( toAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemoveOwnedItem( CItem *toRemove )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes the item toRemove from the player's owned list
//o---------------------------------------------------------------------------o
void CChar::RemoveOwnedItem( CItem *toRemove )
{
	for( ITEMLIST_ITERATOR rIter = ownedItems.begin(); rIter != ownedItems.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			ownedItems.erase( rIter );
			break;
		}
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 MaxHP()
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Maximum hitpoints of the object
//o--------------------------------------------------------------------------
UI16 CChar::GetMaxHP( void )
{
	if( (maxHP_oldstr != GetStrength() || oldRace != GetRace()) && !GetMaxHPFixed() )
	//if str/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxHP = (UI16)(GetStrength() + (UI16)( ((float)GetStrength()) * ((float)(pRace->HPModifier())) / 100 ));
		// set max. hitpoints to strength + hpmodifier% of strength

		maxHP_oldstr	= GetStrength();
		oldRace			= GetRace();

	}
	return maxHP;
}
void CChar::SetMaxHP( UI16 newmaxhp, UI16 newoldstr, RACEID newoldrace )
{
	maxHP			= newmaxhp;
	maxHP_oldstr	= newoldstr;
	oldRace			= newoldrace;
}
void CChar::SetFixedMaxHP( SI16 newmaxhp )
{
	if( newmaxhp > 0 ) {
		SetMaxHPFixed( true );
		maxHP			= newmaxhp;
	} else {
		SetMaxHPFixed( false );
	
		CRace *pRace = Races->Race( GetRace() );
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxHP = (UI16)(GetStrength() + (UI16)( ((float)GetStrength()) * ((float)(pRace->HPModifier())) / 100 ));

		maxHP_oldstr	= GetStrength();
		oldRace			= GetRace();
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 MaxMana()
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Maximum mana of the object
//o--------------------------------------------------------------------------
SI16 CChar::GetMaxMana( void )
{
	if( (maxMana_oldint != GetIntelligence() || oldRace != GetRace()) && !GetMaxManaFixed() )
	//if int/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxMana = (SI16)(GetIntelligence() + (SI16)( ((float)GetIntelligence()) * ((float)(pRace->ManaModifier())) / 100 ));
		// set max. mana to int + manamodifier% of int

		maxMana_oldint	= GetIntelligence();
		oldRace			= GetRace();

	}
	return maxMana;
}
void CChar::SetMaxMana( SI16 newmaxmana, UI16 newoldint, RACEID newoldrace )
{
	maxMana			= newmaxmana;
	maxMana_oldint	= newoldint;
	oldRace			= newoldrace;
}
void CChar::SetFixedMaxMana( SI16 newmaxmana )
{
	if( newmaxmana > 0 ) {
		SetMaxManaFixed( true );
		maxMana			= newmaxmana;
	} else {
		SetMaxManaFixed( false );
	
		CRace *pRace = Races->Race( GetRace() );
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxMana = (SI16)(GetIntelligence() + (SI16)( ((float)GetIntelligence()) * ((float)(pRace->ManaModifier())) / 100 ));

		maxMana_oldint	= GetIntelligence();
		oldRace			= GetRace();
	}
}
//o--------------------------------------------------------------------------
//|	Function		-	SI16 MaxStam()
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Maximum stamina of the object
//o--------------------------------------------------------------------------
SI16 CChar::GetMaxStam( void )
{
	if( (maxStam_olddex != GetDexterity() || oldRace != GetRace()) && !GetMaxStamFixed() )
	//if dex/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxStam = (SI16)(GetDexterity() + (SI16)( ((float)GetDexterity()) * ((float)(pRace->StamModifier())) / 100 ));
		// set max. stamina to dex + stammodifier% of dex

		maxStam_olddex	= GetDexterity();
		oldRace			= GetRace();

	}
	return maxStam;
}
void CChar::SetMaxStam( SI16 newmaxstam, UI16 newolddex, RACEID newoldrace )
{
	maxStam			= newmaxstam;
	maxStam_olddex	= newolddex;
	oldRace			= newoldrace;
}
void CChar::SetFixedMaxStam( SI16 newmaxstam )
{
	if( newmaxstam > 0 ) {
		SetMaxStamFixed( true );
		maxStam			= newmaxstam;
	} else {
		SetMaxStamFixed( false );
	
		CRace *pRace = Races->Race( GetRace() );
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxStam = (SI16)(GetDexterity() + (SI16)( ((float)GetDexterity()) * ((float)(pRace->StamModifier())) / 100 ));

		maxStam_olddex	= GetDexterity();
		oldRace			= GetRace();
	}
}
//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ActualStrength( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the actual strength (minus mods) of the character
//o---------------------------------------------------------------------------o
SI16 CChar::ActualStrength( void ) const
{
	return CBaseObject::GetStrength();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetStrength( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the strength (incl mods) of the character
//o---------------------------------------------------------------------------o
SI16 CChar::GetStrength( void ) const
{
	return (SI16)(CBaseObject::GetStrength() + GetStrength2());
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ActualIntelligence( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the actual intelligence (minus mods) of the char
//o---------------------------------------------------------------------------o
SI16 CChar::ActualIntelligence( void ) const
{
	return CBaseObject::GetIntelligence();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetIntelligence( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intelligence (incl mods) of the character
//o---------------------------------------------------------------------------o
SI16 CChar::GetIntelligence( void ) const
{
	return (SI16)(CBaseObject::GetIntelligence() + GetIntelligence2());
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ActualDextierty( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the actual (minus mods) dexterity of the character
//o---------------------------------------------------------------------------o
SI16 CChar::ActualDexterity( void ) const
{
	return CBaseObject::GetDexterity();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetDexterity( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the effective dexterity (including modifications)
//|					 of the player
//o---------------------------------------------------------------------------o
SI16 CChar::GetDexterity( void ) const
{
	return (SI16)(CBaseObject::GetDexterity() + GetDexterity2());
}

//o---------------------------------------------------------------------------o
//|   Function    -  void IncStrength2( SI08 toAdd = 1 )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Increments strength2 (modifications) by toAdd
//o---------------------------------------------------------------------------o
void CChar::IncStrength2( SI16 toAdd )
{
	SetStrength2( (SI16)(GetStrength2() + toAdd) );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void IncDexterity2( SI08 toAdd = 1 )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Increments dexterity2 (modifications) by toAdd
//o---------------------------------------------------------------------------o
void CChar::IncDexterity2( SI16 toAdd )
{
	SetDexterity2( (SI16)(GetDexterity2() + toAdd) );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void IncSI32elligence2( SI08 toAdd = 1 ) 
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Increments intelligence2 (modifications) by toAdd
//o---------------------------------------------------------------------------o
void CChar::IncIntelligence2( SI16 toAdd )
{
	SetIntelligence2( (SI16)(GetIntelligence2() + toAdd) );
}


//o---------------------------------------------------------------------------o
//|   Function    -  bool IsMurderer( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is a murderer
//o---------------------------------------------------------------------------o
bool CChar::IsMurderer( void ) const
{
	return ( GetFlag() == 0x01 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsCriminal( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is a criminal
//o---------------------------------------------------------------------------o
bool CChar::IsCriminal( void ) const
{
	return ( GetFlag() == 0x02 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsInnocent( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is innocent
//o---------------------------------------------------------------------------o
bool CChar::IsInnocent( void ) const
{
	return ( GetFlag() == 0x04 );
}

//o---------------------------------------------------------------------------o
//|   Function    -	 bool IsNeutral( void ) const
//|   Date        -  18 July 2005
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is neutral
//o---------------------------------------------------------------------------o
bool CChar::IsNeutral( void ) const
{
	return ( GetFlag() == 0x08 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagRed( void )
//|   Date        -  2nd October, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect murderer status
//o---------------------------------------------------------------------------o
void CChar::SetFlagRed( void )
{
	flag = 0x01;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagGray( void )
//|   Date        -  2nd October, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect criminality
//o---------------------------------------------------------------------------o
void CChar::SetFlagGray( void )
{
	flag = 0x02;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagBlue( void )
//|   Date        -  2nd October, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect innocence
//o---------------------------------------------------------------------------o
void CChar::SetFlagBlue( void )
{
	flag = 0x04;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagNeutral( void )
//|   Date        -  18th July, 2005
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect neutrality
//o---------------------------------------------------------------------------o
void CChar::SetFlagNeutral( void )
{
	flag = 0x08;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void DecHunger( const SI08 amt = 1 )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Decrements the character's hunger
//o---------------------------------------------------------------------------o
void CChar::DecHunger( const SI08 amt )
{
	SetHunger( (SI08)(GetHunger() - amt) );
}

void CChar::StopSpell( void )
{
	SetTimer( tCHAR_SPELLTIME, 0 );
	SetCasting( false );
	SetSpellCast( -1 );
}

bool CChar::HandleLine( UString &UTag, UString& data )
{
	bool rvalue = CBaseObject::HandleLine( UTag, data );
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "ACCOUNT" )
				{
					SetAccountNum( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "ATROPHY" )
				{
					for( UI08 aCtr = 0; aCtr < ALLSKILLS; ++aCtr )
					{
						SetAtrophy( data.section( ",", aCtr, aCtr ).stripWhiteSpace().toUShort(), aCtr );
					}
					rvalue = true;
				}
				else if( UTag == "ADVANCEOBJECT" )
				{
					SetAdvObj( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "ADVRACEOBJECT" )
				{
					SetRaceGate( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "ALLMOVE" )
				{
					SetAllMove( data.toUByte() == 1 );
					rvalue = true;
				}
				break;
			case 'B':
				if( UTag == "BEARDSTYLE" )
				{
					SetBeardStyle( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "BEARDCOLOUR" )
				{
					SetBeardColour( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "BASESKILLS" )
				{
					// Format: BaseSkills=[0,34]-[1,255]-[END]
					for( UI08 skillCtr = 0; skillCtr < ALLSKILLS; ++skillCtr )
					{
						UString tempdata	= data.section( "-", skillCtr, skillCtr ).stripWhiteSpace();
						if( tempdata.empty() )
							break;

						UString tempval		= tempdata.section( ",", 1, 1 ).substr( 0, tempdata.section( ",", 1, 1 ).size() - 1 );
						UString tempnum		= tempdata.section( ",", 0, 0 ).substr( 1 );
						SetBaseSkill( tempval.toUShort(), tempnum.toUByte() );
					}
					rvalue = true;
				}
				else if( UTag == "BEARD" )
				{
					SetBeardStyle( data.section( ",", 0, 0 ).stripWhiteSpace().toUShort() );
					SetBeardColour( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					rvalue = true;
				}
				else if( UTag == "BRKPEACECHANCEGAIN" )
				{
					SetBrkPeaceChanceGain( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "BRKPEACECHANCE" )
				{
					SetBrkPeaceChance( data.toUShort() );
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "COMMANDLEVEL" )
				{
					SetCommandLevel( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "CANRUN" )
				{
					SetRun( data.toUByte() == 1 );
					rvalue = true;
				}
				else if( UTag == "CANATTACK" )
				{
					SetCanAttack( data.toUByte() == 1 );
					rvalue = true;
				}
				else if( UTag == "CANTRAIN" )
				{
					SetCanTrain( data.toUShort() == 1 );
					rvalue = true;
				}
				else if( UTag == "COLDRESIST" )
				{
					SetDamageResist( data.toUShort(), DAMAGE_COLD );
					rvalue = true;
				}
				break;
			case 'D':
				if( UTag == "DISPLAYZ" )
					rvalue = true;
				else if( UTag == "DEATHS" )
				{
					SetDeaths( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "DEAD" )
				{
					SetDead( (data.toUByte() == 1) );
					rvalue = true;
				}
				break;
			case 'E':
				if( UTag == "EMOTION" )
				{
					SetEmoteColour( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "ENERGYRESIST" )
				{
					SetDamageResist( data.toUShort(), DAMAGE_ENERGY );
					rvalue = true;
				}
				break;
			case 'F':
				if( UTag == "FIXEDLIGHT" )
				{
					SetFixedLight( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "FX1" )
				{
					SetFx( data.toShort(), 0 );
					rvalue = true;
				}
				else if( UTag == "FX2" )
				{
					SetFx( data.toShort(), 1 );
					rvalue = true;
				}
				else if( UTag == "FY1" )
				{
					SetFy( data.toShort(), 0 );
					rvalue = true;
				}
				else if( UTag == "FY2" )
				{
					SetFy( data.toShort(), 1 );
					rvalue = true;
				}
				else if( UTag == "FZ1" )
				{
					SetFz( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "FLEEAT" )
				{
					SetFleeAt( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "FONTTYPE" )
				{
					SetFontType( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "FOODLIST" )
				{
					SetFood( data.substr( 0, MAX_NAME ) );
					rvalue = true;
				}
				else if( UTag == "FIRERESIST" )
				{
					SetDamageResist( data.toUShort(), DAMAGE_FIRE );
					rvalue = true;
				}
				break;
			case 'G':
				if( UTag == "GUILDFEALTY" )
				{
					SetGuildFealty( data.toLong() );
					rvalue = true;
				}
				else if( UTag == "GUILDTITLE" )
				{
					SetGuildTitle( data );
					rvalue = true;
				}
				else if( UTag == "GUILDNUMBER" )
				{
					SetGuildNumber( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "GUILDTOGGLE" )
				{
					SetGuildToggle( data.toShort() == 1 );
					rvalue = true;
				}
				break;
			case 'H':
				if( UTag == "HUNGER" )
				{
					SetHunger( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "HOLDG" )
				{
					SetHoldG( data.toLong() );
					rvalue = true;
				}
				else if( UTag == "HAIRSTYLE" )
				{
					SetHairStyle( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "HAIRCOLOUR" )
				{
					SetHairColour( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "HAIR" )
				{
					SetHairStyle( data.section( ",", 0, 0 ).stripWhiteSpace().toUShort() );
					SetHairColour( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					rvalue = true;
				}
				break;
			case 'I':
				if( UTag == "ISNPC" )
				{
					SetNpc( (data.toShort() == 1) );
					rvalue = true;
				}
				else if( UTag == "ISSHOP" )
				{
					SetShop( (data.toShort() == 1) );
					rvalue = true;
				}
				else if( UTag == "ISWARRING" )
				{
					SetWar( (data.toShort() == 1) );
					rvalue = true;
				}
				break;
			case 'L':
				if( UTag == "LASTON" )
				{
					SetLastOn( data );
					rvalue = true;
				}
				else if( UTag == "LASTONSECS" )
				{
					SetLastOnSecs( data.toLong() );
					rvalue = true;
				}
				break;
			case 'M':
				if( UTag == "MAYLEVITATE" )
				{
					SetLevitate( (data.toShort() == 1) );
					rvalue = true;
				}
				else if( UTag == "MAKING" ) // Depreciated
					rvalue = true;
				else if( UTag == "MURDERTIMER" )
				{
					SetTimer( tCHAR_MURDERRATE, BuildTimeValue( data.toFloat() ) );
					rvalue = true;
				}
				else if( UTag == "MAXHP" )
				{
					SetFixedMaxHP( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "MAXMANA" )
				{
					SetFixedMaxMana( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "MAXSTAM" )
				{
					SetFixedMaxStam( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "MOUNTED" )
				{
					SetMounted( data.toUShort() == 1 );
					rvalue = true;
				}
				break;
			case 'N':
				if( UTag == "NPCAITYPE" )
				{
					SetNPCAiType( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "NPCWANDER" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						SetNpcWander( data.section( ",", 0, 0 ).stripWhiteSpace().toByte() );
						SetOldNpcWander( data.section( ",", 1, 1 ).stripWhiteSpace().toByte() );
					}
					else
						SetNpcWander( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "NPCFLAG" )
				{
					SetNPCFlag( (cNPC_FLAG)data.toUByte() );
					rvalue = true;
				}
				break;
			case 'O':
				if( UTag == "ORGNAME" )
				{
					SetOrgName( data );
					rvalue = true;
				}
				else if( UTag == "ORIGINALBODYID" )
				{
					SetOrgID( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "ORIGINALSKINID" )
				{
					SetOrgSkin( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "ORIGINALID" )
				{
					SetOrgID( data.section( ",", 0, 0 ).stripWhiteSpace().toUShort() );
					SetOrgSkin( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PRIVILEGES" )
				{
					SetPriv( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "POSTTYPE" )
					rvalue = true;
				else if( UTag == "PACKITEM" )
				{
					packitem = (CItem *)data.toULong();
					rvalue = true;
				}
				else if( UTag == "POISON" )
				{
					SetPoisoned( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "POISONSERIAL" ) // Depreciated
					rvalue = true;
				else if( UTag == "POISONSTRENGTH" )
				{
					SetPoisonStrength( data.toUByte() );
					rvalue = true;
				}
				if( UTag == "PEACEING" )
				{
					SetPeaceing( data.toShort() );
					rvalue = true;
				}
				if( UTag == "PROVOING" )
				{
					SetProvoing( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "PEACETIMER" )
				{
					SetTimer( tCHAR_PEACETIMER, BuildTimeValue( data.toFloat() ) );
					rvalue = true;
				}
				else if( UTag == "POISONRESIST" )
				{
					SetDamageResist( data.toUShort(), DAMAGE_POISON );
					rvalue = true;
				}
				break;
			case 'Q':
				if( UTag == "QUESTTYPE" )
				{
					SetQuestType( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "QUESTDESTINATIONREGION" )
				{
					SetQuestDestRegion( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "QUESTORIGINALREGION" )
				{
					SetQuestOrigRegion( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "QUESTREGIONS" )
				{
					SetQuestOrigRegion( data.section( ",", 0, 0 ).stripWhiteSpace().toUByte() );
					SetQuestDestRegion( data.section( ",", 1, 1 ).stripWhiteSpace().toUByte() );
					rvalue = true;
				}
				break;
			case 'R':
				if( UTag == "ROBESERIAL" )
				{
					SetRobe( data.toULong() );
					rvalue = true;
				}
				else if( UTag == "RESERVED" )
				{
					SetCell( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "RUNNING" )
				{
					rvalue = true;
				}
				else if( UTag == "REGION" )
				{
					SetRegion( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "REATTACKAT" )
				{
					SetReattackAt( data.toShort() );
					rvalue = true;
				}
				break;
			case 'S':
				if( UTag == "SPLIT" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						SetSplit( data.section( ",", 0, 0 ).stripWhiteSpace().toUByte() );
						SetSplitChance( data.section( ",", 1, 1 ).stripWhiteSpace().toUByte() );
					}
					else
						SetSplit( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "SPLITCHANCE" )
				{
					SetSplitChance( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "SUMMONTIMER" )
				{
					SetTimer( tNPC_SUMMONTIME, data.toULong() );
					rvalue = true;
				}
				else if( UTag == "SAY" )
				{
					SetSayColour( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "STEALTH" )
				{
					SetStealth( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "STEP" )
				{
					rvalue = true;
				}
				else if( UTag == "SPATTACK" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						SetSpAttack( data.section( ",", 0, 0 ).stripWhiteSpace().toShort() );
						SetSpDelay( data.section( ",", 1, 1 ).stripWhiteSpace().toByte() );
					}
					else
						SetSpAttack( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "SPECIALATTACKDELAY" )
				{
					SetSpDelay( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "SQUELCHED" )
				{
					SetSquelched( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "SKILLLOCKS" )
				{
					// Format: Baselocks=[0,34]-[1,255]-[END]
					for( UI08 lockCtr = 0; lockCtr < ALLSKILLS; ++lockCtr )
					{
						UString tempdata = data.section( "-", lockCtr, lockCtr ).stripWhiteSpace();
						if( tempdata.empty() )
							break;

						UString tempval = tempdata.section( ",", 1, 1 ).substr( 0, tempdata.section( ",", 1, 1 ).size() - 1 );
						UString tempnum = tempdata.section( ",", 0, 0 ).substr( 1 );
						SetSkillLock( tempval.toUByte(), tempnum.toUByte() );
					}
					rvalue = true;
				}
				else if( UTag == "SPEECH" )
				{
					SetSayColour( data.section( ",", 0, 0 ).stripWhiteSpace().toUShort() );
					SetEmoteColour( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					rvalue = true;
				}
				else if( UTag == "STABLED" )
				{
					SetStabled( data.toUShort() == 1 );
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TAMING" )
				{
					SetTaming( data.toShort() );
					rvalue = true;
				}
				if( UTag == "TAMEDHUNGERRATE" )
				{
					SetTamedHungerRate( data.toByte() );
					rvalue = true;
				}
				if( UTag == "TAMEDHUNGERWILDCHANCE" )
				{
					SetTamedHungerWildChance( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "TOWN" )
				{
					SetTown( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "TOWNVOTE" )
				{
					SetTownVote( data.toULong() );
					rvalue = true;
				}
				else if( UTag == "TOWNPRIVILEGES" )
				{
					SetTownpriv( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "TOWNTITLE" )
				{
					SetTownTitle( data.toShort() == 1 );
					rvalue = true;
				}
				else if( UTag == "TEMPWEIGHT" )
				{
					SetTempWeight( data.toLong() );
					rvalue = true;
				}
				break;
			case 'W':
				if( UTag == "WANDERAREA" )
				{
					SetFx( data.section( ",", 0, 0 ).stripWhiteSpace().toShort(), 0 );
					SetFy( data.section( ",", 1, 1 ).stripWhiteSpace().toShort(), 0 );
					SetFx( data.section( ",", 2, 2 ).stripWhiteSpace().toShort(), 1 );
					SetFy( data.section( ",", 3, 3 ).stripWhiteSpace().toShort(), 1 );
					SetFz( data.section( ",", 4, 4 ).stripWhiteSpace().toByte() );
					rvalue = true;
				}
				else if( UTag == "WILLHUNGER" )
				{
					SetHungerStatus( data.toShort() == 1 );
					rvalue = true;
				}
				break;
			case 'X':
				if( UTag == "XNPCWANDER" )
				{
					SetOldNpcWander( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "XBODYID" ) // Depreciated
					rvalue = true;
				else if( UTag == "XSKINID" ) // Depreciated
					rvalue = true;
				else if( UTag == "XMANA" )	// Depreciated
					rvalue = true;
				else if( UTag == "XSTAMINA" )	// Depreciated
					rvalue = true;
				else if( UTag == "XKARMA" )	// Depreciated
					rvalue = true;
				else if( UTag == "XFAME" )	// Depreciated
					rvalue = true;
				break;
		}
	}
	return rvalue;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool LoadRemnants( UI32 arrayOffset )
//|	Date			-	21st January, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	After handling data specific load, other parts go here
//o--------------------------------------------------------------------------
bool CChar::LoadRemnants( void )
{
	bool rvalue = true;
	SetSerial( serial );

	if( IsNpc() && IsAtWar() )
		SetWar( false );

	CTownRegion *tRegion = calcRegionFromXY( GetX(), GetY(), worldNumber );
	SetRegion( (tRegion != NULL ? tRegion->GetRegionNum() : 0xFF) );
	SetTimer( tCHAR_ANTISPAM, 0 );
	if( GetID() != GetOrgID() && !IsDead() )
		SetID( GetOrgID() );

	const UI16 acct = GetAccount().wAccountIndex;
	if( GetID() > 0x3E1 )
	{
		if( acct == AB_INVALID_ID )
		{
			Console.Warning( 2, "NPC: %s with serial 0x%X with bugged body found, deleting", GetName().c_str(), GetSerial() );
			rvalue = false;
		}
		else
			SetID( 0x0190 );
	}
	if( rvalue )
	{
		MapRegion->AddChar( this );

		const SI16 mx = GetX();
		const SI16 my = GetY();

		MapData_st& mMap = Map->GetMapData( worldNumber );
		const bool overRight = ( mx > mMap.xBlock );
		const bool overBottom = ( my > mMap.yBlock );

		if( acct == AB_INVALID_ID && ( ( overRight && mx < 7000 ) || ( overBottom && my < 7000 ) || mx < 0 || my < 0 ) )
		{
			if( IsNpc() )
			{
				Console.Warning( 2, "NPC: %s with serial 0x%X found outside valid world locations, deleting", GetName().c_str(), GetSerial() );
				rvalue = false;
			}
			else
				SetLocation( 1000, 1000, 0 );
		}
		if( rvalue )
		{
			if( !IsNpc() )
				Accounts->AddCharacter( GetAccountNum(), this );	// account ID, and account object.
		}
	}
	return rvalue;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool SkillUsed( UI08 skillNum )
//|	Date			-	May 2, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the skill is being used
//o--------------------------------------------------------------------------
bool CChar::SkillUsed( UI08 skillNum ) const
{
	bool rvalue = false;
	if( skillNum < ALLSKILLS )
	{
		UI08 part		= static_cast<UI08>(skillNum / 32);
		UI08 offset		= static_cast<UI08>(skillNum % 32);
		UI32 compMask	= power( 2, offset );
		rvalue			= MFLAGGET( skillUsed[part], compMask );
	}
	return rvalue;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SkillUsed( bool value, UI08 skillNum )
//|	Date			-	May 2, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets whether the skill skillNum is being used or not
//o--------------------------------------------------------------------------
void CChar::SkillUsed( bool value, UI08 skillNum )
{
	if( skillNum < ALLSKILLS )
	{
		UI08 part		= static_cast<UI08>(skillNum / 32);
		UI08 offset		= static_cast<UI08>(skillNum % 32);
		UI32 compMask	= power( 2, offset );
		MFLAGSET( skillUsed[part], value, compMask );
	}
}

void CChar::PostLoadProcessing( void )
{
	CBaseObject::PostLoadProcessing();
	SERIAL tempSerial = (SERIAL)packitem;		// we stored the serial in packitem
	if( tempSerial != INVALIDSERIAL )
		SetPackItem( calcItemObjFromSer( tempSerial ) );
	else
		SetPackItem( NULL );
	if( GetWeight() <= 0 || GetWeight() > MAX_WEIGHT )
		SetWeight( Weight->calcCharWeight( this ) );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
		Skills->updateSkillLevel( this, i );
	// We need to add things to petlists, so we can cleanup after ourselves properly - Zane
	SetPostLoaded( true );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsJailed( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is in a jail cell
//o---------------------------------------------------------------------------o
bool CChar::IsJailed( void ) const
{
	return ( GetCell() != -1 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsMounted( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is mounted
//o---------------------------------------------------------------------------o
/*
bool CChar::IsMounted( void ) const
{
	return ( GetX() >= 7000 || GetY() >= 7000 );
}
*/

//o---------------------------------------------------------------------------o
//|	Function	-	bool CChar::isHuman( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if character is Human or Creature
//o---------------------------------------------------------------------------o
bool CChar::isHuman( void )
{
	bool rvalue			= false;
	const UI16 mOrgID	= GetOrgID();
	if( mOrgID == 0x0190 || mOrgID == 0x0191 || mOrgID == 0x025D || mOrgID == 0x025E || GetOrgID() == 0x03DB )	// GM's should be considered "Human" as well
		rvalue = true;
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CChar::inDungeon( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a dungeon
//o---------------------------------------------------------------------------o
bool CChar::inDungeon( void )
{
	bool rValue = false;
	if( GetRegion() != NULL )
        rValue = GetRegion()->IsDungeon();
	return rValue;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CChar::inBuilding( void )
//|	Programmer	-	grimson
//o---------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a building
//o---------------------------------------------------------------------------o
bool CChar::inBuilding( void )
{
	return isInBuilding;
}

//o---------------------------------------------------------------------------o
//|		Function    :	void talk( CSocket *s, SI32 dictEntry, bool antispam, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Npc speech
//o---------------------------------------------------------------------------o
void CChar::talk( CSocket *s, SI32 dictEntry, bool antispam, ... )
{
    if( s != NULL )
	{
		std::string txt = Dictionary->GetEntry( dictEntry, s->Language() );
		if( !txt.empty() )
		{
			va_list argptr;
			char msg[512];
			va_start( argptr, antispam );
			vsprintf( msg, txt.c_str(), argptr );
			va_end( argptr );

			talk( s, msg, antispam );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void talk( CSocket *s, std::string txt, bool antispam )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Npc speech
//o---------------------------------------------------------------------------o
void CChar::talk( CSocket *s, std::string txt, bool antispam )
{
	bool cont = !antispam;
	if( s != NULL && !txt.empty() )
	{
		if( antispam )
		{
			if( GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
			{
				SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 10 ) );
				cont = true;
			}
		}
		if( cont )
		{
			CChar *mChar		= s->CurrcharObj();
			CSpeechEntry& toAdd = SpeechSys->Add();

			toAdd.Font( (FontType)GetFontType() );
			toAdd.Speech( txt );
			toAdd.Speaker( GetSerial() );
			toAdd.SpokenTo( mChar->GetSerial() );
			toAdd.Type( TALK );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_INDIVIDUAL );
			if( GetNPCAiType() == aiEVIL )
				toAdd.Colour( 0x0026 );
			else if( GetSayColour() == 0x1700 )
				toAdd.Colour( 0x5A );
			else if( GetSayColour() == 0x0 )
				toAdd.Colour( 0x5A );
			else
				toAdd.Colour( GetSayColour() );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void talkAll( std::string txt, bool antispam )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC speech to all in range.
//o---------------------------------------------------------------------------o
void CChar::talkAll( std::string txt, bool antispam )
{
	bool cont = !antispam;
	if( !txt.empty() )
	{
		if( antispam )
		{
			if( GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
			{
				SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 10 ) );
				cont = true;
			}
		}
		if( cont )
		{
			CSpeechEntry& toAdd = SpeechSys->Add();

			toAdd.Font( (FontType)GetFontType() );
			toAdd.Speech( txt );
			toAdd.Speaker( GetSerial() );
			toAdd.SpokenTo( INVALIDSERIAL );
			toAdd.Type( TALK );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_PCNPC );
			if( GetNPCAiType() == aiEVIL )
				toAdd.Colour( 0x0026 );
			else if( GetSayColour() == 0x1700 )
				toAdd.Colour( 0x5A );
			else if( GetSayColour() == 0x0 )
				toAdd.Colour( 0x5A );
			else
				toAdd.Colour( GetSayColour() );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void talkAll( SI32 dictEntry, bool antispam, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC speech to all in range.
//o---------------------------------------------------------------------------o
void CChar::talkAll( SI32 dictEntry, bool antispam, ... )
{
	SOCKLIST nearbyChars = FindNearbyPlayers( this );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		CSocket *tSock = (*cIter);
		std::string txt = Dictionary->GetEntry( dictEntry, tSock->Language() );
		if( !txt.empty() )
		{
			char msg[512];
			va_list argptr;
			va_start( argptr, antispam );
			vsprintf( msg, txt.c_str(), argptr );
			va_end( argptr );
			talk( tSock, msg, antispam );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void emote( CSocket *s, std::string txt, bool antispam )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC emotes
//o---------------------------------------------------------------------------o
void CChar::emote( CSocket *s, std::string txt, bool antispam )
{
	bool cont = !antispam;
	if( s != NULL && !txt.empty() )
	{
		if( antispam )
		{
			if( GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
			{
				SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 10 ) );
				cont = true;
			}
		}
		if( cont )
		{
			CChar *mChar		= s->CurrcharObj();
			CSpeechEntry& toAdd = SpeechSys->Add();

			toAdd.Font( (FontType)GetFontType() );

			toAdd.Speech( txt );
			toAdd.Speaker( GetSerial() );
			toAdd.SpokenTo( mChar->GetSerial() );
			toAdd.Type( EMOTE );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( SPTRG_INDIVIDUAL );
			toAdd.Colour( GetEmoteColour() );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void emote( CSocket *s, SI32 dictEntry, bool antispam, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC emotes
//o---------------------------------------------------------------------------o
void CChar::emote( CSocket *s, SI32 dictEntry, bool antispam, ... )
{
	if( s != NULL )
	{
		std::string txt = Dictionary->GetEntry( dictEntry, s->Language() );
		if( !txt.empty() )
		{
			va_list argptr;

			char msg[512];
			va_start( argptr, antispam );
			vsprintf( msg, txt.c_str(), argptr );
			va_end( argptr );
			emote( s, msg, antispam );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void emoteAll( SI32 dictEntry, bool antispam, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC emotes to all in range.
//o---------------------------------------------------------------------------o
void CChar::emoteAll( SI32 dictEntry, bool antispam, ... )
{
	SOCKLIST nearbyChars = FindNearbyPlayers( this );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		CSocket *tSock = (*cIter);
		std::string txt = Dictionary->GetEntry( dictEntry, tSock->Language() );
		if( !txt.empty() )
		{
			char msg[512];
			va_list argptr;
			va_start( argptr, antispam );
			vsprintf( msg, txt.c_str(), argptr );
			va_end( argptr );
			emote( tSock, msg, antispam );
		}
	}
}

void CChar::SendWornItems( CSocket *mSock )
{
	SetOnHorse( false );
	CPWornItem toSend;
	for( CItem *i = FirstItem(); !FinishedItems(); i = NextItem() )
	{
		if( ValidateObject( i ) && !i->isFree() )
		{
			if( i->GetLayer() == IL_MOUNT )
				SetOnHorse( true );
			toSend = (*i);
			mSock->Send( &toSend );
		}
	}
}

void CChar::WalkZ( SI08 newZ )
{
	oldLocZ = z;
	z		= newZ;
}

void CChar::WalkDir( SI08 newDir )
{
	dir = newDir;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::Cleanup( void )
//|   Date        -  11/6/2003
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Cleans up after character
//o---------------------------------------------------------------------------o
void CChar::Cleanup( void )
{
	if( !isFree() )	// We're not the default item in the handler
	{
		MapRegion->RemoveChar( this );

		JSEngine->ReleaseObject( IUE_CHAR, this );

		CBaseObject::Cleanup();

		RemoveFromSight();

		for( CItem *tItem = FirstItem(); !FinishedItems(); tItem = NextItem() )
		{
			if( ValidateObject( tItem ) )
				tItem->Delete();
		}
		CChar *tempChar = NULL;
		tempChar = GetTarg();
		if( ValidateObject( tempChar ) )
		{
			if( tempChar->GetTarg() == this )
			{
				tempChar->SetTarg( NULL );
				tempChar->SetAttacker( NULL );
				tempChar->SetAttackFirst( false );
				if( tempChar->IsAtWar() )
					tempChar->ToggleCombat();
			}
			SetTarg( NULL );
		}
		tempChar = GetAttacker();
		if( ValidateObject( tempChar ) )
		{
			if( tempChar->GetAttacker() == this )
				tempChar->SetAttacker( NULL );

			if( tempChar->GetTarg() == this )
			{
				tempChar->SetTarg( NULL );
				tempChar->SetAttackFirst( false );
				if( tempChar->IsAtWar() )
					tempChar->ToggleCombat();
			}
			SetAttacker( NULL );
		}

		// If we delete a NPC we should delete his tempeffects as well
		cwmWorldState->tempEffects.Push();
		for( CTEffect *Effect = cwmWorldState->tempEffects.First(); !cwmWorldState->tempEffects.Finished(); Effect = cwmWorldState->tempEffects.Next() )
		{
			if( Effect->Destination() == GetSerial() )
				cwmWorldState->tempEffects.Remove( Effect, true );
			
			if( Effect->Source() == GetSerial() )
				Effect->Source( INVALIDSERIAL );
		}
		cwmWorldState->tempEffects.Pop();

		// if we delete a NPC we should delete him from spawnregions
		// this will fix several crashes
		if( IsNpc() && isSpawned() )
		{
			if( GetSpawn() < BASEITEMSERIAL )
			{
				UI16 spawnRegNum = static_cast<UI16>(GetSpawn());
				if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
				{
					CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
					if( spawnReg != NULL )
						spawnReg->deleteSpawnedChar( this );
				}
			}
		}
		
		if( !IsNpc() )
		{
			ACCOUNTSBLOCK& mAcct = GetAccount();
			if( mAcct.wAccountIndex != AB_INVALID_ID )
			{
				for( UI08 actr = 0; actr < 6; ++actr )
				{
					if( mAcct.lpCharacters[actr] != NULL && mAcct.lpCharacters[actr]->GetSerial() == GetSerial() )
					{
						Accounts->DelCharacter( mAcct.wAccountIndex, actr );
						break;
					}
				}
			}
		}

		if( GetSpawnObj() != NULL )
			SetSpawn( INVALIDSERIAL );

		if( IsGuarded() )
		{
			CChar *petGuard = Npcs->getGuardingPet( this, this );
			if( ValidateObject( petGuard ) )
				petGuard->SetGuarding( NULL );
			SetGuarded( false );
		}
		Npcs->stopPetGuarding( this );

		for( tempChar = petsControlled.First(); !petsControlled.Finished(); tempChar = petsControlled.Next() )
		{
			if( ValidateObject( tempChar ) )
				tempChar->SetOwner( NULL );
		}
		RemoveSelfFromOwner();	// Let's remove it from our owner (if any)
	}
}

void CChar::SetStrength( SI16 newValue )
{
	CBaseObject::SetStrength( newValue );
	Dirty( UT_HITPOINTS );
}

void CChar::SetDexterity( SI16 newValue )
{
	CBaseObject::SetDexterity( newValue );
	Dirty( UT_STAMINA );
}

void CChar::SetIntelligence( SI16 newValue )
{
	CBaseObject::SetIntelligence( newValue );
	Dirty( UT_MANA );
}

void CChar::SetHP( SI16 newValue )
{
	CBaseObject::SetHP( UOX_MIN( UOX_MAX( static_cast<SI16>(0), newValue ), static_cast<SI16>(GetMaxHP()) ) );
	Dirty( UT_HITPOINTS );
}

void CChar::SetMana( SI16 newValue )
{
	CBaseObject::SetMana( UOX_MIN( UOX_MAX( static_cast<SI16>(0), newValue ), GetMaxMana() ) );
	Dirty( UT_MANA );
}

void CChar::SetStamina( SI16 newValue )
{
	CBaseObject::SetStamina( UOX_MIN( UOX_MAX( static_cast<SI16>(0), newValue ), GetMaxStam() ) );
	Dirty( UT_STAMINA );
}

void CChar::SetPoisoned( UI08 newValue )
{
	CBaseObject::SetPoisoned( newValue );
	Dirty( UT_UPDATE );
}

void CChar::SetStrength2( SI16 nVal )
{
	CBaseObject::SetStrength2( nVal );
	Dirty( UT_HITPOINTS );
}

void CChar::SetDexterity2( SI16 nVal )
{
	CBaseObject::SetDexterity2( nVal );
	Dirty( UT_STAMINA );
}

void CChar::SetIntelligence2( SI16 nVal )
{
	CBaseObject::SetIntelligence2( nVal );
	Dirty( UT_MANA );
}

void CChar::IncStamina( SI16 toInc )
{
	SetStamina( GetStamina() + toInc );
}

void CChar::IncMana( SI16 toInc )
{
	SetMana( GetMana() + toInc );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void npcToggleCombat( CChar *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	NPC Toggles combat mode
//o---------------------------------------------------------------------------o
void CChar::ToggleCombat( void )
{
	SetWar( !IsAtWar() );
	Movement->CombatWalk( this );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool CanBeObjType()
//|	Date			-	24 June, 2004
//|	Programmer		-	Maarc
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Indicates whether an object can behave as a
//|						particular type
//o--------------------------------------------------------------------------
bool CChar::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CBaseObject::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_CHAR )
			rvalue = true;
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool Delete()
//|   Date        -  11/6/2003
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds character to deletion queue
//o---------------------------------------------------------------------------o
void CChar::Delete( void )
{
	++(cwmWorldState->deletionQueue[this]);
	Cleanup();
	SetDeleted( true );
	ShouldSave( false );
}

// Player Only Functions

//o--------------------------------------------------------------------------o
//|	Function		-	ACCOUNTSBLOCK& Account()
//|	Date			-	1/14/2003 6:17:45 AM
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Sets and Returns the ACCOUNTSBLOCK associated with this player
//o--------------------------------------------------------------------------o
void CChar::SetAccount( ACCOUNTSBLOCK& actbAccount )
{
	if( !IsValidPlayer() )
	{
		if( actbAccount.wAccountIndex != DEFPLAYER_ACCOUNTNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->accountNum = actbAccount.wAccountIndex;
}
ACCOUNTSBLOCK& CChar::GetAccount( void ) 
{
	UI16 rVal = AB_INVALID_ID;
	if( IsValidPlayer() )
		rVal = mPlayer->accountNum;

	return Accounts->GetAccountByID( rVal );
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 AccountNum()
//|	Date			-	1/14/2003 6:17:45 AM
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Sets and Returns the account number associated with this player
//o--------------------------------------------------------------------------o
UI16 CChar::GetAccountNum( void ) const
{
	UI16 rVal = AB_INVALID_ID;
	if( IsValidPlayer() )
		rVal = mPlayer->accountNum;
	return rVal;
}
void CChar::SetAccountNum( UI16 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_ACCOUNTNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->accountNum = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL Robe() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players death robe
//o---------------------------------------------------------------------------o
SERIAL CChar::GetRobe( void ) const
{
	SERIAL rVal = DEFPLAYER_ROBE;
	if( IsValidPlayer() )
		rVal = mPlayer->robe;
	return rVal;
}
void CChar::SetRobe( SERIAL newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_ROBE )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->robe = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  std::string LastOn() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Time player was last online
//o---------------------------------------------------------------------------o
std::string CChar::GetLastOn( void ) const
{
	std::string rVal = "";
	if( IsValidPlayer() )
		rVal = mPlayer->lastOn;
	return rVal;
}
void CChar::SetLastOn( std::string newValue )
{
	if( !IsValidPlayer() )
	{
		if( !newValue.empty() )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->lastOn = newValue;
}

UI32 CChar::GetLastOnSecs( void ) const
{
	UI32 rVal = 0;
	if( IsValidPlayer() )
		rVal = mPlayer->lastOnSecs;
	return rVal;
}
void CChar::SetLastOnSecs( UI32 newValue )
{
	if( IsValidPlayer() )
		mPlayer->lastOnSecs = newValue;
}


//o---------------------------------------------------------------------------o
//|   Function    -  bool GetMounted() 
//|   Date        -  23. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Is NPC currently mounted
//o---------------------------------------------------------------------------o
bool CChar::GetMounted( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
	{
		rVal = MFLAGGET( mNPC->boolFlags, BIT_MOUNTED );
	}
	return rVal;
}
void CChar::SetMounted( bool newValue )
{
	if( IsValidNPC() )
	{
		MFLAGSET( mNPC->boolFlags, newValue, BIT_MOUNTED )
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetStabled() 
//|   Date        -  23. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Is NPC currently stabled
//o---------------------------------------------------------------------------o
bool CChar::GetStabled( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
	{
		rVal = MFLAGGET( mNPC->boolFlags, BIT_STABLED );
	}
	return rVal;
}
void CChar::SetStabled( bool newValue )
{
	if( IsValidNPC() )
	{
		MFLAGSET( mNPC->boolFlags, newValue, BIT_STABLED )
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetMaxHPFixed() 
//|   Date        -  25. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Is the maximum HP fixed.
//o---------------------------------------------------------------------------o
bool CChar::GetMaxHPFixed( void ) const
{
	return MFLAGGET( bools2, BIT_MAXHPFIXED );
}
void CChar::SetMaxHPFixed( bool newValue )
{
	MFLAGSET( bools2, newValue, BIT_MAXHPFIXED )
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetMaxManaFixed() 
//|   Date        -  25. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Is the maximum MANA fixed.
//o---------------------------------------------------------------------------o
bool CChar::GetMaxManaFixed( void ) const
{
	return MFLAGGET( bools2, BIT_MAXMANAFIXED );
}
void CChar::SetMaxManaFixed( bool newValue )
{
	MFLAGSET( bools2, newValue, BIT_MAXMANAFIXED )
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetMaxStamFixed() 
//|   Date        -  25. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Is the maximum STAMINA fixed.
//o---------------------------------------------------------------------------o
bool CChar::GetMaxStamFixed( void ) const
{
	return MFLAGGET( bools2, BIT_MAXSTAMFIXED );
}
void CChar::SetMaxStamFixed( bool newValue )
{
	MFLAGSET( bools2, newValue, BIT_MAXSTAMFIXED )
}

//o---------------------------------------------------------------------------o
//|   Function    -  std::string OrgName() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players original name
//o---------------------------------------------------------------------------o
std::string CChar::GetOrgName( void ) const
{
	std::string rVal = "";
	if( IsValidPlayer() )
		rVal = mPlayer->origName;
	return rVal;
}
void CChar::SetOrgName( std::string newName )
{
	if( !IsValidPlayer() )
	{
		if( !newName.empty() )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->origName = newName;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 OrgID() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players original ID
//o---------------------------------------------------------------------------o
UI16 CChar::GetOrgID( void ) const
{
	UI16 rVal = GetID();
	if( IsValidPlayer() )
		rVal = mPlayer->origID;
	return rVal;
}
void CChar::SetOrgID( UI16 value )
{
	if( IsValidPlayer() )
		mPlayer->origID = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 OrgSkin() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players original skin
//o---------------------------------------------------------------------------o
UI16 CChar::GetOrgSkin( void ) const
{
	UI16 rVal = GetSkin();
	if( IsValidPlayer() )
		rVal = mPlayer->origSkin;
	return rVal;
}
void CChar::SetOrgSkin( UI16 value )
{
	if( IsValidPlayer() )
		mPlayer->origSkin = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 HairStyle() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players default hairstyle
//o---------------------------------------------------------------------------o
UI16 CChar::GetHairStyle( void ) const
{
	UI16 rVal = DEFPLAYER_HAIRSTYLE;
	if( IsValidPlayer() )
		rVal = mPlayer->hairStyle;
	return rVal;
}
void CChar::SetHairStyle( UI16 value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_HAIRSTYLE )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->hairStyle = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 BeardStyle() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players default beardstyle
//o---------------------------------------------------------------------------o
UI16 CChar::GetBeardStyle( void ) const
{
	UI16 rVal = DEFPLAYER_BEARDSTYLE;
	if( IsValidPlayer() )
		rVal = mPlayer->beardStyle;
	return rVal;
}
void CChar::SetBeardStyle( UI16 value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_BEARDSTYLE )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->beardStyle = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  COLOUR HairColour() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players default haircolour
//o---------------------------------------------------------------------------o
COLOUR CChar::GetHairColour( void ) const
{
	COLOUR rVal = DEFPLAYER_HAIRCOLOUR;
	if( IsValidPlayer() )
		rVal = mPlayer->hairColour;
	return rVal;
}
void CChar::SetHairColour( COLOUR value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_HAIRCOLOUR )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->hairColour = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  COLOUR BeardColour() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players default beardcolour
//o---------------------------------------------------------------------------o
COLOUR CChar::GetBeardColour( void ) const
{
	COLOUR rVal = DEFPLAYER_BEARDCOLOUR;
	if( IsValidPlayer() )
		rVal = mPlayer->beardColour;
	return rVal;
}
void CChar::SetBeardColour( COLOUR value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_BEARDCOLOUR )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->beardColour = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CChar * TrackingTarget() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players selected tracking target
//o---------------------------------------------------------------------------o
CChar *CChar::GetTrackingTarget( void ) const
{
	CChar *rVal = NULL;
	if( IsValidPlayer() )
		rVal = calcCharObjFromSer( mPlayer->trackingTarget );
	return rVal;
}
void CChar::SetTrackingTarget( CChar *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != NULL )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->trackingTarget = calcSerFromObj( newValue );
}

//o---------------------------------------------------------------------------o
//|   Function    -  CChar * TrackingTargets( UI08 targetNum ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  List of possible tracking targets
//o---------------------------------------------------------------------------o
CChar *CChar::GetTrackingTargets( UI08 targetNum ) const
{
	CChar *rVal = NULL;
	if( IsValidPlayer() )
	{
		if( targetNum < mPlayer->trackingTargets.size() )
			rVal = mPlayer->trackingTargets[targetNum];
	}
	return rVal;
}
void CChar::SetTrackingTargets( CChar *newValue, UI08 targetNum )
{
	if( !IsValidPlayer() )
	{
		if( newValue != NULL )
			CreatePlayer();
	}
	if( IsValidPlayer() )
	{
		if( targetNum < mPlayer->trackingTargets.size() )
			mPlayer->trackingTargets[targetNum] = newValue;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 CommandLevel() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players command level
//o---------------------------------------------------------------------------o
UI08 CChar::GetCommandLevel( void ) const
{
	UI08 rVal = DEFPLAYER_COMMANDLEVEL;
	if( IsValidPlayer() )
		rVal = mPlayer->commandLevel;
	return rVal;
}
void CChar::SetCommandLevel( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_COMMANDLEVEL )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->commandLevel = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 PostType() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players messageboard posting level
//o---------------------------------------------------------------------------o
UI08 CChar::GetPostType( void ) const
{
	UI08 rVal = DEFPLAYER_POSTTYPE;
	if( IsValidPlayer() )
		rVal = mPlayer->postType;
	return rVal;
}
void CChar::SetPostType( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_POSTTYPE )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->postType = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 CallNum() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  GM or Counsellors currently active call number
//o---------------------------------------------------------------------------o
SI16 CChar::GetCallNum( void ) const
{
	SI16 rVal = DEFPLAYER_CALLNUM;
	if( IsValidPlayer() )
		rVal = mPlayer->callNum;
	return rVal;
}
void CChar::SetCallNum( SI16 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_CALLNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->callNum = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 PlayerCallNum() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players call number in the GM/CNS Queue
//o---------------------------------------------------------------------------o
SI16 CChar::GetPlayerCallNum( void ) const
{
	SI16 rVal = DEFPLAYER_PLAYERCALLNUM;
	if( IsValidPlayer() )
		rVal = mPlayer->playerCallNum;
	return rVal;
}
void CChar::SetPlayerCallNum( SI16 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_PLAYERCALLNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->playerCallNum = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 Squelched() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Players squelch status
//o---------------------------------------------------------------------------o
UI08 CChar::GetSquelched( void ) const
{
	UI08 rVal = DEFPLAYER_SQUELCHED;
	if( IsValidPlayer() )
		rVal = mPlayer->squelched;
	return rVal;
}
void CChar::SetSquelched( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_SQUELCHED )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->squelched = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem * SpeechItem()
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Item related to the speech we're working on
//|                  IE the item for name deed if we're renaming ourselves
//o---------------------------------------------------------------------------o
CItem *CChar::GetSpeechItem( void ) const
{
	CItem *rVal = NULL;
	if( IsValidPlayer() )
		rVal = mPlayer->speechItem;
	return rVal;
}
void CChar::SetSpeechItem( CItem *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != NULL )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->speechItem = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 SpeechMode()
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Speech mode information, as to what mode of speech
//|                  we are in.  Valid values are found just below
//|                  0 normal speech
//|                  1 GM page
//|                  2 Counselor page
//|                  3 Player Vendor item pricing
//|                  4 Player Vendor item describing
//|                  5 Key renaming
//|                  6 Name deed
//|                  7 Rune renaming
//|                  8 Sign renaming
//|                  9 JS Speech
//o---------------------------------------------------------------------------o
UI08 CChar::GetSpeechMode( void ) const
{
	UI08 rVal = DEFPLAYER_SPEECHMODE;
	if( IsValidPlayer() )
		rVal = mPlayer->speechMode;
	return rVal;
}
void CChar::SetSpeechMode( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_SPEECHMODE )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->speechMode = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 SpeechID()
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  The ID for the JS Speech Input
//o---------------------------------------------------------------------------o
void CChar::SetSpeechID( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_SPEECHID )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->speechID = newValue;
}
UI08 CChar::GetSpeechID( void ) const
{
	UI08 rVal = DEFPLAYER_SPEECHID;
	if( IsValidPlayer() )
		rVal = mPlayer->speechID;
	return rVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  cScript * SpeechCallback()
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Callback for the onSpeechInput function
//o---------------------------------------------------------------------------o
cScript *CChar::GetSpeechCallback( void ) const
{
	cScript *rVal = NULL;
	if( IsValidPlayer() )
		rVal = mPlayer->speechCallback;
	return rVal;
}
void CChar::SetSpeechCallback( cScript *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != NULL )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->speechCallback = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 FixedLight()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Fixed light level of the character
//|                  255 is off
//o---------------------------------------------------------------------------o
UI08 CChar::GetFixedLight( void ) const
{
	UI08 rVal = DEFPLAYER_FIXEDLIGHT;
	if( IsValidPlayer() )
		rVal = mPlayer->fixedLight;
	return rVal;
}
void CChar::SetFixedLight( UI08 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_FIXEDLIGHT )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->fixedLight = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 Deaths()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Total number of deaths a player has
//o---------------------------------------------------------------------------o
UI16 CChar::GetDeaths( void ) const
{
	UI16 rVal = DEFPLAYER_DEATHS;
	if( IsValidPlayer() )
		rVal = mPlayer->deaths;
	return rVal;
}
void CChar::SetDeaths( UI16 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_DEATHS )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->deaths = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CSocket *Socket()
//|   Date        -  November 7, 2005
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Socket attached to the player character
//o---------------------------------------------------------------------------o
CSocket * CChar::GetSocket( void ) const
{
	CSocket *rVal = NULL;
	if( IsValidPlayer() )
		rVal = mPlayer->socket;
	return rVal;
}
void CChar::SetSocket( CSocket *newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != NULL )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->socket = newVal;
}

// NPC Only Functions

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 NPCAiType()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  AI type of the NPC
//o---------------------------------------------------------------------------o
SI16 CChar::GetNPCAiType( void ) const
{
	SI16 rVal = DEFNPC_AITYPE;
	if( IsValidNPC() )
		rVal = mNPC->aiType;
	return rVal;
}
void CChar::SetNPCAiType( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_AITYPE )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->aiType = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL Guarding()
//|   Date        -  Unknown
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  SERIAL of the object the character is guarding
//o---------------------------------------------------------------------------o
CBaseObject *CChar::GetGuarding( void ) const
{
	CBaseObject *rVal = NULL;
	if( IsValidNPC() )
		rVal = mNPC->petGuarding;
	return rVal;
}
void CChar::SetGuarding( CBaseObject *newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != NULL )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->petGuarding = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Taming()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Minimum skill required to tame the character
//o---------------------------------------------------------------------------o
SI16 CChar::GetTaming( void ) const
{
	SI16 rVal = DEFNPC_TAMING;
	if( IsValidNPC() )
		rVal = mNPC->taming;
	return rVal;
}
void CChar::SetTaming( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_TAMING )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->taming = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Peaceing()
//|   Date        -  25. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Minimum skill required to peace the character
//o---------------------------------------------------------------------------o
SI16 CChar::GetPeaceing( void ) const
{
	SI16 rVal = DEFNPC_PEACEING;
	if( IsValidNPC() )
		rVal = mNPC->peaceing;
	else if( IsValidPlayer() )
		rVal = static_cast<SI16>( GetBaseSkill( 9 ) );
	return rVal;
}
void CChar::SetPeaceing( SI16 newValue )
{
	if( IsValidNPC() )
		mNPC->peaceing = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Provoing()
//|   Date        -  25. Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Minimum skill required to provocate the character
//o---------------------------------------------------------------------------o
SI16 CChar::GetProvoing( void ) const
{
	SI16 rVal = DEFNPC_PROVOING;
	if( IsValidNPC() )
		rVal = mNPC->provoing;
	else if( IsValidPlayer() )
		rVal = static_cast<SI16>( GetBaseSkill( 22 ) );
	return rVal;
}
void CChar::SetProvoing( SI16 newValue )
{
	if( IsValidNPC() )
		mNPC->provoing = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 TrainingPlayerIn()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Skill the player is being trained in. 255 is no training
//o---------------------------------------------------------------------------o
UI08 CChar::GetTrainingPlayerIn( void ) const
{
	UI08 rVal = DEFNPC_TRAININGPLAYERIN;
	if( IsValidNPC() )
		rVal = mNPC->trainingPlayerIn;
	return rVal;
}
void CChar::SetTrainingPlayerIn( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_TRAININGPLAYERIN )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->trainingPlayerIn = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI32 HoldG()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Amount of gold being held by a Player Vendor
//o---------------------------------------------------------------------------o
UI32 CChar::GetHoldG( void ) const
{
	UI32 rVal = DEFNPC_HOLDG;
	if( IsValidNPC() )
		rVal = mNPC->goldOnHand;
	return rVal;
}
void CChar::SetHoldG( UI32 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_HOLDG )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->goldOnHand = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 Split()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Split level of the character
//o---------------------------------------------------------------------------o
UI08 CChar::GetSplit( void ) const
{
	UI08 rVal = DEFNPC_SPLIT;
	if( IsValidNPC() )
		rVal = mNPC->splitNum;
	return rVal;
}
void CChar::SetSplit( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPLIT )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->splitNum = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 SplitChance()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  The chance of an NPC splitting
//o---------------------------------------------------------------------------o
UI08 CChar::GetSplitChance( void ) const
{
	UI08 rVal = DEFNPC_SPLITCHANCE;
	if( IsValidNPC() )
		rVal = mNPC->splitChance;
	return rVal;
}
void CChar::SetSplitChance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPLITCHANCE )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->splitChance = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 Fx( UI08 part )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  x1 and x2 boundry of an npc wander area
//o---------------------------------------------------------------------------o
SI16 CChar::GetFx( UI08 part ) const
{
	SI16 rVal = DEFNPC_WANDERAREA;
	if( IsValidNPC() )
	{
		if( part < 2 )
			rVal = mNPC->fx[part];
	}
	return rVal;
}
void CChar::SetFx( SI16 newVal, UI08 part )
{
	if( !IsValidNPC() )
	{
		if( newVal != DEFNPC_WANDERAREA )
			CreateNPC();
	}
	if( IsValidNPC() )
	{
		if( part < 2 )
			mNPC->fx[part] = newVal;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 Fy( UI08 part )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  y1 and y2 boundry of an npc wander area
//o---------------------------------------------------------------------------o
SI16 CChar::GetFy( UI08 part ) const
{
	SI16 rVal = DEFNPC_WANDERAREA;
	if( IsValidNPC() )
	{
		if( part < 2 )
			rVal = mNPC->fy[part];
	}
	return rVal;
}
void CChar::SetFy( SI16 newVal, UI08 part )
{
	if( !IsValidNPC() )
	{
		if( newVal != DEFNPC_WANDERAREA )
			CreateNPC();
	}
	if( IsValidNPC() )
	{
		if( part < 2 )
			mNPC->fy[part] = newVal;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Fz()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  z of an npc wander area
//o---------------------------------------------------------------------------o
SI08 CChar::GetFz( void ) const
{
	SI08 rVal = DEFNPC_FZ1;
	if( IsValidNPC() )
		rVal = mNPC->fz;
	return rVal;
}
void CChar::SetFz( SI08 newVal )
{
	if( !IsValidNPC() )
	{
		if( newVal != DEFNPC_FZ1 )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->fz = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 NpcWander()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC Wander mode
//o---------------------------------------------------------------------------o
SI08 CChar::GetNpcWander( void ) const
{
	SI08 rVal = DEFNPC_WANDER;
	if( IsValidNPC() )
		rVal = mNPC->wanderMode;
	return rVal;
}
void CChar::SetNpcWander( SI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_WANDER )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->wanderMode = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 OldNpcWander()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Previous NPC Wander mode
//o---------------------------------------------------------------------------o
SI08 CChar::GetOldNpcWander( void ) const
{
	SI08 rVal = DEFNPC_OLDWANDER;
	if( IsValidNPC() )
		rVal = mNPC->oldWanderMode;
	return rVal;
}
void CChar::SetOldNpcWander( SI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_OLDWANDER )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->oldWanderMode = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CChar * FTarg()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC Follow target
//o---------------------------------------------------------------------------o
CChar *CChar::GetFTarg( void ) const
{
	CChar *rVal = NULL;
	if( IsValidNPC() )
		rVal = calcCharObjFromSer( mNPC->fTarg );
	return rVal;
}
void CChar::SetFTarg( CChar *newTarg )
{
	if( !IsValidNPC() )
	{
		if( newTarg != NULL )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->fTarg = calcSerFromObj( newTarg );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 SpAttack()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC's Spell Attack setting
//o---------------------------------------------------------------------------o
SI16 CChar::GetSpAttack( void ) const
{
	SI16 rVal = DEFNPC_SPATTACK;
	if( IsValidNPC() )
		rVal = mNPC->spellAttack;
	return rVal;
}
void CChar::SetSpAttack( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPATTACK )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->spellAttack = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 SpDelay()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC's Spell Delay setting
//o---------------------------------------------------------------------------o
SI08 CChar::GetSpDelay( void ) const
{
	SI08 rVal = DEFNPC_SPADELAY;
	if( IsValidNPC() )
		rVal = mNPC->spellDelay;
	return rVal;
}
void CChar::SetSpDelay( SI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPADELAY )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->spellDelay = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 QuestType()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC's Quest Type
//o---------------------------------------------------------------------------o
UI08 CChar::GetQuestType( void ) const
{
	UI08 rVal = DEFNPC_QUESTTYPE;
	if( IsValidNPC() )
		rVal = mNPC->questType;
	return rVal;
}
void CChar::SetQuestType( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_QUESTTYPE )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->questType = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 QuestOrigRegion()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC's Quest Origin Region
//o---------------------------------------------------------------------------o
UI08 CChar::GetQuestOrigRegion( void ) const
{
	UI08 rVal = DEFNPC_QUESTORIGREGION;
	if( IsValidNPC() )
		rVal = mNPC->questOrigRegion;
	return rVal;
}
void CChar::SetQuestOrigRegion( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_QUESTORIGREGION )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->questOrigRegion = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 QuestDestRegion()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  NPC's Quest Destination Region
//o---------------------------------------------------------------------------o
UI08 CChar::GetQuestDestRegion( void ) const
{
	UI08 rVal = DEFNPC_QUESTDESTREGION;
	if( IsValidNPC() )
		rVal = mNPC->questDestRegion;
	return rVal;
}
void CChar::SetQuestDestRegion( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_QUESTDESTREGION )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->questDestRegion = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 FleeAt()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Health value at which an NPC will turn tail and run.
//o---------------------------------------------------------------------------o
SI16 CChar::GetFleeAt( void ) const
{
	SI16 rVal = DEFNPC_FLEEAT;
	if( IsValidNPC() )
		rVal = mNPC->fleeAt;
	return rVal;
}
void CChar::SetFleeAt( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_FLEEAT )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->fleeAt = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI16 ReattackAt()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Health value at which an NPC will being fighting again.
//o---------------------------------------------------------------------------o
SI16 CChar::GetReattackAt( void ) const
{
	SI16 rVal = DEFNPC_REATTACKAT;
	if( IsValidNPC() )
		rVal = mNPC->reAttackAt;
	return rVal;
}
void CChar::SetReattackAt( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_REATTACKAT )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->reAttackAt = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 Push/Pop Direction()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Queue of directions for NPC
//o---------------------------------------------------------------------------o
UI08 CChar::PopDirection( void )
{
	UI08 rVal = 0;
	if( IsValidNPC() )
	{
		if( !mNPC->pathToFollow.empty() )
		{
			rVal = mNPC->pathToFollow.front();
			mNPC->pathToFollow.pop();
		}
	}
	return rVal;
}
void CChar::PushDirection( UI08 newDir )
{
	if( !IsValidNPC() )
		CreateNPC();

	mNPC->pathToFollow.push( newDir );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool StillGotDirs()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Check if Direction Queue is empty
//o---------------------------------------------------------------------------o
bool CChar::StillGotDirs( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
		rVal = !mNPC->pathToFollow.empty();
	return rVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  FlushPath()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Empty the Queue of Directions.
//o---------------------------------------------------------------------------o
void CChar::FlushPath( void )
{
	if( IsValidNPC() )
	{
		while( StillGotDirs() )
			PopDirection();
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  CHARLIST *GetFriendList( void )
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the characters list of friends
//o---------------------------------------------------------------------------o
CHARLIST *CChar::GetFriendList( void )
{
	CHARLIST *rVal = NULL;
	if( IsValidNPC() )
		rVal = &mNPC->petFriends;
	return rVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddFriend( CChar *toAdd ) const
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds the friend toAdd to the player's friends list
//|					 ensuring it is already not on it
//o---------------------------------------------------------------------------o
void CChar::AddFriend( CChar *toAdd )
{
	if( !IsValidNPC() )
	{
		if( toAdd != NULL )
			CreateNPC();
	}
	if( IsValidNPC() )
	{
		CHARLIST_CITERATOR i = mNPC->petFriends.begin();
		while( i != mNPC->petFriends.end() )
		{
			if( (*i) == toAdd )
				break;
			++i;
		}
		if( i == mNPC->petFriends.end() )
			mNPC->petFriends.push_back( toAdd );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemoveFriemd( CChar *toRemove )
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes the friend toRemove from the pets friends list
//o---------------------------------------------------------------------------o
void CChar::RemoveFriend( CChar *toRemove )
{
	if( IsValidNPC() )
	{
		for( CHARLIST_ITERATOR rIter = mNPC->petFriends.begin(); rIter != mNPC->petFriends.end(); ++rIter )
		{
			if( (*rIter) == toRemove )
			{
				mNPC->petFriends.erase( rIter );
				break;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  cNPC_Flag GetNPCFlag()
//|   Date        -  February 9, 2006
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the NPC's default flag color
//o---------------------------------------------------------------------------o
cNPC_FLAG CChar::GetNPCFlag( void ) const
{
	cNPC_FLAG retVal = fNPC_NEUTRAL;

	if( IsValidNPC() )
		retVal = mNPC->npcFlag;

	return retVal;
}
//o---------------------------------------------------------------------------o
//|   Function    -  SetNPCFlag( cNPC_Flag flagType )
//|   Date        -  February 9, 2006
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the NPC's default flag color
//o---------------------------------------------------------------------------o
void CChar::SetNPCFlag( cNPC_FLAG flagType )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->npcFlag = flagType;
}

bool DTEgreater( DamageTrackEntry *elem1, DamageTrackEntry *elem2 )
{
	if( elem1 == NULL )
		return false;
	if( elem2 == NULL )
		return true;
	return elem1->damageDone > elem2->damageDone;
}

void CChar::Heal( SI16 healValue, CChar *healer )
{
	SetHP( hitpoints + healValue );
	if( healer != NULL )
	{
		const SERIAL healerSerial	= healer->GetSerial();
		bool persFound				= false;
		for( DamageTrackEntry *i = damageHealed.First(); !damageHealed.Finished(); i = damageHealed.Next() )
		{
			if( i->damager == healerSerial )
			{
				i->damageDone		+= healValue;
				i->lastDamageDone	= cwmWorldState->GetUICurrentTime();
				persFound			= true;
				break;
			}
		}
		if( !persFound )
		{
			damageHealed.Add( new DamageTrackEntry( healerSerial, healValue, cwmWorldState->GetUICurrentTime() ) );
		}
		damageHealed.Sort( DTEgreater );
	}
}

void CChar::Damage( SI16 damageValue, CChar *attacker )
{
	SetHP( hitpoints - damageValue );
	if( attacker != NULL )
	{
		const SERIAL attackerSerial	= attacker->GetSerial();
		bool persFound				= false;
		for( DamageTrackEntry *i = damageDealt.First(); !damageDealt.Finished(); i = damageDealt.Next() )
		{
			if( i->damager == attackerSerial )
			{
				i->damageDone		+= damageValue;
				i->lastDamageDone	= cwmWorldState->GetUICurrentTime();
				persFound			= true;
				break;
			}
		}
		if( !persFound )
		{
			damageDealt.Add( new DamageTrackEntry( attackerSerial, damageValue, cwmWorldState->GetUICurrentTime() ) );
		}
		damageDealt.Sort( DTEgreater );
	}
}

void CChar::Die( void )
{
}

void CChar::UpdateDamageTrack( void )
{
	TIMERVAL currentTime	= cwmWorldState->GetUICurrentTime();
	DamageTrackEntry *i		= NULL;
	// Update the damage stuff
	for( i = damageDealt.First(); !damageDealt.Finished(); i = damageDealt.Next() )
	{
		if( i == NULL )
		{
			damageDealt.Remove( i );
			continue;
		}
		if( (i->lastDamageDone + 300000) < currentTime )	// if it's been 5 minutes since they did any damage
			damageDealt.Remove( i, true );
	}
	// Update the healing stuff
	for( i = damageHealed.First(); !damageHealed.Finished(); i = damageHealed.Next() )
	{
		if( i == NULL )
		{
			damageHealed.Remove( i );
			continue;
		}
		if( (i->lastDamageDone + 300000) < currentTime )	// if it's been 5 minutes since they did any damage
			damageHealed.Remove( i, true );
	}
}

}
