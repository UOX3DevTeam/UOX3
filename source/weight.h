#ifndef __WEIGHT_H__
#define __WEIGHT_H__

class cWeight
{
private:
	R64		RecursePacks( CItem *pack );
	bool	checkMaxPackWeight( CChar *c );
public:
	bool	checkWeight( CChar *s, bool isTele );
	void	calcWeight( CChar *p );
	R64		calcPackWeight( CItem *pack );
	SI32	calcItemWeight( CItem *item );
	void	AddItemWeight( CItem *i, CChar *s );
	void	SubtractItemWeight( CItem *i, CChar *s );
};

#endif

