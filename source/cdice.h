#ifndef __CDICE__
#define __CDICE__

#include <string>
#include "Prerequisites.h"
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

	SI32	roll( void );
	void	setDice( SI32 newDice );
	void	setSides( SI32 newSides );
	void	setAddition( SI32 newAddition );
};

#endif
