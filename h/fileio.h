//------------------------------------------------------------------------
//  fileio.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by Unknown real name (Fur)
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

#ifndef __FILEIO_H
#define __FILEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class UOXFile;

#ifndef __UOX3_H
#include "uox3.h"
#endif

#ifndef __CLASSES_H
#include "classes.h"
#endif

class UOXFile
{
  private:

  int fmode, ok, bSize, bIndex;
  unsigned char *ioBuff;
  FILE *theFile;

  public:

  UOXFile(char *, char *);
  ~UOXFile();

  inline int qRefill(void) { return (bIndex >= bSize); };
  inline int ready(void) { return (ok); };
  void rewind(void);
  void seek(long, int);
  inline int eof(void) { return (feof(theFile)); };
  int getch(void);
  void refill(void);
  char *gets(char *, int);
  int puts(char *);
  void getUChar(unsigned char *, unsigned int);
  void getUChar(unsigned char *c) { getUChar((unsigned char *) c, 1); }

  void getChar(signed char *, unsigned int);
  void getChar(signed char *c) { getChar(c, 1); }

  void getUShort(unsigned short *, unsigned int = 1);
  void getShort(signed short *, unsigned int = 1);
  void getULong(UI32 *, unsigned int = 1);
  void getLong(SI32 *, unsigned int = 1);
  //long tell(void);
  //int getChkSum(void);
  //int getLength(void);

  void get_versionrecord(struct versionrecord *, unsigned int = 1);
  void get_st_multi(struct st_multi *, unsigned int = 1);
  void get_land_st(struct land_st *, unsigned int = 1);
  void get_tile_st(struct tile_st *, unsigned int = 1);
  void get_map_st(struct map_st *, unsigned int = 1);
  void get_st_multiidx(struct st_multiidx *, unsigned int = 1);
  void get_staticrecord(struct staticrecord *, unsigned int = 1);
};

#endif
