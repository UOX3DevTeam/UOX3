#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__

namespace UOX
{

class CMultiObj : public CItem
{
protected:
	std::string			deed;

	ITEMLIST			itemInMulti;
	CHARLIST			charInMulti;
	CHARLIST			owners;
	CHARLIST			banList;
	ITEMLIST			lockedList;
	UI16				maxLockedDown;

	ITEMLIST_ITERATOR	itemMultiIterator;
	CHARLIST_ITERATOR	charMultiIterator;

	bool				CharIsInMulti( CChar *toFind ) const;
	bool				ItemIsInMulti( CItem *toFind ) const;

	void				RemoveCharFromMulti( CChar *toFind );
	void				RemoveItemFromMulti( CItem *toFind );

	void				AddCharToMulti( CChar *toFind );
	void				AddItemToMulti( CItem *toFind );

	virtual bool		LoadRemnants( void );

public:
						CMultiObj();
	virtual				~CMultiObj();

	void				AddToBanList( CChar *toBan );
	void				RemoveFromBanList( CChar *toRemove );

	void				AddAsOwner( CChar *newOwner );
	void				RemoveAsOwner( CChar *toRemove );
	void				ClearOwners( void );

	bool				IsInMulti( cBaseObject *toFind ) const;
	bool				IsOnBanList( CChar *toBan ) const;
	bool				IsOwner( CChar *toFind ) const;

	void				AddToMulti( cBaseObject *toAdd );
	void				RemoveFromMulti( cBaseObject *toRemove );

	size_t				GetLockDownCount( void ) const;
	UI32				GetMaxLockDowns( void ) const;
	void				SetMaxLockDowns( UI16 newValue );
	void				LockDownItem( CItem *toLock );
	void				RemoveLockDown( CItem *toRemove );

	virtual bool		Save( std::ofstream &outStream ) const;
	virtual bool		DumpHeader( std::ofstream &outStream ) const;
	virtual bool		DumpBody( std::ofstream &outStream ) const;
	virtual bool		HandleLine( UString &UTag, UString &data );

	CChar *				FirstCharMulti( void );
	CChar *				NextCharMulti( void );
	bool				FinishedCharMulti( void ) const;
	UI32					NumCharMulti( void ) const;

	CItem *				FirstItemMulti( void );
	CItem *				NextItemMulti( void );
	bool				FinishedItemMulti( void ) const;
	UI32				NumItemMulti( void ) const;

	virtual void		SetOwner( CChar *newOwner );

	std::string			GetDeed( void ) const;
	void				SetDeed( std::string newDeed );

	virtual void		Cleanup( void );

	virtual bool		CanBeObjType( ObjectType toCompare ) const;
};

class CBoatObj : public CMultiObj
{
protected:
	SERIAL				tiller;
	SERIAL				planks[2];
	SERIAL				hold;
	UI08				moveType;

	TIMERVAL			nextMoveTime;

private:
	virtual bool		DumpHeader( std::ofstream &outStream ) const;
	virtual bool		DumpBody( std::ofstream &outStream ) const;

	virtual bool		HandleLine( UString &UTag, UString &data );

public:
						CBoatObj();
	virtual				~CBoatObj();

	SERIAL				GetTiller( void ) const;
	SERIAL				GetPlank( UI08 plankNum ) const;
	SERIAL				GetHold( void ) const;
	UI08				GetMoveType( void ) const;

	void				SetPlank( UI08 plankNum, SERIAL newVal );
	void				SetTiller( SERIAL newVal );
	void				SetHold( SERIAL newVal );
	void				SetMoveType( UI08 newVal );

	TIMERVAL			GetMoveTime( void ) const;
	void				SetMoveTime( TIMERVAL newVal );

	virtual bool		CanBeObjType( ObjectType toCompare ) const;
};

}

#endif

