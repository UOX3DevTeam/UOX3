#ifndef __Races__
#define __Races__

#include <vector>
#include <string>
using namespace std;

struct ColourPair
{
	COLOUR cMin;
	COLOUR cMax;
	ColourPair( COLOUR a, COLOUR b ) : cMin( a ), cMax( b ) { }
	ColourPair() : cMin( 0 ), cMax( 0 ) { }
};

typedef vector< ColourPair >	COLOURLIST;
typedef UI16					BITLIST;
typedef vector< SI08 >			RACEIDLIST;

class CRace 
{
private:
	SI16			HPMod;
	SI16			ManaMod;
	SI16			StamMod;
protected:

	SKILLVAL		iSkills[SKILLS];
	string			raceName;

	COLOURLIST		beardColours;
	COLOURLIST		hairColours;
	COLOURLIST		skinColours;

	UI16			bools;

	GENDER			restrictGender;
	RACEIDLIST		racialEnemies;

	LIGHTLEVEL		lightLevel;
	LIGHTLEVEL		nightVision;
	ARMORCLASS		armourRestrict;

	SECONDS			weathSecs[WEATHNUM];
	SI08			weathDamage[WEATHNUM];
	
	SKILLVAL		languageMin;
	RANGE			visDistance;
	R32				poisonResistance;	// % of poison to cancel
	R32				magicResistance;	// % of magic to cancel

public:

					CRace();
					CRace( int numRaces );
	virtual			~CRace();

	SKILLVAL		Skill( int skillNum ) const;
	const char *	Name( void ) const;
	bool			RequiresBeard( void ) const;
	bool			NoBeard( void ) const;
	bool			IsPlayerRace( void ) const;
	bool			AffectedByLight( void ) const;
	bool			AffectedByRain( void ) const;
	bool			AffectedByCold( void ) const;
	bool			AffectedByHeat( void ) const;
	bool			AffectedByLightning( void ) const;
	bool			AffectedBySnow( void ) const;
	bool			NoHair( void ) const;

	GENDER			GenderRestriction( void ) const;
	LIGHTLEVEL		LightLevel( void ) const;
	LIGHTLEVEL		NightVision( void ) const;
	ARMORCLASS		ArmourClassRestriction( void ) const;
	SECONDS			WeatherSeconds( WeatherType iNum ) const;
	SI08			WeatherDamage( WeatherType iNum ) const;
	R32				MagicResistance( void ) const;
	R32				PoisonResistance( void ) const;

	SKILLVAL		LanguageMin( void ) const;
	RANGE			VisibilityRange( void ) const;
	SI08			RaceRelation( RACEID race ) const;

	void			Skill( SKILLVAL newValue, int iNum );
	void			Name( const char *newName );
	void			RequiresBeard( bool newValue );
	void			NoBeard( bool newValue );
	void			IsPlayerRace( bool newValue );
	void			AffectedByLight( bool newValue );
	void			AffectedByRain( bool newValue );
	void			AffectedByCold( bool newValue );
	void			AffectedByHeat( bool newValue );
	void			AffectedByLightning( bool newValue );
	void			AffectedBySnow( bool newValue );
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

	void			Load( SI32 sectNum, int modCount );
	CRace&			operator =( CRace& trgRace );
};

typedef vector< CRace * >			RACELIST;

struct combatModifiers
{
	UI08 value;
};

typedef vector< combatModifiers >	MODIFIERLIST;
class cRaces 
{
	private:
		// data
		RACELIST		races;
		MODIFIERLIST	combat;

		void			DefaultInitCombat( void );

		// functions - accessors
		SECONDS			Secs( RACEID race, WeatherType element ) const;
		SI08			Damage( RACEID race, WeatherType element ) const;

		// Mutators
		void			RacialInfo( RACEID race, RACEID toSet, RACEREL value );
		void			Secs( RACEID race, WeatherType element, SECONDS value );
		void			Damage( RACEID race, WeatherType element, SI08 value );

		bool			InvalidRace( RACEID x ) const;

	public:
		// Functions
		virtual			~cRaces();
						cRaces( void );
		void			load( void );					// loads races
		SI08			Compare( PLAYER player1, PLAYER player2 ) const;	// compares chars
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
		const char *	Name( RACEID race ) const;	// Returns race name of player
		SKILLVAL		Skill( int skill, RACEID race ) const; 
		GENDER			GenderRestrict( RACEID race ) const; 
		bool			LightAffect( RACEID race ) const; 
		bool			RainAffect( RACEID race ) const; 
		bool			HeatAffect( RACEID race ) const; 
		bool			ColdAffect( RACEID race ) const; 
		bool			LightningAffect( RACEID race ) const; 
		bool			SnowAffect( RACEID race ) const; 
		bool			RequireBeard( RACEID race ) const;
		bool			IsPlayerRace( RACEID race ) const;
		LIGHTLEVEL		LightLevel( RACEID race ) const; 
		SECONDS			LightSecs( RACEID race ) const; 
		SECONDS			RainSecs( RACEID race ) const; 
		SECONDS			HeatSecs( RACEID race ) const; 
		SECONDS			ColdSecs( RACEID race ) const; 
		SECONDS			SnowSecs( RACEID race ) const; 
		SECONDS			LightningSecs( RACEID race ) const; // this is probably not going to be worthi it
		ARMORCLASS		ArmorRestrict( RACEID race ) const; 
		COLOUR			RandomSkin( RACEID x ) const; 
		COLOUR			RandomHair( RACEID x ) const; 
		COLOUR			RandomBeard( RACEID x ) const;
		SI08			LightDamage( RACEID x ) const;
		SI08			LightningDamage( RACEID x ) const;
		SI08			RainDamage( RACEID x ) const;
		SI08			SnowDamage( RACEID x ) const;
		SI08			HeatDamage( RACEID x ) const;
		SI08			ColdDamage( RACEID x ) const;
		SI32			DamageFromSkill( int skill, RACEID x ) const;
		SI32			FightPercent( int skill, RACEID x ) const;
		SKILLVAL		LanguageMin( RACEID x ) const;
		LIGHTLEVEL		VisLevel( RACEID x ) const;
		RANGE			VisRange( RACEID x ) const;
		bool			NoBeard( RACEID x ) const;

		// Mutators
		void			Skill( int skill, int value, RACEID race );
		void			GenderRestrict( GENDER gender, RACEID race ); 
		void			LightAffect( bool value, RACEID race ); 
		void			RainAffect( bool value, RACEID race ); 
		void			HeatAffect( bool value, RACEID race ); 
		void			ColdAffect( bool value, RACEID race ); 
		void			LightningAffect( bool value, RACEID race ); 
		void			SnowAffect( bool value, RACEID race ); 
		void			RequireBeard( bool value, RACEID race ); 
		void			NoBeard( bool value, RACEID race );
		void			LightLevel( RACEID race, LIGHTLEVEL value ); 
		void			LightSecs( RACEID race, SECONDS value ); 
		void			ArmorRestrict( RACEID race, ARMORCLASS value ); 
		void			RacialEnemy( RACEID race, RACEID enemy );
		void			RacialAlly( RACEID race, RACEID ally );
		void			RacialNeutral( RACEID race, RACEID neutral );
		void			LanguageMin( SKILLVAL toSetTo, RACEID race );

		void			LightDamage( RACEID race, SI08 value );
		void			LightningDamage( RACEID race, SI08 value );
		void			HeatDamage( RACEID race, SI08 value );
		void			ColdDamage( RACEID race, SI08 value );
		void			SnowDamage( RACEID race, SI08 value );
		void			RainDamage( RACEID race, SI08 value );

		void			LightningSecs( RACEID race, SECONDS value ); 
		void			HeatSecs( RACEID race, SECONDS value ); 
		void			ColdSecs( RACEID race, SECONDS value ); 
		void			SnowSecs( RACEID race, SECONDS value ); 
		void			RainSecs( RACEID race, SECONDS value ); 
		void			VisLevel( RACEID x, LIGHTLEVEL bonus );
		void			VisRange( RACEID x, RANGE range );
		void			IsPlayerRace( RACEID x, bool value );

		void			debugPrint( RACEID race );
		void			debugPrintAll( void );

		SI32			Count( void ) const;
};

#endif
