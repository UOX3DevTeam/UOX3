#!/bin/sh
echo "touch README"
touch README || exit;
echo "touch NEWS"
touch NEWS || exit;
echo "touch AUTHORS"
touch AUTHORS || exit;
echo "aclocal"
aclocal || exit;
echo "automake --add-missing --copy"
automake --add-missing --copy;
echo "autoconf"
autoconf || exit;
echo "automake"
automake || exit;

echo "Building required linux file js32.a"

cd ../spidermonkey;
# gcc -o jscpucfg jscpucfg.c ; ./jscpucfg >  jsautocfg.h
# gcc -g -c -DXP_UNIX *.c ; ar -r js32.a *.o
make -ref Makefile.ref CC="gcc -fno-stack-protector"
cp Linux_All_DBG.OBJ/jsautocfg.h .
ar -r js32.a Linux_All_DBG.OBJ/*.o
cp js32.a ../source;
cd ../source;

echo "Done building js32.a"

echo "Running configure."
./configure $@
echo "Please do ./configure --help to set options such as --enable-debug"
echo "You can also run make now to compile UOX3. Enjoy!"
