//========================================================================
//FILE: utilsys.cpp
//========================================================================
//	Copyright (c) 2001 by Sheppard Norfleet and Charles Kerr
//  All Rights Reserved
// 
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided the following conditions are met:
//
//	Redistributions of source code must retain the above copyright notice,
//	this list of conditions and the following disclaimer. Redistributions
//	in binary form must reproduce the above copyright notice, this list of
//	conditions and the following disclaimer in the documentation and/or
//	other materials provided with the distribution.
//
//	Neither the name of the SWORDS  nor the names of its contributors may
//	be used to endorse or promote products derived from this software
//	without specific prior written permission. 
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  `AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	A PARTICULAR PURPOSE ARE DISCLAIMED. .
//========================================================================
#include "utilsys.h"
//========================================================================
// Begin getNormalizedTime
UI32 getNormalizedTime()
{
	static UI32	uiOffset= 0 ;
	static bool bOneShot = false ;

	if (!bOneShot)
	{
		uiOffset = getPlatformTime() ;
		bOneShot = true ;
	}

	return getPlatformTime() - uiOffset ;
}

//Begin getRealTimeString()
string getRealTimeString()
{
	string sTime ;
#if defined(OBSOLETETIME)
	timeb stTimeB;
	::ftime(&stTimeB);
	sTime = ctime((time_t*)&stTimeB.time);
#else
	timeval stTimeV ;
	gettimeofday(&stTimeV,NULL) ;
	sTime = ctime((time_t*)&stTimeV.tv_sec) ;
#endif

	return sTime ;
}
// end


//Begin getPlatformTime
UI32 getPlatformTime()
{
	UI32 uiBaseTime ;
#if defined(OBSOLETETIME)
	timeb stTimeB;
	::ftime(&stTimeB);
	uiBaseTime= static_cast<UI32>(stTimeB.time*1000)+static_cast<UI32>(stTimeB.millitm);
#else
	timeval stTimeV ;
	gettimeofday(&stTimeV,NULL) ;
	uiBaseTime= static_cast<UI32>(stTimeV.tv_sec*1000)+static_cast<UI32>(stTimeV.tv_usec/1000) ;
#endif
	return uiBaseTime ;
}
//End of getPlatformTime

// Begin getPlatformDay()
UI32 getPlatformDay()
{
        UI32 uiTime ;
        uiTime = getPlatformTime() ;
        return uiTime/8640000  ; // (1sec/1000mill * 1min/60secs * 1hr/60min * 1day/24hr) ;
}

//========================================================================
//Begin makeDirectory
bool makeDirectory(string sDirectory)
{
	bool bStatus = false ;
	SI32 siCode ;
	// First determine if the directory all ready exists
	
	if(!isDirectory(sDirectory))
	{
		mode_t mMode = 0777 ;
		siCode = _mkdir(sDirectory.c_str(), mMode);
        if (siCode == 0)
		{	// we made it
			bStatus = true ;
		}
		else
		{
			bStatus = false ;
		}
	}
	else
	{
		// Directory all ready exists
		bStatus = true ;
	}
	
	return bStatus ;
}
//End of makeDirectory
//=========================================================================
//Begin isDirectory
bool isDirectory(string sDirectory)
{
	bool bStatus = false ;
	SI32 siCode ;

	// We do a stat on the name
	struct _stat stStat ;
	if ((siCode = _stat(sDirectory.c_str(),&stStat)) ==0)
	{ 
		if (S_ISDIR(stStat.st_mode)) 
		{
			bStatus = true ;
		}
	}	

	return bStatus ;
}
//End of isDirectory
//=========================================================================
//Begin listDirectory
vector<string> listDirectory(string sDirectory)
{
	vector<string> vecDirList ;
	SI32 siStatus ;
	string sName ;
	
#if defined(_POSIX)
	glob_t stDir ;
	stDir.gl_offs =0 ;
	siStatus = glob(sDirectory.c_str(),0,NULL,&stDir) ;
	if (siStatus == 0)
	{
		// Success
		for (size_t siIndex = 0 ; siIndex < stDir.gl_pathc; siIndex++)
		{
			sName = stDir.gl_pathv[siIndex] ;
			vecDirList.push_back(sName) ;
		}
		globfree(&stDir) ;
	}
#elif defined(WIN32)
	// structure to find data
	_finddata_t stFind ;
	SI32 siHandle ;
	string sBase ;
	// We need under windows to get the base direcotry, to add back
	sBase = sDirectory.substr(0,sDirectory.find_last_of("\\/") + 1) ;
#if defined(__borland__)
	char* szTemp ;
	szTemp = new char[sDirectory.size() + 1] ;
	for (UI32 uiBozo=0; uiBozo < sDirectory.size() ; uiBozo++)
	{
		szTemp[uiBozo] = sDirectory[uiBozo] ;
	}
	szTemp[sDirectory.size()] = 0 ;
	siStatus = _findfirst(szTemp,&stFind) ;
	delete[] szTemp ;
#else
	siStatus = _findfirst(sDirectory.c_str(),&stFind) ;
#endif
	siHandle = siStatus ;
	
	while (siStatus != -1)
	{
		// Data found
		sName = sBase + stFind.name ;
		
		vecDirList.push_back(sName) ;
		siStatus = _findnext(siHandle,&stFind) ;
	}
	if (siHandle != -1)
	{
		_findclose(siHandle) ;
	}
#endif 
	
	return vecDirList ;
}
//End of listDirectory
//===========================================================================
//Begin deleteDirectory
bool deleteDirectory(string sDirectory)
{
	bool bValue = false ;
	SI32 siCode ;
	// Delete any files we have in that directory
	string sFiles = sDirectory + "*" ;
	vector<string> vecFiles ;
	vecFiles = listDirectory(sFiles) ;
	for (UI32 siIndex = 0 ; siIndex < vecFiles.size() ; siIndex++)
	{
		deleteFile(vecFiles[siIndex]) ;
	}
	vecFiles.clear() ;
	
	if ((siCode = _rmdir(sDirectory.c_str())) == 0)
		bValue = true ;

	return bValue ;
}
//End of deleteDirectory
//===========================================================================
//Begin deleteFile
bool deleteFile(string sFile)
{
	bool bValue = false ;

	SI32 siCode ;

	if ((siCode = remove(sFile.c_str())) == 0)
		bValue = true ;

	return bValue ;
}
//End of deleteFile
//===========================================================================
//===========================================================================
//=======================  End of utilsys.cpp  ================================
//===========================================================================
//===========================================================================
