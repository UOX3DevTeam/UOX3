#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

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

class CBaseTile
{
protected:
	UI32 flags;
public:
	CBaseTile() : flags( 0 )
	{
	}
	virtual ~CBaseTile()
	{
	}
	UI08 Flags( void ) const			{	return flags;	}
	void Flags( UI08 newVal )			{	flags = newVal;	}

	bool AtFloorLevel( void ) const		{	return MFLAGGET( flags, 0x00000001 );	}
	bool Holdable( void ) const			{	return MFLAGGET( flags, 0x00000002 );	}
	bool SignGuildBanner( void ) const	{	return MFLAGGET( flags, 0x00000004 );	}
	bool WebDirtBlood( void ) const		{	return MFLAGGET( flags, 0x00000008 );	}
	bool WallVertTile( void ) const		{	return MFLAGGET( flags, 0x00000010 );	}
	bool Damaging( void ) const			{	return MFLAGGET( flags, 0x00000020 );	}
	bool Blocking( void ) const			{	return MFLAGGET( flags, 0x00000040 );	}
	bool LiquidWet( void ) const		{	return MFLAGGET( flags, 0x00000080 );	}
	bool UnknownFlag1( void ) const		{	return MFLAGGET( flags, 0x00000100 );	}
	bool Standable( void ) const		{	return MFLAGGET( flags, 0x00000200 );	}
	bool Climbable( void ) const		{	return MFLAGGET( flags, 0x00000400 );	}
	bool Stackable( void ) const		{	return MFLAGGET( flags, 0x00000800 );	}
	bool WindowArchDoor( void ) const	{	return MFLAGGET( flags, 0x00001000 );	}
	bool CannotShootThru( void ) const	{	return MFLAGGET( flags, 0x00002000 );	}
	bool DisplayAsA( void ) const		{	return MFLAGGET( flags, 0x00004000 );	}
	bool DisplayAsAn( void ) const		{	return MFLAGGET( flags, 0x00008000 );	}
	bool DescriptionTile( void ) const	{	return MFLAGGET( flags, 0x00010000 );	}
	bool FadeWithTrans( void ) const	{	return MFLAGGET( flags, 0x00020000 );	}
	bool PartialHue( void ) const		{	return MFLAGGET( flags, 0x00040000 );	}
	bool UnknownFlag2( void ) const		{	return MFLAGGET( flags, 0x00080000 );	}
	bool Map( void ) const				{	return MFLAGGET( flags, 0x00100000 );	}
	bool Container( void ) const		{	return MFLAGGET( flags, 0x00200000 );	}
	bool Equipable( void ) const		{	return MFLAGGET( flags, 0x00400000 );	}
	bool LightSource( void ) const		{	return MFLAGGET( flags, 0x00800000 );	}
	bool Animated( void ) const			{	return MFLAGGET( flags, 0x01000000 );	}
	bool NoDiagonal( void ) const		{	return MFLAGGET( flags, 0x02000000 );	}
	bool UnknownFlag3( void ) const		{	return MFLAGGET( flags, 0x04000000 );	}
	bool WholeBodyItem( void ) const	{	return MFLAGGET( flags, 0x08000000 );	}
	bool WallRoofWeap( void ) const		{	return MFLAGGET( flags, 0x10000000 );	}
	bool Door( void ) const				{	return MFLAGGET( flags, 0x20000000 );	}
	bool ClimbableBit1( void ) const	{	return MFLAGGET( flags, 0x40000000 );	}
	bool ClimbableBit2( void ) const	{	return MFLAGGET( flags, 0x80000000 );	}

	void AtFloorLevel( bool val )		{	MFLAGSET( flags, val, 0x00000001 )	}
	void Holdable( bool val )			{	MFLAGSET( flags, val, 0x00000002 )	}
	void SignGuildBanner( bool val )	{	MFLAGSET( flags, val, 0x00000004 )	}
	void WebDirtBlood( bool val )		{	MFLAGSET( flags, val, 0x00000008 )	}
	void WallVertTile( bool val )		{	MFLAGSET( flags, val, 0x00000010 )	}
	void Damaging( bool val )			{	MFLAGSET( flags, val, 0x00000020 )	}
	void Blocking( bool val )			{	MFLAGSET( flags, val, 0x00000040 )	}
	void LiquidWet( bool val )			{	MFLAGSET( flags, val, 0x00000080 )	}
	void UnknownFlag1( bool val )		{	MFLAGSET( flags, val, 0x00000100 )	}
	void Standable( bool val )			{	MFLAGSET( flags, val, 0x00000200 )	}
	void Climbable( bool val )			{	MFLAGSET( flags, val, 0x00000400 )	}
	void Stackable( bool val )			{	MFLAGSET( flags, val, 0x00000800 )	}
	void WindowArchDoor( bool val )		{	MFLAGSET( flags, val, 0x00001000 )	}
	void CannotShootThru( bool val )	{	MFLAGSET( flags, val, 0x00002000 )	}
	void DisplayAsA( bool val )			{	MFLAGSET( flags, val, 0x00004000 )	}
	void DisplayAsAn( bool val )		{	MFLAGSET( flags, val, 0x00008000 )	}
	void DescriptionTile( bool val )	{	MFLAGSET( flags, val, 0x00010000 )	}
	void FadeWithTrans( bool val )		{	MFLAGSET( flags, val, 0x00020000 )	}
	void PartialHue( bool val )			{	MFLAGSET( flags, val, 0x00040000 )	}
	void UnknownFlag2( bool val )		{	MFLAGSET( flags, val, 0x00080000 )	}
	void Map( bool val )				{	MFLAGSET( flags, val, 0x00100000 )	}
	void Container( bool val )			{	MFLAGSET( flags, val, 0x00200000 )	}
	void Equipable( bool val )			{	MFLAGSET( flags, val, 0x00400000 )	}
	void LightSource( bool val )		{	MFLAGSET( flags, val, 0x00800000 )	}
	void Animated( bool val )			{	MFLAGSET( flags, val, 0x01000000 )	}
	void NoDiagonal( bool val )			{	MFLAGSET( flags, val, 0x02000000 )	}
	void UnknownFlag3( bool val )		{	MFLAGSET( flags, val, 0x04000000 )	}
	void WholeBodyItem( bool val )		{	MFLAGSET( flags, val, 0x08000000 )	}
	void WallRoofWeap( bool val )		{	MFLAGSET( flags, val, 0x10000000 )	}
	void Door( bool val )				{	MFLAGSET( flags, val, 0x20000000 )	}
	void ClimbableBit1( bool val )		{	MFLAGSET( flags, val, 0x40000000 )	}
	void ClimbableBit2( bool val )		{	MFLAGSET( flags, val, 0x80000000 )	}

	UI08 ClimbableBits( void )
	{
		UI08 p1 = 0, p2 = 0;
		if( ClimbableBit1() )
			p1 = 1;
		if( ClimbableBit2() )
			p2 = 1;
		return (UI08)(p1 * 2 + p2);
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
	CTile() : unknown1( 0 ), unknown2( 0 ), unknown3( 0 ), unknown4( 0 ), unknown5( 0 ), weight( 0 ), height( 0 ), layer( 0 ), hue( 0 ), animation( 0 )
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
	SI08 basez;
	UI08 mType;		// 0 = Terrain, 1 = Item
	UI16 id;
	SI08 height;

public:
	CTileUni() : basez( 0 ), mType( 0 ), id( 0 ), height( 0 )
	{
	}

	SI08 BaseZ( void ) const	{	return basez;	}
	UI08 Type( void ) const		{	return mType;	}
	UI16 ID( void ) const		{	return id;		}
	SI08 Height( void ) const	{	return height;	}

	void BaseZ( SI08 nVal )		{	basez = nVal;	}
	void Type( UI08 nVal )		{	mType = nVal;	}
	void ID( UI16 nVal )		{	id = nVal;		}
	void Height( SI08 nVal )	{	height = nVal;	}

	CTileUni &operator=( CLand &toCopy )
	{
		Flags( toCopy.Flags() );
		return (*this);
	}
	CTileUni &operator=( CTile &toCopy )
	{
		Flags( toCopy.Flags() );
		Height( toCopy.Height() );
		return (*this);
	}
};

}

#endif

