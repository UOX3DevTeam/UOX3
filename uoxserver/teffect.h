#ifndef __TEFFECT_H__
#define __TEFFECT_H__

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
	UI16			assocScript;

public:
	CTEffect() : source( INVALIDSERIAL ), dest( INVALIDSERIAL ), expiretime( 0 ), num( 0 ), more1( 0 ), more2( 0 ), more3( 0 ), dispellable( false ), objptr( nullptr ), assocScript( 0xFFFF )
	{
	}

	UI16	AssocScript( void ) const		{	return assocScript;		}
	CBaseObject *ObjPtr( void ) const		{	return objptr;			}
	bool	Dispellable( void ) const		{	return dispellable;		}
	UI32	ExpireTime( void ) const		{	return expiretime;		}
	SERIAL	Source( void ) const			{	return source;			}
	SERIAL	Destination( void ) const		{	return dest;			}
	UI08	Number( void ) const			{	return num;				}
	UI16	More1( void ) const				{	return more1;			}
	UI16	More2( void ) const				{	return more2;			}
	UI16	More3( void ) const				{	return more3;			}

	void	Source( SERIAL value )			{	source = value;			}
	void	Destination( SERIAL value )		{	dest = value;			}
	void	ExpireTime( UI32 value )		{	expiretime = value;		}
	void	Number( UI08 value )			{	num = value;			}
	void	More1( UI16 value )				{	more1 = value;			}
	void	More2( UI16 value )				{	more2 = value;			}
	void	More3( UI16 value )				{	more3 = value;			}
	void	Dispellable( bool value )		{	dispellable = value;	}
	void	ObjPtr( CBaseObject *value )	{	objptr = value;			}
	void	AssocScript( UI16 value )		{	assocScript = value;	}

	bool	Save( std::ofstream &effectDestination ) const; // saves the current effect
};

#endif

