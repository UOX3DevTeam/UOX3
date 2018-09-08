#ifndef __FILEIO_H
#define __FILEIO_H


namespace UOX
{

class UOXFile
{
public:
	UOXFile( const char* const, const char* const );
	~UOXFile();

	inline int	ready( void ) const { return ( memPtr != NULL ); }
	void		rewind( void ) { bIndex = 0; }
	void		seek( size_t, UI08 );
	bool		eof( void ) const { return ( bIndex >= fileSize ); }
	int			getch( void );

	void		getUChar(	UI08 *, UI32 = 1 );
	void		getChar(	SI08 *, UI32 = 1 );
	void		getUShort(	UI16 *, UI32 = 1 );
	void		getShort(	SI16 *, UI32 = 1 );
	void		getULong(	UI32 *, UI32 = 1 );
	void		getLong(	SI32 *, UI32 = 1 );

	size_t		getPosition( void ) const { return bIndex; }
	size_t		getLength( void ) const { return fileSize; }

private:
	char		*memPtr;		// ptr to the beginning of the files
	size_t		fileSize;	// file size
	size_t		bIndex;		// current position
};

}

#endif
