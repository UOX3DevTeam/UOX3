#ifndef __CSKILLCLASS_H__
#define __CSKILLCLASS_H__

#include <algorithm>

class cSkillClass
{
public:
	UI08 skill;
	UI16 value;

	cSkillClass( UI08 nskill, UI16 nvalue ) : skill( nskill ), value( nvalue )
	{
	}
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

