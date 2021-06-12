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
#include "combat.h"


#define EFFECTIVE 2
const UI08 MAXVAL	= 0;
const UI08 MINVAL	= 1;
const UI08 CURRVAL	= 2;
const UI08 TEMP		= 0;
const UI08 WIND		= 1;

cWeatherAb *Weather = nullptr;
// Version History
// 1.0		 		Unknown
//			Initial implementation
//			Based on a weatherSys structure system
// 2.0		 		11th April, 2001
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
	StormChance( 1 );
	LightMin( 300 );
	LightMax( 300 );
	CurrentLight( 300 );

	MaxTemp( 30 );
	MinTemp( 5 );
	RainTempDrop( 5 );
	StormTempDrop( 10 );
	ColdIntensityHigh( 0 );
	HeatIntensityHigh( 35 );
	StormIntensityHigh( 100 );
	SnowIntensityHigh( 100 );
	RainIntensityHigh( 100 );
	StormIntensityLow( 0 );
	SnowIntensityLow( 0 );
	RainIntensityLow( 0 );
	StormIntensity( 0 );
	SnowIntensity( 0 );
	RainIntensity( 0 );
	Temp( 15 );
	MaxWindSpeed( 0 );
	MinWindSpeed( 0 );
	WindSpeed( 0 );
	SnowThreshold( 15.0f );
}

CWeather::~CWeather()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HeatActive( void ) const
//|					void HeatActive( bool value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is having a heat wave
//o-----------------------------------------------------------------------------------------------o
bool CWeather::HeatActive( void ) const
{
	return weather[HEAT].Active;
}
void CWeather::HeatActive( bool value )
{
	weather[HEAT].Active = value;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ColdActive( void ) const
//|					void ColdActive( bool value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/sets whether the weather system is having a cold snap
//o-----------------------------------------------------------------------------------------------o
bool CWeather::ColdActive( void ) const
{
	return weather[COLD].Active;
}
void CWeather::ColdActive( bool value )
{
	weather[COLD].Active = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 CurrentLight( void ) const
//|					void CurrentLight( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current light level of the system, IF
//|					 LightMin and LightMax are less than 300
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::CurrentLight( void ) const
{
	return light[CURRVAL];
}
void CWeather::CurrentLight( R32 value )
{
	light[CURRVAL] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool PeriodicUpdate( void )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the current light level of the system and the current temperature of
//|					 the system. Wind is not currently updated
//o-----------------------------------------------------------------------------------------------o
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
		currTemp -= StormTempDrop();
		if ( currTemp < 0 )
			currTemp = 0;
	}
	else if( RainActive() )
	{
		currTemp -= RainTempDrop();
		if ( currTemp < 0 )
			currTemp = 0;
	}
	Temp( currTemp );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void NewDay( void )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the system up for a new day. Heat wave, cold snap are calculated
//o-----------------------------------------------------------------------------------------------o
void CWeather::NewDay( void )
{
	bool isHeatWave = false, isColdDay = false;

	R32	effTempMax		= MaxTemp();
	R32	effTempMin		= MinTemp();
	R32 currentTemp;

	if( (UI08)RandomNum( 0, 100 ) <= HeatChance() )
	{
		isHeatWave = true;
		currentTemp = RandomNum( (SI32)effTempMax, (SI32)HeatIntensityHigh() );
		effTempMax = currentTemp;
		effTempMin = currentTemp;
	}
	else if( (UI08)RandomNum( 0, 100 ) <= ColdChance() )
	{
		isColdDay = true;
		currentTemp = RandomNum( (SI32)ColdIntensityHigh(), (SI32)effTempMin );
		effTempMax = currentTemp;
		effTempMin = currentTemp;
	}
	HeatActive( isHeatWave );
	ColdActive( isColdDay );

	EffectiveMaxTemp( effTempMax );
	EffectiveMinTemp( effTempMin );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void NewHour( void )
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates rain, snow and storm changes.
//o-----------------------------------------------------------------------------------------------o
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

	//Calculate intensity values for the weather types
	RainIntensity( (SI08)RandomNum( static_cast<std::int16_t>(RainIntensityLow()), static_cast<std::int16_t>(RainIntensityHigh()) ) );
	SnowIntensity( (SI08)RandomNum( static_cast<std::int16_t>(SnowIntensityLow()), static_cast<std::int16_t>(SnowIntensityHigh())) );
	StormIntensity( (SI08)RandomNum( static_cast<std::int16_t>(StormIntensityLow()), static_cast<std::int16_t>(StormIntensityHigh()) ) );

	SnowActive( isSnowing );
	RainActive( isRaining );
	StormBrewing( isStorm );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 IntensityHigh( UI08 weathType ) const
//|					SI08 IntensityLow( UI08 weathType ) const
//|					SI08 Intensity( UI08 weathType ) const
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of the weather weathType
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::IntensityHigh( UI08 weathType ) const
{
	return weather[weathType].IntensityHigh;
}
SI08 CWeather::IntensityLow( UI08 weathType ) const
{
	return weather[weathType].IntensityLow;
}
SI08 CWeather::Intensity( UI08 weathType ) const
{
	return weather[weathType].Intensity;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IntensityHigh( UI08 weathType, SI08 value )
//|					void IntensityLow( UI08 weathType, SI08 value )
//|					void Intensity( UI08 weathType, SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the intensity of weather weathType to value
//o-----------------------------------------------------------------------------------------------o
void CWeather::IntensityHigh( UI08 weathType, SI08 value )
{
	weather[weathType].IntensityHigh = value;
}
void CWeather::IntensityLow( UI08 weathType, SI08 value )
{
	weather[weathType].IntensityLow = value;
}
void CWeather::Intensity( UI08 weathType, SI08 value )
{
	weather[weathType].Intensity = value;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 Chance( UI08 weathType ) const
//|					void Chance( UI08 weathType, SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of weather weathType occurring
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::Chance( UI08 weathType ) const
{
	return weather[weathType].Chance;
}
void CWeather::Chance( UI08 weathType, SI08 value )
{
	weather[weathType].Chance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 Value( UI08 valType, UI08 valOff ) const
//|					void Value( UI08 valType, UI08 valOff, R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the value associated with valType and valOff
//|					 valType is either MAXVAL, MINVAL, or CURRVAL
//|					 valOff is either WIND, TEMP or EFFECTIVE
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::Value( UI08 valType, UI08 valOff ) const
{
	return assortVals[valType][valOff];
}
void CWeather::Value( UI08 valType, UI08 valOff, R32 value )
{
	assortVals[valType][valOff] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 SnowIntensityHigh( void ) const
//|					SI08 SnowIntensityLow( void ) const
//|					SI08 SnowIntensity( void ) const
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of the snow (if any)
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::SnowIntensityHigh( void ) const
{
	return weather[SNOW].IntensityHigh;
}
SI08 CWeather::SnowIntensityLow( void ) const
{
	return weather[SNOW].IntensityLow;
}
SI08 CWeather::SnowIntensity( void ) const
{
	return weather[SNOW].Intensity;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 RainIntensityHigh( void ) const
//|					SI08 RainIntensityLow( void ) const
//|					SI08 RainIntensity( void ) const
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of the rain, if any
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::RainIntensityHigh( void ) const
{
	return weather[RAIN].IntensityHigh;
}
SI08 CWeather::RainIntensityLow( void ) const
{
	return weather[RAIN].IntensityLow;
}
SI08 CWeather::RainIntensity( void ) const
{
	return weather[RAIN].Intensity;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 StormIntensityHigh( void ) const
//|					SI08 StormIntensityLow( void ) const
//|					SI08 StormIntensity( void ) const
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of the storm, if any
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::StormIntensityHigh( void ) const
{
	return weather[STORM].IntensityHigh;
}
SI08 CWeather::StormIntensityLow( void ) const
{
	return weather[STORM].IntensityLow;
}
SI08 CWeather::StormIntensity( void ) const
{
	return weather[STORM].Intensity;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 HeatIntensityHigh( void ) const
//|					void HeatIntensityHigh( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the heat intensity of the weather system
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::HeatIntensityHigh( void ) const
{
	return weather[HEAT].IntensityHigh;
}
void CWeather::HeatIntensityHigh( SI08 value )
{
	weather[HEAT].IntensityHigh = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 ColdIntensityHigh( void ) const
//|					void ColdIntensityHigh( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the cold intensity of the weather system
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::ColdIntensityHigh( void ) const
{
	return weather[COLD].IntensityHigh;
}
void CWeather::ColdIntensityHigh( SI08 value )
{
	weather[COLD].IntensityHigh = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MaxTemp( void ) const
//|					void MaxTemp( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum temperature of the day (non effective)
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::MaxTemp( void ) const
{
	return Value( MAXVAL, TEMP );
}
void CWeather::MaxTemp( R32 value )
{
	Value( MAXVAL, TEMP, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MinTemp( void ) const
//|					void MinTemp( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum temperature of the day (non effective)
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::MinTemp( void ) const
{
	return Value( MINVAL, TEMP );
}
void CWeather::MinTemp( R32 value )
{
	Value( MINVAL, TEMP, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 Temp( void ) const
//|					void Temp( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the current temperature of the day
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::Temp( void ) const
{
	return Value( CURRVAL, TEMP );
}
void CWeather::Temp( R32 value )
{
	Value( CURRVAL, TEMP, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 RainTempDrop( void ) const
//|					void RainTempDrop( R32 value )
//|	Date		-	19th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount the temperature drops when it rains
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::RainTempDrop( void ) const
{
	return rainTempDrop;
}
void CWeather::RainTempDrop( R32 value )
{
	rainTempDrop = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 StormTempDrop( void ) const
//|					void StormTempDrop( R32 value )
//|	Date		-	19th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount the temperature drops when it storms
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::StormTempDrop( void ) const
{
	return stormTempDrop;
}
void CWeather::StormTempDrop( R32 value )
{
	stormTempDrop = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MaxWindSpeed( void ) const
//|					void MaxWindSpeed( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum wind speed for the day
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::MaxWindSpeed( void ) const
{
	return Value( MAXVAL, WIND );
}
void CWeather::MaxWindSpeed( R32 value )
{
	Value( MAXVAL, WIND, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MinWindSpeed( void ) const
//|					void MinWindSpeed( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum wind speed for the day
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::MinWindSpeed( void ) const
{
	return Value( MINVAL, WIND );
}
void CWeather::MinWindSpeed( R32 value )
{
	Value( MINVAL, WIND, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 WindSpeed( void ) const
//|					void WindSpeed( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the current wind speed
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::WindSpeed( void ) const
{
	return Value( CURRVAL, WIND );
}
void CWeather::WindSpeed( R32 value )
{
	Value( CURRVAL, WIND, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 RainChance( void ) const
//|					void RainChance( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of rain for the day
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::RainChance( void ) const
{
	return weather[RAIN].Chance;
}
void CWeather::RainChance( SI08 value )
{
	weather[RAIN].Chance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 SnowChance( void ) const
//|					void SnowChance( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of snow for the day
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::SnowChance( void ) const
{
	return weather[SNOW].Chance;
}
void CWeather::SnowChance( SI08 value )
{
	weather[SNOW].Chance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 StormChance( void ) const
//|					void StormChance( SI08 value )
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of storm for the day
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::StormChance( void ) const
{
	return weather[STORM].Chance;
}
void CWeather::StormChance( SI08 value )
{
	weather[STORM].Chance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 HeatChance( void ) const
//|					void HeatChance( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of a heat wave for the day
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::HeatChance( void ) const
{
	return weather[HEAT].Chance;
}
void CWeather::HeatChance( SI08 value )
{
	weather[HEAT].Chance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 ColdChance( void ) const
//|					void ColdChance( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the chance of a cold snap for the day
//o-----------------------------------------------------------------------------------------------o
SI08 CWeather::ColdChance( void ) const
{
	return weather[COLD].Chance;
}
void CWeather::ColdChance( SI08 value )
{
	weather[COLD].Chance = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 LightMin( void ) const
//|					void LightMin( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum light level for the day (brightest)
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::LightMin( void ) const
{
	return light[MINVAL];
}
void CWeather::LightMin( R32 value )
{
	light[MINVAL] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 LightMax( void ) const
//|					void LightMax( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum light level for the day (darkest)
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::LightMax( void ) const
{
	return light[MAXVAL];
}
void CWeather::LightMax( R32 value )
{
	light[MAXVAL] = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RainActive( void ) const
//|					void RainActive( bool value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is raining
//o-----------------------------------------------------------------------------------------------o
bool CWeather::RainActive( void ) const
{
	return weather[RAIN].Active;
}
void CWeather::RainActive( bool value )
{
	weather[RAIN].Active = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SnowActive( void ) const
//|					void SnowActive( bool value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is snowing
//o-----------------------------------------------------------------------------------------------o
bool CWeather::SnowActive( void ) const
{
	return weather[SNOW].Active;
}
void CWeather::SnowActive( bool value )
{
	weather[SNOW].Active = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StormBrewing( void ) const
//|					void StormBrewing( bool value )
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the weather system is brewing a storm
//o-----------------------------------------------------------------------------------------------o
bool CWeather::StormBrewing( void ) const
{
	return weather[STORMBREW].Active;
}
void CWeather::StormBrewing( bool value )
{
	weather[STORMBREW].Active = value;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StormActive( void ) const
//|					void StormActive( bool value )
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a storm is active in the weather system
//o-----------------------------------------------------------------------------------------------o
bool CWeather::StormActive( void ) const
{
	return weather[STORM].Active;
}
void CWeather::StormActive( bool value )
{
	weather[STORM].Active = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StormDelay( void ) const
//|					void StormDelay( bool value )
//|	Date		-	17th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether there is a storm delay in the weather system
//o-----------------------------------------------------------------------------------------------o
bool CWeather::StormDelay( void ) const
{
	return stormDelay;
}
void CWeather::StormDelay( bool value )
{
	stormDelay = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 SnowThreshold( void ) const
//|					void SnowThreshold( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the temperature below which snow kicks in
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::SnowThreshold( void ) const
{
	return snowThreshold;
}
void CWeather::SnowThreshold( R32 value )
{
	snowThreshold = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SnowIntensityHigh( SI08 value )
//|					void SnowIntensityLow( SI08 value )
//|					void SnowIntensity( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the snow intensity of the system
//o-----------------------------------------------------------------------------------------------o
void CWeather::SnowIntensityHigh( SI08 value )
{
	weather[SNOW].IntensityHigh = value;
}
void CWeather::SnowIntensityLow( SI08 value )
{
	weather[SNOW].IntensityLow = value;
}
void CWeather::SnowIntensity( SI08 value )
{
	weather[SNOW].Intensity = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void StormIntensityHigh( SI08 value )
//|					void StormIntensityLow( SI08 value )
//|					void StormIntensity( SI08 value )
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the storm intensity of the system
//o-----------------------------------------------------------------------------------------------o
void CWeather::StormIntensityHigh( SI08 value )
{
	weather[STORM].IntensityHigh = value;
}
void CWeather::StormIntensityLow( SI08 value )
{
	weather[STORM].IntensityLow = value;
}
void CWeather::StormIntensity( SI08 value )
{
	weather[STORM].Intensity = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RainIntensityHigh( SI08 value )
//|					void RainIntensityLow( SI08 value )
//|					void RainIntensity( SI08 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the rain intensity of the system
//o-----------------------------------------------------------------------------------------------o
void CWeather::RainIntensityHigh( SI08 value )
{
	weather[RAIN].IntensityHigh = value;
}
void CWeather::RainIntensityLow( SI08 value )
{
	weather[RAIN].IntensityLow = value;
}
void CWeather::RainIntensity( SI08 value )
{
	weather[RAIN].Intensity = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 EffectiveMaxTemp( void ) const
//|					void EffectiveMaxTemp( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective maximum temperature for the day
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::EffectiveMaxTemp( void ) const
{
	return Value( MAXVAL, EFFECTIVE );
}
void CWeather::EffectiveMaxTemp( R32 value )
{
	Value( MAXVAL, EFFECTIVE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 EffectiveMinTemp( void ) const
//|					void EffectiveMinTemp( R32 value )
//|	Date		-	11th April, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective minimum temperature for the day
//o-----------------------------------------------------------------------------------------------o
R32 CWeather::EffectiveMinTemp( void ) const
{
	return Value( MINVAL, EFFECTIVE );
}
void CWeather::EffectiveMinTemp( R32 value )
{
	Value( MINVAL, EFFECTIVE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	cWeatherAb()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Default weather constructor
//o-----------------------------------------------------------------------------------------------o
cWeatherAb::cWeatherAb()
{
	weather.resize( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~cWeatherAb()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Destroys any allocated memory
//o-----------------------------------------------------------------------------------------------o
cWeatherAb::~cWeatherAb()
{
	weather.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t Count( void ) const
//|	Date		-	31st July, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns count of weather systems
//o-----------------------------------------------------------------------------------------------o
size_t cWeatherAb::Count( void ) const
{
	return weather.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Load( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the weather system loaded okay
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::Load( void )
{
	weather.resize( FileLookup->CountOfEntries( weathab_def ) );
	std::string tag, data, UTag;
	std::string entryName;
	size_t i = 0;
	for( Script *weathScp = FileLookup->FirstScript( weathab_def ); !FileLookup->FinishedScripts( weathab_def ); weathScp = FileLookup->NextScript( weathab_def ) )
	{
		if( weathScp == nullptr )
			continue;

		for( ScriptSection *WeatherStuff = weathScp->FirstEntry(); WeatherStuff != nullptr; WeatherStuff = weathScp->NextEntry() )
		{
			if( WeatherStuff == nullptr )
			{
				continue;
			}

			entryName			= weathScp->EntryName();
			auto ssecs 			= strutil::sections( entryName, " " );
			i					= static_cast<UI32>(std::stoul(ssecs[1], nullptr, 0));
			if( i >= weather.size() )
			{
				weather.resize( i+1 );
			}

			for( tag = WeatherStuff->First(); !WeatherStuff->AtEnd(); tag = WeatherStuff->Next() )
			{
				UTag = strutil::toupper( tag );
				data = WeatherStuff->GrabData();
				switch( tag[0] )
				{
					case 'c':
					case 'C':
						if( UTag == "COLDCHANCE" ) // chance for a cold day
						{
							ColdChance( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						else if( UTag == "COLDINTENSITY" ) // cold intensity
						{
							ColdIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						break;

					case 'h':
					case 'H':
						if( UTag == "HEATCHANCE" ) // chance for a hot day
						{
							HeatChance( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						else if( UTag == "HEATINTENSITY" ) // heat intensity
						{
							HeatIntensityHigh( static_cast<weathID>(i),static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						break;

					case 'l':
					case 'L':
						if( UTag == "LIGHTMIN" ) // minimum light level
						{
							LightMin( static_cast<weathID>(i), std::stof(data) );
						}
						else if( UTag == "LIGHTMAX" ) // maximum light level
						{
							LightMax( static_cast<weathID>(i), std::stof(data));
						}
						break;

					case 'm':
					case 'M':
						if( UTag == "MAXTEMP" ) // maximum temperature
						{
							MaxTemp( static_cast<weathID>(i), std::stof(data) );
						}
						else if( UTag == "MINTEMP" ) // minimum temperature
						{
							MinTemp( static_cast<weathID>(i), std::stof(data) );
						}
						else if( UTag == "MAXWIND" ) // maximum wind speed
						{
							MaxWindSpeed( static_cast<weathID>(i), std::stof(data) );
						}
						else if( UTag == "MINWIND" ) // minimum wind speed
						{
							MinWindSpeed( static_cast<weathID>(i), std::stof(data) );
						}
						break;

					case 'r':
					case 'R':
						if( UTag == "RAINCHANCE" ) // chance of rain
						{
							RainChance( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						else if( UTag == "RAININTENSITY" ) // intensity of rain
						{
							auto csecs = strutil::sections( data, "," );
							if( csecs.size() > 1 )
							{
								RainIntensityLow( static_cast<weathID>(i), static_cast<SI08>(std::stoi(strutil::stripTrim(csecs[0]),nullptr,0)) );
								RainIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(strutil::stripTrim(csecs[1]),nullptr,0)) );
							}
							else
							{
								RainIntensityLow( static_cast<weathID>(i), 0 );
								RainIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
							}
						}
						else if( UTag == "RAINTEMPDROP" ) // temp drop of rain
						{
							RainTempDrop( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						break;
					case 's':
					case 'S':
						if( UTag == "SNOWCHANCE" ) // chance of snow
						{
							SnowChance( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						else if( UTag == "SNOWINTENSITY" ) // intensity of snow
						{
							auto csecs = strutil::sections( data, "," );
							if( csecs.size() > 1 )
							{
								SnowIntensityLow( static_cast<weathID>(i), static_cast<SI08>(std::stoi(strutil::stripTrim(csecs[0]),nullptr,0)));
								SnowIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(strutil::stripTrim(csecs[1]),nullptr,0)) );
							}
							else
							{
								SnowIntensityLow( static_cast<weathID>(i), 0 );
								SnowIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
							}
						}
						else if( UTag == "SNOWTHRESHOLD" ) // temperature at which snow kicks in
						{
							SnowThreshold( static_cast<weathID>(i), std::stof(data) );
						}
						else if( UTag == "STORMCHANCE" ) // chance of a storm
						{
							StormChance( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						else if( UTag == "STORMINTENSITY" ) // chance of a storm
						{
							auto csecs = strutil::sections( data, "," );
							if( csecs.size() > 1 )
							{
								SnowIntensityLow( static_cast<weathID>(i), static_cast<SI08>(std::stoi(strutil::stripTrim(csecs[0]),nullptr,0)) );
								SnowIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(strutil::stripTrim(csecs[1]),nullptr,0)) );
							}
							else
							{
								SnowIntensityLow( static_cast<weathID>(i), 0 );
								SnowIntensityHigh( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
							}
						}
						else if( UTag == "STORMTEMPDROP" ) // temp drop of storm
						{
							StormTempDrop( static_cast<weathID>(i), static_cast<SI08>(std::stoi(data, nullptr, 0)) );
						}
						break;
				}
			}
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 IntensityHigh( weathID toCheck, UI08 weathType )
//|					SI08 IntensityLow( weathID toCheck, UI08 weathType )
//|					SI08 Intensity( weathID toCheck, UI08 weathType )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to return intensity of a weather type
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::IntensityHigh( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].IntensityHigh( weathType );
}
SI08 cWeatherAb::IntensityLow( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].IntensityLow( weathType );
}
SI08 cWeatherAb::Intensity( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].Intensity( weathType );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IntensityHigh( weathID toCheck, UI08 weathType, SI08 value )
//|					void IntensityLow( weathID toCheck, UI08 weathType, SI08 value )
//|					void Intensity( weathID toCheck, UI08 weathType, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to set the intensity of a particular weather type
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::IntensityHigh( weathID toCheck, UI08 weathType, SI08 value )
{
	weather[toCheck].IntensityHigh( weathType, value );
}
void cWeatherAb::IntensityLow( weathID toCheck, UI08 weathType, SI08 value )
{
	weather[toCheck].IntensityLow( weathType, value );
}
void cWeatherAb::Intensity( weathID toCheck, UI08 weathType, SI08 value )
{
	weather[toCheck].Intensity( weathType, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 Chance( weathID toCheck, UI08 weathType )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to return chance of a weather type
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::Chance( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].Chance( weathType );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Chance( weathID toCheck, UI08 weathType, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to set the chance of finding a weather type
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::Chance( weathID toCheck, UI08 weathType, SI08 value )
{
	weather[toCheck].Chance( weathType, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 Value( weathID toCheck, UI08 valType, UI08 valOff )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to return value of some weather types
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::Value( weathID toCheck, UI08 valType, UI08 valOff )
// PRE:		toCheck is valid, valType is valid, valOff is valid
// POST:	returns value of valType and valOff in toCheck
{
	return weather[toCheck].Value( valType, valOff );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Value( weathID toCheck, UI08 valType, UI08 valOff, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper func to set the value of a particular weather type
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::Value( weathID toCheck, UI08 valType, UI08 valOff, R32 value )
// PRE:		toCheck is valid, valType and valOff is valid, value is valid
// POST:	value of valType and valOff in toCheck is value
{
	weather[toCheck].Value( valType, valOff, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 SnowIntensityHigh( weathID toCheck )
//|					SI08 SnowIntensityLow( weathID toCheck )
//|					SI08 SnowIntensity( weathID toCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of toCheck's snow
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::SnowIntensityHigh( weathID toCheck )
{
	return IntensityHigh( toCheck, SNOW );
}
SI08 cWeatherAb::SnowIntensityLow( weathID toCheck )
{
	return IntensityLow( toCheck, SNOW );
}
SI08 cWeatherAb::SnowIntensity( weathID toCheck )
{
	return Intensity( toCheck, SNOW );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 StormIntensityHigh( weathID toCheck )
//|					SI08 StormIntensityLow( weathID toCheck )
//|					SI08 StormIntensity( weathID toCheck )
//|	Date		-	16th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of toCheck's storm
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::StormIntensityHigh( weathID toCheck )
{
	return IntensityHigh( toCheck, STORM );
}
SI08 cWeatherAb::StormIntensityLow( weathID toCheck )
{
	return IntensityLow( toCheck, STORM );
}
SI08 cWeatherAb::StormIntensity( weathID toCheck )
{
	return Intensity( toCheck, STORM );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 RainIntensityHigh( weathID toCheck )
//|					SI08 RainIntensityLow( weathID toCheck )
//|					SI08 RainIntensity( weathID toCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the intensity of toCheck's rain
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::RainIntensityHigh( weathID toCheck )
{
	return IntensityHigh( toCheck, RAIN );
}
SI08 cWeatherAb::RainIntensityLow( weathID toCheck )
{
	return IntensityLow( toCheck, RAIN );
}
SI08 cWeatherAb::RainIntensity( weathID toCheck )
{
	return Intensity( toCheck, RAIN );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 HeatIntensityHigh( weathID toCheck )
//|					void HeatIntensityHigh( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the high intensity of toCheck's heat
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::HeatIntensityHigh( weathID toCheck )
{
	return IntensityHigh( toCheck, HEAT );
}
void cWeatherAb::HeatIntensityHigh( weathID toCheck, SI08 value )
{
	IntensityHigh( toCheck, HEAT, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 ColdIntensityHigh( weathID toCheck )
//|					void ColdIntensityHigh( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the high intensity of toCheck's cold
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::ColdIntensityHigh( weathID toCheck )
{
	return IntensityHigh( toCheck, COLD );
}
void cWeatherAb::ColdIntensityHigh( weathID toCheck, SI08 value )
{
	IntensityHigh( toCheck, COLD, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MaxTemp( weathID toCheck )
//|					void MaxTemp( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum temperature of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::MaxTemp( weathID toCheck )
{
	return Value( toCheck, MAXVAL, TEMP );
}
void cWeatherAb::MaxTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MAXVAL, TEMP, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MinTemp( weathID toCheck )
//|					void MinTemp( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum temperature of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::MinTemp( weathID toCheck )
{
	return Value( toCheck, MINVAL, TEMP );
}
void cWeatherAb::MinTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MINVAL, TEMP, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 Temp( weathID toCheck )
//|					void Temp( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the temperature of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::Temp( weathID toCheck )
{
	return Value( toCheck, CURRVAL, TEMP );
}
void cWeatherAb::Temp( weathID toCheck, R32 value )
{
	Value( toCheck, CURRVAL, TEMP, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 RainTempDrop( weathID toCheck )
//|					void RainTempDrop( weathID toCheck, R32 value )
//|	Date		-	19th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the rain temperature drop of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::RainTempDrop( weathID toCheck )
{
	return weather[toCheck].RainTempDrop();
}
void cWeatherAb::RainTempDrop( weathID toCheck, R32 value )
{
	weather[toCheck].RainTempDrop( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 RainTempDrop( weathID toCheck )
//|					void cWeatherAb::StormTempDrop( weathID toCheck, R32 value )
//|	Date		-	19th Feb, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the storm temperature drop of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::StormTempDrop( weathID toCheck )
{
	return weather[toCheck].StormTempDrop();
}
void cWeatherAb::StormTempDrop( weathID toCheck, R32 value )
{
	weather[toCheck].StormTempDrop( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MaxWindSpeed( weathID toCheck )
//|					void MaxWindSpeed( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the maximum wind speed of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::MaxWindSpeed( weathID toCheck )
{
	return Value( toCheck, MAXVAL, WIND );
}
void cWeatherAb::MaxWindSpeed( weathID toCheck, R32 value )
{
	Value( toCheck, MAXVAL, WIND, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 MinWindSpeed( weathID toCheck )
//|					void MinWindSpeed( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the minimum wind speed of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::MinWindSpeed( weathID toCheck )
{
	return Value( toCheck, MINVAL, WIND );
}
void cWeatherAb::MinWindSpeed( weathID toCheck, R32 value )
{
	Value( toCheck, MINVAL, WIND, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 WindSpeed( weathID toCheck )
//|					void WindSpeed( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the wind speed of toCheck
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::WindSpeed( weathID toCheck )
{
	return Value( toCheck, CURRVAL, WIND );
}
void cWeatherAb::WindSpeed( weathID toCheck, R32 value )
{
	Value( toCheck, CURRVAL, WIND, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SnowIntensityHigh( weathID toCheck, SI08 value )
//|					void SnowIntensityLow( weathID toCheck, SI08 value )
//|					void SnowIntensity( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the intensity of toCheck's snow
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::SnowIntensityHigh( weathID toCheck, SI08 value )
{
	IntensityHigh( toCheck, SNOW, value );
}
void cWeatherAb::SnowIntensityLow( weathID toCheck, SI08 value )
{
	IntensityLow( toCheck, SNOW, value );
}
void cWeatherAb::SnowIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, SNOW, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void StormIntensityHigh( weathID toCheck, SI08 value )
//|					void StormIntensityLow( weathID toCheck, SI08 value )
//|					void StormIntensity( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the intensity of toCheck's storm
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::StormIntensityHigh( weathID toCheck, SI08 value )
{
	IntensityHigh( toCheck, STORM, value );
}
void cWeatherAb::StormIntensityLow( weathID toCheck, SI08 value )
{
	IntensityLow( toCheck, STORM, value );
}
void cWeatherAb::StormIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, STORM, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RainIntensityHigh( weathID toCheck, SI08 value )
//|					void RainIntensityLow( weathID toCheck, SI08 value )
//|					void RainIntensity( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the intensity of toCheck's rain
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::RainIntensityHigh( weathID toCheck, SI08 value )
{
	IntensityHigh( toCheck, RAIN, value );
}
void cWeatherAb::RainIntensityLow( weathID toCheck, SI08 value )
{
	IntensityLow( toCheck, RAIN, value );
}
void cWeatherAb::RainIntensity( weathID toCheck, SI08 value )
{
	Intensity( toCheck, RAIN, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 RainChance( weathID toCheck )
//|					void RainChance( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's rain chance
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::RainChance( weathID toCheck )
{
	return Chance( toCheck, RAIN );
}
void cWeatherAb::RainChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, RAIN, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 SnowChance( weathID toCheck )
//|					void SnowChance( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's snow chance
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::SnowChance( weathID toCheck )
{
	return Chance( toCheck, SNOW );
}
void cWeatherAb::SnowChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, SNOW, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 StormChance( weathID toCheck )
//|					void StormChance( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's storm chance
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::StormChance( weathID toCheck )
{
	return Chance( toCheck, STORM );
}
void cWeatherAb::StormChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, STORM, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 HeatChance( weathID toCheck )
//|					void HeatChance( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's heat chance
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::HeatChance( weathID toCheck )
{
	return Chance( toCheck, HEAT );
}
void cWeatherAb::HeatChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, HEAT, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 ColdChance( weathID toCheck )
//|					void ColdChance( weathID toCheck, SI08 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's cold chance
//o-----------------------------------------------------------------------------------------------o
SI08 cWeatherAb::ColdChance( weathID toCheck )
{
	return Chance( toCheck, COLD );
}
void cWeatherAb::ColdChance( weathID toCheck, SI08 value )
{
	Chance( toCheck, COLD, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 SnowThreshold( weathID toCheck )
//|					void SnowThreshold( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets toCheck's snow threshold
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::SnowThreshold( weathID toCheck )
{
	return weather[toCheck].SnowThreshold();
}
void cWeatherAb::SnowThreshold( weathID toCheck, R32 value )
{
	weather[toCheck].SnowThreshold( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool NewDay( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Begins a new day in the weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::NewDay( void )
{
	std::vector< CWeather >::iterator wIter;
	for( wIter = weather.begin(); wIter != weather.end(); ++wIter )
		(*wIter).NewDay();
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool NewHour( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Begins a new hour in the weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::NewHour( void )
{
	std::vector< CWeather >::iterator wIter;
	for( wIter = weather.begin(); wIter != weather.end(); ++wIter )
		(*wIter).NewHour();
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 EffectiveMinTemp( weathID toCheck )
//|					void EffectiveMinTemp( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective min temperature of a weather system
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::EffectiveMinTemp( weathID toCheck )
{
	return Value( toCheck, MINVAL, EFFECTIVE );
}
void cWeatherAb::EffectiveMinTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MINVAL, EFFECTIVE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 EffectiveMaxTemp( weathID toCheck )
//|					void EffectiveMaxTemp( weathID toCheck, R32 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the effective max temperature of a weather system
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::EffectiveMaxTemp( weathID toCheck )
{
	return Value( toCheck, MAXVAL, EFFECTIVE );
}
void cWeatherAb::EffectiveMaxTemp( weathID toCheck, R32 value )
{
	Value( toCheck, MAXVAL, EFFECTIVE, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoStuff( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Periodically called from main loop to update weather types
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::DoStuff( void )
{
	std::vector< CWeather >::iterator wIter;
	for( wIter = weather.begin(); wIter != weather.end(); ++wIter )
		(*wIter).PeriodicUpdate();
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HeatActive( weathID toCheck )
//|					void HeatActive( weathID toCheck, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets heat in specified weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::HeatActive( weathID toCheck )
{
	return weather[toCheck].HeatActive();
}
void cWeatherAb::HeatActive( weathID toCheck, bool value )
{
	weather[toCheck].HeatActive( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ColdActive( weathID toCheck )
//|					void ColdActive( weathID toCheck, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets cold in specified weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::ColdActive( weathID toCheck )
{
	return weather[toCheck].ColdActive();
}
void cWeatherAb::ColdActive( weathID toCheck, bool value )
{
	weather[toCheck].ColdActive( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool RainActive( weathID toCheck )
//|					void RainActive( weathID toCheck, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets rain in specified weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::RainActive( weathID toCheck )
{
	return weather[toCheck].RainActive();
}
void cWeatherAb::RainActive( weathID toCheck, bool value )
{
	weather[toCheck].RainActive( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SnowActive( weathID toCheck )
//|					void SnowActive( weathID toCheck, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets snow in specified weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::SnowActive( weathID toCheck )
{
	return weather[toCheck].SnowActive();
}
void cWeatherAb::SnowActive( weathID toCheck, bool value )
{
	weather[toCheck].SnowActive( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StormBrewing( weathID toCheck )
//|					void StormBrewing( weathID toCheck, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a storm is brewing in specified weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::StormBrewing( weathID toCheck )
{
	return weather[toCheck].StormBrewing();
}
void cWeatherAb::StormBrewing( weathID toCheck, bool value )
{
	weather[toCheck].StormBrewing( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool StormActive( weathID toCheck )
//|					void StormActive( weathID toCheck, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether a storm is active in specified weather system
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::StormActive( weathID toCheck )
{
	return weather[toCheck].StormActive();
}
void cWeatherAb::StormActive( weathID toCheck, bool value )
{
	weather[toCheck].StormActive( value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoPlayerStuff( CSocket *s, CChar *p )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates weather effects for players when light levels change,
//|					or depending on whether player is inside or outside of buildings
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::DoPlayerStuff( CSocket *s, CChar *p )
{
	if( !ValidateObject( p ) || p->IsNpc() )
		return true;
	weathID currval = p->GetRegion()->GetWeather();
	if( currval > weather.size() || weather.empty() || p->inBuilding() )
	{
		if( s != nullptr )
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

	bool isStorm = StormActive( currval );
	bool brewStorm = StormBrewing( currval );
	bool isSnowing = SnowActive( currval );
	bool isRaining = RainActive( currval );
	SI08 temp	   = (SI08)Temp( currval );

	if( isStorm )
	{
		DoPlayerWeather( s, 5, temp, currval );
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
		DoPlayerWeather( s, 4, temp, currval );
	}
	else if( isSnowing && SnowThreshold( currval ) > Temp( currval ) )
	{
		DoPlayerWeather( s, 2, temp, currval );
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
		DoPlayerWeather( s, 1, temp, currval );
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
		DoPlayerWeather( s, 0, temp, currval );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoNPCStuff( CChar *p )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates weather effects for NPCs when light levels change,
//|					or depending on whether NPC is inside or outside of buildings
//o-----------------------------------------------------------------------------------------------o
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

	bool isStorm = StormActive( currval );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendJSWeather( CChar *mChar, WeatherType weathType, SI08 currentTemp )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends weather changes to JS engine and triggers relevant JS events
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::SendJSWeather( CChar *mChar, WeatherType weathType, SI08 currentTemp )
{
	// Check for events in specific scripts attached to character
	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			toExecute->OnWeatherChange( mChar, weathType );
			toExecute->OnTempChange( mChar, currentTemp );
		}
	}

	// Check global script as well
	cScript *toExecuteGlobal = JSMapping->GetScript( static_cast<UI16>(0) );
	if( toExecuteGlobal != nullptr )
	{
		toExecuteGlobal->OnWeatherChange( mChar, weathType );
		toExecuteGlobal->OnTempChange( mChar, currentTemp );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DoPlayerWeather( CSocket *s, UI08 weathType, SI08 currentTemp, weathID currval )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends weather effects to player's client
//o-----------------------------------------------------------------------------------------------o
void cWeatherAb::DoPlayerWeather( CSocket *s, UI08 weathType, SI08 currentTemp, weathID currval )
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

	if( s == nullptr )
		return;

	CPWeather dry( 0xFF, 0x00, currentTemp );
	CPWeather rain( 0x00, (UI08)RainIntensity( currval ), currentTemp );
	CPWeather storm( 0x01, (UI08)StormIntensity( currval ), currentTemp );
	CPWeather snow( 0x02, (UI08)SnowIntensity( currval ), currentTemp );
	CPWeather strmbrw( 0x03, (UI08)(StormIntensity( currval ) / 2), currentTemp );

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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 LightMin( weathID toCheck )
//|					void LightMin( weathID toCheck, R32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light (min) value of specified weather system
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::LightMin( weathID toCheck )
{
	return weather[toCheck].LightMin();
}
void cWeatherAb::LightMin( weathID toCheck, R32 newValue )
{
	weather[toCheck].LightMin( newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 LightMax( weathID toCheck )
//|					void LightMax( weathID toCheck, R32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light (max) value of specified weather system
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::LightMax( weathID toCheck )
{
	return weather[toCheck].LightMax();
}
void cWeatherAb::LightMax( weathID toCheck, R32 newValue )
{
	weather[toCheck].LightMax( newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	R32 CurrentLight( weathID toCheck )
//|					void CurrentLight( weathID toCheck, R32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light (current) value of specified weather system
//o-----------------------------------------------------------------------------------------------o
R32 cWeatherAb::CurrentLight( weathID toCheck )
{
	return weather[toCheck].CurrentLight();
}
void cWeatherAb::CurrentLight( weathID toCheck, R32 newValue )
{
	weather[toCheck].CurrentLight( newValue );
}

CWeather *cWeatherAb::Weather( weathID toCheck )
{
	if( toCheck >= weather.size() )
		return nullptr;
	else
		return &weather[toCheck];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool doLightEffect( CSocket *mSock, CChar& mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles effects of light levels on characters affected by light
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::doLightEffect( CSocket *mSock, CChar& mChar )
{
	bool didDamage = false;

	if( mChar.IsInvulnerable() || mChar.IsDead() || !Races->Affect( mChar.GetRace(), LIGHT ) || mChar.inBuilding() )
		return false;

	if( mChar.GetWeathDamage( LIGHT ) != 0 && mChar.GetWeathDamage( LIGHT ) <= cwmWorldState->GetUICurrentTime() )
	{
		R32 damageModifier		= 0;
		SI32 damage				= 0;
		R32 baseDamage			= (R32)Races->Damage( mChar.GetRace(), LIGHT );
		R32 lightLevel			= (R32)Races->LightLevel( mChar.GetRace() );
		R32 currentLight		= 255;
		R32 lightMin			= 255;
		R32 lightMax			= 255;
		SI32 message			= 0;
		bool ampm				= cwmWorldState->ServerData()->ServerTimeAMPM();

		weathID weatherSys = mChar.GetRegion()->GetWeather();
		if( !weather.empty() && weatherSys < weather.size() )
		{
			lightMin = LightMin( weatherSys );
			lightMax = LightMax( weatherSys );

			if( lightMin < 300 && lightMax < 300 )
				currentLight = CurrentLight( weatherSys );
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
			R32 dungeonLight		= 255;
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

		damage = Combat->ApplyDefenseModifiers( LIGHT, nullptr, &mChar, 0, 0, damage, true);

		if( damage > 0 )
		{
			mChar.Damage( damage );
			Effects->PlayStaticAnimation( (&mChar), 0x3709, 0x09, 0x19 );
			Effects->PlaySound( (&mChar), 0x0208 );
			didDamage = true;

			if( message != 0 && mSock != nullptr)
				mSock->sysmessage( message );
		}
		mChar.SetWeathDamage( static_cast<UI32>(BuildTimeValue( static_cast<R32>(Races->Secs( mChar.GetRace(), LIGHT )) )), LIGHT );
	}
	return didDamage;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool doWeatherEffect( CSocket *mSock, CChar& mChar, WeatherType element )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles effects of weather on characters affected by weather
//o-----------------------------------------------------------------------------------------------o
bool cWeatherAb::doWeatherEffect( CSocket *mSock, CChar& mChar, WeatherType element )
{
	WeatherType resistElement = element;

	if( element == NONE || element == LIGHT || element == WEATHNUM )
		return false;

	if( mChar.IsInvulnerable() || mChar.IsDead() || !Races->Affect( mChar.GetRace(), element ) || mChar.inBuilding())
		return false;

	bool didDamage			= false;
	weathID weatherSys = mChar.GetRegion()->GetWeather();
	if( !(weatherSys > weather.size() || weather.empty()) && mChar.GetWeathDamage( element ) != 0 && mChar.GetWeathDamage( element ) <= cwmWorldState->GetUICurrentTime())
	{
		const R32 tempCurrent	= Temp( weatherSys );
		//const R32 tempMax		= MaxTemp( weatherSys );
		//const R32 tempMin		= MinTemp( weatherSys );
		//const R32 tempSnowMax	= SnowThreshold( weatherSys );
		const R32 tempEffMax	= EffectiveMaxTemp( weatherSys );
		const R32 tempEffMin	= EffectiveMinTemp( weatherSys );


		R32 damageModifier		= 0;
		SI32 damage				= 0;
		R32 baseDamage			= (R32)Races->Damage( mChar.GetRace(), element );
		R32 heatLevel			= (R32)Races->HeatLevel( mChar.GetRace() );
		R32 coldLevel			= (R32)Races->ColdLevel( mChar.GetRace() );

		SI32 damageMessage		= 0;
		UI16 damageAnim			= 0x373A;

		if( element == RAIN )
		{
			damageModifier = (R32)RainIntensity( weatherSys );
			damage = (SI32)roundNumber( ( baseDamage / 100 ) * damageModifier );
			damageMessage = 1219;
			resistElement = NONE;
		}

		if( element == SNOW )
		{
			damageModifier = (R32)SnowIntensity( weatherSys );
			damage = (SI32)roundNumber( ( baseDamage / 100 ) * damageModifier );
			damageMessage = 1220;
			// Snow is also cold damage when it comes to resistance values
			resistElement = COLD;
		}

		if( element == STORM)
		{
			damageModifier = (R32)StormIntensity( weatherSys );
			damage = (SI32)roundNumber( ( baseDamage / 100 ) * damageModifier );
			damageMessage = 1775;
			resistElement = NONE;

			if( Races->Affect( mChar.GetRace(), LIGHTNING ) )
			{
				if( (UI08)RandomNum( 0, 100 ) <= Races->Secs( mChar.GetRace(), LIGHTNING ) )
				{
					damage = (SI32)Races->Damage( mChar.GetRace(), LIGHTNING );
					Effects->bolteffect( &mChar );
					damageMessage = 1777;
					damageAnim = 0x0;
					resistElement = LIGHTNING;
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

		damage = Combat->ApplyDefenseModifiers( resistElement, nullptr, &mChar, 0, 0, damage, true);

		if( damage > 0 )
		{
				mChar.Damage( damage );
				mChar.SetStamina( mChar.GetStamina() - 2 );
				if( mSock != nullptr )
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

