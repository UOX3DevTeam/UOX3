#!/usr/bin/env bash
if (( $# == 0 )); then
  # No parameters, all stages
  stage=0
else
  stage="$1"
fi

if [ $stage -eq 0 ] || [ $stage -eq 1 ]; then
  echo "Building spidermonkey"
  cd spidermonkey
  if [ "$(uname)" = "FreeBSD" ]
  then
    gmake -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=clang
  else
    make -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=gcc
  fi
  
  ev=$?
  if [ $ev -ne 0 ]; then
    echo "Unable to build spidermonkey, cannot continue"
    exit $ev
  fi
  if [ "$(uname)" = "Darwin" ]
  then
          # Mac OS X
          ar rcs libjs32.a Darwin_DBG.OBJ/*.o
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
  cd ..
  if [ $stage -eq 1 ]; then exit 0; fi
fi

if [ $stage -eq 0 ] || [ $stage -eq 2 ]; then
  cd zlib
  echo "Bulding zlib"
  make distclean
  ./configure
  make
  
  ev=$?
  if [ $ev -ne 0 ]; then
    echo "Unable to build zlib, cannot continue"
    exit $ev
  fi
  cd ..
  if [ $stage -eq 2 ]; then exit 0; fi
fi

if [ $stage -eq 0 ] || [ $stage -eq 3 ]; then
  cd source
  echo "Building UOX3"
  if [ "$(uname)" = "FreeBSD" ]
  then
    gmake
  else
    make
  fi
  ev=$?
  if [ -f ./uox3 ]; then
    cp uox3 ..
    echo "Done! You should now find the compiled uox3 binary in the root UOX3 project directory. Copy this binary to a separate directory dedicated to running your UOX3 shard, along with the contents of the UOX3/data directory, to avoid potential git conflicts and accidental overwriting of data when pulling UOX3 updates in the future."
  else
    echo "uox3 output not created!  Please review compile status"
  fi
  cd ..
  exit $ev
fi
