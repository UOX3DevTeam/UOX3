#ifndef __SKILLS_H__
#define __SKILLS_H__

typedef void (TargetFunc)( cSocket *s );

struct resourceEntry
{
	SI16	oreAmt;
	int		oreTime;
	SI16	logAmt;
	int		logTime;
};
class cSkills
{
private:
	vector< miningData >			ores;
	map< UI16, createMenu >			actualMenus;
	map< UI16, createMenuEntry >	skillMenus;
	map< UI16, createEntry >		itemsForMenus;
	resourceEntry					resources[610][410];
private:

	void	RegenerateOre( long grX, long grY );
	void	RegenerateLog( long grX, long grY );

#ifndef __LINUX__
	TargetFunc Hide;
	TargetFunc Stealth;
	TargetFunc PeaceMaking;
	TargetFunc RandomSteal;
	TargetFunc Meditation;
#else
	void Hide(cSocket *s);
	void Stealth(cSocket *s);
	void PeaceMaking(cSocket *s);
	void RandomSteal(cSocket *s);
	void Meditation(cSocket *s);
#endif

	SI08 FindSkillPoint( int sk, int value );
	void AnvilTarget( cSocket *s, CItem& item, SI16 oreType );
	void Atrophy( CChar *c, UI08 sk );
	void PlayInstrumentWell( cSocket *s, CItem *i);
	void PlayInstrumentPoor( cSocket *s, CItem *i);
	CItem *GetInstrument( cSocket *s );
	UI08 TrackingDirection( cSocket *s, CChar *i );
	void Zero_Itemmake( cSocket *s );

	bool LoadMiningData( void );
	void LoadCreateMenus( void );
	void LoadResourceData( void );
	bool AdvanceSkill( CChar *s, int sk, bool skillused );

public:
			cSkills( void );
	virtual	~cSkills( void );

	SI32 CalcRank( cSocket *s, int skill );
	SI32 CalcRankAvg( CChar *player, createEntry& skillMake );

#ifndef __LINUX__
	TargetFunc AlchemyTarget;
	TargetFunc AnatomyTarget;
	TargetFunc AnimalLoreTarget;
	TargetFunc ArmsLoreTarget;
	TargetFunc BeggingTarget;
	TargetFunc BottleTarget;
	TargetFunc BowCraft;
	TargetFunc Carpentry;
	TargetFunc CookMeat;
	TargetFunc CreateBandageTarget;
	TargetFunc DetectHidden;
	TargetFunc EnticementTarget1;
	TargetFunc EnticementTarget2;
	TargetFunc EvaluateIntTarget;
	TargetFunc FishTarget;
	TargetFunc Fletching;
	TargetFunc ForensicsTarget;
	TargetFunc GraveDig;
	TargetFunc HealingSkillTarget;
	TargetFunc ItemIDTarget;
	TargetFunc LockPick;
	TargetFunc Loom;
	TargetFunc MakeDough;
	TargetFunc MakePizza;
	TargetFunc Mine;
	TargetFunc Persecute;
	TargetFunc PoisoningTarget;
	TargetFunc ProvocationTarget1;
	TargetFunc ProvocationTarget2;
	TargetFunc RepairMetal;
	TargetFunc RepairLeather;
	TargetFunc RepairBow;
	TargetFunc SmeltOre;
	TargetFunc Smith;
	TargetFunc SpiritSpeak;
	TargetFunc StealingTarget;
	TargetFunc Tailoring;
	TargetFunc TameTarget;
	TargetFunc TDummy;
	TargetFunc TinkerAwg;
	TargetFunc TinkerAxel;
	TargetFunc TinkerClock;
	TargetFunc Tinkering;
	TargetFunc TreeTarget;
#else
	void AlchemyTarget(cSocket *s);
	void AnatomyTarget(cSocket *s);
	void AnimalLoreTarget(cSocket *s);
	void ArmsLoreTarget(cSocket *s);
	void BeggingTarget(cSocket *s);
	void BottleTarget(cSocket *s);
	void BowCraft(cSocket *s);
	void Carpentry(cSocket *s);
	void CookMeat(cSocket *s);
	void CreateBandageTarget(cSocket *s);
	void DetectHidden(cSocket *s);
	void EnticementTarget1(cSocket *s);
	void EnticementTarget2(cSocket *s);
	void EvaluateIntTarget(cSocket *s);
	void FishTarget(cSocket *s);
	void Fletching(cSocket *s);
	void ForensicsTarget(cSocket *s);
	void GraveDig(cSocket *s);
	void HealingSkillTarget(cSocket *s);
	void ItemIDTarget(cSocket *s);
	void LockPick(cSocket *s);
	void Loom(cSocket *s);
	void MakeDough(cSocket *s);
	void MakePizza(cSocket *s);
	void Mine(cSocket *s);
	void Persecute(cSocket *s);
	void PoisoningTarget(cSocket *s);
	void ProvocationTarget1(cSocket *s);
	void ProvocationTarget2(cSocket *s);
	void RepairMetal( cSocket *s );
	void RepairLeather( cSocket *s );
	void RepairBow( cSocket *s );
	void SmeltOre(cSocket *s);
	void Smith(cSocket *s);
	void SpiritSpeak(cSocket *s);
	void StealingTarget(cSocket *s);
	void Tailoring(cSocket *s);
	void TameTarget(cSocket *s);
	void TDummy(cSocket *s);
	void TinkerAwg(cSocket *s);
	void TinkerAxel(cSocket *s);
	void TinkerClock(cSocket *s);
	void Tinkering(cSocket *s);
	void TreeTarget(cSocket *s);
#endif

	void SaveResources( void );

	createEntry *FindItem( UI16 itemNum );
	void MakeItem( createEntry &toMake, CChar *player, cSocket *sock, UI16 itemEntry );
	void ApplyRank( cSocket *s, CItem *c, int rank );

	void HandleMakeMenu( cSocket *s, int button, int menu );
	void Load( void );
	void Tracking( cSocket *s, int selection );
	void CreatePotion( CChar *s, char type, char sub, CItem *mortar );
	void Fish( CChar *i );
	void AdvanceStats( CChar *s, int sk, bool skillsuccess );

	void Track( CChar *i );
	void DoPotion( cSocket *s, int type, int sub, CItem *mortar);
	void MakeMenuTarget( cSocket *s, string x, int skill );
	void NewMakeMenu( cSocket *s, int menu, UI08 skill );
	void Wheel( cSocket *s );
	void PotionToBottle( CChar *s, CItem *mortar );
	bool CheckSkill( CChar *s, UI08 sk, int lowSkill, int highSkill );
	void SkillUse( cSocket *s, int x );
	void CreateTrackingMenu( cSocket *s, int m);
	void TrackingMenu( cSocket *s, int gmindex);
	void Inscribe( cSocket *s, long snum );
	bool EngraveAction( cSocket *s, CItem *i, int getCir, int getSpell );
	void updateSkillLevel( CChar *c, int s);
	void AButte( cSocket *s, CItem *x );

	int			GetNumberOfOres( void );
	miningData *GetOre( int number );
	miningData *GetOre( string name );
	int			GetOreIndex( string name );
	void		MakeOre( int Region, CChar *actor, cSocket *s );
	SI16 FindOreType( UI16 colour );
	void Snooping( cSocket *s, CChar *target, SERIAL serial );
	SI16 calcStealDiff( CChar *c, CItem *i );
};

#endif

