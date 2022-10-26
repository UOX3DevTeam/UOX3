#ifndef __WEIGHT_H__
#define __WEIGHT_H__

const SI32 MAX_WEIGHT = 6553500;	// Max weight (This number / 100, thus max actual weight is 65535.00)

class CWeight
{
private:
	bool	CalcAddWeight( CItem *item, SI32 &totalWeight );
	bool	CalcSubtractWeight( CItem *item, SI32 &totalWeight );
	bool	IsWeightedContainer( CItem *toCheck );

public:
	SI32	CalcCharWeight( CChar *mChar );
	SI32	CalcWeight( CItem *pack );

	bool	IsOverloaded( CChar *mChar ) const;
	bool	CheckPackWeight( CChar *ourChar, CItem *pack, CItem *item );
	bool	CheckCharWeight( CChar *ourChar, CChar *mChar, CItem *item, UI16 amount = 0 );

	void	AddItemWeight( CBaseObject *getObj, CItem *item );
	void	AddItemWeight( CChar *mChar, CItem *item );
	void	AddItemWeight( CItem *pack, CItem *item );

	void	SubtractItemWeight( CBaseObject *getObj, CItem *item );
	void	SubtractItemWeight( CChar *mChar, CItem *item );
	void	SubtractItemWeight( CItem *pack, CItem *item );
};

extern CWeight *Weight;

#endif

