#ifndef __COMBAT_H__
#define __COMBAT_H__

class cCombat
{
private:
	void	ItemCastSpell( cSocket *s, CChar *c, CItem *i );
	void	ItemSpell( CChar *attacker, CChar *defender );
	bool	CastSpell( CChar *attack, CChar *defend, SI16 spellNum );
	void	doSoundEffect( CChar *p, CItem *weapon );
	void	doMissedSoundEffect( CChar *p );
	void	Kill( CChar *attack, CChar *defend );
public:
//	UI16	GetSwingRate( CChar *iNPC, CItem *weapon );
	UI08	getBowType( CChar *i );
	UI08	getWeaponType( CItem *i );
	UI08	getCombatSkill( CChar *i );
	CItem *	getWeapon( CChar *i );
	CItem *	getShield( CChar *i );
	SI16	calcAtt( CChar *p );
	UI16	calcDef( CChar *p, SI08 x, bool doDamage);
	void	CombatOnHorse( CChar *i );
	void	CombatOnFoot( CChar *i );
	void	CombatHit( CChar *attack, CChar *defend, SI32 weaponType = -1 );
	void	DoCombat( CChar *attack );
	void	SpawnGuard( CChar *mChar, CChar *targChar, SI16 x, SI16 y, SI08 z );
	bool	TimerOk( CChar *c );
};

#endif

