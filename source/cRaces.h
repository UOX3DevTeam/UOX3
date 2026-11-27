#ifndef __Races__
#define __Races__
#include <unordered_set>
#include <array>

class CLevelProfession
{
private:
	UI08 professionId;
	std::string professionName;
	std::string description;
	std::string professionType;
	UI08 skillId;
	SKILLVAL startValue;
	SKILLVAL maxValue;
	UI32 experiencePerPoint;
	bool primaryProfession;

public:
	CLevelProfession();
	bool Load( UI08 id );
	UI08 Id() const;
	const std::string &Name() const;
	const std::string &Description() const;
	const std::string &Type() const;
	UI08 SkillId() const;
	const std::string &SkillName() const;
	bool IsPrimary() const;
	SKILLVAL StartValue() const;
	SKILLVAL MaxValue() const;
	UI32 ExperiencePerPoint() const;
	UI64 MaxExperience() const;
	SKILLVAL ProficiencyFromExperience( UI64 experience ) const;
	UI64 ExperienceForProficiency( SKILLVAL proficiency ) const;
};

class CLevelClassAbility
{
private:
	UI16 abilityId;
	std::string abilityName;
	std::string description;
	std::string action;
	SI32 value;
	UI16 requiredLevel;
	UI16 manaCost;
	UI16 staminaCost;
	UI16 cooldownSeconds;
	UI16 scriptId;
	std::string functionName;

public:
	CLevelClassAbility();
	bool Load( UI16 id );
	UI16 Id() const;
	const std::string &Name() const;
	const std::string &Description() const;
	const std::string &Action() const;
	SI32 Value() const;
	UI16 RequiredLevel() const;
	UI16 ManaCost() const;
	UI16 StaminaCost() const;
	UI16 CooldownSeconds() const;
	UI16 ScriptId() const;
	const std::string &FunctionName() const;
};

class CLevelClass
{
private:
	struct SkillTier_st
	{
		UI16 startValue;
		UI16 capPercent;
	};

	UI08 classId;
	std::string className;
	std::string description;
	std::array<UI08, ALLSKILLS> skillTiers;
	std::array<SkillTier_st, 4> tierSettings;
	UI16 healthPerLevel;
	UI16 manaPerLevel;
	UI16 staminaPerLevel;
	std::unordered_set<UI16> allowedEquipment;
	std::unordered_set<UI16> bannedEquipment;
	std::vector<CLevelClassAbility> abilities;

	void LoadSkillList( const std::string &data, UI08 tier );
	void LoadEquipmentList( const std::string &sectionName, bool allowList );
	void LoadAbility( const std::string &data );

public:
	CLevelClass();
	void Load( UI08 id );
	UI08 Id() const;
	const std::string &Name() const;
	const std::string &Description() const;
	UI16 HealthPerLevel() const;
	UI16 ManaPerLevel() const;
	UI16 StaminaPerLevel() const;
	UI08 SkillTier( UI08 skill ) const;
	std::string SkillList( UI08 tier ) const;
	UI16 TierStart( UI08 tier ) const;
	UI16 TierCap( UI08 tier ) const;
	SKILLVAL SkillValue( UI08 skill, UI16 level, UI16 levelCap, SKILLVAL skillCap ) const;
	bool CanEquipItem( UI16 itemId ) const;
	UI16 AbilityCount() const;
	const CLevelClassAbility *Ability( UI16 abilityIndex ) const;
};

class CRace
{
private:
	struct ColourPair_st
	{
		COLOUR cMin;
		COLOUR cMax;
		ColourPair_st( COLOUR a, COLOUR b ) : cMin( a ), cMax( b )
		{
		}
		ColourPair_st() : cMin( 0 ), cMax( 0 )
		{
		}
	};

	typedef std::vector<ColourPair_st>	COLOURLIST;
	typedef std::vector<RaceRelate>		RACEIDLIST;
	typedef std::unordered_set<UI16>	ALLOWEQUIPLIST;
	typedef std::unordered_set<UI16>	BANEQUIPLIST;

	SI16				HPMod;
	SI16				ManaMod;
	SI16				StamMod;
	SI16				RaceHPRegenBonus;
	SI16				RaceStamRegenBonus;
	SI16				RaceManaRegenBonus;

	SI16				RaceMaxWeightBonus;
private:
	std::array<SKILLVAL, SKILLS> iSkills;
	//SKILLVAL			iSkills[SKILLS];
	std::string			raceName;
	RACEID				raceID;

	std::bitset<8>		bools;
	RANGE				visDistance;
	LIGHTLEVEL			nightVision;
	ARMORCLASS			armourRestrict;
	LIGHTLEVEL			lightLevel;
	GENDER				restrictGender;
	SKILLVAL			languageMin;
	R32					poisonResistance;	// % of poison to cancel
	R32					magicResistance;	// % of magic to cancel

	COLOURLIST			beardColours;
	COLOURLIST			hairColours;
	COLOURLIST			skinColours;
	COLOUR				bloodColour;

	RACEIDLIST			racialEnemies;

	COLDLEVEL			coldLevel;
	HEATLEVEL			heatLevel;

	ALLOWEQUIPLIST		allowedEquipment;
	BANEQUIPLIST		bannedEquipment;

	std::bitset<WEATHNUM>	weatherAffected;
	std::array<SECONDS, WEATHNUM> weathSecs;
	//SECONDS					weathSecs[WEATHNUM];
	std::array<SI08, WEATHNUM> weathDamage;
	//SI08					weathDamage[WEATHNUM];

	bool			doesHunger;
	bool			doesThirst;
	UI16			hungerRate;
	UI16			thirstRate;
	SI16			hungerDamage;
	SI16			thirstDrain;

public:

	CRace();
	CRace( SI32 numRaces );
	~CRace();

	SKILLVAL		Skill( SI32 skillNum ) const;
	const std::string Name( void ) const;
	void			SetRaceID( RACEID id );
	bool			RequiresBeard( void ) const;
	bool			NoBeard( void ) const;
	bool			IsPlayerRace( void ) const;
	bool			AffectedBy( WeatherType iNum ) const;
	void			AffectedBy( bool value, WeatherType iNum );
	bool			NoHair( void ) const;
	bool			CanEquipItem( UI16 itemId ) const;

	GENDER			GenderRestriction( void ) const;
	LIGHTLEVEL		LightLevel( void ) const;
	COLDLEVEL		ColdLevel( void ) const;
	HEATLEVEL		HeatLevel( void ) const;
	LIGHTLEVEL		NightVision( void ) const;
	ARMORCLASS		ArmourClassRestriction( void ) const;
	SECONDS			WeatherSeconds( WeatherType iNum ) const;
	SI08			WeatherDamage( WeatherType iNum ) const;
	R32				MagicResistance( void ) const;
	R32				PoisonResistance( void ) const;

	SKILLVAL		LanguageMin( void ) const;
	RANGE			VisibilityRange( void ) const;
	RaceRelate		RaceRelation( RACEID race ) const;

	UI16			GetHungerRate( void ) const;
	UI16			GetThirstRate( void ) const;
	void			SetHungerRate( UI16 newValue );
	void			SetThirstRate( UI16 newValue );
	SI16			GetHungerDamage( void ) const;
	SI16			GetThirstDrain( void ) const;
	void			SetHungerDamage( SI16 newValue );
	void			SetThirstDrain( SI16 newValue );
	bool			DoesHunger( void ) const;
	bool			DoesThirst( void ) const;
	void			DoesHunger( bool newValue );
	void			DoesThirst( bool newValue );

	void			Skill( SKILLVAL newValue, SI32 iNum );
	void			Name( const std::string& newName );
	RACEID			GetRaceID() const;
	void			RequiresBeard( bool newValue );
	void			NoBeard( bool newValue );
	void			IsPlayerRace( bool newValue );
	void			NoHair( bool newValue );
	void			RestrictGear( bool newValue );

	SI16			HPModifier( void ) const;
	void			HPModifier( SI16 value );
	SI16			ManaModifier( void ) const;
	void			ManaModifier( SI16 value );
	SI16			StamModifier( void ) const;
	void			StamModifier( SI16 value );

	SI16			HPRegenBonus( void ) const;
	void			HPRegenBonus( SI16 value );
	SI16			ManaRegenBonus( void ) const;
	void			ManaRegenBonus( SI16 value );
	SI16			StamRegenBonus( void ) const;
	void			StamRegenBonus( SI16 value );

	SI16			MaxWeightBonus( void ) const;
	void			MaxWeightBonus( SI16 value );

	void			MagicResistance( R32 value );
	void			PoisonResistance( R32 value );

	void			GenderRestriction( GENDER newValue );
	void			LightLevel( LIGHTLEVEL newValue );
	void			ColdLevel( COLDLEVEL newValue );
	void			HeatLevel( HEATLEVEL newValue );
	void			NightVision( LIGHTLEVEL newValue );
	void			ArmourClassRestriction( ARMORCLASS newValue );
	void			WeatherSeconds( SECONDS newValue, WeatherType iNum );
	void			WeatherDamage( SI08 newValue, WeatherType iNum );
	void			LanguageMin( SKILLVAL newValue );
	void			VisibilityRange( RANGE newValue );

	void			NumEnemyRaces( SI32 iNum );
	void			RaceRelation( RaceRelate value, RACEID race );

	COLOUR			RandomSkin( void ) const;
	COLOUR			RandomHair( void ) const;
	COLOUR			RandomBeard( void ) const;
	COLOUR			BloodColour( void ) const;
	void			BloodColour( COLOUR newValue );

	bool			IsSkinRestricted( void ) const;
	bool			IsHairRestricted( void ) const;
	bool			IsBeardRestricted( void ) const;

	bool			IsValidSkin( COLOUR val ) const;
	bool			IsValidHair( COLOUR val ) const;
	bool			IsValidBeard( COLOUR val ) const;

	void			Load( size_t sectNum, SI32 modCount );
	CRace&			operator = ( CRace& trgRace );
};

class cRaces
{
private:
	struct CombatModifiers_st
	{
		UI08 value;
	};
	typedef std::vector<CRace *>			RACELIST;
	typedef std::vector<CombatModifiers_st>	MODIFIERLIST;
	// data
	RACELIST		races;
	MODIFIERLIST	combat;
	std::vector<CLevelClass>	levelClasses;
	std::vector<CLevelProfession>	levelProfessions;
	std::array<UI08, ALLSKILLS>	professionBySkill;
	RACEID			nextUniqueRaceID;

	void			DefaultInitCombat( void );

	// functions - accessors


	// Mutators
	void			RacialInfo( RACEID race, RACEID toSet, RaceRelate value );

	bool			InvalidRace( RACEID x ) const;
	bool initialized;
public:
	// Functions
	~cRaces();
	cRaces();
	void			Load( void );					// loads races
	RaceRelate		Compare( CChar *player1, CChar *player2 ) const;
	RaceRelate		CompareByRace( RACEID race1, RACEID race2 ) const;	// compares chars
	void			ApplyRace( CChar *s, RACEID x, bool always = false ); // Race Gate
	bool			BeardInRange( COLOUR color, RACEID x ) const;
	bool			SkinInRange( COLOUR color, RACEID x ) const;
	bool			HairInRange( COLOUR color, RACEID x ) const;
	bool			BeardRestricted( RACEID x ) const;
	bool			HairRestricted( RACEID x ) const;
	bool			SkinRestricted( RACEID x ) const;

	CRace *			Race( RACEID x );
	CLevelClass *	LevelClass( UI08 classId );
	const CLevelClass * LevelClass( UI08 classId ) const;
	UI08			LevelClassCount() const;
	CLevelProfession * LevelProfession( UI08 professionId );
	const CLevelProfession * LevelProfession( UI08 professionId ) const;
	const CLevelProfession * ProfessionForSkill( UI08 skillId ) const;
	UI08			LevelProfessionCount() const;

	// Accessors
	SECONDS			Secs( RACEID race, WeatherType element ) const;
	SI08			Damage( RACEID race, WeatherType element ) const;
	bool			Affect( RACEID race, WeatherType element ) const;
	const std::string Name( RACEID race ) const;	// Returns race name of player
	SKILLVAL		Skill( SI32 skill, RACEID race ) const;
	GENDER			GenderRestrict( RACEID race ) const;
	bool			RequireBeard( RACEID race ) const;
	bool			IsPlayerRace( RACEID race ) const;
	LIGHTLEVEL		LightLevel( RACEID race ) const;
	COLDLEVEL		ColdLevel( RACEID race ) const;
	HEATLEVEL		HeatLevel( RACEID race ) const;
	bool			DoesHunger( RACEID race ) const;
	bool			DoesThirst( RACEID race ) const;
	UI16			GetHungerRate( RACEID race ) const;
	UI16			GetThirstRate( RACEID race ) const;
	SI16			GetHungerDamage( RACEID race ) const;
	SI16			GetThirstDrain( RACEID race ) const;
	ARMORCLASS		ArmorRestrict( RACEID race ) const;
	COLOUR			RandomSkin( RACEID x ) const;
	COLOUR			RandomHair( RACEID x ) const;
	COLOUR			RandomBeard( RACEID x ) const;
	COLOUR			BloodColour( RACEID x ) const;
	SI32			DamageFromSkill( SI32 skill, RACEID x ) const;
	SI32			FightPercent( SI32 skill, RACEID x ) const;
	SKILLVAL		LanguageMin( RACEID x ) const;
	LIGHTLEVEL		VisLevel( RACEID x ) const;
	RANGE			VisRange( RACEID x ) const;
	bool			NoBeard( RACEID x ) const;

	// Mutators
	void			Secs( RACEID race, WeatherType element, SECONDS value );
	void			Damage( RACEID race, WeatherType element, SI08 value );
	void			Affect( RACEID race, WeatherType element, bool value );
	void			Skill( SI32 skill, SI32 value, RACEID race );
	void			GenderRestrict( GENDER gender, RACEID race );
	void			RequireBeard( bool value, RACEID race );
	void			NoBeard( bool value, RACEID race );
	void			LightLevel( RACEID race, LIGHTLEVEL value );
	void			ColdLevel( RACEID race, COLDLEVEL value );
	void			HeatLevel( RACEID race, HEATLEVEL value );
	void			DoesHunger( RACEID race, bool value );
	void			DoesThirst( RACEID race, bool value );
	void			SetHungerRate( RACEID race, UI16 value );
	void			SetThirstRate( RACEID race, UI16 value );
	void			SetHungerDamage( RACEID race, SI16 value );
	void			SetThirstDrain( RACEID race, SI16 value );
	void			ArmorRestrict( RACEID race, ARMORCLASS value );
	void			RacialEnemy( RACEID race, RACEID enemy );
	void			RacialAlly( RACEID race, RACEID ally );
	void			RacialNeutral( RACEID race, RACEID neutral );
	void			LanguageMin( SKILLVAL toSetTo, RACEID race );
	void			BloodColour( RACEID race, COLOUR newValue );

	void			VisLevel( RACEID x, LIGHTLEVEL bonus );
	void			VisRange( RACEID x, RANGE range );
	void			IsPlayerRace( RACEID x, bool value );

	void			DebugPrint( RACEID race );
	void			DebugPrintAll( void );

	size_t			Count( void ) const;
};

extern cRaces *Races;

#endif
