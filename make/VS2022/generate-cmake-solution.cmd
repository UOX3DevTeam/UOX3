@echo off
setlocal

set "SOURCE_DIR=%~dp0..\cmake"

cmake --preset vs2022-clang -S "%SOURCE_DIR%"
if errorlevel 1 (
    echo CMake configuration failed. Install Visual Studio C++ Clang tools and Rust first.
    exit /b 1
)

set "SOLUTION=%~dp0..\..\build-vs2022\uox3.sln"
echo Generated %SOLUTION%
if /I "%~1"=="--no-open" exit /b 0
start "" "%SOLUTION%"
