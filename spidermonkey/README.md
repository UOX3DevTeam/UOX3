# SpiderMonkey 1.7 configured for UOX3

Herein are configured a minimum set of files required to compile a static library of SpiderMonkey 1.7 on Windows, Linux/FreeBSD and macOS, for linking with [Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/) and utilized to access Ultima Online data files and packets. Build setups can be found in the make/ folder, but extraneous build files and folders have been removed from this configuration. For the latest full, official version of the SpiderMonkey project - which is not compatible with UOX3 - visit https://spidermonkey.dev

For detailed instructions on compiling and using this SpiderMonkey configuration with UOX3, check out [Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/). Alternatively, follow the below instructions at your own peril:

<details>
  <summary>Windows</summary>

  ### Visual Studio 2017/2022
  1. Open jscript.sln in the *make/[VS2017/VS2022]/jscript* folder
  2. Choose *Release/Debug* build type from dropdown menu
  3. Go *Build -> Build jscript*

  ### CMake
  1. Open Developer Command Prompt for VS2017 or Developer Powershell for VS2022
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