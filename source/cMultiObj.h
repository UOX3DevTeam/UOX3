#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__


class CMultiObj : public CItem
{
protected:
	std::string			deed;

	std::map< CChar *, UI08 >	housePrivList;

	ITEMLIST			lockedList;
	UI16				maxLockedDown;

	virtual bool		LoadRemnants( void ) override;

	CDataList< CItem * >	itemInMulti;
	CDataList< CChar * >	charInMulti;

public:
	CMultiObj();
	virtual				~CMultiObj();

	void				AddToBanList( CChar *toBan );
	void				RemoveFromBanList( CChar *toRemove );

	void				AddAsOwner( CChar *newOwner );
	void				RemoveAsOwner( CChar *toRemove );

	bool				IsOnBanList( CChar *toBan ) const;
	bool				IsOwner( CChar *toFind ) const;

	void				AddToMulti( CBaseObject *toAdd );
	void				RemoveFromMulti( CBaseObject *toRemove );

	size_t				GetLockDownCount( void ) const;
	UI32				GetMaxLockDowns( void ) const;
	void				SetMaxLockDowns( UI16 newValue );
	void				LockDownItem( CItem *toLock );
	void				RemoveLockDown( CItem *toRemove );

	virtual bool		Save( std::ofstream &outStream ) override;
	virtual bool		DumpHeader( std::ofstream &outStream ) const override;
	virtual bool		DumpBody( std::ofstream &outStream ) const override;
	virtual bool		HandleLine( UString &UTag, UString &data ) override;

	virtual void		SetOwner( CChar *newOwner ) override;

	std::string			GetDeed( void ) const;
	void				SetDeed( const std::string &newDeed );

	virtual void		Cleanup( void ) override;

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;

	CDataList< CChar * > *	GetCharsInMultiList( void );
	CDataList< CItem * > *	GetItemsInMultiList( void );
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
	virtual bool		DumpHeader( std::ofstream &outStream ) const override;
	virtual bool		DumpBody( std::ofstream &outStream ) const override;

	virtual bool		HandleLine( UString &UTag, UString &data ) override;

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

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;
};

#endif

