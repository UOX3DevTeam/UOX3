#ifndef __TEFFECT_H__
#define __TEFFECT_H__

namespace UOX
{

class CTEffect
{
private:
	SERIAL			source;
	SERIAL			dest;
	UI32			expiretime;
	UI08			num;
	UI16			more1;
	UI16			more2;
	UI16			more3;
	bool			dispellable;
	CBaseObject *	objptr;
	UI16		assocScript;

public:
	UI16 AssocScript( void ) const
	{
		return assocScript;
	}
	CBaseObject *ObjPtr( void ) const
	{
		return objptr;
	}
	bool	Dispellable( void ) const
	{
		return (dispellable == 1);
	}
	UI32	ExpireTime( void ) const
	{
		return expiretime;
	}
	SERIAL	Source( void ) const
	{
		return source;
	}
	SERIAL	Destination( void ) const
	{
		return dest;
	}
	UI08	Number( void ) const
	{
		return num;
	}
	UI16	More1( void ) const
	{
		return more1;
	}
	UI16	More2( void ) const
	{
		return more2;
	}
	UI16	More3( void ) const
	{
		return more3;
	}

	void	Source( SERIAL value )
	{
		source = value;
	}
	void	Destination( SERIAL value )
	{
		dest = value;
	}
	void	ExpireTime( UI32 value )
	{
		expiretime = value;
	}
	void	Number( UI08 value )
	{
		num = value;
	}
	void	More1( UI16 value )
	{
		more1 = value;
	}
	void	More2( UI16 value )
	{
		more2 = value;
	}
	void	More3( UI16 value )
	{
		more3 = value;
	}
	void	Dispellable( bool value )
	{
		dispellable = value;
	}
	void	ObjPtr( CBaseObject *value )
	{
		objptr = value;
	}
	void	AssocScript( UI16 value )
	{
		assocScript = value;
	}

	bool	Save( std::ofstream &effectDestination ) const; // saves the current effect
	CTEffect() : source( INVALIDSERIAL ), dest( INVALIDSERIAL ), expiretime( 0 ), num( 0 ), more1( 0 ), more2( 0 ), more3( 0 ), dispellable( false ), objptr( NULL ), assocScript( 0 )
	{
	}
};

// This class is designed as a replacement for the teffect array (which is nasty, and too big)
// It'll grow and shrink as required, and will hopefully help reduce memory troubles
class CTEffectContainer
{
private:
	std::vector< CTEffect * >			internalData, backupKeep, backupRemove;
	std::vector< CTEffect * >::iterator	currentEffect;
	bool								isScanning;

public:
	CTEffectContainer();
	~CTEffectContainer();
	CTEffect *First( void );					// returns the first entry
	CTEffect *Current( void );					// returns current entry
	CTEffect *Next( void );						// returns the next entry
	bool AtEnd( void );							// have we hit the end?
	bool Add( CTEffect *toAdd );				// adds another entry
	UI16 Count( void );							// returns count of number of effects
	CTEffect *GrabSpecific( UI16 index );		// grabs a specific index
	CTEffect *CreateEffect( void );				// have our container get us a new one

	void StartQueue( void );					// We're queueing up now
	void QueueToKeep( CTEffect *toKeep );		// Queue it to keep
	void QueueToRemove( CTEffect *toRemove );	// Queue it to remove
	void Prune( void );							// Finish with the queues
};

extern CTEffectContainer *TEffects;

}

#endif

