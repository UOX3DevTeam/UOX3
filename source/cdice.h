#ifndef __CDICE__
#define __CDICE__

#include <string>
//using namespace std;

class cDice
{
private:
	int		dice;
	int		sides;
	int		addition;

	bool	convStringToDice( std::string dieString );
public:
			cDice();
			cDice( std::string dieString );
			cDice( int d, int s, int a );
	virtual	~cDice();

	int		roll( void );
	void	setDice( int newDice );
	void	setSides( int newSides );
	void	setAddition( int newAddition );
};

#endif

