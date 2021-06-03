#!/usr/bin/env bash
echo "Building spidermonkey"
cd spidermonkey
make -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=gcc

if [ "$(uname)" = "Darwin" ]
then
        # Mac OS X
        ar rcs libjs32.a Darwin_DBG.OBJ/*.o
        cp Darwin_DBG.OBJ/jsautocfg.h ./
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]
then
        # Linux
        ar -r libjs32.a Linux_All_DBG.OBJ/*.o
        cp Linux_All_DBG.OBJ/jsautocfg.h ./
fi
cd ../zlib
echo "Bulding zlib"
make distclean
./configure 
make 

cd ../source
echo "Building UOX3"
make
cp uox3 ..
cd ..
echo "Done! You should now find the compiled uox3 binary in the root UOX3 project directory. Copy this binary to a separate directory dedicated to running your UOX3 shard, along with the contents of the UOX3/data directory, to avoid potential git conflicts and accidental overwriting of data when pulling UOX3 updates in the future."