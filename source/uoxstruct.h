#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H

const UI08 BIT_CANFLY		=	0;
const UI08 BIT_ANTIBLINK	=	1;
const UI08 BIT_ANIMAL		=	2;
const UI08 BIT_WATER		=	3;
const UI08 BIT_AMPHI		=	4;
const UI08 BIT_HUMAN		=	5;

class CCreatures
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying)
	//             1: birds .. only one "bird-shape" and zillions of sounds ...
	//             2: only 3 sounds ->  (attack, defence, dying)
	//             3: only 4 sounds ->   (attack-started, attack, defence, dying)
	//             4: only 1 sound !!
	//
	// who_am_i bit # 1 creature can fly (must have the animations, so better not change)
	//              # (OUTDATED) 2 anti-blink: these creatures don't have animation #4, if not set creature will randomly disappear in battle
	//                              if you find a creature that blinks while fighting, set that bit
	//              # 3 animal-bit
	//              # 4 water creatures
	//				# 5 amphibians (water + land)
	//				# 6 human-bit
	//
	// icon: used for tracking, to set the appropriate icon
	////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UI16				creatureId;
	UI16				soundList[SND_COUNT];
	std::bitset<6>		who_am_i;
	UI16				icon;
	UI16				mountId;
	struct CreatureAnim_st
	{
		UI08 animId;
		UI08 animLength;
		CreatureAnim_st() : animId( 0 ), animLength( 0 )
		{
		}
	};

	CreatureAnim_st castAnimArea;
	CreatureAnim_st castAnimTarget;
	CreatureAnim_st castAnim3;
	CreatureAnim_st attackAnim1;
	CreatureAnim_st attackAnim2;
	CreatureAnim_st attackAnim3;

public:
	CCreatures() : creatureId( 0 ), icon( 0 ), mountId( 0 )
	{
		who_am_i.reset();
		memset( soundList, 0x00, SND_COUNT * sizeof( soundList[0] ));
	}
	UI16 GetSound( monsterSound soundType ) const
	{
		return soundList[static_cast<UI08>( soundType )];
	}
	void SetSound( monsterSound soundType, UI16 newVal )
	{
		soundList[static_cast<UI08>( soundType )] = newVal;
	}
	UI16 Icon( void ) const
	{
		return icon;
	}
	UI08 WhoAmI( void ) const
	{
		return static_cast<UI08>( who_am_i.to_ulong() );
	}

	void Icon( UI16 value )
	{
		icon = value;
	}

	UI16 MountId( void ) const
	{
		return mountId;
	}
	void MountId( UI16 value )
	{
		mountId = value;
	}

	// Cast Animation (Area-based)
	UI08 CastAnimAreaId( void ) const
	{
		return castAnimArea.animId;
	}
	UI08 CastAnimAreaLength( void ) const
	{
		return castAnimArea.animLength;
	}
	void CastAnimArea( UI08 value, UI08 length )
	{
		castAnimArea.animId = value;
		castAnimArea.animLength = length;
	}

	// Cast Animation (Target-based)
	UI08 CastAnimTargetId( void ) const
	{
		return castAnimTarget.animId;
	}
	UI08 CastAnimTargetLength( void ) const
	{
		return castAnimTarget.animLength;
	}
	void CastAnimTarget( UI08 value, UI08 length )
	{
		castAnimTarget.animId = value;
		castAnimTarget.animLength = length;
	}

	// Cast Animation #3
	UI08 CastAnim3Id( void ) const
	{
		return castAnim3.animId;
	}
	UI08 CastAnim3Length( void ) const
	{
		return castAnim3.animLength;
	}
	void CastAnim3( UI08 value, UI08 length )
	{
		castAnim3.animId = value;
		castAnim3.animLength = length;
	}

	// Attack Animation #1
	UI08 AttackAnim1Id( void ) const
	{
		return attackAnim1.animId;
	}
	UI08 AttackAnim1Length( void ) const
	{
		return attackAnim1.animLength;
	}
	void AttackAnim1( UI08 value, UI08 length )
	{
		attackAnim1.animId = value;
		attackAnim1.animLength = length;
	}

	// Attack Animation #2
	UI08 AttackAnim2Id( void ) const
	{
		return attackAnim2.animId;
	}
	UI08 AttackAnim2Length( void ) const
	{
		return attackAnim2.animLength;
	}
	void AttackAnim2( UI08 value, UI08 length )
	{
		attackAnim2.animId = value;
		attackAnim2.animLength = length;
	}

	// Attack Animation #3
	UI08 AttackAnim3Id( void ) const
	{
		return attackAnim3.animId;
	}
	UI08 AttackAnim3Length( void ) const
	{
		return attackAnim3.animLength;
	}
	void AttackAnim3( UI08 value, UI08 length )
	{
		attackAnim3.animId = value;
		attackAnim3.animLength = length;
	}

	UI16 CreatureId( void ) const
	{
		return creatureId;
	}
	void CreatureId( UI16 value )
	{
		creatureId = value;
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

struct Point3_st
{
	R32		x, y, z;
	Point3_st(): x( 0 ), y( 0 ), z( 0 )
	{
	}
	Point3_st( UI16 X, UI16 Y, SI08 Z ) : x( X ), y( Y ), z( Z )
	{
	}
	Point3_st( R32 X, R32 Y, R32 Z ) : x( X ), y( Y ), z( Z )
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

inline bool operator == ( Point3_st const &a, Point3_st const &b )
{
	return ( a.x == b.x && a.y == b.y && a.z == b.z );
}
inline Point3_st operator + ( Point3_st const &a, Point3_st const &b )
{
	return ( Point3_st( a.x + b.x, a.y + b.y, a.z + b.z ));
}

inline Point3_st operator - ( Point3_st const &a, Point3_st const &b )
{
	return ( Point3_st( a.x - b.x, a.y - b.y, a.z - b.z ));
}
inline Point3_st operator * ( Point3_st const &a, R32 const &b )
{
	return Point3_st( a.x * b, a.y * b, a.z * b );
}
inline Point3_st operator * ( R32 const &a, Point3_st const &b )
{
	return Point3_st( a * b.x, a * b.y, a * b.z );
}
inline Point3_st operator / ( Point3_st const &a, R32 const &b )
{
	R32 inv = 1.f / b;
	return Point3_st( a.x * inv, a.y * inv, a.z * inv );
}
inline void Point3_st::Assign( UI16 X, UI16 Y, SI08 Z )
{
	x = X;
	y = Y;
	z = Z;
}
inline void Point3_st::Assign( R32 X, R32 Y, R32 Z )
{
	x = X;
	y = Y;
	z = Z;
}
inline R64 Point3_st::Mag3D( void ) const
{
	return static_cast<R32>( sqrt( x * x + y * y + z * z ));
}
inline R32 Point3_st::MagSquared3D( void ) const
{
	return ( x * x + y * y + z * z );
}

inline R64 Point3_st::Mag( void ) const
{
	return static_cast<R32>( sqrt( x * x + y * y ));
}
inline R32 Point3_st::MagSquared( void ) const
{
	return ( x * x + y * y );
}

inline void Point3_st::Normalize( void )
{
	R32 foo = 1 / Mag3D();
	x *= foo;
	y *= foo;
	z *= foo;
}

struct UOXFileWrapper_st
{
	FILE *mWrap;
};

struct GoPlaces_st
{
	SI16 x;
	SI16 y;
	SI08 z;
	UI08 worldNum;
	UI16 instanceId;
	GoPlaces_st() : x( -1 ), y( -1 ), z( -1 ), worldNum( 0 ), instanceId( 0 )
	{
	}
};

// Teleport Locations
class CTeleLocationEntry
{
private:
	Point3_st	src;
	UI08	srcWorld;
	Point3_st	trg;
	UI08	trgWorld;
public:
	CTeleLocationEntry() : srcWorld( 0xFF ), trgWorld( 0 )
	{
		src.Assign( 0, 0, ILLEGAL_Z );
		trg.Assign( 0, 0, ILLEGAL_Z );
	}
	Point3_st SourceLocation( void ) const
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
	Point3_st TargetLocation( void ) const
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
struct LogoutLocationEntry_st
{
	SI16 x1 = 0;
	SI16 y1 = 0;
	SI16 x2 = 0;
	SI16 y2 = 0;
	UI08 worldNum = 0;
	UI16 instanceId = 0;
};

// SOS Locations
struct SOSLocationEntry_st
{
	SI16 x1 = 0;
	SI16 y1 = 0;
	SI16 x2 = 0;
	SI16 y2 = 0;
	UI08 worldNum = 0;
	UI16 instanceId = 0;
};

struct Advance_st
{
	UI16 base;
	UI08 success;
	UI08 failure;
	UI08 amtToGain;
	Advance_st() : base( 0 ), success( 0 ), failure( 0 ), amtToGain( 1 )
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
