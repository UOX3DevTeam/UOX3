# Visual Studio 2022 build

Install these prerequisites:

- Visual Studio 2022 with **Desktop development with C++**
- **C++ Clang tools for Windows** in the Visual Studio Installer
- Windows 10 or newer (provides `tar.exe`)

Open the checked-in solution directly:

- Double-click `make\VS2022\uox3.sln`.
- Select `Debug`, `Release`, or `ReleaseLTO` and the `x64` platform.
- Build the `uox3` project.

`ReleaseLTO` enables Clang full optimization, ThinLTO, and linker
reference/COMDAT folding. Use the regular `Release` configuration as the
baseline when comparing world-load and world-save performance.

The executable is written to `make\VS2022\x64\<configuration>\uox3.exe`.

The project extracts the bundled SpiderMonkey 115.13 static library on the
first build. The extracted library and all compiler output remain under the
ignored `make\VS2022\x64` directory. CMake, Rust, and Cargo are not required.

Command-line equivalent from a Visual Studio developer terminal:

```powershell
msbuild make\VS2022\uox3.sln /m /p:Configuration=Release /p:Platform=x64
```

Replace `Release` with `ReleaseLTO` to build the optimized configuration.
