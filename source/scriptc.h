//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  scriptc.h
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

// One entry in the cache
struct ScriptEntry {
    char *name;
    unsigned long offset;
    
    ScriptEntry(const char *_name, unsigned long _offset) : name(strdup(_name)), offset(_offset) {}
    ~ScriptEntry() { free(name); }
};

struct Script {
public:
    Script(const char *_filename);
    char find(const char *section);
    char isin(const char *section);
    ~Script();
        
    
private:
    void reload();
    
    List<ScriptEntry, 1> *entries;
    time_t last_modification;
    char *filename;
};
