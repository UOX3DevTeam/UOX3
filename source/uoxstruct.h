#ifndef __UOXSTRUCT_H
#define __UOXSTRUCT_H

#include <array>
#include <bitset>
#include <cmath>
#include <string>
#include <cstdio>

constexpr auto BIT_ANIMAL 	= std::uint8_t(2) ;
constexpr auto BIT_ANTIBLINK	= std::uint8_t(1) ;
constexpr auto BIT_CANFLY	= std::uint8_t(0) ;
constexpr auto BIT_WATER	= std::uint8_t(3) ;
constexpr auto BIT_AMPHI	= std::uint8_t(4) ;
constexpr auto BIT_HUMAN	= std::uint8_t(5) ;

// ==============================================================================================
// CCreatures
// ==============================================================================================

class CCreatures {
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
	std::array<UI16,SND_COUNT> soundList;
	std::bitset<6>	who_am_i;
	
	UI16 creatureID;
	UI16	icon;
	UI16	mountID;
	
public:
	CCreatures() : creatureID(0), icon(0), mountID(0){
		who_am_i.reset();
		soundList.fill(0);
	}
	
	auto GetSound( monsterSound soundType ) const ->UI16 		{return soundList[static_cast<UI08>(soundType)];}
	auto SetSound( monsterSound soundType, UI16 newVal ) ->void {soundList[static_cast<UI08>(soundType)] = newVal;}
	
	auto Icon() const ->UI16 			{return icon;}
	auto WhoAmI() const ->UI08 			{return static_cast<UI08>(who_am_i.to_ulong());}
	auto Icon(UI16 value) ->void 			{icon = value;}
	auto MountID( void ) const ->UI16 		{return mountID;}
	auto MountID( UI16 value ) ->void 		{mountID = value;}
	auto CreatureID( void ) const ->UI16 	{return creatureID;}
	auto CreatureID( UI16 value ) ->void 	{creatureID = value;}
	
	auto IsAnimal( void ) const ->bool		{return who_am_i.test( BIT_ANIMAL );}
	auto IsHuman( void ) const ->bool		{return who_am_i.test( BIT_HUMAN );}
	auto AntiBlink( void ) const ->bool		{return who_am_i.test( BIT_ANTIBLINK );}
	auto CanFly( void ) const ->bool		{return who_am_i.test( BIT_CANFLY );}
	auto IsWater( void ) const ->bool		{return who_am_i.test( BIT_WATER );}
	auto IsAmphibian( void ) const ->bool	{return who_am_i.test( BIT_AMPHI );}
	auto IsAnimal( bool value ) ->void		{who_am_i.set( BIT_ANIMAL, value );}
	auto IsHuman( bool value ) ->void		{who_am_i.set( BIT_HUMAN, value );}
	auto AntiBlink( bool value ) ->void		{who_am_i.set( BIT_ANTIBLINK, value );}
	auto CanFly( bool value ) ->void		{who_am_i.set( BIT_CANFLY, value );}
	auto IsWater( bool value ) ->void		{who_am_i.set( BIT_WATER, value );}
	auto IsAmphibian( bool value ) ->void	{who_am_i.set( BIT_AMPHI, value );}
};

// ==============================================================================================
// point3
// ==============================================================================================
struct point3{
	R32 x, y, z;
	point3(UI16 X, UI16 Y, SI08 Z ) : point3(static_cast<R32>(X),static_cast<R32>(Y),static_cast<R32>(Z)){}
	point3( R32 X =0.0, R32 Y =0.0, R32 Z =0.0 ) :x(X),y(Y),z(Z){}
	inline auto	Assign( UI16 X, UI16 Y, SI08 Z ) ->void{ x=static_cast<R32>(X);y=static_cast<R32>(Y);z=static_cast<R32>(z);}
	void	Assign( R32 X, R32 Y, R32 Z ){x=X;y=Y;z=Z;}
	R64	Mag() const;
	R32	MagSquared() const;
	R64	Mag3D() const;
	R32	MagSquared3D() const;
	void	Normalize( void );
	// I made these, but not including. This is what I would expect, but the inline ones create
	// a new instance, and dont touch the old ones.  So you cant "nest" them.  Probably
	// these would be fine, but not risking it.
	//inline auto operator==(const point3 &b) ->bool { return (x==b.x) && (y==b.y) && (z==b.z);}
	//inline auto operator+(const point3 &b) ->point3& {x +=b.x;y+=b.y;z+=b.z;return *this;}
	//inline auto operator-(const point3 &b) ->point3& {x -=b.x;y-=b.y;z-=b.z;return *this;}
	//inline auto operator*(const point3 &b) ->point3& {x *=b.x;y*=b.y;z*=b.z;return *this;}
};

inline auto operator==( point3 const &a, point3 const &b ) ->bool {
	return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) );
}
inline auto operator+( point3 const &a, point3 const &b ) ->point3 {
	return point3( a.x + b.x, a.y + b.y, a.z + b.z );
}

inline auto operator-( point3 const &a, point3 const &b ) ->point3{
	return point3( a.x - b.x, a.y - b.y, a.z - b.z );
}
inline auto operator*( point3 const &a, R32 const &b ) ->point3 {
	return point3( a.x*b, a.y*b, a.z*b );
}
inline auto operator*( R32 const &a, point3 const &b ) ->point3{
	return point3( a * b.x, a * b.y, a * b.z );
}
inline auto operator/( point3 const &a, R32 const &b ) ->point3{
	auto inv = 1.f / b; // I am assuming it is coming from a world, where multplication
				  // was faster then division, so divides once, then multiplies
	return point3( a.x * inv, a.y * inv, a.z * inv );
}

inline auto point3::Mag3D( void ) const ->R64{
	// This is intersting, it returns a R64, but we cast to R32
	// I assume it is because one wants to lose precision.  But then,
	// why not just let the consumer cast it up to R64 if needed?
	// this will "auto cast" up
	return static_cast<R32>(std::sqrt( x*x + y*y + z*z ));
}
inline auto point3::MagSquared3D( void ) const ->R32{
	return ( x*x + y*y + z*z );
}

inline auto point3::Mag( void ) const ->R64{
	// This is intersting, it returns a R64, but we cast to R32
	// I assume it is because one wants to lose precision.  But then,
	// why not just let the consumer cast it up to R64 if needed?
	// this will "auto cast" up
	return static_cast<R32>(std::sqrt( x*x + y*y ));
}
inline auto point3::MagSquared( void ) const ->R32{
	return ( x*x + y*y );
}

inline auto point3::Normalize( void ) ->void{
	auto  foo = 1.f / Mag3D();
	x *= foo;
	y *= foo;
	z *= foo;
}

// ==============================================================================================
// UOXFileWrapper
// ==============================================================================================

struct UOXFileWrapper{
	FILE *mWrap;
	UOXFileWrapper() {mWrap=nullptr;}
};

// ==============================================================================================
// GoPlaces_st
// ==============================================================================================
struct GoPlaces_st{
	SI16 x;
	SI16 y;
	SI08 z;
	UI08 worldNum;
	UI16 instanceID;
	GoPlaces_st():x(-1), y(-1), z(-1), worldNum(0), instanceID(0){}
};
// ==============================================================================================
// CTeleLocationEntry
// ==============================================================================================
// Teleport Locations
class CTeleLocationEntry{
private:
	point3	src;
	UI08		srcWorld;
	point3	trg;
	UI08		trgWorld;
public:
	CTeleLocationEntry() : srcWorld( 0xFF ), trgWorld( 0 ),src(0,0,ILLEGAL_Z),trg(0,0,ILLEGAL_Z){}
	
	auto SourceLocation( void ) const ->point3		{ return src;}
	auto SourceLocation( UI16 x, UI16 y, SI08 z ) ->void	{src.Assign(x,y,z);}
	auto SourceWorld( void ) const ->UI08			{return srcWorld;}
	auto SourceWorld( UI08 newVal ) ->void			{srcWorld = newVal;}
	auto TargetLocation( void ) const ->point3		{return trg;}
	auto TargetLocation( UI16 x, UI16 y, SI08 z )->void	{trg.Assign(x,y,z );}
	auto TargetWorld( void ) const ->UI08			{return trgWorld;}
	auto TargetWorld( UI08 newVal ) ->void			{trgWorld = newVal;}
};
// ==============================================================================================
// LogoutLocationEntry
// ==============================================================================================
// Instalog Locations
struct LogoutLocationEntry{
	SI16 x1 ;
	SI16 y1 ;
	SI16 x2 ;
	SI16 y2 ;
	UI08 worldNum ;
	UI16 instanceID ;
	LogoutLocationEntry():y1(0),x1(0),y2(0),x2(0),worldNum(0),instanceID(0){}
};

// ==============================================================================================
// SOSLocationEntry
// ==============================================================================================
// SOS Locations
// Why this was originally a differnt type, I dont know.
using SOSLocationEntry = LogoutLocationEntry ;
// ==============================================================================================
// advance_st
// ==============================================================================================
struct advance_st{
	UI16 base;
	UI08 success;
	UI08 failure;
	UI08 amtToGain;
	advance_st():base(0),success(0),failure(0),amtToGain(1){}
};

// ==============================================================================================
// TitlePair_st
// ==============================================================================================
struct TitlePair_st{
	SI16 lowBound;
	std::string toDisplay;
	TitlePair_st(SI16 lB=0,const std::string& toDisp="") :lowBound(lB),toDisplay(toDisp){}
};
#endif
