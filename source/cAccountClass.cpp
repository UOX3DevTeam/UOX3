//o--------------------------------------------------------------------------o
//|	File					-	cAccountClass.cpp
//|	Date					-	12/6/2002 4:16:33 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Seeing as I had screwed up accounts so bad, I started from
//|									scratch. The concept of how they work has been reevaluated
//|									and will be changed to suit a more understandable direction.
//|									See xRFC0004 for complete specification and implementaion
//|									information.
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "cAccountClass.h"

extern cVersionClass CVC;

//#ifndef __UOX3_DTL__
//#define __UOX3_DTL__
//#endif

//o--------------------------------------------------------------------------o
//|	Function			-	Class Construction and Desctruction
//|	Date					-	12/6/2002 4:18:58 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Classic class construction and destruction
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __UOX3_DTL__
cAccountClass::cAccountClass():m_sAccountsDirectory(".\\")
#else
cAccountClass::cAccountClass():m_sAccountsDirectory("dsn:uox3db")
#endif
{
	m_wHighestAccount=0x0000;
	I = m_mapUsernameIDMap.end();
#ifdef __UOX3_DTL__
	// We need to open this DB Connection
	dtl::DBConnection::GetDefaultConnection().Connect(m_sAccountsDirectory/*"uid=example;pwd=example;dsn=exampleA;"*/);
#endif
}
//
cAccountClass::cAccountClass(std::string sAccountsPath)
{
	// Call into the initialize
	cAccountClass();
	// Set accounts path.
	try
	{
		m_sAccountsDirectory=sAccountsPath;
	}	catch(...)
	{
		throw;
	}
}
//
cAccountClass::~cAccountClass()
{

}
//

//o--------------------------------------------------------------------------o
//|	Function			-	WORD cAccountClass::CreateAccountSystem(string sActPath)
//|	Date					-	12/6/2002 4:19:53 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Instead of expecting end users to fix the accounts by hand
//|									the idea of a member function that would provide some means
//|									to automate the process was devised.
//|									
//|									This function is intended for use in uox3 v0.97.0 and 
//|									v0.97.1+ builds for converting them to the new system. This 
//|									member function will also create a new account system for 
//|									pre v0.97.00 versions. However pre UOX3 v0.97.00 characters 
//|									will be lost even if world files have been converted and 
//|									unable to locate the respective character ID number when
//|									converting the world.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
UI16 cAccountClass::CreateAccountSystem(void)
{
	// Get the system account path. Should have been set at construction.
	std::string sActPath = m_sAccountsDirectory;
	// Check account path to make sure that its at least got data in it.
	if(sActPath.length()<=3)
	{
		// If your wondering why a length of 3 - IE: 'C:\'. Send back indication that no accounts were processed
		return 0x0000;
	}
	// Build the full path and filename string here for access.adm
	std::string sAccessAdm = sActPath;
	sAccessAdm += (sActPath[sActPath.length()-1]=='\\'||sActPath[sActPath.length()-1]=='/')?"access.adm":"/access.adm";
	// Create stream and open the Access.adm file if it exists.
	bool bIsNew=true;
	std::fstream fs1(sAccessAdm.c_str(),std::ios::in);
	if(!fs1.is_open())
	{
		// Ok there was no access.adm this implies that this is a pre v0.97.00 release
		bIsNew=false;
	}
	// Build the full path and filename string here for accounts.adm
	std::string sAccountsAdm = sActPath;
	sAccountsAdm += (sActPath[sActPath.length()-1]=='\\')?"accounts.adm":"\\accounts.adm";
	// Create stream and open the Accounts.Adm file for processing
	std::fstream fs2(sAccountsAdm.c_str(),std::ios::in);
	if(!fs2.is_open())
	{
		// ok there was some error. Either the file  doesn't exist or a system error.
		if(fs1.is_open())
			fs1.close();
		return 0x0000; 
	}
	// We need to read from the stream once before entering the loop
	char sLine[129];
	fs2.getline(sLine,128);
	// Ok start the loop and process
	bool bBraces[3]={false,false,false};
	bool bBraces2[3]={false,false,false};
	ACCOUNTSBLOCK actb;
	UI16 wAccountID=0x0000;
	UI16 wAccessID=0x0000;
	UI16 wAccountCount=0x0000;
	memset(&actb,0x00,sizeof(ACCOUNTSBLOCK));
	while(!fs2.eof())
	{
		// Process each Line read in from the accounts.adm file. Handle Comments first
		if(sLine[0]=='\\'||sLine[0]==';'||sLine[0]=='/'||sLine[0]=='\''||sLine[0]==0x13||sLine[0]==0x00)
		{
			fs2.getline((char*)&sLine,128);
			continue;
		}
		// Keep track of the section account lines. There is nothing to process till at least one of these has been read
		if(!strnicmp("SECTION ACCOUNT",sLine,sizeof(char)*15))
		{
			// Increment the account acount
			wAccountCount+=1;
			// Ok the section block was found, Tokenize the string to get the account #
			char *l,*r;
			l=r=NULL;
			// Tokenize the line. I decided instead of wasting more pointer storage would just use the same allocated storage
			l=strtok(sLine," ");		
			l=strtok(NULL," ");
			r=strtok(NULL,"\n");
			actb.wAccountIndex=wAccountID=atoi(r);
			// Ok, we have parsed out the account ID, Set bBraces[2] to true to allow block reading
			bBraces[0]=false;
			bBraces[1]=false;
			bBraces[2]=true;
			// Get the next line and continue
			fs2.getline((char*)&sLine,128);
			continue;
		}
		// Fail safe. If bBraces[2] isn't set there is no need to even run this code
		if(!bBraces[2])
		{
			// Make sure the get the next line to process.
			fs2.getline((char*)&sLine,128);
			continue;
		}
		// If we get here then were reading a block now. Check for the openning brace.
		if(sLine[0]=='{'&&!bBraces[0]&&!bBraces[1])
		{
			bBraces[0]=true;
			fs2.getline((char*)&sLine,128);
			continue;
		}
		if(sLine[0]=='{'&&bBraces[0]||sLine[0]=='{'&&bBraces[1])
		{
			fs2.close();
			fs1.close();
			return 0x0000;
		}
		if(sLine[0]=='}'&&bBraces[0]&&!bBraces[1])
		{
			// Ok we shoud shove this into the map(s) for use later
			m_mapUsernameMap[actb.sUsername]=actb;
			m_mapUsernameIDMap[wAccountID]=actb;
			// Ok we have finished with this access block clean up and continue processing
			bBraces[0]=false;
			bBraces[1]=false;
			bBraces[2]=false;
			bBraces[0]=false;
			fs2.getline((char*)&sLine,128);
			continue;
		}
		// Set up the tokenizing
		char *l,*r;
		l=r=NULL;
		// Tokenize the line
		l=strtok(sLine," ");
		r=strtok(NULL,"\n");
		// Parse and store based on tag
		if(!strnicmp("NAME",l,sizeof(char)*4))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.sUsername =r;
			}
			else
			{
				actb.sUsername="ERROR";
			}
			l=r=NULL;
			fs2.getline((char*)&sLine,128);
			continue;
		}
		else if(!strnicmp("PASS",l,sizeof(char)*4))
		{
			if(r)
			{
				// Ok strip the password and store it.
				actb.sPassword=r;
				l=r=NULL;
				// Now we have to parse the crap in access.adm. Were not gonna look at much format just the 2 tags we need.
				char sLine2[129];
				if(fs1.is_open())
				{
					fs1.getline(sLine2,128);
					bBraces2[2]=true;
					while(!fs1.eof()&&bBraces2[2])
					{
						// Process each Line read in from the accounts.adm file. Handle Comments first
						if(sLine2[0]=='\\'||sLine2[0]==';'||sLine2[0]=='/'||sLine2[0]=='\''||sLine2[0]==0x13||sLine[0]==0x00)
						{
							fs1.getline(sLine2,128);
							continue;
						}
						// Keep track of the section account lines. There is nothing to process till at least one of these has been read
						if(!strnicmp("SECTION ACCESS",sLine2,sizeof(char)*14))
						{
							// Ok the section block was found, Tokenize the string to get the account #
							char *l,*r;
							l=r=NULL;
							// Tokenize the line. I decided instead of wasting more pointer storage would just use the same allocated storage
							l=strtok((char*)sLine2," ");		
							l=strtok(NULL," ");
							r=strtok(NULL,"\n");
							wAccessID=atoi(r);
							// Ok, we have parsed out the account ID, Set bBraces[2] to true to allow block reading
							bBraces2[0]=false;
							bBraces2[1]=false;
							bBraces2[2]=true;
							fs1.getline(sLine2,128);
							continue;
						}
						// Fail safe. If bBraces[2] isn't set there is no need to even run this code
						if(!bBraces2[2])
						{
							// Make sure the get the next line to process.
							fs1.getline(sLine2,128);
							continue;
						}
						// If we get here then were reading a block now. Check for the openning brace.
						if(sLine2[0]=='{'&&!bBraces2[0]&&!bBraces2[1])
						{
							bBraces2[0]=true;
							fs1.getline(sLine2,128);
							continue;
						}
						if(sLine2[0]=='{'&&bBraces2[0]||sLine2[0]=='{'&&bBraces2[1])
						{
							fs2.close();
							fs1.close();
							return 0x0000;
						}
						// Loop reading this block till the end brace is encountered.
						while(bBraces2[0]&&!fs1.eof())
						{
							// Read in the next line
							fs1.getline(sLine2,128);
							// check for the closing brace
							if(sLine2[0]=='}'&&bBraces2[0]&&!bBraces2[1])
							{
								//fs1.getline(sLine2,128);
								bBraces2[0]=false;
								bBraces2[1]=false;
								bBraces2[2]=false;
								break;
							}
							// Set up the tokenizing
							char *l,*r;
							l=r=NULL;
							// Tokenize the line
							l=strtok(sLine2," ");
							r=strtok(NULL,"\n");
							// Parse and store based on tag
							if(!strnicmp("PATH",l,sizeof(char)*4))
							{
								if(r)
								{
									// Ok we might as well add the file name to this path, cause we will need it to open the
									std::string sTemp(r);
									sTemp += actb.sUsername;
									sTemp += ".uad";
									// Ok strip the name and store it. We need to make it all the same case for comparisons
									actb.sPath = sTemp;
								}
								else
								{
									actb.sPath="";
								}
								l=r=NULL;
								continue;
							}
							if(!strnicmp("FLAGS",l,sizeof(char)*5))
							{
								if(r)
								{
									// Ok strip the flags and store it. We need to make it all the same case for comparisons
									strlwr(r);
									actb.wFlags = cAccountClass::atol(r);	//<-- Uses internal conversion code
								}
								else
								{
									actb.wFlags= 0;
								}
								l=r=NULL;
								continue;
							}
						}
						fs1.getline(sLine,128);
					}
				}
				else
				{
					// Ok we might as well add the file name to this path, cause we will need it to open the
					std::string sTemp(sActPath);
					sTemp += actb.sUsername;
					sTemp += ".uad";
					// Ok strip the name and store it. We need to make it all the same case for comparisons
					actb.wFlags = cAccountClass::atol(0);	//<-- Uses internal conversion code
				}
			}
			else
			{
				// r was not valid when passed in. Deal with it here
			}
			// Ok we have finished with this access block clean up and continue processing
			bBraces2[0]=false;
			bBraces2[1]=false;
			bBraces2[2]=false;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("LASTIP",l,sizeof(char)*6))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
#ifdef __uox__
				actb.dwLastIP=inet_addr(r);
#else
				actb.dwLastIP=cAccountClass::atol(r);
#endif
			}
			else
			{
				actb.dwLastIP=0x00000000;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CONTACT",l,sizeof(char)*7))
		{
			// Ok strip the name and store it. We need to make it all the same case for comparisons
			if(r)
			{
				strlwr(r);
				actb.sContact=r;
			}
			else
			{
				actb.sContact="UNKNOWN";
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("TIMEBAN",l,sizeof(char)*7))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.wTimeBan = cAccountClass::atol(r);
			}
			else
			{
				actb.wTimeBan=0;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("PUBLIC",l,sizeof(char)*6))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				if(!strnicmp(r,"ON",sizeof(char)*2))
				{
					// Set the Public Flag on. Public implies that a users conatact information can be published on the web.
					actb.wFlags|=0x0004;
				}
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("XGMCLIENT",l,sizeof(char)*9))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				if(!strnicmp(r,"ON",sizeof(char)*2))
				{
					// Set the Public Flag on. Public implies that a users conatact information can be published on the web.
					actb.wFlags|=0x0002;
				}
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-1",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[0] = cAccountClass::atol(r);
			}
			else
			{
				actb.dwCharacters[4] = 0xffffffff;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-2",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[1] = cAccountClass::atol(r);
			}
			else
			{
				actb.dwCharacters[4] = 0xffffffff;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-3",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[2] = cAccountClass::atol(r);
			}
			else
			{
				actb.dwCharacters[4] = 0xffffffff;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-4",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[3] = cAccountClass::atol(r);
			}
			else
			{
				actb.dwCharacters[4] =0xffffffff;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-5",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[4] = cAccountClass::atol(r);
			}
			else
			{
				actb.dwCharacters[4] = 0xffffffff;
			}
			l=r=NULL;
			fs2.getline(sLine,128);
			continue;
		}
		fs2.getline(sLine,128);
		memset(&actb,0x00,sizeof(ACCOUNTSBLOCK));
	}
	//
	m_wHighestAccount=wAccountCount;
	// Make sure to close our open paths.
	fs2.close();
	fs1.close();
	// Ok the next thing were going to have to do is check paths, make new paths and write the UAD file back into the respective directory
	for(std::map<UI16,ACCOUNTSBLOCK>::iterator I = m_mapUsernameIDMap.begin();I!=m_mapUsernameIDMap.end();I++)
	{
		// Pull the data into a usable form
		ACCOUNTSBLOCK actbTemp = I->second;
		// Now we want to copy the files from the path to the new directory.
		char cDirSep=sActPath[sActPath.length()-1];
		std::string sNewPath(sActPath);
		if(cDirSep=='\\'||cDirSep=='/')
		{
			sNewPath += actbTemp.sUsername;
			sNewPath += "/";
		}
		else
		{
			sNewPath += "/";
			sNewPath += actbTemp.sUsername;
			sNewPath += "/";
		}
		// Create the accounts directory now that we have the username added. If it exists it doesn't matter.
		int nDummy = _mkdir(sNewPath.c_str(), 0777);
		if(nDummy<0)
		{
			// if directory exists then we just skip this.
#ifdef WIN32
			if(GetLastError()!=183)
			{
				m_mapUsernameIDMap.clear();
				m_mapUsernameMap.clear();
				return 0L;
			}
#else
			m_mapUsernameIDMap.clear();
			m_mapUsernameMap.clear();
			return 0L;
#endif
		}
		// Now open and copy the files. to their new location.
		if(!actbTemp.sPath.length())
		{
			// the file/path was not found so we should make an entry 
			std::string sOutFile(sNewPath);
			sOutFile += "/";
			sOutFile += actbTemp.sUsername;
			sOutFile += ".uad";
			std::fstream fsOut(sOutFile.c_str(),std::ios::out);
			if(!fsOut.is_open())
			{
				// we cannot open the out file.
				//fsIn.close();
				// free up any allocation of memory made.
				m_mapUsernameIDMap.clear();
				m_mapUsernameMap.clear();
				return 0L;
			}
			// Ok we have to write the new username.uad file in the directory
			cAccountClass::WriteUADHeader(fsOut,actbTemp);
			// Ok write out the characters and the charcter names if we know them
			for(int i=0;i<5;i++)
			{
#ifdef __UOX__
				fsOut << "CHARACTER-" << (i+1) << " 0x" << std::hex << (long)(actbTemp.dwCharacters[i] != NULL ? actbTemp.lpCharacters[i]->GetSerial() : (INVALIDSERIAL) ) << " [" << (char*)(actbTemp.dwCharacters[i] != NULL ? actbTemp.lpCharacters[i]->GetName() : "INVALID" ) << "]\n"; 
#else
				fsOut << "CHARACTER-" << (i+1) << " 0x" << std::hex << actbTemp.dwCharacters[i] << std::dec << " [UNKNOWN]\n"; 
#endif
			}
			// Close the files since we dont need them anymore
			fsOut.close();
		}
		else
		{
			std::fstream fsIn(actbTemp.sPath.c_str(),std::ios::in);
			std::string sOutFile(sNewPath);
			sOutFile += "/";
			sOutFile += actbTemp.sUsername;
			sOutFile += ".uad";
			std::fstream fsOut(sOutFile.c_str(),std::ios::out);
			if(!fsOut.is_open())
			{
				// we cannot open the out file.
				fsIn.close();
				// free up any allocation of memory made.
				m_mapUsernameIDMap.clear();
				m_mapUsernameMap.clear();
				return 0L;
			}
			// ok Lets just copy one file to the next.
			while(!fsIn.eof())
			{
				char szLine[128];
				fsIn.getline(szLine,127);
				fsOut << szLine << std::endl;
			}
			fsOut.close();
			fsIn.close();
		}
		// Now were done so we want to update the path in our records before we write them
		actbTemp.sPath=sNewPath;
		I->second = actbTemp;
	}			
	// Make a back up first, then Dump to new file,
	std::string sBUPath(sAccountsAdm);
	sBUPath += ".bu";
	rename(sAccountsAdm.c_str(),sBUPath.c_str());
	std::fstream fsOut(sAccountsAdm.c_str(),std::ios::out);
	if(!fsOut.is_open())
	{
		// Error opening output file. Free up any allocation of memory made.
		m_mapUsernameIDMap.clear();
		m_mapUsernameMap.clear();
		return 0L;
	}
	cAccountClass::WriteAccountsHeader(fsOut);
	for(std::map<UI16,ACCOUNTSBLOCK>::const_iterator CI = m_mapUsernameIDMap.begin();CI!=m_mapUsernameIDMap.end();CI++)
	{
		ACCOUNTSBLOCK actbTemp = CI->second;
		fsOut << "SECTION ACCOUNT " << std::dec << actbTemp.wAccountIndex << std::endl;
		fsOut << "{" << std::endl;
		fsOut << "NAME " << actbTemp.sUsername << std::endl;
		fsOut << "PASS " << actbTemp.sPassword << std::endl;
		fsOut << "FLAGS 0x" << std::hex << actbTemp.wFlags << std::dec << std::endl;
		fsOut << "PATH " << cAccountClass::PathFix(actbTemp.sPath) << std::endl;
		fsOut << "TIMEBAN 0x" << std::hex << actbTemp.wTimeBan << std::dec << std::endl;
		fsOut << "CONTACT " << actbTemp.sContact << std::endl;
		for(int ii=0;ii<5;ii++)
			fsOut << "CHARACTER-" << std::dec << (ii+1) << " 0x" << std::hex << actbTemp.dwCharacters[ii] << std::dec << std::endl;
		fsOut << "}" << std::endl << std::endl;
	}
	// Close the file
	fsOut.close();
	return wAccountCount;
}

//o--------------------------------------------------------------------------o
//|	Function			-	long cAccountClass::atol(string sValue)
//|	Date					-	12/8/2002 1:00:30 AM
//|	Developers		-	EviLDeD 
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from hex values.
//|									in the form of 0xFFEE to a more usable value.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
long cAccountClass::atol(std::string sValue)
{
	// Ok we need to run a conversion from hex to dec for storage.
	if(!sValue.length())
	{
		return 0l;
	}
	// Declare variables
	unsigned long lnLong = 0L;
	if(!strnicmp(sValue.c_str(),"0X",sizeof(char)*2)||!strnicmp(sValue.c_str(),"X",sizeof(char)*1))
	{
		// Hexidecimal value passed in. Formate either 0x, or x (Is not case sensitive)
		std::istringstream ssHold(sValue.c_str()+2);
		ssHold >> std::hex >> lnLong >> std::dec;
	}
	else if(!strnicmp(sValue.c_str(),"0",sizeof(char)*1))
	{
		// Octal value was passed in. Not sure why we would ever do this, but for those willing. ;)
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::oct >> lnLong >> std::dec;
	}
	else
	{
		// For some reason the value passed in to be converted was a decimal number.
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::dec >> lnLong;
	}
	return lnLong;
}

//o--------------------------------------------------------------------------o
//|	File					-	long cAccountClass::atol(const char *lpszValue)
//|	Date					-	12/8/2002 1:07:00 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
long cAccountClass::atol(const char *lpszValue)
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0L;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return atol(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function			-	string& cAccountClass::PathFix(string sPath)
//|	Date					-	12/12/2002 3:04:36 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTea,
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Change all '\'s to '/'s
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
std::string& cAccountClass::PathFix(std::string& sPath)
{
	// Loop through each char and replace the '\' to '/'
	for(int i=0;i<sPath.length();i++)
	{
		if(sPath[i]=='\\')
			sPath[i]='/';
	}
	return sPath;
}

//o--------------------------------------------------------------------------o
//|	Function			-	WORD cAccountClass::AddAccount(std::string sUsername, std::string sPassword, std::string sContact)
//|									WORD cAccountClass::AddAccount(std::string sUsername, std::string sPassword, std::string sContact,WORD wAttributes)
//|	Date					-	12/12/2002 11:15:20 PM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	This function creates in memory as well as on disk a new
//|									account that will allow access to the server from external
//|									clients. The basic goal of this function is to create a
//|									new account record with no characters. Because this function
//|									may be called from in game a hard copy will be made as well
//|									instead of waiting for a save to make the changes perminent.
//|									
//|									sUsername:   Name of the account
//|									sPassword:   Password of the account
//|									sContact:    Historically this is the valid email address 
//|									             for this account
//|									wAttributes: What attributes should this account start with
//o--------------------------------------------------------------------------o
//|	Returns				-	[WORD] Containing the account number of new accounts or 0
//o--------------------------------------------------------------------------o
//|	NOTE					-	This function does NOT add this account to the accounts
//|									map. This function ONLY creates the directories, and enteries
//|									required to load these new accounts. The idea was not to 
//|									have to rely on the accounts in memory, so that accounts
//|									that are added, are added immediatly to the hard copy.
//|	NOTE					-	For any new accounts to be loaded into the internal accounts
//|									map structure, accounts must be reloaded!
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
UI16 cAccountClass::AddAccount(std::string sUsername, std::string sPassword, std::string sContact,UI16 wAttributes)
{
	// First were going to make sure that the needed fields are sent in with at least data
	if(sUsername.length()<4||sUsername.length()<4||sPassword.length()<4||sPassword.length()<4)
	{
		// Username, and or password must both be 4 characters or more in length
		return 0x0000;
	}
	// Next thing were going to do is make sure there isn't a duplicate username.
	if(cAccountClass::isUser(sUsername))
	{
		// This username is already on the list.
		return 0x0000;
	}
	// If we get here then were going to create a new account block, and create all the needed directories and files.
	ACCOUNTSBLOCK actbTemp;
	memset(&actbTemp,0x00,sizeof(ACCOUNTSBLOCK));
	// Build as much of the account block that we can right now.
	actbTemp.sUsername=sUsername;
	actbTemp.sPassword=sPassword;
	actbTemp.sContact=sContact;
	actbTemp.wFlags=wAttributes;
	actbTemp.wTimeBan=0;
	actbTemp.dwLastIP=0;
	for(int ii=0;ii<5;ii++)
	{
		actbTemp.lpCharacters[ii]=NULL;
		actbTemp.dwCharacters[ii]=0xffffffff;
	}
	// Ok we have everything except the path to the account dir, so make that now
	std::string sTempPath(m_sAccountsDirectory);
	if(sTempPath[sTempPath.length()-1]=='\\'||sTempPath[sTempPath.length()-1]=='/')
	{
		char szTempBuff[33];
		strncpy(szTempBuff,sUsername.c_str(),sizeof(char)*30);
		strlwr(szTempBuff);
		sTempPath += szTempBuff;
		sTempPath += "/";
		cAccountClass::PathFix(sTempPath);
		actbTemp.sPath=sTempPath;
	}
	else
	{		
		char szTempBuff[33];
		strncpy(szTempBuff,sUsername.c_str(),sizeof(char)*30);
		strlwr(szTempBuff);
		sTempPath += "/";
		sTempPath += szTempBuff;
		sTempPath += "/";
		cAccountClass::PathFix(sTempPath);
		actbTemp.sPath=sTempPath;
	}
	// Ok now that we got here we need to make the directory, and create the username.uad file
	int nDummy=_mkdir(actbTemp.sPath.c_str(), 0777);
	if(nDummy<0)
	{
		// if directory exists then we just skip this.
#ifdef WIN32
		if(GetLastError()!=183)
		{
			return 0L;
		}
#else
		return 0L;
#endif
	}
	// Ok now thats finished. We need to do one last thing. Create the username.uad file in the account directory
	std::string sUsernameUADPath(actbTemp.sPath);
	if(sUsernameUADPath[sUsernameUADPath.length()-1]=='\\'||sUsernameUADPath[sUsernameUADPath.length()-1]=='/')
	{
		sUsernameUADPath += actbTemp.sUsername;
		sUsernameUADPath += ".uad";
	}
	else
	{
		sUsernameUADPath += "/";
		sUsernameUADPath += actbTemp.sUsername;
		sUsernameUADPath += ".uad";
	}
	// Fix the paths to make sure that all the characters are unified
	cAccountClass::PathFix(sUsernameUADPath);
	// Open the file in APPEND to end mode.
	std::fstream fsAccountsUAD(sUsernameUADPath.c_str(),std::ios::out|std::ios::trunc);
	if(!fsAccountsUAD.is_open())
	{
		// Ok we were unable to open the file so this user will not be added.
		return 0x0000;
	}
	// were using the ID due to its numbering, it will always show the account blocks in ID order least to greatest.
	actbTemp.wAccountIndex = m_wHighestAccount + 1;
	// Ok we have to write the new username.uad file in the directory
#ifdef cVersionClass
	fsAccountsUAD << "//AI3.0" << "-UV" << CVC.GetVersion() << "-BD" << CVC.GetBuild() << "-DS" << time(NULL) << "-ED" << CVC.GetRealBuild() << "\n";
#else
	fsAccountsUAD << "//AI3.0" << "-UV0.0-BD0-DS" << time(NULL) << "-ED0\n";
#endif
	fsAccountsUAD << "//------------------------------------------------------------------------------" << std::endl;
	fsAccountsUAD << "// UAD Path: " << actbTemp.sPath << std::endl;//"\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------" << std::endl;
	fsAccountsUAD << "// UOX3 uses this file to store any extra administration info\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------\n";
	fsAccountsUAD << "ID " << actbTemp.wAccountIndex << "\n";
	fsAccountsUAD << "BANTIME " << std::hex << actbTemp.wTimeBan << std::dec << "\n";
	fsAccountsUAD << "LASTIP " << (int)(actbTemp.dwLastIP>>24) << "." << (int)(actbTemp.dwLastIP>>16) << "." << (int)(actbTemp.dwLastIP>>8) << "." << (int)(actbTemp.dwLastIP%256) << "\n";
	fsAccountsUAD << "COMMENT " << actbTemp.sContact << "\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------\n";
	// Ok write out the characters and the charcter names if we know them
	for(int i=0;i<5;i++)
	{
#ifdef __UOX__
		fsAccountsUAD << "CHARACTER-" << (i+1) << " 0x" << std::hex << (actbTemp.dwCharacters[i] != NULL ? actbTemp.lpCharacters[i]->GetSerial() : (INVALIDSERIAL) ) << " [" << (char*)(actbTemp.dwCharacters[i] != NULL ? actbTemp.lpCharacters[i]->GetName() : "INVALID" ) << "]\n"; 
#else
		fsAccountsUAD << "CHARACTER-" << (i+1) << " 0x" << std::hex << actbTemp.dwCharacters[i] << " [UNKNOWN]\n"; 
#endif
	}
	// Close the files since we dont need them anymore
	fsAccountsUAD.close();
	// Ok now we can add this record to the accounts.adm file
	std::string sAccountsADMPath(m_sAccountsDirectory);
	if(sAccountsADMPath[sAccountsADMPath.length()-1]=='\\'||sAccountsADMPath[sAccountsADMPath.length()-1]=='/')
		sAccountsADMPath += "accounts.adm";
	else
		sAccountsADMPath += "/accounts.adm";
	// Open the file in APPEND to end mode.
	std::fstream fsAccountsADM(sAccountsADMPath.c_str(),std::ios::out|std::ios::app);
	if(!fsAccountsADM.is_open())
	{
		// Ok we were unable to open the file so this user will not be added.
		return 0x0000;
	}
	// ok since the EOF is not used at all anyhow, we are just going to write our block and close.
	fsAccountsADM << "SECTION ACCOUNT " << (int)actbTemp.wAccountIndex << std::endl;
	fsAccountsADM << "{" << std::endl;
	// Write the actual block data to the file.
	fsAccountsADM << "NAME " << actbTemp.sUsername << std::endl;
	fsAccountsADM << "PASS " << actbTemp.sPassword << std::endl;
	fsAccountsADM << "FLAGS 0x" << std::hex << actbTemp.wFlags << std::dec << std::endl;
	fsAccountsADM << "PATH " << actbTemp.sPath << std::endl;
	fsAccountsADM << "TIMEBAN " << actbTemp.wTimeBan << std::endl;
	fsAccountsADM << "CONTACT " << actbTemp.sContact << std::endl;
	for(int jj=0;jj<5;jj++)
		fsAccountsADM << "CHARACTER-" << std::dec << (jj+1) << " 0xffffffff " << "[UNKNOWN]" << std::endl;
	// Finish with the block and add a blank space.
	fsAccountsADM << "}" << std::endl << std::endl;
	fsAccountsADM.close();
	// Ok might be a good thing to add this account to the map(s) now.
	m_mapUsernameIDMap[actbTemp.wAccountIndex]=actbTemp;
	m_mapUsernameMap[actbTemp.sUsername]=actbTemp;
	m_wHighestAccount=actbTemp.wAccountIndex;
	// Return to the calling function
	return (UI16)m_mapUsernameIDMap.size();
}


//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::isUser(string sUsername)
//|	Date					-	12/16/2002 12:09:13 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	The only function that this function member servers is to 
//|									return a response based on the existance of the specified
//|									username.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::isUser(std::string sUsername)
{
	MAPUSERNAME_ITERATOR I;
	// Call into the map to see if this username exists. but first copy it into a local string and make it lowercase 
	char szTempUsername[32];
	strncpy(szTempUsername,sUsername.c_str(),sizeof(char)*32);
	std::string sTempUsername = strlwr(szTempUsername);
	I=m_mapUsernameMap.find(sTempUsername);
	if(I==m_mapUsernameMap.end())
		return false;
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function			-	WORD cAccountClass::size()
//|	Date					-	12/17/2002 3:35:31 PM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the current count of accounts that are currently
//|									stored in the account map. 
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
UI16 cAccountClass::size()
{
	return m_mapUsernameMap.size();
}

//o--------------------------------------------------------------------------o
//|	Function			-	UI16 cAccountClass::Load()
//|	Date					-	12/17/2002 4:00:47 PM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Load the internal accounts structure from the accounts.adm
//|									file. Due to the nature of this function we will empty the
//|									previous contents of the account map, and reload them.
//|									
//|	Modification	-	1/20/2003 - Forgot to put in place the lookup to see if 
//|									accounts need to be updated to the v3 format. Should only
//|									need to check the first line of the accounts.adm file.
//|									NOTE: Do not remove this line if you wish to convert 
//|									properly. Without the first line this function will assume
//|									that the accounts file is a v2 format file(UOX3 v0.97.0).
//|									
//|	Modification	-	1/20/2003 - Added support for the DTL libs, and create a
//|									general SQL query for MSAccess, MSSQL, and mySQL
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
UI16 cAccountClass::Load(void)
{
	UI16 wAccountsCount=0;
	UI16 wHighestAccount=0;
	// Clear out the previous map contents before we start
	m_mapUsernameMap.clear();
	m_mapUsernameIDMap.clear();
#ifndef __UOX3_DTL__
	// Now we can load the accounts file in and re fill the map.
	std::string sAccountsADM(m_sAccountsDirectory);
	sAccountsADM += (m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='\\'||m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='/')?"accounts.adm":"/accounts.adm";
	cAccountClass::PathFix(sAccountsADM);
	// Check to see what version the current accounts.adm file is.
	char sLine[129];
	std::fstream fsAccountsADMTest(sAccountsADM.c_str());
	if(!fsAccountsADMTest.is_open())
	{
		// we were unable to load any accounts
		return 0x0000;
	}
	fsAccountsADMTest.getline(sLine,128);
	// Check to see if this is a V3 file.
	if(!strstr(sLine,"AV3.0"))
	{
		// Nope, not a v3 file.. so we should attempt a convert
		CreateAccountSystem();
		return 0x0000;
	}
	fsAccountsADMTest.close();
	// OK now we can open the file.
	std::fstream fsAccountsADM(sAccountsADM.c_str());
	if(!fsAccountsADM.is_open())
	{
		// we were unable to load any accounts
		return 0x0000;
	}
	// We need to read from the stream once before entering the loop
	fsAccountsADM.getline(sLine,128);
	// Ok start the loop and process
	bool bBraces[3]={false,false,false};
	bool bBraces2[3]={false,false,false};
	ACCOUNTSBLOCK actb;
	UI16 wAccountID=0x0000;
	UI16 wAccessID=0x0000;
	UI16 wAccountCount=0x0000;
	memset(&actb,0x00,sizeof(ACCOUNTSBLOCK));
	m_wHighestAccount=0x0000;
	while(!fsAccountsADM.eof())
	{
		// Process each Line read in from the accounts.adm file. Handle Comments first
		if(sLine[0]=='\\'||sLine[0]==';'||sLine[0]=='/'||sLine[0]=='\''||sLine[0]==0x13||sLine[0]==0x00)
		{
			fsAccountsADM.getline((char*)&sLine,128);
			continue;
		}
		// Keep track of the section account lines. There is nothing to process till at least one of these has been read
		if(!strnicmp("SECTION ACCOUNT",sLine,sizeof(char)*15))
		{
			// Increment the account acount
			wAccountCount+=1;
			// Ok the section block was found, Tokenize the string to get the account #
			char *l,*r;
			l=r=NULL;
			// Tokenize the line. I decided instead of wasting more pointer storage would just use the same allocated storage
			l=strtok(sLine," ");		
			l=strtok(NULL," ");
			r=strtok(NULL,"\n");
			actb.wAccountIndex=wAccountID=atoi(r);
			// Scan for hiegest account. Needed for additional accounts.
			if(actb.wAccountIndex>m_wHighestAccount)
				m_wHighestAccount=actb.wAccountIndex;
			// Ok, we have parsed out the account ID, Set bBraces[2] to true to allow block reading
			bBraces[0]=false;
			bBraces[1]=false;
			bBraces[2]=true;
			// Get the next line and continue
			fsAccountsADM.getline((char*)&sLine,128);
			continue;
		}
		// Fail safe. If bBraces[2] isn't set there is no need to even run this code
		if(!bBraces[2])
		{
			// Make sure the get the next line to process.
			fsAccountsADM.getline((char*)&sLine,128);
			continue;
		}
		// If we get here then were reading a block now. Check for the openning brace.
		if(sLine[0]=='{'&&!bBraces[0]&&!bBraces[1])
		{
			bBraces[0]=true;
			fsAccountsADM.getline((char*)&sLine,128);
			continue;
		}
		if(sLine[0]=='{'&&bBraces[0]||sLine[0]=='{'&&bBraces[1])
		{
			fsAccountsADM.close();
			return 0x0000;
		}
		// Loop reading this block till the end brace is encountered.
		if(sLine[0]=='}'&&bBraces[0]&&!bBraces[1])
		{
			// Ok we shoud shove this into the map(s) for use later
			m_mapUsernameMap[actb.sUsername]=actb;
			m_mapUsernameIDMap[wAccountID]=actb;
			// Ok we have finished with this access block clean up and continue processing
			bBraces[0]=false;
			bBraces[1]=false;
			bBraces[2]=false;
			fsAccountsADM.getline((char*)&sLine,128);
			continue;
		}
		// Set up the tokenizing
		char *l,*r;
		l=r=NULL;
		// Tokenize the line
		l=strtok(sLine," ");
		r=strtok(NULL,"\n");
		// Parse and store based on tag
		if(!strnicmp("NAME",l,sizeof(char)*4))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.sUsername =r;
			}
			else
			{
				actb.sUsername="ERROR";
			}
			l=r=NULL;
			fsAccountsADM.getline((char*)&sLine,128);
			continue;
		}
		else if(!strnicmp("PASS",l,sizeof(char)*4))
		{
			if(r)
			{
				// Ok strip the password and store it.
				actb.sPassword=r;
				l=r=NULL;
				// Now we have to parse the crap in access.adm. Were not gonna look at much format just the 2 tags we need.
			}
			else
			{
				// r was not valid when passed in. Deal with it here
			}
			// Ok we have finished with this access block clean up and continue processing
			bBraces2[0]=false;
			bBraces2[1]=false;
			bBraces2[2]=false;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("FLAGS",l,sizeof(char)*5))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.wFlags = cAccountClass::atol(r);
			}
			else
			{
				actb.wFlags=0x0004;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("PATH",l,sizeof(char)*4))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				actb.sPath=r;
			}
			else
			{
				actb.sPath="ERROR";
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("TIMEBAN",l,sizeof(char)*7))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.wTimeBan = cAccountClass::atol(r);
			}
			else
			{
				actb.wTimeBan=0;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CONTACT",l,sizeof(char)*7))
		{
			// Ok strip the name and store it. We need to make it all the same case for comparisons
			if(r)
			{
				strlwr(r);
				actb.sContact=r;
			}
			else
			{
				actb.sContact="UNKNOWN";
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-1",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[0] = cAccountClass::atol(r);
				actb.lpCharacters[0] = calcCharObjFromSer(actb.dwCharacters[0]);
				if(actb.lpCharacters[0]!=NULL)
					actb.lpCharacters[0]->SetAccount(actb);
			}
			else
			{
				actb.dwCharacters[0] = -1;
				actb.lpCharacters[0] = NULL;
				actb.wAccountIndex=AB_INVALID_ID;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-2",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[1] = cAccountClass::atol(r);
				actb.lpCharacters[1] = calcCharObjFromSer(actb.dwCharacters[1]);
				if(actb.lpCharacters[1]!=NULL)
					actb.lpCharacters[1]->SetAccount(actb);
			}
			else
			{
				actb.dwCharacters[1] = -1;
				actb.lpCharacters[1] = NULL;
				actb.wAccountIndex=AB_INVALID_ID;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-3",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[2] = cAccountClass::atol(r);
				actb.lpCharacters[2] = calcCharObjFromSer(actb.dwCharacters[2]);
				if(actb.lpCharacters[2]!=NULL)
					actb.lpCharacters[2]->SetAccount(actb);
			}
			else
			{
				actb.dwCharacters[2] = -1;
				actb.lpCharacters[2] = NULL;
				actb.wAccountIndex=AB_INVALID_ID;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-4",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[3] = cAccountClass::atol(r);
				actb.lpCharacters[3] = calcCharObjFromSer(actb.dwCharacters[3]);
				if(actb.lpCharacters[3]!=NULL)
					actb.lpCharacters[3]->SetAccount(actb);
			}
			else
			{
				actb.dwCharacters[3] = -1;
				actb.lpCharacters[3] = NULL;
				actb.wAccountIndex=AB_INVALID_ID;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		else if(!strnicmp("CHARACTER-5",l,sizeof(char)*11))
		{
			if(r)
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				strlwr(r);
				actb.dwCharacters[4] = cAccountClass::atol(r);
				actb.lpCharacters[4] = calcCharObjFromSer(actb.dwCharacters[4]);
				if(actb.lpCharacters[4]!=NULL)
					actb.lpCharacters[4]->SetAccount(actb);
			}
			else
			{
				actb.dwCharacters[4] = -1;
				actb.lpCharacters[4] = NULL;
				actb.wAccountIndex=AB_INVALID_ID;
			}
			l=r=NULL;
			fsAccountsADM.getline(sLine,128);
			continue;
		}
		fsAccountsADM.getline(sLine,128);
		memset(&actb,0x00,sizeof(ACCOUNTSBLOCK));
	}
#else
	// Clear out the vector before using it
	m_vecDTLAccountVector.clear();
	// Make sure to implement a configurable entery for this table name later
	dtl::DBView<cDBAccountClass> dbvView("T_ACCOUNTS");
	dtl::DBView::select_iterator I = dbvView.begin();
	for(;I!=dbvView.end();I++)
	{
		// Push every valid iterator into the internal vector for transfer to the accounts maps.
		m_vecDTLAccountVector	.push_back(*I);
	}
	// Crawl over the vector, inserting records from the DBView and put them into the maps
	std::vector<cDBAccountClass>::iterator J=m_vecDTLAccountVector.begin();
	for(;J!=m_vecDTLAccountVector.end();J++)
	{
		// Ok push each record into the maps
		ACCOUNTSBLOCK actbTemp;
		actbTemp.sUsername=J->sUsername;
		actbTemp.sPassword=J->sPassword;
		actbTemp.sPath=J->sPath;
		actbTemp.sContact=J->sComment;
		actbTemp.wAccountIndex=(UI16)J->dwAccountID;
		// Uncomment this when support has been implemented
		// actbTemp.dwCommenetID=J->dwCommentID;
		//
		actbTemp.wFlags=(UI16)J->dwFlags;
		actbTemp.dwInGame=J->dwInGame;
		actbTemp.dwLastIP=J->dwLaspIP;
		actbTemp.dwCharacters[0]=J->dwCharacter1;
		actbTemp.dwCharacters[1]=J->dwCharacter2;
		actbTemp.dwCharacters[2]=J->dwCharacter3;
		actbTemp.dwCharacters[3]=J->dwCharacter4;
		actbTemp.dwCharacters[4]=J->dwCharacter5;
		// Ok this should be complete as supported, so push it into the maps
		m_mapUsernameIDMap[actbTemp.wAccountIndex]=actbTemp;
		m_mapUsernameMap[actbTemp.sUsername]=actbTemp;
	}
#endif
	// Return the number of accounts loaded
	return m_mapUsernameMap.size();
}
//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::TransCharacter(WORD wSAccountID,WORD wSSlot,WORD wDAccountID)
//|	Date					-	1/7/2003 5:39:12 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Transfer a character from a known slot on a specified 
//|									account, to a new account and slot.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::TransCharacter(UI16 wSAccountID,UI16 wSSlot,UI16 wDAccountID)
{
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wSAccountID);
	if(I==m_mapUsernameIDMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	ACCOUNTSBLOCK actbID;
	actbID = I->second;
	// Ok now we need to get the matching username map 
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.
	ACCOUNTSBLOCK actbName;
	actbName = J->second;
	// ok we will check to see if there is a valid char in source slot, if not we will return
	if(actbID.dwCharacters[wSSlot]==-1||actbID.dwCharacters[wSSlot]==0xffffffff)
		return false;
	//
	MAPUSERNAMEID_ITERATOR II;
	II = m_mapUsernameIDMap.find(wDAccountID);
	if(II==m_mapUsernameIDMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	ACCOUNTSBLOCK actbIID;
	actbIID = II->second;
	// Ok now we need to get the matching username map 
	MAPUSERNAME_ITERATOR JJ;
	JJ = m_mapUsernameMap.find(actbIID.sUsername);
	if(JJ==m_mapUsernameMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.
	ACCOUNTSBLOCK actbJName;
	actbJName = JJ->second;
	// ok at this point I/II = SourceID, and DestID and J/JJ SourceName/DestName
	if(cAccountClass::AddCharacter(wDAccountID,actbID.dwCharacters[wSSlot],actbID.lpCharacters[wSSlot]))
	{
		// OK the character was added, need to remove it from the source.
		cAccountClass::DelCharacter(wSAccountID,(int)wSSlot);
		cAccountClass::Save();
		return true;
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::AddCharacter(WORD accountid, CChar *object)
//|									bool cAccountClass::AddCharacter(WORD accountid, VOID *object)
//|	Date					-	12/18/2002 2:09:04 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Add a character object to the in memory storage. 
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::AddCharacter(UI16 wAccountID, CChar *lpObject)
{
	// Make sure that the lpObject pointer is valid
	if(lpObject==NULL)
		return false;
	// Ok we need to do is get see if this account id exists
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wAccountID);
	if(I==m_mapUsernameIDMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	ACCOUNTSBLOCK actbID;
	actbID = I->second;
	// Ok now we need to get the matching username map 
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.
	ACCOUNTSBLOCK actbName;
	actbName = J->second;
	// ok now that we have both of our account blocks we can update them. We will use teh first empty slot for this character
	bool bExit=false;
	for(int i=0;i<5;i++)
	{
		if(actbID.dwCharacters[i]==-1&&actbID.dwCharacters[i]==0xffffffff&&actbName.dwCharacters[i]==-1&&actbName.dwCharacters[i]==0xffffffff)
		{
			// ok this slot is empty, we will stach this character in this slot
			actbID.lpCharacters[i]=lpObject;
			actbID.dwCharacters[i]=lpObject->GetID();
			actbName.lpCharacters[i]=lpObject;
			actbName.dwCharacters[i]=lpObject->GetID();
			// we do not need to continue throught this loop anymore.
			bExit=true;
			break; 
		}
	}
	// If we were successfull then we return true
	if(bExit)
	{
		// make sure to put the values back into the maps corrected.
		I->second = actbID;
		J->second = actbName;
		cAccountClass::Save();
		return true;
	}
	return false;
}
//
bool cAccountClass::AddCharacter(UI16 wAccountID,UI32 dwCharacterID, CChar *lpObject)
{
	// Make sure that the lpObject pointer is valid
	if(lpObject==NULL)
		return false;
	// Ok we need to do is get see if this account id exists
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wAccountID);
	if(I==m_mapUsernameIDMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	ACCOUNTSBLOCK actbID;
	actbID = I->second;
	// Ok now we need to get the matching username map 
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.
	ACCOUNTSBLOCK actbName;
	actbName = J->second;
	// ok now that we have both of our account blocks we can update them. We will use teh first empty slot for this character
	bool bExit=false;
	for(int i=0;i<5;i++)
	{
		if(actbID.dwCharacters[i]==-1&&actbID.dwCharacters[i]==0xffffffff&&actbName.dwCharacters[i]==-1&&actbName.dwCharacters[i]==0xffffffff)
		{
			// ok this slot is empty, we will stach this character in this slot
			actbID.lpCharacters[i]=lpObject;
			actbID.dwCharacters[i]=dwCharacterID;
			actbName.lpCharacters[i]=lpObject;
			actbName.dwCharacters[i]=dwCharacterID;
			// we do not need to continue throught this loop anymore.
			bExit=true;
			break; 
		}
	}
	// If we were successfull then we return true
	if(bExit)
	{
		// make sure to put the values back into the maps corrected.
		I->second = actbID;
		J->second = actbName;
		cAccountClass::Save();
		return true;
	}
	return false;
}
//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::ModAccount(std::string sUsername,DWORD dwFlags,ACCOUNTSBLOCK &actbBlock)
//|									bool cAccountClass::ModAccount(WORD wAccountID,DWORD dwFlags,ACCOUNTSBLOCK &actbBlock)
//|	Date					-	1/7/2003 7:15:58 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Make modifications to an existing account. Currently a 
//|									flag based system is used to indicate the validity of a
//|									field in the ACCOUNTSBLOCK. A valid field in the 
//|									ACCOUNTSBLOCK structure will be used to update the current
//|									Account block information found in the maps.
//|	
//|	Supported			-	AB_USERNAME
//|									AB_PASSWORD		
//|									AB_FLAGS
//|									AB_PATH
//|									AB_TIMEBAN
//|									AB_CONTACT
//|									AB_CHARACTER1
//|									AB_CHARACTER2
//|									AB_CHARACTER3
//|									AB_CHARACTER4
//|									AB_CHARACTER5
//|									
//|									The values can be OR'd together to specify multiple valid
//|									fields. 
//|									
//|									IE:. ModAccount(0,AB_USERNAME|AB_CONTACT,actbBlock);
//|									
//|									In the above example both the username, and contact fields
//|									would be modified for Account 0 when this function is 
//|									completed.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::ModAccount(std::string sUsername,UI32 dwFlags,ACCOUNTSBLOCK &actbBlock)
{
	// Ok we need to get the name block to get the accounts id
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(sUsername);
	if(J==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbName;
	actbName=J->second;
	// Ok we have the id, so call the actual function to do the work
	return cAccountClass::ModAccount(actbName.wAccountIndex,dwFlags,actbBlock);
}
//
bool cAccountClass::ModAccount(UI16 wAccountID,UI32 dwFlags,ACCOUNTSBLOCK &actbBlock)
{
	// Ok we need to get the ID block again as it wasn't passed in
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wAccountID);
	if(I==m_mapUsernameIDMap.end())
		return false;
	ACCOUNTSBLOCK actbID;
	actbID=I->second;
	// Now we need to get the matching username map entry
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbName;
	actbName=J->second;
	// Ok find out which fields are valid and make the changes.
	for(int i=0;i<11;i++)
	{
		if(dwFlags&AB_USERNAME)
		{
			actbID.sUsername=actbBlock.sUsername;
			actbName.sUsername=actbBlock.sUsername;
		}
		else if(dwFlags&AB_PASSWORD)
		{
			actbID.sPassword=actbBlock.sPassword;
			actbName.sPassword=actbBlock.sPassword;
		}
		else if(dwFlags&AB_FLAGS)
		{
			actbID.wFlags=actbBlock.wFlags;
			actbName.wFlags=actbBlock.wFlags;
		}
		else if(dwFlags&AB_PATH)
		{
			actbID.sPath=actbBlock.sPath;
			actbName.sPath=actbBlock.sPath;
		}
		else if(dwFlags&AB_TIMEBAN)
		{
			actbID.wTimeBan=actbBlock.wTimeBan;
			actbName.wTimeBan=actbBlock.wTimeBan;
		}
		else if(dwFlags&AB_CONTACT)
		{
			actbID.sContact=actbBlock.sContact;
			actbName.sContact=actbBlock.sContact;
		}
		else if(dwFlags&AB_CHARACTER1)
		{
			actbID.dwCharacters[0]=actbBlock.dwCharacters[0];
			actbName.dwCharacters[0]=actbBlock.dwCharacters[0];
			actbID.lpCharacters[0]=actbBlock.lpCharacters[0];
			actbName.lpCharacters[0]=actbBlock.lpCharacters[0];
		}
		else if(dwFlags&AB_CHARACTER2)
		{
			actbID.dwCharacters[1]=actbBlock.dwCharacters[1];
			actbName.dwCharacters[1]=actbBlock.dwCharacters[1];
			actbID.lpCharacters[1]=actbBlock.lpCharacters[1];
			actbName.lpCharacters[1]=actbBlock.lpCharacters[1];
		}
		else if(dwFlags&AB_CHARACTER3)
		{
			actbID.dwCharacters[2]=actbBlock.dwCharacters[2];
			actbName.dwCharacters[2]=actbBlock.dwCharacters[2];
			actbID.lpCharacters[2]=actbBlock.lpCharacters[2];
			actbName.lpCharacters[2]=actbBlock.lpCharacters[2];
		}
		else if(dwFlags&AB_CHARACTER4)
		{
			actbID.dwCharacters[3]=actbBlock.dwCharacters[3];
			actbName.dwCharacters[3]=actbBlock.dwCharacters[3];
			actbID.lpCharacters[3]=actbBlock.lpCharacters[3];
			actbName.lpCharacters[3]=actbBlock.lpCharacters[3];
		}
		else if(dwFlags&AB_CHARACTER5)
		{
			actbID.dwCharacters[4]=actbBlock.dwCharacters[4];
			actbName.dwCharacters[4]=actbBlock.dwCharacters[4];
			actbID.lpCharacters[4]=actbBlock.lpCharacters[4];
			actbName.lpCharacters[4]=actbBlock.lpCharacters[4];
		}
	}
	// Ok put the data back into the map(s)
	I->second=actbID;
	J->second=actbName;
	cAccountClass::Save();
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::clear(void)
//|	Date					-	12/18/2002 2:24:07 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	This function is used to clear out all enteries contained
//|									in both the Username, and UsernameID map structures.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::clear(void)
{
	// First we should check to make sure that we can even use the objects, or they are not already cleared
	try
	{
		if(!m_mapUsernameMap.size()||!m_mapUsernameIDMap.size())
		{
			return false;
		}
		// ok clear the map
		m_mapUsernameIDMap.clear();
		m_mapUsernameMap.clear();
		m_wHighestAccount=0x0000;
	}
	catch(...)
	{
		return false;
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::DelAccount(std::string sUsername)
//|									bool cAccountClass::DelAccount(WORD wAccountID)
//|	Date					-	12/18/2002 2:56:34 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Remove an account from the internal account storage map.
//|									At this point this function will only remove the account
//|									block from the accounts.adm and not the physical files
//|									on the storage medium.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::DelAccount(std::string sUsername)
{
	// Ok were just going to get the ID number for this account and make the ID function do all the work
	MAPUSERNAME_ITERATOR I;
	I = m_mapUsernameMap.find(sUsername);
	if(I==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbTemp;
	actbTemp = I->second;
	// Ok lets do the work now
	return cAccountClass::DelAccount(actbTemp.wAccountIndex);
}
//o--------------------------------------------------------------------------o
bool cAccountClass::DelAccount(UI16 wAccountID)
{
	// Ok we need to get the ID block again as it wasn't passed in
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wAccountID);
	if(I==m_mapUsernameIDMap.end())
		return false;
	ACCOUNTSBLOCK actbID;
	actbID=I->second;
	// Now we need to get the matching username map entry
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbName;
	actbName=J->second;
	// Before we delete this account we need to spin the characters, and list them in the orphan.adm file
	std::string sTempPath(m_sAccountsDirectory);
	if(sTempPath[sTempPath.length()-1]=='\\'||sTempPath[sTempPath.length()-1]=='/')
		sTempPath += "orphans.adm";
	else
		sTempPath += "/orphans.adm";
	// First lets see if the file exists.
	bool bOrphanHeader=false;
	std::fstream fsOrphansADMTest(sTempPath.c_str(),std::ios::in);
	if(!fsOrphansADMTest.is_open())
		bOrphanHeader=true;
	fsOrphansADMTest.close();
	// ok open the stream for append and add this record to the end of the file
	std::fstream fsOrphansADM(sTempPath.c_str(),std::ios::out|std::ios::app);
	if(!fsOrphansADM.is_open())
		return false;
	// If this is a new file then we need to write the header first
	if(bOrphanHeader)
	{
		cAccountClass::WriteOrphanHeader(fsOrphansADM);
	}
	// Spin the characters here
	for(int jj=0;jj<5;jj++)
	{
		// If this slot is 0xffffffff or (-1) then we dont need to put it into the orphans.adm
		if(actbID.dwCharacters[jj]!=0xffffffff&&actbID.dwCharacters[jj]!=(LONG)(-1))
		{
			// Ok build then write what we need to the file
			fsOrphansADM << actbID.sUsername << "=0x" << std::hex << actbID.dwCharacters[jj]  << std::endl;
#ifdef __uox__
			fsOrphansADM << "," << actbID.lpCharacters[jj]->GetName() << "," << actbID.lpCharacters[jj]->GetX() << "," << actbID.lpCharacters[jj]->GetY() << "," actbID.lpCharacters[jj]->GetZ() << std::endl;
#endif
		}
	}
	fsOrphansADM.close();
	// Ok we have both the map iterators pointing to the right place. Erase these enteries
	m_mapUsernameIDMap.erase(I);
	m_mapUsernameMap.erase(J);
	// Just a means to show that an accounts has been removed when looking at the directories
	char szDirName[40];
	char szDirPath[MAX_PATH];
	memset(szDirName,0x00,sizeof(char)*40);
	memset(szDirPath,0x00,sizeof(char)*MAX_PATH);
	// Ok copy only the username portion to this so we can build a correct rename path
	strcpy(szDirName,&actbID.sPath[actbID.sPath.length()-actbID.sUsername.length()-1]);
	strncpy(szDirPath,actbID.sPath.c_str(),actbID.sPath.length()-actbID.sUsername.length()-1);
	std::string sNewDir(szDirPath);
	sNewDir += "_";
	sNewDir += szDirName;
	rename(actbID.sPath.c_str(),sNewDir.c_str());
	// We have to run a save to make sure that the accoung it removed.
	cAccountClass::Save();
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::SetPath(std::string sPath)
//|	Date					-	12/19/2002 12:29:59 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Set the internal string to contain the path to the accounts
//|									directory. This path much not contain a filename, As there
//|									is no checking implemented. 
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::SetPath(std::string sPath)
{
	try
	{
		m_sAccountsDirectory = sPath;
		return true;
	}
	catch(...)
	{
		return false;
	}
}

//o--------------------------------------------------------------------------o
//|	Function			-	std::string cAccountClass::GetPath(void)
//|	Date					-	12/19/2002 12:34:01 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Return a copy of the current accounts directory stored
//|									internally
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
std::string cAccountClass::GetPath(void)
{
	return m_sAccountsDirectory;
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::DelCharacter(WORD wAccountID, int nSlot)
//|	Date					-	12/19/2002 12:45:10 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Remove a character from the accounts map. Currently this
//|									character object resource is not freed. The Character ID, 
//|									and CChar object holders will be set to -1, and NULL 
//|									respectivly. As a consolation the characters will be for
//|									the interim listed in the orphan.adm file. This file will
//|									contain simply the username, and character ID that was 
//|									removed. For those that are wondering why this might be.
//|									Until the system itself deletes the character and items
//|									at least we will have a listing of orphaned character in
//|									game that a GM can use to locate, or even assign to another
//|									account.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::DelCharacter(UI16 wAccountID, int nSlot)
{
	// Do the simple here, save us some work
	if(nSlot<0||nSlot>4)
		return false;
	// ok were going to need to get the respective blocked from the maps
	MAPUSERNAMEID_ITERATOR I;
	I=m_mapUsernameIDMap.find(wAccountID);
	if(I==m_mapUsernameIDMap.end())
		return false;
	ACCOUNTSBLOCK actbID;
	actbID = I->second;
	// Ok now that we have the ID Map block we can get the Username Block
	MAPUSERNAME_ITERATOR J;
	J=m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbName;
	actbName=J->second;
	// We have both blocks now. We should validate the slot, and make the changes
	if(actbID.dwCharacters[nSlot]==-1||actbID.dwCharacters[nSlot]==0xffffffff||actbName.dwCharacters[nSlot]==-1||actbName.dwCharacters[nSlot]==0xffffffff)
		return false;
	// We need to make a entry in the orphan.adm file for this block before we change it
	std::string sTempPath(m_sAccountsDirectory);
	if(sTempPath[sTempPath.length()-1]=='\\'||sTempPath[sTempPath.length()-1]=='/')
		sTempPath += "orphans.adm";
	else
		sTempPath += "/orphans.adm";
	// First lets see if the file exists.
	bool bOrphanHeader=false;
	std::fstream fsOrphansADMTest(sTempPath.c_str(),std::ios::in);
	if(!fsOrphansADMTest.is_open())
		bOrphanHeader=true;
	fsOrphansADMTest.close();
	// ok open the stream for append and add this record to the end of the file
	std::fstream fsOrphansADM(sTempPath.c_str(),std::ios::out|std::ios::app);
	if(!fsOrphansADM.is_open())
		return false;
	// If this is a new file then we need to write the header first
	if(bOrphanHeader)
	{
		cAccountClass::WriteOrphanHeader(fsOrphansADM);
	}
	// Ok build then write what we need to the file
	fsOrphansADM << actbID.sUsername << "=" << std::hex << actbID.dwCharacters[nSlot];
#ifdef __uox__
	fsOrphansADM << actbID.lpCharacters[nSlot]->GetName() << "," << actbID.lpCharacters[nSlot]->GetX() << "," << actbID.lpCharacters[nSlot]->GetY() << "," actbID.lpCharacters[nSlot]->GetZ() << std::endl;
#endif
	fsOrphansADM.close();
	// Ok there is something in this slot so we should remove it.
	actbID.dwCharacters[nSlot]=actbName.dwCharacters[nSlot]=0xffffffff;
	actbID.lpCharacters[nSlot]=actbName.lpCharacters[nSlot]=NULL;
	// Now we have to put the values back into the maps
	try
	{
		I->second=actbID;
		J->second=actbName;
		cAccountClass::Save();
	}
	catch(...)
	{
		return false;
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::GetAccountByName(std::string sUsername,ACCOUNTSBLOCK& actbBlock)
//|	Date					-	12/19/2002 2:16:37 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Search the map for an account but the username that was
//|									specified.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::GetAccountByName(std::string sUsername,ACCOUNTSBLOCK& actbBlock)
{
	// Ok now we need to get the map blocks for this account.
	MAPUSERNAME_ITERATOR I;
	I = m_mapUsernameMap.find(sUsername);
	if(I==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbName;
	actbName=I->second;
	// Get the block from the ID map, so we can check that they are the same.
	MAPUSERNAMEID_ITERATOR J;
	J = m_mapUsernameIDMap.find(actbName.wAccountIndex);
	if(J==m_mapUsernameIDMap.end())
		return false;
	ACCOUNTSBLOCK actbID;
	actbID=J->second;
	// Check to see that these both are equal where it counts.
	if(actbID.sUsername!=actbName.sUsername||actbID.sPassword!=actbName.sPassword)
		return false;
	try
	{
		// Ok we need to copy this to the actbBlock referance from the calling function
		actbBlock=I->second;
		return true;
	}
	catch(...)
	{
		return false;
	}
}

//o--------------------------------------------------------------------------o
//|	Function			-	bool cAccountClass::GetAccountByName(WORD wAccountID,ACCOUNTSBLOCK& actbBlock)
//|	Date					-	12/19/2002 2:17:31 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Search the map for an account but the username that was
//|									specified.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
bool cAccountClass::GetAccountByID(UI16 wAccountID,ACCOUNTSBLOCK& actbBlock)
{
	// Ok now we need to get the map blocks for this account.
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wAccountID);
	if(I==m_mapUsernameIDMap.end())
		return false;
	ACCOUNTSBLOCK actbID;
	actbID=I->second;
	// Get the block from the ID map, so we can check that they are the same.
	MAPUSERNAME_ITERATOR J;
	J = m_mapUsernameMap.find(actbID.sUsername);
	if(J==m_mapUsernameMap.end())
		return false;
	ACCOUNTSBLOCK actbName;
	actbName=J->second;
	// Check to see that these both are equal where it counts.
	if(actbID.sUsername!=actbName.sUsername||actbID.sPassword!=actbName.sPassword)
		return false;
	try
	{
		// Ok we need to copy this to the actbBlock referance from the calling function
		actbBlock=I->second;
		return true;
	}
	catch(...)
	{
		return false;
	}
}

//o--------------------------------------------------------------------------o
//|	Function			-	WORD cAccountClass::Save(void)
//|	Date					-	12/19/2002 2:45:39 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Save the contents of the internal structures out to a flat
//|									file where it can be loaded later, and archived for later
//|									use should a crash occur and the userfile is damaged. At
//|									this stage this function will only write out the accounts.adm
//|									file, even though the access.adm will still eventually
//|									be used for server sharing.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
UI16 cAccountClass::Save(void)
{
#ifdef __uox__
	// Ok the first thing we are going to want to do it flush the maps so that 
	// we can reload them for externalaccountscreation and not lose 
	if(cwmWorldState->ServerData()->GetExternalAccountStatus())
	{
		// Ok flush and maps and reload from file to preserve any new entries
		cAccountClass::clear();
		cAccountClass::Load();
	}
#endif
	// Ok were not going to mess around. so we open truncate the file and write
	std::string sTemp(m_sAccountsDirectory);
	if(sTemp[sTemp.length()-1]=='\\'||sTemp[sTemp.length()-1]=='/')
		sTemp += "accounts.adm";
	else
		sTemp += "/accounts.adm";
	cAccountClass::PathFix(sTemp);
	// Ok we have the path, now open the file and start saving
	std::fstream fsAccountsADM(sTemp.c_str(),std::ios::out|std::ios::trunc);
	if(!fsAccountsADM.is_open())
		return 0x0000;
	// OK first we need to write the header.
	cAccountClass::WriteAccountsHeader(fsAccountsADM);
	// Now we need to iterate through each account block in the map, and save
	MAPUSERNAMEID_CITERATOR CI;
	for(CI=m_mapUsernameIDMap.begin();CI!=m_mapUsernameIDMap.end();CI++)
	{
		// Get a usable structure for this iterator
		ACCOUNTSBLOCK actbID;
		actbID = CI->second;
		// Ok we are going to load up each username block from that map too for checking
		MAPUSERNAME_CITERATOR JI;
		JI=m_mapUsernameMap.find(actbID.sUsername);
		ACCOUNTSBLOCK actbName;
		actbName = JI->second;
		// Check to make sure at least that the username and passwords match
		if(actbID.sUsername!=actbName.sUsername||actbID.sPassword!=actbName.sPassword)
		{
			// there was an error between blocks
			fsAccountsADM.close();
			return 0xFFFF;
		}
		// Ok write this block to the file
		//fsAccountsADM << "SECTION ACCOUNT " << std::dec << actbID.wAccountIndex << std::endl;
		fsAccountsADM << "SECTION ACCOUNT " << std::dec << actbID.wAccountIndex << std::endl;
		fsAccountsADM << "{" << std::endl;
		fsAccountsADM << "NAME " << actbID.sUsername << std::endl;
		fsAccountsADM << "PASS " << actbID.sPassword << std::endl;
		fsAccountsADM << "FLAGS 0x" << std::hex << actbID.wFlags << std::endl;
		fsAccountsADM << "PATH " << (actbID.sPath.length()?actbID.sPath:"ERROR") << std::endl;
		fsAccountsADM << "TIMEBAN 0x" << actbID.wTimeBan << std::endl;
		fsAccountsADM << "CONTACT " << (actbID.sContact.length()?actbID.sContact:"NA") << std::endl;
		for(int i=0;i<5;i++)
		{
#ifdef __uox__
			fsAccountsADM << "CHARACTER-" << std::dec << i+1 << " 0x" << std::hex << actbID.dwCharacters[i] << " [" << actbID.lpCharacters[i]->GetName() << "]" << std::endl;
#else
			fsAccountsADM << "CHARACTER-" << std::dec << i+1 << " 0x" << std::hex << actbID.dwCharacters[i] << " [UNKNOWN]" << std::endl;
#endif
		}
		fsAccountsADM << "}" << std::endl << std::endl;;
		// update the Users UAD file. (this will be used later)
		std::string sOutFile(actbID.sPath);
		sOutFile += actbID.sUsername;
		sOutFile += ".uad";
		// ok write the file
		std::fstream fsOut(sOutFile.c_str(),std::ios::out|std::ios::trunc);
		if(!fsOut.is_open())
		{
			// we cannot open the out file. Normally this would be an error, but support isn't implemented as of yet
			return 0xFFFF;
		}
		// Ok we have to write the new username.uad file in the directory
		cAccountClass::WriteUADHeader(fsOut,actbID);
		// Ok write out the characters and the charcter names if we know them
		for(int ii=0;ii<5;ii++)
		{
#ifdef __uox__
			fsOut << "CHARACTER-" << std::dec << i+1 << " 0x" << std::hex << actbID.dwCharacters[ii] << " [" << actbID.lpCharacters[i]->GetName() << "]" << std::endl;
#else
			fsOut << "CHARACTER-" << std::dec << i+1 << " 0x" << std::hex << actbID.dwCharacters[ii] << " [UNKNOWN]" << std::endl;
#endif
		}
		// Close the files since we dont need them anymore
		fsOut.close();
	}
	// Ok were done looping so we can close the file and return the count of accounts
	fsAccountsADM.close();
	return m_mapUsernameIDMap.size();
}

//o--------------------------------------------------------------------------o
//|	Function			-	MAPUSERNAME_ITERATOR cAccountClass::Begin(void)
//|	Date					-	1/14/2003 5:47:28 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the First iterator in a set. If there is no record
//|									then END() will be returned. This function will set the 
//|									internal Iterator to the first record or will indicate
//|									end().
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
MAPUSERNAMEID_ITERATOR& cAccountClass::begin(void)
{
	I = m_mapUsernameIDMap.begin();
	return I;
}

//o--------------------------------------------------------------------------o
//|	Function			-	MAPUSERNAME_ITERATOR cAccountClass::End(void)
//|	Date					-	1/14/2003 5:48:32 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the Last iterator in a set. This function forces
//|									the internal iterator to one past the last record. It
//|									will also return End() as a result.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
MAPUSERNAMEID_ITERATOR& cAccountClass::end(void)
{
	I = m_mapUsernameIDMap.end();
	return I;
}

//o--------------------------------------------------------------------------o
//|	Function			-	MAPUSERNAMEID_ITERATOR& cAccountClass::Last(void)
//|	Date					-	1/14/2003 6:03:22 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	For those that need a means to get the last valid record
//|									in the container, without having to process the Iterator
//|									and back it up one record. By default the map will return
//|									end() when there is no record to return for us;
//|									
//|									NOTE: This will update the current internal iterator to 
//|									the last record as well.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
MAPUSERNAMEID_ITERATOR& cAccountClass::last(void)
{
	I = m_mapUsernameIDMap.end();
	I--;
	return I;
}

//o--------------------------------------------------------------------------o
//|	Function			-	cAccountClass& cAccountClass::operator++()
//|									cAccountClass& cAccountClass::operator--(int)
//|	Date					-	1/14/2003 5:33:13 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Use this operators to control the internal iterator that
//|									points into the UsernameID map. This will work only on 
//|									this map. It will be assumed that the UsernameMap will 
//|									be matching this exactally. 
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
cAccountClass& cAccountClass::operator++()
{
	// just increment I, the rest will be handled internally
	I++;
	return (*this);
}
//
cAccountClass& cAccountClass::operator--(int)
{	
	I--;
	return (*this);
}

//o--------------------------------------------------------------------------o
//|	Function			-	VOID cAccountClass::AccountsHeader(fstream &fsOut)
//|	Date					-	12/19/2002 2:56:36 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Writes the Accounts.Adm header to the specified output
//|									stream.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
VOID cAccountClass::WriteAccountsHeader(std::fstream &fsOut)
{
#ifdef cVersionClass
	fsOut << "//AV3.0" << "-UV" << CVC.GetVersion() << "-BD" << CVC.GetBuild() << "-DS" << time(NULL) << "-ED" << CVC.GetRealBuild() << std::endl;
#else
	fsOut << "//AV3.0" << "-UV0.0-BD0-DS" << time(NULL) << "-ED0" << std::endl;
#endif
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "//accounts.adm[TEXT] : UOX3 uses this file for shared accounts access between servers" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   Format: " << std::endl;
	fsOut << "//      SECTION ACCOUNT 0" << std::endl;
	fsOut << "//      {" << std::endl;
	fsOut << "//         NAME username" << std::endl;
	fsOut << "//         PASS password" << std::endl;
	fsOut << "//         FLAGS 0x0000" << std::endl;
	fsOut << "//         PATH c:/uox3/Accounts/path2userdata/" << std::endl;
	fsOut << "//         TIMEBAN 0" << std::endl;
	fsOut << "//         CONTACT NONE" << std::endl;
	fsOut << "//         CHARACTER-1 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-2 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-3 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-4 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-5 0xffffffff" << std::endl;
	fsOut << "//      }" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   FLAGS: " << std::endl;
	fsOut << "//      Bit:  1) Banned     2) XGMClient    4) Public           8) Currently Logged In" << std::endl;
	fsOut << "//           16)           32)             64)                128)" << std::endl;
	fsOut << "//          256)          512)           1024)               2048)" << std::endl;
	fsOut << "//         4096)         8192) Seer     16384) GM Counselor 32768) GM Account" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   TIMEBAN: " << std::endl;
	fsOut << "//      This would be the end date of a timed ban." << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   CONTACT: " << std::endl;
	fsOut << "//      Usually this is the email address, but can be used as a comment or ICQ" << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}
//o--------------------------------------------------------------------------o
//|	Description		-	Writes the Access.Adm header to the specified output
//|									stream.
//o--------------------------------------------------------------------------o
VOID cAccountClass::WriteAccessHeader(std::fstream &fsOut)
{
#ifdef cVersionClass
	fsOut << "//SA3.0" << "-UV" << CVC.GetVersion() << "-BD" << CVC.GetBuild() << "-DS" << time(NULL) << "-ED" << CVC.GetRealBuild() << std::endl;
#else
	fsOut << "//SA3.0" << "-UV0.0-BD0-DS" << time(NULL) << "-ED0" << std::endl;
#endif
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "//access.adm[TEXT] : UOX3 uses this file for shared accounts access between servers" << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "//   Format: " << std::endl;
	fsOut << "//      SECTION ACCESS 0" << std::endl;
	fsOut << "//      {" << std::endl;
	fsOut << "//         NAME username" << std::endl;
	fsOut << "//         PASS password" << std::endl;
	fsOut << "//         PATH c:/uox3/Accounts/path2userdata/" << std::endl;
	fsOut << "//         FLAGS 0x0000" << std::endl;
	fsOut << "//         EMAIL NONE" << std::endl;
	fsOut << "//      }" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   FLAGS: " << std::endl;
	fsOut << "//      Bit:  1) Banned     2) XGMClient    4) Public           8) Currently Logged In" << std::endl;
	fsOut << "//           16)           32)             64)                128)" << std::endl;
	fsOut << "//          256)          512)           1024)               2048)" << std::endl;
	fsOut << "//         4096)         8192) Seer     16384) GM Counselor 32768) GM Account" << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}
//o--------------------------------------------------------------------------o
//|	Description		-	Writes the Orphan.Adm header to the specified output
//|									stream.
//o--------------------------------------------------------------------------o
VOID cAccountClass::WriteOrphanHeader(std::fstream &fsOut)
{
#ifdef cVersionClass
	fsOut << "//OI3.0" << "-UV" << CVC.GetVersion() << "-BD" << CVC.GetBuild() << "-DS" << time(NULL) << "-ED" << CVC.GetRealBuild() << "\n";
#else
	fsOut << "//OI3.0" << "-UV0.0-BD0-DS" << time(NULL) << "-ED0\n";
#endif
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// Orphans.Adm " << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UOX3 uses this file to store any characters that have been removed from" << std::endl;
	fsOut << "// from and account. They are stored here so there is at least some information" << std::endl;
	fsOut << "// regarding the ID, Name, X/Y/Z values if available so Shard ops can locate" << std::endl;
	fsOut << "// or transfer characters to GM accounts for removal." << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "// The format is as follows:" << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "//    username=ID,CharacterName,X,Y,Z" << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "// NOTE: CharacterName, and Coordinates may not be available." << std::endl;
	fsOut << "//------------------------------------------------------------------------------\n";
}
//o--------------------------------------------------------------------------o
//|	Description		-	Writes the Username.uad header to the specified output
//|									stream.
//o--------------------------------------------------------------------------o
VOID cAccountClass::WriteUADHeader(std::fstream &fsOut,ACCOUNTSBLOCK& actbTemp)
{
#ifdef cVersionClass
	fsOut << "//AI3.0" << "-UV" << CVC.GetVersion() << "-BD" << CVC.GetBuild() << "-DS" << time(NULL) << "-ED" << CVC.GetRealBuild() << std::endl;
#else
	fsOut << "//AI3.0" << "-UV0.0-BD0-DS" << time(NULL) << "-ED0" << std::endl;
#endif
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UAD Path: " << actbTemp.sPath << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UOX3 uses this file to store any extra administration info\n";
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "ID " << actbTemp.wAccountIndex << std::endl;
	fsOut << "BANTIME " << std::hex << actbTemp.wTimeBan << std::dec << std::endl;
	fsOut << "LASTIP " << (int)(actbTemp.dwLastIP>>24) << "." << (int)(actbTemp.dwLastIP>>16) << "." << (int)(actbTemp.dwLastIP>>8) << "." << (int)(actbTemp.dwLastIP%256) << std::endl;
	fsOut << "COMMENT " << actbTemp.sContact << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}
