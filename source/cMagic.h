#ifndef __CMAGIC_H__
#define __CMAGIC_H__

class cMagic
{
public:

	//:Terrin: adding constructor/destructor
	cMagic();
	virtual ~cMagic(); // no need for a virtual destructor as long as no subclasses us
	bool HasSpell( CItem *book, int spellNum );
	void AddSpell( CItem *book, int spellNum );
	void RemoveSpell( CItem *book, int spellNum );

	void LoadScript( void ); //:Terrin: adding function for spell system "cache"
	void SpellBook( cSocket *mSock );
	bool GateCollision( CChar *s );
	void GateDestruction( void );
	bool SelectSpell( cSocket *mSock, int num );
	void doMoveEffect( int num, cBaseObject *target, CChar *source );
	void doStaticEffect( CChar *source, int num );
	void playSound( CChar *source, int num );
	void DelReagents( CChar *s, reag_st reags );
	void CastSpell( cSocket *s, CChar *caster );	// changed for NPC casting - Hanse
	bool CheckResist( CChar *attacker, CChar *defender, int circle );
	void PoisonDamage( CChar *p, int posion );
	void CheckFieldEffects2( CChar *c, char timecheck );
	bool CheckParry( CChar *player, int circle);
	bool CheckBook( int circle, int spell, CItem *i );
	bool CheckReagents( CChar *s, const reag_st *reagents );
	bool CheckMana( CChar *s, int num );
	bool CheckStamina( CChar *s, int num );
	bool CheckHealth( CChar *s, int num );
	void Recall( cSocket *s ); // we need these four for the commands (separately) !!!
	void Mark( cSocket *s );
	void Gate( cSocket *s );
	void Heal( cSocket *s );
	bool CheckMagicReflect( CChar *i );
	void MagicDamage( CChar *p, int amount, CChar *attacker = NULL );
	void SpellFail( cSocket *s );
	void SubtractMana( CChar *s, int mana );
	void SubtractStamina( CChar *s, int stamina );
	void SubtractHealth( CChar *s, int health, int spellNum );
	void MagicTrap( CChar *s, CItem *i );
	void Polymorph( cSocket *s, int gmindex, int creaturenumer);
	void BoxSpell( cSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2 );
	void SummonMonster( cSocket *s, CChar *caster, UI16 id, char *monstername, UI16 color, SI16 x, SI16 y, SI08 z );
	void PolymorphMenu( cSocket *s, int gmindex );


private:
	bool RegMsg( CChar *s, reag_st failmsg );
	void Log( const char *spell, CChar *player1, CChar *player2, const char *extraInfo );
	UI32 spellCount;

};


#endif

