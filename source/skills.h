#ifndef __SKILLS_H__
#define __SKILLS_H__


// create DFN revisions
struct ResAmountPair_st
{
	std::vector<UI16> idList;
	UI08 amountNeeded;
	UI16 colour;
	UI32 moreVal;
	ResAmountPair_st() : amountNeeded( 1 ), colour( 0 ), moreVal( 0 )
	{
		idList.resize( 0 );
	}
	~ResAmountPair_st()
	{
		idList.resize( 0 );
	}
};

struct ResSkillReq_st
{
	UI08 skillNumber;
	UI16 minSkill;
	UI16 maxSkill;
	ResSkillReq_st() : skillNumber( 0 ), minSkill( 0 ), maxSkill( 0 )
	{
	}
};

struct CreateEntry_st
{
	UI16 colour;
	UI16 targId;
	UI16 soundPlayed;
	UI08 minRank;
	UI08 maxRank;
	std::string addItem;
	SI16 delay;
	UI16 spell;
	std::vector<ResAmountPair_st> resourceNeeded;
	std::vector<ResSkillReq_st> skillReqs;
	std::string name;
	CreateEntry_st() : colour( 0 ), targId( 0 ), soundPlayed( 0 ), minRank( 1 ), maxRank( 10 ), addItem( "" ), delay( 0 ), spell( 0 ), name( "" )
	{
		resourceNeeded.resize( 0 );
		skillReqs.resize( 0 );
	}
	~CreateEntry_st()
	{
		resourceNeeded.resize( 0 );
		skillReqs.resize( 0 );
	}
	R32 AverageMinSkill( void )
	{
		R32 sum = 0;
		for( size_t i = 0; i < skillReqs.size(); ++i )
		{
			sum += skillReqs[i].minSkill;
		}
		return sum / skillReqs.size();
	}
	R32 AverageMaxSkill( void )
	{
		R32 sum = 0;
		for( size_t i = 0; i < skillReqs.size(); ++i )
		{
			sum += skillReqs[i].maxSkill;
		}
		return sum / skillReqs.size();
	}

};

struct MiningData_st
{
	std::string oreName; // ore name from ORE_LIST in skills.dfn
	UI16 colour;		// colour of the ore, for colour of ingot
	UI16 minSkill;		// minimum skill needed to make the ingot
	std::string name;	// name of the ingot: no need to be fixed, as we're loading it dynamically
	SI32 makemenu;		// the makemenu required for making with
	UI16 oreChance;		// default chance of finding ore type if nothing else is specified
	UI16 scriptID;		// scriptID assigned to ore items

	MiningData_st() : oreName( "" ), colour( 0 ), minSkill( 0 ), name( "" ), makemenu( 0 ), oreChance( 0 ), scriptID( 0 )
	{
	}
};

class CSkills
{
private:
	struct CreateMenu_st
	{
		std::vector<UI16> itemEntries;
		std::vector<UI16> menuEntries;

		std::vector<UI16>::iterator iIter;
		std::vector<UI16>::iterator mIter;

		CreateMenu_st()
		{
			itemEntries.resize( 0 );
			menuEntries.resize( 0 );

			iIter = itemEntries.end();
			mIter = menuEntries.end();
		}
		~CreateMenu_st()
		{
			itemEntries.resize( 0 );
			menuEntries.resize( 0 );
		}
	};
	struct CreateMenuEntry_st
	{
		UI16 targId;
		UI16 colour;
		std::string name;
		UI16 subMenu;
		CreateMenuEntry_st() : targId( 0 ), colour( 0 ), name( "" ), subMenu( 0 )
		{
		}
	};

	std::vector<MiningData_st>			ores;
	std::map<UI16, CreateMenu_st>		actualMenus;
	std::map<UI16, CreateMenuEntry_st>	skillMenus;
	std::map<UI16, CreateEntry_st>		itemsForMenus;
private:

	void	RegenerateOre( SI16 grX, SI16 grY, UI08 worldNum );
	void	DoStealing( CSocket *s, CChar *mChar, CChar *npc, CItem *item );
	SI16	CalcStealDiff( CChar *c, CItem *i );

	TargetFunc RandomSteal;

	SI08 FindSkillPoint( UI08 sk, SI32 value );
	void AnvilTarget( CSocket *s, CItem& item, MiningData_st *oreType );
	void HandleSkillChange( CChar *c, UI08 sk, SI08 skillAdvance, bool success, SKILLVAL skAmt = 0, bool triggerEvent = true );

	bool LoadMiningData( void );
	void LoadCreateMenus( void );

public:
	CSkills();
	~CSkills();

	SI32 CalcRankAvg( CChar *player, CreateEntry_st& skillMake );

	TargetFunc Persecute;
	TargetFunc RepairMetal;
	TargetFunc SmeltOre;
	TargetFunc Smith;
	TargetFunc StealingTarget;

	void Load( void );

	void NewMakeMenu( CSocket *s, SI32 menu, UI08 skill );
	CreateEntry_st *FindItem( UI16 itemNum );
	void MakeItem( CreateEntry_st &toMake, CChar *player, CSocket *sock, UI16 itemEntry, UI16 resourceColour = 0 );
	void ApplyRank( CSocket *s, CItem *c, UI08 rank, UI08 maxrank );
	void HandleMakeMenu( CSocket *s, SI32 button, SI32 menu );

	void CreateTrackingMenu( CSocket *s, UI16 m );
	void TrackingMenu( CSocket *s, UI16 gmindex );
	void Track( CChar *i );
	void Tracking( CSocket *s, SI32 selection );
	void MakeNecroReg( CSocket *nSocket, CItem *nItem, UI16 itemId );

	void Snooping( CSocket *s, CChar *target, CItem *pack );

	UI16 CalculatePetControlChance( CChar *mChar, CChar *Npc );
	bool CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill, bool isCraftSkill = false );
	void SkillUse( CSocket *s, UI08 x );
	void UpdateSkillLevel( CChar *c, UI08 s) const;
	void AdvanceStats( CChar *s, UI08 sk, bool skillsuccess );
	bool AdvanceSkill(CChar* s, UI08 sk, bool skillused, SKILLVAL skAmt = 0, bool triggerEvent = true);

	size_t		GetNumberOfOres( void );
	MiningData_st *GetOre( size_t number );
	MiningData_st *FindOre( std::string const &name );
	MiningData_st *FindOre( UI16 const &colour );
};

extern CSkills *Skills;

#endif

