# zlib-1.2.11 configured for UOX3

Herein are configured a minimum set of files required to compile a static library of zlib-1-2.11 on Windows, Linux and macOS, to be linked with [Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/). Extraneous files and folders not required for this have been removed from this configuration; for the full, official zlib project complete with compilation solutions for other platforms and third party contributions, visit https://github.com/madler/zlib

For detailed instructions on compiling and using this zlib configuration with UOX3, check out https://github.com/UOX3DevTeam/UOX3. Alternatively, follow the below instructions at your own peril:

<details>
  <summary>Windows 10 (and 7/8?)</summary>

  To compile on Windows, open zlib.sln (VS2017 or newer) in the root zlib folder, choose your build type (release/debug, x64/x86) and go to Build -> Build zlib-static

</details>

<details>
  <summary>Linux/macOS</summary>

  To compile on Linux/macOS, open a new terminal window, navigate to the root zlib directory, and use the following commands:
./configure
make

</details>