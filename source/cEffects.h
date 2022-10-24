#ifndef __CEFFECTS_H__
#define __CEFFECTS_H__

class cEffects
{
private:
	void	HandleMakeItemEffect( CTEffect *tMake );
public:
	void	TempEffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr = nullptr);
	void	TempEffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 );
	void	CheckTempeffects( void );
	void	SaveEffects( void );
	void	LoadEffects( void );

	// Visual Effects
	void	DeathAction( CChar *s, CItem *x, UI08 fallDirection );
	CItem *	SpawnBloodEffect( UI08 worldNum, UI16 instanceId, UI16 bloodColour, BloodTypes bloodType );
	void	PlayStaticAnimation( CBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode = false );
	void	PlayStaticAnimation( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode );
	void	PlayMovingAnimation( CBaseObject *source, CBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0, bool playLocalMoveFX = false );
	void	PlayMovingAnimation( SI16 srcX, SI16 srcY, SI08 srcZ, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	PlayMovingAnimation( CBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 dwHue = 0, UI32 dwRenderMode = 0 );
	void	PlayCharacterAnimation( CChar *mChar, UI16 actionId, UI08 frameDelay = 0, UI08 frameCount = 7, bool playBackwards = false );
	void	PlayNewCharacterAnimation( CChar *mChar, UI16 actionId, UI16 subActionId = 0, UI08 subSubActionId = 0 );
	void	PlaySpellCastingAnimation( CChar *mChar, UI16 actionId, bool monsterCast, bool areaCastAnim );
	void	Bolteffect( CChar *player );

	// Sound Effects
	void	PlaySound( CSocket *mSock, UI16 soundId, bool allHear );
	void	PlaySound( CBaseObject *baseObj, UI16 soundId, bool allHear = true );
	void	PlayBGSound( CSocket& mSock, CChar& mChar );
	void	ItemSound( CSocket *s, CItem *item, bool bAllHear = false );
	void	GoldSound( CSocket *s, UI32 goldTotal, bool bAllHear = false);
	void	PlayTileSound( CChar *mChar, CSocket *mSock );
	void	PlayDeathSound( CChar *i );
	void	PlayMusic( CSocket *s, UI16 number );
	void	DoSocketMusic( CSocket *s );
};

extern cEffects *Effects;

#endif
