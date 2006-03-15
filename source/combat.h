#ifndef __COMBAT_H__
#define __COMBAT_H__

namespace UOX
{

enum WeaponTypes
{
	DEF_SWORDS = 1,
	SLASH_SWORDS,
	ONEHND_LG_SWORDS,
	TWOHND_LG_SWORDS,
	BARDICHE,
	ONEHND_AXES,
	TWOHND_AXES,
	DEF_MACES,
	LG_MACES,
	BOWS,
	XBOWS,
	DEF_FENCING,
	TWOHND_FENCING,
	DUAL_SWORD,
	DUAL_FENCING_STAB,
	DUAL_FENCING_SLASH,
};

class CHandleCombat
{
private:
	bool	StartAttack( CChar *mChar, CChar *ourTarg );
	void	HandleCombat( CSocket *mSock, CChar& mChar, CChar *ourTarg );
	
	void	InvalidateAttacker( CChar *mChar );
	bool	CastSpell( CChar *mChar, CChar *ourTarg, SI08 spellNum );

	SI16	calcDamage( CChar *mChar, CChar *ourTarg, CSocket *targSock, CItem *mWeapon, UI08 getFightSkill );

	void	PlaySwingAnimations( CChar *mChar );
	void	PlayHitSoundEffect( CChar *mChar, CItem *mWeapon );
	void	PlayMissedSoundEffect( CChar *mChar );

	void	HandleNPCSpellAttack( CChar *mChar, CChar *ourTarg, UI16 playerDistance );
	void	HandleSplittingNPCs( CChar *toSplit );

	CItem *	checkDef( CItem *checkItem, CItem *currItem, SI32 &currDef );
	CItem *	getArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal = false );
	CItem * checkElementDef( CItem *checkItem, CItem *currItem, SI32 &currDef, WeatherType element = NONE );
	CItem * getElementDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal = false , WeatherType element = NONE );

public:
	R32		GetCombatTimeout( CChar *mChar );
	void	PlayerAttack( CSocket *s );
	void	AttackTarget( CChar *mChar, CChar *ourTarg );
	void	petGuardAttack( CChar *mChar, CChar *owner, CBaseObject *guarded );

	void	CombatLoop( CSocket *mSock, CChar& mChar );
	void	Kill( CChar *mChar, CChar *ourTarg );

	SI08	DoHitMessage( CChar *mChar, CChar *ourTarg, CSocket *targSock, SI16 damage );
	UI16	calcDef( CChar *mChar, UI08 hitLoc, bool doDamage = false );
	UI16	calcElementDef( CChar *mChar, UI08 hitLoc, bool doDamage = false, WeatherType element = NONE );
	SI16	calcAtt( CChar *mChar, bool doDamage = false );
	SI16	calcLowDamage( CChar *p );
	SI16	calcHighDamage( CChar *p );
	UI08	getCombatSkill( CItem *wItem );
	UI08	getBowType( CItem *bItem );
	UI08	getWeaponType( CItem *i );
	
	CItem *	getShield( CChar *mChar );
	CItem *	getWeapon( CChar *mChar );

	void	AdjustRaceDamage( CChar *ourTarg, CItem *mWeapon, SI16 &bDamage, UI08 hitLoc, UI16 attSkill );
	void	SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z );
	
	// Combat Animations & Sound Effects
	void	CombatOnHorse( CChar *mChar );
	void	CombatOnFoot( CChar *mChar );
};

extern CHandleCombat *Combat;

}

#endif

