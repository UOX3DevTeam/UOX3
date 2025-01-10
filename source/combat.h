#ifndef __COMBAT_H__
#define __COMBAT_H__

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
	BLOWGUNS,
	THROWN
};

class CHandleCombat
{
private:
	bool	HandleCombat( CSocket *mSock, CChar& mChar, CChar *ourTarg );

	bool	CastSpell( CChar *mChar, CChar *ourTarg, SI32 spellNum );

	SI16	CalcDamage( CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc );

	void	PlaySwingAnimations( CChar *mChar );
	void	PlayHitSoundEffect( CChar *mChar, CItem *mWeapon );
	void	PlayMissedSoundEffect( CChar *mChar );

	void	HandleNPCSpellAttack( CChar *mChar, CChar *ourTarg, UI16 playerDistance );

	CItem *	CheckDef( CItem *checkItem, CItem *currItem, SI32 &currDef, WeatherType resistType );
	CItem *	GetArmorDef( CChar *mChar, SI32 &totalDef, UI08 bodyLoc, bool findTotal = false, WeatherType resistType = NONE );

public:
	bool	StartAttack( CChar *mChar, CChar *ourTarg );
	void	InvalidateAttacker( CChar *mChar );

	R32		GetCombatTimeout( CChar *mChar );
	void	PlayerAttack( CSocket *s );
	void	AttackTarget( CChar *mChar, CChar *ourTarg );
	void	PetGuardAttack( CChar *mChar, CChar *owner, CBaseObject *guarded, CChar *petGuard = nullptr );

	void	CombatLoop( CSocket *mSock, CChar& mChar );
	void	Kill( CChar *mChar, CChar *ourTarg );

	void	DoHitMessage( CChar *mChar, CChar *ourTarg, SI08 hitLoc, SI16 damage );
	SI08	CalculateHitLoc( void );
	UI16	CalcDef( CChar *mChar, UI08 hitLoc, bool doDamage = false, WeatherType element = PHYSICAL );
	SI16	CalcAttackPower( CChar *mChar, bool doDamage = false );
	SI16	CalcLowDamage( CChar *p );
	SI16	CalcHighDamage( CChar *p );
	UI08	GetCombatSkill( CItem *wItem );
	UI08	GetBowType( CItem *bItem );
	UI08	GetWeaponType( CItem *i );

	CItem *	GetShield( CChar *mChar );
	CItem *	GetWeapon( CChar *mChar );

	SI16	ApplyDamageBonuses( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage );
	SI16	ApplyDefenseModifiers( WeatherType damageType, CChar *mChar, CChar *ourTarg, UI08 getFightSkill, UI08 hitLoc, SI16 baseDamage, bool doArmorDamage );

	SI16	AdjustRaceDamage( CChar *attack, CChar *defend, CItem *weapon, SI16 bDamage, UI08 hitLoc, UI08 getFightSkill );
	SI16	AdjustArmorClassDamage( CChar *attack, CChar *defend, CItem *weapon, SI16 bDamage, UI08 hitLoc );
	void	SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z );

	// Combat Animations & Sound Effects
	void	CombatAnimsNew( CChar *mChar );
	void	CombatOnHorse( CChar *mChar );
	void	CombatOnFoot( CChar *mChar );
};

extern CHandleCombat *Combat;

#endif

