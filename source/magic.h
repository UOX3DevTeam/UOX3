#ifndef __MAGIC_H
#define __MAGIC_H

#define MAGIC_CHARTARG_LIST cSocket *, CChar *, CChar *, CChar *
#define MAGIC_ITEMTARG_LIST cSocket *, CChar *, CItem *
#define MAGIC_LOCATION_LIST cSocket *, CChar *, SI16, SI16, SI08
#define MAGIC_FIELD_LIST cSocket *, CChar *, UI08, SI16, SI16, SI08
#define MAGIC_AREA_STUB_LIST CChar *, CChar *
#define MAGIC_NOTARG_LIST cSocket *, CChar *

#define MAGIC_CHARFUNC	bool (*)( MAGIC_CHARTARG_LIST )
#define MAGIC_ITEMFUNC  bool (*)( MAGIC_ITEMTARG_LIST )
#define MAGIC_LOCFUNC   bool (*)( MAGIC_LOCATION_LIST )
#define MAGIC_FIELDFUNC bool (*)( MAGIC_FIELD_LIST )
#define MAGIC_NOFUNC    bool (*)( MAGIC_NOTARG_LIST )

#define MAGIC_STUBFUNC  void (*)( MAGIC_AREA_STUB_LIST )

#define MAG_CHARHANDLER( name ) extern bool name ( MAGIC_CHARTARG_LIST )
#define MAG_ITEMHANDLER( name ) extern bool name ( MAGIC_ITEMTARG_LIST )
#define MAG_LOCHANDLER(  name ) extern bool name ( MAGIC_LOCATION_LIST )
#define MAG_FIELDHANDLER( name ) extern bool name ( MAGIC_FIELD_LIST )
#define MAG_NOHANDLER( name ) extern bool name ( MAGIC_NOTARG_LIST )
#define MAG_AREASTUB( name ) extern void name ( MAGIC_AREA_STUB_LIST )

#define MAGIC_DEFN bool (*) ()

struct MagicTable_s 
{
	int				spell_name;
	bool			(*mag_extra) ();	// extra data - see above
};

struct jsMagicTable_st
{
	std::string		spellName;
	cScript *		spellObj;
};

bool AreaAffectSpell( cSocket *sock, CChar *caster, void (*trgFunc)( MAGIC_AREA_STUB_LIST ) );
bool DiamondSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length );
bool FieldSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 fieldDir );
bool FloodSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length );
bool SquareSpell( cSocket *sock, CChar *caster, UI16 id, SI16 x, SI16 y, SI08 z, UI08 length );

extern MagicTable_s						magic_table[];
extern std::vector< jsMagicTable_st >	jsMagicTable;

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
MAG_LOCHANDLER( splArchCure );
MAG_LOCHANDLER( splArchProtection );
MAG_CHARHANDLER( splCurse );
MAG_FIELDHANDLER( splFireField );
MAG_CHARHANDLER( splGreaterHeal );
MAG_CHARHANDLER( splLightning );
MAG_CHARHANDLER( splManaDrain );
MAG_ITEMHANDLER( splRecall );
MAG_LOCHANDLER( splBladeSpirits );
MAG_LOCHANDLER( splDispelField );
MAG_NOHANDLER( splIncognito );
MAG_NOHANDLER( splMagicReflection );
MAG_CHARHANDLER( splMindBlast );
MAG_CHARHANDLER( splParalyze );
MAG_FIELDHANDLER( splPoisonField );
MAG_NOHANDLER( splSummonCreature );
MAG_CHARHANDLER( splDispel );
MAG_CHARHANDLER( splEnergyBolt );
MAG_CHARHANDLER( splExplosion );
MAG_CHARHANDLER( splInvisibility );
MAG_ITEMHANDLER( splMark );
MAG_LOCHANDLER( splMassCurse );
MAG_FIELDHANDLER( splParalyzeField );
MAG_LOCHANDLER( splReveal );
MAG_LOCHANDLER( splChainLightning );
MAG_FIELDHANDLER( splEnergyField );
MAG_CHARHANDLER( splFlameStrike );
MAG_ITEMHANDLER( splGateTravel );
MAG_CHARHANDLER( splManaVampire );
MAG_LOCHANDLER( splMassDispel );
MAG_LOCHANDLER( splMeteorSwarm );
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

// All command_ functions take an int value of the player that triggered the command.

#endif

