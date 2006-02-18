#include "uox3.h"
#include "cWeather.hpp"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "scriptc.h"
#include "mapstuff.h"
#include "cScript.h"
#include "CJSMapping.h"

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
	StormChance( 5 );
	LightMin( 300 );
	LightMax( 300 );
	CurrentLight( 300 );

	MaxTemp( 30 );
	MinTemp( 5 );
	ColdIntensity( 0 );
	HeatIntensity( 35 );
	StormIntensity( 100 );
	SnowIntensity( 100 );
	RainIntensity( 100 );
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

	if( StormDelay() && StormBrewing() )
		StormActive( true );
	else if( StormBrewing() )
		StormDelay( true );

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
	
	if( StormActive() )
	{
		currTemp -= 10.0f;
		if ( currTemp < 0 )
			currTemp = 0;
	}
	else if( RainActive() )
	{
		currTemp -= 5.0f;
		if ( currTemp < 0 )
			currTemp = 0;
	}
	Temp( currTemp );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void NewDay( void )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the system up for a new day
//|					 Heat wave, cold snap are calculated
//o---------------------------------------------------------------------------o
void CWeather::NewDay( void )
{
	bool isHeatWave = false, isColdDay = false;

	R32	effTempMax		= MaxTemp();
	R32	effTempMin		= MinTemp();
	R32 currentTemp;

	if( (UI08)RandomNum( 0, 100 ) <= HeatChance() )
	{
		isHeatWave = true;
		currentTemp = RandomNum( (int)effTempMax, (int)HeatIntensity() );
		effTempMax = currentTemp;
		effTempMin = currentTemp;
	}
	else if( (UI08)RandomNum( 0, 100 ) <= ColdChance() )
	{
		isColdDay = true;
		currentTemp = RandomNum( (int)ColdIntensity(), (int)effTempMin );
		effTempMax = currentTemp;
		effTempMin = currentTemp;
	}
	HeatActive( isHeatWave );
	ColdActive( isColdDay );

	EffectiveMaxTemp( effTempMax );
	EffectiveMinTemp( effTempMin );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void NewHour( void )
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Calculates rain, snow and storm changes.
//o---------------------------------------------------------------------------o
void CWeather::NewHour( void )
{
	bool isSnowing = false, isRaining = false, isStorm = false;
	if( (UI08)RandomNum( 0, 100 ) <= StormChance() )
	{
		isStorm = true;
	}
	if( (UI08)RandomNum( 0, 100 ) <= SnowChance() )
	{
		isSnowing = true;
	}
	if( (UI08)RandomNum( 0, 100 ) <= RainChance() )
	{
		isRaining = true;
	}
	if( !isStorm ) {
		StormDelay( false );
		StormActive( false );
	}

	SnowActive( isSnowing );
	RainActive( isRaining );
	StormBrewing( isStorm );
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
//|   Function    -  SI08 StormIntensity( void ) const
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the storm, if any
//o---------------------------------------------------------------------------o
SI08 CWeather::StormIntensity( void ) const
{
	return weather[STORM].Intensity;
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
//|   Function    -  SI08 StormChance( void ) const
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of storm for the day
//o---------------------------------------------------------------------------o
SI08 CWeather::StormChance( void ) const
{
	return weather[STORM].Chance;
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
//|   Function    -  bool StormBrewing( void ) const
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system is brewing a storm
//o---------------------------------------------------------------------------o
bool CWeather::StormBrewing( void ) const
{
	return weather[STORMBREW].Active;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool StormActive( void ) const
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the weather system is storming
//o---------------------------------------------------------------------------o
bool CWeather::StormActive( void ) const
{
	return weather[STORM].Active;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool StormDelay( void ) const
//|   Date        -  17th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the storm delay is over
//o---------------------------------------------------------------------------o
bool CWeather::StormDelay( void ) const
{
	return stormDelay;
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
//|   Function    -  void StormIntensity( SI08 value )
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the storm intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::StormIntensity( SI08 value )
{
	weather[STORM].Intensity = value;
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
//|   Function    -  void StormChance( SI08 value )
//|   Date        -  16th Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of a storm brewing in the system
//o---------------------------------------------------------------------------o
void CWeather::StormChance( SI08 value )
{
	weather[STORM].Chance = value;
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
//|   Function    -  void StormBrewing( bool value )
//|   Date        -  16th Feb, 2006
//|   Programmer  -  Grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether a storm is brewing or not
//o---------------------------------------------------------------------------o
void CWeather::StormBrewing( bool value )
{
	weather[STORMBREW].Active = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void StormActive( bool value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether a storm is active or not
//o---------------------------------------------------------------------------o
void CWeather::StormActive( bool value )
{
	weather[STORM].Active = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void StormDelay( bool value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the storm delay
//o---------------------------------------------------------------------------o
void CWeather::StormDelay( bool value )
{
	stormDelay = value;
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
	weather.resize( FileLookup->CountOfEntries( weathab_def ) );
	UString tag, data, UTag;
	UString entryName;
	size_t i = 0;
	for( Script *weathScp = FileLookup->FirstScript( weathab_def ); !FileLookup->FinishedScripts( weathab_def ); weathScp = FileLookup->NextScript( weathab_def ) )
	{
		if( weathScp == NULL )
			continue;

		for( ScriptSection *WeatherStuff = weathScp->FirstEntry(); WeatherStuff != NULL; WeatherStuff = weathScp->NextEntry() )
		{
			if( WeatherStuff == NULL )
				continue;

			entryName			= weathScp->EntryName();
			i					= entryName.section( " ", 1, 1 ).toULong();
			if( i >= weather.size() )
				weather.resize( i+1 );

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
						else if( UTag == "STORMCHANCE" )	// chance of a storm
							StormChance( static_cast<weathID>(i), data.toByte() );
						else if( UTag == "STORMINTENSITY" )	// chance of a storm
							StormIntensity( static_cast<weathID>(i), data.toByte() );
						break;
				}
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
//|   Function    -  SI08 StormIntensity( weathID toCheck )
//|   Date        -  16th Feb, 2006
//|   Programmer  -  grimson
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's storm
//o---------------------------------------------------------------------------o
SI08 cWeatherAb::StormIntensity( weathID toCheck )
{
	return Intensity( toCheck, STORM );
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
void cWeatherAb::StormIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, STORM, value );
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
void cWeatherAb::StormChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, STORM, value );
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
SI08 cWeatherAb::StormChance( weathID toCheck )
{
	return Chance( toCheck, STORM );
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

bool cWeatherAb::NewHour( void )
{
	std::vector< CWeather >::iterator wIter;
	for( wIter = weather.begin(); wIter != weather.end(); ++wIter )
		(*wIter).NewHour();
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
void cWeatherAb::HeatActive( weathID toCheck, bool value )
{
	weather[toCheck].HeatActive( value );
}
void cWeatherAb::ColdActive( weathID toCheck, bool value )
{
	weather[toCheck].ColdActive( value );
}
void cWeatherAb::RainActive( weathID toCheck, bool value )
{
	weather[toCheck].RainActive( value );
}
void cWeatherAb::StormBrewing( weathID toCheck, bool value )
{
	weather[toCheck].StormBrewing( value );
}
void cWeatherAb::StormActive( weathID toCheck, bool value )
{
	weather[toCheck].StormActive( value );
}

bool cWeatherAb::HeatActive( weathID toCheck )
{
	return weather[toCheck].HeatActive();
}
bool cWeatherAb::ColdActive( weathID toCheck )
{
	return weather[toCheck].ColdActive();
}
bool cWeatherAb::RainActive( weathID toCheck )
{
	return weather[toCheck].RainActive();
}
bool cWeatherAb::SnowActive( weathID toCheck )
{
	return weather[toCheck].SnowActive();
}
bool cWeatherAb::StormBrewing( weathID toCheck )
{
	return weather[toCheck].StormBrewing();
}
bool cWeatherAb::StormActive( weathID toCheck )
{
	return weather[toCheck].StormActive();
}

bool cWeatherAb::DoPlayerStuff( CSocket *s, CChar *p )
{
	if( !ValidateObject( p ) || p->IsNpc() )
		return true;
	weathID currval = p->GetRegion()->GetWeather();
	if( currval > weather.size() || weather.empty() || p->inBuilding() )
	{
		if( s != NULL )
		{
			CPWeather dry( 0xFF, 0x00, 0 );
			s->Send( &dry );
			if( p->GetWeathDamage( SNOW ) != 0 )
				p->SetWeathDamage( 0, SNOW );
			if( p->GetWeathDamage( STORM ) != 0 )
				p->SetWeathDamage( 0, STORM );
			if( p->GetWeathDamage( RAIN ) != 0 )
				p->SetWeathDamage( 0, RAIN );
			if( p->GetWeathDamage( COLD ) != 0 )
				p->SetWeathDamage( 0, COLD );
			if( p->GetWeathDamage( HEAT ) != 0 )
				p->SetWeathDamage( 0, HEAT );
			SendJSWeather( p, LIGHT, 0 );
		}
		return false;
	}

	bool isStorm   = StormActive( currval );
	bool brewStorm = StormBrewing( currval );
	bool isSnowing = SnowActive( currval );
	bool isRaining = RainActive( currval );
	SI08 temp	   = (SI08)Temp( currval );

	if( isStorm )
	{
		DoPlayerWeather( s, 5, temp );
		if( p->GetWeathDamage( STORM ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), STORM )) )), STORM );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
		SendJSWeather( p, STORM, temp );
	}
	else if( brewStorm )
	{
		DoPlayerWeather( s, 4, temp );
	} 
	else if( isSnowing && SnowThreshold( currval ) > Temp( currval ) )
	{
		DoPlayerWeather( s, 2, temp );
		if( p->GetWeathDamage( SNOW ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), SNOW )) )), SNOW );
		if( p->GetWeathDamage( STORM ) != 0 )
			p->SetWeathDamage( 0, STORM );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
		SendJSWeather( p, STORM, temp );
	} 
	else if( isRaining )
	{
		DoPlayerWeather( s, 1, temp );
		if( p->GetWeathDamage( RAIN ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), RAIN )) )), RAIN );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( STORM ) != 0 )
			p->SetWeathDamage( 0, STORM );
		SendJSWeather( p, STORM, temp );
	}
	else
	{
		DoPlayerWeather( s, 0, temp );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( STORM ) != 0 )
			p->SetWeathDamage( 0, STORM );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
		SendJSWeather( p, STORM, temp );
	}

	if( (Races->Affect( p->GetRace(), HEAT )) && p->GetWeathDamage( HEAT ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), HEAT )) )), HEAT );

	if( (Races->Affect( p->GetRace(), COLD )) && p->GetWeathDamage( COLD ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), COLD )) )), COLD );

	return true;
}

bool cWeatherAb::DoNPCStuff( CChar *p )
{
	if( !ValidateObject( p ) )
		return true;
	weathID currval = p->GetRegion()->GetWeather();
	if( currval > weather.size() || weather.empty() || p->inBuilding() )
	{
		SendJSWeather( p, LIGHT, 0 );
		return false;
	}

	bool isStorm   = StormActive( currval );
	bool isSnowing = SnowActive( currval );
	bool isRaining = RainActive( currval );
	SI08 temp = (SI08)Temp( currval );

	if( isStorm )
	{
		SendJSWeather( p, STORM, temp );
		if( p->GetWeathDamage( STORM ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), STORM )) )), STORM );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
	}
	else if( isSnowing && SnowThreshold( currval ) > Temp( currval ) )
	{
		SendJSWeather( p, SNOW, temp );
		if( p->GetWeathDamage( SNOW ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), SNOW )) )), SNOW );
		if( p->GetWeathDamage( STORM ) != 0 )
			p->SetWeathDamage( 0, STORM );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
	} 
	else if( isRaining )
	{
		SendJSWeather( p, RAIN, temp );
		if( p->GetWeathDamage( RAIN ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), RAIN )) )), RAIN );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( STORM ) != 0 )
			p->SetWeathDamage( 0, STORM );
	}
	else
	{
		SendJSWeather( p, LIGHT, temp );
		if( p->GetWeathDamage( SNOW ) != 0 )
			p->SetWeathDamage( 0, SNOW );
		if( p->GetWeathDamage( STORM ) != 0 )
			p->SetWeathDamage( 0, STORM );
		if( p->GetWeathDamage( RAIN ) != 0 )
			p->SetWeathDamage( 0, RAIN );
	}

	if( (Races->Affect( p->GetRace(), HEAT )) && p->GetWeathDamage( HEAT ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), HEAT )) )), HEAT );

	if( (Races->Affect( p->GetRace(), COLD )) && p->GetWeathDamage( COLD ) == 0 )
			p->SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( p->GetRace(), COLD )) )), COLD );

	return true;
}

void cWeatherAb::SendJSWeather( CChar *mChar, WeatherType weathType, SI08 currentTemp )
{
	cScript *onWeatherChangeScp = JSMapping->GetScript( mChar->GetScriptTrigger() );
	if( onWeatherChangeScp != NULL )
		onWeatherChangeScp->OnWeatherChange( mChar, weathType );
	else 
	{
		onWeatherChangeScp = JSMapping->GetScript( (UI16)0 );
		
		if( onWeatherChangeScp != NULL )
			onWeatherChangeScp->OnWeatherChange( mChar, weathType );
	}

	cScript *onTempChangeScp = JSMapping->GetScript( mChar->GetScriptTrigger() );
	if( onTempChangeScp != NULL )
		onTempChangeScp->OnTempChange( mChar, currentTemp );
	else 
	{
		onTempChangeScp = JSMapping->GetScript( (UI16)0 );
		
		if( onTempChangeScp != NULL )
			onTempChangeScp->OnTempChange( mChar, currentTemp );
	}
}

void cWeatherAb::DoPlayerWeather( CSocket *s, UI08 weathType, SI08 currentTemp )
// Weather Types 
// 0 - dry
// 1 - rain
// 2 - snow
// 3 - rain and snow
// 4 - storm is brewing
// 5 - storm
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
		case 4:
			Effects->PlaySound( mChar, 0x14 + RandomNum( 0, 1 ) );
			s->Send( &strmbrw );
			break;
		case 5:
			Effects->PlaySound( mChar, 0x14 + RandomNum( 0, 1 ) );
			s->Send( &storm );
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
void cWeatherAb::CurrentLight( weathID toCheck, R32 newValue )
{
	weather[toCheck].CurrentLight( newValue );
}

R32 cWeatherAb::LightMin( weathID toCheck )
{
	return weather[toCheck].LightMin();
}
R32 cWeatherAb::LightMax( weathID toCheck )
{
	return weather[toCheck].LightMax();
}
R32 cWeatherAb::CurrentLight( weathID toCheck )
{
	return weather[toCheck].CurrentLight();
}

CWeather *cWeatherAb::Weather( weathID toCheck )
{
	if( toCheck >= weather.size() )
		return NULL;
	else
		return &weather[toCheck];
}

bool cWeatherAb::doLightEffect( CSocket *mSock, CChar& mChar )
{
	bool didDamage = false;

	if( mChar.IsInvulnerable() || !Races->Affect( mChar.GetRace(), LIGHT ) || mChar.inBuilding() )
		return false;
	
	if( mChar.GetWeathDamage( LIGHT ) != 0 && mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
	{
		R32 damageModifier		= 0;
		SI32 damage				= 0;
		R32 baseDamage			= (R32)Races->Damage( mChar.GetRace(), LIGHT );
		R32 lightLevel			= (R32)Races->LightLevel( mChar.GetRace() );
		R32 currentLight		= 255;
		R32 dungeonLight		= 255;
		R32 lightMin			= 255;
		R32 lightMax			= 255;
		int	message				= 0;
		bool ampm				= cwmWorldState->ServerData()->ServerTimeAMPM();

		weathID weatherSys = mChar.GetRegion()->GetWeather();
		if( weatherSys > weather.size() || weather.empty() )
		{
			lightMin = LightMin( weatherSys );
			lightMax = LightMax( weatherSys );

			if( lightMin < 300 && lightMax < 300 )
			{
				currentLight = CurrentLight( weatherSys );
			}
			else
			{
				currentLight = cwmWorldState->ServerData()->WorldLightCurrentLevel();
				lightMin = cwmWorldState->ServerData()->WorldLightDarkLevel();
				lightMax = cwmWorldState->ServerData()->WorldLightBrightLevel();
			}
		}
		else
		{
			currentLight = cwmWorldState->ServerData()->WorldLightCurrentLevel();
			lightMin = cwmWorldState->ServerData()->WorldLightDarkLevel();
			lightMax = cwmWorldState->ServerData()->WorldLightBrightLevel();
		}
		
		if ( mChar.inDungeon() )
		{
			dungeonLight = cwmWorldState->ServerData()->DungeonLightLevel();


			if (lightLevel > dungeonLight )
			{
				if( lightLevel > 0)
					damageModifier = ( dungeonLight / lightLevel );
				else
					damageModifier = 0;

				damage = (SI32)roundNumber( baseDamage - ( baseDamage * damageModifier));
			
				if( roundNumber( damageModifier ) > 0 )
					message = 1216;
				else
					message = 1217;
			}
			else if( lightLevel == currentLight )
			{	
				if( ampm )
					message = 1218;
				else
					message = 1215;
			}
		}
		else
		{
			if (lightLevel > currentLight )
			{
				if( lightLevel > 0)
					damageModifier = ( currentLight / lightLevel );
				else
					damageModifier = 0;

				damage = (SI32)roundNumber( baseDamage - ( baseDamage * damageModifier));
			
				if( roundNumber( damageModifier ) > 0 )
					message = 1216;
				else
					message = 1217;
			}
			else if( lightLevel == currentLight )
			{	
				if( ampm )
					message = 1218;
				else
					message = 1215;
			}
		}
		
		if( message != 0 && mSock != NULL)
			mSock->sysmessage( message );

		if( damage > 0 )
		{
				mChar.Damage( damage );
				Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
				Effects->PlaySound( (&mChar), 0x0208 );     
				didDamage = true;
		}
		mChar.SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( mChar.GetRace(), LIGHT )) )), LIGHT );
	}
	return didDamage;
}

bool cWeatherAb::doWeatherEffect( CSocket *mSock, CChar& mChar, WeatherType element )
{
	if( element == LIGHT || element == WEATHNUM )
		return false;
	
	if( mChar.IsInvulnerable() || !Races->Affect( mChar.GetRace(), element ))
		return false;

	bool didDamage			= false;
	weathID weatherSys = mChar.GetRegion()->GetWeather();
	if( !(weatherSys > weather.size() || weather.empty()) && mChar.GetWeathDamage( element ) != 0 && mChar.GetWeathDamage( element ) <= cwmWorldState->GetUICurrentTime())
	{
		const R32 tempCurrent	= Temp( weatherSys );
		const R32 tempMax		= MaxTemp( weatherSys );
		const R32 tempMin		= MinTemp( weatherSys );
		const R32 tempSnowMax	= SnowThreshold( weatherSys );
		const R32 tempEffMax	= EffectiveMaxTemp( weatherSys );
		const R32 tempEffMin	= EffectiveMinTemp( weatherSys );


		R32 damageModifier		= 0;
		SI32 damage				= 0;
		R32 baseDamage			= (R32)Races->Damage( mChar.GetRace(), element );
		R32 heatLevel			= (R32)Races->HeatLevel( mChar.GetRace() );
		R32 coldLevel			= (R32)Races->ColdLevel( mChar.GetRace() );

		int	damageMessage		= 0;
		UI16 damageAnim			= 0x373A;

		if( element == RAIN )
		{
			damageModifier = (R32)RandomNum( 0, (int)RainIntensity( weatherSys ) );
			damage = (SI32)roundNumber( ( baseDamage / 100 ) * damageModifier );
			damageMessage = 1219;
		}

		if( element == SNOW )
		{
			damageModifier = (R32)RandomNum( 0, (int)SnowIntensity( weatherSys ) );
			damage = (SI32)roundNumber( ( baseDamage / 100 ) * damageModifier );
			damageMessage = 1220;
		}

		if( element == STORM)
		{
			damageModifier = (R32)RandomNum( 0, (int)StormIntensity( weatherSys ) );
			damage = (SI32)roundNumber( ( baseDamage / 100 ) * damageModifier );
			damageMessage = 1775;

			if( Races->Affect( mChar.GetRace(), LIGHTNING ) )
			{
				if( (UI08)RandomNum( 0, 100 ) <= Races->Secs( mChar.GetRace(), LIGHTNING ) )
				{
					damage = (SI32)Races->Damage( mChar.GetRace(), LIGHTNING );
					Effects->bolteffect( &mChar );
					damageMessage = 1777;
					damageAnim = 0x0;
				}
			}
		}

		if( element == COLD && tempCurrent <= coldLevel )
		{
			if( (coldLevel - tempEffMin) != 0)
				damageModifier = ( (tempCurrent - tempEffMin) / (coldLevel - tempEffMin) );
			else 
				damageModifier = 0;

			damage = (SI32)roundNumber( baseDamage - ( baseDamage * damageModifier));
			damageMessage = 1776;
		}

		if( element == HEAT && tempCurrent >= heatLevel)
		{
			if( (tempEffMax - heatLevel) != 0)
				damageModifier = ( (tempCurrent - heatLevel) / (tempEffMax - heatLevel) );
			else
				damageModifier = 0;
			
			damage = (SI32)roundNumber( baseDamage * damageModifier);
			damageMessage = 1221;
			damageAnim = 0x3709;
		}

		if( damage > 0 )
		{
				mChar.Damage( damage );
				mChar.SetStamina( mChar.GetStamina() - 2 );
				if( mSock != NULL )
					mSock->sysmessage( damageMessage );
				if( damageAnim != 0x0)
					Effects->PlayStaticAnimation( (&mChar), damageAnim, 0x09, 0x19 );
				Effects->PlaySound( (&mChar), 0x0208 );     
				didDamage = true;
		}
		mChar.SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( mChar.GetRace(), element )) )), element );
	}
	return didDamage;
}

}
