#ifndef __CDICE__
#define __CDICE__
#include <cstdint>
#include <string>

#include "typedefs.h"
class cDice
{
private:
	SI32	dice;
	SI32	sides;
	SI32	addition;

	bool	convStringToDice( std::string dieString );
public:
	cDice();
	cDice( const std::string &dieString );
	cDice( SI32 d, SI32 s, SI32 a );
	~cDice();

	SI32	RollDice( void );
	void	SetDice( SI32 newDice );
	void	SetSides( SI32 newSides );
	void	SetAddition( SI32 newAddition );
};

#endif
