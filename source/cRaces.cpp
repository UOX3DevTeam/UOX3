// Abaddon:  I have a VERY set idea of features and plans for races
// Please DO NOT TOUCH THEM, because I will be working on them quite
// solidly, along with EviLDeD, over the next few months.  

#include "uox3.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"

const RACEREL MIN_ENEMY = -1;
const RACEREL MIN_ALLY  = -1;
const RACEREL NEUTRAL   = 0;
const RACEREL MAX_ENEMY = -100;
const RACEREL MAX_ALLY  = 100;

bool cRaces::InvalidRace( RACEID x ) const
{
	return( x >= races.size() );
}

const char *cRaces::Name( RACEID race ) const
// PRE:	race is a valid race
// POST:	returns pointer to a string
{
	return races[race]->Name();
}

CRace * cRaces::Race( RACEID x )
{
	if( InvalidRace( x ) )
		return NULL;
	return races[x];
}

cRaces::cRaces( void )
{
}

cRaces::~cRaces( void )
// PRE:	cRaces has been initialized
// POST:	Dynamic memory deleted
{
	for( UI32 i = 0; i < races.size(); i++ )
	{
		delete races[i];
		races[i] = NULL;
	}
	races.resize( 0 );
}

void cRaces::DefaultInitCombat( void )
{
	combat.resize( 4 );
	combat[0].value = 0;
	combat[1].value = 50;
	combat[2].value = 100;
	combat[3].value = 200;
}

void cRaces::load( void )
// PRE:		races.scp exists
// POST:	class loaded and populated, dynamically
{
	UI32 i = 0;
	UI32 raceCount = 0;
	bool done = false;

	char sect[512];
	const char *tag = NULL;
	const char *data = NULL;

	while( !done )
	{
		sprintf( sect, "RACE %d", raceCount );
		ScriptSection *tempSect = FileLookup->FindEntry( sect, race_def );
		if( tempSect == NULL )
			done = true;
		else
			raceCount++;
	}

	races.resize( raceCount );
	for( i = 0; i < raceCount; i++ )
		races[i] = new CRace( raceCount );

	ScriptSection *CombatMods = FileLookup->FindEntry( "COMBAT MODS", race_def );
	if( CombatMods != NULL )
	{
		tag = CombatMods->First();
		if( tag == NULL )	// location didn't exist!!!
		{
			DefaultInitCombat();
		}
		else
		{
			if( strcmp( tag, "MODCOUNT" ) )
			{
				Console << "MODCOUNT must come before any entries!" << myendl;
				DefaultInitCombat();
			}
			else
			{
				UI32 modifierCount = makeNum( CombatMods->GrabData() );
				if( modifierCount < 4 )
				{
					Console << "MODCOUNT must be more >= 4, or it uses the defaults!" << myendl;
					DefaultInitCombat();
				}
				else
				{
					combat.resize( modifierCount );
					for( i = 0; i < modifierCount; i++ )
					{
						tag = CombatMods->Next();
						data = CombatMods->GrabData();
						if( data != NULL )
							combat[i].value = static_cast<UI08>(makeNum( data ));
						else
							combat[i].value = 100;
					}
				}
			}
		}
	}
	else
	{
		DefaultInitCombat();
	}
	for( i = 0; i < raceCount; i++ )
	{
		races[i]->Load( i, combat.size() );
	}
}

SI08 cRaces::Compare( PLAYER player1, PLAYER player2 ) const
// PRE: player1.race and player2.race are below the maximum number of races
// POST: Returns 0 if no enemy or ally, 1 if enemy, or 2 if ally
{
	RACEID r1 = chars[player1].GetRace();
	RACEID r2 = chars[player2].GetRace();
	if( r1 >= races.size() ) // invalid race?
	{
		Console.Error( 2, Dictionary->GetEntry( 368 ), player1, r1 );
		return 0;
	}
	else if( r2 >= races.size() ) // invalid race?
	{
		Console.Error( 2, Dictionary->GetEntry( 368 ), player2, r2 );
		return 0;
	}
	else
		return races[r1]->RaceRelation( r2 ); // enemy race
}

SI08 cRaces::Compare( CChar *player1, CChar *player2 ) const
{
	if( player1 == NULL || player2 == NULL )
		return 0;
	RACEID r1 = player1->GetRace();
	RACEID r2 = player2->GetRace();
	if( r1 >= races.size() || r2 >= races.size() )
		return 0;
	return races[r1]->RaceRelation( r2 );
}

SI08 cRaces::CompareByRace( RACEID race1, RACEID race2 ) const
// PRE: race1 and race2 are below the maximum number of races
// POST: Returns 0 if no enemy or ally, 1 if enemy, or 2 if ally
{
	if( race1 >= races.size() ) // invalid race?
	{
		return 0;
	}
	else if( race2 >= races.size() ) // invalid race?
	{
		return 0;
	}
	else
		return races[race1]->RaceRelation( race2 ); // enemy race
}

void cRaces::gate( CChar *s, RACEID x, bool always )
// PRE:	PLAYER s is valid, x is a race index and always = 0 or 1
// POST: PLAYER s belongs to new race x or doesn't change based on restrictions
{
	if( s == NULL )
		return;
	CItem *hairobject = NULL, *beardobject = NULL;
	UI16 hairColor = 0;

	CRace *pRace = Race( x );
	if( pRace == NULL )
		return;

	cSocket *mSock = calcSocketObjFromChar( s );
	if( !pRace->IsPlayerRace() )
	{
		sysmessage( mSock, 369 );
		return;
	}
	if( s->GetRaceGate() == 65535 || always )
	{
		std::map< UI08, std::string > lossMap;

		lossMap[STRENGTH] = "strength";
		lossMap[DEXTERITY] = "speed";
		lossMap[INTELLECT] = "wisdom";
	
		beardobject = s->GetItemAtLayer( 0x10 );
		hairobject	= s->GetItemAtLayer( 0x0B );
		if( pRace->GenderRestriction() != 0 )
		{
			if( pRace->GenderRestriction() != FEMALE && s->GetID() == 0x0191 )
			{
				sysmessage( mSock, 370 );
				return;
			}
			if( pRace->GenderRestriction() != MALE && s->GetID() == 0x0190 )
			{
				sysmessage( mSock, 370 );
				return;
			}
		}
		Effects->staticeffect( s, 0x373A, 0, 15 );
		Effects->PlaySound( s, 0x01E9 );
		s->SetRaceGate( x );
		s->SetRace( x );

		SI16 stats[3];
		stats[0] = s->ActualStrength();
		stats[1] = s->ActualDexterity();
		stats[2] = s->ActualIntelligence();

		for( UI08 counter = STRENGTH; counter <= INTELLECT; counter++ )
		{
			if( stats[counter-STRENGTH] > pRace->Skill( counter ) )
			{
				sysmessage( mSock, 371, lossMap[counter].c_str() );
				stats[counter-STRENGTH] = pRace->Skill( counter );
			}
			else 
				stats[counter-STRENGTH] = 0;
		}
		if( stats[0] != 0 )
			s->SetStrength( stats[0] );
		if( stats[1] != 0 )
			s->SetDexterity( stats[1] );
		if( stats[2] != 0 )
			s->SetIntelligence( stats[2] );
		if( mSock != NULL )
			statwindow( mSock, s );

		if( hairobject != NULL )
		{
			if( pRace->IsHairRestricted() )
			{
				hairColor = ( hairobject->GetColour() );
				if( pRace->IsValidHair( hairColor ) )
				{
					hairColor = RandomHair( x );
					hairobject->SetColour( hairColor );
					RefreshItem( hairobject );
				}
			}
			if( pRace->NoHair() )
			{
				Items->DeleItem( hairobject );
				hairobject = NULL;
			}
		}
		if( pRace->RequiresBeard() && s->GetID() == 0x0190 && beardobject == NULL )
		{
			if( pRace->IsBeardRestricted() )
				hairColor = RandomBeard( x );
			else
				hairColor = 0x0480;
			CItem *n = Items->SpawnItem( NULL, s, 1, "#", false, 0x204C, hairColor, false, false );
			if( n != NULL )
			{
				n->SetLayer( 0x10 );
				if( n->SetCont( s ) )
				{
					RefreshItem( n );
					beardobject = n;
				}
			}
		}
		if( beardobject != NULL )
		{
			if( pRace->IsBeardRestricted() )
			{
				hairColor = beardobject->GetColour();
				if( pRace->IsValidBeard( hairColor ) )
				{
					hairColor = RandomBeard( x );
					beardobject->SetColour( hairColor );
					RefreshItem( beardobject );
				}
			}
			if( pRace->NoBeard() )
			{
				Items->DeleItem( beardobject );
				beardobject = NULL;
			}
		}
		if( pRace->IsSkinRestricted() )	// do we have a limited skin colour range?
		{
			hairColor = s->GetSkin();
			if( pRace->IsValidSkin( hairColor ) )	// if not in range
			{
				hairColor = RandomSkin( x );	// get random skin in range
				s->SetSkin( hairColor );
				s->SetxSkin( hairColor );
				s->SetOrgSkin( hairColor );
			}
		}
		s->Update();
	}
	else 
		sysmessage( mSock, 372 );
}

bool cRaces::beardInRange( COLOUR color, RACEID x ) const
// PRE:	Race is valid
// POST:	Returns whether colour is valid for beard
{
	if( InvalidRace( x ) )
		return false;
	return races[x]->IsValidBeard( color );
}
bool cRaces::skinInRange( COLOUR color, RACEID x ) const
// PRE:	Race is valid
// POST:	Returns whether colour is valid for skin
{
	if( InvalidRace( x ) )
		return false;
	return races[x]->IsValidSkin( color );
}
bool cRaces::hairInRange( COLOUR color, RACEID x ) const
// PRE:	Race is valid
// POST:	Returns whether colour is valid for hair
{
	if( InvalidRace( x ) )
		return false;
	return races[x]->IsValidHair( color );
}

SKILLVAL cRaces::Skill( int skill, RACEID race ) const
// PRE:	skill is valid, race is valid
// POST:	returns skill bonus associated with race
{ 
	if( InvalidRace( race ) || skill >= ALLSKILLS )
		return 0;
	return races[race]->Skill( skill );
}

void cRaces::Skill( int skill, int value, RACEID race )
// PRE:	skill is valid, value is valid, race is valid
// POST:	sets race's skill bonus to value
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->Skill( value, skill );
}

GENDER cRaces::GenderRestrict( RACEID race ) const
// PRE:	Race is valid
// POST:	returns whether race's gender is restricted, and if so, which gender
//		0 - none 1 - male 2 - female
{ 
	if( InvalidRace( race ) )
		return MALE;
	return races[race]->GenderRestriction();
}

void cRaces::GenderRestrict( GENDER gender, RACEID race )
// PRE:	Race is valid, gender is valid
// POST:	Sets race's gender restriction
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->GenderRestriction( gender );
}

bool cRaces::LightAffect( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by light
{ 
	if( InvalidRace( race ) )
		return false;
	return races[race]->AffectedByLight();
}

void cRaces::LightAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->AffectedByLight( value );
}

bool cRaces::RequireBeard( RACEID race ) const
// PRE:	race is valid
// POST:	returns whether race must have beard
{ 
	if( InvalidRace( race ) )
		return false;
	return races[race]->RequiresBeard();
}

void cRaces::RequireBeard( bool value, RACEID race )
// PRE:	Race is valid, and value is true or false
// POST:	sets whether race requires a beard
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->RequiresBeard( value ); 
}

void cRaces::ArmorRestrict( RACEID race, ARMORCLASS value )
// PRE:	Race is valid, value is a valid armorclass
// POST:	sets the armor class of race
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->ArmourClassRestriction( value ); 
}

ARMORCLASS cRaces::ArmorRestrict( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns armor class of race
{ 
	if( InvalidRace( race ) )
		return 0;
	return races[race]->ArmourClassRestriction();
}

COLOUR cRaces::RandomSkin( RACEID x ) const
// PRE:	Race is valid
// POST:	returns a valid skin colour for the race
{ 
	if( InvalidRace( x ) )
		return 0000;
	return races[x]->RandomSkin();
}

COLOUR cRaces::RandomHair( RACEID x ) const
// PRE:	Race is valid
// POST:	returns a valid hair colour for the race
{ 
	if( InvalidRace( x ) )
		return 0000;
	return races[x]->RandomHair();
}

COLOUR cRaces::RandomBeard( RACEID x ) const
// PRE:	Race is valid
// POST:	returns a valid beard colour for the race
{ 
	if( InvalidRace( x ) )
		return 0;
	return races[x]->RandomBeard();
}

bool cRaces::beardRestricted( RACEID x ) const
// PRE:	race is valid
// POST:	returns true if race's beard colour is restricted
{ 
	if( InvalidRace( x ) )
		return false;
	return races[x]->IsBeardRestricted();
}

bool cRaces::hairRestricted( RACEID x ) const
// PRE:	race is valid
// POST:	returns true if race's hair colour is restricted
{ 
	if( InvalidRace( x ) )
		return false;
	return races[x]->IsHairRestricted();
}

bool cRaces::skinRestricted( RACEID x ) const
// PRE:	race is valid
// POST:	returns true if race's skin colour is restricted
{ 
	if( InvalidRace( x ) )
		return false;
	return races[x]->IsSkinRestricted();
}

SI32 cRaces::DamageFromSkill( int skill, RACEID x ) const
// PRE:	x is valid, skill is valid
// POST:	returns chance difference to race x in skill skill
{
//	assert( skill <= SKILLS );
	if( InvalidRace( x ) )
		return 0;
	if( skill >= ALLSKILLS )
		return 0;
	int modifier = races[x]->Skill( skill );
	if( modifier >= static_cast<int>(combat.size()) )
		return -(combat[modifier].value);
	else
		return (combat[modifier].value);
	return 0;
}

SI32 cRaces::FightPercent( int skill, RACEID x ) const
// PRE:	x is valid, skill is valid
// POST:	returns positive/negative fight damage bonus for race x with skill skill
{
	if( InvalidRace( x ) )
		return 100;
	int modifier = races[x]->Skill( skill );
	int divValue = combat[modifier].value / 10;
	divValue = divValue / 10;
	if( divValue == 0 )
		return 100;
	if( modifier >= static_cast<int>(combat.size()) )
		return -(int)(100/(R32)divValue);
	else
		return (int)(100/(R32)divValue);
	return 100;
}

void cRaces::RacialInfo( RACEID race, RACEID toSet, RACEREL value )
// PRE:		race and toSet are valid races, value is a valid relation
// POST:	the relation between race and toset is set to value
{
	if( InvalidRace( race ) )
		return;
	races[race]->RaceRelation( value, toSet );
}
void cRaces::RacialEnemy( RACEID race, RACEID enemy )
// PRE:		race and enemy are valid
// POST:	enemy is race's enemy
{
	if( InvalidRace( race ) )
		return;
	RacialInfo( race, enemy, MIN_ENEMY );
}
void cRaces::RacialAlly( RACEID race, RACEID ally )
// PRE:		race and ally are valid
// POST:	ally is race's ally
{
	if( InvalidRace( race ) )
		return;
	RacialInfo( race, ally, MIN_ALLY );
}
void cRaces::RacialNeutral( RACEID race, RACEID neutral )
// PRE:		race and neutral are valid
// POST:	neutral is neutral to race
{
	if( InvalidRace( race ) )
		return;
	RacialInfo( race, neutral, NEUTRAL );
}


SKILLVAL cRaces::LanguageMin( RACEID x ) const
//  PRE:		x is a valid race
// POST:		returns race's minimum skill for language
{
	return races[x]->LanguageMin();
}

void cRaces::LanguageMin( SKILLVAL toSetTo, RACEID race )
//  PRE:		race and toSetTo is valid
// POST:		race's min language requirement is set to toSetTo
{
	if( InvalidRace( race ) )
		return;
	races[race]->LanguageMin( toSetTo );
}


// SNOW SECTION

void cRaces::SnowDamage( RACEID race, SI08 value )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	if( InvalidRace( race ) )
		return;
	Damage( race, SNOW, value );
}

bool cRaces::SnowAffect( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by light
{
	if( InvalidRace( race ) )
		return false;
	return races[race]->AffectedBySnow();
}

void cRaces::SnowAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{
	if( InvalidRace( race ) )
		return;
	races[race]->AffectedBySnow( value );
}


// RAIN SECTION

void cRaces::RainDamage( RACEID race, SI08 value )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by light
{ 
	if( InvalidRace( race ) )
		return;
	Damage( race, RAIN, value );
}

bool cRaces::RainAffect( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by light
{ 
	if( InvalidRace( race ) )
		return false;
	return races[race]->AffectedByRain();
}

void cRaces::RainAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by rain
{
	if( InvalidRace( race ) )
		return;
	races[race]->AffectedByRain( value );
}

// COLD SECTION

void cRaces::ColdDamage( RACEID race, SI08 value )
// PRE:	Race is valid, value is true or false
// POST:	sets how much race is affected by cold
{
	if( InvalidRace( race ) )
		return;
	Damage( race, COLD, value ); 
}

bool cRaces::ColdAffect( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by cold
{
	if( InvalidRace( race ) )
		return false;
	return races[race]->AffectedByCold();
}

void cRaces::ColdAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by cold
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->AffectedByCold( value );
}


// HEAT SECTION

void cRaces::HeatDamage( RACEID race, SI08 value )
// PRE:	Race is valid, value is true or false
// POST:	sets how much race is affected by heat
{ 
	if( InvalidRace( race ) )
		return;
	Damage( race, HEAT, value );
}

bool cRaces::HeatAffect( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by heat
{
	if( InvalidRace( race ) )
		return false;
	return races[race]->AffectedByHeat();
}

void cRaces::HeatAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by heat
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->AffectedByHeat( value );
}


// LIGHT SECTION

void cRaces::LightDamage( RACEID race, SI08 value )
// PRE:	Race is valid, value is valid UI08
// POST:	sets how much race is affected by light
{
	if( InvalidRace( race ) )
		return;
	Damage( race, LIGHT, value ); 
}

SI08 cRaces::LightDamage( RACEID race ) const
{
	if( InvalidRace( race ) )
		return 0;
	return Damage( race, LIGHT );
}

SI08 cRaces::RainDamage( RACEID race ) const
{
	if( InvalidRace( race ) )
		return 0;
	return Damage( race, RAIN );
}

SI08 cRaces::SnowDamage( RACEID race ) const
{
	if( InvalidRace( race ) )
		return 0;
	return Damage( race, SNOW );
}

SI08 cRaces::HeatDamage( RACEID race ) const
{
	if( InvalidRace( race ) )
		return 0;
	return Damage( race, HEAT );
}

SI08 cRaces::ColdDamage( RACEID race ) const
{
	if( InvalidRace( race ) )
		return 0;
	return Damage( race, COLD );
}

void cRaces::LightLevel( RACEID race, LIGHTLEVEL value )
// PRE:	Race is valid, value is a valid light level
// POST:	the light level that race burns at is set to value
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->LightLevel( value ); 
}

LIGHTLEVEL cRaces::LightLevel( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns the light level that race burns at
{ 
	if( InvalidRace( race ) )
		return 0;
	return races[race]->LightLevel(); 
}

void cRaces::LightSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from light
{ 
	if( InvalidRace( race ) )
		return;
	Secs( race, LIGHT, value );
}

SECONDS cRaces::LightSecs( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from light
{ 
	if( InvalidRace( race ) )
		return 1;
	return Secs( race, LIGHT ); 
}

// LIGHTNING SECTION

void cRaces::LightningDamage( RACEID race, SI08 value )
// PRE:	Race is valid, value is valid UI08
// POST:	sets how much race is affected by lightning
{ 
	if( InvalidRace( race ) )
		return;
	Damage( race, LIGHTNING, value );
}

bool cRaces::LightningAffect( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns if race is affected adversely by lightning
{ 
	if( InvalidRace( race ) )
		return false;
	return races[race]->AffectedByLightning();
}

void cRaces::LightningAffect( bool value, RACEID race )
// PRE:	Race is valid, value is true or false
// POST:	sets whether race is affected by lightning
{ 
	if( InvalidRace( race ) )
		return;
	races[race]->AffectedByLightning( value );
}

void cRaces::RainSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from rain
{ 
	if( InvalidRace( race ) )
		return;
	Secs( race, RAIN, value ); 
}

SECONDS cRaces::RainSecs( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from rain
{ 
	if( InvalidRace( race ) )
		return 1;
	return Secs( race, RAIN ); 
}

void cRaces::SnowSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from snow
{ 
	if( InvalidRace( race ) )
		return;
	Secs( race, SNOW, value ); 
}

SECONDS cRaces::SnowSecs( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from snow
{ 
	if( InvalidRace( race ) )
		return 1;
	return Secs( race, SNOW ); 
}

void cRaces::HeatSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from heat
{ 
	if( InvalidRace( race ) )
		return;
	Secs( race, HEAT, value ); 
}

SECONDS cRaces::HeatSecs( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from heat
{ 
	if( InvalidRace( race ) )
		return 1;
	return Secs( race, HEAT ); 
}

void cRaces::ColdSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from cold
{ 
	if( InvalidRace( race ) )
		return;
	Secs( race, COLD, value ); 
}

SECONDS cRaces::ColdSecs( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from cold
{ 
	if( InvalidRace( race ) )
		return 1;
	return Secs( race, COLD ); 
}

void cRaces::LightningSecs( RACEID race, SECONDS value )
// PRE:	Race is valid, value is a valid number of seconds
// POST:	sets the number of seconds between burns for race from lightning
{ 
	if( InvalidRace( race ) )
		return;
	Secs( race, LIGHTNING, value );
}

SECONDS cRaces::LightningSecs( RACEID race ) const
// PRE:	Race is valid
// POST:	Returns number of seconds between burns for race from lightning
{ 
	if( InvalidRace( race ) )
		return 1;
	return Secs( race, LIGHTNING ); 
}

SECONDS cRaces::Secs( RACEID race, WeatherType element ) const
// PRE:	Race is valid, element is an element of weather
// POST:	Returns number of seconds between burns for race from element
{
	if( InvalidRace( race ) )
		return 1;
	return races[race]->WeatherSeconds( element );
}

void cRaces::Secs( RACEID race, WeatherType element, SECONDS value )
// PRE:	Race is valid, element is element of weather, value is seconds
// POST:	Sets number of seconds between burns for race from element
{
	if( InvalidRace( race ) )
		return;
	races[race]->WeatherSeconds( value, element );
}

SI08 cRaces::Damage( RACEID race, WeatherType element ) const
// PRE:	Race is valid, element is an element of weather
// POST:	Returns damage incurred by race from element
{
	if( InvalidRace( race ) )
		return 1;
	return races[race]->WeatherDamage( element );
}

void cRaces::Damage( RACEID race, WeatherType element, SI08 damage )
// PRE:	race is valid, element is element of weather, damage is > -127 && < 127
// POST:	sets damage incurred by race from element
{
	if( InvalidRace( race ) )
		return;
	races[race]->WeatherDamage( damage, element );
}

LIGHTLEVEL cRaces::VisLevel( RACEID x ) const
// PRE:	x is valid
// POST:	returns light level bonus of race x
{
	if( InvalidRace( x ) )
		return 0;
	return races[x]->NightVision();
}
void cRaces::VisLevel( RACEID x, LIGHTLEVEL bonus )
// PRE:	x is valid
// POST:	sets race's light level bonus to bonus
{
	if( InvalidRace( x ) )
		return;
	races[x]->NightVision( bonus );
}

RANGE cRaces::VisRange( RACEID x ) const
// PRE:	x is valid
// POST:	Returns distance that race can see
{
	if( InvalidRace( x ) )
		return 0;
	return races[x]->VisibilityRange();
}

void cRaces::VisRange( RACEID x, RANGE range )
// PRE:	x is valid and range is valid
// POST:	sets distance that race can see to range
{
	if( InvalidRace( x ) )
		return ;
	races[x]->VisibilityRange( range );
}

bool cRaces::NoBeard( RACEID x ) const
{
	if( InvalidRace( x ) )
		return false;
	return races[x]->NoBeard();
}

void cRaces::NoBeard( bool value, RACEID race )
{
	if( InvalidRace( race ) )
		return;
	races[race]->NoBeard( value );
}

void cRaces::debugPrint( RACEID x )
{
	if( InvalidRace( x ) )
		return;
	Console << "Race ID: " << x << myendl;
	Console << "Race: " << races[x]->Name() << myendl;
	if( races[x]->RequiresBeard() ) 
		Console << "Req Beard: Yes" << myendl;
	else 
		Console << "Req Beard: No" << myendl;
	if( races[x]->NoBeard() ) 
		Console << "No Beard: Yes" << myendl;
	else 
		Console << "No Beard: No" << myendl;
	if( races[x]->IsPlayerRace() ) 
		Console << "Player Race: Yes" << myendl;
	else 
		Console << "Player Race: No" << myendl;
	Console << "Restrict Gender: " << races[x]->GenderRestriction() << myendl;
	Console << "LightLevel: " << races[x]->LightLevel() << myendl;
	Console << "NightVistion: " << races[x]->NightVision() << myendl;
	Console << "ArmorRestrict: " << races[x]->ArmourClassRestriction() << myendl;
	Console << "LangMin: " << races[x]->LanguageMin() << myendl;
	Console << "Vis Distance: " << races[x]->VisibilityRange() << myendl << myendl;
}

void cRaces::debugPrintAll( void )
{
	for( RACEID x = 0; x < races.size(); ++x )
		debugPrint( x );
}

bool cRaces::IsPlayerRace( RACEID race ) const
{
	if( InvalidRace( race ) )
		return false;
	return races[race]->IsPlayerRace();
}

void cRaces::IsPlayerRace( RACEID x, bool value )
// PRE:		x is a valid race, value is either true or false
// POST:	sets if x is a player race or not
{
	if( InvalidRace( x ) )
		return;
	races[x]->IsPlayerRace( value );
}

SKILLVAL CRace::Skill( int skillNum ) const
{
	return iSkills[skillNum];
}
const char * CRace::Name( void ) const
{
	return raceName.c_str();
}
bool CRace::RequiresBeard( void ) const
{
	return ( (bools&0x0001) == 0x0001 );
}
bool CRace::NoBeard( void ) const
{
	return ( (bools&0x0002) == 0x0002 );
}
bool CRace::IsPlayerRace( void ) const
{
	return ( (bools&0x0004) == 0x0004 );
}
bool CRace::AffectedByLight( void ) const
{
	return ( (bools&0x0008) == 0x0008 );
}
bool CRace::AffectedByRain( void ) const
{
	return ( (bools&0x0010) == 0x0010 );
}
bool CRace::AffectedByCold( void ) const
{
	return ( (bools&0x0020) == 0x0020 );
}
bool CRace::AffectedByHeat( void ) const
{
	return ( (bools&0x0040) == 0x0040 );
}
bool CRace::AffectedByLightning( void ) const
{
	return ( (bools&0x0080) == 0x0080 );
}
bool CRace::AffectedBySnow( void ) const
{
	return ( (bools&0x0100) == 0x0100 );
}

bool CRace::NoHair( void ) const
{
	return ( (bools&0x0200) == 0x0200 );
}

GENDER CRace::GenderRestriction( void ) const
{
	return restrictGender;
}
LIGHTLEVEL CRace::LightLevel( void ) const
{
	return lightLevel;
}
LIGHTLEVEL CRace::NightVision( void ) const
{
	return nightVision;
}
ARMORCLASS CRace::ArmourClassRestriction( void ) const
{
	return armourRestrict;
}
SECONDS CRace::WeatherSeconds( WeatherType iNum ) const
{
	return weathSecs[iNum];
}
SI08 CRace::WeatherDamage( WeatherType iNum ) const
{
	return weathDamage[iNum];
}

SKILLVAL CRace::LanguageMin( void ) const
{
	return languageMin;
}
RANGE CRace::VisibilityRange( void ) const
{
	return visDistance;
}

void CRace::Skill( SKILLVAL newValue, int iNum )
{
	iSkills[iNum] = newValue;
}
void CRace::Name( const char *newName )
{
	raceName = newName;
}
void CRace::RequiresBeard( bool newValue )
{
	if( newValue )
		bools |= 0x0001;
	else
		bools &= 0xFFFE;
}
void CRace::NoBeard( bool newValue )
{
	if( newValue )
		bools |= 0x0002;
	else
		bools &= 0xFFFD;
}
void CRace::IsPlayerRace( bool newValue )
{
	if( newValue )
		bools |= 0x0004;
	else
		bools &= 0xFFFB;
}
void CRace::AffectedByLight( bool newValue )
{
	if( newValue )
		bools |= 0x0008;
	else
		bools &= 0xFFF7;
}
void CRace::AffectedByRain( bool newValue )
{
	if( newValue )
		bools |= 0x0010;
	else
		bools &= 0xFFEF;
}
void CRace::AffectedByCold( bool newValue )
{
	if( newValue )
		bools |= 0x0020;
	else
		bools &= 0xFFDF;
}
void CRace::AffectedByHeat( bool newValue )
{
	if( newValue )
		bools |= 0x0040;
	else
		bools &= 0xFFBF;
}
void CRace::AffectedByLightning( bool newValue )
{
	if( newValue )
		bools |= 0x0080;
	else
		bools &= 0xFF7F;
}
void CRace::AffectedBySnow( bool newValue )
{
	if( newValue )
		bools |= 0x0100;
	else
		bools &= 0xFEFF;
}

void CRace::NoHair( bool newValue )
{
	if( newValue )
		bools |= 0x0200;
	else
		bools &= 0xFDFF;
}

void CRace::GenderRestriction( GENDER newValue )
{
	restrictGender = newValue;
}
void CRace::LightLevel( LIGHTLEVEL newValue )
{
	lightLevel = newValue;
}
void CRace::NightVision( LIGHTLEVEL newValue )
{
	nightVision = newValue;
}
void CRace::ArmourClassRestriction( ARMORCLASS newValue )
{
	armourRestrict = newValue;
}
void CRace::WeatherSeconds( SECONDS newValue, WeatherType iNum )
{
	weathSecs[iNum] = newValue;
}
void CRace::WeatherDamage( SI08 newValue, WeatherType iNum )
{
	weathDamage[iNum] = newValue;
}
void CRace::LanguageMin( SKILLVAL newValue )
{
	languageMin = newValue;
}
void CRace::VisibilityRange( RANGE newValue )
{
	visDistance = newValue;
}

CRace::CRace() : bools( 4 ), visDistance( 0 ), nightVision( 0 ), armourRestrict( 0 ), lightLevel( 1 ),
restrictGender( 0 ), languageMin( 0 ), poisonResistance( 0.0f ), magicResistance( 0.0f )
{
	memset( &iSkills[0], 0, sizeof( SKILLVAL ) * SKILLS );
	memset( &weathDamage[0], 0, sizeof( SI08 ) * WEATHNUM );
	memset( &weathSecs[0], 0, sizeof( SECONDS ) * WEATHNUM );
	
	Skill( 100, STRENGTH );
	Skill( 100, DEXTERITY );
	Skill( 100, INTELLECT );
	HPModifier( 0 );
	ManaModifier( 0 );
	StamModifier( 0 );

}


CRace::CRace( int numRaces ) : bools( 4 ), visDistance( 0 ), nightVision( 0 ), armourRestrict( 0 ), lightLevel( 1 ),
restrictGender( 0 ), languageMin( 0 ), poisonResistance( 0.0f ), magicResistance( 0.0f )
{
	NumEnemyRaces( numRaces );

	memset( &iSkills[0], 0, sizeof( SKILLVAL ) * SKILLS );
	memset( &weathDamage[0], 0, sizeof( SI08 ) * WEATHNUM );
	memset( &weathSecs[0], 0, sizeof( SECONDS ) * WEATHNUM );

	Skill( 100, STRENGTH );
	Skill( 100, DEXTERITY );
	Skill( 100, INTELLECT );
	HPModifier( 0 );
	ManaModifier( 0 );
	StamModifier( 0 );

}
void CRace::NumEnemyRaces( int iNum )
{
	racialEnemies.resize( iNum );
}
SI08 CRace::RaceRelation( RACEID race ) const
{
	return racialEnemies[race];
}

COLOUR CRace::RandomSkin( void ) const
{
	if( !IsSkinRestricted() )
		return 0;
	int sNum = RandomNum( 0, skinColours.size() - 1 );
	return (COLOUR)RandomNum( skinColours[sNum].cMin, skinColours[sNum].cMax );
}
COLOUR CRace::RandomHair( void ) const
{
	if( !IsHairRestricted() )
		return 0;
	int sNum = RandomNum( 0, hairColours.size() - 1 );
	return (COLOUR)RandomNum( hairColours[sNum].cMin, hairColours[sNum].cMax );
}
COLOUR CRace::RandomBeard( void ) const
{
	if( !IsBeardRestricted() )
		return 0;
	int sNum = RandomNum( 0, beardColours.size() - 1 );
	return (COLOUR)RandomNum( beardColours[sNum].cMin, beardColours[sNum].cMax );
}

bool CRace::IsSkinRestricted( void ) const
{
	return ( skinColours.size() != 0 );
}
bool CRace::IsHairRestricted( void ) const
{
	return ( hairColours.size() != 0 );
}
bool CRace::IsBeardRestricted( void ) const
{
	return ( beardColours.size() != 0 );
}

bool CRace::IsValidSkin( COLOUR val ) const
{
	if( !IsSkinRestricted() )
		return true;
	for( UI32 i = 0; i < skinColours.size(); i++ )
	{
		if( val >= skinColours[i].cMin && val <= skinColours[i].cMax )
			return true;
	}
	return false;
}
bool CRace::IsValidHair( COLOUR val ) const
{
	if( !IsHairRestricted() )
		return true;
	for( UI32 i = 0; i < hairColours.size(); i++ )
	{
		if( val >= hairColours[i].cMin && val <= hairColours[i].cMax )
			return true;
	}
	return false;
}
bool CRace::IsValidBeard( COLOUR val ) const
{
	if( !IsBeardRestricted() )
		return true;
	for( UI32 i = 0; i < beardColours.size(); i++ )
	{
		if( val >= beardColours[i].cMin && val <= beardColours[i].cMax )
			return true;
	}
	return false;
}

void CRace::RaceRelation( SI08 value, RACEID race )
{
	racialEnemies[race] = value;
}

SI16 CRace::HPModifier( void ) const
{
	return HPMod;
}

void CRace::HPModifier( SI16 value )
{
	if( value > -100 )
		HPMod = value;
	else
		HPMod = -99;
}

SI16 CRace::ManaModifier( void ) const
{
	return ManaMod;
}

void CRace::ManaModifier( SI16 value )
{
	if( value > -100 )
		ManaMod = value;
	else
		ManaMod = -99;
}

SI16 CRace::StamModifier( void ) const
{
	return StamMod;
}

void CRace::StamModifier( SI16 value )
{
	if( value > -100 )
		StamMod = value;
	else
		StamMod = -99;
}

void CRace::Load( SI32 sectNum, int modCount )
{
	char sect[16];
	const char *tag = NULL;
	const char *data = NULL;
	SI32 raceDiff = 0;
	sprintf( sect, "RACE %d", sectNum );
	ScriptSection *RacialPart = FileLookup->FindEntry( sect, race_def );

	COLOUR beardMin = 0, skinMin = 0, hairMin = 0;

	for( tag = RacialPart->First(); !RacialPart->AtEnd(); tag = RacialPart->Next() )
	{
		data = RacialPart->GrabData();
		switch( tag[0] )
		{

		case 'a':
		case 'A':
			if( !strcmp( "ARMORREST", tag ) )
				ArmourClassRestriction( static_cast<ARMORCLASS>(makeNum( data )) );		// 8 classes, value 0 is all, else it's a bit comparison
			break;

		case 'b':
		case 'B':
			if( !strcmp( "BEARDMIN", tag ) )
				beardMin = (UI16)makeNum( data );
			else if( !strcmp( "BEARDMAX", tag ) )
				beardColours.push_back( ColourPair( beardMin, (UI16)makeNum( data ) ) );
			break;

		case 'c':
		case 'C':
			if( !strcmp( "COLDAFFECT", tag ) )	// are we affected by light?
				AffectedByCold( true );
			else if( !strcmp( "COLDDAMAGE", tag ) )	// how much damage to take from light
				WeatherDamage( static_cast<SI08>(makeNum( data )), COLD );
			else if( !strcmp( "COLDSECS", tag ) )		// how often light affects in secs
				WeatherSeconds( static_cast<SECONDS>(makeNum( data )), COLD );
			break;

		case 'd':
		case 'D':
			if( !strcmp( "DEXCAP", tag ) )
				Skill( static_cast<SKILLVAL>(makeNum( data )), DEXTERITY );
			break;

		case 'g':
		case 'G':
			if( !strcmp( "GENDER", tag ) )
			{
				if( !strcmp( "MALE", data ) )
					GenderRestriction( MALE );
				else if( !strcmp( "FEMALE", data ) )
					GenderRestriction( FEMALE );
				else
					GenderRestriction( MALE );
			}
			break;

		case 'h':
		case 'H':
			if( !strcmp( "HAIRMIN", tag ) )
				hairMin = (UI16)makeNum( data );
			else if( !strcmp( "HAIRMAX", tag ) )
				hairColours.push_back( ColourPair( hairMin, (UI16)makeNum( data ) ) );
			else if( !strcmp( "HEATAFFECT", tag ) )	// are we affected by light?
				AffectedByHeat( true );
			else if( !strcmp( "HEATDAMAGE", tag ) )	// how much damage to take from light
				WeatherDamage( static_cast<SI08>(makeNum( data )), HEAT );
			else if( !strcmp( "HEATSECS", tag ) )		// how often light affects in secs
				WeatherSeconds( static_cast<SECONDS>(makeNum( data )), HEAT );
			else if( !strcmp( "HPMOD", tag ) ) // how much additional percent of strength are hitpoints
				HPModifier( static_cast<SI16>(makeNum( data )) );
			break;

		case 'i':
		case 'I':
			if( !strcmp( "INTCAP", tag ) )
				Skill( static_cast<SKILLVAL>(makeNum( data )), INTELLECT );
			break;

		case 'l':
		case 'L':
			if( !strcmp( "LIGHTAFFECT", tag ) )	// are we affected by light?
				AffectedByLight( true );
			else if( !strcmp( "LIGHTDAMAGE", tag ) )	// how much damage to take from light
				WeatherDamage( static_cast<SI08>(makeNum( data )), LIGHT );
			else if( !strcmp( "LIGHTLEVEL", tag ) )	// light level at which to take damage
				LightLevel( static_cast<LIGHTLEVEL>(makeNum( data )) );
			else if( !strcmp( "LIGHTSECS", tag ) )		// how often light affects in secs
				WeatherSeconds( static_cast<SECONDS>(makeNum( data )), LIGHT );

			else if( !strcmp( "LIGHTNINGAFFECT", tag ) )	// are we affected by light?
				AffectedByLightning( true );
			else if( !strcmp( "LIGHTNINGDAMAGE", tag ) )	// how much damage to take from light
				WeatherDamage( static_cast<SI08>(makeNum( data) ), LIGHTNING );
			else if( !strcmp( "LIGHTNINGSECS", tag ) )		// how often light affects in secs
				WeatherSeconds( static_cast<SECONDS>(makeNum( data )), LIGHTNING );
			else if( !strcmp( "LANGUAGEMIN", tag ) ) // set language min 
				LanguageMin( static_cast<SKILLVAL>(makeNum( data )) ); 
			break;

		case 'm':
		case 'M':
			if( !strcmp( "MAGICRESISTANCE", tag ) )	// magic resistance?
				MagicResistance( static_cast<R32>(atof( data ) ));
			else if( !strcmp( "MANAMOD", tag ) ) // how much additional percent of int are mana
				ManaModifier( static_cast<SI16>(makeNum( data )) );
			break;

		case 'n':
		case 'N':
			if( !strcmp( tag, "NAME" ) )
				Name( data );
			else if( !strcmp( "NOBEARD", tag ) )
				NoBeard( true );
			else if( !strcmp( "NIGHTVIS", tag ) )					// night vision level... light bonus
				NightVision( static_cast<LIGHTLEVEL>(makeNum( data )) );
			break;

		case 'p':
		case 'P':
			if( !strcmp( "PLAYERRACE", tag ) ) // is it a player race?
				IsPlayerRace( (makeNum( data ) != 0) );
			else if( !strcmp( "POISONRESISTANCE", tag ) )	// poison resistance?
				PoisonResistance( static_cast<R32>(atof( data ) ));
			else if( !strcmp( "PARENTRACE", tag ) )
			{
				CRace *pRace = Races->Race( static_cast<RACEID>(makeNum( tag )) );
				if( pRace != NULL )
					(*this) = (*pRace);
			}
			break;

		case 'r':
		case 'R':
			if( !strcmp( "REQUIREBEARD", tag ) )
				RequiresBeard( true );
			else if( !strcmp( "RAINAFFECT", tag ) )	// are we affected by light?
				AffectedByRain( true );
			else if( !strcmp( "RAINDAMAGE", tag ) )	// how much damage to take from light
				WeatherDamage( static_cast<SI08>(makeNum( data )), RAIN );
			else if( !strcmp( "RAINSECS", tag ) )		// how often light affects in secs
				WeatherSeconds( static_cast<SECONDS>(makeNum( data )), RAIN );
			else if( !strcmp( "RACERELATION", tag ) )
			{
				int nValue;
				if( sscanf( data, "%i %i", &raceDiff, &nValue ) == 2 )	// found 2 values
					RaceRelation( (SI08)nValue, static_cast<RACEID>(raceDiff) );
			}
			else if( !strcmp( "RACIALENEMY", tag ) )
			{
				raceDiff = makeNum( data );
				if( raceDiff > static_cast<SI32>(racialEnemies.size()) )
					Console << "Error in race " << sectNum << ", invalid enemy race " << raceDiff << myendl;
				else
					RaceRelation( MIN_ENEMY, static_cast<RACEID>(raceDiff) );
			}
			else if( !strcmp( "RACIALAID", tag ) )
			{
				raceDiff = makeNum( data );
				if( raceDiff > static_cast<SI32>(racialEnemies.size() ))
					Console << "Error in race " << sectNum << ", invalid ally race " <<  raceDiff << myendl;
				else
					RaceRelation( MIN_ALLY, static_cast<RACEID>(raceDiff ));
			}
			break;

		case 's':
		case 'S':
			if( !strcmp( tag, "STRCAP" ) )
				Skill( static_cast<SKILLVAL>(makeNum( data) ), STRENGTH );
			else if( !strcmp( "SKINMIN", tag ) )
				skinMin = (UI16)makeNum( data );
			else if( !strcmp( "SKINMAX", tag ) )
				skinColours.push_back( ColourPair( skinMin, (UI16)makeNum( data ) ) );
			else if( !strcmp( "SNOWAFFECT", tag ) )	// are we affected by light?
				AffectedBySnow( true );
			else if( !strcmp( "SNOWDAMAGE", tag ) )	// how much damage to take from light
				WeatherDamage( static_cast<SI08>(makeNum( data )), SNOW );
			else if( !strcmp( "SNOWSECS", tag ) )		// how often light affects in secs
				WeatherSeconds( static_cast<SECONDS>(makeNum( data )), SNOW );
			else if( !strcmp( "STAMMOD", tag ) ) // how much additional percent of int are mana
				StamModifier( static_cast<SI16>(makeNum( data) ) );
			break;

		case 'v':
		case 'V':
			if( !strcmp( "VISRANGE", tag ) )					// set visibility range ... defaults to 18
				VisibilityRange( static_cast<RANGE>(makeNum( data )) );
			break;

		}

		for( int iCountA = 0; iCountA < TRUESKILLS; iCountA++ )
		{
			char skillthing[64];
			sprintf( skillthing, "%sG", skillname[iCountA] );
			if( !strcmp( skillthing, tag ) )
				Skill( static_cast<SKILLVAL>(makeNum( data )), iCountA );
			else
			{
				sprintf( skillthing, "%sL", skillname[iCountA] );
				if( !strcmp( skillthing, tag ) )
					Skill( modCount + static_cast<SKILLVAL>(makeNum( data )), iCountA );
			}
		}
	}
}

CRace::~CRace()
{
}

R32 CRace::MagicResistance( void ) const
{
	return magicResistance;
}
R32 CRace::PoisonResistance( void ) const
{
	return poisonResistance;
}

void CRace::MagicResistance( R32 value )
{
	magicResistance = value;
}

void CRace::PoisonResistance( R32 value )
{
	poisonResistance = value;
}

CRace& CRace::operator =( CRace& trgRace )
{
	memcpy( iSkills, trgRace.iSkills, sizeof( SKILLVAL ) * SKILLS );
	raceName = trgRace.raceName;;

	beardColours.resize( trgRace.beardColours.size() );
	for( UI32 bCtr = 0; bCtr < beardColours.size(); bCtr++ )
	{
		beardColours[bCtr].cMax = trgRace.beardColours[bCtr].cMax;
		beardColours[bCtr].cMin = trgRace.beardColours[bCtr].cMin;
	}

	hairColours.resize( trgRace.hairColours.size() );
	for( UI32 hCtr = 0; hCtr < hairColours.size(); hCtr++ )
	{
		hairColours[hCtr].cMax = trgRace.hairColours[hCtr].cMax;
		hairColours[hCtr].cMin = trgRace.hairColours[hCtr].cMin;
	}

	skinColours.resize( trgRace.skinColours.size() );
	for( UI32 sCtr = 0; sCtr < skinColours.size(); sCtr++ )
	{
		skinColours[sCtr].cMax = trgRace.skinColours[sCtr].cMax;
		skinColours[sCtr].cMin = trgRace.skinColours[sCtr].cMin;
	}

	bools = trgRace.bools;
	restrictGender = trgRace.restrictGender;

	racialEnemies.resize( trgRace.racialEnemies.size() );
	for( UI32 rCtr = 0; rCtr < racialEnemies.size(); rCtr++ )
		racialEnemies[rCtr] = trgRace.racialEnemies[rCtr];

	lightLevel = trgRace.lightLevel;
	nightVision = trgRace.nightVision;
	armourRestrict = trgRace.armourRestrict;

	memcpy( weathSecs, trgRace.weathDamage, sizeof( SECONDS ) * WEATHNUM );
	memcpy( weathSecs, trgRace.weathDamage, sizeof( SI08 ) * WEATHNUM );

	languageMin = trgRace.languageMin;
	visDistance = trgRace.visDistance;
	poisonResistance = trgRace.poisonResistance;
	magicResistance = trgRace.magicResistance;

	return (*this);
}

SI32 cRaces::Count( void ) const
{
	return races.size();
}

