#ifndef __UOXMAP_CLASSES_H__
#define __UOXMAP_CLASSES_H__

#include "fileio.h"

class BaseTile
{
protected:
	UI08 flag1;
	UI08 flag2;
	UI08 flag3;
	UI08 flag4;
public:
	BaseTile() : flag1( 0 ), flag2( 0 ), flag3( 0 ), flag4( 0 ) { }
	virtual ~BaseTile() {}
	UI08 Flag1( void )			{	return flag1;		}
	UI08 Flag2( void )			{	return flag2;		}
	UI08 Flag3( void )			{	return flag3;		}
	UI08 Flag4( void )			{	return flag4;		}

	void Flag1( UI08 newVal )	{	flag1 = newVal;			}
	void Flag2( UI08 newVal )	{	flag2 = newVal;			}
	void Flag3( UI08 newVal )	{	flag3 = newVal;			}
	void Flag4( UI08 newVal )	{	flag4 = newVal;			}

	bool AtFloorLevel( void )	{	return (flag1&0x01) == 0x01;	}
	bool Holdable( void )		{	return (flag1&0x02) == 0x02;	}
	bool SignGuildBanner( void ){	return (flag1&0x04) == 0x04;	}
	bool WebDirtBlood( void )	{	return (flag1&0x08) == 0x08;	}
	bool WallVertTile( void )	{	return (flag1&0x10) == 0x10;	}
	bool Damaging( void )		{	return (flag1&0x20) == 0x20;	}
	bool Blocking( void )		{	return (flag1&0x40) == 0x40;	}
	bool LiquidWet( void )		{	return (flag1&0x80) == 0x80;	}

	bool Unknown1( void )		{	return (flag2&0x01) == 0x01;	}
	bool Standable( void )		{	return (flag2&0x02) == 0x02;	}
	bool Climbable( void )		{	return (flag2&0x04) == 0x04;	}
	bool Stackable( void )		{	return (flag2&0x08) == 0x08;	}
	bool WindowArchDoor( void )	{	return (flag2&0x10) == 0x10;	}
	bool CannotShootThru( void ){	return (flag2&0x20) == 0x20;	}
	bool DisplayAsA( void )		{	return (flag2&0x40) == 0x40;	}
	bool DisplayAsAn( void )	{	return (flag2&0x80) == 0x80;	}

	bool DescriptionTile( void ){	return (flag3&0x01) == 0x01;	}
	bool FadeWithTrans( void )	{	return (flag3&0x02) == 0x02;	}
	bool Unknown2( void )		{	return (flag3&0x04) == 0x04;	}
	bool Unknown3( void )		{	return (flag3&0x08) == 0x08;	}
	bool Map( void )			{	return (flag3&0x10) == 0x10;	}
	bool Container( void )		{	return (flag3&0x20) == 0x20;	}
	bool Equipable( void )		{	return (flag3&0x40) == 0x40;	}
	bool LightSource( void )	{	return (flag3&0x80) == 0x80;	}

	bool Animated( void )		{	return (flag4&0x01) == 0x01;	}
	bool Unknown4( void )		{	return (flag4&0x02) == 0x02;	}
	bool Walk( void )			{	return (flag4&0x04) == 0x04;	}
	bool WholeBodyItem( void )	{	return (flag4&0x08) == 0x08;	}
	bool WallRoofWeap( void )	{	return (flag4&0x10) == 0x10;	}
	bool Door( void )			{	return (flag4&0x20) == 0x20;	}
	bool ClimbableBit1( void )	{	return (flag4&0x40) == 0x40;	}
	bool ClimbableBit2( void )	{	return (flag4&0x80) == 0x80;	}
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
protected:
	SI32 unknown1;  // longs must go at top to avoid bus errors - fur
	SI32 animation;
	UI08 weight;
	SI08 layer;
	SI08 unknown2;
	SI08 unknown3;
	SI08 height;
	SI08 name[23];	// manually padded to long to avoid bus errors - fur

public:
	CTile() : unknown1( 0 ), unknown2( 0 ), unknown3( 0 ), weight( 0 ), height( 0 ), layer( 0 ), animation( 0 ) { name[0] = 0; }
	CTile( UOXFile *toRead ) { Read( toRead ); }
	CTile( std::ifstream &toRead );
	void Read( UOXFile *toRead );
	SI32 Unknown1( void )	{	return unknown1;	}
	SI32 Animation( void )	{	return animation;	}
	UI08 Weight( void )		{	return weight;		}
	SI08 Layer( void )		{	return layer;		}
	SI08 Unknown2( void )	{	return unknown2;	}
	SI08 Unknown3( void )	{	return unknown3;	}
	SI08 Height( void )		{	return height;		}
	char *Name( void )		{	return (char *)name;}

	void Unknown1( SI32 newVal )	{	unknown1 = newVal;		}
	void Animation( SI32 newVal )	{	animation = newVal;		}
	void Weight( UI08 newVal )		{	weight = newVal;		}
	void Layer( SI08 newVal )		{	layer = newVal;			}
	void Unknown2( SI08 newVal )	{	unknown2 = newVal;		}
	void Unknown3( SI08 newVal )	{	unknown3 = newVal;		}
	void Height( SI08 newVal )		{	height = newVal;		}
	void Name( char *newVal )		{	strcpy( (char *)name, (char *)newVal );	}

};

class CLand : public BaseTile
{
protected:
	SI08 unknown1;
	SI08 unknown2;
	SI08 name[20];
public:
	CLand() : unknown1( 0 ), unknown2( 0 ) { name[0] = 0; }
	CLand( std::ifstream &toRead );
	CLand( UOXFile *toRead ) { Read( toRead ); }
	void	Read( UOXFile *toRead );

	char	Unknown1( void )	{	return unknown1;		}
	char	Unknown2( void )	{	return unknown2;		}
	char *	Name( void )		{	return (char *)name;	}

	void	Unknown1( char newVal )			{	unknown1 = newVal;				}
	void	Unknown2( char newVal )			{	unknown2 = newVal;				}
	void	Name( char *newVal )			{	strcpy( (char *)name, newVal );	}
};

class CTileUni : public BaseTile
{
protected:
	SI08 basez;
	char mType;		// 0 = Terrain, 1 = Item
	UI16 id;
	SI08 height;
	UI08 weight;

public:
			CTileUni() : basez( 0 ), mType( 0 ), id( 0 ), height( 0 ), weight( 0 ) { }

	SI08	BaseZ( void ) const		{ return basez;		}
	char	Type( void ) const		{ return mType;		}
	UI16	ID( void ) const		{ return id;		}
	SI08	Height( void ) const	{ return height;	}
	UI08	Weight( void ) const	{ return weight;	}

	void	BaseZ( SI08 nVal )		{ basez = nVal;		}
	void	Type( char nVal )		{ mType = nVal;		}
	void	ID( UI16 nVal )			{ id = nVal;		}
	void	Height( SI08 nVal )		{ height = nVal;	}
	void	Weight( UI08 nVal )		{ weight = nVal;	}
	CTileUni	&operator=( CLand &toCopy )
	{
		Flag1( toCopy.Flag1() );
		Flag2( toCopy.Flag2() );
		Flag3( toCopy.Flag3() );
		Flag4( toCopy.Flag4() );
		return (*this);
	}
	CTileUni	&operator=( CTile &toCopy )
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

#endif

