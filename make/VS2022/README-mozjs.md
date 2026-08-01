# Visual Studio 2022 build

Install these prerequisites:

- Visual Studio 2022 with **Desktop development with C++**
- **C++ Clang Compiler for Windows** under **Individual components**
- **MSBuild support for LLVM (clang-cl) toolset** under **Individual components**

Open the checked-in solution directly:

- Double-click `make\VS2022\uox3.sln`.
- Select `Debug`, `Release`, or `ReleaseLTO` and the `x64` platform.
- Build the `uox3` project.

`ReleaseLTO` is the performance-optimized configuration.

The executable is written to `make\VS2022\x64\<configuration>\uox3.exe`.

Command-line equivalent from a Visual Studio developer terminal:

```powershell
msbuild make\VS2022\uox3.sln /m /p:Configuration=Release /p:Platform=x64
```

Replace `Release` with `ReleaseLTO` to build the performance-optimized configuration.
