#!/usr/bin/env bash

cd ../spidermonkey
make -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=gcc

if [ "$(uname)" = "Darwin" ]
then
        # Mac OS X
        ar rcs libjs32.a Dawrwin_DBG.OBJ/*.o
        cp Darwin_DBG.OBJ/jsautocfg.h ./
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]
then
        # Linux
        ar -r libjs32.a Linux_All_DBG.OBJ/*.o
        cp Linux_All_DBG.OBJ/jsautocfg.h ./
fi
cd ../source
make