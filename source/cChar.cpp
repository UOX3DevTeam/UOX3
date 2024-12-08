//o------------------------------------------------------------------------------------------------o
//|	File			-	cChar.cpp
//|	Date			-	29th March, 2000
//o------------------------------------------------------------------------------------------------o
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
//o------------------------------------------------------------------------------------------------o
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
// Character Privs
const UI32 BIT_GM				=	0; // 0x1
const UI32 BIT_BROADCAST		=	1; // 0x2
const UI32 BIT_INVULNERABLE		=	2; // 0x4
const UI32 BIT_SINGCLICKSER		=	3; // 0x8
const UI32 BIT_SKILLTITLES		=	4; // 0x10
const UI32 BIT_GMPAGEABLE		=	5; // 0x20
const UI32 BIT_SNOOP			=	6; // 0x40
const UI32 BIT_COUNSELOR		=	7; // 0x80
const UI32 BIT_ALLMOVE			=	8; // 0x100
const UI32 BIT_FROZEN			=	9; // 0x200
const UI32 BIT_VIEWHOUSEASICON	=	10; // 0x400
const UI32 BIT_NONEEDMANA		=	11; // 0x800
const UI32 BIT_DISPELLABLE		=	12; // 0x1000
const UI32 BIT_TEMPREFLECTED	=	13; // 0x2000
const UI32 BIT_NONEEDREAGS		=	14; // 0x4000
const UI32 BIT_PERMREFLECTED	=	15; // 0x8000

// Character Bools
const UI32 BIT_UNICODE			=	1;
const UI32 BIT_NPC				=	2;
const UI32 BIT_SHOP				=	3;
const UI32 BIT_DEAD				=	4;
const UI32 BIT_ATWAR			=	5;
const UI32 BIT_DISGUISED		=	6;
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
const UI32 BIT_HIRELING			=	32;
const UI32 BIT_ISPASSIVE		=	33;
const UI32 BIT_HASSTOLEN		=	34;
const UI32 BIT_KARMALOCK		=	35;

const UI32 BIT_MOUNTED			=	0;
const UI32 BIT_STABLED			=	1;
const UI32 BIT_AWAKE			=   2;

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
const UI16			DEFPLAYER_CONTROLSLOTSUSED	= 0;
const UI32			DEFPLAYER_CREATEDON			= 0;
const UI32			DEFPLAYER_NPCGUILDJOINED	= 0;
const UI32			DEFPLAYER_PLAYTIME			= 0;

CChar::PlayerValues_st::PlayerValues_st() : callNum( DEFPLAYER_CALLNUM ), playerCallNum( DEFPLAYER_PLAYERCALLNUM ), trackingTarget( DEFPLAYER_TRACKINGTARGET ),
squelched( DEFPLAYER_SQUELCHED ), commandLevel( DEFPLAYER_COMMANDLEVEL ), postType( DEFPLAYER_POSTTYPE ), hairStyle( DEFPLAYER_HAIRSTYLE ), beardStyle( DEFPLAYER_BEARDSTYLE ),
hairColour( DEFPLAYER_HAIRCOLOUR ), beardColour( DEFPLAYER_BEARDCOLOUR ), speechItem( nullptr ), speechMode( DEFPLAYER_SPEECHMODE ), speechId( DEFPLAYER_SPEECHID ),
speechCallback( nullptr ), robe( DEFPLAYER_ROBE ), accountNum( DEFPLAYER_ACCOUNTNUM ), origSkin( DEFPLAYER_ORIGSKIN ), origId( DEFPLAYER_ORIGID ),
fixedLight( DEFPLAYER_FIXEDLIGHT ), deaths( DEFPLAYER_DEATHS ), socket( nullptr ), townVote( DEFPLAYER_TOWNVOTE ), townPriv( DEFPLAYER_TOWNPRIV ), controlSlotsUsed( DEFPLAYER_CONTROLSLOTSUSED ),
createdOn( DEFPLAYER_CREATEDON ), npcGuildJoined( DEFPLAYER_NPCGUILDJOINED ), playTime( DEFPLAYER_PLAYTIME )
{
	//memset( &lockState[0],		0, sizeof( UI08 )		* (INTELLECT+1) );
	// Changed to the following, as only the 15?16? first lockStates would get initialized or whanot
	memset( &lockState[0],		0, sizeof( lockState ));

	for( UI08 j = 0; j <= INTELLECT; ++j )
	{
		atrophy[j] = j;
	}

	if( cwmWorldState != nullptr )
	{
		trackingTargets.resize( cwmWorldState->ServerData()->TrackingMaxTargets() );
	}
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
const UI08			DEFNPC_FLEEDISTANCE			= 0;
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
const UI16			DEFNPC_CONTROLSLOTS			= 0;
const UI16			DEFNPC_MAXLOYALTY			= 100;
const UI16			DEFNPC_LOYALTY				= 25;
const UI16			DEFNPC_ORNERINESS			= 0;
const SI08			DEFNPC_PATHRESULT			= 0;
const UI16			DEFNPC_PATHTARGX			= 0;
const UI16			DEFNPC_PATHTARGY			= 0;

CChar::NPCValues_st::NPCValues_st() : wanderMode( DEFNPC_WANDER ), oldWanderMode( DEFNPC_OLDWANDER ), fTarg( DEFNPC_FTARG ), fz( DEFNPC_FZ1 ),
aiType( DEFNPC_AITYPE ), spellAttack( DEFNPC_SPATTACK ), spellDelay( DEFNPC_SPADELAY ), taming( DEFNPC_TAMING ), fleeAt( DEFNPC_FLEEAT ),
reAttackAt( DEFNPC_REATTACKAT ), fleeDistance( DEFNPC_FLEEDISTANCE ), splitNum( DEFNPC_SPLIT ), splitChance( DEFNPC_SPLITCHANCE ), trainingPlayerIn( DEFNPC_TRAININGPLAYERIN ),
goldOnHand( DEFNPC_HOLDG ), questType( DEFNPC_QUESTTYPE ), questDestRegion( DEFNPC_QUESTDESTREGION ), questOrigRegion( DEFNPC_QUESTORIGREGION ),
petGuarding( nullptr ), npcFlag( DEFNPC_NPCFLAG ), boolFlags( DEFNPC_BOOLFLAG ), peaceing( DEFNPC_PEACEING ), provoing( DEFNPC_PROVOING ),
tamedHungerRate( DEFNPC_TAMEDHUNGERRATE ), tamedThirstRate( DEFNPC_TAMEDTHIRSTRATE ), hungerWildChance( DEFNPC_HUNGERWILDCHANCE ), 
thirstWildChance( DEFNPC_THIRSTWILDCHANCE ), walkingSpeed( DEFNPC_MOVEMENTSPEED ), runningSpeed( DEFNPC_MOVEMENTSPEED ), 
fleeingSpeed( DEFNPC_MOVEMENTSPEED ), pathFail( DEFNPC_PATHFAIL ), pathResult( DEFNPC_PATHRESULT ), pathTargX( DEFNPC_PATHTARGX ), pathTargY( DEFNPC_PATHTARGY ),
controlSlots( DEFNPC_CONTROLSLOTS ), maxLoyalty( DEFNPC_MAXLOYALTY ), loyalty( DEFNPC_LOYALTY ), orneriness( DEFNPC_ORNERINESS ), mountedWalkingSpeed( DEFNPC_MOVEMENTSPEED ),
mountedRunningSpeed( DEFNPC_MOVEMENTSPEED ), mountedFleeingSpeed( DEFNPC_MOVEMENTSPEED )
{
	fx[0] = fx[1] = fy[0] = fy[1] = DEFNPC_WANDERAREA;
	petFriends.resize( 0 );
	foodList.reserve( MAX_NAME );

	// Mark all characters as "non-awake" my default. This is used by UOX3 to see which
	// NPCs are permanently awake and need updating outside of region-based checks
	boolFlags.set( BIT_AWAKE, false );
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
const UI32			DEFCHAR_LASTMOVETIME		= 0;
//const UI16			DEFCHAR_NOMOVE 				= 0;
//const UI16			DEFCHAR_POISONCHANCE 		= 0;
const UI08			DEFCHAR_POISONSTRENGTH 		= 0;
const BodyType		DEFCHAR_BODYTYPE			= BT_OTHER;
const UI16			DEFCHAR_NPCGUILD			= 0;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CChar() constructor
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This function basically does what the name implies
//o------------------------------------------------------------------------------------------------o
CChar::CChar() : CBaseObject(),
bools( DEFCHAR_BOOLS ),
fontType( DEFCHAR_FONTTYPE ), maxHP( DEFCHAR_MAXHP ), maxHP_oldstr( DEFCHAR_MAXHP_OLDSTR ),
oldRace( DEFCHAR_OLDRACE ), maxMana( DEFCHAR_MAXMANA ), maxMana_oldint( DEFCHAR_MAXMANA_OLDINT ),
maxStam( DEFCHAR_MAXSTAM ), maxStam_olddex( DEFCHAR_MAXSTAM_OLDDEX ), sayColor( DEFCHAR_SAYCOLOUR ),
emoteColor( DEFCHAR_EMOTECOLOUR ), cell( DEFCHAR_CELL ), packItem( nullptr ),
targ( DEFCHAR_TARG ), attacker( DEFCHAR_ATTACKER ), hunger( DEFCHAR_HUNGER ), thirst( DEFCHAR_THIRST ), regionNum( DEFCHAR_REGIONNUM ), town( DEFCHAR_TOWN ),
advObj( DEFCHAR_ADVOBJ ), guildFealty( DEFCHAR_GUILDFEALTY ), guildNumber( DEFCHAR_GUILDNUMBER ), flag( DEFCHAR_FLAG ),
spellCast( DEFCHAR_SPELLCAST ), nextAct( DEFCHAR_NEXTACTION ), stealth( DEFCHAR_STEALTH ), running( DEFCHAR_RUNNING ),
raceGate( DEFCHAR_RACEGATE ), step( DEFCHAR_STEP ), priv( DEFCHAR_PRIV ), PoisonStrength( DEFCHAR_POISONSTRENGTH ), bodyType( DEFCHAR_BODYTYPE ), lastMoveTime( DEFCHAR_LASTMOVETIME ),
npcGuild( DEFCHAR_NPCGUILD )
{
	ownedItems.clear();
	itemLayers.clear();
	layerCtr = itemLayers.end();

	id			= 0x0190;
	objType		= OT_CHAR;
	name		= "Mr. noname";
	sectionId	= "UNKNOWN";

	memset( &regen[0],			0, sizeof( TIMERVAL )	* 3 );
	memset( &weathDamage[0],	0, sizeof( TIMERVAL )	* WEATHNUM );
	memset( &charTimers[0],		0, sizeof( TIMERVAL )	* tCHAR_COUNT );
	memset( &baseskill[0],		0, sizeof( SKILLVAL )	* ALLSKILLS );
	memset( &skill[0],			0, sizeof( SKILLVAL )	* ( INTELLECT + 1 ));

	//SetCanTrain( true );
	bools.set( BIT_TRAIN, true );

	//SetHungerStatus( true );
	bools.set( BIT_WILLHUNGER, true );
	//SetThirstStatus( true );
	bools.set( BIT_WILLTHIRST, true );

	skillUsed[0].reset();
	skillUsed[1].reset();
	updateTypes.reset();

	strength = dexterity = intelligence = 1;

	mPlayer	= nullptr;
	mNPC	= nullptr;

	//SetMaxHPFixed( false );
	bools.set( BIT_MAXHPFIXED, false );
	//SetMaxManaFixed( false );
	bools.set( BIT_MAXMANAFIXED, false );
	//SetMaxStamFixed( false );
	bools.set( BIT_MAXSTAMFIXED, false );
	//SetCanAttack( true );
	bools.set( BIT_CANATTACK, true );
	//SetKarmaLock( false );
	bools.set( BIT_KARMALOCK, false );
	//SetBrkPeaceChanceGain( 0 );
	brkPeaceChanceGain = 0;
	//SetBrkPeaceChance( 0 );
	brkPeaceChance = 0;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::~CChar() deconstructor
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Destructor to clean things up when deleted
//o------------------------------------------------------------------------------------------------o
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
	{
		mNPC = new NPCValues_st();
	}
}

void CChar::CreatePlayer( void )
{
	if( !IsValidPlayer() )
	{
		mPlayer = new PlayerValues_st();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsValidNPC()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks whether character is an NPC
//o------------------------------------------------------------------------------------------------o
bool CChar::IsValidNPC( void ) const
{
	return ( mNPC != nullptr );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsValidPlayer()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks whether character is a Player
//o------------------------------------------------------------------------------------------------o
bool CChar::IsValidPlayer( void ) const
{
	return ( mPlayer != nullptr );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetPathFail()
//|					CChar::SetPathFail()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set number of times Pathfinding has failed for an NPC - resets on success
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetPathFail( void ) const
{
	SI08 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathFail;
	return rVal;
}
void CChar::SetPathFail( SI08 newValue )
{
	if( IsValidNPC() )
	{
		mNPC->pathFail = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetPathResult()
//|					CChar::SetPathResult()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the end result of pathfinding, to pass on to onPathfindEnd event
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetPathResult( void ) const
{
	SI08 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathResult;
	return rVal;
}
void CChar::SetPathResult( SI08 newValue )
{
	if( IsValidNPC() )
	{
		mNPC->pathResult = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetPathTargX()
//|					CChar::SetPathTargX()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the final target X location NPC is pathfinding towards
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetPathTargX( void ) const
{
	UI16 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathTargX;
	return rVal;
}
void CChar::SetPathTargX( UI16 newValue )
{
	if( IsValidNPC() )
	{
		mNPC->pathTargX = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetPathTargY()
//|					CChar::SetPathTargY()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the final target Y location NPC is pathfinding towards
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetPathTargY( void ) const
{
	UI16 rVal = DEFNPC_PATHFAIL;
	rVal = mNPC->pathTargY;
	return rVal;
}
void CChar::SetPathTargY( UI16 newValue )
{
	if( IsValidNPC() )
	{
		mNPC->pathTargY = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetHunger()
//|					CChar::SetHunger()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set hunger level of the character
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetHunger( void ) const
{
	return hunger;
}
bool CChar::SetHunger( SI08 newValue )
{
	if( IsValidNPC() )
	{
		std::vector<UI16> scriptTriggers = GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				// If script returns false/0/nothing, prevent hunger from changing, and prevent
				// other scripts with event from running
				if( toExecute->OnHungerChange(( this ), hunger ) == 0 )
				{
					return false;
				}
			}
		}
	}

	hunger = newValue;
	UpdateRegion();

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function    -	CChar::DoHunger()
//|	Date        -	21. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate Hunger level of the character and do all related effects.
//o------------------------------------------------------------------------------------------------o
void CChar::DoHunger( CSocket *mSock )
{
	// Don't continue if hunger system is disabled
	if( !cwmWorldState->ServerData()->HungerSystemEnabled() )
		return;

	if ( !IsInvulnerable() ) // No need to do anything for invulnerable chars
	{
		UI16 hungerRate;
		SI16 hungerDamage;
		if( !IsNpc() && mSock != nullptr )	// Do Hunger for player chars
		{
			if( WillHunger() && GetCommandLevel() == CL_PLAYER  )
			{
				if( GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					if( Races->DoesHunger( GetRace() )) // prefer the hunger settings frome the race
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
								case 5:	mSock->SysMessage( 1222 );	break; // You are still stuffed from your last meal.
								case 4:	mSock->SysMessage( 1223 );	break; // You are not very hungry but could eat more.
								case 3:	mSock->SysMessage( 1224 );	break; // You are feeling fairly hungry.
								case 2:	mSock->SysMessage( 1225 );	break; // You are extremely hungry.
								case 1:	mSock->SysMessage( 1226 );	break; // You are very weak from starvation!
								case 0:	mSock->SysMessage( 1227 );	break; // You must eat very soon or you will die!
							}
						}
					}
					else if( GetHP() > 0 && hungerDamage > 0 )
					{
						mSock->SysMessage( 1228 ); // You are starving!
						if( Damage( hungerDamage, PHYSICAL ))
						{
							if( GetHP() <= 0 )
							{
								mSock->SysMessage( 1229 ); // You have died of starvation.
							}
						}
					}
					SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>( hungerRate )));
				}
			}
		}
		else if( IsNpc() && !IsTamed() && Races->DoesHunger( GetRace() ))
		{
			// Handle hunger for regular non-tame NPCs
			if( !WillHunger() || GetMounted() || GetStabled() )
				return;

			if( GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				hungerRate	 = Races->GetHungerRate( GetRace() );
				hungerDamage = Races->GetHungerDamage( GetRace() );

				if( GetHunger() > 0 )
				{
					DecHunger();
				}
				else if( GetHP() > 0 && hungerDamage > 0 )
				{
					[[maybe_unused]] bool retVal = Damage( hungerDamage, PHYSICAL );
				}
				SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>( hungerRate )));
			}
		}
		else if( IsTamed() && GetTamedHungerRate() > 0 )
		{
			// Handle hunger for pets
			if( !WillHunger() || GetMounted() || GetStabled() )
				return;

			// Pets don't hunger if owner is offline
			if( cwmWorldState->ServerData()->PetHungerOffline() == false )
			{
				CChar *owner = GetOwnerObj();
				if( !ValidateObject( owner ))
					return;

				if( !IsOnline(( *owner )))
					return;
			}

			if( GetTimer( tCHAR_HUNGER ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
			{
				// Get hungerrate for tamed creatures
				hungerRate = GetTamedHungerRate();

				if( GetHunger() > 0 )
				{
					// Make pet more hungry
					DecHunger();
				}
				else if( GetLoyalty() == 0 && static_cast<UI08>(RandomNum( 0, 100 )) <= GetTamedHungerWildChance() )
				{
					// Make pet go wild from hunger, but only if loyalty is zero
					CChar *owner = GetOwnerObj();
					if( ValidateObject( owner ))
					{
						// Reduce player's control slot usage by the amount of control slots taken up by the pet
						owner->SetControlSlotsUsed( std::max( 0, owner->GetControlSlotsUsed() - GetControlSlots() ));
					}

					// Release the pet
					Npcs->ReleasePet( this );
				}
				else if( GetLoyalty() == 0 )
				{
					// Pet is hungry. Reduce loyalty!
					SetLoyalty( std::max( 0, GetLoyalty() - 1 ));
				}

				// Set timer for next time pet should grow more hungry
				SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>( hungerRate )));
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::GetThirst()
//|                 CChar::SetThirst()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Get/Set Thirst level of the character
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetThirst( void ) const
{
	return thirst;
}

bool CChar::SetThirst( SI08 newValue )
{
	if( IsValidNPC() )
	{
		std::vector<UI16> scriptTriggers = GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript* toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				// If script returns false/0/nothing, prevent thirst from changing, and prevent
				// other scripts with event from running
				if( toExecute->OnThirstChange(( this ), thirst ) == 0 )
				{
					return false;
				}
			}
		}
	}

	thirst = newValue;
	UpdateRegion();

	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::DoThirst()
//| Date        -   6. June, 2021
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Calculate Thirst level of the character and do all related effects.
//o------------------------------------------------------------------------------------------------o
void CChar::DoThirst( CSocket* mSock )
{
	// Don't continue if thirst system is disabled
	if( !cwmWorldState->ServerData()->ThirstSystemEnabled() )
		return;

	if( !IsInvulnerable() ) // No need to do anything for invulnerable chars
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
								case 5: mSock->SysMessage( 2045 );		break; // You are still stuffed from your last drink.
								case 4: mSock->SysMessage( 2046 );		break; // You are not very thirsty, but could drink more.
								case 3: mSock->SysMessage( 2047 );		break; // You are feeling fairly thirsty.
								case 2: mSock->SysMessage( 2048 );		break; // You are extremely thirsty.
								case 1: mSock->SysMessage( 2049 );		break; // You are very weak from dehydration!
								case 0: mSock->SysMessage( 2050 );		break; // You must drink very soon or you will suffer from dehydration!
							}
						}
					}
					else if( GetStamina() > 1 && thirstDrain > 0 )
					{
						mSock->SysMessage( 2051 ); // You are severely dehydrated!
						SetStamina( std::max( static_cast<SI16>( 1 ), static_cast<SI16>( GetStamina() - thirstDrain )));
						if( GetStamina() <= 1 )
						{
							mSock->SysMessage( 2052 ); // You have no stamina because of dehydration.
						}
					}
					SetTimer( tCHAR_THIRST, BuildTimeValue( static_cast<R32>( thirstRate )));
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
						SetStamina( std::max( static_cast<SI16>( 1 ), static_cast<SI16>( GetStamina() - thirstDrain )));
					}
					SetTimer( tCHAR_THIRST, BuildTimeValue( static_cast<R32>( thirstRate )));
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

					if( !IsOnline(( *owner )))
						return;
				}

				if( GetTimer( tCHAR_THIRST ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
				{
					thirstRate = GetTamedThirstRate();

					if( GetThirst() > 0 )
					{
						DecThirst();
					}
					else if( static_cast<UI08>( RandomNum( 1, 100 )) <= GetTamedThirstWildChance() )
					{
						SetOwner( nullptr );
						SetThirst( 6 );
					}
					SetTimer( tCHAR_THIRST, BuildTimeValue( static_cast<R32>( thirstRate )));
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CheckPetOfflineTimeout()
//|	Date		-	21. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if the owner of the was offline for to long and remove him if so.
//o------------------------------------------------------------------------------------------------o
void CChar::CheckPetOfflineTimeout( void )
{
	if( IsTamed() && IsNpc() && GetNpcAiType() != AI_PLAYERVENDOR )
	{
		if( GetMounted() || GetStabled() )
			return;

		CChar *owner = GetOwnerObj();
		if( !ValidateObject( owner ))
		{
			SetOwner( nullptr ); // The owner is gone, so remove him
		}
		else
		{
			if( IsOnline(( *owner )))
				return; // The owner is still online, so leave it alone

			time_t currTime, lastOnTime;
			const UI32 offlineTimeout = static_cast<UI32>( cwmWorldState->ServerData()->PetOfflineTimeout() * 3600 * 24 );

			time( &currTime );
			lastOnTime = static_cast<time_t>( GetLastOnSecs() );

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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetTown()
//|					CChar::SetTown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	The town the character belongs to
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetTown( void ) const
{
	return town;
}
void CChar::SetTown( UI16 newValue )
{
	town = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetBodyType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the body type (human, elf, gargoyle, other) of a character
//o------------------------------------------------------------------------------------------------o
BodyType CChar::GetBodyType( void )
{
	BodyType retVal = BT_OTHER;
	switch( GetId() )
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetBrkPeaceChanceGain()
//|					CChar::SetBrkPeaceChanceGain()
//|	Date		-	25. Feb
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance gain to break peace
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetBrkPeaceChanceGain( void ) const
{
	if( IsNpc() )
	{
		return brkPeaceChanceGain;
	}
	else
	{
		return static_cast<UI08>( GetBaseSkill( PEACEMAKING ) / 10 );
	}
}
void CChar::SetBrkPeaceChanceGain( UI08 newValue )
{
	brkPeaceChanceGain = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetBrkPeaceChance()
//|					CChar::SetBrkPeaceChance()
//|	Date		-	25. Feb
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current chance to break peace
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetBrkPeaceChance( void ) const
{
	return brkPeaceChance;
}
void CChar::SetBrkPeaceChance( UI08 newValue )
{
	brkPeaceChance = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsUnicode()
//|					CChar::SetUnicode()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is speaking in unicode
//o------------------------------------------------------------------------------------------------o
bool CChar::IsUnicode( void ) const
{
	return bools.test( BIT_UNICODE );
}
void CChar::SetUnicode( bool newVal )
{
	bools.set( BIT_UNICODE, newVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsNpc()
//|					CChar::SetNpc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is an npc
//o------------------------------------------------------------------------------------------------o
bool CChar::IsNpc( void ) const
{
	return bools.test( BIT_NPC );
}
void CChar::SetNpc( bool newVal )
{
	bools.set( BIT_NPC, newVal );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsAwake()
//|					CChar::SetAwake()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character (NPC) is permanently awake and updating outside
//|					standard region-checks. If set, these NPCs are not updated as part of the 
//|					regular region update loop, but in a separate loop directly after
//o------------------------------------------------------------------------------------------------o
auto CChar::IsAwake( void ) const -> bool
{
	bool rVal = false;
	if( IsValidNPC() )
	{
		rVal = mNPC->boolFlags.test( BIT_AWAKE );
	}
	return rVal;
}
auto CChar::SetAwake( bool newVal ) -> void
{
	if( IsValidNPC() )
	{
		mNPC->boolFlags.set( BIT_AWAKE, newVal );
		auto alwaysAwakeChars = Npcs->GetAlwaysAwakeNPCs();
		if( newVal )
		{
			// NPC awake, add to awake list
			alwaysAwakeChars->Add( this );
		}
		else
		{
			// NPC not awake, remove from awake list
			alwaysAwakeChars->Remove( this );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsEvading()
//|					CChar::SetEvadeState()
//|	Date		-	04/02/2012
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is evading
//o------------------------------------------------------------------------------------------------o
bool CChar::IsEvading( void ) const
{
	return bools.test( BIT_EVADE );
}
void CChar::SetEvadeState( bool newVal )
{
	bools.set( BIT_EVADE, newVal );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsShop()
//|					CChar::SetShop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is a shopkeeper
//o------------------------------------------------------------------------------------------------o
bool CChar::IsShop( void ) const
{
	return bools.test( BIT_SHOP );
}
void CChar::SetShop( bool newVal )
{
	bools.set( BIT_SHOP, newVal );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsDead()
//|					CChar::SetDead()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is dead
//o------------------------------------------------------------------------------------------------o
bool CChar::IsDead( void ) const
{
	return bools.test( BIT_DEAD );
}
void CChar::SetDead( bool newValue )
{
	bools.set( BIT_DEAD, newValue );
	UpdateRegion();

	if( !IsNpc() )
	{
		if( GetVisible() != VT_VISIBLE && newValue == false )
		{
			SetVisible( VT_VISIBLE );
		}
		else if( newValue == true )
		{
			SetVisible( VT_GHOSTHIDDEN );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetCanAttack()
//|					CChar::SetCanAttack()
//|	Date		-	25. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the char can attack targets
//o------------------------------------------------------------------------------------------------o
bool CChar::GetCanAttack( void ) const
{
	return bools.test( BIT_CANATTACK );
}
void CChar::SetCanAttack( bool newValue )
{
	bools.set( BIT_CANATTACK, newValue );
	SetBrkPeaceChance( 0 );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetKarmaLock()
//|					CChar::SetKarmaLock()
//|	Date		-	8. Dec, 2024
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the char karma is locked
//o------------------------------------------------------------------------------------------------o
bool CChar::GetKarmaLock( void ) const
{
	return bools.test( BIT_KARMALOCK );
}
void CChar::SetKarmaLock( bool newValue )
{
	bools.set( BIT_KARMALOCK, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsAtWar()
//|					CChar::SetWar()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is at war
//o------------------------------------------------------------------------------------------------o
bool CChar::IsAtWar( void ) const
{
	return bools.test( BIT_ATWAR );
}
void CChar::SetWar( bool newValue )
{
	bools.set( BIT_ATWAR, newValue );
	UpdateRegion();

	if( !IsNpc() )
	{
		if( IsDead() && newValue == true )
		{
			SetVisible( VT_VISIBLE );
		}
		else if( IsDead() && newValue == false )
		{
			SetVisible( VT_GHOSTHIDDEN );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsPassive()
//|					CChar::SetPassive()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is passive in combat. Primarily used to
//|					allow players to "tab out" of combat and not hit back even though someone is
//|					hitting them
//o------------------------------------------------------------------------------------------------o
bool CChar::IsPassive( void ) const
{
	return bools.test( BIT_ISPASSIVE );
}
void CChar::SetPassive( bool newValue )
{
	bools.set( BIT_ISPASSIVE, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::HasStolen()
//|					CChar::HasStolen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character has stolen something in the last X minutes
//|					since their last death
//o------------------------------------------------------------------------------------------------o
auto CChar::HasStolen() -> bool
{
	return bools.test( BIT_HASSTOLEN );
}
auto CChar::HasStolen( bool newValue ) -> void
{
	bools.set( BIT_HASSTOLEN, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsOnHorse()
//|					CChar::SetOnHorse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns/Sets whether the character is on a horse
//o------------------------------------------------------------------------------------------------o
bool CChar::IsOnHorse( void ) const
{
	return bools.test( BIT_ONHORSE );
}
void CChar::SetOnHorse( bool newValue )
{
	bools.set( BIT_ONHORSE, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetTownTitle()
//|					CChar::SetTownTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the character's town info is displayed
//o------------------------------------------------------------------------------------------------o
bool CChar::GetTownTitle( void ) const
{
	return bools.test( BIT_TOWNTITLE );
}
void CChar::SetTownTitle( bool newValue )
{
	bools.set( BIT_TOWNTITLE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetReactiveArmour()
//|					CChar::SetReactiveArmour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the character has reactive armour
//o------------------------------------------------------------------------------------------------o
bool CChar::GetReactiveArmour( void ) const
{
	return bools.test( BIT_REACTIVEARMOUR );
}
void CChar::SetReactiveArmour( bool newValue )
{
	bools.set( BIT_REACTIVEARMOUR, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CanTrain()
//|					CChar::SetCanTrain()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character can train
//o------------------------------------------------------------------------------------------------o
bool CChar::CanTrain( void ) const
{
	return bools.test( BIT_TRAIN );
}
void CChar::SetCanTrain( bool newValue )
{
	bools.set( BIT_TRAIN, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CanBeHired()
//|					CChar::SetCanHire()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is available for hire
//o------------------------------------------------------------------------------------------------o
bool CChar::CanBeHired( void ) const
{
	return bools.test( BIT_HIRELING );
}
void CChar::SetCanHire( bool newValue )
{
	bools.set( BIT_HIRELING, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetGuildToggle()
//|					CChar::SetGuildToggle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets whether the character displays guild information
//o------------------------------------------------------------------------------------------------o
bool CChar::GetGuildToggle( void ) const
{
	return bools.test( BIT_GUILDTOGGLE );
}
void CChar::SetGuildToggle( bool newValue )
{
	bools.set( BIT_GUILDTOGGLE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsTamed()
//|					CChar::SetTamed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is tamed
//o------------------------------------------------------------------------------------------------o
bool CChar::IsTamed( void ) const
{
	return bools.test( BIT_TAMED );
}
void CChar::SetTamed( bool newValue )
{
	bools.set( BIT_TAMED, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsGuarded()
//|					CChar::SetGuarded()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is guarded
//o------------------------------------------------------------------------------------------------o
bool CChar::IsGuarded( void ) const
{
	return bools.test( BIT_GUARDED );
}
void CChar::SetGuarded( bool newValue )
{
	bools.set( BIT_GUARDED, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::CanRun()
//|					CChar::SetRun()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character can run
//o------------------------------------------------------------------------------------------------o
bool CChar::CanRun( void ) const
{
	return bools.test( BIT_RUN );
}
void CChar::SetRun( bool newValue )
{
	bools.set( BIT_RUN, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsPolymorphed()
//|					CChar::IsPolymorphed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is polymorphed
//o------------------------------------------------------------------------------------------------o
bool CChar::IsPolymorphed( void ) const
{
	return bools.test( BIT_POLYMORPHED );
}
void CChar::IsPolymorphed( bool newValue )
{
	bools.set( BIT_POLYMORPHED, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsIncognito()
//|					CChar::IsIncognito()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is incognito
//o------------------------------------------------------------------------------------------------o
bool CChar::IsIncognito( void ) const
{
	return bools.test( BIT_INCOGNITO );
}
void CChar::IsIncognito( bool newValue )
{
	bools.set( BIT_INCOGNITO, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsDisguised()
//|					CChar::IsDisguised()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is disguised
//o------------------------------------------------------------------------------------------------o
bool CChar::IsDisguised( void ) const
{
	return bools.test( BIT_DISGUISED );
}
void CChar::IsDisguised( bool newValue )
{
	bools.set( BIT_DISGUISED, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsUsingPotion()
//|					CChar::SetUsingPotion()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is using a potion
//o------------------------------------------------------------------------------------------------o
bool CChar::IsUsingPotion( void ) const
{
	return bools.test( BIT_USINGPOTION );
}
void CChar::SetUsingPotion( bool newVal )
{
	bools.set( BIT_USINGPOTION, newVal );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::MayLevitate()
//|					CChar::SetLevitate()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character can levitate - not yet in use
//o------------------------------------------------------------------------------------------------o
bool CChar::MayLevitate( void ) const
{
	return bools.test( BIT_MAYLEVITATE );
}
void CChar::SetLevitate( bool newValue )
{
	bools.set( BIT_MAYLEVITATE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::WillHunger()
//|					CChar::SetHungerStatus()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character will get hungry
//o------------------------------------------------------------------------------------------------o
bool CChar::WillHunger( void ) const
{
	return bools.test( BIT_WILLHUNGER );
}
void CChar::SetHungerStatus( bool newValue )
{
	bools.set( BIT_WILLHUNGER, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::WillThirst()
//|                 CChar::SetThirstStatus()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Returns/Sets whether the character will get thirsty
//o------------------------------------------------------------------------------------------------o
bool CChar::WillThirst(void) const
{
	return bools.test( BIT_WILLTHIRST );
}
void CChar::SetThirstStatus( bool newValue )
{
	bools.set( BIT_WILLTHIRST, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsMeditating()
//|					CChar::SetMeditating()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is meditating
//o------------------------------------------------------------------------------------------------o
bool CChar::IsMeditating( void ) const
{
	return bools.test( BIT_MEDITATING );
}
void CChar::SetMeditating( bool newValue )
{
	bools.set( BIT_MEDITATING, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsCasting()
//|					CChar::SetCasting()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is casting a spell
//o------------------------------------------------------------------------------------------------o
bool CChar::IsCasting( void ) const
{
	return bools.test( BIT_CASTING );
}
void CChar::SetCasting( bool newValue )
{
	bools.set( BIT_CASTING, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsJSCasting()
//|					CChar::SetJSCasting()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns/Sets whether the character is casting a spell via JS engine
//|	Notes		-	This is only a temporary measure until ALL code is switched over to JS code
//|					As it stands, it'll try and auto-direct cast if you set casting and spell timeouts
//o------------------------------------------------------------------------------------------------o
bool CChar::IsJSCasting( void ) const
{
	return bools.test( BIT_JSCASTING );
}
void CChar::SetJSCasting( bool newValue )
{
	bools.set( BIT_JSCASTING, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::InBuilding()
//|					CChar::SetInBuilding()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a building
//o------------------------------------------------------------------------------------------------o
bool CChar::InBuilding( void )
{
	return bools.test( BIT_INBUILDING );
}
void CChar::SetInBuilding( bool newValue )
{
	bools.set( BIT_INBUILDING, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::SetPeace()
//| Date		-	25.Feb, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Takes character out of combat state
//o------------------------------------------------------------------------------------------------o
void CChar::SetPeace( UI32 newValue )
{
	if( newValue > 0 )
	{
		SetCanAttack( false );
		if( IsAtWar() )
		{
			SetWar( false );
		}
		SetTarg( nullptr );
		SetAttacker( nullptr );
		SetTimer( tCHAR_PEACETIMER, BuildTimeValue( newValue ));
	}
	else
	{
		SetCanAttack( true );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::RemoveSelfFromOwner()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Removes character from its owner's pet (and follower) list
//o------------------------------------------------------------------------------------------------o
void CChar::RemoveSelfFromOwner( void )
{
	CChar *oldOwner = GetOwnerObj();
	if( ValidateObject( oldOwner ))
	{
		oldOwner->GetFollowerList()->Remove( this );
		oldOwner->GetPetList()->Remove( this );
		oldOwner->UpdateRegion();
	}
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AddSelfToOwner()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds character to its new owner's pet (and follower) list
//o------------------------------------------------------------------------------------------------o
void CChar::AddSelfToOwner( void )
{
	CChar *newOwner = GetOwnerObj();
	if( !ValidateObject( newOwner ))
	{
		SetTamed( false );
		SetNpcWander( WT_FREE );
	}
	else
	{
		newOwner->GetPetList()->Add( this );
		newOwner->AddFollower( this );
		newOwner->UpdateRegion();
		if( !CanBeHired() )
		{
			SetTamed( true );
		}
	}
	UpdateFlag( this );
	Dirty( UT_UPDATE );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetGuildFealty()
//|					CChar::SetGuildFealty()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a player's guild fealty - essentially, who the player
//|					votes for as the next guild master
//o------------------------------------------------------------------------------------------------o
UI32 CChar::GetGuildFealty( void ) const
{
	return guildFealty;
}
void CChar::SetGuildFealty( UI32 newValue )
{
	guildFealty = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetGuildTitle()
//|					CChar::SetGuildTitle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a player's guild title
//o------------------------------------------------------------------------------------------------o
std::string CChar::GetGuildTitle( void ) const
{
	return guildTitle;
}
void CChar::SetGuildTitle( const std::string &newValue )
{
	guildTitle = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetRegen()
//|					CChar::SetRegen()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets regeneration timers for player's health, mana and stamina
//o------------------------------------------------------------------------------------------------o
TIMERVAL CChar::GetRegen( UI08 part ) const
{
	return regen[part];
}
void CChar::SetRegen( TIMERVAL newValue, UI08 part )
{
	regen[part] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetWeathDamage()
//|					CChar::SetWeathDamage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timers for weather damage effects on character
//o------------------------------------------------------------------------------------------------o
TIMERVAL CChar::GetWeathDamage( UI08 part ) const
{
	return weathDamage[part];
}
void CChar::SetWeathDamage( TIMERVAL newValue, UI08 part )
{
	weathDamage[part] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetNextAct()
//|					CChar::SetNextAct()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's next spellcasting action
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetNextAct( void ) const
{
	return nextAct;
}
void CChar::SetNextAct( UI08 newVal )
{
	nextAct = newVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetTimer()
//|					CChar::SetTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets timer values for the character
//o------------------------------------------------------------------------------------------------o
TIMERVAL CChar::GetTimer( cC_TID timerId ) const
{
	TIMERVAL rValue = 0;
	if( timerId != tCHAR_COUNT )
	{
		rValue = charTimers[timerId];
	}
	return rValue;
}
void CChar::SetTimer( cC_TID timerId, TIMERVAL value )
{
	if( timerId != tCHAR_COUNT )
	{
		charTimers[timerId] = value;
		if( timerId == tNPC_SUMMONTIME || timerId == tCHAR_MURDERRATE || timerId == tCHAR_PEACETIMER )
		{
			UpdateRegion();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetPoisonStrength()
//|					CChar::SetPoisonStrength()
//|	Date		-	09/23/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the poison strength of the character - used to poison others in combat
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetPoisonStrength( void ) const
{
	return PoisonStrength;
}
void CChar::SetPoisonStrength( UI08 value )
{
	PoisonStrength = value;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetEmoteColour()
//|					CChar::SetEmoteColour()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's emote text colour
//o------------------------------------------------------------------------------------------------o
COLOUR CChar::GetEmoteColour( void ) const
{
	return emoteColor;
}
void CChar::SetEmoteColour( COLOUR newValue )
{
	emoteColor = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSayColour()
//|					CChar::SetSayColour()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's normal speech text colour
//o------------------------------------------------------------------------------------------------o
COLOUR CChar::GetSayColour( void ) const
{
	return sayColor;
}
void CChar::SetSayColour( COLOUR newValue )
{
	sayColor = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSkin()
//|					CChar::SetSkin()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's skin colour
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetSkin( void ) const
{
	return GetColour();
}
void CChar::SetSkin( UI16 value )
{
	SetColour( value );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetStealth()
//|					CChar::SetStealth()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's stealth walk counter
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetStealth( void ) const
{
	return stealth;
}
void CChar::SetStealth( SI08 newValue )
{
	stealth = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetCell()
//|					CChar::SetCell()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the cell in which a character is jailed
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetCell( void ) const
{
	return cell;
}
void CChar::SetCell( SI08 newVal )
{
	cell = newVal;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetRunning()
//|					CChar::SetRunning()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets whether the character is running
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetRunning( void ) const
{
	return running;
}
void CChar::SetRunning( UI08 newValue )
{
	running = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetStep()
//|					CChar::SetStep()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the footstep sequence for the character for the purpose of playing
//|					footstep sounds
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetStep( void ) const
{
	return step;
}
void CChar::SetStep( UI08 newValue )
{
	step = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetRegion()
//|					CChar::SetRegion()
//|					CChar::GetRegionNum()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the town region the character is in
//o------------------------------------------------------------------------------------------------o
CTownRegion *CChar::GetRegion( void ) const
{
	if( cwmWorldState->townRegions.find( regionNum ) == cwmWorldState->townRegions.end() )
	{
		return cwmWorldState->townRegions[0xFF];
	}
	return cwmWorldState->townRegions[regionNum];
}
void CChar::SetRegion( UI16 newValue )
{
	regionNum = newValue;
	UpdateRegion();
}
UI16 CChar::GetRegionNum( void ) const
{
	return regionNum;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetOldLocation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the old/previous location of the character
//o------------------------------------------------------------------------------------------------o
void CChar::SetOldLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	oldLocX = newX;
	oldLocY = newY;
	oldLocZ = newZ;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetLocation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the location of the character to match specified object
//o------------------------------------------------------------------------------------------------o
void CChar::SetLocation( const CBaseObject *toSet )
{
	if( toSet != nullptr )
	{
		SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber(), toSet->GetInstanceId() );
	}
}

void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway );
//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetLocation()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets the location of the character to a specific set of coordinates/world/instance
//o------------------------------------------------------------------------------------------------o
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceId )
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
	SetInstanceId( instanceId );
	CMultiObj *newMulti = FindMulti( newX, newY, newZ, world, instanceId );
	if( GetMultiObj() != newMulti )
	{
		SetMulti( newMulti );
	}
	if( IsNpc() )
	{
		InitializeWanderArea( this, 10, 10 );
	}
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	SetLocation( newX, newY, newZ, WorldNumber(), GetInstanceId() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPackItem()
//|					CChar::SetPackItem()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the main backpack object of the character
//o------------------------------------------------------------------------------------------------o
CItem * CChar::GetPackItem( void )
{
	if( packItem == nullptr )
	{
		CItem *tempItem = GetItemAtLayer( IL_PACKITEM );
		if( ValidateObject( tempItem ) && tempItem->GetType() == IT_CONTAINER )
		{
			packItem = tempItem;
		}
	}
	return packItem;
}
void CChar::SetPackItem( CItem *newVal )
{
	packItem = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CalcSerFromObj()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Return the serial of an object
//o------------------------------------------------------------------------------------------------o
SERIAL CalcSerFromObj( CBaseObject *mObj )
{
	SERIAL toReturn = INVALIDSERIAL;
	if( ValidateObject( mObj ))
	{
		toReturn = mObj->GetSerial();
	}
	return toReturn;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTarg()
//|					CChar::SetTarg()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Returns  character's current target
//o------------------------------------------------------------------------------------------------o
CChar *CChar::GetTarg( void ) const
{
	return CalcCharObjFromSer( targ );
}
void CChar::SetTarg( CChar *newTarg )
{
	targ = CalcSerFromObj( newTarg );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetAttacker()
//|					CChar::SetAttacker()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets character's current attacker
//o------------------------------------------------------------------------------------------------o
CChar *CChar::GetAttacker( void ) const
{
	return CalcCharObjFromSer( attacker );
}
void CChar::SetAttacker( CChar *newValue )
{
	attacker = CalcSerFromObj( newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetAdvObj()
//|					CChar::SetAdvObj()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the "morex" identifier value of the previous advancement used
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetAdvObj( void ) const
{
	return advObj;
}
void CChar::SetAdvObj( UI16 newValue )
{
	advObj = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetRaceGate()
//|					CChar::SetRaceGate()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the raceId value of a race gate
//o------------------------------------------------------------------------------------------------o
RACEID CChar::GetRaceGate( void ) const
{
	return raceGate;
}
void CChar::SetRaceGate( RACEID newValue )
{
	raceGate = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpellCast()
//|					CChar::SetSpellCast()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the spell ID of the next spell an NPC will be casting
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetSpellCast( void ) const
{
	return spellCast;
}
void CChar::SetSpellCast( SI08 newValue )
{
	spellCast = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPriv()
//|					CChar::SetPriv()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the character's special privileges
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetPriv( void ) const
{
	return static_cast<UI16>( priv.to_ulong() );
}
void CChar::SetPriv( UI16 newValue )
{
	priv = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetBaseSkill()
//|					CChar::SetBaseSkill()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a character's specified baseskill (without modifiers)
//o------------------------------------------------------------------------------------------------o
SKILLVAL CChar::GetBaseSkill( UI08 skillToGet ) const
{
	SKILLVAL rVal = 0;
	if( skillToGet < ALLSKILLS )
	{
		rVal = baseskill[skillToGet];
	}
	return rVal;
}
void CChar::SetBaseSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	if( skillToSet < ALLSKILLS )
	{
		baseskill[skillToSet] = newSkillValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSkill()
//|					CChar::SetSkill()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets a character's specified skill (with modifiers)
//o------------------------------------------------------------------------------------------------o
SKILLVAL CChar::GetSkill( UI08 skillToGet ) const
{
	SKILLVAL rVal = 0;
	if( skillToGet <= INTELLECT )
	{
		rVal			= skill[skillToGet];
		SI32 modifier	= Races->DamageFromSkill( skillToGet, race );
		if( modifier != 0 )
		{
			SKILLVAL toAdd	= static_cast<SKILLVAL>( static_cast<R32>( skill[skillToGet] ) * ( static_cast<R32>( modifier ) / 1000 ));		// percentage to add
			rVal			+= toAdd; // return the bonus
		}
	}
	return rVal;
}
void CChar::SetSkill( SKILLVAL newSkillValue, UI08 skillToSet )
{
	if( skillToSet <= INTELLECT )
	{
		skill[skillToSet] = newSkillValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetGuildNumber()
//|					CChar::SetGuildNumber()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets ID of guild character belongs to
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetGuildNumber( void ) const
{
	return guildNumber;
}
void CChar::SetGuildNumber( SI16 newValue )
{
	guildNumber = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFlag()
//|					CChar::SetFlag()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets flag for character (criminal, innocent, murderer, etc)
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetFlag( void ) const
{
	return flag;
}
void CChar::SetFlag( UI08 newValue )
{
	flag = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFontType()
//|					CChar::SetFontType()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the font type used by the character for speech
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetFontType( void ) const
{
	return fontType;
}
void CChar::SetFontType( SI08 newType )
{
	fontType = newType;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsGM()
//|					CChar::SetGM()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character has GM privileges
//o------------------------------------------------------------------------------------------------o
bool CChar::IsGM( void ) const
{
	return priv.test( BIT_GM );
}
void CChar::SetGM( bool newValue )
{
	priv.set( BIT_GM, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::CanBroadcast()
//|					CChar::SetBroadcast()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character is allowed to broadcast messages globally
//o------------------------------------------------------------------------------------------------o
bool CChar::CanBroadcast( void ) const
{
	return priv.test( BIT_BROADCAST );
}
void CChar::SetBroadcast( bool newValue )
{
	priv.set( BIT_BROADCAST, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsInvulnerable()
//|					CChar::SetInvulnerable()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Set whether the character is invulnerable
//o------------------------------------------------------------------------------------------------o
bool CChar::IsInvulnerable( void ) const
{
	return priv.test( BIT_INVULNERABLE );
}
void CChar::SetInvulnerable( bool newValue )
{
	priv.set( BIT_INVULNERABLE, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSingClickSer()
//|					CChar::SetSingClickSer()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether player is allowed to see serials of objects clicked
//o------------------------------------------------------------------------------------------------o
bool CChar::GetSingClickSer( void ) const
{
	return priv.test( BIT_SINGCLICKSER );
}
void CChar::SetSingClickSer( bool newValue )
{
	priv.set( BIT_SINGCLICKSER, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::NoSkillTitles()
//|					CChar::SetSkillTitles()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether skill titles will show up for character
//o------------------------------------------------------------------------------------------------o
bool CChar::NoSkillTitles( void ) const
{
	return priv.test( BIT_SKILLTITLES );
}
void CChar::SetSkillTitles( bool newValue )
{
	priv.set( BIT_SKILLTITLES, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsGMPageable()
//|					CChar::SetGMPageable()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character can answer GM pages
//o------------------------------------------------------------------------------------------------o
bool CChar::IsGMPageable( void ) const
{
	return priv.test( BIT_GMPAGEABLE );
}
void CChar::SetGMPageable( bool newValue )
{
	priv.set( BIT_GMPAGEABLE, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::CanSnoop()
//|					CChar::SetSnoop()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character can snoop in the backpacks of other players and NPCs
//o------------------------------------------------------------------------------------------------o
bool CChar::CanSnoop( void ) const
{
	return priv.test( BIT_SNOOP );
}
void CChar::SetSnoop( bool newValue )
{
	priv.set( BIT_SNOOP, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsCounselor()
//|					CChar::SetCounselor()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character has Counselor privileges
//o------------------------------------------------------------------------------------------------o
bool CChar::IsCounselor( void ) const
{
	return priv.test( BIT_COUNSELOR );
}
void CChar::SetCounselor( bool newValue )
{
	priv.set( BIT_COUNSELOR, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AllMove()
//|					CChar::SetAllMove()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets character can move all items freely, regardless of their moveable state
//o------------------------------------------------------------------------------------------------o
bool CChar::AllMove( void ) const
{
	return priv.test( BIT_ALLMOVE );
}
void CChar::SetAllMove( bool newValue )
{
	priv.set( BIT_ALLMOVE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsFrozen()
//|					CChar::SetFrozen()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character is in frozen state
//o------------------------------------------------------------------------------------------------o
bool CChar::IsFrozen( void ) const
{
	return priv.test( BIT_FROZEN );
}
void CChar::SetFrozen( bool newValue )
{
	priv.set( BIT_FROZEN, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ViewHouseAsIcon()
//|					CChar::SetViewHouseAsIcon()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether the character views icons/deeds instead of actual houses
//o------------------------------------------------------------------------------------------------o
bool CChar::ViewHouseAsIcon( void ) const
{
	return priv.test( BIT_VIEWHOUSEASICON );
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	priv.set( BIT_VIEWHOUSEASICON, newValue );
}

// 0x0800 is free
//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::NoNeedMana()
//|					CChar::SetNoNeedMana()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether mana is required for spellcasting for this character
//o------------------------------------------------------------------------------------------------o
bool CChar::NoNeedMana( void ) const
{
	return priv.test( BIT_NONEEDMANA );
}
void CChar::SetNoNeedMana( bool newValue )
{
	priv.set( BIT_NONEEDMANA, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsDispellable()
//|					CChar::SetDispellable()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether this character is dispellable using dispel spell (i.e. it was summoned)
//o------------------------------------------------------------------------------------------------o
bool CChar::IsDispellable( void ) const
{
	return priv.test( BIT_DISPELLABLE );
}
void CChar::SetDispellable( bool newValue )
{
	priv.set( BIT_DISPELLABLE, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsTempReflected()
//|					CChar::SetTempReflected()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character is temporarily protected by the magic reflect spell
//o------------------------------------------------------------------------------------------------o
bool CChar::IsTempReflected( void ) const
{
	return priv.test( BIT_TEMPREFLECTED );
}
void CChar::SetTempReflected( bool newValue )
{
	priv.set( BIT_TEMPREFLECTED, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsPermReflected()
//|					CChar::SetPermReflected()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether character is permanently protected by the magic reflect spell
//o------------------------------------------------------------------------------------------------o
bool CChar::IsPermReflected( void ) const
{
	return priv.test( BIT_PERMREFLECTED );
}
void CChar::SetPermReflected( bool newValue )
{
	priv.set( BIT_PERMREFLECTED, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::NoNeedReags()
//|					CChar::SetNoNeedReags()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Checks/Sets whether magical reagents are required for spellcasting for this character
//o------------------------------------------------------------------------------------------------o
bool CChar::NoNeedReags( void ) const
{
	return priv.test( BIT_NONEEDREAGS );
}
void CChar::SetNoNeedReags( bool newValue )
{
	priv.set( BIT_NONEEDREAGS, newValue );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::Dupe()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Duplicate character - used for splitting NPCs when hit in combat
//o------------------------------------------------------------------------------------------------o
CChar *CChar::Dupe( void )
{
	CChar *target = static_cast<CChar *>(ObjectFactory::GetSingleton().CreateObject( OT_CHAR ));
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
	target->SetId( id );
	target->SetSkin( GetColour() );
	target->SetFontType( fontType );
	target->SetSayColour( sayColor );
	target->SetEmoteColour( emoteColor );
	target->SetStrength( strength );
	target->SetDexterity( dexterity );
	target->SetIntelligence( intelligence );
	if( GetMaxHPFixed() )
	{
		target->SetFixedMaxHP( maxHP );
	}
	if( GetMaxManaFixed() )
	{
		target->SetFixedMaxMana( maxMana );
	}
	if( GetMaxStamFixed() )
	{
		target->SetFixedMaxStam( maxStam );
	}
	target->SetHP( hitpoints );
	target->SetStamina( stamina );
	target->SetMana( mana );

	target->SetStrength2( st2 );
	target->SetDexterity2( dx2 );
	target->SetIntelligence2( in2 );

	target->SetHiDamage( hiDamage );
	target->SetLoDamage( loDamage );

	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		target->SetBaseSkill( baseskill[i], i );
		target->SetSkill( skill[i], i );
	}

	target->SetCell( cell );
	target->SetPackItem( packItem );
	target->SetWeight( weight );
	target->SetResist( GetResist( PHYSICAL), PHYSICAL );
	target->SetTarg( GetTarg() );
	target->SetRegen( regen[0], 0 );
	target->SetRegen( regen[1], 1 );
	target->SetRegen( regen[2], 2 );
	target->SetAttacker( GetAttacker() );
	target->SetVisible( GetVisible() );

	for( SI32 mTID = static_cast<SI32>( tCHAR_TIMEOUT ); mTID < static_cast<SI32>( tCHAR_COUNT ); ++mTID )
	{
		target->SetTimer( static_cast<cC_TID>( mTID ), GetTimer( static_cast<cC_TID>( mTID )));
	}
	target->SetHunger( hunger );
	target->SetThirst( thirst );
	target->SetBrkPeaceChance( GetBrkPeaceChance() );
	target->SetBrkPeaceChanceGain( GetBrkPeaceChanceGain() );
	target->SetRegion( regionNum );
	target->SetTown( town );
	target->SetAdvObj( advObj );
	target->SetDisabled( IsDisabled() );
	target->SetCanHire( CanBeHired() );
	target->SetCanTrain( CanTrain() );
	target->SetLastOn( GetLastOn() );
	target->SetLastOnSecs( GetLastOnSecs() );
	target->SetPlayTime( GetPlayTime() );
	target->SetGuildTitle( guildTitle );
	target->SetGuildFealty( guildFealty );
	target->SetGuildNumber( guildNumber );
	target->SetFlag( flag );
	target->SetCasting( IsCasting() );
	target->SetJSCasting( IsJSCasting() );
	target->SetSpellCast( spellCast );
	target->SetNextAct( nextAct );
	target->SetSquelched( GetSquelched() );
	target->SetMeditating( IsMeditating() );
	target->SetStealth( stealth );
	target->SetRunning( running );
	target->SetRace( GetRace() );
	target->SetRaceGate( raceGate );
	target->SetCarve( carve );
	target->SetNPCGuild( npcGuild );
	target->SetNPCGuildJoined( npcGuild );
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
		target->SetNPCAiType( GetNpcAiType() );
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
		target->SetOrgId( GetOrgId() );
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
		target->SetCreatedOn( GetCreatedOn() );
	}

	// Add any script triggers present on object to the new object
	target->scriptTriggers = GetScriptTriggers();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::FlagColour()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Return character flag, which determines the color the character highlights in
//o------------------------------------------------------------------------------------------------o
FlagColors CChar::FlagColour( CChar *toCompare )
{
	FlagColors retVal = FC_INNOCENT;

	GUILDRELATION gComp	= GR_UNKNOWN;
	RaceRelate rComp	= RACE_NEUTRAL;

	if( ValidateObject( toCompare ))
	{
		if( !IsIncognito() )
		{
			gComp	= GuildSys->Compare( this, toCompare );
			rComp	= Races->Compare( this, toCompare );
		}
	}

	if( IsInvulnerable() )
	{
		retVal = FC_INVULNERABLE;
	}
	else if( IsMurderer() )
	{
		retVal = FC_MURDERER;
	}
	else if( IsCriminal() || HasStolen() || CheckPermaGreyFlag( toCompare->GetSerial() ))
	{
		retVal = FC_CRIMINAL;
	}
	else if( IsNeutral() || CheckAggressorFlag( toCompare->GetSerial() ))
	{
		retVal = FC_NEUTRAL;
	}
	else if( rComp != RACE_NEUTRAL || gComp != GR_UNKNOWN )
	{
		if( gComp == GR_ALLY || gComp == GR_SAME || rComp >= RACE_ALLY )
		{
			retVal = FC_FRIEND;
		}
		else if( gComp == GR_WAR || rComp <= RACE_ENEMY )
		{
			retVal = FC_ENEMY;
		}
	}
	return retVal;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::RemoveFromSight()
//|	Date		-	April 7th, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all online chars and removes the character from their sight
//o------------------------------------------------------------------------------------------------o
void CChar::RemoveFromSight( CSocket *mSock )
{
	CPRemoveItem toSend = ( *this );
	if( mSock != nullptr )
	{
		mSock->Send( &toSend );
	}
	else
	{
		for( auto &tSock : FindPlayersInOldVisrange( this ))
		{
			if( tSock->LoginComplete() )
			{
				if( tSock->CurrcharObj() != this )
				{
					tSock->Send( &toSend );
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::RemoveAllObjectsFromSight()
//|	Date		-	July 1st, 2020
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through nearby objects and removes them from the player's sight
//o------------------------------------------------------------------------------------------------o
auto CChar::RemoveAllObjectsFromSight( CSocket *mSock ) -> void
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

		for( auto &MapArea : MapRegion->PopulateList( this ))
		{
			if( MapArea == nullptr )
				continue;

			// First remove nearby characters from sight
			auto regChars = MapArea->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( !ValidateObject( tempChar ) || tempChar->GetInstanceId() != this->GetInstanceId() )
					continue;

				CPRemoveItem charToSend = ( *tempChar );
				auto tempX = tempChar->GetX();
				auto tempY = tempChar->GetY();
						
				if( this != tempChar && ( tempX >= minX && tempX <= maxX && tempY >= minY && tempY <= maxY ) &&
					( IsOnline(( *tempChar )) || tempChar->IsNpc() ||
					( IsGM() && cwmWorldState->ServerData()->ShowOfflinePCs() )))
				{
					mSock->Send( &charToSend );
				}
			}

			// Now remove nearby items and multis from sight
			auto regItems = MapArea->GetItemList();
			for( const auto &tempItem : regItems->collection() )
			{
				if( !ValidateObject( tempItem ) || tempItem->GetInstanceId() != this->GetInstanceId() )
					continue;

				CPRemoveItem itemToSend = ( *tempItem );
				auto tempItemX = tempItem->GetX();
				auto tempItemY = tempItem->GetY();
						
				if( tempItem->CanBeObjType( OT_MULTI ))
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
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::SendToSocket()
//|	Date		-	April 7th, 2000
//|	Modified	-	(June 16, 2003)
//|						Got rid of array based packet sending, replaced with
//|						CPDrawObject, simplifying logic
//o------------------------------------------------------------------------------------------------o
//| Purpose     -	Sends the information about this person to socket S
//|					IF in range.  Essentially a replacement for impowncreate
//o------------------------------------------------------------------------------------------------o
void CChar::SendToSocket( CSocket *s, bool drawGamePlayer )
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

		if( mCharObj == this && drawGamePlayer && ( mCharObj->GetVisible() == 0 || mCharObj->IsDead() ))
		{
			// Only send this when updating after a teleport/world change
			CPDrawGamePlayer gpToSend(( *this ));
			s->Send( &gpToSend );

			SendWornItems( s );
		}
		else if( GetVisible() == VT_GHOSTHIDDEN && !mCharObj->IsDead() && GetCommandLevel() >= mCharObj->GetCommandLevel() )
		{
			return;
		}
		else if( mCharObj != this && GetCommandLevel() >= mCharObj->GetCommandLevel() 
			&& (( GetVisible() != VT_VISIBLE && GetVisible() != VT_GHOSTHIDDEN ) || ( !IsNpc() && !IsOnline(( *this )))))
		{
			return;
		}

		CPDrawObject toSend(( *this ));

		toSend.SetRepFlag( static_cast<UI08>( FlagColour( mCharObj )));

		for( LAYERLIST_ITERATOR lIter = itemLayers.begin(); lIter != itemLayers.end(); ++lIter )
		{
			if( ValidateObject( lIter->second ))
			{
				toSend.AddItem( lIter->second, alwaysSendItemHue );
			}
		}

		toSend.Finalize();
		s->Send( &toSend );

		if( s->ClientType() >= CV_SA2D )
		{
			// Send poison state of healthbar
			CPHealthBarStatus hpBarStatus1(( *this ), ( *s ), 1 );
			s->Send( &hpBarStatus1 );

			// Send invulnerable state of healthbar
			CPHealthBarStatus hpBarStatus2(( *this ), ( *s ), 2 );
			s->Send( &hpBarStatus2 );
		}

		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			CPToolTip pSend( GetSerial(), s );
			s->Send( &pSend );
		}
	}
}

void CheckRegion( CSocket *mSock, CChar& mChar, bool forceUpdateLight );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::Teleport()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates teleporting player and sends nearby objects after teleporting
//o------------------------------------------------------------------------------------------------o
auto CChar::Teleport() -> void
{
	CSocket *mSock = GetSocket();
	RemoveFromSight();
	Update( nullptr, true );
	if( mSock != nullptr )
	{
		UI16 visrange = mSock->Range() + Races->VisRange( GetRace() );
		mSock->StatWindow( this );
		mSock->WalkSequence( -1 );

		UI16 mCharX = this->GetX();
		UI16 mCharY = this->GetY();
		auto minX = mCharX - visrange;
		auto minY = mCharY - visrange;
		auto maxX = mCharX + visrange;
		auto maxY = mCharY + visrange;

		for( auto &MapArea : MapRegion->PopulateList( this ))
		{
			if( MapArea == nullptr )
				continue;

			auto regChars = MapArea->GetCharList();
			for( const auto tempChar : regChars->collection() )
			{
				if( !ValidateObject( tempChar ) || tempChar->GetInstanceId() != this->GetInstanceId() )
					continue;

				auto tempX = tempChar->GetX();
				auto tempY = tempChar->GetY();
				if( this != tempChar && ( tempX >= minX && tempX <= maxX && tempY >= minY && tempY <= maxY ) &&
					( IsOnline(( *tempChar )) || tempChar->IsNpc() ||
					( IsGM() && cwmWorldState->ServerData()->ShowOfflinePCs() )))
				{
					tempChar->SendToSocket( mSock );
				}
			}
			auto regItems = MapArea->GetItemList();
			for( const auto tempItem : regItems->collection() )
			{
				if( !ValidateObject( tempItem ) || tempItem->GetInstanceId() != this->GetInstanceId() )
					continue;

				auto tempItemX = tempItem->GetX();
				auto tempItemY = tempItem->GetY();
				if( tempItem->CanBeObjType( OT_MULTI ))
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
	}
	CheckCharInsideBuilding( this, mSock, false );

	bool forceWeatherupdate = true;

	if( ValidateObject( GetMultiObj() ))
	{
		if( GetMultiObj()->CanBeObjType( OT_BOAT ))
		{
			//Don't force a weather update while on boat to prevent spam.
			forceWeatherupdate = false;
		}
	}

	CheckRegion( mSock, ( *this ), forceWeatherupdate );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::ExposeToView()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes character visible
//o------------------------------------------------------------------------------------------------o
void CChar::ExposeToView( void )
{
	SetVisible( VT_VISIBLE );
	SetStealth( -1 );

	// hide it from ourselves, we want to show ourselves to everyone in range
	// now we tell everyone we exist
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::Update()
//|	Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends update to all those in range
//o------------------------------------------------------------------------------------------------o
void CChar::Update( CSocket *mSock, bool drawGamePlayer, bool sendToSelf )
{
	if( mSock != nullptr )
	{
		SendToSocket( mSock, drawGamePlayer );
	}
	else
	{
		for( auto &tSock :FindPlayersInVisrange( this ))
		{
			if( tSock->LoginComplete() )
			{
				if(( tSock->CurrcharObj()== this ) && sendToSelf )
				{
					SendToSocket( tSock, drawGamePlayer );
				}
				SendToSocket( tSock, drawGamePlayer );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetItemAtLayer()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the item at layer Layer on paperdoll
//o------------------------------------------------------------------------------------------------o
CItem *CChar::GetItemAtLayer( ItemLayers Layer )
{
	CItem *rVal = nullptr;
	LAYERLIST_ITERATOR lIter = itemLayers.find( Layer );
	if( lIter != itemLayers.end() )
	{
		rVal = lIter->second;
	}
	return rVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::WearItem()
//|	Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Wears the item toWear and adjusts the stats if any are
//|					required to change.  Returns true if successfully equipped
//o------------------------------------------------------------------------------------------------o
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

	bool rValue = true;
	ItemLayers tLayer = toWear->GetLayer();
	if( tLayer != IL_NONE )	// Layer == 0 is a special case, for things like trade windows and such
	{
		if( ValidateObject( GetItemAtLayer( tLayer )))
		{
#if defined( UOX_DEBUG_MODE )
			std::string charName = GetNpcDictName( this, nullptr, NRS_SYSTEM );
			Console.Warning( oldstrutil::format( "Failed to equip item %s(0x%X) to layer 0x%X on character %s(0x%X, from section [%s]) - another item (%s) is already equipped in that layer!", toWear->GetName().c_str(), toWear->GetSerial(), tLayer, charName.c_str(), serial, GetSectionId().c_str(), GetItemAtLayer( tLayer )->GetName().c_str() ));
#endif
			rValue = false;
		}
		else
		{
			itemLayers[tLayer] = toWear;

			IncStrength2( itemLayers[tLayer]->GetStrength2() );
			IncDexterity2( itemLayers[tLayer]->GetDexterity2() );
			IncIntelligence2( itemLayers[tLayer]->GetIntelligence2() );

			if( toWear->IsPostLoaded() )
			{
				if( itemLayers[tLayer]->GetPoisoned() )
				{
					SetPoisoned( GetPoisoned() + itemLayers[tLayer]->GetPoisoned() );	// should be +, not -
				}

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
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::TakeOffItem()
//|	Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Takes the item off the character's paperdoll, ensuring that any stat
//|					adjustments are made. Returns true if successfully unequipped
//o------------------------------------------------------------------------------------------------o
bool CChar::TakeOffItem( ItemLayers Layer )
{
	bool rValue = false;
	if( ValidateObject( GetItemAtLayer( Layer )))
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
		{
			SetPackItem( nullptr );
		}
		IncStrength2( -itemLayers[Layer]->GetStrength2() );
		IncDexterity2( -itemLayers[Layer]->GetDexterity2() );
		IncIntelligence2( -itemLayers[Layer]->GetIntelligence2() );
		if( itemLayers[Layer]->GetPoisoned() )
		{
			if( itemLayers[Layer]->GetPoisoned() > GetPoisoned() )
			{
				SetPoisoned( 0 );
			}
			else
			{
				SetPoisoned( GetPoisoned() - itemLayers[Layer]->GetPoisoned() );
			}
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
		rValue = true;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::FirstItem()
//|	Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the item reference for the first item on paperdoll
//o------------------------------------------------------------------------------------------------o
CItem *CChar::FirstItem( void )
{
	CItem *rVal = nullptr;

	layerCtr = itemLayers.begin();
	if( !FinishedItems() )
	{
		rVal = layerCtr->second;
	}
	return rVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::NextItem()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the item reference for the next item on paperdoll
//o------------------------------------------------------------------------------------------------o
CItem *CChar::NextItem( void )
{
	CItem *rVal = nullptr;
	++layerCtr;
	if( !FinishedItems() )
	{
		rVal = layerCtr->second;
	}
	return rVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::FinishedItems()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if there are no more items on the paperdoll
//o------------------------------------------------------------------------------------------------o
bool CChar::FinishedItems( void )
{
	return ( layerCtr == itemLayers.end() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::DumpHeader()
//|					CChar::DumpBody()
//|					CChar::NPCValues_st::DumpBody()
//|					CChar::PlayerValues_st::DumpBody()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Dump character data to worldfile
//o------------------------------------------------------------------------------------------------o
bool CChar::DumpHeader( std::ostream &outStream ) const
{
	outStream << "[CHARACTER]" << '\n';
	return true;
}
bool CChar::DumpBody( std::ostream &outStream ) const
{
	CBaseObject::DumpBody( outStream );	// Make the default save of BaseObject members now

	const char newLine = '\n';

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "GuildFealty=0x" << GetGuildFealty() << newLine;
	outStream << "Speech=0x" << GetSayColour() << ",0x" << GetEmoteColour() << newLine;
	outStream << "Privileges=0x" << GetPriv() << newLine;
	if( ValidateObject( packItem ))
	{
		outStream << "PackItem=0x" << packItem->GetSerial() << newLine;	// store something meaningful
	}
	else
	{
		outStream << "PackItem=0x" << INVALIDSERIAL << newLine;
	}

	// Decimal / String Values
	outStream << std::dec;
	outStream << "GuildTitle=" + GetGuildTitle() + newLine;
	outStream << "Hunger=" + std::to_string( GetHunger() ) + newLine;
	outStream << "Thirst=" + std::to_string( GetThirst() ) + newLine;
	outStream << "BrkPeaceChanceGain=" + std::to_string( GetBrkPeaceChanceGain() ) + newLine;
	outStream << "BrkPeaceChance=" + std::to_string( GetBrkPeaceChance() ) + newLine;
	if ( GetMaxHPFixed() )
	{
		outStream << "MAXHP=" + std::to_string( maxHP ) + newLine;
	}
	if ( GetMaxManaFixed() )
	{
		outStream << "MAXMANA=" + std::to_string( maxMana ) + newLine;
	}
	if ( GetMaxStamFixed() )
	{
		outStream << "MAXSTAM=" + std::to_string( maxStam ) + newLine;
	}
	outStream << "Town=" + std::to_string( GetTown() ) + newLine;
	outStream << "SummonTimer=" + std::to_string( GetTimer( tNPC_SUMMONTIME )) + newLine;
	outStream << "MayLevitate=" + std::to_string(( MayLevitate() ? 1 : 0 )) + newLine;
	outStream << "Stealth=" + std::to_string( GetStealth() ) + newLine;
	outStream << "Reserved=" + std::to_string( GetCell() ) + newLine;
	outStream << "Region=" + std::to_string( GetRegionNum() ) + newLine;
	outStream << "AdvanceObject=" + std::to_string( GetAdvObj() ) + newLine;
	outStream << "AdvRaceObject=" + std::to_string( GetRaceGate() ) + newLine;

	// Write out the BaseSkills and the SkillLocks here
	// Format: BaseSkills=[0,34]-[1,255]-[END]
	outStream << "BaseSkills=";
	for( UI08 bsc = 0; bsc < ALLSKILLS; ++bsc )
	{
		outStream << "[" + std::to_string( bsc ) + "," + std::to_string( GetBaseSkill( bsc )) + "]-";
	}
	outStream << "[END]" << newLine;

	outStream << "GuildNumber=" + std::to_string( GetGuildNumber() ) + newLine;
	outStream << "NpcGuild=" + std::to_string( GetNPCGuild() ) + newLine;
	outStream << "NpcGuildJoined=" + std::to_string( GetNPCGuildJoined() ) + newLine;
	outStream << "FontType=" + std::to_string( GetFontType() ) + newLine;
	outStream << "TownTitle=" + std::to_string(( GetTownTitle() ? 1 : 0 )) + newLine;
	//-------------------------------------------------------------------------------------------
	outStream << "CanRun=" + std::to_string((( CanRun() && IsNpc() ) ? 1 : 0 )) + newLine;
	outStream << "CanAttack=" + std::to_string(( GetCanAttack() ? 1 : 0 )) + newLine;
	outStream << "KarmaLock=" + std::to_string(( GetKarmaLock() ? 1 : 0 )) + newLine;
	outStream << "AllMove=" + std::to_string(( AllMove() ? 1 : 0 )) + newLine;
	outStream << "IsNpc=" + std::to_string(( IsNpc() ? 1 : 0 )) + newLine;
	outStream << "IsShop=" + std::to_string(( IsShop() ? 1 : 0 )) + newLine;
	outStream << "Dead=" + std::to_string(( IsDead() ? 1 : 0 )) + newLine;
	outStream << "CanBeHired=" + std::to_string( CanBeHired() ? 1 : 0 ) + newLine;
	outStream << "CanTrain=" + std::to_string(( CanTrain() ? 1 : 0 )) + newLine;
	outStream << "IsWarring=" + std::to_string(( IsAtWar() ? 1 : 0 )) + newLine;
	outStream << "GuildToggle=" + std::to_string(( GetGuildToggle() ? 1 : 0 )) + newLine;
	outStream << "PoisonStrength=" + std::to_string( GetPoisonStrength() ) + newLine;
	outStream << "WillHunger=" + std::to_string(( WillHunger() ? 1 : 0 )) + newLine;

	TIMERVAL mTime = GetTimer( tCHAR_MURDERRATE );
	outStream << "MurderTimer=";
	if( mTime == 0 || mTime < cwmWorldState->GetUICurrentTime() )
	{
		outStream << std::to_string( 0 ) + newLine;
	}
	else
	{
		outStream << std::to_string( mTime - cwmWorldState->GetUICurrentTime() ) + newLine;
	}

	TIMERVAL pTime = GetTimer( tCHAR_PEACETIMER );
	outStream << "PeaceTimer=";
	if( pTime == 0 || pTime < cwmWorldState->GetUICurrentTime() )
	{
		outStream << std::to_string( 0 ) + newLine;
	}
	else
	{
		outStream << std::to_string( pTime - cwmWorldState->GetUICurrentTime() ) + newLine;
	}

	if( IsValidPlayer() )
	{
		mPlayer->DumpBody( outStream );
	}
	if( IsValidNPC() )
	{
		mNPC->DumpBody( outStream );
	}
	return true;
}
void CChar::NPCValues_st::DumpBody( std::ostream& outStream )
{
	const char newLine = '\n';

	// Hexadecimal Values
	outStream << std::hex;

	// Decimal / String Values
	outStream << std::dec;
	outStream << "NpcAIType=" + std::to_string( aiType ) + newLine;
	outStream << "Taming=" + std::to_string( taming ) + newLine;
	outStream << "Peaceing=" + std::to_string( peaceing ) + newLine;
	outStream << "Provoing=" + std::to_string( provoing ) + newLine;
	outStream << "HoldG=" + std::to_string( goldOnHand ) + newLine;
	outStream << "Split=" + std::to_string( splitNum ) + "," + std::to_string( splitChance ) + newLine;
	outStream << "WanderArea=" + std::to_string( fx[0] ) + "," + std::to_string( fy[0] ) + "," + std::to_string( fx[1] ) + "," + std::to_string( fy[1] ) + "," + std::to_string( fz ) + newLine;
	outStream << "NpcWander=" + std::to_string( wanderMode ) + "," + std::to_string( oldWanderMode ) + newLine;
	outStream << "SPAttack=" + std::to_string( spellAttack ) + "," + std::to_string( spellDelay ) + newLine;
	outStream << "QuestType=" + std::to_string( questType ) + newLine;
	outStream << "QuestRegions=" + std::to_string( questOrigRegion ) + "," + std::to_string( questDestRegion ) + newLine;
	outStream << "FleeAt=" + std::to_string( fleeAt ) + newLine;
	outStream << "ReAttackAt=" + std::to_string( reAttackAt ) + newLine;
	outStream << "NPCFlag=" + std::to_string( npcFlag ) + newLine;
	outStream << "Mounted=" + std::to_string(( boolFlags.test( BIT_MOUNTED ) ? 1 : 0 )) + newLine;
	outStream << "Stabled=" + std::to_string(( boolFlags.test( BIT_STABLED ) ? 1 : 0 )) + newLine;
	outStream << "TamedHungerRate=" + std::to_string( tamedHungerRate ) + newLine;
	outStream << "TamedThirstRate=" + std::to_string( tamedThirstRate ) + newLine;
	outStream << "TamedHungerWildChance=" + std::to_string( hungerWildChance ) + newLine;
	outStream << "TamedThirstWildChance=" + std::to_string( thirstWildChance ) + newLine;
	outStream << "Foodlist=" + foodList + newLine;
	outStream << "WalkingSpeed=" + std::to_string( walkingSpeed ) + newLine;
	outStream << "RunningSpeed=" + std::to_string( runningSpeed ) + newLine;
	outStream << "FleeingSpeed=" + std::to_string( fleeingSpeed ) + newLine;
	outStream << "WalkingSpeedMounted=" + std::to_string( mountedWalkingSpeed ) + newLine;
	outStream << "RunningSpeedMounted=" + std::to_string( mountedRunningSpeed ) + newLine;
	outStream << "FleeingSpeedMounted=" + std::to_string( mountedFleeingSpeed ) + newLine;	
	outStream << "ControlSlots=" + std::to_string( controlSlots ) + newLine;
	outStream << "MaxLoyalty=" + std::to_string( maxLoyalty ) + newLine;
	outStream << "Loyalty=" + std::to_string( loyalty ) + newLine;
	outStream << "Orneriness=" + std::to_string( orneriness ) + newLine;
	outStream << "Awake=" + std::to_string( boolFlags.test( BIT_AWAKE ) ? 1 : 0 ) + newLine;

	// Loop through list of previous owners for pet and store a reference to each of those
	for( CChar *tempChar = petOwnerList.First(); !petOwnerList.Finished(); tempChar = petOwnerList.Next() )
	{
		if( ValidateObject( tempChar ))
		{
			outStream << "PetOwner=" + std::to_string( tempChar->GetSerial() ) + newLine;
		}
	}
}
void CChar::PlayerValues_st::DumpBody( std::ostream& outStream )
{
	const char newLine = '\n';

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "RobeSerial=0x" << robe << newLine;
	outStream << "OriginalID=0x" << origId << ",0x" << origSkin << newLine;
	outStream << "Hair=0x" << hairStyle << ",0x" << hairColour << newLine;
	outStream << "Beard=0x" << beardStyle << ",0x" << beardColour << newLine;
	outStream << "TownVote=0x" << townVote << newLine;

	// Decimal / String Values
	outStream << std::dec;
	outStream << "Account=" + std::to_string( accountNum ) + newLine;
	outStream << "LastOn=" + lastOn + newLine;
	outStream << "LastOnSecs=" + std::to_string( lastOnSecs ) + newLine;
	outStream << "CreatedOn=" + std::to_string( createdOn ) + newLine;
	outStream << "PlayTime=" + std::to_string( playTime ) + newLine;
	outStream << "OrgName=" + origName + newLine;
	outStream << "CommandLevel=" + std::to_string( commandLevel ) + newLine;	// command level
	outStream << "Squelched=" + std::to_string( squelched ) + newLine;
	outStream << "Deaths=" + std::to_string( deaths ) + newLine;
	outStream << "ControlSlotsUsed=" + std::to_string( controlSlotsUsed ) + newLine;
	outStream << "FixedLight=" + std::to_string( fixedLight ) + newLine;
	outStream << "TownPrivileges=" + std::to_string( townPriv ) + newLine;
	outStream << "Atrophy=";

	for( UI08 atc = 0; atc <= INTELLECT; ++atc )
	{
		outStream << std::to_string( atrophy[atc] ) + ",";
	}
	outStream << "[END]" << newLine;

	// Format: SkillLocks=[0,34]-[1,255]-[END]
	outStream << "SkillLocks=";
	for( UI08 slc = 0; slc <= INTELLECT; ++slc )
	{
		if( lockState[slc] <= 2 )
		{
			outStream << "[" + std::to_string( slc ) + "," + std::to_string( lockState[slc] ) + "]-";
		}
		else
		{
			outStream << "[" + std::to_string( slc ) + ",0]-";
		}
	}
	outStream << "[END]" << newLine;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::Save()
//|	Date		-	July 21, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves character to worldfile - returns true/false indicating the success
//|					of the write operation
//o------------------------------------------------------------------------------------------------o
bool CChar::Save( std::ostream &outStream )
{
	bool rValue = false;
	if( !IsFree() )
	{
		SI16 mX = GetX();
		SI16 mY = GetY();
		auto [mapWidth, mapHeight] = Map->SizeOfMap( worldNumber );
		if( mX >= 0 && ( mX < mapWidth || mX >= 7000 ))
		{
			if( mY >= 0 && ( mY < mapHeight || mY >= 7000 ))
			{
				DumpHeader( outStream );
				DumpBody( outStream );
				DumpFooter( outStream );

				for( LAYERLIST_ITERATOR lIter = itemLayers.begin(); lIter != itemLayers.end(); ++lIter )
				{
					if( ValidateObject( lIter->second ))
					{
						if( lIter->second->ShouldSave() )
						{
							lIter->second->Save( outStream );
						}
					}
				}
				rValue = true;
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::BreakConcentration()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Breaks the concentration of the character sending a message is a socket exists
//o------------------------------------------------------------------------------------------------o
void CChar::BreakConcentration( CSocket *sock )
{
	if( IsMeditating() )
	{
		SetMeditating( false );
		if( sock != nullptr )
		{
			sock->SysMessage( 100 ); // You break your concentration.
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetAggressorFlags()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of characters the player is marked as aggressor towards
//o------------------------------------------------------------------------------------------------o
auto CChar::GetAggressorFlags() const -> const std::unordered_map<SERIAL, TargetInfo>
{
	return aggressorFlags;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::AddAggressorFlag()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Adds character serial to list of serials player is aggressor to, along with timestamp
//|					for when aggressor flag will timeout for that particular serial
//o------------------------------------------------------------------------------------------------o
auto CChar::AddAggressorFlag( SERIAL toAdd ) -> void
{
	if( aggressorFlags.count( toAdd ) == 0 )
	{
		// Not found in list already, add to list
		aggressorFlags[toAdd] = {cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_AGGRESSORFLAG ), IsNpc()};
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::RemoveAggressorFlag()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Removes character serial from list of characters player is aggressor to
//o------------------------------------------------------------------------------------------------o
auto CChar::RemoveAggressorFlag( SERIAL toRemove ) -> void
{
	aggressorFlags.erase( toRemove );

	// Aggressor flag removed, so loop through player's corpses so flagging can be updated for those!
	for( auto tempCorpse = GetOwnedCorpses()->First(); !GetOwnedCorpses()->Finished(); tempCorpse = GetOwnedCorpses()->Next() )
	{
		if( ValidateObject( tempCorpse ))
		{
			tempCorpse->Dirty( UT_UPDATE );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CheckAggressorFlag()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if serial is in character's aggressorToList, and returns true if it is,
//|					or false if it is not, or if the timestamp has expired (at which point it also
//|					removes the entry from the list)
//o------------------------------------------------------------------------------------------------o
auto CChar::CheckAggressorFlag( SERIAL toCheck ) -> bool
{
	auto it = aggressorFlags.find( toCheck );
	if( it != aggressorFlags.end() )
	{
		// Serial found, but timestamp might be out of date!
		if( it->second.timestamp > cwmWorldState->GetUICurrentTime() )
		{
			// timestamp is still valid, still aggressor
			return true;
		}
		else
		{
			// Timestamp has expired - remove it from the list
			RemoveAggressorFlag( toCheck );
		}
	}

	return false; // serial not found, or timestamp expired and no longer aggressor
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsAggressor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character has any current, non-expired aggressor flags
//o------------------------------------------------------------------------------------------------o
auto CChar::IsAggressor( bool checkForPlayersOnly ) -> bool
{
	bool isAggressor = false;
	if( !aggressorFlags.empty() )
	{
		for( auto it = aggressorFlags.begin(); it != aggressorFlags.end(); ++it )
		{
			// Ignore aggressor flags against NPCs if checking for players only
			if( checkForPlayersOnly && it->second.isNpc )
				continue;

			// Check if any of the entries has a non-expired aggressor timer
			if( it->second.timestamp > cwmWorldState->GetUICurrentTime() )
			{
				isAggressor = true;
				break;
			}
		}
	}
	return isAggressor;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::UpdateAggressorFlagTimestamp()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates aggressor timestamp if serial is found in list
//o------------------------------------------------------------------------------------------------o
auto CChar::UpdateAggressorFlagTimestamp( SERIAL toUpdate ) -> void
{
	auto it = aggressorFlags.find( toUpdate );
	if( it != aggressorFlags.end() )
	{
		it->second.timestamp = cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_AGGRESSORFLAG );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::AggressorFlagMaintenance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Performs maintenance on character's list of aggressor flags and removes expired entries
//o------------------------------------------------------------------------------------------------o
auto CChar::AggressorFlagMaintenance() -> void
{
	// Loop through list of aggressor flags and add any serials with expired timers to a vector
	std::vector<SERIAL> serialsToRemove;
	for( auto it = aggressorFlags.begin(); it != aggressorFlags.end(); ++it )
	{
		if( it->second.timestamp <= cwmWorldState->GetUICurrentTime() )
		{
			serialsToRemove.push_back( it->first );
		}
	}

	// Then loop through vector of serials with expired timers, and safely remove them from the list
	for( SERIAL toRemove : serialsToRemove )
	{
		RemoveAggressorFlag( toRemove );
		if( ValidateObject( GetTarg() ) && GetTarg()->GetSerial() == toRemove )
		{
			Combat->InvalidateAttacker( this );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ClearAggressorFlags()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the character's entire list of aggressor flags
//o------------------------------------------------------------------------------------------------o
auto CChar::ClearAggressorFlags() -> void
{
	aggressorFlags.clear();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPermaGreyFlags()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of permagrey flags for player
//o------------------------------------------------------------------------------------------------o
auto CChar::GetPermaGreyFlags() const -> const std::unordered_map<SERIAL, TargetInfo>
{
	return permaGreyFlags;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::AddPermaGreyFlag()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Adds character to list of characters player has a permagrey flag towards, along
//|					with timestamp for when permagrey flag will timeout for that particular serial (if ever)
//o------------------------------------------------------------------------------------------------o
auto CChar::AddPermaGreyFlag( SERIAL toAdd ) -> void
{
	if( permaGreyFlags.count( toAdd ) == 0 )
	{
		// Not found in list already, add to list
		permaGreyFlags[toAdd] = {cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_PERMAGREYFLAG ), IsNpc()};
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::RemovePermaGreyFlag()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Removes character from list of characters player is permagrey for
//o------------------------------------------------------------------------------------------------o
auto CChar::RemovePermaGreyFlag( SERIAL toRemove ) -> void
{
	permaGreyFlags.erase( toRemove );

	// Permagrey flag removed, so loop through player's corpses so flagging can be updated for those!
	for( auto tempCorpse = GetOwnedCorpses()->First(); !GetOwnedCorpses()->Finished(); tempCorpse = GetOwnedCorpses()->Next() )
	{
		if( ValidateObject( tempCorpse ))
		{
			tempCorpse->Dirty( UT_UPDATE );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CheckPermaGreyFlags()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if serial is in character's permaGreyFlags, and returns true if it is,
//|					or false if it is not, or if the timestamp has expired (at which point it also
//|					removes the entry from the list)
//o------------------------------------------------------------------------------------------------o
auto CChar::CheckPermaGreyFlag( SERIAL toCheck ) -> bool
{
	auto it = permaGreyFlags.find( toCheck );
	if( it != permaGreyFlags.end() )
	{
		// Serial found, but timestamp might be out of date!
		if(  it->second.timestamp > cwmWorldState->GetUICurrentTime() )
		{
			// timestamp is still valid, permagrey flag still active
			return true;
		}
		else
		{
			// Timestamp has expired - remove it from the list
			RemovePermaGreyFlag( toCheck );
		}
	}

	return false; // serial not found, or timestamp expired and permagrey flag no longer active
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsPermaGrey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if character has any active permagrey flags
//o------------------------------------------------------------------------------------------------o
auto CChar::IsPermaGrey( bool checkForPlayersOnly ) -> bool
{
	bool isPermaGrey = false;
	if( !permaGreyFlags.empty() )
	{
		for( auto it = permaGreyFlags.begin(); it != permaGreyFlags.end(); ++it )
		{
			// Ignore aggressor flags against NPCs if checking for players only
			if( checkForPlayersOnly && it->second.isNpc )
				continue;

			// Check if any of the entries has a non-expired permagrey flag timer
			if( it->second.timestamp > cwmWorldState->GetUICurrentTime() )
			{
				isPermaGrey = true;
				break;
			}
		}
	}
	return isPermaGrey;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::UpdatePermaGreyFlagTimestamp()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates aggressor timestamp if serial is found in list
//o------------------------------------------------------------------------------------------------o
auto CChar::UpdatePermaGreyFlagTimestamp( SERIAL toUpdate ) -> void
{
	auto it = permaGreyFlags.find( toUpdate );
	if( it != permaGreyFlags.end() )
	{
		it->second.timestamp = cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_PERMAGREYFLAG );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::PermaGreyFlagMaintenance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Performs maintenance on character's list of permagrey flags and removes expired entries
//o------------------------------------------------------------------------------------------------o
auto CChar::PermaGreyFlagMaintenance() -> void
{
	// Loop through list of permagrey flags and add any serials with expired timers to a vector
	std::vector<SERIAL> serialsToRemove;
	for( auto it = permaGreyFlags.begin(); it != permaGreyFlags.end(); ++it )
	{
		if( it->second.timestamp <= cwmWorldState->GetUICurrentTime() )
		{
			serialsToRemove.push_back( it->first );
		}
	}

	// Then loop through vector of serials with expired timers, and safely remove them from the list
	for( SERIAL toRemove : serialsToRemove )
	{
		RemovePermaGreyFlag( toRemove );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ClearPermaGreyFlags()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the character's entire list of permagrey flags
//o------------------------------------------------------------------------------------------------o
auto CChar::ClearPermaGreyFlags() -> void
{
	permaGreyFlags.clear();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPetList()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of ALL pets the character owns, regardless of where they might be
//o------------------------------------------------------------------------------------------------o
GenericList<CChar *> *CChar::GetPetList( void )
{
	return &petsOwned;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFollowerList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of active followers the character has
//o------------------------------------------------------------------------------------------------o
GenericList<CChar *> *CChar::GetFollowerList( void )
{
	return &activeFollowers;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AddFollower()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds an owned NPC to character's list of followers
//o------------------------------------------------------------------------------------------------o
auto CChar::AddFollower( CChar *npcToAdd ) -> bool
{
	// Only add NPC as follower if they are not stabled
	if( ValidateObject( npcToAdd ) && !npcToAdd->GetStabled() )
	{
		return GetFollowerList()->Add( npcToAdd );
	}

	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::RemoveFollower()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Removes a follower from character's list of followers
//o------------------------------------------------------------------------------------------------o
auto CChar::RemoveFollower( CChar *followerToRemove ) -> bool
{
	return GetFollowerList()->Remove( followerToRemove );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOwnedCorpses()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of the player's corpses
//o------------------------------------------------------------------------------------------------o
auto CChar::GetOwnedCorpses() -> GenericList<CItem *>*
{
	return &ownedCorpses;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AddCorpse()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds a corpse to character's list of corpses
//o------------------------------------------------------------------------------------------------o
auto CChar::AddCorpse( CItem *corpseToAdd ) -> bool
{
	return GetOwnedCorpses()->Add( corpseToAdd );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::RemoveCorpse()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Removes a corpse from character's list of corpses
//o------------------------------------------------------------------------------------------------o
auto CChar::RemoveCorpse( CItem *corpseToRemove ) -> bool
{
	return GetOwnedCorpses()->Remove( corpseToRemove );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOwnedItems()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of items the char owns
//o------------------------------------------------------------------------------------------------o
auto CChar::GetOwnedItems() -> std::vector<CItem *>*
{
	return &ownedItems;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AddOwnedItem()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds the item toAdd to the player's owned list
//|					ensuring that it is not ALREADY in the list
//o------------------------------------------------------------------------------------------------o
auto CChar::AddOwnedItem( CItem *toAdd ) -> void
{
	auto iter = std::find_if( ownedItems.begin(), ownedItems.end(), [toAdd]( CItem *entry )
	{
		return toAdd == entry;
	});
	if( iter == ownedItems.end() )
	{
		ownedItems.push_back( toAdd );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::RemoveOwnedItem()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Removes the item toRemove from the player's owned list
//o------------------------------------------------------------------------------------------------o
auto  CChar::RemoveOwnedItem( CItem *toRemove ) -> void
{
	auto iter = std::find_if( ownedItems.begin(), ownedItems.end(), [toRemove]( CItem *entry )
	{
		return entry == toRemove;
	});
	if( iter != ownedItems.end() )
	{
		ownedItems.erase( iter );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetMaxHP( )
//|					CChar::SetMaxHP()
//|					CChar::SetFixedMaxHP()
//|	Date		-	15 February, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum hitpoints (or fixed maximum hitpoints) of the object
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetMaxHP( void )
{
	if(( maxHP_oldstr != GetStrength() || oldRace != GetRace() ) && !GetMaxHPFixed() )
		//if str/race changed since last calculation, recalculate maxHp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == nullptr )
		{
			pRace = Races->Race( 0 );
		}

		maxHP = static_cast<UI16>(GetStrength() + static_cast<UI16>(( static_cast<R32>( GetStrength() )) * ( static_cast<R32>( pRace->HPModifier() )) / 100 ));
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
	if( newmaxhp > 0 )
	{
		SetMaxHPFixed( true );
		maxHP			= newmaxhp;
	}
	else
	{
		SetMaxHPFixed( false );

		CRace *pRace = Races->Race( GetRace() );
		if( pRace == nullptr )
		{
			pRace = Races->Race( 0 );
		}

		maxHP = static_cast<UI16>( GetStrength() + static_cast<UI16>(( static_cast<R32>( GetStrength() )) * ( static_cast<R32>( pRace->HPModifier() )) / 100 ));

		maxHP_oldstr	= GetStrength();
		oldRace			= GetRace();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetMaxMana()
//|					CChar::SetMaxMana()
//|					CChar::SetFixedMaxMana()
//|	Date		-	15 February, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum mana (or fixed maximum mana) of the object
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetMaxMana( void )
{
	if(( maxMana_oldint != GetIntelligence() || oldRace != GetRace() ) && !GetMaxManaFixed() )
		//if int/race changed since last calculation, recalculate maxHp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == nullptr )
		{
			pRace = Races->Race( 0 );
		}

		maxMana = static_cast<SI16>( GetIntelligence() + static_cast<SI16>(( static_cast<R32>( GetIntelligence() )) * ( static_cast<R32>( pRace->ManaModifier() )) / 100 ));
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
	if( newmaxmana > 0 )
	{
		SetMaxManaFixed( true );
		maxMana			= newmaxmana;
	}
	else
	{
		SetMaxManaFixed( false );

		CRace *pRace = Races->Race( GetRace() );
		if( pRace == nullptr )
		{
			pRace = Races->Race( 0 );
		}

		maxMana = static_cast<SI16>( GetIntelligence() + static_cast<SI16>(( static_cast<R32>( GetIntelligence() )) * ( static_cast<R32>( pRace->ManaModifier() )) / 100 ));

		maxMana_oldint	= GetIntelligence();
		oldRace			= GetRace();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetMaxStam()
//|					CChar::SetMaxStam()
//|					CChar::SetFixedMaxStam()
//|	Date		-	15 February, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum stamina (or fixed maximum stamina) of the object
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetMaxStam( void )
{
	// If dex/race changed since last calculation, recalculate maxHp
	if(( maxStam_olddex != GetDexterity() || oldRace != GetRace() ) && !GetMaxStamFixed() )
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == nullptr )
		{
			pRace = Races->Race( 0 );
		}

		// Set max. stamina to dex + stammodifier% of dex
		maxStam = static_cast<SI16>( GetDexterity() + static_cast<SI16>(( static_cast<R32>( GetDexterity() )) * ( static_cast<R32>( pRace->StamModifier() )) / 100 ));

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
	if( newmaxstam > 0 )
	{
		SetMaxStamFixed( true );
		maxStam			= newmaxstam;
	}
	else
	{
		SetMaxStamFixed( false );

		CRace *pRace = Races->Race( GetRace() );
		if( pRace == nullptr )
		{
			pRace = Races->Race( 0 );
		}

		maxStam = static_cast<SI16>( GetDexterity() + static_cast<SI16>(( static_cast<R32>( GetDexterity() )) * ( static_cast<R32>( pRace->StamModifier() )) / 100 ));

		maxStam_olddex	= GetDexterity();
		oldRace			= GetRace();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ActualStrength()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the actual strength (minus mods) of the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::ActualStrength( void ) const
{
	return CBaseObject::GetStrength();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetStrength()
//|					CChar::SetStrength()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the strength (incl mods) of the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetStrength( void ) const
{
	auto tempStr = static_cast<SI16>( CBaseObject::GetStrength() + GetStrength2() );
	if( tempStr < 1 )
	{
		tempStr = 1;
	}
	return tempStr;
}
void CChar::SetStrength( SI16 newValue )
{
	if( newValue < 1 )
	{
		newValue = 1;
	}

	CBaseObject::SetStrength( newValue );
	Dirty( UT_HITPOINTS );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ActualIntelligence()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the actual intelligence (minus mods) of the char
//o------------------------------------------------------------------------------------------------o
SI16 CChar::ActualIntelligence( void ) const
{
	return CBaseObject::GetIntelligence();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetIntelligence()
//|					CChar::SetIntelligence()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Gets/Sets the intelligence (incl mods) of the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetIntelligence( void ) const
{
	auto tempInt = static_cast<SI16>( CBaseObject::GetIntelligence() + GetIntelligence2() );
	if( tempInt < 1 )
	{
		tempInt = 1;
	}
	return tempInt;
}
void CChar::SetIntelligence( SI16 newValue )
{
	if( newValue < 1 )
	{
		newValue = 1;
	}

	CBaseObject::SetIntelligence( newValue );
	Dirty( UT_MANA );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ActualDexterity()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the actual (minus mods) dexterity of the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::ActualDexterity( void ) const
{
	return CBaseObject::GetDexterity();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetDexterity()
//|					CChar::SetDexterity()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the effective dexterity (including modifications) of the player
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetDexterity( void ) const
{
	auto tempDex = static_cast<SI16>( CBaseObject::GetDexterity() + GetDexterity2() );
	if( tempDex < 1 )
	{
		tempDex = 1;
	}
	return tempDex;
}
void CChar::SetDexterity( SI16 newValue )
{
	if( newValue < 1 )
	{
		newValue = 1;
	}
	
	CBaseObject::SetDexterity( newValue );
	Dirty( UT_STAMINA );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IncStrength2()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Increments strength2 (modifications) by toAdd
//o------------------------------------------------------------------------------------------------o
void CChar::IncStrength2( SI16 toAdd )
{
	SetStrength2( static_cast<SI16>( GetStrength2() + toAdd ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IncDexterity2()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Increments dexterity2 (modifications) by toAdd
//o------------------------------------------------------------------------------------------------o
void CChar::IncDexterity2( SI16 toAdd )
{
	SetDexterity2( static_cast<SI16>( GetDexterity2() + toAdd ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IncIntelligence2()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Increments intelligence2 (modifications) by toAdd
//o------------------------------------------------------------------------------------------------o
void CChar::IncIntelligence2( SI16 toAdd )
{
	SetIntelligence2( static_cast<SI16>( GetIntelligence2() + toAdd ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsMurderer()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is a murderer
//o------------------------------------------------------------------------------------------------o
bool CChar::IsMurderer( void ) const
{
	return ( GetFlag() == 0x01 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsCriminal()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is a criminal
//o------------------------------------------------------------------------------------------------o
bool CChar::IsCriminal( void ) const
{
	return ( GetFlag() == 0x02 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsInnocent()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is innocent
//o------------------------------------------------------------------------------------------------o
bool CChar::IsInnocent( void ) const
{
	return ( GetFlag() == 0x04 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsNeutral()
//| Date		-	18 July 2005
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is neutral
//o------------------------------------------------------------------------------------------------o
bool CChar::IsNeutral( void ) const
{
	return ( GetFlag() == 0x08 );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetFlagRed()
//| Date		-	2nd October, 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect murderer status
//o------------------------------------------------------------------------------------------------o
void CChar::SetFlagRed( void )
{
	flag = 0x01;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetFlagGray()
//| Date		-	2nd October, 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect criminality
//o------------------------------------------------------------------------------------------------o
void CChar::SetFlagGray( void )
{
	flag = 0x02;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetFlagBlue()
//| Date		-	2nd October, 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect innocence
//o------------------------------------------------------------------------------------------------o
void CChar::SetFlagBlue( void )
{
	flag = 0x04;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetFlagNeutral()
//| Date		-	18th July, 2005
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Updates the character's flag to reflect neutrality
//o------------------------------------------------------------------------------------------------o
void CChar::SetFlagNeutral( void )
{
	flag = 0x08;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::DecHunger()
//| Date		-	13 March 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Decrements the character's hunger
//o------------------------------------------------------------------------------------------------o
bool CChar::DecHunger( const SI08 amt )
{
	return SetHunger( static_cast<SI08>( GetHunger() - amt ));
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::DecThirst()
//| Date        -   6 June 2021
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Decrements the character's thirst
//o------------------------------------------------------------------------------------------------o
bool CChar::DecThirst( const SI08 amt )
{
	return SetThirst( static_cast<SI08>( GetThirst() - amt ));
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::StopSpell()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Halts spellcasting action for character
//o------------------------------------------------------------------------------------------------o
void CChar::StopSpell( void )
{
	SetTimer( tCHAR_SPELLTIME, 0 );
	SetCasting( false );
	SetSpellCast( -1 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::HandleLine()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to handle loading of characters tags from world files
//o------------------------------------------------------------------------------------------------o
bool CChar::HandleLine( std::string &UTag, std::string &data )
{
	bool rValue = CBaseObject::HandleLine( UTag, data );
	if( !rValue )
	{
		auto csecs = oldstrutil::sections( data, "," );
		
		switch(( UTag.data()[0] ))
		{
			case 'A':
				if( UTag == "ACCOUNT" )
				{
					SetAccountNum( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ATROPHY" )
				{
					int count = 0;
					for( auto &value : csecs )
					{
						value = oldstrutil::upper( oldstrutil::trim( oldstrutil::removeTrailing( value,"//" )));
						if( value == "[END]" )
						{
							break;
						}
						if( value.empty() )
						{
							break;
						}
						SetAtrophy( static_cast<UI08>( std::stoul( value, nullptr, 0 )), count );
						count = count + 1;
					}
					rValue = true;
				}
				else if( UTag == "ADVANCEOBJECT" )
				{
					SetAdvObj( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ADVRACEOBJECT" )
				{
					SetRaceGate( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ALLMOVE" )
				{
					SetAllMove( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "AWAKE" )
				{
					SetAwake( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'B':
				if( UTag == "BEARDSTYLE" )
				{
					SetBeardStyle( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "BEARDCOLOUR" )
				{
					SetBeardColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "BASESKILLS" )
				{
					auto dsecs = oldstrutil::sections( data, "-" );
					for( auto &value : dsecs )
					{
						if( value.empty() )
						{
							break;
						}
						auto secs = oldstrutil::sections( value, "," );
						if( secs.size() != 2 )
						{
							break;
						}
						auto skillNum = static_cast<SkillLock>( std::stoul( secs[0].substr( 1 ), nullptr, 0 ));
						auto skillValue = static_cast<UI16>( std::stoul( secs[1].substr( 0, secs[1].size() - 1 ), nullptr, 0 ));
						SetBaseSkill( skillValue, skillNum );
					}
					rValue = true;
				}
				else if( UTag == "BEARD" )
				{
					SetBeardStyle( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetBeardColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "BRKPEACECHANCEGAIN" )
				{
					SetBrkPeaceChanceGain( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "BRKPEACECHANCE" )
				{
					SetBrkPeaceChance( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'C':
				if( UTag == "COMMANDLEVEL" )
				{
					SetCommandLevel( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "CANRUN" )
				{
					SetRun( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "CANATTACK" )
				{
					SetCanAttack( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "CANTRAIN" )
				{
					SetCanTrain( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "CANBEHIRED" )
				{
					SetCanHire( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "CONTROLSLOTS" )
				{
					SetControlSlots( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "CONTROLSLOTSUSED" )
				{
					SetControlSlotsUsed( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "CREATEDON" )
				{
					SetCreatedOn( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'D':
				if( UTag == "DEATHS" )
				{
					SetDeaths( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "DEAD" )
				{
					SetDead(( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 ));
					rValue = true;
				}
				break;
			case 'E':
				if( UTag == "EMOTION" )
				{
					SetEmoteColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'F':
				if( UTag == "FIXEDLIGHT" )
				{
					SetFixedLight( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "FX1" )
				{
					SetFx( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )), 0 );
					rValue = true;
				}
				else if( UTag == "FX2" )
				{
					SetFx( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )), 1 );
					rValue = true;
				}
				else if( UTag == "FY1" )
				{
					SetFy( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )), 0 );
					rValue = true;
				}
				else if( UTag == "FY2" )
				{
					SetFy( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )), 1 );
					rValue = true;
				}
				else if( UTag == "FZ1" )
				{
					SetFz( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "FLEEAT" )
				{
					SetFleeAt( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "FONTTYPE" )
				{
					SetFontType( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "FOODLIST" )
				{
					SetFood( data.substr( 0, MAX_NAME - 1 ));
					rValue = true;
				}
				else if( UTag == "FLEEINGSPEED" )
				{
					SetFleeingSpeed( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )))));
					rValue = true;
				}
				else if( UTag == "FLEEINGSPEEDMOUNTED" )
				{
					SetMountedFleeingSpeed( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )))));
					rValue = true;
				}
				break;
			case 'G':
				if( UTag == "GUILDFEALTY" )
				{
					SetGuildFealty( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "GUILDTITLE" )
				{
					SetGuildTitle( data );
					rValue = true;
				}
				else if( UTag == "GUILDNUMBER" )
				{
					SetGuildNumber( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "GUILDTOGGLE" )
				{
					SetGuildToggle( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'H':
				if( UTag == "HUNGER" )
				{
					SetHunger( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "HOLDG" )
				{
					SetHoldG( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "HAIRSTYLE" )
				{
					SetHairStyle( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "HAIRCOLOUR" )
				{
					SetHairColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "HAIR" )
				{
					SetHairStyle( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetHairColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'I':
				if( UTag == "ISNPC" )
				{
					SetNpc(( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 ));
					rValue = true;
				}
				else if( UTag == "ISSHOP" )
				{
					SetShop(( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 ));
					rValue = true;
				}
				else if( UTag == "ISWARRING" )
				{
					SetWar(( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 ));
					rValue = true;
				}
				break;
			case 'K':
				if( UTag == "KARMALOCK" )
				{
					SetKarmaLock( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'L':
				if( UTag == "LASTON" )
				{
					SetLastOn( data );
					rValue = true;
				}
				else if( UTag == "LASTONSECS" )
				{
					SetLastOnSecs( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "LOYALTY" )
				{
					SetLoyalty( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'M':
				if( UTag == "MAYLEVITATE" )
				{
					SetLevitate(( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 ));
					rValue = true;
				}
				else if( UTag == "MURDERTIMER" )
				{
					SetTimer( tCHAR_MURDERRATE, BuildTimeValue( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ))))));
					rValue = true;
				}
				else if( UTag == "MAXHP" )
				{
					SetFixedMaxHP( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MAXMANA" )
				{
					SetFixedMaxMana( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MAXLOYALTY" )
				{
					SetMaxLoyalty( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MAXSTAM" )
				{
					SetFixedMaxStam( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MOUNTED" )
				{
					SetMounted( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'N':
				if( UTag == "NPCAITYPE" )
				{
					SetNPCAiType( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "NPCWANDER" )
				{
					if( csecs.size() >= 2 )
					{
						SetNpcWander( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetOldNpcWander( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetNpcWander( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "NPCFLAG" )
				{
					SetNPCFlag( static_cast<cNPC_FLAG>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					UpdateFlag( this );
					rValue = true;
				}
				else if( UTag == "NPCGUILD" )
				{
					SetNPCGuild( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "NPCGUILDJOINED" )
				{
					SetNPCGuildJoined( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'O':
				if( UTag == "ORGNAME" )
				{
					SetOrgName( data );
					rValue = true;
				}
				else if( UTag == "ORIGINALBODYID" )
				{
					SetOrgId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ORIGINALSKINID" )
				{
					SetOrgSkin( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ORIGINALID" )
				{
					SetOrgId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetOrgSkin( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ORNERINESS" )
				{
					SetOrneriness( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'P':
				if( UTag == "PRIVILEGES" )
				{
					SetPriv( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "PACKITEM" )
				{
					tempContainerSerial = static_cast<SERIAL>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "PETOWNER" )
				{
					CChar *cList = CalcCharObjFromSer( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					if( ValidateObject( cList ))
					{
						AddPetOwnerToList( cList );
					}
					rValue = true;
				}
				else if( UTag == "POISON" )
				{
					SetPoisoned( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "POISONSTRENGTH" )
				{
					SetPoisonStrength( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "PEACEING" )
				{
					SetPeaceing( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "PROVOING" )
				{
					SetProvoing( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "PEACETIMER" )
				{
					SetTimer( tCHAR_PEACETIMER, BuildTimeValue( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ))))));
					rValue = true;
				}
				else if( UTag == "PLAYTIME" )
				{
					SetPlayTime( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'Q':
				if( UTag == "QUESTTYPE" )
				{
					SetQuestType( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "QUESTDESTINATIONREGION" )
				{
					SetQuestDestRegion( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "QUESTORIGINALREGION" )
				{
					SetQuestOrigRegion( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "QUESTREGIONS" )
				{
					SetQuestOrigRegion( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetQuestDestRegion( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'R':
				if( UTag == "ROBESERIAL" )
				{
					SetRobe( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "RESERVED" )
				{
					SetCell( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "RUNNING" )
				{
					rValue = true;
				}
				else if( UTag == "REGION" )
				{
					SetRegion(static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "REATTACKAT" )
				{
					SetReattackAt( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "RUNNINGSPEED" )
				{
					SetRunningSpeed( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )))));
					rValue = true;
				}
				else if( UTag == "RUNNINGSPEEDMOUNTED" )
				{
					SetMountedRunningSpeed( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )))));
					rValue = true;
				}
				break;
			case 'S':
				if( UTag == "SPLIT" )
				{
					if( csecs.size() >= 2 )
					{
						SetSplit( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetSplitChance( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetSplit( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "SPLITCHANCE" )
				{
					SetSplitChance( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SUMMONTIMER" )
				{
					SetTimer( tNPC_SUMMONTIME, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SAY" )
				{
					SetSayColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "STEALTH" )
				{
					SetStealth( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SPATTACK" )
				{
					if( csecs.size() >=2 )
					{
						SetSpAttack( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetSpDelay( static_cast<UI08> (std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetSpAttack( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "SPECIALATTACKDELAY" )
				{
					SetSpDelay( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SQUELCHED" )
				{
					SetSquelched( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SKILLLOCKS" )
				{
					// Format: Baselocks=[0,34]-[1,255]-[END]
					auto dsecs = oldstrutil::sections( data, "-" );
					
					for( auto &value : dsecs )
					{
						if( value.empty() )
						{
							break;
						}
						auto secs = oldstrutil::sections( value, "," );
						if( secs.size() != 2 )
						{
							break;
						}
						auto skillnum = static_cast<UI08>( std::stoul( secs[0].substr( 1 ), nullptr, 0 ));
						auto skillvalue = static_cast<SkillLock>( std::stoul( secs[1].substr( 0, secs[1].size() - 1 ), nullptr, 0 ));
						SetSkillLock( skillvalue, skillnum );
					}
					rValue = true;
				}
				else if( UTag == "SPEECH" )
				{
					SetSayColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetEmoteColour( static_cast<UI16>( std::stoul (oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "STABLED" )
				{
					SetStabled( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'T':
				if( UTag == "TAMING" )
				{
					SetTaming( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TAMEDHUNGERRATE" )
				{
					SetTamedHungerRate( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TAMEDHUNGERWILDCHANCE" )
				{
					SetTamedHungerWildChance( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TAMEDTHIRSTRATE" )
				{
					SetTamedThirstRate( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TAMEDTHIRSTWILDCHANCE" )
				{
					SetTamedThirstWildChance( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TOWN" )
				{
					SetTown( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TOWNVOTE" )
				{
					SetTownVote( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TOWNPRIVILEGES" )
				{
					SetTownpriv( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "TOWNTITLE" )
				{
					SetTownTitle( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "THIRST" )
				{
					SetThirst( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'W':
				if( UTag == "WANDERAREA" )
				{
					SetFx( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )), 0 );
					SetFy( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )), 0 );
					SetFx( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )), 1 );
					SetFy( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 )), 1 );
					SetFz( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[4], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "WILLHUNGER" )
				{
					SetHungerStatus( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "WILLTHIRST" )
				{
					SetThirstStatus( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "WALKINGSPEED" )
				{
					SetWalkingSpeed( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )))));
					rValue = true;
				}
				else if( UTag == "WALKINGSPEEDMOUNTED" )
				{
					SetMountedWalkingSpeed( static_cast<R32>( std::stof( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )))));
					rValue = true;
				}
				break;
			case 'X':
				if( UTag == "XNPCWANDER" )
				{
					SetOldNpcWander( static_cast<SI08>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::LoadRemnants()
//|	Date		-	21st January, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o------------------------------------------------------------------------------------------------o
bool CChar::LoadRemnants( void )
{
	bool rValue = true;
	SetSerial( serial );

	if( IsNpc() && IsAtWar() )
	{
		SetWar( false );
	}

	CTownRegion *tRegion = CalcRegionFromXY( GetX(), GetY(), worldNumber, instanceId, this );
	SetRegion(( tRegion != nullptr ? tRegion->GetRegionNum() : 0xFF ));
	SetTimer( tCHAR_ANTISPAM, 0 );
	if( GetId() != GetOrgId() && !IsDead() )
	{
		SetId( GetOrgId() );
	}

	const UI16 acct = GetAccount().wAccountIndex;

	// Max body/anim ID supported in default client is 2048 (or 0x800),
	// though custom clients like CUO can be modified to bypass this limit
	if( GetId() > 0x999 )
	{
		if( acct == AB_INVALID_ID )
		{
			std::string charName = GetNpcDictName( this, nullptr, NRS_SYSTEM );
			Console.Warning( oldstrutil::format( "NPC: %s with serial 0x%X with bugged body found, deleting", charName.c_str(), GetSerial() ));
			rValue = false;
		}
		else
		{
			SetId( 0x0190 );
		}
	}
	if( rValue )
	{
		MapRegion->AddChar( this );

		const SI16 mx = GetX();
		const SI16 my = GetY();

		auto [mapWidth, mapHeight] = Map->SizeOfMap(worldNumber);
		const bool overRight = ( mx > mapWidth );
		const bool overBottom = ( my > mapHeight );

		if( acct == AB_INVALID_ID && (( overRight && mx < 7000 ) || ( overBottom && my < 7000 ) || mx < 0 || my < 0 ))
		{
			if( IsNpc() )
			{
				std::string charName = GetNpcDictName( this, nullptr, NRS_SYSTEM );
				Console.Warning( oldstrutil::format( "NPC: %s with serial 0x%X found outside valid world locations, deleting", charName.c_str(), GetSerial() ));
				rValue = false;
			}
			else
			{
				SetLocation( 1000, 1000, 0 );
			}
		}
		if( rValue )
		{
			if( !IsNpc() )
			{
				Accounts->AddCharacter( GetAccountNum(), this );	// account ID, and account object.
			}
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::SkillUsed()
//|	Date		-	May 2, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the skill is being used
//o------------------------------------------------------------------------------------------------o
bool CChar::SkillUsed( UI08 skillNum ) const
{
	bool rValue = false;
	if( skillNum < ALLSKILLS )
	{
		UI08 part		= static_cast<UI08>( skillNum / 32 );
		UI08 offset		= static_cast<UI08>( skillNum % 32 );
		rValue			= skillUsed[part].test( offset );
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::SkillUsed()
//|	Date		-	May 2, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets whether the skill skillNum is being used or not
//o------------------------------------------------------------------------------------------------o
void CChar::SkillUsed( bool value, UI08 skillNum )
{
	if( skillNum < ALLSKILLS )
	{
		UI08 part		= static_cast<UI08>( skillNum / 32 );
		UI08 offset		= static_cast<UI08>( skillNum % 32 );
		skillUsed[part].set( offset, value );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::PostLoadProcessing()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment
//|					following the loading of the world
//o------------------------------------------------------------------------------------------------o
void CChar::PostLoadProcessing( void )
{
	CBaseObject::PostLoadProcessing();
	if( tempContainerSerial != INVALIDSERIAL )
	{
		SetPackItem( CalcItemObjFromSer( tempContainerSerial ));
	}
	else
	{
		SetPackItem( nullptr );
	}

	SI32 maxWeight = GetStrength() * cwmWorldState->ServerData()->WeightPerStr() + 40;
	if( GetWeight() <= 0 || GetWeight() > MAX_WEIGHT || GetWeight() > maxWeight )
	{
		SetWeight( Weight->CalcCharWeight( this ));
	}
	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		Skills->UpdateSkillLevel( this, i );
	}

	// If character belongs to a player and doesn't have a created timestamp, add one now
	if( !IsNpc() && GetCreatedOn() == 0 )
	{
		SetCreatedOn( GetMinutesSinceEpoch() );
	}

	// We need to add things to petlists, so we can cleanup after ourselves properly -
	SetPostLoaded( true );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IsJailed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if the character is in a jail cell
//o------------------------------------------------------------------------------------------------o
bool CChar::IsJailed( void ) const
{
	return ( GetCell() != -1 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::InDungeon()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine if player is inside a dungeon
//o------------------------------------------------------------------------------------------------o
bool CChar::InDungeon( void )
{
	bool rValue = false;
	if( GetRegion() != nullptr )
	{
		rValue = GetRegion()->IsDungeon();
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::TextMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle server-triggered speech for characters
//o------------------------------------------------------------------------------------------------o
void CChar::TextMessage( CSocket *s, std::string toSay, SpeechType msgType, bool spamTimer )
{
	bool canSpeak = !spamTimer;
	if( !toSay.empty() )
	{
		if( spamTimer )
		{
			if( GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
			{
				SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 10 ));
				canSpeak = true;
			}
		}
		if( canSpeak )
		{
			UI16 txtColor = 0x5A;
			if( msgType == EMOTE || msgType == ASCIIEMOTE )
			{
				txtColor = GetEmoteColour();
			}
			else if( msgType == TALK || msgType == ASCIITALK )
			{
				if( GetNpcAiType() == AI_EVIL || GetNpcAiType() == AI_EVIL_CASTER )
				{
					txtColor = 0x0026;
				}
				else
				{
					txtColor = GetSayColour();
				}
			}
			else if( msgType == SYSTEM )
			{
				txtColor = 0x07FA;
			}
			else if( msgType == OBJ )
			{
				txtColor = 0x3b2;
			}
			if( !txtColor || txtColor == 0x1700 )
			{
				txtColor = 0x5A;
			}

			SERIAL speakTo		= INVALIDSERIAL;
			SpeechTarget target	= SPTRG_PCNPC;
			CChar *mChar		= nullptr;
			if( s != nullptr )
			{
				mChar			= s->CurrcharObj();
				speakTo			= mChar->GetSerial();
				target			= SPTRG_INDIVIDUAL;
			}

			if( cwmWorldState->ServerData()->UseUnicodeMessages() )
			{
				// Doesn't enter the speech-queue, though!
				bool sendAll = true;
				if( target == SPTRG_INDIVIDUAL || target == SPTRG_ONLYRECEIVER )
				{
					sendAll = false;
				}

				// Send speech as a unicode-enabled cliloc message (packet 0xC1)
				//ClilocMessage( s, this, msgType, txtColor, (FontType)GetFontType(), 1114057, sendAll, "s", toSay.c_str() );

				CPUnicodeMessage unicodeMessage;
				unicodeMessage.Message( toSay );
				unicodeMessage.Font( static_cast<FontType>( GetFontType() ));
				unicodeMessage.Colour( txtColor );
				unicodeMessage.Type( msgType );
				unicodeMessage.Language( "ENG" );
				unicodeMessage.Name( this->GetNameRequest( mChar, NRS_SPEECH ));
				unicodeMessage.ID( INVALIDID );
				unicodeMessage.Serial( GetSerial() );

				bool sendSock = ( s != nullptr );

				if( sendAll )
				{
					UI16 searchDistance = DIST_SAMESCREEN;
					if( msgType == WHISPER || msgType == ASCIIWHISPER )
					{
						searchDistance = DIST_SAMETILE;
					}
					else if( msgType == YELL || msgType == ASCIIYELL )
					{
						searchDistance = DIST_SAMESCREEN * 1.5;
					}
					else if( msgType == EMOTE || msgType == ASCIIEMOTE )
					{
						searchDistance = DIST_INRANGE;
					}

					for( auto &tSock : FindNearbyPlayers( this, searchDistance ))
					{
						if(( tSock == s ) && sendSock )
						{
							sendSock = false;
						}
						tSock->Send( &unicodeMessage );
					}
				}

				if( sendSock )
				{
					s->Send( &unicodeMessage );
				}
			}
			else
			{
				CSpeechEntry& toAdd = SpeechSys->Add();
				toAdd.Font( static_cast<FontType>( GetFontType() ));
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
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::TextMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle server-triggered speech for characters
//o------------------------------------------------------------------------------------------------o
void CChar::TextMessage( CSocket *s, SI32 dictEntry, SpeechType msgType, int spamTimer, ... )
{
	UnicodeTypes dictLang = ZERO;
	if( s != nullptr )
	{
		dictLang = s->Language();
	}

	std::string txt = Dictionary->GetEntry( dictEntry, dictLang );
	if( !txt.empty() )
	{
		va_list argptr;
		va_start( argptr, spamTimer );
		auto msg = oldstrutil::format( txt, argptr );
		if( msg.size() > 512 )
		{
			msg = msg.substr( 0, 512 );
		}
		auto spam = false;
		if( spamTimer == 1 )
		{
			spam = true;
		}
		TextMessage( s, msg, msgType, spam );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::SendWornItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send items worn on character to client
//o------------------------------------------------------------------------------------------------o
void CChar::SendWornItems( CSocket *mSock )
{
	SetOnHorse( false );
	CPWornItem toSend;
	for( CItem *i = FirstItem(); !FinishedItems(); i = NextItem() )
	{
		if( ValidateObject( i ) && !i->IsFree() )
		{
			if( i->GetLayer() == IL_MOUNT )
			{
				SetOnHorse( true );
			}
			toSend = ( *i );
			mSock->Send( &toSend );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::WalkZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Update character'z Z coordinate based on movement, and handle falling
//o------------------------------------------------------------------------------------------------o
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
				toExecute->OnFall(( this ), fallDistance );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::WalkDir()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Update character's walking direction
//o------------------------------------------------------------------------------------------------o
void CChar::WalkDir( SI08 newDir )
{
	dir = newDir;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::Cleanup()
//| Date		-	11/6/2003
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Cleans up after character
//o------------------------------------------------------------------------------------------------o
void CChar::Cleanup( void )
{
	if( !IsFree() )	// We're not the default item in the handler
	{
		MapRegion->RemoveChar( this );

		JSEngine->ReleaseObject( IUE_CHAR, this );

		CBaseObject::Cleanup();

		RemoveFromSight();

		for( CItem *tItem = FirstItem(); !FinishedItems(); tItem = NextItem() )
		{
			if( ValidateObject( tItem ))
			{
				tItem->Delete();
			}
		}

		// Clear up some stuff for our target in combat
		CChar *tempChar = nullptr;
		tempChar = GetTarg();
		if( ValidateObject( tempChar ))
		{
			// Clear char as target of target, if set
			if( tempChar->GetTarg() == this )
			{
				tempChar->SetTarg( nullptr );

				if( tempChar->IsAtWar() && tempChar->IsNpc() )
				{
					// What if target is fighting other characters too?
					tempChar->ToggleCombat();
				}
			}

			// Remove char as target's attacker, if set
			if( tempChar->GetAttacker() == this )
			{
				tempChar->SetAttacker( nullptr );
			}

			// Remove char from target's aggressorToList
			tempChar->RemoveAggressorFlag( this->GetSerial() );

			SetTarg( nullptr );
		}

		// Do the same for our attacker (who could be a different character than our target)
		tempChar = GetAttacker();
		if( ValidateObject( tempChar ))
		{
			// Again, clear this char as attacker of our attacker, if set
			if( tempChar->GetAttacker() == this )
			{
				tempChar->SetAttacker( nullptr );
			}

			// Also clear attacker's target, if it matches this char
			if( tempChar->GetTarg() == this )
			{
				tempChar->SetTarg( nullptr );
				if( tempChar->IsAtWar() && tempChar->IsNpc() )
				{
					tempChar->ToggleCombat();
				}
			}

			// Remove char from attacker's aggressorToList
			tempChar->RemoveAggressorFlag( this->GetSerial() );

			SetAttacker( nullptr );
		}

		// Clear char's list of aggressor flags
		ClearAggressorFlags();


		// If we delete a NPC we should delete his tempeffects as well
		std::vector<CTEffect *> removedEffect;
		for( const auto &Effect : cwmWorldState->tempEffects.collection() )
		{
			if( Effect->Destination() == GetSerial() )
			{
				removedEffect.push_back( Effect );
			}

			else if( Effect->Source() == GetSerial() )
			{
				Effect->Source( INVALIDSERIAL );
			}
		}
		std::for_each( removedEffect.begin(), removedEffect.end(), []( CTEffect *effect )
		{
			cwmWorldState->tempEffects.Remove( effect, true );
		});

		// if we delete a NPC we should delete him from spawnregions
		// this will fix several crashes
		if( IsNpc() && IsSpawned() )
		{
			if( GetSpawn() < BASEITEMSERIAL )
			{
				UI16 spawnRegNum = static_cast<UI16>( GetSpawn() );
				if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
				{
					CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
					if( spawnReg != nullptr )
					{
						spawnReg->DeleteSpawnedChar( this );
					}
				}
			}
		}

		if( !IsNpc() )
		{
			CAccountBlock_st& mAcct = GetAccount();
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
		{
			SetSpawn( INVALIDSERIAL );
		}

		if( IsGuarded() )
		{
			CChar *petGuard = Npcs->GetGuardingFollower( this, this );
			if( ValidateObject( petGuard ))
			{
				petGuard->SetGuarding( nullptr );
			}
			SetGuarded( false );
		}
		Npcs->StopPetGuarding( this );

		// Clear out char's followers
		for( tempChar = activeFollowers.First(); !activeFollowers.Finished(); tempChar = activeFollowers.Next() )
		{
			if( ValidateObject( tempChar ) )
			{
				tempChar->SetOwner( nullptr );
				RemoveFollower( tempChar );
			}
		}

		// Clear out char's list of owned pets
		for( tempChar = petsOwned.First(); !petsOwned.Finished(); tempChar = petsOwned.Next() )
		{
			if( ValidateObject( tempChar ))
			{
				tempChar->SetOwner( nullptr );
			}
		}

		// Clear out char's list of owned corpses
		for( auto tempCorpse = ownedCorpses.First(); !ownedCorpses.Finished(); tempCorpse = ownedCorpses.Next() )
		{
			if( ValidateObject( tempCorpse ))
			{
				tempCorpse->SetOwner( nullptr );
				RemoveCorpse( tempCorpse );
			}
		}

		// If a pet/follower, reduce owner's pet control slot usage by amount occupied by pet/follower
		CChar *oldOwner = GetOwnerObj();
		if( ValidateObject( oldOwner ))
		{
			oldOwner->SetControlSlotsUsed( std::max( 0, oldOwner->GetControlSlotsUsed() - GetControlSlots() ));
			oldOwner->GetFollowerList()->Remove( this );
		}

		RemoveSelfFromOwner();	// Let's remove it from our owner (if any)

		//Ensure that object is removed from refreshQueue
		RemoveFromRefreshQueue();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetHP()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets hitpoints of the character
//o------------------------------------------------------------------------------------------------o
void CChar::SetHP( SI16 newValue )
{
	CBaseObject::SetHP( std::min( std::max( static_cast<SI16>( 0 ), newValue ), static_cast<SI16>( GetMaxHP() )));
	Dirty( UT_HITPOINTS );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetMana()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets mana of the character
//o------------------------------------------------------------------------------------------------o
void CChar::SetMana( SI16 newValue )
{
	CBaseObject::SetMana( std::min( std::max( static_cast<SI16>( 0 ), newValue ), GetMaxMana() ));
	Dirty( UT_MANA );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetStamina()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets stamina of the character
//o------------------------------------------------------------------------------------------------o
void CChar::SetStamina( SI16 newValue )
{
	CBaseObject::SetStamina( std::min( std::max( static_cast<SI16>( 0 ), newValue ), GetMaxStam() ));
	Dirty( UT_STAMINA );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetPoisoned()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets poisoned state of character
//o------------------------------------------------------------------------------------------------o
void CChar::SetPoisoned( UI08 newValue )
{
	CBaseObject::SetPoisoned( newValue );
	Dirty( UT_UPDATE );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetStrength2()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets bonus strength stat for character
//o------------------------------------------------------------------------------------------------o
void CChar::SetStrength2( SI16 nVal )
{
	CBaseObject::SetStrength2( nVal );
	Dirty( UT_HITPOINTS );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetDexterity2()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets bonus dexterity stat for character
//o------------------------------------------------------------------------------------------------o
void CChar::SetDexterity2( SI16 nVal )
{
	CBaseObject::SetDexterity2( nVal );
	Dirty( UT_STAMINA );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetIntelligence2()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets bonus intelligence stat for character
//o------------------------------------------------------------------------------------------------o
void CChar::SetIntelligence2( SI16 nVal )
{
	CBaseObject::SetIntelligence2( nVal );
	Dirty( UT_MANA );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IncStamina()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Increase character's stamina by specified value
//o------------------------------------------------------------------------------------------------o
void CChar::IncStamina( SI16 toInc )
{
	SetStamina( GetStamina() + toInc );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::IncMana()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Increase character's mana by specified value
//o------------------------------------------------------------------------------------------------o
void CChar::IncMana( SI16 toInc )
{
	SetMana( GetMana() + toInc );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::ToggleCombat()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle combat mode for NPC
//o------------------------------------------------------------------------------------------------o
void CChar::ToggleCombat( void )
{
	SetWar( !IsAtWar() );
	Movement->CombatWalk( this );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CanBeObjType()
//|	Date		-	24 June, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a	particular type (Char, in this case)
//o------------------------------------------------------------------------------------------------o
bool CChar::CanBeObjType( ObjectType toCompare ) const
{
	bool rValue = CBaseObject::CanBeObjType( toCompare );
	if( !rValue )
	{
		if( toCompare == OT_CHAR )
		{
			rValue = true;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::Delete()
//| Date		-	11/6/2003
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds character to deletion queue
//o------------------------------------------------------------------------------------------------o
void CChar::Delete( void )
{
	if( cwmWorldState->deletionQueue.count( this ) == 0 )
	{
		++( cwmWorldState->deletionQueue[this] );
		Cleanup();
		SetDeleted( true );
		ShouldSave( false );
	}
}

// Player Specific Functions

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::SetAccount()
//|					CChar::GetAccount()
//|	Date		-	1/14/2003 6:17:45 AM
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets and Returns the CAccountBlock_st associated with this player
//o------------------------------------------------------------------------------------------------o
void CChar::SetAccount( CAccountBlock_st& actbAccount )
{
	if( !IsValidPlayer() )
	{
		if( actbAccount.wAccountIndex != DEFPLAYER_ACCOUNTNUM )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->accountNum = actbAccount.wAccountIndex;
	}
}
CAccountBlock_st& CChar::GetAccount( void )
{
	UI16 rVal = AB_INVALID_ID;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->accountNum;
	}

	return Accounts->GetAccountById( rVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetAccountNum()
//|					CChar::SetAccountNum()
//|	Date		-	1/14/2003 6:17:45 AM
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets and Returns the account number associated with this player
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetAccountNum( void ) const
{
	UI16 rVal = AB_INVALID_ID;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->accountNum;
	}
	return rVal;
}
void CChar::SetAccountNum( UI16 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_ACCOUNTNUM )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->accountNum = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetRobe()
//|					CChar::SetRobe()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets player's death robe
//o------------------------------------------------------------------------------------------------o
SERIAL CChar::GetRobe( void ) const
{
	SERIAL rVal = DEFPLAYER_ROBE;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->robe;
	}
	return rVal;
}
void CChar::SetRobe( SERIAL newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_ROBE )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->robe = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::LastMoveTime()
//|					CChar::LastMoveTime()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp for when player last moved
//o------------------------------------------------------------------------------------------------o
UI32 CChar::LastMoveTime( void ) const
{
	return lastMoveTime;
}
void CChar::LastMoveTime( UI32 newValue )
{
	lastMoveTime = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetLastOn()
//|					CChar::SetLastOn()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp (in text) for when player was last online
//o------------------------------------------------------------------------------------------------o
std::string CChar::GetLastOn( void ) const
{
	std::string rVal = "";
	if( IsValidPlayer() )
	{
		rVal = mPlayer->lastOn;
	}
	return rVal;
}
void CChar::SetLastOn( std::string newValue )
{
	if( !IsValidPlayer() )
	{
		if( !newValue.empty() )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->lastOn = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetLastOnSecs()
//|					CChar::SetLastOnSecs()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp (in seconds) for when player was last online
//o------------------------------------------------------------------------------------------------o
UI32 CChar::GetLastOnSecs( void ) const
{
	UI32 rVal = 0;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->lastOnSecs;
	}
	return rVal;
}
void CChar::SetLastOnSecs( UI32 newValue )
{
	if( IsValidPlayer() )
	{
		mPlayer->lastOnSecs = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetCreatedOn()
//|					CChar::SetCreatedOn()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp (in seconds) for when player character was created
//o------------------------------------------------------------------------------------------------o
UI32 CChar::GetCreatedOn( void ) const
{
	UI32 rVal = 0;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->createdOn;
	}
	return rVal;
}
void CChar::SetCreatedOn( UI32 newValue )
{
	if( IsValidPlayer() )
	{
		mPlayer->createdOn = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPlayTime()
//|					CChar::SetPlayTime()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets/Updates play time (in minutes) of character since it was created
//o------------------------------------------------------------------------------------------------o
auto CChar::GetPlayTime() const -> UI32
{
	UI32 rVal = 0;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->playTime;
	}
	return rVal;
}
auto CChar::SetPlayTime( UI32 newValue ) -> void
{
	if( IsValidPlayer() )
	{
		mPlayer->playTime = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetAtrophy()
//|					CChar::SetAtrophy()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets skill atrophy for character
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetAtrophy( UI08 skillToGet ) const
{
	UI08 rVal = 0;
	if( IsValidPlayer() && skillToGet <= INTELLECT )
	{
		rVal = mPlayer->atrophy[skillToGet];
	}
	return rVal;
}
void CChar::SetAtrophy( UI08 newValue, UI08 skillToSet )
{
	if( IsValidPlayer() && skillToSet <= INTELLECT )
	{
		mPlayer->atrophy[skillToSet] = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSkillLock()
//|					CChar::SetSkillLock()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets skill (and stat) lock for character
//o------------------------------------------------------------------------------------------------o
SkillLock CChar::GetSkillLock( UI08 skillToGet ) const
{
	SkillLock rVal = SKILL_INCREASE;
	if( IsValidPlayer() && skillToGet <= INTELLECT )
	{
		rVal = mPlayer->lockState[skillToGet];
	}
	return rVal;
}
void CChar::SetSkillLock( SkillLock newValue, UI08 skillToSet )
{
	if( IsValidPlayer() && skillToSet <= INTELLECT )
	{
		mPlayer->lockState[skillToSet] = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetMounted()
//|					CChar::SetMounted()
//| Date		-	23. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets mounted state of character
//o------------------------------------------------------------------------------------------------o
bool CChar::GetMounted( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
	{
		rVal = mNPC->boolFlags.test( BIT_MOUNTED );
	}

	return rVal;
}
void CChar::SetMounted( bool newValue )
{
	if( IsValidNPC() )
	{
		mNPC->boolFlags.set( BIT_MOUNTED, newValue );
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetStabled()
//|					CChar::SetStabled()
//|	Date		-	23. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets stabled state of pet
//o------------------------------------------------------------------------------------------------o
bool CChar::GetStabled( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
	{
		rVal = mNPC->boolFlags.test( BIT_STABLED );
	}
	return rVal;
}
void CChar::SetStabled( bool newValue )
{
	if( IsValidNPC() )
	{
		mNPC->boolFlags.set( BIT_STABLED, newValue );
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsFlying()
//|					CChar::SetFlying()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets flying state of character
//o------------------------------------------------------------------------------------------------o
bool CChar::IsFlying( void ) const
{	
	return bools.test( BIT_FLYING );
}
void CChar::SetFlying( bool newValue )
{
	if( MayLevitate() )
	{
		bools.set( BIT_FLYING, newValue );
	}
}

bool CChar::ToggleFlying( void )
{
	CSocket *tSock = GetSocket();
	if( MayLevitate() )
	{
		if( GetTimer( tCHAR_FLYINGTOGGLE ) <= cwmWorldState->GetUICurrentTime() )
		{
			if( IsFrozen() )
			{
				tSock->SysMessage( 9000 ); // You cannot use this ability while frozen.
			}
			else
			{
				// Start timer for when flying toggle can be used again
				SetTimer( tCHAR_FLYINGTOGGLE, BuildTimeValue( 1.65 ));

				// Freeze character until flying anim is over
				SetFrozen( true );

				// Toggle flying
				SetFlying( !IsFlying() );

				// Refresh our own character
				CPExtMove toSend = ( *this );
				tSock->Send( &toSend );

				// Play takeoff/landing animation
				if( !IsFlying() )
				{
					Effects->PlayNewCharacterAnimation( this, N_ACT_LAND ); // Action 0x0A, subAction 0x00
				}
				else
				{
					Effects->PlayNewCharacterAnimation( this, N_ACT_TAKEOFF ); // Action 0x09, subAction 0x00
				}

				// Send update to nearby characters
				for( auto &tSend : Network->connClients )
				{
					if( tSend )
					{
						CChar *mChar = tSend->CurrcharObj();
						if( ValidateObject( mChar ))
						{
							if(( WorldNumber() == mChar->WorldNumber() ) && (GetInstanceId() == mChar->GetInstanceId() ))
							{
								UI16 effRange = static_cast<UI16>( tSend->Range() );
								const UI16 visibleRange = static_cast<UI16>( tSend->Range() + Races->VisRange( mChar->GetRace() ));
								if( visibleRange >= effRange )
								{
									effRange = visibleRange;
								}
								
								if( this != mChar )
								{
									if( !ObjInRange( this, mChar, effRange ))
										continue;
								}
								
								toSend.FlagColour( static_cast<UI08>( FlagColour( mChar )));
								tSend->Send( &toSend );
							}
						}
					}
				}
			}
		}
	}
	else if( GetId() == 0x029A || GetId() == 0x029B )
	{
		// Enable flying mode for older gargoyle characters that were created before this property was added to character creation
		SetLevitate( true );
	}
	else
	{
		tSock->SysMessage( 9001 ); // This character is not allowed to fly!
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetMaxHPFixed()
//|					CChar::SetMaxHPFixed()
//| Date		-	25. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the maximum hp (fixed) for the character
//o------------------------------------------------------------------------------------------------o
bool CChar::GetMaxHPFixed( void ) const
{
	return bools.test( BIT_MAXHPFIXED );
}
void CChar::SetMaxHPFixed( bool newValue )
{
	bools.set( BIT_MAXHPFIXED, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetMaxManaFixed()
//|					CChar::SetMaxManaFixed()
//|	Date		-	25. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum mana (fixed) for the character
//o------------------------------------------------------------------------------------------------o
bool CChar::GetMaxManaFixed( void ) const
{
	return bools.test( BIT_MAXMANAFIXED );
}
void CChar::SetMaxManaFixed( bool newValue )
{
	bools.set( BIT_MAXMANAFIXED, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetMaxStamFixed()
//|					CChar::SetMaxStamFixed()
//| Date		-	25. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the maximum stamina (fixed) of the character
//o------------------------------------------------------------------------------------------------o
bool CChar::GetMaxStamFixed( void ) const
{
	return bools.test( BIT_MAXSTAMFIXED );
}
void CChar::SetMaxStamFixed( bool newValue )
{
	bools.set( BIT_MAXSTAMFIXED, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOrgName()
//|					CChar::SetOrgName()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's original name
//o------------------------------------------------------------------------------------------------o
std::string CChar::GetOrgName( void ) const
{
	std::string rVal = "";
	if( IsValidPlayer() )
	{
		rVal = mPlayer->origName;
	}
	return rVal;
}
void CChar::SetOrgName( std::string newName )
{
	if( !IsValidPlayer() )
	{
		if( !newName.empty() )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->origName = newName;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetOrgId()
//|					CChar::SetOrgId()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the player character's original ID
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetOrgId( void ) const
{
	UI16 rVal = GetId();
	if( IsValidPlayer() )
	{
		rVal = mPlayer->origId;
	}
	return rVal;
}
void CChar::SetOrgId( UI16 value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_ORIGID )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->origId = value;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOrgSkin()
//|					CChar::SetOrgSkin()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's original skin
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetOrgSkin( void ) const
{
	UI16 rVal = GetSkin();
	if( IsValidPlayer() )
	{
		rVal = mPlayer->origSkin;
	}
	return rVal;
}
void CChar::SetOrgSkin( UI16 value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_ORIGSKIN )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->origSkin = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetHairStyle()
//|					CChar::SetHairStyle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default hairstyle
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetHairStyle( void ) const
{
	UI16 rVal = DEFPLAYER_HAIRSTYLE;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->hairStyle;
	}
	return rVal;
}
void CChar::SetHairStyle( UI16 value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_HAIRSTYLE )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->hairStyle = value;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetBeardStyle()
//|					CChar::SetBeardStyle()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default beardstyle
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetBeardStyle( void ) const
{
	UI16 rVal = DEFPLAYER_BEARDSTYLE;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->beardStyle;
	}
	return rVal;
}
void CChar::SetBeardStyle( UI16 value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_BEARDSTYLE )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->beardStyle = value;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetHairColour()
//|					CChar::SetHairColour()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default haircolour
//o------------------------------------------------------------------------------------------------o
COLOUR CChar::GetHairColour( void ) const
{
	COLOUR rVal = DEFPLAYER_HAIRCOLOUR;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->hairColour;
	}
	return rVal;
}
void CChar::SetHairColour( COLOUR value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_HAIRCOLOUR )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->hairColour = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetBeardColour()
//|					CChar::SetBeardColour()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's default beardcolour
//o------------------------------------------------------------------------------------------------o
COLOUR CChar::GetBeardColour( void ) const
{
	COLOUR rVal = DEFPLAYER_BEARDCOLOUR;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->beardColour;
	}
	return rVal;
}
void CChar::SetBeardColour( COLOUR value )
{
	if( !IsValidPlayer() )
	{
		if( value != DEFPLAYER_BEARDCOLOUR )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->beardColour = value;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTrackingTarget()
//|					CChar::SetTrackingTarget()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's tracking target
//o------------------------------------------------------------------------------------------------o
CChar *CChar::GetTrackingTarget( void ) const
{
	CChar *rVal = nullptr;
	if( IsValidPlayer() )
	{
		rVal = CalcCharObjFromSer( mPlayer->trackingTarget );
	}
	return rVal;
}
void CChar::SetTrackingTarget( CChar *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->trackingTarget = CalcSerFromObj( newValue );
	}
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

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTrackingTargets()
//|					CChar::SetTrackingTargets()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets list of possible tracking targets
//o------------------------------------------------------------------------------------------------o
CChar *CChar::GetTrackingTargets( UI08 targetNum ) const
{
	CChar *rVal = nullptr;
	if( IsValidPlayer() )
	{
		if( targetNum < mPlayer->trackingTargets.size() )
		{
			rVal = mPlayer->trackingTargets[targetNum];
		}
	}
	return rVal;
}
void CChar::SetTrackingTargets( CChar *newValue, UI08 targetNum )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		if( targetNum < mPlayer->trackingTargets.size() )
		{
			mPlayer->trackingTargets[targetNum] = newValue;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetCommandLevel()
//|					CChar::SetCommandLevel()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's command level
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetCommandLevel( void ) const
{
	UI08 rVal = DEFPLAYER_COMMANDLEVEL;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->commandLevel;
	}
	return rVal;
}
void CChar::SetCommandLevel( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_COMMANDLEVEL )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->commandLevel = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPostType()
//|					CChar::SetPostType()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets player character's messageboard posting level
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetPostType( void ) const
{
	UI08 rVal = DEFPLAYER_POSTTYPE;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->postType;
	}
	return rVal;
}
void CChar::SetPostType( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_POSTTYPE )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->postType = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetCallNum()
//|					CChar::SetCallNum()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the currently active GM/Counselor call for the character
//o------------------------------------------------------------------------------------------------o
SERIAL CChar::GetCallNum( void ) const
{
	SI16 rVal = DEFPLAYER_CALLNUM;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->callNum;
	}
	return rVal;
}
void CChar::SetCallNum( SERIAL newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != INVALIDSERIAL )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->callNum = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPlayerCallNum()
//|					CChar::SetPlayerCallNum()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the player character's call number in GM/CNS Queue
//o------------------------------------------------------------------------------------------------o
SERIAL CChar::GetPlayerCallNum( void ) const
{
	SERIAL rVal = DEFPLAYER_PLAYERCALLNUM;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->playerCallNum;
	}
	return rVal;
}
void CChar::SetPlayerCallNum( SERIAL newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != INVALIDSERIAL )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->playerCallNum = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSquelched()
//|					CChar::SetSquelched()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the squelched status of the player's character
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetSquelched( void ) const
{
	UI08 rVal = DEFPLAYER_SQUELCHED;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->squelched;
	}
	return rVal;
}
void CChar::SetSquelched( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_SQUELCHED )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->squelched = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpeechItem()
//|					CChar::SetSpeechItem()
//| Date		-	April 8th, 2000
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets item related to the speech we're working on
//|					IE the item for name deed if we're renaming ourselves
//o------------------------------------------------------------------------------------------------o
CItem *CChar::GetSpeechItem( void ) const
{
	CItem *rVal = nullptr;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->speechItem;
	}
	return rVal;
}
void CChar::SetSpeechItem( CItem *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->speechItem = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpeechMode()
//|					CChar::SetSpeechMode()
//| Date		-	April 8th, 2000
//o------------------------------------------------------------------------------------------------o
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
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetSpeechMode( void ) const
{
	UI08 rVal = DEFPLAYER_SPEECHMODE;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->speechMode;
	}
	return rVal;
}
void CChar::SetSpeechMode( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_SPEECHMODE )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->speechMode = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpeechId()
//|					CChar::SetSpeechId()
//| Date		-	January 20th, 2002
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the ID for the JS Speech Input
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetSpeechId( void ) const
{
	UI08 rVal = DEFPLAYER_SPEECHID;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->speechId;
	}
	return rVal;
}
void CChar::SetSpeechId( UI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_SPEECHID )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
		mPlayer->speechId = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpeechCallback()
//|					CChar::SetSpeechCallback()
//| Date		-	January 20th, 2002
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets callback for the onSpeechInput function
//o------------------------------------------------------------------------------------------------o
cScript *CChar::GetSpeechCallback( void ) const
{
	cScript *rVal = nullptr;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->speechCallback;
	}
	return rVal;
}
void CChar::SetSpeechCallback( cScript *newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != nullptr )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
		mPlayer->speechCallback = newValue;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFixedLight()
//|					CChar::SetFixedLight()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets fixed light level of the character
//|					255 is off
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetFixedLight( void ) const
{
	UI08 rVal = DEFPLAYER_FIXEDLIGHT;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->fixedLight;
	}
	return rVal;
}
void CChar::SetFixedLight( UI08 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_FIXEDLIGHT )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->fixedLight = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetDeaths()
//|					CChar::SetDeaths()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Sets the total number of deaths a player has
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetDeaths( void ) const
{
	UI16 rVal = DEFPLAYER_DEATHS;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->deaths;
	}
	return rVal;
}
void CChar::SetDeaths( UI16 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_DEATHS )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->deaths = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSocket()
//|					CChar::SetSocket()
//| Date		-	November 7, 2005
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets socket attached to the player character
//o------------------------------------------------------------------------------------------------o
CSocket * CChar::GetSocket( void ) const
{
	CSocket *rVal = nullptr;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->socket;
	}
	return rVal;
}
void CChar::SetSocket( CSocket *newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != nullptr )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
		mPlayer->socket = newVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTownVote()
//|					CChar::SetTownVote()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets serial of the player a character has voted to be mayor.
//o------------------------------------------------------------------------------------------------o
UI32 CChar::GetTownVote( void ) const
{
	UI32 retVal = DEFPLAYER_TOWNVOTE;
	if( IsValidPlayer() )
	{
		retVal = mPlayer->townVote;
	}
	return retVal;
}
void CChar::SetTownVote( UI32 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_TOWNVOTE )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->townVote = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTownPriv()
//|					CChar::SetTownpriv()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets town member privledges (1 = Resident, 2 = Mayor)
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetTownPriv( void ) const
{
	SI08 retVal = DEFPLAYER_TOWNPRIV;
	if( IsValidPlayer() )
	{
		retVal = mPlayer->townPriv;
	}
	return retVal;
}
void CChar::SetTownpriv( SI08 newValue )
{
	if( !IsValidPlayer() )
	{
		if( newValue != DEFPLAYER_TOWNPRIV )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->townPriv = newValue;
		UpdateRegion();
	}
}

// NPC Specific Functions

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetMaxLoyalty()
//|					CChar::SetMaxLoyalty()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the max loyalty of the NPC
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetMaxLoyalty( void ) const
{
	UI16 retVal = DEFNPC_MAXLOYALTY;
	if( IsValidNPC() )
	{
		retVal = mNPC->maxLoyalty;
	}
	return retVal;
}
void CChar::SetMaxLoyalty( UI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_MAXLOYALTY != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->maxLoyalty = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetLoyalty()
//|					CChar::SetLoyalty()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the loyalty of the NPC
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetLoyalty( void ) const
{
	UI16 retVal = DEFNPC_LOYALTY;
	if( IsValidNPC() )
	{
		retVal = mNPC->loyalty;
	}
	return retVal;
}
void CChar::SetLoyalty( UI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_LOYALTY != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		std::vector<UI16> scriptTriggers = GetScriptTriggers();
		for( auto i : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				// If script returns false/0/nothing, prevent loyalty from changing, and prevent
				// other scripts with event from running
				if( toExecute->OnLoyaltyChange(( this ), hunger ) == 0 )
				{
					return;
				}
			}
		}

		mNPC->loyalty = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function    -	CChar::DoLoyaltyUpdate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle automatic loyalty change for the NPC character
//o------------------------------------------------------------------------------------------------o
void CChar::DoLoyaltyUpdate( void )
{
	// Don't continue if pet control difficulty system is disabled
	if( !cwmWorldState->ServerData()->CheckPetControlDifficulty() )
		return;

	// No need to do anything for dead or non-tame NPCs, or player vendors
	if( !IsNpc() || IsDead() || !IsTamed() || GetNpcAiType() == AI_PLAYERVENDOR )
		return;

	if( !ValidateObject( GetOwnerObj() ))
		return;

	if( GetTimer( tNPC_LOYALTYTIME ) <= cwmWorldState->GetUICurrentTime() || cwmWorldState->GetOverflow() )
	{
		UI16 loyaltyRate = cwmWorldState->ServerData()->SystemTimer( tSERVER_LOYALTYRATE );
		if( GetLoyalty() > 0 )
		{
			// Reduce loyalty by 1, reset timer
			SetLoyalty( std::max( 0, GetLoyalty() - 1 ));
			SetTimer( tNPC_LOYALTYTIME, BuildTimeValue( static_cast<R32>( loyaltyRate )));

			// Provide some feedback to the player, if they're online
			CSocket *mSock = GetOwnerObj()->GetSocket();
			if( mSock != nullptr )
			{
				if(( GetLoyalty() % 10 ) == 0 )
				{
					// Round loyalty down to nearest 10
					auto loyaltyLevel = 10 * ( GetLoyalty() / 10 );
					switch( loyaltyLevel )
					{
						case 0:
							TextMessage( mSock, 2421, OBJ, true ); // Your pet looks confused
							break;
						case 10:
							TextMessage( mSock, 2422, OBJ, true ); // Your pet looks extremely unhappy
							break;
						case 20:
							TextMessage( mSock, 2423, OBJ, true ); // Your pet looks rather unhappy
							break;
						case 30:
							TextMessage( mSock, 2424, OBJ, true ); // Your pet looks unhappy
							break;
						case 40:
							TextMessage( mSock, 2425, OBJ, true ); // Your pet looks somewhat content
							break;
						case 50:
							TextMessage( mSock, 2426, OBJ, true ); // Your pet looks content
							break;
						case 60:
							TextMessage( mSock, 2427, OBJ, true ); // Your pet looks happy
							break;
						case 70:
							TextMessage( mSock, 2428, OBJ, true ); // Your pet looks rather happy
							break;
						case 80:
							TextMessage( mSock, 2429, OBJ, true ); // Your pet looks very happy
							break;
						case 90:
							TextMessage( mSock, 2430, OBJ, true ); // Your pet looks extremely happy
							break;
						case 100:
							TextMessage( mSock, 2431, OBJ, true ); // Your pet looks wonderfully happy
							break;
						default:
							break;
					}
				}
			}

			// Is loyalty already at the lowest threshold from the last time this check ran?
			if( GetLoyalty() == 0 )
			{
				// Make pet go wild from lack of loyalty
				CChar *owner = GetOwnerObj();
				if( ValidateObject( owner ))
				{
					// Reduce player's control slot usage by the amount of control slots taken up by the pet
					owner->SetControlSlotsUsed( std::max( 0, owner->GetControlSlotsUsed() - GetControlSlots() ));
				}

				// Release the pet
				Npcs->ReleasePet( this );
				return;
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTamedHungerRate()
//|					CChar::SetTamedHungerRate()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Set the rate at which a pet hungers
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetTamedHungerRate( void ) const
{
	UI16 retVal = DEFNPC_TAMEDHUNGERRATE;
	if( IsValidNPC() )
	{
		retVal = mNPC->tamedHungerRate;
	}
	return retVal;
}
void CChar::SetTamedHungerRate( UI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_TAMEDHUNGERRATE != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->tamedHungerRate = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::GetTamedThirstRate()
//|                 CChar::SetTamedThirstRate()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Get/Set the rate at which a pet thirsts
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetTamedThirstRate( void ) const
{
	UI16 retVal = DEFNPC_TAMEDTHIRSTRATE;
	if( IsValidNPC() )
	{
		retVal = mNPC->tamedThirstRate;
	}
	return retVal;
}
void CChar::SetTamedThirstRate( UI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_TAMEDTHIRSTRATE != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->tamedThirstRate = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTamedHungerWildChance()
//|					CChar::SetTamedHungerWildChance()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets chance for a hungry pet to go wild
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetTamedHungerWildChance( void ) const
{
	UI08 retVal = DEFNPC_HUNGERWILDCHANCE;
	if( IsValidNPC() )
	{
		retVal = mNPC->hungerWildChance;
	}
	return retVal;
}
void CChar::SetTamedHungerWildChance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_HUNGERWILDCHANCE != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->hungerWildChance = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::GetTamedThirstWildChance()
//|                 CChar::SetTamedThirstWildChance()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets chance for a thirsty pet to go wild
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetTamedThirstWildChance( void ) const
{
	UI08 retVal = DEFNPC_THIRSTWILDCHANCE;
	if( IsValidNPC() )
	{
		retVal = mNPC->thirstWildChance;
	}
	return retVal;
}
void CChar::SetTamedThirstWildChance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_THIRSTWILDCHANCE != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->thirstWildChance = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFood()
//|					CChar::SetFood()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets FOODLIST entry for feeding tamed pets
//o------------------------------------------------------------------------------------------------o
std::string CChar::GetFood( void ) const
{
	std::string retVal = "";
	if( IsValidNPC() )
	{
		retVal = mNPC->foodList;
	}
	return retVal;
}
void CChar::SetFood( std::string food )
{
	if( !IsValidNPC() )
	{
		if( !food.empty() )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->foodList = food.substr( 0, MAX_NAME - 1 );
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetNpcAiType()
//|					CChar::SetNPCAiType()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the AI type of the NPC
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetNpcAiType( void ) const
{
	SI16 rVal = DEFNPC_AITYPE;
	if( IsValidNPC() )
	{
		rVal = mNPC->aiType;
	}
	return rVal;
}
void CChar::SetNPCAiType( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_AITYPE )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->aiType = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetNPCGuild()
//|					CChar::SetNPCGuild()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC Guild the character belongs to
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetNPCGuild( void ) const
{
	return npcGuild;
}
void CChar::SetNPCGuild( UI16 newValue )
{
	npcGuild = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetNPCGuildJoined()
//|					CChar::SetNPCGuildJoined()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets timestamp (in seconds) for when player character joined NPC guild
//o------------------------------------------------------------------------------------------------o
UI32 CChar::GetNPCGuildJoined( void ) const
{
	UI32 rVal = 0;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->npcGuildJoined;
	}
	return rVal;
}
void CChar::SetNPCGuildJoined( UI32 newValue )
{
	if( IsValidPlayer() )
	{
		mPlayer->npcGuildJoined = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetGuarding()
//|					CChar::SetGuarding()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the SERIAL of the object the character is guarding
//o------------------------------------------------------------------------------------------------o
CBaseObject *CChar::GetGuarding( void ) const
{
	CBaseObject *rVal = nullptr;
	if( IsValidNPC() )
	{
		rVal = mNPC->petGuarding;
	}
	return rVal;
}
void CChar::SetGuarding( CBaseObject *newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != nullptr )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->petGuarding = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTaming()
//|					CChar::SetTaming()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets minimum skill required to tame the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetTaming( void ) const
{
	SI16 rVal = DEFNPC_TAMING;
	if( IsValidNPC() )
	{
		rVal = mNPC->taming;
	}
	return rVal;
}
void CChar::SetTaming( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_TAMING )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->taming = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPeaceing()
//|					CChar::SetPeaceing()
//| Date		-	25. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets minimum skill required to peace the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetPeaceing( void ) const
{
	SI16 rVal = DEFNPC_PEACEING;
	if( IsValidNPC() )
	{
		rVal = mNPC->peaceing;
	}
	else if( IsValidPlayer() )
	{
		rVal = static_cast<SI16>( GetBaseSkill( PEACEMAKING ));
	}
	return rVal;
}
void CChar::SetPeaceing( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_PEACEING )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->peaceing = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetProvoing()
//|					CChar::SetProvoing()
//| Date		-	25. Feb, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets minimum skill required to provocate the character
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetProvoing( void ) const
{
	SI16 rVal = DEFNPC_PROVOING;
	if( IsValidNPC() )
	{
		rVal = mNPC->provoing;
	}
	else if( IsValidPlayer() )
	{
		rVal = static_cast<SI16>( GetBaseSkill( PROVOCATION ));
	}
	return rVal;
}
void CChar::SetProvoing( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_PROVOING )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->provoing = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetTrainingPlayerIn()
//|					CChar::SetTrainingPlayerIn()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets skill the player is being trained in. 255 is no training
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetTrainingPlayerIn( void ) const
{
	UI08 rVal = DEFNPC_TRAININGPLAYERIN;
	if( IsValidNPC() )
	{
		rVal = mNPC->trainingPlayerIn;
	}
	return rVal;
}
void CChar::SetTrainingPlayerIn( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_TRAININGPLAYERIN )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->trainingPlayerIn = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetHoldG()
//|					CChar::SetHoldG()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets amount of gold being held by a Player Vendor
//o------------------------------------------------------------------------------------------------o
UI32 CChar::GetHoldG( void ) const
{
	UI32 rVal = DEFNPC_HOLDG;
	if( IsValidNPC() )
	{
		rVal = mNPC->goldOnHand;
	}
	return rVal;
}
void CChar::SetHoldG( UI32 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_HOLDG )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->goldOnHand = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSplit()
//|					CChar::SetSplit()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets split level of the character
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetSplit( void ) const
{
	UI08 rVal = DEFNPC_SPLIT;
	if( IsValidNPC() )
	{
		rVal = mNPC->splitNum;
	}
	return rVal;
}
void CChar::SetSplit( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPLIT )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->splitNum = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSplitChance()
//|					CChar::SetSplitChance()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the chance of an NPC splitting
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetSplitChance( void ) const
{
	UI08 rVal = DEFNPC_SPLITCHANCE;
	if( IsValidNPC() )
	{
		rVal = mNPC->splitChance;
	}
	return rVal;
}
void CChar::SetSplitChance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPLITCHANCE )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->splitChance = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFx()
//|					CChar::SetFx()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets x1 and x2 boundry of an npc wander area
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetFx( UI08 part ) const
{
	SI16 rVal = DEFNPC_WANDERAREA;
	if( IsValidNPC() )
	{
		if( part < 2 )
		{
			rVal = mNPC->fx[part];
		}
	}
	return rVal;
}
void CChar::SetFx( SI16 newVal, UI08 part )
{
	if( !IsValidNPC() )
	{
		if( newVal != DEFNPC_WANDERAREA )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		if( part < 2 )
		{
			mNPC->fx[part] = newVal;
			UpdateRegion();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFy()
//|					CChar::SetFy()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets y1 and y2 boundry of an npc wander area
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetFy( UI08 part ) const
{
	SI16 rVal = DEFNPC_WANDERAREA;
	if( IsValidNPC() )
	{
		if( part < 2 )
		{
			rVal = mNPC->fy[part];
		}
	}
	return rVal;
}
void CChar::SetFy( SI16 newVal, UI08 part )
{
	if( !IsValidNPC() )
	{
		if( newVal != DEFNPC_WANDERAREA )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		if( part < 2 )
		{
			mNPC->fy[part] = newVal;
			UpdateRegion();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFz()
//|					CChar::SetFz()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets z of an npc wander area
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetFz( void ) const
{
	SI08 rVal = DEFNPC_FZ1;
	if( IsValidNPC() )
	{
		rVal = mNPC->fz;
	}
	return rVal;
}
void CChar::SetFz( SI08 newVal )
{
	if( !IsValidNPC() )
	{
		if( newVal != DEFNPC_FZ1 )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->fz = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetNpcWander()
//|					CChar::SetNpcWander()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC Wander mode
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetNpcWander( void ) const
{
	SI08 rVal = DEFNPC_WANDER;
	if( IsValidNPC() )
	{
		rVal = mNPC->wanderMode;
	}
	return rVal;
}
void CChar::SetNpcWander( SI08 newValue, bool initArea )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_WANDER )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->wanderMode = newValue;
		UpdateRegion();
	}

	// Make sure the wanderarea is properly initialized
	if( initArea && (( newValue == WT_BOX || newValue == WT_CIRCLE )))
	{
		if(( mNPC->fx[0] == -1 ) || ( mNPC->fy[0] == -1 ))
		{
			InitializeWanderArea( this, 10, 10 );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOldNpcWander()
//|					CChar::SetOldNpcWander()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets previous NPC Wander mode
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetOldNpcWander( void ) const
{
	SI08 rVal = DEFNPC_OLDWANDER;
	if( IsValidNPC() )
	{
		rVal = mNPC->oldWanderMode;
	}
	return rVal;
}
void CChar::SetOldNpcWander( SI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_OLDWANDER )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->oldWanderMode = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFTarg()
//|					CChar::SetFTarg()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC Follow target
//o------------------------------------------------------------------------------------------------o
CChar *CChar::GetFTarg( void ) const
{
	CChar *rVal = nullptr;
	if( IsValidNPC() )
	{
		rVal = CalcCharObjFromSer( mNPC->fTarg );
	}
	return rVal;
}
void CChar::SetFTarg( CChar *newTarg )
{
	if( !IsValidNPC() )
	{
		if( newTarg != nullptr )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->fTarg = CalcSerFromObj( newTarg );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpAttack()
//|					CChar::SetSpAttack()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Spell Attack setting
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetSpAttack( void ) const
{
	SI16 rVal = DEFNPC_SPATTACK;
	if( IsValidNPC() )
	{
		rVal = mNPC->spellAttack;
	}
	return rVal;
}
void CChar::SetSpAttack( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPATTACK )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->spellAttack = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetSpDelay()
//|					CChar::SetSpDelay()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Spell Delay setting
//o------------------------------------------------------------------------------------------------o
SI08 CChar::GetSpDelay( void ) const
{
	SI08 rVal = DEFNPC_SPADELAY;
	if( IsValidNPC() )
	{
		rVal = mNPC->spellDelay;
	}
	return rVal;
}
void CChar::SetSpDelay( SI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_SPADELAY )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->spellDelay = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetQuestType()
//|					CChar::SetQuestType()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Quest Type
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetQuestType( void ) const
{
	UI08 rVal = DEFNPC_QUESTTYPE;
	if( IsValidNPC() )
	{
		rVal = mNPC->questType;
	}
	return rVal;
}
void CChar::SetQuestType( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_QUESTTYPE )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->questType = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetQuestOrigRegion()
//|					CChar::SetQuestOrigRegion()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Quest Origin Region - used for escort quests
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetQuestOrigRegion( void ) const
{
	UI08 rVal = DEFNPC_QUESTORIGREGION;
	if( IsValidNPC() )
	{
		rVal = mNPC->questOrigRegion;
	}
	return rVal;
}
void CChar::SetQuestOrigRegion( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_QUESTORIGREGION )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->questOrigRegion = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetQuestDestRegion()
//|					CChar::SetQuestDestRegion()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets NPC's Quest Destination Region
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetQuestDestRegion( void ) const
{
	UI08 rVal = DEFNPC_QUESTDESTREGION;
	if( IsValidNPC() )
	{
		rVal = mNPC->questDestRegion;
	}
	return rVal;
}
void CChar::SetQuestDestRegion( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_QUESTDESTREGION )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->questDestRegion = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFleeAt()
//|					CChar::SetFleeAt()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets health value at which an NPC will turn tail and run
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetFleeAt( void ) const
{
	SI16 rVal = DEFNPC_FLEEAT;
	if( IsValidNPC() )
	{
		rVal = mNPC->fleeAt;
	}
	return rVal;
}
void CChar::SetFleeAt( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_FLEEAT )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->fleeAt = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetReattackAt()
//|					CChar::SetReattackAt()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets health value at which an NPC will start fighting again
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetReattackAt( void ) const
{
	SI16 rVal = DEFNPC_REATTACKAT;
	if( IsValidNPC() )
	{
		rVal = mNPC->reAttackAt;
	}
	return rVal;
}
void CChar::SetReattackAt( SI16 newValue )
{
	if( !IsValidNPC() )
	{
		if( newValue != DEFNPC_REATTACKAT )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->reAttackAt = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function    -   CChar::GetFleeDistance()
//|                 CChar::SetFleeDistance()
//o------------------------------------------------------------------------------------------------o
//| Purpose     -   Gets/Sets the distance an NPC has moved since entering flee/scared wander mode
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetFleeDistance( void ) const
{
	UI08 retVal = DEFNPC_FLEEDISTANCE;
	if( IsValidNPC() )
	{
		retVal = mNPC->fleeDistance;
	}
	return retVal;
}
void CChar::SetFleeDistance( UI08 newValue )
{
	if( !IsValidNPC() )
	{
		if( DEFNPC_FLEEDISTANCE != newValue )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		mNPC->fleeDistance = newValue;
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::PopDirection()
//|					CChar::PushDirection()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets queue of directions for NPC
//o------------------------------------------------------------------------------------------------o
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
	{
		CreateNPC();
	}

	if( pushFront )
	{
		mNPC->pathToFollow.push_front( newDir );
	}
	else
	{
		mNPC->pathToFollow.push_back( newDir );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::StillGotDirs()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Check if NPC's Direction Queue is empty
//o------------------------------------------------------------------------------------------------o
bool CChar::StillGotDirs( void ) const
{
	bool rVal = false;
	if( IsValidNPC() )
	{
		rVal = !mNPC->pathToFollow.empty();
	}
	return rVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::FlushPath()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Empty NPC's Queue of Directions.
//o------------------------------------------------------------------------------------------------o
void CChar::FlushPath( void )
{
	if( IsValidNPC() )
	{
		while( StillGotDirs() )
		{
			PopDirection();
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetCombatIgnore()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the list of targets ignored by NPC in combat
//o------------------------------------------------------------------------------------------------o
auto CChar::GetCombatIgnore() const -> const std::unordered_map<SERIAL, TargetInfo>
{
	return mNPC->combatIgnore;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::AddToCombatIgnore()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Adds serial to list of targets being ignored by NPC in combat, along
//|					with timestamp for when the ignoring of that serial expires
//o------------------------------------------------------------------------------------------------o
auto CChar::AddToCombatIgnore( SERIAL toAdd, bool isNpc ) -> void
{
	if( mNPC->combatIgnore.count( toAdd ) == 0 )
	{
		// Not found in list already, add to list
		mNPC->combatIgnore[toAdd] = {cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_COMBATIGNORE ), isNpc};
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-  CChar::RemoveFromCombatIgnore()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-  Removes serial from list of targets being ignored by NPC in combat
//o------------------------------------------------------------------------------------------------o
auto CChar::RemoveFromCombatIgnore( SERIAL toRemove ) -> void
{
	mNPC->combatIgnore.erase( toRemove );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CheckCombatIgnore()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if serial is in NPC's list of ignored targets in combat, and 
//|					returns true if it is, or false if it is not - or if the timestamp has expired
//|					(at which point it also removes the entry from the list)
//o------------------------------------------------------------------------------------------------o
auto CChar::CheckCombatIgnore( SERIAL toCheck ) -> bool
{
	auto it = mNPC->combatIgnore.find( toCheck );
	if( it != mNPC->combatIgnore.end() )
	{
		// Serial found, but timestamp might be out of date!
		if(  it->second.timestamp > cwmWorldState->GetUICurrentTime() )
		{
			// timestamp is still valid, still ignoring target in combat
			return true;
		}
		else
		{
			// Timestamp has expired - remove it from the list
			RemoveFromCombatIgnore( toCheck );
		}
	}

	return false; // serial not found, or timestamp expired and target no longer ignored in combat
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::CombatIgnoreMaintenance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Performs maintenance on list of character serials being ignored by NPC in combat
//o------------------------------------------------------------------------------------------------o
auto CChar::CombatIgnoreMaintenance() -> void
{
	// Loop through list of ignored targets in combat and add any serials with expired timers to a vector
	std::vector<SERIAL> serialsToRemove;
	for( auto it = mNPC->combatIgnore.begin(); it != mNPC->combatIgnore.end(); ++it )
	{
		if( it->second.timestamp <= cwmWorldState->GetUICurrentTime() )
		{
			serialsToRemove.push_back( it->first );
		}
	}

	// Then loop through vector of serials with expired timers, and safely remove them from the list
	for( SERIAL toRemove : serialsToRemove )
	{
		RemoveFromCombatIgnore( toRemove );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ClearCombatIgnore()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the NPC's entire list of permagrey flags
//o------------------------------------------------------------------------------------------------o
auto CChar::ClearCombatIgnore() -> void
{
	mNPC->combatIgnore.clear();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetPetOwnerList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the pet's list of previous owners
//o------------------------------------------------------------------------------------------------o
GenericList<CChar *> *CChar::GetPetOwnerList( void )
{
	return &mNPC->petOwnerList;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ClearPetOwnerList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the pet's list of previous owners
//o------------------------------------------------------------------------------------------------o
auto CChar::ClearPetOwnerList() -> void
{
	GetPetOwnerList()->Clear();
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AddPetOwnerToList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds a character to a pet's list of previous owners
//o------------------------------------------------------------------------------------------------o
bool CChar::AddPetOwnerToList( CChar *toAdd )
{
	if( ValidateObject( toAdd ))
	{
		if( GetPetOwnerList()->Add( toAdd ))
		{
			UpdateRegion();
			return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::RemovePetOwnerFromList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Removes a character from a pet's list of previous owners
//o------------------------------------------------------------------------------------------------o
bool CChar::RemovePetOwnerFromList( CChar *toRemove )
{
	if( ValidateObject( toRemove ))
	{
		if( GetPetOwnerList()->Remove( toRemove ))
		{
			UpdateRegion();
			return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::IsOnPetOwnerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is a pet's list of previous owners
//o------------------------------------------------------------------------------------------------o
bool CChar::IsOnPetOwnerList( CChar *toCheck )
{
	bool retVal = false;
	if( ValidateObject( toCheck ))
	{
		auto iter = std::find_if( GetPetOwnerList()->collection().begin(), GetPetOwnerList()->collection().end(), [&toCheck]( CChar *petChar )
		{
			return ValidateObject( petChar ) && ( petChar == toCheck );
		});
		if( iter != GetPetOwnerList()->collection().end() )
		{
			retVal = true;
		}
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ClearFriendList()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the characters list of friends
//o------------------------------------------------------------------------------------------------o
auto CChar::ClearFriendList() -> void
{
	mNPC->petFriends.clear();
	mNPC->petFriends.shrink_to_fit();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFriendList()
//| Date		-	20 July 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns the characters list of friends
//o------------------------------------------------------------------------------------------------o
auto CChar::GetFriendList() -> std::vector<CChar *>*
{
	std::vector<CChar *> *rVal = nullptr;
	if( IsValidNPC() )
	{
		rVal = &mNPC->petFriends;
	}
	return rVal;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::AddFriend()
//| Date		-	20 July 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Adds the friend toAdd to the NPC's pet/follower's friends list
//|					ensuring it is already not on it
//o------------------------------------------------------------------------------------------------o
bool CChar::AddFriend( CChar *toAdd )
{
	if( !IsValidNPC() )
	{
		if( toAdd != nullptr )
		{
			CreateNPC();
		}
	}
	if( IsValidNPC() )
	{
		auto iter = std::find_if( mNPC->petFriends.begin(), mNPC->petFriends.end(), [toAdd]( CChar *entry )
		{
			return entry == toAdd;
		});
		if( iter == mNPC->petFriends.end() ) // wasnt found, add it
		{
			mNPC->petFriends.push_back( toAdd );
			return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::RemoveFriend()
//| Date		-	20 July 2001
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Removes the friend toRemove from the NPC pet/follower's friends list
//o------------------------------------------------------------------------------------------------o
bool CChar::RemoveFriend( CChar *toRemove )
{
	if( IsValidNPC() )
	{
		auto iter = std::find_if( mNPC->petFriends.begin(), mNPC->petFriends.end(), [toRemove]( CChar *entry )
		{
			return toRemove == entry;
		});
		if( iter != mNPC->petFriends.end() )
		{
			mNPC->petFriends.erase( iter );
			return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOwnerCount()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Get the total number of owners a pet/follower has had
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetOwnerCount( void )
{
	return static_cast<UI08>( GetPetOwnerList()->Num() );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetControlSlotsUsed()
//|					CChar::SetControlSlotsUsed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Sets the total number of control slots used by a player's active pets/followers
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetControlSlotsUsed( void ) const
{
	UI08 rVal = DEFPLAYER_CONTROLSLOTSUSED;
	if( IsValidPlayer() )
	{
		rVal = mPlayer->controlSlotsUsed;
	}
	return rVal;
}
void CChar::SetControlSlotsUsed( UI08 newVal )
{
	if( !IsValidPlayer() )
	{
		if( newVal != DEFPLAYER_CONTROLSLOTSUSED )
		{
			CreatePlayer();
		}
	}
	if( IsValidPlayer() )
	{
		mPlayer->controlSlotsUsed = newVal;
		Dirty( UT_STATWINDOW );
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetControlSlots()
//|					CChar::SetControlSlots()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Sets the total number of control slots an NPC takes up
//o------------------------------------------------------------------------------------------------o
UI08 CChar::GetControlSlots( void ) const
{
	UI08 rVal = DEFNPC_CONTROLSLOTS;
	if( IsValidNPC() )
	{
		rVal = mNPC->controlSlots;
	}
	return rVal;
}
void CChar::SetControlSlots( UI08 newVal )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}
	if( IsValidNPC() )
	{
		mNPC->controlSlots = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetOrneriness()
//|					CChar::SetOrneriness()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Get/Sets the difficulty of controlling a pet
//o------------------------------------------------------------------------------------------------o
UI16 CChar::GetOrneriness( void ) const
{
	UI16 rVal = DEFNPC_ORNERINESS;
	if( IsValidNPC() )
	{
		rVal = mNPC->orneriness;
	}
	return rVal;
}
void CChar::SetOrneriness( UI16 newVal )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}
	if( IsValidNPC() )
	{
		mNPC->orneriness = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetNPCFlag()
//|					CChar::SetNPCFlag()
//| Date		-	February 9, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's default flag color
//o------------------------------------------------------------------------------------------------o
cNPC_FLAG CChar::GetNPCFlag( void ) const
{
	cNPC_FLAG retVal = fNPC_NEUTRAL;

	if( IsValidNPC() )
	{
		retVal = mNPC->npcFlag;
	}

	return retVal;
}
void CChar::SetNPCFlag( cNPC_FLAG flagType )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->npcFlag = flagType;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetWalkingSpeed()
//|					CChar::SetWalkingSpeed()
//| Date		-	Juni 9, 2007
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's walking speed
//o------------------------------------------------------------------------------------------------o
R32 CChar::GetWalkingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCWalkingSpeed();

	if( IsValidNPC() )
	{
		if( mNPC->walkingSpeed > 0 )
		{
			retVal = mNPC->walkingSpeed;
		}
	}

#if defined( UOX_DEBUG_MODE )
	retVal = (retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetWalkingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->walkingSpeed = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetRunningSpeed()
//|					CChar::SetRunningSpeed()
//| Date		-	Juni 9, 2007
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's running speed
//o------------------------------------------------------------------------------------------------o
R32 CChar::GetRunningSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCRunningSpeed();

	if( IsValidNPC() )
	{
		if( mNPC->runningSpeed > 0 )
		{
			retVal = mNPC->runningSpeed;
		}
	}

#if defined( UOX_DEBUG_MODE )
	retVal = (retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetRunningSpeed( R32 newValue )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->runningSpeed = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetFleeingSpeed()
//|					CChar::SetFleeingSpeed()
//| Date		-	Juni 9, 2007
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's fleeing speed
//o------------------------------------------------------------------------------------------------o
R32 CChar::GetFleeingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCFleeingSpeed();

	if( IsValidNPC() )
	{
		if( mNPC->fleeingSpeed > 0 )
		{
			retVal = mNPC->fleeingSpeed;
		}
	}

#if defined( UOX_DEBUG_MODE )
	retVal = (retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetFleeingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->fleeingSpeed = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetMountedWalkingSpeed()
//|					CChar::SetMountedWalkingSpeed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's mounted walking speed
//o------------------------------------------------------------------------------------------------o
R32 CChar::GetMountedWalkingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCMountedWalkingSpeed();

	if( IsValidNPC() )
	{
		if( mNPC->mountedWalkingSpeed > 0 )
		{
			retVal = mNPC->mountedWalkingSpeed;
		}
	}

#if defined( UOX_DEBUG_MODE )
	retVal = ( retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetMountedWalkingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->mountedWalkingSpeed = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetMountedRunningSpeed()
//|					CChar::SetMountedRunningSpeed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's mounted running speed
//o------------------------------------------------------------------------------------------------o
R32 CChar::GetMountedRunningSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCMountedRunningSpeed();

	if( IsValidNPC() )
	{
		if( mNPC->mountedRunningSpeed > 0 )
		{
			retVal = mNPC->mountedRunningSpeed;
		}
	}

#if defined( UOX_DEBUG_MODE )
	retVal = ( retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetMountedRunningSpeed( R32 newValue )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->mountedRunningSpeed = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetMountedFleeingSpeed()
//|					CChar::SetMountedFleeingSpeed()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the NPC's fleeing speed
//o------------------------------------------------------------------------------------------------o
R32 CChar::GetMountedFleeingSpeed( void ) const
{
	R32 retVal = cwmWorldState->ServerData()->NPCMountedFleeingSpeed();

	if( IsValidNPC() )
	{
		if( mNPC->mountedFleeingSpeed > 0 )
		{
			retVal = mNPC->mountedFleeingSpeed;
		}
	}

#if defined( UOX_DEBUG_MODE )
	retVal = ( retVal * DEBUGMOVEMULTIPLIER );
#endif
	return retVal;
}
void CChar::SetMountedFleeingSpeed( R32 newValue )
{
	if( !IsValidNPC() )
	{
		CreateNPC();
	}

	if( IsValidNPC() )
	{
		mNPC->mountedFleeingSpeed = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	DTEgreater()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Compare damage done for two damage entries
//o------------------------------------------------------------------------------------------------o
bool DTEgreater( DamageTrackEntry_st *elem1, DamageTrackEntry_st *elem2 )
{
	if( elem1 == nullptr )
		return false;

	if( elem2 == nullptr )
		return true;

	return elem1->damageDone > elem2->damageDone;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::Heal()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Heal character, and keep track of healer and amount healed
//o------------------------------------------------------------------------------------------------o
void CChar::Heal( SI16 healValue, CChar *healer )
{
	SetHP( hitpoints + healValue );
	if( healer != nullptr )
	{
		const SERIAL healerSerial	= healer->GetSerial();
		bool persFound				= false;
		for( DamageTrackEntry_st *i = damageHealed.First(); !damageHealed.Finished(); i = damageHealed.Next() )
		{
			if( i->damager == healerSerial )
			{
				i->damageDone		+= healValue;
				i->lastDamageType	= NONE;
				i->lastDamageDone	= cwmWorldState->GetUICurrentTime();
				persFound			= true;
				break;
			}
		}
		if( !persFound )
		{
			damageHealed.Add( new DamageTrackEntry_st( healerSerial, healValue, NONE, cwmWorldState->GetUICurrentTime() ));
		}
		damageHealed.Sort( DTEgreater );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ReactOnDamage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Lets character react to damage dealt to them
//o------------------------------------------------------------------------------------------------o
void CChar::ReactOnDamage( [[maybe_unused]] WeatherType damageType, CChar *attacker )
{
	CSocket *mSock = GetSocket();

	if( ValidateObject( attacker ))
	{
		// Let the target react upon damage
		// If attacker is attacking someone who is not already an aggressor to the attacker...
		auto cAttackOwner = attacker->GetOwnerObj();
		if( ValidateObject( cAttackOwner ) )
		{
			if( !CheckAggressorFlag( cAttackOwner->GetSerial() ) )
			{
				// Add both owner and pet/hireling/summon as aggressor of target!
				cAttackOwner->AddAggressorFlag( this->GetSerial() );
				attacker->AddAggressorFlag( this->GetSerial() );
			}
		}
		else
		{
			if( !CheckAggressorFlag( attacker->GetSerial() ))
			{
				// ...add cAttack as aggressor of target!
				attacker->AddAggressorFlag( this->GetSerial() );
			}
		}

		if( !attacker->IsInvulnerable() && ( !ValidateObject( GetTarg() ) || !ObjInRange( this, GetTarg(), DIST_INRANGE )))
		{
			SetAttacker( attacker );
			if( IsNpc() )
			{
				if( GetVisible() == VT_TEMPHIDDEN || attacker->GetVisible() == VT_INVISIBLE )
				{
					ExposeToView();
				}

				if( !IsAtWar() && !attacker->IsInvulnerable() && GetNpcAiType() != AI_DUMMY )
				{
					SetTarg( attacker );
					ToggleCombat();
					if( GetMounted() )
					{
						SetTimer( tNPC_MOVETIME, BuildTimeValue( GetMountedWalkingSpeed() ));
					}
					else
					{
						SetTimer( tNPC_MOVETIME, BuildTimeValue( GetWalkingSpeed() ));
					}
				}
			}
			else if( mSock != nullptr )
			{
				BreakConcentration( mSock );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::Damage()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Damage character, and keep track of attacker and damage amount
//o------------------------------------------------------------------------------------------------o
bool CChar::Damage( SI16 damageValue, WeatherType damageType, CChar *attacker, bool doRepsys )
{
	CSocket *mSock = GetSocket(), *attSock = nullptr, *attOwnerSock = nullptr;

	if( ValidateObject( attacker ))
	{
		std::vector<UI16> attScriptTriggers = attacker->GetScriptTriggers();
		for( auto i : attScriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( i );
			if( toExecute != nullptr )
			{
				// If event returns false, prevent damage!
				auto retVal = toExecute->OnDamageDeal( attacker, this, damageValue, damageType );
				if( retVal == 0 )
					return false;
			}
		}
	}

	std::vector<UI16> scriptTriggers = GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( i );
		if( toExecute != nullptr )
		{
			// If event returns false, prevent damage!
			auto retVal = toExecute->OnDamage( this, attacker, damageValue, damageType );
			if( retVal == 0 )
				return false;
		}
	}

	if( ValidateObject( attacker ))
	{
		attSock = attacker->GetSocket();
		if( ValidateObject( attacker->GetOwnerObj() ))
		{
			attOwnerSock = attacker->GetOwnerObj()->GetSocket();
		}
	}

	// Display damage numbers
	if( cwmWorldState->ServerData()->CombatDisplayDamageNumbers() )
	{
		CPDisplayDamage toDisplay(( *this ), static_cast<UI16>( damageValue ));
		if( mSock != nullptr )
		{
			mSock->Send( &toDisplay );
		}
		if( attSock != nullptr && attSock != mSock )
		{
			attSock->Send( &toDisplay );
		}
		if( attOwnerSock != nullptr )
		{
			attOwnerSock->Send( &toDisplay );
		}
	}

	// Apply the damage
	SetHP( hitpoints - damageValue );

	// Spawn blood effects
	if( damageValue >= std::max( static_cast<SI16>( 1 ), static_cast<SI16>( floor( GetMaxHP() * 0.01 )))) // Only display blood effects if damage done is higher than 1, or 1% of max health - whichever is higher
	{
		UI08 bloodEffectChance = cwmWorldState->ServerData()->CombatBloodEffectChance();
		bool spawnBlood = ( bloodEffectChance >= static_cast<UI08>( RandomNum( 0, 99 )));
		if( spawnBlood )
		{
			BloodTypes bloodType = BLOOD_BLEED;
			if( damageValue >= static_cast<SI16>( floor( GetMaxHP() * 0.2 )))
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
			CItem *bloodEffect = Effects->SpawnBloodEffect( WorldNumber(), GetInstanceId(), bloodColour, bloodType );
			if( ValidateObject( bloodEffect ))
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
		if( static_cast<UI08>( RandomNum( 1, 100 )) <= currentBreakChance )
		{
			SetCanAttack( true );
			if( mSock != nullptr )
			{
				mSock->SysMessage( 1779 ); // You are no longer affected by peace!
			}
		}
		else
		{
			SetBrkPeaceChance( currentBreakChance + GetBrkPeaceChanceGain() );
		}
	}

	if( ValidateObject( attacker ))
	{
		// Reputation system
		if( doRepsys )
		{
			// If attacker is an aggressor to the character, and if this action will result in a criminal flag
			if( attacker->CheckAggressorFlag( this->GetSerial() ) && WillResultInCriminal( attacker, this )) //REPSYS
			{
				// Update aggressor flag timer
				attacker->UpdateAggressorFlagTimestamp( this->GetSerial() );

				// Flag attacker as criminal
				MakeCriminal( attacker );
				bool regionGuarded = ( GetRegion()->IsGuarded() );
				if( cwmWorldState->ServerData()->GuardsStatus() && regionGuarded && IsNpc() && GetNpcAiType() != AI_GUARD && cwmWorldState->creatures[this->GetId()].IsHuman() )
				{
					TextMessage( nullptr, 335, TALK, true ); // Help! Guards! I've been attacked!
					CallGuards( this, attacker );
				}
				if( ValidateObject( attacker->GetOwnerObj() ))
				{
					MakeCriminal( attacker->GetOwnerObj() );
				}
			}
		}

		// Update Damage tracking
		const SERIAL attackerSerial	= attacker->GetSerial();
		bool persFound				= false;
		for( DamageTrackEntry_st *i = damageDealt.First(); !damageDealt.Finished(); i = damageDealt.Next() )
		{
			if( i->damager == attackerSerial )
			{
				i->damageDone		+= damageValue;
				i->lastDamageType	= damageType;
				i->lastDamageDone	= cwmWorldState->GetUICurrentTime();
				persFound			= true;
				break;
			}
		}
		if( !persFound )
		{
			damageDealt.Add( new DamageTrackEntry_st( attackerSerial, damageValue, damageType, cwmWorldState->GetUICurrentTime() ));
		}
		damageDealt.Sort( DTEgreater );
	}

	if( GetHP() <= 0 )
	{
		Die( attacker, doRepsys );
	}

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::GetKarma()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the character's karma
//o------------------------------------------------------------------------------------------------o
SI16 CChar::GetKarma( void ) const
{
	if( GetOwnerObj() != nullptr && IsTamed() )
	{
		if( ValidateObject( GetOwnerObj() ) && GetOwnerObj() != this )
		{
			// Pets inherit the karma of their owner
			return GetOwnerObj()->GetKarma();
		}
	}
	return karma;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::Die()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Kills character and handles consequences for character death
//o------------------------------------------------------------------------------------------------o
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

	if( ValidateObject( attacker ))
	{
		if( this != attacker && doRepsys )	// can't gain fame and karma for suicide :>
		{
			CSocket *attSock = attacker->GetSocket();
			if( attacker->CheckAggressorFlag( this->GetSerial() ) && WillResultInCriminal( attacker, this ))
			{
				attacker->UpdateAggressorFlagTimestamp( this->GetSerial() );
				attacker->SetKills( attacker->GetKills() + 1 );
				attacker->SetTimer( tCHAR_MURDERRATE, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_MURDERDECAY ));
				if( !attacker->IsNpc() )
				{
					if( attSock != nullptr )
					{
						attSock->SysMessage( 314, attacker->GetKills() ); // You have killed %i innocent people.
						if( attacker->GetKills() == cwmWorldState->ServerData()->RepMaxKills() + 1 )
						{
							attSock->SysMessage( 315 ); // You are now a murderer!
						}
					}
				}
				UpdateFlag( attacker );
			}
			Karma( attacker, this, ( 0 - ( GetKarma() )));
			Fame( attacker, GetFame() );
		}
		if(( !attacker->IsNpc() ) && (!IsNpc() ))
		{
			Console.Log( oldstrutil::format( Dictionary->GetEntry( 1617 ), GetName().c_str(), attacker->GetName().c_str() ), "PvP.log" );
		}

		Combat->Kill( attacker, this );
	}
	else
	{
		HandleDeath( this, nullptr );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::CheckDamageTrack()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Check if char with specified serial dealt damage to character within last X secs
//o------------------------------------------------------------------------------------------------o
auto CChar::CheckDamageTrack( SERIAL serialToCheck, TIMERVAL lastXSeconds ) -> bool
{
	TIMERVAL currentTime	= cwmWorldState->GetUICurrentTime();
	for( DamageTrackEntry_st *i = damageDealt.First(); !damageDealt.Finished(); i = damageDealt.Next() )
	{
		if( i->damager == serialToCheck )
		{
			// Did the last damage dealt happen within the last X seconds?
			if( currentTime < ( i->lastDamageDone + ( lastXSeconds * 1000 )))
			{
				return true;
			}
		}
	}

	return false;
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::UpdateDamageTrack()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Keeps track of damage dealt and healing done to character over time
//o------------------------------------------------------------------------------------------------o
void CChar::UpdateDamageTrack( void )
{
	TIMERVAL currentTime	= cwmWorldState->GetUICurrentTime();
	DamageTrackEntry_st *i		= nullptr;
	// Update the damage stuff
	for( i = damageDealt.First(); !damageDealt.Finished(); i = damageDealt.Next() )
	{
		if( i == nullptr )
		{
			damageDealt.Remove( i );
			continue;
		}
		if(( i->lastDamageDone + 300000 ) < currentTime )	// if it's been 5 minutes since they did any damage
		{
			damageDealt.Remove( i, true );
		}
	}
	// Update the healing stuff
	for( i = damageHealed.First(); !damageHealed.Finished(); i = damageHealed.Next() )
	{
		if( i == nullptr )
		{
			damageHealed.Remove( i );
			continue;
		}
		if(( i->lastDamageDone + 300000 ) < currentTime )	// if it's been 5 minutes since they did any damage
		{
			damageHealed.Remove( i, true );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::SetWeight()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Sets weight of character
//o------------------------------------------------------------------------------------------------o
void CChar::SetWeight( SI32 newVal, [[maybe_unused]] bool doWeightUpdate )
{
	Dirty( UT_STATWINDOW );
	weight = newVal;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::Dirty()
//|	Date		-	25 July, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces the object onto the global refresh queue
//o------------------------------------------------------------------------------------------------o
void CChar::Dirty( UpdateTypes updateType )
{
	if( IsPostLoaded() )
	{
		updateTypes.set( updateType, true );
		CBaseObject::Dirty( updateType );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CChar::UpdateRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Marks region character exists in as updated since last save
//o------------------------------------------------------------------------------------------------o
void CChar::UpdateRegion( void )
{
	// Make sure to only mark region as changed if this is a character we're supposed to save!
	if( ShouldSave() )
	{
		CMapRegion *curCell = MapRegion->GetMapRegion( this );
		curCell->HasRegionChanged( true );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::GetUpdate()
//| Date		-	10/31/2003
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Returns true if we have set a specific UpdateType
//o------------------------------------------------------------------------------------------------o
bool CChar::GetUpdate( UpdateTypes updateType ) const
{
	return updateTypes.test( updateType );
}
//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::ClearUpdate()
//| Date		-	3/20/2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Clears the UpdateType bitlist, used at the end of our refresh queue
//o------------------------------------------------------------------------------------------------o
void CChar::ClearUpdate( void )
{
	updateTypes.reset();
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CChar::InParty()
//| Date		-	21st September, 2006
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets/clears whether the character is in a party or not
//o------------------------------------------------------------------------------------------------o
void CChar::InParty( bool value )
{
	bools.set( BIT_INPARTY, value );
}
bool CChar::InParty( void ) const
{
	return bools.test( BIT_INPARTY );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CountHousesOwnedFunctor()
//|					CChar::CountHousesOwned() 
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to count the houses owned by a given character
//o------------------------------------------------------------------------------------------------o
bool CountHousesOwnedFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	UI32 *ourData		= (UI32 *)extraData;
	CChar *mChar		= CalcCharObjFromSer( ourData[0] );
	bool trackHousesPerAccount = ourData[1];
	bool countCoOwnedHouses = ourData[2];
	if( ValidateObject( mChar ) && ValidateObject( a ) && a->CanBeObjType( OT_MULTI ))
	{
		CMultiObj *i = static_cast<CMultiObj *>( a );
		if( i->GetObjType() == OT_BOAT )
			return false;

		if( !ValidateObject( i->GetOwnerObj() )) // return false if house has no owner
			return false;

		if( trackHousesPerAccount )
		{
			if( countCoOwnedHouses )
			{
				// Count house co-ownership per account
				if( i->GetOwnerObj()->GetAccountNum() != mChar->GetAccountNum() && i->IsOnOwnerList( mChar ))
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
auto CChar::CountHousesOwned( bool countCoOwnedHouses ) -> UI32
{
	auto b = UI32( 0 );
	if( cwmWorldState->ServerData()->TrackHousesPerAccount() || countCoOwnedHouses )
	{
		// Count all houses owned by characters on player's account by iterating over all multis on the server(!)
		UI32 toPass[3];
		toPass[0] = GetSerial();
		toPass[1] = cwmWorldState->ServerData()->TrackHousesPerAccount();
		toPass[2] = countCoOwnedHouses;
		ObjectFactory::GetSingleton().IterateOver( OT_MULTI, b, toPass, &CountHousesOwnedFunctor );
	}
	else
	{
		std::for_each( ownedItems.begin(), ownedItems.end(), [this, &b]( CItem *oItem )
		{
			if( ValidateObject( oItem ) && oItem->GetObjType() == OT_MULTI )
			{
				if( oItem->GetOwnerObj() == this )
				{
					b++;
				}
			}
		});
	}
	return b;
}
