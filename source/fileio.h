#ifndef __FILEIO_H
#define __FILEIO_H


namespace UOX
{

class UOXFile
{
public:
	UOXFile( const char* const, const char* const);
	~UOXFile();

	inline int	ready( void )
	{
		return ( memPtr != NULL );
	};
	void		rewind( void )
	{
		bIndex = 0;
	}
	void		seek( long, int );
	int		eof( void )
	{
		return ( bIndex >=  fileSize );
	};
	int		getch( void );

//	char*		gets( char *, int );		// deprecated, never used
//	int		puts( char * );				// deprecated, never used
	
	void		getUChar( UI08 *, UI32 );
	void		getUChar( UI08 *c )
	{
		getUChar( (UI08 *) c, 1 );
	}

	void		getChar( SI08 *, UI32 );
	void		getChar( SI08 *c)
	{
		getChar( c, 1 );
	}

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

	int		getPosition()
	{
		return bIndex;
	}
	int		getLength( void )
	{
		return fileSize;
	}

private:
	char		*memPtr;		// ptr to the beginning of the files
	size_t	fileSize;	// file size
	int		bIndex;		// current position
};

}

#endif
