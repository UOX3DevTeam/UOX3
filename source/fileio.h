
#ifndef __FILEIO_H
#define __FILEIO_H

class BinBuffer;
class CLand;
class CTile;

class UOXFile
{
private:

	int			fmode, bSize, bIndex;
	bool		ok;
	UI08 *		ioBuff;
	FILE *		theFile;

public:

				UOXFile( const char *, char * );
	virtual		~UOXFile();

	inline int	qRefill( void ) { return( bIndex >= bSize ); };
	inline int	ready( void ) { return( ok ); };
	void		rewind( void );
	void		seek( long, int );
	inline int	eof( void ) { return ( feof( theFile ) ); };
	int			getch( void );
	void		refill( void );

	char *		gets( char *, int );
	int			puts( char * );
	
	void		getUChar( UI08 *, UI32 );
	void		getUChar( UI08 *c ) { getUChar( (UI08 *) c, 1 ); }

	void		getChar( SI08 *, UI32 );
	void		getChar( SI08 *c) { getChar( c, 1 ); }

	void		getUShort( UI16 *, UI32 = 1 );
	void		getShort( SI16 *, UI32 = 1 );
	void		getULong( UI32 *, UI32 = 1 );
	void		getLong( SI32 *, UI32 = 1 );

	void		get_versionrecord( struct versionrecord *, UI32 = 1 );
	void		get_st_multi( struct st_multi *, UI32 = 1 );
	void		get_land_st( CLand *, UI32 = 1 );
	void		get_tile_st( CTile *, UI32 = 1 );
	void		get_map_st( struct map_st *, UI32 = 1 );
	void		get_st_multiidx( struct st_multiidx *, UI32 = 1 );
	void		get_staticrecord( struct staticrecord *, UI32 = 1 );

	int			getLength( void );
};

class BinBuffer
{
public:
	BinBuffer();
	BinBuffer( std::ifstream & );
	~BinBuffer();

	void Read( std::ifstream & );
	void Write( std::ofstream & );
	void Reset( void );

	UI32 Size( void );
	UI08 GetType( void );
	void SetType( UI08 );

	UI08 GetByte( void );
	void PutByte( UI08 );
	SI16 GetShort( void );
	void PutShort( SI16 );
	SI32 GetLong( void );
	void PutLong( SI32 );
	R32 GetFloat( void );
	void PutFloat( R32 );

	void PutStr( const char * );
	int GetStr( char *, int = 255 );

	void Put( const void *, int );
	void Get( void *, int );

	bool End( void );

	int Position( void );
	int Length( void );

protected:
	std::vector<char> Buff;
	int fp;
	UI08 myType;
};

class cFileIO
{
	void	initCreatures( void );
public:
	void	LoadNewWorld( void );
	void	LoadCustomTitle( void );
	void	LoadSkills( void );
	void	LoadPreDefSpawnRegion( UI16 r, std::string name );
	void	LoadSpawnRegions( void );
	void	LoadRegions( void );
	void	LoadTeleportLocations( void );
	void	LoadCreatures( void );
	void	LoadINIFile( void );
};

#endif
