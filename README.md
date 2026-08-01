# UOX3
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![Coverity Scan Build Status](https://scan.coverity.com/projects/23322/badge.svg)](https://scan.coverity.com/projects/ultima-offline-experiment-3)

**master** ![Windows Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(windows-latest)) ![Linux Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(ubuntu-latest)) ![macOS Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(macos-latest))

**develop** ![Windows Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(windows-latest)) ![Linux Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(ubuntu-latest)) ![macOS Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(macos-latest))

**Ultima Offline eXperiment 3** - the original open source Ultima Online server emulator, allowing people to run their own, custom UO shards since 1997. Comes with cross-platform 64-bit support for **Windows**, **Linux**, **macOS**, and **FreeBSD**. Grab the latest version from the [Releases](https://github.com/UOX3DevTeam/UOX3/releases) page, while news, more details on releases, forums, additional documentation and more can be found at https://www.uox3.org

Supported UO Client versions: **~4.0.0p** to at least **~7.0.109.0** (with encryption removed by [ClassicUO](https://www.classicuo.eu), [Razor](https://github.com/msturgill/razor/releases) or similar tools). For additional details on UO client compatibility, check https://www.uox3.org/forums/viewtopic.php?f=1&t=2289

UOX3 relies on **SpiderMonkey v1.8.5** for its JS-based scripting engine, and on **zlib-1.2.11** for data compression matters, and comes bundled with specific, compatible versions of these.

Join the [UOX3 Discord](https://discord.gg/uBAXxhF) for support and/or a quick chat!

---

# How to compile UOX3...
## Step 1: Set up a Build Environment
*First, set up a proper build environment with the various tools needed to clone and compile UOX3.*
<details>
  <summary>Install <strong>build tools</strong></summary>

  > * **Windows** - Download and install [Visual Studio 2022 Community](https://visualstudio.microsoft.com/vs/community/) on Windows 10 or newer.
  >   * In the Visual Studio Installer, select the **Desktop development with C++** workload and the **C++ Clang tools for Windows** individual component.
  >   * Windows 10 or newer provides the required `tar.exe`. CMake, Rust, and Cargo are not required for the Visual Studio 2022 build.
  > * **Linux (Debian-based)** - Run `sudo apt-get update` + `sudo apt install build-essential cmake` in a Terminal:  (or use your Linux distro's package manager)
  > * **FreeBSD** - Run `pkg install cmake` in a Terminal. Alternatively, build `cmake` via ports if desired.
  > * **macOS** - Download [Xcode](https://apps.apple.com/us/app/xcode/id497799835?mt=12) (for building with an IDE) via the App Store, and/or [CMake](https://cmake.org/download/) (for command-line builds)
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
*Next up, clone the UOX3 git repository, which also includes the latest verified compatible version of SpiderMonkey (v1.8.0-RC1) and a minimal set of files required to compile zlib-1.2.11.*
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

  > * **Visual Studio 2022** - (Windows) Open *UOX3\make\VS2022\uox3.sln*, select *Debug* or *Release* and the *x64* platform, then build the *uox3* project. The first build extracts the bundled SpiderMonkey 115.13 static library. The extracted library and all compiler output remain under the ignored *UOX3\make\VS2022\x64* directory.
  > * **automake.sh** - (Linux/FreeBSD) Run `./automake.sh` in a Terminal, from the root of the cloned UOX3 repository. This compiles UOX3 with CMake, but in one command only. Use optional argument `-b debug` to create debug build, and/or `-o clean` to do a clean build
  > * **XCode** - (macOS) Open *UOX3/make/XCode/uox3/uox3.xcworkspace*, select *Build*
</details>

<details>
  <summary><strong>MSBuild</strong> (Windows command line)</summary>

  > From a Visual Studio developer terminal, run:
  > `msbuild make\VS2022\uox3.sln /m /p:Configuration=Release /p:Platform=x64`
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
  <summary>(Troubleshooting) Adding SpiderMonkey/zlib references in Configuration Manager</summary>

  > If VS give you link errors when attempting to build UOX3, references to SpiderMonkey or zlib might have gone missing! Try the following steps to add them back.

  > 1) Right click on **UOX3_Official** in the Solution Explorer, and select Properties.
  > 2) With the desired configuration (ex: Release, x64) selected at the top of the panel, add references to SpiderMonkey and zlib in these sections:
  > * *VC++ Directories >* **Include Directories** (add path to SpiderMonkey and zlib root folders)
  > * *VC++ Directories >* **Library Directories** (add path to **spidermonkey\make\VS2017\jscript\x64\Release** folder, as well as **zlib\make\VS2017\x64\Release** folder)
  > * *VC++ Directories >* **Source Directories** (add path to SpiderMonkey and zlib root folders)
  > * *Linker >* **Additional Library Dependencies** (add path to **spidermonkey\make\VS2017\jscript\x64\Release** folder, as well as **zlib\make\VS2017\x64\Release** folder)
  > Press apply!
  > Repeat process for both Release and Debug configurations (chosen at top of panel), then retry the UOX3 build process!

</details>
</details>
<details>
  <summary><strong>Docker/Podman</strong></summary>

  Build and tag the image from the root of the repository like: `docker buildx build --progress=plain -t uox3 .` or `podman build -t uox3 .`
</details>

---
## UOX3 Compiled! Now what?
Once done compiling, you can copy the compiled UOX3 binary/executable to the directory you intend to run your UOX3 shard from, along with all the files and folders contained in the UOX3/data subdirectory. Where you'll find the compiled UOX3 binary/executable depends on your platform and build method. Examples:
  * **Windows** - (Visual Studio 2022/MSBuild) Compiled uox3.exe can be found in **UOX3/make/VS2022/x64/Release** (or **Debug** for a debug build)
  * **Linux/FreeBSD** - (automake.sh) Compiled uox3 binary can be found in **root UOX3 repository**
  * **macOS** - (XCode) Compiled uox3 binary can be found in **UOX3\make\XCode\Build\Products\Release**
  * **Either Platform** - (CMake, manual) Compiled uox3 binary can be found in **UOX3\make\cmake\build**

**It is recommended** to run your UOX3 shard from a separate, dedicated directory instead of the data directory in your local UOX3 git repository, to avoid potential git conflicts and accidental overwrites when pulling updates to UOX3 from GitHub in the future.

Once you have all the required files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs/index.html#configureUOX3) to finish your UOX3 setup.
