cd source\mozilla\js\src
nmake /f js.mak CFG="jsshell - Win32 Release" clean all
nmake /f js.mak CFG="jsshell - Win32 Debug" clean all
copy release\*.dll ..\..\..
copy release\*.lib ..\..\..
cd ..\..\..
set INCLUDE=./mozilla/js/src;%INCLUDE%
nmake /f UOX3_Official.mak CFG="UOX3_Official - Win32 Release"
cd ..
