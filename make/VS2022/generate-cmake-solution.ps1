$ErrorActionPreference = "Stop"

$sourceDir = Resolve-Path (Join-Path $PSScriptRoot "..\cmake")
$buildDir = Join-Path $PSScriptRoot "..\..\build-vs2022"

cmake --preset vs2022-clang -S $sourceDir
if ($LASTEXITCODE -ne 0) {
    throw "CMake configuration failed. Install Visual Studio's C++ Clang tools first."
}

$solution = Join-Path $buildDir "uox3.sln"
Write-Host "Generated $solution"
Start-Process $solution
