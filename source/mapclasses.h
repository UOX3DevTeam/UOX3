#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

#include "power.h"

namespace UOX
{

#define PACK_NEEDED

struct Static_st
{
	UI16 itemid;
	UI08 xoff;
	UI08 yoff;
	SI08 zoff;
	char align;	// force word alignment by hand to avoid bus errors - fur
} PACK_NEEDED;

struct map_st
{
	UI16 id;
	SI08 z;
};

struct MultiIndex_st
{
	SI32 start;
	SI32 length;
	SI32 unknown;
} PACK_NEEDED;

struct Multi_st
{
	SI32 visible;  // this needs to be first so it is word aligned to avoid bus errors - fur
	UI16 tile;
	SI16 x;
	SI16 y;
	SI08 z;
	SI08 empty;
} PACK_NEEDED;

enum TileFlags
{
	TF_FLOORLEVEL	= 0,
	TF_HOLDABLE,
	TF_TRANSPARENT,
	TF_TRANSLUCENT,
	TF_WALL,
	TF_DAMAGING,
	TF_BLOCKING,
	TF_WET,
	TF_UNKNOWN1,
	TF_SURFACE,
	TF_CLIMBABLE,
	TF_STACKABLE,
	TF_WINDOW,
	TF_NOSHOOT,
	TF_DISPLAYA,
	TF_DISPLAYAN,
	TF_DESCRIPTION,
	TF_FOLIAGE,
	TF_PARTIALHUE,
	TF_UNKNOWN2,
	TF_MAP,
	TF_CONTAINER,
	TF_WEARABLE,
	TF_LIGHT,
	TF_ANIMATED,
	TF_NODIAGONAL,
	TF_UNKNOWN3,
	TF_ARMOR,
	TF_ROOF,
	TF_DOOR,
	TF_STAIRBACK,
	TF_STAIRRIGHT,
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
		UI32 mFlags = flags.to_ulong();
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
	UI32 FlagsNum( void ) const					{	return flags.to_ulong();	}
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
	SI08 name[20];

public:
	CTile() : weight( 0 ), layer( 0 ), unknown1( 0 ), unknown2( 0 ), quantity( 0 ), animation( 0 ), unknown3( 0 ), hue( 0 ), unknown4( 0 ), unknown5( 0 ), height( 0 )
	{
		name[0] = 0;
	}
	CTile( UOXFile *toRead )
	{
		Read( toRead );
	}
	void Read( UOXFile *toRead );
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
	UI16 textureID;
	SI08 name[20];
public:
	CLand() : textureID( 0 )
	{
		name[0] = 0;
	}
	CLand( UOXFile *toRead )
	{
		Read( toRead );
	}

	void Read( UOXFile *toRead );

	UI16 TextureID( void ) const	{	return textureID;		}
	char *Name( void ) const		{	return (char *)name;	}

	void TextureID( UI08 newVal )	{	textureID = newVal;		}
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
	SI08 top;

public:
	CTileUni() : baseZ( 0 ), mType( 0 ), top( 0 )
	{
	}

	SI08 BaseZ( void ) const	{	return baseZ;	}
	SI08 Top( void ) const		{	return top;		}
	UI08 Type( void ) const		{	return mType;	}

	void BaseZ( SI08 nVal )		{	baseZ	= nVal;	}
	void Type( UI08 nVal )		{	mType	= nVal;	}
	void Top( SI08 nVal )		{	top		= nVal;	}
};

}

#endif

