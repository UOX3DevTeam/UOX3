@echo off

IF "%1" == "" (
 echo You must specify a path to install to.
 goto end
)

call copydata %1

IF "%2"=="y" (
 call buildbinary
)

call copybinary %1

IF "%3"=="y" (
 call copysource %1
)

:end