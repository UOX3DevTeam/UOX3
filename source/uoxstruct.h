#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H

namespace UOX
{

class CCreatures
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying, see uox.h)
    //             1: birds .. only one "bird-shape" and zillions of sounds ...
	//             2: only 3 sounds ->  (attack,defence,dying)    
	//             3: only 4 sounds ->   (attack-started,attack,defnce,dying)
	//             4: only 1 sound !!
	//
	// who_am_i bit # 1 creature can fly (must have the animations, so better not change)
	//              # 2 anti-blink: these creatures don't have animation #4, if not set creature will randomly disappear in battle
	//                              if you find a creature that blinks while fighting, set that bit
	//              # 3 animal-bit (currently not used/set)
	//              # 4 water creatures (currently not used/set)
	// icon: used for tracking, to set the appropriate icon
	////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UI16 basesound;
	UI08 soundflag;
	UI08 who_am_i; 
	UI16 icon;
public:
			CCreatures() : basesound( 0 ), soundflag( 0 ), who_am_i( 0 ), icon( 0 )
			{
			}
	UI16	BaseSound( void ) const
	{
		return basesound;
	}
	UI16	Icon( void ) const
	{
		return icon;
	}
	UI08	SoundFlag( void ) const
	{
		return soundflag;
	}
	UI08	WhoAmI( void ) const
	{
		return who_am_i;
	}

	void	BaseSound( UI16 value )
	{
		basesound = value;
	}
	void	Icon( UI16 value )
	{
		icon = value;
	}
	void	SoundFlag( UI08 value )
	{
		soundflag = value;
	}
	void	WhoAmI( UI08 value )
	{
		who_am_i = value;
	}

	bool	IsAnimal( void ) const
	{
		return ( ( who_am_i & 4 ) == 4 );
	}
	bool	AntiBlink( void ) const
	{
		return ( ( who_am_i & 2 ) == 2 );
	}
	bool	CanFly( void ) const
	{
		return ( ( who_am_i & 1 ) == 1 );
	}
	bool	IsWater( void ) const
	{
		return ( ( who_am_i & 8 ) == 8 );
	}
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

struct vector2D
{
	R32 x;
	R32 y;
	vector2D()
	{
	}
	vector2D( R32 X, R32 Y ) : x(X), y(Y)
	{
	}
};

struct vector3D
{
	SI32 x;
	SI32 y;
	SI08 z;
	vector3D()
	{
	}
	vector3D( SI32 X, SI32 Y, SI08 Z ) : x(X), y(Y), z(Z)
	{
	}
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
	line2D()
	{
	}
	line2D( vector2D LOC, vector2D DIR ) : loc(LOC), dir(DIR)
	{
	}
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
	line3D()
	{
	}
	line3D( vector3D LOC, vector3D DIR ) : loc(LOC), dir(DIR)
	{
	}
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
			point3()
			{
			}
			point3( UI16 X, UI16 Y, SI08 Z ) : x( X ), y( Y ), z( Z )
			{
			}
			point3( R32 X, R32 Y, R32 Z ) : x( X ), y( Y ), z( Z )
			{
			}
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
	resAmountPair()
	{
		itemID = 0; amountNeeded = 1; colour = 0;
	};
};

struct resSkillReq
{
	UI08 skillNumber;
	UI16 minSkill;
	UI16 maxSkill;
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
	createEntry() : colour( 0 ), targID( 0 ), soundPlayed( 0 ), minRank( 1 ), maxRank( 10 ), addItem( "" ), delay( 0 ), name( "" )
	{
	}
	R32 AverageMinSkill( void ) 
	{ 
		R32 sum = 0;
		for( UI32 i = 0; i < skillReqs.size(); ++i )
			sum += skillReqs[i].minSkill;
		return sum / skillReqs.size();
	}
	R32 AverageMaxSkill( void ) 
	{ 
		R32 sum = 0;
		for( UI32 i = 0; i < skillReqs.size(); ++i )
			sum += skillReqs[i].maxSkill;
		return sum / skillReqs.size();
	}

};

struct UOXFileWrapper
{
	FILE *mWrap;
};

struct GoPlaces_st
{
	SI16 x;
	SI16 y;
	SI08 z;
	UI08 worldNum;
	GoPlaces_st() : x( -1 ), y( -1 ), z( -1 ), worldNum( 0 )
	{
	}
};

// Teleport Locations
class CTeleLocationEntry
{
private:
	point3	src;
	UI08	srcWorld;
	point3	trg;
	UI08	trgWorld;
public:
	CTeleLocationEntry()
	{
		src.Assign( 0, 0, ILLEGAL_Z );
		trg.Assign( 0, 0, ILLEGAL_Z );
		srcWorld = 0xFF;	// 0xFF indicates all worlds
		trgWorld = 0;
	}
	point3 SourceLocation( void ) const
	{
		return src;
	}
	void SourceLocation( UI16 x, UI16 y, SI08 z )
	{
		src.Assign( x, y, z );
	}
	UI08 SourceWorld( void ) const
	{
		return srcWorld;
	}
	void SourceWorld( UI08 newVal )
	{
		srcWorld = newVal;
	}
	point3 TargetLocation( void ) const
	{
		return trg;
	}
	void TargetLocation( UI16 x, UI16 y, SI08 z )
	{
		trg.Assign( x, y, z );
	}
	UI08 TargetWorld( void ) const
	{
		return trgWorld;
	}
	void TargetWorld( UI08 newVal )
	{
		trgWorld = newVal;
	}
};

// Instalog Locations
struct LogoutLocationEntry
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
	std::string madeword;
	std::vector< advance_st > advancement;
};

// Custom Titles
struct title_st
{
	std::string fame;
	std::string skill;
};

// Scriptable Murder Tags - Zane
struct MurderPair
{
	SI16 loBound;
	std::string toDisplay;
	MurderPair() : loBound( 0 )
	{
	}
	MurderPair( SI16 lB, const std::string toDisp ) : loBound( lB )
	{
		toDisplay = toDisp;
	}
};

struct ProwessTitle
{
	SI16 minBaseSkill;
	std::string toDisplay;
	ProwessTitle() : minBaseSkill( 0 )
	{
	}
	ProwessTitle( SI16 mBS, const std::string toDisp ) : minBaseSkill( mBS )
	{
		toDisplay = toDisp;
	}
};

}
#endif
