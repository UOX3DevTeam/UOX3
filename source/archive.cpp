//------------------------------------------------------------------------
//  archive.cpp
//
//------------------------------------------------------------------------
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
//------------------------------------------------------------------------
#include "uox3.h"

// Author: Dupois 
// Rewritten by Lord Binary, fixed a crash when using it in  Unix enviroments, 
// and adopted it to Zippy's new file format
// the filenames MUST have be of the form x.y
// but this is the case,cause the files are items.wsc and chars.wsc
   
void fileArchive(char *pFile2Archive_chars, char *pFile2Archive_items, char *pArchiveDir)
{
	char ext[255] = "";
	char ext2[255] = "";
	char *pext = NULL;
	char *pext2 = NULL;
    char destfile[255]="";
	char destfile2[255]="";
	char *pdestfile_c = destfile; 
	char *pdestfile_i = destfile2;
	time_t mytime;
	tm *ptime;

  /*************** chars archiving ************************/

	int	ch = '.';
	int p=0;

	// Determine the extension of the file and save the extension into array ext

	pext = (strchr(pFile2Archive_chars, ch)); //charsfile extension
	if (pext != NULL)
	{
		while (pext[p] != '\0')
		{
			ext[p]=pext[p];
			p++;
		}
	}

	p=0;
	pext2 = (strchr(pFile2Archive_items, ch)); // itemfile extension
	if (pext2 != NULL)
	{
		while (pext2[p] != '\0')
		{
			ext2[p]=pext2[p];
			p++;
		}
	}


	// Create the destination file name


	// charsfile string
	strcpy(destfile, pArchiveDir);
	strcat(destfile, pFile2Archive_chars);

	pdestfile_c=(strchr(destfile, ch));
	*pdestfile_c=0;
	strcat(destfile, "-");
	pdestfile_c = &destfile[strlen(destfile)];


	// itemsfile string
	strcpy(destfile2, pArchiveDir);
	strcat(destfile2, pFile2Archive_items);

	pdestfile_i=(strchr(destfile2, ch));
	*pdestfile_i=0;
	strcat(destfile2, "-");
	pdestfile_i = &destfile2[strlen(destfile2)];

	time(&mytime);
	ptime=localtime(&mytime); // get local time
	char *timenow=asctime(ptime);   // convert it to a string
//	timenow = asctime( ptime ); // convert it to a string
    
	// overwriting the characters that aren't allowed in paths
	for (int a=0;a<strlen(timenow);a++)
	  if (isspace(timenow[a]) || timenow[a]==':')
	    timenow[a]='-';

	strcat(destfile, timenow);
	strcat(destfile, ext);

	strcat(destfile2,timenow);
	strcat(destfile2,ext2);

    // printf("File2Archive_chars:%s\n%s\n",pFile2Archive_chars,destfile);
	// printf("File2Archive_items:%s\n%s\n",pFile2Archive_items,destfile2);

	// Rename/Move the pFile2Archive to destfile

	if( rename( pFile2Archive_chars, destfile ) != 0 )
		printf( "UOX3: Err-Could not rename/move file.\nFile '%s' or directory '%s' may not exist.\n", pFile2Archive_chars, pArchiveDir );
	else
		printf( "UOX3: '%s' renamed/moved to '%s'\n", pFile2Archive_chars, destfile );


    if( rename( pFile2Archive_items, destfile2 ) != 0 )
		printf( "UOX3: Err-Could not rename/move file.\nFile '%s' or directory '%s' may not exist.\n", pFile2Archive_items, pArchiveDir );
	else
		printf( "UOX3: '%s' renamed/moved to '%s'\n", pFile2Archive_items, destfile2 );


	printf("finished backup\n");
//	delete timenow;
	return;
}
