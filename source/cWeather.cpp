#include "uox3.h"
#include "cWeather.hpp"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"

namespace UOX
{

#define EFFECTIVE 2
const UI08 MAXVAL	= 0;
const UI08 MINVAL	= 1;
const UI08 CURRVAL	= 2;
const UI08 TEMP		= 0;
const UI08 WIND		= 1;

cWeatherAb *Weather = NULL;
// Version History
// 1.0		Abaddon		Unknown
//			Initial implementation
//			Based on a weatherSys structure system
// 2.0		Abaddon		11th April, 2001
//			Rewritten system
//			Each weather sys is a CWeather class
//			PeriodicUpdate for CWeather also updates light level
//			cWeatherAb simplified to access CWeather containers
//			Replaced a new'd array with a vector of CWeather


CWeather::CWeather()
{
	memset( weather, 0, sizeof( WeathPart ) * WEATHNUM );
	ColdChance( 5 );
	RainChance( 10 );
	SnowChance( 1 );
	HeatChance( 10 );
	LightMin( 300 );
	LightMax( 300 );
	CurrentLight( 300 );

	MaxTemp( 30 );
	MinTemp( 0 );
	Temp( 15 );
	MaxWindSpeed( 0 );
	MinWindSpeed( 0 );
	WindSpeed( 0 );
	SnowThreshold( 15.0f );
}

CWeather::~CWeather()
{
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool HeatActive( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system is having a heat wave
//o---------------------------------------------------------------------------o
bool CWeather::HeatActive( void ) const
{
	return weather[HEAT].Active;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool ColdActive( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system undergoing a cold snap
//o---------------------------------------------------------------------------o
bool CWeather::ColdActive( void ) const
{
	return weather[COLD].Active;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 CurrentLight( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the current light level of the system, IF
//|					 LightMin and LightMax are less than 300
//o---------------------------------------------------------------------------o
R32 CWeather::CurrentLight( void ) const
{
	return light[CURRVAL];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CurrentLight( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the current light value to value
//o---------------------------------------------------------------------------o
void CWeather::CurrentLight( R32 value )
{
	light[CURRVAL] = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool PeriodicUpdate( void )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the current light level of the system and the 
//|					 current temperature of the system.
//|					 Wind is not currently updated
//|					 Chooses a random intensity if snow or rain is falling
//o---------------------------------------------------------------------------o
bool CWeather::PeriodicUpdate( void )
{
	R32 currTemp;

	UI08 hour	= cwmWorldState->ServerData()->ServerTimeHours();
	UI08 minute = cwmWorldState->ServerData()->ServerTimeMinutes();
	bool ampm	= cwmWorldState->ServerData()->ServerTimeAMPM();

	if( LightMin() < 300 && LightMax() < 300 )
	{
		R32 hourIncrement = static_cast<R32>(fabs( ( LightMax() - LightMin() ) / 12.0f ));	// we want the amount to subtract from LightMax in the morning / add to LightMin in evening
		R32 minuteIncrement = hourIncrement / 60.0f;
		R32 tempLight = hourIncrement * (R32)hour + minuteIncrement * (R32)minute;
		if( ampm )
			CurrentLight( LightMin() + tempLight );
		else
			CurrentLight( LightMax() - tempLight );
	}

	R32	effTempMax			= EffectiveMaxTemp();
	R32	effTempMin			= EffectiveMinTemp();
	R32	tempHourIncrement	= static_cast<R32>(fabs( ( effTempMax - effTempMin ) / 12.0f ));
	R32	tempMinuteIncrement = tempHourIncrement / 60.0f;

	R32	tempLightChange		= tempHourIncrement * (R32)hour + tempMinuteIncrement * (R32)minute;

	if( ampm )
		currTemp = effTempMax - tempLightChange;	// maximum temperature minus time
	else
		currTemp = effTempMin + tempLightChange;	// minimum temperature plus time 
	Temp( currTemp );
	if( SnowActive() && SnowThreshold() > currTemp )
		SnowIntensity( (UI08)RandomNum( 1, 4 ) );
	else if( RainActive() )
		RainIntensity( (UI08)RandomNum( 1, 4 ) );

	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void NewDay( void )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the system up for a new day
//|					 Heat wave, cold snap, snow and rain is recalculated
//|					 Under heat wave or cold snap, new effective max and min
//|					 temperatures are calculated
//o---------------------------------------------------------------------------o
void CWeather::NewDay( void )
{
	bool isSnowing = false, isRaining = false, isHeatWave = false, isColdDay = false;

	R32	effTempMax		= MaxTemp();
	R32	effTempMin		= MinTemp();
	UI08	chanceForWeath	= (UI08)RandomNum( 0, 100 );
	if( chanceForWeath < HeatChance() )
	{
		isHeatWave = true;
		HeatIntensity( (UI08)RandomNum( 1, 4 ) );
		effTempMax += RandomNum( 0, (int)(effTempMax / 10 * HeatIntensity() ) );
	}
	if( chanceForWeath < ColdChance() )
	{
		isColdDay = true;
		ColdIntensity( (UI08)RandomNum( 1, 4 ) );
		effTempMin -= RandomNum( 0, (int)(effTempMin / 10 * ColdIntensity() ) );
	}
	if( chanceForWeath < SnowChance() )
	{
		isSnowing = true;
		SnowIntensity( (UI08)RandomNum( 1, 4 ) );
	}
	if( chanceForWeath < RainChance() )
	{
		isRaining = true;
		RainIntensity( RandomNum( 1, 4 ) );
	}

	HeatActive( isHeatWave );
	ColdActive( isColdDay );
	SnowActive( isSnowing );
	RainActive( isRaining );

	EffectiveMaxTemp( effTempMax );
	EffectiveMinTemp( effTempMin );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Intensity( UI08 weathType ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the weather weathType
//o---------------------------------------------------------------------------o
SI08 CWeather::Intensity( UI08 weathType ) const
{
	return weather[weathType].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( UI08 weathType, SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the intensity of weather weathType to value
//o---------------------------------------------------------------------------o
void CWeather::Intensity( UI08 weathType, SI08 value )
{
	weather[weathType].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Chance( UI08 weathType ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of weather weathType occurring
//o---------------------------------------------------------------------------o
SI08 CWeather::Chance( UI08 weathType ) const
{
	return weather[weathType].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatActive( UI08 weathType, SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of weather weathType occuring to value
//o---------------------------------------------------------------------------o
void CWeather::Chance( UI08 weathType, SI08 value )
{
	weather[weathType].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 Value( UI08 valType, UI08 valOff ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the value associated with valType and valOff
//|					 valType is either MAXVAL, MINVAL, or CURRVAL
//|					 valOff is either WIND, TEMP or EFFECTIVE
//o---------------------------------------------------------------------------o
R32 CWeather::Value( UI08 valType, UI08 valOff ) const
{
	return assortVals[valType][valOff];
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Value( UI08 valType, UI08 valOff, R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the value associated with valType and valOff to value
//|					 valType is either MAXVAL, MINVAL or CURRVAL
//|					 valOff is either WIND, TEMP or EFFECTIVE
//o---------------------------------------------------------------------------o
void CWeather::Value( UI08 valType, UI08 valOff, R32 value )
{
	assortVals[valType][valOff] = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 SnowIntensity( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the snow (if any)
//o---------------------------------------------------------------------------o
SI08 CWeather::SnowIntensity( void ) const
{
	return weather[SNOW].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool RainIntensity( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the rain, if any
//o---------------------------------------------------------------------------o
SI08 CWeather::RainIntensity( void ) const
{
	return weather[RAIN].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool HeatIntensity( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the heat wave, if any
//o---------------------------------------------------------------------------o
SI08 CWeather::HeatIntensity( void ) const
{
	return weather[HEAT].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ColdIntensity( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the cold snap, if any
//o---------------------------------------------------------------------------o
SI08 CWeather::ColdIntensity( void ) const
{
	return weather[COLD].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MaxTemp( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the maximum temperature of the day (non effective)
//o---------------------------------------------------------------------------o
R32 CWeather::MaxTemp( void ) const
{
	return Value( MAXVAL, TEMP );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MinTemp( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the minimum temperature of the day (non effective)
//o---------------------------------------------------------------------------o
R32 CWeather::MinTemp( void ) const
{
	return Value( MINVAL, TEMP );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 Temp( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the current temperature of the day
//o---------------------------------------------------------------------------o
R32 CWeather::Temp( void ) const
{
	return Value( CURRVAL, TEMP );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MaxWindSpeed( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the maximum wind speed for the day
//o---------------------------------------------------------------------------o
R32 CWeather::MaxWindSpeed( void ) const
{
	return Value( MAXVAL, WIND );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MinWindSpeed( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the minimum wind speed for the day
//o---------------------------------------------------------------------------o
R32 CWeather::MinWindSpeed( void ) const
{
	return Value( MINVAL, WIND );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 WindSpeed( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the current wind speed
//o---------------------------------------------------------------------------o
R32 CWeather::WindSpeed( void ) const
{
	return Value( CURRVAL, WIND );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 RainChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of rain for the day
//o---------------------------------------------------------------------------o
SI08 CWeather::RainChance( void ) const
{
	return weather[RAIN].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 SnowChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of snow for the day
//o---------------------------------------------------------------------------o
SI08 CWeather::SnowChance( void ) const
{
	return weather[SNOW].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 HeatChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of a heat wave for the day
//o---------------------------------------------------------------------------o
SI08 CWeather::HeatChance( void ) const
{
	return weather[HEAT].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ColdChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of a cold snap for the day
//o---------------------------------------------------------------------------o
SI08 CWeather::ColdChance( void ) const
{
	return weather[COLD].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 LightMin( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the minimum light level for the day (brightest)
//o---------------------------------------------------------------------------o
R32 CWeather::LightMin( void ) const
{
	return light[MINVAL];
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 LightMax( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the maximum light level for the day (darkest)
//o---------------------------------------------------------------------------o
R32 CWeather::LightMax( void ) const
{
	return light[MAXVAL];
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool RainActive( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system is raining
//o---------------------------------------------------------------------------o
bool CWeather::RainActive( void ) const
{
	return weather[RAIN].Active;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool SnowActive( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system is snowing
//o---------------------------------------------------------------------------o
bool CWeather::SnowActive( void ) const
{
	return weather[SNOW].Active;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 SnowThreshold( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the temperature below which snow kicks in
//o---------------------------------------------------------------------------o
R32 CWeather::SnowThreshold( void ) const
{
	return snowThreshold;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void WindSpeed( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the current wind speed
//o---------------------------------------------------------------------------o
void CWeather::WindSpeed( R32 value )
{
	Value( CURRVAL, WIND, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MaxWindSpeed( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the maximum wind speed
//o---------------------------------------------------------------------------o
void CWeather::MaxWindSpeed( R32 value )
{
	Value( MAXVAL, WIND, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MinWindSpeed( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum wind speed
//o---------------------------------------------------------------------------o
void CWeather::MinWindSpeed( R32 value )
{
	Value( MINVAL, WIND, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MaxTemp( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the maximum (noneffective) temperature
//o---------------------------------------------------------------------------o
void CWeather::MaxTemp( R32 value )
{
	Value( MAXVAL, TEMP, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MinTemp( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum (noneffective) temperature
//o---------------------------------------------------------------------------o
void CWeather::MinTemp( R32 value )
{
	Value( MINVAL, TEMP, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Temp( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the current temperature
//o---------------------------------------------------------------------------o
void CWeather::Temp( R32 value )
{
	Value( CURRVAL, TEMP, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SnowIntensity( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the snow intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::SnowIntensity( SI08 value )
{
	weather[SNOW].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatIntensity( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the heat intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::HeatIntensity( SI08 value )
{
	weather[HEAT].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RainIntensity( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the rain intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::RainIntensity( SI08 value )
{
	weather[RAIN].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void ColdIntensity( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the cold intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::ColdIntensity( SI08 value )
{
	weather[COLD].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RainChance( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of rain in the system
//o---------------------------------------------------------------------------o
void CWeather::RainChance( SI08 value )
{
	weather[RAIN].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SnowChance( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of snow in the system
//o---------------------------------------------------------------------------o
void CWeather::SnowChance( SI08 value )
{
	weather[SNOW].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatChance( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of a heat wave in the system
//o---------------------------------------------------------------------------o
void CWeather::HeatChance( SI08 value )
{
	weather[HEAT].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void ColdChance( SI08 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of a cold snap in the system
//o---------------------------------------------------------------------------o
void CWeather::ColdChance( SI08 value )
{
	weather[COLD].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SnowThreshold( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the temperature at which below it snows
//o---------------------------------------------------------------------------o
void CWeather::SnowThreshold( R32 value )
{
	snowThreshold = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void LightMin( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum (brightest) light value of the system
//o---------------------------------------------------------------------------o
void CWeather::LightMin( R32 value )
{
	light[MINVAL] = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void LightMax( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the maximum (darkest) light value of the system
//o---------------------------------------------------------------------------o
void CWeather::LightMax( R32 value )
{
	light[MAXVAL] = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 EffectiveMaxTemp( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the effective maximum temperature for the day
//o---------------------------------------------------------------------------o
R32 CWeather::EffectiveMaxTemp( void ) const
{
	return Value( MAXVAL, EFFECTIVE );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 EffectiveMinTemp( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the effective minimum temperature for the day
//o---------------------------------------------------------------------------o
R32 CWeather::EffectiveMinTemp( void ) const
{
	return Value( MINVAL, EFFECTIVE );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void EffectiveMaxTemp( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the maximum effective temperature for the day
//o---------------------------------------------------------------------------o
void CWeather::EffectiveMaxTemp( R32 value )
{
	Value( MAXVAL, EFFECTIVE, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void EffectiveMinTemp( R32 value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum effective temperature for the day
//o---------------------------------------------------------------------------o
void CWeather::EffectiveMinTemp( R32 value )
{
	Value( MINVAL, EFFECTIVE, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RainActive( bool value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether rain is falling or not
//o---------------------------------------------------------------------------o
void CWeather::RainActive( bool value )
{
	weather[RAIN].Active = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SnowActive( bool value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether snow is falling or not
//o---------------------------------------------------------------------------o
void CWeather::SnowActive( bool value )
{
	weather[SNOW].Active = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatActive( bool value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether heat wave is active
//o---------------------------------------------------------------------------o
void CWeather::HeatActive( bool value )
{
	weather[HEAT].Active = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void ColdActive( bool value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether a cold snap is active
//o---------------------------------------------------------------------------o
void CWeather::ColdActive( bool value )
{
	weather[COLD].Active = value;
}


//o---------------------------------------------------------------------------o
//|   Function    -  cWeatherAb()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Default weather constructor
//o---------------------------------------------------------------------------o
cWeatherAb::cWeatherAb()
{
	weather.resize( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  ~cWeatherAb()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Destroys any allocated memory
//o---------------------------------------------------------------------------o
cWeatherAb::~cWeatherAb()
{
	weather.clear();
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI32 Count( void ) const
//|   Date        -  31st July, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns count of weather systems
//o---------------------------------------------------------------------------o
size_t cWeatherAb::Count( void ) const
{
	return weather.size();
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool Load( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system loaded okay
//o---------------------------------------------------------------------------o
bool cWeatherAb::Load( void )
{
	weathID wthCount = 0;
	bool done = false;
	UString sect;
	while( !done )
	{
		sect = "WEATHERAB " + UString::number( wthCount );
		ScriptSection *tempSect = FileLookup->FindEntry( sect, weathab_def );
		if( tempSect == NULL )
			done = true;
		else
			++wthCount;
	}
	ScriptSection *WeatherStuff = NULL;
	weather.resize( wthCount );

	UString tag;
	UString data;
	UString UTag;

	for( UI16 i = 0; i < weather.size(); ++i )
	{
		sect = "WEATHERAB " + UString::number( i );
		WeatherStuff = FileLookup->FindEntry( sect, weathab_def );
		if( WeatherStuff == NULL )
			continue;

		for( tag = WeatherStuff->First(); !WeatherStuff->AtEnd(); tag = WeatherStuff->Next() )
		{
			UTag = tag.upper();
			data = WeatherStuff->GrabData();
			switch( tag[0] )
			{
				case 'c':
				case 'C':
					if( UTag == "COLDCHANCE" )			// chance for a cold day
						ColdChance( static_cast<weathID>(i), data.toByte() );
					else if( UTag == "COLDINTENSITY" )	// cold intensity
						ColdIntensity( static_cast<weathID>(i), data.toByte() );
					break;

				case 'h':
				case 'H':
					if( UTag == "HEATCHANCE" )			// chance for a hot day
						HeatChance( static_cast<weathID>(i), data.toByte() );
					else if( UTag == "HEATINTENSITY" )	// heat intensity
						HeatIntensity( static_cast<weathID>(i), data.toByte() );
					break;

				case 'l':
				case 'L':
					if( UTag == "LIGHTMIN" )			// minimum light level
						LightMin( static_cast<weathID>(i), data.toFloat() );
					else if( UTag == "LIGHTMAX" )		// maximum light level
						LightMax( static_cast<weathID>(i), data.toFloat() );
					break;

				case 'm':
				case 'M':
					if( UTag == "MAXTEMP" )				// maximum temperature
						MaxTemp( static_cast<weathID>(i), data.toFloat() );
					else if( UTag == "MINTEMP" )		// minimum temperature
						MinTemp( static_cast<weathID>(i), data.toFloat() );
					else if( UTag == "MAXWIND" )		// maximum wind speed
						MaxWindSpeed( static_cast<weathID>(i), data.toFloat() );
					else if( UTag == "MINWIND" )		// minimum wind speed
						MinWindSpeed( static_cast<weathID>(i), data.toFloat() );
					break;

				case 'r':
				case 'R':
					if( UTag == "RAINCHANCE" )			// chance of rain
						RainChance( static_cast<weathID>(i), data.toByte() );
					else if( UTag == "RAININTENSITY" )	// intensity of rain
						RainIntensity( static_cast<weathID>(i), data.toByte() );
					break;
				case 's':
				case 'S':
					if( UTag == "SNOWCHANCE" )			// chance of snow
						SnowChance( static_cast<weathID>(i), data.toByte() );
					else if( UTag == "SNOWINTENSITY" )	// intensity of snow
						SnowIntensity( static_cast<weathID>(i), data.toByte() );
					else if( UTag == "SNOWTHRESHOLD" )	// temperature at which snow kicks in
						SnowThreshold( static_cast<weathID>(i), data.toFloat() );
					break;
			}
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Intensity( weathID toCheck, UI08 weathType )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to return intensity of a weather type
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::Intensity( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].Intensity( weathType );
}
		
//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( weathID toCheck, UI08 weathType, SI08 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to set the intensity of a particular weather type
//o---------------------------------------------------------------------------o
void cWeatherAb::Intensity( weathID toCheck, UI08 weathType, SI08 value )
{
	weather[toCheck].Intensity( weathType, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 Chance( weathID toCheck, UI08 weathType )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to return chance of a weather type
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::Chance( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].Chance( weathType );
}
		
//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( weathID toCheck, UI08 weathType, SI08 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to set the chance of finding a weather type
//o---------------------------------------------------------------------------o
void cWeatherAb::Chance( weathID toCheck, UI08 weathType, SI08 value )
{
	weather[toCheck].Chance( weathType, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 Value( weathID toCheck, UI08 valType, UI08 valOff )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to return value of some weather types
//o---------------------------------------------------------------------------o
R32 cWeatherAb::Value( weathID toCheck, UI08 valType, UI08 valOff )
// PRE:		toCheck is valid, valType is valid, valOff is valid
// POST:	returns value of valType and valOff in toCheck
{
	return weather[toCheck].Value( valType, valOff );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( weathID toCheck, UI08 valType, UI08 valOff, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to set the value of a particular weather type
//o---------------------------------------------------------------------------o
void cWeatherAb::Value( weathID toCheck, UI08 valType, UI08 valOff, R32 value )
// PRE:		toCheck is valid, valType and valOff is valid, value is valid
// POST:	value of valType and valOff in toCheck is value
{
	weather[toCheck].Value( valType, valOff, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 SnowIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's snow
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::SnowIntensity( weathID toCheck )
{
	return Intensity( toCheck, SNOW );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 RainIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's rain
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::RainIntensity( weathID toCheck )
{
	return Intensity( toCheck, RAIN );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 HeatIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's heat
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::HeatIntensity( weathID toCheck )
{
	return Intensity( toCheck, HEAT );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ColdIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's cold
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::ColdIntensity( weathID toCheck )
{
	return Intensity( toCheck, COLD );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MaxTemp( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the maximum temperature of toCheck
//o---------------------------------------------------------------------------o
R32 cWeatherAb::MaxTemp( weathID toCheck )
{
	return Value( toCheck, MAXVAL, TEMP );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MinTemp( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the minimum temperature of toCheck
//o---------------------------------------------------------------------------o
R32 cWeatherAb::MinTemp( weathID toCheck )
{
	return Value( toCheck, MINVAL, TEMP );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 Temp( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the temperature of toCheck
//o---------------------------------------------------------------------------o
R32 cWeatherAb::Temp( weathID toCheck )
{
	return Value( toCheck, CURRVAL, TEMP );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MaxWindSpeed( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the maximum wind speed of toCheck
//o---------------------------------------------------------------------------o
R32 cWeatherAb::MaxWindSpeed( weathID toCheck )
{
	return Value( toCheck, MAXVAL, WIND );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 MinWindSpeed( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the minimum wind speed of toCheck
//o---------------------------------------------------------------------------o
R32 cWeatherAb::MinWindSpeed( weathID toCheck )
{
	return Value( toCheck, MINVAL, WIND );
}

//o---------------------------------------------------------------------------o
//|   Function    -  R32 WindSpeed( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the wind speed of toCheck
//o---------------------------------------------------------------------------o
R32 cWeatherAb::WindSpeed( weathID toCheck )
{
	return Value( toCheck, CURRVAL, WIND );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void WindSpeed( weathID toCheck, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the wind speed of toCheck
//o---------------------------------------------------------------------------o
void cWeatherAb::WindSpeed( weathID toCheck, R32 value )
{
	Value( toCheck, CURRVAL, WIND, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MaxWindSpeed( weathID toCheck, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the maximum wind speed of toCheck
//o---------------------------------------------------------------------------o
void cWeatherAb::MaxWindSpeed( weathID toCheck, R32 value )
{
	Value( toCheck, MAXVAL, WIND, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MinWindSpeed( weathID toCheck, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum wind speed of toCheck
//o---------------------------------------------------------------------------o
void cWeatherAb::MinWindSpeed( weathID toCheck, R32 value )
{
	Value( toCheck, MINVAL, WIND, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MaxTemp( weathID toCheck, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the maximum temperature of toCheck
//o---------------------------------------------------------------------------o
void cWeatherAb::MaxTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MAXVAL, TEMP, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void MinTemp( weathID toCheck, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum temperature of toCheck
//o---------------------------------------------------------------------------o
void cWeatherAb::MinTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MINVAL, TEMP, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Temp( weathID toCheck, R32 value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the temperature of toCheck
//o---------------------------------------------------------------------------o
void cWeatherAb::Temp( weathID toCheck, R32 value )
{
	Value( toCheck, CURRVAL, TEMP, value );
}

void cWeatherAb::SnowIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, SNOW, value );
}
void cWeatherAb::RainIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, RAIN, value );
}
void cWeatherAb::HeatIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, HEAT, value );
}
void cWeatherAb::ColdIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, COLD, value );
}
void cWeatherAb::RainChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, RAIN, value );
}
void cWeatherAb::SnowChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, SNOW, value );
}
void cWeatherAb::HeatChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, HEAT, value );
}
void cWeatherAb::ColdChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, COLD, value );
}

SI08 cWeatherAb::RainChance( weathID toCheck )
{
	return Chance( toCheck, RAIN );
}
SI08 cWeatherAb::SnowChance( weathID toCheck )
{
	return Chance( toCheck, SNOW );
}
SI08 cWeatherAb::HeatChance( weathID toCheck )
{
	return Chance( toCheck, HEAT );
}
SI08 cWeatherAb::ColdChance( weathID toCheck )
{
	return Chance( toCheck, COLD );
}

R32 cWeatherAb::SnowThreshold( weathID toCheck )
{
	return weather[toCheck].SnowThreshold();
}

void cWeatherAb::SnowThreshold( weathID toCheck, R32 value )
{
	weather[toCheck].SnowThreshold( value );
}

bool cWeatherAb::NewDay( void )
{
	std::vector< CWeather >::iterator wIter;
	for( wIter = weather.begin(); wIter != weather.end(); ++wIter )
		(*wIter).NewDay();
	return true;
}

void cWeatherAb::EffectiveMaxTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MAXVAL, EFFECTIVE, value );
}
void cWeatherAb::EffectiveMinTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MINVAL, EFFECTIVE, value );
}

R32 cWeatherAb::EffectiveMaxTemp( weathID toCheck )
{
	return Value( toCheck, MAXVAL, EFFECTIVE );
}
R32 cWeatherAb::EffectiveMinTemp( weathID toCheck )
{
	return Value( toCheck, MINVAL, EFFECTIVE );
}

bool cWeatherAb::DoStuff( void )
{
	std::vector< CWeather >::iterator wIter;
	for( wIter = weather.begin(); wIter != weather.end(); ++wIter )
		(*wIter).PeriodicUpdate();
	return true;
}

void cWeatherAb::RainActive( weathID toCheck, bool value )
{
	weather[toCheck].RainActive( value );
}
void cWeatherAb::SnowActive( weathID toCheck, bool value )
{
	weather[toCheck].SnowActive( value );
}

bool cWeatherAb::RainActive( weathID toCheck )
{
	return weather[toCheck].RainActive();
}
bool cWeatherAb::SnowActive( weathID toCheck )
{
	return weather[toCheck].SnowActive();
}

bool cWeatherAb::DoPlayerStuff( CSocket *s, CChar *p )
{
	if( !ValidateObject( p ) || p->IsNpc() )
		return true;
	weathID currval = p->GetRegion()->GetWeather();
	if( currval > weather.size() || weather.empty() )
	{
		if( s != NULL )
		{
			CPWeather dry( 0xFF, 0x00, 0 );
			s->Send( &dry );
		}
		return false;
	}

	bool isSnowing = SnowActive( currval );
	bool isRaining = RainActive( currval );
	if( isSnowing && SnowThreshold( currval ) > Temp( currval ) )
	{
		DoPlayerWeather( s, 2, (SI08)Temp( currval ) );
		if( p->GetWeathDamage( SNOW ) == 0 )
			p->SetWeathDamage( BuildTimeValue( Races->Secs( p->GetRace(), SNOW ) ), SNOW );
	} 
	else if( isRaining )
	{
		DoPlayerWeather( s, 1, (SI08)Temp( currval ) );
		if( p->GetWeathDamage( RAIN ) == 0 )
			p->SetWeathDamage( BuildTimeValue( Races->Secs( p->GetRace(), RAIN ) ), RAIN );
	}
	else
	{
		DoPlayerWeather( s, 0, (SI08)Temp( currval ) );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
	}
	return true;
}

void cWeatherAb::DoPlayerWeather( CSocket *s, UI08 weathType, SI08 currentTemp )
// Weather Types 
// 0 - dry
// 1 - rain
// 2 - snow
// 3 - rain and snow
{
//	Byte 1 - 0x65 (Weather)
//	Byte 2 - Weather Type(  0, rain, 1, fierce storm, 2, snow, 3 a storm is brewing, and 0xFF none )
//	Byte 3 - Particle count (upper limit of 70)
//	Byte 4 - Temperature

	if( s == NULL )
		return;
	CPWeather dry( 0xFF, 0x00, currentTemp );
	CPWeather rain( 0x00, 0x40, currentTemp );
	CPWeather storm( 0x01, 0x40, currentTemp );
	CPWeather snow( 0x02, 0x40, currentTemp );
	CPWeather strmbrw( 0x03, 0x40, currentTemp );
	
	CChar *mChar = s->CurrcharObj();
	s->Send( &dry );

	switch( weathType )
	{
		case 0:								break;
		case 1:		s->Send( &rain );		break;
		case 2:
			Effects->PlaySound( mChar, 0x14 + RandomNum( 0, 1 ) );
			s->Send( &snow );
			break;
		case 3:
			s->Send( &rain );
			Effects->PlaySound( mChar, 0x14 + RandomNum( 0, 1 ) );
			s->Send( &snow );
			break;
		default:							break;
	}
}

void cWeatherAb::LightMin( weathID toCheck, R32 newValue )
{
	weather[toCheck].LightMin( newValue );
}
void cWeatherAb::LightMax( weathID toCheck, R32 newValue )
{
	weather[toCheck].LightMax( newValue );
}

R32 cWeatherAb::LightMin( weathID toCheck )
{
	return weather[toCheck].LightMin();
}
R32 cWeatherAb::LightMax( weathID toCheck )
{
	return weather[toCheck].LightMax();
}

CWeather *cWeatherAb::Weather( weathID toCheck )
{
	if( toCheck >= weather.size() )
		return NULL;
	else
		return &weather[toCheck];
}

// FUNCTION NEEDS REWORKING
bool doLightEffect( CSocket& mSock, CChar& mChar )
{
	bool didDamage = false;

	if( mChar.IsNpc() || mChar.IsGM() || !Races->Affect( mChar.GetRace(), LIGHT ) )
		return false;

	UI08 hour = cwmWorldState->ServerData()->ServerTimeHours();
	bool ampm = cwmWorldState->ServerData()->ServerTimeAMPM();

	if( mChar.GetFixedLight() != 255 )
	{
		if( hour < 5 && ampm || hour >= 5 && !ampm )	// time of day we can be burnt
		{
			if( mChar.GetWeathDamage( LIGHT ) != 0 )
			{
				if( mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
				{
					mSock.sysmessage( 1216 );
					mChar.SetHP( mChar.GetHP() - Races->Damage( mChar.GetRace(), LIGHT ) );
					mChar.SetWeathDamage( BuildTimeValue( Races->Secs( mChar.GetRace(), LIGHT ) ), LIGHT );
					Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
					Effects->PlaySound( (&mChar), 0x0208 );     
					didDamage = true;
				}
			}
			else
				mChar.SetWeathDamage( BuildTimeValue( Races->Secs( mChar.GetRace(), LIGHT ) ), LIGHT );
			
		}
		else if( hour < 6 && ampm || hour >= 4 && !ampm )	// slightly burnt at this time of day
		{
			if( mChar.GetWeathDamage( LIGHT ) != 0 )
			{
				if( mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
				{
					mSock.sysmessage( 1217 );
					mChar.SetHP( mChar.GetHP() - Races->Damage( mChar.GetRace(), LIGHT ) / 2 );
					mChar.SetWeathDamage( BuildTimeValue( static_cast<R32>(Races->Secs( mChar.GetRace(), LIGHT ) * 2 )), LIGHT );
					Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
					Effects->PlaySound( (&mChar), 0x0208 );     
					didDamage = true;
				}
			}
		}
		else
		{
			mChar.SetWeathDamage( 0, LIGHT );
			if( hour > 3 && hour < 4 && !ampm )
				mSock.sysmessage( 1215 );
		}
	}
	else
	{
		if( !mChar.inDungeon() )
		{
			if( hour < 5 && ampm || hour >= 5 && !ampm )
			{
				if( mChar.GetWeathDamage( LIGHT ) != 0 )
				{
					if( mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
					{
						mSock.sysmessage( 1216 );
						mChar.SetHP( mChar.GetHP() - Races->Damage( mChar.GetRace(), LIGHT ) );
						mChar.SetWeathDamage( BuildTimeValue( Races->Secs( mChar.GetRace(), LIGHT ) ), LIGHT );
						Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
						Effects->PlaySound( (&mChar), 0x0208 );     
						didDamage = true;
					}
				}
				else
					mChar.SetWeathDamage( BuildTimeValue( Races->Secs( mChar.GetRace(), LIGHT ) ), LIGHT );
				
			}
			else if( hour < 6 && ampm || hour >= 4 && !ampm )
			{
				if( mChar.GetWeathDamage( LIGHT ) != 0 )
				{
					if( mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
					{
						mSock.sysmessage( 1217 );
						mChar.SetHP( mChar.GetHP() - Races->Damage( mChar.GetRace(), LIGHT ) / 2 );
						mChar.SetWeathDamage( BuildTimeValue( static_cast<R32>(Races->Secs( mChar.GetRace(), LIGHT ) * 2 )), LIGHT );
						Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
						Effects->PlaySound( (&mChar), 0x0208 );     
						didDamage = true;
					}
				}
			}
			else
			{
				mChar.SetWeathDamage( 0, LIGHT );
				if( hour > 3 && hour < 4 && !ampm )
					mSock.sysmessage( 1215 );
			}
		}
		else
		{
			if( hour >= 5 && hour <= 6 && ampm && mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
			{
				mSock.sysmessage( 1218 );
				mChar.SetWeathDamage( BuildTimeValue( static_cast<R32>(Races->Secs( mChar.GetRace(), LIGHT ) * 2 )), LIGHT );
			}
		}
	}
	return didDamage;
}

bool doWeatherEffect( CSocket& mSock, CChar& mChar, WeatherType element )
{
	if( element == LIGHT || element == WEATHNUM )
		return false;

	if( mChar.IsNpc() || !Races->Affect( mChar.GetRace(), element ) )
		return false;

	bool didDamage			= false;
	CTownRegion *charRegion	= mChar.GetRegion();
	const UI08 weatherSys	= charRegion->GetWeather();
	if( weatherSys != 0xFF )
	{
		const R32 tempCurrent	= Weather->Temp( weatherSys );
		const R32 tempMax		= Weather->MaxTemp( weatherSys );
		const R32 tempMin		= Weather->MinTemp( weatherSys );
		if( tempCurrent > ( tempMax - tempMin ) / 4 * 3 )	// 3/4 of the temp is done
		{
			if( mChar.GetWeathDamage( element ) != 0 && mChar.GetWeathDamage( element ) <= cwmWorldState->GetUICurrentTime() )
			{
				const R32 damageModifier = ( tempMax - tempCurrent ) / 5;
				mChar.SetHP((SI16)( mChar.GetHP() - ( (R32)Races->Damage( mChar.GetRace(), element ) * damageModifier )) );
				mChar.SetStamina( mChar.GetStamina() - 2 );
				mSock.sysmessage( (1218 + (int)element) );
				Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
				Effects->PlaySound( (&mChar), 0x0208 );     
				didDamage = true;
			}
			mChar.SetWeathDamage( BuildTimeValue( Races->Secs( mChar.GetRace(), element ) ), element );
		}
	}
	return didDamage;
}

}
