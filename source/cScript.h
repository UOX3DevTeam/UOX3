#ifndef __CSCRIPT_H__
#define __CSCRIPT_H__


class CPIGumpMenuSelect;
class CPIGumpInput;

enum ScriptEvent
{
	seOnCreateDFN = 0,			//	*	Done for PCs on global script
	seOnCreateTile,
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
	seOnEquipAttempt,		//	**
	seOnEquip,				//	**
	seOnUnequipAttempt,		//	**
	seOnUnequip,			//	**
	seOnUseChecked,			//	**  the event that replaces hardcoded use-stuff
	seOnUseUnChecked,
	seOutOfRange,			//	*	Missing character out of range
	seOnLogin,				//	**
	seOnLogout,
	seOnClick,
	seOnFall,
	seOnSell,
	seOnSellToVendor,
	seOnBuy,
	seOnBuyFromVendor,
	seOnSoldToVendor,
	seOnBoughtFromVendor,
	seOnAISliver,			//	**
	seOnSystemSlice,
	seOnUnknownTrigger,
	seOnLightChange,
	seOnWeatherChange,
	seOnTempChange,
	seOnTimer,				//	**
	seOnDeath,				//	**
	seOnResurrect,			//	**
	seOnFlagChange,			//	**
	seOnHungerChange,		//	**
	seOnThirstChange,		//  **
	seOnStolenFrom,			//	**
	seOnSnooped,			//	**
	seOnSnoopAttempt,		//	**
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
	seOnIterateSpawnRegions,
	seOnPacketReceive,
	seOnCharDoubleClick,	//	**  the event that replaces hardcoded character doubleclick-stuff
	seOnSkillGump,			//	**	allows overriding client's request to open default skill gump
	seOnCombatStart,		//	**	allows overriding what happens when combat is initiated
	seOnCombatEnd,			//	**	allows overriding what happens when combat ends
	seOnDeathBlow,
	seOnCombatDamageCalc,
	seOnDamage,
	seOnGumpPress,
	seOnGumpInput,
	seOnDropItemOnItem,
	seOnVirtueGumpPress,
	seOnUseBandageMacro,	//	**	allows overriding what happens when client uses bandage macros
	seOnHouseCommand,		//	**	allows overriding what happens when player speaks house commands
	seOnPathfindEnd,
	seOnEnterEvadeState,
	seOnQuestGump,
	seOnSpecialMove
};

struct SEGump
{
	STRINGLIST *one, *two;
	UI32 TextID;
};

struct SEGumpData
{
	STRINGLIST			sEdits;
	std::vector<SI32>	nButtons;
	std::vector<SI16>	nIDs;
};

class cScript
{
private:

	JSScript *			targScript;
	JSContext *			targContext;
	JSObject *			targObject;

	UI08				runTime;

	bool				isFiring;
	std::bitset< 32 >	eventPresence[3];
	std::bitset< 32 >	needsChecking[3];
	bool				EventExists( ScriptEvent eventNum ) const;
	bool				NeedsChecking( ScriptEvent eventNum ) const;
	void				SetEventExists( ScriptEvent eventNum, bool status );
	void				SetNeedsChecking( ScriptEvent eventNum, bool status );
	bool				ExistAndVerify( ScriptEvent eventNum, std::string functionName );

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


	//|	Modification	-	08162003 - Added these event to handle any script initialization and clean up as the server starts, and is shut down
	bool		OnStart( void );
	bool		OnStop( void );
	//
	bool		OnPacketReceive( CSocket *mSock, UI16 packetNum );
	bool		OnIterate( CBaseObject *a, UI32 &b );
	bool		OnIterateSpawnRegions( CSpawnRegion *a, UI32 &b );
	bool		OnCreate( CBaseObject *thingCreated, bool dfnCreated );
	SI08		OnCommand( CSocket *mSock, std::string command );
	bool		OnDelete( CBaseObject *thingDestroyed );
	SI08		OnSpeech( const char *speech, CChar *personTalking, CBaseObject *talkingTo );
	bool		InRange( CChar *person, CBaseObject *objInRange );
	SI08		OnCollide( CSocket *targSock, CChar *objColliding, CBaseObject *objCollideWith );
	SI08		OnSteal( CChar *thief, CItem *theft, CChar *victim );
	SI08		OnPathfindEnd( CChar *npc, SI08 pathfindResult );
	SI08		OnEnterEvadeState( CChar *npc, CChar *enemy );
	SI08		OnDispel( CBaseObject *dispelled );
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
	SI08		OnDrop( CItem *item, CChar *dropper );
	SI08		OnPickup( CItem *item, CChar *pickerUpper );
	SI08		OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg );
	SI08		OnDecay( CItem *decaying );
	SI08		OnLeaving( CMultiObj *left, CBaseObject *leaving );
	SI08		OnEntrance( CMultiObj *left, CBaseObject *leaving );
	SI08		OnEquipAttempt( CChar *equipper, CItem *equipping );
	SI08		OnEquip( CChar *equipper, CItem *equipping );
	SI08		OnUnequipAttempt( CChar *equipper, CItem *equipping );
	SI08		OnUnequip( CChar *equipper, CItem *equipping );
	SI08		OnUseChecked( CChar *user, CItem *iUsing );
	SI08		OnUseUnChecked( CChar *user, CItem *iUsing );
	bool		OutOfRange( CChar *person, CBaseObject *objVanish );
	bool		OnLogin( CSocket *sockPlayer, CChar *pPlayer );
	bool		OnLogout( CSocket *sockPlayer, CChar *pPlayer );
	SI08		OnClick( CSocket *sockPlayer, CBaseObject *objClicked );
	bool		OnFall( CChar *pFall, SI08 fallDistance );
	SI08		OnAISliver( CChar *pSliver );
	bool		OnSystemSlice( void );
	SI08		OnLightChange( CBaseObject *tObject, UI08 lightLevel );
	bool		OnWeatherChange( CBaseObject *tObject, WeatherType element );
	bool		OnTempChange( CBaseObject *tObject, SI08 temp );
	bool		OnTimer( CBaseObject *tObject, UI08 timerID );
	SI08		OnDeath( CChar *pDead, CItem *iCorpse );
	SI08		OnResurrect( CChar *pAlive );
	SI08		OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus );
	SI08		OnHungerChange( CChar *pChanging, SI08 newStatus );
	bool		OnThirstChange( CChar* pChanging, SI08 newStatus );
	SI08		OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen );
	SI08		OnSnooped( CChar *snooped, CChar *snooper, bool success );
	SI08		OnSnoopAttempt( CChar *snooped, CChar *snooper );
	bool		OnEnterRegion( CChar *entering, UI16 region );
	bool		OnLeaveRegion( CChar *entering, UI16 region );
	SI08		OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum );
	bool		DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum );
	SI16		OnSpellCast( CChar *tChar, UI08 SpellID );
	SI16		OnScrollCast( CChar *tChar, UI08 SpellID );
	SI08		OnSpellSuccess( CChar *tChar, UI08 SpellID );
	SI08		OnTalk( CChar *myChar, const char *mySpeech );
	bool		OnSpeechInput( CChar *myChar, CItem *myItem, const char *mySpeech );
	SI08		OnSpellGain( CItem *book, const UI08 spellNum );
	SI08		OnSpellLoss( CItem *book, const UI08 spellNum );
	SI08		OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill );
	SI08		OnDropItemOnNpc( CChar *srcChar, CChar *targChar, CItem *i );
	SI08		OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest );
	SI08		OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonID );
	SI08		OnQuestGump( CChar *mChar );
	SI08		OnSpecialMove( CChar *mChar, UI08 abilityID );

	bool		AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s );
	bool		CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams );

	bool		ScriptRegistration( std::string scriptType );

	bool		executeCommand( CSocket *s, std::string funcName, std::string executedString );

	bool		MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, SI32 spellNum );
	SI08		OnCharDoubleClick( CChar *currChar, CChar *targChar );
	SI08		OnSkillGump( CChar *mChar );
	SI08		OnUseBandageMacro( CSocket *mSock, CChar *targChar, CItem *bandageItem );
	SI08		OnCombatStart( CChar *attacker, CChar *defender );
	SI08		OnCombatEnd( CChar *attacker, CChar *defender );

	SI08		OnDeathBlow( CChar *mKilled, CChar *mKiller );

	SI16		OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill, UI08 hitLoc );
	bool		OnDamage( CChar *damaged, CChar *attacker, SI16 damageValue );
	SI08		OnBuy( CSocket *targSock, CChar *objVendor );
	SI08		OnBuyFromVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemBought );
	SI08		OnSellToVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemSold );
	SI08		OnSell( CSocket *targSock, CChar *objVendor );
	SI08		OnBoughtFromVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemBought );
	SI08		OnSoldToVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemSold );
	SI08		OnHouseCommand( CSocket *targSock, CMultiObj *multiObj, UI08 targID );

	//	Critical handler type stuff
	bool		IsFiring( void );
	void		Firing( void );
	void		Stop( void );
};

#endif

