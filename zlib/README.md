# zlib-1.3.2 configured for UOX3

Herein is configured a minimum set of files from **zlib v1.3.2** required to compile a static library on Windows, Linux/FreeBSD and macOS, to be linked with [Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/) as a tool for compressing/uncompressing data. UOX3-specific build setups can be found in the make/ folder, but extraneous build files and folders have been stripped from this configuration. For the full official zlib project visit https://github.com/madler/zlib.

This zlib configuration is automatically compiled and linked with UOX3's default build process, but can be compiled as a stand-alone static library by following the instructions below:

<details>
  <summary>Windows</summary>
  
  ### Visual Studio 2022
  1. Open zlib.sln in the *make/VS2022/* folder
  2. Choose *Release* or *Debug* from the build configuration dropdown
  3. Go *Build -> Build zlib-static*
  
  ### CMake
  1. Open Developer PowerShell for VS2022
  2. Traverse to the make/cmake folder, and enter the following commands:
  3. `mkdir build`
  4. `cd build`
  5. `cmake .. -DCMAKE_BUILD_TYPE=Release`
  6. `cmake --build . --config Release`
</details>

---

<details>
  <summary>Linux/FreeBSD/macOS</summary>
  
  ### CMake
  1. Open a new Terminal
  2. Traverse to the make/cmake folder, and enter the following commands:
  3. `mkdir build`
  4. `cd build`
  5. `cmake .. -DCMAKE_BUILD_TYPE=Release` (Linux/FreeBSD)
  5. `cmake .. -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"` (macOS)
  6. `cmake --build . --config Release`
  
</details>
