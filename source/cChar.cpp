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
//|						Thoughts about cBaseObject and prelim plans made
//|
//| 					1.3		Abaddon		28 July, 2000
//|						Initial cBaseObject implementation put in.  CChar reworked to deal with only things it has to
//|						Proper constructor written
//|						Plans for CChar derived objects thought upon (notably CPC and CNPC)
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "power.h"
#include "weight.h"
#include "cGuild.h"
#include "msgboard.h"
#include "townregion.h"
#include "cRaces.h"
#include "skills.h"
#include "trigger.h"
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

namespace UOX
{

const SERIAL		DEFCHAR_ROBE 				= INVALIDSERIAL;
const SERIAL		DEFCHAR_TOWNVOTE 			= INVALIDSERIAL;
const UI32			DEFCHAR_BOOLS 				= 0;
const SI08			DEFCHAR_DISPZ 				= 0;
const SI08			DEFCHAR_FONTTYPE 			= 3;
const UI16			DEFCHAR_MAXHP 				= 0;
const UI16			DEFCHAR_MAXHP_OLDSTR 		= 0;
const RACEID		DEFCHAR_MAXHP_OLDRACE 		= 0;
const UI16			DEFCHAR_MAXMANA				= 0;
const UI16			DEFCHAR_MAXMANA_OLDINT 		= 0;
const RACEID		DEFCHAR_MAXMANA_OLDRACE		= 0;
const UI16			DEFCHAR_MAXSTAM				= 0;
const UI16			DEFCHAR_MAXSTAM_OLDDEX 		= 0;
const RACEID		DEFCHAR_MAXSTAM_OLDRACE		= 0;
const COLOUR		DEFCHAR_SAYCOLOUR 			= 0x0058;
const COLOUR		DEFCHAR_EMOTECOLOUR			= 0x0023;
const SI08			DEFCHAR_CELL 				= -1;
const UI16			DEFCHAR_DEATHS 				= 0;
CItem *				DEFCHAR_PACKITEM 			= NULL;
const UI08			DEFCHAR_FIXEDLIGHT 			= 255;
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
const UI08			DEFCHAR_STEP				= 0;
const UI16			DEFCHAR_PRIV				= 0;
const long			DEFCHAR_SAVEDAT				= 0;
const UI16			DEFCHAR_NOMOVE 				= 0;
const UI16			DEFCHAR_POISONCHANCE 		= 0;
const UI08			DEFCHAR_POISONSTRENGTH 		= 0;
const UI08			DEFCHAR_LAYERCTR			= 0;

const SI16			DEFPLAYER_CALLNUM 			= -1;
const SI16			DEFPLAYER_PLAYERCALLNUM		= -1;
const SERIAL		DEFPLAYER_TRACKINGTARGET 	= INVALIDSERIAL;
const UI08			DEFPLAYER_SQUELCHED			= 0;
const UI08			DEFPLAYER_COMMANDLEVEL 		= PLAYER_CMDLEVEL;
const UI08			DEFPLAYER_POSTTYPE 			= LOCALPOST;
const UI16			DEFPLAYER_HAIRSTYLE			= INVALIDID;
const UI16			DEFPLAYER_BEARDSTYLE 		= INVALIDID;
const COLOUR		DEFPLAYER_HAIRCOLOUR 		= INVALIDCOLOUR;
const COLOUR		DEFPLAYER_BEARDCOLOUR 		= INVALIDCOLOUR;
CItem *				DEFPLAYER_SPEECHITEM		= NULL;
const UI08			DEFPLAYER_SPEECHMODE 		= 0;
const UI08			DEFPLAYER_SPEECHID 			= 0;
const cScript *		DEFPLAYER_SPEECHCALLBACK 	= NULL;

const SI08			DEFNPC_WANDER 				= 0;
const SI08			DEFNPC_OLDWANDER 			= 0;
const SERIAL		DEFNPC_FTARG				= INVALIDSERIAL;
const SI08			DEFNPC_FZ1 					= -1;
const SI16			DEFNPC_AITYPE 				= 0;
const SI16			DEFNPC_SPATTACK				= 0;
const SI08			DEFNPC_SPADELAY				= 0;
const SI16			DEFNPC_TAMING 				= 0;
const SI16			DEFNPC_FLEEAT 				= 0;
const SI16			DEFNPC_REATTACKAT 			= 0;
const UI08			DEFNPC_SPLIT 				= 0;
const UI08			DEFNPC_SPLITCHANCE 			= 0;
const UI08			DEFNPC_TRAININGPLAYERIN		= 0;
const UI32			DEFNPC_HOLDG 				= 0;
const UI08			DEFNPC_QUESTTYPE			= 0;
const UI08			DEFNPC_QUESTDESTREGION 		= 0;
const UI08			DEFNPC_QUESTORIGREGION		= 0;
cBaseObject *		DEFNPC_PETGUARDING 			= NULL;

CChar::CChar() : cBaseObject(),
robe( DEFCHAR_ROBE ), townvote( DEFCHAR_TOWNVOTE ), bools( DEFCHAR_BOOLS ), 
dispz( DEFCHAR_DISPZ ), fonttype( DEFCHAR_FONTTYPE ), maxHP( DEFCHAR_MAXHP ), maxHP_oldstr( DEFCHAR_MAXHP_OLDSTR ), 
maxHP_oldrace( DEFCHAR_MAXHP_OLDRACE ), maxMana( DEFCHAR_MAXMANA ), maxMana_oldint( DEFCHAR_MAXMANA_OLDINT ), maxMana_oldrace( DEFCHAR_MAXMANA_OLDRACE ),
maxStam( DEFCHAR_MAXSTAM ), maxStam_olddex( DEFCHAR_MAXSTAM_OLDDEX ), maxStam_oldrace( DEFCHAR_MAXSTAM_OLDRACE ), saycolor( DEFCHAR_SAYCOLOUR ), 
emotecolor( DEFCHAR_EMOTECOLOUR ), cell( DEFCHAR_CELL ), deaths( DEFCHAR_DEATHS ), packitem( DEFCHAR_PACKITEM ), fixedlight( DEFCHAR_FIXEDLIGHT ), 
targ( DEFCHAR_TARG ), attacker( DEFCHAR_ATTACKER ), npcWander( DEFNPC_WANDER ), oldnpcWander( DEFNPC_OLDWANDER ), ftarg( DEFNPC_FTARG ), fz( DEFNPC_FZ1 ), 
hunger( DEFCHAR_HUNGER ), npcaitype( DEFNPC_AITYPE ), callnum( DEFPLAYER_CALLNUM ), playercallnum( DEFPLAYER_PLAYERCALLNUM ), regionNum( DEFCHAR_REGIONNUM ), 
spattack( DEFNPC_SPATTACK ), spadelay( DEFNPC_SPADELAY ), taming( DEFNPC_TAMING ), trackingtarget( DEFPLAYER_TRACKINGTARGET ), town( DEFCHAR_TOWN ), 
townpriv( DEFCHAR_TOWNPRIV ), advobj( DEFCHAR_ADVOBJ ), fleeat( DEFNPC_FLEEAT ), reattackat( DEFNPC_REATTACKAT ), split( DEFNPC_SPLIT ), splitchance( DEFNPC_SPLITCHANCE ), 
trainingplayerin( DEFNPC_TRAININGPLAYERIN ), guildfealty( DEFCHAR_GUILDFEALTY ), guildnumber( DEFCHAR_GUILDNUMBER ), flag( DEFCHAR_FLAG ), 
spellCast( DEFCHAR_SPELLCAST ), nextact( DEFCHAR_NEXTACTION ), squelched( DEFPLAYER_SQUELCHED ), layerCtr( DEFCHAR_LAYERCTR ),
stealth( DEFCHAR_STEALTH ), running( DEFCHAR_RUNNING ), holdg( DEFNPC_HOLDG ), raceGate( DEFCHAR_RACEGATE ), 
commandLevel( DEFPLAYER_COMMANDLEVEL ), postType( DEFPLAYER_POSTTYPE ), questType( DEFNPC_QUESTTYPE ), 
questDestRegion( DEFNPC_QUESTDESTREGION ), questOrigRegion( DEFNPC_QUESTORIGREGION ), ourAccount(), step( DEFCHAR_STEP ), hairstyle( DEFPLAYER_HAIRSTYLE ), 
beardstyle( DEFPLAYER_BEARDSTYLE ), haircolor( DEFPLAYER_HAIRCOLOUR ), beardcolour( DEFPLAYER_BEARDCOLOUR ), origSkin( colour ), priv( DEFCHAR_PRIV ),
petguarding( DEFNPC_PETGUARDING ), speechItem( DEFPLAYER_SPEECHITEM ), speechMode( DEFPLAYER_SPEECHMODE ),
speechID( DEFPLAYER_SPEECHID ), speechCallback( (cScript *)DEFPLAYER_SPEECHCALLBACK ), PoisonStrength( DEFCHAR_POISONSTRENGTH ),
oldLocX( 0 ), oldLocY( 0 ), oldLocZ( 0 )
{
	memset( charTimers, 0, sizeof( charTimers[0] ) * tCHAR_COUNT );
	id		= 0x0190;
	origID	= id;
	objType = OT_CHAR;
	memset( itemLayers, 0, sizeof( itemLayers[0] ) * MAXLAYERS );
	petFriends.resize( 0 );
	name		= "Mr. noname";
	for( UI08 k = 0; k <= ALLSKILLS; ++k )
		baseskill[k] = 10;
	memset( skill, 0, sizeof( skill[0] ) * (ALLSKILLS+1) );
	memset( lockState, 0, sizeof( lockState[0] ) * (ALLSKILLS+1) );
	for( UI08 j = 0; j <= ALLSKILLS; ++j )
		atrophy[j] = j;
	if( cwmWorldState != NULL )
		trackingtargets.resize( cwmWorldState->ServerData()->TrackingMaxTargets() );
	SetCanTrain( true );
	SetHungerStatus( true );

	memset( weathDamage, 0, sizeof( weathDamage[0] ) * WEATHNUM );
	skillUsed[0] = skillUsed[1] = 0;
	memset( regen, 0, sizeof( UI32 ) * 3 );
	ourAccount.wAccountIndex = AB_INVALID_ID;
	fx[0] = fx[1] = fy[0] = fy[1] = -1;

	strength = dexterity = intelligence = 1;
}

CChar::~CChar()	// Destructor to clean things up when deleted
{
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
	return npcaitype;
}
void CChar::SetNPCAiType( SI16 newValue )
{
	npcaitype = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL Guarding()
//|   Date        -  Unknown
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  SERIAL of the object the character is guarding
//o---------------------------------------------------------------------------o
cBaseObject *CChar::GetGuarding( void ) const
{
	return petguarding;
}
void CChar::SetGuarding( cBaseObject *newValue )
{
	petguarding = newValue;
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
	return taming;
}
void CChar::SetTaming( SI16 newValue )
{
	taming = newValue;
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
	return fixedlight;
}
void CChar::SetFixedLight( UI08 newVal )
{
	fixedlight = newVal;
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
//|   Function    -  UI08 TrainingPlayerIn()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Skill the player is being trained in. 255 is no training
//o---------------------------------------------------------------------------o
UI08 CChar::GetTrainingPlayerIn( void ) const
{
	return trainingplayerin;
}
void CChar::SetTrainingPlayerIn( UI08 newValue )
{
	trainingplayerin = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI32 HoldG( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Amount of gold being held by a Player Vendor
//o---------------------------------------------------------------------------o
UI32 CChar::GetHoldG( void ) const
{
	return holdg;
}
void CChar::SetHoldG( UI32 newValue )
{
	holdg = newValue;
}


//o--------------------------------------------------------------------------o
//|	Function		-	void CChar::SetAccount( UI16 newVal )
//|	Date			-	1/14/2003 6:17:45 AM
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Set the account object for this character to the specified
//|							account block. This functuo uses the accounts ID to link
//|							the character to the account.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void CChar::SetAccount( ACCOUNTSBLOCK& actbAccount )
{
	if( actbAccount.wAccountIndex == AB_INVALID_ID )
		ourAccount.wAccountIndex = AB_INVALID_ID;
	else
	{
		ourAccount = actbAccount;
		Accounts->ModAccount( ourAccount.wAccountIndex, AB_ALL, actbAccount );
	}
}
//

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 Split()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Split level of the character
//o---------------------------------------------------------------------------o
UI08 CChar::GetSplit( void ) const
{
	return split;
}
void CChar::SetSplit( UI08 newValue )
{
	split = newValue;
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
	return splitchance;
}
void CChar::SetSplitChance( UI08 newValue )
{
	splitchance = newValue;
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
	return ( (bools&0x02) == 0x02 );
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
	return ( (bools&0x04) == 0x04 );
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
	return ( (bools&0x08) == 0x08 );
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
	return ( (bools&0x10) == 0x10 );
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
	return ( (bools&0x20) == 0x20 );
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
	return ( (bools&0x40) == 0x40 );
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
	return ( (bools&0x80) == 0x80 );
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
	return ( (bools&0x100) == 0x100 );
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
	return ( (bools&0x200) == 0x200 );
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
	return ( (bools&0x400) == 0x400 );
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
	return ( (bools&0x800) == 0x800 );
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
	return ( (bools&0x1000) == 0x1000 );
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
	return ( (bools&0x2000) == 0x2000 );
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
	return ( (bools&0x4000) == 0x4000 );
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
	return ( (bools&0x8000) == 0x8000 );
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
	return ( (bools&0x10000) == 0x10000 );
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
	return ( (bools&0x20000) == 0x20000 );
}

bool CChar::MayLevitate( void ) const
{
	return ( (bools&0x40000) == 0x40000 );
}

bool CChar::WillHunger( void ) const
{
	return ( (bools&0x80000) == 0x80000 );
}

bool CChar::IsMeditating( void ) const
{
	return ( (bools&0x100000) == 0x100000 );
}

bool CChar::IsCasting( void ) const
{
	return ( (bools&0x200000) == 0x200000 );
}

bool CChar::IsJSCasting( void ) const
{
	return ( (bools&0x400000) == 0x400000 );
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
	if( newVal )
		bools |= 0x0002;
	else
		bools &= ~0x0002;
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
	if( newVal )
		bools |= 0x0004;
	else
		bools &= ~0x0004;
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
	if( newVal )
		bools |= 0x0008;
	else
		bools &= ~0x0008;
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
	if( newValue )
		bools |= 0x0010;
	else
		bools &= ~0x0010;
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
	if( newValue )
		bools |= 0x0020;
	else
		bools &= ~0x0020;
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
	if( newValue )
		bools |= 0x0040;
	else
		bools &= ~0x0040;
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
	if( newValue )
		bools |= 0x0080;
	else
		bools &= ~0x0080;
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
	if( newValue )
		bools |= 0x0100;
	else
		bools &= ~0x0100;
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
	if( newValue )
		bools |= 0x0200;
	else
		bools &= ~0x0200;
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
	if( newValue )
		bools |= 0x0400;
	else
		bools &= ~0x0400;
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
	if( newValue )
		bools |= 0x0800;
	else
		bools &= ~0x0800;
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
	if( newValue )
		bools |= 0x1000;
	else
		bools &= ~0x1000;
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
	if( newValue )
		bools |= 0x2000;
	else
		bools &= ~0x2000;
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
	if( newValue )
		bools |= 0x4000;
	else
		bools &= ~0x4000;
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
	if( newValue )
		bools |= 0x8000;
	else
		bools &= ~0x8000;
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
	if( newValue )
		bools |= 0x10000;
	else
		bools &= ~0x10000;
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
	if( newVal )
		bools |= 0x20000;
	else
		bools &= ~0x20000;
}

void CChar::SetLevitate( bool newValue )
{
	if( newValue )
		bools |= 0x40000;
	else
		bools &= ~0x40000;
}

void CChar::SetHungerStatus( bool newValue )
{
	if( newValue )
		bools |= 0x80000;
	else
		bools &= ~0x80000;
}

void CChar::SetMeditating( bool newValue )
{
	if( newValue )
		bools |= 0x100000;
	else
		bools &= ~0x100000;
}

void CChar::SetCasting( bool newValue )
{
	if( newValue )
		bools |= 0x200000;
	else
		bools &= ~0x200000;
}

// This is only a temporary measure until ALL code is switched over to JS code
// As it stands, it'll try and auto-direct cast if you set casting and spell timeouts
void CChar::SetJSCasting( bool newValue )
{
	if( newValue )
		bools |= 0x400000;
	else
		bools &= ~0x400000;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL Robe() 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Characters death robe
//o---------------------------------------------------------------------------o
SERIAL CChar::GetRobe( void ) const
{
	return robe;
}
void CChar::SetRobe( SERIAL newValue )
{
	robe = newValue;
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
		SetTamed( false );
	else
	{
		newOwner->GetPetList()->Add( this );
		SetTamed( true );
	}
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

std::string CChar::GetOrgName( void ) const
{
	return origName;
}
void CChar::SetOrgName( std::string newName )
{
	origName = newName;
}

std::string CChar::GetLastOn( void ) const
{
	return laston;
}
void CChar::SetLastOn( std::string newValue )
{
	laston = newValue;
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

UI16 CChar::GetOrgID( void ) const
{
	return origID;
}
void CChar::SetOrgID( UI16 value )
{
	origID = value;
}

UI16 CChar::GetHairStyle( void ) const
{
	return hairstyle;
}
void CChar::SetHairStyle( UI16 value )
{
	hairstyle = value;
}

UI16 CChar::GetBeardStyle( void ) const
{
	return beardstyle;
}
void CChar::SetBeardStyle( UI16 value )
{
	beardstyle = value;
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

COLOUR CChar::GetHairColour( void ) const
{
	return haircolor;
}
void CChar::SetHairColour( COLOUR value )
{
	haircolor = value;
}

COLOUR CChar::GetBeardColour( void ) const
{
	return beardcolour;
}
void CChar::SetBeardColour( COLOUR value )
{
	beardcolour = value;
}

UI16 CChar::GetSkin( void ) const
{
	return GetColour();
}
void CChar::SetSkin( UI16 value )
{
	SetColour( value );
}

UI16 CChar::GetOrgSkin( void ) const
{
	return origSkin;
}
void CChar::SetOrgSkin( UI16 value )
{
	origSkin = value;
}

SI16 CChar::GetFx( UI08 part ) const
{
	SI16 rvalue = 0;
	if( part < 2 )
		rvalue = fx[part];
	return rvalue;
}
void CChar::SetFx( SI16 newVal, UI08 part )
{
	if( part < 2 )
		fx[part] = newVal;
}

SI16 CChar::GetFy( UI08 part ) const
{
	SI16 rvalue = 0;
	if( part < 2 )
		rvalue = fy[part];
	return rvalue;
}
void CChar::SetFy( SI16 newVal, UI08 part )
{
	if( part < 2 )
		fy[part] = newVal;
}

SI08 CChar::GetFz( void ) const
{
	return fz;
}
void CChar::SetFz( SI08 newVal )
{
	fz = newVal;
}

SI08 CChar::GetDispZ( void ) const
{
	return dispz;
}
void CChar::SetDispZ( SI08 newZ )
{
	dispz = newZ;
	Dirty( UT_UPDATE );
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

SI08 CChar::GetNpcWander( void ) const
{
	return npcWander;
}
void CChar::SetNpcWander( SI08 newValue )
{
	npcWander = newValue;
}

SI08 CChar::GetOldNpcWander( void ) const
{
	return oldnpcWander;
}
void CChar::SetOldNpcWander( SI08 newValue )
{
	oldnpcWander = newValue;
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

cTownRegion *CChar::GetRegion( void ) const
{
	if( regionNum >= regions.size() )
		return regions.back();
	return regions[regionNum];
}
void CChar::SetRegion( UI08 newValue )
{
	regionNum = newValue;
}
UI08 CChar::GetRegionNum( void ) const
{
	return regionNum;
}

void CChar::SetLocation( const cBaseObject *toSet )
{
	if( toSet != NULL )
	{
		SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber() );
		if( toSet->GetObjType() == OT_CHAR )
			dispz = ( (CChar *)toSet )->GetDispZ();
	}
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world )
{
	Dirty( UT_LOCATION );
	MapRegion->RemoveChar( this );
	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
	x		= newX;
	y		= newY;
	z		= newZ;
	dispz	= newZ;
	WorldNumber( world );
	CMultiObj *newMulti = findMulti( newX, newY, newZ, world );
	SetMulti( newMulti );
	MapRegion->AddChar( this );
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
CChar *CChar::GetFTarg( void ) const
{
	return calcCharObjFromSer( ftarg );
}

UI16 CChar::GetAdvObj( void ) const
{
	return advobj;
}

CChar *CChar::GetTrackingTarget( void ) const
{
	return calcCharObjFromSer( trackingtarget );
}
CChar *CChar::GetTrackingTargets( UI08 targetNum ) const
{
	return trackingtargets[targetNum];
}
RACEID CChar::GetRaceGate( void ) const
{
	return raceGate;
}
void CChar::SetPackItem( CItem *newVal )
{
	packitem = newVal;
}

SERIAL calcSerFromObj( cBaseObject *mObj )
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
void CChar::SetFTarg( CChar *newTarg )
{
	ftarg = calcSerFromObj( newTarg );
}
void CChar::SetAdvObj( UI16 newValue )
{
	advobj = newValue;
}

void CChar::SetTrackingTarget( CChar *newValue )
{
	trackingtarget = calcSerFromObj( newValue );
}
void CChar::SetRaceGate( RACEID newValue )
{
	raceGate = newValue;
}
void CChar::SetTrackingTargets( CChar *newValue, UI08 targetNum )
{
	trackingtargets[targetNum] = newValue;
}

SI08 CChar::GetSpellCast( void ) const
{
	return spellCast;
}
SI16 CChar::GetSpAttack( void ) const
{
	return spattack;
}
SI08 CChar::GetSpDelay( void ) const
{
	return spadelay;
}

void CChar::SetSpellCast( SI08 newValue )
{
	spellCast = newValue;
}
void CChar::SetSpAttack( SI16 newValue )
{
	spattack = newValue;
}
void CChar::SetSpDelay( SI08 newValue )
{
	spadelay = newValue;
}

UI08 CChar::GetQuestType( void ) const
{
	return questType;
}
UI08 CChar::GetQuestOrigRegion( void ) const
{
	return questDestRegion;
}
UI08 CChar::GetQuestDestRegion( void ) const
{
	return questOrigRegion;
}

void CChar::SetQuestDestRegion( UI08 newValue )
{
	questDestRegion = newValue;
}
void CChar::SetQuestType( UI08 newValue )
{
	questType = newValue;
}
void CChar::SetQuestOrigRegion( UI08 newValue )
{
	questOrigRegion = newValue;
}

SI16 CChar::GetFleeAt( void ) const
{
	return fleeat;
}
SI16 CChar::GetReattackAt( void ) const
{
	return reattackat;
}

void CChar::SetFleeAt( SI16 newValue )
{
	fleeat = newValue;
}
void CChar::SetReattackAt( SI16 newValue )
{
	reattackat = newValue;
}

UI08 CChar::GetCommandLevel( void ) const
{
	return commandLevel;
}
UI08 CChar::GetPostType( void ) const
{
	return postType;
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
void CChar::SetPostType( UI08 newValue )
{
	postType = newValue;
}
void CChar::SetCommandLevel( UI08 newValue )
{
	commandLevel = newValue;
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

UI16 CChar::GetDeaths( void ) const
{
	return deaths;
}
SI16 CChar::GetGuildNumber( void ) const
{
	return guildnumber;
}
UI08 CChar::GetFlag( void ) const
{
	return flag;
}

void CChar::SetDeaths( UI16 newVal )
{
	deaths = newVal;
}
void CChar::SetFlag( UI08 newValue )
{
	flag = newValue;
}
void CChar::SetGuildNumber( SI16 newValue )
{
	guildnumber = newValue;
}

SI16 CChar::GetCallNum( void ) const
{
	return callnum;
}
SI16 CChar::GetPlayerCallNum( void ) const
{
	return playercallnum;
}

void CChar::SetPlayerCallNum( SI16 newValue )
{
	playercallnum = newValue;
}
void CChar::SetCallNum( SI16 newValue )
{
	callnum = newValue;
}
SI08 CChar::GetFontType( void ) const
{
	return fonttype;
}
UI08 CChar::GetSquelched( void ) const
{
	return squelched;
}

void CChar::SetFontType( SI08 newType )
{
	fonttype = newType;
}
void CChar::SetSquelched( UI08 newValue )
{
	squelched = newValue;
}

bool CChar::IsGM( void ) const
{
	return ( (priv&0x0001) == 0x0001 );
}
bool CChar::CanBroadcast( void ) const
{
	return ( (priv&0x0002) == 0x0002 );
}
bool CChar::IsInvulnerable( void ) const
{
	return ( (priv&0x0004) == 0x0004 );
}
bool CChar::GetSingClickSer( void ) const
{
	return ( (priv&0x0008) == 0x0008 );
}
bool CChar::NoSkillTitles( void ) const
{
	return ( (priv&0x0010) == 0x0010 );
}
bool CChar::IsGMPageable( void ) const
{
	return ( (priv&0x0020) == 0x0020 );
}
bool CChar::CanSnoop( void ) const
{
	return ( (priv&0x0040) == 0x0040 );
}
bool CChar::IsCounselor( void ) const
{
	return ( (priv&0x0080) == 0x0080 );
}

bool CChar::AllMove( void ) const
{
	return ( (priv&0x0100) == 0x0100 );
}
bool CChar::IsFrozen( void ) const
{
	return ( (priv&0x0200) == 0x0200 );
}
bool CChar::ViewHouseAsIcon( void ) const
{
	return ( (priv&0x0400) == 0x0400 );
}
// 0x0800 is free
bool CChar::NoNeedMana( void ) const
{
	return ( (priv&0x1000) == 0x1000 );
}
bool CChar::IsDispellable( void ) const
{
	return ( (priv&0x2000) == 0x2000 );
}
bool CChar::IsPermReflected( void ) const
{
	return ( (priv&0x4000) == 0x4000 );
}
bool CChar::NoNeedReags( void ) const
{
	return ( (priv&0x8000) == 0x8000 );
}

void CChar::SetGM( bool newValue )
{
	if( newValue )
		priv |= 0x0001;
	else
		priv &= ~0x0001;
}
void CChar::SetBroadcast( bool newValue )
{
	if( newValue )
		priv |= 0x0002;
	else
		priv &= ~0x0002;
}
void CChar::SetInvulnerable( bool newValue )
{
	if( newValue )
		priv |= 0x0004;
	else
		priv &= ~0x0004;
}
void CChar::SetSingClickSer( bool newValue )
{
	if( newValue )
		priv |= 0x0008;
	else
		priv &= ~0x0008;
}
void CChar::SetSkillTitles( bool newValue )
{
	if( newValue )
		priv |= 0x0010;
	else
		priv &= ~0x0010;
}
void CChar::SetGMPageable( bool newValue )
{
	if( newValue )
		priv |= 0x0020;
	else
		priv &= ~0x0020;
}
void CChar::SetSnoop( bool newValue )
{
	if( newValue )
		priv |= 0x0040;
	else
		priv &= ~0x0040;
}
void CChar::SetCounselor( bool newValue )
{
	if( newValue )
		priv |= 0x0080;
	else
		priv &= ~0x0080;
}

void CChar::SetAllMove( bool newValue )
{
	if( newValue )
		priv |= 0x0100;
	else
		priv &= ~0x0100;
}
void CChar::SetFrozen( bool newValue )
{
	if( newValue )
		priv |= 0x0200;
	else
		priv &= ~0x0200;
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	if( newValue )
		priv |= 0x0400;
	else
		priv &= ~0x0400;
}
// 0x0800 is free
void CChar::SetNoNeedMana( bool newValue )
{
	if( newValue )
		priv |= 0x1000;
	else
		priv &= ~0x1000;
}
void CChar::SetDispellable( bool newValue )
{
	if( newValue )
		priv |= 0x2000;
	else
		priv &= ~0x2000;
}
void CChar::SetPermReflected( bool newValue )
{
	if( newValue )
		priv |= 0x4000;
	else
		priv &= ~0x4000;
}
void CChar::SetNoNeedReags( bool newValue )
{
	if( newValue )
		priv |= 0x8000;
	else
		priv &= ~0x8000;
}

CChar *CChar::Dupe( void )
{
	CChar *target = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
	if( target == NULL )
		return NULL;

	cBaseObject::CopyData( target );
	CopyData( target );

	return target;
}

void CChar::CopyData( CChar *target )
{
		target->SetMulti( GetMultiObj() );
		target->SetOwner( GetOwnerObj() );
		target->SetRobe( robe );
		target->SetSpawn( GetSpawn() );
		target->SetTownVote( townvote );
		target->SetPriv( GetPriv() );
		target->SetName( name );
		target->SetTitle( title );
		target->SetOrgName( origName );
		target->SetAccount( GetAccount() );
		target->SetLocation( this );
		target->SetDispZ( dispz );
		target->SetDir( dir );
		target->SetID( id );
		target->SetOrgID( origID );
		target->SetSkin( GetColour() );
		target->SetFontType( fonttype );
		target->SetSayColour( saycolor );
		target->SetEmoteColour( emotecolor );
		target->SetStrength( strength );
		target->SetDexterity( dexterity );
		target->SetIntelligence( intelligence );
		target->SetHP(  hitpoints );
		target->SetStamina( stamina );
		target->SetMana( mana );

		target->SetStrength2( st2 );
		target->SetDexterity2( dx2 );
		target->SetIntelligence2( in2 );

		target->SetHiDamage( hidamage );
		target->SetLoDamage( lodamage );

		for( UI08 i = 0; i <= ALLSKILLS; ++i )
		{
			target->SetBaseSkill( baseskill[i], i );
			target->SetSkill( skill[i], i );
			target->SetAtrophy( atrophy[i], i );
			target->SetSkillLock( lockState[i], i );
		}

		target->SetCell( cell );
		target->SetKarma( karma );
		target->SetFame( fame );
		target->SetKills( kills );
		target->SetDeaths( deaths );
		target->SetPackItem( packitem );
		target->SetFixedLight( fixedlight );
		target->SetWeight( weight );
		target->SetDef( def );
		target->SetTarg( GetTarg() );
		target->SetRegen( regen[0], 0 );
		target->SetRegen( regen[1], 1 );
		target->SetRegen( regen[2], 2 );
		target->SetAttacker( GetAttacker() );
		target->SetNpcWander( npcWander );
		target->SetOldNpcWander( oldnpcWander );
		target->SetFTarg( GetFTarg() );
		target->SetFx( fx[0], 0 );
		target->SetFx( fx[1], 1 );
		target->SetFy( fy[0], 0 );
		target->SetFy( fy[1], 1 );
		target->SetFz( fz );
		target->SetVisible( GetVisible() );

		for( int mTID = (int)tCHAR_TIMEOUT; mTID < (int)tCHAR_COUNT; ++mTID )
			target->SetTimer( (cC_TID)mTID, GetTimer( (cC_TID)mTID ) );
		target->SetHunger( hunger );
		target->SetNPCAiType( npcaitype );
		target->SetGuarding( petguarding );
		target->SetCallNum( callnum );
		target->SetPlayerCallNum( playercallnum );
		target->SetRegion( regionNum );
		target->SetSpAttack( spattack );
		target->SetSpDelay( spadelay );
		target->SetTaming( taming );
		target->SetTrackingTarget( GetTrackingTarget() );
		for( UI08 counter = 0; counter < trackingtargets.size(); ++counter )
		{
			target->SetTrackingTargets( trackingtargets[counter], counter );
		}
		target->SetTown( town );
		target->SetTownpriv( townpriv );
		target->SetAdvObj( advobj );

		target->SetFleeAt( fleeat );
		target->SetReattackAt( reattackat );
		target->SetDisabled( isDisabled() );
		target->SetSplit( split );
		target->SetSplitChance( splitchance );
		target->SetTrainingPlayerIn( trainingplayerin );
		target->SetCanTrain( CanTrain() );
		target->SetLastOn( laston );
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
		target->SetHoldG( holdg );
		target->SetRace( GetRace() );
		target->SetRaceGate( raceGate );
		target->SetCarve( carve );
		target->SetCommandLevel( commandLevel );
		target->SetPostType( postType );
		target->SetQuestType( questType );
		target->SetQuestDestRegion( questDestRegion );
		target->SetQuestOrigRegion( questOrigRegion );
		for( UI08 counter2 = 0; counter2 < WEATHNUM; ++counter2 )
		{
			target->SetWeathDamage( weathDamage[counter2], counter2 );
		}
//		target->SetMaxHP( maxHP, maxHP_oldstr, maxHP_oldrace );
//		target->SetMaxMana( maxMana, maxMana_oldint, maxMana_oldrace );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::RemoveFromSight
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Loops through all online chars and removes the character
//|                  from their sight
//o---------------------------------------------------------------------------o
void CChar::RemoveFromSight( cSocket *mSock )
{
	CPRemoveItem toSend = (*this);
	if( mSock != NULL )
		mSock->Send( &toSend );
	else
	{
		SOCKLIST nearbyChars = FindPlayersInOldVisrange( this );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			(*cIter)->Send( &toSend );
		}
	}
}
//o---------------------------------------------------------------------------o
//|	Function	-	void SendToSocket( cSocket *s, bool sendDispZ )
//|	Date		-	April 7th, 2000
//|	Programmer	-	Abaddon
//|	Modified	-	Maarc (June 16, 2003)
//|						Got rid of array based packet sending, replaced with
//|						CPDrawObject, simplifying logic
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends the information about this person to socket S
//|                  IF in range.  Essentially a replacement for impowncreate
//o---------------------------------------------------------------------------o

void CChar::SendToSocket( cSocket *s )
{
	if( s != NULL )
	{
		CChar *mCharObj = s->CurrcharObj();
		if( mCharObj == this )
		{
			CPDrawGamePlayer gpToSend = (*this);
			s->Send( &gpToSend );
		}
		else if( GetVisible() == VT_PERMHIDDEN && GetCommandLevel() > mCharObj->GetCommandLevel() )
			return;

		UI08 cFlag = 0;
		CPDrawObject toSend = (*this);
		toSend.SetZ( GetDispZ() );
		if( ( !IsNpc() && !isOnline( this ) ) || ( GetVisible() != VT_VISIBLE )  || ( IsDead() && !IsAtWar() ) )
			cFlag |= 0x80;
		if( GetPoisoned() )
			cFlag |= 0x04;
		toSend.SetCharFlag( cFlag );
		GUILDRELATION guild = GuildSys->Compare( mCharObj, this );
		SI08 raceCmp		= Races->Compare( mCharObj, this );
		if( GetKills() > cwmWorldState->ServerData()->RepMaxKills() )
			toSend.SetRepFlag( 6 );
		else if( guild == GR_ALLY || guild == GR_SAME || raceCmp > 0 ) // Same guild (Green), racial ally, allied guild
			toSend.SetRepFlag( 2 );
		else if( guild == GR_WAR || raceCmp < 0 ) // Enemy guild.. set to orange
			toSend.SetRepFlag( 5 );
		else
		{
			if( IsMurderer() )		// Murderer
				toSend.SetRepFlag( 6 );
			else if( IsCriminal() )	// Criminal
				toSend.SetRepFlag( 3 );
			else					// Other
				toSend.SetRepFlag( 1 );
		}

		for( UI08 counter = 0; counter < MAXLAYERS; ++counter )
		{
			if( ValidateObject( itemLayers[counter] ) )
			{
				toSend.AddItem( itemLayers[counter] );
				CPQueryToolTip pSend( (*itemLayers[counter]) );
				s->Send( &pSend );
			}
		}

		toSend.Finalize();
		s->Send( &toSend );

		CPQueryToolTip pSend( (*this) );
		s->Send( &pSend );
	}
}

void checkRegion( cSocket *mSock, CChar *i );
void CChar::Teleport( void )
{
	cSocket *mSock = calcSocketObjFromChar( this );
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
			SubRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( ValidateObject( tempChar ) )
				{
					if( this != tempChar && objInRange( this, tempChar, visrange ) && ( isOnline( tempChar ) || tempChar->IsNpc() || IsGM() ) )
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

	checkRegion( mSock, this );
}

void CChar::ExposeToView( void )
{
	SetVisible( VT_VISIBLE );
	SetStealth( -1 );

	// hide it from ourselves, we want to show ourselves to everyone in range
	// now we tell everyone we exist
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechItem
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item related to the speech we're working on
//|                  IE the item for name deed if we're renaming ourselves
//o---------------------------------------------------------------------------o
CItem *CChar::GetSpeechItem( void ) const
{
	return speechItem;
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechMode
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns speech mode information, as to what mode of speech
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
	return speechMode;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void CChar::SetSpeechItem
//|	Date		-	April 8th, 2000
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose     -	Stores the the item that is  likely to be deleted on 
//|					switching out of our current mode
//o---------------------------------------------------------------------------o
void CChar::SetSpeechItem( CItem *newValue )
{
	speechItem = newValue;
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechMode
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the mode of speech that will be used next time
//o---------------------------------------------------------------------------o
void CChar::SetSpeechMode( UI08 newValue )
{
	speechMode = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Update( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends update to all those in range
//o---------------------------------------------------------------------------o
void CChar::Update( cSocket *mSock )
{
	if( mSock != NULL )
		SendToSocket( mSock );
	else
	{
		SOCKLIST nearbyChars = FindNearbyPlayers( this );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			SendToSocket( (*cIter) );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *GetItemAtLayer( UI08 Layer ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item at layer Layer on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::GetItemAtLayer( UI08 Layer ) const
{
	CItem *rvalue = NULL;
	if( Layer < MAXLAYERS )
		rvalue = itemLayers[Layer];
	return rvalue;
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
	UI08 tLayer = toWear->GetLayer();
	if( tLayer != 0 )	// Layer == 0 is a special case, for things like trade windows and such
	{
		if( tLayer < MAXLAYERS )
		{
			if( ValidateObject( itemLayers[tLayer] ) )
			{
#ifdef _DEBUG
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
				if( itemLayers[tLayer]->GetPoisoned() )
					SetPoisoned( GetPoisoned() + itemLayers[tLayer]->GetPoisoned() );	// should be +, not -

				UI16 scpNum			= toWear->GetScriptTrigger();
				cScript *tScript	= Trigger->GetScript( scpNum );
				if( tScript != NULL )
					tScript->OnEquip( this, toWear );
			}
		}
		else
			rvalue = false;
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool TakeOffItem( UI08 Layer )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Takes the item off the character's paperdoll,
//|					 ensuring that any stat adjustments are made
//|					 Returns true if successfully unequipped
//o---------------------------------------------------------------------------o
bool CChar::TakeOffItem( UI08 Layer )
{
	bool rvalue = false;
	if( Layer < MAXLAYERS && ValidateObject( itemLayers[Layer] ) )
	{
		if( Layer == 0x15 )	// It's our pack!
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
		tScript = Trigger->GetScript( scpNum );
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
	layerCtr = 0;
	return itemLayers[layerCtr];
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
	CItem *rvalue = NULL;
	++layerCtr;
	if( layerCtr < MAXLAYERS )
		rvalue = itemLayers[layerCtr];
	return rvalue;
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
	return ( layerCtr >= MAXLAYERS );
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

	cBaseObject::DumpBody( outStream );	// Make the default save of BaseObject members now
	dumping << "Account=" << GetConstAccount().wAccountIndex << std::endl;
	dumping << "LastOn=" << GetLastOn() << std::endl;
	dumping << "OrgName=" << GetOrgName() << std::endl;
	dumping << "GuildTitle=" << GetGuildTitle() << std::endl;  
	dumping << "NpcAIType=" << GetNPCAiType() << std::endl;
	dumping << "Taming=" << GetTaming() << std::endl;
	dumping << "Weight=" << GetWeight() << std::endl;
	dumping << "Hunger=" << (SI16)GetHunger() << std::endl;
	dumping << "FixedLight=" << (SI16)GetFixedLight() << std::endl;
	dumping << "Town=" << (SI16)GetTown() << std::endl;
	dumping << "HoldG=" << GetHoldG() << std::endl;
	dumping << "Split=" << (SI16)GetSplit() << std::endl;
	dumping << "SplitChance=" << (SI16)GetSplitChance() << std::endl;
	dumping << "RobeSerial=" << GetRobe() << std::endl;
	dumping << "TownVote=" << GetTownVote() << std::endl;
	dumping << "GuildFealty=" << GetGuildFealty() << std::endl;  
	dumping << "SummonTimer=" << GetTimer( tNPC_SUMMONTIME ) << std::endl;
	dumping << "OriginalBodyID=" << GetOrgID() << std::endl;
	dumping << "HairStyle=" << GetHairStyle() << std::endl;
	dumping << "BeardStyle=" << GetBeardStyle() << std::endl;
	dumping << "OriginalSkinID=" << GetOrgSkin() << std::endl;
	dumping << "HairColour=" << GetHairColour() << std::endl;
	dumping << "BeardColour=" << GetBeardColour() << std::endl;
	dumping << "Say=" << GetSayColour() << std::endl;
	dumping << "Emotion=" << GetEmoteColour() << std::endl;
	dumping << "MayLevitate=" << (MayLevitate()?1:0) << std::endl;
	dumping << "WanderArea=" << GetFx( 0 ) << "," << GetFy( 0 ) << "," << GetFx( 1 ) << "," << GetFy( 1 ) << "," << (SI16)GetFz() << std::endl;
	dumping << "DisplayZ=" << (SI16)GetDispZ() << std::endl;
	dumping << "Stealth=" << (SI16)GetStealth() << std::endl;
	dumping << "Reserved=" << (SI16)GetCell() << std::endl;
	dumping << "NpcWander=" << (SI16)GetNpcWander() << std::endl;
	dumping << "XNpcWander=" << (SI16)GetOldNpcWander() << std::endl;
	dumping << "Running=" << (SI16)GetRunning() << std::endl;
	dumping << "Step=" << (SI16)GetStep() << std::endl;
	dumping << "Region=" << (SI16)GetRegionNum() << std::endl;
	if( ValidateObject( packitem ) )
		dumping << "PackItem=" << packitem->GetSerial() << std::endl;	// store something meaningful
	else
		dumping << "PackItem=" << INVALIDSERIAL << std::endl;
	dumping << "AdvanceObject=" << GetAdvObj() << std::endl;
	dumping << "AdvRaceObject=" << GetRaceGate() << std::endl;
	dumping << "SPAttack=" << GetSpAttack() << std::endl;
	dumping << "SpecialAttackDelay=" << (SI16)GetSpDelay() << std::endl;
	dumping << "QuestType=" << (SI16)GetQuestType() << std::endl;
	dumping << "QuestDestinationRegion=" << (SI16)GetQuestDestRegion() << std::endl;
	dumping << "QuestOriginalRegion=" << (SI16)GetQuestOrigRegion() << std::endl;
	dumping << "FleeAt=" << GetFleeAt() << std::endl;
	dumping << "ReAttackAt=" << GetReattackAt() << std::endl;
	dumping << "Privileges=" << (SI16)GetPriv() << std::endl;
	dumping << "CommandLevel=" << (SI16)GetCommandLevel() << std::endl;	// command level
	dumping << "PostType=" << (SI16)GetPostType() << std::endl;
	dumping << "TownPrivileges=" << (SI16)GetTownPriv() << std::endl;

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
	dumping << "Deaths=" << GetDeaths() << std::endl;
	dumping << "FontType=" << (SI16)GetFontType() << std::endl;
	dumping << "Squelched=" << (SI16)GetSquelched() << std::endl;
	dumping << "TownTitle=" << (GetTownTitle()?1:0) << std::endl;
	//-------------------------------------------------------------------------------------------
	dumping << "CanRun=" << (SI16)((CanRun() && IsNpc())?1:0) << std::endl;
	dumping << "AllMove=" << (SI16)(AllMove()?1:0) << std::endl;
	dumping << "IsNpc=" << (SI16)(IsNpc()?1:0) << std::endl;
	dumping << "IsShop=" << (SI16)(IsShop()?1:0) << std::endl;
	dumping << "Dead=" << (SI16)(IsDead()?1:0) << std::endl;
	dumping << "CanTrain=" << (SI16)(CanTrain()?1:0) << std::endl;
	dumping << "IsWarring=" << (SI16)(IsAtWar()?1:0) << std::endl;
	dumping << "GuildToggle=" << (SI16)(GetGuildToggle()?1:0) << std::endl;
	dumping << "PoisonStrength=" << (UI16)(GetPoisonStrength()) << std::endl;
	dumping << "WillHunger=" << (SI16)(WillHunger()?1:0) << std::endl;

	outStream << dumping.str();
	return true;
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
		if( mX >= 0 && ( mX < 6144 || mX >= 7000 ) )
		{
			if( mY >= 0 && ( mY < 4096 || mY >= 7000 ) )
			{
				DumpHeader( outStream );
				DumpBody( outStream );
				DumpFooter( outStream );

				for( UI08 tempCounter = 0; tempCounter < MAXLAYERS; ++tempCounter )
				{
					if( ValidateObject( itemLayers[tempCounter] ) )
					{
						if( itemLayers[tempCounter]->ShouldSave() )
							itemLayers[tempCounter]->Save( outStream );
					}
				}
				rvalue = true;
			}
		}
	}
	return rvalue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	ACCOUNTSBLOCK &CChar::GetAccount( void )
//|	Date			-	3/13/2003
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the account, if any, associated with the character
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
ACCOUNTSBLOCK &CChar::GetAccount(void) 
{
	if( IsNpc() || ourAccount.wAccountIndex == AB_INVALID_ID )
	{
		// Set this just in case that this is an NPC.
		ourAccount.wAccountIndex=AB_INVALID_ID;
	}
	//
	return ourAccount;
}
//
const ACCOUNTSBLOCK &CChar::GetConstAccount( void ) const
{
	return ourAccount;
}

UI16 CChar::GetAccountNum( void ) const
{
	return ourAccount.wAccountIndex;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void BreakConcentration( cSocket *sock = NULL )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Breaks the concentration of the character
//|					 sending a message is a socket exists
//o---------------------------------------------------------------------------o
void CChar::BreakConcentration( cSocket *sock )
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
//|   Function    -  CHARLIST *GetFriendList( void )
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the characters list of friends
//o---------------------------------------------------------------------------o
CHARLIST *CChar::GetFriendList( void )
{
	return &petFriends;
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
//|   Function    -  void AddFriend( CChar *toAdd ) const
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds the friend toAdd to the player's friends list
//|					 ensuring it is already not on it
//o---------------------------------------------------------------------------o
void CChar::AddFriend( CChar *toAdd )
{
	CHARLIST_CITERATOR i;
	for( i = petFriends.begin(); i != petFriends.end(); ++i )
	{
		if( (*i) == toAdd )
			break;
	}
	if( i == petFriends.end() )
		petFriends.push_back( toAdd );
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
	for( CHARLIST_ITERATOR rIter = petFriends.begin(); rIter != petFriends.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			petFriends.erase( rIter );
			break;
		}
	}
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
	if( maxHP_oldstr != GetStrength() || maxHP_oldrace != GetRace() )
	//if str/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxHP = (UI16)(GetStrength() + (UI16)( ((float)GetStrength()) * ((float)(pRace->HPModifier())) / 100 ));
		// set max. hitpoints to strength + hpmodifier% of strength

		maxHP_oldstr = GetStrength();
		maxHP_oldrace = GetRace();

	}
	return maxHP;
}
void CChar::SetMaxHP( UI16 newmaxhp, SI16 newoldstr, RACEID newoldrace )
{
	maxHP			= newmaxhp;
	maxHP_oldstr	= newoldstr;
	maxHP_oldrace	= newoldrace;
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
	if( maxMana_oldint != GetIntelligence() || maxMana_oldrace != GetRace() )
	//if int/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxMana = (SI16)(GetIntelligence() + (SI16)( ((float)GetIntelligence()) * ((float)(pRace->ManaModifier())) / 100 ));
		// set max. mana to int + manamodifier% of int

		maxMana_oldint = GetIntelligence();
		maxMana_oldrace = GetRace();

	}
	return maxMana;
}
void CChar::SetMaxMana( SI16 newmaxmana, SI16 newoldint, RACEID newoldrace )
{
	maxMana			= newmaxmana;
	maxMana_oldint	= newoldint;
	maxMana_oldrace = newoldrace;
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
	if( maxStam_olddex != GetDexterity() || maxStam_oldrace != GetRace() )
	//if dex/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );

		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );

		maxStam = (SI16)(GetDexterity() + (SI16)( ((float)GetDexterity()) * ((float)(pRace->StamModifier())) / 100 ));
		// set max. stamina to dex + stammodifier% of dex

		maxStam_olddex	= GetDexterity();
		maxStam_oldrace	= GetRace();

	}
	return maxStam;
}
void CChar::SetMaxStam( SI16 newmaxstam, SI16 newolddex, RACEID newoldrace )
{
	maxStam			= newmaxstam;
	maxStam_olddex	= newolddex;
	maxStam_oldrace = newoldrace;
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
	return cBaseObject::GetStrength();
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
	return (SI16)(cBaseObject::GetStrength() + GetStrength2());
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
	return cBaseObject::GetIntelligence();
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
	return (SI16)(cBaseObject::GetIntelligence() + GetIntelligence2());
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
	return cBaseObject::GetDexterity();
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
	return (SI16)(cBaseObject::GetDexterity() + GetDexterity2());
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
	return ( (GetFlag()&0x01) == 0x01 );
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
	return ( (GetFlag()&0x02) == 0x02 );
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
	return ( (GetFlag()&0x04) == 0x04 );
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
	bool rvalue = cBaseObject::HandleLine( UTag, data );
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "ACCOUNT" )
				{
					//SetAccount( makeNum( data ) );
					ourAccount.wAccountIndex = data.toUShort();
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
						UString tempval		= tempdata.section( ",", 1, 1 ).substr( 0, tempdata.section( ",", 1, 1 ).size() - 1 );
						UString tempnum		= tempdata.section( ",", 0, 0 ).substr( 1 );
						SetBaseSkill( tempval.toUShort(), tempnum.toUByte() );
					}
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
				else if( UTag == "CANTRAIN" )
				{
					SetCanTrain( data.toUShort() == 1 );
					rvalue = true;
				}
				break;
			case 'D':
				if( UTag == "DISPLAYZ" )
				{
					SetDispZ( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "DEATHS" )
				{
					SetDeaths( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "DEAD" )
				{
					SetDead( data.toUByte() == 1 );
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
					SetHunger( data.toByte() );
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
				break;
			case 'M':
				if( UTag == "MAYLEVITATE" )
				{
					SetLevitate( (data.toShort() == 1) );
					rvalue = true;
				}
				else if( UTag == "MAKING" ) // Depreciated
					rvalue = true;
				break;
			case 'N':
				if( UTag == "NPCAITYPE" )
				{
					SetNPCAiType( data.toShort() );
					rvalue = true;
				}
				else if( UTag == "NPCWANDER" )
				{
					SetNpcWander( data.toByte() );
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
				break;
			case 'P':
				if( UTag == "PRIVILEGES" )
				{
					SetPriv( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "POSTTYPE" )
				{
					SetPostType( data.toUByte() );
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
				else if( UTag == "POISONSERIAL" ) // Depreciated
					rvalue = true;
				else if( UTag == "POISONSTRENGTH" )
				{
					SetPoisonStrength( data.toUByte() );
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
					SetRunning( data.toUByte() );
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
					SetStep( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "SPATTACK" )
				{
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
						UString tempval = tempdata.section( ",", 1, 1 ).substr( 0, tempdata.section( ",", 1, 1 ).size() - 1 );
						UString tempnum = tempdata.section( ",", 0, 0 ).substr( 1 );
						SetSkillLock( tempval.toUByte(), tempnum.toUByte() );
					}
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TAMING" )
				{
					SetTaming( data.toShort() );
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

	if( IsCounselor() && GetCommandLevel() < CNS_CMDLEVEL && !IsGM() ) // interim line to retain compatibility, MUST BE TAKEN out in the long term!
		SetCommandLevel( CNS_CMDLEVEL );
	if( IsGM() && GetCommandLevel() < GM_CMDLEVEL )	// interim line to retain compatibility, MUST BE TAKEN out in the long term!
		SetCommandLevel( GM_CMDLEVEL );
	//if( ourAccount->lpaarHolding->->bFlags|=0x8000 )
	//	SetCommandLevel( GM_CMDLEVEL );
	////////////////////////////////////////////////////////////////////

	cTownRegion *tRegion = calcRegionFromXY( GetX(), GetY(), worldNumber );
	SetRegion( (tRegion != NULL ? tRegion->GetRegionNum() : 0xFF) );
	SetTimer( tCHAR_ANTISPAM, 0 );
	if( GetID() != GetOrgID() )
		SetID( GetOrgID() );

	UI16 k = GetID();
	if( k > 0x3E1 )
	{
		if( GetAccount().wAccountIndex==AB_INVALID_ID )
		{
			Console << "npc: " << GetSerial() << "[" << GetName() << "] with bugged body value detected, deleted for stability reasons" << myendl;
			rvalue = false;
		}
		else
			SetID( 0x0190 );
	}
	if( rvalue )
	{
		MapRegion->AddChar( this );

		SI16 mx = GetX();
		SI16 my = GetY();
		UI16 acct = GetAccount().wAccountIndex;

		bool overRight = ( mx > MapWidths[worldNumber] );
		bool overBottom = ( my > MapHeights[worldNumber] );

		if( acct == AB_INVALID_ID && ( ( overRight && mx < 7000 ) || ( overBottom && my < 7000 ) || mx < 0 || my < 0 ) )
		{
			if( IsNpc() )
				rvalue = false;
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
		rvalue			= ( ( skillUsed[part] & compMask ) == compMask );
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
		if( value )
			skillUsed[part] |= compMask;
		else
		{
			UI32 flagMask = 0xFFFFFFFF;
			flagMask ^= compMask;
			skillUsed[part] &= flagMask;
		}
	}
}

UI08 CChar::PopDirection( void )
{
	UI08 rvalue = 0;
	if( !pathToFollow.empty() )
	{
		rvalue = pathToFollow.front();
		pathToFollow.pop();
	}
	return rvalue;
}
void CChar::PushDirection( UI08 newDir )
{
	pathToFollow.push( newDir );
}

bool CChar::StillGotDirs( void ) const
{
	return !pathToFollow.empty();
}

void CChar::FlushPath( void )
{
	while( StillGotDirs() )
		PopDirection();
}

void CChar::PostLoadProcessing( void )
{
	cBaseObject::PostLoadProcessing();
	SERIAL tempSerial = (SERIAL)packitem;		// we stored the serial in packitem
	if( tempSerial != INVALIDSERIAL )
		SetPackItem( calcItemObjFromSer( tempSerial ) );
	else
		SetPackItem( NULL );
	if( GetWeight() < 0 || GetWeight() > MAX_WEIGHT )
		SetWeight( Weight->calcCharWeight( this ) );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
		Skills->updateSkillLevel( this, i );
	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
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
bool CChar::IsMounted( void ) const
{
	return ( GetX() >= 7000 || GetY() >= 7000 );
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
	flag |= 0x01;
	flag &= 0x1B;
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
	flag |= 0x04;
	flag &= 0x1C;
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
	flag |= 0x02;
	flag &= 0x1B;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechID
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the ID for the JS Speech Input
//o---------------------------------------------------------------------------o
void CChar::SetSpeechID( UI08 newValue )
{
	speechID = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechCallback
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the Callback for the onSpeechInput function
//o---------------------------------------------------------------------------o
void CChar::SetSpeechCallback( cScript *newValue )
{
	speechCallback = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechID
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Gets the ID for the JS Speech Input
//o---------------------------------------------------------------------------o
UI08 CChar::GetSpeechID( void ) const
{
	return speechID;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechCallback
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Gets the Callback for the onSpeechInput function
//o---------------------------------------------------------------------------o
cScript *CChar::GetSpeechCallback( void ) const
{
	return speechCallback;
}


//o---------------------------------------------------------------------------o
//|	Function	-	bool CChar::isHuman( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if character is Human or NPC
//o---------------------------------------------------------------------------o
bool CChar::isHuman( void )
{
	bool rvalue = false;
	if( GetOrgID() == 0x0190 || GetOrgID() == 0x0191 )
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
//|		Function    :	void talk( cSocket *s, SI32 dictEntry, bool antispam, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Npc speech
//o---------------------------------------------------------------------------o
void CChar::talk( cSocket *s, SI32 dictEntry, bool antispam, ... )
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
//|		Function    :	void talk( cSocket *s, std::string txt, bool antispam )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Npc speech
//o---------------------------------------------------------------------------o
void CChar::talk( cSocket *s, std::string txt, bool antispam )
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
			CSpeechEntry *toAdd = SpeechSys->Add();

			toAdd->Font( (FontType)GetFontType() );
			toAdd->Speech( txt );
			toAdd->Speaker( GetSerial() );
			toAdd->SpokenTo( mChar->GetSerial() );
			toAdd->Type( TALK );
			toAdd->At( cwmWorldState->GetUICurrentTime() );
			toAdd->TargType( SPTRG_INDIVIDUAL );
			if( GetNPCAiType() == aiEVIL )
				toAdd->Colour( 0x0026 );
			else if( GetSayColour() == 0x1700 )
				toAdd->Colour( 0x5A );
			else
				toAdd->Colour( GetSayColour() );
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
			CSpeechEntry *toAdd = SpeechSys->Add();

			toAdd->Font( (FontType)GetFontType() );
			toAdd->Speech( txt );
			toAdd->Speaker( GetSerial() );
			toAdd->SpokenTo( INVALIDSERIAL );
			toAdd->Type( TALK );
			toAdd->At( cwmWorldState->GetUICurrentTime() );
			toAdd->TargType( SPTRG_PCNPC );
			if( GetNPCAiType() == aiEVIL )
				toAdd->Colour( 0x0026 );
			else if( GetSayColour() == 0x1700 )
				toAdd->Colour( 0x5A );
			else
				toAdd->Colour( GetSayColour() );
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
		cSocket *tSock = (*cIter);
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
//|		Function    :	void emote( cSocket *s, std::string txt, bool antispam )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC emotes
//o---------------------------------------------------------------------------o
void CChar::emote( cSocket *s, std::string txt, bool antispam )
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
			CSpeechEntry *toAdd = SpeechSys->Add();

			toAdd->Font( (FontType)GetFontType() );

			toAdd->Speech( txt );
			toAdd->Speaker( GetSerial() );
			toAdd->SpokenTo( mChar->GetSerial() );
			toAdd->Type( EMOTE );
			toAdd->At( cwmWorldState->GetUICurrentTime() );
			toAdd->TargType( SPTRG_INDIVIDUAL );
			toAdd->Colour( GetEmoteColour() );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void emote( cSocket *s, SI32 dictEntry, bool antispam, ... )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	NPC emotes
//o---------------------------------------------------------------------------o
void CChar::emote( cSocket *s, SI32 dictEntry, bool antispam, ... )
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
		cSocket *tSock = (*cIter);
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

void CChar::SendWornItems( cSocket *mSock )
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

void CChar::WalkXY( SI16 newX, SI16 newY )
{
	oldLocX = x;
	oldLocY = y;
	x = newX;
	y = newY;
}

void CChar::WalkZ( SI08 newZ )
{
	oldLocZ = z;
	z		= newZ;
	dispz	= newZ;
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
		RemoveFromSight();
		cBaseObject::Cleanup();

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
				tempChar->SetTarg( NULL );
			SetTarg( NULL );
		}
		tempChar = GetAttacker();
		if( ValidateObject( tempChar ) )
		{
			if( tempChar->GetAttacker() == this )
				tempChar->SetAttacker( NULL );
			SetAttacker( NULL );
		}
		MapRegion->RemoveChar( this );

		// If we delete a NPC we should delete his tempeffects as well
		for( CTEffect *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
		{
			if( Effect->Destination() == GetSerial() )
				Effect->Destination( INVALIDSERIAL );
			
			if( Effect->Source() == GetSerial() )
				Effect->Source( INVALIDSERIAL );
		}

		// if we delete a NPC we should delete him from spawnregions
		// this will fix several crashes
		if( IsNpc() && isSpawned() )
		{
			if( GetSpawn() < BASEITEMSERIAL )
			{
				UI16 spawnRegNum = (UI16)((GetSpawn()>>16) + (GetSpawn()>>8));
				if( spawnRegNum < spawnregions.size() )
				{
					cSpawnRegion *spawnReg = spawnregions[spawnRegNum];
					spawnReg->deleteSpawnedChar( this );
				}
			}
		}
		
		if( !IsNpc() )
		{
			ACCOUNTSBLOCK mAcct = GetAccount();
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
	cBaseObject::SetStrength( newValue );
	Dirty( UT_HITPOINTS );
}

void CChar::SetDexterity( SI16 newValue )
{
	cBaseObject::SetDexterity( newValue );
	Dirty( UT_STAMINA );
}

void CChar::SetIntelligence( SI16 newValue )
{
	cBaseObject::SetIntelligence( newValue );
	Dirty( UT_MANA );
}

void CChar::SetHP( SI16 newValue )
{
	cBaseObject::SetHP( UOX_MIN( UOX_MAX( static_cast<SI16>(0), newValue ), static_cast<SI16>(GetMaxHP()) ) );
	Dirty( UT_HITPOINTS );
}

void CChar::SetMana( SI16 newValue )
{
	cBaseObject::SetMana( UOX_MIN( UOX_MAX( static_cast<SI16>(0), newValue ), GetMaxMana() ) );
	Dirty( UT_MANA );
}

void CChar::SetStamina( SI16 newValue )
{
	cBaseObject::SetStamina( UOX_MIN( UOX_MAX( static_cast<SI16>(0), newValue ), GetMaxStam() ) );
	Dirty( UT_STAMINA );
}

void CChar::SetPoisoned( UI08 newValue )
{
	cBaseObject::SetPoisoned( newValue );
	Dirty( UT_UPDATE );
}

void CChar::SetStrength2( SI16 nVal )
{
	cBaseObject::SetStrength2( nVal );
	Dirty( UT_HITPOINTS );
}

void CChar::SetDexterity2( SI16 nVal )
{
	cBaseObject::SetDexterity2( nVal );
	Dirty( UT_STAMINA );
}

void CChar::SetIntelligence2( SI16 nVal )
{
	cBaseObject::SetIntelligence2( nVal );
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
	bool rvalue = cBaseObject::CanBeObjType( toCompare );
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
	++(deletionQueue[this]);
	Cleanup();
	SetDeleted( true );
	ShouldSave( false );
}

point3 CChar::GetOldLocation( void )
{
	return point3( oldLocX, oldLocY, oldLocZ );
}

}
