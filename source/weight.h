#ifndef __WEIGHT_H__
#define __WEIGHT_H__

class cWeight
{
private:
	bool	calcAddWeight( CItem *item, SI32 &totalWeight );
	bool	calcSubtractWeight( CItem *item, SI32 &totalWeight );

public:
	SI32	calcCharWeight( CChar *mChar );
	SI32	calcWeight( CItem *pack );

	bool	isOverloaded( CChar *mChar );
	bool	checkPackWeight( CItem *pack, CItem *item );

	void	addItemWeight( cBaseObject *getObj, CItem *item );
	void	addItemWeight( CChar *mChar, CItem *item );
	void	addItemWeight( CItem *pack, CItem *item );

	void	subtractItemWeight( cBaseObject *getObj, CItem *item );
	void	subtractItemWeight( CChar *mChar, CItem *item );
	void	subtractItemWeight( CItem *pack, CItem *item );
};

#endif

