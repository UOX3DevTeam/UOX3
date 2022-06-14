#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

#include "power.h"
#include <cstdint>
#include <sstream>
#include "MultiMul.hpp"
#include "osunique.hpp"
struct Static_st {
	UI16 itemid;
	UI08 xoff;
	UI08 yoff;
	SI08 zoff;
};

struct map_st {
	UI16 id;
	SI08 z;
};

enum TileFlags {
	// Flag:				Also known as:
	TF_FLOORLEVEL	= 0,		// "Background"
	TF_HOLDABLE,			// "Weapon"
	TF_TRANSPARENT,			// "SignGuildBanner"
	TF_TRANSLUCENT,			// "WebDirtBlood"
	TF_WALL,				// "WallVertTile"
	TF_DAMAGING,
	TF_BLOCKING,			// "Impassable"
	TF_WET,				// "LiquidWet"
	TF_UNKNOWN1,			// "Ignored"
	TF_SURFACE,				// "Standable"
	TF_CLIMBABLE,			// "Bridge"
	TF_STACKABLE,			// "Generic"
	TF_WINDOW,				// "WindowArchDoor"
	TF_NOSHOOT,				// "CannotShootThru"
	TF_DISPLAYA,			// "Prefix A"
	TF_DISPLAYAN,			// "Prefix An"
	TF_DESCRIPTION,			// "Internal"
	TF_FOLIAGE,				// "FadeWithTrans"
	TF_PARTIALHUE,
	TF_UNKNOWN2,
	TF_MAP,
	TF_CONTAINER,
	TF_WEARABLE,			// "Equipable"
	TF_LIGHT,				// "LightSource"
	TF_ANIMATED,
	TF_NODIAGONAL,			// "HoverOver" in SA clients and later, to determine if tiles can be moved on by flying gargoyles
	TF_UNKNOWN3,			// "NoDiagonal" in SA clients and later?
	TF_ARMOR,				// "WholeBodyItem"
	TF_ROOF,				// "WallRoofWeap"
	TF_DOOR,
	TF_STAIRBACK,			// "ClimbableBit1"
	TF_STAIRRIGHT,			// "ClimbableBit2"

	// Following flags were added in HS expansion? Purpose unknown
	TF_ALPHABLEND,
	TF_USENEWART,
	TF_ARTUSED,
	TF_NOSHADOW,
	TF_PIXELBLEED,
	TF_PLAYANIMONCE,
	TF_MULTIMOVABLE,
	TF_COUNT
};

//=======================================================================================
// Pre declare tileinfo so we can make it a friend of CBaseTile, CTile, and CLand
class tileinfo ;

//=======================================================================================
class CBaseTile {
	friend tileinfo ;
protected:
	std::bitset<64 >	flags;	// We are goint to use a 64 bit value here, so speicify 64 bit.  This one day may be changed
	std::string name;
	
public:
	CBaseTile() {
		flags.reset();
	}
	virtual ~CBaseTile() =default ;
	/*
	 auto Flag(UI08 part) const  ->std::uint8_t {
	 auto mFlags = static_cast<std::uint32_t>(flags.to_ulong());
	 auto retVal = std::uint8_t(0);
	 switch( part ) {
	 case 0: retVal = static_cast<std::uint8_t>(mFlags>>24);	break;
	 case 1: retVal = static_cast<std::uint8_t>(mFlags>>16);	break;
	 case 2: retVal = static_cast<std::uint8_t>(mFlags>>8);	break;
	 case 3: retVal = static_cast<std::uint8_t>(mFlags%256);	break;
	 }
	 return retVal;
	 }
	 */
	auto Name() const -> const std::string& {return name;}
	auto Name(const std::string &value) ->void {name = value;}
	
	auto FlagsNum() const ->std::uint32_t {	return static_cast<std::uint32_t>(flags.to_ulong());}
	auto Flags() const ->std::bitset<64> { return flags;}
	void Flags(std::bitset<64> newVal ){flags = newVal;}
	
	bool CheckFlag( TileFlags toCheck ) const {
		if( toCheck >= TF_COUNT ){
			return false;
		}
		return flags.test( toCheck );
	}
	void SetFlag( TileFlags toSet, bool newVal ){
		if( toSet >= TF_COUNT ){
			return;
		}
		flags.set( toSet, newVal );
	}
};
//=======================================================================================
class CTile : public CBaseTile {
private:
	friend tileinfo ;
	std::uint8_t weight;
	std::int8_t layer;
	std::uint16_t unknown1;
	std::uint8_t unknown2;
	std::uint8_t quantity;
	std::uint16_t animation;
	std::uint8_t unknown3;
	std::uint8_t hue;
	std::uint8_t unknown4;
	std::uint8_t unknown5;
	std::int8_t height;
	std::string name ;
	
public:
	CTile() :CBaseTile(), weight( 0 ), layer( 0 ), unknown1( 0 ), unknown2( 0 ), quantity( 0 ), animation( 0 ), unknown3( 0 ), hue( 0 ), unknown4( 0 ), unknown5( 0 ), height( 0 ) {
	}
	~CTile() = default ;
	UI16 Unknown1() const{return unknown1;}
	UI08 Unknown2() const{return unknown2;}
	UI08 Unknown3() const{return unknown3;}
	UI08 Unknown4() const{return unknown4;}
	UI08 Unknown5() const{return unknown5;}
	UI08 Hue() const{return hue;}
	UI08 Quantity() const{return quantity;}
	UI16 Animation() const	{return animation;}
	UI08 Weight() const{return weight;}
	SI08 Layer() const{return layer;}
	SI08 Height() const{return height;}
	auto ClimbHeight() const ->std::int8_t {
		if (CheckFlag(TF_CLIMBABLE)) {
			return height/2 ;
		}
		return height ;
	}
	void Unknown1( UI16 newVal )	{unknown1 = newVal;}
	void Unknown2( UI08 newVal )	{unknown2 = newVal;}
	void Unknown3( UI08 newVal )	{unknown3 = newVal;}
	void Unknown4( UI08 newVal )	{unknown4 = newVal;}
	void Unknown5( UI08 newVal )	{unknown5 = newVal;}
	void Animation( UI16 newVal )	{animation = newVal;}
	void Weight( UI08 newVal ){	weight = newVal;}
	void Layer( SI08 newVal ){layer = newVal;}
	void Height( SI08 newVal ){height = newVal;}
	void Hue( UI08 newVal )	{hue = newVal;}
	void Quantity( UI08 newVal )	{quantity = newVal;}
	/*
	 void Name( const char *newVal ) {
	 strncopy( (char *)name,21, newVal, 20 );
	 }
	 */
};
//=======================================================================================
class CLand : public CBaseTile {
	friend tileinfo ;
private:
	std::uint16_t textureID;
	// List of road tiles, or road-related tiles, on which houses cannot be placed
	constexpr static
	std::array<std::uint16_t,18> roadIDs{
		0x0009, 0x0015, // furrows
		0x0071, 0x0078, // dirt
		0x00E8, 0x00EB, // dirt
		0x0150, 0x015C, // furrows
		0x0442, 0x0479, // sand stone
		0x0501, 0x0510, // sand stone
		0x07AE, 0x07B1, // dirt
		0x3FF4, 0x3FF4, // dirt
		0x3FF8, 0x3FFB  // dirt
	};
	
public:
	CLand() :CBaseTile(),textureID(0) {
	}
	~CLand() = default ;
	auto TextureID() const ->std::uint16_t {return textureID;}
	auto TextureID( std::uint16_t newVal )->void {textureID = newVal;}
	auto isRoadID() const ->bool {
		auto iter = std::find_if(roadIDs.begin(),roadIDs.end(),[this](const std::uint16_t &value){
			return textureID == value ;
		});
		return iter != roadIDs.end() ;
	}
};


//=================================================================================================================
// Is this used for anything?
//
class CTileUni : public CBaseTile
{
private:
	SI08 baseZ;
	UI08 mType;		// 0 = Terrain, 1 = Item
	UI16 mID;
	SI08 top;
	SI08 height;
	
public:
	CTileUni() :CBaseTile(), baseZ( 0 ), mType( 0 ), mID( 0 ), top( 0 ), height( 0 ) {
	}
	~CTileUni() =default;
	SI08 BaseZ( void ) const {return baseZ;}
	SI08 Top( void ) const {return top;}
	UI08 Type( void ) const	{return mType;}
	SI08 Height( void )  const {return height;}
	UI16 GetID( void )  const {return mID;}
	
	void BaseZ(SI08 nVal) {baseZ = nVal;}
	void Type(UI08 nVal) {mType = nVal;}
	void Top(SI08 nVal) {top = nVal;}
	void Height(SI08 nval) {height = nval;}
	void SetID(UI16 nval) { mID = nval;}
};
#endif

