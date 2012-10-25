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
#include "combat.h"

namespace UOX
{

// Bitmask bits

const UI32 BIT_GM				=	0;
const UI32 BIT_BROADCAST		=	1;
const UI32 BIT_INVULNERABLE		=	2;
const UI32 BIT_SINGCLICKSER		=	3;
const UI32 BIT_SKILLTITLES		=	4;
const UI32 BIT_GMPAGEABLE		=	5;
const UI32 BIT_SNOOP			=	6;
const UI32 BIT_COUNSELOR		=	7;
const UI32 BIT_ALLMOVE			=	8;
const UI32 BIT_FROZEN			=	9;
const UI32 BIT_VIEWHOUSEASICON	=	10;
const UI32 BIT_NONEEDMANA		=	11;
const UI32 BIT_DISPELLABLE		=	12;
const UI32 BIT_PERMREFLECTED	=	13;
const UI32 BIT_NONEEDREAGS		=	14;


const UI32 BIT_UNICODE			=	1;
const UI32 BIT_NPC				=	2;
const UI32 BIT_SHOP				=	3;
const UI32 BIT_DEAD				=	4;
const UI32 BIT_ATWAR			=	5;
const UI32 BIT_ATTACKFIRST		=	6;
const UI32 BIT_ONHORSE			=	7;
const UI32 BIT_TOWNTITLE		=	8;
const UI32 BIT_REACTIVEARMOUR	=	9;
const UI32 BIT_TRAIN			=	10;
const UI32 BIT_GUILDTOGGLE		=	11;
const UI32 BIT_TAMED			=	12;
const UI32 BIT_GUARDED			=	13;
const UI32 BIT_RUN				=	14;
const UI32 BIT_POLYMORPHED		=	15;
const UI32 BIT_INCOGNITO		=	16;
const UI32 BIT_USINGPOTION		=	17;
const UI32 BIT_MAYLEVITATE		=	18;
const UI32 BIT_WILLHUNGER		=	19;
const UI32 BIT_MEDITATING		=	20;
const UI32 BIT_CASTING			=	21;
const UI32 BIT_JSCASTING		=	22;
const UI32 BIT_MAXHPFIXED		=	23;
const UI32 BIT_MAXMANAFIXED		=	24;
const UI32 BIT_MAXSTAMFIXED		=	25;
const UI32 BIT_CANATTACK		=	26;
const UI32 BIT_INBUILDING		=	27;
const UI32 BIT_INPARTY			=	28;	// This property is not saved
const UI32 BIT_EVADE			=	29; // This property is not saved

const UI32 BIT_MOUNTED			=	0;
const UI32 BIT_STABLED			=	1;

// Player defaults

const SI16			DEFPLAYER_CALLNUM 			= -1;
const SI16			DEFPLAYER_PLAYERCALLNUM		= -1;
const SERIAL		DEFPLAYER_TRACKINGTARGET 	= INVALIDSERIAL;
const UI08			DEFPLAYER_SQUELCHED			= 0;
const UI08			DEFPLAYER_COMMANDLEVEL 		= CL_PLAYER;
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
const SERIAL		DEFPLAYER_TOWNVOTE 			= INVALIDSERIAL;
const SI08			DEFPLAYER_TOWNPRIV 			= 0;

CChar::PlayerValues_st::PlayerValues_st() : callNum( DEFPLAYER_CALLNUM ), playerCallNum( DEFPLAYER_PLAYERCALLNUM ), trackingTarget( DEFPLAYER_TRACKINGTARGET ),
squelched( DEFPLAYER_SQUELCHED ), commandLevel( DEFPLAYER_COMMANDLEVEL ), postType( DEFPLAYER_POSTTYPE ), hairStyle( DEFPLAYER_HAIRSTYLE ), beardStyle( DEFPLAYER_BEARDSTYLE ),
hairColour( DEFPLAYER_HAIRCOLOUR ), beardColour( DEFPLAYER_BEARDCOLOUR ), speechItem( NULL ), speechMode( DEFPLAYER_SPEECHMODE ), speechID( DEFPLAYER_SPEECHID ),
speechCallback( NULL ), robe( DEFPLAYER_ROBE ), accountNum( DEFPLAYER_ACCOUNTNUM ), origSkin( DEFPLAYER_ORIGSKIN ), origID( DEFPLAYER_ORIGID ), 
fixedLight( DEFPLAYER_FIXEDLIGHT ), deaths( DEFPLAYER_DEATHS ), socket( NULL ), townvote( DEFPLAYER_TOWNVOTE ), townpriv( DEFPLAYER_TOWNPRIV )
{
	//memset( &lockState[0],		0, sizeof( UI08 )		* (INTELLECT+1) );
	// Changed to the following, as only the 15?16? first lockStates would get initialized or whanot (Xuri)
	memset( &lockState[0],		0, sizeof( lockState ));

	for( UI08 j = 0; j <= INTELLECT; ++j )
		atrophy[j] = j;

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
const UI16			DEFNPC_TAMEDHUNGERRATE		= 0;
const UI08			DEFNPC_HUNGERWILDCHANCE		= 0;
const R32			DEFNPC_MOVEMENTSPEED		= -1;
const SI08			DEFNPC_PATHFAIL				= -1;

CChar::NPCValues_st::NPCValues_st() : wanderMode( DEFNPC_WANDER ), oldWanderMode( DEFNPC_OLDWANDER ), fTarg( DEFNPC_FTARG ), fz( DEFNPC_FZ1 ),
aiType( DEFNPC_AITYPE ), spellAttack( DEFNPC_SPATTACK ), spellDelay( DEFNPC_SPADELAY ), taming( DEFNPC_TAMING ), fleeAt( DEFNPC_FLEEAT ),
reAttackAt( DEFNPC_REATTACKAT ), splitNum( DEFNPC_SPLIT ), splitChance( DEFNPC_SPLITCHANCE ), trainingPlayerIn( DEFNPC_TRAININGPLAYERIN ),
goldOnHand( DEFNPC_HOLDG ), questType( DEFNPC_QUESTTYPE ), questDestRegion( DEFNPC_QUESTDESTREGION ), questOrigRegion( DEFNPC_QUESTORIGREGION ),
petGuarding( NULL ), npcFlag( DEFNPC_NPCFLAG ), boolFlags( DEFNPC_BOOLFLAG ), peaceing( DEFNPC_PEACEING ), provoing( DEFNPC_PROVOING ), 
tamedHungerRate( DEFNPC_TAMEDHUNGERRATE ), hungerWildChance( DEFNPC_HUNGERWILDCHANCE ), walkingSpeed( DEFNPC_MOVEMENTSPEED ),
runningSpeed( DEFNPC_MOVEMENTSPEED ), fleeingSpeed( DEFNPC_MOVEMENTSPEED ), pathFail( DEFNPC_PATHFAIL )
{
	fx[0] = fx[1] = fy[0] = fy[1] = DEFNPC_WANDERAREA;
	petFriends.resize( 0 );
	foodList.reserve( MAX_NAME );
}

const UI32			DEFCHAR_BOOLS 				= 0;
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
const UI16			DEFCHAR_REGIONNUM 			= 255;
const UI16			DEFCHAR_TOWN 				= 0;
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

CChar::CChar() : CBaseObject(),
bools( DEFCHAR_BOOLS ), 
fonttype( DEFCHAR_FONTTYPE ), maxHP( DEFCHAR_MAXHP ), maxHP_oldstr( DEFCHAR_MAXHP_OLDSTR ), 
oldRace( DEFCHAR_OLDRACE ), maxMana( DEFCHAR_MAXMANA ), maxMana_oldint( DEFCHAR_MAXMANA_OLDINT ),
maxStam( DEFCHAR_MAXSTAM ), maxStam_olddex( DEFCHAR_MAXSTAM_OLDDEX ), saycolor( DEFCHAR_SAYCOLOUR ), 
emotecolor( DEFCHAR_EMOTECOLOUR ), cell( DEFCHAR_CELL ), packitem( NULL ), 
targ( DEFCHAR_TARG ), attacker( DEFCHAR_ATTACKER ), hunger( DEFCHAR_HUNGER ), regionNum( DEFCHAR_REGIONNUM ), town( DEFCHAR_TOWN ), 
advobj( DEFCHAR_ADVOBJ ), guildfealty( DEFCHAR_GUILDFEALTY ), guildnumber( DEFCHAR_GUILDNUMBER ), flag( DEFCHAR_FLAG ), 
spellCast( DEFCHAR_SPELLCAST ), nextact( DEFCHAR_NEXTACTION ), stealth( DEFCHAR_STEALTH ), running( DEFCHAR_RUNNING ), 
raceGate( DEFCHAR_RACEGATE ), step( DEFCHAR_STEP ), priv( DEFCHAR_PRIV ), PoisonStrength( DEFCHAR_POISONSTRENGTH )
{
	ownedItems.clear();
	itemLayers.clear();
	layerCtr = itemLayers.end();

	id		= 0x0190;
	objType = OT_CHAR;
	name	= "Mr. noname";

	memset( &regen[0],			0, sizeof( TIMERVAL )	* 3 );
	memset( &weathDamage[0],	0, sizeof( TIMERVAL )	* WEATHNUM );
	memset( &charTimers[0],		0, sizeof( TIMERVAL )	* tCHAR_COUNT );
	memset( &baseskill[0],		0, sizeof( SKILLVAL )	* ALLSKILLS );
	memset( &skill[0],			0, sizeof( SKILLVAL )	* (INTELLECT+1) );

	SetCanTrain( true );

	SetHungerStatus( true );

	skillUsed[0].reset();
	skillUsed[1].reset();
	updateTypes.reset();

	strength = dexterity = intelligence = 1;

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
//|   Function    -  SI08 PathFail()
//|   Date        -  Unknown
//|   Programmer  -  Xuri
//o---------------------------------------------------------------------------o
//|   Purpose     -  Number of times Pathfinding has failed for an NPC - resets on success
//o---------------------------------------------------------------------------o
SI08 CChar::GetPathFail( void ) const
{
	SI08 rVal = DEFNPC_PATHFAIL;
		rVal = mNPC->pathFail;
	return rVal;
}
void CChar::SetPathFail( SI08 newValue )
{
	if( IsValidNPC() )
		mNPC->pathFail = newValue;
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
bool CChar::SetHunger( SI08 newValue )
{
	bool JSEventUsed = false;

	hunger = newValue;
	
	const UI16 HungerTrig = GetScriptTrigger();
	cScript *toExecute = JSMapping->GetScript( HungerTrig );
	if( toExecute != NULL )
		JSEventUsed = toExecute->OnHungerChange( (this), hunger );
	
	return JSEventUsed;
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
	if ( !IsDead() && !IsInvulnerable() )	// No need to do anything on dead or invulnerable chars
	{
		UI16 hungerRate;
		SI16 hungerDamage;
		if( !IsNpc() && mSock != NULL )	// Do Hunger for player chars
		{
			if( WillHunger() && GetCommandLevel() == CL_PLAYER  )
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
						bool doHungerMessage = !DecHunger();
						if( doHungerMessage )
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
						DecHunger();
					else if( GetHP() > 0 && hungerDamage > 0)
						Damage( hungerDamage );
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
						DecHunger();
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
//|   Purpose     -  Check if the owner of the was offline for to long and
//|					 remove him if so.
//o---------------------------------------------------------------------------o
void CChar::checkPetOfflineTimeout( void )
{
	if( IsTamed() && IsNpc() && GetNPCAiType() != AI_PLAYERVENDOR )
	{
		if( GetMounted() || GetStabled() )
			return;

		CChar *owner = GetOwnerObj();
		if( !ValidateObject( owner ) )
			SetOwner( NULL ); // The owner is gone, so remove him
		else
		{
			if( isOnline( (*owner) ) )
				return; // The owner is still online, so leave it alone

			time_t currTime, lastOnTime;
			const UI32 offlineTimeout = static_cast<UI32>(cwmWorldState->ServerData()->PetOfflineTimeout() * 3600 * 24);

			time( &currTime );
			lastOnTime = static_cast<time_t>(GetLastOnSecs());
			
			if( currTime > 0 && lastOnTime > 0)
			{
				if( difftime( currTime, lastOnTime) >= offlineTimeout )
				{
					SetOwner( NULL );
					SetHunger( 6 );
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 Town()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  The town the character belongs to
//o---------------------------------------------------------------------------o
UI16 CChar::GetTown( void ) const
{
	return town;
}
void CChar::SetTown( UI16 newValue )
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
		return static_cast<UI08>( GetBaseSkill( PEACEMAKING ) / 10);
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
	return bools.test( BIT_UNICODE );
}
void CChar::setUnicode( bool newVal )
{
	bools.set( BIT_UNICODE, newVal );
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
	return bools.test( BIT_NPC );
}
void CChar::SetNpc( bool newVal )
{
	bools.set( BIT_NPC, newVal );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsEvading( void ) const
//|   Date        -  04/02/2012
//|   Programmer  -  Xuri
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is evading
//o---------------------------------------------------------------------------o
bool CChar::IsEvading( void ) const
{
	return bools.test( BIT_EVADE );
}
void CChar::SetEvadeState( bool newVal )
{
	bools.set( BIT_EVADE, newVal );
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
	return bools.test( BIT_SHOP );
}
void CChar::SetShop( bool newVal )
{
	bools.set( BIT_SHOP, newVal );
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
	return bools.test( BIT_DEAD );
}
void CChar::SetDead( bool newValue )
{
	bools.set( BIT_DEAD, newValue );

	if( !IsNpc() )
	{
		if( GetVisible() != VT_VISIBLE && newValue == false )
			SetVisible( VT_VISIBLE );
		else if( newValue == true )
			SetVisible( VT_GHOSTHIDDEN );
	}
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
	return bools.test( BIT_CANATTACK );
}
void CChar::SetCanAttack( bool newValue )
{
	bools.set( BIT_CANATTACK, newValue );
	SetBrkPeaceChance( 0 );
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
	return bools.test( BIT_ATWAR );
}
void CChar::SetWar( bool newValue )
{
	bools.set( BIT_ATWAR, newValue );

	if( !IsNpc() )
	{
		if( IsDead() && newValue == true )
			SetVisible( VT_VISIBLE );
		else if( IsDead() && newValue == false )
			SetVisible( VT_GHOSTHIDDEN );
	}
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
	return bools.test( BIT_ATTACKFIRST );
}
void CChar::SetAttackFirst( bool newValue )
{
	bools.set( BIT_ATTACKFIRST, newValue );
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
	return bools.test( BIT_ONHORSE );
}
void CChar::SetOnHorse( bool newValue )
{
	bools.set( BIT_ONHORSE, newValue );
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
	return bools.test( BIT_TOWNTITLE );
}
void CChar::SetTownTitle( bool newValue )
{
	bools.set( BIT_TOWNTITLE, newValue );
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
	return bools.test( BIT_REACTIVEARMOUR );
}
void CChar::SetReactiveArmour( bool newValue )
{
	bools.set( BIT_REACTIVEARMOUR, newValue );
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
	return bools.test( BIT_TRAIN );
}
void CChar::SetCanTrain( bool newValue )
{
	bools.set( BIT_TRAIN, newValue );
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
	return bools.test( BIT_GUILDTOGGLE );
}
void CChar::SetGuildToggle( bool newValue )
{
	bools.set( BIT_GUILDTOGGLE, newValue );
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
	return bools.test( BIT_TAMED );
}
void CChar::SetTamed( bool newValue )
{
	bools.set( BIT_TAMED, newValue );
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
	return bools.test( BIT_GUARDED );
}
void CChar::SetGuarded( bool newValue )
{
	bools.set( BIT_GUARDED, newValue );
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
	return bools.test( BIT_RUN );
}
void CChar::SetRun( bool newValue )
{
	bools.set( BIT_RUN, newValue );
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
	return bools.test( BIT_POLYMORPHED );
}
void CChar::IsPolymorphed( bool newValue )
{
	bools.set( BIT_POLYMORPHED, newValue );
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
	return bools.test( BIT_INCOGNITO );
}
void CChar::IsIncognito( bool newValue )
{
	bools.set( BIT_INCOGNITO, newValue );
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
	return bools.test( BIT_USINGPOTION );
}
void CChar::SetUsingPotion( bool newVal )
{
	bools.set( BIT_USINGPOTION, newVal );
}

bool CChar::MayLevitate( void ) const
{
	return bools.test( BIT_MAYLEVITATE );
}
void CChar::SetLevitate( bool newValue )
{
	bools.set( BIT_MAYLEVITATE, newValue );
}

bool CChar::WillHunger( void ) const
{
	return bools.test( BIT_WILLHUNGER );
}
void CChar::SetHungerStatus( bool newValue )
{
	bools.set( BIT_WILLHUNGER, newValue );
}

bool CChar::IsMeditating( void ) const
{
	return bools.test( BIT_MEDITATING );
}
void CChar::SetMeditating( bool newValue )
{
	bools.set( BIT_MEDITATING, newValue );
}

bool CChar::IsCasting( void ) const
{
	return bools.test( BIT_CASTING );
}
void CChar::SetCasting( bool newValue )
{
	bools.set( BIT_CASTING, newValue );
}

bool CChar::IsJSCasting( void ) const
{
	return bools.test( BIT_JSCASTING );
}
// This is only a temporary measure until ALL code is switched over to JS code
// As it stands, it'll try and auto-direct cast if you set casting and spell timeouts
void CChar::SetJSCasting( bool newValue )
{
	bools.set( BIT_JSCASTING, newValue );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool CChar::inBuilding( void )
//|	Programmer	-	grimson
//o---------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a building
//o---------------------------------------------------------------------------o
bool CChar::inBuilding( void )
{
	return bools.test( BIT_INBUILDING );
}
void CChar::SetInBuilding( bool newValue )
{
	bools.set( BIT_INBUILDING, newValue );
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
		SetAttackFirst( false );
		SetTimer( tCHAR_PEACETIMER, BuildTimeValue( newValue ) );
	}
	else
		SetCanAttack( true );
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
		SetNpcWander( WT_FREE );
	}
	else
	{
		newOwner->GetPetList()->Add( this );
		SetTamed( true );
	}
	UpdateFlag( this );
	Dirty( UT_UPDATE );
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
void CChar::SetRegion( UI16 newValue )
{
	regionNum = newValue;
}
UI16 CChar::GetRegionNum( void ) const
{
	return regionNum;
}

void CChar::SetLocation( const CBaseObject *toSet )
{
	if( toSet != NULL )
		SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber() );
}

void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway );
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
	if( IsNpc() )
		InitializeWanderArea( this, 10, 10 );
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
	return static_cast<UI16>(priv.to_ulong());
}

void CChar::SetPriv( UI16 newValue )
{
	priv = newValue;
}

SKILLVAL CChar::GetBaseSkill( UI08 skillToGet ) const
{
	SKILLVAL rVal = 0;
	if( skillToGet < ALLSKILLS )
		rVal = baseskill[skillToGet];
	return rVal;
}

SKILLVAL CChar::GetSkill( UI08 skillToGet ) const
{
	SKILLVAL rVal = 0;
	if( skillToGet <= INTELLECT )
	{
		rVal			= skill[skillToGet];
		SI32 modifier	= Races->DamageFromSkill( skillToGet, race );
		if( modifier != 0 )
		{
			SKILLVAL toAdd	= (SKILLVAL)( (R32)skill[skillToGet] * ( (R32)modifier / 1000 ) );		// percentage to add
			rVal			+= toAdd; // return the bonus
		}
	}
	return rVal;
}

void CChar::SetBaseSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	if( skillToSet < ALLSKILLS )
		baseskill[skillToSet] = newSkillValue;
}
void CChar::SetSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	if( skillToSet <= INTELLECT )
		skill[skillToSet] = newSkillValue;
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
	return priv.test( BIT_GM );
}
bool CChar::CanBroadcast( void ) const
{
	return priv.test( BIT_BROADCAST );
}
bool CChar::IsInvulnerable( void ) const
{
	return priv.test( BIT_INVULNERABLE );
}
bool CChar::GetSingClickSer( void ) const
{
	return priv.test( BIT_SINGCLICKSER );
}
bool CChar::NoSkillTitles( void ) const
{
	return priv.test( BIT_SKILLTITLES );
}
bool CChar::IsGMPageable( void ) const
{
	return priv.test( BIT_GMPAGEABLE );
}
bool CChar::CanSnoop( void ) const
{
	return priv.test( BIT_SNOOP );
}
bool CChar::IsCounselor( void ) const
{
	return priv.test( BIT_COUNSELOR );
}

bool CChar::AllMove( void ) const
{
	return priv.test( BIT_ALLMOVE );
}
bool CChar::IsFrozen( void ) const
{
	return priv.test( BIT_FROZEN );
}
bool CChar::ViewHouseAsIcon( void ) const
{
	return priv.test( BIT_VIEWHOUSEASICON );
}
// 0x0800 is free
bool CChar::NoNeedMana( void ) const
{
	return priv.test( BIT_NONEEDMANA );
}
bool CChar::IsDispellable( void ) const
{
	return priv.test( BIT_DISPELLABLE );
}
bool CChar::IsPermReflected( void ) const
{
	return priv.test( BIT_PERMREFLECTED );
}
bool CChar::NoNeedReags( void ) const
{
	return priv.test( BIT_NONEEDREAGS );
}

void CChar::SetGM( bool newValue )
{
	priv.set( BIT_GM, newValue );
}
void CChar::SetBroadcast( bool newValue )
{
	priv.set( BIT_BROADCAST, newValue );
}
void CChar::SetInvulnerable( bool newValue )
{
	priv.set( BIT_INVULNERABLE, newValue );
}
void CChar::SetSingClickSer( bool newValue )
{
	priv.set( BIT_SINGCLICKSER, newValue );
}
void CChar::SetSkillTitles( bool newValue )
{
	priv.set( BIT_SKILLTITLES, newValue );
}
void CChar::SetGMPageable( bool newValue )
{
	priv.set( BIT_GMPAGEABLE, newValue );
}
void CChar::SetSnoop( bool newValue )
{
	priv.set( BIT_SNOOP, newValue );
}
void CChar::SetCounselor( bool newValue )
{
	priv.set( BIT_COUNSELOR, newValue );
}

void CChar::SetAllMove( bool newValue )
{
	priv.set( BIT_ALLMOVE, newValue );
}
void CChar::SetFrozen( bool newValue )
{
	priv.set( BIT_FROZEN, newValue );
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	priv.set( BIT_VIEWHOUSEASICON, newValue );
}
void CChar::SetNoNeedMana( bool newValue )
{
	priv.set( BIT_NONEEDMANA, newValue );
}
void CChar::SetDispellable( bool newValue )
{
	priv.set( BIT_DISPELLABLE, newValue );
}
void CChar::SetPermReflected( bool newValue )
{
	priv.set( BIT_PERMREFLECTED, newValue );
}
void CChar::SetNoNeedReags( bool newValue )
{
	priv.set( BIT_NONEEDREAGS, newValue );
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
	
	target->SetHiDamage( hidamage );
	target->SetLoDamage( lodamage );

	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		target->SetBaseSkill( baseskill[i], i );
		target->SetSkill( skill[i], i );
	}

	target->SetCell( cell );
	target->SetPackItem( packitem );
	target->SetWeight( weight );
	target->SetResist( GetResist( PHYSICAL), PHYSICAL );
	target->SetTarg( GetTarg() );
	target->SetRegen( regen[0], 0 );
	target->SetRegen( regen[1], 1 );
	target->SetRegen( regen[2], 2 );
	target->SetAttacker( GetAttacker() );
	target->SetVisible( GetVisible() );

	for( int mTID = (int)tCHAR_TIMEOUT; mTID < (int)tCHAR_COUNT; ++mTID )
		target->SetTimer( (cC_TID)mTID, GetTimer( (cC_TID)mTID ) );
	target->SetHunger( hunger );
	target->SetBrkPeaceChance( GetBrkPeaceChance() );
	target->SetBrkPeaceChanceGain( GetBrkPeaceChanceGain() );
	target->SetRegion( regionNum );
	target->SetTown( town );
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
		target->SetTamedHungerRate( GetTamedHungerRate() );
		target->SetTamedHungerWildChance( GetTamedHungerWildChance() );
		target->SetFood( GetFood() );
		target->SetFleeAt( GetFleeAt() );
		target->SetReattackAt( GetReattackAt() );
		target->SetFTarg( GetFTarg() );
		target->SetFx( GetFx( 0 ), 0 );
		target->SetFx( GetFx( 1 ), 1 );
		target->SetFy( GetFy( 0 ), 0 );
		target->SetFy( GetFy( 1 ), 1 );
		target->SetFz( GetFz() );
		target->SetNpcWander( GetNpcWander() );
		target->SetOldNpcWander( GetOldNpcWander() );
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
		target->SetNPCFlag( GetNPCFlag() );
		target->SetWalkingSpeed( GetWalkingSpeed() );
		target->SetRunningSpeed( GetRunningSpeed() );
		target->SetFleeingSpeed( GetFleeingSpeed() );
	}
	if( IsValidPlayer() )
	{
		target->SetTownpriv( GetTownPriv() );
		target->SetTownVote( GetTownVote() );
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
		for( UI08 j = STRENGTH; j <= INTELLECT; ++j )
		{
			target->SetAtrophy( mPlayer->atrophy[j], j );
			target->SetSkillLock( mPlayer->lockState[j], j );
		}
	}
}

FlagColors CChar::FlagColour( CChar *toCompare )
{
	FlagColors retVal = FC_INNOCENT;

	GUILDRELATION gComp	= GR_UNKNOWN;
	RaceRelate rComp	= RACE_NEUTRAL;

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
	else if( rComp != RACE_NEUTRAL || gComp != GR_UNKNOWN )
	{
		if( gComp == GR_ALLY || gComp == GR_SAME || rComp >= RACE_ALLY )
			retVal = FC_FRIEND;
		else if( gComp == GR_WAR || rComp <= RACE_ENEMY )
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
		else if( GetVisible() == VT_GHOSTHIDDEN && !mCharObj->IsDead() && GetCommandLevel() >= mCharObj->GetCommandLevel() )
			return;
		else if( ( ( GetVisible() != VT_VISIBLE && GetVisible() != VT_GHOSTHIDDEN ) || ( !IsNpc() && !isOnline( (*this) ) ) ) && GetCommandLevel() >= mCharObj->GetCommandLevel() )
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
					if( tempItem->CanBeObjType( OT_MULTI ) && objInRange( this, tempItem, DIST_BUILDRANGE ) )
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
	{
		if( GetMultiObj()->CanBeObjType( OT_BOAT ) ) //Don't force a weather update while on boat to prevent spam.
			forceWeatherupdate = false;
	}

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
			Console.Warning( "Failed to equip item %s(0x%X) to layer 0x%X on character %s(0x%X)", toWear->GetName().c_str(), toWear->GetSerial(), tLayer, GetName().c_str(), serial );
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

bool CChar::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[CHARACTER]" << '\n';
	return true;
}

bool CChar::DumpBody( std::ofstream &outStream ) const
{
	CBaseObject::DumpBody( outStream );	// Make the default save of BaseObject members now

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "GuildFealty=" << "0x" << GetGuildFealty() << '\n';  
	outStream << "Speech=" << "0x" << GetSayColour() << ",0x" << GetEmoteColour() << '\n';
	outStream << "Privileges=" << "0x" << GetPriv() << '\n';
	if( ValidateObject( packitem ) )
		outStream << "PackItem=" << "0x" << packitem->GetSerial() << '\n';	// store something meaningful
	else
		outStream << "PackItem=" << "0x" << INVALIDSERIAL << '\n';

	// Decimal / String Values
	outStream << std::dec;
	outStream << "GuildTitle=" << GetGuildTitle() << '\n';  
	outStream << "Hunger=" << (SI16)GetHunger() << '\n';
	outStream << "BrkPeaceChanceGain=" << (SI16)GetBrkPeaceChanceGain() << '\n';
	outStream << "BrkPeaceChance=" << (SI16)GetBrkPeaceChance() << '\n';
	if ( GetMaxHPFixed() )
		outStream << "MAXHP=" << (SI16)maxHP << '\n';
	if ( GetMaxManaFixed() )
		outStream << "MAXMANA=" << (SI16)maxMana << '\n';
	if ( GetMaxStamFixed() )
		outStream << "MAXSTAM=" << (SI16)maxStam << '\n';
	outStream << "Town=" << (SI16)GetTown() << '\n';
	outStream << "SummonTimer=" << GetTimer( tNPC_SUMMONTIME ) << '\n';
	outStream << "MayLevitate=" << (MayLevitate()?1:0) << '\n';
	outStream << "Stealth=" << (SI16)GetStealth() << '\n';
	outStream << "Reserved=" << (SI16)GetCell() << '\n';
	outStream << "Region=" << (SI16)GetRegionNum() << '\n';
	outStream << "AdvanceObject=" << GetAdvObj() << '\n';
	outStream << "AdvRaceObject=" << GetRaceGate() << '\n';

	// Write out the BaseSkills and the SkillLocks here
	// Format: BaseSkills=[0,34]-[1,255]-[END]
	outStream << "BaseSkills=";
	for( UI08 bsc = 0; bsc < ALLSKILLS; ++bsc )
		outStream << "[" << (SI32)bsc << "," << GetBaseSkill( bsc ) << "]-";
	outStream << "[END]" << '\n';

	outStream << "GuildNumber=" << GetGuildNumber() << '\n';
	outStream << "FontType=" << (SI16)GetFontType() << '\n';
	outStream << "TownTitle=" << (SI16)(GetTownTitle()?1:0) << '\n';
	//-------------------------------------------------------------------------------------------
	outStream << "CanRun=" << (SI16)((CanRun() && IsNpc())?1:0) << '\n';
	outStream << "CanAttack=" << (SI16)(GetCanAttack()?1:0) << '\n';
	outStream << "AllMove=" << (SI16)(AllMove()?1:0) << '\n';
	outStream << "IsNpc=" << (SI16)(IsNpc()?1:0) << '\n';
	outStream << "IsShop=" << (SI16)(IsShop()?1:0) << '\n';
	outStream << "Dead=" << (SI16)(IsDead()?1:0) << '\n';
	outStream << "CanTrain=" << (SI16)(CanTrain()?1:0) << '\n';
	outStream << "IsWarring=" << (SI16)(IsAtWar()?1:0) << '\n';
	outStream << "GuildToggle=" << (SI16)(GetGuildToggle()?1:0) << '\n';
	outStream << "PoisonStrength=" << (UI16)(GetPoisonStrength()) << '\n';
	outStream << "WillHunger=" << (SI16)(WillHunger()?1:0) << '\n';

	TIMERVAL mTime = GetTimer( tCHAR_MURDERRATE );
	outStream << "MurderTimer=";
	if( mTime == 0 || mTime < cwmWorldState->GetUICurrentTime() )
		outStream << (SI16)0 << '\n';
	else
		outStream << (UI32)(mTime - cwmWorldState->GetUICurrentTime()) << '\n';

	TIMERVAL pTime = GetTimer( tCHAR_PEACETIMER );
	outStream << "PeaceTimer=";
	if( pTime == 0 || pTime < cwmWorldState->GetUICurrentTime() )
		outStream << (SI16)0 << '\n';
	else
		outStream << (UI32)(pTime - cwmWorldState->GetUICurrentTime()) << '\n';

	if( IsValidPlayer() )
		mPlayer->DumpBody( outStream );
	if( IsValidNPC() )
		mNPC->DumpBody( outStream );
	return true;
}

void CChar::NPCValues_st::DumpBody( std::ofstream& outStream )
{
	// Hexadecimal Values
	outStream << std::hex;

	// Decimal / String Values
	outStream << std::dec;
	outStream << "NpcAIType=" << aiType << '\n';
	outStream << "Taming=" << taming << '\n';
	outStream << "Peaceing=" << peaceing << '\n';
	outStream << "Provoing=" << provoing << '\n';
	outStream << "HoldG=" << goldOnHand << '\n';
	outStream << "Split=" << (SI16)splitNum << "," << (SI16)splitChance << '\n';
	outStream << "WanderArea=" << fx[0] << "," << fy[0] << "," << fx[1] << "," << fy[1] << "," << (SI16)fz << '\n';
	outStream << "NpcWander=" << (SI16)wanderMode << "," << (SI16)oldWanderMode << '\n';
	outStream << "SPAttack=" << spellAttack << "," << (SI16)spellDelay << '\n';
	outStream << "QuestType=" << (SI16)questType << '\n';
	outStream << "QuestRegions=" << (SI16)questOrigRegion << "," << (SI16)questDestRegion << '\n';
	outStream << "FleeAt=" << fleeAt << '\n';
	outStream << "ReAttackAt=" << reAttackAt << '\n';
	outStream << "NPCFlag=" << (SI16)npcFlag << '\n';
	outStream << "Mounted=" << (SI16)(boolFlags.test( BIT_MOUNTED )?1:0) << '\n';
	outStream << "Stabled=" << (SI16)(boolFlags.test( BIT_STABLED )?1:0) << '\n';
	outStream << "TamedHungerRate=" << tamedHungerRate << '\n';
	outStream << "TamedHungerWildChance=" << (SI16)hungerWildChance << '\n';
	outStream << "Foodlist=" << foodList << '\n';
	outStream << "WalkingSpeed=" << walkingSpeed << '\n';
	outStream << "RunningSpeed=" << runningSpeed << '\n';
	outStream << "FleeingSpeed=" << fleeingSpeed << '\n';
}

void CChar::PlayerValues_st::DumpBody( std::ofstream& outStream )
{
	// Hexadecimal Values
	outStream << std::hex;
	outStream << "RobeSerial=" << "0x" << robe << '\n';
	outStream << "OriginalID=" << "0x" << origID << ",0x" << origSkin << '\n';
	outStream << "Hair=" << "0x" << hairStyle << ",0x" << hairColour << '\n';
	outStream << "Beard=" << "0x" << beardStyle << ",0x" << beardColour << '\n';
	outStream << "TownVote=" << "0x" << townvote << '\n';

	// Decimal / String Values
	outStream << std::dec;
	outStream << "Account=" << accountNum << '\n';
	outStream << "LastOn=" << lastOn << '\n';
	outStream << "LastOnSecs=" << lastOnSecs << '\n';
	outStream << "OrgName=" << origName << '\n';
	outStream << "CommandLevel=" << (SI16)commandLevel << '\n';	// command level
	outStream << "Squelched=" << (SI16)squelched << '\n';
	outStream << "Deaths=" << deaths << '\n';
	outStream << "FixedLight=" << (SI16)fixedLight << '\n';
	outStream << "TownPrivileges=" << (SI16)townpriv << '\n';
	outStream << "Atrophy=";
	for( UI08 atc = 0; atc <= INTELLECT; ++atc )
	{
		outStream << (SI16)atrophy[atc] << "," ;
	}
	outStream << "[END]" << '\n';

	// Format: SkillLocks=[0,34]-[1,255]-[END]
	outStream << "SkillLocks=";
	for( UI08 slc = 0; slc <= INTELLECT; ++slc )
	{
		if( lockState[slc] <= 2 )
			outStream << "[" << (SI16)slc << "," << (SI16)lockState[slc] << "]-";
		else
			outStream << "[" << (SI16)slc << ",0]-";
	}
	outStream << "[END]" << '\n';
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
bool CChar::DecHunger( const SI08 amt )
{
	return SetHunger( (SI08)(GetHunger() - amt) );
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
		size_t numSections = 0;
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
					numSections = data.sectionCount( "," );
					for( UI08 aCtr = 0; aCtr < numSections; ++aCtr )
					{
						if( data.section( ",", aCtr, aCtr ).empty() )
							break;

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
					numSections = data.sectionCount( "-" );
					// Format: BaseSkills=[0,34]-[1,255]-[END]
					for( UI08 skillCtr = 0; skillCtr < numSections; ++skillCtr )
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
				break;
			case 'D':
				if( UTag == "DEATHS" )
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
				else if( UTag == "FLEEINGSPEED" )
				{
					SetFleeingSpeed( data.toFloat() );
					rvalue = true;
				}
				break;
			case 'G':
				if( UTag == "GUILDFEALTY" )
				{
					SetGuildFealty( data.toULong() );
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
					UpdateFlag( this );
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
				else if( UTag == "POISONSTRENGTH" )
				{
					SetPoisonStrength( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "PEACEING" )
				{
					SetPeaceing( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "PROVOING" )
				{
					SetProvoing( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "PEACETIMER" )
				{
					SetTimer( tCHAR_PEACETIMER, BuildTimeValue( data.toFloat() ) );
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
					SetRegion( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "REATTACKAT" )
				{
					SetReattackAt( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "RUNNINGSPEED" )
				{
					SetRunningSpeed( data.toFloat() );
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
					numSections = data.sectionCount( "-" );
					for( UI08 lockCtr = 0; lockCtr < numSections; ++lockCtr )
					{
						UString tempdata = data.section( "-", lockCtr, lockCtr ).stripWhiteSpace();
						if( tempdata.empty() )
							break;

						UString tempval = tempdata.section( ",", 1, 1 ).substr( 0, tempdata.section( ",", 1, 1 ).size() - 1 );
						UString tempnum = tempdata.section( ",", 0, 0 ).substr( 1 );
						SetSkillLock( (SkillLock)tempval.toUByte(), tempnum.toUByte() );
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
				else if( UTag == "TAMEDHUNGERRATE" )
				{
					SetTamedHungerRate( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "TAMEDHUNGERWILDCHANCE" )
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
				else if( UTag == "WALKINGSPEED" )
				{
					SetWalkingSpeed( data.toFloat() );
					rvalue = true;
				}
				break;
			case 'X':
				if( UTag == "XNPCWANDER" )
				{
					SetOldNpcWander( data.toByte() );
					rvalue = true;
				}
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
	if( GetID() > 0x4DF )
	{
		if( acct == AB_INVALID_ID )
		{
			Console.Warning( "NPC: %s with serial 0x%X with bugged body found, deleting", GetName().c_str(), GetSerial() );
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
				Console.Warning( "NPC: %s with serial 0x%X found outside valid world locations, deleting", GetName().c_str(), GetSerial() );
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
		rvalue			= skillUsed[part].test( offset );
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
		skillUsed[part].set( offset, value );
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
	SI32 maxWeight = GetStrength() * cwmWorldState->ServerData()->WeightPerStr() + 40;
	if( GetWeight() <= 0 || GetWeight() > MAX_WEIGHT || GetWeight() > maxWeight )
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
//|		Function    :	void TextMessage( CSocket *s, std::string toSay, SpeechType msgType, bool spamTimer )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Npc speech
//o---------------------------------------------------------------------------o
void CChar::TextMessage( CSocket *s, std::string toSay, SpeechType msgType, bool spamTimer )
{
	bool canSpeak = !spamTimer;
	if( !toSay.empty() )
	{
		if( spamTimer )
		{
			if( GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
			{
				SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 10 ) );
				canSpeak = true;
			}
		}
		if( canSpeak )
		{
			UI16 txtColor = 0x5A;
			if( msgType == EMOTE )
				txtColor = GetEmoteColour();
			else if( msgType == TALK )
			{
				if( GetNPCAiType() == AI_EVIL )
					txtColor = 0x0026;
				else
					txtColor = GetSayColour();
			}
			else if( msgType == SYSTEM )
				txtColor = 0x07FA;
			if( !txtColor || txtColor == 0x1700 )
				txtColor = 0x5A;

			SERIAL speakTo		= INVALIDSERIAL;
			SpeechTarget target	= SPTRG_PCNPC;
			if( s != NULL )
			{
				CChar *mChar	= s->CurrcharObj();
				speakTo			= mChar->GetSerial();
				target			= SPTRG_INDIVIDUAL;
			}

			CSpeechEntry& toAdd = SpeechSys->Add();
			toAdd.Font( (FontType)GetFontType() );
			toAdd.Speech( toSay );
			toAdd.Speaker( GetSerial() );
			toAdd.SpokenTo( speakTo );
			toAdd.Type( msgType );
			toAdd.At( cwmWorldState->GetUICurrentTime() );
			toAdd.TargType( target );
			toAdd.Colour( txtColor );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void TextMessage( CSocket *s, SI32 dictEntry, SpeechType msgType, bool spamTimer, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Npc speech
//o---------------------------------------------------------------------------o
void CChar::TextMessage( CSocket *s, SI32 dictEntry, SpeechType msgType, bool spamTimer, ... )
{
	UnicodeTypes dictLang = ZERO;
	if( s != NULL )
		dictLang = s->Language();

	std::string txt = Dictionary->GetEntry( dictEntry, dictLang );
	if( !txt.empty() )
	{
		va_list argptr;

		char msg[512];
		va_start( argptr, spamTimer );
		vsprintf( msg, txt.c_str(), argptr );
		va_end( argptr );
		TextMessage( s, msg, msgType, spamTimer );
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
	UI08 fallDistance = oldLocZ - z;

	if( fallDistance > MAX_Z_FALL )
	{
		bool JSEventUsed = false;
		const UI16 FallTrig = GetScriptTrigger();
		cScript *toExecute = JSMapping->GetScript( FallTrig );
		if( toExecute != NULL )
			JSEventUsed = toExecute->OnFall( (this), fallDistance );
	}
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
			CAccountBlock& mAcct = GetAccount();
			if( mAcct.wAccountIndex != AB_INVALID_ID )
			{
				for( UI08 actr = 0; actr < 7; ++actr )
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
	if( cwmWorldState->deletionQueue.count(this) == 0 )
	{
		++(cwmWorldState->deletionQueue[this]);
		Cleanup();
		SetDeleted( true );
		ShouldSave( false );
	}
}

// Player Only Functions

//o--------------------------------------------------------------------------o
//|	Function		-	CAccountBlock& Account()
//|	Date			-	1/14/2003 6:17:45 AM
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Sets and Returns the CAccountBlock associated with this player
//o--------------------------------------------------------------------------o
void CChar::SetAccount( CAccountBlock& actbAccount )
{
	if( !IsValidPlayer() )
	{
		if( actbAccount.wAccountIndex != DEFPLAYER_ACCOUNTNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->accountNum = actbAccount.wAccountIndex;
}
CAccountBlock& CChar::GetAccount( void ) 
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

UI08 CChar::GetAtrophy( UI08 skillToGet ) const
{
	UI08 rVal = 0;
	if( IsValidPlayer() && skillToGet <= INTELLECT )
		rVal = mPlayer->atrophy[skillToGet];
	return rVal;
}
void CChar::SetAtrophy( UI08 newValue, UI08 skillToSet )
{
	if( IsValidPlayer() && skillToSet <= INTELLECT )
		mPlayer->atrophy[skillToSet] = newValue;
}
SkillLock CChar::GetSkillLock( UI08 skillToGet ) const
{
	SkillLock rVal = SKILL_INCREASE;
	if( IsValidPlayer() && skillToGet <= INTELLECT )
		rVal = mPlayer->lockState[skillToGet];
	return rVal;
}
void CChar::SetSkillLock( SkillLock newValue, UI08 skillToSet )
{
	if( IsValidPlayer() && skillToSet <= INTELLECT )
		mPlayer->lockState[skillToSet] = newValue;
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
		rVal = mNPC->boolFlags.test( BIT_MOUNTED );

	return rVal;
}
void CChar::SetMounted( bool newValue )
{
	if( IsValidNPC() )
		mNPC->boolFlags.set( BIT_MOUNTED, newValue );
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
		rVal = mNPC->boolFlags.test( BIT_STABLED );
	return rVal;
}
void CChar::SetStabled( bool newValue )
{
	if( IsValidNPC() )
	{
		mNPC->boolFlags.set( BIT_STABLED, newValue );
		CChar *oldOwner = GetOwnerObj();
		if( ValidateObject( oldOwner ))
		{
			if( newValue == true )
			{
				oldOwner->GetPetList()->Remove( this );
			}
			else if( newValue == false )
			{
				oldOwner->GetPetList()->Add( this );
			}
		}
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
	return bools.test( BIT_MAXHPFIXED );
}
void CChar::SetMaxHPFixed( bool newValue )
{
	bools.set( BIT_MAXHPFIXED, newValue );
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
	return bools.test( BIT_MAXMANAFIXED );
}
void CChar::SetMaxManaFixed( bool newValue )
{
	bools.set( BIT_MAXMANAFIXED, newValue );
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
	return bools.test( BIT_MAXSTAMFIXED );
}
void CChar::SetMaxStamFixed( bool newValue )
{
	bools.set( BIT_MAXSTAMFIXED, newValue );
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
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_ORIGID )
			CreatePlayer();
	}
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
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_ORIGSKIN )
			CreatePlayer();
	}
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

//o---------------------------------------------------------------------------o
//|   Function    -  UI32 TownVote()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Serial of the player a character has voted to be mayor.
//o---------------------------------------------------------------------------o
UI32 CChar::GetTownVote( void ) const
{
	UI32 retVal = DEFPLAYER_TOWNVOTE;
	if( IsValidPlayer() )
		retVal = mPlayer->townvote;
	return retVal;
}
void CChar::SetTownVote( UI32 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_TOWNVOTE )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->townvote = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 TownPriv()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Town member privledges (1 = Resident, 2 = Mayor)
//o---------------------------------------------------------------------------o
SI08 CChar::GetTownPriv( void ) const
{
	SI08 retVal = DEFPLAYER_TOWNPRIV;
	if( IsValidPlayer() )
		retVal = mPlayer->townpriv;
	return retVal;
}
void CChar::SetTownpriv( SI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_TOWNPRIV )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->townpriv = newValue;
}

// NPC Only Functions

//o---------------------------------------------------------------------------o
//|   Function    -  UI16 TamedHungerRate
//|   Date        -  Unknown
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Rate at which a pet hungers
//o---------------------------------------------------------------------------o
UI16 CChar::GetTamedHungerRate( void ) const
{
	UI16 retVal = DEFNPC_TAMEDHUNGERRATE;
	if( IsValidNPC() )
		retVal = mNPC->tamedHungerRate;
	return retVal;
}
void CChar::SetTamedHungerRate( UI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_TAMEDHUNGERRATE != newValue )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->tamedHungerRate = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 TamedHungerWildChance
//|   Date        -  Unknown
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Chance for a hungry pet to go wild
//o---------------------------------------------------------------------------o
UI08 CChar::GetTamedHungerWildChance( void ) const
{
	UI08 retVal = DEFNPC_HUNGERWILDCHANCE;
	if( IsValidNPC() )
		retVal = mNPC->hungerWildChance;
	return retVal;
}
void CChar::SetTamedHungerWildChance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_HUNGERWILDCHANCE != newValue )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->hungerWildChance = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  std::string Food()
//|   Date        -  Unknown
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  FOODLIST entry for feeding tamed pets
//o---------------------------------------------------------------------------o
std::string CChar::GetFood( void ) const
{
	std::string retVal = "";
	if( IsValidNPC() )
		retVal = mNPC->foodList;
	return retVal;
}
void CChar::SetFood( std::string food )
{
	if( !IsValidNPC() )
	{
		if( !food.empty() )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->foodList = food.substr( 0, MAX_NAME - 1 );
}

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
		rVal = static_cast<SI16>( GetBaseSkill( PEACEMAKING ) );
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
		rVal = static_cast<SI16>( GetBaseSkill( PROVOCATION ) );
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

	// Make shure the wanderarea is properly initialized
	if( ( newValue == WT_BOX ) || ( newValue == WT_CIRCLE ) )
		if( ( mNPC->fx[0] == -1 ) || ( mNPC->fy[0] == -1 ) )
			InitializeWanderArea( this, 10, 10 );
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
			mNPC->pathToFollow.pop_front();
		}
	}
	return rVal;
}
void CChar::PushDirection( UI08 newDir, bool pushFront )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( pushFront )
		mNPC->pathToFollow.push_front( newDir );
	else
		mNPC->pathToFollow.push_back( newDir );
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

//o---------------------------------------------------------------------------o
//|   Function    -  R32 GetWalkingSpeed
//|   Date        -  Juni 9, 2007
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the NPC's walking speed
//o---------------------------------------------------------------------------o
R32 CChar::GetWalkingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCWalkingSpeed();

	if( IsValidNPC() )
		if( mNPC->walkingSpeed > 0 )
			retVal = mNPC->walkingSpeed;

	return retVal;
}
//o---------------------------------------------------------------------------o
//|   Function    -  SetWalkingSpeed( R32 newValue )
//|   Date        -  Juni 9, 2007
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the NPC's walking speed
//o---------------------------------------------------------------------------o
void CChar::SetWalkingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->walkingSpeed = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 GetRunningSpeed
//|   Date        -  Juni 9, 2007
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the NPC's running speed
//o---------------------------------------------------------------------------o
R32 CChar::GetRunningSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCRunningSpeed();

	if( IsValidNPC() )
		if( mNPC->runningSpeed > 0 )
			retVal = mNPC->runningSpeed;

	return retVal;
}
//o---------------------------------------------------------------------------o
//|   Function    -  SetRunningSpeed( R32 newValue )
//|   Date        -  Juni 9, 2007
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the NPC's running speed
//o---------------------------------------------------------------------------o
void CChar::SetRunningSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->runningSpeed = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 GetFleeingSpeed
//|   Date        -  Juni 9, 2007
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the NPC's fleeing speed
//o---------------------------------------------------------------------------o
R32 CChar::GetFleeingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCFleeingSpeed();

	if( IsValidNPC() )
		if( mNPC->fleeingSpeed > 0 )
			retVal = mNPC->fleeingSpeed;

	return retVal;
}
//o---------------------------------------------------------------------------o
//|   Function    -  SetFleeingSpeed( R32 newValue )
//|   Date        -  Juni 9, 2007
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the NPC's fleeing speed
//o---------------------------------------------------------------------------o
void CChar::SetFleeingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->fleeingSpeed = newValue;
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

void CChar::ReactOnDamage( WeatherType damageType, CChar *attacker )
{
	CSocket *mSock = GetSocket();

	if( ValidateObject( attacker ) )
	{
		// Let the target react upon damage
		attacker->SetAttackFirst( ( GetTarg() != attacker ) );
		if( !attacker->IsInvulnerable() && (!ValidateObject( GetTarg() ) || !objInRange( this, GetTarg(), DIST_INRANGE )) )
		{
			SetAttacker( attacker );
			SetAttackFirst( false );
			if( IsNpc() )
			{
				if( GetVisible() == VT_TEMPHIDDEN || attacker->GetVisible() == VT_INVISIBLE )
					ExposeToView();

				if( !IsAtWar() && !attacker->IsInvulnerable() && GetNPCAiType() != AI_DUMMY )
				{
					SetTarg( attacker );
					ToggleCombat();
					SetTimer( tNPC_MOVETIME, BuildTimeValue( GetWalkingSpeed() ) );
				}
			} else if( mSock != NULL )
				BreakConcentration( mSock );
		}
	}
}

void CChar::Damage( SI16 damageValue, CChar *attacker, bool doRepsys )
{
	UI16 dbScript		= GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( dbScript );
	if( toExecute != NULL )
		toExecute->OnDamage( this, attacker, damageValue );

	CSocket *mSock = GetSocket(), *attSock = NULL, *attOwnerSock = NULL;
	
	if( ValidateObject( attacker ) )
	{
		attSock = attacker->GetSocket();
		if( ValidateObject( attacker->GetOwnerObj() ) )
			attOwnerSock = attacker->GetOwnerObj()->GetSocket();
	}

	// Display damage numbers
	if( cwmWorldState->ServerData()->CombatDisplayDamageNumbers() )
	{
		CPDisplayDamage toDisplay( (*this), (UI16)damageValue );
		if( mSock != NULL )
			mSock->Send( &toDisplay );
		if( attSock != NULL && attSock != mSock )
			attSock->Send( &toDisplay );
		if( attOwnerSock != NULL )
			attOwnerSock->Send( &toDisplay );
	}

	// Apply the damage
	SetHP( hitpoints - damageValue );

	// Handle peace state
	if( !GetCanAttack() )
	{	
		const UI08 currentBreakChance = GetBrkPeaceChance();
		if( (UI08)RandomNum( 1, 100 ) <= currentBreakChance )
		{
			SetCanAttack( true );
			if( mSock != NULL )
				mSock->sysmessage( 1779 );
		}
		else
			SetBrkPeaceChance( currentBreakChance + GetBrkPeaceChanceGain() );
	}

	if( ValidateObject( attacker ) )
	{
		// Reputation system
		if( doRepsys )
		{
			if( WillResultInCriminal( attacker, this ) ) //REPSYS
			{
				criminal( attacker );
				bool regionGuarded = ( GetRegion()->IsGuarded() );
				if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && IsNpc() && GetNPCAiType() != AI_GUARD && cwmWorldState->creatures[this->GetID()].IsHuman() )
				{
					TextMessage( NULL, 335, TALK, true );
					callGuards( this, attacker );
				}
				if( ValidateObject( attacker->GetOwnerObj() ) )
					criminal( attacker->GetOwnerObj() );
			}
		}

		// Update Damage tracking
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

	if( GetHP() <= 0 )
		Die( attacker, doRepsys );
}

void CChar::Die( CChar *attacker, bool doRepsys )
{
	UI16 dbScript		= GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( dbScript );
	if( toExecute != NULL )
	{
		if( toExecute->OnDeathBlow( this, attacker ) == 1 ) // if it exists and we don't want hard code, return
			return;
	}
	
	if( ValidateObject( attacker ) )
	{
		if( this != attacker && doRepsys )	// can't gain fame and karma for suicide :>
		{
			CSocket *attSock = attacker->GetSocket();
			if( attacker->DidAttackFirst() && WillResultInCriminal( attacker, this ) )
			{
				attacker->SetKills( attacker->GetKills() + 1 );
				attacker->SetTimer( tCHAR_MURDERRATE, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_MURDERDECAY ) );
				if( !attacker->IsNpc() )
				{
					if( attSock != NULL )
					{
						attSock->sysmessage( 314, attacker->GetKills() );
						if( attacker->GetKills() == cwmWorldState->ServerData()->RepMaxKills() + 1 )
							attSock->sysmessage( 315 );
					}
				}
				UpdateFlag( attacker );
			}
			Karma( attacker, this, ( 0 - ( GetKarma() ) ) );
			Fame( attacker, GetFame() );
		}
		if( !attacker->IsNpc() && !IsNpc() )
			Console.Log( Dictionary->GetEntry( 1617 ).c_str(), "PvP.log", GetName().c_str(), attacker->GetName().c_str() );
		
		Combat->Kill( attacker, this );
	}
	else
		HandleDeath( this );
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

void CChar::SetWeight( SI32 newVal, bool doWeightUpdate )
{
	Dirty( UT_STATWINDOW );
	weight = newVal;
}

//o---------------------------------------------------------------------------o
//|		Function    :	void Dirty( void ) const
//|		Date        :	25 July, 2003
//|		Programmer  :	Maarc
//o---------------------------------------------------------------------------o
//|		Purpose     :	Forces the object onto the global refresh queue
//o---------------------------------------------------------------------------o
void CChar::Dirty( UpdateTypes updateType )
{
	if( isPostLoaded() )
	{
		updateTypes.set( updateType, true );
		CBaseObject::Dirty( updateType );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetUpdate()
//|   Date        -  10/31/2003
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if we have set a specific UpdateType
//o---------------------------------------------------------------------------o
bool CChar::GetUpdate( UpdateTypes updateType ) const
{
	return updateTypes.test( updateType );
}
//o---------------------------------------------------------------------------o
//|   Function    -  ClearUpdate()
//|   Date        -  3/20/2006
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Clears the UpdateType bitlist, used at the end of our refresh queue
//o---------------------------------------------------------------------------o
void CChar::ClearUpdate( void )
{
	updateTypes.reset();
}

//o---------------------------------------------------------------------------o
//|   Function    -  InParty( bool value )
//|   Date        -  21st September, 2006
//|   Programmer  -  Maarc
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets/clears whether the character is in a party or not
//o---------------------------------------------------------------------------o
void CChar::InParty( bool value )
{
	bools.set( BIT_INPARTY, value );
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool InParty()
//|   Date        -  21st September, 2006
//|   Programmer  -  Maarc
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is in a party or not
//o---------------------------------------------------------------------------o
bool CChar::InParty( void ) const
{
	return bools.test( BIT_INPARTY );
}

}
