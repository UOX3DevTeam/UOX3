#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H

const UI08 BIT_ANIMAL		=	2;
const UI08 BIT_ANTIBLINK	=	1;
const UI08 BIT_CANFLY		=	0;
const UI08 BIT_WATER		=	3;
const UI08 BIT_AMPHI		=	4;
const UI08 BIT_HUMAN		=	5;

class CCreatures
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying)
	//             1: birds .. only one "bird-shape" and zillions of sounds ...
	//             2: only 3 sounds ->  (attack,defence,dying)
	//             3: only 4 sounds ->   (attack-started,attack,defnce,dying)
	//             4: only 1 sound !!
	//
	// who_am_i bit # 1 creature can fly (must have the animations, so better not change)
	//              # 2 anti-blink: these creatures don't have animation #4, if not set creature will randomly disappear in battle
	//                              if you find a creature that blinks while fighting, set that bit
	//              # 3 animal-bit
	//              # 4 water creatures
	//				# 5 amphibians (water + land)
	//				# 6 human-bit
	//
	// icon: used for tracking, to set the appropriate icon
	////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UI16				soundList[SND_COUNT];
	std::bitset< 6 >	who_am_i;
	UI16				icon;
	UI16				mountID;
public:
	CCreatures() : icon( 0 ), mountID( 0 )
	{
		who_am_i.reset();
		memset( soundList, 0x00, SND_COUNT );
	}
	UI16	GetSound( monsterSound soundType ) const
	{
		return soundList[static_cast<UI08>(soundType)];
	}
	void	SetSound( monsterSound soundType, UI16 newVal )
	{
		soundList[static_cast<UI08>(soundType)] = newVal;
	}
	UI16	Icon( void ) const
	{
		return icon;
	}
	UI08	WhoAmI( void ) const
	{
		return static_cast<UI08>(who_am_i.to_ulong());
	}

	void	Icon( UI16 value )
	{
		icon = value;
	}

	UI16	MountID( void ) const
	{
		return mountID;
	}
	void	MountID( UI16 value )
	{
		mountID = value;
	}

	bool	IsAnimal( void ) const		{		return who_am_i.test( BIT_ANIMAL );		}
	bool	IsHuman( void ) const		{		return who_am_i.test( BIT_HUMAN );		}
	bool	AntiBlink( void ) const		{		return who_am_i.test( BIT_ANTIBLINK );	}
	bool	CanFly( void ) const		{		return who_am_i.test( BIT_CANFLY );		}
	bool	IsWater( void ) const		{		return who_am_i.test( BIT_WATER );		}
	bool	IsAmphibian( void ) const	{		return who_am_i.test( BIT_AMPHI );		}
	void	IsAnimal( bool value )		{		who_am_i.set( BIT_ANIMAL, value );		}
	void	IsHuman( bool value )		{		who_am_i.set( BIT_HUMAN, value );		}
	void	AntiBlink( bool value )		{		who_am_i.set( BIT_ANTIBLINK, value );	}
	void	CanFly( bool value )		{		who_am_i.set( BIT_CANFLY, value );		}
	void	IsWater( bool value )		{		who_am_i.set( BIT_WATER, value );		}
	void	IsAmphibian( bool value )	{		who_am_i.set( BIT_AMPHI, value );		}
};

struct point3
{
	R32		x, y, z;
	point3(): x( 0 ), y( 0 ), z( 0 )
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
	R64		Mag( void ) const;
	R32		MagSquared( void ) const;
	R64		Mag3D( void ) const;
	R32		MagSquared3D( void ) const;
	void	Normalize( void );
};

inline bool operator==( point3 const &a, point3 const &b )
{
	return (a.x == b.x && a.y == b.y && a.z == b.z );
}
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
inline R64 point3::Mag3D( void ) const
{
	return (R32)sqrt( x*x + y*y + z*z );
}
inline R32 point3::MagSquared3D( void ) const
{
	return ( x*x + y*y + z*z );
}

inline R64 point3::Mag( void ) const
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
	UI16 instanceID;
	GoPlaces_st() : x( -1 ), y( -1 ), z( -1 ), worldNum( 0 ), instanceID( 0 )
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
	CTeleLocationEntry() : srcWorld( 0xFF ), trgWorld( 0 )
	{
		src.Assign( 0, 0, ILLEGAL_Z );
		trg.Assign( 0, 0, ILLEGAL_Z );
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
	SI16 x1 = 0;
	SI16 y1 = 0;
	SI16 x2 = 0;
	SI16 y2 = 0;
	UI08 worldNum = 0;
};

struct advance_st
{
	UI16 base;
	UI08 success;
	UI08 failure;
	UI08 amtToGain;
	advance_st() : base( 0 ), success( 0 ), failure( 0 ), amtToGain( 1 )
	{
	}
};

struct TitlePair_st
{
	SI16 lowBound;
	std::string toDisplay;
	TitlePair_st() : lowBound( 0 ), toDisplay( "" )
	{
	}
	TitlePair_st( SI16 lB, const std::string& toDisp ) : lowBound( lB )
	{
		toDisplay = toDisp;
	}
};
#endif
