#!/bin/sh

# Support optional arguments:
#   -b debug
#   -o clean
buildtype="Release"
buildoption="Normal"
while getopts ":b:o:" flag
do
  case "${flag}" in
    b)
      if [ ${OPTARG} = "debug" ] || [ ${OPTARG} = "Debug" ]
      then
        buildtype="Debug"
      fi;;
    o)
      if [ ${OPTARG} = "clean" ] || [ ${OPTARG} = "Clean" ]
      then
        buildoption="Clean"
      fi;;
  esac
done

# if -o clean was provided, do a clean build
if [ $buildoption = "Clean" ]
then
  echo "Preparing for clean build..."
  rm -R make/cmake/build/
fi

echo "Creating Build directory"
cd make/cmake
mkdir -p build
cd build

echo "Creating Make Files"
if [ "$(uname)" = "Darwin" ]
then
  # Mac OS X
  cmake .. -DCMAKE_BUILD_TYPE=$buildtype -G"Unix Makefiles"
else 
	cmake .. -DCMAKE_BUILD_TYPE=$buildtype
fi

echo "Building UOX3 ($buildtype)..."
cmake --build . --config $buildtype

ev=$?
if [ $ev -ne 0 ]; then
  echo "Unable to build UOX3 ($buildtype)! Exiting..."
  cd ../../..
  exit $ev
else
  if [ -f ./uox3 ]; then
    cp uox3 ../../..
    echo "Done! You should now find the compiled uox3 binary in the root UOX3 project directory. Copy this binary"
    echo "to a separate directory dedicated to running your UOX3 shard, along with the contents of the UOX3/data directory,"
    echo "to avoid potential git conflicts and accidental overwriting of data when pulling UOX3 updates in the future."    
    cd ../../..
    rm -R make/cmake/build/
    exit $ev
  else
    echo "uox3 program not found! Please review the build status."
    cd ../../..
  fi
fi
