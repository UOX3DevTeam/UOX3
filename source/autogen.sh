#!/bin/sh
echo "touch README"
touch README || exit;
echo "touch NEWS"
touch NEWS || exit;
echo "touch AUTHORS"
touch AUTHORS || exit;
echo "aclocal"
aclocal || exit;
echo "automake --ad-missing --copy"
automake --add-missing --copy;
echo "autoconf"
autoconf || exit;
echo "automake"
automake || exit;

echo "Building required linux file js32.a"

cd ./mozilla/js/src;
gcc -o jscpucfg jscpucfg.c ; ./jscpucfg >  jsautocfg.h
gcc -c -DXP_UNIX *.c ; ar -r js32.a *.o
cp js32.a ../../../;
cd ../../../;

echo "Done building js32.a"

echo "Running configure."
./configure $@
echo "Please do ./configure --help to set options such as --enable-debug"
echo "You can also run make now to compile UOX3. Enjoy!"
