#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H

#include "uox3.h"


// these are the fixed record lengths as determined by the .mul files from OSI
// i made them longs because they are used to calculate offsets into the files
const UI32 VersionRecordSize = 20L;
const UI32 MultiRecordSize = 12L;
const UI32 LandRecordSize = 26L;
const UI32 TileRecordSize = 37L;
const UI32 MapRecordSize = 3L;
const UI32 MultiIndexRecordSize = 12L;
const UI32 StaticRecordSize = 7L;

struct r2Data
{
	string tag;
	string data;
	r2Data( char *t, char *d ) : tag( t ), data( d ) { }
	r2Data() : tag( "" ), data( "" ) { }
};

typedef const char *CLPSTR;

struct sectData
{
	char *tag;
	char *data;
};

struct point3
{
	union
	{
		struct
		{
			float x, y, z;
		};
		float v[3];
	};
	point3() { }
	point3( UI16 X, UI16 Y, SI08 Z ) : x(X), y(Y), z(Z) { }
	point3( float X, float Y, float Z ) : x(X), y(Y), z(Z) { }
	void Assign( UI16 X, UI16 Y, SI08 Z );
	void Assign( float X, float Y, float Z );
	float Mag( void ) const;
	float MagSquared( void ) const;
	float Mag3D( void ) const;
	float MagSquared3D( void ) const;
	void  Normalize( void );
};

inline point3 operator+( point3 const &a, point3 const &b )
{
	return (point3( a.x + b.x, a.y + b.y, a.z + b.z ));
};

inline point3 operator-( point3 const &a, point3 const &b )
{
	return (point3( a.x - b.x, a.y - b.y, a.z - b.z ));
};
inline point3 operator*( point3 const &a, float  const &b )
{
	return point3( a.x*b, a.y*b, a.z*b );
};
inline point3 operator*( float  const &a, point3 const &b )
{
	return point3( a*b.x, a*b.y, a*b.z );
};
inline point3 operator/( point3 const &a, float  const &b )
{
	float inv = 1.f / b;
	return point3( a.x*inv, a.y*inv, a.z*inv );
};
inline void point3::Assign( UI16 X, UI16 Y, SI08 Z )
{
	x = X;
	y = Y;
	z = Z;
}
inline void point3::Assign( float X, float Y, float Z )
{
	x = X;
	y = Y;
	z = Z;
}
inline float point3::Mag3D( void ) const
{
	return (float)sqrt( x*x + y*y + z*z );
}
inline float point3::MagSquared3D( void ) const
{
	return ( x*x + y*y + z*z );
}

inline float point3::Mag( void ) const
{
	return (float)sqrt( x*x + y*y );
}
inline float point3::MagSquared( void ) const
{
	return ( x*x + y*y );
}

inline void point3::Normalize( void )
{
	float foo = 1 / Mag3D();
	x *= foo;
	y *= foo;
	z *= foo;
}

#endif
