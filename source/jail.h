
#ifndef __JAIL_H__
#define __JAIL_H__

struct JailOccupant_st
{
	SERIAL pSerial;
	SI16 x;
	SI16 y;
	SI08 z;
	UI08 world;
	UI16 instanceId;
	time_t releaseTime;
	JailOccupant_st() : pSerial( INVALIDSERIAL ), x( 0 ), y( 0 ), z( 0 ), world( 0 ), instanceId( 0 ), releaseTime( 0 )
	{
	}
};

class CJailCell
{
private:
	SI16 x;
	SI16 y;
	SI08 z;
	UI08 world;
	UI16 instanceId;
	std::vector<JailOccupant_st *> playersInJail;
public:
	CJailCell() : x( 0 ), y( 0 ), z( 0 ), world( 0 ), instanceId( 0 )
	{
	}
	~CJailCell();
	bool	IsEmpty( void ) const;
	size_t	JailedPlayers( void ) const;
	SI16	X( void ) const;
	SI16	Y( void ) const;
	SI08	Z( void ) const;
	UI08	World( void ) const;
	UI16	InstanceId( void ) const;
	void	X( SI16 nVal );
	void	Y( SI16 nVal );
	void	Z( SI08 nVal );
	void	World( UI08 nVal );
	void	InstanceId( UI16 nVal );
	void	AddOccupant( CChar *pAdd, SI32 secsFromNow );
	void	EraseOccupant( size_t occupantId );
	JailOccupant_st *Occupant( size_t occupantId );
	void	PeriodicCheck( void );
	void	AddOccupant( JailOccupant_st *toAdd );
	void	WriteData( std::ofstream &outStream, size_t cellNum );
};

class CJailSystem
{
private:
	std::vector<CJailCell> jails;
public:
	CJailSystem() = default;
	~CJailSystem() = default;
	void	ReadSetup( void );
	void	ReadData( void );
	void	WriteData( void );
	void	PeriodicCheck( void );
	bool	JailPlayer( CChar *toJail, SI32 numSecsToJail );
	void	ReleasePlayer( CChar *toRelase );
};

extern CJailSystem *JailSys;

#endif

