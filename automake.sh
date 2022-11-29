#!/usr/bin/env sh

echo "Building SpiderMonkey..."
cd spidermonkey
if [ "$(uname)" = "FreeBSD" ]
then
  gmake -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=clang
else
  make -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=gcc
fi

ev=$?
if [ $ev -ne 0 ]; then
  echo "Unable to build SpiderMonkey! Exiting..."
  exit $ev
else
  echo "Done building SpiderMonkey."
fi

echo "Creating JavaScript library..."
if [ "$(uname)" = "Darwin" ] # macOS
then
  libtool -static -o libjs32.a -s Darwin_DBG.OBJ/*.o
  cp Darwin_DBG.OBJ/jsautocfg.h ./
elif [ "$(uname)" = "FreeBSD" ]
then
  ar rcs libjs32.a FreeBSD_DBG.OBJ/*.o
  cp FreeBSD_DBG.OBJ/jsautocfg.h ./
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]
then
  # Linux
  ar -r libjs32.a Linux_All_DBG.OBJ/*.o
  cp Linux_All_DBG.OBJ/jsautocfg.h ./
fi

ev=$?
if [ $ev -ne 0 ]; then
  echo "Unable to create JavaScript library! Exiting..."
  exit $ev
else
  echo "Done creating JavaScript library."
fi

echo "Bulding zlib..."
cd ../zlib
make distclean
./configure && make

ev=$?
if [ $ev -ne 0 ]; then
  echo "Unable to build zlib! Exiting..."
  exit $ev
else
  echo "Done building zlib."
fi

echo "Building UOX3..."
cd ../source
if [ "$(uname)" = "FreeBSD" ]
then
  gmake
else
  make
fi

ev=$?
if [ $ev -ne 0 ]; then
  echo "Unable to build UOX3! Exiting..."
  exit $ev
else
  if [ -f ./uox3 ]; then
    cp uox3 ..
    echo "Done! You should now find the compiled uox3 binary in the root UOX3 project directory. Copy this binary"
    echo "to a separate directory dedicated to running your UOX3 shard, along with the contents of the UOX3/data directory,"
    echo "to avoid potential git conflicts and accidental overwriting of data when pulling UOX3 updates in the future."
  else
    echo "uox3 program not found! Please review the build status."
  fi
fi
cd ..
exit $ev
