//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  cweather.h
//
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

#ifndef __WeatherAb__
#define __WeatherAb__

#define EFFECTIVE 2
struct weatherSys
{
	weathID ID;
	float assortVals[3][3];			// max, min and curr values for temp and wind
	char weathChance[WEATHNUM];
	char weathIntensity[WEATHNUM];
	float snowThreshold;
	bool weathActive[WEATHNUM];
};

class cWeatherAb
{
	protected:
		weatherSys *weather;
		weathID weathCount;
		char getIntensity( weathID toCheck, unsigned char weathType );
		void setIntensity( weathID toCheck, unsigned char weathType, char value );
		char getChance( weathID toCheck, unsigned char weathType );
		void setChance( weathID toCheck, unsigned char weathType, char value );
		float getValue( weathID toCheck, unsigned char valType, unsigned char valOff );
		void setValue( weathID toCheck, unsigned char valType, unsigned char valOff, float value );

	public:
    cWeatherAb();
    ~cWeatherAb();
		bool load( void );
		bool newDay( void );

		char getSnowIntensity( weathID toCheck );
		char getRainIntensity( weathID toCheck );
		char getHeatIntensity( weathID toCheck );
		char getColdIntensity( weathID toCheck );
		float getMaxTemp( weathID toCheck );
		float getMinTemp( weathID toCheck );
		float getTemp( weathID toCheck );
		float getMaxWindSpeed( weathID toCheck );
		float getMinWindSpeed( weathID toCheck );
		float getWindSpeed( weathID toCheck );
		char getRainChance( weathID toCheck );
		char getSnowChance( weathID toCheck );
		char getHeatChance( weathID toCheck );
		char getColdChance( weathID toCheck );
		bool getRainActive( weathID toCheck );
		bool getSnowActive( weathID toCheck );

		float getSnowThreshold( weathID toCheck );

		void setWindSpeed( weathID toCheck, float value );
		void setMaxWindSpeed( weathID toCheck, float value );
		void setMinWindSpeed( weathID toCheck, float value );
		void setMaxTemp( weathID toCheck, float value );
		void setMinTemp( weathID toCheck, float value );
		void setTemp( weathID toCheck, float value );
		void setSnowIntensity( weathID toCheck, char value );
		void setRainIntensity( weathID toCheck, char value );
		void setHeatIntensity( weathID toCheck, char value );
		void setColdIntensity( weathID toCheck, char value );
		void setRainChance( weathID toCheck, char value );
		void setSnowChance( weathID toCheck, char value );
		void setHeatChance( weathID toCheck, char value );
		void setColdChance( weathID toCheck, char value );
		void setSnowThreshold( weathID toCheck, float value );

		float getEffectiveMaxTemp( weathID toCheck );
		float getEffectiveMinTemp( weathID toCheck );
		void setEffectiveMaxTemp( weathID toCheck, float value );
		void setEffectiveMinTemp( weathID toCheck, float value );
		void setRainActive( weathID toCheck, bool value );
		void setSnowActive( weathID toCheck, bool value );
		bool doStuff( void );
		bool doPlayerStuff( CHARACTER p );
		void doPlayerWeather( UOXSOCKET s, unsigned char weathType );
};

#endif
