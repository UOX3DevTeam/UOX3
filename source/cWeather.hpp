#ifndef __WeatherAb__
#define __WeatherAb__

class CWeather
{
private:
	struct WeathPart_st
	{
		SI08 Chance;
		SI08 Intensity;
		SI08 IntensityLow;
		SI08 IntensityHigh;
		bool Active;
	};

	[[maybe_unused]] WEATHID	ID;
	WeathPart_st	weather[WEATHNUM];
	R32			assortVals[3][3];
	R32			snowThreshold;
	R32			light[3];
	R32			rainTempDrop;
	R32			stormTempDrop;
	bool		stormDelay;

public:
	CWeather();
	virtual	~CWeather();

	SI08	IntensityHigh( UI08 weathType ) const;
	void	IntensityHigh( UI08 weathType, SI08 value );
	SI08	IntensityLow( UI08 weathType ) const;
	void	IntensityLow( UI08 weathType, SI08 value );
	SI08	Intensity( UI08 weathType ) const;
	void	Intensity( UI08 weathType, SI08 value );

	SI08	Chance( UI08 weathType ) const;
	void	Chance( UI08 weathType, SI08 value );

	R32		Value( UI08 valType, UI08 valOff ) const;
	void	Value( UI08 valType, UI08 valOff, R32 value );

	SI08	SnowIntensityHigh( void ) const;
	SI08	SnowIntensityLow( void ) const;
	SI08	SnowIntensity( void ) const;
	SI08	RainIntensityHigh( void ) const;
	SI08	RainIntensityLow( void ) const;
	SI08	RainIntensity( void ) const;
	SI08	HeatIntensityHigh( void ) const;
	SI08	ColdIntensityHigh( void ) const;
	SI08	StormIntensityHigh( void ) const;
	SI08	StormIntensityLow( void ) const;
	SI08	StormIntensity( void ) const;

	R32		MaxTemp( void ) const;
	R32		MinTemp( void ) const;
	R32		Temp( void ) const;
	R32		MaxWindSpeed( void ) const;
	R32		MinWindSpeed( void ) const;
	R32		WindSpeed( void ) const;
	R32		RainTempDrop( void ) const;
	R32		StormTempDrop( void ) const;

	SI08	RainChance( void ) const;
	SI08	SnowChance( void ) const;
	SI08	StormChance( void ) const;
	SI08	HeatChance( void ) const;
	SI08	ColdChance( void ) const;

	R32		LightMin( void ) const;
	R32		LightMax( void ) const;
	R32		CurrentLight( void ) const;

	bool	RainActive( void ) const;
	bool	SnowActive( void ) const;
	bool	StormBrewing( void ) const;
	bool	StormActive( void ) const;
	bool	HeatActive( void ) const;
	bool	ColdActive( void ) const;
	bool	StormDelay( void ) const;

	R32		SnowThreshold( void ) const;

	void	WindSpeed( R32 value );
	void	MaxWindSpeed( R32 value );
	void	MinWindSpeed( R32 value );
	void	MaxTemp( R32 value );
	void	MinTemp( R32 value );
	void	Temp( R32 value );
	void	RainTempDrop( R32 value );
	void	StormTempDrop( R32 value );
	void	SnowIntensityHigh( SI08 value );
	void	SnowIntensityLow( SI08 value );
	void	SnowIntensity( SI08 value );
	void	HeatIntensityHigh( SI08 value );
	void	RainIntensityHigh( SI08 value );
	void	RainIntensityLow( SI08 value );
	void	RainIntensity( SI08 value );
	void	ColdIntensityHigh( SI08 value );
	void	StormIntensityHigh( SI08 value );
	void	StormIntensityLow( SI08 value );
	void	StormIntensity( SI08 value );
	void	RainChance( SI08 value );
	void	SnowChance( SI08 value );
	void	StormChance( SI08 value );
	void	HeatChance( SI08 value );
	void	ColdChance( SI08 value );
	void	SnowThreshold( R32 value );
	void	LightMin( R32 value );
	void	LightMax( R32 value );
	void	CurrentLight( R32 value );

	R32		EffectiveMaxTemp( void ) const;
	R32		EffectiveMinTemp( void ) const;
	void	EffectiveMaxTemp( R32 value );
	void	EffectiveMinTemp( R32 value );

	void	RainActive( bool value );
	void	SnowActive( bool value );
	void	StormBrewing( bool value );
	void	StormActive( bool value );
	void	HeatActive( bool value );
	void	ColdActive( bool value );
	void	StormDelay( bool value );

	void	NewDay( void );
	void	NewHour( void );
	bool	PeriodicUpdate( void );
};

class cWeatherAb
{
private:
	std::vector<CWeather> weather;
	SI08	IntensityHigh( WEATHID toCheck, UI08 weathType );
	void	IntensityHigh( WEATHID toCheck, UI08 weathType, SI08 value );
	SI08	IntensityLow( WEATHID toCheck, UI08 weathType );
	void	IntensityLow( WEATHID toCheck, UI08 weathType, SI08 value );
	SI08	Intensity( WEATHID toCheck, UI08 weathType );
	void	Intensity( WEATHID toCheck, UI08 weathType, SI08 value );
	SI08	Chance( WEATHID toCheck, UI08 weathType );
	void	Chance( WEATHID toCheck, UI08 weathType, SI08 value );
	R32		Value( WEATHID toCheck, UI08 valType, UI08 valOff );
	void	Value( WEATHID toCheck, UI08 valType, UI08 valOff, R32 value );

public:
	cWeatherAb();
	~cWeatherAb();
	bool	Load( void );
	bool	NewDay( void );
	bool	NewHour( void );
	size_t	Count( void ) const;

	SI08	SnowIntensityHigh( WEATHID toCheck );
	SI08	SnowIntensityLow( WEATHID toCheck );
	SI08	SnowIntensity( WEATHID toCheck );
	SI08	RainIntensityHigh( WEATHID toCheck );
	SI08	RainIntensityLow( WEATHID toCheck );
	SI08	RainIntensity( WEATHID toCheck );
	SI08	HeatIntensityHigh( WEATHID toCheck );
	SI08	ColdIntensityHigh( WEATHID toCheck );
	SI08	StormIntensityHigh( WEATHID toCheck );
	SI08	StormIntensityLow( WEATHID toCheck );
	SI08	StormIntensity( WEATHID toCheck );
	R32		MaxTemp( WEATHID toCheck );
	R32		MinTemp( WEATHID toCheck );
	R32		Temp( WEATHID toCheck );
	R32		RainTempDrop( WEATHID toCheck );
	R32		StormTempDrop( WEATHID toCheck );
	R32		MaxWindSpeed( WEATHID toCheck );
	R32		MinWindSpeed( WEATHID toCheck );
	R32		WindSpeed( WEATHID toCheck );
	SI08	RainChance( WEATHID toCheck );
	SI08	SnowChance( WEATHID toCheck );
	SI08	HeatChance( WEATHID toCheck );
	SI08	ColdChance( WEATHID toCheck );
	SI08	StormChance( WEATHID toCheck );
	R32		LightMin( WEATHID toCheck );
	R32		LightMax( WEATHID toCheck );
	R32		CurrentLight( WEATHID toCheck );
	bool	RainActive( WEATHID toCheck );
	bool	SnowActive( WEATHID toCheck );
	bool	StormBrewing( WEATHID toCheck );
	bool	StormActive( WEATHID toCheck );
	bool	HeatActive( WEATHID toCheck );
	bool	ColdActive( WEATHID toCheck );

	R32		SnowThreshold( WEATHID toCheck );

	void	WindSpeed( WEATHID toCheck, R32 value );
	void	MaxWindSpeed( WEATHID toCheck, R32 value );
	void	MinWindSpeed( WEATHID toCheck, R32 value );
	void	MaxTemp( WEATHID toCheck, R32 value );
	void	MinTemp( WEATHID toCheck, R32 value );
	void	Temp( WEATHID toCheck, R32 value );
	void	RainTempDrop( WEATHID toCheck, R32 value );
	void	StormTempDrop( WEATHID toCheck, R32 value );
	void	SnowIntensityHigh( WEATHID toCheck, SI08 value );
	void	SnowIntensityLow( WEATHID toCheck, SI08 value );
	void	SnowIntensity( WEATHID toCheck, SI08 value );
	void	RainIntensityHigh( WEATHID toCheck, SI08 value );
	void	RainIntensityLow( WEATHID toCheck, SI08 value );
	void	RainIntensity( WEATHID toCheck, SI08 value );
	void	HeatIntensityHigh( WEATHID toCheck, SI08 value );
	void	ColdIntensityHigh( WEATHID toCheck, SI08 value );
	void	StormIntensityHigh( WEATHID toCheck, SI08 value );
	void	StormIntensityLow( WEATHID toCheck, SI08 value );
	void	StormIntensity( WEATHID toCheck, SI08 value );
	void	RainChance( WEATHID toCheck, SI08 value );
	void	SnowChance( WEATHID toCheck, SI08 value );
	void	HeatChance( WEATHID toCheck, SI08 value );
	void	ColdChance( WEATHID toCheck, SI08 value );
	void	StormChance( WEATHID toCheck, SI08 value );
	void	SnowThreshold( WEATHID toCheck, R32 value );
	void	LightMin( WEATHID toCheck, R32 newValue );
	void	LightMax( WEATHID toCheck, R32 newValue );
	void	CurrentLight( WEATHID toCheck, R32 newValue );

	R32		EffectiveMaxTemp( WEATHID toCheck );
	R32		EffectiveMinTemp( WEATHID toCheck );
	void	EffectiveMaxTemp( WEATHID toCheck, R32 value );
	void	EffectiveMinTemp( WEATHID toCheck, R32 value );
	void	RainActive( WEATHID toCheck, bool value );
	void	SnowActive( WEATHID toCheck, bool value );
	void	StormBrewing( WEATHID toCheck, bool value );
	void	StormActive( WEATHID toCheck, bool value );
	void	HeatActive( WEATHID toCheck, bool value );
	void	ColdActive( WEATHID toCheck, bool value );
	bool	DoStuff( void );
	bool	DoPlayerStuff( CSocket *mSock, CChar *p );
	void	DoPlayerWeather( CSocket *s, UI08 weathType, SI08 currentTemp, WEATHID currval );
	bool	doWeatherEffect( CSocket *mSock, CChar& mChar, WeatherType element );
	bool	DoLightEffect( CSocket *mSock, CChar& mChar );
	bool	DoNPCStuff( CChar *p );
	bool	DoItemStuff( CItem *p );
	void	SendJSWeather( CBaseObject *mObj, WeatherType weathType, SI08 currentTemp );

	CWeather *Weather( WEATHID toCheck );
};

extern cWeatherAb *Weather;

#endif
