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
	@echo make linuxmgpp          - Compile Linux/UNIX binary minus -lg++
mingw32:
	$(MAKE) $(EXE)

cross:
	$(MAKE) $(EXE) "CC=gnuwin32gcc"

linux:
	$(MAKE) uox3_dep "EXE=uox3" "LIBS=-lm -lg++"

linuxmgpp:
	$(MAKE) uox3_dep "EXE=uox3" "LIBS=-lm"

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
	
