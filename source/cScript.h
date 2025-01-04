#ifndef __CSCRIPT_H__
#define __CSCRIPT_H__


class CPIGumpMenuSelect;
class CPIGumpInput;

enum ScriptEvent
{
	seOnCreateDFN = 0,
	seOnCreateTile,
	seOnCreatePlayer,		//	*	Done for PCs on global script
	seOnCommand,
	seOnDelete,				//	**
	seOnSpeech,				//	*	Missing item response at the moment
	seInRange,				//	*	Missing character in range
	seOnCollide,			//	**	Items only
	seOnMoveDetect,			//	***
	seOnSteal,				//	**
	seOnDispel,				//	**
	seOnSkill,
	seOnStat,
	seOnTooltip,
	seOnNameRequest,
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
	seOnContRemoveItem,
	seOnSwing,
	seOnDecay,
	seOnTransfer,
	seOnEntrance,			//	**
	seOnLeaving,			//	**
	seOnMultiLogout,		//	**
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
	seOnLoyaltyChange,		//	**
	seOnHungerChange,		//	**
	seOnThirstChange,		//  **
	seOnStolenFrom,			//	**
	seOnSnooped,			//	**
	seOnSnoopAttempt,		//	**
	seOnEnterRegion,		//  **
	seOnLeaveRegion,		//	**
	seOnSpellTarget,
	seOnSpellTargetSelect,
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
	seOnAICombatTarget,		//	**	allows overriding target selection taking place for regular AI behaviours
	seOnCombatEnd,			//	**	allows overriding what happens when combat ends
	seOnCombatHit,			//	**	allows overriding what happens when combat hits
	seOnDeathBlow,
	seOnCombatDamageCalc,
	seOnDamage,
	seOnDamageDeal,
	seOnGumpPress,
	seOnGumpInput,
	seOnScrollingGumpPress,
	seOnDropItemOnItem,
	seOnVirtueGumpPress,
	seOnUseBandageMacro,	//	**	allows overriding what happens when client uses bandage macros
	seOnHouseCommand,		//	**	allows overriding what happens when player speaks house commands
	seOnMakeItem,
	seOnPathfindEnd,
	seOnEnterEvadeState,
	seOnCarveCorpse,
	seOnDyeTarget,
	seOnQuestGump,
	seOnHelpButton,
	seOnContextMenuRequest,
	seOnContextMenuSelect,
	seOnWarModeToggle,
	seOnSpecialMove,
	seOnFacetChange
};

struct SEGump_st
{
	std::vector<std::string> *one, *two;
	UI32 textId;
};

struct SEGumpData_st
{
	std::vector<std::string>	sEdits;
	std::vector<SI32>			nButtons;
	std::vector<SI16>			nIDs;
};

class cScript
{
private:

	JSScript *			targScript;
	JSContext *			targContext;
	JSObject *			targObject;

	bool				isFiring;
	UI08				runTime;

	std::bitset<64>		eventPresence[3];
	std::bitset<64>		needsChecking[3];
	bool				EventExists( ScriptEvent eventNum ) const;
	bool				NeedsChecking( ScriptEvent eventNum ) const;
	void				SetEventExists( ScriptEvent eventNum, bool status );
	void				SetNeedsChecking( ScriptEvent eventNum, bool status );
	bool				ExistAndVerify( ScriptEvent eventNum, std::string functionName );

	std::vector<SEGump_st *>		gumpDisplays;

	void		Cleanup( void );

public:
	void		CollectGarbage( void );

	size_t		NewGumpList( void );
	SEGump_st *	GetGumpList( SI32 index );
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
	bool		OnCreate( CBaseObject *thingCreated, bool dfnCreated, bool isPlayer );
	bool		DoesEventExist( char *eventToFind );
	SI08		OnCommand( CSocket *mSock, std::string command );
	bool		OnDelete( CBaseObject *thingDestroyed );
	SI08		OnSpeech( const char *speech, CChar *personTalking, CBaseObject *talkingTo );
	bool		InRange( CBaseObject *srcObj, CBaseObject *objInRange );
	SI08		OnCollide( CSocket *targSock, CChar *objColliding, CBaseObject *objCollideWith );
	SI08		OnMoveDetect( CBaseObject *sourceObj, CChar *CharInRange, UI08 rangeToChar, UI16 oldCharX, UI16 oldCharY );
	SI08		OnSteal( CChar *thief, CItem *theft, CChar *victim );
	SI08		OnPathfindEnd( CChar *npc, SI08 pathfindResult );
	SI08		OnEnterEvadeState( CChar *npc, CChar *enemy );
	SI08		OnCarveCorpse( CChar *player, CItem *corpse );
	SI08		OnDyeTarget( CChar *player, CItem *dyeTub, CItem *target );
	SI08		OnDispel( CBaseObject *dispelled );
	bool		OnSkill( CBaseObject *skillUse, SI08 skillUsed );
	bool		OnStat( void );
	std::string		OnTooltip( CBaseObject *myObj, CSocket *pSocket );
	std::string		OnNameRequest( CBaseObject *myObj, CChar *nameRequester, UI08 requestSource );
	bool		OnAttack( CChar *attacker, CChar *defender );
	bool		OnDefense( CChar *attacker, CChar *defender );
	SI08		OnSkillGain( CChar *player, SI08 skill, UI32 skillAmtGained );
	SI08		OnSkillLoss( CChar *player, SI08 skill, UI32 skillAmtLost );
	bool		OnSkillChange( CChar *player, SI08 skill, SI32 skillAmtChanged );
	SI08		OnStatGained( CChar *player, UI32 stat, SI08 skill, UI32 statGainedAmount );
	bool		OnStatGain( CChar *player, UI32 stat, SI08 skill, UI32 statGainAmount );
	SI08		OnStatLoss( CChar *player, UI32 stat, UI32 statLossAmount );
	bool		OnStatChange( CChar *player, UI32 stat, SI32 statChangeAmount );
	SI08		OnDrop( CItem *item, CChar *dropper );
	SI08		OnPickup( CItem *item, CChar *pickerUpper, CBaseObject *objCont );
	bool		OnContRemoveItem( CItem *contItem, CItem *item, CChar *pickerUpper );
	SI08		OnSwing( CItem *swinging, CChar *swinger, CChar *swingTarg );
	SI08		OnDecay( CItem *decaying );
	SI08		OnEntrance( CMultiObj *left, CBaseObject *leaving );
	SI08		OnLeaving( CMultiObj *left, CBaseObject *leaving );
	SI08		OnMultiLogout( CMultiObj* iMulti, CChar* cPlayer );
	SI08		OnEquipAttempt( CChar *equipper, CItem *equipping );
	SI08		OnEquip( CChar *equipper, CItem *equipping );
	SI08		OnUnequipAttempt( CChar *equipper, CItem *equipping );
	SI08		OnUnequip( CChar *equipper, CItem *equipping );
	SI08		OnUseChecked( CChar *user, CItem *iUsing );
	SI08		OnUseUnChecked( CChar *user, CItem *iUsing );
	bool		OutOfRange( CBaseObject *srcObj, CBaseObject *objVanish );
	bool		OnLogin( CSocket *sockPlayer, CChar *pPlayer );
	bool		OnLogout( CSocket *sockPlayer, CChar *pPlayer );
	SI08		OnClick( CSocket *sockPlayer, CBaseObject *objClicked );
	bool		OnFall( CChar *pFall, SI08 fallDistance );
	SI08		OnAISliver( CChar *pSliver );
	bool		OnSystemSlice( void );
	SI08		OnLightChange( CBaseObject *tObject, UI08 lightLevel );
	bool		OnWeatherChange( CBaseObject *tObject, WeatherType element );
	bool		OnTempChange( CBaseObject *tObject, SI08 temp );
	bool		OnTimer( CBaseObject *tObject, UI16 timerId );
	SI08		OnDeath( CChar *pDead, CItem *iCorpse );
	SI08		OnResurrect( CChar *pAlive );
	SI08		OnFlagChange( CChar *pChanging, UI08 newStatus, UI08 oldStatus );
	SI08		OnLoyaltyChange( CChar *pChanging, SI08 newStatus );
	SI08		OnHungerChange( CChar *pChanging, SI08 newStatus );
	bool		OnThirstChange( CChar* pChanging, SI08 newStatus );
	SI08		OnStolenFrom( CChar *stealing, CChar *stolenFrom, CItem *stolen );
	SI08		OnSnooped( CChar *snooped, CChar *snooper, bool success );
	SI08		OnSnoopAttempt( CChar *snooped, CItem *pack, CChar *snooper );
	bool		OnEnterRegion( CChar *entering, UI16 region );
	bool		OnLeaveRegion( CChar *entering, UI16 region );
	SI08		OnSpellTarget( CBaseObject *target, CChar *caster, UI08 spellNum );
	SI08		OnSpellTargetSelect( CChar *caster, CBaseObject *target, UI08 spellNum );
	bool		DoCallback( CSocket *tSock, SERIAL targeted, UI08 callNum );
	SI16		OnSpellCast( CChar *tChar, UI08 SpellId );
	SI16		OnScrollCast( CChar *tChar, UI08 SpellId );
	SI08		OnSpellSuccess( CChar *tChar, UI08 SpellId );
	SI08		OnTalk( CChar *myChar, const char *mySpeech );
	bool		OnSpeechInput( CChar *myChar, CItem *myItem, const char *mySpeech );
	SI08		OnSpellGain( CItem *book, const UI08 spellNum );
	SI08		OnSpellLoss( CItem *book, const UI08 spellNum );
	SI08		OnSkillCheck( CChar *myChar, const UI08 skill, const UI16 lowSkill, const UI16 highSkill, bool isCraftSkill );
	SI08		OnDropItemOnNpc( CChar *srcChar, CChar *targChar, CItem *i );
	SI08		OnDropItemOnItem( CItem *item, CChar *dropper, CItem *dest );
	SI08		OnVirtueGumpPress( CChar *mChar, CChar *tChar, UI16 buttonId );
	SI08		OnScrollingGumpPress( CSocket *tSock, UI16 gumpId, UI16 buttonId );
	SI08		OnQuestGump( CChar *mChar );
	SI08		OnHelpButton( CChar *mChar );
	SI08		OnContextMenuRequest( CSocket *tSock, CBaseObject *baseObj );
	SI08		OnContextMenuSelect( CSocket *tSock, CBaseObject *baseObj, UI16 popupEntry );
	SI08		OnWarModeToggle( CChar *mChar );
	SI08		OnSpecialMove( CChar *mChar, UI08 abilityId );
	SI08		OnFacetChange( CChar *mChar, const UI08 oldFacet, const UI08 newFacet );

	bool		AreaObjFunc( char *funcName, CBaseObject *srcObject, CBaseObject *tmpObject, CSocket *s );
	bool		CallParticularEvent( const char *eventToCall, jsval *params, SI32 numParams, jsval *eventRetVal );

	bool		ScriptRegistration( std::string scriptType );

	bool		executeCommand( CSocket *s, std::string funcName, std::string executedString );

	bool		MagicSpellCast( CSocket *mSock, CChar *tChar, bool directCast, SI32 spellNum );
	SI08		OnCharDoubleClick( CChar *currChar, CChar *targChar );
	SI08		OnSkillGump( CChar *mChar );
	SI08		OnUseBandageMacro( CSocket *mSock, CChar *targChar, CItem *bandageItem );
	SI08		OnAICombatTarget( CChar *attacker, CChar *target );
	SI08		OnCombatHit( CChar *attacker, CChar *defender );
	SI08		OnCombatStart( CChar *attacker, CChar *defender );
	SI08		OnCombatEnd( CChar *attacker, CChar *defender );

	SI08		OnDeathBlow( CChar *mKilled, CChar *mKiller );

	SI16		OnCombatDamageCalc( CChar *attacker, CChar *defender, UI08 getFightSkill, UI08 hitLoc );
	SI08		OnDamage( CChar *damaged, CChar *attacker, SI16 damageValue, WeatherType damageType );
	SI08		OnDamageDeal( CChar *attacker, CChar *damaged, SI16 damageValue, WeatherType damageType );
	SI08		OnBuy( CSocket *targSock, CChar *objVendor );
	SI08		OnBuyFromVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemBought, UI16 numItemsBuying );
	SI08		OnSellToVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemSold, UI16 numItemsSelling );
	SI08		OnSell( CSocket *targSock, CChar *objVendor );
	SI08		OnBoughtFromVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemBought, UI16 numItemsBought );
	SI08		OnSoldToVendor( CSocket *targSock, CChar *objVendor, CBaseObject *objItemSold, UI16 numItemsSold );
	SI08		OnHouseCommand( CSocket *targSock, CMultiObj *multiObj, UI08 targId );
	SI08		OnMakeItem( CSocket *mSock, CChar *objChar, CItem *objItem, UI16 createEntryId );

	//	Critical handler type stuff
	bool		IsFiring( void );
	void		Firing( void );
	void		Stop( void );
};

#endif

