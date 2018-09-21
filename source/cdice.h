#ifndef __CDICE__
#define __CDICE__

namespace UOX
{

class cDice
{
private:
	int		dice;
	int		sides;
	int		addition;

	bool	convStringToDice( std::string dieString );
public:
			cDice();
			cDice( const std::string &dieString );
			cDice( int d, int s, int a );
			~cDice();

	int		roll( void );
	void	setDice( int newDice );
	void	setSides( int newSides );
	void	setAddition( int newAddition );
};

}

#endif
