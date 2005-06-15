// Abaddon:  I have a VERY set idea of features and plans for races
// Please DO NOT TOUCH THEM, because I will be working on them quite
// solidly, along with EviLDeD, over the next few months.  

#include "uox3.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "classes.h"

namespace UOX
{

cRaces *Races = NULL;

const UI08 MALE = 1;
const UI08 FEMALE = 2;

const RACEREL MIN_ENEMY = -1;
const RACEREL MIN_ALLY  = -1;
const RACEREL NEUTRAL   = 0;
const RACEREL MAX_ENEMY = -100;
const RACEREL MAX_ALLY  = 100;

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
	for( size_t i = 0; i < races.size(); ++i )
	{
		delete races[i];
		races[i] = NULL;
	}
	races.clear();
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

	UString sect;
	UString tag;
	UString data;

	while( !done )
	{
		sect					= "RACE " + UString::number( raceCount );
		ScriptSection *tempSect = FileLookup->FindEntry( sect, race_def );
		if( tempSect == NULL )
			done = true;
		else
			++raceCount;
	}

	for( i = 0; i < raceCount; ++i )
		races.push_back( new CRace( raceCount ) );

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
			if( tag.upper() != "MODCOUNT" )
			{
				Console << "MODCOUNT must come before any entries!" << myendl;
				DefaultInitCombat();
			}
			else
			{
				UI32 modifierCount = CombatMods->GrabData().toULong();
				if( modifierCount < 4 )
				{
					Console << "MODCOUNT must be more >= 4, or it uses the defaults!" << myendl;
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
							combat[i].value = data.toUByte();
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
	for( size_t er = 0; er < raceCount; ++er )
		races[er]->Load( er, combat.size() );
}

SI08 cRaces::Compare( CChar *player1, CChar *player2 ) const
{
	if( !ValidateObject( player1 ) || !ValidateObject( player2 ) )
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
	if( !ValidateObject( s ) )
		return;
	CItem *hairobject = NULL, *beardobject = NULL;
	UI16 hairColor = 0;

	CRace *pRace = Race( x );
	if( pRace == NULL )
		return;

	CSocket *mSock = calcSocketObjFromChar( s );
	if( !pRace->IsPlayerRace() )
	{
		mSock->sysmessage( 369 );
		return;
	}
	if( s->GetRaceGate() == 65535 || always )
	{
		std::map< UI08, std::string > lossMap;

		lossMap[STRENGTH] = "strength";
		lossMap[DEXTERITY] = "speed";
		lossMap[INTELLECT] = "wisdom";
	
		beardobject = s->GetItemAtLayer( IL_FACIALHAIR );
		hairobject	= s->GetItemAtLayer( IL_HAIR );
		if( pRace->GenderRestriction() != 0 )
		{
			if( pRace->GenderRestriction() != FEMALE && s->GetID() == 0x0191 )
			{
				mSock->sysmessage( 370 );
				return;
			}
			if( pRace->GenderRestriction() != MALE && s->GetID() == 0x0190 )
			{
				mSock->sysmessage( 370 );
				return;
			}
		}
		Effects->PlayStaticAnimation( s, 0x373A, 0, 15 );
		Effects->PlaySound( s, 0x01E9 );
		s->SetRaceGate( x );
		s->SetRace( x );

		SI16 stats[3];
		stats[0] = s->ActualStrength();
		stats[1] = s->ActualDexterity();
		stats[2] = s->ActualIntelligence();

		for( UI08 counter = STRENGTH; counter <= INTELLECT; ++counter )
		{
			if( stats[counter-STRENGTH] > pRace->Skill( counter ) )
			{
				mSock->sysmessage( 371, lossMap[counter].c_str() );
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
			mSock->statwindow( s );

		if( ValidateObject( hairobject ) )
		{
			if( pRace->IsHairRestricted() )
			{
				hairColor = ( hairobject->GetColour() );
				if( pRace->IsValidHair( hairColor ) )
				{
					hairColor = RandomHair( x );
					hairobject->SetColour( hairColor );
				}
			}
			if( pRace->NoHair() )
			{
				hairobject->Delete();
				hairobject = NULL;
			}
		}
		if( pRace->RequiresBeard() && s->GetID() == 0x0190 && !ValidateObject( beardobject ) )
		{
			if( pRace->IsBeardRestricted() )
				hairColor = RandomBeard( x );
			else
				hairColor = 0x0480;
			CItem *n = Items->CreateItem( NULL, s, 0x204C, 1, hairColor, OT_ITEM );
			if( n != NULL )
			{
				n->SetLayer( IL_FACIALHAIR );
				if( n->SetCont( s ) )
				{
					beardobject = n;
				}
			}
		}
		if( ValidateObject( beardobject ) )
		{
			if( pRace->IsBeardRestricted() )
			{
				hairColor = beardobject->GetColour();
				if( pRace->IsValidBeard( hairColor ) )
				{
					hairColor = RandomBeard( x );
					beardobject->SetColour( hairColor );
				}
			}
			if( pRace->NoBeard() )
			{
				beardobject->Delete();
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
				s->SetOrgSkin( hairColor );
			}
		}
	}
	else 
		mSock->sysmessage( 372 );
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
	SKILLVAL modifier = races[x]->Skill( skill );
	if( modifier >= static_cast<SKILLVAL>(combat.size()) )
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
	SKILLVAL modifier = races[x]->Skill( skill );
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

bool cRaces::Affect( RACEID race, WeatherType element ) const
{
	bool rValue = false;
	if( !InvalidRace( race ) )
	{
		switch( element )
		{
		case LIGHT:		rValue = races[race]->AffectedByLight();		break;
		case RAIN:		rValue = races[race]->AffectedByRain();			break;
		case COLD:		rValue = races[race]->AffectedByCold();			break;
		case HEAT:		rValue = races[race]->AffectedByHeat();			break;
		case LIGHTNING: rValue = races[race]->AffectedByLightning();	break;
		case SNOW:		rValue = races[race]->AffectedBySnow();			break;
		default:														break;
		}
	}
	return rValue;
}

void cRaces::Affect( RACEID race, WeatherType element, bool value )
{
	if( !InvalidRace( race ) )
	{
		switch( element )
		{
		case LIGHT:		races[race]->AffectedByLight( value );			break;
		case RAIN:		races[race]->AffectedByRain( value );			break;
		case COLD:		races[race]->AffectedByCold( value );			break;
		case HEAT:		races[race]->AffectedByHeat( value );			break;
		case LIGHTNING: races[race]->AffectedByLightning( value );		break;
		case SNOW:		races[race]->AffectedBySnow( value );			break;
		default:														break;
		}
	}
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
const std::string CRace::Name( void ) const
{
	return raceName;
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
void CRace::Name( const std::string newName )
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
	size_t sNum = RandomNum( static_cast< size_t >(0), skinColours.size() - 1 );
	return (COLOUR)RandomNum( skinColours[sNum].cMin, skinColours[sNum].cMax );
}
COLOUR CRace::RandomHair( void ) const
{
	if( !IsHairRestricted() )
		return 0;
	size_t sNum = RandomNum( static_cast< size_t >(0), hairColours.size() - 1 );
	return (COLOUR)RandomNum( hairColours[sNum].cMin, hairColours[sNum].cMax );
}
COLOUR CRace::RandomBeard( void ) const
{
	if( !IsBeardRestricted() )
		return 0;
	size_t sNum = RandomNum( static_cast< size_t >(0), beardColours.size() - 1 );
	return (COLOUR)RandomNum( beardColours[sNum].cMin, beardColours[sNum].cMax );
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

void CRace::Load( size_t sectNum, int modCount )
{
	UString tag;
	UString data;
	UString UTag;
	SI32 raceDiff = 0;
	UString sect = "RACE " + UString::number( sectNum );
	ScriptSection *RacialPart = FileLookup->FindEntry( sect, race_def );

	COLOUR beardMin = 0, skinMin = 0, hairMin = 0;

	for( tag = RacialPart->First(); !RacialPart->AtEnd(); tag = RacialPart->Next() )
	{
		UTag = tag.upper();
		data = RacialPart->GrabData();
		switch( tag[0] )
		{
			case 'a':
			case 'A':
				if( UTag == "ARMORREST" )
					ArmourClassRestriction( data.toUByte() );		// 8 classes, value 0 is all, else it's a bit comparison
				break;

			case 'b':
			case 'B':
				if( UTag == "BEARDMIN" )
					beardMin = data.toUShort();
				else if( UTag == "BEARDMAX" )
					beardColours.push_back( ColourPair( beardMin, data.toUShort() ) );
				break;

			case 'c':
			case 'C':
				if( UTag == "COLDAFFECT" )	// are we affected by light?
					AffectedByCold( true );
				else if( UTag == "COLDDAMAGE" )	// how much damage to take from light
					WeatherDamage( data.toByte(), COLD );
				else if( UTag == "COLDSECS" )		// how often light affects in secs
					WeatherSeconds( data.toUByte(), COLD );
				break;

			case 'd':
			case 'D':
				if( UTag == "DEXCAP" )
					Skill( data.toUShort(), DEXTERITY );
				break;

			case 'g':
			case 'G':
				if( UTag == "GENDER" )
				{
					if( data.upper() == "MALE" )
						GenderRestriction( MALE );
					else if( data.upper() == "FEMALE" )
						GenderRestriction( FEMALE );
					else
						GenderRestriction( MALE );
				}
				break;

			case 'h':
			case 'H':
				if( UTag == "HAIRMIN" )
					hairMin = data.toUShort();
				else if( UTag == "HAIRMAX" )
					hairColours.push_back( ColourPair( hairMin, data.toUShort() ) );
				else if( UTag == "HEATAFFECT" )	// are we affected by light?
					AffectedByHeat( true );
				else if( UTag == "HEATDAMAGE" )	// how much damage to take from light
					WeatherDamage( data.toByte(), HEAT );
				else if( UTag == "HEATSECS" )		// how often light affects in secs
					WeatherSeconds( data.toUByte(), HEAT );
				else if( UTag == "HPMOD" ) // how much additional percent of strength are hitpoints
					HPModifier( data.toShort() );
				break;

			case 'i':
			case 'I':
				if( UTag == "INTCAP" )
					Skill( data.toUShort(), INTELLECT );
				break;

			case 'l':
			case 'L':
				if( UTag == "LIGHTAFFECT" )	// are we affected by light?
					AffectedByLight( true );
				else if( UTag == "LIGHTDAMAGE" )	// how much damage to take from light
					WeatherDamage( data.toByte(), LIGHT );
				else if( UTag == "LIGHTLEVEL" )	// light level at which to take damage
					LightLevel( data.toUByte() );
				else if( UTag == "LIGHTSECS" )		// how often light affects in secs
					WeatherSeconds( data.toUByte(), LIGHT );

				else if( UTag == "LIGHTNINGAFFECT" )	// are we affected by light?
					AffectedByLightning( true );
				else if( UTag == "LIGHTNINGDAMAGE" )	// how much damage to take from light
					WeatherDamage( data.toByte(), LIGHTNING );
				else if( UTag == "LIGHTNINGSECS" )		// how often light affects in secs
					WeatherSeconds( data.toUByte(), LIGHTNING );
				else if( UTag == "LANGUAGEMIN" ) // set language min 
					LanguageMin( data.toUShort() ); 
				break;

			case 'm':
			case 'M':
				if( UTag == "MAGICRESISTANCE" )	// magic resistance?
					MagicResistance( data.toFloat() );
				else if( UTag == "MANAMOD" ) // how much additional percent of int are mana
					ManaModifier( data.toShort() );
				break;

			case 'n':
			case 'N':
				if( UTag == "NAME" )
					Name( data );
				else if( UTag == "NOBEARD" )
					NoBeard( true );
				else if( UTag == "NIGHTVIS" )					// night vision level... light bonus
					NightVision( data.toUByte() );
				break;

			case 'p':
			case 'P':
				if( UTag == "PLAYERRACE" ) // is it a player race?
					IsPlayerRace( (data.toUByte() != 0) );
				else if( UTag == "POISONRESISTANCE" )	// poison resistance?
					PoisonResistance( data.toFloat() );
				else if( UTag == "PARENTRACE" )
				{
					CRace *pRace = Races->Race( data.toUShort() );
					if( pRace != NULL )
						(*this) = (*pRace);
				}
				break;

			case 'r':
			case 'R':
				if( UTag == "REQUIREBEARD" )
					RequiresBeard( true );
				else if( UTag == "RAINAFFECT" )	// are we affected by light?
					AffectedByRain( true );
				else if( UTag == "RAINDAMAGE" )	// how much damage to take from light
					WeatherDamage( data.toByte(), RAIN );
				else if( UTag == "RAINSECS" )		// how often light affects in secs
					WeatherSeconds( data.toUByte(), RAIN );
				else if( UTag == "RACERELATION" )
				{
					if( data.sectionCount( " " ) != 0 )
						RaceRelation( data.section( " ", 1, 1 ).stripWhiteSpace().toByte(), data.section( " ", 0, 0 ).stripWhiteSpace().toUShort() );
				}
				else if( UTag == "RACIALENEMY" )
				{
					raceDiff = data.toLong();
					if( raceDiff > static_cast<SI32>(racialEnemies.size()) )
						Console << "Error in race " << static_cast< UI32 >(sectNum) << ", invalid enemy race " << raceDiff << myendl;
					else
						RaceRelation( MIN_ENEMY, static_cast<RACEID>(raceDiff) );
				}
				else if( UTag == "RACIALAID" )
				{
					raceDiff = data.toLong();
					if( raceDiff > static_cast<SI32>(racialEnemies.size() ))
						Console << "Error in race " << static_cast< UI32 >(sectNum) << ", invalid ally race " <<  raceDiff << myendl;
					else
						RaceRelation( MIN_ALLY, static_cast<RACEID>(raceDiff ));
				}
				break;

			case 's':
			case 'S':
				if( UTag == "STRCAP" )
					Skill( data.toUShort(), STRENGTH );
				else if( UTag == "SKINMIN" )
					skinMin = data.toUShort();
				else if( UTag == "SKINMAX" )
					skinColours.push_back( ColourPair( skinMin, data.toUShort() ) );
				else if( UTag == "SNOWAFFECT" )	// are we affected by light?
					AffectedBySnow( true );
				else if( UTag == "SNOWDAMAGE" )	// how much damage to take from light
					WeatherDamage( data.toByte(), SNOW );
				else if( UTag == "SNOWSECS" )		// how often light affects in secs
					WeatherSeconds( data.toUByte(), SNOW );
				else if( UTag == "STAMMOD" ) // how much additional percent of int are mana
					StamModifier( data.toShort() );
				break;

			case 'v':
			case 'V':
				if( UTag == "VISRANGE" )					// set visibility range ... defaults to 18
					VisibilityRange( data.toByte() );
				break;
		}

		for( int iCountA = 0; iCountA < ALLSKILLS; ++iCountA )
		{
			UString skillthing = skillname[iCountA];
			skillthing += "G";
			if( skillthing == tag )
				Skill( data.toUShort(), iCountA );
			else
			{
				skillthing = skillname[iCountA];
				skillthing += "L";
				if( skillthing == tag )
					Skill( modCount + data.toUShort(), iCountA );
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

size_t cRaces::Count( void ) const
{
	return races.size();
}

}
