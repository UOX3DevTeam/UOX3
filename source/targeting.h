#ifndef __TARGETING_H__
#define __TARGETING_H__

typedef void (TargetFunc)( cSocket *s );

class cTargets
{
private:
	TargetFunc AddNpcTarget;
	TargetFunc AddTarget;
	TargetFunc AllSetTarget;
	TargetFunc AmountTarget;
	TargetFunc AttackTarget;
	TargetFunc AxeTarget;
	TargetFunc BanTarg;
	TargetFunc BoltTarget;
	TargetFunc BuyShopTarget;
	TargetFunc CanTrainTarget;
	TargetFunc CloseTarget;
	TargetFunc CnsTarget;
	TargetFunc ColorsTarget;
	TargetFunc CommandLevel;
	TargetFunc CorpseTarget;
	TargetFunc DupeTarget;
	TargetFunc DvatTarget;
	TargetFunc DyeTarget;
	TargetFunc ExpPotionTarget;
	TargetFunc FollowTarget;
	TargetFunc FontTarget;
	TargetFunc FreezeTarget;
	TargetFunc FullStatsTarget;
	TargetFunc GhostTarget;
	TargetFunc GlowTarget;
	TargetFunc GmOpenTarget;
	TargetFunc GMTarget;
	TargetFunc GuardTarget;
	TargetFunc FriendTarget;
	TargetFunc HandleGuildTarget;
	TargetFunc HouseBanTarget;
	TargetFunc HouseEjectTarget;
	TargetFunc HouseFriendTarget;
	TargetFunc HouseLockdown;
	TargetFunc HouseOwnerTarget;
	TargetFunc HouseRelease;
	TargetFunc HouseUnlistTarget;
	TargetFunc IncXTarget;
	TargetFunc IncYTarget;
	TargetFunc IncZTarget;
	TargetFunc InfoTarget;
	TargetFunc IstatsTarget;
	TargetFunc KeyTarget;
	TargetFunc LoadCannon;
	TargetFunc MakeShopTarget;
	TargetFunc MakeStatusTarget;
	TargetFunc MakeTownAlly;
	TargetFunc ManaTarget;
	TargetFunc MoreTarget;
	TargetFunc MoreXTarget;
	TargetFunc MoreXYZTarget;
	TargetFunc MoreYTarget;
	TargetFunc MoreZTarget;
	TargetFunc MovableTarget;
	TargetFunc MoveToBagTarget;
	TargetFunc NewXTarget;
	TargetFunc NewYTarget;
	TargetFunc NewzTarget;
	TargetFunc NpcAITarget;
	TargetFunc NpcCircleTarget;
	TargetFunc NpcRectTarget;
	TargetFunc NpcTarget;
	TargetFunc NpcTarget2;
	TargetFunc NpcWanderTarget;
	TargetFunc ObjPrivTarget;
	TargetFunc OwnerTarget;
	TargetFunc permHideTarget;
	TargetFunc PlVBuy;
	TargetFunc PrivTarget;
	TargetFunc RemoveTarget;
	TargetFunc RenameTarget;
	TargetFunc ResurrectionTarget;
	TargetFunc SellStuffTarget;
	TargetFunc SetAdvObjTarget;
	TargetFunc SetDirTarget;
	TargetFunc SetInvulFlag;
	TargetFunc SetPoisonedTarget;
	TargetFunc SetPoisonTarget;
	TargetFunc SetRestockTarget;
	TargetFunc SetSpaDelayTarget;
	TargetFunc SetSpAttackTarget;
	TargetFunc SetSplitChanceTarget;
	TargetFunc SetSplitTarget;
	TargetFunc SetBuyValueTarget;
	TargetFunc SetSellValueTarget;
	TargetFunc SetWipeTarget;
	TargetFunc ShowDetail;
	TargetFunc ShowSkillTarget;
	TargetFunc SmeltTarget;
	TargetFunc SquelchTarg;
	TargetFunc StaminaTarget;
	TargetFunc SwordTarget;
	TargetFunc TeleStuff;
	TargetFunc TeleTarget;
	TargetFunc Tiling;
	TargetFunc TitleTarget;
	TargetFunc TransferTarget;
	TargetFunc TypeTarget;
	TargetFunc UnfreezeTarget;
	TargetFunc UnglowTarget;
	TargetFunc unHideTarget;
	TargetFunc VisibleTarget;
	TargetFunc WstatsTarget;
	TargetFunc xBankTarget;
	TargetFunc XgoTarget;
	TargetFunc xSpecialBankTarget;
	TargetFunc DeleteCharTarget;

	void KillTarget( cSocket *s, UI08 ly);
	void newCarveTarget( cSocket *s, CItem *i );
//	void triggertarget( int ts ); // By Magius(CHE)
public:
	TargetFunc CstatsTarget;
	TargetFunc TweakTarget;
	TargetFunc GetAccount;
	TargetFunc IDtarget;
	TargetFunc MultiTarget;
	TargetFunc Wiping;
	TargetFunc AreaCommand;

	CChar *	NpcMenuTarget( cSocket *s );
	void	NpcResurrectTarget( CChar *s );
	void	JailTarget( cSocket *s, SERIAL c );
	void	ReleaseTarget( cSocket *s, SERIAL c );
	CItem *	AddMenuTarget( cSocket *s, bool x, char *addmitem );
	void	XTeleport( cSocket *s, UI08 x );
	bool	BuyShop( cSocket *s, CChar *c );
};

#endif

