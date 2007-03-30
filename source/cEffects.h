#ifndef __CEFFECTS_H__
#define __CEFFECTS_H__

namespace UOX
{

class cEffects
{
private:
	void	HandleMakeItemEffect( CTEffect *tMake );
public:
	void	tempeffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr = NULL);
	void	tempeffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 );
	void	checktempeffects( void );
	void	SaveEffects( void );
	void	LoadEffects( void );

	// Visual Effects
	void	deathAction( CChar *s, CItem *x, UI08 fallDirection );
	void	PlayStaticAnimation( CBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode = false );
	void	PlayStaticAnimation( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode );
	void	PlayMovingAnimation( CBaseObject *source, CBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	PlayMovingAnimation( CBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	PlayCharacterAnimation( CChar *mChar, UI16 actionID, UI08 frameDelay = 0 );
	void	PlaySpellCastingAnimation( CChar *mChar, UI16 actionID );
	void	bolteffect( CChar *player );

	// Sound Effects
	void	PlaySound( CSocket *mSock, UI16 soundID, bool allHear );
	void	PlaySound( CBaseObject *baseObj, UI16 soundID, bool allHear = true );
	void	PlayBGSound( CSocket& mSock, CChar& mChar );
	void	itemSound( CSocket *s, CItem *item, bool bAllHear = false );
	void	goldSound( CSocket *s, UI32 goldtotal, bool bAllHear = false);
	void	playTileSound( CSocket *mSock );
	void	playDeathSound( CChar *i );
	void	playMidi( CSocket *s, UI16 number );
	void	dosocketmidi( CSocket *s );
};

extern cEffects *Effects;

}

#endif
