//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  fileio.cpp
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

#include "fileio.h"

#define IOBUFFLEN 2048

UOXFile::UOXFile(char *fileName, char *mode)
{
  char  localMode[16];

  fmode = -1, ok = 0;

  typedef unsigned char UCHAR;
  ioBuff = new UCHAR[IOBUFFLEN];

  memset( ioBuff, 0x00, sizeof( BYTE ) * IOBUFFLEN );

  if(ioBuff != NULL)
  {
    strcpy(localMode,mode);

    if(*mode == 'r') fmode = 0;
    else if(*mode == 'w') fmode = 1;

    theFile = fopen(fileName, localMode);

    if(theFile == NULL) { ok = 0; return; }
    else bSize = bIndex = IOBUFFLEN, ok = 1;  
  }
}

UOXFile::~UOXFile()
{
  if(ioBuff!=NULL) 
    delete[] ioBuff;
  if(theFile) 
    fclose(theFile);
}

void UOXFile::rewind()
{
  fseek(theFile, 0, SEEK_SET);
  bSize = bIndex = IOBUFFLEN;
}

void UOXFile::seek(long offset, int whence)
{
  if(whence == SEEK_SET || whence == SEEK_CUR || whence == SEEK_END)
  {
    fseek(theFile, offset, whence);
    bSize = bIndex = IOBUFFLEN;
  }
}

int UOXFile::getch(void)
{
  if(qRefill()) refill();

  if(bSize != 0) return ioBuff[(int)bIndex++];
  else return -1;
}

void UOXFile::refill()
{
  bSize = fread(ioBuff, sizeof(char), IOBUFFLEN, theFile);
  bIndex = 0;
}

char *UOXFile::gets(char *lnBuff, int lnSize)
{
  if(fmode == 0)
  {
    int i;

    lnSize--;
    i = 0;
    do
    {
      if(qRefill()) refill();

      while(i < lnSize && bIndex != bSize && ioBuff[(int)bIndex] != 0x0A)
      {
        if(ioBuff[(int)bIndex] == 0x0D) bIndex++;
        else lnBuff[i++] = ioBuff[(int)bIndex++];
      }

      if(ioBuff[(int)bIndex] == 0x0A) 
      {
        bIndex++;
        if(i != lnSize) lnBuff[i++] = 0x0A;
        break;
      }

      if(i == lnSize) { break; }
    } while(bSize != 0);

    lnBuff[i] = '\0';

    if(bSize != IOBUFFLEN && i == 0) return NULL;

    return lnBuff;
  }
  else return NULL;
}

int UOXFile::puts(char *lnBuff)
{
  if(fmode == 1)
  {
    if(lnBuff)
     return fwrite(lnBuff, sizeof(char), strlen(lnBuff), theFile);
  }

  return -1;
}

void UOXFile::getUChar(unsigned char *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
    buff[i] = this->getch();
}

void UOXFile::getChar(signed char *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
    buff[i] = this->getch();
}

void UOXFile::getUShort(unsigned short *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    buff[i] = this->getch();
    buff[i] |= this->getch() << 8;
  }
}

void UOXFile::getShort(short *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    buff[i] = this->getch();
    buff[i] |= this->getch() << 8;
  }
}

void UOXFile::getULong(UI32 *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    buff[i] = this->getch();
    buff[i] |= this->getch() << 8;
    buff[i] |= this->getch() << 16;
    buff[i] |= this->getch() << 24;
  }
}

void UOXFile::getLong(SI32 *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    buff[i] = this->getch();
    buff[i] |= this->getch() << 8;
    buff[i] |= this->getch() << 16;
    buff[i] |= this->getch() << 24;
  }
}
/*
** this implementation of tell is broken since it doesn't take into account that
** the buffer may already hold xxx number of characters. so i've taken it out for now
** (nothing was really needing to use it) - fur

long UOXFile::tell()
{
  return ftell(theFile);
}

int UOXFile::getChkSum()
{
  long pos, chksum;

  pos = ftell(theFile);
  rewind();
  ftell(theFile);
  chksum = 0L;
  do
  {
    if(qRefill()) refill();
    while(bIndex != bSize) chksum += ioBuff[(int)bIndex++];
  }
  while(bSize != 0);

  fseek(theFile, pos, SEEK_SET);

  return chksum;
}

int UOXFile::getLength()
{
  long currentPos = ftell(theFile);
  
  fseek(theFile, 0L, SEEK_END);

  long pos = ftell(theFile);

  fseek(theFile, currentPos, SEEK_SET);

  return pos-1;
}
*/

void UOXFile::get_versionrecord(struct versionrecord *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getLong(&buff[i].file);
    getLong(&buff[i].block);
    getLong(&buff[i].filepos);
    getLong(&buff[i].length);
    getLong(&buff[i].unknown);
  }
}
/*
** More info from Alazane & Circonian on this...
**
Index entry:
DWORD: File ID (see index below)
DWORD: Block (Item number, Gump number or whatever; like in the file)
DWORD: Position (Where to find this block in verdata.mul)
DWORD: Size (Size in Byte)
DWORD: Unknown (Perhaps some CRC for the block, most blocks in UO files got this) 

    File IDs: (* means used in current verdata)
00 - map0.mul
01 - staidx0.mul
02 - statics0.mul
03 - artidx.mul
04 - art.mul*
05 - anim.idx
06 - anim.mul
07 - soundidx.mul
08 - sound.mul
09 - texidx.mul
0A - texmaps.mul
0B - gumpidx.mul
0C - gumpart.mul*
0D - multi.i
*/

void UOXFile::get_st_multi(struct st_multi *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getShort(&buff[i].tile);
    getShort(&buff[i].x);
    getShort(&buff[i].y);
    getChar((signed char *) &buff[i].z);
    getChar((signed char *) &buff[i].empty);
    getLong(&buff[i].visible);
  }
}

void UOXFile::get_land_st(struct land_st *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getChar((signed char *) &(buff[i].flag1));
    getChar((signed char *) &(buff[i].flag2));
    getChar((signed char *) &(buff[i].flag3));
    getChar((signed char *) &(buff[i].flag4));
    getChar((signed char *) &(buff[i].unknown1));
    getChar((signed char *) &(buff[i].unknown2));
    getChar((signed char *) buff[i].name, 20);
  }
}

void UOXFile::get_tile_st(struct tile_st *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getUChar(&(buff[i].flag1));
    getUChar(&(buff[i].flag2));
    getUChar(&(buff[i].flag3));
    getUChar(&(buff[i].flag4));
    getUChar(&(buff[i].weight));
    getChar(&(buff[i].layer));
    getLong(&(buff[i].unknown1));
    getLong(&(buff[i].animation));
    getChar(&(buff[i].unknown2));
    getChar(&(buff[i].unknown3));
    getChar(&(buff[i].height));
    getChar(buff[i].name, 20);
  }
}

void UOXFile::get_map_st(struct map_st *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getShort(&buff[i].id);
    getChar(&buff[i].z);
  }
}

void UOXFile::get_st_multiidx(struct st_multiidx *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getULong( &buff[i].start );
    getULong( &buff[i].length );
    getULong( &buff[i].unknown );
  }
}

void UOXFile::get_staticrecord(struct staticrecord *buff, unsigned int number)
{
  for(unsigned int i = 0; i < number; i++)
  {
    getShort(&buff[i].itemid);
    getUChar(&buff[i].xoff);
    getUChar(&buff[i].yoff);
    getChar(&buff[i].zoff);
    short int extra;
    getShort(&extra);
  }
}

