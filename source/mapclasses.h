#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

#include "power.h"
#include <cstdint>
#include <sstream>
#include "MultiMul.hpp"

struct Static_st
{
	UI16 itemid;
	UI08 xoff;
	UI08 yoff;
	SI08 zoff;
};

struct map_st
{
	UI16 id;
	SI08 z;
};

enum TileFlags
{
	// Flag:				Also known as:
	TF_FLOORLEVEL	= 0,	// "Background"
	TF_HOLDABLE,			// "Weapon"
	TF_TRANSPARENT,			// "SignGuildBanner"
	TF_TRANSLUCENT,			// "WebDirtBlood"
	TF_WALL,				// "WallVertTile"
	TF_DAMAGING,
	TF_BLOCKING,			// "Impassable"
	TF_WET,					// "LiquidWet"
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

class CBaseTile
{
protected:
	std::bitset< TF_COUNT >	flags;
public:
	CBaseTile()
	{
		flags.reset();
	}
	virtual ~CBaseTile()
	{
	}
	UI08 Flag( UI08 part ) const
	{
		UI32 mFlags = static_cast<std::uint32_t>(flags.to_ulong());
		UI08 retVal = 0;
		switch( part )
		{
			case 0:	retVal = static_cast<UI08>(mFlags>>24);	break;
			case 1:	retVal = static_cast<UI08>(mFlags>>16);	break;
			case 2: retVal = static_cast<UI08>(mFlags>>8);	break;
			case 3: retVal = static_cast<UI08>(mFlags%256);	break;
		}
		return retVal;
	}
	UI32 FlagsNum( void ) const					{	return static_cast<std::uint32_t>(flags.to_ulong());	}
	std::bitset< TF_COUNT > Flags( void ) const	{	return flags;				}
	void Flags( std::bitset< TF_COUNT > newVal ){	flags = newVal;				}

	bool CheckFlag( TileFlags toCheck ) const
	{
		if( toCheck >= TF_COUNT )
			return false;
		return flags.test( toCheck );
	}
	void SetFlag( TileFlags toSet, bool newVal )
	{
		if( toSet >= TF_COUNT )
			return;
		flags.set( toSet, newVal );
	}
};

class CTile : public CBaseTile
{
private:
	UI32 unknown0;
	UI08 weight;
	SI08 layer;
	UI16 unknown1;
	UI08 unknown2;
	UI08 quantity;
	UI16 animation;
	UI08 unknown3;
	UI08 hue;
	UI08 unknown4;
	UI08 unknown5;
	SI08 height;
	SI08 name[21];

public:
	CTile() : unknown0( 0 ), weight( 0 ), layer( 0 ), unknown1( 0 ), unknown2( 0 ), quantity( 0 ), animation( 0 ), unknown3( 0 ), hue( 0 ), unknown4( 0 ), unknown5( 0 ), height( 0 )
	{
		name[0] = 0;
	}
	CTile( UOXFile *toRead, bool useHS  )
	{
		Read( toRead,useHS );
	}
	void Read( UOXFile *toRead, bool useHS);
	UI32 Unknown0( void ) const		{	return unknown0;		}
	UI16 Unknown1( void ) const		{	return unknown1;		}
	UI08 Unknown2( void ) const		{	return unknown2;		}
	UI08 Unknown3( void ) const		{	return unknown3;		}
	UI08 Unknown4( void ) const		{	return unknown4;		}
	UI08 Unknown5( void ) const		{	return unknown5;		}
	UI08 Hue( void ) const			{	return hue;				}
	UI08 Quantity( void ) const		{	return quantity;		}
	UI16 Animation( void ) const	{	return animation;		}
	UI08 Weight( void ) const		{	return weight;			}
	SI08 Layer( void ) const		{	return layer;			}
	SI08 Height( void ) const		{	return height;			}
	char *Name( void ) const		{	return (char *)name;	}

	void Unknown0( UI32 newVal )	{	unknown0 = newVal;		}
	void Unknown1( UI16 newVal )	{	unknown1 = newVal;		}
	void Unknown2( UI08 newVal )	{	unknown2 = newVal;		}
	void Unknown3( UI08 newVal )	{	unknown3 = newVal;		}
	void Unknown4( UI08 newVal )	{	unknown4 = newVal;		}
	void Unknown5( UI08 newVal )	{	unknown5 = newVal;		}
	void Animation( UI16 newVal )	{	animation = newVal;		}
	void Weight( UI08 newVal )		{	weight = newVal;		}
	void Layer( SI08 newVal )		{	layer = newVal;			}
	void Height( SI08 newVal )		{	height = newVal;		}
	void Hue( UI08 newVal )			{	hue = newVal;			}
	void Quantity( UI08 newVal )	{	quantity = newVal;		}
	void Name( const char *newVal )
	{
		strncpy( (char *)name, newVal, 20 );
	}

};

class CLand : public CBaseTile
{
private:
	UI32 unknown1;
	UI16 textureID;
	SI08 name[21];
public:
	CLand() : unknown1( 0 ), textureID( 0 )
	{
		name[0] = 0;
	}
	CLand( UOXFile *toRead, bool useHS )
	{
		Read( toRead, useHS);
	}

	void Read( UOXFile *toRead, bool useHS );

	UI32 Unknown1( void ) const		{	return unknown1;		}
	UI16 TextureID( void ) const	{	return textureID;		}
	char *Name( void ) const		{	return (char *)name;	}

	void Unknown1( UI32 newVal )	{	unknown1 = newVal;		}
	void TextureID( UI16 newVal )	{	textureID = newVal;		}
	void Name( char *newVal )
	{
		strncpy( (char *)name, newVal, 20 );
	}
};

class CTileUni : public CBaseTile
{
private:
	SI08 baseZ;
	UI08 mType;		// 0 = Terrain, 1 = Item
	UI16 mID;
	SI08 top;
	SI08 height;

public:
	CTileUni() : baseZ( 0 ), mType( 0 ), mID( 0 ), top( 0 ), height( 0 )
	{
	}

	SI08 BaseZ( void ) const	{	return baseZ;	}
	SI08 Top( void ) const		{	return top;		}
	UI08 Type( void ) const		{	return mType;	}
	SI08 Height( void )  const    {   return height; }
	UI16 GetID( void )  const    {   return mID; }

	void BaseZ( SI08 nVal )		{	baseZ	= nVal;	}
	void Type( UI08 nVal )		{	mType	= nVal;	}
	void Top( SI08 nVal )		{	top		= nVal;	}
	void Height(SI08 nval)      {   height  = nval; }
	void SetID(UI16 nval)      {   mID  = nval; }
};

#endif

