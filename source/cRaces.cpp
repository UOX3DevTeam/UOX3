#include "uox3.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "classes.h"
#include "CJSEngine.h"
#include "power.h"
#include <unordered_set>


cRaces *Races = nullptr;

const UI08 MALE = 1;
const UI08 FEMALE = 2;

//const RACEREL MAX_ENEMY = -100;
//const RACEREL MAX_ALLY  = 100;

const UI32 BIT_REQBEARD		= 0;
const UI32 BIT_NOBEARD		= 1;
const UI32 BIT_PLAYERRACE	= 2;
const UI32 BIT_NOHAIR		= 3;

bool cRaces::InvalidRace( RACEID x ) const
{
	return ( x >= races.size() );
}

const std::string cRaces::Name( RACEID race ) const
// PRE:	race is a valid race
// POST:	returns pointer to a string
{
	return races[race]->Name();
}

CRace * cRaces::Race( RACEID x )
{
	if( InvalidRace( x ))
		return nullptr;

	return races[x];
}

cRaces::cRaces():initialized(false)
{
}
cRaces::~cRaces()
// PRE:	cRaces has been initialized
// POST:	Dynamic memory deleted
{
	if( /* DISABLES CODE */ ( false ))
	{
	//if (initialized){
		JSEngine->ReleaseObject( IUE_RACE, this );

		for( size_t i = 0; i < races.size(); ++i )
		{
			delete races[i];
			races[i] = nullptr;
		}
		races.clear();
	}
}

void cRaces::DefaultInitCombat( void )
{
	combat.resize( 4 );
	combat[0].value = 0;
	combat[1].value = 50;
	combat[2].value = 100;
	combat[3].value = 200;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load race details from races.dfn
//o------------------------------------------------------------------------------------------------o
void cRaces::Load()
{
	initialized = true;
	UI32 i = 0;
	UI32 raceCount = 0;
	bool done = false;

	std::string sect;
	std::string tag;
	std::string data;

	while( !done )
	{
		sect = std::string( "RACE " ) + oldstrutil::number( raceCount );
		CScriptSection *tempSect = FileLookup->FindEntry( sect, race_def );
		if( tempSect == nullptr )
		{
			done = true;
		}
		else
		{
			++raceCount;
		}
	}

	for( i = 0; i < raceCount; ++i )
	{
		races.push_back( new CRace( raceCount ));
	}

	CScriptSection *CombatMods = FileLookup->FindEntry( std::string( "COMBAT MODS" ), race_def );
	if( CombatMods != nullptr )
	{
		tag = CombatMods->First();
		if( tag.empty() )	// location didn't exist!!!
		{
			DefaultInitCombat();
		}
		else
		{
			if( oldstrutil::upper( tag ) != "MODCOUNT" )
			{
				Console.Error( "MODCOUNT must come before any entries!" );
				DefaultInitCombat();
			}
			else
			{
				UI32 modifierCount = static_cast<UI32>( std::stoul( CombatMods->GrabData(), nullptr, 0 ));
				if( modifierCount < 4 )
				{
					Console.Warning( "MODCOUNT must be more >= 4, or it uses the defaults!" );
					DefaultInitCombat();
				}
				else
				{
					combat.resize( modifierCount );
					for( i = 0; i < modifierCount; ++i )
					{
						tag = CombatMods->Next();
						data = CombatMods->GrabData();
						if( !data.empty() )
						{	
							combat[i].value = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
						}
						else
						{
							combat[i].value = 100;
						}
					}
				}
			}
		}
	}
	else
	{
		DefaultInitCombat();
	}
	for( size_t er = 0; er < raceCount; ++er )
	{
		races[er]->Load( er, static_cast<SI32>( combat.size() ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Compare()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compare and return the relation between the races of two characters
//|	Notes		-	Returns 0 if no enemy or ally, -1 if enemy or 1 if ally
//o------------------------------------------------------------------------------------------------o
RaceRelate cRaces::Compare( CChar *player1, CChar *player2 ) const
{
	if( !ValidateObject( player1 ) || !ValidateObject( player2 ))
		return RACE_NEUTRAL;

	RACEID r1 = player1->GetRace();
	RACEID r2 = player2->GetRace();
	if( r1 >= races.size() || r2 >= races.size() )
		return RACE_NEUTRAL;

	return races[r1]->RaceRelation( r2 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::CompareByRace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compare and return the relation between two races
//|	Notes		-	Returns 0 if no enemy or ally, -1 if enemy or 1 if ally
//o------------------------------------------------------------------------------------------------o
RaceRelate cRaces::CompareByRace( RACEID race1, RACEID race2 ) const
{
	if( race1 >= races.size() ) // invalid race?
	{
		return RACE_NEUTRAL;
	}
	else if( race2 >= races.size() ) // invalid race?
	{
		return RACE_NEUTRAL;
	}
	else
	{
		return races[race1]->RaceRelation( race2 ); // enemy race
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::ApplyRace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply race (and all the consequences of that) to a character
//|	Notes		-	Primarily used by race gates
//o------------------------------------------------------------------------------------------------o
void cRaces::ApplyRace( CChar *s, RACEID x, bool always )
{
	if( !ValidateObject( s ))
		return;

	CItem *hairobject = nullptr, *beardobject = nullptr;

	CRace *pRace = Race( x );
	if( pRace == nullptr )
		return;

	CSocket *mSock = s->GetSocket();
	if( mSock && !pRace->IsPlayerRace() )
	{
		mSock->SysMessage( 369 ); // This race is not for players!
		return;
	}
	if( s->GetRaceGate() == 65535 || always )
	{
		UI16 hairColor = 0;
		std::map<UI08, std::string> lossMap;

		lossMap[STRENGTH] = "strength";
		lossMap[DEXTERITY] = "speed";
		lossMap[INTELLECT] = "wisdom";

		beardobject = s->GetItemAtLayer( IL_FACIALHAIR );
		hairobject	= s->GetItemAtLayer( IL_HAIR );
		if( pRace->GenderRestriction() != 0 )
		{
			if( pRace->GenderRestriction() != FEMALE && ( s->GetId() == 0x0191 || s->GetId() == 0x025E || s->GetId() == 0x029B || s->GetId() == 0x02EF || s->GetId() == 0x00B8 || s->GetId() == 0x00BA ))
			{
				if( mSock )
				{
					mSock->SysMessage( 370 ); // You are not of the right gender!
				}
				return;
			}
			if( pRace->GenderRestriction() != MALE && ( s->GetId() == 0x0190 || s->GetId() == 0x025D || s->GetId() == 0x029A || s->GetId() == 0x02EE || s->GetId() == 0x00B7 || s->GetId() == 0x00B9 ))
			{
				if( mSock )
				{
					mSock->SysMessage( 370 ); // You are not of the right gender!
				}
				return;
			}
		}
		s->SetRaceGate( x );
		s->SetRace( x );

		SI16 stats[3];
		stats[0] = s->ActualStrength();
		stats[1] = s->ActualDexterity();
		stats[2] = s->ActualIntelligence();

		for( UI08 counter = STRENGTH; counter <= INTELLECT; ++counter )
		{
			if( stats[counter-STRENGTH] > pRace->Skill( counter ))
			{
				if( mSock )
				{
					mSock->SysMessage( 371, lossMap[counter].c_str() );
				}
				stats[counter-STRENGTH] = pRace->Skill( counter );
			}
			else
			{
				stats[counter-STRENGTH] = 0;
			}
		}
		if( stats[0] != 0 )
		{
			s->SetStrength( stats[0] );
		}
		if( stats[1] != 0 )
		{
			s->SetDexterity( stats[1] );
		}
		if( stats[2] != 0 )
		{
			s->SetIntelligence( stats[2] );
		}

		if( ValidateObject( hairobject ))
		{
			if( pRace->IsHairRestricted() )
			{
				hairColor = ( hairobject->GetColour() );
				if( pRace->IsValidHair( hairColor ))
				{
					hairColor = RandomHair( x );
					hairobject->SetColour( hairColor );
				}
			}
			if( pRace->NoHair() )
			{
				hairobject->Delete();
				hairobject = nullptr;
			}
		}
		if( pRace->RequiresBeard() && ( s->GetId() == 0x0190 || s->GetId() == 0x025D ) && !ValidateObject( beardobject ))
		{
			if( pRace->IsBeardRestricted() )
			{
				hairColor = RandomBeard( x );
			}
			else
			{
				hairColor = 0x0480;
			}
			CItem *n = Items->CreateItem( nullptr, s, 0x204C, 1, hairColor, OT_ITEM );
			if( n != nullptr )
			{
				n->SetDecayable( false );
				n->SetLayer( IL_FACIALHAIR );
				if( n->SetCont( s ))
				{
					beardobject = n;
				}
			}
		}
		if( ValidateObject( beardobject ))
		{
			if( pRace->IsBeardRestricted() )
			{
				hairColor = beardobject->GetColour();
				if( pRace->IsValidBeard( hairColor ))
				{
					hairColor = RandomBeard( x );
					beardobject->SetColour( hairColor );
				}
			}
			if( pRace->NoBeard() )
			{
				beardobject->Delete();
				beardobject = nullptr;
			}
		}
		if( pRace->IsSkinRestricted() )	// do we have a limited skin colour range?
		{
			hairColor = s->GetSkin();
			if( pRace->IsValidSkin( hairColor ))	// if not in range
			{
				hairColor = RandomSkin( x );	// get random skin in range
				s->SetSkin( hairColor );
				s->SetOrgSkin( hairColor );
			}
		}
		s->Teleport();
		Effects->PlayStaticAnimation( s, 0x373A, 0, 15 );
		Effects->PlaySound( s, 0x01E9 );
	}
	else
	{
		if( mSock )
		{
			mSock->SysMessage( 372 ); // You have already used a race gate with this character.
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::BeardInRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified colour is a valid beard colour for specified race
//o------------------------------------------------------------------------------------------------o
bool cRaces::BeardInRange( COLOUR color, RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->IsValidBeard( color );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::SkinInRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified colour is a valid skin colour for specified race
//o------------------------------------------------------------------------------------------------o
bool cRaces::SkinInRange( COLOUR color, RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->IsValidSkin( color );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::HairInRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified colour is a valid hair colour for specified race
//o------------------------------------------------------------------------------------------------o
bool cRaces::HairInRange( COLOUR color, RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->IsValidHair( color );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Skill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets skill bonus associated with race
//o------------------------------------------------------------------------------------------------o
SKILLVAL cRaces::Skill( SI32 skill, RACEID race ) const
{
	if( InvalidRace( race ) || skill >= ALLSKILLS )
		return 0;

	return races[race]->Skill( skill );
}
void cRaces::Skill( SI32 skill, SI32 value, RACEID race )
{
	if( InvalidRace( race ))
		return;

	races[race]->Skill( value, skill );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GenderRestrict()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's gender restriction
//|	Purpose		-	0 = none, 1 = male, 2 = female
//o------------------------------------------------------------------------------------------------o
GENDER cRaces::GenderRestrict( RACEID race ) const
{
	if( InvalidRace( race ))
		return MALE;

	return races[race]->GenderRestriction();
}
void cRaces::GenderRestrict( GENDER gender, RACEID race )
{
	if( InvalidRace( race ))
		return;

	races[race]->GenderRestriction( gender );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RequireBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race members must have a beard
//o------------------------------------------------------------------------------------------------o
bool cRaces::RequireBeard( RACEID race ) const
{
	if( InvalidRace( race ))
		return false;

	return races[race]->RequiresBeard();
}
void cRaces::RequireBeard( bool value, RACEID race )
{
	if( InvalidRace( race ))
		return;

	races[race]->RequiresBeard( value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::ArmorRestrict()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets armor class of race
//o------------------------------------------------------------------------------------------------o
void cRaces::ArmorRestrict( RACEID race, ARMORCLASS value )
{
	if( InvalidRace( race ))
		return;

	races[race]->ArmourClassRestriction( value );
}
ARMORCLASS cRaces::ArmorRestrict( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->ArmourClassRestriction();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RandomSkin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random, valid skin colour for specified race
//o------------------------------------------------------------------------------------------------o
COLOUR cRaces::RandomSkin( RACEID x ) const
{
	if( InvalidRace( x ))
		return 0000;

	return races[x]->RandomSkin();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RandomHair()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random, valid hair colour for specified race
//o------------------------------------------------------------------------------------------------o
COLOUR cRaces::RandomHair( RACEID x ) const
{
	if( InvalidRace( x ))
		return 0000;

	return races[x]->RandomHair();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RandomBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random, valid beard colour for specified race
//o------------------------------------------------------------------------------------------------o
COLOUR cRaces::RandomBeard( RACEID x ) const
{
	if( InvalidRace( x ))
		return 0;

	return races[x]->RandomBeard();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::BloodColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets blood colour for members of specified race
//o------------------------------------------------------------------------------------------------o
COLOUR cRaces::BloodColour( RACEID x ) const
{
	if( InvalidRace( x ))
		return 0000;

	return races[x]->BloodColour();
}
void cRaces::BloodColour( RACEID x, COLOUR newValue )
{
	if( InvalidRace( x ))
		return;

	races[x]->BloodColour( newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::BeardRestricted()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if race's beard color is restricted
//o------------------------------------------------------------------------------------------------o
bool cRaces::BeardRestricted( RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->IsBeardRestricted();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::HairRestricted()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if race's hair color is restricted
//o------------------------------------------------------------------------------------------------o
bool cRaces::HairRestricted( RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->IsHairRestricted();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::SkinRestricted()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if race's skin color is restricted
//o------------------------------------------------------------------------------------------------o
bool cRaces::SkinRestricted( RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->IsSkinRestricted();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DamageFromSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns bonus percentage modifier for damage based on skill and race
//o------------------------------------------------------------------------------------------------o
SI32 cRaces::DamageFromSkill( SI32 skill, RACEID x ) const
{
	if( InvalidRace( x ))
		return 0;

	if( skill >= ALLSKILLS )
		return 0;

	SKILLVAL modifier = races[x]->Skill( skill );
	if( modifier >= static_cast<SKILLVAL>( combat.size() ))
	{
		return -(combat[modifier].value);
	}
	else
	{
		return (combat[modifier].value);
	}
	return 0;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::FightPercent()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns positive/negative fight damage bonus for race x with skill skill
//o------------------------------------------------------------------------------------------------o
SI32 cRaces::FightPercent( SI32 skill, RACEID x ) const
{
	if( InvalidRace( x ))
		return 100;

	SKILLVAL modifier = races[x]->Skill( skill );
	SI32 divValue = combat[modifier].value / 10;
	divValue = divValue / 10;
	if( divValue == 0 )
		return 100;

	if( modifier >= static_cast<SI32>( combat.size() ))
	{
		return -static_cast<SI32>( 100 / static_cast<R32>( divValue ));
	}
	else
	{
		return static_cast<SI32>( 100 / static_cast<R32>( divValue ));
	}
	return 100;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialInfo()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set the relation between two races to a specified value
//o------------------------------------------------------------------------------------------------o
void cRaces::RacialInfo( RACEID race, RACEID toSet, RaceRelate value )
{
	if( InvalidRace( race ))
		return;

	races[race]->RaceRelation( value, toSet );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialEnemy()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets up two races as racial enemies
//o------------------------------------------------------------------------------------------------o
void cRaces::RacialEnemy( RACEID race, RACEID enemy )
{
	if( InvalidRace( race ))
		return;

	RacialInfo( race, enemy, RACE_ENEMY );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialAlly()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets up two races as racial allies
//o------------------------------------------------------------------------------------------------o
void cRaces::RacialAlly( RACEID race, RACEID ally )
{
	if( InvalidRace( race ))
		return;

	RacialInfo( race, ally, RACE_ALLY );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialNeutral()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets up two races as neutral to each other
//o------------------------------------------------------------------------------------------------o
void cRaces::RacialNeutral( RACEID race, RACEID neutral )
{
	if( InvalidRace( race ))
		return;

	RacialInfo( race, neutral, RACE_NEUTRAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::LanguageMin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum (spirit speak?) skill required to understand race's language
//o------------------------------------------------------------------------------------------------o
SKILLVAL cRaces::LanguageMin( RACEID x ) const
{
	return races[x]->LanguageMin();
}
void cRaces::LanguageMin( SKILLVAL toSetTo, RACEID race )
{
	if( InvalidRace( race ))
		return;

	races[race]->LanguageMin( toSetTo );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::LightLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the light level the race burns (takes damage) at
//o------------------------------------------------------------------------------------------------o
void cRaces::LightLevel( RACEID race, LIGHTLEVEL value )
{
	if( InvalidRace( race ))
		return;

	races[race]->LightLevel( value );
}
LIGHTLEVEL cRaces::LightLevel( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->LightLevel();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::ColdLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the cold level the race burns (takes damage) at
//o------------------------------------------------------------------------------------------------o
void cRaces::ColdLevel( RACEID race, COLDLEVEL value )
{
	if( InvalidRace( race ))
		return;

	races[race]->ColdLevel( value );
}
COLDLEVEL cRaces::ColdLevel( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->ColdLevel();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::HeatLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the heat level the race burns (takes damage) at
//o------------------------------------------------------------------------------------------------o
void cRaces::HeatLevel( RACEID race, HEATLEVEL value )
{
	if( InvalidRace( race ))
		return;

	races[race]->HeatLevel( value );
}
HEATLEVEL cRaces::HeatLevel( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->HeatLevel();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DoesHunger()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether members of race will grow hungry
//o------------------------------------------------------------------------------------------------o
void cRaces::DoesHunger( RACEID race, bool value )
{
	if( InvalidRace( race ))
		return;

	races[race]->DoesHunger( value );
}
bool cRaces::DoesHunger( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->DoesHunger();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DoesThirst()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether members of race will grow thirsty
//o------------------------------------------------------------------------------------------------o
void cRaces::DoesThirst( RACEID race, bool value )
{
	if( InvalidRace( race ))
		return;

	races[race]->DoesThirst( value );
}
bool cRaces::DoesThirst( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->DoesThirst();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetHungerRate()
//|					cRaces::SetHungerRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set rate at which members of race will grow hungry
//o------------------------------------------------------------------------------------------------o
UI16 cRaces::GetHungerRate( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->GetHungerRate();
}
void cRaces::SetHungerRate( RACEID race, UI16 value )
{
	if( InvalidRace( race ))
		return;

	races[race]->SetHungerRate( value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetThirstRate()
//|					cRaces::SetThirstRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set rate at which members of race will grow thirsty
//o------------------------------------------------------------------------------------------------o
UI16 cRaces::GetThirstRate( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->GetThirstRate();
}
void cRaces::SetThirstRate( RACEID race, UI16 value )
{
	if( InvalidRace( race ))
		return;

	races[race]->SetThirstRate( value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetHungerDamage()
//|					cRaces::SetHungerDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set damage that members of race will take when hungry
//o------------------------------------------------------------------------------------------------o
SI16 cRaces::GetHungerDamage( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->GetHungerDamage();
}
void cRaces::SetHungerDamage( RACEID race, SI16 value )
{
	if( InvalidRace( race ))
		return;

	races[race]->SetHungerDamage( value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetThirstDrain()
//|					cRaces::SetThirstDrain()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set stamina drain that members of race will suffer when thirsty
//o------------------------------------------------------------------------------------------------o
SI16 cRaces::GetThirstDrain( RACEID race ) const
{
	if( InvalidRace( race ))
		return 0;

	return races[race]->GetThirstDrain();
}
void cRaces::SetThirstDrain( RACEID race, SI16 value )
{
	if( InvalidRace( race ))
		return;

	races[race]->SetThirstDrain( value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Affect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether race is affected by a particular element
//o------------------------------------------------------------------------------------------------o
bool cRaces::Affect( RACEID race, WeatherType element ) const
{
	bool rValue = false;
	if( !InvalidRace( race ))
	{
		rValue = races[race]->AffectedBy( element );
	}
	return rValue;
}
void cRaces::Affect( RACEID race, WeatherType element, bool value )
{
	if( !InvalidRace( race ))
	{
		races[race]->AffectedBy( value, element );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Secs()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set number of seconds between burns for race from element
//o------------------------------------------------------------------------------------------------o
SECONDS cRaces::Secs( RACEID race, WeatherType element ) const
{
	if( InvalidRace( race ))
		return 1;

	return races[race]->WeatherSeconds( element );
}

void cRaces::Secs( RACEID race, WeatherType element, SECONDS value )
{
	if( InvalidRace( race ))
		return;

	races[race]->WeatherSeconds( value, element );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Damage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set damage incurred by race from element when they burn
//o------------------------------------------------------------------------------------------------o
SI08 cRaces::Damage( RACEID race, WeatherType element ) const
{
	if( InvalidRace( race ))
		return 1;

	return races[race]->WeatherDamage( element );
}
void cRaces::Damage( RACEID race, WeatherType element, SI08 damage )
{
	if( InvalidRace( race ))
		return;

	races[race]->WeatherDamage( damage, element );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::VisLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set light level bonus for race
//o------------------------------------------------------------------------------------------------o
LIGHTLEVEL cRaces::VisLevel( RACEID x ) const
{
	if( InvalidRace( x ))
		return 0;

	return races[x]->NightVision();
}
void cRaces::VisLevel( RACEID x, LIGHTLEVEL bonus )
{
	if( InvalidRace( x ))
		return;

	races[x]->NightVision( bonus );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::VisRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set distance that race can see objects at
//o------------------------------------------------------------------------------------------------o
RANGE cRaces::VisRange( RACEID x ) const
{
	if( InvalidRace( x ))
		return 0;

	return races[x]->VisibilityRange();
}
void cRaces::VisRange( RACEID x, RANGE range )
{
	if( InvalidRace( x ))
		return;

	races[x]->VisibilityRange( range );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::NoBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether race requires members to not have beards
//o------------------------------------------------------------------------------------------------o
bool cRaces::NoBeard( RACEID x ) const
{
	if( InvalidRace( x ))
		return false;

	return races[x]->NoBeard();
}
void cRaces::NoBeard( bool value, RACEID race )
{
	if( InvalidRace( race ))
		return;

	races[race]->NoBeard( value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DebugPrint()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump info about a particular race to Console
//o------------------------------------------------------------------------------------------------o
void cRaces::DebugPrint( RACEID x )
{
	if( InvalidRace( x ))
		return;

	Console << "Race ID: " << x << myendl;
	Console << "Race: " << races[x]->Name() << myendl;
	if( races[x]->RequiresBeard() )
	{
		Console << "Req Beard: Yes" << myendl;
	}
	else
	{
		Console << "Req Beard: No" << myendl;
	}
	if( races[x]->NoBeard() )
	{
		Console << "No Beard: Yes" << myendl;
	}
	else
	{
		Console << "No Beard: No" << myendl;
	}
	if( races[x]->IsPlayerRace() )
	{
		Console << "Player Race: Yes" << myendl;
	}
	else
	{
		Console << "Player Race: No" << myendl;
	}
	Console << "Restrict Gender: " << races[x]->GenderRestriction() << myendl;
	Console << "LightLevel: " << races[x]->LightLevel() << myendl;
	Console << "NightVistion: " << races[x]->NightVision() << myendl;
	Console << "ArmorRestrict: " << races[x]->ArmourClassRestriction() << myendl;
	Console << "LangMin: " << races[x]->LanguageMin() << myendl;
	Console << "Vis Distance: " << races[x]->VisibilityRange() << myendl << myendl;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DebugPrintAll()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump info about all races to Console
//o------------------------------------------------------------------------------------------------o
void cRaces::DebugPrintAll( void )
{
	for( RACEID x = 0; x < races.size(); ++x )
	{
		DebugPrint( x );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::IsPlayerRace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether race is a player race or not
//o------------------------------------------------------------------------------------------------o
bool cRaces::IsPlayerRace( RACEID race ) const
{
	if( InvalidRace( race ))
		return false;

	return races[race]->IsPlayerRace();
}
void cRaces::IsPlayerRace( RACEID x, bool value )
{
	if( InvalidRace( x ))
		return;

	races[x]->IsPlayerRace( value );
}

//o------------------------------------------------------------------------------------------------o
//|	cRace
//o------------------------------------------------------------------------------------------------o


//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::Skill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets skill bonus associated with race (?)
//o------------------------------------------------------------------------------------------------o
SKILLVAL CRace::Skill( SI32 skillNum ) const
{
	return iSkills[skillNum];
}
void CRace::Skill( SKILLVAL newValue, SI32 iNum )
{
	iSkills[iNum] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::Name()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets name of race
//o------------------------------------------------------------------------------------------------o
const std::string CRace::Name( void ) const
{
	return raceName;
}
void CRace::Name( const std::string& newName )
{
	raceName = newName;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::RequiresBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race requires a beard of its members
//o------------------------------------------------------------------------------------------------o
bool CRace::RequiresBeard( void ) const
{
	return bools.test( BIT_REQBEARD );
}
void CRace::RequiresBeard( bool newValue )
{
	bools.set( BIT_REQBEARD, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::NoBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race requires its members to NOT have beards
//o------------------------------------------------------------------------------------------------o
bool CRace::NoBeard( void ) const
{
	return bools.test( BIT_NOBEARD );
}
void CRace::NoBeard( bool newValue )
{
	bools.set( BIT_NOBEARD, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::IsPlayerRace()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race is counted as a player race
//o------------------------------------------------------------------------------------------------o
bool CRace::IsPlayerRace( void ) const
{
	return bools.test( BIT_PLAYERRACE );
}
void CRace::IsPlayerRace( bool newValue )
{
	bools.set( BIT_PLAYERRACE, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::NoHair()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether requires its members to NOT have hair
//o------------------------------------------------------------------------------------------------o
bool CRace::NoHair( void ) const
{
	return bools.test( BIT_NOHAIR );
}
void CRace::NoHair( bool newValue )
{
	bools.set( BIT_NOHAIR, newValue );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GenderRestriction()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race is gender-restricted
//o------------------------------------------------------------------------------------------------o
GENDER CRace::GenderRestriction( void ) const
{
	return restrictGender;
}
void CRace::GenderRestriction( GENDER newValue )
{
	restrictGender = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::LightLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light level at which members of race will burn
//o------------------------------------------------------------------------------------------------o
LIGHTLEVEL CRace::LightLevel( void ) const
{
	return lightLevel;
}
void CRace::LightLevel( LIGHTLEVEL newValue )
{
	lightLevel = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::ColdLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the cold level at which members of race will burn
//o------------------------------------------------------------------------------------------------o
COLDLEVEL CRace::ColdLevel( void ) const
{
	return coldLevel;
}
void CRace::ColdLevel( COLDLEVEL newValue )
{
	coldLevel = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::HeatLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the heat level at which members of race will burn
//o------------------------------------------------------------------------------------------------o
HEATLEVEL CRace::HeatLevel( void ) const
{
	return heatLevel;
}
void CRace::HeatLevel( HEATLEVEL newValue )
{
	heatLevel = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::NightVision()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets bonus the race gets to lightlevels
//o------------------------------------------------------------------------------------------------o
LIGHTLEVEL CRace::NightVision( void ) const
{
	return nightVision;
}
void CRace::NightVision( LIGHTLEVEL newValue )
{
	nightVision = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::ArmourClassRestriction()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which armour classes race is restricted from wearing
//o------------------------------------------------------------------------------------------------o
ARMORCLASS CRace::ArmourClassRestriction( void ) const
{
	return armourRestrict;
}
void CRace::ArmourClassRestriction( ARMORCLASS newValue )
{
	armourRestrict = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::WeatherSeconds()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets interval at which members of a race burn from a given weather type
//o------------------------------------------------------------------------------------------------o
SECONDS CRace::WeatherSeconds( WeatherType iNum ) const
{
	return weathSecs[iNum];
}
void CRace::WeatherSeconds( SECONDS newValue, WeatherType iNum )
{
	weathSecs[iNum] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::WeatherDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how much damage members of race take from a given weather type when they burn
//o------------------------------------------------------------------------------------------------o
SI08 CRace::WeatherDamage( WeatherType iNum ) const
{
	return weathDamage[iNum];
}
void CRace::WeatherDamage( SI08 newValue, WeatherType iNum )
{
	weathDamage[iNum] = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::LanguageMin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum language skill (spirit speak?) required to understand race's language
//o------------------------------------------------------------------------------------------------o
SKILLVAL CRace::LanguageMin( void ) const
{
	return languageMin;
}
void CRace::LanguageMin( SKILLVAL newValue )
{
	languageMin = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::VisibilityRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ???
//o------------------------------------------------------------------------------------------------o
RANGE CRace::VisibilityRange( void ) const
{
	return visDistance;
}
void CRace::VisibilityRange( RANGE newValue )
{
	visDistance = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::AffectedBy()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether members of race are affected by a given weather type
//o------------------------------------------------------------------------------------------------o
bool CRace::AffectedBy( WeatherType iNum ) const
{
	return weatherAffected.test( iNum );
}
void CRace::AffectedBy( bool value, WeatherType iNum )
{
	weatherAffected.set( iNum, value );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetHungerRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets rate at which members of race grow more hungry
//o------------------------------------------------------------------------------------------------o
UI16 CRace::GetHungerRate( void ) const
{
	return hungerRate;
}
void CRace::SetHungerRate( UI16 newValue )
{
	hungerRate = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetThirstRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets rate at which members of race grow more thirsty
//o------------------------------------------------------------------------------------------------o
UI16 CRace::GetThirstRate( void ) const
{
	return thirstRate;
}
void CRace::SetThirstRate( UI16 newValue )
{
	thirstRate = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetHungerDamage()
//|					cRace::SetHungerDamage
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of damage members of race take when hungry
//o------------------------------------------------------------------------------------------------o
SI16 CRace::GetHungerDamage( void ) const
{
	return hungerDamage;
}
void CRace::SetHungerDamage( SI16 newValue )
{
	hungerDamage = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetThirstDrain()
//|					cRace::SetThirstDrain
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of stamina drain members of race suffer when thirsty
//o------------------------------------------------------------------------------------------------o
SI16 CRace::GetThirstDrain( void ) const
{
	return thirstDrain;
}
void CRace::SetThirstDrain( SI16 newValue )
{
	thirstDrain = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::DoesHunger()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether members of race become hungry
//o------------------------------------------------------------------------------------------------o
bool CRace::DoesHunger( void ) const
{
	return doesHunger;
}
void CRace::DoesHunger( bool newValue )
{
	doesHunger = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::DoesThirst()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether members of race become thirsty
//o------------------------------------------------------------------------------------------------o
bool CRace::DoesThirst( void ) const
{
	return doesThirst;
}
void CRace::DoesThirst( bool newValue )
{
	doesThirst = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::CanEquipItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether a given race can equip a given item, based on lists of allowed
//|					or banned equipment from dfndata/race/races.dfn
//o------------------------------------------------------------------------------------------------o
bool CRace::CanEquipItem( UI16 itemId ) const
{
	if( allowedEquipment.size() > 0 )
	{
		// Race has a list of allowed equipment. If it's not in the list, disallow usage
		//return ( std::find( allowedEquipment.begin(), allowedEquipment.end(), itemId ) != allowedEquipment.end() );
		return ( allowedEquipment.find( itemId ) != allowedEquipment.end() );
	}
	
	if( bannedEquipment.size() > 0 )
	{
		// Race has a list of banned equipment. If item is in the list, disallow usage
		//return !( std::find( bannedEquipment.begin(), bannedEquipment.end(), itemId ) != bannedEquipment.end() );
		return !( bannedEquipment.find( itemId ) != bannedEquipment.end() );
	}
	return true;
}

CRace::CRace() : bools( 4 ), visDistance( 0 ), nightVision( 0 ), armourRestrict( 0 ), lightLevel( 1 ),
restrictGender( 0 ), languageMin( 0 ), poisonResistance( 0.0f ), magicResistance( 0.0f ), bloodColour( 0 )
{
	iSkills.fill( 0 );
	weathDamage.fill( 0 );
	weathSecs.fill( 60 );
	//memset( &iSkills[0], 0, sizeof( SKILLVAL ) * SKILLS );
	//memset( &weathDamage[0], 0, sizeof( SI08 ) * WEATHNUM );
	//memset( &weathSecs[0], 60, sizeof( SECONDS ) * WEATHNUM );

	Skill( 100, STRENGTH );
	Skill( 100, DEXTERITY );
	Skill( 100, INTELLECT );
	HPModifier( 0 );
	ManaModifier( 0 );
	StamModifier( 0 );
	DoesHunger( false );
	SetHungerRate( 0 );
	SetHungerDamage( 0 );
	DoesThirst( false );
	SetThirstRate( 0 );
	SetThirstDrain( 0 );
}


CRace::CRace( SI32 numRaces ) : bools( 4 ), visDistance( 0 ), nightVision( 0 ), armourRestrict( 0 ), lightLevel( 1 ),
restrictGender( 0 ), languageMin( 0 ), poisonResistance( 0.0f ), magicResistance( 0.0f ), bloodColour( 0 )
{
	NumEnemyRaces( numRaces );

	iSkills.fill( 0 );
	weathDamage.fill( 0 );
	weathSecs.fill( 60 );

	Skill( 100, STRENGTH );
	Skill( 100, DEXTERITY );
	Skill( 100, INTELLECT );
	HPModifier( 0 );
	ManaModifier( 0 );
	StamModifier( 0 );
	DoesHunger( false );
	SetHungerRate( 0 );
	SetHungerDamage( 0 );
	DoesThirst( false );
	SetThirstRate( 0 );
	SetThirstDrain( 0 );
	weatherAffected.reset();
}
void CRace::NumEnemyRaces( SI32 iNum )
{
	racialEnemies.resize( iNum );
}
RaceRelate CRace::RaceRelation( RACEID race ) const
{
	return racialEnemies[race];
}

COLOUR CRace::RandomSkin( void ) const
{
	if( !IsSkinRestricted() )
		return 0;

	size_t sNum = RandomNum( static_cast<size_t>( 0 ), skinColours.size() - 1 );
	return static_cast<COLOUR>( RandomNum( skinColours[sNum].cMin, skinColours[sNum].cMax ));
}
COLOUR CRace::RandomHair( void ) const
{
	if( !IsHairRestricted() )
		return 0;

	size_t sNum = RandomNum( static_cast<size_t>( 0 ), hairColours.size() - 1 );
	return static_cast<COLOUR>( RandomNum( hairColours[sNum].cMin, hairColours[sNum].cMax ));
}
COLOUR CRace::RandomBeard( void ) const
{
	if( !IsBeardRestricted() )
		return 0;

	size_t sNum = RandomNum( static_cast<size_t>( 0 ), beardColours.size() - 1 );
	return static_cast<COLOUR>( RandomNum( beardColours[sNum].cMin, beardColours[sNum].cMax ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::BloodColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the blood colour of members of race
//o------------------------------------------------------------------------------------------------o
COLOUR CRace::BloodColour( void ) const
{
	return bloodColour;
}
void CRace::BloodColour( COLOUR newValue )
{
	bloodColour = newValue;
}

bool CRace::IsSkinRestricted( void ) const
{
	return ( !skinColours.empty() );
}
bool CRace::IsHairRestricted( void ) const
{
	return ( !hairColours.empty() );
}
bool CRace::IsBeardRestricted( void ) const
{
	return ( !beardColours.empty() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::IsValidSkin()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specific colour is valid for skins for this race
//o------------------------------------------------------------------------------------------------o
bool CRace::IsValidSkin( COLOUR val ) const
{
	if( !IsSkinRestricted() )
		return true;

	for( size_t i = 0; i < skinColours.size(); ++i )
	{
		if( val >= skinColours[i].cMin && val <= skinColours[i].cMax )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::IsValidHair()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specific colour is valid for hairs for this race
//o------------------------------------------------------------------------------------------------o
bool CRace::IsValidHair( COLOUR val ) const
{
	if( !IsHairRestricted() )
		return true;

	for( size_t i = 0; i < hairColours.size(); ++i )
	{
		if( val >= hairColours[i].cMin && val <= hairColours[i].cMax )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::IsValidBeard()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specific colour is valid for beards for this race
//o------------------------------------------------------------------------------------------------o
bool CRace::IsValidBeard( COLOUR val ) const
{
	if( !IsBeardRestricted() )
		return true;

	for( size_t i = 0; i < beardColours.size(); ++i )
	{
		if( val >= beardColours[i].cMin && val <= beardColours[i].cMax )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::RaceRelation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets race's ractial relations with another race
//o------------------------------------------------------------------------------------------------o
void CRace::RaceRelation( RaceRelate value, RACEID race )
{
	racialEnemies[race] = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::HPModifier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's hp modifier
//o------------------------------------------------------------------------------------------------o
SI16 CRace::HPModifier( void ) const
{
	return HPMod;
}
void CRace::HPModifier( SI16 value )
{
	if( value > -100 )
	{
		HPMod = value;
	}
	else
	{
		HPMod = -99;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::ManaModifier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's mana modifier
//o------------------------------------------------------------------------------------------------o
SI16 CRace::ManaModifier( void ) const
{
	return ManaMod;
}
void CRace::ManaModifier( SI16 value )
{
	if( value > -100 )
	{
		ManaMod = value;
	}
	else
	{
		ManaMod = -99;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::StamModifier()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's stam modifier
//o------------------------------------------------------------------------------------------------o
SI16 CRace::StamModifier( void ) const
{
	return StamMod;
}
void CRace::StamModifier( SI16 value )
{
	if( value > -100 )
	{
		StamMod = value;
	}
	else
	{
		StamMod = -99;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load details of race from races.dfn
//o------------------------------------------------------------------------------------------------o
void CRace::Load( size_t sectNum, SI32 modCount )
{
	std::string tag;
	std::string data;
	std::string UTag;
	SI32 raceDiff = 0;
	std::string sect = std::string( "RACE " ) + oldstrutil::number( sectNum );
	CScriptSection *RacialPart = FileLookup->FindEntry( sect, race_def );
	if( RacialPart == nullptr )
		return;

	COLOUR beardMin = 0, skinMin = 0, hairMin = 0;

	for( tag = RacialPart->First(); !RacialPart->AtEnd(); tag = RacialPart->Next() )
	{
		UTag = oldstrutil::upper( tag );
		data = RacialPart->GrabData();
		
		switch( tag[0] )
		{
			case 'a':
			case 'A':
			{
				if( UTag == "ALLOWEQUIPLIST" )
				{
					// Allowed equipment from [EQUIPMENT #] sections in races.dfn
					std::string subTag;
					std::string subUTag;
					std::string subData;
					std::string subSect = std::string( "EQUIPLIST " ) + oldstrutil::number( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
					CScriptSection *RacialEquipment = FileLookup->FindEntry( subSect, race_def );
					if( RacialEquipment == nullptr )
						break;

					for( subTag = RacialEquipment->First(); !RacialEquipment->AtEnd(); subTag = RacialEquipment->Next() )
					{
						subUTag = oldstrutil::upper( subTag );
						subData = RacialEquipment->GrabData();
						switch( subTag[0] )
						{
							case 'i':
							case 'I':
								if( subUTag == "ITEMS" )
								{
									auto csecs = oldstrutil::sections( subData, "," );
									for( size_t i = 0; i < csecs.size() - 1; i++ )
									{
										UI16 temp = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[i], "//" )), nullptr, 0 ));
										//allowedEquipment.push_back( temp );
										allowedEquipment.insert( temp );
									}
								}
								break;
							default:
								break;
						}
					}
				}
				else if( UTag == "ARMORREST" ) // 8 classes, value 0 is all, else it's a bit comparison
				{
					ArmourClassRestriction( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
				}
				break;
			}
			case 'b':
			case 'B':
				if( UTag == "BANEQUIPLIST" )
				{
					// Banned equipment from [EQUIPMENT #] sections in races.dfn
					std::string subTag;
					std::string subUTag;
					std::string subData;
					std::string subSect = std::string( "EQUIPLIST " ) + oldstrutil::number( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
					CScriptSection *RacialEquipment = FileLookup->FindEntry( subSect, race_def );
					if( RacialEquipment == nullptr )
						break;

					for( subTag = RacialEquipment->First(); !RacialEquipment->AtEnd(); subTag = RacialEquipment->Next() )
					{
						subUTag = oldstrutil::upper( subTag );
						subData = RacialEquipment->GrabData();
						switch( subTag[0] )
						{
							case 'i':
							case 'I':
								if( subUTag == "ITEMS" )
								{
									auto csecs = oldstrutil::sections( subData, "," );
									for( size_t i = 0; i < csecs.size() - 1; i++ )
									{
										UI16 temp = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[i], "//" )), nullptr, 0 ));
										//bannedEquipment.push_back( temp );
										bannedEquipment.insert( temp );
									}
								}
								break;
							default:
								break;
						}
					}
				}
				else if( UTag == "BEARDMIN" )
				{
					beardMin = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
				}
				else if( UTag == "BEARDMAX" )
				{
					beardColours.push_back( ColourPair_st( beardMin, static_cast<UI16>( std::stoul( data, nullptr, 0 ))));
				}
				else if( UTag == "BLOODCOLOUR" )
				{
					bloodColour = static_cast<COLOUR>( std::stoul( data, nullptr, 0 ));
				}
				break;

			case 'c':
			case 'C':
				if( UTag == "COLDAFFECT" ) // are we affected by cold?
				{
					AffectedBy( true, COLD );
				}
				else if( UTag == "COLDLEVEL" ) // cold level at which to take damage
				{
					ColdLevel( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
				}
				else if( UTag == "COLDDAMAGE" ) // how much damage to take from cold
				{
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), COLD );
				}
				else if( UTag == "COLDSECS" ) // how often cold affects in secs
				{
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), COLD );
				}
				break;

			case 'd':
			case 'D':
				if( UTag == "DEXCAP" )
				{
					Skill( static_cast<UI16>( std::stoul( data, nullptr, 0 )), DEXTERITY );
				}
				break;

			case 'g':
			case 'G':
				if( UTag == "GENDER" )
				{
					auto udata = oldstrutil::upper( data );
					if( udata == "MALE" )
					{
						GenderRestriction( MALE );
					}
					else if( udata == "FEMALE" )
					{
						GenderRestriction( FEMALE );
					}
					else
					{
						GenderRestriction( MALE );
					}
				}
				break;

			case 'h':
			case 'H':
				if( UTag == "HAIRMIN" )
				{
					hairMin = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
				}
				else if( UTag == "HAIRMAX" )
				{
					hairColours.push_back( ColourPair_st( hairMin, static_cast<UI16>( std::stoul( data, nullptr, 0 ))));
				}
				else if( UTag == "HEATAFFECT" )
				{
					// are we affected by light?
					AffectedBy( true, HEAT );
				}
				else if( UTag == "HEATDAMAGE" )
				{
					// how much damage to take from light
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), HEAT );
				}
				else if( UTag == "HEATLEVEL" )
				{
					// heat level at which to take damage
					HeatLevel( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
				}
				else if( UTag == "HEATSECS" )
				{		// how often light affects in secs
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), HEAT );
				}
				else if( UTag == "HPMOD" )
				{
					// how high percentage of strength is added as bonus hitpoints
					HPModifier( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
				}
				else if( UTag == "HUNGER" )	
				{
					// does race suffer from hunger
					auto csecs = oldstrutil::sections( data, "," );
					if( csecs.size() > 1 )
					{
						SetHungerRate( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetHungerDamage( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetHungerRate( 0 );
						SetHungerDamage( 0 );
					}
					if( GetHungerRate() > 0 )
					{
						DoesHunger( true );
					}
					else
					{
						DoesHunger( false );
					}
				}
				break;

			case 'i':
			case 'I':
				if( UTag == "INTCAP" )
				{
					Skill( static_cast<UI16>( std::stoul( data, nullptr, 0 )), INTELLECT );
				}
				break;

			case 'l':
			case 'L':
				if( UTag == "LIGHTAFFECT" ) // are we affected by light?
				{
					AffectedBy( true, LIGHT );
				}
				else if( UTag == "LIGHTDAMAGE" ) // how much damage to take from light
				{
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), LIGHT );
				}
				else if( UTag == "LIGHTLEVEL" ) // light level at which to take damage
				{
					LightLevel( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
				}
				else if( UTag == "LIGHTSECS" ) // how often light affects in secs
				{
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), LIGHT );
				}

				else if( UTag == "LIGHTNINGAFFECT" ) // are we affected by light?
				{
					AffectedBy( true, LIGHTNING );
				}
				else if( UTag == "LIGHTNINGDAMAGE" ) // how much damage to take from light
				{
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), LIGHTNING );
				}
				else if( UTag == "LIGHTNINGCHANCE" ) // how big is the chance to get hit by a lightning
				{
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), LIGHTNING );
				}
				else if( UTag == "LANGUAGEMIN" ) // set language min
				{
					LanguageMin( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
				}
				break;

			case 'm':
			case 'M':
				if( UTag == "MAGICRESISTANCE" )	// magic resistance?
				{
					MagicResistance( std::stof( data ));
				}
				else if( UTag == "MANAMOD" ) // how high percentage of int to add as bonus mana
				{
					ManaModifier( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
				}
				break;

			case 'n':
			case 'N':
				if( UTag == "NAME" )
				{
					Name( data );
				}
				else if( UTag == "NOBEARD" )
				{
					NoBeard( true );
				}
				else if( UTag == "NIGHTVIS" ) // night vision level... light bonus
				{
					NightVision( static_cast<UI08>( std::stoul( data, nullptr, 0 )));
				}
				break;

			case 'p':
			case 'P':
				if( UTag == "PLAYERRACE" )// is it a player race?
				{
					IsPlayerRace(( static_cast<UI16>( std::stoul( data, nullptr, 0 )) != 0 ));
				}
				else if( UTag == "POISONRESISTANCE" ) // poison resistance?
				{
					PoisonResistance( std::stof( data ));
				}
				else if( UTag == "PARENTRACE" )
				{
					CRace *pRace = Races->Race( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
					if( pRace != nullptr )
					{
						( *this ) = ( *pRace );
					}
				}
				break;

			case 'r':
			case 'R':
				if( UTag == "REQUIREBEARD" )
				{
					RequiresBeard( true );
				}
				else if( UTag == "RAINAFFECT" ) // are we affected by light?
				{
					AffectedBy( true, RAIN );
				}
				else if( UTag == "RAINDAMAGE" )	// how much damage to take from light
				{
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), RAIN );
				}
				else if( UTag == "RAINSECS" ) // how often light affects in secs
				{
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), RAIN );
				}
				else if( UTag == "RACERELATION" )
				{
					auto ssecs = oldstrutil::sections(data," ");
					if( ssecs.size() > 1 )
					{
						RaceRelation( static_cast<RaceRelate>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )), static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
					}
				}
				else if( UTag == "RACIALENEMY" )
				{
					raceDiff = std::stoi( data, nullptr, 0 );
					if( raceDiff > static_cast<SI32>( racialEnemies.size() ))
					{
						Console << "Error in race " << static_cast<UI32>( sectNum ) << ", invalid enemy race " << raceDiff << myendl;
					}
					else
					{
						RaceRelation( RACE_ENEMY, static_cast<RACEID>( raceDiff ));
					}
				}
				else if( UTag == "RACIALAID" )
				{
					raceDiff = std::stoi( data, nullptr, 0 );
					if( raceDiff > static_cast<SI32>( racialEnemies.size() ))
					{
						Console << "Error in race " << static_cast<UI32>( sectNum ) << ", invalid ally race " <<  raceDiff << myendl;
					}
					else
					{
						RaceRelation( RACE_ALLY, static_cast<RACEID>( raceDiff ));
					}
				}
				break;

			case 's':
			case 'S':
				if( UTag == "STRCAP" )
				{
					Skill( static_cast<UI16>( std::stoul( data, nullptr, 0 )), STRENGTH );
				}
				else if( UTag == "SKINMIN" )
				{
					skinMin = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
				}
				else if( UTag == "SKINMAX" )
				{
					skinColours.push_back( ColourPair_st( skinMin, static_cast<UI16>( std::stoul( data, nullptr, 0 ))));
				}
				else if( UTag == "SNOWAFFECT" ) // are we affected by light?
				{
					AffectedBy( true, SNOW );
				}
				else if( UTag == "SNOWDAMAGE" ) // how much damage to take from light
				{
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), SNOW );
				}
				else if( UTag == "SNOWSECS" ) // how often light affects in secs
				{
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), SNOW );
				}
				else if( UTag == "STORMAFFECT" ) // are we affected by storm?
				{
					AffectedBy( true, STORM );
				}
				else if( UTag == "STORMDAMAGE" ) // how much damage to take from storm
				{
					WeatherDamage( static_cast<UI16>( std::stoul( data, nullptr, 0 )), STORM );
				}
				else if( UTag == "STORMSECS" ) // how often storm affects in secs
				{
					WeatherSeconds( static_cast<UI16>( std::stoul( data, nullptr, 0 )), STORM );
				}
				else if( UTag == "STAMMOD" ) // how high percentage of dex is added as bonus stamina
				{
					StamModifier( static_cast<SI16>( std::stoi( data, nullptr, 0 )));
				}
				break;
			case 't':
			case 'T':
				if( UTag == "THIRST" )
				{
					// does race suffer from thirst
					auto csecs = oldstrutil::sections( data, "," );
					if( csecs.size() > 1 )
					{
						SetThirstRate( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetThirstDrain( static_cast<SI16>( std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetThirstRate( 0 );
						SetThirstDrain( 0 );
					}
					if( GetThirstRate() > 0 )
					{
						DoesThirst( true );
					}
					else
					{
						DoesThirst( false );
					}
				}
				break;

			case 'v':
			case 'V':
				if( UTag == "VISRANGE" ) // set visibility range ... defaults to 18
				{
					VisibilityRange( static_cast<char>( std::stoi( data, nullptr, 0 )));
				}
				break;
		}

		for( SI32 iCountA = 0; iCountA < ALLSKILLS; ++iCountA )
		{
			std::string skillthing = cwmWorldState->skill[iCountA].name;
			skillthing += "G";
			if( skillthing == tag )
			{
				Skill( static_cast<UI16>( std::stoul( data, nullptr, 0 )), iCountA );
			}
			else
			{
				skillthing = cwmWorldState->skill[iCountA].name;
				skillthing += "L";
				if( skillthing == tag )
				{
					Skill( modCount + static_cast<UI16>( std::stoul( data, nullptr, 0 )), iCountA );
				}
			}
		}
	}
}

CRace::~CRace()
{
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::MagicResistance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's magic resistance
//o------------------------------------------------------------------------------------------------o
R32 CRace::MagicResistance( void ) const
{
	return magicResistance;
}
void CRace::MagicResistance( R32 value )
{
	magicResistance = value;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::PoisonResistance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's posion resistance
//o------------------------------------------------------------------------------------------------o
R32 CRace::PoisonResistance( void ) const
{
	return poisonResistance;
}
void CRace::PoisonResistance( R32 value )
{
	poisonResistance = value;
}

CRace& CRace::operator = ( CRace& trgRace )
{
	iSkills = trgRace.iSkills;
	//memcpy( iSkills, trgRace.iSkills, sizeof( SKILLVAL ) * SKILLS );
	raceName = trgRace.raceName;

	beardColours.resize( trgRace.beardColours.size() );
	for( size_t bCtr = 0; bCtr < beardColours.size(); ++bCtr )
	{
		beardColours[bCtr].cMax = trgRace.beardColours[bCtr].cMax;
		beardColours[bCtr].cMin = trgRace.beardColours[bCtr].cMin;
	}

	hairColours.resize( trgRace.hairColours.size() );
	for( size_t hCtr = 0; hCtr < hairColours.size(); ++hCtr )
	{
		hairColours[hCtr].cMax = trgRace.hairColours[hCtr].cMax;
		hairColours[hCtr].cMin = trgRace.hairColours[hCtr].cMin;
	}

	skinColours.resize( trgRace.skinColours.size() );
	for( size_t sCtr = 0; sCtr < skinColours.size(); ++sCtr )
	{
		skinColours[sCtr].cMax = trgRace.skinColours[sCtr].cMax;
		skinColours[sCtr].cMin = trgRace.skinColours[sCtr].cMin;
	}

	bools = trgRace.bools;
	restrictGender = trgRace.restrictGender;

	racialEnemies.resize( trgRace.racialEnemies.size() );
	for( size_t rCtr = 0; rCtr < racialEnemies.size(); ++rCtr )
	{
		racialEnemies[rCtr] = trgRace.racialEnemies[rCtr];
	}

	lightLevel = trgRace.lightLevel;
	nightVision = trgRace.nightVision;
	armourRestrict = trgRace.armourRestrict;
	weathSecs = trgRace.weathSecs;
	weathDamage = trgRace.weathDamage;
	//memcpy( weathSecs, trgRace.weatSec, sizeof( SECONDS ) * WEATHNUM );
	//memcpy( weathSecs, trgRace.weathDamage, sizeof( SI08 ) * WEATHNUM );

	languageMin = trgRace.languageMin;
	visDistance = trgRace.visDistance;
	poisonResistance = trgRace.poisonResistance;
	magicResistance = trgRace.magicResistance;
	bloodColour = trgRace.bloodColour;

	return ( *this );
}

size_t cRaces::Count( void ) const
{
	return races.size();
}
