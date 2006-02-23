#ifndef __CSCRIPT_H__
#define __CSCRIPT_H__


namespace UOX
{

class CPIGumpMenuSelect;
class CPIGumpInput;

enum ScriptEvent
{
	seOnCreate = 0,			//	*	Done for PCs on global script
	seOnCommand,
	seOnDelete,				//	**
	seOnSpeech,				//	*	Missing item response at the moment
	seInRange,				//	*	Missing character in range
	seOnCollide,			//	**	Items only
	seOnSteal,				//	**
	seOnDispel,				//	**
	seOnSkill,
	seOnStat,
	seOnAttack,
	seOnDefense,
	seOnSkillGain,			//	**
	seOnSkillLoss,			//	**
	seOnSkillChange,		//	**
	seOnStatGained,			//	**
	seOnStatGain,			//	**
	seOnStatLoss,			//	**
	seOnStatChange,			//	**
	seOnDrop,
	seOnPickup,
	seOnSwing,
	seOnDecay,
	seOnTransfer,
	seOnLeaving,			//	**
	seOnEntrance,			//	**
	seOnEquip,				//	**
	seOnUnequip,			//	**
	seOnUse,				//	**  the event that replaces hardcoded use-stuff
	seOutOfRange,			//	*	Missing character out of range
	seOnLogin,				//	**
	seOnLogout,
	seOnClick,
	seOnFall,
	seOnSell,
	seOnBuy,
	seOnAISliver,			//	**
	seOnSystemSlice,
	seOnUnknownTrigger,
	seOnLightChange,
	seOnWeatherChange,
	seOnTempChange,
	seOnXYZEvent,
	seOnPortal,
	seOnTimer,				//	**
	seOnDeath,				//	**
	seOnResurrect,			//	**
	seOnFlagChange,			//	**
	seOnHungerChange,		//	**
	seOnStolenFrom,			//	**
	seOnSnooped,			//	**
	seOnEnterRegion,		//  **
	seOnLeaveRegion,		//	**
	seOnSpellTarget,
	seOnSpellCast,
	seOnSpellSuccess,
	seOnTalk,
	seOnScrollCast,
	seOnSpeechInput,
	seOnSpellGain,
	seOnSpellLoss,
	seOnSkillCheck,
	seOnDropItemOnNpc,
	seOnStart,
	seOnStop,
	seOnIterate,
	seOnPacketReceive,
	seOnCharDoubleClick,	//	**  the event that replaces hardcoded character doubleclick-stuff
	seOnSkillGump,			//	**	allows overriding client's request to open default skill gump
	seOnCombatStart,		//	**	allows overriding what happens when combat is initiated
	seOnCombatEnd,			//	**	allows overriding what happens when combat ends
	seOnDeathBlow,
	seOnGumpPress,
	seOnGumpInput
};

struct SEGump
{
	STRINGLIST *one, *two;
	UI32 TextID;
};

//o--------------------------------------------------------------------------o
//|	Class/Struct	-	struct SEGumpData
//|	Date			-	1/21/2003 7:05:06 AM
//|	Developers		-	
//|	Organization	-	
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
struct SEGumpData
{
	STRINGLIST			sEdits;
	std::vector<int>	nButtons;
	std::vector<short>	nIDs;
};

class cScript
{
private:

	JSScript *	targScript;
	JSContext *	targContext;
	JSObject *	targObject;

	UI08		runTime;

	bool		isFiring;
	UI32		eventPresence[3];
	UI32		needsChecking[3];
	bool		EventExists( ScriptEvent eventNum ) const;
	bool		NeedsChecking( ScriptEvent eventNum ) const;
	void		SetEventExists( ScriptEvent eventNum, bool status );
	void		SetNeedsChecking( ScriptEvent eventNum, bool status );
	bool		ExistAndVerify( ScriptEvent eventNum, std::string functionName );

	std::vector< SEGump * >		gumpDisplays;

	void		Cleanup( void );

public:
	void		CollectGarbage( void );

	size_t		NewGumpList( void );
	SEGump *	GetGumpList( SI32 index );
	void		RemoveGumpList( SI32 index );
	void		SendGumpList( SI32 index, CSocket *toSendTo );

	void		HandleGumpPress( CPIGumpMenuSelect *packet );
	void		HandleGumpInput( CPIGumpInput *pressing );

				cScript( std::string targFile, UI08 runTime );
				~cScript();

	JSObject *	Object( void ) const;	// returns object pointer

	
	//|	Modification	-	08162003 - EviLDeD - Added these event to handle any script initialization and clean up as the server starts, and is shut down
	bool		OnStart( void );
	bool		OnStop( void );
	//
	bool		OnPacketReceive( CSocket *mSock, UI16 packetNum );
	bool		OnIterate( CBaseObject *a, UI32 &b );
	bool		OnCreate( CBaseObject *thingCreated, bool dfnCreated );
	bool		OnCommand( CSocket *mSock ); 
	bool		OnDelete( CBaseObject *thingDestroyed );
	SI08		OnSpeech( const char *speech, CChar *personTalking, CChar *talkingTo );
	bool		InRange( CChar *person, CBaseObject *objInRange );
	bool		OnCollide( CSocket *targSock, CChar *objColliding, CBaseObject *objCollideWith );
	bool		OnSteal( CChar *thief, CItem *theft );
	bool		OnDispel( CBaseObject *dispelled );
	bool		OnSkill( CBaseObject *skillUse, SI08 skillUsed );
	bool		OnStat( void );
	bool		OnAttack( CChar *attacker, CChar *defender );
	bool		OnDefense( CChar *attacker, CChar *defender );
	bool		OnSkillGain( CChar *player, SI08 skill );
	bool		OnSkillLoss( CChar *player, SI08 skill );
	bool		OnSkillChange( CChar *player, SI08 skill );
	bool		OnStatGained( CChar *player, UI32 stat, SI08 skill );
	bool		OnStatGain( CChar *player, UI32 stat, SI08 skill );
	bool		OnStatLoss( CChar *player, UI32 stat );
	bool		OnStatChange( CChar *player, UI32 stat );
	bool		OnDrop( CItem *item, CChar *dropper );
	bool		OnPickup( CItem *item, CChar *pickerUpper );
	bool		OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg );
	bool		OnDecay( CItem *decaying );
	bool		OnLeaving( CMultiObj *left, CBaseObject *leaving );
	bool		OnEntrance( CMultiObj *left, CBaseObject *leaving );
	bool		OnEquip( CChar *equipper, CItem *equipping );
	bool		OnUnequip( CChar *equipper, CItem *equipping );
	SI08		OnUse( CChar *user, CItem *iUsing );
	bool		OutOfRange( CChar *person, CBaseObject *objVanish );
	bool		OnLogin( CSocket *sockPlayer, CChar *pPlayer );
	bool		OnLogout( CSocket *sockPlayer, CChar *pPlayer );
	bool		OnClick( CSocket *sockPlayer, CItem *iClicked );
	bool		OnFall( CChar *pFall, SI08 fallDistance );
	bool		OnAISliver( CChar *pSliver );
	bool		OnSystemSlice( void );
	bool		OnLightChange( CBaseObject *tObject, UI08 lightLevel );
	bool		OnWeatherChange( CBaseObject *tObject, WeatherType element );
	bool		OnTempChange( CBaseObject *tObject, SI08 temp );
	bool		OnTimer( CBaseObject *tObject, UI08 timerID );
	bool		OnDeath( CChar *pDead );
	bool		OnResurrect( CChar *pAlive );
	bool		OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus );
	bool		OnHungerChange( CChar *pChanging, SI08 newStatus );
	bool		OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen );
	bool		OnSnooped( CChar *snooped, CChar *snooper, bool success );
	bool		OnEnterRegion( CChar *entering, SI16 region );
	bool		OnLeaveRegion( CChar *entering, SI16 region );
	bool		OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum );
	bool		DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum );
	SI16		OnSpellCast( CChar *tChar, UI08 SpellID );
	SI16		OnScrollCast( CChar *tChar, UI08 SpellID );
	bool		OnSpellSuccess( CChar *tChar, UI08 SpellID );
	bool		OnTalk( CChar *myChar, const char *mySpeech );
	bool		OnSpeechInput( CChar *myChar, CItem *myItem, const char *mySpeech );
	bool		OnSpellGain( CItem *book, const UI08 spellNum );
	bool		OnSpellLoss( CItem *book, const UI08 spellNum );
	bool		OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill );
	// MACTEP: 041802: Support for new event
	UI08		OnDropItemOnNpc( CChar *srcChar, CChar *targChar, CItem *i);
	bool		AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s );
	bool		CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams );

	bool		ScriptRegistration( std::string scriptType );

	bool		executeCommand( CSocket *s, std::string funcName, std::string executedString );
	
	bool		MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, int spellNum );
	SI08		OnCharDoubleClick( CChar *currChar, CChar *targChar );
	SI08		OnSkillGump( CChar *mChar );
	SI08		OnCombatStart( CChar *attacker, CChar *defender );
	SI08		OnCombatEnd( CChar *attacker, CChar *defender );

	bool		OnDeathBlow( CChar *mKilled, CChar *mKiller );

	//	Critical handler type stuff
	bool		IsFiring( void );
	void		Firing( void );
	void		Stop( void );
};

}

#endif

