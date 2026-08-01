# UOX3
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![Coverity Scan Build Status](https://scan.coverity.com/projects/23322/badge.svg)](https://scan.coverity.com/projects/ultima-offline-experiment-3)

**master** ![Windows Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(windows-latest)) ![Linux Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(ubuntu-latest)) ![macOS Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(macos-latest))

**develop** ![Windows Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(windows-latest)) ![Linux Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(ubuntu-latest)) ![macOS Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(macos-latest))

**Ultima Offline eXperiment 3** - the original open source Ultima Online server emulator, allowing people to run their own, custom UO shards since 1997. Comes with cross-platform 64-bit support for **Windows**, **Linux**, **macOS**, and **FreeBSD**. Grab the latest version from the [Releases](https://github.com/UOX3DevTeam/UOX3/releases) page, while news, more details on releases, forums, additional documentation and more can be found at https://www.uox3.org

Supported UO Client versions: **~4.0.0p** to at least **~7.0.109.0** (with encryption removed by [ClassicUO](https://www.classicuo.eu), [Razor](https://github.com/msturgill/razor/releases) or similar tools). For additional details on UO client compatibility, check https://www.uox3.org/forums/viewtopic.php?f=1&t=2289

UOX3 includes **SpiderMonkey 115.13** for its JavaScript scripting engine and **zlib 1.3.2** for data compression.

Join the [UOX3 Discord](https://discord.gg/uBAXxhF) for support and/or a quick chat!

---

# How to compile UOX3...
## Step 1: Set up a Build Environment
*First, set up a proper build environment with the various tools needed to clone and compile UOX3.*
<details>
  <summary>Install <strong>build tools</strong></summary>

  > * **Windows** - Download and install [Visual Studio 2022 Community](https://visualstudio.microsoft.com/vs/community/) on Windows 10 or newer.
  >   * In the Visual Studio Installer, select the **Desktop development with C++** workload.
  >   * Under **Individual components**, select **C++ Clang Compiler for Windows** and **MSBuild support for LLVM (clang-cl) toolset**.
  > * **Linux (Debian-based)** - Run `sudo apt update && sudo apt install build-essential cmake curl` in a Terminal (or use your distribution's package manager).
  > * **FreeBSD** - Run `pkg install cmake` in a Terminal. Alternatively, build `cmake` via ports if desired.
  > * **macOS** - Install the Xcode command-line tools with `xcode-select --install`, then run `brew install cmake` using [Homebrew](https://brew.sh/).
  > * **Docker/Podman** - Ensure that it is downloaded and installed (Windows) or installed from your package manager
</details>

<details>
  <summary>Install <strong>Git</strong></summary>

  > * **Windows/macOS** - Grab [GitHub Desktop](https://desktop.github.com) or your preferred git tool
  > * **Linux** - Run `sudo apt install git` in a Terminal.
  > * **FreeBSD** - Run `pkg install git` in a Terminal. Alternatively, build `git` via ports if desired.
</details>

---

## Step 2: Clone the UOX3 Git Repository
*Next up, clone the UOX3 git repository, which includes the supported SpiderMonkey headers and precompiled libraries, along with zlib source.*
<details>
  <summary>Using <strong>GitHub Desktop</strong> (Windows/macOS)</summary>

  > 1. Run GitHub Desktop and click **File->Clone Repository** from the menu.
  > 2. Enter **https://github.com/UOX3DevTeam/UOX3.git** in the URL tab, and provide a local path for storing the cloned repo on your drive.
  > 3. Hit the **Clone** button to clone the stable master branch of UOX3 to the specified local path
</details>

<details>
  <summary>Using <strong>Command Line git</strong></summary>

  > 1. Run `git clone https://github.com/UOX3DevTeam/UOX3.git` in a Terminal to clone the stable master branch of UOX3 into a subdirectory of the current directory you're in.
</details>

<details>
  <summary>(Optional) Checking out Other Branches</summary>

  > If you'd rather grab another branch of the git repository, like the **develop** branch where most updates get pushed first before being merged into the master branch, you can use the following command *after* completing the previous step, and *after* navigating into the directory created for the UOX3 repository:
    `git checkout develop`

  > GitHub Desktop users can change the active branch via the *Current Branch* dropdown menu in GitHub Desktop.

</details>

---

## Step 3: Compile UOX3
*Finally, compile UOX3 using the build environment you set up in Step 1.*
<details>
  <summary><strong>Visual Studio 2022</strong> (Windows), <strong>automake.sh</strong> (Linux/FreeBSD), <strong>XCode</strong> (macOS)</summary>

  > * **Visual Studio 2022** - (Windows) Open *UOX3\make\VS2022\uox3.sln*, select *Debug*, *Release*, or the performance-optimized *ReleaseLTO* configuration and the *x64* platform, then build the *uox3* project.
  > * **automake.sh** - (Linux/macOS) From the root of the UOX3 repository, run `./automake.sh`. The bundled SpiderMonkey library for your platform is used automatically. Use optional argument `-b debug` for a debug build and/or `-o clean` for a clean build.
  > * **automake.sh** - (FreeBSD) From the root of the UOX3 repository, run `./automake.sh`. Use optional argument `-b debug` for a debug build and/or `-o clean` for a clean build.
  > * **XCode** - (macOS) Open *UOX3/make/XCode/uox3/uox3.xcworkspace*, select *Build*
</details>

<details>
  <summary><strong>MSBuild</strong> (Windows command line)</summary>

  > From a Visual Studio developer terminal, run:
  > `msbuild make\VS2022\uox3.sln /m /p:Configuration=Release /p:Platform=x64`
  >
  > To build the performance-optimized configuration, replace `Release` with `ReleaseLTO`.
</details>

<details>
  <summary><strong>CMake</strong> (Any platform, manual instructions)</summary>

  > If you don't wish to rely on the automake.sh script, but want control over the process yourself, follow these steps (same as what automake.sh does) in a Terminal. This also works on Windows/macOS as an alternative to compiling with IDEs:
  > - Navigate to root of cloned UOX3 git repository, and execute these commands:\
  > `cmake make/cmake -B ./build -DCMAKE_BUILD_TYPE=Release`\
  > `cmake --build ./build --config Release`

  > Replace "Release" with "Debug" in the above instructions to create a debug-build; delete **build** directory to do clean builds.
</details>

<details>
  <summary><strong>GCC/make/gmake</strong> (Linux/FreeBSD, manual instructions)</summary>

  > If you'd rather use GCC (v9.x or higher)/make (GNU Make 4.2.1 or higher) than CMake, you can follow these manual steps. Note that for FreeBSD, this approach requires installing **gmake** as an alternative to *make*: `pkg install gmake`

  > First, navigate to **spidermonkey** directory and run these commands:\
  > `make -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=gcc` (Linux)\
  > `ar -r libjs32.a Linux_All_DBG.OBJ/*.o` (Linux)\
  > `cp Linux_All_DBG.OBJ/jsautocfg.h ./` (Linux)\
  > `gmake -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=clang` (FreeBSD)\
  > `ar rcs libjs32.a FreeBSD_DBG.OBJ/*.o` (FreeBSD)\
  > `cp FreeBSD_DBG.OBJ/jsautocfg.h ./` (FreeBSD)

  > Next, head to the **zlib** directory:\
  > `cd ../zlib`\
  > `make distclean`\
  > `./configure`\
  > `make`

  > Finally, head to **UOX3/source** directory:\
  > `cd ../source`\
  > `make` (Linux)\
  > `gmake` (FreeBSD)
</details>

<details>
  <summary><strong>Docker/Podman</strong></summary>

  Build and tag the image from the root of the repository like: `docker buildx build --progress=plain -t uox3 .` or `podman build -t uox3 .`
</details>

<details>
  <summary>Build <strong>SpiderMonkey 115.13</strong> from source (Linux/macOS — optional)</summary>

  > Use this instead of the bundled library if you prefer to build SpiderMonkey yourself. Install Rust, Python, Clang, and libclang first, then run from the UOX3 repository:
  >
  > `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`\
  > `source "$HOME/.cargo/env"`\
  > `cd spidermonkey/uox3-mozjs-bridge`\
  > `MOZJS_FROM_SOURCE=1 cargo build --locked --release`\
  > `cd ../../`\
  > `UOX3_MOZJS_LIBRARY="$PWD/spidermonkey/uox3-mozjs-bridge/target/release/libuox3_mozjs_bridge.a" ./automake.sh`
</details>

<details>
  <summary>Build <strong>SpiderMonkey 115.13</strong> from source (Windows — optional)</summary>

  Use this instead of the ZIP included with UOX3 if you prefer to build SpiderMonkey yourself.

  1. Install [Rust](https://rustup.rs/), Python 3.11, [LLVM/Clang 14 or newer](https://releases.llvm.org/), and [MozTools 4.0](https://github.com/servo/servo-build-deps/releases/download/msvc-deps/moztools-4.0.zip). In Visual Studio Installer, also install the Windows SDK, C++ ATL, and C++ MFC components.
  2. Open a Visual Studio Developer PowerShell in `spidermonkey\uox3-mozjs-bridge`, update the paths for your installation, and build:

     ```powershell
     $env:MOZJS_FROM_SOURCE = "1"
     Remove-Item Env:MOZJS_ARCHIVE -ErrorAction SilentlyContinue
     $env:CC = "clang-cl"
     $env:CXX = "clang-cl"
     $env:LD = "lld-link"
     $env:PYTHON = "C:\Python311\python.exe"
     $env:PYTHON3 = "C:\Python311\python.exe"
     $env:MOZTOOLS_PATH = "C:\path\to\moztools-4.0"
     $env:LIBCLANG_PATH = "C:\path\to\LLVM\lib"
     cargo build --release
     ```

  3. Create the drop-in archive:

     ```powershell
     Copy-Item target\release\uox3_mozjs_bridge.lib mozjs-115.13-windows-x64-clangcl.lib
     Compress-Archive mozjs-115.13-windows-x64-clangcl.lib mozjs-115.13-windows-x64-clangcl.zip
     ```

  Replace `spidermonkey\mozjs-115.13-windows-x64-clangcl.zip` in your UOX3 checkout with the ZIP you created, then follow the normal Windows build instructions.
</details>

---
## UOX3 Compiled! Now what?
Once done compiling, you can copy the compiled UOX3 binary/executable to the directory you intend to run your UOX3 shard from, along with all the files and folders contained in the UOX3/data subdirectory. Where you'll find the compiled UOX3 binary/executable depends on your platform and build method. Examples:
  * **Windows** - (Visual Studio 2022/MSBuild) Compiled uox3.exe can be found in **UOX3/make/VS2022/x64/Release**, **ReleaseLTO**, or **Debug**, matching the selected configuration
  * **Linux/FreeBSD** - (automake.sh) Compiled uox3 binary can be found in **root UOX3 repository**
  * **macOS** - (XCode) Compiled uox3 binary can be found in **UOX3\make\XCode\Build\Products\Release**
  * **Either Platform** - (CMake, manual) Compiled uox3 binary can be found in **UOX3\make\cmake\build**

**It is recommended** to run your UOX3 shard from a separate, dedicated directory instead of the data directory in your local UOX3 git repository, to avoid potential git conflicts and accidental overwrites when pulling updates to UOX3 from GitHub in the future.

Once you have all the required files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs/index.html#configureUOX3) to finish your UOX3 setup.
