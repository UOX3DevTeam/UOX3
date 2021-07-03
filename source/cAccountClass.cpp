//o-----------------------------------------------------------------------------------------------o
//|	File		-	cAccountClass.cpp
//|	Date		-	12/6/2002 4:16:33 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Seeing as I had screwed up accounts so bad, I started from
//|					scratch. The concept of how they work has been reevaluated
//|					and will be changed to suit a more understandable direction.
//|					See xRFC0004 for complete specification and implementaion
//|					information.
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "cVersionClass.h"
#include "enums.h"
#include <cstdint>
#include <filesystem>
#include "StringUtility.hpp"
#if PLATFORM != WINDOWS
#include <arpa/inet.h>
#endif

cAccountClass *Accounts;

const UI08 CHARACTERCOUNT = 7;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	Class Construction and Desctruction
//|	Date		-	12/6/2002 4:18:58 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Classic class construction and destruction
//|
//|	Changes		-	Added the member to handle the stream to packet packing
//|					symantics. I put it here because from most levels of
//|					object the pointer to accounts data is fairly consistant
//|					and readily available
//o-----------------------------------------------------------------------------------------------o
cAccountClass::cAccountClass():m_sAccountsDirectory(".\\")
{
	m_wHighestAccount = 0x0000;
	I = m_mapUsernameIDMap.end();
	actbInvalid.wAccountIndex = AB_INVALID_ID;
}
//
cAccountClass::cAccountClass( const std::string &sAccountsPath )
{
	// Call into the initialize
	cAccountClass();
	// Set accounts path.
	try
	{
		m_sAccountsDirectory=sAccountsPath;
	}
	catch( ... )
	{
		throw;
	}
}
//
cAccountClass::~cAccountClass()
{

}
//

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 CreateAccountSystem( void )
//|	Date		-	12/6/2002 4:19:53 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Instead of expecting end users to fix the accounts by hand
//|					the idea of a member function that would provide some means
//|					to automate the process was devised.
//|
//|					This function is intended for use in uox3 v0.97.0 and
//|					v0.97.1+ builds for converting them to the new system. This
//|					member function will also create a new account system for
//|					pre v0.97.00 versions. However pre UOX3 v0.97.00 characters
//|					will be lost even if world files have been converted and
//|					unable to locate the respective character ID number when
//|					converting the world.
//o-----------------------------------------------------------------------------------------------o
UI16 cAccountClass::CreateAccountSystem( void )
{
	// Get the system account path. Should have been set at construction.
	std::string sActPath = m_sAccountsDirectory;
	// Check account path to make sure that its at least got data in it.
	if( sActPath.length() <= 3 )
	{
		// If your wondering why a length of 3 - IE: 'C:\'. Send back indication that no accounts were processed
		return 0x0000;
	}
	// Build the full path and filename string here for access.adm
	std::string sAccessAdm = sActPath;
	sAccessAdm += (sActPath[sActPath.length()-1]=='\\'||sActPath[sActPath.length()-1]=='/')?"access.adm":"/access.adm";
	// Create stream and open the Access.adm file if it exists.
	//bool bIsNew = true;
	std::fstream fs1( sAccessAdm.c_str(),std::ios::in );

	// Following is commented out, as it only serves to set bIsNew=false at the moment, which is not used anywhere
	//if( !fs1.is_open() )
	//{
	// Ok there was no access.adm this implies that this is a pre v0.97.00 release
	//	bIsNew=false;
	//}

	// Build the full path and filename string here for accounts.adm
	std::string sAccountsAdm = sActPath;
	sAccountsAdm += (sActPath[sActPath.length()-1]=='\\'||sActPath[sActPath.length()-1]=='/')?"accounts.adm":"/accounts.adm";
	// Create stream and open the Accounts.Adm file for processing
	std::fstream fs2(sAccountsAdm.c_str(),std::ios::in);
	if( !fs2.is_open() )
	{
		// ok there was some error. Either the file  doesn't exist or a system error.
		if( fs1.is_open() )
			fs1.close();
		return 0x0000;
	}
	// We need to read from the stream once before entering the loop
	std::string sLine;
	std::getline( fs2, sLine );
	// Ok start the loop and process
	bool bBraces[3]	= { false, false, false };
	bool bBraces2[3]= { false, false, false };
	CAccountBlock	actb;
	UI16 wAccountID		= 0x0000;
	UI16 wAccessID		= 0x0000;
	UI16 wAccountCount	= 0x0000;
	UI08 nLockCount		= 0x00;
	bool bSkipUAD		= false;
	actb.reset();
	sLine = strutil::stripTrim( sLine );
	while( !fs2.eof() && !fs2.fail() )
	{
		if( sLine.empty() || sLine.length() == 0 )	// Either nothing on the line, or there was a comment we've ignored
		{
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// Check to see if there is an EOF for those that are like legacy FREAKS!!! Freaks I say!!
		if( "EOF" == sLine )
		{
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// Keep track of the section account lines. There is nothing to process till at least one of these has been read
		if( sLine.substr( 0, 15 ) == "SECTION ACCOUNT" )
		{
			// Ok the section block was found, Tokenize the string to get the account #
			auto accountIDFromFile = strutil::value<std::int32_t>( strutil::extractSection( sLine, " ", 2, 2 ) );
			if( accountIDFromFile == 0 && wAccountCount > 0 )
			{
				actb.wAccountIndex = wAccountCount;
				wAccountID = wAccountCount;
			}
			else
			{
				actb.wAccountIndex = accountIDFromFile;
				wAccountID = accountIDFromFile;
			}

			// Increment the account acount
			++wAccountCount;

			// Ok, we have parsed out the account ID, Set bBraces[2] to true to allow block reading
			bBraces[0] = false;
			bBraces[1] = false;
			bBraces[2] = true;
			// Get the next line and continue
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// Fail safe. If bBraces[2] isn't set there is no need to even run this code
		if( !bBraces[2] )
		{
			// Make sure the get the next line to process.
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// If we get here then were reading a block now. Check for the openning brace.
		if( sLine[0] == '{' && !bBraces[0] && !bBraces[1] )
		{
			bBraces[0] = true;
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		if( ((sLine[0] == '{') && bBraces[0]) || ((sLine[0] == '{') && bBraces[1] ))
		{
			fs2.close();
			fs1.close();
			return 0x0000;
		}
		if( sLine[0] == '}' && bBraces[0] && !bBraces[1] )
		{
			// Ok we shoud shove this into the map(s) for use later
			m_mapUsernameIDMap[actb.wAccountIndex] = actb;
			m_mapUsernameMap[actb.sUsername] = &m_mapUsernameIDMap[actb.wAccountIndex];
			// Ok we have finished with this access block clean up and continue processing
			bBraces[0] = false;
			bBraces[1] = false;
			bBraces[2] = false;
			bBraces[0] = false;
			nLockCount = 0;
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// Set up the tokenizing
		auto ssecs = strutil::sections( sLine, " " );
		auto l = strutil::stripTrim( ssecs[0] );
		std::string r = "";
		if( ssecs.size() > 1 )
		{
			r = strutil::stripTrim( ssecs[1] );
		}
		//auto r = strutil::stripTrim( ssecs[1] );
		// Parse and store based on tag
		if( "NAME" == l )
		{
			if( !r.empty() && r.length() != 0 )
			{
				actb.sUsername = strutil::tolower(r);
				if( ssecs.size() > 2 )
					actb.sUsername += strutil::tolower(ssecs[2]);

				// Next thing were going to do is make sure there isn't a duplicate username.
				while( isUser( actb.sUsername ) )
				{
					// This username is already on the list, keep adding a random value to the end of the username until we find a free one
					actb.sUsername += strutil::number( RandomNum(10000, 20000) );
				}
			}
			else
			{
				actb.sUsername = "ERROR";
			}
			std::getline( fs2, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "PASS" )
		{
			if( !r.empty() && r.length() != 0 )
			{
				// Ok strip the password and store it.
				actb.sPassword = r;
				// Now we have to parse the crap in access.adm. Were not gonna look at much format just the 2 tags we need.
				std::string sLine2;
				if( fs1.is_open() )
				{
					std::getline( fs1, sLine2 );
					sLine2 = strutil::stripTrim( sLine2 );
					bBraces2[2] = true;
					while( !fs1.eof() && bBraces2[2] && !fs1.fail() )
					{
						if( sLine2.empty() && sLine2.length() != 0 )
						{
							std::getline( fs1, sLine2 );
							sLine2 = strutil::stripTrim( sLine2 );
							continue;
						}
						// Keep track of the section account lines. There is nothing to process till at least one of these has been read
						if( sLine2.substr( 0, 14 ) ==  "SECTION ACCESS" )
						{
							// Ok the section block was found, Tokenize the string to get the account #
							wAccessID = strutil::value<std::int32_t>(strutil::extractSection(sLine2, " ", 2 ));
							// Ok, we have parsed out the account ID, Set bBraces[2] to true to allow block reading
							bBraces2[0] = false;
							bBraces2[1] = false;
							bBraces2[2] = true;
							std::getline( fs1, sLine2 );
							continue;
						}
						// Fail safe. If bBraces[2] isn't set there is no need to even run this code
						if( !bBraces2[2] )
						{
							// Make sure the get the next line to process.
							std::getline( fs1, sLine2 );
							continue;
						}
						// If we get here then were reading a block now. Check for the openning brace.
						if( sLine2[0] == '{' && !bBraces2[0] && !bBraces2[1] )
						{
							bBraces2[0] = true;
							std::getline( fs1, sLine2 );
							continue;
						}
						if( ((sLine2[0] == '{') && bBraces2[0]) || ((sLine2[0] == '{') && bBraces2[1]) )
						{
							fs2.close();
							fs1.close();
							return 0x0000;
						}
						// Loop reading this block till the end brace is encountered.
						while( bBraces2[0] && !fs1.eof() && !fs1.fail() )
						{
							// Read in the next line
							std::getline( fs1, sLine2 );
							// check for the closing brace
							if( sLine2[0] == '}' && bBraces2[0] && !bBraces2[1] )
							{
								bBraces2[0] = false;
								bBraces2[1] = false;
								bBraces2[2] = false;
								break;
							}
							auto ssecs = strutil::sections( sLine2, " " );
							auto l2 = strutil::stripTrim( ssecs[0] );
							auto r2 = strutil::stripTrim( ssecs[1] );
							// Parse and store based on tag
							if( "PATH" == l2 )
							{
								if( !r2.empty() && r2.length() != 0 ) // Ok strip the name and store it. We need to make it all the same case for comparisons
									actb.sPath = r2 + actb.sUsername + ".uad";
								else
									actb.sPath = "";
								continue;
							}
							if( l2 == "FLAGS" )
							{
								if( !r2.empty() && r2.length() != 0 )
								{
									// Ok strip the flags and store it. We need to make it all the same case for comparisons
									actb.wFlags = static_cast<UI16>(std::stoul(r2, nullptr, 0));	//  -- Uses internal conversion code
									if( actb.wAccountIndex == 0 )
										actb.wFlags.set( AB_FLAGS_GM, true );
								}
								else
								{
									actb.wFlags.reset();
									if( actb.wAccountIndex == 0 )
										actb.wFlags.set( AB_FLAGS_GM, true );
								}
								continue;
							}
						}
						std::getline( fs1, sLine );
					}
				}
				else
				{
					// Ok we might as well add the file name to this path, cause we will need it to open the
					std::string sTemp(sActPath);
					sTemp += actb.sUsername;
					sTemp += "/";
					actb.sPath=sTemp;
					// Need to create the directory for this entry as an access.adm isn't available
					std::filesystem::create_directory(std::filesystem::path(actb.sPath));

					// Now build the uad filename, and path,
					sTemp += actb.sUsername;
					sTemp += ".uad";
					// Now that we have this we need to open, and write out this file.
					std::fstream fsUADFile(sTemp.c_str(),std::ios::trunc|std::ios::out);
					if( !fsUADFile.is_open() )
					{
						fs2.close();
						return 0x0000;
					}
					WriteUADHeader(fsUADFile,actb);
					// Need to write out the charcters
					for( UI08 ii = 1; ii < CHARACTERCOUNT + 1; ++ii )
					{
						fsUADFile << "CHARACTER-" << std::dec << (SI32)ii << " 0xffffffff [UNKNOWN]" << std::endl;
					}
					fsUADFile.close();
					// Flood fill the actb class for accounts
					for( UI08 i = 0; i < CHARACTERCOUNT; i++ )
					{
						actb.dwCharacters[i] = INVALIDSERIAL;
						actb.lpCharacters[i] = nullptr;
					}
					// Ok strip the name and store it. We need to make it all the same case for comparisons
					actb.wFlags = 0x0004;
					// If account 0 then we set gm privs
					if( wAccountID == 0 )
						actb.wFlags.set( AB_FLAGS_GM, true );

					bSkipUAD = true;
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
			std::getline( fs2, sLine );
			continue;
		}
		else if( l == "LASTIP" )
		{
			auto psecs = strutil::sections( r, "." );
			if( !r.empty() && r.length() != 0 && psecs.size() == 4 )
			{
				actb.dwLastIP = calcserial( strutil::value<SI08>( psecs[0] ), strutil::value<SI08>( psecs[1] ), strutil::value<SI08>( psecs[2] ), strutil::value<SI08>( psecs[3] ) );
			}
			else
			{
				actb.dwLastIP = 0x00000000;
			}

			std::getline( fs2, sLine );
			continue;
		}
		else if( l == "CONTACT" )
		{
			// Ok strip the name and store it. We need to make it all the same case for comparisons
			if( !r.empty() && r.length() != 0 )
				actb.sContact = strutil::tolower( r );
			else
				actb.sContact = "UNKNOWN";

			std::getline( fs2, sLine );
			continue;
		}
		else if( l == "TIMEBAN" )
		{
			if( !r.empty() && r.length() != 0 )
			{
				actb.wTimeBan = static_cast<UI32>(std::stoul(r, nullptr, 0));
			}
			else
			{
				actb.wTimeBan = 0;
			}

			std::getline( fs2, sLine );
			continue;
		}
		else if( l == "LOCK" )
		{
			if( nLockCount <= 5 )
			{
				if( std::stoi(r, nullptr, 0) > 0 )
				{
					actb.wFlags.set( (AB_FLAGS_CHARACTER1 + static_cast<size_t>(nLockCount)), true );
				}
			}
			++nLockCount;
			std::getline( fs2, sLine );
			continue;
		}
		else if( l == "PUBLIC" )
		{
			if( !r.empty() && r.length() != 0 )
			{
				// Ok strip the name and store it. We need to make it all the same case for comparisons
				if( r == "ON" ) // Set the Public Flag on. Public implies that a users conatact information can be published on the web.
					actb.wFlags.set( AB_FLAGS_PUBLIC, true );
			}
			std::getline( fs2, sLine );
			continue;
		}
		else if( l.substr( 0, 10 ) == "CHARACTER-" )
		{
			SI32 charNum = std::stoi(l.substr( 10 ) , nullptr, 0);
			if( charNum < 1 || charNum > CHARACTERCOUNT )
			{
				Console.error( "Invalid character found in accounts" );
			}
			else
			{
				if( !r.empty() && r.length() != 0 && strutil::value<std::uint32_t>( r ) != INVALIDSERIAL )
				{
					actb.dwCharacters[charNum-1] = static_cast<UI32>(std::stoul(r, nullptr, 0));
					actb.lpCharacters[charNum-1] = calcCharObjFromSer( actb.dwCharacters[charNum-1] );
				}
				else
				{
					actb.dwCharacters[charNum-1] = INVALIDSERIAL;
					actb.lpCharacters[charNum-1] = nullptr;
				}
			}
			std::getline( fs2, sLine );
			continue;
		}
		std::getline( fs2, sLine );
	}
	//
	m_wHighestAccount=wAccountCount;
	// Make sure to close our open paths.
	fs2.close();
	fs1.close();
	// If this is the first run with a legacy accounts.adm then we dont need to do this
	if( !bSkipUAD )
	{
		// Ok the next thing were going to have to do is check paths, make new paths and write the UAD file back into the respective directory
		for( MAPUSERNAMEID_ITERATOR CJ = m_mapUsernameIDMap.begin();CJ!=m_mapUsernameIDMap.end(); ++CJ )
		{
			// Pull the data into a usable form
			CAccountBlock& actbTemp = CJ->second;
			// Now we want to copy the files from the path to the new directory.
			char		cDirSep = sActPath[sActPath.length()-1];
			std::string	sNewPath( sActPath );
			if( cDirSep == '\\' || cDirSep == '/' )
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
			if (!std::filesystem::exists(std::filesystem::path(sNewPath))) {
				auto create_status = std::filesystem::create_directory(std::filesystem::path(sNewPath));
				if (!create_status) {
					Console.error( strutil::format("CreateAccountSystem(): Couldn't create directory %s", sNewPath.c_str()) );
					m_mapUsernameIDMap.clear();
					m_mapUsernameMap.clear();
					return 0L;

				}
			}

			// Now open and copy the files. to their new location.
			if( !actbTemp.sPath.length() )
			{
				// the file/path was not found so we should make an entry
				std::string sOutFile(sNewPath);
				if( sOutFile[sOutFile.length()-1]!='\\'&&sOutFile[sOutFile.length()-1]!='/' )
					sOutFile += "/";
				sOutFile += actbTemp.sUsername;
				sOutFile += ".uad";
				std::fstream fsOut(sOutFile.c_str(),std::ios::out);
				if( !fsOut.is_open() )
				{
					return 0L;
				}
				// Ok we have to write the new username.uad file in the directory
				WriteUADHeader( fsOut, actbTemp );
				// Ok write out the characters and the charcter names if we know them
				for( SI32 i = 0; i < CHARACTERCOUNT; ++i )
				{
					fsOut << "CHARACTER-" << (i+1) << " 0x" << std::hex << (actbTemp.dwCharacters[i] != INVALIDSERIAL ? actbTemp.lpCharacters[i]->GetSerial() : INVALIDSERIAL ) << " [" << (char*)(actbTemp.lpCharacters[i] != nullptr ? actbTemp.lpCharacters[i]->GetName().c_str() : "UNKNOWN" ) << "]\n";
				}
				// Close the files since we dont need them anymore
				fsOut.close();
			}
			else
			{
				std::fstream fsIn(actbTemp.sPath.c_str(),std::ios::in);
				std::string sOutFile(sNewPath);
				if( sOutFile[sOutFile.length()-1]!='\\'&&sOutFile[sOutFile.length()-1]!='/' )
					sOutFile += "/";
				sOutFile += actbTemp.sUsername;
				sOutFile += ".uad";
				sOutFile = strutil::replaceSlash( sOutFile );
				std::fstream fsOut(sOutFile.c_str(),std::ios::out);
				if( !fsOut.is_open() )
				{
					// we cannot open the out file.
					fsIn.close();
					// free up any allocation of memory made.
					m_mapUsernameIDMap.clear();
					m_mapUsernameMap.clear();
					return 0L;
				}
				// ok Lets just copy one file to the next.
				while( !fsIn.eof() && !fsIn.fail() )
				{
					char szLine[128];
					fsIn.getline(szLine,127);
					fsOut << szLine << std::endl;
					if( !fsIn.is_open() )
						break;
				}
				fsOut.close();
				fsIn.close();
			}
			// Now were done so we want to update the path in our records before we write them
			actbTemp.sPath = sNewPath;
		}
	}
	// Make a back up first, then Dump to new file,
	std::string sBUAccess(sAccessAdm);
	sBUAccess += ".bu";
	[[maybe_unused]] int renameResult = rename(sAccessAdm.c_str(),sBUAccess.c_str());
	//
	SI32 kk=0;
	std::string szltoa2 ;
	std::string sBUPath(sAccountsAdm);
	sBUPath += ".bu";
	SI32 nResp= rename(sAccountsAdm.c_str(),sBUPath.c_str());
	std::string sNewPath = sBUPath;
	while( nResp == -1 )
	{
		// Loop through 255 numbers, these will be added to the end of the bu in case a bu exists.
		szltoa2 = strutil::format( "%d", kk++ );
		sNewPath += szltoa2;
		nResp= rename(sAccountsAdm.c_str(),sNewPath.c_str());
		sNewPath = sBUPath;
	};
	std::fstream fsOut(sAccountsAdm.c_str(),std::ios::trunc|std::ios::out);
	if( !fsOut.is_open() )
	{
		// Error opening output file. Free up any allocation of memory made.
		m_mapUsernameIDMap.clear();
		m_mapUsernameMap.clear();
		return 0L;
	}
	WriteAccountsHeader( fsOut );
	for( MAPUSERNAMEID_ITERATOR CI = m_mapUsernameIDMap.begin(); CI != m_mapUsernameIDMap.end(); ++CI )
	{
		CAccountBlock& actbTemp = CI->second;
		WriteAccountSection( actbTemp, fsOut );
	}
	// Close the file
	fsOut.close();
	return wAccountCount;
}

void cAccountClass::WriteAccountSection( CAccountBlock& actbTemp, std::fstream& fsOut )
{
	fsOut << "SECTION ACCOUNT " << std::dec << actbTemp.wAccountIndex << std::endl;
	fsOut << "{" << std::endl;
	fsOut << "NAME " << actbTemp.sUsername << std::endl;
	fsOut << "PASS " << actbTemp.sPassword << std::endl;
	fsOut << "FLAGS 0x" << std::hex << actbTemp.wFlags.to_ulong() << std::dec << std::endl;
	fsOut << "PATH " << strutil::replaceSlash(actbTemp.sPath) << std::endl;
	fsOut << "TIMEBAN 0x" << std::hex << actbTemp.wTimeBan << std::dec << std::endl;
	fsOut << "LASTIP " << (SI32)((actbTemp.dwLastIP&0xFF000000)>>24) << "." << (SI32)((actbTemp.dwLastIP&0x00FF0000)>>16) << "." << (SI32)((actbTemp.dwLastIP&0x0000FF00)>>8) << "." << (SI32)((actbTemp.dwLastIP&0x000000FF)%256) << std::endl;
	fsOut << "CONTACT " << (actbTemp.sContact.length()?actbTemp.sContact:"NA") << std::endl;
	for( UI08 ii = 0; ii < CHARACTERCOUNT; ++ii )
	{
		SERIAL charSer			= INVALIDSERIAL;
		std::string charName	= "UNKNOWN";
		if( actbTemp.lpCharacters[ii] != nullptr && actbTemp.dwCharacters[ii] != INVALIDSERIAL )
		{
			charSer		= actbTemp.dwCharacters[ii];
			charName	= actbTemp.lpCharacters[ii]->GetName();
		}
		fsOut << "CHARACTER-" << std::dec << (ii+1) << " 0x" << std::hex << charSer << std::dec;
		fsOut << " [" << charName.c_str() << "]" << std::endl;
	}
	fsOut << "}" << std::endl << std::endl;

}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 AddAccount( std::string sUsername, std::string sPassword, const std::string &sContact,UI16 wAttributes )
//|					UI16 AddAccount( std::string sUsername, std::string sPassword, std::string sContact,UI16 wAttributes )
//|	Date		-	12/12/2002 11:15:20 PM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This function creates in memory as well as on disk a new account that will
//|					allow access to the server from external clients. The basic goal of this
//|					function is to create a new account record with no characters. Because this
//|					function may be called from in game a hard copy will be made as well instead of
//|					waiting for a save to make the changes perminent.
//|
//|						sUsername:   Name of the account
//|						sPassword:   Password of the account
//|						sContact:    Historically this is the valid email address for this account
//|						wAttributes: What attributes should this account start with
//o-----------------------------------------------------------------------------------------------o
//|	Returns		-	[UI16] Containing the account number of new accounts or 0
//o-----------------------------------------------------------------------------------------------o
//|	NOTE		-	This function does NOT add this account to the accounts map. This function ONLY
//|					creates the directories, and entries required to load these new accounts. The
//|					idea was not to have to rely on the accounts in memory, so that accounts that
//|					are added, are added immediatly to the hard copy.
//|	NOTE		-	For any new accounts to be loaded into the internal accounts map structure,
//|					accounts must be reloaded!
//o-----------------------------------------------------------------------------------------------o
UI16 cAccountClass::AddAccount( std::string sUsername, std::string sPassword, const std::string &sContact, UI16 wAttributes )
{
	// First were going to make sure that the needed fields are sent in with at least data
	if( sUsername.length() < 4 || sPassword.length() < 4 )
	{
		// Username, and or password must both be 4 characters or more in length
		Console.log(strutil::format("ERROR: Unable to create account for username '%s' with password of '%s'. Username/Password to short",sUsername.c_str(),sPassword.c_str()),"accounts.log");
		return 0x0000;
	}
	// Next thing were going to do is make sure there isn't a duplicate username.
	if( isUser( sUsername ) )
	{
		// This username is already on the list.
		return 0x0000;
	}
	// If we get here then were going to create a new account block, and create all the needed directories and files.
	CAccountBlock actbTemp;
	actbTemp.reset();

	// Build as much of the account block that we can right now.
	actbTemp.sUsername	= sUsername;
	actbTemp.sPassword	= sPassword;
	actbTemp.sContact	= sContact;
	actbTemp.wFlags		= wAttributes;
	actbTemp.wTimeBan	= 0;
	actbTemp.dwLastIP	= 0;
	for( UI08 ii = 0; ii < CHARACTERCOUNT; ++ii )
	{
		actbTemp.lpCharacters[ii] = nullptr;
		actbTemp.dwCharacters[ii] = INVALIDSERIAL;
	}
	// Ok we have everything except the path to the account dir, so make that now
	std::string sTempPath( m_sAccountsDirectory );
	if( sTempPath[sTempPath.length() - 1] == '\\' || sTempPath[sTempPath.length() - 1] == '/' )
	{
		auto szTempBuff	=    strutil::tolower( sUsername );
		sTempPath			+= szTempBuff;
		sTempPath			+= "/";
		sTempPath = strutil::replaceSlash( sTempPath );
		actbTemp.sPath		= sTempPath;
	}
	else
	{
		auto szTempBuff	= strutil::tolower( sUsername );
		sTempPath			+= "/";
		sTempPath			+= szTempBuff;
		sTempPath			+= "/";
		sTempPath = strutil::replaceSlash( sTempPath );
		actbTemp.sPath = sTempPath;
	}
	// Ok now that we got here we need to make the directory, and create the username.uad file
	if( !std::filesystem::exists(std::filesystem::path(actbTemp.sPath)) )
	{
		auto create_status = std::filesystem::create_directory(std::filesystem::path(actbTemp.sPath));
		if (!create_status) {
			Console.error( strutil::format("AddAccount(): Couldn't create directory %s", actbTemp.sPath.c_str()));
			return 0x0000;

		}
	}

	// Ok now thats finished. We need to do one last thing. Create the username.uad file in the account directory
	std::string sUsernameUADPath(actbTemp.sPath);
	if( sUsernameUADPath[sUsernameUADPath.length() - 1] == '\\' || sUsernameUADPath[sUsernameUADPath.length() - 1] == '/' )
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
	sUsernameUADPath = strutil::replaceSlash( sUsernameUADPath );
	// Open the file in APPEND to end mode.
	std::fstream fsAccountsUAD( sUsernameUADPath.c_str(), std::ios::out|std::ios::trunc );
	if( !fsAccountsUAD.is_open() )
	{
		// Ok we were unable to open the file so this user will not be added.
		return 0x0000;
	}
	// were using the ID due to its numbering, it will always show the account blocks in ID order least to greatest.
	actbTemp.wAccountIndex = m_wHighestAccount + 1;
	// Ok we have to write the new username.uad file in the directory
	fsAccountsUAD << "//AI3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" << CVersionClass::GetBuild() << "-DS" << time(nullptr) << "-ED" << CVersionClass::GetRealBuild() << "\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------" << std::endl;
	fsAccountsUAD << "// UAD Path: " << actbTemp.sPath << std::endl;//"\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------" << std::endl;
	fsAccountsUAD << "// UOX3 uses this file to store any extra administration info\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------\n";
	fsAccountsUAD << "ID " << actbTemp.wAccountIndex << "\n";
	fsAccountsUAD << "NAME " << actbTemp.sUsername << "\n";
	fsAccountsUAD << "PASS " << actbTemp.sPassword << "\n";
	fsAccountsUAD << "BANTIME " << std::hex << "0x" << actbTemp.wTimeBan << std::dec << "\n";
	fsAccountsUAD << "LASTIP " << (SI32)((actbTemp.dwLastIP&0xFF000000)>>24) << "." << (SI32)((actbTemp.dwLastIP&0x00FF0000)>>16) << "." << (SI32)((actbTemp.dwLastIP&0x0000FF00)>>8) << "." << (SI32)((actbTemp.dwLastIP&0x000000FF)%256) << "\n";
	fsAccountsUAD << "CONTACT " << actbTemp.sContact << "\n";
	fsAccountsUAD << "//------------------------------------------------------------------------------\n";
	// Ok write out the characters and the charcter names if we know them
	for( UI08 i = 0; i < CHARACTERCOUNT; ++i )
	{
		fsAccountsUAD << "CHARACTER-" << (i+1) << " 0x" << std::hex << (actbTemp.dwCharacters[i] != INVALIDSERIAL ? actbTemp.dwCharacters[i]:INVALIDSERIAL) << " [" << (char*)(actbTemp.lpCharacters[i] != nullptr ? actbTemp.lpCharacters[i]->GetName().c_str() : "INVALID" ) << "]\n";
	}
	// Close the files since we dont need them anymore
	fsAccountsUAD.close();
	// Ok now we can add this record to the accounts.adm file
	std::string sAccountsADMPath( m_sAccountsDirectory );
	if( sAccountsADMPath[sAccountsADMPath.length() - 1] == '\\' || sAccountsADMPath[sAccountsADMPath.length() - 1] == '/' )
		sAccountsADMPath += "accounts.adm";
	else
		sAccountsADMPath += "/accounts.adm";
	// Open the file in APPEND to end mode.
	std::fstream fsAccountsADM( sAccountsADMPath.c_str(), std::ios::out|std::ios::app );
	if( !fsAccountsADM.is_open() )
	{
		// Ok we were unable to open the file so this user will not be added.
		return 0x0000;
	}
	WriteAccountSection( actbTemp, fsAccountsADM );
	fsAccountsADM.close();
	// Ok might be a good thing to add this account to the map(s) now.
	m_mapUsernameIDMap[actbTemp.wAccountIndex] = actbTemp;
	m_mapUsernameMap[actbTemp.sUsername] = &m_mapUsernameIDMap[actbTemp.wAccountIndex];
	m_wHighestAccount = actbTemp.wAccountIndex;
	// Return to the calling function
	return (UI16)m_mapUsernameIDMap.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isUser( string sUsername )
//|	Date		-	12/16/2002 12:09:13 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	The only function that this function member servers is to return a response
//|					based on the existance of the specified username.
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::isUser( std::string sUsername )
{
	MAPUSERNAME_ITERATOR I	= m_mapUsernameMap.find( sUsername );
	return( I != m_mapUsernameMap.end() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t cAccountClass::size()
//|	Date		-	12/17/2002 3:35:31 PM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the current count of accounts that are currently stored in the account map.
//o-----------------------------------------------------------------------------------------------o
UI32 cAccountClass::size()
{
	return static_cast<std::uint32_t>(m_mapUsernameMap.size());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 cAccountClass::Load()
//|	Date		-	12/17/2002 4:00:47 PM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load the internal accounts structure from the accounts.adm
//|							file. Due to the nature of this function we will empty the
//|							previous contents of the account map, and reload them.
//|
//|	Changes		-	1/20/2003 - Forgot to put in place the lookup to see if
//|							accounts need to be updated to the v3 format. Should only
//|							need to check the first line of the accounts.adm file.
//|							NOTE: Do not remove this line if you wish to convert
//|							properly. Without the first line this function will assume
//|							that the accounts file is a v2 format file(UOX3 v0.97.0).
//|
//|	Changes		-	1/20/2003 - Added support for the DTL libs, and create a
//|							general SQL query for MSAccess, MSSQL, and mySQL
//o-----------------------------------------------------------------------------------------------o
UI16 cAccountClass::Load(void)
{
	// Now we can load the accounts file in and re fill the map.
	std::string sAccountsADM( m_sAccountsDirectory );
	sAccountsADM += (m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='\\'||m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='/')?"accounts.adm":"/accounts.adm";
	sAccountsADM = strutil::replaceSlash(sAccountsADM);
	// Check to see what version the current accounts.adm file is.
	std::string sLine;
	std::fstream fsAccountsADMTest(sAccountsADM.c_str(),std::ios::in);
	if( !fsAccountsADMTest.is_open() )
	{
		// we were unable to load any accounts
		return 0x0000;
	}
	std::getline( fsAccountsADMTest, sLine );
	// Check to see if this is a V3 file.
	if( sLine.find( "AV3.0" ) == std::string::npos )
	{
		// CreateAccountSystem renames accounts.adm for back up purposes, so we need to release its file isntance before calling
		fsAccountsADMTest.close();
		Console << myendl << "  o Processing legacy UOX3 accounts.adm file";
		UI16 wResponse = CreateAccountSystem();
		Console.PrintDone();
		Console << "    - Processed " << wResponse << " account blocks." << myendl;
		// Nope, not a v3 file.. so we should attempt a convert
		return wResponse;
	}
	else
		Console << myendl << " o Processing accounts file";

	if( fsAccountsADMTest.is_open() )
		fsAccountsADMTest.close();
	// OK now we can open the file.
	std::fstream fsAccountsADM(sAccountsADM.c_str(),std::ios::in);
	if( !fsAccountsADM.is_open() ) // we were unable to load any accounts
		return 0x0000;

	// We need to read from the stream once before entering the loop
	std::getline( fsAccountsADM, sLine );
	sLine = strutil::stripTrim( sLine );
	// Ok start the loop and process
	bool bBraces[3]					= { false, false, false };
	bool bBraces2[3]				= { false, false, false };
	CAccountBlock actb;
	UI16 wAccountID					= 0x0000;
	UI16 wAccountCount				= 0x0000;
	UI32 dwChars[CHARACTERCOUNT]	= {INVALIDSERIAL,INVALIDSERIAL,INVALIDSERIAL,INVALIDSERIAL,INVALIDSERIAL,INVALIDSERIAL};
	actb.reset();
	m_wHighestAccount				= 0x0000;

	while( !fsAccountsADM.eof() && !fsAccountsADM.fail() )
	{
		// Process each Line read in from the accounts.adm file. Handle Comments first
		if( sLine[0]=='\\'||sLine[0]==';'||sLine[0]=='/'||sLine[0]=='\''||sLine[0]==0x13||sLine[0]==0x00 )
		{
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// Keep track of the section account lines. There is nothing to process till at least one of these has been read
		if( sLine.substr( 0, 15 ) == "SECTION ACCOUNT" )
		{
			// Increment the account acount
			++wAccountCount;
			actb.wAccountIndex = wAccountID = strutil::value<std::int32_t>(strutil::extractSection(sLine, " ", 2 ));

			// Scan for hiegest account. Needed for additional accounts.
			if( actb.wAccountIndex>m_wHighestAccount )
				m_wHighestAccount=actb.wAccountIndex;
			// Ok, we have parsed out the account ID, Set bBraces[2] to true to allow block reading
			bBraces[0] = false;
			bBraces[1] = false;
			bBraces[2] = true;
			// Get the next line and continue
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// Fail safe. If bBraces[2] isn't set there is no need to even run this code
		if( !bBraces[2] )
		{
			// Make sure the get the next line to process.
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		// If we get here then were reading a block now. Check for the openning brace.
		if( sLine[0]=='{'&&!bBraces[0]&&!bBraces[1] )
		{
			bBraces[0]=true;
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		if( ((sLine[0]=='{') &&bBraces[0]) ||((sLine[0]=='{')&&bBraces[1]) )
		{
			fsAccountsADM.close();
			return 0x0000;
		}
		// Loop reading this block till the end brace is encountered.
		if( sLine[0]=='}'&&bBraces[0]&&!bBraces[1] )
		{
			// Peel the dwChars Array here
			CAccountBlock actbTemp;
			CAccountBlock actbTempName;
			MAPUSERNAME_ITERATOR J;
			MAPUSERNAMEID_ITERATOR I = m_mapUsernameIDMap.find( actb.wAccountIndex );
			if( I != m_mapUsernameIDMap.end() )
			{
				// OK there is an id in memory so we should check to see if its the same.
				actbTemp=I->second;
				J=m_mapUsernameMap.find(actbTemp.sUsername);
				actbTempName=(*J->second);
				for( UI08 ii = 0; ii < CHARACTERCOUNT; ++ii )
				{
					// Doesn't matter, at all times the accounts.adm file will be valid over memory
					if( actbTemp.dwCharacters[ii]!=INVALIDSERIAL )
					{
						actbTemp.dwCharacters[ii] = actbTempName.dwCharacters[ii] = dwChars[ii];
						actbTempName.lpCharacters[ii]=actbTemp.lpCharacters[ii]=calcCharObjFromSer(dwChars[ii]);
						if( actbTemp.lpCharacters[ii]!=nullptr&&actbTempName.lpCharacters[ii]!=nullptr )
						{
							actbTemp.lpCharacters[ii]->SetAccount( actbTemp );
							actbTempName.lpCharacters[ii]->SetAccount( actbTempName );
						}
					}
				}
			}
			else
			{
				// OK well since its not in memory then its safe to assume that we need to add this one
				actbTemp = actbTempName = actb;
				for( UI08 jj = 0; jj < CHARACTERCOUNT; ++jj )
				{
					actbTemp.dwCharacters[jj] = actbTempName.dwCharacters[jj] = dwChars[jj];
					actbTemp.lpCharacters[jj] = actbTempName.lpCharacters[jj] = calcCharObjFromSer(dwChars[jj]);
					if( actbTemp.lpCharacters[jj]!=nullptr&&actbTempName.lpCharacters[jj]!=nullptr )
					{
						actbTemp.lpCharacters[jj]->SetAccount( actbTemp );
						actbTempName.lpCharacters[jj]->SetAccount( actbTempName );
					}
				}
			}
			actb.wAccountIndex=actbTemp.wAccountIndex=wAccountID;
			actbTempName.wAccountIndex=wAccountID;
			// Ok we shoud shove this into the map(s) for use later
			m_mapUsernameIDMap[wAccountID]=actbTemp;
			m_mapUsernameMap[actb.sUsername]=&m_mapUsernameIDMap[wAccountID];
			actb.reset();

			// Ok we have finished with this access block clean up and continue processing
			bBraces[0] = false;
			bBraces[1] = false;
			bBraces[2] = false;
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			for( UI08 kk = 0; kk < CHARACTERCOUNT; ++kk )
			{
				dwChars[kk]=INVALIDSERIAL;
			}
			continue;
		}
		auto ssecs = strutil::sections( sLine, " " );
		auto l	= strutil::stripTrim( ssecs[0] );
		auto r	= strutil::stripTrim( ssecs[1] );
		// Parse and store based on tag
		if( l == "NAME" )
		{
			// Ok strip the name and store it. We need to make it all the same case for comparisons
			if( !r.empty() && r.length() != 0 )
			{
				actb.sUsername = strutil::tolower( r );
			}
			else
			{
				actb.sUsername = "ERROR";
			}

			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "PASS" )
		{
			if( !r.empty() && r.length() != 0 )
			{
				// Ok strip the password and store it.
				actb.sPassword = r;
				// Now we have to parse the crap in access.adm. Were not gonna look at much format just the 2 tags we need.
			}
			else
			{
				// r was not valid when passed in. Deal with it here
			}
			// Ok we have finished with this access block clean up and continue processing
			bBraces2[0] = false;
			bBraces2[1] = false;
			bBraces2[2] = false;
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "FLAGS" )
		{
			if( !r.empty() && r.length() != 0 )
			{
				actb.wFlags = static_cast<UI16>(std::stoul(r, nullptr, 0));
				if( actb.wAccountIndex == 0 )
				{
					actb.wFlags.set( AB_FLAGS_GM, true );
				}
			}
			else
			{
				actb.wFlags.set( AB_FLAGS_PUBLIC, true );
			}

			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "PATH" )
		{
			// Ok strip the name and store it. We need to make it all the same case for comparisons
			if( !r.empty() && r.length() != 0 )
			{
				actb.sPath = r;
			}
			else
			{
				actb.sPath = "ERROR";
			}

			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "TIMEBAN" )
		{
			if( !r.empty() && r.length() != 0 )
			{
				actb.wTimeBan = static_cast<UI32>(std::stoul(r, nullptr, 0));
			}
			else
			{
				actb.wTimeBan = 0;
			}

			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "CONTACT" )
		{
			// Ok strip the name and store it. We need to make it all the same case for comparisons
			if( !r.empty() && r.length() != 0 )
			{
				actb.sContact = strutil::tolower( r );
			}
			else
			{
				actb.sContact = "UNKNOWN";
			}

			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l == "LASTIP" )
		{
			auto psecs = strutil::sections( r, "." );
			if( !r.empty() && r.length() != 0 && psecs.size() == 4 )
			{
				actb.dwLastIP = calcserial( strutil::value<SI08>(psecs[0]), strutil::value<SI08>(psecs[1]), strutil::value<SI08>(psecs[2]), strutil::value<SI08>(psecs[3]) );
			}
			else
			{
				actb.dwLastIP = 0x00000000;
			}

			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		else if( l.substr( 0, 10 ) == "CHARACTER-" )	// It's a character
		{
			auto charNum = static_cast<UI08>(std::stoul(l.substr(10), nullptr, 0));
			if( charNum < 1 || charNum > CHARACTERCOUNT )
			{
				Console.error( "Invalid character found in accounts" );
			}
			else
			{
				if( !r.empty() && r.length() != 0 )
				{
					// Ok strip the name and store it. We need to make it all the same case for comparisons
					dwChars[charNum-1] = static_cast<UI32>(std::stoul(r, nullptr, 0));
					std::getline( fsAccountsADM, sLine );
					continue;
				}
				else
				{
					dwChars[charNum-1] = INVALIDSERIAL;
				}
			}
			std::getline( fsAccountsADM, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		std::getline( fsAccountsADM, sLine );
		sLine = strutil::stripTrim( sLine );
		actb.reset();
	}
	// We need to see if there are any new accounts to come.
	UI16 wImportCount = 0x0000;
	wImportCount = ImportAccounts();
	// Return the number of accounts loaded
	return static_cast<UI16>(m_mapUsernameMap.size());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool TransCharacter( UI16 wSAccountID, UI16 wSSlot, UI16 wDAccountID )
//|	Date		-	1/7/2003 5:39:12 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Transfer a character from a known slot on a specified
//|							account, to a new account and slot.
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::TransCharacter( UI16 wSAccountID, UI16 wSSlot, UI16 wDAccountID )
{
	//   please leave these declarations alone. When we do this VC autocompetion doesn't work cause this isn't shoved on the stack with type info
	MAPUSERNAMEID_ITERATOR I = m_mapUsernameIDMap.find(wSAccountID);
	//
	if( I==m_mapUsernameIDMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	CAccountBlock& actbID	= I->second;
	// Ok now we need to get the matching username map
	MAPUSERNAME_ITERATOR J	= m_mapUsernameMap.find(actbID.sUsername);
	if( J==m_mapUsernameMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.

	// ok we will check to see if there is a valid char in source slot, if not we will return
	if( actbID.dwCharacters[wSSlot]==INVALIDSERIAL )
		return false;
	//
	MAPUSERNAMEID_ITERATOR II = m_mapUsernameIDMap.find( wDAccountID );
	if( II == m_mapUsernameIDMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	CAccountBlock& actbIID	= II->second;
	// Ok now we need to get the matching username map
	MAPUSERNAME_ITERATOR JJ	= m_mapUsernameMap.find(actbIID.sUsername);
	if( JJ==m_mapUsernameMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.

	// ok at this point I/II = SourceID, and DestID and J/JJ SourceName/DestName
	if( AddCharacter( wDAccountID, actbID.dwCharacters[wSSlot], actbID.lpCharacters[wSSlot] ) )
	{
		// OK the character was added, need to remove it from the source.
		DelCharacter( wSAccountID, (SI32)wSSlot );
		Save( false );
		return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool AddCharacter( UI16 wAccountID, CChar *lpObject )
//|					bool AddCharacter( UI16 wAccountID, UI32 dwCharacterID, CChar *lpObject )
//|	Date		-	12/18/2002 2:09:04 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Add a character object to the in memory storage.
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::AddCharacter( UI16 wAccountID, CChar *lpObject )
{
	// Make sure that the lpObject pointer is valid
	if( lpObject==nullptr )
		return false;
	// Ok we need to do is get see if this account id exists
	MAPUSERNAMEID_ITERATOR I = m_mapUsernameIDMap.find( wAccountID );
	if( I == m_mapUsernameIDMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	CAccountBlock& actbID	= I->second;
	// Ok now we need to get the matching username map
	MAPUSERNAME_ITERATOR J	= m_mapUsernameMap.find(actbID.sUsername);
	if( J==m_mapUsernameMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.
	CAccountBlock& actbName = (*J->second);
	// ok now that we have both of our account blocks we can update them. We will use teh first empty slot for this character
	bool bExit = false;
	for( UI08 i = 0; i < CHARACTERCOUNT; ++i )
	{
		if( actbID.dwCharacters[i]!=lpObject->GetSerial()&&actbName.dwCharacters[i]!=lpObject->GetSerial()&&actbID.dwCharacters[i]==INVALIDSERIAL&&actbName.dwCharacters[i]==INVALIDSERIAL )
		{
			// ok this slot is empty, we will stach this character in this slot
			actbID.lpCharacters[i]=lpObject;
			actbID.dwCharacters[i]=lpObject->GetSerial();
			actbName.lpCharacters[i]=lpObject;
			actbName.dwCharacters[i]=lpObject->GetSerial();
			// we do not need to continue throught this loop anymore.
			bExit=true;
			break;
		}
	}
	// If we were successfull then we return true
	if( bExit )
	{
		// make sure to put the values back into the maps corrected.
		m_mapUsernameIDMap[actbID.wAccountIndex] = actbID;
		return true;
	}
	return false;
}
//
bool cAccountClass::AddCharacter( UI16 wAccountID, UI32 dwCharacterID, CChar *lpObject )
{
	// Make sure that the lpObject pointer is valid
	if( lpObject==nullptr )
		return false;
	// Ok we need to do is get see if this account id exists
	MAPUSERNAMEID_ITERATOR I;
	I = m_mapUsernameIDMap.find(wAccountID);
	if( I==m_mapUsernameIDMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this ID
	CAccountBlock& actbID = I->second;
	// Ok now we need to get the matching username map
	MAPUSERNAME_ITERATOR J = m_mapUsernameMap.find(actbID.sUsername);
	if( J==m_mapUsernameMap.end() )
	{
		// This ID was not found.
		return false;
	}
	// Get the account block for this username.
	CAccountBlock& actbName = (*J->second);
	// ok now that we have both of our account blocks we can update them. We will use teh first empty slot for this character
	bool bExit=false;
	for( UI08 i = 0; i < CHARACTERCOUNT; ++i )
	{
		if( actbID.dwCharacters[i]!=lpObject->GetSerial()&&actbName.dwCharacters[i]!=lpObject->GetSerial()&&actbID.dwCharacters[i]==INVALIDSERIAL&&actbName.dwCharacters[i]==INVALIDSERIAL )
		{
			// ok this slot is empty, we will stach this character in this slot
			actbID.lpCharacters[i]		= lpObject;
			actbID.dwCharacters[i]		= dwCharacterID;
			actbName.lpCharacters[i]	= lpObject;
			actbName.dwCharacters[i]	= dwCharacterID;
			// we do not need to continue throught this loop anymore.
			bExit=true;
			break;
		}
	}
	// If we were successfull then we return true
	if( bExit )
	{
		// make sure to put the values back into the maps corrected.
		m_mapUsernameIDMap[actbID.wAccountIndex]=actbID;
		Save(false);
		return true;
	}
	return false;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool cAccountClass::clear( void )
//|	Date		-	12/18/2002 2:24:07 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This function is used to clear out all entries contained
//|						in both the Username, and UsernameID map structures.
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::clear( void )
{
	// First we should check to make sure that we can even use the objects, or they are not already cleared
	try
	{
		if( m_mapUsernameMap.empty() || m_mapUsernameIDMap.empty() )
		{
			return false;
		}
		// ok clear the map
		m_mapUsernameIDMap.clear();
		m_mapUsernameMap.clear();
		m_wHighestAccount=0x0000;
	}
	catch( ... )
	{
		return false;
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool cAccountClass::DelAccount( std::string sUsername )
//|					bool cAccountClass::DelAccount( UI16 wAccountID )
//|	Date		-	12/18/2002 2:56:34 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove an account from the internal account storage map.
//|						At this point this function will only remove the account
//|						block from the accounts.adm and not the physical files
//|						on the storage medium.
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::DelAccount( std::string sUsername )
{
	// Ok were just going to get the ID number for this account and make the ID function do all the work
	MAPUSERNAME_ITERATOR I = m_mapUsernameMap.find(sUsername);
	if( I==m_mapUsernameMap.end() )
		return false;
	CAccountBlock& actbTemp = (*I->second);
	// Ok lets do the work now
	return DelAccount( actbTemp.wAccountIndex );
}
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::DelAccount( UI16 wAccountID )
{
	// Ok we need to get the ID block again as it wasn't passed in
	MAPUSERNAMEID_ITERATOR I = m_mapUsernameIDMap.find(wAccountID);
	if( I==m_mapUsernameIDMap.end() )
		return false;
	CAccountBlock& actbID=I->second;
	// Now we need to get the matching username map entry
	MAPUSERNAME_ITERATOR J = m_mapUsernameMap.find(actbID.sUsername);
	if( J==m_mapUsernameMap.end() )
		return false;

	// Before we delete this account we need to spin the characters, and list them in the orphan.adm file
	std::string sTempPath(m_sAccountsDirectory);
	if( sTempPath[sTempPath.length()-1]=='\\'||sTempPath[sTempPath.length()-1]=='/' )
		sTempPath += "orphans.adm";
	else
		sTempPath += "/orphans.adm";
	// First lets see if the file exists.
	bool bOrphanHeader=false;
	std::fstream fsOrphansADMTest(sTempPath.c_str(),std::ios::in);
	if( !fsOrphansADMTest.is_open() )
		bOrphanHeader=true;
	fsOrphansADMTest.close();
	// ok open the stream for append and add this record to the end of the file
	std::fstream fsOrphansADM(sTempPath.c_str(),std::ios::out|std::ios::app);
	if( !fsOrphansADM.is_open() )
		return false;
	// If this is a new file then we need to write the header first
	if( bOrphanHeader )
	{
		WriteOrphanHeader( fsOrphansADM );
	}
	// Spin the characters here
	for( UI08 jj = 0; jj < CHARACTERCOUNT; ++jj )
	{
		// If this slot is 0xffffffff or (-1) then we dont need to put it into the orphans.adm
		if( actbID.dwCharacters[jj] != INVALIDSERIAL ) // Ok build then write what we need to the file
			fsOrphansADM << "," << actbID.lpCharacters[jj]->GetName() << "," << actbID.lpCharacters[jj]->GetX() << "," << actbID.lpCharacters[jj]->GetY() << "," << (SI32)actbID.lpCharacters[jj]->GetZ() << std::endl;
	}
	fsOrphansADM.close();
	// Ok we have both the map iterators pointing to the right place. Erase these entries
	m_mapUsernameIDMap.erase(I);
	m_mapUsernameMap.erase(J);
	// Just a means to show that an accounts has been removed when looking at the directories
	char szDirName[40];
	char szDirPath[MAX_PATH];
	memset(szDirName,0x00,sizeof(SI08)*40);
	memset(szDirPath,0x00,sizeof(SI08)*MAX_PATH);
	// Ok copy only the username portion to this so we can build a correct rename path
	strcpy(szDirName,&actbID.sPath[actbID.sPath.length()-actbID.sUsername.length()-1]);
	strncpy(szDirPath,actbID.sPath.c_str(),actbID.sPath.length()-actbID.sUsername.length()-1);
	std::string sNewDir(szDirPath);
	sNewDir += "_";
	sNewDir += szDirName;
	[[maybe_unused]] int retVal = rename(actbID.sPath.c_str(),sNewDir.c_str());
	// We have to run a save to make sure that the accoung it removed.
	Save( false );
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SetPath( std::string sPath )
//|	Date		-	12/19/2002 12:29:59 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Set the internal string to contain the path to the accounts
//|						directory. This path much not contain a filename, As there
//|						is no checking implemented.
//o-----------------------------------------------------------------------------------------------o
bool cAccountClass::SetPath( const std::string &sPath )
{
	try
	{
		m_sAccountsDirectory = sPath;
		return true;
	}
	catch( ... )
	{
		return false;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string cAccountClass::GetPath( void )
//|	Date		-	12/19/2002 12:34:01 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Return a copy of the current accounts directory stored
//|						internally
//o-----------------------------------------------------------------------------------------------o
std::string cAccountClass::GetPath( void )
{
	return m_sAccountsDirectory;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 DelCharacter( UI16 wAccountID, UI08 nSlot )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a character from the accounts map. Currently this
//|						character object resource is not freed. The Character ID,
//|						and CChar object holders will be set to -1, and nullptr
//|						respectivly. As a consolation the characters will be for
//|						the interim listed in the orphan.adm file. This file will
//|						contain simply the username, and character ID that was
//|						removed. For those that are wondering why this might be.
//|						Until the system itself deletes the character and items
//|						at least we will have a listing of orphaned character in
//|						game that a GM can use to locate, or even assign to another
//|						account.
//o-----------------------------------------------------------------------------------------------o
SI08 cAccountClass::DelCharacter( UI16 wAccountID, UI08 nSlot )
{
	// Do the simple here, save us some work
	if( nSlot >= CHARACTERCOUNT )
		return CDR_BADREQUEST;

	// ok were going to need to get the respective blocked from the maps
	MAPUSERNAMEID_ITERATOR I = m_mapUsernameIDMap.find(wAccountID);
	if( I == m_mapUsernameIDMap.end() )
		return CDR_CHARNOTFOUND;
	CAccountBlock& actbID = I->second;

	// Ok now that we have the ID Map block we can get the Username Block
	MAPUSERNAME_ITERATOR J = m_mapUsernameMap.find(actbID.sUsername);
	if( J == m_mapUsernameMap.end() )
		return CDR_BADPASSWORD;
	CAccountBlock& actbName=(*J->second);

	// Check to see if this record has been flagged changed
	// Edit: What's this supposed to do? All it does is prevent players from deleting their character on the first try... then it succeedes on the second try
	/*if( actbID.bChanged )
	{
		I->second.bChanged = false;
		return CDR_CHARISQUEUED;
	}*/

	// We have both blocks now. We should validate the slot, and make the changes
	if( actbID.dwCharacters[nSlot] == INVALIDSERIAL || actbName.dwCharacters[nSlot] == INVALIDSERIAL )
		return CDR_CHARNOTFOUND;

	// We need to make a entry in the orphan.adm file for this block before we change it
	std::string sTempPath(m_sAccountsDirectory);
	if( sTempPath[sTempPath.length() - 1] == '\\' || sTempPath[sTempPath.length() - 1] == '/' )
		sTempPath += "orphans.adm";
	else
		sTempPath += "/orphans.adm";

	// First lets see if the file exists.
	bool bOrphanHeader=false;
	std::fstream fsOrphansADMTest(sTempPath.c_str(), std::ios::in);
	if( !fsOrphansADMTest.is_open() )
		bOrphanHeader = true;
	fsOrphansADMTest.close();

	// ok open the stream for append and add this record to the end of the file
	std::fstream fsOrphansADM(sTempPath.c_str(), std::ios::out|std::ios::app);
	if( !fsOrphansADM.is_open() )
		return CDR_BADREQUEST;

	// If this is a new file then we need to write the header first
	if( bOrphanHeader )
	{
		WriteOrphanHeader(fsOrphansADM);
	}

	// Ok build then write what we need to the file
	fsOrphansADM << actbID.sUsername << "=0x" << std::hex << actbID.dwCharacters[nSlot] << "," 
		<< (actbID.lpCharacters[nSlot] != nullptr ? actbID.lpCharacters[nSlot]->GetName() : "UNKNOWN") << ",0x" 
		<< (actbID.lpCharacters[nSlot] != nullptr ? actbID.lpCharacters[nSlot]->GetX() : 0) << ",0x" 
		<< (actbID.lpCharacters[nSlot] != nullptr ? actbID.lpCharacters[nSlot]->GetY() : 0) << "," << std::dec 
		<< (actbID.lpCharacters[nSlot] != nullptr ? (SI32)actbID.lpCharacters[nSlot]->GetZ() : (SI32)0) << std::endl;
	fsOrphansADM.close();

	// Ok there is something in this slot so we should remove it.
	actbID.dwCharacters[nSlot] = actbName.dwCharacters[nSlot] = INVALIDSERIAL;
	actbID.lpCharacters[nSlot] = actbName.lpCharacters[nSlot] = nullptr;

	// need to reorder the accounts or have to change the addchar code to ignore invalid serials(earier here)
	CAccountBlock actbScratch;
	actbScratch.reset();
	actbScratch = actbID;
	SI32 kk = 0;

	// Dont mind this loop, becuase we needed a copy of the data too we need to invalidate actbScracthes pointers, and indexs
	for( UI08 ll = 0; ll < CHARACTERCOUNT; ++ll )
	{
		if( actbID.dwCharacters[ll] != INVALIDSERIAL && actbID.lpCharacters[ll] != nullptr )
		{
			// OK we keep this entry
			actbScratch.dwCharacters[kk] = actbID.dwCharacters[ll];
			actbScratch.lpCharacters[kk] = actbID.lpCharacters[ll];
			kk += 1;
		}
	}

	// Fill the rest with standard empty values.
	UI08 jj = 0;
	for( jj = kk; jj < CHARACTERCOUNT; ++jj )
	{
		actbScratch.dwCharacters[jj] = INVALIDSERIAL;
		actbScratch.lpCharacters[jj] = nullptr;
	}

	// Now copy back out the info to the structures
	for( jj = 0; jj < CHARACTERCOUNT; ++jj )
	{
		actbID.dwCharacters[jj] = actbName.dwCharacters[jj] = actbScratch.dwCharacters[jj];
		actbID.lpCharacters[jj] = actbName.lpCharacters[jj] = actbScratch.lpCharacters[jj];
	}
	actbID.bChanged = actbName.bChanged = true;

	// Now we have to put the values back into the maps
	try
	{
		I->second = actbID;
		[[maybe_unused]] MAPUSERNAMEID_ITERATOR Q = m_mapUsernameIDMap.find(actbID.wAccountIndex);
		Save( false );
	}
	catch( ... )
	{
		return CDR_BADREQUEST;
	}
	return CDR_SUCCESS;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CAccountBlock& GetAccountByName( std::string sUsername )
//|	Date		-	12/19/2002 2:16:37 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Search the map for an account but the username that was specified.
//o-----------------------------------------------------------------------------------------------o
CAccountBlock& cAccountClass::GetAccountByName( std::string sUsername )
{
	// Ok now we need to get the map blocks for this account.
	MAPUSERNAME_ITERATOR I = m_mapUsernameMap.find(sUsername);
	if( I != m_mapUsernameMap.end() )
	{
		CAccountBlock &actbName = (*I->second);
		// Get the block from the ID map, so we can check that they are the same.
		MAPUSERNAMEID_ITERATOR J = m_mapUsernameIDMap.find( actbName.wAccountIndex );
		if( J != m_mapUsernameIDMap.end() )
		{
			CAccountBlock& actbID = J->second;
			// Check to see that these both are equal where it counts.
			if( actbID.sUsername == actbName.sUsername || actbID.sPassword == actbName.sPassword )
				return actbName;
		}
	}
	return actbInvalid;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CAccountBlock& cAccountClass::GetAccountByName( UI16 wAccountID )
//|	Date		-	12/19/2002 2:17:31 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Search the map for an account but the username that was specified.
//o-----------------------------------------------------------------------------------------------o
CAccountBlock& cAccountClass::GetAccountByID( UI16 wAccountID )
{
	// Ok now we need to get the map blocks for this account.
	MAPUSERNAMEID_ITERATOR I = m_mapUsernameIDMap.find(wAccountID);
	if( I != m_mapUsernameIDMap.end() )
	{
		CAccountBlock &actbID = I->second;
		// Get the block from the ID map, so we can check that they are the same.
		MAPUSERNAME_ITERATOR J = m_mapUsernameMap.find( actbID.sUsername );
		if( J != m_mapUsernameMap.end() )
		{
			CAccountBlock& actbName = (*J->second);
			// Check to see that these both are equal where it counts.
			if( actbID.sUsername == actbName.sUsername || actbID.sPassword == actbName.sPassword )
				return actbID;
		}
	}
	return actbInvalid;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 Save( bool bForceLoad )
//|	Date		-	12/19/2002 2:45:39 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save the contents of the internal structures out to a flat
//|							file where it can be loaded later, and archived for later
//|							use should a crash occur and the userfile is damaged. At
//|							this stage this function will only write out the accounts.adm
//|							file, even though the access.adm will still eventually
//|							be used for server sharing.
//o-----------------------------------------------------------------------------------------------o
UI16 cAccountClass::Save( bool bForceLoad )
{
	// Ok were not going to mess around. so we open truncate the file and write
	std::string sTemp(m_sAccountsDirectory);
	if( sTemp[sTemp.length()-1]=='\\'||sTemp[sTemp.length()-1]=='/' )
		sTemp += "accounts.adm";
	else
		sTemp += "/accounts.adm";
	sTemp = strutil::replaceSlash(sTemp);
	// Ok we have the path, now open the file and start saving
	std::fstream fsAccountsADM(sTemp.c_str(),std::ios::out|std::ios::trunc);
	if( !fsAccountsADM.is_open() )
		return 0x0000;
	// OK first we need to write the header.
	WriteAccountsHeader(fsAccountsADM);
	// Now we need to iterate through each account block in the map, and save
	MAPUSERNAMEID_ITERATOR CI;
	for( CI = m_mapUsernameIDMap.begin();CI!=m_mapUsernameIDMap.end();++CI )
	{
		// Get a usable structure for this iterator
		CAccountBlock& actbID		= CI->second;
		// Ok we are going to load up each username block from that map too for checking
		MAPUSERNAME_ITERATOR JI	= m_mapUsernameMap.find( actbID.sUsername );
		CAccountBlock& actbName		= (*JI->second);
		// Check to make sure at least that the username and passwords match
		if( actbID.sUsername != actbName.sUsername || actbID.sPassword != actbName.sPassword )
		{
			// there was an error between blocks
			Console.error( strutil::format("Save(): Mismatch %s - %s (Duplicate username in accounts file?)", actbID.sUsername.c_str(), actbName.sUsername.c_str()) );
			fsAccountsADM.close();
			return 0xFFFF;
		}
		// Ok write this block to the file
		WriteAccountSection( actbID, fsAccountsADM );
		// Need to check to see if the path is valid.
		std::fstream fsTest(actbID.sPath.c_str(),std::ios::in);
		bool bPathExists=fsTest.is_open();
		fsTest.close();
		// Make sure that if there is no path that we make one
		if( !actbID.sPath.length()||!bPathExists )
		{
			std::string sTempPath(m_sAccountsDirectory);
			if( sTempPath[sTempPath.length()-1]=='\\'||sTempPath[sTempPath.length()-1]=='/' )
			{
				auto szTempBuff	= strutil::tolower( actbID.sUsername );
				sTempPath			+= szTempBuff;
				sTempPath			+= "/";
				sTempPath = strutil::replaceSlash( sTempPath );
				actbID.sPath		= sTempPath;
			}
			else
			{
				auto szTempBuff	= strutil::tolower( actbID.sUsername );
				sTempPath			+= "/";
				sTempPath			+= szTempBuff;
				sTempPath			+= "/";
				sTempPath = strutil::replaceSlash( sTempPath );
				actbID.sPath		= sTempPath;
			}
		}
		// Close the test path
		// Ok now that we got here we need to make the directory, and create the username.uad file
		if (!std::filesystem::exists(std::filesystem::path(actbID.sPath))) {
			auto create_status = std::filesystem::create_directory(std::filesystem::path(actbID.sPath));
			if (!create_status) {
				Console.error( strutil::format("Save(): Couldn't create directory %s", actbID.sPath.c_str()));
				fsAccountsADM << "// !!! Couldn't save .uad file !!!" << std::endl;
				continue;

			}
		}

		// Ok now thats finished. We need to do one last thing. Create the username.uad file in the account directory
		std::string sUsernameUADPath(actbID.sPath);
		if( sUsernameUADPath[sUsernameUADPath.length()-1]=='\\'||sUsernameUADPath[sUsernameUADPath.length()-1]=='/' )
		{
			sUsernameUADPath += actbID.sUsername;
			sUsernameUADPath += ".uad";
		}
		else
		{
			sUsernameUADPath += "/";
			sUsernameUADPath += actbID.sUsername;
			sUsernameUADPath += ".uad";
		}
		// Fix the paths to make sure that all the characters are unified
		sUsernameUADPath = strutil::replaceSlash(sUsernameUADPath);
		// Open the file in APPEND to end mode.
		std::fstream fsAccountsUAD(sUsernameUADPath.c_str(),std::ios::out|std::ios::trunc);
		if( !fsAccountsUAD.is_open() )
		{
			// Ok we were unable to open the file so this user will not be added.
			Console.error( strutil::format("Save(): Couldn't open file %s", sUsernameUADPath.c_str() ));
			fsAccountsADM << "// !!! Couldn't save .uad file !!!" << std::endl;
			continue;
		}
		// Ok we have to write the new username.uad file in the directory
		WriteUADHeader(fsAccountsUAD,actbID);
		// Ok write out the characters and the charcter names if we know them
		for( UI08 ii = 0; ii < CHARACTERCOUNT; ++ii )
		{
			fsAccountsUAD << "CHARACTER-" << (ii+1) << " 0x" << std::hex << (actbID.dwCharacters[ii] != INVALIDSERIAL ? actbID.dwCharacters[ii]:INVALIDSERIAL) << " [" << (char*)(actbID.lpCharacters[ii] != nullptr ? actbID.lpCharacters[ii]->GetName().c_str() : "INVALID" ) << "]\n";
		}
		// Close the files since we dont need them anymore
		fsAccountsUAD.close();
	}
	// Ok were done looping so we can close the file and return the count of accounts
	fsAccountsADM.close();
	return static_cast<UI16>(m_mapUsernameIDMap.size());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 ImportAccounts( void )
//|	Date		-	4/30/2003 2:32:13 PM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Scan for a file with the name of "newaccounts.adm" and if
//|						this file exists then parsed for the new accounts which
//|						will be added to the internal account maps. If this file
//|						does not exist then no action will take place and will
//|						return 0. Otherwise this function will return the total
//|						count of accounts that were added. Please note that the
//|						file will be deleted after it is parsed, and duplicates
//|						will not be allowed.
//|
//|	NOTES       -	Format: user=username,password,flags,emailaddress
//|
//|						Where:. Username, Password, Flags, and Email are all selfexplanitory.
//|
//o-----------------------------------------------------------------------------------------------o
UI16 cAccountClass::ImportAccounts( void )
{
	// Prepare to load in the newaccounts.adm file. This file if it exists will contain new accounts to be added to the server.
	std::string sImportAccounts(m_sAccountsDirectory);
	sImportAccounts += (m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='\\'||m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='/')?"newaccounts.adm":"/newaccounts.adm";
	sImportAccounts = strutil::replaceSlash(sImportAccounts);
	// Check to see what version the current accounts.adm file is.
	std::string sLine;
	std::fstream fsInputAccountsTest( sImportAccounts.c_str(), std::ios::in );
	if( !fsInputAccountsTest.is_open() )
	{
		// we were unable to load any accounts
		return 0x0000;
	}
	// Now that we have a file, we want to read it, and add these accounts to the system
	std::getline( fsInputAccountsTest, sLine );
	sLine = strutil::stripTrim( sLine );
	UI16 wAccountCount = 0x0000;
	UI16 wCurrentFlags = 0x0004;
	while( !fsInputAccountsTest.eof() && !fsInputAccountsTest.fail() )
	{
		// Process each Line read in from the accounts.adm file. Handle Comments first
		if( sLine.empty() || sLine.length() == 0 )
		{
			std::getline( fsInputAccountsTest, sLine );
			sLine = strutil::stripTrim( sLine );
			continue;
		}
		auto esecs = strutil::sections( sLine, "=" );
		auto l = strutil::stripTrim( esecs[0] );
		auto r = strutil::stripTrim( esecs[1] );
		if( l == "USER" )
		{
			// OK we have an account to import, start parsing it.
			std::string user, pass;
			auto csecs = strutil::sections( r, "," );
			
			user = strutil::stripTrim( csecs[0] );
			if( csecs.size() > 1 )
			{
				pass = strutil::stripTrim( csecs[1] );
			}

			if( user.empty() || user.length() == 0 || pass.empty() || pass.length() == 0 )
			{
				// error there are no NULLS allowed for usernames, passwords. So we move to the next.
				// NOTE! This record will be dropped onces parsing is completed as file is deleted.
				std::getline( fsInputAccountsTest, sLine );
				sLine = strutil::stripTrim( sLine );
				continue;
			}
			
			std::string flags;
			if( csecs.size() > 2 )
			{
			  flags = strutil::stripTrim( csecs[2] );
			}
			
			// Set flags to a default value. and in this case I believe that its 0x00000004
			if( flags.empty() || flags.length() == 0 )
			{
				wCurrentFlags = 0x0000;
			}
			else
			{
				wCurrentFlags = static_cast<UI16>(std::stoul(flags, nullptr, 0));
			}
			
			std::string email;
			if( csecs.size() > 3 )
			{
				email = strutil::stripTrim( csecs[3] );
			}

			if( email.empty() || email.length() == 0 )
			{
				email = "N/A";
			}
			if( AddAccount( user, pass, email, wCurrentFlags ) == 0x0000 )
			{
				// As requested we are going to stuff back accounts into their own file
				std::string sOutputBadAccounts(m_sAccountsDirectory);
				sOutputBadAccounts += (m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='\\'||m_sAccountsDirectory[m_sAccountsDirectory.length()-1]=='/')?"failed_accounts.log":"/failed_accounts.log";
				sOutputBadAccounts = strutil::replaceSlash(sOutputBadAccounts);
				// Open the file and append this to the end
				std::fstream fsOutputBadAccounts(sOutputBadAccounts.c_str(),std::ios::app);
				if( !fsOutputBadAccounts.is_open() )
				{
					// The bad accounts file wasn't writable or something so were going to skip it
					std::getline( fsInputAccountsTest, sLine );
					sLine = strutil::stripTrim( sLine );
					continue;
				}
				// Write the failed line to the file
				fsOutputBadAccounts << sLine << std::endl;
				fsOutputBadAccounts.close();
				// OK there was a problem entering this accounts into the system. Possibly a duplicate? or Other issues
				Console << "NOTICE: New account was not processed. Please see failed_accounts.log for details." << myendl;
				std::getline( fsInputAccountsTest, sLine );
				sLine = strutil::stripTrim( sLine );
				continue;
			}
			wAccountCount++;
		}
		// Need to make sure we get the next line
		std::getline( fsInputAccountsTest, sLine );
		sLine = strutil::stripTrim( sLine );
	}
	// Make sure to close the file
	fsInputAccountsTest.close();
	// No that we have this step completed, we need to empty the file, and write a header
	std::fstream fsWriteHeader( sImportAccounts.c_str(), std::ios::out );
	if( !fsWriteHeader.is_open() )
	{
		// we were unable to load any accounts
		return wAccountCount;
	}
	// Write the header and close.
	WriteImportHeader( fsWriteHeader );
	fsWriteHeader.close();
	// Return total of accounts that were successfully added
	return wAccountCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	MAPUSERNAME_ITERATOR Begin( void )
//|	Date		-	1/14/2003 5:47:28 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the First iterator in a set. If there is no record
//|							then END() will be returned. This function will set the
//|							internal Iterator to the first record or will indicate
//|							end().
//o-----------------------------------------------------------------------------------------------o
MAPUSERNAMEID_ITERATOR& cAccountClass::begin( void )
{
	I = m_mapUsernameIDMap.begin();
	return I;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	MAPUSERNAME_ITERATOR End( void )
//|	Date		-	1/14/2003 5:48:32 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the Last iterator in a set. This function forces
//|						the internal iterator to one past the last record. It
//|						will also return End() as a result.
//o-----------------------------------------------------------------------------------------------o
MAPUSERNAMEID_ITERATOR& cAccountClass::end( void )
{
	I = m_mapUsernameIDMap.end();
	return I;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	MAPUSERNAMEID_ITERATOR& Last( void )
//|	Date		-	1/14/2003 6:03:22 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	For those that need a means to get the last valid record
//|						in the container, without having to process the Iterator
//|						and back it up one record. By default the map will return
//|						end() when there is no record to return for us.
//|
//|						NOTE: This will update the current internal iterator to
//|						the last record as well.
//o-----------------------------------------------------------------------------------------------o
MAPUSERNAMEID_ITERATOR& cAccountClass::last( void )
{
	I = m_mapUsernameIDMap.end();
	--I;
	return I;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	cAccountClass& cAccountClass::operator++()
//|					cAccountClass& cAccountClass::operator--(SI32)
//|	Date		-	1/14/2003 5:33:13 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Use this operators to control the internal iterator that
//|						points into the UsernameID map. This will work only on
//|						this map. It will be assumed that the UsernameMap will
//|						be matching this exactally.
//o-----------------------------------------------------------------------------------------------o
cAccountClass& cAccountClass::operator++()
{
	// just increment I, the rest will be handled internally
	I++;
	return (*this);
}
//
cAccountClass& cAccountClass::operator--(SI32)
{
	I--;
	return (*this);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AccountsHeader( fstream &fsOut )
//|	Date		-	12/19/2002 2:56:36 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes the Accounts.Adm header to the specified output
//|						stream.
//o-----------------------------------------------------------------------------------------------o
void cAccountClass::WriteAccountsHeader( std::fstream &fsOut )
{
	fsOut << "//AV3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" << CVersionClass::GetBuild() << "-DS" << time(nullptr) << "-ED" << CVersionClass::GetRealBuild() << std::endl;
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
	fsOut << "//         LASTIP 127.0.0.1" << std::endl;
	fsOut << "//         CONTACT NONE" << std::endl;
	fsOut << "//         CHARACTER-1 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-2 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-3 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-4 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-5 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-6 0xffffffff" << std::endl;
	fsOut << "//         CHARACTER-7 0xffffffff" << std::endl;
	fsOut << "//      }" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   FLAGS: " << std::endl;
	fsOut << "//      Bit:  0x0001) Banned           0x0002) Suspended        0x0004) Public           0x0008) Currently Logged In" << std::endl;
	fsOut << "//            0x0010) Char-1 Blocked   0x0020) Char-2 Blocked   0x0040) Char-3 Blocked   0x0080) Char-4 Blocked" << std::endl;
	fsOut << "//            0x0100) Char-5 Blocked   0x0200) Char-6 Blocked   0x0400) Char-7 Blocked   0x0800) Unused" << std::endl;
	fsOut << "//            0x1000) Unused           0x2000) Seer             0x4000) Counselor        0x8000) GM Account" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   TIMEBAN: " << std::endl;
	fsOut << "//      This would be the end date of a timed ban." << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   CONTACT: " << std::endl;
	fsOut << "//      Usually this is the email address, but can be used as a comment or ICQ" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   LASTIP: " << std::endl;
	fsOut << "//      The last IP this account was used from." << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WriteAccessHeader( std::fstream &fsOut )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes the Access.Adm header to the specified output stream.
//o-----------------------------------------------------------------------------------------------o
void cAccountClass::WriteAccessHeader( std::fstream &fsOut )
{
	fsOut << "//SA3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" << CVersionClass::GetBuild() << "-DS" << time(nullptr) << "-ED" << CVersionClass::GetRealBuild() << std::endl;
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
	fsOut << "//         CONTACT NONE" << std::endl;
	fsOut << "//      }" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//   FLAGS: " << std::endl;
	fsOut << "//      Bit:  0x0001) Banned           0x0002) Suspended        0x0004) Public           0x0008) Currently Logged In" << std::endl;
	fsOut << "//            0x0010) Char-1 Blocked   0x0020) Char-2 Blocked   0x0040) Char-3 Blocked   0x0080) Char-4 Blocked" << std::endl;
	fsOut << "//            0x0100) Char-5 Blocked   0x0200) Char-6 Blocked   0x0400) Char-7 Blocked   0x0800) Unused" << std::endl;
	fsOut << "//            0x1000) Unused           0x2000) Seer             0x4000) Counselor        0x8000) GM Account" << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WriteOrphanHeader( std::fstream &fsOut )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes the Orphan.Adm header to the specified output stream.
//o-----------------------------------------------------------------------------------------------o
void cAccountClass::WriteOrphanHeader( std::fstream &fsOut )
{
	fsOut << "//OI3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" << CVersionClass::GetBuild() << "-DS" << time(nullptr) << "-ED" << CVersionClass::GetRealBuild() << "\n";
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// Orphans.Adm " << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UOX3 uses this file to store any characters that have been removed from" << std::endl;
	fsOut << "// an account. They are stored here so there is some history of user deltions" << std::endl;
	fsOut << "// of their characters. At best the co-ordinate may be available." << std::endl;
	fsOut << "// Name, X/Y/Z values if available will be displayed to ease locating trouble" << std::endl;
	fsOut << "// users, and where they deleted their characters last." << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "// The format is as follows:" << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "//    username=ID,CharacterName,X,Y,Z" << std::endl;
	fsOut << "// " << std::endl;
	fsOut << "// NOTE: CharacterName, and Coordinates may not be available." << std::endl;
	fsOut << "//------------------------------------------------------------------------------\n";
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WriteUADHeader( std::fstream &fsOut, CAccountBlock& actbTemp )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes the Username.uad header to the specified output stream.
//o-----------------------------------------------------------------------------------------------o
void cAccountClass::WriteUADHeader( std::fstream &fsOut, CAccountBlock& actbTemp )
{
	fsOut << "//AI3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" << CVersionClass::GetBuild() << "-DS" << time(nullptr) << "-ED" << CVersionClass::GetRealBuild() << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UAD Path: " << actbTemp.sPath << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UOX3 uses this file to store any extra administration info\n";
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "ID " << actbTemp.wAccountIndex << std::endl;
	fsOut << "NAME " << actbTemp.sUsername << std::endl;
	fsOut << "PASS " << actbTemp.sPassword << std::endl;
	fsOut << "BANTIME " << std::hex << "0x" << actbTemp.wTimeBan << std::dec << std::endl;
	fsOut << "LASTIP " << (SI32)((actbTemp.dwLastIP&0xFF000000)>>24) << "." << (SI32)((actbTemp.dwLastIP&0x00FF0000)>>16) << "." << (SI32)((actbTemp.dwLastIP&0x0000FF00)>>8) << "." << (SI32)((actbTemp.dwLastIP&0x000000FF)%256) << std::endl;
	fsOut << "CONTACT " << actbTemp.sContact << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WriteImportHeader( std::fstream &fsOut )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes the Username.uad header to the specified output stream.
//o-----------------------------------------------------------------------------------------------o
void cAccountClass::WriteImportHeader( std::fstream &fsOut )
{
	fsOut << "//AIMP3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" << CVersionClass::GetBuild() << "-DS" << time(nullptr) << "-ED" << CVersionClass::GetRealBuild() << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// UOX3 uses this file to store new accounts that are to be imported on the next" << std::endl;
	fsOut << "// time the server does a world save, or world load." << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
	fsOut << "// FORMAT: " << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "//    USER=username,password,flags,contact" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "// WHERE: username   - Username of the accounts to create." << std::endl;
	fsOut << "//        password   - Password of the account to create." << std::endl;
	fsOut << "//        flags      - See accounts.adm for correct flag values." << std::endl;
	fsOut << "//        contact    - Usually this is the email address, but can be used as a comment or ICQ" << std::endl;
	fsOut << "//" << std::endl;
	fsOut << "// NOTE: Flags, and contact values are not required, defaults will be used." << std::endl;
	fsOut << "// NOTE: Please ensure you press ENTER after your last line for proper loading." << std::endl;
	fsOut << "//------------------------------------------------------------------------------" << std::endl;
}
