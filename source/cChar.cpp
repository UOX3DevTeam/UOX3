//o-----------------------------------------------------------------------------------------------o
//|	File			-	cChar.cpp
//|	Date			-	29th March, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	New class written based upon old UOX char_st.  Number of old members removed
//|						and a number of members types modified as well
//|
//|	Version History -
//|
//|						1.0		 		29th March, 2000
//|						Initial implementation, all get/set mechanisms written up to
//|						the end of SetSayColour().  Bools reduced down to a single char
//|						with get/set mechanisms for setting/clearings bits
//|						No documentation currently done, all functions obvious so far
//|						Note:  individual byte setting within longs/chars need speeding up
//|						consider memcpy into correct word, note that this will be endian specific!
//|
//| 					1.1		 		2nd April, 2000
//|						Finished up the initial implementation on all classes. Fixed a minor bit shift error
//|						on some of the char variables, all working fine now
//|
//| 					1.2		 		25 July, 2000
//|						All Accessor funcs plus a few others are now flagged as const functions, meaning that
//|						those functions GUARENTEE never to alter data, at compile time
//|						Thoughts about CBaseObject and prelim plans made
//|
//| 					1.3		 		28 July, 2000
//|						Initial CBaseObject implementation put in.  CChar reworked to deal with only things it has to
//|						Proper constructor written
//|						Plans for CChar derived objects thought upon (notably CPC and CNPC)
//|
//|						1.4		 		27 September, 2005
//|						Added PlayerValues_st and NPCValues_st to allow saving wasted memory on unnecesarry variables
//|						Organized many functions to their respective areas and added documentation for them.
//|						Changed itemLayers to a map
//o-----------------------------------------------------------------------------------------------o
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
#include "StringUtility.hpp"
#include "cEffects.h"
#include <algorithm>

#define DEBUGMOVEMULTIPLIER 1.75

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
const UI32 BIT_FLYING			=	30; // This property is not saved
const UI32 BIT_WILLTHIRST		=	31;

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
hairColour( DEFPLAYER_HAIRCOLOUR ), beardColour( DEFPLAYER_BEARDCOLOUR ), speechItem( nullptr ), speechMode( DEFPLAYER_SPEECHMODE ), speechID( DEFPLAYER_SPEECHID ),
speechCallback( nullptr ), robe( DEFPLAYER_ROBE ), accountNum( DEFPLAYER_ACCOUNTNUM ), origSkin( DEFPLAYER_ORIGSKIN ), origID( DEFPLAYER_ORIGID ),
fixedLight( DEFPLAYER_FIXEDLIGHT ), deaths( DEFPLAYER_DEATHS ), socket( nullptr ), townvote( DEFPLAYER_TOWNVOTE ), townpriv( DEFPLAYER_TOWNPRIV )
{
	//memset( &lockState[0],		0, sizeof( UI08 )		* (INTELLECT+1) );
	// Changed to the following, as only the 15?16? first lockStates would get initialized or whanot
	memset( &lockState[0],		0, sizeof( lockState ));

	for( UI08 j = 0; j <= INTELLECT; ++j )
		atrophy[j] = j;

	if( cwmWorldState != nullptr )
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
const UI16			DEFNPC_TAMEDTHIRSTRATE      = 0;
const UI08			DEFNPC_HUNGERWILDCHANCE		= 0;
const UI08			DEFNPC_THIRSTWILDCHANCE     = 0;
const R32			DEFNPC_MOVEMENTSPEED		= -1;
const SI08			DEFNPC_PATHFAIL				= -1;

CChar::NPCValues_st::NPCValues_st() : wanderMode( DEFNPC_WANDER ), oldWanderMode( DEFNPC_OLDWANDER ), fTarg( DEFNPC_FTARG ), fz( DEFNPC_FZ1 ),
aiType( DEFNPC_AITYPE ), spellAttack( DEFNPC_SPATTACK ), spellDelay( DEFNPC_SPADELAY ), taming( DEFNPC_TAMING ), fleeAt( DEFNPC_FLEEAT ),
reAttackAt( DEFNPC_REATTACKAT ), splitNum( DEFNPC_SPLIT ), splitChance( DEFNPC_SPLITCHANCE ), trainingPlayerIn( DEFNPC_TRAININGPLAYERIN ),
goldOnHand( DEFNPC_HOLDG ), questType( DEFNPC_QUESTTYPE ), questDestRegion( DEFNPC_QUESTDESTREGION ), questOrigRegion( DEFNPC_QUESTORIGREGION ),
petGuarding( nullptr ), npcFlag( DEFNPC_NPCFLAG ), boolFlags( DEFNPC_BOOLFLAG ), peaceing( DEFNPC_PEACEING ), provoing( DEFNPC_PROVOING ),
tamedHungerRate( DEFNPC_TAMEDHUNGERRATE ), tamedThirstRate( DEFNPC_TAMEDTHIRSTRATE ), hungerWildChance( DEFNPC_HUNGERWILDCHANCE ), thirstWildChance( DEFNPC_THIRSTWILDCHANCE ), walkingSpeed( DEFNPC_MOVEMENTSPEED ),
runningSpeed( DEFNPC_MOVEMENTSPEED ), fleeingSpeed( DEFNPC_MOVEMENTSPEED ), pathFail( DEFNPC_PATHFAIL )
{
	fx[0] = fx[1] = fy[0] = fy[1] = DEFNPC_WANDERAREA;
	petFriends.resize( 0 );
	foodList.reserve( MAX_NAME );
}

const UI32			DEFCHAR_BOOLS 				= 0;
//const SI08			DEFCHAR_DISPZ 				= 0;
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
const SI08			DEFCHAR_THIRST              = 6;
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
//const UI16			DEFCHAR_NOMOVE 				= 0;
//const UI16			DEFCHAR_POISONCHANCE 		= 0;
const UI08			DEFCHAR_POISONSTRENGTH 		= 0;
const BodyType		DEFCHAR_BODYTYPE			= BT_OTHER;


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar constructor
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This function basically does what the name implies
//o-----------------------------------------------------------------------------------------------o
CChar::CChar() : CBaseObject(),
bools( DEFCHAR_BOOLS ),
fonttype( DEFCHAR_FONTTYPE ), maxHP( DEFCHAR_MAXHP ), maxHP_oldstr( DEFCHAR_MAXHP_OLDSTR ),
oldRace( DEFCHAR_OLDRACE ), maxMana( DEFCHAR_MAXMANA ), maxMana_oldint( DEFCHAR_MAXMANA_OLDINT ),
maxStam( DEFCHAR_MAXSTAM ), maxStam_olddex( DEFCHAR_MAXSTAM_OLDDEX ), saycolor( DEFCHAR_SAYCOLOUR ),
emotecolor( DEFCHAR_EMOTECOLOUR ), cell( DEFCHAR_CELL ), packitem( nullptr ),
targ( DEFCHAR_TARG ), attacker( DEFCHAR_ATTACKER ), hunger( DEFCHAR_HUNGER ), thirst( DEFCHAR_THIRST ), regionNum( DEFCHAR_REGIONNUM ), town( DEFCHAR_TOWN ),
advobj( DEFCHAR_ADVOBJ ), guildfealty( DEFCHAR_GUILDFEALTY ), guildnumber( DEFCHAR_GUILDNUMBER ), flag( DEFCHAR_FLAG ),
spellCast( DEFCHAR_SPELLCAST ), nextact( DEFCHAR_NEXTACTION ), stealth( DEFCHAR_STEALTH ), running( DEFCHAR_RUNNING ),
raceGate( DEFCHAR_RACEGATE ), step( DEFCHAR_STEP ), priv( DEFCHAR_PRIV ), PoisonStrength( DEFCHAR_POISONSTRENGTH ), bodyType( DEFCHAR_BODYTYPE )
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
	SetThirstStatus( true );

	skillUsed[0].reset();
	skillUsed[1].reset();
	updateTypes.reset();

	strength = dexterity = intelligence = 1;

	mPlayer	= nullptr;
	mNPC	= nullptr;

	SetMaxHPFixed( false );
	SetMaxManaFixed( false );
	SetMaxStamFixed( false );
	SetCanAttack( true );
	SetBrkPeaceChanceGain( 0 );
	SetBrkPeaceChance( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar deconstructor
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Destructor to clean things up when deleted
//o-----------------------------------------------------------------------------------------------o
CChar::~CChar()
{
	if( IsValidNPC() )
	{
		delete mNPC;
		mNPC = nullptr;
	}

	if( IsValidPlayer() )
	{
		delete mPlayer;
		mPlayer = nullptr;
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsValidNPC( void ) const
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks whether character is an NPC
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsValidNPC( void ) const
{
	return ( mNPC != nullptr );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsValidPlayer( void ) const
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks whether character is a Player
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsValidPlayer( void ) const
{
	return ( mPlayer != nullptr );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetPathFail( void ) const
//|					void SetPathFail( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set number of times Pathfinding has failed for an NPC - resets on success
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetPathResult( void ) const
//|					void SetPathResult( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the end result of pathfinding, to pass on to onPathfindEnd event
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetPathResult( void ) const
{
	SI08 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathResult;
	return rVal;
}
void CChar::SetPathResult( SI08 newValue )
{
	if( IsValidNPC() )
		mNPC->pathResult = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetPathTargX( void ) const
//|					void SetPathTargX( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the final target X location NPC is pathfinding towards
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetPathTargX( void ) const
{
	UI16 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathTargX;
	return rVal;
}
void CChar::SetPathTargX( UI16 newValue )
{
	if( IsValidNPC() )
		mNPC->pathTargX = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetPathTargY( void ) const
//|					void SetPathTargY( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the final target Y location NPC is pathfinding towards
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetPathTargY( void ) const
{
	UI16 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathTargY;
	return rVal;
}
void CChar::SetPathTargY( UI16 newValue )
{
	if( IsValidNPC() )
		mNPC->pathTargY = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetHunger( void ) const
//|					bool SetHunger( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set hunger level of the character
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetHunger( void ) const
{
	return hunger;
}
bool CChar::SetHunger( SI08 newValue )
{
	std::vector<UI16> scriptTriggers = GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( i );
		if( toExecute != nullptr )
		{
			// If script returns false/0/nothing, prevent hunger from changing, and prevent
			// other scripts with event from running
			if( toExecute->OnHungerChange( (this), hunger ) == 0 )
			{
				return false;
			}
		}
	}

	hunger = newValue;

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function    -	void DoHunger()
//|	Date        -	21. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate Hunger level of the character and do all related effects.
//o-----------------------------------------------------------------------------------------------o
void CChar::DoHunger( CSocket *mSock )
{
	// Don't continue if hunger system is disabled
	if( !cwmWorldState->ServerData()->HungerSystemEnabled() )
		return;

	if ( !IsDead() && !IsInvulnerable() )	// No need to do anything on dead or invulnerable chars
	{
		UI16 hungerRate;
		SI16 hungerDamage;
		if( !IsNpc() && mSock != nullptr )	// Do Hunger for player chars
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
						SetOwner( nullptr );
						SetHunger( 6 );
					}
					SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>(hungerRate) ) );
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   SI08 GetThirst( void ) const
//|                 bool SetThirst( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Get/Set Thirst level of the character
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetThirst( void ) const
{
	return thirst;
}

bool CChar::SetThirst( SI08 newValue )
{
	std::vector<UI16> scriptTriggers = GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript* toExecute = JSMapping->GetScript(i);
		if( toExecute != nullptr )
		{
			// If script returns false/0/nothing, prevent thirst from changing, and prevent
			// other scripts with event from running
			if( toExecute->OnThirstChange( ( this ), thirst ) == 0 )
			{
				return false;
			}
		}
	}

	thirst = newValue;

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   void DoThirst()
//| Date        -   6. June, 2021
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Calculate Thirst level of the character and do all related effects.
//o-----------------------------------------------------------------------------------------------o
void CChar::DoThirst( CSocket* mSock )
{
	// Don't continue if thirst system is disabled
	if( !cwmWorldState->ServerData()->ThirstSystemEnabled() )
		return;

	if( !IsDead() && !IsInvulnerable() ) // No need to do anything on dead or invulnerable chars
	{
		UI16 thirstRate;
		SI16 thirstDrain;
		if( !IsNpc() && mSock != nullptr )  // Do Thirst for player chars
		{
			if( WillThirst() && GetCommandLevel() == CL_PLAYER )
			{
				if( GetTimer( tCHAR_THIRST ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					if( Races->DoesThirst( GetRace() )) // prefer the thirst settings frome the race
					{
						thirstRate = Races->GetThirstRate( GetRace() );
						thirstDrain = Races->GetThirstDrain( GetRace() );
					}
					else // use the global values if there is no race setting
					{
						thirstRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_THIRSTRATE );
						thirstDrain = cwmWorldState->ServerData()->ThirstDrain();
					}

					if( GetThirst() > 0 )
					{
						bool doThirstMessage = !DecThirst();
						if( doThirstMessage )
						{
							switch( GetThirst() )
							{
								default:
								case 6:									break;
								case 5: mSock->sysmessage( 2045 );		break;
								case 4: mSock->sysmessage( 2046 );		break;
								case 3: mSock->sysmessage( 2047 );		break;
								case 2: mSock->sysmessage( 2048 );		break;
								case 1: mSock->sysmessage( 2049 );		break;
								case 0: mSock->sysmessage( 2050 );		break;
							}
						}
					}
					else if( GetStamina() > 1 && thirstDrain > 0 )
					{
						mSock->sysmessage( 2051 );
						SetStamina( std::max( static_cast<SI16>(1), static_cast<SI16>(GetStamina() - thirstDrain )));
						if( GetStamina() <= 1 )
						{
							mSock->sysmessage( 2052 );
						}
					}
					SetTimer( tCHAR_THIRST, BuildTimeValue( static_cast<R32>(thirstRate) ));
				}
			}
		}
		else if( IsNpc() && !IsTamed() && Races->DoesThirst( GetRace() ))
		{
			if( WillThirst() && !GetMounted() && !GetStabled() )
			{
				if( GetTimer( tCHAR_THIRST ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					thirstRate = Races->GetThirstRate( GetRace() );
					thirstDrain = Races->GetThirstDrain( GetRace() );

					if( GetThirst() > 0 )
					{
						DecThirst();
					}
					else if( GetStamina() > 1 && thirstDrain > 0 )
					{
						SetStamina( std::max( static_cast<SI16>( 1 ), static_cast<SI16>( GetStamina() - thirstDrain ) ) );
					}
					SetTimer( tCHAR_THIRST, BuildTimeValue( static_cast<R32>(thirstRate) ));
				}
			}
		}
		else if( IsTamed() && GetTamedThirstRate() > 0 )
		{
			if( WillThirst() && !GetMounted() && !GetStabled() )
			{
				if( cwmWorldState->ServerData()->PetHungerOffline() == false )
				{
					CChar* owner = GetOwnerObj();
					if( !ValidateObject( owner ))
						return;

					if( !isOnline((*owner)) )
						return;
				}

				if( GetTimer( tCHAR_THIRST ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					thirstRate = GetTamedThirstRate();

					if( GetThirst() > 0 )
						DecThirst();
					else if( (UI08)RandomNum( 0, 100 ) <= GetTamedThirstWildChance() )
					{
						SetOwner( nullptr );
						SetThirst( 6 );
					}
					SetTimer( tCHAR_THIRST, BuildTimeValue( static_cast<R32>(thirstRate) ));
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checkPetOfflineTimeout()
//|	Date		-	21. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the owner of the was offline for to long and remove him if so.
//o-----------------------------------------------------------------------------------------------o
void CChar::checkPetOfflineTimeout( void )
{
	if( IsTamed() && IsNpc() && GetNPCAiType() != AI_PLAYERVENDOR )
	{
		if( GetMounted() || GetStabled() )
			return;

		CChar *owner = GetOwnerObj();
		if( !ValidateObject( owner ) )
			SetOwner( nullptr ); // The owner is gone, so remove him
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
					SetOwner( nullptr );
					SetHunger( 6 );
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetTown( void ) const
//|					void SetTown( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	The town the character belongs to
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetTown( void ) const
{
	return town;
}
void CChar::SetTown( UI16 newValue )
{
	town = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	BodyType GetBodyType( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the body type (human, elf, gargoyle, other) of a character
//o-----------------------------------------------------------------------------------------------o
BodyType CChar::GetBodyType( void )
{
	BodyType retVal = BT_OTHER;
	switch( GetID() )
	{
		case 0x0190: // Male Human
		case 0x0191: // Female Human
		case 0x0192: // Male Human Ghost
		case 0x0193: // Female Human Ghost
		case 0x00B7: // Male Human Savage
		case 0x00B9: // Male Human Savage
		case 0x02EE: // Male Human Savage
		case 0x00B8: // Female Human Savage
		case 0x00BA: // Female Human Savage
		case 0x02EF: // Female Human Savage
			retVal = BT_HUMAN;
			break;
		case 0x025D: // Male Elf
		case 0x025E: // Female Elf
		case 0x025F: // Male Elf Ghost
		case 0x0260: // Female Elf Ghost
			retVal = BT_ELF;
			break;
		case 0x029A: // Male Gargoyle
		case 0x029B: // Female Gargoyle
		case 0x02B6: // Male Gargoyle Ghost
		case 0x02B7: // Female Gargoyle Ghost
			retVal = BT_GARGOYLE;
			break;
		default:
			retVal = BT_OTHER;
			break;
	}
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetBrkPeaceChanceGain( void ) const
//|					void SetBrkPeaceChanceGain( UI08 newValue )
//|	Date		-	25. Feb
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance gain to break peace
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetBrkPeaceChance( void ) const
//|					void SetBrkPeaceChance( UI08 newValue )
//|	Date		-	25. Feb
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current chance to break peace
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetBrkPeaceChance( void ) const
{
	return brkPeaceChance;
}
void CChar::SetBrkPeaceChance( UI08 newValue )
{
	brkPeaceChance = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isUnicode( void ) const
//|					void setUnicode( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is speaking in unicode
//o-----------------------------------------------------------------------------------------------o
bool CChar::isUnicode( void ) const
{
	return bools.test( BIT_UNICODE );
}
void CChar::setUnicode( bool newVal )
{
	bools.set( BIT_UNICODE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsNpc( void ) const
//|					void SetNpc( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is an npc
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsNpc( void ) const
{
	return bools.test( BIT_NPC );
}
void CChar::SetNpc( bool newVal )
{
	bools.set( BIT_NPC, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsEvading( void ) const
//|					void SetEvadeState( bool newVal )
//|	Date		-	04/02/2012
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is evading
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsEvading( void ) const
{
	return bools.test( BIT_EVADE );
}
void CChar::SetEvadeState( bool newVal )
{
	bools.set( BIT_EVADE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsShop( void ) const
//|					void SetShop( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is a shopkeeper
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsShop( void ) const
{
	return bools.test( BIT_SHOP );
}
void CChar::SetShop( bool newVal )
{
	bools.set( BIT_SHOP, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsDead( void ) const
//|					void SetDead( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is dead
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetCanAttack( void ) const
//|					void SetCanAttack( bool newValue )
//|	Date		-	25. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the char can attack targets
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetCanAttack( void ) const
{
	return bools.test( BIT_CANATTACK );
}
void CChar::SetCanAttack( bool newValue )
{
	bools.set( BIT_CANATTACK, newValue );
	SetBrkPeaceChance( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsAtWar( void ) const
//|					void SetWar( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is at war
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DidAttackFirst( void ) const
//|					void SetAttackFirst( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character attacked first
//o-----------------------------------------------------------------------------------------------o
bool CChar::DidAttackFirst( void ) const
{
	return bools.test( BIT_ATTACKFIRST );
}
void CChar::SetAttackFirst( bool newValue )
{
	bools.set( BIT_ATTACKFIRST, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOnHorse( void ) const
//|					void SetOnHorse( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is on a horse
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsOnHorse( void ) const
{
	return bools.test( BIT_ONHORSE );
}
void CChar::SetOnHorse( bool newValue )
{
	bools.set( BIT_ONHORSE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetTownTitle( void ) const
//|					void SetTownTitle( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the character's town info is displayed
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetTownTitle( void ) const
{
	return bools.test( BIT_TOWNTITLE );
}
void CChar::SetTownTitle( bool newValue )
{
	bools.set( BIT_TOWNTITLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetReactiveArmour( void ) const
//|					void SetReactiveArmour( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the character has reactive armour
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetReactiveArmour( void ) const
{
	return bools.test( BIT_REACTIVEARMOUR );
}
void CChar::SetReactiveArmour( bool newValue )
{
	bools.set( BIT_REACTIVEARMOUR, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanTrain( void ) const
//|					void SetCanTrain( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character can train
//o-----------------------------------------------------------------------------------------------o
bool CChar::CanTrain( void ) const
{
	return bools.test( BIT_TRAIN );
}
void CChar::SetCanTrain( bool newValue )
{
	bools.set( BIT_TRAIN, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool GetGuildToggle( void ) const
//|					void SetGuildToggle( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets whether the character displays guild information
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetGuildToggle( void ) const
{
	return bools.test( BIT_GUILDTOGGLE );
}
void CChar::SetGuildToggle( bool newValue )
{
	bools.set( BIT_GUILDTOGGLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsTamed( void ) const
//|					void SetTamed( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is tamed
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsTamed( void ) const
{
	return bools.test( BIT_TAMED );
}
void CChar::SetTamed( bool newValue )
{
	bools.set( BIT_TAMED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsGuarded( void ) const
//|					void SetGuarded( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is guarded
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsGuarded( void ) const
{
	return bools.test( BIT_GUARDED );
}
void CChar::SetGuarded( bool newValue )
{
	bools.set( BIT_GUARDED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool CanRun( void ) const
//|					void SetRun( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character can run
//o-----------------------------------------------------------------------------------------------o
bool CChar::CanRun( void ) const
{
	return bools.test( BIT_RUN );
}
void CChar::SetRun( bool newValue )
{
	bools.set( BIT_RUN, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsPolymorphed( void ) const
//|					void IsPolymorphed( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is polymorphed
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsPolymorphed( void ) const
{
	return bools.test( BIT_POLYMORPHED );
}
void CChar::IsPolymorphed( bool newValue )
{
	bools.set( BIT_POLYMORPHED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsIncognito( void ) const
//|					void IsIncognito( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is incognito
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsIncognito( void ) const
{
	return bools.test( BIT_INCOGNITO );
}
void CChar::IsIncognito( bool newValue )
{
	bools.set( BIT_INCOGNITO, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsUsingPotion( void ) const
//|					void SetUsingPotion( bool newVal )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is using a potion
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsUsingPotion( void ) const
{
	return bools.test( BIT_USINGPOTION );
}
void CChar::SetUsingPotion( bool newVal )
{
	bools.set( BIT_USINGPOTION, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool MayLevitate( void ) const
//|					void SetLevitate( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character can levitate - not yet in use
//o-----------------------------------------------------------------------------------------------o
bool CChar::MayLevitate( void ) const
{
	return bools.test( BIT_MAYLEVITATE );
}
void CChar::SetLevitate( bool newValue )
{
	bools.set( BIT_MAYLEVITATE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool WillHunger( void ) const
//|					void SetHungerStatus( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character will get hungry
//o-----------------------------------------------------------------------------------------------o
bool CChar::WillHunger( void ) const
{
	return bools.test( BIT_WILLHUNGER );
}
void CChar::SetHungerStatus( bool newValue )
{
	bools.set( BIT_WILLHUNGER, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool WillThirst( void ) const
//|                 void SetThirstStatus( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Returns/Sets whether the character will get thirsty
//o-----------------------------------------------------------------------------------------------o
bool CChar::WillThirst(void) const
{
	return bools.test( BIT_WILLTHIRST );
}
void CChar::SetThirstStatus( bool newValue )
{
	bools.set( BIT_WILLTHIRST, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsMeditating( void ) const
//|					void SetMeditating( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is meditating
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsMeditating( void ) const
{
	return bools.test( BIT_MEDITATING );
}
void CChar::SetMeditating( bool newValue )
{
	bools.set( BIT_MEDITATING, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsCasting( void ) const
//|					void SetCasting( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is casting a spell
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsCasting( void ) const
{
	return bools.test( BIT_CASTING );
}
void CChar::SetCasting( bool newValue )
{
	bools.set( BIT_CASTING, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsJSCasting( void ) const
//|					void SetJSCasting( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is casting a spell via JS engine
//|	Notes		-	This is only a temporary measure until ALL code is switched over to JS code
//|					As it stands, it'll try and auto-direct cast if you set casting and spell timeouts
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsJSCasting( void ) const
{
	return bools.test( BIT_JSCASTING );
}
void CChar::SetJSCasting( bool newValue )
{
	bools.set( BIT_JSCASTING, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	inBuilding( void )
//|					SetInBuilding( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a building
//o-----------------------------------------------------------------------------------------------o
bool CChar::inBuilding( void )
{
	return bools.test( BIT_INBUILDING );
}
void CChar::SetInBuilding( bool newValue )
{
	bools.set( BIT_INBUILDING, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetPeace( UI32 newValue )
//| Date		-	25.Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Takes character out of combat state
//o-----------------------------------------------------------------------------------------------o
void CChar::SetPeace( UI32 newValue )
{
	if( newValue > 0 )
	{
		SetCanAttack( false );
		if( IsAtWar() )
			SetWar( false );
		SetTarg( nullptr );
		SetAttacker( nullptr );
		SetAttackFirst( false );
		SetTimer( tCHAR_PEACETIMER, BuildTimeValue( newValue ) );
	}
	else
		SetCanAttack( true );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-  void RemoveSelfFromOwner( void )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-  Removes character from its owner's pet list
//o-----------------------------------------------------------------------------------------------o
void CChar::RemoveSelfFromOwner( void )
{
	CChar *oldOwner = GetOwnerObj();
	if( ValidateObject( oldOwner ) )
		oldOwner->GetPetList()->Remove( this );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void AddSelfToOwner( void )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Adds character to its new owner's pet list
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI32 GetGuildFealty( void ) const
//|					void SetGuildFealty( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a player's guild fealty - essentially, who the player
//|					votes for as the next guild master
//o-----------------------------------------------------------------------------------------------o
UI32 CChar::GetGuildFealty( void ) const
{
	return guildfealty;
}
void CChar::SetGuildFealty( UI32 newValue )
{
	guildfealty = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	std::string GetGuildTitle( void ) const
//|					void SetGuildTitle( const std::string &newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a player's guild title
//o-----------------------------------------------------------------------------------------------o
std::string CChar::GetGuildTitle( void ) const
{
	return guildtitle;
}
void CChar::SetGuildTitle( const std::string &newValue )
{
	guildtitle = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	TIMERVAL GetRegen( UI08 part ) const
//|					void SetRegen( TIMERVAL newValue, UI08 part )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets regeneration timers for player's health, mana and stamina
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CChar::GetRegen( UI08 part ) const
{
	return regen[part];
}
void CChar::SetRegen( TIMERVAL newValue, UI08 part )
{
	regen[part] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	TIMERVAL GetWeathDamage( UI08 part ) const
//|					void SetWeathDamage( TIMERVAL newValue, UI08 part )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timers for weather damage effects on character
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CChar::GetWeathDamage( UI08 part ) const
{
	return weathDamage[part];
}
void CChar::SetWeathDamage( TIMERVAL newValue, UI08 part )
{
	weathDamage[part] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetNextAct( void ) const
//|					void SetNextAct( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's next spellcasting action
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetNextAct( void ) const
{
	return nextact;
}
void CChar::SetNextAct( UI08 newVal )
{
	nextact = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetTimer( cC_TID timerID ) const
//|					void SetTimer( cC_TID timerID, TIMERVAL value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets timer values for the character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 PoisonStrength()
//|	Date		-	09/23/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the poison strength of the character - used to poison others in combat
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetPoisonStrength( void ) const
{
	return PoisonStrength;
}
void CChar::SetPoisonStrength( UI08 value )
{
	PoisonStrength = value;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	COLOUR GetEmoteColour( void ) const
//|					void SetEmoteColour( COLOUR newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's emote text colour
//o-----------------------------------------------------------------------------------------------o
COLOUR CChar::GetEmoteColour( void ) const
{
	return emotecolor;
}
void CChar::SetEmoteColour( COLOUR newValue )
{
	emotecolor = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	COLOUR GetSayColour( void ) const
//|					void SetSayColour( COLOUR newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's normal speech text colour
//o-----------------------------------------------------------------------------------------------o
COLOUR CChar::GetSayColour( void ) const
{
	return saycolor;
}
void CChar::SetSayColour( COLOUR newValue )
{
	saycolor = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetSkin( void ) const
//|					void SetSkin( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's skin colour
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetSkin( void ) const
{
	return GetColour();
}
void CChar::SetSkin( UI16 value )
{
	SetColour( value );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetStealth( void ) const
//|					void SetStealth( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's stealth walk counter
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetStealth( void ) const
{
	return stealth;
}
void CChar::SetStealth( SI08 newValue )
{
	stealth = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetCell( void ) const
//|					void SetCell( SI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the cell in which a character is jailed
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetCell( void ) const
{
	return cell;
}
void CChar::SetCell( SI08 newVal )
{
	cell = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetRunning( void ) const
//|					void SetRunning( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets whether the character is running
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetRunning( void ) const
{
	return running;
}
void CChar::SetRunning( UI08 newValue )
{
	running = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetStep( void ) const
//|					void SetStep( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the footstep sequence for the character for the purpose of playing
//|					footstep sounds
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetStep( void ) const
{
	return step;
}
void CChar::SetStep( UI08 newValue )
{
	step = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CTownRegion *GetRegion( void ) const
//|					void SetRegion( UI16 newValue )
//|					UI16 GetRegionNum( void ) const
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the town region the character is in
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetOldLocation( SI16 newX, SI16 newY, SI08 newZ )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the old/previous location of the character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetOldLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	oldLocX = newX;
	oldLocY = newY;
	oldLocZ = newZ;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetLocation( const CBaseObject *toSet )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the location of the character to match specified object
//o-----------------------------------------------------------------------------------------------o
void CChar::SetLocation( const CBaseObject *toSet )
{
	if( toSet != nullptr )
		SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber(), toSet->GetInstanceID() );
}

void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway );
//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceID )
//|					void SetLocation( SI16 newX, SI16 newY, SI08 newZ )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the location of the character to a specific set of coordinates/world/instance
//o-----------------------------------------------------------------------------------------------o
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceID )
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
	SetInstanceID( instanceID );
	CMultiObj *newMulti = findMulti( newX, newY, newZ, world, instanceID );
	if( GetMultiObj() != newMulti )
		SetMulti( newMulti );
	if( IsNpc() )
		InitializeWanderArea( this, 10, 10 );
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	SetLocation( newX, newY, newZ, WorldNumber(), GetInstanceID() );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CItem *GetPackItem( void )
//|					void SetPackItem( CItem *newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the main backpack object of the character
//o-----------------------------------------------------------------------------------------------o
CItem * CChar::GetPackItem( void )
{
	if( packitem == nullptr )
	{
		CItem *tempItem = GetItemAtLayer( IL_PACKITEM );
		if( ValidateObject( tempItem ) && tempItem->GetType() == IT_CONTAINER )
			packitem = tempItem;
	}
	return packitem;
}
void CChar::SetPackItem( CItem *newVal )
{
	packitem = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SERIAL calcSerFromObj( CBaseObject *mObj )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Return the serial of an object
//o-----------------------------------------------------------------------------------------------o
SERIAL calcSerFromObj( CBaseObject *mObj )
{
	SERIAL toReturn = INVALIDSERIAL;
	if( ValidateObject( mObj ) )
		toReturn = mObj->GetSerial();
	return toReturn;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CChar *GetTarg( void ) const
//|					void SetTarg( CChar *newTarg )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Returns  character's current target
//o-----------------------------------------------------------------------------------------------o
CChar *CChar::GetTarg( void ) const
{
	return calcCharObjFromSer( targ );
}
void CChar::SetTarg( CChar *newTarg )
{
	targ = calcSerFromObj( newTarg );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CChar *GetAttacker( void ) const
//|					void SetAttacker( CChar *newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets character's current attacker
//o-----------------------------------------------------------------------------------------------o
CChar *CChar::GetAttacker( void ) const
{
	return calcCharObjFromSer( attacker );
}
void CChar::SetAttacker( CChar *newValue )
{
	attacker = calcSerFromObj( newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetAdvObj( void ) const
//|					void SetAdvObj( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the "morex" identifier value of the previous advancement used
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetAdvObj( void ) const
{
	return advobj;
}
void CChar::SetAdvObj( UI16 newValue )
{
	advobj = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	RACEID GetRaceGate( void ) const
//|					void SetRaceGate( RACEID newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the raceID value of a race gate
//o-----------------------------------------------------------------------------------------------o
RACEID CChar::GetRaceGate( void ) const
{
	return raceGate;
}
void CChar::SetRaceGate( RACEID newValue )
{
	raceGate = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetSpellCast( void ) const
//|					void SetSpellCast( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the spell ID of the next spell an NPC will be casting
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetSpellCast( void ) const
{
	return spellCast;
}
void CChar::SetSpellCast( SI08 newValue )
{
	spellCast = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetPriv( void ) const
//|					void SetPriv( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's special privileges
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetPriv( void ) const
{
	return static_cast<UI16>(priv.to_ulong());
}
void CChar::SetPriv( UI16 newValue )
{
	priv = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SKILLVAL GetBaseSkill( UI08 skillToGet ) const
//|					void SetBaseSkill( SKILLVAL newSkillValue, UI08 skillToSet )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a character's specified baseskill (without modifiers)
//o-----------------------------------------------------------------------------------------------o
SKILLVAL CChar::GetBaseSkill( UI08 skillToGet ) const
{
	SKILLVAL rVal = 0;
	if( skillToGet < ALLSKILLS )
		rVal = baseskill[skillToGet];
	return rVal;
}
void CChar::SetBaseSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	if( skillToSet < ALLSKILLS )
		baseskill[skillToSet] = newSkillValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SKILLVAL GetSkill( UI08 skillToGet ) const
//|					void SetSkill( SKILLVAL newSkillValue, UI08 skillToSet )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a character's specified skill (with modifiers)
//o-----------------------------------------------------------------------------------------------o
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
void CChar::SetSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	if( skillToSet <= INTELLECT )
		skill[skillToSet] = newSkillValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetGuildNumber( void ) const
//|					void SetGuildNumber( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets ID of guild character belongs to
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::GetGuildNumber( void ) const
{
	return guildnumber;
}
void CChar::SetGuildNumber( SI16 newValue )
{
	guildnumber = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetFlag( void ) const
//|					void SetFlag( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets flag for character (criminal, innocent, murderer, etc)
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetFlag( void ) const
{
	return flag;
}
void CChar::SetFlag( UI08 newValue )
{
	flag = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetFontType( void ) const
//|					void SetFontType( SI08 newType )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the font type used by the character for speech
//o-----------------------------------------------------------------------------------------------o
SI08 CChar::GetFontType( void ) const
{
	return fonttype;
}
void CChar::SetFontType( SI08 newType )
{
	fonttype = newType;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsGM( void ) const
//|					void SetGM( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character has GM privileges
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsGM( void ) const
{
	return priv.test( BIT_GM );
}
void CChar::SetGM( bool newValue )
{
	priv.set( BIT_GM, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool CanBroadcast( void ) const
//|					void SetBroadcast( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character is allowed to broadcast messages globally
//o-----------------------------------------------------------------------------------------------o
bool CChar::CanBroadcast( void ) const
{
	return priv.test( BIT_BROADCAST );
}
void CChar::SetBroadcast( bool newValue )
{
	priv.set( BIT_BROADCAST, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsInvulnerable( void ) const
//|					void SetInvulnerable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Set whether the character is invulnerable
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsInvulnerable( void ) const
{
	return priv.test( BIT_INVULNERABLE );
}
void CChar::SetInvulnerable( bool newValue )
{
	priv.set( BIT_INVULNERABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool GetSingClickSer( void ) const
//|					void SetSingClickSer( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether player is allowed to see serials of objects clicked
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetSingClickSer( void ) const
{
	return priv.test( BIT_SINGCLICKSER );
}
void CChar::SetSingClickSer( bool newValue )
{
	priv.set( BIT_SINGCLICKSER, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool NoSkillTitles( void ) const
//|					void SetSkillTitles( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether skill titles will show up for character
//o-----------------------------------------------------------------------------------------------o
bool CChar::NoSkillTitles( void ) const
{
	return priv.test( BIT_SKILLTITLES );
}
void CChar::SetSkillTitles( bool newValue )
{
	priv.set( BIT_SKILLTITLES, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsGMPageable( void ) const
//|					void SetGMPageable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character can answer GM pages
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsGMPageable( void ) const
{
	return priv.test( BIT_GMPAGEABLE );
}
void CChar::SetGMPageable( bool newValue )
{
	priv.set( BIT_GMPAGEABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool CanSnoop( void ) const
//|					void SetSnoop( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character can snoop in the backpacks of other players and NPCs
//o-----------------------------------------------------------------------------------------------o
bool CChar::CanSnoop( void ) const
{
	return priv.test( BIT_SNOOP );
}
void CChar::SetSnoop( bool newValue )
{
	priv.set( BIT_SNOOP, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsCounselor( void ) const
//|					void SetCounselor( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character has Counselor privileges
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsCounselor( void ) const
{
	return priv.test( BIT_COUNSELOR );
}
void CChar::SetCounselor( bool newValue )
{
	priv.set( BIT_COUNSELOR, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool AllMove( void ) const
//|					void SetAllMove( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets character can move all items freely, regardless of their moveable state
//o-----------------------------------------------------------------------------------------------o
bool CChar::AllMove( void ) const
{
	return priv.test( BIT_ALLMOVE );
}
void CChar::SetAllMove( bool newValue )
{
	priv.set( BIT_ALLMOVE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsFrozen( void ) const
//|					void SetFrozen( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character is in frozen state
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsFrozen( void ) const
{
	return priv.test( BIT_FROZEN );
}
void CChar::SetFrozen( bool newValue )
{
	priv.set( BIT_FROZEN, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool ViewHouseAsIcon( void ) const
//|					void SetViewHouseAsIcon( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character views icons/deeds instead of actual houses
//o-----------------------------------------------------------------------------------------------o
bool CChar::ViewHouseAsIcon( void ) const
{
	return priv.test( BIT_VIEWHOUSEASICON );
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	priv.set( BIT_VIEWHOUSEASICON, newValue );
}

// 0x0800 is free
//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool NoNeedMana( void ) const
//|					void SetNoNeedMana( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether mana is required for spellcasting for this character
//o-----------------------------------------------------------------------------------------------o
bool CChar::NoNeedMana( void ) const
{
	return priv.test( BIT_NONEEDMANA );
}
void CChar::SetNoNeedMana( bool newValue )
{
	priv.set( BIT_NONEEDMANA, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsDispellable( void ) const
//|					void SetDispellable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether this character is dispellable using dispel spell (i.e. it was summoned)
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsDispellable( void ) const
{
	return priv.test( BIT_DISPELLABLE );
}
void CChar::SetDispellable( bool newValue )
{
	priv.set( BIT_DISPELLABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsPermReflected( void ) const
//|					void SetPermReflected( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character is protected by the magic reflect spell
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsPermReflected( void ) const
{
	return priv.test( BIT_PERMREFLECTED );
}
void CChar::SetPermReflected( bool newValue )
{
	priv.set( BIT_PERMREFLECTED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool NoNeedReags( void ) const
//|					void SetNoNeedReags( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether magical reagents are required for spellcasting for this character
//o-----------------------------------------------------------------------------------------------o
bool CChar::NoNeedReags( void ) const
{
	return priv.test( BIT_NONEEDREAGS );
}
void CChar::SetNoNeedReags( bool newValue )
{
	priv.set( BIT_NONEEDREAGS, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CChar *Dupe( void )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Duplicate character - used for splitting NPCs when hit in combat
//o-----------------------------------------------------------------------------------------------o
CChar *CChar::Dupe( void )
{
	CChar *target = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
	if( target == nullptr )
		return nullptr;

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

	for( SI32 mTID = (SI32)tCHAR_TIMEOUT; mTID < (SI32)tCHAR_COUNT; ++mTID )
		target->SetTimer( (cC_TID)mTID, GetTimer( (cC_TID)mTID ) );
	target->SetHunger( hunger );
	target->SetThirst( thirst );
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
		target->SetNpc( IsNpc() );
		target->SetTamedHungerRate( GetTamedHungerRate() );
		target->SetTamedThirstRate( GetTamedThirstRate() );
		target->SetTamedHungerWildChance( GetTamedHungerWildChance() );
		target->SetTamedThirstWildChance( GetTamedThirstWildChance() );
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
		target->SetMountedWalkingSpeed( GetMountedWalkingSpeed() );
		target->SetMountedRunningSpeed( GetMountedRunningSpeed() );
		target->SetMountedFleeingSpeed( GetMountedFleeingSpeed() );
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

	// Add any script triggers present on object to the new object
	target->scriptTriggers = GetScriptTriggers();
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	FlagColors FlagColour( CChar *toCompare )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Return character flag, which determines the color the character highlights in
//o-----------------------------------------------------------------------------------------------o
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
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromSight
//|	Date		-	April 7th, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all online chars and removes the character from their sight
//o-----------------------------------------------------------------------------------------------o
void CChar::RemoveFromSight( CSocket *mSock )
{
	CPRemoveItem toSend = (*this);
	if( mSock != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveAllObjectsFromSight
//|	Date		-	July 1st, 2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through nearby objects and removes them from the player's sight
//o-----------------------------------------------------------------------------------------------o
void CChar::RemoveAllObjectsFromSight( CSocket *mSock )
{
	if( mSock != nullptr )
	{
		//CChar *myChar = mSock->CurrcharObj();
		//mSock->CurrcharObj();

		// Calculate player's visibility range so we can use it to find nearby objects
		UI16 visRange = mSock->Range() + Races->VisRange( GetRace() );
		UI16 mCharX = this->GetX();
		UI16 mCharY = this->GetY();
		auto minX = mCharX - visRange;
		auto minY = mCharY - visRange;
		auto maxX = mCharX + visRange;
		auto maxY = mCharY + visRange;

		REGIONLIST nearbyRegions = MapRegion->PopulateList( this );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == nullptr )	// no valid region
				continue;

			// First remove nearby characters from sight
			GenericList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( ValidateObject( tempChar ) && tempChar->GetInstanceID() == this->GetInstanceID() )
				{
					CPRemoveItem charToSend = (*tempChar);
					auto tempX = tempChar->GetX();
					auto tempY = tempChar->GetY();

					if( this != tempChar && ( tempX >= minX && tempX <= maxX && tempY >= minY && tempY <= maxY ) &&
					   ( isOnline( ( *tempChar ) ) || tempChar->IsNpc() ||
						( IsGM() && cwmWorldState->ServerData()->ShowOfflinePCs() ) ) )
					{
						mSock->Send( &charToSend );
					}
				}
			}
			regChars->Pop();

			// Now remove nearby items and multis from sight
			GenericList< CItem * > *regItems = MapArea->GetItemList();
			regItems->Push();
			for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
			{
				if( ValidateObject( tempItem ) && tempItem->GetInstanceID() == this->GetInstanceID() )
				{
					CPRemoveItem itemToSend = (*tempItem);
					auto tempItemX = tempItem->GetX();
					auto tempItemY = tempItem->GetY();

					if( tempItem->CanBeObjType( OT_MULTI ) )
					{
						if( tempItemX >= mCharX - DIST_BUILDRANGE && tempItemX <= mCharX + DIST_BUILDRANGE
							&& tempItemY >= mCharY - DIST_BUILDRANGE && tempItemY <= mCharY + DIST_BUILDRANGE )
						{
							mSock->Send( &itemToSend );
						}
					}
					else
					{
						if( tempItemX >= minX && tempItemX <= maxX && tempItemY >= minY && tempItemY <= maxY )
						{
							mSock->Send( &itemToSend );
						}
					}
				}
			}
			regItems->Pop();
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendToSocket( CSocket *s )
//|	Date		-	April 7th, 2000
//|	Modified	-	(June 16, 2003)
//|						Got rid of array based packet sending, replaced with
//|						CPDrawObject, simplifying logic
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -	Sends the information about this person to socket S
//|					IF in range.  Essentially a replacement for impowncreate
//o-----------------------------------------------------------------------------------------------o
void CChar::SendToSocket( CSocket *s )
{
	if( s != nullptr && s->LoginComplete() )
	{
		CChar *mCharObj = s->CurrcharObj();
		bool alwaysSendItemHue = false;
		if( s->ReceivedVersion() )
		{
			if( s->ClientVerShort() >= CVS_70331 )
			{
				alwaysSendItemHue = true;
			}
		}
		else if( cwmWorldState->ServerData()->ClientSupport70331() || cwmWorldState->ServerData()->ClientSupport704565() || cwmWorldState->ServerData()->ClientSupport70610() )
		{
			// No client version received yet. Rely on highest client support enabled in UOX.INI
			alwaysSendItemHue = true;
		}

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
				toSend.AddItem( lIter->second, alwaysSendItemHue );
		}

		toSend.Finalize();
		s->Send( &toSend );

		CPToolTip pSend( GetSerial() );
		s->Send( &pSend );
	}
}


void checkRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Teleport( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates teleporting player and sends nearby objects after teleporting
//o-----------------------------------------------------------------------------------------------o
void CChar::Teleport( void )
{
	CSocket *mSock = GetSocket();
	RemoveFromSight();
	Update();
	if( mSock != nullptr )
	{
		UI16 visrange = mSock->Range() + Races->VisRange( GetRace() );
		mSock->statwindow( this );
		mSock->WalkSequence( -1 );

		UI16 mCharX = this->GetX();
		UI16 mCharY = this->GetY();
		auto minX = mCharX - visrange;
		auto minY = mCharY - visrange;
		auto maxX = mCharX + visrange;
		auto maxY = mCharY + visrange;

		REGIONLIST nearbyRegions = MapRegion->PopulateList( this );
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == nullptr )	// no valid region
				continue;
			GenericList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( ValidateObject( tempChar ) && tempChar->GetInstanceID() == this->GetInstanceID() )
				{
					auto tempX = tempChar->GetX();
					auto tempY = tempChar->GetY();
					if( this != tempChar && ( tempX >= minX && tempX <= maxX && tempY >= minY && tempY <= maxY ) &&
					   ( isOnline( (*tempChar) ) || tempChar->IsNpc() ||
						( IsGM() && cwmWorldState->ServerData()->ShowOfflinePCs() ) ) )
						tempChar->SendToSocket( mSock );
				}
			}
			regChars->Pop();
			GenericList< CItem * > *regItems = MapArea->GetItemList();
			regItems->Push();
			for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
			{
				if( ValidateObject( tempItem ) && tempItem->GetInstanceID() == this->GetInstanceID() )
				{
					auto tempItemX = tempItem->GetX();
					auto tempItemY = tempItem->GetY();
					if( tempItem->CanBeObjType( OT_MULTI ) )
					{
						if( tempItemX >= mCharX - DIST_BUILDRANGE && tempItemX <= mCharX + DIST_BUILDRANGE
							&& tempItemY >= mCharY - DIST_BUILDRANGE && tempItemY <= mCharY + DIST_BUILDRANGE )
						{
							tempItem->SendToSocket( mSock );
						}
					}
					else
					{
						if( tempItemX >= minX && tempItemX <= maxX && tempItemY >= minY && tempItemY <= maxY )
						{
							tempItem->SendToSocket( mSock );
						}
					}
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ExposeToView( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes character visible
//o-----------------------------------------------------------------------------------------------o
void CChar::ExposeToView( void )
{
	SetVisible( VT_VISIBLE );
	SetStealth( -1 );

	// hide it from ourselves, we want to show ourselves to everyone in range
	// now we tell everyone we exist
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Update( CSocket *mSock )
//|	Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends update to all those in range
//o-----------------------------------------------------------------------------------------------o
void CChar::Update( CSocket *mSock )
{
	if( mSock != nullptr )
		SendToSocket( mSock );
	else
	{
		SOCKLIST nearbyChars = FindPlayersInVisrange( this );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			if( !(*cIter)->LoginComplete() )
				continue;

			// Send one extra update to self to fix potential issues with world changing
			if( (*cIter)->CurrcharObj() == this )
				SendToSocket( (*cIter) );

			SendToSocket( (*cIter) );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CItem *GetItemAtLayer( ItemLayers Layer )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the item at layer Layer on paperdoll
//o-----------------------------------------------------------------------------------------------o
CItem *CChar::GetItemAtLayer( ItemLayers Layer )
{
	CItem *rVal = nullptr;
	LAYERLIST_ITERATOR lIter = itemLayers.find( Layer );
	if( lIter != itemLayers.end() )
		rVal = lIter->second;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool WearItem( CItem *toWear )
//|	Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Wears the item toWear and adjusts the stats if any are
//|					required to change.  Returns true if successfully equipped
//o-----------------------------------------------------------------------------------------------o
bool CChar::WearItem( CItem *toWear )
{
	// Run event prior to equipping item, allowing script to prevent equip
	std::vector<UI16> scriptTriggers = toWear->GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *tScript = JSMapping->GetScript( i );
		if( tScript != nullptr )
		{
			// If script returns false, prevent item from being equipped
			if( tScript->OnEquipAttempt( this, toWear ) == 0 )
			{
				return false;
			}
		}
	}

	bool rvalue = true;
	ItemLayers tLayer = toWear->GetLayer();
	if( tLayer != IL_NONE )	// Layer == 0 is a special case, for things like trade windows and such
	{
		if( ValidateObject( GetItemAtLayer( tLayer ) ) )
		{
#if defined( UOX_DEBUG_MODE )
			Console.warning( strutil::format("Failed to equip item %s(0x%X) to layer 0x%X on character %s(0x%X) - another item is already equipped in that layer!", toWear->GetName().c_str(), toWear->GetSerial(), tLayer, GetName().c_str(), serial ));
#endif
			rvalue = false;
		}
		else
		{
			itemLayers[tLayer] = toWear;

			IncStrength2( itemLayers[tLayer]->GetStrength2() );
			IncDexterity2( itemLayers[tLayer]->GetDexterity2() );
			IncIntelligence2( itemLayers[tLayer]->GetIntelligence2() );

			if( toWear->isPostLoaded() )
			{
				if( itemLayers[tLayer]->GetPoisoned() )
					SetPoisoned( GetPoisoned() + itemLayers[tLayer]->GetPoisoned() );	// should be +, not -

				std::vector<UI16> scriptTriggers = toWear->GetScriptTriggers();
				for( auto i : scriptTriggers )
				{
					cScript *tScript = JSMapping->GetScript( i );
					if( tScript != nullptr )
					{
						// If script returns 1, prevent other scripts with event from running
						if( tScript->OnEquip( this, toWear ) == 1 )
						{
							break;
						}
					}
				}
			}
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TakeOffItem( ItemLayers Layer )
//|	Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Takes the item off the character's paperdoll, ensuring that any stat
//|					adjustments are made. Returns true if successfully unequipped
//o-----------------------------------------------------------------------------------------------o
bool CChar::TakeOffItem( ItemLayers Layer )
{
	bool rvalue = false;
	if( ValidateObject( GetItemAtLayer( Layer ) ) )
	{
		// Run event prior to equipping item, allowing script to prevent equip
		std::vector<UI16> scriptTriggers = itemLayers[Layer]->GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *tScript = JSMapping->GetScript( i );
			if( tScript != nullptr )
			{
				if( tScript->OnUnequipAttempt( this, itemLayers[Layer] ) == 0 )
				{
					return false;
				}
			}
		}

		if( Layer == IL_PACKITEM )	// It's our pack!
			SetPackItem( nullptr );
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

		for( auto i : scriptTriggers )
		{
			cScript *tScript = JSMapping->GetScript( i );
			if( tScript != nullptr )
			{
				// If script returns true/1, prevent other scripts with event from running
				if( tScript->OnUnequip( this, itemLayers[Layer] ) == 0 )
				{
					break;
				}
			}
		}

		itemLayers[Layer] = nullptr;
		rvalue = true;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *FirstItem( void )
//|	Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the item reference for the first item on paperdoll
//o-----------------------------------------------------------------------------------------------o
CItem *CChar::FirstItem( void )
{
	CItem *rVal = nullptr;

	layerCtr = itemLayers.begin();
	if( !FinishedItems() )
		rVal = layerCtr->second;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CItem *NextItem( void )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the item reference for the next item on paperdoll
//o-----------------------------------------------------------------------------------------------o
CItem *CChar::NextItem( void )
{
	CItem *rVal = nullptr;
	++layerCtr;
	if( !FinishedItems() )
		rVal = layerCtr->second;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool FinishedItems( void )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if there are no more items on the paperdoll
//o-----------------------------------------------------------------------------------------------o
bool CChar::FinishedItems( void )
{
	return ( layerCtr == itemLayers.end() );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool DumpHeader( std::ofstream &outStream ) const
//|					bool DumpBody( std::ofstream &outStream ) const
//|					void NPCValues_st::DumpBody( std::ofstream& outStream )
//|					void PlayerValues_st::DumpBody( std::ofstream& outStream )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Dump character data to worldfile
//o-----------------------------------------------------------------------------------------------o
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
	outStream << "Thirst=" << (SI16)GetThirst() << '\n';
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
	outStream << "TamedThirstRate=" << tamedThirstRate << '\n';
	outStream << "TamedHungerWildChance=" << (SI16)hungerWildChance << '\n';
	outStream << "TamedThirstWildChance=" << (SI16)thirstWildChance << '\n';
	outStream << "Foodlist=" << foodList << '\n';
	outStream << "WalkingSpeed=" << walkingSpeed << '\n';
	outStream << "RunningSpeed=" << runningSpeed << '\n';
	outStream << "FleeingSpeed=" << fleeingSpeed << '\n';
	outStream << "WalkingSpeedMounted=" << mountedWalkingSpeed << '\n';
	outStream << "RunningSpeedMounted=" << mountedRunningSpeed << '\n';
	outStream << "FleeingSpeedMounted=" << mountedFleeingSpeed << '\n';	
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Save( std::ofstream &outStream )
//|	Date		-	July 21, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves character to worldfile - returns true/false indicating the success
//|					of the write operation
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void BreakConcentration( CSocket *sock )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Breaks the concentration of the character sending a message is a socket exists
//o-----------------------------------------------------------------------------------------------o
void CChar::BreakConcentration( CSocket *sock )
{
	if( IsMeditating() )
	{
		SetMeditating( false );
		if( sock != nullptr )
			sock->sysmessage( 100 );
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	GenericList< CChar * > *GetPetList( void )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of pets the character owns
//o-----------------------------------------------------------------------------------------------o
GenericList< CChar * > *CChar::GetPetList( void )
{
	return &petsControlled;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	ITEMLIST *GetOwnedItems( void )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of items the char owns
//o-----------------------------------------------------------------------------------------------o
ITEMLIST *CChar::GetOwnedItems( void )
{
	return &ownedItems;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void AddOwnedItem( CItem *toAdd )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Adds the item toAdd to the player's owned list
//|					ensuring that it is not ALREADY in the list
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void RemoveOwnedItem( CItem *toRemove )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Removes the item toRemove from the player's owned list
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxHP( void )
//|					void SetMaxHP( UI16 newmaxhp, UI16 newoldstr, RACEID newoldrace )
//|					void SetFixedMaxHP( SI16 newmaxhp )
//|	Date		-	15 February, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum hitpoints (or fixed maximum hitpoints) of the object
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetMaxHP( void )
{
	if( (maxHP_oldstr != GetStrength() || oldRace != GetRace()) && !GetMaxHPFixed() )
		//if str/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == nullptr )
			pRace = Races->Race( 0 );

		maxHP = (UI16)(GetStrength() + (UI16)( ((R32)GetStrength()) * ((R32)(pRace->HPModifier())) / 100 ));
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
		if( pRace == nullptr )
			pRace = Races->Race( 0 );

		maxHP = (UI16)(GetStrength() + (UI16)( ((R32)GetStrength()) * ((R32)(pRace->HPModifier())) / 100 ));

		maxHP_oldstr	= GetStrength();
		oldRace			= GetRace();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetMaxMana( void )
//|					void SetMaxMana( SI16 newmaxmana, UI16 newoldint, RACEID newoldrace )
//|					void SetFixedMaxMana( SI16 newmaxmana )
//|	Date		-	15 February, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum mana (or fixed maximum mana) of the object
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::GetMaxMana( void )
{
	if( (maxMana_oldint != GetIntelligence() || oldRace != GetRace()) && !GetMaxManaFixed() )
		//if int/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == nullptr )
			pRace = Races->Race( 0 );

		maxMana = (SI16)(GetIntelligence() + (SI16)( ((R32)GetIntelligence()) * ((R32)(pRace->ManaModifier())) / 100 ));
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
		if( pRace == nullptr )
			pRace = Races->Race( 0 );

		maxMana = (SI16)(GetIntelligence() + (SI16)( ((R32)GetIntelligence()) * ((R32)(pRace->ManaModifier())) / 100 ));

		maxMana_oldint	= GetIntelligence();
		oldRace			= GetRace();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetMaxStam( void )
//|					void SetMaxStam( SI16 newmaxstam, UI16 newolddex, RACEID newoldrace )
//|					void SetFixedMaxStam( SI16 newmaxstam )
//|	Date		-	15 February, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum stamina (or fixed maximum stamina) of the object
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::GetMaxStam( void )
{
	if( (maxStam_olddex != GetDexterity() || oldRace != GetRace()) && !GetMaxStamFixed() )
		//if dex/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == nullptr )
			pRace = Races->Race( 0 );

		maxStam = (SI16)(GetDexterity() + (SI16)( ((R32)GetDexterity()) * ((R32)(pRace->StamModifier())) / 100 ));
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
		if( pRace == nullptr )
			pRace = Races->Race( 0 );

		maxStam = (SI16)(GetDexterity() + (SI16)( ((R32)GetDexterity()) * ((R32)(pRace->StamModifier())) / 100 ));

		maxStam_olddex	= GetDexterity();
		oldRace			= GetRace();
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 ActualStrength( void ) const
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the actual strength (minus mods) of the character
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::ActualStrength( void ) const
{
	return CBaseObject::GetStrength();
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetStrength( void ) const
//|					void SetStrength( SI16 newValue )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the strength (incl mods) of the character
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::GetStrength( void ) const
{
	return (SI16)(CBaseObject::GetStrength() + GetStrength2());
}
void CChar::SetStrength( SI16 newValue )
{
	CBaseObject::SetStrength( newValue );
	Dirty( UT_HITPOINTS );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 ActualIntelligence( void ) const
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the actual intelligence (minus mods) of the char
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::ActualIntelligence( void ) const
{
	return CBaseObject::GetIntelligence();
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetIntelligence( void ) const
//|					void SetIntelligence( SI16 newValue )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-  Gets/Sets the intelligence (incl mods) of the character
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::GetIntelligence( void ) const
{
	return (SI16)(CBaseObject::GetIntelligence() + GetIntelligence2());
}
void CChar::SetIntelligence( SI16 newValue )
{
	CBaseObject::SetIntelligence( newValue );
	Dirty( UT_MANA );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 ActualDexterity( void ) const
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the actual (minus mods) dexterity of the character
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::ActualDexterity( void ) const
{
	return CBaseObject::GetDexterity();
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetDexterity( void ) const
//|					void SetDexterity( SI16 newValue )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the effective dexterity (including modifications) of the player
//o-----------------------------------------------------------------------------------------------o
SI16 CChar::GetDexterity( void ) const
{
	return (SI16)(CBaseObject::GetDexterity() + GetDexterity2());
}
void CChar::SetDexterity( SI16 newValue )
{
	CBaseObject::SetDexterity( newValue );
	Dirty( UT_STAMINA );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void IncStrength2( SI16 toAdd )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Increments strength2 (modifications) by toAdd
//o-----------------------------------------------------------------------------------------------o
void CChar::IncStrength2( SI16 toAdd )
{
	SetStrength2( (SI16)(GetStrength2() + toAdd) );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void IncDexterity2( SI16 toAdd )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Increments dexterity2 (modifications) by toAdd
//o-----------------------------------------------------------------------------------------------o
void CChar::IncDexterity2( SI16 toAdd )
{
	SetDexterity2( (SI16)(GetDexterity2() + toAdd) );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void IncIntelligence2( SI16 toAdd )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Increments intelligence2 (modifications) by toAdd
//o-----------------------------------------------------------------------------------------------o
void CChar::IncIntelligence2( SI16 toAdd )
{
	SetIntelligence2( (SI16)(GetIntelligence2() + toAdd) );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsMurderer( void ) const
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is a murderer
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsMurderer( void ) const
{
	return ( GetFlag() == 0x01 );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsCriminal( void ) const
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is a criminal
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsCriminal( void ) const
{
	return ( GetFlag() == 0x02 );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsInnocent( void ) const
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is innocent
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsInnocent( void ) const
{
	return ( GetFlag() == 0x04 );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsNeutral( void ) const
//| Date		-	18 July 2005
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is neutral
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsNeutral( void ) const
{
	return ( GetFlag() == 0x08 );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetFlagRed( void )
//| Date		-	2nd October, 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect murderer status
//o-----------------------------------------------------------------------------------------------o
void CChar::SetFlagRed( void )
{
	flag = 0x01;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetFlagGray( void )
//| Date		-	2nd October, 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect criminality
//o-----------------------------------------------------------------------------------------------o
void CChar::SetFlagGray( void )
{
	flag = 0x02;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetFlagBlue( void )
//| Date		-	2nd October, 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect innocence
//o-----------------------------------------------------------------------------------------------o
void CChar::SetFlagBlue( void )
{
	flag = 0x04;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetFlagNeutral( void )
//| Date		-	18th July, 2005
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect neutrality
//o-----------------------------------------------------------------------------------------------o
void CChar::SetFlagNeutral( void )
{
	flag = 0x08;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool DecHunger( const SI08 amt )
//| Date		-	13 March 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Decrements the character's hunger
//o-----------------------------------------------------------------------------------------------o
bool CChar::DecHunger( const SI08 amt )
{
	return SetHunger( (SI08)(GetHunger() - amt) );
}

//o-----------------------------------------------------------------------------------------------o
//| Function    -   bool DecThirst( const SI08 amt )
//| Date        -   6 June 2021
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Decrements the character's thirst
//o-----------------------------------------------------------------------------------------------o
bool CChar::DecThirst( const SI08 amt )
{
	return SetThirst( (SI08)(GetThirst() - amt) );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void StopSpell( void )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Halts spellcasting action for character
//o-----------------------------------------------------------------------------------------------o
void CChar::StopSpell( void )
{
	SetTimer( tCHAR_SPELLTIME, 0 );
	SetCasting( false );
	SetSpellCast( -1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( std::string &UTag, std::string &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to handle loading of characters tags from world files
//o-----------------------------------------------------------------------------------------------o
bool CChar::HandleLine( std::string &UTag, std::string &data )
{
	bool rvalue = CBaseObject::HandleLine( UTag, data );
	if( !rvalue )
	{
		auto csecs = strutil::sections( data, "," );
		
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "ACCOUNT" )
				{
					SetAccountNum( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)));
					rvalue = true;
				}
				else if( UTag == "ATROPHY" )
				{
					int count = 0 ;
					for( auto &value : csecs )
					{
						value = strutil::toupper( strutil::stripTrim( value ));
						if( value == "[END]" )
						{
							break;
						}
						if( value.empty() )
						{
							break;
						}
						SetAtrophy( static_cast<UI08>(std::stoul(value, nullptr, 0)), count );
						count = count + 1;
					}
					rvalue = true;
				}
				else if( UTag == "ADVANCEOBJECT" )
				{
					SetAdvObj( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)));
					rvalue = true;
				}
				else if( UTag == "ADVRACEOBJECT" )
				{
					SetRaceGate( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)));
					rvalue = true;
				}
				else if( UTag == "ALLMOVE" )
				{
					SetAllMove( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'B':
				if( UTag == "BEARDSTYLE" )
				{
					SetBeardStyle( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)));
					rvalue = true;
				}
				else if( UTag == "BEARDCOLOUR" )
				{
					SetBeardColour( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)));
					rvalue = true;
				}
				else if( UTag == "BASESKILLS" )
				{
					auto dsecs = strutil::sections( data, "-" );
					for( auto &value : dsecs )
					{
						if( value.empty() )
						{
							break;
						}
						auto secs = strutil::sections( value, "," );
						if( secs.size() != 2 )
						{
							break;
						}
						auto skillNum = static_cast<SkillLock>(std::stoul(secs[0].substr(1), nullptr, 0));
						auto skillValue = static_cast<UI16>(std::stoul(secs[1].substr(0, secs[1].size() - 1), nullptr, 0));
						SetBaseSkill( skillValue, skillNum );
					}
					rvalue = true;
				}
				else if( UTag == "BEARD" )
				{
					SetBeardStyle( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetBeardColour( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "BRKPEACECHANCEGAIN" )
				{
					SetBrkPeaceChanceGain( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "BRKPEACECHANCE" )
				{
					SetBrkPeaceChance( static_cast<UI16>(std::stoul(strutil::stripTrim(data),nullptr,0)) );
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "COMMANDLEVEL" )
				{
					SetCommandLevel( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "CANRUN" )
				{
					SetRun( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "CANATTACK" )
				{
					SetCanAttack( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "CANTRAIN" )
				{
					SetCanTrain( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'D':
				if( UTag == "DEATHS" )
				{
					SetDeaths( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "DEAD" )
				{
					SetDead( (static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1) );
					rvalue = true;
				}
				break;
			case 'E':
				if( UTag == "EMOTION" )
				{
					SetEmoteColour( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'F':
				if( UTag == "FIXEDLIGHT" )
				{
					SetFixedLight( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "FX1" )
				{
					SetFx( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)), 0 );
					rvalue = true;
				}
				else if( UTag == "FX2" )
				{
					SetFx( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)), 1 );
					rvalue = true;
				}
				else if( UTag == "FY1" )
				{
					SetFy( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)), 0 );
					rvalue = true;
				}
				else if( UTag == "FY2" )
				{
					SetFy( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)), 1 );
					rvalue = true;
				}
				else if( UTag == "FZ1" )
				{
					SetFz( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "FLEEAT" )
				{
					SetFleeAt( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "FONTTYPE" )
				{
					SetFontType( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "FOODLIST" )
				{
					SetFood( data.substr( 0, MAX_NAME - 1 ) );
					rvalue = true;
				}
				else if( UTag == "FLEEINGSPEED" )
				{
					SetFleeingSpeed( static_cast<R32>(std::stof(strutil::stripTrim( data ))) );
					rvalue = true;
				}
				else if( UTag == "FLEEINGSPEEDMOUNTED" )
				{
					SetMountedFleeingSpeed( static_cast<R32>(std::stof(strutil::stripTrim( data ))) );
					rvalue = true;
				}
				break;
			case 'G':
				if( UTag == "GUILDFEALTY" )
				{
					SetGuildFealty( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "GUILDTITLE" )
				{
					SetGuildTitle( data );
					rvalue = true;
				}
				else if( UTag == "GUILDNUMBER" )
				{
					SetGuildNumber( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "GUILDTOGGLE" )
				{
					SetGuildToggle( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'H':
				if( UTag == "HUNGER" )
				{
					SetHunger( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "HOLDG" )
				{
					SetHoldG( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "HAIRSTYLE" )
				{
					SetHairStyle( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "HAIRCOLOUR" )
				{
					SetHairColour( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "HAIR" )
				{
					SetHairStyle( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0]), nullptr, 0)) );
					SetHairColour( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1]), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'I':
				if( UTag == "ISNPC" )
				{
					SetNpc( (static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1) );
					rvalue = true;
				}
				else if( UTag == "ISSHOP" )
				{
					SetShop( (static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1) );
					rvalue = true;
				}
				else if( UTag == "ISWARRING" )
				{
					SetWar( (static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1) );
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
					SetLastOnSecs( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'M':
				if( UTag == "MAYLEVITATE" )
				{
					SetLevitate( (static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1) );
					rvalue = true;
				}
				else if( UTag == "MURDERTIMER" )
				{
					SetTimer( tCHAR_MURDERRATE, BuildTimeValue( static_cast<R32>(std::stof(strutil::stripTrim( data ))) ) );
					rvalue = true;
				}
				else if( UTag == "MAXHP" )
				{
					SetFixedMaxHP( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "MAXMANA" )
				{
					SetFixedMaxMana( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "MAXSTAM" )
				{
					SetFixedMaxStam( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "MOUNTED" )
				{
					SetMounted( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'N':
				if( UTag == "NPCAITYPE" )
				{
					SetNPCAiType( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "NPCWANDER" )
				{
					if( csecs.size() >= 2 )
					{
						SetNpcWander( static_cast<SI08>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
						SetOldNpcWander( static_cast<SI08>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					}
					else
					{
						SetNpcWander( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					}
					rvalue = true;
				}
				else if( UTag == "NPCFLAG" )
				{
					SetNPCFlag( static_cast<cNPC_FLAG>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
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
					SetOrgID( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "ORIGINALSKINID" )
				{
					SetOrgSkin( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "ORIGINALID" )
				{
					SetOrgID( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetOrgSkin( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1]), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PRIVILEGES" )
				{
					SetPriv( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "PACKITEM" )
				{
					temp_container_serial =  static_cast<SERIAL>(std::stoul(strutil::stripTrim( data ), nullptr, 0) );
					rvalue = true;
				}
				else if( UTag == "POISON" )
				{
					SetPoisoned( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "POISONSTRENGTH" )
				{
					SetPoisonStrength( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "PEACEING" )
				{
					SetPeaceing( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "PROVOING" )
				{
					SetProvoing( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "PEACETIMER" )
				{
					SetTimer( tCHAR_PEACETIMER, BuildTimeValue( static_cast<R32>(std::stof(strutil::stripTrim( data ))) ) );
					rvalue = true;
				}
				break;
			case 'Q':
				if( UTag == "QUESTTYPE" )
				{
					SetQuestType( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "QUESTDESTINATIONREGION" )
				{
					SetQuestDestRegion( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "QUESTORIGINALREGION" )
				{
					SetQuestOrigRegion( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "QUESTREGIONS" )
				{
					SetQuestOrigRegion( static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetQuestDestRegion( static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'R':
				if( UTag == "ROBESERIAL" )
				{
					SetRobe( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "RESERVED" )
				{
					SetCell( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "RUNNING" )
				{
					rvalue = true;
				}
				else if( UTag == "REGION" )
				{
					SetRegion(static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "REATTACKAT" )
				{
					SetReattackAt( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "RUNNINGSPEED" )
				{
					SetRunningSpeed( static_cast<R32>(std::stof(strutil::stripTrim( data ))) );
					rvalue = true;
				}
				else if( UTag == "RUNNINGSPEEDMOUNTED" )
				{
					SetMountedRunningSpeed( static_cast<R32>(std::stof(strutil::stripTrim( data ))) );
					rvalue = true;
				}
				break;
			case 'S':
				if( UTag == "SPLIT" )
				{
					if( csecs.size() >= 2 )
					{
						SetSplit( static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
						SetSplitChance( static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					}
					else
					{
						SetSplit( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					}
					rvalue = true;
				}
				else if( UTag == "SPLITCHANCE" )
				{
					SetSplitChance( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SUMMONTIMER" )
				{
					SetTimer( tNPC_SUMMONTIME, static_cast<UI32>(std::stoul( strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SAY" )
				{
					SetSayColour( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "STEALTH" )
				{
					SetStealth( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SPATTACK" )
				{
					if( csecs.size() >=2 )
					{
						SetSpAttack( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
						SetSpDelay( static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					}
					else
					{
						SetSpAttack( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					}
					rvalue = true;
				}
				else if( UTag == "SPECIALATTACKDELAY" )
				{
					SetSpDelay( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SQUELCHED" )
				{
					SetSquelched( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SKILLLOCKS" )
				{
					// Format: Baselocks=[0,34]-[1,255]-[END]
					auto dsecs = strutil::sections( data, "-" );
					
					for( auto &value : dsecs )
					{
						if( value.empty() )
						{
							break;
						}
						auto secs = strutil::sections( value, "," );
						if( secs.size() != 2 )
						{
							break;
						}
						auto skillnum = static_cast<UI08>(std::stoul(secs[0].substr(1), nullptr, 0));
						auto skillvalue = static_cast<SkillLock>(std::stoul(secs[1].substr(0,secs[1].size() - 1), nullptr, 0));
						SetSkillLock( skillvalue, skillnum );
					}
					rvalue = true;
				}
				else if( UTag == "SPEECH" )
				{
					SetSayColour( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetEmoteColour( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "STABLED" )
				{
					SetStabled( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TAMING" )
				{
					SetTaming( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "TAMEDHUNGERRATE" )
				{
					SetTamedHungerRate( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "TAMEDHUNGERWILDCHANCE" )
				{
					SetTamedHungerWildChance( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "TAMEDTHIRSTRATE" )
				{
					SetTamedThirstRate( static_cast<SI08>( std::stoi( strutil::stripTrim( data ), nullptr, 0 ) ) );
					rvalue = true;
				}
				else if( UTag == "TAMEDTHIRSTWILDCHANCE" )
				{
					SetTamedThirstWildChance( static_cast<SI08>( std::stoi( strutil::stripTrim( data ), nullptr, 0 ) ) );
					rvalue = true;
				}
				else if( UTag == "TOWN" )
				{
					SetTown( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "TOWNVOTE" )
				{
					SetTownVote( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "TOWNPRIVILEGES" )
				{
					SetTownpriv( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "TOWNTITLE" )
				{
					SetTownTitle( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if (UTag == "THIRST")
				{
					SetThirst( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'W':
				if( UTag == "WANDERAREA" )
				{
					SetFx( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0)), 0 );
					SetFy( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0)), 0 );
					SetFx( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[2] ), nullptr, 0)), 1 );
					SetFy( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[3] ), nullptr, 0)), 1 );
					SetFz( static_cast<SI08>(std::stoi(strutil::stripTrim( csecs[4] ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "WILLHUNGER" )
				{
					SetHungerStatus( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if (UTag == "WILLTHIRST")
				{
					SetThirstStatus( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "WALKINGSPEED" )
				{
					SetWalkingSpeed( static_cast<R32>(std::stof(strutil::stripTrim( data ))));
					rvalue = true;
				}
				else if( UTag == "WALKINGSPEEDMOUNTED" )
				{
					SetMountedWalkingSpeed( static_cast<R32>(std::stof(strutil::stripTrim( data ))));
					rvalue = true;
				}
				break;
			case 'X':
				if( UTag == "XNPCWANDER" )
				{
					SetOldNpcWander( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LoadRemnants( void )
//|	Date		-	21st January, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o-----------------------------------------------------------------------------------------------o
bool CChar::LoadRemnants( void )
{
	bool rvalue = true;
	SetSerial( serial );

	if( IsNpc() && IsAtWar() )
		SetWar( false );

	CTownRegion *tRegion = calcRegionFromXY( GetX(), GetY(), worldNumber, instanceID );
	SetRegion( (tRegion != nullptr ? tRegion->GetRegionNum() : 0xFF) );
	SetTimer( tCHAR_ANTISPAM, 0 );
	if( GetID() != GetOrgID() && !IsDead() )
		SetID( GetOrgID() );

	const UI16 acct = GetAccount().wAccountIndex;
	if( GetID() > 0x4DF )
	{
		if( acct == AB_INVALID_ID )
		{
			Console.warning( strutil::format("NPC: %s with serial 0x%X with bugged body found, deleting", GetName().c_str(), GetSerial()) );
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
				Console.warning( strutil::format("NPC: %s with serial 0x%X found outside valid world locations, deleting", GetName().c_str(), GetSerial()) );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SkillUsed( UI08 skillNum ) const
//|	Date		-	May 2, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the skill is being used
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SkillUsed( bool value, UI08 skillNum )
//|	Date		-	May 2, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets whether the skill skillNum is being used or not
//o-----------------------------------------------------------------------------------------------o
void CChar::SkillUsed( bool value, UI08 skillNum )
{
	if( skillNum < ALLSKILLS )
	{
		UI08 part		= static_cast<UI08>(skillNum / 32);
		UI08 offset		= static_cast<UI08>(skillNum % 32);
		skillUsed[part].set( offset, value );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PostLoadProcessing( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment
//|					following the loading of the world
//o-----------------------------------------------------------------------------------------------o
void CChar::PostLoadProcessing( void )
{
	CBaseObject::PostLoadProcessing();
	if (temp_container_serial != INVALIDSERIAL)
	{
		SetPackItem(calcItemObjFromSer(temp_container_serial));
	}
	else
	{
		SetPackItem(nullptr);
	}

	SI32 maxWeight = GetStrength() * cwmWorldState->ServerData()->WeightPerStr() + 40;
	if( GetWeight() <= 0 || GetWeight() > MAX_WEIGHT || GetWeight() > maxWeight )
		SetWeight( Weight->calcCharWeight( this ) );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
		Skills->updateSkillLevel( this, i );
	// We need to add things to petlists, so we can cleanup after ourselves properly -
	SetPostLoaded( true );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool IsJailed( void ) const
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is in a jail cell
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsJailed( void ) const
{
	return ( GetCell() != -1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool inDungeon( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a dungeon
//o-----------------------------------------------------------------------------------------------o
bool CChar::inDungeon( void )
{
	bool rValue = false;
	if( GetRegion() != nullptr )
		rValue = GetRegion()->IsDungeon();
	return rValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TextMessage( CSocket *s, std::string toSay, SpeechType msgType, bool spamTimer )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle server-triggered speech for characters
//o-----------------------------------------------------------------------------------------------o
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
			if( s != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TextMessage( CSocket *s, SI32 dictEntry, SpeechType msgType, bool spamTimer, ... )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle server-triggered speech for characters
//o-----------------------------------------------------------------------------------------------o
void CChar::TextMessage( CSocket *s, SI32 dictEntry, SpeechType msgType, int spamTimer, ... )
{
	UnicodeTypes dictLang = ZERO;
	if( s != nullptr )
		dictLang = s->Language();

	std::string txt = Dictionary->GetEntry( dictEntry, dictLang );
	if( !txt.empty() )
	{
		va_list argptr;
		va_start( argptr, spamTimer );
		auto msg = strutil::format(txt,argptr);
		if (msg.size()>512){
			msg = msg.substr(0,512);
		}
		auto spam = false;
		if( spamTimer == 1 )
		{
			spam = true;
		}
		TextMessage( s, msg, msgType, spam );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendWornItems( CSocket *mSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send items worn on character to client
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WalkZ( SI08 newZ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Update character'z Z coordinate based on movement, and handle falling
//o-----------------------------------------------------------------------------------------------o
void CChar::WalkZ( SI08 newZ )
{
	oldLocZ = z;
	z		= newZ;
	UI08 fallDistance = oldLocZ - z;

	if( fallDistance > MAX_Z_FALL )
	{
		std::vector<UI16> scriptTriggers = GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				toExecute->OnFall( (this), fallDistance );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WalkDir( SI08 newDir )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Update character's walking direction
//o-----------------------------------------------------------------------------------------------o
void CChar::WalkDir( SI08 newDir )
{
	dir = newDir;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void Cleanup( void )
//| Date		-	11/6/2003
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Cleans up after character
//o-----------------------------------------------------------------------------------------------o
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
		CChar *tempChar = nullptr;
		tempChar = GetTarg();
		if( ValidateObject( tempChar ) )
		{
			if( tempChar->GetTarg() == this )
			{
				tempChar->SetTarg( nullptr );
				tempChar->SetAttacker( nullptr );
				tempChar->SetAttackFirst( false );
				if( tempChar->IsAtWar() )
					tempChar->ToggleCombat();
			}
			SetTarg( nullptr );
		}
		tempChar = GetAttacker();
		if( ValidateObject( tempChar ) )
		{
			if( tempChar->GetAttacker() == this )
				tempChar->SetAttacker( nullptr );

			if( tempChar->GetTarg() == this )
			{
				tempChar->SetTarg( nullptr );
				tempChar->SetAttackFirst( false );
				if( tempChar->IsAtWar() )
					tempChar->ToggleCombat();
			}
			SetAttacker( nullptr );
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
					if( spawnReg != nullptr )
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
					if( mAcct.lpCharacters[actr] != nullptr && mAcct.lpCharacters[actr]->GetSerial() == GetSerial() )
					{
						Accounts->DelCharacter( mAcct.wAccountIndex, actr );
						break;
					}
				}
			}
		}

		if( GetSpawnObj() != nullptr )
			SetSpawn( INVALIDSERIAL );

		if( IsGuarded() )
		{
			CChar *petGuard = Npcs->getGuardingPet( this, this );
			if( ValidateObject( petGuard ) )
				petGuard->SetGuarding( nullptr );
			SetGuarded( false );
		}
		Npcs->stopPetGuarding( this );

		for( tempChar = petsControlled.First(); !petsControlled.Finished(); tempChar = petsControlled.Next() )
		{
			if( ValidateObject( tempChar ) )
				tempChar->SetOwner( nullptr );
		}
		RemoveSelfFromOwner();	// Let's remove it from our owner (if any)

		//Ensure that object is removed from refreshQueue
		RemoveFromRefreshQueue();
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetHP( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets hitpoints of the character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetHP( SI16 newValue )
{
	CBaseObject::SetHP( std::min( std::max( static_cast<SI16>(0), newValue ), static_cast<SI16>(GetMaxHP()) ) );
	Dirty( UT_HITPOINTS );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetMana( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets mana of the character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetMana( SI16 newValue )
{
	CBaseObject::SetMana( std::min( std::max( static_cast<SI16>(0), newValue ), GetMaxMana() ) );
	Dirty( UT_MANA );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetStamina( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets stamina of the character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetStamina( SI16 newValue )
{
	CBaseObject::SetStamina( std::min( std::max( static_cast<SI16>(0), newValue ), GetMaxStam() ) );
	Dirty( UT_STAMINA );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetPoisoned( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets poisoned state of character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetPoisoned( UI08 newValue )
{
	CBaseObject::SetPoisoned( newValue );
	Dirty( UT_UPDATE );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetStrength2( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets bonus strength stat for character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetStrength2( SI16 nVal )
{
	CBaseObject::SetStrength2( nVal );
	Dirty( UT_HITPOINTS );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetDexterity2( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets bonus dexterity stat for character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetDexterity2( SI16 nVal )
{
	CBaseObject::SetDexterity2( nVal );
	Dirty( UT_STAMINA );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetIntelligence2( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets bonus intelligence stat for character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetIntelligence2( SI16 nVal )
{
	CBaseObject::SetIntelligence2( nVal );
	Dirty( UT_MANA );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void IncStamina( SI16 toInc )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Increase character's stamina by specified value
//o-----------------------------------------------------------------------------------------------o
void CChar::IncStamina( SI16 toInc )
{
	SetStamina( GetStamina() + toInc );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void IncMana( SI16 toInc )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Increase character's mana by specified value
//o-----------------------------------------------------------------------------------------------o
void CChar::IncMana( SI16 toInc )
{
	SetMana( GetMana() + toInc );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ToggleCombat( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle combat mode for NPC
//o-----------------------------------------------------------------------------------------------o
void CChar::ToggleCombat( void )
{
	SetWar( !IsAtWar() );
	Movement->CombatWalk( this );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a	particular type (Char, in this case)
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void Delete( void )
//| Date		-	11/6/2003
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Adds character to deletion queue
//o-----------------------------------------------------------------------------------------------o
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

// Player Specific Functions

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetAccount( CAccountBlock& actbAccount )
//|					CAccountBlock& GetAccount( void )
//|	Date		-	1/14/2003 6:17:45 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets and Returns the CAccountBlock associated with this player
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetAccountNum( void ) const
//|					void SetAccountNum( UI16 newVal )
//|	Date		-	1/14/2003 6:17:45 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets and Returns the account number associated with this player
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetRobe( void ) const
//|					void SetRobe( SERIAL newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets player's death robe
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	std::string GetLastOn( void ) const
//|					void SetLastOn( std::string newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp (in text) for when player was last online
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI32 GetLastOnSecs( void ) const
//|					void SetLastOnSecs( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp (in seconds) for when player was last online
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetAtrophy( UI08 skillToGet ) const
//|					void SetAtrophy( UI08 newValue, UI08 skillToSet )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets skill atrophy for character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SkillLock GetSkillLock( UI08 skillToGet ) const
//|					void SetSkillLock( SkillLock newValue, UI08 skillToSet )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets skill (and stat) lock for character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetMounted( void ) const
//|					void SetMounted( bool newValue )
//| Date		-	23. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets mounted state of character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetStabled( void ) const
//|					void SetStabled( bool newValue )
//|	Date		-	23. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stabled state of pet
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsFlying( void ) const
//|					void SetFlying( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets flying state of character
//o-----------------------------------------------------------------------------------------------o
bool CChar::IsFlying( void ) const
{	
	return bools.test( BIT_FLYING );
}
void CChar::SetFlying( bool newValue )
{
	if( MayLevitate() )
		bools.set( BIT_FLYING, newValue );
}

bool CChar::ToggleFlying( void )
{
	CSocket *tSock = GetSocket();
	if( MayLevitate() )
	{
		if( GetTimer( tCHAR_FLYINGTOGGLE ) <= cwmWorldState->GetUICurrentTime() )
		{
			if( IsFrozen() )
				tSock->sysmessage("You cannot use this ability while frozen.");
			else
			{
				// Start timer for when flying toggle can be used again
				SetTimer( tCHAR_FLYINGTOGGLE, BuildTimeValue( 1.65 ) );

				// Freeze character until flying anim is over
				SetFrozen( true );

				// Toggle flying
				SetFlying( !IsFlying() );

				// Refresh our own character
				CPExtMove toSend = (*this);
				tSock->Send(&toSend);

				// Play takeoff/landing animation
				if( !IsFlying() )
					Effects->PlayNewCharacterAnimation( this, N_ACT_LAND ); // Action 0x0A, subAction 0x00
				else
					Effects->PlayNewCharacterAnimation( this, N_ACT_TAKEOFF ); // Action 0x09, subAction 0x00

				// Send update to nearby characters
				Network->pushConn();
				for( CSocket *tSend = Network->FirstSocket(); !Network->FinishedSockets(); tSend = Network->NextSocket() )
				{
					if( tSend == nullptr )
						continue;
					CChar *mChar = tSend->CurrcharObj();
					if( !ValidateObject( mChar ))
						continue;
					if( WorldNumber() != mChar->WorldNumber() || GetInstanceID() != mChar->GetInstanceID() )
						continue;

					UI16 effRange = static_cast<UI16>(tSend->Range());
					const UI16 visibleRange = static_cast<UI16>( tSend->Range() + Races->VisRange( mChar->GetRace() ));
					if( visibleRange >= effRange )
						effRange = visibleRange;

					if( this != mChar)
					{
						if( !objInRange( this, mChar, effRange ))
						{
							continue;
						}
					}

					toSend.FlagColour(static_cast<UI08>(FlagColour(mChar)));
					tSend->Send(&toSend);
				}
				Network->popConn();
			}
		}
	}
	else if( GetID() == 0x029A || GetID() == 0x029B )
	{
		// Enable flying mode for older gargoyle characters that were created before this property was added to character creation
		SetLevitate( true );
	}
	else
		tSock->sysmessage( "This character is not allowed to fly!" );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool GetMaxHPFixed( void ) const
//|					void SetMaxHPFixed( bool newValue )
//| Date		-	25. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the maximum hp (fixed) for the character
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetMaxHPFixed( void ) const
{
	return bools.test( BIT_MAXHPFIXED );
}
void CChar::SetMaxHPFixed( bool newValue )
{
	bools.set( BIT_MAXHPFIXED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetMaxManaFixed( void ) const
//|					void SetMaxManaFixed( bool newValue )
//|	Date		-	25. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum mana (fixed) for the character
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetMaxManaFixed( void ) const
{
	return bools.test( BIT_MAXMANAFIXED );
}
void CChar::SetMaxManaFixed( bool newValue )
{
	bools.set( BIT_MAXMANAFIXED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool GetMaxStamFixed( void ) const
//|					void SetMaxStamFixed( bool newValue )
//| Date		-	25. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the maximum stamina (fixed) of the character
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetMaxStamFixed( void ) const
{
	return bools.test( BIT_MAXSTAMFIXED );
}
void CChar::SetMaxStamFixed( bool newValue )
{
	bools.set( BIT_MAXSTAMFIXED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	std::string GetOrgName( void ) const
//|					void SetOrgName( std::string newName )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's original name
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetOrgID( void ) const
//|					void SetOrgID( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the player character's original ID
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetOrgSkin( void ) const
//|					void SetOrgSkin( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's original skin
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetHairStyle( void ) const
//|					void etHairStyle( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default hairstyle
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetBeardStyle( void ) const
//|					void SetBeardStyle( UI16 value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default beardstyle
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	COLOUR GetHairColour( void ) const
//|					void SetHairColour( COLOUR value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default haircolour
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	COLOUR GetBeardColour( void ) const
//|					void SetBeardColour( COLOUR value )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default beardcolour
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CChar *GetTrackingTarget( void ) const
//|					void SetTrackingTarget( CChar *newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's tracking target
//o-----------------------------------------------------------------------------------------------o
CChar *CChar::GetTrackingTarget( void ) const
{
	CChar *rVal = nullptr;
	if( IsValidPlayer() )
		rVal = calcCharObjFromSer( mPlayer->trackingTarget );
	return rVal;
}
void CChar::SetTrackingTarget( CChar *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->trackingTarget = calcSerFromObj( newValue );
}
SERIAL CChar::GetTrackingTargetSerial( void ) const
{
	SERIAL rVal = INVALIDSERIAL;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->trackingTarget;
	}
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CChar *GetTrackingTargets( UI08 targetNum ) const
//|					void SetTrackingTargets( CChar *newValue, UI08 targetNum )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets list of possible tracking targets
//o-----------------------------------------------------------------------------------------------o
CChar *CChar::GetTrackingTargets( UI08 targetNum ) const
{
	CChar *rVal = nullptr;
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
		if( newValue != nullptr )
			CreatePlayer();
	}
	if( IsValidPlayer() )
	{
		if( targetNum < mPlayer->trackingTargets.size() )
			mPlayer->trackingTargets[targetNum] = newValue;
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetCommandLevel( void ) const
//|					void SetCommandLevel( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's command level
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetPostType( void ) const
//|					void SetPostType( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets player character's messageboard posting level
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SERIAL GetCallNum( void ) const
//|					void SetCallNum( SERIAL newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the currently active GM/Counselor call for the character
//o-----------------------------------------------------------------------------------------------o
SERIAL CChar::GetCallNum( void ) const
{
	SI16 rVal = DEFPLAYER_CALLNUM;
	if( IsValidPlayer() )
		rVal = mPlayer->callNum;
	return rVal;
}
void CChar::SetCallNum( SERIAL newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_CALLNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->callNum = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SERIAL GetPlayerCallNum( void ) const
//|					void SetPlayerCallNum( SERIAL newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's call number in GM/CNS Queue
//o-----------------------------------------------------------------------------------------------o
SERIAL CChar::GetPlayerCallNum( void ) const
{
	SERIAL rVal = DEFPLAYER_PLAYERCALLNUM;
	if( IsValidPlayer() )
		rVal = mPlayer->playerCallNum;
	return rVal;
}
void CChar::SetPlayerCallNum( SERIAL newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_PLAYERCALLNUM )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->playerCallNum = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetSquelched( void ) const
//|					void SetSquelched( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the squelched status of the player's character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CItem *GetSpeechItem( void ) const
//|					void SetSpeechItem( CItem *newValue )
//| Date		-	April 8th, 2000
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets item related to the speech we're working on
//|					IE the item for name deed if we're renaming ourselves
//o-----------------------------------------------------------------------------------------------o
CItem *CChar::GetSpeechItem( void ) const
{
	CItem *rVal = nullptr;
	if( IsValidPlayer() )
		rVal = mPlayer->speechItem;
	return rVal;
}
void CChar::SetSpeechItem( CItem *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->speechItem = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetSpeechMode( void ) const
//|					void SetSpeechMode( UI08 newValue )
//| Date		-	April 8th, 2000
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets Speech mode information, as to what mode of speech
//|					we are in.  Valid values are found just below
//|						0 normal speech
//|						1 GM page
//|						2 Counselor page
//|						3 Player Vendor item pricing
//|						4 Player Vendor item describing
//|						5 Key renaming
//|						6 Name deed
//|						7 Rune renaming
//|						8 Sign renaming
//|						9 JS Speech
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetSpeechID( void ) const
//|					void SetSpeechID( UI08 newValue )
//| Date		-	January 20th, 2002
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the ID for the JS Speech Input
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetSpeechID( void ) const
{
	UI08 rVal = DEFPLAYER_SPEECHID;
	if( IsValidPlayer() )
		rVal = mPlayer->speechID;
	return rVal;
}
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	cScript *GetSpeechCallback( void ) const
//|					void SetSpeechCallback( cScript *newValue )
//| Date		-	January 20th, 2002
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets callback for the onSpeechInput function
//o-----------------------------------------------------------------------------------------------o
cScript *CChar::GetSpeechCallback( void ) const
{
	cScript *rVal = nullptr;
	if( IsValidPlayer() )
		rVal = mPlayer->speechCallback;
	return rVal;
}
void CChar::SetSpeechCallback( cScript *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->speechCallback = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetFixedLight( void ) const
//|					void SetFixedLight( UI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets fixed light level of the character
//|					255 is off
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetDeaths( void ) const
//|					void SetDeaths( UI16 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Sets the total number of deaths a player has
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CSocket *GetSocket( void ) const
//|					void SetSocket( CSocket *newVal )
//| Date		-	November 7, 2005
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets socket attached to the player character
//o-----------------------------------------------------------------------------------------------o
CSocket * CChar::GetSocket( void ) const
{
	CSocket *rVal = nullptr;
	if( IsValidPlayer() )
		rVal = mPlayer->socket;
	return rVal;
}
void CChar::SetSocket( CSocket *newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != nullptr )
			CreatePlayer();
	}
	if( IsValidPlayer() )
		mPlayer->socket = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI32 GetTownVote( void ) const
//|					void SetTownVote( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets serial of the player a character has voted to be mayor.
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetTownPriv( void ) const
//|					void SetTownpriv( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets town member privledges (1 = Resident, 2 = Mayor)
//o-----------------------------------------------------------------------------------------------o
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

// NPC Specific Functions

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI16 GetTamedHungerRate( void ) const
//|					void SetTamedHungerRate( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Set the rate at which a pet hungers
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI16 GetTamedThirstRate( void ) const
//|                 void SetTamedThirstRate( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Get/Set the rate at which a pet thirsts
//o-----------------------------------------------------------------------------------------------o
UI16 CChar::GetTamedThirstRate( void ) const
{
	UI16 retVal = DEFNPC_TAMEDTHIRSTRATE;
	if( IsValidNPC() )
		retVal = mNPC->tamedHungerRate;
	return retVal;
}
void CChar::SetTamedThirstRate( UI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_TAMEDTHIRSTRATE != newValue )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->tamedThirstRate = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetTamedHungerWildChance( void ) const
//|					void SetTamedHungerWildChance( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets chance for a hungry pet to go wild
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function    -   UI08 GetTamedThirstWildChance( void ) const
//|                 void SetTamedThirstWildChance( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets chance for a thirsty pet to go wild
//o-----------------------------------------------------------------------------------------------o
UI08 CChar::GetTamedThirstWildChance( void ) const
{
	UI08 retVal = DEFNPC_THIRSTWILDCHANCE;
	if( IsValidNPC() )
		retVal = mNPC->thirstWildChance;
	return retVal;
}
void CChar::SetTamedThirstWildChance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_THIRSTWILDCHANCE != newValue )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->thirstWildChance = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	std::string GetFood( void ) const
//|					void SetFood( std::string food )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets FOODLIST entry for feeding tamed pets
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetNPCAiType( void ) const
//|					void SetNPCAiType( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the AI type of the NPC
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CBaseObject *GetGuarding( void ) const
//|					void SetGuarding( CBaseObject *newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the SERIAL of the object the character is guarding
//o-----------------------------------------------------------------------------------------------o
CBaseObject *CChar::GetGuarding( void ) const
{
	CBaseObject *rVal = nullptr;
	if( IsValidNPC() )
		rVal = mNPC->petGuarding;
	return rVal;
}
void CChar::SetGuarding( CBaseObject *newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != nullptr )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->petGuarding = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetTaming( void ) const
//|					void SetTaming( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets minimum skill required to tame the character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetPeaceing( void ) const
//|					void SetPeaceing( SI16 newValue )
//| Date		-	25. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets minimum skill required to peace the character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetProvoing( void ) const
//|					void SetProvoing( SI16 newValue )
//| Date		-	25. Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets minimum skill required to provocate the character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetTrainingPlayerIn( void ) const
//|					void SetTrainingPlayerIn( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets skill the player is being trained in. 255 is no training
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI32 GetHoldG( void ) const
//|					void SetHoldG( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets amount of gold being held by a Player Vendor
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetSplit( void ) const
//|					void SetSplit( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets split level of the character
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetSplitChance( void ) const
//|					void SetSplitChance( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the chance of an NPC splitting
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetFx( UI08 part ) const
//|					void SetFx( SI16 newVal, UI08 part )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets x1 and x2 boundry of an npc wander area
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetFy( UI08 part ) const
//|					void SetFy( SI16 newVal, UI08 part )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets y1 and y2 boundry of an npc wander area
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetFz( void ) const
//|					void SetFz( SI08 newVal )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets z of an npc wander area
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetNpcWander( void ) const
//|					void SetNpcWander( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC Wander mode
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetOldNpcWander( void ) const
//|					void SetOldNpcWander( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets previous NPC Wander mode
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CChar *GetFTarg( void ) const
//|					void SetFTarg( CChar *newTarg )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC Follow target
//o-----------------------------------------------------------------------------------------------o
CChar *CChar::GetFTarg( void ) const
{
	CChar *rVal = nullptr;
	if( IsValidNPC() )
		rVal = calcCharObjFromSer( mNPC->fTarg );
	return rVal;
}
void CChar::SetFTarg( CChar *newTarg )
{
	if( !IsValidNPC() )
	{
		if( newTarg != nullptr )
			CreateNPC();
	}
	if( IsValidNPC() )
		mNPC->fTarg = calcSerFromObj( newTarg );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetSpAttack( void ) const
//|					void SetSpAttack( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Spell Attack setting
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI08 GetSpDelay( void ) const
//|					void SetSpDelay( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Spell Delay setting
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetQuestType( void ) const
//|					void SetQuestType( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Quest Type
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetQuestOrigRegion( void ) const
//|					void SetQuestOrigRegion( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Quest Origin Region - used for escort quests
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 GetQuestDestRegion( void ) const
//|					void SetQuestDestRegion( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Quest Destination Region
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetFleeAt( void ) const
//|					void SetFleeAt( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets health value at which an NPC will turn tail and run
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	SI16 GetReattackAt( void ) const
//|					void SetReattackAt( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets health value at which an NPC will start fighting again
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	UI08 PopDirection( void )
//|					void PushDirection( UI08 newDir, bool pushFront )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets queue of directions for NPC
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool StillGotDirs( void ) const
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Check if NPC's Direction Queue is empty
//o-----------------------------------------------------------------------------------------------o
bool CChar::StillGotDirs( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
		rVal = !mNPC->pathToFollow.empty();
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void FlushPath( void )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Empty NPC's Queue of Directions.
//o-----------------------------------------------------------------------------------------------o
void CChar::FlushPath( void )
{
	if( IsValidNPC() )
	{
		while( StillGotDirs() )
			PopDirection();
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	CHARLIST *GetFriendList( void )
//| Date		-	20 July 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the characters list of friends
//o-----------------------------------------------------------------------------------------------o
CHARLIST *CChar::GetFriendList( void )
{
	CHARLIST *rVal = nullptr;
	if( IsValidNPC() )
		rVal = &mNPC->petFriends;
	return rVal;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void AddFriend( CChar *toAdd )
//| Date		-	20 July 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Adds the friend toAdd to the player's friends list
//|					ensuring it is already not on it
//o-----------------------------------------------------------------------------------------------o
void CChar::AddFriend( CChar *toAdd )
{
	if( !IsValidNPC() )
	{
		if( toAdd != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void RemoveFriend( CChar *toRemove )
//| Date		-	20 July 2001
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Removes the friend toRemove from the pets friends list
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	cNPC_FLAG GetNPCFlag( void ) const
//|					void SetNPCFlag( cNPC_FLAG flagType )
//| Date		-	February 9, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's default flag color
//o-----------------------------------------------------------------------------------------------o
cNPC_FLAG CChar::GetNPCFlag( void ) const
{
	cNPC_FLAG retVal = fNPC_NEUTRAL;

	if( IsValidNPC() )
		retVal = mNPC->npcFlag;

	return retVal;
}
void CChar::SetNPCFlag( cNPC_FLAG flagType )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->npcFlag = flagType;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	R32 GetWalkingSpeed( void ) const
//|					void SetWalkingSpeed( R32 newValue )
//| Date		-	Juni 9, 2007
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's walking speed
//o-----------------------------------------------------------------------------------------------o
R32 CChar::GetWalkingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCWalkingSpeed();

	if( IsValidNPC() )
		if( mNPC->walkingSpeed > 0 )
			retVal = mNPC->walkingSpeed;

#if defined( UOX_DEBUG_MODE )
	retVal = (retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetWalkingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->walkingSpeed = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	R32 GetRunningSpeed( void ) const
//|					void SetRunningSpeed( R32 newValue )
//| Date		-	Juni 9, 2007
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's running speed
//o-----------------------------------------------------------------------------------------------o
R32 CChar::GetRunningSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCRunningSpeed();

	if( IsValidNPC() )
		if( mNPC->runningSpeed > 0 )
			retVal = mNPC->runningSpeed;

#if defined( UOX_DEBUG_MODE )
	retVal = (retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetRunningSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->runningSpeed = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	R32 GetFleeingSpeed( void ) const
//|					void SetFleeingSpeed( R32 newValue )
//| Date		-	Juni 9, 2007
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's fleeing speed
//o-----------------------------------------------------------------------------------------------o
R32 CChar::GetFleeingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCFleeingSpeed();

	if( IsValidNPC() )
		if( mNPC->fleeingSpeed > 0 )
			retVal = mNPC->fleeingSpeed;

#if defined( UOX_DEBUG_MODE )
	retVal = (retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetFleeingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->fleeingSpeed = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	R32 GetMountedWalkingSpeed( void ) const
//|					void SetMountedWalkingSpeed( R32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's mounted walking speed
//o-----------------------------------------------------------------------------------------------o
R32 CChar::GetMountedWalkingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCMountedWalkingSpeed();

	if( IsValidNPC() )
		if( mNPC->mountedWalkingSpeed > 0 )
			retVal = mNPC->mountedWalkingSpeed;

#if defined( UOX_DEBUG_MODE )
	retVal = ( retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetMountedWalkingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->mountedWalkingSpeed = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	R32 GetMountedRunningSpeed( void ) const
//|					void SetMountedRunningSpeed( R32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's mounted running speed
//o-----------------------------------------------------------------------------------------------o
R32 CChar::GetMountedRunningSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCMountedRunningSpeed();

	if( IsValidNPC() )
		if( mNPC->mountedRunningSpeed > 0 )
			retVal = mNPC->mountedRunningSpeed;

#if defined( UOX_DEBUG_MODE )
	retVal = ( retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetMountedRunningSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->mountedRunningSpeed = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	R32 GetMountedFleeingSpeed( void ) const
//|					void SetMountedFleeingSpeed( R32 newValue )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's fleeing speed
//o-----------------------------------------------------------------------------------------------o
R32 CChar::GetMountedFleeingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCMountedFleeingSpeed();

	if( IsValidNPC() )
		if( mNPC->mountedFleeingSpeed > 0 )
			retVal = mNPC->mountedFleeingSpeed;

#if defined( UOX_DEBUG_MODE )
	retVal = ( retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetMountedFleeingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
		CreateNPC();

	if( IsValidNPC() )
		mNPC->mountedFleeingSpeed = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool DTEgreater( DamageTrackEntry *elem1, DamageTrackEntry *elem2 )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Compare damage done for two damage entries
//o-----------------------------------------------------------------------------------------------o
bool DTEgreater( DamageTrackEntry *elem1, DamageTrackEntry *elem2 )
{
	if( elem1 == nullptr )
		return false;
	if( elem2 == nullptr )
		return true;
	return elem1->damageDone > elem2->damageDone;
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void Heal( SI16 healValue, CChar *healer )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Heal character, and keep track of healer and amount healed
//o-----------------------------------------------------------------------------------------------o
void CChar::Heal( SI16 healValue, CChar *healer )
{
	SetHP( hitpoints + healValue );
	if( healer != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void ReactOnDamage( WeatherType damageType, CChar *attacker )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Lets character react to damage dealt to them
//o-----------------------------------------------------------------------------------------------o
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
					if( GetMounted() )
						SetTimer( tNPC_MOVETIME, BuildTimeValue( GetMountedWalkingSpeed() ) );
					else
						SetTimer( tNPC_MOVETIME, BuildTimeValue( GetWalkingSpeed() ) );
				}
			} else if( mSock != nullptr )
				BreakConcentration( mSock );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void Damage( SI16 damageValue, CChar *attacker, bool doRepsys )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Damage character, and keep track of attacker and damage amount
//o-----------------------------------------------------------------------------------------------o
void CChar::Damage( SI16 damageValue, CChar *attacker, bool doRepsys )
{
	std::vector<UI16> scriptTriggers = GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( i );
		if( toExecute != nullptr )
			toExecute->OnDamage(  this, attacker, damageValue );
	}

	CSocket *mSock = GetSocket(), *attSock = nullptr, *attOwnerSock = nullptr;

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
		if( mSock != nullptr )
			mSock->Send( &toDisplay );
		if( attSock != nullptr && attSock != mSock )
			attSock->Send( &toDisplay );
		if( attOwnerSock != nullptr )
			attOwnerSock->Send( &toDisplay );
	}

	// Apply the damage
	SetHP( hitpoints - damageValue );

	// Spawn blood effects
	if( damageValue >= std::max(static_cast<SI16>(1), static_cast<SI16>(floor(GetMaxHP() * 0.01))) ) // Only display blood effects if damage done is higher than 1, or 1% of max health - whichever is higher
	{
		UI08 bloodEffectChance = cwmWorldState->ServerData()->CombatBloodEffectChance();
		bool spawnBlood = ( bloodEffectChance >= static_cast<UI08>(RandomNum( 0, 99 )));
		if( spawnBlood )
		{
			BloodTypes bloodType = BLOOD_BLEED;
			auto foijf = GetMaxHP() * 0.2;
			if( damageValue >= static_cast<SI16>(floor(GetMaxHP() * 0.2 )) )
			{
				// If damage done is higher than 20% of max health, spawn larger blood splats
				bloodType = BLOOD_CRITICAL;
			}

			UI16 bloodColour = Races->BloodColour( GetRace() ); // Fetch blood color from race property
			if( bloodColour == 0xffff )
			{
				// If blood colour is 0xffff in the race setup, inherit color of NPC instead!
				bloodColour = GetSkin();
			}
			CItem * bloodEffect = Effects->SpawnBloodEffect( WorldNumber(), GetInstanceID(), bloodColour, bloodType );
			if( ValidateObject( bloodEffect ) )
			{
				// Finally, set blood's location to match that of the character
				bloodEffect->SetLocation( this );
			}
		}
	}

	// Handle peace state
	if( !GetCanAttack() )
	{
		const UI08 currentBreakChance = GetBrkPeaceChance();
		if( (UI08)RandomNum( 1, 100 ) <= currentBreakChance )
		{
			SetCanAttack( true );
			if( mSock != nullptr )
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
					TextMessage( nullptr, 335, TALK, true );
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

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void Die( CChar *attacker, bool doRepsys )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Kills character and handles consequences for character death
//o-----------------------------------------------------------------------------------------------o
void CChar::Die( CChar *attacker, bool doRepsys )
{
	std::vector<UI16> scriptTriggers = GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( i );
		if( toExecute != nullptr )
		{
			SI08 retStatus = toExecute->OnDeathBlow( this, attacker );

			// -1 == script doesn't exist, or returned -1
			// 0 == script returned false, 0, or nothing - don't execute hard code
			// 1 == script returned true or 1
			if( retStatus == 0 )
				return;
		}
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
					if( attSock != nullptr )
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
		if( (!attacker->IsNpc()) && (!IsNpc()) ) {
			Console.log( strutil::format(Dictionary->GetEntry( 1617 ),GetName().c_str(), attacker->GetName().c_str()), "PvP.log");
		}

		Combat->Kill( attacker, this );
	}
	else
		HandleDeath( this, nullptr );
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void UpdateDamageTrack( void )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Keeps track of damage dealt and healing done to character over time
//o-----------------------------------------------------------------------------------------------o
void CChar::UpdateDamageTrack( void )
{
	TIMERVAL currentTime	= cwmWorldState->GetUICurrentTime();
	DamageTrackEntry *i		= nullptr;
	// Update the damage stuff
	for( i = damageDealt.First(); !damageDealt.Finished(); i = damageDealt.Next() )
	{
		if( i == nullptr )
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
		if( i == nullptr )
		{
			damageHealed.Remove( i );
			continue;
		}
		if( (i->lastDamageDone + 300000) < currentTime )	// if it's been 5 minutes since they did any damage
			damageHealed.Remove( i, true );
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void SetWeight( SI32 newVal, bool doWeightUpdate )
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets weight of character
//o-----------------------------------------------------------------------------------------------o
void CChar::SetWeight( SI32 newVal, bool doWeightUpdate )
{
	Dirty( UT_STATWINDOW );
	weight = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Dirty( UpdateTypes updateType )
//|	Date		-	25 July, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces the object onto the global refresh queue
//o-----------------------------------------------------------------------------------------------o
void CChar::Dirty( UpdateTypes updateType )
{
	if( isPostLoaded() )
	{
		updateTypes.set( updateType, true );
		CBaseObject::Dirty( updateType );
	}
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool GetUpdate( UpdateTypes updateType ) const
//| Date		-	10/31/2003
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if we have set a specific UpdateType
//o-----------------------------------------------------------------------------------------------o
bool CChar::GetUpdate( UpdateTypes updateType ) const
{
	return updateTypes.test( updateType );
}
//o-----------------------------------------------------------------------------------------------o
//| Function	-	void ClearUpdate( void )
//| Date		-	3/20/2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the UpdateType bitlist, used at the end of our refresh queue
//o-----------------------------------------------------------------------------------------------o
void CChar::ClearUpdate( void )
{
	updateTypes.reset();
}

//o-----------------------------------------------------------------------------------------------o
//| Function	-	void InParty( bool value )
//| Date		-	21st September, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Sets/clears whether the character is in a party or not
//o-----------------------------------------------------------------------------------------------o
void CChar::InParty( bool value )
{
	bools.set( BIT_INPARTY, value );
}
//o-----------------------------------------------------------------------------------------------o
//| Function	-	bool InParty( void ) const
//| Date		-	21st September, 2006
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is in a party or not
//o-----------------------------------------------------------------------------------------------o
bool CChar::InParty( void ) const
{
	return bools.test( BIT_INPARTY );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CountHousesOwnedFunctor( CBaseObject *a, UI32 &b, void *extraData )
//|					void CountHousesOwned( CChar *mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to count the houses owned by a given character
//o-----------------------------------------------------------------------------------------------o
bool CountHousesOwnedFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	UI32 *ourData		= (UI32 *)extraData;
	CChar *mChar		= calcCharObjFromSer( ourData[0] );
	bool trackHousesPerAccount = ourData[1];
	bool countCoOwnedHouses = ourData[2];
	if( ValidateObject( mChar ) && ValidateObject( a ) && a->CanBeObjType( OT_MULTI ) )
	{
		CMultiObj *i = static_cast< CMultiObj * >(a);
		if( i->GetObjType() == OT_BOAT )
			return false;

		if( trackHousesPerAccount )
		{
			if( countCoOwnedHouses )
			{
				// Count house co-ownership per account
				if( i->GetOwnerObj()->GetAccountNum() != mChar->GetAccountNum() && i->IsOnOwnerList( mChar ) )
				{
					++b;
					return true;
				}
				else
				{
					// Loop through characters in house's ownerlist to see if any other characters
					// on the same account co-owns the house
					bool coOwnedByOtherCharOnAccount = i->CheckForAccountCoOwnership( mChar );
					if( coOwnedByOtherCharOnAccount )
					{
						++b;
						return true;
					}
				}
			}
			else if( !countCoOwnedHouses && i->GetOwnerObj()->GetAccountNum() == mChar->GetAccountNum() )
			{
				// Count house ownership per account
				++b;
				return true;
			}
		}
		else
		{
			if( countCoOwnedHouses && ( i->GetOwnerObj()->GetAccountNum() != mChar->GetAccountNum() && i->IsOnOwnerList( mChar )))
			{
				// Count house co-ownership per character
				++b;
				return true;
			}
			else if( !countCoOwnedHouses && i->GetOwnerObj() == mChar )
			{
				// Count house ownership per character
				++b;
				return true;
			}
		}
	}
	return true;
}
UI32 CChar::CountHousesOwned( bool countCoOwnedHouses )
{
	UI32 b		= 0;
	if( cwmWorldState->ServerData()->TrackHousesPerAccount() || countCoOwnedHouses )
	{
		// Count all houses owned by characters on player's account by iterating over all multis on the server(!)
		UI32 toPass[3];
		toPass[0] = GetSerial();
		toPass[1] = cwmWorldState->ServerData()->TrackHousesPerAccount();
		toPass[2] = countCoOwnedHouses;
		ObjectFactory::getSingleton().IterateOver( OT_MULTI, b, toPass, &CountHousesOwnedFunctor );
	}
	else
	{
		// Count all houses owned by player by checking list of character's owned items!
		ITEMLIST *ownedItems = GetOwnedItems();
		for( ITEMLIST_CITERATOR I = ownedItems->begin(); I != ownedItems->end(); ++I )
		{
			CBaseObject *oItem = ( *I );
			if( ValidateObject( oItem ) && oItem->GetObjType() == OT_MULTI )
			{
				if( oItem->GetOwnerObj() == this )
					b++;
			}
		}
	}
	return b;
}
