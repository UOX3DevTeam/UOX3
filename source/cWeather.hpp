#ifndef __WeatherAb__
#define __WeatherAb__

#define EFFECTIVE 2

struct WeathPart
{
	char Chance;
	char Intensity;
	bool Active;
};

class CWeather
{
private:
	weathID		ID;
	WeathPart	weather[WEATHNUM];
	R32			assortVals[3][3];
	R32			snowThreshold;
	R32			light[3];
public:
			CWeather();
	virtual	~CWeather();

	char	Intensity( UI08 weathType ) const;
	void	Intensity( UI08 weathType, char value );

	char	Chance( UI08 weathType ) const;
	void	Chance( UI08 weathType, char value );

	R32		Value( UI08 valType, UI08 valOff ) const;
	void	Value( UI08 valType, UI08 valOff, R32 value );

	char	SnowIntensity( void ) const;
	char	RainIntensity( void ) const;
	char	HeatIntensity( void ) const;
	char	ColdIntensity( void ) const;

	R32		MaxTemp( void ) const;
	R32		MinTemp( void ) const;
	R32		Temp( void ) const;
	R32		MaxWindSpeed( void ) const;
	R32		MinWindSpeed( void ) const;
	R32		WindSpeed( void ) const;

	char	RainChance( void ) const;
	char	SnowChance( void ) const;
	char	HeatChance( void ) const;
	char	ColdChance( void ) const;

	R32		LightMin( void ) const;
	R32		LightMax( void ) const;
	R32		CurrentLight( void ) const;

	bool	RainActive( void ) const;
	bool	SnowActive( void ) const;
	bool	HeatActive( void ) const;
	bool	ColdActive( void ) const;

	R32		SnowThreshold( void ) const;

	void	WindSpeed( R32 value );
	void	MaxWindSpeed( R32 value );
	void	MinWindSpeed( R32 value );
	void	MaxTemp( R32 value );
	void	MinTemp( R32 value );
	void	Temp( R32 value );
	void	SnowIntensity( char value );
	void	HeatIntensity( char value );
	void	RainIntensity( char value );
	void	ColdIntensity( char value );
	void	RainChance( char value );
	void	SnowChance( char value );
	void	HeatChance( char value );
	void	ColdChance( char value );
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
	void	HeatActive( bool value );
	void	ColdActive( bool value );

	void	NewDay( void );
	bool	PeriodicUpdate( void );
};

class cWeatherAb
{
	protected:
		vector< CWeather > weather;
		char	Intensity( weathID toCheck, UI08 weathType );
		void	Intensity( weathID toCheck, UI08 weathType, char value );
		char	Chance( weathID toCheck, UI08 weathType );
		void	Chance( weathID toCheck, UI08 weathType, char value );
		R32		Value( weathID toCheck, UI08 valType, UI08 valOff );
		void	Value( weathID toCheck, UI08 valType, UI08 valOff, R32 value );

	public:
				cWeatherAb();
		virtual	~cWeatherAb();
		bool	Load( void );
		bool	NewDay( void );
		UI32	Count( void ) const;

		char	SnowIntensity( weathID toCheck );
		char	RainIntensity( weathID toCheck );
		char	HeatIntensity( weathID toCheck );
		char	ColdIntensity( weathID toCheck );
		R32		MaxTemp( weathID toCheck );
		R32		MinTemp( weathID toCheck );
		R32		Temp( weathID toCheck );
		R32		MaxWindSpeed( weathID toCheck );
		R32		MinWindSpeed( weathID toCheck );
		R32		WindSpeed( weathID toCheck );
		char	RainChance( weathID toCheck );
		char	SnowChance( weathID toCheck );
		char	HeatChance( weathID toCheck );
		char	ColdChance( weathID toCheck );
		R32		LightMin( weathID toCheck );
		R32		LightMax( weathID toCheck );
		bool	RainActive( weathID toCheck );
		bool	SnowActive( weathID toCheck );

		R32		SnowThreshold( weathID toCheck );

		void	WindSpeed( weathID toCheck, R32 value );
		void	MaxWindSpeed( weathID toCheck, R32 value );
		void	MinWindSpeed( weathID toCheck, R32 value );
		void	MaxTemp( weathID toCheck, R32 value );
		void	MinTemp( weathID toCheck, R32 value );
		void	Temp( weathID toCheck, R32 value );
		void	SnowIntensity( weathID toCheck, char value );
		void	RainIntensity( weathID toCheck, char value );
		void	HeatIntensity( weathID toCheck, char value );
		void	ColdIntensity( weathID toCheck, char value );
		void	RainChance( weathID toCheck, char value );
		void	SnowChance( weathID toCheck, char value );
		void	HeatChance( weathID toCheck, char value );
		void	ColdChance( weathID toCheck, char value );
		void	SnowThreshold( weathID toCheck, R32 value );
		void	LightMin( weathID toCheck, R32 newValue );
		void	LightMax( weathID toCheck, R32 newValue );

		R32		EffectiveMaxTemp( weathID toCheck );
		R32		EffectiveMinTemp( weathID toCheck );
		void	EffectiveMaxTemp( weathID toCheck, R32 value );
		void	EffectiveMinTemp( weathID toCheck, R32 value );
		void	RainActive( weathID toCheck, bool value );
		void	SnowActive( weathID toCheck, bool value );
		bool	DoStuff( void );
		bool	DoPlayerStuff( CChar *p );
		void	DoPlayerWeather( cSocket *s, UI08 weathType, SI08 currentTemp );

		CWeather *Weather( weathID toCheck );
};

#endif
