#ifndef __MAGIC_H
#define __MAGIC_H

class CMagicMove
{
private:
	UI16	effect;
	UI08	speed;
	UI08	loop;
	UI08	explode;
public:
	CMagicMove() : effect( INVALIDID ), speed( 0 ), loop( 0 ), explode( 0 )
	{
	}

	UI16	Effect( void ) const
	{
		return effect;
	}
	UI08	Speed( void ) const
	{
		return speed;
	}
	UI08	Loop( void ) const
	{
		return loop;
	}
	UI08	Explode( void ) const
	{
		return explode;
	}
	void	Effect( UI08 p1, UI08 p2 )
	{
		effect = static_cast<UI16>(( p1 << 8 ) + p2 );
	}
	void	Speed( UI08 newVal )
	{
		speed = newVal;
	}
	void	Loop( UI08 newVal )
	{
		loop = newVal;
	}
	void	Explode( UI08 newVal )
	{
		explode = newVal;
	}
};

class CMagicStat
{
private:
	UI16	effect;
	UI08	speed;
	UI08	loop;
public:
	CMagicStat() : effect( INVALIDID ), speed( 0xFF ), loop( 0xFF )
	{
	}
	UI16	Effect( void ) const
	{
		return effect;
	}
	UI08	Speed( void ) const
	{
		return speed;
	}
	UI08	Loop( void ) const
	{
		return loop;
	}
	void	Effect( UI08 p1, UI08 p2 )
	{
		effect = static_cast<UI16>(( p1 << 8 ) + p2 );
	}
	void	Speed( UI08 newVal )
	{
		speed = newVal;
	}
	void	Loop( UI08 newVal )
	{
		loop = newVal;
	}
};

struct Reag_st
{
	UI08 ginseng;
	UI08 moss;
	UI08 drake;
	UI08 pearl;
	UI08 silk;
	UI08 ash;
	UI08 shade;
	UI08 garlic;
	UI08 batwing;
	UI08 daemonblood;
	UI08 gravedust;
	UI08 noxcrystal;
	UI08 pigiron;
	Reag_st() : ginseng( 0 ), moss( 0 ), drake( 0 ), pearl( 0 ), silk( 0 ), ash( 0 ), shade( 0 ), garlic( 0 ), batwing( 0 ), daemonblood( 0 ), gravedust( 0 ),
	noxcrystal( 0 ), pigiron( 0 )
	{
	}
};

class CSpellInfo
{
private:
	SI16		mana;
	SI16		stamina;
	SI16		health;
	R32			delay;			// Casting time of spell
	R32			damageDelay;	// Minimum delay between targeting of a damage spell and the application of damage
	R32			recoveryDelay;	// Minimum delay between the end of one spellcast and the start of another
	UI16		action;
	Reag_st		reags;
	std::string mantra;
	std::string strToSay;	// string visualized with targ. system
	bool		enabled;
	UI08		circle;
	UI16		flags;
	CMagicMove	moveEffect;
	UI16		effect;
	CMagicStat	staticEffect;
	SI16		hiskill;
	SI16		loskill;
	SI16		sclo;
	SI16		schi;
	SI32		tithing;
	UI16		jsScript;
	SI16		baseDmg;
public:
	CSpellInfo() : mana( 0 ), stamina( 0 ), health( 0 ), delay( 0 ), damageDelay( 0 ), recoveryDelay( 1.0f ), action( 0 ), mantra( "" ), strToSay( "" ), enabled( false ),
	circle( 1 ), flags( 0 ), effect( INVALIDID ), hiskill( 0 ), loskill( 0 ), sclo( 0 ), schi( 0 ), tithing( 0 ), jsScript( 0 ), baseDmg( 0 )
	{
	}

	UI16 Action( void ) const
	{
		return action;
	}
	R32 Delay( void ) const
	{
		return delay;
	}
	R32 DamageDelay( void ) const
	{
		return damageDelay;
	}
	R32 RecoveryDelay( void ) const
	{
		return recoveryDelay;
	}
	SI16 Health( void ) const
	{
		return health;
	}
	SI16 Stamina( void ) const
	{
		return stamina;
	}
	SI16 Mana( void ) const
	{
		return mana;
	}
	SI32 Tithing( void ) const
	{
		return tithing;
	}

	void Action( UI16 newVal )
	{
		action = newVal;
	}
	void Delay( R32 newVal )
	{
		delay = newVal;
	}
	void DamageDelay( R32 newVal )
	{
		damageDelay = newVal;
	}
	void RecoveryDelay( R32 newVal )
	{
		recoveryDelay = newVal;
	}
	void Health( SI16 newVal )
	{
		health = newVal;
	}
	void Stamina( SI16 newVal )
	{
		stamina	= newVal;
	}
	void Mana( SI16 newVal )
	{
		mana = newVal;
	}
	SI16 BaseDmg( void ) const
	{
		return baseDmg;
	}
	Reag_st	Reagants( void ) const
	{
		return reags;
	}
	Reag_st *ReagantsPtr( void )
	{
		return &reags;
	}
	const std::string Mantra( void ) const
	{
		return mantra;
	}
	const std::string StringToSay( void ) const
	{
		return strToSay;
	}
	SI16 ScrollLow( void ) const
	{
		return sclo;
	}
	SI16 ScrollHigh( void ) const
	{
		return schi;
	}
	SI16 HighSkill( void ) const
	{
		return hiskill;
	}
	SI16 LowSkill( void ) const
	{
		return loskill;
	}
	UI08 Circle( void ) const
	{
		return circle;
	}
	bool Enabled( void ) const
	{
		return enabled;
	}
	bool RequireTarget( void ) const
	{
		return ( flags & 0x01 ) == 0x01;
	}
	bool RequireItemTarget( void ) const
	{
		return ( flags & 0x02 ) == 0x02;
	}
	bool RequireLocTarget( void ) const
	{
		return ( flags & 0x04 ) == 0x04;
	}
	bool RequireCharTarget( void ) const
	{
		return ( flags & 0x08 ) == 0x08;
	}
	bool RequireNoTarget( void ) const
	{
		return ( flags & 0x00 ) == 0x00;
	}
	bool TravelSpell( void ) const
	{
		return ( flags & 0x10 ) == 0x10;
	}
	bool FieldSpell( void ) const
	{
		return ( flags & 0x20 ) == 0x20;
	}
	bool SpellReflectable( void ) const
	{
		return ( flags & 0x40 ) == 0x40;
	}
	bool AggressiveSpell( void ) const
	{
		return ( flags & 0x80 ) == 0x80;
	}
	bool Resistable( void ) const
	{
		return ( flags & 0x100 ) == 0x100;
	}
	void Mantra( const std::string& toPut )
	{
		mantra = toPut;
	}
	void StringToSay( const std::string& toPut )
	{
		strToSay = toPut;
	}
	void BaseDmg( SI16 newVal )
	{
		baseDmg = newVal;
	}
	void ScrollLow( SI16 newVal )
	{
		sclo = newVal;
	}
	void ScrollHigh( SI16 newVal )
	{
		schi = newVal;
	}
	void HighSkill( SI16 newVal )
	{
		hiskill	= newVal;
	}
	void LowSkill( SI16 newVal )
	{
		loskill	= newVal;
	}
	void Circle( UI08 newVal )
	{
		circle = newVal;
	}
	void Enabled( bool newVal )
	{
		enabled	= newVal;
	}
	void Flags( UI16 newVal )
	{
		flags = newVal;
	}
	void Tithing( SI32 newVal )
	{
		tithing	= newVal;
	}
	UI16 JSScript( void )
	{
		return jsScript;
	}
	void JSScript( UI16 scpAssign )
	{
		jsScript = scpAssign;
	}
	UI16	Effect( void )const		{	return effect;	}
	void	Effect( UI16 newVal )	{	effect = newVal; }
	CMagicMove MoveEffect( void ) const
	{
		return moveEffect;
	}
	CMagicStat StaticEffect( void ) const
	{
		return staticEffect;
	}
	CMagicMove * MoveEffectPtr( void )
	{
		return &moveEffect;
	}
	CMagicStat * StaticEffectPtr( void )
	{
		return &staticEffect;
	}
};

#define MAGIC_CHARTARG_LIST CChar *, CChar *, CChar *, SI08
#define MAGIC_ITEMTARG_LIST CSocket *, CChar *, CItem *, SI08
#define MAGIC_LOCATION_LIST CSocket *, CChar *, SI16, SI16, SI08, SI08
#define MAGIC_FIELD_LIST CSocket *, CChar *, UI08, SI16, SI16, SI08, SI08
#define MAGIC_AREA_STUB_LIST CChar *, CChar *, SI08, SI08
#define MAGIC_NOTARG_LIST CSocket *, CChar *, SI08

#define MAGIC_TEST_LIST CSocket *, CChar *, CChar *, CChar *, SI08

#define MAGIC_TESTFUNC	bool (*)( MAGIC_TEST_LIST )
#define MAGIC_CHARFUNC	bool (*)( MAGIC_CHARTARG_LIST )
#define MAGIC_ITEMFUNC  bool (*)( MAGIC_ITEMTARG_LIST )
#define MAGIC_LOCFUNC   bool (*)( MAGIC_LOCATION_LIST )
#define MAGIC_FIELDFUNC bool (*)( MAGIC_FIELD_LIST )
#define MAGIC_NOFUNC    bool (*)( MAGIC_NOTARG_LIST )

#define MAGIC_STUBFUNC  void (*)( MAGIC_AREA_STUB_LIST )

#define MAG_TESTHANDLER( name ) extern bool name ( MAGIC_TEST_LIST )
#define MAG_CHARHANDLER( name ) extern bool name ( MAGIC_CHARTARG_LIST )
#define MAG_ITEMHANDLER( name ) extern bool name ( MAGIC_ITEMTARG_LIST )
#define MAG_LOCHANDLER(  name ) extern bool name ( MAGIC_LOCATION_LIST )
#define MAG_FIELDHANDLER( name ) extern bool name ( MAGIC_FIELD_LIST )
#define MAG_NOHANDLER( name ) extern bool name ( MAGIC_NOTARG_LIST )
#define MAG_AREASTUB( name ) extern void name ( MAGIC_AREA_STUB_LIST )

#define MAGIC_DEFN bool (*) (...)

struct MagicTable_st
{
	SI32			spell_name;
	bool			( *mag_extra ) (...);	// extra data - see above
};

bool AreaAffectSpell( CSocket *sock, CChar *caster, void ( *trgFunc )( MAGIC_AREA_STUB_LIST ), SI08 curSpell );
bool DiamondSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length );
bool FieldSpell( CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 fieldDir, SI08 spellNum );
bool FloodSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length );
bool SquareSpell( CSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length );
SI16 CalcSpellDamageMod( CChar *caster, CChar *target, SI16 baseDamage, bool spellResisted );

extern const MagicTable_st						magic_table[];

MAG_CHARHANDLER( splClumsy );
MAG_NOHANDLER( splCreateFood );
MAG_CHARHANDLER( splFeeblemind );
MAG_CHARHANDLER( splHeal );
MAG_CHARHANDLER( splMagicArrow );
MAG_CHARHANDLER( splNightSight );
MAG_CHARHANDLER( splReactiveArmor );
MAG_CHARHANDLER( splWeaken );
MAG_CHARHANDLER( splAgility );
MAG_CHARHANDLER( splCunning );
MAG_CHARHANDLER( splCure );
MAG_CHARHANDLER( splHarm );
MAG_ITEMHANDLER( splMagicTrap );
MAG_ITEMHANDLER( splMagicUntrap );
MAG_CHARHANDLER( splProtection );
MAG_CHARHANDLER( splStrength );
MAG_CHARHANDLER( splBless );
MAG_CHARHANDLER( splFireball );
MAG_ITEMHANDLER( splMagicLock );
MAG_CHARHANDLER( splPoison );
MAG_ITEMHANDLER( splTelekinesis );
MAG_LOCHANDLER( splTeleport );
MAG_ITEMHANDLER( splUnlock );
MAG_FIELDHANDLER( splWallOfStone );
MAG_TESTHANDLER( splArchCure );
MAG_TESTHANDLER( splArchProtection );
MAG_CHARHANDLER( splCurse );
MAG_FIELDHANDLER( splFireField );
MAG_CHARHANDLER( splGreaterHeal );
MAG_CHARHANDLER( splLightning );
MAG_CHARHANDLER( splManaDrain );
MAG_ITEMHANDLER( splRecall );
MAG_LOCHANDLER( splBladeSpirits );
MAG_NOHANDLER( splDispelField );
MAG_NOHANDLER( splIncognito );
MAG_NOHANDLER( splMagicReflection );
MAG_CHARHANDLER( splMindBlast );
MAG_CHARHANDLER( splParalyze );
MAG_FIELDHANDLER( splPoisonField );
MAG_LOCHANDLER( splSummonCreature );
MAG_CHARHANDLER( splDispel );
MAG_CHARHANDLER( splEnergyBolt );
MAG_CHARHANDLER( splExplosion );
MAG_CHARHANDLER( splInvisibility );
MAG_ITEMHANDLER( splMark );
MAG_TESTHANDLER( splMassCurse );
MAG_FIELDHANDLER( splParalyzeField );
MAG_LOCHANDLER( splReveal );
MAG_TESTHANDLER( splChainLightning );
MAG_FIELDHANDLER( splEnergyField );
MAG_CHARHANDLER( splFlameStrike );
MAG_ITEMHANDLER( splGateTravel );
MAG_CHARHANDLER( splManaVampire );
MAG_TESTHANDLER( splMassDispel );
MAG_TESTHANDLER( splMeteorSwarm );
MAG_NOHANDLER( splPolymorph );
MAG_NOHANDLER( splEarthquake );
MAG_LOCHANDLER( splEnergyVortex );
MAG_CHARHANDLER( splResurrection );
MAG_NOHANDLER( splSummonAir );
MAG_NOHANDLER( splSummonDaemon );
MAG_NOHANDLER( splSummonEarth );
MAG_NOHANDLER( splSummonFire );
MAG_NOHANDLER( splSummonWater );
MAG_NOHANDLER( splRandom );
MAG_CHARHANDLER( splNecro1 );
MAG_NOHANDLER( splNecro2 );
MAG_CHARHANDLER( splNecro3 );
MAG_CHARHANDLER( splNecro4 );
MAG_CHARHANDLER( splNecro5 );

// All command_ functions take an SI32 value of the player that triggered the command.

#endif

