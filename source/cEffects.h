#ifndef __CEFFECTS_H__
#define __CEFFECTS_H__

namespace UOX
{

enum monsterSound
{
	SND_STARTATTACK = 0,
	SND_IDLE,
	SND_ATTACK,
	SND_DEFEND,
	SND_DIE
};

class cEffects
{
private:
	void	HandleMakeItemEffect( CTEffect *tMake );
public:
			cEffects();
			~cEffects();
	void	tempeffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr = NULL);
	void	tempeffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 );
	void	checktempeffects( void );
	void	SaveEffects( void );
	void	LoadEffects( void );

	// Visual Effects
	void	deathAction( CChar *s, CItem *x );
	void	PlayStaticAnimation( cBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode = false );
	void	PlayStaticAnimation( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode );
	void	PlayMovingAnimation( cBaseObject *source, cBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	PlayMovingAnimation( cBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	PlayCharacterAnimation( CChar *mChar, UI16 actionID );
	void	PlaySpellCastingAnimation( CChar *mChar, UI16 actionID );
	void	bolteffect( CChar *player );

	// Sound Effects
	void	PlaySound( cSocket *mSock, UI16 soundID, bool allHear );
	void	PlaySound( cBaseObject *baseObj, UI16 soundID, bool allHear = true );
	void	bgsound( cSocket *mSock, CChar *s );
	void	itemSound( cSocket *s, CItem *item, bool bAllHear = false );
	void	doorSound( CItem *item, UI16 id, bool isOpen );
	void	goldSound( cSocket *s, UI32 goldtotal, bool bAllHear = false);
	void	playMonsterSound( CChar *monster, UI16 id, monsterSound sfx );
	void	playTileSound( cSocket *mSock );
	void	playDeathSound( CChar *i );
	void	playMidi( cSocket *s, UI16 number );
	void	dosocketmidi( cSocket *s );
};

extern cEffects *Effects;

}

#endif
