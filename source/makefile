#""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
#  makefile
#
#"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
#  This File is part of UOX3
#  Ultima Offline eXperiment III
#  UO Server Emulation Program
#  
#  Copyright 1997 - 2001 by Marcus Rating (Cironian)
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#	
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#   
#""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
# This is a unified GNU Makfefile for all "other" targets - type "make"
# to get a list of targets!

ifdef WITH_DEP
include depend
endif

CC = gcc

# Optimization
OPT = -O2

# Name of executable
EXE = uox3.exe

# Debugging
#DEBUG = -g

# Libraries
LIBS = -lwsock32

# Warning level
WARN = -w

# Other c-flags
OTHER = -funsigned-char -fconserve-space -fkeep-inline-functions -D__LINUX__ --static

# Source
SRC := $(wildcard *.cpp)

# Objects
OBJS := $(patsubst %.cpp, %.o, $(SRC))

C_FLAGS = $(WARN) $(OTHER) $(OPT) $(DEBUG)

all:
	@echo Please specify one of the following:
	@echo -------------------------------------------------------------------
	@echo make mingw32            - Compile native Win32 binary usign mingw32
	@echo make cross              - Cross compile Win32 binary on Linux
	@echo make linux              - Compile Linux/UNIX binary
	@echo make egcs               - As above, but using egcs - for Redhat!

mingw32:
	$(MAKE) $(EXE)

cross:
	$(MAKE) $(EXE) "CC=gnuwin32gcc"

linux:
	$(MAKE) uox3_dep "EXE=uox3" "LIBS=-lm -lg++"

egcs:
	$(MAKE) uox3_dep "EXE=uox3" "LIBS=-lm -lg++" "CC=egcs"

uox3_dep:  depend
	$(MAKE) uox3 "EXE=$(EXE)" "LIBS=$(LIBS)" "CC=$(CC)" "WITH_DEP=1"

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) -c $(C_FLAGS) $< -o $@

clean:
	rm -f *.o uox3
	rm depend

tags:
	ctag *.cpp *.h

depend:    mkdep
	mkdep *.cpp *h > depend
	
mkdep:
	$(CC) mkdep.c -o mkdep 
	
