#ifndef __XGM_H_
#define __XGM_H_

namespace UOX
{

const UI32 XGMVersion = calcserial( 0, 0, 1, 0 );

enum XGMServerStat
{
	XSS_ONLINECOUNT = 0,
	XSS_CHARACTERCOUNT,
	XSS_ITEMCOUNT,
	XSS_PINGRESPONSE,
	XSS_ACCOUNTCOUNT,
	XSS_SIMULATIONCYCLES,
	XSS_RACECOUNT,
	XSS_REGIONCOUNT,
	XSS_WEATHERCOUNT,
	XSS_TIMESINCERESTART,
	XSS_TIMESINCEWORLDSAVE,
	XSS_LASTPLAYERLOGGEDIN,
	XSS_GMPAGECOUNT,
	XSS_CNSPAGECOUNT,
	XSS_GHOSTCOUNT,
	XSS_MURDERERCOUNT,
	XSS_BLUECOUNT,
	XSS_HOUSECOUNT,
	XSS_TOTALGOLD,
	XSS_PEAKCONNECTION,
	XSS_GMSLOGGEDIN,
	XSS_CNSLOGGEDIN
};

class cPXGMLoginResponse : public cPXGMBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPXGMLoginResponse();
						cPXGMLoginResponse( UI08 error, UI08 clearance );
	virtual void		Error( UI08 error );
	virtual void		Clearance( UI08 clearance );
};

class cPXGMServerStatResponse : public cPXGMBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPXGMServerStatResponse();
	virtual ~			cPXGMServerStatResponse();
						cPXGMServerStatResponse( XGMServerStat error );
	virtual void		ServerStat( XGMServerStat error );
	virtual void		GenericData( UI32 generic );
};

class cPXGMServerVerResponse : public cPXGMBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPXGMServerVerResponse();
						cPXGMServerVerResponse( UI32 version );
	virtual void		Version( UI32 generic );
};

class cPXGMWhoResponse : public cPXGMBuffer
{
protected:
	virtual void		InternalReset( void );
	UI16				playerNum;
public:
						cPXGMWhoResponse();
						cPXGMWhoResponse( UI16 numPlayers );
	virtual void		NumEntries( UI16 numPlayers );
	virtual void		AddPlayer( SERIAL serToAdd, const char *toAdd );
	virtual void		StatusType( UI08 statusType );
};

class cPXGMChangeLevelResponse : public cPXGMBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPXGMChangeLevelResponse();
						cPXGMChangeLevelResponse( UI08 error, UI08 clearance );
	virtual void		Error( UI08 error );
	virtual void		Clearance( UI08 clearance );
};

class cPIXGMLogin : public cPInputBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPIXGMLogin();
						cPIXGMLogin( CSocket *s );
	virtual void		Receive( void );
	virtual const char *Name( void );
	virtual const char *Pass( void );
	virtual UI08		Clearance( void );
	virtual bool		Handle( void );
};

class cPIXGMServerStat : public cPInputBuffer
{
protected:
	virtual void			InternalReset( void );
public:
							cPIXGMServerStat();
	virtual ~				cPIXGMServerStat();
							cPIXGMServerStat( CSocket *s );
	virtual void			Receive( void );
	virtual bool			Handle( void );
	virtual XGMServerStat	Stat( void );
};

class cPIXGMClientVer : public cPInputBuffer
{
protected:
	virtual void			InternalReset( void );
public:
							cPIXGMClientVer();
							cPIXGMClientVer( CSocket *s );
	virtual void			Receive( void );
	virtual bool			Handle( void );
};

class cPIXGMServerVer : public cPInputBuffer
{
public:
							cPIXGMServerVer();
							cPIXGMServerVer( CSocket *s );
	virtual void			Receive( void );
	virtual bool			Handle( void );
};

class cPIXGMWhoOnline : public cPInputBuffer
{
public:
							cPIXGMWhoOnline();
							cPIXGMWhoOnline( CSocket *s );
	virtual void			Receive( void );
	virtual bool			Handle( void );
};

class cPIXGMChangeLevel : public cPInputBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPIXGMChangeLevel();
						cPIXGMChangeLevel( CSocket *s );
	virtual void		Receive( void );
	virtual UI08		Clearance( void );
	virtual bool		Handle( void );
};

class cPIXGMShowQueueRequest : public cPInputBuffer
{
protected:
	virtual void		InternalReset( void );
public:
						cPIXGMShowQueueRequest();
						cPIXGMShowQueueRequest( CSocket *s );
	virtual void		Receive( void );
	virtual UI08		Queue( void ) const;
	virtual bool		Handle( void );
};

}

#endif
