#ifndef __CMAGIC_H__
#define __CMAGIC_H__

struct reag_st;

class cMagic
{
public:

	// adding constructor/destructor
	cMagic();
	~cMagic(); // no need for a virtual destructor as long as no subclasses us
	bool HasSpell( CItem *book, SI32 spellNum );
	void AddSpell( CItem *book, SI32 spellNum );
	void RemoveSpell( CItem *book, SI32 spellNum );

	void LoadScript( void ); // adding function for spell system "cache"
	void SpellBook( CSocket *mSock );
	void GateCollision( CSocket *mSock, CChar *mChar, CItem *itemCheck, ItemTypes type );
	bool SelectSpell( CSocket *mSock, SI32 num );
	void doMoveEffect( SI32 num, CBaseObject *target, CChar *source );
	void doStaticEffect( CChar *source, SI32 num );
	void playSound( CChar *source, SI32 num );
	void DelReagents( CChar *s, reag_st reags );
	void CastSpell( CSocket *s, CChar *caster );	// changed for NPC casting
	bool CheckResist( CChar *attacker, CChar *defender, SI32 circle );
	void PoisonDamage( CChar *p, SI32 posion );
	void CheckFieldEffects( CChar& mChar );
	bool HandleFieldEffects( CChar *mChar, CItem *fieldItem, UI16 id );
	bool CheckBook( SI32 circle, SI32 spell, CItem *i );
	bool CheckReagents( CChar *s, const reag_st *reagents );
	bool CheckMana( CChar *s, SI32 num );
	bool CheckStamina( CChar *s, SI32 num );
	bool CheckHealth( CChar *s, SI32 num );
	bool CheckMagicReflect( CChar *i );

	void MagicDamage( CChar *p, SI16 amount, CChar *attacker = nullptr, WeatherType element = NONE );
	void SpellFail( CSocket *s );
	void SubtractMana( CChar *s, SI32 mana );
	void SubtractStamina( CChar *s, SI32 stamina );
	void SubtractHealth( CChar *s, SI32 health, SI32 spellNum );
	void MagicTrap( CChar *s, CItem *i );
	void Polymorph( CSocket *s, UI16 polyID );
	void BoxSpell( CSocket *s, CChar *caster, SI16& x1, SI16& x2, SI16& y1, SI16& y2, SI08& z1, SI08& z2 );
	void SummonMonster( CSocket *s, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z );
	void PolymorphMenu( CSocket *s, UI16 gmindex );

	std::vector< SpellInfo >	spells;		// adding variable for spell system "cache"

	void	Register( cScript *toRegister, SI32 spellNumber, bool isEnabled );
	void	SetSpellStatus( SI32 spellNumber, bool isEnabled );

private:
	UI08 getFieldDir( CChar *s, SI16 x, SI16 y );
	bool RegMsg( CChar *s, reag_st failmsg );
	void Log( std::string spell, CChar *player1, CChar *player2, const std::string &extraInfo );
	UI08 spellCount;

};

extern cMagic *Magic;

#endif

