class teffect_st;

class cEffects
{
private:
	void	HandleMakeItemEffect( teffect_st *tMake );
public:
			cEffects();
	virtual ~cEffects();
	void	tempeffect( CChar *source, CChar *dest, SI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr = NULL);
	void	tempeffect( CChar *source, CItem *dest, SI08 num, UI16 more1, UI16 more2, UI16 more3 );
	void	tempeffectsoff( void );
	void	tempeffectson( void );
	void	checktempeffects( void );
	void	reverseEffect( UI16 i );
	void	SaveEffects( void );
	void	LoadEffects( void );

	// Visual Effects
	void	deathAction( CChar *s, CItem *x );
	void	staticeffect( cBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode = false );
	void	staticeffect( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode );
	void	movingeffect( cBaseObject *source, cBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	movingeffect( cBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	action( cSocket *s, SI16 x );
	void	impaction( cSocket *s, SI16 act );
	void	bolteffect( CChar *player );

	// Sound Effects
	void	PlaySound( cSocket *mSock, UI16 soundID, bool allHear );
	void	PlaySound( cBaseObject *baseObj, UI16 soundID, bool allHear = true );
	void	bgsound( CChar *s );
	void	itemSound( cSocket *s, CItem *item, bool bAllHear = false );
	void	doorSound( CItem *item, UI16 id, bool isOpen );
	void	goldSound( cSocket *s, UI32 goldtotal, bool bAllHear = false);
	void	playMonsterSound( CChar *monster, UI16 id, UI08 sfx );
	void	playTileSound( cSocket *mSock );
	void	playDeathSound( CChar *i );
	void	playMidi( cSocket *s, UI16 number );
	void	dosocketmidi( cSocket *s );
};


