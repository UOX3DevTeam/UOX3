//------------------------------------------------------------------------
//  craces.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by Daniel Stratton (Abaddon)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------

#ifndef __Races__
#define __Races__

struct internalStruct {
	SKILLVAL      iSkills[SKILLS];
	char          *raceName;
	bool          reqBeard;
	bool		  noBeard;
	bool		  playerRace;
	COLOR         beardColors[2];
	COLOR         hairColors[2];
	COLOR         skinColors[2];

	GENDER        restrictGender;
	RACEID        *racialEnemies;
	bool          weathAffect[WEATHNUM];
	LIGHTLEVEL    lightLevel;
	LIGHTLEVEL    nightVision;
	SECONDS       weathSecs[WEATHNUM];
	ARMORCLASS    armorRestrict;
	unsigned char weathDamage[WEATHNUM];
	SKILLVAL      languageMin;
	RANGE		  visDistance;
};

struct combatModifiers
{
	unsigned char value;
};

class cRaces 
{
	private:
		// data
		unsigned int   iCount;
		internalStruct *races;
		combatModifiers *combat;
		unsigned int modifierCount;

		// functions - accessors
		COLOR         getColorValue( unsigned char colorType, unsigned char maxmin, RACEID race );
		COLOR         getHairMin( RACEID race ); 
		COLOR         getHairMax( RACEID race ); 
		COLOR         getBeardMin( RACEID race ); 
		COLOR         getBeardMax( RACEID race ); 
		COLOR         getSkinMin( RACEID race ); 
		COLOR         getSkinMax( RACEID race ); 
		SECONDS 	  getSecs( RACEID race, int element );
		bool 		  getAffect( RACEID race, int element );
		unsigned char getDamage( RACEID race, int element );

		// Mutators
		void          setHairMin( RACEID race, COLOR value ); 
		void          setHairMax( RACEID race, COLOR value ); 
		void          setBeardMin( RACEID race, COLOR value ); 
		void          setBeardMax( RACEID race, COLOR value ); 
		void          setSkinMin( RACEID race, COLOR value ); 
		void          setSkinMax( RACEID race, COLOR value ); 
		void          setColorValue( unsigned char colorType, unsigned char maxmin, COLOR value, RACEID race );
		void		  setRacialInfo( RACEID race, RACEID toSet, RACEREL value );
		void		  setSecs( RACEID race, int element, SECONDS value );
		void 		  setAffect( RACEID race, int element, bool value);
		void		  setDamage( RACEID race, int element, unsigned char value);

	public:
		// Functions
		~cRaces();
		cRaces() { iCount = 0; }
		void          load( void );					// loads races
		int           Compare( PLAYER player1, PLAYER player2 );	// compares chars
		void          gate( PLAYER s, RACEID x, int always=0 ); // Race Gate
		int           armor( int id1, int id2 );		// Armor restriction
		bool          beardInRange( COLOR color, RACEID x );
		bool          skinInRange( COLOR color, RACEID x );
		bool          hairInRange( COLOR color, RACEID x );
		bool          beardRestricted( RACEID x ); 
		bool          hairRestricted( RACEID x ); 
		bool          skinRestricted( RACEID x ); 

		// Accessors
		//char         *getName( PLAYER player1 );	// Returns race name of player
		char         *getName( RACEID race );	// Returns race name of player
		int           getSkill( int skill, RACEID race ); 
		GENDER        getGenderRestrict( RACEID race ); 
		bool          getLightAffect( RACEID race ); 
		bool          getRainAffect( RACEID race ); 
		bool          getHeatAffect( RACEID race ); 
		bool          getColdAffect( RACEID race ); 
		bool          getLightningAffect( RACEID race ); 
		bool          getSnowAffect( RACEID race ); 
		bool          getRequireBeard( RACEID race );
		bool		  isPlayerRace( RACEID race ) { return races[race].playerRace; };
		LIGHTLEVEL    getLightLevel( RACEID race ); 
		SECONDS       getLightSecs( RACEID race ); 
		SECONDS       getRainSecs( RACEID race ); 
		SECONDS       getHeatSecs( RACEID race ); 
		SECONDS       getColdSecs( RACEID race ); 
		SECONDS       getSnowSecs( RACEID race ); 
		SECONDS       getLightningSecs( RACEID race ); // this is probably not going to be worthi it
		ARMORCLASS    getArmorRestrict( RACEID race ); 
		COLOR         getRandomSkin( RACEID x ); 
		COLOR         getRandomHair( RACEID x ); 
		COLOR         getRandomBeard( RACEID x ); 
		unsigned char getLightDamage( RACEID x ) { return races[x].weathDamage[LIGHT]; }
		unsigned char getLightningDamage( RACEID x ) { return races[x].weathDamage[LIGHTNING]; }
		unsigned char getRainDamage( RACEID x ) { return races[x].weathDamage[RAIN]; }
		unsigned char getSnowDamage( RACEID x ) { return races[x].weathDamage[SNOW]; }
		unsigned char getHeatDamage( RACEID x ) { return races[x].weathDamage[HEAT]; }
		unsigned char getColdDamage( RACEID x ) { return races[x].weathDamage[COLD]; }
		SI16		  getDamageFromSkill( int skill, RACEID x );
		SI16		  getFightPercent( int skill, RACEID x );
		SKILLVAL      getLanguageMin( RACEID x );
		LIGHTLEVEL	  getVisLevel( RACEID x );
		RANGE		  getVisRange( RACEID x );
		bool		  getNoBeard( RACEID x );

		// Mutators
		void          setSkill( int skill, int value, RACEID race );
		void          setGenderRestrict( GENDER gender, RACEID race ); 
		void          setLightAffect( bool value, RACEID race ); 
		void          setRainAffect( bool value, RACEID race ); 
		void          setHeatAffect( bool value, RACEID race ); 
		void          setColdAffect( bool value, RACEID race ); 
		void          setLightningAffect( bool value, RACEID race ); 
		void          setSnowAffect( bool value, RACEID race ); 
		void          setRequireBeard( bool value, RACEID race ); 
		void		  setNoBeard( bool value, RACEID race );
		void          setLightLevel( RACEID race, LIGHTLEVEL value ); 
		void          setLightSecs( RACEID race, SECONDS value ); 
		void          setArmorRestrict( RACEID race, ARMORCLASS value ); 
		void		  setRacialEnemy( RACEID race, RACEID enemy );
		void		  setRacialAlly( RACEID race, RACEID ally );
		void		  setRacialNeutral( RACEID race, RACEID neutral );
		void		  setLanguageMin( SKILLVAL toSetTo, RACEID race );

		void		  setLightDamage( RACEID race, unsigned char value );
		void		  setLightningDamage( RACEID race, unsigned char value );
		void		  setHeatDamage( RACEID race, unsigned char value );
		void		  setColdDamage( RACEID race, unsigned char value );
		void		  setSnowDamage( RACEID race, unsigned char value );
		void		  setRainDamage( RACEID race, unsigned char value );

		void          setLightningSecs( RACEID race, SECONDS value ); 
		void          setHeatSecs( RACEID race, SECONDS value ); 
		void          setColdSecs( RACEID race, SECONDS value ); 
		void          setSnowSecs( RACEID race, SECONDS value ); 
		void          setRainSecs( RACEID race, SECONDS value ); 
		void		  setVisLevel( RACEID x, LIGHTLEVEL bonus );
		void		  setVisRange( RACEID x, RANGE range );
		void		  setIsPlayerRace( RACEID x, bool value );

		void debugPrint( RACEID race );
		void debugPrintAll();
		void InitRace( RACEID race );
};

#endif
