#ifndef __TEFFECT_H__
#define __TEFFECT_H__

class teffect_st
{
private:
	SERIAL	source;
	SERIAL	dest;
	UI32	expiretime;
	SI08	num;
	UI16	more1;
	UI16	more2;
	UI16	more3;
	bool	dispellable;
	cBaseObject * objptr;

public:
	cBaseObject *ObjPtr( void ) const		{	return objptr;				}
	bool	Dispellable( void ) const	{	return (dispellable == 1);	}
	UI32	ExpireTime( void ) const	{	return expiretime;			}
	SERIAL	Source( void ) const		{	return source;				}
	SERIAL	Destination( void ) const	{	return dest;				}
	SI08	Number( void ) const		{	return num;					}
	UI16	More1( void ) const			{	return more1;				}
	UI16	More2( void ) const			{	return more2;				}
	UI16	More3( void ) const			{	return more3;				}

	void	Source( SERIAL value )		{	source = value;				}
	void	Destination( SERIAL value )	{	dest = value;				}
	void	ExpireTime( UI32 value )	{	expiretime = value;			}
	void	Number( SI08 value )		{	num = value;				}
	void	More1( UI16 value )			{	more1 = value;				}
	void	More2( UI16 value )			{	more2 = value;				}
	void	More3( UI16 value )			{	more3 = value;				}
	void	Dispellable( bool value )	{	dispellable = value;		}
	void	ObjPtr( cBaseObject *value )		{	objptr = value;			}

	bool	Save( std::ofstream &effectDestination, SI32 mode ) const; // saves the current effect
	teffect_st() : source( INVALIDSERIAL ), dest( INVALIDSERIAL ), expiretime( 0 ), num( -1 ), more1( 0 ), more2( 0 ), more3( 0 ), dispellable( false ), objptr( NULL )
	{
	}
};

// This class is designed as a replacement for the teffect array (which is nasty, and too big)
// It'll grow and shrink as required, and will hopefully help reduce memory troubles
class cTEffect
{
private:
	std::vector< teffect_st * > internalData;
	UI16 effectCount;
	UI16 currentEffect;
	bool delFlag;

public:
	cTEffect();
	virtual ~cTEffect();
	teffect_st *First( void );				// returns the first entry
	teffect_st *Current( void );			// returns current entry
	teffect_st *Next( void );				// returns the next entry
	bool AtEnd( void );						// have we hit the end?
	bool Add( teffect_st toAdd );			// adds another entry
	bool DelCurrent( void );				// deletes the current entry
	UI16 Count( void );						// returns count of number of effects
	teffect_st *GrabSpecific( UI16 index );	// grabs a specific index
};

#endif

