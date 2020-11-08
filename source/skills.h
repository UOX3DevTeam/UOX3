#ifndef __SKILLS_H__
#define __SKILLS_H__


// create DFN revisions
struct resAmountPair
{
	std::vector< UI16 > idList;
	UI08 amountNeeded;
	UI16 colour;
	resAmountPair() : amountNeeded( 1 ), colour( 0 )
	{
		idList.resize( 0 );
	}
	~resAmountPair()
	{
		idList.resize( 0 );
	}
};

struct resSkillReq
{
	UI08 skillNumber;
	UI16 minSkill;
	UI16 maxSkill;
	resSkillReq() : skillNumber( 0 ), minSkill( 0 ), maxSkill( 0 )
	{
	}
};

struct createEntry
{
	UI16 colour;
	UI16 targID;
	UI16 soundPlayed;
	UI08 minRank;
	UI08 maxRank;
	std::string addItem;
	SI16 delay;
	UI16 spell;
	std::vector< resAmountPair > resourceNeeded;
	std::vector< resSkillReq > skillReqs;
	std::string name;
	createEntry() : colour( 0 ), targID( 0 ), soundPlayed( 0 ), minRank( 1 ), maxRank( 10 ), addItem( "" ), delay( 0 ), spell( 0 ), name( "" )
	{
		resourceNeeded.resize( 0 );
		skillReqs.resize( 0 );
	}
	~createEntry()
	{
		resourceNeeded.resize( 0 );
		skillReqs.resize( 0 );
	}
	R32 AverageMinSkill( void )
	{
		R32 sum = 0;
		for( size_t i = 0; i < skillReqs.size(); ++i )
			sum += skillReqs[i].minSkill;
		return sum / skillReqs.size();
	}
	R32 AverageMaxSkill( void )
	{
		R32 sum = 0;
		for( size_t i = 0; i < skillReqs.size(); ++i )
			sum += skillReqs[i].maxSkill;
		return sum / skillReqs.size();
	}

};

struct miningData
{
	std::string oreName; // ore name from ORE_LIST in skills.dfn
	UI16 colour;		// colour of the ore, for colour of ingot
	UI16 minSkill;		// minimum skill needed to make the ingot
	std::string name;	// name of the ingot: no need to be fixed, as we're loading it dynamically
	SI32 makemenu;		// the makemenu required for making with
	UI16 oreChance; // default chance of finding ore type if nothing else is specified

	miningData() : oreName( "" ), colour( 0 ), minSkill( 0 ), name( "" ), makemenu( 0 ), oreChance( 0 )
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
		~createMenu()
		{
			itemEntries.resize( 0 );
			menuEntries.resize( 0 );
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

	std::vector< miningData >			ores;
	std::map< UI16, createMenu >		actualMenus;
	std::map< UI16, createMenuEntry >	skillMenus;
	std::map< UI16, createEntry >		itemsForMenus;
private:

	void	RegenerateOre( SI16 grX, SI16 grY, UI08 worldNum );
	void	doStealing( CSocket *s, CChar *mChar, CChar *npc, CItem *item );
	SI16	calcStealDiff( CChar *c, CItem *i );

	TargetFunc RandomSteal;

	SI08 FindSkillPoint( UI08 sk, SI32 value );
	void AnvilTarget( CSocket *s, CItem& item, miningData *oreType );
	void HandleSkillChange( CChar *c, UI08 sk, SI08 skillAdvance, bool success );

	bool LoadMiningData( void );
	void LoadCreateMenus( void );
	bool AdvanceSkill( CChar *s, UI08 sk, bool skillused );

public:
	cSkills( void );
	~cSkills( void );

	SI32 CalcRankAvg( CChar *player, createEntry& skillMake );

	TargetFunc FishTarget;
	TargetFunc GraveDig;
	TargetFunc ItemIDTarget;
	TargetFunc Mine;
	TargetFunc Persecute;
	TargetFunc RepairMetal;
	TargetFunc SmeltOre;
	TargetFunc Smith;
	TargetFunc StealingTarget;

	void Load( void );

	void NewMakeMenu( CSocket *s, SI32 menu, UI08 skill );
	createEntry *FindItem( UI16 itemNum );
	void MakeItem( createEntry &toMake, CChar *player, CSocket *sock, UI16 itemEntry );
	void ApplyRank( CSocket *s, CItem *c, UI08 rank, UI08 maxrank );
	void HandleMakeMenu( CSocket *s, SI32 button, SI32 menu );

	void CreateTrackingMenu( CSocket *s, UI16 m );
	void TrackingMenu( CSocket *s, UI16 gmindex );
	void Track( CChar *i );
	void Tracking( CSocket *s, SI32 selection );
	void MakeNecroReg( CSocket *nSocket, CItem *nItem, UI16 itemID );
	void Fish( CSocket *mSock, CChar *mChar );

	void Snooping( CSocket *s, CChar *target, CItem *pack );

	bool CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill );
	void SkillUse( CSocket *s, UI08 x );
	void updateSkillLevel( CChar *c, UI08 s) const;
	void AdvanceStats( CChar *s, UI08 sk, bool skillsuccess );

	size_t		GetNumberOfOres( void );
	miningData *GetOre( size_t number );
	miningData *FindOre( std::string const &name );
	miningData *FindOre( UI16 const &colour );
};

extern cSkills *Skills;

#endif

