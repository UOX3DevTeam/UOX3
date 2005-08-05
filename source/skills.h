#ifndef __SKILLS_H__
#define __SKILLS_H__

namespace UOX
{

struct miningData
{
	UI16 colour;		// colour of the ore, for colour of ingot
	UI16 minSkill;		// minimum skill needed to make the ingot
	std::string name;		// name of the ingot: no need to be fixed, as we're loading it dynamically
	bool foreign;		// if not iron, then it can print out that it's a stranger ore when failing
	int makemenu;		// the makemenu required for making with
	UI08 minAmount;		// min number of ingots to load anything

	miningData() : colour( 0 ), minSkill( 0 ), name( "" ), foreign( false ), makemenu( 0 ), minAmount( 0 )
	{
	}
};

class cSkills
{
private:
	struct createMenu
	{
		std::vector< UI16 > itemEntries;
		std::vector< UI16 > menuEntries;

		std::vector< UI16 >::iterator iIter;
		std::vector< UI16 >::iterator mIter;

		createMenu()
		{
			itemEntries.resize( 0 );
			menuEntries.resize( 0 );

			iIter = itemEntries.end();
			mIter = menuEntries.end();
		}
	};
	struct createMenuEntry
	{
		UI16 targID;
		UI16 colour;
		std::string name;
		UI16 subMenu;
		createMenuEntry() : targID( 0 ), colour( 0 ), name( "" ), subMenu( 0 )
		{
		}
	};

	struct resourceEntry
	{
		SI16	oreAmt;
		UI32	oreTime;
		SI16	logAmt;
		UI32	logTime;

		resourceEntry() : oreAmt( 0 ), oreTime( 0 ), logAmt( 0 ), logTime( 0 )
		{
		}
	};

	std::vector< miningData >			ores;
	std::map< UI16, createMenu >		actualMenus;
	std::map< UI16, createMenuEntry >	skillMenus;
	std::map< UI16, createEntry >		itemsForMenus;
	resourceEntry						resources[610][410];
private:

	void	RegenerateOre( long grX, long grY );
	void	RegenerateLog( long grX, long grY );
	void	doStealing( CSocket *s, CChar *mChar, CChar *npc, CItem *item );
	SI16	calcStealDiff( CChar *c, CItem *i );

	TargetFunc PeaceMaking;
	TargetFunc RandomSteal;
	TargetFunc Meditation;

	SI08 FindSkillPoint( UI08 sk, int value );
	void AnvilTarget( CSocket *s, CItem& item, miningData *oreType );
	void Atrophy( CChar *c, UI08 sk );
	void PlayInstrument( CSocket *s, CItem *i, bool wellPlayed );
	CItem *GetInstrument( CSocket *s );
	UI08 TrackingDirection( CSocket *s, CChar *i );

	bool LoadMiningData( void );
	void LoadCreateMenus( void );
	void LoadResourceData( void );
	bool AdvanceSkill( CChar *s, UI08 sk, bool skillused );

public:
			cSkills( void );
			~cSkills( void );

	SI32 CalcRankAvg( CChar *player, createEntry& skillMake );

	TargetFunc BottleTarget;
	TargetFunc BowCraft;
	TargetFunc EnticementTarget1;
	TargetFunc EnticementTarget2;
	TargetFunc FishTarget;
	TargetFunc GraveDig;
	TargetFunc ItemIDTarget;
	TargetFunc LockPick;
	TargetFunc handleCooking;
	TargetFunc Mine;
	TargetFunc Persecute;
	TargetFunc ProvocationTarget1;
	TargetFunc ProvocationTarget2;
	TargetFunc RepairMetal;
	TargetFunc SmeltOre;
	TargetFunc Smith;
	TargetFunc StealingTarget;
	TargetFunc TinkerAwg;
	TargetFunc TinkerAxel;
	TargetFunc TinkerClock;
	TargetFunc TreeTarget;
	TargetFunc Inscribe;

	void Load( void );
	void SaveResources( void );

	void NewMakeMenu( CSocket *s, int menu, UI08 skill );
	createEntry *FindItem( UI16 itemNum );
	void MakeItem( createEntry &toMake, CChar *player, CSocket *sock, UI16 itemEntry );
	void ApplyRank( CSocket *s, CItem *c, UI08 rank );
	void HandleMakeMenu( CSocket *s, int button, int menu );

	void CreateTrackingMenu( CSocket *s, UI16 m );
	void TrackingMenu( CSocket *s, UI16 gmindex );
	void Track( CChar *i );
	void Tracking( CSocket *s, int selection );
	void MakeNecroReg( CSocket *nSocket, CItem *nItem, UI16 itemID );
	void Fish( CChar *i );

	bool EngraveAction( CSocket *s, CItem *i, int getCir, int getSpell );
	void Snooping( CSocket *s, CChar *target, CItem *pack );

	bool CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill );
	void SkillUse( CSocket *s, UI08 x );
	void updateSkillLevel( CChar *c, UI08 s);
	void AdvanceStats( CChar *s, UI08 sk, bool skillsuccess );

	size_t		GetNumberOfOres( void );
	miningData *GetOre( size_t number );
	miningData *FindOre( std::string name );
	miningData *FindOre( UI16 colour );
	void		MakeOre( UI08 Region, CChar *actor, CSocket *s );
};

extern cSkills *Skills;

}

#endif

