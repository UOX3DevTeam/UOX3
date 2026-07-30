# Visual Studio 2022 mozjs build

Install these prerequisites:

- Visual Studio 2022 with **Desktop development with C++**
- **C++ Clang tools for Windows** in the Visual Studio Installer
- CMake 3.25 or newer

From PowerShell in the repository root, run:

```powershell
.\make\VS2022\generate-cmake-solution.cmd
```

The `.cmd` launcher works even when PowerShell script execution is disabled.

The script generates and opens `build-vs2022\uox3.sln`. Build the `uox3`
project using `Debug`, `Release`, or `RelWithDebInfo` and the `x64` platform.
The executable is written to `build-vs2022\<configuration>\uox3.exe`.

CMake extracts the bundled SpiderMonkey 115.13 static library into the build
directory when the solution is generated. Rust and Cargo are not required.

The older solution stored directly in `make\VS2022` targets the retired
SpiderMonkey libraries and must not be used for the mozjs 115 branch.

Command-line equivalents:

```powershell
Push-Location make\cmake
cmake --preset vs2022-clang
cmake --build --preset vs2022-release
Pop-Location
```
