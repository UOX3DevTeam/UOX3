#ifndef __SKILLS_H__
#define __SKILLS_H__

#include "targeting.h"

struct createMenu
{
	std::vector< UI16 > itemEntries;
	std::vector< UI16 > menuEntries;
};
struct createMenuEntry
{
	UI16 targID;
	UI16 colour;
	std::string name;
	UI16 subMenu;
	createMenuEntry() : targID( 0 ), colour( 0 ), name( "" ), subMenu( 0 ) { }
};

struct resourceEntry
{
	SI16	oreAmt;
	int		oreTime;
	SI16	logAmt;
	int		logTime;
};

struct miningData
{
	UI16 colour;		// colour of the ore, for colour of ingot
	UI16 minSkill;		// minimum skill needed to make the ingot
	std::string name;		// name of the ingot: no need to be fixed, as we're loading it dynamically
	bool foreign;		// if not iron, then it can print out that it's a stranger ore when failing
	int makemenu;		// the makemenu required for making with
	UI08 minAmount;		// min number of ingots to load anything
};

class cSkills
{
private:
	std::vector< miningData >			ores;
	std::map< UI16, createMenu >		actualMenus;
	std::map< UI16, createMenuEntry >	skillMenus;
	std::map< UI16, createEntry >		itemsForMenus;
	resourceEntry						resources[610][410];
private:

	void	RegenerateOre( long grX, long grY );
	void	RegenerateLog( long grX, long grY );
	void	doStealing( cSocket *s, CChar *mChar, CChar *npc, CItem *item );
	SI16	calcStealDiff( CChar *c, CItem *i );

#if !defined(__unix__)
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

	SI08 FindSkillPoint( UI08 sk, int value );
	void AnvilTarget( cSocket *s, CItem& item, SI16 oreType );
	void Atrophy( CChar *c, UI08 sk );
	void PlayInstrument( cSocket *s, CItem *i, bool wellPlayed );
	CItem *GetInstrument( cSocket *s );
	UI08 TrackingDirection( cSocket *s, CChar *i );

	bool LoadMiningData( void );
	void LoadCreateMenus( void );
	void LoadResourceData( void );
	bool AdvanceSkill( CChar *s, UI08 sk, bool skillused );

public:
			cSkills( void );
	virtual	~cSkills( void );

	SI32 CalcRankAvg( CChar *player, createEntry& skillMake );

	TargetFunc AlchemyTarget;
	TargetFunc AnatomyTarget;
	TargetFunc AnimalLoreTarget;
	TargetFunc ArmsLoreTarget;
	TargetFunc BeggingTarget;
	TargetFunc BottleTarget;
	TargetFunc BowCraft;
	TargetFunc Carpentry;
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
	TargetFunc handleCooking;
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
	TargetFunc Inscribe;

	void Load( void );
	void SaveResources( void );

	void NewMakeMenu( cSocket *s, int menu, UI08 skill );
	createEntry *FindItem( UI16 itemNum );
	void MakeItem( createEntry &toMake, CChar *player, cSocket *sock, UI16 itemEntry );
	void ApplyRank( cSocket *s, CItem *c, UI08 rank );
	void HandleMakeMenu( cSocket *s, int button, int menu );

	void CreateTrackingMenu( cSocket *s, int m);
	void TrackingMenu( cSocket *s, int gmindex);
	void Track( CChar *i );
	void Tracking( cSocket *s, int selection );

	void Fish( CChar *i );

	void Wheel( cSocket *s );
	bool EngraveAction( cSocket *s, CItem *i, int getCir, int getSpell );
	void AButte( cSocket *s, CItem *x );
	void Snooping( cSocket *s, CChar *target, CItem *pack );

	bool CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill );
	void SkillUse( cSocket *s, UI08 x );
	void updateSkillLevel( CChar *c, UI08 s);
	void AdvanceStats( CChar *s, UI08 sk, bool skillsuccess );

	int			GetNumberOfOres( void );
	miningData *GetOre( int number );
	miningData *GetOre( std::string name );
	int			GetOreIndex( std::string name );
	void		MakeOre( UI08 Region, CChar *actor, cSocket *s );
	SI16		FindOreType( UI16 colour );
};

#endif

