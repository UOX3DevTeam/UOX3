#include "uox3.h"
#include "ssection.h"

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

	UI08 hour = cwmWorldState->ServerData()->GetServerTimeHours();
	UI08 minute = cwmWorldState->ServerData()->GetServerTimeMinutes();
	bool ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();

	if( LightMin() < 300 && LightMax() < 300 )
	{
		R32 hourIncrement = fabs( ( LightMax() - LightMin() ) / 12.0f );	// we want the amount to subtract from LightMax in the morning / add to LightMin in evening
		R32 minuteIncrement = hourIncrement / 60.0f;
		R32 tempLight = hourIncrement * (R32)hour + minuteIncrement * (R32)minute;
		if( ampm )
			CurrentLight( LightMin() + tempLight );
		else
			CurrentLight( LightMax() - tempLight );
	}

	R32	effTempMax			= EffectiveMaxTemp();
	R32	effTempMin			= EffectiveMinTemp();
	R32	tempHourIncrement	= fabs( ( effTempMax - effTempMin ) / 12.0f );
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
//|   Function    -  char Intensity( UI08 weathType ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the weather weathType
//o---------------------------------------------------------------------------o
char CWeather::Intensity( UI08 weathType ) const
{
	return weather[weathType].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( UI08 weathType, char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the intensity of weather weathType to value
//o---------------------------------------------------------------------------o
void CWeather::Intensity( UI08 weathType, char value )
{
	weather[weathType].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char Chance( UI08 weathType ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of weather weathType occurring
//o---------------------------------------------------------------------------o
char CWeather::Chance( UI08 weathType ) const
{
	return weather[weathType].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatActive( UI08 weathType, char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of weather weathType occuring to value
//o---------------------------------------------------------------------------o
void CWeather::Chance( UI08 weathType, char value )
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
//|   Function    -  char SnowIntensity( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the snow (if any)
//o---------------------------------------------------------------------------o
char CWeather::SnowIntensity( void ) const
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
char CWeather::RainIntensity( void ) const
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
char CWeather::HeatIntensity( void ) const
{
	return weather[HEAT].Intensity;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char ColdIntensity( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of the cold snap, if any
//o---------------------------------------------------------------------------o
char CWeather::ColdIntensity( void ) const
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
//|   Function    -  char RainChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of rain for the day
//o---------------------------------------------------------------------------o
char CWeather::RainChance( void ) const
{
	return weather[RAIN].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char SnowChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of snow for the day
//o---------------------------------------------------------------------------o
char CWeather::SnowChance( void ) const
{
	return weather[SNOW].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char HeatChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of a heat wave for the day
//o---------------------------------------------------------------------------o
char CWeather::HeatChance( void ) const
{
	return weather[HEAT].Chance;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char ColdChance( void ) const
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of a cold snap for the day
//o---------------------------------------------------------------------------o
char CWeather::ColdChance( void ) const
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
//|   Function    -  void SnowIntensity( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the snow intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::SnowIntensity( char value )
{
	weather[SNOW].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatIntensity( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the heat intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::HeatIntensity( char value )
{
	weather[HEAT].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RainIntensity( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the rain intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::RainIntensity( char value )
{
	weather[RAIN].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void ColdIntensity( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the cold intensity of the system
//o---------------------------------------------------------------------------o
void CWeather::ColdIntensity( char value )
{
	weather[COLD].Intensity = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RainChance( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of rain in the system
//o---------------------------------------------------------------------------o
void CWeather::RainChance( char value )
{
	weather[RAIN].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SnowChance( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of snow in the system
//o---------------------------------------------------------------------------o
void CWeather::SnowChance( char value )
{
	weather[SNOW].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void HeatChance( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of a heat wave in the system
//o---------------------------------------------------------------------------o
void CWeather::HeatChance( char value )
{
	weather[HEAT].Chance = value;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void ColdChance( char value )
//|   Date        -  11th April, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance of a cold snap in the system
//o---------------------------------------------------------------------------o
void CWeather::ColdChance( char value )
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
	weather.resize( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI32 Count( void ) const
//|   Date        -  31st July, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns count of weather systems
//o---------------------------------------------------------------------------o
UI32 cWeatherAb::Count( void ) const
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
	char sect[512];
	while( !done )
	{
		sprintf( sect, "WEATHERAB %d", wthCount );
		ScriptSection *tempSect = FileLookup->FindEntry( sect, weathab_def );
		if( tempSect == NULL )
			done = true;
		else
			wthCount++;
	}
	ScriptSection *WeatherStuff = NULL;
	weather.resize( wthCount );

	const char *tag = NULL;
	const char *data = NULL;

	for( SI16 i = 0; i < weather.size(); i++ )
	{
		sprintf( sect, "WEATHERAB %d", i );
		WeatherStuff = FileLookup->FindEntry( sect, weathab_def );
		if( WeatherStuff == NULL )
			continue;

		for( tag = WeatherStuff->First(); !WeatherStuff->AtEnd(); tag = WeatherStuff->Next() )
		{
			data = WeatherStuff->GrabData();
			switch( tag[0] )
			{

			case 'c':
			case 'C':
				if( !strcmp( "COLDCHANCE", tag ) )			// chance for a cold day
					ColdChance( i, makeNum( data ) );
				else if( !strcmp( "COLDINTENSITY", tag ) )	// cold intensity
					ColdIntensity( i, makeNum( data ) );
				break;

			case 'h':
			case 'H':
				if( !strcmp( "HEATCHANCE", tag ) )			// chance for a hot day
					HeatChance( i, makeNum( data ) );
				else if( !strcmp( "HEATINTENSITY", tag ) )	// heat intensity
					HeatIntensity( i, makeNum( data ) );
				break;

			case 'l':
			case 'L':
				if( !strcmp( "LIGHTMIN", tag ) )			// minimum light level
					LightMin( i, atof( data ) );
				else if( !strcmp( "LIGHTMAX", tag ) )		// maximum light level
					LightMax( i, atof( data ) );
				break;

			case 'm':
			case 'M':
				if( !strcmp( "MAXTEMP", tag ) )				// maximum temperature
					MaxTemp( i, atof( data ) );
				else if( !strcmp( "MINTEMP", tag ) )		// minimum temperature
					MinTemp( i, atof( data ) );
				else if( !strcmp( "MAXWIND", tag ) )		// maximum wind speed
					MaxWindSpeed( i, atof( data ) );
				else if( !strcmp( "MINWIND", tag ) )		// minimum wind speed
					MinWindSpeed( i, atof( data ) );
				break;

			case 'r':
			case 'R':

				if( !strcmp( "RAINCHANCE", tag ) )			// chance of rain
					RainChance( i, makeNum( data ) );
				else if( !strcmp( "RAININTENSITY", tag ) )	// intensity of rain
					RainIntensity( i, makeNum( data ) );
				break;

			case 's':
			case 'S':

				if( !strcmp( "SNOWCHANCE", tag ) )			// chance of snow
					SnowChance( i, makeNum( data ) );
				else if( !strcmp( "SNOWINTENSITY", tag ) )	// intensity of snow
					SnowIntensity( i, makeNum( data ) );
				else if( !strcmp( "SNOWTHRESHOLD", tag ) )	// temperature at which snow kicks in
					SnowThreshold( i, atof( data ) );
				break;
			}
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  char Intensity( weathID toCheck, UI08 weathType )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to return intensity of a weather type
//o---------------------------------------------------------------------------o
char cWeatherAb::Intensity( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].Intensity( weathType );
}
		
//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( weathID toCheck, UI08 weathType, char value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to set the intensity of a particular weather type
//o---------------------------------------------------------------------------o
void cWeatherAb::Intensity( weathID toCheck, UI08 weathType, char value )
{
	weather[toCheck].Intensity( weathType, value );
}

//o---------------------------------------------------------------------------o
//|   Function    -  char Chance( weathID toCheck, UI08 weathType )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to return chance of a weather type
//o---------------------------------------------------------------------------o
char cWeatherAb::Chance( weathID toCheck, UI08 weathType )
{
	return weather[toCheck].Chance( weathType );
}
		
//o---------------------------------------------------------------------------o
//|   Function    -  void Intensity( weathID toCheck, UI08 weathType, char value )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Helper func to set the chance of finding a weather type
//o---------------------------------------------------------------------------o
void cWeatherAb::Chance( weathID toCheck, UI08 weathType, char value )
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
//|   Function    -  char SnowIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's snow
//o---------------------------------------------------------------------------o
char cWeatherAb::SnowIntensity( weathID toCheck )
{
	return Intensity( toCheck, SNOW );
}

//o---------------------------------------------------------------------------o
//|   Function    -  char RainIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's rain
//o---------------------------------------------------------------------------o
char cWeatherAb::RainIntensity( weathID toCheck )
{
	return Intensity( toCheck, RAIN );
}

//o---------------------------------------------------------------------------o
//|   Function    -  char HeatIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's heat
//o---------------------------------------------------------------------------o
char cWeatherAb::HeatIntensity( weathID toCheck )
{
	return Intensity( toCheck, HEAT );
}

//o---------------------------------------------------------------------------o
//|   Function    -  char ColdIntensity( weathID toCheck )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intensity of toCheck's cold
//o---------------------------------------------------------------------------o
char cWeatherAb::ColdIntensity( weathID toCheck )
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

void cWeatherAb::SnowIntensity( weathID toCheck, char value )
{
	Intensity( toCheck, SNOW, value );
}
void cWeatherAb::RainIntensity( weathID toCheck, char value )
{
	Intensity( toCheck, RAIN, value );
}
void cWeatherAb::HeatIntensity( weathID toCheck, char value )
{
	Intensity( toCheck, HEAT, value );
}
void cWeatherAb::ColdIntensity( weathID toCheck, char value )
{
	Intensity( toCheck, COLD, value );
}
void cWeatherAb::RainChance( weathID toCheck, char value )
{
	Chance( toCheck, RAIN, value );
}
void cWeatherAb::SnowChance( weathID toCheck, char value )
{
	Chance( toCheck, SNOW, value );
}
void cWeatherAb::HeatChance( weathID toCheck, char value )
{
	Chance( toCheck, HEAT, value );
}
void cWeatherAb::ColdChance( weathID toCheck, char value )
{
	Chance( toCheck, COLD, value );
}

char cWeatherAb::RainChance( weathID toCheck )
{
	return Chance( toCheck, RAIN );
}
char cWeatherAb::SnowChance( weathID toCheck )
{
	return Chance( toCheck, SNOW );
}
char cWeatherAb::HeatChance( weathID toCheck )
{
	return Chance( toCheck, HEAT );
}
char cWeatherAb::ColdChance( weathID toCheck )
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
	for( weathID currval = 0; currval < weather.size(); currval++ )
		weather[currval].NewDay();
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
	for( weathID currval = 0; currval < weather.size(); currval++ )
		weather[currval].PeriodicUpdate();
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

bool cWeatherAb::DoPlayerStuff( CChar *p )
{
	if( p == NULL || p->IsNpc() )
		return true;
	weathID currval = region[p->GetRegion()]->GetWeather();
	cSocket *s = calcSocketObjFromChar( p );
	if( currval > weather.size() || weather.size() == 0 )
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
			p->SetWeathDamage( BuildTimeValue( Races->SnowSecs( p->GetRace() ) ), SNOW );
	} 
	else if( isRaining )
	{
		DoPlayerWeather( s, 1, (SI08)Temp( currval ) );
		if( p->GetWeathDamage( RAIN ) == 0 )
			p->SetWeathDamage( BuildTimeValue( Races->RainSecs( p->GetRace() ) ), RAIN );
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

void cWeatherAb::DoPlayerWeather( cSocket *s, UI08 weathType, SI08 currentTemp )
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
		soundeffect( mChar, 0x14 + RandomNum( 0, 1 ) );
		s->Send( &snow );
		break;
	case 3:
		s->Send( &rain );
		soundeffect( mChar, 0x14 + RandomNum( 0, 1 ) );
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
bool doLightEffect( CChar *i )
{
	bool didDamage = false;
	
	if( i == NULL )
		return false;
	if( i->IsNpc() || i->IsGM() || !isOnline( i ) || !Races->LightAffect( i->GetRace() ) )
		return false;

	UI08 hour = cwmWorldState->ServerData()->GetServerTimeHours();
	bool ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();

	cSocket *mSock = calcSocketObjFromChar( i );
	if( i->GetFixedLight() != 255 )
	{
		if( hour < 5 && ampm || hour >= 5 && !ampm )	// time of day we can be burnt
		{
			if( i->GetWeathDamage( LIGHT ) != 0 )
			{
				if( i->GetWeathDamage( LIGHT ) <= uiCurrentTime )
				{
					sysmessage( mSock, 1216 );
					i->SetHP( i->GetHP() - Races->LightDamage( i->GetRace() ) );
					i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) ), LIGHT );
					staticeffect( i, 0x3709, 0x09, 0x19 );
					soundeffect( i, 0x0208 );     
					didDamage = true;
				}
			}
			else
				i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) ), LIGHT );
			
		}
		else if( hour < 6 && ampm || hour >= 4 && !ampm )	// slightly burnt at this time of day
		{
			if( i->GetWeathDamage( LIGHT ) != 0 )
			{
				if( i->GetWeathDamage( LIGHT ) <= uiCurrentTime )
				{
					sysmessage( mSock, 1217 );
					i->SetHP( i->GetHP() - Races->LightDamage( i->GetRace() ) / 2 );
					i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) * 2 ), LIGHT );
					staticeffect( i, 0x3709, 0x09, 0x19 );
					soundeffect( i, 0x0208 );     
					didDamage = true;
				}
			}
		}
		else
		{
			i->SetWeathDamage( 0, LIGHT );
			if( hour > 3 && hour < 4 && !ampm )
				sysmessage( mSock, 1215 );
		}
	}
	else
	{
		if( !inDungeon( i ) )
		{
			if( hour < 5 && ampm || hour >= 5 && !ampm )
			{
				if( i->GetWeathDamage( LIGHT ) != 0 )
				{
					if( i->GetWeathDamage( LIGHT ) <= uiCurrentTime )
					{
						sysmessage( mSock, 1216 );
						i->SetHP( i->GetHP() - Races->LightDamage( i->GetRace() ) );
						i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) ), LIGHT );
						staticeffect( i, 0x3709, 0x09, 0x19 );
						soundeffect( i, 0x0208 );     
						didDamage = true;
					}
				}
				else
					i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) ), LIGHT );
				
			}
			else if( hour < 6 && ampm || hour >= 4 && !ampm )
			{
				if( i->GetWeathDamage( LIGHT ) != 0 )
				{
					if( i->GetWeathDamage( LIGHT ) <= uiCurrentTime )
					{
						sysmessage( mSock, 1217 );
						i->SetHP( i->GetHP() - Races->LightDamage( i->GetRace() ) / 2 );
						i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) * 2 ), LIGHT );
						staticeffect( i, 0x3709, 0x09, 0x19 );
						soundeffect( i, 0x0208 );     
						didDamage = true;
					}
				}
			}
			else
			{
				i->SetWeathDamage( 0, LIGHT );
				if( hour > 3 && hour < 4 && !ampm )
					sysmessage( mSock, 1215 );
			}
		}
		else
		{
			if( hour >= 5 && hour <= 6 && ampm && i->GetWeathDamage( LIGHT ) <= uiCurrentTime )
			{
				sysmessage( mSock, 1218 );
				i->SetWeathDamage( BuildTimeValue( Races->LightSecs( i->GetRace() ) * 2 ), LIGHT );
			}
		}
	}
	return didDamage;
}

bool doRainEffect( CChar *i )
{
	if( i == NULL )
		return false;
	if( i->IsNpc() || !isOnline( i ) || !Races->RainAffect( i->GetRace() ) )
		return false;
	bool didDamage = false;
	if( !inDungeon( i ) && Weather->RainActive( region[i->GetRegion()]->GetWeather() ) )
	{
		if( i->GetWeathDamage( RAIN ) != 0 && i->GetWeathDamage( RAIN ) <= uiCurrentTime )
		{
			sysmessage( calcSocketObjFromChar( i ), 1219 );
			i->SetHP( i->GetHP() - Races->RainDamage( i->GetRace() ) );
			i->SetWeathDamage( BuildTimeValue( Races->RainSecs( i->GetRace() ) ), RAIN );
			staticeffect( i, 0x3709, 0x09, 0x19 );
			soundeffect( i, 0x0208 );     
			didDamage = true;
		}
		else
			i->SetWeathDamage( BuildTimeValue( Races->RainSecs( i->GetRace() ) ), RAIN );
	}
	else
		i->SetWeathDamage( 0, RAIN );
	return didDamage;
}

bool doSnowEffect( CChar *i )
{
	if( i == NULL )
		return false;
	if( i->IsNpc() || !isOnline( i ) || !Races->SnowAffect( i->GetRace() ) )
		return false;
	bool didDamage = false;
	if( !inDungeon( i ) && Weather->SnowActive( region[i->GetRegion()]->GetWeather() ) )
	{
		if( i->GetWeathDamage( SNOW ) != 0 && i->GetWeathDamage( SNOW ) <= uiCurrentTime )
		{
			sysmessage( calcSocketObjFromChar( i ), 1220 );
			i->SetHP( i->GetHP() - Races->SnowDamage( i->GetRace() ) );
			i->SetWeathDamage( BuildTimeValue( Races->SnowSecs( i->GetRace() ) ), SNOW );
			staticeffect( i, 0x3709, 0x09, 0x19 );
			soundeffect( i, 0x0208 );     
			didDamage = true;
		}
		else
			i->SetWeathDamage( BuildTimeValue( Races->SnowSecs( i->GetRace() ) ), SNOW );
	}
	else
		i->SetWeathDamage( 0, SNOW );
	return didDamage;
}

bool doHeatEffect( CChar *i )
{
	if( i == NULL )
		return false;
	if( i->IsNpc() || !isOnline( i ) || !Races->HeatAffect( i->GetRace () ) )
		return false;
	bool didDamage = false;
	UI08 charRegion = i->GetRegion();
	UI08 weatherSys = region[charRegion]->GetWeather();
	if( weatherSys != 255 )
	{
		R32 tempCurrent = Weather->Temp( weatherSys );
		R32 tempMax = Weather->MaxTemp( weatherSys );
		R32 tempMin = Weather->MinTemp( weatherSys );
		if( tempCurrent > ( tempMax - tempMin ) / 4 * 3 )	// 3/4 of the temp is done
		{
			if( i->GetWeathDamage( HEAT ) != 0 && i->GetWeathDamage( HEAT ) <= uiCurrentTime )
			{
				R32 damageModifier = ( tempMax - tempCurrent ) / 5;
				i->SetHP((SI16)( i->GetHP() - ( (R32)Races->HeatDamage( i->GetRace() ) * damageModifier )) );
				i->SetStamina( i->GetStamina() - 2 );
				sysmessage( calcSocketObjFromChar( i ), 1221 );
				staticeffect( i, 0x3709, 0x09, 0x19 );
				soundeffect( i, 0x0208 );     
				didDamage = true;
			}
			i->SetWeathDamage( BuildTimeValue( Races->HeatSecs( i->GetRace() ) ), HEAT );
		}
	}
	return didDamage;
}

bool doColdEffect( CChar *i )
{
	if( i == NULL )
		return false;
	if( i->IsNpc() || !isOnline( i ) || !Races->HeatAffect( i->GetRace () ) )
		return false;
	bool didDamage = false;
	UI08 charRegion = i->GetRegion();
	UI08 weatherSys = region[charRegion]->GetWeather();
	if( weatherSys != 255 )
	{
		R32 tempCurrent = Weather->Temp( weatherSys );
		R32 tempMax = Weather->MaxTemp( weatherSys );
		R32 tempMin = Weather->MinTemp( weatherSys );
		if( tempCurrent < ( tempMax - tempMin ) / 4 * 1 )	// 3/4 of the temp is done
		{
			if( i->GetWeathDamage( COLD ) != 0 && i->GetWeathDamage( COLD ) <= uiCurrentTime )
			{
				R32 damageModifier = ( tempMax - tempCurrent ) / 5;
				i->SetHP((SI16)( i->GetHP() - ( (R32)Races->ColdDamage( i->GetRace() ) * damageModifier ) ));
				sysmessage( calcSocketObjFromChar( i ), 1606 );
				staticeffect( i, 0x3709, 0x09, 0x19 );
				soundeffect( i, 0x0208 );     
				didDamage = true;
			}
			i->SetWeathDamage( BuildTimeValue( Races->HeatSecs( i->GetRace() ) ), HEAT );
		}
	}
	return didDamage;
}
