# zlib-1.3.2 configured for UOX3

Herein are configured a minimum set of files from the official **zlib v1.3.2** release required to compile a static library on Windows, Linux/FreeBSD and macOS, for linking with [Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/) and utilized to access Ultima Online data files and packets. UOX3-specific build setups can be found in the make/ folder, but extraneous upstream build files and folders have been removed from this configuration. For the full official zlib project, including build configurations for other platforms and third-party contributions, visit https://github.com/madler/zlib

For detailed instructions on compiling and using this zlib configuration with UOX3, check out https://github.com/UOX3DevTeam/UOX3. Alternatively, follow the below instructions at your own peril:

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
