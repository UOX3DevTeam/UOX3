//------------------------------------------------------------------------
//  scriptc.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Unknown real name (erwin)
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
/*
 Cache script section locations by <erwin@andreasen.com>
 Reads through the contents of the file and saves location of each
 SECTION XYZ entry

 Calling Script::find() will then seek to that location directly rather
 than having to parse through all of the script
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "uox3.h"
//#include "List.h"
//#include "scriptcache.h"
#include "debug.h"
#include <assert.h>

#define DBGFILE "scriptc.cpp"

extern FILE* scpfile;

// This may not be portable to non-POSIX systems?
char get_modification_date(const char *filename, time_t* mod_time) {
    struct stat stat_buf;
    
    if ((stat(filename, &stat_buf)))
        return 0;

    *mod_time = stat_buf.st_mtime;
    return 1;
}

void Script::reload() {
    FILE *fp;
    char buf[1024], section_name[256];
    int count = 0;
    
    // i'll just bypass the problems by deleting the old and reallocating a new one
    // by flushing the old one - fur
    assert(entries);
    entries->flush();	// this removes data, but not structure...

    
    if (!(fp = fopen(filename, "r"))) {
        fprintf(stderr, "Cannot open %s: %s", filename, strerror(errno));
        exit(1);
    }
    printf ("Reloading %-15s: ", filename); fflush (stdout);

    // Snarf the part of SECTION... until EOL
    while(fgets(buf, sizeof(buf), fp))
        if (sscanf(buf, "SECTION %256[^\n]", section_name) == 1) {
            entries->insert(new ScriptEntry(section_name, ftell(fp)));	
            count++;
        }

    printf ("%6d sections found.\n", count);
    
    fclose(fp);
}

// Parse this script, caching section positions
Script::Script(const char *_filename) {
    filename = strdup(_filename);
//Try to fix Ming error    entries = NULL;
    entries = new List<ScriptEntry,1>;
    
    if (!(get_modification_date(filename, &last_modification))) {
        fprintf(stderr, "Cannot stat %s: %s", filename, strerror(errno));
        exit(1);
    }

    reload();
}

// be sure to free the filename and the list of entries
Script::~Script()
{
	free(filename);
	delete entries;
}

// Look for that section in this previously parsed script file
char Script::find(const char *section) {
    time_t current;
    ScriptEntry *sc;
    
    if (!get_modification_date(filename, &current)) {
        fprintf(stderr, "Cannot stat %s: %s", filename, strerror(errno));
        exit(1);
    }

    if (current > last_modification) {
        reload();
        last_modification = current;
    }

#ifndef __LINUX__
    for (sc = entries->rewind(); sc; sc = entries->next())
	{
		if( strlen( sc->name ) == strlen( section ) )
		{
			if(!strncmp( sc->name, section, strlen(section)))
	            break;
		}
	}
#else
	for( sc = entries->rewind(); sc; sc = entries->next() )
	{
		if( !strncmp( sc->name, section, strlen( section ) ) )
			break;
	}
#endif
    if (!sc)
        return 0;

    fseek(scpfile, sc->offset, SEEK_SET);
    return 1;
}
// Look for that section in this previously parsed script file
char Script::isin(const char *section) {
    time_t current;
    ScriptEntry *sc;
    
    if (!get_modification_date(filename, &current)) {
        fprintf(stderr, "Cannot stat %s: %s", filename, strerror(errno));
        exit(1);
    }

    if (current > last_modification) {
        reload();
        last_modification = current;
    }

    for (sc = entries->rewind(); sc; sc = entries->next())
        if (strstr(sc->name,section))
        {
          fseek(scpfile, sc->offset, SEEK_SET);
          return 1;
        }

    return 0;

}

