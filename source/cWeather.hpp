#ifndef __WeatherAb__
#define __WeatherAb__

class CWeather
{
private:
	struct WeathPart
	{
		SI08 Chance;
		SI08 Intensity;
		SI08 IntensityLow;
		SI08 IntensityHigh;
		bool Active;
	};

	weathID		ID;
	WeathPart	weather[WEATHNUM];
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
	std::vector< CWeather > weather;
	SI08	IntensityHigh( weathID toCheck, UI08 weathType );
	void	IntensityHigh( weathID toCheck, UI08 weathType, SI08 value );
	SI08	IntensityLow( weathID toCheck, UI08 weathType );
	void	IntensityLow( weathID toCheck, UI08 weathType, SI08 value );
	SI08	Intensity( weathID toCheck, UI08 weathType );
	void	Intensity( weathID toCheck, UI08 weathType, SI08 value );
	SI08	Chance( weathID toCheck, UI08 weathType );
	void	Chance( weathID toCheck, UI08 weathType, SI08 value );
	R32		Value( weathID toCheck, UI08 valType, UI08 valOff );
	void	Value( weathID toCheck, UI08 valType, UI08 valOff, R32 value );

public:
	cWeatherAb();
	~cWeatherAb();
	bool	Load( void );
	bool	NewDay( void );
	bool	NewHour( void );
	size_t	Count( void ) const;

	SI08	SnowIntensityHigh( weathID toCheck );
	SI08	SnowIntensityLow( weathID toCheck );
	SI08	SnowIntensity( weathID toCheck );
	SI08	RainIntensityHigh( weathID toCheck );
	SI08	RainIntensityLow( weathID toCheck );
	SI08	RainIntensity( weathID toCheck );
	SI08	HeatIntensityHigh( weathID toCheck );
	SI08	ColdIntensityHigh( weathID toCheck );
	SI08	StormIntensityHigh( weathID toCheck );
	SI08	StormIntensityLow( weathID toCheck );
	SI08	StormIntensity( weathID toCheck );
	R32		MaxTemp( weathID toCheck );
	R32		MinTemp( weathID toCheck );
	R32		Temp( weathID toCheck );
	R32		RainTempDrop( weathID toCheck );
	R32		StormTempDrop( weathID toCheck );
	R32		MaxWindSpeed( weathID toCheck );
	R32		MinWindSpeed( weathID toCheck );
	R32		WindSpeed( weathID toCheck );
	SI08	RainChance( weathID toCheck );
	SI08	SnowChance( weathID toCheck );
	SI08	HeatChance( weathID toCheck );
	SI08	ColdChance( weathID toCheck );
	SI08	StormChance( weathID toCheck );
	R32		LightMin( weathID toCheck );
	R32		LightMax( weathID toCheck );
	R32		CurrentLight( weathID toCheck );
	bool	RainActive( weathID toCheck );
	bool	SnowActive( weathID toCheck );
	bool	StormBrewing( weathID toCheck );
	bool	StormActive( weathID toCheck );
	bool	HeatActive( weathID toCheck );
	bool	ColdActive( weathID toCheck );

	R32		SnowThreshold( weathID toCheck );

	void	WindSpeed( weathID toCheck, R32 value );
	void	MaxWindSpeed( weathID toCheck, R32 value );
	void	MinWindSpeed( weathID toCheck, R32 value );
	void	MaxTemp( weathID toCheck, R32 value );
	void	MinTemp( weathID toCheck, R32 value );
	void	Temp( weathID toCheck, R32 value );
	void	RainTempDrop( weathID toCheck, R32 value );
	void	StormTempDrop( weathID toCheck, R32 value );
	void	SnowIntensityHigh( weathID toCheck, SI08 value );
	void	SnowIntensityLow( weathID toCheck, SI08 value );
	void	SnowIntensity( weathID toCheck, SI08 value );
	void	RainIntensityHigh( weathID toCheck, SI08 value );
	void	RainIntensityLow( weathID toCheck, SI08 value );
	void	RainIntensity( weathID toCheck, SI08 value );
	void	HeatIntensityHigh( weathID toCheck, SI08 value );
	void	ColdIntensityHigh( weathID toCheck, SI08 value );
	void	StormIntensityHigh( weathID toCheck, SI08 value );
	void	StormIntensityLow( weathID toCheck, SI08 value );
	void	StormIntensity( weathID toCheck, SI08 value );
	void	RainChance( weathID toCheck, SI08 value );
	void	SnowChance( weathID toCheck, SI08 value );
	void	HeatChance( weathID toCheck, SI08 value );
	void	ColdChance( weathID toCheck, SI08 value );
	void	StormChance( weathID toCheck, SI08 value );
	void	SnowThreshold( weathID toCheck, R32 value );
	void	LightMin( weathID toCheck, R32 newValue );
	void	LightMax( weathID toCheck, R32 newValue );
	void	CurrentLight( weathID toCheck, R32 newValue );

	R32		EffectiveMaxTemp( weathID toCheck );
	R32		EffectiveMinTemp( weathID toCheck );
	void	EffectiveMaxTemp( weathID toCheck, R32 value );
	void	EffectiveMinTemp( weathID toCheck, R32 value );
	void	RainActive( weathID toCheck, bool value );
	void	SnowActive( weathID toCheck, bool value );
	void	StormBrewing( weathID toCheck, bool value );
	void	StormActive( weathID toCheck, bool value );
	void	HeatActive( weathID toCheck, bool value );
	void	ColdActive( weathID toCheck, bool value );
	bool	DoStuff( void );
	bool	DoPlayerStuff( CSocket *mSock, CChar *p );
	void	DoPlayerWeather( CSocket *s, UI08 weathType, SI08 currentTemp, weathID currval );
	bool	doWeatherEffect( CSocket *mSock, CChar& mChar, WeatherType element );
	bool	doLightEffect( CSocket *mSock, CChar& mChar );
	bool	DoNPCStuff( CChar *p );
	void	SendJSWeather( CChar *mChar, WeatherType weathType, SI08 currentTemp );

	CWeather *Weather( weathID toCheck );
};

extern cWeatherAb *Weather;

#endif
