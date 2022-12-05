#!/bin/sh
echo "Creating Build directory"
mkdir build 
cd build
echo "Creating Make Files"
if [ "$(uname)" = "Darwin" ]
then
        # Mac OS X
        cmake ../cmake -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"
else 
cmake ../cmake -DCMAKE_BUILD_TYPE=Release
fi
cmake --build . --config Release
cp uox3 ../
cd ..
rm -R build/
exit $ev
