#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H
#include "uox3.h"
#include "cmath"

struct r2Data
{
	std::string tag;
	std::string data;
	r2Data( char *t, char *d ) : tag( t ), data( d ) { }
	r2Data() : tag( "" ), data( "" ) { }
};

class CMagicMove
{
private:
	UI16	effect;
	UI08	speed;
	UI08	loop;
	UI08	explode;
public:
	CMagicMove() : effect( INVALIDID ), speed( 0 ), loop( 0 ), explode( 0 ) {}

	UI16	Effect( void ) const		{ return effect; }
	UI08	Speed( void ) const			{ return speed; }
	UI08	Loop( void ) const			{ return loop; }
	UI08	Explode( void ) const		{ return explode; }
	void	Effect( UI08 p1, UI08 p2 )	{ effect = (UI16)((p1<<8) + p2); }
	void	Speed( UI08 newVal )		{ speed = newVal; }
	void	Loop( UI08 newVal )			{ loop = newVal; }
	void	Explode( UI08 newVal )		{ explode = newVal; }
};

class CMagicStat
{
private:
	UI16	effect;
	UI08	speed;
	UI08	loop;
public:
			CMagicStat() : effect( INVALIDID ), speed( 0xFF ), loop( 0xFF ) { }
	UI16	Effect( void ) const		{ return effect; }
	UI08	Speed( void ) const			{ return speed; }
	UI08	Loop( void ) const			{ return loop; }
	void	Effect( UI08 p1, UI08 p2 )	{ effect = (UI16)((p1<<8) + p2); }
	void	Speed( UI08 newVal )		{ speed = newVal; }
	void	Loop( UI08 newVal )			{ loop = newVal; }
};

class CMagicSound
{
private:
	UI16	effect;
public:
	CMagicSound() : effect( INVALIDID )	{}
	UI16	Effect( void ) const		{ return effect; }
	void	Effect( UI08 p1, UI08 p2 )	{ effect = (UI16)((p1<<8) + p2); }
};

struct creat_st
{
private:
	SI32 basesound;
	UI08 soundflag;
	UI08 who_am_i; 
	SI32 icon;
public:
			creat_st() : basesound( 0 ), soundflag( 0 ), who_am_i( 0 ), icon( 0 ) { }
	SI32	BaseSound( void )		{	return basesound;	}
	SI32	Icon( void )			{	return icon;		}
	UI08	SoundFlag( void )		{	return soundflag;	}
	UI08	WhoAmI( void )			{	return who_am_i;	}

	void	BaseSound( SI32 value )	{	basesound = value;	}
	void	Icon( SI32 value )		{	icon = value;		}
	void	SoundFlag( UI08 value )	{	soundflag = value;	}
	void	WhoAmI( UI08 value )	{	who_am_i = value;	}

	bool	IsAnimal( void )		{	return ( ( who_am_i & 4 ) == 4 );	}
	bool	AntiBlink( void )		{	return ( ( who_am_i & 2 ) == 2 );	}
	bool	CanFly( void )			{	return ( ( who_am_i & 1 ) == 1 );	}
	bool	IsWater( void )			{	return ( ( who_am_i & 8 ) == 8 );	}
	void	IsAnimal( bool value )
	{
		if( value )
			who_am_i |= 4;
		else
			who_am_i &= 0xFB;
	}
	void	AntiBlink( bool value )
	{
		if( value )
			who_am_i |= 2;
		else
			who_am_i &= 0xFD;
	}
	void	CanFly( bool value )
	{
		if( value )
			who_am_i |= 1;
		else
			who_am_i &= 0xFE;
	}
	void	IsWater( bool value )
	{
		if( value )
			who_am_i |= 8;
		else
			who_am_i &= 0xF7;
	}
};

struct versionrecord
{
	SI32 file;
	SI32 block;
	SI32 filepos;
	SI32 length;
	SI32 unknown;
} PACK_NEEDED;

struct staticrecord
{
	UI16 itemid;
	// short int extra; // Unknown yet --Zippy unknown thus not used thus taking up mem.
	UI08 xoff;
	UI08 yoff;
	SI08 zoff;
	char align;	// force word alignment by hand to avoid bus errors - fur
} PACK_NEEDED;

struct map_st {
	UI16 id;
	SI08 z;
};

struct st_multiidx
{
	SI32 start;
	SI32 length;
	SI32 unknown;
} PACK_NEEDED;
struct st_multi
{
	SI32 visible;  // this needs to be first so it is word aligned to avoid bus errors - fur
	UI16 tile;
	SI16 x;
	SI16 y;
	SI08 z;
	SI08 empty;
} PACK_NEEDED;

struct location_st
{
	SI16 x1;
	SI16 y1;
	SI16 x2;
	SI16 y2;
	UI08 region;
};

struct logout_st//Instalog
{
	SI16 x1;
	SI16 y1;
	SI16 x2;
	SI16 y2;
};

struct advance_st
{
	UI16 base;
	UI16 success;
	UI16 failure;
};
struct skill_st
{
	UI16 strength;
	UI16 dexterity;
	UI16 intelligence;
	char madeword[50];
	std::vector< advance_st > advancement;
};

struct title_st // For custom titles
{
	char fame[50];
	char skill[50];
	char prowess[50];
};

// Scriptable Murder Tags - Zane

struct MurderPair
{
	SI16 loBound;
	std::string toDisplay;
	MurderPair() : loBound( 0 ) { }
	MurderPair( SI16 lB, const char *toDisp ) : loBound( lB ) { toDisplay = toDisp; }
};
struct reag_st
{
	UI08 ginseng;
	UI08 moss;
	UI08 drake;
	UI08 pearl;
	UI08 silk;
	UI08 ash;
	UI08 shade;
	UI08 garlic;
	reag_st() : ginseng( 0 ), moss( 0 ), drake( 0 ), pearl( 0 ), silk( 0 ), ash( 0 ), shade( 0 ), garlic( 0 ) { }
};

typedef const char *CLPSTR;

class SpellInfo
{
private:
	SI16 mana, stamina, health;
	SI32 action, delay;

	reag_st reags;
	char mantra[25];
	char strToSay[100]; // string visualized with targ. system
	bool enabled;
	UI08 circle;
	UI16 flags;
	CMagicMove	moveEffect;
	CMagicSound soundEffect;
	CMagicStat	staticEffect;
	SI16 hiskill, loskill, sclo, schi;
public:
	SpellInfo() : enabled( false ), mana( 0 ), stamina( 0 ), health( 0 ), action( 0 ), delay( 0 ), circle( 1 ), flags( 0 ), hiskill( 0 ), loskill( 0 ), sclo( 0 ), schi( 0 )
	{
	}

	SI32	Action( void ) const			{ return action;					}
	SI32	Delay( void ) const				{ return delay;						}
	SI16	Health( void ) const			{ return health;					}
	SI16	Stamina( void ) const			{ return stamina;					}
	SI16	Mana( void ) const				{ return mana;						}

	void	Action( SI32 newVal )			{ action = newVal;					}
	void	Delay( SI32 newVal )			{ delay = newVal;					}
	void	Health( SI16 newVal )			{ health = newVal;					}
	void	Stamina( SI16 newVal )			{ stamina = newVal;					}
	void	Mana( SI16 newVal )				{ mana = newVal;					}

	reag_st	Reagants( void ) const			{ return reags;						}
	reag_st *ReagantsPtr( void )			{ return &reags;					}
	CLPSTR	Mantra( void ) const			{ return mantra;					}
	CLPSTR	StringToSay( void ) const		{ return strToSay;					}
	SI16	ScrollLow( void ) const			{ return sclo;						}
	SI16	ScrollHigh( void ) const		{ return schi;						}
	SI16	HighSkill( void ) const			{ return hiskill;					}
	SI16	LowSkill( void ) const			{ return loskill;					}
	UI08	Circle( void ) const			{ return circle;					}
	bool	Enabled( void ) const			{ return enabled;					}
	bool	RequireTarget( void ) const		{ return (flags&0x01) == 0x01;		}
	bool	RequireItemTarget( void ) const { return (flags&0x02) == 0x02;		}
	bool	RequireLocTarget( void ) const	{ return (flags&0x04) == 0x04;		}
	bool	RequireCharTarget( void ) const { return (flags&0x08) == 0x08;		}
	bool	TravelSpell( void ) const		{ return (flags&0x10) == 0x10;		}
	bool	FieldSpell( void ) const		{ return (flags&0x20) == 0x20;		}
	bool	SpellReflectable( void ) const	{ return (flags&0x40) == 0x40;		}
	bool	AggressiveSpell( void ) const	{ return (flags&0x80) == 0x80;		}
	bool	Resistable( void ) const		{ return (flags&0x100) == 0x100;	}

	void	Mantra( CLPSTR toPut )			{ strcpy( mantra, toPut );			}
	void	StringToSay( CLPSTR toPut )		{ strcpy( strToSay, toPut );		}
	void	ScrollLow( SI16 newVal )		{ sclo = newVal;					}
	void	ScrollHigh( SI16 newVal )		{ schi = newVal;					}
	void	HighSkill( SI16 newVal )		{ hiskill = newVal;					}
	void	LowSkill( SI16 newVal )			{ loskill = newVal;					}
	void	Circle( UI08 newVal )			{ circle = newVal;					}
	void	Enabled( bool newVal )			{ enabled = newVal;					}
	void	Flags( UI08 val1, UI08 val2 )	{ flags = (UI16)((val1<<8) + val2);	}

	CMagicMove		MoveEffect( void ) const	{ return moveEffect;			}
	CMagicSound		SoundEffect( void ) const	{ return soundEffect;			}
	CMagicStat		StaticEffect( void ) const	{ return staticEffect;			}
	CMagicMove *	MoveEffectPtr( void )		{ return &moveEffect;			}
	CMagicSound *	SoundEffectPtr( void )		{ return &soundEffect;			}
	CMagicStat *	StaticEffectPtr( void )		{ return &staticEffect;			}
};

struct commandLevel_st
{
	char name[17];		// name of level
	UI08 commandLevel;	// upper limit of level
	UI16 defaultPriv;	// default privs associated with it
	UI16 nickColour;	// colour of a person's name
	UI16 allSkillVals;	// if 0, skills left same, if not, all skills set to this value
	UI16 targBody;		// target body value
	UI16 bodyColour;	// target body colour
	bool stripOff;		// strips off hair, beard and clothes
};

struct miningData
{
	UI16 colour;		// colour of the ore, for colour of ingot
	UI16 minSkill;		// minimum skill needed to make the ingot
	std::string name;		// name of the ingot: no need to be fixed, as we're loading it dynamically
	bool foreign;		// if not iron, then it can print out that it's a stranger ore when failing
	int makemenu;		// the makemenu required for making with
	UI08 minAmount;		// min number of ingots to load anything
};

struct vector2D
{
	R32 x;
	R32 y;
	vector2D() { }
	vector2D( R32 X, R32 Y ) : x(X), y(Y) { }
};

struct vector3D
{
	SI32 x;
	SI32 y;
	SI08 z;
	vector3D() { }
	vector3D( SI32 X, SI32 Y, SI08 Z ) : x(X), y(Y), z(Z) { }
};

inline bool operator== (vector3D const &a, vector3D const &b )
{
	return ( ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) );
}

inline bool operator< (vector3D const &a, vector3D const &b )
{
	return ( ( a.x < b.x ) && ( a.y < b.y ) && ( a.z < b.z ) );
}

struct line2D
{
	vector2D loc;
	vector2D dir;
	line2D() { }
	line2D( vector2D LOC, vector2D DIR ) : loc(LOC), dir(DIR) { }
	vector2D CollideLines2D( line2D toCollide ) const;
};

inline vector2D line2D::CollideLines2D( line2D toCollide ) const
{
	if( ( ( dir.x == 0 ) && ( toCollide.dir.x == 0 ) ) ||
		( ( dir.y == 0 ) && ( toCollide.dir.y == 0 ) ) )
		return vector2D( -1.0f, -1.0f ); // error, parallel or invalid lines
	if( ( ( dir.x * toCollide.dir.y ) - ( toCollide.dir.x * dir.y ) ) == 0 )
		return vector2D( -1.0f, -1.0f ); // error, parallel lines
		
	R32 t = 0.0f; // parameter of toCollide-line
	// linear evaluation of extended 2x2 matrix
	t = ( ( ( ( loc.x - toCollide.loc.x ) * (- dir.y) ) + ( dir.x * ( loc.y - toCollide.loc.y ) ) ) /
		( ( dir.x * toCollide.dir.y ) - ( toCollide.dir.x * dir.y ) ) );

	return vector2D( ( toCollide.loc.x + t * toCollide.dir.x ), ( toCollide.loc.y + t * toCollide.dir.y ) );
}

struct line3D
{
	vector3D loc;
	vector3D dir;
	line3D() { }
	line3D( vector3D LOC, vector3D DIR ) : loc(LOC), dir(DIR) { }
	R32 dzInDirectionX( void ) const;
	R32 dzInDirectionY( void ) const;
	line2D Projection2D( void ) const;
};

inline R32 line3D::dzInDirectionX( void ) const
{
	if( dir.x == 0 )
		return (R32)( dir.z );
	else
		return (R32)( (R32)( dir.z ) / (R32)( dir.x ) );
}

inline R32 line3D::dzInDirectionY( void ) const
{
	if( dir.y == 0 )
		return (R32)( dir.z );
	else
		return (R32)( (R32)( dir.z ) / (R32)( dir.y ) );
}

inline line2D line3D::Projection2D( void ) const
{
	if( ( dir.x == 0 ) && ( dir.y == 0 ) )
		return line2D( vector2D( -1.0f, -1.0f ), vector2D( -1.0f, -1.0f ) );
	else
		return line2D( vector2D( (R32)loc.x, (R32)loc.y ), vector2D( (R32)dir.x, (R32)dir.y ) );
}

struct point3
{
	R32		x, y, z;
			point3() { }
			point3( UI16 X, UI16 Y, SI08 Z ) : x( X ), y( Y ), z( Z ) { }
			point3( R32 X, R32 Y, R32 Z ) : x( X ), y( Y ), z( Z ) { }
	void	Assign( UI16 X, UI16 Y, SI08 Z );
	void	Assign( R32 X, R32 Y, R32 Z );
	R32		Mag( void ) const;
	R32		MagSquared( void ) const;
	R32		Mag3D( void ) const;
	R32		MagSquared3D( void ) const;
	void	Normalize( void );
};

inline point3 operator+( point3 const &a, point3 const &b )
{
	return (point3( a.x + b.x, a.y + b.y, a.z + b.z ));
}

inline point3 operator-( point3 const &a, point3 const &b )
{
	return (point3( a.x - b.x, a.y - b.y, a.z - b.z ));
}
inline point3 operator*( point3 const &a, R32 const &b )
{
	return point3( a.x*b, a.y*b, a.z*b );
}
inline point3 operator*( R32 const &a, point3 const &b )
{
	return point3( a * b.x, a * b.y, a * b.z );
}
inline point3 operator/( point3 const &a, R32 const &b )
{
	R32 inv = 1.f / b;
	return point3( a.x * inv, a.y * inv, a.z * inv );
}
inline void point3::Assign( UI16 X, UI16 Y, SI08 Z )
{
	x = X;
	y = Y;
	z = Z;
}
inline void point3::Assign( R32 X, R32 Y, R32 Z )
{
	x = X;
	y = Y;
	z = Z;
}
inline R32 point3::Mag3D( void ) const
{
	return (R32)sqrt( x*x + y*y + z*z );
}
inline R32 point3::MagSquared3D( void ) const
{
	return ( x*x + y*y + z*z );
}

inline R32 point3::Mag( void ) const
{
	return (R32)sqrt( x*x + y*y );
}
inline R32 point3::MagSquared( void ) const
{
	return ( x*x + y*y );
}

inline void point3::Normalize( void )
{
	R32 foo = 1 / Mag3D();
	x *= foo;
	y *= foo;
	z *= foo;
}

// create.scp revisions
struct resAmountPair
{
	UI16 itemID;
	UI08 amountNeeded;
	UI16 colour;
	resAmountPair() { itemID = 0; amountNeeded = 1; colour = 0; };
};

struct resSkillReq
{
	UI08 skillNumber;
	UI16 minSkill;
	UI16 maxSkill;
};

struct createMenu
{
	std::vector< UI16 > itemEntries;
	std::vector< UI16 > menuEntries;
};
struct createMenuEntry
{
	UI16 targID;
	UI16 colour;
	std::string name;
	UI16 subMenu;
	createMenuEntry() : targID( 0 ), colour( 0 ), name( "" ), subMenu( 0 ) { }
};

struct createEntry
{
	UI16 colour;
	UI16 targID;
	UI16 soundPlayed;
	UI08 minRank;
	UI08 maxRank;
	std::string addItem;
	SI16 delay;
	UI16 spell;
	std::vector< resAmountPair > resourceNeeded;
	std::vector< resSkillReq > skillReqs;
	std::string name;
	createEntry() : colour( 0 ), targID( 0 ), soundPlayed( 0 ), minRank( 1 ), maxRank( 10 ), addItem( "" ), delay( 0 ), name( "" ) { }
	R32 AverageMinSkill( void ) 
	{ 
		R32 sum = 0;
		for( UI32 i = 0; i < skillReqs.size(); i++ )
			sum += skillReqs[i].minSkill;
		return sum / skillReqs.size();
	}
	R32 AverageMaxSkill( void ) 
	{ 
		R32 sum = 0;
		for( UI32 i = 0; i < skillReqs.size(); i++ )
			sum += skillReqs[i].maxSkill;
		return sum / skillReqs.size();
	}

};

struct TeleLocationEntry
{
	SI16 src[3];
	SI16 srcWorld;
	SI16 trg[3];
	UI08 trgWorld;
	TeleLocationEntry() 
	{ 
		src[0] = src[1] = src[2] = 0; 
		trg[0] = trg[1] = trg[2] = 0; 
		srcWorld = -1;	// -1 indicates all worlds
		trgWorld = 0;
	}
};

#endif
