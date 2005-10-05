
#ifndef __JAIL_H__
#define __JAIL_H__

namespace UOX
{

struct JailOccupant
{
	SERIAL pSerial;
	SI16 x;
	SI16 y;
	SI08 z;
	time_t releaseTime;
	JailOccupant() : pSerial( INVALIDSERIAL ), x( 0 ), y( 0 ), z( 0 ), releaseTime( 0 )
	{
	}
};

class JailCell
{
private:
	SI16 x;
	SI16 y;
	SI08 z;
	std::vector< JailOccupant * > playersInJail;
public:
			JailCell() : x( 0 ), y( 0 ), z( 0 )
			{
			}
			~JailCell();
	bool	IsEmpty( void ) const;
	size_t	JailedPlayers( void ) const;
	SI16	X( void ) const;
	SI16	Y( void ) const;
	SI08	Z( void ) const;
	void	X( SI16 nVal );
	void	Y( SI16 nVal );
	void	Z( SI08 nVal );
	void	AddOccupant( CChar *pAdd, SI32 secsFromNow ) ;
	void	EraseOccupant( size_t occupantID );
	JailOccupant *Occupant( size_t occupantID );
	void	PeriodicCheck( void );
	void	AddOccupant( JailOccupant *toAdd );
	void	WriteData( std::ofstream &outStream, size_t cellNum );
};

class JailSystem
{
private:
	std::vector< JailCell > jails;
public:
			JailSystem();
			~JailSystem();
	void	ReadSetup( void );
	void	ReadData( void );
	void	WriteData( void );
	void	PeriodicCheck( void );
	bool	JailPlayer( CChar *toJail, SI32 numSecsToJail );
	void	ReleasePlayer( CChar *toRelase );
};

extern JailSystem *JailSys;

}

#endif

