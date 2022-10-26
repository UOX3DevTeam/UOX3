// This command tells the command user the current in-game time

function CommandRegistration()
{
	RegisterCommand( "time", 1, true );
}

function command_TIME( socket, cmdString )
{
	TellTime( socket );
}

function TellTime( pSock )
{
	if( pSock )
	{
		var hour	= GetHour();
		var minute	= GetMinute();
		var pLang	= pSock.language;

		var tempString;
		if( minute <= 14 )
		{
			tempString = GetDictionaryEntry( 1248, pLang ); // It is
		}
		else if( minute >= 15 && minute <= 30 )
		{
			tempString = GetDictionaryEntry( 1249, pLang ); // It is a quarter past
		}
		else if( minute >= 30 && minute <= 45 )
		{
			tempString = GetDictionaryEntry( 1250, pLang ); // It is half past
		}
		else
		{
			tempString = GetDictionaryEntry( 1251, pLang ); // It is a quarter till
			if( hour == 24 )
			{
				hour = 1;
			}
			else
			{
				++hour;
			}
		}

		tempString += " ";

		if( hour == 12 )
		{
			tempString += GetDictionaryEntry( 1264, pLang ); //  noon.
		}
		else if( hour == 24 || hour == 0 )
		{
			tempString += GetDictionaryEntry( 1263, pLang ); //  midnight.
		}
		else if( hour < 24 )
		{
			if( hour < 12 )
			{
				tempString += GetDictionaryEntry( 1251 + hour, pLang );
			}
			else
			{
				tempString += GetDictionaryEntry( 1251 + (hour - 12), pLang );
			}

			tempString += " ";

			if( hour >= 21 || ( hour >= 1 && hour < 5 ))
			{
				tempString += GetDictionaryEntry( 1267, pLang ); //  at night.
			}
			else if( hour >= 5 && hour < 13 )
			{
				tempString += GetDictionaryEntry( 1269, pLang ); //  in the morning.
			}
			else if( hour >= 13 && hour < 18 )
			{
				tempString += GetDictionaryEntry( 1265, pLang ); //  in the afternoon.
			}
			else if( hour >= 18 && hour < 21 )
			{
				tempString += GetDictionaryEntry( 1266, pLang ); //  in the evening.
			}
		}
		pSock.SysMessage( tempString );
	}
}

