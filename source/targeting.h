#ifndef __TARGETING_H__
#define __TARGETING_H__

namespace UOX
{

enum TargetIDs
{
	TARGET_ADDITEM = 0,
	TARGET_ADDSCRIPTITEM,
	TARGET_ADDNPC,
	TARGET_ADDSCRIPTNPC,
	TARGET_ADDSPAWNER,
	TARGET_ADDSCRIPTSPAWNER,
	TARGET_TELE,
	TARGET_DYE,
	TARGET_XGO,
	TARGET_KEY,
	TARGET_ISTATS,
	TARGET_CSTATS,
	TARGET_REPAIRLEATHER,
	TARGET_REPAIRBOW,
	TARGET_KILL,
	TARGET_RESURRECT,
	TARGET_REPAIRMETAL,
	TARGET_KICK,
	TARGET_ARMSLORE,
	TARGET_OWNER2,
	TARGET_DYEALL,
	TARGET_DVAT,
	TARGET_FREEZE,
	TARGET_UNFREEZE,
	TARGET_ALLSET,
	TARGET_ANATOMY,
	TARGET_ITEMID,
	TARGET_EVALINT,
	TARGET_TAME,
	TARGET_FISH,
	TARGET_INFO,
	TARGET_SHOWDETAIL,
	TARGET_TOWNALLY,
	TARGET_SMITH,
	TARGET_MINE,
	TARGET_SMELTORE,
	TARGET_WSTATS,
	TARGET_NPCRESURRECT,
	TARGET_NPCCIRCLE,
	TARGET_NPCWANDER,
	TARGET_TWEAK,
	TARGET_NPCRECT,
	TARGET_MAKESTATUS,
	TARGET_SETSCPTRIG,
	TARGET_DELETECHAR,
	TARGET_AXE,
	TARGET_DETECTHIDDEN,
	TARGET_PROVOCATION,
	TARGET_PROVOCATION2,
	TARGET_ENTICEMENT,
	TARGET_ENTICEMENT2,
	TARGET_SWORD,
	TARGET_AREACOMMAND,
	TARGET_CASTSPELL,
	TARGET_ALCHEMY,
	TARGET_DUPE,
	TARGET_MAKESHOP,
	TARGET_FOLLOW,
	TARGET_ATTACK,
	TARGET_TRANSFER,
	TARGET_GUARD,
	TARGET_RESTOCK,
	TARGET_FRIEND,
	TARGET_JAIL,
	TARGET_RELEASE,
	TARGET_CREATEBANDAGE,
	TARGET_HEALING,
	TARGET_PERMHIDE,
	TARGET_UNHIDE,
	TARGET_CARPENTRY,
	TARGET_XTELEPORT,
	TARGET_BEGGING,
	TARGET_ANIMALLORE,
	TARGET_FORENSICS,
	TARGET_APPLYPOISON,
	TARGET_POISONITEM,
	TARGET_INSCRIBE,
	TARGET_VOTEFORMAYOR,
	TARGET_LOCKPICK,
	TARGET_WHEEL,
	TARGET_LOOM,
	TARGET_TAILORING,
	TARGET_COOKING,
	TARGET_LOADCANNON,
	TARGET_FLETCHING,
	TARGET_TINKERING,
	TARGET_TINKERAXEL,
	TARGET_TINKERAWG,
	TARGET_TINKERCLOCK,
	TARGET_VIAL,
	TARGET_TILING,
	TARGET_WIPING,
	TARGET_STEALING,
	TARGET_POSSESS,
	TARGET_TELESTUFF,
	TARGET_TELESTUFF2,
	TARGET_SQUELCH,
	TARGET_PLVBUY,
	TARGET_HOUSEOWNER,
	TARGET_HOUSEEJECT,
	TARGET_HOUSEBAN,
	TARGET_HOUSEFRIEND,
	TARGET_HOUSEUNLIST,
	TARGET_HOUSELOCKDOWN,
	TARGET_HOUSERELEASE,
	TARGET_SMELT,
	TARGET_SHOWSKILLS,
	TARGET_INCZ,
	TARGET_INCX,
	TARGET_INCY,
	TARGET_BUILDHOUSE,
	TARGET_NOFUNC
};

class cTargets
{
private:
	void			AddItem( cSocket *mSock, ObjectType = OT_ITEM );
	void			AddScriptItem( cSocket *mSock, ObjectType = OT_ITEM );
	TargetFunc AddNpc;
	TargetFunc AddScriptNpc;

	TargetFunc AllSetTarget;
	TargetFunc AreaCommand;
	TargetFunc AttackTarget;
	TargetFunc AxeTarget;
	TargetFunc CloseTarget;
	TargetFunc ColorsTarget;
	TargetFunc CorpseTarget;
	TargetFunc DeleteCharTarget;
	TargetFunc DupeTarget;
	TargetFunc DvatTarget;
	TargetFunc DyeTarget;
	TargetFunc FollowTarget;
	TargetFunc GuardTarget;
	TargetFunc FriendTarget;
	TargetFunc HandleGuildTarget;
	TargetFunc HandleSetScpTrig;
	TargetFunc HouseBanTarget;
	TargetFunc HouseEjectTarget;
	TargetFunc HouseFriendTarget;
	TargetFunc HouseLockdown;
	TargetFunc HouseOwnerTarget;
	TargetFunc HouseRelease;
	TargetFunc HouseUnlistTarget;
	TargetFunc InfoTarget;
	TargetFunc IstatsTarget;
	TargetFunc KeyTarget;
	TargetFunc LoadCannon;
	TargetFunc MakeShopTarget;
	TargetFunc MakeStatusTarget;
	TargetFunc MakeTownAlly;
	TargetFunc permHideTarget;
	TargetFunc PlVBuy;
	TargetFunc PossessTarget;
	TargetFunc ResurrectionTarget;
	TargetFunc ShowDetail;
	TargetFunc ShowSkillTarget;
	TargetFunc SmeltTarget;
	TargetFunc SquelchTarg;
	TargetFunc SwordTarget;
	TargetFunc TeleStuff;
	TargetFunc TeleStuff2;
	TargetFunc TeleTarget;
	TargetFunc Tiling;
	TargetFunc TransferTarget;
	TargetFunc unHideTarget;
	TargetFunc WstatsTarget;
	TargetFunc VialTarget;
	TargetFunc BuildHouseTarget;

	TargetFunc KillTarget;
	void	newCarveTarget( cSocket *s, CItem *i );
	void	MultiSetCharTarget( cSocket *s, TargetIDs targetID );
	void	MultiSetItemTarget( cSocket *s, TargetIDs targetID );
public:
	TargetFunc CstatsTarget;
	TargetFunc TweakTarget;
	TargetFunc MultiTarget;
	TargetFunc Wiping;

	void	NpcResurrectTarget( CChar *s );
	void	JailTarget( cSocket *s, SERIAL c );
	void	ReleaseTarget( cSocket *s, SERIAL c );
	void	XTeleport( cSocket *s, UI08 x );
	bool	BuyShop( cSocket *s, CChar *c );
};

extern cTargets *Targ;

}

#endif
