#ifndef __FUNCDECLS_H__
#define __FUNCDECLS_H__

// Pack functions
ITEM	GetPackOwner( CHARACTER p );
CChar *	GetPackOwner( CItem *p );
ITEM	packitem( CHARACTER p );
CItem *	packitem( CChar *p );
void	sendbpitem( UOXSOCKET s, int i );

// Range check functions
bool	checkItemRange( UOXSOCKET s, int i, int distance );
bool	inrange( cBaseObject *a, cBaseObject *b, UI16 distance );
bool	iteminrange( UOXSOCKET s, int i, int distance );
bool	iteminrange( UOXSOCKET s, CItem *i, short distance );
bool	npcinrange( UOXSOCKET s, int i, int distance);  //check for horse distance...
bool	npcinrange( UOXSOCKET s, CChar *i, short distance );
bool	inbankrange( CHARACTER i );
bool	inrange1( UOXSOCKET a, UOXSOCKET b );
bool	inrange1p (CHARACTER a, CHARACTER b);
bool	inrange1p( CChar *a, CChar *b );
bool	inrange2( UOXSOCKET s, ITEM i );
bool	inrange2( UOXSOCKET s, CItem *i );

// Calculation functions (socket, char, item and so forth)
int		calcItemFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 );
int		calcItemFromSer( SERIAL ser ); // Added by Magius(CHE) (2)
CItem *	calcItemObjFromSer( SERIAL targSerial );
int		calcCharFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 );
int		calcCharFromSer( SERIAL ser );
CChar *	calcCharObjFromSer( SERIAL targSerial );
int		calcLastContainerFromSer( int ser ); // Magius(che) (2)

inline int RandomNum( int nLowNum, int nHighNum )
{
	if( nHighNum - nLowNum + 1 )
		return ((rand() % ( nHighNum - nLowNum + 1 )) + nLowNum );
	else
		return nLowNum;
};
inline UI32 calcserial( UI08 a1, UI08 a2, UI08 a3, UI08 a4 ) { return ( (a1<<24) + (a2<<16) + (a3<<8) + a4 ); }	
void	Shutdown( SI32 retCode );
void	setcharflag( CHARACTER c );

void	read2( FILE *toRead ); 
r2Data	readline( ifstream &toRead );
void	loadnewworld( void );
void	loadoldworld( void );
void	loadspawnregions( void );
void	loadregions( void );
// String related functions
void	splitline( void );
int		hexnumber( int countx );
int		makenumber( int countx );
void	setLastOn( UOXSOCKET s );
void	safeCopy( char *dest, const char *src, UI32 maxLen );
void	endmessage( int x );
void	consolebroadcast( char *txt );
void	gettokennum(char * s, int num);
void	setrandomname(int s, char * namelist);
int		hstr2num( char *s );
void	numtostr( int i, char *string );
void	hextostr( int i, char *string );
int		makenum2( char *s );

// Amount related
int		getamount(    CHARACTER s, UI16 realID );
int		getsubamount( ITEM p, UI16 realID );
SI32	delequan(     CHARACTER s, UI16 realID, int amount );
int		delesubquan(  ITEM p, UI16 realID, int amount);

void	clearalltrades( void );

int		FindItem( CChar *toFind, UI08 type );
int		SearchSubPackForItem( CItem *toSearch, UI08 type );
int		FindItem( CChar *toFind, UI16 itemID );
int		SearchSubPackForItem( CItem *toSearch, UI16 itemID );
int		FindItemOnLayer( CHARACTER toFind, UI08 layer );
CItem *	FindItemOnLayer( CChar *toFind, UI08 layer );
short	DeleteItemsFromChar( CChar *toFind, short int itemID );
short	DeleteItemsFromPack( CItem *item, short int itemID );
int		getamount( CChar *s, UI16 realID );
int		getsubamount( CItem *p, UI16 realID );
void	delequan( CChar *s, UI16 realID, int amount );
int		delesubquan( CItem *p, UI16 realID, int amount );
int		GetBankCount( CHARACTER p, UI16 itemID, UI16 colour = 0x0000 );
int		DeleBankItem( CHARACTER p, UI16 itemID, UI16 colour, int amt );

void	ReadWorldTagData( ifstream &inStream, char *tag, char *data );
ITEM	GetRootPack( ITEM p );

#endif