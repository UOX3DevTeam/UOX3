#ifndef __CSKILLCLASS_H__
#define __CSKILLCLASS_H__

#include <algorithm>
using namespace std;

class cSkillClass
{
public:
	int skill;
	int value;

	cSkillClass( int nskill, int nvalue ) : skill(nskill), value(nvalue) {}
};

inline bool operator==(const cSkillClass& x, const cSkillClass& y)
{
    return ( x.value == y.value );
}

inline bool operator<(const cSkillClass& x, const cSkillClass& y)
{
	return ( ( x.value == y.value ) ? ( x.skill < y.skill ) : ( x.value < y.value ) );
}

inline bool operator>(const cSkillClass& x, const cSkillClass& y)
{
	return ( ( x.value == y.value ) ? ( x.skill > y.skill ) : ( x.value > y.value ) );
}

#endif

