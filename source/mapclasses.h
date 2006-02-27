#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

namespace UOX
{

#define PACK_NEEDED

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

class BaseTile
{
protected:
	UI08 flag1;
	UI08 flag2;
	UI08 flag3;
	UI08 flag4;
public:
	BaseTile() : flag1( 0 ), flag2( 0 ), flag3( 0 ), flag4( 0 )
	{
	}
	virtual ~BaseTile()
	{
	}
	UI08 Flag1( void ) const			{	return flag1;	}
	UI08 Flag2( void ) const			{	return flag2;	}
	UI08 Flag3( void ) const			{	return flag3;	}
	UI08 Flag4( void ) const			{	return flag4;	}

	void Flag1( UI08 newVal )			{	flag1 = newVal;	}
	void Flag2( UI08 newVal )			{	flag2 = newVal;	}
	void Flag3( UI08 newVal )			{	flag3 = newVal;	}
	void Flag4( UI08 newVal )			{	flag4 = newVal;	}

	bool AtFloorLevel( void ) const		{	return (flag1&0x01) == 0x01;	}
	bool Holdable( void ) const			{	return (flag1&0x02) == 0x02;	}
	bool SignGuildBanner( void ) const	{	return (flag1&0x04) == 0x04;	}
	bool WebDirtBlood( void ) const		{	return (flag1&0x08) == 0x08;	}
	bool WallVertTile( void ) const		{	return (flag1&0x10) == 0x10;	}
	bool Damaging( void ) const			{	return (flag1&0x20) == 0x20;	}
	bool Blocking( void ) const			{	return (flag1&0x40) == 0x40;	}
	bool LiquidWet( void ) const		{	return (flag1&0x80) == 0x80;	}
	bool Unknown1( void ) const			{	return (flag2&0x01) == 0x01;	}
	bool Standable( void ) const		{	return (flag2&0x02) == 0x02;	}
	bool Climbable( void ) const		{	return (flag2&0x04) == 0x04;	}
	bool Stackable( void ) const		{	return (flag2&0x08) == 0x08;	}
	bool WindowArchDoor( void ) const	{	return (flag2&0x10) == 0x10;	}
	bool CannotShootThru( void ) const	{	return (flag2&0x20) == 0x20;	}
	bool DisplayAsA( void ) const		{	return (flag2&0x40) == 0x40;	}
	bool DisplayAsAn( void ) const		{	return (flag2&0x80) == 0x80;	}
	bool DescriptionTile( void ) const	{	return (flag3&0x01) == 0x01;	}
	bool FadeWithTrans( void ) const	{	return (flag3&0x02) == 0x02;	}
	bool Unknown2( void ) const			{	return (flag3&0x04) == 0x04;	}
	bool Unknown3( void ) const			{	return (flag3&0x08) == 0x08;	}
	bool Map( void ) const				{	return (flag3&0x10) == 0x10;	}
	bool Container( void ) const		{	return (flag3&0x20) == 0x20;	}
	bool Equipable( void ) const		{	return (flag3&0x40) == 0x40;	}
	bool LightSource( void ) const		{	return (flag3&0x80) == 0x80;	}
	bool Animated( void ) const			{	return (flag4&0x01) == 0x01;	}
	bool Unknown4( void ) const			{	return (flag4&0x02) == 0x02;	}
	bool Walk( void ) const				{	return (flag4&0x04) == 0x04;	}
	bool WholeBodyItem( void ) const	{	return (flag4&0x08) == 0x08;	}
	bool WallRoofWeap( void ) const		{	return (flag4&0x10) == 0x10;	}
	bool Door( void ) const				{	return (flag4&0x20) == 0x20;	}
	bool ClimbableBit1( void ) const	{	return (flag4&0x40) == 0x40;	}
	bool ClimbableBit2( void ) const	{	return (flag4&0x80) == 0x80;	}

	void AtFloorLevel( bool val )		{	MFLAGSET( flag1, val, 0x01 )	}
	void Holdable( bool val )			{	MFLAGSET( flag1, val, 0x02 )	}
	void SignGuildBanner( bool val )	{	MFLAGSET( flag1, val, 0x04 )	}
	void WebDirtBlood( bool val )		{	MFLAGSET( flag1, val, 0x08 )	}
	void WallVertTile( bool val )		{	MFLAGSET( flag1, val, 0x10 )	}
	void Damaging( bool val )			{	MFLAGSET( flag1, val, 0x20 )	}
	void Blocking( bool val )			{	MFLAGSET( flag1, val, 0x40 )	}
	void LiquidWet( bool val )			{	MFLAGSET( flag1, val, 0x80 )	}

	void Unknown1( bool val )			{	MFLAGSET( flag2, val, 0x01 )	}
	void Standable( bool val )			{	MFLAGSET( flag2, val, 0x02 )	}
	void Climbable( bool val )			{	MFLAGSET( flag2, val, 0x04 )	}
	void Stackable( bool val )			{	MFLAGSET( flag2, val, 0x08 )	}
	void WindowArchDoor( bool val )		{	MFLAGSET( flag2, val, 0x10 )	}
	void CannotShootThru( bool val )	{	MFLAGSET( flag2, val, 0x20 )	}
	void DisplayAsA( bool val )			{	MFLAGSET( flag2, val, 0x40 )	}
	void DisplayAsAn( bool val )		{	MFLAGSET( flag2, val, 0x80 )	}

	void DescriptionTile( bool val )	{	MFLAGSET( flag3, val, 0x01 )	}
	void FadeWithTrans( bool val )		{	MFLAGSET( flag3, val, 0x02 )	}
	void Unknown2( bool val )			{	MFLAGSET( flag3, val, 0x04 )	}
	void Unknown3( bool val )			{	MFLAGSET( flag3, val, 0x08 )	}
	void Map( bool val )				{	MFLAGSET( flag3, val, 0x10 )	}
	void Container( bool val )			{	MFLAGSET( flag3, val, 0x20 )	}
	void Equipable( bool val )			{	MFLAGSET( flag3, val, 0x40 )	}
	void LightSource( bool val )		{	MFLAGSET( flag3, val, 0x80 )	}

	void Animated( bool val )			{	MFLAGSET( flag4, val, 0x01 )	}
	void Unknown4( bool val )			{	MFLAGSET( flag4, val, 0x02 )	}
	void Walk( bool val )				{	MFLAGSET( flag4, val, 0x04 )	}
	void WholeBodyItem( bool val )		{	MFLAGSET( flag4, val, 0x08 )	}
	void WallRoofWeap( bool val )		{	MFLAGSET( flag4, val, 0x10 )	}
	void Door( bool val )				{	MFLAGSET( flag4, val, 0x20 )	}
	void ClimbableBit1( bool val )		{	MFLAGSET( flag4, val, 0x40 )	}
	void ClimbableBit2( bool val )		{	MFLAGSET( flag4, val, 0x80 )	}

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
class CTile : public BaseTile
{
private:
	SI32 unknown1;  // longs must go at top to avoid bus errors - fur
	SI08 unknown2;
	SI08 unknown3;
	UI08 weight;
	SI08 height;
	SI08 layer;
	SI32 animation;
	SI08 name[23];	// manually padded to long to avoid bus errors - fur

public:
	CTile() : unknown1( 0 ), unknown2( 0 ), unknown3( 0 ), weight( 0 ), height( 0 ), layer( 0 ), animation( 0 )
	{
		name[0] = 0;
	}
	CTile( UOXFile *toRead )
	{
		Read( toRead );
	}
	CTile( std::ifstream &toRead );
	void Read( UOXFile *toRead );
	SI32 Unknown1( void ) const		{	return unknown1;		}
	SI32 Animation( void ) const	{	return animation;		}
	UI08 Weight( void ) const		{	return weight;			}
	SI08 Layer( void ) const		{	return layer;			}
	SI08 Unknown2( void ) const		{	return unknown2;		}
	SI08 Unknown3( void ) const		{	return unknown3;		}
	SI08 Height( void ) const		{	return height;			}
	char *Name( void ) const		{	return (char *)name;	}

	void Unknown1( SI32 newVal )	{	unknown1 = newVal;		}
	void Animation( SI32 newVal )	{	animation = newVal;		}
	void Weight( UI08 newVal )		{	weight = newVal;		}
	void Layer( SI08 newVal )		{	layer = newVal;			}
	void Unknown2( SI08 newVal )	{	unknown2 = newVal;		}
	void Unknown3( SI08 newVal )	{	unknown3 = newVal;		}
	void Height( SI08 newVal )		{	height = newVal;		}
	void Name( const char *newVal )
	{
		strncpy( (char *)name, newVal, 22 );
	}

};

class CLand : public BaseTile
{
private:
	UI16 textureID;
	SI08 name[20];
public:
	CLand() : textureID( 0 )
	{
		name[0] = 0;
	}
	CLand( std::ifstream &toRead );
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
		strcpy( (char *)name, newVal );
	}
};

class CTileUni : public BaseTile
{
private:
	SI08 basez;
	UI08 mType;		// 0 = Terrain, 1 = Item
	UI16 id;
	SI08 height;
	UI08 weight;

public:
			CTileUni() : basez( 0 ), mType( 0 ), id( 0 ), height( 0 ), weight( 0 )
			{
			}

	SI08 BaseZ( void ) const	{	return basez;	}
	UI08 Type( void ) const		{	return mType;	}
	UI16 ID( void ) const		{	return id;		}
	SI08 Height( void ) const	{	return height;	}
	UI08 Weight( void ) const	{	return weight;	}

	void BaseZ( SI08 nVal )		{	basez = nVal;	}
	void Type( UI08 nVal )		{	mType = nVal;	}
	void ID( UI16 nVal )		{	id = nVal;		}
	void Height( SI08 nVal )	{	height = nVal;	}
	void Weight( UI08 nVal )	{	weight = nVal;	}

	CTileUni &operator=( CLand &toCopy )
	{
		Flag1( toCopy.Flag1() );
		Flag2( toCopy.Flag2() );
		Flag3( toCopy.Flag3() );
		Flag4( toCopy.Flag4() );
		return (*this);
	}
	CTileUni &operator=( CTile &toCopy )
	{
		Flag1( toCopy.Flag1() );
		Flag2( toCopy.Flag2() );
		Flag3( toCopy.Flag3() );
		Flag4( toCopy.Flag4() );
		Height( toCopy.Height() );
		Weight( toCopy.Weight() );
		return (*this);
	}
};

}

#endif

