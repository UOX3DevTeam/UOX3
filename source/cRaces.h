#ifndef __Races__
#define __Races__

namespace UOX
{

class CRace 
{
private:
struct ColourPair
{
	COLOUR cMin;
	COLOUR cMax;
	ColourPair( COLOUR a, COLOUR b ) : cMin( a ), cMax( b )
	{
	}
	ColourPair() : cMin( 0 ), cMax( 0 )
	{
	}
};

	typedef std::vector< ColourPair >	COLOURLIST;
	typedef UI16						BITLIST;
	typedef std::vector< SI08 >			RACEIDLIST;

	SI16			HPMod;
	SI16			ManaMod;
	SI16			StamMod;
private:

	SKILLVAL		iSkills[SKILLS];
	std::string			raceName;

	COLOURLIST		beardColours;
	COLOURLIST		hairColours;
	COLOURLIST		skinColours;

	UI16			bools;

	GENDER			restrictGender;
	RACEIDLIST		racialEnemies;

	LIGHTLEVEL		lightLevel;
	COLDLEVEL		coldLevel;
	HEATLEVEL		heatLevel;
	LIGHTLEVEL		nightVision;
	ARMORCLASS		armourRestrict;

	BITLIST			weatherAffected;
	SECONDS			weathSecs[WEATHNUM];
	SI08			weathDamage[WEATHNUM];
	
	SKILLVAL		languageMin;
	RANGE			visDistance;
	R32				poisonResistance;	// % of poison to cancel
	R32				magicResistance;	// % of magic to cancel
	bool			doesHunger;
	UI16			hungerRate;
	SI16			hungerDamage;

public:

					CRace();
					CRace( int numRaces );
					~CRace();

	SKILLVAL		Skill( int skillNum ) const;
	const std::string Name( void ) const;
	bool			RequiresBeard( void ) const;
	bool			NoBeard( void ) const;
	bool			IsPlayerRace( void ) const;
	bool			AffectedBy( WeatherType iNum ) const;
	void			AffectedBy( bool value, WeatherType iNum );
	bool			NoHair( void ) const;

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
	SI08			RaceRelation( RACEID race ) const;

	UI16			GetHungerRate( void ) const;
	void			SetHungerRate( UI16 newValue );
	SI16			GetHungerDamage( void ) const;
	void			SetHungerDamage( SI16 newValue );
	bool			DoesHunger( void ) const;
	void			DoesHunger( bool newValue );

	void			Skill( SKILLVAL newValue, int iNum );
	void			Name( const std::string newName );
	void			RequiresBeard( bool newValue );
	void			NoBeard( bool newValue );
	void			IsPlayerRace( bool newValue );
	void			NoHair( bool newValue );

	SI16			HPModifier( void ) const;
	void			HPModifier( SI16 value );
	SI16			ManaModifier( void ) const;
	void			ManaModifier( SI16 value );
	SI16			StamModifier( void ) const;
	void			StamModifier( SI16 value );

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

	void			NumEnemyRaces( int iNum );
	void			RaceRelation( SI08 value, RACEID race );

	COLOUR			RandomSkin( void ) const;
	COLOUR			RandomHair( void ) const;
	COLOUR			RandomBeard( void ) const;

	bool			IsSkinRestricted( void ) const;
	bool			IsHairRestricted( void ) const;
	bool			IsBeardRestricted( void ) const;

	bool			IsValidSkin( COLOUR val ) const;
	bool			IsValidHair( COLOUR val ) const;
	bool			IsValidBeard( COLOUR val ) const;

	void			Load( size_t sectNum, int modCount );
	CRace&			operator =( CRace& trgRace );
};

class cRaces 
{
	private:
		struct combatModifiers
		{
			UI08 value;
		};
		typedef std::vector< CRace * >			RACELIST;
		typedef std::vector< combatModifiers >	MODIFIERLIST;
		// data
		RACELIST		races;
		MODIFIERLIST	combat;

		void			DefaultInitCombat( void );

		// functions - accessors


		// Mutators
		void			RacialInfo( RACEID race, RACEID toSet, RACEREL value );

		bool			InvalidRace( RACEID x ) const;

	public:
		// Functions
						~cRaces();
						cRaces( void );
		void			load( void );					// loads races
		SI08			Compare( CChar *player1, CChar *player2 ) const;
		SI08			CompareByRace( RACEID race1, RACEID race2 ) const;	// compares chars
		void			gate( CChar *s, RACEID x, bool always = false ); // Race Gate
		bool			beardInRange( COLOUR color, RACEID x ) const;
		bool			skinInRange( COLOUR color, RACEID x ) const;
		bool			hairInRange( COLOUR color, RACEID x ) const;
		bool			beardRestricted( RACEID x ) const; 
		bool			hairRestricted( RACEID x ) const; 
		bool			skinRestricted( RACEID x ) const; 

		CRace *			Race( RACEID x );

		// Accessors
		SECONDS			Secs( RACEID race, WeatherType element ) const;
		SI08			Damage( RACEID race, WeatherType element ) const;
		bool			Affect( RACEID race, WeatherType element ) const;
		const std::string Name( RACEID race ) const;	// Returns race name of player
		SKILLVAL		Skill( int skill, RACEID race ) const; 
		GENDER			GenderRestrict( RACEID race ) const; 
		bool			RequireBeard( RACEID race ) const;
		bool			IsPlayerRace( RACEID race ) const;
		LIGHTLEVEL		LightLevel( RACEID race ) const;
		COLDLEVEL		ColdLevel( RACEID race ) const;
		HEATLEVEL		HeatLevel( RACEID race ) const;
		bool			DoesHunger( RACEID race ) const;
		UI16			GetHungerRate( RACEID race ) const;
		SI16			GetHungerDamage( RACEID race ) const;
		ARMORCLASS		ArmorRestrict( RACEID race ) const; 
		COLOUR			RandomSkin( RACEID x ) const; 
		COLOUR			RandomHair( RACEID x ) const; 
		COLOUR			RandomBeard( RACEID x ) const;
		SI32			DamageFromSkill( int skill, RACEID x ) const;
		SI32			FightPercent( int skill, RACEID x ) const;
		SKILLVAL		LanguageMin( RACEID x ) const;
		LIGHTLEVEL		VisLevel( RACEID x ) const;
		RANGE			VisRange( RACEID x ) const;
		bool			NoBeard( RACEID x ) const;

		// Mutators
		void			Secs( RACEID race, WeatherType element, SECONDS value );
		void			Damage( RACEID race, WeatherType element, SI08 value );
		void			Affect( RACEID race, WeatherType element, bool value );
		void			Skill( int skill, int value, RACEID race );
		void			GenderRestrict( GENDER gender, RACEID race ); 
		void			RequireBeard( bool value, RACEID race ); 
		void			NoBeard( bool value, RACEID race );
		void			LightLevel( RACEID race, LIGHTLEVEL value ); 
		void			ColdLevel( RACEID race, COLDLEVEL value ); 
		void			HeatLevel( RACEID race, HEATLEVEL value ); 
		void			DoesHunger( RACEID race, bool value );
		void			SetHungerRate( RACEID race, UI16 value ); 
		void			SetHungerDamage( RACEID race, SI16 value );
		void			ArmorRestrict( RACEID race, ARMORCLASS value ); 
		void			RacialEnemy( RACEID race, RACEID enemy );
		void			RacialAlly( RACEID race, RACEID ally );
		void			RacialNeutral( RACEID race, RACEID neutral );
		void			LanguageMin( SKILLVAL toSetTo, RACEID race );

		void			VisLevel( RACEID x, LIGHTLEVEL bonus );
		void			VisRange( RACEID x, RANGE range );
		void			IsPlayerRace( RACEID x, bool value );

		void			debugPrint( RACEID race );
		void			debugPrintAll( void );

		size_t			Count( void ) const;
};

extern cRaces *Races;

}

#endif
