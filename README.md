# UOX3
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![Coverity Scan Build Status](https://scan.coverity.com/projects/23322/badge.svg)](https://scan.coverity.com/projects/ultima-offline-experiment-3)

**master** ![Windows Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(windows-latest)) ![Linux Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(ubuntu-latest)) ![macOS Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=master&job=build+(macos-latest))

**develop** ![Windows Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(windows-latest)) ![Linux Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(ubuntu-latest)) ![macOS Build](https://github.com/UOX3DevTeam/UOX3/actions/workflows/UOX3Build.yml/badge.svg?branch=develop&job=build+(macos-latest))

**Ultima Offline eXperiment 3** - the original open source Ultima Online server emulator, allowing people to run their own, custom UO shards since 1997. Comes with cross-platform 64-bit support for **Windows**, **Linux**, **macOS**, and **FreeBSD**. Grab the latest version from the [Releases](https://github.com/UOX3DevTeam/UOX3/releases) page, while news, more details on releases, forums, additional documentation and more can be found at https://www.uox3.org

Supported UO Client versions: **~4.0.0p** to at least **~7.0.116.x** (with encryption removed by [ClassicUO](https://www.classicuo.eu), [Razor](https://github.com/msturgill/razor/releases) or similar tools). For additional details on UO client compatibility, check https://www.uox3.org/forums/viewtopic.php?f=1&t=2289

UOX3 includes **SpiderMonkey 115.13** for its JavaScript scripting engine, **zlib 1.3.2** for data compression and **utf8cpp** for UTF-8 and UTF-16 string conversions.

Join the [UOX3 Discord](https://discord.gg/uBAXxhF) for support and/or a quick chat!

---

# How to compile UOX3...
## Step 1: Set up a Build Environment
*First, set up a proper build environment with the various tools needed to clone and compile UOX3.*
<details>
  <summary>Install <strong>build tools</strong></summary>

  > * **Windows** - Download and install [Visual Studio 2022 Community](https://aka.ms/vs/17/release/vs_community.exe) on Windows 10 or newer.
  >   * In the Visual Studio Installer, select the **Desktop development with C++** workload.
  >   * Under **Individual components**, select **C++ Clang Compiler for Windows** and **MSBuild support for LLVM (clang-cl) toolset**.
  > * **Linux (Debian-based)** - Run `sudo apt update && sudo apt install build-essential cmake curl` in Terminal (or use your distribution's package manager).
  > * **FreeBSD** - Run `sudo pkg install -y cmake curl gmake` in Terminal (may require installing `sudo` first, or running as `root`).
  > * **macOS** - Download [Xcode](https://apps.apple.com/us/app/xcode/id497799835?mt=12) from the App Store (for building with an IDE) and launch it once to complete setup. Alternatively, install the lightweight Xcode command-line tools via `xcode-select --install` in Terminal. After, run `brew install cmake` (requires [Homebrew](https://brew.sh/)) to install the CMake build system.
  > * **Docker/Podman** - Ensure that it is downloaded and installed (Windows) or installed from your package manager
</details>

<details>
  <summary>Install <strong>Git</strong></summary>

  > * **Windows/macOS** - Grab [GitHub Desktop](https://desktop.github.com) or your preferred git tool
  > * **Linux** - Run `sudo apt install git` in Terminal (or use your distribution's package manager).
  > * **FreeBSD** - Run `sudo pkg install git` in Terminal (may require installing `sudo` first, or running as `root`). Alternatively, build `git` via ports if desired.
</details>

---

## Step 2: Clone the UOX3 Git Repository
*Next up, clone the UOX3 git repository, which also includes headers and precompiled libraries for SpiderMonkey and a minimal set of files required to compile zlib.*
<details>
  <summary>Using <strong>GitHub Desktop</strong> (Windows/macOS)</summary>

  > 1. Run GitHub Desktop and click **File->Clone Repository** from the menu.
  > 2. Enter **https://github.com/UOX3DevTeam/UOX3.git** in the URL tab, and provide a local path for storing the cloned repo on your drive.
  > 3. Hit the **Clone** button to clone the stable master branch of UOX3 to the specified local path
</details>

<details>
  <summary>Using <strong>Command Line git</strong></summary>

  > 1. Run `git clone https://github.com/UOX3DevTeam/UOX3.git` in Terminal to clone the stable master branch of UOX3 into a subdirectory of the current directory you're in.
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
  <summary><strong>Visual Studio 2022</strong> or <strong>MSBuild</strong> (Windows), <strong>automake.sh</strong> (Linux/FreeBSD), <strong>XCode</strong> (macOS)</summary>

  > * **Visual Studio 2022** - (Windows) Open *UOX3\make\VS2022\uox3.sln*, choose *Release/Debug* from dropdown menu, and hit *Build -> Build UOX3*
  > * **automake.sh** - (Linux/macOS/FreeBSD) Run `./automake.sh` in Terminal, from the root of the cloned UOX3 repository. This compiles UOX3 and dependencies in one command using CMake. Use optional argument `-b debug` to create debug build and/or `-o clean` for a clean build
  > * **XCode** - (macOS) Open *UOX3/make/XCode/uox3/uox3.xcworkspace*, select *Build*
</details>

<details>
  <summary><strong>MSBuild</strong> (Windows command line)</summary>

  > Run the following from the root of the cloned UOX3 repository, in a Developer Powershell for VS:
  > `msbuild make\VS2022\uox3.sln /m /p:Configuration=Release /p:Platform=x64`
  >
</details>

<details>
  <summary><strong>CMake</strong> (Any platform, manual instructions)</summary>

  > If you don't wish to rely on the automake.sh script, but want control over the process yourself, follow these steps:
  >
  > Linux/macOS, using Terminal:\
  > Navigate to root of cloned UOX3 git repository, then run:\
  > `cmake make/cmake -B ./build -DCMAKE_BUILD_TYPE=Release`\
  > `cmake --build ./build --config Release`
  >
  > Windows, using Developer Powershell for VS:\
  > Navigate to root of cloned UOX3 git repository, then run:\
  > `cmake make/cmake -B ./build -G "Visual Studio 17 2022" -A x64 -T ClangCL`\
  > `cmake --build ./build --config Release`
  >
  > Replace `Release` with `Debug` in the above instructions to create a debug-build. Delete the **make/cmake/build** directory to do clean builds.
</details>

<details>
  <summary><strong>Docker/Podman</strong></summary>

  Build and tag the image from the root of the repository like: `docker buildx build --progress=plain -t uox3 .` or `podman build -t uox3 .`
</details>

<details>
  <summary>(Optional) Build <strong>SpiderMonkey 115.13</strong> from source (Windows)</summary>

  If you prefer to build SpiderMonkey yourself rather than relying on the precompiled libraries that come with UOX3, follow these steps:

  1. Install [Rust](https://rustup.rs/), [Python 3.11](https://www.python.org/downloads/windows/), [LLVM/Clang 14 or newer](https://releases.llvm.org/), and [MozTools 4.0](https://github.com/servo/servo-build-deps/releases/download/msvc-deps/moztools-4.0.zip). In Visual Studio Installer, also install the Windows SDK, C++ ATL, and C++ MFC components.
  2. Open Developer Powershell for VS and navigate to `spidermonkey\uox3-mozjs-bridge`, update the paths for your installed tools, then build:

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

  Replace `spidermonkey\mozjs-115.13-windows-x64-clangcl.zip` in your local UOX3 repository with the ZIP you created, then follow normal Windows build instructions.
</details>

<details>
  <summary>(Optional) Build <strong>SpiderMonkey 115.13</strong> from source (Linux/macOS/FreeBSD)</summary>

  If you prefer to build SpiderMonkey yourself rather than relying on the precompiled libraries that come with UOX3, follow these steps:

  1. Install dependencies:
     * **Linux / macOS**: Install Python 3.11, Clang and libclang from your preferred package manager (or via Xcode / Homebrew on macOS).
     * **FreeBSD**: Run `sudo pkg install -y rust python311 llvm18 bash pkgconf`
  2. Open Terminal and run from the UOX3 repository:
  >
  > *(One-time operation, if Rust is not already installed on Linux/macOS)*:\
  > `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`\
  > `source "$HOME/.cargo/env"`\
  > \
  > `cd spidermonkey/uox3-mozjs-bridge`\
  > `export MAKE=gmake` *(Required on FreeBSD)*\
  > `export PYTHON3=python3.11` *(Required on FreeBSD)*\
  > `export CC=clang18` *(Required on FreeBSD)*\
  > `export CXX=clang++18` *(Required on FreeBSD)*\
  > `cargo fetch` *(Required on FreeBSD)*\
  > `python3.11 fix_freebsd.py` *(Required on FreeBSD)*\
  > `MOZJS_FROM_SOURCE=1 cargo build --locked --release`\
  > `cd ../../`\
  > `export UOX3_MOZJS_LIBRARY="$PWD/spidermonkey/uox3-mozjs-bridge/target/release/libuox3_mozjs_bridge.a"`
  3. You can now use `./automake.sh` or the manual CMake instructions to build UOX3.

  > **Note for FreeBSD builders**:
  > * `MAKE=gmake` & `PYTHON3=python3.11`: Directs the build to use GNU Make and Python 3.11 explicitly.
  > * `CC=clang18` & `CXX=clang++18`: Ensures the build uses LLVM 18 to avoid breaking C++ standard library deprecations present in newer toolchains.
  > * `python3.11 fix_freebsd.py`: Automatically patches the downloaded `mozjs-sys` dependency to add missing FreeBSD platform support to `jsglue.cpp` and apply upstream C++ fixes in `ExclusiveData.h` (Mozilla Bug 1894423) and `Locale.cpp`.
</details>

---
## UOX3 Compiled! Now what?
Once done compiling, you can copy the compiled UOX3 binary/executable to the directory you intend to run your UOX3 shard from, along with all the files and folders contained in the UOX3/data subdirectory. Where you'll find the compiled UOX3 binary/executable depends on your platform and build method. Examples:
  * **Windows** - (Visual Studio 2022/MSBuild) Compiled uox3.exe can be found in **UOX3/make/VS2022/x64/Release** or **Debug**, matching the selected configuration
  * **Linux/FreeBSD** - (automake.sh) Compiled uox3 binary can be found in **root UOX3 repository**
  * **macOS** - (XCode) Compiled uox3 binary can be found in **UOX3\make\XCode\Build\Products\Release**
  * **Either Platform** - (CMake, manual) Compiled uox3 binary can be found in **UOX3\make\cmake\build**

**It is recommended** to run your UOX3 shard from a separate, dedicated directory instead of the data directory in your local UOX3 git repository, to avoid potential git conflicts and accidental overwrites when pulling updates to UOX3 from GitHub in the future.

Once you have all the required files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs/index.html#configureUOX3) to finish your UOX3 setup.
